# A500 and A1200 guest input, reset, and save-state tests

Tested on 27 October 2026 using the unchanged WinUAE 6.0.3 integration DLL and the user's external Kickstart ROMs. **Guest keyboard, mouse, joystick, and hard reset passed on both models. Save-state restore is not accepted: A500 crashed in RetroArch, and direct API tests encountered restore access violations on both models.** A1200 completed a verified RetroArch round trip, but that pass does not resolve the other failures.

The reproducible tools, logs, guest observations, configurations, and results are saved in [the evidence directory](runtime/2026-10-27-guest-roundtrip/). [summary.json](runtime/2026-10-27-guest-roundtrip/summary.json) indexes every trial. Emulator source and both core DLLs were left unchanged. The test target was published commit `f0fbc678818ec509212a59316f31b4ed2e626df5`; these tools and results are recorded in a subsequent commit on `integrate-winuae-20261027`.

## Results

| Check | A500 / Kickstart 1.3, 34.5 | A1200 / Kickstart 3.1, 40.68 |
| --- | --- | --- |
| Boot original diagnostic floppy using external ROM | Pass | Pass |
| Guest keyboard A press and release | Pass; CIA serial bytes `BF BE` | Pass; CIA serial bytes `BF BE` |
| Mouse X/Y movement | Pass; guest counters advance 20 X / 12 Y | Pass; guest counters advance 20 X / 12 Y |
| Mouse left/right press and release | Pass | Pass |
| Joystick four directions, fire, and release | Pass | Pass |
| Hard reset through libretro API | Pass; diagnostic boots again | Pass; diagnostic boots again |
| Reset through RetroArch | Pass | Pass |
| Serialize / create state file | Pass | Pass |
| Direct API restore with stronger v2 diagnostic | Fail; null-read access violation | Fail; null-read access violation |
| RetroArch restore with stronger v2 diagnostic | Fail; process exits `0xC0000005` | Pass; RAM marker, RAM counter, CPU register counter, and continued execution verified |
| Overall save-state acceptance | **Failed** | **Withheld due to inconsistent results** |

The input checks exercise the core's libretro input interface and verify values sampled by actual guest instructions. They do not qualify physical controllers, Windows keyboard layouts, RetroArch input drivers, on-screen keyboard behavior, or every key/button. Only hard reset was tested; the Amiga soft-reset key chord was not tested.

## Test target and isolation

| Item | Identity |
| --- | --- |
| Source branch / HEAD | `integrate-winuae-20261027` / `f0fbc678818ec509212a59316f31b4ed2e626df5` |
| Updated core | `C:\Projects\puae\puae603_libretro.dll`; SHA256 `38578cee10c58bfcb1dabdde4df8a76263c0efcb4e8cc466d0a704be438b61d2` |
| Original 5.3.1 control | `C:\Projects\puae\puae_libretro.dll`; SHA256 `bcd4827e5138a14e22a39535754aea0d3913ef0404b636d1cc666476aa3c19e8` |
| RetroArch | Isolated Windows x64 1.22.2; SHA256 `81c11b6f24932bf7918f05eee8928035bff3887335fd2a081507c75e9d94d06a` |
| A500 ROM | `kick34005.A500`; SHA256 `ee05862d8102a08436ac4056da7d549db31625c7d47b24dfb7b3c9a5c113ca53` |
| A1200 ROM | `kick40068.A1200`; SHA256 `6d43840d4099a74170ea0f0425b6257c3891ebcaa39c4d1840075a9ab22b5707` |
| Current original diagnostic ADF, v2 | 901,120 bytes; SHA256 `6f99078e0ea5ff8db1d97284b57d0ccb3dfe2d38c762cd88ed46fd91ae0ffa6e` |
| Initial original diagnostic ADF, v1 | SHA256 `082b3571a9e52cb9af26ac5b7944276a851e0ba2034527b738ebf4cc6a802512` |

Each run has a fresh profile at `C:\Projects\puae-runtime\profiles\<run-name>`. A verified copy of the matching ROM, the generated ADF, and raw state files stay there, outside Git. Floppy write protection is enabled. Tests use PAL defaults and RGB565 output; the earlier diskless boot tests separately covered XRGB8888. No AROS fallback is used. Logs identify the Kickstart, and v2 guest code reads the emulated ROM's version/revision header itself.

The ROM extraction manifest remains [runtime/kickstarts/manifest.json](runtime/kickstarts/manifest.json). Final [integrity.json](runtime/2026-10-27-guest-roundtrip/integrity.json) records source, tool, DLL, ROM, archive, fixture, and state reverification. Git ignore rules also exclude binary states, firmware, DLLs, and Python caches.

## Diagnostic and observations

[Generate-GuestInputProbe.py](../scripts/Generate-GuestInputProbe.py) creates an original 68000 boot program in a DOS0 boot block with a valid carry checksum. It does not copy ROM or third-party program bytes. The guest disables normal DMA/interrupt activity, polls hardware, and writes a 64-byte diagnostic record at chip RAM `$70000`. Its color marker drives `COLOR00`, giving the screenshots a simple visible state witness.

| Record offset | Guest-written value |
| --- | --- |
| 0 | `GTI1` signature |
| 4 | Boot count, when RAM survives |
| 8 | RAM tick counter, advanced once per polled PAL frame |
| 12 / 14 | `JOY0DAT` / `JOY1DAT` |
| 16 / 18 | CIAA PRA / `POTGOR` button values |
| 20 / 24 / 32..47 | Last keyboard serial byte, event count, serial event ring |
| 28 | State/color marker: initial `$248`, saved `$48A`, then changed to `$A42` |
| 52 | Counter maintained in CPU register D2 and mirrored into RAM each frame, v2 only |
| 56 / 58 | Executing ROM version / revision, v2 only |

The host reads the record through `RETRO_MEMORY_SYSTEM_RAM`. It changes only the diagnostic's marker for the state experiment. Input registers, keyboard events, counters, and ROM header observations are written by the guest. The v2 D2 witness checks that execution resumes with restored CPU state, beyond restoring RAM alone.

[Test-GuestRoundTrip.py](../scripts/Test-GuestRoundTrip.py) loads the pinned DLL through Python ctypes, provides libretro callbacks, and uses a small native variadic logging shim. It supplies four frames of mouse delta `(5,3)`, verifies guest delta `(20,12)`, then presses/releases mouse buttons. It presses/releases each joystick direction and fire individually. Observed held `JOY1DAT` values were up `$0100`, down `$0001`, left `$FF00`, right `$00FF`; releases returned to neutral. Keyboard A produced make/break serial bytes `$BF/$BE` in the guest's ring. The keyboard protocol follows the [Commodore Amiga Hardware Reference Manual, appendix G](https://www.theflatnet.de/pub/cbm/amiga/AmigaDevDocs/hard_g.html).

After `retro_reset()`, the harness observes cleared RAM and waits for the diagnostic to boot again, with its marker reset and counters restarted. It saves a state with marker `$48A`, changes it to `$A42`, runs 60 frames, and restores. Acceptance requires the original marker, rewound RAM and D2 counters, and resumed execution. Further planned operations repeat restore and restore after another hard reset. Failures at the first restore prevent those later checks from running; they are not counted as passes.

[Test-RetroArchGuestState.py](../scripts/Test-RetroArchGuestState.py) independently runs the real RetroArch executable. It uses the [documented network control interface](https://docs.libretro.com/development/retroarch/network-control-interface/) over `127.0.0.1` for `RESET`, `SAVE_STATE`, `LOAD_STATE`, and diagnostic memory reads/writes. It verifies a disk state file, changes the marker, advances the guest, loads, checks both counters and the marker, then checks continued execution. Audio is disabled in these frontend checks. No Windows UI automation is used.

## Stronger v2 cases

| Evidence run | Result |
| --- | --- |
| [09-api-A500-early](runtime/2026-10-27-guest-roundtrip/09-api-A500-early/result.json) | All input, ROM header, hard-reset, and serialization checks pass. First `retro_unserialize()` raises a null-read access violation. Saved tick/D2 14; mutated tick/D2 74. |
| [10-api-A1200](runtime/2026-10-27-guest-roundtrip/10-api-A1200/result.json) | Same checks pass before first restore; first `retro_unserialize()` raises a null-read access violation. |
| [11-api-A500-settled](runtime/2026-10-27-guest-roundtrip/11-api-A500-settled/result.json) | Repeating with 100 settling frames before save instead of 8 still fails at first restore. |
| [12-baseline-A500-control](runtime/2026-10-27-guest-roundtrip/12-baseline-A500-control/result.json) | Original 5.3.1 DLL passes the same v2 input/reset test, two restores, CPU-register checks, restore after hard reset, continued execution, and clean unload. |
| [13-retroarch-A1200](runtime/2026-10-27-guest-roundtrip/13-retroarch-A1200/result.json) | Native frontend passes reset, save, restore, D2 check, continued execution, and clean exit 0. |
| [14-retroarch-A500](runtime/2026-10-27-guest-roundtrip/14-retroarch-A500/result.json) | Native frontend passes boot, ROM header, reset, and save. No guest readback after `LOAD_STATE`; RetroArch exits 3221225477 (`0xC0000005`, access violation). |

A1200 native run 13 observed tick/D2 **68 -> 13** across reset. The read after saving was **71**; after marker mutation and advancement it was **148**; restore returned **47** with marker `$48A`; subsequent execution advanced to **94**. RetroArch continues running between commands, so the post-save read is an upper bound on the saved counter rather than its exact value. Both restored counters agreed. Its compressed state was 41,481 bytes, SHA256 `bae5855cb9ca3741e4eb69afc2220cc51666eb7e0c41962fee737f11be80f92c`.

A500 native run 14 observed tick/D2 **65 -> 16** across reset, **74** after save, and **151** after mutation. Readbacks following load timed out and the process crashed. Its compressed state was 18,607 bytes, SHA256 `08a366fd44f10469eb9be62344ca82eda31abf763099848b4056541395d307b8`. `02-restored.png` in this failed trial is a stale copy of the previous screenshot: the screenshot helper reused the newest file when no fresh capture arrived. **It is not evidence of a successful restore.** Command logs and process/guest results determine this failure.

## Earlier trials and harness setup

All failed and successful trials were retained. The fixture and harness were strengthened during investigation, so these are separate bounded observations rather than identical repeated samples.

| Runs | Interpretation |
| --- | --- |
| `01-api-A500`, `02-api-A500` | Harness bring-up aborted during initialization before guest execution. Neither has a completed `result.json`; no input/reset/state acceptance. A native logging callback was added before run 03. |
| `03-api-A500` | v1 input/reset pass; null-read access violation on first restore. |
| `04-api-A1200` | v1 input/reset and three restore operations pass, with clean unload; v1 did not include a separate CPU-register witness. |
| `05-baseline-A500-control` | Original 5.3.1 DLL passes the v1 sequence. |
| `06-retroarch-A500` | UDP startup `WinError 10054`; harness stopped its own process. No state coverage. |
| `07-retroarch-A500` | Guest boot was observed, but pause/control polling stalled. Harness stopped its own process. No state coverage. |
| `08-retroarch-A500` | v1 native reset/save/restore and continued execution pass; clean exit. This earlier pass does not override v2 run 14's crash. |

Archived `fixture-v1-generator.py` and `api-v1-runner.py` in the evidence root document the v1 tools. They are historical source snapshots, not entry points to execute from that directory. Use the current scripts under `scripts/` for reproduction. Current v2 tools are hashed in the integrity record. Startup/control corrections in the native runner were not all snapshotted individually; each native trial preserves its own actual command journal, launch arguments, configs, and outcome.

## Restore failure and next investigation

This is a reproducible failure of the updated binary under the direct API sequence, with an independent A500 failure in the production frontend. The A500 5.3.1 control passes the same stronger direct test. That evidence supports treating save-state restoration as an integration regression requiring investigation. The A1200 direct-host failure still needs comparison with a native call stack and frontend callback behavior; no A1200 baseline control was run.

Updated restore logs report `Chunk 'BLIT' total size 63 bytes but read 4 bytes!`, followed by restore/reset messages. The same warning also appears in the passing native A1200 case, so it is a diagnostic lead, **not an established cause**. No native crash stack was obtained. No emulator fix or DLL rebuild was performed during this test task.

Start debugging with the preserved v2 A500 API reproducer and native run 14. Obtain a native access-violation stack from a separate debug build, inspect the save/restore chunk readers and frame/CPU restore boundary, then repeat both models and frontend paths. Keep the current release DLL as the failing reference. `retro_unserialize()` currently assumes success after its bounded internal loop; an API return or a "State restored" log alone must not satisfy acceptance. Cross-version state compatibility was not tested, and none of these states should be treated as portable between 5.3.1 and 6.0.3.

## Reproduce locally

Use the existing user-local MSYS2 toolchain and Python 3.14. No new package installation is required. Build only the test logging helper:

```powershell
Set-Location C:\Projects\puae
New-Item -ItemType Directory -Force -Path build-test-tools | Out-Null
& C:\Users\Onur\AppData\Local\Programs\msys64\usr\bin\bash.exe -lc 'export PATH=/ucrt64/bin:$PATH; cd /c/Projects/puae; gcc -O2 -shared -static-libgcc scripts/libretro-test-log.c -o build-test-tools/libretro-test-log.dll'
```

Each command below requires a previously unused run name. Run the processes sequentially, in separate Python processes, because a failed native restore can corrupt its host process. The updated API cases are expected to exit unsuccessfully on the preserved failing binary; inspect the emitted `result.json`, `trace.json`, and `core.log`.

```powershell
& C:\Python314\python.exe scripts\Test-GuestRoundTrip.py --model A500 --run-name repeat-api-A500
& C:\Python314\python.exe scripts\Test-GuestRoundTrip.py --model A1200 --run-name repeat-api-A1200
& C:\Python314\python.exe scripts\Test-GuestRoundTrip.py --model A500 --settle-frames 100 --run-name repeat-api-A500-settled
& C:\Python314\python.exe scripts\Test-GuestRoundTrip.py --model A500 --core baseline --run-name repeat-baseline-A500
```

Cross-check the actual frontend using two unused UDP ports:

```powershell
& C:\Python314\python.exe scripts\Test-RetroArchGuestState.py --model A1200 --run-name repeat-native-A1200 --port 55423
& C:\Python314\python.exe scripts\Test-RetroArchGuestState.py --model A500 --run-name repeat-native-A500 --port 55424
```

The native runner derives its isolation settings from the preserved release boot configuration, creates a fresh profile, and records exact arguments in `launch.json`. It operates on its own RetroArch process, requests `QUIT` afterward, and terminates only that process on timeout. Raw states remain in each external profile's `states/` directory. The runner checks the core's pinned SHA256 and verifies each ROM before use. For a future corrected core, update the pinned identity deliberately and create a new evidence series instead of replacing these records.

## Acceptance limits

This adds guest input, diagnostic floppy boot/read, and hard-reset coverage to the earlier diskless boot results. It identifies a save-state restore blocker. It does not establish gameplay compatibility, writable disk behavior, disk swap, filesystem/hard-drive/CD behavior, WHDLoad, audible audio quality, rewind, cross-session or cross-version states, RTG, advanced display modes, MCU firmware emulation, physical input device behavior, other platforms, or original-machine fidelity. Save states made here are disposable diagnostic artifacts kept outside the repository.
