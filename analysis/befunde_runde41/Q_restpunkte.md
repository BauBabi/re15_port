# Spur Q "restpunkte" - Dossier (Runde 41, Basis master 91cd9ec82)

Baum `.claude/worktrees/r41_restpunkte`, Zweig r41/restpunkte. Fortlaufend geschrieben, nach jedem Schritt committet.
Vorgaenger hinterliess keine Commits (`git log master..HEAD` leer beim Start, Baum sauber).

## Punkte
1. Masken: Restloch ROOM6010/6011 Cut 5 Teil 8 schliessen (Abnahme M 5 Mangel 1).
2. Teile-Bericht M_bilder/NB5/teile_nb5.txt auf den ausgelieferten Stand; 8 fehlende Stellen in die Freistellungs-Liste;
   4040 C12 HINTER-Beleg berichtigen (Mangel 2/3).
3. Riegel Elza-Kette 2031 -> 20B1 -> 2051; Ursache der stillen Enden von integration_r41_ventilkette_kette (UEBERGABE 3b).
4. Toter Alligator bleibt tot (RE2 ROOM40A0 @0x0054, sub02 @0x00AE); Leon und Elza; Raum verlassen + wieder betreten,
   Spielstand laden; Water Key bleibt erreichbar oder aufgehoben.
5. Spinnen-Filter in gatorszene_r41.c unter RE15_GATOR_ORIGINAL pruefen/abschalten; Riegel Leon und Elza.

## Stand
- [x] Bau im Spurbaum
- [x] Punkt 1
- [x] Punkt 2
- [x] Punkt 3
- [x] Punkt 4
- [x] Punkt 5

## Punkt 4 - Der tote Alligator bleibt tot

### Messung vorher (exe Basis 91cd9ec82, Lauf Q41/runs/v4L, natuerlicher R1-Tod, Tuer Slot 0, Leiter zurueck)
Rezept des Riegels `integration_r41_alligator2_schluessel` + `RE15_FIRE_AOT=0@720#2090`: `RE2 R1 ende (Leiche) pos=(-5828,-20967)`, Folge L F217,
`Kanister verbraucht (R1-Tod) -> Bit 116 = 0` F220, Tuer, 2080, Leiter. Im naechsten 2090-Abschnitt: `[enemy] ROOM2090: Alligator (0x23) in Wasserzone gesetzt, Slot 35`,
state.log letzte Zeile `t=23 st=1 ss1=0 ... @(-7200,-9200,r1089)] hp=25300`. Volle Energie, kein zweiter Kanister: nicht mehr toetbar. Reproduziert H3 der Abnahme P 1.

### RE-Belege (selbst gelesen)
- RE2 ROOM40A0.RDT SCD-init sub00 (Datei 0x2502, Offsets relativ zum SCD): @0x0050 `06 00 64 00` Ifel_ck, @0x0054 `21 04 33 00` Ck Bank 4 Bit 0x33 = 0, @0x0058 `21 05 1f 01`,
  @0x00A0 `44 00 00 23 00 00 01 16 00 00 4a 16 b8 0b 3c eb 00 04 00 00 00 00` Sce_em_set Typ 0x23 liegt im Block: Bit gesetzt = kein Alligator.
- RE2 SCD-main sub02 @0x00A4: `46 05 ...` Aot_reset, @0x00AE `22 04 33 01` Set Bit, dann Intro. RE2 setzt das Sperrbit also beim KAMPFBEGINN, nicht beim Tod.
- RE2 hat daneben ein Tot-Bit: EM23_OVL_0000.BIN R1 P3 `801021e0 addiu a0,s2,17240` (s2 = 0x800d051c -> a0 = 0x800d4874 = Bank 5), `80102654 lhu v0,270(s1)`, `80102658 addiu a1,zero,30`,
  `8010265c ori v0,v0,0x4000`, `80102660 jal 0x8007730c` (Bit setzen: `8007730c srl a2,a1,5` .. `80077328 or v1,v1,v0`), `80102664 sh v0,270(s1)` = (5,0x1E). Leser SCD @0x07D0 `21 05 1e 00`, @0x0D56/@0x0DAC `21 05 1e 01`.
- RE2 hat ausserdem ein Kill-Flag im Satz: Sce_em_set FUN_8005714c `800571c0 lbu a1,9(v0)`, `800571c4 addiu v0,zero,255`, `800571c8 beq a1,v0`, `800571d0 jal 0x80077360`, `800571d8 bne v0,zero,0x800576dc`: pc[9] = 0x00, also Zone 0x800d489c Bit 0. EM23 setzt dieses Bit nicht (0x8007730c nur @0x80101848 Bank 5 Bit 29 und @0x80102660 Bank 5 Bit 30): der Mechanismus des Alligators ist das SCD-Bit, nicht das Kill-Flag.
- Warum RE2 das Bit beim Kampfbeginn setzen darf: die Tueren sind im Kampf Ereignisse (sub12 @0x0828/@0x0832 Aot_reset 0/1 -> sub29/sub30, Text), die Schalttafel AOT 4 (@0x00FA -> sub26 @0x0CC8, Abfrage `21 0b 1f 00` @0x0CDE) gibt sie frei (sub27/sub28): wer flieht, dem fehlt in RE2 danach nur der Gegner.
- Im Port traegt der Alligator den Water Key (Nutzer-Auftrag; Tor 2090/20A0): ein Sperrbit beim Kampfbeginn nimmt nach einer Flucht ueber die Leiter den Schluessel weg, also Softlock. PORT-WAHL: die Form bleibt RE2 (Ck-Tor vor dem Setzen, Bit gespeichert), der Zeitpunkt ist RE2s Tot-Bit (R1 P3 @0x80102660).

### Umsetzung
- Kein neues Port-Bit: (9,111) `RE15_GSZ_BIT_LIEGT` ist genau "Alligator tot" (include/re15_gatorszene_r41.h:62; einziger Setzer gatorszene_r41.c key_tod_setzen, bleibt nach dem Aufheben gesetzt, Taken-Bit ist (9,112)). Bit 126 bleibt frei.
- engine/src/gatorszene_r41.c `re15_gatorszene_r41_gator_kommt`, include/re15_gatorszene_r41.h, Haken in platform/pc/main.c (Alligator-Pool, eine Bedingung).

### Messung nachher (Laeufe Q41/runs/n2L, n3L)
n2L (Leon): R1 F358-360, Folge O F450, Tuer F500, `[save] saved (room 2080)`, Leiter, 2090-Abschnitt 510 Bilder ohne eine einzige t=23-Zeile, genau eine Zeile `Alligator (0x23) in Wasserzone gesetzt`, Schluessel unveraendert an (-5700,-1725,-7500).
n3L: Karte aus n2L, `[save] CONTINUE: resumed in room 2080`, Leiter: 200 Bilder ohne t=23, `Schluessel-Ersatzlage (-170,-1702,-23629)`.

## Punkt 5 - Spinnen-Filter unter RE15_GATOR_ORIGINAL

### Messung vorher (Hauptbaum-exe, Kopie, Lauf Q41/runs/o0L)
`RE15_GATOR_ORIGINAL=1`: `[gatorszene] ROOM2090: Spinne 0x25 bei (-100,-8296) verworfen`, ebenso (-2300,-23896). Es laeuft nur der Alligator, der Raum ist nicht der ausgelieferte.

### Beleg
ROOM2090 main00 `@0x00AB0 44 00 25 ...` und `@0x00AC4 44 01 25 ...` (F_gatorszene.md Z. 77-78): zwei Spinnen sind Inhalt des ausgelieferten Raums. Der Filter ist Nutzer-Design ("Entferne die Spinnen aus dem ROOM 2090", AUFTRAG.md) und gehoert zu dem Paket, das der Schalter abstellt (Commit aa463a744: Ankunft, Montage, Water-Key-Tor bleiben aus).

### Umsetzung, Messung nachher (Lauf Q41/runs/o1L)
gatorszene_r41.c `re15_gatorszene_r41_spawn_weg`: `if (!s_spinnen_weg || re15_gator_original()) return 0;`. Nachher: keine Zeile `verworfen`, F500 `[1 t=25 ... @(-1027,-11432..)]`, `[2 t=25 ... @(-6995,-25679..)]`, `EM23 loaded: 15 meshes, 15 bones, 43 clips`, keine einzige `[gatorszene]`-Zeile.

## Punkt 1 - Restloch ROOM6010/6011 Cut 5 Teil 8

### Messung vorher
Abnahme M 5 (Lauf o6010_05, `M41abn5/bilder/o6010_05_voll.png`): (-19912,-19720) Band 0, 96 Figurpunkte auf Teil 8, davon 82 sichtbar. `unit_r41_masken36_fern`: ROOM6010/6011 C5 je 1508 Punkte "zu fern", R113..R124.
Eigene Vermessung (`Q41/tools/t8.py`, Kamera des Spiels `ansicht_spiel`, Koerper Modell-Zeile Zelle @0x0092C `31 0b 2d 19 31 b2 dc a1 01 ff 00 13` Typ 1 Band 1 Hoehe -2311):
Teil 8 = 426 Punkte in R113..R127, Bild X 162..184, Y 202..239, je Rechteck eine Tiefe (126..112), aber die Stufen der Punkte eines Rechtecks spannen bis zu 11 Stufen (R113: bucket 116..126, Tiefe 126).

### Weg, der verworfen wurde (gemessen)
Einstieg des Auftrags: Kapazitaet 128 -> 255, Ausschluss-Tabelle bits[8], Spalten teilen. Gebaut und gemessen (Lauf `aus_s1.inc`, `fern_s1.log`): Spalten exakt je Stufe = 155 Stuecke, 113 + 155 = 268 > 255 (Byte-Grenze @0x8003928c `lbu v0,7(a0)`), geht also nicht. Spanne 1 = 82 Stuecke, Sektion 195 Rechtecke: 6010 C5 "zu fern" 675. Beides zurueckgenommen: es gibt einen genaueren Weg ohne Aenderung an der Engine.

### Umsetzung
Die Stufen des Teils sind nur 15 (112..126), genau so viele wie es Rechtecke hat, und sie sind die alten Rechteck-Tiefen. tools/maske/stufen_r41.py legt je Stufe EIN Rechteck an: Fenster im Vordergrundatlas `ROOM6010_PRI05.TIM` mit genau den Punkten dieser Stufe, Rest Index 0. Die Atlas-Flaeche reicht (3 808 von 65 536 Texeln belegt, neue Fenster 4 209 Flaeche).
Rechteckzahl 128, Index-Lage R113..R127, Tiefe je Index, Palette, alle uebrigen Rechtecke und Fenster bleiben: `gen_r41_masken36_zellen.inc`, `gen_r41_masken36_platziert.inc`, Ausschluss-Tabelle (Neurechnung `aus_st.inc` bytegleich), `re15_pri.h` unveraendert. Dateien: `shared_assets/PSX/MASKS/ROOM6010.MSK`, `ROOM6011.MSK`, `ROOM6010_PRI05.TIM`, `ROOM6011_PRI05.TIM` (Leon = Elza bytegleich).
Konstanten: bucket = (1023*vz)>>16 (@0x8002565c `sra v1,v1,4`, ZSF3 341 @0x80066c70), Masken-OT = depth (@0x80039650 `lh a0,2(s3)`), Vergleich depth < bucket(Figur) wie bisher.

### Messung nachher
`test_r37_masken36 teile`: 101 280 Punkte, Eintritt hinter der Tiefe 0. `test_r37_masken36 fern`: ROOM6010/6011 C5 je 422 statt 1508, gesamt 1188 (Stage 3: 2, 4: 72, 5: 270, 6: 844). Die 422 waren OT-Gleichstand mit der Zellflaeche: Plaetze nur noch x -20112..-19712 (`fern_plaetze.log`, vorher -20112..-19512). Stand der Nachbesserung 1: je 0, gesamt 344.
An der exe (Leon, je 4 Laeufe mit/ohne/leer/leer0, A0 = (20000,20000), Bild 120, `Q41/runs/`, `tools/sz_ausw.py`):
| Stelle | Figur auf Teil 8 | vorher verdeckt / sichtbar | nachher verdeckt / sichtbar |
|---|---|---|---|
| (-19612,-19720) | 195 | 192 / 3 | 195 / 0 |
| (-19712,-19820) | 155 | 114 / 41 | 146 / 9 |
| (-19912,-19720) Stelle der Abnahme | 96 | 14 / 82 | 33 / 63 |
| (-20112,-19620) vor der Wange | 29 | - | 2 / 27 |
Bilder `Q_belege/m1_p2_vorher.png`, `m1_p2_nachher.png` (je A0, B, A, Lupe mit Rest in Rot).
**Berichtigt (Nachbesserung 1):** Hier stand, die 63 Punkte der Abnahme-Stelle seien kein Loch und sie zu verdecken verstosse gegen Regel 6. Das galt nur gegen das Modell: der Koerper war die Kollisionszelle @0x0092C, nicht die Wange. Die Wange liegt 672 Einheiten naeher an der Kamera, die Figur steht hinter ihr. Die Verschiebe-Probe (k = 1: 59 sichtbar, 2: 48, 4: 23, 8: 0) mass gegen dieselbe Zelle, die sie belegen sollte. Stand jetzt: Abschnitt "Nachbesserung 1".

## Punkt 2 - Teile-Bericht, Freistellungs-Liste, 4040 C12

### Messung vorher
Abnahme M 5 Mangel 2/3: `M_bilder/NB5/teile_nb5.txt` aus pl_a2 (266/278), MASKS aus pl_a3 (267/277), 21 Teile anders, 34 von 61 Sektionskoepfen falsch; 8 Sektionen fehlen in der Liste; Szenen t4040_12/t4040_12e messen das 3D-Tor (S_bau 440, S_fig 0).

### Umsetzung
- tools/maske/teile_bericht_r41.py schreibt den Bericht aus platzieren_r41.json. Eichung: aus pl_a2 gerechnet 625 von 625 Datenzeilen gleich der alten Datei (0 Abweichungen), danach aus pl_a3 geschrieben. Zusatz am Teil 6010 C5 T8: Tiefe je OT-Stufe.
- Nachher: `Q41/tools/teile_vergleich.py` (Skript der Abnahme): `pl_a3 ... OHNE 277, MIT 267 | teile_nb5.txt ... OHNE 277, MIT 267`, `Teile mit anderem Ergebnis: 0`. Summen 267 Teile / 103 112 Punkte, 277 Teile / 144 225 Punkte = Dossier M Z. 1161.
- Liste: Abgleich aller 62 Zeilen gegen pl_a3 ohne Abweichung, die 8 fehlenden eingefuegt (3040 C10 218, 3050 C1 106, 3050 C6 209, 5050 C1 275, 5050 C2 174, 6000 C7 172, 6000 C9 108, 6030 C12 139), keine Mindestgroesse mehr; je Stelle Restbild `M_bilder/NB5_rest/NB5_rest_ROOMxxxx_Cnn.png` (70 Bilder).
- 4040 C12: das Teil ist der schmale Streifen am Torrahmen (Rechtecke 105..125, Bild X 75..106, 723 Punkte). Reale Stellensuche (`Q41/tools/stellen_q.py` ueber alle 1060 erreichbaren Plaetze von `test_r37_masken36 plaetze 4040 12`, berichtigt nach Abnahme 0 H6, Ausgabe `PLAETZE 1060`): HINTER-Kandidaten nur (-4000,4678), (-3900,4578), (-3800,4478). An der exe je 4 Laeufe: Figur X 45..70, Figur auf dem Teil 0/0/0. Jenseits des Tors gibt es keinen Stehplatz. Damit gibt es dort KEINE HINTER-Stelle: Szenen t4040_12/t4040_12e entfernt, Dossier M Tabelle und OFFEN 21 berichtigt.
- Laeufer test_r41_masken36_stage2.cmake: Art M mit A0 zaehlt nur die Spielfigur (S = |B - D|, dritter Lauf). Alle 38 verbliebenen M-Szenen der Gruppen a1..d2 tragen A0 aus der Liste der Abnahme (`M41abn5/aussen_alle.txt`), 6010 C5 hat keinen erreichbaren Platz draussen: A0=20000,20000 wie Lauf o6010_05. So faellt ein 3D-Objekt im Fenster ab jetzt auf ("Leon im Figurfenster nicht zu sehen").
- Szenenzahl: 106 laut Dateien (Dossier/Commit nannten 108), jetzt 104.

## Punkt 3 - Elza-Kette und die stillen Enden des Ketten-Riegels

### Messung vorher
Serie mit dem Rezept des Riegels (Software-Renderer, RE15_NOAUDIO, `Q41/tools/kette_gdb.sh`, Ergebnis `Q41/kette_serie.txt`), parallel zu anderen Agenten:
| Serie | Laeufe | rv=139 (Absturz) | Bild |
|---|---|---|---|
| ohne gdb, Leon (kn1, kb1, kb2, ku1, ku2) | 19 | 6 | immer letzte Zeile `[walk] F750 cut=10`, also wie die Laeufe 2-5 aus INT_belege/riegel_kette_laeufe.txt |
| unter gdb, Leon 5, Elza 5 | 10 | 0 | gdb legt die Windows-Debug-Halde an, die Lage der Bloecke aendert sich |
rv=139 = Zugriffsfehler. Der Riegel hat `_rv` nie ausgegeben, und ein Absturz der exe hinterlaesst auf dieser Maschine keinen WER-Eintrag (auch Abnahme P H1); daher kam "still" und "kein Absturz" in INT_belege.

### Ursache (gemessen)
Ausnahme-Haken in main.c, viermal gleich: `[flow] AUSNAHME 0xC0000005 ... Adresse +0x21f406 Ziel 00000146632a22d8`, Stapel `SDL_BlitTriangle_Slow+1394 <- SDL_SW_BlitTriangle+2757 <- SW_RunCommandQueue+3758 <- FlushRenderCommands <- SDL_RenderPresent <- re15_render_end_frame <- main`.
Der CPU-Rasterer von SDL 2.28.5 liest ausserhalb der Textur. Ursache dort: `SDL_triangle.c:618 s2_x_area.x = s2->x * area;` rechnet in `int` (SDL_Point), `area` kommt aus `cross_product` von Koordinaten `<< FP_BITS`; ab area > 2^31/255 = 8 421 504 laeuft das ueber, und `TRIANGLE_GET_TEXTCOORD` (Z. 171/172) liefert Muell ohne Klemme.
Messhaken nur in Messkopien (Lauf ku2, nicht im Code): in 20B0 gibt es solche Dreiecke nur in F755..F767, Slot 42 (ESP-Effekt 0x0f, Textur 256x192), Flaeche bis 1 130 976 900, Koordinate bis 10 250: Effekt-Quader des Wassers kurz vor der Kamera. Das ist genau das Fenster F751..F779 der roten Laeufe. Ob der falsche Zugriff eine unbelegte Seite trifft, haengt an der Lage der Halde: daher zufaellig, daher dieselbe Stelle, daher bis zur letzten Zeile gleiche Logs. Fremdlast war nicht die Ursache.
Der Spieler sieht diesen Absturz nicht: der beschleunigte Renderer hat die Rechnung nicht. Er zeichnet diese Quader aber, die Hardware nicht: psx-spx graphicsprocessingunitgpu.md "Size Restriction": "The maximum distance between two vertices is 1023 horizontally, and 511 vertically. Polygons and lines that are exceeding that dimensions are NOT rendered."

### Umsetzung
- include/re15_gpu_grenze_r41.h, platform/pc/src/render_pc.c beide Dreiecks-Eingaenge: zuerst GTE-Saettigung der Ecken auf -400h..+3FFh (psx-spx geometrytransformationenginegte.md Z. 418/419; der Modell-Zeichner prueft kein FLAG, @0x80025640 AVSZ3, @0x80025658 beq nur auf otz>>6), dann GPU-Grenze 1023 / 511 (psx-spx, oben); die ersten 16 Verwerfungen je Prozess stehen als `[gpu-grenze]` im Log. Wichtig: ohne die Saettigung fielen die grossen Dreiecke der Spielfigur nah vor der Kamera (2030 F1088.., Koordinaten bis 12 288) ganz weg, die Hardware zeichnet sie gesaettigt. Der Schutz gilt NUR im CPU-Rasterer (`s_cpu_rasterer`, aus SDL_GetRendererInfo: RE15_SOFTWARE_RENDER oder Rueckfall ohne GPU): der beschleunigte Renderer des Spielers hat den Fehler nicht, und Port-eigene Flaechen (Wasserflaechen als Prop, ventilkette_r41.c) sind nicht gegen die Hardware-Grenze gebaut. Das Bild des Spielers aendert sich dadurch nicht.
- render_pc.c SDL_QUIT schreibt jetzt `[flow] SDL_QUIT (Fenster geschlossen) -> exit`; platform/pc/src/endgrund_r41_pc.c (Haken in main.c): Ausnahme mit Adresse und Stapel im debug.log, nur Log, EXCEPTION_CONTINUE_SEARCH. Kein Ende der exe ist mehr still.
- test_r41_ventilkette_kette.cmake: -DFIGUR=elza, Fehlermeldung mit exit-Code und letzter Log-Zeile, SDL_ASSERT=always_ignore (Lauf 1 der INT-Liste endete an der Assertion WIN_AddDisplay). Riegel integration_r41_ventilkette_kette_elza in probes/r41_restpunkte.cmake.

### Messung nachher
Serien mit dem Rezept des Riegels ohne gdb, parallel zu anderen Agenten und zu ctest (`Q41/kette_serie.txt`):
| Stand | Laeufe | rv=139 |
|---|---|---|
| vorher (kn1, kb1, kb2, ku1, ku2) Leon | 19 | 6 |
| Grenze ohne Saettigung (kc1..kc3 Leon, kc4 Elza) | 20 | 0 |
| Endstand (kd1, kd2 Leon, kd3 Elza) | 12 | 0 |
Riegel auf dem Endstand: integration_r41_ventilkette_kette Passed 236,64 s, integration_r41_ventilkette_kette_elza Passed 252,14 s (`2031 richtige Folge zu Fuss -> Ost-Tuer zu Fuss -> 20B1 Szene sub02/sub05 -> (3,235)=1 -> frei -> Kanal -> Leiter -> ROOM2051 in einem Lauf`).
Der beschleunigte Renderer bleibt unberuehrt: Lauf vg1 (direct3d) 0 Zeilen `[gpu-grenze]`.

## Punkt 4/5 - Riegel am Endstand
integration_r41_restpunkte_gator Passed 185,44 s, _gator_elza Passed 215,71 s: R1-Tod, Tuer, Spielstand, Leiter: 510 Bilder ohne t=23, genau 1 x `Alligator (0x23) in Wasserzone gesetzt`, Schluessel da, keine Kampfmusik; CONTINUE in 2080/2081, Leiter: kein Alligator, Ersatzlage; zu Fuss aufgehoben (Elza `[inv-r39] F212 R2091 ... 34:1`); Bits 110/111/112: kein Alligator, keine Zone.
integration_r41_restpunkte_original Passed 19,32 s, _original_elza Passed 36,04 s: 2 Spinnen, RE1.5-Alligator (15/15/43), keine Szene.

## Punkt 1 - Riegel am Endstand
unit_r41_restpunkte_teil8(_elza): 426 Punkte, Stufe != Rechteck-Tiefe 0. Gegenprobe mit dem Stand NB5: 381 von 426 daneben, FAIL.
integration_r41_restpunkte_masken Passed 230,05 s, "6 Szenen OK". Gegenprobe mit dem Stand NB5: q8h1 1 von 118 sichtbar, q8h2 12 von 39 sichtbar, FAIL. Die beiden Elza-Szenen q8h1e/q8h2e bestehen auch im Stand NB5 (Elza steht dort anders zur Wange): fuer Elza traegt unit_r41_restpunkte_teil8_elza und `fern`.
unit_r41_masken36_fern auf die neue Schranke gezogen (1188, Stage 6: 844). Alle Masken-Gruppen a1..d2 mit Art M gegen Lauf D gruen.

## Tests (gezielt, keine volle Suite)
- `ctest -R "^(unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_bss_room_source|prop_bss_chunk|unit_r38_itemorte|unit_r39_optik|unit_stage6_hintergrund|unit_r41_restpunkte|unit_r36_kartenbits|unit_r41_gatorszene|unit_r41_alligator2|unit_r41_kanister2080|unit_r41_re2gator)" -j 2`: `100% tests passed, 0 tests failed out of 102`
- `ctest -R "^(integration_r41_masken36_teile_(a1|a2|b1|b2|c1|c2|c3|d1|d2)|integration_r41_masken36_laufpruefung|integration_r41_masken36_stage2|integration_r41_masken36_fern)$" -j 3`: `100% tests passed, 0 tests failed out of 12`
- Endstand: `ctest -R "^integration_r41_(gatorszene|alligator2|kanister2080|re2gator|restpunkte|ventilkette_kette|wasser20b0)|^integration_r41_masken36_teile_d[12]|^integration_r38_wasser" -j 3 --timeout 1500`: `100% tests passed, 0 tests failed out of 40`, 2893,99 s
- Endstand: `ctest -R "^(unit_r41_restpunkte|unit_r41_masken36_fern|unit_r41_masken36_teile|unit_r41_masken36_platziert|unit_r41_masken36_zellen3d|unit_r37_masken36_ausschluss)" -j 2`: `100% tests passed, 0 tests failed out of 10`
Die Masken-Gruppen a1..c3 liefen auf dem Stand vor der letzten Aenderung am Renderer (nur CPU-Rasterer-Schutz und Log); d1, d2 und die Teil-8-Szenen auf dem Endstand. Die 102 Unit-Tests liefen vor dieser Aenderung, sie beruehrt nur platform/pc.

## OFFEN
1. **Nebenbefund fuer den Spieler, nicht behoben (Bild `Q_belege/n3_westgang_F753_F768.png`):** ROOM20B0 nach der Szene, Leon im Westgang, beschleunigter Renderer (Lauf vg1, direct3d): F755..F768 fuellen grobe grau-gruene Bloecke 50-90 % des Bildes (Helligkeit 41 -> 109). Es sind dieselben Effekt-Quader ESP 0x0f. Das Original baut sie als POLY_FT4 (`sb v0,7(t2)` @0x8005369c, Code 0x2C/0x2E), Mitte aus der GTE (`0x4a180001` RTPS @0x80053534), einzige Sperre SZ < 4 (`sra v0,a0,2` @0x80053578, `beq` @0x8005357c); danach greift die GPU-Grenze, Quader ueber 1023 x 511 zeichnet die Hardware nicht. Der Port zeichnet sie. Vorschlag: `re15_gpu_dreieck_verwerfen` auch fuer die ESP-Quader im beschleunigten Renderer (zwei Zeilen an main.c:589/591); ob die kleineren Quader danach dem Original entsprechen, braucht einen DuckStation-Vergleich desselben Wegs. Liegt ausserhalb der Spur Q, das Bild des Spielers ist hier unveraendert.
2. Geschlossen durch Nachbesserung 1: Abnahme-Stelle (-19912,-19720) 0 von 96 sichtbar.
3. Geschlossen durch Nachbesserung 1: "zu fern" in 6010/6011 C5 0 + 0. Die fruehere Aussage "nicht weiter zu trennen" war falsch, der Fehler lag in der Ebene.
4. Der Port setzt das Sperrbit beim Tod, RE2 beim Kampfbeginn: fliehen laesst den Alligator im Port leben, weil er den Water Key traegt (PORT-WAHL, Punkt 4).
5. Der Spielstand in 2080 ist ein Testhaken: STAGE2 hat nur den Speicherpunkt 2010/2011 (re15_savepoint.c). Geprueft ist Laden + Tuerweg nach 2090; ein Laden IN 2090 kommt im Spiel nicht vor, der Boot-Weg ruft re15_gatorszene_r41_install nicht.
6. Abnahme P H1 (Access violation F56, direct3d, 2091) ist damit nicht erklaert, das ist ein anderer Renderer. Der neue Ausnahme-Haken zeigt beim naechsten Mal Adresse und Stapel.

## Fuer den Nutzer
- Der tote Alligator bleibt tot: nach dem Verlassen des Raums, ueber Spielstand laden und nach dem Aufheben des Water Key, Leon und Elza. Flieht man vor seinem Tod ueber die Leiter, ist er beim Zurueckkommen weiter da, sonst waere der Schluessel weg.
- RE15_GATOR_ORIGINAL zeigt jetzt den ausgelieferten Raum mit den zwei Spinnen.
- Treppenwange ROOM6010/6011 Kamera 5: Figuren hinter der Wange sind verdeckt, an allen gemessenen Stellen 0 Punkte auf dem Teil sichtbar (Leon und Elza). Im Spiel selbst steht an diesen Plaetzen nie Kamera 5 (Abnahme 0: Cut 7, Cut 13), die Aenderung sieht man nur mit RE15_FORCE_CUT=5; eine Figur auf der Treppe in dieser Kamera bleibt unberuehrt.
- Freistellungs-Liste (Dossier M "Fuer den Nutzer") ist vollstaendig: 8 Stellen dazu, je mit Restbild, Teile-Bericht aus dem ausgelieferten Lauf. ROOM4040/4041 Kamera 12 hat keine HINTER-Stelle.
- Assets fuer das Paket: nur geaenderte Dateien `shared_assets/PSX/MASKS/ROOM6010.MSK`, `ROOM6011.MSK` (Sektion 5 jetzt 126 statt 128 Rechtecke, Datei 40 Byte kuerzer), `ROOM6010_PRI05.TIM`, `ROOM6011_PRI05.TIM`. Keine neuen Dateien unter shared_assets/RE2, keine Sprachdateien, keine Bank-9-Bits (Bit 126 bleibt frei), keine Nachrichten-Ids. Neue Quelle `platform/pc/src/endgrund_r41_pc.c` (nur Windows, beim naechsten Configure erfasst).
- Entscheidung: Soll die GPU-Grenze auch im Spiel fuer die Effekt-Quader gelten (OFFEN 1)? Dann verschwinden die Bloecke im Westgang von 20B0.

## Nachbesserung 1 (Abnahme 0, Mangel M1, Weg (a): Teil 8 auf die Ebene der Wange)

### Beleg: wo die Wange liegt (Triangulation, Schritt 1, UNGEBAUT)
Die Zelle @0x0092C (Westflaeche x -19919) ist eine Kollisionszelle, keine Wange. Kameras 4 (-24312,-10127,-6937) und 5 (-23912,-9927,-23336) sehen die Aussenflaeche der Westwange, Basislinie rund 16 000. Bildkanten je Zeile aus dem Hintergrund (Helligkeitsprofil, Sprung olive/schwarz), Sehebene je Kamera, Schnitt der zwei Ebenen:
| Kante | Bildkante Cut 5 | Bildkante Cut 4 | x bei z -20000 | y bei z -20000 | dx/dz | dy/dz |
|---|---|---|---|---|---|---|
| Unterkante der Aussenflaeche | (147.5,204)..(165.7,236) | (259.5,110)..(271.6,134) | -20630 (Sigma 7) | -4044 | 0,013 | -0,834 |
| Oberkante der Aussenflaeche | (158.5,204)..(177.6,236) | (265.5,110)..(277.6,134) | -20552 (Sigma 6) | -4717 | 0,008 | -0,815 |
dx/dz ~ 0: beide Linien laufen parallel zur Treppe (achsparallel), die Paarung der Kanten stimmt. dy/dz passt zur Zellenlaenge (5400/6445 = 0,838), nicht zum Stair-Record (5400/6895 = 0,783, das ist der Laufbereich). Die Stufenlinie der Zelle (y = -1800 - 0,838 (z + 24100)) liegt bei z -20000 auf -5236, die Unterkante also 1192 darunter: das ist der Abstand, den die Abnahme an der Ostwange in Cut 6/11 mass (1125..1225).

### Ursache des Mangels
Tiefe von Teil 8 kam aus dem Koerper der Modell-Zeile, und der war die Kollisionszelle @0x0092C (Westflaeche x -19919). Diese Flaeche liegt mitten unter den Stufen, die Wange 672 Einheiten westlich davon. Die Stufen 112..126 waren je Punkt 19..22 Stufen zu fern; eine Figur zwischen Wange und Zellflaeche lag vor der Tiefe und blieb sichtbar. Der Erststand hat nur die Spaltentiefe behoben, und seine Begruendung mass gegen dasselbe Modell. Die Abnahme hat das richtig bewertet.

### Messung vorher
Liegt aus Abnahme 0 vor, nicht wiederholt: (-19912,-19720) 63 von 96 sichtbar, (-19812,-19620) 50 von 141, (-20012,-19820) 58 von 61; Elza (-19612,-19720) 14 von 114. `fern` 844 in Stage 6. Abnahme: k = 12 ergibt an 8 Stellen 0 sichtbar, 0 uebermalt.

### Beleg
- Kameras 4 und 5 aus dem RDT (@RDT+0x24 Kamerasatz, `re15_camera_build_view`). `tools/maske/wange_r41.py`, Proben je Bildzeile im Helligkeitsprofil des Hintergrunds, `--ppm` prueft jede Probe am Bild: 0 neben dem Sprung, Spruenge 16..83.
- Ergebnis mit der exakten Inverse der gerundeten Kamera-Matrix: Unterkante z -20500 (-20631,-3606), dx/dz 0,011, dy/dz -0,835; Oberkante (-20551,-4291), dx/dz 0,006, dy/dz -0,815. Ebene x -20591. Die Tabelle oben entstand noch mit der Transponierten (-20630 / -20552); der Riegel in C fiel darauf auf, das Werkzeug rechnet jetzt wie `koerper_r41.ansicht`.
- Gegenprobe der Paarung: die Kanten mit Cut 11 geschnitten ergeben keine Treppenlinie (x -19688, Oberkante dx/dz 0,075): Cut 11 zeigt die Ostwange, wie die Abnahme schrieb.
- Stair-Record slot 9/10 `corner=-24300 extent=6895` ist der Laufbereich: mit ihm rechnete die Abnahme 5400/6895 = 0,783, die Treppe im Bild steigt wie die Zelle @0x0092C (5400/6445 = 0,838). Der echte Aufstieg bestaetigt das: (-19704,-4384,-21134), (-19701,-6184,-19025), Steigung 0,853.

### Aenderung
- Koerper = Zelle @0x0092C bis zur Westflaeche der Wange: x -20591..-17054, z -24100..-17655, Band 1, Hoehe -3290 (`platzieren_r41.oberkante`). Sued-, Nord-, Ostkante und die 12 Bytes bleiben Beleg, Westkante und Hoehe sind Messung. Eine Zelle fuer die Wange gibt es im RDT nicht: Band 4 @0x938 x -20284..-19945 reicht nur z -18284..-9741 und die Strahlen von Teil 8 treffen z -20963..-20069.
- `tools/maske/stufen_r41.py --wange -20591`: 13 Stufen 105..93, je Stufe ein Rechteck R113..R125 (vorher 15, R113..R127, 126..112). 426 Punkte, Fensterflaeche im Atlas neu vergeben, R0..R112 bytegleich. Sektion 126 Rechtecke.
- `gen_r41_masken36_zellen.inc` Zeilen R113..R125, R126/R127 weg; `gen_r41_masken36_platziert.inc` Sektion 114 -> 112; Ausschluss-Tabelle unveraendert (Riegel `unit_r37_masken36_ausschluss` gruen), `engine/` nicht beruehrt.
- `test_r37_masken36.c`: Tabelle `r41_wangen[]` (x -20591, Beleg der Zeilen: Zelle + Westkante, Ostkante gleich), Modus `wange`.
- Konstanten: Ebene -20591 aus `wange_r41.py`; bucket = (1023*vz)>>16 (@0x8002565c `sra v1,v1,4`, ZSF3 341 @0x80066c70), Masken-OT = depth (@0x80039650). Die Form "Tiefe = bucket(Eintritt in den Koerper)" ist Regel 6 von platzieren_r41.py. Teil 8 ist im Original nicht eingesetzt: Teile-Einsatz ist Entscheid M-E2, die Ebene eine Messung am Bild, keine Spielkonstante.

### Messung nachher (exe, Bild 120, A mit, B ohne, A0 mit ohne Figur, D ohne ohne Figur, Laeufe Q41n1/runs)
| Stelle Leon | Figur auf dem Teil | sichtbar vorher | sichtbar nachher |
|---|---|---|---|
| (-19912,-19720) Stelle des Mangels | 96 | 63 | 0 |
| (-19812,-19620) | 141 | 50 | 0 |
| (-20012,-19820) | 61 | 58 | 0 |
| (-20112,-19920) | 32 | 32 | 0 |
| (-20212,-19920) | 16 | 16 | 0 |
| (-20312,-19820) | 1 | 1 | 0 |
| (-19812,-19920) | 103 | 47 | 0 |
| (-19912,-19520) | 98 | 54 | 0 |
| (-19912,-19320) | 77 | 32 | 0 |
| (-19612,-19720) | 195 | 0 | 0 |
| (-19712,-19820) | 155 | 9 | 0 |
| (-20112,-19620) | 29 | 27 | 0 |
| (-20212,-19620) | 8 | nicht gemessen | 0 |
| Stelle Elza | | | |
| (-19812,-19620) | 62 | 23 | 0 |
| (-19612,-19720) | 114 | 14 | 0 |
| (-19712,-19820) | 74 | 14 | 0 |
| (-20012,-19820) | 20 | 20 | 0 |
| (-20112,-19620) | 1 | nicht gemessen | 0 |
Ausserhalb von Teil 8 uebermalt: an allen 22 Stellen 0. (-20312,-20020), (-20412,-19920), (-20312,-19620), Elza (-20212,-19920): 0 Figurpunkte auf dem Teil.
`fern`: ROOM6010/6011 C5 422 + 422 -> 0 + 0, Stage 6 844 -> 0, gesamt 1188 -> 344. `neustellen 6010 5 113`: `V -`, an keinem erreichbaren Platz steht eine Figur VOR dem Teil auf ihm.
**Figur auf der Treppe** (echter Handler, Aktionstaste am Fuss, RE15_FORCE_CUT=5, westlichste Spur x -19700): Leon 41 Bilder F60..F180, Elza 23 Bilder F114..F136: 0 Figurpunkte auf dem Teil. Sie kommt bis 7,2 Bildpunkte heran (Elza 10,8), im Kasten von Teil 8 liegen Bild 123/126/129 20/27/38 Figurpunkte, davon 0 uebermalt (Elza Bild 126/130/131 7/6/5, 0). Andere Teile uebermalen sie in 4 Bildern mit 22 Punkten: das sind die alten Rechtecke, bytegleich zum Stand davor.

### Riegel
- `unit_r41_restpunkte_wange`: Ebene aus den RDT-Kameras nachgerechnet, dx/dz, dy/dz, Tabelle.
- `unit_r41_restpunkte_teil8(_elza)` jetzt R113..R125: jeder Punkt bucket(Eintritt in die Wange) == Rechteck-Tiefe.
- `unit_r41_restpunkte_teil8_davor`: `V -`.
- `integration_r41_restpunkte_masken`: 10 Szenen Art Z, Stelle des Mangels q8a, Fenster nur aus Figurpunkten auf dem Teil (`Q41n1/tools/fenster.py`). Die Szenen q8v/q8ve ("Figur VOR der Wange") sind weg: die Figur stand dort hinter der Wange.
- `integration_r41_restpunkte_treppe(_elza)`: neu.
- `unit_r41_masken36_fern`: Schranke 344, Stage 6 == 0.

### Gegenprobe
Die vier Maskendateien des Stands davor (6887d9b88) eingesetzt, Szenen unveraendert, `integration_r41_restpunkte_masken`: FAILED, 9 von 10 Szenen rot, q8a 41 von 66 sichtbar, q8b 26 von 89, q8c 44 von 45, q8d 26 von 26, q8h2 6 von 119, Elza q8ea 13 von 37, q8eb 11 von 88, q8ec 12 von 65, q8ed 14 von 14. Nur q8h1 besteht dort, das ist die Stelle, die der Erststand behoben hatte. Damit unterscheiden auch die Elza-Szenen beide Staende (Hinweis H7 der Abnahme erledigt). Dateien danach zurueck, `git status` leer.

### OFFEN aus Nachbesserung 1
1. Teil 8 ist nicht die ganze Wange. In den Bildzeilen 202..239 deckt es 232 von 438 Punkten der Aussenflaeche (Band aus den Kanten UNTEN5/OBEN5), mehr hat der Atlas dort nicht. Ausserhalb des Teils bleiben an den Stellen 0..18 Figurpunkte auf der Flaeche sichtbar (meist (165,235), (166,237); groesste Stelle (-19912,-19320) 18 Punkte um (160,224)). Von Band-0-Stellen erreicht die Figur die Flaeche oberhalb von Bildzeile 202 nicht (0 Punkte an allen Stellen). Das sieht man nur mit RE15_FORCE_CUT.
2. Die Ebene ist eine Messung am Bild, keine Spielkonstante. Die beiden Kanten liegen 80 auseinander (-20631 / -20551), die Fehlergrenze aus 0,5 Bildpunkten je Probe ist Sigma 7; ob die Flaeche leicht schraeg steht oder die Kanten verschiedene Flansche sind, ist nicht bestimmt. Fuer das Bild aendert das nichts: die Abnahme mass k = 12..20 Stufen ohne Unterschied, die Ebene liegt mit 19..22 Stufen dahinter nur um wenige Stufen weiter, und vor ihr steht keine Figur auf dem Teil.
3. Wer `platzieren_r41.py` neu laufen laesst, muss danach `stufen_r41.py --von 113 --wange -20591 --tabelle-aus ... --platziert ...` fahren, sonst steht Teil 8 wieder auf der Zelle. Die Riegel `unit_r41_restpunkte_teil8` und `unit_r41_masken36_fern` fallen dann rot.
