# Fahrstuhl: 1F -> 2F/3F bleibt 1F — Mechanismus vollstaendig belegt, Port-Kette GEMESSEN korrekt, Ursache NICHT reproduziert

Stand 2026-09-26. Kurzurteil: Der Original-Mechanismus ist byte-genau geklaert (Raum, Tueren,
Knopf-Zellen, Etagen-Flags, Fahrt-Subs). Die Kette im Port loest auf Engine-Ebene **alle drei
Etagen richtig auf** — das ist heute gemessen, nicht argumentiert. Damit liegt die Ursache
NICHT in der SCD-Kette, sondern in einer Schicht darueber, und die habe ich in diesem Lauf
NICHT eingegrenzt. §6 sagt genau, was fehlt und wie es zu messen ist. Keine Zahl geraten.

---

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Der Fahrstuhl funktioniert noch nicht. bin ich auf 1F und druecke 2F
oder 3F und Leon steigt aus, bleibt er immer bei 1F."

Eigene Messung (heute, mit der bereits gebauten Sonde
`.build_diag1d3/tests/unit/probe_elevator_1080.exe`, Stand 2026-09-14, NICHT neu gebaut —
diese Runde darf nicht bauen):

```
-- nach main00/sub00: Tuer-AOTs --
   slot0 type=1 sceflg=0x31 rect c=(0,0) half=(0,0) dest=st0 rm0x04 cut=5 spawn=(-21936,0,-11000)
   slot1 type=1 sceflg=0x31 rect c=(0,0) half=(0,0) dest=st0 rm0x0C cut=0 spawn=(1450,0,7300)
   slot2 type=1 sceflg=0x31 rect c=(0,0) half=(0,0) dest=st0 rm0x12 cut=0 spawn=(1300,0,7300)
-- Knopf-AOTs --
   slot3 active=1 type=0 ev=6 sceflg=0x31 band=0x00 c=(-12050,-50)  half=(400,400)
   slot4 active=1 type=7 ev=4 sceflg=0x44 band=0x00 c=(-19575,21225) half=(1025,1025)
   slot5 active=1 type=7 ev=5 sceflg=0x44 band=0x00 c=(-19575,24075) half=(1025,1025)
   slot6 active=1 type=7 ev=6 sceflg=0x44 band=0x00 c=(-19575,27025) half=(1025,1025)
   slot7 active=1 type=7 ev=7 sceflg=0x44 band=0x00 c=(-18375,16825) half=(1025,1025)
-- Cursor-Prop idx=0 pos=(-19554,22684) band=0 --
== Fahrt: Knopf 1F ==  Cursor start notch=0 -> nach 4x DOWN (-19554,21884) notch=4
   ROOM-CHANGE nach 161 Ticks: -> 0x1040 spawn=(-21936,0,-11000) cut=5
== Fahrt: Knopf 2F ==  nach 4x UP (-19554,23484) notch=5
   ROOM-CHANGE nach 161 Ticks: -> 0x10C0 spawn=(1450,0,7300) cut=0
== Fahrt: Knopf 3F ==  nach 17x UP (-19554,26084) notch=6
   ROOM-CHANGE nach 161 Ticks: -> 0x1120 spawn=(1300,0,7300) cut=0
   1F: Soll 0x1040 Ist 0x1040 OK | 2F: Soll 0x10C0 Ist 0x10C0 OK | 3F: Soll 0x1120 Ist 0x1120 OK
== Knopf TUER (slot7) mit Etagen-Flag 2F (z3 b55) ==  notch=7 -> 0x10C0 OK, z3 b55 danach=0
===== BEFUND: OK =====
```

Das ist der ECHTE Engine-Pfad (`scd_vm_tick` + `re15_aot_scan` + `re15_actor_step_all_walkers`
+ `re15_aot_stamp_entities` + `re15_object_notch_update`, Pad ueber
`re15_pad_virtual_word`), nur ohne `re15_game_step` und ohne den echten Spieler-Controller.

---

## 2. Das Original — mit Adressen und Bytes

### 2.1 Der Fahrstuhl IST ROOM1080 (Kabine); Gegenstellen 1040 / 10C0 / 1120

Debug-JUMP-Tabelle (aus DEBUG.BIN erzeugt, `re15_port/include/debug_jump_table.h:22`):
Stage-1-Slot 8 = "ELEVATOR" -> ROOM1080. Slot 4 = "WEST CORRIDOR" (1F, ROOM1040),
Slot 12 = "2F WEST SIDE" (ROOM10C0), Slot 18 = "3F WEST SIDE" (ROOM1120).

`re15_port/shared_assets/PSX/STAGE1/ROOM1080.RDT`, main00 (Datei 0x482..0x4E4) —
DREI `Door_aot_set` (0x3B), alle mit **Null-Rect** (Breite = Tiefe = 0), also NICHT
begehbar, sondern nur per `Aot_on` zu feuern:

```
@Datei 0x0482  3b 00 02 31 00 00 00 00 00 00 00 00 00 00 | 50 aa 00 00 08 d5 | 00 04 | 00 04 05 00 ...
               slot 0  sce2  Rect 0/0/0/0      Spawn (-21936,0,-11000) Yaw 0x0400  Stage 0 Room 0x04 Cut 5
@Datei 0x04A2  3b 01 02 31 00 00 00 00 00 00 00 00 00 00 | aa 05 00 00 84 1c | 00 0c | 00 0c 00 00 ...
               slot 1  sce2  Rect 0/0/0/0      Spawn (  1450,0,  7300) Yaw 0x0C00  Stage 0 Room 0x0C Cut 0
@Datei 0x04C2  3b 02 02 31 00 00 00 00 00 00 00 00 00 00 | 14 05 00 00 84 1c | 00 0c | 00 12 00 00 ...
               slot 2  sce2  Rect 0/0/0/0      Spawn (  1300,0,  7300) Yaw 0x0C00  Stage 0 Room 0x12 Cut 0
@Datei 0x04E2  01 00   Evt_end
```

Payload-Felder belegt ueber den Warp `FUN_8001d600` (Door-Zweig @0x8001d82c liest
a0 = [DAT_800ac9a8]): `lh 0=X @0x8001d87c`, `lh 2=Y @0x8001d89c`, `lh 4=Z @0x8001d8bc`,
`lhu 6=Yaw @0x8001d8dc`, `lbu 8=STAGE @0x8001d960`, `lbu 9=ROOM @0x8001d94c`,
`lbu 10=CUT @0x8001d930`.

=> **Tuer 0 = 1F (ROOM1040), Tuer 1 = 2F (ROOM10C0), Tuer 2 = 3F (ROOM1120).**

Gegenprobe ROOM10C0 (2F), main00 @Datei 0x0E82:
`3b 01 02 31 00 00 f4 01 0c 17 d0 07 e8 03 | ae ca 00 00 7c fc | 00 04 | 00 08 00 ...`
= Rect (500,5900) 2000x1000, Spawn (-13650,0,-900) Yaw 0x0400, Stage 0 **Room 0x08 = ROOM1080**.
Das ist der Eintritts-Spawn in der Kabine: **(-13650, 0, -900)**.

### 2.2 Das Etagen-Menue ist ein CURSOR-Raetsel — dieselbe Maschine wie ROOM11F0

`ROOM1080.RDT` sub00 (Datei 0x4FA..0x582):

```
@Datei 0x04FA  2d 00 04 00 00 00 00 01 00 00 9e b3 00 00 9c 58 ...
               Obj_model_set  obj 0  type 4  band 0  Pos (-19554, 0, 22684)   <- DER CURSOR
@Datei 0x051C  2c 03 03 31 00 00 5e cf 3e fe 20 03 20 03 | ff 00 18 06 | 00 00
               Aot_set slot 3  sce 3  flags 0x31  Rect (-12450,-450) 800x800  Daten ff 00 18 06
@Datei 0x0530  2c 04 05 44 00 00 88 af e8 4e 02 08 02 08 00 00 00 00 00 00  Zelle 4 Rect (-20600,20200) 2050x2050
@Datei 0x0544  2c 05 05 44 00 00 88 af 0a 5a 02 08 02 08 ...                Zelle 5 Rect (-20600,23050) 2050x2050
@Datei 0x0558  2c 06 05 44 00 00 88 af 90 65 02 08 02 08 ...                Zelle 6 Rect (-20600,26000) 2050x2050
@Datei 0x056C  2c 07 05 44 00 00 38 b4 b8 3d 02 08 02 08 ...                Zelle 7 Rect (-19400,15800) 2050x2050
```

Das Rect-Format ist **Ecke + Groesse**, nicht Mitte + Halbmass — daher die Zentren
(-19575,21225) / (-19575,24075) / (-19575,27025) / (-18375,16825) mit Halbmass 1025
(so rechnet der Port, und so misst die Sonde oben).

`sce_flags = 0x44` = Pool-Maske 0x04 (**OBJEKT-Pool**) + 0x40 (CENTRE-Test), Bit 0x10 NICHT
gesetzt => AUTO-Pfad. Die Bit-Bedeutungen sind im Original disassembliert
(zitiert in `re15_port/engine/src/aot_common.c:735-745`, jede Zeile eine Instruktion):
`@0x80042c8c and v0,v0,a3` (Pool-Maske 1=Spieler / 2=Gegner / **4=Objekt**),
`@0x80042ca4 bne v0,s6` (nur AUTO), `@0x80042ea0 lbu v1,1(s0)` (0x40 CENTRE / 0x20 FORWARD),
`@0x80042fc4 sb v0,11(s1)` (Stempel auf Entitaet **+0x0B**), `@0x8004301c bne` (LAST-WINS),
Objekt-Clear `@0x80043788 sb zero,0(at)`.

=> Die vier Zellen stempeln das **Cursor-Objekt** (`Obj_model_set obj 0`) auf +0x0B.

**Panel-Ausloeser selbst nachdisassembliert** — SCE-Tabelle und Handler 3:

```
re15_disasm.py table 0x8007469c -> [3] 0x800746a8 -> 0x800430f0
@0x800430f0  addiu sp,sp,-24
@0x800430f4  addu  v0,a0,zero
@0x800430fc  lhu   a0,0(v0)      ; a0 = u16 @payload+0  = 0x00ff  (Bedingungsbyte)
@0x80043100  lbu   a1,3(v0)      ; a1 = u8  @payload+3  = 0x06    (Sub-Nummer)
@0x80043104  jal   0x8003ee3c    ; = der Evt_exec-Slotvergeber FUN_8003ee3c(cond, sub)
```
Die Aot_set-Nutzlast von Slot 3 ist `ff 00 18 06` => cond 0x00ff, **sub 6**.
Also: Aktion am Panel (Slot 3, Flags 0x31 = Spieler-Pool + ACTION + FORWARD-620) -> sub06.

### 2.3 sub06 / sub01 — Panel einschalten und je Bild abfragen

sub06 (Datei 0x6C6..0x6EE):
```
@0x06C6 29 01           Cut_chg 1                    ; Panel-Kamera
@0x06C8..0x06E8 22 05 0x 01  Set(5, 0..8, 1)          ; Steuer-Modus AN (neun Bits)
@0x06EC 01 00           Evt_end
```

sub01 (Datei 0x582..0x696) — der Per-Bild-Thread:
```
@0x0582 06 00 14 00   Ifel_ck      @0x0586 21 05 00 01  Ck(5,0,1)
@0x058A 06 00 0a 00   Ifel_ck      @0x058E 51 01 01 00  Sce_key_ck(1,0x0001)  -> @0x0592 04 ff 18 02  sub02 (+Z)
@0x059A/059E/05A6/05AA  Ck(5,1,1) + Sce_key_ck(1,0x0004) -> sub03  (-Z)
@0x05B2/05B6/05BE/05C2  Ck(5,2,1) + Sce_key_ck(1,0x0002) -> sub04  (+X)
@0x05CA/05CE/05D6/05DA  Ck(5,3,1) + Sce_key_ck(1,0x0008) -> sub05  (-X)

@0x05E2 06 00 24 00        Ifel_ck
@0x05E6 21 05 04 01        Ck(5,4,1)
@0x05EA 2e 03 00           Work_set(kind 3, idx 0)        <- Arbeits-Entitaet = CURSOR-OBJEKT
@0x05EE 06 00 16 00        Ifel_ck
@0x05F2 3e 00 0f 00 04 00  Member_cmp(member 15 == 4)     <- Cursor ueber Zelle 4?
@0x05F8 06 00 0a 00        Ifel_ck
@0x05FC 51 01 40 00        Sce_key_ck(1,0x0040)           <- BESTAETIGEN (virt. Bit 6)
@0x0600 04 ff 18 07        Evt_exec sub07                 <- 1F
@0x060A..0x0630   Ck(5,5,1) / member15==5 -> sub08   <- 2F
@0x0632..0x0658   Ck(5,6,1) / member15==6 -> sub09   <- 3F
@0x065A..0x0680   Ck(5,7,1) / member15==7 -> sub10   <- TUER AUF (aktuelle Etage)
@0x0682 06 00 0e 00 / 21 05 08 01 / 22 02 00 01 / 22 02 02 01
                  Ck(5,8,1) -> Set(2,0,1) + Set(2,2,1)    ; Spieler + AI einfrieren, solange das Panel laeuft
@0x0694 01 00     Evt_end
```

Member 15 = Entitaet **+0x0B** (Port `actor_common.c:127` / `scd_vm.c:3390`), also genau der
AOT-Stempel aus §2.2.

sub02..sub05 (Datei 0x696 / 0x6A2 / 0x6AE / 0x6BA), je 12 Byte:
`2e 03 00` Work_set(3,0) · `2f 02 c8 00` / `2f 02 38 ff` / `2f 00 c8 00` / `2f 00 38 ff`
Speed_set(Achse 2 bzw. 0, **±200**) · `30` Add_speed · `02` Evt_next · `01 00` Evt_end.
=> **200 Einheiten Cursor-Weg pro gehaltenem Bild.**

Tastenmasken: virtuelles Wort, Preset 0. **Selbst nachgelesen**
(`re15_disasm.py bytes 0x80073dbc 32`):
```
80073dbc: 00 10 00 20 00 40 00 80 00 10 00 40 80 00 80 00
80073dcc: 08 00 40 00 08 00 04 00 00 80 00 20 80 00 40 00
```
= u16 LE [0]=0x1000 UP, [1]=0x2000 RIGHT, [2]=0x4000 DOWN, [3]=0x8000 LEFT, … [6]=0x0080 SQUARE,
[14]=0x0080 SQUARE, [15]=0x0040 CROSS. Damit: virt 0x0001 = UP, 0x0002 = RIGHT, 0x0004 = DOWN,
0x0008 = LEFT, **0x0040 = SQUARE = Bestaetigen**. Der Port baut dieselbe Tabelle
(`pad_common.c:28-37`) — geprueft, stimmt Eintrag fuer Eintrag.

### 2.4 Bank 3 Bits 54/55/56 = AUF WELCHER ETAGE DIE KABINE STEHT

Roh-Byte-Zensus ueber **alle 206 ausgelieferten RDTs** (Muster `22 03 36/37/38` = Set,
`21 03 36/37/38` = Ck). Nur vier Raeume benutzen diese drei Bits:

| Datei | Datei-Offset | Bytes | Bedeutung |
|---|---|---|---|
| ROOM1040 (1F) | 0x15C8 / 0x15D6 | `21 03 36 00` / `22 03 36 01` | Ck(3,54,0) -> Set(3,54,1) = Kabine 1F |
| ROOM1040 | 0x15E2 / 0x15F0 | `21 03 37 01` / `22 03 37 00` | Bit 55 loeschen |
| ROOM1040 | 0x15FC / 0x160A | `21 03 38 01` / `22 03 38 00` | Bit 56 loeschen |
| ROOM10C0 (2F) | 0x0FE0 / 0x0FEE | `21 03 37 00` / `22 03 37 01` | setzt 55, loescht 54 (0x0FFA) und 56 (0x1014) |
| ROOM1120 (3F) | 0x0D5E / 0x0D6C | `21 03 38 00` / `22 03 38 01` | setzt 56, loescht 54 (0x0D88) und 55 (0x0DA2) |
| ROOM1080 | 0x06F6 | `21 03 36 00` | sub07: `if (Kabine NICHT auf 1F)` |
| ROOM1080 | 0x0788 | `21 03 37 00` | sub08: `if (Kabine NICHT auf 2F)` |
| ROOM1080 | 0x081A | `21 03 38 00` | sub09: `if (Kabine NICHT auf 3F)` |
| ROOM1080 | 0x08AC/0x08B0 | `21 03 36 01` / `22 03 36 00` | sub10: Kabine 1F -> Bit weg, `Aot_on(0)` |
| ROOM1080 | 0x08BC/0x08C0 | `21 03 37 01` / `22 03 37 00` | sub10: Kabine 2F -> `Aot_on(1)` |
| ROOM1080 | 0x08CC/0x08D0 | `21 03 38 01` / `22 03 38 00` | sub10: Kabine 3F -> `Aot_on(2)` |

Genau EIN Bit ist gesetzt: **54 = 1F, 55 = 2F, 56 = 3F**, gepflegt von den drei Etagen-Raeumen.

Bank 3 liegt auf `DAT_800b0ff8` (Zonen-Tabelle `@0x80074664`, Eintrag [3]) und wird beim
Raumaufbau **NICHT** geloescht — selbst nachdisassembliert, `FUN_8003ecec`:
```
@0x8003ed74  sw zero,4136(at)    -> 0x800b1028   = Bank 5, WORT 0    <- das einzige Flag-Wort, das faellt
@0x8003ed7c  sh zero,4082(at)    -> 0x800b0ff2   = work_vars[0x11]
@0x8003ed60  sh v0,4084(at)      -> 0x800b0ff4   = work_vars[0x12] := 0xFF
@0x8003ed98/eda0/eda8  DAT_800aca3c &= ~0x800, &= ~0x400
```
Bank 3 (0x800b0ff8) kommt nicht vor => die Etagen-Flags ueberleben jeden Raumwechsel. Richtig.
Der Port macht genau dasselbe: `scd_room_setup.c:276  g_game.flags[5][0] = 0;` — **byte-true**.

### 2.5 sub07/08/09 — die Fahrt, und WO die Zieltuer gewaehlt wird

sub07 (0x6EE..0x780), sub08 (0x780..0x812), sub09 (0x812..0x8A4) sind byte-identisch bis auf
drei Stellen (Ck-Bit, Cut-Nummer, Aot_on-Slot):

```
@0x06EE 09 0a 01 00   Sleep 1
@0x06F2 06 00 86 00   Ifel_ck
@0x06F6 21 03 36 00   Ck(3,54,0)               ; sub08: 21 03 37 00 | sub09: 21 03 38 00
@0x06FA..0x071A       Set(5,0..8,0)            ; Panel aus
@0x071E 22 02 00 00   Set(2,0,0)               ; Spieler-Freeze AUS  (= g_pauseflags 0x80000000)
@0x0722 22 02 02 00   Set(2,2,0)               ; AI-Freeze AUS       (= 0x20000000)
@0x0726 29 02         Cut_chg 2                ; sub08: 29 03 | sub09: 29 04
@0x0728 09 0a 0f 00   Sleep 15
@0x072C 22 02 07 01   Set(2,7,1)               ; Pad-Freeze AN       (= 0x01000000)
@0x0730 22 01 1b 01   Set(1,27,1)              ; Letterbox / Cinematic
@0x0734 29 00         Cut_chg 0
@0x0736 2e 01 00      Work_set(1,0)            ; Arbeits-Entitaet = SPIELER
@0x073A 34 00 ae ca   Member_set(0, -13650)    ; X
@0x073E 34 02 cc f7   Member_set(2, -2100)     ; Z
@0x0742 34 04 00 0c   Member_set(4, 3072)      ; Yaw
@0x0746..0x0762       Set(1,28,1)/Sleep 8/Set(1,28,0)/Sleep 90/Set(1,28,1)/Sleep 8/Set(1,28,0)/Sleep 20
@0x0766 40 00 04 20 ae ca a8 fd   Plc_dest(slot 0, Mode 4 = WALK, Flag-Bit 0x20, Ziel (-13650,-600))
@0x076E 11 00 08 00 / 02 / 00 / 12 04 / 21 05 20 00   Do { Evt_next } while (Ck(5,32,0))
@0x077A 08 00         Endif
@0x077C 47 00         Aot_on 0                 ; sub08: 47 01 | sub09: 47 02   <- DIE ZIELTUER
@0x077E 01 00         Evt_end
```

**`Aot_on` steht AUSSERHALB des `Ifel_ck`** (Endif @0x077A, Aot_on @0x077C): die Zieltuer feuert
IMMER, die Fahrt-Choreografie nur, wenn die Kabine noch nicht auf der Zieletage steht.
=> **Bank 3 kann die ZIELETAGE gar nicht veraendern.** Nur sub10 liefert "aktuelle Etage".

`Aot_on` (0x47) ist FEUER-JETZT: `LAB_800407bc`, Record aus `DAT_800ac9b0[pc[1]]`
(@0x800407cc-ec), `jalr PTR_8007469c[rec[0]]` @0x8004082c mit a0 = Payload (@0x80040804/808).
Tabelle selbst gelesen: `[2] 0x800746a4 -> 0x800430bc` = Tuer-Handler.

`Ck(5,32,0)` in der Do-Schleife ist das **Plc_dest-Ankunftsflag**: Plc_dest-Byte pc[3] = 0x20
ist die Flag-Nummer in Bank 5 (Port: `scd_vm.c:2424/2481` clear, `actor_locomotion.c:383` set).
Zensus ueber alle 206 RDTs: `Ck(5,32)` **90x**, `Set(5,32)` **1x** (nur ROOM11B0 @0x145E) —
also ein Engine-Handshake, kein Skript-Flag.

### 2.6 Die Panel-Kamera — Cut 1..4 schauen SENKRECHT von oben auf die Knopf-Ebene

ROOM1080.RDT Kamera-Sektion (Offsettabelle Eintrag 7 = 0x60, 5 Saetze à 32 B), selbst gelesen:

```
cut0: pos=(-14340,-7396,-3595)  target=(-13490,-3620,-2254)   ; die Kabine
cut1: pos=(-19628,-17442,22616) target=(-19628,15928,22617)   ; PANEL, senkrecht nach unten
cut2: pos=(-19628,-17442,22616) target=(-19628,15928,22617)   ; Fahrt 1F
cut3: = cut2                                                   ; Fahrt 2F
cut4: = cut2 (flag=1)                                          ; Fahrt 3F
```

Die Kamera steht bei (x=-19628, z=22616) — praktisch auf dem Cursor-Startpunkt — und blickt
in +Y. Das absichtliche `target_z = pos_z + 1` haelt `atan2(dx,dz)` definiert.
Durchgerechnet mit dem Port-Builder (`camera_common.c:65ff`, byte-true `FUN_80053ca4`):
dx=0, dz=1, dy=33370 -> horiz=1, sy=0, cy=4096, sp=-4096, cp=0 ->
V = [[4096,0,0],[0,0,-4096],[0,4096,0]], d.h. **Bild-X = Welt-X, Bild-Y = -Welt-Z**.
Welt +Z ist also OBEN im Bild: 3F (z 27025) oben, 2F (24075) Mitte, 1F (21225) unten,
"Tuer auf" (16825) ganz unten. Und D-Pad UP (virt 0x0001) -> sub02 -> Speed_set(Achse 2, +200)
-> Cursor +Z -> Bild nach OBEN -> Richtung 3F. **Die Richtung stimmt**, in beiden Zweigen des
Builders (der `horiz==0`-Sonderzweig liefert dieselbe Matrix).

### 2.6b Der Cursor wird unter Cut 1 NICHT weggeschnitten

Der Objekt-Cull ist der Anker-Quad der RVD-Zone mit `cam_from == aktiver Cut`
(`FUN_80014324` -> `DAT_800ac790`, gefuettert an `FUN_8002c18c`; Port
`rdt_common.c:463-479` + `re15_aot.h:408-421`). ROOM1080s RVD-Block liegt @Datei 0x108
(u32 @0x28), Satzlaenge 20 — selbst gelesen:

```
zone0 @0x0108  camFrom=0 camTo=0  x[-16800..-10450]  z[ -5250..  1050]   ; die Kabine
zone1 @0x011C  camFrom=1 camTo=0  x[-32700.. -6500]  z[ 12500.. 32700]   ; der PANEL-Raum
zone2 @0x0130  camFrom=2 camTo=0  x[ 31300.. 32500]  z[ 32000.. 32700]   ; Fahrt-Cuts: Dummy
zone3 @0x0144  camFrom=3 camTo=0  x[ 31300.. 32500]  z[ 32000.. 32700]
zone4 @0x0158  camFrom=4 camTo=0  x[ 31300.. 32500]  z[ 32000.. 32700]
terminator @0x016C
```

Cursor (-19554, 22684) und alle vier Zellen (x -20600..-17350, z 15800..28050) liegen
**innerhalb** von zone1 => unter Cut 1 wird der Cursor gezeichnet, der Spieler (bei
(-12050,-670), z < 12500) dagegen weggeschnitten — genau die gewollte reine Panel-Ansicht.
Die These "der Nutzer waehlt blind, weil der Cursor unsichtbar ist" ist damit **widerlegt**,
solange der Port `re15_rdt_get_region_quad` benutzt (tut er).

### 2.7 Die Soll-Kette in einem Satz

Panel ansprechen (AOT 3) -> sub06 setzt Bank-5-Bits 0..8 + Cut 1 -> sub01 faehrt je Bild den
Cursor (Prop 0) mit dem D-Pad und liest seinen AOT-Stempel +0x0B -> SQUARE ueber Zelle 4/5/6
startet sub07/08/09 -> Fahrt -> `Aot_on(0/1/2)` = Tuer 1F/2F/3F. Zelle 7 startet sub10 =
"Tuer auf der aktuellen Etage" (Bank 3 Bit 54/55/56).

---

## 3. Was der Port tut — mit datei.c:zeile

| Glied | Port-Stelle | Befund |
|---|---|---|
| Tuer-Records | `re15_port/engine/src/scd_vm.c` op_door_aot_set; Warp-Zweig `aot_common.c:577-596` | korrekt; Payload wird IMMER benutzt (Null-Rect-Sonderfall wurde 2026-08-08 entfernt) |
| `Aot_on` (0x47) | `scd_vm.c:4144-4153` (`op_aot_on`) | FEUER-JETZT, byte-true LAB_800407bc; Sonde zeigt `[aot] DOOR FIRE slot=1` |
| SCE 3 -> Sub | `scd_vm.c:2942-2956` (ev != 0 / != 0xFF, `flags & 0x10` -> GENERIC), Feuer `aot_common.c:693-697` | korrekt: ev = pc[17] = 6 -> sub06 |
| Knopf-Zellen | `scd_vm.c` op_aot_set, `ev == 0` -> `RE15_AOT_TYPE_EXAMINE_WORKVAR`, ev := slot | Sonde: slot4..7, type 7, ev 4..7, sceflg 0x44 — passt |
| Objekt-Stempel +0x0B | `aot_common.c:418-489` (`re15_object_notch_update`), Auswahl `aot_obj_record_ok` (`aot_common.c:342-349`) | korrekt: Pool-Bit 0x04, kein 0x10, CENTRE/FORWARD; LAST-WINS |
| Stempel-Aufruf je Bild | `game_step_common.c:2228-2231` (nach AI, wie `@0x8001ce1c`) | laeuft |
| `Work_set(3,0)` | `scd_vm.c:3080-3093` (`scd_work_bind`, kind 3 = Prop nach obj_id) | korrekt |
| `Member_cmp(15)` | `scd_vm.c:3408-3450` | liest fuer Props `member_0b` |
| `Sce_key_ck(1,0x0040)` | `scd_vm.c:4715-4776` | byte-true gegen `g_scd_pad_held`; virt. Bit 6 = SQUARE |
| Pad-Wort | `pad_common.c:25-42`, gesetzt `game_step_common.c:1037` | Tabelle @0x80073dbc Eintrag fuer Eintrag geprueft |
| Pad-Maske bei PAUSE_PAD | `game_step_common.c:1062-1064` | `& 0xf000` NUR bei Bit 0x01000000; das Panel setzt Bit 0/2, nicht 7 — greift also nicht |
| Freeze Set(2,0)/Set(2,2) | `game_state.c:148-152` + `re15_game_flag_set` Zone-2-Zweig | Zone 2 Wort 0 geht ins echte `g_re15_pauseflags` — korrekt |
| Bank-5-Clear je Raum | `scd_room_setup.c:276` | byte-true `@0x8003ed74` (nur Wort 0) |
| Plc_dest Mode 4 | `scd_vm.c:2334-2365` (`is_walk` enthaelt 0x04), Ankunft `actor_locomotion.c:368-384` setzt `flag(5,0x20)` | korrekt |
| Kamera Cut 1 | `camera_common.c:65-118` | Senkrecht-Fall korrekt, +Z = Bild oben (§2.6) |

**Gemessen (§1): alle drei Etagen loesen im Port richtig auf, und Zelle 7 loest auf die
aktuelle Etage auf.** Die Sonde ist `re15_port/tests/unit/probe_elevator_1080.c`.

---

## 4. Der Unterschied, in einem Satz

**Keiner, den ich in der SCD-/AOT-/Tuer-Kette messen kann** — die Kette waehlt im Port
nachweislich Tuer 0/1/2 nach der gedrueckten Zelle; die Ursache des Nutzer-Befunds liegt
folglich ausserhalb dieser Kette und ist in diesem Lauf NICHT gefunden.

---

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

**Es gibt in diesem Lauf KEINE belegte Code-Aenderung.** Ein Fix ohne reproduzierten Defekt
waere geraten. Der Plan ist deshalb ein MESSPLAN, in dieser Reihenfolge:

1. **Reproduktion im echten Spiel** (`re15_port/build/platform/pc/re15_pc.exe`, existiert bereits):
   `RE15_INPUT_SCRIPT` + Debug-Menue (Token E = SELECT, T = Dreieck, A = Quadrat;
   `input_pc.c:395-404`) -> Stage 0 / JUMP-Index 8 = "ELEVATOR"
   (`debug_jump_table.h:22`), dann Panel ansprechen und 2F druecken.
   Mitschreiben mit `RE15_EVT_TRACE=1` (`scd_vm.c:1110-1122`, druckt je `Evt_exec`
   Bild/Sub/Slot) und `RE15_FLAG_TRACE=1`. **Erwartung, wenn die Kette stimmt:**
   `Evt_exec sub=8`. Kommt stattdessen `sub=7` oder `sub=10`, ist die Zellenwahl das
   Problem (Cursor-Sichtbarkeit / Projektion); kommt `sub=8` und der Raum ist trotzdem
   0x1040, liegt es an `re15_room_apply_pending` (`room_common.c:196`).
2. **Sichtlauf am echten Fenster** (Skill `re15-port-visual-verify`, gdigrab — NICHT
   AUTOSHOT/SOFTWARE_RENDER): zeigt das Panel (Cut 1) den Cursor an der Stelle, an der der
   Nutzer ihn vermutet? Der Region-Cull scheidet als Erklaerung aus (§2.6b), die PRI-Maske
   des Cuts (`pri=0x23c`) und das Prop-Modell selbst sind NICHT geprueft.
3. **Erst wenn 1./2. den Mechanismus zeigen:** Fix an der dann benannten Stelle, jede
   Konstante mit `@0x…`.
4. **Regressionsnetz:** `probe_elevator_1080` ist bereits geschrieben und deckt die Kette ab;
   es fehlt ein Lauf mit vorbelegtem `flag(3,54)=1` (Start auf 1F) — das ist die Lage des
   Nutzers und die einzige, die die Sonde noch nicht faehrt.

---

## 6. Offen / nicht belegt

* **Die Ursache selbst.** Versucht: (a) vollstaendiger Opcode-Dump ROOM1080 main00+sub00..sub10
  mit Datei-Offsets; (b) Roh-Byte-Zensus ueber alle 206 RDTs fuer Bank-3-Bits 54/55/56 und
  fuer `Ck/Set(5,32)`; (c) Disassemblat der SCE-Tabelle @0x8007469c, Handler 3 @0x800430f0,
  des Raum-Flag-Clears `FUN_8003ecec` @0x8003ed50-eda8 und der Pad-Preset-Tabelle
  @0x80073dbc; (d) Code-Durchgang aller zwoelf Port-Glieder (§3); (e) Lauf der bestehenden
  Sonde `probe_elevator_1080.exe` — Ergebnis 3/3 Etagen richtig; (f) Kamera-Cut-1-Rechnung.
  Naechster Weg: der Messplan §5 Punkt 1 und 2 (echtes Spiel + Sichtlauf).
* **Die Sonde ist vom 2026-09-14 gebaut**, der Port vom 2026-09-26. Zwischen beiden liegen
  Commits, die genau diese Glieder anfassen: `228bf344` (Klick-Laut, aendert
  `re15_object_notch_update` und `op_sce_key_ck`), `3f201732` (Obj_model_set pc[5];
  ROOM1080 hat pc[5]=0x00, also parent = -1, sollte folgenlos sein), `88a988d5` (Plc_dest
  +0x1c4), `630e87d2` (Prop-Verstecken per obj_id — der Cursor IST obj_id 0), sowie die
  vier `discard`-Commits vom 2026-09-22, die eine neue Frueh-Rueckkehr
  `re15_discard_frozen()` (`game_step_common.c:985`) vor den SCD-Tick setzen.
  **Ob die heutige Engine die Sonde noch besteht, ist NICHT gemessen** — dazu muesste
  `probe_elevator_1080` neu gebaut werden, was diese Runde nicht darf. Das ist der erste
  Schritt der naechsten Runde und billiger als alles andere.
* **Teilweise geprueft:** der Region-Cull schneidet den Cursor unter Cut 1 NICHT weg
  (§2.6b, RVD-Zone 1 deckt den ganzen Panel-Raum). NICHT geprueft ist dagegen, ob der
  Cursor-Prop (obj 0, **type 4**) auf PC ueberhaupt ein Modell bekommt: die Prop-Schleife
  liegt in `re15_port/platform/pc/main.c:9295-9350`, das Modell kommt aus dem
  RDT-Objektblock (`nOmodel=1`). Immerhin: der Port KENNT den Typ — `re15_prop_render_y`
  (`re15_aot.h:471-474`) hebt genau `obj_type == 4` um 900 an (`@0x8002c23c/@0x8002c24c`),
  also die Box-Mitte c(0,-900,0) des Cursors. Der Cursor ist damit ein bekannter, behandelter
  Prop. Dass er auch wirklich am richtigen Bildort erscheint, ist trotzdem nur per Sichtlauf
  zu sagen — es ist der letzte Verdacht, der nach §2.6b noch steht.
* **Nicht geprueft:** RE2-Vergleich (Auftragspunkt d). Er war nicht noetig, weil RE1.5 selbst
  vollstaendig lesbar war; falls der Messplan scheitert, waere
  `RE2_Quellcode_Overlays/` + `info/re2leon/` der naechste Vergleichsweg.
* **Beifund, nicht Teil des Auftrags:** sub07/08/09 setzen `Set(1,27,1)` (Letterbox) @0x0730
  und loeschen es nie. Im Original wird Bank 1 beim Raumaufbau nur um die Bits 0x800 und
  0x400 beschnitten (@0x8003ed98/eda0) — Bit 27 (Maske 0x10) ist NICHT darunter. Wer das
  Letterbox-Latch im Port loescht, steht in `room_common.c:300-302` (scd_room_reenter);
  das ist eine eigene Frage mit eigener Verifikation und hier nicht entschieden.
