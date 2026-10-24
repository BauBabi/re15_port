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
- 2026-10-24: Dossier angelegt.

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

=> Einordnung (Memory reai-v2-beta-zu-retail): Das Original zeigt in ROOM3091 fuer Sherry einen **Platzhalter des
Prototyps** (Elza-NPC-Modell mit Pistole unter Typ 0x42) — in jedem anderen Sherry-Raum des Elza-Szenarios steht das
fertige Sherry-Modell 0x4B. Das ist UNFERTIG, nicht "fertig und anders" -> Ziel = Sherrys fertiges Modell (0x4B), ohne
Waffe (Nutzer-Wortlaut). Mechanik der Umsetzung siehe K1.4 (nach der Messung).

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
