#!/usr/bin/env bash
# blaetter.sh - Abnahme 5 (zweiter Abnehmer): Bildleisten der eigenen RE15_FRAMEDUMP-Serien (blatt5.py des Vorgaengers, PIL im Nutzer-Site -> python ohne -I)
M=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/annb6/abn5/mein
WZ=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/befunde_runde41/AN_belege/abn5/werkzeug
cd "$M" || exit 2
b() { local n=$1 out=$2 cols=$3 wahl=$4 titel=$5; [ -f "laeufe/$n/ergebnis.txt" ] || { echo "kein Lauf $n"; return; }; python $WZ/blatt5.py "laeufe/$n" "belege/$out" "$cols" "$wahl" 240 "$titel"; }
b s5090 s5090_szene_abn5.jpg 8 1150-1960/30 "Abnahme 5 (2. Abnehmer) s5090: Kette 6000 -> 6030 -> 5090, RE2-Szene F1168 Szenenstart .. F1776 Kampfstart (state.log bitgleich mit Abnahme 4)"
b s5090_leiche750 s5090_leiche750_abn5.jpg 6 1850-1905/5 "Abnahme 5 (2. Abnehmer) s5090_leiche750: Leiche (Phase FREI) ab F1860 750 westlich von Leon -> Leon auf 801 geschoben (RE2 350 + 450)"
b leiche_west leiche_west_abn5.jpg 8 300-700/20 "Abnahme 5 (2. Abnehmer) leiche_west: Wiedereintritt (3,42)=1, Leon geht westlich an der Leiche vorbei, Minimum 802, kein Schub"
b an_durch an_durch_abn5.jpg 7 130-400/10 "Abnahme 5 (2. Abnehmer) an_durch: lebende 0x4D (Kasten 450) auf Leons Schusslinie, 6 Schuesse gehen durch (NB5, hp -1)"
b an_vorszene an_vorszene_abn5.jpg 8 130-1300/50 "Abnahme 5 (2. Abnehmer) an_vorszene: 4 Schuesse von der Wagentuer, danach die Szene vollstaendig"
b kampf_linie kampf_linie_abn5.jpg 8 1800-3400/80 "Abnahme 5 (2. Abnehmer) kampf_linie: Leiche ab F1790 auf der Linie Leon -> G5, die Schuesse gehen durch die Leiche auf die G5"
b elza5091 elza5091_abn5.jpg 8 100-1300/40 "Abnahme 5 (2. Abnehmer) elza5091: Elza in 5091, keine 0x4D, keine Szene"
b an6000_direkt an6000_direkt_abn5.jpg 8 90-300/10 "Abnahme 5 (2. Abnehmer) an6000_direkt: 6000 ohne Messhaken, 0x4D steht nach ihrem Weg still, Leon geht von Osten in sie hinein"
b kampf kampf_abn5.jpg 8 1700-5400/100 "Abnahme 5 (2. Abnehmer) kampf: Kampf mit der Pistole, 144 Schuesse, G5 600 -> -1 (state.log bitgleich mit Abnahme 4)"
