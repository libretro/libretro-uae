[CmdletBinding()]
param(
    [string]$MsysRoot = (Join-Path $env:LOCALAPPDATA 'Programs\msys64'),
    [ValidateRange(1, 64)]
    [int]$Jobs = 8,
    [string]$EvidenceDirectory,
    [ValidatePattern('^[A-Za-z0-9_-]+$')]
    [string]$ObjectDirectory = 'build',
    [ValidatePattern('^[A-Za-z0-9_.-]+\.dll$')]
    [string]$TargetName = 'puae_libretro.dll'
)

$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$bash = Join-Path $MsysRoot 'usr\bin\bash.exe'
if (-not (Test-Path -LiteralPath $bash -PathType Leaf)) {
    throw "MSYS2 Bash was not found at $bash. See docs/BUILD_WINDOWS_X64.md."
}

if (-not $EvidenceDirectory) {
    $stamp = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ')
    $EvidenceDirectory = Join-Path $workspace "docs\builds\windows-x64-$stamp"
}
$EvidenceDirectory = [IO.Path]::GetFullPath($EvidenceDirectory)
New-Item -ItemType Directory -Path $EvidenceDirectory -Force | Out-Null
$buildLog = Join-Path $EvidenceDirectory 'build.log'
if (Test-Path -LiteralPath $buildLog) {
    throw "Build log already exists: $buildLog. Select a new evidence directory to preserve the prior run."
}

$git = (Get-Command git -ErrorAction Stop).Source
$head = (& $git -C $workspace rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Cannot identify repository HEAD.' }
& $git -C $workspace status --short --branch |
    Set-Content -LiteralPath (Join-Path $EvidenceDirectory 'git-status-before.txt') -Encoding utf8
& $git -c core.safecrlf=false -C $workspace diff --quiet HEAD --
$trackedUnchangedBefore = $LASTEXITCODE -eq 0

$shellScript = @'
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
make -k -j"$PUAE_BUILD_JOBS" platform=win CC=gcc OBJDIR="$PUAE_BUILD_OBJECTS" TARGET="$PUAE_BUILD_TARGET"
'@
$shellPath = Join-Path $EvidenceDirectory 'build-command.sh'
[IO.File]::WriteAllText($shellPath, $shellScript.Replace("`r`n", "`n") + "`n", [Text.UTF8Encoding]::new($false))

$names = @('MSYSTEM', 'CHERE_INVOKING', 'PUAE_WORKSPACE', 'PUAE_GIT_DIRECTORY', 'PUAE_EVIDENCE_DIRECTORY', 'PUAE_BUILD_JOBS', 'PUAE_BUILD_OBJECTS', 'PUAE_BUILD_TARGET')
$previous = @{}
foreach ($name in $names) {
    $previous[$name] = [Environment]::GetEnvironmentVariable($name, 'Process')
}
$started = [DateTime]::UtcNow
$timer = [Diagnostics.Stopwatch]::StartNew()
$buildExit = -1
try {
    $env:MSYSTEM = 'UCRT64'
    $env:CHERE_INVOKING = '1'
    $env:PUAE_WORKSPACE = $workspace
    $env:PUAE_GIT_DIRECTORY = Split-Path $git -Parent
    $env:PUAE_EVIDENCE_DIRECTORY = $EvidenceDirectory
    $env:PUAE_BUILD_JOBS = $Jobs.ToString()
    $env:PUAE_BUILD_OBJECTS = $ObjectDirectory
    $env:PUAE_BUILD_TARGET = $TargetName
    & $bash -lc 'source "$(cygpath -u "$PUAE_EVIDENCE_DIRECTORY")/build-command.sh"' *> $buildLog
    $buildExit = $LASTEXITCODE
}
finally {
    $timer.Stop()
    foreach ($name in $names) {
        [Environment]::SetEnvironmentVariable($name, $previous[$name], 'Process')
    }
    & $git -c core.safecrlf=false -C $workspace diff --quiet HEAD --
    $trackedUnchangedAfter = $LASTEXITCODE -eq 0
    & $git -C $workspace status --short --branch |
        Set-Content -LiteralPath (Join-Path $EvidenceDirectory 'git-status-after.txt') -Encoding utf8
    $result = [ordered]@{
        head = $head
        started_utc = $started.ToString('o')
        finished_utc = [DateTime]::UtcNow.ToString('o')
        elapsed_seconds = [Math]::Round($timer.Elapsed.TotalSeconds, 3)
        command = "make -k -j$Jobs platform=win CC=gcc OBJDIR=$ObjectDirectory TARGET=$TargetName"
        environment = 'UCRT64'
        msys_root = $MsysRoot
        exit_code = $buildExit
        tracked_files_unchanged_before = $trackedUnchangedBefore
        tracked_files_unchanged_after = $trackedUnchangedAfter
        output = (Join-Path $workspace $TargetName)
        log = $buildLog
    }
    if (Test-Path -LiteralPath $result.output -PathType Leaf) {
        $result.output_bytes = (Get-Item -LiteralPath $result.output).Length
        $result.output_sha256 = (Get-FileHash -LiteralPath $result.output -Algorithm SHA256).Hash.ToLowerInvariant()
    }
    $result | ConvertTo-Json -Depth 4 |
        Set-Content -LiteralPath (Join-Path $EvidenceDirectory 'build-result.json') -Encoding utf8
}

Get-Content -LiteralPath $buildLog -Tail 12
$result | ConvertTo-Json -Depth 4
if ($buildExit -ne 0) { throw "Windows x64 build failed with exit code $buildExit. See $buildLog." }
