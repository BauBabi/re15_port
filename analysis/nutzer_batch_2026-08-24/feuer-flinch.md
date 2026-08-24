# FINDING 5 — "fehlt noch die Verletzt Animation wenn man in das Feuer reinlaeuft" (ROOM1090, beide Flavors)

**Status: URSACHE GEFUNDEN UND GEMESSEN. Keine Code-Aenderung in dieser Phase.**
**Datum:** 2026-08-24 · **Raum:** ROOM1090 (brennender Hinterhof) · **Typ 0x26 = Feuer-Emitter**

---

## 0. Kurzfassung

Der v0.3.18-Fix war **nicht falsch, aber unvollstaendig** — und die damalige Sonde hat genau
die Haelfte gemessen, die schon funktionierte.

* **Engine-Zustand ist korrekt.** Der Feuer-Kontakt schreibt `pl->motion = 8/9`,
  `s_hit_flinch = 22`, `-2 HP/Frame`, CORE-SE 1/2 — alles byte-true, alles messbar.
* **Der RENDER-Pfad zeigt die Pose trotzdem nicht.** In ROOM1090 posiert der Port den
  Spieler nicht aus der COMMON-Bank `PL00.EDD` (dort hat Clip 8 = **22 Bilder**), sondern
  aus der **Raum-Cinematic-Bank** (RDT@0x5C, Record 0), in der Clip 8 **1 Bild** hat.
  Ergebnis: der 22 Bilder lange Flinch rendert als **ein einziger, eingefrorener
  Cutscene-Keyframe**. Der Nutzer sieht: keine Verletzt-Animation.
* Das erklaert auch "**beide Flavors**": der KI-Flavor beruehrt die Render-Bank nicht.
* Das erklaert auch, warum das **Geraeusch inzwischen stimmt** (Engine-Seite, v0.3.18 hat
  sie repariert) und **nur noch die Animation fehlt** (Render-Seite, nie angefasst).

Es ist **kein** Feuer-Bug. Es ist ein **Spieler-Render-Bank-Bug**, der in jedem Raum mit
RDT-Animationsblock jede cmd-2/cmd-3-Reaktion (Flinch, Knockdown, Tod) trifft. ROOM1090
ist nur der Raum, in dem der Nutzer zuverlaessig Schaden nimmt.

---

## 1. Was das ORIGINAL tut

### 1.1 Der Feuer-Schaden — Root `FUN_80116288`, Tail (STAGE1.BIN, `--bin STAGE1.BIN`)

Dispatch `0x80072bac[0x26] = 0x80116288` (Registrierung `addiu v0,v0,25224` @0x8011E8F4 →
`sw v0,11332(at)` @0x8011E8FC). Der Tail laeuft nach JEDEM Zustands-Dispatch:

```
80116320  lbu  v0,464(a0)          ; entity+0x1D0 = Flammen-Phase
80116328  sltiu v0,v0,0xd
8011632c  bne  v0,zero,0x801163dc  ; Phase < 13 -> intangibel (flags |=2, |=0x40)
...
80116360  lui  a1,0x800b
80116364  lw   a1,-14460(a1)       ; a1 = Feuer-Entity
80116368  jal  0x8002aec4          ; Hitbox-Ueberlapp; a0 = s0 = 0x800aca54 = SPIELER
8011636c  addu a0,s0,zero
80116370  beq  v0,zero,0x80116408  ; kein Ueberlapp -> raus
80116378  lui  v0,0x800b
8011637c  lbu  v0,-13593(v0)       ; = 0x800ACAE7 = Spieler+0x93 (hit_react)
80116384  bne  v0,zero,0x801163b0  ; hit_react != 0 -> KEIN Stagger, aber Schaden trotzdem
80116388  ori  v0,zero,0x2
8011638c  lui  at,0x800b
80116390  sb   v0,-13736(at)       ; 0x800ACA58 = cmd := 2            <<< MAILBOX
80116394  jal  0x8001a780          ; a0 = SPIELER (nur EIN Argument!)
80116398  addu a0,s0,zero
8011639c  addiu v0,v0,2
801163a0  lui  at,0x800b
801163a4  sb   v0,-13735(at)       ; 0x800ACA59 = Sub-Kommando := ret+2  -> [2] oder [3]
801163a8  lui  at,0x800b
801163ac  sh   zero,-13734(at)     ; 0x800ACA5A = Phase := 0
801163b0  lui  v0,0x800b
801163b4  lh   v0,-13586(v0)       ; 0x800ACAEE = Spieler+0x9A = HP
801163c0  slti v0,v0,4
801163c4  bne  v0,zero,0x80116408  ; HP < 4 -> kein Schaden (Feuer toetet NICHT)
801163c8  addiu v0,v1,-2
801163d0  sh   v0,-13586(at)       ; HP -= 2                          <<< pro Ueberlapp-Frame
```

**Merkmale:** Schadens-Tick = **jedes** Ueberlapp-Bild, **−2 HP**, Untergrenze HP ≥ 4,
Stagger-Anforderung nur bei `+0x93 == 0`, Wiederholung sobald der NORMAL-Prolog
`sb zero,DAT_800acae7` @0x80031964 den Latch wieder freigibt.

### 1.2 Der Flinch-Handler — welche Bank er wirklich benutzt

Sub-Tabelle @0x800741a8 (cmd 2, indiziert mit `DAT_800aca59`):
`[0]/[1]` ≈ 0x80035bd0 (Clip 0xa) · `[2]` = 0x80035de0 (Clip 8) · `[3]` = 0x80035f64 (Clip 9)
· `[4]` = 0x800360e8 (Knockdown vorn) · `[5]` = 0x8003644c (Knockdown hinten).

Handler `[2]` @0x80035de0, Phase 0 + Phase 1 (PSX.EXE, selbst disassembliert):

```
80035e38  ori  v0,zero,0x8
80035e40  sb   v0,-13592(at)       ; 0x800ACAE8 = Spieler+0x94 = CLIP 8
80035e44  ori  v0,zero,0xc8        ; DAT_800acae0 = 200 Rueckstoss
80035e64  sb   zero,-13597(at)     ; 0x800ACAE3 = +0x8F = 0  (HARTER Pose-Snap)
80035e6c  ori  v1,zero,0x32        ; Decay 50/Frame
80035e74  ori  v0,v0,0x1
80035e7c  sb   v0,-13593(at)       ; +0x93 |= 1
80035e80  jal  0x80045024          ; Se_on CORE 1  (0x0401_0001)
...
80035ea0  addu a2,zero,zero        ; a2 = 0 = VORWAERTS
80035ea4  lui  a0,0x800b
80035ea8  lw   a0,-13608(a0)       ; a0 = DAT_800ACAD8  = PL00.EMR   <<< COMMON-BANK
80035eac  lui  a1,0x800b
80035eb0  lw   a1,-13376(a1)       ; a1 = DAT_800ACBC0  = PL00.EDD   <<< COMMON-BANK
80035eb4  jal  0x8001f314          ; anim_set
80035eb8  ori  a3,zero,0x200
```

Handler `[3]` @0x80035f64 ist strukturgleich: Clip 9 @0x80035fbc-c4, SE CORE 2 @0x80036004,
und **dieselbe Bank** `lw a0,DAT_800acad8` @0x8003602c / `lw a1,DAT_800acbc0` @0x80036034.

### 1.3 Die COMMON-Bank kann ein Raum-RBJ NICHT umbiegen

Vollstaendiger EXE-Scan (eigener Wort-Scan ueber `PSX.EXE` t_addr=0x80010000,
t_size=0xAF000, Opcodes `lw`/`sw` mit imm 0xCAD8 bzw. 0xCBC0):

| Zeiger | Schreiber | Leser |
|---|---|---|
| `DAT_800ACAD8` (PL00.EMR) | **2**: `sw v0,-13608(at)` @**0x80031578**, @0x800317dc | 29 |
| `DAT_800ACBC0` (PL00.EDD) | **2**: `sw v0,-13376(at)` @**0x8003154c**, @0x800317b0 | 29 |

Beide Schreiber liegen im Spieler-Loader `FUN_800314b0` und laden das **PLD-Directory**
(PL00.EDD / PL00.EMR). Der RBJ-Binder `FUN_8001b3f8` repointet nur Entity-Felder
(+0x16c/+0x170/…), **nie** diese beiden Globals. ⇒ Clip 8/9/0xa sind im Original **immer**
PL00-Clips, in jedem Raum.

**Die 29 COMMON-Bank-Posier-Stellen, Reaktions-Familie** (Handler-Grenzen aus der
cmd-Tabelle @0x80073f90 = `[2]`=0x80035af0, `[3]`=0x800366bc, `[5]`=0x80036834,
`[6]`=0x800368c0, `[7]`=0x8003694c):

| Adresse (a0/a1-Paar) | Handler | Clips |
|---|---|---|
| 0x80035c28 / 0x80035c30 | cmd-2 `[0]` | 0x0a |
| 0x80035d60 / 0x80035d68 | cmd-2 `[1]` | 0x0a |
| **0x80035ea8 / 0x80035eb0** | **cmd-2 `[2]`** | **8** |
| **0x8003602c / 0x80036034** | **cmd-2 `[3]`** | **9** |
| 0x800361b8, 0x80036284, 0x800363d0 | cmd-2 `[4]` Knockdown vorn | 0x0b–0x10 |
| 0x80036518, 0x80036640 | cmd-2 `[5]` Knockdown hinten | 0x0b–0x10 |
| 0x800367d0 | **cmd-3 TOD** (0x800366bc) | Todes-Clips |
| 0x800358bc/0x80035960/0x80035a2c/0x80035a80 | Kisten-Schieben | 0x11/0x12 — *im Port bereits gefixt* |
| 0x80031134 / 0x800312a4 | Plc_dest 7/8 Rueckwaerts | 0 — *im Port bereits gefixt* |
| 0x80038260 … 0x80039014 | Klettern/Treppe | *im Port bereits gefixt* |

---

## 2. Was der PORT tut — GEMESSEN, echter Durchlauf, echte EXE

### 2.1 Messmethode

`re15_pc.exe` (mingw64-Build von heute, `local_build.sh build` = OK), **kein**
`RE15_AUTOSHOT`, **kein** `RE15_SOFTWARE_RENDER`. Eintritt in ROOM1090 ueber den
**echten Tuerpfad** (`RE15_GOTO_ROOM=1090` → Tuerspawn `(-10100,-1800,4200)` yaw 3072 cut 3
aus `re15_room_spawns[]`, identisch zum ROOM1050-Tuereintritt). Der Spieler-Clip wurde mit
`RE15_FORCE_MOTION=8` auf den Flinch-Clip gepinnt und der **aufgeloeste Render-Zustand**
mit `RE15_ANIM_TRACE` mitgeschrieben (`re15_compute_actor_kf` — die einzige Stelle, an der
im Port ueberhaupt eine Pose aufgeloest wird). Kontroll-Lauf identisch in **ROOM1140**.

### 2.2 Ergebnis

Boot-Log (`debug.log`):

```
[rbj] room 1090 cinematic overlay: 25 clips, 284 kf
[rbj] room 1140 has no RBJ (RBJ/ROOM1140.RBJ) — Leon auf PL00-Basis zurueckgesetzt
```

Anim-Trace (`frame actor typ motion clip_idx fc_render cur slot fc_clock loco rev`):

```
ROOM1090:   1704 0 0  8  8   1  0 0 0 0 0      <<< Cliplaenge 1
ROOM1140:    381 0 0  8  8  22  0 0 0 0 0      <<< Cliplaenge 22 (korrekt)
```

**Der Flinch-Clip ist in ROOM1090 ein Ein-Bild-Clip.** `re15_compute_actor_kf` faellt damit
in den HOLD-LAST-Zweig, `slot = 0`, und posiert 22 Bilder lang **denselben** Keyframe.

### 2.3 Die Datenlage dazu (direkt aus den Assets geparst)

`ROOM1090.RDT` @0x5C = 0x298C, RBJ-Container `total_length=0xCBC8`, `record_count=2`,
Record 0: `emr_prefix=0x8`, `edd_off=0x58D4`, EMR: 15 Knochen, ksize 80, **284 Keyframes**.

| Clip | PL00.EDD (Soll) | ROOM1090 RBJ rec0 (Ist im Port) |
|---|---|---|
| 0x08 (Flinch vorn) | **fc = 22** | **fc = 1** → Keyframe 8 |
| 0x09 (Flinch hinten) | **fc = 22** | **fc = 1** → Keyframe 9 |
| 0x0a (Flinch ohne Angreifer) | fc = 20 | fc = 1 → Keyframe 10 |
| 0x0b … 0x0e (Knockdown) | 25/50/10/30 | fc = 1 (Keyframe 11…14) |
| 0x0f / 0x10 | 24 / 16 | 20 / 30 |
| 0x11 / 0x12 (Schieben) | 10 / 25 | 30 / 20 |

PL00.EDD hat 24 Clips, die Raum-Bank 25 — die Tabellen sind komplett verschieden belegt.
Die Clips 0x00–0x0E der Raum-Bank sind **Ein-Bild-Standposen** der ROOM1090-Cutscene.

### 2.4 Der Code-Ort

`re15_port/engine/src/anim_select_common.c`, `re15_actor_anim_select()`. Es gibt
Sentinel-Zweige fuer 105/100 (Laufen), 200/210/211/212 (Idle), 213/214 (verletztes Idle),
220/221 (Treppe), 230–235 (Klettern), 236 (Rueckwaerts) und einen Sonderzweig fuer den
Schiebe-Substate (`re15_player_push_substate()` → PL00 + `clip_override = motion`).

**Fuer motion 8/9/0x0a und 0x0b–0x10 existiert KEIN Zweig.** Sie fallen auf
`out->skel/anim = banks->def_skel/def_anim` mit `clip_override = -1` durch — und
`def_skel/def_anim` sind in ROOM1090 von `re15_apply_room_cinematic()`
(`enemy_common.c:141`, Aufruf `platform/pc/main.c:2949` beim Boot / `:5320` beim
Raumwechsel) mit der RBJ-Record-0-Bank **ueberschrieben** worden.

Die Kommentare am Schiebe-Zweig dokumentieren exakt dieselbe Fehlerklasse — sie wurde
2026-08 fuer das Kistenschieben in ROOM1090 gefunden und gefixt, fuer die
Treffer-Reaktionen aber nie.

### 2.5 Warum die alte Sonde "OK" gemeldet hat

`re15_port/tests/unit/probe_1090_flame_touch.c` misst:

```
hp 100 -> 98 (erster Schaden f5), Flinch-Clip f5 (mo=9), CORE-SE +1 (last=2)
probe_1090_flame_touch: OK
```

Ihre Zusicherung lautet `pl->motion == 8 || pl->motion == 9` — ein **Zustandswert**.
Der Zustandswert war und ist richtig. Die Sonde ruft `re15_actor_anim_select` /
`re15_compute_actor_kf` **nie** auf und kann darum die gerenderte Pose per Konstruktion
nicht sehen. Genau die Klasse "Sonde luegt" aus `lesson.txt` — vergleiche den
identischen Vorfall beim NPC-Gleiten (`reai-v2-npc-crossfade-decay`: "nur State-Werte
statt der GERENDERTEN Pose verglichen").

---

## 3. Warum es dem Nutzer nur beim Feuer auffaellt

Zensus ueber alle STAGE1-RDTs (`RDT@0x5C` → RBJ Record 0, Clip 8/9/0x0a Framecounts):

| Raum | Raum-Bank | clip 8 / 9 / 0x0a | Wirkung auf den Flinch |
|---|---|---|---|
| **ROOM1090** | 25 Clips | **1 / 1 / 1** | **eingefrorene Standpose** ← Nutzer-Fall |
| ROOM1050, ROOM1141, ROOM1170, ROOM11B0, ROOM11C0 | 25–26 Clips | 1 / 1 / 1 | eingefrorene Standpose |
| ROOM1040 / ROOM1041 | 5 Clips | – | `clip_idx % 5` → Clip 3/4/0, voellig fremde Pose |
| ROOM1171 | 2 Clips | – | `% 2` → Clip 0 |
| ROOM1011/1021/1031/10B1/1211 | 10–13 Clips | 18 / 24 / 20 | falsche Geste, laeuft aber |
| ROOM10D0 | 10 Clips | 24 / 20 / – | falsche Geste |
| ROOM1150 / ROOM1151 / ROOM11B1 | 15–20 Clips | 24/20/20 bzw. 40/20/25 | falsche Geste |
| ROOM1140, 1190, 1230, 10C0, 11C1, … (48 von 78) | **kein Anim-Block** | PL00 | **korrekt** |

Die Kampfraeume, in denen bisher verifiziert wurde — ROOM1140 (Zombies), ROOM1190/1230
(Hunde), ROOM10C0 (Kraehen) — haben **keinen** RDT-Animationsblock. Dort war der Flinch
immer richtig; deshalb ist die Divergenz durch alle Gegner-Kampagnen gerutscht.
ROOM1090 (Feuer) und ROOM11C0 (Gorilla-Boss) sind die Raeume, die beides haben:
Raum-Cinematic-Bank **und** regelmaessigen Spieler-Schaden.

**Konsequenz ueber den Flinch hinaus:** in denselben Raeumen sind auch der
**Knockdown** (Clips 0x0b–0x10, Handler `[4]`/`[5]`) und die **Todes-Animation**
(cmd-3 @0x800366bc, COMMON-Leser @0x800367d0) betroffen. In ROOM11C0 trifft das genau
den Gorilla-Heavy-Biss, der als einziger STAGE1-Ausloeser den Knockdown scharfschaltet.

---

## 4. Die Ursache in einem Satz

> Der Port waehlt die Spieler-Pose-Bank nach *motion-Sentinel*; fuer die
> Treffer-/Knockdown-/Todes-Clips gibt es keinen Sentinel, also nimmt er die
> Default-Bank — und die ist in Raeumen mit RDT-Animationsblock die Raum-Cinematic-Bank
> statt der COMMON-Bank PL00, obwohl das Original diese Clips ausnahmslos aus
> `DAT_800ACAD8`/`DAT_800ACBC0` = PL00.EMR/PL00.EDD posiert
> (@0x80035ea8/0x80035eb0 bzw. @0x8003602c/0x80036034; die beiden Zeiger haben
> EXE-weit nur die Schreiber @0x80031578 / @0x8003154c im Spieler-Loader).

Was **nicht** die Ursache ist (jeweils ueberprueft und ausgeschlossen):
* kein Aim-Gate (das Original hat keins, der Port hat es 2026-08 entfernt),
* keine `+0x93`-Ein-Treffer-Klemme (der NORMAL-Prolog raeumt sie, `game_step_common.c:930` = @0x80031964),
* keine Sentinel-Kollision (236 ist der naechste belegte Wert, 8/9/0x0a sind frei),
* kein Flavor-Einfluss (die Render-Bank haengt nicht am KI-Flavor — deckt sich mit "beide Flavors"),
* kein Prioritaets-Konflikt mit anderen Kommandos (cmd 2 gewinnt im Port wie im Original).

---

## 5. Was die Umsetzung braucht (KEINE Aenderung in dieser Phase)

1. **Ein PL00-Zweig fuer die Reaktions-Clips** in `re15_actor_anim_select()` — dieselbe
   Form wie der bereits vorhandene Schiebe-Zweig: `skel/anim = banks->pl00_*`,
   `clip_override = motion` (im Original ist `+0x94` der DIREKTE Clipindex, kein
   Motion→Clip-Mapping — `FUN_8001f314` indiziert ungeprueft `motion*4`).
   Abdeckung nach Beleglage: **Flinch 8/9/0x0a** (@0x80035ea8/0x8003602c/0x80035c28/0x80035d60),
   **Knockdown 0x0b–0x10** (@0x800361b8/0x80036284/0x800363d0/0x80036518/0x80036640),
   **Tod** (@0x800367d0). Das Gate darf **nicht** auf der Clip-Nummer allein stehen (ein
   Cutscene-`Plc_motion 8` waere sonst betroffen), sondern auf dem Spieler-Kommando —
   der Port hat dafuer `s_hit_flinch`/`s_knockdown`/`re15_player_is_dead()` bzw. das
   Kommandowort `pl->state`.
2. **Regressionstest, der die POSE misst, nicht den Zustand.** Der bestehende
   `probe_1090_flame_touch` reicht nicht. Der Test muss durch
   `re15_actor_anim_select` + `re15_compute_actor_kf` gehen (Muster:
   `probe_1090_push_anim.c`, der genau das fuer das Schieben tut) und pruefen:
   `fc_render == 22` fuer Clip 8/9 in ROOM1090, `Bank == PL00`, `fc_render == fc_soll`.
3. **Kein separater Feuer-Fix.** Der Feuer-Pfad (`enemy_ai_common.c` Spider/Feuer-Tail,
   `re15_player_stagger_cmd2`) ist gegen 0x80116288 geprueft und byte-true; er bleibt unberuehrt.

---

## 6. Ehrlicher Rest / offen

* **Nicht abgeschlossen:** der physische Hineinlauf in die Flamme in der echten EXE.
  Der Hof hinter dem Rolltor ist im automatisierten Harness nicht erreichbar — der
  Autopilot bleibt reproduzierbar bei x ≈ −9400 stehen (`[auto] fest bei (-9397,4268)`),
  das Tor ist zu. `RE15_FORCE_EVENT=6` (sub06, `Aot_on 3` = Selbst-Tuer nach
  `(1252,-1800,-2529)` mitten in den Hof) feuert zwar (`[force-event] scd_event_fire(6)
  at F400`), versetzt den Spieler aber nicht — er bleibt im Skript-Modus stehen. Deshalb
  ist die Kontakt-Haelfte auf der echten `ROOM1090.RDT` im echten `re15_game_step`
  gemessen (`probe_1090_flame_touch`: `hp 100→98 ab f5`, `mo=9`, CORE-SE 2), die
  Render-Haelfte in der echten EXE im echten Raum (`fc_render = 1`). Beide Haelften sind
  belegt, aber nicht in EINEM Lauf. Fuer die Abnahme des Fixes sollte der Verifizierer
  ueber den **echten Torweg** (oder einen Savestate im Hof) gehen.
* **Nicht untersucht:** ob die Raum-Cinematic-Bank in ROOM1090 ueberhaupt auf Leons
  Default-Bank gehoeren darf. Das Original repointet fuer Cutscenes das **Entity-Feld**
  (+0x16c/+0x170), nicht die COMMON-Globals — der Port modelliert das als globalen
  Default-Bank-Tausch. Der Ein-Zweig-Fix oben behebt das Symptom byte-true; die
  strukturelle Frage (Entity-Kanal statt Default-Tausch) bleibt offen und ist die
  eigentliche Wurzel dieser ganzen Fehlerfamilie.
* **Nicht geprueft:** STAGE2–6. Der Zensus oben deckt nur STAGE1 ab.

---

## 7. Reproduktion (Copy-Paste)

```bash
cd c:/workspace/git/reAi_v2/re15_port/build/platform/pc
taskkill //F //IM re15_pc.exe 2>/dev/null

# FEHLERFALL — ROOM1090 (Raum-Bank)
RE15_NO_INTRO=1 RE15_FORCE_MOTION=8 RE15_GOTO_ROOM=1090 \
RE15_ANIM_TRACE="C:/temp/at_1090.txt" \
RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="W2,A1,W3,A1,W3,A1,W3,A1,W40" \
timeout 140 ./re15_pc.exe >/dev/null 2>&1
awk '$2==0' C:/temp/at_1090.txt | tail -3      # Spalte 6 = fc_render -> 1   (FALSCH)

# KONTROLLE — ROOM1140 (keine Raum-Bank)
RE15_NO_INTRO=1 RE15_FORCE_MOTION=8 RE15_GOTO_ROOM=1140 \
RE15_ANIM_TRACE="C:/temp/at_1140.txt" \
RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="W2,A1,W3,A1,W3,A1,W3,A1,W40" \
timeout 140 ./re15_pc.exe >/dev/null 2>&1
awk '$2==0' C:/temp/at_1140.txt | tail -3      # Spalte 6 = fc_render -> 22  (RICHTIG)

taskkill //F //IM re15_pc.exe 2>/dev/null
```

Spalten der Anim-Trace: `frame actor typ motion clip_idx fc_render cur slot fc_clock loco rev`.
