set -euo pipefail
export PATH="$PATH:$(cygpath -u "$PUAE_GIT_DIRECTORY")"
cd "$(cygpath -u "$PUAE_WORKSPACE")"
{
    uname -a
    command -v gcc make git
    gcc --version
    gcc -dumpmachine
    make --version
    ld --version
    git --version
    pacman -Q
} > "$(cygpath -u "$PUAE_EVIDENCE_DIRECTORY")/toolchain.txt" 2>&1
test "$(gcc -dumpmachine)" = x86_64-w64-mingw32
make -j"$PUAE_BUILD_JOBS" platform=win CC=gcc OBJDIR="$PUAE_BUILD_OBJECTS" TARGET="$PUAE_BUILD_TARGET"
