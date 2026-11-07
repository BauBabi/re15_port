#!/usr/bin/env bash
# NB3 Bauer: Bildleisten aus den RE15_FRAMEDUMP-Serien der finalen Laeufe (final_runs/), Werkzeug des Messers fdleiste.py (F-genau).
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3"
W="$SP/werkzeug"
R="$SP/final_runs/runs"
AUS="C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/analysis/befunde_runde41/G2_belege/nb3/bau"
mkdir -p "$AUS"
# Rampe oben, Leon: alter Stand (gleiche Eingabe) | neu ohne | neu mit Kanister. Klettern F48-F82, Rammen F383 (Zaehler-Querung), CROSS-Biss F483, Kanister
# angehaengt F1013, Rammen F1339, CROSS-Biss F1439, Hochbiss F1585/F1690/F1881/F1986/F2107, Rammen F2410, CROSS-Biss F2510 (Treffer = obenlog).
python "$W/fdleiste.py" "$AUS/nb3_bau_oben_leon_F90-F1440.jpg" "oben Leon" "a661eb8aa ohne=$R/oben_leon_ohne_alt" "neu ohne=$R/oben_leon_ohne" "neu mit=$R/oben_leon_neu" -- 90 390 480 1020 1350 1440
python "$W/fdleiste.py" "$AUS/nb3_bau_oben_leon_F1590-F2520.jpg" "oben Leon" "a661eb8aa ohne=$R/oben_leon_ohne_alt" "neu ohne=$R/oben_leon_ohne" "neu mit=$R/oben_leon_neu" -- 1590 1890 2100 2430 2520
# Rampe oben, Elza: neu ohne | neu mit (Rammen F383, CROSS-Biss F483, Rammen F1337, CROSS-Biss F1437, Hochbiss F1583, Rammen F2408)
python "$W/fdleiste.py" "$AUS/nb3_bau_oben_elza_F90-F2430.jpg" "oben Elza" "neu ohne=$R/oben_elza_ohne" "neu mit=$R/oben_elza_neu" -- 90 390 480 1350 1440 1590 2430
# Plattform-/Rampen-Suedkante, Leon ohne Kanister, Cut-2-Lage west4_leon (RUECK_LAGE 4200,-18800,3072,2; Laeufe kante2_bau.sh): alter Stand (Haenge-Fenster
# F2218-F2367, Alligator pendelt an der Plattform-SO-Ecke) | neu (31 Treffer, 0 Fenster). Die Riegel-Lage rampe (3000,-18800, Cut 0) zeigt beide nicht im Bild
# (Leiste nb3_bau_suedkante_leon_F2400-F2800 verworfen).
python "$W/fdleiste.py" "$AUS/nb3_bau_suedkante_cut2_leon_F200-F2800.jpg" "Suedkante Cut 2" "a661eb8aa ohne=$R/kante2_leon_ohne_alt" "neu ohne=$R/kante2_leon_ohne" -- 200 600 1000 1500 2250 2300 2350 2800
ls -la "$AUS"/*.jpg
