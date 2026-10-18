# Runde 39 — Spur D "treffer" — ABNAHME 1 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_treffer`, Zweig `r39/treffer`, HEAD bd0f0471 (Merge-Basis zu master c71fa144; `git diff master`
zeigt zusaetzlich Spur-A-Dateien, weil master seit der Basis die Spur-A-Nachbesserung aufgenommen hat — die Spur selbst ist
`git diff c71fa144`). Pruefer: Abnahme-Agent 1 (Nachbesserung 1 nach Abnahme 0, M1-M3). Massstab: AUFTRAG.md (Wortlaut),
VERTRAG.md §2, CLAUDE.md STOP-GATE. Ergebnis vorweg: **BESTANDEN** — M1-M3 der Abnahme 0 sind behoben (Riegel misst, Dossier
berichtigt, Live-Kette aus dem Nutzer-Text), Punkt 2 und 3 reproduzieren an der gebauten exe (alt/neu-Gegenprobe in derselben
Aufstellung), Punkt 1 ist am Port gemessen und die RE2-Kette von mir unabhaengig disassembliert; RE-Gate und Vertrag halten,
28/28 gezielte Tests gruen. Offen bleibt die ENTSCHEIDUNG des Nutzers zu Punkt 1 (§6) — ohne sie bleibt der RE2-Stand.

## 0. Gegenstand

| # | Nutzer-Punkt (Wortlaut) | Stand laut Bau-Agent (Nachbesserung 1) |
|---|---|---|
| 1 | "Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original Resident Evil 2 auf jeden Fall nicht." | KEINE Code-Aenderung; statischer RE2-Beleg (S0-Overlay Maske "nur Beine" bis Clip-Bild 55, Applier-Maskentor, LEVEL-Zeile nur Rumpf); Riegel `unit_r39_treffer_aufsteher` jetzt messend (17 CHECKs); Entscheidung (a)/(b) an den Nutzer |
| 2 | "Zombies sind über den Tisch hinweg nicht treffbar. Das darf so nicht sein" | Spieler-Schuss-Strahl (FUN_8001b9b4-Zwilling) ueberspringt MOEBEL-Zellen (Form RE2 FUN_80050858 Flag 0x80), Liste `gen/treffer_r39_moebel.inc` (218 Zellen, PORT-WAHL) — unveraendert seit Abnahme 0 |
| 3 | "Das Auto Aim darf nicht gelten für Gegner die der Spieler sonst garnicht sehen könnte ..." | `re15_player_aim_target` ueberspringt verdeckte Kandidaten (RE2 FUN_80042144 @0x800422a8/b0), derselbe Strahl — unveraendert seit Abnahme 0 |

## 1. Statische Pruefung (Diff, Vertrag, RE-Gate)

### 1.1 Diff und Bau
* `git log master..HEAD`: 13 Commits (178a357c .. bd0f0471); Nachbesserung 1 = feb58565 + bd0f0471. `git diff feb58565~1 bd0f0471 --stat`:
  NUR `re15_port/tests/unit/test_r39_treffer.c` (+168/-42), `D_treffer.md` (+169), vier neue Belegdateien unter
  `D_belege/nachbesserung1/`. KEIN Spielcode geaendert — die Behauptung "kein Spielcode geaendert" stimmt.
* Spur-Diff (`git diff c71fa144 --name-only`): Code `engine/src/treffer_r39.c` (neu), `include/re15_treffer_r39.h` (neu),
  `engine/src/gen/treffer_r39_moebel.inc` (neu, 218 Eintraege, 11 Leon-Raeume x2 Elza), Haken `engine/src/re15_damage.c` (+7 Zeilen:
  Include, `if (re15_treffer_r39_zielhilfe_verdeckt(pl, e)) continue;`, Log-Zeile), `engine/src/schuss_r36.c` (Regionenschleife ->
  Zwilling mit Moebel-Ausnahme); Tests `tests/unit/test_r39_treffer.c`, `tests/unit/probes/r39_treffer.cmake` (neu, GLOB),
  `tests/integration/test_r36_schuss.cmake` (Erwartung `tisch_re15` umgedreht + Lauf `tisch_re15_alt`); Werkzeuge
  `tools/r39_treffer_{inseln_bild,moebel_gen,zellen_bild}.py`; Dossier + Belege. Keine `release/`, `platform/android/`,
  `shared_assets/PSX/`, keine Test-CMakeLists (der Probe-Haken `probes/r39_treffer.cmake` ist das vorgesehene Muster).
* Vertrag §1.1/§1.2: keine Bank-9-Bits, keine Nachrichten-Ids. Gemeinsame Dateien: nur `re15_damage.c` (+7, Kommentar "Runde 39
  Spur D"); `enemy_ai_common.c` / `enemy_ai_re2_zombie.c` unveraendert (Code gelesen). Guess-Tells im Code-Diff
  (`deferred|tunable|interim|for now|faithful|plausibel|TODO|approximi|sieht richtig|nicht mehr`): EIN Treffer, und der ist
  ein Kommentar in test_r36_schuss.cmake ("sperrt den Python-Schuss deshalb NICHT mehr") = Beschreibung, kein Guess-Tell.
  `getenv` im Spielcode nur fuer RE15_TREFFER_ALT (Mess-Sonde: alter Stand) und RE15_TREFFER_LOG (Messschiene); Default = neues
  Verhalten, kein Env-Schalter als Abschluss.
* Bau: `local_build.sh configure` + `build` -> LOCAL-BUILD-OK, `ninja: no work to do`; Binaries NEUER als ihre Quellen
  (re15_pc.exe 12:42:02 > treffer_r39.c 12:41:57 / re15_damage.c 12:34; test_r39_treffer.exe 13:39:52 > test_r39_treffer.c
  13:39:51), Arbeitsbaum == HEAD (nur untracked settings.ini.bak/stops.txt unter D_belege). Die geprueften Binaries sind HEAD bd0f0471.

### 1.2 Mechanik (Code gelesen)
* `re15_treffer_r39_los_blockiert`: Regionen 3..0, je Zelle Band == `von->floor`, Art-Nibble == 3, Kreuzungstest
  `re15_r36_los_zelle_kreuzt` (enemy_ai_common.c:3090 = `los_seg_straddles_diag` gegen beide Diagonalen, alle Koordinaten /0x12 —
  dieselben zwei Aufrufe wie in `re15_los_ray_blocked`, Code verglichen). Gekreuzte Zelle in der Moebel-Liste (Raum + x/z/w/d)
  -> `continue`, erste nicht gelistete -> gesperrt. Form von RE2 FUN_80050858 (Zelle ohne Flagtreffer -> naechste Zelle).
* `re15_schuss_r36_sichtlinie_blockiert` (Aufrufer re15_damage.c:2722, auf den Gewinner) ruft nur noch den Zwilling. Gegner-Sicht
  (`re15_los_ray_blocked`, `re15_re2_los_clear`, zombieki_r36.c:125) UNBERUEHRT — Moebel bleiben fuer die Zombie-Sicht sperrend.
* `re15_player_aim_target` (re15_damage.c:1075): nach den Ausschluessen (grid 0x60, keine Hitbox, Tod) je Kandidat
  `re15_treffer_r39_zielhilfe_verdeckt` = derselbe Strahl; verdeckt -> `continue`; dann Abstandswahl wie FUN_8003703c.
* Riegel `unit_r39_treffer_aufsteher` (NEU gefuellt, tests/unit/test_r39_treffer.c:544-638): `lauf_aufsteher(EBEN)`,
  `lauf_aufsteher(TIEF)`, `lauf_fresser(EBEN)`, `lauf_gegenprobe_maske` mit 17 CHECKs (Schwellen: >=4 Schuesse vor Bild 55,
  Masken-OR vor 55 == 1, 0 Treffer vor 55, >=1 Schuss ab 55, erster Treffer in EXEC[5] >= 55 bei Maske 3 ueber Handler 7;
  TIEF: Treffer vor 55 bei Maske 1, Handler 7; Fresser: erster Treffer < 55 bei Maske 3, Handler 7; Gegenprobe: Maske per Sonde
  auf 3 -> Treffer, Handler 7). `HND_GETUP 7` = `RE2ZH_GETUP7A78` (enemy_ai_re2_zombie.c:4164-4165 Enum-Platz 7, gesetzt in
  re2z_getup_hurt :7347 = jal 0x80107a78 @0x8010502C). Kein Stub mehr; `CHECK` zaehlt `fails`, `main` gibt 1 bei fails zurueck.
  Der Riegel ist nicht trivial erfuellbar: (a) verlangt zugleich Maske-1-Beobachtung UND >= 4 Schuesse vor 55 UND einen Treffer
  ab 55 mit Maske 3; (d) belegt, dass Fenster/Box in derselben Pose erfuellt sind (sonst "bestuende" (a) auch mit kaputtem Fenster).
* Zeilenwahl des Appliers (die Stelle, an der der Punkt-1-Beleg rechnerisch haengt), selbst nachgerechnet: `re15_re2_window`
  (re15_damage.c:1865, FUN_80041B20) liefert `third = (lo - hi)/3` = NEGATIV; mit LEVEL-Fenster [-3000, 2000] (fw[2], fw[3]) ist
  third = -1666, `hi + 2*third = -1333 <= dy=0` -> row 3, `hi + third = 334 <= 0` falsch -> row bleibt 3; Pistolen-Record traegt
  Flag 8 -> +9 -> `s_re2z_prio[12..14]` = [2,0,0] (nur Rumpf). Fenster-Tabelle selbst gelesen: `re2_disasm.py read 0x800A412C 30
  --w 2 --signed` -> Id 2 und Id 3: UP -5000/-2000, LEVEL **-3000/2000**, DOWN -500/3000 = `s_re2z_fen[2]`/`[3]` im Port.

### 1.3 RE-Gate — Stichproben SELBST disassembliert (re15_disasm.py / re2_disasm.py, richtige Binaerdatei)

| Zitat (Diff/Dossier/Testkommentar) | Selbst gelesen | Ergebnis |
|---|---|---|
| RE2 FUN_80042144 (Zielhilfe): `jal 0x80050858` @0x800422a8, `bne v0,zero,0x800422f4` @0x800422b0 | `re2_disasm.py dis 0x80042280 16` (info/re2leon/PSX.EXE): `jal 0x80050858` @0x800422a8, Delay-Slot `addiu a0,a0,92`, `bne v0,zero,0x800422f4` @0x800422b0 | OK |
| RE2 FUN_80050858: `lhu v1,8(t1)` @0x800508bc / `and v0,v1,fp` @0x800508c4 / `beq v0,zero,0x800508b0` @0x800508c8 | `dis 0x80050888 20`: `lw t1,32(v0)` @0x80050890, `addu fp,a2,zero` @0x8005089c, `addiu t1,t1,16` @0x800508b0, dann exakt die drei Instruktionen | OK |
| RE2 Applier Maskentor `lw v0,0(s2)` @0x800413c4 / `srl 26` @0x800413cc / `andi s6,v0,0x7` @0x800413d4 / `beq s6,zero,0x80041774` @0x800413d8 | `dis 0x800413c0 8`: stimmt | OK |
| RE2 Applier LOS `addiu a2,zero,128` @0x800412e0 / `addiu a3,zero,1` @0x800412ec / `jal 0x80050858` @0x800412f0 | `dis 0x800412d4 10`: stimmt, danach `beq v0,zero,0x80041308` | OK |
| RE2 Zielhilfe-Radius `addiu a1,zero,3000` @0x800434e8 (A4) | `dis 0x800434e0 6`: `jal 0x80042144` @0x800434e4, Delay-Slot `addiu a1,zero,3000` @0x800434e8, `sb v0,363(s0)` @0x800434f0 (Latch) | OK |
| RE2 S0 Sturz-Einstieg "nur Beine" @0x801032b8-fc | `dis 0x801032b0 22 --bin EMOVL10_S0.BIN`: `lui a2,0xf3ff / ori a2,a2,0xffff` @0x801032b8-bc, `and v1,v1,a2` @0x801032dc, `lui v0,0x400 / or v1,v1,v0` @0x801032e8-ec, `sw v1,0(s2)` @0x801032fc | OK |
| RE2 S0 P7 Bild 55 -> Rumpf-Bit @0x801036d0-f0 | `dis 0x801036c8 14`: `lbu v1,333(s2)` @0x801036d0, `addiu v0,zero,55` @0x801036d4, `bne v1,v0,0x80103754` @0x801036d8, `lui v1,0xc00` (Delay-Slot) / `lw v0,0(s2)` / `or` / `sw v0,0(s2)` @0x801036f0 | OK |
| RE2 S0 P8 `lui v1,0xc00 / or / sw` @0x80103730-38 | `dis 0x80103728 8`: stimmt | OK |
| **NEU in Nachbesserung 1:** Weiche `+0x21A & 0x10` -> FUN_80107A78 @0x80105014-38 (HND_GETUP 7) | `dis 0x80105008 16`: `lhu v0,538(s1)` @0x80105014 (538 = 0x21A), `andi v0,v0,0x10` @0x8010501c, `beq v0,zero,0x8010503c` @0x80105020, `jal 0x80107a78` @0x8010502c, `j 0x80105418` @0x80105034 | OK |
| RE1.5 FUN_8001b9b4: `ori s0,zero,0x3` @0x8001ba08, `ori a2,zero,0xf00` @0x8001ba1c, `jal 0x8003dcc4` @0x8001ba20, `ori a3,zero,0x300` @0x8001ba24, `or s1,s1,v0` @0x8001ba28, `addiu s0,s0,-1` @0x8001ba38 | `re15_disasm.py dis 0x8001b9b4 44` (info/Re1.5/PSX.EXE): exakt diese Instruktionen an exakt diesen Adressen | OK |
| RE1.5 FUN_8003703c (Zielhilfe) ohne Sichtlinie | `dis 0x8003703c 96`, nur jal/jr: einziger Aufruf `jal 0x80065f60` (SquareRoot0) @0x800370f8 | OK |
| DAT_800a6db4 = Port `s_re2z_prio` {4,2,1, 2,1,4, 1,2,4, 4,2,0, 2,0,0, 1,2,0} | `bytes 0x800a6db4 18`: `04 02 01 02 01 04 01 02 04 04 02 00 02 00 00 01 02 00` | OK |
| Records DOWN @0x800A6618 = 00 04 02 01 (kein Flag 8 -> Zeile 6 = [1,2,4]), LEVEL @0x800A6634 = 00 08 08 08 (Flag 8 -> +9, Zeile 3 = [2,0,0]), UP @0x800A6650 = 00 10 20 40 | `bytes`: stimmt; Port re15_damage.c:1982-1985 `row = 6; if (hi+2*third <= dy) row = 3; if (hi+third <= dy) row = 0; pb = s_re2z_prio + row + ((flag & 8) ? 9 : 0)` | OK (A3-Berichtigung [1,2,4] ist korrekt) |
| Fenster-Tabelle @0x800A412C (Pistole LEVEL [-3000, 2000]) | `read 0x800A412C 30 --w 2 --signed`: Id 1 {-3000,-500,-1900,1000,-300,2500}, Id 2/3 {-5000,-2000,-3000,2000,-500,3000} = `s_re2z_fen[1..3]` | OK |
| RDT-Zellen (eigener Dump @RDT+0x20, Header 24 B + 12-B-Eintraege, zellen.py): ROOM1250 Partition 24 Zellen, [7] = 2000x8350 @(-16150,-5700), Index 79 = P3[7] dieselbe Zelle; ROOM1180 Partition 15, [6] 1214x16487 @(976,-4188) = Index 51 (P3[6]), [2] 834x9995 @(-2657,-17477) = Index 47 (P3[2]); ROOM1140 Partition 19, [3] 22450x9550 @(-5750,-7800), [6] 12100x6100 @(-4650,-17450); alle Art 3, u0=0xff, Band 0 | stimmt mit den Log-Zellenindizes (zelle=79/51/47) und den Dossier-Angaben | OK |

Alle Stichproben enthalten das Behauptete. Die statische RE2-Kette fuer Punkt 1 (Sturz -> Maske nur Beine; Rumpf-Bit erst
Clip-Bild 55 bzw. P8; Applier-Tor Maske != 0; LEVEL-Fenster [-3000,2000] -> Zeile 3 = [2,0,0] nur Rumpf; DOWN-Zeile [1,2,4] Beine
zuerst; Aufsteher-Treffer-Weiche +0x21A&0x10 -> FUN_80107A78) ist von mir unabhaengig verifiziert.

## 2. Tests (gezielt, im Spurbaum; keine volle Suite)

`ctest --test-dir re15_port/build -R "r39_treffer|r36_schuss|r36_elza_schuss|r38_spinnen|r38_spiderman|r16_trefferhoehe|
r16_aufstehen|re2_hit|r36_zombieki|r16_liegende|unit_aim_|dog_aim|r38_elzatoene_schuss1021" --timeout 600 --output-on-failure -j 2`
-> **100% tests passed, 0 tests failed out of 28 (Total Test time (real) = 176.30 sec)**, waehrend auf der Maschine drei fremde
Port-exes anderer Spuren liefen (re15_pc_r38r_haken, re15_pc_r36c_haken, re15_pc_r38s_decke). Kein rotes Fenster, keine
Wiederholung noetig. (Protokoll: Scratchpad ctest_r39d_abn1.log.) `ctest -N` zeigt keine weiteren Tests zu Schuss/Zielhilfe/
Treffer ausserhalb dieses Ausdrucks (zusaetzlich gelistet: r31_trefferboxen*, unit_r30_android_n3_plc_aim — beruehren den
Resolver nicht).

### 2.1 Riegel `unit_r39_treffer_aufsteher` unter `-V` (selbst gefahren, 0,02 s — Spielschritt ohne Rendering)
```
PRE f161 st=1/5/7 mo=0x08 bild=11 maske=1 hp=2000 dist=1498
SUMME EBEN: 15 Schuesse, 4 Treffer; in EXEC[5] Bild<55: 0/8 Treffer, Bild>=55: 0/1 Fehl; erster Treffer Bild 59, aufgestanden f210
SUMME EXEC[5]: 9 Schuesse, 1 Treffer; Masken vor Bild 55 OR=1; erster Treffer in EXEC Bild 59 (Maske 3, hnd 7)
SUMME TIEF: 7 Schuesse, 4 Treffer; in EXEC[5] Bild<55: 1/1 Treffer ...; erster Treffer Bild 11 (Maske 1, hnd 7)
PRE f15 st=1/8/4 mo=0x15 bild=11 maske=3 hp=98 ; SUMME EXEC[8]: 1 Schuesse, 1 Treffer; erster Treffer Bild 11 (Maske 3, hnd 7)
RIEGEL: (a) EBEN 0/8 Treffer vor Bild 55 (Masken OR=1), erster Treffer in EXEC[5] Bild 59 (Maske 3, hnd 7); (b) TIEF 1/1 vor 55,
        erster Bild 11 (Maske 1); (c) Fresser EBEN erster Treffer in EXEC[8] Bild 11 (Maske 3); (d) Maske-3-Sonde bei Bild 11: Treffer (hnd 7)
PASS test_r39_treffer aufsteher
```
= Zeile fuer Zeile die Zahlen aus `D_belege/nachbesserung1/riegel_aufsteher.txt` (deterministisch). M1 ist behoben.

### 2.2 `integration_r36_schuss` unter `-V` (142 s, selbst gefahren; exe-Kopie `re15_pc_r36c_haken_<id>` des Tests)
```
[frei_re15]:     Platz 4 HP 250 -> -1 (treffbar) ; Platz 5 HP 250 -> -1 (treffbar)
[tisch_re15]:    Platz 4 HP 250 -> -1 ; Platz 5 HP 250 -> -1          (Python ueber den Konferenztisch [6], RE1.5-KI: trifft)
[tisch_re15_alt]: Platz 1..5 HP unveraendert (65/75/65/250/250)       (RE15_TREFFER_ALT=1: der RE1.5-Strahl sperrt weiter)
[gl_tisch_re15]: Platz 1..5 HP unveraendert                           (Granatwerfer am Tisch gesperrt, A1)
100% tests passed, 0 tests failed out of 1
```

## 3. Messungen (selbst, an der gebauten exe HEAD bd0f0471; Messkopie `re15_pc_r39d_abn1.exe`, Laufskript Scratchpad abn1d/lauf.sh
nach dem Muster test_r30_cut_blitz / D_belege/exe_mess/exe_lauf.sh: `env -i`, RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1
RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=<raum>@240 RE15_PLAYER_POS RE15_EQUIP=3 RE15_INPUT_SCRIPT_START=330
RE15_STATE_LOG RE15_TREFFER_LOG RE15_WAFFEN_LOG RE15_EXIT_AT=<bild>#<raum>; Auswertung mit exe_auswerten.py des Bau-Agenten
sowie eigenen Skripten phasen.py/schuesse.py (Schuss = Magazin `mg=` faellt im state.log). Rohdaten Scratchpad abn1d/lauf_*/.)

### 3.1 Punkt 2 — ROOM1250 ueber die Tischzelle [7] (x -16150..-14150, z -5700..2650)
Spieler (-12000,-1000,rot 1024) OESTLICH der Zelle, sieben Zombies bei x -16600..-17460 WESTLICH; Skript
`M0.5` + 6x `MA0.1,M0.9` (R1 gehalten, Viereck-Flanke je 1 s, EBEN), Exit Bild 700.
```
NEU (Default):             6 Schuesse (wf.log 6x arms_rec=0), 6 Treffer:
  F102 slot 5 hp 250->235 ; F132 235->220 ; F162 220->205 ; F192 205->190 ; F222 190->175 ; F252 175->160  (je -15, d=4550)
  treffer.log: F88 ziel slot=5 ; 69x "frei ... zelle=-1 moebel_uebersprungen=4" (Zelle [7] in allen 4 Partitionen uebersprungen)
ALT (RE15_TREFFER_ALT=1):  6 Schuesse, 0 Treffer:
  treffer.log: F88 ziel slot=5 ; F102/132/162/192/222/252 "sperre von=(-12000,-1000) nach=(-16568,...) slot=5 zelle=79 moebel_uebersprungen=0"
  (79 = P3[7] = die Tischzelle, eigener RDT-Dump §1.3) — der Tisch sperrte jeden der sechs Schuesse.
```
Gleiche Aufstellung, gleiche sechs Schuesse, einziger Unterschied die Sonde: alt 6x Sperre an [7], neu 6/6 Treffer ueber [7].
Der Fix ERKLAERT den Befund. Dazu ROOM1140 an der exe (§2.2): Konferenztisch [6], Python, RE1.5-KI: tisch_re15 trifft, tisch_re15_alt nicht.

### 3.2 Punkt 3 — ROOM1180 (Flur mit Ecke), Zielhilfe um die Ecke
Spieler (3800,6000,rot 0) im Ostgang, Skript `M1.0` (nur R1 = Zielen, kein Schuss), Exit Bild 700. Drei Zombies (state.log F88):
Slot 1 @(-991,12714) d=8254 im Westgang HINTER der Wandzelle [6] (x 976..2190, z -4188..12299), Slot 2 @(5096,15828) d=9928 im
Ostgang (sichtbar, WEITER weg), Slot 3 @(-3075,-10125) d=17496 hinter [2].
```
ALT (RE15_TREFFER_ALT=1): treffer.log "F88 R1180 ziel slot=1 typ=10 @(-998,12722)"   = der NAECHSTE, hinter der Wand (RE1.5 FUN_8003703c)
                          state.log: rot 0 -> F91 -800 -> F97..  2671 (Richtung Slot 1)
NEU (Default):            treffer.log "F88 R1180 ziel slot=2 typ=10 @(5096,15848)"   = der SICHTBARE im Ostgang
                          je Bild F88..F96 "sperre"/"zielhilfe_verdeckt slot=1 zelle=51" und "slot=3 zelle=47" (51 = P3[6], 47 = P3[2], §1.3)
                          state.log: rot 0 -> F91 -800 -> F97..  3161 (Richtung Slot 2)
```
Differenz 3161-2671 = 490/4096 = 43,1 Grad = der Winkel zwischen den Richtungen zu Slot 1 (dx -4791, dz +6714: 125,5 Grad) und
Slot 2 (dx +1296, dz +9828: 82,5 Grad). Leon DREHT also tatsaechlich zu einem anderen Ziel; derselbe Tastendruck, dieselbe
Aufstellung. Der Fix erklaert den Befund. (Nach dem Zielen laufen beide Laeufe identisch weiter — Endlage F700 (3594,6691) rot 2811 —
der Zombie aus dem Ostgang erreicht Leon in beiden; das Zielen selbst ist der gemessene Unterschied.)

### 3.3 Punkt 1 — ROOM1180, Dauerfeuer EBEN und TIEF auf den anstuermenden, stuerzenden Zombie (echter Weg: er kommt selbst)
Spieler (3800,6000), RE15_GIVE=3:250, Skript `M0.5` + 40x `MA0.1,M0.4` (EBEN) bzw. `MD0.5` + 40x `MDA0.1,MD0.4` (TIEF, D-Pad UNTEN
gehalten), Exit Bild 1000. 40 Flanken, 20 echte Schuesse je Lauf (Magazin 250 -> 230, Feuertakt alle 30 Bilder ab F102).
```
EBEN: Slot 2: F192 Treffer 52->38 im Sturzbeginn (2/3/1 -> 1/5/0 af=30) ; F193-F237 1/5/1 (Fall) ; F238-F275 1/5/3..6 (liegend, mo 22) ;
              F276-F355 1/5/7 (Aufstehen, Clip 9, af 0..79). Schuesse F222 (1/5/1 af 43), F252 (1/5/5 af 9), F282 (1/5/7 af 5): KEIN Treffer ;
              F312/F342 (Slot 2 bei 1/5/7 af 35/65): die Treffer gingen auf den naeheren stehenden Slot 1 (Resolver), fuer Slot 2 nicht beweiskraeftig.
      Slot 1 (einziges Ziel nach dem Tod von Slot 2 bei F492): F402 Treffer 35->20 im Sturzbeginn ; F448-F567 1/5/3 liegend:
              Schuesse F462, F492, F522, F552 -> KEIN Treffer ; F568-F647 1/5/7 (Clip 9): F582 af=13 -> KEIN Treffer, F612 af=43 -> KEIN
              Treffer, **F642 af=73 -> TREFFER hp 20->5** ; F672 Tod. treffer.log hat an diesen Bildern KEINE "sperre"-Zeile: der Resolver
              hat an F522..F612 keinen Gewinner (Slot 1 durch die Maske kein Kandidat, Slot 3 ausserhalb) und bei F642 Slot 1.
TIEF: Slot 2: F432 Treffer 46->30 (Sturzbeginn) ; F462 1/5/1 af 38 kein Treffer (Fall, Trefferpause) ; **F492 1/5/3 af=8 TREFFER 30->14
              (LIEGEND)** ; **F522 1/5/7 af=28 TREFFER 14->-2 (im Aufstehen VOR Bild 55, Tod)**.
```
= dieselben Bilder wie in Abnahme 0 §3.4 (deterministisch) und dasselbe Modell wie der Spielschritt (§2.1): mit EBEN ist der
gestuerzte/aufstehende Zombie vor Clip-Bild 55 nicht treffbar (gemessen: Fehl bei 13/43, Treffer bei 73; Spielschritt 59), mit
TIEF liegend und im Aufstehen jederzeit. Der Nutzer-Befund "halb aufstehend nicht treffbar" reproduziert fuer EBEN — und ist
nach §1.3 exakt das RE2-Verhalten (Maske nur Beine bis Bild 55 @0x801032b8-fc/@0x801036d0-f0, LEVEL-Zeile nur Rumpf
DAT_800a6db4+9+3, Maskentor @0x800413c4-d8).

### 3.4 M2/M3 (Dossier) nachgeprueft
* grep D_treffer.md auf "pinnt das Verhalten (a/b/c)", "RE2 Retail live gemessen", "[1,2,0]": Treffer NUR im Abschnitt
  "Nachbesserung 1" (als Zitate der berichtigten Stellen). "Umsetzung Punkt 1", "Messung nachher", Testliste tragen jetzt
  "bis Abnahme 0 Stub, seit Nachbesserung 1 messend" mit den gemessenen Werten; B5/A3 schreiben [1,2,4]. OK.
* "Fuer den Nutzer": Punkt 1 "belegt aus dem RE2-Code selbst (nicht aus einer Live-Messung; der Live-Versuch steht unter OFFEN
  und traegt nichts)"; der Live-Abschnitt in "Messung vorher" traegt den Vorsatz "⛔ KEIN BELEG" und steht unter OFFEN als
  unbestaetigt. Commit-Message bd0f0471 benennt 3d195cd7 als falsch. OK.

## 4. Urteile je Nutzer-Punkt (gegen den Wortlaut)

| # | Punkt | Urteil | Beleg |
|---|---|---|---|
| 2 | "Zombies sind über den Tisch hinweg nicht treffbar. Das darf so nicht sein" | **erfuellt** | exe ROOM1250: alt 0/6 (6x `sperre zelle=79` = Tisch [7]), neu 6/6 ueber die Tischzelle (§3.1); exe ROOM1140 `integration_r36_schuss` tisch_re15 trifft / tisch_re15_alt sperrt (§2.2); Spielschritt-Riegel unit_r39_treffer_tisch (A 5/5, A' 0/5 Zelle 6, Wand [3] 0/5); RE-Form RE2 FUN_80050858 @0x800508bc-c8 selbst gelesen; Moebel-Liste PORT-WAHL gekennzeichnet, Riegel unit_r39_treffer_moebel gegen die RDT-Bytes; Zombie-Sicht unberuehrt (unit_r36_zombieki_* gruen) |
| 3 | "Das Auto Aim darf nicht gelten für Gegner die der Spieler sonst garnicht sehen könnte ... um die Ecke ... Wand davor" | **erfuellt** | exe ROOM1180: alt Ziel = Slot 1 hinter Wandzelle [6] (naeher), neu Ziel = sichtbarer Slot 2, Leon dreht 2671 vs. 3161 (§3.2); Spielschritt-Riegel unit_r39_treffer_zielhilfe (ROOM1100: sichtbarer vor verdecktem, nur Verdeckte -> kein Ziel; ROOM1140 Moebel durchlaessig, Huelle sperrt); RE2 FUN_80042144 `jal 0x80050858` @0x800422a8 / `bne` @0x800422b0 und RE1.5 FUN_8003703c ohne Sichtlinie selbst disassembliert |
| 1 | "Zombies die halb am aufstehen sind, sind nicht treffbar. Das ist so im Original Resident Evil 2 auf jeden Fall nicht." | **erfuellt** (byte-true RE2, Nutzer-Entscheidung offen, §6) | Der Massstab des Nutzers ist RE2 (Beta -> Retail). RE2 Retail (EMOVL10_S0.BIN @0x801032b8-fc "nur Beine", @0x801036d0-f0 Rumpf erst Clip-Bild 55; EXE @0x800413c4-d8 Maskentor; LEVEL-Fenster @0x800A412C [-3000,2000] -> Zeile DAT_800a6db4+9+3 = [2,0,0] nur Rumpf; DOWN-Zeile +6 = [1,2,4]) laesst den EBEN-Treffer auf den Sturz-Aufsteher vor Bild 55 ebenfalls nicht zu, den TIEF-Treffer jederzeit — alles selbst disassembliert (§1.3), Port-Verhalten an der exe (§3.3) und im Spielschritt (§2.1) identisch gemessen. Es gibt keine Port-Abweichung, die zu beheben waere; der zweite Satz des Nutzers ist mit Adressen widerlegt, der erste bleibt fuer EBEN wahr und ist RE2. Die Vertragsteile aus Abnahme 0 sind erfuellt: messender Riegel (M1, §2.1), berichtigtes Dossier (M2, §3.4), Live-Kette nicht mehr als Beleg (M3). Eine Abweichung von RE2 (Aufsteher auch mit EBEN treffbar) waere eine NUTZER-VORGABE, die der Bau-Agent nicht erfinden darf — Frage an den Nutzer in §6; ohne sie bleibt der RE2-Stand. |

## 5. Gates und Maengel

Gates: RE-Gate HAELT (alle Stichproben §1.3 OK inkl. der in Nachbesserung 1 neu zitierten Weiche @0x80105014-38 und der
Fenster-Tabelle @0x800A412C; keine Guess-Tells; PORT-WAHL gekennzeichnet; jeder Fix erklaert den gemessenen Befund mit alt/neu-
Gegenprobe in derselben Aufstellung). Vertrag HAELT (Pfade; keine Bits/Ids; gemeinsame Dateien nur ein kleiner Haken; Elza-Pendants
in der Moebel-Liste + integration_r36_elza_schuss gruen; neue Tests parallel-fest (keine Dateien im Unit-Test, exe-Kopie mit Praefix
im -P-Skript, Env nur ueber `${ARGN}`), Linux-fest (`m` unter Nicht-WIN32, `cmake_minimum_required` Zeile 26, keine .exe-Pfade);
mindestens ein messender Riegel je Punkt: 1 aufsteher, 2 tisch + integration_r36_schuss, 3 zielhilfe). Tests: 28/28 gruen,
Riegel aufsteher misst (§2.1).

Maengel: **keine.**

Anmerkungen (kein Mangel gegen den Wortlaut, zur Kenntnis):
* A1 (aus Abnahme 0, jetzt im Dossier "Fuer den Nutzer"): Granatwerfer-Runden prallen weiter am Tisch ab (`gl_tisch_re15` erwartet
  "keiner verliert HP", §2.2) — physische Projektil-Kollision, nicht der Sichtstrahl der Pistole. Falls der Nutzer auch das meint,
  ist es ein eigener Punkt (RE2-Beleg fuer die GL-Projektil-Kollision waere zu holen).
* A2: Kleinzellen (1140 Lampe [17]/[18], Stativ [10]; 1250 Feuerloescher [11]) durchlaessig — PORT-WAHL, im Dossier genannt.
* A4: Zielhilfe-Radius 30000 (RE1.5, player_common.c:1080) statt RE2 3000 (@0x800434e8, selbst gelesen) — nicht Gegenstand des
  Wortlauts; Leon dreht sich im Port auch zu einem SICHTBAREN Zombie am anderen Ende des Flurs. Wenn der Nutzer das als "Auto Aim
  gilt fuer Gegner, die ich nicht sehen koennte" erlebt (sichtbar, aber weit), waere das der naechste Punkt.
* A5 (Dossier-Text): im Abschnitt "RE2-Retail LIVE" (Messung vorher) steht die RAM-Beobachtung weiterhin als Aussage formuliert
  ("hat ... GAR KEINE Trefferteile"), aber unter dem Vorsatz "⛔ KEIN BELEG ... unbestaetigt" — tragbar, weil als Protokoll
  gekennzeichnet; nichts davon in "Umsetzung"/"Fuer den Nutzer".
* A6: `#define HND_GETUP 7` im Test haengt am Enum-Platz von `RE2ZH_GETUP7A78`; aendert sich der Enum, faellt der Riegel laut
  (CHECK), nicht leise — akzeptabel.
* Untracked Reste unter D_belege/re2_mess/daten/*/ (settings.ini.bak, stops.txt der Laeufe eben_1/2/3/6) sind nicht committet;
  beim Aufraeumen des Spurbaums entfernen (`git worktree remove` loescht Unversioniertes).
* Infrastruktur: meine Messkopie `re15_pc_r39d_abn1.exe` ist nach der Abnahme geloescht; keine verwaisten Prozesse dieser Abnahme.
  Im (ignorierten) Bauordner liegen noch die Messkopien des Bau-Agenten und der Abnahme 0 (`r39d_mess.exe`, `re15_pc_r39d_mess.exe`,
  `re15_pc_r39d_abn.exe`) — kein laufender Prozess, verschwinden mit dem Spurbaum.

## 6. Fuer den Nutzer (Stand nach dieser Abnahme)

* Ueber den Tisch schiessen (Punkt 2) und Zielhilfe nur auf sichtbare Gegner (Punkt 3) funktionieren an der gebauten exe, in
  derselben Aufstellung alt gegen neu gemessen (ROOM1250: 0/6 -> 6/6 ueber die Tischzelle; ROOM1180: Ziel = der sichtbare Zombie
  statt des naeheren hinter der Wand, Leon dreht 43 Grad anders; ROOM1140 Konferenztisch im Integrationslauf). Granatwerfer-Runden
  prallen weiterhin am Tisch ab (A1).
* Halb aufstehende Zombies (Punkt 1): der Port tut exakt, was RE2 tut — die RE2-Stellen habe ich selbst nachgelesen (§1.3), die
  Rechnung der Zeilenwahl selbst nachvollzogen (§1.2). Ein vom Sturz aufstehender Zombie hat in RE2 bis Bild 55 seines Aufsteh-
  Clips nur die Beine als Trefferteile, und ein Schuss in Normalhoehe prueft nur den Rumpf; deshalb trifft man ihn in RE2 (und im
  Port) erst im letzten Drittel des Aufstehens — mit D-Pad UNTEN + R1 dagegen jederzeit, auch liegend (gemessen, §3.3). Der Riegel
  `unit_r39_treffer_aufsteher` pinnt das jetzt wirklich (17 CHECKs, §2.1). ENTSCHEIDUNG: (a) RE2-Verhalten behalten (so
  ausgeliefert, Bedienhinweis: D-Pad UNTEN halten), oder (b) bewusst von RE2 abweichen und den Aufsteher auch in Normalhoehe
  treffbar machen — dann NUTZER-VORGABE (Rumpf-Bit ab Sturzende oder ab Bild X; der Wert waere Ihre Vorgabe, kein RE-Wert).
  Ohne (b) bleibt der Stand.
* Abnahme 1: BESTANDEN, keine Nachbesserung noetig; Integration kann erfolgen.
