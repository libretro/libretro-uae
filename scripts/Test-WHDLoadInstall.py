"""Exercise bundled WHDLoad installation/upgrades through the native macOS core API.

Uses an inert .slave path to trigger helper preparation; no ROM or game media
is needed and no game behavior is claimed. Logs and disposable profiles are kept
under --output. The historical-upgrade cases require the repository's history.
"""
import argparse
import ctypes as C
import hashlib
import importlib.util
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
ROOT = Path(__file__).resolve().parent.parent
LEGACY = ['d0b45aff', 'b26def5c', 'ce521086', '4806716d', '94ba8069', '22d39901', 'b777295b']

def worker(args):
    spec = importlib.util.spec_from_file_location('guest_host', ROOT / 'scripts/Test-GuestRoundTrip.py')
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    m.ROOT = args.output.parent
    h = m.Host('A1200', args.output, args.output, args.core, '24bit')
    h.options[b'puae_use_whdload'] = b'hdfs' if args.worker == 'hdf' else b'files'
    h.lib.retro_init()
    content = str(args.output / 'inert.slave').encode()
    game = m.Game(content, None, 0, None)
    assert h.lib.retro_load_game(C.byref(game))
    h.lib.retro_unload_game()
    h.lib.retro_deinit()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--core', type=Path, required=True)
    p.add_argument('--output', type=Path)
    p.add_argument('--worker')
    args = p.parse_args()
    args.core = args.core.resolve(strict=True)
    if args.worker:
        worker(args)
        return
    if sys.platform != 'darwin':
        p.error('This host currently uses a native macOS core and logger.')
    out = (args.output or Path(tempfile.mkdtemp(prefix='puae-whdload-install-'))).resolve()
    out.mkdir(parents=True, exist_ok=True)
    (out / 'build-test-tools').mkdir(exist_ok=True)
    subprocess.run(['cc', '-dynamiclib', '-D__declspec(x)=', str(ROOT / 'scripts/libretro-test-log.c'),
                    '-o', str(out / 'build-test-tools/libretro-test-log.dll')], check=True)
    release = json.loads((ROOT / 'whdload/WHDLoad.release.json').read_text())
    expected = {n: (ROOT / 'whdload/WHDLoad_files' / n).read_bytes() for n in release['assets']}
    results = []
    cases = ['fresh', *LEGACY, 'custom', 'custom-vfs', 'missing', 'backup-conflict', 'temp-conflict', 'hdf']
    for name in cases:
        profile = out / name
        profile.mkdir()
        for folder in ['system', 'saves', 'assets']:
            (profile / folder).mkdir()
        (profile / 'inert.slave').write_bytes(b'Owned helper-install diagnostic; never executed.')
        helper = profile / 'saves/WHDLoad'
        old = None
        if name not in ['fresh', 'hdf']:
            shutil.copytree(ROOT / 'whdload/WHDLoad_files', helper)
            if name in LEGACY or name in ['backup-conflict', 'temp-conflict']:
                ref = name if name in LEGACY else LEGACY[0]
                old = subprocess.check_output(['git', 'show', ref + ':whdload/WHDLoad_files/C/WHDLoad'], cwd=ROOT)
                (helper / 'C/WHDLoad').write_bytes(old)
                (helper / 'C/WHDLoad.VFS').unlink()
            elif name == 'custom':
                old = b'User-supplied or future WHDLoad executable'
                (helper / 'C/WHDLoad').write_bytes(old)
            elif name == 'custom-vfs':
                (helper / 'C/WHDLoad.VFS').write_bytes(b'User-selected archive plugin')
            elif name == 'missing':
                (helper / 'C/WHDLoad').unlink()
            if name == 'backup-conflict':
                (helper / 'C/WHDLoad.pre-20.1').write_bytes(b'Unrelated existing backup')
            if name == 'temp-conflict':
                (helper / 'C/WHDLoad.puae-20.1-new').write_bytes(b'Unrelated existing file')
        protected = {
            profile / 'saves/WHDSaves/user-save': b'Keep all save data',
            profile / 'system/WHDLoad.prefs': (ROOT / 'whdload/WHDLoad.prefs').read_bytes() + b'\n;User custom preference\n',
        }
        if helper.exists():
            protected.update({helper / 'S/WHDLoad-Startup': b'Echo "Custom startup hook"\n',
                              helper / 'S/Startup-Sequence': b';Custom boot script\n',
                              helper / 'L/WHDLoad.key': b'User key placeholder',
                              helper / 'Devs/Kickstarts/user.rom': b'User ROM placeholder'})
        for path, data in protected.items():
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        # First startup updates; second startup must be idempotent.
        hashes = None
        for run in range(2):
            command = [sys.executable, str(Path(__file__).resolve()), '--core', str(args.core),
                       '--output', str(profile), '--worker', name]
            proc = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            (profile / ('run-%d.log' % run)).write_text(proc.stdout)
            assert proc.returncode == 0, (name, proc.stdout[-2000:])
            if name == 'hdf':
                assert (profile / 'saves/WHDLoad.hdf').read_bytes() == (ROOT / 'whdload/WHDLoad.hdf').read_bytes()
            else:
                for path, data in expected.items():
                    if path == 'C/WHDLoad' and name in ['custom', 'backup-conflict', 'temp-conflict']:
                        data = old
                    elif path == 'C/WHDLoad.VFS' and name == 'custom-vfs':
                        data = b'User-selected archive plugin'
                    assert (helper / path).read_bytes() == data, (name, path)
                if name in LEGACY:
                    assert (helper / 'C/WHDLoad.pre-20.1').read_bytes() == old
            for path, data in protected.items():
                if path.name == 'WHDLoad.prefs':
                    assert b';User custom preference' in path.read_bytes()
                else:
                    assert path.read_bytes() == data, (name, path)
            current = {str(x.relative_to(profile / 'saves')): hashlib.sha256(x.read_bytes()).hexdigest()
                       for x in (profile / 'saves').rglob('*') if x.is_file()}
            if hashes is not None:
                assert hashes == current, name + ': second launch changed helper/save files'
            hashes = current
        results.append({'case': name, 'passed': True, 'idempotent': True})
    (out / 'results.json').write_text(json.dumps(results, indent=2) + '\n')
    print('WHDLoad helper installation: %d cases / %d launches passed.' % (len(results), len(results)*2))

if __name__ == '__main__':
    main()
