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

(Abschnitt wird fortgeschrieben: Clip-Wahl am Bild, Umsetzung, Messung nachher.)

## Stand
- [x] Messung vorher (Szene fehlt)
- [ ] RE-Belege (Raum/Form/Bibliothek stehen; Clip-Wahl am Bild offen)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests
