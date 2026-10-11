# Runde 36 — Spur F "zombieki": Abnahme 6

Datum: 2026-10-11. Pruefer: unabhaengige Abnahme 6 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **654b27e0** (Basis master 78a4a748 = merge-base; master selbst steht bei 08aa6f96, nur
Dokumente der UEBERGABE). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md, Vorgaenger-Abnahme `F_abnahme_5.md`
(Stand e856447f: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3).

> **ERGEBNIS: siehe Abschnitt 9/10 (wird fortlaufend geschrieben).**

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

(Abschnitte 3 ff. folgen: exe-Messungen, gezielte Regression, Vertrag, Gates, Urteile, Maengel.)
