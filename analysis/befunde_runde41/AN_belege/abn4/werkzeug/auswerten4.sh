#!/usr/bin/env bash
# auswerten4.sh <lauf> - Vergleich mit dem Abnahme-3-Lauf gleichen Namens (falls vorhanden), Schuesse, Abstand, Szene.
W=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2abn
A3=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/build/abn3
N=$1; L=$W/an4/laeufe/$N; O=$W/belege/auswertung_$N.txt
{
echo "# Auswertung Lauf $N (Abnahme 4 Spur AN, exe cefd57402)"; cat $L/ergebnis.txt; echo
echo "## env"; cat $L/env.txt; echo
echo "## Ende debug.log"; tail -3 $L/debug.log | cut -c1-200; echo
if [ -f $A3/$N/state.log ]; then echo "## Vergleich state.log gegen Abnahme 3 (exe b646c72a6, NB4): logvergleich.py"; python $W/werkzeug/logvergleich.py $A3/$N/state.log $L/state.log 4; echo; fi
echo "## Abstand Spieler <-> Annette: abstand4.py"; python $W/werkzeug/abstand4.py $L/state.log; echo
echo "## Schuesse und Treffer (letzter Raumabschnitt): schuss4.py"; python $W/werkzeug/schuss4.py $L/state.log | head -70; echo
echo "## Annette-Szene: szene4.py"; python $W/werkzeug/szene4.py $L/debug.log
} > $O 2>&1
echo "-> $O"
