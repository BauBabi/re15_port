# Runde 41 — Spur J "stage3szenen_a": Abnahme 1 (unabhaengig)

Baum `.claude/worktrees/r41_stage3szenen_a`, Zweig `r41/stage3szenen_a`, Kopf ea6abad5 (Nachbesserung 1), Merge-Basis
master 4c667c97, master-Kopf beim Start dba6ff8e. Pruefer: Abnahme 1, 2026-10-25 (nachts). Arbeitsordner (Laeufe, Logs,
Bilder, eigene Leseskripte): Scratchpad `J41abn1/` (Sitzung c41eae99). Nichts am Code geaendert; nur diese Datei committet.

## Punkte (Wortlaut AUFTRAG.md, Zeilen 83-85)
1. "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist falsch in ROOM 3060."
2. "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von
   dieser Cutscene hat Elza auch erneut eine falsche Animation."
Dazu die Maengel der Abnahme J 0: V1 (Vertrag) und M2 (Doku ROOM4031), Orchestrator-Entscheid J-E1.

## Stand der Pruefung (fortlaufend)
- [x] Diff gelesen (`git diff master...HEAD`, 46 Dateien; Code-Diff vollstaendig gelesen; seit Abnahme 0 (ed8b1457) im
      Code nur der Kommentar-Kopf `re15_superobj_r41.h`, dazu Riegel (Zensus, 3060 per Quadrat, probes-Kopf) + Dossier)
- [x] Bau: `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)`, "ninja: no work to do"; exe 23:37:49 und
      Test-exe 23:38:53 juenger als jede Quellaenderung (letzte 23:32, Kommentar-Kopf 23:28), `git status` leer
- [x] Riegel der Spur unit 7/7, integration 2/2 (eigene Laeufe, s. Gate T)
- [x] RE-Gate (7 Adressbereiche selbst disassembliert, s. Gate RE)
- [x] Gegenprobe mit der master-exe (Riegel 3060 rot; Vorher-Bilder 3060/3061 reproduzieren die Nutzer-Befunde)
- [x] Integrations-Probe (`git merge-tree`, ohne Aenderung am Baum)
- [x] Original-Savestates selbst gelesen (eigenes Skript `J41abn1/py/orig_lesen.py`, `orig_3061.py`)
- [x] M2-Bytes selbst gelesen (25 Byte-Folgen in ROOM4030/4031/4040/4041/4000/4001)
- [x] Punkt 1 gemessen (eigene Laeufe p1a/p1b, echter Tuerweg 3040 -> 3060 per Quadrat, Framedump, Wechsel nach 3020)
- [x] Punkt 2a gemessen (eigener Lauf p2, echter Tuerweg 3011 -> 3061 per Quadrat, Szene per Quadrat am AOT, Framedump)
- [x] Punkt 2b gemessen (dto.)
- [x] gezielte Riegel der beruehrten Module, Unit-Ebene: 227/227 (s. Gate T)
- [ ] gezielte Integrationsriegel der beruehrten Module (31, laeuft)
- [x] Vertrag (Pfade, Woerter, Env-Schalter, Bits/Ids) — s. Gate V
- [x] Nebenwirkungen — s. Gate N

## Gate RE — selbst disassembliert (info/Re1.5/PSX.EXE, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`)
| Stelle | eigene Disasm (Auszug) | Behauptung im Code/Dossier | Urteil |
|---|---|---|---|
| Super_set 0x80040b20 | `80040b38 andi v1,a1,0xc0`; 0x80 -> `80040bdc andi v0,a0,0x80` (Teil-Bit); `80040be4..bf4` v1 = pc[2]*500; `80040c18 addiu at,at,-11340` (0x8009d3b4) + v1 -> bei pc[2] = 0x80: 0x8009d3b4 + 0xFA00 = **0x800acdb4 = Gegner 0 + 0x188**; `80040c20 lw v1,0(at)`; Teil*172 (`80040bfc..c24`) + 64 (`80040c30`); `80040c74 sw v0,116(a2)`; danach `80040c78..` lokal pc+4/6/8 | Vater = Gegner-Teil-Weltmatrix, **kein Lesen von Typ/Zustand** (M2) | stimmt |
| FUN_800369f8 | `80036a30 lw s3,392(v0)`, `80036a34 jal 0x80068098`, `80036a48/58 jal 0x80022da0` (s3+24, s3+196), `80036a60..70` s3 += a1*516 + 688, `80036a78/88/98` s3-320 / s3-148 / s3+24 (= Teile 2,3,4 bzw. 5,6,7, Stride 172); Modus 0: `80036ab4 lw v1,36(sp)`, `80036ab8 lw a0,84(s3)`, `subu`, `subu`, `80036ac8 sw v0,52(a1)` (x); `80036ad4..aec` dasselbe mit 44(sp)/92(s3) -> 60(a1) (z) | feste Kette 0-1-2-3-4 / 0-1-5-6-7, x/z -= (neu - Part+84/+92) | stimmt |
| Sub 2 0x80050f00 | `80050f88 lw a0,368(v0)` (+0x170), `80050f90 lw a1,372(v0)` (+0x174), `80050f94 jal 0x8001f314`; Rueckgabe != 0 -> `80050fe8 ori v0,zero,0x2` / `80050fec sb v0,6(v1)` (Phase 2); `80051004 andi v0,v0,0x4` -> `80051010 sb v0(=1),6(v1)`; Phase 2 -> `80050f24 beq v0,zero,0x80051014` = Ruecksprung ohne anim_set | Kanal +0x170/+0x174, Halten Phase 2, Schleife nur mit Bit 4 | stimmt |
| W-Bank-Lader 0x80036b68 + Tabelle 0x800741e8 | `80036b84 lbu 0x800aca5c` (Modell), `80036b8c lbu 0x800aca5d` (Waffe), `80036bac addiu at,at,16872` (0x800741e8), `80036bc4 addu a0,v0,a0`, `80036be4 sw v0,-13368(at)` (0x800acbc8 = +0x174), `80036c04 sw v0,-13372(at)` (0x800acbc4 = +0x170); Tabelle (u16) `4c 4c 4c 4c 61 61 61 61 4c 4c 4c 4c 4c 4c 4c 76` | Modell 0x0D -> 76 (PL00Wxx), 4 -> 97 (PL04Wxx), 15 -> 118 | stimmt; Gegenprobe: 97-76 = 21 = Zahl der Dateien PL00W00..W14, 118-97 = 21 = PL04W00..W14 (shared_assets/PSX/PLD) |
| NPC-Executor 0x80050c4c..ca8 | `80050c50 xori v0,v0,0x20`, `80050c6c andi v0,v0,0x1` -> `80050c78 jal 0x800369f8` (Delay `addu a1,zero,zero`), `80050c94 andi v0,v0,0x2` -> `80050ca0 jal 0x800369f8` (Delay `80050ca4 ori a1,zero,0x1`) | Bit 1 -> Kette 0, Bit 2 -> Kette 1 | stimmt |
| Super_reset 0x80040cd0 | `80040cd8..ce0 lw 92/96/100(v0)` (Welt +0x5c/+0x60/+0x64), `80040ce8 addiu v1,v1,11596` (0x80072d4c) -> `80040cec sw v1,116(v0)`, `80040cf0..cf8 sw 52/56/60(v0)` (+0x34/+0x38/+0x3c), `80040cfc..d1c lhu 2/4/6(a1)` -> `sh 104/106/108(v0)` (Rot), `80040d18 addiu a1,a1,8` | Welt-Lage -> lokal, Vater = Welt, Rot = pc+2/4/6, 8 Bytes | stimmt |
| RBJ-Binder 0x8001b464..4d8 | `8001b470 andi v0,a0,0x1`, `8001b478 srl a0,a0,1` (Delay), `8001b47c andi v0,a3,0xff`, `8001b498 addiu at,at,-12884` (0x800acdac = Gegner + 0x180) + a3*500, `8001b4a0 sw t1,0(at)`, `8001b4c0 bne a0,zero,0x8001b470` mit Delay **`8001b4c4 addiu a3,a3,1`** (je Bitposition), `8001b4cc..d4` Record-Schleife ohne a3-Ruecksetzen | a3 kumulativ, spaeterer Record ueberschreibt | stimmt |

## Original selbst gelesen (Savestates des Bau-Agenten, eigenes Skript `J41abn1/py/orig_lesen.py` / `orig_3061.py`)
Code-Gleichheit RAM = PSX.EXE (Auslieferungsstand) in jedem gelesenen Stand: Stub 0x80026e4c `08 00 e0 03`, 0x80041b90,
0x80050f00, 0x800369f8, 0x80040b20, 0x80036b68, 0x80050be8, Tabelle 0x800741e8 — alle gleich.
PLW-Dateien selbst geparst: PL00W01.PLW Verzeichnis @0x59a0, EDD @+0x8, 14 Clips, **Clip 6 = 10 Bilder, Eintraege
EDD+0x2e4..0x30c, letztes Bild EDD+0x308**; PL04W01.PLW EDD @+0x8, 14 Clips, **Clip 6 = 10 Bilder, EDD+0x3c0..0x3e8, letztes
Bild EDD+0x3e4**. (PL00W00/W01/W02 und PL04W00/W01/W02 sind byte-gleiche Dateien. Der PLW-Puffer 0x801d7700 im RAM von
o3060_s2_t03 ist in EDD (Datei 0x8..0x464) und EMR (0x464..0x49cc) byte-gleich mit PL00W01.PLW; die 89 abweichenden Bytes liegen
nur in 0x49d0..0x529b = Bloecke 3/4 des Verzeichnisses @0x59a0, nicht in den Animationsdaten.)
- `J41/orig/o3060_s2_t03_6.5s.sav` (ROOM3060): Spieler **Modell 0x0d (Marvin), Waffe 1, cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x95 0,
  +0x168 = +0x174 + 0x308 = PL00W01-Clip-6 Bild 9 (letztes)**, +0x170 801d7b64 / +0x174 801d7708, (2063,-17192) ry 1601.
  t02 (5,0 s): cmd 4 / Sub 6 / Clip 1 (+0x168 = EDD+0xc8). t04 (8,0 s): **Modell 0x00 (Leon), cmd 0 / Sub 0 / Phase 0** bei
  (-11250,-13250) ry 3072 (ROOM3020, Ankunft der 3060-Tuer).
- `J41b/orig/o3061_s2_t08..t19` (ROOM3061, Elza Modell 4, Waffe 1):

| Stand | Elza | Sherry (Gegner 0) | Gitter (Objekt 0x800b3f98) | Hund (Gegner 2) |
|---|---|---|---|---|
| t09 22 s | Sub 0 | Clip 0 +0x95 20 (-18860,-26266) | lokal = Welt (-19095,-495,-27045) Rot (0,3072,0), Vater 0x80072d4c | cmd 4 Sub 1 (-13350,-22000) |
| t10 24 s | Sub 0 | Clip 0 +0x95 78 | (-18356,-266,-26788) Rot (72,3072,0) | |
| t11 26 s | Sub 0 | Clip 1 +0x95 47, +0x1c4 0x22 (-19054,-27044) | **(-18116,-266,-26788)** = Endlage | |
| t12 28 s | Sub 6 (-18510,-26206) ry 1939 | Clip 2 +0x95 30 (-18792,-28234) | dto. | |
| t13 30 s | dto. | Clip 2 +0x95 22 (-18799,-29042) | dto. | |
| t14..t16 | dto. | **(-18795,-30700)** | dto. | t16 cmd 1 Sub 12 |
| t17 38 s | **cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x95 0, +0x168 = EDD+0x3e4 = PL04W01-Clip-6 Bild 9**, (-18510,-26206) **ry 3651**; John Sub 2 Phase 2 Clip 6 | (-18795,-30700) | dto. | cmd 1 Sub 2 Clip 0 (-12932,-22482) ry 650 |
| t18 40 s | dto. | dto. | dto. | (-12578,-23421) ry 830 |
| t19 42 s | Modell 0x0f (Sherry), Waffe 0, cmd 0 (-19100,-1350) = ROOM30C1 | | | |

- `J41b/orig/o3061_s3` (dichte Reihe, 0,4 s; Code = PSX.EXE): t03 Sherry Clip 0 +0x95 52, Gitter lokal (252,192,-153) Rot
  (0,1024,3072) Vater 0x8014046c (= angehaengt, die Super_set-Werte); t04 Gitter (-18276,-266,-26788) Rot (72,3072,0) Vater
  0x80072d4c (rutscht); t05 Gitter (-18116,-266,-26788), Sherry Clip 1 +0x95 17 (-18980,-26266); **t08 Clip 2 +0x95 19 +0x1c4 0x26
  (-18780,-28017); t09 +0x95 44 0x05 (-18791,-28390); t10 +0x95 12 0x06 (-18783,-28806); t11 +0x95 39 0x25 (-18810,-29279)**; t12/t13
  (-18795,-30700). Die Soll-Werte des Riegels `unit_r41_stage3szenen_a_anker` (t08 -> t10, t09 -> t11) sind genau diese
  Original-Werte — der Riegel ist nicht selbstbestaetigend.

Diese Werte stimmen mit Dossier §1.3/§2.5/§2.8 und der Abnahme J 0 ueberein (hier unabhaengig nachgelesen).

## Gate M2 — Bytes selbst gelesen (shared_assets/PSX/STAGE4 = info/Re1.5/PSX/STAGE4, md5 gleich)
ROOM4031 @0x004F6 `06 00 1c 00`, @0x004FA `21 03 57 00`, @0x00516 `2c 05 05 31`, @0x0054E `06 00 34 00`, @0x00552 `21 03 58 00`,
@0x00556 `44 00 49 40 00 00 00 ff 2d a1 00 00 dc ae 00 00 d3 03 00 00` (Gegner 0 = Typ 0x49 John), @0x0056A `44 01 2d`,
@0x0057E `04 ff 18 08`, @0x00586 `44 00 2d 00 .. 00 02 00 00` (Gegner 0 = Typ 0x2D), @0x00620 `14 00 0a 00 05 00`, @0x0062A
`04 ff 18 02`, @0x0068E `18 06`, @0x006CE `22 03 4e 01`, @0x006DA `04 ff 18 07`, @0x0071E `2e 03 00`, @0x00722 `48 00 80 81 00..00`,
@0x0075C `22 03 58 01`; ROOM4030 @0x0052C `44 00 2d 00`, @0x0068C `22 03 4e 01`, @0x006DC `2e 03 00`, @0x006E0 `48 00 80 81 00..00`;
ROOM4040/4041 @0x01020 `22 03 57 01`; ROOM4000 @0x0105E und ROOM4001 @0x01046 `21 03 4e 00` — **alle 25 Byte-Folgen wie im
Kopf `re15_superobj_r41.h` / Dossier NB1.M2 behauptet**. Zusammen mit dem Disasm von 0x80040b20 (kein Typ-/Zustands-Lesen)
ist die berichtigte Aussage belegt.

## Punkt 1 — ROOM3060, Marvins Animation am Ende seines Abschnitts (Leon-Szenario)

### Messung (eigene Laeufe, exe-Kopie `re15_pc_abnJ1_<lauf>.exe`, Laufordner `J41abn1/laeufe/`, Skript `J41abn1/lauf.sh`)
**Lauf p1a_3060_tuer** — echter Tuerweg in den Raum: `RE15_SET_FLAG=4:199,4:196 RE15_GOTO_ROOM=3020 RE15_FORCE_EVENT=2@150
RE15_DEBUG_JUMP=3040@1150 RE15_PLAYER_POS=-8900,-26300,1024,0` (Ankunftspunkt der Gegentuer ROOM3060 Slot 2 @0x00ED6, Blick
zur Tuer), Quadrat alle 1,5 s (`A0.1,W1.4` x 70), kein RE15_FIRE_AOT (RE15_*-Umgebung des Aufrufers vor dem Start geleert),
RE15_ANIM_TRACE, RE15_STATE_LOG, `RE15_FRAMEDUMP=20-104/2`, `RE15_EXIT_AT=104#3060`. debug.log:
```
[force-event] scd_event_fire(2) at F150                       (ROOM3020 sub02, Leon -> Marvin)
[aot] DOOR FIRE slot=2 ... spawn=(-11250,0,-10900) / [room] PC loaded room30c0.rdt / [pld] Spielermodell -> PLD/PL0D.PLD
[parity] JUMP-Spawn ersetzt durch RE15_PLAYER_POS (-8900,-26300) rot=1024 band=0 / [room] PC loaded room3040.rdt
[input-script] Tick 2880 -> F40 Tasten 0x8000                  (Quadrat)
[aot] DOOR FIRE slot=0 rect=(-8800,-26950,hw=1850,hh=350) target_cut=4 spawn=(2063,0,-17192)
[room] PC loaded room3060.rdt
[wclip41] ROOM3060 F65 Spieler Plc_motion(2,6) -> W-Bank-Clip 6 (0x80073e30[2] = 0x80050f00, Kanal +0x170/+0x174, Waffe 1, 10 Bilder)
[flow] EXIT_AT: Bild 104 in Raum 3060 erreicht -> exit
```
Keine `[fire-aot]`-Zeile. RE15_ANIM_TRACE Spieler ROOM3060 (Bild:Motion/Clip/Bildzahl/Slot): F0-6 `210/1/16`, F7-24 `200/3/1`,
F25-27 `105/5/30` (Drehen), F28-43 `1/1/16`, F44-64 `2/2/52`, **F65-74 `6/6/10` Slot 0..9 je einmal, F75-104 `6/6/10` Slot 9** =
31 x Bild 9 gehalten (Phase 2 @0x80050fec). Kein Bild mit 50-Bilder-Clip 6 (PL00-Grundbank = Knien).
Bilder (Framedump 320x240, Ausschnitt 75,105-125,170 dreifach): `J41abn1/p1a_marvin_zoom.png` — F56-F68 steht, F70/F72 Arm
hoch, ab F74 bis F104 Arm vor der Brust gehalten. Original-Video selbst ausgeschnitten (`J41b/orig/o3060_v4.mp4`, ffmpeg fps=10,
Ausschnitt 150,210-250,340): `J41abn1/o3060v4_marvin_zoom.png` — o21-o32 steht, o33 Arm hoch, o34-o43 gehalten, ab o45 schwarz
(Tuer). **Gleicher Ablauf, gleiche Endhaltung, kein Knien.** Zusammen (oben Port, unten Original): `J41abn1/vergleich_p1_marvin.png`.

**Lauf p1b_3060_wechsel** (gleicher Weg bis in den naechsten Raum; `RE15_EXIT_AT=1200#3020` — Bild 1200 erreicht erst der
ZWEITE 3020-Aufenthalt, der erste endet bei Bild 1097; `RE15_FRAMEDUMP=0-60/4`):
```
[aot] DOOR FIRE slot=3 rect=(0,0,hw=0,hh=0) target_cut=6 spawn=(-11250,0,-13250)    (ROOM3060 sub03 Aot_on 3)
[room] PC loaded room3020.rdt / [pld] Spielermodell -> PLD/PL00.PLD
```
ANIM_TRACE: letzte ROOM3060-Bilder F95-F104 `6/6/10/9`; ROOM3020 F0-5 `210/1/16`, ab F6 `200/3/1` (Stand) — **kein W-Clip-6-Bild
nach dem Wechsel**. Bilder `J41abn1/p1b_3020_anfang.png`: F0 schwarz, F4 Aufblende, ab F8 Leon mit haengenden Armen.
Original (selbst gelesen): o3060_s2 t04-t07 (8,0-12,5 s) Modell 0, cmd 0 (Raumwechsel, im Video schwarz), **t08 (14,0 s) cmd 1 /
Sub 0 / Clip 3 (+0x168 = EDD+0x1a0 = W-Clip 3 = Stand)** bei (-11250,-13250) ry 3072 — Lage, Blick und Stand-Clip wie im Port.
Riegel `integration_r41_stage3szenen_a_3060` (eigener ctest-Lauf): Passed 114,1 s — Tuer per Quadrat, `40 Bilder Motion 6, davon
0 nicht Clip 6/10 Bilder, 31 x Bild 9 gehalten`.
Takt: Plc_motion(2,6) F65 -> Tuer nach F104 = 40 Bilder = `Sleep 40` @0x010C2 (`09 0a 28 00`) vor dem Modellwechsel @0x010D2 und
Aot_on 3 @0x010E4 (ROOM3060 sub03, Dump J41abn0/scd3060.txt); Halten 31 Bilder = 40 - 9.

### Urteil Punkt 1: **erfuellt**
Beleg: Port spielt am echten Tuerweg W-Bank PL00W01 Clip 6 (10 Bilder; Tabelle @0x800741e8 Modell 0x0D -> 76, Kanal
@0x80050f88/90) und haelt Bild 9 (@0x80050fec) bis zur Tuer; Original o3060_s2_t03 cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x168 =
PL00W01-Clip-6 Bild 9 (selbst gelesen); Bildfolge = Original-Video; nach dem Wechsel Leon im Stand wie Original t08.

## Punkt 2 — ROOM3061 (Elza): (a) Sherry kriecht in den Schacht, (b) Elzas Animation am Ende der Szene

### Messung — echter Weg durch die Tuer (Lauf p2_3011_tuer, Skript `J41abn1/lauf_karte.sh`)
Spielstand Elza (probe_r36_elza_karte) in **ROOM3011** bei (-7578,10065) Blick 0 vor der Tuer Slot 1 (Ziel ROOM3061), Bits
4:244/3:160, CONTINUE; `RE15_INPUT_SCRIPT_BASIS=spiel`, Start Bild 30: `A0.2,W14,XU3.8,R0.35,XU1.2` + Quadrat alle 1,5 s
(die Zeitachse beginnt im neuen Raum neu: in 3011 oeffnet das erste Quadrat die Tuer, in 3061 wartet Elza den Einlauf von
Sherry/John ab, rennt an die Wand und drueckt Quadrat am Ausloese-AOT); RE15_FUSSANKER_LOG, RE15_STATE_LOG, RE15_ANIM_TRACE,
`RE15_FRAMEDUMP=1200-1760/4`, `RE15_EXIT_AT=1#30C1`. debug.log:
```
[save] CONTINUE: resumed in room 3011 (hp=100)
[aot] DOOR FIRE slot=1 rect=(-6563,9810,hw=705,hh=900) target_cut=0 spawn=(-26027,0,-22532)      (Quadrat, ROOM3011 -> 3061)
[room] PC loaded room3061.rdt
[scd] thread start slot=10 first_op=0x22 / [scd F664] Cut_chg(6)                                (Quadrat am Slot-1-AOT -> sub02)
[superobj41] F1216 Super_set Objekt 0 Art 0x80 Teil-Byte 0x8E lokal (252,192,-153) Rot (0,1024,3072) -> angehaengt
[superobj41] F1242 Super_reset Objekt 0 Welt-Lage uebernommen -> lokal (-18753,-266,-26778) Rot (72,3072,0)
[wclip41] ROOM3061 F1623 Spieler Plc_motion(2,6) -> W-Bank-Clip 6 (... Waffe 1, 10 Bilder)
[aot] DOOR FIRE slot=3 ... spawn=(-19100,0,-1350) / [room] PC loaded room30c1.rdt / [pld] Spielermodell -> PLD/PL0F.PLD
```
Anker-Takte (155 `[fussanker41]`-Zeilen, Kette/Clip x Bilder): `0/1x22 1/1x22 0/1x11 1/2x26 0/2x24 1/2x26 0/2x24` = Plc_flg-Folge
sub04 (SET 1/2/1, 2 x [6/5]).

| Groesse | Port (Lauf p2) | Original (selbst gelesen) | Abweichung |
|---|---|---|---|
| Elza vor der Szene | F1200 (-18890,-24504) rot 1049 | t08-t11 (-18890,-24504) ry 1049 | 0 |
| Sherry vor dem Kriechen | F1200 (-18857,-26256) r1067 | t09 (-18860,-26266) | (3,10) — Ende des NPC-Gangs, Dossier §4.3 |
| Gitter Loslass-Lage (Super_reset) | (-18753,-266,-26778) | Endlage t11 (-18116,-266,-26788) - 640 = (-18756,-266,-26788) | (3,0,10) |
| Sherry Clip 1 Bild 46 | F1318 (-19051,-27034) | t11 +0x95 47 (-19054,-27044) | (3,10) |
| Sherry Clip 2 Bild 29, Runde 1 | F1376 (-18789,-28224) | t12 +0x95 30 (-18792,-28234) | (3,10) |
| Sherry Clip 2 Bild 21, Runde 2 | F1428 (-18796,-29032) | t13 +0x95 22 (-18799,-29042) | (3,10) |
| Sherry tiefste Lage | F1467 (-18792,-30700) | t14-t18 (-18795,-30700) | (3,0) |
| Elza nach dem Kriechen | F1400-F1600 (-18510,-26206) rot 1939 | t12-t16 (-18510,-26206) ry 1939 | 0 |
| Elza Ende | F1622-F1688 (-18510,-26206) **rot 3651** | t17/t18 (-18510,-26206) **ry 3651** | 0 |

Der Rest (3,10) entsteht vor dem Kriechen und wird vom Anker unveraendert mitgetragen (bytegleicher Riegel
`unit_r41_stage3szenen_a_anker` von Original-Stand zu Original-Stand, eigener Lauf: Abweichung (0,0) / (0,0)).

Elza RE15_ANIM_TRACE: F1595-1604 `2/2/52`, F1605-1622 `105/5/30` (Drehen), **F1623-1632 `6/6/10` Slot 0..9, F1632-1687 Slot 9
(56 x)**, 0 Zeilen Motion 6 mit anderem Clip/Bildzahl; ROOM30C1 F0/F1 `210/1/16` (Modell PL0F). Takt: F1623 -> letztes 3061-Bild
F1687 = 65 Bilder = `Sleep 5` + `Sleep 60` hinter Plc_motion(2,6) @0x011DE / John @0x011EA (sub02, Bytes `09 0a 05 00` @0x011E2
und `09 0a 3c 00` @0x011EE selbst gelesen). John (Aktor 2, Typ 0x49): F1628 (= F1623 + 5) `6/6/10` Slot 0..9, ab F1637 Slot 9
gehalten bis F1687 — wie Original t17 (Gegner 1 Sub 2 Phase 2 Clip 6).

Bilder (Port 320x240) gegen das selbst ausgeschnittene Original-Video `J41b/orig/o3061_v3.mp4` (ffmpeg ab 10 s, 5 B./s):
- Kriechen `J41abn1/p2_kriechen_zoom.png` / `J41abn1/o3061v3_kriechen_zoom.png` / zusammen `J41abn1/vergleich_p2a_kriechen.png`:
  Sherry kniet am Gitter und haelt es (Port F1200-F1216 / Orig o54-o58), zieht es heraus (F1224 / o59-o61), legt es links flach
  ab (F1232-F1264 / o62-o64), geht auf alle Viere und kriecht hinein (F1272-F1296 / o65-o71), nur noch die Beine schauen heraus,
  "Elza: Sherry, wait!", Elzas Arm oben rechts (F1304-F1336 / o72-o79), Elza tritt vor, Sherrys Fuesse in der Oeffnung
  (F1344-F1360 / o80-o82). **Gleicher Ablauf, gleiche Gitter-Ablage.**
- Elzas Ende `J41abn1/p2_ende_zoom.png` / `J41abn1/o3061v3_ende_zoom.png` / zusammen `J41abn1/vergleich_p2b_elza.png`
  (Uebersicht `J41abn1/p2_ende_voll.png`): Elza steht (F1588-F1604 / o120-o124), dreht sich (F1608-F1620 / o125-o127), hebt die
  Arme mit der Waffe nach vorn (F1624-F1632 / o128-o129) und haelt (F1636-F1680 / o130-o137); John ebenso. **Kein Knien.**
- Unterschied im Bild, nicht Spur J: der Hund laeuft im Port ab F1640 zu Elza (F1687 (-18666,-25265)), im Original nach rechts
  vorn (t17 (-12932,-22482) -> t18 (-12578,-23421)) = Nebenbefund N1 der Abnahme J 0, Dossier §4.7.

Riegel `integration_r41_stage3szenen_a_3061` (eigener ctest-Lauf, CONTINUE in 3061 + Quadrat am AOT): Passed 60,1 s — gleiche
Super_set/Super_reset-Zeilen, gleiche Anker-Takte, Sherry (-18792,-30700), Elza W-Clip 6 mit 56 x Bild 9, Wechsel 30C1/PL0F.

### Urteil Punkt 2a (Sherry kriecht in den Schacht): **erfuellt**
Beleg: Glied-Anker FUN_800369f8 (Disasm selbst: Kette 0-1-2-3-4 / 0-1-5-6-7, x/z -= Differenz) und Super_set/Super_reset
(0x80040b20/0x80040cd0) im Port; Lagen je Anker-Takt = Original bis auf den konstanten Rest (3,10) von vorher; Endlage im
Schacht z -30700 = Original; Bildfolge am echten Tuerweg = Original-Video.

### Urteil Punkt 2b (Elzas Animation am Ende der Szene): **erfuellt**
Beleg: Plc_motion(2,6) @0x011DE spielt W-Bank PL04W01 Clip 6 (Tabelle @0x800741e8 Modell 4 -> 97), haelt Bild 9 (56 x) bis
zum Wechsel; Original o3061_s2_t17/t18 cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x168 = PL04W01-Clip-6 Bild 9 (EDD+0x3e4, selbst
gelesen); Lage/Blick (-18510,-26206) 3651 gleich; Bildfolge (Drehen, Arme mit Waffe vor, Halten) = Original-Video.

## Gegenprobe — der Riegel misst den Nutzer-Befund (master-exe ohne Spur J)
Hauptbaum-exe (master-Stand 2026-10-24 19:03, ohne Spur J) als Kopie im Baum, derselbe Riegel `test_r41_stage3szenen_a.cmake`
TEIL 3060 (Aufruf per `cmake -P`, Laufordner `J41abn1/gegenprobe/`): Tuer per Quadrat ok, dann
`FAIL ... keine [wclip41]-Zeile ROOM3060 W-Bank-Clip 6 / 10 Bilder` und
`FAIL ... RE15_ANIM_TRACE ROOM3060 Marvin: 40 Bilder Motion 6, davon 40 nicht Clip 6/10 Bilder (z.B. '6:6:50:0'), 0 x Bild 9
gehalten` -> EXIT 1. Auf master spielt Marvin also den 50-Bilder-Clip 6 der PLD-Grundbank (Knien); mit Spur J W-Clip 6 /
10 Bilder gehalten. Der Riegel schlaegt beim alten Verhalten an.
Vorher-Bilder mit derselben master-exe (eigene Laeufe, Erklaerung des Nutzer-Befunds):
- Lauf m3060_master (Weg wie p1a): ANIM_TRACE F65-F104 `6/6/50` Slot 0..39 (laeuft durch), Bild `J41abn1/m3060_master_zoom.png`:
  ab F68 beugt sich Marvin vor und **kniet** (F72-F104) — der Nutzer-Befund; mit Spur J (p1a) Arm vor der Brust wie im Original.
- Lauf m3061_master (CONTINUE in 3061 + Quadrat am AOT wie der Riegel): Sherry tiefste Lage **(-19041,-26308)** — sie bleibt vor
  der Wand; Elza Motion 6 = **Clip 6 mit 50 Bildern** (65 Zeilen). Bilder `J41abn1/m3061_master_kriechen.png` (das Gitter bleibt
  in der Oeffnung, Sherry liegt davor) und `J41abn1/m3061_master_ende.png` (Elza **kniet** neben der liegenden Sherry) — beide
  Nutzer-Befunde reproduziert; mit Spur J (p2) Gitter abgelegt, Sherry bei z -30700 im Schacht, Elza W-Clip 6.
Der Fix erklaert damit die Befunde: (1)/(2b) Plc_motion(2,6) lief auf der PLD-Grundbank statt auf +0x170/+0x174 (W-Bank);
(2a) Bit 1 fuehrte den Keyframe-Wurzelweg der eigenen Bank aus (keine Bewegung), Bit 2 war "deferred", Super_set/Super_reset
fehlten (Gitter blieb stehen).

## Gate V — Vertrag (VERTRAG.md) und Orchestrator-Entscheide
- Pfade (`git diff master...HEAD --name-only`, 46 Dateien): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/,
  kein CMakeLists.txt; Riegel nur in `tests/unit/probes/r41_stage3szenen_a.cmake` / `_rbj.cmake`. **ok**
- Woerter in den `+`-Zeilen (`git diff master...HEAD -- re15_port`): deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/
  approx/vermutlich/geschaetzt -> nur zwei "deferred" als Zitat des ERSETZTEN alten Executor-Zweigs. **ok**
- Neue Env-Schalter: nur `RE15_FUSSANKER_LOG` (Messzeile, kein Verhalten). Kein Env-Schalter als Abschluss. **ok**
- Konstanten mit Beleg: jede verhaltensrelevante Zahl im Code mit @0x... (Stichproben oben selbst disassembliert) bzw. als
  PORT-WAHL gekennzeichnet (Schutzgrenzen RBJ 8 Records / 32 Gegner, "kein Vorbild-Stand -> keine Verschiebung"). **ok**
- Bank-9-Bits / Nachrichten-Ids: keine neuen (kein game_flag-/Message-Eintrag im Diff, test_r36_kartenbits.c unberuehrt). **ok**
- Gemeinsame Dateien: scd_vm.c (register_opcodes 0x48/0x49, 3 Zeilen) und platform/pc/main.c (pc_prop_world, 4 Zeilen) sind
  durch den Orchestrator-Entscheid **J-E1** angenommen (AUFTRAG.md "2026-10-24 23:10", im Zweig = master dba6ff8e, selbst
  gelesen) -> **Mangel V1 der Abnahme J 0 erledigt**. RBJ-Haken main.c (1 Zeile + include), enemy_common.c, asset_psx.c = A-E5;
  enemy_ai_common.c = NPC-Code (J); dining_r39.c 3 Zeilen + include (keine andere Runde-41-Spur). **ok**
- Riegel je Punkt vorhanden und messend: Punkt 1 `integration_..._3060` (Gegenprobe oben) + `unit_..._wclip`; Punkt 2a
  `unit_..._anker` (bytegleich gegen Original-Staende), `unit_..._superobj`, `unit_..._zensus`, `integration_..._3061`;
  Punkt 2b `integration_..._3061` + `unit_..._wclip`. Parallel-/Linux-fest: eigener Laufordner + exe-Kopie mit Zufalls-Id
  je Lauf (Kopie auch bei Abbruch entfernt), `m` auf Nicht-WIN32, `cmake_minimum_required` im -P-Skript, keine .exe-/
  Backslash-Pfade, 3060 setzt `unset(ENV{RE15_FIRE_AOT})`. **ok**
- **Mangel M2 der Abnahme J 0 erledigt**: Kopf `re15_superobj_r41.h` + Dossier §2.6/§4.1/NB1.M2 berichtigt; jede Byte-Angabe
  selbst nachgelesen (Gate M2), Handler-Aussage selbst disassembliert (Gate RE); Zensus-Riegel pinnt die Praemissen.

## Gate N — Nebenwirkungen (beide Szenarien, Nachbarraeume)
- Super_set/Super_reset-Zensus selbst (scd_dump_room.py ueber alle 240 RDTs von info/Re1.5/PSX): Super_set nur ROOM3061
  @0x0127E, ROOM4030 @0x006E0, ROOM4031 @0x00722; **Super_reset nur ROOM3061 @0x0129E** -> der neue Reset-Weg wirkt nur in
  3061; 4030/4031 bleiben beim Ueberspringen wie auf master (ausser dem unerreichbaren John-Fall, in dem der Port wie der
  typ-blinde Original-Handler anhaengt).
- Anker-Bits: Port-Schreiber von `anim_flags` selbst gesucht (grep `anim_flags |=/=/^=` in engine/ und platform/pc): nur
  0x04/0x10/0x20/0x80/0x1000 bzw. Member 16 und Plc_motion pc[3] (Zensus: in allen 1359 Aufrufen 0) -> Glied-Anker wirkt nur
  bei Plc_flg mit Bit 1/2 = ROOM3061 sub04.
- W-Bank-Regel: Spieler-Plc_motion(2,c) nur 3060 @0x010BE / 3061 @0x011DE (Zensus-Riegel); ROOM1141 bleibt auf der Regel der
  Runde 39 (`unit_..._wclip`). Nach dem Raumwechsel kein W-Clip (Lauf p1b, ROOM3020 ab F6 Stand).
- RBJ-Binder (A-E5): eigene Nachrechnung der Schleife (aus dem selbst gelesenen Disasm) fuer 6030/3060/3061/3091/1040/2000/4001
  aus den Original-RDTs = Port-Bindung (z.B. 6030 Marker 0x1/0x2/0x3 -> Spieler rec2, Gegner {0:1, 1:2}; 3060 0x1/0x2/0x4 ->
  {0:1, 2:2}; 1040 0x7e -> Spieler -1). Laufzeit-Stichprobe (Lauf rbj_6030, Leon, DEBUG_JUMP 6030): `[rbj-binder] ROOM6030
  Spieler rec2 (alt rec0) Gegner 0:rec1(alt rec1) 1:rec2(alt rec-1)`, `[rbj] room 6030 cinematic overlay: 22 clips, 762 kf`,
  `Aktor 1 Typ 0x4D Marker-Slot 1 -> rec1 gebunden` (vorher 1 Platzhalter-Clip).
  Sichtbare Stichprobe ROOM4010 (Leon, DEBUG_JUMP 4010, Laeufe rbj_4010 / rbj_4010_master, Bilder `J41abn1/rbj_4010_uebersicht.png`
  / `J41abn1/rbj_4010_master.png`): mit Spur J `[rbj-binder] ROOM4010 Aktor 2 Typ 0x40 Marker-Slot 2 -> rec2 gebunden`, Marvin
  spielt Clip 25 aus rec2 (36 Bilder, ANIM_TRACE `25/25/36`) und **liegt verwundet auf der Liege**, Ada (Typ 0x42, rec1) daneben;
  mit der master-exe spielt derselbe Aktor `25/1/32/0` (fremde Bank, Bild 0 eingefroren) und **steht riesig im Vordergrund auf
  der Liege**. Die byte-true Bindung behebt dort also einen sichtbaren Fehler; keine nachteilige Nebenwirkung gefunden.
- Beide Szenarien: Leon (3060/3020/6030) und Elza (3011 -> 3061 -> 30C1) selbst gefahren; Nachbarraeume 3020 (Rueckweg) und
  30C1 (Wechsel zu Sherry, PL0F) ohne Auffaelligkeit in den Messzeilen.
- Nebenbefund N1 (Hund am Ende der 3061-Szene) unveraendert vorhanden, nicht Spur J (Dossier §4.7, eigener Auftrag empfohlen).

## Integrations-Probe (ohne Aenderung am Baum: `git merge-tree --write-tree --name-only --messages master HEAD`, master 6449eb43)
- Konflikte: `analysis/befunde_runde41/AUFTRAG.md`, `VERTRAG.md` (Doku; master-Fassung nehmen — fuer VERTRAG vom Bau-Agenten
  vermerkt, fuer AUFTRAG neu: master hat seit dba6ff8e die Nachtraege 22-24b) und **`re15_port/engine/src/enemy_common.c`
  `rbj_resolve_slot`** (Code). Ursache: master enthaelt seit der Integration der Spur A (5705fafb/fa883caa, 2026-10-24 morgens)
  deren Haken d1809dbf `if (mslot < 32 && (rbj_record_marker(r) & (1u << mslot)))` (RE15_ACTOR_MAX 36), die Spur J ersetzt
  dieselbe Schleife durch `int rec = re15_rbj_binder_gegner(s_room_rbj, s_room_rbj_size, mslot - 1);`. Die Zweig-Basis 4c667c97
  liegt vor der Integration der Spur A (`git merge-base --is-ancestor d1809dbf 4c667c97` -> nein); der Bau-Agent hat nur andere
  Runde-41-Zweige auf register_opcodes geprueft, nicht master auf enemy_common.c.
  Aufloesung: **Seite der Spur J nehmen** — der Binder deckt die Absicht von Spur A ab (Gegner-Index >= 32 -> -1 ueber
  RE15_RBJ_BINDER_MAX_GEGNER, kein `1u << 32..35`); die master-Seite allein kompiliert nicht (`int rec = -1;` entfaellt im
  Merge-Ergebnis). Nach dem Merge `unit_r39_heliport1171_binder` (Riegel der Spur A zu RE15_ACTOR_MAX 36) und die Riegel
  `r41_stage3szenen_a` fahren. Spur K (cherry-pick 299b6deb des Binders) trifft denselben Konflikt.
- scd_vm.c, platform/pc/main.c, enemy_ai_common.c mergen ohne Konflikt. Behauptung "kein anderer Runde-41-Zweig aendert
  register_opcodes" nachgeprueft (`git diff master...r41/<zweig> -- scd_vm.c`, alle 13 anderen Zweige): Hunks nur @32/@48/@78/
  @680/@758/@759/@1820/@3865/@4560, keiner beruehrt `s_op_table[0x48]/[0x49]` — stimmt.

## Gate T — gezielte Tests (eigene Laeufe, keine volle Suite; Logs `J41abn1/ctest_*.log`)
1. Riegel der Spur: `ctest -R "^unit_r41_stage3szenen_a" -V` -> **7/7 Passed** (zensus mit den NB1-Pruefungen 4030/4031/4040/4041
   und 9/9 Gliedern, anker (0,0)/(0,0), superobj (0,0,0), wclip, rbj_mechanik, rbj_zensus, rbj_kanal);
   `ctest -R "^integration_r41_stage3szenen_a_" -j 2` -> **2/2 Passed** (3061 60,1 s, 3060 114,1 s per Quadrat).
2. Unit-/Probe-Ebene der beruehrten Module (NPC-Executor, RBJ-Bindung, SCD-Opcodes, Zeichner-/Raum-Module, Szenen-Riegel der
   Runden 33-39; Regex `J41abn1/unit_regex.txt`): `ctest -R "<regex>" -j 4` -> **227/227 Passed** (22,8 s).
3. Integrationsriegel der beruehrten Raeume/Module (dining1141-Haken, RBJ-Raeume 3091/1040/1041/10F0, NPC-Szenen 1150/11C0/Roy,
   Elza-Kette, STAGE3-Masken, Tuerweg; Regex `J41abn1/integ_regex.txt`, 31 Riegel, `-j 3`, Log `J41abn1/ctest_integ_modul.log`):
   Stand beim Schreiben 9/9 Passed (story_elza, szenenleon, masken36_stage3, cut10f0, dining1141_s, r39_dining1141_d/_kampf/
   _zielen/_zielen_nachladen), Rest laeuft (Ergebnis s. Schluss).
4. Gegenprobe (master-exe, s. oben): Riegel 3060 **rot** (2 Fehler) — er misst den Nutzer-Befund.

## Gesamturteil (vorlaeufig, wird am Ende ersetzt)
| Punkt | Urteil |
|---|---|
| 1 ROOM3060 Marvin | erfuellt |
| 2a ROOM3061 Sherry kriecht | erfuellt |
| 2b ROOM3061 Elzas Ende | erfuellt |
Gates: RE haelt, V haelt (V1 durch J-E1, M2 berichtigt und belegt), N ohne erreichbare Nebenwirkung, T offen bis zum Ende des
Integrationslaufs.
