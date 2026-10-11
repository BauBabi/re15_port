# Runde 36 — Spur F "zombieki": Abnahme 6

Datum: 2026-10-11. Pruefer: unabhaengige Abnahme 6 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **654b27e0** (Basis master 78a4a748 = merge-base; master selbst steht bei 08aa6f96, nur
Dokumente der UEBERGABE). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md, Vorgaenger-Abnahme `F_abnahme_5.md`
(Stand e856447f: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3).

> **ERGEBNIS: BESTANDEN.** P1 erfuellt, P2 erfuellt, P3 erfuellt. Bau, gezielte Regression (232/232), Riegel (77/77), RE-Zitate (acht
> Stellen selbst disassembliert, inkl. Finisher @0x80118cc8-d68 und Kontaktcode-Mapping gegen das RE1.5-SCA-Layout), Sonden, exe-Messreihe
> (33 Laeufe, alle gy/st-Logs bitgleich zu den Laeufen des Bau-Agenten; gegen Abnahme 5 nur die erwarteten Stellen anders), Zufallsgang-
> Stichprobe (24/24 Zeilen bitgleich) und Vertrag halten. Die drei Maengel der Abnahme 5 sind behoben und selbst gemessen: Ostgasse
> (Gasse einzeln 2258 -> Angriff F157, O1 1368/1615 -> 0 / 248 + Anstehen, exe-Stelle 8 468 -> 0), Nachruecken (588 -> 105 Bilder nach dem
> Tod; exe nw12_schuss 532 -> 141), Dossier berichtigt. J-Fenster bitgleich, P1/P3 unveraendert. Offen (dokumentiert, kein Mangel):
> Flugklemme beim Sprung aus der Pad-Mitte (Stelle 7, vorbestehende Klasse), Zufallsgang Anker 400 +8 % bei Stillstand 125 -> 0.

## 0. Stand, Bau, Messmittel

- `git log --oneline master..HEAD`: Abnahme-5-Stand (8030e41f) + 13 Commits der Nachbesserung 6 (bb5a7507 .. 654b27e0, s. Behauptung
  des Bau-Agenten). Eigener Diff des Zweigs = `git diff 78a4a748..HEAD` (ein blankes `git diff master` zeigt die seit dem Abzweig in
  master gelandeten Dateien — synchro/STAGE1/room10xx.. — als "geloescht"; nicht aus diesem Zweig).
- Code seit Abnahme 5 (`git diff 8030e41f..HEAD --numstat -- re15_port`): `zombieki_r36.c` +282/-62, `re15_zombieki_r36.h` +25
  (nur Kommentar/Prototypen), `platform/pc/main.c` +1 (Messschienen-Haken `re15_r36_affen_log_tick()`, hinter `re15_trage1200_mess()`),
  `probes/r36_zombieki.cmake` +6 (Kommentar), Riegel `test_r36_zombieki_affen_block.c` +142/-15. Gemeinsame Dateien
  enemy_ai_common.c / enemy_ai_re2_zombie.c seit Abnahme 4 unveraendert; P1-/P3-Funktionen unveraendert.
- Der Code-Diff wurde vollstaendig gelesen (zombieki_r36.c: `s_r36_nb6`-Bits, Nachruecken in `re15_r36_affen_fest_takt`, `r36_weg_folgen`
  (aus `r36_uw_route` herausgeloest, Rueckgabe/Zielwahl unveraendert; Zusatz nur unter `nah`), `r36_bissplatz`, `r36_seiten_code`,
  `r36_am_pad`, `r36_nah_route` (Grund 1..5/10..14), `r36_pad_sprung_code`/`r36_pad_sprung` (aus dem bisherigen Inline-Sprung
  herausgeloest, Original-Schreibfolge +0x9f/+0x5/+0x6/+0x7 unveraendert), Haken in `re15_r36_affen_padsuche_decide` (m == 2/3 neu,
  Fuss-Weg erst ab R36_NAH_START), Sonden-Leser, Messschiene `re15_r36_affen_log_tick` (nur PC, nur bei gesetztem
  `RE15_AFFEN_LOG`, schreibt, aendert nichts)).
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to do",
  `LOCAL-BUILD-OK (build)`. Baum sauber (`git status` leer), exe `re15_pc.exe` 16:34:53 (Quellen: zombieki_r36.c 13:03, Header 16:23,
  HEAD-Commit 16:44 enthaelt nur Dossier/Belege) => exe = HEAD, md5 **4e2b8d37cd90b6c943328dadb0443fa3** (= Angabe des Bau-Agenten).
  Messkopie `re15_pc_abn6f.exe` (gleiche md5). Vergleichs-exes: `re15_pc_nb6l.exe` c76387e9… (Bau 30c2ecb3, Messreihe des Bau-Agenten;
  Diff 30c2ecb3..HEAD -- re15_port = nur Header-Kommentar), `re15_pc_abn5f_neu.exe` 714f8527… (Abnahme-5-Stand), `re15_pc_abn0f_basis.exe`
  2fd81d20… (v0.8.23). Rohdaten der Abnahme 5 (`F_abn5/`) und des Bau-Agenten (`F_nb6l/`, `F_nb6m/`) im gemeinsamen Scratch, nur gelesen.
- Eigene Laeufe: Scratch `F_abn6/` (`run.sh` = Muster F_abn5, je Lauf eigener Prozess und Ordner; `driver.sh` 6 Warteschlangen + r1010a;
  Kommandozeilen `jobsA6.txt`/`jobsB6.txt`/`jobsR6.txt` = die 33 Laeufe der Abnahme 5 mit `abn6f`; `sonden.sh` = Sonden-Laeufe des
  Riegel-Binaries; `ausw5.py` = Auswerter der Abnahme 5, unveraendert).

## 1. RE-Gate (Stichprobe selbst disassembliert, richtige Binaerdatei)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`; STAGE1.BIN (`info/Re1.5/PSX/BIN/`, md5 a482d196…) roh @0x80100000;
PSX.EXE (md5 b55fdaa5…). Alle im NB6-Diff/-Dossier zitierten Stellen gelesen:

| Zitat (Code/Dossier) | Datei | gelesen | Urteil |
|---|---|---|---|
| Pad-Box-Abfrage A[4] Path A: `ori a3,zero,0x10` @0x80117ee0, `lbu a2,130(v1)` (+0x82) @0x80117ee4, `lhu a1,6(v0)` (box[6]) @0x80117ef4, `addiu a0,a0,52` (+0x34) @0x80117ef8, `jal 0x8003b93c` @0x80117efc, `addiu a1,a1,100` @0x80117f00 | STAGE1.BIN | Wort fuer Wort so; davor `lbu v0,482(v1)` (+0x1e2) / `beq v0,zero,0x80118028` @0x80117ed4-dc | stimmt |
| Yaw-Fenster @0x80117f18-38 | STAGE1.BIN | `lbu a1,144(a0)` / `lh v1,106(a0)` / `andi v0,a1,0xf0` / `sll v0,v0,4` / `subu v0,v0,v1` / `addiu v0,v0,512` / `andi v0,v0,0xfff` / `slti v0,v0,1024` / `beq v0,zero,0x80117f78` | stimmt (Port `r36_pad_sprung_code`: `(((zc&0xf0)<<4) - rot_y + 512) & 0xfff < 1024`) |
| Sprung +0x9f/+0x5..7 @0x80117f40-74 | STAGE1.BIN | `sb a1,159(a0)` @0x80117f40 (+0x9f = +0x90), `ori v0,zero,0x7` @0x80117f3c / `sb v0,5(v1)` @0x80117f50, `sb zero,6(v0)` @0x80117f60, `ori v0,zero,0x1` @0x80117f6c / `sb v0,7(v1)` @0x80117f74 | stimmt |
| Kontaktcode FUN_8001bf04: 0xf00 @0x8001bf74, 0x000 @0x8001bfa4, 0x800 @0x8001bfac, 0xc00 @0x8001bfe8, 0x400 @0x8001bfec, `jr ra`/`addu v0,t2,zero` @0x8001bff0-f4 | PSX.EXE | alle fuenf `ori/addu t2` an den genannten Adressen; Klassifikation: x-Code aus signbit(px-(X+W)) und signbit(px-X) (`lh a1,4(a2)` = X, `lhu v0,0(a2)` = W — RE1.5-SCA-Layout W@0, D@2, X@4, Z@6, re15_rdt.h), Code 3 -> 0x000, 0 -> 0x800, 2 -> dasselbe fuer Z (3 -> 0xc00, 0 -> 0x400, sonst 0xf00); Decompile FUN_8001bf04.c deckungsgleich | stimmt; `r36_seiten_code` rechnet exakt so (= Engine `coll_contact_dir`) |
| `sb v0,144(a0)` (+0x90) = (Richtung >> 4) + 8 + (u1 & 3) @0x8003ba8c | PSX.EXE | `sra v0,v0,4` / `addiu v0,v0,8` / `lbu v1,0(s0)` / `andi v1,v1,0x3` / `addu v0,v0,v1` / `sb v0,144(a0)` @0x8003ba70-8c; davor in FUN_8003b93c die Box-Pruefung (px - (X - r)) <u W + 2r @0x8003ba1c-34 (X = `lh -6(s0)`, s0 = Zelle+10; W = `lhu 0(s1)`), Z analog @0x8003ba40-58, `jal 0x8001bf04` @0x8003ba68 mit a2 = Zelle | stimmt (Port `r36_am_pad` = dieses Rechteck um r = hit_radius_min + 100) |
| Abstand +0x1d4 `jal 0x80065f60` (SquareRoot0) / `sh v0,468(v1)` @0x80117300-14 | STAGE1.BIN | so | stimmt (Nachruecken prueft `dog_dist < 0xbb8`) |
| Zustandstabelle @0x801213c8: [3] 0x8011b6fc DEATH, [7] 0x8011be54 CORPSE; Wurzel `lbu v0,4(v1)` / `sll v0,v0,2` / `lui at,0x8012` / `addiu at,at,5064` / `lw v0,0(at)` / `jalr v0` @0x80116e00-20 | STAGE1.BIN | `table 0x801213c8 8`: [0] 0x80116f50, [1] 0x80117254, [2] 0x8011af5c, [3] 0x8011b6fc, [4]/[5] 0x8011bdec, [6] 0x8011c598, [7] 0x8011be54; Wurzel so | stimmt (Nachruecken: `q->state == 3 \|\| == 7`) |
| Finisher aus dem Flug @0x80118cc8-d68 (Bild 0x13, Leon HP < 50, +0x93 == 0, a804(0x1388,0xc0), +0x1dc == 0 -> Sub 8, Se(7)) | STAGE1.BIN | `lbu v1,149(v0)` / `ori v0,zero,0x13` / `bne` @0x80118cd4-dc; `lh v0,0(a2)` (player hp) / `slti v0,v0,50` @0x80118cec-f4; `lbu v0,-13593(v0)` (+0x93) / `bne` @0x80118d04-0c; `ori a0,zero,0x1388` / `ori a1,zero,0xc0` / `jal 0x8001a804` @0x80118d10-18; `bgez v0` @0x80118d20; `lh v0,476(v1)` (+0x1dc) / `bne` @0x80118d34-3c; `ori v0,zero,0x8` / `sb v0,5(v1)` @0x80118d40-44; `sb zero,6(v0)` @0x80118d54; `ori a0,zero,0x7` / `jal 0x800453d0` / `sb zero,7(v0)` @0x80118d60-68 | stimmt (Dossier-Aussage zu Stelle 7 belegt) |
| Flug: +0x7 != 0 -> Yaw = Code, Band +0x82 @0x80118ac0-af0 | STAGE1.BIN | `lbu v0,7(v1)` / `beq` @0x80118ac0-c8, `lbu v0,159(v1)` / `andi 0xf0` / `sll 4` / `sh v0,106(v1)` @0x80118ad0-e0, `sb a0,130(v0)` @0x80118af0 | stimmt |

- NB6-Diff (`git diff 8030e41f..HEAD -- re15_port`, nur `+`-Zeilen) nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/
  approx/schaetz/ungefaehr/vorerst/vorlaeufig/"sieht richtig"/geraten/guess: **0 Treffer**. `getenv` nur in der Messschiene
  `re15_r36_affen_log_tick` (`RE15_AFFEN_LOG`, schreibt eine Datei, liest keinen Zustand zurueck — kein Verhalten; s. 3.4 exe-Gegenprobe).
  Schalter `re15_r36_affen_nb6_enable` (neu) hat in engine/ und platform/ keinen Aufrufer (nur Sonden; Default `s_r36_nb6 = 3`).
- Stellgroessen der NB6: `R36_FEST_START` (= 45, NB3) auch als Beginn des Nah-Umwegs ueber ein Pad, `R36_NAH_START = 354` fuer den
  Fuss-Weg — im Code, Header, Dossier (NB6 OFFEN 4) und Commit als PORT-WAHL gekennzeichnet, mit Messreihe (354 auch fuer das Pad: O1
  549/888; 45: 0/248, `nb6_lage_vorher_nachher.txt`). "Genau einmal je Pad-Aufenthalt" (Grund 14, `s_r36_g14_pad1`) ist PORT-WAHL mit
  gemessener Begruendung (NB6c: Sprung alle ~85 Bilder auf der Stelle an Stelle 7; vier Varianten NB6d-g gemessen) — kein Zahlenwert.

## 2. Sonden (Riegel-Binary `test_r36_zombieki_affen_block.exe`, je eigener Prozess, `F_abn6/sonden.sh`; Modus 1 = PORT (NB6-Bits 3), Modus 6 = Stand NB5)

| Lage (Kommandozeile der Abnahme 5 / des Riegels) | Stand NB5 (Modus 6) | HEAD (Modus 1) | Kriterium Abnahme 5 |
|---|---|---|---|
| Gasse einzeln `lage 2400 <m> 30 7873 -1412 0 6418 -4333 3373 3 0 -9013 -15461 2656 0 0` | G1 kein Angriff, NAH fest **2258** (fest 2291, fern 33), `frei 0` am Ende (Leon von G2 nach (11986,-693) geschoben) | G1 Angriff **F157** .. F2387 (965 Angriffsbilder), NAH fest vor dem ersten Angriff **0**, fest 0 | <= 450: **erfuellt** |
| O1 `lage 2400 <m> 30 6000 -9000 1332 -5525 -14883 2731 3 0 -8915 -12487 93 3 0` | G1 Angriff F195, letzter F1032, NAH fest **1368**; G2 nie, NAH fest **1615** | G1 F195 .. F2391, NAH fest 0; G2 Angriff **F937** nach **248** NAH fest, danach 323 Bilder Anstehen (`frei 0`: kein freier Bissplatz-Knoten, Leon in der Ecke (11159,-1181) hinter G1) | <= 450 oder nachweislich Anstehen: **erfuellt** |
| Stelle 8 verkeilt `lage 900 <m> 30 7320 -7071 0 6418 -3617 857 3 0 -9013 -15461 2656 0 0` | G1 kein Angriff, 900/900 NAH fest, Grund 1 (kein Startknoten) | G1 Angriff **F112** .. F867, NAH fest 0, erster Nah-Umweg-Grund **13** (verkeilt -> Sprung mit dem Seitencode) | **erfuellt** |
| Nachruecken `lage 1200 <m> 1 -26606 14578 18 -23302 14489 2067 3 0 -20579 16118 1885 4 0 400 2` (G1 F400 getoetet: Zustand 3 F400, Leiche 7 F470) | G2 Angriff nach dem Tod erst **F988** (= 588 Bilder) | G2 Sub 5 **F505** (= **105** Bilder nach dem Tod; Spur je Bild: `F505 \| G2 -24523 15926 … s1/5/1/0 … ng10`) | <= 450: **erfuellt** |
| kanon (J-Fenster) `kanon 0 1 1 <bits>` mit Bits 3 / 0 / ohne Nah-Umweg (`kanon 0 1 0 3`) | — | alle drei: Freigabe F1066, Tod +1194, Bisse 363 468 521 571 624 674 727 778 830 882 933 986 1036 1090 1139 1194, Slot-2-NAH-Anker vor dem Angriff **353** — **bitgleich** zu `F_belege/nb6/nb6g_kanon.txt` und zur eigenen Messreihe der Abnahme 5 | unveraendert: **erfuellt** |

- Die Spuren `lage_gasse_einzeln_m1_spur30`, `lage_o1_m1_spur30`, `lage_stelle8_m1_spur30` sind byteweise gleich (`cmp`) zu den Dateien
  des Bau-Agenten `F_belege/nb6/nb6g_lage_{gasse,o1,stelle8}_m1_spur30.txt`; Nachruecken: LAGE-Zeilen gleich (`nb6g_lage_nachruecken_m1.txt`
  traegt keine Spur). Die Dossier-Zahlen (157; 195 / 937 nach 248; 112 Grund 13; 505 = 105; NB5 2258 / 1368+1615 / 900 / 988) stimmen.
- Riegel direkt `test_r36_zombieki_affen_block riegel` (`F_abn6/riegel_affen_block.txt`): **77 Pruefungen, 0 Fehler**, RC 0. Darin die neuen
  Pins: Nachruecken Gegenprobe Stand NB5 "Angriff erst Bild 988, 588 Bilder nach dem Tod" (> 850), NB6 "Bild 505 = 105 (<= 450)"; Gasse
  einzeln / O1 je Gorilla "NAH fest vor dem ersten Angriff <= 450, danach <= 450 oder steht an (freie Bissplatz-Knoten 0)" mit Gegenprobe
  Stand NB5 (>= 1000 NAH fest, in den letzten 1000 Bildern kein Angriff); Stelle 8 verkeilt (Gegenprobe 900/900, NB6 Angriff 112, Grund 13);
  J-Fenster `nahv == 353 && tod == 1194 && gl == 15`. Der Riegel prueft damit die Schranke 450 (Mangel 2 der Abnahme 5: "nicht nur `> 400`").

## 3. exe-Messungen (HEAD `re15_pc_abn6f.exe`, die 33 Kommandozeilen der Abnahme 5, je Lauf eigener Prozess und Ordner, `F_abn6/jobs*.txt`, `runs.log`)

Basis-Haken je Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log` (`run.sh`).
Auswertung `ausw5.py` (Abnahme 5, unveraendert; `F_abn6/auswertung.txt`). Vergleich byteweise (`cmp`) gegen die Laeufe des Bau-Agenten
(`F_nb6l/*_nb6l`, exe c76387e9… = Bau 30c2ecb3) und der Abnahme 5 (`F_abn5/*_abn5f_neu`, e856447f).

### 3.1 Bitgleichheit
| Lauf | gegen Bau-Agent nb6l | gegen Abnahme 5 |
|---|---|---|
| 14 Gorilla-Stellen | gy.log **alle 14 gleich** | 11 gleich; Stellen 3 (6000,-9000), 7 (-14746,-4140), 8 (7200,-4130) anders (NB6 wirkt, wie im Dossier) |
| t11b0kampf, t11b0szene, szene11c0 (kanonisch), flucht_west | gy + st **gleich** | gy + st **gleich** |
| nw12_schuss, nw4_schuss, nw13_schuss, t11b0kampf_schuss | gy + st gleich | anders (NB6 wirkt, wie im Dossier) |
| nw11_schuss, stelle3_schuss, t11b0szene_schuss | gy + st gleich | gy + st **gleich** |
| P1: r1040w, r10b0, tuer1010, tuer1040 (+st), t1220a (+st), t1220c (+st) | gleich | **gleich** |
| P3: p3lache | gy + st gleich | gy + st gleich, **51 von 51 Framedumps (.ppm) gleich** |
| r1010a (rc 124 = Zeitlimit 900 s wie in allen Abnahmen) | R1010-Teil (780 Zeilen `grep " R1010 "`) gleich zur Wiederholung des Bau-Agenten `F_nb6m/r1010a_nb6l` | R1010-Teil **780 Zeilen gleich** (`F_abn5/r1010a_head_1010.txt`) |

### 3.2 Punkt 2 — 14 Stellen (Kampf-Layout `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700`, keine Eingabe)

| # | Leon | Abnahme 5: erster Angriff S2 / S3, fest S2 / S3 (FERN) | **HEAD** (selbst gemessen) | laengste Strecke HEAD |
|---|---|---|---|---|
| 1 | (-7000,12500) | F1627 / F730, 341 / 250 | gleich | S2 F1254+341 (-13527,11922) d3556..3673 |
| 2 | (-14000,2000) | F680 / nie, 0 / 0 | gleich | — |
| 3 | (6000,-9000) | F1596 / F324, 429 (45) / 368 | **F1651 / F324, 271 (0) / 368** | S2 F1048+152, F1278+119; S3 F826+368 (5771,-8824) |
| 4 | (-24000,10000) | F853 / F1570, 0 / 1193 (1) | gleich | S3 F1781+920 (-23302,13088) (Anstehen Tasche) |
| 5 | (7000,4000) | nie / F18, 96 (96) / 0 | gleich | S2 F396+96 FERN |
| 6 | (9000,10000) | F1175 / F309, 0 / 0 | gleich | — |
| 7 | (-14746,-4140) | F364 / F1021, 179 / 108 (108) | **F364 / nie, 0 / 108 (108)** | S3 F406+108 FERN (6418,1676) — S2 fest 0, Leon stirbt F1204 (s. 3.4) |
| 8 | (7200,-4130) | F1050 / F274, 0 / 468 | **F1204 / F274, 0 / 0** | — (Grund-13-Sprung F693, s. 3.4) |
| 9 | (-14000,4000) | F704 / F1496, 250 / 0 | gleich | S2 F1913+250 |
| 10 | (-14500,6000) | F694 / F892, 0 / 0 | gleich | — |
| 11 | (-24452,11938) | F828 / F1253, 0 / 1335 | gleich | S3 F1366+1335 (-23061,13333) (Anstehen) |
| 12 | (-22288,11938) | F868 / F1025, 0 / 833 (50) | gleich | S3 F2204+472 (-23302,13082) (Anstehen) |
| 13 | (-26500,15500) | F842 / nie, 0 / 1650 | gleich | S3 F1065+1518 (-23302,13217) (Anstehen) |
| 14 | (-20000,14500) | F883 / F896, 0 / 0 | gleich | — |

Summen fest (FERN / NAH): v0.8.23 25936 (19812 / 6124) -> Abnahme 4 10466 (255 / 10211) -> Abnahme 5 7500 (300 / 7200) -> **HEAD 6695 (255 / 6440)**
(aus den eigenen Rohdaten mit `ausw5.py` nachgerechnet; Dossier-Zahlen stimmen). Weder Gesamt-Blockade noch FERN steigt; der NB5-Zuwachs
FERN +45 an Stelle 3 ist weg. Kanonischer Kampf: szene11c0 S2 398, t11b0szene S2 408 (= Original, bitgleich); t11b0kampf S2 1309 NAH
(Anstehen Nordstreifen) / S3 F866 — bitgleich zu Abnahme 5.

### 3.3 Punkt 2 im echten Ablauf — Leon wehrt sich (`RE15_GIVE=7:250 RE15_EQUIP=7 RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6)x110,W1`, Kommandozeilen Abnahme 5 Abschnitt 4)

| Ablauf | Abnahme 5: erster Angriff S2 / S3, fest (Art) | **HEAD** |
|---|---|---|
| nw12_schuss (Leon (-22288,11938)) | F868 / F1144; **S2 532 NAH** (F1464+141, F1605+391) / 0 | F868 / F1144; **S2 141 NAH** (nur F1464+141, VOR dem Tod von S3 bei F1620; danach Nachruecken) / 0 |
| nw4_schuss | F853 / F1342; 0 / 105 | F853 / F1357; 0 / 105 (F1028+105 hinter S2) |
| nw11_schuss | F828 / F1291; 0 / 0 | gleich |
| nw13_schuss | F840 / F1623; 0 / 341 | F840 / **F1509**; 0 / **249** (F1020+249, Anstehen hinter S2) |
| stelle3_schuss | F1002 / F324; 0 / 0 | gleich |
| t11b0kampf_schuss (echter Tuerweg 11B0 -> 11C0) | F1416 / F862; 0 / 0 | F1415 / F862; 0 / 0 |
| t11b0szene_schuss (echter Tuerweg, Ankunftsszene) | F1276 / F1434; 0 / 0 | gleich |

Kriterium Mangel 2 in der exe: nw12_schuss kein Gorilla > 450 Bilder NAH fest — **erfuellt** (141 / 0). In allen sieben Ablaeufen greifen
weiterhin beide Gorillas an (v0.8.23: in 3/5 einer nie).

### 3.4 Messschiene `RE15_AFFEN_LOG` (neu in NB6; Behauptung "kein Verhalten, gy.log bitgleich") und die Spuren der Stellen 7 und 8
- Stelle 7 (`affen_-14746,-4140` + `RE15_AFFEN_LOG=affen.log`): gy.log **byteweise gleich** zum Lauf ohne Messschiene. Stelle 8 ebenso
  (`stelle8_log2`). Der ERSTE Stelle-8-Lauf mit Messschiene war unbrauchbar: `debug.log` zeigt waehrend des Laufs `[pad] Controller
  abgezogen` + erneute Erkennung "Xbox 360 Controller", gy.log weicht ab F11 ab (Leon laeuft 50 Einheiten/Bild, d. h. echte Pad-Eingabe
  ohne RE15_INPUT_SCRIPT) — exakt das Artefakt, das der Bau-Agent fuer seinen r1010a-Lauf beschreibt (`nb6_r1010a_controller.txt`);
  Wiederholung ohne Controller-Ereignis -> gleich. Alle anderen 33 Laeufe ohne Controller-Ereignis (`grep abgezogen */debug.log`: nur
  der verworfene Lauf).
- **Stelle 7**, Slot 2 (eigene Spur, 23 von 23 Zeilen des Bau-Agenten-Auszugs `nb6_stelle7_affenlog_nb6g.txt` an denselben Bildern
  bitgleich, gy = aff - 241): gy F915 NAH-Anker 45 mit der Mitte in Pad #47 (-16251,-3490), Leon (-15759,-6944) suedlich (dd 3491) ->
  **F916 Grund 14**, 9f=48 (Nordseiten-Code -> Richtung Sueden), Sub 7 (+0x7 = 1); Flug F925..F951 von (-16360,-3590) nach (-16391,-3618)
  = **ohne Ortswechsel** (Flugklemme); **F934 Sub 8** (der Original-Finisher @0x80118cc8-d68, s. Abschnitt 1); F955 Sub 4; F1001 Grund 11
  (zu Fuss zum Startknoten (-14600,900)), F1071 Grund 12 (Anlauf), F1085 FERN -> FERN-Umweg. Leon (ohne Eingabe) stirbt F1204 (Abnahme 5:
  F1636; dort S2 179 NAH fest). Die Dossier-Darstellung (NB6d-g, OFFEN 1) ist damit bestaetigt.
- **Stelle 8**, Slot 3: gy F623..F647 Biss aus Pad #36 heraus bei (6418,-3617), Leon dabei nach (7320,-7071) geschoben; F648 Sub 3 NAH
  (dd 3565); **F693 (= 45 Bilder = R36_FEST_START) Grund 13**, 9f=48 -> Sub 7; Flug bis (6418,-10638) F728 (echter Ortswechsel nach
  Sueden); F732 Sub 4; **F759 Sub 6 Angriff**. Der 468-Bilder-Stillstand der Abnahme 5 (F648+468) ist weg (0). Hinweis: die Dossier-Zeile
  "Stelle 8: S3 468 -> 0 (Angriff F274, Grund 13 ...)" fasst zwei Dinge zusammen — F274 ist der ERSTE Angriff von S3 (seit Abnahme 5
  unveraendert, ohne Pad-Sprung, `ng 0`), der Grund-13-Sprung kommt F693 mit Angriff F759.

### 3.5 Punkt 1 — Zombies (P1-Code seit Abnahme 2 unveraendert)
- Alle P1-Laeufe bitgleich zu Abnahme 5 (3.1): ROOM1040 Westgang, ROOM10B0 Terrasse, echte Tuerwege 1020->1010, 1020->1040, 1210->1220
  (aot1/aot3), r1010a (Abnahme-1-Fall, R1010-Teil 780 Zeilen).
- Eintritts-Zensus am HEAD selbst gefahren (`test_r36_zombieki_eintritt mess`, 78 Tueren, `F_abn6/eintritt_mess_head.txt`): **266 Faelle,
  PASSIV 0, KLEMME 0** (= Abnahme 5).

### 3.6 Punkt 3 — Blutlache (Code seit Abnahme 2 unveraendert)
- exe p3lache (Kommandozeile Abnahme 5, `RE15_FORCE_CUT=5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=200-700/10:fd_`): gy.log, st.log und **alle 51
  Framedumps byteweise gleich** zu Abnahme 5 (dort angesehen und gezaehlt: Lache 292 -> 494 dunkelrote Pixel ueber ~90 Bilder ab dem
  ersten Leichentakt 0x5a @0x8011be80, dann konstant; RE @0x8011bed0-bf14).

### 3.7 Zufallsgang (Riegel-Binary, `eins <i> 1 6000 400 0 1 1 <bits>`, Seeds i = 0..11, je eigener Prozess, `F_abn6/seeds12_bits{3,0}.txt`)
- Bits 3 (NB6) und Bits 0 (Stand NB5): **24 von 24 Zeilen bitgleich** zu den Dateien des Bau-Agenten `F_belege/nb6/seeds/zg_a400_nb6g_{3,0}.txt`
  (je Seed verglichen). Stichproben-Summen: Bits 3 blockiert 4921 / Griffe 70, Bits 0 4278 / 62. Die 52-Seed-Summen des Dossiers (17655 / 0 /
  252 gegen 16352 / 125 / 242) wurden nicht neu gerechnet (Stichprobe); die Erklaerung der zwei Zuwachs-Seeds (vorbestehende
  Sprungschleifen-Klasse, auch im Original-Modus) steht mit Spur je Bild im Dossier (`zg_seed18_*`, `zg_seed27_*`).

## 4. Abarbeitung der Maengel der Abnahme 5

| Mangel Abnahme 5 | Kriterium | Stand 654b27e0 (selbst gemessen) |
|---|---|---|
| 1 Ostgasse: naher Gorilla hinter dem 0x10-Pad #36 steht unbegrenzt | beide Sonden-Lagen: jeder nahe Gorilla greift nach <= 450 NAH fest an oder steht nachweislich an; exe-Stelle 8 S3 <= 450; J-Pins + J-Fenster unveraendert; Riegel mit Gegenprobe | **behoben**: Gasse einzeln G1 Angriff F157 (NAH fest vor dem Angriff 0; NB5 2258 ohne Angriff); O1 G1 F195 / G2 F937 nach 248, danach Anstehen `frei 0` (NB5 1368/1615 ohne Angriff); Stelle 8 verkeilt Angriff F112, Grund 13 (NB5 900/900); exe-Stelle 8 S3 **0** (468); `unit_r35_affen_*` 20/20, kanon bitgleich (363 468 521 …, Tod +1194, Anker 353); Riegel-Pins mit Gegenprobe Stand NB5 (Abschnitt 2) |
| 2 Wartezeit zaehlt nach dem Tod des Partners / Gleiten neu | Nachruecken <= 450 nach dem Tod; exe nw12_schuss kein Gorilla > 450 NAH fest; J-Fenster bitgleich; Riegel prueft die Schranke | **behoben**: Nachruecken G2 Biss F505 = **105** nach dem Tod (NB5 988 = 588); exe nw12_schuss S2 **141** / S3 0 (532); kanon bitgleich; Riegel `ag[1] <= 400 + 450` + Gegenprobe NB5 `> 850` |
| 3 Dossier: NB5-OFFEN "Gleiten" falsch, Nutzertext unvollstaendig | beide Stellen nach Messung berichtigt | **behoben** (gelesen, Dossier Z. 1274-1283 und 1308-1313): "Gleiten" durchgestrichen und mit der eigenen Messung (Zaehler 354 in F776 / 478 in F900, Grund 2 in 124/124 Bildern, G2 belegt den Streifen, `platz` mit Ausschluss = 0) als ANSTEHEN berichtigt; "Gassenmitte" berichtigt (beide Gorillas, 1368/1615, ~1530); "Fuer den Nutzer" mit der Verzoegerung (588 / ~375 / 532) und der Gassenmitte (~1530) nachgetragen |

## 5. Gezielte Regression (keine Voll-Suite im Spurbaum)

`ctest --test-dir re15_port/build -R "<Regex der Abnahme 5, F_belege/nb6/regex_abnahme5.txt>" --timeout 600 -j 4 --output-on-failure`
(lief parallel zur exe-Messreihe und zum Riegel) -> **100% tests passed, 0 tests failed out of 232** (Total Test time (real) = 506.96 sec,
RC 0; `F_abn6/ctest_gezielt.txt`). Darin alle 6 Spur-Tests (`unit_r36_zombieki_{wecker,szenen,affen,sicht,affen_block,eintritt}`), alle
20 `unit_r35_affen_*` (Spur J, bildgenau gegen GDB), `unit_maggot_*`, `integration_r35_cut1150`, `integration_r35_zgirl`. Kein Fenster-
Haken rot, kein Nachfahren noetig. Riegel direkt 77/77 (Abschnitt 2), Zensus 266/0/0 (3.5).

## 6. Vertrag

- Pfade (`git diff 78a4a748..HEAD --name-only`): analysis/befunde_runde36/F_* (+ F_belege/), engine/src/{enemy_ai_common.c,
  enemy_ai_re2_zombie.c, zombieki_r36.c}, include/re15_zombieki_r36.h, platform/pc/main.c, tests/unit/probes/r36_zombieki.cmake,
  tests/unit/test_r36_zombieki_* — **keine** release/, platform/android/, shared_assets/PSX/, Test-CMakeLists, RELEASE_NOTES.
- Gemeinsame Dateien (numstat Zweig): enemy_ai_common.c +32/-11 und enemy_ai_re2_zombie.c +2/-1 seit Abnahme 4 unveraendert (Spur F =
  Eigentuemer der Sichtfilter-Haken, VERTRAG 1.4); main.c +5 gesamt, davon NB6 **+1 Zeile** (Messschienen-Haken, nur env, kein Verhalten,
  3.4).
- zombieki_r36.c / Header: kein `flag_set`/`msg_install`/`scd_event_fire`/`re15_flag` (grep 0) => keine Bank-9-Bits, Nachrichten-IDs,
  Ereignisse. Tests nur ueber `probes/r36_zombieki.cmake` (6 Tests, Registrierung in NB6 unveraendert; +6 Kommentarzeilen).
- Nebenwirkungen: NB6-Code nur im Gorilla-Zweig (Typ 0x27; `re15_r36_affen_fest_takt`/`_padsuche_decide`), Nachruecken filtert
  `type == 0x27`; Zombie-Raeume bitgleich (3.1, 3.5), kanonischer Gorilla-Kampf (Sprung + echter Tuerweg) bitgleich, P3 bitgleich. Elzas
  Parkplatz ROOM11C1 hat keine Gorillas (Abnahme 2/3; eigene Zaehlung der `Sce_em_set`-Typbytes 0x27 im RDT: 11C0 = 4, 11C1 = 0).
- Commits: Konstanten mit `@0x…` im Code und in 654b27e0; PORT-WAHL als solche gekennzeichnet; keine Backtick-Probleme.

## 7. Gates

- **Bau:** OK (Abschnitt 0; "ninja: no work to do", exe = HEAD, md5 = Angabe des Bau-Agenten).
- **Gezielte Regression:** 232/232 (Abschnitt 5); Riegel direkt 77/77.
- **RE-Gate:** acht Zitate selbst disassembliert (Abschnitt 1), alle stimmen; Kontaktcode-Mapping gegen das RE1.5-SCA-Layout (W@0, D@2, X@4,
  Z@6) und den Decompile geprueft; 0 Guess-Tells; neuer Schalter nur Sonde; Stellgroessen als PORT-WAHL mit Messreihe.
- **Fix erklaert den Befund?** Ja, je Mangel: (1) Ursache gemessen (Nah-Umweg sah nur die eigene Fuss-Komponente; verkeilt in der Pad-Mitte
  kein Startknoten; an der Pad-Kante lag der Raster-Startknoten 1970 ausserhalb der Gasse -> FERN-Kipp) -> Fix (Bissplatz auf Leons Seite
  ueber Pad-Kanten, Sprung = Original-Pad-Sprung; verkeilt -> Seitencode; "am Pad" = Box-Abfrage des Originals) -> Wirkung (2258 -> 157,
  900 -> 112, 1368/1615 -> 0/248, exe 468 -> 0, Stelle 3 S2 nie -> F1651). (2) Ursache (Anker-Resets durch Gleiten nach dem Tod, Zaehlung von
  vorn) -> Fix (Tod in Bissweite laesst die Wartezeit der NAH Wartenden ablaufen, Anker = Lage) -> Wirkung (588 -> 105; nw12 532 -> 141).
- **Vertrag:** OK (Abschnitt 6).
- **Volle Suite:** im Spurbaum nicht gefahren (Auftrag) — Orchestrator im Integrationsbaum.

## 8. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Zombies bleiben ewig beteiligungslos stehen, fast vor der Nase, auch innerhalb eines Raumes | **erfuellt** | P1-Code seit Abnahme 2 unveraendert; alle P1-Laeufe (1040 Westgang, 10B0 Terrasse, Tuerwege 1020->1010 / 1020->1040 / 1210->1220 aot1+aot3, r1010a) bitgleich zu Abnahme 5; Eintritts-Zensus am HEAD 266 Faelle, PASSIV 0, KLEMME 0. |
| 2 Monkeys ROOM11C0 oft blockiert — verbessern | **erfuellt** | Die beiden in Abnahme 5 offen gemessenen Stillstaende sind weg: Ostgasse (Gasse einzeln 2258 ohne Angriff -> Angriff F157; O1 1368/1615 -> 0 / 248 + Anstehen `frei 0`; exe-Stelle 8 468 -> 0) und Nachruecken (588 -> 105; nw12_schuss 532 -> 141). 14 Stellen fest 25936 (v0.8.23) -> 6695 (FERN 255), mit Gegenwehr 7/7 Ablaeufe beide Gorillas; Zufallsgang Stillstand (Anker 40) 125 -> 0 (Bau-Agent, Stichprobe bitgleich). Nordwest-Tasche: Anstehen geometrisch erzwungen (Abnahme 5, 2.1). Dokumentierte Grenzen (OFFEN, kein Mangel): Flugklemme beim Sprung aus der Pad-Mitte (Stelle 7, vorbestehend), Anker 400 +8 %. |
| 3 Blut der toten Monkeys laeuft nicht aus | **erfuellt** | exe p3lache: gy.log, st.log und 51/51 Framedumps bitgleich zu Abnahme 5 (dort angesehen: Lache 292 -> 494 Pixel ueber ~90 Bilder, 0x5a @0x8011be80); Code seit Abnahme 2 unveraendert. |

## 9. Maengel

Keine.

## 10. Hinweise (kein Mangel)

(a) **Flugklemme / Zurueckschnappen** (Dossier NB6 OFFEN 1): an exe-Stelle 7 springt S2 einmal aus der Mitte von Pad #47 (Grund 14) und
bleibt im Flug am Start haengen (gemessen F925..F951 ohne Ortswechsel); der Original-Finisher @0x80118cc8-d68 (selbst gelesen) trifft Leon
mit < 50 HP aus dem haengenden Flug. Vorbestehende Klasse (Zufallsgang auch im Stand NB5 und im Original-Modus); NB6g begrenzt sie auf
einen Sprung je Pad-Aufenthalt, danach zu Fuss (F1001 Grund 11). Fuer eine spaetere Runde: Original-Landung @0x80118c3c-cc4 gegen
`re15_collision_constrain_contact_band` im Flug RE'en (Messweg steht im Dossier). (b) **Zufallsgang Anker 400** 16352 -> 17655 (+8 %) durch
zwei Seeds dieser Klasse bei Stillstand (Anker 40) 125 -> 0 und Griffen 242 -> 252 — Stichprobe 24/24 bitgleich, Erklaerung je Seed im
Dossier. (c) Dossier-Wortlaut "Stelle 8: Angriff F274, Grund 13" (3.4): F274 ist der erste Angriff ohne Pad-Sprung, der Grund-13-Sprung
kommt F693 (Angriff F759); in der Sache richtig (468 -> 0). (d) **Messhinweis**: ein an-/abgesteckter Xbox-Controller veraendert Laeufe
ohne RE15_INPUT_SCRIPT (eigener Stelle-8-Lauf verworfen, `debug.log [pad] Controller abgezogen`); vor dem Vergleich debug.log pruefen.
(e) PSX-Laufzeit (Rasterscan + A* je Bild) weiter OFFEN mit Plan (NB5/NB6 OFFEN 5). (f) Volle Suite: Orchestrator. (g) Messmittel im
Pruefer-Scratch `F_abn6/` (nicht eingecheckt): `run.sh`, `driver.sh`, `jobsA6/B6/R6.txt`, `sonden.sh`, `seeds12.sh`, `ausw_abn6.sh`,
`auswertung.txt`, Rohdaten je Lauf (gy/st/debug/affen.log, Framedumps), `lage_*.txt`, `kanon_*.txt`, `riegel_affen_block.txt`,
`eintritt_mess_head.txt`, `seeds12_bits{3,0}.txt`, `ctest_gezielt.txt`.
