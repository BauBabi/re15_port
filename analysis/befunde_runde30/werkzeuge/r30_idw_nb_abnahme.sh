#!/usr/bin/env bash
# Runde 30 / irons-diary-welt, NACHBESSERUNG: alle Abnahme-Laeufe am gebauten Spiel erneut
# (neue Lage B, geteilte Aufhebe-Rechtecke). Marken "nb_*" unter build/r30_irons-diary-welt/bau/.
# Nutzt r30_idw_bau_lauf.sh (Tuer-/Sprung-Weg) und r30_idw_bau_laden.sh (Lade-Weg).
# Beschleunigter Renderer, RE15_FRAMEDUMP; kein AUTOSHOT, kein SOFTWARE_RENDER.
#
#   r30_idw_nb_abnahme.sh [gruppe ...]    Gruppen: bild druck aufheben laden bestand (Standard: alle)
set -uo pipefail
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LAUF="$HIER/r30_idw_bau_lauf.sh"
LADEN="$HIER/r30_idw_bau_laden.sh"
GRUPPEN="${*:-bild druck aufheben laden bestand}"

for g in $GRUPPEN; do
case "$g" in
bild)
    # Cut 2 / Cut 6, mit / ohne Props, einzeln; Spieler weit weg (Standard) bzw. vor dem Tisch
    bash "$LAUF" nb_c2 2
    bash "$LAUF" nb_c2_null 2 null
    EXTRA_SET_FLAG=9:55 bash "$LAUF" nb_c2_nurdiary 2
    EXTRA_SET_FLAG=9:54 bash "$LAUF" nb_c2_nurkarte 2
    bash "$LAUF" nb_c6 6
    bash "$LAUF" nb_c6_null 6 null
    EXTRA_SET_FLAG=9:55 bash "$LAUF" nb_c6_nurdiary 6
    EXTRA_SET_FLAG=9:54 bash "$LAUF" nb_c6_nurkarte 6
    SPIELER=-22664,-18450,2048 bash "$LAUF" nb_c2_spieler 2
    SPIELER=-22664,-18450,2048 bash "$LAUF" nb_c2_spieler_null 2 null
    ;;
druck)
    # EIN Aktionsdruck (Viereck) in Bild 400, Stand an der Tischkante, Blick -X
    for z in -18900 -18800 -18720 -18649 -18463 -18462 -18275 -17950; do
        SPIELER="-22664,$z,2048" PAD_AT=400:A SERIE=400-420/10 EXIT="425#1150" SEK=40 \
            bash "$LAUF" "nb_druck_$z" auto
    done
    ;;
aufheben)
    # Karte an der Kartenmitte: Druck F400, Bestaetigen F520. Buch an der Buchmitte: Druck
    # F400, Leser zu (Kreuz) F540, Meldung bestaetigen F660, START F760 + R1 F780 = FILE-Liste.
    SPIELER=-22664,-18649,2048 PAD_AT=400:A,520:A SERIE=390-660/10 EXIT="670#1150" SEK=60 \
        bash "$LAUF" nb_karte_aufheben 6
    SPIELER=-22664,-18649,2048 SERIE=390-660/10 EXIT="670#1150" SEK=60 \
        bash "$LAUF" nb_karte_null_gleichpos 6 null
    SPIELER=-22664,-18275,2048 PAD_AT=400:A,540:X,660:A,760:S,780:M SERIE=390-820/10 \
        EXIT="830#1150" SEK=70 bash "$LAUF" nb_diary_aufheben 6
    SPIELER=-22664,-18275,2048 SERIE=390-820/10 EXIT="830#1150" SEK=70 \
        bash "$LAUF" nb_diary_null_gleichpos 6 null
    ;;
laden)
    bash "$LADEN" nb_laden1150 1150 2
    bash "$LADEN" nb_laden1150_gen 1150 2 genommen
    bash "$LADEN" nb_laden1151 1151 2
    bash "$LADEN" nb_laden1151_gen 1151 2 genommen
    SPIELER=-22664,-18649,2048 PAD_AT=100:A,220:A bash "$LADEN" nb_laden1150_karte 1150 auto
    SPIELER=-22664,-18275,2048 PAD_AT=100:A,240:X,360:A EXIT="400#1150" \
        bash "$LADEN" nb_laden1150_diary 1150 auto
    SPIELER=-22664,-18649,2048 PAD_AT=100:A,220:A EXIT="320#1151" \
        bash "$LADEN" nb_laden1151_karte 1151 auto
    SPIELER=-22664,-18275,2048 PAD_AT=100:A,240:X,360:A EXIT="400#1151" \
        bash "$LADEN" nb_laden1151_diary 1151 auto
    ;;
bestand)
    for r in 1140 1100 1110; do
        RAUM=$r SPIELER=keine bash "$LAUF" "nb_neu_$r" auto
    done
    ;;
*)
    echo "unbekannte Gruppe $g"; exit 2 ;;
esac
done
echo "[nb-abnahme] fertig: $GRUPPEN"
