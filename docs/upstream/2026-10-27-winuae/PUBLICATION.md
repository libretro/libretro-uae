# WinUAE 6.0.3 publication record

Publication was authorized on **27 October 2026**. The target is [Buyukcaglar/libretro-uae, branch integrate-winuae-20261027](https://github.com/Buyukcaglar/libretro-uae/tree/integrate-winuae-20261027).

The source integration is commit [`8d656a8cf6addae75e79b0f41061d6d66613e5b8`](https://github.com/Buyukcaglar/libretro-uae/commit/8d656a8cf6addae75e79b0f41061d6d66613e5b8), based on PUAE commit `7387937066b1b693816c3b4e2b7a0c601d41363d` and WinUAE stable commit `857f48fcb09399d4d95527e29a2fdeaedb41f827`. It contains the 243 integration files recorded in [source-manifest.json](source-manifest.json), plus firmware/archive ignore rules. The accompanying documentation commit preserves the scripts, architecture/setup findings, build logs, source inventory, static validation, and native smoke-test captures.

## Validation provenance

All 243 source files were checked against their normalized SHA256 values before staging, then their staged Git blobs were checked against the same manifest. Publishing does not rerun or replace the recorded build. The tested optimized DLL remains SHA256 `38578cee10c58bfcb1dabdde4df8a76263c0efcb4e8cc466d0a704be438b61d2`.

The [build record](build-19-release/validation.json) and [four release smoke results](release-smoke-results.json) refer to that pre-commit binary. Its embedded Git-version suffix identifies the acquisition HEAD used during compilation; the source manifest connects its validated source snapshot to the integration commit. Future rebuilds may embed a later Git suffix and produce a different binary hash.

The tests establish Windows x64 compilation/static inspection and bounded diskless A500/A1200 boot in RGB565 and XRGB8888. Gameplay, input/reset/save-state round trips, old-state compatibility, advanced display modes, other platforms, and pixel parity remain outside that acceptance.

## Files kept out of Git

The supplied Kickstart ROMs remain under `C:\Projects\puae-runtime\firmware\kickstarts`. The original ZIP remains at `C:\Users\Onur\Downloads\KickStartRoms.zip`. The repository includes ROM filenames, hashes, configuration, logs, and screenshots; it excludes the ROM and archive bytes. The `.gitignore` rules cover the verified model filenames, common Kickstart `.rom`/`.bin` names, and the supplied archive name, including case variants and nested locations. Existing WHDLoad `.RTB` support files remain source-controlled metadata.

Staged payloads are checked against all twelve ROM SHA256 values and the original ZIP SHA256 before publication. Compiled DLLs, object files, generated tool executables, and the isolated RetroArch installation remain local. Consequently, artifact paths mentioned in historical evidence may refer to local files rather than downloadable repository files.
