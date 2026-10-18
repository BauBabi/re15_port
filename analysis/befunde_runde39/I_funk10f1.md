# Runde 39 — Spur I "funk10f1": Elza-Funkszene ROOM10F1 (Dossier, laufend)

Baum `.claude/worktrees/r39_funk10f1`, Zweig `r39/funk10f1`, Basis 1038dc15, Merge master 1600e580 (Welle 1+2, darin
Spur H: Walker-Sub-Init @0x800313a4/@0x80030b58/@0x80030d90). Start 2026-10-18; Fortsetzung ab 17:05 (Vorgaenger nur
d5c25ce7 "Dossier angelegt", alles Weitere hier neu gemessen).

## Auftrag (Wortlaut des Nutzers, AUFTRAG.md)
1. In Room 10F1 möchte ich, das wenn Elza ihn betritt eine Cutscene läuft wo sie Rechts zum Kommunikationssystem geht
   und von der Animation her drauf drückt und sagt "Hello, anyone, can you hear me? We Need help!"
2. Dann kommt "..."
3. Dann "Uhh, this Thing is broken!".
4. Suche die dir passenden Animationen dafür heraus.

Ressourcen (VERTRAG 1.1/1.2): Bank-9-Bit 70 (Szene gesehen), Port-Nachrichten-Ids 10F1 40-44.

## 1. Messung vorher (gebauter Stand 1600e580, configure+build `LOCAL-BUILD-OK`, exe 17:10)
Lauf v0 (Scratch `laeufe/v0`, Messkopie `re15_pc_r39i_mess_v0.exe`): `RE15_TITLE_SHOT_CHAR=1` (Elza) `RE15_GOTO_ROOM=10F1`
(Spawn = Tuer-Eintritt aus ROOM10D1, re15_room_spawns.h:41 `{ 8400, 0, -350, 2048, 0 }`), `RE15_STATE_LOG`, Bilder F60..F200.
- debug.log: `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[room] PC loaded room10f1.rdt`, `[rbj] room 10F1 has no
  RBJ`, Faeden nur Slot 0 (main00, first_op 0x3B) und Slot 1 (sub00, 0x06) — KEIN Szenenfaden.
- state.txt F9..F200: `PL(8400,-350,rot=2048)` unveraendert, `pm=0` (Spieler frei), `cam=0`, `msg(a=0 ...)` — keine Nachricht.
- Bilder (Cut 0): Elza steht an der Tuer, nichts geschieht. => **Die Szene existiert nicht** (alle vier Punkte offen).

## 2. RE-Belege
### 2.1 Raum (ROOM10F1.RDT == ROOM10F0.RDT, `cmp -l` 0 Unterschiede, 225092 B)
- Nachrichten @0x1168 (6 Stueck, Id 0..5; `tools/rdt_msgdump.py`): **msg 0 @0x1174 "A large communications device. It's out of
  commission. I can't send any messages from here."** — das Kommunikationssystem (und Elzas "broken" = "out of commission").
- main00 @0x00F32 (`tools/scd_dump_room.py`): **Aot_set Slot 2 @0x00F72 `2c 02 01 31 00 00 58 1b 28 0a 20 03 e4 25 00 00 ...`
  = Text msg 0, Rechteck x 7000..7800, z 2600..12300** (Ostwand); Door_aot_set Slot 0 @0x00F32 (Tuer -> ROOM10D0/10D1,
  Rechteck (8800,-1300,1000,2000)); Slot 1 @0x00F52 (floor 1, oben ueber den Tritt -> 1090/1091).
- Eintritt (der einzige Weg HINEIN fuer Elza: ROOM1091 hat keine Tuer nach 0x0F, Door_aot_set nur Slot 0 -> 0x05 und Slot 1
  -> 0x10): ROOM10D1 main00 @0x01174 Door_aot_set Slot 0 `... d0 20 00 00 a2 fe 00 08 00 0f 00 ...` = Spawn (8400,0,-350),
  Gierung 2048 (Blick -X = nach Westen in den Raum), Cut 0.
- Kollision (SCA @0x678, 35 Zellen, `tools/r39_funk10f1/grundriss_10f1.py`): Ostwand/Konsolenfuss Zelle 10 x 7400..9300,
  z 2500..14750 (Typ 1); davor drei Konsolen-Tische Typ 3: Zelle 13 x 6800..8300 z 3800..5300, Zelle 12 x 6600..8100
  z 7500..9000, Zelle 14 x 6700..8200 z 10300..11800. Projektion in Cut 1 (`tools/r39_treffer_zellen_bild.py`): Zelle 13 =
  der vorspringende Tisch mit Tastatur und Telefon.
- **"Rechts"**: Elza tritt bei (8400,-350) mit Gierung 2048 ein (Blick -X). Mit y nach unten (PSX) ist ihre rechte Seite
  f x oben = (-1,0,0) x (0,-1,0) = (0,0,+1) = **+Z** — das Kommunikationssystem (z ab 2600) liegt rechts von ihr.
- Kameras (RDT @0x60, 32 B je Cut): Cut 0 (6535,-3754,6105) Blick nach Sueden auf die Tuer (Konsole links im Vordergrund);
  Cut 1 (5806,-3627,10200) Blick nach Sueden: die Konsole links (Ostwand) mit Monitoren, Tastatur, Telefon; Cut 2
  (5874,-3618,1851) Blick nach Norden. RVD: 0->1 bei z 2900..3900, 1->2 bei z 7000..8000 (K_cut10f0.md §1.1).
  Bild Elza bei (6300,4550) Gierung 0, Cut 0/1/2 (Scratch `pos_blatt.png`): Cut 0 = Elza zu nah (nur Kopf), **Cut 1 = Elza
  ganz im Bild vor der Konsole**, Cut 2 = Elza angeschnitten im Vordergrund.

### 2.2 Original-Form einer Elza-Szene: ROOM1031 sub13 @0x02982..@0x02A58 (Elzas erste Szene, ausgeliefert)
```
@0x02986 22 02 07 01  Set(2,7)=1        @0x0298A 22 01 1b 01  Set(1,27)=1      @0x0298E 29 04  Cut_chg 4
@0x02990 2e 01 00 00  Work_set(1,0)     @0x029A4 3f 00 0b 00  Plc_motion(0,11) @0x029A8 43 00 04 00  Plc_flg 4 (halten)
@0x029AC 09 0a 14 00  Sleep 20          @0x029B0 41 01 ..     Plc_neck 1        @0x029BA 09 0a 14 00  Sleep 20
@0x029BE 3f 00 00 00  Plc_motion(0,0)   @0x029C2 43 00 80 00  Plc_flg 0x80 (rueckwaerts = Arm senken)  @0x029C6 Sleep 30
@0x029CA 29 06 Cut_chg 6  @0x029CC 40 00 09 20 77 dc a1 aa Plc_dest Modus 9 (Bit 0x20) + @0x029D4 18 0e Gosub sub14 (Warten)
@0x029D6 40 00 04 20 ..  Plc_dest Modus 4 (gehen) + Gosub 14   @0x029E0 Plc_dest Modus 9 + Gosub 14
@0x02A08 2b 12 00 00  Message_on 18 "Elza: That was close, ..."  @0x02A0C 3f 00 0a 00 + 43 00 04 00 + Sleep 100
@0x02A24 2b 13 00 00  Message_on 19 "Elza: Hey, is anyone here?!"  ... @0x02A3A 41 04 03 00 00 00 00 00 3c 00 Plc_neck 4
                      (3 Schwuenge = sich umsehen, keine Antwort) @0x02A44 Sleep 100
@0x02A48 3c 01 Cut_auto 1  @0x02A4A Set(2,7)=0  @0x02A4E Set(1,27)=0  @0x02A52 Work_set(1,0)  @0x02A56 42 00 Plc_ret  @0x02A58 01 00
sub14 @0x02A5A 11 00 08 00 / 02 00 / 12 04 / 21 05 20 00 / 01 00  (Do / Evt_next / Edwhile / Ck(5,32)==0)
```
Nachrichtenform Elza (ROOM1031 msg 19 @0x2FA7): `04 00 05 01 21 48 56 3d 16 05 00 00` "Elza:" ... `1b 1a` "?!" `04 01 01 63`.

### 2.3 Elzas Gestenbibliothek (Record 0 = Spieler, Marker Bit 0, Binder FUN_8001b3f8)
Zensus `tools/r34n_d/rbj_zensus.py liste` ueber alle ROOMxxx1: RBJ nur in 1011, 1021, 1031, 1041 (Marker 0x7E, kein
Spieler), 10B1, 1141, 1151, 1171, 11B1, 1211; ROOM10F1 hat keinen (RDT+0x5C = 0). Inhaltsvergleich (Keyframe-Folge je Clip,
`tools/r39_funk10f1/elza_gesten.py`, Elza = PL04-Basis): eindeutige Clips = ROOM1031 c0..c12, ROOM1141 c25, ROOM1151
c6..c19 (alle uebrigen sind Kopien davon; keiner gleicht einem Leon-Clip aus 1150/11B0/1170/1050/1090/11C0/10D0).
Handbahn je Bild (`elza_gesten.py bahn`, Modellraum, Hoehe ueber der Sohle):
| Clip | Bilder | Hand am weitesten vorn | Verwendung im Original (Spieler) |
|---|---|---|---|
| 1031 c0 | 20 | L 739 vorn / 2015 hoch (Arm waagerecht nach vorn), Bild 15 | 1031 sub13 @0x029BE rueckwaerts; 1021/10B1/1211 Dialoge |
| 1031 c11 | 2 | L 712 / 1964 (Haltepose "Arm vorn") | 1031 sub13 @0x029A4 + Plc_flg 4 (halten) |
| 1031 c12 | 50 | L 456 / 2228 (Hand zum Kopf) | 1031 sub13 @0x02A00 |
| 1141 c25 | 28 | vorgebeugt, beide Haende vorn ~1583 | nicht benutzt |
| 1151 c17 | 40 | L 438 / 1651 (vorgebeugt, Hand vorn-unten auf Tischhoehe), Bild 19 | nicht benutzt |

### 2.4 Was Elza in ROOM1031 sub13 mit Clip 11/0 tut: sie DRUECKT den Rolltor-Notschalter
- Standort der Szene: Member_set @0x02994 `34 00 d7 cc` (x -13097), @0x02998 `34 02 a1 aa` (z -21855), @0x0299C `34 04 00 04`
  (Gierung 1024 = Blick -Z). Direkt vor ihr (z <= -21922) liegt Aot_set Slot 7 @0x01D88 `2c 07 01 31 00 00 4a c8 6c 9d 72 05
  f2 0c 02 00 ...` = Text msg 2 "The emergency shutters have already been activated." (x -14262..-12868, z -25236..-21922).
  Wandzelle davor: SCA-Zelle 30 x -15518..-12566, z -24134..-22694 (Abstand Elza -> Wandflaeche 839).
- Messung am gebauten Stand (Lauf `e1031b`: Neues Spiel Elza, `RE15_PSELECT_AUTO(_SWITCH)`, Bilder F440..F545/15, Scratch
  `e1031_blatt.png`, angesehen): F455..F485 Cut 4 — Elza streckt den Arm nach vorn auf den Schaltkasten an der Wand und haelt
  ihn (Clip 11 + Plc_flg 4), F500..F515 Arm sinkt (Clip 0 rueckwaerts), F530 Cut 6: das Rolltor ist unten, Zombies dahinter.
  => Clip 11 (+ halten) / Clip 0 rueckwaerts ist Elzas ORIGINALE "auf ein Geraet druecken"-Geste.
- Clip 12 (50 Bilder) = Hand an die Stirn (Streifen `s1031_c12.png`: Bild 12..36 Hand an der Stirn) — im Original direkt nach
  dem Rolltor-Druck @0x02A00 vor "That was close ...". Clip 9 (24 Bilder) = Hand an die Huefte (`s1031_c9.png`), im Original
  @0x02A32 vor dem Umsehen Plc_neck 4 nach "Hey, is anyone here?!" (niemand antwortet).

### 2.5 Clip-Wahl (Nutzer Punkt 4: "Suche die dir passenden Animationen dafür heraus") — Begruendung
| Moment | Wahl | Warum (Beleg) | verworfen |
|---|---|---|---|
| zur Konsole gehen | Plc_dest Modus 9/4/9 (Lauf-Clip der Spieler-Walk-Subs, Spur-H-Init @0x80030b58/@0x800313a4) | Elzas eigener Gang in 1031 sub13 @0x029CC..@0x029E0 | — |
| "drauf drueckt" | Clip 11 + Plc_flg 4 (halten), danach Clip 0 + Plc_flg 0x80 | ihr Druck auf den Rolltor-Notschalter (§2.4) — dieselbe Handlung "Taste an einem Geraet druecken" | 1151 c17 (vorgebeugt, Hand vorn-unten, unbenutzt, zweite Haelfte Zurueckweichen), 1141 c25 (vorgebeugt-statisch, unbenutzt), 1031 c0 vorwaerts allein (Zeigegeste der Dialoge) |
| Zeile 1 sprechen | waehrend des Haltens (Message_on 20 Bilder nach dem Griff) | Sprechtaste gedrueckt halten, Zeilentakt Sleep 100 wie @0x02A14 | erst loslassen, dann sprechen |
| "..." | Clip 9 (Hand an die Huefte) + Plc_neck Modus 4 (3 Schwuenge) | Elzas Original-Reaktion auf "keine Antwort" nach "Hey, is anyone here?!" @0x02A32..@0x02A44 | Plc_neck 2+4 (Kopfschuetteln Leon/Ada) |
| "Uhh, this Thing is broken!" | Clip 12 (Hand an die Stirn) | Elzas "Uff"-Geste @0x02A00 | Clip 4 (Arm seitlich), Clip 6 (Arm hoch) |
Gestenquelle: Record 0 von ROOM1031 (13 Clips); ROOM10F1 hat keinen Block -> Leihe (Muster Runde 35 K ROOM10F0 <- ROOM11B0).

## 3. Umsetzung (Dateien, Konstanten)
- NEU `re15_port/include/re15_funk10f1_r39.h` (Konstanten mit Beleg/Kennzeichnung), `re15_port/engine/src/funk10f1_r39.c`
  (Installer, Ereignis-Weiche, Nachrichten, Leih-Quelle, Pruefhaken), `re15_port/engine/src/gen/funk10f1_r39_szene.inc`
  (Programm 208 B / 53 Opcodes + msg 40/41/42; ERZEUGT von `re15_port/tools/r39_funk10f1/szene_bauen.py`, der jede Vorlage gegen
  die ROOM1031-Bytes prueft und die Zeilenbreite gegen das Korpus-Budget 285 px misst: msg 40 = 250/93 px, msg 41 = 46 px,
  msg 42 = 210 px). Werkzeuge `tools/r39_funk10f1/elza_gesten.py` (Elza-Posenstreifen/-Katalog/-Handbahn, PL04-Basis),
  `tools/r39_funk10f1/grundriss_10f1.py` (Draufsicht Zellen/Zonen/Kameras).
- Haken (je 1-2 Zeilen, Kommentar "Runde 39 Spur I"): `engine/src/scd_room_setup.c` Installer am ENDE des Blocks;
  `engine/src/scd_vm.c` scd_event_fire (`re15_funk10f1_r39_ereignis`, nach Runde 38 F); `platform/pc/src/cut10f0_pc.c`
  Leih-Quelle (`if (!quelle) quelle = re15_funk10f1_r39_rbj_quelle(room_id)`) + Logzeile; `tests/unit/test_r36_kartenbits.c`
  k_port[] Bit 70.
- KEIN Haken im Boot-/CONTINUE-Weg (platform/pc/main.c): ROOM10F1 hat keinen Speicherpunkt (re15_savepoint.c: 1070/1120/1150/
  ...), ein Spielstand IN 10F1 entsteht im Spiel nicht; der einzige Weg hinein ist die Tuer aus ROOM10D1 (§2.1). Ein Start
  der Gehszene von einer beliebigen Ladeposition koennte an Zellen haengen bleiben (Pad gesperrt) — deshalb bewusst nur
  ueber scd_room_reenter (Tuer, Debug-Sprung, GOTO).
- Konstanten: Bit (9,70) VERTRAG 1.1; Ids 40/41/42 VERTRAG 1.2; Ereignis 27 (frei: belegt 2..6, 13, 20..26, 28, 29);
  Ankunftsbit 0x20 = ROOM1031 @0x029CC/@0x02A62; Clips 11/0/9/12 = ROOM1031 rec0 (§2.3/2.4); Kamera Cut 1 (§2.1, Bild).
  PORT-WAHL (Lage, belegt aus Kollision/Zone/Bild): Weg (6250,2900) -> (6250,5900) -> Platz (6900,6400), Blick (7400,6400).

### 3.1 Platz und Weg (PORT-WAHL, Form belegt)
- "Rechts": Eintritt Gierung 2048 (-X) -> rechts = +Z (§2.1). Der erste Wegpunkt liegt bei z 2900 (+Z), die erste Gehgierung
  ist 2693 (gemessen, §4) = zwischen 2048 und 3072.
- Spielerradius gegen Zellen GEMESSEN: Teleport nach (6950,3150) -> Spieler steht bei (6932,3150) (Zelle 10 Kante x 7400 ->
  Radius 468); (6350,4550) -> (6343,4550) (Zelle 13 Kante 6800 -> 457). Weg mit > 468 Abstand zu jeder Zelle (Riegel misst
  das an 3 x 101 Punkten).
- Platz (6900,6400): zwischen den Konsolentischen 13 (z <= 5300) und 12 (z >= 7500) direkt vor dem Konsolenfuss (x 7400,
  500 Abstand); Pruefpunkt 620 vor dem Spieler (7520,6400) liegt in Slot 2 msg 0 (x 7000..7800) = "das Kommunikationssystem".
  Bildwahl (Scratch `st_blatt.png`, Cut 1): (6932,6400) = Elza an der Konsole mit Telefon/Tastatur/Monitoren, gut im Bild;
  (6932,3150) zu klein an der Tuerseite; (6343,4550) vor dem Tisch statt an der Konsole; (6750,9900) ausserhalb des Bildes.
- Kamera: Cut 0 beim Eintritt (Tuer), waehrend des Gehens Cut_auto 1 (RVD 0->1 bei z 2900..3900 -> Cut 1), am Platz Cut_chg 1
  (Cut 1 sieht die Konsole von Norden; Elzas linke Seite mit dem Druck-Arm zur Kamera).

## 4. Messung nachher (gebauter Stand fe2c1935, configure + build `LOCAL-BUILD-OK`)
### 4.1 Lauf n1 (Elza, `RE15_GOTO_ROOM=10F1`, STATE_LOG + ANIM_TRACE + MSG_LOG, Bilder F0..F720/10; Scratch `laeufe/n1`)
- debug.log: `[funk10f1] F30 ROOM10F1: Funkszene gestartet (Ereignis 27, Faden 10, Flag (9,70)=0)`, `[rbj] Animationsblock von
  ROOM1031 geliehen (26760 B, Runde 39 Spur I)`, `[rbj] room 10F1 cinematic overlay: 13 clips, 316 kf`; Plc_dest (6250,2900)
  Modus 9 -> 4, (6250,5900) 4, (6900,6400) 4, (7400,6400) 9; `[scd F127] Cut_chg(1)`; Plc_motion 11 + Plc_flg 0x0004;
  `[msgtext] room=10f1 id=40 "Elza: Hello, anyone, can you hear me? We Need help!"`; F247 Plc_motion 0 + Plc_flg 0x0080; id=41
  "Elza: ..." + Plc_motion 9; id=42 "Elza: Uhh, this Thing is broken!" + Plc_motion 12.
- state.txt: Gang ab dem ersten Bild mit rot=2693 (rechts: zwischen 2048 und 3072), (7854,456) -> (6292,2831) F66, Kamera
  Cut 0 -> Cut 1 ab F74 (RVD), (6250,5827) F106, Platz (6862,6368) F122, Drehung auf rot=4057 (Osten); F130..F247 mo=11 bei
  (6862,6368) cam=1, msg 40 aktiv F~150..F248 (Druck gehalten WAEHREND der Zeile); F250 mo=0 (Senken); F282..F377 mo=9 msg 41;
  F386..F481 mo=12 msg 42; F482 mo=200 (Ruhe) pm=1, ab F498 pm=0 (Spieler frei), Kamera bleibt Cut 1.
- ANIM_TRACE Spieler (Bild: Motion Clip Bilder ... rueckwaerts): F0 210/1 (Ankunftspose) -> F6 105/5 (Gang, mit Drehung) ->
  ... -> F127 11/11 fc 2 -> F247 0/0 fc 20 cur 19 rueckwaerts=1 -> F277 9/9 fc 24 -> F382 12/12 fc 50 -> F482 200 (Ruhe).
- Bilder (angesehen, Scratch `n1_a.png`/`n1_b.png`/`n1_zoom.png`/`n1_balken.png`): F30 Elza an der Tuer dreht sich nach rechts
  (zur Kamera), F50 geht, F70..F110 Cut 1 den Gang entlang, F130/F140 linker Arm vorn auf dem Bedienfeld der Konsole (unter
  den Monitoren), F160/F200 Untertitel zweizeilig "Elza: Hello, anyone, can you hear me? / We Need help!" bei gehaltener Hand,
  F250 Arm sinkt, F300/F340 "Elza: ..." Hand an der Huefte, Kopf dreht (Umsehen), F400/F420 "Elza: Uhh, this Thing is broken!"
  Hand an der Stirn, F480 Ende; Balken oben/unten waehrend der Szene (F200), danach keine (F500).

### 4.2 Lauf echt2 = ECHTER WEG durch die Tuer + zweites Betreten (Scratch `laeufe/echt2`)
`RE15_TITLE_SHOT_CHAR=1 RE15_GOTO_ROOM=10D1 RE15_SET_FLAG=3:50 RE15_FIRE_AOT=0@40#10d1 RE15_DEBUG_JUMP=10f1@620
RE15_EXIT_AT=700#10f1 RE15_AUDIO_CAP_SYNC=cap.raw`, Stellvertreter `synchro/STAGE1/room10F1/main40.wav` (= room1031/main19.wav)
im Laufordner. (Ohne (3,50) ist die Tuer zu: `[msgtext] room=10d1 id=6 "Communication Room"It's electronically locked..."` —
ROOM10D1 main00 @0x01026 Ck(3,0x32)==0 legt dort einen Text-Platz statt der Tuer; (3,50) = Kartenleser benutzt.)
- `[fire-aot] slot=0 at F40 (Raum 10D1)` -> `[tuer] Sequenz Archiv 2 DOOR13 ...` -> `[room] PC loaded room10f1.rdt` ->
  `[funk10f1] F40 ROOM10F1: Funkszene gestartet` -> Leihe ROOM1031 -> msg 40 + `[voice] clip loaded: main40.wav (32000 Hz x1 ->
  111104 @44100 Hz)` -> msg 41 + `[voice] ROOM10F1 msg 41: keine Aufnahme (synchro/STAGE1/room10f1/main41.wav) - Zeile bleibt
  stumm` -> msg 42 + dito main42.wav. => Stimme je Zeile angefordert; liegt die Datei da, spielt sie.
- `[debug-menu] AUTO-JUMP -> ROOM10F1 (Frame 620)` (Raum neu aufgebaut, (9,70)=1): danach 0 Zeilen `Plc_dest|Message_on|funk10f1`,
  state.txt F1..F700 (8400,-350) mo=211 pm=0 msg(a=0) — Bilder `echt2_wieder.png`: Elza steht an der Tuer, keine Balken.

### 4.3 Lauf echt3 = echter Weg mit Bildern + Ton-Mitschnitt (Scratch `laeufe/echt3`, `RE15_AUDIO_CAP_SYNC=cap.raw`)
- Bilder nach der Tuer (Scratch `echt3_anfang.png`, angesehen): F0 schwarz (Einblendung nach der Tuersequenz), F10 Elza an der
  Tuer (Cut 0), Balken fahren ein, F20..F30 sie dreht sich nach IHRER RECHTEN (zur Kamera), F40..F60 geht auf die Konsole im
  linken Vordergrund zu, F80 Cut 1 den Gang entlang.
- Ton: normierte Kreuzkorrelation der Stellvertreter-Datei (main40.wav, 32 kHz -> 44,1 kHz) gegen den Mitschnitt: Maximum
  0,698 bei Mitschnitt-Bild 380,00 (naechstbester Wert ausserhalb +-3 Bilder 0,095), Verstaerkung 1,003; Beginn 373 Bilder vor
  dem Mitschnitt-Ende (= 10F1-Bild 520) -> Bild 147 = erste state-Zeile mit msg id=40 (F147). Die Stimme spielt GENAU mit
  der Zeile 40, ueber die Raummusik gemischt.

## 5. Tests (probes/r39_funk10f1.cmake, nur dort registriert)
| Test | misst | Ergebnis |
|---|---|---|
| unit_r39_funk10f1_form | Laengen schliessen auf Evt_end; Set(9,70) zuerst; Rahmen/Warteschleife/Druck/Senken/Umsehen/Stirn/Schwanz BYTEGLEICH mit ROOM1031 sub13/sub14 (Datei gelesen); erster Wegpunkt rechts (+Z); Pruefpunkt (7520,6400) in Slot 2 msg 0; Weg 3 x 101 Punkte: kleinster Zellabstand 500 (Zelle 10) > 468 | gruen |
| unit_r39_funk10f1_texte | msg 40/41/42 = Nutzer-Wortlaut; Kopf/Ende = ROOM1031 msg 19 @0x2FA7/@0x2FC8; nur Ids 40..42 | gruen |
| unit_r39_funk10f1_szene | echte VM: (9,70) ab Bild 0; erste Gehgierung 2691 (rechts); msg 40 -> 41 -> 42; SCD_AUDIO_VOICE_ON 40, 41, 42; Motions 105 ... 11 0 9 12 200; 99 Bilder Druck gehalten WAEHREND msg 40 bei (6862,6368) Gierung 4057 Cut 1; 30 Bilder Clip 0 rueckwaerts; Balken 476 Bilder, Ende Bild 476, frei Bild 490 | gruen |
| unit_r39_funk10f1_einmal | zweiter Raumaufbau: keine Szene/Nachricht/Stimme, frei; keine Leihe, Ereignis 27 -> NULL; Leon ROOM10F0: nie, (9,70) bleibt 0 | gruen |
| integration_r39_funk10f1_echt | echte exe, Elza, Kartenleser-Tuer ROOM10D1 -> RE2-Tuersequenz -> ROOM10F1: Szene genau 1x, Leihe ROOM1031, Plc_dest-Folge, msg 40/41/42 woertlich, Stimme 40 gespielt (Stellvertreter main40.wav im Laufordner), 41/42 angefordert, ANIM_TRACE 11:11:0 -> 0:0:1 -> 9:9:0 -> 12:12:0, Gehgierung 2691, 99 Bilder Druck bei (6862,6368) rot 4057 Cut 1, danach pm=0; zweites Betreten (Raum neu aufgebaut): nichts | gruen (67,9 s) |
| integration_r39_funk10f1_zweit | echte Tuer mit (9,70)=1: keine Szene, keine Leihe ("room 10F1 has no RBJ"), Spieler frei | gruen (18,5 s) |
| integration_r39_funk10f1_leon | Leon ROOM10F0 ((9,71)=1): keine Funkszene, keine Leihe von ROOM1031 | gruen (8,6 s) |
| unit_r36_kartenbits | k_port[] + Bit 70 (im Original frei) | gruen |

## Stand
- [x] Messung vorher (Szene fehlt)
- [ ] RE-Belege (Raum/Form/Bibliothek stehen; Clip-Wahl am Bild offen)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests
