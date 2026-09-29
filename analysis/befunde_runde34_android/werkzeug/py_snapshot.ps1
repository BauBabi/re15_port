# Kanonischer Schnappschuss: Python-Registry (HKCU/HKLM) + Startmenue + LOCALAPPDATA\Python
$out = @()
foreach ($root in 'HKCU:\Software\Python', 'HKLM:\Software\Python') {
  try { Get-ChildItem $root -Recurse -ErrorAction Stop | ForEach-Object { $out += "REG $($_.Name)" } } catch { $out += "REG (fehlt) $root" }
}
foreach ($d in "$env:APPDATA\Microsoft\Windows\Start Menu\Programs", "$env:ProgramData\Microsoft\Windows\Start Menu\Programs") {
  Get-ChildItem $d -Filter 'Python*' -ErrorAction SilentlyContinue | ForEach-Object { $out += "MENU $($_.FullName) $($_.LastWriteTime.ToString('s'))" }
}
foreach ($d in "$env:LOCALAPPDATA\Python", "$env:LOCALAPPDATA\Programs\Python") {
  Get-ChildItem $d -ErrorAction SilentlyContinue | ForEach-Object { $out += "DIR $($_.FullName) $($_.LastWriteTime.ToString('s'))" }
}
$out | Sort-Object
