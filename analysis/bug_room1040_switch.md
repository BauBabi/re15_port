# BUG 3 — ROOM1040: „It's a shutter switch. Will you push it?" direkt beim Betreten

**Report (Nutzer):** „In ROOM1040 kommt DIREKT BEIM BETRETEN die Frage, ob man den Schalter
betaetigen will — obwohl man gar nicht am Schalter steht. Kontrolle wird auch hier nicht
zurueckgegeben."

**Stand:** Ursache vollstaendig belegt (Original-Disasm + RDT-Bytes) UND im Port gemessen
(Probe `re15_port/tests/unit/probe_room1040_switch.c`). Es ist **kein AOT-Scan-Fehler und kein
Rect-Parse-Fehler** — beide Kandidaten sind widerlegt (§5). Der Defekt ist **generell**, nicht
raumspezifisch: der Port **loescht Flag-Bank 5 nicht beim Raumladen**, obwohl das Original
genau das tut. Bank 5 ist der **pro-Raum-Scratch-Speicher** von 94 Raeumen.

---

## 1. Was der Prompt ist — die Message

RDT-Adresstabelle ROOM1040: `messageStart` @0x3C = **0x21c8**, `mainScd` @0x40 = 0x1074,
`subScd` @0x44 = 0x1524.

`msg[0]` @Datei **0x21cc** (Block+0x0004), Bytes verifiziert:

```
04 02 25 50 3a 4f 00 3d 00 4f 44 51 50 50 41 4e 00 4f 53 45 50 3f 44 57
08 33 45 48 48 00 55 4b 51 00 4c 51 4f 44 00 45 50 1b 03 02 01
= <04:02> "It's a shutter switch. Will you push it" <1b:YESNO><03:WAIT><02:LF><END>
```

Das `0x03` macht daraus im Port eine **blockierende YES/NO-Query**
(`re15_msg_is_choice`, msg_common.c:293-308).
(`msg[1]` = „It's an elevator. Pressing the button has no effect…" — das ist die Message des
sce-1-AOT Slot 0, NICHT der gemeldete Text.)

## 2. Wer die Message zeigt — Message_on-Operanden byte-true

SCD-Opcode 0x2B, Handler **@0x800404f4** (PSX.EXE):

```
800404fc  lw    v0,28(a0)       ; v0 = thread->pc (+0x1c)
80040500  ori   a1,zero,0x300   ; param_2 = 0x300
80040504  lbu   a2,1(v0)        ; param_3 = pc[1]  <-- MESSAGE-INDEX
80040508  lhu   a3,2(v0)        ; u16 @ pc[2]
8004050c  addiu v0,v0,4         ; pc += 4
80040518  jal   0x80027e68
8004051c  sll   a3,a3,16        ; param_4 = u16@pc[2] << 16 (Farbe/Pause)
```

⇒ `2b 00 ff ff` = **Message_on(msg 0)**. Der Port liest ebenfalls `t->pc[1]`
(scd_vm.c:1275/1288/1305) — hier ist der Port korrekt.

**In ROOM1040 steht `2b 00 ff ff` genau dreimal, jeweils als ERSTES Opcode eines Subs:**

| Sub | scd-Off | Datei-Off | Bytes |
|---|---|---|---|
| sub02 | 0x00f2 | **0x1616** | `2b 00 ff ff` |
| sub07 | 0x0402 | **0x1926** | `2b 00 ff ff` |
| sub08 | 0x0478 | **0x199c** | `2b 00 ff ff` |

## 3. Wer diese Subs startet — sub01, gated auf Flag(5,0x21)

ROOM1040 **sub01** (scd 0x0042..0x00f2, Datei 0x1566..0x1616), Wortlaut:

```
scd 0x0042  f0x1566  06 00 30 00   Ifel_ck -> 0x0076
scd 0x0046  f0x156a  21 05 21 01   Ck(zone 5, bit 0x21) == 1     <-- der Schalter-Latch
scd 0x004a  f0x156e  21 05 00 00   Ck(zone 5, bit 0x00) == 0     <-- „busy"-Latch
scd 0x004e  f0x1572  21 04 05 00   Ck(zone 4, bit 0x05) == 0
scd 0x0052  f0x1576  22 05 00 01   Set(5,0x00)=1
scd 0x0056  f0x157a  22 04 05 01   Set(4,0x05)=1
scd 0x005a  f0x157e  06 00 10 00   Ifel_ck -> 0x006e
scd 0x005e  f0x1582  21 04 04 00   Ck(4,0x04)==0
scd 0x0062  f0x1586  22 04 04 01   Set(4,0x04)=1
scd 0x0066  f0x158a  04 ff 18 08   Evt_exec sub08   <-- Message_on(0)
scd 0x006a  f0x158e  07 00 0a 00   Else_ck -> 0x0074
scd 0x006e  f0x1592  04 ff 18 07   Evt_exec sub07   <-- Message_on(0)
scd 0x0076  f0x159a  06 00 1a 00   Ifel_ck -> 0x0094
scd 0x007a  f0x159e  21 05 21 01   Ck(5,0x21)==1
scd 0x007e  f0x15a2  21 05 00 00   Ck(5,0x00)==0
scd 0x0082  f0x15a6  21 04 05 01   Ck(4,0x05)==1
scd 0x0086  f0x15aa  22 05 00 01   Set(5,0x00)=1
scd 0x008a  f0x15ae  22 04 05 00   Set(4,0x05)=0
scd 0x008e  f0x15b2  04 ff 18 02   Evt_exec sub02   <-- Message_on(0)
scd 0x0094  f0x15b8  18 0a         Gosub sub10
scd 0x0096  f0x15ba  18 09         Gosub sub09
```

Einziger Zuendschluessel: **Flag(5,0x21) == 1 UND Flag(5,0x00) == 0**.

## 4. Wer Flag(5,0x21) setzt — und wer sie loescht

### 4a. Setzen: der Schalter-AOT (Slot 4)

ROOM1040 main00 @scd **0x00a4**, Datei **0x1118**:

```
2c 04 04 31 00 00  70 9a  80 c1  34 08  98 08  05 00 21 00 01 00
Aot_set slot=4 sce=4 flags=0x31 band=0 follow=0
        rect corner=(-26000,-16000) extent=(2100,2200)
        payload: bank=0x0005  bit=0x0021  on=0x0001
```

sce-4-Handler = Dispatch `0x8007469c[4]` → **LAB_80043120**:

```
80043120 lhu v1,0(a0)   ; bank-Index
80043124 lhu a1,2(a0)   ; bit
80043128 lhu a0,4(a0)   ; on/off
8004312c sll v1,v1,2
80043130 srl v0,a1,3 / 80043144 andi v0,v0,0x1c    ; Byte-Offset = (bit>>5)*4
80043138 addiu at,at,0x74664 (=0x80074664)         ; Bank-Pointer-Tabelle
80043150 andi v0,a1,0x1f / 80043154 lui v1,0x8000 / 8004315c srlv v1,v1,v0
80043160/80043180  or / sw   (on)      bzw. 80043178 nor + and + sw  (off)
```

⇒ exakt Set/Clear von Bank[bit], MSB-first — der Port ist hier byte-true
(`re15_game_flag_set`, game_state.c:37-43).

Flags `0x31` hat Bit 0x10 gesetzt = **ACTION-Gate** (nur beim Tastendruck; byte-true
FUN_80042bac). Gemessen: der Port feuert diesen AOT korrekt **nur** auf SQUARE (§6).

**In ROOM1040.RDT gibt es KEIN `22 05 21 xx` (Set(5,0x21,*))** — Byte-Scan der ganzen Datei.
Das Skript loescht die Flag also nie.

### 4b. Loeschen: das Original wischt Bank 5 bei JEDEM Raumladen ⭐

Bank-Pointer-Tabelle `0x80074664`:

```
[ 5] 0x80074678 -> 0x800b1028      ; Bank 5
[ 6] 0x8007467c -> 0x800b1030      ; => Bank 5 ist genau 8 Byte = Bits 0x00..0x3F
```

Bit 0x21 (=33) liegt damit in Wort 1 @**0x800b102c**.
Die *einzigen* Schreiber dieser beiden Worte im ganzen EXE:

```
FUN_8003ebf4:
  8003ebf4  addiu v0,zero,-1
  8003ebfc  sh v0,0x800b0fd0      ; work_vars[0] = -1
  8003ec04  sh v0,0x800b0fd2      ; work_vars[1] = -1
  8003ec0c  sh v0,0x800b0fd4      ; work_vars[2] = -1
  8003ec14  sh v0,0x800b0fd6      ; work_vars[3] = -1
  8003ec1c  sw zero,0x800b102c    ; *** Flag-Bank 5, Bits 0x20..0x3F = 0  (incl. 0x21!) ***

FUN_8003ecec:
  8003ed74  sw zero,0x800b1028    ; *** Flag-Bank 5, Bits 0x00..0x1F = 0  (incl. 0x00) ***
```

Beide werden aus der **SCD-Raum-Init `FUN_8003ef6c`** gerufen:

```
8003ef74  jal 0x8003ea3c          ; RNG
8003ef7c  jal 0x8003ebf4          ; <- work_vars[0..3]=-1, Bank5 Bits 0x20..0x3F = 0
8003ef84  jal 0x8003ecec          ; <- SCD-Instanzen reset, Bank5 Bits 0x00..0x1F = 0
8003efa0  lw v0,64(v0) / 8003efac sw v0,0x800b3f70   ; base = room+0x40 (mainScd)
8003efb0  jal FUN_8003ee3c        ; a0=0, a1=0  -> Thread-Slot 0 = main00
8003efc4  lw v0,68(v0) / 8003efd0 sw v0,0x800b3f70   ; base = room+0x44 (subScd)
8003efd4  jal FUN_8003ee3c        ; a0=1, a1=0  -> Thread-Slot 1 = sub00   (NICHT sub01)
8003f018  jal 0x8003f0a0          ; VM einmal laufen lassen
```

und `FUN_8003ef6c` haengt an der Raum-(Neu-)Lade-Kette **`FUN_800396fc` @0x80039a00**
(Ghidra-XREF: `FUN_8003ef6c XREF[1]: FUN_800396fc:80039a00(c)`).

⇒ **Flag-Zone 5 ist ein pro-Raum-SCRATCH-Bank und wird bei jedem Raumladen komplett
genullt.** Das deckt sich mit dem unabhaengig beobachteten Befund in
`STAGE1_FLAG_MAP.md:30` („z5 = Per-room event/working flags … transient, not persistent
progression", 1250 Reads). Ein Byte-Scan zeigt: **94 Raeume** benutzen Zone 5 fuer ihre
eigenen lokalen Zustandsmaschinen (ROOM1030 45 Ck/42 Set, ROOM1040 48/51, ROOM10D0 44/91,
ROOM11F0 36/89 …) — ohne den Wisch verschmutzen sie sich gegenseitig.

### 4c. Der Port wischt NICHT

`scd_room_reenter` (`re15_port/engine/src/scd_room_setup.c:109-176`) macht
`memset(&g_scd,…)` und loescht ausdruecklich nur **Bank 1 Bits 16..31** (Zeile 122).
`g_game.flags[5][]` bleibt fuer die gesamte Session stehen.

## 5. Widerlegte Kandidaten

| Kandidat | Befund |
|---|---|
| (a) AOT-Scan feuert Typ ohne Aktionstaste | **widerlegt/gemessen.** Slot 4 (`flags 0x31`) laeuft im Port ueber `(a->sce_flags & 0x10) ? (gen_reach && g_aot_action_pressed …)` (aot_common.c:935-938). Probe: ohne Tastendruck bleibt f(5,0x21)=0 auch nach 30 Frames. |
| (b) Rect-Parse (Ecke/Ausdehnung vertauscht) | **widerlegt.** Original-Test **FUN_80042b64** @0x80042b64: `lh v0,0(a1)`=Ecke, `lw v1,0(a2)`=Entity, `subu v1,v1,v0`, `lhu a0,4(a1)`=Ausdehnung, `sltu a0,a0,v1` → **UNSIGNED**, also `0 <= dx <= w`. Ecke+Ausdehnung, genau wie der Port (scd_vm.c:2231-2238) es nach Mitte+Halbmass umrechnet. **Rest-Nit:** bei UNGERADEN Ausdehnungen verliert die Halbierung 1 Einheit an der +x/+z-Kante (w=5 → dx 0..4 statt 0..5). In ROOM1040 sind alle Ausdehnungen gerade ⇒ hier wirkungslos. |
| (c) Band/Floor-Gate fehlt | **nicht ursaechlich.** Der Port hat das Gate (aot_common.c:780-790). Alle ROOM1040-AOTs haben `band=0`. |
| (d) `examine_poll_pending` feuert beim Raum-Eintritt | **teilweise.** Der Zustellweg ist der Einmal-Start von sub01 in `scd_room_reenter:171-174` (nicht der examine-Pfad; ROOM1040 hat keinen sce-5-AOT). Der eigentliche Ausloeser ist die stehengebliebene Flag. |

## 6. MESSUNG im Port — `probe_room1040_switch.c`

Probe laedt ROOM1040.RDT, setzt den Spieler auf den Tuer-Spawn `(-24000, 0, 16500)`
(= inbound-Door aus ROOM1020, `next=(-24000,0,16500)` cut 6) und ruft **dieselbe** Funktion,
die der Tuer-Pfad benutzt (`scd_room_reenter`, room_common.c:171).

**Lauf A — frische Flags (Referenz):** kein Prompt, kein SCD-Thread, `msg active=0`.

**Lauf B — kompletter Nutzer-Ablauf (`probe_room1040_switch 1040 -24000 16500 0 1`):**

```
== 1) SQUARE am Schalter gedrueckt (+60 Frames) ==
   msg: active=0 query=0 fsm=0 id=0            <-- NICHTS passiert (kein Rolltor, kein Prompt)
   threads:                                     <-- kein SCD-Thread laeuft
   slot  4: FLAG_CHG sceflags=0x31 c=(-24950,-14900) inside=1
   f(5,0x21)=1                                  <-- Latch gesetzt, nichts loescht ihn

== 2) Raum verlassen + WIEDER BETRETEN ==
   msg: active=1 query=1 fsm=1 id=0            <-- *** blockierende YES/NO-Query, msg 0 ***
   threads: [10]                                <-- sub08 geparkt
   slot  4: inside=0                            <-- Spieler an der TUER (-24000,16500),
                                                    31.400 Einheiten vom Schalter entfernt
   f(4,5)=1 f(4,4)=1 f(5,0x21)=1 f(5,0)=1
```

Log: `[scd] thread start slot=10 first_op=0x2B` + `[msg] room=1040 id=0` — **im allerersten
Frame nach `scd_room_reenter`**. Das ist exakt der Report.

## 7. Warum die Kontrolle wegbleibt

sub08 (Datei 0x199c ff.), nach der Antwort:

```
scd 0x0478 f0x199c  2b 00 ff ff   Message_on(0)              ; die Frage
scd 0x047c f0x19a0  02            Evt_next                   ; 1 Frame warten
scd 0x0482 f0x19a6  21 0c 1f 00   Ck(zone 12, bit 0x1f)==0   ; die YES/NO-Antwort
scd 0x0486 f0x19aa  22 01 1b 01   Set(1,0x1b)=1              ; *** Cutscene-Latch 1 ***
scd 0x048a f0x19ae  22 02 07 01   Set(2,0x07)=1              ; *** Cutscene-Latch 2 ***
…  Se_on / Sleep / Work_set(1) / Plc_dest -> (-25200,-360) / For+Add_speed …
scd 0x0508 f0x1a2c  22 01 1b 00   Set(1,0x1b)=0
scd 0x050c f0x1a30  22 02 07 00   Set(2,0x07)=0
scd 0x0513 f0x1a37  42            Plc_ret                    ; Kontrolle zurueck
```

`flag(1,27) || flag(2,7)` zieht im Port jeden Frame `g_scd.player_mode = 2` (scripted, Pad
ignoriert) — `platform/pc/main.c:3012-3026`. Die Freigabe steht erst hinter dem kompletten
Rolltor-/Aufzug-Ablauf inkl. `Plc_dest` auf **(-25200, -360)** (= die Aufzugsplattform,
`Obj_model_set[0] pos=(-25200,0,-360)`). Weil der Spieler in Wahrheit an der Tuer steht
(31.400 Einheiten entfernt), laeuft das Skript ins Leere ⇒ die beiden Latches bleiben stehen
⇒ **kein Pad, keine Kontrolle**. (Die Probe zeigt Thread 10 nach 30 Frames noch aktiv.)

## 8. Zweite Divergenz derselben Klasse — sub01 laeuft im Port nicht pro Frame

Das Original re-seedet **jeden Gameplay-Frame** SCD-Slot 1 mit sub01:

```
Gameplay-Tick @0x8001cdec:  jal FUN_8003f038

FUN_8003f038:
  8003f040  lw v0,0x800aca40
  8003f044  lui v1,0x200 / and / 8003f04c bne  -> skip, falls (DAT_800aca40 & 0x02000000)
  8003f060  lw v0,0x800ac778           ; room work ptr
  8003f064  ori a0,zero,0x1            ; a0 = SLOT 1
  8003f070  lw v0,68(v0)               ; room+0x44 = subScd-Basis
  8003f07c  sw v0,0x800b3f70
  8003f080  jal FUN_8003ee3c
  8003f084  ori a1,zero,0x1            ; a1 = SUB-ID 1  ==> sub01
  8003f088  jal 0x8003f0a0             ; VM laufen lassen

FUN_8003edec (Seed):
  8003ee1c  lw v0,0x800b3f70 / 8003ee20 sll a1,a1,1 / 8003ee28 lhu v1,0(a1)
  8003ee30  addu v0,v0,v1 / 8003ee38 sw v0,28(a0)     ; pc = sub_scd[1]
```

Der Port startet sub01 dagegen **einmal** beim Raum-Eintritt (`scd_room_setup.c:171-174`)
und sonst nur als Einmal-Schuss ueber `examine_poll_pending` (`scd_vm.c:525-528`).

Folgen (beide gemessen, §6 Lauf B):
* Der ROOM1040-Rolltor-/Aufzugsschalter **funktioniert im Port ueberhaupt nicht** — druecken
  passiert nichts.
* Ein einmal gesetzter Latch detoniert stattdessen beim **naechsten Raum-Eintritt**.

## 9. Dritte Divergenz derselben Funktion — work_vars[0..3]

`FUN_8003ebf4` setzt beim Raumladen zusaetzlich `work_vars[0..3] = -1`
(@0x8003ebfc/ec04/ec0c/ec14 → 0x800b0fd0/d2/d4/d6). Der Port `memset`et g_scd → **0**.
Belegter Folgefall: **ROOM5100/5101 sub01** @Datei 0x08da `23 00 01 00 00 00` =
`Cmp(work_vars[1] == 0)` — im Original beim Eintritt FALSCH (-1), im Port WAHR (0) ⇒ das
Sub feuert spurios im ersten Frame (gleiche Fehlerklasse wie ROOM1040, anderer Raum).

## 10. Fix-Plan (belegt, minimal, alles in `scd_room_setup.c` / `scd_vm.c`)

**F1 (Kern des Reports) — Flag-Bank 5 beim Raumladen nullen.**
In `scd_room_reenter()` VOR `scd_thread_start(0, rdt->main_scd)`:
```c
/* byte-true: FUN_8003ebf4 @0x8003ec1c `sw zero,0x800b102c` (Bits 0x20..0x3F)
 *          + FUN_8003ecec @0x8003ed74 `sw zero,0x800b1028` (Bits 0x00..0x1F);
 * beide aus der SCD-Raum-Init FUN_8003ef6c @0x8003ef7c/@0x8003ef84,
 * die an der Raum-Ladekette FUN_800396fc @0x80039a00 haengt.
 * Bank 5 ist im Original GENAU 8 Byte (Tabelle 0x80074664[5]=0x800b1028,
 * [6]=0x800b1030) -> nur Wort 0 und 1 loeschen, NICHT die ganze Port-Zone. */
g_game.flags[5][0] = 0;
g_game.flags[5][1] = 0;
```

**F2 — work_vars[0..3] = -1 beim Raumladen.**
Nach dem `memset(&g_scd,…)`, vor `scd_vm_tick()`:
```c
/* byte-true FUN_8003ebf4 @0x8003ebfc/ec04/ec0c/ec14: sh -1 -> 0x800b0fd0/d2/d4/d6 */
for (int i = 0; i < 4; i++) g_scd.work_vars[i] = -1;
```
(Reihenfolge beachten: `g_scd.work_vars[10] = entry_scenario` bleibt danach unangetastet.)

**F3 — sub01 pro Frame re-seeden statt einmal.**
In `scd_vm_tick()` VOR der Thread-Schleife (ersetzt den Einmal-Start in
`scd_room_reenter:171-174` und den `examine_poll_pending`-Ersatzpfad in `scd_vm.c:525-528`):
```c
/* byte-true FUN_8003f038 @0x8003f064/70/80/84: jeden Gameplay-Frame
 * FUN_8003ee3c(slot=1, id=1) -> Thread 1 wird auf sub_scd[1] zurueckgesetzt
 * (FUN_8003edec @0x8003ee1c-38), danach laeuft die VM.
 * Gate @0x8003f040-4c: nur wenn (DAT_800aca40 & 0x02000000) == 0,
 * das ist Flag-Bank 2, MSB-first-Index 6 (0x80000000>>6 == 0x02000000). */
if (s_current_rdt && s_current_rdt->sub_scd[1] && !re15_game_flag_get(2, 6))
    scd_thread_start(1, s_current_rdt->sub_scd[1]);   /* unbedingter Reset, wie FUN_8003edec */
```
Wichtig: `scd_thread_start` muss den Thread-Zustand vollstaendig zuruecksetzen (level 0,
loop-/block-Stack leer, aktiv=1) — genau das tut FUN_8003edec (`sb 1,1(a0)`, `sb 0,0(a0)`,
`sb -1,4(a0)`, `sb -1,8(a0)`, `sw pc,28(a0)`). Ein sub01, das `Evt_next` macht, wird im
Original schlicht im naechsten Frame neu gestartet.

**Reihenfolge im Original:** Raum-Init (F1/F2) → main00 + sub00 (1 VM-Lauf) → ab dem ersten
Gameplay-Frame sub01-Re-Seed (F3) → danach `FUN_800436a8` (AOT-Auto-Scan, @0x8001ce1c).

Mit F1 allein ist der gemeldete Prompt weg (die Gate-Flag ist beim Eintritt 0). F3 ist
noetig, damit der Schalter ueberhaupt funktioniert; ohne F1 wuerde F3 den Prompt nur vom
Tuer-Frame auf den ersten Gameplay-Frame verschieben.

## 11. Regressions-Risiko und Absicherung

| Fix | Was kaputtgehen koennte | Absicherung |
|---|---|---|
| F1 | Alles, was (faelschlich) auf Zone-5-Persistenz ueber Raumgrenzen baut. Reale Nutzer im Port: Gegner-`walk_flag_bit` (`actor_locomotion.c:323`, `enemy_ai_common.c:6505`), Ivy-Consume z5/31 (`enemy_ai_common.c:8777`), Keypad z5/13..16, ROOM1030-Gate z5/32..34. **Alle** setzen+lesen innerhalb EINES Raums ⇒ per Konstruktion unbetroffen (das Original wischt ja auch). | Voller `ctest` (110 Tests), insbesondere `integration_keypad` (test_keypad.c), `test_ivy_ai`, `test_flag_gate`, `test_aot_sce_census` (prueft z5/0x21!), `probe_cut_1030`. Zusaetzlich: NEUER ctest aus `probe_room1040_switch` — Lauf A (frisch) MUSS `msg.active==0` liefern, Lauf B nach Fix ebenfalls. |
| F1 | Nur Wort 0+1 loeschen! Ein `memset` der ganzen Port-Zone (8 Worte = 256 Bit) wuerde Bits ≥ 0x40 mit-loeschen, die das Original in Bank 6/7 schreibt (FUN_8004ef90 rechnet `bank + (bit>>5)*4`, ohne Bank-Grenze). `walk_flag_bit` = entity+0x1c3 kann > 63 sein. | Code-Review + `test_enemy_ai` / `test_npc_ai` / `test_room1140_combat` (nutzen walk_flag_bit-Pfad). |
| F2 | Skripte, die `work_vars[0..3] == 0` als „unbenutzt" lesen, brechen (jetzt -1) — das ist genau der Punkt. Betroffen bekannt: ROOM1150 sub01 `Cmp(work_vars[0]==2)` (unveraendert), ROOM1190/1191 (`==2`, unveraendert), ROOM5100/5101 (`==0`, wird korrekt FALSE). | `test_room1150_itembox`, `probe_headlook_10d0`, `test_scd_*`, `test_member`. Danach ROOM1150 „examine Irons → I'll be fine" manuell/probeweise nachfahren. |
| F3 | **Groesstes Risiko** — jedes sub01 laeuft ab jetzt 30x/s statt 1x. Auditiert: ROOM1130 sub01 = `Aot_reset(3,sce1,msg1)` (idempotent, Datei 0xa1c); ROOM1150 sub01 = `Cmp(wv0==2) && Ck(3,0x9d)==0 → Set(3,0x9d)=1; Evt_exec sub03` (durch den z3-Latch exakt einmal). Raeume mit unlatchtem sub01 wuerden loopen. | Vor dem Merge: Byte-Scan aller 240 sub01 auf „Evt_exec ohne Einmal-Latch". Danach voller `ctest` + die Raum-Proben (`probe_door_1170`, `probe_zreentry_10d0`, `probe_marvin_10d0`, `probe_stair_10a0`, `test_room1150_itembox`, `test_room1140_*`) + ein Durchlauf ab EXE-Start (Title→NEW GAME→Raumkette, kein Raum-Sprung). |
| F3 | Der `examine_poll_pending`-Ersatzpfad (`scd_vm.c:525-528`) wird redundant; ihn stehen zu lassen wuerde sub01 doppelt starten (Slot 1 UND Slot 2). | Beim Entfernen `test_room1150_itembox` + `probe_headlook_10d0` erneut fahren (die haengen an dem Pfad). |

## 12. Offen

* **Live-Verifikation im echten Fenster** (gdigrab, Skill `re15-port-visual-verify`) steht aus.
  Die Messung hier ist die deterministische Probe auf demselben `scd_room_reenter`-Pfad, den
  `room_common.c:171` (Tuer) benutzt; ein Voll-Durchlauf ab Title bis ROOM1040 wurde wegen
  Laufzeit nicht gefahren.
* **Wie die Flag beim Nutzer scharf wurde**, ist nicht direkt gemessen. Zwei belegte Wege:
  (i) ROOM1040s eigener Schalter — ein einziger SQUARE-Druck genuegt (gemessen §6);
  (ii) **ROOM1030** setzt dieselbe Flag(5,0x21) ueber `Aot_set slot=4 sce=4 flags=0x42`
  (main00 @scd 0x0082, Datei 0x1cde, Rect Ecke(-14800,-22500) Ausdehnung(12000,2200)) —
  **flags 0x42 hat Bit 0x10 NICHT gesetzt ⇒ AUTO, kein Tastendruck noetig**, Pool-Bits
  `0x42 & 0x07 = 0x02` = Gegner-Pool. ROOM1030 liegt direkt an ROOM1040 (Tuer Slot 2 →
  room 0x04). Ein Zombie, der durch diese Zone laeuft, bewaffnet damit den ROOM1040-Prompt
  **ohne jedes Zutun des Spielers**. Die ROOM1030-Zombies spawnen bei z ≈ −25655..−30000
  (gemessen), die Zone liegt bei z ∈ [−22500, −20300] — sie muessen also erst dorthin laufen;
  ob das im Report-Lauf passiert ist, ist NICHT gemessen ⇒ „wahrscheinlich".
* **ROOM10D0-Beobachtung aus der frueheren Session** (gleicher Prompt nach `RE15_GOTO_ROOM`)
  ist NICHT re-reproduziert. Dort ist der Text `msg[11]` (@Datei 0x20b9) und wird von
  **sub21** @scd 0x07d2 (Datei 0x19de) `2b 0b 80 ff` gezeigt; sub21 haengt ausschliesslich am
  AOT Slot 18 (`Aot_set slot=18 sce=3 flags=0x31 ev=0x15`, sub00 @scd 0x0036, Datei 0x1242,
  Rect Ecke(4700,19400) Ausdehnung(800,800)) — es gibt in ROOM10D0.RDT weder `04 ff 18 15`
  (Evt_exec sub21) noch `47 12` (Aot_on 18). Der GOTO-Spawn (7650,11400) liegt NICHT in der
  Zone. ⇒ dort muss ein anderer Weg (spurioser Action-Edge / Slot-18-Zustand) gefeuert haben:
  **OFFEN**, separat zu messen.

---

## Anhang — vollstaendige AOT-Liste ROOM1040 (main00, Datei 0x1076 ff.)

Rect-Angabe = **Ecke(x,z) + Ausdehnung(w,d)** (Original-Semantik, FUN_80042b64).

| scd | Datei | Opcode | Slot | sce | flags | Ecke | Ausdehnung | Nutzlast / Ziel |
|---|---|---|---|---|---|---|---|---|
| 0x0002 | 0x1076 | `Ifel_ck` → 0x0022 | | | | | | `Ck(4,0xf3)==0` |
| 0x000a | 0x107e | Aot_set | 0 | **1 MESSAGE** | 0x31 | (−22936,−11000) | (2300,1400) | msg **1** („It's an elevator…"), pause 0xFFFF |
| 0x001e | 0x1092 | `Else_ck` → 0x0044 | | | | | | |
| 0x0022 | 0x1096 | Door_aot_set | 0 | 2 | 0x31 | (−22936,−11000) | (2300,1400) | → **ROOM1080** cut 0, Spawn (−13650,0,−900) |
| 0x0044 | 0x10b8 | Door_aot_set | 1 | 2 | 0x31 | (−20400,−4900) | (1500,2200) | → **ROOM1030** cut 8 |
| 0x0064 | 0x10d8 | Door_aot_set | 2 | 2 | 0x31 | (−20400,−14100) | (1500,2200) | → **ROOM1060** cut 7, Spawn (26000,0,25300) |
| 0x0084 | 0x10f8 | Door_aot_set | 3 | 2 | 0x31 | (−24300,15600) | (1500,2200) | → **ROOM1020** cut 0, Spawn (−25883,0,−8767) |
| 0x00a4 | 0x1118 | Aot_set | **4** | **4 FLAG** | **0x31 (ACTION)** | (−26000,−16000) | (2100,2200) | **Bank 5, Bit 0x21, on** ← der Schalter |
| 0x00b8 | 0x112c | Aot_set | 5 | 4 FLAG | 0x42 (AUTO/Gegner) | (−27000,−1500) | (3600,2000) | Bank 5, Bit 0x20, on |
| 0x00cc | 0x1140 | Aot_set | 6 | 4 FLAG | 0x42 (AUTO/Gegner) | (−26700,20200) | (3900,1800) | Bank 5, Bit 0x22, on |
| 0x00e0 | 0x1154 | Aot_set | 7 | 4 FLAG | 0x42 (AUTO/Gegner) | (−27100,15200) | (3800,3000) | Bank 5, Bit 0x23, on |
| 0x00f4.. | 0x1168.. | 4× Obj_model_set | | | | | | u.a. Aufzugs-/Rolltor-Objekt @(−25200,0,−360) |
| 0x0180 | 0x11f4 | `Ifel_ck` `Ck(4,0x05)==1` | | | | | | 2× 20 `Sce_em_set` (Gegner-Roster) |

Eintritts-Spawns nach ROOM1040 (aus den inbound-Doors aller Raeume):
ROOM1020 → (−24000,0,16500) cut 6 · ROOM1030 → (−20083,0,−3559) cut 0 ·
ROOM1060 → (−21008,0,−13134) cut 5 · ROOM1080 → (−21936,0,−11000) cut 5.
Keiner davon liegt im Schalter-Rect x ∈ [−26000,−23900], z ∈ [−16000,−13800].

## Werkzeuge

* Probe: `re15_port/tests/unit/probe_room1040_switch.c` (in
  `re15_port/tests/unit/CMakeLists.txt` als Diagnose-Executable registriert, kein ctest).
  Aufruf: `probe_room1040_switch <raum-hex> <x> <z> <preflag 0|1> <press 0|1>`.
* Disasm: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis <addr> <n>`.

---

## STATUS 2026-08-06 — BEHOBEN (Fix-Plan vom Pruefer korrigiert)

Der Pruefer hat **beide** Fix-Stellen des Dossiers korrigiert; eingebaut ist die korrigierte
Fassung.

### Was eingebaut wurde

**1. Bank-5-Wisch — NICHT beim Raumladen, sondern am Ende jedes VM-Laufs.**
Das Dossier verortete den ganzen Wisch im Raum-Load. `FUN_8003ebf4` hat aber **XREF[2]**: der
zweite Caller ist der VM-Executor `FUN_8003f0a0` @0x8003f18c, unbedingt hinter der Slot-Schleife
(@0x8003f17c-88). Selbst nachdisassembliert:
```
8003f18c: jal 0x8003ebf4          ; unbedingt am Ende JEDES VM-Laufs
8003ebf4: addiu v0,zero,-1
8003ebfc: sh v0,4048(at)          ; 0x800b0fd0  work_vars[0]
8003ec04/0c/14: sh v0 -> 0x800b0fd2/d4/d6
8003ec1c: sw zero,4140(at)        ; 0x800b102c  Flag-Bank 5, WORT 1
```
Bank-5-Wort-1 (Bits 0x20..0x3F) ist also **Ein-Frame-Handshake**, kein Raum-Latch: der AOT-Scan
laeuft nach der VM (@0x8001ce1c) und setzt die Bits, sub01 liest sie im naechsten Frame, der
VM-Tail wischt sie. Haette man F1 wie beschrieben eingebaut, waeren aus ROOM1040s Gegner-Pool-Zonen
(Slots 5/6/7 = Bits 0x20/0x22/0x23) permanente Latches geworden.

Eingebaut: `g_game.flags[5][1] = 0;` am Ende von `scd_vm_tick()` (FUN_8003ebf4 @0x8003ec1c aus
FUN_8003f0a0 @0x8003f18c) **plus** `g_game.flags[5][0] = 0;` beim Raumladen (FUN_8003ecec
@0x8003ed74). Die Port-Reihenfolge passt: `scd_vm_tick()` (main.c) laeuft vor `re15_game_step()`
mit dem AOT-Scan.

**2. sub01-Reseed — braucht einen unbedingten Reset UND eine Init-Unterdrueckung.**
Der abgedruckte Schnipsel war wirkungslos (`scd_thread_start` verweigert einen aktiven Slot,
das Original re-initialisiert unbedingt: FUN_8003ee3c -> FUN_8003edec @0x8003ee1c-38). Der
unbedingte Reset allein liess 3 gruene Tests fallen, weil `scd_room_reenter` denselben
`scd_vm_tick()` fuer den Init-Lauf benutzt und sub00 geklobbert wird, bevor es laeuft.
Das Original hat dafuer ZWEI Call-Sites: Init `FUN_8003ef6c` ruft den Dispatcher direkt
(@0x8003f018), Gameplay `FUN_8003f038` reseedet erst und ruft dann (@0x8003f088).

Eingebaut: neue `scd_thread_reseed()` (unbedingt, ohne Log-Spam) + Schalter
`scd_vm_set_room_init()`, den `scd_room_reenter` und der Boot-Pfad um ihren Init-Lauf legen.
Der Einmal-Start von sub01 in Slot 2 und der `examine_poll_pending`-Sonderweg (beides
Port-Erfindungen) sind entfallen.

**Zusatzfund beim Live-Test:** der Boot-Pfad in `main.c` startet sub00 erst NACH seinem
Init-Tick (die Pre-Intro-Erkennung braucht dessen Ergebnis) — der Reseed haette es verdraengt,
bevor es sein erstes Opcode ausfuehrt (an ROOM1150 gemessen: sub00s Evt_exec spawnte keinen
Thread mehr). Behoben durch einen init-gegateten Tick direkt nach dem sub00-Start, genau wie
`scd_room_reenter` es macht.

### BEWUSST NICHT eingebaut

**B3-3 (`work_vars[0..3] = -1` am Frame-Ende).** Byte-true (dieselbe FUN_8003ebf4), gehoert aber
zu keinem der gemeldeten Fehler und wuerde die Bedeutung der Skript-Scratch-Variablen spielweit
aendern. Im Test messbar: 6 Assertions in `unit_scd_opcodes` (Calc/Save/Loop/Switch-Semantik)
fallen damit. Bleibt als zitierte, dokumentierte Divergenz stehen — separat anzugehen.

**B3-6 (Rect-Ecke+Ausdehnung).** Vom Dossier selbst als "OPTIONAL, nicht Teil von BUG 3"
eingestuft; beruehrt jede AOT-Zone im Spiel. Nicht angefasst.

### Verifikation

`probe_room1040_switch` ist jetzt ctest `unit_room1040_switch` und prueft den **vollen
Nutzer-Ablauf**:

| Schritt | vorher | nachher |
|---|---|---|
| SQUARE am Schalter | nichts (`msg active=0`, kein Thread) | `msg active=1 query=1` — Frage kommt |
| Raum verlassen + neu betreten | `msg active=1 query=1 fsm=1`, Spieler 31.400 Einheiten entfernt | `msg active=0`, `player_mode=0` |
| Flag(5,0x21) nach dem Frame | latcht dauerhaft | 0 (Ein-Frame-Handshake) |

Voller `ctest`: 113/113 gruen. Live-Boot gegen den HEAD-Baseline-Build frame-fuer-frame
identisch (F0..F300 `[walk]`-Spur + alle `[scd] thread start`).
