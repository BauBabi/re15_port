# BUG 2 — ROOM1030: „Cutscene mit den Zombies triggert komisch, man erhält die Kontrolle nicht (zurück)"

Datum: 2026-08-05 · Status: **RE abgeschlossen, Fix-Plan belegt, KEIN Engine-Code geändert**
Messsonde: `re15_port/tests/unit/probe_cut_1030.c` (Target `probe_cut_1030`, kein ctest-Gate)
Live-Lauf: `re15_pc.exe` mit `RE15_GOTO_ROOM=1030 RE15_FLAG_TRACE=1`

---

## 0. Kurzfassung

Zwei getrennte Port-Divergenzen erzeugen zusammen exakt die beiden gemeldeten Symptome:

| # | Divergenz | Symptom |
|---|---|---|
| **F1** | Der Port pollt `sub01` **nicht pro Frame**. Im Original wird Thread-Slot 1 **jeden Frame** auf `sub_scd[1]` zurückgesetzt (`FUN_8003f038` @0x8003f064/@0x8003f084). ROOM1030s Cutscene-Trigger **liegt komplett in sub01**. | Cutscene feuert **live nie**; sie feuert stattdessen beim **nächsten Raum-Betreten** (weil `scd_room_reenter` sub01 genau einmal ausführt und die Flags persistieren) → „triggert komisch" |
| **F2** | `platform/pc/main.c:3012-3025` leitet `player_mode = 2` aus `flag(1,27) \|\| flag(2,7)` ab. Im Original bedeuten diese Bits **etwas ganz anderes** (Letterbox / Inventar-Sperre / Pad-Maske) und fassen `DAT_800aca58` (das Spieler-Routinen-Register) **nie** an. | Der Spieler wird in einer reinen Kamera-Cutscene skript-eingefroren; und wenn ein Raumwechsel `flag(2,7)` gesetzt zurücklässt (Raum-Load löscht nur bank1), bleibt `player_mode` **für immer** auf 2 → „Kontrolle kommt nicht zurück" |

ROOM1030s gesamter SCD enthält **null** `Plc_*`-Opcodes (verifiziert über alle 1 main + 12 sub Skripte).
Das Skript nimmt dem Spieler die Kontrolle im Original **ausschließlich** über die Pad-Maske
`DAT_800ac768 &= 0xf000` (`FUN_80030444` @0x800304f4-0x8003051c), die an `flag(2,7)` hängt.

---

## 1. Das Original — ROOM1030.RDT byte-genau

`main_scd @0x1c5c` (1 Eintrag, Body @0x1c5e) · `sub_scd @0x1fd0` (12 Einträge)

### 1.1 main00 (@0x1c5e) — die drei Trigger-Zonen und die 20 Zombies

| Datei-Offset | Bytes | Bedeutung |
|---|---|---|
| 0x01cca | `2c 03 04 41 00 00 44b2 10dc 740e 0807 0500 2000 0100` | `Aot_set slot=3 sce=4 flags=0x41` Rect(x=-19900,z=-9200,w=3700,d=1800) → **Set flag(5,32)=1**, Pool-Maske `0x41 & 7 = 1` = **nur SPIELER** |
| 0x01cde | `2c 04 04 42 … 0500 2100 0100` | `slot=4 sce=4 flags=0x42` Rect(-14800,-22500,12000,2200) → **flag(5,33)=1**, Pool `2` = **nur GEGNER** |
| 0x01cf2 | `2c 05 04 42 … 0500 2200 0100` | `slot=5 sce=4 flags=0x42` Rect(-12900,-25300,9200,1100) → **flag(5,34)=1**, Pool `2` = **nur GEGNER** |
| 0x01de6 … 0x01e5e | `44 <slot> 16 0d 00 00 <b6> <slot> …` ×20 | `Sce_em_set` 20× Typ **0x16**, behavior **0x0d**, kill-Flag = Slot-Index |
| 0x01f98 | `06 00 1c 00 / 21 04 0f 00` | `if Ck(4,15)==0` → Aot_set 17 mit msg 0, sonst msg 1 |

### 1.2 sub00 (@0x1fe8) — Raum-Init (läuft im Original NUR im Load-Frame)

`Set(3,116)=1` @0x1fe8 („Raum betreten"), dann Umbau der 20 Entity-Positionen je nach `Ck(4,15)`,
zum Schluss `Evt_exec(0xff,0x18,sub11)` @0x2172 und `Evt_exec(0xff,0x18,sub02)` @0x2176, `Save(7,0)` @0x217a, **`Evt_end` @0x217e**.

### 1.3 sub01 (@0x2180) — **DER CUTSCENE-TRIGGER** (im Original per-Frame-Poll)

```
0x2180  06 00 20 00   Ifel_ck
0x2184  21 03 74 01   Ck(zone=3,bit=116,val=1)     ; Raum betreten (sub00)
0x2188  21 04 0f 00   Ck(zone=4,bit=15, val=0)     ; Cutscene noch NICHT gesehen
0x218c  21 05 20 01   Ck(zone=5,bit=32,val=1)      ; SPIELER stand in AOT3
0x2190  06 00 0e 00   Ifel_ck
0x2194  21 05 22 01   Ck(zone=5,bit=34,val=1)      ; ein GEGNER steht in AOT5 (am Fenster)
0x2198  22 04 0f 01   Set(4,15)=1                  ; Guard SOFORT setzen (Einmal-Schuss)
0x219c  04 ff 18 08   Evt_exec(slot=0xff, 0x18, sub08)   ; -> die Cutscene
0x21a0  08 00 / 08 00 / 01 00
```

Das `Set(4,15)=1` **unmittelbar vor** dem `Evt_exec` ist der Beweis, dass dieses Skript für einen
**Per-Frame-Poll geschrieben** ist: nur so verhindert der Guard, dass die Cutscene jeden Frame neu
gespawnt wird.

### 1.4 sub08 (@0x2764) — die Cutscene selbst

```
0x2764  22 02 07 01   Set(2,7)=1     ; -> DAT_800aca40 |= 0x01000000
0x2768  22 01 1b 01   Set(1,27)=1    ; -> DAT_800aca3c |= 0x10
0x276c  46 11 01 31 01 00 ff ff 00 00   Aot_reset(17, sce=1, msg=1)
0x2776  36 02 0c 00 …                   Se_on bank=12
0x2782  4b 00 09 / 4b 03 0a / 4b 04 0b / 4b 06 0c   Cut_replace ×4
0x278e  37 02 06 f7 / 37 03 06 f7        Sca_id_set ×2  (Kollision umbauen)
0x2796  24 05 00 00 / 24 07 00 00        Save(5,0) / Save(7,0)
0x279e  18 09  Gosub sub09 / 09 0a 05 00 Sleep(5) / Gosub sub09 / Sleep(20)
0x27aa  29 0c        Cut_chg(12)
0x27ac  2e 01 00     Work_set(kind=1,idx=0)          ; WORK = SPIELER
0x27b0  34 04 b7 02  Member_set(id=4, 695)           ; = rot_y  (Leon dreht sich zum Fenster)
        Gosub sub09 / Sleep(10) / Gosub sub09 / Sleep(15) / Sleep(20) / Sleep(180)
0x27c8  29 0b        Cut_chg(11)
        09 0a c8 00  Sleep(200)
0x27ce  29 05        Cut_chg(5)
0x27d0  3c 01        Cut_auto(1)
0x27d2  22 02 07 00  Set(2,7)=0     ; <- HIER kommt die Kontrolle zurück
0x27d6  22 01 1b 00  Set(1,27)=0    ; <- Letterbox-Balken fahren zu
0x27da  22 05 14 01  Set(5,20)=1    ; „Cutscene fertig" für sub02
0x27de  01 00        Evt_end
```

Gesamtdauer der Sleeps: 5+20+10+15+20+180+200 = **450 Ticks ≈ 15 s @30 Hz**.

**Es gibt in ROOM1030 KEIN `Plc_motion`/`Plc_dest`/`Plc_ret`** — Zähler über alle Skripte des Raums
für 0x3F/0x40/0x41/0x42/0x43/0x4A = **0**. Der einzige Spieler-Zugriff ist das `Member_set(4, 695)`
oben (Blickrichtung).

---

## 2. Was `flag(2,7)` und `flag(1,27)` im ORIGINAL wirklich tun

Flag-Bank-Tabelle `PTR_DAT_80074664` (dumpen: `re15_disasm.py table 0x80074664 14`):

```
[0] 0x800aca38   [1] 0x800aca3c   [2] 0x800aca40 (= g_pauseflags)   [5] 0x800b1028
```
Bit-Modell (byte-true, Port identisch): `mask = 0x80000000 >> (idx & 0x1f)`.
→ `Set(1,27)` = `0x800aca3c |= 0x10`, `Set(2,7)` = `0x800aca40 |= 0x01000000`.

### 2.1 `flag(1,27)` (= 0x800aca3c & 0x10) → **NUR die Letterbox**

```
FUN_80021a0c:
  80021a10  lw   v0, -13764(0x800b0000)   ; 0x800aca3c
  80021a24  andi v0, v0, 0x10
  80021a28  beq  v0, zero, 0x80021a58
  80021a40  sltiu v0, v0, 0xf0            ; Level 0x800b5568 += 0x10 bis 0xF0
```
Kein weiterer Leser dieses Bits. **Der Port macht das bereits richtig** (`re15_letterbox_tick`,
`fade_common.c:81`).

### 2.2 `flag(2,7)` (= 0x800aca40 & 0x01000000) → **Inventar-Sperre + Pad-Maske**

**(a) Inventar-Öffnen blockieren** — Haupt-Loop `FUN_8001c...`:
```
8001cd04  andi v0, a0, 0x40            ; a0 = 0x800aca3c (Menü schon offen?)
8001cd08  bne  v0, zero, 0x8001cdec    ; -> Block überspringen
8001cd0c  lui  v1, 0x100               ; 0x01000000
8001cd14  lw   v0, -13760(0x800b0000)  ; 0x800aca40
8001cd1c  and  v0, v0, v1
8001cd20  bne  v0, zero, 0x8001cdec    ; <-- flag(2,7) gesetzt => Status-Screen NICHT öffenbar
   … 0x8001cd94-0x8001cde8: Menü-Anforderung 0x800aca3c|0x8000 -> |0x40, 0x800aca40 |= 0xff000000
```

**(b) PAD-MASKE — das ist die eigentliche „Kontrolle weg"-Mechanik** — Pad-Leser `FUN_80030444`:
```
80030444  (Funktionsanfang: PadRead -> 0x800ac758, Remap-Tabelle 0x80073e1c -> 0x800ac768)
800304f0  lui  v0, 0x800b
800304f4  lw   v0, -13760(v0)          ; 0x800aca40
800304f8  lui  v1, 0x100               ; 0x01000000
800304fc  and  v0, v0, v1
80030500  beq  v0, zero, 0x80030520
8003050c  lw   v0, -14488(0x800b0000)  ; 0x800ac768 = VIRTUELLES HELD-Wort
80030514  andi v0, v0, 0xf000          ; <<< nur virtuelle Bits 12..15 bleiben stehen
8003051c  sw   v0, -14488(at)
80030520+ Flankenwort 0x800ac76c wird DARAUS abgeleitet (xor/and @0x80030550-7c)
```
Virtuelle Bit-Belegung (Tabelle `0x80073dbc`, im Port als `re15_pad_virtual_word`,
`pad_common.c:29-45`): Bits 0..3 = UP/RIGHT/DOWN/LEFT, 8/10 = R1, 9 = CROSS, 11 = L1,
**12/13 = Menü-LEFT/RIGHT, 14 = CONFIRM (SQUARE), 15 = CANCEL (CROSS)**.

→ `& 0xf000` streicht **Laufen, Drehen, Rennen, Zielen** und lässt **nur Dialog-/Menü-Bestätigung**
stehen. Genau das ist ROOM1030s Cutscene: Leon steht fest, sein Aktor läuft aber ganz normal weiter
(Idle-Anim, Routine 1). Beide Pad-Wörter 0x800ac768/0x800ac76c sind die, die der Spieler-Code liest
(30 Leseorte, u.a. 0x80031f4c/0x80031ff4/0x800322fc/0x80032410 = die Lauf-/Ziel-FSM).

### 2.3 Was `player_mode` im Original wirklich ist

`DAT_800aca58` = Spieler-Entity `+0x4` (Routinen-Register), gelesen von `FUN_80031c44` @0x80031c8c,
Dispatch `PTR 0x80073f90` = {0x800318f8, 0x80031de8, 0x80035af0, 0x800366bc, 0x80030660, 0x80036834}.
Geschrieben wird es von:
* `Plc_motion` (0x3F) `@0x80041bb0  sb v1(=4), 4(v0)` auf die **Work-Entity** — ist die Work-Entity
  der Spieler (Block @0x800aca54, +0x4 = 0x800aca58), so setzt das die Routine auf 4.
* `Plc_ret` (0x42) @0x80041f90 → 1.
* Raumwechsel `@0x8001cbdc  sb zero, -13736(at)` → 0.

**Kein einziger Schreiber ist ein Flag-Bit.** Die Ableitung `player_mode = flag(1,27)||flag(2,7)` hat
im Original kein Gegenstück.

### 2.4 Raum-Load löscht nur bank1

```
FUN_800396fc:
  8003970c  lw  v0, -13764(v0)   ; 0x800aca3c
  80039710  lui v1, 0xffff
  80039728  and v0, v0, v1       ; aca3c &= 0xffff0000
  80039730  sw  v0, -13764(at)
```
`0x800aca40` (bank2) wird **nicht** angefasst — im Original harmlos (das Bit maskiert nur das Pad,
und ein maskierter Spieler kann keine Tür erreichen), im Port fatal (siehe F2/§4).

### 2.5 Der Per-Frame-sub01-Poll

```
FUN_8003f038  (Per-Frame-SCD-Tick, gerufen @0x8001cdec)
  8003f040  lw   v0, -13760(v0)          ; g_pauseflags 0x800aca40
  8003f044  lui  v1, 0x200               ; 0x02000000
  8003f04c  bne  v0, zero, 0x8003f090    ; SCD-Pause-Gate
  8003f054  jal  0x8003ea3c              ; RNG-Advance
  8003f060  lw   v0, -14472(v0)          ; 0x800ac778 = aktuelle RDT
  8003f064  ori  a0, zero, 0x1           ; <<< THREAD-SLOT 1
  8003f06c  sb   zero, 16250(at)         ; 0x800b3f7a = Tabellenwahl 0
  8003f070  lw   v0, 68(v0)              ; RDT + 0x44 = SUB-SCD-Tabelle
  8003f07c  sw   v0, 16240(at)           ; 0x800b3f70
  8003f080  jal  0x8003ee3c              ; thread_start(slot=1, index=1)
  8003f084  ori  a1, zero, 0x1           ; <<< SUB-INDEX 1 = sub01
  8003f088  jal  0x8003f0a0              ; Dispatcher
```
`FUN_8003edec` @0x8003ee18-0x8003ee38 setzt `thread.pc = sub_base + table[a1]`, `active=1`.
Zum Vergleich der Raum-Load `FUN_8003ef6c`: @0x8003efa0 `lw v0,64(v0)` (RDT+0x40 = main) →
`FUN_8003ee3c(a0=0,a1=0)`; @0x8003efc4 `lw v0,68(v0)` (RDT+0x44) → `FUN_8003ee3c(a0=1,a1=0)` = sub00.

**Folge:** sub00 läuft nur im Load-Frame; ab dem ersten Gameplay-Frame ist Slot 1 permanent sub01,
das jeden Frame von vorn abläuft.

---

## 3. Was der PORT tut (gemessen, nicht modelliert)

### 3.1 F1 — sub01 wird nicht gepollt

* `engine/src/scd_room_setup.c:171-174` — sub01 wird **einmal** nach main00/sub00 in Slot 2 gestartet.
* `engine/src/scd_vm.c:519-529` — sonst nur, wenn `g_scd.examine_poll_pending` gesetzt ist; das setzt
  **ausschließlich** ein `sce=5`-EXAMINE-AOT (`aot_common.c:1051`). ROOM1030 installiert initial
  **keinen** sce-5-AOT (erst `sub02 +0x0014 Aot_reset(6, sce=5, …)` @0x21ba, das aber selbst hinter
  `Ck(5,20)==1` = „Cutscene schon gelaufen" hängt → zirkulär).

**Messung** (`probe_cut_1030`, Phasen 1-4):

```
[flags Entry           ] (3,116)=1 (4,15)=0 (5,32)=0 (5,33)=0 (5,34)=0 | cine=0 pmode=0
== Phase 2: Spieler in AOT-3-Rect ==
[flags nach AOT3       ] (5,32)=1  (5,33)=0 (5,34)=0
== Phase 3: Gegner-Slot 1 in AOT-5-Rect ==
[flags nach AOT5       ] (5,32)=1  (5,33)=0 (5,34)=1     <- beide Bedingungen erfüllt
== Phase 4: 900 Ticks ==
[flags t=0/150/…/750   ] (4,15)=0 … cine_active=0 pmode=0
[threads Ende] 10 11                                       <- sub08 NIE gestartet
```
Die Pool-Masken funktionieren korrekt (Spieler setzt nur 5,32; Gegner nur 5,34) — es fehlt
**ausschließlich** der Poll.

### 3.2 Der „komische" Trigger: Cutscene beim Raum-Betreten

`scd_room_reenter` führt sub01 einmal aus, und `g_game.flags` überleben den Raumwechsel. Also:
1. Erst-Besuch: Spieler läuft durch AOT3 → `flag(5,32)=1`; ein Zombie erreicht AOT5 → `flag(5,34)=1`.
   Keine Cutscene (kein Poll).
2. Raum verlassen, wieder betreten → sub01 läuft im Load → beide Flags stehen → **Cutscene startet
   im selben Frame wie der Türwechsel.**

**Messung** (`probe_cut_1030`, Szenario B):
```
##### SZENARIO B: Raum-Neubetretung mit stehenden Flags (5,32)+(5,34) #####
[flags direkt nach Raum-Load] (4,15)=1 … (1,27)=1 (2,7)=1 -> cine_active=1 | pmode=0 cam=9
[threads direkt nach Raum-Load] 10 11 12          <- sub08 läuft bereits
[flags B t=0  ] … pmode=2   cam=9
[flags B t=24 ] … pmode=2   cam=12
[flags B t=249] … pmode=2   cam=11
[flags B t=449] (5,20)=1 (1,27)=0 (2,7)=0 -> cine_active=0 | pmode=2 lb=14 cam=5
[flags B t=463] … pmode=0                          <- Kontrolle erst nach 450+14 Ticks
```

### 3.3 F2 — `flag(1,27)||flag(2,7)` ⇒ `player_mode = 2`

`platform/pc/main.c:3011-3035` (identisch `platform/psx/main.c:312`):
```c
int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
re15_letterbox_tick(re15_game_flag_get(1, 27));
if (cine_active) {
    g_scd.player_mode         = 2;    /* scripted */
    g_scd.letterbox_countdown = -1;
} else if (s_cine_was_active) {
    g_scd.letterbox_countdown = 15;
}
…
if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) g_scd.player_mode = 0;
```
`player_mode == 2` sperrt im Port die **komplette** Pad-Bewegung (`player_common.c:331`,
`:627`, `:641`), unterdrückt alle Nicht-CAM_SWITCH-AOTs (`aot_common.c:582`) und blockt Treppen
(`stair_common.c:286`). Das ist **strenger als das Original** (dort bleiben Bits 12..15 = Confirm/
Cancel/Menü-L/R stehen) und trifft in ROOM1030 einen Aktor, den das Skript gar nicht führt.

### 3.4 Der permanente Kontrollverlust (LIVE gemessen)

`scd_room_reenter` (`scd_room_setup.c:118-121`) löscht byte-true bank1 idx 16..31 — also
`flag(1,27)` — aber **nicht** bank2 (`flag(2,7)`), genau wie das Original @0x80039710-30.
Zusammen mit §3.3 heißt das: **jede** Unterbrechung eines `Set(2,7)=1`-Skripts durch einen Raumwechsel
(der `memset(&g_scd,…)` in `scd_room_reenter` tötet alle Threads, also auch das noch offene sub08 mit
seinem `Set(2,7)=0` @0x27d2) hinterlässt `flag(2,7)=1` — und `player_mode` wird ab da **in jedem Raum,
jeden Frame** wieder auf 2 gezogen. Nichts im Port löscht das Bit je wieder.

**Live-Beleg** (`RE15_GOTO_ROOM=1030 RE15_FLAG_TRACE=1`, `debug.log`):
```
[flag] z1/27 = 1
[flag] z2/7  = 1
[goto] queued room change -> ROOM1030 at F30
[room] PC loaded room1030.rdt (191528 bytes)
[flag] z1/27 = 0                        <- bank1 wird gelöscht
[cine] F300  z1/27=0 z2/7=1 cine=1 pmode=2 letterbox=-1
[cine] F600  z1/27=0 z2/7=1 cine=1 pmode=2 letterbox=-1
[cine] F900  z1/27=0 z2/7=1 cine=1 pmode=2 letterbox=-1
[cine] F1200 z1/27=0 z2/7=1 cine=1 pmode=2 letterbox=-1
[cine] F1500 z1/27=0 z2/7=1 cine=1 pmode=2 letterbox=-1
[cine] F1800 z1/27=0 z2/7=1 cine=1 pmode=2 letterbox=-1
```
Reproduktion in der Sonde (`probe_cut_1030`, Szenario C):
```
[flags C mitten in Cutscene]      (1,27)=1 (2,7)=1 -> cine=1 pmode=2
[flags C direkt nach Raumwechsel] (1,27)=0 (2,7)=1 -> cine=1 pmode=0
[flags C t=0 / 30 / 200 / 599]    (1,27)=0 (2,7)=1 -> cine=1 pmode=2
C-BEFUND: player_mode=2 (2 = SKRIPT = keine Kontrolle), flag(2,7)=1
```

---

## 4. Fix-Plan

### FIX-1 (F1): sub01 byte-true pro SCD-Tick neu starten

**Ort:** `engine/src/scd_vm.c` `scd_vm_tick()` (ersetzt den `examine_poll_pending`-Block Z. 519-529).
**Beleg:** `FUN_8003f038` @0x8003f064 (`ori a0,zero,0x1`), @0x8003f06c (`sb zero,0x800b3f7a`),
@0x8003f070 (`lw v0,68(v0)` = RDT+0x44), @0x8003f084 (`ori a1,zero,0x1`) → `FUN_8003ee3c(1,1)`.

```
vor der Thread-Schleife, JEDEN Tick:
    if (s_current_rdt && s_current_rdt->sub_scd[1])
        scd_thread_start(2, s_current_rdt->sub_scd[1]);
```
`examine_poll_pending` wird damit überflüssig (der Poll läuft ohnehin jeden Frame).

**Regressionsrisiko (hoch, breit) — gemessener Zensus über alle 240 RDTs:**
**111 Räume** haben ein nicht-leeres sub01; darin **348 `Evt_exec`-Aufrufe** gesamt.
Klassifikation (Opcode-Kontext der 3 Ops vor dem `Evt_exec`):

| Klasse | Anzahl | Bewertung |
|---|---|---|
| `Sce_key_ck` (0x51) direkt davor — Pad-HELD-gegated (Dev-/Debug-Hooks, z.B. ROOM10D0 @scd 0x00e4, ROOM1240 @scd 0x0012) | **232** | im Original **ebenfalls** per-Frame; kein neues Risiko, ABER siehe Slot-Pool unten |
| `Set(zone,bit,1)` als Guard direkt davor (ROOM1030 `Set(4,15)=1` @0x2198 vor `Evt_exec` @0x219c; ROOM1150 `Set(3,0x9d)=1` @scd 0x002a vor `Evt_exec` @scd 0x002e) | **49** | selbst-limitierend, unkritisch |
| Rest (Guard steckt in `Ck`/`Cmp`-Prädikat oder im gerufenen Sub) | **67** in 34 Räumen (10B1, 10D0/1, 1190/1, 11B0/1, 11C0, 11F0/1, 1230/1, 2060/1, 20A0/1, 3000/1, 3090/1, 30C0, 4000/1, 40A0/1, 5090/1, 50B0, 50C0/1, 5100/1, 6030/1) | **muss je Raum geprüft werden**, bevor FIX-1 eingebaut wird |

Der Original-`Evt_exec(0xff,…)` (@0x8003f2a0 → `FUN_8003ee3c` @0x8003ee60-ee9c) hat **keinen**
„läuft schon"-Test; er nimmt den ersten freien Slot **2..8** (Player-Tabelle). Der Port nimmt
**10..23** (`scd_vm.c:906-916`). Bei einem per-Frame-Poll mit gehaltener Taste füllt sich der Pool
also anders → das ist der Punkt, an dem FIX-1 und die Slot-Pool-Divergenz (§5.4) **zusammen**
verifiziert werden müssen.

Weitere Muster in sub01 (unkritisch, idempotent):
* `Ck(...)` → `Aot_reset(...)`: ROOM1130, ROOM1140, ROOM1051, ROOM2030 …
* `Ck(...)` → `Set(...)`: ROOM10C0/1120/11A0/1200/5000 …

Zweiter Nebeneffekt: im Original stirbt sub00 mit dem Slot-1-Reset nach dem Load-Frame; der Port
lässt sub00 in Slot 1 weiterlaufen. Für ROOM1030 egal (sub00 endet mit `Evt_end` @0x217e), für
andere Räume **offen**.

**Absicherung:** `ctest` komplett (110 Tests), speziell `unit_scd_aot_opcodes`, `test_scd_flag_fade`,
`test_room1150_itembox`, `probe_marvin_10d0`, `probe_zreentry_10d0 A/B`, `unit_door_1170_lock`,
`unit_stair_1170_regression`; zusätzlich eine neue Probe je Muster (1150 Work-Var-Poll,
1130 Aot_reset-Poll, 10D0 `Sce_key_ck`-Poll) plus `probe_cut_1030` als Positiv-Nachweis
(Cutscene feuert live, sobald 5,32 & 5,34 stehen — **nicht** erst beim Raum-Load).

### FIX-2 (F2): `flag(2,7)` byte-true als Pad-Maske + Inventar-Sperre, NICHT als `player_mode`

**Belege:** Pad-Maske `FUN_80030444` @0x800304f4/@0x800304f8/@0x80030514/@0x8003051c;
Inventar-Sperre @0x8001cd0c/@0x8001cd14/@0x8001cd1c/@0x8001cd20;
Letterbox `FUN_80021a0c` @0x80021a24 (nur `flag(1,27)`);
`player_mode` = Entity+0x4, geschrieben von `Plc_motion` @0x80041bb0 / `Plc_ret` @0x80041f90 /
Raumwechsel @0x8001cbdc.

**Stufe A — minimal, entschärft den permanenten Lock sofort:**
`cine_active` **nur** aus `flag(1,27)` bilden (`main.c:3012`, `platform/psx/main.c:312`).
Begründung: `flag(1,27)` wird beim Raum-Load byte-true gelöscht (@0x80039730 bzw.
`scd_room_setup.c:118-121`), `flag(2,7)` nicht. Damit ist der unrettbare Zustand weg, und für
ROOM1030/1170/1240 ändert sich nichts an der Cutscene-Erkennung (alle drei setzen **beide** Bits;
ROOM1030 sub08 @0x2764/@0x2768, ROOM1240-Intro live gemessen `[flag] z1/27=1` + `[flag] z2/7=1`).
*Risiko:* ein Skript, das nur `Set(2,7)` ohne `Set(1,27)` benutzt, würde den Spieler nicht mehr
einfrieren — Zensus über alle RDTs vor dem Einbau nötig.

**Stufe B — byte-true (das eigentliche Ziel):**
1. `flag(2,7)` → nach dem Pad-Remap (`re15_pad_virtual_word`, `pad_common.c`) `held &= 0xf000` und
   das daraus abgeleitete Edge-Wort ebenso (@0x80030514 + @0x80030550-7c).
2. `flag(2,7)` → Status-Screen/Menü-Öffnen sperren (`re15_menu_toggle`-Pfad; @0x8001cd20).
3. `player_mode` **nicht** mehr aus Flags ableiten, sondern aus dem Routinen-Register: `Plc_motion`
   (0x3F) / `Plc_dest` (0x40) setzen es auf 4 **wenn die Work-Entity der Spieler ist**
   (@0x80041bb0 `sb 4, 4(work_entity)`), `Plc_ret` auf 1 (@0x80041f90), Raumwechsel auf 0
   (@0x8001cbdc — im Port bereits `room_common.c:140`).

**Regressionsrisiko Stufe B (hoch):** Das Helipad-Intro (ROOM1170 sub02) und das Pre-Intro
(ROOM1240) verlassen sich heute auf den Flag-Proxy. Im Live-Log dieser Session lief
`[scd] Plc_motion(entity=0, motion=15, flags=0x00) → slot=1` — die Plc-Opcodes zielen dort auf
**Aktor-Slot 1** (Elliot), nicht auf den Spieler. Ob Leon im Intro überhaupt je ein `Plc_*` bekommt,
ist **OFFEN**; ohne das würde Stufe B ihn im Intro laufen lassen. Vor Stufe B daher zwingend:
`Work_set`/`Plc_*`-Zensus für ROOM1170/1240/1150 + Durchlauf ab EXE-Start
(`RE15_NO_INTRO=1`, Title → NEW GAME → 1240 → 1170), Skill `re15-port-visual-verify` (gdigrab).

**Absicherung:** `ctest` komplett; `probe_cut_1030` Szenario C muss danach `player_mode=0` liefern;
Intro-Durchlauf ab Boot mit `RE15_FLAG_TRACE=1` (die `[cine]`-Zeile muss nach dem Intro `pmode=0`
zeigen); `probe_door_1170`, `probe_stair_1170`, `test_msg_dialog`, `test_item_modal`.

---

## 5. Offene Punkte (ehrlich als OFFEN markiert)

1. **Bewegen sich die 20 Typ-0x16-Zombies im Port überhaupt bis in AOT4/AOT5?** Die Sonde hat den
   Gegner von Hand gesetzt. Ohne AI-Lauf ist unbelegt, ob `flag(5,34)` im echten Spiel je durch
   einen Zombie gesetzt wird. Nächster Schritt: `probe_cut_1030` um `re15_enemy_ai_run_all` +
   EM016-Bank erweitern und die Zombie-Trajektorien über ~1200 Ticks loggen.
2. **`sub04`/`sub06`/`sub09` — die Zombie-Choreografie.** `Member_cmp(id=15, ==, 4/5)` und
   `Member_set2(id=16, …)` (Datei 0x24b2 / 0x2754 / 0x27e0). Welche Entity-Felder Member-ID 15 und
   16 im Port sind und ob `Calc`-Ops 5/6 (`+`/`&`) byte-true sind, wurde hier **nicht** geprüft.
3. **`Sce_bgmtbl_set`/0x59** in sub09 (`59 05 04 01`): der Port hat 0x59 als `op_flag_set2`
   („indexed flag SET/CLEAR/TOGGLE", `scd_vm.c:106`) registriert. Ob das für die drei Operanden hier
   stimmt, ist ungeprüft.
4. **`Evt_exec(0xff)`-Slot-Pool:** Original nimmt Slot 2..8 (Player-Tabelle, @0x8003ee60-ee9c) bzw.
   10..12 (Event-Tabelle, @0x8003eea8-eef0); der Port nimmt 10..23 (`scd_vm.c:906-916`). Divergenz
   belegt, Symptom hier nicht nachgewiesen.
5. **`Cut_replace`/`Sca_id_set` in sub08** (Datei 0x2782/0x278e) — Handler existieren im Port
   (`s_op_table[0x4B]`, `[0x37]`), byte-true-Verifikation der ROOM1030-Operanden steht aus.
6. **sub00-Lebensdauer:** Original tötet Slot 1 (sub00) ab dem ersten Gameplay-Frame; der Port lässt
   sub00 laufen. Auswirkung außerhalb ROOM1030 ungeprüft.

---

## 6. Reproduktion

```bash
# Sonde bauen + laufen (deterministisch, alle drei Szenarien)
PATH=/c/msys64/mingw64/bin:$PATH cmake -S re15_port -B re15_port/build -G Ninja \
     -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON
PATH=/c/msys64/mingw64/bin:$PATH cmake --build re15_port/build --target probe_cut_1030
./re15_port/build/tests/unit/probe_cut_1030.exe

# Live (permanenter player_mode=2 nach Raumwechsel mit stehendem flag(2,7))
cd re15_port/build/platform/pc
RE15_NO_INTRO=1 RE15_PSELECT_AUTO=1 RE15_GOTO_ROOM=1030 RE15_FLAG_TRACE=1 ./re15_pc.exe
# -> debug.log:  [cine] F1800 z1/27=0 z2/7=1 cine=1 pmode=2

# SCD-Dump
python <scratchpad>/scddis.py re15_port/shared_assets/PSX/STAGE1/ROOM1030.RDT
# Disasm
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x8003f038 30
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis 0x80030444 50
python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py table 0x80074664 14
```

---

## STATUS 2026-08-06 — BEHOBEN

### Eingebaut

**B1030-1 (per-Frame-sub01-Reseed)** — gemeinsam mit BUG 3, Details in
`analysis/bug_room1040_switch.md`. Der Pruefer hat den abgedruckten Einzeiler korrigiert
(Slot 1 statt 2, unbedingter Reset statt `scd_thread_start`, plus Init-Unterdrueckung).

**B1030-3 (permanenter Kontrollverlust)** — aber NICHT als "Stufe A". Der Pruefer hat Stufe A
widerlegt: von 240 RDTs enthalten 16 ein `Set(2,7)=1` OHNE begleitendes `Set(1,27)=1`
(ROOM1100/1101, 11F0/11F1, 1200/1201, 2030/2031, 3050/3051, 5050/5051, 50D0/50D1, 6040/6041) —
deren Cutscenes haetten den Spieler-Freeze komplett verloren.

Stattdessen: **`re15_game_flag_set(2, 7, 0)` beim Raum-(Re)Load** (`scd_room_reenter`, neben der
schon vorhandenen Bank-1-Maske). Begruendung: `flag(2,7)` hat im Port GENAU EINEN Leser — den
`cine_active`-Proxy in `platform/{pc,psx}/main.c` (repo-weiter grep). Es ist damit die
Port-Speicherstelle fuer "der Spieler ist skriptgefuehrt", im Original das Routinen-Register
`DAT_800aca58`, das die Transitions-FSM @0x8001CBDC `sb zero,0x800aca58` beim Raumwechsel
UNBEDINGT nullt. `flag(1,27)` loeschte der Raum-Load schon byte-true mit (@0x80039730); jetzt
folgen beide Haelften des Proxys derselben Regel wie das Original. Der Freeze *innerhalb* eines
Raums bleibt unveraendert erhalten.

### BEWUSST NICHT eingebaut

**B1030-2 Stufe B (player_mode aus dem Routinen-Register).** Vom Pruefer als PLAUSIBLE mit einem
nachweislich falschen Teilfakt eingestuft (DAT_800aca58 hat 28 Xrefs, nicht 3 — u.a. der
Player-Damage-Handler FUN_80012d60 @0x80012ebc/ef4 und ~20 Schreiber in der Spieler-FSM selbst).
Dazu die offene Frage, ob Leon im ROOM1240-/ROOM1170-Intro ueberhaupt je ein `Plc_*` bekommt —
falls nicht, liefe er dort frei. Zu riskant fuer einen Bugfix-Durchgang.

**B1030-4 (Pad-Maske `DAT_800ac768 &= 0xf000` + Inventar-Sperre).** Der Pruefer hat gezeigt, dass
der vorgeschlagene Schritt 1 im Port **wirkungslos** waere: die Spieler-FSM liest das ROHE Pad
(`re15_player_tick(c->cam_view, c->pad_current)`), nicht das virtuelle Wort. Ein `held &= 0xf000`
nach dem Remap wuerde die Bewegung gar nicht sperren. Bleibt als dokumentierte Luecke.

**B1030-5 (Evt_exec-Slot-Pool 2..8/10..12).** Die Port-Erweiterung auf 24 Slots wurde bewusst
gemacht, weil ROOM1170 waehrend des Helipad-Kinos 8+ gleichzeitige Event-Subs feuert. Eine
Rueckkehr kann genau diese Regression wiederholen. Nicht angefasst.

### Verifikation (`probe_cut_1030`)

| | vorher | nachher |
|---|---|---|
| Trigger (Zombie betritt AOT-5-Zone) | `(4,15)` bleibt 900 Ticks 0, sub08 startet nie | feuert sofort: `(4,15)=1`, `cine_active=1`, Thread 12 = sub08 |
| Cutscene-Ende | — | t=391 Latches weg, t=405 `pmode=0` — Kontrolle zurueck |
| Szenario C (Raumwechsel mitten in der Cutscene) | `flag(2,7)=1`, `pmode=2` ueber 600 Frames, unrettbar | `cine_active=0`, `pmode=0` ab dem Raumwechsel |

Voller `ctest`: 113/113 gruen.
