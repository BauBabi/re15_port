# Karte STAGE1-6: Messlatte und Bestand (Strang r42/karte, Stufe 1, Runde 2)

Stand: Zweig r42/karte, Basis aaea5f8c2, Runde 1 = cb4f078c1, diese Runde = Arbeitsstand vor dem Commit
(Commit-Id im Bericht). Gemessen mit
- der Sonde `re15_port/tests/unit/probe_r42_karte_bestand.c` (echter Pfad: Raum laden wie das Spiel, Zone aus der
  Lage, echte Op-Liste `re15_inv_screen_build`, Marker `re15_inv_map_marker`), jeder Raum, Leon (xxx0) UND Elza
  (xxx1), nur der eigene Raum besucht, kein Plan im Besitz;
- Kartenschirm-Abzuegen aus dem ECHTEN Spiel (`RE15_INV_FB_SHOT`, Spielstand per `probe_r36_karte`, Bild 96 =
  helle Pulsphase des Markers, nur der eigene Raum besucht - der Schleier ist also sichtbar), je KAMERA eine Lage
  in STAGE2/3 (Leon) und bis zu drei Kameras je Raum in STAGE4-6, dazu je Elza-Raum eine Lage (§5);
- dem Riegel `unit_r42_karte_st23` ([A]-[H], §4).

## 1. Messlatte = was STAGE1 heute kann (gemessen, unveraendert seit aaea5f8c2)

| # | Punkt | STAGE1 gemessen | Beleg |
|---|---|---|---|
| M1 | Blatt je Bereich/Etage | jeder Raum mit Zone zeigt sein Blatt; Etagenwechsel ueber das Band (1060 Treppe, 1080 Fahrstuhl, 1090, 1170) | `re15_inv_map_page_shown`, Original-Init @0x8004b568; Pins unit_map_floor_visited, unit_map_etagen_rect |
| M2 | Raum als Grundriss | 74 von 78 Raeumen mit Zone (ohne: 1240/1241, 1260/1261); Marker auf den gemalten Texeln des eigenen Rechtecks 82 % (2272/2770 Rasterpunkte); ungemalte Raeume als Schema-Kasten | Sonde |
| M3 | Tuersymbole | 132 von 195 Tueren mit Marke ueber die Identitaet (+8 ungepaart); Marken auf der gemeinsamen Kante; gemalte Tuersymbole tragen ihre Marke auf der Wandlinie: Blatt 1 9/9 (3-4 px vom Symbolmittelpunkt), Blatt 2 9/9 (2-4 px), Blatt 4 6/6 (2-3 px); Blatt 0 (B1-Schema) 4-19 px, Blatt 3 (2F, Ersatz-Seitentabelle) 14-40 px | Sonde, Pin unit_map_durchgang, Katalog analysis/kartensymbole |
| M4 | Schleier | nur der eigene Raum gezeichnet (ohne Plan): 770/850 Kamera-Stichproben; Ausnahmen 10B0, 1110 (Teilbereiche), 1240/1260 | Sonde |
| M5 | Zeichenreihenfolge | aktueller Raum zuerst = oben, sichtbar rot 778/850 (gleiche Ausnahmen) | integration_map_raum_live |
| M6 | Marker an der richtigen Stelle, laeuft mit | 85 % verschiedene Marker-Positionen ueber das 7x7-Raster je Zone; einzig 10B0 steht | Sonde |
| M7 | Marker-RICHTUNG (kein Spiegel) | alle STAGE1-Zonen nur 0/0 oder 1/1 (Drehung 0 oder 180 Grad); Kreuzprodukt der Abbildung wie bei den ausgelieferten Zeilen (FUN_800473f8 @0x8004741c-0x80047528) | Riegel [E] |
| M8 | Treppen | Treppenmarken aus den SCD-Zonen Aot_set Typ 12/13: Blatt 0: 2, 1: 2, 2: 4, 3: 2, 4: 3, 5: 1 | Header s_map_marks kind 4/5 |
| M9 | Seitenwechsel | Blatt folgt Raum/Band; Tuer auf ein anderes Blatt = Blattwechsel | Sonde DOOR-Zeilen |
| M10 | Sprung beim Tuerdurchgang | Median 4 px, 8 von 149 ueber 8 px | Sonde DOOR-Zeilen |

STAGE1 ist selbst nicht fehlerfrei: 10B0 ohne Rot/Schleier und mit stehendem Marker (Schema), 1110 Schleier 7/13,
10A0 Marker 0/49 auf der Kachel, 1240/1260 ohne Zone, Blatt 3 und Blatt 0 mit weiten Tuermarken. Diese Punkte
gehoeren nicht zu diesem Strang-Schritt und sind nur als Grenze der Messlatte vermerkt.

## 2. Zusammenfassung STAGE2/3 (Sonde, Leon+Elza)

| | aaea5f8c2 | Runde 1 (cb4f078c1) | jetzt |
|---|---|---|---|
| STAGE2 Raeume mit Zone | 18/24 | 22/24 | 22/24 (20B0 von keiner Tuer erreichbar) |
| STAGE2 Tueren mit eigener Marke | 32+6/58 | 36+4/58 | 36+4/58 |
| STAGE2 gespiegelte Zonen | 2040 (1,0) | 2070 z1 (1,0), 20A0 (0,1) | keine |
| STAGE3 Tueren mit eigener Marke | 31+20/59 | 43/59 | 49/59 (Rest: Skript-Warps mit Trigger 0x0) |
| STAGE3 Tuersprung Median / max (gleiches Blatt) | 19 / 75 px | 8 / 74 px | 7,5 / 56 px (56 und 47 sind Skript-Warps; begehbare Tueren max 17 px) |
| STAGE3 gespiegelte Zonen | 3050 (1,0) | 3050, 30A0 (1,0) | keine |
| Treppenmarken Blatt 6/7 | 3 / 4 | 0 / 0 | 2 / 4 |
| gemalte Tuersymbole mit Marke (<= 4 px) Blatt 6/7 | 4/6, 6/15 | 6/6, 11/15 | 6/6, 14/15 |

Die Zeilen "Marker auf eigener Kachel" (STAGE2 40 % -> 96 %, STAGE3 76 % -> 93 %) stehen hier bewusst NICHT
als Guetemass: bei hergeleiteten Zeilen ist das per Konstruktion erfuellt (Pruefbericht R2). Sie halten nur
fest, dass kein Raum auf einem fremden Rechteck liegt.

## 3. Was in Runde 2 gilt (Drehung je Raum mit Belegen)

Alle Zeilen der Stub-Raeume sind PORT-ERGAENZUNG (Original-Slots {0,0,1,1}). Die Drehung ist je Raum im Plan von
`re15_port/tools/karte_r42_zeilen.py` festgelegt und wird dort gegen die Tuer-Belege GEPRUEFT (Ausgabe:
`python re15_port/tools/karte_r42_zeilen.py 6|7`). Belegarten: (a) gemaltes Tuersymbol an der Grenze zu genau
diesem Nachbarn, (b) Gegen-Tuer mit AUSGELIEFERTER Zeile, (c) Lage des Nachbar-Rechtecks.

| Raum | Rechteck | Drehung | Stimmen dafuer / dagegen | wichtigste Belege |
|---|---|---|---|---|
| 2010 | r1 | R180 (1,1) | 3 / 0 | (b) 2000 @0x800769E0 -> (91,131); (a) gemalt (88,131) E |
| 2020 | r4 | R180 | 4 / 0 (2 Tueren) | (a) (125,98) S an r2, (182,120) N an r6 |
| 2030 | r6 | R180 | 6 / 0 (4 Tueren) | (a) (182,120) N, (201,126) W; (c) r3/r4/r7/r8 |
| 2040 | r5 | R180 | 2 / 0 (2 Eck-Tueren, Schnitt eindeutig) | (c) r2 im Westen, r7 im Sueden |
| 2050 | r2 | R180 | 4 / 0 (3 Tueren) | (b) Eck-Tuer 2000 @0x800769E0 -> (91,103) = SW-Ecke |
| 2060 | r7 | R180 | 3 / 0 | (a) (201,126) W |
| 2070 Zone 0 | r3 | Rahmen von 2000 (R0) | - | Trigger 2000->2070 (-19800,-6200) / Spawn (-18650,-6450) |
| 2070 Zone 1 | r3 | R180 | 2 / 0 | (a) (105,141) E an r0; (c) r6 (unerreichbar: keine Tuer setzt dort ab) |
| 2080 | r8 | R0 | 2 / 0 | (c) r6 im Westen, r9 im Osten |
| 2090 | r9 | R0 | 2 / 0 | (c) r8 im Westen, r10 im Osten |
| 20A0 | r10 | R90ccw (Tausch, 1,0) | 1 / 0 - PORT-WAHL | Nordwand (Trigger 3000x1000 + Ankunfts-Yaw) -> Westseite von r10; Original: Blatt 0xD ohne Kunst |
| 3020, 30D0 Zone 0 | r2 | R0 | 3 / 0 | (b) 3010 @0x80076A48 -> (167,188); (a) (170,174) W |
| 3050 | r5 | R90cw (Tausch, 0,1) | 7 / 0 (2 Tueren) | (b) 3040 @0x80076A60 -> (194,57); (a) (194,54) W und (187,98) E |
| 30A0 | r10 | R90ccw (Tausch, 1,0) | 2 / 0 (2 Belegarten) | (b) 3090 @0x80076A88 -> (244,155), Gegen-Trigger 1510x570 waagerecht; (a) (242,158) N |
| 30B0 | r11 | R0 | 2 / 0 | (b) 3090 -> (222,189); (a) (218,191) E |
| 30C0, 30D0 Zone 1 | r12 | R0 | 6 / 0 | (b) 3090 -> (217,137), 3030 @0x80076A58 -> (208,127); (a) (218,132) W, (208,126) S |
| 30E0 Zone 0 | r4 Suedband | R0 | 6 / 0 | (a) (187,98) E an r5; (b) 3030 -> (208,106) |

Ausgeliefert und unveraendert: 2000 @0x800769E0, 3000 @0x80076A40, 3010 @0x80076A48, 3030 @0x80076A58,
3040 @0x80076A60, 3060 @0x80076A70, 3070 @0x80076A78, 3080 @0x80076A80, 3090 @0x80076A88, 30E0 Zone 1 @0x80076AB0.

## 4. Pruefgroessen ausserhalb des Fits (Riegel unit_r42_karte_st23)

- [E] Haendigkeit: 226 Zonen aller Blaetter, keine gespiegelt (aaea5f8c2: 2040, 3050; cb4f078c1: 2070 z1, 20A0, 3050, 30A0).
- [F] Marker an der Tuer gegen gemaltes Symbol bzw. ausgelieferte Nachbar-Zeile, 17 Tueren hergeleiteter Raeume:
  3, 3, 4, 3, 5, 5, 6, 3, 7, 4, 5, 2, 4, 2, 8, 6, 5 px (Median 4, max 8), und jedes Mal naeher als die Zonenmitte.
  Diese Belege haben nur die DREHUNG gewaehlt (diskret), nicht Versatz und Massstab.
- Projektion beider Seiten eines Durchgangs -> gemaltes Symbol (Werkzeugausgabe): wo BEIDE Zeilen ausgeliefert sind,
  liegt das Original selbst 0-21 px daneben (3040<->3060: 13 / 21 px; 3010<->3060: 0 / 4 px).
- [G] Treppen: 3040 (203,75)/(231,76) und 3010 (135,188) aus AUSGELIEFERTEN Zeilen exakt; 30C0 (189,131),
  2030 (187,124), 2080 (226,155) aus hergeleiteten Zeilen - fuer diese drei gibt es kein gemaltes Treppensymbol,
  an dem man sie unabhaengig pruefen koennte (die gemalten Treppen auf Blatt 6 sitzen an Durchgaengen/Leitern).
- [H] gemalte Tuersymbole: Blatt 6 6/6, Blatt 7 14/15 mit Marke <= 4 px neben der Wandlinie.

## 5. Kartenschirm im echten Spiel (FB_SHOT)

Abzug bei Inventarbild 96 (helle Pulsphase), nur der eigene Raum besucht, kein Plan. Gesucht wird der gelbe
Marker-Ring; Erwartung = Wert der Sonde fuer dieselbe Lage. Weicht der Ring ab, wird die Erwartung aus der
TATSAECHLICHEN Spielerlage im Spiel (debug.log "[walk] ... pl pos") neu gerechnet (`r2/nachpruefen.py`).

| Stage | Abzuege geplant | Marker genau (<= 1 px) | abweichend, aber Figur vom Spiel verschoben (0 px zur echten Lage) | sonst abweichend | kein Marker | kein Abzug |
|---|---|---|---|---|---|---|
| STAGE2 (je Kamera) | 83 | 60 | 6 | 0 | 0 | 17 (20B0/20B1: kein Raum des Spiels fuehrt hinein) |
| STAGE3 (je Kamera) | 119 | 97 | 12 | 0 | 0 | 10 (3000: Eingangsszene laeuft, Karte oeffnet nicht; 3080 dito) |
| STAGE4 (bis 3 Kameras/Raum) | 40 | 24 | 8 | 0 | 0 | 8 (4000, 4010, 4031: Raum-Ereignis) |
| STAGE5 (bis 3 Kameras/Raum) | 75 | 66 | 1 | 0 | 8 (5070/5130: keine Zone) | 0 |
| STAGE6 (bis 3 Kameras/Raum) | 18 | 5 | 0 | 4 (6010 4 px, 6020 ohne Zeile) | 0 | 9 (6000, 6030, 6040) |

Schleier: in keinem STAGE2/3-Abzug ist ein fremder Raum gezeichnet; verletzt nur in 5090/5091 (besucht-Fuellung
sichtbar, STAGE5). Messbilder: `messbild_blatt6_r2.png` / `messbild_blatt7_r2.png` (Marker je Raum, letzte Kachel
alle Orte besucht) und `messbild_richtung_blatt6_r2.png` / `_blatt7_r2.png` (Richtung je Achse).

## 6. Offen je Stage

- **STAGE2:** 2040-Suedteil (mit Treppe Aot_set sce 13 bei (-1350,-16100)) und 2070 Zone 2 (Gang 2040 -> 11A0)
  sind nirgends gemalt - Marker am Rand. ROOM11A0 (STAGE1) teilt Blatt 6 rect 2 mit 2050. 20A0 auf r10 ist
  PORT-WAHL (Original Blatt 0xD ohne Kunst) und nur 11x8 px gross. 2030<->2080 (Treppentuer) springt 17 px.
  20B0 von keiner Tuer erreichbar.
- **STAGE3:** gemaltes Tuersymbol (198,163) N zwischen r2 und r12 ohne Tuer-Datensatz; 30E0 Zone 0 leuchtet als
  ganze r4-Kachel (geteilt mit 3040), der Nordarm des L-Gangs hat keine eigene Kunst; 3030/3080 sind 8 px breit
  gemalt (ausgelieferte Zeilen); 3000/3080 lassen sich per Spielstand nicht abziehen (Raum-Ereignis).
- **STAGE4 (Blatt 8):** Marker steht in 4040 (7/21) und 4050 (5/38); 4070 zeigt Blatt 11 statt 8 (Etagenzeile);
  4000 nur 25/36 auf der Kachel (ausgelieferte Zeile @0x80076AB8); 4020 drei Tueren ohne Marke; 4000/4010/4031
  im Spiel nicht abziehbar (Raum-Ereignis).
- **STAGE5 (Blaetter 9-11):** 5070 und 5130 ohne Zone; 5020 und 5090 ohne sichtbares Rot, 5090 Schleier verletzt;
  5040/5120 liegen auf r5, ihre ausgelieferte Zeile @0x80076B38/@0x80076BA8 trifft r4; 50A0/5140 auf r14, Zeile
  @0x80076B68/@0x80076BB8 trifft r10; 5100 hat eine ausgelieferte Zeile @0x80076B98, die Zone nicht; 50D0 zeigt
  Blatt 11 statt 10; Spruenge bis 170 px.
- **STAGE6 (Blatt 12):** 6040 ohne Zone (ausgelieferte Zeile @0x80076BE0 (202,136,2193,2145) vorhanden); Marker
  steht in 6000 (7/48) und 6030 (10/35); 6020 ohne Zeile (Bbox-Streckung); 6000/6030 im Spiel nicht abziehbar.

Konflikt-Vorwarnung: U-11 (Wandplan 402 -> Laborblatt 8) und U-12 (U-Bahn-Blatt 12, Wagenkacheln) baut r42/rest
in genau den Blaettern 8 und 12, die hier als STAGE4/STAGE6 offen stehen - dieselben Tabellen
(`re15_map_zones.h` s_map_zones/s_map_marks, `s_map_rectfix`). Neu in dieser Runde: das Feld `tausch` am ENDE
von `re15_map_zone_t` (Zeilen mit 17 Werten bleiben gueltig, der Wert ist dann 0).

## 7. Tabelle je Raum STAGE2-6 (Leon und Elza)

"Befund" = automatisch aus der Sonde; "FB_SHOT" = Abzuege im echten Spiel (ok / abweichend / ohne Marker / ohne Abzug).

| Raum | Blatt orig/gez. | Rechteck | Marker auf Kachel | Mitlaufen | Tueren ident/einzeln/ohne | rot+Schleier (Kam.) | FB_SHOT je Kamera (ok/daneben/ohne Marker/ohne Abzug) | Befund aaea5f8c2 | Befund jetzt |
|---|---|---|---|---|---|---|---|---|---|
| 2000 | 6 / 6 | 6/0 | 24/24 | 24/24 | 2/2/0 | 16+16/16 | 16/0/0/0 | Uebergang >16 px: [29, 29, 101] | Uebergang >16 px: [29, 101] |
| 2001 (Elza) | 6 / 6 | 6/0 | 24/24 | 24/24 | 2/2/0 | 16+16/16 | 1/0/0/0 | Uebergang >16 px: [29, 29, 101] | Uebergang >16 px: [29, 101] |
| 2010 | 6 / 6 | 6/1 | 49/49 | 49/49 | 1/0/0 | 6+6/6 | 1/1/0/0 (k2010_c00 DANEBEN 3 px) | Marker neben der Kachel (35/49 auf) | ok |
| 2011 (Elza) | 6 / 6 | 6/1 | 49/49 | 49/49 | 1/0/0 | 6+6/6 | 0/1/0/0 (k2011_c00 DANEBEN 3 px) | Marker neben der Kachel (35/49 auf) | ok |
| 2020 | 6 / 6 | 6/4 | 49/49 | 42/49 | 1/0/0 | 7+7/7 | 1/0/0/0 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [160] | ok |
| 2021 (Elza) | 6 / 6 | 6/4 | 49/49 | 42/49 | 1/0/0 | 7+7/7 | 1/0/0/0 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [160] | ok |
| 2030 | 6 / 6 | 6/6 | 45/45 | 45/45 | 3/0/0 | 16+16/16 | 4/1/0/0 (k2030_c00 DANEBEN 2 px) | Marker neben der Kachel (18/45 auf) | Uebergang >16 px: [17] |
| 2031 (Elza) | 6 / 6 | 6/6 | 45/45 | 45/45 | 3/0/0 | 16+16/16 | 0/1/0/0 (k2031_c00 DANEBEN 2 px) | Marker neben der Kachel (18/45 auf) | Uebergang >16 px: [17] |
| 2040 | 6 / 6 | 6/5 | 43/43 | 42/43 | 2/0/2 | 16+16/16 | 4/0/0/0 | Marker neben der Kachel (6/28 auf); Marker steht z0 (12/28 Pos.); Uebergang >16 px: [51] | 2 Tuer(en) ohne Marke |
| 2041 (Elza) | 6 / 6 | 6/5 | 43/43 | 42/43 | 2/0/2 | 16+16/16 | 1/0/0/0 | Marker neben der Kachel (6/28 auf); Marker steht z0 (12/28 Pos.); Uebergang >16 px: [51] | 2 Tuer(en) ohne Marke |
| 2050 | 6 / 6 | 6/2 | 41/49 | 49/49 | 3/0/0 | 16+16/16 | 2/0/0/0 | Marker neben der Kachel (0/49 auf); Marker steht z0 (1/49 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [56, 160] | ok |
| 2051 (Elza) | 6 / 6 | 6/2 | 41/49 | 49/49 | 3/0/0 | 16+16/16 | 1/0/0/0 | Marker neben der Kachel (0/49 auf); Marker steht z0 (1/49 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [56, 160] | ok |
| 2060 | 6 / 6 | 6/7 | 49/49 | 49/49 | 2/0/0 | 13+13/13 | 10/0/0/0 | Marker neben der Kachel (28/49 auf); Uebergang >16 px: [51] | ok |
| 2061 (Elza) | 6 / 6 | 6/7 | 49/49 | 49/49 | 2/0/0 | 13+13/13 | 1/0/0/0 | Marker neben der Kachel (28/49 auf); Uebergang >16 px: [51] | ok |
| 2070 | 6 / 6 | 6/3 | 128/147 | 79/147 | 2/0/2 | 13+13/13 | 6/0/0/0 | Marker neben der Kachel (58/147 auf); 2 Tuer(en) ohne Marke; Uebergang >16 px: [29, 54, 59] | 2 Tuer(en) ohne Marke |
| 2071 (Elza) | 6 / 6 | 6/3 | 128/147 | 79/147 | 2/0/2 | 13+13/13 | 1/0/0/0 | Marker neben der Kachel (58/147 auf); 2 Tuer(en) ohne Marke; Uebergang >16 px: [29, 54, 59] | 2 Tuer(en) ohne Marke |
| 2080 | 6 / 6 | 6/8 | 98/98 | 91/98 | 1/0/0 | 9+9/9 | 3/0/0/0 | Marker neben der Kachel (12/98 auf); 1 Tuer(en) ohne Marke | Uebergang >16 px: [17] |
| 2081 (Elza) | 6 / 6 | 6/8 | 98/98 | 91/98 | 1/0/0 | 9+9/9 | 1/0/0/0 | Marker neben der Kachel (12/98 auf); 1 Tuer(en) ohne Marke | Uebergang >16 px: [17] |
| 2090 | 6 / 6 | 6/9 | 49/49 | 49/49 | 1/0/0 | 13+13/13 | 0/1/0/0 (k2090_c00 DANEBEN 3 px) | Marker neben der Kachel (33/49 auf) | ok |
| 2091 (Elza) | 6 / 6 | 6/9 | 49/49 | 49/49 | 1/0/0 | 13+13/13 | 0/1/0/0 (k2091_c00 DANEBEN 3 px) | Marker neben der Kachel (33/49 auf) | ok |
| 20A0 | 13 / 6 | 6/10 | 36/36 | 21/36 | 0/0/1 | 16+16/16 | 5/0/0/0 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke | Blatt weicht vom Original-Init ab (16 Kam.); 1 Tuer(en) ohne Marke |
| 20A1 (Elza) | 13 / 6 | 6/10 | 36/36 | 21/36 | 0/0/1 | 16+16/16 | 1/0/0/0 | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke | Blatt weicht vom Original-Init ab (16 Kam.); 1 Tuer(en) ohne Marke |
| 20B0 | 13 / 13 | - | - | 0/0 | 0/0/4 | 0+0/16 | 0/0/0/16 (k20B0_c00 kein Abzug, k20B0_c01 kein Abzug, k20B0_c02 kein Abzug) | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] |
| 20B1 (Elza) | 13 / 13 | - | - | 0/0 | 0/0/4 | 0+0/16 | 0/0/0/1 (k20B1_c00 kein Abzug) | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] | KEINE Zone (Karte leer, kein Marker); 4 Tuer(en) ohne Marke; Uebergang >16 px: [141, 195] |
| 3000 | 7 / 7 | 7/0 | 38/43 | 43/43 | 1/0/1 | 4+4/4 | 0/0/0/4 (k3000_c00 kein Abzug, k3000_c01 kein Abzug, k3000_c02 kein Abzug) | 1 Tuer(en) ohne Marke | 1 Tuer(en) ohne Marke |
| 3001 (Elza) | 7 / 7 | 7/0 | 38/43 | 43/43 | 1/0/1 | 4+4/4 | 0/0/0/1 (k3001_c00 kein Abzug) | 1 Tuer(en) ohne Marke | 1 Tuer(en) ohne Marke |
| 3010 | 7 / 7 | 7/1 | 46/49 | 49/49 | 4/0/0 | 10+10/10 | 9/1/0/0 (k3010_c01 DANEBEN 5 px) | Uebergang >16 px: [43, 48] | Uebergang >16 px: [47] |
| 3011 (Elza) | 7 / 7 | 7/1 | 46/49 | 49/49 | 4/0/0 | 10+10/10 | 1/0/0/0 | Uebergang >16 px: [43, 48] | Uebergang >16 px: [47] |
| 3020 | 7 / 7 | 7/2 | 49/49 | 49/49 | 1/0/1 | 9+9/9 | 1/0/0/0 | 1 Tuer(en) ohne Marke | 1 Tuer(en) ohne Marke; Uebergang >16 px: [17] |
| 3021 (Elza) | 7 / 7 | 7/2 | 49/49 | 49/49 | 1/0/0 | 9+9/9 | 1/0/0/0 | ok | ok |
| 3030 | 7 / 7 | 7/3 | 42/42 | 7/42 | 2/0/0 | 2+2/2 | 2/0/0/0 | Uebergang >16 px: [19] | Marker steht z0 (7/42 Pos.) |
| 3031 (Elza) | 7 / 7 | 7/3 | 42/42 | 7/42 | 2/0/0 | 2+2/2 | 1/0/0/0 | Uebergang >16 px: [19] | Marker steht z0 (7/42 Pos.) |
| 3040 | 7 / 7 | 7/4 | 46/46 | 46/46 | 1/0/0 | 14+14/14 | 12/2/0/0 (k3040_c02 DANEBEN 3 px, k3040_c13 DANEBEN 2 px) | Uebergang >16 px: [75] | ok |
| 3041 (Elza) | 7 / 7 | 7/4 | 46/46 | 46/46 | 1/0/0 | 14+14/14 | 1/0/0/0 | Uebergang >16 px: [75] | ok |
| 3050 | 7 / 7 | 7/5 | 43/43 | 43/43 | 2/0/0 | 16+16/16 | 13/2/0/0 (k3050_c08 DANEBEN 2 px, k3050_c13 DANEBEN 2 px) | Marker neben der Kachel (29/43 auf); Marker steht z0 (21/43 Pos.); Uebergang >16 px: [37, 74] | ok |
| 3051 (Elza) | 7 / 7 | 7/5 | 43/43 | 43/43 | 2/0/0 | 16+16/16 | 1/0/0/0 | Marker neben der Kachel (29/43 auf); Marker steht z0 (21/43 Pos.); Uebergang >16 px: [37, 74] | ok |
| 3060 | 7 / 7 | 7/6 | 30/37 | 30/37 | 1/0/1 | 7+7/7 | 7/0/0/0 | Marker steht z0 (5/37 Pos.); Uebergang >16 px: [44, 56] | 1 Tuer(en) ohne Marke; Uebergang >16 px: [56] |
| 3061 (Elza) | 7 / 7 | 7/6 | 30/37 | 30/37 | 1/0/1 | 7+7/7 | 1/0/0/0 | Marker steht z0 (5/37 Pos.); Uebergang >16 px: [32, 44] | 1 Tuer(en) ohne Marke; Uebergang >16 px: [48] |
| 3070 | 7 / 7 | 7/7 | 45/48 | 48/48 | 1/0/0 | 13+13/13 | 12/1/0/0 (k3070_c10 DANEBEN 3 px) | Marker neben der Kachel (35/48 auf); Marker steht z0 (3/48 Pos.); Uebergang >16 px: [36] | ok |
| 3071 (Elza) | 7 / 7 | 7/7 | 45/48 | 48/48 | 1/0/0 | 13+13/13 | 1/0/0/0 | Marker neben der Kachel (35/48 auf); Marker steht z0 (3/48 Pos.); Uebergang >16 px: [36] | ok |
| 3080 | 7 / 7 | 7/8 | 32/38 | 5/38 | 0/0/1 | 4+4/4 | 0/0/0/4 (k3080_c00 kein Abzug, k3080_c01 kein Abzug, k3080_c02 kein Abzug) | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke |
| 3081 (Elza) | 7 / 7 | 7/8 | 32/38 | 5/38 | 0/0/1 | 4+4/4 | 0/0/0/1 (k3081_c00 kein Abzug) | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke | Marker steht z0 (5/38 Pos.); 1 Tuer(en) ohne Marke |
| 3090 | 7 / 7 | 7/9 | 26/38 | 38/38 | 3/0/0 | 16+16/16 | 14/2/0/0 (k3090_c03 DANEBEN 3 px, k3090_c11 DANEBEN 4 px) | Marker neben der Kachel (18/38 auf); Marker steht z0 (18/38 Pos.); Uebergang >16 px: [17] | Marker neben der Kachel (26/38 auf) |
| 3091 (Elza) | 7 / 7 | 7/9 | 26/38 | 38/38 | 0/0/0 | 16+16/16 | 1/0/0/0 | Marker neben der Kachel (18/38 auf); Marker steht z0 (18/38 Pos.) | Marker neben der Kachel (26/38 auf) |
| 30A0 | 7 / 7 | 7/10 | 45/48 | 48/48 | 1/0/0 | 7+7/7 | 6/1/0/0 (k30A0_c04 DANEBEN 2 px) | Marker neben der Kachel (9/48 auf) | ok |
| 30A1 (Elza) | 7 / 7 | 7/10 | 45/48 | 48/48 | 1/0/0 | 7+7/7 | 1/0/0/0 | Marker neben der Kachel (9/48 auf) | ok |
| 30B0 | 7 / 7 | 7/11 | 45/49 | 49/49 | 1/0/0 | 8+8/8 | 1/1/0/0 (k30B0_c00 DANEBEN 3 px) | Marker neben der Kachel (17/49 auf); Uebergang >16 px: [17] | ok |
| 30B1 (Elza) | 7 / 7 | 7/11 | 45/49 | 49/49 | 1/0/0 | 8+8/8 | 0/1/0/0 (k30B1_c00 DANEBEN 3 px) | Marker neben der Kachel (17/49 auf); Uebergang >16 px: [17] | ok |
| 30C0 | 7 / 7 | 7/12 | 49/49 | 49/49 | 2/0/0 | 15+15/15 | 1/0/0/0 | ok | ok |
| 30C1 (Elza) | 7 / 7 | 7/12 | 49/49 | 49/49 | 2/0/0 | 15+15/15 | 1/0/0/0 | ok | ok |
| 30D0 | 7 / 7 | 7/12 | 56/63 | 98/98 | 3/0/0 | 16+16/16 | 1/0/0/0 | Uebergang >16 px: [27, 39] | ok |
| 30D1 (Elza) | 7 / 7 | 7/12 | 56/63 | 98/98 | 3/0/0 | 16+16/16 | 1/0/0/0 | Uebergang >16 px: [27, 39] | ok |
| 30E0 | 7 / 7 | 7/4;7/13 | 69/69 | 63/69 | 3/0/2 | 10+10/10 | 6/1/0/0 (k30E0_c08 DANEBEN 2 px) | Marker neben der Kachel (45/69 auf); Marker steht z0 (5/39 Pos.); 2 Tuer(en) ohne Marke; Uebergang >16 px: [20, 36, 37] | 2 Tuer(en) ohne Marke |
| 30E1 (Elza) | 7 / 7 | 7/4;7/13 | 69/69 | 63/69 | 3/0/1 | 10+10/10 | 1/0/0/0 | Marker neben der Kachel (45/69 auf); Marker steht z0 (5/39 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [20, 36, 37] | 1 Tuer(en) ohne Marke |
| 4000 | 8 / 8 | 8/0 | 25/36 | 34/36 | 2/2/0 | 8+8/8 | 0/0/0/3 (k4000_c00 kein Abzug, k4000_c01 kein Abzug, k4000_c02 kein Abzug) | Marker neben der Kachel (25/36 auf); Uebergang >16 px: [18] | (unveraendert) |
| 4001 (Elza) | 8 / 8 | 8/0 | 25/36 | 34/36 | 1/1/0 | 8+8/8 | 0/0/0/1 (k4001_c00 kein Abzug) | Marker neben der Kachel (25/36 auf); Uebergang >16 px: [18] | (unveraendert) |
| 4010 | 8 / 8 | 8/1 | 40/44 | 36/44 | 1/0/0 | 6+6/6 | 0/0/0/3 (k4010_c00 kein Abzug, k4010_c01 kein Abzug, k4010_c02 kein Abzug) | ok | (unveraendert) |
| 4011 (Elza) | 8 / 8 | 8/1 | 40/44 | 36/44 | 1/0/0 | 6+6/6 | 1/0/0/0 | ok | (unveraendert) |
| 4020 | 8 / 8 | 8/2 | 49/49 | 25/49 | 0/0/3 | 9+9/9 | 3/0/0/0 | 3 Tuer(en) ohne Marke | (unveraendert) |
| 4021 (Elza) | 8 / 8 | 8/2 | 49/49 | 25/49 | 0/0/3 | 9+9/9 | 1/0/0/0 | 3 Tuer(en) ohne Marke | (unveraendert) |
| 4030 | 8 / 8 | 8/3 | 47/49 | 49/49 | 1/2/0 | 6+6/6 | 0/3/0/0 (k4030_c00 DANEBEN 2 px, k4030_c01 DANEBEN 2 px, k4030_c02 DANEBEN 2 px) | Uebergang >16 px: [18] | (unveraendert) |
| 4031 (Elza) | 8 / 8 | 8/3 | 47/49 | 49/49 | 1/2/0 | 6+6/6 | 0/0/0/1 (k4031_c00 kein Abzug) | Uebergang >16 px: [18] | (unveraendert) |
| 4040 | 8 / 8 | 8/12 | 20/21 | 7/21 | 0/4/0 | 13+13/13 | 3/0/0/0 | Marker steht z0 (7/21 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4041 (Elza) | 8 / 8 | 8/12 | 20/21 | 7/21 | 0/4/0 | 13+13/13 | 1/0/0/0 | Marker steht z0 (7/21 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4050 | 8 / 8 | 8/2 | 38/38 | 5/38 | 12/2/0 | 15+15/15 | 3/0/0/0 | Marker steht z0 (5/38 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4051 (Elza) | 8 / 8 | 8/2 | 38/38 | 5/38 | 12/2/0 | 15+15/15 | 1/0/0/0 | Marker steht z0 (5/38 Pos.); Uebergang >16 px: [35, 36] | (unveraendert) |
| 4070 | 8 / 11 | 8/7 | 0/49 | 49/49 | 0/3/0 | 14+14/14 | 1/0/0/0 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (0/49 auf); Uebergang >16 px: [32, 41, 91] | (unveraendert) |
| 4071 (Elza) | 8 / 11 | 8/7 | 0/49 | 49/49 | 0/3/0 | 14+14/14 | 1/0/0/0 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (0/49 auf); Uebergang >16 px: [32, 41, 91] | (unveraendert) |
| 4080 | 8 / 8 | 8/8 | 19/21 | 17/21 | 2/1/0 | 9+9/9 | 3/0/0/0 | ok | (unveraendert) |
| 4081 (Elza) | 8 / 8 | 8/8 | 19/21 | 17/21 | 2/1/0 | 9+9/9 | 1/0/0/0 | ok | (unveraendert) |
| 4090 | 8 / 8 | 8/9 | 36/49 | 49/49 | 1/0/0 | 6+6/6 | 1/2/0/0 (k4090_c01 DANEBEN 3 px, k4090_c02 DANEBEN 2 px) | Marker neben der Kachel (36/49 auf) | (unveraendert) |
| 4091 (Elza) | 8 / 8 | 8/9 | 36/49 | 49/49 | 1/0/0 | 6+6/6 | 1/0/0/0 | Marker neben der Kachel (36/49 auf) | (unveraendert) |
| 40A0 | 8 / 8 | 8/10 | 32/32 | 30/32 | 1/0/1 | 8+8/8 | 2/1/0/0 (k40A0_c01 DANEBEN 4 px) | 1 Tuer(en) ohne Marke | (unveraendert) |
| 40A1 (Elza) | 8 / 8 | 8/10 | 32/32 | 30/32 | 1/0/1 | 8+8/8 | 1/0/0/0 | 1 Tuer(en) ohne Marke | (unveraendert) |
| 40B0 | 8 / 8 | 8/11 | 29/49 | 30/49 | 1/0/0 | 5+5/5 | 0/1/0/0 (k40B0_c00 DANEBEN 2 px) | Marker neben der Kachel (29/49 auf) | (unveraendert) |
| 40B1 (Elza) | 8 / 8 | 8/11 | 29/49 | 30/49 | 1/0/0 | 5+5/5 | 0/1/0/0 (k40B1_c00 DANEBEN 2 px) | Marker neben der Kachel (29/49 auf) | (unveraendert) |
| 5000 | 9 / 9 | 9/0 | 24/39 | 39/39 | 2/1/0 | 9+9/9 | 3/0/0/0 | Marker neben der Kachel (24/39 auf); Uebergang >16 px: [20, 22, 58] | (unveraendert) |
| 5001 (Elza) | 9 / 9 | 9/0 | 24/39 | 39/39 | 2/1/0 | 9+9/9 | 1/0/0/0 | Marker neben der Kachel (24/39 auf); Uebergang >16 px: [20, 22, 58] | (unveraendert) |
| 5010 | 9 / 9 | 9/1 | 49/49 | 49/49 | 1/0/0 | 5+5/5 | 3/0/0/0 | Uebergang >16 px: [20] | (unveraendert) |
| 5011 (Elza) | 9 / 9 | 9/1 | 49/49 | 49/49 | 1/0/0 | 5+5/5 | 1/0/0/0 | Uebergang >16 px: [20] | (unveraendert) |
| 5020 | 9 / 9 | 9/7 | 17/49 | 35/49 | 0/1/0 | 0+0/13 | 3/0/0/0 | rot fehlt 13/13; Schleier 0/13; Marker neben der Kachel (17/49 auf); Uebergang >16 px: [58] | (unveraendert) |
| 5021 (Elza) | 9 / 9 | 9/7 | 17/49 | 35/49 | 0/2/0 | 0+0/13 | 1/0/0/0 | rot fehlt 13/13; Schleier 0/13; Marker neben der Kachel (17/49 auf); Uebergang >16 px: [46, 58] | (unveraendert) |
| 5030 | 9 / 9 | 9/3 | 28/35 | 35/35 | 1/3/0 | 12+12/12 | 3/0/0/0 | Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5031 (Elza) | 9 / 9 | 9/3 | 28/35 | 35/35 | 1/3/0 | 12+12/12 | 1/0/0/0 | Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5040 | 9 / 9 | 9/5 | 18/18 | 7/18 | 1/2/0 | 7+7/7 | 3/0/0/0 | Marker steht z0 (7/18 Pos.); Uebergang >16 px: [23, 28] | (unveraendert) |
| 5041 (Elza) | 9 / 9 | 9/5 | 18/18 | 7/18 | 1/2/0 | 7+7/7 | 1/0/0/0 | Marker steht z0 (7/18 Pos.); Uebergang >16 px: [23, 28] | (unveraendert) |
| 5050 | 9 / 9 | 9/4 | 14/49 | 42/49 | 1/0/0 | 6+6/6 | 2/0/0/0 | Marker neben der Kachel (14/49 auf) | (unveraendert) |
| 5051 (Elza) | 9 / 9 | 9/4 | 14/49 | 42/49 | 1/0/0 | 6+6/6 | 1/0/0/0 | Marker neben der Kachel (14/49 auf) | (unveraendert) |
| 5060 | 9 / 9 | 9/6 | 36/37 | 37/37 | 0/1/0 | 13+13/13 | 3/0/0/0 | Uebergang >16 px: [24] | (unveraendert) |
| 5061 (Elza) | 9 / 9 | 9/6 | 36/37 | 37/37 | 0/1/0 | 13+13/13 | 1/0/0/0 | Uebergang >16 px: [24] | (unveraendert) |
| 5070 | 9 / 9 | - | - | 0/0 | 0/0/1 | 0+0/10 | 0/0/3/0 (k5070_c00 kein Marker, k5070_c01 kein Marker, k5070_c02 kein Marker) | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5071 (Elza) | 9 / 9 | - | - | 0/0 | 0/0/1 | 0+0/10 | 0/0/1/0 (k5071_c00 kein Marker) | KEINE Zone (Karte leer, kein Marker); 1 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5080 | 9 / 9 | 9/9 | 33/49 | 49/49 | 0/1/0 | 5+5/5 | 1/0/0/0 | Marker neben der Kachel (33/49 auf) | (unveraendert) |
| 5081 (Elza) | 9 / 9 | 9/9 | 33/49 | 49/49 | 0/1/0 | 5+5/5 | 1/0/0/0 | Marker neben der Kachel (33/49 auf) | (unveraendert) |
| 5090 | 9 / 9 | 9/7 | 13/43 | 43/43 | 4/2/0 | 7+0/16 | 3/0/0/0 | rot fehlt 9/16; Schleier 0/16; Marker neben der Kachel (13/43 auf) | (unveraendert) |
| 5091 (Elza) | 9 / 9 | 9/7 | 13/43 | 43/43 | 4/2/0 | 7+0/16 | 1/0/0/0 | rot fehlt 9/16; Schleier 0/16; Marker neben der Kachel (13/43 auf) | (unveraendert) |
| 50A0 | 9 / 9 | 9/14 | 10/34 | 10/34 | 0/2/0 | 14+14/14 | 3/0/0/0 | Marker neben der Kachel (10/34 auf); Marker steht z0 (10/34 Pos.); Uebergang >16 px: [38] | (unveraendert) |
| 50A1 (Elza) | 9 / 9 | 9/14 | 10/34 | 10/34 | 0/2/0 | 14+14/14 | 1/0/0/0 | Marker neben der Kachel (10/34 auf); Marker steht z0 (10/34 Pos.); Uebergang >16 px: [38] | (unveraendert) |
| 50B0 | 9 / 9 | 9/11 | 36/49 | 49/49 | 0/2/0 | 4+4/4 | 1/0/0/0 | Marker neben der Kachel (36/49 auf); Uebergang >16 px: [38] | (unveraendert) |
| 50B1 (Elza) | 9 / 9 | 9/11 | 36/49 | 49/49 | 0/2/0 | 4+4/4 | 1/0/0/0 | Marker neben der Kachel (36/49 auf); Uebergang >16 px: [38] | (unveraendert) |
| 50C0 | 10 / 10 | 10/0 | 24/25 | 25/25 | 1/0/0 | 7+7/7 | 3/0/0/0 | ok | (unveraendert) |
| 50C1 (Elza) | 10 / 10 | 10/0 | 24/25 | 25/25 | 1/0/0 | 7+7/7 | 1/0/0/0 | ok | (unveraendert) |
| 50D0 | 10 / 11 | 10/1 | 2/49 | 49/49 | 3/1/0 | 14+14/14 | 3/0/0/0 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (2/49 auf); Uebergang >16 px: [42, 73] | (unveraendert) |
| 50D1 (Elza) | 10 / 11 | 10/1 | 2/49 | 49/49 | 3/1/0 | 14+14/14 | 1/0/0/0 | Blatt weicht vom Original-Init ab (14 Kam.); Marker neben der Kachel (2/49 auf); Uebergang >16 px: [42, 73] | (unveraendert) |
| 50E0 | 10 / 10 | 10/2 | 40/49 | 35/49 | 1/0/0 | 10+10/10 | 1/0/0/0 | ok | (unveraendert) |
| 50E1 (Elza) | 10 / 10 | 10/2 | 40/49 | 35/49 | 1/0/0 | 10+10/10 | 0/1/0/0 (k50E1_c00 DANEBEN 2 px) | ok | (unveraendert) |
| 50F0 | 11 / 11 | 11/2 | 35/49 | 49/49 | 0/1/0 | 10+10/10 | 1/0/0/0 | Marker neben der Kachel (35/49 auf); Uebergang >16 px: [42] | (unveraendert) |
| 50F1 (Elza) | 11 / 11 | 11/2 | 35/49 | 49/49 | 0/1/0 | 10+10/10 | 1/0/0/0 | Marker neben der Kachel (35/49 auf); Uebergang >16 px: [42] | (unveraendert) |
| 5100 | 11 / 11 | 11/1 | 25/38 | 38/38 | 1/1/0 | 10+10/10 | 3/0/0/0 | Marker neben der Kachel (25/38 auf); Uebergang >16 px: [32, 74] | (unveraendert) |
| 5101 (Elza) | 11 / 11 | 11/1 | 25/38 | 38/38 | 1/1/0 | 10+10/10 | 1/0/0/0 | Marker neben der Kachel (25/38 auf); Uebergang >16 px: [32, 74] | (unveraendert) |
| 5110 | 9 / 9 | 9/3 | 31/37 | 37/37 | 0/0/4 | 7+7/7 | 3/0/0/0 | 4 Tuer(en) ohne Marke; Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5111 (Elza) | 9 / 9 | 9/3 | 31/37 | 37/37 | 0/0/4 | 7+7/7 | 1/0/0/0 | 4 Tuer(en) ohne Marke; Uebergang >16 px: [22, 28, 38, 170] | (unveraendert) |
| 5120 | 9 / 9 | 9/5 | 23/23 | 7/23 | 0/0/1 | 7+7/7 | 3/0/0/0 | Marker steht z0 (7/23 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [28] | (unveraendert) |
| 5121 (Elza) | 9 / 9 | 9/5 | 23/23 | 7/23 | 0/0/1 | 7+7/7 | 1/0/0/0 | Marker steht z0 (7/23 Pos.); 1 Tuer(en) ohne Marke; Uebergang >16 px: [28] | (unveraendert) |
| 5130 | 9 / 9 | - | - | 0/0 | 0/0/3 | 0+0/10 | 0/0/3/0 (k5130_c00 kein Marker, k5130_c01 kein Marker, k5130_c02 kein Marker) | KEINE Zone (Karte leer, kein Marker); 3 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5131 (Elza) | 9 / 9 | - | - | 0/0 | 0/0/3 | 0+0/10 | 0/0/1/0 (k5131_c00 kein Marker) | KEINE Zone (Karte leer, kein Marker); 3 Tuer(en) ohne Marke; Uebergang >16 px: [170] | (unveraendert) |
| 5140 | 9 / 9 | 9/14 | 3/27 | 11/27 | 0/2/0 | 14+14/14 | 3/0/0/0 | Marker neben der Kachel (3/27 auf); Marker steht z0 (11/27 Pos.); Uebergang >16 px: [38, 38] | (unveraendert) |
| 5141 (Elza) | 9 / 9 | 9/14 | 3/27 | 11/27 | 0/2/0 | 14+14/14 | 1/0/0/0 | Marker neben der Kachel (3/27 auf); Marker steht z0 (11/27 Pos.); Uebergang >16 px: [38, 38] | (unveraendert) |
| 6000 | 12 / 12 | 12/7 | 34/48 | 7/48 | 2/0/1 | 13+13/13 | 0/0/0/3 (k6000_c00 kein Abzug, k6000_c02 kein Abzug, k6000_c03 kein Abzug) | Marker neben der Kachel (34/48 auf); Marker steht z0 (7/48 Pos.); 1 Tuer(en) ohne Marke | (unveraendert) |
| 6001 (Elza) | 12 / 12 | 12/7 | 34/48 | 7/48 | 2/0/1 | 13+13/13 | 1/0/0/0 | Marker neben der Kachel (34/48 auf); Marker steht z0 (7/48 Pos.); 1 Tuer(en) ohne Marke | (unveraendert) |
| 6010 | 12 / 12 | 12/4 | 20/49 | 18/49 | 1/1/0 | 14+14/14 | 2/1/0/0 (k6010_c04 DANEBEN 6 px) | Marker neben der Kachel (20/49 auf); Marker steht z0 (18/49 Pos.) | (unveraendert) |
| 6011 (Elza) | 12 / 12 | 12/4 | 20/49 | 18/49 | 1/1/0 | 14+14/14 | 1/0/0/0 | Marker neben der Kachel (20/49 auf); Marker steht z0 (18/49 Pos.) | (unveraendert) |
| 6020 | 12 / 12 | 12/5;12/6 | 58/98 | 98/98 | 0/2/0 | 9+9/9 | 1/2/0/0 (k6020_c00 DANEBEN 6 px, k6020_c04 DANEBEN 9 px) | Marker neben der Kachel (58/98 auf) | (unveraendert) |
| 6021 (Elza) | 12 / 12 | 12/5;12/6 | 58/98 | 98/98 | 0/2/0 | 9+9/9 | 0/1/0/0 (k6021_c00 DANEBEN 6 px) | Marker neben der Kachel (58/98 auf) | (unveraendert) |
| 6030 | 12 / 12 | 12/7 | 35/35 | 10/35 | 3/1/0 | 14+14/14 | 0/0/0/3 (k6030_c00 kein Abzug, k6030_c01 kein Abzug, k6030_c02 kein Abzug) | Marker steht z0 (10/35 Pos.) | (unveraendert) |
| 6031 (Elza) | 12 / 12 | 12/7 | 35/35 | 10/35 | 3/1/0 | 14+14/14 | 0/0/0/1 (k6031_c00 kein Abzug) | Marker steht z0 (10/35 Pos.) | (unveraendert) |
| 6040 | 12 / 12 | - | - | 0/0 | 0/0/0 | 0+0/9 | 0/0/0/1 (k6040_c00 kein Abzug) | KEINE Zone (Karte leer, kein Marker) | (unveraendert) |
| 6041 (Elza) | 12 / 12 | - | - | 0/0 | 0/0/0 | 0+0/9 | 0/0/0/1 (k6041_c00 kein Abzug) | KEINE Zone (Karte leer, kein Marker) | (unveraendert) |
