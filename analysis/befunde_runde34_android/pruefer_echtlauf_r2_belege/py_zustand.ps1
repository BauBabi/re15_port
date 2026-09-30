# Pruefer echtlauf r2 - Python-Installer-Schnappschuss (nichts wird gestartet). Basis: r1-Skript
# (pruefer_echtlauf_r1_belege/py_zustand.ps1), erweitert um: Prozesse python/py/pymanager/msiexec,
# Dateien NEUER als -seit in den Python-/WindowsApps-/Startmenue-Ordnern, Uninstall-Werte.
# Aufruf: powershell -NoProfile -File py_zustand.ps1 [-baum <arbeitsbaum>] [-seit 'yyyy-MM-dd HH:mm:ss']
param([string]$baum = 'C:\workspace\git\reAi_v2\.claude\worktrees\r34a_android', [string]$seit = '')
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
# Laufende Installer-/Python-Prozesse (Momentaufnahme)
Get-CimInstance Win32_Process -ErrorAction SilentlyContinue | Where-Object { $_.Name -match '^(python|pythonw|py|pymanager|msiexec)' } | ForEach-Object {
  $out.Add("PROC $($_.Name) pid=$($_.ProcessId) $($_.ExecutablePath)") }
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
