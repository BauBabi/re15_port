#!/usr/bin/env bash
# auswerten6.sh: alle eigenen Laeufe der Abnahme 6 auswerten -> messung_abnahme6.txt (aussen6.py, mass6.py, vergleich6.py gegen a661eb8aa und Runde 1)
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn6"
W="$SP/werkzeug"; R="$SP/runs"
A4="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2abn4/runs"      # Abnahme 4: exe d872c8e6 = a661eb8aa
R1="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/abn5r2/runs"  # Abnahme 5: exe 029efac6 = Runde 1
AUS="$SP/messung_abnahme6.txt"
{
echo "# Abnahme 6 (G2-RE2 NB3 Runde 2) - eigene Messung, $(date '+%F %T'); exe neu 5910fc24 (re15_pc_g2abn6.exe), alt d872c8e6 (a661eb8aa), r1 029efac6 (10603425b)"
echo
echo "## 1. Figur ausserhalb (aussen6.py: alle Bilder in 2090/2091; Deck verlassen / aussen / Nische / im Block; Kletterbilder getrennt)"
for d in "$R"/*/; do python "$W/aussen6.py" "$d"; done
echo
echo "## 2. Mass je Lauf (mass6.py: Treffer, Oben-Abschnitte, Pausen, Schaden, ohne Fortschritt, Haenge-Fenster, Rueckfall-Zeilen, Kanister-Sitz)"
for d in "$R"/*/; do python "$W/mass6.py" "$d"; echo; done
echo
echo "## 3. Vergleich Bild fuer Bild (vergleich6.py): Abnahme-4-Laeufe gegen a661eb8aa (g2abn4/runs) und gegen Runde 1 (abn5r2/runs), mit gegen ohne"
for l in west4_leon west4_elza west5_elza frei4_leon frei4_elza; do
  for v in neu ohne; do
    [ -d "$R/${v}_$l" ] || continue
    [ -d "$A4/${v}_$l" ] && { echo "### $v $l gegen a661eb8aa"; python "$W/vergleich6.py" "$A4/${v}_$l" "$R/${v}_$l"; }
    [ -d "$R1/${v}_$l" ] && { echo "### $v $l gegen Runde 1"; python "$W/vergleich6.py" "$R1/${v}_$l" "$R/${v}_$l"; }
  done
  [ -d "$R/neu_$l" ] && [ -d "$R/ohne_$l" ] && { echo "### $l mit gegen ohne Kanister"; python "$W/vergleich6.py" "$R/neu_$l" "$R/ohne_$l"; }
done
for l in oben_leon oben_elza; do
  [ -d "$R1/neu_$l" ] && [ -d "$R/neu_$l" ] && { echo "### neu $l gegen Runde 1"; python "$W/vergleich6.py" "$R1/neu_$l" "$R/neu_$l"; }
  [ -d "$R1/ohne_$l" ] && [ -d "$R/ohne_$l" ] && { echo "### ohne $l gegen Runde 1"; python "$W/vergleich6.py" "$R1/ohne_$l" "$R/ohne_$l"; }
  [ -d "$R/neu_$l" ] && [ -d "$R/ohne_$l" ] && { echo "### $l mit gegen ohne Kanister"; python "$W/vergleich6.py" "$R/neu_$l" "$R/ohne_$l"; }
done
for p in "ohne_abn2wand_leon" "neu_abn2wand_leon"; do
  [ -d "$R1/$p" ] && [ -d "$R/$p" ] && { echo "### $p gegen Runde 1"; python "$W/vergleich6.py" "$R1/$p" "$R/$p"; }
done
for k in "$R"/alt_*; do
  n=$(basename "$k"); m="${n#alt_}"
  [ -d "$R/$m" ] && { echo "### $m gegen a661eb8aa (eigener alt-Lauf)"; python "$W/vergleich6.py" "$k" "$R/$m"; }
done
echo
echo "## 4. RAUMHALT-Zeilen aller neuen Laeufe nach Figur-Zustand (hr = hit_react, mo = Clip)"
grep -h "RAUMHALT" "$R"/*/gator_boss.log | sed -E 's/.*Band ([01]).*hr=([0-9-]+) mo=([0-9-]+).*/Band \1 hr=\2 mo=\3/' | sort | uniq -c
} > "$AUS" 2>&1
echo "geschrieben $AUS ($(wc -l < "$AUS") Zeilen)"
