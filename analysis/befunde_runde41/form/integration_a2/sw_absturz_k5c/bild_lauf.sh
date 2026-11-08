#!/usr/bin/env bash
# $1 = vor|nach
W=/c/workspace/git/reAi_v2/.claude/worktrees/r41_integration
A=C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5c/bild/$1
mkdir -p "$A"
cd "$W" || exit 2
lauf() { ziel=$1; shift; echo "=== $ziel $*"; o=$(bash re15_port/tools/form_spiel.sh "$@" --aus "$A/$ziel" --nach 6 --env "RE15_FRAMEDUMP=300-3000/50:fd_" 2>&1); echo "$o" | grep -a "Verfahren\|Bahnsteig-Form\|typ 0x30\|kein Bild\|Laufverzeichnis\|sw-teilung\|\[tim\]" | cut -c1-220; d=$(echo "$o" | grep -a Laufverzeichnis | sed 's/Laufverzeichnis: //'); mkdir -p "$A/${ziel%.png}"; cp "$d"/fd_*.ppm "$A/${ziel%.png}/" 2>/dev/null; grep -a "Renderer" "$d/debug.log" | head -1; ls "$A/${ziel%.png}" | wc -l; }
lauf pose1_r5080_s1.png --raum 5080 --pose "-18564,-18386,80,0,11,1"
lauf pose2_r5090_s11.png --pose "941,-23776,3874,12,0,11"
echo BILD-FERTIG
