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
- 17:45 Umsetzung Schacht/Gift/Babys (spinnen_r38.c + Haken), Koerperschub aus dem Spieler.
- 18:05 Unit-Test test_r38_spinnen.c (A/B/C) gruen; Rest-Abweichung Kriechbahn (x +175) im Original vermessen:
  Objekt-Ausschiebung FUN_8002cabc am Schachtgitter (1.5/2.5), portiert (Teil 4), Bahn jetzt Bild fuer Bild gleich.
- ~18:18 Sitzungslimit; Reste als a7a84080 "UNGEPRUEFT" gesichert.
- 22:10 FORTSETZUNG (neue Sitzung): `local_build.sh build` -> "ninja: no work to do" (Stand a7a84080 war bereits
  gebaut), `unit_r38_spinnen` gruen (0 FAIL, Ausgabe 4.0). Danach Messung NACHHER in der echten exe (Abschnitt 4).

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
Identitaet der Mess-Disc nachgewiesen (ISO-Leser ueber das MODE2/2352-Abbild, md5 gegen den Auslieferungsbaum):
PSX.EXE b55fdaa5b9ec0f84bdd8a6b1107ea0b0, PSX/BIN/STAGE2.BIN 73a0a8e2d19437cf7da9509cbea77a73,
PSX/STAGE2/ROOM2060.RDT 3a8f7024acd63057231c9bd5dc4e0880 — alle drei byte-gleich mit info/Re1.5 bzw. shared_assets.
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

### 1.5 ORIGINAL-Messung: wer klemmt die Kriechbahn? (`R_belege/gdb_klemme.py`)
Port vorher: Phase-1-Bahn z 6400 -> 4200 in 11 x 200 bei x 5800; Original (1.4) Bild 67 z 5525 statt 5400, Bild 68
x 5800 -> 5975, Ende (5975,4325). ROOM2060-SCA hat KEIN Band-2-Feld (alle 28 Felder Band 0/1), FUN_800245d8 ist ein
reiner Vorschub (@0x800245d8-70c, kein Klemmaufruf).
- Klammer-Messung (`orig_2060_klemme.txt`): Haltepunkte hinter jedem Aufruf der Spinnen-Wurzel (0x80110a54 Zustand,
  0x80110a64 b498, 0x80110a74 aec4, 0x80110a84 b544, 0x80110aa4 b0a4 SCA, 0x80110b58 b064): x/z bleiben in der
  ganzen Wurzel stehen; die Aenderung liegt zwischen Wurzel-Ende (Bild 66 z 5400) und der naechsten Gegner-Schleife
  (Bild 67 z 5525) — also AUSSERHALB der Spinne.
- Schreib-Wachpunkt Z2 auf e0+0x3c (`orig_2060_watch.txt`): Treffer `pc=8002cde0 ra=8002be48` genau in Bild 66->67
  und 67->68 = FUN_8002cabc, gerufen aus dem Objekt-Tick FUN_8002bd44 @0x8002be40.
- Wachpunkt auf e0+0x40 (`orig_2060_watch42.txt`): Schreiber `pc=8001d128` (Zeichen-Schleife, `sh` @0x8001d124/
  @0x8001d13c/@0x8001d154) mit der schon geklemmten Lage = Lage-Spiegel +0x40/+0x42/+0x44 NACH dem Objekt-Tick.
- ROOM20A0 (`orig_20a0_schacht.txt` Bild 50/51): Gitter obj 2 (22532,-6610,-27050) -> z -27250 = -27050 - 200,
  x 22732 = 22532 + 200: derselbe Mechanismus.

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

### 2.5 Objekt-Ausschiebung der Aktoren (PSX.EXE, selbst disassembliert)
Bild-Reihenfolge @0x8001ce04 Gegner-Schleife FUN_8001a50c -> @0x8001ce0c Spieler-FSM -> @0x8001ce14 Objekt-Tick
FUN_8002bd44. Dort: `lbu s3,2(v0)` @0x8002bd78 = nOmodel (RDT +2), Objekte von hinten (s2 = 0x800b3f98 + 148*(s3-1),
@0x8002bdb0-c8), nur aktive (`andi 1` @0x8002bdd4); je Objekt ALLE aktiven Aktoren (`lbu s1` 0x800aca4e @0x8002be14,
s0 = 0x800acc2c, Stride 0x1f4): `jal 0x8002cabc` @0x8002be40 mit a0 = Aktor, a1 = Objekt, a2 = 0 (Delay
`addu a2,zero,zero`). KEIN Typ-Filter. FUN_8002cabc (a2 = 0):
| Adresse | Inhalt |
|---|---|
| 0x8002cad0-dc | Aktor-Wort0 & 0x40 -> Rueckgabe 0 |
| 0x8002cae4-f0 | Objekt-Wort0 & 2 -> Rueckgabe 0 |
| 0x8002caf8-cb38 | t8 = (objbox.cx + obj.x) - (akt+0x7c.x + akt.x), t7 analog z |
| 0x8002cb3c-4c | t5 = objbox.hx + aktbox.hx, t4 = objbox.hz + aktbox.hx (Aktor +6 fuer beide Achsen) |
| 0x8002cb50-70 | `sltu (2*t5),(t8+t5)` / `sltu (2*t4),(t7+t4)` -> keine Ueberlappung |
| 0x8002cb84-c8 | Y-Test: t6 = (objbox.cy + obj.y) - (aktbox.cy + akt.y), a1 = aktbox.hy + objbox.hy |
| 0x8002cbcc-cc38 | Aktor+0xa := Objekt-Index, Objekt+0xa := Aktor-Index (kein Leser im Spinnen-Code 0x801109e4-0x801158a0) |
| 0x8002cc3c-70 | Vor-Hoehe aus den Spiegeln +0x42 (`lh`) — vorher nicht in der Hoehe -> Y-Ausschiebung @0x8002ccb4-e4 |
| 0x8002cce8-d0c | `slt` \|t7*t5\| < \|t8*t4\| -> x @0x8002cd50-7c, sonst z @0x8002cdb8-dc (je +-t5/t4 an die Objektkante) |

KEINE Null-Kasten-Ausnahme: das Schachtgitter obj 0 (5775,-4860,5325) traegt im Obj_model_set h(0,0,0) und ist mit
t5 = 0 + 200 (Spinnen-Kasten C) ein Punkt-Hindernis. Nachgerechnet Bild 67: dx -25, dz -75 -> \|dx*200\| < \|dz*200\|
-> z = 5325 + 200 = 5525; Bild 68: dz 0 -> x = 5775 + 200 = 5975 — beides gemessen. Lage-Spiegel: Sce_em_set
`sh` @0x800421a0/@0x800421ac/@0x800421b8 (= Spawnlage pc[8..13]), danach je Bild @0x8001d124-154; Objekt-Spiegel
`sh v1,0x42(s2)` @0x8002c0f8 am Ende jedes Objekt-Durchgangs.
Port vorher: re15_actor_prop_pushout schob nur die NPC-Familie 0x40..0x4d (mit Spieler-PR 450) und
re15_collision.c behandelt Null-Kaesten als nicht kollidierend (Annahme fuer Heli/Rotor ROOM1170, nicht aus
FUN_8002cabc) — die Spinne lief durch das Gitter.

### 2.6 Minispinnen GEZEICHNET in der Luft: falsches Animationspaar der RE2-Babybank (Fortsetzung, neu)
**Messung im Port (vor diesem Fix, Stand a7a84080, echte exe, `c_2030`):** die Babys stehen LOGISCH auf dem Boden
(Messschiene: y = +0x1C2 = -5400 in 544 von 585 Bildern, sonst Sprung), im Framedump liegt aber eine Kette brauner
Klumpen auf GELAENDERHOEHE neben Leon (`R_belege/babys_vorher_zoom.png`, Bild 200/280/400/520). Sonde
`R_belege/sonde_babybank.c` (gleiche Zeichenkette wie der NPC-Renderer: Pose je Keyframe, Mesh-Vertex-Y), Ausgabe
`R_belege/sonde_babybank_ausgabe.txt`: die Port-Bank fuer kind 0x26 ist **Paar 3** (EMD dir[5]/dir[6], 3 Clips 14/36/20
Bilder, 80-Byte-Keyframes) mit Wurzel **(0,-1797,0)** in Keyframe 0 und -1797..-1585 ueber alle 70 Keyframes ->
gezeichnete Unterkante **-1797..-1166** ueber der Lage = die Babys schweben in Brusthoehe. (Adult EM025 zum Vergleich:
Wurzel -400, Unterkante +122 = steht.) Ursache im Port: re2_emd_parse_bank nimmt als Hauptbank "das Paar mit den
meisten Clips" (PORT-Regel, re2_ems.c) — fuer EM026 Paar 3 (3 Clips) statt Paar 1 (1 Clip).
**Original (RE2 Retail, selbst disassembliert):**
| Adresse | Bytes/Instruktion | Bedeutung |
|---|---|---|
| PSX.EXE 0x8001abb0 | `sw v1,380(s1)` | Binder: EMD dir[1] (EDD Paar 1) -> Entity +0x17C |
| PSX.EXE 0x8001abc0 | `sw v1,264(s1)` | Binder: EMD dir[2] (EMR Paar 1) -> Entity +0x108 |
| PSX.EXE 0x8001abd0/abe0/abf0/ac00 | `sw v1,388/384/396/392(s1)` | dir[3..6] -> +0x184/+0x180/+0x18C/+0x188 |
| EMS26 0x8010005c / 0x80100060 | `lw a1,264(s0)` / `lw a2,380(s0)` | Wurzel reicht +0x108/+0x17C an die Zustandstabelle @0x80101084 |
| EMS26 0x801004ec-f4, 0x80100854-5c | `addu a1,s2 / addu a2,s3 / jal 0x8002959c` | Animations-Treiber mit genau diesen Zeigern (5 Aufrufe) |
| EMS26 0x8010011c / 0x80100120 | `lui v0,0xf` / `sw v0,332(s0)` | einziger Clip-Schreiber: +0x14C = 0x000F0000 = Clip 0 |
Vollscan EMS26.BIN (1086 Worte): ausser 0x8010005c/60 KEIN Zugriff auf +0x108/+0x17C und KEINER auf +0x180..+0x18C.
=> Das RE2-Baby spielt immer **Paar 1 Clip 0**: 2 Bilder (Keyframes 0/1), Wurzel (0,0,0), Vertex-Y -275..0 = steht auf
seiner Lage. Paar 3 ist die Opfer-Bank (Wurzel -1797 = Brusthoehe des Spielers) und wird vom Baby nie abgespielt.
**Dynamischer Gegenbeweis im RE2-Original: nicht erreicht** (versucht, Dossier 6): DuckStation-GDB mit
`SLUS-00748_5.sav` + Raumsprung per RAM nach ROOM2010 (`R_belege/re2_gdb_baby.py`) laedt den Raum, die Spinnen
stehen dort aber im MAIN-Teil der SCD (Unterskript 11 @Datei 0x1088, Saetze @0x10EE/0x1104/0x111A) und erscheinen
erst auf ein Ereignis — Entity-Tabelle 0x800CFE18 blieb leer; Haltepunkte im Entity-Takt (0x800268A4) beenden die
GDB-Verbindung (DuckStation trennt). PCSX-Redux (`R_belege/re2_baby.lua`/`re2_baby_run.py`) startet in dieser Sitzung
nicht (bekannt aus Runde 35 H: kein Monitor fuer GLFW). Der statische Beleg oben ist vollstaendig (alle Zugriffswege
auf die Paar-Zeiger aufgezaehlt).

## 3. Umsetzung

Neue Datei `re15_port/engine/src/spinnen_r38.c` + `re15_port/include/re15_spinnen_r38.h`; Haken (je 1-3 Zeilen,
Kommentar "Runde 38 Spur R"):

| Datei | Haken | Zweck |
|---|---|---|
| engine/src/enemy_ai_common.c | `re15_spinnen_r38_lauer_re2(e)` vor dem RE2-Spinnen-Tick | RE2-KI: Schachtspinne (grid 0x41) laeuft die RE1.5-Choreografie, danach RE2-INIT am Boden |
| engine/src/enemy_ai_common.c | Zustand 4 Unterzustand 1 -> `re15_spinnen_r38_lauer(e)` | RE1.5-KI: neun Phasen @0x80114ec8 (2.1) |
| engine/src/enemy_ai_common.c | INIT: `re15_aspider_anim(e)` | anim_set `jal 0x8001f314` @0x80110d60 (Halteposen-Bild 1) |
| engine/src/enemy_ai_common.c | Unterzustand 7: `re15_spinnen_r38_spucken(e,0)` | Spucken 0x0d002000 `jal 0x80019700` @0x80112770 |
| engine/src/enemy_ai_common.c | run_all: `re15_enemy_selfpush_from_player(s,e)` | Spinnen-Wurzel `jal 0x8002aec4` @0x80110a6c (RE1.5); RE2-Wurzel `jal 0x80035530` @0x8010016c -> FUN_80034d0c(other,self) schreibt self x/z @0x800350c8/cc |
| engine/src/enemy_ai_common.c | re15_actor_prop_pushout: Typ 0x25 -> `re15_spinnen_r38_objekt_pass` | FUN_8002cabc @0x8002be40 (2.5) |
| engine/src/enemy_ai_re2_spider.c | Unterzustand 8: `re15_spinnen_r38_spucken(e,1)` | RE2-Spucken FUN_801058A4 @0x8010101C (2.3) |
| engine/src/enemy_ai_re2_spider.c | Baby-Spawner: `dog_floor_y`/`floor` = Elternteil | +0x1C2 = Boden unter der Entity (RE2 @0x8002686c/74) |
| engine/src/enemy_ai_re2_spider.c | Biss: Reichweite 450 + 10 | Ziel+0x9A = 450 (RE2 @0x8003bdc0/c4), @0x80100E40 |
| engine/src/re2_ems.c | `re15_spinnen_r38_re2_bank(kind, eb)` in re2_ems_load_bank | **neu in der Fortsetzung**: Baby 0x26 = Paar 1 (2.6) |
| engine/src/re15_esp.c | `re15_spinnen_r38_routine_a(f)` | ESP-Routinen A 24 @0x80017f50 / 25 @0x80017fa4 (2.2) |
| engine/src/game_step_common.c | `re15_spinnen_r38_gift_tick(pl)` im Kommando-1-Prolog | Gift-Wirkung @0x80031e44-ecc |
| engine/src/scd_vm.c | Sce_em_set: pos_s_x/y/z := Spawnlage | Lage-Spiegel `sh` @0x800421a0-b8 (Vor-Hoehe von FUN_8002cabc) |
| platform/pc/main.c | `re15_spinnen_r38_log_frame` | Messschiene RE15_SPINNEN_LOG (env-gegatet) |

Konstanten (alle mit Adresse im Code): Phasentabelle @0x8010027c, Schub 200/300 (`ori 0xc8` @0x80114fa8 / `ori 0x12c`
@0x8011502c), +1000 @0x8011501c, Abstieg 100 + (rng & 0xf) @0x801150dc-e8 / @0x80115198-bc, Schwellen -1800/-500
@0x80115100/@0x80115188, Landung 1488 je Quadrant @0x80115278-310, Kaesten A/C @0x80118dd4/@0x80118dec, Spuck-Versatz
DAT_80118ee8 = (500,0,0) bzw. RE2 (1000,0,-100) @0x80105910-24, Routine 25: +30/+30 @0x80017fbc-d8, Treffer-Radius 500
@0x80017ffc, Kind `0x030c0000 | (+0x72 + 500)` @0x80018030-54, SE 0x03030001 @0x8001805c-60, Gift-Takt 0x78/8 und -2 HP
@0x80031e7c-ac, nie toedlich @0x80031ebc-c0, RE2-Spieler-Radius 450 @0x8003bdc0, EM025-Cliplaengen aus CDEMD0.EMS Blob 10.
**NUTZER-VORGABE / Beta->Retail (Gift):** RE1.5 wuerfelt das Gift-Bit tot (1.4: a0 = 0x0A in 13/13, Bit 0x2 @0x80012eac
nie gesetzt); RE2 vergiftet bei JEDEM Gift-Treffer (Op 32 `sh 5400,0x800cfe10` @0x80020130). Port: derselbe Treffer
(Giftklumpen im Spielerkasten, Routine 25) setzt das RE1.5-Gift-Bit 0x2; Wirkung und Heilung sind RE1.5 (2.2).

## 4. Messung nachher

Alle Laeufe ueber die echte exe (Messkopie `re15_pc_r38sp_mess2.exe`, RE2-KI = Default, `RE15_DEBUG_JUMP=<raum>@250`,
Messschiene RE15_SPINNEN_LOG, Bilder RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1). Laufordner `re15_port/build/r38_lauf/<name>`.

### 4.0 Unit-Test vor der Fortsetzung
`unit_r38_spinnen` auf Stand a7a84080: 0 FAIL (A RE1.5/RE2 Schacht Bild fuer Bild, A2 Landung neben dem Spieler, B Gift,
C Babys). Ergaenzt in der Fortsetzung: C "gezeichnet auf der Lage" (2.6) und D (Abschnitt 5).

### 4.1 Punkt 1 — Lage aller Stage-2-Spinnen und Schacht-Ausstieg (RE2-KI)
Je Raum 1300 Bilder ab dem Sprungpunkt (Leon steht), Auswertung je Spinne (`ana.py`-Zusammenfassung):
| Raum | Spinnen | y (alle Bilder) | Modus +0x222 | Neigung | Befund |
|---|---|---|---|---|---|
| ROOM2030 (Sprung, Cut 0) | 3 x grid 0x00 floor 3 | -5400 = Steg (Leon -5400) | 0 | 0 | am Boden; Spucken + Gift |
| ROOM2030 (ECHTE Tuer 2060 -> 2030, Cut 13) | 1 x floor 9 | -16200 = oberer Steg (Leon -16200) | 0 | 0 | am Boden (Bild `R_belege/2030_oben_tuer.png`), Gift |
| ROOM2050 | 2 x floor 3 | -5400 | 0 | 0 | am Boden |
| ROOM2070 | 2 x floor 0 | 0 | 0 | 0 | am Boden, greifen an |
| ROOM2090 | 2 x floor 0 | 0 | 0 | 0 | am Boden, greifen an |
| ROOM2060 | 2 x grid 0x41 floor 2 | -3600 | 0 | 0 | warten im Schacht (Zustand 4/1), bis Cut 1/5 |
| ROOM20A0 | grid 0x41 floor 3/2 | -5400/-3600 | 0 | 0 | warten im Schacht, bis Cut 4/5 |
Alle Spinnen ausser den Schachtspinnen stehen in jedem Bild auf ihrer Sce_em_set-Hoehe = Boden des Stegs (y = +0x1ba).
Elza-Raeume ROOM20x1: Sce_em_set/Member_set-Zeilen bytegleich mit ROOM20x0 (md5 der Zeilen: 2030/2031 d4ad8ac5,
2050/2051 bbb2dd03, 2060/2061 bc29823b, 2070/2071 8a3c7569, 2090/2091 edfe6258, 20A0/20A1 d5eb059c).
**Schacht ROOM2060, echter Weg** (`w_2060`: Spawn an der Tuerlage aus 2030 (7600,0,3950), zu Fuss in Cut 1): Bild 19
grid 0x41 -> 0x42 (sub02), Kriechbahn z 6400 -> 4325 mit x 5975 (Gitter-Ausschiebung), Bild 49 y -2600 / Neigung 1024,
Abstieg -1545, -462, Bild 89 Landung **(5975,0,3137) floor 0**, Bild 99 RE2-INIT Zustand 1 Modus 0 auf **y 0**.
**Schacht ROOM20A0, echter Weg** (`w_20A0`: Lauf durch den Gang, an der Kante x 20082 SQUARE = Klettern nach unten auf
y 0, damit Cut 3 -> 4): Bild ~159 grid 0x42, Kriechen z -29000 -> -26850 (x 22732 = Gitter obj 2 + 200), Abstieg,
Bild 239 Landung **(22732,0,-25194)** (Original 1.4: (22732,0,-25662), dort mit erzwungenem Cut ohne Spieler-Weg), danach
Angriff auf Leon (HP 100 -> 80 -> 60 ...). Bilder ROOM2060 (`w2_2060`, derselbe Weg, Bild 12..132 je 8):
`R_belege/2060_schacht.png` — Cut 0 an der Tuer, Cut 1: die Spinne kriecht aus dem Deckenschacht am hinteren Gitter,
haengt senkrecht an der Wand (Neigung 1024), steigt ab und steht ab Bild ~92 neben Leon auf dem Boden.

### 4.2 Punkt 2 — Gift -> Vergiftung -> Gegengift (echte exe)
* Spucken sichtbar (`s_2030`, Bild 8-34 je 2): violette Giftwolke fliegt von der Spinne zu Leon (`R_belege/spucken_2030.png`);
  Messschiene: 3 Effektplaetze 0x0d (A 24 -> 25), Treffer in Bild 33: HP 100 -> 90, Zustand 2, **Status 0x0002 (Gift)**.
* Wirkung: danach alle 121 Bilder -2 HP (v_2030: 1268 von 1300 Bildern vergiftet, HP 100 -> 32 durch Spucken + Gift).
* Statusschirm (`g_2030`): EKG zeigt **"Poison"** (violett); Gegengift (RE15_GIVE=0x23:1, Platz 2) per Menue ITEM ->
  Antidote Spray -> USE: Bild 440 **HP 86 -> 111 (+25, ohne Deckel = RE1.5 @0x8004afa8-b0), Status 0x0002 -> 0x0000**,
  EKG wechselt auf **"Fine"** (gruen), das Spray verschwindet aus der Liste (`R_belege/gegengift_2030.png`).
  Danach spuckt die Spinne erneut (Bild 640: HP 101, Status 0x0002).

### 4.3 Punkt 3 — Minispinnen am Boden mit Angriff (echte exe)
`c2_2030`: Spinne 1 mit der Schrotflinte getoetet (RE15_GIVE=8:30, RE15_EQUIP=8, Zielen+Feuer), Bild 176: 9 Babys
(Typ 0x26, RE2-Spawner) auf dem Steg: y = -5400 = Boden in 544-570 von 585 Bildern, sonst Sprung (Modus 1, Scheitel
-6680 = 1280 ueber dem Boden, RE2-Sprung vy -300/+40); nie unter dem Boden. **Gezeichnet**: vorher (`c_2030`, vor 2.6)
Klumpenkette auf Gelaenderhoehe, nachher auf dem Steg zu Leons Fuessen (`R_belege/babys_vorher_zoom.png` /
`babys_nachher_zoom.png`). **Angriff**: HP-Abzuege in 760 Bildern: 13 x 1 HP (Bisse), 5 x 2 HP (Gift), 1 x 10 (Spucken);
der Lauf vor der Fortsetzung (`c_2030`, 1500 Bilder): 28 Bisse.

## 5. Tests

## 6. OFFEN

## 7. Fuer den Nutzer
