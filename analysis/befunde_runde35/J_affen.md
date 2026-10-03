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

