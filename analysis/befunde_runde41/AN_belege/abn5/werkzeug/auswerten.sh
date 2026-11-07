#!/usr/bin/env bash
# auswerten.sh - Abnahme 5 (zweiter Abnehmer): Auswertung eines eigenen Fensterlaufs -> belege/auswertung_<lauf>.txt
#   auswerten.sh <lauf> [abstand_ab_bild=0] [abstand_bis=999999]
# Vergleiche: Abnahme 4 (w2abn/an4/laeufe, exe cefd57402, 0x4D ueberall 350), Vorgaenger-Abnehmer (abn5/laeufe, exe 99bbe97ba),
# Abnahme 3 (re15_port/build/abn3, exe b646c72a6, 0x4D ueberall 450) - jeweils wenn vorhanden.
set -uo pipefail
B=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/annb6/abn5
WZ=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_belege/abn5/werkzeug
N="${1:?lauf}"; AB="${2:-0}"; BIS="${3:-999999}"
L="$B/mein/laeufe/$N"; O="$B/mein/belege/auswertung_$N.txt"
{
echo "# Auswertung Lauf $N (Abnahme 5 Spur AN, zweiter Abnehmer, Messkopie re15_pc_anabn5.exe sha256 $(cat $L/exe_sha.txt) = 99bbe97ba)"
cat "$L/ergebnis.txt"
echo; echo "## env"; cat "$L/env.txt"
echo; echo "## Ende debug.log"; tail -3 "$L/debug.log"
echo; echo "## Tuerbank/Audio (debug.log)"; grep -h "Tuerbank\|DOOR2A\|audio\] " "$L/debug.log" | head -3
for ref in "Abnahme 4 (exe cefd57402, 0x4D ueberall 350)|$B/../../w2abn/an4/laeufe/$N/state.log" "Vorgaenger-Abnehmer (exe 99bbe97ba)|$B/laeufe/$N/state.log" "Abnahme 3 (exe b646c72a6, 0x4D ueberall 450)|/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3/$N/state.log"; do
  nm="${ref%%|*}"; p="${ref#*|}"
  echo; echo "## Vergleich state.log gegen $nm"
  if [ -f "$p" ]; then python -I "$WZ/vergleich5.py" "$p" "$L/state.log" 2; else echo "(kein Referenzlauf $p)"; fi
done
echo; echo "## Abstand Spieler <-> 0x4D (abstand5.py, letzter Raumabschnitt, ab F$AB bis F$BIS)"; python -I "$WZ/abstand5.py" "$L/state.log" 4d "$AB" "$BIS"
echo; echo "## Schuesse und hp-Wechsel (schuss5.py, letzter Raumabschnitt)"; python -I "$WZ/schuss5.py" "$L/state.log"
echo; echo "## Annette-Protokoll (debug.log)"; grep -h "\[annette\]" "$L/debug.log" | head -40
echo; echo "## Spawn-Diagnose 0x4D (debug.log)"; grep -h "4d\|0x4D" "$L/debug.log" | grep -i "spawn\|em_set\|hitbox" | head -6
} > "$O" 2>&1
echo "-> $O ($(wc -l < "$O") Zeilen)"
