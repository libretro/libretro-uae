# Horizontal centering regression after WinUAE 6 integration

Lotus 2 was reported off-center during gameplay. The regression was introduced
by the renderer replacement in `8d656a8c`.

PUAE already detects stable DIW changes in `update_audiovideo()` and calculates
both a renderer origin (`visible_left_border`) and a frontend crop offset in
`update_video_center_horizontal()`. Those calculations survived the integration.
The upstream renderer deliberately excluded its own origin selection under
`__LIBRETRO__` (see `73879370:sources/src/drawing.c`, `center_image()`, and upstream
commit `15760ce9`, “Replace internal centering with video_cb offset”). The WinUAE
6 replacement removed that exclusion and stopped applying libretro's origin to
the raster output. It therefore overwrote centering corrections every frame.

The integration also scaled the new DIW bounds without translating their origin.
They are measured from Denise's line-buffer start, while PUAE's thresholds and
game-specific corrections expect hardware horizontal-counter coordinates. In the
standard A500 high-resolution diagnostic, DIW start `$81` became 186 pixels rather
than the expected 258 pixels.

The fix restores libretro ownership of the renderer border, converts cached DIW
bounds back to hardware coordinates, and translates the chosen hardware origin
into Denise's pixel counter when opening each raster line. The existing delayed
mode-change detection, frontend crop offsets, and game-specific corrections are
retained. Both accurate drawing and cached fast lines use the resulting origin.

## Verification

The native macOS release build passes with `-O3`; existing compiler warnings remain.
A synthetic white 320×256 Amiga playfield reproduces the regression without ROM
or game media. At 720-pixel output, the unmodified build produces margins of 76
and 4 pixels. The corrected build produces 40 pixels on each side:

| Before | After |
| --- | --- |
| ![Before](runtime/2026-10-27-horizontal-centering/before.png) | ![After](runtime/2026-10-27-horizontal-centering/after.png) |

The regression diagnostic also moves the hardware display window 16 low-resolution
pixels to the right, waits for the normal delayed centering, and returns it to its
original position. It checks actual framebuffer pixels and DIW coordinates.

```sh
python3 scripts/Test-HorizontalCentering.py \
  --core /absolute/path/to/puae_libretro.dylib \
  --output /tmp/puae-horizontal-centering
```

The test compiles a separate native diagnostic helper against an exported-symbol
macOS core. That helper injects an original copper list and CPU loop into chip RAM;
it is never linked into the shipped core. The test reuses the existing libretro
callback host and accepts `--model`, `--resolution`, `--pixel`, `--crop`,
`--horizontal`, and `--accurate` to exercise different paths.

All 48 combinations of A500/A1200, low/high/super-high resolution, RGB565/XRGB8888,
disabled/automatic crop, and fast/accurate rendering pass with the diagnostic
renderer build and again with the optimized release build. Another 18 A500 cases cover manual offsets -8, 0, and +8 across
all three resolutions and both pixel formats. Sixteen additional optimized-release
cases switch the bitplane mode from low to high resolution and back using
`--resolution auto-lores`, covering both models, formats, crop modes, and render
paths. The original release fails the same diagnostic. These tests exercise rendering and mode changes. The user subsequently
reported that visual inspection of Lotus 2 centering on the deployed iPhone
build passed. Interlace and productivity modes require separate runtime checks.

## iPhone deployment

The fixed iOS arm64 core was built with `-O3`, minimum iOS 13.0, and SDK 27.0,
then signed and installed as build 2 of the existing **RetroArch PUAE** app. The
runtime log confirms **PUAE 6.0.3 a9fe5714-centering** loading
`Lotus2_v1.13_0497.lha` with the A1200 Kickstart 3.1 ROM. The game was left open
for the user to test gameplay centering; the user reported that visual inspection
passed. The app configuration and PUAE options
were preserved. Build, signing, installation, launch, binary hashes, and startup
evidence are under `artifacts/ios/horizontal-centering-20261027/`.
