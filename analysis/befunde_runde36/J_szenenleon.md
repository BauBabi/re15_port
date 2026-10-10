# Runde 36 — Spur J "szenenleon" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_szenenleon` (Zweig `r36/szenenleon`, Basis master 78a4a748).
Auftrag (AUFTRAG.md, woertlich, inkl. Nachtrag 14:00) — Spur J:

1. ROOM11B0-Cutscene Leon/Ada/Marvin: Animation + gemeinsames Wegschieben des Containers mit Marvin klappt nicht.
2. ROOM11B0-Cutscene am Ende erweitern (geaendert 14:00): Marvin "Hold on" -> rennt zur Innenseite der
   Werkstatt-Tuer in 11B0 -> Zuschliess-Animation + Zuschliess-Ton (RE2 Robert Kendo Spielanfang) -> Tuer
   bleibt zu (Elzas Meldung ROOM11B1 msg 12 "The door is locked.") -> zurueck zu Leon/Ada -> Marvin: "I just
   want to make sure we don't have any unwanted followers." Parkplatz-Tuer 11B0->11C0 NICHT abschliessen.
3. ROOM10F0-Szene: Kamera bleibt auf Leon, wenn Marvin/Ada den Raum verlassen (kein "Ploppen"), nur
   Tuer-schliessen-Ton zum Schluss.
4. Irons: sobald tot (gegen Ende Stage 1), kein Kopf-Tracking hinter Leon mehr.

Ressourcen (VERTRAG.md): Bank-9-Bits 84 (Werkstatt-Tuer verschlossen), 85 Reserve; Nachrichten-IDs ROOM11B0
20..27; Ereignisse 22, 23 in ROOM11B0; Eigentuemer cut_10f0.c (Kamera/Abgang) + 11B0-Szene.

Werkzeuge dieser Spur: `re15_port/tools/r36_j/messlauf.sh` (exe-Kopie `re15_pc_r36j.exe`, RE15_WINDOW_SCALE=1,
Bilder per RE15_FRAMEDUMP). ⛔ Messfalle: mit dem Standard-Renderer (D3D9) brachen die Framedumps nach ~140 Bildern mit
`SDL_RenderReadPixels failed: GetRenderTargetData(): DEVICELOST` ab (parallele Agenten auf demselben Desktop) —
alle Messlaeufe dieser Spur laufen mit `SDL_RENDER_DRIVER=direct3d11` (beschleunigt, KEIN Software-Renderer).
⛔ Zweite Falle: RE15_FRAMEDUMP-Bildnummern zaehlen je Raum neu — verlaesst die Szene den Raum (11B0 sub06 endet
mit `Aot_on 05` = Gully), ueberschreibt der Folgeraum die Bilder; deshalb immer RE15_EXIT_AT=<bild>#<raum>.

## Stand / Fortschritt

- [x] Punkt 1 Container-Schieben — vorher §1.1, RE §1.2 (Sub @0x80052864, Member 6 @0x800411c8, Bit 0x200
      @0x8002bec0), Umsetzung §1.3, nachher §1.4 (exe + Riegel schub)
- [x] Punkt 2 Erweiterung Werkstatt-Tuer — RE §2.1 (Zelle (3,0x0C) @0x00EC8, Elzas Platz 8 @0x0112A, Kendo RE2
      ROOM1010 sub09 @0x01C18 Clip 0 == 11B0 RBJ Clip 15, Ton Satz 0x0A), Umsetzung §2.2, nachher §2.3 (exe + Riegel
      erweiterung/tuer + integration_r36_szenenleon)
- [x] Punkt 3 10F0 Kamera/Abgang — vorher §3.1, Beleg §3.2, Umsetzung §3.3, nachher §3.4 (+ Riegel abgang10f0)
- [x] Punkt 4 Irons-Kopf nach Tod — RE §4.1, Umsetzung §4.2 (+ ROOM1151 fuer Spur L), vorher/nachher §4.3/§4.4
      (exe 78a4a748 gegen diesen Zweig, Riegel irons)
- [x] Nachbesserung 1 (Abnahme 0 M1-M4) — Abschnitt "Nachbesserung 1" am Ende: Tuer sichtbar zu (Cut_replace-
      Rueck-Tausch, Ereignis 23 im Init-Lauf), Schliessen vor dem Abschliessen (Clip 20 + Tuerbild 8->2 + RE2-Tuer-zu-
      Ton DOOR1A), Falschaussage berichtigt, Riegel auf Tuerbild/Ereignisfolge

FORTSETZUNG 2026-10-10 ~20:45 (Nachfolger nach dem Sitzungslimit): 3d394d71 BAUT (ninja: no work to do nach
configure, Objekt szenenleon_r36.c.obj vorhanden, exe 18:10 nach den Quellen 18:07). Der Vorgaenger hatte noch
einen Messlauf m8_11b0 (Bilder 1600..1880/4) mit dem neuen Code gefahren — Auswertung §1.4.

## §1 Punkt 1 — Container-Schieben ROOM11B0 sub06

### 1.1 Messung vorher (gebauter Stand 78a4a748, exe-Kopie re15_pc_r36j.exe)
Lauf `RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3` (0x43 = 11C0-Bosse tot = Zuender von sub06,
0x82 = sub04 "Sherry"-Szene gesehen, 0xF3 = Strom an), Bilder 1560..1900/6, RE15_EXIT_AT=1900#11b0:
- debug.log: `[scd] Plc_dest(slot=0 mode=0x13 dest=(0,0)) -> state4/sub19 (POSE/EVENT, kein Walk)` und dasselbe fuer
  slot=1 (Marvin) — der Port kennt Modus 0x13 nicht: der Spieler bleibt in State 4 ohne Handler, Marvin faellt in
  `re15_npc_sub_dispatch` auf `default: re15_npc_sub_idle`.
- Bilder f_001644..f_001776 (Cut 6): Leon und Marvin STEHEN neben dem Container (Leon Gesicht zur Kamera, Marvin
  rechts), keine Schiebe-Animation, der Container bleibt bei z -20918 stehen; danach kommt Ada ins Bild.
- `Member_set 06 = 0x0201` auf Work_set(3,0) (= Objekt 0, der Container) ist im Port ein No-Op
  (scd_vm.c op_member_set: Prop-Zweig kennt nur die Ids 0..5/0x0B..0x10, `default: break`).

### 1.2 RE-Belege (selbst disassembliert, info/Re1.5/PSX.EXE bzw. ROOM11B0.RDT)
SCD (ROOM11B0.RDT, `tools/scd_dump_room.py`):
- `0x016C4 40 00 05 00 f3 f9 88 ac` Leon rennt nach (-1549,-21368); `0x016DE 40 00 05 02 e8 03 88 ac` Marvin nach
  (1000,-21368); `0x016E8`/`0x016F6` Modus 9 drehen beide nach +z.
- `0x01702 2e 03 00` Work_set(3,0) = Objekt 0 (Container, Obj_model_set `0x010C4` pos (-2168,0,-20918), Box
  c(1800,-1350,1080) h(1800,1350,1080)); `0x01706 34 06 01 02` Member_set(6) = 0x0201.
- `0x0170E 40 00 13 14 00 00 00 00` Leon Plc_dest Modus 0x13, Bit 0x14; `0x0171A` dasselbe fuer Marvin.
- `0x01722 09 0a 55 00` Sleep 85; `0x01726 22 05 14 01` Set(5,20)=1 (= das Bit +0x1c3 = 0x14 der beiden Schieber).
- `0x017D2 2e 03 00` / `0x017D6 34 06 81 00` am Szenenende Member_set(6) = 0x0081; `0x017DA..0x017E6` Sca_id_set /
  Sca_floor_set 2/4 und 3/9 (Kollision der neuen Lage).
- Erneuter Eintritt: main00 `0x010BC` Ck(4,205)==0 -> `0x010C4` z -20918, sonst `0x010EA` z **-19368** — die
  Szene setzt (4,205)=1 als 2. Opcode (`0x0147C 22 04 cd 01`). Der Schub ist also **+1550 in z**.
Member_set (Handler @0x800410b8 -> FUN_8004116c @0x8004116c, Sprungtabelle @0x80010c8c):
- Id 6 -> @0x800411c4 `j 0x80041230` / @0x800411c8 `sw a2,0(a0)` = WORT +0x00 der Work-Entity (Objekt-Flags).
  0x0201 = aktiv (Bit 0) | 0x200; 0x0081 = aktiv | 0x80.
Objekt-Pass FUN_8002bd44:
- @0x8002be24..@0x8002be4c: fuer JEDEN aktiven Aktor (Stride 500) `jal 0x8002cabc` mit a2=0 @0x8002be44 = der
  Aktor (Marvin) wird aus dem Objekt geschoben.
- @0x8002beb8 `lw v0,0(s2)` / @0x8002bec0 `andi v0,v0,0x200` / @0x8002bec4 `beq` -> @0x8002bed0 a0 = Spieler
  (0x800aca54) / @0x8002bed4 `jal 0x8002cabc` / @0x8002bed8 `ori a2,zero,0x1` = **der SPIELER schiebt das Objekt**
  (FUN_8002cabc a2=1 schreibt obj+0x34/+0x3C), danach @0x8002bedc/e0 Lage gesichert, @0x8002bee4 `j 0x8002c0b8`
  (FUN_8003b558 + Spieler-Ausschiebung wie sonst). 0x80 sperrt dagegen den Typ-4-Schub (@0x8002bf78).
Plc_dest Modus 0x13 = Executor-Sub @0x80052864 (Spieler-Tabelle 0x80073e30[19] UND NPC-Tabelle 0x80076ca0[19],
beide -> 0x80052864; Phasen-Sprungtabelle @0x800112fc ueber +0x6):
- [0] @0x800528a4: +0x6=1, +0x94=5 (@0x800528bc/c0), +0x95=0, +0x8f=7, +0x8c=0 (@0x800528f0) -> faellt in [1]
- [1] @0x800528f4: Schritt-SE bei Bild-Wort-Bit 0x4000 (FUN_80045630 @0x80052930), anim_set(+0x170,+0x174,0,0x200)
  @0x8005294c (EIGENE Bank, Clip 5), Yaw-Rastung @0x80052960-88 (t=(yaw>>2)&0xff; yaw&0x200 ? +t : -t),
  (yaw&0x3e0)==0 -> +0x6=2 (@0x800529a0-b0)
- [2] @0x800529b4: +0x6=3, +0x94=0x11 (@0x800529cc/d0), +0x95=0, +0x8f=7 -> faellt in [3]
- [3] @0x800529f4: anim_set(+0x84,+0x16c,0,0x200) @0x80052a08 (LOCO-Paar = dir[2]/dir[1], FUN_80022300
  @0x800224a8/@0x80022498), +0x6 += Rueckgabe (Clip-Ende -> 4), Yaw-Rastung weiter
- [4] @0x80052a74: +0x6=5, +0x94=0x12 (@0x80052a8c/90), +0x95=0, +0x8f=7 -> faellt in [5]
- [5] @0x80052ab4: Bild 1 -> FUN_80045024(0x02070000, pos) @0x80052ad8 (Schiebe-SE Bank 2 Satz 7); anim_set Loop;
  Bild-Wort-Bit 0x2000 -> FUN_800369f8(0,0) @0x80052b24 (Wurzelbewegung); FUN_8004efe4(0x800b1028, +0x1c3)
  @0x80052b44 = Bank-5-Bit gesetzt -> +0x6=6, +0x94=0x11, +0x95=0
- [6] @0x80052b84: anim_set(+0x84,+0x16c,**1**,0x200) @0x80052b98 = Clip 0x11 RUECKWAERTS, +0x6 += Rueckgabe
- [7] @0x80052bc0: FUN_80045024(0x02070000) @0x80052bcc, +0x5=6 (@0x80052be0, Uebergabe an Sub 6 Event-Reach),
  +0x6=0
Fuer den SPIELER sind +0x84/+0x16c = 0x800acad8/0x800acbc0 = die COMMON-Globals PL00.EMR/PL00.EDD (einzige
Schreiber @0x80031578/@0x8003154c, anim_select_common.c) — dieselben Clips 0x11/0x12 wie das Kistenschieben
(LAB_80035810: Clip 0x11 = 10 Bilder, 0x12 = 25 Bilder, Wurzel +523 je Zyklus auf Bit-0x2000-Bildern 0..14).
Fuer MARVIN (0x40) ist es die EM040-Loco-Bank dir[1]/dir[2] (Laengen s_irons_clip_len[17]=10, [18]=25 — dieselben
Laengen). Der RBJ-Binder FUN_8001b3f8 schreibt nur +0x180/+0x184 (Spieler 0x800acbd4/d8, Gegner 0x800acdac+i*0x1f4)
— der Schiebe-Kanal bleibt unberuehrt.
Plausibilitaet: Sleep 85 nach dem Start, 10 Bilder Clip 0x11, danach ~73 Bilder Clip 0x12 = 3 Schub-Phasen (Bilder
0..14 je 25er Zyklus) x 523 = ~1569 gegen die +1550 der Raum-Records.

### 1.3 Umsetzung (Code seit 3d394d71, in der Fortsetzung gebaut, Haken gelesen und gemessen)
- `engine/src/szenenleon_r36.c` Teil 1 + `include/re15_szenenleon_r36.h` (Konstanten mit Adresse): Spieler-Sub 0x13
  (Phasen 0..7 wie §1.2, Kurs-Rastung @0x80052960-88/@0x80052a40-70, Wurzelschritt auf Bild-Wort-Bit 0x2000
  @0x80052b18, Bank-5-Bit @0x80052b44 -> Phase 6, Phase 7 -> Sub 6 @0x80052be0), NPC-Sub 0x13 (Loco-Kanal +0x84/+0x16c
  in Phase 2..7, eigene Bank +0x170/+0x174 in 0/1), Objekt-Pass Bit 0x200 (FUN_8002cabc(Spieler,obj,1) @0x8002bed4).
- Haken (je 1-3 Zeilen): scd_vm.c op_plc_dest (Modus 0x13 -> Spieler-Sub-Start) + op_member_set (Id 6 = Objekt-Wort
  0 @0x800411c8); aot_common.c re15_prop_push_tick (Bit 0x200 vor dem Typ-4-Zweig); enemy_ai_common.c (Dispatch
  `case 0x13`, Kanal-Auskunft own/loco, NPC-Tick laeuft im Executor); game_step_common.c (Takt nach re15_player_tick,
  re15_player_event_reach_end beendet auch Sub 0x13); anim_select_common.c (PL00-Bank fuer Phase 2..7); main.c (W-Bank
  Clip 5 fuer Phase 0/1 auf +0x170, @0x8005294c).

### 1.4 Messung nachher
- exe (Lauf m8/m9, gleiche Schalter wie 1.1): debug.log `[szl-r36] ROOM11B0 Spieler: Plc_dest Modus 0x13 (Schub-Sub
  @0x80052864)` -> `Schub-Sub zu Ende -> Sub 6`; `[walk] F1650 mo=17` (0x11) `F1680/F1700/F1710 mo=18` (0x12, af
  2 -> 22 -> 7 = 25er-Schleife) `F1740 mo=1` (Sub 6); state.log Marvin `ss1=19` (Sub 0x13) `ss2=3 mo=17` F1650 ->
  `ss2=5 mo=18` F1700 -> `ss1=6` F1740. Leon z -21535 -> -19999 (+1536), Marvin -21516 -> -19999 (+1517).
- Bilder `J_belege/p1_nachher_schieben_F1636_F1772.png`: beide in Schiebehaltung am Container, der Container wandert
  nach hinten, der Gully wird frei (vorher: beide standen, `p1_vorher_11b0_schieben_F1560_F1896.png`).
- Riegel unit_r36_szenenleon_schub (echte VM + Spielschritt, PL00 + EM040 geladen): Container z -20918 -> -19549
  (+1369), Objektwort 0x0201 waehrend, 0x0081 am Ende, snd0 Satz 7 achtmal (2 Schieber x Bild 1 von 0x12 x 2 Zyklen
  + Phase 7).
- Die Raum-Records legen den Container beim Wiederbetreten auf -19368 (+1550, main00 @0x010EA). Der Unterschied
  (181) kommt aus dem Startpunkt: Leon haelt den Lauf-Plc_dest (-1549,-21368) schon im Ankunftsradius 300
  (`0x12c` LAB_80030e5c, actor_locomotion.c PLC_ARRIVAL_RUN) bei z -21535 an, 167 vor der Kontaktlinie
  -20918 - 450 = -21368; der Schub selbst (+1536) entspricht den +1550. Beim Wiederbetreten springt der Container
  also wie im Original auf den Record-Wert (die Szene verlaesst den Raum ohnehin durch den Gully).

## §4 Punkt 4 — Irons-Kopf nach dem Tod

### 4.1 RE-Belege
- Irons = Typ 0x45 in ROOM1150 (`0x00E88 44 00 45 00 ...` Slot 0, (-21150,-720,-26131)), Wurzel 0x8011d140
  (eingetragen @0x8011e93c, analysis/befunde_runde34_granaten/re_gegner_sonst_werkzeug/reg_scan.txt:17).
- Wurzel 0x8011d140 (STAGE1.BIN, selbst disassembliert): Pause-Gate @0x8011d158/@0x8011d160 (0x20000000),
  +0x9&0x20-Gate @0x8011d174..@0x8011d180, Zustands-Dispatch @0x8011d228 (`jalr`, Tabelle 0x80121738), danach
  UNBEDINGT @0x8011d278 `jal 0x80037358` = die Kopf-FSM. Standardziel der FSM ist der Spieler (+0x1a8, Flags 0 =
  Entity-Tracking) — das ist die beobachtete Kopfbewegung "hinter Leon her".
- Der Tod selbst ist PORT-Inhalt (Runde 35 Spur L/K, NUTZER-VORGABE); im Original stirbt Irons in Stage 1 nicht.
  Die ORIGINAL-FORM fuer "Kopf folgt niemandem mehr" ist das Loslassen per Plc_neck Modus 0: Handler @0x80041e98,
  caseD Modus 0 -> +0x1b8 |= 0x12 (0x02 Ziel = Keyframe, 0x10 Pitch 0); Satzform ROOM11B0 sub06 @0x01646
  `41 00 00 00 00 00 00 00 64 00` (Marvin) bzw. ROOM1150 sub08 @0x012D2. Danach slewt die Kopf-FSM auf die Pose des
  Clips zurueck und haelt sie (kein Ziel mehr).

### 4.2 Umsetzung
- `engine/src/szenenleon_r36.c` irons_kopf_tick (aus re15_szl_tick, game_step_common.c neben re15_cut10f0_tick):
  ROOM1150, Aktor-Slot 1 Typ 0x45, (9,73)=1 UND Todespose (state 4 / Motion-Sub 0 / Clip 2 = RE15_IT_IRONS_CLIP_TOT,
  irons_tod_1150.c irons_pose) -> `neck_flags` (+0x1b8) = 0x12 (RE15_SZL_HALS_FREI; Form `ori v0,zero,0x12`
  @0x80042614 / `sb v0,440(s0)` @0x80042618 bzw. Plc_neck Modus 0 caseD @0x80041ed8). Vor dem Armfall (Clips 4/3/6/5
  der Sterbeszene) bleibt der Blick auf Leon. Die Kopf-FSM (skeleton_common.c) faehrt dann Yaw auf den Keyframe
  (0x02 Release-Yaw @0x80037528-48) und die Gesamt-Neigung auf 0 (0x10 Pitch-Null @0x80037554-60).

### 4.3 Messung vorher / nachher
- VORHER (Stand 78a4a748): `git show 78a4a748:re15_port/engine/src/irons_tod_1150.c` enthaelt kein `neck` — die Spur
  L/K-Todespose laesst +0x1b8 auf dem Spawn-INIT-Wert 0 (re15_npc_neck_spawn_init, Behavior 0x00 -> kein 0x12
  @0x8004260c) = ENTITY-TRACKING des Spielers (+0x1a8 = &player @0x8011d314). Gemessen im Riegel als Gegenprobe
  (gleicher Code-Pfad, lebend): Kopf-Akku -615 bei Leon auf dem Skript-Platz (0xafc6,0x9dc5), -491 bei Leon
  (+3000,+2500) daneben — der Kopf folgt Leon.
- NACHHER (unit_r36_szenenleon_irons): tot -> Flags 0x12, Kopf-Akku (Yaw 0, Pitch -153) an BEIDEN Standpunkten
  gleich = Kopf auf der Clip-Pose, Neigung neutral, Leon egal. exe-Messung §4.4.

### 4.4 exe-Messung vorher/nachher (gleiche Schalter, Bild 150, Cut 7 = Nahaufnahme der Liege)
Lauf `RE15_DEBUG_JUMP=1150@250 RE15_SET_FLAG=9:0x49,3:0x5e RE15_FORCE_CUT=7`, Leon einmal bei A (-17500,-25500), einmal
bei B (-21000,-22500); debug.log beider Staende `[irons-tod] ROOM1150 Zustand 0: (9,73)=1: Irons tot (Clip 2 Bild 89
gehalten)`, state.log Irons `st=4 ss1=0 ss2=2 mo=2 af=89`.
- VORHER = exe aus 78a4a748 (Quellstand per `git archive` in den Notizblock, gebaut mit RE15_TESTS=OFF, Assets ueber
  RE15_CD_ROOT): Bilddifferenz A gegen B = Rechteck (244,61)-(279,92) = Irons' KOPF — der Tote dreht den Kopf zu Leon.
- NACHHER (dieser Zweig): `[szl-r36] ROOM1150 Irons tot: Kopf-FSM +0x1b8 = 0x12 (kein Blickziel mehr)`; Bilddifferenz
  A gegen B = KEINE (bitgleich). `J_belege/p4_irons_kopf_vorher_nachher_F150.png` (Ausschnitt, je A/B).
- ROOM1151 (Elza): Spur L legt Irons "von Anfang an tot" hin (NUTZER-VORGABE). Dieselbe Regel greift dort, sobald die
  gehaltene Todespose steht (Clip 2, Phase 2) — das Original-1151 nutzt fuer Irons lebend nur Clip 3/6 (@0x00EB8,
  @0x00F02, @0x00F12), (9,73) ist ein Leon-Bit. Riegel irons: Stellvertreter-Todespose in 1151 -> 0x12, Kopf an zwei
  Standpunkten gleich; lebend (Clip 3) bleibt das Tracking. Setzt Spur L eine ANDERE Pose, greift der Zweig nicht
  (dann im Integrationsbaum nachziehen).
## §3 Punkt 3 — ROOM10F0: Kamera bleibt auf Leon, kein "Ploppen", nur Tuer-zu-Ton

### 3.1 Messung vorher (78a4a748, `RE15_DEBUG_JUMP=10f0@250`, Bilder 2560..2960/4, RE15_STATE_LOG)
- debug.log: `[scd F2650] Cut_chg(0)` (Abgang-Kamera auf die Tuer), `[scd F2700] Cut_chg(2)`.
- state.log: Marvin (Aktor 2) bei F2657 noch bei (6213,1837) im Bild von Cut 0, bei F2677 schon (-30000,-30000);
  Ada (Aktor 1) bei F2677 (6814,1613), bei F2697 geparkt — beide verschwinden MITTEN IM BILD von Cut 0.
- Bilder `J_belege/p3_vorher_10f0_abgang_F2636_F2728.png`: f_002672 Marvin verschwindet mitten im Raum, f_002688..2696
  Ada ebenso ("plopp"), f_002704 Schnitt zurueck auf Cut 2.
- Zeitachse: die Szene laeuft heute ~430 Bilder spaeter als in K_cut10f0.md §4 gemessen (Zeile 23 bei F2260+3*110);
  die Bildnummern hier gelten fuer 78a4a748.

### 3.2 Beleg (Form) und Nutzer-Vorgabe
- NUTZER-VORGABE (AUFTRAG.md Z.72): Kamera beim Abgang auf Leon, nur ein Tuer-schliessen-Ton zum Schluss.
- Kamera Cut 2 = ROOM10F0.RDT @0x0A0 pos (5874,-3618,1851), Blick +88 Grad (Nord) — die Tuer (8400,-350)
  (ROOM10D0 main00 @0x01174, Spawn/Ziel) liegt HINTER der Kamera (z -350 < 1851). Ohne den Schnitt laufen beide aus
  dem unteren Bildrand und werden erst an der Tuer geparkt.
- Parken = Pos_set weit weg (Form ROOM10D0 sub21 @0x01D9E `32 00 1c 57 00 00 34 ef`), unveraendert.
- Ton = vorhandenes Se_on Port-Bank 0x0E Satz 1 = RE2 DOOR13 Door_exit (gen/tuer_zuordnung.inc "S047 T027 ROOM10F0 ->
  ROOM10D0 | DOOR13 V0", RE2-Archivtabelle @0x8009A604) — der Schliesston GENAU dieser Tuer; er steht jetzt als
  letzter Ton nach dem Parken beider (der Nutzer nennt den Door_exit-Satz in Runde 36 selbst "Sound von Tuer
  schliessen", AUFTRAG.md Z.67 ueber den DOOR04-Door_exit der Montage).

### 3.3 Umsetzung
- `re15_port/tools/r35_k/szene_bauen.py`: im Abgang `cut_chg(0)` und das spaetere `sleep(10)` + `cut_chg(2)`
  entfernt; Reihenfolge sonst unveraendert (Marvin/Ada Plc_dest Modus 5 ueber die Wegpunkte zur Tuer, Pos_set
  geparkt bei Ankunft, dann Se_on, Sleep 45, Schwanz). Neu erzeugt: `engine/src/gen/cut10f0_szene.inc` 1342 B /
  285 Opcodes (vorher 1350 B / 288), RE15_CUT10F0_OFF_TUER_ZU 0x0512.

### 3.4 Messung nachher (gleicher Lauf, Bilder 2560..2800/2, `J_belege/p3_nachher_10f0_abgang_F2600_F2790.png`)
- debug.log: nach `[scd F914] Cut_chg(2)` KEIN weiterer Cut_chg; state.log cam=2 durchgehend bis zum Szenenende.
- Marvin: (5460,2758) F2650 -> (8046,4) F2670 -> geparkt F2675; Ada: (5503,3359) F2665 -> (7814,373) F2685 ->
  geparkt F2690; Tuerton danach (`[se] Se_on bank=14 id=1 -> RE2-Tuerbank`).
- Pixel-Differenz aufeinanderfolgender Bilder (Schwelle 60, 2er-Schritt): bis F2668 wandert die Aenderung nach unten
  aus dem Bild (bbox y bis 215 = Unterkante des Bildfelds), **ab F2670 0 geaenderte Pixel** — die beiden Park-
  Zeitpunkte F2671..2675 und F2686..2690 aendern im Bild NICHTS: kein Ploppen.
- Riegel unit_r36_szenenleon_abgang10f0 (echte VM, ROOM10F0 ab Raumaufbau): Zeile 23 B2466, danach KEIN
  Kamerawechsel, Cut 2 bis zum Ende B2721; Marvin geparkt B2651 von (8247,-199), Ada B2676 von (8315,-246) — beide
  Punkte projizieren in Cut 2 (RDT @0x0A0, re15_camera_build_view) AUSSERHALB des Bildfelds; genau ein Tuerton
  (Bank 0x0E Satz 1) B2676 = im Bild des letzten Parkens (Programmfolge Pos_set -> Se_on -> Sleep 45).
- unit_r35_cut10f0_* (7/7) bleiben gruen (Kamerafolge dort jetzt "2 0 1 2 0 2", letzter Cut 2).

## §2 Punkt 2 — Erweiterung der 11B0-Szene: Marvin schliesst die Werkstatt-Tuer ab

### 2.1 RE-Belege
**Die "Werkstatt-Tuer" ist die Durchgangszelle der Trennwand** (analysis/story_leon_elza/D_gegenpruefung.md §2.1,
hier nachgeprueft): SCA ROOM11B0 @0xDE4, Region 3 Index 12 @0x00EC8 `x -22150..-21410 z -17230..-15230 u0=ff fl=03`
(solid im Auslieferungszustand); Leon sub00 @0x0118C `37 03 0c 00` + @0x01190 `39 03 0c 40` oeffnet sie bei Strom
((4,6) @0x01170). Elza ROOM11B1 sub00 @0x0112A `2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 0c 00 ff ff 00 00`
= Text-Platz 8, Rechteck (-22400,-17200,1200,1700), Nachricht 12 @0x179F "The door is locked."
(`rdt_msgdump.py`), solange (3,0x83)==0. Bei Leon liegt dort KEIN Platz. Im Bild: Cut 8 (= Cut 2 mit Licht,
sub00 @0x01188 `4b 02 08`, Kamera RDT @0x0160 (-15552,-1476,-26712) -> (-19998,-1764,-17082)) zeigt die Tuer
(gemalt, geschlossen) an der Trennwand, Zellen-Ostkante projiziert auf x 134..151 (`J36/tools/kam.py`).
**Kendo schliesst ab — RE2 Leon ROOM1010 (Gun Shop) sub09 @0x01C00..@0x01C64** (`tools/r34n_c/re2_scd_dump.py`):
`2e 03 00` Work_set Kendo (Sce_em_set sub04 @0x01A94 `44 00 00 48 ...` Typ 0x48) / @0x01C04 `40 00 04 21 f4 c3 01 c9`
gehen zur Tuer (-15372,-14079) / @0x01C0E `40 00 09 21 d3 d5 92 c8` zur Tuer drehen / @0x01C18 `3f 00 00 00`
**Clip 0** / @0x01C1C `09 0a 14 00` Sleep 20 / @0x01C20 `36 02 0a 01 00 00 d8 c3 f8 f8 c6 c8` **Se_on Raumbank 2
Satz 0x0A** / @0x01C2C Sleep 10 / @0x01C30 `3f 00 00 c0` / Sleep 5 / @0x01C38 `3f 00 00 40` / @0x01C3C `5b 0a`
Plc_cnt / Sleep 5 / @0x01C42 `66` Plc_stop / Sleep 25 / @0x01C48 zurueckdrehen. Tuer = Door_aot_set INIT @0x017A2
Rechteck x -14900.. z -14025.. — Kendo steht davor und sieht nach Osten.
**Der Clip ist bytegleich in RE1.5:** RE2 room1010/animation.rbj (= ROOM1010.RDT @0x47DC, 22632 B, 1 Record,
Marker 0x5, 15 Knochen, ksize 80, 14 Clips) Clip 0 == ROOM11B0 RBJ Record 1 (Marker 0x2 = Marvin) **Clip 15**
— Bildwoerter UND Keyframe-Bytes aller 20 Bilder gleich (`J36/tools/rbj_vgl.py`; ebenso RE2 Clip 1/2/3/4 ==
RE1.5 Clip 18/19/20/23: die Gesten-Bibliothek ist dieselbe). Marvin spielt also mit `3f 00 0f 00` in ROOM11B0
GENAU Kendos Abschliess-Bewegung (linke Hand vor + hoch, Hand max (544,-707,120), `gesten_arme.py`).
**Der Ton:** RE2 ROOM1010 snd0 (RDT-Kopfworte +0x08/+0x0C/+0x10 = EDT @0xA044 192 B / VH @0xA104 3616 B / VB
@0xAF24 82736 B = room1010/snd0.edt/.vh/.vb), EDT Satz 0x0A @0x28 `00 00 83 01` = Prog 0 Ton 8 (vab_common.c
re15_edt_decode: tone = e[2]>>4) -> VAG 4 (6800 B). Bank 2 = Raumbank (`srl t1,a0,24` @0x8005ba30, Herleitung
re15_lock_se.h). RE2_SOUND_LISTE.md: "ROOM101.SAP - Gun Shop".

### 2.2 Umsetzung (Ereignis 22/23, Texte 20..22, Bit (9,84), Port-Bank 0x3A)
- `tools/r36_j/szene11b0_bauen.py` -> `engine/src/gen/szl11b0_szene.inc`: Erweiterung 248 B (Ereignis 22) + Tuer-zu-
  Programm 30 B (Ereignis 23) + msg 20/21/22. Jede Zeile mit Vorbild-Offset im Generator-Kopf. Ablauf:
  `22 09 54 01` Set(9,84)=1 ZUERST -> Work_set(2,0) Marvin -> msg 20 "Marvin: Hold on." (NUTZER-VORGABE, Punkt =
  Satzform der Original-Zeilen) + Clip 15/23 im Zeilentakt @0x014F6..@0x0150A -> Plc_dest Modus 5 Bit 2 ueber
  (1250,-21571) (-2600,-21571) (-6103,-25224) (-16603,-25224) (-20860,-16230) je mit `18 0b` (Gosub sub11
  @0x018BE, Form sub08 @0x0185A); Cut_chg 10 vor dem 3., Cut_chg 8 vor dem 5. Wegpunkt -> Modus 9 zur Tuer
  (-22903,-16230) -> **Kendo-Form** `3f 00 0f 00` / Sleep 20 / `36 3a 0a 00 ..` / Sleep 10 / `3f 00 0f 00` +
  `43 00 80 00` / Sleep 35 -> **Tuer zu** `37 03 0c ff` + `39 03 0c 03` + `2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06
  16 00 ff ff 00 00` -> Rueckweg (Cut 10, Cut 6) nach (1100,-19999), Modus 9 zu Leon (sub06 @0x0174C) -> msg 21
  "Marvin: I just want to make sure / we don't have any unwanted followers." (NUTZER-VORGABE; Zeilenbreite 214/262 px
  <= 271 px = 99 %-Grenze der ausgelieferten Zeilen) + Clip 16/23 -> `47 05 01 00` (= sub06 @0x017EA/@0x017EC).
  Wegpunkte PORT-WAHL, frei von allen SCA-Zellen + Radius 450 (Gully (3,9) @0x00EA4, Block (3,11) @0x00EBC,
  (3,10) @0x00EB0, Trennwand (3,7) @0x00E8C) und mit >= 900 Abstand zu Leon (-1639,-19999) / Ada (-1490,-22549).
  PORT-WAHL Abweichung von Kendo: RE1.5 hat kein Plc_cnt (RE2 0x5B @0x80057120 `sb pc[1],333(v1)` = Bild setzen)
  und kein Plc_stop (RE2 0x66 @0x80056ff0, +0x4 <-> 0x1404); Kendos 5+5+25 Bilder nach dem Rueckwaerts sind hier
  ein Sleep 35 mit vollem Rueckwaerts-Clip (RE2 Plc_motion @0x80056b8c: pc[3] -> +0x1CC = Richtungs-/Halte-Flags).
- `engine/src/szenenleon_r36.c` Teil 2: `re15_szl_aot_on_umleitung` (scd_vm.c op_aot_on, 1 Zeile): steht der Faden
  auf `rdt->sub_scd[6] + (0x017EA - 0x01478)` mit `47 05` und ist (9,84)=0 -> pc = Erweiterung; deren Schwanz
  `47 05` liegt ausserhalb von sub06 und laeuft normal. `re15_szl_install` (scd_room_setup.c, letzte Zeile des
  Installer-Blocks "Runde 36 Spur J"): Texte 20..22 + Dauern in ROOM11B0; bei (9,84)=1 `scd_event_fire(23)` ->
  Tuer-zu-Programm im ersten Spielbild (Leon sub00 @0x0118C hat die Zelle im Init-Lauf geoeffnet).
  `re15_szl_ereignis` (scd_event_fire, 1 Zeile). `re15_szl_lock_stelle` (lock_se_common.c, 2 Zeilen):
  (0x11B0, 22) -> (0x11B1, 12) = Elzas Zeile `{ 0x11B1, 12, M, 1 }` in gen/lock_se_sites.inc -> gleicher
  RE2-Tuer-zu-Ton wie bei Elza.
- `platform/pc/src/audio_pc.c`: Schlossbank SCHLOSS1010 (Muster GLAS1090) + 1 Zeile im Se_on-Zweig vor der
  Bank-Weiche (Port-Bank 0x3A, die Original-Weiche FUN_80045024 kennt nur Bank 0..5).
- NEUE ASSETS: `shared_assets/RE2/SCHLOSS1010.EDT` (192 B), `.VH` (3616 B), `.VB` (82736 B) =
  RE2 ROOM1010 snd0, bytegleich gegen die RDT geprueft (`tools/r36_j/schloss1010_extrakt.py --pruefen`).

### 2.3 Messung nachher (exe-Kopie, `RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3`)
- debug.log: `[szl-r36] ROOM11B0 sub06 @0x017EA Aot_on 5 -> erst die Erweiterung` bei F2135; msg 20; Plc_dest
  Marvin zu den 5 Wegpunkten; `Cut_chg(10)` F2238, `Cut_chg(8)` F2321; an der Tuer (-20745,-16468) r2119 (Ankunft
  < 300, Sub 5 @0x80051764); `Se_on bank=0x3A id=10 -> RE2-Schlossbank SCHLOSS1010` + `Stimme: se=10 vag=3 note=66
  center=90 vol=127` (VAG 4 der VH, 0-basiert 3); Rueckweg `Cut_chg(10)` F2507, `Cut_chg(6)` F2592; Marvin daheim
  (1222,-20189) r2121 (Blick nach Westen zu Leon), msg 21 ~F2625; F2748 Raumwechsel ROOM1260 (Gully) wie im Original.
  Dauer der Erweiterung 613 Bilder (~20 s). Kleinster Abstand Marvin-Ada 971 (F2600), Marvin-Leon 1575.
- Bilder `J_belege/p2_nachher_hold_on_lauf_F2140_F2375.png` (Text "Marvin: Hold on.", Lauf durch Cut 6/10/8),
  `p2_nachher_abschliessen_F2380_F2450.png` (Clip 15 vor: linke Hand zur Tuer, F2408 Endbild, rueckwaerts F2414..),
  `p2_nachher_rueckweg_zeile_F2450_F2740.png` (Rueckweg, Zeile 21 zweizeilig).
- Tuer danach (Lauf mit (3,0x83),(4,0xCD),(9,84)=1, Leon vor der Tuer, `U1.5 A0.2`): von OSTEN bleibt Leon bei
  x -20942 stehen, von WESTEN bei x -22618 (Zelle (3,0x0C) solid); Aktionstaste -> "The door is locked."
  (`J_belege/p2_nachher_tuer_text_ost_west.png`) und `tuerse.log`: `raum=11B1 nachricht=12 art=M weg=AOT satz=0(ZU_A)`
  = derselbe RE2-Ton wie bei Elza.
- ~~⛔ BILD: der Hintergrund zeigt die Tuer in Cut 8 OFFEN gemalt — genau so sieht Elza sie ...~~ **FALSCH (Abnahme 0
  M3, berichtigt in Nachbesserung 1 §N1.1):** ROOM11B.BSS hat BEIDE Zustaende — Bild 2/0 geschlossen, Bild 8/7 offen
  (gleiche Kamera). Elza sieht im gesperrten Zustand Bild 0/2 (geschlossen); nur Leons sub00 tauscht bei Strom auf
  8/7. Die Erweiterung tauscht jetzt zurueck (N1.2).

## Tests (gezielt, KEINE volle Suite — VERTRAG/Auftrag)
Neue Riegel (tests/unit/probes/r36_szenenleon.cmake):
| Test | Punkt | misst | Ergebnis |
|---|---|---|---|
| unit_r36_szenenleon_programm | 2 | Erweiterung/Tuerprogramm statisch: Laengen -> Evt_end, Set(9,84) zuerst, Kendo-Form Clip 15 + Sleep 20 + Se_on 0x3A/0x0A, Tuer zu, Schwanz `47 05 01 00`, msg 22 == 11B1 msg 12, Platz 8 == 11B1 @0x0112A bis auf die Nachricht | gruen |
| unit_r36_szenenleon_schub | 1 | Sub 0x13 Leon (PL00 0x11/0x12/0x11-rueck) + Marvin (Loco 0x11/0x12), Container 0x0201 -> +1369 in z -> 0x0081, Leon +1536, snd0 Satz 7 x8 | gruen |
| unit_r36_szenenleon_erweiterung | 2 | Umleitung @0x017EA nach dem Schub, msg 20, Marvin < 300 an der Tuer, Clip 15 + Se_on 0x3A/0x0A, Zelle (3,0x0C)=0xff + Platz 8 = msg 22, zurueck (1152,-20190), msg 21, dann Aot_on 5 -> ROOM1260, Kameras 6-10-8-10-6 | gruen |
| unit_r36_szenenleon_tuer | 2 | (9,84): Init oeffnet (sub00), Ereignis 23 schliesst (u0 0xff fl 0x03), Platz 8 Text "The door is locked.", Ton wie (0x11B1,12), Leon kommt nicht durch; ohne (9,84) Original; ROOM11B1 unberuehrt | gruen |
| unit_r36_szenenleon_abgang10f0 | 3 | nach Zeile 23 kein Cut_chg, Cut 2 bis zum Ende; Marvin/Ada vor dem Parken AUSSERHALB des Bildes von Cut 2; ein Tuerton zum Schluss | gruen |
| unit_r36_szenenleon_irons | 4 | lebend Tracking (A -615 / B -491); tot (9,73): 0x12, Akku (0,-153) an beiden Standpunkten; ROOM1151 tote Pose -> 0x12, lebend Tracking | gruen |
| integration_r36_szenenleon | 1+2 | echte exe: A Sub 0x13 Start/Ende, Umleitung, msg 20, "-> RE2-Schlossbank SCHLOSS1010" (audio_pc.c), msg 21, ROOM1260; B Ereignis 23 + tuerse.log "raum=11B1 nachricht=12 art=M weg=AOT" | gruen |

Gezielter Lauf (Testgruppen der beruehrten Module: scd_vm/aot/enemy_ai/anim_select/game_step/lock_se/audio/cut_10f0/
irons_tod + eigene), woertlich:
```
ctest --test-dir re15_port/build -R "r36_szenenleon|unit_r35_cut10f0|unit_r35_cut1150|integration_r35_cut1150|unit_member|unit_npc|unit_marvin|unit_1090_|unit_irons_neck_1150|unit_neck_headlook|unit_r34n_d_adaruf|unit_r30_tuer|unit_r31_tuer_ton|unit_door_1170_lock|unit_scd_|unit_aot_|prop_scd_pc_advance|prop_sca_collision|unit_r35_affen|unit_enemy_body_push|unit_r35_inventar1050_kiste|unit_r20_birkin_push_pin|unit_ada_eintritt_1050|unit_r27_ada_cutscene_1050|unit_room_transition_cmd|unit_audio_|unit_re2z_pushoff_crawl|integration_door_lock|integration_room_transition" --timeout 900
100% tests passed, 0 tests failed out of 105
```
Davor (Stand 2237ed0a, vor Boot-Haken/1151-Zweig, beide beruehren 10F0 nicht):
```
ctest --test-dir re15_port/build -R "integration_r35_cut10f0|integration_door_lock|integration_r35_cut1150|integration_room_transition" --timeout 900
100% tests passed, 0 tests failed out of 4
```

## OFFEN (mit Adresse + Messweg)
1. ~~**Bild der Werkstatt-Tuer** (offen gemalt, "kein geschlossenes Tuerblatt im Original", Entscheidungsfrage)~~ —
   ERLEDIGT/BERICHTIGT in Nachbesserung 1 (M1/M3): die Original-Daten haben das geschlossene Bild (BSS 2/0), die
   Mechanik ist Cut_replace; keine Entscheidungsfrage mehr.
2. **Kendos Nachwippen**: RE2 ROOM1010 sub09 @0x01C30..@0x01C44 spielt nach dem Klick Clip 0 5 Bilder rueckwaerts
   (`3f 00 00 c0`), setzt per Plc_cnt (0x5B @0x80057120 `sb pc[1],333(v1)`) Bild 10 und friert per Plc_stop (0x66
   @0x80056ff0) 25 Bilder ein. RE1.5 hat beide Opcodes nicht (Tabelle 0x00..0x5E; 0x5B ist ein Feld-Setter
   @0x800414e0) — hier: voller Rueckwaerts-Clip + Sleep 35 (gleiche Gesamtdauer). Messweg: RE2 in DuckStation, GDB auf
   Kendos +0x14D waehrend sub09.
3. **Raumklang des Schloss-Tons**: RE2 spielt Se_on @0x01C20 positional (x,y,z = Kendos Platz); der Port spielt die
   RE2-Raumbaenke (wie GLAS1090) nicht-positional (audio_pc.c se_play_layers, OFFEN-Block dort, FUN_80045a64).
4. **Elza / Zeitleiste**: ROOM11B1 bleibt Original (Text-Platz 8 bis (3,0x83), danach offen). Die Story-Spur P
   (D_gegenpruefung.md §2.1 Punkt 2) muss entscheiden, ob Marvins Schloss bei Elzas Rueckkehr E21 wirken soll —
   Leons (9,84) liegt im Leon-Spielstand und erreicht ROOM11B1 nicht.

## Fuer den Nutzer
- **Neue Assets (Paket-Gate, release/make_package.sh - nur Orchestrator):** `re15_port/shared_assets/RE2/SCHLOSS1010.EDT`
  (192 B), `SCHLOSS1010.VH` (3616 B), `SCHLOSS1010.VB` (82736 B) = RE2 ROOM1010 snd0 bytegleich
  (`tools/r36_j/schloss1010_extrakt.py --pruefen`). Fehlen sie, bleibt nur der Klick stumm ("[schlossse] ... fehlt").
- **Sprachdateien (optional):** Marvins neue Zeilen sind ROOM11B0 msg 20 "Hold on." und msg 21 "I just want to make
  sure / we don't have any unwanted followers." — `synchro/STAGE1/room11b0/main20.wav` / `main21.wav` bindet der Port
  automatisch, wenn sie da sind.
- **Ablauf zum Ansehen (Stand Nachbesserung 1):** Leon-Spielstand vor der Container-Szene (Strom an, 11C0 erledigt)
  -> ROOM11B0; nach "Okay! Be careful down there!" folgen "Hold on." (Kamera bleibt am Container), Marvins Lauf durch
  die Werkstatt (Cut 10), an der OFFENEN Tuer (Cut 8) greift er zur Tuer — die Tuer ist zu (Bild 2, geschlossen gemalt)
  mit dem RE2-Tuer-zu-Ton —, dann Kendos Abschliessen (linke Hand zum Schloss, RE2-Klick), der Rueckweg (Cut 4) und
  "I just want to make sure..."; dann steigt Leon wie im Original in den Gully. Danach ist die Tuer von beiden Seiten
  GESCHLOSSEN gemalt (Bild 2 Ost / Bild 0 West, wie bei Elza) und zeigt "The door is locked." mit Elzas RE2-Ton.
- ~~Entscheidungsfrage~~ entfaellt (Abnahme 0 M3: beruhte auf der falschen Behauptung, das Original habe kein
  geschlossenes Tuerbild).

## Nachbesserung 1 (nach Abnahme 0, analysis/befunde_runde36/J_abnahme_0.md) — STAND: IN ARBEIT

Maengel M1-M4 der Abnahme; Punkte 1/3/4 (erfuellt) bleiben unberuehrt (ihre Riegel gruen halten).

### N1.0 Messung vorher (Abnahme-Laeufe, nicht wiederholt)
- M1: Erweiterung F2446 Cut 8 = Tuer OFFEN gemalt; spaeter Ost Cut 8 / West Cut 7 offen, waehrend "The door is
  locked." erscheint (Abnahme §2.3, Laeufe a2/d1/d2). Elza 11B1 gesperrt: Cut 0 geschlossen (Lauf c2).
- M2: an der Tuer nur Kendos Folge (Programm +0x005C..+0x0080: Clip 15, Sleep 20, Se_on 0x3A/0x0A, Clip 15 rueck,
  Sleep 35), kein Schliessen, kein Tuer-zu-Ton.
- M3: Dossier §2.3 / OFFEN 1 / Entscheidungsfrage behaupten "kein geschlossenes Tuerbild im Original" — falsch.
- M4: kein Riegel auf den gezeigten Cut / die Ereignisfolge.

### N1.1 RE-Belege (selbst gemessen/disassembliert)
**Wie die Werkstatt-Tuer im Raum dargestellt ist** (Nachtrag abends: "belegen ... Hintergrund, Obj_model_set, ESP,
SCA-Zelle"): NUR im Hintergrund.
- ROOM11B.BSS, dekodiert mit `build/tests/unit/probe_bg_dump.exe shared_assets/PSX <aus> 1`, Pixelvergleich (Schwelle
  30): Bild 2 vs 8 Unterschied NUR bbox (127,86)-(170,145), 1075 px = die Tuer (2 geschlossen, 8 Blatt aufgeschwungen);
  Bild 0 vs 7 bbox (206,36)-(231,97), 649 px (0 zu, 7 offen); Bild 3 vs 9 46 px (165..185,79..83), Bild 4 vs 10 36 px
  (173..187,100..106) = Licht durch die offene Tuer unter dem Wagen.
- Kamera-Records (RDT @0x24, 0x20 B je Cut, `tools/r36_j/kam.py`): Cut 2 @0x00A0 == Cut 8 @0x0160, Cut 0 @0x0060 ==
  Cut 7 @0x0140, Cut 3 @0x00C0 == 9 @0x0180, Cut 4 @0x00E0 == 10 @0x01A0 — gleiche Kamera, anderes Bild.
- RVD (RDT @0x258): zwei Netze, geschlossen 0..6 (@0x0258..@0x03D4) und offen 7..10 (@0x03E8..@0x04C4).
- Kein Tuer-Objekt: main00 Obj_model_set nur obj 0 Container (-2168,-20918) @0x010C4, obj 1 (-22838,-20368) @0x0110E,
  obj 2 (-7463,-24868) @0x01130 (Box 0); Door_aot_set nur Slot 0/1/5 (West-Tuer, Parkplatz, Gully); kein ESP
  (RDT @0x4C effectStart = 0, @0x54 espTimStart = 0, nSprite 0, kein Sce_espr in main/sub); SCA (3,0x0C) @0x00EC8
  ist nur Kollision.
**Mechanik des Tuerbildes (Original):**
- Leon sub00 @0x0116C `06 00 36 00` + @0x01170 `21 04 06 01` (Strom): @0x01174 `06 00 0c 00` / @0x01178
  `23 00 0a 00 00 00` / @0x0117E `29 07` / @0x01180 `3c 01` (steht der Cut auf 0 -> 7), @0x01184 `4b 00 07`,
  @0x01188 `4b 02 08`, @0x0118C `37 03 0c 00` (Zelle auf). Elza ROOM11B1 sub00: dieselben Cut_replace @0x0110E/@0x01112
  NUR im Zweig (3,0x83)==1 (@0x010FA); im gesperrten Zweig (@0x0112A Text-Platz) KEIN Cut_replace -> Bild 0/2.
- Cut_replace LAB_80040414 (PSX.EXE, selbst disassembliert): @0x80040438 `lbu a2,1(v1)` (a), @0x80040440 `lbu
  a1,2(v1)` (b); Schleife @0x8004044c..@0x800404a8: cam_from @0x80040458 `bne v0,a2` -> @0x80040464 `sb a1,0(v1)`,
  sonst @0x80040468 `bne v0,a1` -> @0x80040470 `sb a2,0(v1)`; cam_to @0x8004047c..@0x80040494 ebenso; Schritt 20
  @0x80040498, Ende bei 0xff @0x800404a4 = TAUSCH a<->b, also selbstinvers. Die Ansicht wechselt er nicht
  (@0x800404ac..@0x800404c8 nur Anker-Fixup, scd_vm.c op_cut_replace).
- Cut_chg LAB_800402a0: @0x800402d4 `ori v0,v0,0x100` (Auto-Kamera aus), @0x800402fc `sh a0,4068(at)` (work_vars[0x0A]
  = Cut), @0x80040300 `jal 0x800142f4` (Cut zeigen). Cut_auto @0x800403ac: pc[1]==1 -> @0x800403d8 `and v1,v1,a0`
  (a0 = -257, Bit 0x100 weg = Auto an).
- Ohne Strom laeuft die Szene nie: sub01 @0x011F2 (4,0xF3)==0 -> Cut 13/14 + @0x01216 Aot_reset Slot 1 (Parkplatz-Tuer
  -> Text) — (3,0x43) aus 11C0 ist ohne Strom unerreichbar; an der Szene sind die Bilder also immer auf 7/8 getauscht.
**Init-Lauf (fuer M1, Zeitpunkt von Ereignis 23):** FUN_8003ef6c @0x8003efd4 `jal 0x8003ee3c` mit @0x8003efd8
`addu a1,zero,zero` (Slot 1 = sub00), @0x8003f018 `jal 0x8003f0a0` = der Dispatcher-Lauf beim Raumaufbau; dessen
Schleife @0x8003f17c `addiu s2,s2,1` / @0x8003f180 `sltiu v0,s2,0xa` / @0x8003f188 `addiu s1,s1,368` geht die Slots
der Reihe nach durch — ein in diesem Lauf gestarteter Faden in einem hoeheren Slot laeuft im selben Lauf (Evt_exec
aus sub00, Form ROOM1050 sub00 @0x00CA2 `04 ff 18 03`, selbst nachgesehen; Port-Ereignis-Slots 10..23).
**Tuer-zu-Ton (RE2-Tuerbank):** RE2 PSX.EXE @0x800141f0 `jal 0x8005ba28` mit @0x800141f4 `lui a0,0x1` = Satz 1
(Door_exit) nach dem Ausblenden (selbst disassembliert, info/re2leon/PSX.EXE). Archiv: RE2-Tabelle @0x8009A658
`38 53 fc 92 0b 00 00 00 f5 f4 00 00` = DOOR1A, Tonteil 0x5338 B (selbst gelesen), Tonkopf `00 00 14 16 | 00 00 24
17 | ff..` + "pBAV" @0x10 (tuerton_bauen.py prueft), bytegleich DOOR15 (Familie F4). PORT-WAHL DOOR1A: Archiv der
anderen Tuer desselben Raums (gen/tuer_zuordnung.inc S094 11B0->11C0), "Druecker + Kartenleser" (tueren_02_re2.md
Z.256) = die elektronische Tuer des Nutzers (AUFTRAG.md Z.129). Gemessen in der exe: derselbe Ton wie beim Durchgang
11C0->11B0 (`[se] Stimme: se=1 layer=0 vag=2 note=61 ... (13641 Hz)` beim Eintritt UND beim Schliessen).
**Schliess-Animation:** PORT-WAHL (Nutzer: "geeignete Animationen aussuchen") Marvin RBJ rec1 Clip 20 (25 Bilder,
`tools/r34n_d/rbj_zensus.py liste`): `tools/r34n_d/gesten_arme.py ROOM11B0.RDT 1 20` -> linke Hand vor +362 / hoch 515 /
links 219 mit Maximum bei Bild 15 (Clip 15 = Kendo +544/707 bei Bild 19, Clip 16 +333/403 bei 23). FORM: Plc_motion
`3f 00 14 00` wie sub06 @0x014F6, rueckwaerts `43 00 80 00` wie ROOM11C0 sub02 @0x018E4.

### N1.2 Umsetzung (je Mangel)
**M2 — erst schliessen, dann abschliessen** (`tools/r36_j/szene11b0_bauen.py` -> `gen/szl11b0_szene.inc`, 294 B):
an der Tuer (Marvin nach Modus-9-Drehung, Cut 8 = Tuer offen) neu +0x005C..+0x0089:
`3f 00 14 00` Plc_motion(0,20) / `09 0a 0f 00` Sleep 15 (Hand-Maximum) / `4b 00 07 00` + `4b 02 08 00` = sub00
@0x01184..@0x0118B Byte fuer Byte (Rueck-Tausch) / `29 02` Cut_chg 2 (gleiche Kamera, Tuer geschlossen gemalt) /
`36 0e 01 00 ..` Se_on Port-Bank 0x0E Satz 1 (Door_exit) / Sleep 10 / Clip 20 + `43 00 80 00` rueckwaerts / Sleep 25;
DANN unveraendert Kendos Abschliessen (+0x008A) und Zelle/Text (+0x00AE). Rueckweg `29 04` statt `29 0a`.
Tuerbank: `re15_szl_aot_on_umleitung` laedt beim Start der Erweiterung den Tonteil DOOR1A
(`re15_audio_re2_tuer_laden`, `gen/szl11b0_tuerton.inc` aus `tools/r36_j/tuerton_bauen.py`, 21304 B, sha1 b5312b10);
die Port-Bank 0x0E fuehrt audio_pc.c schon heute vor der Bank-Weiche auf `re15_audio_re2_tuer_se` (cut_10f0.c,
Runde 35) — kein neuer Haken in audio_pc.c.
**M1 — Tuer bleibt sichtbar zu:** in der Szene durch denselben Rueck-Tausch (Zonen danach wie ausgeliefert = Netz
0..6). Beim Raumaufbau mit (9,84) Ereignis 23 (80 B): Zelle + Text-Platz wie bisher, dazu `06 00 2e 00 21 04 06 01`
(Strom-Zweig, Kopf sub00 @0x0116C/@0x01170) { Cut_replace 0/7 + 2/8; `06 00 0c 00 23 00 0a 00 07 00 29 00 3c 01 08 00`
(Cut 7 -> 0); dasselbe 8 -> 2 } — Form sub00 @0x01174..@0x01183, nur Wert/Ziel umgekehrt.
Ereignis 23 laeuft jetzt IM Init-Lauf hinter sub00: `re15_szl_init_lauf` (szenenleon_r36.c) wird nach dem Start von
Slot 1 und vor dem Init-Tick gerufen — 1 Zeile in scd_room_setup.c (nach `scd_thread_start(1, rdt->sub_scd[0])`) und
2 Zeilen im Boot-/CONTINUE-Weg von main.c (ebenso nach `scd_thread_start(1, ...)`, NACH der Pre-Intro-Erkennung, die
belegte Ereignis-Slots als Pre-Intro wertet). Ereignis-Slot 10 laeuft in derselben Slot-Schleife nach Slot 1.
Grund (gemessen, Stand 48043096 mit dem Faden-Start nach dem Init-Lauf): der erste Spielbild-Lauf der VM kam erst in
Bild 6 (Einblenden); Bild 1..6 zeigten Cut 8 (`[scd F6] Cut_chg(2)`, state.log F1..F5 cam=8; Bild 6 voll hell mit
|Bild-8| = 0.0 im Tuerbereich = OFFENE Tuer sichtbar).
**M3:** §2.3, OFFEN 1 und "Fuer den Nutzer/Entscheidungsfrage" berichtigt (durchgestrichen + Verweis auf N1.1).
**M4:** Riegel N1.5.
Messhaken (nur Log): `[szl-r36] ROOM11B0 F<bild> +0x<marke> ...` je Marke (Schliessen, Tuer zu, Tuer-zu-Ton,
Abschliessen, Abschliess-Ton), `re15_szl_marke()` / `re15_szl_programm_versatz()` fuer den Riegel.

### N1.3 Messung nachher — Szene (Echtweg Tuer 11C0 -> 11B0, Lauf n2)
`lauf.sh n2_11b0 480 RE15_DEBUG_JUMP=11c0@250 RE15_SET_FLAG=3:0x43,3:0x82,4:0xf3,4:0x40 RE15_FIRE_AOT=0@60#11c0
RE15_MSG_LOG=1 RE15_SE_DEBUG=1 RE15_STATE_LOG=... RE15_FRAMEDUMP=2300-2800/1:... RE15_EXIT_AT=30#1260`
(beschleunigt, SDL_RENDER_DRIVER=direct3d11, RE15_WINDOW_SCALE=1; Notizblock N1/):
```
[scd F2320] Cut_chg(8)
[szl-r36] ROOM11B0 F2381 +0x005C Schliessen: Plc_motion(0,20) Marvin greift zur offenen Tuer
[scd F2396] Cut_chg(2)
[se] Se_on bank=14 id=1 -> RE2-Tuerbank (Runde 35 Spur K)
[se] Stimme: se=1 layer=0 vag=2 note=61 fine=109 center=83 vol=110 pitch=0x4f3 (13641 Hz) -> SE-Stimme 7 (SPU 23)
[szl-r36] ROOM11B0 F2396 +0x0064 Tuer zu: Cut_replace 0<->7 + 2<->8 (sub00 @0x01184/@0x01188 zurueck), Cut_chg 2
[szl-r36] ROOM11B0 F2396 +0x006E Tuer-zu-Ton: Se_on Port-Bank 0x0E Satz 1 (RE2 DOOR1A Door_exit)
[szl-r36] ROOM11B0 F2431 +0x008A Abschliessen: Plc_motion(0,15) (RE2 Kendo Clip 0)
[se] Se_on bank=0x3A id=10 -> RE2-Schlossbank SCHLOSS1010 (Runde 36 Spur J)
[szl-r36] ROOM11B0 F2451 +0x0092 Abschliess-Ton: Se_on Port-Bank 0x3A Satz 0x0A (SCHLOSS1010)
[scd F2556] Cut_chg(4)  ...  [scd F2641] Cut_chg(6)  ...  [msg] room=11b0 id=21  ...  room1260.rdt
```
**Schliessen F2381 < Tuer-zu-Ton F2396 < Abschliessen F2431 < Abschliess-Ton F2451.** Der Tuer-zu-Ton ist derselbe
Ton wie beim Durchgang 11C0 -> 11B0 (gleiche Stimmzeile beim Eintritt, debug.log Z.88).
Tuerbild je Bild (`tuerzustand.py`: Tuerbereich (128..142, 92..112) gegen BSS Bild 2/8): F2348..F2396 OFFEN
(|Bild-8| 1.1..1.4), **ab F2397 ZU** (|Bild-2| 0.1..0.4) bis zum Kamerawechsel F2556 (F2320..F2347 verdeckt Marvin
auf dem Weg zur Tuer den Bereich). Framedump-Serie `J_belege/nb1_m2_schliessen_abschliessen_F2378_F2468.png`:
F2378 Marvin vor der offenen Tuer, F2384..F2396 Arm zur Tuer, F2402 Tuer geschlossen gemalt mit der Hand am Blatt,
F2408..F2426 Arm zurueck, F2432..F2456 Kendos Griff zum Schloss, F2462 zurueck.

### N1.4 Messung nachher — spaetere Besuche (Abnahme-Laeufe d1/d2 mit Framedump JEDES Bildes 0..400)
`lauf.sh e_ost 150 RE15_DEBUG_JUMP=11b0@250 RE15_SET_FLAG=4:0xf3,3:0x43,3:0x82,3:0x83,4:0xcd,9:0x54
RE15_PLAYER_POS=-20500,-16300,2048 RE15_TUERSE_LOG=... RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=300
RE15_INPUT_SCRIPT=U1.5,W0.5,A0.2,W3 RE15_MSG_LOG=1 RE15_STATE_LOG=... RE15_FRAMEDUMP=0-400/1:... RE15_EXIT_AT=420#11b0`
(West: `RE15_PLAYER_POS=-23300,-16300,0`):
- debug.log: `Werkstatt-Tuer verschlossen (9,84): Ereignis 23 im Init-Lauf nach sub00 ...`, `[scd F250] Cut_chg(7)`
  (sub00 @0x0117E) und im SELBEN Init-Lauf `[scd F250] Cut_chg(0)` (Ereignis 23).
- state.log: Ost 420 Bilder cam=2 (nie 8), West 420 Bilder cam=0 (nie 7).
- Pixel (Tuerbereich gegen BSS): Ost Bild 1..400 alle ZU (ab Bild 6 |Bild-2| = 0.0; 1..5 Einblenden, naeher an 2 als
  an 8), West Bild 1..400 alle ZU (ab Bild 6 |Bild-0| = 0.0..0.7).
- F360 "The door is locked." + tuerse.log `F360 raum=11B1 nachricht=12 art=M weg=AOT satz=0(ZU_A) nr=1` (Ost und West).
- Bild `J_belege/nb1_m1_tuer_spaeter_vorher_nachher_F370.png`: Abnahme-Laeufe d1/d2 (vorher: Cut 8/7 offen) gegen
  nachher (Cut 2/0 geschlossen), je mit Ausschnitt.
