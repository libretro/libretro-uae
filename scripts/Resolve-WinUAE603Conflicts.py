"""Apply reviewed PUAE integration decisions to the staged WinUAE 6.0.3 merge."""

import json
import pathlib
import re

ROOT = pathlib.Path('.git/winuae-stable-6030')
PATTERN = re.compile(r'^<<<<<<< PUAE\n(.*?)^\|\|\|\|\|\|\| WinUAE-base\n(.*?)^=======\n(.*?)^>>>>>>> WinUAE-target\n', re.M | re.S)


def resolve_file(name, resolver):
    path = ROOT / 'sources/src' / name
    text = path.read_text(encoding='utf-8', errors='surrogateescape')
    counter = 0

    def replace(match):
        nonlocal counter
        counter += 1
        return resolver(counter, *match.groups())

    path.write_text(PATTERN.sub(replace, text), encoding='utf-8', errors='surrogateescape', newline='\n')


ours = lambda i, o, b, t: o
theirs = lambda i, o, b, t: t
both = lambda i, o, b, t: o + t
for name in ['a2091.c', 'cd32_fmv.c', 'draco.c', 'x86.c']:
    resolve_file(name, ours)
resolve_file('audio.c', lambda i, o, b, t: o.replace('setirq(nr, 0)', 'setirq(nr, 1)'))
resolve_file('blkdev_cdimage.c', lambda i, o, b, t: '#ifdef WITH_MP3\n')
for name in ['cdtv.c', 'cfgfile.c', 'devices.c', 'expansion.c', 'include/newcpu.h', 'zfile.c']:
    resolve_file(name, both if name != 'include/newcpu.h' else lambda i, o, b, t: t + o)
for name in ['disk.c', 'include/debug.h', 'include/specialmonitors.h', 'specialmonitors.c', 'gfxutil.c', 'debug.c']:
    resolve_file(name, theirs)
resolve_file('include/rommgr.h', lambda i, o, b, t: o + '\n'.join(line for line in t.splitlines() if 'scan_single_rom' in line) + '\n')
resolve_file('include/sysdeps.h', lambda i, o, b, t: o + 'extern bool is_interactive_console(void);\nextern void deactivate_console(void);\nextern void set_console_input_mode(int);\n')


def cpu_region(i, o, b, t):
    if i in (1, 2):
        return '#ifdef __LIBRETRO__\n\tif (libretro_frame_end)\n\t\tset_special(SPCFLAG_BRK);\n#endif\n'
    if i == 3:
        return t + o
    if i == 4:
        return o + t
    raise ValueError(i)


resolve_file('newcpu.c', cpu_region)


def state_region(i, o, b, t):
    if i == 1:
        return '#ifdef __LIBRETRO__\n\tint comp = 0;\n\tsavestate_flags |= SAVESTATE_NODIALOGS;\n#else\n' + t + '#endif\n'
    if i == 2:
        return t + '#ifdef __LIBRETRO__\n\tf = zfile_fopen_empty(NULL, description, size);\n#else\n'
    if i == 3:
        return '#endif\n' + t
    if i == 4:
        return o
    raise ValueError(i)


resolve_file('savestate.c', state_region)

# These modules were substantially rewritten upstream. Reapply their host hooks
# against the new implementations rather than retaining obsolete renderer code.
for name in ['custom.c', 'drawing.c', 'aros.rom.c']:
    destination = ROOT / 'sources/src' / name
    destination.write_bytes(destination.with_name(destination.name + '.theirs').read_bytes())

remaining = []
for item in json.loads((ROOT / 'outcomes.json').read_text()):
    path = ROOT / item['local']
    count = path.read_bytes().count(b'<<<<<<< PUAE')
    if count:
        remaining.append({'local': item['local'], 'regions': count})
(ROOT / 'remaining-manual.json').write_text(json.dumps(remaining, indent=2) + '\n')
print(json.dumps(remaining, indent=2))
