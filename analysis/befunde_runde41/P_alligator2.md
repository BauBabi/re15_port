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

Massgeblich: AUFTRAG.md Nachtrag 35 (Alligator-Befunde des Nutzers, 2026-10-30 16:46), sechs Punkte, Punkt 1 ist Pflicht fuer das Release. Die erste Fertigmeldung nannte sie nicht (M8).
Stand der Maengel: siehe Unterabschnitte M1..M8; fortlaufend gefuellt.

## OFFEN
1. integration_r35_granate Teil birkin ist auf master rot (s.o.) - Orchestrator / Birkin-Zweig, nicht Spur P.
2. Punkt 1: die Wurzel kann im RE2-Band nicht im Bild von Cut 0 liegen (Rechnung oben): am Kampfbeginn ragen Kopf und Hals von rechts ins Bild, der Rumpf folgt ab +111 Bilder. Das ist das Beste, was Band + Zone 0 + Cut 0 zulassen.
3. Punkt 3: die weiche Klemme laesst Kopf/Kanister-Ende kurz ueber die Wand ragen (0,43 % der Tragebilder ueber 300, max 2282, vor allem Biss-Ausholen an der Wand); in diesen Bildern gleitet der Alligator mit bis zu 340 je Bild zurueck. Sichtbar als schnelles Zurueckweichen, kein Sprung.
4. CROSS mit dem Kanister: keine Kopf-Klemme (gb_klemme_aus, wie bisher); die Bahn beginnt jetzt an der Lage des Gators. Ohne Kanister bleibt der Sprung auf den Anlaufpunkt (bis ~1400, Spur B / Runde 9) - nicht Teil dieses Auftrags.
5. Schluessel an der neuen Lage: Ring in der Ankunft ~8 x 7 Bildpunkte (Spitzen 12 x 12), Helligkeit wie abgenommen; die Groesse hatte der Nutzer mit 7 x 9 angenommen (Nachtrag 31 Punkt 3) - Rangfolge F-E2: Kampfbeginn vor Schluessel.
6. Stufen 1..3 der Lagewahl legen den Schluessel bis 2 x 3070 von der Leiche, wenn naeher keine Kamera 10,4 Bildpunkte zeigt (5 % der Lagen im Scan, vor allem Westkanal/Nordbecken).
7. PSX-Bau hier nicht pruefbar.

## Fuer den Nutzer
- Keine neuen Assets, keine Sprachdateien, keine neuen Bits/Nachrichten-Ids.
- Gemeinsame Dateien: enemy_ai_boss_gator.c (Haken (2b), gb_cross_begin, GBP_CROSS, je markiert "Runde 41 Spur P"), gator_re2_r41.c (Kopf-Klemme, re15_gr2_r1_start), gatorszene_r41.c/.h, re15_gator_re2_r41.h; Tests test_r41_gatorszene.c/.cmake, test_r41_re2gator.c, probes/r41_gatorszene.cmake.
- Kampfbeginn: Der Alligator frisst die Leiche weiter vorn im Becken und schwimmt nach dem Schrei von rechts durch die Beckenmitte auf Leon zu; er liegt nicht mehr vor der Kamera. Abstand und Zeit bis zum ersten Biss bleiben im RE2-Bereich (rund 4,5 s).
- Mit dem Kanister im Maul springt er nicht mehr; an der Wand weicht er zurueck, dabei kann der Kopf kurz in die Wand ragen.
- Schluessel nach dem Tod: Er liegt jetzt auch in der Wandlage gut sichtbar (mindestens 10,4 Bildpunkte); die Kamera der Einstellung meidet Stellen, die Leon verdeckt.
