# Runde 39 Spur C "inventar" — Abnahme 0 (unabhaengig, 2026-10-18)

Pruefer: Abnahme-Agent 0 (eigene Messungen, eigene exe-Kopie `re15_pc_abn0_mess.exe` neben der exe, eigener Laufordner
`<scratchpad>/abn/runs/<name>/` mit `debug.log`, `karte.txt`, ggf. `state.txt`/`status.ppm`; scratchpad =
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad`).
Baum: `.claude/worktrees/r39_inventar`, HEAD a78ee047 (Basis 3e49ff87 = v0.8.26 + 2 Doku-Commits), Arbeitsbaum sauber.
Gelesen: AUFTRAG.md, VERTRAG.md, C_inventar.md, `git log 3e49ff87..HEAD` (13 Commits), `git diff 3e49ff87 -- re15_port`
(20 Dateien, +1545/-64) vollstaendig, inkl. aller Riegel.

**Ergebnis: BESTANDEN — alle vier Punkte erfuellt, alle Gates halten. Keine Maengel. Hinweise in Abschnitt 4.**

## 0. Bau und Riegel (selbst gefahren)

* `bash re15_port/tools/local_build.sh configure` + `build`: `LOCAL-BUILD-OK (configure)`, `LOCAL-BUILD-OK (build)`,
  Ninja "no work to do" — der Bau-Agent hatte den Stand gebaut; `ninja -n` bestaetigt: keine Quelle neuer als ihr
  Ziel (re15_pc.exe 07:31; test_r39_inventar.exe 07:35 > .c 07:34; test_r37_feuerloescher.exe 07:39 > .c 07:37;
  r21/r26-exe 07:39 > .c 07:37; die Commits danach aendern nur Doku/cmake-Skripte).
* `ctest -R "unit_r39_inventar"`: **100% tests passed, 0 tests failed out of 4** (spind/stapeln/status/discard).
* `ctest -R "unit_(r21_discard|r26_inventar|r36_kartenbits|r37_feuerloescher|r37_kiste|r37_kistenregel|r38_itemorte|
  r38_kleinkram|r35_inventar1050|r35_werfer|r36_inventar|r36_dachkiste|r37_codepanel|item_stack|savepoint|em_status|
  debug_menu|pri_inventar|inv_fsm|inv_grant|itembox|room1150_itembox|r25)"`: **100% tests passed, 0 tests failed out
  of 52** (15,4 s).
* `ctest -R "integration_r39_inventar" -j2`: **100% tests passed, 0 tests failed out of 4** (spind 50 s, discard 72 s,
  stapeln 41 s, status 33 s).
* Integrations-Riegel der beruehrten Module `ctest -R "integration_(r35_inventar1050|r36_inventar|r36_dachkiste|
  r37_feuerloescher|r37_kiste|r37_kiste2feld|r37_kistenregel|r38_itemorte|r38_kleinkram_inventar)$" -j2`:
  **100% tests passed, 0 tests failed out of 9** (1471 s gesamt unter Fremdlast anderer Spuren; r36_dachkiste 1418 s,
  r38_itemorte 406 s, r37_feuerloescher 399 s, r37_kistenregel 266 s, r36_inventar 145 s, r35_inventar1050 75 s,
  r37_kiste 63 s, r38_kleinkram_inventar 59 s, r37_kiste2feld 57 s).

## 1. Gates

### 1.1 Pfade / Vertrag
* `git diff 3e49ff87 --name-only`: 25 Dateien, davon 20 unter `re15_port/` und 5 unter `analysis/befunde_runde39/`.
  Kein `release/`, kein `platform/android/`, kein `shared_assets/PSX/`, keine CMakeLists (grep leer). Die
  `release/make_package.sh`-Zeile im `git diff master --stat` ist master-seitig (387a7a69, Spur B), nicht dieser Zweig.
* Bank-9-Bit: nur **69** (VERTRAG 1.1 = C) — `RE15_R39C_MUNITION_BIT 69`, in `k_port[]` eingetragen (eine Zeile +
  Include). Nachrichten-Id 40: nicht gebraucht (ROOM1000 msg 0 ist der vorhandene Text). AOT-Slot 11 = PORT-WAHL,
  im Header so gekennzeichnet und begruendet (erster freier Slot, Sonde).
* Gemeinsame Dateien (VERTRAG 1.3): `inventory_common.c` 2 Zeilen + Include, `menu_common.c` 5 Zeilen,
  `re15_itembox.c` 1 Zeile + Include, `scd_room_setup.c` 1 Installer-Zeile am Ende des Blocks + Include,
  `platform/pc/main.c` 1 Installer-Zeile + 1 Messschienen-Zeile + Include, `test_r36_kartenbits.c` 1 Zeile + Include.
  Alle mit Kommentar "Runde 39 Spur C". `inv_render_pc.c` (52 Zeilen) ist keine Vertrags-Gemeinschaftsdatei
  (render_pc.c gehoert L; inv_render_pc.c ist der Inventar-Zeichner = Domaene C).
* Guess-Tells (`deferred|tunable|interim|for now|faithful|plausibel|TODO|vorerst|approx|vermutlich|wahrscheinlich`) in
  den `+`-Zeilen des Diffs: **0 Treffer**. Kein Env-Schalter als Abschluss (RE15_INV_LOG ist eine reine Messschiene;
  `re15_inventar_r39_log_frame` enthaelt nur `fprintf`, keinen Zustand).
* Je Punkt ein Unit- und ein Integrations-Riegel vorhanden und messend (Abschnitt 0, selbst gefahren); Alt-Riegel
  r21/r26/r37 mit Kommentar auf die Runde-39-Sollwerte nachgezogen (Diff gelesen: r21 "9 Gegenstaende", TEIL R 5 Gates /
  5 lebend + 1 tot; r26 C7a "EIN Platz, 2 + 2 = 4"; r37 R/S 1000 Slot 3 Munition / Text Slot 11, erster Druck Modal,
  zweiter msg 0; r37-Integration Teil D mit `flag:9:69`).

### 1.2 RE-Gate — selbst disassembliert / Bytes selbst gelesen (ghidra1_V2.txt, ghidra_re2_Leon.txt, PSX.EXE,
DEBUG.BIN, RDTs; alles aus dem Auslieferungsstand `info/Re1.5/` bzw. `re15_port/shared_assets/PSX`)

| Zitat im Diff | Selbst gelesen | Stimmt |
|---|---|---|
| Ziffern-Gate `sltiu id,0x22` @0x80049124 | `80049124 22 00 42 2c sltiu v0,v0,0x22` nach `lbu v0,DAT_800b10ac` @0x8004911c (Item-Id des Platzes), `beq v0,zero,LAB_80049264` | ja |
| Obergrenze `lbu` @0x8004e338 auf @0x80074DA8 | `8004e338 00 00 23 90 lbu v1,0x0(at)=>DAT_80074da8` | ja |
| Satz-Zeiger +4 @0x8004e9e8, Anzahl +9 @0x8004e9d8 | `lw v1,PTR_DAT_80074dac = 80074c88`, `lbu a1,DAT_80074db1` | ja |
| Aktion 5 = Executor @0x8004e664 | Jump-Table `switchD_8004e1a4` @0x800112ac: `caseD_5 = 0x8004e664`; dort `andi v0,s4,0xff / andi v1,s3,0xff / sltu / bne v0,zero,LAB_8004e324 / j LAB_8004e430` | ja |
| Zeile 0x21 @0x80074F34 `fa 00 00 00 88 4c 07 80 01 00` | PSX.EXE (t_addr 0x80010000): `fa 00 00 00 88 4c 07 80 01 00 00 00`, Satz @0x80074c88 n=0 | ja |
| 0x09/0x0A/0x0B @0x80074E14/E20/E2C `fa`, Null-Satz | `fa 00 00 00 88 4c 07 80 03/01/02 00 00 00`, n=0 | ja |
| 0x15 Selbst-Satz @0x80074CCC `15 15 05 00` | Zeile 0x15 @0x80074ea4 `fa .. cc 4c 07 80 00 07`, Satz: `15 15 05 00 03 03 03 00 ...` (7 Saetze) | ja |
| 0x16 @0x80074CE8 `16 16 05 00` | `16 16 05 00 0d 0d 03 00 08 08 03 00` | ja |
| Handgun 0x03 Obergrenze = Magazin | Zeile 0x03 @0x80074dcc `0f 00 00 00 8c 4c 07 80 03 01`, Satz `15 03 02 00` (Nachladen) | ja (15 = Magazin) |
| Aktions-Scan @0x80042f50 `beq v0,zero` / @0x80042f94 `j 0x80043028` | `80042f50 beq v0,zero,LAB_8004301c`, `80042f94 j LAB_80043028` | ja |
| Raumlader @0x800397ec `addu a0,a0,v0`, Merge @0x80039784 | `800397ec _addu a0,a0,v0`, `80039784 or v0,v0,v1` | ja |
| `sltiu id,0x15` @0x80047d54 | `80047d54 sltiu v0,v0,0x15` | ja |
| DEBUG.BIN FUN_800c01c4: `lbu v1,-13732(v1)` @0x800c01d0, `addiu v0,v0,15064` @0x800c01d8, `lhu a0,0(v0)` @0x800c01e4, `jal 0x80013b60` @0x800c01f0 | DEBUG.BIN (0x40000 B, kein Header, Basis 0x800c0000): 01cc `3c03800b`, 01d0 `9063ca5c` (lbu v1,-0x35a4(v1) = 0x800aca5c), 01d4 `3c028007`, 01d8 `24423ad8` (= 0x80073ad8), 01dc `00031840` (sll v1,v1,1), 01e0 `00431021`, 01e4 `94440000`, 01f0 `0c004ed8` (jal 0x80013b60) | ja |
| Aufrufer @0x800316d0 | `800316d0 jal FUN_800c01c4` | ja |
| Tabelle @0x80073ad8 = u16 0x2c..0x3b | `002c 002d ... 003b` (16 Eintraege) | ja |
| RE2 Ink Ribbon 0x1E @0x800A9F0C `fa 00 00 01 b8 9d 0a 80`, Satz @0x800A9DB8 `1e 05 1e ff` | re2leon PSX.EXE: identisch; 0x14 @0x800A9D4C `14 05 14 ff` | ja |
| RE2 Kisten-Gate @0x80070490/94 | `addiu v0,a1,-0x14` / `sltiu v0,v0,0xc` | ja |
| RE2 Zaehler @0x80051810 / Abfrage @0x80051824..34 (Logzeile) | `80051810 addiu v0,v0,-0x1`, `8005181c sb`, `80051824 bne v0,zero`, `8005182c li a1,0x100`, `80051830 li a2,9`, `80051834 jal FUN_8002fe38` | ja |
| ROOM1000.RDT @0x00C24 | `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 31 00 01 00 86 00 ff 00` | ja |
| ROOM1001.RDT @0x00C24 | `50 03 09 31 00 00 14 50 e0 fc 20 03 20 03 15 00 1e 00 86 00 ff 00` | ja |
| ROOM1000 msg 0 @0x0D22 "Nothing unusual." = einziger Text | `rdt_msgdump.py`: genau 1 Nachricht, id 0 @0x0D22 "Nothing unusual." | ja |
| ROOM1000 @0x00CAA Satzform `2c 06 01 31 ...` | `2c 06 01 31 00 00 68 f7 6c ee e8 03 e8 03 00 00` | ja |
| ROOM1240 @0x0055A `22 03 8b 01`, ROOM1241 `22 03 6f 01` | identisch | ja |
| Set(3,0x8B) nur in 11E0/11E1 @0x01A8A und 1240 @0x0055A | Byte-Suche `22 03 8b 01` ueber alle 240 RDTs: 11E0 @0x01A8A, 11E1 @0x01A8A, 1240 @0x0055A — sonst keine | ja |
| ROOM11E1 msg 5 Ausloeser @0x01F86 | `2b 05 ff ff` (Message_on 5, Maske ffff) | ja |
| ROOM11E0/11E1 main00 @0x01576 If-Zweig / Else @0x016C0 Tuer (Dossier 2.4) | @0x01574 `06 00 4e 01` (If) + @0x01578 `21 03 8b 00` (Ck(3,0x8B,0)) -> Leser `2c 03 01 31 ...`; @0x016C0 `07 00 26 00` (Else) + @0x016C4 `3b 03 02 31 ...` (Door_aot_set Slot 3 = Tuer); beide Dateien in beiden Bloecken bytegleich | ja |
| STPIC-Varianten 00=01=02, 04=05=06 | md5: 00/01/02 = 483127ea..., 04/05/06 = 3a9add40..., 03 und 07 verschieden | ja |

ROOM1000-SCD auf Slot-11-Schreiber abgesucht (alle Regionen main/sub, Opcodes 0x2C/0x3B/0x50): nur Slots 0..9 (Tueren
0-2 @0x00BBE/BDE/BFE, Items 3-5 @0x00C24/C3A/C50, Texte 6-9 @0x00CAA..CE6). Slot 10 = Port (r38 Itemorte 0x4A),
Slot 11 frei; Sonde `probe_r34n_e_dokumente aots 1000` (gebauter Stand): Slot 3 ITEM 0x15 x30 bit=69 prop=255, Slot 11
MESSAGE auf demselben Rechteck, erster freier Slot 12.

### 1.3 Erklaert der Fix den gemessenen Befund?
* P1: v0.8.26 `feuerloescher_r37.c spind_umwidmen` macht Slot 3 zum Text (nur Text, Dossier 1 P1) -> jetzt Munition in 3,
  Text in 11. Erklaert.
* P2: v0.8.26 `re15_pickup_stapelt_nutzer` schneidet das Fenster mit "eigener Satz", 0x21 hat den Null-Satz -> zweiter
  Platz; Menue-Matcher ohne Satz -> kein Paar. Jetzt `re15_inventar_r39_stapelt` (Fenster oder Obergrenze 250) +
  Port-Paar Aktion 5. Erklaert.
* P3: v0.8.26 `inv_render_pc.c` laedt fest `ITEM/STPIC_00.TIM`. Jetzt Index aus `work_vars[0x10]`. Erklaert.
* P4: v0.8.26 hatte fuer die Yellow Keycard KEINE Abfrage (siehe 2.4) — der Wortlaut "obwohl ich gesagt habe discard" ist
  auf v0.8.26 nicht reproduzierbar; der Fix liefert die Abfrage nach dem Code und das Entfernen bei "Ja" (Hinweis H1).

## 2. Messungen je Nutzer-Punkt (eigene Laeufe; Lauf-Skript `<scratchpad>/abn/lauf.sh`, Env je Lauf: RE15_NO_INTRO=1
RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_MSG_LOG=1 RE15_INV_LOG=1
RE15_EXIT_AT=<n>; Spielstand per `probe_r37_feuerloescher_karte`; Bestaetigen = Viereck; Protokolle in
`<scratchpad>/abn/runs/<lauf>/debug.log`)

### 2.1 Punkt 1 — "Room 1000 ... im Spind ... jetzt einmal Handfeuerwaffen Munition ... Erst danach den text" — **erfuellt**
| Lauf | Eingabe | Protokoll | Ergebnis |
|---|---|---|---|
| m1a Leon ROOM1000 (CONTINUE vor dem Spind) | `1000 20900 -1100 3072`, RE15_INPUT_SCRIPT=W1,A0.1,W3.5,A0.1,W4,A0.1,W4,A0.1,W4 (Basis spiel, Start 60) | `[feuerloescher] ROOM1000: Spind Slot 3 (Item 0x31 @0x00C24) -> Raumtext msg 0` DANN `[inventar-r39] ROOM1000 Spind: Slot 3 Aufnahme Item 0x15 x30 Bit (9,69) (Form ROOM1001 @0x00C24), Raumtext msg 0 -> Slot 11`; `F0 ... inv=03:15,15:50` -> Druck 1 Modal -> Druck 2 Ja `[inv-r39] F198 ... inv=03:15,15:65` -> Druck 3 `[msgtext] room=1000 id=0 "Nothing unusual."` (genau 1x, F321 msg=1/0) -> Druck 4 schliesst (F445) | erst Munition, dann der Text |
| m1b Leon, Bit (9,69) schon gesetzt | `... flag:9:69`, W1,A,W3.5,A | `[inventar-r39] ROOM1000 Spind: Munition schon genommen (9,69)=1 -> Slot 3 Raumtext msg 0`; keine Inventar-Aenderung (nur `F0`-Zeile); erster Druck -> `msg 0 "Nothing unusual."` | Wiederbetreten: nur der Text |
| m1e Elza ROOM1001 (Nebenwirkung) | `1001 ... elza`, Script wie m1a | kein `[inventar-r39]`-Eintrag (Installer greift nur in 0x1000), `F198 ... inv=03:15,15:65` ueber den ORIGINAL-Satz (Item 0x15 x30, Bit 0x86) | Elza unveraendert = Original |
| **t4 ECHTER WEG (Tuer)** | CONTINUE in ROOM1050 IM Tuer-Rechteck (13700,-13450) rot 2048; Script W1,A0.1 (Tuer),W6,R0.34 (-> rot 3008),W0.3,U7.4 (Gehweg +z 12600 u),W1,A0.1,W3.5,A0.1,W4,A0.1,W4,A0.1,W4; RE15_STATE_LOG=state.txt | `[aot] DOOR FIRE slot=3 rect=(13700,-13450,...) spawn=(21850,0,-13400)` -> ROOM1000: `[feuerloescher] ... -> Raumtext msg 0` dann `[inventar-r39] ... Slot 3 Aufnahme Item 0x15 ... -> Slot 11` (Tuer-Pfad scd_room_reenter); Leon geht (21850,-13400) -> (20803,-818) (state.txt F300..F540); Druck 1 (F544) Modal, Druck 2 (F652) `[inv-r39] F652 R1000 ... inv=03:15,15:65`, Druck 3 (F775) `[msgtext] room=1000 id=0 "Nothing unusual."`, Druck 4 (F898) zu (F899 msg=0/0) | wie m1a, ueber die Tuer betreten |

Menge: 30 im Satz (ROOM1001 @0x00C34 `1e 00`), 15 im Beutel durch die Port-Halbierung JEDER Welt-Aufnahme von Munition
(NUTZER-ENTSCHEIDUNG 2026-09-20, `re15_pickup_menge_nutzer`, `re15_inventory.h:95-115`) — identisch mit jeder anderen
30er-Packung; der Bau-Agent stellt die Entscheidungsfrage "30 statt 15" korrekt dem Nutzer. "Nothing useful" = der einzige
Text von ROOM1000, msg 0 @0x0D22 "Nothing unusual." (rdt_msgdump, selbst gelesen; "den du jetzt schon hast"). Vorrang
Slot 3 vor Slot 11 = Aktions-Scan kleinster Slot (@0x80042f94 `j LAB_80043028`, selbst gelesen); nach der Aufnahme ist
Slot 3 ohne sce -> Slot 11 antwortet. Hinweis zum Messaufbau: der Bildzaehler (g_engine.frame_count) startet beim
Raumwechsel neu, ein RE15_INPUT_SCRIPT mit Basis "spiel" laeuft im neuen Raum also von vorn — deshalb ein Praefix, das in
beiden Raeumen harmlos ist (A im Tuer-Rechteck in 1050 / A ins Leere in 1000).

### 2.2 Punkt 2 — "Memory Cards ... stapelbar, alles mit einer Zahl dran ... was gleich ist" — **erfuellt**
| Lauf | Karte / Eingabe | Protokoll (debug.log) | Ergebnis |
|---|---|---|---|
| m2a Aufnahme Leon ROOM1010 Regal | `1010 2800 -1300 0 give:0x21:3`, Script W1,A,W3.5,A,... | `[inv-r39] F0 ... inv=03:15,15:50,21:3,00:0` -> `[inv-r39] F198 ... inv=03:15,15:50,21:6,00:0` | EIN Platz (v0.8.26: zweiter Platz 21:3,21:3, Dossier 1 P2) |
| m2b COMBINE Leon | `leer give:0x21:3 give:0x21:2`, RE15_PRESS=start@120,square@139,square@149,right@158,square@160,right@162,square@164 | F149 st=3, F158 cmd=3 (COMBINE), F160 st=7, F162 cur2=1, `[inventar-r39] Kombinieren: Item 0x21 Platz 0 + 1 -> Aktion 5 ...`, `[inv-r39] F164 ... inv=21:5,00:0` | stapelt ueber das Menue |
| m2c COMBINE Elza ROOM1001 | wie m2b + `elza` | identische Zustandsfolge, `[inv-r39] F164 R1001 ... inv=21:5,00:0` | beide Szenarien |
| m2d ITEM BOX | `leer give:0x21:3 kiste:0x21:2`, RE15_BOX_SHOT=1 | `F0 ... inv=21:3 box=2:21:2` -> `F34 ... inv=21:5 box=-` | Kiste verschmilzt |
| m2g Granaten (Regel "alles mit Zahl") | `leer give:0x09:1 give:0x09:2`, COMBINE wie m2b | `Kombinieren: Item 0x09 Platz 0 + 1 -> Aktion 5`, `F164 ... inv=09:3,00:0` | Granaten fallen unter die Regel (Dossier OFFEN 3, Entscheidungsfrage an den Nutzer) |

Mechanik gegen den Diff gelesen: `re15_inventar_r39_stapelt` = Id 1..0x21 (Ziffern-Gate) UND (Munitionsfenster 0x15..0x21
ODER Obergrenze 250 in der Tabelle @0x80074DA8) -> 0x15..0x21 + 0x09..0x0B (Zensus-Riegel Z: 16 Ids, 0 Abweichungen,
selbst gefahren). Feuerwaffen (Zahl = Magazin, Obergrenze 4..100) bleiben draussen — Lesart des Wortlauts "an amounts", im
Dossier 2.2 begruendet; laut Zensus gibt es keine Feuerwaffe zweimal. Die Halbierung (`re15_pickup_menge_nutzer`) nutzt
weiter den alten Helfer `re15_pickup_stapelt_nutzer` (0x21 ausgenommen): Memory Card x3 bleibt x3 (m2a: 3 + 3 = 6).

### 2.3 Punkt 3 — "Elza hat im Player Inventory noch das Bild und die Karte von Leon oben drin" — **erfuellt**
| Lauf | Eingabe | Protokoll | Bild |
|---|---|---|---|
| m3a Elza ROOM1001 (CONTINUE) | `1001 20900 -1100 3072 elza`, RE15_PRESS=start@120, RE15_FRAMEDUMP=145:status.ppm | `F129 offen=1`, `[inv] Status-Bild ITEM/STPIC_04.TIM (Figur 4, Runde 39 Spur C)` | `<scratchpad>/abn/m3a_elza_status.png`: oben links "RACCOON CITY DRIVER LICENSE Elza Walker" mit Elzas Foto (selbst angesehen) |
| m3b Leon ROOM1000 (CONTINUE) | `1000 20900 -1100 3072`, dito | `[inv] Status-Bild ITEM/STPIC_00.TIM (Figur 0, ...)` | `m3b_leon_status.png`: "POLICE Leon S. Kennedy" mit Foto — Leon unveraendert |
| e1 Elza ECHTER WEG (Neuspiel, kein CONTINUE) | RE15_TITLE_CONFIRM_MS=3000 (NEW GAME) + RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 (Elza), RE15_PRESS=start@300..2700, RE15_FRAMEDUMP=300-2800/100 | ROOM1241 -> ROOM1031 (Elzas Eroeffnung, msg 20-23/18/19), `F1022 offen=1`, `[inv] Status-Bild ITEM/STPIC_04.TIM (Figur 4, ...)` | `e1_elza_neu_1100.png`: Driver License Elza Walker (selbst angesehen) |

Texel-Vergleich (`probe_r39_inventar_bild <ppm> <STPIC> 14 26 63`): m3a vs STPIC_04 **6789/6789 (100 %)**, vs STPIC_00
556/6786 (8 %); m3b vs STPIC_00 **100 %**, vs STPIC_04 8 %; e1 vs STPIC_04 **100 %**. (Zeilen 63..102 liegen unter der
CONDITION-Tafel, ueber alle 103 Zeilen daher 65 %.) Mechanismus: Index = `work_vars[0x10]` (DAT_800b0ff0, dasselbe Feld
wie das Spielermodell), Rueckfall `g_gameflow.character`; Dateien STPIC_00..0F vorhanden (je 12492 B), Index auf 0..15
geklemmt, kein Dauer-Nachladen moeglich. Westen-/Schadensvarianten: md5 00=01=02 und 04=05=06 (selbst gerechnet).

### 2.4 Punkt 4 — "Ich habe die gelbe Karte bei Elza noch, obwohl ich gesagt habe discard" — **erfuellt**
| Lauf | Eingabe | Protokoll | Ergebnis |
|---|---|---|---|
| m4a Elza ROOM11E1, Ja | `11E1 10400 4250 0 elza give:0x39:1 flag:9:0x8A`, RE15_PRESS=square@100,140,270,320,420,500,560,700,1000, RE15_SET_FLAG_AT=5:13,5:14,5:15,5:16@600 | msg 7 "A card reader..." (F101) -> Ja -> msg 9 "You've used the Yellow Keycard." -> msg 0 "Enter the first number." -> Code-Bits F600 -> msg 5 "You've opened the lock." + `F601 ... msg=1/5 wegwerf=1` -> `F701 ... inv=...,39:0` (Zaehler, @0x80051810-Form) -> `F788 ... wegwerf=1 ... bereit=1` (Abfrage getippt) -> Ja F1000 -> **`[inv-r39] F1000 R11E1 ... inv=03:15,15:50,00:0`** | Karte weg |
| m4b Elza, Nein (Kreuz@1000), danach Viereck@1150 | wie m4a, cross@1000 | bis F788 identisch; `F1000 ... wegwerf=0`, `[inv-r39] F1000 ... inv=03:15,15:50,39:1`; keine weitere Abfrage bis F1300 | Karte bleibt (Anzahl 1 zurueck, @0x800517C4-Form), keine Wiederholung — der Leser ist danach eine Tuer |
| m4c Leon ROOM11E0 mit (3,139) wie nach ROOM1240 @0x0055A | `11E0 10400 4250 0 give:0x39:1 flag:9:0x8A flag:3:139`, square@100,200,300 | kein msg 7, kein `wegwerf=1`, nur `F0 ... 39:1` | Leon unveraendert (Leser tot, E5 fuer 11E0 bleibt) |

Vorher (v0.8.26, statisch aus master `gen/discard_sites.inc`, Stand seit c5b9bb7c 2026-09-22): ROOM11E0 UND ROOM11E1
"VERWORFEN E5", Panel 11E1 `tot=1`, keine Stelle fuer 0x39; `re15_discard_besitz_vor_nachricht` kennt nur die Tabelle +
die Port-Stellen r36/r38 (andere Items) -> auf v0.8.26 gab es fuer die Yellow Keycard KEINE Abfrage (Dossier 1 P4 misst
`wegwerf=0` in jedem Bild). Der Wortlaut "obwohl ich gesagt habe discard" ist damit nicht reproduzierbar; der Bau-Agent hat
das in OFFEN 1 offen benannt. Der Fix liefert, was der Wortlaut verlangt: Elza wird nach dem richtigen Code gefragt, "Ja"
entfernt die Karte. Beleg der Generator-Aenderung (E5 je Szenario-Variante): Set(3,0x8B) steht in genau drei Dateien
(11E0/11E1 @0x01A8A, 1240 @0x0055A — selbst gesucht), ROOM1241 @0x0055A traegt `22 03 6f 01`; Raumlader @0x800397ec
`addu a0,a0,v0` (Elza = Leon-Datei + 1). Die erzeugte Tabelle (15 Stellen, 11E1 msg 5 Gate (3,139), 11E0 gestrichen) wurde
gegen die Bytes gelesen und vom Riegel discard T (selbst gefahren) geprueft.

## 3. Nebenwirkungen (geprueft)
* Beide Szenarien: Elza ROOM1001 unveraendert (m1e, Riegel spind E); Stapelregel bei Elza (m2c); Leon-Statusschirm
  unveraendert (m3b, 100 % STPIC_00); Leon ROOM11E0 keine neue Abfrage (m4c, Riegel discard L, Integration H).
* Bit (9,0x86) in Elzas Spiel: `feuerloescher_r37.c k_lagen[]` kennt nur 0x1030 (Leon, Bit 0x86), 0x1090 (FEUER_AUS) und
  0x1091 (IMMER, kein Bit) -> Elzas Original-Munition in 1001 (Bit 0x86) kollidiert mit nichts (unveraendert seit r37/r38).
* AOT-Slot 11 in ROOM1000: kein SCD-Schreiber (1.2), Installer prueft `active` und bricht mit Logzeile ab, falls belegt
  (fail-closed); Idempotenz: zweiter Aufruf sieht in Slot 3 keinen Text-Satz mehr und tut nichts.
* Executor-Haken `re15_inventar_r39_paar` nur im Bestaetigungszweig (`exchange_match()` @ menu_common.c:1191, nach
  `re15_pad_virtual_word(pressed) & 0x4000`) — keine Vorschau, also kein vorzeitiges Umhaengen der Ausruestung.
* Granaten 0x09..0x0B: fallen jetzt unter Aufnahme-Stapeln, COMBINE (m2g 09:1 + 09:2 -> 09:3) und Kiste. Wortlaut "alles
  mit einer Zahl" deckt das; Entscheidungsfrage im Dossier (7) an den Nutzer gestellt. Im Spiel nur EINE Granaten-Ausgabe
  (Port ROOM1150), Wirkung also nur ueber die ITEM BOX.
* Halbierung bleibt am alten Helfer (`re15_pickup_stapelt_nutzer`, 0x21 ausgenommen) -> Memory Card weiterhin x3, nicht x1.
* STPIC-Nachladen: nur bei Indexwechsel; Index 0..15 geklemmt, alle 16 Dateien vorhanden -> kein Dauer-Nachladen/Logspam.
  Ein Figurwechsel im selben Prozess (Tod -> Titel -> NEW GAME andere Figur) wurde NICHT gefahren (nur Code gelesen:
  `stpic_refresh` vergleicht den Index je Bild und laedt den Block neu).
* PSX-Ziel: `inventar_r39.c` nutzt `fprintf`/`getenv` nur unter `RE15_PLATFORM_PC`; nicht gebaut (kein PSn00bSDK hier) —
  wie bei allen Spuren.
* Linux-Portabilitaet der neuen Riegel: `cmake_minimum_required` vorhanden, `file(COPY_FILE)` wie in 70 bestehenden
  Integrationsskripten, `m` auf Nicht-WIN32 gelinkt, keine .exe-/Backslash-Pfade, Text-Zaehler statt Regex. Nur statisch
  geprueft (Windows-Baum); die Suite faehrt der Orchestrator im Integrationsbaum.
* Maschinenlast waehrend der Abnahme: parallel liefen Riegel anderer Spuren (re15_pc_r39a_1050_*, re15_pc_r37p3_*,
  fremde ctest-Prozesse) — alle Laeufe hier sind bildgenau (RE15_EXIT_AT), Ergebnisse davon unabhaengig.

## 4. Hinweise (keine Abnahme-Maengel)
* H1 (P4): Der vom Nutzer beschriebene Ablauf ("discard gesagt, Karte noch da") kann auf v0.8.26 fuer die Yellow Keycard
  nicht stattgefunden haben (keine Abfrage vorhanden). Moeglich: Verwechslung mit Blue/Red Keycard (deren Abfrage seit
  Runde 25 existiert). Die Umsetzung erfuellt den Wortlaut (Abfrage nach dem Code, Ja = weg); der Orchestrator sollte dem
  Nutzer die Frage des Dossiers (OFFEN 1) mitgeben.
* H2 (P1): 15 statt 30 Schuss durch die Nutzer-Halbierung aller Welt-Aufnahmen; Entscheidungsfrage des Dossiers (7.1).
* H3 (P2): Granaten stapeln jetzt ebenfalls (Regel "alles mit einer Zahl"); Entscheidungsfrage des Dossiers (7.2).
* H4 (Doku): C_inventar.md traegt Abschnitt 2 vor Abschnitt 1 (Nummerierung), inhaltlich vollstaendig.
* H5 (Zweig): r39/inventar liegt hinter master (nur Doku/Orchestrator/make_package.sh von Spur B) — Merge durch den
  Orchestrator, keine Code-Ueberschneidung mit den master-Commits (geprueft: `git diff 3e49ff87 master -- re15_port` =
  nur `release/make_package.sh`).

## 5. Urteil
| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Spind ROOM1000 erst Munition, dann "Nothing useful" | erfuellt | m1a/m1b/t4 (Tuerweg): Druck 1 Modal 0x15, Druck 2 Ja 15:50 -> 15:65, Druck 3 msg 0 "Nothing unusual." genau 1x; Bit 69; Satzform ROOM1001 @0x00C24 |
| 2 Memory Cards / alles mit Zahl stapelbar | erfuellt | m2a 21:3 + 3 -> 21:6 ein Platz; m2b/m2c COMBINE 21:3 + 21:2 -> 21:5 (Leon + Elza); m2d Kiste 21:3 + 21:2 -> 21:5; m2g 09:1 + 09:2 -> 09:3; Aktion 5 @0x8004e664 (caseD_5), Obergrenze @0x80074DA8 |
| 3 Elza ohne Leons Bild/Karte | erfuellt | m3a + e1 (Neuspiel): STPIC_04 100 % Driver License Elza Walker; m3b Leon STPIC_00 100 %; DEBUG.BIN @0x800c01d0..f0, Tabelle @0x80073ad8 |
| 4 gelbe Karte nach "discard" weg | erfuellt | m4a: Abfrage nach "You've opened the lock." (Gate (3,139) @0x01A8A), Ja -> Inventar ohne 0x39; m4b Nein -> bleibt; m4c Leon unveraendert; H1 beachten |

Gates: Pfade/Vertrag halten, RE-Gate 29/29 Stichproben stimmen, Guess-Tells 0, Riegel je Punkt vorhanden und gruen
(4 + 4 + 52 Alt-Riegel), Nebenwirkungen auf beide Szenarien geprueft. **bestanden = true.**
