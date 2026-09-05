# Diagnose: RE2-Modus — Krähen-Kill ohne Spieler-Sterbeanimation (Symptom 3)

**Nutzer-Report:** „Wenn ich durch Krähen sterbe – bekomme ich keine Sterbeanimation, sondern
bleibe einfach stehen." (RE2-AI-Modus)

**Datum:** 2026-09-05. Alle `@0x…`-Zitate aus EIGENEM Disasm dieser Session
(`re2_disasm.py` auf `info/re2leon/COMMON/BIN/EMOVL21_S0.BIN` bzw. `info/re2leon/PSX.EXE`)
oder aus `RE2_Quellcode_V2/FUN_*.c` (Ghidra-Decompilat, selbst gelesen). Port-Zitate als
`datei:zeile` gegen den aktuellen Arbeitsbaum.

---

## 0. Kurzbefund

Der Port hat für den RE2-Krähen-Kill **keinen Spieler-Todes-Latch**. Im Original endet die
Kette in einem Schreiber `PL-State := (3,0,0,0)` — dem **generischen Spieler-Todes-Kommando
cmd 3** (Victim-Hook Phase 3, `@0x80104818-38` im Krähen-Overlay). Der Port besitzt das exakte
Gegenstück (`re15_player_death_cmd3()`, für die RE1.5-Krähe gebaut), **ruft es im RE2-Pfad aber
nie**: die Krähe macht bei Tod nur Broadcast 128 + Release, und der generische
hp<0-Kantendetektor in `re15_game_step` ist mit `!re15_player_is_grabbed()` gegatet — während
des Krähen-Grabs ist der Spieler aber Victim-State 1/3, das Gate blockiert, und die
hp-Kante (`s_prev_hp >= 0`) ist danach für immer verbraucht. Ergebnis: Leon spielt den
Krähen-Release-Clip (20 F), fällt auf den Idle-Sentinel `motion=200` und bleibt aufrecht
stehen; die Game-Over-Präsentation (White-Flash/YOU DIED/Death-Cam) läuft zwar an
(`re15_player_is_dead()` = hp<0), aber ohne Kollaps-Animation.

---

## 1. Original-Kette (RE2 Retail) — vollständig, selbst disassembliert

### 1.1 Der Peck-Schaden: `FUN_800401d4(5, aliveflag)`

Aufruf im Krähen-GRAB Sub 14 P1 (EMOVL21_S0.BIN, eigener Dump):

```
80102650: addiu a0,zero,2        ; SE 2 (Picken)
80102654: jal 0x8005bd6c
8010265c: lbu a1,536(s0)         ; a1 = +0x218 = (PL-HP>0)?1:0
80102660: jal 0x800401d4         ; a0 = 5 (Schaden)
80102668: addu v1,v0,zero
8010266c: addiu v0,zero,1
80102670: beq v1,v0,0x80102848   ; ret 1 (One-Save) -> RELEASE
80102678: bne v1,v0,0x801026b4   ; (v0=2) ret != 2 -> weiter
80102680: jal 0x8010452c         ; ret 2 (TOD):
80102684: addiu a0,zero,128      ;   Broadcast 128 (Pacify)
80102688: j 0x80102848           ;   -> RELEASE
```

`FUN_800401d4` selbst (Decompilat `RE2_Quellcode_V2/FUN_800401d4.c`, Disasm-Gegenprobe
PSX.EXE `@0x800402a8-f0`): Todeszweig (mode 0, HP<-14 ODER One-Save-Latch 0x1000 gesetzt):

```
800402c0: lbu v1,0x1d3(a2)       ; PL+0x1D3
800402cc: ori v1,v1,0x80
800402d0: sb v1,0x1d3(a2)        ; PL+0x1D3 |= 0x80
800402d4: lw v1,0(a1)            ; DAT_800cfb74
800402d8: lui a0,0x400
800402dc: or v1,v1,a0
800402e4: sw v1,0(a1)            ; DAT_800cfb74 |= 0x04000000  <- globales TODESBIT
                                 ; return 2
```

**`FUN_800401d4` setzt bei Tod also selbst zwei Zustände**, die der Port NICHT portiert hat
(`re15_port/engine/src/enemy_ai_re2_zombie.c:778-786` — `re2z_player_damage` gibt nur 2
zurück): das globale Todesbit `0x800cfb74 |= 0x4000000` und `PL+0x1D3 |= 0x80`.

### 1.2 Konsument des Todesbits (RE2-EXE-Hauptschleife)

Eigener Dump `ghidra_re2_Leon.txt` (Haupt-Gameplay-Schleife, `LAB_800266c0`):

```
800266c8: lw v0,0(s0)            ; DAT_800cfb74
800266cc: lui v1,0x400
800266d0: and v0,v0,v1           ; Bit 0x4000000?
800266d4: beq v0,zero,...        ; nein -> skip
800266dc: jal FUN_80032138 (a0=1); Task-Slot-1-Status (DAT_800d76a4[0x40])
800266e4: bne v0,zero,...        ; Slot belegt -> warten
800266f0: lbu DAT_800dfc1b       ; != 0 -> skip
80026704-14: lh DAT_800d4820 vs lbu DAT_800cfbf2 ; ungleich -> skip
8002671c: jal FUN_80031e80 (1,2) ; TASK 2 in Slot 1 schedulen (FUN_80012fb8 "execute"
                                 ;  + FUN_80031f6c: Status:=2) = die Game-Over-/Death-Task
80026724-34: and 0xfbffffff ; sw ; Bit 0x4000000 LOESCHEN
```

Das ist die **Präsentations**-Seite (Port-Analog: `re15_gameover_fsm_tick`, bereits auf
hp<0 gekeyt — läuft im Port auch heute). Die **Animations**-Seite kommt aus 1.3/1.4.

### 1.3 Krähen-Release schreibt UNBEDINGT `PL+0x6 = 3`

`grab_release @0x80102848` (eigener Dump; gemeinsamer Trichter ALLER Release-Wege —
ret 1, ret 2, Timeout `+0x219<0`, P0-hp<0-Frühausstieg):

```
80102848: jal 0x80015fe8         ; rand
80102850: andi v0,v0,0x7f        ; Nav-Reseed
80102854: jal 0x8004aa50
8010285c-68: jal 0x80104088 (a1=4)   ; Sub := 4 (Kreisen)
8010286c: lhu v0,554(s0)         ; +0x22A
80102874: andi v0,v0,0x4         ; geclaimt?
80102878: beq v0,zero,0x8010289c ; nein -> Ende
80102880: jal 0x8010472c (350)   ; Volume
80102888: jal 0x8010452c (16)    ; Broadcast 16 (Abbruch)
80102890: addiu v0,zero,3
80102898: sb v0,-1026(at)        ; 0x800cfbfe = PL+0x6 := 3   <- IMMER, tot ODER lebend
```

### 1.4 Der Victim-Hook `0x80104740` — Phasen, und der TODES-HANDOFF

Install (Krähen-INIT, eigener Dump):

```
801004b4: lui v0,0x8010
801004b8: addiu v0,v0,18240     ; v0 = 0x80104740
801004c0: sw v0,-7292(at)       ; 0x800CE384 = 0x800CE300[kind 0x21] = cmd-5-Hook
```

**Zensus über das gesamte Overlay (4770 Instr.): es gibt KEINEN Store nach `0x800CE484`
(= 0x800CE400[0x21], der cmd-6-Slot).** Die Krähe installiert also — anders als der Zombie
(`0x800CE300[kind]`+`0x800CE400[kind]` @0x801010ec-120, EMOVL10_S0.BIN) — **keinen
Kollaps-Handler (cmd 6)**. (EXE-Dispatcher der Hook-Tabellen: cmd 5 = `LAB_8004006c`,
`lw 0x800CE300[+8(kind)<<2]` @0x800400b0; cmd 6 = `LAB_800400d0`, `lw 0x800CE400[...]`
@0x80040114 — eigener Dump.)

`0x80104740` (a0 = Spieler-Entity): Dispatch auf `PL+0x5` über `@0x80104A80`
([0] = `0x8010477C`, Rest 0), Maschine `0x8010477C`: Dispatch auf `PL+0x6 < 6` über
Tabelle `@0x80100124` (eigener Dump):

```
[0] 0x801047B8   Ph0: Clip-Wort 0x70000 (INTRO Clip 0, Rate 7), +0x6:=1, Advance(512);
[1] 0x801047C8        done -> Clip-Wort 0x70001 (HOLD Clip 1), +0x6:=2
[2] 0x801047F0   Ph2: nur Advance, Rueckgabe VERWORFEN = HOLD-LOOP Clip 1
[3] 0x80104804   Ph3 (der RELEASE aus 1.3):
                   80104808: lh v0,-690(v0)     ; 0x800cfd4e = PL-HP
                   80104810: bgez v0,0x80104844 ; HP >= 0 -> lebend
                   ; --- TOT (HP < 0): ---
                   80104818: addiu v0,zero,3
                   80104820: sb v0,-1028(at)    ; 0x800cfbfc = PL+0x4 := 3   *** TOD ***
                   80104828: sb zero,-1027(at)  ; PL+0x5 := 0
                   80104830: sb zero,-1026(at)  ; PL+0x6 := 0
                   80104838: sb zero,-1025(at)  ; PL+0x7 := 0
                   ; KEIN Release-Clip fuer den Toten!
                   ; --- LEBEND: ---
                   80104844-48: Clip-Wort 0x70002 (RELEASE Clip 2), +0x6:=4
[4] 0x80104854   Ph4: Advance; done -> jal 0x80104078(0x800cfbf8, 1, 0) = PL (1,0) frei
[5] 0x80104884   Exit-Stub
```

**Damit ist die Frage des Auftrags beantwortet: die Victim-Bank der Krähe (EMD Paar 3 =
{14, 36, 20} F = Clips 0/1/2 Intro/Hold/Release, RE15_RE2_AI.md:1182-1187) enthält KEINEN
Kollaps — der Hook kennt nur die Clip-Wörter 0x70000/1/2. Der Spieler stirbt an einem
Krähen-Grab über `PL-State := (3,0,0,0)` = das generische Spieler-Todes-Kommando cmd 3.**

### 1.5 Der RE2-Spieler-cmd-3-Handler (EXE) — der eigentliche Todes-Kollaps

Spieler-Kommando-Dispatch-Tabelle der RE2-EXE gefunden (eigener Dump, `table 0x800a4028`):
Basis `0x800a4030`, `[cmd]`:

```
cmd 0 -> 0x8003c210    cmd 3 -> 0x8003fee4  (TOD)
cmd 1 -> 0x8003c57c    cmd 5 -> 0x8004006c  (Grab  -> Hook 0x800CE300[kind])
cmd 2 -> 0x8003f600    cmd 6 -> 0x800400d0  (Fress -> Hook 0x800CE400[kind])
                       cmd 7 -> 0x80040134  (Leiche/Lache)
```

`FUN_8003fee4` (cmd 3, eigener Dump; Sub-Dispatch `@0x800a40fc[PL+0x5]`, Maschine
`@0x8003ff3c`):

```
Ph0 @0x8003ff9c: lui v0,0x7 / ori v0,0x2 / sw v0,332(s1)  ; Clip-Wort 0x70002 =
                                                          ;   SPIELER-EIGENE Bank, Clip 2, Rate 7
                 (Bank = a1/a2 aus dem Prolog @0x8003ff08-0c: PL+0x108 / PL+0x17C
                  = die NORMALE Spieler-Animbank, NICHT die Victim-Bank!)
                 sw zero,324(s1) / sh zero,328(s1)        ; +0x144-Vektor clear
                 +0x1D3 |= 0x80 (@0x8003ffb0/c4/cc), +0x1C0 := 1 (@0x8003ffdc)
                 jal 0x8005ba28 (a0=0x0403'0001)          ; Se_on 0x04030001 = DERSELBE
                                                          ;   Todes-SE wie RE1.5-cmd-3
                                                          ;   (@0x800367a8, RE1.5-EXE)
                 jal 0x8003947c(3,0) / 0x80039514(8,200,0) / 0x800395b8(90,200,0,8)
                                                          ; Shake + Rumble
                 +0x6 := 1
Ph1 @0x80040004: Advance 0x8002959c(a3=512); +0x6 += done
Ph2 @0x8004002c: sw 7,4(s1)                               ; PL-State-Wort := 7 (cmd 7 = Lache)
```

Architektonisch **identisch** zu RE1.5s cmd 3 (`@0x800366bc`: PL00-Clip 7, Se_on(0x04030001),
danach cmd 7) — die Tabellen-Slots 3/5/6/7 sind in beiden Engines dieselben. Das Port-Analog
existiert bereits: `re15_player_death_cmd3()` / `…_tick()`
(`re15_port/engine/src/game_step_common.c:169-243`), gebaut für die RE1.5-Krähe.

---

## 2. Der funktionierende Vergleichspfad: RE2-Zombie (und -Hund)

Der Zombie hat einen EIGENEN Kollaps: sein Overlay installiert cmd-5 UND cmd-6-Hooks
(`0x800CE300/0x800CE400[kind]` @0x801010ec-120; cmd 6 = `0x8010B3C0`, Kollaps-Clips 13/15,
Tod erst in P2 `sh -32768,342` @0x8010B738 — RE15_RE2_AI.md:477-502). Der Kill-Tick des
Zombies schreibt `PL cmd (dir<<8)|6` (@0x8010293C-50) — und der **Port bildet genau das ab**
(`re15_port/engine/src/enemy_ai_re2_zombie.c:1483-1491`):

```c
if (r & 2) {                              /* player dead @0x80102920-50 */
    pl->state = 7;                        /* grabbed death (port death FSM) */
    re15_re2z_victim_devour(e, re15_ai_facing_aligned(e, pl));
    re15_ai_set_state_word(e, 0x601);
}
```

`re15_re2z_victim_devour` → `re15_player_victim_devour` → `g_player_victim = 2` (Kollaps)
(`enemy_ai_common.c:2434-2444` / `:1160-1207`) → der Victim-Tick spielt die Kollaps-Clips.
**Die Krähe hat nichts davon** — im Original zu Recht (kein cmd-6-Hook, kein Kollaps-Clip in
der Bank), aber das Original ersetzt es durch den `(3,0,0,0)`-Handoff (1.4), und DEN hat der
Port nicht.

---

## 3. Port-Kette beim Krähen-Kill — wo genau es klemmt

### 3.1 Kill-Tick (Krähe)

`re15_port/engine/src/enemy_ai_re2_crow.c:1092-1098` (Sub-14 P1):

```c
int r = re15_re2_player_damage_mode(pl, 5, (int)e->re2d_route218);
if (r == 1) { re2c_grab_release(e, pl); goto grab_out; }
if (r == 2) {                              /* Tod @0x80102674-78 */
    re2c_broadcast(128);                   /* @0x80102680-84 */
    re2c_grab_release(e, pl); goto grab_out;
}
```

`re15_re2_player_damage_mode` (`enemy_ai_re2_zombie.c:798-805` → `re2z_player_damage`
`:778-786`) senkt nur `pl->hp` und gibt 2 zurück — **kein Spieler-Zustand, kein Todesbit**
(die Original-Stores `@0x800402c0-e4` aus 1.1 sind nicht portiert).

### 3.2 Release lässt den toten Spieler im Victim-State 1 stehen

`enemy_ai_re2_crow.c:1032-1047` (`re2c_grab_release`):

```c
re2c_sub(e, 4);                                        /* @0x8010285C-64 */
if (e->re2c_flags22a & 0x4u) {
    re2c_vol(e, 350);
    re2c_broadcast(16);
    if (pl->hp >= 0 && re15_player_victim_state() == 1)
        re15_player_victim_throwoff();                 /* PL+0x6=3 @0x80102890-98 (Port:
                                                        * Victim-Release-Phase; toter Spieler
                                                        * laeuft ueber den Port-Todespfad) */
}
```

Für den TOTEN Spieler (`hp < 0`) passiert hier **gar nichts** — der Kommentar verweist auf
den „Port-Todespfad", der aber nie erreicht wird (3.4). Im Original schreibt dieselbe Stelle
`PL+0x6 := 3` **unbedingt** (`@0x80102890-98`), und die Victim-Maschine macht daraus im
Ph3-Tick den `(3,0,0,0)`-Todes-Handoff (`@0x80104804-38`).

### 3.3 Der Victim-Tick spielt dem Toten den RELEASE-Clip und stellt ihn dann in den Idle

Da die Krähe nach dem Release nicht mehr pinnt (Pin-Gate `sub_state_1 == 14`,
`enemy_ai_re2_crow.c:1157-1159`; `s_player_grabbed` wird pro Frame in `run_all` gelöscht,
`enemy_ai_common.c:13164`), sieht der Victim-Tick im selben game_step:

* `enemy_ai_common.c:1471-1485`: Zustand 1, `!s_player_grabbed` → **Zustand 3** +
  `player->motion = c_release` (Krähen-Clip 2, 20 F) — **auch für den Toten** (Divergenz:
  das Original spielt dem Toten NIE den Release-Clip, `bgez @0x80104810` überspringt ihn).
* `enemy_ai_common.c:1699-1751`: Release-Clip zu Ende → `g_player_victim = 0`,
  `player->motion = 200` (**Idle-Sentinel**), `anim_frame = 0`.

Ab hier steht der tote Leon aufrecht im Idle — exakt das Symptom.

### 3.4 Warum der „Port-Todespfad" (cmd 3) NIE feuert

`re15_port/engine/src/game_step_common.c:1005-1008`:

```c
if (c->rdt_ok && pl->hp < 0 && s_prev_hp >= 0 &&
    !re15_player_is_grabbed() && !re15_stair_active())
    re15_player_death_cmd3();
s_prev_hp = pl->hp;
```

* Auf der hp<0-KANTE (erster Tick nach dem Kill-Peck) ist `re15_player_is_grabbed()` **wahr**:
  `enemy_ai_common.c:540-549` zählt Victim-State 1/2/3 als „grabbed", und der Spieler steht
  in 1 (Kill-Tick) bzw. 3 (Release-Finish, ~20 Ticks). → cmd 3 wird NICHT gelatcht.
* Danach ist `s_prev_hp < 0` — **die Kante ist ein One-Shot und für immer verbraucht.** Wenn
  der Victim-State nach dem Release-Clip auf 0 fällt, kann der Detektor nie wieder feuern.
* Der Kommentar direkt darüber (`game_step_common.c:1001-1004`) dokumentiert die Absicht:
  „Die KRAEHE latcht cmd 3 dagegen selbst, aus ihrem Brain heraus (@0x80113B48 / @0x80113F20 /
  @0x80114518, s. enemy_ai_common.c)" — **das gilt nur für die RE1.5-Krähe**
  (`enemy_ai_common.c:5880/5972/6062` rufen `re15_player_death_cmd3()`). Die RE2-Krähe
  (`enemy_ai_re2_crow.c`) hat keinen einzigen Aufruf (grep über den Baum: nur die drei
  RE1.5-Stellen + der generische Detektor).

### 3.5 Was stattdessen läuft: nur die Präsentation

`re15_player_is_dead()` (`re15_damage.c:266-294`) ist hp<0; die RE2-Pacing-Ausnahme
(`:284-290`) klammert nur Zombie-Familie + Hund (0x20) aus, NICHT die Krähe (0x21). Deshalb:

* `game_step_common.c:1035-1036`: `re15_gameover_fsm_tick()` läuft ab dem Kill-Tick
  (White-Flash nach ~50, Schwarz-BG/YOU-DIED/Death-Cam nach ~77 Ticks, `:557-617`).
* `game_step_common.c:1099-1123`: der Death-Zweig friert den Spieler ein (kein player_tick).

Der Nutzer sieht also einen aufrecht stehenden Leon, um den die Todes-Präsentation
herumläuft — „keine Sterbeanimation, bleibe einfach stehen".

---

## 4. Der NICHT-Grab-Fall (Auftrag Punkt 4): STRIKE kann im Port nicht töten

`enemy_ai_re2_crow.c:921-940` (Sub 12 P0): schreibt nur `pl->hit_react |= 1` — **kein
HP-Verlust** (dokumentiert offen: „SCHADENSHOEHE liegt im EXE-Spieler-State-2-Handler — nicht
RE'd"; RE15_RE2_AI.md:1175-1178: „kein 0x800401d4-Caller"). Im Original schreibt der STRIKE
den Spieler-State (2, Sub 1/0) `@0x8010201C-34`; der Schaden fällt im EXE-cmd-2-Handler
(`0x8003f600`, Tabelle 1.5 — noch nicht RE'd). **Damit ist der GRAB-Peck (5 HP, mode-0-Pfad
mit One-Save-Latch) der einzige Kill-Weg der RE2-Krähe im Port** — jeder Krähen-Tod des
Nutzers läuft durch die blockierte Kette aus §3.

Randnotiz: die RE2-Spinne wirft den Rückgabewert von `re15_re2_player_damage_mode(pl,20,0)`
weg (`enemy_ai_re2_spider.c:338`) — ihr Biss ist aber kein Grab (kein Victim-State), dort
greift der generische Kantendetektor (§3.4) mit `is_grabbed()==0`. Nur die Krähe kombiniert
„Kill im Grab" mit „kein eigener Todes-Latch".

---

## 5. Fix-Skizze (byte-true Mapping, KEIN Code geändert)

Das Original prüft die HP im **Victim-Hook Ph3** (`@0x80104804-38`): tot → `PL(3,0,0,0)`,
Release-Clip wird übersprungen; lebend → Release-Clip 2 → `PL(1,0)`. `grab_release`
(`@0x80102848`) ist der EINZIGE Schreiber von `PL+0x6=3` und der gemeinsame Trichter aller
Release-Wege (ret 1, ret 2, Timeout, P0-hp<0). Port-Äquivalent:

1. **In `re2c_grab_release` (enemy_ai_re2_crow.c:1042), innerhalb des Claimed-Gates:**
   `pl->hp < 0` → `re15_player_death_cmd3()` statt (bzw. vor) dem Nichtstun.
   `re15_player_death_cmd3()` ruft `re15_player_victim_reset()` (game_step_common.c:177)
   — der Krähen-Release-Clip wird damit NIE gespielt, exakt die `bgez`-Semantik
   `@0x80104810`; der Todes-Clip (PL00 Clip 7 + Se_on 0x04030001) startet im
   `re15_player_death_cmd3_tick` desselben game_step (game_step_common.c:1761).
   Beleg-Kommentar: `PL+0x4:=3 @0x80104818-38` (Victim-Hook Ph3, EMOVL21_S0.BIN) +
   RE2-cmd-3-Handler `FUN_8003fee4` (Clip-Wort 0x70002 eigene Bank @0x8003ff9c,
   Se_on 0x04030001 @0x8003ffa0-a4, cmd 7 @0x8004002c).
   * Timing-Detail: das Original schreibt `(3,0,0,0)` erst im Ph3-Tick des Spieler-
     Dispatchers (ein Dispatch nach dem Release-Write); der Port-Latch am Release-Tick ist
     1 Dispatch früher. Alternativ (noch wörtlicher) den Check in die Victim-FSM-Transition
     `enemy_ai_common.c:1472` legen (Typ 0x21 + RE2-Flavor + hp<0 → cmd 3 statt Zustand 3).
     Beide Stellen decken dieselben Fälle ab, weil `grab_release` der einzige
     `PL+0x6=3`-Producer ist.
2. **Nachziehen (klein, adressbelegt):** `re2z_player_damage`-Todeszweig um
   `pl->re2z_self1d3 |= 0x80` ergänzen (`@0x800402c0-d0`) — Original-Store im selben
   ret-2-Pfad. Das globale Todesbit `0x800cfb74|=0x4000000` (`@0x800402d4-e4`) braucht kein
   eigenes Port-Feld: sein einziger Konsument (Haupt-Loop `@0x800266c8-0x80026734`) schedult
   die Game-Over-Task — Port-Analog `re15_gameover_fsm_tick` keyt bereits auf hp<0
   (game_step_common.c:1035). Als MAPPING im Kommentar dokumentieren.
3. **Nicht anfassen:** RE1.5-Krähe (hat ihre drei cmd-3-Sites), RE2-Zombie/Hund
   (cmd-6-Kollaps über `re15_re2z_victim_devour`), der generische Kantendetektor
   (game_step_common.c:1005 — sein `!is_grabbed()`-Gate ist byte-true: der Original-Resolver
   FUN_80012D60 schließt den Griff ebenso aus, Kommentar `:999-1004`).

## 6. Repro / Messung (für den Hauptagenten)

* Vorlage: `re15_port/tests/unit/probe_crow_kill.c` (misst exakt diese Kette für die
  RE1.5-Krähe: `pl->motion/anim_frame/victim_state/g_death_*` über 200 Ticks nach hp<0;
  Erwartung motion→7 für 113 Frames). Für RE2: dieselbe Sonde mit RE2-Flavor
  (`re15_ai_flavor` auf RE2 setzen wie in `test_re2_room10c0_ab`), Krähen-Grab erzwingen
  (Sub 13→14 via Distanz/Claim wie in `test_re2_crow_ai`), HP auf 0 + One-Save-Latch setzen,
  einen Peck ticken. **Erwartetes IST (der Bug):** `motion == 2` (Krähen-Release-Clip)
  für ~20 Ticks, dann `motion == 200` dauerhaft; `re15_player_death_cmd3_active() == 0`;
  `motion == 7` wird NIE erreicht. **SOLL:** motion springt im Release-Tick (±1 Tick,
  s. Fix-Skizze Timing) auf 7 und läuft `pl00_anim->clips[7].frame_count` (113) Frames,
  danach `state 7`/Lache (cmd-7-Analog).
* Live: `RE15_AI_FLAVOR=RE2` + `RE15_DEBUG_JUMP=10c0@gp`, von Krähen töten lassen
  (nur der Grab-Peck kann töten, §4 — Grab abwarten, nicht mashen).

## 7. OPEN (ehrlich, mit nächstem RE-Weg)

* **RE2-STRIKE-Schadenshöhe** (Spieler-cmd-2-Handler `0x8003f600`, Tabelle `@0x800a4030[2]`,
  jetzt bekannt!): der bisherige OFFEN-Eintrag „Handler-Tabelle nicht gefunden"
  (RE15_RE2_AI.md:1223-1225) ist damit ÜBERHOLT — nächster Schritt: `0x8003f600`
  disassemblieren (Sub-Tabelle vermutlich analog `@0x800a40xx`), dort liegt der
  STRIKE-HP-Verlust. Erst dann darf der Port-STRIKE HP abziehen.
* Reihenfolge Spieler-Dispatch vs. Gegner-Tick innerhalb eines RE2-Frames (ob der
  Ph3-Handoff im selben oder im Folge-Frame des Release liegt) — für die Fix-Platzierung
  (±1 Tick) irrelevant genug, aber nicht gemessen.
* `DAT_800d4820`/`DAT_800cfbf2`/`DAT_800dfc1b`-Semantik der Task-Gates (§1.2) — nur für die
  Präsentations-Task relevant, Port deckt sie über die eigene Game-Over-FSM ab.

## 8. Beleg-Inventar (Kurzliste)

| Behauptung | Beleg |
|---|---|
| Peck-Kill → nur Broadcast 128 + Release | eigener Dump `@0x80102660-88` |
| FUN_800401d4 ret 2 setzt `0xcfb74\|=0x4000000`, `+0x1D3\|=0x80` | Decompilat FUN_800401d4.c:34-38 + Dump `@0x800402c0-e4` |
| Release schreibt `PL+0x6=3` unbedingt | Dump `@0x80102890-98` |
| Victim-Hook-Install, KEIN cmd-6-Hook | Dump `@0x801004b4-c0`; Zensus 4770 Instr.: 0× `0x800ce484` |
| Ph3: HP<0 → `PL(3,0,0,0)`, kein Release-Clip | Dump `@0x80104804-38`, Tabelle `@0x80100124` |
| Victim-Bank ohne Kollaps (Clips 0/1/2) | Hook-Clip-Wörter 0x70000/1/2 (Dump) + RE15_RE2_AI.md:1182-1187 |
| RE2-cmd-Tabelle, cmd 3 = FUN_8003fee4 | Dump `table @0x800a4028`, XREFs `800a4044/48(*)` |
| cmd 3 = eigener Todes-Clip 2 + Se_on 0x04030001 + cmd 7 | Dump `@0x8003ff9c-0x8004002c` |
| Todesbit-Konsument = Game-Over-Task | Dump `@0x800266c8-34` + FUN_80031e80/80031f6c/80032138.c |
| Port-Kill-Zweig ohne Todes-Latch | enemy_ai_re2_crow.c:1092-1098 |
| Port-Release: Toter bleibt Victim-1 | enemy_ai_re2_crow.c:1032-1047 |
| Victim-Tick: Toter spielt Release-Clip, dann Idle 200 | enemy_ai_common.c:1471-1485, 1699-1751 |
| cmd-3-Kante gegatet + One-Shot | game_step_common.c:1005-1008; is_grabbed enemy_ai_common.c:540-549 |
| RE1.5-Krähe latcht cmd 3 selbst (funktionierender Zwilling) | enemy_ai_common.c:5963-5972, 6061-6064; game_step_common.c:66-135 |
| Zombie-Kill-Pfad funktioniert (cmd-6-Analog) | enemy_ai_re2_zombie.c:1483-1491; enemy_ai_common.c:2434-2444 |
| STRIKE ohne HP-Verlust (einziger Kill-Weg = Grab) | enemy_ai_re2_crow.c:921-940; RE15_RE2_AI.md:1175-1178 |
| Präsentation läuft trotzdem (Leon steht) | re15_damage.c:266-294; game_step_common.c:1035-1036, 557-617 |
