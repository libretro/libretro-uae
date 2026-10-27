"""Check native macOS renderer centering with an original synthetic Amiga display.

Example: python3 scripts/Test-HorizontalCentering.py --core /tmp/puae_libretro.dylib
No Kickstart or game media is needed. The test injects a copper list and a CPU
loop after core initialization; it does not validate ROM boot or game behavior.
"""
import argparse
import ctypes as C
import importlib.util
import json
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--core', type=Path, required=True)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--model', choices=['A500', 'A1200'], default='A500')
    parser.add_argument('--resolution', choices=['lores', 'hires', 'superhires', 'auto-lores'], default='hires')
    parser.add_argument('--pixel', choices=['16bit', '24bit'], default='24bit')
    parser.add_argument('--crop', choices=['disabled', 'auto'], default='disabled')
    parser.add_argument('--horizontal', default='auto')
    parser.add_argument('--accurate', action='store_true')
    args = parser.parse_args()
    if sys.platform != 'darwin':
        parser.error('This diagnostic uses a native macOS dylib with engine symbols exported.')
    core = args.core.resolve(strict=True)
    out = args.output or Path(tempfile.mkdtemp(prefix='puae-centering-'))
    out = out.resolve()
    for folder in ['system', 'assets', 'saves', 'build-test-tools']:
        (out / folder).mkdir(parents=True, exist_ok=True)
    subprocess.run(['cc', '-dynamiclib', '-D__declspec(x)=',
                    str(ROOT / 'scripts/libretro-test-log.c'), '-o',
                    str(out / 'build-test-tools/libretro-test-log.dll')], check=True)
    includes = ['sources/src', 'sources/src/include', 'retrodep', 'libretro', 'libretro-common/include']
    subprocess.run(['cc', '-dynamiclib', '-w', '-std=gnu99', '-D__LIBRETRO__', '-DUSE_NAMED_SEMAPHORES',
                    *['-I' + str(ROOT / p) for p in includes],
                    str(ROOT / 'scripts/centering-fixture.c'), str(core),
                    '-o', str(out / 'fixture.dylib')], check=True)
    spec = importlib.util.spec_from_file_location('guest_host', ROOT / 'scripts/Test-GuestRoundTrip.py')
    host_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(host_module)
    host_module.ROOT = out  # Native logger location; all runtime writes stay in the test profile.
    h = host_module.Host(args.model, out, out, core, args.pixel)
    h.options.update({b'puae_video_resolution': args.resolution.encode(),
                      b'puae_crop': args.crop.encode(), b'puae_horizontal_pos': args.horizontal.encode(),
                      b'puae_crop_delay': b'enabled'})
    fixture = C.CDLL(str(out / 'fixture.dylib'))
    fixture.probe_display.argtypes = [C.c_int, C.c_int]
    fixture.probe_display.restype = None
    fixture.probe_resolution.argtypes = [C.c_int]
    fixture.probe_resolution.restype = None
    result = {'core': str(core), 'model': args.model, 'resolution': args.resolution,
              'pixel': args.pixel, 'crop': args.crop, 'horizontal': args.horizontal,
              'accurate': args.accurate, 'phases': []}

    def capture(name, shift):
        scale = C.c_ubyte.in_dll(h.lib, 'width_multiplier').value
        width, height, pitch, pixel, data = h.image
        bpp = 4 if pixel == 1 else 2
        white = b'\xff\xff\xff' if bpp == 4 else b'\xff\xff'
        spans = []
        for y in range(height):
            row = data[y * pitch:(y + 1) * pitch]
            xs = [x for x in range(width) if row[x*bpp:x*bpp+len(white)] == white]
            if xs:
                spans.append((min(xs), max(xs)))
        assert spans, 'No playfield was rendered'
        (left, right), rows = Counter(spans).most_common(1)[0]
        first = C.c_int.in_dll(h.lib, 'retro_min_diwstart').value
        border = C.c_int.in_dll(h.lib, 'visible_left_border').value
        cropx = C.c_ushort.in_dll(h.lib, 'retrox_crop').value
        phase = dict(name=name, width=width, height=height, left=left, right=right,
                     rows=rows, diw_start=first, border=border, crop_x=cropx)
        result['phases'].append(phase)
        h.capture(name)
        # Check hardware-coordinate compatibility as well as actual raster output.
        assert abs(first - (129 + shift) * scale) <= scale, phase
        if args.horizontal == 'auto':
            assert abs(left - (width - 1 - right)) <= 4 * scale, phase
            assert right - left + 1 >= min(width, 320 * scale) - 4 * scale, phase
        else:
            assert abs(left - max(0, first - border - cropx)) <= 2 * scale, phase
        return border + cropx

    h.lib.retro_init()
    assert h.lib.retro_load_game(None)
    try:
        h.run(10)
        fixture.probe_display(0, args.accurate)
        h.run(80)
        before = capture('initial', 0)
        fixture.probe_display(16, args.accurate)
        h.run(30)
        after = capture('shifted', 16)
        if args.horizontal == 'auto':
            assert after != before or args.crop == 'auto', result
        fixture.probe_display(0, args.accurate)
        h.run(30)
        capture('returned', 0)
        if args.resolution == 'auto-lores':
            fixture.probe_resolution(1)
            h.run(60)
            capture('hires', 0)
            assert C.c_ubyte.in_dll(h.lib, 'width_multiplier').value == 2
            fixture.probe_resolution(0)
            h.run(60)
            capture('lores', 0)
            assert C.c_ubyte.in_dll(h.lib, 'width_multiplier').value == 1
        result['passed'] = True
    finally:
        h.lib.retro_unload_game()
        h.lib.retro_deinit()
        (out / 'result.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
