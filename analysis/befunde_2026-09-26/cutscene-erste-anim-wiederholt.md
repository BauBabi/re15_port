# Cutscene nach Raumwechsel: die erste Animation wiederholt sich — die Eintritts-Pose wird als Dauer-Motion geparkt und vom Modulo-Zweig zyklisch abgespielt

## 1. Was der Nutzer meldet / was ich gemessen habe

Nutzer: "Bei den Charakteren - Leon, Ada etc. wiederholt sich am Anfang immer die 1.
Animation, wenn sie eine Cutscene nach dem Raumwechsel haben. So zum Beispiel bei Leon
ROOM 1170 nach dem durchlaufen der Tuer, oder auch bei Leon/Ada wenn sie Room 1050
betreten und die Cutscene startet."

Vorrunde: `analysis/befunde_2026-09-21/ada-eintritt-1050.md` (Runde 18) hat EINE Doppelung
in 1050 behoben (Spawn-Saat `entity+0x94` fuer Typ 0x42 = 2 statt Platzhalter 0). Der neue
Report ist BREITER ("Leon, Ada etc.", "immer", ROOM1170 nach der Tuer). Die hier gefundene
Ursache ist eine ANDERE und betrifft den SPIELER.

### 1.1 Erste Spur (spaeter WIDERLEGT, steht bewusst hier)

Beide genannten Raeume fahren in ihrer Cutscene dasselbe SCD-Muster: derselbe Clip zweimal,
beim zweiten Mal mit gesetztem Rueckwaerts-Bit. Ich hielt das zuerst fuer die Ursache
(„Rueckwaerts greift nicht, also laeuft die Animation zweimal vorwaerts"). Lauf C in §1.2
widerlegt das.

ROOM1050.RDT (`re15_port/shared_assets/PSX/STAGE1/ROOM1050.RDT`), sub03 (= die Cutscene,
gestartet aus sub00 @Datei 0x00CA2 `Evt_exec 04 ff 18 03`):

    Datei 0x00DDA  3f 00 0f 00   Plc_motion(0, 15, 0)
    Datei 0x00DDE  09 0a 1e 00   Sleep(30)
    Datei 0x00DE2  3f 00 0f 00   Plc_motion(0, 15, 0)     <-- derselbe Clip 15 nochmal
    Datei 0x00DE6  43 00 80 00   Plc_flg(0, 0x80, 0)      <-- Bit 0x80 = RUECKWAERTS
    Datei 0x00DEA  09 0a 1f 00   Sleep(31)

ROOM1170.RDT, sub02 (Boot-Cutscene) — vier solcher Paare:

    Datei 0x01472  3f 00 0f 00 / 0x01476 Sleep(50) / 0x0147A 3f 00 0f 00 / 0x0147E 43 00 80 00
    Datei 0x01492  3f 00 14 00 / 0x01496 Sleep(50) / 0x0149A 3f 00 14 00 / 0x0149E 43 00 80 00
    Datei 0x014AA  3f 00 10 00 / 0x014AE Sleep(25) / 0x014B2 3f 00 10 00 / 0x014B6 43 00 80 00
    Datei 0x01542  3f 00 0f 00 / 0x01546 Sleep(30) / 0x0154A 3f 00 0f 00 / 0x0154E 43 00 80 00

ROOM1170.RDT, sub14 (= der Tuer-Eintritts-Pfad, den der Nutzer nennt; gestartet aus sub00
@Datei 0x0141E `Evt_exec 04 ff 18 0e`):

    Datei 0x01762  3f 00 12 00   Plc_motion(0, 18, 0)
    Datei 0x01782  3f 00 12 00   Plc_motion(0, 18, 0)
    Datei 0x01786  43 00 80 00   Plc_flg(0, 0x80, 0)

### 1.2 GEMESSEN — der Pose-Strom, Bild fuer Bild

Werkzeug: `RE15_ANIM_TRACE=<datei>` — der einzige Ort im Port, an dem eine Pose aufgeloest
wird (`re15_compute_actor_kf`, anim_select_common.c:64). Jede Zeile =
`Bild Aktor Typ motion clip fc cur slot fc_clock loco reverse`. Echte exe
`re15_port/build/platform/pc/re15_pc.exe` (Stand 2026-09-26 12:08), echter Renderpfad,
kein AUTOSHOT / kein Softwarerenderer. **Nichts neu gebaut.**

**Lauf A — ROOM1050, echter Eintritt.**
`RE15_NO_INTRO=1 RE15_TITLE_SHOT=… RE15_AP_CUTSCENE_CLICK=1 RE15_DEBUG_JUMP=1050@gp
RE15_SET_FLAG=3:0x6e` (Flag 3/110 ist das Tor, an dem ROOM1050 sub00 @Datei 0x00C8A haengt
und Ada spawnt + sub03 startet). Spieler = Aktor 0; der Bildzaehler springt beim
Raumwechsel auf 0:

    F0..F5    mo=210 clip=1 fc=16 cur=0      slot=0        Blende, eingefroren
    F6..F20   mo=210 clip=1 fc=16 cur=1..15  slot=1..15    1. Durchlauf
    F21..F36  mo=210 clip=1 fc=16 cur=16..31 slot=0..15    2. DURCHLAUF
    F37..F52  mo=210 clip=1 fc=16 cur=32..47 slot=0..15    3. DURCHLAUF
    F53..F55  mo=210 clip=1 fc=16 cur=48..50 slot=0..2     4. Durchlauf beginnt
    F56       mo=105  (die Cutscene uebernimmt: sub03 Plc_dest)

**Die Eintritts-Animation laeuft dreieinhalb Mal.** Das ist woertlich der Nutzer-Befund.

**Lauf B — ROOM1170, Leon nach dem Durchlaufen der Tuer.** Derselbe Aufbau plus
`RE15_FIRE_AOT=0@500#1170` (Tuer-Slot 0 = der Selbst-Reenter, der sub14 startet — derselbe
Pfad, den auch das Hineinlaufen nimmt):

    F501..F506 mo=210 clip=1 fc=16 cur=0      slot=0        Blende, eingefroren
    F507..F521 mo=210 clip=1 fc=16 cur=1..15  slot=1..15    1. Durchlauf
    F522..F525 mo=210 clip=1 fc=16 cur=16..19 slot=0..3     2. DURCHLAUF
    F526       mo=18   (sub14 Plc_motion(0,18,0) @Datei 0x1762)

Zustandsspur desselben Laufs (`RE15_STATE_LOG`) an denselben Bildern:

    F501 mo=210 pf=00000007 pm=0
    F505 mo=210 pf=FF000007 pm=2     <- die Uebergangsmaske 0xFF000000 steht
    F510 mo=210 pf=00000007 pm=2     <- Maske weg, der Clip laeuft los
    F525 mo=210 pf=00000007 pm=2
    F526 mo=18  pf=00000007 pm=2

**Lauf C — Gegenprobe: die Plc_motion/Plc_flg-PAARE sind im Port KORREKT.**
Derselbe Trace, Leon, ROOM1170 sub02 @0x1542 bzw. sub14 @0x1762/0x1782:

    sub02  F308..F327  clip15 slot 0..19  rev=0   vorwaerts, EINMAL
           F328..F357  clip15 slot 19     rev=0   HALTEN
           F358..F376  clip15 slot 18..0  rev=1   RUECKWAERTS, EINMAL
           F377..      clip15 slot 0      rev=1   HALTEN
    sub14  F526..F545  clip18 slot 0..19  rev=0
           F546..F615  clip18 slot 19     rev=0
           F616..F634  clip18 slot 18..0  rev=1

Das Rueckwaerts-Bit greift also. §1.1 ist damit widerlegt.

Einzige Rest-Abweichung im Paar (1 Bild, kein Doppel): der ZWEITE Plc_motion rendert sein
erstes Bild schon bei `cur=1` (Slot 18 statt 19), weil `re15_actor_set_motion`
(`include/re15_actor.h:1047`) den Vorhalte-Tick `motion_init_delay = 1` nur setzt, wenn
sich die Clip-Nummer AENDERT — bei `Plc_motion(15)` auf `Plc_motion(15)` tut sie das nicht.
Das Original kennt diese Bedingung nicht (§2.5).

### 1.3 Wann tritt es auf — und warum genau bei einer Cutscene

Im normalen Spiel ersetzt der Pad-/Idle-Zweig (`want_motion`, player_common.c:1085 ff.) die
Eintritts-Pose binnen ein bis zwei Bildern. Bei `player_mode == 2` (Skript-Szene) laeuft
dieser Zweig NICHT, und der pm-2-Zweig (player_common.c:1148) holt nur die Lauf-Sentinels
105/100 in den Idle zurueck, NIE die 210. Genau deshalb bleibt die Eintritts-Pose nur dann
lange genug stehen, um sichtbar zu zyklieren, **wenn nach dem Raumwechsel eine Cutscene
startet** — exakt die Bedingung, die der Nutzer nennt.

## 2. Das Original — mit Adressen und Bytes

Alles hier selbst aus `info/Re1.5/PSX.EXE` disassembliert
(`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`), nicht aus einem Decompilat.

### 2.1 Der Opcode-Tabelleneintrag (damit die Handler-Adresse belegt ist, nicht zitiert)

Rohwort `90 1b 04 80` bei Datei-Offset 0x64DA4 = RAM 0x800745A4 = Tabelleneintrag
Opcode 0x3F -> **0x80041b90** (Tabellenbasis 0x800744A8 = 0x800745A4 - 0x3F*4).
Nachbarn: [0x40] 0x80041be4, [0x41] 0x80041e98, [0x42] 0x80041f88 (Plc_ret),
[0x43] **0x80041fb8** (Plc_flg), [0x44] 0x800420a0 (Sce_em_set).

### 2.2 Plc_motion-Handler @0x80041b90 — schreibt +0x95 NICHT

    80041b90: lw   v0,28(a0)        ; v0 = thread pc
    80041b98: lbu  a2,1(v0)         ; a2 = pc[1]  (Entity-Byte)
    80041b9c: lhu  a1,2(v0)         ; a1 = pc[2..3] als 16 Bit
    80041ba0: lw   v0,340(a0)       ; v0 = thread+0x154 = WORK-Entity
    80041ba4: ori  v1,zero,0x4
    80041ba8: sb   a1,148(v0)       ; +0x94 = Clip (low byte)
    80041bac: srl  a1,a1,8
    80041bb0: sb   v1,4(v0)         ; +0x04 = 4  (Kommandowort)
    80041bb4: sb   zero,6(v0)       ; +0x06 = 0  (PHASE)
    80041bb8: sb   zero,7(v0)       ; +0x07 = 0
    80041bbc: sh   zero,456(v0)     ; +0x1C8 = 0
    80041bc0: sh   zero,458(v0)     ; +0x1CA = 0
    80041bc4: sb   a2,5(v0)         ; +0x05 = pc[1]
    80041bc8: sh   a1,452(v0)       ; +0x1C4 = pc[3]  (Anim-Flags, UEBERSCHRIEBEN)
    80041bdc: jr   ra

Kein Store auf +0x95 — der Clip-Cursor wird vom Handler nicht genullt.

### 2.3 Plc_flg-Handler @0x80041fb8 — Bit 0x80 landet in +0x1C4

    80041fc0: lbu  a1,1(v0)              ; pc[1] = Unter-Opcode
    80041fc4: lhu  a2,2(v0)              ; pc[2..3] = Maske
    80041fc8: beq  a1,v1(=1),0x80042000  ; 1 -> SETZEN
    80041fd0: beq  a1,zero,0x80041fec    ; 0 -> ODER
    80041fdc: beq  a1,v0(=2),0x8004200c  ; 2 -> dritter Zweig
    80041fec: lw   v1,340(a0)
    80041ff4: lhu  v0,452(v1)
    80041ffc: or   v0,v0,a2               ; +0x1C4 |= Maske   <<< Plc_flg(0,0x80,0)
    80042008: sh   a2,452(v0)             ; (Zweig 1: setzen)

### 2.4 Der Spieler-Dispatcher: Kommandowort -> Handler

    80031c54: lw   a0,-13760(a0)     ; a0 = g_pauseflags
    80031c78: bltz a0,0x80031da8     ; Bit 0x80000000 gesetzt -> KEIN cmd-Handler
    80031c8c: lbu  v1,-13736(v1)     ; 0x800aca58 = Kommandowort +0x04
    80031c94: sll  v1,v1,2
    80031ca4: addiu at,at,16272      ; 0x80073f90
    80031cac: lw   v0,0(at)
    80031cb4: jalr v0

Tabelle @0x80073f90: [0] = **0x800318f8** (Eintritt/Uebergang), [1] 0x80031de8,
[4] 0x80030660 (Plc-Executor), [6] 0x800368c0.

Der Plc-Executor 0x80030660 indiziert ueber +0x05 die Tabelle @0x80073e30:

    8003068c: lbu  v0,-13735(v0)     ; 0x800aca59 = +0x05
    80030694: sll  v0,v0,2
    8003069c: addiu at,at,15920      ; 0x80073e30
    800306a4: lw   v0,0(at)
    800306ac: jalr v0

@0x80073e30: [0] = **0x80050cb8**, [1] 0x80050ddc, [2] 0x80050f00, [3] 0x80051024,
[6] 0x800517f0. Alle ausgelieferten Plc_motion tragen pc[1] = 0 -> 0x80050cb8.

### 2.5 Pose-FSM @0x80050cb8 — WO und WIE OFT +0x95 genullt wird

    80050cc8: lbu  v1,6(a0)              ; PHASE (+0x06)
    80050cd0: beq  v1,v0(=1),0x80050d34  ; Phase 1 -> direkt posieren
    80050cd4: ori  a3,zero,0x200
    80050cdc: beq  v0,zero,0x80050dcc    ; Phase >= 2 -> ENDE (HALTEN, kein anim_set)
    80050cec: sb   v0(=1),6(a0)          ; PHASE := 1
    80050cfc: sb   v0(=7),143(v1)        ; +0x8F = 7  (Crossfade-Saat)
    80050d0c: sb   zero,149(v0)          ; +0x95 := 0   <<< EINZIGER RESET, NUR IN PHASE 0
    80050d24: andi v0,v0,0x40            ; +0x1C4 & 0x40 -> +0x8F = 0 @0x80050d30
    80050d40: lw   a0,384(v0)            ; +0x180 = Skelett
    80050d44: lbu  a2,452(v0)            ; +0x1C4 low byte
    80050d48: lw   a1,388(v0)            ; +0x184 = EDD
    80050d4c: jal  0x8001f314            ; anim_set(skel, edd, reverse, 0x200)
    80050d50: srl  a2,a2,7               ; a2 = Bit 0x80 = RUECKWAERTS
    80050d54: bne  v0,zero,0x80050d98    ; Rueckgabe != 0 -> Clip zu Ende
    80050da4: sb   v0(=2),6(v1)          ; PHASE := 2 (HALTEN)
    80050dbc: andi v0,v0,0x4             ; Bit 0x04 = LOOP
    80050dc8: sb   v0(=1),6(v1)          ;   -> PHASE := 1 statt 2

Der Reset von +0x95 haengt allein an PHASE == 0, und Plc_motion setzt die Phase auf 0
(@0x80041bb4). Nach JEDEM Plc_motion — auch bei gleicher Clip-Nummer — wird der Cursor
also genau einmal genullt und im selben Bild posiert.

### 2.6 Der Richtungs-Mechanismus @0x8001f314 — Spiegelung des INDEX

    8001f324: lbu  v0,148(t0)            ; +0x94 = Clip
    8001f334: lw   v0,0(a2)              ; low16 = Framezahl (t1), high16 = Offset
    8001f338: beq  t2,zero,0x8001f358    ; t2 = Reverse-Argument
    8001f344: lbu  v0,149(t0)            ; +0x95
    8001f34c: subu v0,t1,v0
    8001f354: addiu v0,v0,-1             ; index = Framezahl - Cursor - 1
    8001f358: lbu  v0,149(t0)            ; vorwaerts: index = Cursor
    8001f36c: sw   a2,360(t0)            ; +0x168 = Zeiger auf das Frame-Wort

### 2.7 Der Cursor-Vorschub @0x8001f610 — im POSE-Aufruf, nicht in einem eigenen Pass

Ende von FUN_8001f3bc (Keyframe-Integrator, von 0x8001f314 gerufen):

    8001f610: lbu  v0,149(v1)       ; +0x95
    8001f618: addiu v0,v0,1
    8001f61c: sb   v0,149(v1)       ; +0x95 += 1
    8001f624: sltu v0,v0,s4         ; s4 = Framezahl
    8001f628: bne  v0,zero,0x8001f640  ; noch drin -> Rueckgabe 0
    8001f63c: sb   zero,149(v1)     ; sonst +0x95 := 0 und Rueckgabe 1 (Clip zu Ende)

Eigener Zensus ueber ALLE 57 Zugriffe auf Offset 149 in PSX.EXE: genau ZWEI Stellen
inkrementieren (0x8001F61C und 0x8001FB58, die zweite in der Marker-/Tween-Schwester
FUN_8001f8b4); 45 sind `sb zero,149(v0)` in Phase-0-Zweigen von FSMs. **+0x95 bewegt sich im
Original ausschliesslich innerhalb eines anim_set-Aufrufs.**

### 2.8 Reihenfolge der Subsysteme im Haupt-Loop @0x8001cdec..0x8001ce2c

    8001cdd4-e8  g_pauseflags |= 0xFF000000       (Uebergangs-Maske)
    8001cdec     jal 0x8003f038   SCD-VM (Gameplay-Lauf, mit sub01-Reseed)
    8001ce04     jal 0x8001a50c   Entity-/KI-Schleife
    8001ce0c     jal 0x80031c44   SPIELER-Kommando-Dispatcher
    8001ce2c     jal 0x80019e20   Action-Driver / Keyframe-Integration

### 2.9 Raum-Init FUN_8003ef6c — EIN Dispatch, KEIN sub01-Reseed

    8003efa0 lw v0,64(v0)   RDT+0x40 = main_scd -> 8003efb0 jal FUN_8003ee3c(slot 0)
    8003efc4 lw v0,68(v0)   RDT+0x44 = sub_scd  -> 8003efd4 jal FUN_8003ee3c(slot 1)
    8003efdc jal 0x8001a4c0                      Entity-Array
    8003f018 jal 0x8003f0a0                      <-- VM-Dispatcher DIREKT, ohne Reseed

Gegenstueck, Gameplay-Lauf FUN_8003f038:

    8003f040-4c  Pause-Bit 0x02000000 -> sofortiges Ende
    8003f064 ori a0,zero,0x1 / 8003f070 lw v0,68(v0) / 8003f084 ori a1,zero,0x1
    8003f080 jal 0x8003ee3c   = Slot 1 JEDES Bild auf sub01 zuruecksetzen
    8003f088 jal 0x8003f0a0   = Dispatch

### 2.10 DIE EINTRITTS-POSE: cmd-0-Handler @0x800318f8, Kern @0x80031bf8

Der Raumwechsel setzt das Kommandowort DAT_800aca58 auf 0 (Transitions-FSM @0x8001cbdc);
der Dispatcher ruft damit @0x80073f90[0] = 0x800318f8. Dessen Kern:

    80031924: sb   zero,0x800acae8      ; +0x94 = 0 (Vorspann)
    8003192c: sw   v0(=1),0x800aca58    ; KOMMANDO := 1  -> cmd 0 laeuft GENAU EINMAL
    80031954: sb   zero,0x800acae9      ; +0x95 = 0
    ...
    80031bf4: addu a2,zero,zero         ; a2 = 0 -> VORWAERTS
    80031bfc: lw   a0,0x800acbc4        ; PLW-Skelett (W-Bank der ausgeruesteten Waffe)
    80031c04: lw   a1,0x800acbc8        ; PLW-EDD
    80031c10: sb   v0(=1),0x800acae8    ; +0x94 = 1   (W-Bank CLIP 1)
    80031c18: sb   zero,0x800acae9      ; +0x95 = 0
    80031c20: sb   zero,0x800acae3      ; +0x8f = 0   (kein Blend)
    80031c24: jal  0x8001f314           ; anim_set(..., reverse=0, 0x200)

(Spielerbasis 0x800aca54: +0x94 = 0x800acae8, +0x95 = 0x800acae9, +0x8f = 0x800acae3.)

**Das ist ein EINMAL-Ereignis**: derselbe Durchlauf stellt das Kommandowort schon auf 1
(@0x8003192c), ab dem naechsten Bild dispatcht der Dispatcher also 0x80073f90[1].

### 2.11 Waehrend der Tuer-Blende steht der Spieler KOMPLETT still

    80031c78: bltz a0,0x80031da8      ; a0 = g_pauseflags; Bit 0x80000000 -> Funktionsende

Die Uebergangs-FSM setzt @0x8001cddc-e8 `g_pauseflags |= 0xFF000000`; damit laeuft weder
ein cmd-Handler noch (Bit 0x10000000, @0x80019e28-40) der Action-Driver. +0x95 advanct in
diesen Bildern gar nicht.

### 2.12 Die byte-true UEBERGABE nach dem Eintritt: Idle-FSM Fall 0 (switchD_8003206c)

    80032074: ori  v0,zero,0x1
    8003207c: sb   v0,-13734(at)     ; 0x800aca5a = +0x06 = 1
    80032080: ori  v0,zero,0x3
    80032088: sb   v0,-13592(at)     ; 0x800acae8 = +0x94 = 3   (W-Bank CLIP 3 = neutraler Idle)
    8003208c: ori  v0,zero,0x7
    80032094: sb   zero,-13591(at)   ; 0x800acae9 = +0x95 = 0
    8003209c: sb   v0,-13597(at)     ; 0x800acae3 = +0x8f = 7   (Crossfade-Saat)
    800320a4: sh   zero,-13600(at)   ; 0x800acae0 = 0
    800320a8: jal  0x8001af20        ; rng
    800320b0: andi v0,v0,0x1f
    800320b4: addiu v0,v0,90         ; Haltezeit = 90 + (rng & 0x1f)
    800320bc: sh   v0,-13584(at)     ; 0x800acaf0

Die Fidget-Faelle daneben setzen ihre Clips ebenso mit eigenem `+0x95 = 0` und eigener
Haltezeit (z.B. @0x80032140 `+0x94 = 1` / @0x80032148 `+0x95 = 0` / @0x80032150 `+0x8f = 0`
mit +0x06 = 3 @0x80032134; @0x80032174 `+0x94 = 2` / @0x80032180 `+0x8f = 7` mit +0x06 = 5
@0x80032164) — also Phasen, keine Modulo-Schleife.

## 3. Was der Port tut — mit datei.c:zeile

1. `re15_port/engine/src/player_common.c:244-256` — `re15_player_room_entry_pose()` setzt
   `motion = RE15_MOTION_IDLE_SETTLE` (= 210, player_common.c:97) und **parkt sie dort**.
   Ein Gegenstueck zum Original-Store `sw v0(=1),0x800aca58` @0x8003192c gibt es nur fuer
   das Kommandowort (room_common.c:144 `state = 1`), NICHT fuer die Pose. Der Kommentar in
   room_common.c:140-143 sagt es selbst: "Der Port hat keinen cmd-0-Tick".

2. `re15_port/engine/src/player_common.c:751` — `p->anim_frame++` laeuft jedes Bild
   unbedingt weiter (angehalten nur von `motion_init_delay` und dem Event-Reach-Sub). Im
   Original advanct +0x95 ausschliesslich in einem anim_set-Aufruf (@0x8001f618), und der
   cmd-0-Handler nullt +0x95 vor jedem Aufruf (@0x80031c18) — der Zaehler kann dort nicht
   davonlaufen. Gemessen ist genau das Davonlaufen: ROOM1050 `cur` 0 -> 50 auf motion 210.

3. `re15_port/engine/src/anim_select_common.c:397-405` — Sentinel 210 wird auf
   `clip_override = 1` (W-Bank-Clip 1) abgebildet.

4. `re15_port/engine/src/anim_select_common.c:95` —
   `} else if (a->walk_active || clip_override >= 0) { slot = re15_actor_playback_slot(...) }`,
   und `re15_actor_playback_slot` (anim_select_common.c:55-59) rechnet `cur % frame_count`.
   **Jede Pose mit clip_override LOOPT damit.** Zusammen mit (2): die geparkte
   Eintritts-Pose spielt sich im Kreis, bis die Cutscene ihren ersten Plc_motion absetzt.

5. `re15_port/engine/src/player_common.c:1148` — der `player_mode == 2`-Zweig holt nur die
   Lauf-Sentinels 105/100 in den Idle zurueck; 210 bleibt stehen. Deshalb faellt der Defekt
   nur bei einer Cutscene nach dem Raumwechsel auf (§1.3).

6. `re15_port/include/re15_actor.h:1047` — `re15_actor_set_motion` setzt
   `motion_init_delay/anim_freeze/anim_frame/anim_flags/anim_frac` nur bei
   `a->motion != m`. Das Original kennt diese Bedingung nicht (§2.5). Wirkung: 1 fehlendes
   Bild beim WIEDERHOLTEN Plc_motion (gemessen F358 `cur=1 slot=18` statt `cur=0 slot=19`).

## 4. Der Unterschied, in einem Satz

Das Original setzt die Eintritts-Pose (W-Bank-Clip 1, Bild 0) EINMAL und gibt im selben
Handler-Durchlauf an Kommando 1 ab (@0x80031c10-c24 + @0x8003192c), waehrend der Port sie
als Dauer-Motion 210 parkt und ueber den Modulo-Zweig `cur % frame_count`
(anim_select_common.c:95 / 55-59) immer wieder von vorn abspielt — bei einer Cutscene, die den
Spieler auf `player_mode 2` festhaelt, gemessen bis zu 3,5-mal (ROOM1050 F0..F55).

## 5. Umsetzungsplan (konkrete Dateien/Zeilen, jede Konstante mit @0x)

**Schritt 1 — die Eintritts-Pose zu einem EINMAL-Ereignis machen (die Ursache).**
`re15_port/engine/src/player_common.c`, `re15_player_room_entry_pose()` (:244) plus ein
Einmal-Latch, der im Spieler-Tick vor dem Advance (:751) ausgewertet wird:

* Solange `re15_room_transition_active()` bzw. `g_re15_pauseflags & 0x80000000` steht:
  nichts advancen (Original: Dispatcher-Gate `bltz a0,0x80031da8` @0x80031c78;
  Action-Driver-Gate Bit 0x10000000 @0x80019e28-40). Das tut der Port heute schon richtig
  (gemessen ROOM1050 F0..F5 / ROOM1170 F501..F506 = Slot 0 konstant).
* ERSTES Bild nach dem Fallen der Maske: `motion = 210` (W-Clip 1, @0x80031c10),
  `anim_frame = 0` (@0x80031c18), `anim_frac = 0` (@0x80031c20), Richtung vorwaerts
  (a2 = 0 @0x80031bf4) — das ist die heutige `re15_player_room_entry_pose()`, unveraendert.
* ZWEITES Bild: Uebergabe an Idle-FSM Fall 0, byte-true aus switchD_8003206c —
  `+0x06 = 1` @0x8003207c, `+0x94 = 3` @0x80032088 (Port: `motion = RE15_MOTION_IDLE` =
  200, player_common.c:96), `+0x95 = 0` @0x80032094, `+0x8f = 7` @0x8003209c,
  `+0x8c = 0` @0x800320a4, Haltezeit `90 + (rng & 0x1f)` @0x800320b0/@0x800320b4.
  Das gilt UNABHAENGIG von `player_mode` — das Original gated hier nichts am Skript-Modus;
  die Szene uebernimmt erst mit ihrem eigenen Plc_motion (Kommandowort 4 @0x80041bb0).

**Schritt 2 — den Modulo-Zweig fuer die Eintritts-Pose entschaerfen.**
`re15_port/engine/src/anim_select_common.c:95`. Der Zweig
`a->walk_active || clip_override >= 0` -> `cur % frame_count` ist fuer Lauf-/Loop-Clips
richtig (Original-LOOP = `+0x1C4 & 0x04`, @0x80050dbc), fuer die Eintritts-Pose aber
falsch: dort ruft das Original `anim_set` genau einmal und nullt +0x95 vorher
(@0x80031c18). Konkret: den Sentinel 210 waehrend des Eintritts-Latches (Schritt 1) nicht
ueber `clip_override` fuehren, sondern hart als Bild 0 posieren. Kein neuer Zahlenwert
noetig — die Regel ist der Store @0x80031c18.

**Schritt 3 — der fehlende Vorhalte-Tick beim WIEDERHOLTEN Plc_motion** (1 Bild, belegt,
kein Doppel): `include/re15_actor.h:1047`. `op_plc_motion` (scd_vm.c:2216-2296) setzt heute
schon unbedingt `anim_frame = 0`, `anim_flags`, `anim_frac`; ihm fehlen
`motion_init_delay = 1` und `anim_freeze = 0` fuer den Gleich-Clip-Fall, weil
`re15_actor_set_motion` bei `motion == m` komplett aussteigt. Belegt durch die
verzweigungsfreie Phase-0-Kette @0x80050cec..@0x80050d0c hinter `+0x06 = 0` @0x80041bb4.

**Riegel (Messhaken; in diesem Lauf bewusst KEINE Engine-Aenderung):**
`RE15_ANIM_TRACE` plus ein Pruefer, der pro (Aktor, motion)-Block die Zahl der
Vorwaerts-NEUSTARTS zaehlt. Das heutige `re15_port/tools/check_anim_trace.py` wertet einen
`fc-1 -> 0`-Wrap als legal und faellt deshalb NICHT (auf beiden Messlaeufen meldet es nur
Kraehen-Spruenge). Sollwert fuer motion 210 nach einem Raumwechsel: **0 Neustarts,
hoechstens 1 Bild in Slot 0**. Gemessener Ist-Stand: ROOM1050 = 3 Neustarts / 56 Bilder,
ROOM1170-Tuer = 1 Neustart / 25 Bilder.

## 6. Offen / nicht belegt

* **Nicht belegt, wie GROSS die sichtbare Bewegung von W-Bank-Clip 1 ist.** Dass die Pose
  3,5-mal durchlaeuft, ist gemessen; der Port-Kommentar player_common.c:232-234 behauptet,
  bei 18 von 21 PLW-Baenken sei Clip 1 Bild 0 == Clip 3 Bild 0. Das habe ich NICHT
  nachgemessen. Naechster Weg: PL00W01-EDD/EMR-Keyframes von Clip 1 direkt auslesen und
  Wurzel-/Knochenwinkel ueber die 16 Bilder auftragen.
* **Nicht belegt, ob das Looping der Sentinels 210/211/212 im normalen Idle-Fidget
  byte-true ist.** Das Original hat pro Fidget-Clip eigene Phasen-Stores (§2.12). Der
  Umsetzungsplan fasst den Fidget deshalb bewusst nicht an.
* **Zweiter, unabhaengiger Befund derselben Messung (nicht Teil dieses Themas):** Ada
  (Aktor 1, Typ 0x42) startet in ROOM1050 ihren Lauf-Clip 0 bei F65 mitten im Zyklus neu
  (`cur` springt 8 -> 1); Verdacht: das zweite `Plc_dest` in sub04 @Datei 0x0E18. Nicht
  weiterverfolgt.
* **Nicht gemessen:** die uebrigen Cutscene-Raeume ("Leon, Ada etc.", "immer"). Der
  Mechanismus haengt nur an Raumwechsel + `player_mode == 2` und ist damit raum-unabhaengig,
  gemessen sind aber nur ROOM1050 und ROOM1170.
* **Widerlegte Spur (bewusst stehengelassen):** §1.1 vermutete, `Plc_flg(0,0x80,0)` greife
  nicht und der Clip laufe deshalb zweimal vorwaerts. Lauf C widerlegt das (rev=1,
  Slots 19 -> 0).
* **Nicht gemessen:** ob die NPCs (Ada, Elliot) denselben Eintritts-Pose-Defekt haben. Ihre
  Pose kommt aus dem Spawn-Saat-Wert (Runde 18) bzw. der NPC-Sub-VM, nicht aus dem
  cmd-0-Handler — die hier belegte Ursache ist SPIELER-spezifisch.

---

### Mess-Artefakte (Rohdaten dieses Laufs)

    <scratchpad>/anim/tr1050.txt    Pose-Strom ROOM1050 (Lauf A)
    <scratchpad>/anim/st1050.txt    Zustandsspur ROOM1050
    <scratchpad>/anim/tr1170b.txt   Pose-Strom ROOM1170 + Tuer-0-Reenter (Lauf B/C)
    <scratchpad>/anim/st1170b.txt   Zustandsspur dazu
    <scratchpad>/anim/tr1170.txt    Pose-Strom ROOM1170 Boot-Cutscene (Lauf C, sub02)
    <scratchpad>/anim/wrap.py       Auswertung: Neustarts je (Aktor, motion)-Block

(scratchpad =
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\d917ef01-c258-42fe-ac63-7a1e284c7456\scratchpad`)
