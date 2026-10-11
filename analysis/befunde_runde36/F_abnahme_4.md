# Runde 36 — Spur F "zombieki": Abnahme 4

Datum: 2026-10-11. Pruefer: unabhaengige Abnahme 4 (kein Code geaendert). Baum `.claude/worktrees/r36_zombieki`,
Zweig `r36/zombieki`, Stand **c541415c** (Basis master 78a4a748). Massgeblich: AUFTRAG.md (Wortlaut), VERTRAG.md,
Vorgaenger-Abnahme `F_abnahme_3.md` (Stand 43220d8d: P1 erfuellt, P2 teilweise, P3 erfuellt; Maengel 1-3).

> **ERGEBNIS: NICHT BESTANDEN.** P1 erfuellt, P2 teilweise, P3 erfuellt. Bau, gezielte Regression (232/232), Spur-Riegel
> (54/54), RE-Zitate (selbst gelesen), GDB-Rohdaten und Vertrag halten. Abnahme-3-Mangel 1 ist behoben (FERN-Stau am
> Nordwest-Rand: 1465/691 -> 0/50 Bilder, Slot 3 greift an), alle 14 exe-Stellen bitgleich zu den Laeufen des Bau-Agenten.
> Offen (Abschnitt 8): an den Stoss-Zielen steht ein Gorilla weiter 783..1875 Bilder NAH ohne Angriff (auch im echten Tuerweg
> 11B0 -> 11C0: 1309 Bilder); die Begruendung "passt nicht hinein" ist gemessen falsch (die Umweg-Mechanik des Ports bringt den
> nahen Gorilla aus genau diesen Lagen in 30..100 Bildern zum Biss), und der Riegel schreibt den Stillstand als Soll fest.

## 0. Stand, Bau, Messmittel

- `git log master..HEAD`: Abnahme-3-Stand (5a724df3) + 98c2c6e4, b227bca9, 84a4f972, c541415c. Eigener Diff des Zweigs =
  `git diff 78a4a748..HEAD` (master ist seit dem Abzweig um 836023f5/565da62b weiter; ein blankes `git diff master` zeigt
  deren Dateien — synchro/STAGE1/room1011.. und den AUFTRAG-Nachtrag — als "geloescht"; nicht aus diesem Zweig).
- Code seit Abnahme 3 (`git diff --numstat 5a724df3..HEAD -- re15_port`): NUR `zombieki_r36.c` (+57/-2: Vortritt in
  `re15_r36_affen_fest_takt` + `re15_r36_affen_padsuche_decide`, Sonden-Leser `re15_r36_affen_fest_lesen`, Sonden-Schalter
  `re15_r36_affen_vortritt_enable`), Header-Doku (+12), Riegel `test_r36_zombieki_affen_block.c` (+106/-1). P1-Code und
  P3-Code unveraendert; gemeinsame Dateien (enemy_ai_common.c, enemy_ai_re2_zombie.c, main.c) seit Abnahme 3 unveraendert.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> "ninja: no work to do",
  `LOCAL-BUILD-OK (build)`. exe 03:09:09, letzte Quellaenderung (Header-Kommentar) 03:08:56, Baum sauber => exe = HEAD
  (md5 c3e7d409...). Messkopie `re15_pc_abn4f_neu.exe` (= HEAD). Vergleich: `re15_pc_abn0f_basis.exe` (v0.8.23-Spielcode,
  md5 2fd81d20...), `re15_pc_abn3f_neu.exe` (NB3 = Abnahme-3-Stand, md5 4b1c5208...). Rohdaten der Abnahme 3 (`F_abn3/`) und
  der NB4 (`F_nb4/`, inkl. vollstaendiger GDB-Laeufe) liegen im gemeinsamen Scratch; nur gelesen. Eigene Laeufe: `F_abn4/`.

## 1. RE-Gate (NB4-Zitate selbst gelesen, richtige Binaerdatei)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (STAGE1.BIN roh @0x80100000).

| Zitat (Code/Dossier/Commit) | Datei | gelesen | Urteil |
|---|---|---|---|
| Biss-Test A[3] "unter 0x1771 nur Biss < 0xbb8 @0x80117a60-74" | STAGE1.BIN | `ori a0,zero,0xbb8` @0x80117a60, `ori a1,zero,0x180` @0x80117a64, `addiu s0,a2,-95` (a2 = 0x800acae7 = Leon +0x93 -> s0 = 0x800aca88 = Leon +0x34) @0x80117a68, `jal 0x8001a804` @0x80117a6c, `bgez v0,0x80117b40` @0x80117a74; danach +0x1dc == 0 -> `sb v0,5(v1)` (Sub 5) @0x80117a98, Variante Sub 15 @0x80117b34-3c. Rest von A[3] (@0x80117b40-c74): Zustandswechsel NUR FERN (`sltiu v0,v0,0x1771` @0x80117c34, `bne` -> Ruecksprung) + Sicht (+0x1d0&1) -> `sb v0,5(v1)` = 4 @0x80117c54; NAH ausserhalb der Bissweite nur +0x6/+0x7 := 0 (@0x80117bf8-c08), Sub bleibt 3 | stimmt |
| Fern-Tor `sltiu 0x1771` @0x80117bc8 | STAGE1.BIN | `lhu v0,468(v1)` @0x80117bc0, `sltiu v0,v0,0x1771` @0x80117bc8 | stimmt |
| B[3] steuert auf playerX @0x80117d4c, `jal 0x8001a8f8` @0x80117d50 | STAGE1.BIN | `lui a0,0x800b` / `addiu a0,a0,-13688` (= 0x800aca88) @0x80117d48-4c, `jal 0x8001a8f8` @0x80117d50, a1 = +0x9e = rng&0x1f + 6 @0x80117d1c-44 | stimmt |
| Radius 1600 @0x80121356 | STAGE1.BIN | Bytes ab 0x80121350 `00 00 60 fa 00 00 40 06` -> u16 @+6 = 0x0640 | stimmt |
| +0x1de @0x80117448-6c, INIT @0x8011706c | STAGE1.BIN | `lhu v0,470(v1)` / `beq v0,zero,0x8011746c` / `lhu v0,478(v1)` / `addiu v0,v0,1` / `j` + `sh v0,478(v1)` / `sh zero,478(v1)`; `sh zero,478(v0)` @0x8011706c | stimmt |
| GDB-Messweg: cur 0x800ac784, Leon 0x800aca54 | STAGE1.BIN | `lw v1,-14460(0x800b0000)` = 0x800ac784 (g_entity(cur)) in A[3]; Leon +0x93 = 0x800acae7 / +0x34 = 0x800aca88 => Basis 0x800aca54 | stimmt (Skript `gdb_affen.py` liest/schreibt genau diese Basen) |
| Schatten-INIT (P3, unveraendert) @0x801171d8-f0 | STAGE1.BIN | `lhu a3,6(v1)` (v1 = +0x78 Record), `addiu a2,a3,100` @0x801171e8, `jal 0x8001af5c` @0x801171ec, `addiu a3,a3,200` @0x801171f0 | stimmt |
| ROOM11C0.RDT #21 @0x01524, #36 @0x015d8, #47 @0x0165c, #13 @0x014c4, #20 @0x01518 | shared_assets/PSX/STAGE1/ROOM11C0.RDT (12-Byte-Saetze w,d,x,z,t,u0,u1,fl) | #21 `e0 15 0e 10 c8 88 10 27 01 ff 00 03` = x -30520..-24920, z 10000..14110, fl 03; #36 `f8 11 3c 0f 24 13 40 e8 01 00 10 02` = x 4900..9500, z -6080..-2180, u1 0x10; #47 x -17397..-12096, z -6590..-1690, u1 0x10; #13 x -28290..-17790, z -28550..8112; #20 x -29390..-17790, z 6800..11470 | stimmt |

- Statischer Befund fuer Mangel 2 der Abnahme 3: A[3] hat NAH (< 0x1771) und ausserhalb des Bissfensters (0xbb8/0x180) KEINEN
  Ausgang; B[3] steuert gerade auf playerX. Ein naher Gorilla, dessen gerader Weg an einer Zelle endet, bleibt im Original in
  Sub 3 stehen — das ist die Original-Regel (bestaetigt).
- Code-Diff des Zweigs (`git diff 78a4a748..HEAD -- re15_port/engine re15_port/include re15_port/platform`, nur `+`-Zeilen)
  nach deferred/tunable/interim/for now/faithful/plausib/TODO/FIXME/approx/schaetz/ungefaehr/vorerst/vorlaeufig/getenv/
  "sieht richtig"/geraten: **0 Treffer**. Schalter `re15_r36_*_enable` (7 Stueck, darunter NEU `re15_r36_affen_vortritt_enable`)
  haben in engine/ und platform/ KEINEN Aufrufer (nur Sonden).
- Neue Stellgroessen der NB4: `R36_VORTRITT_START = R36_FEST_START / 2 = 22` und der Kontaktradius `r_q + r_e + r_e/2`
  (Rasterschritt) — beide im Code und in den Commits 84a4f972/c541415c als PORT-WAHL mit Messreihe gekennzeichnet
  (K 1/8/15/22/30/37/45/60); 22 liegt innerhalb der Reihe (Abnahme-3-Hinweis "Rand der Reihe" fuer 45 hier nicht mehr
  zutreffend). Die Reihe ist nicht monoton (37 schlechter als 30 und 45) — eine per Messung gewaehlte Stellgroesse, kein
  Original-Wert, und so benannt.
- Commit-Messages b227bca9/84a4f972/c541415c tragen die Adressen (0x80116db8, 0x800787dc, 0x800ac784, 0x800ace20/0x800ad014/
  0x800aca54, @0x80117448-6c, @0x80117bc8, @0x80121356, @0x80117d4c, @0x80117a60-74, @0x80117fe4-80118024, ROOM11C0.RDT
  0x01524/0x015d8).

## 1a. GDB-Messung des Originals (Rohdaten selbst nachgerechnet)

Die je-30-Auszuege im Repo (`F_belege/nb4/g_o*_je30.txt`) und die VOLLSTAENDIGEN Laeufe im Scratch (`F_nb4/g_o*.txt`, je eine
Zeile pro Bild) selbst ausgewertet (Skript im Pruefer-Scratch, Spalten = Zeilenformat von `gdb_affen.py`):

| Lauf | Zeilen (Bilder) | e1 | e2 | Leon |
|---|---|---|---|---|
| O3 Stau | 900 (VSync 10029..11827) | NUR Sub 3, x -23318..-23302, z 14225..14510, Sicht-Latch 1 | NUR Sub 4, z 16118 konstant, x -20790..-20567, +0x1de 179 -> 1078 | HP 100 je Bild, Lage fest |
| O3b allein | 900 | NUR Sub 3, x -23305..-22957, z 14484..14603, d <= 3647, kein Angriff | Sub 0/3, weit weg | HP 100 |
| O4 Ostgasse | 900 | NUR Sub 3, Ende (5871,-8818) d 3093, +0x1de bis 898 | Sub 3/4, Ende (3045,-10340) d 5394, +0x1de bis 1078 | HP 100 |
| O5 Nordende NW-Gang | 900 | NUR Sub 3, x -13874..-13520, z 11905..11960, +0x1de STETIG bis 898 | Sub 4/6, FERN bei (-12316,-8452) | HP 100 |
| O1 Leon (6000,-9000) | 2400 | Sub 3/4/5/6 (Angriffe), schiebt Leon bis (7880,-1379), dann Gassenmitte (6418,-4282..-4421) | Sub 3/4/5 | HP 100 |

Die Dossier-Tabelle "NB4 Mangel 2" ist damit gedeckt (Kleinigkeit: O3 e2 +0x1de endet bei 1078, Dossier/Commit nennen 1079).
Der Messweg ist plausibel abgesichert: Haltepunkt 0x80116db8 (Gorilla-Wurzel), cur-Zeiger 0x800ac784 und Leon-Basis 0x800aca54
sind dieselben Adressen, die A[3] selbst verwendet (Abschnitt 1); `g_kalib.txt` (ohne Patch) liefert e1 (-5525,-14883),
e2 (-8999..-8609, ...) = die Original-Lage F195 des Riegels. Den GDB-Lauf selbst habe ich NICHT wiederholt (er schreibt
`%LOCALAPPDATA%/DuckStation/settings.ini` um — ausserhalb des Baums; Auftrag: nichts ausserhalb aendern).

## 1b. Vertrag

- Pfade `git diff 78a4a748..HEAD --name-only`: nur analysis/befunde_runde36/F_*, engine/src/{enemy_ai_common.c,
  enemy_ai_re2_zombie.c, zombieki_r36.c}, include/re15_zombieki_r36.h, platform/pc/main.c, tests/unit/probes/r36_zombieki.cmake,
  tests/unit/test_r36_zombieki_* — keine release/, platform/android/, shared_assets/PSX/, Test-CMakeLists, RELEASE_NOTES.
- Gemeinsame Dateien (numstat): enemy_ai_common.c +32/-11 (Haken je 1-2 Zeilen + zwei Sicht-Exporte; Spur F = Eigentuemer der
  Sichtfilter-Haken laut VERTRAG 1.4), enemy_ai_re2_zombie.c +2/-1, main.c +4 — seit Abnahme 3 unveraendert.
- zombieki_r36.c / re15_zombieki_r36.h: kein flag_set / msg_install / scd_event_fire — keine Bank-9-Bits, Nachrichten-IDs,
  Ereignisse. Tests nur ueber `probes/r36_zombieki.cmake`.

## 2. Gezielte Regression (keine Voll-Suite im Spurbaum)

Regex = Abnahme-3-Regex (Module/Raeume des Zweig-Diffs) **erweitert** um die uebrigen Gegner-KI-Tests, weil enemy_ai_common.c
alle Gegner traegt (`spider_ai|adult_spider|cockroach|unit_npc_ai|zgirl_ai|birkin|writher|alligator|fx_emitter|tyrant|ivy_ai|
re2_spider|baby_spider|gator|g5_|spinne|autofire|r35_entladen_gegner`):
`ctest --test-dir re15_port/build -R "<regex>" --timeout 600 -j 6 --output-on-failure` (03:12-03:21) ->
**100% tests passed, 0 tests failed out of 232** (509,3 s). Darin alle 6 Spur-Tests (`unit_r36_zombieki_{wecker,szenen,affen,
sicht,affen_block,eintritt}`), alle 20 `unit_r35_affen_*` (Spur-J-Nahjagd bildgenau gegen GDB, u. a. `griff`, `szene`, `takt`),
`unit_maggot_*`, `unit_r17_waffen_loop_pin`, `integration_r35_cut1150` (507 s), `integration_r35_zgirl`. Kein Fenster-Haken rot.
Die Suite zaehlt im Spurbaum 598 Tests (master 592 + 6 Spur-Tests).
- Zufallsgang-Stichprobe selbst nachgefahren (`test_r36_zombieki_affen_block eins <i> 1 6000 400`, Seeds 0..11, je eigener
  Prozess, Standard-Schwelle): **12/12 Zeilen bitgleich** zu `F_belege/nb4/seeds/s400d_k22.txt`. Summen der Dossier-Dateien
  nachgerechnet: k22 Anker 400 16352 / 242 Griffe (nah 13583 / fern 2769), Anker 40 125; vt0 (= NB3) 17975 / 245 — wie Dossier.
- Riegel direkt `test_r36_zombieki_affen_block riegel` (`F_abn4/riegel_affen_block.txt`): **54 Pruefungen, 0 Fehler**. Stau
  Modus 0 / 3 / 1: G1 0/0/91, G2 0/0/101 Angriff; G2 FERN 900/900/0. Pins: O3b Ende (-23272,14587) gegen Original (-23302,14597),
  kein Angriff; O1 (6418,-4399) gegen (6418,-4409), fest 1368; O5 (-13557,11920) gegen (-13527,11914), kein Angriff.

## 3. Punkt 1 — "Zombies ... bleiben hier ewig beteiligungslos stehen, obwohl ich ziemlich in Ihrer Naehe vor ihnen bin, fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind."

P1-Code seit Abnahme 2 unveraendert (NB3/NB4 aendern nur Gorilla-Funktionen in zombieki_r36.c). Gemessen: (a) Bitgleichheit
der Zombie-Laeufe der Abnahme 3 mit der HEAD-exe, (b) ein NEUER echter Tuerweg (keine fruehere Abnahme).
- exe HEAD `re15_pc_abn4f_neu.exe`, Kommandozeilen der Abnahme 3 (Basis `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp
  RE15_TITLE_SHOT_AF=2 RE15_GEGNER_Y_LOG=gy.log`, Skript `F_abn4/run.sh`), gy.log byteweise (`cmp`) gegen die NB3-Laeufe der
  Abnahme 3 (`F_abn3/*_abn3f_neu`, dort "erfuellt"):
  - ROOM1040 Westgang (`RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1040@250 RE15_PLAYER_POS=-29900,6000,0,0 RE15_EXIT_AT=900`):
    **bitgleich** (1151 Zeilen).
  - ROOM10B0 Terrasse (`RE15_DEBUG_JUMP=10B0@250 RE15_PLAYER_POS=-17000,-8500,2048,0 RE15_EXIT_AT=2400`): **bitgleich** (2651).
  - Echter Tuerweg ROOM1020 -> ROOM1010 (`RE15_DEBUG_JUMP=1020@250 RE15_PLAYER_POS=-2950,-17700,1024,0 RE15_INPUT_SCRIPT_BASIS=spiel
    RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=U0.5,W0.3,A0.1,W8,U0.7,W60 RE15_EXIT_AT=2600`): **bitgleich** (5082).
  - Echter Tuerweg ROOM1020 -> ROOM1040 (`RE15_PLAYER_POS=-26300,-8200,2048,0 ... RE15_INPUT_SCRIPT=U0.3,W0.3,A0.1,W8,U1.5,W60
    RE15_EXIT_AT=2600`): **bitgleich** (5845).
  - Abnahme-1-Fall ROOM1010 (`RE15_PLAYER_POS=3750,4300,1024,0 RE15_EXIT_AT=1500#1010`; endet wie in Abnahme 3 erst am
    Zeitlimit, rc=124): R1010-Teil (780 Zeilen) **bitgleich**.
- **NEUER echter Tuerweg ROOM1210 -> ROOM1220** (Zellentrakt; von keiner frueheren Abnahme gefahren; Tuer-AOTs laut
  `test_r36_zombieki_eintritt aot 1210`: aot1 (-21450,-6100) -> Ankunft (-22400,-6500) Cut 0, aot3 (-21650,-13600) -> (-22400,
  -14000) Cut 4; `RE15_AI_FLAVOR=re2 RE15_DEBUG_JUMP=1210@250 RE15_PLAYER_POS=-20600,-6100,2048,0` bzw. `-20800,-13600,2048,0`
  `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT=U0.3,W0.3,A0.1,W8,W60 RE15_STATE_LOG=st.log
  RE15_EXIT_AT=2000`, HEAD und v0.8.23, `F_abn4/t1220{a,c}_*`): 38 Bilder 1210, dann 1220 per Tuer. HEAD und v0.8.23 je Tuer
  im Verhalten gleich.
  - Tuer aot1 (Cut 0): zwei Typ-0x16-Zombies (gy.log `t=` ist hex) mit Spawn-Byte 0x81 bei (-25398,-6739) d 2335 und
    (-23500,-9700) d 3228 bleiben 2000 Bilder in Zustand 1/2/1, Clip 23, af 9, Sicht 1, Leon HP 100. Gegengeprueft: das sind
    KRIECHER (Clip 23 = Kriecher-Eintritt `sw 23,332` vor @0x80100B18 laut Port-Kommentar) im RE2-Entscheider DECIDE[2] —
    selbst disassembliert in `info/re2leon/COMMON/BIN/EMZ0.BIN`: `jal 0x80015614` a3=1024 (verworfen) @0x80103a98, a3=512
    @0x80103ab4, `sltiu v0,s1,0x514` @0x80103ac4 (s1 = +0x1F0 = Abstand), Sicht +0x154&0x800 @0x80103ae8-f4, dann `sw 257,4(s0)`
    @0x80103b10-14. Der RE2-Kriecher lauert also bis Leon < 1300 im 512er-Sektor ist; selbst gefahren mit der Spur-Sonde
    (`test_r36_zombieki_eintritt spur 1220 1210 0 <1|2> 1 300`, Leon geht hin): beide wachen auf und greifen Leon (grab1 bei
    F30, d 1389 bzw. 1592). Kein P1-Fall (liegend, kein "stehen ... vor ihnen"); der Zensus fuehrt sie als Klasse 4 "lauert"
    (der Vorher-Zensus `nb1_eintritt_vorher.txt` hatte sie noch als PASSIV).
  - Tuer aot3 (Cut 4): S6 (Gesicht zu Leon) geht ab +30 los und greift +240; S5 bei (-25950,-13650) rot 2048 (Blick nach Westen,
    Leon oestlich HINTER ihm) steht 390 Bilder (1/0, Sicht 1, d 2896), geht dann los und greift an; Leon stirbt (~+1016). Der
    Wortlaut-Fall ("vor ihnen ... vor ihrer Nase") ist das nicht; S6 zeigt die sofortige Reaktion von vorn.
- **Urteil P1: erfuellt.** P1-Code unveraendert, alle sechs Abnahme-3-Laeufe der HEAD-exe bitgleich zum dort mit "erfuellt"
  bewerteten Stand; der neue Tuerweg 1210 -> 1220 zeigt von vorn sofortige Reaktion und Griff, die Ausnahmen (Kriecher-Lauern,
  Ruecken zu Leon) sind RE2-Regeln mit selbst gelesenem Beleg.

## 4. Punkt 2 — "Die Monkeys scheinen noch oft blockiert zu sein. Das muessen wir irgendwie verbessern."

### 4.1 exe Kampf-Layout, 14 Stellen der Abnahme 3 (HEAD, je eigener Prozess)
`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700` (+ Basis-Haken), keine Eingabe; alle
14 Laeufe selbst gefahren (`F_abn4/affen_<x,z>_abn4f_neu/`, je rc=0). Auswertung mit EIGENEM Skript `F_abn4/ausw4.py`
(Definition der Abnahmen 2/3: fest = aktiv, kein Angriff Sub 5/6/8/15, Leon > 3000, >= 90 Bilder im 400er-Anker; fern =
d >= 6001); v0.8.23/NB3 = die gy.log-Dateien der Abnahme 3. **Alle 14 HEAD-gy.log byteweise gleich den nb4h-Laeufen des
Bau-Agenten**; Stellen 1-10 und 13/14 zusaetzlich gleich NB3.

| # | Leon | v0.8.23 S2/S3 Angriff (fest; fern; FERN nach 1. Angriff) | NB3 | **HEAD** |
|---|---|---|---|---|
| 1 | (-7000,12500) | nie / F1012 (1882/0; 1882/0; 1283/0) | nie / F730 (826/0; 0/0; 0/0) | nie / F730 (826/0; 0/0; 0/0) |
| 2 | (-14000,2000) | F680 / nie (0/930; 0/930; 0/629) | F680 / nie (0/0) | F680 / nie (0/0) |
| 3 | (6000,-9000) | nie / F324 (1573/1875; 0/0) | nie / F324 (1653/1875; 0/0) | nie / F324 (**1653/1875**; 0/0) |
| 4 | (-24000,10000) | nie / nie (1819/1418; alle fern) | F853 / F1570 (0/1193; 0/1) | F853 / F1570 (0/**1193**; 0/1) |
| 5 | (7000,4000) | nie / F18 (1012/0; 1012/0) | nie / F18 (96/0; 96/0) | nie / F18 (96/0; 96/0) |
| 6 | (9000,10000) | nie / F309 (1102/0; 1102/0) | F1175 / F309 (0/0) | F1175 / F309 (0/0) |
| 7 | (-14746,-4140) | F364 / nie (1835/0; 0/0) | F364 / F1021 (179/108; 0/108) | F364 / F1021 (179/108; 0/108) |
| 8 | (7200,-4130) | F1246 / F274 (0/670) | F1050 / F274 (0/468) | F1050 / F274 (0/468) |
| 9 | (-14000,4000) | F704 / nie (0/1088; 0/1088) | F704 / F1496 (250/0) | F704 / F1496 (250/0) |
| 10 | (-14500,6000) | F694 / F1256 (171/298; 0/298) | F694 / F892 (0/0) | F694 / F892 (0/0) |
| 11 | (-24452,11938) | nie / nie (1821/1588; alle fern) | F828 / nie (1529/1465; 0/1465; **0/1465**) | **F828 / F1253 (0/1335; 0/0; 0/0)** |
| 12 | (-22288,11938) | nie / nie (1769/1672; alle fern) | F868 / F1025 (760/894; 0/691; **0/691**) | **F868 / F1025 (0/833; 0/50; 0/50)** |
| 13 | (-26500,15500) | nie / F1620 (1841/90; alle fern) | F842 / nie (0/1650) | F842 / nie (0/**1650**; 0/0) |
| 14 | (-20000,14500) | nie / F1541 (1482/0; 1482/0) | F883 / F896 (0/0) | F883 / F896 (0/0) |

Summen fest (fern / nah): v0.8.23 25936 (19812 / 6124) -> NB3 12946 (2361 / 10585) -> **HEAD 10466 (255 / 10211)**; FERN nach dem
ersten Angriff 6868 -> 2361 -> **255**. Damit ist das **Abnahmekriterium von Abnahme-3-Mangel 1 erfuellt**: an beiden
Nordwest-Rand-Stellen kein Gorilla >= 90 Bilder FERN nach dem ersten Angriff (11: 0, 12: 50); Slot 3 greift an Stelle 11 an
(F1253). Die Angabe "fern 0" ist in Dossier (NB4 "Berichtigung") und Commit c541415c berichtigt.

Aber: die NAH-fest-Bilder sind gegenueber v0.8.23 GESTIEGEN (6124 -> 10211). An den Nordwest-Stellen 4/11/12/13 kamen die
Gorillas in v0.8.23 gar nicht an (alles FERN); jetzt bringt sie der Umweg hin, und danach steht EINER von ihnen in der
Original-Nahjagd an der #21-Ecke (Stelle 11: F1366+1335 bei (-23061,13333) d 3358..5472; 12: 472 am Stueck bei
(-23302,13082); 13: 1650; 4: 1193). An Stelle 3 stoesst Slot 3 Leon F324 in die Ostgasse (Leon (5858,-5716)) und steht danach
F826-F2700 (1875 Bilder, 62 s) am Gasseneingang (5876,-8818) d 3102, Slot 2 1653 Bilder bei (3030,-10336) d ~5400 — kein Angriff
mehr (Spur `F_abn4/affen_6000,-9000_abn4f_neu/gy.log`, je 150 Bilder ausgegeben).

### 4.2 Vortritt (Mangel 1) selbst nachgemessen, Sonde `lage` (Riegel-Binary, je eigener Prozess, Spur je 30 Bilder)
- Riegel-Lage O3 (Leon (-26606,14578), G1 (-23302,14489) Sub 3, G2 (-20579,16118) Sub 4): Modus 0 (Original-Regel) und 3 (NB3)
  900/900 fest, G2 900 FERN (= GDB-Original); Modus 1 (HEAD): ab F24 Vortritt fuer G1 (`vt1 uw1`), G1 laeuft ueber den
  Nordstreifen (F60 (-23302,15685), F90 (-25273,16118)) und beisst F91, danach im 52er-Takt; G2 springt (Sub 7) an die #21-Ecke
  nach und steht dort 713 Bilder NAH (d 3436..4509) ohne Angriff (`F_abn4/lage_o3_m{0,3,1}.txt`).
- EIGENE Varianten (nicht im Riegel): V2 Slots vertauscht -> spiegelbildlich (G2 beisst F83, G1 F117, danach G1 664 nah an der
  #21-Ecke); V1 Leon an Stelle 13 (-26500,15500) -> G1 beisst F67, G2 514 nah an der #21-Ecke; Modus 0/3 jeweils 825-900/900
  fest. Der Vortritt ist symmetrisch und loest den FERN-Stau; es bleibt jeweils EIN Gorilla nah an der #21-Ecke stehen.
- **V3 konstruierter Stau vor der Ostgasse** (Leon (5858,-5716) = O4-Lage in der Gasse, G1 NAH am Eingang (5876,-8818) Sub 3,
  G2 FERN dahinter (5876,-12000) Sub 4): Modus 0 G1 900/900 am Eingang d 3102 (= GDB O4: e1 (5871,-8818) d 3093); Modus 1: G1
  bekommt F24 den Vortritt, geht ZU FUSS 510 Einheiten nach Osten (F24 (5880,-8818) -> F50 (6387,-8818), Sub 3), dort 166 weiter
  nach Norden (F52 (6437,-8652), d 2992, Sub 3) und **beisst F54** (d 2717 < 0xbb8); danach greifen beide an (G2 F674), Leon wird
  nach Norden geschoben (`F_abn4/lage_v3_ostgasse_m{0,3,1}.txt`, Feinspur je 2 Bilder im Pruefer-Protokoll). Die Wartelage am
  Gasseneingang liegt also nur 102 Einheiten ausserhalb des Bissfensters (d 3102 gegen 0xbb8 = 3000).

### 4.3 Bewertung der Nah-Wartezeit (Mangel 2 der Abnahme 3)
- Original gemessen: ja — die GDB-Laeufe O1/O3/O3b/O4/O5 liegen vollstaendig vor und decken die Dossier-Tabelle (Abschnitt 1a);
  statisch bestaetigt A[3]/B[3] (Abschnitt 1). Im Dossier und unter "Fuer den Nutzer" benannt: ja.
- **Die Begruendung an den Nutzer ist aber falsch.** "Fuer den Nutzer" (Dossier Z. 1073-1077): "Steht Leon in einer Ecke oder
  Gasse, in die ein Gorilla **nicht hineinpasst** — die Nordwest-Ecke hinter dem Block, die schmale Ostgasse zwischen den Wagen
  —, wartet der Gorilla davor"; Dossier Z. 960-961 / Commit c541415c: "In die Ostgasse (2711 breit < 2 x 1600, Pad #36 ...)
  kommt kein Gorilla zu Fuss". Gemessen: (a) Ostgasse — V3: ein naher Gorilla erreicht Leon aus GENAU der Wartelage der exe-Stelle 3
  ((5876,-8818), Leon (5858,-5716)) mit 510 Einheiten Seitenschritt + 166 nach Norden zu Fuss und beisst nach 30 Bildern; im
  GDB-Original O1 steht e1 am Ende sogar in der Gassenmitte (6418,-4282..-4421), die das Dossier selbst so nennt ("e1 in der
  Gassenmitte (6418,-4330)"). (b) Nordwest-Ecke — der Vortritt selbst fuehrt den nahen Gorilla ueber den
  Nordstreifen z 15710..16118 in Bissweite (Riegel: Biss F91; V1/V2: F67/F83). Der Stillstand kommt also nicht vom Platz, sondern
  davon, dass B[3] GERADE auf playerX steuert (@0x80117d4c) und A[3] nah keinen anderen Ausgang hat — die Umweg-Mechanik des Ports
  loest genau diese Lagen, wird aber fuer EINEN nahen Gorilla nicht eingesetzt.
- Die Begruendung "ein Gate dagegen wuerde die Spur-J-Riegel griff/szene brechen" ist nur fuer den NB3-Beginn (45 Bilder im
  Anker) gemessen. Was griff/szene pinnen: `griff` N1 "kein Biss bis T495" nach der Freigabe T378 (~117 Bilder), `szene` den
  kanonischen Kampf (Heavy-Treffer Freigabe + 361..367, Biss-Takt 51,8, Tod + 1181..1205). Die laengste Nah-Wartezeit des
  kanonischen Kampfes im echten Tuerweg (unten 4.4) ist 408 Bilder; die Rest-Wartezeiten an den Stoss-Zielen sind 783..1875
  Bilder. Ein spaeterer Beginn (z. B. ab ~450 Bildern Nah-Haengen) ist weder gemessen noch verworfen.
- Die neuen Pins O3b/O5 (`greift in 900 Bildern nicht an`) und O1 schreiben die Nah-Wartezeit als Soll fest.

### 4.4 Echte Wege (Tuer / Flucht)
- **Echter Tuerweg ROOM11B0 -> ROOM11C0, Szenen-Layout** (neu; Tuer-AOT aot1 (-25480,-28190) laut `test_r36_zombieki_eintritt aot
  11B0`, Ziel laut `tools/engine_tueren.txt` "11B0 11C0 -25480 -28190 -25279 17268"; `RE15_SET_FLAG=4:243,3:130
  RE15_DEBUG_JUMP=11B0@250 RE15_PLAYER_POS=-25552,-25331,1024,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20
  RE15_INPUT_SCRIPT=U1.0,W0.3,A0.1,W8,W100 RE15_STATE_LOG=st.log RE15_EXIT_AT=3400`): 59 Bilder 11B0, 2541 Bilder 11C0 (per Tuer),
  Ankunftsszene, dann der kanonische Kampf bei Leon (-7148,-12363): S2 wartet F1143+408 bei (-4936,-14532) d 3009..3149, greift
  F1551 an, S3 F1430; beide greifen danach fortlaufend an, Leon stirbt (~F2541). Kein FERN-Haengen.
- Kanonischer Kampf per Sprung (Kommandozeile der Abnahme 3, `RE15_SET_FLAG=4:243,3:130 RE15_DEBUG_JUMP=11C0@250
  RE15_PLAYER_POS=-7000,12500,2048,0 RE15_EXIT_AT=1700`): gy.log UND st.log **bitgleich** zu NB3 (= v0.8.23 laut Abnahme 3).
- Flucht durch die Westgasse (`RE15_PLAYER_POS=-14500,-9000,3072,0 ... RE15_INPUT_SCRIPT=W15,XU6,W60`): **bitgleich** zu NB3.
- **Echter Tuerweg ROOM11B0 -> ROOM11C0, Kampf-Layout** (neu; derselbe Weg mit `RE15_SET_FLAG=4:64,4:243,3:130`, also Parkplatz
  schon erreicht = Wiederkehr; Leon kommt im Nordwesten an und bleibt bei (-26932,17268) stehen; `RE15_EXIT_AT=3000`; HEAD, NB3,
  v0.8.23, `F_abn4/t11b0kampf_*`):

  | Stand | S2 | S3 | Leon |
  |---|---|---|---|
  | v0.8.23 | nie; 2041 Bilder FERN festgefahren bei (-16227,-4878) d ~24000 | erster Angriff F1443, dann fortlaufend | lebt bis F3000 |
  | NB3 | nie; kommt ueber die Westgasse auf den Nordstreifen, steht ab ~F1200 hinter S3 bei (-22447,16118), **1309 Bilder NAH** (d 3841..4725), laengste Strecke F1991+483 | **F866**, dann fortlaufend (652 Angriffsbilder) | stirbt ~F2473 |
  | **HEAD** | **bitgleich NB3** (gy.log `cmp`) | bitgleich NB3 | bitgleich NB3 |

  Der Vortritt greift hier nicht (S2 und S3 beide NAH, Koerperkontakt 3195): im realistischsten Nordwest-Fall steht der zweite
  Gorilla 44 s nah hinter dem ersten, ohne anzugreifen. Verbessert gegenueber v0.8.23: kein FERN-Haengen, erster Angriff 577
  Bilder frueher.

## 5. Punkt 3 — "Bei den Monkeys in ROOM 11C0 laeuft das Blut nicht aus wenn sie tot sind."

- Code seit Abnahme 2 unveraendert (main.c-Haken `re15_r36_affen_lache_render`, `re15_r36_affen_lache_{init,tick}`, INIT-/Leichen-
  Haken in enemy_ai_common.c). RE-Stichprobe selbst gelesen (STAGE1.BIN): `ori v0,zero,0x5a` / `sb v0,158(a0)` @0x8011be80-84
  (90 Takte), `lui a2,0xff` @0x8011be6c + `ori a2,a2,0xff38` @0x8011bed0 -> +0xc4/+0xec = (alt & 0xff000000) | 0x00ffff38
  @0x8011bee0-bf00, `addiu v0,v0,8` / `addiu v1,v1,8` / `sh v0,188(a0)` / `sh v1,190(a0)` @0x8011bf08-14 (+0xbc/+0xbe += 8);
  Record-Groesse 1600 + 100 / + 200 @0x801171d8-f0 (Abschnitt 1) — Code-Kommentare zitieren genau das.
- exe HEAD (`re15_pc_abn4f_neu.exe`, Kommandozeile der Abnahme 3: `RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240
  RE15_PLAYER_POS=-9013,-11800,1024,0 RE15_GIVE=7:200 RE15_EQUIP=7 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=20
  RE15_INPUT_SCRIPT=W0.1,(MA0.1,M0.6) x45 RE15_FORCE_CUT=5 RE15_WINDOW_SCALE=1 RE15_FRAMEDUMP=200-700/10:fd_ RE15_EXIT_AT=700`,
  Scratch `F_abn4/p3lache_abn4f_neu/`): **alle 51 Framedumps und das gy.log bitgleich zum Abnahme-3-Lauf**. Slot 2 Leiche
  (Zustand 7) ab F300 bei (-7478,-12875). Eigene Zaehlung rot-dunkler Pixel (G,B <= 24, R >= G+14) im Fenster (120..260,
  90..160): F290 292, F300 294, F330 340, F360 422, F380 465, **F390 494**, F400..F570 494 — Wachstum ueber ~90 Bilder ab dem
  ersten Leichentakt (= 0x5a), dann konstant. Bildstreifen `F_abn4/p3_lache_streifen_abn4.png` (F260/280/300/330/360/400/450/550)
  selbst angesehen: unter der Leiche erscheint eine dunkelrote Lache und waechst, danach bleibt sie liegen.
- Riegel `unit_r36_zombieki_affen` gruen (Abschnitt 2).
- **Urteil P3: erfuellt.**

## 6. Gates

- **Bau:** OK (`local_build.sh configure` + `build`, "ninja: no work to do", exe = HEAD; Abschnitt 0).
- **Gezielte Regression:** 232/232 gruen (Abnahme-3-Regex + alle uebrigen Gegner-KI-Tests; Abschnitt 2), darin alle 6 Spur-Tests
  und alle 20 Spur-J-Riegel; `test_r36_zombieki_affen_block riegel` direkt 54/54; Zufallsgang-Stichprobe 12/12 bitgleich.
- **RE-Gate:** alle NB4-Zitate selbst gelesen und stimmen (Abschnitt 1, 8 Zeilen), dazu P3 @0x8011be6c-bf14 und RE2 DECIDE[2]
  @0x80103a98-b14 (EMZ0.BIN); GDB-Rohdaten nachgerechnet (Abschnitt 1a). 0 Guess-Tells, keine Env-Schalter als Abschluss; neue
  Stellgroessen (22, Kontakt + Radius/2) als PORT-WAHL mit Messreihe gekennzeichnet.
- **Fix erklaert den Befund?** Abnahme-3-Mangel 1 (FERN-Stau): ja — der nahe Gorilla versperrt den einzigen Streifen ueber #21,
  der Vortritt gibt ihn frei (Sonde O3: 900/900 -> Biss F91/F101; exe 11/12 FERN 1465/691 -> 0/50). Abnahme-3-Mangel 2: das
  Original ist gemessen und wartet ebenso; die dazu gegebene ERKLAERUNG ("passt nicht hinein") stimmt aber nicht (Abschnitt 4.3).
- **Vertrag:** OK (Abschnitt 1b).
- **Nebenwirkungen:** Zombie-Raeume bitgleich zum Abnahme-3-Stand (6 Laeufe); kanonischer Gorilla-Kampf (Sprung und echter
  Tuerweg) und Westgassen-Flucht bitgleich zu NB3; Gorilla-Code nur Typ 0x27 (nur ROOM11C0); andere Gegner-Tests gruen.
- **Volle Suite:** im Spurbaum nicht gefahren (Auftrag) — Orchestrator im Integrationsbaum.

## 7. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Zombies bleiben ewig beteiligungslos stehen, fast vor der Nase, auch innerhalb eines Raumes | **erfuellt** | P1-Code unveraendert; 6 Laeufe der Abnahme 3 (1040 Westgang, 10B0, Tuerwege 1020->1010 / 1020->1040, 1010-Fall) bitgleich; neuer Tuerweg 1210->1220: Zombie von vorn sofort los und Griff +240; liegende Kriecher lauern nach RE2 DECIDE[2] (@0x80103ac4 `sltiu 0x514`, selbst gelesen) und greifen beim Naeherkommen (F30). |
| 2 Monkeys ROOM11C0 oft blockiert — verbessern | **teilweise** | Verbessert: FERN-Haengen fast weg (14 Stellen 19812 -> 255, nach dem ersten Angriff 6868 -> 255; Zufallsgang 114029 -> 16352), Nordwest-Rand-Stau geloest (Slot 3 greift an 11 an). Nicht erfuellt: an den Stoss-Zielen steht ein Gorilla weiter 783..1875 Bilder NAH ohne Angriff (Stellen 3/4/11/12/13), im echten Tuerweg ins Kampf-Layout S2 1309 Bilder hinter S3; NAH-fest an den 14 Stellen 6124 (v0.8.23) -> 10211; die Port-Umweg-Mechanik loest genau diese Lagen (V3 Biss F54, Vortritt F67..F101), wird fuer einen einzelnen nahen Gorilla aber nicht eingesetzt — begruendet mit "passt nicht hinein" (gemessen falsch) und "bricht griff/szene" (nur fuer den 45-Bilder-Beginn gemessen). |
| 3 Blut der toten Monkeys laeuft nicht aus | **erfuellt** | exe HEAD: 51 Framedumps + gy.log bitgleich zum Abnahme-3-Lauf; Lache 292 -> 494 Pixel ueber ~90 Bilder (0x5a @0x8011be80), dann konstant; Bildstreifen selbst angesehen; RE @0x8011bed0-bf14 selbst gelesen. |

## 8. Maengel (nummeriert, nachpruefbar)

1. **P2 — Nah-Stillstand an den Stoss-Zielen bleibt und ist an den Pruefstellen gegenueber v0.8.23 gestiegen.** exe HEAD
   (`RE15_SET_FLAG=4:64 RE15_DEBUG_JUMP=11C0@240 RE15_PLAYER_POS=<x,z>,1024,0 RE15_EXIT_AT=2700`, Abschnitt 4.1): Stelle 3
   (6000,-9000) — Slot 3 stoesst Leon F324 in die Ostgasse und steht danach 1875 Bilder am Eingang (5876,-8818) d 3102 (102
   ausserhalb 0xbb8), Slot 2 1653 Bilder bei (3030,-10336); Stellen 4/11/12/13 — Slot 3 steht 1193/1335/833 (davon 50 FERN)/1650
   Bilder (laengste Strecke am Stueck 920/1335/472/1518) an der #21-Ecke (-23061..-23302, 13082..13333) d 3211..5514; echter
   Tuerweg 11B0 -> 11C0 Kampf-Layout (Abschnitt 4.4) — S2 steht 1309 Bilder (laengste Strecke 483) nah hinter S3 (d 3841..4725).
   NAH-fest Summe 14 Stellen 6124 -> 10211. Dieselbe Lage loest die vorhandene Umweg-Mechanik in 30..100 Bildern (V3: 510
   Seitenschritt + 166 vor, Biss F54; Nordwest-Ecke ueber den Nordstreifen, Biss F67/F83/F91). **Kriterium:** an den Stellen 3,
   4, 11, 12, 13 und im Tuerweg `t11b0kampf` (Kommandozeilen Abschnitte 4.1/4.4, Leon steht) steht kein Gorilla in SUMME mehr als
   450 Bilder NAH fest (Definition 4.1; der kanonische Original-Kampf im echten Tuerweg hat in Summe 408), `unit_r35_affen_griff/
   _szene` und alle `unit_r35_affen_*` bleiben gruen, ein Riegel pinnt das mit Gegenprobe (Original-Regel = Befund). Ist das
   nachweislich nicht ohne Bruch von griff/szene moeglich, gehoert die MESSUNG dafuer ins Dossier (Gate-Beginn N Bilder ->
   welcher Pin mit welchem Wert rot), fuer einen Beginn oberhalb des J-Fensters, nicht nur fuer 45.
2. **P2 — falsche Begruendung an den Nutzer und im Dossier/Commit.** "Fuer den Nutzer" (F_zombieki.md Z. 1073-1077): "in die ein
   Gorilla nicht hineinpasst — die Nordwest-Ecke hinter dem Block, die schmale Ostgasse"; Z. 960-961 und Commit c541415c:
   "In die Ostgasse (2711 breit < 2 x 1600 ...) kommt kein Gorilla zu Fuss". Gemessen falsch (Abschnitt 4.2/4.3): an der Ostgasse
   kommt ein naher Gorilla mit 510 Seitenschritt zu Fuss in Bissweite (V3), im GDB-Original O1 steht e1 in der Gassenmitte
   (6418,-4282..-4421); die Nordwest-Ecke erreicht der Vortritt-Gorilla ueber den Nordstreifen. Richtig ist: B[3] steuert gerade
   auf playerX (@0x80117d4c) und A[3] hat nah nur den Biss (@0x80117a60-74); der Gorilla bleibt an der Klemme knapp ausserhalb
   des Bissfensters haengen. Text an beiden Stellen nach Messung berichtigen.
3. **P2 — Riegel schreibt den Stillstand als Soll fest.** `unit_r36_zombieki_affen_block` prueft O3b/O5 "greift in 900 Bildern
   nicht an" und O1 "wartet NAH" im PORT-Modus (`lage <b> 1 ...` = Umweg + Vortritt an). Damit wird jede Verbesserung von Mangel 1
   rot. Die Gleichheit mit dem GDB-Original gehoert in den Modus Original-Regel (`lage <b> 0 ...`), wie es der Stau-Block schon tut.

Hinweise (kein Mangel): (a) O3 e2 +0x1de endet in den Rohdaten bei 1078 (Dossier/Commit: 1079). (b) PSX-Laufzeit: der Vortritt
ruft fuer den nahen Gorilla je Bild `r36_uw_route` (A*); bleibt unter OFFEN mit Plan (Tabelle fuer ROOM11C0). (c) ROOM1220-Kriecher
und der Zombie mit dem Ruecken zu Leon (Abschnitt 3) sind RE2-Regeln, kein P1-Fall; der Vorher-Zensus hatte die Kriecher als
PASSIV, der jetzige Zensus fuehrt sie als "lauert". (d) Stelle 7 Slot 3 108 und Stelle 5 Slot 2 96 Bilder FERN (vor dem eigenen
Angriff, nicht am Nordwest-Rand) wie NB3, von Abnahme 3 nicht beanstandet. (e) Volle Suite: Orchestrator im Integrationsbaum.
(f) Messmittel im Pruefer-Scratch `F_abn4/` (nicht eingecheckt): `run.sh`, `ausw4.py`, `jobs2.txt`, `jobs3.txt`, Rohdaten je Lauf
(`gy.log`, `st.log`, Framedumps), `lage_*.txt`, `riegel_affen_block.txt`, `seeds12_k22_eigen.txt`, `p3_lache_streifen_abn4.png`.
