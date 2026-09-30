#!/bin/bash
# satz_bauen.sh - Runde 34 Nacht, Spur E: baut die Seiten FILE26..29 REPRODUZIERBAR aus den Werkzeugen
# und vergleicht sie mit der md5-Liste des Dossiers (analysis/befunde_runde34_nacht/E_belege/satz_md5.txt).
#   1. Atlas der RE2-Dokumentschrift   re15_port/tools/re2_doc_satz.py atlas
#   2. je Dokument der Satz            re15_port/tools/r34n_e/doc_satz_brief.py (Vorlage = RE2-Bildsatz
#                                      der Nutzer-Papierwahl: FILE00 / FILE08 / FILE02 / FILE06)
#   3. md5-Vergleich gegen satz_md5.txt (Abweichung -> exit 1)
# Der Bauschritt kopiert danach build/r34n_e/satz_neu/FILEnn/FILEnn_*.TIM nach
# re15_port/shared_assets/RE2/FILES/ (E_dokumente.md 5.1).
# Aufruf: bash re15_port/tools/r34n_e/satz_bauen.sh [ausgabe-verzeichnis]
set -eu
BAUM="$(cd "$(dirname "$0")/../../.." && pwd)"
PY=C:/Python310/python.exe
OUT="${1:-$BAUM/build/r34n_e/satz_neu}"
T="$BAUM/analysis/befunde_runde34_nacht/E_texte"
mkdir -p "$OUT"
cd "$BAUM"
"$PY" re15_port/tools/re2_doc_satz.py atlas --out "$OUT/atlas" > "$OUT/atlas.log"
F="$OUT/atlas/re2_doc_font.json"
"$PY" re15_port/tools/r34n_e/doc_satz_brief.py --out "$OUT/FILE26" --font "$F" --text "$T/dok1_police_officer.txt" \
      --titel "POLICE OFFICER'S FINAL DIARY ENTRY" --doc 26 --vorlage 0 > "$OUT/FILE26.log"
"$PY" re15_port/tools/r34n_e/doc_satz_brief.py --out "$OUT/FILE27" --font "$F" --text "$T/dok2_elliot.txt" \
      --titel "ELLIOT'S DIARY" --doc 27 --vorlage 8 > "$OUT/FILE27.log"
"$PY" re15_port/tools/r34n_e/doc_satz_brief.py --out "$OUT/FILE28" --font "$F" --text "$T/dok3_marvin.txt" \
      --titel "MARVIN'S NOTES" --doc 28 --vorlage 2 --unterschrift > "$OUT/FILE28.log"
"$PY" re15_port/tools/r34n_e/doc_satz_brief.py --out "$OUT/FILE29" --font "$F" --text "$T/dok4_armory.txt" \
      --titel "ARMORY NOTICE" --doc 29 --vorlage 6 > "$OUT/FILE29.log"
fehler=0
for n in 26 27 28 29; do
  zeile=""
  for f in $(cd "$OUT/FILE$n" && ls FILE${n}_*.TIM | sort); do
    zeile="$zeile$(md5sum < "$OUT/FILE$n/$f" | cut -c1-8) $f;"
  done
  soll="$(grep "FILE${n}_" "$BAUM/analysis/befunde_runde34_nacht/E_belege/satz_md5.txt" | tr -d '\r')"
  if [ "$zeile" = "$soll" ]; then echo "FILE$n gleich der md5-Liste"; else echo "FILE$n ABWEICHUNG"; echo " ist:  $zeile"; echo " soll: $soll"; fehler=1; fi
done
exit $fehler
