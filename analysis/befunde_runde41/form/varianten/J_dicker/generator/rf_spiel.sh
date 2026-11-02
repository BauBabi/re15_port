#!/usr/bin/env bash
# Rohbau final: Schluss-Pruefung im echten Spiel. Pose 2 ROOM5090 Schnitt 11, Pose 1 und Ruhe ROOM5080 Schnitt 1, dazu die Nachstellung von Bild 1 aus Abnahme 1. Lage und Gier aus form.json.
# Getrennte Sitzung (qwinsta Getr.): gdigrab per hwnd liefert Muell, auch wenn die Desktop-Probe von form_spiel.sh durchgeht, PrintWindow gelegentlich Schwarz.
# Darum eine Kopie von form_spiel.sh, die immer PrintWindow nimmt, und je Lauf bis zu drei Versuche. Gut ist ein Bild nur mit 640x480 und mittlerer Helligkeit.
F=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/rohbau_final
W=C:/workspace/git/reAi_v2/.claude/worktrees/r41_form
cd "$W" || exit 2
# Python schreibt unter Windows CRLF: ohne tr haengt das CR an der letzten Zahl, und sscanf im Spiel verwirft die ganze Pose.
Z=$(python -c "
import json
j=json.load(open(r'$F/form.json'))
a,b=j['bild1'],j['bild2']
print(a['lage'][0],a['lage'][2],a['gier'],b['lage'][0],b['lage'][2],b['gier'])" | tr -d '\015')
read -r X1 Z1 G1 X2 Z2 G2 <<< "$Z"

SPIEL="$F/zw/form_spiel_pw.sh"
sed -e "s|^BAUM=.*|BAUM=\"$W\"|" -e 's|then WEG=gdigrab$|then WEG=PrintWindow|' re15_port/tools/form_spiel.sh > "$SPIEL"

gut() {
    python -c "
import cv2,sys
a=cv2.imread(r'$1')
sys.exit(0 if a is not None and a.shape[:2]==(480,640) and 8<a.mean()<170 else 1)"
}

lauf() {
    # lauf ZIEL.png weitere Optionen
    ziel=$1; shift
    for v in 1 2 3; do
        bash "$SPIEL" --emd "$F/FORM.EMD" --tim "$F/FORM.TIM" "$@" --aus "$ziel" --nach 6 2>&1 | tail -6
        gut "$ziel" && return 0
        echo "Bild unbrauchbar, neuer Versuch"
    done
    return 1
}

lauf "$F/spiel_pose2_r5090_s11.png" --pose "$X2,$Z2,$G2,1,0,11"
lauf "$F/spiel_pose1_r5080_s1.png" --raum 5080 --pose "$X1,$Z1,$G1,0,0,1"
lauf "$F/spiel_ruhe_r5080_s1.png" --raum 5080 --pose "$X1,$Z1,$G1,2,0,1"
# Nachstellung Bild 1 (Abnahme 1, Abschnitt 4): Figur auf der Blickachse von Cut 1, Abstand wie Bild 1, Gier so, dass der Kamera dieselbe Seite zugewandt ist.
lauf "$F/spiel_pose1_nachstellung_r5080.png" --raum 5080 --pose "-20711,-15984,756,0,0,1"
