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

- [ ] Punkt 1 Container-Schieben — Messung + RE fertig (§1), Umsetzung offen
- [ ] Punkt 2 Erweiterung Werkstatt-Tuer
- [ ] Punkt 3 10F0 Kamera/Abgang
- [ ] Punkt 4 Irons-Kopf nach Tod — RE fertig (§4)

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
