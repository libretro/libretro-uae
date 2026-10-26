[CmdletBinding()]
param(
    [ValidatePattern('^[A-Za-z0-9_-]+$')]
    [string]$RunName = ('aros-' + [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ')),
    [ValidateSet('A500', 'A1200')]
    [string]$Model = 'A1200',
    [ValidateRange(1, 60000)]
    [int]$MaxFrames = 6000,
    [string]$RuntimeRoot = 'C:\Projects\puae-runtime',
    [string]$EvidenceRoot
)

$ErrorActionPreference = 'Stop'
$workspace = Split-Path $PSScriptRoot -Parent
$app = Join-Path $RuntimeRoot 'RetroArch-Win64'
$exe = Join-Path $app 'retroarch.exe'
$sourceCore = Join-Path $workspace 'puae_libretro.dll'
$core = Join-Path $app 'cores\puae_libretro.dll'
$expectedHash = 'bcd4827e5138a14e22a39535754aea0d3913ef0404b636d1cc666476aa3c19e8'
foreach ($file in @($exe, $sourceCore)) {
    if (-not (Test-Path -LiteralPath $file -PathType Leaf)) { throw "Missing required file: $file" }
}
if ((Get-FileHash -LiteralPath $sourceCore -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash) {
    throw 'The core differs from the preserved unchanged build. Select and record a new baseline before testing another binary.'
}
if (Test-Path -LiteralPath $core) {
    if ((Get-FileHash -LiteralPath $core -Algorithm SHA256).Hash.ToLowerInvariant() -ne $expectedHash) {
        throw 'The runtime core differs from the baseline; refusing to overwrite it.'
    }
}
else {
    Copy-Item -LiteralPath $sourceCore -Destination $core
}

if (-not $EvidenceRoot) { $EvidenceRoot = Join-Path $workspace 'docs\runtime\2026-10-27-aros-smoke' }
$evidence = Join-Path $EvidenceRoot $RunName
$profile = Join-Path $RuntimeRoot "profiles\$RunName"
if ((Test-Path -LiteralPath $profile) -or (Test-Path -LiteralPath $evidence)) {
    throw 'Run profile or evidence already exists. Use a fresh RunName to preserve previous results.'
}
New-Item -ItemType Directory -Path $profile, $evidence -Force | Out-Null
foreach ($name in @('system', 'saves', 'states', 'screenshots', 'logs', 'config', 'remaps', 'playlists', 'content', 'cache', 'recordings')) {
    New-Item -ItemType Directory -Path (Join-Path $profile $name) | Out-Null
}
function ConfigPath([string]$path) { $path.Replace('\', '/') }
$cfg = Join-Path $profile 'retroarch.cfg'
$options = Join-Path $profile 'core-options.cfg'
$config = @"
config_save_on_exit = "false"
system_directory = "$(ConfigPath (Join-Path $profile 'system'))"
savefile_directory = "$(ConfigPath (Join-Path $profile 'saves'))"
savestate_directory = "$(ConfigPath (Join-Path $profile 'states'))"
screenshot_directory = "$(ConfigPath (Join-Path $profile 'screenshots'))"
log_dir = "$(ConfigPath (Join-Path $profile 'logs'))"
rgui_config_directory = "$(ConfigPath (Join-Path $profile 'config'))"
input_remapping_directory = "$(ConfigPath (Join-Path $profile 'remaps'))"
playlist_directory = "$(ConfigPath (Join-Path $profile 'playlists'))"
content_directory = "$(ConfigPath (Join-Path $profile 'content'))"
cache_directory = "$(ConfigPath (Join-Path $profile 'cache'))"
recording_output_directory = "$(ConfigPath (Join-Path $profile 'recordings'))"
recording_config_directory = "$(ConfigPath (Join-Path $profile 'recordings'))"
core_options_path = "$(ConfigPath $options)"
libretro_directory = "$(ConfigPath (Join-Path $app 'cores'))"
libretro_info_path = "$(ConfigPath (Join-Path $app 'info'))"
assets_directory = "$(ConfigPath (Join-Path $app 'assets'))"
video_filter_dir = "$(ConfigPath (Join-Path $app 'filters\video'))"
audio_filter_dir = "$(ConfigPath (Join-Path $app 'filters\audio'))"
history_list_enable = "false"
auto_overrides_enable = "false"
auto_remaps_enable = "false"
auto_shaders_enable = "false"
savestate_auto_save = "false"
savestate_auto_load = "false"
rewind_enable = "false"
run_ahead_enabled = "false"
sort_savefiles_enable = "false"
sort_savestates_enable = "false"
menu_driver = "rgui"
menu_show_start_screen = "false"
menu_pause_libretro = "true"
video_driver = "d3d11"
video_fullscreen = "false"
video_windowed_fullscreen = "false"
video_windowed_scale = "1.5"
video_vsync = "true"
video_gpu_screenshot = "false"
video_shader_enable = "false"
audio_driver = "wasapi"
audio_enable = "true"
audio_sync = "true"
pause_nonactive = "false"
input_driver = "dinput"
input_joypad_driver = "xinput"
input_menu_toggle = "f1"
input_save_state = "f2"
input_reset = "f3"
input_load_state = "f4"
input_screenshot = "f8"
input_exit_emulator = "escape"
quit_press_twice = "false"
log_verbosity = "true"
frontend_log_level = "0"
libretro_log_level = "0"
"@
$coreOptions = @"
puae_model = "$Model"
puae_kickstart = "auto"
puae_video_standard = "PAL"
puae_use_boot_hd = "disabled"
puae_use_whdload = "disabled"
"@
$config | Set-Content -LiteralPath $cfg -Encoding utf8
$coreOptions | Set-Content -LiteralPath $options -Encoding utf8
Copy-Item -LiteralPath $cfg, $options -Destination $evidence
$log = Join-Path $evidence 'retroarch.log'
$screenshot = Join-Path $evidence 'final-frame.png'
$arguments = @('--verbose', '--config', ('"{0}"' -f $cfg), '-L', ('"{0}"' -f $core),
    ('--log-file="{0}"' -f $log), "--max-frames=$MaxFrames", '--max-frames-ss',
    ('--max-frames-ss-path="{0}"' -f $screenshot))
$started = [DateTime]::UtcNow
$launch = [ordered]@{
    started_utc = $started.ToString('o')
    head = (& git -C $workspace rev-parse HEAD).Trim()
    executable = $exe
    executable_sha256 = (Get-FileHash -LiteralPath $exe -Algorithm SHA256).Hash.ToLowerInvariant()
    core = $core
    core_sha256 = $expectedHash
    model = $Model
    kickstart_option = 'auto'
    profile = $profile
    system_files_before = @(Get-ChildItem -LiteralPath (Join-Path $profile 'system') -File -Recurse | ForEach-Object FullName)
    content_files_before = @(Get-ChildItem -LiteralPath (Join-Path $profile 'content') -File -Recurse | ForEach-Object FullName)
    working_directory = $app
    arguments = $arguments
    max_frames = $MaxFrames
}
$launch | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $evidence 'launch.json') -Encoding utf8
$process = Start-Process -FilePath $exe -ArgumentList $arguments -WorkingDirectory $app -PassThru `
    -RedirectStandardOutput (Join-Path $evidence 'stdout.txt') -RedirectStandardError (Join-Path $evidence 'stderr.txt')
$process.Id | Set-Content -LiteralPath (Join-Path $evidence 'process-id.txt') -Encoding ascii
Write-Output "RetroArch PID $($process.Id); evidence: $evidence"
$process.WaitForExit()
$result = [ordered]@{
    started_utc = $started.ToString('o')
    finished_utc = [DateTime]::UtcNow.ToString('o')
    elapsed_seconds = [Math]::Round(([DateTime]::UtcNow - $started).TotalSeconds, 3)
    exit_code = $process.ExitCode
    core_sha256_after = (Get-FileHash -LiteralPath $core -Algorithm SHA256).Hash.ToLowerInvariant()
    system_files_after = @(Get-ChildItem -LiteralPath (Join-Path $profile 'system') -File -Recurse | ForEach-Object FullName)
    profile = $profile
    final_frame_exists = Test-Path -LiteralPath $screenshot
}
$result | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $evidence 'run-result.json') -Encoding utf8
$result | ConvertTo-Json -Depth 5
if ($process.ExitCode -ne 0) { throw "RetroArch exited with $($process.ExitCode); see $log" }
