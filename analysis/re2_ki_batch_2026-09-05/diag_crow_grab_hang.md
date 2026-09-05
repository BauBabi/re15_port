# Diagnose: RE2-Kraehe "keine Animation, haengt an mir, folgt mir" (GRAB-Pfad Sub 13→14)

Symptom (Nutzer, RE2-Modus): *"Einmal hatte die Kraehe keine Animation mehr und hing einfach
nur noch an mir und folgte mir"* — Kraehe klebt am Spieler, Spieler kann weiterlaufen.

Alle Original-Zitate unten stammen aus EIGENEM Disasm dieser Sitzung
(`python .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis/table … --bin EMOVL21_S0.BIN`,
Overlay laedt @0x80100000). Port-Zitate: `re15_port/engine/src/enemy_ai_re2_crow.c` (= "crow.c"),
`enemy_ai_common.c` (= "common.c"), `enemy_ai_re2_dog.c`, `player_common.c`,
`game_step_common.c`, `platform/pc/main.c`.

---

## 0. Kurzfassung (Verdict)

Der Kraehen-seitige Sub-14-GRAB ist gegen das Original byte-genau (eigenes Disasm unten,
alle Phasen abgeglichen). **Der Defekt liegt im Spieler-Halte-Geschirr des Ports**: das
Original haelt den Spieler ueber einen PERSISTENTEN Command-State (PL→(5,0), Anker
PL+0x1B4/+0x188/+0x18C), der Port leitet den Halt jeden Frame NEU her
(`s_player_grabbed`-Clear in run_all + Pin-Bedingung in exec14 + Auto-Release im
victim-tick). Faellt diese Kette EINEN Frame aus oder latcht `re15_re2z_victim_begin`
gar nicht erst (victim_ok-Gate, vs∉{0,3}-Gate), entsteht ein Zustand, den das Original
NICHT erreichen kann: **Spieler frei und laufend, Kraehe weiter im Grab-Zyklus**. Die
Kraehe faehrt dann endlos Claim → Sub 13 Hover (FOLGT dem Spieler) → Sub 14 GRAB
(HAENGT reglos am Spieler, nur der 8-Frame-Krallen-Loop) → Timeout-Release → Re-Claim.
Dass sie dabei "keine Animation" zeigt, hat zwei Anteile: Sub 14 hat NULL
Horizontalbewegung und nur den subtilen 8f-Clip-5-Loop (byte-true), und der Port
rendert die Kraehe mangels Anker-Port frei schwebend an ihrer Entity-Position statt am
Spieler verankert.

---

## 1. Frage 1 — ANIMATION in Sub 14

### 1.1 Port

* Clip-Schreiber: P0 `re2c_clip(e, 5, 0)` — crow.c:1077 (`/* Krallen 0x70005 @0x801025A4-AC */`)
  und die P4→P1-Rueckkehr crow.c:1146 (`@0x80102828-30`). `re2c_clip` (crow.c:146-154) setzt
  `anim_frac=7`, `anim_blend_rate=0x200`, `anim_freeze=0` und Loop-Bit `anim_flags|=0x04`.
* Advance: JEDER Grab-Tick laeuft durch P2/P3/P4, alle drei rufen `re2c_adv(e)`
  (crow.c:1107, 1123, 1136) = `re15_re2_advance_959c(e, 0x200)`.
* `re15_re2_advance_959c` (enemy_ai_re2_dog.c:195-205) ist der geteilte Port des
  RE2-EXE-Treibers FUN_8002959C, Kern selbst nachdisassembliert (dog.c:183-189):
  ```
  80029b28: lbu v0,333(s2)   ; Frame-Byte +0x14D
  80029b30: addiu v0,v0,1 ; 80029b34: sb v0,333(s2)
  80029b3c: sltu v0,v0,s3    ; (frame+1) < Clip-Laenge?
  80029b48: sb zero,333(s2)  ; WRAP AUF 0
  80029b4c: addiu v0,zero,1  ; done NUR am Wrap-Tick
  ```
  → **der Treiber LOOPT** (wrap auf 0, done=1 am Wrap-Tick). Ein play-once-Pin
  ("Clip laeuft aus und friert am letzten Frame") existiert in diesem Treiber NICHT —
  weder im Port noch im Original.
* Globaler Advancer: Typ 0x21 steht in `re15_type_self_advances_anim`
  (player_common.c:957) und wird in `re15_actors_anim_advance` uebersprungen
  (player_common.c:972). **Kein Doppel-Advance, kein fremder Pin.** Kehrseite: wenn das
  Kraehen-Brain nicht tickt, adanct NIEMAND die Kraehe — jeder Tick-Ausfall = sofortiger
  Stand der Pose.

### 1.2 Original (eigenes Disasm, EMOVL21_S0.BIN)

Exec14 @0x8010249C, Phasen-Tabelle @0x801000AC (selbst gedumpt):
`{[0] 0x80102508, [1] 0x801025cc, [2] 0x801026c0, [3] 0x8010273c, [4] 0x801027b8, [5] 0x80102848}`,
Dispatch-Gate `sltiu v0,v1,0x6; beq → 0x8010289c` @0x801024e4-e8 (sub2≥6 = NOP-Tick).

* Clip 5: `lui v0,0x7; ori v0,v0,0x5; sw v0,332(s0)` @0x801025a4-ac (P0) und
  @0x80102828-30 (P4-Ende → P1).
* Advance in P2/P3/P4: `jal 0x8002959c; addiu a3,zero,512` @0x801026cc, @0x80102748,
  @0x801027c4 — jeder Grab-Tick, derselbe loopende Treiber.
* Clip 5 der EM021-Bank = **8 Frames** (EDD-Framezahlen 15/4/21/16/10/8/24/33/16/6/40/40,
  RE15_RE2_AI.md:1183, dort selbst geparst).

**Antwort Q1:** Port und Original spielen in Sub 14 Clip 5 (8 Frames) mit Rate 1 Frame/Tick
als ENDLOS-LOOP (a3=512-Blend). Der Clip kann NICHT auslaufen und einfrieren. Ein
buchstaebliches "keine Animation" entsteht im Port nur ueber zwei andere Wege:
(a) **Tick-Ausfall des Brains** (self-advancing Typ — s. 1.1), oder
(b) `re15_actor_clip_len(e) <= 0` (common.c:5076ff: `re15_enemy_find(0x21)` schlaegt fehl
oder `motion >= clip_count`) → `re15_re2_advance_959c` returned done=1 OHNE
`anim_frame` anzufassen (dog.c:200) → Pose steht dauerhaft, waehrend die FSM (ueber die
done-Signale) normal weiterlaeuft — ein port-exklusiver Freeze-Modus. Im normalen
Ladeweg ist die Bank aber da (main.c:549 loggt "RE2 EM021 loaded: … 12 clips").
Drittens gilt: Sub 14 ist auch byte-true fast bewegungslos — KEIN `move3d` in der ganzen
Phase (Original: einziger Positionscode ist y-Snap @0x80102580-88 und die
Lift/Drop-Rampen @0x8010277c-8c / @0x801027f8-81c; Port identisch crow.c:1052-1153) —
plus 8f-Mikro-Loop wirkt das wie "haengt reglos".

---

## 2. Frage 2 — SPIELER-SEITE (Victim-Hold)

### 2.1 Original (eigenes Disasm)

Exec14 P0 @0x80102508:

```
80102510: lh v0,0(s1)          ; PL-HP (0x800cfd4e)
80102518: bltz v0,0x80102848   ; toter Spieler -> Release
80102520: lhu v0,554(s0)       ; +0x22A
80102534: andi v0,v0,0x4       ; CLAIM-Gate
80102538: beq v0,zero,0x80102580   ; ungeclaimt: KEIN Spieler-Write
80102540: jal 0x8010452c (a0=8)    ; Broadcast 8
80102548: jal 0x8010452c (a0=64)   ; Broadcast 64
80102550: addiu a0,s1,-342     ; a0 = 0x800CFBF8 = SPIELER-Basis
80102558: jal 0x80104078 (a1=5,a2=0)  ; PL -> (5,0)  = Opfer-Command
80102560: sw s0,-596(at)       ; 0x800CFDAC = PL+0x1B4 := Kraehen-Zeiger (Greifer)
80102568: lw v0,392(s0) ; sw v0,-640(at)  ; 0x800CFD80 = PL+0x188 := Kraehe+0x188
80102574: lw v0,396(s0) ; sw v0,-636(at)  ; 0x800CFD84 = PL+0x18C := Kraehe+0x18C
80102580: lh v0,548(s0) ; sw v0,60(s0)    ; y-Snap auf +0x224 (BEIDE Zweige)
8010259c: jal 0x80015558 (a3=2048)        ; Yaw-Snap auf den Spieler
801025a4-ac: Clip 0x70005 ; 801025b0-b4: Timeout +0x219=100 ; 801025b8-bc: Speed 300
```

Der Spieler-seitige Handler ist die kraehen-installierte Opfer-Maschine
(Dispatcher 0x80104740 → Tabelle @0x80104A80 auf PL+0x5; FSM 0x8010477C mit
Phasen-Tabelle @0x80100124 auf PL+0x6 — alles selbst disassembliert):

* P0 @0x801047b8: PL-Clip 0x70000 (Intro, 14f), Phase 1.
* P1 @0x801047c8: 959c(512); done → Clip 0x70001 (Hold, 36f), Phase 2 (@0x801047d8-ec).
* P2 @0x801047f0: NUR 959c(512) → **Hold-Clip 1 loopt ENDLOS, kein eigener Ausstieg,
  kein Timeout** — die Maschine wartet ausschliesslich auf PL+0x6=3 (Release der Kraehe)
  oder HP<0.
* P3 @0x80104804: PL-HP<0 → **PL cmd = 3** (Todes-Command; `sb 3,0x800cfbfc` +
  Nullen +0x5/6/7 @0x80104818-38); sonst Clip 0x70002 (Release, 20f) + Phase 4
  (@0x80104844-50).
* P4 @0x80104854: 959c; done → **PL → (1,0)** via 0x80104078 (@0x80104870-80) = frei.

Der Original-Spieler in cmd 5 hat KEINE Locomotion und KEINE Waffe (der cmd-Dispatch
ersetzt den gesamten Command-State) → **"Spieler laeuft, waehrend die Kraehe greift" ist
im Original UNERREICHBAR.** Zudem folgt die Original-Kraehe dem Spieler in Sub 14 nicht
(kein XZ-Code, s.o.) — das Paar steht.

### 2.2 Port — wo der Hold unterbleiben oder zerfallen kann

Der Port setzt den Hold in exec14 P0 (claimed): `re15_re2z_victim_begin(e, pl, 0)`
(crow.c:1073). Gates darin (common.c:2350-2428):

1. **Bank-Gate** (common.c:2352-2353): `vb = re15_enemy_find(0x21)` muss existieren UND
   `vb->victim_ok` (RE2-Pfad: re2_ems.c:115, Paar 3 = Victim {14,36,20}F; RE1.5-Fallback:
   main.c:819). Schlaegt das fehl, passiert spielerseitig NUR
   `player->hit_react |= 1; s_player_grabbed = 1` (common.c:2426-2427 — ein Frame) —
   **kein Victim-State, kein Pin** → der Spieler laeuft frei weiter, waehrend die Kraehe
   den kompletten Grab inkl. Pecks durchzieht.
2. **vs-Gate** (common.c:2388): Eintritt nur bei `g_player_victim ∈ {0, 3}`. Bei vs 1/2
   (fremder Greifer) unterbleibt der Eintritt, ABER common.c:2354-2356 ueberschreibt
   `g_player_victim_type/zombie/variant` TROTZDEM (steht VOR dem vs-Check) —
   Cross-Greifer-Korruption der laufenden Opfer-FSM (in reinen Kraehen-Raeumen ohne
   Zweit-Greifer nicht ausloesbar).
3. **Per-Frame-Pin**: `re15_enemy_ai_run_all` nullt `s_player_grabbed` JEDEN Frame
   (common.c:13164). Die Kraehe re-pinnt am exec14-Ende NUR wenn
   `claimed && state==1 && sub==14 && re15_player_victim_state()==1` (crow.c:1157-1159).
4. **Auto-Release**: der victim-tick (game_step_common.c:1755 → common.c:1385ff) macht
   bei `vs==1 && !s_player_grabbed` sofort vs:=3 (Release-Finish, common.c:1471-1485)
   und bei fehlender Bank sogar vs:=0 HART (common.c:1390).

Konsequenz: **jeder einzelne Frame, in dem exec14 nicht bis zum Pin kommt** (Kraehe in
HURT state 2/3; Brain-Tick-Ausfall; vs kippt aus anderem Grund), loest den Spieler —
die Kraehe bleibt aber claimed in Sub 14 und **peckt weiter** (P1-Peck crow.c:1084-1099
prueft NUR das Claim-Bit, nie den Victim-State): `re15_re2_player_damage_mode(pl, 5, mode)`
zieht einem frei laufenden Spieler unsichtbar 5 HP pro Peck-Zyklus ab
(enemy_ai_re2_zombie.c:798-805).

Victim-Clips selbst sind korrekt gemappt: Kraehe = {Intro 0 (14f), Hold 1 (36f, loopt),
Release 2 (20f)} (common.c:882-892) — deckungsgleich mit der Original-Maschine oben;
fuer 0x21 gibt es byte-true KEINE Platzierung (kein ad68 — common.c:1512-1516, Original
bestaetigt: die FSM 0x8010477C enthaelt nur 959c-Aufrufe).

**Anker-Divergenz (Render):** die Original-Writes PL+0x1B4 (Greifer-Zeiger) und
PL+0x188/+0x18C (@0x80102560-7c) sind im Port dokumentiert uebersprungen
("EXE-Render-Zeiger, doc" crow.c:1068-1072); der PC-Renderer zeichnet die Kraehe an
ihrer eigenen Entity-Position (main.c:7346-7351 fuer den Schatten; Modellpfad generisch
npc->x/y/z). **Selbst ein KORREKTER Grab zeigt im Port also eine frei neben/ueber Leon
schwebende Kraehe** statt der am Spieler verankerten — das verstaerkt den
"haengt einfach nur da"-Eindruck. Wer PL+0x188/+0x18C im RE2-EXE-Renderer liest, ist
nicht RE'd (OPEN, s. §6).

---

## 3. Frage 3 — RELEASE-Matrix

Original-Release @0x80102848-98 (eigenes Disasm):

```
80102848: jal 0x80015fe8 ; andi 0x7f ; sb v0,544(s0)   ; Nav-Reseed +0x220
80102854: jal 0x8004aa50 ; sb v0,543(s0)               ; +0x21F
8010285c: jal 0x80104088 (a1=4)                        ; Sub 4
8010286c: lhu v0,554(s0) ; andi 0x4 ; beq -> exit      ; nur wenn CLAIMED:
80102880: jal 0x8010472c (350)                         ; Vol 350
80102888: jal 0x8010452c (16)                          ; Broadcast 16
80102890: addiu v0,zero,3
80102898: sb v0,-1026(at)      ; 0x800cfbfe = PL+0x6 := 3   ; BEDINGUNGSLOS
```

| Pfad | Port | Original | Befund |
|---|---|---|---|
| Timeout (+0x219 signed <0) | P2/3/4 je `-= 3*mash+1`, Test `(int8_t)<0` → `re2c_grab_release` (crow.c:1109-1111/1125-1126/1138-1139) | @0x801026dc-f8 / @0x80102758-74 / @0x801027d4-f0, `sll 24; bltz` | identisch; Dekrement laeuft in JEDEM exec14-Tick (P0→P1→P2-Kaskade am Eintritt = Review-Fix #4, byte-true Fallthroughs @0x801025c8→cc, @0x801026b8→c0). **Nicht ueberspringbar, solange das Brain tickt** — max. 100 Ticks (~3,3 s). |
| Mash | beschleunigt nur das Timeout (re15_re2z_mash → re15_mash_pressed, common.c:2344) | 0x8001598c @0x801026d4 etc. | identisch |
| One-Save (Peck wuerde toeten, Spieler lebte) | `r==1` → release (crow.c:1094); danach `pl->hp>=0 && vs==1` → throwoff | ret-1-Zweig @0x80102670 → 0x80102848 | Port-Gate s.u. |
| Peck-Tod | `r==2` → Broadcast 128 + release (crow.c:1095-1097) | @0x80102678-88 | identisch; Original schreibt PL+0x6=3 auch hier, der P3-Handler biegt bei HP<0 auf PL-cmd 3 (Tod) ab (@0x80104810-38) — Port ueberlaesst das dem Port-Todespfad (hp<0) |
| P0-Eintritt mit totem Spieler | crow.c:1062 | `bltz @0x80102518` | identisch |
| **Spieler-Release beim Release** | **gated**: `pl->hp>=0 && re15_player_victim_state()==1 → re15_player_victim_throwoff()` (crow.c:1042-1043) | **bedingungslos** `sb 3,0x800cfbfe` @0x80102890-98 (nur Claim-Gate) | **DIVERGENZ** — harmlos nur, weil der Port-Auto-Release die vs≠1-Faelle schon abgeraeumt hat; sie dokumentiert aber, dass der Port hier auf das Per-Frame-Geschirr angewiesen ist |
| **HURT waehrend Grab** | KEIN `grab_release`-Aufruf im ganzen HURT-Pfad (crow.c:1439-1453, Root-Zweig 1812) — der Damage-Writer ueberschreibt +0x4/+0x5, der Post-Pass gibt Mutex+Claim frei (crow.c:1866-1870 = @0x801044c4-f0), den SPIELER loest nur der Auto-Release | strukturell gleich (kein Release-Write im HURT-Dispatch @0x80104A18) — aber unerreichbar: der gepinnte cmd-5-Spieler kann nicht schiessen | **port-erreichbare EINSEITIGKEIT**: Kraehe raus aus Sub 14, Spieler haengt (bis Auto-Release greift); bzw. umgekehrt nach Auto-Release: Spieler frei, Kraehe peckt weiter |
| Abort-Listener (Bit 16) in DEC14 | crow.c:1048-1051 → Sub 4 OHNE Spieler-Release | DEC14 @0x8010247c = nur `jal 0x80104400` (selbst disasm'd) | identisch; fuer den CLAIMER unerreichbar (Broadcast schliesst Bit-4-Traeger aus, crow.c:229 = `andi 4; bne` @0x8010456c-70) |
| Raumwechsel | re15_enemy_reset → re15_player_victim_reset (common.c:1761), Aktoren neu | Room-Init FUN_80052f3c cleart 0x800CFBF4 | kein Leck |
| sub2 ≥ 5 in Sub 14 | Port: `else` = P4 (crow.c:1135) | Original: [5]=Release-Zeile, ≥6 = NOP-Exit (`sltiu 0x6` @0x801024e4) | latente Divergenz OHNE Produzenten (kein Schreiber erzeugt sub2≥5 in Sub 14) |

**Kann ein Release die Kraehen-Seite loesen und den "Anker" stehen lassen?** Im Port gibt
es keinen Anker (doc-skip, §2.2) — nichts kann stehenbleiben. **Umgekehrt** (Spieler
geloest, Kraehe bleibt in Sub 14) ist der port-typische Fall: JEDER vs-Zerfall
(Auto-Release common.c:1471-1485, Bank-Hart-Reset common.c:1390) loest NUR den Spieler;
die Kraehe merkt davon nichts und bleibt bis zu 100 Ticks im Grab, peckend, reglos.

---

## 4. Frage 4 — Was laesst die Kraehe FOLGEN?

* **Sub 13 Hover ist der einzige Spieler-Verfolger der Angriffsfamilie** (crow.c:989-1029;
  Original exec13-Schwanz selbst disasm'd @0x801023f4-45c): Steer auf den Spieler
  (claimed: Rate 96, Accel +4; sonst 384/+12; Cap 300 — crow.c:1008-1014 =
  @0x80102328-a4), Hoehenklemme ±60 auf `pl_floor−2880`, und `move3d` NUR bei
  dist ≥ 650 (`sltiu 0x28a; beq → 0x80102454: jal 0x80015350` — bei dist<650 UND
  verfehltem Hoehenfenster steht sie auf der Stelle und dreht nur mit). Commit:
  dist<650 + Hoehenfenster ±99 → Sub 14 (`jal 0x80104088(14)` @0x80102444-48) —
  **OHNE Claim-Gate** (Join-Kraehen greifen als Phantom).
* **Sub 4 Kreisen** steuert ebenfalls auf den Spieler (Navigator-MAPPING, crow.c:617).
* **Sub 14 folgt NICHT**: kein XZ-Move in P0-P4 (Port crow.c:1052-1153; Original: nur
  y-Snap @0x80102580-88, Yaw-Snap 15558 @0x8010259c, Lift/Drop-Rampen). Speed 300 wird
  gesetzt (@0x801025b8-bc), aber nie in eine Bewegung umgesetzt — sie wirkt erst nach dem
  Release in Sub 4.
* Nebenschauplatz: der Root-Tail-Body-Push `re15_body_push(pl, 450, e, r)`
  (crow.c:1891-1892; Pushee = Kraehe, common.c:4013ff verschiebt den Pushee) schiebt die
  haengende Kraehe vor dem hineinlaufenden Spieler her — "klebt an mir" auch bei Kontakt.

**Die beobachtete Kombination "haengt an mir + folgt mir" = der Wechselzyklus**:
Claim (Arbiter, Fenster dist∈[901,7195], crow.c:283-291) → Sub 13 (folgt) → Sub 14
(haengt, ≤100 Ticks) → Release → Sub 4 → Re-Claim (der Releaser bekommt KEINEN eigenen
Cooldown — `re2c_grab_release` fasst `re2d_abort21c` nicht an, crow.c:1032-1047; das
Original auch nicht @0x80102848-98; nur das dist≥901-Fenster verzoegert) → wieder Sub 13…
Solange der Spieler frei laeuft, wiederholt sich das endlos. Genau dieser Zustand ist im
Original unerreichbar, weil der Spieler waehrend des Zyklus im cmd-5-Pin steht.

---

## 5. Ursachen (gerankt)

### C1 (likely) — Einseitiger Hold-Zerfall / nie gelatchter Hold → endloser Haeng-Folge-Zyklus
Der Kraehen-Grab-Zyklus laeuft gegen einen freien Spieler weiter, weil die
Spieler-Seite ihn verlassen hat oder nie betreten hat. Konkrete Code-Gates (alle
verifiziert, Ausloeser der einzelnen Nutzer-Sitzung nicht gemessen):
* Latch-Gates: `vb && vb->victim_ok` (common.c:2352) und `vs ∈ {0,3}` (common.c:2388).
* Pin-Kette: run_all-Clear (common.c:13164) + exec14-Pin nur bei
  `claimed && state==1 && sub==14 && vs==1` (crow.c:1157-1159) + Auto-Release
  `vs==1 && !s_player_grabbed → vs=3` (common.c:1471-1485) + Bank-Hart-Reset `vs=0`
  (common.c:1390).
* Folge sichtbar als: Sub 13 = folgt, Sub 14 = haengt reglos (kein XZ-Code, 8f-Loop),
  P1-Pecks treffen den freien Spieler unsichtbar weiter (crow.c:1084-1099).

### C2 (confirmed, klein) — Spieler-Release im Port gegated, im Original bedingungslos
Port crow.c:1042 `pl->hp>=0 && vs==1` vs Original `sb 3,0x800cfbfe` @0x80102890-98
(nur Claim-Gate). Teil der Matrix in §3; macht den Port vom Per-Frame-Geschirr abhaengig.

### C3 (confirmed, beitragend) — Anker nicht portiert
Original-Grab verankert PL+0x1B4/+0x188/+0x18C an der Kraehe (@0x80102560-7c); Port
ueberspringt das dokumentiert (crow.c:1068-1072) und rendert die Kraehe an ihrer
Entity-Position (main.c:7346ff) → auch der KORREKTE Grab sieht aus wie eine frei
schwebende, fast reglose Kraehe neben dem Spieler.

### C4 (speculative) — clip_len≤0-Freeze
`re15_actor_clip_len(e) <= 0` laesst `re15_re2_advance_959c` jeden Tick done melden,
ohne den Frame zu bewegen (dog.c:200) → woertlich "keine Animation" in JEDEM Zustand bei
weiterlaufender FSM. Braucht einen Fehlschlag von `re15_enemy_find(0x21)` — im normalen
Ladeweg widerlegt (main.c:549-550), als Einmal-Ereignis nicht ausgeschlossen, nicht belegt.

---

## 6. OPEN + naechste RE-/Mess-Schritte

1. **Messen, welcher Zerfall in der Nutzer-Sitzung vorlag**: `RE15_RE2_TRACE=1` loggt
   jede Kraehen-Transition (`[re2crow] … state/sub/clip/dist/flags`, crow.c:1904-1917).
   Ergaenzend eine stderr-Zeile im Auto-Release (common.c:1471-1485) und im
   victim_begin-Fail (common.c:2352/2388) — dann zeigt das Log SOFORT, ob vs kippte
   (Auto-Release), nie latchte (victim_ok/vs-Gate) oder die Bank fehlte (common.c:1390).
   Signatur des Defekt-Zustands im Log: `sub=14` + Claim-Bit (fl&4) waehrend
   `re15_player_victim_state()==0` und Spieler-Positionsaenderung.
2. **Anker-Leser RE'n** (fuer die "haengt AN mir"-Optik): wer konsumiert PL+0x188/+0x18C
   (0x800CFD80/84) und 0x800CFDAC im RE2-EXE-Render? Naechster Weg: grep
   `ghidra_re2_Leon.txt` nach 0x800cfd80/0x800cfdac-Xrefs bzw. `lw …,392/396`-Mustern im
   Spieler-/Modell-Render, dann `re2_disasm.py` ohne `--bin` auf die Treffer.
3. **Cross-Greifer-Overwrite** (common.c:2354-2356 vor dem vs-Gate) als eigenen Fund an
   die Zombie/Hund-Lane melden — in Kraehen-Raeumen inert, in gemischten Raeumen
   korrumpiert er die laufende Opfer-FSM.
4. Latente Divergenz sub2≥5 (Original-NOP vs Port-P4) dokumentieren — kein Produzent,
   kein Fix noetig.
