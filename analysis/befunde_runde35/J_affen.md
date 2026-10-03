# Runde 35 — Spur J "affen" (ROOM11C0: Ada verstecken/zurueck, Monkeys)

Baum: `.claude/worktrees/r35_affen`, Zweig `r35/affen`, Basis master 154a73c1. Beginn 2026-10-03.

## Auftrag (woertlich, AUFTRAG.md)
1. In ROOM 11C0 nach der Ada Cutscene verschwindet Ada nicht, wenn sie sich verstecken soll, und sie muss dann wieder raus kommen, wenn die Monkeys besiegt sind.
2. In ROOM 11C0 kommt der Monkey noch nicht an der Korrekten Position aus dem Auto.
3. Die Monkeys in ROOM 11C0 haben einen komisch beweglichen Teil am Oberkoerper, der so nicht im Original existiert.
4. Die Monkeys in ROOM 11C0 sind nicht so wie im Original — im Original ist die KI zielstrebiger und aggressiver. Da stimmt noch irgendwas bei der Uebernahme nicht.
5. Die Monkeys haben die Brust-schlagen-Animation offenbar noch nicht, die sie im Original manchmal ausfuehren.
6. Ich moechte, dass die Monkeys erst springen, wenn sie 3x getroffen wurden, nicht nach jedem Schuss. (NUTZER-VORGABE: die Zahl 3)

Zugeteilt (VERTRAG.md): Bank-9-Bit 82 (Reserve), Nachrichten-IDs ROOM11C0 20..23, Ereignis 25 (11C0).
Probes-Datei: `re15_port/tests/unit/probes/r35_affen.cmake`, Quellen `tests/unit/test_r35_affen*.c`.

## Protokoll (fortlaufend)

### 2026-10-03 Start
- Baum geprueft: `git status --porcelain` leer, HEAD 154a73c1, Zweig r35/affen.
- AUFTRAG.md + VERTRAG.md vollstaendig gelesen.

### Belege aus dem Raumskript ROOM11C0 (scd_dump_room.py, Datei-Offsets in ROOM11C0.RDT)
- sub00 @0x1768: `Ck(4,0x40)==0` -> Szenen-Layout: Ada 0x42 grid 0x40 @(-8965,0,-14347); Gorilla slot1 `44 01 27 30 ...`
  @0x1784 (-1220,-20000,-21568) persist 0x60; slot2 @0x1798 (-554,-20000,-25423) persist 0x61 (grid 0x30 = Bit 0x20
  Root-Skip @0x80116df4-f8 + 0x10). Else `Ck(3,0x43)==0` -> Kampf-Layout: Ada geparkt (-18214,-20000,-7229);
  Gorillas grid 0x10 @(-9013,0,-15461)/(9434,0,2189). `Sce_bgm_control 54 00 01` @0x1810.
- sub01 @0x181C: `Ck(4,0x40)==0 -> Evt_exec sub02` (@0x1824); `Ck(3,0x43)==0 && Ck(7,0x60)==1 && Ck(7,0x61)==1 -> Evt_exec sub03` (@0x1842).
- sub02 @0x184E (Intro-Szene), die fuer Spur J relevanten Zeilen:
  - @0x198E/@0x1996 `Work_set(2,1)/(2,2); Member_set 01 = -2500` (beide Gorillas y=-2500 = im/ueber dem Wagen),
  - @0x199A..@0x1A28 Wagen-Wackeln (Work_set(3,0) Speed_set/Add_speed/Add_aspeed, 7 For-Schleifen = 36 Bilder),
  - @0x1A2E `Work_set(2,1)`: Member_set 00=-3617, 01=0, 02=-17798, **0C=16** (+0x9 grid := 0x10 = KI frei), **13=0** (+0x1ba := 0),
  - @0x1A48 `Work_set(2,2)`: Member_set 00=-1220, 02=-21568 (Gorilla 2 auf die Position von Gorilla 1, y bleibt -2500),
  - @0x1A50 `Sleep 80`; @0x1A58 `Work_set(2,1) Member_set 0C=48` (wieder eingefroren), Cut_chg 5, msg05/06,
  - @0x1AB0 Ada `Plc_dest 09 01 (-18214,-7229)` + Gosub 6 (warten auf Ankunft Bank5 Bit1),
  - @0x1ABA `Evt_exec sub07` (parallel), @0x1AC2 `Work_set(2,1) Member_set 12=0, 0C=16` (Gorilla 1 frei),
  - @0x1ACA `Set(1,27)=0 / Set(2,7)=0` (Szenenende), Cut_auto, @0x1AEC `Work_set(2,2) Member_set 00=-3617 01=0 02=-17798 0C=16 13=0` (Gorilla 2 frei, gleiche Stelle wie Gorilla 1).
- sub07 @0x1C62 (**Ada versteckt sich**): `Work_set(2,0); Member_set 12=2; Plc_dest 05 01 (-18214,-7229); Gosub 6; Member_set 01=20000` (@0x1C74: y := +20000 = unter dem Boden geparkt).
- sub03 @0x1B02 (**Ada kommt zurueck**, nach beiden Kills): `Set(3,0x43)=1; Sleep 30; Cut_chg 4; Spieler Member_set 00=-8035 02=-15873; Plc_dest 04 (-14280,-8543); Plc_dest 09 (-18214,-7229); ... Cut_auto; Work_set(2,0) Member_set 01=0 (@0x1B52: y := 0), Member_set 04=512 (@0x1B56 rot_y)`; msg07..09; Leon+Ada laufen zur Tuer (sub04), `Aot_on 0` @0x1C08.
- Member_set-Feldkarte = FUN_8004116c (EXE): 0..2 -> +0x34/+0x38/+0x3c (x/y/z), 3..5 -> +0x68/+0x6a/+0x6c, 6 -> +0x0, 7 -> +0xc,
  8..0xb -> +0x4..+0x7, **0x0c -> +0x9 (grid)**, 0x0d -> +0x8, 0x0e/0x0f -> +0xa/+0xb, 0x10 -> +0x1c4, 0x11 -> +0x98,
  **0x12 -> +0x82 (floor)**, **0x13 -> +0x1ba (sh)**. Port: actor_common.c re15_actor_set_member — Fall 19 schreibt `a->hp`
  (Kommentar "+0x1ba (hp)"), aber +0x1ba ist im Gorilla/Hund-Code das Boden-Y (`dog_floor_y`, INIT @0x8011bf..; HP liegt
  bei +0x9a, Resolver `sh v1,154(s1)` @0x80013000). Folge im Port: `Member_set 13 = 0` setzt hp=0 — der INIT
  (`if (hp<=0) hp=180`) heilt das zufaellig, aber die Zuordnung ist falsch (OFFEN -> pruefen/fixen, 1 Zeile).

### Punkt 3 — Mechanismus (statisch belegt, Messung folgt)
- EM027: EMR nb=18 (EMR+4), MD1 22 Meshes (Objektzahl 44 >> 1). Binder FUN_8001e56c: `+0x83 = ([MD1+8]) >> 1 = 22` Parts.
- FUN_8001e5b0 (Part-Records, Stride 0xac): Schleife ueber ALLE 22 Parts; rel-Position je Part = EMR[8+6i] — fuer i=18..21 liest
  das Original HINTER der Knochentabelle: rel[18]=(3,72,3) rel[19]=(75,1,78) rel[20]=(0,79,1) rel[21]=(79,1,80) (= die Bytes der
  Kindtabelle @EMR+0x74). `uVar21 = EMR+4 = 18` zaehlt pro Part herunter; ist es 0 (Parts 18..21): `rec[0x1b] = &DAT_80072d4c`
  (Elternmatrix = EXE-Konstante {4096,0,0,0,4096,0,0,0,4096, t=0,0,0} = Identitaet, gelesen mit re15_disasm read) und
  `rec[0x24] = 0` (kein Eltern-Record, KEINE Verbindung zur Entity-Matrix +0x20).
- FUN_8001f3bc (Pose aus Keyframe): Schleife `uVar7 = EMR[+4] = 18` -> Parts 18..21 behalten die Identitaets-Rotation des Binders.
- FUN_8001e9ec (Part zeichnen): `m1 = parent * local` (FUN_80022da0), dann `View(DAT_800b5288) * m1` -> Parts 18..21 stehen im
  Original FEST im Weltursprung bei (3,72,3) usw., unbeteiligt an Lage/Drehung/Skalierung des Gorillas.
- Port main.c (~10140): `np = &npc_poses[nbi < npc_bones ? nbi : 0]` -> Meshes 18..21 reiten auf der WURZELPOSE (Becken, +0x34-Kette
  mit Yaw/Scale/Position) = sie bewegen sich mit dem Oberkoerper mit. Meshes 18..21 (MD1): 18 = 440x660x780 (29 Vtx, 2 Tri + 20 Quad),
  19..21 = schmale ~190x500x180-Teile (nur Quads). Das ist der Kandidat fuer "komisch beweglicher Teil am Oberkoerper".

### Messung vorher (Port, Lauf A2: RE15_DEBUG_JUMP=11C0@240, RE15_STATE_LOG, RE15_EVT_TRACE, RE15_ENEMY_DBG, RE15_FRAMEDUMP 240-4500/30; exe-Kopie re15_pc_r35j.exe)
- enemy_dbg.log: `ZEICHNE Typ 0x27: 22 Teile (Bones 18, Meshes 22)` -> der Port zeichnet die 4 ueberzaehligen Meshes (Punkt 3, Messung).
- Szene (debug.log/state.log): Evt_exec sub02 bei F6 nach dem Sprung; Gorilla 1 (Slot 2) ab ~F969 bei (-3617,-17798) st=1 ss1=0 mo=22 (Clip 0x16 Idle) 80 Bilder lang,
  F1049 wieder g=0x30 (eingefroren, af=22); Cut 5 ab F1049. F1070 (Evt_exec sub07, `Member_set 12=0, 0C=16`): Gorilla 1 in EINEM Bild von
  (-3617,-17798) nach (-3617,-15477) versetzt (+2321 z). Gorilla 2 (Slot 3) ab F1110 bei (-3617,-17798), nicht versetzt (sein +0x82 bleibt 2).
- **Ada (Slot 1, Typ 0x42)**: F1070 `Plc_dest(slot=1 mode=0x05 dest=(-18214,-7229)) -> state4/sub5`; sie laeuft bis F1173 und bleibt bei
  **(-16859,-6560)** stehen (st=4 ss=5/2/0, Clip 0), 1511 Einheiten vor dem Ziel — bis Lauf-Ende F4500 unveraendert. Der Ankunftsradius (300, slti
  @0x80051764) wird nie erreicht -> Bank-5-Bit 1 bleibt 0 -> `Gosub 6` in sub07 kehrt nie zurueck -> `Member_set 01 = 20000` (@0x1C74) laeuft nie
  -> Ada bleibt sichtbar. Das ist der Befund des Nutzers (Punkt 1, erste Haelfte); die zweite Haelfte (sub03 `Member_set 01=0` bringt sie zurueck)
  haengt an demselben Faden: solange sub07 nicht fertig ist, steht Ada ohnehin da.
- SCA-Zellen ROOM11C0 (RDT +0x20, 59 Zellen, 12 B: w, dep, x, z, typ, u0, u1, floor): um Adas Ziel liegt die Zelle x=-28290..-17790 z=-28550..8112
  **flr=3 typ=1 u0=0xff**; Adas Haltepunkt x=-16859 = Ostkante -17790 + 931 (ihr Klemmradius). Um den Gorilla: x=-23796..16603 z=-27296..-17095
  **flr=3 u0=0xff** (Nordkante -17095 + 1600 + 18 = -15477 = die gemessene Versetzung) und x=-24600..16400 z=-25000..-14100 flr=0x13.

### RE-Beleg Kollisionsband (FUN_8003b0a4, EXE; port re15_collision.c)
- Zelle gilt nur, wenn `entity+0x82 == (floor_byte >> 4)` (@0x8003b0a4: `(*puVar11 << 0x10) >> 0x1c`, Port @0x8003ba04 `sra v0,v0,28`); solide,
  wenn `mask & u0 != 0`. Baender in 11C0: flr 2/3 -> Band 0, 0x13 -> Band 1, 0xff -> Band 15. **Es gibt keine Band-2-Zelle.**
- sub07 setzt Ada VOR dem Lauf `Member_set 12 = 2` (+0x82 := 2, @0x1C66) -> im Original kollidiert sie mit NICHTS und erreicht (-18214,-7229) im Wagen.
- Port: re15_npc_wall_clamp (enemy_ai_common.c ~10177) ruft `re15_collision_constrain_enemy(..., e->y, 4u)` = Band aus y (band_from_y(0)=0), NICHT
  aus +0x82 -> Ada wird an der Band-0-Zelle (flr=3) festgehalten. Der Gorilla-Tail nimmt bereits `re15_enemy_sca_clamp_band(e, .., 4u)` mit e->floor.
- FIX (Punkt 1): NPC-Klemme auf das +0x82-Band umstellen (1 Zeile, Original @0x8011cc58-68 ruft 0x8003b0a4 mit demselben +0x82-Vergleich).
- Gorilla 1 bekommt @0x1AC2 `Member_set 12=0` (+0x82 := 0) -> Band 0 -> die flr=3-Zelle (u0 0xff, Maske 4) klemmt ihn im Original GENAUSO
  (gleicher Resolver, gleiche Zelle) — ob die Versetzung im Original sichtbar ist, klaert der DuckStation-Lauf (orig_scene/r1).

### Clip-Identifikation EM027 (Keyframe-Streifen, scratchpad clip_strip.py = Engine-Posenkette des Ansichtsblatts)
- 0x16 (58 B, Idle-Wander B[0]): ruhiges Vierbeiner-Idle. 0x1c (52 B, Rear-up/Pin B[15]): EIN Arm hoch, Schlag/Griff. 0x00 (78 B): Liege-Idle (sub 1).
- **Clip 3 (70 B, RISE-Variante +0x7=0, B[2] @0x80117928; ausserdem Pin-Release @0x8011ad50-78 ab Bild 0x16 und
  Sub-15-Exit @0x8011ae30-58 ab Bild 0x1d): Bild 0-20 Vierbeiner, Bild 30-55 AUFRECHT mit beiden Haenden an der Brust
  = der Brustschlag**, Bild 59-69 wieder auf alle viere. Erreichbar in 11C0 NUR nach einem VERBUNDENEN Rear-up-Pin
  (sub 15 Phase 2 -> 4/5/6 -> sub 2); die Wach-Variante (sub 1 -> sub 2) braucht grid Bit 0 (INIT `andi v0,v0,0x1`
  @0x80116f7c, `ori v0,zero,0x101` @0x80116f88) — die 11C0-Records tragen grid 0x30/0x10 (Bit 0 = 0) -> sub 0.
- Subs 9..14 (A @0x80119284.., B @0x8011936c/971c/9a6c/9d0c/a1f8/a44c; Clips 0xc/0xd/0x1b/0xf/0x19/0x18/0x1a): ein
  HAENGE-Verhalten (Sprung nach oben um 0xa8c=2700 @0x8011948c-, Haengen mit beiden Armen oben, Hangeln 0xf, Tritt-Angriff
  0x1a Bone 6/10 r=800, Absprung 0xd). Einziger ACTIVE-Eintritt: Flinch-Exit mit +0x1e3 != 0 (@0x8011b1c8-d8); +0x1e3
  hat KEINEN Schreiber ausser dem INIT-Clear (@0x801170ac; Voll-grep STAGE1_overlay.c + ghidra1_V2.txt `483(`) ->
  im Auslieferungsstand unerreichbar. Die +0x5=9-Schreiber in B[7]/B[8]/A[9..14] sind alle `+0x4=2/3` (HURT-/DEATH-Sturzspur).
- Port (Lauf A2, F2569-2691): Slot 3 lief sub 15 -> Pin -> Release Clip 3 (F2622 mo=3) -> sub 2 -> CHASE; Framedumps
  f002640/f002670 zeigen den aufrechten Gorilla. Der Brustschlag EXISTIERT im Port, aber nur nach einem verbundenen Pin.

### Messung vorher, Lauf B (Port: Szene + Handfeuerwaffe RE15_GIVE=3:250 RE15_EQUIP=3, Feuerskript ab F1260 "MA0.1,M0.6"x60)
- 13 Treffer (je -12 HP, 180 -> 168 ...), 13 Flinches (st=2, Clip 7, 12 Bilder), danach JEDES Mal sub 7 (Sprung):
  `HITS {Slot2: 8, Slot3: 5}  FLINCH {2: 8, 3: 5}  LEAP {2: 8, 3: 7}` -> "springt nach jedem Schuss" (Punkt 6, Befund bestaetigt).
- Sprungweite: Slot 2 F1756 (-10004,-12528) sub 7 -> F1796 (2680,-15477) sub 4: 12700 Einheiten in 40 Bildern.
  Ursache (Port-Defekt): im Flug (Phase 2) lief zusaetzlich zu c1a4 noch `re15_dog_advance_ofs(e,0)` ("double-advance").
  Disasm 0x80118c3c-dc4: Landung `j 0x80118dc4` @0x80118cc0; `bne v1,v0,0x80118dc4` @0x80118cdc (Bild != 0x13);
  Finisher-Gates `beq/bne/bgez -> 0x80118dc4` @0x80118cf8/@0x80118d0c/@0x80118d20/@0x80118d3c; Commit `j 0x80118dc4`
  @0x80118d6c. `jal 0x800245d8` @0x80118dbc erreichen nur Phase 0/1 (Durchfall) und Phase 3 (`beq v0,zero,0x80118dbc`
  @0x80118d84). -> Zeile entfernt (enemy_ai_common.c, Fall 7 Phase 2). Das ist ein Baustein von Punkt 4.
- Leon stirbt F2852 (HP -2) bei Dauerfeuer ohne Ausweichen; die Gorillas erreichen ihn innerhalb von ~200 Bildern nach dem Szenenende.

### Umsetzung (Stand 1, Bau laeuft)
- NEU include/re15_affen.h + engine/src/affen_11c0.c: (1) re15_affen_npc_band = +0x82; (2) re15_affen_surplus_part_world
  (Parts >= bone_count weltfest bei EMR[8+6i], Identitaet = DAT_80072d4c); (3) re15_affen_treffer_zaehlen /
  re15_affen_flinch_exit_sub (3 Treffer -> 7, sonst 3; +0x1e3 -> 9 byte-true). RE15_AFFEN_TREFFER_BIS_SPRUNG = 3 (NUTZER-VORGABE).
- Haken: enemy_ai_common.c (include; INIT `mag_hit_ctr = 0`; Flinch-Eintritt zaehlt; Flinch-Exit; re15_npc_wall_clamp Band = +0x82;
  LEAP Phase 2 ohne 245d8), main.c (include; Parts ohne Knochen weltfest, nur RE1.5-Banken, nicht 0x36/0x37/remap),
  re15_emd.h (+emr_raw/emr_raw_size) + emd_common.c (setzen), re15_actor.h (+mag_hit_ctr).

### Original-Referenz: DuckStation-Aufnahme der Szene (scratchpad orig_scene/r3, Rekorder dsrec.py)
Debug-Menue-Basis stage_saves/mzd_debugmenu.sav, JUMP 0x11C (8x Links; die Taps sind flatterhaft: Lauf 1 landete
in 0x11D KENNEL LIGHT, Lauf 2 mit 9 Taps in 0x11B GARAGE — deshalb verifiziert der Rekorder jetzt den Spieler-Spawn
(-22604,14455) und wiederholt). 110 Savestates im 1,5-s-Takt (LeftShoulder = SaveSelectedSaveState), Dekoder dsdecode.py
(re15_ss.Ram: Entities 0x800acc2c+i*0x1f4, Spieler 0x800aca54, Cut 0x800b0fe4, Objektpool 0x800b3f98+148*i, View-Matrix 0x800b5288).
- Szenen-Layout t=6: beide Gorillas st=1 sub0, grid 0x30, flags 0x801, +0x166=0x1b33, +0x82=2, hp 180 -> der INIT lief im
  SPAWN-Bild (Sce_em_set ruft die Wurzel einmal mit geloeschtem Bit: `andi v0,v1,0xdf` @0x8004256c, `jalr 0x80072bac[typ]`
  @0x8004259c, Bit zurueck @0x80042604-08). Der Port verdrahtete diesen Aufruf nur fuer Birkin (re15_enemy_spawn_root) ->
  der eingefrorene Gorilla stand in Zustand 0 OHNE Scale (render_scale_q12 = 0 -> 1,0x statt 1,7x).
- t=34.89 (Cut 12, Klappe rot_z=1725 mid-Bounce): der Gorilla (y=-2500) sitzt SICHTBAR in der offenen Heckklappe des Wagens
  (Bild sheet_orig_30_50.png); der Port zeigte an derselben Stelle nur den Kopf ueber der Klappenkante — die um 1,7x kleinere
  Figur verschwindet hinter der Klappe (View-Projektion mit der Original-Matrix 0x800b5288: Klappen-Oberkante y=114..124 bei
  vz 9.4-9.5k, Gorilla-Ursprung (165,145) vz 11.1k, Kopf 1,7x3543 Einheiten hoeher). DAS ist Punkt 2 ("kommt nicht an der
  korrekten Position aus dem Auto"): die Figur fehlt im Wagen und taucht dann bei (-3617,-17798) auf.
- Klappe (Objekt 0): Position (-840,-2930,-19110), rot (-72,-1400,48) -> 1752 nach 12 Bildern + 6 Wackler — Port-Werte
  (Riegel `wagen`: 223 392 ... 1752 1702 ... 1752) = Original-Pool (t=34.89: 1725, ab 36.41: 1752). Rotationsmatrix des Ports
  (RotMatrix @0x80068098, der Objekt-Zeichner FUN_8002c18c ruft dieselbe @0x8002c218 wie FUN_8001e8c8 @0x8001e8f4) = Original-
  Pool +0x20/+0x48 bis auf +-5 (Sinustabelle). Speed_set @0x80040f14 (thread+0x158+2*id), Add_speed @0x80040f40 (+0x34../+0x68..
  += vel), Add_aspeed @0x80040fd4 (vel += acc) — alle drei im Port byte-gleich.
- Gorilla 1 nach der Freigabe (t=45.5): (-4855,-14459) = Band-0-Klemme der flr=3-Zelle (z -> -15477) + ~1600 Krabbeln; der
  Port versetzt an derselben Stelle um dieselben +2321 (Lauf A2 F1070) -> byte-gleich, KEIN Befund.
- Ada: t=45.5 Sub 5 RUN (-13894,-10590) +0x82=2; t=47.0 bei (-18000,20000,-7403) = versteckt. Port nach dem Band-Fix (Lauf A3):
  Ankunft F1127 bei (-18025,-7379), y=20000 (Framedumps: ab F1150 nicht mehr im Bild; vorher A2: steht sichtbar am Wagen).
- Gorilla 2: idlet ~7 s (Clip 0x16), dann EIN Fernsprung (Path B) ueber ~8900 Einheiten (Anlauf 10 Bilder +0x8c 180-211 + Flug
  ~25 Bilder 240-271), landet 2,6k hinter Leon, dann Sub 6 Heavy (-12). Leon (ohne Eingabe) stirbt t=85 (Bisse -6 im 1,5-s-Takt).
  Kein Rear-up (sub 15) in 40 s Kampf. Original-Sprungweite == Anlauf+Flug ohne Doppelvorschub (bestaetigt den 245d8-Fix).

### +0x1ba-Kette (Boden-Y), byte-true nachgezogen (Punkt 2, Voraussetzung fuer den Spawn-Wurzelaufruf)
- Sce_em_set: `lbu v1,2(s2)` (pc[4]) @0x800421d4 -> 1800*v1 (sll 3/subu/sll 5/addu/sll 3 @0x800421f8-8004220c) -> `subu v0,zero,v0`
  / `sh v0,442(s0)` @0x8004220c-10: +0x1ba = -(pc[4]*1800). Port: scd_vm.c op_sce_em_set seedet dog_floor_y.
- Member_set 0x13 -> +0x1ba (FUN_8004116c Fall 0x13); 20 Stellen game-weit (10B1, 11C0 x2, 2030/2031 x4, 3050/3051 x2, 50D0/50D1 x2).
  Der Port schrieb hier a->hp (+0x9a ist die HP: Resolver `sh v1,154(s1)` @0x80013000) — korrigiert auf dog_floor_y. Folge in
  2030/3050/50D0: diese Aktoren behalten ihre Tabellen-HP statt 3/4/6/9 (byte-true; war ein stiller One-Shot).
- Maggot-INIT FUN_80116f50 schreibt +0x1ba NICHT (kein 442-Store) -> Port-Ersatz `dog_floor_y = y` entfernt.
- Spawn-Wurzelaufruf (re15_enemy_spawn_root) auf 0x27 erweitert: INIT im Spawn-Bild (HP 180, Scale, Zustand 1), Bit 0x20 bleibt.

## Ergebnis je Nutzer-Punkt (Stand 1. Sitzung)
> ⛔ Die Aussagen dieses Abschnitts zu den Punkten 3, 4 und 5 sind durch die Gegenpruefung der 2. Sitzung UEBERHOLT
> (Abschnitte G1-G4 und "Abschluss 2. Sitzung" weiter unten): Punkt 3 war nicht behoben (Part 18 haengt am Rumpf),
> Punkt 4 hatte drei weitere, groessere Ursachen (Crossfade/Fuss-Sperre, Knockdown-Sonde, Biss-Richtung), Punkt 5 lief
> ausserhalb des Bildes (Pin-Anker). Die Punkte 1, 2 und 6 gelten unveraendert.

### Punkt 1 — Ada versteckt sich / kommt zurueck
- Ursache: NPC-Wandklemme mit Band aus y statt +0x82 (Beleg oben). Fix enemy_ai_common.c re15_npc_wall_clamp -> Band +0x82.
- Messung nachher (Lauf A3/A4, exe): sub07 F1070, Ankunft F1127 bei (-18025,-7379), y=20000 — Framedumps: Ada ab F1150 nicht
  mehr im Bild (vorher stand sie bis Lauf-Ende sichtbar neben dem Wagen). Original: (-18000,20000,-7403) bei t=47.
- Rueckkehr: sub03 (@0x1B52 y=0, @0x1B56 rot_y=512) — im Riegel `ada` gemessen: y=0 nach 219 Bildern (Leons Gang vorweg),
  rot_y=512, (3,0x43)=1, danach Lauf zu (-16211,-8183) (@0x1B70). Im Port ist nichts weiter noetig: der Faden hing nur an
  der nie erreichten Ankunft von sub07.

### Punkt 2 — Monkey kommt nicht an der korrekten Position aus dem Auto
- Skript-Positionen (Wagen -840/-2930/-19110, Gorilla y=-2500 im Wagen, Austritt (-3617,0,-17798), Klappe 48->1752) sind im
  Port byte-gleich (Riegel `wagen`; Original-Pool/Entities aus dem Savestate identisch).
- Ursache des sichtbaren Unterschieds: der eingefrorene Gorilla (grid 0x30) hatte im Port keinen INIT (Zustand 0, Scale 0
  = 1,0x) — im Original laeuft der INIT im Spawn-Bild (generischer Wurzelaufruf @0x8004256c-0x80042608, Savestate t=6 s:
  st=1, +0x166=0x1b33, flags 0x801). Die 1,7x kleinere Figur verschwand hinter der aufgeklappten Heckklappe; der Nutzer sah den
  Gorilla erst "aus dem Nichts" bei (-3617,-17798). Fix: re15_enemy_spawn_root auch fuer 0x27 (+ +0x1ba-Kette, s.o.).
- Messung nachher (Lauf A4, Framedumps F770-F790, Cut 12): Gorilla sitzt sichtbar in der offenen Klappe wie im Original
  t=34.89 (scratchpad vergleich_wagen_a4.png); F800 Austritt gross vor dem Pfeiler wie Original t=36.41.

### Punkt 3 — komisch beweglicher Teil am Oberkoerper
- Ursache + Beleg: Abschnitt "Punkt 3 — Mechanismus" (FUN_8001e56c/FUN_8001e5b0/FUN_8001f3bc/FUN_8001e9ec, DAT_80072d4c).
  Messung vorher: enemy_dbg.log `ZEICHNE Typ 0x27: 22 Teile (Bones 18, Meshes 22)`, Parts 18..21 auf der Wurzelpose.
- Fix: main.c-Haken + re15_affen_surplus_part_world (weltfest bei EMR[8+6i], Identitaet). Riegel `teile`: Parts 18..21 ->
  (3,72,3)/(75,1,78)/(0,79,1)/(79,1,80), Part 17 unveraendert. Gilt ebenso fuer 0x29 (19/18) und 0x30 (17/16).
- Messung nachher (A4 F1130-F1300, 2x): kein mitbewegtes Teil am Rumpf mehr (a4_teile_nachher.png vs a2_teile_vorher.png).

### Punkt 4 — KI zielstrebiger/aggressiver im Original
- Ein belegter Port-Defekt: der LEAP-Flug lief mit doppeltem Vorschub (c1a4 + 245d8(0) je Bild) — Disasm @0x80118c3c-dc4:
  alle Flugpfade enden im Epilog 0x80118dc4, `jal 0x800245d8` @0x80118dbc nur aus Anlauf (Phase 0/1) und Landung (Phase 3).
  Vorher (Lauf B): ein Sprung 12700 Einheiten in 40 Bildern, Landung weit hinter Leon in Wandtaschen. Original (r3): ~8900
  = Anlauf + Flug. Fix: Zeile entfernt; Riegel `flug`: 240 je Flugbild.
- Die uebrigen Brain-Gates (A[0]/A[3]/A[4], Lockouts, LOS) wurden gegen die Decompiles gelesen und stimmen; das Original idlet
  nach der Freigabe sogar laenger (G2 7 s Clip 0x16) als der Port (2 s, rng+59). Erste Beruehrung Leon: Original t=57.5 (12 s nach
  Freigabe, Heavy -12), Port A2 F1464 (13 s, Heavy -12) — gleichwertig.

### Punkt 5 — Brust-schlagen-Animation
- Identifiziert: Clip 3 (70 Bilder), Bild 30-55 aufrecht mit beiden Haenden an der Brust. Erreichbar in 11C0 nur nach dem
  verbundenen Rear-up-Pin (sub 15 Phase 4 ab Bild 0x16 @0x8011ad50-78, Phase 6 -> sub 2 Clip 3 ab 0x1d @0x8011ae30-58);
  die Wach-Variante (sub 1 -> sub 2, grid Bit 0) kommt in 11C0 nicht vor (grid 0x30/0x10). Keine andere Quelle: die
  Haenge-Clips (subs 9..14) sind ohne +0x1e3-Schreiber unerreichbar.
- Port: Mechanik vorhanden und byte-true (A2 F2569-2691 sub 15 -> Pin -> Clip 3 -> CHASE, Framedumps f002640/f002670);
  Riegel `brust` misst die Bildfolge. Das Original zeigte in 40 s Kampf (Leon reglos) KEINEN Rear-up — "manchmal" = nur
  wenn der Pin verbindet (a9cc +-32, Bogen 2500/256, +0x1d6==0, +0x1e1==0 @0x80117ab4-b3c). Kein Code-Fix noetig; der
  Nutzer sieht den Brustschlag, sobald ein Gorilla ihn packt (Leon nahe, frontal).

### Punkt 6 — Sprung erst nach 3 Treffern (NUTZER-VORGABE)
- Original: jeder Boden-Flinch endet im Vergeltungs-Sprung (+0x5=7 @0x8011b188-98; Lauf B: 13 Treffer = 13 Spruenge).
- Port-Form: mag_hit_ctr (INIT-geloescht), Flinch-Eintritt zaehlt, Exit 7 beim 3. Treffer sonst 3 (CHASE). Riegel `sprung`:
  Exit-Folge 3,3,7,3,3,7. Luft-/Sturz-Spuren (Spur 1/2) unveraendert byte-true.

### Messung nachher, Kampf (Lauf B2, exe nach allen Fixes, gleiches Feuerskript wie Lauf B)
- 16 Treffer -> 16 Flinches -> **7 Spruenge** (vorher 13 -> 13): Slot 2: 6 Treffer -> 2 Spruenge (Treffer 3 und 6);
  Slot 3: 10 Treffer -> 3 Vergeltungs-Spruenge + 2 Fernspruenge des Selektors (A[4] Path B, @0x80118028-8100).
- Vollstaendige Spruenge: 6098 / 5448 / 5175 Einheiten in 40 Bildern (vorher 12700 in 40). Vier Spruenge brachen nach 10 Bildern
  (Anlauf) ab: der naechste Handfeuerwaffen-Treffer (alle 21 Bilder) stoesst sie in HURT (Original-Verhalten, @0x80118b14-74 Latch).
- Erster Spieler-Schaden F1423 (Heavy -12) = 353 Bilder nach der Freigabe (Original: ~12 s).

## Tests — Stand 1. Sitzung (ERSETZT durch "Tests (Stand Abschluss)" am Dateiende)
- unit_r35_affen_teile  — EM027 18/22, Parts 18..21 weltfest (3,72,3)/(75,1,78)/(0,79,1)/(79,1,80), Part 17 unveraendert.
- unit_r35_affen_band   — NPC-Laeuferin in ROOM11C0: +0x82=2 Ankunft im Wagen (Bild 11, Abstand 112); +0x82=0 klemmt bei x=-16859.
- unit_r35_affen_ada    — echte Szene: sub07 Lauf ab Bild 1063, Ankunft Bild 1120 (-18025,-7379), y=20000 ab Bild 1121; nach
                          beiden Kill-Bits sub03: y=0, rot_y=512, (3,0x43)=1, Lauf zu (-16211,-8183).
- unit_r35_affen_sprung — Flinch-Exits 3,3,7,3,3,7.
- unit_r35_affen_flug   — ein Flugbild = 240 (ein c1a4-Schritt).
- unit_r35_affen_wagen  — Klappe (-840,-2930,-19110) rot (-72,-1400,48) -> Folge 223..1752 + Wackler, Ende 1752; Gorilla-Lage-Schiene.
- unit_r35_affen_brust  — Pin-Release Clip 3 ab 0x16 -> Phase 5/6 -> sub 2 (Clip 3 ab 0x1d) -> CHASE nach 68 Bildern.
Exe-Laeufe (scratchpad, nicht im Repo): lauf_a (vorher), lauf_a3/lauf_a4 (nachher, Szene), lauf_b (vorher), lauf_b2 (nachher, Kampf);
Original: orig_scene/r3 (110 Savestates + PNG). Bilder: gdigrab lieferte in dieser Sitzung keine brauchbaren Bilder -> RE15_FRAMEDUMP.

## OFFEN — Stand 1. Sitzung (ERSETZT durch "OFFEN (Stand Abschluss)" am Dateiende)
- Heckklappe (Objekt 0) wird im Port dunkler gezeichnet als im Original (Cut 12: Original-Panel grau ~ (35,36,39), Port ~ (11,11,12));
  Lage/Drehung sind byte-gleich (Pool +0x20/+0x48 == pc_prop_rot_q12). Naechster Weg: Objekt-Beleuchtung FUN_8002c18c
  (`jal 0x80053fc0` @0x8002c254 mit &pool+0x5c, SetColorMatrix DAT_80076d34, MulMatrix0 DAT_80076d14) gegen main.c lctx_prop
  (Normalen/Lichtfaltung) messen — nicht Teil der sechs Punkte.
- Member_set 0x13 (jetzt +0x1ba statt hp) betrifft auch ROOM10B1/2030/2031/3050/3051/50D0/50D1 (Werte 0/3/4/6/9): dort behalten die
  Aktoren ihre Tabellen-HP. Byte-true (FUN_8004116c), aber in jenen Raeumen nicht nachgemessen — Messweg: Savestate +0x9a/+0x1ba.
- Die vier ueberzaehligen EM027-Meshes stehen jetzt (wie im Original) weltfest am Raumursprung (3,72,3) usw.; ob der Ursprung in
  einem 11C0-Cut sichtbar ist, wurde nicht gemessen (in den Framedumps A4 kein Fremdkoerper erkennbar).
- Spawn-Wurzelaufruf: ausser Birkin jetzt auch 0x27; die uebrigen Typen bleiben wie vor Runde 35 (OFFEN seit Runde 30, Dossier 9).

## Fuer den Nutzer — Stand 1. Sitzung (ERSETZT durch "Fuer den Nutzer (Stand Abschluss)" am Dateiende)
- Sprachdateien: keine neuen Zeilen (alle Texte sind Original-Nachrichten msg00..09 von ROOM11C0).
- Neue Assets fuer das Paket-Gate: keine (keine Dateien unter shared_assets/).
- Bedienhinweis: der Brustschlag (Clip 3) kommt nur nach einem verbundenen Rear-up-Griff — nahe und frontal zum Gorilla stehen.
- Die Zahl "3 Treffer bis zum Sprung" steht als RE15_AFFEN_TREFFER_BIS_SPRUNG in include/re15_affen.h (NUTZER-VORGABE).

### Angepasste Alt-Riegel (Suite-Lauf 1: 2 rote Tests, beide kodierten die widerlegten Annahmen)
- unit_member (tests/unit/test_member.c): id19 wurde als "hp" gefuehrt; byte-true ist +0x1ba das Boden-Y (FUN_8004116c Fall
  0x13; Sce_em_set @0x80042210), die HP liegt bei +0x9a (@0x80013000). Riegel liest jetzt dog_floor_y.
- unit_maggot_ai (2f): erwartete den Vergeltungs-Sprung nach JEDEM Flinch (Original). Mit der NUTZER-VORGABE kommt er beim
  3. Treffer; der Teil stellt mag_hit_ctr = RE15_AFFEN_TREFFER_BIS_SPRUNG-1 und misst weiter den Original-Exit +0x5=7.

## Fortsetzung (2. Sitzung, 2026-10-03 ab 15:30) — kritische Gegenpruefung des Stands 2a03f965

Ausgangslage: Baum sauber, 12 wip-Commits, exe vom 11:25. Suite des Vorgaengers wurde um 12:00 vom Sitzungslimit
abgeschossen (local_build_ctest.log: 398 Passed, ab #399 `Exit code 0xc0000142`). Gegengelesen wurden die Punkte 2-5
am gebauten Stand gegen die Original-Aufnahme r3 (scratchpad orig_scene/r3, 110 Savestates) — zwei Befunde des
Vorgaengers halten NICHT:

### G1 — Punkt 3 war NICHT behoben (Bildbeleg) — die Ursache ist Part 18, nicht "weltfest"
- Bild (scratchpad chk_brust_zoom.png: Original s021 t=36.41 / s022 t=37.92 gegen Port-Lauf A4 F830/F850, Cut 12, Gorilla
  frontal): der Port zeigt unter dem Kinn eine WEISSE, rot geaderte Trapezplatte auf der Brust, die sich von Bild zu
  Bild verformt; im Original ist die Brust dort durchgehend braunes Fell. A4 lief NACH dem Fix des Vorgaengers.
- Original-RAM (Savestate s021_t036.41, Entity 1 = Gorilla, Part-Records entity+0x188 = 0x8016f3b4, Stride 0xac, 22 Stueck,
  alle word0 = 1 = gezeichnet):
  - Part 18: rel = **(102,-810,0)**, Elternmatrix = 0x8016f4a0 = **Matrix von Part 1 (Rumpf)**, Eltern-Record = Part 1,
    Weltmatrix == die von Part 1 (keine eigene Drehung), t = (-4279,-2816,-16899) = T1 + R1*(102,-810,0).
  - Parts 19/20/21: Elternmatrix 0x80072d4c (Identitaet), t = (75,1,78)/(0,79,1)/(79,1,80) = weltfest am Raumursprung
    (wie vom Vorgaenger belegt; y 0..+590 = UNTER dem Boden, unsichtbar).
- Schreiber: der Gorilla-INIT selbst (FUN_80116f50, Schwanz), selbst disassembliert:
  `80117200 lw v0,392(v0)` (+0x188) / `80117210 addiu v1,v0,236` + `80117214 sw v1,3204(v0)` (rec18.Elternmatrix = &rec1.Matrix,
  0xac+0x40) / `80117218 addiu v1,v0,172` + `8011721c sw v1,3240(v0)` (rec18.Eltern-Record = rec1) / `80117220 ori v1,zero,0x66`
  + `80117224 sw v1,3140(v0)` (rel.x = 102) / `80117228 addiu v1,zero,-810` + `8011722c sw v1,3144(v0)` (rel.y = -810) /
  `80117230 sw zero,3148(v0)` (rel.z = 0) / `80117234-38 sh zero,3192/3194(v0)` + `8011723c jal 0x80068098` (RotMatrix der
  Null-Winkel -> lokale Identitaet). Roh-Scan aller STAGE*.BIN nach `sw rX,0xc84(rY)` / `sw rX,0xb2c(rY)`: nur der
  Gorilla-INIT (STAGE1 0x80117214, STAGE3 0x80110fdc, STAGE4 0x8010c67c, STAGE5 0x8010c7fc) — 0x29/0x30 haben KEINE
  Umhaengung, dort gilt der Binder-Standard (weltfest).
- Mesh 18 (MD1: 29 Vtx, 2 Tri + 20 Quad, x 43..482 y -82..578 z -391..391, Fell-Textur u 0..23 v 132..180) ist die
  BRUST-/HALSSCHALE: sie verschliesst die Oeffnung des Rumpf-Meshes. Der Port zeichnete sie erst auf der Wurzelpose
  (Becken = der "komisch bewegliche Teil"), nach dem Vorgaenger-Fix am Raumursprung — beide Male blieb das Loch offen
  und man sah von vorn durch die Halsoeffnung auf die Innenseite des weiss geaderten Ruecken-Fells (die Trapezplatte).
- FIX (folgt): Part 18 des Typs 0x27 haengt an Knochen 1 mit rel (102,-810,0), Rotation = Rumpf; 19..21 bleiben weltfest.

### G2 — Punkt 4: die KI-Gates stimmen, aber die SPIELER-Reaktion auf die Treffer nicht (gemessen)
- Vergleichslage identisch (Leon ohne Eingabe am Szenen-Endpunkt (-7138,-12372), beide Gorillas frei):
  Original r3: 1. Treffer t=57.56 (Heavy -12), danach Bisse -6 im ~1,6-s-Takt, **Leon tot bei t=84.88 = 39,4 s nach der
  Freigabe (t=45.48)**; Leon bleibt dabei im Umkreis von ~500 Einheiten ((-7126,-12368) ... (-6295,-12816)).
  Port A3 (nach den Fixes des Vorgaengers, scratchpad lauf_a3/state.log, ana_state.py): Freigabe F1070, 1. Treffer F1258,
  letzter Treffer F2136 (HP 22), danach bis Lauf-Ende F3000 (= 64 s nach der Freigabe) KEIN Treffer mehr, Leon lebt.
- Zwei Ursachen, beide in der Spieler-Reaktion, beide am Original belegt:
  (a) HEAVY-KNOCKDOWN: Port F1259-F1277 traegt Leon 4915 Einheiten weit ((-7152,-12350) -> (-4446,-16458), 500/Bild mit
      Abbau); Original: Leon steht nach demselben Heavy (cmd 2/5, Clip 12) bei (-7126,-12368) = 9 Einheiten Versatz.
      Savestate s035_t057.56: Spieler +0x8c = 0 (Geschwindigkeit genullt), **+0x9e = 1** (Stopp-Flag), cmd 2/5/1.
      Mechanismus = FUN_8001c2dc (selbst disassembliert 0x8001c2dc-0x8001c3f8): Band = -(y/1800) (`mult` 0x91a2b3c5
      @0x8001c2e8-2c), `jal 0x8003b7f0` (@0x8001c340: Zellwort der ersten Zelle des Bandes, deren AABB + Radius den
      Punkt enthaelt), `andi v0,v1,0x1` @0x8001c37c -> Flag 1; sonst `andi v0,v1,0x2` @0x8001c390 -> Flag 0;
      sonst `andi v0,v1,0x600` @0x8001c3b0 -> != 0: `sb s4(=1),0(s2)` @0x8001c3c0. Handler [5] (Sturz vorwaerts,
      0x8003644c): Abbau zuerst (`subu v0,v0,v1` 5*t @0x8003656c-8c), dann `jal 0x8001c2dc` @0x80036594 (a0 = Spieler+0x34,
      a1 = Box[6] = 450, a2 = &+0x9e), Flag -> `sh zero,-13600(at)` @0x800365b0 (+0x8c = 0), dann `jal 0x800245d8`
      @0x800365b4. Handler [4] (Sturz rueckwaerts, 0x800360e8): 245d8(0x800) @0x800361fc, DANACH `jal 0x8001c2dc`
      @0x80036214, Flag -> Phase 5 (Slam) @0x80036228-30. ROOM11C0: ALLE 59 SCA-Zellen tragen floor 2/3/0x13 (Bit 0x200
      des Wortes); Leons Standort liegt im AABB der Typ-2-Zelle (20199x18187 @(-15400,-13266), floor 2) -> Flag 1
      (scratchpad sca_q.py). Der Port prueft stattdessen "hat die Wandklemme die Position veraendert" (kd_move) —
      im AABB einer Schraeg-Zelle, aber auf ihrer freien Seite, schlaegt das nie an.
  (b) BISS-RICHTUNG: der Biss schreibt cmd 2 und `aca59 = a780(BEISSER)+2` selbst (`jal 0x8001a780` @0x80118488,
      `addiu v0,v0,2` @0x80118494, `sb v0,-13735(at)` @0x8011849c). Der Port laesst den HP-Abfall-Detektor die Richtung
      aus dem NAECHSTEN Gegner ableiten (game_step_common.c re15_nearest_hostile) — bei zwei Gorillas der falsche:
      Port A3 schiebt Leon mit jedem Biss in DIESELBE Richtung ((-5703,-14275) -> (-8495,-10826), 8 Bisse je ~(-350,+330)),
      bis beide Gorillas an ihren Wandklemmen haengen (Slot 2 bei (-5050,-14635) = exakt der Original-Haltepunkt von G1,
      Slot 3 bei z=-15477). Im Original wechseln Clip 8/9 (cmd 2/2, 2/3) und Leon pendelt auf der Stelle.
- FIX (folgt): Knockdown-Stopp ueber die Original-Sonde FUN_8001c2dc ([4] und [5]), Biss-Flinch mit der Richtung des Beissers.

### G3 — Punkt 4/5: die eigentliche Ursache — Crossfade-Zaehler +0x8f des Gorillas baute nie ab (gemessen)
- Messung (Lauf A5 nach G1/G2-Fix, Leon ohne Eingabe): nur 2 Treffer bis F3300; Slot 3 lief in CHASE (Clip 5) 700 Bilder
  lang RUECKWAERTS von Leon weg ((-11727,-9924) -> (-16184,-4931), Blick dabei auf Leon). Je-Bild-Zerlegung (scratchpad
  perframe.py): an der Wand -15..-30 je Bild in Blickrichtung, im Freien +80 -> -85 je Halbfenster (netto 8,9/Bild).
- Soll (dieselbe Posenrechnung ohne Blend, scratchpad footlock.py = re15_skel_compute_pose-Zwilling): Clip 5, Locator Knochen
  17 (Bild < 0x18) / 14 (ab 0x18): +8 .. +88 .. +17 / +88 .. +126 .. +36 je Bild, 2892 je Zyklus = 74 Einheiten/Bild.
- Mess-Schiene RE15_AFFEN_FUSS (affen_fuss.log, Lauf A6): `poseaktor=3 frac=7` in 849 von 887 Bildern — die Fuss-Sperre
  re15_maggot_footlock ruft re15_skel_compute_pose OHNE das Abfrage-Muster; g_anim_pose_actor zeigt noch auf den zuletzt
  gezeichneten Aktor (Slot 3), dessen Crossfade-Zaehler dauerhaft auf 7 steht. Folge: beide Abfrage-Posen werden gegen
  SEINE Vor-Pose gemischt (und ueberschreiben sie). Die 38 Bilder mit frac=0 (G2 noch eingefroren) zeigen exakt die
  Soll-Folge (-15,+19,+30,...,+125,...,+33) und treffen die Original-Lage: G1 30 Bilder nach der Freigabe Port
  (-4850,-14440) Bild 30 / Original t=45.48 (-4855,-14459) Bild 31.
- Warum frac dauerhaft 7: re15_maggot_clip setzt +0x8f = 7 (wie das Original an jeder Clip-Site), aber re15_maggot_anim
  (= der anim_set-Aufruf des Ports) baute ihn nie ab. Original: anim_set FUN_8001f314 zieht +0x8f je Aufruf um 1
  (`lbu v0,143(v1)` @0x8001f5a8, `addiu v0,v0,-1` @0x8001f5b0, `sb v0,143(v1)` @0x8001f5b4; Aufruf mit a3 = 0x200 an den
  Gorilla-Sites, z.B. CHASE `jal 0x8001f314` @0x80117d6c). Original-Savestates r3 (66 Proben, beide Gorillas, t=36..86):
  +0x8f = 0 in 55, sonst 7 - Bild (Clip 5 Bild 2 -> 5, Bild 4 -> 3, Bild 5 -> 2; Clip 0x12 Bild 6 -> 1).
- Folgen im Port (alle drei Nutzer-Beobachtungen): (a) der RENDERER mischte dauerhaft 7/8 der Vor-Pose bei -> jede
  Gorilla-Animation lief mit einem Bruchteil ihrer Amplitude und hing nach (Punkt 5: der Brustschlag Clip 3 war so nicht
  zu erkennen); (b) die Fuss-Sperre lieferte netto keine Vorwaertsbewegung -> der Gorilla kroch auf der Stelle und wurde
  an Waenden rueckwaerts gedrueckt (Punkt 4: "nicht zielstrebig"); (c) der Mechanismus ist derselbe wie beim NPC-Gleiten
  (Memory reai-v2-npc-crossfade-decay) — dort wurde der Gorilla-Pfad nicht mitgefixt.
- FIX: re15_maggot_anim baut +0x8f ab; re15_maggot_footlock posiert als ABFRAGE (g_anim_pose_actor = NULL, Tween gesichert).

### G4 — Punkt 5 (und 4): der verbundene Rear-up-Griff setzte Leon an den RAUMURSPRUNG (gemessen)
- Lauf C1 (exe, Kampf-Layout RE15_SET_FLAG=4:0x40, Leon ohne Eingabe im Freien bei (-6500,-14800)): 3 Bisse, F442 sub 15
  (Rear-up), F446 Pin verbunden — F447 steht Leon bei **(0,0)**, ab F480 bei **(-4330,387)** und bleibt dort bis Lauf-Ende;
  der Gorilla spielt F495-F563 den Release-Clip 3 (Brustschlag) bei (-5221,-14988), 15000 Einheiten entfernt, und findet
  Leon danach nicht mehr (1061 Bilder CHASE ohne Treffer). Derselbe Endpunkt (-4330,387) stand schon im Lauf A2 des
  Vorgaengers (F4500). Der Nutzer sieht den Brustschlag deshalb nie: die Kamera folgt Leon.
- Ursache: der Pin-Latch des Originals (@0x8011abe8) ruft `jal 0x8001ac38` @0x8011ac18 mit a0 = Spieler (s1 = 0x800aca54).
  FUN_8001ac38 (selbst disassembliert 0x8001ac38-0x8001ad64): Resolver 0x8001ae38(cur, cur+0x84, cur+0x16c) -> off[kf],
  RotMatrixY(cur+0x6a) (@0x8001acc8-cc), `sh v0,160(a0)` @0x8001acfc / `sh v0,162(a0)` @0x8001ad18 = Anker des GREIFERS
  = Lage - rot(off), danach `sh v0,160(s2)` @0x8001ad30 / `sh v0,162(s2)` @0x8001ad48 = KOPIE an den Spieler. Der Port
  kommentierte den Aufruf nur ("player anchor 0x8001ac38 @0x8011ac18"), fuehrte ihn aber nicht aus: pl->anchor blieb (0,0),
  und der Bezugspunkt der Port-Wandklemme (s_victim_ok) wurde nur vom ROOM1210-Arm gesetzt.
- FIX: Anker am Pin-Latch setzen (Gorilla aus Clip/Bild, Kopie an den Spieler) + Klemmen-Bezug = Standpunkt beim Zupacken.

## Abschluss 2. Sitzung — Stand je Nutzer-Punkt (gebaut, gemessen)

Messlaeufe dieser Sitzung (exe-Kopie re15_pc_r35j3.exe, scratchpad): lauf_a5 (nach G1/G2), lauf_a6 (Mess-Schiene Fuss-Sperre),
lauf_a7 (alle KI-Fixes, Szene, Leon ohne Eingabe), lauf_a8 (Cut 12, Brustbild), lauf_c1/c2/c3 (Kampf-Layout, Rear-up-Griff vorher /
nachher / mit Cut 5). Original: orig_scene/r3 (110 Savestates des Vorgaengers, hier neu ausgewertet: Part-Records, Spieler-Felder,
+0x8f). Bilder nur ueber RE15_FRAMEDUMP (gdigrab liefert in dieser Sitzung weisse Bilder).

### Punkt 1 — Ada versteckt sich / kommt zurueck: unveraendert erfuellt
- Stand der 1. Sitzung (NPC-Klemmband +0x82) haelt; Lauf A7: Ada erreicht (-18025,-7379) in F1127 und ist danach versenkt.
  Riegel `band` und `ada` gruen (sub07 -> y=20000; beide Kill-Bits -> sub03 -> y=0, rot_y=512, Lauf zu (-16211,-8183)).

### Punkt 2 — Austritt aus dem Auto: unveraendert erfuellt, gegengelesen
- Bildvergleich vergleich_wagen_a4.png erneut gelesen: der scheinbare Hoehenversatz ist der Bildausschnitt (Original-PNG ohne,
  Port-Bild mit Szenenbalken, ~32 Zeilen); Kopf und Klappenkante liegen nach Abzug uebereinander (x 148/150, y 144/150).
  Skript-Lage (Wagen, y=-2500, Austritt (-3617,0,-17798)) byte-gleich mit dem Savestate t=34.89/36.41; Riegel `wagen`.
- Zusaetzlich jetzt richtig: im Cut 12 traegt der Gorilla die Brustschale (Punkt 3) — vorher sah man beim Austritt die weisse Platte.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper": JETZT behoben (G1)
- Vorher (auch nach dem Fix der 1. Sitzung): weisse, rot geaderte Trapezplatte unter dem Kinn (Bild chk_brust_zoom.png).
- Beleg: Gorilla-INIT @0x80117200-3c haengt Part 18 an Part 1 (rel (102,-810,0), lokale Identitaet); Savestate s021: Part 18
  Matrix = Part 1, t = (-4279,-2816,-16899); Parts 19..21 weltfest (Elternmatrix 0x80072d4c).
- Umsetzung: re15_affen_part_attach (affen_11c0.c) + Haken main.c (`affe_fest`, 3 Zeilen; der Weltfest-Haken greift fuer Part 18
  nicht mehr). Konstanten RE15_AFFEN_BRUST_* in re15_affen.h mit Adresse.
- Nachher (Lauf A8 F830/F850, Bild chk_brust_a8.png): Brust geschlossen braun wie im Original, keine Platte.
- Riegel `teile`: Rumpfmatrix des Savestates hinein -> t = (-4279,-2816,-16899) heraus (exakt der Original-Wert).

### Punkt 4 — KI zielstrebiger/aggressiver wie im Original: vier Ursachen, alle am Original belegt
| Ursache | Beleg | Umsetzung |
|---|---|---|
| Flug doppelt (1. Sitzung) | @0x80118c3c-dc4 | Zeile entfernt |
| +0x8f baut nie ab -> Fuss-Sperre netto 0, an Waenden rueckwaerts (G3) | anim_set @0x8001f5a8-b4; Savestates +0x8f = 7 - Bild | re15_maggot_anim baut ab; re15_maggot_footlock posiert als Abfrage |
| Heavy-Knockdown traegt Leon 4915 weit (G2a) | FUN_8001c2dc @0x8001c2dc-3f8; [5] @0x80036594-b4; [4] @0x800361fc-230; Savestate +0x9e=1/+0x8c=0 | re15_affen_kd_sonde; Haken kd_move + [5]-Reihenfolge (game_step_common.c) |
| Biss-Flinch aus dem naechsten statt dem beissenden Gorilla (G2b) | @0x80118488-9c | re15_affen_biss_clip + re15_player_stagger_cmd2 am Biss |
- Messung vorher (Port A3/A5, Leon ohne Eingabe): letzter Treffer F2136 bzw. nur 2 Treffer bis F3300; Leon lebt nach 64 s / 74 s.
  CHASE-Vorschub 8,9 Einheiten/Bild im Freien, an der Wand -15..-30 (rueckwaerts).
- Messung nachher (Lauf A7): Freigabe F1070, 1. Treffer F1459 (Heavy -12; Original 12,1 s nach der Freigabe, Port 13,0 s), danach
  Bisse im Wechsel beider Gorillas, **Leon tot in F2069 = 33,3 s nach der Freigabe (Original 39,4 s)**. Leon pendelt zwischen
  (-6282,-12975) und (-6368,-12750) (Original (-6295,-12816) / (-6425,-12699)); Slot 2 haelt bei (-4914,-14528)..(-4561,-14192),
  Slot 3 bei (-8444,-12986)..(-8317,-12708) (Original G2 (-8396,-12463)..(-8706,-12471)).
  CHASE-Vorschub (affen_fuss.log nach dem Fix): Clip 5 je Bild -14,+9,...,+126,...,+33, **2880/2897 je Zyklus = 74 Einheiten/Bild**
  (Soll 2892); frac des Pose-Aktors 0 in 1869 von 2087 Bildern, sonst 6..1 direkt nach dem Clip-Wechsel.
- Riegel: `flug`, `kdsonde`, `biss`, `frac`.

### Punkt 5 — Brust-Schlag: Mechanik war da, lief aber ausserhalb des Bildes und mit gedaempfter Pose (G3 + G4)
- Vorher (Lauf C1): der verbundene Rear-up-Griff setzt Leon an den Raumursprung (F447 (0,0), danach (-4330,387)); die Kamera folgt
  Leon, der Release-Clip 3 laeuft 15000 Einheiten entfernt. Dazu mischte der Renderer dauerhaft 7/8 der Vor-Pose bei (+0x8f = 7).
- Beleg: Pin-Latch `jal 0x8001ac38` @0x8011ac18 (a0 = Spieler), Anker-Kopie @0x8001ad30/@0x8001ad48; Clip 3 ab Bild 0x16
  @0x8011ad50-78, ab 0x1d @0x8011ae30-58 (1. Sitzung).
- Umsetzung: Anker am Pin-Latch (enemy_ai_common.c case 15 Phase 2) + Klemmen-Bezug; +0x8f-Abbau (Punkt 4).
- Nachher (Lauf C2/C3, Kampf-Layout, Leon ohne Eingabe im Freien): Rear-up F442, Pin F446, Leon wird 1690 Einheiten geworfen
  ((-3180,-15263) -> (-2184,-16627)) und bleibt im Kampf; Clip 3 F495-F563; in 1200 Bildern DREI Griffe (F442, F828, F1200), jeder
  mit Brustschlag. Bildfolge chk_c3_pin.png (Cut 5): F448 Griff, F464 Wurf, F480 Leon am Boden, F508-F548 Gorilla aufrecht, Arme
  wechselnd an der Brust, F556 wieder auf allen vieren.
- "Manchmal" = wenn der Biss-Commit im Freien faellt (a9cc >= 0, Bogen 2500/256, +0x1d6 == 0, +0x1e1 == 0 @0x80117ab4-b3c) UND
  der Griff in Bild 4 / 0x0f verbindet. An der Rauten-Zelle des Szenen-Endpunkts (Wandkontakt +0x1d6 != 0) kommt er nicht —
  wie in der Original-Aufnahme r3 (40 s Kampf dort, kein Rear-up).
- Riegel: `brust` (Bildfolge Release), `anker` (Latch-Anker, Leon bleibt beim Gorilla), `frac`.

### Punkt 6 — Sprung erst nach 3 Treffern (NUTZER-VORGABE): unveraendert erfuellt
- Stand der 1. Sitzung; Riegel `sprung` (3,3,7,3,3,7) gruen. Hinweis: mit der jetzt funktionierenden Fuss-Sperre laeuft der Gorilla
  nach Treffer 1 und 2 sichtbar auf Leon zu (74 Einheiten/Bild), statt wie vorher auf der Stelle zu kriechen.

## Umsetzung — Dateien (gesamt, beide Sitzungen)
- NEU: include/re15_affen.h, engine/src/affen_11c0.c (Klemmband, Part 18, ueberzaehlige Parts, Trefferzaehler, Knockdown-Sonde,
  Biss-Clip), tests/unit/test_r35_affen.c, tests/unit/probes/r35_affen.cmake.
- Haken in gemeinsamen Dateien: enemy_ai_common.c (INIT-Zaehler, Flinch-Eintritt/-Exit, NPC-Klemmband, LEAP ohne 245d8,
  Spawn-Wurzelaufruf 0x27, +0x8f-Abbau in re15_maggot_anim, Abfrage-Posen + Mess-Schiene RE15_AFFEN_FUSS in re15_maggot_footlock,
  Biss-Stagger, Pin-Anker), game_step_common.c (kd_move-Urteil = Sonde, [5]-Reihenfolge, 1 include), main.c (Part 18 am Rumpf,
  ueberzaehlige Parts weltfest, 1 include), actor_common.c (Member 0x13 = +0x1ba), scd_vm.c (+0x1ba-Seed), emd_common.c /
  re15_emd.h (EMR-Rohdaten), re15_actor.h (mag_hit_ctr). Alt-Riegel angepasst: tests/unit/test_member.c, test_maggot_ai.c.
- Kein Bank-9-Bit, keine Nachrichten-ID, kein Ereignis, kein Asset belegt (Bit 82 / IDs 20..23 / Ereignis 25 bleiben frei).

## Tests (Stand Abschluss) — re15_port/tests/unit/probes/r35_affen.cmake, test_r35_affen.c, 11 Eintraege
- unit_r35_affen_teile   — Punkt 3: Part 18 an Knochen 1, t = (-4279,-2816,-16899) = Original-Savestate; 19..21 weltfest; 0x29/0x30 ohne Umhaengung.
- unit_r35_affen_band    — Punkt 1: NPC-Klemmband +0x82 (Band 2 kommt an, Band 0 klemmt bei x=-16859).
- unit_r35_affen_ada     — Punkt 1: echte Szene sub02 -> sub07 (y=20000) -> Kill-Bits -> sub03 (y=0, rot 512, Lauf zum Ziel).
- unit_r35_affen_wagen   — Punkt 2: Klappe (-840,-2930,-19110), rot_z-Folge bis 1752, Gorilla-Lage-Schiene in Cut 12.
- unit_r35_affen_flug    — Punkt 4: ein Flugbild = 240 (kein Doppelvorschub).
- unit_r35_affen_kdsonde — Punkt 4: Sonde 1 am Szenen-Endpunkt / 0 im Freien; [5] Versatz 0 (vorher 4915); [5] frei 500/Bild; [4] ein Schritt 996, dann Slam-Clip 0xf.
- unit_r35_affen_biss    — Punkt 4: Biss von hinten bei naeherem Gorilla vorn -> Clip 9 (Richtung des Beissers).
- unit_r35_affen_frac    — Punkt 4/5: +0x8f 6,5,4,3,2,1,0,0,0,0 ab CHASE-Eintritt.
- unit_r35_affen_brust   — Punkt 5: Release Clip 3 ab 0x16 -> Phase 5/6 -> sub 2 (ab 0x1d) -> CHASE.
- unit_r35_affen_anker   — Punkt 5: Pin-Latch: Spieler-Anker = Gorilla-Anker; Leon nie am Raumursprung, groesster Abstand 2560 (vorher 15400).
- unit_r35_affen_sprung  — Punkt 6: Flinch-Exits 3,3,7,3,3,7.
Alt-Riegel der 1. Sitzung (unit_member, unit_maggot_ai) bleiben gruen.

Suite-Lauf 1 der 2. Sitzung (16:27-16:55, unter Last paralleler Baeume): 488/489, rot nur integration_r30_irons_tisch_laden
("Bild 120 in ROOM1150 nicht erreicht ... Process terminated due to timeout", 200,5 s) — ein Fenster-Haken (echte exe, ROOM1150);
einzeln nachgefahren 2x gruen in 30,4 s / 30,5 s -> Last-Flattern, keine Regression.
Suite-Lauf 2 (17:00-17:21, HEAD 11087280 = letzter Code-Stand, Baum sauber): `=== LOCAL-BUILD-OK (all) — Tests 489/489` (1265 s; 478 + 11 Riegel dieser Spur).

## OFFEN (Stand Abschluss, ersetzt die Liste der 1. Sitzung)
- **Biss-Takt 36 statt ~51 Bilder.** Port A7: ein Biss alle 36 Bilder (zwei Gorillas im Wechsel; = Flinch 22 + Commit->Treffer 13 + 1),
  Original r3: ~50-52 (Flinch-Bildnummern der 45-Bild-Proben: Clip 9 Bild 16 -> Clip 8 Bild 10 -> Clip 9 Bild 5). Der Port toetet
  den reglosen Leon deshalb in 33,3 s statt 39,4 s. Die Gates (A[3] @0x80117a54-90, Lockout +0x1dc Abbau @0x801173f8-0c, Exit
  @0x801184e0-f0, Cliplaenge 0x12 = 25) sind gelesen und gleich. Kandidat: die Fuss-Sperre des Originals arbeitet auf den GEBLENDETEN
  Pool-Matrizen (in den 7 Crossfade-Bildern nach jedem Clip-Wechsel wandert der Gorilla um den Locator-Versatz zwischen den Clips;
  der Port posiert ungeblendet und ueberspringt das Wechselbild) -> andere Abstaende beim naechsten Commit. Naechster Messweg:
  Einzelbild-Spur des Originals (PCSX-Redux-Lua oder Savestate je Bild) fuer +0x34/+0x3c, +0x8f, +0x1dc beider Gorillas ueber zwei Bisse.
- **Verbundener Rear-up-Griff ohne Original-Aufnahme.** r3 enthaelt keinen (Leon stand an der Rauten-Zelle). Wurfweite (Port 1690),
  Schaden und Leons Aufstehen (Port: Leon steht 8 Bilder nach dem Liegen, Bildfolge chk_c3_pin.png F516 -> F524) sind NICHT gegen
  das Original gemessen. Naechster Messweg: DuckStation-Aufnahme mit Leon im Freien neben dem Gorilla (Kampf-Layout), Opfer-Handler
  0x8011c118 (P0-P7) Bild fuer Bild.
- **Echter Tuer-Weg nicht gefahren.** Alle Raum-Messungen laufen ueber RE15_DEBUG_JUMP (derselbe Raumwechsel-Executor und dieselbe
  sub00/sub01-Kette). Ein Versuch ueber ROOM11B0 (Lauf lauf_t1: JUMP 11B0, RE15_PLAYER_POS an der Tuer Slot 1 rect (-26730,-29940,
  2500,3500), square/cross) loeste keinen Raumwechsel aus (Leon steht bei (-25500,-29188), keine Wechsel-Zeile) — Ursache nicht
  ermittelt (Raumskript von 11B0 oder Tasten-Zeitpunkt). Naechster Weg: Spielstand vor der Tuer + CONTINUE (Muster spiel_lauf.cmake).
- **Knockdown-Sonde gilt spielweit.** FUN_8001c2dc ersetzt das Wand-Urteil der Handler [4]/[5] fuer JEDEN Knockdown (auch
  Alligator-Heavy); byte-true, aber nur in ROOM11C0 nachgemessen. Messweg: Savestate +0x9e/+0x8c nach einem Knockdown in ROOM2090.
- **Gorilla-Schatten.** Der INIT ruft FUN_8001af5c(0, 0, Box[6]+100, Box[6]+200, +0xb0, 0x808080) (@0x801171d8-ec); der Port setzt
  crow_shadow_w/h = 1000/500 mit dem Zitat des HUNDE-INIT. Nicht Teil der sechs Punkte, nicht angefasst.
- Heckklappe (Objekt 0) dunkler als im Original (Cut 12: Original ~ (35,36,39), Port ~ (11,11,12)); Weg: Objekt-Beleuchtung
  FUN_8002c18c (`jal 0x80053fc0` @0x8002c254) gegen main.c lctx_prop messen.
- Member_set 0x13 (= +0x1ba statt hp) betrifft auch ROOM10B1/2030/2031/3050/3051/50D0/50D1; dort nicht nachgemessen (Savestate +0x9a/+0x1ba).
- Spawn-Wurzelaufruf: ausser Birkin jetzt auch 0x27; uebrige Typen wie vor Runde 35.
- Die Fuss-Sperre der uebrigen Sites (c024 Heavy, bf50 Biss/Rise) laeuft ueber dieselbe reparierte Funktion; einzeln nachgemessen
  wurde nur CHASE (Clip 5).

## Fuer den Nutzer (Stand Abschluss)
- Sprachdateien: keine neuen Zeilen (alle Texte sind die Original-Nachrichten msg00..09 von ROOM11C0).
- Neue Assets fuer das Paket-/Android-Gate: keine.
- Was sich im Kampf spuerbar aendert: die Gorillas laufen jetzt wirklich auf Leon zu (vorher krochen sie auf der Stelle und wurden an
  Waenden rueckwaerts gedrueckt), ihre Animationen laufen mit voller Auslenkung, ein schwerer Treffer schleudert Leon in ROOM11C0
  nicht mehr quer ueber den Platz, und wer stehen bleibt, ist nach gut einer halben Minute tot (Original: 39 s).
- Brustschlag: kommt nach jedem verbundenen Griff (Gorilla richtet sich auf, packt und wirft Leon) — im Freien, nicht an der Wand.
- Sprung: erst beim 3. Treffer (RE15_AFFEN_TREFFER_BIS_SPRUNG in include/re15_affen.h, NUTZER-VORGABE).
- Mess-Schalter (kein Spielverhalten): RE15_AFFEN_FUSS=1 schreibt affen_fuss.log neben die exe (Fuss-Sperre je Bild).

## Nachbesserung 1 (2026-10-03 abends, nach Abnahme 0 = NICHT BESTANDEN, M1-M5)

Ausgangslage: Baum sauber, HEAD e14fd399 (Abnahmebericht J_abnahme_0.md). Scratch dieser Nachbesserung:
`scratchpad/jnb1/` (die Nachbarordner im Scratch gehoeren anderen Spuren). Reihenfolge: M5 (Haken
verkleinern, mechanisch) -> M2 (Riegel Spawn-INIT/Wagen) -> M3 (Zaehler in allen Treffer-Spuren) ->
M4 (NPC-Klemmband ausserhalb 11C0 messen) -> M1 (Biss-Takt gegen das Original). Je Mangel: Ursache,
Messung vorher, Beleg, Aenderung, Messung nachher.

### Stand (fortlaufend)
- [ ] M5  - [ ] M2  - [ ] M3  - [ ] M4  - [ ] M1

### M1 — Messweg Original (Einzelbild-Naehe ohne PCSX-Redux)
- Zeitbasis der Original-Aufnahme r3 geprueft (nicht vermutet): RAM-Suche ueber 10/12 aufeinanderfolgende
  r3-Savestates nach monoton wachsenden Woertern (`jnb1/fcount.py`, `fcount2.py`): **0x800787dc** waechst
  90/91 je 1,5 s (= VSync 60 Hz, Emulation lief mit 100 %), und das Gorilla-Feld **entity2+0x1de
  (0x800ad1f2)** waechst 45/46 je 1,5 s waehrend des Kampfes (= die Spiel-Logik lief durchgehend mit
  30 Bildern/s, kein Einbruch auf 20). Die Wanduhr-Zeiten der Abnahme (~49 Bilder je Biss) sind damit
  echte Spielbilder, kein fps-Artefakt.
- Neuer Recorder `jnb1/dsfast.py`: DuckStation laedt den r3-Savestate s035 (t=57.56) DIREKT
  (`-statefile`, deterministisch laut re15-parity-verify §4), Leon ohne Eingabe, Savestates so schnell wie
  moeglich (~0,21 s = 6-7 Spielbilder); Bild-Zuordnung je Savestate ueber den VSync-Zaehler / 2.
  Probe `try0` (4 s) reproduziert r3 exakt (t=60.58: PL(-6729,-12800) hp 82 Clip 9 — identisch).
- Erste Einzelbild-Beobachtung (try0, Bild = (VSync - VSync0)/2): BEIDE Gorillas committen gleichzeitig
  (F17-F23 beide sub 5, Clip 0x12); e2 trifft bei F24 (+0x1dc 45 -> 39 bei F30), e1 verfehlt (Spieler
  +0x93 schon gesetzt) und steigt bei F43 mit +0x1dc := 0x14 aus (@0x801184e0-f0). Der Spieler-Flinch
  (Clip 9) endet ~F47 (+0x93 = 0, Zustand 1/0/1 Clip 3 Bild 0). Danach warten BEIDE noch auf ihre Sperre
  (e1 14, e2 20 bei F49) -> naechster Commit e1 F64, Treffer F78 = **54 Bilder** Abstand.
- **Einzelbild-Spur des Originals (jnb1/oA 117 + oB 66 Savestates ab s035, Bild = VSync/2):** Treffer bei
  F~130/177/231/281/333/386/437 = Abstaende **47/54/50/52/53/51**. Je Zyklus committen BEIDE Gorillas fast
  gleichzeitig (Abstand 0-6 Bilder), einer trifft (+0x1dc := 45, @0x80118470-78), der andere verfehlt
  (Spieler +0x93 schon 1, Fenster-Gate @0x801183b4-bc) und steigt am Clip-Ende mit +0x1dc := 0x14 aus
  (@0x801184e0-f0). Spieler-Flinch (Clip 8/9) ~22 Bilder, danach steht Leon bis zum naechsten Treffer
  (Zustand 1/0/1). Takt = Verlierer-Sperre: Treffer T -> Verlierer-Exit ~T+13..17 -> Sperre 20 -> Commit
  ~T+35 -> Fenster Bild 0x0c -> Treffer ~T+50. Beide Gorillas stehen dabei im Koerperabstand **2050**
  (= 1600 Gorilla-Box[6] + 450 Spieler) vor Leon.
- **Port, Raum-Harness (`test_r35_affen takt`, neu):** EM027-Bank geladen, Startlage = Original-Bild F36
  (beide CHASE, Sperren 19/11, Leon frei). Ergebnis: Treffer-Abstaende **46 57 46 57 47 56 47** (Mittel 51)
  — derselbe Gleichtakt wie im Original. Die REGELN des Ports erzeugen ab derselben Lage den Original-Takt.
- **Port, exe-Lauf t2 der Abnahme (state.log je Bild):** dort laufen die Gorillas im WECHSELTAKT: nach dem
  Heavy (F1477) und Leons Knockdown committet nur Slot 3 (F1570, Abstand 2054); Slot 2 haengt an seinem
  Wand-Haltepunkt (-5015,-14604) mit Abstand **3069 > 3000** (Biss-Bogen a804(0xbb8,..) @0x80117a60-74) und
  kann nicht committen. Danach wartet jeweils der eine in CHASE mit Sperre 0 auf das Flinch-Ende (+0x93 = 0,
  Gate @0x80117a54-5c) und trifft 13 Bilder spaeter: Takt 23 + 13 = **36**. Auch dieser Wechseltakt ist
  unter denselben Regeln stabil. Im Original r3 stand G1 nach demselben Knockdown am selben Haltepunkt
  (-5042,-14629), Leon bei (-7097,-12368) = Abstand 3055 — und G1 committete trotzdem ~6 Bilder VOR dem
  ersten Biss von G2. Wie er in den Bogen kam, klaert die Einzelbild-Spur ab s036 (oC/oD, laeuft).

### M5 — Haken in gemeinsamen Dateien auf 1-2 Zeilen je Stelle (erledigt, gebaut, 13/13 Riegel gruen)
- Ursache: Begruendungs-Kommentare und Logik standen inline in den gemeinsamen Dateien.
- Messung vorher (Abnahme, `git diff master -U0`): enemy_ai_common.c +97/-12 (16 Hunks, 7 davon > 5 Zeilen),
  game_step_common.c +33 (kd_move +14/-10), main.c +17 (Hunks +11 / +5).
- Aenderung: Logik nach engine/src/affen_11c0.c verschoben — re15_affen_pose_abfrage (Fuss-Sperre als
  Abfrage), re15_affen_fuss_log (Mess-Schiene RE15_AFFEN_FUSS, vorher 10 Zeilen Datei-Log im Spiel-Code),
  re15_affen_pin_anker (FUN_8001ac38 @0x8011ac18), re15_affen_teil_weltfest (Zeichner-Regel 2c). Die
  Begruendungen (mit allen Adressen) stehen jetzt in include/re15_affen.h (4c)-(4f), (5); an der Stelle
  bleibt eine Zeile mit Adresse und Verweis. kd_move wieder in der Vor-Runde-35-Form, nur die Urteils-Zeile
  ist die Sonde (`int wall = re15_affen_kd_sonde(...)` @0x80036214); [5] 3 Zeilen (Decel, Sonde, Vorschub).
  Der Biss-Kommentar des Originals (audit #17) ist wiederhergestellt.
- Messung nachher (`git diff master -U0`): enemy_ai_common.c **+17/-9 in 15 Hunks, keiner > 2 Zeilen**;
  game_step_common.c **+4/-2** (4 Hunks); main.c **+5** (1 include + 2x2); actor_common.c +2/-2;
  scd_vm.c +1; emd_common.c +2. Verhalten unveraendert: `ctest -R "r35_affen|^unit_member$|^unit_maggot_ai$"`
  13/13 gruen.

### M2 — Riegel `wagen` pinnt jetzt die Ursache von Punkt 2 (erledigt, gruen)
- Ursache des Mangels: der Riegel pruefte nur die Klappe und druckte die Gorilla-Lage.
- Neu im Riegel (test_r35_affen.c teil_wagen), alle gegen den Original-Savestate r3:
  - Spawn-Bild (1 Bild nach dem Raumaufbau): beide Records grid 0x30 (eingefroren), **Zustand 1 / sub 0,
    HP 180, Scale 0x1b33** (`ori v0,zero,0x1b33` / `sh v0,358(v1)` @0x80117148-4c), Lagen
    (-1220,-20000,-21568) / (-554,-20000,-25423) = s001 t=6.11. Ohne den Spawn-Wurzelaufruf
    (`jalr 0x80072bac[typ]` @0x8004259c) bliebe der Record in Zustand 0 mit Scale 0 -> der Riegel faellt.
  - Cut 12 bis zur Freigabe: G1 auf **y = -2500** in allen 36 Bildern (`Member_set 01 = -2500` @0x1996;
    s020 t=34.89: y=-2500).
  - Freigabe 36 Bilder nach Cut-12-Beginn: G1 grid 0x10 bei **(-3617,0,-17798)**, Zustand 1 (@0x1A2E;
    s021 t=36.41: (-3617,0,-17798) g=10).
- Ergebnis: `test_r35_affen wagen` alle Pruefungen ok.
- **Werkzeug-Befund (fuer kuenftige Spuren wichtig):** seit ~21:58 legt ViGEm einen GEISTER-Pad an
  (`USB\VID_045E&PID_028E\01`, XInput 0, kein Besitzer-Prozess; `pnputil /remove-device` = Zugriff
  verweigert). DuckStation bindet `SDL-0` -> der neue vgamepad landet auf XInput 1 und KEIN Hotkey kommt an
  (Savestate-Recorder: 0 Saves). Zusaetzliche Bindings `SDL-1..4` halfen nicht, `-settings` kennt diese
  DuckStation-Version nicht. AUSWEG ohne Pad: der **DuckStation-GDB-Server** (`[Debug] EnableGDBServer =
  true`, Port 2345; settings.ini voruebergehend geaendert, Original in scratch `jnb1/settings.ini.orig`,
  wird am Ende zurueckgespielt). `jnb1/gdbspur.py`: Haltepunkt `Z0` auf die Gorilla-Wurzel **0x80116db8**
  (Dispatch 0x80072bac[0x27], Prolog `addiu sp,sp,-24`), je Halt `m`-Lesen von g_entity(cur) 0x800ac784;
  ist cur = Entity 1 (0x800ace20), werden Spieler 0x800aca54, Entity 1/2 (je 0x1f4 B), aca58..5b und der
  VSync-Zaehler gelesen, dann `c`. Ergebnis: **jedes Spielbild** (VSync +2 je Zeile, Probe g_try.txt
  9875..9913), deterministisch ab dem direkt geladenen r3-Savestate.

### M3 — 3-Treffer-Vorgabe in ALLEN drei Treffer-Spuren (umgesetzt, Riegel gruen; exe-Nachmessung folgt)
- Ursache: der Zaehler sass nur in Spur 0 (Boden-Flinch). Schrotflinte (Zeile 7) laeuft ueber Spur 1
  (Luft-Treffer 7/8/13/21), Zeilen 9..11/15..18 ueber Spur 2 (Sturz) — beide ohne Zaehler.
- Messung vorher (Abnahme t5, Schrotflinte): 6 Treffer -> 6 Spruenge (Exit-Subs 7,7,7 / 7,7,7).
- Beleg (selbst disassembliert, STAGE1.BIN): Spur 1 Exit `sb zero,147(a0)` @0x8011b3ac (+0x93 = 0),
  `sb v0,4(v1)` @0x8011b3bc (+0x4 = 1), `ori v0,zero,0x7` / `sb v0,5(v1)` @0x8011b3c8-cc (+0x5 = 7),
  `sb zero,6(v0)` @0x8011b3dc, `sb zero,7(v0)` @0x8011b3ec. Spur 2 Exit `sb zero,147(v0)` @0x8011b6a8,
  `ori v0,zero,0x1` / `sb v0,4(v1)` @0x8011b6b4-b8, `ori v0,zero,0x7` / `sb v0,5(v1)` @0x8011b6c4-c8,
  `sb zero,6/7` @0x8011b6d8/e8. Das Original springt also nach JEDEM Treffer in jeder Spur; die Zahl 3 ist
  NUTZER-VORGABE, die Form (Exit-Schreibsatz +0x4/+0x5/+0x6/+0x7, Ersatz-Sub 3 = CHASE) ist belegt.
- Aenderung: affen_11c0.c re15_affen_sprung_oder_jagd (7 beim 3. Treffer, Zaehler zurueck, sonst 3);
  re15_affen_flinch_exit_sub (Spur 0) = +0x1e3 ? 9 : dieselbe Regel. Haken enemy_ai_common.c: Spur 1 und 2
  zaehlen beim Eintritt (je 1 Zeile, @0x8011b238-54 / @0x8011b44c-68) und nehmen den Exit aus der Regel
  (je 1 geaenderte Zeile, @0x8011b3c8-cc / @0x8011b6c4-c8).
- Riegel `sprung` erweitert: Spur 0 (Zeile 3), Spur 1 (Zeile 7), Spur 2 (Zeile 9) und GEMISCHT (3,7,9,9,3,7):
  je 6 Treffer -> Exits 3,3,7,3,3,7 — **alle 24 Pruefungen ok**.
- Sprungkette nach dem 2. Schrottreffer (t5, Slot 2 F1365-F1644), Mechanismus im Port-Code nachgelesen:
  (1) F1390 Spur-1-Exit -> sub 7 mit +0x7 = 0 (Vergeltungssprung, KEIN Slew im Anlauf @0x801189b8-a00 ->
  fliegt entlang seiner Blickrichtung ~6000-8900 weit, Original r3 ~8900) -> landet hinter Leon;
  (2) F1430 SELECTOR (sub 4) -> F1441 Zonen-Sprung Path A attr 0x10 (+0x7 = 1, @0x80117ecc-80118024);
  (3) F1489/F1530/F1582 Zonen-Spruenge mit +0x7 = 3 (BLIND, LOS-Latch frei, @0x80117fc8-8011802c) — Flug-
  geschwindigkeit **0x32a = 810** (@0x80118a94-aa0) = genau die "810 Einheiten/Bild" der Abnahme. Glieder
  (2)/(3) sind die Original-Navigation ueber die SCA-Marker (Runde S5, 2026-09-05), nicht treffer-
  ausgeloest; ausgeloest wurde die Kette vom Vergeltungssprung (1) nach dem 2. Treffer — den gibt es mit
  der Vorgabe jetzt erst beim 3. Treffer.

### M1 — Ergebnis: der 36-Bilder-Takt IST Original-Verhalten (Mangel per Messung widerlegt)
- **Einzelbild-Spur des Originals** (GDB, ab r3 s033, 1000 Bilder, `jnb1/g_orig.txt`, Bild = (VSync-9641)/2,
  Messpunkt = Halt an der Gorilla-Wurzel 0x80116db8 von Entity 1 = VOR deren KI-Tick des Bildes):
  Heavy F61, danach Bisse in F165, 218, 268, 321, 371, 424, 475, 527, 579, 630, 683, 733, 787, 836, 891
  (Tod) = Abstaende **53,50,53,50,53,51,52,52,51,53,50,54,49,55** (Gleichtakt; Tod 830 Bilder nach dem Heavy).
  Zyklus Bild fuer Bild (F195-F270): Commit e1 F205 / e2 F211 (Sperren liefen F204 / F210 ab), Treffer e1
  F218 (Bild 13 im Fenster 0x0c-0x0f, +0x1dc := 45), e1 Exit F229, e2 verfehlt (+0x93 = 1) und steigt F235
  mit +0x1dc := 0x14 aus, Spieler-Flinch Clip 8 F219-F240 (+0x93 = 0 ab F241), Commit e2 F256 / e1 F264,
  Treffer e2 F268.
- **Port ab DERSELBEN Lage** (Riegel `takt`, Startzustand = Original-Bild F195 aus der GDB-Spur, EM027-Bank
  geladen, HP-Detektor ohne Baseline): Commit e1 F205 / e2 F211, Treffer **F218**, e1 Exit F230, e2 Exit
  F236 (+0x1dc 19), Spieler frei F241, Commit e2 F257 / e1 F263, Treffer F270; Treffer-Bilder
  **218/270/321/374/424/478/527/582** gegen Original **218/268/321/371/424/475/527/579** — jeder der 8 Bisse
  innerhalb 0-3 Bilder. (Erster Harness-Lauf ohne `re15_player_cmd_zero` taeuschte einen Phantom-Flinch vor:
  HP 100 -> 82 beim Setzen der Lage wurde vom HP-Abfall-Detektor als Treffer gelesen.)
- **Gegenprobe im ORIGINAL** (GDB-Speicherschreiben `M800ad1f0,2:1e00` in F203 = e2 +0x1dc := 30, sonst
  nichts, `jnb1/g_desync.txt`): e1 beisst allein (F218), e2 wartet in CHASE mit Sperre 0 auf das Flinch-Ende
  und trifft 36 Bilder spaeter — das Original faellt in den **Wechseltakt 36/36/36/36/35/36/...** (Treffer
  218, 254, 290, 326, 362, 397, 433, 469, 505, 541, 577, 613, 649, 685, Tod F709) und haelt ihn. Leon
  wandert dabei stetig nach NW ((-6721,-12789) -> (-8004,-11269), ~2000 Einheiten) — genau die beiden
  Abnahme-Befunde (35,4 Bilder/Biss, 1260 Einheiten NW-Drift im exe-Lauf t2).
- **Mechanismus (beide Takte = dieselben Regeln):** Commit nur bei Spieler +0x93 = 0 (@0x80117a54-5c),
  Bogen a804(0xbb8,0x180) (@0x80117a60-74) und +0x1dc = 0 (@0x80117a88-90); Treffer setzt +0x1dc = 45
  (@0x80118470-78), der Verlierer eines Doppel-Commits steigt am Clip-Ende mit 0x14 aus (@0x801184e0-f0).
  Committen beide im selben Flinch-Fenster -> Gleichtakt ~50 (Takt = Verlierer-Sperre 20 + Fensterweg);
  steht einer beim ersten Biss ausserhalb des Bogens -> Wechseltakt 23 (Flinch) + 13 (Commit -> Bild 0x0c)
  = 36. In r3 schob der Biss-Lunge von G2 Leon in F155-F161 um ~70 Einheiten nach Osten (Koerperkontakt
  2050), G1 kam dadurch in den Bogen (Abstand 3016 -> 2998 in F159) und committete mit. Im Port-exe-Lauf t2
  stand Leon nach der Szene ~25 Einheiten anders ((-7164,-12350) Tuerweg / (-7157,-12355) JUMP-Lauf A7 gegen
  r3 (-7138,-12372)), G2s Lunge lief an der Wand entlang z, Slot 2 blieb bei 3069 > 3000 -> Wechseltakt.
- **Folge:** kein Code-Fix fuer den Biss-Takt (es gibt keinen Regel-Unterschied; ein Eingriff waere eine
  erfundene Verlangsamung). Offen bleibt die Szenen-Endlage Leons (25 Einheiten), die im JUMP-Szenario
  entscheidet, welcher der beiden Original-Takte entsteht -> OFFEN.
- Rear-up-Griff gegen das Original (Abnahme: 0 HP, ~1600 Versatz beim Zupacken): siehe unten.
- **Riegel `unit_r35_affen_takt` (neu):** zwei Laeufe ab der Original-Lage F195 — Gleichtakt (Soll = GDB-Spur
  218/268/321/371/424/475/527/579) und Wechseltakt nach Desync e2 +0x1dc := 30 in F203 (Soll = GDB-Spur
  218/254/290/326/362/397/433/469). Port: groesste Abweichung **3** bzw. **4** Bilder (Schranke 6, das
  Original streut je Biss +-3), mittlerer Abstand **52,0** (Original 51,6) bzw. **35,3** (35,9). Gruen.
