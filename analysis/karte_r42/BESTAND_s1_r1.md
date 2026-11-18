# Karte STAGE1-6: Messlatte und Bestand (Strang r42/karte, Stufe 1, Runde 1)

Stand: Zweig r42/karte, Basis aaea5f8c2 (gemessen) und der neue Stand dieser Runde (STAGE2 + STAGE3 behoben).
Gemessen mit der Sonde `re15_port/tests/unit/probe_r42_karte_bestand.c` (echter Pfad: Raum laden wie das Spiel,
Zone aus der Lage, echte Op-Liste `re15_inv_screen_build`, Marker `re15_inv_map_marker`), ausgewertet mit
Texeln der Original-Kacheln `DATA/MAP0x.PIX`. Jeder Raum, Leon (xxx0) UND Elza (xxx1). Nur der eigene Raum
besucht, kein Plan im Besitz (Schleier-Pruefung). Dazu Kartenschirm-Abzuege aus dem echten Spiel (FB_SHOT).

## 1. Messlatte = was STAGE1 heute kann (gemessen an aaea5f8c2)

| # | Punkt | STAGE1 gemessen | Beleg |
|---|---|---|---|
| M1 | Blatt je Bereich/Etage | jeder Raum mit Zone zeigt sein Blatt; Etagenwechsel ueber das Band (1060 Treppe, 1080 Fahrstuhl, 1090, 1170) | `re15_inv_map_page_shown`, Original-Init @0x8004b568; Pins unit_map_floor_visited, unit_map_etagen_rect |
| M2 | Raum als Grundriss | 74 von 78 Raeumen mit Zone (ohne: 1240/1241, 1260/1261); Marker auf den gemalten Texeln des eigenen Rechtecks 82 % (2272/2770 Rasterpunkte); ungemalte Raeume als Schema-Kasten | Sonde Spalte "Marker auf Kachel" |
| M3 | Tuersymbole | 132 von 195 Tueren mit Marke ueber die Identitaet (+8 ungepaart); Marken auf der gemeinsamen Kante (Pin unit_map_durchgang: 0 verdreht, <=4 neben der Kante) | Sonde, Pin |
| M4 | Schleier | nur der eigene Raum gezeichnet (RE2-Zustaende, ohne Plan): 770/850 Kamera-Stichproben; Ausnahmen 10B0, 1110 (Teilbereiche), 1240/1260 | Sonde Spalte "rot+Schleier" |
| M5 | Zeichenreihenfolge | aktueller Raum zuerst = oben, sichtbar rot 778/850 (gleiche Ausnahmen) | integration_map_raum_live |
| M6 | Marker an der richtigen Stelle, laeuft mit | 85 % verschiedene Marker-Positionen ueber das 7x7-Raster je Zone; Korrelation Welt->Karte je Achse 0,9..1,0; einzig 10B0 steht | Sonde "Mitlaufen" |
| M7 | Seitenwechsel | Blatt folgt Raum/Band; Tuer auf ein anderes Blatt = Blattwechsel | Sonde DOOR-Zeilen |
| M8 | Sprung beim Tuerdurchgang | Median 4 px, 8 von 149 ueber 8 px | Sonde DOOR-Zeilen |

STAGE1 ist selbst nicht fehlerfrei (gemessen): 10B0 ohne Rot/Schleier und mit stehendem Marker (Schema), 1110
Schleier 7/13 (Teilbereiche), 10A0 Marker 0/49 auf der Kachel, 1240/1260 ohne Zone, 1170 nur 45/96 auf der Kachel.
Diese Punkte gehoeren nicht zu diesem Strang-Schritt und sind nur als Grenze der Messlatte vermerkt.

## 2. Zusammenfassung je Stage (vorher -> nachher)

Vorher = aaea5f8c2, nachher = dieser Stand. Zahlen je Stage ueber Leon+Elza.

```
    == bestand_basis.json
     STAGE1 {'raeume': 78, 'mit_zone': 74, 'rot': '778/850', 'schleier': '770/850', 'auf_kachel': '2272/2770 (82%)', 'mitlauf_versch': '3360/3936 (85%)', 'tuer_identitaet': '132+8 einzeln /195', 'uebergang': 'n=149 median=4 >8px=8 max=140', 'marker_steht': ['10B0', '10B1'], 'ohne_zone': ['1240', '1241', '1260', '1261']}
     STAGE2 {'raeume': 24, 'mit_zone': 18, 'rot': '236/314', 'schleier': '236/314', 'auf_kachel': '428/1076 (40%)', 'mitlauf_versch': '892/1076 (83%)', 'tuer_identitaet': '32+6 einzeln /58', 'uebergang': 'n=48 median=29.0 >8px=26 max=195', 'marker_steht': ['2050', '2051'], 'ohne_zone': ['2020', '2021', '20A0', '20A1', '20B0', '20B1']}
     STAGE3 {'raeume': 30, 'mit_zone': 30, 'rot': '302/302', 'schleier': '302/302', 'auf_kachel': '1064/1394 (76%)', 'mitlauf_versch': '928/1394 (67%)', 'tuer_identitaet': '31+20 einzeln /59', 'uebergang': 'n=54 median=19.0 >8px=41 max=75', 'marker_steht': ['3060', '3061', '3070', '3071', '3080', '3081', '30E0', '30E1'], 'ohne_zone': []}
     STAGE4 {'raeume': 22, 'mit_zone': 22, 'rot': '198/198', 'schleier': '198/198', 'auf_kachel': '670/874 (77%)', 'mitlauf_versch': '662/874 (76%)', 'tuer_identitaet': '41+27 einzeln /76', 'uebergang': 'n=41 median=12 >8px=24 max=91', 'marker_steht': ['4050', '4051'], 'ohne_zone': []}
     STAGE5 {'raeume': 42, 'mit_zone': 38, 'rot': '322/406', 'schleier': '308/406', 'auf_kachel': '922/1496 (62%)', 'mitlauf_versch': '1292/1496 (86%)', 'tuer_identitaet': '32+41 einzeln /91', 'uebergang': 'n=65 median=38 >8px=57 max=170', 'marker_steht': [], 'ohne_zone': ['5070', '5071', '5130', '5131']}
     STAGE6 {'raeume': 10, 'mit_zone': 8, 'rot': '100/118', 'schleier': '100/118', 'auf_kachel': '294/460 (64%)', 'mitlauf_versch': '266/460 (58%)', 'tuer_identitaet': '12+8 einzeln /22', 'uebergang': 'n=8 median=2.0 >8px=0 max=5', 'marker_steht': ['6000', '6001'], 'ohne_zone': ['6040', '6041']}
    == bestand_neu.json
     STAGE1 {'raeume': 78, 'mit_zone': 74, 'rot': '778/850', 'schleier': '770/850', 'auf_kachel': '2272/2770 (82%)', 'mitlauf_versch': '3360/3936 (85%)', 'tuer_identitaet': '132+8 einzeln /195', 'uebergang': 'n=149 median=4 >8px=8 max=140', 'marker_steht': ['10B0', '10B1'], 'ohne_zone': ['1240', '1241', '1260', '1261']}
     STAGE2 {'raeume': 24, 'mit_zone': 22, 'rot': '282/314', 'schleier': '282/314', 'auf_kachel': '1222/1276 (96%)', 'mitlauf_versch': '1080/1276 (85%)', 'tuer_identitaet': '36+4 einzeln /58', 'uebergang': 'n=48 median=9.0 >8px=32 max=195', 'marker_steht': [], 'ohne_zone': ['20B0', '20B1']}
     STAGE3 {'raeume': 30, 'mit_zone': 30, 'rot': '302/302', 'schleier': '302/302', 'auf_kachel': '1318/1422 (93%)', 'mitlauf_versch': '1262/1492 (85%)', 'tuer_identitaet': '43+0 einzeln /59', 'uebergang': 'n=54 median=8.0 >8px=17 max=74', 'marker_steht': ['3030', '3031', '3080', '3081', '30E0', '30E1'], 'ohne_zone': []}
     STAGE4 {'raeume': 22, 'mit_zone': 22, 'rot': '198/198', 'schleier': '198/198', 'auf_kachel': '670/874 (77%)', 'mitlauf_versch': '662/874 (76%)', 'tuer_identitaet': '41+27 einzeln /76', 'uebergang': 'n=41 median=12 >8px=24 max=91', 'marker_steht': ['4050', '4051'], 'ohne_zone': []}
     STAGE5 {'raeume': 42, 'mit_zone': 38, 'rot': '322/406', 'schleier': '308/406', 'auf_kachel': '922/1496 (62%)', 'mitlauf_versch': '1292/1496 (86%)', 'tuer_identitaet': '32+41 einzeln /91', 'uebergang': 'n=65 median=38 >8px=57 max=170', 'marker_steht': [], 'ohne_zone': ['5070', '5071', '5130', '5131']}
     STAGE6 {'raeume': 10, 'mit_zone': 8, 'rot': '100/118', 'schleier': '100/118', 'auf_kachel': '294/460 (64%)', 'mitlauf_versch': '266/460 (58%)', 'tuer_identitaet': '12+8 einzeln /22', 'uebergang': 'n=8 median=2.0 >8px=0 max=5', 'marker_steht': ['6000', '6001'], 'ohne_zone': ['6040', '6041']}
```

Lesart: `auf_kachel` = Marker auf den gemalten Texeln des eigenen Rechtecks; `mitlauf_versch` = verschiedene
Marker-Positionen / Rasterpunkte; `uebergang` = Marker vor und nach der Tuer auf demselben Blatt;
`marker_steht` = Zone mit weniger als 25 % verschiedenen Positionen.

## 3. Was in STAGE2 und STAGE3 falsch war und jetzt gilt

**STAGE2 (Blatt 6, MAP07.PIX):** sechs Orte (2010/2030/2060/2070/2080) lagen auf rect 0 = dem U-Kanal von ROOM2000,
ROOM2020 hatte gar keine Zeile (Karte leer, kein Marker), ROOM2050 stand auf rect 1 und der Marker bewegte sich
nicht, ROOM20A0 zeigte das Blatt 0xD (im Original ohne Kunst, CD-Id @0x80074c66 = 0) = leere Karte.
Jetzt: 2000 r0 (ausgeliefert @0x800769E0), 2010 r1, 2020 r4 (neu), 2030 r6, 2040 r5, 2050 r2, 2060 r7,
2070 r3 (Zone 0 im Rahmen von 2000), 2080 r8 (Schacht + Querstueck), 2090 r9, 20A0 r10 (PORT-WAHL).

**STAGE3 (Blatt 7, MAP08.PIX):** 3060 auf r3 (8 px breit) und 3070 auf r8 (8x16) - Marker stand waagerecht still;
3030 auf r6, 3090 auf r10, 30A0+30B0 beide auf r9, 3050 auf r12, 30E0 Zone 1 auf r7. Jetzt nach den
ausgelieferten Zeilen: 3030 r3, 3060 r6, 3070 r7, 3090 r9, 30A0 r10, 30B0 r11, 30E0 Zone 1 r13, 3050 r5,
30D0 in zwei Zonen (r2 wie 3020, r12 wie 30C0 - Varianten-Regel v0.3.96).

Herleitung und Belege: `re15_port/tools/karte_r42_zeilen.py`, Kommentarbloecke in `re15_map_zones.h`.

## 4. Offen je Stage (nicht in dieser Runde behoben)

- **STAGE2:** 2040 Suedteil und 2070 Zone 2 (Gang 2040 -> 11A0) sind nirgends gemalt - Marker am Rand von r5
  bzw. auf r3 (Runde 2: Schema-Kasten nordoestlich von r5). ROOM11A0 (STAGE1) liegt seit Runde 36 ebenfalls
  auf Blatt 6 rect 2 - das ist nach Tueranker und gemaltem Symbol die Kachel von ROOM2050 (Tuer 2000->2050
  (91,103), Marke 2020<->2050 1 px neben dem gemalten Symbol (123,96)); 11A0 braucht einen eigenen Platz.
  20A0 auf r10 (11x8 px) laeuft nur in kleinen Schritten. 20B0 von keiner Tuer erreichbar, ohne Zone.
  Treppen-/Leitermarken fehlen auf Blatt 6/7.
- **STAGE3:** 30E0 Zone 0 - die ausgelieferte Zeile @0x80076AB0 legt sie auf r6 (= 3060), Tuerpaare
  30E0<->3030 / 30E0<->3050 springen 19..74 px; 3030 und 3080 sind 8 px breit gemalt (Klemmung 4 px laesst
  eine Spalte).
- **STAGE4 (Blatt 8):** Marker steht in 4040 (7/21) und 4050 (5/38); 4070 zeigt Blatt 11 statt 8 (Etagenzeile),
  Marker 0/49 auf der Kachel; 4000 nur 25/36 auf der Kachel (ausgelieferte Zeile @0x80076AB8 trifft r0 zu 46 %);
  4020 drei Tueren ohne Marke. Raeume 40C0-40F0/4060 sind im Baum Platzhalter (4 Byte).
- **STAGE5 (Blaetter 9-11):** 5070 und 5130 ohne Zone (Karte leer); 5020 und 5090 ohne sichtbares Rot;
  5040/5120 liegen auf r5, ihre ausgelieferte Zeile @0x80076B38/@0x80076BA8 trifft r4 (68 %); 50A0/5140 auf r14,
  Zeile @0x80076B68/@0x80076BB8 trifft r10; 5100 hat eine ausgelieferte Zeile @0x80076B98 (r0 zu 64 %), die
  Zone aber keine (rect 1); 50D0 zeigt Blatt 11 statt 10; Spruenge bis 170 px.
- **STAGE6 (Blatt 12):** 6040 ohne Zone (die ausgelieferte Zeile @0x80076BE0 (202,136,2193,2145) gibt es);
  Marker steht in 6000 (7/48) und 6030 (10/35); 6030s Zeile @0x80076BD8 legt 55 % der Punkte neben jede Kachel.
  6050-6070 sind im Baum Platzhalter (4 Byte).

Konflikt-Vorwarnung: U-11 (Wandplan 402 -> Laborblatt 8) und U-12 (U-Bahn-Blatt 12, Wagenkacheln) baut r42/rest
in genau den Blaettern 8 und 12, die hier als STAGE4/STAGE6 offen stehen - die Zonen-/Rechteck-Tabellen
(`re15_map_zones.h`, `s_map_rectfix`) sind dieselben.

## 5. Tabelle je Raum (Spalten "Befund" = automatisch aus der Sonde)

Rechteck = Blatt/rect der Zone(n); "Marker auf Kachel" und "Mitlaufen" ueber das 7x7-Raster je Zone im
RVD-Bereich; "rot+Schleier (Kam.)" = Kamera-Stichproben mit sichtbarem Rot / nur eigener Raum gezeichnet.
Spalten 3-7 = neuer Stand; "Befund aaea5f8c2" = vorher.

| Raum | Blatt orig/gez. | Rechteck | Marker auf Kachel | Mitlaufen (verschieden/Raster) | Tueren ident/einzeln/ohne | rot+Schleier (Kam.) | Befund aaea5f8c2 | Befund r42 |
|---|---|---|---|---|---|---|---|---|
| 2000 | 6 / 6 | 6/0 | 24/24 | 24/24 | 2/2/0 | 16+16/16 | Uebergang >16 px: [29, 29, 101] | Uebergang >16 px: [29, 101] |
| 2001 (Elza) | 6 / 6 | 6/0 | 24/24 | 24/24 | 2/2/0 | 16+16/16 | Uebergang >16 px: [29, 29, 101] | Uebergang >16 px: [29, 101] |
| 2010 | 6 / 6 | 6/1 | 49/49 | 49/49 | 1/0/0 | 6+6/6 | Marker neben der Kachel (35/49 auf) | ok |
| 2011 (Elza) | 6 / 6 | 6/1 | 49/49 | 49/49 | 1/0/0 | 6+6/6 | Marker neben der Kachel (35/49 auf) | ok |
| 2020 | 6 / 6 | 6/4 | 49/49 | 42/49 | 1/0/0 | 7+7/7 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [160] | ok |
| 2021 (Elza) | 6 / 6 | 6/4 | 49/49 | 42/49 | 1/0/0 | 7+7/7 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [160] | ok |
| 2030 | 6 / 6 | 6/6 | 45/45 | 45/45 | 3/0/0 | 16+16/16 | Marker neben der Kachel (18/45 auf) | Uebergang >16 px: [17] |
| 2031 (Elza) | 6 / 6 | 6/6 | 45/45 | 45/45 | 3/0/0 | 16+16/16 | Marker neben der Kachel (18/45 auf) | Uebergang >16 px: [17] |
| 2040 | 6 / 6 | 6/5 | 43/43 | 42/43 | 2/0/2 | 16+16/16 | Marker neben der Kachel (6/28 auf); Marker steht z0 (12/28 Pos.); Uebergang >16 px: [51] | 2 Tuer(en) ohne Marke |
| 2041 (Elza) | 6 / 6 | 6/5 | 43/43 | 42/43 | 2/0/2 | 16+16/16 | Marker neben der Kachel (6/28 auf); Marker steht z0 (12/28 Pos.); Uebergang >16 px: [51] | 2 Tuer(en) ohne Marke |
| 2050 | 6 / 6 | 6/2 | 41/49 | 49/49 | 3/0/0 | 16+16/16 | Marker neben der Kachel (0/49 auf); Marker steht z0 (1/49 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [56, 160] | ok |
| 2051 (Elza) | 6 / 6 | 6/2 | 41/49 | 49/49 | 3/0/0 | 16+16/16 | Marker neben der Kachel (0/49 auf); Marker steht z0 (1/49 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [56, 160] | ok |
| 2060 | 6 / 6 | 6/7 | 49/49 | 49/49 | 2/0/0 | 13+13/13 | Marker neben der Kachel (28/49 auf); Uebergang >16 px: [51] | ok |
| 2061 (Elza) | 6 / 6 | 6/7 | 49/49 | 49/49 | 2/0/0 | 13+13/13 | Marker neben der Kachel (28/49 auf); Uebergang >16 px: [51] | ok |
| 2070 | 6 / 6 | 6/3 | 128/147 | 83/147 | 2/0/2 | 13+13/13 | Marker neben der Kachel (58/147 auf); 2 Tuer(en) ohne Marke; Uebergang >16 px: [29, 54, 59] | 2 Tuer(en) ohne Marke |
| 2071 (Elza) | 6 / 6 | 6/3 | 128/147 | 83/147 | 2/0/2 | 13+13/13 | Marker neben der Kachel (58/147 auf); 2 Tuer(en) ohne Marke; Uebergang >16 px: [29, 54, 59] | 2 Tuer(en) ohne Marke |
| 2080 | 6 / 6 | 6/8 | 98/98 | 91/98 | 1/0/0 | 9+9/9 | Marker neben der Kachel (12/98 auf); 1 Tuer(en) ohne Marke | Uebergang >16 px: [17] |
| 2081 (Elza) | 6 / 6 | 6/8 | 98/98 | 91/98 | 1/0/0 | 9+9/9 | Marker neben der Kachel (12/98 auf); 1 Tuer(en) ohne Marke | Uebergang >16 px: [17] |
| 2090 | 6 / 6 | 6/9 | 49/49 | 49/49 | 1/0/0 | 13+13/13 | Marker neben der Kachel (33/49 auf) | ok |
| 2091 (Elza) | 6 / 6 | 6/9 | 49/49 | 49/49 | 1/0/0 | 13+13/13 | Marker neben der Kachel (33/49 auf) | ok |
| 20A0 | 13 / 6 | 6/10 | 36/36 | 17/36 | 0/0/1 | 16+16/16 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke | Blatt weicht vom Original-Init ab (16 Kam.); Marker steht z0 (17/36 Pos.); 1 Tuer(en) ohne Marke |
| 20A1 (Elza) | 13 / 6 | 6/10 | 36/36 | 17/36 | 0/0/1 | 16+16/16 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke | Blatt weicht vom Original-Init ab (16 Kam.); Marker steht z0 (17/36 Pos.); 1 Tuer(en) ohne Marke |
| 20B0 | 13 / 13 | - | - | 0/0 | 0/0/4 | 0+0/16 | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] |
| 20B1 (Elza) | 13 / 13 | - | - | 0/0 | 0/0/4 | 0+0/16 | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] |
| 3000 | 7 / 7 | 7/0 | 38/43 | 43/43 | 1/0/1 | 4+4/4 | 1 Tuer(en) ohne Marke | 1 Tuer(en) ohne Marke |
| 3001 (Elza) | 7 / 7 | 7/0 | 38/43 | 43/43 | 1/0/1 | 4+4/4 | 1 Tuer(en) ohne Marke | 1 Tuer(en) ohne Marke |
| 3010 | 7 / 7 | 7/1 | 46/49 | 49/49 | 4/0/0 | 10+10/10 | Uebergang >16 px: [43, 48] | Uebergang >16 px: [47] |
| 3011 (Elza) | 7 / 7 | 7/1 | 46/49 | 49/49 | 4/0/0 | 10+10/10 | Uebergang >16 px: [43, 48] | Uebergang >16 px: [47] |
| 3020 | 7 / 7 | 7/2 | 49/49 | 49/49 | 1/0/1 | 9+9/9 | 1 Tuer(en) ohne Marke | 1 Tuer(en) ohne Marke; Uebergang >16 px: [17] |
| 3021 (Elza) | 7 / 7 | 7/2 | 49/49 | 49/49 | 1/0/0 | 9+9/9 | ok | ok |
| 3030 | 7 / 7 | 7/3 | 42/42 | 7/42 | 1/0/1 | 2+2/2 | Uebergang >16 px: [19] | Marker steht z0 (7/42 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [19] |
| 3031 (Elza) | 7 / 7 | 7/3 | 42/42 | 7/42 | 1/0/1 | 2+2/2 | Uebergang >16 px: [19] | Marker steht z0 (7/42 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [19] |
| 3040 | 7 / 7 | 7/4 | 46/46 | 46/46 | 1/0/0 | 14+14/14 | Uebergang >16 px: [75] | ok |
| 3041 (Elza) | 7 / 7 | 7/4 | 46/46 | 46/46 | 1/0/0 | 14+14/14 | Uebergang >16 px: [75] | ok |
| 3050 | 7 / 7 | 7/5 | 43/43 | 43/43 | 1/0/1 | 16+16/16 | Marker neben der Kachel (29/43 auf); Marker steht z0 (21/43 Pos.); Uebergang >16 px: [37, 74] | 1 Tuer(en) ohne Marke; Uebergang >16 px: [74] |
| 3051 (Elza) | 7 / 7 | 7/5 | 43/43 | 43/43 | 1/0/1 | 16+16/16 | Marker neben der Kachel (29/43 auf); Marker steht z0 (21/43 Pos.); Uebergang >16 px: [37, 74] | 1 Tuer(en) ohne Marke; Uebergang >16 px: [74] |
| 3060 | 7 / 7 | 7/6 | 30/37 | 30/37 | 1/0/1 | 7+7/7 | Marker steht z0 (5/37 Pos.); Uebergang >16 px: [44, 56] | 1 Tuer(en) ohne Marke; Uebergang >16 px: [56] |
| 3061 (Elza) | 7 / 7 | 7/6 | 30/37 | 30/37 | 1/0/1 | 7+7/7 | Marker steht z0 (5/37 Pos.); Uebergang >16 px: [32, 44] | 1 Tuer(en) ohne Marke; Uebergang >16 px: [48] |
| 3070 | 7 / 7 | 7/7 | 45/48 | 48/48 | 1/0/0 | 13+13/13 | Marker neben der Kachel (35/48 auf); Marker steht z0 (3/48 Pos.); Uebergang >16 px: [36] | ok |
| 3071 (Elza) | 7 / 7 | 7/7 | 45/48 | 48/48 | 1/0/0 | 13+13/13 | Marker neben der Kachel (35/48 auf); Marker steht z0 (3/48 Pos.); Uebergang >16 px: [36] | ok |
| 3080 | 7 / 7 | 7/8 | 32/38 | 5/38 | 0/0/1 | 4+4/4 | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke |
| 3081 (Elza) | 7 / 7 | 7/8 | 32/38 | 5/38 | 0/0/1 | 4+4/4 | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke |
| 3090 | 7 / 7 | 7/9 | 26/38 | 38/38 | 3/0/0 | 16+16/16 | Marker neben der Kachel (18/38 auf); Marker steht z0 (18/38 Pos.); Uebergang >16 px: [17] | Marker neben der Kachel (26/38 auf) |
| 3091 (Elza) | 7 / 7 | 7/9 | 26/38 | 38/38 | 0/0/0 | 16+16/16 | Marker neben der Kachel (18/38 auf); Marker steht z0 (18/38 Pos.) | Marker neben der Kachel (26/38 auf) |
| 30A0 | 7 / 7 | 7/10 | 43/48 | 48/48 | 1/0/0 | 7+7/7 | Marker neben der Kachel (9/48 auf) | ok |
| 30A1 (Elza) | 7 / 7 | 7/10 | 43/48 | 48/48 | 1/0/0 | 7+7/7 | Marker neben der Kachel (9/48 auf) | ok |
| 30B0 | 7 / 7 | 7/11 | 45/49 | 49/49 | 1/0/0 | 8+8/8 | Marker neben der Kachel (17/49 auf); Uebergang >16 px: [17] | ok |
| 30B1 (Elza) | 7 / 7 | 7/11 | 45/49 | 49/49 | 1/0/0 | 8+8/8 | Marker neben der Kachel (17/49 auf); Uebergang >16 px: [17] | ok |
| 30C0 | 7 / 7 | 7/12 | 49/49 | 49/49 | 2/0/0 | 15+15/15 | ok | ok |
| 30C1 (Elza) | 7 / 7 | 7/12 | 49/49 | 49/49 | 2/0/0 | 15+15/15 | ok | ok |
| 30D0 | 7 / 7 | 7/12 | 56/63 | 98/98 | 3/0/0 | 16+16/16 | Uebergang >16 px: [27, 39] | ok |
| 30D1 (Elza) | 7 / 7 | 7/12 | 56/63 | 98/98 | 3/0/0 | 16+16/16 | Uebergang >16 px: [27, 39] | ok |
| 30E0 | 7 / 7 | 7/3;7/13 | 69/69 | 29/69 | 2/0/3 | 10+10/10 | Marker neben der Kachel (45/69 auf); Marker steht z0 (5/39 Pos.); 2 Tuer(en) ohne Marke; Uebergang >16 px: [20, 36, 37] | Marker steht z0 (5/39 Pos.); 3 Tuer(en) ohne Marke; Uebergang >16 px: [20, 73] |
| 30E1 (Elza) | 7 / 7 | 7/3;7/13 | 69/69 | 29/69 | 2/0/2 | 10+10/10 | Marker neben der Kachel (45/69 auf); Marker steht z0 (5/39 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [20, 36, 37] | Marker steht z0 (5/39 Pos.); 2 Tuer(en) ohne Marke; Uebergang >16 px: [20, 73] |
| 4000 | 8 / 8 | 8/0 | 25/36 | 34/36 | 2/2/0 | 8+8/8 | Marker neben der Kachel (25/36 auf); Uebergang >16 px: [18] | (unveraendert) |
| 4001 (Elza) | 8 / 8 | 8/0 | 25/36 | 34/36 | 1/1/0 | 8+8/8 | Marker neben der Kachel (25/36 auf); Uebergang >16 px: [18] | (unveraendert) |
| 4010 | 8 / 8 | 8/1 | 40/44 | 36/44 | 1/0/0 | 6+6/6 | ok | (unveraendert) |
| 4011 (Elza) | 8 / 8 | 8/1 | 40/44 | 36/44 | 1/0/0 | 6+6/6 | ok | (unveraendert) |
| 4020 | 8 / 8 | 8/2 | 49/49 | 25/49 | 0/0/3 | 9+9/9 | 3 Tuer(en) ohne Marke | (unveraendert) |
| 4021 (Elza) | 8 / 8 | 8/2 | 49/49 | 25/49 | 0/0/3 | 9+9/9 | 3 Tuer(en) ohne Marke | (unveraendert) |
| 4030 | 8 / 8 | 8/3 | 47/49 | 49/49 | 1/2/0 | 6+6/6 | Uebergang >16 px: [18] | (unveraendert) |
| 4031 (Elza) | 8 / 8 | 8/3 | 47/49 | 49/49 | 1/2/0 | 6+6/6 | Uebergang >16 px: [18] | (unveraendert) |
| 4040 | 8 / 8 | 8/12 | 20/21 | 7/21 | 0/4/0 | 13+13/13 | Marker steht z0 (7/21 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4041 (Elza) | 8 / 8 | 8/12 | 20/21 | 7/21 | 0/4/0 | 13+13/13 | Marker steht z0 (7/21 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4050 | 8 / 8 | 8/2 | 38/38 | 5/38 | 12/2/0 | 15+15/15 | Marker steht z0 (5/38 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4051 (Elza) | 8 / 8 | 8/2 | 38/38 | 5/38 | 12/2/0 | 15+15/15 | Marker steht z0 (5/38 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4070 | 8 / 11 | 8/7 | 0/49 | 49/49 | 0/3/0 | 14+14/14 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (0/49 auf); Uebergang >16 px: [32, 41, 91] | (unveraendert) |
| 4071 (Elza) | 8 / 11 | 8/7 | 0/49 | 49/49 | 0/3/0 | 14+14/14 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (0/49 auf); Uebergang >16 px: [32, 41, 91] | (unveraendert) |
| 4080 | 8 / 8 | 8/8 | 19/21 | 17/21 | 2/1/0 | 9+9/9 | ok | (unveraendert) |
| 4081 (Elza) | 8 / 8 | 8/8 | 19/21 | 17/21 | 2/1/0 | 9+9/9 | ok | (unveraendert) |
| 4090 | 8 / 8 | 8/9 | 36/49 | 49/49 | 1/0/0 | 6+6/6 | Marker neben der Kachel (36/49 auf) | (unveraendert) |
| 4091 (Elza) | 8 / 8 | 8/9 | 36/49 | 49/49 | 1/0/0 | 6+6/6 | Marker neben der Kachel (36/49 auf) | (unveraendert) |
| 40A0 | 8 / 8 | 8/10 | 32/32 | 30/32 | 1/0/1 | 8+8/8 | 1 Tuer(en) ohne Marke | (unveraendert) |
| 40A1 (Elza) | 8 / 8 | 8/10 | 32/32 | 30/32 | 1/0/1 | 8+8/8 | 1 Tuer(en) ohne Marke | (unveraendert) |
| 40B0 | 8 / 8 | 8/11 | 29/49 | 30/49 | 1/0/0 | 5+5/5 | Marker neben der Kachel (29/49 auf) | (unveraendert) |
| 40B1 (Elza) | 8 / 8 | 8/11 | 29/49 | 30/49 | 1/0/0 | 5+5/5 | Marker neben der Kachel (29/49 auf) | (unveraendert) |
| 5000 | 9 / 9 | 9/0 | 24/39 | 39/39 | 2/1/0 | 9+9/9 | Marker neben der Kachel (24/39 auf); Uebergang >16 px: [20, 22, 58] | (unveraendert) |
| 5001 (Elza) | 9 / 9 | 9/0 | 24/39 | 39/39 | 2/1/0 | 9+9/9 | Marker neben der Kachel (24/39 auf); Uebergang >16 px: [20, 22, 58] | (unveraendert) |
| 5010 | 9 / 9 | 9/1 | 49/49 | 49/49 | 1/0/0 | 5+5/5 | Uebergang >16 px: [20] | (unveraendert) |
| 5011 (Elza) | 9 / 9 | 9/1 | 49/49 | 49/49 | 1/0/0 | 5+5/5 | Uebergang >16 px: [20] | (unveraendert) |
| 5020 | 9 / 9 | 9/7 | 17/49 | 35/49 | 0/1/0 | 0+0/13 | rot fehlt 13/13; Schleier 0/13; Marker neben der Kachel (17/49 auf); Uebergang >16 px: [58] | (unveraendert) |
| 5021 (Elza) | 9 / 9 | 9/7 | 17/49 | 35/49 | 0/2/0 | 0+0/13 | rot fehlt 13/13; Schleier 0/13; Marker neben der Kachel (17/49 auf); Uebergang >16 px: [46, 58] | (unveraendert) |
| 5030 | 9 / 9 | 9/3 | 28/35 | 35/35 | 1/3/0 | 12+12/12 | Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5031 (Elza) | 9 / 9 | 9/3 | 28/35 | 35/35 | 1/3/0 | 12+12/12 | Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5040 | 9 / 9 | 9/5 | 18/18 | 7/18 | 1/2/0 | 7+7/7 | Marker steht z0 (7/18 Pos.); Uebergang >16 px: [23, 28] | (unveraendert) |
| 5041 (Elza) | 9 / 9 | 9/5 | 18/18 | 7/18 | 1/2/0 | 7+7/7 | Marker steht z0 (7/18 Pos.); Uebergang >16 px: [23, 28] | (unveraendert) |
| 5050 | 9 / 9 | 9/4 | 14/49 | 42/49 | 1/0/0 | 6+6/6 | Marker neben der Kachel (14/49 auf) | (unveraendert) |
| 5051 (Elza) | 9 / 9 | 9/4 | 14/49 | 42/49 | 1/0/0 | 6+6/6 | Marker neben der Kachel (14/49 auf) | (unveraendert) |
| 5060 | 9 / 9 | 9/6 | 36/37 | 37/37 | 0/1/0 | 13+13/13 | Uebergang >16 px: [24] | (unveraendert) |
| 5061 (Elza) | 9 / 9 | 9/6 | 36/37 | 37/37 | 0/1/0 | 13+13/13 | Uebergang >16 px: [24] | (unveraendert) |
| 5070 | 9 / 9 | - | - | 0/0 | 0/0/1 | 0+0/10 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5071 (Elza) | 9 / 9 | - | - | 0/0 | 0/0/1 | 0+0/10 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5080 | 9 / 9 | 9/9 | 33/49 | 49/49 | 0/1/0 | 5+5/5 | Marker neben der Kachel (33/49 auf) | (unveraendert) |
| 5081 (Elza) | 9 / 9 | 9/9 | 33/49 | 49/49 | 0/1/0 | 5+5/5 | Marker neben der Kachel (33/49 auf) | (unveraendert) |
| 5090 | 9 / 9 | 9/7 | 13/43 | 43/43 | 4/2/0 | 7+0/16 | rot fehlt 9/16; Schleier 0/16; Marker neben der Kachel (13/43 auf) | (unveraendert) |
| 5091 (Elza) | 9 / 9 | 9/7 | 13/43 | 43/43 | 4/2/0 | 7+0/16 | rot fehlt 9/16; Schleier 0/16; Marker neben der Kachel (13/43 auf) | (unveraendert) |
| 50A0 | 9 / 9 | 9/14 | 10/34 | 10/34 | 0/2/0 | 14+14/14 | Marker neben der Kachel (10/34 auf); Marker steht z0 (10/34 Pos.); Uebergang >16 px: [38] | (unveraendert) |
| 50A1 (Elza) | 9 / 9 | 9/14 | 10/34 | 10/34 | 0/2/0 | 14+14/14 | Marker neben der Kachel (10/34 auf); Marker steht z0 (10/34 Pos.); Uebergang >16 px: [38] | (unveraendert) |
| 50B0 | 9 / 9 | 9/11 | 36/49 | 49/49 | 0/2/0 | 4+4/4 | Marker neben der Kachel (36/49 auf); Uebergang >16 px: [38] | (unveraendert) |
| 50B1 (Elza) | 9 / 9 | 9/11 | 36/49 | 49/49 | 0/2/0 | 4+4/4 | Marker neben der Kachel (36/49 auf); Uebergang >16 px: [38] | (unveraendert) |
| 50C0 | 10 / 10 | 10/0 | 24/25 | 25/25 | 1/0/0 | 7+7/7 | ok | (unveraendert) |
| 50C1 (Elza) | 10 / 10 | 10/0 | 24/25 | 25/25 | 1/0/0 | 7+7/7 | ok | (unveraendert) |
| 50D0 | 10 / 11 | 10/1 | 2/49 | 49/49 | 3/1/0 | 14+14/14 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (2/49 auf); Uebergang >16 px: [42, 73] | (unveraendert) |
| 50D1 (Elza) | 10 / 11 | 10/1 | 2/49 | 49/49 | 3/1/0 | 14+14/14 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (2/49 auf); Uebergang >16 px: [42, 73] | (unveraendert) |
| 50E0 | 10 / 10 | 10/2 | 40/49 | 35/49 | 1/0/0 | 10+10/10 | ok | (unveraendert) |
| 50E1 (Elza) | 10 / 10 | 10/2 | 40/49 | 35/49 | 1/0/0 | 10+10/10 | ok | (unveraendert) |
| 50F0 | 11 / 11 | 11/2 | 35/49 | 49/49 | 0/1/0 | 10+10/10 | Marker neben der Kachel (35/49 auf); Uebergang >16 px: [42] | (unveraendert) |
| 50F1 (Elza) | 11 / 11 | 11/2 | 35/49 | 49/49 | 0/1/0 | 10+10/10 | Marker neben der Kachel (35/49 auf); Uebergang >16 px: [42] | (unveraendert) |
| 5100 | 11 / 11 | 11/1 | 25/38 | 38/38 | 1/1/0 | 10+10/10 | Marker neben der Kachel (25/38 auf); Uebergang >16 px: [32, 74] | (unveraendert) |
| 5101 (Elza) | 11 / 11 | 11/1 | 25/38 | 38/38 | 1/1/0 | 10+10/10 | Marker neben der Kachel (25/38 auf); Uebergang >16 px: [32, 74] | (unveraendert) |
| 5110 | 9 / 9 | 9/3 | 31/37 | 37/37 | 0/0/4 | 7+7/7 | 4 Tuer(en) ohne Marke; Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5111 (Elza) | 9 / 9 | 9/3 | 31/37 | 37/37 | 0/0/4 | 7+7/7 | 4 Tuer(en) ohne Marke; Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5120 | 9 / 9 | 9/5 | 23/23 | 7/23 | 0/0/1 | 7+7/7 | Marker steht z0 (7/23 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [28] | (unveraendert) |
| 5121 (Elza) | 9 / 9 | 9/5 | 23/23 | 7/23 | 0/0/1 | 7+7/7 | Marker steht z0 (7/23 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [28] | (unveraendert) |
| 5130 | 9 / 9 | - | - | 0/0 | 0/0/3 | 0+0/10 | KEINE Zone (Karte leer, kein Marker); 3 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5131 (Elza) | 9 / 9 | - | - | 0/0 | 0/0/3 | 0+0/10 | KEINE Zone (Karte leer, kein Marker); 3 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5140 | 9 / 9 | 9/14 | 3/27 | 11/27 | 0/2/0 | 14+14/14 | Marker neben der Kachel (3/27 auf); Marker steht z0 (11/27 Pos.); Uebergang >16 px: [38, 38] | (unveraendert) |
| 5141 (Elza) | 9 / 9 | 9/14 | 3/27 | 11/27 | 0/2/0 | 14+14/14 | Marker neben der Kachel (3/27 auf); Marker steht z0 (11/27 Pos.); Uebergang >16 px: [38, 38] | (unveraendert) |
| 6000 | 12 / 12 | 12/7 | 34/48 | 7/48 | 2/0/1 | 13+13/13 | Marker neben der Kachel (34/48 auf); Marker steht z0 (7/48 Pos.); 1 Tuer(en) ohne Marke | (unveraendert) |
| 6001 (Elza) | 12 / 12 | 12/7 | 34/48 | 7/48 | 2/0/1 | 13+13/13 | Marker neben der Kachel (34/48 auf); Marker steht z0 (7/48 Pos.); 1 Tuer(en) ohne Marke | (unveraendert) |
| 6010 | 12 / 12 | 12/4 | 20/49 | 18/49 | 1/1/0 | 14+14/14 | Marker neben der Kachel (20/49 auf); Marker steht z0 (18/49 Pos.) | (unveraendert) |
| 6011 (Elza) | 12 / 12 | 12/4 | 20/49 | 18/49 | 1/1/0 | 14+14/14 | Marker neben der Kachel (20/49 auf); Marker steht z0 (18/49 Pos.) | (unveraendert) |
| 6020 | 12 / 12 | 12/5;12/6 | 58/98 | 98/98 | 0/2/0 | 9+9/9 | Marker neben der Kachel (58/98 auf) | (unveraendert) |
| 6021 (Elza) | 12 / 12 | 12/5;12/6 | 58/98 | 98/98 | 0/2/0 | 9+9/9 | Marker neben der Kachel (58/98 auf) | (unveraendert) |
| 6030 | 12 / 12 | 12/7 | 35/35 | 10/35 | 3/1/0 | 14+14/14 | Marker steht z0 (10/35 Pos.) | (unveraendert) |
| 6031 (Elza) | 12 / 12 | 12/7 | 35/35 | 10/35 | 3/1/0 | 14+14/14 | Marker steht z0 (10/35 Pos.) | (unveraendert) |
| 6040 | 12 / 12 | - | - | 0/0 | 0/0/0 | 0+0/9 | KEINE Zone (Karte leer, kein Marker) | (unveraendert) |
| 6041 (Elza) | 12 / 12 | - | - | 0/0 | 0/0/0 | 0+0/9 | KEINE Zone (Karte leer, kein Marker) | (unveraendert) |
