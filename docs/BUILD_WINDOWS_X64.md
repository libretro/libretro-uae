# Windows x64 baseline build (PUAE 5.3.1)

The unchanged Windows x64 release core compiled successfully on 27 October 2026. The output is `C:\Projects\puae\puae_libretro.dll`, built from commit `7387937066b1b693816c3b4e2b7a0c601d41363d`. The Windows build environment is installed at `C:\Users\Onur\AppData\Local\Programs\msys64`. Use the PowerShell wrapper below to rebuild through its UCRT64 environment without adding GCC or MSYS2 utilities to the global Windows PATH.

The [build evidence directory](builds/2026-10-27-windows-x64/) contains the installation receipt, package update logs, compiler output, package versions, and build result. The original [repository intake manifest](repository-baseline.json) remains a historical record of the earlier setup state.

For the current WinUAE 6.0.3 source update and its separate candidate DLL, use the [integration build record](upstream/2026-10-27-winuae/INTEGRATION_STATUS.md). Commands run against the current checkout build its current source. The candidate build instructions select a separate target and object directory to preserve the baseline below.

## Build result

| Check | Result |
| --- | --- |
| Build command | `make -j8 platform=win CC=gcc` |
| Start and finish in Istanbul | 27 October 2026, 00:09:24 to 00:11:46 |
| Build duration | 142.556 seconds |
| Make exit code | 0 |
| C compiler invocations and object files | 227 each |
| Compiler diagnostics | 0 errors, 1 warning |
| DLL size | 17,737,728 bytes |
| DLL SHA256 | `bcd4827e5138a14e22a39535754aea0d3913ef0404b636d1cc666476aa3c19e8` |
| PE type | PE32+ DLL, AMD64 machine `0x8664` |
| Exports | All 25 API entry points declared by the bundled `libretro.h`; no missing or additional exports |
| Static imports | `KERNEL32.dll` and 11 Windows UCRT API-set DLLs |
| Toolchain runtime imports | No MSYS, Cygwin, libgcc, libstdc++, or libwinpthread DLL imports |
| Source preservation | Git HEAD/tree unchanged; all tracked files match HEAD before and after compilation |
| Runtime execution | Not performed |

The single compiler warning is `deps/libmpeg2/src/alloc.c:48:26: warning: cast from pointer to integer of different size [-Wpointer-to-int-cast]`, for `align_buf -= (long)align_buf & 63;`. It did not prevent compilation or linking. The source was retained unchanged; this result does not establish the runtime correctness of that code.

The measured build result is in [build-result.json](builds/2026-10-27-windows-x64/build-result.json). [validation.json](builds/2026-10-27-windows-x64/validation.json) records the PE header, API export comparison, imports, diagnostics, hash, and source-preservation checks. Raw inspection output is in `dll-file-header.txt` and `dll-pe-details.txt`; the full compiler and linker commands are in [build.log](builds/2026-10-27-windows-x64/build.log).

The first DLL is also preserved at [artifacts/puae_libretro.dll](builds/2026-10-27-windows-x64/artifacts/puae_libretro.dll) beneath the evidence directory, so a later build can replace the root DLL without losing this baseline. Both DLL paths are ignored by the repository's existing `*.dll` rule. Documentation, scripts, logs, and binaries remain local; no commit or push was made.

## Installed environment

| Item | Installed value |
| --- | --- |
| Setup date | 27 October 2026, Europe/Istanbul |
| MSYS2 base release | `2026-09-27` |
| Environment | UCRT64, native Windows x64 target |
| GCC | `16.2.0`, MSYS2 Rev6 |
| GNU Make | `4.4.1` |
| GNU ld | `2.47.20260726` |
| GCC target | `x86_64-w64-mingw32` |
| Explicitly installed packages | `make`, `mingw-w64-ucrt-x86_64-gcc` |
| Git used by the build | Existing Git `2.53.0.windows.3` |
| System integration | User-local archive installation; no persistent PATH changes or administrator helper required |

The compiler package installs its required runtime, headers, CRT, and binutils dependencies. Exact resolved package versions are captured by `pacman -Q` in [toolchain.txt](builds/2026-10-27-windows-x64/toolchain.txt). Future package updates can change the compiler; retain this inventory when comparing later results.

## Installation steps

The setup follows the official [MSYS2 installer](https://www.msys2.org/docs/installer/), [environment](https://www.msys2.org/docs/environments/), and [update](https://www.msys2.org/docs/updating/) guidance. The archive distribution becomes a usable MSYS2 installation after its first login shell initializes it. UCRT64 supplies a native x64 GCC alongside the MSYS shell tools.

1. Download the pinned [MSYS2 base archive](https://github.com/msys2/msys2-installer/releases/download/2026-09-27/msys2-base-x86_64-20260927.tar.xz) and its `.sha256` file to `C:\Users\Onur\AppData\Local\puae-build-tools\downloads\`.
2. Verify its SHA256 against both the published checksum and the GitHub release asset digest. The verified value is `ea2f31a0b6ade63914ce441ffb022f0f6aa96982bfefa2326460a26d5fb01322`. The archive is 42,860,696 bytes.
3. Inspect the archive paths and extract its `msys64/` directory beneath `C:\Users\Onur\AppData\Local\Programs\`. This was a new installation, with no existing target directory overwritten.
4. Initialize a UCRT64 login shell and run a full package update. The first update replaced the shell, runtime, and package manager, then closed the MSYS2 processes as expected. Start a fresh shell and run the update again.
5. Install GNU Make and the UCRT64 GCC package, retaining normal package signature verification.

The extraction command, after checksum and path verification, was:

```powershell
tar.exe -xf "$env:LOCALAPPDATA\puae-build-tools\downloads\msys2-base-x86_64-20260927.tar.xz" -C "$env:LOCALAPPDATA\Programs"
```

The update and installation commands were run from PowerShell:

```powershell
$env:MSYSTEM = 'UCRT64'
$env:CHERE_INVOKING = '1'
$bash = "$env:LOCALAPPDATA\Programs\msys64\usr\bin\bash.exe"
& $bash -lc 'pacman --noconfirm -Syu'
# Start a fresh shell after the core update closes the first one.
& $bash -lc 'pacman --noconfirm -Syu'
& $bash -lc 'pacman --noconfirm -S --needed make mingw-w64-ucrt-x86_64-gcc'
```

Both update invocations and package installation returned exit code 0. Their complete logs are `01-msys2-update.log`, `02-msys2-update.log`, and `03-toolchain-packages.log` in the evidence directory. [toolchain-installation.json](builds/2026-10-27-windows-x64/toolchain-installation.json) records the download URL, hashes, and install location.

## Build command

From PowerShell:

```powershell
Set-Location C:\Projects\puae
.\scripts\Build-WindowsX64.ps1 -Jobs 8
```

The wrapper creates a new timestamped evidence directory by default. Set `-MsysRoot` to use a different MSYS2 root or `-EvidenceDirectory` to choose a new log directory. It refuses to overwrite an existing `build.log`. It records the Git revision and worktree state, tool versions and package inventory, the executed Bash script, complete compiler output, exit code, duration, and output hash. It checks the compiler target before compilation.

The equivalent command inside the configured UCRT64 shell is:

```bash
cd /c/Projects/puae
make -j8 platform=win CC=gcc
```

The first run began without `build/` or `puae_libretro.dll`, so it was a full compilation. It used the original Makefile defaults: release `-O3`, GNU C99, bundled zlib and 7-Zip, CHD/Zstandard, MPEG-2, and libretro VFS. No generator, emulator source patch, Makefile patch, compatibility flag override, or feature reduction was used.

Objects and dependency files stay under `build/`; the DLL is written to `C:\Projects\puae\puae_libretro.dll`. The wrapper does not clean existing objects. Changing compiler versions or significant flags requires a clean rebuild; debug and release builds share object paths in the original Makefile. See [Getting started](GETTING_STARTED.md) for the cleanup targets and runtime test preparation.

## Validation scope

The requested milestone is compilation and static inspection of the resulting Windows DLL. Frontend loading, emulation, audio/video behavior, game compatibility, save-state behavior, and original-machine fidelity require later runtime checks. No frontend, Kickstart ROM, or game media is part of this build setup.
