# usage: powershell -File cap.ps1 -Name n -Exe path -Dur 36 -Delay 0 -EnvList "A=1;B=2"
param([string]$Name, [string]$Exe, [double]$Dur, [double]$Delay, [string]$EnvList)
$SP = "C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\abn2"
$Out = Join-Path $SP $Name
if (Test-Path $Out) { Remove-Item -Recurse -Force $Out }
New-Item -ItemType Directory -Force $Out | Out-Null
$env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
$env:SDL_ASSERT = "always_ignore"
$env:RE15_NO_INTRO = "1"
foreach ($kv in ($EnvList -split ';')) { if ($kv -match '^([A-Za-z_0-9]+)=(.*)$') { Set-Item -Path ("env:" + $Matches[1]) -Value $Matches[2] } }
$p = Start-Process -FilePath $Exe -WorkingDirectory $Out -PassThru -RedirectStandardOutput (Join-Path $Out "stdout.txt") -RedirectStandardError (Join-Path $Out "stderr.txt")
"pid=$($p.Id)" | Out-File -Encoding ascii (Join-Path $Out "run.txt")
$h = [IntPtr]::Zero
for ($i = 0; $i -lt 300; $i++) { $p.Refresh(); if ($p.HasExited) { break }; $h = $p.MainWindowHandle; if ($h -ne [IntPtr]::Zero) { break }; Start-Sleep -Milliseconds 100 }
if ($h -eq [IntPtr]::Zero) { "kein Fenster" | Out-File -Append -Encoding ascii (Join-Path $Out "run.txt"); exit 3 }
("hwnd=0x{0:x} t_fenster={1}" -f $h.ToInt64(), (Get-Date -Format o)) | Out-File -Append -Encoding ascii (Join-Path $Out "run.txt")
if ($Delay -gt 0) { Start-Sleep -Milliseconds ([int]($Delay * 1000)) }
("t_cap={0}" -f (Get-Date -Format o)) | Out-File -Append -Encoding ascii (Join-Path $Out "run.txt")
$hx = "0x{0:x}" -f $h.ToInt64()
& "C:\ProgramData\chocolatey\bin\ffmpeg.exe" -hide_banner -loglevel error -f gdigrab -framerate 30 -draw_mouse 0 -i "hwnd=$hx" -t $Dur -c:v libx264 -preset ultrafast -crf 16 -pix_fmt yuv444p (Join-Path $Out "cap.mkv") 2> (Join-Path $Out "ffmpeg.txt")
("ffmpeg_rc={0} t_capend={1}" -f $LASTEXITCODE, (Get-Date -Format o)) | Out-File -Append -Encoding ascii (Join-Path $Out "run.txt")
if (-not $p.WaitForExit(90000)) { "kill eigene pid $($p.Id)" | Out-File -Append -Encoding ascii (Join-Path $Out "run.txt"); Stop-Process -Id $p.Id -Force }
"exe_exit=$($p.ExitCode)" | Out-File -Append -Encoding ascii (Join-Path $Out "run.txt")
