# Runde 37 — Spur G "beretta": UNABHAENGIGE ABNAHME 2 (2026-10-13)

Baum `.claude/worktrees/r37_beretta`, Zweig `r37/beretta`, Kopf 4548f6e7 (Basis = merge-base f1e019ca; master ist weiter,
Spur-Diff daher gegen f1e019ca). Geprueft wird die Nachbesserung 2 auf den Mangel N1 der Abnahme 1 (`G_abnahme_1.md`,
Gitterarm 0x1A) und erneut der ganze Nutzer-Punkt.

Eigener Bau: `bash re15_port/tools/local_build.sh configure` + `build` -> `LOCAL-BUILD-OK (configure)`,
`ninja: no work to do.`, `LOCAL-BUILD-OK (build)`. exe `re15_port/build/platform/pc/re15_pc.exe` 2026-10-13 22:44:47
(md5 2bb9b0f0fbea6b9420802bd249f87f42 = die im Dossier 9.5 genannte exe); Quellen der Spur aelter (beretta_r37.c 22:44:32,
re15_beretta_r37.h 22:44:40); Baum sauber bis auf das unversionierte `mess_r37g/`. Gemessen wird ueber eine Messkopie
unter eigenem Namen.

## Ergebnis: BESTANDEN
| Pruefung | Urteil |
|---|---|
| Punkt 1 (Beretta M93R: jeder Schuss des Feuerstosses trifft) | **erfuellt** — an der exe: Gitterarm 0x1A EIN Treffer, Rueckzug einmal, Ende E+10 wie Browning (Std/RE1.5/MIXED, M93R und Glock 18); Kugel 2 trifft den naechsten Arm in der Bahn; alle anderen Klassen weiter drei Kugeln; G5 jetzt auch an der exe gemessen (Abschnitt 1) |
| N1 der Abnahme 1 | behoben (1a, 1b, Gegenprobe Abschnitt 2) |
| RE-Gate | **haelt** — 4 Belege selbst disassembliert (EM2D-Overlay selbst geschnitten), keine neue Konstante, keine Guess-Tells, Fix erklaert den Befund (Gegenprobe 6 FAIL) |
| Vertrag | haelt |
| Gezielte Tests | 100/100 Unit (Regex U2), 5/5 exe-Integration (Regex X) |

## 1. Punkt 1 (Nachtrag, Wortlaut) — Messung an der exe
> bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden.
> Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.

Messaufbau (eigenes Skript `abn2/run.sh` im Scratchpad der Abnahme; Messkopie `re15_abn2g.exe` neben der exe; Logs im
Scratchpad `abn2/mess/<lauf>/`; Auswertung eigenes `abn2/ausw.py`: Segment nach dem Raumsprung, je Bild Munitions-,
HP- und st/ss1-Wechsel je Platz; Muendung = wf.log `SPAWN id=2 sub=0`). Je Lauf:
`env -u RE15_AI_FLAVOR RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=<raum>@250 [RE15_PLAYER_POS=<x,z,rot>] RE15_GIVE=<w>:15 RE15_EQUIP=<w> RE15_INPUT_SCRIPT_BASIS=spiel
RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=<eingabe> RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log
RE15_EXIT_AT=<bild>#<raum> [RE15_AI_FLAVOR=re15|mixed] [RE15_RE2_TRACE=1]` — alle Laeufe `exit=0`,
`[flow] EXIT_AT: Bild <n> in Raum <r> erreicht -> exit`. "std" = KEIN RE15_AI_FLAVOR (so spielt der Nutzer, Default RE2).

### 1a. N1 nachgemessen: Gitterarm 0x1A, Aufbau Abnahme 1f (ROOM1210, Leon (-19500,-13000,1024), Eingabe
`U0.8,R0.5,M1.0,MA0.1,M2.0,W2`, EXIT 130#1210, `RE15_RE2_TRACE=1`, Trace je Lauf aus dem exe-Ordner verschoben)
| Lauf | KI | Waffe | Munition | Muendung | HP Arm 5 | Rueckzug (Trace `[re2arm] slot 5`) | Ende (hp -1, 1/7) |
|---|---|---|---|---|---|---|---|
| arm_w5_std | Standard | 5 M93R | F70 15 -> 12 | F70/F73/F76 | **F70 250 -> 245 (einer)** | 1/1 -> 5/0 -> 5/1 -> 7/0 (EINMAL) | **F80 (E+10)** bei (-21750,-15897) |
| arm_w5_re15 | RE1.5 | 5 M93R | F70 15 -> 12 | F70/F73/F76 | F70 -5 (einer) | einmal (zeilengleich) | F80 |
| arm_w5_mixed | MIXED | 5 M93R | F70 15 -> 12 | F70/F73/F76 | F70 -5 (einer) | einmal | F80 |
| arm_w6_std | Standard | 6 Glock 18 | F70 15 -> 12 | F70/F73/F76 | F70 -5 (einer) | einmal | F80 |
| arm_w6_re15 | RE1.5 | 6 Glock 18 | F70 15 -> 12 | F70/F73/F76 | F70 -5 (einer) | einmal | F80 |
| arm_w3_std (Kontrolle) | Standard | 3 Browning | F70 15 -> 14 | F70 | F70 -5 | einmal | F80 |
Trace aller M93R-/Glock-Laeufe zeilengleich mit dem Browning-Lauf (sub 1/1 -> 5/0 fr=12 hp=245 -> 5/1 clip=4 -> 7/0 hp=-1
pos=(-21750,-2500,-15897)). Abnahme 1 hatte am NB1-Stand F70/F73/F76 je -5, Rueckzug dreimal, Ende F86 gemessen.
=> N1-Befund an der exe behoben, in allen drei KI-Modi und fuer beide Feuerstoss-Waffen.

### 1b. Kugel 2/3 an den naechsten Kandidaten — an der exe (vom Bau-Agenten nur in der Sonde Teil J gezeigt)
Aufbau: ROOM1210 Standard-KI, Leon (-19500,-13000) geht weiter in den Flur, bis Arm 5 (-21490,-15747) UND Arm 6
(-21490,-17130) heraus sind (beide 1/1), dann Zielen (Auto-Aim dreht auf den naechsten Arm) und EIN Abzug, EXIT 200/220.
| Lauf | Eingabe / Leon beim Abzug | Waffe | Muendung | Treffer | Rueckzug-Enden |
|---|---|---|---|---|---|
| **v1** | Start rot 3072, `D3.0,M0.7,MA0.1,M1.0,W2`; Leon (-19500,-18950) rot 2529 (Blick zurueck den Flur entlang) | 5 M93R | F112/F115/F118 | **F112 Arm 6 -5, F115 Arm 5 -5**, F118 keiner | Arm 6 F122 (E+10), Arm 5 F125 (E+3+10) |
| two_d_w6 | Start rot 1024, `U1.9,M0.7,MA0.1,M0.7,MA0.1,M1.0,W2`; Leon (-19500,-16900) rot 1975 (quer zur Wand) | 6 Glock 18 | F79/F82/F85 (2. Stoss F103/106/109) | **F79 Arm 6 -5, F82 Arm 5 -5**, F85 keiner | F89, F92 |
| two_d_w5 | dito | 5 M93R | F79/F82/F85 | F79 Arm 6 -5; Kugel 2/3 keiner | F89 (Arm 5 zieht sich erst F116 von selbst zurueck) |
| two_d_w3 (Gegenprobe) | dito, zwei Einzelschuesse | 3 Browning | F79, F103 | F79 Arm 6 -5; F103 keiner (Arm 5 noch 1/1) | F89 |
=> Die Regel "getroffener Arm sperrt, Kugel 2 geht an den naechsten Kandidaten, Kugel 3 findet keinen (beide Arme
gesperrt)" laeuft an der exe so, wie RE2 sie vorgibt (Gate +0x1D3 @0x80041270-78). Quer zur Wand (two_d) erreicht die M93R
den zweiten Arm nicht, die Glock 18 schon: das ist die waffeneigene Keilbreite des Ports (s_player_wpn_reach[5] = 1000,
[6] = 1200, re15_damage.c:1024-1027 "@0x8006e5a0"; Keil-Halbbreite r + reach, re15_gun_wedge_inside) — die Browning (reach
1000) trifft Arm 5 aus derselben Lage ebenfalls nicht (F103). Bestand, nicht die Spur. Blickt Leon den Flur entlang (v1),
trifft auch die M93R mit Kugel 2 den zweiten Arm. (v3 = Leon naeher an der Wand: Arm 5 greift Leon, kein Abzug — Aufbau.)

### 1c. Nebenwirkung am Arm: wiederholte Abzuege ueber 560 Bilder (Aufbau 1a, Eingabe
`U0.8,R0.5,M1.0,MA0.1,M3.0,MA0.1,M3.0,MA0.1,M3.0,MA0.1,M3.0,MA0.1,M3.0,W2`, EXIT 560#1210)
| Lauf | Waffe | Abzuege (Munition) | Treffer |
|---|---|---|---|
| rep_w5 | 5 M93R | F70/F163/F256/F349/F442 (15 -> 0) | F70 Arm 5 -5, Ende F80; danach keiner (Arm 5 kommt bis F560 nicht wieder heraus) |
| rep_w3 | 3 Browning | F70/F163/F256/F349/F442 (15 -> 10) | identisch: F70 -5, Ende F80, danach keiner |
=> Nach einem Feuerstoss verhaelt sich der Arm im weiteren Verlauf genau wie nach einem Browning-Schuss.

### 1d. Gegenproben der uebrigen Gegnerklassen am NB2-Stand (Standard-KI) — der Fix darf sie nicht beruehren
| Lauf | Aufbau | Waffe | Muendung | HP-Abzuege |
|---|---|---|---|---|
| zomb_w5_std | 1140, Leon (200,-18200,1024), `M2.0,MA0.1,M3.0,W2` | M93R | F61/F64/F67 | Zombie Platz 5 (0x11) F61/F64/F67 je -16 |
| spin_w5_std | 2050, Leon (-7000,-6082,0), `M0.5,MA0.1,M0.6,MA0.1,M1.0,W2` | M93R | F16/19/22, F37/40/43 | Spinne 0x25 Platz 2: sechs Treffer je -17 (110 -> 8) |
| gor_w5_std | 11C0 ohne PLAYER_POS, `W36.7,M1.0,MA0.1,M3.0,W2`, EXIT 1300 (Raum-Ereignis laeuft voll) | M93R | F1132/1135/1138 | Gorilla 0x27 Platz 2 je -12 (180 -> 144) |
| birk_w5_std | 5080, Leon (-18100,1700,1024), `M2.0,MA0.1,M3.0,W2` | M93R | F61/64/67 | Birkin 0x30 je -15 (300 -> 255) |
| gator_w5_std | 2090, Leon (-7200,-5200,1024), `M0.5,MA0.1,M3.0,W2` | M93R | F16/19/22 | Alligator 0x23 Platz 20 je -7 (3000 -> 2979) |
| integration_r37_beretta (eigener ctest-Lauf, Logs `build/tests/integration/r37_beretta_wd/lauf_a5f45bbb/`, selbst ausgewertet) | leon_re2_w5 1140, t2b_std_w5 5040, roach_std_w5 3040, elza_re2_w5 11E1 | M93R | E/E+3/E+6 | Zombie 3 x -16; 0x2b 3 x -10; Kakerlake 0x29 3 x -14; Elza: Zombie 3 x -16 |
=> Alle Gegnerklassen nehmen weiter drei Kugeln; Zahlen gleich Abnahme 1 (NB1-Stand).

### 1e. G5 0x36 (vom Bau-Agenten NICHT an der exe gemessen) — jetzt gemessen
Aufbau wie test_r35_granate.cmake (birkin): `RE15_DEBUG_JUMP=5090@250 RE15_DEBUG_SUB=4@255` (Kampfstart), Leon
PLAYER_POS (-4000,-20000,1536) — das Raum-Ereignis setzt ihn auf (-6151,-18264) und schiebt ihn in +z; Eingabe
`W12.5,M0.7|0.8|0.9,MA0.1,M2.0,W2`, EXIT 520#5090. Der Boss (Platz 2, Typ 0x36, hp 600) kriecht ~140/Bild in +x.
| Lauf | KI | Waffe | Abzug | Treffer |
|---|---|---|---|---|
| g5b_w5_std | Standard | M93R | F397 (15 -> 12), Muendung F397/400/403 | F397 -15, F400 -15; F403 keiner |
| g5b_w5_re15 | RE1.5 | M93R | F397 | F397 -15, F400 -15; F403 keiner |
| g5b_w3_std | Standard | Browning | F397 | F397 -15 |
| g5c_w3_f400 | Standard | Browning | F400 | F400 -15 |
| **g5c_w3_f403** (Gegenprobe Kugel 3) | Standard | Browning | F403 | **keiner** — der Boss (-6859,-23400) liegt in diesem Bild ausserhalb des Schusskeils (Leon rot 1365 fest) |
| g5c_w5_f400 | Standard | M93R | F400, Muendung F400/403/406 | F400 -15; F403/F406 keiner (wie die Browning in F403) |
=> G5 nimmt jede Kugel, solange er im Keil liegt (zwei Treffer im Abstand 3 Bilder); die fehlende dritte Kugel ist die
Geometrie — ein Browning-Einzelschuss im selben Bild trifft ebenso nicht. Das neue Gate (+0x1D3 != 0) greift bei G5 nicht:
der Schuss stempelt 0 (re15_re2_stun_frames `default: return 0`, re15_damage.c:595), das Modul setzt nie Bit 0x80 an sich
(einziger Schreiber enemy_ai_boss_g5.c:1542 ist das Abzaehlen der unteren 7 Bits). Offener Punkt des Bau-Agenten erledigt.

### 1f. Urteil Punkt 1: **erfuellt**
An der exe unter der Standard-KI (und stichprobenhaft RE1.5/MIXED) nimmt jede Gegnerklasse, die der Feuerstoss erreicht,
mehrere Kugeln im Takt E/E+3/E+6 (Zombie, Spinne, Gorilla, Kakerlake, 0x2b, Birkin, Alligator, Elza-Zombie; G5 bei jeder
Kugel im Keil); drei Patronen, drei Muendungsblitze. Der Gitterarm (einziger Gegner, den RE2 nach dem ersten Treffer per
+0x1D3 sperrt und der nach EINEM Treffer ohnehin am Rueckzugsende verschwindet) nimmt EINE Kugel, die zweite geht an den
naechsten Arm in der Bahn (v1, two_d_w6) — keine Kugel bleibt mehr wirkungslos am ohnehin sterbenden Arm haengen.
Browning-Gegenproben unveraendert (eine Kugel je Abzug). N1 der Abnahme 1 ist behoben.

## 2. RE-Gate
Code-Diff der Nachbesserung 2 (`git diff 46e72af2 HEAD -- re15_port/engine re15_port/include`): EINE neue Abfrage
`re2_kugel_gate_sperrt(e)` = `re15_ai_re2_for_type(e->type) && e->re2z_self1d3 != 0` und EINE Zeile in riegel_loesen()
(`if (re2_kugel_gate_sperrt(e)) continue;`), sonst Kommentar/Header. Keine neue Konstante (nur `!= 0`, das selbst das
zitierte `bne v0,zero` ist). Guess-Tells (`grep '^+'` auf deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|
sieht richtig|getenv) im ganzen Spur-Diff gegen f1e019ca: **keine**. Kein Env-Schalter.
Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` / `re15_disasm.py` des Hauptbaums;
`info/re2leon/PSX.EXE`; `info/Re1.5/PSX.EXE` = Auslieferungsstand, Datei-Offset von 0x80026e4c = `08 00 e0 03`; EM2D-Overlay
SELBST aus der RE2-TOC geschnitten: Eintrag (0x2D-0x10)*4+1 @0x8009ADF4 -> Sektor 2483, 5416 B, md5 82ab57601b18a0233338d56c287017f1
= die Datei des Bau-Agenten):
1. **RE2-Kugel-Resolver-Gate** `dis 0x80041260 0x2c`: `andi v0,v0,0x1` / `beq v0,zero,0x80041300` @0x80041264-68,
   **`lbu v0,467(s2)` @0x80041270**, **`bne v0,zero,0x80041300` @0x80041278**, `lh v0,342(s2)` / `bltz` @0x80041280-88,
   `lhu v0,270(s2)` / `andi 0xc000` / `bne` @0x80041290-9c; Sprungziel `80041300: j 0x800417d0` / `addiu fp,fp,4`;
   `800417d0..dc: lui/lw 0x800ce334 / bne fp,v0,0x80041254` (Schleifenkopf `80041254: lw s2,0(fp)`) = naechster Listeneintrag.
   Funktion beginnt `800410cc: addiu sp,sp,-176`; Ruf **`80048a04: jal 0x800410cc`** in derselben Funktion wie die Kugelbilder
   (0x800487d8 `addiu sp,sp,-56` .. kein `jr ra` bis 0x80048a04). **Bestaetigt.**
2. **EM2D-HURT** `RE_OVERLAY_DIR=<eigener Schnitt> dis 0x80100F8C 14 --bin CDEMD0_EM2D_ai1.BIN`: `lbu v0,467(s0)` @0x80100f9c,
   `addiu v1,zero,1281` / `sw v1,4(s0)` @0x80100fa0-a4, **`ori v0,v0,0x80` @0x80100fa8**, `jal 0x80015fe8` @0x80100fac,
   **`sb v0,467(s0)` @0x80100fb0** (Delay-Slot). Rueckzug-Ende `addiu v0,zero,1793` / `sw` @0x80100e40-44, `addiu v1,zero,-1` /
   **`sh v1,342(s0)` @0x80100e4c-50**, `ori 0x80` / `sb` @0x80100e54-58. **Bestaetigt.**
   "Geloescht nur INIT und A0": ALLE Zugriffe auf `467(` im ganzen Overlay (1354 Instruktionen) selbst gelistet — Loeschen nur
   `sb zero,467(s0)` @0x80100130, `sb zero,467(s1)` @0x801005b8 und @0x8010061c; alle anderen Schreiber (@0x8010071c,
   @0x80100df8, @0x80100e58, @0x80100fb0, @0x801010ac) sind `ori 0x80`. **Bestaetigt.** Port spiegelt das
   (enemy_ai_re2_zellenarm.c:564/602/608 = 0, 628/776/786/813/827 `|= 0x80`).
3. **RE2-Zeile 4 des Arm-Typs** (Pause 5, Dossier 9.2): Zeiger `read 0x800A6B3C` = 0x800A5180 (= 0x800A6A88 + 0x2D*4, Basis
   wie re15_damage.c:47), + 3*0x14 = 0x800A51BC; `bytes 0x800A51BC 8` = `10 3c e0 00 14 0a 85 02` -> w1 0x02850A14,
   (w1 >> 9) & 0x7f = 5. **Bestaetigt.**
4. **RE1.5-Rueckgriff** (Dossier 9.4 "setzt Bit 1 dazu und greift auf den naechsten Kandidaten zurueck") `dis 0x800123f0 12`:
   `lbu v1,147(s1)` @0x800123fc, `andi v0,v1,0x1` @0x80012404, `beq` @0x80012408, **`ori v0,v1,0x2` @0x8001240c**,
   `sb v0,147(s1)` @0x80012410, **`jal 0x80011f50` @0x80012418** (Rekursion). Port re15_damage.c:2679-2681 + Ausschluss
   `(hit_react & 3) == 3` re15_damage.c:2031. **Bestaetigt.**
Erklaert der Fix den Befund? Ja. Befund N1 = der NB1-Code loeste den Riegel des Arms vor Kugel 2/3 (kein Pausenfilter fuer
0x1A). GEGENPROBE selbst gebaut (Scratchpad `abn2/neg/`: beretta_r37.c ohne die Zeile `if (re2_kugel_gate_sperrt(e))
continue;`, mit den Flags aus build.ninja kompiliert und VOR libre15_engine.a mit dem gebauten test_r37_beretta.c.obj
gelinkt): **`test_r37_beretta: 6 FAIL`** (102/103/105/106/108/109: "hp 250 -> 245, Rest 235, weitere 2", "Rueckzug 3 x,
Ende Tick 16 wie Browning 10; ... 0x2b (-0 ...)") — dieselben Zahlen wie die exe am NB1-Stand (Abnahme 1f: 3 x -5, Ende E+16);
mit dem ausgelieferten Code `test_r37_beretta: OK` (60 x ok). Die exe-Laeufe 1a/1b zeigen die Behebung.
Begruendung im Header/Dossier jetzt richtig: "stirbt NICHT am Treffer (HP 250 -> 245), sondern am Ende seines Rueckzugs"
(gemessen 1a: 245 -> -1 in E+10); Dossier 8.3 richtiggestellt; Pruefung 70 fuehrt 0x1a nicht mehr.
**RE-Gate: haelt.**

## 3. Vertrag (VERTRAG.md)
* Pfade (`git diff f1e019ca --name-only`, 24 Dateien): analysis/befunde_runde37/G_* (Dossier, Abnahmen, Messskripte),
  engine/src/beretta_r37.c, include/re15_beretta_r37.h (neu), engine/src/game_step_common.c (+8/-3, seit Abnahme 0 unveraendert),
  engine/src/re15_damage.c (+6, unveraendert), tests/unit/probes/r37_beretta.cmake, tests/unit/test_r37_beretta.c,
  tests/integration/test_r37_beretta.cmake, tests/unit/test_re2_hp_model.c (unveraendert seit Abnahme 0). Die Nachbesserung 2
  (`git diff 46e72af2 HEAD --stat`) beruehrt NUR beretta_r37.c/.h, die eigenen Tests, Dossier und run_nb2.sh. Keine release/,
  platform/android/, shared_assets/PSX/, keine Test-CMakeLists. **Haelt.**
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / Slots: keine. **Haelt.**
* Tests nur ueber `probes/r37_beretta.cmake` (neu: `target_compile_definitions RE15_ASSET_RE2_DIR` wie die vorhandenen
  RE2-Sonden, tests/unit/CMakeLists.txt:1237 ff.; CDEMD0.EMS ist versioniert); Integration mit
  `cmake_minimum_required(VERSION 3.16)`, Messkopie `re15_pc_r37g_haken_<id>`; Teil J ohne POSIX-Aufrufe (fopen/fread).
  **Haelt.**
* Riegel fuer N1 vorhanden und messend: unit Teil J (Gegenprobe 6 FAIL, Abschnitt 2) und integration arm_std_w5/arm_re15_w5
  (verlangt genau `0:S:-5;10:S:-246`; am NB1-Stand lieferte derselbe Aufbau `0:5:-5;3:5:-5;6:5:-5`, Abnahme 1f).

## 4. Gezielte Tests (eigener Lauf im Spurbaum, keine volle Suite)
* Regex U2 des Bau-Agenten (Regex U + Gitterarm/Writher/1210/G5-Gruppen, 100 Tests):
  `ctest --test-dir re15_port/build -R "^(unit_r37_beretta|...|unit_1210_gitterhaende|unit_1210_arme|unit_1210_arme_re2|
  unit_r35_raeume_arme|unit_r35_karte_r1210|unit_g5_tentakel|unit_g5_morph|unit_g5_boss)$" --timeout 600 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 100** (27.50 s). `ctest -N | grep -iE "arm|1210|gitter|zellen|beretta|m93|
  burst|feuer|schuss|g5"`: alle Treffer liegen in U2 (unit_r36_blut_pin_arm/_burst ueber `unit_r36_blut_pin_.*`) bzw. X.
* Regex X: `ctest --test-dir re15_port/build -R "^(integration_r37_beretta|integration_r36_schuss|integration_r36_elza_schuss|
  integration_r35_werfer|integration_r36_granate)$" --timeout 900 --output-on-failure` -> **100% tests passed, 0 tests failed
  out of 5** (565.09 s; r35_werfer 134.14, r36_elza_schuss 48.43, r36_granate 91.33, r36_schuss 127.33, r37_beretta 163.83).
  Lief parallel zu meinen exe-Messlaeufen; kein Fenster-Flatterer, kein Nachfahren noetig.
* Aufraeumen: Messkopie `re15_abn2g.exe` und die von meinen Trace-Laeufen erzeugten `re2_ki.log` aus dem exe-Ordner
  entfernt (Trace je Lauf ins Messverzeichnis verschoben); kein taskkill, alle Laeufe endeten ueber RE15_EXIT_AT mit exit=0.

## 5. Maengel
**Keine.**

Hinweise (kein Mangel, nachpruefbar):
* H1 — Dossier 9.9 "Die Kugeln 2/3 fliegen weiter und treffen den naechsten Gegner in der Schusslinie": an der exe trifft
  Kugel 2 den naechsten Arm nur, wenn er im Keil DIESER Waffe liegt (M93R reach 1000, Glock 18 1200, re15_damage.c:1024-1027;
  two_d_w5 gegen two_d_w6, Browning-Gegenprobe two_d_w3), und Kugel 3 findet bei zwei getroffenen Armen keinen Kandidaten
  mehr (v1, two_d_w6) — beides wie RE2 (Gate +0x1D3) bzw. Bestand des Ports, nicht die Spur.
* H2 — Hund 0x20 / Kraehe 0x21 am NB2-Stand nicht erneut an der exe gemessen: fuer sie greift in riegel_loesen() weiter das
  `continue` der Pausenfilter-Typen VOR dem neuen Gate (beretta_r37.c, Reihenfolge HP < 0 -> Filter -> Gate); Abnahme 1 hat
  am NB1-Stand den Hund an der exe gemessen (drei Treffer) und die Kraehe im Code belegt (stirbt an Kugel 1). Spinne (gleicher Filterweg) am NB2-Stand
  gemessen: sechs Treffer aus zwei Stoessen (1d).
* H3 — Die Messlogs des Bau-Agenten (`mess_r37g/`) sind unversioniert; die Zahlen stehen im Dossier 9.1/9.5 und sind mit
  meinen Laeufen 1a deckungsgleich.
