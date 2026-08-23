# Aim-Target-Reacquire 0x80037250

Alle RE-Fragen sind beantwortet — Mechanismus, einziger Aufrufer, Trigger, Filter-Vergleich mit FUN_8003703c, Port-Stand. Hier die Rohdaten für das Dossier.

---

# RE-DOSSIER: Zyklisches AIM-TARGET-REACQUIRE FUN_80037250 (Gun-FSM Sub5 „L1-Retarget")

## 1) MECHANISMUS (vollständig, disasm-belegt)

### 1.1 Kontext: Die Gun-Aim-Zustandsmaschine (FUN_80032e9c)

Root `FUN_80032e9c` dispatcht über `DAT_800aca5a` (Sub-State-Byte) in die Tabelle `0x800740f4`:

```
80032ea0  lbu v0,-0x35a6(v0)   ; DAT_800aca5a (Sub-State)
80032eac  sll v0,v0,0x2
80032eb4  addiu at,at,0x40f4   ; Tabelle @0x800740f4
80032ebc  lw  v0,0x0(at)
80032ec4  jalr v0
```
Tabelle @0x800740f4 (Ghidra splittet sie in PTR_LAB_800740f4 + PTR_LAB_80074100, physisch EIN Block):
- sub0 `0x800740f4` → `LAB_80032f18` = RAISE (Clip 6, ruft `FUN_8003703c(0x7530)` @0x80032fa8)
- sub1 `0x800740f8` → `LAB_80033180` = HOLD (Clips 8/10/12)
- sub2 `0x800740fc` → `LAB_80033460` = DISCHARGE (Recoil-Clips 7/9/11; inner Dispatch @0x800334d8 über `0x80074100 + DAT_800aca5d(Waffe)*4`)
- sub3 `0x80074100` → `LAB_80033c74` = LOWER
- sub4 `0x80074104` → `LAB_80033d7c` = RELOAD (Clip 0xD @0x80033da4-ac)
- **sub5 `0x80074108` → `LAB_80033eec` = RETARGET — der EINZIGE Aufrufer von FUN_80037250**
- sub6/7 → `LAB_800337bc`, sub8/9 → `LAB_800338a8` (Fire-Executor)

`DAT_800aca5a`/`DAT_800aca5b` sind ein Paar: `sh N` schreibt Sub-State (low) UND Init-Latch (high) zugleich (`sh 5` ⇒ sub=5, Latch=0 → Cold-Init; `sh 0x101` ⇒ sub=1, Latch=1 → HOLD WARM ohne Re-Init).

### 1.2 Trigger — wann feuert sub5 (Antwort: NUR Taste, NICHT Reload, NICHT Zielverlust)

**Einziger Schreiber von 5 nach `DAT_800aca5a` in der Gun-Maschine = HOLD @0x800332f8** (alle `sh/sb`-Stores auf aca5a im Bereich 0x80032e9c–0x80034010 geprüft: 0x800330fc→1, 0x8003314c→1, 0x8003316c→1, 0x80033200→3, **0x800332f8→5**, 0x80033328→2, 0x80033378→4, 0x8003364c→3, 0x8003367c→1, 0x80033d54→0, 0x80033ebc→1, 0x80033f70→0x101, 0x80033fbc→3, 0x80033fe4→1):

```
800332e0  lhu v0,-0x389e(v0)   ; DAT_800ac762 = ROHES Pad-EDGE-Wort (byte-swapped Layout,
800332e8  andi v0,v0,0x4       ;   L1 = 0x04; vgl. menu_common.c:38/1386: CROSS=0x40, START=0x800)
800332ec  beq v0,zero,LAB_800332fc
800332f0  _ori v0,zero,0x5
800332f8  sh  v0,DAT_800aca5a  ; sub=5 + Latch=0 (Cold-Entry)
```
⇒ **L1-PRESS-EDGE während Gun-HOLD.** Danach FÄLLT der Code DURCH in die Fire-/Reload-Prüfung: Fire-held `DAT_800ac768&0x40` + `FUN_8004ea6c()!=0` → `sh 2` @0x80033328 (DISCHARGE) bzw. Edge `DAT_800ac76c&0x40` + `FUN_8004eb70()!=0` + Waffe `DAT_800aca5d<9` (`sltiu` @0x80033368) → `sh 4` @0x80033378 (RELOAD) — **Fire/Reload im selben Frame ÜBERSCHREIBEN die 5.** Elevation-Wechselframes (`sh`-Exits @0x8003323c/0x80033288/0x800332d4) verlassen HOLD VOR dem L1-Check — L1-Edge in genau so einem Frame verfällt.

**RELOAD löst KEIN Reacquire aus:** sub4-Exit ist `sh 1` @0x80033eb8 (→ HOLD cold) + `FUN_8004ebdc` (Munition) + SE `FUN_80045024(0x1030001)` @0x80033ed0. Die Task-Prämisse „Post-Reload-Zielwechsel" ist damit **widerlegt** — es ist der L1-Zielzyklus. (Auch der Port-Kommentar player_common.c:622 „post-reload-exec @0x80033fbc" ist ein Mislabel: 0x80033fbc ist der R1-Release-Exit von sub5.)

**Aufrufer-Census FUN_80037250:** exakt EIN Callsite `jal` @0x80033f50 (Ghidra `XREF[1]: 80033f50(c)`; zusätzlich verifiziert: 0 Treffer für `addr FUN_80037250` und 0 Treffer für das Byte-Muster `50 72 03 80` als Datenpointer). Die Melee-Maschine liest `DAT_800ac762` nirgends (einziger Read im FSM-Bereich = 0x800332e0) — L1-Retarget existiert NUR für Schusswaffen.

### 1.3 Sub5-Handler LAB_80033eec (der Rahmen um den Scan)

Cold-Entry (`DAT_800aca5b==0` @0x80033f00-08):
```
80033f14  lhu v1,DAT_800acaec       ; Elevation-Flags (0x8000=UP @0x8003322c, 0x2000=DOWN @0x80033274, 0x4000=neutral @0x800332c0)
80033f1c  sb  7,DAT_800acae3        ; Blend/Crossfade = 7
80033f24  sb  1,DAT_800aca5b        ; Latch=1
80033f2c  sb  0,DAT_800acae9        ; Frame-Zähler = 0
80033f30-44  v0 = ((acaec>>15)<<1) + 8 + ((acaec>>11)&4)
80033f4c  sb  v0,DAT_800acae8       ; Clip = 8 / 10 / 12 (HOLD-Clip je Elevation)
80033f50  jal FUN_80037250
80033f54  _ori a0,zero,0x7530       ; TOTER Parameter: FUN_80037250 überschreibt a0 sofort
80033f5c  sb  v0,DAT_800acaf3       ; Ergebnis-Slot-Index (⚠️ RAISE speichert hier den 703c-RETURN-CODE @0x80032fbc — Semantik-Overload)
80033f60-64  if (v0==0):
80033f70    sh 0x101,DAT_800aca5a   ;   KEIN Ziel: → HOLD WARM (kein Re-Init!)
80033f78    sb 0,DAT_800acae3       ;   Blend = 0
```
Warm-Pfad (jeden Frame, LAB_80033f84):
```
80033f8c  lw  a0,DAT_800acbfc       ; aktuelles Ziel
80033f90  ori a1,zero,0xc8          ; Slew-Step 200
80033f94  jal FUN_8001a8f8(target+0x34, 0xc8)   ; Yaw-Slew auf Ziel (player+0x6a via DAT_800ac784)
80033fa0-ac  DAT_800ac768 & 0x100 (R1 gehalten)?
80033fbc    nein: sh 3,DAT_800aca5a ;   → LOWER
80033fb0  _ori a1,zero,0x64         ; Toleranz 100
80033fcc  jal FUN_8001a9cc(target+0x34, 0x64)   ; Ausrichtungs-Test: 0 wenn |bearing−yaw|<100 (mod 0x1000)
80033fd4  bne v0,zero → bleibe sub5
80033fe4    sonst: sh 1,DAT_800aca5a ;  ausgerichtet → HOLD (cold)
80033ff8  jal FUN_8001f314(DAT_800acbc4, DAT_800acbc8, 0, 0x200)  ; Anim-Tick
```
Sub5 hat **keinen manuellen Turn** (kein `DAT_800acabe`-Zugriff) — während des Re-Face dreht ausschließlich der Slew. `FUN_8001a9cc` (RE_15_Quellcode_V2): `d=(bearing−yaw+tol)&0xfff; return (d<2*tol)?0:(d<=0x800?+tol:−tol)`.

### 1.4 FUN_80037250 selbst — der zyklische Scan (jede Zeile belegt)

```
80037250  lui  a0,0x800b / addiu a0,-13746   ; a0 = 0x800aca4e (Entity-Count-Byte) — ÜBERSCHREIBT a0: KEIN Radius-Parameter!
80037258  addiu a1,a0,478                    ; a1 = 0x800acc2c (Enemy-Slot-1-Basis; Player-Slot0 = 0x800aca38, Stride 0x1F4=500)
8003725c/68  v0 = 0x10624DD3                 ; Magic für signed /500
80037264  lw  v1,-13316(v1)                  ; DAT_800acbfc = aktueller Ziel-Zeiger
8003726c  subu v1,v1,a1                      ; Byte-Offset zum Slot-1-Array
80037270-88  mult/mfhi/sra 5/subu → a2 = i   ; START-INDEX = Index des AKTUELLEN Ziels (0-basiert ab Slot 1)
                                             ;   (Decompile-Artefakt „+0x1ffd4cf5": uint*-Arithmetik, *4 = −0x800ACC2C)
80037278  lbu a0,0(a0)                       ; count = [0x800aca4e]
8003728c-a0  v0 = 500*(i&0xff)               ; (i*31*4+i)*4
800372a4  beq a0,zero → Exit                 ; count==0 → return 0, Latch unverändert
800372a8  addu a1,v0,a1                      ; ptr = &enemy[i]
Schleife:
800372ac  addiu a2,a2,1                      ; i++  ← Scan beginnt beim NÄCHSTEN Slot nach dem aktuellen Ziel
800372b4  sltiu v0,(a2&0xff),0x14            ; WRAP bei 20: i>=0x14 →
800372c0-c8    a2=0; a1=0x800acc2c           ;   zurück auf Slot 1 (ZYKLISCH über 20 Enemy-Slots)
800372cc/d4  lw 0(a1); andi 1                ; FILTER 1: Wort+0x00 Bit0 aktiv (inaktiv → weiter OHNE count--)
800372e0/e8  lh v0,154(a1); bltz → skip      ; FILTER 2: entity+0x9A (HP, s16) >= 0  → Tote/NPCs (hp=-1) ausgeschlossen
800372f0/f8  lbu 9(a1); andi 0x60; bne→skip  ; FILTER 3: (entity+0x09 & 0x60)==0 (AI-Freeze 0x20 / Cutscene 0x40)
80037304/08  GEFUNDEN: a3 = i+1; break       ; Ergebnis = 1-basierter absoluter Slot-Index
8003730c/14  skip: count--; weiter solange count!=0   ; count-- NUR bei aktiv-aber-unwählbar
Exit:
80037324  beq a1,zero → return 0             ; nichts gefunden: DAT_800acbfc UNVERÄNDERT
80037328-48  v1 = 0x800aca38 + 500*idx       ; (addiu a0,v0,-452 = Basis 0x800aca38)
8003734c  sw  v1,0(v0)                       ; DAT_800acbfc = neues Ziel
80037354  return idx                         ; 0 = keins
```

**Semantik (beweisbar aus obigem):**
- Startet beim aktuellen Ziel-Slot **+1**, läuft zyklisch über die 20 Enemy-Slots (Wrap `0x14` @0x800372b4), nimmt den **ERSTEN** wählbaren Treffer in Slot-Reihenfolge — **KEINE Distanz, KEIN Radius** (der `a0=0x7530` des Callers @0x80033f54 ist tot: a0 wird @0x80037250 sofort überschrieben), **KEINE Prioritäts-Buckets**.
- Ist das aktuelle Ziel das einzig wählbare, kommt der Zyklus nach einer Runde wieder bei IHM an → **Selbst-Reselect** (Rückgabe = alter Index).
- Count-Terminierung ≡ genau eine volle Runde: jede aktive Entity wird pro Runde genau einmal besucht und dekrementiert count, wenn unwählbar; alle unwählbar → return 0.
- **Filter-Vergleich mit FUN_8003703c** (Klassen K1/K2/K3, port-dokumentiert game_step_common.c:359-386): 37250 akzeptiert **K1 ∪ K2** (hp>=0, Bit `+0x9&0x80` egal — kein Bucket-Ranking) und schließt **K3** (hp<0: Leichen/NPCs) aus; gleiche `+0x9&0x60`-Ausnahme wie 703c (@0x8003710c); im Gegensatz zu 703c ohne `SquareRoot0`-Distanz (703c @0x800370f8) und ohne Radius. Feld-Mapping (Port-etabliert): `+0x9A`→`actor.hp`, `+0x09`→`actor.grid_id`, `+0x34/+0x3C`→x/z.

## 2) PORT-STAND

- `re15_port/engine/src/player_common.c:666-686` — **die Divergenz**: „L1 RETARGET" ist als *L1-GEHALTEN in READY → jeden Frame `re15_player_aim_target(30000,…)` (= Nächster-Scan) + Slew 0xC8* implementiert. Original: L1-**EDGE** → eigener Sub-State 5, **EIN** zyklischer `FUN_80037250`-Pick ohne Radius, dann Slew bis ±100, zurück nach HOLD. Der Port-Kommentar dort zitiert 0x80033eec/0x800332f8 bereits, aber mit falscher Semantik („nearest", „held").
- `re15_port/engine/src/re15_damage.c:961-985` — `re15_player_aim_target` (703c-Ableger: nearest all-around, Filter `hit_radius_min>0`, `state!=7`, `hp>=0`; ohne `grid_id&0x60`, ohne Buckets, gibt nur Koordinaten zurück). **Es existiert KEIN persistenter Ziel-Latch** (kein Äquivalent zu `DAT_800acbfc` im Aim-Pfad; `neck_target_slot` in `game_step_common.c:390-421` = byte-true 703c `re15_autolook_scan`, aber nur für den Neck).
- Schuss-Resolver (`re15_damage.c:336ff`, FUN_80011f50-Kern) zielt selbst (nearest-in-front) und liest `DAT_800acbfc` NICHT — der beobachtbare Effekt des Zyklus ist das **Facing** (`rot_y`-Konvergenz), das dann den Schuss-Empfänger bestimmt.
- `RE15_ACTOR_MAX = 16` (`re15_port/include/re15_actor.h:22`) vs. Original-Wrap 20.
- Kein Vorkommen von `80037250` im gesamten Port (grep-verifiziert).

## 3) IMPLEMENTIERUNGS-PLAN

1. **`re15_damage.c`: Aim-Latch einführen.** `static int s_aim_target_slot = 1;` (Spiegel von `DAT_800acbfc` als Slot-Index; Default 1 = der 703c-Fail-Store `0x800acc2c` @0x800371d8-dc). `re15_player_aim_target()` schreibt zusätzlich den Sieger-Slot in den Latch (Fail → 1) und bekommt eine Slot-Rückgabe (oder Out-Param); Raum-Reset wischt auf 1.
2. **`re15_damage.c`: `int re15_player_aim_cycle(void)`** — byte-true FUN_80037250: `i = s_aim_target_slot`; eine volle Runde `i=i%W+1` (Wrap W = RE15_ACTOR_MAX−1; Original 0x14 @0x800372b4 — dokumentierte Anpassung, Slots 16-20 existieren im Port nicht); Filter exakt `active` (@0x800372d4) && `hp>=0` (@0x800372e0/e8) && `!(grid_id&0x60)` (@0x800372f0/f8) — **NICHT** `hit_radius_min`/`state==7` übernehmen, 37250 filtert das nicht; Treffer → Latch setzen (@0x8003734c) + Slot zurück; sonst 0, Latch unverändert (@0x80037324). Kein Radius (toter a0 @0x80033f54).
3. **`player_common.c:666-686` ersetzen:** L1-EDGE (static prev-pad; Original liest das ROHE Edge-Wort `DAT_800ac762&4` — Port-`RE15_PAD_BIT_L1`-Edge ist das Äquivalent) in READY && !recoil && !melee → neue Phase `RE15_AIM_RETARGET` (sub5): einmal `re15_player_aim_cycle()`; 0 → READY bleiben mit Blend 0 (@0x80033f70/78: HOLD warm, `acae3=0`); sonst Clip=Hold-Clip re-init (`anim_frame=0, anim_frac=7` @0x80033f1c/2c). In der Phase: Slew 0xC8/Frame auf den Latch-Slot (@0x80033f90-94), **kein manueller Aim-Turn** (sub5 hat keinen), !R1 → LOWER (@0x80033fbc), Ausrichtung `((bearing−rot_y+100)&0xfff) < 200` (a9cc, tol 0x64 @0x80033fb0) → READY (@0x80033fe4). Gleiche-Frame-Präzedenz Fire/Reload > L1 (Fall-through @0x800332fc) nachbilden.
4. **RAISE-Angleich (optional, gleicher Block):** Original latcht EINMAL im Cold-Init (703c @0x80032fa8) und slewt warm nur bei `acaf3&1` (= 703c-Return==1, Live-Bucket; @0x80032fbc/0x80032fd0-d8) — der Port re-scannt derzeit jeden Frame. Beim Umbau auf den Latch mitziehen.
5. **Verifikation (Sonde):** Unit-ctest nach Muster `tests/unit/test_aim_all_around.c`: Zombies in Slot 2 (FERN, 25000) und Slot 3 (NAH, 2000). (a) Latch=3 (nearest), `aim_cycle()` → **erwartet 2** (zyklisch: 3→4…→Wrap→2; der alte Nearest-Scan gäbe 3). (b) nochmal → 3. (c) Slot2 hp=−1, Latch=3 → Selbst-Reselect **3**. (d) beide hp<0 → **0**, Latch unverändert. (e) `grid_id=0x20` schließt aus, `grid_id=0x80` NICHT (K2 wählbar!). Danach Live-Gegenprobe per `re15-parity-verify` (ROOM1140, R1-Aim, L1-Edge, `rot_y`-Konvergenz PSX vs. Port).

## 4) OFFEN (ehrlich)

- **`DAT_800ac762`-Bit 0x4 = L1**: gestützt auf das im Port etablierte byte-swapped Raw-Layout (menu_common.c:38/1386: CROSS=0x40, START=0x800 ⇒ Standard-Swap L2=1/R2=2/L1=4/R1=8), nicht auf einen frischen Pad-Treiber-Disasm. Nächster RE-Schritt bei Zweifel: Schreiber `sh a0→DAT_800ac762` @0x800305a0 rückverfolgen.
- **`DAT_800acaf3`-Overload**: RAISE speichert den 703c-Return-CODE (0/1/2), sub5 den 37250-Slot-INDEX — Rückleser in der Gun-Maschine ist nur das `&1`-Gate @0x80032fd0-d8 (nur RAISE-warm, wird vor jedem RAISE-Cold neu geschrieben → keine Wechselwirkung gefunden). Die Spezial-Maschine (switch @0x80036480) nutzt acaf3 als Ramp-Zähler (@0x8003655c-80) — nicht weiter verfolgt.
- **`DAT_800acbfc` wird vom Grab/Devour-System mitbeschrieben** (Dog `@0x80103c58`, Devour `@0x801191d8`, enemy_ai_common.c:3213/7674): nach einem Grab startet der nächste L1-Zyklus vom Grabber-Slot. Der Plan latcht zunächst nur die Aim-Schreiber; volle Shared-Global-Parität (Devour setzt den Latch mit) = Folgeentscheidung.
- Kein vollständiger `DAT_800acbfc`-Leser-Census außerhalb Gun-FSM/Grab (z.B. Reticle/Render) — im Gun-FSM lesen nur a8f8/a9cc-Aufrufe.
- Spezial-Waffen-Maschinen (Root LAB_80034014/Tabelle 0x80074150; per-Waffe-Dispatch @0x800345e4) nutzen 37250 nachweislich NICHT (einziger Callsite), ihr eigenes L1-Verhalten wurde aber nicht auditiert; dito das seltsame `acaf3&1`-Gate @0x80034124 dort.
- Wrap 20→15 (RE15_ACTOR_MAX=16) ist eine dokumentierte Port-Anpassung, kein Byte-Beleg-Ersatz.
