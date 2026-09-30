# Pruefer echtlauf r3 - Python-Installer-Schnappschuss (nichts wird gestartet). Basis: r2-Skript
# (pruefer_echtlauf_r2_belege/py_zustand.ps1). Aenderung r3: Prozesse nur, wenn sie nach Installer
# aussehen (pymanager/msiexec/py.exe oder python* aus WindowsApps bzw. %LOCALAPPDATA%\Python) - die
# C:\Python310-Prozesse paralleler Sitzungen sind Rauschen und stehen nur in der Beilage (-proc).
# Aufruf (Execution-Policy blockt -File): & ([scriptblock]::Create((Get-Content -Raw <datei>))) -seit '...'
param([string]$baum = 'C:\workspace\git\reAi_v2\.claude\worktrees\r34a_android', [string]$seit = '', [switch]$proc)
$out = New-Object System.Collections.Generic.List[string]
foreach ($root in 'HKCU:\Software\Python', 'HKLM:\Software\Python', 'HKLM:\Software\WOW6432Node\Python') {
  if (Test-Path $root) {
    $out.Add("REG $root")
    Get-ChildItem $root -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
      $k = $_
      $out.Add("REG $($k.Name)")
      foreach ($v in $k.GetValueNames()) { $out.Add("REGVAL $($k.Name) [$v]=$($k.GetValue($v))") }
    }
  } else { $out.Add("REG (fehlt) $root") }
}
foreach ($u in 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall') {
  Get-ChildItem $u -ErrorAction SilentlyContinue | ForEach-Object {
    $dn = $_.GetValue('DisplayName')
    if (($dn -and $dn -match 'Python') -or $_.PSChildName -match 'py') {
      $out.Add("UNINST $($_.PSChildName) $dn InstallDate=$($_.GetValue('InstallDate')) Loc=$($_.GetValue('InstallLocation'))") }
  }
}
foreach ($d in "$env:APPDATA\Microsoft\Windows\Start Menu\Programs", "$env:ProgramData\Microsoft\Windows\Start Menu\Programs") {
  Get-ChildItem $d -Recurse -Force -ErrorAction SilentlyContinue | Where-Object { $_.FullName -match 'Python' } | ForEach-Object {
    $out.Add("MENU $($_.FullName) $($_.LastWriteTime.ToString('s'))") }
}
foreach ($d in "$env:LOCALAPPDATA\Python", "$env:LOCALAPPDATA\Programs\Python") {
  if (Test-Path $d) {
    $all = @(Get-ChildItem $d -Recurse -Force -ErrorAction SilentlyContinue)
    $neu = ($all | Sort-Object LastWriteTime -Descending | Select-Object -First 1)
    $out.Add("DIR $d eintraege=$($all.Count) neuester=$(if ($neu) { $neu.FullName + ' ' + $neu.LastWriteTime.ToString('s') })")
    Get-ChildItem $d -Force -ErrorAction SilentlyContinue | ForEach-Object { $out.Add("DIR1 $($_.FullName) $($_.LastWriteTime.ToString('s'))") }
  } else { $out.Add("DIR (fehlt) $d") }
}
Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WindowsApps" -Force -ErrorAction SilentlyContinue | Where-Object { $_.Name -match '^(py|python|pymanager|pip)' } | ForEach-Object {
  $out.Add("WINAPPS $($_.Name) $($_.LastWriteTime.ToString('s')) $($_.Length)") }
foreach ($p in "$baum\release\Python", "$baum\Python", "$baum\build\Python", "$baum\re15_port\platform\android\Python") {
  $out.Add("BAUM $p vorhanden=$(Test-Path $p)") }
Get-ChildItem "$baum\release" -Force -Filter 'python_install*' -ErrorAction SilentlyContinue | ForEach-Object { $out.Add("BAUMLOG $($_.FullName)") }
$ps = @(Get-CimInstance Win32_Process -ErrorAction SilentlyContinue | Where-Object { $_.Name -match '^(python|pythonw|py|pymanager|msiexec)' })
foreach ($x in $ps) {
  $ep = "$($x.ExecutablePath)"
  if ($x.Name -match '^(pymanager|msiexec|py\.exe)' -or $ep -like '*WindowsApps*' -or $ep -like '*AppData\Local\Python*' -or $ep -like '*Programs\Python*') {
    $out.Add("PROC_INSTALLERVERDACHT $($x.Name) pid=$($x.ProcessId) $ep") }
}
if ($seit) {
  $t = [datetime]::ParseExact($seit, 'yyyy-MM-dd HH:mm:ss', $null)
  foreach ($d in "$env:LOCALAPPDATA\Python", "$env:LOCALAPPDATA\Programs\Python", "$env:LOCALAPPDATA\Microsoft\WindowsApps",
                 "$env:APPDATA\Microsoft\Windows\Start Menu\Programs", "$env:ProgramData\Microsoft\Windows\Start Menu\Programs") {
    if (Test-Path $d) {
      Get-ChildItem $d -Recurse -Force -ErrorAction SilentlyContinue | Where-Object { $_.LastWriteTime -gt $t -or $_.CreationTime -gt $t } | ForEach-Object {
        $out.Add("NEU_SEIT $seit $($_.FullName) $($_.LastWriteTime.ToString('s'))") }
    }
  }
}
$out | Sort-Object
if ($proc) {
  "---- Beilage: alle python*/msiexec-Prozesse (Rauschen paralleler Sitzungen, nicht im Vergleich) ----"
  foreach ($x in $ps) { "PROCINFO $($x.Name) pid=$($x.ProcessId) $($x.ExecutablePath)" }
}
