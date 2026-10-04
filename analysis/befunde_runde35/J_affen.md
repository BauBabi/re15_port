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

## Tests (Stand Abschluss, ERSETZT durch "Tests (Stand Nachbesserung 1)") — 11 Eintraege
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

## OFFEN (Stand Abschluss, ERSETZT durch "OFFEN (Stand Nachbesserung 1)")
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

## Fuer den Nutzer (Stand Abschluss, ERSETZT durch "Fuer den Nutzer (Stand Nachbesserung 1)")
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
- [x] M5  - [x] M2  - [x] M3  - [x] M4  - [x] M1 (widerlegt + Griff-Defekt behoben) — Ergebnis-Tabelle am Abschnittsende

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

### M3 — 3-Treffer-Vorgabe in ALLEN drei Treffer-Spuren (umgesetzt, Riegel gruen; exe-Nachmessung unten)
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

### M1-Nachtrag — Rear-up-Griff gegen das Original: 0 HP belegt, Sprung beim Zupacken = Port-Defekt (behoben)
- **Original-Experiment (GDB):** r3 s033 direkt geladen, in F250 e1 +0x5/+0x6/+0x7 := 15/0/0
  (`M800ace25,3:0f0000`, `jnb1/g_griff.txt`). Ergebnis Bild fuer Bild: Clip 0x1c ab F251, Pin in F254
  (Bild 4), Spieler-Zustand 5 ab F255 mit Yaw := Gorilla (2823), Opfer-Clip 1 **ab Bild 0** (Front-Griff),
  Leon steht F255-F265 auf (-6660,-12486), **erste Wurf-Platzierung F266** (Bild 0x0c), HP **76 -> 76**,
  frei (Zustand 1) in F381 bei (-4645,-10726).
- **0 HP = Original:** Roh-Scan B[15] 0x8011a878-0x8011af40 und Opfer-Handler 0x8011c118-0x8011c598
  (Phasentabelle @0x80100404): kein Schreiber auf Spieler+0x9a (`154(` 0 Treffer; der einzige
  `sh v0,154(v1)` der Umgebung liegt bei 0x8011c748 in einer anderen Funktion).
- **Port vorher (Riegel `griff`, gleiche Lage, Opfer-Bank geladen wie main.c:1270):** Pin F254, Opfer-Clip ab
  **Bild 0x0c** (Rueck-Variante) -> Leon springt im Latch-Bild von (-6658,-12488) nach (-7835,-11422) —
  genau der "~1600 Einheiten beim Zupacken"-Befund der Abnahme (t6 F1202->F1203).
- **Ursache (selbst disassembliert 0x8011abe8-acb4):** `jal 0x8001ac38` @0x8011ac18 (Anker) ->
  `sb v1(5),-13736(at)` @0x8011ac48 (aca58 = 5) -> **`jal 0x8001a780` @0x8011ac50** (a0 = Spieler) ->
  `sb v0,-13735(at)` @0x8011ac68 (aca59 = a780) -> ... -> **`jal 0x8001a8f8` @0x8011acac, a1 = 0x800**
  (Yaw-Latch). Das Original rechnet die Variante mit Leons ALTEM Blick. Der Port rief erst
  re15_player_victim_latch (setzt den Yaw auf den des Gorillas) und danach a780 -> Spieler- und Gorilla-Yaw
  gleich -> a780 = 1 -> immer Rueck-Variante -> Opfer-Clip ab Bild 0x0c -> sofortige Platzierung.
- **Aenderung (1 Zeile, enemy_ai_common.c Pin-Latch):** `re15_player_victim_latch_ex(e, pl,
  re15_maggot_a780(e, pl))` — das Argument wird VOR dem Yaw-Latch ausgewertet; die nachtraegliche
  Zuweisung `g_player_victim_variant = a780` entfaellt.
- **Nachher (Riegel `griff`):** Pin F254 bei (-6658,-12487), Leon steht bis Bild 0x0b, **erste Platzierung
  F266** (Original F266), HP 76 -> 76. Die Platzierungs-Lagen der Bilder 15-18 decken sich mit dem
  Original auf ~10 Einheiten, aber um 1 Bild frueher (Port F269 (-7114,-12075) = Original F270
  (-7110,-12078)); ab Bild ~19 laeuft der Wurf im Port entlang z ~ -12500 nach Osten, im Original nach SO
  (z bis -13711) und endet in einem Sprung nach (-5381,-10551) bei Bild 0x25 -> Endlage weicht ~1900 ab
  (OFFEN, Messweg unten). Riegel `griff` prueft Pin-Bild, Stehen beim Zupacken, Bild der ersten
  Platzierung (+-1) und 0 HP; alle 15 r35-Riegel + unit_member/unit_maggot_ai gruen.

### M4 — NPC-Wandklemme auf +0x82: spielweit gezaehlt und in 10D0/1050 gemessen
- Ursache des Mangels: re15_npc_wall_clamp ist der Wurzel-Schwanz ALLER NPCs (0x40/42/45/47/49/4b/4d,
  Dispatch enemy_ai_common.c ~14680); die Umstellung (Band = +0x82, FUN_8003b0a4 `lbu v1,130(a3)`
  @0x8003b228-3c) war nur in ROOM11C0 gemessen und stand nicht unter OFFEN.
- Mechanik-Beleg: die alte Klemme re15_collision_constrain_enemy(..., y, 4) ruft dieselbe
  constrain_contact_band mit band_from_y(y) (re15_collision.c:797/809). Ist +0x82 == band_from_y(y), sind
  alte und neue Klemme identisch.
- **Zensus aller RDTs** (`jnb1/npcband_census.py`, scd_dump_room.py): 101 NPC-Sce_em_set; **94** mit
  pc[4] (= +0x82-Seed @0x800421c8-d0) == band_from_y(y). Abweichend 7: 1090 Ada 0x42 (+0x82 1, geparkt
  (-30000,-30000), das Skript setzt spaeter y = -1800 @0x02558 -> wieder gleich), 10B1 0x49 (+0x82 5,
  geparkt, Skript setzt +0x82 = 0 @0x017DA/@0x018C2 vor dem Lauf), 11C0 Ada Kampf-Layout (+0x82 0, y
  -20000), 4001 0x49/0x4b (+0x82 0, y -3500 -> band_from_y 1), 6030/6031 Irons 0x40 (+0x82 1, y -920 -> 0).
  Member_set y/+0x82 auf NPC-Slots: 8 (1090, 10B1 x2, 11C0 x4, 4031).
- **Dynamisch (Riegel `npcband`, neu, je Raum 600 Bilder ab Raumaufbau):** 10D0 1 NPC / 0 Bilder mit
  abweichendem Band; 1050 kein NPC-Aktor (die Ada-Ruf-Szene der Runde 34 spawnt keinen NPC, nur
  Plc_*-Opcodes fuer Leon); 1090 0; 10B1 1 NPC, 600 abweichende Bilder, aber **keine Bewegung** (geparkt);
  1170 0; 11B0 3 NPC / 0; 4001 2 NPC / 46 abweichende Bilder, keine Bewegung; 4031 / 6030 / 6031 je 1 NPC / 0.
  **In keinem Raum laeuft ein NPC mit abweichendem Band** — die Umstellung aendert dort nichts Sichtbares.
  Wo sie wirkt (11C0 sub07, 4001 und 6030/6031 falls die NPCs dort laufen), ist sie byte-true, weil das
  Original dieselbe +0x82-Klemme faehrt (@0x8011cc58-68 -> 0x8003b0a4).
- Riegel `npcband` pinnt 10D0 und 1050 (0 laufende NPC mit abweichendem Band); die uebrigen Raeume werden
  gedruckt. Szenen, die erst ueber Flags laufen (Marvin-Gang 10D0, Irons 6030), deckt der Raumaufbau nicht
  ab -> OFFEN (Messweg: Szenen-Trigger im Harness, gleiche Zaehlung).

### M3-Nachtrag — Vergeltungssprung nach einem Schrot-Treffer gegen das Original (GDB-Experiment)
- Original: r3 s033 direkt geladen, F250 e1 +0x4..+0x7 := 2/7/1/0 (Spur 1, Zeile 7 = Schrotflinte;
  `M800ace24,4:02070100`, `jnb1/g_schrot.txt`): Clip 8 F251-F275, Exit F276 -> sub 7, Anlauf F277-F286,
  Flug F286-F312, **Landung F312 bei (-10040,-8792)** = 5361 von Leon (der Sprung ueberfliegt ihn), F316
  SELECTOR -> **Clip-6-Heavy-Anlauf bis F444**, dann CHASE. **Kein Zonen-Sprung.** Das Ueberfliegen ist also
  Original (Sprung entlang der Blickrichtung ohne Slew, @0x801189b8-a00).
- Port (Riegel `schrot`, neu; gleiche Lage, mag_hit_ctr = 2 damit es der 3. Treffer ist): Exit **F275**,
  Landung **F311** bei (-10110,-8325) (472 vom Original), SELECTOR **F315**, Clip 6 bis **F444**, CHASE F445 —
  jedes Ereignis 0-1 Bild neben dem Original, kein Zonen-Sprung. Die Zonen-Sprung-Kette der Abnahme (t5)
  entstand dort aus anderer Lage (Landepunkt an den SCA-Markern, LOS-Latch frei); sie ist dieselbe
  Path-A-Regel (@0x80117ecc-8011802c), hier nicht ausgeloest — im Original wie im Port.

### Messung nachher am gebauten Stand — exe ueber den ECHTEN Tuerweg (Schrotflinte, wie Abnahme t5)
- Lauf `jnb1/n5` (exe-Kopie re15_pc_jnb1.exe, Env = Abnahme t5: RE15_SET_FLAG=4:243,3:130,
  RE15_DEBUG_JUMP=11B0@240, RE15_PLAYER_POS an der Tuer, Quadrat-Tasten, RE15_GIVE=7:200 RE15_EQUIP=7,
  Feuerskript ab 11C0-F1260): `DOOR FIRE` -> room11c0, sub02 F6, sub07 F1088.
- Auswertung mit dem Abnahme-Skript `jab3/p6.py` (Segment 11C0):
  Slot 2: 3 Treffer, Exit-Subs **3, 3, 7** -> 1 Sprung (nach dem 3. Treffer, F1390);
  Slot 3: 3 Treffer, Exit-Subs **3, 3, 7** -> 1 Sprung nach dem 3. Treffer (F1789) + 1 Fernsprung des
  Selektors aus sub 4 (F1395, Path B @0x80118028-8100 — kein Treffer-Sprung, auch r3 zeigt ihn bei t=54.55).
  Vorher (Abnahme t5, gleiche Eingaben): 6 Treffer -> 6 Spruenge (7,7,7 / 7,7,7) + 4 Zonen-Spruenge.
- Punkt 1 haelt: beide Gorillas tot -> sub03 F2018 (Ada kommt zurueck), sub04 F2606 (gemeinsamer Gang),
  danach Raumwechsel nach 11B0 (wie Abnahme t5).

### Nachbesserung 1 — Ergebnis je Mangel
| Mangel | Ursache | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| M1 Biss-Takt 35 statt ~49, Tod 33 statt 39 s, NW-Drift | KEIN Regel-Unterschied: das Original hat zwei stabile Takte (Gleichtakt ~51, Wechseltakt 36); welcher entsteht, entscheidet die Lage nach dem ersten Heavy (Bogen 3000 @0x80117a60-74) | GDB-Einzelbild-Spur r3 (Treffer 218/268/321/...); Original-Desync (+0x1dc := 30) -> 36/36/36..., Leon driftet ~2000 nach NW | keine am Biss-Takt (waere erfunden) | Port ab Original-Lage F195: 8 Bisse auf 0-3 Bilder (Gleichtakt), nach Desync 0-4 Bilder (Wechseltakt); Riegel `takt` |
| M1 Rear-up-Griff (0 HP, ~1600 Sprung beim Zupacken) | 0 HP = Original; der Sprung = Port-Defekt: aca59 = a780 nach statt vor dem Yaw-Latch | `jal 0x8001a780` @0x8011ac50 vor `jal 0x8001a8f8` @0x8011acac; GDB-Griff: Leon steht bis F265, Platzierung ab F266, HP 76 -> 76 | Pin-Latch: `re15_player_victim_latch_ex(e, pl, re15_maggot_a780(e, pl))` | Riegel `griff`: Pin F254, Leon steht, erste Platzierung F266, 0 HP |
| M2 Riegel pinnt Ursache von Punkt 2 nicht | Riegel `wagen` druckte nur | s001 t=6.11 / s020 / s021 | `wagen` prueft Spawn-INIT (Zustand 1, HP 180, Scale 0x1b33 @0x80117148), y = -2500 (36 Bilder), Austritt (-3617,0,-17798) | gruen |
| M3 Schrotflinte springt nach jedem Treffer | Zaehler nur in Spur 0 | Exits Spur 1 @0x8011b3c8-cc, Spur 2 @0x8011b6c4-c8 (+0x5 = 7) | Zaehler in Spur 1/2 (Eintritt + Exit, je 1 Zeile) | exe Tuerweg + Schrot: 3,3,7 / 3,3,7 (vorher 7,7,7 / 7,7,7); Riegel `sprung` 24/24, `schrot` (Original-GDB 0-1 Bild) |
| M4 NPC-Klemme +0x82 spielweit ungemessen | Wurzel-Schwanz aller NPCs | FUN_8003b0a4 `lbu v1,130(a3)` @0x8003b228-3c; Zensus 101 Spawns (94 gleich) | keine (byte-true); OFFEN-Eintrag | Riegel `npcband`: in 10 Raeumen kein laufender NPC mit abweichendem Band |
| M5 Haken > 1-5 Zeilen | Logik/Kommentare inline | — | Logik + Mess-Schiene nach affen_11c0.c, Belege in re15_affen.h | enemy_ai_common.c +22/-13 in 19 Hunks (max 2 Zeilen), game_step_common.c +4/-2, main.c +5 |

## Tests (Stand Nachbesserung 1) — probes/r35_affen.cmake, test_r35_affen.c, 15 Eintraege
- Bisherige 11 (teile, band, ada, sprung, flug, wagen, brust, kdsonde, biss, frac, anker), davon erweitert:
  `wagen` (M2: Spawn-INIT, y im Wagen, Austritt), `sprung` (M3: Spur 0/1/2 + gemischt, 24 Pruefungen).
- NEU `takt` (M1: Gleich- und Wechseltakt gegen die GDB-Spur des Originals), `griff` (M1: Rear-up-Griff wie das
  Original, 0 HP), `npcband` (M4: NPC-Band 10D0/1050), `schrot` (M3: Schrot-Vergeltungssprung wie das Original).
- Alt-Riegel unit_member, unit_maggot_ai gruen.

## OFFEN (Stand Nachbesserung 1; ersetzt die Liste "Stand Abschluss")
- **Leons Szenen-Endlage in 11C0** weicht ~25 Einheiten ab (Port JUMP-Lauf A7 (-7157,-12355), Tuerweg t2
  (-7164,-12350), Original r3 (-7138,-12372)). Sie entscheidet im JUMP-Szenario, welcher der beiden
  Original-Takte entsteht (M1). Messweg: GDB-Spur des Originals ueber sub02 (Leons Plc_dest-Gang) Bild fuer
  Bild gegen den Port-Gang; dazu eine Original-Aufnahme ueber den TUERWEG (Generator-Flag per GDB `M`).
- **Wurf-Bahn ab Opfer-Clip-Bild ~19**: Port laeuft entlang z ~ -12500, Original nach SO und springt bei
  Bild 0x25 nach (-5381,-10551); Endlage ~1900 auseinander (Riegel `griff`, GDB `jnb1/g_griff.txt`).
  Messweg: 0x8001ad68 (Platzierung) Bild fuer Bild mit GDB-Haltepunkt, Gorilla-Anker +0xa0/+0xa2 mitlesen.
- **NPC-Klemme +0x82 in Szenen hinter Flags** (Marvin-Gang 10D0, Irons 6030/6031, 4001 0x49/0x4b mit
  +0x82 0 bei y -3500): Raumaufbau zeigt keinen laufenden NPC mit abweichendem Band; die Szenen selbst sind
  nicht gefahren. Messweg: Szenen-Trigger im Harness (Riegel `npcband` erweitern).
- Knockdown-Sonde FUN_8001c2dc gilt spielweit (auch Alligator), nur in ROOM11C0 nachgemessen.
- Gorilla-Schatten: INIT Box[6]+100/+200 (@0x801171d8-ec), Port 1000/500 — nicht angefasst.
- Heckklappe (Objekt 0) dunkler als im Original (Cut 12); Weg: FUN_8002c18c `jal 0x80053fc0` @0x8002c254.
- Member_set 0x13 (+0x1ba statt hp) in 10B1/2030/2031/3050/3051/50D0/50D1 nicht nachgemessen.
- Spawn-Wurzelaufruf: ausser Birkin jetzt auch 0x27; uebrige Typen wie vor Runde 35.
- Werkzeug: ViGEm-Geisterpad `USB\VID_045E&PID_028E\01` blockiert den Savestate-Recorder (SDL-0) bis zum
  Neustart; der GDB-Weg (`jnb1/gdbspur.py`, settings.ini [Debug] EnableGDBServer = true nur waehrend der
  Messung) ersetzt ihn und liefert jedes Bild.

## Fuer den Nutzer (Stand Nachbesserung 1)
- Sprachdateien: keine neuen Zeilen.
- Neue Assets fuer das Paket-/Android-Gate: keine.
- Spuerbar neu: (1) der Sprung nach drei Treffern gilt jetzt fuer ALLE Waffen (vorher sprang der Gorilla bei
  Schrotflinte/Magnum nach jedem Treffer); (2) packt ein Gorilla Leon von vorn, bleibt Leon kurz stehen,
  bevor er geworfen wird (wie im Original) — vorher wurde er im selben Bild weggerissen.
- Zur Aggressivitaet: der Port beisst jetzt in genau den Takten des Originals. Das Original hat selbst zwei
  Takte — beissen beide Gorillas gleichzeitig, kommt ein Biss etwa alle 1,7 s; laufen sie versetzt, alle 1,2 s
  (Leon stirbt dann schneller und wird nach hinten gedraengt). Beides ist im Original gemessen.
- Mess-Schalter (kein Spielverhalten): RE15_AFFEN_FUSS=1 -> affen_fuss.log (jetzt in affen_11c0.c).

### Suite (Nachbesserung 1)
- `bash re15_port/tools/local_build.sh all` (22:20-22:47, parallel bauende Nachbarbaeume, Code-Stand = HEAD
  nach dem Riegel `schrot`; danach nur Dossier/Kommentar im probes-Kopf geaendert):
  `=== LOCAL-BUILD-OK (all) — Tests 493/493` (1114 s; 478 Basis + 15 Riegel dieser Spur). Kein
  Fenster-Haken rot.
- DuckStation settings.ini nach den GDB-Messungen auf den Ausgangsstand zurueckgespielt (diff leer).

## Nachbesserung 2 (2026-10-03 spaet, nach Abnahme 1 = NICHT BESTANDEN, A1-A3)

Ausgangslage: Baum sauber, HEAD f604a1dc (Bericht J_abnahme_1.md). Der Vorgaenger dieser Nachbesserung hat
nichts hinterlassen (kein Commit, kein Scratch `jnb2/`). Scratch: `scratchpad/jnb2/`. Original-Spuren der
Nachbesserung 1 liegen in `scratchpad/jnb1/` (g_orig.txt, g_griff.txt, g_desync.txt, g_schrot.txt +
*_dec.txt; Werkzeuge gdbspur.py, gdec.py). Reihenfolge: A3 (Doku) -> A2 (Wurf-Bahn, Mechanismus +
Riegel) -> A1 (Szenen-Endlage / Takt im JUMP-Szenario).

### Stand (fortlaufend)
- [x] A3  - [x] A2 (Riegel + exe)  - [x] A1 (Riegel + exe JUMP + Tuerweg)  - [x] H2

### A3 — Kopfkommentar (3) in re15_affen.h (erledigt)
- Vorher: "Luft-/Sturz-Spuren (1/2) bleiben byte-true" (falsch seit 30484afa).
- Jetzt: (3) nennt alle drei Eintritte (@0x8011b064-70 / @0x8011b238-54 / @0x8011b44c-68) und alle drei
  Original-Exits (@0x8011b188-98 / @0x8011b3bc-ec / @0x8011b6b4-e8) und sagt, dass der Zaehler in allen dreien
  gilt — deckungsgleich mit dem Funktionskommentar bei re15_affen_treffer_zaehlen. Nur Kommentar, kein Code.

### A2 — Messung: WER bewegt Leon beim Wurf? (Original, GDB-Haltepunkte im Spieler-Tick)
- Werkzeug `jnb2/gdbmulti.py` (DuckStation-GDB-Server, settings.ini [Debug] EnableGDBServer nur waehrend der
  Messung; Original in `jnb2/settings.ini.orig`): r3 s033 direkt geladen, gleiches Experiment wie g_griff
  (`M800ace25,3:0f0000` bei VSync 10141), ab VSync 10135 Haltepunkte an den Stationen von FUN_80031c44
  (selbst disassembliert): Eintritt 0x80031c44, nach dem Kommando-Handler 0x80031cbc (= `jal 0x8002b544`),
  nach dem Koerper-Schub 0x80031cc4, vor/nach `jal 0x8002dc48` 0x80031d38/d40, vor/nach `jal 0x8002b498`
  0x80031d58/d60, vor/nach `jal 0x8003b0a4` 0x80031d70/d78 (a0 = Spieler+0x34 `addiu a0,s0,52` @0x80031d68,
  a1 = Radius `lhu a1,6(v0)` @0x80031d6c, a2 = 1 @0x80031d74), nach `jal 0x80037358` 0x80031d80, dazu
  0x8001ad68 (Eintritt, a0/ra). Spur `jnb2/g_wer.txt` (2090 Halte, VSync 10135-10470), Dekoder `wer.py`.
- Ergebnis (Bild = VSync-Paar):
  - Der Wurf-Handler (P2, `jal 0x8001ad68` @0x8011c294, ra 0x8011c29c, a0 = 0x800aca54) setzt Leon ABSOLUT
    (Anker (-7507,-10327) = FUN_8001ac38-Kopie, unveraendert ueber den ganzen Wurf). Danach im SELBEN
    Spieler-Tick: Koerper-Schub FUN_8002b544 (vs10171: +142/+113, vs10173: +192/+151, vs10175: +437/+178
    — der zweite Gorilla e2 steht im Koerperabstand) und die WANDKLEMME FUN_8003b0a4 (vs10171 -614/+585,
    vs10215 +217/-240, vs10219 -254/+180, vs10221 +21/+80). FUN_8002dc48 / FUN_8002b498 bewegen nie.
  - Opfer-Bild 0x23 (vs10219, +0x95 beim Eintritt = 0x23): Platzierung (-4334,-11423) -> Wand -> **(-4588,-11243)**;
    Opfer-Bild 0x24 (vs10221, letzte Platzierung: `sltiu v0,v0,0x25` @0x8011c278 prueft +0x95 VOR dem anim_set
    @0x8011c2b0) (-5402,-10631) -> Wand -> **(-5381,-10551)** = die beiden "Spruenge" der Abnahme (die
    g_griff-Dekodierung zeigt +0x95 NACH dem anim_set, daher dort "Bild 0x24/0x25").
  - Ab vs10223 KEINE Platzierung mehr, aber FUN_8003b0a4 schiebt Leon JEDES Bild um ~100-125 in wechselnde
    Richtungen (vs10223 +79/-82, +95/-66, +109/-38, -92/-54, -47/-100, ...) — auch nach der Freigabe (Zustand
    1/0/1), bis er bei vs~10461 auf (-3009,-11643) zur Ruhe kommt. Das "Weiterwandern ~100/Bild" der Abnahme ist
    also die Wandklemme, kein Clip-Wurzelweg (FUN_8001f314/f3bc schreiben +0x34/+0x3c nicht; RE_15_Quellcode_V2
    FUN_8001f3bc.c: nur Part-Records und +0x95).
- Port (Riegel griff vorher, `jnb2/griff_vorher.txt`): Platzierung + "letzter begehbarer Standpunkt"-Klemme in
  re15_victim_place (fuer JEDEN Greifer, Kommentar dort: 0x27 seit 2026-08-29), Wandklemme des Grabbed-Zweigs
  VOR der Platzierung (game_step_common.c: `re15_player_body_and_walls(c, pl, pl->x, pl->z)` im Grabbed-Zweig,
  die Platzierung laeuft erst danach in re15_player_victim_tick), danach nur Schub + Klemme-wenn-geschoben.
  Folge: ab Bild 0x21 haelt die Klemme Leon auf (-3965,-12507) (die Platzierungen 0x22-0x25 liegen in Zellen),
  danach steht er (keine Wandklemme auf den alten Standpunkt) -> frei bei (-3685,-12254).
- Reihenfolge im Original (FUN_80031c44 + Haltepunkt-Folge je Bild: Gorilla-Wurzel VOR dem Spieler-Tick):
  Gegner-KI -> Spieler: Kommando-Handler (cmd 5 = 0x8011c118-Kette: Platzierung) -> b544 -> dc48 -> b498 -> b0a4
  (Bezug +0x40/+0x44 = Lage am Ende des Vorbilds, `addiu a2,a2,64` @0x8003b4ac).
- **Port-Wandklemme gegen das Original (neuer Riegel `wand`):** die 168 Original-Bilder (Bezug = Lage beim
  Eintritt 0x80031c44 = Spiegel +0x40/+0x44, Eingang = Lage vor `jal 0x8003b0a4` @0x80031d70, Ausgang = Lage
  @0x80031d78; `jnb2/triples.py`) durch re15_collision_constrain (Band 0, r 450, Maske 1) geschickt:
  **168/168 bitgleich**, darunter alle 135 Bilder mit Schub (auch das ~100er-Zittern nach dem Wurf). Der
  Resolver des Ports IST FUN_8003b0a4 — falsch ist allein, WO der Port ihn beim Gorilla-Wurf aufruft
  (vor statt nach der Platzierung) und dass er die Platzierung zusaetzlich mit "letzter begehbarer Standpunkt"
  ueberschreibt.

### A2 — weitere Belege (selbst disassembliert, STAGE1.BIN) und Umsetzung
- **Erstes P2-Bild ist 0x0b, nicht 0x0c:** P1 0x8011c228: `jal 0x8001f314` @0x8011c23c (a3 = 0x200), danach
  `lbu v1,acae9` / `ori v0,zero,0xb` / `bne v1,v0,0x8011c3b8` / `sb 2,aca5a` @0x8011c244-5c — P2 beginnt im Bild,
  dessen +0x95 beim Eintritt 0x0b ist; P2 platziert mit diesem Wert (`sltiu v0,v0,0x25` @0x8011c278, `jal
  0x8001ad68` @0x8011c294) und posiert erst danach (`jal 0x8001f314` @0x8011c2b0). GDB: vs10171 Eintritt mo 010b ->
  AD68(a0=0x800aca54, ra 0x8011c29c). Port-Fenster war [0x0c,0x25) -> jetzt [0x0b,0x25).
- **Rueck-Variante:** `beq v1,zero,0x8011c228` @0x8011c208 (aca59 = 0 -> Front), sonst `sb 2,aca5a` @0x8011c210,
  `sb 0xc,acae9` @0x8011c214-1c, `j 0x8011c3b8` @0x8011c220 = KEIN anim_set, KEINE Platzierung im P0-Bild. Port: Seed
  0x0b (wird vor dem Platzieren auf 0x0c gezaehlt), Fenster ab 0x0b + Variante -> 0xc wird im Folgebild platziert.
- **Aufsteher P3-P6 aus der COMMON-Bank, Clip 0xb RUECKWAERTS:** P3 0x8011c2e8 (aca5a 4, acae8 0x10, acae9 0,
  acae3 7, `j 0x8011c34c` mit `addu a2,zero,zero` im Delay-Slot @0x8011c318); P5 0x8011c31c (aca5a 6, acae8 0xb,
  acae9 0, acae3 7) faellt in 0x8011c348 `ori a2,zero,0x1` = anim_set RUECKWAERTS; beide mit `lw a0,-13608(a0)` =
  DAT_800acad8 @0x8011c350 / `lw a1,-13376(a1)` = DAT_800acbc0 @0x8011c358 (= PL00.EMR/EDD, wie die Knockdown-Handler);
  Phasenvorschub `addu v1,v1,v0` / `sb v1,aca5a` @0x8011c370-78. P7 0x8011c384: `ori v0,zero,0x1` / `sw v0,aca58`
  @0x8011c384-8c (Kommando 1, nicht 2 wie der alte Port-Kommentar), +0x93 = 0 @0x8011c3a0, aca3c &= ~0x80 & ~0x40
  @0x8011c398-b4. GDB: Clip 0x10 T338-T353 (16 Bilder), Clip 0xb T354-T377 + Freigabe T378.
- **Griff-Paar ohne Koerper-Schub:** Gorilla-Wort |= 0x1000 beim Pin-Latch (`lw v0,0(v1)` / `ori v0,v0,0x1000` /
  `sw v0,0(v1)` @0x8011ac2c-38, v1 = g_entity(cur)), Spieler-Wort @0x8011ac3c-4c; Gorilla loescht seins in Phase 4
  (`addiu v1,zero,-4097` / `and` / `sw v0,0(a0)` @0x8011ad88-94). FUN_8002aec4 `andi 0x1000` auf das UND beider Worte
  @0x8002af14. GDB-Wort 0 (g_griff.txt): e1 0x60001811 ab T254, 0x60000811 ab T302; Leon 0x00001010 T254-T335.
  Der Port nahm nur Zombie-Greifer (sub 3..6) aus -> der Gorilla schob Leon in T273-T282 um ~420 weg (gemessen:
  Port T273 (-6729,-12534) statt Platzierung (-6542,-12913) = Schub vom Greifer, Abstand 1630 < 2050).
- **Umsetzung (Dateien):**
  - enemy_ai_common.c (Haken je 1 Zeile, Kommentar "Runde 35 Spur J (6x)"): Seed `? 0x0b : 0` (6b); Fenster
    `>= 0x0b + g_player_victim_variant` (6a/6b); P3 `anim_flags &= ~0x80`, P5 `anim_flags |= 0x80`, P7 Endpose
    (6c); re15_victim_place: Ersatzklemme nicht fuer 0x27 (6d); `re15_player_victim_gorilla()` (6d);
    re15_body_push_player: `if (pl_locked && re15_affen_griff_paar(e)) continue;` (6e); 3 Kommentarzeilen korrigiert.
  - game_step_common.c: `wurf`/`wurf_alt_x/z` (Bezug = Lage beim Eintritt in den Grabbed-Zweig = Spiegel +0x40/+0x44);
    im Grabbed-Zweig die Wandklemme fuer den Gorilla-Wurf NICHT vor der Platzierung; nach re15_player_victim_tick
    `re15_player_body_and_walls(c, pl, wurf_alt_x, wurf_alt_z)` (Schub @0x80031cbc -> Klemme @0x80031d70).
  - anim_select_common.c: COMMON-Bank-Gate um `re15_player_victim_own_bank()` erweitert (P3-P6).
  - affen_11c0.c: `re15_affen_griff_paar` (6e); re15_affen.h Abschnitt (6a)-(6e).
- **Messung nachher (Riegel, gleiche Lage wie das Original-Experiment):**
  - `wand`: Klemme 168/168 bitgleich; Schub-Kette T265-T267 mit e2 auf seiner Original-Lage: Schub UND Klemme
    bitgleich ((-7190,-10771)/(-7804,-10186), (-7140,-10733)/(-6686,-10348), (-6756,-11225)/(-7340,-11870)).
  - `griff` (vorher: ab T289 festgeklemmt auf (-3965,-12507), frei bei (-3685,-12254)): Pin T254, erste
    Platzierung **T265** (Original T265), Bahn T268-T290 im Mittel **18**, hoechstens 173 (T290) neben dem Original,
    T287/T288 (-3965,-12507) = Original (-3964,-12506), T289 (-4548,-11295) / T290 (-5374,-10724) (Original
    (-4588,-11243) / (-5381,-10551)); P3/P4 16 Bilder Clip 0x10 PL00 vorwaerts, P5/P6 25 Bilder Clip 0xb PL00
    rueckwaerts; **Freigabe T378** (Original T378) bei (-4580,-10518) (Original (-4759,-10633)); 0 HP; danach
    schiebt die Wandklemme Leon zur Ruhelage **(-3018,-11651)** (Original (-3009,-11643)).
    **[KORREKTUR Nachbesserung 3, Abnahme 2 M1]** Diese beiden Zahlen galten nur fuer den Zwischenstand
    `jnb2/griff_nach3/4.txt` (T290 (-5374,-10724)). Am Stand b3755c41/062e8f82 lieferte der Riegel: T290
    (-5376,-10728), **frei T378 bei (-5433,-10693)** (677 vom Original), **keine Ruhelage**, Bisse T396 (76 -> 70) und
    T450 (70 -> 64), Ende (-5893,-10250) HP 64. Die Pruefungen "Freigabe <= 400 vom Original" und "Ruhelage <= 60 bis
    T412" waren damit rot und wurden in 1ac1ea84 entfernt, ohne dass das Dossier es sagte. Messung, Ursache und neue
    Pruefungen: Abschnitt "Nachbesserung 3".
  - T265-T267 im Harness weicht ab (bis 1573), weil der Port-e2 dort ~400 weiter weg steht (Port e2 T264
    (-8707,-12438), Original (-8890,-12048)) und Leon nicht schiebt — die Kette selbst ist bitgleich (s. `wand`).
    Der Port-e2 steht T250-T264 still, das Original-e2 gleitet in derselben Zeit ~400 an Leons Koerper entlang
    -> gehoert zu A1 (Lage/Bewegung im Koerperkontakt), dort weiter. **[KORREKTUR N3, M2]** Das wurde dort nicht
    weiterverfolgt; Ursache (Ritt-Platzierung des Greifers fehlte, aec4 ohne Paar-Ausnahme, e2 faelschlich als
    Greifer vom Schub ausgenommen) und Behebung: Abschnitt "Nachbesserung 3".
- **Messung nachher, exe ueber den ECHTEN Tuerweg** (`jnb2/w6`, exe-Kopie re15_pc_jnb2.exe, Env wie Abnahme n6b:
  RE15_SET_FLAG=4:243,3:130, RE15_DEBUG_JUMP=11B0@240, RE15_PLAYER_POS an der Tuer, Quadrat-Tasten,
  RE15_INPUT_SCRIPT=W34.5,U2.5,W1; debug.log `DOOR FIRE` -> room11c0, sub02 F6, sub07 F1088):
  Rear-up S3 F1198, Pin F1203 (Leon mo 1), Leon steht bis F1213, **erste Platzierung F1214 = Pin + 11** (Original
  T254 -> T265 = +11; vorher Abnahme n6b F1215 = +12), Wurf-Bahn alle 2 Bilder wie die Clip-Daten, letzte Platzierung
  F1239 (Opfer-Bild 0x24: 1437 Einheiten — derselbe Clip-Sprung wie im Original, dort Platzierung 0x23 -> 0x24
  (-4334,-11423) -> (-5402,-10631) = 1330; die Abnahme las ihn als Port-Sprung), danach steht Leon (freier Boden,
  keine Zelle -> die Klemme schiebt nicht); **P3 Clip 0x10 ab F1286 = Pin + 83** (Original T337/338), Clip 0xb ab
  F1302 (+16), Freigabe F1327/28 (+25), HP 88 -> 88. Bilder `jnb2/w6_aufsteh.png` (F1284-F1328): in dieser Kamera
  verdeckt der Gorilla Leon beim Aufstehen (nur die Beine am Wagen sichtbar) — die Pose (Bank PL00, Richtung)
  belegt der Riegel `griff` ueber dieselbe Auswahlfunktion re15_actor_anim_select, die der Zeichner nimmt.
- Harness und exe verhalten sich jetzt gleich: die Abweichung "Harness bleibt stehen / exe springt bei 0x24" kam
  von der Ersatzklemme (Harness: Platzierungen 0x21-0x24 lagen in Zellen -> festgehalten; exe: freier Boden -> keine
  Klemme); jetzt laufen beide durch dieselbe Kette Platzierung -> Schub -> FUN_8003b0a4.

### A1 — Messung: Leons sub02-Gang im Original Bild fuer Bild (GDB) gegen den Port
- Werkzeug `jnb2/gdbpl.py`: r3 s001 (t=6.11, Leon am Spawn (-22604,14455)) direkt geladen, Haltepunkt am
  Spieler-Tick 0x80031c44, je Bild Spielerrecord 0x800aca54 (0x1f4 B) + aca58; 330 Bilder -> `jnb2/g_gang.txt`.
- Original: erster Lauf-Schritt (-22604,14455) -> **(-22413,14397)** (dx +191, **dz -58**), danach je Bild
  (+191,-58); Yaw 191 -> 189 (ab (-18593,13237)) -> 185 -> 183; Ankunft Bein 1 bei (-16108,12491); Bein 2 Yaw
  855..1015, x +2/Bild; Bein 3 Yaw 397/395, Schritt (+163,-115)/(+164,-114); **Endlage (-7138,-12372)**, Yaw 1513.
- Port (Abnahme n10, state.log): erster Schritt -> **(-22413,14398)** (**dz -57**), je Bild (+191,-57); Yaw 191 ->
  **193** (Drift zur anderen Seite, weil Leon jedes Bild 1 Einheit zu weit noerdlich steht); Bein 2 Yaw 1019; Bein 3
  Yaw 401/405, Schritt (+163,-115)/(+162,-116); Endlage (-7157,-12355), Yaw 1505.
- **Ursache = Rundung des Schritts.** FUN_800245d8 (selbst disassembliert): Vektor (+0x8c,0,0) @0x800245f0-80024600,
  `jal 0x800659d0` (RotMatrixY auf die Identitaet 0x80072d4c, Yaw +0x6a + a0) @0x80024660-64, `ctc2` der Matrix
  @0x80024674-90, `lwc2` V0 @0x8002469c-a0, **GTE MVMVA sf=1 (cop2 0x0486012) @0x800246ac**, `mfc2` IR1/IR2/IR3
  @0x800246bc-c4 -> dx = (R11*v) SAR 12, dz = (R31*v) SAR 12 mit R31 = -sin (psx-spx: "MAC1 = (R11*VX+...) SAR
  (sf*12)"). Fuer Yaw 191: -1183*200 = -236600 SAR 12 = **-58**. Der Port rechnet `z -= (s*speed) >> 12` = -57
  (re15_port/engine/src/actor_locomotion.c) — die Negation NACH der Verschiebung rundet zur Null statt nach unten.
  Eine Einheit je Bild ueber ~230 Laufbilder = die 26 Einheiten der Szenen-Endlage.
- **Messung nach dem Rundungs-Fix (exe `jnb2/n10`):** Bein 1 bitgleich, ab der Wende Abweichung: Original dreht
  am Ende von Bein 1 sechs Bilder auf der Stelle (183 -> 759, +96) und nimmt im 7. Bild (Eintritt 759) noch +96 ->
  855 ohne Schritt; der Port wechselte schon beim Eintritt 663 in den Laufzustand. **Ursache = Kegelgrenze.**
  Mode-5-Handler 0x80030d28 Zustand 1 (selbst disassembliert): `jal 0x8001ab9c` mit a2 = 0x15e @0x80030db8-bc VOR
  dem Slew `jal 0x8001aac4` (a2 = 0x60) @0x80030df4-f8. FUN_8001ab9c: d = (Peilung - Yaw + k) & 0xfff @0x8001abe4-ec,
  `slt v0,v1,v0` mit v0 = 2k @0x8001abf8-fc -> "im Kegel" <=> -k <= Peilung - Yaw < k (HALBOFFEN). Der Port pruefte
  `|delta| <= 0x15e` (geschlossen) -> bei delta = +350 einen Zustand zu frueh. Gleicher Test im Mode-9-Handler
  (`jal 0x8001ab9c`, a2 = 0x60 @0x800313d0-d4) -> dort ebenso `< 0x60`.
- **Messung nach beiden Walker-Fixes (exe `jnb2/n10b`, Werkzeug `gangvgl.py`):** alle **207** Lage-/Yaw-Zustaende
  des sub02-Gangs **bitgleich** mit der Original-Spur, Endlage **(-7138,-12372) Yaw 1513** = Original.
- **Aber der Takt blieb 36** (n10b: Bisse alle 35-36 Bilder, Tod F2062). Die Lage allein erklaert ihn also nicht
  (= die Gegenprobe, die die Abnahme verlangte). Weiter mit dem Kampf Bild fuer Bild (`kampf.py`) gegen g_orig:
  Heavy des Port-e2 (S3) mit Yaw **160**, Original **4228** (= 132, das Original maskiert +0x6a hier nicht);
  dadurch rutscht S3 waehrend des Heavy ~400 an Leons Koerper entlang, steht danach ~370 weiter suedlich und
  sein Biss-Lunge schiebt Leon nicht in den Bogen des anderen Gorillas -> Wechseltakt.
- **Ursache = die Zufallszahlen.** Im Original drehte e2 im Heavy-Anlauf (B[4]) 21 Bilder lang um genau **+73**
  und lief mit +0x8c **188** (g_orig, e2 +0x9e/+0x8c je Bild); der Port zog je Bild neu (xorshift-Ersatz) 64-95.
  FUN_8001af20 (selbst disassembliert, s. auch Memory reai-v2-rng-determinism) liest den State @0x800ac774 NICHT
  (`lhu t1` @0x8001af28 tot), sondern hasht das **a0-Register des Aufrufers**. GDB-Zensus aller Ziehungen
  (`jnb2/gdbrng.py`, Haltepunkt 0x8001af20, ra/a0/g_entity(cur); `g_rng.txt` ab s033, `g_rng2.txt` ab s026):
  - B[4] @0x80118164/@0x8011817c (ra 0x8011816c/0x80118184): a0 = **Entity+0x34** (21/21 + 22/22: 0x800ad048 =
    e2+0x34, danach verkettet 0xa0e8) -> (8, 9) -> 188 / 73. Ausnahme: im Bild, in dem A[3] auf sub 4 schaltet,
    laeuft B[4] mit dem a0 aus A[3] (vs9597: 0x027e6945 = Abstand^2 aus a804).
  - B[3] CHASE @0x80117ce0/@0x80117d1c (ra 0x80117ce8/0x80117d24): a0 = **0xbb8**, wenn Spieler +0x93 != 0 (Delay-
    Slot `ori a0,zero,0xbb8` @0x80117a60 des `bne` @0x80117a5c), sonst das a0 nach a804(0xbb8,0x180) @0x80117a6c:
    a804 laesst nach SquareRoot0 a0 = dx^2+dz^2 stehen (SquareRoot0 @0x80065f60 schreibt a0 nicht) und kehrt bei
    r < d ohne atan2 zurueck (`slt s0,s0,s1` / `bne` @0x8001a870-74); sonst atan2 FUN_8001a6d4 -> a0 = dz<<12 bei
    dx = 0 (Delay-Slot @0x8001a718), sonst der CORDIC-Rest y_11 aus catan (`lw a0,56(a1)` @0x80065928 in der
    12. Iteration). Modell (`jnb2/a0model.py`) gegen die Original-Ziehungen: **e1 517/517, e2 443/443** (die
    uebrigen 15 sind die Kette nach der Eintritts-Ziehung: H(0xbb8) -> a0 0x17cf).
  - B[0] Leerlauf-Timer @0x80117594 (ra 0x8011759c): a0 = **Entity-Zeiger** (vs9117: 0x800ad014) -> H = 180 ->
    +0x9c = 239. Port (xorshift): 264 -> Heavy 26 Bilder spaeter (Port F1461 = Freigabe + 390, Original vs9765 =
    Freigabe vs9037 + 364).
- Freigabe im Original exakt: aca58 4 -> 1 bei **vs9037** (`jnb2/g_frei.txt`, s026); Tod vs11423 (g_orig F891)
  -> **1193 Bilder = 39,8 s**.

### A1 — Umsetzung (Dateien, Konstanten mit Beleg)
- actor_locomotion.c (Plc_dest-Walker des Spielers, 1 Zeile je Stelle, Kommentar "Runde 35 Spur J"):
  `a->z += ((-s * speed) >> 12)` statt `a->z -= ((s * speed) >> 12)` (GTE MVMVA sf=1 @0x800246ac, R31 = -sin aus
  RotMatrixY @0x80024660); Ausrichtungs-Kegel `delta >= -0x15e && delta < 0x15e` und Mode-9-Ankunft
  `delta >= -0x60 && delta < 0x60` (FUN_8001ab9c `slt` @0x8001abfc; Aufrufe @0x80030db8 / @0x80030b80 / @0x800313d0);
  die ungenutzte `abs_delta` entfaellt.
- re15_damage.c re15_player_knockback_delta (Rueckstoss der Biss-/Treffer-Reaktion ueber FUN_800245d8): dieselbe
  dz-Rundung (1 Zeile).
- affen_11c0.c / re15_affen.h (7): `re15_affen_rng_a0` (= FUN_8001af20 @0x8001af30-4c auf einem gegebenen a0),
  `re15_affen_psx_entity` (0x800acc2c + (Slot-1)*0x1f4), `re15_affen_a804_a0` (a0 nach a804: d^2 bei r < d, sonst
  atan2-Rest), `re15_affen_b3_a0` (B[3]-Eintritt), `re15_affen_b0_a0` (B[0]-Eintritt).
- enemy_ai_common.c (je 1-2 Zeilen): B[0]-Timer, B[3]-Eintritts- und Bild-Ziehung, B[4]-Ziehungen (a0 = Entity+0x34,
  wenn Path B vor a9cc kurzschliesst; sonst bleibt der xorshift-Ersatz), B[7]-Absprung (a0 = g_entity @0x80118a24).

### A1 — Messung nachher
- exe JUMP-Szenario ohne Eingabe (`jnb2/n10d`, gleiche Eingabe wie r3/Abnahme n10): Gang 207/207 bitgleich,
  Endlage (-7138,-12372) Yaw 1513; Heavy-Anlauf von S3 Yaw 2768, 2841, ... +73 je Bild bis 132 (Original 4228 =
  132), Lage (-9419,-11137) (Original (-9419,-11139)); Heavy F1436 = Freigabe F1071 + 365 (Original +364);
  Bisse relativ zum Heavy 105/157/209/260/313/363/417/466/521/569/625/672/729/775/832(Tod) gegen Original
  104/156/206/259/309/362/413/465/517/568/621/671/725/774/829 — **jeder Biss 0-4 Bilder neben dem Original**,
  Abstaende 52,52,51,53,50,54,49,55,48,56,47,57,46,57 (Original 52,50,53,50,53,51,52,52,51,53,50,54,49,55);
  **Tod Freigabe + 1197 Bilder = 39,9 s** (Original 1193 = 39,8 s; vorher Abnahme n10: 999 = 33,3 s).
- Bild fuer Bild im Kontakt (n10d F1526-F1546 gegen g_orig F152-F172, Versatz 1374): S3 F1532 (-9066,-12016) =
  Original F158 (-9066,-12016) exakt; e1 committet F1534 = Original F160; Leon wird aber EIN Bild spaeter
  geschoben (Port F1534 (-7032,-12373) = Original F158 (-7045,-12373) ~ F160): der Port schiebt den Spieler im
  Spieler-Zweig VOR der Gegner-KI (game_step), das Original im Spieler-Tick NACH den Entity-Ticks
  (Haltepunkt-Folge Gorilla-Wurzel -> 0x80031c44). Daraus die restlichen 0-4 Bilder -> OFFEN.
- Riegel `szene` (Raum-Harness, gleiche Eingabe): Gang 207/207, Heavy +365, Takt Mittel 51,5 (kuerzester 46),
  Tod +1197 — gruen.

### H2 (Hinweis der Abnahme) — re15_maggot_a780 Operandenfolge (behoben)
- FUN_8001a780 (selbst disassembliert): `lh v0,106(a0)` (a0 = Spieler) @0x8001a788, `lh v1,106(v1)` (g_entity(cur))
  @0x8001a78c, `subu v0,v0,v1` @0x8001a794, `addiu 1024` / `andi 0xfff` / `slti 2048` @0x8001a798-a4. Der Port
  rechnete Gorilla - Spieler (unterscheidet sich nur bei Differenz genau +-0x400). Jetzt Spieler - Gorilla (1 Zeile).

## Tests (Stand Nachbesserung 2) — probes/r35_affen.cmake, test_r35_affen.c, 17 Eintraege
- Bisherige 15 (teile, band, ada, sprung, flug, wagen, brust, kdsonde, biss, frac, anker, takt, griff, npcband, schrot).
- `griff` NEU gefasst (A2): Pin T254, erste Platzierung T265 (= Opfer-Bild 0x0b), Wurf-Bahn T268-T290 gegen die
  Original-Spur (Mittel 18-19, max 177), P3/P4 Clip 0x10 aus PL00 vorwaerts (16 Bilder), P5/P6 Clip 0xb aus PL00
  RUECKWAERTS (25 Bilder), Freigabe T378, nach Bild 0x24 bewegt nur noch die Wandklemme (87/87 Bilder), 0 HP.
  **[KORREKTUR N3]** Nicht genannt war: Commit 1ac1ea84 hat zwei rot gewordene Pruefungen ENTFERNT ("Lage bei der
  Freigabe <= 400 vom Original (-4759,-10633)" und "Ruhelage <= 60 von (-3009,-11643) bis T412"); der Riegel lieferte
  frei (-5433,-10693) und keine Ruhelage. Ersatz mit Messung: Abschnitt "Nachbesserung 3" (Tests).
- NEU `wand` (A2): re15_collision_constrain = FUN_8003b0a4 in 168/168 Original-Bildern; Schub + Klemme T265-T267
  mit e2 auf Original-Lage bitgleich.
- NEU `szene` (A1): Raumeintritt wie r3, keine Eingabe: sub02-Gang 207/207 bitgleich, Heavy Freigabe+365
  (Original +364), Biss-Takt Mittel 51,5 / kuerzester 46 (Original 51,8 / 49), Tod +1197 (Original +1193).
- Alt-Riegel unit_member, unit_maggot_ai.

## OFFEN (Stand Nachbesserung 2; ersetzt die Liste "Stand Nachbesserung 1")
- **Spieler-Schub-Reihenfolge**: der Port schiebt den Spieler aus den Gegner-Koerpern im Spieler-Zweig VOR der
  Gegner-KI (game_step), das Original im Spieler-Tick NACH allen Entity-Ticks (FUN_80031c44 `jal 0x8002b544`
  @0x80031cbc; GDB-Haltepunktfolge Gorilla-Wurzel -> 0x80031c44). Gemessen: im Biss-Kontakt wird Leon 1 Bild
  spaeter geschoben (n10d F1532-F1536 gegen g_orig F158-F162); daraus die restlichen 0-4 Bilder je Biss.
  Spielweite Reihenfolge, nicht nur 11C0 -> nicht in dieser Spur angefasst. Messweg: Station SCHUB je Bild gegen
  die Original-Stationen 0x80031cbc/cc4 (Werkzeug jnb2/gdbmulti.py).
- **A/B im selben Tick**: der Gorilla-Zustand-1-Handler liest +0x5 nach A[sub] neu und ruft B[neu] im selben Bild
  (`lbu v0,5(v0)` @0x80117358, `jalr` @0x80117378); der Port bricht nach jedem Wechsel ab und laesst B im Folgebild
  laufen (gemessen: Heavy +365 statt +364). Betrifft alle Gorilla-Subs; die Riegel takt/schrot/szene liegen trotzdem
  bei 0-4 Bildern.
- **RNG-a0 nicht modelliert** (bleibt xorshift-Ersatz, Memory reai-v2-rng-determinism): B[4] im Bild des
  A[3]->4-Wechsels (a0 = A[3]-Rest, vs9597 0x027e6945) und bei Path B mit a9cc-Aufruf (a0 = atan2-Rest);
  Path-B-Muenze @0x801180a8 (a0 nach a9cc = atan2-Rest, vs9599: 5); B[7]-Anlauf @0x8011898c (a0 haengt am
  Eintrittsweg: Path B -> Spieler-Zeiger nach a780 (vs9599: 0x800aca54), Zonen-Sprung -> Entity+0x34, Flinch-Exit
  -> Abstand^2); B[1] (sub 1, in 11C0 nicht erreicht); Spieler-Ziehungen 0x800320b0/0x8003218c (a0 = 0).
- **Gleiche Rundung ausserhalb dieser Spur**: climb_common.c:360 (`p->z -= (s_speed*s)>>12`) und
  enemy_ai_re2_spider.c:257 rechnen dz ebenfalls als -(s*v>>12); FUN_800245d8/GTE rundet nach unten. Nicht
  angefasst (andere Raeume/Spuren), Beleg @0x800246ac.
- Weiter offen aus Nachbesserung 1: NPC-Klemme +0x82 in Szenen hinter Flags (Marvin 10D0, Irons 6030/6031, 4001);
  Knockdown-Sonde spielweit nur in 11C0 nachgemessen; Gorilla-Schatten Box[6]+100/+200 (@0x801171d8-ec);
  Heckklappe dunkler (FUN_8002c18c `jal 0x80053fc0` @0x8002c254); Member_set 0x13 in 10B1/2030/2031/3050/3051/
  50D0/50D1 nicht nachgemessen.
- Werkzeug: ViGEm-Geisterpad blockiert den Savestate-Recorder; der GDB-Weg (gdbspur/gdbmulti/gdbpl/gdbrng in
  `jnb1/`/`jnb2/`, settings.ini [Debug] EnableGDBServer nur waehrend der Messung) ersetzt ihn.

## Fuer den Nutzer (Stand Nachbesserung 2)
- Sprachdateien: keine neuen Zeilen. Neue Assets fuer das Paket-/Android-Gate: keine.
- Spuerbar neu: (1) Wirft ein Gorilla Leon, laeuft der Wurf jetzt wie im Original: Leon fliegt die volle Bahn,
  landet notfalls im Hindernis und wird Bild fuer Bild herausgeschoben, steht dann mit der richtigen (rueckwaerts
  gespielten) Aufsteh-Animation auf — vorher blieb er mitten im Wurf an einer Ersatzklemme haengen und der
  zweite Gorilla schob ihn weg. (2) Ohne Eingabe beissen die Gorillas im Original-Rhythmus (etwa alle 1,7 s,
  beide zugleich) und Leon stirbt nach ~40 s wie im Original — vorher 1,2 s-Rhythmus und Tod nach 33 s.
  (3) Leons Laufwege in Zwischensequenzen (Plc_dest) enden jetzt auf der Einheit genau wie im Original (vorher
  bis ~1 Einheit je Schritt zu kurz, ueber einen langen Gang ~25 Einheiten); das gilt fuer alle Raeume.
- Bedienhinweis: keine. Mess-Schalter (kein Spielverhalten): RE15_AFFEN_FUSS=1 -> affen_fuss.log.

## Messung nachher am Endstand (exe-Kopie re15_pc_jnb2.exe = Stand nach allen Aenderungen, `jnb2/fin_runs.sh`)
- **A1, gleiche Eingabe wie das Original** (`n10e`: RE15_DEBUG_JUMP=11C0@240, keine Eingabe): Gang 207/207 bitgleich,
  Endlage (-7138,-12372) Yaw 1513; Freigabe (sub07) F1071; Bisse ab dem Heavy +105, dann Abstaende
  53,51,52,52,51,53,50,54,49,55,48,56,47,57 (Gleichtakt, Original 52,50,53,50,53,51,52,52,51,53,50,54,49,55);
  **Tod F2269 = Freigabe + 1198 Bilder = 39,9 s (Original 1193 = 39,8 s)**. Abnahme n10 vorher: 36er-Takt, 999 Bilder.
- **A1 ueber den ECHTEN Tuerweg** (`t2`: Generator-Flags, 11B0-Tuer, Quadrat-Tasten, danach keine Eingabe):
  `DOOR FIRE` -> room11c0, sub07 F1088, Leon (-7148,-12363) Yaw 647 (anderer Start als der JUMP-Spawn), dieselben
  Abstaende wie n10e, **Tod F2286 = Freigabe + 1198 = 39,9 s** (Abnahme n2 vorher: Tod F2079 = +991, 36er-Takt).
- **A2 ueber den Tuerweg, Leon im Freien** (`w7`, Eingabe wie Abnahme n6b): Griff 1 Pin F1202, Clip 0x10 F1285
  (+83), Clip 0xb F1301 (+16), frei F1327 (+25... +26), HP 88 -> 88; Griff 2 (Rueck-Variante: Opfer-Clip ab
  Bild 0x0c) Pin F1572, Clip 0x10 F1644 (+72 = 83 - 11), Leon wird waehrend P3 von der Wandklemme aus einer Zelle
  geschoben (F1644 (-7347,-9511) -> F1660 (-8275,-8608)), frei F1686, 0 HP. Brustschlag (Clip 3) nach beiden Griffen
  (F1250, F1609).

### Suite (Nachbesserung 2)
- Lauf 1 (Code-Stand vor B[0]-Verfeinerung/B[7]/H2): 494/495 — rot nur `unit_plc_back_yaw_1090`: Regressions-Pins
  auf PORT-Messwerte (Endyaw 111, (-77,-1997)) und `Mode 5 Endyaw == bearing`, die beide die alte Rundung zur Null
  kodierten. Nachgezogen (Endyaw 133, (-77,-1999); Mode-5-Drift <= 0x20 wie R1, Beleg: Original-Drift 191 -> 183
  im 11C0-Gang), Kommentar mit @0x800246ac / @0x8001abfc.
- Lauf 2 (Endstand): `bash re15_port/tools/local_build.sh all` ->
  `=== LOCAL-BUILD-OK (all) — Tests 495/495` (478 Basis + 17 Riegel dieser Spur). Kein Fenster-Haken rot.
- DuckStation settings.ini nach den GDB-Messungen auf den Ausgangsstand zurueckgespielt (EnableGDBServer = false).

### Nachbesserung 2 — Ergebnis je Mangel
| Mangel | Ursache (gemessen) | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| A1 Tod 6 s frueher, 36er-Takt, Endlage 26 Einheiten | (1) Plc_dest-Schritt dz rundete zur Null (-57 statt -58); (2) Kegeltest geschlossen statt halboffen; (3) xorshift-Ersatz statt des a0-Hashs des Originals in B[0]/B[3]/B[4] (Heavy-Anlauf drehte 64-95 statt 73 je Bild) | GTE MVMVA @0x800246ac; FUN_8001ab9c `slt` @0x8001abfc; FUN_8001af20 @0x8001af30-4c + GDB-Zensus (B[4] 43/43, B[3] 960/960, B[0] vs9117) | actor_locomotion.c (2 Stellen), re15_damage.c (Rueckstoss), affen_11c0.c (7)/(7b)/(7c), enemy_ai_common.c (Ziehstellen) | Gang 207/207 bitgleich; JUMP und Tuerweg: Gleichtakt, Tod 39,9 s (Original 39,8 s); Riegel `szene` |
| A2 Wurf-Bahn ab Bild ~0x22, Harness != exe | Ersatzklemme "letzter begehbarer Standpunkt" statt Platzierung -> Schub -> FUN_8003b0a4; Wandklemme vor statt nach der Platzierung; Griff-Paar nicht vom Schub ausgenommen; P2 ab 0x0b; Clip 0xb rueckwaerts aus COMMON | GDB-Stationen 0x80031cbc/d70/d78; Paar-Bit 0x1000 @0x8011ac34-38/@0x8011ad8c-94; P1/P2 @0x8011c244-5c/@0x8011c278; P5 `ori a2,1` @0x8011c348 | enemy_ai_common.c, game_step_common.c, anim_select_common.c, affen_11c0.c (6a)-(6e) | Riegel `wand` 168/168 + Schub-Kette bitgleich; `griff` Bahn T268-T290 Mittel 19, Freigabe T378, 87/87 Bilder nur Wandklemme; exe Tuerweg: Pin+11/+83/+16 |
| A3 Kopfkommentar (3) | veraltet seit 30484afa | — | re15_affen.h (3) | — |
| H2 a780-Operanden | Gorilla - Spieler statt Spieler - Gorilla | @0x8001a788-a4 | 1 Zeile | Riegel gruen |

## Nachbesserung 3 (2026-10-04, nach Abnahme 2 = NICHT BESTANDEN, M1/M2)

Ausgangslage: Baum sauber, HEAD 062e8f82 (Bericht J_abnahme_2.md). Alle sechs Nutzer-Punkte dort "erfuellt";
offen sind nur M1 (Dossier-Zahl Z. 858-859 stimmt am HEAD nicht, zwei Pruefungen still entfernt, "chaotisch"
nicht gemessen) und M2 (Port-e2 steht T250-T264 still, Original-e2 gleitet ~400; weder geklaert noch unter OFFEN).
Scratch: `scratchpad/jnb3/`. Original-Spuren: `jnb1/g_griff.txt` (+ `_dec`), `jnb2/g_wer.txt`.

### Stand (fortlaufend)
- [x] M1 (a) Zahlen auf HEAD  - [x] M1 (b) Entfernung offenlegen  - [x] M1 (c) Chaos messen  - [x] M2 Ursache e2 (behoben)  - [x] Suite  - [x] exe Tuerweg

### M1 — Messung vorher (HEAD 062e8f82) und Herkunft der Dossier-Zahlen
- Riegel `griff` am HEAD (Abnahme `jabn2/griff_v.txt`, deckungsgleich mit `jnb2/fin_griff.txt` 00:41):
  T290 (-5376,-10728), **frei T378 bei (-5433,-10693)** (677 vom Original (-4759,-10633)), keine Ruhelage:
  Leon wird T396 (76 -> 70) und T450 (70 -> 64) gebissen, Ende T495 (-5893,-10250) HP 64.
- Die Zahlen in Z. 858-859 ("frei (-4580,-10518)", "Ruhelage (-3018,-11651)") stammen aus `jnb2/griff_nach3/4.txt`
  (23:47/23:49, Stand VOR den A1-Aenderungen): dort Pin T254 bei (-6658,-12487), T290 **(-5374,-10724)**,
  frei (-4580,-10518), Ruhelage (-3018,-11651) ab T392. Ab `griff_nach5.txt` (00:19, nach den A1-Aenderungen
  dz-Rundung/Kegel/RNG-a0): Pin T254 (-6656,-12489), T290 **(-5376,-10728)** (2 bzw. 4 Einheiten anders),
  frei (-5433,-10693), keine Ruhelage -> die beiden PRUEFs "Lage bei der Freigabe <= 400 vom Original" und
  "Ruhelage <= 60 von (-3009,-11643) bis T412" wurden ROT (griff_nach5: "FEHLER: ... (-5433,-10693)",
  "FEHLER: ... Ruhelage (0,0) in T-1"). Commit 1ac1ea84 (00:20) hat beide entfernt und durch die
  Strukturpruefung "nach Bild 0x24 bewegt nur die Wandklemme (87/87)" ersetzt; das Dossier (Z. 858-859,
  Testliste Z. 966-975, OFFEN) hat das nicht nachgetragen. Das war ein Fehler des Dossiers.
- Original (Riegel-Tabelle s_wand_orig, GDB `jnb2/g_wer.txt`): ab T291 (vs10223) bis T414 (vs10469) gilt in
  **124/124** Bildern Bezug == Eingang == Ausgang des Vorbilds (Python-Pruefung ueber die Tabelle) -> nach
  Opfer-Bild 0x24 ist die Bahn bis zur Ruhe REINE Iteration von FUN_8003b0a4 (@0x80031d70, Bezug +0x40/+0x44).

### M2 — Messung vorher und Ursache
- Port (Riegel `griff` HEAD, `jabn2/griff_v.txt`): e1 (Greifer) steht T254-T273 fest auf (-5865,-14392..-14396),
  springt T274 um 425 nach (-5683,-14784) (Koerper-Schub), e2 steht T249-T264 fest auf (-8706..-8710,-12437..-12448).
- Original (`jnb1/g_griff_dec.txt`, T = F - 1; Tabelle `jnb3/e12_orig.inc`): e1 laeuft ab dem Pin die Bahn des
  Clips 0x1c: T254 (-5882,-14389), T256 (-6062,-14169), T258 (-6199,-14044), T260 (-6404,-13856), T261
  (-6471,-13796), zurueck T264 (-6307,-13944), T267 (-6191,-14052). e2 steht bis T255 (Kontakt mit Leon, d 2050-2053),
  danach **Abstand e1-e2 = 3200 (= 2 x 1600) in T256-T264** (3205, 3201, 3208, 3209, 3206, 3168, 3174, 3202, 3204;
  aus den Lagen gerechnet) und e2 weicht nach (-9094,-12016) aus (T262), Yaw 27 -> 123: **e2 wird vom
  vorrueckenden e1 geschoben** (b544 im Gorilla-Wurzelschwanz). Danach T268 Sprung e2 (-8655,-12000) ->
  (-9377,-12112) = Abstand 2051 zu Leon (-7340,-11870): aec4(Spieler, Gorilla) des Wurzelschwanzes.
- **Ursache = die Ritt-Platzierung des Greifers fehlt im Port.** STAGE1.BIN, selbst disassembliert:
  - Phase 2 (Pin-Latch 0x8011abe8): `jal 0x8001ac38` a0 = Spieler @0x8011ac18 (Anker), Paar-Bits @0x8011ac2c-54,
    `jal 0x8001a780` @0x8011ac50, Spieler +0x93 |= 1 @0x8011ac8c-a0, **`jal 0x8001a8f8` a0 = Spieler+0x34
    (`addiu a0,s0,-372` @0x8011ac60), `ori a1,zero,0x800` @0x8011acb0** (Yaw-Fang des Greifers auf Leon), dann
    KEIN Sprung: Phase 2 faellt in den Koerper von Phase 3.
  - Phase 3 (0x8011acb4): `lw a0,g_entity` @0x8011acc0, `lw a1,132(v0)` (+0x84) @0x8011acc4, `lw a2,364(v0)`
    (+0x16c) @0x8011acc8, **`jal 0x8001ad68` @0x8011accc** = der GORILLA SELBST wird absolut aus seinem Anker
    platziert; danach anim_set `jal 0x8001f314` (a2 = 0, a3 = 0x200) @0x8011ace8-ec, +0x6 += v0 @0x8011acfc-ad0c.
  - FUN_8001ad68 (PSX.EXE): FUN_8001ae38 (Versatz des laufenden Bildes) @0x8001ad84, RotMatrixY(+0x6a)
    @0x8001addc, ApplyMatrix @0x8001adec, `+0x34 = +0xa0 + vx` @0x8001adf4-ae04, `+0x3c = +0xa2 + vz`
    @0x8001ae08-18 (= Port re15_clip_root_motion_abs).
  - FUN_8001a8f8 (PSX.EXE): Peilung atan2 @0x8001a928, d = (s1 + Peilung - Yaw) & 0xfff @0x8001a960-68,
    `slt` gegen 2*s1 @0x8001a974 -> bei s1 = 0x800 immer Yaw := Peilung (`sh a0,106(v1)` @0x8001a984)
    (= Port re15_enemy_steer_point mit slew 0x800).
  - Port Phase 2/3 (enemy_ai_common.c case 15): Anker ja (re15_affen_pin_anker), aber weder Yaw-Fang noch
    Platzierung — e1 bleibt stehen, schiebt e2 nicht, und Leons Kette T265-T267 trifft e2 an der falschen Stelle.
  - Kein Harness-Startzustand: auch das Original-e2 steht bis T255 (wie der Port); die Bewegung beginnt erst mit
    dem Kontakt zu e1 in T256.
- **Zweite Ursache (gemessen nach dem ersten Bau mit Ritt-Platzierung, `jnb3/griff1.txt`):** e1 blieb T255-T265 auf
  genau **2050** zu Leon stehen ((-5867,-14385) -> d 2053, (-5868,-14382) -> d 2050; Original-Bahn rueckt ~830 vor) und
  lief erst ab T266 los, als Leons Platzierung ihn freigab. Taeter: der Port-Wurzelschwanz des Gorillas
  (enemy_ai_common.c, Ende re15_maggot_ai_tick) ruft aec4(Spieler, Gorilla) OHNE die Paar-Ausnahme. Original
  FUN_8002aec4 (PSX.EXE, selbst disassembliert): `lw a0,0(s3)` / `lw v1,0(s2)` @0x8002aef8-fc, `and v0,a0,v1` /
  `andi v0,v0,0x1000` / `bne v0,zero,0x8002b464` @0x8002af14-1c -> beide Wort-Bits 0x1000 (Gorilla @0x8011ac34-38,
  Spieler @0x8011ac4c-54) = KEIN Schub. Die Spieler-Seite (re15_body_push_player) hatte die Ausnahme seit
  Nachbesserung 2 (6e), die Gorilla-Seite nicht. Haken: `if (!(re15_player_is_grabbed() && re15_affen_griff_paar(e)))`.
- **Messung nach Bau 2 (`jnb3/griff2.txt`, Ritt-Platzierung + Paar-Ausnahme im Wurzelschwanz):** e1 T254-T268 2-17
  neben dem Original (T261 d2; vorher bis 844), e2 T254-T265 hoechstens 78 (T260; T262 d7, T264 d6), e2-Weg T254->T262
  579 (Original 542), Abstand e1-e2 3189-3214 wie im Original. **M2 im Kern behoben.**
- **Dritte Ursache (T265-T267):** Leons Eingang T265 im Port = reine Platzierung (-7357,-10904), Original
  Platzierung + Schub von e2 (-7332,-10884) -> (-7190,-10771). Port-Spielerschub re15_body_push_player ueberspringt
  waehrend Leon gehalten wird JEDEN Gegner mit Zustand 1 / Sub 3..6 (Zombie-Paar-Naeherung, typ-unabhaengig) — e2
  steht in Gorilla-Sub 3 (Jagd) und war damit faelschlich "Greifer". Original FUN_8002aec4 nimmt nur das Paar mit
  BEIDEN Bits 0x1000 aus (@0x8002af14-1c); e2 hat das Bit nie (gesetzt nur im Pin-Latch des Greifers @0x8011ac34-38).
  Haken: Zombie-Bedingung `&& e->type != 0x27u` (der Gorilla hat seine eigene Paar-Abfrage re15_affen_griff_paar).
  Ob die typ-unabhaengige Bedingung auch andere Greifer-Typen (Hund 0x20, Kraehe 0x21) falsch ausnimmt: nicht
  Gegenstand dieser Spur -> OFFEN.
- **Messung nach Bau 3 (`jnb3/griff3.txt`):** Kette T265-T267 Eingang 5/4/12, Ausgang 17/16/11 neben dem Original
  (vorher Eingang 213/276/496, Ausgang 1241/1573/207); e1 T254-T288 hoechstens 21, e2 T254-T270 hoechstens 78.

### M1 (c) — Chaos gemessen + Weg 2 (Riegel `griff`, Bau 3)
- **Weg 1:** re15_collision_constrain ab dem Original-Zustand T290 (-5381,-10551) iteriert (Bezug = Eingang = Vorbild):
  **124/124 Bilder T291-T414 bitgleich** mit der Original-Bahn, Freigabe T378 (-4759,-10633), Ruhe T410 (-3009,-11643).
- **Zerlegung des Riegel-Laufs:** dieselbe Iteration ab der Riegel-Lage T290 (-5376,-10728) liefert 88/88 Bilder des
  Riegels T291-T378 und dessen Freigabe (-5433,-10693) -> die 677 Einheiten bei der Freigabe sind allein der Versatz
  bei T290 (177).
- **Empfindlichkeit:** 24 Starts im 5x5-Gitter (+-1/+-2 Einheiten) um den Original-T290: alle 24 laufen nach 1-9 Bildern
  (T292-T300) > 50 vom Original weg; bei T378 liegen alle 24 > 100 und 20 > 400 neben der Original-Freigabe
  (153 .. 5825). Die beiden Port-Staende vor/nach A1: (-5374,-10724) -> T378 (-4580,-10518), Ruhe ab T392 bei
  (-3018,-11651) (= die alten Dossier-Zahlen Z. 858-859); (-5376,-10728) -> (-5433,-10693), keine Ruhe. 2 bzw. 4
  Einheiten Startunterschied = 873 Einheiten bei der Freigabe. Damit ist "chaotisch" gemessen, nicht behauptet.
- **Weg 2 (Herkunft des Startversatzes):** Lauf mit Leon/e1/e2 am Ende von T253 auf Original-Lage/-Yaw (g_griff F254):
  Anker beim Pin **(-7507,-10327) = Original** (Lauf 0: (-7539,-10349), 39 daneben), e1 T254 (-5882,-14389) r2823 und
  T255 (-5953,-14268) **bitgleich** (Yaw-Fang + Ritt-Platzierung), Kette T268-T290 **d0 in jedem Bild**, T290
  (-5381,-10551) = Original, **Freigabe T378 (-4759,-10633) = Original**, Ruhe T410 (-3009,-11643) = Original.
  Kette der Abweichung im Lauf 0 also: Anlauf T196-T253 (e1 30 Einheiten / Yaw 2815 statt 2825 beim Pin, Restpunkte
  aus OFFEN A1) -> Anker 39 daneben -> Platzierung 2-28 daneben -> Wandklemme in T289/T290 verstaerkt auf 64/177 ->
  Klemmen-Iteration (empfindlich) -> 677 bei der Freigabe.
- **Neuer Befund nach der Ruhe (Weg 2):** Original bis T518 ohne Biss (Leon HP 76, e1 ~3925 / e2 ~4990 entfernt,
  beide +0x1d6 = 1), Port Weg 2 Bisse T420 (76 -> 70) und T480 (70 -> 64). Ursache im Lauf sichtbar: e1 springt am
  Ende des Brustschlags (Sub 2, Clip 3 Bild 69 -> 0, T370) um ~1300 ((-5674,-14523) -> (-4776,-15477)); Original
  F370 -> F371 (c3/69 -> c3/0) nur (4,-4). affen_fuss.log: Port-Fusssperre im Wrap-Bild kf 74/143 d=(-890,1689).

### Vierte Ursache — Fusssperre posierte das Bild NACH dem Vorschub (gefunden ueber den Befund "Bisse nach der Ruhe")
- RE (selbst disassembliert): anim_set FUN_8001f314 holt das Bildwort zu +0x95 (`lbu v0,149(t0)` @0x8001f344 /
  @0x8001f35c, Zeiger `sw a2,360(t0)` (+0x168) @0x8001f36c, 0x8000-Test @0x8001f378, `jal 0x8001f3bc` @0x8001f38c).
  FUN_8001f3bc schreibt DESSEN Pose in den Pool +0x188 (`lw s1,392(v1)` @0x8001f40c; Decompilat RE_15_Quellcode_V2/
  FUN_8001f3bc.c: Wurzel aus dem Keyframe, RotMatrix je Record, Stride 0x2b Worte = 172 B) und zaehlt +0x95 erst danach
  hoch (`lbu v0,149(v1)` / `sb v0,149(v1)` @0x8001f610-1c, `sltu` @0x8001f624, Wrap `sb zero,149(v1)` @0x8001f63c).
  FUN_8011bf50 (STAGE1): `lw s0,392(v0)` @0x8011bf78, CompMatrix(+0x20, Pool-Record 0) @0x8011bf80, Records
  12/13/14 (+3*a1) @0x8011bfa4/b4/c4, `+0x34 -= (m.tx - rec[84])` @0x8011bfd4-e8, `+0x3c -= (m.tz - rec[92])`
  @0x8011bfec-c008 -> die Fusssperre bewegt mit der Pose des Bildes VOR dem Vorschub.
- Port re15_maggot_footlock nahm s_now = +0x95 NACH re15_maggot_anim (ein Bild voraus). Am Ende des Brustschlags
  (Sub 2, Clip 3 Bild 69 -> Wrap 0, T370) rechnete er kf 74 gegen kf 143: Locator-Sprung 1118 roh (Diagnose-Teil
  `fuss`, `jnb3/fuss/fuss_diag.txt`: Wurzel-Versatz der Clip-3-Keyframes (0,0), Locator Bild 69 (-348,-281) -> Bild 0
  (770,-164)), x 1.7 = ~1900 -> e1 sprang ~1300. Im Original rechnet bf50 im Wrap-Bild Bild 69 gegen 68 (F370 -> F371:
  (4,-4)); das Folgebild verlaesst Sub 2 ohne bf50 (B[2] Phase 2 0x801179a8).
- Haken (je 1 Zeile, enemy_ai_common.c): re15_maggot_anim merkt das Bild vor dem Vorschub (s_maggot_pose_bild),
  re15_maggot_footlock posiert es. Alle sieben bf50/c024-Aufrufe stehen direkt hinter re15_maggot_anim (gezaehlt).

### Umsetzung (Dateien, Konstanten mit Beleg)
- enemy_ai_common.c (Haken, Kommentar "Runde 35 Spur J (8)/(9)"):
  - Gorilla case 15 Phase 2: `re15_affen_ritt_platz(e, pl, 1)` (Yaw-Fang a8f8 0x800 @0x8011acac, ad68 @0x8011accc);
    Phase 3: `re15_affen_ritt_platz(e, pl, 0)` (ad68 @0x8011accc) — je 1 Zeile.
  - Gorilla-Wurzelschwanz aec4(Spieler, Gorilla): Paar-Ausnahme `and`/`andi 0x1000` @0x8002af14-1c (1 Zeile).
  - re15_body_push_player: Zombie-Paar-Naeherung (Sub 3..6) nicht fuer Typ 0x27 (Bedingung um `e->type != 0x27u`
    erweitert, 1 Zeile geaendert).
  - re15_maggot_anim / re15_maggot_footlock: Pool-Pose = Bild vor dem Vorschub (@0x8001f40c/@0x8001f610-1c,
    @0x8011bf80-c008) — 1 Zeile + 1 Zeile + 1 Deklaration.
- affen_11c0.c: re15_affen_ritt_platz (re15_enemy_steer_point mit 0x800 = FUN_8001a8f8-Kern `slt` @0x8001a974,
  re15_clip_root_motion_abs_pub = FUN_8001ad68 @0x8001adf4-ae18). re15_affen.h: Abschnitte (8) und (9).
- Keine neue Datei, keine Assets, keine Bank-9-Bits/Nachrichten/AOT/Ereignisse.

### Messung nachher (Riegel, Endstand Code; `jnb3/griff6.txt`, `takt5.txt`, `szene5.txt`, `schrot5.txt`)
- `griff` Lauf 0: e1 T254-T288 hoechstens 28 neben dem Original (vorher bis 844), Abstand e1-e2 T256-T264 3171..3211
  (Original 3168..3209), e2-Weg T254->T262 570 (Original 542, vorher 4); Kette T265-T267 Eingang 22/31/19, Ausgang
  64/104/4 (vorher 213/276/496 bzw. 1241/1573/207); Bahn T265-T290 im Mittel 31, hoechstens 166 (T290); Freigabe T378
  (-6153,-9995) = Klemmen-Iteration ab der eigenen T290-Lage (-5348,-10714) in 88/88 Bildern; Anker (-7539,-10347).
- `griff` Weg 2: Anker (-7507,-10327), e1 T254/T255 bitgleich, Kette T268-T290 23/23 bitgleich, Freigabe T378
  (-4759,-10633), Leon T291-T414 124/124 = Original, Ruhe (-3009,-11643) ab T410, **bis T495 kein Biss (HP 76)** wie das
  Original (g_griff F496: (-3009,-11643) HP 76; vor dem Fusssperren-Fix: Bisse T420/T480). e1/e2 nach der Ruhe 35-200
  neben dem Original (Rest: OFFEN 3/4), e1 in Sub 2 hoechstens 64 je Bild (Wrap-Sprung ~1300 weg).
- `takt`: Gleichtakt 8 Bisse, groesste Abweichung 3 Bilder (vorher 3), Mittel 52,0 (Original 51,6); Wechseltakt
  nach Desync groesste Abweichung **2** (vorher 4), Mittel 35,6 (Original 35,9).
- `szene`: Gang 207/207, Heavy Freigabe + 365 (Original +364), Takt Mittel 52,1 / kuerzester 51, Tod + 1198
  (Original +1193) — unveraendert gegenueber Nachbesserung 2.
- `schrot`: Exit F275 -> Sub 7 (Original F276), Landung F311 (Original F312), danach SELECTOR, kein Zonen-Sprung.

### Tests (Stand Nachbesserung 3) — probes/r35_affen.cmake unveraendert (17 Eintraege), test_r35_affen.c
- `griff` erweitert (M1/M2), neue Pruefungen:
  - M2: e1 T254-T288 <= 40 neben dem Original (Schranke = Anker-Versatz 39); Abstand e1-e2 T256-T264 in 3150..3230
    und e2-Weg T254->T262 >= 400 (Original 542; vorher 4); Wurf-Bahn jetzt T265-T290 (26 Bilder, die Ausnahme
    T265-T267 entfaellt), T265-T267 <= 200.
  - M1: Klemmen-Iteration ab dem Original-T290 = Original-Bahn 124/124 (T291-T414); Riegel-Freigabe = Iteration ab der
    eigenen T290-Lage (88/88); 24/24 Starts 1-2 Einheiten neben dem Original-T290 enden > 100 neben der Original-
    Freigabe; die alten Dossier-Zahlen (-4580,-10518)/(-3018,-11651) = Iteration ab (-5374,-10724), dieselbe ab
    (-5376,-10728) = (-5433,-10693).
  - Weg 2 (Original-Startzustand T253): e1 T254/T255 bitgleich; Anker (-7507,-10327), Kette T268-T290 23/23, Freigabe
    T378 (-4759,-10633); Leon T291-T414 124/124 = Original und am Laufende T495 (-3009,-11643) = Original; e1 in Sub 2
    <= 100 je Bild (vorher ~1300 im Clip-3-Wrap).
- Die zwei in 1ac1ea84 entfernten Pruefungen (Freigabe <= 400, Ruhelage <= 60) kommen NICHT in Lauf 0 zurueck: dort
  entscheidet der Anker-Versatz 39 aus dem Anlauf ueber die chaotische Klemmen-Iteration (gemessen, s. M1 (c)); im
  Lauf Weg 2 gelten sie verschaerft (bitgleich).
- Diagnose-Teil `fuss` (nur von Hand: `test_r35_affen.exe fuss`, nicht in ctest): Locator je Bild fuer Clip 3/5.

### OFFEN (Stand Nachbesserung 3; ersetzt die Liste "Stand Nachbesserung 2")
1. **Startversatz im Riegel-Lauf 0 / im Spiel:** e1 steht beim Pin 30 Einheiten / Yaw 2815 statt 2825 neben dem Original
   (Anlauf T196-T253) -> Anker 39 daneben -> Freigabe chaotisch verschieden (Lauf 0 T378 (-6153,-9995)). Herkunft =
   die A1-Reste unten (Spieler-Schub-Reihenfolge @0x80031cbc, A/B im selben Bild @0x80117358/78, RNG-a0-Reste).
   Messweg: Riegel griff Lauf 0 gegen Weg 2 (gleiches Programm, nur der Zustand T253 verschieden).
2. **e2-Eigenbewegung ab T271** (Kriechen B[3]) bis ~200 neben dem Original, ab T289 Kontakt e1-e2 -> e1 77-116.
   Gleiche Restursachen wie 1. Messweg: Riegel griff, e2-Tabelle s_e12_orig (bis T302).
3. **Sub-2-Eintritt (Brustschlag-Rest, Phase 6 -> Sub 2 Bild 0x1d, +0x8f = 7):** Port e1 bis 64 je Bild (T331-T335),
   Original bis 34. Im Original mischt FUN_8001f3bc die Pool-Pose mit +0x8f gegen die letzte (GPF12/GPL12-Wurzel,
   FUN_80020510-Winkel, Decompilat Z. 37-57/76-85), die Port-Fusssperre posiert ungemischt (re15_affen_pose_abfrage).
   Messweg: Fusssperre mit gemischter Pose gegen g_griff F331-F337.
4. **Clip-Wechsel-Sprung der Fusssperre:** der Port ueberspringt bf50 im Bild eines Clip-Wechsels (s_prev_clip), das
   Original rechnet die neue Pool-Pose (gemischt) gegen die letzte gerenderte (g_griff F372 -> F373: (46,-43); F373-F377
   schwingt e1 bis z -15022 aus, Port nicht). Gleicher Messweg wie 3.
5. Typ-unabhaengige Zombie-Paar-Naeherung in re15_body_push_player (Sub 3..6 waehrend Leon gehalten wird): fuer den
   Gorilla jetzt ausgenommen; ob sie andere Greifer-Typen (0x20 Hund, 0x21 Kraehe, ...) falsch ausnimmt, ist nicht
   geprueft (andere Spuren). Beleg fuer das Original: FUN_8002aec4 `and`/`andi 0x1000` @0x8002af14-1c.
6. Weiter offen aus Nachbesserung 2 (unveraendert): Spieler-Schub-Reihenfolge (@0x80031cbc); A/B im selben Bild
   (@0x80117358/@0x80117378); RNG-a0 an Path-B-Muenze @0x801180a8, B[7]-Anlauf @0x8011898c, B[1], B[4] im
   A[3]->4-Wechselbild, Spieler-Ziehungen; dz-Rundung in climb_common.c:360 / enemy_ai_re2_spider.c:257
   (@0x800246ac); NPC-Klemme +0x82 in Szenen hinter Flags; Knockdown-Sonde spielweit nur in 11C0; Gorilla-Schatten
   (Box[6]+100/+200 @0x801171d8-ec); Heckklappe dunkler (@0x8002c254); Member_set 0x13 in anderen Raeumen.

### Fuer den Nutzer (Stand Nachbesserung 3)
- Sprachdateien: keine neuen Zeilen. Neue Assets fuer das Paket-/Android-Gate: keine. Bedienhinweise: keine.
- Spuerbar neu: (1) Packt ein Gorilla Leon, springt er jetzt wie im Original mit dem ganzen Koerper auf ihn zu (vorher
  blieb er beim Zupacken an Ort und Stelle stehen) und schiebt dabei den zweiten Gorilla zur Seite. (2) Der zweite
  Gorilla schiebt Leon beim Wurf wie im Original an. (3) Am Ende des Brustschlags springt der Gorilla nicht mehr ~1300 Einheiten
  ueber den Platz; nach einem Wurf bleiben beide Gorillas wie im Original zwischen den Wagen haengen, statt Leon
  gleich wieder zu beissen.

### Messung nachher, exe ueber den ECHTEN Tuerweg (exe-Kopie re15_pc_jnb3.exe, md5 62b8fb5a... = re15_pc.exe; `jnb3/run.sh`)
- Env wie Abnahme 2: RE15_SET_FLAG=4:243,3:130, RE15_DEBUG_JUMP=11B0@240, RE15_PLAYER_POS an der 11B0-Tuer,
  Quadrat-Tasten; debug.log jeweils `DOOR FIRE` -> `PC loaded room11c0.rdt` -> F6 sub02 -> F1088 sub07.
- `t3` (keine Eingabe): Freigabe F1088 bei (-7148,-12363); Treffer +365, 470, 524, 574, 628, ... 1148; **Tod +1198 =
  39,9 s** (Original +1194 = 39,8 s; Nachbesserung 2 ebenfalls +1198) — unveraendert.
- `o7` (RE15_INPUT_SCRIPT=W34.5,U2.5,W1, Leon im Freien): Rear-up S3 F1199, **Pin F1204**; S3 reitet die Clip-0x1c-Bahn
  (F1204 (-5609,-16230) -> F1208 (-6362,-15619) -> F1211 (-6428,-15546) -> zurueck F1214 (-6175,-15843); vorher stand der
  Greifer); Clip 0x10 **F1287 = Pin + 83**, Clip 0xb **F1303 = +16**, frei **F1329 = +26** (Original +83/+16/+26);
  Brustschlag Clip 3 nach jedem Griff (F1252 15/5, F1279 Sub 2), Clip-3-Wrap F1319 -> F1320 (-6922,-16439) ->
  (-6934,-16436) ohne Sprung. Weitere Griffe F1576 und F1914. Bild `jnb3/o7_griff.png` (RE15_FRAMEDUMP, F1200-F1320):
  der Gorilla richtet sich auf und rueckt vor, Leon fliegt F1232 ueber ihm; kein Koerper im Wagen.

### Suite (Nachbesserung 3)
- `bash re15_port/tools/local_build.sh all` (Endstand Code, 1123,7 s) -> `test OK — 495/495 bestanden` /
  **`=== LOCAL-BUILD-OK (all) — Tests 495/495`** (Schranke 478; kein Fenster-Haken rot).

### Nachbesserung 3 — Ergebnis je Mangel
| Mangel | Ursache (gemessen) | Beleg | Aenderung | Messung nachher |
|---|---|---|---|---|
| M1 (a)/(b) Dossier-Zahl Z. 858-859, zwei Pruefungen still entfernt | Zahlen stammten aus griff_nach3/4 (T290 (-5374,-10724)); nach A1 T290 (-5376,-10728) -> frei (-5433,-10693), keine Ruhe; 1ac1ea84 entfernte die rot gewordenen PRUEFs | jnb2/griff_nach4 vs nach5; Riegel: Iteration ab beiden Staenden gibt genau diese Zahlen | Z. 858-863 und Testliste N2 korrigiert (KORREKTUR-Vermerke), Entfernung offengelegt | — |
| M1 (c) "chaotisch" unbelegt | Bahn ab T291 = reine Iteration von FUN_8003b0a4 (124/124 Original-Bilder Bezug = Eingang = Vorbild); Iteration hochempfindlich | @0x80031d70; s_wand_orig vs10223-10469 | Riegel griff: Iteration 124/124, Zerlegung 88/88, 24/24 Starts > 100 daneben, Weg 2 | Weg 2 (Original-Zustand T253): Anker, Kette T268-T290, Freigabe T378, Bahn T291-T414 und Ruhe bitgleich |
| M2 e2 steht T250-T264 still | (1) Greifer ohne Ritt-Platzierung/Yaw-Fang; (2) aec4(Spieler,Gorilla) ohne Paar-Ausnahme; (3) e2 (Sub 3) vom Spielerschub als Greifer ausgenommen | ad68 @0x8011accc, a8f8 0x800 @0x8011acac; `and`/`andi 0x1000` @0x8002af14-1c | re15_affen_ritt_platz (Phase 2/3), 2 Haken im Schub | e1 T254-T288 <= 28, e1-e2 3171..3211 (Original 3168..3209), e2-Weg 570 (Original 542); Kette T265-T267 <= 104 (vorher 1573) |
| (neu) Bisse nach der Ruhe / Wrap-Sprung e1 | Fusssperre posierte das Bild nach dem Vorschub | FUN_8001f3bc @0x8001f40c / @0x8001f610-1c, bf50 @0x8011bf80-c008 | Pool-Pose = Bild vor dem Vorschub (2 Zeilen) | Weg 2 bis T495 kein Biss wie das Original; Wechseltakt 2 statt 4 Bilder; Tod +1198 unveraendert |
- Abschluss-Commit: fix(r35-affen): Nachbesserung 3 (Haken enemy_ai_common.c: 7 Hunks je 1-2 Zeilen; affen_11c0.c +11;
  re15_affen.h (8)/(9); test_r35_affen.c Riegel griff erweitert + Diagnose-Teil fuss).
