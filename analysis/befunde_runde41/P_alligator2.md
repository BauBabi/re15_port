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
- [x] Nachbesserung 1 (Abnahme 0): Nachtrag 35 Punkte 1..6 (M1..M8), s. Abschnitt "Nachbesserung 1"
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
| rechte Bildhaelfte Median / Spitze / > 25 % / > 50 % | 45 % / 57 % / 118 / 18 Bilder | 26 % / 45 % / 86 / 0 Bilder | Median <= 35 %, 0 Bilder > 50 % |
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

## Tests

Neu (tests/unit/probes/r41_alligator2.cmake, tests/unit/test_r41_alligator2.c, tests/integration/test_r41_alligator2.cmake; der Harness ist test_r41_gatorszene.c, per #include):
| Riegel | misst | Ergebnis |
|---|---|---|
| unit_r41_alligator2_kamera, _kamera_elza | Abstand Wurzel/Knochen zur Kamera Cut 0 ueber 150 Bilder, Gator ab Bild 0 im Bild; Gegenprobe Lage der Nachbesserung 5 (Wurzel 1755, Teil 610 fallen durch) | gruen |
| integration_r41_alligator2_kamera, _kamera_elza | exe: Bildanteil rechte Bildhaelfte gegen Median-Hintergrund (Median <= 35 %, kein Bild > 50 %), cam=0, Wurzel >= 3800; Gegenprobe von Hand: der Auswerter faellt auf dem Vorher-Lauf V_L1 durch (Median 45 %, 18 Bilder > 50 %) | gruen |
| unit_r41_alligator2_schluessel | FG-E7 (a): Westkanal-Lage sucht weiter (Stufe 2), Becken-Scan nie unter 10,4 | gruen |
| integration_r41_alligator2_k3, _k3_elza | exe: K3-Wandlage mit Kopf-Klemme, Quadrat >= 10,4, bilddiff in jedem Bild der Einstellung, Kasten >= 10 x 9 | gruen |
| integration_r41_gatorszene_tod_r1, _tod_r1_elza | DISABLED entfernt | gruen |
| unit_r41_alligator2_tragen | echter Boss-Takt mit Kanister: Bildschritt <= 340; Gegenprobe harte Klemme (max 1005, 68 Bildpaare > 340) | gruen |
| integration_r41_alligator2_tragen, _tragen_elza | exe: Wurzelschritt <= 340 zwischen Aufnahme und R1, Kanister beim R1 innerhalb der Aussenwaende | gruen |
| integration_r41_alligator2_uebergabe, _uebergabe_elza | exe: Leiter -> Szene -> Leiter -> Knall -> 8 Zombies oben, ohne RE15_SET_FLAG=9:110 | gruen |

Nachgezogen mit Beleg: unit_r41_gatorszene_leiche (neue Lage), unit_r41_gatorszene_tod (Verdeckung wie die Engine, Naehe bis zum Doppelten), unit_r41_gatorszene_tod_explosion (K = X = 0 moeglich; K != X bleibt im exe-Riegel),
unit_r41_kanister2080_kopfklemme (Ueberstand wird ueber bis zu 9 Bilder abgebaut, Schritt <= 340).

Gezielte Laeufe (keine volle Suite):
1. Code-Stand ffb02351a (danach nur Dossier, Werkzeug, Kommentare), Regex = alle Testdateien, die gatorszene/re15_gr2_/GB_TEST/GATOR_KANISTER oder 2090/2091 nennen (Abnahme F 5) + Kanister + Spur P:
   `ctest --test-dir "<baum>/re15_port/build" -R "<regex unten>" -j4 --timeout 1200 --output-on-failure`
   -> **`99% tests passed, 1 tests failed out of 256`** (`Total Test time (real) = 2928.36 sec`). Rot nur `529 - integration_r35_granate`: Teil birkin (ROOM5090) `Slot 2: Typ 30 grid 10 im Bild 399 (soll 36 / 13)`.
   VORBESTEHEND: derselbe Riegel faellt auf der Basis f5a5e2f7e mit derselben Zeile durch (`0% tests passed, 1 tests failed out of 1`, Baum kurz auf die Basis gesetzt und neu gebaut), die Teile wand/zombie/gator sind gruen. Nicht Spur P (Birkin-Zweig 2341475d0 ff.).
2. Endstand: `ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_alligator2_|unit_r41_gatorszene_|unit_r41_kanister2080_|unit_r41_re2gator_|integration_r41_alligator2_|integration_r41_gatorszene_tod)" -j4 --timeout 600 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 68`** (`Total Test time (real) = 463.01 sec`).

Regex Lauf 1: `^(unit_r41_gatorszene_|integration_r41_gatorszene_|unit_r41_re2gator_|integration_r41_re2gator$|unit_r36_kartenbits$|unit_r26_inventar$|unit_r21_discard_wegwerfen$|unit_r39_inventar_|integration_r39_inventar_discard$|unit_item_|unit_inv_|unit_sca_wall_hit$|unit_re2_spider_|unit_spider_ai$|unit_adult_spider_ai$|unit_r38_spinnen$|integration_r38_spinnen_echtlauf$|unit_writher_ai$|unit_enemy_shootable$|unit_collision_layers$|unit_alligator_ai$|unit_gator_sweep$|unit_gator_fress$|unit_r36_schluessel_|integration_r36_schluessel|unit_r38_dining1141_(schluessel1011|tuer1131)$|unit_r38_rolltor20a0|integration_r38_rolltor20a0|unit_r37_feuerloescher_|integration_r37_feuerloescher$|unit_r39_optik_schluessel$|unit_r39_schluessel2_|unit_aot_sce_census$|unit_aot_edge$|unit_scd_|unit_door_seq$|unit_r38_wasser_|integration_r38_wasser$|unit_r41_kleinkram_a$|integration_r41_kleinkram_a_blink$|r31_trefferboxen|unit_rotor_bgm_pin$|unit_r35_cut10f0_bgm$|unit_r39_johnruf1041_musik$|unit_r35_cut1150_knallbank$|unit_r36_szene1150_knall_re2$|integration_r39_elzazombies_knall$|unit_r36_inventar$|unit_r36_kartenitems$|unit_r37_itemband$|integration_r37_itemband$|unit_r36_tueren_|unit_r33_tueren_|unit_r30_tuer|unit_r31_tuer_ton$|unit_r35_cut10f0_tuerton$|unit_r39_karten_tueren$|integration_r36_tueren$|unit_camera_view$|unit_cut_old_displayed$|unit_r34n_b_cut_old$|prop_zone_dispatch$|prop_scd_pc_advance$|unit_r38_montage_kameras$|integration_(cross_stage|room_probe_sweep|rvd_coverage|door_traversal|door_lock|item_name_census|keydoor)$|prop_door_roundtrip$|prop_spawn_entered$|integration_r35_granate$|integration_r41_wasser20b0_npc$|integration_r38_wasserzombie$|unit_r35_granate$|unit_irons_mittelmodell$|unit_r34_reaktion$|unit_1210_arme$|unit_r30_irons_tisch_licht$|unit_r35_affen_|unit_r36_szenenleon_|unit_r36_ziel1190$|unit_r37_pri1100_|unit_r38_npckante_|unit_r38_wasserzombie$|unit_r41_wasser20b0_|integration_r36_kartenitems_bild$|unit_r41_kanister2080_|integration_r41_kanister2080|unit_r41_alligator2_|integration_r41_alligator2_)`

Messzeilen Endstand (ctest -V): kamera/_elza Median 26 %, Spitze 45 %, > 25 %: 86, > 50 %: 0, Wurzel-Abstand min 4008; k3/_elza Kamera 0, Tiefe 2606 = 14,3 Bildpunkte, Kasten 12 x 13, hellster 75 .. 111 (x1,48), 44/44 Bilder; tragen F114..F1792 max 338, Kanister beim R1 (-8900,-9351); tragen_elza F170..F919 max 338, Kanister (6864,-9489).

## Nachbesserung 1 (Abnahme 0, Nachtrag 35)

Massgeblich: AUFTRAG.md Nachtrag 35 (Alligator-Befunde des Nutzers, 2026-10-30 16:46), sechs Punkte, Punkt 1 ist Pflicht fuer das Release. Die erste Fertigmeldung nannte sie nicht (M8); dieser Abschnitt arbeitet alle sechs ab.
Messordner (nicht im Repo): Scratchpad `P41n/` (`runs/<lauf>`, `lauf.sh`, `kampf.sh`, `rid2090.py`, `rid40a0.py`). RE2-Bytes selbst gelesen mit re2_disasm.py (EM23_OVL_0000.BIN, PSX.EXE), SCD ROOM40A0 mit dem Walker der Spur B.

### M1 (BLOCKER, N35.1 + N35.3): die Aktionstaste kletterte auf den Schluessel
- Ursache: key_prop legte obj 4 mit Band 0 an. Die Klettersonde FUN_8002d2c0 prueft nur Bit 0x101 und das Band, die Null-Box trifft FUN_8002da4c bei Rand -112 aus jeder Stellung mit |dx|, |dz| >= 112. Dasselbe erklaert "ich konnte auf den toten Alligator klettern": Elza stand auf y -1800 ueber dem Kadaver (Abnahme 0 k3Er), der Koerper selbst ist nicht erkletterbar.
- Messung vorher (Abnahme 0): k3Lr/k3Er `sub=9 kind=0x81 obj=4`, 56,1 % der Stellungen ueber 1688 Lagen.
- Beleg: `8002d364 lbu v1,0(s3)`, `8002d368 lbu v0,130(s0)`, `8002d370 bne v1,v0,0x8002d3a4` (Band-Gate). ROOM2090 selbst: obj 2 @0x00A6C `2d 02 00 00 02 00 0a 00`, obj 3 @0x00A8E `2d 03 00 00 02 00 0a 00` = pc[4] 2 fuer die schwimmenden Objekte, Obj_model_set legt es nach pool+130 (@0x8004096c-74). Zensus Runde 30: 100 von 121 Item-Props Band 1 = "Band ueber dem Boden".
- Aenderung: GSZ_KEY_BAND 2 (gatorszene_r41.c). Band 1 waere auf der Plattform (Spielerband 1) derselbe Fehler; die SCA von ROOM2090 hat nur die Baender 0 und 1.
- Messung nachher: Lauf k3r, dieselbe Eingabe und Stellung (-391,-26408) Blick 15: 0 x `sub=9`, itemband `F297 room=2090 ITEM slot 15 floor 0 item 0x34 tk 112 spieler-band 0 spieler=(-391,0,-26408) -> FREI`. Scan 1688 Todeslagen, 4 704 256 Stellungen: Klettern 0. Gegenprobe im Riegel (Band 0 von Hand): 1024 von 3600 Stellungen.

### Nebenbefund zu N35.1: der Schluessel lag bei 202 von 1688 Lagen IN der Wand
- Die Wasserzonen decken auch den Maschinenblock (x -2000..2300, z -8300..-6800) ab; key_suche pruefte nur Wasser und Bild. Das ist die Nutzer-Beobachtung "ob der in der Wand haengen geblieben ist". Karte: Abnahme p0_karte.
- Aenderung: key_begehbar (Spieler-Klemme FUN_8003b0a4 mit Radius 450, Maske 1, Band 0 fest, Objekt-Kasten) in key_suche; der Rueckfall ohne Kamera nimmt den naechsten begehbaren Wasserpunkt am Kopf.
- Messung nachher: `test_r41_alligator2 erreichbar_scan 1000`: 1688 Lagen, nicht erreicht 0 (Flutfuellung ab der Leiter, 24933 Zellen Raster 100).

### M2 (BLOCKER, N35.1 + N35.3): der Kadaver sperrte den Westkanal
- Ursache: RE2 nimmt dem Alligator mit dem Explosionstod jede Koerper-Kollision; der Port liess den Koerperlaengen-Push (k_gb_seg) und re15_body_push in GBP_RE2_TOD an.
- Messung vorher (Abnahme 0): Elza stehen bei (-7143,-19051), (-8432,-18453), (-2168,-19264), Leon bei (-2168,-19677); Schluessel und Leiter unerreichbar in kl/ke/we.
- Beleg RE2: R1 P0 `801022ec lw v0,0(s1)`, `801022f8 ori v0,v0,0x80a`, `801022fc sw v0,0(s1)`. FUN_80034d0c (einziger Spieler-Push, FUN_800355c4 aus dem Spieler-Tick) beginnt mit `((*param_1 | *param_2) & 2) == 0`. P3 `8010265c ori v0,v0,0x4000`, `80102664 sh v0,270(s1)`: FUN_8003db38 (Ziel) und FUN_800410cc (Schussprobe) fragen `(+0x10E & 0xc000) == 0`. Die Leiche ist in RE2 Kulisse. Zur Klettersonde: FUN_8002d474 fragt nur Objekte und SCA, nie Aktoren.
- Aenderung: re15_gator_boss_ohne_koerper (Phase GBP_RE2_TOD) sperrt beide Pushs (Haken enemy_ai_common.c re15_body_push_player, Gate Koerperlaengen-Push); R1 P3 setzt hit_radius 0 (keine Zielhilfe, kein Treffer auf die Leiche).
- Messung nachher: unit_r41_alligator2_koerper: lebend schieben 5 von 5 Koerperpunkten, im R1 und an der Leiche 0 von 63 Bildern; Lauf durch die Leiche im Westkanal; integration_r41_alligator2_schluessel(_elza): s. Tests.

### M3 (N35.1): Riegel ueber viele Todeslagen, echter Lauf bis zum Aufheben
- unit_r41_alligator2_erreichbar: Raster 3000, vier Gieren, zwei Leon-Lagen, dazu 13 Wand- und natuerliche Lagen: 221 Lagen, 640128 Stellungen mit der ECHTEN Sonde re15_climb_try_start, je Lage die echte Aktionstaste aus schraeger Stellung (|dx| = |dz| = 300) bis ins Inventar.
- unit_r41_alligator2_weg: Eingabe-Lauf (Lenken ueber Pad-Bits, re15_game_step) von der Leiter und vom Nordbecken bis zum Aufheben, fuenf Lagen.
- integration_r41_alligator2_schluessel(_elza): exe, natuerlicher Kampf bis zum R1, Lauf 1 liefert Figur und Schluessel, Werkzeug wegplan faehrt im Harness zum Schluessel und schreibt die Tasten, Lauf 2 spielt sie an der exe: Leon `[inv-r39] F688 R2090 ... inv=03:57,15:50,34:1`.

### M4 (N35.2): Putz 1:1 wie RE2
- Ursache: der Putz der Spur B war der falsche Effekt am falschen Ausloeser. sub14 (id 4, 54 Partikel) laeuft nur ueber sub13, sub11 startet es bei Cut 9 (`Cmp 23 00 1a 00 09 00` @SCD 0x0778, `Evt_exec 04 ff 18 0d` @0x0786), und Cut 9 setzt allein R2 P1 (`jal 0x8002c7ac`, `addiu a0,zero,9` @0x80102784-88): der Rueckzug nach Beschuss bei offenem Maul, den der Port nicht hat. Dazu kam Sleep 170 und die Lage an der Ecke Bein 2/3.
- Der Putz, den man in RE2 sieht, haengt am Schuetteln. RE2-Belege (SCD ROOM40A0, Datei-Offset SCD-main 0x25C0 + Spalte):
  - sub01 @0x008C (je Bild): `Ck 21 01 1d 01`, `Ck 21 05 10 00`, `Set 22 05 10 01`, `Evt_exec 04 ff 18 0f`. Flag-Tafel @0x800a78c8: [1] = 0x800cfbd8, [5] = 0x800d4874. FUN_80077360 `srlv v0,v0(0x80000000),a1`: Bit 0x1d = Wert 4. Das ist `0x800cfbd8 |= 4` der Ereignis-Maschine @0x80101dd8-f8 (R1 @0x801022a8-b4, @0x8010252c-58) = Stark-Schuetteln.
  - sub15 @0x0916: `Evt_next`, `Switch 13 1a`: Cut 0,1,2 -> sub16; 3..6 -> sub18; 7,8,9 -> sub20; `Set 22 05 10 00` @0x0966.
  - sub16/18/20: 5 x Gosub, `Sleep 1`, `Sleep 4`, `Se_on 36 02 0d 01 01` (@0x0978), `Sleep 3`, `Sleep 4`.
  - sub17/19/21: `Sce_rnd` FUN_80052ca0, `Calc` (and/sub) @0x8005458c, `Work_copy` @0x800542b4 schreibt in die SKRIPTBYTES (Ziel = pc + 4 + Versatz, also x/z des folgenden Records), `Sce_espr_on 3a 00 19 00 00 00 00 06 x e8 ea z 00 00` -> FUN_8001bf10(0x19000600, 0, Einheitsmatrix, {x,-5400,z}) @0x800565a4-0x80056618.
  - Bein 1: x = (r & 0x3fff) - 0x4a38, z = (r & 0xfff) - 0x66ce; Bein 2: x = (r & 0xfff) - 0x66ce, z = (r & 0x7fff) - 0x559e; Bein 3: x = (r & 0x3fff) - 0x61a8, z = (r & 0xfff) - 0x0abe.
  - Raum-ESP Id-Liste @RDT 0x59CC `16 19 08 09 26 1a 18`, Bank 0x19 TIM = "DUS 0.00" (Bild P_belege/nb1_tim19.png): Bank-Op 21 = FUN_8001f5a4 (Tafel @0x8009d8bc): Staubspur Kern-Bank 3 Sub 9, Skala + 2816. Op 21 fehlte in re2_fx.c (Spur ergab `unbekannte Ops 62`, der Brocken ohne Staub).
  - Raumbank EDT 13 `00 00 c3 01`, Se_on-Ruf @0x80056428 (a2 = pc[4] = 1: Lage der Spielerfigur).
- Aenderung: gr2_putz_takt = sub01 + sub15..21 (fuenf Brocken in den Bildern 1/2/6/9/13 von sub15, Satz 13 mit dem dritten, Rechteck je Ringseite, y -5400), eigener Zufallsstrom wie Sce_rnd (die Spielzufaelle bleiben unberuehrt), Op 21 in re2_fx.c, GATOR40A0.VBS um Satz 2 (RE2-Satz 13) erweitert. Der Dash spawnt nichts mehr. PORT-WAHL nur die Lage: RE2 hat drei Beine, ROOM2090 einen Ring aus vier Seiten (k_putz_seite: Cut 0/1 Sued, 2/3 Ost, 4/5 Nord, 6/7 West, Masken wie RE2, Cut 8..12 wie RE2s Cut >= 10 ohne Putz).
- Messung nachher: Lauf kl: 8 Stoesse in 360 Bildern, Stoss F41, F86, F130, F186, F276, F290, F323, F355 (je Schwimm-Stoss einer); fxspur Bank 0x19: 30 Bilder Fall 5220, Op 21 je Bild doppelt, unbekannte Ops 0; unit_r41_re2gator_putz: Bilder 0/1/5/8/12, 5 Brocken, Satz 13 einmal, Rechteck je Seite, Sonderkamera 0 Brocken. Bild P_belege/nb1_putz.png.
- Nicht pruefbar: RE2 laeuft hier nicht (kein Image), der Vergleich steht auf den Bytes, nicht auf einem Bild aus dem Original.

### M5 (N35.4): Finisher, Spieler verschwindet wie RE2
- Beleg RE2 P2-Ende: `80101190 beq v0,zero,0x801011e8` (Clip 5 zu Ende), `80101194 lui v1,0x8`, `8010119c addiu a0,a0,-1032` = 0x800cfbf8 (Spieler-Entity, Lage +0x38 = 0x800cfc30), `801011a0 lw v0,0(a0)`, `801011a8 or v0,v0,v1`, `801011ac sw v0,0(a0)`, `801011b0-c0` Phase + 1. FUN_80027160: `(param_3 & 0x80000) == 0` sonst nur Teile mit 0x9000; FUN_80019b3c/FUN_80019cd0 zeichnen bei dem Bit nicht.
- Aenderung: P3 setzt pl->no_draw (enemy_ai_boss_gator.c), Rueckweg ueber den vorhandenen Continue-Zweig/Raumlader. Spur B hatte "kein no_draw" festgeschrieben, das war der Befund.
- Messung nachher: exe Lauf des Riegels: `RE2 FRESS P3: Spieler-Modell aus F639`, RE15_VIS_TRACE vis=1 bis F638, vis=0 ab F639 bis zum Raumwechsel F779; unit_r41_alligator2_finisher: Clip 4 sichtbar 14, Clip 5 sichtbar 120, Clip 11 aus.

### M6 (N35.5): Leiche sitzt im Maul
- Ursache: die eingefrorene Relation hielt den Prop-Ursprung, so wie er im Koppelbild lag; die Rumpfmitte hing unter Hals und Rumpf (Abnahme 0 zoom_fressen).
- Beleg: RE2 hat keine Leiche im Maul; Form = der Kanister: Kind der Part-6-Weltmatrix (@0x801018d0-e0), Lokallage (1750,-210) (T2 @0x801019c4, @0x80101a4c), Schritte x/z 145 (@0x80101a30-38), y 55 (@0x80101ad4-dc); Beute verschwindet mit dem Clip-5-Ende (M5).
- Aenderung: nach dem Koppelbild fuehrt re15_gatorszene_r41_nachtakt die Lokallage so heran, dass die Rumpfmitte (Modell 1090,-170,-100, MD1 @0x2054) auf der Maullage liegt; leiche_weg + Folge V jetzt beim Uebergang Clip 5 -> Clip 11. PORT-WAHL: Rumpfmitte y -170 = halbe Rueckenhoehe.
- Messung nachher: unit_r41_alligator2_leiche(_elza): Start 1123 vom Maul, nach 8 Bildern da, danach hoechstens 35 ueber 121 Bilder, weg im Bild des Clip-5-Endes. Bild P_belege/nb1_leiche.png.

### M7 (N35.6): Kanister gerade im Maul
- Ursache: RE2 friert rel = R6^T * Rk ein (FUN_8002d030 @0x801018f0) und hat immer dieselbe Geometrie (Gier 0xC00, Kanister quer zur Bahn). Im Port kommt der Gator aus jeder Richtung, der Kanister liegt aber fest in Welt-Drehung: je nach Gier schief oder laengs (Abnahme 0 zoom_kan F120).
- Beleg: Dir (0x690, 0, 0xfc00) ROOM40A0 sub05 @0x048C: Modell-y zeigt nach Welt +X. Gier 0xC00: Sce_em_set @0x25A0 Gier 0x400, Intro-Ende +0x400 @0x80100938-48, Dash +0x400 @0x80100eb0-c0. Kopf-Kippung +0x22A dreht um die Quer-Achse, also bleibt die Tank-Achse auf ihr. Querlage: Pos_set x -22816 @SCD 0x0484, Bahn x -23078 @0x80101048, kein Schreiber auf +0x40 in den Stufen 3-7: Rig-z -262.
- Aenderung: rel = R6a^T * Y(d) * Rk, d laeuft in den 26 Bildern der Stufen 3..6 von 0 auf Gier - 0xC00 (kein Sprung beim Anhaengen); Querlage -262 statt 0 (ersetzt die PORT-WAHL der Nachbesserung 3).
- Messung nachher: unit_r41_alligator2_kanister_sitz: acht Gieren, Tank-Achse * Quer-Achse -4073..-4083 von 4096, Lokallage (1750,-210,-262). Bild P_belege/nb1_kanister.png. Der Tank ragt wie in RE2 nach einer Seite aus dem Maul; im Port bleibt er in Originalgroesse am 2/3-Gator, die Ueberlaenge ist darum sichtbar laenger als in RE2 (OFFEN).

### Tests der Nachbesserung 1
Neu (tests/unit/probes/r41_alligator2.cmake, tests/unit/test_r41_alligator2.c, tests/integration/test_r41_alligator2.cmake):
| Riegel | misst | Ergebnis |
|---|---|---|
| unit_r41_alligator2_erreichbar | 221 Todeslagen inkl. Wand- und natuerliche Lagen: Schluessel erreichbar, echte Klettersonde in 640128 Stellungen nie auf ihm, Aktionstaste aus schraeger Stellung nimmt ihn auf; Gegenprobe Band 0 (1024 von 3600 Stellungen klettern) | gruen |
| unit_r41_alligator2_weg | Lauf ueber die Pad-Eingabe bis zum Aufheben, fuenf Lagen, Figur im Sued- und im Nordbecken, Leiche im Westkanal | gruen |
| unit_r41_alligator2_koerper | lebend 5 von 5 Koerperpunkten schieben, im R1 und an der Leiche 0; keine Trefferbox, keine Zielhilfe; kein Kletter-Start an der Leiche; Lauf durch die Leiche im Westkanal (118 Bilder) | gruen |
| unit_r41_alligator2_finisher | Leon sichtbar in Clip 4 (14 Bilder) und Clip 5 (120), ab dem ersten Bild von Clip 11 aus | gruen |
| unit_r41_alligator2_leiche, _leiche_elza | Rumpfmitte im Maul nach 8 Bildern, danach Abstand hoechstens 35, weg im Bild des Clip-5-Endes | gruen |
| unit_r41_alligator2_kanister_sitz | acht Gieren: Tank-Achse * Quer-Achse -4073..-4083 von 4096, Lokallage (1750,-210,-262) | gruen |
| integration_r41_alligator2_schluessel, _schluessel_elza, _schluessel_west_elza | exe: natuerlicher Kampf, R1, Eingabe-Lauf (Werkzeug wegplan), Aufheben: Inventar `34:1`, kein Kletter-Start auf ein Objekt | gruen |
| integration_r41_alligator2_finisher, _finisher_elza | exe: RE15_VIS_TRACE, sichtbar bis zum Clip-5-Ende, danach kein Bild mit Spielerfigur | gruen |

Nachgezogen mit Beleg: unit_r41_re2gator_putz + integration_r41_re2gator (statt 6 x 9 id 4 beim Dash: Stoss = 5 Brocken Bank 0x19 + Staub, Bilder 0/1/5/8/12, Satz 13, Lage je Ringseite, Sonderkamera ohne Putz, Op 21 gezaehlt); unit_r41_re2gator_aufnahme Fall 3 (Querlage -262 statt 0, RE2-Wert); integration_r41_gatorszene_tod_r1/_elza: Schuss F955 statt F931 (mit dem Kanister quer im Maul fuhr F916/F931 am Clip-Ende in die Kopf-Klemme, Ueberstand 62/55, Vorbedingung des Riegels FG-E7; F955 ohne Klemme: Leiche (1162,-23582), Kamera 2, Tiefe 2729 = 13,7 Bildpunkte, Leon = Elza).

Gezielter Lauf (keine volle Suite), Regex = Lauf 1 oben + unit_enemy_body_push, unit_r34_re2fx*, unit_r34_plattform/_wasser/_wurf, unit/integration_r35_werfer*, unit_r35_fenster_glas, Flammenwerfer, unit_r41_wasser20b0, unit_r38_spinnen, unit_r35_affen, unit_1090_nodraw_pin, unit_r39_hund1121, unit_birkin_szenen, unit_r37_beretta, unit_r38_spiderman:
1. Stand vor der letzten Kommentar-Aenderung, Regex = Scratch `regex2.txt` (Lauf 1 plus die Zusaetze oben): `ctest --test-dir "<baum>/re15_port/build" -R "<regex>" -j4 --timeout 1200 --output-on-failure`
   -> **`99% tests passed, 1 tests failed out of 290`** (`Total Test time (real) = 2036.94 sec`). Rot nur `529 - integration_r35_granate`, Teil birkin `Slot 2: Typ 30 grid 10 im Bild 399 (soll 36 / 13 ...)`, dieselbe Zeile wie auf der Basis f5a5e2f7e (Tests oben, vorbestehend, nicht Spur P); wand/zombie/gator gruen.
2. `ctest --test-dir "<baum>/re15_port/build" -R "^integration_r41_alligator2_schluessel" -j3 --timeout 1300 -V`
   -> **`100% tests passed, 0 tests failed out of 3`** (`Total Test time (real) = 773.86 sec`). Messzeilen: Leon `Figur (-8432,-11485), Schluessel (-5700,-7500)`, `F688 R2090 ... 34:1`; Elza `Figur (-3143,-14991)`, `F706 R2091 ... 34:1`; west_elza `Figur (-3068,-19123), Schluessel (600,-26000)`, Leiche (-3217,-23328) dazwischen, `F837 R2091 ... 34:1`.
3. Endstand: `ctest --test-dir "<baum>/re15_port/build" -R "^(unit_r41_alligator2_|unit_r41_gatorszene_|unit_r41_kanister2080_|unit_r41_re2gator_|integration_r41_alligator2_|integration_r41_gatorszene_tod|integration_r41_re2gator$)" -j4 --timeout 1300 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 81`** (`Total Test time (real) = 640.58 sec`).
Timeouts der Laeufe im Riegel schluessel: 620 s je exe-Lauf, Test 1300 s (bei parallelen Agenten bis 3,7 Bilder/s gemessen, Lauf 1 lief sonst in die Zeitgrenze).
Punkt 3 nachgemessen mit dem neuen Sitz: `TRAGSCAN: 306 Laeufe (270 ohne Aufnahme), 275400 Bilder, Sprung max 338 ... > 340: 0, > 400: 0, Ueberstand max 1366, Bilder Ueberstand > 300: 69` (vorher max 2282, 1192 Bilder). K3 mit begehbarer Lage: Schluessel (0,-1722,-26500), Kamera 0, Tiefe 2721 (vorher (0,-1723,-26800), die lag 200 vor der Suedwand).

### M8 (Dossier)
Dieser Abschnitt; Fertigmeldung nennt Nachtrag 35 und den Release-Pflichtpunkt.

## OFFEN
1. integration_r35_granate Teil birkin ist auf master rot (s.o.) - Orchestrator / Birkin-Zweig, nicht Spur P.
2. Punkt 1: die Wurzel kann im RE2-Band nicht im Bild von Cut 0 liegen (Rechnung oben): am Kampfbeginn ragen Kopf und Hals von rechts ins Bild, der Rumpf folgt ab +111 Bilder. Das ist das Beste, was Band + Zone 0 + Cut 0 zulassen.
3. Punkt 3: die weiche Klemme laesst Kopf/Kanister-Ende kurz ueber die Wand ragen (0,43 % der Tragebilder ueber 300, max 2282, vor allem Biss-Ausholen an der Wand); in diesen Bildern gleitet der Alligator mit bis zu 340 je Bild zurueck. Sichtbar als schnelles Zurueckweichen, kein Sprung.
4. CROSS mit dem Kanister: keine Kopf-Klemme (gb_klemme_aus, wie bisher); die Bahn beginnt jetzt an der Lage des Gators. Ohne Kanister bleibt der Sprung auf den Anlaufpunkt (bis ~1400, Spur B / Runde 9) - nicht Teil dieses Auftrags.
5. Schluessel an der neuen Lage: Ring in der Ankunft ~8 x 7 Bildpunkte (Spitzen 12 x 12), Helligkeit wie abgenommen; die Groesse hatte der Nutzer mit 7 x 9 angenommen (Nachtrag 31 Punkt 3) - Rangfolge F-E2: Kampfbeginn vor Schluessel.
6. Stufen 1..3 der Lagewahl legen den Schluessel bis 2 x 3070 von der Leiche, wenn naeher keine Kamera 10,4 Bildpunkte zeigt (5 % der Lagen im Scan, vor allem Westkanal/Nordbecken).
7. PSX-Bau hier nicht pruefbar.
8. Nachbesserung 1: RE2 laeuft hier nicht (kein Image); Putz, Finisher und Kanister-Sitz stehen auf den Bytes, ein Bild aus dem Original gibt es nicht. Die Saat von Sce_rnd ist in RE2 eine laufend beschriebene Pool-Zelle (0x800d8cb8, FUN_8001c224), der Port nimmt eine feste Zahl plus Bildzaehler.
9. Der Kanister bleibt in Originalgroesse am 2/3-Gator (GB_SCALE_Q12): Sitz und Achse sind RE2, die Ueberlaenge zur Seite ist 1,5 x so lang wie in RE2. Nicht angefasst: er liegt vorher in derselben Groesse im Wasser, eine Umskalierung beim Biss waere erfunden.
10. RE2-R1 setzt mit 0x80a auch Bit 0x800 = Zeichen-Skala +0x176 (3678 -> 3478, FUN_80027160 ScaleMatrix, r1_176 in gator_re2_r41.c): die Leiche wird in RE2 10-15 % kleiner gezeichnet; der Port nicht. Nicht Teil von Nachtrag 35.
11. Mit dem Kanister quer ragt sein Ende bis 2600 zur Seite; die Kopf-Klemme haelt ihn in den Waenden, der Alligator bleibt dadurch auf der Tank-Seite so weit von der Wand weg (Riegel tragen gruen, Bildschritt <= 340).

## Fuer den Nutzer
- Assets: `shared_assets/RE2/GATOR40A0.VBS` geaendert (39064 B, md5 8afd43aa998b8fcaeb843c5c9c9b4535; vorher 34296 B): neu Satz 2 = RE2-Raumbank Satz 13 (Putz). Name unveraendert, steht schon in make_package.sh. Keine Sprachdateien, keine neuen Bits/Nachrichten-Ids.
- Gemeinsame Dateien: enemy_ai_boss_gator.c (Haken (2b), gb_cross_begin, GBP_CROSS, Push-Gate, P3 no_draw, je markiert "Runde 41 Spur P"), enemy_ai_common.c (1 Zeile re15_body_push_player), re2_fx.c (Op 21), gator_re2_r41.c (Kopf-Klemme, re15_gr2_r1_start, Putz, Kanister-Sitz), gatorszene_r41.c/.h, re15_gator_re2_r41.h, re15_boss_gator.h, platform/pc/src/gator_re2_pc.c (1 Zeile), tools/r41_re2gator/gator40a0_se_cut.py; Tests test_r41_gatorszene.c/.cmake, test_r41_re2gator.c/.cmake, probes/r41_gatorszene.cmake, probes/r41_re2gator.cmake.
- Kampfbeginn: Der Alligator frisst die Leiche weiter vorn im Becken und schwimmt nach dem Schrei von rechts durch die Beckenmitte auf Leon zu; er liegt nicht mehr vor der Kamera. Abstand und Zeit bis zum ersten Biss bleiben im RE2-Bereich (rund 4,5 s).
- Mit dem Kanister im Maul springt er nicht mehr; an der Wand weicht er zurueck, dabei kann der Kopf kurz in die Wand ragen.
- Schluessel nach dem Tod: Er liegt jetzt auch in der Wandlage gut sichtbar (mindestens 10,4 Bildpunkte); die Kamera der Einstellung meidet Stellen, die Leon verdeckt.
- Schluessel nach dem Tod (Nachtrag 35 Punkt 1): Er liegt immer dort, wo die Figur hinkommt, nie mehr im Maschinenblock. Die Aktionstaste hebt ihn auf; vorher kletterte die Figur oft auf ihn, das war auch das "Klettern auf den toten Alligator".
- Die Leiche des Alligators hat wie in RE2 keine Kollision mehr: man laeuft durch sie hindurch, sie sperrt den Westkanal nicht mehr, und Leon zielt nicht mehr auf sie.
- Putz: Er faellt jetzt bei jedem starken Stoss, wie in RE2 an die Stelle des Schuettelns gebunden: fuenf Brocken mit Staubspur, zufaellig in der Beckenseite der laufenden Kamera, mit dem RE2-Geraeusch. Der alte Staub ueber Leon beim Dash war der falsche Effekt (in RE2 gehoert er zum Rueckzug des Alligators nach Beschuss, den es hier nicht gibt). Weit hinten im Becken sind die Brocken klein, wie in RE2 auf Entfernung.
- Finisher: Leon verschwindet mit dem Ende des Schuettelns, danach kaut der Alligator allein.
- Fress-Szene: Die Leiche liegt quer im Maul, folgt dem Kopf Bild fuer Bild und ist mit dem Ende des Schuettelns weg.
- Kanister: Er liegt quer im Maul wie in RE2 und ragt nach einer Seite heraus, egal aus welcher Richtung der Alligator kommt.
