# Runde 36 — Spur H "ziel1190" (Dossier, fortlaufend)

Baum `.claude/worktrees/r36_ziel1190`, Zweig `r36/ziel1190`, Basis master 78a4a748.
Zuteilung (VERTRAG.md 1.2/1.3): Nachrichten-IDs ROOM1190/1191 = 6..11, Ereignis 27 (1190); ROOM1090 nur Dossier.
Keine Bank-9-Bits.

## Auftrag (woertlich, AUFTRAG.md Zeile 27 und 60)

1. "Der von dir ergänzte Text in ROOM 1190 bei der Zielscheibe ist falsch. Die Texte: "This target has a
   surprisingly large number of bullet holes." und "This target does not have many bullet holes" sollen sich
   hinten bei den Zielscheiben befinden, wenn man sie anklickt, nicht vorne bei den Knöpfen, die die
   Zielscheiben bewegen lassen... Bei Elza sind es genau die anderen zwei Zielscheiben, deshalb müssen die
   Texte da genau vertauscht sein."
2. "Dann habe ich mal die Frage - was hat es mit der Kiste in ROOM 1090 auf sich. Hat die irgendeinen
   Verwendungszweck?"

## Stand (Arbeitsprotokoll)

* [x] Punkt 1: Messung vorher (Knopf-Text r35 an der exe) — §1.1
* [x] Punkt 1: RE ROOM1190/1191 — Scheiben, Bahn begehbar, Kamera-Zuordnung, Elza-Variante, AOT-Form — §1.2
* [ ] Punkt 1: Umbau (Knoepfe zurueck auf Original, Texte an die Scheiben)
* [ ] Punkt 1: Messung nachher + Pins
* [ ] Punkt 2: Kiste ROOM1090 Zensus + Antwort

Werkzeuge (Scratchpad, nicht versioniert): `lauf.sh` (Messkopie `re15_pc_h36mess.exe` neben der exe, eigenes
Arbeitsverzeichnis, RE15_NO_INTRO/NOAUDIO/WINDOW_SCALE=1/TITLE_SHOT_AF=2), `py/raum_geo.py` (SCA/Kamera/RVD-Dump),
`py/zensus_super.py` (AOT-super- und Obj-Typ-Zensus ueber alle 240 RDT), `py/blatt.py` (Kontaktbogen).

---

## Punkt 1 — Zielscheiben-Texte ROOM1190/1191

### 1.1 Messung vorher (Basis 78a4a748, gebaute exe)

Lauf `v_knopf0`: `RE15_DEBUG_JUMP=1190@gp RE15_PLAYER_POS=-3180,-24500,2048 RE15_SET_FLAG=4:243 RE15_MSG_LOG=1
RE15_PRESS=square@1800..1803,@1960..1963 RE15_FRAMEDUMP=1790-2100/30:fd_ RE15_EXIT_AT=2100#1190`.
debug.log: `[ziel1190] ROOM1190 Slot 0 -> Port-Nachricht 6 (viele Einschuesse) + Original msg 0`.
Bild `H_ziel1190/p1_vorher_knopf0_F1790-2090.png`: am Knopf von Kabine 1 erscheint zuerst "This target has a
surprisingly / large number of bullet holes.", danach "There's a switch here. Push it? Yes No" — der Nutzerbefund
("vorne bei den Knöpfen") ist reproduziert. Ursache = Runde-35-Bau (engine/src/ziel_1190.c, Haken in scd_vm.c
op_message_on + re15_scd_show_message), keine Regression.

### 1.2 RE-Belege (ROOM1190.RDT / ROOM1191.RDT / PSX.EXE)

**(a) Die Scheiben** = Obj 0..3, sub13 Obj_model_set @0x0297E/0x029A0/0x029C2/0x029E4 (in 1190 und 1191
bytegleich): `2d 0n 00 00 01 00 88 00 00 00 a4 ac a2 ee <z> ...` x=-21340 (`a4 ac`), y=-4446, z=-24516/-20916/
-17316/-13716 (`3c a0`/`4c ae`/`5c bc`/`6c ca`), Kollisionsbox c(0,1260,0) h(45,1260,700) (`2d 00 ec 04 bc 02`).
Die Knoepfe (main00 Aot_set slot 0..3 @0x02226/0x0223A/0x0224E/0x02262, sce 5, Rechteck x=-4400 w=600) bewegen
Obj n: sub02..05 `Work_set 2e 03 0n` @0x024C8/0x02540/0x025B8/0x02630 -> sub06 (vor, `2f 00 96 00` +150/Bild
@0x0269A) bzw. sub08 (zurueck, `2f 00 6a ff` -150 @0x02702); beim Betreten setzt sub00 die Scheibe n auf x=-5220
(`34 00 9c eb` @0x02336/0x02348/0x0235A/0x0236C), wenn (4,n)=1. **Die Scheiben haengen also hinten (x=-21340)
und fahren bis vor den Tresen (x=-5220).**

**(b) Die Bahn ist begehbar** (Voraussetzung fuer "hinten ... anklicken"):
* SCA ROOM1190 (RDT+0x20 -> 0x1C04, 5 Regionen x 24 Zellen, je Region gleich): Tresen r0[0] @0x01C1C
  x=-5150..-4070 z=-28172..**-11504** (Typ 1, u0=fb, Band 0); Trennwand r0[6] @0x01C64 endet bei z=-11414;
  Suedwand r0[1] @0x01C28 beginnt bei z=**-10280** -> Durchgang 1124..1224 Einheiten am Tresenende.
* RVD-Kamerazonen (RDT+0x28 -> 0x2C8) schalten in der Bahn auf Cut 3 (Kamera am Bahnende, Blick zurueck),
  Cut 5 (Blick die Bahn hinunter), Cut 9 (Nahsicht der Scheiben) — Zone @0x002F0 liegt genau im Durchgang
  (x=-9003..-3303, z=-11500..-6900). Nach dem Hunde-Ereignis setzt sub13 einen Hund IN die Bahn
  (Sce_em_set @0x02968 x=-13814 z=-16682).
* GEMESSEN an der exe (Lauf `walk1`, `RE15_PLAYER_POS=-2800,-10900,2048`, `RE15_INPUT_SCRIPT=XU8`): Leon laeuft
  am Tresenende vorbei bis x=-20792 (F1891), Kamera 0 -> 3 -> 5.
* GEMESSEN je Scheibe (Laeufe `walkS24516/20916/17316/13716`, Start x=-17000 auf der Scheiben-z, Blick -x, U4):
  Leon bleibt stehen bei x=**-20845** (S0, S1, S2) bzw. **-20792** (S3). -20845 = Scheibenbox-Vorderkante
  -21340+45 = -21295 plus Radius 450; S3 haelt an der Zelle r0[12] (Kante x=-21260).

**(c) Welche Scheibe welche ist** (Kamera-Projektion, Runde 35 gemessen, Bild
`analysis/befunde_runde35/H_raeume/p2_kameras_cut5_6_7_9.png`): in Cut 5/6/7 (Blick vom Stand die Bahn hinunter,
-x) und Cut 9 (Nahsicht) liegen die Scheiben von links nach rechts als Obj 0, 1, 2, 3 (Kamera-Rechts = Welt +z
bei Blick -x). "Scheibe 1 und 3 von links" = Obj 0 und Obj 2.

**(d) Leon/Elza — das Raetsel selbst bestaetigt die Zuordnung.** sub01 kennt zwei Loesungen:
* Variante A @0x02438-0x02468 (`Ck 03 6f 00` = (3,111)=0): (5,4)=1, (5,5)=1, (5,6)=0, (5,7)=0 -> sub10 (Hunde).
  (5,4) setzt sub02 = Knopf 0 @0x024F4, (5,5) sub04 = Knopf 2 @0x025E4, (5,6) sub03 = Knopf 1 @0x0256C,
  (5,7) sub05 = Knopf 3 @0x0265C -> **Loesung A = Scheibe 0 und 2 vorn**.
* Variante B @0x02478-0x024A8 (`Ck 03 70 01` = (3,112)=1): (5,4)=0, (5,5)=0, (5,6)=1, (5,7)=1 -> **Loesung B =
  Scheibe 1 und 3 vorn**.
* Elzas Spielstart ROOM1241 sub02 (von sub00 `04 ff 18 02` @0x00544 sofort gestartet): `22 03 6f 01` @0x0055A
  und `22 03 70 01` @0x0055E -> (3,111)=1, (3,112)=1 -> in ROOM1191 gilt Variante B. Leon startet in ROOM1240
  ohne diese Bits -> Variante A. (Einzige Setzer im Bestand: ROOM1190/1191 sub10 @0x02776/0x0277E und ROOM1241,
  Runde-35-Zensus.)
* Elza spielt die "1"-RDT: Dateiindex = Leon-Index + Elza-Bit, `addu a0,a0,v0` @0x800397ec (re15_gameflow.h
  re15_room_for_char) -> ROOM1191.
=> Die "vielen Einschuesse" markieren in beiden Szenarien genau die Scheiben, die nach vorn muessen: Leon Obj 0+2
(= Nutzer "Scheibe 1 und 3"), Elza Obj 1+3 (= Nutzer "genau die anderen zwei ... vertauscht").

**(e) Form eines Anklick-Texts an einem Objekt** (Original-Engine, kein Raten):
* Text-Platz = sce 1 MESSAGE. Vorbild im selben Raum: main00 Else-Zweig @0x0227A
  `2c 00 01 31 00 00 d0 ee 9c 9b 58 02 60 09 02 00 ff ff 00 00` = slot, sce 1, sat 0x31 (Aktion + Vorwaertspunkt),
  Band 0, Rechteck w=600 (`58 02`) d=2400 (`60 09`), Nutzlast msg-Nummer + Pausenmaske 0xffff (alle 524
  ausgelieferten sce-1-Records tragen 0xffff, scd_vm.c op_aot_set). Handler LAB_80043084 -> FUN_80027e68
  (@0x80043098 `lhu a3,2(v0)`, @0x800430a4 `sll a3,a3,16`).
* Vorwaertspunkt 620 vor dem Spieler: FUN_80042bac `ori v0,zero,0x26c` @0x80042bd0, gedreht mit FUN_8004f008
  @0x80042bfc; Rechtecktest FUN_80042b64 @0x80042f20.
* **Anhaengen an ein Objekt** (der Scan kann das selbst): FUN_80042bac liest das "super"-Byte rec[3]
  (= Aot_set pc[5]) `lbu v1,3(s0)` @0x80042cd4; Bit 0x80 gesetzt -> Bit 0x40 klar = Objekt-Pool:
  Index `andi v1,v1,0x3f` @0x80042d58, Stride 0x94 (`sll 3 / addu / sll 2 / addu / sll 2` @0x80042d5c-6c),
  `lhu v0,0(at)` aus 0x800b3fcc (= Pool 0x800b3f98 + 0x34 = Objekt-x) @0x80042d7c und 0x800b3fd4 (+0x3C =
  Objekt-z) @0x80042db4; Bit 0x40 gesetzt = Gegner-Array 0x800acc60/0x800acc68 (Stride 0x1f4); Bit 0x80 klar ->
  Versatz 0 (`sw zero,40(sp)` @0x80042dc0). Danach Rechteck-Ecke = Versatz + rec x/z (`lhu v1,4(s0)` /
  `addu` @0x80042e60-0x80042e84), Breite/Tiefe unveraendert (@0x80042e88-9c). Zensus (alle 240 RDT,
  `py/zensus_super.py`): **kein** ausgelieferter Aot_set/Door_aot_set/Item_aot_set nutzt super&0x80 — die
  Mechanik ist in der Engine, aber unbenutzt; der Port wertet pc[5] fuer Plaetze nicht aus.
* Folge fuer den Bau: ein Text-Platz, der an Obj n haengt, ist im Original EIN Aot_set mit super = 0x80|n und
  einem Rechteck RELATIV zum Objekt — er faehrt mit der Scheibe mit (hinten wie vorn).


### 1.3 Umsetzung (Commits ac70f38f, f833268f; Fortsetzung 2026-10-11)

* `include/re15_ziel1190.h` (umgebaut): Herleitung im Kopf; Konstanten `RE15_ZIEL1190_SLOT0` 17 (PORT-WAHL:
  erste freie Plaetze nach dem letzten Raumplatz 16, sub14 @0x02D90), `HALB_X` 300 / `HALB_Z` 1200 (= w 600
  `58 02` @0x02284 / d 2400 `60 09` @0x02286 des Vorbild-Records main00 @0x0227A, halbiert), `SAT` 0x31
  @0x0227D, `BAND` 0 @0x0227E, `PAUSE` 0xffff @0x0228A, Nachrichten 6 = viele / 7 = wenige (VERTRAG 1.2).
* `engine/src/ziel_1190.c` (umgebaut): `re15_ziel1190_viele(room, obj)` (Leon Obj 0+2 = sub01 Loesung A
  @0x02438-0x02468; Elza-RDT `(room & 0xF) == 1` @0x800397ec -> vertauscht = Loesung B @0x02478-0x024A8),
  `_text` (Kopf `04 02` + Satz + `01 00` = Form von msg 2 @0x2E5E, Umbruch 0x08 @0x2E79), `_install`
  (vier sce-1-Plaetze 17..20, Mitte = Lage der Scheibe aus dem Prop-Pool, nie ueber einen aktiven
  Raum-Platz), `_folgen` (super-Anbindung: Platzmitte = Objekt-x/z vor jedem Scan, @0x80042d7c /
  @0x80042db4 / @0x80042e60-0x80042e84), `_show` (Text ablegen, Stimme `synchro/STAGE1/room<Raum>/main06|07.wav`,
  Oeffnen wie der sce-1-Weg, `work_vars[0]` = getroffener Platz @0x80042f3c fuer das Log).
* Haken (je eine Zeile): `aot_common.c` re15_aot_scan -> `re15_ziel1190_folgen()`; `scd_room_setup.c`
  Installer-Block -> `re15_ziel1190_install()`; `platform/pc/main.c` Boot-/CONTINUE-Weg (ohne
  scd_room_reenter) -> `re15_ziel1190_install()`; `scd_vm.c` re15_scd_show_message -> `re15_ziel1190_show()`
  (nur Id 6/7). Der Runde-35-Haken in `op_message_on` (Knopf msg 0 -> Scheiben-Satz) ist ENTFERNT: die
  Knoepfe laufen wieder ueber den ausgelieferten Weg (sub02..05 `Message_on 2b 00 00 00` bzw. sce-1 msg 2).
* Tests: `tests/unit/test_r36_ziel1190.c` (unit_r36_ziel1190, probes/r36_ziel1190.cmake) — Mechanik-Riegel
  (A) Plaetze, (B) 2 Raeume x 3 Zustaende x 4 Scheiben hinten, (C) Knoepfe = Original-Nachricht Byte fuer
  Byte, (D) Platz faehrt mit der Scheibe, (E) Betreten mit Scheibe vorn. `test_r35_raeume_ziel.c` entfernt
  (prueften den zurueckgenommenen Knopf-Text). `probe_r36_ziel1190_karte.c` = Mess-Werkzeug (Speicherkarte
  Leon ROOM1190 / Elza ROOM1191 fuer den LADE-Weg, kein add_test).
* Kein Asset-Patch; keine neuen Daten unter gen/ oder shared_assets/.

### 1.4 Messung nachher (gebaute exe des Zweigs, Stand f833268f + Merge master 2da98280)

**Leon, echter Laufweg vom Durchgang am Tresenende zu jeder Scheibe** (JUMP-Weg: `RE15_DEBUG_JUMP=1190@gp
RE15_PLAYER_POS=-2800,-10900,2048 RE15_INPUT_SCRIPT_START=1800 RE15_INPUT_SCRIPT=XU3.5,L0.36667,U<t>,R0.36667,U1,A0.1,W6`,
t = 6.05 / 4.45 / 2.85 / 1.25 fuer Scheibe 0..3, Laeufe `n_leon_s0..s3`, von der Vorgaenger-Sitzung gefahren,
hier ausgewertet). debug.log je Lauf:
```
n_leon_s0: [ziel1190] ROOM1190 Scheibe 0 angeklickt (Platz 17) -> Nachricht 6 (viele Einschuesse)
n_leon_s1: [ziel1190] ROOM1190 Scheibe 1 angeklickt (Platz 18) -> Nachricht 7 (wenige Einschuesse)
n_leon_s2: [ziel1190] ROOM1190 Scheibe 2 angeklickt (Platz 19) -> Nachricht 6 (viele Einschuesse)
n_leon_s3: [ziel1190] ROOM1190 Scheibe 3 angeklickt (Platz 20) -> Nachricht 7 (wenige Einschuesse)
```
Bild `H_ziel1190/p1_nachher_leon_s0-s3_F2309-2165.png` (Kontaktbogen, je das letzte Bild des Laufs, Cut 4 =
Nahsicht hinten): Leon steht vor der jeweiligen Scheibe, Untertitel "This target has a surprisingly / large
number of bullet holes." (Scheibe 0 und 2) bzw. "This target does not have / many bullet holes." (1 und 3).
Keine Ja/Nein-Frage, kein Original-Knopftext.

**Elza, LADE-Weg** (Speicherkarte aus `probe_r36_ziel1190_karte.exe re15_card.mcr 1191`, Charakter 4;
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=150`,
gleiches Laufskript). debug.log `e_elza_s0`: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`,
`[save] CONTINUE: resumed in room 1191 (hp=100)`, Plaetze `Scheibe 0 -> Text-Platz 17 (Nachricht 7, wenige)`,
`1 -> 18 (6, viele)`, `2 -> 19 (7, wenige)`, `3 -> 20 (6, viele)`, dann
`[ziel1190] ROOM1191 Scheibe 0 angeklickt (Platz 17) -> Nachricht 7 (wenige Einschuesse)`. Bild
`H_ziel1190/p1_nachher_elza_s0_F650.png`: Elza (PL04) vor Scheibe 0, Untertitel "This target does not have /
many bullet holes." — bei Elza genau vertauscht.
Elza Scheibe 1..3 (gleicher LADE-Weg, Laeufe `e_elza_s1..s3`, U-Zeiten 4.45 / 2.85 / 1.25):
```
e_elza_s1: [ziel1190] ROOM1191 Scheibe 1 angeklickt (Platz 18) -> Nachricht 6 (viele Einschuesse)
e_elza_s2: [ziel1190] ROOM1191 Scheibe 2 angeklickt (Platz 19) -> Nachricht 7 (wenige Einschuesse)
e_elza_s3: [ziel1190] ROOM1191 Scheibe 3 angeklickt (Platz 20) -> Nachricht 6 (viele Einschuesse)
```
Kontaktbogen `H_ziel1190/p1_nachher_elza_s0-s3_F690-555.png`: Elza vor Scheibe 0..3, Untertitel "does not have
many" / "surprisingly large number" / "does not have many" / "surprisingly large number" — Leon Obj 0+2 viele,
Elza Obj 1+3 viele, wie vom Nutzer verlangt ("genau vertauscht").

**Knopf 0 (Gegenprobe, Knoepfe wieder Original)** — derselbe Lauf wie die Messung vorher (§1.1): `RE15_DEBUG_JUMP=1190@gp
RE15_PLAYER_POS=-3180,-24500,2048 RE15_SET_FLAG=4:243 RE15_MSG_LOG=1 RE15_PRESS=square@1800,square@1960`, Lauf
`n_knopf0`. debug.log: `[msg] room=1190 id=0 savepoint=0` (zweimal: Frage + Antwort), KEINE `[ziel1190] ...
angeklickt`-Zeile. Bild `H_ziel1190/p1_nachher_knopf0_F1850-2090.png`: am Knopf von Kabine 1 kommt jetzt sofort
"There's a switch here. Push it?" (Schreibmaschine bei F1850), kein Scheiben-Satz mehr — im Gegensatz zum
Vorher-Bild (§1.1), wo zuerst der Scheiben-Satz stand.

Zeitbasis der Laeufe: JUMP-Laeufe mit `RE15_INPUT_SCRIPT_START=1800` (der Sprung meldet `AUTO-JUMP -> ROOM1190
(Frame 1768)`), LADE-Laeufe mit `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=150`.

Berichtigung Zeitbasis: ALLE Laeufe mit `RE15_INPUT_SCRIPT_BASIS=spiel` (Spielbilder); der Bildzaehler
(g_engine.frame_count, Basis von EXIT_AT/FRAMEDUMP/Skript) beginnt mit JEDEM Raumladen bei 0 — JUMP-Laeufe
`START=1800` zaehlt also ab dem Laden des Sprungziels, LADE-Laeufe `START=150` ab dem CONTINUE-Laden.

### 1.5 Tests

* `unit_r36_ziel1190` (tests/unit/test_r36_ziel1190.c, probes/r36_ziel1190.cmake): Mechanik-Riegel, s. 1.3.
* NEU `integration_r36_ziel1190_scheiben` (tests/integration/test_r36_ziel1190_scheiben.cmake, registriert in
  probes/r36_ziel1190.cmake): neun exe-Laeufe ueber den LADE-Weg (Speicherkarte aus probe_r36_ziel1190_karte,
  CONTINUE): L0..L3 Leon ROOM1190 Scheibe 0..3 -> Nachricht 6/7/6/7, E0..E3 Elza ROOM1191 (PL04 geladen) ->
  7/6/7/6, K Leon mit Strom 620 vor Knopf 0 -> `[msg] room=1190 id=0` und kein Scheiben-Satz. Eigener exe-Name
  (re15_pc_r36h_haken_<id>.exe, Muster test_r34n_b_cursor.cmake), Arbeitsordner r36_ziel1190_wd.
  Erster Lauf: `1/1 Test #572: integration_r36_ziel1190_scheiben ...   Passed  237.49 sec` —
  `100% tests passed, 0 tests failed out of 1` (ctest -R "integration_r36_ziel1190_scheiben").
* `probe_r36_ziel1190_karte` (Mess-Werkzeug, kein add_test): Speicherkarte 1190/1191 (Scheiben) und — nur fuer
  die Messung in Punkt 2 — 1090/1091 mit Lage `pos=x,y,z,rot` (Band aus y wie re15_collision_band_from_y).

---

## Punkt 2 — Die Kiste in ROOM1090: was sie ist und wozu sie dient

Vorweg (Verwechslungs-Pruefung): ROOM1190 (Schiessstand) hat KEIN Typ-4-Objekt (scd_dump_room: 0 Treffer
"type=4"); die Frage betrifft also ROOM1090/1091 (Hof hinter dem Revier mit dem brennenden Schuppen). Dort
stehen ZWEI Kisten.

### 2.1 Bestand (ROOM1090.RDT / ROOM1091.RDT, Datei-Offsets; scd_dump_room.py, raum_geo.py, rdt_msgdump.py)

* **Kisten** = Obj_model_set Typ 4 (schieb- und kletterbar), Flags pc[6..7] `00 01` | 1 = 0x0101 (Kletterbit
  0x100, FUN_8002d2c0 `& 0x101` @0x8002d358; Installer LAB_80040914 @0x80040990-a4), Box c(0,-900,0) h(900,900,900):
  * **obj 0** main00 @0x0216E (1091: @0x021BE): Band 1, (-8136,-1800,4753) = HOF (untere Ebene, y -1800).
  * **obj 1** main00 @0x02190 (1091: @0x021E0): Band 5, (-5960,-9000,-15482) = DACH (obere Ebene, y -9000).
* **Ebenen** (SCA @0x1DD0; Bandbyte = floor>>4; Spielerband = -y/1800, re15_collision_band_from_y /
  DAT_800acad6): Dachplatte = Zelle r2[9] @0x01F8C (x -10000..4500, z -17500..-3600, Byte 0x43 -> Band 4, Oberkante
  Band 5 = y -9000: gemessen steht der Spieler dort mit `band=5`); Nordbruestung des Dachs r2[0] @0x01F20 (x -7256..4624,
  z -18560..-16403, Byte 0x53 -> Oberkante Band 6) und Nordmauer r2[10] @0x01F98 (Byte 0x63); Hofboden r0[3] @0x01E0C
  (x -15243..10285, z -6284..8458, Byte 0x03 -> Oberkante Band 1 = y -1800).
* **Tueren**: slot 0 @0x0211A floor 1 (Hof), Rechteck (-12430,2260,4700,1600), Nutzlast `8a 4d 00 00 e4 a8 00 0c 00 05 06 00`
  -> ROOM1050 (19850,0,-22300) Cut 6; **slot 1 @0x0213A floor 6**, Rechteck x -7920..-3720, z -21440..-15940,
  Nutzlast `5c ae 00 00 a0 d7 00 04 00 10 05 00` -> **ROOM1100** (-20900,0,-10336), dir 1024, Cut 5, floor 0;
  Selbsttuer slot 3 @0x02352 (Szenen-Warp nach (1252,-1800,-2529), nur per `Aot_on 3` sub06 @0x02726).
* **Wege herein**: ROOM10F0/10F1 slot 1 @0x00F52 -> (-13600,-9000,1300) floor 5 = DACH (dort kommt man nach dem
  Klettern in 10F0 an, Spur A); ROOM1050/1051 slot 5 @0x00B7A -> (-10100,-1800,4200) floor 1 = HOF. **Kein Weg
  zurueck aus ROOM1100** (dessen Tueren @0x0099A -> ROOM10D0 und @0x009BA -> ROOM1110; Zensus aller STAGE1-RDT:
  nur 10F0/10F1, 1050/1051 und die Selbsttuer fuehren nach Raum 0x09).
* **Skript zu den Kisten**: genau EIN Record nennt eine Kiste. Aot_set slot 3 @0x0215A (1091: slot 2 @0x0215A),
  sce 3, sat 0x41 (0x40 Mitte-Test/Beruehrung + 0x01 Spieler, kein 0x10 = ohne Taste), **floor 5**, Rechteck
  x -15500..3572, z -17124..4465 = die ganze Dachflaeche, Nutzlast `ff 00 18 07` -> sub07 (1091: `18 02` -> sub02):
  `Aot_reset 3` @0x0272C, `Work_set 2e 03 00` @0x02736 (Objekt 0), `Member_set 34 00 94 d9` @0x0273A -> **Kiste 0
  x := -9836** (1091: @0x02232/@0x0223C/@0x02240). Keine Flagge, keine Bedingung: feuert bei jedem Betreten der
  Dachflaeche einmal (dann loescht es sich selbst). Sonst nichts: keine AOT auf Band 2 oder 6 ausser Tuer 1, keine
  Item-AOT (nItem 0, keine Item_aot_set), keine Flagge, keine Szene nutzt eine Kiste (sub02/sub03 = Frau im Schuppen
  und Rettung, sub06 = Feuerloescher, Nachrichten 0..9 — alles auf Band 1 im Hof).

### 2.2 Kiste 1 (Dach) = die Stufe ueber die Bruestung zum Tor nach ROOM1100 — gemessen

* Die Tuer 1 verlangt **Band 6** (Band-Gate des AOT-Scans FUN_80042cac: Spielerband DAT_800acad6 == rec[2],
  aot_common.c:1270); auf dem Dach ist der Spieler Band 5. Band 6 = y -10800 = die Oberkante der Kiste 1
  (Klettern Substate 9: Y -= 1800 @0x800381e4, Band += 1 @0x80038200-08). Die Kiste steht mit ihrer Nordkante
  (z -16382) an der Bruestung (-16403) und unter dem Suedrand des Tuerrechtecks (-15940).
* **exe, Lauf k1090_tor3** (JUMP 1090, Spieler (-5960,-12000) Blick -z = rot 1024, Skript U1.5 / A / W4 / U1.5 / A):
  climb.txt `T5050 pos=(-5960,-9000,-14132) band=5 sub=9 kind=0x81 obj=1` (Kletter-Sonde FUN_8002d474 findet
  Kiste 1; Halt davor = -14582 + 450) -> `T5081 pos=(-5960,-10800,-15280) band=6 stand=1` (oben, y -10800,
  Band 6) -> auf der Kiste vorgegangen bis `pos=(-5960,-10800,-15932)` -> Aktion `pad=8000` -> debug.log
  `[aot] DOOR FIRE slot=1 rect=(-5820,-18690,hw=2100,hh=2750) target_cut=5 spawn=(-20900,0,-10336)`,
  `[tuer] Sequenz Archiv 2 DOOR36 ...`, `[room] PC loaded room1100.rdt`, climb.txt `T5228 room=1100
  pos=(-20900,0,-10336) band=0`. (Lauf k1090_tor2 ohne den Schritt nach vorn: Aktion bei z -15280 -> Vorwaertspunkt
  620 @0x80042bd0 = -15900, 40 vor dem Rechteck -> noch kein Tor; der Schritt auf der Kiste bringt den Punkt hinein.)
* **Folge**: Vom Dach (Ankunft aus ROOM10F0) gibt es genau EINEN Weg weiter: auf die Kiste klettern und ueber die
  Bruestung durch das Tor nach ROOM1100 (einseitig, kein Rueckweg). Tuer 0 (floor 1) liegt im Hof und ist vom Dach
  nicht erreichbar; der einzige andere Ausgang ist der Rueckweg nach 10F0. **Die Kiste 1 ist also Pflicht-Requisite
  fuer den Fortgang.**
* Hinweis: die Kiste ist schiebbar (FUN_8002bd44 @0x8002bf14-0x8002bfe4) und seit Spur A (objfall_r36.c, Original
  FUN_8002c444) fallfaehig: Spur A zaehlte fuer obj 1 acht Absprungzellen auf dem Dach (erste (-9328,3376); Landung
  Band 3, y -5400 = Oberkante der Mauerzelle r1[18] @0x01F08 Byte 0x23). Wer sie vom Dach schiebt, verliert den Weg
  zum Tor (Original-Mechanik, kein Rueckweg) — Hinweis fuer den Nutzer, kein Fix.

### 2.3 Kiste 0 (Hof): in den Daten ohne Zweck

* Steht im Hof neben der Ankunft aus ROOM1050 (-10100,4200) und noerdlich der Tuer 0. Das Skript verschiebt sie beim
  Betreten der Dachflaeche um 1700 nach Westen (x -9836) — **gemessen exe** (Lauf k1090_tor2, JUMP-Spawn (-10200,-9000,1400)
  liegt im Rechteck): `[prop-member F6] prop[0] obj=0x00 m=0 -> val=-9836` sofort nach dem Laden. Eine Folge dieser
  Verschiebung gibt es nicht: kein Record haengt an ihrer Lage.
* Sie ist kletterbar (Band 1 -> 2, y -3600; oben Eingrenzung FUN_8002cfd4, Pin unit_1090_stay_on_crate) und schiebbar
  (Pin unit_1090_push_pin), erreicht aber nichts: auf Band 2 gibt es keine AOT, keine Flagge, keine Szene, keinen
  Gegenstand. Beleg: Bestand 2.1 (alle Records von 1090/1091 gelesen).
