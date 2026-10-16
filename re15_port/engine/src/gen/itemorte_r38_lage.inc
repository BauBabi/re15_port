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

/* SCHLUESSEL PARK (Item 0x45, RE2-MD1 md5 3c9176ee9bbb7e91b7c9433d6fe5c371): s = 6.4636 -> sichtbar 735 x 468 Einheiten;
 * Sichtseite Modell +1y, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x 132.50..144.46 y 77.63..85.27. */
#define RE15_R38K_PARK_X        (1358)
#define RE15_R38K_PARK_Y        (-2495)
#define RE15_R38K_PARK_Z        (-3400)
#define RE15_R38K_PARK_ROT_X    3072
#define RE15_R38K_PARK_ROT_Y    0
#define RE15_R38K_PARK_ROT_Z    2048
#define RE15_R38K_PARK_S10000   64636   /* Massstab x 10000 (Sonden/Riegel) */

/* SCHLUESSEL PRISON (Item 0x42, RE2-MD1 md5 c5d49ef60b6b111ecac53a19652f3278): s = 5.3297 -> sichtbar 740 x 347 Einheiten;
 * Sichtseite Modell +1y, Textur-u nach Bild-rechts; Huelle im Nutzer-Cut x 132.50..144.45 y 78.66..84.29. */
#define RE15_R38K_PRISON_X        (1358)
#define RE15_R38K_PRISON_Y        (-2495)
#define RE15_R38K_PRISON_Z        (-3400)
#define RE15_R38K_PRISON_ROT_X    1024
#define RE15_R38K_PRISON_ROT_Y    0
#define RE15_R38K_PRISON_ROT_Z    0
#define RE15_R38K_PRISON_S10000   53297   /* Massstab x 10000 (Sonden/Riegel) */

/* Aufhebe-Rechteck der Schluessel (beide Varianten derselbe Ort): 1000 x 1000 um (x_Marke, Wand -3400 + 300),
 * rechts vor Slot 12 (x ab 1800) beendet. */
#define RE15_R38K_KS_RECT_X   (798)
#define RE15_R38K_KS_RECT_Z   (-3600)
#define RE15_R38K_KS_RECT_W   1000
#define RE15_R38K_KS_RECT_D   1000

