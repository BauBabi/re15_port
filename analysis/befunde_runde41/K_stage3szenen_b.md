# Runde 41 — Spur K "stage3szenen_b" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_stage3szenen_b`, Zweig `r41/stage3szenen_b`, Basis 700483de (master 4c667c97 + AUFTRAG/VERTRAG
Stand Nachtrag 11). Wortlaut des Nutzers (AUFTRAG.md, 2026-10-23 16:3x):

> K1: "Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist völlig falsch und kaputt. Außerdem sollte sie keine
>     Waffe haben."
> K2: "Die Cutscene im ROOM 3091 hat STÄNDIG falsche Animationen...."

Orchestrator A-E5: RBJ-Binder (FUN_8001b3f8) gehoert Spur J; K uebernimmt J's Commit `rbj-binder` UNVERAENDERT per
cherry-pick und misst 3091 darauf. Bits: J/K = 121-124 bei Bedarf. Nachrichten: nur vorhandene.

## Plan
1. K1 Messung vorher: welcher Typ/welches Modell/welche Bank fuer Sherry in 30C1..3091 (Port), Framedump.
2. K1 Original: Sce_em_set-Zensus, EMD/CDEMD1-TOC, Textur/CLUT, Waffen-Mesh-Frage; Savestate/Video.
3. K1 Umsetzung + Riegel (Modell-Hash/Mesh-Zahl, kein Waffen-Mesh).
4. K2 Messung vorher (RE15_ANIM_TRACE je Akteur je Zeile), Original-SCD-Subs, Clip-Folge je Akteur.
5. K2 cherry-pick rbj-binder (J), Messung darauf; Ursachen belegen, beheben, Riegel.
6. Gezielte Tests, Abschluss.

## Protokoll (neueste unten)
- 2026-10-24 20:03: Dossier angelegt.
- 2026-10-24: rbj-binder der Spur J (e4a9f39c) UNVERAENDERT per cherry-pick uebernommen (hier 299b6deb, A-E5).
- 2026-10-24: K1 (Spielfigur Sherry) gebaut, gemessen, Riegel; K2 (3091) am Original per GDB gemessen, Port gleich.

## Kurzfassung
* K1 "Ab 30C1 ... Sherrys Charakter Modell ist voellig falsch und kaputt ... keine Waffe": ab der Schacht-Tuer 3061 -> 30C1
  spielt man SHERRY (ROOM3061 @0x011F6 `24 10 0f 00`). Der Port tauschte nur Mesh+Textur auf Elzas Skelett/Animation/
  Waffe. Jetzt wie FUN_800314b0 (@0x8003154c EDD, @0x80031578 EMR) + FUN_80036b68 (@0x800741e8[15] = PL0FW00, Waffe 0
  @0x800316c4, kein Hand-Mesh @0x80036c08), kein Zielen ohne Waffe (@0x80032014 ff.), Tempo Zeile 15 (@0x80073ec2/
  @0x80073f42), Kletter-Ausloesebild 0x63 (@0x8003818c). Original-Bild + Original-Tempo gemessen, Port gleich.
* K2 "3091 STAENDIG falsche Animationen": Spieler spielte den rec0-Platzhalter (2 Bilder), Elza-NPC war ungebunden
  (Bank-0-Clips). Mit J's Binder + K1 sind die Clipfolgen aller drei Figuren = Original (GDB-Mitschrift 2953 Bilder)
  und 7 Original-Szenenbilder = Port-Bilder.


## K1 — Sherry ab 30C1 (Modell falsch/kaputt, Waffe)

### K1.1 Wer ist "Sherry" in Elzas STAGE3 (statischer Zensus, eigene Dumps)
Quelle: `re15_port/tools/scd_dump_room.py` je Raum (Dumps `K_belege/k2_scd3091.txt`), Typ-Zensus Runde 37
(`analysis/befunde_runde37/N_belege/n1_typzensus.txt`), Texte `analysis/story_leon_elza/zensus.md` (rdt_msgdump):
* Sherry ist im RE1.5-Material **Typ 0x4B** ("Girl": ROOM11B1 msg 3 "Girl: ...Sherry.", ROOM2001 msg 8 "Sherry: ...").
  Elzas Weg 3061 -> (Tuer main00 @0x00EF8 Slot 3 -> ROOM30C Cut 10) -> 30C1 -> 3071 -> 3081 -> 3091 -> 30C1 (Marvin).
  Sherry als 0x4B: 3061 @0x00F44, 3071 @0x034C2, 3081 @0x007A0, 30E1 @0x00EC0, 4001, 5011, 5021, 5031, 6021, 6031 ...
* **ROOM3091 (Elza) ist die Ausnahme:** sub00 @0x014F4 `44 01 42 40 00 00 00 ff 35 a0 00 00 cf 0f 00 00 70 f4 00 00`
  = Sce_em_set Gegner 1 **Typ 0x42** (John = Gegner 0 @0x014E0 Typ 0x49). Die Szene sub07 fuehrt Gegner 1 durch alle
  Sherry-Zeilen (msg 15 "Sherry: It's Sherry...", 20, 22, 24, 28, 29; Work_set 2,1 @0x0171A/@0x01752/@0x01808/...).
* In 30C1 selbst steht KEINE Sherry (einziges Sce_em_set sub00 @0x00BEE `44 00 40 40 ..` = Marvin 0x40, nur bei
  Ck(3,57)==1, gesetzt in 3091 sub07 @0x016D4). "Ab 30C1" = der Abschnitt nach 3061: 30C1 -> 3071 -> 3081 -> 3091.
* Klaerung der Einstiegs-Widersprueche: Kommentar enemy_ai_common.c:9640 "0x45 Sherry" ist FALSCH (0x45 = Irons:
  ROOM1150/1151 msg 2 "Irons: ..."; B_figuren.md 1.3); test_r37_elzaems "elza3091 Typ 0x42 CDEMD1 Sektor 1505" ist
  richtig, aber dieser Typ ist im Original NICHT das Sherry-Modell (K1.2).

### K1.2 Was das Original fuer 0x42 im Elza-Archiv laedt (EXE-TOC, eigener Leser)
Lader FUN_80022300 (Runde 37 N §1.1, Elza-Bit bltz @0x80022334 -> Datei 0x27 @0x800223ac, TOC Sektor @0x800223e0,
Laenge @0x800223c4). TOC-Bytes selbst aus info/Re1.5/PSX.EXE gelesen (`K_belege/k1_baenke.py` -> `k1_baenke.txt`):

| Typ / Archiv | TOC-Adresse | Bytes | Sektor / Laenge |
|---|---|---|---|
| 0x42 CDEMD0 (Ada) | 0x800730c8 | `dd 05 00 00 dc 7a 02 00` | 1501 / 162524 |
| **0x42 CDEMD1** | **0x80073308** | **`e1 05 00 00 a0 32 02 00`** | **1505 / 144032** |
| 0x4B CDEMD0 | 0x80073110 | `ee 06 00 00 08 1f 02 00` | 1774 / 139016 |
| 0x4B CDEMD1 | 0x80073350 | `f0 06 00 00 08 1f 02 00` | 1776 / 139016 (Inhalt bytegleich, n1_zensus) |

Ansichtsblaetter (Port-Leser `re15_port/tools/emd_ansichtsblatt.py`, nur EMS_PFAD umgestellt: `K_belege/k1_ansicht.py`):
* `K_belege/k1_cdemd1_typ0x42.png` (Kopfzeile des Werkzeugs sagt faelschlich "CDEMD0", gelesen ist CDEMD1 Sektor 1505,
  Blobgroesse 144032 = TOC): **eine erwachsene Frau im rot-weissen RACCOON-Rennanzug mit blondem Pferdeschwanz = Elzas
  Kleidung — mit einer PISTOLE in der rechten Hand** (VORNE/RECHTS). Textur: Anzug mit "RACCOON"-Schriftzug, Elzas
  Gesicht. Modellhoehe 2782 (Pose Clip 0). Das ist ein **Elza-Doppel (NPC-Format) als Platzhalter** — nicht Sherry.
* `K_belege/k1_cdemd0_typ0x4B.png`: **Sherry** = Maedchen, weisse Matrosenbluse mit blauem Kragen, dunkler Rock,
  Anhaenger/Taschenuhr ("My, that's a nice pocket watch!" ROOM2001), **keine Waffe**; Hoehe 2094.
* Bank-Vergleich (`k1_baenke.txt`): alle drei Skelette 15 Knochen mit DERSELBEN Hierarchie
  `[-1,0,1,2,3,1,5,6,0,0,9,10,0,12,13]`; Bind-Lagen 0x42-CDEMD1 erwachsen (Wurzel y -1709, Oberschenkel 609),
  0x4B Kind (Wurzel y -1268, Oberschenkel 427). Bank 0 (dir1): 0x42 beide Archive 24 Clips {34,32,50,20,...};
  0x4B 24 Clips {28,28,133,30,20,38,...}. Eigene Bank (dir3): 0x42-CDEMD1 14 Clips {22,16,52,1,105,30,10,16,1,17,1,17,1,32}
  (Layout wie John 0x49 {22,16,52,1,50,30,10,16,...}), 0x4B 9 Clips {20,30,60,1,30,28,15,1,8}.
* Runde 37 N §5 hatte genau dieses Modell als "blonde Figur in Rot/Weiss wie das Original" in den Port geholt
  (`N_belege/n5_sherry3091_original_vorher_nachher.png`: auch im ORIGINAL steht in 3091 ein zweites Elza-Bild neben Elza).

=> **UEBERHOLT durch K1.3** (erste Lesart, hier stehen gelassen): das 0x42-Modell ist nicht "Sherry-Platzhalter", sondern ELZA als NPC — in 3091 ist Sherry die SPIELFIGUR (Modell 0x0F). Massgeblich sind K1.3 ff.

### K1.3 KORREKTUR (2026-10-24 ~21:00): in 3091 ist SHERRY DER SPIELER, das 0x42-Modell ist ELZA als NPC
K1.1/K1.2 oben haben die Rollen verwechselt. Belege (eigene Dumps, Datei-Offsets):
* ROOM3061 sub02 (Kriech-Szene) endet mit `2e 01 00` Work_set Spieler, **`24 10 0f 00` @0x011F6 = Save
  work_vars[0x10] = 0x0F**, `47 03` @0x011FA = Aot_on 3 = Door_aot_set Slot 3 @0x00EF8 (Rechteck 0, Ziel Raum 0x0C
  = 30C, Cut 10, Lage (-19100,-,-1350)). Raumlader FUN_800396fc @0x80039760-8c (Kommentar platform/pc/main.c "SPIELER-
  MODELL-WECHSEL", Runde 2026-08-26): Modell-Nibble von DAT_800aca5c != work_vars[0x10] (0x800b0ff0) -> neues Modell
  -> FUN_800314b0 laedt PL0<n>.PLD (Datei-Id-Tabelle @0x80073f70). **Modell 0x0F = PL0F.PLD = Sherry als SPIELFIGUR**
  (PLD/PL0F.PLD 126892 B, Waffen-Datei PL0FW00.PLW 14696 B; Waffen-Tabelle @0x800741e8[15] = 118 = PL0FW00, Spur J
  §1.2). => **"Ab 30C1" = ab der Ankunft in 30C1 Cut 10 spielt man SHERRY.**
* ROOM3091 sub07 endet mit `24 10 04 00` @0x01D9C bzw. `24 10 05 00` @0x01DA8 (je Ck(3,117)) = Spielermodell zurueck
  auf Elza (4/5), dann `47 00` Aot_on 0 -> 30C1 Cut 1 (Marvin-Szene "Elza: Who are you?").
* In der 3091-Szene spricht der SPIELER die Sherry-Zeilen (msg 15 "Sherry: It's Sherry..." nach Work_set 1,0
  @0x018EA, Plc_motion(0,12) @0x018EE) und Gegner 1 (Typ 0x42) die Elza-Zeilen (msg 11 "Elza: Yes, somehow!" nach
  Work_set 2,1 @0x01808, Plc_motion(0,4) @0x0181A). Das CDEMD1-0x42-Modell (Elza-Anzug, Pistole) ist also ELZA als
  NPC — richtig so, Elza traegt ihre Pistole.
* Wurzelhoehen der Raum-Animation 3091 (`K_belege/k1_rbj_clips.py 3 3091 2`): rec2 (Marker 0x3 = Spieler + Gegner 1,
  FUN_8001b3f8) Clips 0..10 Wurzel-y -1678..-1723 (Erwachsene: Elza), **Clips 11..13 Wurzel-y -1257..-1261 (Kind:
  Sherry; 0x4B-Eigenbank Median -1255, Bind -1268)**. Das Skript spielt 11/12/13 auf dem SPIELER (Work_set 1,0:
  @0x0170E (0,13), @0x018EE (0,12), @0x01A1E (0,11) ...) und 0..9 auf Gegner 1 (Elza-NPC). Gegenprobe ROOM4001
  (Elza ist dort Spielerin, Sherry 0x4B Gegner 1): dort spielt Gegner 1 die Kind-Clips 11..13 und der Spieler 0..9.
  => Die Clips sind fuer genau diese Rollen gebaut: Spieler = Sherry-Skelett, Gegner 1 = Elza-Skelett.
* K1 ist damit: **die SPIELFIGUR Sherry (Modell 0x0F) ist im Port falsch/kaputt und traegt eine Waffe.** Port-Stand:
  `pc_sync_player_model` (platform/pc/main.c ~Z.1394) tauscht beim Modellwechsel NUR MD1 + TIM aus PL<n>.PLD und
  behaelt Skelett (EMR) und Animation (EDD) der bisherigen Figur sowie deren Waffe — fuer die Weste (PL00<->PL01: EDD
  und EMR bytegleich) richtig, fuer Sherry (Kind-Skelett, eigene Clips, eigene Waffendatei) falsch. Messung folgt.

### K1.4 Messung vorher (Port, Bau 750bfe36 = Basis + rbj-binder J + Messhaken)
Lauf `re15_port/tools/r41_k/lauf.sh v30c1s` (Elza-Neuspiel RE15_PSELECT_AUTO(_SWITCH), RE15_SET_FLAG=4:244,
RE15_DEBUG_JUMP=3060@250 -> ROOM3061, **RE15_WORKVAR_AT=0x10:15@30#3061** = was ROOM3061 @0x011F6 schreibt,
**RE15_FIRE_AOT=3@60#3061** = die Schacht-Tuer @0x00EF8, also der Tuerweg der Szene -> ROOM30C1 Cut 10,
RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1). debug.log: `[workvar-at] Bild 30 Raum 3061: work_vars[0x10] = 15`,
`[fire-aot] slot=3 at F60`, `[room] PC loaded room30c1.rdt`, **`[pld] Spielermodell -> PLD/PL0F.PLD (Mesh 15 Teile,
Textur 256x256)`** — nur Mesh+Textur getauscht.
Bilder: `K_belege/k1_vorher_30c1_bogen.png`, Zoom `k1_vorher_30c1_zoom.png`, Vergleich
**`K_belege/k1_original_vs_port_vorher_30c1.png`**:
* ORIGINAL (Nutzer-/Spur-J-Stand HASH-957757946319438E_1.sav, sauber: Stub 08 00 e0 03; RAM DAT_800aca5c = **0x0F**,
  DAT_800aca5d = **0x00**, work_vars[0x10] = 0x000F, Lage (-19100,-1350) = Cut 10 der Schacht-Tuer; Framebuffer aus dem
  VRAM, `K_belege/k1_original_30c1_sherry_fb.png`): Sherry steht als Kind mit geschlossenen Gliedern, Schuhe im
  Schatten, KEINE Waffe.
* PORT vorher: Sherrys Teile haengen am ERWACHSENEN Elza-Skelett (PL04-Bindelagen/-Animation): Beine auseinander-
  gerissen, Fuesse schweben, Schuhe liegen getrennt im Schatten; in der rechten Hand ein grauer Stab = Elzas Messer-Hand
  (wpn_md1[0] = PL04W00 dir[2], gezeichnet weil main.c ~Z.9555 `vis = wpn_md1_ok[wi]` auch fuer Waffe 0 zeichnet).
  => Nutzer-Befund "voellig falsch und kaputt" + "Waffe" am Port reproduziert.

### K1.5 RE-Beleg Original: Modellwechsel FUN_800314b0 + Waffenlader FUN_80036b68 (selbst disassembliert)
Aufruf: Raumlader FUN_800396fc @0x80039788 `jal 0x800314b0` (wenn Nibble != work_vars[0x10], s. K1.3).
FUN_800314b0 (ghidra1_V2.txt Z.~126300, RE_15_Quellcode_V2/FUN_800314b0.c, re15_disasm.py):
* @0x800314d4 `lbu v0,DAT_800aca5c`, @0x80031500 Tabelle **@0x80073f70** (u16 je Modell: `3c 00 3d 00 ... 4b 00`) ->
  @0x8003151c `jal 0x80013b60` laedt die PLD nach 0x801bd814. Modell 15 -> Datei 0x4B = PL0F.PLD (CD-Dateitabelle
  @0x8006f440, Eintrag 74: Groesse 126892 = PLD/PL0F.PLD; Eintrag 59 = 189108 = PL00.PLD).
* ALLE VIER Teile werden gebunden: @0x8003154c `sw v0,DAT_800acbc0` = dir[0] **EDD** (Spieler +0x16c), @0x80031578
  `sw v0,DAT_800acad8` = dir[1] **EMR** (+0x84), @0x800315a0 `sw v0,DAT_800aca64` = dir[2] **MD1**
  (@0x800315c8 `jal 0x80022150`), @0x800315b0 `jal 0x8004ee78` = dir[3] **TIM**; @0x80031614 `jal 0x8001e56c` +
  @0x80031624 `jal 0x8001e5b0` = Skelett-Aufbau aus dem neuen EMR. => Skelett + Animation + Mesh + Textur der Figur.
* Waffe: @0x8003167c `lbu v1,DAT_800aca5c`, @0x80031680 `jal 0x800c00a8` (DEBUG.BIN, resident @0x800c0000: Datei-
  Offset 0xa8; liest Byte-Tabelle @0x800c00d4[Modell] = `01 x15, 00` -> 1 fuer Modell 0..14, 0 fuer 15),
  @0x80031690 `sb v0,DAT_800aca5d`; @0x80031694 `ori v0,zero,0xf` / @0x800316b8 `bne v1,v0` / **@0x800316c4
  `sb zero,DAT_800aca5d` = Modell 0x0F -> Waffe 0** (Bytes `03 00 62 14` / `5d ca 20 a0`).
* Danach @0x800316e8 `jal 0x800440c4(Modell)` (CORE-Klangbank je Modell, Tabelle @0x80073a88/@0x80073ab0),
  @0x800316f0 `jal 0x80036b68` (Waffe), @0x80031708 `jal 0x80043d8c(Waffe)` (ARMS-Klangbank), @0x80031718
  `sh 100,DAT_800acaee` (HP = 100).
FUN_80036b68 (Waffenlader): @0x80036b84/@0x80036b8c Modell/Waffe, Tabelle **@0x800741e8** (u16 je Modell:
`4c x4, 61 x4, 4c x7, 76 00`) -> Datei Basis + Waffe; Modell 15 -> 118 = PL0FW00.PLW (Eintrag 117: 14696 B).
@0x80036be4 `sw ..,DAT_800acbc8` = dir[0] W-EDD (+0x174), @0x80036c04 `sw ..,DAT_800acbc4` = dir[1] W-EMR (+0x170);
**@0x80036bf8 `lbu v1,DAT_800aca5d` / @0x80036c08 `beq v1,zero,0x80036da0` (Bytes `65 00 60 10`): Waffe 0 ->
KEIN Hand-/Waffen-Mesh, keine Waffen-TIM** (der ganze Anhang @0x80036c10..@0x80036d9c entfaellt).
PLD-Inhalt (`K_belege`, eigener Schnitt ueber die Verzeichnistabelle): PL0F.PLD EDD 24 Clips
{28,28,133,30,20,38,1x11,45,31,1x5}, EMR 15 Knochen Hierarchie wie PL04, Bind Wurzel (0,-1268,0) (Kind; PL04 -1709),
MD1 15 Meshes, TIM 256x256 8bpp. PL0FW00.PLW: EDD 9 Clips {20,1,40,1,1,28,15,1,8}, EMR 15 Knochen 115 Keyframes,
MD1 1 Mesh, dir[3] 2336 B.

### K1.6 Weitere Modell-0x0F-Stellen im Original (alle 33 Leser von DAT_800aca5c, ghidra1_V2.txt, + Overlays)
* Bewegung je Modell [Tempo, Drehung], Index Modell*2 (Zeile 15 = Sherry, Bytes aus PSX.EXE):
  GEHEN @0x80073ea4 (Leser Tempo @0x800324d4, Drehung @0x800323a0) Zeile 15 @0x80073ec2 = `32 30` = [50,48];
  RUECK @0x80073ec4 (@0x80032d5c / @0x80032c4c) Zeile 15 `23 30` = [35,48]; DREHEN @0x80073ee4 (@0x80032a08) Zeile 15
  `00 50` = [0,80]; RENNEN @0x80073f24 (@0x800327e8 / @0x80032688) Zeile 15 @0x80073f42 = `78 40` = [120,64].
  (Leon Zeile 0: [75,48] [70,48] [0,96] [200,72]; Elza Zeile 4: [70,48] [65,48] [0,96] [210,72].)
* Gehen @0x8003258c / Rennen @0x80032898: `ori v0,zero,0xf` + `beq` -> Modell 0x0F ueberspringt den Fussanker
  FUN_800369f8 und die Tempo-Mischung (Port fuehrt diese Mischung fuer niemanden -> nichts zu tun).
* Hochklettern @0x8003818c-a8: Ausloesebild 0x63 statt 0x1d fuer Modell 0x0F (Sherrys Clip 2 hat 133 Bilder).
  Port: climb_common.c verglich `p->type == 0x0f` — der Spieler-Typ ist im Port aber immer 0 (actor_common.c:38),
  der Zweig war tot -> jetzt `re15_sherry_r41_modell()`.
* Zielen nur mit Waffe != 0 (`lbu DAT_800aca5d` + beq/bne zero vor `ori v0,zero,0x701`): Stehen @0x80032014, Gehen
  @0x80032430, Rennen @0x80032720, Drehen @0x80032a80, Rueckwaerts @0x80032cb4. Port zielte mit R1 immer.
* Wund-Zeile = Modell (FUN_80037c1c @0x80037c8c `lbu DAT_800aca5c`, 8 Zeilen @0x80074208-87); Zeile 15 liegt hinter
  der Tabelle (Original liest @0x800742f8 Fremddaten) -> Port: keine Zeile (re15_kleidung_panel_ziel liefert 0).
* Overlays STAGE1-5 je 1 Leser (Griff-Zweig Leon/Elza ueber Bit 2) — nicht Sherry-spezifisch.

### K1.7 Umsetzung (Commit cdb12ea8, Messung nachher K1.8)
| Datei | Inhalt |
|---|---|
| NEU engine/src/sherry_r41.c, include/re15_sherry_r41.h | Modell-Spiegel (`re15_sherry_r41_modell`), Waffe beim Wechsel (0x0F -> 0 @0x800316c4; zurueck -> vorherige Waffe = PORT-WAHL wie Westen-Weg, Original: DEBUG.BIN-Tabelle @0x800c00d4 = 1 Messer), Ausruesten gesperrt solange 0x0F, Bewegungs-Zeile 15, Messhaken RE15_WORKVAR_AT |
| NEU platform/pc/src/sherry_pc_r41.c/.h | Koerper-Tausch: bei neuer Waffen-Familie (@0x800741e8[Modell]: 76/97/118) EDD+EMR der PLD binden (@0x8003154c/@0x80031578), W00-Bank der Familie (PL0FW00.PLW, @0x80036be4/@0x80036c04, Verbund Bindelage+Keyframes wie main.c [wpn-bank]), alle Hand-Meshes aus (@0x80036c08), Wunden loeschen (FUN_80037c1c @0x800316c8), CORE-Bank des Modells (FUN_800440c4 @0x800316e8 -> CORE0F.VB 41200 B); Boot-Familie gesichert + beim Zurueckkehren wiederhergestellt; gleiche Familie (Weste PL00/01, Elza PL04-07: EDD/EMR md5-gleich) -> unveraendert |
| platform/pc/main.c (Haken) | include; Anmeldung nach pl00_*; Aufruf in pc_sync_player_model nach MD1/TIM; Hand-Mesh nur bei Waffe != 0 (@0x80036c08); Wund-Zeile = Modell (@0x80037c8c) |
| engine/src/re15_damage.c (2 Zeilen) | `re15_player_waffe_roh` (nur aca5d, kein Ausruest-Platz); Ausruesten gesperrt solange Sherry |
| engine/src/player_common.c (4 Haken) | Tempo/Drehung Zeile 15 fuer Modell 0x0F; kein Zielen mit Waffe 0 (5 Original-Stellen oben) |
| engine/src/climb_common.c (1 Zeile) | Ausloesebild 0x63 fuer Modell 0x0F (@0x8003818c) ueber den Modell-Spiegel |

### K1.8 Messung nachher (gleicher Lauf, Bau cdb12ea8: `re15_port/build/r41k_laeufe/n30c1s`)
debug.log: `[sherry-r41] Spielermodell 4 -> 15, Waffe (aca5d) 0`, `[sherry-r41] Koerper -> Familie 118 (PL0F): 15 Knochen,
24 Clips, W00 9 Clips, Wurzel-Bind y -1268`, `[pld] Spielermodell -> PLD/PL0F.PLD`. Bild
**`K_belege/k1_original_vs_port_nachher_30c1.png`** (Original-Puffer | Port vorher F200 | Port nachher F20 | F200):
nachher steht Sherry wie im Original als Kind mit geschlossenen Gliedern, Schuhe im Schatten, keine Waffe.

## K2 — ROOM3091-Szene "staendig falsche Animationen"

### K2.1 Skript (ROOM3091 sub07 @0x016CA..@0x01DB0, `K_belege/k2_scd3091.txt`) und Rollen
Spieler = SHERRY (Modell 0x0F, K1.3), Gegner 0 = John (0x49, Sce_em_set @0x014E0), Gegner 1 = Elza als NPC (0x42 aus
CDEMD1 Sektor 1505, @0x014F4). Plc_motion(0,n) = Kanal-Sub 0 = Raum-RBJ (+0x180/+0x184, Handler @0x80041b90:
+0x94 = pc[2] @0x80041ba8, +0x5 = pc[1] @0x80041bc4, +0x1c4 = pc[3] @0x80041bc8), Plc_motion(2,6) = Sub 2 = eigene
Bank (+0x170/+0x174), Plc_dest Modus 5/9 = Geh-/Dreh-Sub (eigene Bank). Raum-RBJ 3091: rec0 Marker 0x1 (1 Clip,
2 Keyframes = Platzhalter), rec1 Marker 0x2 (10 Clips, John), rec2 Marker 0x3 (14 Clips; 0..10 Erwachsenenhoehe =
Elza, 11..13 Kindhoehe = Sherry, K1.3). Binder FUN_8001b3f8 (Spur J, @0x8001b410/@0x8001b450/@0x8001b4c4):
Spieler = letzter Record mit Bit 0 = rec2, Gegner 0 = rec1, Gegner 1 = rec2.

### K2.2 Messung vorher (Port, Basis 700483de ohne Binder; Laeufe `re15_port/build/r41k_laeufe/v3091a|b`)
Elza-Neuspiel, RE15_DEBUG_JUMP=3090@250, RE15_SUBSTART=7@40#3091, Kreuz alle 30 Bilder (RE15_PAD_AT), RE15_ANIM_TRACE,
RE15_FRAMEDUMP. Clip-Folge `K_belege/k2_port_vorher_3091_clipfolge.txt`, Bilder `k2_vorher_bogen1.png`,
`k2_vorher_zoom1.png`. debug.log `[rbj] room 3091 cinematic overlay: 1 clips, 2 kf`:
* Spieler: in ALLEN Spielerzeilen Clip 0 mit 2 Bildern (rec0-Platzhalter) — die Figur steht starr, keine Geste.
* Gegner 1 (Elza-NPC): UNGEBUNDEN -> Plc_motion(0,n) spielt Bank 0 des NPC-Blobs (dir1: Clips {34,32,50,20,20,20,
  50,1,...}): u.a. Clip 9 = 1 Bild, hunderte Bilder lang gehalten (F520-661, F762-1142, ...), Clip 3/4 mit
  Wurzelversatz -> Bilder zeigen verrenkte/kniende Posen.
* Gegner 0 (John): rec1 (richtig gebunden).
=> Nutzer-Befund reproduziert. Ursache: Binder (A-E5, Spur J) — Spieler rec0 statt rec2, Gegner 1 ohne Record.

### K2.3 ORIGINAL gemessen (DuckStation-GDB, `re15_port/tools/r41_k/k2_orig_3091.py`)
Ausgang: Spielstand HASH-957757946319438E_1.sav (sauber, Modell 0x0F in 30C1, Kopie scratchpad K41/orig). Bild 20:
Flags (4,8)=1 + (9,21)=1 (Bank-Tabelle @0x80074664), Tuerwechsel wie FUN_800430bc mit der Nutzlast der 30C1-Tuer
Slot 0 (@0x00B5A, Ziel 3091), Spieler vor die Riegeltuer (Slot 1), Viereck (= raw 0x80, AOT-Pruefung) -> sub02 ->
sub07: "SZENE (4,16)=1 bei Bild 175". Danach je Bild (Haltepunkt 0x8001ce04) Spieler + Gegner 0/1: Sub, Phase, Clip,
Bild, +0x1c4, +0x180/+0x184, Lage; Kreuz alle 40 Bilder. 2953 Szenenbilder bis zum Raumwechsel nach 30C1.
settings.ini und resume.sav danach bytegleich zurueck (Protokoll-Kopf `K_belege/k2_orig_3091_gdb_lauf2_kopf.txt`,
volles Protokoll `k2_orig_3091_gdb_lauf2.log.gz`).
* RBJ-Kanaele im RAM: Spieler (Typ 0x0f) und Gegner 1 (0x42) w180/w184 = 0x8012737c/0x8012da54 = rec2, Gegner 0
  (0x49) 0x80121de4/0x80126edc = rec1 (Basis RBJ+0x1440; Record-Offsets aus dem RDT-Trailer) -> Binder-Ergebnis
  der Spur J am Original bestaetigt.
* Sub-0-Clipfolgen (Original):
  Spieler: 13(Schleife), 12, 12r, 13, 11, 11r, 13, 11, 11r, 13, 12, 12r, 13, 12, 12r, 11, 11r, 13
  Gegner 0: 3, 3r, 2, 0, 0r, 8, 0, 0r, 8, 1, 1r, 8, 4, 4r, 3, 3r, 8, 1, 1r, 0, 0r, 8, 4, 4r, 0, 0r, 8, 3, 3r, 0, 0r, 8
  Gegner 1: 4, 4r, 3, 3r, 9, 1, 1r, 0, 0r, 9, 4, 4r, 9, 1, 1r, 0, 0r, 9, 4, 4r, 9, 5, 5r, 2, 9, 6, 6r, 9
  (r = Plc_flg 0x80 rueckwaerts). Dazwischen die Geh-/Dreh-Subs 5/6/9 (eigene Bank Clip 0/1/2/5) und Sub 2 Clip 6.

### K2.4 Umsetzung
Keine eigene Aenderung am Binder (A-E5): J's Commit e4a9f39c `rbj-binder` UNVERAENDERT per cherry-pick (hier
299b6deb). Dazu K1 (Sherry-Koerper), damit rec2-Clips 11..13 auf dem Kind-Skelett laufen, fuer das sie gebaut sind.

### K2.5 Messung nachher (Bau cdb12ea8, Lauf `re15_port/build/r41k_laeufe/n3091s`)
Sherry als Spielfigur (RE15_WORKVAR_AT=0x10:15@200 vor dem Sprung -> Raumlader tauscht den Koerper), sonst wie K2.2.
debug.log: `[rbj-binder] ROOM3091 Spieler rec2 (alt rec0) Gegner 0:rec1(alt rec1) 1:rec2(alt rec-1)`,
`Aktor 1 Typ 0x49 Marker-Slot 1 -> rec1`, `Aktor 2 Typ 0x42 Marker-Slot 2 -> rec2`.
Vergleich `K_belege/k2_vergleich_orig_port_nachher.txt` (`re15_port/tools/r41_k/k2_vergleich.py`): die Sub-0-
Clipfolgen von Spieler, John und Elza-NPC sind im Port nachher IDENTISCH mit dem Original (gleiche Clips, gleiche
Reihenfolge, gleiche Vor-/Rueckwaerts-Paare, gleiche Schleife Clip 13); Bildzahlen je Clip gleich (rec2: 13=40,
12=30, 11=15; Elza 4=30, 3=20, 9=24, 1=30, 0=20, 5=25, 2=30, 6=30; John rec1). Absolute Bildnummern weichen nur
durch den Tastentakt der Nachrichten ab (Port 30, Original 40 Bilder). Bilder `K_belege/k2_nachher_zoom1.png`.

### K2.6 Bild fuer Bild: Original-Spielstaende gegen Port (gleicher Szenenzustand)
Zweiter Original-Lauf (`k2_orig_3091.py --lb 300,700,...,2700`, Protokoll-Kopf `K_belege/k2_orig_3091_gdb_lauf5_kopf.txt`):
derselbe Ablauf (wieder 2953 Szenenbilder), dazu je ein Slot-1-Spielstand (LB = SaveSelectedSaveState) bei Szenenbild
300/700/1100/1500/1900/2300/2700 (gespeichert ~6 Bilder spaeter); Slot-1-.sav/.bak und resume.sav danach bytegleich
zurueck (md5). Aus jedem Stand der angezeigte VRAM-Puffer (y 240..479; Puffer 1 enthielt den halb gezeichneten
naechsten Rahmen). Port-Gegenstueck = erstes Framedump-Bild (alle 20 Bilder, Lauf `r41k_laeufe/n3091t` ueber die
echte Riegeltuer) mit DERSELBEN Clip-Kombination je Aktor (Spieler/John/Elza inkl. Rueckwaerts-Bit) in Reihenfolge.
**`K_belege/k2_original_vs_port_3091.png`**: alle 7 Paare zeigen dieselbe Textzeile ("Yes, somehow!...", "What's your
name, kid?", "You seem to have taken a liking to her.", "Your... dad...?", "I dunno. Sounds like somebody's in there."
mit Elzas Pistolen-Pose = eigene Bank Clip 6, "Will do!", "Sherry: Good luck!"), dieselbe Aufstellung (Elza links,
John Mitte, Sherry als Kind rechts) und dieselben Posen.

### K1.9 Gehen/Rennen der Spielfigur Sherry - Original gegen Port (je Bild)
* ORIGINAL (`re15_port/tools/r41_k/k1_orig_tempo.py`, Spielstand 30C1, Steuerkreuz hoch bzw. hoch+Kreuz per vgamepad,
  Lage 0x800aca88/0x800aca90 + Tempo DAT_800acae0 je Bild): Gehen **50 je Bild**, DAT_800acae0 = 50, W-Clip 5;
  Rennen **120 je Bild**, DAT_800acae0 = 120, W-Clip 0 (20 Bilder); Wand bei z -3632 (`K_belege/k1_orig_tempo_30c1.txt`,
  `k1_orig_rennen_30c1.txt`).
* PORT nachher (gleicher Ort, RE15_PAD_AT, RE15_STATE_LOG): Gehen 50 je Bild, Rennen 18 x 120 + 2 bis z -3632 - dieselbe
  Folge wie das Original (-1350 -> -3630 -> -3632). R1 gehalten: kein Zielen (mo bleibt Ruhe-Sentinel).
* Nebenbefund (nicht Spur K): nach dem Loslassen bremst das Original ab (DAT_800acae0 80/40/0/-40, Schritt zurueck,
  B52-B55) = Renn-Stopp @0x80032918-4c, spielweit Spur E (Entscheid E-E2); der Port haelt hier sofort.

## Tests (gezielt, keine volle Suite)
Neue Riegel (`re15_port/tests/unit/probes/r41_stage3szenen_b.cmake`, Quelle `tests/unit/test_r41_stage3szenen_b.c`,
`tests/integration/test_r41_stage3szenen_b_{sherry,3091}.cmake`; eigene exe-Kopien re15_pc_r41ks_/r41kz_ + Laufordner,
RE15_*-Umgebung des Aufrufers geloescht, `cmake_minimum_required` in jedem -P-Skript, libm unter Linux):
| Riegel | misst | Punkt |
|---|---|---|
| unit_r41_stage3szenen_b_pins | Original-Bytes PSX.EXE (@0x80073f70[15], @0x800741e8[15], CD-Eintraege 74/117 = Dateigroessen, @0x80031694/b8/c4, @0x80036bf8/c08, Zeile 15 der vier Tabellen, 5 Ziel-Sperren), ROOM3061 @0x011F6, ROOM3091 @0x01D9C/@0x01DA8, RBJ rec2 Kind-/Erwachsenenhoehe | K1, K2 |
| unit_r41_stage3szenen_b_waffe | Modellwechsel -> Waffe 0, Ausruesten gesperrt, Tabelle Zeile 15, zurueck -> Waffe wieder da, Elza 4->5 unveraendert | K1 |
| unit_r41_stage3szenen_b_koerper | PC-Modul an echten Dateien: PL0F-Koerper (15 Knochen, Bind -1268, 24 Clips), W00 (9 Clips, 115 Keyframes), keine Hand-Meshes, Wunden 0, CORE 0x0F; zurueck auf PL04 (alle 42 Banken/Meshes); Weste/Elza-Varianten aendern nichts | K1 |
| integration_r41_stage3szenen_b_sherry | echte exe, Schacht-Tuer 3061 -> 30C1: Raumlader-Zeilen, Knochenlage Bild 60 (Huefte -1261, Brust -1726, Knoechel -71), Gehen 8/8 x 50, Rennen 10/10 x 120, R1 40/40 ohne Zielen | K1 |
| integration_r41_stage3szenen_b_3091 | echte exe, Viereck an der Riegeltuer (sce-5-Scan) -> sub07: Binder-Zeilen + Sub-0-Clipfolge Spieler/John/Elza = Original | K2 |

Gezielte Laeufe (Bau 2ac553ef + Dossier; Regex = eigene Riegel + Gruppen der beruehrten Module player_common/re15_damage/
climb_common/enemy_common+rbj_binder (J)/main.c-Haken: Zielen, Waffen, Inventar, Weste, Kleidung/Wunden, Klettern,
Raumwechsel, RBJ-/NPC-Szenen):
* `ctest --test-dir re15_port/build -R "^(unit|probe|prop)_(r41_stage3szenen_b|r41_stage3szenen_a_rbj|damage|weapon_select|inv_fsm|aim_|savedata|weste_effekt|re2_room1190_ab|wound_accumulator|room_transition_cmd|anim_|re2_weapon_rows|1090_climb_pin|rvd_scan_ungated_by_climb|1190_|autofire_pin|r17_waffen_loop_pin|r26_inventar|r30_android_n3_plc_aim|r30_granate|r35_granate|r35_inventar1050|r35_werfer|r36_elza_schuss|r36_granate|r36_inventar|r36_karte_r1190|r36_kleidung|r36_klettern|r36_schuss|r36_ziel1190|r37_beretta|r37_elzaems|r38_kleinkram|r39_elza_klettern|r39_inventar|r39_treffer|r39_zustand|tank_controls|npc|marvin|room1040|lane_b1|r35_cut1150|r35_cut10f0|r35_entladen|r36_szene|r37_roy|r37_szene11c0|r38_dining1141|r38_heliport1171|r38_npckante|r38_szenenschutz|r39_dining1141|r39_heliport1171|r39_johnruf1041|room1140|r36_zombieki_szenen|r38_elzatoene|r33_tuer1120|r34n_d_adaruf|r35_affen|r36_elza|r36_szenenleon|r36_gegnerzahl|r36_schluessel|r39_elzazombies|r39_funk10f1|r16_tuer1040)|^(r28_messer_dauerschlag|r29_1d3_doppelabzug|r31_trefferboxen)$" -j 6 --timeout 600 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 275`**
* `ctest --test-dir re15_port/build -R "^integration_(r41_stage3szenen_b_sherry|r41_stage3szenen_b_3091|r37_elzaems_elza3091|r37_elzaems_elza3071|weste_load_pin|r36_kleidung_bild|r36_kleidung_spielweg|r39_zustand_kleidung|r39_dining1141_zielen|r36_klettern|room_transition|r36_elza_schuss|r36_schuss|r36_ziel1190_scheiben|r39_inventar_status|r37_szene11c0|r36_szene1150)$" -j 3 --timeout 2400 --output-on-failure`
  -> **`100% tests passed, 0 tests failed out of 17`** (Total Test time (real) = 918.56 sec)

## OFFEN (mit Adresse, nicht erfunden)
1. Rueckwechsel auf Elza (3091-Ende): das Original setzt bei JEDEM Modell-Neuladen die Waffe auf die DEBUG.BIN-Tabelle
   @0x800c00d4[Modell] (= 1, Messer; @0x80031680-90) und HP := 100 (@0x80031710/@0x80031718). Der Port gibt die vorher
   ausgeruestete Waffe zurueck und laesst die HP stehen — wie sein Westen-Weg (Nutzer-Entscheid 2026-08-30 "Weste soll
   nicht heilen"). Fuer Sherry nicht entschieden -> Frage an den Nutzer (unten).
2. Elzas Bewegungswerte: Tabellen-Zeile 4 ist [70,48]/[65,48]/[0,96]/[210,72] (Gehen/Rueck/Drehen/Rennen), der Port
   faehrt fuer Leon UND Elza die Leon-Werte 75/70/96/200 (player_common.c Konstanten). Nicht Spur K (ausserhalb des
   Auftrags, aendert Elzas Spielgefuehl spielweit) -> Orchestrator.
3. Renn-Stopp (Abbremsen 80/40/0/-40 @0x80032918-4c, am Original gemessen B52-B55) fehlt im Port fuer alle Figuren —
   Spur E (E-E2), nicht K.
4. Wunden beim Westen-Wechsel: FUN_80037c1c (@0x800316c8) loescht im Original bei JEDEM Modell-Neuladen; der Port tut es
   nur beim Familienwechsel (Sherry hin/zurueck). Wund-Zeile 15 (Sherry) liegt im Original hinter der 8-Zeilen-Tabelle
   (@0x800742f8, Fremddaten) -> Port: keine Wund-Stempel fuer Sherry (Beta -> Retail).
5. Ausruesten waehrend Sherry ist gesperrt (PORT-WAHL, Form = RE2-Retail: Sherry fuehrt keine Waffe). Das Original
   wuerde Datei 118 + Waffe laden (FUN_80036b68 @0x80036bb4-c4) = keine PLW (Beta-Luecke).
6. PSX-Ziel: der Koerper-Tausch steckt im PC-Modul (platform/pc/src/sherry_pc_r41.c); platform/psx tauscht weiter nur
   MD1/TIM (PSX hier nicht gebaut).
7. Die Kriech-Szene ROOM3061 sub02 selbst (Sherry kriecht in den Schacht) ist Spur J; per RE15_SUBSTART=2 gestartet blieb
   sie im Port haengen (Lauf v30c1a, Plc_motion(1,22) + Gosub-Warten) — an J gemeldet (hier nur vermerkt). Meine Riegel
   nehmen deshalb den Tuerweg der Szene (Save 0x10 = 0x0F + Schacht-Tuer Slot 3) ohne die Szene.

## Fuer den Nutzer
* Keine neuen Assets (kein shared_assets/RE2), keine Sprachdateien; synchro/ und pri/ unveraendert.
* Was sich aendert: Nach der Kriech-Szene in 3061 spielt man in Elzas Szenario Sherry (wie im Original). Sherry ist jetzt
  das echte Kind-Modell mit eigenem Skelett und eigenen Bewegungen (PL0F.PLD + PL0FW00.PLW), traegt KEINE Waffe, kann
  nicht zielen und laeuft so schnell wie im Original (Gehen 50, Rennen 120 je Bild; Elza 70/210). In der 3091-Szene ist
  Sherry die Spielfigur, Elza steht als NPC mit ihrer Pistole daneben (so baut es das Original: Gegner 1 = Typ 0x42 aus
  CDEMD1 = Elza-Modell); alle drei Figuren spielen jetzt dieselben Gesten in derselben Reihenfolge wie das Original.
* Entscheidungsfragen: (a) Nach der 3091-Szene setzt das Original Elzas Waffe auf das Messer und fuellt die Energie auf
  100 — soll der Port das uebernehmen oder (wie jetzt) Elzas vorherige Waffe und Energie behalten? (b) Elzas Lauf-/
  Gehtempo im Port entspricht Leon (200/75) statt Elzas Original-Werten (210/70) — angleichen?
