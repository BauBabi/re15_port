# Item-Stapel: zweites Item wird ohne neuen Tastendruck mitgenommen

**Nutzer-Report:** "Wenn mehrere Items uebereinander liegen, nimmt der Spieler automatisch das
darunter auch gleich mit nach dem Bestaetigen des Items davor, statt noch einmal druecken zu
muessen."

**Befund (statisch, proven):** Port-Bug. Der Bestaetigungs-SQUARE des Item-Modals ist im Port im
selben Host-Frame noch als Press-Edge sichtbar, wenn `re15_game_step` nach dem Modal-Ende
weiterlaeuft — der ACTION-Scan feuert das naechste ueberlappende ITEM-AOT sofort. Das Original
verhindert genau das dreifach: (1) Frame-Ordnung (Modal-FSM tickt NACH dem Spieler-Dispatcher),
(2) Pauseflags-Freeze des Dispatchers waehrend des Prompts, (3) expliziter Edge-Verbrauch
`DAT_800ac768 = 0xFFFF` beim Confirm, der die virtuelle Flanke des GESAMTEN Folgeframes nullt.
Dazu kommt (4) der Re-Arm-Guard des sce-9-Handlers auf den Modal-State.

---

## 1. Port-Stand (die Kette, Datei:Zeile)

Reihenfolge im Host-Frame (PC-Loop, ein Durchlauf):

1. **`re15_port/platform/pc/main.c:3854-3866`** — Modal-Tick zuerst:
   `if (... && re15_item_modal_active()) re15_item_modal_tick(re15_pad_virtual_word(pc_pad_config(g_engine.pad_pressed)), ...)`.
   Der Tick bekommt die virtuelle Flanke DIESES Frames.
2. **`re15_port/engine/src/item_modal_common.c:266-268`** (state 6, Yes/No):
   `if (!(pad_edge & 0x4000)) return; ... s_prompt = 0; s_state = 7; again = 1;` — der Confirm
   laeuft per `again`-Schleife im SELBEN Tick in state 7 (Grant, Zeilen 274-299) und endet mit
   `s_state = 0` (Z.299). Gleiches beim Can't-Carry-Dismiss Z.261-263 (`pad_edge & 0xc000`).
   (Das Same-Tick-Durchfallen ist fuer sich byte-true — im Original faellt case 6 ohne `break`
   in case 7, s.u.)
3. **`re15_port/platform/pc/main.c:4640`** — `gctx.pad_pressed = pc_pad_config((uint16_t)g_engine.pad_pressed)`:
   dasselbe Frame-Edge-Wort, das gerade den Confirm geliefert hat.
4. **`re15_port/platform/pc/main.c:5167`** — `re15_game_step(&gctx)`.
5. **`re15_port/engine/src/game_step_common.c:799`** — `if (re15_item_modal_active()) return;`
   Der Freeze ist NUR dieser Early-Return; das Modal ist seit Schritt 2 inaktiv → er greift nicht
   mehr. (Der Port setzt fuer das Item-Modal KEINE `g_re15_pauseflags` — als OPEN dokumentiert in
   game_step_common.c:882-885.)
6. **`re15_port/engine/src/game_step_common.c:816`** —
   `g_aot_action_pressed = (c->pad_pressed & RE15_PAD_BIT_SQUARE) ? 1 : 0;` → **1**, denn der
   Bestaetigungs-SQUARE IST der Press-Edge dieses Frames.
7. **`re15_port/engine/src/game_step_common.c:1499`** — Normalpfad ruft
   `re15_aot_scan(pl->x, pl->z, ...)`.
8. **`re15_port/engine/src/aot_common.c:1222`** — ITEM-Fire-Bedingung
   `(gen_reach && g_aot_action_pressed && !msg_block && !action_fired)`:
   - `gen_reach` = FORWARD-620-Test (Items sind 0x31/0x51, Bit 0x20) — der Spieler steht noch vor
     dem Stapel → wahr fuer das zweite, ueberlappende ITEM-AOT.
   - `msg_block` (aot_common.c:829) liest nur `g_scd.message_query`/`message_display_frames`;
     der Item-Prompt lebt modal-intern in `s_prompt` (item_modal_common.c:44) → **0**.
   - `action_fired` ist scan-lokal (aot_common.c:798) → 0 am Scan-Anfang.
9. **`re15_port/engine/src/aot_common.c:1287`** — `re15_item_modal_start(...)`; dessen Guard
   `if (s_state != 0) return;` (item_modal_common.c:136) greift nicht mehr, weil `s_state`
   bereits 0 ist. → **Zweites Modal startet im Bestaetigungs-Frame, ohne neuen Druck.**

Nebenbefund (gleiche Wurzel): weil der Modal-Tick (main.c:3854) im Loop VOR `re15_game_step`
(main.c:5167) steht, tickt der INIT-State des Modals erst einen Host-Frame NACH dem Armen —
im Original laeuft er im Trigger-Frame selbst (FUN_8001db28 @0x8001ce34 NACH dem Scan
@0x8001ce0c). 1-Frame-Praesentations-Offset.

## 2. Original-Mechanismus (Belege @0x…)

### 2.1 Frame-Ordnung im Hauptloop (FUN_8001c6e8, ghidra1_V2.txt)

```
8001ce0c  jal FUN_80031c44    ; Spieler-Dispatcher = EINZIGER Weg zum ACTION-Scan
8001ce14  jal FUN_8002bd44
8001ce1c  jal FUN_800436a8    ; AUTO-Pool-Scan
8001ce34  jal FUN_8001db28    ; ITEM-MODAL-FSM — NACH dem Dispatcher
```

Die ACTION-Scan-Callsite (cmd-1-DECIDE, substate 0):

```
80031fbc  lw   v0,-14484(v0)      ; DAT_800ac76c (virtuelles EDGE-Wort)
80031fc4  andi v0,v0,0x80         ; ACTION-Flanke
80031fc8  beq  v0,zero,0x80031ff4 ; keine Flanke -> kein Scan
80031fe4  jal  0x80042bac         ; FUN_80042bac(player, 1, 0x10)
```

Der Modal-Abschluss (state 6→7→0) passiert @0x8001ce34, also IMMER nach jedem moeglichen
ACTION-Scan desselben Frames. FUN_8001db28 case 6 faellt bei geschlossener Box ohne `break` in
case 7 (Decompile RE_15_Quellcode_V2/FUN_8001db28.c Z.153-181) und endet `DAT_80072d3b = 0`.

### 2.2 Pauseflags-Freeze waehrend des Prompts

Item-Modal state 5 oeffnet den Prompt mit Maske 0xff000000
(FUN_8001db28.c Z.137/149: `FUN_80027e68(0,0x100,uVar9,0xff000000)`); FUN_80027e68 setzt im
0x100-Zweig `DAT_800aca40 = param_4 | DAT_800aca40` (RE_15_Quellcode_V2/FUN_80027e68.c Z.27)
und sichert den Vor-Wert nach `DAT_800b853c` (Z.16/35). Der Spieler-Dispatcher:

```
80031c54  lw   a0,-13760(a0)     ; g_pauseflags (0x800aca40)
80031c78  bltz a0,0x80031da8     ; Bit 31 (0xff000000) -> komplette cmd-FSM uebersprungen
```

→ Solange die Box offen ist, laeuft KEIN DECIDE und damit kein ACTION-Scan.

### 2.3 Der Edge-VERBRAUCH beim Confirm (die Kernantwort auf Frage 3)

Message-VM FUN_80028134, case 4 (Yes/No-Wait):

```
80028568  lw   v1,-14484(v1)     ; DAT_800ac76c
80028570  andi v0,v1,0x4000      ; Confirm-Flanke
80028574  beq  v0,zero,LAB_800285b8
80028578  _ori v0,zero,0xffff    ; (Delay-Slot)
80028588  sw   v0,-14488(at)     ; DAT_800ac768 = 0xFFFF   <-- HELD-Wort geflutet
8002859c  sb   v0,0(v1)          ; DAT_800b8520 &= 0x7f    (Box zu)
800285a4  sw   a0,-13760(at)     ; DAT_800aca40 = DAT_800b853c (Pauseflags-Restore)
800285a8  jal  FUN_8002877c      ; Prompt-Callback (u.a. Welt-Modell-Loeschung)
```

Wirkung im Folgeframe — Pad-Builder FUN_80030444:

```
80030484  lw a0,-14488(a0)   ; altes HELD (= 0xFFFF nach dem Confirm)
800304ac  sw a0,-14480(at)   ; DAT_800ac770 (prev) := 0xFFFF
80030490  sw zero,-14488(at) ; HELD neu aus dem Roh-Pad aufgebaut
80030534  lw v0,DAT_800ac770 ; \
8003053c  lw a1,DAT_800ac768 ;  > edge = (prev ^ held) & held
8003057c  sw v0,-14484(at)   ; /  DAT_800ac76c
```

Mit `prev = 0xFFFF` ist `edge = (~held) & held = 0` — die virtuelle Flanke des GESAMTEN
Folgeframes ist fuer JEDE Taste 0. Erst ein Druck ab Frame N+2 erzeugt wieder eine Flanke.
Denselben Verbrauch machen auch die anderen Dismiss-Pfade: page-advance case 2
(@0x80028460 `_ori 0xffff` / @0x80028468 `sw →DAT_800ac768`) und full-message case 5
(@0x800286a0 / @0x800286b0).

### 2.4 Vierter Riegel: der sce-9-Handler-Guard

```
8004332c  lbu v0,11579(v0)        ; DAT_80072d3b (Modal-State)
80043334  bne v0,zero,0x80043368  ; State != 0 -> jr ra, KEIN Re-Arm
8004334c  sb  v1,11579(at)        ; sonst: State := 1 (Modal armen)
80043364  sw  a0,-13776(at)       ; 0x800aca30 := Record-Payload
```

Selbst in der hypothetischen Reihenfolge, in der der Message-Tick (FUN_800280b4, gerufen aus
FUN_80010000 @0x80010044, Flip-Service) die Pauseflags noch im Confirm-Frame VOR dem
Spieler-Dispatcher restauriert und der Scan die Frame-Flanke (0x80) noch saehe: der Handler
sieht `DAT_80072d3b == 6` (die 6→7→0-Transition laeuft erst @0x8001ce34) und tut nichts —
der Druck ist verbraucht.

### 2.5 Antwort auf Frage 3

**Ja** — das Original gibt bei zwei ueberlappenden Item-AOTs das zweite Item erst nach einem
NEUEN SQUARE-Druck, in beiden denkbaren Intra-Frame-Reihenfolgen:

- *Dispatcher vor Message-Tick:* Confirm-Frame N: Dispatcher gefreezt (bltz @0x80031c78) →
  kein Scan; Modal endet in N (oder N+1). Frame N+1: `edge == 0` (2.3) → DECIDE ruft den Scan
  gar nicht. Frame N+2: neue Flanke noetig.
- *Message-Tick vor Dispatcher:* Confirm-Frame N: Scan liefe, aber Handler-Guard @0x80043334
  (State noch 6) schluckt den Treffer. Frame N+1: `edge == 0`. Frame N+2: neue Flanke noetig.

Der "Edge-Verbrauch" liegt also NICHT in FUN_80042bac (der Scan hat kein Re-Arm-Gate und kein
prev-inside-Feld) und nicht in einem expliziten Loeschen von DAT_800ac76c, sondern in
`sw 0xFFFF → DAT_800ac768` @0x80028588 (Folgeframe flankenfrei) plus der Frame-Position
@0x8001ce34 + bltz @0x80031c78 + Guard @0x80043334 (Confirm-Frame selbst scanfrei bzw.
handler-verweigert).

## 3. Divergenz (kompakt)

| Mechanismus | Original | Port |
|---|---|---|
| Modal-Tick-Position im Frame | @0x8001ce34, NACH Dispatcher @0x8001ce0c | main.c:3854, VOR game_step main.c:5167 |
| Freeze beim offenen Prompt | pauseflags 0xff000000 → bltz @0x80031c78 | nur `re15_item_modal_active()`-Early-Return (game_step_common.c:799) — nach Modal-Ende sofort weg |
| Edge-Verbrauch beim Confirm | `DAT_800ac768=0xFFFF` @0x80028588 → Folgeframe-Flanke 0 | fehlt; game_step_common.c:816 liest die rohe Frame-Flanke |
| Re-Arm-Guard | @0x80043334 (State noch 6 im Confirm-Frame) | item_modal_common.c:136 wirkungslos, weil State schon 0 |

## 4. Fix-Vorschlag

**Primaer (byte-true Frame-Ordnung):** den `re15_item_modal_tick`-Aufruf (main.c:3854-3866)
hinter `re15_game_step(&gctx)` (main.c:5167) verlegen — Original-Ordnung @0x8001ce0c (Scan) →
@0x8001ce34 (Modal-FSM). Wirkung: Im Confirm-Frame sieht game_step das Modal noch aktiv
(Early-Return game_step_common.c:799) → kein Scan; im Folgeframe ist `c->pad_pressed`
flankenfrei (Host-Edge ist per Definition 1 Frame). Bonus: der Modal-INIT laeuft dann wie im
Original im Trigger-Frame (der heutige 1-Frame-Offset verschwindet). Achtung beim Umbau: der
Tick haengt in der `if/else if`-Kette mit dem Menu-Freeze (main.c:3867) und der 30-Hz-Kadenz
`(target_fps == 30 || (frame_count & 1) == 0)` — Kadenz-Gate mitnehmen.

**Sekundaer (voller byte-true Edge-Verbrauch):** das virtuelle prev/held/edge-Tripel des
Originals modellieren und bei JEDEM Message-Dismiss `held := 0xFFFF` schreiben
(@0x80028588, @0x80028468, @0x800286b0) — dann ist wie im Original auch jede ANDERE Taste im
Folgeframe flankenlos (heute wuerde z.B. ein exakt im Folgeframe gedrueckter Knopf im Port
feuern, im Original nicht). Das betrifft auch die SCD-Message-Dismiss-Pfade, nicht nur das
Item-Modal.

**Minimal-Alternative** (falls die Loop-Umstellung zu riskant erscheint): ein Latch
"Modal endete in diesem Frame" (gesetzt in item_modal_common.c beim Uebergang state→0 aus 7/8),
das in game_step_common.c:816 `g_aot_action_pressed` auf 0 zwingt. Deckt exakt den gemeldeten
Bug, laesst aber den Folgeframe-Verbrauch anderer Tasten offen und ist Mechanismus-fremd.

## 5. Offen

- Die Intra-Frame-Position des Message-VM-Ticks (FUN_800280b4 via FUN_80010000 @0x80010044,
  Flip-Service) relativ zur Gameplay-Task FUN_8001c6e8 ist statisch nicht abschliessend
  geklaert. Fuer das Ergebnis irrelevant (beide Reihenfolgen in §2.5 durchgerechnet, beide
  verhindern das Doppel-Feuern), aber fuer ein exaktes "in welchem Frame grantet das Original"
  waere eine dynamische Messung noetig: PCSX-Redux-Breakpoints auf 0x800285a8
  (Confirm-Callback) und auf den state-7-Abschluss `sb zero,DAT_80072d3b` (@0x8001e0e0) mit
  Frame-Zaehler-Vergleich (Skill `re15-pcsx-watchpoint`).
- Ob der Port nach dem Primaer-Fix den Folgeframe-Verbrauch ANDERER Tasten (Original blankt
  ALLE virtuellen Flanken fuer 1 Frame) braucht, ist eine eigene Paritaetsfrage — beobachtbar
  z.B. bei sofortigem Zielen/Menue-Oeffnen direkt nach einem Text-Dismiss.
