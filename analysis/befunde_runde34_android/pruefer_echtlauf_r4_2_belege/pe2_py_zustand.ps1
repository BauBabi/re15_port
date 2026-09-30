# Pruefer echtlauf R4-2 - eigener Python-Installer-Schnappschuss (liest nur, startet nichts).
# Aufruf: powershell -NoProfile -ExecutionPolicy Bypass -File pe2_py_zustand.ps1
$o = New-Object System.Collections.Generic.List[string]
foreach ($r in 'HKCU:\Software\Python','HKLM:\Software\Python','HKLM:\Software\WOW6432Node\Python') {
  if (Test-Path $r) {
    $o.Add("REG $r")
    Get-ChildItem $r -Recurse -ErrorAction SilentlyContinue | ForEach-Object {
      $k=$_; $o.Add("REG $($k.Name)")
      foreach ($v in $k.GetValueNames()) { $o.Add("VAL $($k.Name) [$v]=$($k.GetValue($v))") } }
  } else { $o.Add("REG-FEHLT $r") }
}
foreach ($u in 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall','HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall','HKLM:\Software\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall') {
  Get-ChildItem $u -ErrorAction SilentlyContinue | ForEach-Object { $dn=$_.GetValue('DisplayName'); if ($dn -match 'Python|pymanager') { $o.Add("UNINST $u $($_.PSChildName) $dn $($_.GetValue('DisplayVersion'))") } } }
foreach ($d in "$env:APPDATA\Microsoft\Windows\Start Menu\Programs","$env:ProgramData\Microsoft\Windows\Start Menu\Programs") {
  Get-ChildItem $d -Recurse -Force -ErrorAction SilentlyContinue | Where-Object { $_.Name -match 'Python|pymanager' } | ForEach-Object { $o.Add("MENU $($_.FullName) $($_.LastWriteTime.ToString('s'))") } }
foreach ($d in "$env:LOCALAPPDATA\Programs\Python","$env:LOCALAPPDATA\Python","C:\Program Files\Python*","C:\Python*") {
  Get-Item $d -Force -ErrorAction SilentlyContinue | ForEach-Object { $o.Add("DIR $($_.FullName) $($_.LastWriteTime.ToString('s'))")
    Get-ChildItem $_.FullName -Force -ErrorAction SilentlyContinue | ForEach-Object { $o.Add("DIR1 $($_.FullName) $($_.LastWriteTime.ToString('s'))") } } }
Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WindowsApps" -Force -ErrorAction SilentlyContinue | Where-Object { $_.Name -match '^(py|python|pymanager|pip)' } | ForEach-Object { $o.Add("WINAPPS $($_.Name) $($_.LastWriteTime.ToString('s'))") }
Get-AppxPackage -ErrorAction SilentlyContinue | Where-Object { $_.Name -match 'Python' } | ForEach-Object { $o.Add("APPX $($_.Name) $($_.Version)") }
$o | Sort-Object
