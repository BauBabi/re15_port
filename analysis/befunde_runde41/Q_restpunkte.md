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
- [ ] Bau im Spurbaum
- [ ] Punkt 1
- [ ] Punkt 2
- [ ] Punkt 3
- [ ] Punkt 4
- [ ] Punkt 5

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
`test_r37_masken36 teile`: 101 280 Punkte, Eintritt hinter der Tiefe 0. `test_r37_masken36 fern`: ROOM6010/6011 C5 je 422 statt 1508, gesamt 1188 (Stage 3: 2, 4: 72, 5: 270, 6: 844). Die 422 sind OT-Gleichstand: Rechteck-Tiefe = bucket(Eintritt) an JEDEM Punkt, also heisst "hinter dem Eintritt und nicht verdeckt" genau bucket(Figur) == Tiefe; Plaetze nur noch x -20112..-19712 (`fern_plaetze.log`, vorher -20112..-19512), also auf der Westflaeche der Zelle (x -19919).
An der exe (Leon, je 4 Laeufe mit/ohne/leer/leer0, A0 = (20000,20000), Bild 120, `Q41/runs/`, `tools/sz_ausw.py`):
| Stelle | Figur auf Teil 8 | vorher verdeckt / sichtbar | nachher verdeckt / sichtbar |
|---|---|---|---|
| (-19612,-19720) | 195 | 192 / 3 | 195 / 0 |
| (-19712,-19820) | 155 | 114 / 41 | 146 / 9 |
| (-19912,-19720) Stelle der Abnahme | 96 | 14 / 82 | 33 / 63 |
| (-20112,-19620) vor der Wange | 29 | - | 2 / 27 |
Bilder `Q_belege/m1_p2_vorher.png`, `m1_p2_nachher.png` (je A0, B, A, Lupe mit Rest in Rot).
Die 63 Punkte der Abnahme-Stelle sind kein Loch: die Stelle liegt 7 Einheiten neben der Westflaeche, der Kopf ragt zur Kamera vor die Flaeche. Gegenprobe mit um k Stufen vorgezogener Maske (nur fuer die Messung, Datei danach zurueck): k = 1: 59 sichtbar, 2: 48, 4: 23, 8: 0. Diese Punkte liegen also bis 8 Stufen VOR dem Koerper, sie zu verdecken waere der Fehler, den Regel 6 verbietet (Figur vor dem Gegenstand wird nie verdeckt).
