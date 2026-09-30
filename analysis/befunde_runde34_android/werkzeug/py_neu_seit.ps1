param([string]$seit = '2026-09-29 20:30:00')
$t = [datetime]::Parse($seit)
$n = 0
foreach ($d in "$env:LOCALAPPDATA\Python", "$env:LOCALAPPDATA\Microsoft\WindowsApps", "$env:LOCALAPPDATA\Programs\Python", "$env:APPDATA\Microsoft\Windows\Start Menu\Programs") {
  Get-ChildItem $d -Recurse -Force -ErrorAction SilentlyContinue | Where-Object { $_.LastWriteTime -gt $t -or $_.CreationTime -gt $t } | ForEach-Object { "NEU: $($_.FullName) $($_.LastWriteTime.ToString('s'))"; $n++ }
}
"Dateien/Ordner neuer als ${seit}: $n"
