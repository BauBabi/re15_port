# Runde 38 — Spur R "spinnen" (Dossier, fortlaufend)

Baum `.claude/worktrees/r38_spinnen` (Zweig `r38/spinnen`, Basis e2cc7673). Angelegt 2026-10-16 16:23.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md Z. 68-70)
1. "In Stage 2 die Spinnen die fliegen fast alle in der Luft und kommen teilweise nicht aus den Schächten raus wie im Original"
2. "In Stage 2 die Spinnen verschießen kein Gift wie im Original, was dumm ist - denn das Gift ist das einzige das die
   Charaktere vergiften kann und damit der einzige Grund für das Gegengift Item"
3. "In Stage 2 die Minispinnen, die rauskommen, wenn man eine Spinne getötet hat fliegen weitestgehend in der Luft und
   greifen einen nicht an wie im Original"

## Arbeitsstand
- 16:23 Dossier angelegt; Pflichtlektuere AUFTRAG/VERTRAG/UEBERGABE gelesen.
- 16:55 Messschiene RE15_SPINNEN_LOG gebaut; ROOM2060 in beiden KI-Modi gemessen (1.1/1.2).
- 17:20 Minispinnen gemessen (1.3); Original per DuckStation-GDB gemessen (1.4); RE-Belege 2.1-2.4.

## 1. Messung vorher

### 1.0 Werkzeug
Messschiene `RE15_SPINNEN_LOG=<datei>` (neu, engine/src/spinnen_r38.c `re15_spinnen_r38_log_frame`, Haken in
platform/pc/main.c hinter dem RE15_STATE_LOG-Block): je Bild Spieler (x,y,z,hp,Status-Wort +0x98) und je Spinne
(Typ 0x25/0x26) Zustand/Unterzustaende/grid/floor/RE2-Modus +0x222/Lage x,y,z/Drehung/hp. Laeufe ueber die echte
exe (Messkopie `re15_pc_r38sp_mess.exe`), `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=<raum>@250`.

### 1.1 Zensus der Stage-2-Spinnen (Sce_em_set, `tools/scd_dump_room.py` ueber ALLE ROOM*.RDT)
Typ 0x25 (Adult) NUR in STAGE2: ROOM2030/2031 (7 Saetze, grid 0x00, floor 3 = y -5400 bzw. Satz 4 floor 9 =
y -16200), ROOM2050/2051 (2x grid 0x00 floor 3, y -5400), ROOM2060/2061 (2 Plaetze je if/else: grid 0x41 floor 2
y -3600 IM SCHACHT, sonst grid 0x00 floor 0), ROOM2070/2071 (12 Saetze grid 0x00 floor 0), ROOM2090/2091
(2x grid 0x00 floor 0), ROOM20A0/20A1 (je if/else grid 0x41 floor 3/2 bzw. grid 0x00 floor 0).
Typ 0x26 hat ausser ROOM1090 (7x = FEUER-Emitter, memory reai-v2-spider-ai) KEINEN Satz; STAGE2.BIN registriert 0x26
gar nicht (Registrierung @0x80116eec-0x801170c8: 0x25 -> 0x801109e4 @0x80116f84 `sw v0,11328(at)`, danach
0x40/0x42/0x49/0x4b — kein Store auf 0x80072c44).

ROOM2060 main00 (Datei 0x0DDA-0x0E74): Obj_model_set 0/1 band 2 bei (5775,-4860,5325)/(-5525,-4860,825) = die
SCHACHTGITTER; `Ck(4,0xFB)==0 -> Sce_em_set 44 00 25 41 02 .. (5800,-3600,6400)` sonst `44 00 25 00 00 .. (5200,0,-1000)`;
`Ck(4,0xD5)==0 -> 44 01 25 41 02 .. (-6600,-3600,800)` sonst `(-3800,0,-1400)`. sub01 (pro Bild): `Ck(4,0xFB)==0 &&
Cmp(work[0x0A]==1)` (= angezeigter Cut 1, work_vars[0x0A] = DAT_800b0fe4 @0x8001d948) -> `Evt_exec sub02` + Set(4,0xFB);
dasselbe mit Cut 5 -> sub03 / (4,0xD5). sub02 @0x133E: `Work_set 2 0` (Gegner 0) + `Member_set 0x0C 0x42` (= +0x09
grid := 0x42, actor_common.c Member 12) + `Work_set 3 0` (Objekt 0 = Gitter) + Se_on + Speed/Add_speed-Schleifen =
das Gitter faellt/klappt auf. => Im Original kommt die Schachtspinne heraus, sobald das Gitter faellt und grid
0x41 -> 0x42 wechselt.

### 1.2 Port-Messung ROOM2060 (Debug-Sprung, Spieler-Spawn (7600,0,3950), Cut 0)
- **RE2-KI (Default)**: beide Spinnen (Slot 1/2) starten mit grid 0x41 floor 2 auf **y = -3600** und laufen im
  RE2-Bodenmodus (+0x222 = 0) frei in X/Z umher — **die ganze Zeit auf y -3600, 3600 Einheiten ueber dem Boden
  (Spieler y 0)**. Sie beissen den Spieler aus der Luft: HP 100 -> 80 (F~249) -> 60 (F~449). Bild
  (Framedump F310/F450/F560/F700): die Spinnen haengen ueber Leon unter der Decke neben dem Schachtgitter.
  => Nutzerpunkt 1 ("fliegen in der Luft") REPRODUZIERT. Ursache im Code: enemy_ai_common.c re15_adult_spider_ai_tick
  mappt +0x10E <- grid 0x41 -> RE2-INIT-Sprungtabelle Eintrag 1 (`k == 1` = nichts) -> Modus 0 BODEN auf der
  Spawn-Hoehe; der RE2-Bodenmodus hat keinen Fall/keine Bodenhoehe.
- **RE1.5-KI** (RE15_AI_FLAVOR=re15), Spieler bleibt in Cut 0: beide Spinnen in Zustand 4 (AMBUSH) Unterzustand 1,
  Clip 0x10, unbewegt im Schacht (5800,-3600,6400)/(-6600,-3600,800) = richtig bis zum Ausloeser.
- **RE1.5-KI**, Spieler auf (6000,3000) = Cut 1 (sub02 feuert sofort): Spinne 1 grid 0x42, Zustand 1, bleibt auf
  y -3600 im Schacht stehen (der Port-Zweig `grid != 0x41 -> Zustand 1` hat keinen Abstieg, Kommentar "drop = OPEN").
  => Nutzerpunkt 1 ("kommen nicht aus den Schaechten raus") REPRODUZIERT.

### 1.3 Port-Messung Minispinnen (RE2-KI, ROOM2030, Boden y -5400; Schrotflinte, Spinne 1 getoetet)
`RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,2000,3072,3 RE15_GIVE=8:30 RE15_EQUIP=8`, Feuerskript. Spinne 1 HP
110 -> 55 -> 0 -> -55 (F~350-450); danach Slots 4..N Typ 0x26 grid 0x02. Die Babys **fallen von y -5400 auf y 0**
(5400 UNTER den Raumboden) und laufen dort; einige springen (Unterzustand 3, +0x222 = 1, y -560..-1280). Spieler-HP
bleibt 100 — **kein einziger Biss**. Ursachen im Code: (a) `re2s_spawn_babies` (enemy_ai_re2_spider.c) setzt das
RE2-Feld +0x1C2 (Port dog_floor_y) des Kindes nicht -> 0 -> `re2sb_probe` landet erst bei y >= 0; (b) `re2sb_bite`
prueft `dist < 10` statt `dist < Ziel+0x9A + 10` (Kommentar "OPEN, reine +10-Reichweite") -> nie in Reichweite,
dazu die y-Probe `(u32)(pl.y - e.y) < 3000` mit dem Baby 5400 tiefer. => Nutzerpunkt 3 REPRODUZIERT.

### 1.4 ORIGINAL-Messung RE1.5 (DuckStation-GDB, MZD-Disc = identische PSX.EXE/STAGE2.BIN-Adressen)
Werkzeug `R_belege/gdb_spinne.py` (Muster Runde 36 F nb4): Savestate `stage_saves/mzd_debugmenu.sav`, Haltepunkt
Debug-Menue-Tick 0x8001443c -> Schreibsatz von Quadrat in der JUMP-Zeile (@0x80014a44-58: 0x800b5359 := 1,
0x800ac9a8 := 0, Menue zu 0x800bbe5c := 0) mit Stage-Var 0x800bbe5e := 1 und Raum-Index 0x800bbe5f[1] := 6/10
(@0x8001d62c-660 liest genau diese), danach Haltepunkt Gegner-Schleife 0x8001a50c = einmal je Bild; Prolog-Kontrolle
`801109e4 = e8ffbd27` (addiu sp,sp,-24 = Spinnen-Wurzel). settings.ini EnableGDBServer nur waehrend des Laufs.
- **ROOM2060, Schacht** (`R_belege/orig_2060_schacht_gift.txt`, Spieler bei Bild 60 nach (6000,3000) = Cut-1-Zone
  "0->1" der RVD): Bild 62 Cut 1 -> sub02 setzt grid 0x41 -> 0x42; Bild 63-73 Clip 16, z -200/Bild (6400 -> 4325,
  Wandklemme schiebt x 5800 -> 5975), +0x93 = 03; Bild 74-92 Clip 16 ohne Bewegung bis Bild 31; Bild 94 y -3600 ->
  -2600 (+1000), z +300 (rueckwaerts), +0x1ba := 0, rot_z := 1024; Bild 95-102 Clip 2, y +111/Bild bis -1720 (> -1800);
  Bild 103-114 Clip 13, y +103..111 bis -428 (>= -500 stoppt); Bild 133 Clip 13 Bild 31; Bild 134 y := +0x1ba = 0,
  rot_z 0, grid 5, floor 0, Lage (5740,0,4332); Bild 135 Zustand 1, +0x93 = 0; Bild 136 SPUCKEN (Unterzustand 7,
  Clip 12). Drei Effektplaetze mit Routine A 24 -> 25 fliegen zum Spieler; Spieler Zustand 2 (getroffen).
- **ROOM2060, Gift** (`R_belege/orig_2060_gift_hp.txt`, 1800 Bilder): Treffer alle ~62 Bilder, je **-10 HP**
  (100->90->80...), Spieler Zustand 2 — aber **in 14 Treffern KEIN Gift** (Status 0x800acaec bleibt 0000).
- **ROOM2060, Gift-Wuerfel** (`R_belege/orig_2060_rng_a0.txt`): Haltepunkt 0x80012e78 (erster `jal 0x8001af20` im
  Spielerzweig von FUN_80012d60): **a0 = 0x0000000A in 13 von 13 Treffern** (= der SE-Id 10 aus dem Delay-Slot
  `ori a0,zero,0xa` @0x80012e74, FUN_800453d0 laesst a0 stehen). FUN_8001af20 wertet NUR a0 aus (@0x8001af30-4c:
  `srl v1,a0,7 / andi 0xff / addu / andi 0xff`) -> r1 = 10, a0' = 10, r2 = 10 -> beide gerade -> `mult` = 0 ->
  **das Gift-Bit 0x2 @0x80012eac wird im Original NIE gesetzt** (toter Wuerfel; die "1/4"-Annahme in
  re15_damage.c:76-90 ist damit widerlegt).
- **ROOM20A0, Schacht** (`R_belege/orig_20a0_schacht.txt`, Spieler (21500,-24000), Cut 4 gesetzt): Spinne 0 (floor 3,
  y -5400) laeuft dieselbe Choreografie und landet auf **y 0, floor 0** (Lage (22732,0,-25662)), obwohl der Steg des
  Spielers auf -3600 liegt — das Original setzt +0x1ba fest auf 0 (@0x80115044) und y := +0x1ba (@0x80115264-6c).

## 2. RE-Belege

### 2.1 Schacht-Ausstieg = Zustand 4 Unterzustand 1 (grid 0x41), STAGE2.BIN (selbst disassembliert)
Zustand-4-Wurzel 0x80114e18: `lbu +0x5` -> Tabelle @0x80118f90 = {0x80114e60 (grid 0x40), 0x80114ec8 (grid 0x41)}.
0x80114ec8: Phase +0x7, Gate `sltiu v0,v1,0x9` @0x80114ee0, Sprungtabelle @0x8010027c (9 Eintraege, selbst gelesen):

| Phase | Adresse | Inhalt (Bytes) |
|---|---|---|
| 0 | 0x80114f08 | grid (+0x9) == 0x41 -> warten (`ori v0,zero,0x41 / beq` @0x80114f18-1c); sonst +0x7 = 1, Clip +0x94 = 0x10, +0x95 = 0, +0x8f = 7, +0x78 = *(0x80118e0c) = Kasten 0x80118dec, +0x93 \|= 3 (@0x80114f24-88); faellt durch |
| 1 | 0x80114f8c | +0x95 < 11 (`sltiu v0,v0,0xb` @0x80114fa0) -> +0x8c = 200 (`ori 0xc8` @0x80114fa8), FUN_800245d8(0); anim_set(+0x84,+0x16c,0,0x200); +0x95 == 0x1f -> +0x7 = 2 (@0x80114fe4-f4) |
| 2 | 0x80114ff8 | +0x7 = 3; y += 1000 (`addiu v0,v0,1000` @0x8011501c); +0x8c = 300 (`ori 0x12c` @0x8011502c); FUN_800245d8(0x800) (rueckwaerts); +0x1ba = 0 (`sh zero,442` @0x80115044); +0x6c += 1024 (@0x8011505c); anim_set |
| 3 | 0x80115068 | Clip 2, +0x7 = 4, +0x95 = 0, +0x8f = 7; faellt in 4 |
| 4 | 0x801150a8 | anim_set; y += 100 + (rng & 0xf) (@0x801150dc-e8); (+0x1ba - 1800) < y -> +0x7 = 5 (@0x801150f8-114) |
| 5 | 0x80115118 | +0x7 = 6, Clip 0xd, +0x95 = 0, +0x8f = 7; faellt in 6 |
| 6 | 0x80115158 | anim_set; y < +0x1ba - 500 -> y += 100 + (rng & 0xf) (@0x80115180-bc); +0x95 == 0x1f -> +0x7 = 7 |
| 7 | 0x801151e4 | +0x7 = 8, +0x8f = 0, +0x6c -= 1024, +0x78 = *(0x80118e04) = Kasten 0x80118dd4, +0x82 = 0, grid = 5, y = +0x1ba; Quadrant (+0x6a & 0xc00) >> 10: 0 x += 1488, 1 z -= 1488, 2 x -= 1488, 3 z += 1488 (@0x80115278-310); anim_set |
| 8 | 0x80115338 | +0x93 = 0, Clip 0, +0x95 = 0, +0x8f = 0, anim_set, Zustandswort `sh 1,4` / `sh 0,6` (@0x80115394-b0) |

Die Original-Messung 1.4 trifft jede Phase (Bilder 62-136). Zensus: grid 0x40 (@0x80114e60 -> CLIMB) kommt in KEINEM
ausgelieferten Sce_em_set vor (nur 0x00/0x41), wird deshalb nicht portiert.

### 2.2 Gift = Effekt 0x0d der Raum-ESP (RE1.5)
- Spucken (Unterzustand 7) @0x801126d4: `func_0x80019700(0x0d002000, +0x6a, *(+0x188)+0x40, &DAT_80118ee8)` @0x80112770
  + SE 4 (`jal 0x800453d0` @0x80112778), Clip 0xc; Ende -> Unterzustand 0xb, +0x1d6 = 0xf.
- ESP 0x0d (alle Stage-2-Spinnenraeume, Raum-ESP RDT+0x4C): sub 0 = 3 Stroeme je 3 Zeilen (`R_belege/esp_ids.py`):
  Zeile 0 A=10 (Darstellung), Zeile 1 A=24, Zeile 2 A=25 acc (0,10,0); Groessen 4096/3800/3600, Startgeschwindigkeit
  (150,-40,0)/(120,-32,0)/(90,-24,0).
- Routine 24 @0x80017f50 (Tabelle 0x80071d40[24] = 0x80071da0): row[0x0e] != 0 -> row[0x0e]--, sonst Vorschub
  FUN_800174e4; danach immer Routine 25 (`jal 0x80017fa4` @0x80017f8c).
- Routine 25 @0x80017fa4: +0x04 += 30, +0x06 += 30 (@0x80017fbc-d8); P = Weltlage +0x28/+0x2a/+0x2c; Boden =
  FUN_8001c6e8(&P,0,8,0x100) (@0x80017ff4); Treffer = FUN_80012d60(500,&P,0) (`ori a0,zero,0x1f4` @0x80017ffc,
  `addu a2,zero,zero` @0x80018004 = Angriffsart 0 = 10 HP); Treffer ODER P.y >= Boden -> Kind
  `0x030c0000 | (+0x72 + 500)` an P (FUN_800199d4 @0x80018054) + SE 0x03030001 an P (@0x80018064), dann A = B = 0,
  +0x08..+0x14 = 0, Anim +0x6e = 0x0d (@0x80018078-98).
- Spielerzweig FUN_80012d60 Art 0: HP -= 10, SE 10, Gift-Wuerfel (tot, s. 1.4), Zustand 2.
- Gift-Wirkung (Spieler Zustand 1 = Handler 0x80031de8 = Tabelle 0x80073f90[1] @0x80073f94): (status & 7) != 0 ->
  Zaehler +0x1d3 (0x800acc27) -1; war 0 -> Zaehler = 0x78 (8 bei Bit 0x4), HP -= 2, HP < 0 und nicht Bit 0x4 -> HP = 1
  (@0x80031e44-ecc). Heilung: Gegengift 0x23 / Blau 0x26 / Mischungen `andi 0xfffd` @0x8004af8c (Port vorhanden,
  item_use_common.c). Der Port hat die Wirkung NICHT (player_common.c:86 "not ported").

### 2.3 Gift in RE2 Retail (Ziel, wo RE1.5 unfertig ist)
- RE2-Spinne (EMS25.BIN) Modus 0 Unterzustand 8 @0x80100F88 ("Oberflaechenwechsel" im Port) ist das SPUCKEN:
  Clip 11 + FUN_801058A4(self, +0x222+8, +0x76+0x21A) @0x8010101C; FUN_801058A4 @0x801058a4-9c8 spawnt
  `0x1d001000 | (id << 16)` (`lui v0,0x1d00 / ori 0x1000` @0x801058f0-f4, `jal 0x8001bf10` @0x801059c4) an Knochen 0
  + (1000,0,-100) = FX 0x1D sub 8..11.
- FX 0x1D (RE2 Raum-ESP room2010/3020/4050, `R_belege/re2esp.py`): 4 Kugeln, Zeile 1 Op B 16 -> Op A = step[0x0B] =
  0x20 = **Op 32** @0x800200b0 (Tabelle 0x8009d868[32] = 0x8009d8e8): Treffertest FUN_80047664(P, 800, 20); Ziel-Art
  < 0x10 (Spieler) und `andi v0,v0,0x8` @0x80020118 (sub & 8) -> **0x800cfe10 := 5400** (`sh v0,-0x1f0(at)`
  @0x80020130, Gift), 0x800cfe12 := 30, PL+0x1D3 \|= 0x80. RE2-Gift-Wirkung FUN_8003bfac: alle 30 Bilder HP -1 bis 1
  (@0x8003c12c-160). => In RE2 vergiftet JEDER Gift-Treffer.

### 2.4 Minispinnen (RE2 EMS26.BIN / EMS25.BIN)
- RE1.5 STAGE2 registriert Typ 0x26 nicht und der RE1.5-Spinnen-Code spawnt keine Babys (Effekt-Spawns im Bereich
  0x801109e4-0x801158a0: nur 0x0d002000 und 0x0008xxxx Blut) -> Minispinnen gibt es nur im RE2-Modus.
- RE2-Spawner FUN_80105D38: Kind-Lage = Eltern-Lage (@0x80105e10-30), KEIN +0x1C2-Schreiber; +0x1C2 setzt die
  RE2-Engine fuer JEDE Entity je Bild = FUN_8004fba0(Lage, 0, (+0x9)<<8, 0) (`jal 0x8004fba0` @0x8002686c,
  `sh v0,0x1c2(s0)` @0x80026874) = Bodenhoehe unter der Entity.
- Biss FUN_80100DE8 (EMS26): `dist < (u16)Ziel[+0x9A] + 10` @0x80100E40; RE2-Spieler +0x9A = **450**
  (`addiu v1,zero,450` @0x8003bdc0 / `sh v1,154(s2)` @0x8003bdc4, RE2 PSX.EXE) -> Reichweite 460.

## 3. Umsetzung

## 4. Messung nachher

## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
