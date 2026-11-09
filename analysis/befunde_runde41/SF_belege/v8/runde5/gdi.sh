#!/usr/bin/env bash
# gdigrab des ECHTEN Spielfensters (Runde 5, wie runde4/suche/gdi.sh): eigene exe-Kopie aus lauf_v4.sh; das Fenster wird ueber die EIGENE PID
# gesucht (Prozessname re15_sfv4_<PID des lauf_v4.sh-Prozesses>, Sicht-Pruefer r4) und per SetWindowPos ganz nach oben gelegt.
# Kein AUTOSHOT, kein SOFTWARE_RENDER. Fremde re15-Prozesse werden nicht angefasst.
S=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8
O=$S/r5/gdi
mkdir -p $O
D=$S/szene_gdi_r5
export BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8
FD=1100-1100/1 bash $BAUM/re15_port/tools/r41_sichel/v4/lauf_v4.sh $D - 1110 > $O/gdi_lauf.txt 2>&1 &
LPID=$!
R=""
for i in $(seq 1 60); do
  R=$(powershell -NoProfile -Command '
Add-Type @"
using System; using System.Runtime.InteropServices;
public class W { [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
[DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
[DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr a, int x, int y, int cx, int cy, uint f);
public struct RECT { public int L, T, R, B; } public struct POINT { public int X, Y; } }
"@
$p = Get-Process | Where-Object { $_.ProcessName -eq "re15_sfv4_'"$LPID"'" -and $_.MainWindowHandle -ne 0 } | Select-Object -First 1
if ($p) { [W]::SetWindowPos($p.MainWindowHandle, [IntPtr](-1), 0, 0, 0, 0, 3) | Out-Null
  $r = New-Object W+RECT; [W]::GetClientRect($p.MainWindowHandle, [ref]$r) | Out-Null; $pt = New-Object W+POINT; [W]::ClientToScreen($p.MainWindowHandle, [ref]$pt) | Out-Null; "$($p.Id) $($pt.X) $($pt.Y) $($r.R) $($r.B)" }')
  if [ -n "$R" ]; then break; fi
  sleep 1
done
echo "Fenster (Prozess re15_sfv4_$LPID): $R" > $O/gdi_fenster.txt
set -- $R
W=$(( $4 / 2 * 2 )); H=$(( $5 / 2 * 2 ))
ffmpeg -y -loglevel error -f gdigrab -framerate 30 -offset_x $2 -offset_y $3 -video_size ${W}x${H} -t 50 -i desktop -c:v libx264 -pix_fmt yuv420p $O/gdi_szene_v8r5.mp4 2> $O/gdi_err.txt
wait $LPID
echo fertig >> $O/gdi_fenster.txt
