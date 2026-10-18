# Runde 39 — Spur E "gegner" (Parking-Lot-Gorillas, Hundebiss, liegende Zombies, Gorilla-Schatten)

Baum: `.claude/worktrees/r39_gegner`, Zweig `r39/gegner`, Basis master c71fa144. Beginn 2026-10-18.

## Auftrag (woertlich, AUFTRAG.md)
1. "Die Gorillas beim Parking Lot können ruhig doppelt so viel Energie haben. Außerdem sollten sie erst die flinch
   Animation machen, wenn sie von der Energie her 3x mit der Handfeuerwaffe getroffen wurden, nicht bei jeden Schuss."
2. "Wenn ich einmal von einem Hund gerade angegriffen/gebissen werde, soll ich nicht parallel noch von anderen Hunden
   angegriffen/gebissen werden können, wenn ich mich nicht wehren kann, so zum Beispiel bei Room 1090."
3. "Die Zombies in ROOM 1220/1221 und in ROOM 1010 die am Boden liegen kriechen im Original. Das sollen sie bei uns auch."
4. "Die Gorillas haben beim springen direkt ihren Schatten unter dem beim." (Nachtrag 2026-10-18; Orchestrator: der
   Schatten haengt beim Sprung mit in der Luft unter dem Koerper statt auf dem Boden zu bleiben.)

Zuteilung VERTRAG.md §0: Spur E = genau diese vier Punkte. Keine Bank-9-Bits, keine Nachrichten-Ids noetig.

## Protokoll (fortlaufend)

### 2026-10-18 Start
- Baum geprueft: `git status --porcelain` leer, HEAD c71fa144, Zweig r39/gegner.
- AUFTRAG.md + VERTRAG.md vollstaendig gelesen; Vorrunden: Runde 35 J_affen.md, Runde 36 O_pri.md/F_zombieki,
  Runde 38 S_affen3.md, B_toene.md (Runde 39), Runde-16-Dossiers analysis/befunde_2026-09-19/kriecher-1010.md,
  liegende-zombies.md.
- Baseline-Build (local_build.sh configure + build) in diesem Baum: OK (exe 10:44).

### Schritt 0 — Zensus: welche Gegner, welche Raeume (Sce_em_set, `scd_dump_room.py`)
- **ROOM11C0 ("PARKING LOT", debug_jump_table.h)**: zwei Gorillas Typ **0x27** (`44 01 27 30 ..` / `44 02 27 30 ..`
  Szenen-Variante grid 0x30 @0x01784/0x01798; Kampf-Variante grid 0x10 @0x017E8/0x017FC). ROOM1141 traegt ebenfalls
  0x27 (grid 0x30/0x10) — darum Raum-Tor auf 0x11C0 (Nutzer: "beim Parking Lot", Einstieg: "nicht 1141").
- **"Room 1090" mit Hunden = ROOM1190** (Schiessstand B1, Hunde-Luke): ROOM1090.RDT hat keine Hunde (nur 0x26 Feuer,
  0x42 NPC, Zombies 0x10/0x11); ROOM1190.RDT: drei Hunde 0x20 (`44 00 20 40 ..` @0x02900/0x02914/0x02928 Szene,
  `44 00 20 00 ..` @0x02940/0x02954/0x02968). Dieselbe Zuordnung traf Spur B (B_toene.md §0.2) fuer den Gitter-Ton.
- **ROOM1220/1221**: zwei Zombies **0x16 mit grid 0x81** (liegend, @0x00F2A/@0x00F3E) + acht 0x16 grid 0x02.
  **ROOM1010/1011**: zwei Zombies **0x10 mit grid 0x81** (@0x00A12/@0x00A26 bzw. @0x00ACE/@0x00AE2).
- Flavor des Nutzers: RE2-KI (Standard seit 2026-08-22). Hund = RE2-Hundemodul (enemy_ai_re2_dog.c), Zombies =
  RE2-Zombiemodul (enemy_ai_re2_zombie.c); der Gorilla 0x27 hat in beiden Flavors das RE1.5-Gehirn
  (enemy_ai_common.c re15_maggot_ai_tick, Wurzel STAGE1.BIN 0x80116db8).

## Punkt 1 — Parking-Lot-Gorillas: doppelte Energie, Flinch erst nach 3 Handfeuerwaffen-Treffern (Energie)

### RE-Belege (Original, selbst gelesen)
- **HP-Saat** (STAGE1.BIN ohne Kopf, Datei-Offset = Adresse - 0x80100000): INIT FUN_80116f50
  `jal 0x8001af20` (rng) @0x801170cc, `lui a0,0x8012 / addiu a0,a0,-4044` = 0x8011f034 @0x801170d4-d8,
  `andi v0,v0,0xf` @0x801170e4, `lbu v1,8(a1)` (Typ) @0x801170e8, `sll v0,v0,1 / sll v1,v1,5 / addu / addu`
  @0x801170ec-f8, `lhu v0,0(v0)` @0x801170fc, **`sh v0,154(a1)` = +0x9a (HP) @0x80117104**.
  Zeile Typ 0x27 = 0x8011f034 + 0x27*0x20 = **@0x8011f514**: 16 x **180** (Datei 0x1f514, alle 16 Spalten gleich).
  Port: enemy_ai_common.c INIT `if (e->hp <= 0) e->hp = 180;` (konstant, weil alle 16 Spalten 180 sind).
- **Handfeuerwaffen-Schaden am Gorilla** (PSX.EXE t_addr 0x80010000, Datei-Offset = Adresse - 0x80010000 + 0x800):
  Schadenstabelle @0x8006e0d0 + Typ*0x58 + Waffe*4 (u16); Zeile 0x27 @**0x8006ee38** =
  `{0,7,30,12,12,28,28,50,38,40,40,70,8,50,8,40,40,70,400,20,0,100}`; **Waffe 3 (Pistole, RE1.5-Waffen-Id
  DAT_8006f430[0]=3 @0x8006f430) = 12 @0x8006ee44**. Port: re15_damage.c s_wpn_dmg_maggot[3] = 12,
  `dmg = dmg_row[weapon_id]` im Schusspfad re15_player_weapon_fire.
- **Treffer-Pfad (EXE, FUN_80011f50 / Resolver)**: +0x5 = Waffen-Id (@0x800124bc), hp -= dmg, +0x93 |= 1
  (Einmal-Treffer-Riegel), +0x7 = 0 (@0x80012428), **+0x4 = 2 (HURT) bzw. 3 (@0x80012520)**.
- **Gorilla-HURT Spur 0 (Boden-Flinch) FUN_8011b018** (Spur = Waffen-Id ueber Tabelle @0x801214a8; Pistole 3 -> 0):
  Phase 0: `lbu v0,147(a0) / ori v0,v0,0x2 / sb v0,147(a0)` @0x8011b064-70 (+0x93 |= 2), `ori v1,zero,0x7 /
  sb v1,148(v0)` @0x8011b08c-90 (**Flinch-Clip 7**), Blut @0x8011b0bc-130, `jal 0x800453d0 / ori a0,zero,0x3`
  @0x8011b134-38 (Se 3). Exit Phase 2: `sb zero,147(a0)` @0x8011b178 (+0x93 = 0), `sb v0,4(v1)` @0x8011b188
  (+0x4 = 1), `ori v0,zero,0x7 / sb v0,5(v1)` @0x8011b194-98 (+0x5 = 7 Vergeltungs-Sprung; 9 nur bei +0x1e3
  @0x8011b1c8-d8). Runde 35 Spur J: Sprung erst beim 3. Treffer (mag_hit_ctr), sonst +0x5 = 3 (Jagd).
- Das Original flincht also bei JEDEM Treffer (der Resolver schreibt +0x4 = 2 bedingungslos; es gibt keine
  Schadens-Schwelle). Beides — doppelte HP und die Flinch-Schwelle — sind **NUTZER-VORGABEN** (Form unten).

### Port-Ist (vor der Aenderung, Code gelesen)
- enemy_ai_common.c:8890 INIT hp = 180; :9423 `case 2` HURT-Wurzel; Spur 0 spielt Clip 7 bei jedem Eintritt.
- re15_damage.c re15_player_weapon_fire: `e->state = (e->hp >= 0) ? 2 : 3;` nach `hp -= dmg` — jeder Treffer ->
  HURT. Der Schuss faellt im Spieler-Schritt VOR `re15_enemy_ai_run_all` (game_step_common.c:2414), der Gorilla-Tick
  sieht den Treffer also im selben Bild.

### Umsetzung (geplant -> s. Protokoll unten)
- Neue Datei engine/src/gegner_r39.c + include/re15_gegner_r39.h; Haken (je 1 Zeile) in enemy_ai_common.c
  (INIT-HP, HURT-Eintritt, Zustands-Merker am Tick-Ende).
- HP: NUTZER-VORGABE x2 -> 360 (= 2 x Tabellenwert @0x8011f514), nur ROOM11C0 (g_current_room_id).
- Flinch-Schwelle: Schadens-Akkumulator seit dem letzten Flinch (hp_ref - hp); Flinch (Spur 0/1/2 wie bisher), wenn
  >= 3 x 12 = 36 (= 3 x @0x8006ee44); darunter: Treffer OHNE Zucken = Blut (wie der Original-Luftschuss ohne
  Flinch @0x80118b78-bd0) + Se 3, Zustand/Unterzustaende aus dem Merker des letzten Gehirn-Ticks zurueck,
  +0x93 = 0 wie der Flinch-Exit @0x8011b178 (sonst bliebe der Einmal-Riegel zu und der Gorilla waere unverwundbar).
  Andere Waffen "entsprechend ihrem Schaden": Schrotflinte 38/50, Magnum 40 >= 36 -> Flinch sofort.

## Punkt 2 — Kein paralleler Hundebiss (ROOM1190, RE2-Hundemodul)

### RE-Belege (RE2 Retail, EMD0G_MOD0.BIN RAW @0x80100000 + PSX.EXE Leon, selbst disassembliert)
- **Boden-Biss-Spielerhaken 0x801051BC** (a0 = 0: Spieler 0x800cfbf8, sonst Partner 0x800cfe18; a1 = Richtung):
  `lbu v1,467(a0)` @0x801051dc (PL+0x1D3), `addiu v0,zero,2 / sb v0,4(a0)` @0x801051e0-e4 (**PL +0x4 = 2 HURT**),
  `addiu v0,a1,2 / sb v0,5(a0)` @0x801051e8-ec (+0x5 = dir+2), `sb zero,6(a0) / sb zero,7(a0)` @0x801051f0-f4,
  **`ori v1,v1,0x80` @0x801051f8 / `sb v1,467(a0)` @0x80105200 = PL+0x1D3 |= 0x80 (EIN-ANGREIFER-RIEGEL)**.
  Aufruf aus dem Kontakt-Handler 0x80104DF0 nach FUN_800401D4(20,0) (Biss-Schaden 20 @0x80104EB8-C0), lebender
  Spieler (@0x80104F7C-88 -> 0x801051BC); toter Spieler -> 0x80105204 (PL +0x4 = 3).
- **Kontakt-Tor desselben Moduls**: `lbu v0,467(s1)` @0x80104e5c / `andi v0,v0,0x80` @0x80104e64 /
  `bne v0,zero,0x80104fcc` @0x80104e68 -> **kein Kontakt, solange der Spieler beansprucht ist**; ebenso das
  Flug-/Latch-Tor `lbu v0,467(s1)` @0x80104d84 / `andi 0x80` @0x80104d8c / `bne -> -4` @0x80104d90-94.
- **Freigabe durch den SPIELER** (PSX.EXE): Spieler-Wurzel FUN_8003bfac dispatcht PTR_LAB_800a4030[+0x4];
  Zustand 2 (HURT) = 0x8003f600, Unterdispatch @0x800a40e4[+0x5]: [0]/[1] 0x8003f658, **[2]/[3] 0x8003f778**
  (dir+2 des Hundebisses), [4] 0x8003f8e4, [5] 0x8003fc3c, [6] 0x8003ff3c.
  0x8003f778 Phase 0: Clip 3/4 (`addu v0,v0,a2` 0x30003 @0x8003f810-14), **`lbu v1,467(s0)` @0x8003f830 /
  `ori v1,v1,0x80` @0x8003f83c / `sb v1,467(s0)` @0x8003f844** (der Spieler beansprucht sich selbst erneut);
  Phase 1: Anim (0x8002959c @0x8003f88c); **Phase 2: `sw a0,4(s0)` (a0 = 1 -> Zustand 1) @0x8003f8b0 /
  `sb zero,467(s0)` @0x8003f8b4 = PL+0x1D3 := 0** (+0x1C0 &= 0xED @0x8003f8b8-c0, +0x154 |= 2 @0x8003f8bc-c4).
  Gleiches Muster [0]/[1]: `ori 0x80` @0x8003f6d4-d8, Exit `sb zero,467(s0)` @0x8003f748; [4]: @0x8003fa4c;
  [5]: @0x8003fe68 / @0x8003fec4. Die unteren 7 Bits von +0x1D3 zaehlt die Spieler-Wurzel jedes Bild herunter
  (`lbu v1,0x1d3(s0) / andi v0,v1,0x7f / beq -> / addiu v0,v1,-1 / sb v0,0x1d3(s0)` @0x8003bff4-c008).
  **Mechanismus: der Riegel steht genau so lange, wie der Spieler in seiner Treffer-Reaktion (Zustand 2) ist.**
- FUN_800401D4 (Spieler-Schaden) setzt den Riegel selbst nur im Todesfall (`lbu v1,0x1d3(a2) / ori 0x80 / sb`
  @0x800402c0-d0, Rueckgabe 2); der Port fuehrt das schon (re2z_player_damage).

### Port-Ist (Code gelesen)
- enemy_ai_re2_dog.c re2d_contact: Tor nur `re15_player_is_grabbed()` (Griff-Pin), **liest PL+0x1D3 (Port-Feld
  pl->re2z_self1d3) NICHT**; Boden-Biss setzt nur `pl->hit_react |= 1` + Kleidungsstempel — **kein Riegel**
  (Kommentar dort: "der Claim ueber den Grab-Pin unterbleibt (kein Latch)"). Folge: zweiter/dritter Hund beisst
  parallel waehrend der 22 Bilder Flinch (cmd 2, game_step_common.c s_hit_flinch) — der Nutzer-Befund.
- Der RE2-Zombie fuehrt denselben Riegel korrekt (Setzer @0x80102754-60, Loescher HURT @0x80104FA0 / DEATH
  @0x801082E8, enemy_ai_re2_zombie.c:1950/4349), die Zellenarme ebenso (enemy_ai_re2_zellenarm.c:690-696/872).
- Spieler-Seite im Port: cmd-2-Flinch 22 Bilder (PL00-Clip 8/9), Exit `pl->state = 1` (game_step_common.c:1599-1611),
  der NORMAL-Prolog loescht +0x93 (`pl->hit_react = 0` @0x80031964, :1626) im Folgebild; Knockdown-Exit kd_ende.

### Umsetzung (geplant)
- Boden-Biss: `pl->re2z_self1d3 |= 0x80` (@0x801051f8-0x80105200) + Merker "Riegel vom Hund gesetzt".
- Kontakt-Tor + Flug-Tor: zusaetzlich `pl->re2z_self1d3 & 0x80` (@0x80104e5c-68 / @0x80104d84-94).
- Freigabe: Port-Zwilling von @0x8003f8b4 = Ende der Spieler-Reaktion. Im Port endet sie, wenn der NORMAL-Prolog
  +0x93 Bit 1 loescht (@0x80031964) — Freigabe-Tick je Bild (Haken in re15_enemy_ai_run_all), nur fuer den vom Hund
  gesetzten Riegel (Zombie-/Arm-Riegel bleiben bei ihren eigenen Loeschern).

## Punkt 3 — Liegende Zombies (0x81) kriechen (ROOM1010/1011, 1220/1221)

### RE-Belege
- **RE1.5 (STAGE1.BIN) DECIDE[5] FUN_8010466C** (Dossier 2026-08-26 / Port enemy_ai_common.c:4794-4825, selbst
  disassembliert): Block B @0x80104704: `sltiu 0xbb8` @0x80104718 (dist < 3000), arc 0x200 @0x8010472C-34,
  **`ori v0,1` @0x80104738 / `sw` @0x80104748 = Wort 1 = Kriech-Lokomotion**. Liegende 0x81-Zombies des Originals
  kriechen also los, sobald der Spieler in Sicht (+-0x200) und naeher als 3000 ist.
- **RE2 (EMZ0.BIN) INIT Kriecher-Zweig** @0x80100B60-8C: Deskriptor 1 -> `sw 0x201` @0x80100B74 = KRIECHER-WAIT
  (EXEC[2] @0x80103B48: Clip 23, Rate 0, **kein Ausgang**); DECIDE[2] @0x80103A70 weckt NUR in den Griff (512er-Sektor,
  dist < 0x514, Riegel frei, Sicht, Etage -> 0x101 @0x80103B10-14). Einen Distanz-Wecker in die Lokomotion hat RE2
  nicht (kriecher-1010.md §2.4). Deskriptor 3 -> `sw a1(=1)` @0x80100B8C = kriecht sofort.
- Port (Runde 16 F2, enemy_ai_re2_zombie.c:8708): RE1.5-sel 1 unter 0x80 -> RE2-WAIT (Totstellen, damals
  Nutzerwunsch "stellen sich erst einmal tot"), sel 3 -> Lokomotion. Folge heute: die 0x81-Zombies in 1010/1220
  liegen, bis der Spieler auf < 1300 im Sektor steht (Griff) oder sie anschiesst — sie **kriechen nie los**.
  Der Nutzer vergleicht mit dem RE1.5-Original (DuckStation): dort kriechen sie ab 3000.

### Umsetzung (geplant)
- NUTZER-VORGABE "kriechen wie im Original" in RE1.5-Form: im RE2-WAIT (re2z_crawl_decide_wait) nach dem byte-true
  RE2-Griff-Wecker der RE1.5-Wecker Block B (dist < 0xbb8 @0x80104718 && arc 0x200 @0x8010472C-34 -> Wort 1
  @0x80104738/48) -> RE2-Kriech-Lokomotion (Wort 1 = 0x80101210 P0). Das Totstellen bleibt (bis zur Naehe).

## Punkt 4 — Gorilla-Schatten beim Sprung auf dem Boden

### RE-Belege
- **RE1.5 Gorilla-Wurzel FUN_80116db8 (STAGE1.BIN), Schwanz jedes Bild**: `lw v0,-14460(v0)` (g_entity) @0x80116f24,
  **`lh a1,442(v0)` @0x80116f30 = a1 = +0x1ba (Boden-Referenz)**, `jal 0x8001b064` @0x80116f34, `addiu a0,a0,176`
  @0x80116f38 (a0 = +0xb0 Schatten-Record). FUN_8001b064 (EXE, Decompile RE_15_Quellcode_V2): `local_88.t[1] =
  (long)param_2` = Quad-Hoehe = a1; Ecken aus rec+0xc/+0xe (+0xbc/+0xbe), Versatz rec+8/+10, RotMatrixY(+0x6a).
  Dasselbe Muster: Hund @0x8010d91c (Runde 35 Spur H), kleiner Affe 0x29 STAGE3.BIN @0x80110c78 (`lh a1,442(v0)` /
  `jal 0x8001b064` @0x80110c7c / `addiu a0,a0,176` @0x80110c80).
- Sprung: +0x1ba bleibt der Boden (Ballistik c1a4 landet auf +0x1ba; Sce_em_set saet +0x1ba = -(pc[4]*1800)
  @0x800421d4-0x80042210, Port scd_vm.c:3929 dog_floor_y).
- RE2 (Gegenprobe, PSX.EXE Leon): Entity-Schleife `jal 0x8004fba0` @0x8002686c / `sh v0,0x1c2(s0)` @0x80026874,
  `lh a1,450(s0)` @0x800268c8 -> 0x800168b4 -> Quad-Y = +0x1C2 (Boden-Sonde) fuer JEDE Entity.

### Port-Ist (Code gelesen)
- platform/pc/main.c:9859 `int32_t nsh_y = npc->y;` — Quad-Hoehe = KOERPER-Y; Ausnahmen nur Hund (Runde 35 H),
  NPC-Kante (Runde 38 M), Wasserzombie (Runde 38 U), Kraehe. Der Gorilla (0x27) und der kleine Affe (0x29) fallen
  auf npc->y -> beim Sprung (y steigt bis zu +0x1ba - 1600 im Rear-Up / Ballistik) haengt der Blob unter dem Koerper.

### Umsetzung (geplant)
- Haken (1 Zeile) in main.c hinter der Wasserzombie-Zeile: `re15_r39_schatten_y(npc, &nsh_y)` -> fuer 0x27 und 0x29
  Quad-Y = dog_floor_y (+0x1ba, @0x80116f30 / @0x80110c78). Messung: Framedump je Sprungbild + Log Koerper-Y /
  Boden-Y / Quad-Y (RE15_R39_LOG).

## OFFEN
- (wird fortlaufend gepflegt)

## Fuer den Nutzer
- (wird am Ende ausgefuellt)
