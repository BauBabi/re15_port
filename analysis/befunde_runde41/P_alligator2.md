# Spur P "alligator2" (Runde 41, Nachtrag 34 / Abschlussplan v0.8.30) - Dossier

Baum `.claude/worktrees/r41_alligator2`, Zweig `r41/alligator2`, Basis master f5a5e2f7e.

Punkte (Wortlaut AUFTRAG.md, massgeblich):
1. Nachtrag 31 Punkt 2: Kampfbeginn im Becken VOR Leon zu sehen, nicht direkt vor der Linse der laufenden Kamera (Cut 0);
   RE2-Band F-E2 (Abstand 11430..13924, erster Kontakt 121..158 Bilder, Zonen wie RE2) bleibt; Riegel Abstand zur Kamera / Bildanteil.
2. FG-E7 Option (a): Schluessel-Lagewahl sucht weiter, wenn die beste Zonenkamera < 10,4 px liefert; DISABLED der Riegel
   integration_r41_gatorszene_tod_r1/_elza entfernen, beide gruen.
3. Lagespruenge mit dem Kanister im Maul (Abnahme G 4 O1): Kopf-Klemme <-> Koerper-Klemme.
4. Riegel Uebergabe Ankunftsszene 2090 -> Knall + 8 Zombies 2080 ueber den echten Weg, ohne RE15_SET_FLAG=9:110, Leon + Elza.

## Stand
- [x] Punkt 1 Nachtrag 31.2 (Lage, Riegel) - [x] Punkt 2 FG-E7 (a) - [x] Punkt 3 Lagespruenge - [x] Punkt 4 Riegel F -> G
- Messordner (nicht im Repo): Scratchpad `P41/` (`runs/<lauf>`, `tools/`). Bildbelege: `P_belege/`.

## Punkt 1 - Nachtrag 31 Punkt 2 "nicht vor der Kamera"

### Messung vorher (exe, RE15_WINDOW_SCALE=1, Leiterweg, Rezept Abnahme F 5, Laeufe V_L1/V_E1)
Kampfbeginn F297, Cut 0, Gator-Wurzel (3197,-24407) Gier 1981, Leon (-8200,-25550), Abstand 11454, Biss +131, Treffer +138; Wurzel <-> Kamera Cut 0 (RID @0x0060: (2754,-3132,-26190))
1837, kleinster Wert 1755, kleinster Knochen-Abstand 610 (test_r41_alligator2 kamera). Vordergrund-Anteil rechte Bildhaelfte (Auswerter `bildanteil`, Mass anteil.py der Abnahme F 5): Median 45 %,
Spitze 57 %, > 25 %: 118 Bilder, > 50 %: 18 Bilder. Leon = Elza Bild fuer Bild. Bild P_belege/n31_vorher_leon.png.

### Beleg / Geometrie
- Band unveraendert (F-E2): Abstand 11430..13924 (SquareRoot0 11402..13890), Kontakt 121..158 (test_r41_gatorszene re2soll), Zonen: Gator und Leon Zone 0 (RE2 @0x80100990-ec,
  Port sce-8-Zonen ROOM2090 @0x009D6/@0x009EA/@0x009FE).
- Rechnung: Kamera Cut 0 Blick (-0,918, 0,395), halber Bildwinkel atan(160/208) = 37,6 Grad. Jeder Punkt im Bild von Cut 0 mit z <= -18100 (Zone 0) liegt hoechstens ~10000 von Leon -
  die WURZEL kann am Kampfbeginn im Band nicht im Bild liegen. Moeglich ist: Wurzel rechts ausserhalb, Kopf im Bild, weit weg von der Linse (Nordostteil des Suedbeckens).
- Die Wurzel kommt aus der Fress-Kette: Kampfbeginn = Kau-Lage + Schrei-Rueckstoss, Kau-Lage = Leiche + Kopf-Versatz, also sind Leichen-Lage + Anlauf-Gier der Freiheitsgrad
  (Nachtrag 6: "Du darfst auch die Startposition des Aligators und die Position der schwimmenden Zombie Leiche ... aendern").

### Umsetzung
- engine/src/gatorszene_r41.c, PORT-WAHL-Block: GSZ_LEICHE_X_STD 600, GSZ_LEICHE_Z_STD -24400, GSZ_LEICHE_ROT_Y_STD 2560, GSZ_GATOR_GIER_STD 1680, GSZ_KEY_AUF_RX_STD -640, GSZ_KEY_AUF_RZ_STD -384
  (Durchbruch 22 und Schluessel-Lokallage (1090,-420,-100) unveraendert; Nachtrag 31 Punkt 1 "So lassen").
- Wahl = Messung im echten Ablauf: test_r41_alligator2 lagesuche2 (Szene + Spur-B-Kamerawahl + 150 Bilder Kamera-Mass je Kandidat), Vorauswahl = Form leiche_lage_n5.py --kandidaten mit Kampfbeginn
  >= 3500 von der Kamera (Scratch tools/kand_p.py, Raster 100: 3437 Kandidaten). 1383 erfuellen Band + Zonen + Kontakt + frontal > 0 + Primitivfarbe >= 84/125; von den 62 mit Knochen-Abstand >= 2600 die mit
  dem hoechsten Schluessel-Kasten (10,4 x 12,6; zum Vergleich abgenommen 11,7 x 12,0).
- Neuer Pruefhaken re15_gatorszene_r41_key_kasten (Kasten des Schluessel-Quadrats, Weltmatrix wie der Prop-Zeichner).

### Messung nachher (exe, Laeufe D_L1/D_E1, Leon = Elza Bild fuer Bild; Riegel-Laeufe gleich)
| Mass | vorher | nachher | Soll |
|---|---|---|---|
| Kampfbeginn | F297 (3197,-24407) | F299 (3033,-21904) Gier 1843, Cut 0 | Zone 0 |
| Abstand Wurzel <-> Leon | 11454 (SquareRoot0 11448) | 11810 (SquareRoot0 11808) | 11402..13890 |
| erster Biss / Treffer | +131 / +138 | +137 / +144 | 121..158 |
| Clip-Folge | 0, 1 F407, 4 F428 | 0 F300, 1 F409, 4 F436 (kein Dash) | wie RE2 |
| Wurzel <-> Kamera (150 Bilder) | min 1755 | min 4007 | >= 3800 (Riegel) |
| Knochen <-> Kamera | min 610 | min 2684 | >= 2500 (Riegel) |
| rechte Bildhaelfte Median / > 50 % | 45 % / 18 Bilder | ~28 % / 0 Bilder | <= 35 % / 0 |
| Fress-Kamera | Cut 1 frontal 4116 | Cut 1 frontal 4005, Massstab 983 | frontal > 0 |
| Schluessel Primitivfarbe | 85 -> 126 | 86 -> 128 | >= 84 / >= 125, <= 128 |
| Schluessel im Bild (ring_a5) | hellster 76 .. 112, Ring ~7 x 9 | hellster bis 112, Ring ~8 x 7 (Spitzen 12 x 12) | Helligkeit Pflicht, Groesse vom Nutzer angenommen (31.3) |

Bilder P_belege/n31_nachher_leon.png, n31_nachher_elza.png (Kopf ab dem ersten Bild im Becken vor Leon, Rumpf ab +111 im Bild, kein Bild vor der Linse), n31_ankunft_schluessel.png,
n31_schluessel_zoom_neu_oben_alt_unten.png. Die Spitzen 40-45 % einzelner Bilder (F380-384, F394) sind das Flimmern des Wasserbodens, nicht der Gator (Nachbarbilder 27 %, dieselbe Pose).

## Punkt 2 - FG-E7 Option (a)

### Messung vorher
Riegel entsperrt, integration_r41_gatorszene_tod_r1/_elza rot: `bilddiff: 43 Bilder, 0 mit Schluessel-Unterschied` (Lauf T0). Ursache NICHT die Groesse: Schluessel (1200,-1699,-24800) Kamera 0, Tiefe 2304,
16 Bildpunkte, aber Leon (1325,-25550) steht 1570 vor der Kamera davor. K3-Wandlage (Rezept FG-E7, Laeufe K3/KW, mit Kopf-Klemme): nur Kamera 1, Tiefe 5918 = 6,3 Bildpunkte; Kamera 0
und 2 fehlten ganz, weil die Verdeckungsregel jede Knochen-Kugel an der Bildebene (0 < Tiefe <= 800) als "verdeckt immer" zaehlte, auch ausserhalb des Bildes.

### Beleg (Massstab)
10,4 Bildpunkte = Quadrat des Ankunfts-Schluessels der abgenommenen Fassung 1 (F-E2 Punkt 2, Riegel `_quad10 LESS 104`, Tiefe <= 3600 bei H = 208, RID @0x060). Alligator-Kasten
{1000,-720,0,2200,720,800} @0x80118b98 (Kugelradius 800 unveraendert). Kein Original-Vorbild fuer die Lagewahl: PORT-WAHL der Form (Spur F, Nachtrag 6).

### Umsetzung (engine/src/gatorszene_r41.c)
- key_lage_bei_tod: Suchstufen r_nah x (1 + n/4), n = 0..GSZ_SUCH_STUFEN 4, Abbruch sobald die beste Zonenkamera >= GSZ_KEY_BILD_MIN10 104 zeigt; Pruefhaken re15_gatorszene_r41_key_stufe, Messzeile "Schluessel-Suche".
- kugeln_verdeckt (ersetzt leiche_verdeckt): Tiefe auf den Radius geklemmt, eine Kugel an der Bildebene zaehlt nur, wenn ihr Bildkreis den Schluessel trifft.
- Spielfigur als Verdecker: spieler_punkte (fuenf Kugeln bis 3000 Hoehe, zwei am Arm), GSZ_SPIELER_RADIUS 500 (gemessen: 60 Bildpunkte hoch bei Tiefe 10309, Rumpf 125 Bildpunkte bei Tiefe 1390).
- GSZ_BILD_RAND 4 (die Mitte steigt mit der Kippung, Fall r1 x 24 -> 22).

### Messung nachher
- tod_r1/_elza: Kamera 2, Lage (4200,-1726,-23700), Tiefe 2729 = 13 Bildpunkte, 1173 von der Leiche, bilddiff gruen. DISABLED entfernt (probes/r41_gatorszene.cmake).
- K3 (Kopf-Klemme an F62..F83): Kamera 0, Lage (0,-1723,-26800), Tiefe 2606 = 14,3 Bildpunkte, 1639 von der Leiche, Stufe 0. KW: Kamera 0, Tiefe 2368, 1267 von der Leiche. Bild P_belege/k3_schluessel_cut0.png.
- Scan `test_r41_alligator2 schluessel_scan` (Raster 1000, vier Gieren, Leon Leiter + Suedbecken): 1688 Lagen, Stufe 0: 1595, 1: 48, 2: 33, 3: 12, 4: 0, unter 10,4: 0.

## Punkt 3 - Lagespruenge mit dem Kanister im Maul (Abnahme G 4 O1)

### Messung vorher
Abnahme G 4: v1_leon 1517 (F156), 37 Bildpaare > 400. Hier: `test_r41_alligator2 trag_scan` (echter Boss-Takt, Startlagen x 8 Gieren x 6 Leon-Lagen, je 900 Bilder), Stand vor der Aenderung (erste Fassung des Scans, 288 Laeufe): max 1546,
> 340: 946, > 400: 695, > 600: 208, > 1000: 68. Exe (Scratch tools/trage.sh, Rezept kampf + Schussbloecke ab F900, harte Klemme): Leon West max 446, Leon Nordost max 429 (5 Bildpaare > 340).
Vier Ursachen getrennt gemessen: (a) der ganze Ueberstand in EINEM Bild beim Anhaengen und Drehen (v1 1517); (b) die Kopf-Klemme schob die Wurzel in den Slab der Wurzel-Klemme, die warf sie im
naechsten Takt zurueck (F+43 (-1634,-18311) -> (-3184,-18307), 1546); (c) Biss-Clip 4: der Kopf holt bis 4400 aus, die Klemme schob 550 je Bild (F+207..F+210); (d) CROSS-Start mit dem Kanister: Sprung auf den Anlaufpunkt (bis 1425).

### Beleg
- Massstab 340 = groesster authored Wurzelschritt der Clips 6/3/0/1 x Skala 2731 (unit_r41_re2gator_lauer: Clip 6 340, 3 72, 0 104, 1 259) = Riegel integration_r41_re2gator "sichtbarer Lagesprung".
- RE2 hat keine Wandklemme fuer den Gator (Dossier G 11.3, Root @0x80100190-3a4); die Kopf-Klemme bleibt PORT-WAHL (G-E5), nur ihr Abbau aendert sich.
- R1: der Vortrieb beginnt mit -500 je Bild: `8010246c addiu v1,zero,-500` / `80102470 sh v1,324(s1)` (EM23_OVL_0000.BIN, selbst disassembliert) -> Klemme im R1 hoechstens 500 je Bild (GR2_KK_SCHRITT_R1).

### Umsetzung
- gator_re2_r41.c re15_gr2_kopf_klemme: Verschiebung je Bild auf 340 begrenzt (R1: 500). re15_gr2_kk_schritt: ganzer Bildschritt (Takt + Klemme) hoechstens 340.
- enemy_ai_boss_gator.c Haken (2b): beim Tragen dieselbe Klemme re15_collision_constrain_enemy wie (2), die Wurzel kommt nicht mehr in den Slab; fuehrt die Wurzel-Klemme um eine Ecke (> 340), bleibt die Wurzel in diesem Bild stehen.
- re15_gr2_r1_start: Rest-Ueberstand beim Schuss verschiebt nur den Kanister (er zerplatzt im selben Bild) - G-E5 "Kanister beim R1 innerhalb" bleibt; Lauf T_K_w: Kanister (-8900,-1918,-9351).
- gb_cross_begin / GBP_CROSS: mit dem Kanister beginnt die Bahn an der Lage des Gators, Wende 0x60 je Bild (Wert der GIER-WACHE) statt Snap.
- Pruefhaken re15_gr2_kk_hart_setzen (Gegenprobe der Riegel, kein Spielweg, kein Env-Schalter).

### Messung nachher
- trag_scan (306 Laeufe, 275400 Bilder, nur Kanister im Wasser): Sprung max 340, > 340: 0, > 400: 0, > 600: 0, > 1000: 0. Preis: Ueberstand ueber 300 in 1192 Bildern (0,43 %), max 2282 (Biss-Ausholen an der Wand, CROSS ohne Klemme) -
  dort ragt Kopf bzw. Kanister-Ende kurz in die Wand, statt zu springen.
- Riegel-Auswahl `tragen`: weich max 338, 0 Bildpaare > 340; Gegenprobe harte Klemme max 1005, 68 Bildpaare > 340.
- exe Lauf T_K_w: Aufnahme F114, R1 F1792, 1678 Bildpaare, max 338; T_N_w: F170..F919, max 338 (harte Klemme derselben Laeufe 446 / 429).

## Punkt 4 - Riegel Uebergabe Ankunftsszene -> Knall + 8 Zombies

Nur Riegel (keine Engine-Aenderung). integration_r41_alligator2_uebergabe / _elza: RE15_SET_FLAG nur 3:245 (Original-Rechner), kein 9:110 (Pruefung: keine Zeile `[setflag] flag(9,110`).
Eingabe je Spielbild: Aktionstaste an der Leiter in 2080 (Slot 1 @0x00BC2), Szene bis Folge C, Linksdrehung 21 Bilder zur Leiter, Aktionstaste an der Tuer Slot 0 @0x0996 (DOOR16).
Gemessen Leon und Elza: Folge DACDKNZ (D Tuerweg, A Ankunft, C Kampfbeginn, D Tuerweg zurueck, K Knall, N "nach Bit 110: 8 lebende Zombies", Z Zombies), 8 Sce_em_set Typ 0x16 behavior 0x02 alle y -14400, Knall-Stimme
`se=0 layer=0 vag=0 note=67 fine=0 center=91` direkt hinter der Ausloese-Zeile. Vorher kein Riegel (UEBERGABE 2026-10-26 02:33).
