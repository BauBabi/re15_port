# Dossier — „Finisher-Biss-Sound des Zombies fehlt" (RE2-KI-Modus)

Nutzer-Report FINDING 1b: *„… als auch der finisher biss sound des zombies fehlt."*
(Der Biss WÄHREND der Fress-/Kollaps-Sequenz, RE2-KI-Modus.)

Status: **RE + Messung abgeschlossen. Ursache belegt. KEINE Code-Änderung in dieser Phase.**

---

## 0. Kurzfassung — das Urteil

| Frage | Antwort (belegt) |
|---|---|
| Ist der heiße Kandidat `Se_on(0x02070001)` @0x8010a804 der Finisher-Biss? | **NEIN.** Das ist Bank 2 = RDT-**snd0** (die Schritt-/Material-Bank), Record 7, und liegt auf dem **Spieler**-Kollaps. Der Zombie-Biss kommt von woanders. |
| Was IST der Finisher-Biss? | **snd1 (Bank 3) SE 3**, gespielt vom ZOMBIE in `FUN_80102bd8` (Devour-Animate): `jal 0x800453d0` **@0x80102cb0**, `ori a0,zero,0x3` @0x80102cb4, Gate `entity+0x95 == 0x28` **@0x80102ca0-a8**. |
| Warum ist SE 3 „der Biss"? | Unabhängiger Datenbeleg: SE 3 ist das **Frame-Wort-SFX-Bit der beiden Biss-Angriffs-Clips** der RE1.5-Zombiebank — EM10 Clip 0x27 f18+f47 → SE3, Clip 0x28 f21 → SE3 (selbst aus `EMD/CDEMD0.EMS` gedumpt). Dekoder `FUN_8001b38c`, `srl s0,v0,22` @0x8001b3b4 → `jal 0x800453d0` @0x8001b3cc. Zusätzlich spielt der Leichen-Fress-Executor denselben SE 3 auf demselben Frame-Gate 0x28 (@0x801045f0-0x80104600). |
| Fehlt er im Port? | **JA, im RE2-Modus.** Gemessen: während des gesamten RE2-Finishers (EXEC[6], Zombie-Clip 0x18, 123 Frames) fällt **kein einziger** snd1-SE. Im RE1.5-Modus fällt er byte-genau auf Zombie-Frame 0x28. |
| Spielt RE2 an dieser Stelle selbst einen Biss? | **NEIN.** Vollständiger Scan aller 43 `jal 0x8005bd6c` (RE2-ENEMSE-Player) in `EMOVL10_S0.BIN`: EXEC[6] @0x801039f0 spielt **ENEMSE 10/11 = MOAN**, keinen Biss. Die RE2-Spieler-Kollaps-Maschine 0x8010b464-0x8010b7d0 enthält **null** ENEMSE-Aufrufe. |
| Was gewinnt unter dem SOUND-MANDAT? | Die **RE1.5-Präsentation**: `re15_audio_room_se(3)` auf dem laufenden RE2-Devour-Clip. Der einzige zitierbare Frame-Anker ist die RE1.5-Absolutkonstante **0x28 @0x80102ca4** (auf dem 123-Frame-RE2-Clip erreichbar). |
| Nebenbefund (NEU, korrigiert das Vorgänger-Dossier) | RE2 spielt in seinem Kollaps-P0 sehr wohl `Se_on(0x04030001)` = CORE 3 = das Todes-Stöhnen — `lui a0,0x403` @0x8010b4fc / `ori a0,a0,0x1` @0x8010b500 / `jal 0x8005ba28` @0x8010b510. Das Vorgänger-Dossier (`todes-stoehnen.md`, O1) hielt 0x8005ba28 für die „RE2-Blutlache" und schloss „keine RE2-Adresse für einen Todes-Sprach-Frame" — das ist **widerlegt**. |

---

## 1. Der Kandidat aus dem Auftrag: `Se_on(0x02070001)` @0x8010a804 — neu verifiziert

### 1.1 Die Instruktionen (selbst disassembliert, `STAGE1.BIN` RAW @0x80100000)

```
; FUN_8010a6f8 (Spieler-Fress-Kollaps), Phase [1] @0x8010a7c0
8010a7d8: jal   0x8001ad68           ; Wurzel-Platzierung (jeden Tick)
8010a7e0: lui   v1,0x800b
8010a7e4: lbu   v1,-13591(v1)        ; v1 = *(0x800acae9) = player+0x95 = SPIELER-Anim-Frame
8010a7e8: ori   v0,zero,0x23         ; <-- FRAME-GATE 0x23 = 35
8010a7ec: bne   v1,v0,0x8010a824     ; ungleich -> ueberspringen
8010a7f0: ori   v0,zero,0x37         ; (Delay-Slot, laedt schon das naechste Gate)
8010a7f4: lui   a0,0x207
8010a7f8: lui   a1,0x800b
8010a7fc: lw    a1,-14460(a1)        ; a1 = *(0x800ac784) = g_entity(cur) = DER ZOMBIE
8010a800: ori   a0,a0,0x1            ; a0 = 0x02070001
8010a804: jal   0x80045024           ; Se_on
8010a808: addiu a1,a1,52             ; (Delay-Slot) a1 = entity+0x34 = Emitter-POSITION des Zombies
8010a80c: addiu v0,zero,-1
8010a810: lui   at,0x800b
8010a814: sh    v0,-13586(at)        ; player+0x9a (HP) := -1
8010a818: lui   v1,0x800b
8010a81c: lbu   v1,-13591(v1)        ; player+0x95 neu geladen
```

**Bestätigt, Feld für Feld:**
* **Frame-Gate:** strikte Gleichheit `player+0x95 == 0x23` (@0x8010a7e8/ec) auf dem **Spieler**-Frame-Zähler.
* **a0 = `0x02070001`** (@0x8010a7f4 + @0x8010a800). Zerlegt durch `Se_on` (FUN_80045024, PSX.EXE):
  * `srl v1,a0,24` @0x80045028 → **Bank = 2**
  * `srl v0,a0,16` / `andi s4,v0,0xff` @0x80045078-7c → **Record = 7**
  * `andi a0,a0,0xff` @0x80045080 → **Positional-Flag = 1**
* **a1 (Positionsquelle):** `g_entity(cur) + 0x34` — die Position **des Zombies**, nicht Leons (@0x8010a7fc + @0x8010a808).
* Bank-2-Handler @0x800450e4 → `j 0x80045130` → `lw a0,8(v0)` @0x8004513c mit `v0 = *(0x800ac778)` = RDT-Basiszeiger ⇒ **RDT-snd0-EDT** (Header +0x08). `0x800ac778` wird @0x80039740 aus dem RDT-Basiszeiger gesetzt (`sw a0,-14472(at)`).

### 1.2 Der Bank-Census — selbst ausgezählt

Alle **240** RDT unter `re15_port/shared_assets/PSX/STAGE{1..6}`; snd0-EDT = RDT-Header +0x08,
Record 7 = EDT + 28..31.

```
240 RDT gesamt | 34 ohne snd0-Tabelle | 206 mit snd0
snd0 rec7 BELEGT: 43      snd0 rec7 = 00 00 00 00: 163
```

Die 43 belegten (vollständig):

```
STAGE1: ROOM1090/1091 (00 00 45 10)   ROOM10F0/10F1 (00 00 45 11)
        ROOM1170/1171 (00 00 45 10)   ROOM11A0/11A1 (00 00 d5 11)
        ROOM1260/1261 (00 00 d5 10)
STAGE2: ROOM2000/2001 2020/2021 2030/2031 2080/2081 20B0/20B1 (00 00 d5 10)
        ROOM2040/2041 2050/2051 2060/2061 2070/2071 2090/2091 20A0/20A1 (00 00 d5 11)
        => ALLE 22 STAGE2-RDT belegt
STAGE3: ROOM3050/3051 3060/3061 3090/3091 30A0/30A1 30B0/30B1 (00 00 45 11)
        ROOM3080 (00 00 45 10)
STAGE4/5/6: KEINE
```

**In den spielbaren STAGE1-Zombie-/Gegner-Räumen ist Record 7 durchgehend `00 00 00 00`:**

```
ROOM1030 snd0=0x3760  rec7=00 00 00 00      ROOM1140 snd0=0x1680  rec7=00 00 00 00
ROOM1031 snd0=0xa328  rec7=00 00 00 00      ROOM1141 snd0=0xd560  rec7=00 00 00 00
ROOM10D0 snd0=0xee58  rec7=00 00 00 00      ROOM10D1 snd0=0x3ad4  rec7=00 00 00 00
ROOM1190 snd0=0x68bc  rec7=00 00 00 00      ROOM1191 snd0=0x68bc  rec7=00 00 00 00
ROOM1200 snd0=0x1744  rec7=00 00 00 00      ROOM1230 snd0=0x5f74  rec7=00 00 00 00
ROOM10C0 snd0=0x1658  rec7=00 00 00 00
```

### 1.3 ⛔ Ist `00 00 00 00` im ORIGINAL wirklich still? — NEIN, nachweislich nicht

Das Vorgänger-Dossier schrieb „in den STAGE1-Kampfräumen ist dieser Aufruf **auch im Original
stumm**". Das ist **nicht belegbar** — `Se_on` hat **keinen Leer-Test**. Die lineare
Disassembly von 0x80045140 bis 0x800451b8 enthält genau eine Verzweigung, und die betrifft
nur das Bank-Override-Bit:

```
80045140: sll  v0,s4,2
80045144: addu a0,a0,v0            ; a0 = snd0_edt + rec*4
80045148: lbu  v1,0(a0)            ; byte0
80045150: andi v0,v1,0x80          ; nur: Bank-Override-Bit
80045154: beq  v0,zero,0x80045160
8004515c: andi fp,v1,0x7f
80045160: lbu  v0,2(a0)  / 80045164: lbu v1,3(a0)
80045168: srl  s2,v0,4              ; TONE   = byte2>>4
8004516c: srl  s3,v1,5              ; Extra  = byte3>>5
80045174: lbu  v1,1(a0)  -> 80045180: andi s7,v1,0x7f   ; PROG = byte1&0x7f
8004517c: andi s0,v0,0x1f           ; VOICE = byte3&0x1f
80045184: sll  v0,s7,9  / 80045188: addiu v0,v0,2080     ; VagAtr-Basis = VAB+0x820 + prog*512
80045190: sll  v0,s2,5  / 80045194: addu s1,s6,v0        ; + tone*32
800451a8: sltiu v0,s0,0x10 ...      ; Stimmen-Zweig, KEIN Abbruch
```

Der einzige Abbruch weit oben ist der **Bank-Handle**: `lb a1,0x800b21ec[bank]` @0x80045064,
`beq a1,-1 → 0x8004539c` @0x8004506c. Bank 2 = `DAT_800b21ee`, geschrieben vom Bank-2-Lader
`FUN_80043eac` (@0x80043f00/@0x80043f3c) — dieselbe Bank, aus der der Material-Schritt-SE
`FUN_80045630` liest (@0x80045740/@0x80045764). Die Bank ist in jedem Raum geladen.

`00 00 00 00` löst also auf zu **Programm 0 / Tone 0 / Voice 0**. Selbst gemessen aus den
echten VABs (`pBAV`-Header, VagAtr-Feld +22 = VAG-Index):

```
ROOM1140 snd0 VH@0x1700  nprog=1 nTones=9   VagAtr p0t0 -> VAG index 1   (NICHT leer)
ROOM1030 snd0 VH@0x37e0  nprog=1 nTones=9   VagAtr p0t0 -> VAG index 1
ROOM10D0 snd0 VH@0xeed8  nprog=1 nTones=12  VagAtr p0t0 -> VAG index 1
ROOM1190 snd0 VH@0x693c  nprog=1 nTones=12  VagAtr p0t0 -> VAG index 1
ROOM1200 snd0 VH@0x17c4  nprog=1 nTones=4   VagAtr p0t0 -> VAG index 1
```

⇒ Das Original spielt an Frame 0x23 in ROOM1140 die **erste Probe der Schritt-Bank** —
nicht Stille, sondern ein nicht-autorisierter Fehlgriff des Original-Codes (ein „content bug":
der Autor hat rec7 in diesen Räumen nie befüllt, der Code fragt trotzdem ab).

**Der Port unterdrückt das per uncitierter Heuristik.** `vab_common.c:263`
`out->empty = (e[2] == 0 && e[3] == 0);` (ebenso :188, :217) — dieser Test steht **nirgends im
Original**; `audio_pc.c:687` bricht darauf ab. Das ist eine getrennte, kleine Port-Abweichung
(ohne `@0x…`), die hier nur dokumentiert und **nicht** angefasst wird.

### 1.4 Urteil zu O4 / `Se_on(0x02070001)`

* Der Aufruf ist real und im Port nirgends verdrahtet (`re15_audio_room_se_snd0()` existiert,
  wird aber nur vom Schiebe-FSM benutzt, `player_common.c:501`).
* Er ist **nicht** der gemeldete Finisher-Biss: er hängt am **Spieler**-Frame 0x23, spielt aus
  der **Schritt-Bank**, und ist in genau den Räumen, in denen der Nutzer stirbt (1030/1140/
  10D0/1200/1230), inhaltlich unautorisiert.
* **Empfehlung: NICHT für diesen Report nachbauen.** Wer ihn nachbaut, müsste zugleich die
  uncitierte `empty`-Heuristik entfernen und würde in ROOM1140 einen Schritt-Sample-Fehlgriff
  reproduzieren. Relevanz hätte er in STAGE2/STAGE3 (dort autorisiert; z.B. ROOM1170 rec7
  `00 00 45 10` → prog 0/tone 4 → VAG 4) — als eigener, kleiner Nachzug mit eigener Messung.

---

## 2. Der ECHTE Finisher-Biss — `FUN_80102bd8`, snd1 SE 3 @0x80102cb0

### 2.1 Die Instruktionen (STAGE1.BIN)

`FUN_80102bd8` = DEVOUR-FINISH-Animate, Dispatch `@0x8011f890[+0x5 = 5/6]`.

```
; --- Eintritt (+0x6 == 0), @0x80102c08 ff. ---
80102c08: sb    v0,6(a0)             ; +0x6 := 1
80102c18: lbu   v0,5(v1)             ; +0x5
80102c20: addiu v0,v0,4
80102c24: sb    v0,148(v1)           ; +0x94 (Clip) := (+0x5)+4  = 9 (Front) / 0x0A (Hinten)
80102c34: sb    zero,149(v0)         ; +0x95 (Frame) := 0
80102c40: ori   v0,zero,0x7
80102c44: sb    v0,143(v1)           ; +0x8f := 7 (Crossfade-Seed)
80102c58: sw    v1,-13316(at)        ; 0x800acbfc = Opfer-Greifer
80102c5c: lbu   v0,5(v1)
80102c64: addiu v0,v0,-5
80102c68: sll   v0,v0,8
80102c78: ori   v0,v0,0x6
80102c80: sw    v0,-13736(at)        ; 0x800aca58 = Spieler-Cmd ((+0x5)-5)<<8 | 6 = KOLLAPS
80102c8c: jal   0x800453d0           ; ** snd1 SE 4 = Devour-Einstiegs-SE **
80102c90: ori   a0,zero,0x4          ;    (Delay-Slot)

; --- pro Tick ---
80102c94: lui   v0,0x800b
80102c98: lw    v0,-14460(v0)        ; v0 = g_entity(cur) = DER ZOMBIE
80102ca0: lbu   v1,149(v0)           ; v1 = entity+0x95 = ZOMBIE-Anim-Frame  (NICHT player+0x95!)
80102ca4: ori   v0,zero,0x28         ; <-- FRAME-GATE 0x28 = 40
80102ca8: bne   v1,v0,0x80102cb8
80102cac: nop
80102cb0: jal   0x800453d0           ; ** snd1 SE 3 = DER FINISHER-BISS **
80102cb4: ori   a0,zero,0x3          ;    (Delay-Slot)
80102cb8: ...
80102cd0: jal   0x8001ad68           ; Wurzel-Motion (zieht den Zombie auf die Leiche)
80102cec: jal   0x8001f314           ; anim_set(a3=0x200); +0x6 += Rueckgabe -> INERT
```

`FUN_800453d0` (PSX.EXE) ist der **Bank-3**-Pfad: `lw a2,-14472(a2)` @0x80045430 = RDT-Basis,
`lb fp,8687(fp)` @0x80045438 = Handle `DAT_800b21ef` (= Bank 3), `lw a1,20(a2)` @0x80045444 =
RDT-Header **+0x14 = snd1-EDT**, Bound `sltiu v0,v1,0x19` @0x80045420 (25 Records).
(Nebenbei @0x8004540c: `lw v0,0(g_entity_cur)` / `andi v0,v0,0x2000` → gesetzt ⇒ `a0 += 12`
@0x80045418 — der Zweit-Varianten-Offset der Bank.)

### 2.2 Warum SE 3 = „Biss" — unabhängiger Datenbeleg

Frame-Wort-SFX-Dekoder `FUN_8001b38c`:
```
8001b3a4: lw   v0,360(v0)      ; +0x168 = Zeiger auf das AKTUELLE Frame-Wort
8001b3ac: lw   v0,0(v0)
8001b3b4: srl  s0,v0,22        ; obere 10 Bit = SFX-Maske
8001b3c0: andi v0,s0,0x1
8001b3cc: jal  0x800453d0      ; Bit-Index == snd1-SE-Id
8001b3d0: andi a0,s1,0xff
```
Dump der SFX-Bits aus `shared_assets/PSX/EMD/CDEMD0.EMS`, Bank EM10 (Zombie), mit dem
Port-Parser (`re15_emd_parse_container`, Scratchpad `dump_sfxbits.exe`):

```
clip 0x0B fc= 55 : f14->SE1  f46->SE0      (Loco)
clip 0x0D fc= 55 : f30->SE0  f48->SE0
clip 0x11 fc= 60 : f42->SE1
clip 0x16 fc=159 : f102->SE1 f130->SE1
clip 0x27 fc= 69 : f18->SE3  f47->SE3      <== BISS-ANGRIFF A
clip 0x28 fc= 82 : f21->SE3                <== BISS-ANGRIFF B
clip 0x09 fc= 65 : (keine SE-Bits)         <== Devour Front
clip 0x0A fc= 65 : (keine SE-Bits)         <== Devour Hinten
```
⇒ **SE 3 ist der Biss-SE der Zombiebank**, und die Devour-Clips tragen KEINE eigenen Bits —
der einzige Ton während des Finishers ist der explizite `jal` @0x80102cb0. (EM11 und EM16
identisch.)

Dritter, unabhängiger Beleg: der **Leichen-Fress-Executor** spielt denselben SE 3 auf demselben
Gate 0x28 —
```
801045ec: lbu v1,149(v0)     ; entity+0x95
801045f0: ori v0,zero,0x28
801045f4: bne v1,v0,0x80104604
801045fc: jal 0x800453d0
80104600: ori a0,zero,0x3
```

### 2.3 Was der SE in den Räumen konkret ist

snd1-EDT (RDT +0x14), Record 3, selbst ausgelesen — in JEDEM STAGE1-Zombieraum identisch:

```
ROOM1140 snd1@0x2320  rec3 = 00 00 43 14  -> prog 0 / tone 4 / prio-Nibble 3 / voice 0x14-0x10 = 4
                                             VagAtr p0t4 -> VAG index 4  (nvag=11)
ROOM1030 snd1@0x4400  rec3 = 00 00 43 14  -> VAG index 4
ROOM10D0 snd1@0xfaf8  rec3 = 00 00 43 14  -> VAG index 4
ROOM1200 snd1@0x23e4  rec3 = 00 00 43 14  -> VAG index 4
ROOM1190 snd1@0x755c  rec3 = 00 00 43 16  (Hunderaum, andere Bank)
```
snd1 rec3 ist in **196 von 206** RDT belegt — anders als snd0 rec7 also praktisch game-weit da.

---

## 3. Die RE2-Seite — spielt RE2 dort einen Biss?

### 3.1 Vollständiger ENEMSE-Scan

`jal 0x8005bd6c` (RE2-ENEMSE-Player) in `info/re2leon/COMMON/BIN/EMOVL10_S0.BIN`, alle
43 Fundstellen mit Rückverfolgung von `addiu a0,zero,N`:

| Adresse | Id | Kontext |
|---|---|---|
| **@0x801028ec** | **3 (BISS)** | Griff-**Kampf**-Schleife P3 — der Biss WÄHREND des Ringens, vor dem Tod |
| **@0x801039f0** | **10 / 11** | **EXEC[6] = der Finisher** (`addiu a0,zero,11` @0x801039e8 / `10` @0x801039ec, Würfel `jal 0x80015fe8` @0x801039d8) — **MOAN, kein Biss** |
| @0x8010448c, @0x80104814 | 10 / 11 | weitere Moans |
| @0x80104e00 | 5 | Hund-/Flug-Zweig |
| @0x801052b8 | 9 | Kriech-Grab-Abriss |
| … | 12/13/2/… | Hit/Death/Fall |
| **0x8010b464 – 0x8010b7d0** | **—** | **NULL Fundstellen** |

### 3.2 EXEC[6] byte-true (RE2-Finisher)

```
80103990: lui   v0,0xf                 ; (Delay-Slot)
801039b0: ori   v0,v0,0x18             ; Clip-Wort 0x000F0018 -> Clip 0x18, Frame 0, Rate 0xF
801039b8: addiu a0,a0,-1032            ; a0 = 0x800CFBF8 = die SPIELER-Struktur
801039c8: sw    v0,332(s0)
801039d0: jal   0x80015b94             ; Anker (Zombie + Kopie in den Spieler)
801039d4: sb    v0,6(s0)               ; Phase 1 -> FALLTHROUGH
801039d8: jal   0x80015fe8             ; rand
801039e0: andi  v0,v0,0x1
801039e4: beq   v0,zero,0x801039f0
801039e8: addiu a0,zero,11             ; ENEMSE 11
801039ec: addiu a0,zero,10             ; ENEMSE 10
801039f0: jal   0x8005bd6c             ; ** der EINZIGE SE von EXEC[6]: ein MOAN **
801039f4: addu  a1,s0,zero
80103a08: ... jal 0x80015cb8 (Platzierung) / jal 0x8002959c (Advance, a3=256)
80103a44: lhu v0,270(s0) / sw v1,4(s0) mit v1 = 0x801 (@0x801039a4) -> Zustand FRESSEN
```
Der Fress-Loop danach (EXEC[8]) konsumiert die EDD-Frame-Flags **nicht** — RE2 verwertet die
`0x0C0405B2`-Flags nur in WALK und BUMP. Der RE2-Fress-Loop ist im Original also ebenfalls
stumm; der Port spielt dort per dokumentiertem Mandats-Mapping
`re2z_frame_flag_se(e, 3u, 3)` (`enemy_ai_re2_zombie.c:2218`).

### 3.3 Die RE2-Spieler-Kollaps-Maschine

Phasentabelle `@0x8010022c` (selbst dekodiert):
`{0x8010b4c4, 0x8010b5cc, 0x8010b724, 0x8010b744, 0x8010b774, 0x8010b78c, 0x8010b7ac, 0}`.

Alle `jal` in 0x8010b464-0x8010b7d0:
```
0x8005ba28  <- 0x8010b510     (RE2-Se_on)
0x800395b8  <- 0x8010b548, 0x8010b5c4     (Rumble)
0x80039514  <- 0x8010b560, 0x8010b584, 0x8010b598, 0x8010b5ac
0x8003947c  <- 0x8010b570
0x80015cb8  <- 0x8010b5e8     (Platzierung)
0x8002959c  <- 0x8010b60c, 0x8010b758     (Advance)
0x80015fe8  <- 0x8010b648, 0x8010b654, 0x8010b694, 0x8010b6bc, 0x8010b778  (rand)
0x8001bf10  <- 0x8010b668, 0x8010b6a8, 0x8010b6d0     (RE2-Gore-FX)
```
**Kein ENEMSE (0x8005bd6c) ⇒ kein Zombie-Biss-SE.** Der einzige Ton ist:

```
8010b4fc: lui   a0,0x403
8010b500: ori   a0,a0,0x1        ; a0 = 0x04030001
8010b504: lbu   v0,448(s2)
8010b508: addiu a1,s2,56         ; a1 = PL+0x38 = Spieler-Position
8010b50c: ori   v0,v0,0x1
8010b510: jal   0x8005ba28       ; RE2-Se_on
8010b514: sb    v0,448(s2)
```
`0x8005ba28` ist byte-true das RE2-Gegenstück zu `Se_on`: `srl t1,a0,24` @0x8005ba30 (Bank),
Handle-Tabelle `0x800d4c48[bank]` @0x8005ba64, `srl v0,a0,16 / andi 0xff` @0x8005ba7c-80
(Record), `andi a0,a0,0xff` @0x8005ba9c (Positional). ⇒ **Bank 4 / Record 3 = das
Todes-Stöhnen, am Kollaps-EINTRITT (P0), nicht frame-gegatet.**

> ⚠ Damit ist **O1 des Vorgänger-Dossiers widerlegt.** Dort hieß es, die einzigen `jal` in P0
> seien „0x8005ba28 (RE2-Blutlache)" und es gebe „keine RE2-Adresse für einen Todes-Sprach-
> Frame". 0x8005ba28 ist Se_on, und die Adresse ist 0x8010b510.

---

## 4. MESSUNG — was der Port heute tut

Harnisch: Kopie von `tests/unit/probe_re2z_devour.c` (Scratchpad `probe_se_bite.c`), ECHTES
`game_step`, ROOM1140-SCD-Spawns, geladene RE2-Bänke, SE-Spione aus `tests/test_support.c`
(`re15_audio_room_se` / `re15_audio_core_se` / `re15_audio_room_se_snd0`).

### 4.1 RE2-Flavor (der Report-Fall)

```
== seed 0: GRIFF ab f59 (Zombie slot 2 typ 0x10) ==
   *** SE f80   CORE se=1  | vs=2 Zclip=12 Zfr=16 | Lclip=15 Lfr=0     <- Kollaps-Einstieg
   *** SE f135  CORE se=3  | vs=2 Zclip=24 Zfr=54 | Lclip=15 Lfr=55    <- Todes-Stoehnen (v0.3.21)
   ---- Zombie-Clip 0x18 (24) laeuft von ~f81 bis ~f227: KEIN EINZIGER snd1-SE ----
   *** SE f228  SND1 se=3  | vs=2 Zclip=18 Zfr=23 Zs1=8 Zs2=1          <- erst der FRESS-LOOP
   *** SE f253  SND1 se=3  | vs=2 Zclip=18 Zfr=48 Zs1=8 Zs2=1
   ... (18 weitere Chomps, alle Zs1=8 = EXEC[8])
== seed 1 identisch: CORE1 f167, CORE3 f222, erster SND1 se=3 erst f406 ==
```
**Zwischen Kollaps-Eintritt (f80) und dem ersten Biss (f228) liegen 148 Frames ≈ 5 s Stille.**
Genau das ist der Nutzer-Report.

### 4.2 RE1.5-Flavor (Kontrast)

```
   *** SE f57   SND1 se=4  | vs=2 Zclip=10 Zfr= 0 Zs1=6 Zs2=1   <- Devour-Einstieg @0x80102c8c
   *** SE f57   CORE se=1
   *** SE f86   CORE se=3
   *** SE f97   SND1 se=3  | vs=2 Zclip=10 Zfr=40 Zs1=6 Zs2=1   <- BISS, Zfr=40 = 0x28 @0x80102cb0
```
Zombie-Frame **40 = 0x28**, exakt das Gate @0x80102ca4. (Deckt sich mit der Messung des
Vorgänger-Dossiers: „SE f126 … afr=40 : SND1 se=3".)

> Messhinweis: dieser Harnisch lädt IMMER die RE2-Bänke (`load_bank2` → `RE2/CDEMD0.EMS`),
> auch unter RE1.5-Flavor. Die **SE-Sequenz** (SE 4 auf Zfr 0, SE 3 auf Zfr 0x28) ist davon
> unberührt — beide Gates sind auf beiden Bänken erreichbar. Die zusätzlichen `SND1 se=5/8`
> auf Zfr 24 in diesem Lauf stammen aus den Frame-Bits der **RE2**-Bank; die echte
> RE1.5-Bank trägt auf Clip 9/0x0A **keine** SFX-Bits (§2.2) — der RE1.5-Finisher hat also
> genau ZWEI Zombie-Töne: SE 4 und SE 3.

### 4.3 Clip-Längen (aus den echten Bänken gemessen)

| | Zombie-Devour-Clip | Leon-Kollaps-Clip |
|---|---|---|
| RE1.5 (EM10, `EMD/CDEMD0.EMS`) | Clip 9 / 0x0A = **65** Frames | Opfer-Clip 6/7 = 65 Frames |
| RE2 (EM010, `RE2/CDEMD0.EMS`) | Clip 0x18 = **123** Frames | Opfer-Clip 13/15 = 116 Frames |

Das RE1.5-Gate **0x28 = 40** ist auf dem 123-Frame-RE2-Clip erreichbar (40 < 123).

---

## 5. Die Port-Lücke, exakt lokalisiert

| Datei / Funktion | Devour-Einstiegs-SE 4 | **Biss-SE 3 @ Frame 0x28** |
|---|---|---|
| `engine/src/enemy_ai_common.c:3793` `re15_enemy_ai_live_devour()` (**RE1.5**) | vorhanden (Z. 3802) | **vorhanden (Z. 3806)** |
| `engine/src/enemy_ai_re2_zombie.c:2008` `re2z_exec_six()` (**RE2**) | `re2z_se(10/11)` → Mapper → RE1.5 SE 4/5 | **FEHLT — kein snd1-Aufruf im ganzen Zweig** |

Der Wiedergabepfad selbst ist fertig und getestet: `re15_audio_room_se(3)` → `audio_pc.c:805`
→ snd1-EDT-Record 3 → VAG 4 der Raum-snd1-Bank. **Es fehlt nur der Aufruf.**

Das steht im direkten Widerspruch zum SOUND-MANDAT (Entscheidungen aus RE2, *Sounds und
Präsentation aus RE1.5*): der RE2-Finisher-Zweig wurde ohne die RE1.5-Präsentation gebaut —
derselbe Fehlertyp wie beim Todes-Stöhnen (v0.3.21), nur eine Ebene tiefer.

---

## 6. Was die Umsetzung braucht (nur Fakten, keine geratenen Zahlen)

* **Der Laut:** `re15_audio_room_se(3)` — snd1/Bank 3, Record 3.
  Belege: `jal 0x800453d0` **@0x80102cb0** mit `ori a0,zero,0x3` @0x80102cb4 (Devour);
  identisch @0x801045fc/@0x80104600 (Leichen-Fressen); SE-Semantik über die Frame-Bits der
  Biss-Clips 0x27 f18/f47 und 0x28 f21 (Dekoder `srl s0,v0,22` @0x8001b3b4 →
  `jal 0x800453d0` @0x8001b3cc).
* **Der Frame-Anker:** die einzige zitierbare Konstante ist **`0x28` @0x80102ca4**, gelesen vom
  **Zombie**-Frame-Zähler `entity+0x95` (@0x80102ca0) — **nicht** vom Spieler-Zähler.
* **Der Einstiegs-SE** (SE 4 @0x80102c8c) ist im RE2-Zweig durch den Mandats-Mapper bereits
  näherungsweise abgedeckt (ENEMSE 10 → RE1.5 SE 4, ENEMSE 11 → RE1.5 SE 5, Würfel
  @0x801039d8-ec). Ob die RE1.5-Präsentation hier den **festen** SE 4 verlangt, ist eine
  bewusste Festlegung — siehe O2.
* **Retrigger-Schutz:** das Original feuert über ein Gleichheits-Gate auf einem monoton
  wachsenden Zähler ⇒ genau einmal. Der Port-Advancer klemmt play-once-Clips auf `fc-1`; ein
  Klemm-Gate würde jeden Tick neu feuern. Der neue Aufruf gehört an ein **einmaliges Latch**
  (dieselbe Lehre wie O3 des Vorgänger-Dossiers). `re2z_sfx_slot` (Feld in `re15_actor.h`) ist
  die im RE2-Zweig bereits etablierte Dedup-Infrastruktur.

---

## 7. Offene Fragen (bewusst NICHT geraten)

**O1 — Verbatim-Frame 40 oder proportional?**
Der RE1.5-Clip hat 65 Frames, Gate 40 = 61,5 %. Der RE2-Clip 0x18 hat 123 Frames; Frame 40
liegt dort bei 32,5 %. Beide Varianten sind vertretbar; nur *verbatim 40* trägt eine
`@0x…`-Adresse (0x80102ca4). Eine proportionale Skalierung wäre eine Präsentations-
Entscheidung ohne Beleg. **Empfehlung: verbatim 0x28**, und falls der Nutzer den Biss zu früh
findet, danach mit einer Messung (gdigrab, echtes Fenster) nachjustieren — nicht vorab raten.

**O2 — Fester SE 4 am Devour-Einstieg?**
RE1.5 spielt am Einstieg **immer** SE 4 (@0x80102c8c, kein Würfel). RE2 würfelt 10/11, was der
Mapper auf SE 4/5 abbildet. Unter dem Mandat („Präsentation aus RE1.5") wäre SE 4 fest
korrekt; unter „Entscheidungen aus RE2" bleibt der Würfel. Bewusste Festlegung nötig.

**O3 — `Se_on(0x02070001)` @0x8010a804 (snd0 rec 7).**
Real, im Port nirgends gerufen. In den Sterbe-Räumen von STAGE1 ist rec7 `00 00 00 00` und
löst im Original auf prog0/tone0 = VAG 1 der Schritt-Bank auf (§1.3) — ein Original-Fehlgriff,
kein autorisierter Laut. In STAGE2 (alle 22 RDT) und STAGE3 ist rec7 autorisiert. Getrennter
Nachzug; würde zugleich eine Entscheidung über die uncitierte `empty`-Heuristik
(`vab_common.c:188/217/263`) erzwingen.

**O4 — Timing des Todes-Stöhnens im RE2-Modus.**
Der Port spielt CORE 3 auf dem RE1.5-Gate 0x37 (gemessen Lfr=55, f135). RE2 selbst spielt es
am Kollaps-**Eintritt** (@0x8010b510, P0, Lfr=0 ≈ f81) — 54 Frames früher. Unter dem Mandat
gewinnt die RE1.5-Präsentation (0x37), aber der Widerspruch ist jetzt belegt und sollte
bewusst stehengelassen oder korrigiert werden.

**O5 — Der 4. STAGE1-Opfer-Kollaps** (CORE-3-Stelle @0x8011c4b8, Tabelle @0x80100414):
hat er ein eigenes Biss-Gate? Für dieses Dossier nicht nötig, aber offen.

---

## 8. Reproduktion

```bash
S=.claude/skills/re15-psx-disasm/scripts/re15_disasm.py
S2=.claude/skills/re15-psx-disasm/scripts/re2_disasm.py

# RE1.5 — der Biss
python $S dis 0x80102bd8 90                 # FUN_80102bd8: SE 4 @0x80102c8c, SE 3 @0x80102cb0, Gate 0x28 @0x80102ca4
python $S dis 0x801045c0 22                 # Leichen-Fressen: dasselbe Gate 0x28 -> SE 3
python $S dis 0x8001b38c 24                 # Frame-Wort-SFX-Dekoder (srl >>22 -> jal 0x800453d0)
python $S dis 0x800453d0 40                 # Bank-3-Pfad (snd1, RDT +0x14)

# RE1.5 — der Kandidat aus dem Auftrag
python $S dis 0x8010a7c0 50                 # Gate 0x23 @0x8010a7e8, Se_on(0x02070001) @0x8010a804
python $S dis 0x80045024 70                 # Se_on: Bank/Record/Positional
python $S dis 0x80045130 60                 # Bank-2-Handler + Record-Dekodierung (KEIN Leer-Test)
python $S table 0x80010e70 8                # Bank-Dispatch

# RE2
python $S dis 0x80103970 60 --bin "../../../re2leon/COMMON/BIN/EMOVL10_S0.BIN"   # EXEC[6]: Moan 10/11
python $S dis 0x8010b4c4 40 --bin "../../../re2leon/COMMON/BIN/EMOVL10_S0.BIN"   # Kollaps P0: Se_on(0x04030001)
python $S table 0x8010022c 8 --bin "../../../re2leon/COMMON/BIN/EMOVL10_S0.BIN"  # Kollaps-Phasentabelle
python $S2 dis 0x8005ba28 30                # RE2-Se_on (info/re2leon/PSX.EXE)

# Messung (Scratchpad-Sonden, kein Repo-Code)
export PATH="/c/msys64/mingw64/bin:$PATH"
cd re15_port/build
gcc -std=c11 '-DRE15_ASSET_PSX_DIR="C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX"' \
    -DRE15_PLATFORM_PC=1 -I../include -o $SP/probe_se_bite.exe $SP/probe_se_bite.c \
    engine/libre15_engine.a tests/libre15_test_support.a -lm
cd ..
$SP/probe_se_bite.exe 2 900          # RE2   -> kein snd1-SE waehrend Clip 0x18
SE_FLAVOR=1 $SP/probe_se_bite.exe 2 700   # RE1.5 -> SND1 se=3 auf Zfr=40
$SP/dump_sfxbits.exe                 # SFX-Bits der RE1.5-Zombiebank (0x27 f18/f47 -> SE3)

# Bank-Census
python $SP/snd0census.py             # snd0 rec7: 43 belegt / 163 leer / 34 ohne Tabelle
python $SP/snd1census.py             # snd1 rec3: 196/206 belegt
python $SP/vabprobe.py               # VagAtr-Aufloesung prog/tone -> VAG-Index
python $SP/scan_enemse.py            # 43 jal 0x8005bd6c in EMOVL10_S0.BIN
python $SP/scan_jal.py               # alle jal in 0x8010b464-0x8010b7d0
```
