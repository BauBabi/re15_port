# NPC-Verhaltens-VM Welle 2 (t42/t45/t49/t4b)

All RE questions are answered. Final deliverable:

---

# P4 „NPC-Dialog-VM Welle 2" — RE-Dossier-Rohdaten (STAGE1-NPC-Typen 0x42/0x45/0x49/0x4b)

## 1) MECHANISMUS (vollständig, disasm-belegt)

### 1.1 Registrierung und Typ→Root-Zuordnung

`FUN_8011e864` (STAGE1_full/FUN_8011e864.c, Overlay-Setup) schreibt in die Entity-Dispatch-Tabelle `@0x80072bac + type*4`:

| Typ | Slot | Root | State-Tabelle (auf `+0x4`) |
|---|---|---|---|
| 0x40 Irons | `@0x80072cac` | FUN_8011c5a0 | @0x80121598 (52-Wort-Blob) |
| 0x42 | `@0x80072cb4` | FUN_8011cb70 | @0x80121668 (52) |
| 0x45 Sherry | `@0x80072cc0` | FUN_8011d140 | @0x80121738 (26) |
| 0x47 Annette/Elliot | `@0x80072cc8` | FUN_8011d6d4 | @0x801217a0 (26) |
| 0x49 | `@0x80072cd0` | FUN_8011dc68 | @0x80121808 (52) |
| 0x4b Katherine | `@0x80072cd8` | FUN_8011e22c | @0x801218d8 (45) |

Blob-Layout (raw aus STAGE1.BIN, `re15_disasm.py table`): `[0]`=INIT, `[1]`=ACTIVE, `[2]/[3]`=HURT/DEATH-Slots, `[4]`=EXE-Executor 0x80050be8, `[5]`=Verhaltens-VM-Paar-Dispatcher, dann EXE-Leaf-Familien, am Blob-Ende 3 Datenworte (Hitbox) + Selbstzeiger. **t4b-Sonderfall: `[6]`=FUN_8011e6c0 (zweiter Paar-Dispatcher, Escort-Familie), alles danach um 1 verschoben.**

### 1.2 Die sechs Roots — drei Archetypen

Alle Roots: Pause-Gate `(DAT_800aca40&0x20000000)||(+0x9&0x20)`, Look-Aux `0x8001bd60(-10,0x14)`, Nav-Steer `0x80039e7c`, `+0x4`-Dispatch, Shared-Tail (`0x8002b498`/`aec4`/`b544`, SCA `0x8003b0a4(+0x34, hitbox[+6], 4)`, `0x80037358`, SFX `0x8001b38c`), Schatten `0x8001b064(+0xb0, +0x1ba)` — auch bei Freeze.
- **t45 (FUN_8011d140): FOLLOW-PLAYER** — einziger Root, der `0x80039e7c(&DAT_800aca88)` mit der SPIELER-Position füttert und pro Frame `+0x1d0 = SquareRoot0(dx²+dz²)` zum Spieler schreibt.
- **t42/t47/t49 (FUN_8011cb70/d6d4/dc68): PUNKT-STEER** — `0x80039e7c({+0x1dc, 0, +0x1de})`.
- **t4b (FUN_8011e22c)**: wie Punkt-Steer, zusätzlich `sh zero, +0x1c8` jeden Frame (Halbwort-Latch-Clear).

### 1.3 ACTIVE (Main-State 1) — per Typ

- **t45 `FUN_8011d460` / t47 `FUN_8011d9f4` (raw verifiziert @0x8011d470 `lbu v0,9(v0)`, `andi 0xf`, Tabelle @0x8012174c bzw. @0x801217b4 = Blob-Eintrag[5]):** passiver Dispatch `T[5 + (+0x9&0xf)]`, danach `jal 0x80012aa4 / a0=0xbb8` (Elevations-Band-Stempel 3000, @0x8011d49c-a0).
- **t40/t42/t49/t4b: ESCORT-PROBE**, jeden Frame:
  1. `FUN_800509e4(15000, out, ang0, 0)` — ang0: t40=0x5dc, t42/t49=**0x800**, t4b=**0x514**.
  2. `FUN_8005070c(r, a, ±a)` → `+0x1d0` — t40: (0x5dc, a, **+a**), t42: (0x5dc, a, **−a**), t49: (0x5dc, a, **+a**), t4b: Miss (700, a, −a) / Hit (0x578, a, −a).
  3. `+0x9 = 0` (kein Gegner) / `1` (Gegner gefunden), dann Dispatch `T[5+(+0x9&0xf)]`.
- **t4b-Zusatzübergang `FUN_8011e518` (raw @0x8011e5cc-5f8):** `if ((+0x1c2 & 1) && DAT_800acae0 >= 0x65 /*slti 101*/) { +0x4=2; +0x5=4; +0x6=0; }` — `+0x1c2` Bit0 = „Spieler hat mich body-gepusht" (Port: `contact_flags`, enemy_ai_common.c:10931), `DAT_800acae0` = Spieler-Momentangeschwindigkeit (≥101 = rennen). **Katherine flieht, wenn der rennende Spieler sie anrempelt.**

### 1.4 Die EXE-Helfer (PSX.EXE, selbst disassembliert)

**`FUN_800509e4(maxdist, out_s16*, ang0, mode)` — Nächster-lebender-Gegner-Scan:**
`sh a2,0(out)` @0x80050a28 (Default-Winkel bleibt bei Miss stehen!); Scan über Gegner-Slots Basis 0x800acc2c, Stride 0x1f4, Anzahl `lbu @0x800aca4e`; Filter: Flags-Wort Bit0 @0x80050a54, **Typ < 0x40** (`sltiu v0,0x40` @0x80050a78 — NPCs zählen nie), **HP(+0x9a) ≥ 0** (`bltz` @0x80050ac8 — Invulnerable/-1 raus), Distanz `SquareRoot0` @0x80050ab8 < bisheriges Minimum (< maxdist). Treffer, mode==0: `out[0] = atan2_q12(found→player) − DAT_800acabe` @0x80050b4c-68 (acabe = Spieler-Heading-Akku, Port ↔ `pl->rot_y`), `+0x1a8 := &player` @0x80050b7c; mode≠0: `out[0]=atan2(cur→found)`, `+0x1a8 := &found` @0x80050bb0. Return 1/0.

**`FUN_8005070c(r, a1, a2)` — Zwei-Punkte-Wahl:** Punkt A = Spielerpos + RotMatrix(acabe+a1)·(r,0,0) (RotMatrix 0x80068098 @0x80050754, ApplyMatrix 0x80067a28 @0x80050774), Punkt B analog mit a2; schreibt den NÄHEREN (zur NPC-Position) nach `+0x1dc/+0x1de` (@0x80050880-8e8), Return = Distanz dorthin. Da `a = Peilung(Gegner→Spieler) − acabe`, liegt der Punkt bei Treffer auf der gegner-abgewandten Seite des Spielers; bei Miss (a=ang0-Default) schräg hinter dem Spieler.

### 1.5 Die Verhaltens-VM (State-5-Paar-Dispatcher; `+0x5`=Sub, `+0x6`=Phase)

`+0x9=0` → Blob[5] = Doppel-Dispatch `A[+0x5]()` (Logik) + `B[+0x5]()` (Anim/Move): t40 FUN_8011c950 (A@0x801215b0/B@0x801215d0), t42 FUN_8011cf20 (@0x80121680/@0x801216a0), t45 FUN_8011d4b4 (@0x80121750/**@0x8012175c — B überlappt A+3!**), t47 FUN_8011da48 (@0x801217b8/@0x801217c4, ebenso), t49 FUN_8011e00c (@0x80121820/@0x80121840), t4b FUN_8011e644 (@0x801218f4/@0x80121914). **t4b `+0x9=1`** → FUN_8011e6c0 (Escort-Familie A'@0x80121934/B'@0x80121954). **t42/t49 `+0x9=1`** → Blob[6] = **FUN_8004f100 ALLEIN** (nur Logik, KEIN Anim-Leaf → Pose friert ein, solange ein Gegner ≤15000 lebt — Shipped-Byte-Stand, s. Totcode unten).

Leaf-Katalog (PSX.EXE, alle selbst disassembliert; Typtabellen-Index = `type−0x40`, Paare {speed, cone}):

| +0x5 | A-Leaf (Logik) | B-Leaf (Anim/Move) | Semantik |
|---|---|---|---|
| 0 | f100: dist(+0x1d0)≥0x5dd→1; arc(player,0x4b0)→2; aca52&1&&typ==0x4b→6; player.hit_react→6 | f310: Clip 2 (Bank +0x170/+0x174), +0x8f=7 | IDLE |
| 0' (Escort) | f204: **dist≥0xc9→1**; arc 0x4b0→2; aca52-Check; KEIN hit_react | f310 | IDLE eng am Punkt |
| 1 | f3a4: dist<0x1f4→{3,+0x6=1}; arc(steer,0x400)→{3,1}; dist≥0xbb9→{5,0}; 52/hit→6 | f4e0: Clip 5, Speed=tbl@**0x80076c00**, Steer `0x8001aac4(+0x1bc/+0x1be, cone tbl@0x80076c01)`, Walker `0x800245d8(0)` | WALK zum Steer-Ziel |
| 2 | f5e8: arc(player,0x40)==0→0; dist≥0x7d1→{1,1}; 52/hit→6 | f6f0: Clip 5, `+0x6a += arc(player, cone tbl@0x80076c41)`, KEIN Walker | TURN zum Spieler |
| 3 | f7dc: arc(+0x1dc/de,0x200)==0 && dist≥0x3e9→{1,1}; dist<**0x64 (nur 0x4b)** bzw. <**0x12c (sonst)**→{4,1}; 52/hit→6 | f9ec: Clip 5, Speed=**tbl/2** (`srl 1` @0x8004fa84), Steer, Footstep-SE `0x80045630(0, 7−3·bit12)` | APPROACH (halbe Geschw.) |
| 4 | fb3c: `0x8001aa68(acabe, cone41)==0`→0; 52/hit→6 | fc2c: Clip 5, Slew auf SPIELER-HEADING (`aa68(acabe, cone·2)`), bei 0 Snap `+0x6a := acabe` @0x8004fcf8-d08 | ALIGN mit Blickrichtung des Spielers |
| 4' (Escort) | 50198: arc(player,cone41)==0→0; 52/hit→6 | 50290: Clip 5, Slew ZUM SPIELER (`aac4(playerX/Z, cone·2)`), Footstep-SE | FACE den Spieler |
| 5 | fd3c: dist<0x3e8→{3,0}; arc(steer,0x400)→{3,0}; 52/hit→6 | fe44: **Clip 0**, Speed=tbl@**0x80076c80** (200/210/120!), Steer cone tbl@0x80076c81, SE(a0=1) | AUFHOL-RENNEN |
| 6 | ff90: `if(aca52&1) +0x9e=0x78` (Timer-Refresh) | ffc0: 6-Phasen-Geste via Sprungtabelle **@0x800112bc**: P0/P1 Clip 6 einmal; P2/P3 Clip 7 halten, `+0x9e`−− (120 Frames); P4/P5 Clip 6, dann `aca52&1`? → P0-Loop : `+0x5=0` | ALARM-Geste („Spieler liegt am Boden") |
| 7 | 503a8: `player.hit_react==0` → {0,0} | 503e4: Clip 7→(`+0x8f=7`), Speed=tbl@**0x80076c60**, `+0x6a += arc(player, tbl@0x80076c61)`, anim auf **LOCO-Bank +0x84/+0x16c**, Walker `0x800245d8(0x800)` | REAKTION solange Spieler getroffen wird |

`DAT_800aca52` Bit0 = Spieler-am-Boden-Latch (bereits RE'd/portiert: enemy_ai_common.c:4657-4664, `g_aca52_flags`).

**Typ-Parametertabellen (raw bytes @0x80076c00..0x80076c9f):** Walk-Speed t40/42=0x4b(75), t45/47=0x46(70), t49=0x4b, **t4b=0x32(50)**; Walk-Cone überall 0x30. Turn-Cone @0x80076c41: 0x60, für 0x4b/0x4c/0x4f 0x50. Sub-7-Speed @0x80076c60: 0x3c/0x37/**0x1e(t4b)**, Cone 0x30/0x28. Sub-5-Speed @0x80076c80: **0xc8(200)**/0xd2(210)/**0x78(120, t4b)**, Cone 0x48/0x40. (@0x80076ca0 beginnt die Executor-Tabelle 0x80050cb8… — bereits portiert.)

### 1.6 Sonderzustände

- **t4b Main-State 2 (= HURT-Slot, umgenutzt): Tabelle @0x80121974 = {NULL,NULL,NULL,NULL, 0x80050550} auf `+0x5`** — nur Sub 4 existiert. **`FUN_80050550` = Katherine-Flucht:** P0: Clip 0, `+0x8f=7`, Speed `+0x8c=0x12c` (300), Fluchtkurs `+0x9c = atan2(me→player) − 0x800` @0x80050600-630; P1: Speed −30/Frame (clamp 0) @0x80050648-6c, anim(LOCO +0x84/+0x16c), `+0x6 += anim-ret`, Walker `0x800245d8(+0x9c − +0x6a)` @0x800506c8; P2 @0x800506d8: `+0x4=1, +0x5=0, +0x6=0` → zurück in ACTIVE.
- **Main-States [2]/[3] der übrigen Typen** (HURT/DEATH, HP=−1 ⇒ tot): Executor-Leaf-Dispatch auf **`+0x6`** (t40 @0x80121630, t42 @0x80121700, t45 @0x80121768, t47 @0x801217d0, t49 @0x801218a0, t4b[3] @0x80121988).
- **INITs**: alle 6 byte-identisch bis auf den `+0x78`-Hitbox-Zeiger: t40 @0x80121588 {0,−1530,0,**450**,1530,450}, t42 @0x80121658 {450}, t45 @0x80121728 {0,−1440,0,**500**,1440,500}, t47 @0x80121790 {450}, t49 @0x801217f8 {450}, t4b @0x801218c8 {0,−1440,0,**300**,1440,300}.
- **⛔ TOTCODE (byte-belegt): `FUN_8011cf9c` (t42) und `FUN_8011e088` (t49)** = die Escort-Familien-Paar-Dispatcher (A@0x801216c0/B@0x801216e0 bzw. @0x80121860/@0x80121880). **NULL Referenzen in ganz STAGE1.BIN** (Scan: kein gespeicherter Pointer, kein jal/j, kein addiu/ori-Immediate). Die shipped Tabellen routen `+0x9=1` stattdessen auf f100 allein; nur t4b bekam den Dispatcher (als Tabelleneintrag [6]). Die t42/t49-Blob-Einträge [22..37] (f204-Familie) sind damit toter Datenballast.

### 1.7 ERREICHBARKEITS-ZENSUS (alle 80 STAGE1-RDTs, Walker = aot_sce_census-Methode inkl. beider Sektionsende-Fixes)

NPC-`Sce_em_set` (0x44; `pc[3]`=behavior→`+0x9`, byte-true @LAB_800420a0):

| Raum (Leon) | Typ | behavior | Folge |
|---|---|---|---|
| 1050 sub00, 1090 sub00, 11B0, 11C0×2, 1260×2 | 0x42 | **0x40** | state 4 Executor Sub 6 |
| 10D0, 11A0, 11B0, 1260×2 | 0x40 | 0x40 | state 4 |
| **1150 main00** | **0x45** | **0x00** | **Main-State 1 ACTIVE — LIVE im Leon-Spiel** |
| 1170 sub15 | 0x47 | 0x40 | state 4 (Elliot, portiert) |
| 11B0 | 0x4b | 0x40 | state 4 |
| Elza: 1011/1021(0x47,0x49)/10B1(0x49)/1141(0x49)/11A1(0x49,0x4b)/11B1(0x40/42/4b)/1211/1261(0x49) | | alle 0x40 außer **1171 sub07: 0x49 behavior 0x00** und 1151: 0x45 behavior 0x00 | **t49-ACTIVE live NUR in Elza-ROOM1171** |

- **Kein einziges `Member_set(m=8)`** (=`+0x4` State, Feld-Map FUN_8004116c, Port actor_common.c:136-165) auf NPCs in ganz STAGE1 → Main-States 2/3/5/6/7 sind SCD-unerreichbar; `m=12`(→`+0x9`)-Writes existieren nur in Elza-Räumen auf Zombie-Slots (Freeze-Bits 0x30/0x10).
- NPC-Steuerung im State 4 läuft komplett über `Plc_motion(0x3F)/Plc_dest(0x40)/Plc_neck(0x41)` nach `Work_set(2,idx)` (1011/1050/1090/1150/1170/1260…) — portiert.
- **Konsequenz:** Shipped-erreichbar von den 4 Audit-States sind: alle 6 per-Typ-INITs (jeder Spawn), **t45-ACTIVE (ROOM1150 Leon**, vom Spawn bis zum ersten `Plc_motion`, das state 4 setzt**)**, **t49-ACTIVE (ROOM1171, nur Elza)**. t40/t42-ACTIVE, t4b-ACTIVE+Flucht: in STAGE1 dormant (behavior immer 0x40) — Erreichbarkeit in STAGE2-6 ungeprüft.

## 2) PORT-STAND (Datei:Zeile)

- Routing aller 7 Typen → `re15_npc_ai_tick`: enemy_ai_common.c:11068-11083; Tick selbst :8376; INIT (case 0) :8452-8469 byte-true inkl. `+0x9&0x40`-Gate; **State 1 = Idle-STUB :8478-8482 („hold the idle pose") — die eigentliche Lücke**; State 4 Executor :8349-8374 + Sub-VM :8079-8346 (Subs 0-9 komplett, inkl. Tabellen 0x80076c00/01/21/41 als `s_npc_*_param`/`s_npc_turn_cone` :8060-8078).
- f100/f3a4/f5e8-Logik bereits als `re15_ai_exe_assess/search/turn` portiert (:222-268, inkl. aca52/0x4b-Checks), aber unter Zombie-„System A"-Etikett und OHNE die B-Anim-Geschwister (:288 „DEFERRED").
- Bausteine vorhanden: `re15_nav_update_steer` (=0x80039e7c, nav_zone_common.c:195), `re15_ai_arc_test` (ab9c), aa68-Slew (:2341), aac4-Slew (actor_locomotion.c:154), Walker 245d8 (`re15_dog_advance`/`re15_dog_advance_ofs`), `FUN_80012aa4` (re15_damage.c:1075), `g_aca52_flags` (:4657), `contact_flags`+0x1c2 (:10931), Spieler-Speed, `re15_squareroot0`, Footstep-SE-API (Blocker −8-Shift für Typ≥0x40 dokumentiert :8283-8291, audio_pc.c:2448).
- `op_plc_motion`/`op_plc_dest` setzen byte-true state 4 (scd_vm.c; Kommentar :8430-8446) — nach der ersten Cutscene bleibt auch das Original in state 4.
- Savestates mit laufenden NPCs vorhanden: `mzd_stage1_npc.sav` (0x40/0x42/0x4b), `orig_1170_gp.sav` (0x47) (:8060).
- „Welle 2"-Marker präzisiert: :7956 = der eigentliche Deferral („dialogue behaviour VM + per-NPC overlay states"); die Audit-Zeilen 7733/10857 sind Zeilendrift (Gorilla-Sub-Fold bzw. SE-Spiegel-Kommentar, nicht NPC).

## 3) IMPLEMENTIERUNGS-PLAN (priorisiert: erreichbar+sichtbar zuerst)

1. **t45/t47-ACTIVE (M, ROOM1150 Leon LIVE):** in `re15_npc_ai_tick` case 1: (a) Root-Zusatz t45 = `re15_nav_update_steer(e, player)` + `+0x1d0`-Dist pro Frame (t47: Steer auf `+0x1dc/de`); (b) passiver `+0x9&0xf`-Dispatch: Sub 0 = Paar-VM, Subs 1-3 = f100/f3a4/f5e8 direkt; (c) Paar-VM Subs 0-2 aus vorhandenen `re15_ai_exe_*` + NEU drei B-Leaves f310/f4e0/f6f0 (je ~15 Zeilen, nutzen `re15_npc_clip/anim`, `s_npc_*_param`, `re15_enemy_steer_point`, `re15_dog_advance`); (d) `FUN_80012aa4(3000)`-Stempel; (e) die byte-true Überlapp-Kombos `+0x5=3/5` der 26er-Tabelle (f310-als-Logik + Executor-Leaf als Anim) exakt nachbilden, nicht „reparieren". Verifikation: re15-room-probe ROOM1150 (Spawn→`+0x4==1`, Follow bei dist≥0x5dd, Idle <0x1f4) + DuckStation-Savestate-Serie `+0x4/+0x5/+0x94` beim 1150-Eintritt; ctest.
2. **Volle 8-Sub-Familie (M):** A3-A7/B3-B7 (f7dc/f9ec, fb3c/fc2c, fd3c/fe44, ff90/ffc0, 503a8/503e4) mit den Konstanten aus §1.5 + Tabellen 0x80076c60/80. Sichtbar: Alarm-Geste (Sub 6/7) bei Spieler-Knockdown für jeden state-1-NPC; t49-ACTIVE für Elza-1171 (Kampagne Elza 0/40 kommt).
3. **Escort-ACTIVE t40/t42/t49 (M):** 0x800509e4-Scan (über `g_actors`: `active`, `type<0x40`, `hp>=0`, dist<15000) + 0x8005070c-Punktwahl als zwei C-Helfer; Typ-Parameter (ang0/r/Vorzeichen) exakt wie §1.3; t42/t49-`+0x9=1`-Pfad = f100 ALLEIN (Shipped-Quirk, Totcode dokumentieren).
4. **t4b-Spezials (S):** Probe 0x514/700/0x578; Flucht-Transition (`contact_flags&1 && player_speed>=0x65`) + `FUN_80050550`-Zustand; Escort-Paar-Familie (f204, 50198/50290); Root-`+0x1c8`-Clear. In STAGE1 dormant → Test contrived (Room-Probe mit forciertem state 1).
5. **Main-States 2/3 (+0x6-Dispatch) + 5/6/7 (S, SCD-unerreichbar):** nur der Vollständigkeit; vorher STAGE2-6-Zensus, ob dort NPC-Spawns mit behavior ≠0x40/0x00 oder `m=8`-Writes existieren (0x4d-STAGE6-Root @0x801017a0, Tabelle @0x80102794 — Struktur laut Port identisch, Konstanten ungeprüft).

## 4) OFFEN (ehrlich)

- **Wie lange t45-ACTIVE in ROOM1150 sichtbar läuft** (wann das erste `Plc_motion` aus sub02/03 feuert) — statisch nicht entscheidbar; nächster Schritt: DuckStation-1150-Savestate, `+0x4` lesen (`mzd_stage1_npc.sav`-Familie existiert schon).
- **`+0x5=3`-Endzustand bei t45/t47** (statisch: kein Leaf schreibt `+0x5` mehr → Follow endet dauerhaft nach erster Annäherung <500): nur statisch hergeleitet; per Savestate verifizieren, bevor der Port-Kommentar es als Fakt trägt.
- **Sce_em_set pc[4]/pc[5]/pc[6]** (Werte 1/4/5 im Zensus) — Konsument im Spawner @0x800420a0 nicht nachgezogen.
- **`0x8004f94c`** (Sub-3-Variante ohne Typ-Checks, direkt hinter f7dc) — Tabellen-Referenz nicht lokalisiert (vermutlich Zombie-Familie); nächster Schritt: Pointer-Scan über 0x801215xx-0x80121fxx.
- **Walker-Offset 0x800 in 503e4** (180°-Versatz mit Clip 7 auf Loco): Semantik nur aus Bytes; visuelle Wirkung (rückwärts weichen?) unverifiziert.
- **`FUN_8001bd60(-10,0x14)`** Look-Aux: weiterhin port-weit unmodelliert (bestehender Zustand).
- **Katalog-Hygiene:** RE15_FUN_CATALOG.md:80 und enemy_ai_common.c:208-217 etikettieren PTR_801217a0/FUN_8011d9f4 als „Zombie-FSM"; laut Registrierung @0x8011e864 ist das die **t47-NPC**-Tabelle (Typ 0x10 → LAB_80100424). Welche Typen die Tabelle LIVE durchlaufen, wurde hier nicht re-gemessen — nur Doku-Flag, kein Code-Change auf dieser Basis.
- Zensus-Näherung: „current work" beim SCD-Walk ist linear (nicht ausgeführt) rekonstruiert — für die m=8-Negativ-Aussage unkritisch (0 Treffer in ALLEN Ops), für Zuordnung einzelner Plc-Ops zu Slots ggf. per SCD-Disasm des Einzelraums gegenprüfen.

Scratch: `C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\e15d7177-43f5-413e-8f99-d5e5f369c973\scratchpad\npc_census.py` (+ `npc_census_out.txt`, vollständige Op-Listen aller NPC-Räume).
