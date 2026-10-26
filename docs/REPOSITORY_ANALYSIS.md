# PUAE repository analysis

PUAE is an Amiga emulator core loaded by a libretro frontend such as RetroArch. The active project builds a shared library or a platform-specific static core. Its main development seams are the frontend adapter in `libretro/`, host services in `retrodep/`, and the UAE emulation engine in `sources/src/`.

This analysis records the source baseline acquired on 26 October 2026, with findings persisted on 27 October 2026. Repository setup, static inspection, and an unchanged Windows x64 release build are complete; runtime behavior remains unverified. The [Windows build record](BUILD_WINDOWS_X64.md) contains the measured result and logs. Read [Getting started](GETTING_STARTED.md) for the first development steps. Exact acquisition metadata is in [repository-baseline.json](repository-baseline.json).

The baseline below is historical. Subsequent [AROS testing](AROS_SMOKE_TEST.md) and [Kickstart testing](KICKSTART_SMOKE_TEST.md) have been recorded separately. The active WinUAE stable update is documented in [integration status](upstream/2026-10-27-winuae/INTEGRATION_STATUS.md); use that record for current source and build status.

## Repository baseline

| Item | Value |
| --- | --- |
| Local checkout | `C:\Projects\puae` |
| Fork and origin | [Buyukcaglar/libretro-uae](https://github.com/Buyukcaglar/libretro-uae) |
| Upstream | [libretro/libretro-uae](https://github.com/libretro/libretro-uae) |
| Fork visibility | Public |
| Default and checked-out branch | `master`, tracking `origin/master` |
| Commit | `7387937066b1b693816c3b4e2b7a0c601d41363d` |
| Commit subject | Fix Allow Hz Change option change without full restart + option label corrections |
| Commit date in Istanbul | 7 October 2026 |
| Git tree | `82ac0ff914e60f53155feb5b5181cabecea78c96` |
| Other remote branch | `2.6.1`, at `151828f14d43bb2b0651327ee19a23b36a7081b2` |
| Stated WinUAE base | 5.3.1, commit `51b7be351f359dc97f967f051da390283bc4b409` |

The fork is a GitHub fork of `libretro/libretro-uae`. Both branch heads matched their upstream counterparts at acquisition. The initial worktree was clean and `git fsck --full` passed. The checkout has full history, with 1,734 commits reachable from this `master`, and no Git submodules. These are dated observations; fetch and compare again before later work.

The WinUAE base is declared by the [root README](../README.md); the core version is also defined in [`retrodep/sysconfig.h`](../retrodep/sysconfig.h). That declaration is not an independent verification of parity with WinUAE.

## Source layout

The baseline contains 2,493 tracked files totaling 111,978,684 uncompressed blob bytes, excluding Git history. File counts include vendored code, generated code, resources, and inactive backends.

| Location | Files | Responsibility |
| --- | ---: | --- |
| [`libretro/`](../libretro/) | 14 | libretro lifecycle, core options, content preparation, disk control, input mapping, overlays, and frontend callbacks |
| [`retrodep/`](../retrodep/) | 40 | Host configuration, memory allocation, file adapters, threading, sound integration, target GUI hooks, and embedded helper assets |
| [`sources/`](../sources/) | 1,025 | UAE CPU and device emulation, rendering, filesystems, archive handling, generators, inherited platform backends, and resources |
| [`libretro-common/`](../libretro-common/) | 598 | Vendored libretro portability, filesystem and VFS utilities, strings, timing, API headers, and dependency tests |
| [`deps/`](../deps/) | 726 | Bundled zlib, 7-Zip, libchdr, Zstandard, and libmpeg2 sources |
| [`whdload/`](../whdload/) | 71 | WHDLoad boot and save images, helper files, libraries, and startup scripts |
| [`deps-wiiu/`](../deps-wiiu/) and [`deps-ps3/`](../deps-ps3/) | 9 | Platform compatibility code |
| [`jni/`](../jni/) | 3 | Android NDK build files and local configuration header |
| Root files | 7 | Build definitions, user documentation, license, ignore rules, and CI configuration |

There are 825 `.c` and 118 `.cpp` files across the whole tree. The active core source list in [`Makefile.common`](../Makefile.common) is C. The presence of inherited C++, GUI, JIT, and platform source files does not mean those components are enabled or built.

## Architecture

### Frontend integration

[`libretro/libretro-core.c`](../libretro/libretro-core.c) is the central integration module, approximately 9,000 lines long. It combines several responsibilities:

- libretro entry points and frontend capability negotiation;
- core option definitions, visibility, changes, and reset requirements;
- model presets, Kickstart selection, content inspection, and UAE configuration assembly;
- archive extraction, WHDLoad helper setup, and disk control callbacks;
- video geometry and refresh changes, audio delivery, resets, and save states.

`retro_set_environment()` registers options and controllers, obtains frontend directories, advertises support for running without content, and negotiates VFS version 2 when enabled. `retro_init()` sets up logging, disk control, keyboard callbacks, buffers, and initial options. See `libretro-core.c:700`, `:3250`, and `:5072`.

`retro_get_system_info()` identifies the core as `PUAE`, requires content paths, and asks the frontend to leave archives to the core. The advertised extensions are `adf|adz|dms|fdi|raw|ipf|hdf|hdz|lha|slave|info|cue|ccd|nrg|mds|iso|chd|uae|m3u|zip|7z|/`. Format advertisement alone does not establish that all runtime dependencies are present. See `libretro-core.c:5206`.

### Loading and configuration

`retro_load_game()` negotiates RGB565 or XRGB8888 output, calls `retro_create_config()`, initializes UAE through `umain()`, and performs the first `m68k_go(1, 0)` pass. It then prepares redirected floppy handling, estimates save-state storage, and exports memory descriptors. See `libretro-core.c:8688` and [`sources/src/main.c:1360`](../sources/src/main.c#L1360).

`retro_create_config()` selects a model preset, examines content, prepares archive or WHDLoad mounts, and assembles UAE configuration. `sources/src/cfgfile.c`, `rommgr.c`, and `main.c` consume the resulting configuration. Configuration overrides can come from a loaded `.uae`, model and global files, and a content-specific file in the save directory. The root README documents the supported override names; the final configuration is available through debug-level logging. See `libretro-core.c:6362` and `README.md:423`.

Core options and UAE preferences are related state. Reset paths can rebuild configuration and reread options, so a change to an option must be checked both during initial loading and when changing it in an active session.

### Frame execution and output

The frontend drives the main emulation loop one frame at a time:

```text
Frontend calls retro_run
  -> apply changed options and poll input
  -> handle pending UAE restart
  -> m68k_go(1, 1)
       -> CPU execution and device events
       -> return at libretro_frame_end
  -> update audio and video configuration
  -> draw virtual keyboard and statusbar
  -> video callback and audio upload
```

This is a cooperative frame boundary in the engine, rather than a standalone desktop event loop. UAE worker-thread support also exists for configured subsystems; the frame boundary does not imply that every subsystem is single-threaded. See `libretro-core.c:8588`, `sources/src/newcpu.c:6608` and `:6880`, and `retrodep/threaddep/`.

[`libretro/libretro-glue.c`](../libretro/libretro-glue.c) provides target drawing hooks, host input translation, statusbar support, filesystem and archive helpers, CD/CHD integration, and target stubs. [`libretro-mapper.c`](../libretro/libretro-mapper.c) polls and maps keyboard, mouse, joystick, and controller actions. [`libretro-vkbd.c`](../libretro/libretro-vkbd.c) implements the virtual keyboard; [`libretro-graph.c`](../libretro/libretro-graph.c) supplies software drawing utilities.

Video is delivered from a software framebuffer. The sound adapter queues signed 16-bit samples through `retro_audio_queue()`, and `retro_run()` uploads the accumulated output. See `retrodep/sounddep/sound.h:26` and `libretro-core.c:5324`.

### Emulation engine

| Area | Main source files |
| --- | --- |
| 680x0 CPU and timing | `sources/src/newcpu.c`, `newcpu_common.c`, `cpuemu_*.c`, `cpustbl.c`, `events.c` |
| Memory and expansion | `memory.c`, `autoconf.c`, `expansion.c`, `cpuboard.c` |
| Custom chips and display | `custom.c`, `drawing.c`, `blitter.c`, `blitfunc.c`, `linetoscr*.c` |
| Audio and timers | `audio.c`, `cia.c`, plus `retrodep/sounddep/` |
| Floppy and disk formats | `disk.c`, `diskutil.c`, `fdi2raw.c`, `caps/` |
| Hard disks and filesystems | `hardfile.c`, `hardfile_unix.c`, `filesys.c`, `filesys_unix.c`, `fsdb*.c`, `ide*.c`, `scsi*.c` |
| CD systems | `akiko.c`, `cdtv.c`, `blkdev*.c`, `cdrom.c`, `cd32_fmv*.c` |
| Configuration, ROMs, and state | `cfgfile.c`, `rommgr.c`, `savestate.c`, `zfile.c`, `zfile_archive.c` |

Paths in the table are relative to `sources/src/` unless stated otherwise. [`retrodep/sysconfig.h`](../retrodep/sysconfig.h) enables multiple interpreter models, AGA/ECS, CD32/CDTV, FPU/MMU, filesystems, save states, and threading. JIT is commented out there, and its source additions are commented out in `Makefile.common:153`. A JIT implementation exists in the tree but is outside the default build. Other inherited features, including Picasso96 and several networking and accelerator options, are also disabled by this configuration.

### Media and persistence

[`libretro/libretro-dc.c`](../libretro/libretro-dc.c) manages disk lists, M3U parsing, labels, replacement, and save disks. The disk list capacity is 20 entries (`libretro-dc.h:31`). ZIP and 7z content is inspected after extraction into a `TEMP` directory below the frontend save directory. WHDLoad support can materialize helper directories or HDF images from C arrays included by `libretro-core.c`.

State serialization uses UAE's in-memory `zfile` mechanism. The frontend receives an estimated buffer size based on configured RAM plus device/filesystem headroom. Restoration advances emulation internally until the state operation completes or a 50-frame limit is reached. See `libretro-core.c:8747`, `:8829`, and `:8862`.

Reset, unload, and deinitialization have persistence responsibilities. They can send the WHDLoad quit key, flush writes, compress save disks, close redirected media, and remove temporary archive contents. Test these paths with disposable copies and explicit system/save directories. See `libretro-core.c:7426`, `:8782`, and `:5128`.

## Build structure

[`Makefile`](../Makefile) selects the platform, output format, compiler flags, feature switches, build rules, and generators. [`Makefile.common`](../Makefile.common) selects include directories and compilation units. A normal desktop build compiles committed C sources into `build/` and links `puae_libretro.dll`, `.so`, or `.dylib` at the root. There is no root CMake project or active WinUAE Visual Studio solution in this checkout.

The default compiler mode is GNU C99. Release builds use `-O3` and strip link output; `DEBUG=1` selects `-O0 -g`. zlib, 7-Zip support, CHD/Zstandard, MPEG-2, and libretro VFS are enabled by the root build defaults. Their implementations are bundled. IPF normally uses an additional CAPSIMG dynamic library at runtime. See `Makefile:294` through `:366`, `Makefile.common`, and `sources/src/caps/caps.c:25`.

The repository also contains Android NDK files and Makefile branches for Unix, macOS, Apple mobile platforms, ARM systems, and several consoles. Platform definitions and CI configuration show intended targets; they are not evidence that those targets currently build. All 227 distinct source paths explicitly listed in uncommented sections of `Makefile.common` exist, including conditional groups; this path check does not evaluate Make or compile those files. The [build guide](GETTING_STARTED.md) contains commands and qualifications for starting on this Windows host.

### Generated code and embedded resources

CPU dispatch tables, CPU interpreter files, blitter routines, and line conversion files are already committed. `make gen` builds generators and overwrites tracked files in `sources/src/` and its `include/` directory. It is not a prerequisite for an ordinary baseline build. Keep generator changes and their generated output together, and review the resulting diff.

`retrodep/WHDLoad_cmaker.sh` rebuilds embedded WHDLoad arrays using `zip`, `gzip`, and `xxd`. `retrodep/driveclick_cmaker.sh` rebuilds embedded sound samples. Both scripts use relative paths expecting the working directory to be `retrodep/`. Their outputs are source assets; do not regenerate them as incidental setup work.

## Validation and maintenance findings

These are source observations and follow-up candidates. No implementation was changed during intake.

| Finding | Source evidence | Development consequence |
| --- | --- | --- |
| Save-directory fallback and indexing need review | `libretro-core.c:268` declares a fixed array; `:726` checks `!retro_save_directory`; `:735` and `:740` index at `strlen(...) - 1` and `- 2` | The array-address check cannot detect an empty directory string. Missing or very short frontend directory values can reach invalid indexing. Reproduce with a minimal frontend callback harness and fix the fallback and length guards together. |
| Save-state restore can report success after its timeout | `libretro-core.c:8914` limits restoration to 50 frames; `:8924` through `:8929` explicitly assume success after the loop | A `true` result alone cannot prove the restore completed. Check pending state, clean up timeout behavior, and test failed as well as successful restoration before relying on rewind or state robustness. |
| Dependency include line has an extra closing parenthesis | `Makefile:352` is `-include $(OBJECTS:.o=.d))` | Static reading indicates a trailing `)` is attached to the final dependency filename. GNU Make validation should confirm whether the final unit misses incremental header dependency updates. |
| Core integration tests are not wired into the root build | Root Makefile has no `test` or `check` target; the checked-in CI files build platform artifacts | Dependency suites, CPU-test code, and inherited `test_optflag` sources do not establish core regression coverage. Add narrow tests around the first chosen change and keep frontend smoke tests separate. |
| Documentation contains inherited build instructions | `sources/README.md` describes WinUAE Visual Studio projects absent from the active core | Use the root Makefiles and Android files as build authority. Avoid installing the inherited WinUAE desktop prerequisites for this core. |
| Configuration and persistence are concentrated in a large adapter | `libretro-core.c` owns loading, options, resets, WHDLoad, and state operations | Begin with a small behavior change and explicit acceptance cases; broad refactoring would obscure the baseline. |

Recent upstream history includes refresh-rate option changes, MultiDrive changes, macOS disk reinsertion, and WHDLoad save-state/rewind fixes. Those are useful regression scenarios when touching configuration or media; their commit subjects are not independent runtime validation. See commits `73879370`, `2ccb4819`, `5ed69b56`, and `bc0c2b0e`.

### Dependencies and CI

All main dependency trees are vendored rather than submodules. The bundled headers identify zlib as 1.2.7 and Zstandard as 1.5.5. Track dependency updates as explicit changes with their own build and content checks; a version string alone is not a vulnerability assessment. The latest commit touching `libretro-common/` in this history is `6de65d7d`, dated 31 August 2026; the intake did not identify a separate upstream vendor revision.

`.gitlab-ci.yml` delegates Windows, Linux, macOS, Android, Apple mobile, and selected console builds to external `libretro-infrastructure/ci-templates`. `.travis.yml` contains an older Ubuntu Trusty/GCC 7 setup that clones `libretro-super`. There is no repository-level GitHub Actions workflow in the baseline; workflows under `deps/zstd/` belong to that vendored project. No CI service result was checked during this intake.

The root `COPYING` contains GPL version 2. Dependencies carry additional notices, including libchdr's BSD license. Preserve existing license and resource notices with any future packaging work.

## Where to start a change

| Intended work | Read first | First meaningful checks |
| --- | --- | --- |
| Core option or model behavior | `libretro-core.c` option definitions, `update_variables`, preset/config builders, reset paths; `cfgfile.c` | Fresh load, live option change, reset, unload and reload |
| Input or virtual keyboard | `libretro-mapper.c`, `libretro-vkbd.c`, glue input functions | Keyboard and controller mappings, sticky keys, release events, mouse/joystick switching |
| Disk or archive handling | `libretro-dc.c`, core content preparation, glue extractors, `disk.c` | Single image, M3U swaps, archive launch, save redirect, cleanup |
| WHDLoad or saves | Core helper/mount setup, `whdload/`, `filesys.c`, unload and quit-key paths | Files and HDF modes, changed files after unload, archive relaunch, disposable save-state/rewind cases |
| Display or audio | Core AV updates, glue drawing hooks, `drawing.c`, `custom.c`, `audio.c`, sound adapter | PAL/NTSC, geometry changes, output formats, audio continuity, reset |
| CPU or device accuracy | Relevant engine module, generated-code provenance, `sysconfig.h` | Targeted diagnostic input and a defined reference; compilation alone is insufficient |

The unchanged Windows x64 core now builds with GCC 16.2.0 and GNU Make 4.4.1 using the default release flags. Static inspection confirms the expected x64 DLL and libretro exports; see the [build record](BUILD_WINDOWS_X64.md). The next milestone is a repeatable frontend smoke test, followed by one scoped change with explicit acceptance cases. A standalone application, a new platform port, or new emulated hardware would require additional design beyond this intake.
