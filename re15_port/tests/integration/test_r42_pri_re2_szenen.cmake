# Runde 42 Strang rest, Stufe 6 - Szenen fuer test_r41_masken36_stage2.cmake (SZENEN=diese Datei, Riegel integration_r42_pri_re2):
# die RE2-Zusatzmasken (re15_pri.h re15_pri_re2_anhaengen, Daten tools/maske/re2_uebernahme_r42.py) an der echten exe, Leon und Elza.
# Lagen: erreichbare Stehplaetze (test_r37_masken36 plaetze, Bestandsaufnahme pri_stage456/plaetze), Figurkasten 3000 x +-350.
#   H  Figur HINTER dem Gegenstand (solide SCA-Zelle zwischen Auge und Figur), Kamera-z der Figur >= 1500 ueber der Schwelle der
#      umgerechneten RE2-Tiefe: im Fenster (Figurkasten geschnitten mit den Maskenpunkten) >= Haelfte der Figurpunkte verdeckt.
#   L  Figur VOR dem Gegenstand, nur die Spielfigur zaehlt (dritter Lauf D ohne Masken, Figur an A0 ausserhalb des Regionsvierecks):
#      0 Punkte uebermalt. v* = staerkste Ueberdeckung des Figurkastens mit Maskenpunkten ohne Verdeckung im Modell; x* = die Lagen,
#      die das Kastenmodell als "davor und doch verdeckt" meldete (50D0 K10 93 Punkte, 6000 K1 79 Punkte) - an der exe 0.
#   5100 K7: Fenster nur Kopf/Oberkoerper (y 104..139) - die Beine verdecken schon die Port-Masken (R15M), gemessen ohne Zusatz
#   773/1238 im ganzen Kasten, im Fenster 0/417 (Leon) bzw. 0/362 (Elza); mit Zusatz 411/417 bzw. 362/362.
#   Gegenprobe: ohne die Zusatzdateien (Verhalten aaea5f8c2) fallen alle acht H-Szenen (50D0 K10 schon an "masks=0").
#   name raum cut x z band art x0 y0 x1 y1 [E] [A0=x,z]
set(R41M_SZENEN
    "h4090_02 4090 2 -22700 -24532 0 H 3 93 28 155"
    "h50D0_10 50D0 10 -13150 -15600 0 H 296 93 320 179"
    "h5100_07 5100 7 -14130 -2500 0 H 65 104 88 139"
    "h6000_01 6000 1 13533 -4984 2 H 294 80 316 143"
    "e4091_02 4090 2 -22700 -24532 0 H 3 93 28 155 E"
    "e50D1_10 50D0 10 -13150 -15600 0 H 296 93 320 179 E"
    "e5101_07 5100 7 -14130 -2500 0 H 65 104 88 139 E"
    "e6001_01 6000 1 13533 -4984 2 H 294 80 316 143 E"
    "v4090_02 4090 2 -17300 -17532 0 L 0 138 75 240 A0=0,0"
    "v50D0_10 50D0 10 -16350 -22100 0 L 108 56 168 240 A0=0,0"
    "x50D0_10 50D0 10 -19950 -16600 0 L 117 96 134 167 A0=0,0"
    "v5100_07 5100 7 -11230 -6400 0 L 3 96 67 240 A0=-26430,-26300"
    "v6000_01 6000 1 4533 -1184 2 L 192 47 261 240 A0=-4167,-22584"
    "x6000_01 6000 1 10633 -1184 2 L 212 78 234 153 A0=-4167,-22584"
)
