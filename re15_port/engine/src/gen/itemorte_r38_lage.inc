/* AUTOMATISCH ERZEUGT von re15_port/tools/r38_itemorte/itemorte_export.py --schreiben - nicht von Hand.
 * Runde 38 Spur K (Dossier analysis/befunde_runde38/K_itemorte.md). Herleitung jeder Zahl: Kopf des Skripts.
 * Nutzer-Marken: fire_extinguisher.bmp x38..48 y111..122 (ROOM1030 Cut 0), key_location.bmp x133..143 y77..85
 * (ROOM10F0 Cut 5). */

/* FEUERLOESCHER ROOM1030 (stehendes Netz Runde 37: Hoehe 792, Radius 110): Pfeiler -x-Flaeche x -2688, Achse x -2813
 * (R + 15), Mitte auf dem Sehstrahl der Markenmitte (-2813,-2518,-17904); Huelle im Nutzer-Cut x 40.9..46.3 y 109.6..124.4.
 * Aufhebe-Rechteck: Mitte x -3480 = Klemmgrenze -4100 + 620 voraus, z ab -18370 (Slot 13 endet bei -18372). */
#define RE15_R38K_FL_X        (-2813)
#define RE15_R38K_FL_Y        (-2122)
#define RE15_R38K_FL_Z        (-17904)
#define RE15_R38K_FL_ROT_Y    3072
#define RE15_R38K_FL_RECT_X   (-3980)
#define RE15_R38K_FL_RECT_Z   (-18370)
#define RE15_R38K_FL_RECT_W   1000
#define RE15_R38K_FL_RECT_D   1000

/* SCHLUESSEL PARK (Item 0x45, RE2-MD1 md5 3c9176ee9bbb7e91b7c9433d6fe5c371): s = 0.8063 -> sichtbar 92 x 58 Einheiten;
 * Sichtseite Modell +1y, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x 137.76..139.25 y 81.01..81.97. */
#define RE15_R38K_PARK_X        (1358)
#define RE15_R38K_PARK_Y        (-2495)
#define RE15_R38K_PARK_Z        (-3400)
#define RE15_R38K_PARK_ROT_X    3072
#define RE15_R38K_PARK_ROT_Y    0
#define RE15_R38K_PARK_ROT_Z    2048
#define RE15_R38K_PARK_S10000   8063   /* Massstab x 10000 (Sonden/Riegel); Runde 39 Spur M: hoechstens 0,5 x Spur L */
/* Runde 39 Spur L (kleiner/subtiler): Bildlaenge der gezeichneten Huelle = oberes Quartil (75. Perzentil, linear interpoliert) der RE2-Bildlaenge
 * L_nat*H/vz desselben Modells an seinen 2 RE2-Kameras (ROOM6150 C8 4.84, ROOM6150 C9 3.30; Median 4.07);
 * Runde-38-Massstab 6.4636 fuellte die Marke (11.94 px breit). */
#define RE15_R39L_PARK_S38_10000    64636   /* Runde-38-Massstab x 10000 (Marke gefuellt) */
#define RE15_R39L_PARK_ZIEL_PX100   445   /* RE2 oberes Quartil der Bildlaenge, Pixel x 100 */
#define RE15_R39L_PARK_MEDIAN_PX100 407   /* RE2-Median der Bildlaenge, Pixel x 100 (nur zur Einordnung) */
#define RE15_R39L_PARK_HUELLE_PX100 446   /* gezeichnete Huelle im Nutzer-Cut beim Spur-L-Massstab, Pixel x 100 */
#define RE15_R39L_PARK_RE2_KAMERAS  2
#define RE15_R39L_PARK_LNAT100      11370   /* sichtbare Modell-Laenge bei s = 1, Einheiten x 100 */
/* Runde 39 Spur M (NUTZER-VORGABE Nachtrag 5 "maximal halb so gross", 0,5 x RE2-Quartil-Groesse = Spur L): das groesste
 * Bild, dessen gezeichnete Huelle UND GTE-Viereck in Breite und Hoehe hoechstens RE15_R39M_FAKTOR100/100 x Spur L sind;
 * geliefert die Mitte des Massstab-Intervalls gleicher ganzzahliger Ecken (S_UNTEN..S_OBEN). */
#define RE15_R39M_PARK_SL_10000          24100   /* Spur-L-Massstab x 10000 (Basis) */
#define RE15_R39M_PARK_L_BREITE_PX1000   4461   /* gezeichnete Huelle Spur L, Breite, Pixel x 1000 */
#define RE15_R39M_PARK_L_HOEHE_PX1000    2985   /* gezeichnete Huelle Spur L, Hoehe, Pixel x 1000 */
#define RE15_R39M_PARK_BREITE_PX1000     1483   /* gezeichnete Huelle (geliefert), Breite, Pixel x 1000 */
#define RE15_R39M_PARK_HOEHE_PX1000      995   /* gezeichnete Huelle (geliefert), Hoehe, Pixel x 1000 */
/* GTE-Raster (FUN_80022f0c RTPT @0x80022fb0, ganzzahlige SXY ins Polygon @0x80022fd8): Viereck der Bildschirm-Ecken
 * im Nutzer-Cut, Spur L (135,82) (135,79) (141,82) (141,79) (138,81), geliefert (137,81) (137,80) (139,81) (139,80) (138,81); bindende Grenze darueber: GTE-Raster. */
#define RE15_R39M_PARK_L_GTE_X0  135
#define RE15_R39M_PARK_L_GTE_Y0  79
#define RE15_R39M_PARK_L_GTE_B   6   /* Spur L: Breite des GTE-Vierecks, Pixel (320er Raster) */
#define RE15_R39M_PARK_L_GTE_H   3   /* Spur L: Hoehe */
#define RE15_R39M_PARK_GTE_X0    137
#define RE15_R39M_PARK_GTE_Y0    80
#define RE15_R39M_PARK_GTE_B     2   /* geliefert: Breite des GTE-Vierecks */
#define RE15_R39M_PARK_GTE_H     1   /* geliefert: Hoehe */
#define RE15_R39M_PARK_S_UNTEN_10000  7195   /* Massstab-Intervall gleicher ganzzahliger Ecken, unten (x 10000) */
#define RE15_R39M_PARK_S_OBEN_10000   8931   /* ... oben (groesster Massstab, der beide Grenzen haelt) */
#define RE15_R39M_PARK_MX100000   -99687   /* Mitte des sichtbaren Rechtecks im RE2-Modell (x), Einheiten x 100000 */
#define RE15_R39M_PARK_MZ100000   160897   /* ... (z) */

/* SCHLUESSEL PRISON (Item 0x42, RE2-MD1 md5 c5d49ef60b6b111ecac53a19652f3278): s = 1.2073 -> sichtbar 168 x 79 Einheiten;
 * Sichtseite Modell +1y, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x 137.15..139.85 y 80.86..82.13. */
#define RE15_R38K_PRISON_X        (1358)
#define RE15_R38K_PRISON_Y        (-2495)
#define RE15_R38K_PRISON_Z        (-3400)
#define RE15_R38K_PRISON_ROT_X    1024
#define RE15_R38K_PRISON_ROT_Y    0
#define RE15_R38K_PRISON_ROT_Z    0
#define RE15_R38K_PRISON_S10000   12073   /* Massstab x 10000 (Sonden/Riegel); Runde 39 Spur M: hoechstens 0,5 x Spur L */
/* Runde 39 Spur L (kleiner/subtiler): Bildlaenge der gezeichneten Huelle = oberes Quartil (75. Perzentil, linear interpoliert) der RE2-Bildlaenge
 * L_nat*H/vz desselben Modells an seinen 11 RE2-Kameras (ROOM10F0 C0 3.62, ROOM10F0 C1 4.45, ROOM20E0 C0 4.74, ROOM20E0 C1 4.03, ROOM20E0 C4 2.90, ROOM20E0 C3 7.14, ROOM20E0 C5 9.00, ROOM20E0 C6 6.82, ROOM2020 C1 2.67, ROOM2020 C5 2.28, ROOM2020 C6 4.26; Median 4.26);
 * Runde-38-Massstab 5.3032 fuellte die Marke (11.96 px breit). */
#define RE15_R39L_PRISON_S38_10000    53032   /* Runde-38-Massstab x 10000 (Marke gefuellt) */
#define RE15_R39L_PRISON_ZIEL_PX100   578   /* RE2 oberes Quartil der Bildlaenge, Pixel x 100 */
#define RE15_R39L_PRISON_MEDIAN_PX100 426   /* RE2-Median der Bildlaenge, Pixel x 100 (nur zur Einordnung) */
#define RE15_R39L_PRISON_HUELLE_PX100 577   /* gezeichnete Huelle im Nutzer-Cut beim Spur-L-Massstab, Pixel x 100 */
#define RE15_R39L_PRISON_RE2_KAMERAS  11
#define RE15_R39L_PRISON_LNAT100      13883   /* sichtbare Modell-Laenge bei s = 1, Einheiten x 100 */
/* Runde 39 Spur M (NUTZER-VORGABE Nachtrag 5 "maximal halb so gross", 0,5 x RE2-Quartil-Groesse = Spur L): das groesste
 * Bild, dessen gezeichnete Huelle UND GTE-Viereck in Breite und Hoehe hoechstens RE15_R39M_FAKTOR100/100 x Spur L sind;
 * geliefert die Mitte des Massstab-Intervalls gleicher ganzzahliger Ecken (S_UNTEN..S_OBEN). */
#define RE15_R39M_PRISON_SL_10000          25623   /* Spur-L-Massstab x 10000 (Basis) */
#define RE15_R39M_PRISON_L_BREITE_PX1000   5770   /* gezeichnete Huelle Spur L, Breite, Pixel x 1000 */
#define RE15_R39M_PRISON_L_HOEHE_PX1000    2868   /* gezeichnete Huelle Spur L, Hoehe, Pixel x 1000 */
#define RE15_R39M_PRISON_BREITE_PX1000     2716   /* gezeichnete Huelle (geliefert), Breite, Pixel x 1000 */
#define RE15_R39M_PRISON_HOEHE_PX1000      1338   /* gezeichnete Huelle (geliefert), Hoehe, Pixel x 1000 */
/* GTE-Raster (FUN_80022f0c RTPT @0x80022fb0, ganzzahlige SXY ins Polygon @0x80022fd8): Viereck der Bildschirm-Ecken
 * im Nutzer-Cut, Spur L (141,79) (141,83) (135,79) (135,83) (138,81), geliefert (139,80) (139,82) (136,80) (136,82) (138,81); bindende Grenze darueber: stetig. */
#define RE15_R39M_PRISON_L_GTE_X0  135
#define RE15_R39M_PRISON_L_GTE_Y0  79
#define RE15_R39M_PRISON_L_GTE_B   6   /* Spur L: Breite des GTE-Vierecks, Pixel (320er Raster) */
#define RE15_R39M_PRISON_L_GTE_H   4   /* Spur L: Hoehe */
#define RE15_R39M_PRISON_GTE_X0    136
#define RE15_R39M_PRISON_GTE_Y0    80
#define RE15_R39M_PRISON_GTE_B     3   /* geliefert: Breite des GTE-Vierecks */
#define RE15_R39M_PRISON_GTE_H     2   /* geliefert: Hoehe */
#define RE15_R39M_PRISON_S_UNTEN_10000  11337   /* Massstab-Intervall gleicher ganzzahliger Ecken, unten (x 10000) */
#define RE15_R39M_PRISON_S_OBEN_10000   12809   /* ... oben (groesster Massstab, der beide Grenzen haelt) */
#define RE15_R39M_PRISON_MX100000   -558472   /* Mitte des sichtbaren Rechtecks im RE2-Modell (x), Einheiten x 100000 */
#define RE15_R39M_PRISON_MZ100000   201017   /* ... (z) */

#define RE15_R39L_RE2_QUANTIL100  75   /* Perzentil der RE2-Bildlaengen (oberes Quartil), x 100 */
#define RE15_R39M_FAKTOR100       50   /* NUTZER-VORGABE Nachtrag 5 ("maximal halb so gross"), 0,5 x Spur L, x 100 - kein RE-Wert */

/* Aufhebe-Rechteck der Schluessel (beide Varianten derselbe Ort): 1000 x 1000 um (x_Marke, Wand -3400 + 300),
 * rechts vor Slot 12 (x ab 1800) beendet. */
#define RE15_R38K_KS_RECT_X   (798)
#define RE15_R38K_KS_RECT_Z   (-3600)
#define RE15_R38K_KS_RECT_W   1000
#define RE15_R38K_KS_RECT_D   1000

