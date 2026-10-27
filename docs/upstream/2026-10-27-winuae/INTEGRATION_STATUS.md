# WinUAE 6.0.3 stable integration

The shared WinUAE engine has been integrated locally on `integrate-winuae-20261027`. The selected target is **6.0.3 stable**, following the user's choice. Release tag `6030` resolves to `857f48fcb09399d4d95527e29a2fdeaedb41f827`, dated 3 March 2026; `od-win32/win32.h` sets `WINUAEPUBLICBETA` to 0. The target is [this upstream commit](https://github.com/tonioni/WinUAE/commit/857f48fcb09399d4d95527e29a2fdeaedb41f827).

The optimized Windows x64 build passed. Its SHA256 is `38578cee10c58bfcb1dabdde4df8a76263c0efcb4e8cc466d0a704be438b61d2`. PE inspection passed: AMD64/PE32+ DLL, exactly 25 libretro exports, and 13 Windows/UCRT imports with no compiler-runtime DLL dependency. All four optimized release smoke checks passed: A500 and A1200, each in RGB565 and XRGB8888, 1,500 frames per run with clean exits and visually confirmed Kickstart boot output. [Release results](release-smoke-results.json) record the exact ROM, DLL, and screenshot hashes. Diagnostic DLLs and process exit codes alone were not treated as acceptance.

## Source provenance and scope

- Local starting commit: `7387937066b1b693816c3b4e2b7a0c601d41363d`.
- Recorded WinUAE vendor base: tag `5310`, commit `51b7be351f359dc97f967f051da390283bc4b409` (21 October 2024).
- Added remote: `winuae`, `https://github.com/tonioni/WinUAE.git`. Existing fork `origin` and libretro `upstream` remain configured.
- Pre-integration notes/scripts were backed up to `C:\Projects\puae-integration-backup\20261027-pre-winuae`.
- This is a vendor-source integration into PUAE's C engine and host adapters. The repositories have different layouts and frontend implementations; an unrelated-history Git merge would not perform the required port.
- The stable upstream diff contains 317 records: 273 modifications, 32 additions, 11 deletions, and one rename. Strict mapping uses the `sources/src/` relocation, `.cpp` to `.c` translation, and existing header relocation. No basename guessing was used in the applied import.
- Updated 201 shared files and added 32 shared files. Together with 10 host/build/version files, the integration has 211 tracked changes and 32 new source files. [applied-paths.json](applied-paths.json) records the upstream mapping; [source-manifest.json](source-manifest.json) records normalized source hashes.
- The WinUAE desktop frontend, Windows drivers, native Direct3D backends, MSVC projects, installers, GitHub workflow, Lua integration, and other absent platform code are outside the libretro import. The 72 modified unmapped files are listed in [stable-inventory.json](stable-inventory.json).
- Legacy `adide.c`, `linetoscr.c`, and `linetoscr-be.c` remain inactive historical files. The active renderer includes the sixteen new `linetoscr_*.c` files.

The initial comparison against master `15110cf6606d1188800ea7ad86bf1cbd9c688741` (6.1.0 beta 19) is superseded investigation. It was not the selected integration target. Initial scratch files are under `.git/winuae-stable-6030/`; do not replay the scratch resolvers over the adapted live sources.

## Port adaptations

The custom-chip and drawing modules were substantially rewritten upstream. Their stable implementations were imported, then adapted to libretro's cooperative CPU loop, frame boundaries, timing ownership, video output, and status overlays. The surrounding three-way merge retained existing PUAE platform guards, VFS calls, overload renamings, and in-memory save-state interfaces while taking the shared upstream changes.

The renderer always draws internally in XRGB8888. The host copies that buffer to the frontend, converting to RGB565 when selected. Separate overlay color tables preserve the two frontend formats. Buffer dimensions, row stride, allocation bounds, and lock state are supplied by the host. WinUAE's visible/blanking limits are refreshed at lock time; otherwise their initial zero values blank every line. An unlock with no drawn lines must not publish an empty frame or end a libretro CPU slice. Display-bound coordinates are converted from the new renderer's superhires units for frontend cropping.

The seven new keyboard MCU units compile as C++17 alongside the 227 C units. Their C engine headers have explicit C linkage, and C-port declarations/default arguments are reconciled. The MCU mnemonic table is const-correct. Windows links compiler and pthread support statically so the resulting DLL requires only Windows/UCRT libraries. The core uses synchronous Denise drawing (`multithread_enabled = 0`) to retain the cooperative frontend model. Native interactive-console hooks remain unavailable through the existing libretro adapter; their new lifecycle calls are handled consistently with that adapter.

The Makefile dependency-include typo is fixed. The renderer generator now writes sixteen `.c` files in the requested working directory, and its recipe invokes it from `sources/src/`. `make gen/genlinetoscr platform=win CC=gcc` compiled the generator. A scratch run reproduced all sixteen imported files exactly after line-ending normalization; see [generator-validation.json](generator-validation.json). Normal core builds consume the checked-in generated files and do not require regeneration. Other inherited generator families were not exercised.

## Build and binary custody

Use the installed user-local MSYS2 UCRT64 toolchain: GCC/G++ 16.2.0, GNU Make 4.4.1. The wrapper sets process-local paths and preserves a fresh log directory for each build:

```powershell
Set-Location C:\Projects\puae
.\scripts\Build-WindowsX64.ps1 -Jobs 8 `
  -ObjectDirectory build-winuae603-final `
  -TargetName puae603_libretro.dll `
  -EvidenceDirectory C:\Projects\puae\docs\upstream\2026-10-27-winuae\build-repeat
```

Choose a new evidence directory each time. For a completely fresh build, also choose a new object directory; compiler flags are not encoded in the object-directory name automatically. The candidate is `C:\Projects\puae\puae603_libretro.dll`. The original 5.3.1 DLL remains at `C:\Projects\puae\puae_libretro.dll`, SHA256 `bcd4827e5138a14e22a39535754aea0d3913ef0404b636d1cc666476aa3c19e8`, with its preserved build artifact and logs.

`build-09-fresh` compiled all 234 units from a new object directory in 352.377 seconds, exit 0. Its 256 MCU const warnings were fixed and the affected units rebuilt in `build-10-final`; the baseline libmpeg2 pointer-width warning remains the only unresolved compile warning in the resulting object set. A static-link pass removed a discovered `libwinpthread-1.dll` import. Subsequent small builds repaired the runtime adapter. The temporary `-O0` renderer diagnostic and its logging were removed before `build-19-release`. That pass rebuilt the adapter and renderer with `-O3` and linked successfully in 219.677 seconds, with no diagnostics. The final DLL is 23,353,856 bytes. [validation.json](build-19-release/validation.json) confirms its PE type, exports, imports, optimization flag, and absence of temporary logging. A preserved copy is under `build-19-release/artifacts/`.

`build-01` through `build-08` preserve failed compile/link checkpoints. `compile-rest.log` gathered independent compiler diagnostics while the first renderer compile was running. `build-14-diagnostic` records a temporary diagnostic declaration error, subsequently corrected. Later numbered directories preserve successful builds and their exact binary hashes. Earlier successful DLLs are not the accepted final binary.

## ROM smoke testing

The supplied archive was extracted and verified as described in [Kickstart setup and tests](../../KICKSTART_SMOKE_TEST.md). All twelve ROMs match their documented MD5 values and remain outside the repository, under `C:\Projects\puae-runtime\firmware\kickstarts`. Each run has its own RetroArch profile and selected-ROM copy. AROS is not selected for these tests.

The original core passed A500 and A1200 reference boots (`01-baseline-A500`, `02-baseline-A1200`). Candidate runs `03` through `10` loaded the correct ROM and exited cleanly, but their black screenshots failed visual acceptance. Diagnostics established the missing lock/blanking-limit integration and empty-unlock behavior. Diagnostic runs `11` and `12` show the expected A500 Kickstart 1.3 hand/disk screen and A1200 Kickstart 3.1 ROM 40.068 purple boot screen. Accepted release runs `13-release-A500-16bit`, `14-release-A1200-16bit`, `15-release-A500-24bit`, and `16-release-A1200-24bit` repeat both models in both pixel formats against the optimized DLL hash above. Their wall times were 31.139, 31.092, 30.987, and 31.109 seconds, respectively. All four screenshots were inspected; the log and runtime DLL hashes agree. The native screenshots remain 720x287, with horizontally different artwork placement from the older reference renderer. Pixel parity is not claimed.

Example release test:

```powershell
.\scripts\Start-KickstartSmoke.ps1 -RunName release-A500-repeat `
  -CoreDll C:\Projects\puae\puae603_libretro.dll `
  -Model A500 -ColorDepth 16bit -MaxFrames 1500
```

Use `A1200` for `kick40068.A1200`, and `24bit` for XRGB8888. The launcher verifies ROM SHA256, confirms the selected Kickstart in the log, uses a core copy named by its hash, captures the final native frame, and records process exit/time. Its default process timeout is 120 seconds; increase `-TimeoutSeconds` when deliberately raising the frame bound. Screenshots require separate visual inspection.

## Acceptance limits and next work

The optimized Windows x64 build, PE/API/dependency inspection, generator reproduction, source sanity/hash checks, and four bounded external-ROM boots have passed. A source sanity check found no merge markers or unbalanced preprocessor guards in the 243 integration files. The final source manifest was rechecked against all 243 live integration files; the root DLL, preserved artifact, and four runtime copies have the same accepted SHA256. The original 5.3.1 DLL hash remains unchanged.

The subsequent [guest input/reset/state tests](../../GUEST_ROUNDTRIP_TESTS.md) used this unchanged release DLL and the user's Kickstarts. An original diagnostic floppy booted on A500 and A1200; keyboard A make/break, mouse motion/buttons, joystick directions/fire, and hard reset passed. Both models serialized states, but restore is not accepted: A500 crashed in RetroArch with `0xC0000005`, and direct API tests found restore access violations on both models. A1200 completed one stronger native frontend round trip including RAM, CPU-register, and continued-execution checks; that bounded pass does not resolve the other failures. The preserved 5.3.1 A500 control passed the same direct sequence.

Save-state restoration is a blocker for further acceptance. Compatibility with old states, representative games, writable floppy/disk-swap/hard-drive/CD media, audible audio quality, RTG, expansion hardware, MCU firmware execution, interlaced/productivity modes, advanced cropping controls, other architectures, and original-machine fidelity require separate checks. The pre-existing AROS result remains a historical baseline and does not validate this candidate's fallback.

Source integration commit: [`8d656a8cf6addae75e79b0f41061d6d66613e5b8`](https://github.com/Buyukcaglar/libretro-uae/commit/8d656a8cf6addae75e79b0f41061d6d66613e5b8). The user authorized committing and publishing the integration and documentation on 27 October 2026. The publication target is the fork branch `integrate-winuae-20261027`; see [publication record](PUBLICATION.md) for source/build provenance and firmware exclusions.
