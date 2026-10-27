"""Exercise the unchanged PUAE DLL through the bundled libretro C ABI.

The generated boot program, not the host, samples guest input registers. Raw
states and ROM copies stay in the external runtime profile. Evidence contains
only owned diagnostic records, screenshots, logs, hashes, and API outcomes.
"""
import argparse
import ctypes as C
import datetime
import hashlib
import importlib.util
import json
import shutil
import struct
import time
import traceback
import zlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CORE_SHA = '38578cee10c58bfcb1dabdde4df8a76263c0efcb4e8cc466d0a704be438b61d2'
MAGIC = b'GTI1'
RAM = 0x70000
ENV = C.CFUNCTYPE(C.c_bool, C.c_uint, C.c_void_p)
VIDEO = C.CFUNCTYPE(None, C.c_void_p, C.c_uint, C.c_uint, C.c_size_t)
AUDIO = C.CFUNCTYPE(None, C.c_int16, C.c_int16)
BATCH = C.CFUNCTYPE(C.c_size_t, C.c_void_p, C.c_size_t)
POLL = C.CFUNCTYPE(None)
INPUT = C.CFUNCTYPE(C.c_int16, C.c_uint, C.c_uint, C.c_uint, C.c_uint)
KEYBOARD = C.CFUNCTYPE(None, C.c_bool, C.c_uint, C.c_uint32, C.c_uint16)


class Variable(C.Structure):
    _fields_ = [('key', C.c_char_p), ('value', C.c_char_p)]


class Game(C.Structure):
    _fields_ = [('path', C.c_char_p), ('data', C.c_void_p), ('size', C.c_size_t), ('meta', C.c_char_p)]


class OptionValue(C.Structure):
    _fields_ = [('value', C.c_char_p), ('label', C.c_char_p)]


class OptionV2(C.Structure):
    _fields_ = [(n, C.c_char_p) for n in ['key', 'desc', 'desc_categorized', 'info', 'info_categorized', 'category_key']] + [('values', OptionValue * 128), ('default_value', C.c_char_p)]


class OptionsV2(C.Structure):
    _fields_ = [('categories', C.c_void_p), ('definitions', C.POINTER(OptionV2))]


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def png(path, frame):
    width, height, pitch, pixel, data = frame
    scan = bytearray()
    for y in range(height):
        scan.append(0)
        for x in range(width):
            offset = y * pitch + x * (4 if pixel == 1 else 2)
            if pixel == 1:
                b, g, r = data[offset:offset + 3]
            else:
                v = struct.unpack_from('<H', data, offset)[0]
                r, g, b = ((v >> 11) & 31) * 255 // 31, ((v >> 5) & 63) * 255 // 63, (v & 31) * 255 // 31
            scan += bytes((r, g, b))
    def chunk(tag, payload):
        return struct.pack('>I', len(payload)) + tag + payload + struct.pack('>I', zlib.crc32(tag + payload))
    Path(path).write_bytes(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0))
                           + chunk(b'IDAT', zlib.compress(scan)) + chunk(b'IEND', b''))


class Host:
    def __init__(self, model, profile, evidence, core, pixel):
        self.model, self.profile, self.evidence = model, profile, evidence
        self.core_path = core
        self.logger = C.CDLL(str(ROOT / 'build-test-tools/libretro-test-log.dll'))
        self.logger.probe_log_open.argtypes = [C.c_char_p]
        assert self.logger.probe_log_open(str(evidence / 'core.log').encode()) == 0
        self.pixel = 2
        self.frame = 0
        self.image = None
        self.keyboard = None
        self.held_keys = set()
        self.mouse = [0, 0, 0, 0]
        self.joy = set()
        self.events = []
        self.dirs = {9: str(profile / 'system').encode(), 30: str(profile / 'assets').encode(), 31: str(profile / 'saves').encode()}
        self.options = {b'puae_model': model.encode(), b'puae_kickstart': b'auto', b'puae_gfx_colors': pixel.encode(),
                        b'puae_video_standard': b'PAL', b'puae_use_boot_hd': b'disabled', b'puae_use_whdload': b'disabled',
                        b'puae_statusbar': b'disabled', b'puae_crop': b'disabled', b'puae_floppy_write_protection': b'enabled'}
        self.callbacks = [ENV(self.environment), VIDEO(self.video), AUDIO(lambda l, r: None),
                          BATCH(lambda data, frames: frames), POLL(lambda: None), INPUT(self.input)]
        self.lib = C.CDLL(str(core))
        for name, callback in zip(['environment', 'video_refresh', 'audio_sample', 'audio_sample_batch', 'input_poll', 'input_state'], self.callbacks):
            print('Registering callback:', name, flush=True)
            fn = getattr(self.lib, 'retro_set_' + name)
            fn.argtypes = [type(callback)]
            fn.restype = None
            fn(callback)
        print('Callbacks registered', flush=True)
        for name in ['retro_init', 'retro_deinit', 'retro_unload_game', 'retro_run', 'retro_reset']:
            getattr(self.lib, name).restype = None
        self.lib.retro_load_game.argtypes = [C.POINTER(Game)]
        self.lib.retro_load_game.restype = C.c_bool
        self.lib.retro_get_memory_size.argtypes = [C.c_uint]
        self.lib.retro_get_memory_size.restype = C.c_size_t
        self.lib.retro_get_memory_data.argtypes = [C.c_uint]
        self.lib.retro_get_memory_data.restype = C.c_void_p
        self.lib.retro_serialize_size.restype = C.c_size_t
        self.lib.retro_serialize.argtypes = [C.c_void_p, C.c_size_t]
        self.lib.retro_serialize.restype = C.c_bool
        self.lib.retro_unserialize.argtypes = [C.c_void_p, C.c_size_t]
        self.lib.retro_unserialize.restype = C.c_bool
        self.lib.retro_set_controller_port_device.argtypes = [C.c_uint, C.c_uint]

    def environment(self, command, data):
        cmd = command & 0xffff
        if self.frame == 0:
            print('Environment initialization:', command, flush=True)
        if cmd in self.dirs:
            C.cast(data, C.POINTER(C.c_char_p))[0] = self.dirs[cmd]
            return True
        if cmd == 27:
            C.cast(data, C.POINTER(C.c_void_p))[0] = C.cast(self.logger.probe_log, C.c_void_p).value
            return True
        if cmd == 15:
            var = C.cast(data, C.POINTER(Variable)).contents
            var.value = self.options.get(var.key)
            return var.value is not None
        if cmd == 16:
            variables = C.cast(data, C.POINTER(Variable))
            for i in range(1000):
                var = variables[i]
                if not var.key:
                    break
                self.options.setdefault(var.key, var.value.split(b'; ', 1)[-1].split(b'|')[0])
            return True
        if cmd == 70:
            var = C.cast(data, C.POINTER(Variable)).contents
            self.options[var.key] = var.value
            return True
        if cmd in [3, 17, 49]:
            C.cast(data, C.POINTER(C.c_bool))[0] = cmd == 3
            return True
        if cmd in [39, 52, 57, 59, 61]:
            C.cast(data, C.POINTER(C.c_uint))[0] = {39: 0, 52: 2, 57: 1, 59: 1, 61: 2}[cmd]
            return True
        if cmd in [67, 68]:
            if cmd == 68:
                options = C.cast(data, C.POINTER(C.POINTER(OptionsV2)))[0].contents
            else:
                options = C.cast(data, C.POINTER(OptionsV2)).contents
            for i in range(1000):
                option = options.definitions[i]
                if not option.key:
                    break
                self.options.setdefault(option.key, option.default_value or option.values[0].value)
            return True
        if cmd == 10:
            self.pixel = C.cast(data, C.POINTER(C.c_uint))[0]
            return self.pixel in [1, 2]
        if cmd == 12:
            self.keyboard = KEYBOARD(C.cast(data, C.POINTER(C.c_void_p))[0])
            return True
        if cmd == 47:
            C.cast(data, C.POINTER(C.c_uint))[0] = 3
            return True
        if cmd == 51:
            return True
        return cmd in [1, 6, 8, 11, 13, 18, 32, 35, 36, 37, 53, 55, 58, 60, 62, 63, 65, 69]

    def input(self, port, device, index, button):
        if device == 3:
            return int(button in self.held_keys)
        if device == 2 and port == 0 and button < 4:
            return self.mouse[button]
        if device == 1 and port == 0:
            if button == 256:
                return sum(1 << b for b in self.joy)
            return int(button in self.joy)
        return 0

    def video(self, data, width, height, pitch):
        if data and data != C.c_void_p(-1).value:
            self.image = (width, height, pitch, self.pixel, C.string_at(data, height * pitch))

    def run(self, count):
        for _ in range(count):
            self.lib.retro_run()
            self.frame += 1

    def record(self):
        size = self.lib.retro_get_memory_size(2)
        pointer = self.lib.retro_get_memory_data(2)
        if not pointer or size < RAM + 64:
            return {'ready': False, 'ram_bytes': size}
        raw = C.string_at(pointer + RAM, 64)
        return {'ready': raw[:4] == MAGIC, 'ram_bytes': size, 'boot_count': struct.unpack_from('>I', raw, 4)[0],
                'ticks': struct.unpack_from('>I', raw, 8)[0], 'joy0': struct.unpack_from('>H', raw, 12)[0],
                'joy1': struct.unpack_from('>H', raw, 14)[0], 'cia_buttons': raw[16], 'potgor': struct.unpack_from('>H', raw, 18)[0],
                'last_serial': raw[20], 'keyboard_count': struct.unpack_from('>I', raw, 24)[0],
                'marker': struct.unpack_from('>I', raw, 28)[0], 'serial_ring': list(raw[32:48]),
                'register_ticks': struct.unpack_from('>I', raw, 52)[0], 'rom_version': struct.unpack_from('>H', raw, 56)[0],
                'rom_revision': struct.unpack_from('>H', raw, 58)[0], 'frontend_frame': self.frame}

    def event(self, name, **extra):
        row = {'event': name, 'guest': self.record(), **extra}
        self.events.append(row)
        (self.evidence / 'trace.json').write_text(json.dumps(self.events, indent=2) + '\n', encoding='utf-8')
        (self.evidence / 'progress.json').write_text(json.dumps(row, indent=2) + '\n', encoding='utf-8')
        print(name, row['guest'], flush=True)
        return row['guest']

    def ready(self, limit=2000):
        for i in range(limit):
            self.run(1)
            r = self.record()
            if r.get('ready') and r['ticks'] >= 6:
                return i + 1
        raise AssertionError('Diagnostic boot did not become ready within frame bound')

    def marker(self, value):
        pointer = self.lib.retro_get_memory_data(2)
        C.memmove(pointer + RAM + 28, struct.pack('>I', value), 4)

    def key(self, code, down):
        (self.held_keys.add if down else self.held_keys.discard)(code)
        if self.keyboard:
            self.keyboard(down, code, code if down else 0, 0)

    def capture(self, name):
        assert self.image is not None
        path = self.evidence / (name + '.png')
        png(path, self.image)
        return {'file': path.name, 'sha256': sha(path), 'dimensions': self.image[:2]}

    def save(self, name):
        self.event(name + '-before-serialize')
        size = self.lib.retro_serialize_size()
        assert 0 < size < 128 * 1024 * 1024, size
        buf = C.create_string_buffer(size)
        ok = self.lib.retro_serialize(buf, size)
        assert ok, 'retro_serialize returned false'
        path = self.profile / 'states' / (name + '.state')
        path.write_bytes(buf.raw)
        self.event(name + '-saved', state_bytes=size, state_sha256=sha(path), state_path=str(path))
        return buf

    def restore(self, name, buf):
        self.event(name + '-before-unserialize')
        ok = self.lib.retro_unserialize(buf, len(buf))
        self.event(name + '-after-unserialize', api_return=bool(ok))
        assert ok, 'retro_unserialize returned false'
        self.run(4)
        return self.event(name + '-settled')


def main():
    sys.dont_write_bytecode = True
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--model', choices=['A500', 'A1200'], required=True)
    ap.add_argument('--run-name', required=True)
    ap.add_argument('--pixel', choices=['16bit', '24bit'], default='16bit')
    ap.add_argument('--core', choices=['updated', 'baseline'], default='updated')
    ap.add_argument('--settle-frames', type=int, default=8)
    args = ap.parse_args()
    assert all(c.isalnum() or c in '-_' for c in args.run_name)
    profile = Path('C:/Projects/puae-runtime/profiles') / args.run_name
    evidence = ROOT / 'docs/runtime/2026-10-27-guest-roundtrip' / args.run_name
    assert not profile.exists() and not evidence.exists(), 'Use a fresh run name'
    for path in [evidence, *[profile / n for n in ['system', 'assets', 'saves', 'states', 'content']]]:
        path.mkdir(parents=True)
    expected_core = CORE_SHA if args.core == 'updated' else 'bcd4827e5138a14e22a39535754aea0d3913ef0404b636d1cc666476aa3c19e8'
    core = ROOT / ('puae603_libretro.dll' if args.core == 'updated' else 'puae_libretro.dll')
    assert sha(core) == expected_core
    fw = json.loads((ROOT / 'docs/runtime/kickstarts/manifest.json').read_text(encoding='utf-8-sig'))
    filename = 'kick34005.A500' if args.model == 'A500' else 'kick40068.A1200'
    rom = Path(fw['runtime_firmware_directory']) / filename
    expected = next(x for x in fw['entries'] if x['file'] == filename)
    assert sha(rom) == expected['sha256']
    shutil.copyfile(rom, profile / 'system' / filename)
    spec = importlib.util.spec_from_file_location('probe', ROOT / 'scripts/Generate-GuestInputProbe.py')
    probe = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(probe)
    disk = profile / 'content/guest_input_probe.adf'
    fixture = probe.make_disk(disk)
    (evidence / 'fixture.json').write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8')
    checks = {}
    result = {'model': args.model, 'core_variant': args.core, 'core_sha256': expected_core, 'firmware': filename, 'firmware_sha256': expected['sha256'],
              'profile': str(profile), 'pixel_option': args.pixel, 'checks': checks, 'state_bytes_kept_outside_git': True}
    result.update(harness_sha256=sha(__file__), generator_sha256=sha(ROOT / 'scripts/Generate-GuestInputProbe.py'),
                  logger_sha256=sha(ROOT / 'build-test-tools/libretro-test-log.dll'), settle_frames=args.settle_frames)
    started = time.monotonic()
    host = None
    try:
        host = Host(args.model, profile, evidence, core, args.pixel)
        print('Calling retro_init', flush=True)
        host.lib.retro_init()
        print('Calling retro_load_game', flush=True)
        game = Game(str(disk).encode(), None, 0, None)
        assert host.lib.retro_load_game(C.byref(game)), 'Core rejected diagnostic ADF'
        host.lib.retro_set_controller_port_device(0, 1)
        host.lib.retro_set_controller_port_device(1, 1)
        boot_frames = host.ready()
        host.run(30)
        base = host.event('boot-ready', boot_frames=boot_frames, screenshot=host.capture('01-boot-ready'))
        checks['kickstart_booted_diagnostic'] = True
        checks['executing_rom_header'] = (base['rom_version'], base['rom_revision']) == ((34, 5) if args.model == 'A500' else (40, 68))
        host.mouse = [5, 3, 0, 0]
        host.run(4)
        moved = host.event('mouse-moved')
        host.mouse = [0, 0, 1, 0]
        host.run(4)
        clicked = host.event('mouse-left-held')
        host.mouse = [0, 0, 0, 1]
        host.run(4)
        right = host.event('mouse-right-held')
        host.mouse = [0, 0, 0, 0]
        host.run(4)
        released = host.event('mouse-released')
        checks['mouse_motion'] = ((moved['joy0'] & 255) - (base['joy0'] & 255)) % 256 == 20 and ((moved['joy0'] >> 8) - (base['joy0'] >> 8)) % 256 == 12
        checks['mouse_left_press_release'] = not(clicked['cia_buttons'] & 0x40) and bool(released['cia_buttons'] & 0x40)
        checks['mouse_right_press_release'] = not(right['potgor'] & 0x400) and bool(released['potgor'] & 0x400)
        for button, label in [(4, 'up'), (5, 'down'), (6, 'left'), (7, 'right'), (0, 'fire')]:
            neutral = host.record()
            host.joy = {button}
            host.run(4)
            pressed = host.event('joystick-' + label + '-held')
            host.joy = set()
            host.run(4)
            up = host.event('joystick-' + label + '-released')
            value = pressed['joy1']
            decoded = {name for name, active in [('up', ((value >> 8) ^ (value >> 9)) & 1), ('down', (value ^ (value >> 1)) & 1), ('left', (value >> 9) & 1), ('right', (value >> 1) & 1)] if active}
            checks['joystick-' + label] = ((decoded == {label} and up['joy1'] == neutral['joy1']) if label != 'fire'
                                           else (not(pressed['cia_buttons'] & 0x80) and bool(up['cia_buttons'] & 0x80)))
        host.run(30)
        k0 = host.record()['keyboard_count']
        host.key(97, True)
        host.run(8)
        host.event('keyboard-a-held')
        host.key(97, False)
        host.run(8)
        keys = host.event('keyboard-a-released')
        serial = [keys['serial_ring'][i % 16] for i in range(k0, keys['keyboard_count'])]
        checks['keyboard_a_make_break'] = serial == [0xbf, 0xbe]
        before = host.event('before-reset')
        host.lib.retro_reset()
        host.run(2)
        reset_first = host.event('reset-request-applied')
        reset_frames = host.ready()
        after = host.event('after-reset-boot', reboot_frames=reset_frames, screenshot=host.capture('02-after-reset'))
        checks['reset_reboots_same_diagnostic'] = after['ready'] and after['ticks'] < before['ticks'] and after['marker'] == 0x248
        host.marker(0x48a)
        host.run(args.settle_frames)
        saved = host.event('state-a-guest', screenshot=host.capture('03-state-a'))
        state = host.save('state-a')
        checks['state_serialization'] = True
        host.marker(0xa42)
        host.run(60)
        mutated = host.event('state-a-mutated', screenshot=host.capture('04-mutated'))
        checks['state_roundtrip_1'] = False
        restored = host.restore('roundtrip-1', state)
        checks['state_roundtrip_1'] = restored['marker'] == saved['marker'] and saved['ticks'] <= restored['ticks'] < saved['ticks'] + 15 and restored['ticks'] < mutated['ticks']
        checks['cpu_register_roundtrip_1'] = saved['register_ticks'] <= restored['register_ticks'] < saved['register_ticks'] + 15 and restored['register_ticks'] == restored['ticks']
        before_replay = restored['ticks']
        host.run(20)
        replay = host.event('roundtrip-1-replay')
        checks['execution_after_restore'] = replay['ticks'] >= before_replay + 18
        host.marker(0xabc)
        host.run(30)
        restored2 = host.restore('roundtrip-2', state)
        checks['state_roundtrip_2'] = restored2['marker'] == saved['marker'] and saved['ticks'] <= restored2['ticks'] < saved['ticks'] + 15
        checks['cpu_register_roundtrip_2'] = saved['register_ticks'] <= restored2['register_ticks'] < saved['register_ticks'] + 15 and restored2['register_ticks'] == restored2['ticks']
        host.lib.retro_reset()
        host.run(2)
        host.ready()
        host.event('fresh-reset-before-state-restore')
        restored3 = host.restore('restore-after-reset', state)
        checks['state_restore_after_reset'] = restored3['marker'] == saved['marker'] and saved['ticks'] <= restored3['ticks'] < saved['ticks'] + 15
        checks['cpu_register_restore_after_reset'] = saved['register_ticks'] <= restored3['register_ticks'] < saved['register_ticks'] + 15 and restored3['register_ticks'] == restored3['ticks']
        host.run(20)
        host.event('final', screenshot=host.capture('05-restored'))
        result['all_checks_pass'] = all(checks.values())
        host.lib.retro_unload_game()
        host.lib.retro_deinit()
        result['clean_unload'] = True
    except BaseException as error:
        result['error'] = str(error)
        result['traceback'] = traceback.format_exc()
        result['all_checks_pass'] = False
        print(result['traceback'], flush=True)
    finally:
        result['elapsed_seconds'] = round(time.monotonic() - started, 3)
        result['core_sha256_after'] = sha(core)
        result['fixture_sha256_after'] = sha(disk)
        result['timestamp_utc'] = datetime.datetime.now(datetime.UTC).isoformat()
        (evidence / 'result.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
        print(json.dumps(result, indent=2), flush=True)
    return 0 if result.get('all_checks_pass') and result.get('clean_unload') else 1


if __name__ == '__main__':
    raise SystemExit(main())
