"""Cross-check reset/save/restore through RetroArch's documented local UDP API."""
import argparse
import hashlib
import importlib.util
import json
import re
import shutil
import socket
import struct
import subprocess
import sys
import time
from pathlib import Path

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parent.parent
RUNTIME = Path('C:/Projects/puae-runtime')
CORE_SHA = '38578cee10c58bfcb1dabdde4df8a76263c0efcb4e8cc466d0a704be438b61d2'


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--model', choices=['A500', 'A1200'], required=True)
    ap.add_argument('--run-name', required=True)
    ap.add_argument('--port', type=int, required=True)
    args = ap.parse_args()
    assert all(c.isalnum() or c in '-_' for c in args.run_name)
    profile = RUNTIME / 'profiles' / args.run_name
    evidence = ROOT / 'docs/runtime/2026-10-27-guest-roundtrip' / args.run_name
    assert not profile.exists() and not evidence.exists()
    for p in [evidence, *[profile / n for n in ['system', 'saves', 'states', 'screenshots', 'logs', 'config', 'remaps', 'playlists', 'content', 'cache', 'recordings']]]:
        p.mkdir(parents=True)
    core = ROOT / 'puae603_libretro.dll'
    assert digest(core) == CORE_SHA
    manifest = json.loads((ROOT / 'docs/runtime/kickstarts/manifest.json').read_text(encoding='utf-8-sig'))
    rom_name = 'kick34005.A500' if args.model == 'A500' else 'kick40068.A1200'
    rom = RUNTIME / 'firmware/kickstarts' / rom_name
    expected_rom = next(x['sha256'] for x in manifest['entries'] if x['file'] == rom_name)
    assert digest(rom) == expected_rom
    shutil.copyfile(rom, profile / 'system' / rom_name)
    spec = importlib.util.spec_from_file_location('probe', ROOT / 'scripts/Generate-GuestInputProbe.py')
    probe = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(probe)
    disk = profile / 'content/guest_input_probe.adf'
    fixture = probe.make_disk(disk)
    template = (ROOT / 'docs/runtime/2026-10-27-kickstart-smoke/13-release-A500-16bit/retroarch.cfg').read_text(encoding='utf-8-sig')
    template = template.replace('C:/Projects/puae-runtime/profiles/13-release-A500-16bit', profile.as_posix())
    template += f'\nnetwork_cmd_enable = "true"\nnetwork_cmd_port = "{args.port}"\nsavestate_thumbnail_enable = "false"\naudio_enable = "false"\n'
    cfg = profile / 'retroarch.cfg'
    cfg.write_text(template, encoding='utf-8')
    options = f'puae_model = "{args.model}"\npuae_kickstart = "auto"\npuae_gfx_colors = "16bit"\npuae_use_boot_hd = "disabled"\npuae_use_whdload = "disabled"\npuae_floppy_write_protection = "enabled"\n'
    (profile / 'core-options.cfg').write_text(options, encoding='utf-8')
    shutil.copyfile(cfg, evidence / cfg.name)
    shutil.copyfile(profile / 'core-options.cfg', evidence / 'core-options.cfg')
    (evidence / 'fixture.json').write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8')
    executable = RUNTIME / 'RetroArch-Win64/retroarch.exe'
    journal = []
    result = {'model': args.model, 'core_sha256': CORE_SHA, 'rom': rom_name, 'rom_sha256': expected_rom,
              'executable_sha256': digest(executable), 'profile': str(profile), 'commands_port': args.port, 'checks': {}}
    process = None
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp.bind(('127.0.0.1', 0))
    udp.settimeout(.25)
    def record(name, **data):
        item = {'event': name, 'elapsed': round(time.monotonic() - started, 3), **data}
        journal.append(item)
        (evidence / 'commands.json').write_text(json.dumps(journal, indent=2) + '\n', encoding='utf-8')
        (evidence / 'progress.json').write_text(json.dumps(item, indent=2) + '\n', encoding='utf-8')
        print(item, flush=True)
    def command(text, reply=False):
        if process.poll() is not None:
            raise RuntimeError(f'RetroArch exited with {process.returncode}')
        udp.sendto((text + '\n').encode(), ('127.0.0.1', args.port))
        response = None
        if reply:
            try:
                response = udp.recvfrom(65536)[0].decode(errors='replace').strip()
            except (TimeoutError, ConnectionResetError):
                pass  # Windows may report ICMP port-unreachable during startup.
        record('command', command=text, response=response)
        return response
    memory_command = None
    def memory():
        nonlocal memory_command
        choices = [memory_command] if memory_command else ['READ_CORE_MEMORY', 'READ_CORE_RAM']
        for kind in choices:
            response = command(f'{kind} 70000 64', True)
            if response:
                try:
                    parts = response.split()
                    raw = bytes.fromhex(' '.join(parts[2:]))
                    if len(raw) == 64:
                        memory_command = kind
                        return {'signature': raw[:4].decode(errors='replace'), 'ticks': struct.unpack_from('>I', raw, 8)[0], 'marker': struct.unpack_from('>I', raw, 28)[0],
                                'register_ticks': struct.unpack_from('>I', raw, 52)[0], 'rom_version': struct.unpack_from('>H', raw, 56)[0], 'rom_revision': struct.unpack_from('>H', raw, 58)[0]}
                except ValueError:
                    pass
        return None
    def ready(limit=20):
        deadline = time.monotonic() + limit
        while time.monotonic() < deadline:
            r = memory()
            if r and r['signature'] == 'GTI1' and r['ticks'] > 6:
                return r
            time.sleep(.25)
        raise AssertionError('Guest RAM probe did not become ready')
    def screenshot(name):
        command('SCREENSHOT')
        time.sleep(.4)
        files = list((profile / 'screenshots').glob('*.png'))
        if files:
            newest = max(files, key=lambda x: x.stat().st_mtime_ns)
            shutil.copyfile(newest, evidence / (name + '.png'))
    started = time.monotonic()
    try:
        arguments = [str(executable), '--verbose', '--config', str(cfg), '-L', str(core),
                     '--log-file=' + str(evidence / 'retroarch.log'), '--max-frames=5000', str(disk)]
        result['arguments'] = arguments
        (evidence / 'launch.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
        with (evidence / 'stdout.txt').open('wb') as out, (evidence / 'stderr.txt').open('wb') as err:
            process = subprocess.Popen(arguments, cwd=executable.parent, stdout=out, stderr=err)
            result['pid'] = process.pid
            record('launched', pid=process.pid)
            deadline = time.monotonic() + 10
            while time.monotonic() < deadline:
                if command('VERSION', True):
                    break
                time.sleep(.25)
            initial = ready()
            result['checks']['executing_rom_header'] = (initial['rom_version'], initial['rom_revision']) == ((34, 5) if args.model == 'A500' else (40, 68))
            time.sleep(1)
            before_reset = memory()
            record('before-reset', guest=before_reset)
            command('RESET')
            time.sleep(.5)
            reset = ready()
            after_reset = memory()
            result['checks']['frontend_reset'] = after_reset['signature'] == 'GTI1' and after_reset['ticks'] < before_reset['ticks']
            record('reset-complete', guest=after_reset)
            screenshot('01-after-reset')
            write = command('WRITE_CORE_MEMORY 7001c 00 00 04 8a', True)
            assert write and write.endswith(' 4'), write
            time.sleep(.1)
            lower = memory()
            command('SAVE_STATE')
            deadline = time.monotonic() + 10
            states = []
            while time.monotonic() < deadline:
                states = [p for p in (profile / 'states').rglob('*') if p.is_file() and '.state' in p.name and not p.name.endswith('.png')]
                if states:
                    time.sleep(.5)
                    break
                time.sleep(.1)
            assert states, 'No RetroArch state file created'
            saved = memory()
            result['checks']['frontend_save_state'] = True
            result['state_files'] = [{'path': str(p), 'bytes': p.stat().st_size, 'sha256': digest(p)} for p in states]
            record('saved', guest=saved, states=result['state_files'])
            write = command('WRITE_CORE_MEMORY 7001c 00 00 0a 42', True)
            assert write and write.endswith(' 4'), write
            time.sleep(1.5)
            advanced = memory()
            record('advanced-before-load', guest=advanced)
            result['checks']['frontend_state_roundtrip'] = False
            command('LOAD_STATE')
            record('load-state-sent')
            time.sleep(.15)
            restored = memory()
            record('restored', guest=restored)
            result['checks']['frontend_state_roundtrip'] = bool(restored and restored['marker'] == 0x48a and lower['ticks'] <= restored['ticks'] <= saved['ticks'] + 15 and restored['ticks'] < advanced['ticks'])
            result['checks']['frontend_cpu_register_roundtrip'] = bool(restored and restored['register_ticks'] == restored['ticks'] and restored['register_ticks'] < advanced['register_ticks'])
            screenshot('02-restored')
            time.sleep(.5)
            replay = memory()
            result['checks']['frontend_execution_after_restore'] = bool(replay and replay['ticks'] > restored['ticks'] + 10)
            record('replay', guest=replay)
            command('QUIT')
            process.wait(timeout=10)
            result['exit_code'] = process.returncode
            result['clean_exit'] = process.returncode == 0
    except BaseException as error:
        result['error'] = str(error)
        record('failure', error=str(error), process_exit=process.poll() if process else None)
    finally:
        if process and process.poll() is None:
            try:
                command('QUIT')
                process.wait(timeout=3)
            except BaseException:
                process.kill()
                process.wait(timeout=3)
                result['forced_cleanup'] = True
        if process:
            result['exit_code'] = process.returncode
        result['elapsed_seconds'] = round(time.monotonic() - started, 3)
        result['all_checks_pass'] = all(result['checks'].values()) and result.get('clean_exit', False)
        result['core_sha256_after'] = digest(core)
        result['fixture_sha256_after'] = digest(disk)
        (evidence / 'result.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
        print(json.dumps(result, indent=2), flush=True)
        udp.close()
    return 0 if result['all_checks_pass'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
