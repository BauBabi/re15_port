# Runde 41 — Spur J "stage3szenen_a": Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r41_stage3szenen_a`, Zweig `r41/stage3szenen_a`, Kopf ed8b1457, Merge-Basis master 4c667c97.
Pruefer: Abnahme 0, 2026-10-24 (abends). Arbeitsordner (Laeufe, Logs, Bilder): Scratchpad `J41abn0/`
(Sitzung c41eae99). Nichts am Code geaendert; nur diese Datei committet.

## Punkte (Wortlaut AUFTRAG.md, Zeilen 83-85)
1. "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck gewechselt wird zu Leon ist falsch in ROOM 3060."
2. "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht nicht. Am Ende von
   dieser Cutscene hat Elza auch erneut eine falsche Animation."
0. (Orchestrator-Entscheid A-E5) RBJ-Binder FUN_8001b3f8 byte-true als eigener Commit `rbj-binder`.

## Stand der Pruefung (fortlaufend)
- [x] Diff gelesen (git diff master...HEAD, 45 Dateien; Code-Diff vollstaendig gelesen, s. Gate V)
- [x] Bau (local_build.sh configure + build: LOCAL-BUILD-OK, "ninja: no work to do" = exe 20:31 entspricht HEAD ed8b1457;
      Baum sauber, `git status` leer)
- [x] Riegel der Spur (9/9) + alle unit_/probe_/prop_ + 54 Integrationsriegel der beruehrten Raeume/Module (Gate T)
- [x] Punkt 1 gemessen (echter Tuerweg 3040 -> 3060, Framedump, Original-Video + Savestates selbst gelesen)
- [x] Punkt 2a gemessen (CONTINUE-Weg UND echter Tuerweg 3011 -> 3061 per Quadrat, Framedump, Original selbst gelesen)
- [x] Punkt 2b gemessen (dto.)
- [x] RE-Gate (11 Adressbereiche selbst disassembliert, alle stimmen)
- [x] Vertrag (Mangel V1 formal) / Nebenwirkungen (keine erreichbare; Mangel M2 = falsche Doku-Aussage zu ROOM4031; Nebenbefund N1 Hund)

## Gate RE — selbst disassembliert (info/Re1.5/PSX.EXE, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`)
Alle zitierten Stellen enthalten das Behauptete (Auszug, Bytes/Instruktionen aus dem eigenen Lauf):
| Stelle | eigene Disasm | Behauptung im Code/Dossier |
|---|---|---|
| Plc_motion 0x80041b90 | `80041b98 lbu a2,1(v0)`, `80041ba8 sb a1,148(v0)`, `80041bb0 sb v1(=4),4(v0)`, `80041bc4 sb a2,5(v0)`, `80041bc8 sh a1,452(v0)` (a1 = pc[2..3]>>8) | +0x94 Clip, +0x4 = 4, **+0x5 = pc[1]**, +0x1c4 = pc[3] — stimmt |
| Tabelle 0x80073f90 / 0x80073e30 | [4] = 0x80030660; 0x80073e30: [0] 0x80050cb8, [1] 0x80050ddc, **[2] 0x80050f00**, [3] 0x80051024 | stimmt |
| Spieler-Executor 0x80030660 | `8003068c lbu v0,-13735(v0)` (0x800aca59 = +0x5), `8003069c addiu at,at,15920` (0x80073e30), `800306cc andi v0,v0,0x1` -> `800306d8 jal 0x800369f8` (a1 = 0), `800306e8 andi 0x2` -> `800306f4 jal 0x800369f8` (`ori a1,zero,0x1`) | stimmt (auch §4.4 OFFEN: Spieler hat dieselben Anker-Bits) |
| Sub 2 0x80050f00 | `80050f34 sb v0(=1),6(a0)`, `80050f44 sb v0(=7),143(v1)`, `80050f54 sb zero,149(v0)`, `+0x1c4&0x40 -> +0x8f = 0`, **`80050f88 lw a0,368(v0)` / `80050f90 lw a1,372(v0)` / `80050f94 jal 0x8001f314`**, Rueckgabe != 0 -> **`80050fec sb v0(=2),6(v1)`**, `80051004 andi 0x4` -> `80051010 +0x6 = 1` | Kanal +0x170/+0x174, Halten Phase 2, Schleife nur mit Bit 4 — stimmt |
| Waffen-Lader 0x80036b68 | `80036b84 lbu 0x800aca5c`, `80036bac 0x800741e8`, `80036bc4 addu a0,v0,a0` (+ Waffe 0x800aca5d), **`80036be4 sw v0,-13368(at)` = 0x800acbc8 (+0x174)**, **`80036c04 sw v0,-13372(at)` = 0x800acbc4 (+0x170)**; Tabelle 0x800741e8 = [76,76,76,76,97,97,97,97,76,76,76,76,76,76,76,118] | Modell 0x0D -> 76 (PL00Wxx), Modell 4 -> 97 (PL04Wxx) — stimmt |
| NPC-Executor 0x80050be8 | Sub-Tabelle `80050c24 addiu at,at,27808` = 0x80076ca0 (= dieselben 4 Subs), `80050c50 xori 0x20`, **`80050c6c andi 0x1` -> `80050c78 jal 0x800369f8` (a1 = 0)**, **`80050c94 andi 0x2` -> `80050ca0 jal 0x800369f8` (`80050ca4 ori a1,zero,0x1`)** | stimmt |
| FUN_800369f8 | `80036a30 lw s3,392(v0)` (+0x188), `80036a34 jal 0x80068098` (RotMatrix +0x68 -> +0x20), `80036a48/58 jal 0x80022da0` (s3+24, s3+196), `80036a60..70` s3 += a1*516 + 688, `80036a78/88/98` s3-320 / s3-148 / s3+24, Modus 0: **`80036ab4..ac8` x(+0x34) -= sp+36 - Part+84**, **`80036ad4..aec` z(+0x3c) -= sp+44 - Part+92** | feste Kette 0-1-2-3-4 / 0-1-5-6-7, x/z -= Differenz — stimmt |
| Super_set 0x80040b20 / Super_reset 0x80040cd0 | Art `andi v1,a1,0xc0`; 0x80 + Part-Bit: `80040c18 addiu at,at,-11340` (0x8009d3b4) + pc[2]*500 (= Gegner+0x188 fuer pc[2]-0x80), Part*172 + 64; `80040c74 sw v0,116(a2)`; `80040c78..cbc` lokal pc+4/6/8, Rot pc+10/12/14; Reset `80040cd8..ce0` +0x5c/60/64 -> `80040cf0..cf8` +0x34/38/3c, `80040cec` Vater 0x80072d4c | stimmt |
| RBJ-Binder 0x8001b3f8 | `8001b404 lw a2,92(v0)`, `8001b410 addu a3,zero,zero` (einmal), `8001b440 andi 0x1` -> `8001b450/460` Spieler-Kanal (jeder Bit-0-Record, letzter gewinnt), `8001b4a0/4bc` Gegner-Kanal 0x800acdac/b0 + (a3&0xff)*500, **`8001b4c4 addiu a3,a3,1` im Delay-Slot je Bitposition**; Aufrufer `80039a08 jal 0x8001b3f8` | stimmt; 3060-Rechnung Marker 1/2/4 -> Gegner {0:1, 2:2} nachgerechnet |
| Bild-Zaehler 0x8001f314/0x8001f3bc | `8001f36c sw a2,360(t0)` (+0x168 = Eintrag Bild +0x95), `8001f5f0 jal 0x80068098` (lokale Part-Matrizen), **`8001f618 addiu v0,v0,1` / `8001f61c sb v0,149(v1)`**, `8001f63c sb zero` + Rueckgabe 1 | "+0x95 = gezeichnetes Bild + 1" — stimmt |
| Plc_flg 0x80041fb8 | `80041fc8 beq a1(pc[1]),1 -> 80042008 sh a2,452(v0)` (SET), pc[1] = 0 -> `80041ffc or`, 2 -> `8004201c xor` | stimmt |

Ergebnis: alle 11 geprueften Adressbereiche stimmen (richtige Datei PSX.EXE, Sprungziele/Tabellen nachgeschlagen).
Suche im Diff (`git diff master...HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausibel/
TODO/FIXME/approx/vermutlich: nur zwei Treffer "deferred" = Zitat des ERSETZTEN alten Zweigs. Einziger neuer Env-Schalter
`RE15_FUSSANKER_LOG` (nur Messzeile, kein Verhalten). Konstanten: jede mit @0x... bzw. als PORT-WAHL gekennzeichnet
(RE15_RBJ_BINDER_MAX_REC 8 / _MAX_GEGNER 32 = Schutzgrenzen; "kein Vorbild-Stand -> keine Verschiebung"). Der Fix erklaert
die gemessenen Befunde: Knien = PLD-Grundbank-Clip 6 statt W-Bank-Clip 6 (Punkte 1/2b); Liegenbleiben = Bit 1 fuehrte den
Keyframe-Wurzelweg der eigenen Bank aus (No-op), Bit 2 war "deferred", Super_set/Super_reset fehlten (Punkt 2a).

### Original-Daten selbst gelesen (Savestates des Bau-Agenten, eigenes Leseskript `J41abn0/py/*.py` auf re15_ss.Ram)
- MZD-Disc = Auslieferungs-EXE: in o3060_s2_t03 / o3061_s2_t17 / o3061_s3_t08 sind die RAM-Bytes an 0x80026e4c (Stub `08 00 e0 03`,
  kein Patch-Build), 0x80041b90, 0x80050f00, 0x800369f8, 0x80040b20, 0x8001b3f8, 0x80030660, 0x80036b68, 0x80050be8,
  0x8001f314 **gleich** PSX.EXE. ROOM3060/3061-SCD (sub02-Ende, sub03, sub04) im RAM gleich den Dateien in shared_assets
  (Unterschiede nur in relokierten Zeigern und den zur Laufzeit beschriebenen AOT-Datensaetzen).
- o3060_s2_t03 (6,5 s): Raum 3060, Modell 0x0D, Waffe 1, Spieler **cmd 4 / Sub 2 / Phase 2 / Clip 6 / +0x95 0**, +0x168 0x801d7a10 =
  +0x174 (0x801d7708) + 0x308 = PL00W01-EDD Clip 6 letztes Bild (eigene PLW-Lesung: Clip 6 = 10 Bilder ab EDD+0x2e4). t04 (8,0 s):
  ROOM3020, Modell 0, cmd 0 / Sub 0.
- o3061_s2_t17 (38 s): Modell 4, Waffe 1, Spieler **cmd 4 / Sub 2 / Phase 2 / Clip 6**, +0x168 = 0x801d7708 + 0x3e4 = PL04W01 Clip 6
  letztes Bild; John (Gegner 1) Sub 2 Phase 2 Clip 6; Sherry (Gegner 0) (-18795,-30700). t19: ROOM30C1, Modell 0x0F, Waffe 0.
- o3061_s3: t08 Clip 2 +0x95 19 (-18780,-28017), t09 +0x95 44 (-18791,-28390), t10 +0x95 12 (-18783,-28806), t11 +0x95 39
  (-18810,-29279), t12 (-18795,-30700); Gitter t03 lokal (252,192,-153) Rot (0,1024,3072) Vater 0x8014046c, t04 lokal (-18276,..)
  Rot (72,3072,0) Vater 0x80072d4c, ab t05 (-18116,-266,-26788). = die Sollwerte in den Riegeln (keine erfundenen Zahlen).

## Punkt 1 — ROOM3060, Marvins Animation am Ende seines Abschnitts (Leon-Szenario)

### Messung (eigene Laeufe, eigene exe-Kopie `re15_pc_abnJ0_*.exe`, Laufordner `J41abn0/laeufe/`)
**Lauf p1a_3060_tuer** — wie der Riegel, aber die Tuer ROOM3040 -> ROOM3060 per **echtem Quadrat-Druck** statt RE15_FIRE_AOT:
`RE15_SET_FLAG=4:199,4:196 RE15_GOTO_ROOM=3020 RE15_FORCE_EVENT=2@150 RE15_DEBUG_JUMP=3040@1150
RE15_PLAYER_POS=-8900,-26300,1024,0` (= Ankunftspunkt der 3060->3040-Tuer, ROOM3060 @0x00ED6, Blick nach -z auf die Tuer),
Quadrat alle 1,5 s (`A0.1,W1.4` x 70), RE15_ANIM_TRACE, RE15_STATE_LOG.
debug.log (Auszug):
```
[force-event] scd_event_fire(2) at F150                      (ROOM3020 sub02 = Szene, Spieler -> Marvin)
[pld] Spielermodell -> PLD/PL0D.PLD
[debug-menu] JUMP -> 304 FACTORY LINE A (ROOM3040) spawn=(-8900,0,-26300) cut=0
[aot] DOOR FIRE slot=0 rect=(-8800,-26950,hw=1850,hh=350) target_cut=4 spawn=(2063,0,-17192)   <- Quadrat an der Tuer (F39)
[room] PC loaded room3060.rdt
[wclip41] ROOM3060 F65 Spieler Plc_motion(2,6) -> W-Bank-Clip 6 (0x80073e30[2] = 0x80050f00, Kanal +0x170/+0x174, Waffe 1, 10 Bilder)
[aot] DOOR FIRE slot=3 ... spawn=(-11250,0,-13250)            (@0x010E4 Aot_on 3)
[room] PC loaded room3020.rdt
[pld] Spielermodell -> PLD/PL00.PLD
```
RE15_ANIM_TRACE Spieler im ROOM3060-Block (Bild Motion Clip Bildzahl Slot): F25-27 `105 5 30` (Drehbeginn), F28-43 `1 1 16`
(W-Clip 1), F44-64 `2 2 52` (W-Clip 2), **F65-74 `6 6 10` Slot 0..9 je einmal, F75-F104 `6 6 10` Slot 9 (30 Bilder; zusammen 31 x
Bild 9)** = Phase 2 Halten (@0x80050fec) bis zur Tuer. Kein Bild mit PL00-Clip 6 (50 Bilder = Knien).
Erster Block ROOM3020 danach: F0-F5 `210 1 16`, F6.. `200 3 1` (Leon-Stand) — **kein W-Clip-6-Bild nach dem Wechsel**; Original
o3060_s2_t04 dort cmd 0 / Sub 0, Modell 0 (selbst gelesen) — gleich.
Original (selbst gelesen, s. oben): o3060_s2_t03 Spieler cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x168 = PL00W01-Clip-6 Bild 9.

Riegel `integration_r41_stage3szenen_a_3060` (eigener Lauf): Passed 214,6 s — `40 Bilder Motion 6, davon 0 nicht Clip 6/10 Bilder,
31 x Bild 9 gehalten`.

**Lauf p1b_3060_bilder** (gleicher Weg, echte Tuer, RE15_FRAMEDUMP 20-100/2, RE15_EXIT_AT=100#3060): Bildbogen
`J41abn0/p1b_3060_marvin_zoom.png` (Ausschnitt 62,85-112,165, 3-fach): F56-F68 Marvin steht, F70-F74 hebt er den Arm vor die
Brust, F76-F100 haelt er diese Haltung. Original-Video selbst ausgeschnitten (`J41b/orig/o3060_v4.mp4`, ffmpeg fps=10,
`J41abn0/o3060v4_marvin_zoom.png`, Ausschnitt 124,170-224,330): o21-o32 steht, o33 Arm vor die Brust, o33-o43 gehalten, o44
Abblende. **Gleicher Bewegungsablauf, gleiche Endhaltung; kein Knien.** Zusammengefasst (obere Zeile Port, untere Original):
`J41abn0/vergleich_p1_marvin.png`.

**Lauf p1c_3060_waffe3** (gleicher Tuerweg, zusaetzlich `RE15_EQUIP=3` = Handfeuerwaffe, weil der Nutzer nicht zwingend das
Messer traegt; ROOM3020 sub02 setzt keine Waffe, die W-Bank folgt im Original der ausgeruesteten Waffe @0x80036bc4):
`[equip] W-bank -> W03`, `[wclip41] ROOM3060 F65 ... Waffe 3, 10 Bilder`, ANIM_TRACE `6 6 10` Slot 0..8 je 1x, Slot 9 27x bis F100;
Bild `J41abn0/p1c_3060_waffe3_zoom.png`: Marvin hebt die Pistole vor die Brust und haelt — auch mit anderer Waffe kein Knien.

### Urteil Punkt 1: **erfuellt**
Beleg: Ursache (Spieler-Plc_motion(2,6) lief auf der PLD-Grundbank statt +0x170/+0x174) und Mechanismus (Sub 2 0x80050f00,
W-Bank-Lader 0x80036b68/Tabelle 0x800741e8, Halten @0x80050fec) selbst disassembliert; Port spielt am echten Tuerweg W-Clip 6
(10 Bilder) und haelt Bild 9 bis zum Wechsel, erstes ROOM3020-Bild ohne W-Clip; Bild = Original-Video.

## Punkt 2 — ROOM3061 (Elza): (a) Sherry kriecht in den Schacht, (b) Elzas Animation am Ende der Szene

### Messung (eigene Laeufe)
**Riegel `integration_r41_stage3szenen_a_3061`** (eigener ctest-Lauf, Passed 93,9 s, Laufordner
`re15_port/build/r41_stage3szenen_a_wd_3061_18a3b040/lauf`): Spielstand Elza (-3500,-25500) Blick 1024 + Bits 4:244/3:160
(probe_r36_elza_karte), CONTINUE, ab Spielbild 420 vor an die Wand und **Quadrat** (kein RE15_FIRE_AOT):
`[scd F497] Cut_chg(6)` (sub02 laeuft), `[superobj41] F1049 Super_set Objekt 0 Art 0x80 Teil-Byte 0x8E lokal (252,192,-153)
Rot (0,1024,3072) -> angehaengt`, `F1075 Super_reset ... -> lokal (-18753,-266,-26778) Rot (72,3072,0)` (Original-Loslass-Lage
(-18756,-266,-26788)), Anker-Takte `0/1x22 1/1x22 0/1x11 1/2x26 0/2x24 1/2x26 0/2x24` (= Plc_flg-Folge sub04), Sherry-Lagen je
(3,10) neben dem Original, Sherry-Endlage (-18792,-30700) (Original (-18795,-30700)), `[wclip41] ROOM3061 F1456 Spieler
Plc_motion(2,6) -> W-Bank-Clip 6 (... Waffe 1, 10 Bilder)`, 56 x Bild 9, Wechsel ROOM30C1 als PL0F.

**Lauf p2a_3061_bilder** (gleicher Weg + RE15_FRAMEDUMP 1000-1520/4, RE15_STATE_LOG, RE15_ANIM_TRACE, RE15_FUSSANKER_LOG):
dieselben Zeilen (Super_set F1049, Super_reset F1075 (-18753,-266,-26778), 155 Anker-Zeilen mit derselben Takt-Folge,
state.txt Sherry F1200 (-18781,-28058) ... F1299 (-18792,-29716) -> F1300 (-18792,-30700), danach unveraendert).
Elza (state.txt): F1400 (-18510,-26206) rot 1939, F1520 rot 3651 — **Original o3061_s2 t16/t17 (selbst gelesen): (-18510,-26206)
rot 1939 / 3651**. RE15_ANIM_TRACE Elza: F1438-F1455 `105 5 30` (Drehen, Plc_dest 9), **F1456-F1465 `6 6 10` Slot 0..9,
F1465-F1520 Slot 9 (56 x)** bis zum Tuerwechsel; ROOM30C1 F0/F1 `210 1 16`.

Bilder (Port, 320x240 Framedump) gegen das selbst ausgeschnittene Original-Video `J41b/orig/o3061_v3.mp4` (ffmpeg ab 10 s, 5 B./s):
- Kriechen: `J41abn0/p2a_3061_kriechen.png` / `J41abn0/o3061v3_kriechen.png`: Sherry kniet am Gitter (Port F1040 / Orig o57),
  zieht es mit der Hand heraus (F1048-F1064 / o59-o62), legt es links ab (F1072-F1112 / o63-o70), geht auf alle Viere und kriecht
  hinein (F1120-F1136 / o70-o73), nur noch die Beine schauen heraus, Elzas ausgestreckter Arm ("Sherry, wait!") oben rechts im
  Bild (F1140-F1172 / o74-o78), dann Elza im Vordergrund (F1176+ / o79+). Ablauf und Gitter-Ablage gleich.
- Elzas Ende: `J41abn0/p2a_3061_ende_zoom.png` / `J41abn0/o3061v3_ende_zoom.png` / `J41abn0/vergleich_3061_ende_voll.png`:
  Elza steht (F1396-F1436 / o117-o124), dreht sich (F1440-F1452 / o125-o127), hebt die Arme mit der Waffe nach vorn
  (F1456-F1465 / o128) und haelt (F1468-F1516 / o129-o137); John ebenso. Kein Knien mehr (vorher: J_belege/e3061_vor_ende_elza_kniet.png).
  Zusammengefasst (oben Port F1436-F1516, unten Original o124-o137): `J41abn0/vergleich_p2b_elza.png`.

**Lauf d3_3011_tuer_szene — der echte Weg durch die Tuer** (statt CONTINUE im Raum): Spielstand Elza in **ROOM3011** vor der Tuer
Slot 1 (ROOM3011 @0x01FF2, Ziel ROOM3060/3061) bei (-7578,10065) Blick 0, Bits 4:244/3:160; Eingaben ab Spielbild 30
(`RE15_INPUT_SCRIPT_BASIS=spiel`): Quadrat an der Tuer, 14 s warten (Einlauf sub09/sub10), 3,8 s rennen nach +x, 0,35 s rechts
drehen (Blick ~1072 = nach -z), 1,2 s rennen bis an die Wand, dann Quadrat alle 1,5 s.
```
[aot] DOOR FIRE slot=1 rect=(-6563,9810,hw=705,hh=900) target_cut=0 spawn=(-26027,0,-22532)   (ROOM3011 -> ROOM3061)
[scd F703] Cut_chg(6)                                                                         (Quadrat am Slot-1-AOT -> sub02)
[superobj41] F1255 Super_set Objekt 0 Art 0x80 Teil-Byte 0x8E lokal (252,192,-153) Rot (0,1024,3072) -> angehaengt
[superobj41] F1281 Super_reset Objekt 0 Welt-Lage uebernommen -> lokal (-18753,-266,-26778) Rot (72,3072,0)
[wclip41] ROOM3061 F1662 Spieler Plc_motion(2,6) -> W-Bank-Clip 6 (... Waffe 1, 10 Bilder)
[aot] DOOR FIRE slot=3 ... spawn=(-19100,0,-1350) / [pld] Spielermodell -> PLD/PL0F.PLD
```
Anker-Takte `0/1x22 1/1x22 0/1x11 1/2x26 0/2x24 1/2x26 0/2x24`, Sherry tiefste Lage (-18792,-30700), Elza letzte ROOM3061-Zeile
(-18510,-26206) rot 3651, ANIM_TRACE Elza `6 6 10` Slot 0..8 je 1x, **Slot 9 56x**. Ergebnis identisch mit dem CONTINUE-Weg.

### Urteil Punkt 2a (Sherry kriecht in den Schacht): **erfuellt**
Beleg: Glied-Anker FUN_800369f8 und Super_set/Super_reset selbst disassembliert (s. Gate RE); Riegel `unit_r41_stage3szenen_a_anker`
trifft die selbst gelesenen Original-Staende o3061_s3 t08->t10 / t09->t11 **bytegleich** (Abweichung (0,0)); Gitter-Loslass-Lage
im Riegel bytegleich (-18756,-266,-26788), im Spiellauf (-18753,-266,-26778) (Rest (3,10) aus Sherrys Lage vor dem Kriechen,
NPC-Gang, s. Dossier §4.3); Sherry erreicht (-18792,-30700) (Original (-18795,-30700)); Bildfolge Kriechen = Original-Video.

### Urteil Punkt 2b (Elzas Animation am Ende der Szene): **erfuellt**
Beleg: Plc_motion(2,6) @0x011DE spielt W-Bank PL04W01 Clip 6 (Tabelle @0x800741e8 Modell 4 -> 97), haelt Bild 9 bis zum Wechsel;
Original o3061_s2_t17 cmd 4 / Sub 2 / Phase 2 / Clip 6, +0x168 = PL04W01-Clip-6 letztes Bild; Lage/Blick (-18510,-26206) 3651 gleich;
Bildfolge (Drehen, Arme mit Waffe vor, Halten) = Original-Video; kein Knien mehr.

## Gate V — Vertrag (VERTRAG.md)
- Pfade (`git diff master...HEAD --name-only`, 45 Dateien): keine release/, platform/android/, shared_assets/PSX/, pri/, synchro/,
  keine Test-CMakeLists (Riegel nur in `tests/unit/probes/r41_stage3szenen_a.cmake` / `_rbj.cmake`). **ok**
- Bank-9-Bits / Nachrichten-Ids: keine neuen (kein Eintrag in test_r36_kartenbits.c, kein game_flag/Message im Diff). **ok**
- Gemeinsame Dateien (VERTRAG 1.3): enemy_ai_common.c = "NPC-Code (J, K)" (2 Zeilen Executor + 1 Zeile Vorbild-Stand + include,
  alter 20-Zeilen-Zweig ersetzt) **ok**; enemy_common.c / platform/psx/src/asset_psx.c (RBJ-Binder) = Auftrag A-E5 **ok**;
  dining_r39.c (3 Zeilen + include) nicht in der Liste, keine andere Runde-41-Spur **ok**.
  **ABER: scd_vm.c (register_opcodes 0x48/0x49, 3 Zeilen) ist laut 1.3 nur C, F, G zugeteilt; platform/pc/main.c
  (pc_prop_world 4 Zeilen) nur D und F/G** — klein, markiert "Runde 41 Spur J", technisch ohne Alternative (Opcode-Tabelle,
  PC-Zeichner), aber ohne Orchestrator-Entscheid (Vorbild B5 / A-E3 / E-E1). Der RBJ-Spieler-Haken in main.c (1 Zeile +
  include) ist durch A-E5 gedeckt. -> **Mangel V1 (formal)**.
- Doku-Stand im Zweig: AUFTRAG.md/VERTRAG.md im Stand Nachtrag 11 (Commit 1d19b24b); master ist weiter (Nachtrag 16+). Bei der
  Integration die master-Fassung nehmen (vom Bau-Agenten selbst vermerkt) — kein Mangel der Spur.
- Riegel je Punkt vorhanden und messend: Punkt 1 `integration_r41_stage3szenen_a_3060` + `unit_..._wclip`; Punkt 2a
  `unit_..._anker` (bytegleich gegen Original-Staende), `unit_..._superobj`, `unit_..._zensus`, `integration_..._3061`
  (Gitter, Anker-Takte, Lagen, Schacht); Punkt 2b `integration_..._3061` (W-Clip 6, >= 40 x Bild 9) + `unit_..._wclip`.
  Alle Riegel schlagen bei altem Verhalten an (Clip 6 mit 50 Bildern / Sherry z > -29500 / fehlende Messzeilen).
  Parallel-/Linux-fest: eigener Laufordner + exe-Kopie je Lauf (Zufalls-Id), Kopie auch bei Abbruch entfernt (ed8b1457),
  `m` auf Nicht-WIN32, `cmake_minimum_required` im -P-Skript, keine .exe-/Backslash-Pfade. **ok**
  Kleinigkeit: der Kopf von `probes/r41_stage3szenen_a.cmake` nennt fuer `_anker` noch "Abweichung <= 40" und fuer `_superobj`
  "~ Original", die Riegel pruefen inzwischen strenger auf 0 (Kommentar veraltet, kein Mangel).

## Gate N — Nebenwirkungen (beide Szenarien, Nachbarraeume)
- **Vorher-Stand selbst nachgemessen** (master-exe des Hauptbaums 19:03 = master ohne Spur J, als Kopie im Baum gestartet, Lauf
  `J41abn0/laeufe/m3061_master`, gleicher Weg wie p2a): Sherry kommt nie tiefer als z -26308 (bleibt vor der Wand), Elza ab F1460
  Motion 6 (PLD-Clip 6 = Knien) — die Nutzer-Befunde sind auf master reproduziert, mit Spur J behoben.
- Spieler-Plc_motion(2,c) gibt es nur in 3060/3061 (Zensus-Riegel), Plc_flg-Bit 1/2 nur in 3061 sub04, Port-Code setzt +0x1c4 nie mit
  Bit 1/2 (eigene Suche `anim_flags |=/=` im engine/: nur 0x10/0x80/0x1000/Member-16-Pfade) -> Executor-Wechsel und W-Bank-Regel
  wirken ausserhalb 3060/3061 nicht. ROOM1141 (Runde 39) bleibt auf seiner eigenen Regel (Riegel `_wclip`).
- RBJ-Binder (A-E5): 12 Raeume aendern die Bindung byte-true (Zensus-Riegel gegen eigene Nachrechnung der Schleife @0x8001b3f8).
  Commit e4a9f39c (erste Zeile mit `rbj-binder`) ist in sich geschlossen und seitdem unveraendert (`git diff e4a9f39c HEAD` auf
  seinen Dateien leer); Spur K hat ihn als 299b6deb uebernommen und dort 3091 im Original per GDB gegengemessen ("Spieler+Elza-NPC
  rec2, John rec1 ... = Port nach rbj-binder", K-Commit b7764be3 — Messung der Spur K, hier nur gelesen).
  Eigene Stichprobe zur Laufzeit (Lauf `J41abn0/laeufe/rbj_4001`, Elza ueber PSELECT, RE15_DEBUG_JUMP=4000@250 -> ROOM4001):
  `[rbj-binder] ROOM4001 Spieler rec2 (alt rec0) Gegner 0:rec1(alt rec1) 1:rec2(alt rec-1)`, `[rbj] room 4001 cinematic overlay:
  14 clips, 329 kf`, `Aktor 1 Typ 0x49 Marker-Slot 1 -> rec1 gebunden`, `Aktor 2 Typ 0x4B Marker-Slot 2 -> rec2 gebunden` — wie die
  eigene Nachrechnung der Schleife (Marker 1/2/3: Spieler = letzter Record mit Bit 0, a3 kumulativ).
- **Super_set in ROOM4031 (Elza): anders als im Dossier behauptet — aber im Spiel nicht erreichbar.** Selbst gemessen, Lauf
  `J41abn0/laeufe/e4031_super` (Elza ueber PSELECT, RE15_DEBUG_JUMP=4030@250 -> ROOM4031, Kunstgriff RE15_FORCE_EVENT=6@600 =
  sub06 -> sub07 `48 00 80 81` @0x00722): `[superobj41] F621 Super_set Objekt 0 Art 0x80 Teil-Byte 0x81 lokal (0,0,0) Rot (0,0,0)
  -> angehaengt`. Grund: beim ersten Besuch (Bit 3:88 = 0, main00 @0x00552/@0x00556) ist Gegner 0 **John (Typ 0x49)** und bleibt
  nach seiner Szene sub08 in State 4 (state.txt bis F900 `[1 t=49 st=4 ...]`); der Typ 0x2D ist dann Gegner 1. Das Dossier (§4.1,
  Bau-Bericht "einziger Fall ROOM4030/4031 sub07, Typ 0x2D in State 1: ... wie vor dieser Spur uebersprungen") und der Kopf von
  `re15_superobj_r41.h` stimmen fuer 4031 also nicht.
  Erreichbarkeit selbst geprueft (SCD aller 240 RDTs + Tuerkette): sub07 kommt nur aus sub06 (einziger Aufruf `Gosub 18 06`
  @0x0068E in sub02), sub02 nur ueber den Aufzugs-AOT Slot 5, den main00 nur bei Bit 3:87 = 1 setzt (@0x004FA/@0x00516).
  3:87 setzt nur ROOM4040/4041 @0x01020; 4040/4041 erreicht man nur ueber 4030/4031 Slot 1 oder ueber 4050/4070, 4070 nur aus
  4040 oder STAGE5, STAGE5 nur ueber 4020/4021, und deren Tuer in 4000/4001 (Slot 0) ist nur bei Bit 3:78 eine Tuer
  (@0x01046); 3:78 setzt nur sub06 von 4030/4031 (@0x0068C/@0x006CE) — der Aufzug selbst. Der erste 4031-Besuch (mit John) liegt
  also immer vor 3:87; jeder spaetere Besuch hat 3:88 = 1 (sub08 @0x0075C) und damit Gegner 0 = Typ 0x2D in State 1 -> wie vorher
  uebersprungen. **Keine im Spiel erreichbare Nebenwirkung**, aber eine falsche Aussage im Dossier/Kopf -> **Mangel M2 (Doku)**.
### Nebenbefund N1 (kein Spur-J-Punkt, nicht von Spur J verursacht): der Hund am Ende der 3061-Szene
Original (o3061_s2 selbst gelesen, Gegner 2 Typ 0x20): t16 36 s cmd 1 / Sub 12 bei (-13350,-22000); t17 38 s cmd 1 / Sub 2 / Clip 0
(Gehen) (-12932,-22482) rot 650; t18 40 s (-12578,-23421) rot 830 — er geht langsam nach rechts vorn (zur Kamera), nicht zu Elza
(Video o125-o137). Port (p2a und gleich auf der master-exe m3061_master): F1430 Sub 12, ab F1460 rot 1747, Motion 2, ~200
Einheiten/Bild direkt zu Elza, ab F1490 an ihren Fuessen (Abstand 949) — sichtbar anders als das Original, noch vor der Abblende.
Hund-KI (Typ 0x20) ist von Spur J unberuehrt (master-exe zeigt dasselbe). Empfehlung: eigener Auftrag (Hund-KI Sub 2 nach dem
Aufstehen, Ziel/Tempo gegen o3061_s2 t17/t18).

## Gate T — gezielte Tests (eigene Laeufe, keine volle Suite)
Logs unter Scratchpad `J41abn0/ctest_*.log`.
1. Riegel der Spur: `ctest -R "^unit_r41_stage3szenen_a" -V` -> **7/7 Passed** (zensus, anker (0,0)/(0,0), superobj (0,0,0),
   wclip, rbj_mechanik, rbj_zensus, rbj_kanal); `ctest -R "^integration_r41_stage3szenen_a_"` -> **2/2 Passed** (3060 214,6 s,
   3061 93,9 s).
2. Alle unit_/probe_/prop_ (Diff beruehrt NPC-Executor, RBJ-Bindung, SCD-Opcodes, Zeichner, Raumwechsel — breit genug, um die
   ganze Unit-Ebene zu fahren): `ctest -R "^(unit|probe|prop)_" -j 4` -> **917/923 Passed**, 6 x Timeout, alle sechs sind
   Skript-Riegel von release/ (unit_r35_android_pruefkette, _bash_mutanten, unit_r38_releaseskript_trocken, _ablauf,
   _fehler_bau, _fehler_umfeld): ihre Pruefzeilen standen alle auf ok, der Prozess wurde unter der Parallel-Last nicht fertig;
   release/ ist vom Diff unberuehrt. Einzeln nachgefahren: `unit_r38_releaseskript_trocken` **Passed (788,7 s)**; die uebrigen
   fuenf laufen einzeln nach (Stand am Ende dieses Berichts).
3. Integrationsriegel der beruehrten Raeume/Module (NPC-Szenen mit State-4-Executor, RBJ-Raeume inkl. 1041/3091, dining_r39-Haken
   1141, Tuer-/Raumwechselweg in main.c, STAGE3-Masken, Elza-Ketten), Regex in `J41abn0/integ_regex.txt`:
   `ctest -R "<regex>" -j 3` -> **52/54 Passed**; rot unter Last: `integration_elza_vollstart` (Zeitschranke 100 s Wanduhr, Szene
   lief noch: letzte Zeile F737 `[knee] ... cut=6`) und `integration_r35_cut1150` (Zeitablauf 900 s mitten in Lauf C, normal
   ~550-615 s). Einzeln nachgefahren: `integration_elza_vollstart` **Passed (113,7 s)**; `integration_r35_cut1150` siehe Stand am
   Ende. Gruen u.a.: r39_dining1141_* (4), r38_dining1141_* (7), r37_elzaems_elza3091/3071, r37_szene11c0, r36_szene1150,
   r35_cut10f0, r36_szenenleon, r36_story_elza, r36_elza_* (5), r37_roy_* (3), r37_masken36_stage3, r38_elzatoene_* (2),
   r38/r39_heliport1171_* (5), r38_szenenschutz, r39_elza_karte, r39_johnruf1041_* (8), r39_parkplatz11c0_* (5), door_traversal,
   door_lock, keydoor.

## Maengel (nummeriert, nachpruefbar)
**V1 (Vertrag, formal):** Haken ausserhalb der Zuteilung VERTRAG 1.3: `re15_port/engine/src/scd_vm.c` register_opcodes
(`s_op_table[0x48]/[0x49]`, 3 Zeilen, laut 1.3 nur C/F/G) und `re15_port/platform/pc/main.c` pc_prop_world (4 Zeilen, laut 1.3 nur
D und F/G). Klein, markiert "Runde 41 Spur J", technisch ohne Alternative (Opcode-Tabelle und PC-Zeichner liegen nur dort), aber ohne
Orchestrator-Entscheid; Konfliktgefahr mit C/F/G in register_opcodes bei der Integration. Abhilfe: Entscheid wie B5/A-E3/E-E1
(annehmen) oder Haken verlegen. (Der RBJ-Spieler-Haken in main.c ist durch A-E5 gedeckt.)

**M2 (Doku, klein):** Die Nebenwirkungs-Aussage zu Super_set in ROOM4031 ist falsch: Dossier §4.1, Bau-Bericht ("einziger Fall
ROOM4030/4031 sub07, Typ 0x2D in State 1 ... uebersprungen") und Kopf `re15_superobj_r41.h`. In ROOM4031 ist Gegner 0 beim ersten
Besuch John (Typ 0x49, State 4); per Kunstgriff gemessen haengt der Port Objekt 0 dort jetzt an (Lauf `J41abn0/laeufe/e4031_super`,
`[superobj41] F621 ... -> angehaengt`). Im Spiel ist diese Lage nach eigener Pruefung nicht erreichbar (Aufzugs-AOT erst mit Bit
3:87 aus 4040/4041, das erst nach dem ersten 4031-Besuch gesetzt werden kann; Kette s. Gate N) — der Bau-Agent hat 4031 aber
weder gemessen noch so begruendet. Abhilfe: Dossier §4.1 und Header berichtigen (4031: Gegner 0 = John beim ersten Besuch,
unerreichbar mit Beleg-Kette; der Original-Handler @0x80040c18 nimmt Gegner 0 ohne Typpruefung).

## Hinweise (keine Maengel)
- H1: `integration_r41_stage3szenen_a_3060` nimmt die Tuer ROOM3040 -> ROOM3060 noch per `RE15_FIRE_AOT=0@1200#3040`. Mein Lauf
  p1a/p1b zeigt, dass der echte Weg genauso geht (RE15_PLAYER_POS=-8900,-26300,1024,0 = Ankunftspunkt der Gegentuer @0x00ED6,
  Quadrat -> `[aot] DOOR FIRE slot=0 rect=(-8800,-26950,...)`). Empfehlung: Riegel wie 3061 auf Quadrat umstellen.
- H2: Kopfkommentar `tests/unit/probes/r41_stage3szenen_a.cmake` ("Abweichung <= 40", "~ Original") ist veraltet; die Riegel pruefen
  auf 0.
- H3: Fuer jeden NPC in State 4 rechnet `re15_fussanker_r41_merken_aktor` je Bild zwei Posen (Part 4/7), auch ohne Anker-Bit.
  Auf dem PC ohne Belang; fuer das (derzeit nicht baubare) PSX-Ziel waere "nur merken, wenn Bit 1/2 gesetzt ist oder im naechsten
  Bild gesetzt werden kann" guenstiger (Laufzeitbudget). Kein Verhaltensunterschied.
- H4: Neue Quellen fussanker/superobj/wclip/rbj_binder kompilieren mit `-Wall -Wextra` ohne eigene Warnung (eigener Probelauf in den
  Scratchpad, einzige Meldung ist die alte `-Wcomment` in re15_emd.h:37).

## Gesamturteil: **NICHT bestanden** — alle drei Nutzer-Punkte erfuellt; offen sind nur zwei kleine Gate-Maengel (V1 formal, M2 Doku)
| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 ROOM3060 Marvin | **erfuellt** | echter Tuerweg 3040 -> 3060 per Quadrat; W-Clip 6 (10 Bilder) + 31 x Bild 9 bis zur Tuer, erstes 3020-Bild ohne W-Clip; Original o3060_s2_t03 cmd 4/Sub 2/Phase 2/Clip 6 (+0x168 = PL00W01-Clip-6 Bild 9); Bildfolge = Original-Video; auch mit Pistole |
| 2a ROOM3061 Sherry kriecht | **erfuellt** | Anker FUN_800369f8 bytegleich zur Original-Reihe o3061_s3; Super_set/Super_reset; Sherry (-18792,-30700) vs (-18795,-30700); Bildfolge = Original-Video; CONTINUE-Weg und echter Tuerweg 3011 -> 3061 identisch; auf master reproduziert (z bleibt -26308) |
| 2b ROOM3061 Elzas Ende | **erfuellt** | W-Bank PL04W01 Clip 6 + Halten (56 x Bild 9); Original o3061_s2_t17 cmd 4/Sub 2/Phase 2/Clip 6; Lage/Blick (-18510,-26206) 3651 gleich; Bildfolge = Original-Video |
| 0 A-E5 RBJ-Binder | erfuellt | FUN_8001b3f8 selbst disassembliert, eigener Commit e4a9f39c, Riegel, Laufzeit-Stichprobe ROOM4001, Gegenmessung der Spur K in 3091 |

Gates: RE **haelt** (11 Adressbereiche selbst disassembliert, Original-Daten selbst gelesen, MZD-RAM = PSX.EXE); V **Mangel V1**
(formal, Orchestrator-Entscheid wie B5 genuegt); N **keine erreichbare Nebenwirkung**, aber **Mangel M2** (falsche Dossier-Aussage
zu ROOM4031) + Nebenbefund N1 (Hund, nicht Spur J); T: Riegel der Spur 9/9, unit/probe/prop 917/923 (6 release/-Skript-Zeitablaeufe
unter Last, vom Diff unberuehrt), Integration 52/54 + Nachlaeufe (s. Gate T und Stand unten).
