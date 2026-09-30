#!/usr/bin/env bash
# Spur C (Runde 34 Nacht): Sichtpruefung am ECHTEN Fenster (Skill re15-port-visual-verify): Lauf S2
# (Loesung 7, 9, 3, 1, 5) ohne Framedump/Readback, das Fenster der EIGENEN exe-Kopie per Fenster-Handle
# (gdigrab hwnd=, nicht per Titel — parallele Agenten haben Fenster mit demselben Titel) mit 4 Bildern/s
# aufnehmen. Ausgabe: build/r34n_c/bau/gd/gd_###.png
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34n_generator
OUT="$BAUM/build/r34n_c/bau/gd"; mkdir -p "$OUT"; rm -f "$OUT"/gd_*.png
bash "$BAUM/re15_port/tools/r34n_c/lauf.sh" "$BAUM/build/r34n_c/bau/gd_lauf" 900 \
  "U0.3,W0.2,A0.07,D0.75,W0.2,A0.07,L1.17,U0.4,W0.2,A0.07,U0.6,W0.2,A0.07,D1.35,W0.2,A0.07,W0.7,UA3,W6" &
LP=$!
H=""
for i in $(seq 1 120); do
  H=$(powershell.exe -NoProfile -Command "(Get-Process re15_r34nc -ErrorAction SilentlyContinue | Where-Object { \$_.MainWindowHandle -ne 0 } | Select-Object -First 1).MainWindowHandle" 2>/dev/null | tr -d '\r\n ')
  [ -n "$H" ] && [ "$H" != "0" ] && break
  sleep 0.5
done
echo "hwnd=$H"
HX=$(printf '0x%x' "$H")
/c/ProgramData/chocolatey/bin/ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 4 -draw_mouse 0 \
  -t 45 -i "hwnd=$HX" "$OUT/gd_%03d.png"
echo "ffmpeg rc=$?"
wait $LP
ls "$OUT" | wc -l
