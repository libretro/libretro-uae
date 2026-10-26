# Starting development on PUAE

The workspace is `C:\Projects\puae`. The active branch, `integrate-winuae-20261027`, contains the local WinUAE **6.0.3 stable** integration. The optimized Windows x64 candidate is `puae603_libretro.dll`, built with the installed MSYS2 UCRT64 GCC/G++ 16.2.0 toolchain. Read the [integration record](upstream/2026-10-27-winuae/INTEGRATION_STATUS.md) for the source mapping, build command, DLL hash, validation status, and remaining runtime scope.

The original 5.3.1 DLL, build logs, AROS evidence, and ROM boot references are preserved. The user's twelve verified Kickstarts are stored outside the checkout at `C:\Projects\puae-runtime\firmware\kickstarts`; [Kickstart setup and tests](KICKSTART_SMOKE_TEST.md) records the manifest and isolated launcher. The architecture map in [repository analysis](REPOSITORY_ANALYSIS.md) remains useful; its baseline tables describe the acquired source.

Use a fresh evidence directory and a separate object directory for changes to build flags:

```powershell
.\scripts\Build-WindowsX64.ps1 -Jobs 8 `
  -ObjectDirectory build-mychange -TargetName puae-mychange_libretro.dll `
  -EvidenceDirectory C:\Projects\puae\docs\builds\mychange
```

Normal builds consume the checked-in generated CPU and renderer sources. The engine uses GNU C99 plus seven C++17 keyboard MCU units, and the final link uses G++. Further work should add input/reset/save-state and representative media coverage before making compatibility or fidelity claims. Other architectures have not been built in this workspace.

## Historical baseline readiness

| Check | Result as of 27 October 2026 |
| --- | --- |
| GitHub fork | `Buyukcaglar/libretro-uae`, public, parent `libretro/libretro-uae` |
| Clone and remotes | Complete; `origin` is the fork and `upstream` is libretro |
| Source baseline | `master` at `7387937066b1b693816c3b4e2b7a0c601d41363d` |
| Branch comparison | Fork and upstream `master` and `2.6.1` matched at acquisition |
| Git integrity | `git fsck --full` passed |
| Initial worktree | Clean before these documentation files were added |
| Installed build environment | User-local MSYS2 UCRT64 at `C:\Users\Onur\AppData\Local\Programs\msys64`, GCC 16.2.0, GNU Make 4.4.1 |
| PowerShell entry point | `scripts/Build-WindowsX64.ps1`; global PATH changes are unnecessary |
| Windows x64 compilation | Passed with default release flags, 227 C units, 0 errors and 1 libmpeg2 warning |
| Static DLL inspection | PE32+ x64 DLL, all 25 libretro API exports, Windows/UCRT imports only |
| Frontend loading and bounded AROS boot | Passed in isolated RetroArch 1.22.2; 3,000 frames, clean exit, AROS waiting screen |
| Input, reset, state, and media checks | Unverified; interaction checks stopped when the user stopped Computer Use |

The original intake state is preserved in `repository-baseline.json`. Build tools and an isolated portable RetroArch were subsequently installed. The Windows release command and a bounded AROS boot were verified; external Kickstarts were supplied later, while game media remain untested. The Linux and other platform commands remain untested here.

## Establish the Windows build environment

The required POSIX-style shell, GNU Make, and Windows-targeting GCC are installed. Run `.\scripts\Build-WindowsX64.ps1 -Jobs 8` from the repository in PowerShell, or use the configured UCRT64 shell directly. The Makefile recipes use shell `if` syntax and `mkdir -p`, so the wrapper invokes Bash for the build. Core, frontend, and any CAPSIMG library must have matching architecture.

Start with the root [`Makefile`](../Makefile) and [`Makefile.common`](../Makefile.common). The instructions in `sources/README.md` describe an inherited WinUAE desktop build and do not describe this core. The baseline core has no root CMake build and needs no source generation before compilation.

In the configured build shell, verify the tools and target:

```bash
cd /c/Projects/puae
gcc --version
gcc -dumpmachine
make --version
git status --short --branch
```

Confirm the compiler targets Windows and that its architecture matches the frontend. Record the actual compiler and Make versions in the first build log. The source snapshot does not establish a minimum supported modern GCC version.

## Compile the unchanged core

A first Windows release build from a shell where `make` and `gcc` are available:

```bash
cd /c/Projects/puae
make -j8 platform=win CC=gcc
```

The expected result is `C:\Projects\puae\puae_libretro.dll`; object and dependency files go under `build/`. The root build defines `__LIBRETRO__`, enables GNU C99, compiles the bundled sources, and exports `retro_*` symbols through `libretro/link.T`. Normal builds have no dependency-download step.

To switch from release to debug, first remove the previous objects because compiler flags are not represented as separate build configurations:

```bash
make objectclean platform=win
make -j4 platform=win CC=gcc DEBUG=1
```

`objectclean` deletes `build/` beneath the checkout. `clean` deletes that directory and the selected output library. Check the working directory and preserve any needed build artifacts before using those targets. Debug objects and release objects otherwise share the same paths. Use a full rebuild when changing important flags; the dependency include typo recorded in the analysis was subsequently fixed during the stable integration.

For a separate Linux environment, the root Makefile supports:

```bash
make -j4 platform=unix CC=gcc
```

The expected output there is `puae_libretro.so`, with POSIX threading linked. Linux has not been configured or tested for this checkout.

### Build switches

| Setting | Source behavior |
| --- | --- |
| `DEBUG=1` | Selects `-O0 -g`; normal builds select `-O3` and stripped link output |
| `HAVE_CHD=0` | Overrides the root default of 1 and removes the conditional CHD/Zstandard compilation group |
| `HAVE_MPEG2=0` | Overrides the root default of 1 and removes the conditional MPEG-2 decoder group |
| `NO_7ZIP=1` | Disables the root 7-Zip feature defines; common 7-Zip sources remain listed |
| `NO_LIBRETRO_VFS=1` | Disables the `USE_LIBRETRO_VFS` define |
| `platform=...` | Selects platform flags and shared, static, or bitcode output rules |

Keep the defaults for the first baseline build. `capsimg=1` changes the CAPS compilation mode; the normal configuration uses runtime dynamic loading, so that switch is not needed merely to load an IPF through CAPSIMG. Do not enable JIT by setting one macro: its source group is disabled too, and integration would need separate work.

Android uses `jni/Android.mk` and `Application.mk`, which include the common C source list and use NDK Clang. It has its own flags and feature selection; do not assume it inherits every root Makefile default. Other platform branches require their corresponding SDKs and toolchains. Desktop compilation does not validate those targets.

## Prepare an isolated frontend baseline

Use a frontend that can load the produced libretro library. For the first Windows check, prepare a separate RetroArch profile with an explicit system directory, save directory, and disposable content directory. Keep runtime files outside the source tree, for example under `C:\Projects\puae-runtime\`. Enable frontend logging and record the frontend version, core DLL hash, core revision, selected model, and options.

Provide a matching Kickstart from your own media for the chosen Amiga model. The exact accepted filenames and MD5 values are in `README.md:69`. The core has an embedded AROS fallback, but reaching that fallback does not validate compatibility with a particular Kickstart or game. A configured frontend can load the core without content for an initial boot check.

Start with a small known diagnostic or game image and disposable copies of writable media. IPF additionally needs a matching CAPSIMG dynamic library (`capsimg.dll` on Windows), normally in the frontend system directory. The first baseline does not need IPF, WHDLoad, or every advertised media type.

WHDLoad startup can create helper files under saves and preferences under system. The core also uses `saves/TEMP` for archive extraction and cleans it during lifecycle operations. Keep these paths explicit and dedicated. Preserve the checked-in `whdload/WHDLoad.hdf` and `WHDSaves.hdf` as source assets; do not mount those checkout files as writable test disks.

### First acceptance checks

1. Build the unchanged source and retain the complete build log and output hash. Confirm the DLL exports the expected `retro_*` entry points and is loadable by the chosen frontend.
2. Load without content, then load a known A500 floppy image with the intended Kickstart. Verify visible output, input, audio, and stable frame delivery.
3. Exercise soft/hard reset and unload/reload. Check logs and files written to the dedicated runtime directories.
4. Add an M3U disk swap and one A1200/AGA case. Exercise PAL/NTSC and a geometry-changing option if the intended work touches video or model selection.
5. Before changing persistence, add WHDLoad Files mode, archive launch, a disposable save-state round trip, and rewind. Verify restored behavior and saved files after unloading; a successful API return is only one part of that check.

Source review, successful compilation, frontend loading, emulation behavior, and original-machine fidelity are separate results. Record what each check actually exercised. Add platform or media cases when the chosen change affects them.

## Start the first scoped change

After the baseline is captured, create a named topic branch for the agreed feature or fix. Refresh Git status first, preserve these notes, and keep source edits separate from vendor or generated-code updates.

The initial analysis identifies three concrete candidates: save-directory fallback and length guards, save-state timeout reporting, and the Makefile dependency include typo. Choose a focused issue and define a failing case before implementation. For frontend callback edge cases, a small libretro harness can isolate the behavior without requiring a full game session. A media or timing change still needs appropriate frontend checks afterward.

The root build currently has no integrated `test` or `check` target. Tests in `libretro-common/`, `deps/zstd/`, and inherited CPU-test sources have separate entry points and do not cover the whole core automatically. Test infrastructure should grow around the chosen changes rather than being inferred from the number of test files in the repository.

## Git workflow and continuation

The remotes are already configured:

```text
origin    https://github.com/Buyukcaglar/libretro-uae.git
upstream  https://github.com/libretro/libretro-uae.git
```

To inspect later upstream changes without modifying local work:

```powershell
Set-Location C:\Projects\puae
git status --short --branch
git fetch upstream
git log --oneline HEAD..upstream/master
git diff --stat HEAD upstream/master
```

Fetch does not merge updates. Decide how to integrate them after reviewing the current branch and local changes. Use `origin` for future project pushes. Intake and build setup left the emulator source unchanged. No generators, emulator sessions, commits, or pushes were performed.

For the next session, read `docs/REPOSITORY_ANALYSIS.md`, `docs/BUILD_WINDOWS_X64.md`, `docs/AROS_SMOKE_TEST.md`, this guide, and `docs/repository-baseline.json`, then refresh Git status. The unchanged Windows x64 build and bounded AROS boot are complete. Input/reset/state acceptance and diagnostic or game-media checks remain separate next steps.
