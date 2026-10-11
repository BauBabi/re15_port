# Runde 36 — Spur F "zombieki": Abnahme 5

Datum: 2026-10-11. Pruefer: unabhaengige Abnahme 5 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **e856447f** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md,
Vorgaenger-Abnahme `F_abnahme_4.md` (Stand c541415c: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3).

> **ERGEBNIS: (in Arbeit — Bericht wird fortlaufend geschrieben)**

## 0. Stand, Bau, Messmittel

- `git log master..HEAD`: Abnahme-4-Stand (0b12bbf6) + 5d130d71, 842ec35a, 083b0c98, 218e76cc, e856447f. Eigener Diff des
  Zweigs = `git diff master...HEAD` bzw. `git diff 78a4a748..HEAD` (master ist seit dem Abzweig um 836023f5/565da62b weiter; ein
  blankes `git diff master` zeigt deren Dateien — synchro/STAGE1/room1011.. — als "geloescht"; nicht aus diesem Zweig).
- Code seit Abnahme 4 (`git diff 0b12bbf6..HEAD --stat -- re15_port`): NUR `zombieki_r36.c` (+80/-?: NAH-Anker `s_r36_nah`,
  `r36_belegt_koerper`, `r36_nah_route`, Haken in `re15_r36_affen_padsuche_decide`, Sonden-Leser/-Schalter), Header-Doku (+9),
  Riegel `test_r36_zombieki_affen_block.c` (+256). Gemeinsame Dateien (enemy_ai_common.c, enemy_ai_re2_zombie.c, main.c) seit
  Abnahme 4 unveraendert; P1-/P3-Funktionen unveraendert.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to do",
  `LOCAL-BUILD-OK (build)`. Baum sauber (`git status` leer), exe 04:24:07 nach der letzten Quellaenderung (zombieki_r36.c
  04:23:47, Header 04:23:53; e856447f fuegt in Code-Dateien nur Kommentare hinzu) => exe = HEAD, md5 714f8527... (= `re15_pc_nb5b.exe`
  des Bau-Agenten; seine Dossier-Laeufe liefen mit `re15_pc_nb5a.exe` 03:59, vor 218e76cc). Messkopie `re15_pc_abn5f_neu.exe`
  (= HEAD). Vergleich: `re15_pc_abn4f_neu.exe` (Abnahme-4-Stand c541415c, md5 c3e7d409...), Rohdaten der Abnahme 4
  (`F_abn4/`, gemeinsamer Scratch, nur gelesen). Eigene Laeufe: `F_abn5/` (Skript `run.sh` nach dem Muster F_abn4, je Lauf eigener
  Prozess und Ordner).

## 1. RE-Gate (Stichprobe selbst disassembliert, richtige Binaerdatei)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`; STAGE1.BIN (`info/Re1.5/PSX/BIN/` = `shared_assets/PSX/BIN/`,
md5 a482d196...) roh @0x80100000; PSX.EXE fuer das Sprungziel.

| Zitat (Code/Dossier/Commit) | Datei | gelesen | Urteil |
|---|---|---|---|
| A[3] Biss-Test `ori a0,zero,0xbb8` / `ori a1,zero,0x180` / `jal 0x8001a804` / `bgez v0,0x80117b40` @0x80117a60-74 | STAGE1.BIN | `lbu v0,0(a2)` @0x80117a54, `bne v0,zero,0x80117b40` @0x80117a5c, `ori a0,zero,0xbb8` @0x80117a60, `ori a1,zero,0x180` @0x80117a64, `addiu s0,a2,-95` @0x80117a68, `jal 0x8001a804` @0x80117a6c, `addu a2,s0,zero` (Delay) @0x80117a70, `bgez v0,0x80117b40` @0x80117a74 | stimmt |
| Sprungziel FUN_8001a804 = "Bissweite" | PSX.EXE | s1 = `SquareRoot0`(dx²+dz²) @0x8001a838-864 (Ziel +0/+8 gegen cur +0x34/+0x3c); `slt s0,s0,s1` / `bne` -> Rueckgabe Abstand (>= 0) wenn Reichweite < Abstand @0x8001a870-74; sonst `atan2_q12` @0x8001a894, minus Gierwinkel +0x6a @0x8001a8a8-b0, `+a1`, `&0xfff`, `slt v0,v0,(a1<<1)` @0x8001a8c4, Bit 31 gesetzt (`or s1,s1,0x80000000` @0x8001a8d0) => negativ nur bei Abstand <= 0xbb8 UND Kegel +-0x180 | stimmt (Bissweite = Abstand <= 3000 im Kegel +-384; der Port waehlt Bissplaetze nur nach Abstand < 3000 — Zielwahl, der Kegel ist eine Frage der Drehung) |
| B[3] steuert gerade auf playerX `addiu a0,a0,-13688` @0x80117d4c, `jal 0x8001a8f8` @0x80117d50 | STAGE1.BIN | `jal 0x8001af20` (rng) @0x80117d1c, `andi v0,v0,0x1f` @0x80117d24, `addiu v0,v0,6` @0x80117d30, `sb v0,158(v1)` (+0x9e) @0x80117d34, `lb a1,158(v0)` @0x80117d44, `lui a0,0x800b` @0x80117d48, `addiu a0,a0,-13688` (= 0x800aca88 playerX) @0x80117d4c, `jal 0x8001a8f8` @0x80117d50 | stimmt |
| Radius box[6] 1600 @0x80121356 | STAGE1.BIN | Bytes ab 0x80121350 `00 00 60 fa 00 00 40 06 a0 05 40 06` -> u16 @+6 = 0x0640 = 1600 | stimmt |

- Code-Diff des Zweigs (`git diff 78a4a748..HEAD -- re15_port/engine re15_port/include re15_port/platform`, nur `+`-Zeilen) und der
  NB5-Diff inkl. Tests (`git diff 0b12bbf6..HEAD -- re15_port`) nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/
  approx/schaetz/ungefaehr/vorerst/vorlaeufig/getenv/"sieht richtig"/geraten/guess: **0 Treffer**. Schalter `re15_r36_*_enable`
  (8 Stueck, NEU `re15_r36_affen_nah_enable`) haben in engine/ und platform/ KEINEN Aufrufer (nur Sonden).
- Neue Stellgroesse der NB5: `R36_NAH_START = 354` — im Code (zombieki_r36.c Kopf des NB5-Blocks), im Header und in den Commits
  842ec35a/e856447f als PORT-WAHL gekennzeichnet, mit Messreihe (Sonde `kanon`, N = 1..354) und der Begruendung "kleinster Beginn,
  der den Spur-J-gepinnten Kampf unberuehrt laesst"; Bissweite `< 0xbb8` (@0x80117a60), NAH-Grenze `0x1771` (@0x80117bc8), Koerper-
  kontakt = Summe der Radien box[6] (@0x80121356), Anker 400 (seit NB3 PORT-WAHL) — alle mit Zitat.

## 2. Messungen P2 — eigene Sonden (Riegel-Binary `test_r36_zombieki_affen_block.exe`, je eigener Prozess)

### 2.1 Bissplaetze (`platz <lx> <lz> 50`, `F_abn5/platz_eigen.txt`)
Sonden-Code gelesen: alle Punkte (Schritt 50) mit Abstand < 3000 zu Leon, die die Original-Klemme (`re15_collision_constrain_contact_band`,
Radius 1600, Band 0, Maske 4, Vorlage x+1) nicht verschiebt; Durchmesser = groesster Abstand zweier solcher Punkte.

| Leon | Bissplaetze | Bereich | Durchmesser | Bedeutung |
|---|---|---|---|---|
| (-26932,17268) Ecke Nord (t11b0kampf) | 279 | x -25782..-24182, z 15718..16118 | 1649 | nur EINER |
| (-26932,14578) | 279 | x -25782..-24182, z 15728..16128 | 1649 | nur EINER |
| (-26932,15700) / (-26932,15707) | 296 | x -25782..-23982, z 15750..16107 | 1833 | nur EINER |
| (-26932,15419) / (-26932,15712) — Leon-Lagen der exe-Stelle 11/4 waehrend S3 wartet | 329 / 333 | x -25782..-23982 | 1843 | nur EINER |
| (-26600,14578) / (-26500,15500) Stelle 13 | 342 / 366 | x -25800..-23550 | 1990 / 2277 | nur EINER |
| (-24452,11938) Stelle 11 Start (= Leon waehrend der ersten S3-Strecken an Stelle 4) | 696 | x -23302..-21702, z 13088..14688 | 2262 | nur EINER |
| (-25177,15695) (Stelle 4, Leon auf dem Weg in die Ecke) | 1316 | x -25777..-22227 | 3716 | zwei |
| (-7107,-12395) / (-7138,-12372) kanonisch | 4030 / 4035 | | 5809 | zwei |
| (5858,-5716) Ostgasse | 1769 | x 6408..8258, z -8616..-4466 | 4154 | zwei (nur an beiden Enden) |

Die Dossier-Tabelle "NB5 Bissplaetze" ist bitgleich reproduziert. In der Nordwest-Tasche (Leon an der Westwand x -26932/-26600/-26500)
liegt jeder Bissplatz auf dem Nordstreifen (z 15712..16128, 400 breit) — dort kann nur EIN Gorilla beissen (Durchmesser < 3168 = gemessener
Koerperabstand Spur J). Das ist eine notwendige Bedingung, keine hinreichende: in der Ostgasse (Durchmesser 4000..4154) passen zwei nur,
wenn sie an den beiden Enden des Streifens stehen (s. 2.2).

### 2.2 "Gleiten an Schraegkanten" (Dossier NB5 OFFEN, Sonde des Bau-Agenten selbst nachgefahren)
`lage 900 1 1 5858 -5716 0 5876 -8818 3077 3 0 9434 2189 2656 0 0` (`F_abn5/lage_gleiten_m1_spur1.txt`, Spur je Bild; Modus 0 zum
Vergleich `lage_gleiten_m0.txt`): Ergebnis wie im Dossier "fest 892" (G1), ABER:
- G1 gleitet F301-F330 von x 5869 nach 4794 (Anker-Reset F303/F307), ein weiterer Reset F422 — danach steigt der NAH-Anker-Zaehler
  ungestoert: **nah 354 ab F776, nah 478 bei F900**. Der Nah-Umweg wird trotzdem in **0** Bildern aktiv (`uw0` durchgehend).
- Grund: die Sonde hat nicht "EIN Gorilla", sondern G2 (Start (9434,2189) Sub 0) wird aktiv, springt F300 (Sub 7) in die Gasse und beisst
  Leon ab F330 fortlaufend aus (7124..7897, -5951..-6896). Bissplaetze fuer Leon (5268,-5719) (dorthin geschoben): x 6418..7968,
  z -8469..-4469 (`platz`), jeder < 2300 von G2 entfernt => kein freier Bissplatz => `r36_nah_route` liefert 0 => G1 steht an.
- **Die Dossier-Aussage "der Zaehler kommt nicht auf 354" und "EIN Gorilla am Ostgassen-Eingang gleitet" ist gemessen falsch**; der Fall
  ist ein ANSTEHEN in der Ostgasse (zweiter Gorilla belegt den einspurigen Bissstreifen in der Mitte), kein Gleit-Problem. Der als
  "naechster Weg" genannte Plan (Anker an den Abstand koppeln) wuerde diesen Fall nicht aendern. (Mangel, s. Abschnitt 8.)

### 2.3 Nachruecken (Riegel-Lage, Gorilla 1 F400 per Treffer getoetet; `lage 1200 1 10 -26606 14578 18 -23302 14489 2067 3 0 -20579 16118
1885 4 0 400 2`, `F_abn5/lage_nachruecken_m1.txt`)
- G1 beisst F91, G2 steht ab ~F110 an der #21-Ecke (-23302, 13070..14680), d 3645..3976. G1 F400 Zustand 3 -> F470 Leiche (Zustand 7).
- G2 kriecht danach nach Norden (F480 z 13535, F610 z 14580) — jede Bewegung > 400 setzt den NAH-Anker zurueck (F480, F580); erst ab
  F933 (`nah` 351 -> F940 361) startet der Nah-Umweg (`uw1` F940), G2 laeuft ueber den Nordstreifen und **beisst F990**.
- Der Bissplatz ist ab F400 frei (`r36_belegt_koerper` zaehlt nur Zustand 1); G2 braucht **590 Bilder (~20 s) nach dem Tod von G1**,
  davon ~530 Bilder Stillstand/Kriechen an der #21-Ecke — allein wegen der festen Schwelle R36_NAH_START = 354, die fuer den
  kanonischen Ankunftskampf gewaehlt ist. Der Riegel prueft nur `ag[1] > 400` (irgendein Angriff in 800 Bildern).

### 2.4 Ostgasse O1 im PORT-Modus (Riegel-Lage O1, `lage 2400 1 30 6000 -9000 1332 -5525 -14883 2731 3 0 -8915 -12487 93 3 0`,
`F_abn5/lage_o1_m1.txt`)
Der Riegel prueft O1 nur im Modus Original-Regel; im PORT-Modus (alle Verbesserungen an) selbst gefahren: G1 beisst F195 und schiebt
Leon durch die Ostgasse bis (7873,-1412); danach **G1 1368 Bilder in der Gassenmitte (6418,-4333..-4399), G2 1615 Bilder am Suedeingang
(8061..8093,-7078..-7130), beide Sub 3 NAH (d 3254 / 5683), `nah` bis 1368/1358, `uw0` durchgehend, kein Angriff bis F2400** — obwohl
um Leon 4557 Bissplaetze liegen (`platz 7873 -1412`: x 6423..10673, z -2812..1538, Durchmesser 4608, zwei passen). Der Nah-Umweg sucht
nur in der EIGENEN Fuss-Komponente (keine Pads, Dossier NB5), die Gasse ist am Pad verkeilt => kein Ziel => Original-Stillstand. Das
ist der Dossier-OFFEN-Punkt "Gassenmitte" — gemessen betrifft er im PORT-Modus BEIDE Gorillas, 45/54 s lang, nicht nur "Slot 3 468".

## 3. exe-Messungen (HEAD `re15_pc_abn5f_neu.exe`, Kommandozeilen der Abnahme 4, je Lauf eigener Prozess, `F_abn5/jobsA.txt`)

Basis-Haken je Lauf: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log` (Skript
`F_abn5/run.sh`). Vergleich byteweise (`cmp`) gegen die Laeufe der Abnahme 4 (HEAD c541415c, `F_abn4/*_abn4f_neu`) und des Bau-Agenten
(`F_nb5/*_nb5a`).

### 3.1 Bitgleichheit
| Lauf | gegen Abnahme 4 (c541415c) | gegen nb5a (Bau-Agent) |
|---|---|---|
| 12 Gorilla-Stellen (alle ausser 1 und 3) | gy.log **gleich** | gleich |
| Stelle 1 (-7000,12500), Stelle 3 (6000,-9000) | anders (NB5 wirkt) | **gleich** |
| szene11c0 (kanonischer Kampf, Sprung), t11b0szene (kanonisch, echter Tuerweg), t11b0kampf, flucht_west | gy.log **gleich** (t11b0*/szene: st.log gleich) | gleich |
| P1: r1040w, r10b0, tuer1010, tuer1040, t1220a, t1220c | gy.log **gleich** (tuer1040/t1220a/t1220c auch st.log) | — |
| P3: p3lache | gy.log, st.log und **alle 51 Framedumps gleich** | — |

### 3.2 Punkt 2 — 14 Stellen (Kampf-Layout `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700`,
keine Eingabe; Auswertung `F_abn5/ausw4.py` (= Auswerter der Abnahme 4) und eigener `ausw5.py` mit Strecken + Leon-Lage, `F_abn5/ausw4_14.txt`)

| # | Leon | erster Angriff S2 / S3 | fest S2 / S3 (FERN) | laengste Strecke / Lage | Ursache (gemessen) |
|---|---|---|---|---|---|
| 1 | (-7000,12500) | F1627 / F730 | 341 / 250 | S2 F1254+341 (-13527,11922) d 3556..3673; S3 F1911+250 bis Laufende (Leon stirbt ~F2160) | NAH, unter Schwelle 354 |
| 2 | (-14000,2000) | F680 / nie | 0 / 0 | — | |
| 3 | (6000,-9000) | **F1596 / F324** | **429 (45) / 368** | S3 F826+368 (5771,-8824) d 3087..3158 -> Nah-Umweg F1180, Biss; S2 3 Strecken 152/119/158 | **Kriterium erfuellt** (vorher 1653/1875) |
| 4 | (-24000,10000) | F853 / F1570 | 0 / **1193** | S3 F1781+920 (-23302,13088) d 3211..5514 | Anstehen (Tasche, 2.1) |
| 5 | (7000,4000) | nie / F18 | 96 (96) / 0 | S2 FERN vor dem eigenen Angriff | |
| 6 | (9000,10000) | F1175 / F309 | 0 / 0 | — | |
| 7 | (-14746,-4140) | F364 / F1021 | 179 / 108 (108) | | |
| 8 | (7200,-4130) | F1050 / F274 | 0 / **468** | S3 F648+468 (6418,-3617) d 3016..3570 | Gassenmitte (2.4), > 450 |
| 9 | (-14000,4000) | F704 / F1496 | 250 / 0 | | |
| 10 | (-14500,6000) | F694 / F892 | 0 / 0 | — | |
| 11 | (-24452,11938) | F828 / F1253 | 0 / **1335** | S3 F1366+1335 (-23061,13333) d 3358..5472, Leon (-26932,15419..15707) | Anstehen |
| 12 | (-22288,11938) | F868 / F1025 | 0 / **833 (50)** | S3 F2204+472 (-23302,13082) | Anstehen |
| 13 | (-26500,15500) | F842 / **nie** | 0 / **1650** | S3 F1065+1518 (-23302,13217) | Anstehen |
| 14 | (-20000,14500) | F883 / F896 | 0 / 0 | — | |

Summen fest (FERN / NAH): v0.8.23 25936 (19812 / 6124) -> Abnahme 4 10466 (255 / 10211) -> **HEAD 7500 (300 / 7200)** (beide Summen mit
demselben Auswerter aus den Rohdaten nachgerechnet; Dossier-Zahlen stimmen). Kanonischer Kampf: szene11c0 S2 398, t11b0szene S2 408 (= Original,
bitgleich). **t11b0kampf: S2 1309 NAH (laengste F1991+483 auf dem Nordstreifen (-22290,16118) hinter S3), S3 greift ab F866 an, Leon
stirbt ~F2473 — bitgleich zu Abnahme 4.**

Abnahme-4-Kriterium (Mangel 1: Stellen 3, 4, 11, 12, 13 und t11b0kampf, je Gorilla <= 450 NAH fest): erfuellt an Stelle 3; NICHT erfuellt
an 4 (1193), 11 (1335), 12 (833), 13 (1650), t11b0kampf (1309). Fuer diese fuenf ist der Grund gemessen (2.1): in der Tasche passt nur
EIN Gorilla in Bissweite; der zweite kann dort nicht beissen, solange der erste angreift. Ausserhalb der Liste bleibt Stelle 8 mit 468.

### 2.5 Ein einzelner Gorilla in der Gassenmitte (eigene Lage; `lage 2400 <m> 60 7873 -1412 0 6418 -4333 3373 3 0 -9013 -15461 2656 0 0`,
`F_abn5/lage_gasse_einzeln_m{1,0}.txt`)
Leon am Nordende der Ostgasse (= O1-Endlage), G1 NAH in der Gassenmitte im Pad #36 (= O1/exe-Stelle-8-Lage), G2 weit weg (Kampf-Startlage).
PORT-Modus: **G1 steht 1200 Bilder** (6418,-4319..-4421), `nah` 59 -> 1199, `uw0`, kein Angriff — erst als G2 (FERN-Umweg, Angriff F1197)
Leon weiter schiebt, verlaesst G1 die Gasse und steht danach hinter G2 erneut an (Summe fest 2291). Modus Original-Regel: 2400/2400.
Der Nah-Umweg greift fuer einen NAHEN Gorilla, den nur ein 0x10-Pad von Leon trennt, nie.

### 3.3 Punkt 1 — Zombies (P1-Code seit Abnahme 2 unveraendert, NB5 aendert nur Gorilla-Funktionen)
- Alle P1-Laeufe der Abnahme 4 bitgleich (3.1): ROOM1040 Westgang, ROOM10B0 Terrasse, echte Tuerwege 1020->1010 und 1020->1040, 1210->1220
  (aot1/aot3); r1010a (Abnahme-1-Fall) s. 3.5.
- Eintritts-Zensus am HEAD selbst gefahren (`test_r36_zombieki_eintritt mess`, 78 Tueren, Szenarien "steht"/"hin", `F_abn5/eintritt_mess_head.txt`):
  **266 Faelle, PASSIV 0, KLEMME 0**.

### 3.4 Punkt 3 — Blutlache (Code seit Abnahme 2 unveraendert)
- exe p3lache (`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=-9013,-11800,1024,0 RE15_GIVE=7:200 RE15_EQUIP=7
  RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6)x45 RE15_FORCE_CUT=5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=200-700/10:fd_ RE15_EXIT_AT=700`): alle 51
  Framedumps bitgleich zu Abnahme 4. Eigene Zaehlung dunkelroter Pixel (G,B <= 24, R >= G+14) im Fenster (120..260, 90..160): F290 292,
  F300 294, F330 340, F360 422, F380 465, **F390 494**, F400..F570 494 — Wachstum ueber ~90 Bilder ab dem ersten Leichentakt (0x5a
  @0x8011be80), dann konstant. Bilder selbst angesehen: `F_abn5/p3_lache_streifen_abn5.png` (F260..F550) und Ausschnitt
  `F_abn5/p3_lache_zoom_abn5.png` (F290 keine Lache, F330 kleiner Fleck, F390/F450 dunkelrote Lache unter der Leiche, bleibt liegen).

(Fortsetzung folgt.)
