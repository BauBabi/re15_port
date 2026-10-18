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

/* SCHLUESSEL PARK (Item 0x45, RE2-MD1 md5 3c9176ee9bbb7e91b7c9433d6fe5c371): s = 1.1992 -> sichtbar 136 x 87 Einheiten;
 * Sichtseite Modell +1y, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x 137.40..139.61 y 80.78..82.20. */
#define RE15_R38K_PARK_X        (1358)
#define RE15_R38K_PARK_Y        (-2495)
#define RE15_R38K_PARK_Z        (-3400)
#define RE15_R38K_PARK_ROT_X    3072
#define RE15_R38K_PARK_ROT_Y    0
#define RE15_R38K_PARK_ROT_Z    2048
#define RE15_R38K_PARK_S10000   11992   /* Massstab x 10000 (Sonden/Riegel); Runde 39 Spur M: 0,5 x Spur L */
/* Runde 39 Spur L (kleiner/subtiler): Bildlaenge der gezeichneten Huelle = oberes Quartil (75. Perzentil, linear interpoliert) der RE2-Bildlaenge
 * L_nat*H/vz desselben Modells an seinen 2 RE2-Kameras (ROOM6150 C8 4.84, ROOM6150 C9 3.30; Median 4.07);
 * Runde-38-Massstab 6.4636 fuellte die Marke (11.94 px breit). */
#define RE15_R39L_PARK_S38_10000    64636   /* Runde-38-Massstab x 10000 (Marke gefuellt) */
#define RE15_R39L_PARK_ZIEL_PX100   445   /* RE2 oberes Quartil der Bildlaenge, Pixel x 100 */
#define RE15_R39L_PARK_MEDIAN_PX100 407   /* RE2-Median der Bildlaenge, Pixel x 100 (nur zur Einordnung) */
#define RE15_R39L_PARK_HUELLE_PX100 446   /* gezeichnete Huelle im Nutzer-Cut beim Spur-L-Massstab, Pixel x 100 */
#define RE15_R39L_PARK_RE2_KAMERAS  2
#define RE15_R39L_PARK_LNAT100      11370   /* sichtbare Modell-Laenge bei s = 1, Einheiten x 100 */
/* Runde 39 Spur M (NUTZER-VORGABE Nachtrag 5 "maximal halb so gross", 0,5 x RE2-Quartil-Groesse = Spur L): groesster
 * Massstab, bei dem die gezeichnete Huelle in Breite UND Hoehe hoechstens RE15_R39M_FAKTOR100/100 x die Spur-L-Huelle ist. */
#define RE15_R39M_PARK_SL_10000          24100   /* Spur-L-Massstab x 10000 (Basis) */
#define RE15_R39M_PARK_L_BREITE_PX1000   4461   /* gezeichnete Huelle Spur L, Breite, Pixel x 1000 */
#define RE15_R39M_PARK_L_HOEHE_PX1000    2985   /* gezeichnete Huelle Spur L, Hoehe, Pixel x 1000 */
#define RE15_R39M_PARK_BREITE_PX1000     2212   /* gezeichnete Huelle (geliefert), Breite, Pixel x 1000 */
#define RE15_R39M_PARK_HOEHE_PX1000      1485   /* gezeichnete Huelle (geliefert), Hoehe, Pixel x 1000 */

/* SCHLUESSEL PRISON (Item 0x42, RE2-MD1 md5 c5d49ef60b6b111ecac53a19652f3278): s = 1.2809 -> sichtbar 178 x 83 Einheiten;
 * Sichtseite Modell +1y, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x 137.07..139.94 y 80.82..82.17. */
#define RE15_R38K_PRISON_X        (1358)
#define RE15_R38K_PRISON_Y        (-2495)
#define RE15_R38K_PRISON_Z        (-3400)
#define RE15_R38K_PRISON_ROT_X    1024
#define RE15_R38K_PRISON_ROT_Y    0
#define RE15_R38K_PRISON_ROT_Z    0
#define RE15_R38K_PRISON_S10000   12809   /* Massstab x 10000 (Sonden/Riegel); Runde 39 Spur M: 0,5 x Spur L */
/* Runde 39 Spur L (kleiner/subtiler): Bildlaenge der gezeichneten Huelle = oberes Quartil (75. Perzentil, linear interpoliert) der RE2-Bildlaenge
 * L_nat*H/vz desselben Modells an seinen 11 RE2-Kameras (ROOM10F0 C0 3.62, ROOM10F0 C1 4.45, ROOM20E0 C0 4.74, ROOM20E0 C1 4.03, ROOM20E0 C4 2.90, ROOM20E0 C3 7.14, ROOM20E0 C5 9.00, ROOM20E0 C6 6.82, ROOM2020 C1 2.67, ROOM2020 C5 2.28, ROOM2020 C6 4.26; Median 4.26);
 * Runde-38-Massstab 5.3032 fuellte die Marke (11.96 px breit). */
#define RE15_R39L_PRISON_S38_10000    53032   /* Runde-38-Massstab x 10000 (Marke gefuellt) */
#define RE15_R39L_PRISON_ZIEL_PX100   578   /* RE2 oberes Quartil der Bildlaenge, Pixel x 100 */
#define RE15_R39L_PRISON_MEDIAN_PX100 426   /* RE2-Median der Bildlaenge, Pixel x 100 (nur zur Einordnung) */
#define RE15_R39L_PRISON_HUELLE_PX100 577   /* gezeichnete Huelle im Nutzer-Cut beim Spur-L-Massstab, Pixel x 100 */
#define RE15_R39L_PRISON_RE2_KAMERAS  11
#define RE15_R39L_PRISON_LNAT100      13883   /* sichtbare Modell-Laenge bei s = 1, Einheiten x 100 */
/* Runde 39 Spur M (NUTZER-VORGABE Nachtrag 5 "maximal halb so gross", 0,5 x RE2-Quartil-Groesse = Spur L): groesster
 * Massstab, bei dem die gezeichnete Huelle in Breite UND Hoehe hoechstens RE15_R39M_FAKTOR100/100 x die Spur-L-Huelle ist. */
#define RE15_R39M_PRISON_SL_10000          25623   /* Spur-L-Massstab x 10000 (Basis) */
#define RE15_R39M_PRISON_L_BREITE_PX1000   5770   /* gezeichnete Huelle Spur L, Breite, Pixel x 1000 */
#define RE15_R39M_PRISON_L_HOEHE_PX1000    2868   /* gezeichnete Huelle Spur L, Hoehe, Pixel x 1000 */
#define RE15_R39M_PRISON_BREITE_PX1000     2878   /* gezeichnete Huelle (geliefert), Breite, Pixel x 1000 */
#define RE15_R39M_PRISON_HOEHE_PX1000      1429   /* gezeichnete Huelle (geliefert), Hoehe, Pixel x 1000 */

#define RE15_R39L_RE2_QUANTIL100  75   /* Perzentil der RE2-Bildlaengen (oberes Quartil), x 100 */
#define RE15_R39M_FAKTOR100       50   /* NUTZER-VORGABE Nachtrag 5 ("maximal halb so gross"), 0,5 x Spur L, x 100 - kein RE-Wert */

/* Aufhebe-Rechteck der Schluessel (beide Varianten derselbe Ort): 1000 x 1000 um (x_Marke, Wand -3400 + 300),
 * rechts vor Slot 12 (x ab 1800) beendet. */
#define RE15_R38K_KS_RECT_X   (798)
#define RE15_R38K_KS_RECT_Z   (-3600)
#define RE15_R38K_KS_RECT_W   1000
#define RE15_R38K_KS_RECT_D   1000

