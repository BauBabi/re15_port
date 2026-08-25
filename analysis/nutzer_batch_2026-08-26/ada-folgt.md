# FINDING 5 — „room 1090 … ausserdem folgt mir ada nicht nach der cutscene"

**Datum:** 2026-08-24 · **Phase:** RE-Dossier, KEINE Code-Änderung
**Urteil:** ⛔ **PORT-BUG.** Das Original **hat** ein Folge-Verhalten für diese Figur, und zwar eine
vollständige Eskorte-Maschine (Ziel = ein Punkt **1500 Einheiten am Spieler**, jeden Frame neu
gerechnet, mit Lauf-/Ankunfts-/Wieder-Aufbruch-Schleife). Der Port lässt diesen Zweig unbedient:
`re15_npc_ai_tick` behandelt NPC-**State 1** in `case 1: default:` als „Idle-Pose halten".
Der Fehler ist **global**, nicht raumspezifisch — 12 `Plc_ret` auf einem NPC-Work-Slot in 8 Räumen
laufen in denselben toten State.

---

## 0. Wer ist „Ada" hier? — die Typ-Id, aus den RDT-Spawns belegt

ROOM1090 hat **genau einen** NPC-Spawn. `re15_port/shared_assets/PSX/STAGE1/ROOM1090.RDT`,
sub_scd[0], Datei-Offset **0x22CC**:

```
0022A2  06 00 40 00              If (len 0x40)
0022A6  21 03 84 01              Ck(bank 3, bit 0x84, val 1)
0022AA  2d 02 …                  Obj_model_set[2]
0022CC  44 00 42 40 01 00 00 ff  Sce_em_set  slot=0  type=0x42  behavior=0x40  kill=0xFF
        d0 8a 00 00 d0 8a 00 00     pos = (-30000, 0, -30000)   (Park-Position, s.u.)
        f8 9b 00 00
0022E0  04 ff 18 03              Evt_exec(cond 0xff, sub 3)    <- startet die Rettungs-Cutscene
0022E4  08 00                    Endif
```

Also **Entity-Typ 0x42**, Deskriptor-Byte 0x40, gestartet zusammen mit **sub_scd[3]**.
Die Nachrichten-Sektion desselben RDT (RDT+0x3C = Datei 0x2748) benennt sie:

| Id | Text |
|----|------|
| 00 | `Woman: Anyone! Can someone please get me out of here!?` |
| 01 | `Leon: Hey! Who's that?!` |
| 02 | `Woman: Someone, anyone! If out there, open the door!` |
| 03 | `Leon: There! Are you okay?` |
| 04 | `Woman: You saved me... Thank you, you have my gratitude.` |
| 05 | `Leon: We can talk later... It's not safe here.` |
| 06 | `Leon: Besides, you need to get that wound treated, right? **Let's go.**` |
| 07 | `I must hurry up and get something to put out this fire to save that woman!` |
| 08 | `Will you use the Fire Extinguisher?` |
| 09 | `You've used the Fire Extinguisher.` |

msg03–06 sind exakt die vier `Message_on` von sub03 (@0x2586, @0x25E8, @0x2640, @0x2670).
Das RE1.5-Skript nennt sie nur „Woman"; die Zuordnung des Namens „Ada" ist eine
Community-/Nutzer-Etikettierung (RE2-Retail nummeriert Ada als `EM041/EM043`, das ist ein
**anderes Spiel** und trägt hier nicht). Für dieses Dossier gilt: **„Ada" = die Typ-0x42-Figur
von ROOM1090** — die einzige NPC in diesem Raum, also eindeutig.

---

## 1. Erste, ehrliche Frage: JA — das Original hat ein Folge-Verhalten. Belegt.

### 1.1 Der Handler-Eintrag (Typ → Overlay-Root)

Die EXE-Dispatch-Tabelle `@0x80072bac` wird vom STAGE1-Overlay befüllt.
`0x80072bac + 0x42*4 = 0x80072cb4`:

```
; STAGE1.BIN, Registrierungs-Routine
8011e910: lui   v0,0x8012
8011e914: addiu v0,v0,-14944        ; v0 = 0x8011c5a0   (= Typ 0x40, Irons-Root)
8011e918: lui   at,0x8007
8011e91c: sw    v0,11436(at)        ; -> 0x80072cac = [0x40]
8011e920: lui   v0,0x8012
8011e924: addiu v0,v0,-13456        ; v0 = 0x8011cb70
8011e928: lui   at,0x8007
8011e92c: sw    v0,11444(at)        ; -> 0x80072cb4 = [0x42]     ⇐ ROOT für Typ 0x42
```

### 1.2 Der Root 0x8011cb70 — er füttert JEDEN Frame einen Nav-Zielpunkt

```
8011cb84: lw    v0,0(s0)            ; s0 = 0x800aca40 g_pauseflags
8011cb90: bne   v0,zero,0x8011cc7c  ; globaler AI-Freeze (0x20000000)
8011cba4: lbu   v0,9(v0)            ; +0x9
8011cbb0: bne   v0,zero,0x8011cc7c  ; per-Entity-Skip (&0x20)
8011cbbc: jal   0x8001bd60          ; Kopf-/Neck-Look (a0=-10, a1=0x14)
8011cbd0: lh    v0,476(v1)          ; +0x1dc  = Nav-Ziel X
8011cbdc: lh    v0,478(v1)          ; +0x1de  = Nav-Ziel Z
8011cbe8: jal   0x80039e7c          ; NAV-STEER  (Ziel -> Wegpunkt +0x1bc/+0x1be)
8011cbfc: lbu   v0,4(v0)            ; +0x4 = State
8011cc0c: addiu at,at,5736          ; Tabelle 0x80121668
8011cc1c: jalr  v0                  ; State-Dispatch
```

**State-Tabelle @0x80121668** (byte-gelesen):

| Idx | Ziel | |
|----|------|---|
| 0 | `0x8011ccac` | INIT (Overlay) |
| **1** | **`0x8011ce54`** | **die Folge-/Eskorte-Maschine (Overlay)** |
| 2 | `0x8011d018` | Hurt |
| 3 | `0x8011d060` | — |
| 4 | `0x80050be8` | EXE-Executor (Sub-VM @0x80076ca0) |
| 5 | `0x8011cf20` | Decide+Exec-Wrapper (Overlay) |
| 6..13 | `0x8004f100 / f3a4 / f5e8 / f7dc / fb3c / fd3c / ff90 / 503a8` | EXE-Shared **DECIDE**-Tabelle |
| 14..21 | `0x8004f310 / f4e0 / f6f0 / f9ec / fc2c / fe44 / ffc0 / 503e4` | EXE-Shared **EXEC**-Tabelle |

### 1.3 Warum sie VOR der Cutscene nur steht — Deskriptor 0x40

INIT `0x8011ccac`:

```
8011ccc0: sb v0,4(v1)               ; +0x4 = 1        (Default -> State 1)
8011cd18: sh v0,154(v1)             ; +0x9a = -1      (unverwundbar)
8011cd90: sb v0,148(v1)             ; +0x94 = 2       (Idle-Clip)
…
8011ce1c: lbu v0,9(v1)              ; +0x9
8011ce24: andi v0,v0,0x40
8011ce28: beq v0,zero,0x8011ce44
8011ce2c: ori v0,zero,0x4
8011ce30: sb v0,4(v1)               ; +0x4 = 4        (EXE-Executor)
8011ce40: sb v0,5(v1)               ; +0x5 = 6        (Sub 6 = Event-Reach/Idle 0x800517f0)
```

Der ROOM1090-Record hat `behavior = 0x40` → sie spawnt in **State 4 / Sub 6 = stehen**,
NICHT in State 1. Das ist byte-true und im Port bereits korrekt (gemessen, §3).

### 1.4 Die Cutscene übergibt sie in State 1 — `Plc_ret`

sub_scd[3] (Datei 0x24CE–0x26E4). Ende der Szene:

```
0026A4  2e 02 00                 Work_set(kind=2, idx=0)      ; Work = enemy_array[0] = die NPC
0026A8  41 00 00 00 00 00 00 00 64 00   Plc_neck (Kopf lösen)
0026B6  41 03 01 00 …            Plc_neck
0026C0  09 0a 1e 00              Sleep 30
0026C4  42                       Plc_ret                      ⇐ auf der NPC
0026C6  2e 01 00                 Work_set(kind=1, idx=0)      ; Work = Spieler
0026D8  42                       Plc_ret                      ; auf dem Spieler
0026DA  22 02 07 00              Set(2,0x07,0)                ; Cinematic-Latch aus
0026DE  22 01 1b 00              Set(1,0x1b,0)
0026E2  3c 01                    Cut_auto
0026E4  01 00                    Evt_end
```

`Work_set` @0x80040d2c: `kind==2` → `enemy_array` (Basis `0x800acc2c`, Stride 500)
(@0x80040db4-d0), `kind==1` → Spieler `0x800aca54` (@0x80040da4-ac).

`Plc_ret` (Opcode 0x42) @**0x80041f88** — vollständig:

```
80041f88: lw   v0,340(a0)     ; v0 = Work-Entity (thread+0x154)
80041f8c: ori  v1,zero,0x1
80041f90: sb   v1,4(v0)       ; +0x4 = 1        ⇐ STATE 1
80041f94: sb   zero,5(v0)     ; +0x5 = 0
80041f98: sb   zero,6(v0)     ; +0x6 = 0
80041f9c: sb   zero,7(v0)     ; +0x7 = 0
80041fa8: addiu v0,v0,1       ; pc += 1
```

**Damit ist der Mechanismus benannt: die Cutscene selbst gibt Ada per `Plc_ret` den State 1.**

### 1.5 State 1 = die Eskorte-Maschine, `0x8011ce54`

```
8011ce5c: ori  a0,zero,0x3a98     ; a0 = 15000  (Suchradius)
8011ce60: addiu a1,sp,16          ; a1 = &winkel
8011ce64: ori  a2,zero,0x800      ; a2 = 0x800 = 180° (Default-Winkel)
8011ce68: jal  0x800509e4         ; „nächster LEBENDER Gegner in Reichweite?"
8011ce6c: addu a3,zero,zero       ; a3 = 0 (Modus: Winkel spielerrelativ)
8011ce70: andi v0,v0,0xff
8011ce74: beq  v0,zero,0x8011ceac ; kein Gegner -> unterer Zweig
8011ce78: ori  a0,zero,0x5dc      ; (Delay-Slot, BEIDE Zweige) a0 = 1500 = Folge-Radius
  ; Gegner gefunden:
8011ce7c: lh   a1,16(sp)
8011ce80: jal  0x8005070c         ; FOLGE-PUNKT rechnen + nach +0x1dc/+0x1de schreiben
8011ce84: subu a2,zero,a1         ;   Kandidat B = -Winkel
8011ce94: sw   v0,464(v1)         ; +0x1d0 = Distanz NPC -> Folge-Punkt
8011cea8: sb   v0,9(v1)           ; +0x9 = 1
  ; kein Gegner:
8011ceb0: jal  0x8005070c         ; (a0=1500, a1=0x800, a2=-0x800)
8011cec4: sw   v0,464(v1)         ; +0x1d0 = Distanz
8011ced4: sb   zero,9(v0)         ; +0x9 = 0
  ; Unter-Dispatch:
8011cee4: lbu  v0,9(v0)
8011ceec: andi v0,v0,0xf
8011cef8: addiu at,at,0x167c      ; Tabelle 0x8012167c  ( = &StateTab[5] )
8011cf08: jalr v0                 ;   [0] = 0x8011cf20   [1] = 0x8004f100
```

Roh-Bytes zur Absicherung (der ganze Befund hängt an dieser Tabellen-Basis):

```
8011cef4: 12 80 01 3c   = 0x3c018012  lui   at,0x8012
8011cef8: 7c 16 21 24   = 0x2421167c  addiu at,at,0x167c
8012167c: 20 cf 11 80   -> 0x8011cf20
80121680: 00 f1 04 80   -> 0x8004f100
```

**`0x800509e4` = „nächster lebender Gegner"** (EXE):

```
80050a10 / 14 : a0 = 0x800aca4e (Zähler), s1 = a0+478 = 0x800acc2c = &entity[0]
80050a4c: lw  v0,0(s1) / andi 1        ; nur AKTIVE Entities
80050a70: lbu v0,-146(s0)              ; s0 = entity+0x9a  ->  -146 = +0x08 = TYP
80050a78: sltiu v0,v0,0x40             ; nur TYP < 0x40   (= Gegner, keine NPCs)
80050a8c/a0: lw +0x34 / +0x3c          ; dx, dz gegen die eigene Position
80050ab8: jal 0x80065f60               ; SquareRoot0
80050ac0: lh  v1,0(s0) / bltz          ; HP (+0x9a) < 0  ->  ÜBERSPRINGEN (NPCs = -1)
80050ad0: sltu v0,a0,s4 / addu s4,a0   ; Minimum halten; s4 startet = a0 = 15000
Rückgabe: 0 = keiner, sonst index+1
```

**`0x8005070c` = der FOLGE-PUNKT-Rechner** — hier steht das eigentliche Folge-Verhalten:

```
80050728: s4 = 0x800acabe              ; Spieler-Yaw
8005073c/4c: v0 = spieler_yaw + a1     ; Winkel-Kandidat A
80050754: jal 0x80068098               ; RotMatrix
8005076c: sh s1,56(sp)                 ; Vektor = (a0, 0, 0), a0 = Radius (hier 1500)
80050774: jal 0x80067a28               ; Vektor drehen -> Offset (sp+64, sp+68)
80050784-d4: Distanz(EIGENE Pos, spieler + offsetA) -> s1
800507e4-64: dasselbe mit a2           ; Winkel-Kandidat B -> Offset (sp+72, sp+76)
8005086c: lhu v1, playerX
80050870: lhu a0, 64(sp)
80050880: sh  v1,476(a1)               ; +0x1dc = playerX + offsetA.x     ⇐ ZIEL
8005088c/9c: sh v1,478(a1)             ; +0x1de = playerZ + offsetA.z     ⇐ ZIEL
800508a4: sltu v0,v1,s1                ; ist Kandidat B näher?
800508cc: sh  v0,476(a0)               ; ja -> +0x1dc = playerX + offsetB.x
800508e8: sh  v0,478(a0)               ;       +0x1de = playerZ + offsetB.z
Rückgabe: die gewählte Distanz
```

**Das Ziel der NPC ist also ein Punkt, der jeden Frame 1500 Einheiten AM SPIELER neu
gesetzt wird** (bei „kein Gegner" mit Winkel ±0x800 = **direkt hinter dem Spieler**).
Der Root (§1.2) schiebt genau dieses `+0x1dc/+0x1de` jeden Frame in den Pathfinder
`0x80039e7c`. Das ist eine Eskorte, kein Standbild.

### 1.6 Die Lauf-Schleife (Sub-States, EXE-Shared)

| Sub (+0x5) | DECIDE | EXEC | Verhalten (mit Adresse) |
|---|---|---|---|
| 0 | `0x8004f100` | `0x8004f310` | **stehen**: `+0x94=2, +0x95=0, +0x8f=7` @0x8004f354-74, `anim_set` @0x8004f38c |
| 1 | `0x8004f3a4` | `0x8004f4e0` | **gehen** |
| 2 | `0x8004f5e8` | `0x8004f6f0` | drehen (Cone-Verlust) |
| 3 | `0x8004f7dc` | `0x8004f9ec` | angekommen / warten |

**Sub-0-DECIDE `0x8004f100` = der Aufbruch-Trigger:**

```
8004f110: lw    v0,464(v1)        ; +0x1d0 = Distanz zum Folge-Punkt
8004f118: sltiu v0,v0,0x5dd       ; < 1501 ?
8004f11c: bne   v0,zero,0x8004f138; ja -> nichts tun
8004f124: sb    v0,5(v1)          ; nein -> +0x5 = 1   ⇐ LOSLAUFEN
8004f134: sb    zero,6(v0)        ;         +0x6 = 0
8004f148: jal   0x8001ab9c        ; arc_test(playerX, playerZ, 0x4b0)
8004f164: sb    v1,5(v0)          ;   Spieler aus dem Kegel -> +0x5 = 2 (drehen)
8004f1c4: lbu   v0,-13593(v0)     ; player.hit_react
8004f1e0: sb    v1,5(v0)          ;   Spieler getroffen -> +0x5 = 6
```

**Sub-1-EXEC `0x8004f4e0` = die tatsächliche Fortbewegung:**

```
8004f514: sb v0,6(a0)             ; Phase 1
8004f524: sb v0,148(v1)           ; +0x94 = 5   (Geh-Clip)
8004f534: sb zero,149(v0)         ; +0x95 = 0
8004f544: sb v0,143(v1)           ; +0x8f = 7
8004f55c: addiu v0,v0,-64
8004f560: sll  v0,v0,1
8004f568: addiu at,at,27648       ; Tabelle 0x80076c00 [(typ-0x40)*2]
8004f578: sh v0,140(v1)           ; +0x8c = GESCHWINDIGKEIT
8004f588: lh a0,444(v1)           ; +0x1bc  \ Nav-Wegpunkt (vom Pathfinder)
8004f590: lh a1,446(v1)           ; +0x1be  /
8004f5a0: addiu at,at,27649       ; 0x80076c01 = Dreh-Rate
8004f5ac: jal 0x8001aac4          ; yaw-face
8004f5c8: jal 0x8001f314          ; anim_set
8004f5d0: jal 0x800245d8          ; POS_ADVANCE  ⇐ sie bewegt sich wirklich
```

Tabelle `0x80076c00` (u8, byte-gelesen): `75, 48, 75, 48, 75, 48, 75, 48, 70, 48, …`
→ Typ 0x42 = Offset 4 → **Geschwindigkeit 0x4B = 75, Dreh-Rate 0x30 = 48**.

**Sub-1-DECIDE `0x8004f3a4` (Ankunft):** `sltiu v0,v0,0x1f4` @0x8004f3bc → Distanz < **500**
→ `+0x5 = 3` @0x8004f3c8, `+0x6 = 1` @0x8004f3d8.

**Sub-3-DECIDE `0x8004f7dc` (Wieder-Aufbruch):** `sltiu v0,v0,0x3e9` @0x8004f818 → Distanz
≥ **1001** → `+0x5 = 1` @0x8004f824 → sie läuft wieder los.

**Damit ist die Schleife geschlossen:** Ziel 1500 hinter dem Spieler → losgehen bei ≥1501 →
stehenbleiben bei <500 → wieder los bei ≥1001. Klassische Eskorte.

### 1.7 Die „Gegner in der Nähe"-Asymmetrie — und warum sie in ROOM1090 nicht greift

Bei `+0x9 = 1` (Gegner < 15000 gefunden) dispatcht State 1 auf `0x8004f100` — das ist die
**DECIDE-Funktion allein**, ohne EXEC. Sie schreibt nur `+0x5/+0x6` und kehrt zurück
(`jr ra` @0x8004f1fc). Die NPC bewegt sich in diesem Zweig also **nicht**.

In ROOM1090 ist dieser Zweig zum Rettungs-Zeitpunkt **inaktiv**, und zwar aus den Daten belegt:

* Die 7 brennenden Trümmer sind Typ **0x26** und hätten HP — INIT `0x801164b0`:
  `801164f8: ori v0,zero,0x64` / `801164fc: sh v0,154(v1)` → `+0x9a = 100`. Sie würden also
  vom Scan gezählt.
* Aber ihr Spawn hängt an `If / Ck(3,0x81, val 0)` (sub00 @0x21EA/@0x21EE), und
  **sub06 setzt `Set(3,0x81,1)`** (@0x271E) beim Löschen mit dem Feuerlöscher — direkt neben
  `Set(3,0x84,1)` (@0x2722), dem Gate der Ada-Szene. Beim Raum-Durchlauf, in dem Ada spawnt,
  sind die Feuer-Entities also **nicht mehr da**.
* Die 4 Zombies hängen an `Ck(3,0xbb,1)` (sub00 @0x237A); `Set(3,0xbb,1)` steht in **sub03**
  (@0x24D2) — also erst NACH dem sub00-Durchlauf dieses Besuchs. Auf diesem Besuch: keine Zombies.

→ Zum Zeitpunkt „nach der Cutscene" ist in ROOM1090 kein lebender Gegner im Raum, `+0x9 = 0`,
und der volle Decide+Exec-Pfad (0x8011cf20) läuft. **Sie folgt.**

*(Ehrlich markiert: dass der `+0x9=1`-Zweig wirklich „stehenbleiben" bedeutet, ist aus dem
Code gelesen, nicht live gemessen — für ROOM1090 ist es nach obiger Flag-Kette ohnehin
gegenstandslos. Siehe OPEN-1.)*

### 1.8 Gegenprobe: identische Maschine bei Typ 0x40

`0x8011c884` (State 1 von Typ 0x40, Tabelle @0x80121598) ist dieselbe Konstruktion:
`ori a0,0x3a98` @0x8011c88c → `jal 0x800509e4` @0x8011c898 → `jal 0x8005070c` @0x8011c8b0
→ `sb +0x9` → Dispatch @`0x801215ac` (= &StateTab[5]). Nur die Default-Winkel unterscheiden
sich (0x5dc statt 0x800, und `a2 = +a1` statt `-a1`). Das Folge-Verhalten ist also ein
**Familien-Merkmal der STAGE1-NPC-Roots**, kein ROOM1090-Sonderfall.
Ein zweiter, unabhängiger Beleg steht schon im Port-Quelltext:
`re15_port/engine/src/enemy_ai_common.c:11137` — „*The walker+pathfind-15000 chase is
type-0x40 state-1 @0x80116ec8 (jal 0x800509e4, a0=0x3a98=15000)*" (STAGE4-Variante).

---

## 2. Was der Port tut — gemessen

### 2.1 Messaufbau (kein Raum-Sprung an der Install-Sequenz vorbei)

```
re15_pc.exe
  RE15_NO_INTRO=1
  RE15_INPUT_SCRIPT="…"  RE15_INPUT_SCRIPT_START=1     # echter Pad-Input durchs Front-End
  RE15_SET_FLAG="3:0x81,3:0x84"                        # Zustand NACH dem Feuerlöscher
  RE15_DEBUG_JUMP="1090@150"                           # Debug-Menü-JUMP (= Quadrat-Druck)
  RE15_SUBSTART="3@200#1090"                           # sub_scd[3] starten (Rettungs-Cutscene)
  RE15_STATE_LOG=<pfad>
```

### 2.2 Ergebnis

**Spawn (byte-true, stimmt):**
```
F1    [1 t=42 st=0 ss1=0 ss2=0 ss3=0 g=40 mo=0 af=0 @(-30000,-30000,r-25608)]
F6…   [1 t=42 st=4 ss1=6 ss2=0 ss3=0 g=40 mo=2 af=1 @(-30000,-30000,r-25608)]
```
→ `g=0x40` → State 4 / Sub 6, genau wie INIT @0x8011ce1c-40 es vorschreibt. ✅

**Cutscene läuft** (`[substart] sub_scd[3] at F200`), sie wird per `Member_set` positioniert,
spielt Clip 23 / Clip 11, Plc_neck usw.

**Cutscene-Ende — `Plc_ret` greift korrekt:**
```
F705  PL(1252,-4068,rot=5341) … [1 t=42 st=1 ss1=0 ss2=0 ss3=0 g=40 mo=23 af=0 @(635,-3576,r3421)]
```
→ `st=1 ss1=0 ss2=0 ss3=0` = exakt das `Plc_ret`-Wort @0x80041f90-9c. ✅

**Danach — nichts.** Zweiter Lauf, diesmal mit einem Spieler, der nach der Szene wegläuft
(`…,U2,L0.6` ×25 hinter dem Bestätigungs-Teil des Skripts):

```
F705   PL(1252,-4068)   NPC(635,-3576) rot=3421   Abstand   789
F3715  PL(…)            NPC(635,-3576) rot=3421   Abstand  8087   <- Maximum
F5065  PL(6328,-3134)   NPC(635,-3576) rot=3421   Abstand  5710
```

Über **4361 Bilder (≈ 145 s)** State 1 ändert sich ihre Position um **exakt 0** und ihre
Rotation um **exakt 0** — während der Spieler sich bis auf **8087 Einheiten** entfernt, also
das 5,4-fache der Aufbruch-Schwelle 1501 (@0x8004f118). Nur `af` (Anim-Frame) läuft im Kreis:
`mo=23 af=0..29`. Sie steht und atmet.
(Erster Lauf ohne Spieler-Bewegung, 1582 Bilder: identisch bewegungslos.)

### 2.3 Die Port-Stelle

`re15_port/engine/src/enemy_ai_common.c:9042-9046`:

```c
    case 1:   /* Irons overlay state 1 = idle/scripted (wave 2) -> hold the idle pose */
    default:  /* all other NPC states (watchers / overlay) = wave 2 -> hold the idle pose */
        if (e->motion < 24 && s_irons_clip_len[e->motion] <= 1) re15_npc_clip(e, 2);
        re15_npc_anim(e);
        break;
```

Der Port hat für **State 1 überhaupt keinen Inhalt**. Es fehlen, jeweils ohne Port-Entsprechung:

| Original | Zweck | Port |
|---|---|---|
| `0x800509e4` | nächster lebender Gegner < 15000 | **fehlt** |
| `0x8005070c` | Folge-Punkt 1500 am Spieler → `+0x1dc/+0x1de` | **fehlt** |
| `+0x1dc/+0x1de` (Nav-Ziel) | Schreiber **irgendwo** im Port | **kein Schreiber** |
| `0x8011ce54` State-1-FSM | `+0x9`-Latch + Unter-Dispatch | **fehlt** |
| Sub 0/1/3 DECIDE (`f100/f3a4/f7dc`) | 1501/500/1001-Schwellen | **fehlt** |
| Sub 1 EXEC `0x8004f4e0` + `0x800245d8` | Geh-Clip 5 + pos_advance | **fehlt** |

Der Port kennt `re15_npc_executor` (State 4) mit Idle-/Pose-/Turn-Subs, aber die Walk-Subs
sind dort selbst noch als „deferred" markiert (`re15_npc_sub_walk`, Kommentar
`enemy_ai_common.c:8920-8932`: *„the NPC forward translation therefore comes from a DIFFERENT
mechanism … that is not yet RE'd"*). **Genau dieses „andere Mechanismus" ist
`0x8004f4e0` → `0x800245d8` mit `+0x8c` aus `0x80076c00` — jetzt oben belegt.**

---

## 3. ⛔ Der Fehler ist GLOBAL, nicht ROOM1090-spezifisch

Zensus über alle 240 RDT (SCD-Walker mit Work_set-Tracking): **12 `Plc_ret` auf einem
Enemy/NPC-Work-Slot in 8 Räumen** — jeder davon landet im Port in demselben toten `case 1`:

| Raum | Sub | Offset | Work |
|---|---|---|---|
| ROOM1090 | sub03 | 0x26C4 | kind2, idx0 |
| ROOM2001 | sub05 | 0x1D04 | kind2, idx0 |
| ROOM3000 | sub03 | 0x181A | kind2, idx0 |
| ROOM3001 | sub03 | 0x1822 / 0x1828 | kind2, idx0 / idx1 |
| ROOM3071 | sub05 | 0x37D5 / 0x37DF | kind2, idx0 / idx1 |
| ROOM30E0 | sub03 | 0xF90 / 0xF9A | kind2, idx1 / idx0 |
| ROOM30E1 | sub02 | 0xE96 / 0xEA0 | kind2, idx0 / idx1 |
| ROOM6000 | sub06 | 0x1184 | kind2, idx0 |

Zusätzlich erreicht jeder NPC mit Deskriptor ≠ 0x40 State 1 **direkt aus dem INIT**
(`sb 1,4(v1)` @0x8011ccc0) — z.B. ROOM1150/1151 (0x45, beh 0x00), ROOM1171 (0x49, beh 0x00),
ROOM3010/3011/3020/3021/3060/3070 (0x40/0x42, beh 0x00), ROOM6000 (0x4D, beh 0x00).
**Die Reparatur gehört in `re15_npc_ai_tick` State 1 (shared), nicht in ROOM1090.**

---

## 4. Was zu tun ist (nächste Phase, mit den Adressen von oben)

1. `re15_npc_follow_point(radius, angA, angB)` = `0x8005070c` — Ziel nach `steer_dest_x/z`
   (= `+0x1dc/+0x1de`) und Rückgabe → `ai_dist` (= `+0x1d0`).
2. `re15_npc_nearest_enemy(range)` = `0x800509e4` — Filter `type < 0x40` (@0x80050a78) und
   `hp >= 0` (@0x80050ac8), Winkel via `atan2_q12` − `player_yaw` (@0x80050b4c-68).
3. `case 1:` = `0x8011ce54` — `(15000, 0x800, 0)` @0x8011ce5c-68, dann `(1500, ang, −ang)`
   @0x8011ce80, `+0x9 = 0/1` @0x8011cea8/@0x8011ced4, Unter-Dispatch @0x8012167c.
4. Sub-DECIDE/EXEC-Paare 0/1/2/3 mit den Schwellen **1501** (@0x8004f118), **500**
   (@0x8004f3bc), **1001** (@0x8004f818), Cone **0x4b0** (@0x8004f14c) / **0x400**
   (@0x8004f3f4) / **0x200** (@0x8004f7f8).
5. Geh-EXEC `0x8004f4e0`: Clip **5** (@0x8004f524), Speed **75** / Dreh-Rate **48** aus
   `0x80076c00[(typ−0x40)*2]` (@0x8004f568/@0x8004f5a0), Ziel `+0x1bc/+0x1be` (@0x8004f588),
   `pos_advance` (@0x8004f5d0).
6. Der Root-Pfad `+0x1dc/+0x1de → 0x80039e7c → +0x1bc/+0x1be` (@0x8011cbd0-e8) muss für NPCs
   pro Frame laufen — im Port existiert der Nav-Steer bereits (Zombie-/Hund-Pfad), er wird für
   NPCs nur nicht mit einem Ziel gefüttert.

---

## 5. OPEN

* **OPEN-1** — Der `+0x9 = 1`-Zweig (Gegner < 15000) dispatcht auf `0x8004f100`, eine reine
  DECIDE-Funktion ohne EXEC. Aus dem Code gelesen = „steht still, entscheidet weiter".
  Für ROOM1090 gegenstandslos (§1.7), aber für ROOM3070/30E0/6000 (NPC + lebende Gegner im
  selben Raum) muss das an einem echten DuckStation-Savestate gegengeprüft werden, bevor es
  portiert wird.
* **OPEN-2** — Der reale Auslöse-Pfad der Rettungs-Cutscene wurde **nicht** durchgespielt:
  gemessen wurde mit `RE15_SET_FLAG="3:0x81,3:0x84"` + `RE15_SUBSTART=3`. Die Flag-Kette
  (`sub06` @0x271E/@0x2722 → `Ck(3,0x84)` in sub00 @0x22A6 → `Evt_exec(3)` @0x22E0) ist aus den
  RDT-Bytes belegt, aber ob der Port sie im echten Spielfluss (Feuerlöscher benutzen → Raum
  verlassen → zurück) fehlerfrei fährt, ist offen. Im ersten Messlauf ohne `SUBSTART` wurde
  `sub03` NICHT gestartet (die NPC blieb auf `st=4 ss1=6`) — separat zu prüfen, ebenso die
  Dauer-Warnung `[scd] WARN: event 7 DROPPED (no free event slot)`, die in ROOM1090 jeden
  Frame feuert.
* **OPEN-3** — Was passiert bei Raumwechsel, während sie folgt? In ROOM1090 löscht `sub03`
  ihr Spawn-Gate (`Set(3,0x84,0)` @0x24CE), sie wird beim Wieder-Betreten also nicht neu
  gespawnt. Ob das Original sie in den Nachbarraum „mitnimmt" (Typ 0x42 spawnt auch in
  ROOM1050 und ROOM11C0), ist ungeklärt.
* **OPEN-4** — Der Name. Das RE1.5-Skript nennt sie nur „Woman". Die Zuordnung
  Typ 0x42 → „Ada" ist Nutzer-/Community-Sprachgebrauch und in den RE1.5-Daten nicht belegt.
