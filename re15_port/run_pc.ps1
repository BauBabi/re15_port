# =============================================================================
# run_pc.ps1 — startet den re15_port PC-Build (transplantierte psx_dev-Engine).
#
#   .\run_pc.ps1            # bootet ROOM1240 (Intro-Raum)
#   .\run_pc.ps1 1170       # bootet ROOM1170 (Heli-Raum, Referenz)
#   .\run_pc.ps1 10A0       # beliebiger Raum (Hex-ID wie im Spiel)
#
# Setzt:
#   - PATH  -> mingw64\bin (liefert die dynamisch gelinkte SDL2.dll)
#   - RE15_ASSET_ROOT -> psx_dev\assets_shared (extrahierter Asset-Baum, Phase-A)
#   - RE15_START_ROOM -> Raum-ID (hex)
# =============================================================================
param([string]$Room = "1240")
$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path

$env:Path = "C:\msys64\mingw64\bin;" + $env:Path
$env:RE15_ASSET_ROOT = (Resolve-Path (Join-Path $here "..\psx_dev\assets_shared")).Path
$env:RE15_START_ROOM = $Room

$exe = Join-Path $here "build\platform\pc\re15_pc.exe"
if (-not (Test-Path $exe)) {
    Write-Error "Nicht gebaut: $exe`nErst bauen:  cmake --build `"$here\build`" --target re15_pc"
    exit 1
}
Write-Host "Starte ROOM$Room  (Asset-Root: $env:RE15_ASSET_ROOT)" -ForegroundColor Cyan
Write-Host "Log: $here\build\platform\pc\debug.log" -ForegroundColor DarkGray
& $exe
