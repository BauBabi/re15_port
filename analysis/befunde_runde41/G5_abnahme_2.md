# Abnahme 2 der Spur G5A (G5-Auftritt im Zug, ROOM5090 Leon / 5091 Elza) - unabhaengig

Zweig `r41/g5auftritt`, Kopf `e77832d22` (Stand Nachbesserung 2). Pruefer ist nicht der Autor. Massstab laut Nutzer: "die letzte Stufe
GENAUSO machen wie bei Resident Evil 2 ... zunaechst die Tentakel vom finalen Birkin G5, und der zieht ihn dann rein". Belege:
`G5_belege/abnahme2/` (Leisten, Messdateien, Werkzeuge `abnahme2/werkzeug/`). Alle Zahlen unten selbst gemessen.

## Urteil: NICHT BESTANDEN

Ablauf, Kamera, Arme, Ton, Spieler-Abgang, Pfosten und Uebergang in den Kampf stimmen mit der echten RE2-Aufnahme ueberein, Leon und Elza
gleich; der Kampf danach ist fair und mit der Pistole gewinnbar. Es bleibt ein Mangel im Kernbild der Szene: **im echten Fenster wird
die Masse des G5 waehrend Schnitt 16/17 ohne ihre "Aufbaeumen"-Verformung (Morph-Gewicht 1) gezeichnet** (M1). Im Motor ist alles
RE2-gleich - Lage, Drehung, Gelenke, Skinning und Morph-Rechnung je auf 1-3 Einheiten gegen RE2-RAM, Gewicht 1 = 2865 wie RE2 -, aber das
gezeichnete Bild folgt dem Umriss der ungemorphten EMD-Grundform. Genau daher kommt die vom Autor in NB2 8.9 offen gelassene Abweichung
("Stacheln frontal statt schraeg, Masse hoeher/groesser"). Daneben ein kleiner, schon vorher vorhandener Unterschied im Atmen der Masse
(M2) und Kleinigkeiten (M4-M6).

## 1. Bau, Laeufe, Zeitachsen

- Bau: `bash re15_port/tools/local_build.sh configure` + `build` im Worktree: LOCAL-BUILD-OK, ninja "no work to do"; `git status` sauber,
  keine Quelle neuer als `re15_pc.exe` (20:30), die exe ist also der Stand `e77832d22`.
- Fensterlaeufe (`werkzeug/run_port.sh`): eigene exe-Kopie `re15_g5abn2_<name>.exe`, gdigrab auf das hwnd der eigenen PID, 20 B/s, kein
  AUTOSHOT, kein SOFTWARE_RENDER, Ton an (RE15_AUDIO_CAP_SYNC, bildgenau). Eingabe bis zur Szene wie `integration_r41_birkin_pistole`
  (Debug-Sprung, Pistole, Zielen + Feuer gehalten), danach je Lauf verschieden (4.). Protokoll beider Figuren: Vorlauf F1558, Armierung
  F1613 nach 55 Vorlaufbildern, Szenenende F2775 Bild 1162, Spieler x 13873, Cut 12. Laeufe: `leon_steh`, `elza_steh` (Szene + Kampf,
  Zielen/Feuer gehalten), `leon_frei` (nach F2548 keine Eingabe, mit RE15_BIRKIN_DBG), `leon_nopri` (RE15_NO_PRI=1, nur Szene).
- Zeitachse Port (`werkzeug/port_zeit.py`): Schnittwechsel aus state.log (cam 16/17/12), Videokanten per Bilddifferenz und Farbstich.
  Leon 29,37 B/s, Kante 16 -> 17 vorhergesagt 82,60 s, gemessen 82,55 s; Elza 29,42 B/s, 83,52 / 83,50 s (Maschine unter Last).
- Zeitachse RE2 echt (`werkzeug/re2_kanten.py`, `re2_zeit.py`, Original-Mitschnitt Lauf 1 640x480 der Abnahme 1): Schnittkanten 61,471 (9)
  / 79,643 (10) / 92,766 (5) / 104,869 (6) s; Standbilder 67,806-68,258 s (Xa_on 0x0B @0x01920, Bild 132), 78,539-79,643 (vor Schnitt 10),
  92,030-92,766 (vor 5), 104,585-104,869 (vor 6). Leon laeuft bei 63,87 s los = Bild 12 (@0x01906 `3f 0c 01 04`), also **Bild 0 = 63,47 s**
  (bestaetigt NB1 63,48; Abnahme 1 hatte den Laufbeginn als Bild 0 genommen). Probe: Bild 132 = 63,47 + 132/29,9 = 67,88 s, das Xa-Standbild
  beginnt 67,81 s; Bild 437 = 68,26 + 304/29,9 = 78,43 s, Standbild ab 78,54 s.

## 2. Bild fuer Bild gegen RE2 echt (Leisten `abnahme2/vergleich_a2_*.jpg`, je Zeile RE2 | Port Leon 5090 | Port Elza 5091)

Szenenbild n: RE2-Zeit nach 1. (`re2_zeit.py`), Port-Zeit a + b*(1613 + n) (`port_bilder.py`, Mitte des Spielbilds).

| Bild | RE2 echt | Port Leon / Elza | Bewertung |
|---|---|---|---|
| -40 (Vorlauf) | leere offene Tuer; im dunklen Tuerloch ziehen Lichtpunkte vorbei (Tunnellichter) | leere Tuer, Tuerloch schwarz | Lichter fehlen (M4) |
| Vorlauf | Figur tritt 0,9 s nach dem Schnitt ins Bild, steht bei ~1,6 s; Vorlauf 61,471 -> 63,47 s = 2,0 s | tritt 0,9 s nach dem Schnitt ein, steht bei ~1,9 s; Vorlauf 55 Bilder = 1,87 s (`vorlauf_re2_oben_port_unten.jpg`) | gleich |
| 0 | Figur in der Mitte, Blick zur Tuer | gleich | gleich |
| 12 | Leon dreht schon nach rechts (RE2-Szenenclip 15 ab Bild 1, @0x018EA) | Figur steht noch, laeuft ab 12 los | PORT-WAHL, gering |
| 30 | Figur draussen, erster Arm flach ueber dem Boden | gleich | gleich |
| 45 / 100 | Arme an Rahmen, Decke, Boden | deckungsgleich | gleich |
| 180 / 240 | Masse unten links hinter dem Pfosten, Stachelreihe **schraeg nach rechts unten** | Masse hinter dem Pfosten, Stachelreihe **senkrecht**, Masse kompakter | **M1** |
| 265 / 271 | kein Sprung | kein Sprung | gleich |
| 360 | Kopf oben links, Stachelreihe als Diagonale bis Bildmitte unten | Kopf gleich, Stachelreihe senkrecht bei x ~140/320, rechts davon Hintergrund | **M1** |
| 450 / 472 (Schnitt 10/17) | Figur rechts vorn von hinten, Speere, Masse klein und flach | Figur und Speere gleich; Masse groesser und hoeher | **M1** |
| 520 / 600 / 700 | Masse kriecht heran, Maul fuellt das Bild | gleich (ab [T8] ist Gewicht 1 auch in RE2 wieder 0) | gleich |
| 840 / 1000 (Schnitt 5 / Cut 12) | grauer RE2-Wagen, G5 mittelgross links, Figur rechts vorn | oranger RE1.5-Wagen (Cut 12, Auge 18526), G5 klein und weit hinten, Feuer am Boden; Figur geht rueckwaerts auf die Kamera zu | PORT-WAHL (RE1.5-Kamera), sichtbar anders, vertretbar |
| 1162 / 1192 | Schnitt 6 vom Ostende, Leon Mitte, G5 dahinter | Cut 12, Figur von hinten bei 13873, G5 dahinter, Balken blenden aus | PORT-WAHL (s. 4) |

- Pfosten-Verdeckung: in 16 liegt der linke Pfosten vor der Masse wie in RE2; mit `RE15_NO_PRI=1` liegt die Masse ueber dem Pfosten
  (`masse_ohne_pri_bild180_271_313_360.jpg`). Die RE2-Masken wirken; sie sind nicht die Ursache von M1 (die Form ist ohne Masken dieselbe).
- Spruenge: groesste Bilddifferenz der Szene ausserhalb der Schnittkanten 6,2 (Leon) / 5,2 (Elza) bei Median 0,96 - kein Sprung.
- Laenge: Port 55 + 1162 Bilder = 40,6 s bei 30 B/s; RE2 Schnitt 9 -> 6 = 43,40 s, davon 2,57 s CD-Standbilder (1.) - Rest 40,83 s.
- Licht/Farbe der Masse (eigene Flaeche x 40..140, y 160..230 im 320x240-Bild): Bild 180 RE2 R/G/B 35/18/11, Port 44/20/12 (Leon = Elza);
  Bild 240 32/17/10 gegen 45/21/12. Der Port ist rund ein Drittel roeter; ein Teil davon ist die andere Form (M1). Gering.
- Bildlage: Hintergrund Port gegen RE2 um 1 Zeile (320x240) versetzt - deckungsgleich.

## 3. Die Masse: Lage, Drehung, Hoehe, Form - gemessen

### 3.1 Motor gegen RE2-RAM

Sonde `werkzeug/sonde_masse.c` (= test_r41_g5auftritt.c plus Messausgaben, gegen libre15_engine.a gelinkt, kein Repo-Code geaendert);
RE2-RAM aus den 26 Savestates der Abnahme 1 (`werkzeug/re2_g5_masse.py`, G5 = `*(0x800cfe1c)`, Teile `*(e+0x198)`, Stride 172).

| Groesse | RE2-RAM | Port (Motor, gleiche Phase/Zeitgeber) | Datei |
|---|---|---|---|
| G5-Lage / Richtung | s05 -9525, s06 -5126, s08 -2825, s09..s12 -2786; +0x76 = 0 | Bild 129 -9525, 165 -5126, 264 -2825, 313..441 -2786; rot 0 | `messung_port_g5_zustand_je_bild.txt`, `messung_re2_ram_g5.txt` |
| Teil-0/1-Winkel | s06 p1 (4068,4070,3855), s09 p0 (0,0,4091) p1 (0,0,3), s12 p1 (0,0,4050) | identisch | dto. |
| Wurzel-y | -4271 / -3191 / -3205 / -3215 | identisch | dto. |
| Teil-2-Weltlage (Bindung 1800/4500 an Teil 0, Ctor @0x80100680-98) | s09 (-959,1274,-23400) | Spiel (RE15_BIRKIN_DBG, F1920) (-952,1257,-23400) | `messung_spiel_g5_tick_und_teillagen.txt` |
| Mesh 0, 2-Bone-Skinning (Welt-Vertexpuffer 0x80132588, 199 Vertices) | s06/s08/s09/s10/s11 | max. 1 Einheit, 166-171 von 199 bitgleich | `messung_mesh0_skinning_port_gegen_re2_ram.txt` |
| Morph-Rechnung Mesh 2 (re15_emd_morph_apply mit den RE2-Gewichten gegen RE2-Vertexpuffer 0x80132be0, 635 Vertices) | 10 Savestates | max. 3 Einheiten | `messung_morph_port_gegen_re2_ram.txt` |
| Gewicht 1 (Aufbaeumen) | 2865 in s08..s13 | Motor: Morph-Bbox y -4223..-1312, z +-2372 = Hebung mit ~2865 | `messung_port_g5_zustand_je_bild.txt` |
| Gewicht 0 (Atmen) | pendelt -4216..10296, Blob 3 Phase 7, Tempo 192 | 774 -> 2820 gleichmaessig (6 je Bild), Blob 3/1; im Spiel dieselben Werte (`gw0` 990 bei Takt 165) | M2 |

Die vom Autor vermutete Ursache "Morph-Gewichte" trifft im Motor nur Gewicht 0, und dessen Wirkung ist klein: Port-w0 gegen die RE2-Spanne
verschiebt hoechstens 189-282 Einheiten, die Bbox bleibt gleich (`messung_w0_wirkung.txt`).

### 3.2 Was das Fenster zeigt (Silhouetten-Probe, `werkzeug/silhouette_n.py`, `silhouette_basis.py`)

Die Masse aus dem RE2-RAM (Mesh 2 gemorpht, Teil-2-Lage/Matrix aus dem RAM) mit der RE2-Kamera (RID Schnitt 9 @0x184 bzw. 10 @0x1A4)
projiziert, Vorderseiten als Umriss; dazu dieselben Polygone in der EMD-Grundform (ungemorpht). Projektion geprueft: der Nordpfosten
(z -21990) landet bei sx 7,3 (Dossier: 8).

| Bild | RE2 echt | Port | Datei |
|---|---|---|---|
| 313 (s09, Gewicht 1 = 2865) | Stachelreihe liegt genau auf dem gemorphten Umriss (Diagonale) | Stachelreihe liegt auf dem rechten Rand der **Grundform**; wo der gemorphte Umriss liegt, ist Hintergrund | `masse_silhouette_bild313.jpg`, `masse_gemorpht_gegen_basis_bild313.jpg` |
| 165 (s06, 2895) | gemorpht | Grundform | `masse_gemorpht_gegen_basis_bild165_479_677.jpg` oben |
| 479 (s13, 2865, Schnitt 17) | gemorpht (klein, flach) | Grundform (groesser, hoeher) | dto. Mitte |
| 677 (s17, Gewicht 1 = 0) | Umriss passt | Umriss passt ebenso | dto. unten (Gegenprobe: Projektion und Lage stimmen auch im Port) |

Folgerung: im Spiel fehlt die Gewicht-1-Verformung im gezeichneten Mesh, genau solange RE2 sie fuehrt ([T2]..[T7], Bild ~40..~650, also
fast die ganze Grossaufnahme und Schnitt 17). Ausgeschlossen: Masken (NO_PRI-Lauf), Teil-2-Lage (Spiel-Protokoll = RE2), KI-Zustand
(Spiel-`gw0` = Sonde), Ladereihenfolge der EM36-Bank (in der Sonde nachgestellt: Bank erst nach `re15_bf_g5_auftritt_start` gueltig ->
Gewicht 1 weiter 2865). Offen ist, warum der Zeichner des Spiels (main.c `re15_g5_morph_verts`, Zeile ~10927) die Hebung nicht zeigt:
entweder steht Gewicht 1 im Spiel auf 0 (Kopplung `s_g5_w1_ok`, enemy_ai_boss_g5.c 772/808/865, wird nirgends protokolliert) oder der
Morphpuffer erreicht das Mesh nicht. Die Sonde/der Riegel `unit_r41_g5auftritt` laeuft ohne Zeichner und kann den Fehler nicht sehen.
Naechster Schritt fuer den Autor: Gewicht 1 und `re15_g5_morph_verts(...) != NULL` je Bild im Fensterlauf protokollieren.

### 3.3 Atmen (M2)

RE2 Blob-Zustand 3 = Vorkampf-Puls @0x801044FC (Sprungtabelle 8 Phasen @0x8010d084, `sltiu v0,v1,0x8` @0x80104518): ph0 @0x80104540
(Zeitgeber 30 + rnd&0x1F, Tempo 6), ph1 @0x80104584-98 w0 += Tempo, Tempo += 6 bis 162 (@0x801045c0-e4), ab w0 >= 8501 (`slti v0,v0,8501`
@0x801045f8) Tempo -= 6, bei < 0 ph2 mit Tempo 8 (@0x80104620-30); ph2 @0x80104634-48 w0 -= Tempo, Tempo += 8 bis 192 (@0x80104670-94),
unter -1048 (@0x801046a8) abbremsen, zurueck nach ph1 (@0x801046b4-d8); weitere Phasen bis 7 (RAM s06..s25 Phase 7, Tempo 192).
Port (enemy_ai_boss_g5.c `case 3`, Kommentar "vereinfachte Puls-Variante"): Tempo fest 6, nur ph1/ph2. Folge: im Intro atmet die Masse
im Port kaum (w0 774 -> 2820 statt Pendel -4216..10296). Wirkung klein (3.1); der Code stammt nicht aus dieser Spur, wird aber erst in
dieser Grossaufnahme sichtbar. Belege: `messung_re2_ram_blob_vorkampfpuls.txt` (RAM-Tabelle + Disassembly).

## 4. Uebergang in den Kampf und Kampf

- Startlage: Port (13873, -23570), Richtung 0x800, Cut 12; RE2 Pos_set @0x0196C `32 00 02 3a 00 00 ee a3` = 14850, Kamera 6. Die
  PORT-WAHL ist belegt: ROOM5090 RVD @0x4CC `01 ff 0c 0d b0 36 ..` = Zone 12 -> 13 fuer x 14000..15400 (bei 14850 spraenge die Kamera
  sofort), @0x4F4 = 13 -> 12 bis x 13873. Folge: 977 Einheiten naeher am G5 als RE2.
- G5 bei Szenenende x 1842, zieht los: 5166 nach 3,7 s, 9172 nach 11,6 s, nie ueber ~11200 (Zug-Kappe `slti a0,a0,12000` @0x80100fd0,
  Biss-Vorstoss bis 12000 @0x80104060-94, selbst disassembliert).
- Reaktionszeit (Leon und Elza identisch, je 45 HP aus dem Formenkampf; `kampf_port_*.txt`):

| Spieler nach Szenenende | erster Treffer | weiter | Ende |
|---|---|---|---|
| steht, Zielen + Feuer gehalten | +18,6 s (Peitsche, 45 -> 38) | Biss +20,0 s (-> 18), +24,1 s (-> 0) | gefressen +25,7 s, G5 HP 600 -> 105 |
| steht, keine Eingabe | +12,0 s (Biss, 45 -> 25) | +16,2 s (-> 5), +20,0 s (-> 0) | gefressen +20,8 s |
| geht 12 s zurueck, dann Feuer (integration_r41_birkin_pistole) | keiner | - | G5 tot, Spieler 45 HP (ctest gruen) |

  Bei +7,7 s schieben die Arme den stehenden Spieler um 1000-2600 Einheiten nach Osten (ohne Schaden). Ein Spieler hat mindestens 12 s bis
  zum ersten Biss; wer hinter x 18000 zurueckgeht, wird nie gebissen. **Fair, mit der Pistole gewinnbar.**
- RE2-Gegenstueck im Mitschnitt nicht messbar: in Lauf 1 steht bei Schnitt 6 die Zug-Selbstzerstoerungsuhr auf 0'00"00, bei ~107,5 s wird
  das Bild weiss, bei 112 s YOU DIED (`re2_lauf1_kampfbeginn_zugtimer.jpg`) - Folge des RAM-Sprungs der Abnahme 1 (nur Flag 4:0x89), kein
  G5-Biss. Die RE2-Seite des Kampfes beruht daher auf dem Code (oben), nicht auf einer Aufnahme.

## 5. Ton

- Xa-Einsatz im Port-Mitschnitt (`werkzeug/xa_ton.py`: R704_XA0B/0C.XAS selbst dekodiert, Kreuzkorrelation ueber den bildgenauen
  Mitschnitt): 0x0B bei Takt 2171 (Korrelation 0,84), 0x0C bei Takt 2477 (0,66), Abstand **306 Takte = 438 - 132**; Leon und Elza gleich
  (`messung_xa_einsatz_leon.txt`). RE2: Xa-Standbild beginnt bei Bild ~132 (1.). Ton stimmt.
- Musik: RE2 Sce_bgm_control @0x018F4/@0x018FA/@0x01900 (Bild 12) senkt MAIN11-Programme; der Port laesst die RE1.5-Musik laufen
  (PORT-WAHL, MAIN11 laeuft im Port nicht). Gering (M5).
- Vorlauf-Ton/Beben: RE2 sub04 @0x0169E..@0x016DC (Se_on `36 02 0c 00 01 ..` @0x016AC, 9x Sce_shake_on @0x016A4..@0x016D8), gestartet
  von sub06 @0x0189A `04 06 18 04` - fehlt im Port. Gering (M4).

## 6. Restpunkte aus NB2 (8.6/8.9) - Bewertung

| Punkt | Befund | Schwere |
|---|---|---|
| Vorlauf-Ton, Beben, Tunnellichter | fehlen; Tunnellichter in RE2 im Tuerloch sichtbar (sub06 @0x01894 `04 07 18 09` -> sub09 @0x01B20 -> sub0B/0C, Abschaltung sub0A @0x01B2E bei Schnitt 10) | gering (M4) |
| Musik MAIN11 | s. 5 | gering (M5) |
| vereinfachter RE2-Rueckzug (sub3) | nicht einzeln geprueft; Kampf mit und ohne Eingabe nachvollziehbar (4.) | offen, ausserhalb der Szene |
| Masse gedreht/hoeher | gemessen: nicht die Gewichte im Motor, sondern die gezeichnete Form im Spiel (3.2) | **hoch (M1)** |
| Atmen der Masse | Vorkampf-Puls vereinfacht (3.3) | gering-mittel (M2) |
| PSX | Szenen-Schnitte, Xa, RE2-Licht nur PC (bg_pc.c, audio_pc.c, main.c) | bekannt (M6) |

## 7. Code und Adressen (Stichproben, selbst gelesen)

- RE2 ROOM7040 sub06 (`re2_scd_dump.py ROOM7040 MAIN 6`): @0x018DE `34 17 01 00`, @0x018EA `3f 00 0f 80`, @0x01906 `3f 0c 01 04`,
  @0x01920 `59 00 0b 00`, @0x0192A `32 00 64 19 00 00 ee a3`, @0x01940 `3f 0b 01 54`, @0x01946 `59 00 0c 00`, @0x01956 `32 00 4c 2c ..`,
  @0x0196C `32 00 02 3a 00 00 ee a3`, @0x0197C `42`, @0x01981 `3c 01`: wie im Code.
- CDEMD0_EM36_ai0.BIN (`re2_disasm.py`): @0x80103e00-3c (`lw v0,496(s0)`, `sltiu v0,v0,0x1770`, +0x229-Sperre, `jal 0x80015614`, a3 128,
  `bne`), @0x80100fd0 `slti a0,a0,12000`, @0x80104060-94 Biss-Vorstoss bis 12000, @0x8010044c `sh zero,118(s0)`, @0x80101500-2c Phase 7
  an alle vier Arme, @0x801011d0/@0x801011d8 -9000/-23400: alle wie zitiert.
- Ctor-Bindungen (@0x80100670-740, `messung_re2_ctor_teilbindungen.txt`): Teil 2 an Teil 0 (1800,4500,0), Teil 3 an Teil 0 (800,500,0),
  Teile 4/5/6 an Teil 2 ((1698,-1398,0), (1432,-2691,1023), (1430,-2284,-2078)); RAM s09 bestaetigt alle sieben Weltlagen. Der Port bindet
  2/3 (main.c `nbi <= 3`) und kappt die Bank auf 4 Meshes (main.c ~1209); Meshes 4..6 sind 10x10-Platzhalter (1 Quad) - ohne Bildwirkung.
- LIT ROOM7040 @0x668 (40 Byte) = `k_re2_licht9` in g5auftritt_r41.c. RVD ROOM5090 @0x4CC/@0x4F4 wie im Kommentar zu G5A_X_ENDE.
- Konstanten in g5auftritt_r41.c tragen @0x-Adresse oder PORT-WAHL; keine nackte Zahl gefunden. Nicht byte-true und nicht als PORT-WAHL
  gefuehrt: Blob-Zustand 3 in enemy_ai_boss_g5.c (M2, aelter als diese Spur).

## 8. Tests (re15_port/build, ctest)

| Lauf | Ergebnis |
|---|---|
| Alle Nicht-Integrationstests ohne die 6 Release-Skript-Tests (-j 6) | **1068/1068 gruen**, 877 s |
| unit_r38_releaseskript_{trocken,ablauf,fehler_bau,fehler_umfeld,cmd}, unit_r39_titelversion_ablauf | alle 6 Timeout 1200 s (Test-TIMEOUT schlaegt `--timeout 90`). **Umgebung**: `df -Pk C:/workspace` haengt auf der Maschine auch allein (mein `timeout 8 df` kam nach 120 s nicht zurueck); 60+ haengende df.exe seit 17:38. Meine 6 df-Waisen (Start 21:49:08-10, Eltern beendet) danach per PID beendet. |
| integration_r41_birkin_pistole | gruen, 426 s |
| integration_r41_g5_tod | gruen, 280 s |
| integration_r41_g5_westen | gruen, 440 s |
| integration_r35_zgirl_5090 | gruen, 148 s |
| integration_pri_masken | gruen (Datentest, 0,2 s) |
| integration_r36_pri_schatten | gruen, 29 s |

Kein Test prueft M1: `unit_r41_g5auftritt` misst den Motor ohne Zeichner (dort ist die Hebung da), die Integrationslaeufe pruefen den Kampf.

## 9. Maengel

| Nr | Was | Beleg | Schwere |
|---|---|---|---|
| M1 | Masse in Schnitt 16/17 im echten Fenster in Grundform statt mit Gewicht-1-Hebung gezeichnet: Stachelreihe senkrecht statt schraeg, Masse in 17 groesser/hoeher. Motor ist RE2-gleich (3.1); Ursache im Spielpfad nicht isoliert (3.2) | `masse_silhouette_bild313.jpg`, `masse_gemorpht_gegen_basis_bild313.jpg`, `masse_gemorpht_gegen_basis_bild165_479_677.jpg`, `vergleich_a2_2_*.jpg`, `vergleich_a2_3_*.jpg` | hoch (Kernbild der Szene, ~600 Bilder) |
| M2 | Vorkampf-Puls (Blob 3) vereinfacht: Masse atmet im Intro kaum (w0 774..2820 statt -4216..10296); RE2 @0x801044FC 8 Phasen, Tempo 162/192 | `messung_re2_ram_blob_vorkampfpuls.txt`, `messung_w0_wirkung.txt` | gering-mittel (bis 282 Einheiten) |
| M3 | Startlage 13873 statt 14850 (@0x0196C), Cut 12 statt Kamera 6 | RVD @0x4CC/@0x4F4 | PORT-WAHL, belegt; Kampf trotzdem fair (4.) - kein Mangel im engeren Sinn |
| M4 | Vorlauf: Tunnellichter (sub09/0B/0C), Beben + Se_on (sub04) fehlen | `vorlauf_re2_oben_port_unten.jpg`, 5. | gering |
| M5 | Musik: RE2 senkt MAIN11 bei Bild 12, Port behaelt RE1.5-Musik | 5. | gering (PORT-WAHL) |
| M6 | PSX-Bau ohne Szenen-Schnitte/Xa/RE2-Licht | NB2 8.9 | bekannt |

Nicht mehr Mangel (gegen Abnahme 1 / NB1 / NB2 nachgemessen): Figur laeuft aus der Grossaufnahme und geht in 17/12 rueckwaerts hinaus;
Arme deckungsgleich mit RE2 (Bild 30/45/100); G5-Lage = RE2-RAM; Pfosten verdeckt; kein Sprung bei 265/271; Xa 0x0B/0x0C auf dem Bild;
Speere in 17; Uebergang in den Kampf ohne Sprung; Frau/Feuer in 16/17 unsichtbar; Kampf gewinnbar und fair.

## 10. Belege (`G5_belege/abnahme2/`)

- Vergleichsleisten RE2 echt | Leon | Elza: `vergleich_a2_1_bild-40_bis_100.jpg`, `vergleich_a2_2_bild180_bis_360.jpg`,
  `vergleich_a2_3_schnitt17_bild450_bis_700.jpg`, `vergleich_a2_4_schnitt5_kampf_bild840_bis_1252.jpg`, `vorlauf_re2_oben_port_unten.jpg`.
- Masse: `masse_silhouette_bild313.jpg`, `masse_gemorpht_gegen_basis_bild313.jpg`, `masse_gemorpht_gegen_basis_bild165_479_677.jpg`,
  `masse_ohne_pri_bild180_271_313_360.jpg`; Messdateien `messung_*.txt`.
- Kampf: `kampf_port_leon_steh.txt`, `kampf_port_elza_steh.txt`, `kampf_port_leon_ohne_eingabe.txt`, `re2_lauf1_kampfbeginn_zugtimer.jpg`.
- Zeitachsen `zeitachse_port_*.json`; Werkzeuge `werkzeug/` (Python: Pfade auf den Scratch-Ordner dieser Sitzung; `sonde_masse.c` ist
  test_r41_g5auftritt.c plus Messausgaben, Bau: gcc -std=c11 mit `-I re15_port/include` gegen `build/engine/libre15_engine.a` und
  `build/tests/libre15_test_support.a`; `morph_vgl.c` vergleicht den Port-Morph mit RE2-Vertexpuffern).
