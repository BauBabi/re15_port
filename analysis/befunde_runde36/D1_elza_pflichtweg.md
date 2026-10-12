# Runde 36 — D1: Elzas neuer Pflichtweg zum Prison Key (Messbericht)

Auftrag: Q_abnahme_1.md §8 H7 / NACHLAUF.md Nr. 63 — die Kette Minidisc Player (1201) -> Stimmschloss (1101) ->
Blue Keycard (1111) -> Kartenleser (10D1) -> Prison Key (10F1) -> Zelle (1211) ist seit Spur Q fuer Elza Pflicht.
Gemessen am Integrationsbaum `r36/integration`, exe von HEAD ba384fac (gebaut 01:28, md5 712b9f43cfe0c600bab270c185f33390,
Messkopie `re15_pc_D1.exe`, nach den Laeufen entfernt). Kein Code geaendert, nicht gebaut, kein ctest.
Nach der Messung kam nur Spur kartenbits dazu (9dbeea94/31571009: Lageplan-Bits 90/91 -> 77/78, Dateien
`re15_kartenitems_r36.h`, `gen/kartenitems_r36.inc`, `re15_map_owned.c`) — kein Mechanismus der Kette (Hinweis H5).

## Ergebnis: GANGBAR — kein Glied sperrt

Elza kommt nach E10/E11 mit echten Interaktionen durch die ganze Kette: Minidisc Player genommen (1201), Stimmschloss
geoeffnet (1101, Cursor-Raetsel + Wegwerf-Abfrage), Blue Keycard genommen (1111), Kartenleser mit Code 4312 geoeffnet
(10D1, Wegwerf-Abfrage), Prison Key genommen (10F1, (4,0xEC)=1), zurueck nach 11E1, Tuer "Prisons", Befreiungsszene in
1211 bis "Elza: John. Wait!" und Wegwerf-Abfrage (Ja -> weg), danach (4,0xFC)/(4,0x60)/(3,0x2C) = 1 (die Pflichtkette
E13 ff. geht weiter). Drei Bedingungen, die H7 nicht nennt, liegen zusaetzlich auf dem Weg — alle erfuellbar, keine
braucht einen neuen Gegenstand:
1. Die Tuer 11E1 -> 1201 liegt hinter dem Draht-Gitter: erst nach E10 ((4,0x24), Pliers aus 11B1) erreichbar.
2. In 10D1 liegt die Tuer nach 1101 hinter dem Rolltor; Elza muss den Rolltor-Schalter druecken ((4,0xF7)).
3. Der Kartenleser 10D1 verlangt neben der Blue Keycard den Code 4312. Dessen Original-Quelle (Zettel der Leiche in
   1111) hat der Port ersetzt; einzige Quelle ist Marvin's Notes in ROOM1021 (Hinweis H2).

## 1. Statisch: Glieder, Sperren, Reihenfolge

### 1.1 Glieder (RDT-Datei-Offsets aus `re15_port/tools/scd_dump_room.py`, RDTs = `shared_assets/PSX/STAGE1`)

| # | Glied | Ort / Ausloeser | Bedingung (Original-SCD) | Ergebnis |
|---|---|---|---|---|
| G0 | Weg nach 1201 | ROOM11E1 main00 @0x01532 Door_aot_set Slot 1 (sce 2, x 1000..3000 z -25000..-23500) -> 1201 | Tuer selbst unbedingt; der Gang D dorthin kreuzt SCA-Zelle 0 (x -14100..-6800 z -12600..-10600), die sub00 sperrt: @0x01798 `21 03 7f 00` -> @0x017B0..@0x017C0 `37 0p 00 ff`, sonst @0x017CC `21 04 24 00` -> @0x0181A..@0x0182A `37 0p 00 ff` (u0 = 0xff) | offen erst mit (3,0x7F) Pliers UND (4,0x24) Draht geschnitten (sub21 @0x01FC6 `22 04 24 01`) = E10 |
| G1 | Minidisc Player | ROOM1201 sub00 @0x0091E Aot_set Slot 2 (sce 3 -> sub03, x -27000..-26000 z -17100..-15800); Gegenstand @0x00932 `50 03 09 31 .. 44 00 01 00 68 00` (0x44, Menge 1, Bit (9,0x68), Rechteck 0 = nur per Aot_on) | sub03 @0x009F4 Set(2,7)=1, @0x009FC Ck(9,0x68)==0 -> @0x00A00 `47 03` Aot_on 3 (Aufnahme-Modal); danach @0x00A0C Evt_exec sub02 (Trage), @0x00A36 Set(4,0x11)=1 | sub01 @0x0094E Ck(9,0x68)==1 & @0x00956 Ck(3,0x67)==0 -> @0x0095A `22 03 67 01` |
| G2 | Rolltor 10D1 | ROOM10D1 sub00 @0x0123C `21 04 f7 00` -> @0x01240 Aot_set Slot 18 (sce 3 -> sub21, x 4700..5500 z 19400..20200) + @0x01268 Rolltor-Obj 0; sonst @0x012A2..@0x012B2 `37 0p 0c 00` | sub21 @0x019C8 msg 11 "It's a shutter switch. Will you push it?", @0x019D2 Ck(12,0x1F)==0 -> @0x019D6 `22 04 f7 01`, @0x01A68..@0x01A78 `37 0p 0c 00` (SCA-Zelle 12 x 4500..9350 z 21500..21800 auf) | Gang zur Tuer main00 @0x011D6 Slot 16 -> 1101 frei; kein Gegenstand noetig (Elzas 10D1 ohne Marvin-Szene) |
| G3 | Stimmschloss | ROOM1101 sub00: @0x00AC8 Ck(3,0x67)==0 -> @0x00ACC Tuer Slot 1 inert + @0x00AD6 Text msg 1; sonst @0x00B06 Ck(4,0xE8)==0 -> @0x00B0A Tuer inert, @0x00B14 Slot 2 sce 3 -> sub07, Zellen Slot 4/5 @0x00B3C/@0x00B50, Cursor Obj 0 @0x00B64 (-19554,0,22684) | sub07 @0x00D56 msg 3 "Will you use the Minidisc Player w/ Disc?", @0x00D60 Ck(12,0x1F)==0; sub01 Sce_key_ck 0x01/0x04/0x02/0x08 (@0x00BB0/@0x00BC8/@0x00BE0/@0x00BF8) -> sub03..06 (Speed_set +-200), @0x00C14 Member_cmp(m0F==4) + @0x00C1E `51 01 40 00` -> sub08, @0x00C3C Member_cmp(m0F==5) -> sub02 | sub02 @0x00C68 `22 04 e8 01`, @0x00C90 msg 4, @0x00CBA msg 2, @0x00CC8 `46 01 02 31 ..` Tuer 1111 scharf. Kein Inventar-Test auf 0x44 (nur (3,0x67)) |
| G4 | Blue Keycard | ROOM1111 main00 @0x00B2E `50 08 09 31 .. 38 00 01 00 34 00` (0x38, Bit (9,0x34), x 3000..3800 z 4750..5550) | unbedingt | Aufnahme setzt (9,0x34) |
| G5 | Kartenleser + Code | ROOM10D1 main00 @0x0103E Slot 1 sce 3 -> sub20 (nur solange Ck(3,0x32)==0) | sub20 @0x01998 msg 7 (Frage), @0x019A2 Ck(12,0x1F)==0, @0x019AA `21 09 34 01` -> msg 9 + @0x019B4 Evt_exec sub17 (Tastenfeld, @0x018D6..@0x018DE (5,0..2)=1); Ziffern-Subs sub09/08/06/07 = Zellen 6/5/3/4 = "4312"; sub01 @0x01510 Evt_exec sub19 + @0x01514 `22 03 32 01` | sub19 @0x01972 `46 02 02 31 ..` Tuer 10F1 scharf; beim Wiederbetreten main00 @0x01174 Door Slot 0 -> 10F1 |
| G6 | Prison Key | ROOM10F1 Port-Zone Slot 15 (Spur Q, `schluessel_r36.c`, x 7850..8850 z 2050..3050, Item 0x42, Bit (9,83)) | nur solange (9,83)=0 | Aufnahme -> (9,83) + (4,0xEC) (Haken `item_modal_common.c`) |
| G7 | Tuer "Prisons" | ROOM11E1 main00 @0x01576 `21 03 8b 00` (Text/Kartenleser/inerte Tuer) bzw. @0x016C4 Door Slot 3 -> 1211 | (3,0x8B) = E11 (Yellow Keycard + 0513) | Tuer im Gang B, NICHT hinter dem Draht-Gitter |
| G8 | Zelle | ROOM1211 sub00 @0x01DEA `21 04 ec 00` -> @0x01DEE Text Slot 8 (msg 10); sonst @0x01E1A Ck(4,0xFC)==0 -> @0x01E22 `2c 06 03 41 ..` (Slot 6 Auto-Zone -> sub02) + @0x01E36 John (Typ 0x49) | (4,0xEC)=1 | sub02 @0x01F76 (4,0xFC)=1, @0x01F82 (4,0x60)=1, @0x01F8A (3,0x2C)=1, @0x0226A msg 9 |

Alle Gates der Kette sind Fortschritts-Flags (Taken-Bits/Bank 3/4), keine Inventar-Abfragen: wer Minidisc Player oder
Blue Keycard zwischendurch in die Item-Box legt, kommt trotzdem weiter (G3 prueft (3,0x67), G5 prueft (9,0x34),
G8 prueft (4,0xEC)).

### 1.2 Wege zwischen den Gliedern (Tuergraph, alle Tueren `sce 2` ohne Bedingung, soweit nicht genannt)

11E1 -> 1201 -> 11E1 -> 10A1 (Treppenhaus, Band 1 -> 8) -> 1051 -> 1031 -> 1041 -> 1061 (Treppenhaus, Band 0 -> 4)
-> 10C1 -> 10D1 -> 1101 -> 1111 -> 1101 -> 10D1 -> 10F1 -> 10D1 -> 10C1 -> 1061 -> 1041 -> 1031 -> 1051 -> 10A1 -> 11E1 -> 1211.

Belege: 11E1 @0x01552 (-> 10A1); 10A1 @0x00DB2 (-> 1051, Band 8) / @0x00E20 (-> 11E1, Band 1), Treppen-AOTs sce 13
@0x00E40..@0x00ECC; 1051 @0x00B5A (-> 10A1) / @0x00ADA (-> 1031), Rolltor 1051: sub00 @0x00C48 `21 03 79 00`, sonst
@0x00C86..@0x00C96 `37 0p 13 00` (Zelle 19 auf) — im Port erst nach der Sicherung (Runde 34 A, `re15_rolltor.h`; fuer
E9 ohnehin Pflicht); 1031 @0x01CD8 / @0x01D18; 1041 @0x010B8 / @0x010D8; 1061 @0x00D52 (Band 0) / @0x00D32 (Band 4),
Treppen @0x00D72..@0x00DFE; 10C1 @0x00E42 / @0x00EA4; 10D1 @0x011B6 / @0x011D6 / @0x01174; 1101 @0x0099A / @0x009BA;
1111 @0x00A4E; 10F1 @0x00F32. Andere Wege fallen aus: Fahrstuhl 1081 nur mit Strom (1041 @0x01096, 10C1 @0x00E82
`!(Ck(4,0xF3)==0)`), 1091 -> 1101 nur ueber die Dach-Kiste.

### 1.3 Reihenfolge in Elzas Hauptweg (ablauf_elza.md E1-E12) — vor der Zelle erreichbar

- Fruehester Beginn: nach E10. Die Tuer nach 1201 (G0) liegt hinter dem Draht-Gitter; Pliers (E9, 11B1) -> Draht (E10).
- 1101/1111/10D1/10F1 sind ab Spielbeginn betretbar, aber G3 braucht (3,0x67) aus 1201 -> die Kette ist erst nach E10
  vollstaendig loesbar. E11 (Tuer "Prisons") ist von der Kette unabhaengig (Gang B, nicht hinter dem Gitter).
- Nichts in E1-E11 sperrt ein Glied wieder: G2/G3/G5 haengen nur an Flags, die erst die Kette selbst setzt; Leons
  Fortschritt spielt keine Rolle (eigene RDT-Varianten, Flags im Elza-Spielstand).
- Damit: E9 -> E10 -> [E11] -> G1 -> G2 -> G3 -> G4 -> G5 -> G6 -> zurueck -> E12. Ohne Prison Key bleibt die Zelle zu
  (G8 Text msg 10), Elza kann aber jederzeit zur Kette zurueck — kein Softlock.

## 2. Dynamisch an der exe (Elza)

### 2.1 Methode
- Startzustand per Kartenwerkzeug `probe_r36_schluessel_karte` + CONTINUE (`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`):
  ROOM11E1 (9100,4200) vor dem Kartenleser "Prisons" = Elza nach E11, Figur Elza (`[pl] Spieler-Familie PL04
  (character=4, Elza-Bit=1)`). Flags (synthetisch, nur was E1-E11 setzt): (3,0x6F) (3,0x70) (3,0xC1) (3,0x7D) (9,0xA5)
  (3,0xAD) (4,0xF1) (4,0xFA) (3,0x74) (9,0x8A) (4,0x0F) (9,53) (9,63) (3,121) (9,0x0A) (3,0x7F) (3,0x84) (4,0x24) (3,0x8B);
  Inventar SIG P228 x15, H. Gun Bullets x30, Yellow Keycard (`D1_belege/startkarte_argumente.txt`). (9,53)/(9,63)/(3,121)
  = Sicherung genommen/eingesetzt/Rolltor 1051 offen (Port, Runde 34 A — fuer E9 Pflicht).
- Alles danach ECHT gespielt: Laufen geschlossen geregelt mit `RE15_WEGPLAN` (GEHE/BLICK/AKTION = Viereck), Druecke in
  Szenen/Raetseln mit `RE15_PAD_AT` auf gemessene Bilder (Zeitachsen vorher in den Erkundungslaeufen x1101c/x10d1b
  bestimmt, in b1/b3 bildgleich bestaetigt). Zwischen den Abschnitten speichert das Spiel selbst
  (`RE15_SAVE_TEST_AGAIN`/`RE15_SAVE_TEST_EXIT_AFTER`, naechster Kartenplatz) und der naechste Lauf laedt per
  CONTINUE — der Zustand wird nicht per Haken uebertragen. Karte mit allen Zwischenstaenden:
  `D1_belege/re15_card_kette.mcr` (Platz 0 Start ... Platz 7 vor der Tuer "Prisons" mit Prison Key).
- Messhilfe: `GEGNERWEG` (alle Gegner-Aktoren beim Raumeintritt freigegeben) in jedem Raum ausser 1211 (dort ist John
  ein Aktor). Kampf ist nicht Teil der Messung (Hinweis H3).

### 2.2 Je Glied (Bildnummern = Raum-Bildzaehler; Wegplan-Log `laeufe/<lauf>/wegplan.txt`, debug.log, discard/modal)

| Glied | Lauf | Beleg |
|---|---|---|
| Start | a3b | `[save] CONTINUE: resumed in room 11e1 (hp=100)` |
| G0 Tuer 1201 | a3b | 11E1 durch den offenen Gang D, F992 AKTION bei (1995,-25321) -> `[aot] DOOR FIRE slot=1 rect=(2000,-24250,hw=1000,hh=750) ... spawn=(-20154,0,-25245)` -> `PC loaded room1201` |
| G1 Minidisc | a3b | F273 AKTION bei (-25661,-16394) Blick 2032; modal.log F275 `type=0x44` (Zoom), F301 Zustand 6 (Frage); `RE15_PAD_AT=440:A` (Ja); `[evt] F441 room=1201 Evt_exec sub=2` (Trage); Wegplan F527 FLAG (9,104)=1, F528 (3,103)=1 [= (3,0x67)], F529 (4,17)=1 |
| Rueckweg 1201 -> 10A1 | a3b | F793 -> 11E1, F791 bei (4682,-4292) -> `DOOR FIRE slot=2 ... spawn=(21200,-1800,25500)` -> 10A1; F34 (9,104)=1; `[save] saved (room 10a1)` |
| 10A1 -> 10D1 | a4d | stair_dbg `[stair] START dir=UP cur_band=1 target_band=3` ... `6 -> 8`; F1283 -> 1051; F562 -> 1031; F581 -> 1041; F365 -> 1061; `UP 0 -> 2`, `UP 2 -> 4`; F653 -> 10C1; F458 -> 10D1 |
| G2 Rolltor 10D1 | a4d | F208 AKTION (5771,19761) -> `[msgtext] room=10d1 id=11 "It's a shutter switch. Will you push it?"`; F301 AKTION (Ja); F584 FLAG (4,247)=1; F1038 bei (-18518,23561) -> `DOOR FIRE slot=16 ... spawn=(-20975,0,-25000)` -> 1101; F34 (3,103)=1, F35 (4,232)=0 |
| G3 Stimmschloss | b1 | F43 AKTION -> msg 1 "It's a control panel..."; F352 -> msg 3 "Will you use the Minidisc Player w/ Disc?" Ja; F395 (5,6)=1; Cursor (-19554,22684) -> F466 (-22554,18884); `[evt] F471 room=1101 Evt_exec sub=8`; F568 (5,5)=1; Cursor -> (-16554,16284); `[evt] F621 ... sub=2`; `id=4 "You've used the Minidisc Player w/ Disc."`; discard F781 `frage=8 gegenstand=0x44` -> F950 `weg=1`; `id=2 "You've opened the lock."`; F1185 FLAG (4,232)=1 |
| Weg 1101 -> 1111 | b2 | F32 (4,232)=1; F77 AKTION (-25940,-10857) -> `DOOR FIRE slot=1 ... spawn=(-2400,0,-5000)` -> 1111 |
| G4 Blue Keycard | b2 | F292 AKTION (3365,4391) Blick 3072 -> modal F293 `type=0x38`, F319 Zustand 6; F425 AKTION (Ja); F468 FLAG (9,52)=1 [= (9,0x34)]; F728 -> 1101; F543 -> 10D1; F34 (4,247)=1, F35 (3,50)=0 |
| G5 Kartenleser 4312 | b3 | F43..F764 8 x AKTION: msg 7 (Frage, Ja F558), msg 9 "You've used the Blue Keycard.", `[evt] F663 ... sub=17`, msg 0 "Enter the first number."; F787 (5,0)=1; Ziffern `[evt] F851 sub=9` ('4'), F938 (5,13)=1; `F1051 sub=8` ('3'), F1139 (5,14)=1; `F1231 sub=6` ('1'), F1320 (5,15)=1; `F1391 sub=7` ('2'), `F1392 sub=19`; `id=5 "You've opened the lock."`; discard F1551 `frage=8 gegenstand=0x38` -> F1720 `weg=1` |
| Tuer 10F1 | b4a | F32 FLAG (3,50)=1 [= (3,0x32)]; F95 AKTION (1693,-7012) -> `DOOR FIRE slot=0 rect=(800,-7050,...) spawn=(8400,0,-350)` -> 10F1 |
| G6 Prison Key | b4a | `[schluessel] ROOM10F1: PRISON KEY (Item 0x42, Bit (9,83)) Prop obj 4 bei (8050,-1460,2700), Zone Slot 15`; F103 AKTION (8344,2014) -> modal F104 `type=0x42`, F130 Zustand 6; F236 AKTION (Ja) -> `[schluessel] PRISON KEY genommen: Item 0x42, Bit (9,83)` + `Prison Key genommen -> Flag (4,0xEC) = 1 (Zelle ROOM1211)`; F279 (9,83)=1, F280 (4,236)=1 |
| Rueckweg 10F1 -> 11E1 | b4a, b4b | F367 -> 10D1; F510 -> 10C1; F527 -> 1061; `DOWN 4 -> 2`, `DOWN 2 -> 0`; F669 -> 1041; F357 -> 1031; F594 -> 1051; b4b F540 -> 10A1; `DOWN 8->6, 6->4, 4->3, 3->1`; F1306 -> 11E1 (`spawn=(5112,0,-5348)`); F34 (3,139)=1, F35 (4,236)=1 |
| G7/G8 Zelle | b5 | F35 AKTION (9108,5482) -> `DOOR FIRE slot=3 rect=(10450,5500,...) spawn=(-26400,0,-2200)` -> 1211; `[enemy] EM49 loaded` (John nur mit (4,0xEC)); F33 (4,236)=1, F34 (4,252)=0; F373 Szene (pm=2) bei (-20099,-18406) = Slot 6; msg 0 "Elza: Hey! You must be John right? Get up now!" ... msg 9 "Elza: John. Wait!"; discard F2188 `frage=8 gegenstand=0x42` -> `RE15_PAD_AT=2300:A` -> F2300 `weg=1`; F2303 Szenenende (pm 1), F2317 pm 0 (frei); F2377 (4,252)=1, F2378 (4,96)=1 [(4,0x60)], F2379 (3,44)=1 [(3,0x2C)], F2380 (9,83)=1 |

### 2.3 Gegenproben (synthetisch per Kartenwerkzeug, je eine Bedingung weggelassen)
- g11e1: 11E1 ab (5112,-5348) ohne (4,0x24): F32 FLAG (4,36)=0; GEHE durch Gang D -> state.log F479..F899 Spieler bei
  z = -10132 (= Zellen-Kante -10600 + Radius 468), nur seitliches Rutschen x -10369 -> -9949, kein `DOOR FIRE`.
- g10d1: 10D1 ab (7650,11400) ohne (4,0xF7): F32 FLAG (4,247)=0; GEHE nach Norden -> F479..F899 bei z = 21032
  (= 21500 - 468), kein `DOOR FIRE`.

=> G0 und G2 sind echte Sperren, die in der Kette ueber E10 bzw. den Rolltor-Schalter aufgehen.

### 2.4 Wegaufwand
Summe der Raum-Bildzaehler vom Kartenleser "Prisons" (Start) ueber die ganze Kette zurueck vor die Tuer "Prisons":
~18 480 Spielbilder (~10 min bei 30 Bildern/s; Summe der Teilwerte unten 18 479), ohne Kampf, ohne Tuersequenzen, mit Messroboter-Wegen
(a3b 2576, a4d 5497, b1 1122, b2 2292, b3 1720, b4a 3119, b4b 2153 Bilder bis zur jeweiligen Tuer/Stelle).

## 3. Blocker

Keine. Jedes Glied ist mit seiner Original-Mechanik (bzw. Spur Q fuer G6) im Port loesbar.

## 4. Hinweise (nicht blockierend, fuer Orchestrator/Nutzer)

- H1 Spielerfuehrung: Kein Text weist Elza auf den Funkraum. Roy zeigt nur msg 18 (Zettel 0513, Q_abnahme_1 Lauf
  R1_roy), die Zelle meldet msg 10 "It's locked. It seems that there is someone inside..." (1211 sub00 @0x01DEE), der
  Schluessel ist in Cut 0 von 10F1 9 Bildpunkte gross (Q H1), und der bisherige Nebenweg (2 Raetsel) ist jetzt Pflicht.
  Ob ein Hinweistext gewuenscht ist, ist eine Nutzerentscheidung (PORT-WAHL); mechanisch nicht noetig.
- H2 Code 4312: Original-Quelle ROOM1111 sub02 @0x00D04 Message_on 0 ("... The numbers 4312 are printed on the slip.")
  ersetzt der Port (Runde 34 F, `re15_leiche.h`: RE15_LEICHE_1110_RAUM als RE15_ROOM_BASE, gilt fuer 1111). Einzige
  Quelle: Marvin's Notes (Dok 3, `dokumente_r34.c` `dokument_des_raums` mit `room & 0xFFFE` -> auch ROOM1021;
  E_dokumente.md §3.7 "Dok 3 4312 oeffnet den Communication Room"). 1021 liegt auf Elzas Hauptweg (E5/E13). Dynamisch
  nicht aufgenommen (der Lauf kennt den Code).
- H3 Gegnerlast: auf dem Pflichtweg stehen (je Erstbesuch, `GEGNERWEG`-Zeilen) 11E1 4 (Gang D, zweimal zu durchqueren),
  1201 4 (werden beim Aufheben geweckt, sub03 @0x00A16..@0x00A32), 1031 6 (Typ 0x16 in der Halle), 10C1 3 Kraehen,
  10D1 1, 1101 5, dazu 1041 5 (Typ 0x16). Fehlversuch a2 (beim Wiederbetreten von 11E1 nicht entfernt): Elza bei
  (-10760,-16510) gegriffen F375, hp 100 -> 80 (F396) -> ... -> -20 (F660, pst 7) = Tod
  (`laeufe/fehlversuch_a2/state_hp_wechsel.txt`). Kein Blocker, aber deutlich mehr Kampf als der Original-Weg.
- H4 Weglaenge: ~10 min reine Laufzeit Umweg (2.4).
- H5 Lageplan B2 in 1201: im gemessenen Stand nahm die Karte Bit (9,90) (`[kartenitems] ROOM1201: Karte Blatt 1
  (Bit 9,90)`), das Original-Taken-Bit des Gegenstands 0x3F in ROOM2090/2091 (kartenbits_abnahme_0.md Z. 54-56); in
  HEAD 9dbeea94 auf Bit 77 verlegt. 1201 ist jetzt Pflichtraum, deshalb erwaehnt; die Kette nutzt die Karte nicht.
- H6 Fuer kuenftige Elza-Startzustaende: (3,0xCF) ist nach E2 wieder 0 (ROOM1031 sub13 @0x02982 `22 03 cf 00`);
  gesetzt startet main00 @0x020B2 den Prolog erneut (Fehlversuch a4: Tod in 1031 waehrend der Szene).

## 5. Grenzen der Messung
- Startflags nach E1-E11 synthetisch (Liste 2.1); E1-E11 selbst nicht gespielt.
- Gegner entfernt (Messhilfe), Kampf/Schaden nicht bewertet; Leiche 1111 nicht untersucht.
- Original (PSX/DuckStation) nicht gefahren: G0-G5/G7/G8 sind unveraenderte Original-Skripte, die die Port-VM
  ausfuehrt (Offsets 1.1); G6 und die Wegwerf-Abfragen sind Port-Zusaetze (Spur Q, Runde 21/22).

## 6. Messdateien
`analysis/befunde_runde36/D1_belege/`: `lauf.sh` (Laufskript), `plaene/` (Wegplaene a3, a4d, b1..b5f, Erkundung,
Gegenproben, Fehlversuche a2/a4), `pad_1101_stimmschloss.txt`, `pad_10d1_tastenfeld.txt` (PAD_AT-Folgen),
`startkarte_argumente.txt`, `re15_card_kette.mcr` (Speicherkarte mit allen Zwischenstaenden), `laeufe/<lauf>/`
(wegplan.txt, debug.log, run.log, stair_dbg.log, discard_auszug.txt, modal_auszug.txt, state_wechsel.txt),
`laeufe/fehlversuch_a2|a4/`, Hilfsskripte `doors.py`/`cells.py`/`plot2.py` (Tuer-/SCA-Auszug aus den RDTs).
