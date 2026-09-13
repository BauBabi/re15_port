# BEFUND finisher-timing — "ich werde schon gefressen VOR dem YOU ARE DEAD Bildschirm"

Nutzer 2026-09-13 zum Alligator-Fress-Finisher: *"Das sieht schon stark aus, aber ich
werde schon gefressen VOR dem YOU ARE DEAD Bildschirm. Das ist aber der Finisher des
Bildschirms. Ich muss also DORT gefressen werden. Nicht vorher."*

Analyse 2026-09-13, Runde 8. NUR statische Analyse (Read/Grep/eigene MIPS-Disasm).
Quellen: `info/Re1.5/PSX.EXE` (t_addr 0x80010000, Datei-Off = 0x800+(addr-0x80010000)),
`info/re2leon/PSX.EXE` (dito), `re15_port/shared_assets/RE2/EM23_OVL_0000.BIN`
(= CDEMD0.EMS kind 0x23 rec 1, gelinkt @0x80100000, Datei-Off = addr-0x80100000),
`RE2_Quellcode_V2/`, Port `re15_port/`. Werkzeug:
`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py` +
eigene EA-Scanner (Konstanten-Propagation lui/addiu/ori) im Scratchpad.
Vorbefunde: Runde 6 `gator-vollausbau.md` §6.1, Runde 7 `victim-render-gate.md`.

---

## 0. KURZFAZIT

**Die Lesart-Hypothese ist falsch herum — und der Nutzer hat recht.**
Im Original ist die Fress-Sequenz nicht etwas, das *vor* der Todes-Praesentation
laeuft, sondern etwas, das **mittendrin** laeuft: beide Engines starten die
Game-Over-Praesentation **waehrend** der Gegner den Spieler noch frisst.

- **RE1.5** (die Engine des Ports): die Game-Over-FSM `FUN_8001500c` hat ein einziges
  Gate — das Spieler-Kommando `DAT_800aca58` in {3, 6, 7} (@0x80015014-0x80015030).
  **Kommando 6 IST der Fress-Zustand** (gegner-eigener Handler, Typ-Tabelle 0x800AC858,
  @0x8003690c-2c). Die FSM laeuft also per Definition waehrend des Fressens, und ihr
  sub0 kopiert sogar **Spieler UND Greifer** in einen Snapshot (@0x800150d0-0x8001510c)
  — die Praesentation ist um ein LAUFENDES Fressen herum gebaut.
- **RE2** (die Engine des Gators): der Zonen-Fress-Finisher setzt den **Todes-Latch
  `0x800CFB74 |= 0x04000000` erst bei Gator-Clip-4-Frame 13** (@0x80101104-0x80101134)
  — also genau dann, wenn das Maul zuschnappt und das 120-Frame-Schuetteln beginnt.
  Der Flow-Manager @0x800266c8-0x80026734 sieht diesen Latch und startet das
  **DIE-Demo-Overlay** (`FUN_80031e80(1,2)` @0x8002671c). Das Schuetteln laeuft damit
  **unter** dem YOU-DIED.
- **Der Port** macht genau das Gegenteil: seit Runde 7 (`re15_damage.c`-is_dead-Ausnahme
  fuer Victim-Modus 4) ist die Game-Over-FSM waehrend des **gesamten** Finishers
  abgeschaltet und startet erst, wenn `re15_player_victim_force_end()` gerufen wird
  (`enemy_ai_boss_gator.c:1929`) — ~255 Ticks nach dem toedlichen Biss. Erst danach
  laufen die 77 Frames bis Blackbg/YOU-DIED. Ergebnis: **erst gefressen, dann der
  Bildschirm** = exakt der Nutzer-Report.

Fix: die **zwei Gates trennen** (im Original sind es zwei verschiedene) — die
Spieler-FSM bleibt fuer Modus 4 gesperrt (Runde-7-Fix A/B bleibt), die Game-Over-FSM
bekommt ihr eigenes Gate und startet ab dem f13-Latch. Patch-Plan §4.

---

## 1. RE2-ORIGINAL — wann ist der Spieler tot, wann startet YOU DIED (Frage 1)

### 1.1 Der Fress-Einstieg setzt HP=-1 SOFORT (P0, Frame 0)

EM23_OVL_0000.BIN, ACTIVE sub4 P0, selbst disassembliert:

```
80100fc4: lw    v0,0(a0)             ; a0 = 0x800CFBD8  (Spielzustand-Bits)
80100fcc: ori   v0,v0,0x100
80100fd0: sw    v0,0(a0)             ; 0x800CFBD8 |= 0x100
80100fe0: jal   0x80077360           ; Flag16-Test -> Cut 14 : Cut 7
80100ff4: jal   0x8002c7ac           ; KAMERA-CUT setzen
80100ffc: addiu v0,zero,1
80101004: sb    v0,-997(at)          ; 0x800DFC1B = 1      <- "Cut haengt noch"
80101024: addiu v0,zero,5
8010102c: sb    v0,-1028(at)         ; 0x800CFBFC = 5      <- SPIELER-ROUTINE 5
80101038: sb    zero,-1027(at)       ; 0x800CFBFD = 0      <- Sub-Routine 0 (Variante [0])
8010103c: addiu v0,zero,-1
80101040: sh    v0,-690(at)          ; 0x800CFD4E = -1     <- SPIELER-HP = -1
801010c4: ori   v1,v1,0x80
801010d4: sb    v1,-565(at)          ; 0x800CFDCB |= 0x80  <- Status "gepackt"
```

HP ist also ab Frame 0 negativ — **aber das loest in RE2 gar nichts aus**: RE2 pollt
HP<0 nirgends global. Eigener EA-Scan ueber die ganze RE2-EXE nach Zugriffen auf
0x800CFD4E liefert exakt 6 Stellen (0x800401f4, 0x800402f8, 0x80040310, 0x80040340,
0x800591b8, 0x8006953c) — die vier 0x8004xxxx sind der Schadens-Arbiter `FUN_800401d4`,
0x800591b8 ist ein SCD-Opcode, 0x8006953c der Fine/Caution/Danger-Klassifizierer.
**Kein Game-Over-Watcher auf HP.**

### 1.2 Der echte Todes-Latch: 0x800CFB74 Bit 0x04000000

Generischer Tod (Gegner-Treffer), Schadens-Applikator:

```
800418d0: lhu   v0,342(t0)           ; t0 = Spieler, +0x156 = HP
800418d8: subu  v0,v0,v1             ; HP -= Schaden
800418e0: sh    v0,342(t0)
800418e8: addiu v0,zero,2
800418ec: sw    v0,4(t0)             ; Routine-Wort = 2 (Treffer)
800418f0: bgez  a0,0x80041900        ; HP >= 0 -> fertig
800418f8: addiu v0,zero,3
800418fc: sw    v0,4(t0)             ; HP < 0  -> Routine-Wort = 3  (TOD)
```
(= 1:1 der RE1.5-Zwilling @0x80012ee8-ef8, `sb 3,4(s1)`.)

Und der Sofort-Tod-Zweig desselben Arbiters:
```
800402c0: lbu   v1,467(a2)           ; a2 = 0x800CFBF8, +0x1D3 = Status
800402cc: ori   v1,v1,0x80
800402d0: sb    v1,467(a2)           ; Status |= 0x80
800402d4: lw    v1,0(a1)             ; a1 = 0x800CFB74
800402d8: lui   a0,0x400             ; 0x04000000
800402dc: or    v1,v1,a0
800402e4: sw    v1,0(a1)             ; 0x800CFB74 |= 0x04000000   <- TODES-LATCH
```

### 1.3 Der Ausloeser der Praesentation (Flow-Manager)

Der grosse Spielfluss-Automat (0x80025xxx-0x80026xxx, ein Rumpf, alle Fehlzweige nach
0x80026738) enthaelt genau die gesuchte Kette:

```
800266c0: lui   s0,0x800d
800266c4: addiu s0,s0,-1164          ; s0 = 0x800CFB74
800266c8: lw    v0,0(s0)
800266cc: lui   v1,0x400             ; 0x04000000
800266d0: and   v0,v0,v1
800266d4: beq   v0,zero,0x80026738   ; Latch nicht gesetzt -> nichts
800266dc: jal   0x80032138           ; (a0=1) Slot-Belegt-Test 0x800D7724
800266e4: bne   v0,zero,0x80026738   ; belegt -> nichts
800266ec: lbu   v0,-997(v0)          ; 0x800DFC1B
800266f8: bne   v0,zero,0x80026738   ; CUT HAENGT NOCH -> nichts     <<<<
80026704: lh    v1,18464(v1)         ; 0x800D4820 = gewuenschter Cut
8002670c: lbu   v0,-1038(v0)         ; 0x800CFBF2 = aktiver Cut
80026714: bne   v1,v0,0x80026738     ; Cut noch nicht angekommen -> nichts
8002671c: jal   0x80031e80           ; <<<< PRAESENTATION STARTEN
80026720: addiu a1,zero,2            ;      (a0=1, a1=2)
80026724: lui   v1,0xfbff
80026730: and   v0,v0,v1
80026734: sw    v0,0(s0)             ; 0x800CFB74 &= ~0x04000000 (Latch quittiert)
```

`FUN_80031e80(1,2)` ist ein **Overlay-Ladeauftrag**: es indiziert drei Tabellen mit
Schrittweite 12 (@0x800A20B8 / @0x800A20BC / @0x800A20C0, `s0 = a1*12`,
@0x80031e90-98) und ruft `FUN_80012fb8(id, *desc, 0, 0x80010AD0)` + `FUN_80031f6c`.
Fuer a1=2: id = `[0x800A20D0]` = 1, Einstieg = `[0x800A20D4]` = **0x8019009C**,
Deskriptor = `[0x800A20D8]` = 0x800107EC -> Ladeadresse **0x80190000**.
Das ist der DIE-Demo-Slot (`info/re2leon/COMMON/BIN/DIEDEMO.BIN`, 14536 B); die
Nachbareintraege zeigen auf 0x801BFA84/0x801BFAA8 = die bekannte OPENING/ENDING-Region
(vgl. Memory `reai-v2-preintro-re2`: OPENING.BIN @0x801bfa18 — derselbe Deskriptorwert
steht in `[0x800107F4..0x80010800]`).
Es gibt nur zwei Aufrufer mit a1=2: @0x80025884 und @0x8002671c — beide im selben
Flow-Manager-Rumpf.

**Zwischenfazit:** in RE2 startet die Praesentation, sobald `0x800CFB74` Bit 0x04000000
gesetzt ist und der Kamera-Cut steht. Nicht ueber HP, nicht ueber das Routine-Wort.

### 1.4 WANN setzt der Fress-Finisher diesen Latch? — bei Clip-4-Frame 13

```
801010ec: addiu a1,zero,4
801010f4: jal   0x8001a330           ; Gator-Clip 4 (Schnapp, 45 F)
801010fc: lbu   v1,333(s0)           ; +0x14D = aktueller Frame
80101100: addiu v0,zero,13
80101104: bne   v1,v0,0x801011e8     ; Frame != 13 -> nichts
80101108: lui   t0,0x400             ; (Delay-Slot) t0 = 0x04000000
8010110c: ori   t0,t0,0x100          ; t0 = 0x04000100
80101110: lui   v1,0x800d
80101114: addiu v1,v1,-1164          ; v1 = 0x800CFB74
80101118: addiu a0,v1,132            ; a0 = 0x800CFBF8 (Spieler)
80101124: lw    v0,0(v1)
8010112c: or    v0,v0,t0
80101130: jal   0x80015b94           ; gemeinsames ANKER-Paar setzen
80101134: sw    v0,0(v1)             ; 0x800CFB74 |= 0x04000100   <<<< TODES-LATCH
80101148: addiu v0,zero,1
80101150: sh    v0,-688(at)          ; 0x800CFD50 = 1 (Opfer-Sync an die Opfer-Maschine)
80101154: lhu   v0,118(s0)
8010115c: sh    v0,-914(at)          ; Spieler-Yaw = Gator-Yaw
```

**Derselbe Frame, derselbe Befehlsblock**, in dem Leon auf Opfer-Paar-3-Clip-1
(das 120-Frame-Rumschleudern) umgeschaltet wird, setzt auch den Todes-Latch.
Praesentation und Schuetteln starten also **gemeinsam**.
Gegenprobe Proximity-Kill: dessen Leon-Handler-P0 setzt `0x800CFB74 |= 0x04000000`
ebenfalls direkt beim Packen (@0x80102FC0-CC, Runde 6 §6.2); der Fang-Einstieg schreibt
das Routine-Wort als 32-Bit-Wort `0x105` (Routine0=5/Routine1=1) @0x8010176c-74 plus
HP=-1 @0x80101780-88 und Status |=0x80 @0x80101794-a8.

### 1.5 Ist der Ausloeser waehrend der Sequenz GEGATET? — nur um den Kamera-Cut

Das einzige Gate, das der Gator selbst bedient, ist `0x800DFC1B` (@0x80101004 = 1).
Eigener EA-Scan: im Overlay wird 0x800DFC1B **nur auf 1** geschrieben (@0x80101004 P0 +
sechs Stellen in den Gator-Todes-Routinen 0x80102278/2510/2794/28b4/2a8c/2b14); in der
EXE gesetzt bei "Cut gewuenscht != Cut aktiv" (@0x8002b9f0-ba0c) und **genullt**, sobald
die Kamera-Funktion durch ist (@0x8002bd7c `sb zero,315(s1)`, s1+315 = 0x800DFC1B, wenn
`0x800CFB74 & 0x200 == 0`). Es ist also ein **1-2-Frame-Halt fuer den Cut-Wechsel**,
kein Riegel ueber die ganze Sequenz.

Ebensowenig gaten die Routine-Worte: Routine 5 (@0x8004006c) und 6 (@0x800400d0) setzen
`0x800CFBD8 |= 0x40`; die einzigen gefundenen Leser dieses Bits sind das Inventar-/
AOT-Gate (@0x80026414 `andi 0x40`, @0x80052084 `andi 0x8040`), die nur Menue und
Trigger sperren.

### 1.6 Neben-Befund: Runde-6-OPEN ist GELOEST (0x800CE38C)

Runde 6 §6.1 vermerkte: *"der EXE-Aufrufer, der 0x800CE38C laedt, ist statisch nicht
auffindbar"*. Er ist gefunden — es ist der **Spieler-Routine-5-Handler**:

```
; Spieler-Dispatcher (Ende von FUN_8003bfac)
8003c188: lbu   v0,4(s0)             ; s0 = Spieler, +4 = Routine0
8003c190: sll   v0,v0,2
8003c19c: lw    v0,16432(at)         ; Tabelle @0x800A4030
8003c1a4: jalr  v0                   ; a0=Spieler, a1=+0x108(EMR1), a2=+0x17C(EDD1)
; @0x800A4030: [0]=0x8003c210 [1]=0x8003c57c [2]=0x8003f600 [3]=0x8003fee4
;              [4]=0x80065c88 [5]=0x8004006c [6]=0x800400d0 [7]=0x80040134

; Routine 5 @0x8004006c
80040074: addiu v1,v1,-1064          ; v1 = 0x800CFBD8
80040084: ori   v0,v0,0x40
80040088: sw    v0,0(v1)             ; 0x800CFBD8 |= 0x40
80040090: lw    a2,436(a0)           ; a2 = Spieler+0x1B4 = GREIFER-Zeiger
8004009c: lbu   v0,8(a2)             ; Greifer+8 = TYP
800400a4: lw    a1,392(a0) / a2,396(a0)   ; Opfer-Paar +0x188/+0x18C
800400ac: addu  v1,v1,v0*4
800400b0: lw    v0,-6360(v1)         ; = 0x800CE300 + Typ*4
800400b8: jalr  v0
```
0x800CFBD8 - 6360 = **0x800CE300**; fuer Typ 0x23 (35) -> 0x800CE300 + 0x8C =
**0x800CE38C** — genau der Slot, in den der Gator beim Init seinen Dispatcher schreibt:
```
801004f4: lui   v0,0x8010
801004f8: addiu v0,v0,11528          ; v0 = 0x80102D08
80100500: sw    v0,-7284(at)         ; 0x800CE38C = 0x80102D08
```
Routine 6 (@0x800400d0) ist derselbe Code mit Tabelle 0x800CFBD8-6104 = **0x800CE400**.

Damit ist die Architektur-Gleichung exakt:

| | RE1.5 | RE2 |
|---|---|---|
| Spieler-Kommando-Tabelle | @0x80073F90 | @0x800A4030 |
| generischer Tod | cmd 3 -> 0x800366bc | Routine 3 -> 0x8003fee4 (-> Routine 7, `addiu v0,7` @0x8003ff90 / `sw v0,4(s1)` @0x8004002c) |
| gepackt (lebend) | cmd 5 -> 0x80036834, Typ-Tabelle **0x800AC758** | Routine 5 -> 0x8004006c, Typ-Tabelle **0x800CE300** |
| **gefressen / gegner-eigene Todes-Praesentation** | **cmd 6 -> 0x800368c0, Typ-Tabelle 0x800AC858** | **Routine 6 -> 0x800400d0, Typ-Tabelle 0x800CE400** |
| Tod mit Fade-Timer | cmd 7 -> 0x8003694c (Timer 0x78 @0x8003697c-88) | Routine 7 -> 0x80040134 (Timer 60 @0x80040180-84) |

Der Gator nutzt **Routine 5** (nicht 6), weil er im selben Slot auch die drei
ueberlebbaren Chomp-Varianten faehrt (Routine1 waehlt [0]..[3], Runde 6 §6.2/6.3).
Funktional ist Variante [0] das RE1.5-cmd-6.

### 1.7 Korrektur zu Runde 6: `0x800CFBF8 |= 0x80000` ist NO-DRAW, kein Game-Over

Runde 6 §6.1 notiert am Ende von P2 ("Clip 5 fertig") `0x800CFBF8 |= 0x80000`
(@0x80101190-ac, `lui v1,0x8` im Delay-Slot @0x80101194). Bedeutung: **Bit 0x80000 des
Wort-0 ist das Nicht-Zeichnen-Bit**, geprueft in allen drei Zeichen-Pfaden:
`FUN_80019b3c:23` und `FUN_80019cd0:96` (`(*param_1 & 0x80000) == 0`) sowie in der
Objekt-Zeichenschleife `FUN_800363f0:21` (`(*puVar4 & 1) != 0 && (*puVar4 & 0x80000) == 0`).
Nach dem 120-Frame-Schuetteln wird Leon also **unsichtbar (verschluckt)** — das ist das
RE2-Original des alten Port-`no_draw`. Es ist **kein** Praesentations-Trigger.

---

## 2. RE1.5 — die Engine, der der Port folgen MUSS (der eigentliche Massstab)

### 2.1 Das Gate der Game-Over-FSM: cmd in {3, 6, 7}

Die FSM ist ein **eigener Top-Level-Aufruf** im Haupt-Loop (@0x8001cdfc, VOR dem
Spieler-Dispatcher @0x8001ce0c — im Port bereits so verdrahtet,
`game_step_common.c:1020-1042`). Ihr Rumpf:

```
8001500c: addiu sp,sp,-24
80015010: lui   v1,0x800b
80015014: lbu   v1,-13736(v1)        ; v1 = DAT_800ACA58 = Spieler-KOMMANDO
80015018: ori   v0,zero,0x6
8001501c: beq   v1,v0,0x80015038     ; cmd == 6 (GEFRESSEN) -> LAEUFT
80015024: ori   v0,zero,0x3
80015028: beq   v1,v0,0x80015038     ; cmd == 3 (TOD)       -> LAEUFT
8001502c: ori   v0,zero,0x7
80015030: bne   v1,v0,0x80015064     ; cmd != 7             -> sofort `jr ra`
80015038: lui   v0,0x800b
8001503c: lhu   v0,21034(v0)         ; DAT_800B522A = Sub-Zustand
8001504c: addiu at,at,7440           ; Tabelle @0x80071D10 (7 Sub-Handler)
8001505c: jalr  v0
```
Tabelle @0x80071D10 = {0x80015074, 0x800151a0, 0x80015284, 0x80015528, 0x800155c4,
0x8001569c, 0x80015798}.

Und die Kommando-Tabelle @0x80073F90 belegt, dass cmd 6 der **Fress**-Handler ist:
```
800368c0: addiu sp,sp,-24            ; cmd-6-Handler (@0x80073FA8)
800368c8: addiu a0,a0,-13734         ; a0 = 0x800ACA5A (Substate)
80036914: lw    v0,-13316(v0)        ; v0 = [0x800ACBFC] = GREIFER-Entity
8003691c: lbu   v0,8(v0)             ; Greifer+8 = TYP
80036924: sll   v0,v0,2
80036928: addu  v0,a0,v0
8003692c: lw    v0,-514(v0)          ; = 0x800AC858 + Typ*4  (cmd-5-Zwilling: -770 = 0x800AC758)
80036934: jalr  v0
```

**Damit ist die Frage beantwortet: RE1.5 faehrt die Todes-Praesentation ausdruecklich
AUCH im Fress-Zustand (cmd 6).**

### 2.2 sub0 sichert Spieler UND GREIFER — die Praesentation ist um das Fressen gebaut

```
80015074: addiu sp,sp,-24
80015078: ori   a0,zero,0x3c
80015080: jal   0x800449f4           ; BGM-Decrescendo (0x3c)
8001508c: lh    v0,21036(v0)         ; DAT_800B522C = Frame-Zaehler
80015094: bne   v0,zero,0x80015124   ; nur beim ERSTEN Tick:
800150ac: lbu   v1,0(a1)             ; a1 = 0x800ACA58 (cmd)
800150b4: bne   v1,v0(=3),0x800150d0
800150c4: sh    -32768,21038(at)     ; cmd==3 -> DAT_800B522E = -32768 ("kein Greifer")
800150d0: addiu a1,a1,-4             ; a1 = 0x800ACA54 = SPIELER
800150d4: jal   0x8004ee38           ; memcpy(0x80190000, Spieler, 0x1D8)
800150d8: ori   a2,zero,0x1d8
800150e8: lw    a1,-13316(a1)        ; a1 = [0x800ACBFC] = GREIFER-Entity
800150ec: jal   0x8004ee38           ; memcpy(0x801901D8, Greifer, 0x1F4)
800150f0: ori   a2,zero,0x1f4
80015108: jal   0x8004ee38           ; memcpy(0x801903CC, [0x800ACBDC]+1376, 0xAC)
80015130: ori   v1,zero,0x32         ; Zaehler-Schwelle 50
```
Die FSM legt beim Start ausdruecklich ein **Standbild des Paares Opfer+Greifer** an.
(Randbeobachtung, nicht als Beweis gewertet: das Ziel 0x80190000 ist in RE2 genau die
Ladeadresse des DIE-Demo-Overlays, §1.3 — RE1.5 hat inline, was RE2 nachlaedt.)

### 2.3 Zeitachse der RE1.5-Praesentation (alle Konstanten belegt)

| Sub | Adresse | Dauer | was passiert |
|---|---|---|---|
| 0 | 0x80015074 | **50 F** (`ori v1,zero,0x32` @0x80015130) | BGM-Decrescendo (`ori a0,0x3c` @0x80015078), Snapshot, dann Rate 0x4bd (@0x8001514c) |
| 1 | 0x800151a0 | **27 F** (`slti v0,v0,27` @0x800151cc) | Rate -0x2556 (@0x800151d8), **`FUN_80021634(2,0)` = BLACKBG** (@0x80015200-08), 0x800ACA38 \|= 0x100000\|0x4000000\|0x4000, 0x800ACA3C \|= 0x40 &= ~0x1E (@0x8001520c-64) |
| 2 | 0x80015284 | 1 F | **YOU DIED + Todes-Kamera** aus [0x800ACBDC] (@0x800152a4-f0) |
| 3/4 | 0x80015528 / 0x800155c4 | 3 x (19+3) F | Herzschlag-Pulse |
| 5 | 0x8001569c | 16 F | Schluss-Schwelle |
| 6 | 0x80015798 | Fade ab ctr 0x50, Ende 0x6d | Fade-to-Black -> Title |

Port-Gegenstueck `re15_gameover_fsm_tick` (`game_step_common.c:557-622`) traegt genau
diese Zahlen (0x32/0x4bd/0x1b/-0x2556/0x13/0x2aaa/0x7ff/0x50/0x6d) — **byte-true, am
FSM-Inhalt ist nichts zu aendern.**

=> **Blackbg bei Frame 77, YOU DIED bei Frame 78** nach dem Latch.
Wichtig: `g_death_blackbg` heisst "Raum-Kulisse aus, 3D bleibt stehen"
(`game_step_common.c:56`) — die gefressene Leon-Pose im Maul **bleibt sichtbar**, und
die Todes-Kamera (`main.c:4794-4830`) fuehrt per Frame auf den Spieler nach
(`/60`-Ease auf XZ, `/20` auf Y). Genau das meint der Nutzer mit "ich muss DORT
gefressen werden".

---

## 3. PORT — was heute passiert (Frage 2) und was fehlt (Frage 3)

### 3.1 Die Zeitachse ab dem toedlichen Biss (Tick-genau, aus dem Code gelesen)

| Tick | Ort | Zustand |
|---|---|---|
| N (im `run_all`) | `enemy_ai_boss_gator.c:349-360` `gb_biss_abschluss` | `re15_player_take_damage(..,GB_BITE_TYPE=5,..)` -> hp<0; `g->phase=GBP_FRESSEN; g->timer=0`; **`re15_player_victim_force(e->type,0,0)` -> Victim-Modus 4** (`enemy_ai_common.c:806-813`); `e->motion=4` |
| N+1 | `game_step_common.c:1013` | `pl->hp<0 && s_prev_hp>=0` — aber `re15_player_is_grabbed()` ist schon 1 (`enemy_ai_common.c:578`, `vs == 4`) -> `re15_player_death_cmd3()` wird **nie** geladen; die hp-Flanke ist mit `s_prev_hp` (:1014) verbraucht |
| N+1 | `game_step_common.c:1041` | `re15_player_is_dead()` = **0** (`re15_damage.c:284-289`, Modus-4-Ausnahme) -> **`re15_gameover_fsm_tick()` laeuft NICHT** |
| N+1 | boss_gator.c:1847-1866 | P0: Teleport (`e->x + RotY*(10643,-915)`), Yaw-Kopie, `e->anim_frame=0` |
| N+1 .. N+14 | boss_gator.c:1868-1891 | P1: `e->anim_frame++`; bei ==13 -> Anker + `re15_player_victim_force(type,1,0)` + `g->gefressen=1` + **`g->timer=100`** |
| N+15 .. N+134 | boss_gator.c:1892-1921 | P2: 120 F Schuetteln (Gator Clip 5 + Leon Opfer-Clip 1) |
| N+135 .. N+254 | boss_gator.c:1922-1930 | P3: 120 F Kau-Loop, Leon auf Opfer-Frame 119 geparkt |
| N+255 | boss_gator.c:1928-1933 | **`re15_player_victim_force_end()`** -> Modus 0, `phase=GBP_CHASE` |
| N+256 | game_step_common.c:1041 | jetzt erst `re15_player_is_dead()` = 1 -> **FSM startet bei sub 0** |
| N+333 | FSM sub1-Ende | Blackbg |
| N+334 | FSM sub2 | **YOU DIED** |

Rund **8,5 s Fressen, dann erst der Bildschirm** — genau der Report.

### 3.2 Der zweite Riegel, der heute nichts mehr bewirkt

`game_step_common.c:606-612` haelt in sub 6 den Schluss-Fade an, solange
`re15_gator_fressen_hold()` (`enemy_ai_boss_gator.c:2051-2061`, Bedingung `timer < 215`)
1 liefert. Das war der Runde-6/7-Ersatz fuer den fehlenden Parallellauf — er greift
heute gar nicht mehr, weil die FSM erst startet, wenn der Finisher schon vorbei ist
(timer 255+ > 215). Toter Code mit falscher Semantik.

### 3.3 Auch die RENDER-Seite haengt am falschen Praedikat

`platform/pc/main.c:6205`: `if (re15_player_is_dead() || g_gameover_active) { ... }` —
das ist der Block, der Fade/White/Blackbg/YOUDIED.TIM ueberhaupt an den Renderer gibt.
Mit Modus 4 ist `is_dead()` = 0, also wuerde selbst eine laufende FSM **nichts
zeichnen**. Beide Gates muessen zusammen umgestellt werden, sonst tickt die FSM
unsichtbar.

### 3.4 Warum der Runde-7-Fix trotzdem richtig war — und wo seine Grenze liegt

`re15_damage.c:266-299` benutzt `re15_player_is_dead()` fuer **zwei verschiedene
Original-Gates gleichzeitig**:

- den **Spieler-Zweig** der else-if-Kette (`game_step_common.c:1105`) und den cmd-3-Arm
  (:1013) -> im Original: "laeuft der cmd-3/7-Handler?". Fuer cmd 6 ist die Antwort
  **NEIN** -> Runde-7-Fix A/B (Modus 4 -> `is_dead()`=0, `is_grabbed()`=1) ist
  **korrekt und muss bleiben**.
- die **Game-Over-FSM** (:1041) -> im Original `FUN_8001500c` mit dem *eigenen* Gate
  cmd in {3, **6**, 7}. Fuer cmd 6 ist die Antwort **JA**.

Ein Praedikat kann nicht beide Antworten geben. **Das ist der ganze Defekt.** Die
RE2-Seite bestaetigt die Trennung unabhaengig: dort haengt die Praesentation an
`0x800CFB74 & 0x04000000` und nicht am Routine-Wort (§1.3/§1.5).

### 3.5 Die richtige Reihenfolge (Antwort auf Frage 3)

Mit dem f13-Latch (RE2 @0x80101104-34) und der RE1.5-Zeitachse (§2.3):

| Frame nach dem Biss | was |
|---|---|
| 0 .. 13 | P0/P1 im **normalen Raum**, Kamera-Cut, Leon strampelt — noch keine Praesentation |
| **14** | Maul zu, Anker gesetzt, Opfer-Clip 1 startet **und** die Game-Over-FSM startet (sub 0) |
| 14 .. 63 | sub 0: BGM-Decrescendo, Schuetteln sichtbar im Raum |
| **91** | sub-1-Ende: **Blackbg** — Raum-Kulisse weg, Gator + Leon im Maul bleiben stehen |
| **92** | sub 2: **YOU DIED** fliegt ein, Todes-Kamera orbitet das Paar |
| 92 .. 134 | Rest des 120-F-Schuettelns **unter** YOU DIED — der vom Nutzer gewollte Zustand |
| 135 .. 254 | Kau-Loop unter YOU DIED, Herzschlag-Pulse |
| **251** | sub 6: Fade-to-Black setzt ein |
| **280** | `g_gameover_active` -> Title |

---

## 4. PATCH-PLAN (Datei:Zeile, Frage 4)

**FIX 1 — Todes-Latch im Boss exportieren (das RE2-`0x800CFB74 |= 0x04000000`).**
`re15_port/engine/src/enemy_ai_boss_gator.c`, neben `re15_gator_fressen_hold` bei
**:2051**:
```c
/* RE2-Todes-Latch: sub4-P1 setzt 0x800CFB74 |= 0x04000000 bei Gator-Clip-4-FRAME 13
 * (@0x80101104-0x80101134) - derselbe Block, der das Opfer-Paar koppelt. Ab hier
 * laeuft im Original die Todes-Praesentation (RE2: Flow-Manager @0x800266c8-34 ->
 * FUN_80031e80(1,2) @0x8002671c; RE1.5: FUN_8001500c, Gate cmd 6 @0x8001501c). */
int re15_gator_fress_todeslatch(void)
{
    int i;
    for (i = 0; i < RE15_ACTOR_MAX; i++)
        if (re15_gator_boss_active(&g_actors[i])
            && s_gb[i].phase == GBP_FRESSEN && s_gb[i].gefressen)
            return 1;
    return 0;
}
```
`g->gefressen` wird **genau** an der f13-Stelle gesetzt (`:1878` mit Bank, `:1886` ohne)
— kein neues Feld noetig, kein geratener Frame. Deklaration in
`re15_port/include/re15_boss_gator.h` neben **:30**.

**FIX 2 — die beiden Gates trennen (Kern-Hebel).**
`re15_port/engine/src/game_step_common.c`, neues Praedikat vor
`re15_gameover_fsm_tick` (z. B. bei **:555**):
```c
/* Das Gate der Game-Over-FSM ist im Original NICHT das Gate des Spieler-Zweigs:
 * FUN_8001500c laeuft fuer cmd 3, **6** und 7 (@0x80015014-0x80015030) - cmd 6 ist
 * der GEFRESSEN-Zustand (Handler 0x800368c0, Typ-Tabelle 0x800AC858 @0x8003692c).
 * Der Gator-Finisher ist das Port-Analogon zu cmd 6 (RE2: Spieler-Routine 5,
 * Typ-Tabelle 0x800CE300, Slot 0x800CE38C @0x80100500). */
int re15_death_presentation_active(void)
{
    extern int re15_gator_fress_todeslatch(void);
    return re15_player_is_dead() || re15_gator_fress_todeslatch();
}
```
und **:1041-1042** umstellen:
```c
if (c->rdt_ok && re15_death_presentation_active())
    re15_gameover_fsm_tick();                 /* @0x8001cdfc, vor @0x8001ce0c */
```
`re15_player_is_dead()` bleibt fuer Modus 4 auf 0 (`re15_damage.c:284-289` **nicht
anfassen**) — der Spieler-Zweig :1105 und der cmd-3-Arm :1013 bleiben damit gesperrt,
exakt wie cmd 6 im Original weder cmd 3 noch cmd 7 faehrt.

**FIX 3 — Render-Gate mitziehen.**
`re15_port/platform/pc/main.c:6205`:
`if (re15_player_is_dead() || g_gameover_active)` ->
`if (re15_death_presentation_active() || g_gameover_active)`.
Deklaration zu den anderen `g_death_*`-Exporten in `re15_port/include/re15_room.h`
neben **:161**. Ohne diesen Schritt tickt die FSM unsichtbar.

**FIX 4 — den Ersatz-Riegel entfernen.**
`re15_port/engine/src/game_step_common.c:606-612`: den ganzen Block
(`extern int re15_gator_fressen_hold(void); if (s_go_ctr < 0x50 && ...) break;`)
loeschen. RE1.5-sub6 @0x80015798 hat kein solches Gate; mit FIX 2 laeuft die
Praesentation ohnehin parallel, und der Hold wuerde den Fade jetzt falsch verzoegern.
`re15_gator_fressen_hold()` (boss_gator.c:2051, Header :30) wird damit unbenutzt —
entweder loeschen oder als reine Telemetrie kennzeichnen.

**FIX 5 (klein, aus Runde 7 offen) — Greifer-Slot mitsetzen.**
`re15_port/engine/src/enemy_ai_common.c:806-813` (`re15_player_victim_force`) setzt
`g_player_victim_zombie` nicht. Im Original zeigt Spieler+0x1B4 auf den Greifer
(RE2 @0x80101068 `sw s0,-596(at)` = 0x800CFDAC; RE1.5 liest ihn ueber 0x800ACBFC), und
**die Todes-FSM braucht ihn**: RE1.5-sub0 kopiert den Greifer per `[0x800ACBFC]`
(@0x800150e8), sub2 baut die Todes-Kamera aus `[0x800ACBDC]` (@0x800152a4-f0). Mit
FIX 2 wird dieser Pfad zum ersten Mal waehrend des Finishers betreten — also
`re15_player_victim_force` um den Greifer-Slot erweitern und die Aufrufer
(boss_gator.c:359 und :1877) den Gator-Slot mitgeben lassen.

**Reihenfolge/Abhaengigkeit:** FIX 1 -> FIX 2 -> FIX 3 sind EIN Paket (ohne 3 sieht man
nichts, ohne 1 fehlt der frame-genaue Start). FIX 4 danach. FIX 5 unabhaengig.

**Abnahme am Artefakt (nicht an der Absicht):** in ROOM2090 sterben und im Capture
pruefen: (a) Blackbg **waehrend** das Schuetteln laeuft (Gator-Clip 5 noch nicht durch),
(b) YOU DIED ueber dem Maul-Bild, Leon sichtbar im Maul (der Victim-Render
`main.c:6931` greift weiter, weil Modus 4 bleibt), (c) der Fade erst nach dem Kau-Loop.
Zaehlprobe: Blackbg muss **77 Frames nach dem f13-Schnapp** stehen — nicht 77 Frames
nach dem Sequenzende.

---

## 5. OFFENE PUNKTE (ehrlich markiert)

- **OFFEN:** Der Inhalt von `DIEDEMO.BIN` (RE2, Einstieg 0x8019009C) ist **nicht**
  disassembliert — ob RE2 dort die 3D-Szene weiterzeichnet (wie RE1.5 mit Blackbg) oder
  ein Vollbild legt, ist statisch nicht belegt. Fuer den Port ohne Folgen: der Port ist
  ein **RE1.5**-Port, und RE1.5 macht die Praesentation inline mit stehenbleibendem 3D
  (§2.3) — genau das, was der Nutzer verlangt.
- **OFFEN:** Der zweite `FUN_80031e80(1,2)`-Aufrufer @0x80025884 (Zweig mit Zaehler
  `s6 == 16`) ist nicht auserzaehlt; er liegt im selben Flow-Manager-Rumpf und aendert
  nichts an §1.3.
- **KORREKTUR zu Runde 6:** `0x800CFBF8 |= 0x80000` am P2-Ende ist NO-DRAW (§1.7),
  nicht der Praesentations-Start — die Runde-6-Notiz sollte nachgezogen werden.
- **NICHT gemessen:** diese Runde ist rein statisch (Auftrag). Ein echter
  Gator-Kill-Capture zur Verifikation der Frame-Zahlen aus §3.5 steht weiter aus (schon
  in Runde 7 §5 als OFFEN vermerkt).
