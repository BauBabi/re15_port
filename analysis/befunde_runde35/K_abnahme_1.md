# Runde 35 — Spur K "cut10f0": UNABHAENGIGE ABNAHME 1

Baum `.claude/worktrees/r35_cut10f0`, Zweig `r35/cut10f0`, HEAD 3ee832c5 (Basis master 154a73c1), 2026-10-03 abends.
Geprueft gegen den WORTLAUT in `AUFTRAG.md` Z.40-67 und Z.92 und gegen die vier Maengel aus `K_abnahme_0.md`.
Kein Code geaendert. Messlaeufe mit Kopien der exe unter eigenem Namen (`re15_pc_kabn1.exe` im Baum,
`re15_pc_kabn1m.exe` im Scratch-Bau K+L), Arbeitsverzeichnisse und Bilder im Sitzungs-Scratch `k_abn1/`
(nicht eingecheckt).

## 0. Urteil

**BESTANDEN.** Alle vier Nutzer-Punkte am gebauten Stand gemessen erfuellt, die vier Maengel der Abnahme 0 behoben,
alle Gates halten. Punkt 3 ist nur im Zusammenspiel mit Spur L messbar (Beginn = (9,73) von L, VERTRAG §1.1) und
wurde dort gemessen (Scratch-Bau K+L, §5.2).

| Punkt (AUFTRAG.md) | Urteil | Kurz |
|---|---|---|
| 1 Szene beim ersten Betreten von ROOM10F0 (Z.41-65) | erfuellt | 18 Zeilen in Reihenfolge, Kamera 2 -> 0 -> (1 -> 2 beim Laufen) -> 0 (Marvin) -> 2, 2 Tuerknalle, Abgang, Balken |
| 2 Karte: ROOM11C0 markiert, dann ROOM1150 blinkend, beide bis besucht (Z.66) | erfuellt | Hinweis 3 Perioden je Ziel; normale Karte: B1/11C0 und 3F/1150 blinken; 11C0 blinkt auch nach dem Montage-Schnitt der Spur L weiter |
| 3 MAIN01 durchweg bis zum Parkplatz (Z.92) | erfuellt (mit L) | Beginn erst nach dem Ende der 1150-Montage; laeuft durch Raumwechsel und durch den Skript-Stop im Zwinger; endet an der Tuer zum Parkplatz |
| 4 Animationen passend wie vergleichbare Dialoge (Z.67) | erfuellt | Zeile 6 jetzt zu Ada gedreht (Gierung 2941, Soll 2942) mit Clip 15; jede Sprecherzeile mit Geste/Kopf |

| Mangel aus K_abnahme_0.md | Stand (selbst gemessen) |
|---|---|
| M1 MAIN01 verstummt durch Skript-Stop (ROOM11D0 sub01 @0x01710) | behoben — Lauf G §5.3 |
| M2 MAIN01 beginnt eine Szene zu frueh | behoben — Lauf A (K allein: kein FF01), Laeufe m1/m2/m3 (K+L) §5.2 |
| M3 Zeile 6 Arm an Ada vorbei | behoben — Lauf A §3, Bild `k_abn1/z_a_zeile6.png` |
| M4 Haken > 5 Zeilen | behoben — §7.2 |

## 1. Bau und Suite

- `bash re15_port/tools/local_build.sh configure` + `... build` selbst gefahren: `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. exe 18:15:15; `git diff 515dc35c HEAD --stat` = nur `K_cut10f0.md` -> die exe ist
  der Code des HEAD.
- Ganze Suite NICHT selbst gefahren: das Dossier traegt §9.8 woertlich `=== LOCAL-BUILD-OK (all) — Tests 486/486`
  (>= 478). Gegengeprueft: `re15_port/build/local_build_ctest.log` (18:38:22) endet mit
  `100% tests passed, 0 tests failed out of 486`.
- Selbst gefahren: `ctest -R "r35_cut10f0|unit_cam_selfheal|unit_rotor_bgm_pin"` -> **10/10 gruen**
  (unit_rotor_bgm_pin, unit_cam_selfheal, unit_r35_cut10f0_programm/texte/tuerton/szene/einmal/karte/bgm,
  integration_r35_cut10f0 237,97 s). `test_r35_cut10f0.exe szene` ausgegeben:
  `ok: Zeile 6 'Hey - how did you came in here?' (Leon an der Tuer): Leon blickt zu Ada (Gierung 2941, Soll 2942)`,
  `ok: Zeile 6: Leon steht dabei noch am Tuer-Spawn (8400,-350)`.

## 2. Messlaeufe (alle selbst gefahren)

Start je Lauf: Kartenwerkzeug -> `RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`,
`RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60`, Logs `RE15_STATE_LOG RE15_NECK_LOG RE15_MSG_LOG
RE15_SE_DEBUG RE15_CAM_TRACE RE15_BGM_CTL_DEBUG`, Bilder `RE15_FRAMEDUMP` (beschleunigter Renderer im Baum K),
Ton `RE15_AUDIO_CAP_SYNC` (1470 Stereo-Frames je Tick; Pegel = mittlerer Betrag je 30 Ticks).

| Lauf | exe | Karte / Weg | Zweck |
|---|---|---|---|
| A | K | `probe_r35_cut10f0_karte ""`: ROOM10D0 vor der Tuer, `A0.2` -> echte Tuer -> ROOM10F0, Ende 3300#10f0, Bilder 60-3300/20, Ton | ganze Szene, Hinweis, Musik in K allein |
| A2 | K | wie A, danach START, L1 (Karte), RUNTER, 3x HOCH; Bilder 3040-3650/10 | Statusschirm + normale Karte nach der Szene (3F) |
| A3 | K | wie A, Bilder 0-16/1, Ende 40#10f0 | erste Bilder nach dem Eintritt |
| H1 | K | `gesehen in10f0` (Laden IN ROOM10F0), START, L1, 2x RUNTER; Bilder 200-510/10 | normale Karte Blatt B1 |
| G | K | `gesehen montage vor11d0`: Flur ROOM1180, echte Tuer -> Zwinger ROOM11D0, Ton | M1 |
| E | K | `gesehen montage vor11c0`: ROOM11B0, echte Tuer -> Parkplatz ROOM11C0, Ton | Ende von MAIN01 |
| m1 | K+L | L-Weg "A": `probe_r35_cut1150_karte 1130 nach10f0 ersteszene`, `W1,A0.2,W200`, Ende 1800#1150, `RE15_FLAG_TRACE RE15_IT_LOG=30`, Ton | M2 am echten Montage-Ablauf |
| m2/m3 | K+L | wie m1, nach der Rueckkehr START, L1, 1x bzw. 3x RUNTER; Bilder ab 1600 | Karte nach dem Montage-Schnitt nach ROOM11C0 |

**Scratch-Bau K+L** (nur Messung, nichts davon im Zweig): `git merge-tree --write-tree HEAD r35/cut1150`
(K 3ee832c5, **L bb5b47d5** — L ist seit dem Lauf des Bau-Agenten (6f03b1ef) weitergelaufen) -> Baum d710b234,
`git archive` ohne `shared_assets` in den Scratch, Konflikte aufgeloest wie im Dossier §9.7 beschrieben
(`scd_room_setup.c`/`scd_vm.c` beide Seiten; `enemy_common.c` L-Schleife mit `mslot`, danach die K-Alias-Zeile;
`tests/test_support.c` die drei L-Spionzeilen 100-102 gestrichen), PowerShell `cmake` + `--build --target re15_pc
probe_r35_cut1150_karte probe_r35_cut10f0_karte` -> `[400/400] Linking C executable platform\pc\re15_pc.exe`,
Assets ueber `RE15_CD_ROOT` des Baums K. Die Aufloesungsanleitung des Dossiers stimmt auch fuer L bb5b47d5.

## 3. Punkt 1 — Szene (erfuellt)

Lauf A, `debug.log`: `[cut10f0] ROOM10F0: Szene gestartet (Ereignis 20, Faden 10, Flag (9,71)=0)`,
`[rbj] Animationsblock von ROOM11B0 geliehen (48168 B, ...)`, `[msg] room=10f0 id=6` ... `id=23` in aufsteigender
Reihenfolge, zwei `[se] Se_on bank=14 id=1 -> RE2-Tuerbank`, `[cut10f0] Szene zu Ende`.
Kamera (`[scd F..] Cut_chg`): F6 Cut 2, F66 Cut 0, (RVD beim Laufen: cam 0 -> 1 bei F280 -> 2 bei F334), F394 2,
F770 0 (Marvin an der Tuer), F909 2, F2645 0 (Abgang), F2695 2.

`state.log` (Spieler, Bilder im Raum):

| Bild | gemessen | Wortlaut |
|---|---|---|
| F6-F15 | Cut 2, Leon dreht am Tuer-Spawn 2048 -> 2941 | "Zuerst Kamera auf CUT2 - Ada" |
| F66 | Cut 0 | "Dann Kamera auf CUT0 - Leon" |
| F86-F176 | Nachricht 6, `PL(8400,-350,rot=2941)`, `mo=15` | "Arm strecken ... Hey - how did you came in here?" — Soll-Gierung zu Ada (6000,11500) 2942 |
| F217-F393 | Leon geht ueber (5564,2438) nach (4797,11359), cam 0 -> 1 -> 2 | "auf sie zulaufen", Kamera wechselt |
| F404 | rot 4019 (zu Ada), x 4797 < 6000 | "links von ihr" |
| F428 / F538 / F648 | Nachrichten 7 / 8 / 9; Ada `Plc_motion 19` vor + rueckwaerts bei 7; `PLC_NECK slot=1 mode=2` + `mode=4` bei 8 | 180-Grad-Geste; Kopfschuetteln, Kopf gesenkt |
| ~F760-F766 | Tonspitze 2423 ueber Grund ~650 (Lauf-A-Ton) | "Tuer knallen Sound" |
| F770 / F799 | Cut 0, Marvin an der Tuer; Nachricht 10, Marvin `Plc_motion 15` | "Marvin laden - dann zu Cut0 ... Arm streck" |
| F909-F1092 | Cut 2, Marvin schraeg links; Leon dreht 4019 -> 1415 (zu Marvin) | "steht schraeg links ... alle 3 sehen" (Bild F1100) |
| F1103 / F1229 | Nachricht 11 rot 1415 `mo=15`; Nachricht 12 rot 4019 `mo=15` | Arm zu Marvin; Arm Richtung Ada |
| F1339 / F1449 / F1539 | 13 (Ada Clip 18), 14 (Leon Nicken `slot=0 mode=3`), 15 (Marvin 15, 18) | Vorstellung |
| F1665 | 16, `PLC_NECK slot=0 mode=2` + `mode=4` | Leon Kopfschuetteln, Kopf gebeugt |
| F1795 / F1905 / F2035 | 17 (Marvin 20), 18 "...", 130 Bilder Pause, 19 (21, 17) | "Leon: ..." / "etwas pause" |
| F2145-F2475 | 20, 21, 22, 23 | Reihenfolge wie Auftrag |
| F2600-F2695 | Marvin, dann Ada rennen (Bilder F2600-F2680), Cut 0 Abgang, 2. Knall (Tonspitze F2692), Cut 2 | "rennen hintereinander ... Richtung Tuer" |
| F2695-F2740 | Leon allein, Balken, `mo=200` ab F2740 | "kurz weiter Cutscene Balken" |

Bilder angesehen: `k_abn1/s_a1.png` .. `s_a5.png` (Kontaktboegen), `k_abn1/z_a_zeile6.png` (Zeile 6, 3-fach: Leon der
Kamera von Cut 0 zugewandt — Ada steht hinter dieser Kamera im Raum —, linker Unterarm nach vorn).
Genau einmal: integration B ((9,71)=1) und der Unit-Riegel `einmal` gruen. Boot-Weg (Laden IN ROOM10F0): integration C gruen.

## 4. Punkt 2 — Karte (erfuellt)

- Hinweis am Szenenende (Lauf A): `[hint] F2750 begin`, Blatt "POLICE STATION B1" mit der Kachel ROOM11C0 rot F2751 /
  Umriss F2771 / rot F2790 / Umriss F2810 / rot F2829 / Umriss F2849, je `+Ton Se(2,0x2B)`;
  `[hint] F2868 Folge-Hinweis 1 -> 2 (Zeit)`, Blatt "3F" mit ROOM1150, drei Perioden;
  `[hint] F2986 Zeit um (Hinweis 2, 3 Blinkperioden)`, `schliessen` — 3,95 s je Ziel auf der Wanduhr. Bilder s_a5.png.
- Normale Karte nach der Szene (A2, selbe Sitzung): Statusschirm oeffnet (F3080); Blatt 3F Kachel ROOM1150 rot F3350 /
  F3500 / F3620, Umriss F3410 / F3560 (`s_a2_karte.png`). Nach dem Laden (H1): Blatt B1 Kachel ROOM11C0 rot F260 /
  F350 / F380 / F470 / F500, Umriss F290 / F320 / F410 / F440 (`s_h1.png`).
- **Mit Spur L (m3):** nach der Montage, die ROOM11C0 als Schnitt laedt (`[room] PC loaded room11c0.rdt`, Zonen-Bit
  gesetzt), blinkt die B1-Kachel ROOM11C0 weiter: rot F1760 / F1800 / F1840 / F1880, Umriss F1780 / F1820 / F1860 /
  F1900 (`s_m3.png`); die Fensterzeile nennt `Parkplatz erreicht (4,64)=0`. ROOM1150 ist beim Betreten Ziel-erreicht:
  `[cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1`.
- (4,64)-Zensus selbst: alle 240 RDTs nach `22 04 40` / `21 04 40` durchsucht — im Skriptbereich nur ROOM11C0
  @0x0176C, @0x01820, @0x0184E (die uebrigen Treffer liegen in Modell-/Bilddaten jenseits 0x9000 mit Folgebyte
  e0/33/be).

## 5. Punkt 3 — MAIN01 (erfuellt im Zusammenspiel K+L)

### 5.1 K allein
Lauf A: nur `[bgm] stage=0 room=0F entry=FF20 -> MAIN20`, KEIN `entry=FF01` — weder in der Szene noch danach. Das ist
die Folge von M2 (Beginn = (9,73) von Spur L). Im Zweig K allein erklingt MAIN01 im Spiel nie; erst die
Zusammenfuehrung mit L liefert den Beginn.

### 5.2 K+L (Scratch-Bau, L bb5b47d5) — Beginn nach der Montage (M2)
Lauf m1 `debug.log` (Zeilennummern):
```
  69 [room] PC loaded room1150.rdt / 83 [cut10f0] ROOM1150 nach der Szene betreten: (9,72)=1
  87 [irons-tod] ROOM1150 Zustand 1: Szene startet (Programm 0) / 88 [bgm] room=15 entry=FF1E -> MAIN1E
1388 room1130.rdt / 1392 [flag] z1/27 = 0 / 1397 Montage 1130 (Programm 1) / 1398 entry=FF17 / 1403-04 z2/7, z1/27 = 1
1428 room1040.rdt / 1432 z1/27 = 0 / 1440 Montage 1040 (Programm 2) / 1441 entry=FF00 / 1448-49 Rahmen = 1
1487 room1030.rdt / 1491 z1/27 = 0 / 1499 Montage 1030 (Programm 3) / 1500 entry=4041 / 1509-10 Rahmen = 1
1592 room11c0.rdt / 1594 z1/27 = 0 / 1605 Montage 11C0 (Programm 4) / 1606 entry=FF56 / 1612-13 Rahmen = 1
1669 room1150.rdt / 1672 z1/27 = 0 / 1684 Rueckkehr: Programm 5 / 1690-91 Rahmen = 1 / 1695-96 Rahmen = 0
1697 [cut10f0] MAIN01-Fenster auf in ROOM1150: (9,71)=1 (9,73)=1, Parkplatz erreicht (4,64)=0
1698 [cut10f0] Raummusik-Anstoss in ROOM1150: Soll MAIN01 (letzte Auskunft an die Audio-Schicht: Tabelle)
1699 [bgm] stage=0 room=15 entry=FF01 -> MAIN01(flag 0) SUB--(flags 1/1)
1701 [irons-tod] ROOM1150 Zustand 0: Rueckkehr beendet, Steuerung frei
1735 [bgm] loaded: 9 VAGs, SEQ 7788B   (= MAIN01.BGM)
```
Vor Zeile 1697 KEINE `[cut10f0]`-Fenster- oder Anstoss-Zeile — das Fenster oeffnet weder im Raumaufbau eines
Montage-Schritts (zwischen `z1/27 = 0` und dem ersten Programmlauf) noch im Schnitt nach ROOM11C0. Ton m1: nach dem
Rueckkehr-Laden ab Tick ~3750 durchgehend Musik bis zum Ende (Tick 4020-5440: 434..2342), dieselbe Pegelfolge wie
MAIN01 in Lauf G. m2/m3 wiederholen 1676/1677/1679 identisch.

### 5.3 Durchweg — Skript-Stop im Fenster (M1), Lauf G
```
50 [bgm] stage=0 room=18 entry=FF1D -> MAIN1D / 53 CONTINUE: resumed in room 1180
61 [cut10f0] MAIN01-Fenster auf in ROOM1180 ... / 63 [bgm] room=18 entry=FF01 -> MAIN01 / 70 loaded SEQ 7788B
78 [room] PC loaded room11d0.rdt
89 [bgm] stage=0 room=1D entry=FF01 -> MAIN01(flag 0) ...  [unveraendert, laeuft durch]
93 [bgm] Sce_bgm_control slot=0 op=2 im MAIN01-Fenster NICHT angewandt (Runde 35 Spur K) capTick=596
```
Ton (Pegel je 30 Ticks) ab dem Stop-Befehl bei Tick 596 bis Laufende 1190: 310, 134, 18, 628, 445, 442, 432, 795, 827,
804, 852, 820, 1182, 1517, 1428, 1513, 1786, 1703, 1322, 1333 — kein Abfall auf 0 (Abnahme 0: 580 -> 0). Die kurze
Senke (18) liegt bei derselben Stelle der Sequenz wie in m1 (31 bei Tick 3990, gleiche Pegelfolge davor und danach),
ist also Teil von MAIN01 und keine Stille. Raumwechsel im Fenster: Lauf G (1180 -> 11D0) und integration D
(10F0 -> 10D0) `[unveraendert, laeuft durch]`.

### 5.4 Ende am Parkplatz, Lauf E
`[bgm] room=1B entry=FF01 -> MAIN01`, echte Tuer, `[room] PC loaded room11c0.rdt`, `[bgm] stage=0 room=1C entry=FF56 ->
MAIN16(flag 1)`, `[cut10f0] MAIN01-Fenster zu in ROOM11C0: ... Parkplatz erreicht (4,64)=1`, danach die
Ankunftsszene (`[msg] room=11c0 id=0`, `id=1`). Der Parkplatz spielt seine eigene Musik ab dem Eintritt.

## 6. Punkt 4 — Animationen (erfuellt)

Lauf A, Gesten je Zeile (`[scd] Plc_motion`, Faden-Slot 0 Leon / 1 Ada / 2 Marvin) und Kopf (`neck.log`):
6 Leon 15 (zu Ada gedreht, Blick `PLC_NECK slot=0 mode=1 tgt=(6000,0,11500)` als ERSTE neck-Zeile, also vor der Zeile);
7 Ada 19 vor + rueckwaerts; 8/9 Ada Kopf gesenkt + Schuetteln; 10 Marvin 15; 11 Leon 15 (rot 1415 zu Marvin);
12 Leon 15 (rot 4019 zu Ada); 13 Ada 18; 14 Leon Nicken; 15 Marvin 15 + 18; 16 Leon Kopf gesenkt + Schuetteln;
17 Marvin 20; 19 Leon 21 + 17; 20 Marvin 15 + 16; 21 Leon 15 + 16; 22 Leon 17; 23 Marvin 15 + 16.
Mangel 3 der Abnahme 0 ist behoben: die Form "Blick, Drehung, Schnitt, Sleep 20, Zeile + Clip 15" ist die von
ROOM11C0 sub02 (Bytes selbst gelesen, §7.1). Unit-Riegel `szene` prueft jetzt auch Zeile 6.

## 7. Gates

### 7.1 RE-Gate (@0x) — Stichproben selbst disassembliert (`re15_disasm.py`, info/Re1.5/PSX.EXE)
- **FUN_80044da4 op 2**: Sprungtabelle @0x80010e58 = 0x80044F28, 0x80044E00, **0x80044E50**, 0x80044E88, 0x80044EE8,
  0x80044F20; @0x80044e60 `lb a0,0(at)` (0x800b52ae + slot*8), @0x80044e64 `jal 0x800603dc`, @0x80044e7c `sb v0` (=2).
  Sprungziel 0x800603dc -> `jal 0x80060270` (_SsSndStop), dort @0x8006031c `ori v0,v0,0x4` / `sw v0,144(v1)`.
- **_SsSndReplay**: @0x8005ac88 `lw v1,144(a2)`, @0x8005ac90 `andi v0,v1,0x204`, @0x8005ac94 `bne v0,zero,0x8005ace4`.
  Auto-Replay FUN_800444b0: @0x800444b8 `lbu v1,0x800b52ad`, @0x800444c8 `bne v1,zero`, @0x800444d8 `jal 0x8005acec`.
  FUN_80044210 @0x80044278-80 `andi a0,s0,0x3f` / `andi v1,v1,0x3f` / `beq a0,v1,0x800442f4`.
- **FUN_800396fc**: @0x8003970c `lw v0,0x800aca3c`, @0x80039710 `lui v1,0xffff`, @0x80039728 `and v0,v0,v1`,
  @0x80039730 `sw v0,0x800aca3c`; Flag-Tabelle @0x80074664[1] = 0x800ACA3C (Bank 1) — (1,27) = Maske 0x10 liegt in
  den geloeschten unteren 16 Bits.
- **FUN_8003f038**: @0x8003f040 `lw v0,g_pauseflags`, @0x8003f044 `lui v1,0x200`, @0x8003f04c `bne v0,zero,0x8003f090`
  (springt ueber `jal 0x8003ea3c`). Gemessen dazu (Lauf A3): `pf=FF000007` in F1-F5 von ROOM10F0, `pf=01000007` ab F6 —
  das Szenenprogramm laeuft erst ab F6 (`[scd F6] Cut_chg(2)`).
- RDT-Bytes (shared_assets/PSX/STAGE1): ROOM11C0 @0x01820 `21 04 40 00 04 0a 18 02`, @0x0184E `22 04 40 01`, @0x01886
  `41 01 fb dc 00 00 f5 c7 64 00`, @0x01890 `40 00 09 00 fb dc f5 c7`, @0x0189C `09 0a 14 00`, @0x018A0
  `2b 00 00 00 3f 00 0f 00`, @0x01810 `54 00 01 00 00 00`; ROOM11D0 @0x016E4 fuenf Ck(7,..) + `21 05 00 00`, @0x01710
  `54 00 02 00 00 00`; ROOM10D0 @0x01174 Bytes 14..21 `d0 20 00 00 a2 fe 00 08`, @0x01A02 `36 02 0c 00 01 00`.
  Jede Stelle enthaelt das Behauptete.
- Suchworte im Diff (deferred, tunable, interim, for now, faithful, plausib, TODO, FIXME, approx): keine Treffer.
  getenv im Diff nur fuer die Logzeilen `RE15_BGM_CTL_DEBUG` / `RE15_SE_DEBUG` — kein Env-Schalter als Abschluss.
- Konstanten der Nachbesserung: `RE15_CUT10F0_BGM_START_*` (9,73) = VERTRAG §1.1; `RE15_CUT10F0_ZIEL1_ERREICHT_*`
  (4,64) = @0x0184E; Auswertestelle `g_scd.tick_count > s_vm_tick_aufbau` als PORT-WAHL gekennzeichnet, Form
  (@0x80039710-30, @0x8003f04c) belegt. Szenen-Choreografie (Sleeps, Positionen) im Generatorkopf als
  NUTZER-VORGABE / PORT-WAHL gekennzeichnet, Opcode-Formen je mit Datei-Offset. Commit 3ee832c5 traegt die Adressen.
- Fix erklaert den Befund: M2 — die Vorbedingung (Fenster oeffnet zwischen `z1/27 = 0` und dem ersten Programmlauf)
  steht im m1-Protokoll des Bau-Agenten; in meinem m1 liegt das Oeffnen erst nach dem Loeschen der Rahmenflags der
  Rueckkehr (Zeilen 1695-1697). M1 — der angewandte Stop (Abnahme 0, Pegel 0) ist jetzt als NICHT angewandt geloggt
  und der Pegel bleibt.

### 7.2 Vertrag und Pfade
- Bank-9-Bits 71 und 72 (Spur K), gelesen (9,73) (Spur L) und (4,64) (Original); Nachrichten 6..23 (Zuteilung 6..30);
  Ereignis 20; kein AOT-Slot.
- `git diff master --name-only`: nichts unter `release/`, `platform/android/`, `shared_assets/PSX/`, keine
  `tests/unit/CMakeLists.txt` / `tests/integration/CMakeLists.txt`. Neue Tests unter `tests/unit/probes/r35_cut10f0.cmake`,
  `tests/unit/test_r35_cut10f0.c`, `tests/integration/test_r35_cut10f0.cmake`.
- Haken in gemeinsamen Dateien (`git diff master -U0`, Hunks): menu_common.c 1 / 5 / 1 / 1 Zeilen, platform/pc/main.c
  1 / 4 / 3 / 4, audio_pc.c 1 / 3 / 3 / 3, scd_room_setup.c 1 / 4, scd_vm.c 1 / 2, game_step_common.c 1 / 1,
  enemy_common.c 1 / 3 — keiner ueber 5 Zeilen. M4 behoben.

### 7.3 Tests
Je Punkt vorhanden und messend: Szene (unit programm/texte/tuerton/szene/einmal, echte VM; integration A/B/C, echte exe
am Tuerweg), Karte (unit karte inkl. Montage-Schnitt 11C0; integration A Hinweiskette), MAIN01 (unit bgm inkl.
Raumaufbau mitten in der Montage; integration A/D/E/F/G), Animationen (unit szene: Gierung je Zeile, Zeile 6 neu).
Grenze: der echte Montage-Ablauf der Spur L ist im Zweig K nur durch `RE15_SET_FLAG_AT=9:73` vertreten
(integration A/F) — nach der Zusammenfuehrung sollte der L-Weg "A" um die Pruefung "kein `MAIN01-Fenster auf` vor
`Rueckkehr beendet`" ergaenzt werden (Hinweis, kein Mangel: im Zweig K nicht baubar).

## 8. Maengel

Keine.

## 9. Beobachtungen ohne Urteil (fuer Orchestrator / Nutzer)

1. **Zusammenfuehrung Pflicht:** ohne Spur L erklingt MAIN01 nie (§5.1). Konflikte K <-> L (L bb5b47d5) wie Dossier
   §9.7; aufgeloest gebaut und gemessen (§2).
2. **Zwei helle Bilder Cut 0 vor dem Szenenbeginn** (Lauf A3): die Tuer-Einblendung laeuft auf der Eintrittskamera
   der Tuer (ROOM10D0 @0x01174 Byte 24 = Cut 0), mittlere Helligkeit F4 12,0 / F5 38,3 / F6 48,5 (Vollbild ~49); das
   Szenenprogramm steht bis F5 hinter dem Freeze-Gate @0x8003f04c (`pf=FF000007`), `Cut_chg(2)` ab F6, Cut 2 sichtbar
   ab F7, Balken fahren ab F8 ein. Die Szene selbst (mit Balken) beginnt auf Cut 2; ob die ~67 ms Cut 0 der
   Einblendung stoeren, muss der Nutzer am Bild entscheiden (`k_abn1/s_a3.png`).
3. Waehrend des Montage-Schnitts nach ROOM11C0 (Spur L, vor dem MAIN01-Fenster) ist der Ton ~18 s still (m1 Tick
   3180-3720; Tabelle 0xFF56 = MAIN16 mit Handstart-Flag) — Sache der Spur L, nicht des K-Fensters.
4. Kommentar des Hakens `game_step_common.c` ("Szenen-Ende ROOM10F0 -> Kartenhinweis + MAIN01") ist seit M2 ungenau:
   am Szenenende wird nur noch der Hinweis angefordert; MAIN01 oeffnet der Tick nach der Montage.
5. In m2 blinkt auf Blatt 2F die Kachel ROOM10F0 (Runde-33-Ziel der ersten Irons-Szene), weil die L-Messkarte die Zone
   10F0 nicht als besucht fuehrt — Artefakt der Messkarte, im echten Durchlauf war Leon dort.
6. Offen laut Dossier, nicht nachgemessen: Parkplatz VOR der Szene erreicht ((4,64)=1) -> keine Kachel 11C0, kein
   MAIN01; PSX (audio_psx.c ohne Weiche). Android baut `platform/pc/src/*.c` per GLOB mit
   (`platform/android/jni/CMakeLists.txt:45`), also auch die Weiche — nicht auf dem Geraet gemessen.
7. Sprachdateien `synchro/STAGE1/room10F0/main06..main23.wav` fehlen; die Zeilen laufen stumm mit Untertitel.
8. Leon traegt in der Szene das Messer in der Hand (Spur E). Dialogkamera Cut 2 zeigt die Figuren klein (Nutzer-Vorgabe
   "hinten rechts ... bei CUT2").
