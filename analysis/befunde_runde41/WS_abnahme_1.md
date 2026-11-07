# Spur WS — Unabhaengige Abnahme 1 (Waffenschaden: Rakete + Handgranate), Runde 41, 2026-11-07

Pruefer: unabhaengiger Abnahme-Agent (nicht der Bauer). Dem Bauer wurde nichts geglaubt: jede Zahl unten ist selbst gemessen
oder selbst disassembliert.

## Pruefgegenstand

* Zweig `r41/waffenschaden`, Worktree `C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen`, Basis `r41/integration` 9e0066c26,
  Kopf ac72cd3cb. Sieben Commits: 57d416401 (Blaetter), 82fcd8212 (D_G + Rakete), e0cfc8c5d (H3 25240), 160000d97 (test_r41_granate),
  cfe403cbc (Unit-Riegel), 87391a514 (Integrationsriegel), ac72cd3cb (Dossier `WS_waffenschaden.md`).
* Spezifikation: `analysis/befunde_runde41/WS_ENTSCHEIDUNG.md` (+ `WS_LESER_A.md`, `WS_LESER_B.md`), Orchestrator-Zusatz H3
  (Gator-Boss byte-true 25240), Nutzerentscheid 2026-11-07: "mach wie resident evil 2. Auch bei der Granate. mach sie so stark,
  dass sie gerade so die Standard Gegner mit einer Granate toeten kann."
* Eigene Werkzeuge und Laufausgaben: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/abn1/`
  (`lauf.sh` = ein exe-Lauf mit der Messkopie `re15_pc_wsabn.exe`, immer nur einer zur Zeit; `hpfolge.py` / `zensus2.py` =
  HP-/Zustandswechsel je Slot aus `RE15_STATE_LOG`; `montage.py` = Bildtafeln aus der RE15_FRAMEDUMP-Serie, beschriftet aus
  state.log; `scan_cfb74.py` / `scan_calls.py` = Schreiber-Scan 0x800cfb74; `gegenprobe/` = alte Engine selbst uebersetzt).
  Disassembler: `C:/workspace/git/reAi_v2/.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` und `re15_disasm.py`.
* Bildnummern `F...` = Spielbilder nach dem Debug-Sprung. Massgeblich sind die Logwerte; die Bildtafeln zeigen den Lauf
  (beschleunigter Renderer, RE15_FRAMEDUMP = komponiertes Bild vor dem Present). Parallel liefen andere Agenten (Last).

## URTEIL: BESTANDEN (Maengel M1 und M2 gering, keiner blockierend)

Der Nutzerentscheid ist umgesetzt und im echten Spiel gemessen, mit Leon und Elza:
* **Handgranate D_G = 251** fuer jeden Gegner, ohne Sonderfall. Die Herleitung 250 (`addiu v1,zero,250` @0x801008C8, Typ 0x11)
  plus 1 (`bgez` @0x80013014 / @0x80047284) habe ich selbst disassembliert. Eine Granate toetet jeden gemessenen Standard-Gegner:
  0x10, 0x16, Hund und Spinne deutlich, den dicken 0x11 "gerade so" mit genau -1 (echter Wurf, 250 -> -1). Die Boss-Regel der Spur
  GR ist weg (Gorilla 2 Granaten, Birkin-600 3, Riesenform 4).
* **Rakete = RE2 Zeile 17 K0 je Typ.** Alle 14 Tabellenwoerter stimmen mit meinem eigenen Dekoder ueberein (G4 301 @0x800A5D24,
  G5 200 @0x800A601C, Alligator 110 @0x800A4B54, ...). Gemessen: Riesenform 699/398/97/-204 (4), G5 400/200/0/-200 (4),
  Gator 25130/25020/24910 (3).
* **H3 (Gator-Boss 25240)** haelt: `andi 0x40` @0x80100524, [0x801045ec]/[0x801045ee] = 240, Setter `ori v0,v0,0x40` @0x8002b4ac
  in der RE2-EXE (die Berichtigung des Bauers stimmt, die RE1.5-EXE hat dort `addiu s3,sp,40`). Mein vollstaendigerer
  Schreiber-Scan (72 + 84 Stores) findet keinen, der 0x40 loescht. Im Spiel: 25240 -> 24989 nach einer Granate, Ausloeser sofort.
* Bau sauber (`ninja: no work to do.`). Unit 1066/1066 gruen, Integration 42/42 gruen (1 Lastausreisser, einzeln 3x gruen).
  Die Gegenprobe mit selbst uebersetzter alter Engine und die umgekehrte Gegenprobe (alte Riegel gegen neue Engine) zeigen: die
  Riegel unterscheiden alt und neu, und die Nachzuege waren genau an den Stellen der Liste (e) noetig.

## 1. Bau (Pruefauftrag 1)

| Punkt | Soll | Ist | Beleg | Bewertung |
|---|---|---|---|---|
| Baum aktuell | `local_build.sh build` ohne Arbeit | `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (1,5 s) | eigener Aufruf | OK |
| exe gegen HEAD | exe juenger als jede Quelle des Kopfes | `re15_pc.exe` 12:45:37 (8 770 935 Byte); letzte Quellaenderungen `birkin_formen.c` / `re15_birkin_formen.h` 12:39:37, `re15_damage.c/.h` und `re15_gator_re2_r41.h` 12:33:28. Die Commits nach 12:45 (87391a514, ac72cd3cb) aendern nur `tests/integration/*.cmake` und Doku | `ls --time-style=full-iso`, `git show --stat` | OK |
| Testkonfiguration | Tests eingeschaltet | `RE15_BUILD_TESTS:BOOL=ON` (CMakeCache) | | OK |
| Aenderung im Objekt | D_G und Raketenweg wirklich uebersetzt | `objdump` von `re15_damage.c.obj`: `re15_granate_schaden` = `mov $0xfb,%eax` (251); `nm re15_pc.exe`: `re15_rakete_schaden` vorhanden | eigener Aufruf | OK |

## 2. Code gegen das Entscheidungsblatt (Pruefauftrag 2) — selbst disassembliert

### 2.1 Handgranate D_G = 251

| Punkt | Soll | Ist im Code | eigene Disassembly | Bewertung |
|---|---|---|---|---|
| 250 = Maximum Standard (Typ 0x11) | 250 @0x801008C8 / @0x801008CC | `#define RE15_GRANATE_DG (250 + 1)` (`include/re15_damage.h`), beide Adressen im Kommentar | EMOVL10_S0.BIN: `801008a0 addiu v0,zero,17`, `801008bc bne v1,v0,0x801008d8` (v1 = `lbu v1,8(s2)` = Typ), **`801008c8 addiu v1,zero,250`**, **`801008cc sh v1,342(s2)`** | OK |
| +1 = Todesregel hp < 0 | `bgez` @0x80013014 / @0x80047284 | im Kommentar | RE1.5 `80013004 lh v1,154(s1)`, **`80013014 bgez v1,0x80013024`**, `8001301c ori v0,zero,0x3`; RE2 `8004727c lh v1,342(s1)`, **`80047284 bgez v1,0x80047294`**, `8004728c addiu v0,zero,3` | OK |
| ein Wert fuer jeden Gegner | ohne Sonderfall | `re15_granate_schaden`: `(void)e; return RE15_GRANATE_DG;` | — | OK |
| Boss-Regel weg | GR_REST_* / GR_HP_* entfallen | `git grep "GR_REST_\|GR_HP_"` = 0 Treffer; Includes `re15_gegner_r39.h`, `re15_boss_gator.h`, `re15_gator_re2_r41.h` aus `re15_damage.c` entfernt | — | OK |
| G5-Ausnahme 80 weg | Art 2 am G5 = D_G, Art 3/4 = 70 | E16-Block nur noch `type >= 3u && type <= 4u` -> `dmg = 70` (Z11 @0x800A5FA4 / Z10 @0x800A5F90) | — | OK |
| kein Abfall | voller Wert im Kasten an P und P+900 | Explosion: `re15_re2_gl_kandidat` + `re15_resolver_gegnerzweig(..., NULL)` (granate_r35.c:389), unveraendert | `read 0x80010918` = `[-2000, 0, 1000, 500]`; Op 47 `80020d34 addiu v0,zero,12` / `80020d38 bne v1,v0,0x80020dc0` | OK |

### 2.2 Rakete je Typ = RE2 Zeile 17, Klammer 0

Eigener Dekoder (Python auf `info/re2leon/PSX.EXE`, t_addr 0x80010000): Zeiger `*(0x800A6A88 + Typ*4)`, Satz + 16*20, K0 = w0 & 0x3FF,
Sperre (w1 >> 9) & 0x7F = 15 in jeder Zeile.

| RE2-Typ (Port-Verwendung) | Zeiger @ | Z17 @ | w0 | K0/K1/K2 | im Code |
|---|---|---|---|---|---|
| 0x10/0x11/0x12/0x13/0x14/0x18/0x19, Vorgabe (0x37) | 0x800a6ac8.. | **0x800a426c** | 384e1384 | 900/900/900 | `s_rak_zombie` 0x384E1384 |
| 0x15/0x16/0x17 | 0x800a6adc.. | **0x800a43e8** | 384e1384 | 900 | `s_rak_zombie16` |
| 0x20 Hund | 0x800a6b08 | **0x800a4564** | 12c4b12c | 300 | `s_rak_hund` |
| 0x21 Kraehe | 0x800a6b0c | **0x800a46e0** | 12c4b12c | 300 | `s_rak_kraehe` |
| 0x22 Licker (Port: Gorilla 0x27) | 0x800a6b10 | **0x800a485c** | 0c8320c8 | 200 | `s_rak_licker` |
| 0x23 Alligator (RE1.5-KI und Gator-Boss) | 0x800a6b14 | **0x800a4b54** | 06e1b86e | **110** | `s_rak_gator` |
| 0x25/0x26 Spinne/Baby | 0x800a6b1c/20 | **0x800a4cd0** | 0c8320c8 | 200 | `s_rak_spinne` |
| (0x27 G-Embryo, nicht verwendet) | 0x800a6b24 | 0x800a4e4c | 0c9324c9 | 201 | — |
| 0x29 Kakerlake (Port: kleiner Affe) | 0x800a6b2c | **0x800a5144** | 12c4b12c | 300 | `s_rak_kakerl` |
| 0x2B Tyrant (= 0x2A @0x800a543c) | 0x800a6b34 | **0x800a55b8** | 1f47d1f4 | 500 | `s_rak_tyrant` |
| 0x2D Zombie-Arme (Port: Gitterhaende 0x1A) | 0x800a6b3c | **0x800a52c0** | 0280a028 | 40 | `s_rak_arm` |
| 0x2E Ivy (Port-Ivy 0x2D) | 0x800a6b40 | **0x800a5734** | 12c4b12c | 300 | `s_rak_ivy` |
| 0x30 G1 (Birkin ausser Riesenform) | 0x800a6b48 | **0x800a58b0** | 0c8320c8 | 200 | `s_rak_g1` |
| 0x34 G4 (Riesenform) | 0x800a6b58 | **0x800a5d24** | 12d4b52d | **301** | `s_rak_g4` |
| 0x36 G5 | 0x800a6b60 | **0x800a601c** | 0c8320c8 | **200** | `s_rak_g5` |

* Applier selbst gelesen: `80047114 srl s6,s5,28` (K), `80047214 andi v1,s5,0xffff` (Zeile), `8004722c lw a1,27272(at)`
  (0x800a6a88), `8004723c addiu v0,v0,-20`, `80047254 srlv v1,v1,v0`, `8004725c andi v1,v1,0x3ff`, `80047268 sh v0,342(s1)`.
* Hitcode Gegnerkontakt: `8001f7b0 lui a3,0x3`, `8001f7bc jal 0x800470c0`, `8001f7c0 ori a3,a3,0x11` = 0x00030011.
  Boden: `8001f820 lui a3,0x2`, `8001f82c ori a3,a3,0x11`, `8001f83c jal 0x800470c0`.
* Alle 14 w0-Woerter im Code stimmen mit dem Dekoder ueberein. RE2-KI-Kandidaten nehmen denselben Wert ueber die bestehenden Saetze
  `s_re2gl_rec16_*` (re15_damage.c, Saetze :4250-4256, Wahl :4257-4266: Zombie 900, Zombie16 900, Hund 300, Kraehe 300, Spinne 200, Arm 40, G5 200),
  RE1.5-KI-Kandidaten ueber Art 9: `if (type == 9u) dmg = (int16_t)re15_rakete_schaden(e);`.
* Riesenform-Praedikat: `re15_bf_granate_form(e,&hp0,&ende)` mit `hp0 == RE15_BB_FORM_HP` -> G4, sonst G1 (wie im Bauplan #5).
* **Bewertung: OK.** Die Typ-Zuordnungen ohne RE2-Gegenstueck (Gorilla -> Licker 200, kleiner Affe -> 300, Gitterhaende -> Arme 40,
  Ivy 0x2D -> RE2 0x2E) folgen dem Entscheidungsblatt (b) und sind im Code begruendet.

### 2.3 H3 — Gator-Boss 25240 (Orchestrator-Zusatz)

| Punkt | eigene Disassembly / Messung | Ergebnis |
|---|---|---|
| Init EM23_OVL_0000.BIN (gelinkt @0x80100000: alle Sprungziele im Fenster 0x8010xxxx) | `8010051c lw v1,-1164(v1)` (0x800cfb74), **`80100524 andi v0,v1,0x40`**, `80100528 beq v0,zero,0x80100558`, `8010052c andi v0,v1,0x20`, **`8010053c lhu v0,17900(v0)`** (0x801045ec) bzw. **`8010054c lhu v0,17902(v0)`** (0x801045ee), `addiu v0,v0,25000`, `80100580 sh v0,342(s0)`. Ohne Bit 0x40: **`80100564`** [0x801045bc] / **`80100574`** [0x801045be] | mit Bit 0x40 = 25000 + 240 |
| Daten | `read 0x801045b0 u16` = `[48, 10173, 8, 992, 0, 0, 300, 300, ..., 240, 240]`: [0x801045bc] = [0x801045be] = 300, [0x801045ec] = [0x801045ee] = 240 | 25240 statt 25300 |
| Setter | RE2 PSX.EXE `8002b4a4 lw v0,-1164(v0)`, **`8002b4ac ori v0,v0,0x40`**, `8002b4b4 sw v0,-1164(at)` in FUN_8002b48c; einziger Aufrufer `8002af00 jal 0x8002b48c` am Kopf von `main` (ghidra_re2_Leon.txt: XREF `main:8002af00`) | Bit 0x40 ab dem Start gesetzt |
| Berichtigung des Bauers | RE1.5 PSX.EXE @0x8002b4ac = `addiu s3,sp,40` (selbst gelesen) | stimmt: der Setter steht in der RE2-EXE; fuer das RE2-Gator-Modul ist das der massgebliche Pfad |
| Loescht jemand das Bit? | Eigener Scan mit Konstantenverfolgung je Register ueber PSX.EXE und alle 25 `COMMON/BIN/*.BIN` (darunter STAGE1-7, TITLE, MEM_CARD, ENDING, RESULT, die EM-Overlays): **72** Kandidaten-Stores auf 0x800cfb74..77 (der Bauer nennt 34; sein Muster `lui 0x800d` + `-1164` uebersieht `addiu rX,rX,-1164` + `sw 0(rX)` und `sw 14732(s0)` ueber die G-Basis 0x800cc1e8). Jeder Store ist ein ODER oder ein UND mit einer Maske, die 0x40 behaelt (0x7fffffff, 0x010008d9, 0xfeffffff, 0xfffbffff, 0xffdfffff, 0xfdffffff, 0xfbffffff, 0xefffffff, 0xf7ffffff, 0xfffdffff, 0xfffeffff, -2, -3, -9, -33, -513, -1025, -16385, 0xffff7fff). Die Block-Loeschung `FUN_80076a28` (Wortschleife `sw zero,0(a0)`) laeuft nur ab 0x800cfbd8 aufwaerts; kein Aufruf mit konstantem Zielzeiger ueberdeckt 0x800cfb74. MEM_CARD stellt 0x40 beim Laden nur zusaetzlich her (ODER). Zusaetzlich die 112 RE2-Gegnermodule `build/extracted/re2_ems/CDEMD0|1_EM*_ai0|ai1.BIN`: 84 Stores (42 je Archiv), alle ODER, nur EM20 @0x80104f60 UND mit 0xfbffffff | keiner loescht 0x40 |
| Port konsistent | Zombie-HP-Tabelle des Ports = 0x40-Zweig (`801006d4 andi v0,v0,0x40` -> Tabelle 0x8010C670, re15_damage.c:932); G4 (EM34 ai0/ai1) `80100670 andi v0,v0,0x40` -> `80100678 addiu v0,zero,1000` (ohne 0x40: 1100) | ueberall derselbe Zweig |

* Code: `#define RE15_GR2_HP_INIT (RE15_GR2_HP_SOCKEL + 240)` mit allen Adressen im Kommentar (`include/re15_gator_re2_r41.h`),
  die Commit-Nachricht e0cfc8c5d nennt sie.
* Gator-Riegel per `git grep 25300 / 25000 / RE15_GR2_HP_INIT` geprueft: nachgezogen sind `test_r35_granate.cmake` (gator 251),
  `test_r41_re2gator.cmake` / `test_r41_re2gator.c` / `r41_re2gator.cmake` und `test_r41_granate.c` (PRUEF 82/122/132).
  Die Kanister-Riegel setzen die HP per Messhaken `RE15_K2080_GATOR_HP=24990` und haengen nicht am Startwert. Die alte
  Flinch-Kadenz `GB_FLINCH_STEP` (enemy_ai_boss_gator.c:857-872) steht in `if (0)`, haengt also nicht an GB_HP.
* **Bewertung: Lesung haelt, Aenderung richtig. OK.** (Doku-Ungenauigkeiten: Mangel M1/M2.)

### 2.4 Keine Nebenwege mit 1000

* `re15_damage_table[...]` wird nur an zwei Stellen gelesen: Spielerzweig re15_damage.c:229 (Art 2 am Spieler bleibt 1000, H12)
  und `re15_enemy_take_damage_at` :3198, dort danach `if (type == 2u)` -> D_G und `if (type == 9u)` -> `re15_rakete_schaden`.
  `git grep 1000` in engine/include: kein weiterer Schadensweg (nur Tabellen-Kommentare, Kastenmasse, Reichweiten).
* Waffe 18 (Raketenwerfer) hat im Entlade-Dispatch `[18] = {1,0,1,0}` = resolve 0 (game_step_common.c:2039): kein Hitscan, der
  Schaden kommt nur ueber RE2-Op 24 -> Applier. Art 9 entsteht nur im Applier-Zweig fuer RE1.5-KI-Kandidaten (re15_damage.c:4612-4622).
* Die Handgranaten-Explosion (Art 2) geht fuer RE2- und RE1.5-KI-Gegner durch `re15_resolver_gegnerzweig(..., NULL)` ->
  `re15_enemy_take_damage_at`; ohne `gl` greift der RE2-Satz-Block nicht, D_G bleibt.
* **Bewertung: OK.**

## 3. Tests (Pruefauftrag 3) — selbst gefahren, Summenzeilen woertlich

| Gruppe | Aufruf | Summenzeile |
|---|---|---|
| Unit/Prop/Probe (Auswahl wie der Bauer) | `ctest -R '^(unit_\|prop_\|probe_)' -E 'unit_r38_releaseskript_\|unit_r39_titelversion_ablauf\|unit_r35_android_bash_mutanten' -j 4 --timeout 600`, PATH msys64 zuerst, SDL_ASSERT=always_ignore | `100% tests passed, 0 tests failed out of 1066` (`Total Test time (real) = 468.84 sec`) |
| Integration, 42 Riegel (Auswahl wie der Bauer) | `ctest -R 'integration_(r30_granate_laden\|r34_granaten\|r35_granate\|r35_werfer\|r36_granate\|r36_inventar\|r38_kleinkram_flammenwerfer\|r41_alligator2_\|r41_gatorszene_\|r41_kanister2080\|r41_re2gator\|r41_restpunkte_gator\|r41_birkin_pistole\|r41_g5_westen\|r41_g5_tod)' -j 2 --timeout 3000` | `98% tests passed, 1 tests failed out of 42` (`Total Test time (real) = 3156.22 sec`). Rot nur `integration_r41_alligator2_k3_elza` (Schluessel-Puls im Bildvergleich: `bilddiff: 44 Bilder, 44 mit Schluessel-Unterschied, Kasten (0,0)-(319,239) = 320 x 240` - das ganze Bild wich ab, Last durch parallele Laeufe). Einzeln dreimal nachgefahren: jeweils `100% tests passed, 0 tests failed out of 1` (74.04 / 74.00 / 73.98 sec). Damit **42/42 gruen**; der Riegel prueft die Schluessel-Szene, nicht den Schaden |
| Gegenprobe alte Engine (eigener Bau) | 9e0066c26 per `git archive` (engine, include, platform/pc/src, tests/test_support.c), 232 Objekte mit den Flags des echten Baus (`ninja -t commands`) selbst uebersetzt, gegen `test_r41_granate.c` des Kopfes gelinkt | `ws`: `test_r41_granate: 4 Fehler, erste Pruefung 140` (FAIL 140 `0/39`, 141 `HP -750`, 142 `HP -749`, 144 `29/60`); `boss`: `test_r41_granate: 27 Fehler, erste Pruefung 70` (gruen nur 121 und 131, die G5-Rakete war schon 200). Dieselbe Testdatei gegen die Engine des Kopfes: beide `test_r41_granate: ALLE PRUEFUNGEN GRUEN` |
| Umgekehrte Gegenprobe (eigener Bau) | die Riegel-Dateien VOR WS (9e0066c26: probe_r34_schaden, probe_r34_wurf, test_r35_granate, probe_r34_reaktion, test_birkin_bahnsteig) gegen die Engine des Kopfes gelinkt | `probe_r34_schaden alle: 8 Fehler (erste Pruefung 13)` (13, 13b, 99, 31, 34, 35, 37, 38); `probe_r34_wurf: 1 Fehler, erste Pruefung 24`; `test_r35_granate: 9 Fehler, erste Pruefung 112` (112, 120, 121, 122, 124, 126, 127, 130, 234); `probe_r34_reaktion alle: 3 Fehler (erste Pruefung 190)` (190, 194, 212); `test_birkin_bahnsteig: 2 Fehler` (M4 5090/5091). Genau die Stellen der Liste (e) - die Nachzuege waren noetig, nicht beliebig |

* Die Gegenprobe deckt sich zahlengenau mit der des Bauers (`vorher_ws_boss_beleg.log`): die Riegel unterscheiden alt und neu.
* Die Riegel-Aenderungen in cfe403cbc und 87391a514 habe ich gelesen: sie pruefen weiter genaue Betraege (z. B. CHECK 35
  `300 - RE15_GRANATE_DG` = 49, PRUEF 141/142 `-1`/`0`), keiner ist aufgeweicht.

## 4. Eigene exe-Laeufe (Pruefauftrag 4) — Leon und Elza

Messkopie `re15_pc_wsabn.exe` (Kopie der gebauten exe), ein Lauf zur Zeit. Waffen ueber die Wege der Granaten-/Raketen-Riegel:
echter Wurf (Aufstellung `integration_r35_granate` "zombie"), `RE15_FORCE_EXPLOSION=2@<bild>:<slot>` (echte Explosionsroutine Art 2,
wie `integration_r35_granate` gator/birkin), Rakete per Eingabeskript (wie `integration_r41_g5_tod`). Umgebung und HP-Folge je Lauf:
`analysis/befunde_runde41/WS_belege/abn1/abn1_logauszug.txt`.

### 4.1 Standard-Gegner: eine Granate

| Lauf | Figur / Raum | Ziel | HP-Folge (state.log) | Soll | Bewertung | Bild |
|---|---|---|---|---|---|---|
| zl_wurf | Leon 1140, **echter TIEF-Wurf** | 0x10 s2/s3, 0x11 s4/s5 | `EV resolver art=2 ... eingriffe=4`; F119: s2 50 -> **-201**, s3 80 -> **-171**, s4 250 -> **-1**, s5 250 -> **-1**; alle Zustand 3, Leiche (Z7) F170/F180 | 0x10 tot, 0x11 "gerade so" -1 | OK | abn1_01 |
| ze_16 | Elza 1221 | 0x16 s1 (s2 im Kasten) | F120 s1 95 -> **-156**, s2 75 -> **-176**, Z7 F191 | tot | OK | abn1_02 |
| hund_e | Elza 11C1 | Hund 0x20 s2 | F120 98 -> **-153**, Z7 F142 | tot | OK | abn1_03 |
| hund_l | Leon 1230 | Hund 0x20 s1 | F120 96 -> **-155**, Z7 F142 | tot | OK | abn1_04 |
| spinne_l | Leon 2030 | Spinne 0x25 s2 | F120 111 -> **-140**, Z7 F132 | tot | OK | abn1_05 |
| spinne_e | Elza 2031 | Spinne 0x25 s2 | F120 111 -> **-140**, Z7 F132 | tot | OK | abn1_06 |

### 4.2 Bosse

| Lauf | Figur / Raum | Waffe | HP-Folge (state.log) | Soll (Blatt d / H3) | Bewertung | Bild |
|---|---|---|---|---|---|---|
| gorilla_l2 | Leon 11C0 (Aufstellung GR-Abnahme 2) | 2 Granaten | F300 360 -> **109** (Z2); F360 kein Treffer (Zustand 2 = Treffer-Riegel `Treffer=0`); F420 109 -> **-142** (Z3), Leiche F461 | 2 (109, -142) | OK | abn1_07 |
| birkin_l | Leon 3070 (Flag 4:215), 600-Form | 3 Granaten | F150 600 -> **349** (+5 Fern-Rueckgabe -> 354), F210 -> **103** (+5 -> 108), F270 -> **-143** Z3 | 3 | OK | abn1_09 |
| gator_l / gator_e | Leon 2090 / Elza 2091, Erst-Eintritt | 1 Granate | F6 **25240**, F60 -> **24989** | 25240, 24989 < 25000 | OK | — |
| gator_l2 / gator_e2 | Leon 2090 / Elza 2091, Kanister-Aufstellung (`9:110,3:245`) | 1 Granate | F300 25240 -> **24989**; **F302 `AUSLOESER geschwaecht ... Treffer 1` -> ROOM2080 / ROOM2081** (Kanister-Szene) | 1 Granate bis zur Schwaeche | OK | abn1_08 |
| gator_rak | Leon 2090, Kanister-Aufstellung | 3 Raketen | F266 **25130**, F296 **25020**, F328 **24910**; F370 `AUSLOESER geschwaecht ... Treffer 3` -> ROOM2080 | 3 (25130, 25020, 24910) | OK | — |
| endk_l_ma13 | Leon 5090 (Skript wie g5_tod) | Rakete | Form 1: F413 400 (+5), F444 205 (+5), F475 10 -> Mutation F477 300; Mutation F507-F635 Revive 100; **Riesenform F654 1000: F666 699, F698 398, F730 97, F761 -204** (Z3, Rueckstoss HP 1); G5 F866 600 | Riesenform 4 Raketen | OK | abn1_10 |
| endk_e_ma13 | Elza 5091 (Birkin in Slot 1) | Rakete | dieselbe Folge: Riesenform F654 1000, **699 / 398 / 97 / -204**; G5 F866 600 | 4 | OK | abn1_12 |
| endk_l_gr | Leon 5090, Rakete nur bis zur Mutation (MA9), dann Art 2 | 4 Granaten | Riesenform F654 1000, F700 **749**, F760 **498**, F820 **247**, F880 **-4** (Z3, Rueckstoss HP 1); G5 F991 600 | 4 (749, 498, 247, -4) | OK | abn1_11 |
| endk_e_gr2 | Elza 5091, wie oben, Slot 1 | 4 Granaten | F700 **749**, F760 **498**, F820 **247**, F880 **-4**; G5 F991 600 | 4 | OK | abn1_13 |
| g5_l_lang / g5_l_fd | Leon 5090, Dauerfeuer bis nach der G5-Szene (zweimal, einmal mit Bildserie) | Rakete | G5 F866 600; nach dem Szenenende **F2113 400, F2144 200, F2173 0 (lebt, Z2), F2204 -200 (Z3)**, in beiden Laeufen gleich | 4 (400, 200, 0, -200; H7) | OK | abn1_14 |
| g5_e_lang | Elza 5091, wie oben | Rakete | **F2113 400, F2144 200, F2173 0 (Z2), F2204 -200 (Z3)** | 4 | OK | — |

### 4.3 H8 / H9 (beim Bauer offen)

| Lauf | Raum | Aufstellung | Ergebnis | Bewertung |
|---|---|---|---|---|
| gitter_l, gitter_l2 | Leon 1210, 10 Gitterhaende 0x1A (HP 250) | Spawn bzw. Spieler bei (-19300,-8850), Explosion Art 2 an s1 (F120 bzw. F150/F200/F250) | `Treffer=0` in jedem Versuch, HP bleibt 250 | im Spiel so nicht als Kandidat erreichbar; der Wert D_G an 0x1A ist per Unit belegt (PRUEF 4, Rakete 40 PRUEF 144). Der DEATH-Zweig haengt an hp < 0, nicht am Betrag (vorher 1000 = gleiches Verhalten) |
| feuer_l | Leon 1090, 7 Feuer-Emitter 0x26 (HP 100) | Explosion Art 2 an s1 F120, P.y = Boden - 500 | `Treffer=0` | Emitter ist an dieser Stelle kein Kandidat; H9 im Spiel ohne Wirkung (Unit probe_r34_reaktion 212 belegt nur den Rechenweg 100 -> -151) |

Messweg-Grenzen (keine WS-Fehler, die Logik ist unveraendert gegenueber 9e0066c26):
* ROOM1140 vom Spawnpunkt aus (zl_11/zl_10): `RE15_FORCE_EXPLOSION` F120 -> `Treffer=0`, die Zombies fressen (ss1=8) und sind
  dann keine Kandidaten; mit dem echten Wurf aus der Riegel-Aufstellung (Spieler nah) werden sie getroffen (zl_wurf).
* ROOM11C0 vom Spawnpunkt aus (gorilla_l): dreimal `Treffer=0` (Gorilla weit weg, nicht im Kampf); in der Aufstellung der
  GR-Abnahme 2 (gorilla_l2, Spieler nah, Flag 4:0x40) trifft die Explosion.
* 5080/5081 (Birkin geparkt, ss1=9): `Treffer=0`; 3071 (Elza): die erste Granate trifft (600 -> 349), der Birkin bleibt danach in
  Zustand 2 (Kampf nicht gestartet, wie GR-Abnahme 2 Mangel 2). Die 600-Form ist deshalb im Fenster nur fuer Leon (3070) belegt,
  Elzas Formen ueber den Endkampf 5091 (Form 1, Riesenform, G5) und die Unit-Faelle 76/78/79.
* Bildtafeln Gorilla (abn1_07) und Birkin 3070 (abn1_09): das Ziel steht ausserhalb des Kamerabilds; die Zahlen kommen aus state.log.

## 5. Maengel

| Nr | Punkt | Soll | Ist | Beleg | Bewertung |
|---|---|---|---|---|---|
| M1 | Kommentare mit alten Zahlen | Zahlen passend zu 25240 / D_G | Drei Kommentare nennen noch "43 Pistolenschuesse" bis zur Gator-Schwaeche: `engine/src/kanister2080_r41.c:681`, `tests/integration/test_r41_kanister2080.cmake:16`, `tests/integration/test_r41_kanister2080_kampf.cmake:5`. Mit 25240 sind es **35** (Pistole am Alligator 7 = `read 0x8006ece4` -> 7: 25240 - 34 x 7 = 25002 >= 25000, 25240 - 35 x 7 = 24995 < 25000). `re15_damage.c:3211-3212` sagt weiter, die Feuer-Emitter von ROOM1090 "behalten 1000" (Art 2/9 sind jetzt 251/200; ohne Wirkung, die Emitter lesen ihre HP nicht, Kommentar :742-745) | `git grep "43"`, `abn1_disassembly.txt` | gering, nur Kommentar; kein Riegel haengt daran (Messhaken `RE15_K2080_GATOR_HP`) |
| M2 | Beleg-Genauigkeit H3 | vollstaendiger Schreiber-Scan, RAM-Adressen | Header `re15_gator_re2_r41.h` und Dossier 2.3 nennen "Scan aller 34 Stores"; in PSX.EXE + COMMON/BIN sind es **72** Kandidaten-Stores (Register-indirekt `addiu rX,rX,-1164` + `sw 0(rX)` und ueber die G-Basis `sw 14732(s0)`), dazu 84 in den RE2-Gegnermodulen. Die Overlay-Adressen von TITLE/MEM_CARD/RESULT/ENDING (z. B. "MEM_CARD @0x80103af4", "TITLE @0x80100ce8", "RESULT @0x80100514") sind Werkzeug-Adressen (Datei-Offset + 0x80100000), nicht RAM: TITLE ist auf 0x8019xxxx gelinkt, RESULT auf 0x8015xxxx, ENDING auf 0x8011xxxx, MEM_CARD auf 0x801Bxxxx-0x801Cxxxx (Sprungziele). Das Ergebnis "kein Schreiber loescht 0x40" haelt auch im vollstaendigen Scan | `abn1_h3_schreiberscan.txt` | gering, das Ergebnis und die Aenderung stimmen |

Kein blockierender Mangel. Die Messweg-Grenzen in Abschnitt 4 (1140 vom Spawnpunkt, 5080/5081/3071, H8/H9 ohne Kandidat) sind keine
WS-Fehler: die Kandidaten-Tore sind gegenueber 9e0066c26 unveraendert, WS aendert nur die Betraege.

## 6. Offen / Hinweise fuer die Release Notes

* **Gator-Boss und GR-Wunsch F4 (Nutzerfrage).** Mit RE2-Start-HP 25240 (H3) und D_G 251 schwaecht EINE Handgranate den Alligator
  und startet sofort die Kanister-Szene: gemessen Leon und Elza F300 25240 -> 24989, F302 `AUSLOESER geschwaecht ... Treffer 1`.
  Das widerspricht dem frueheren Wunsch F4 ("nicht einfach mit einer Granate beenden"), folgt aber aus "mach wie resident evil 2".
  Toeten kann ihn weiterhin nur der Kanister. Mit der Rakete: 3 Treffer bis zur Schwaeche (gemessen 25130 / 25020 / 24910).
* **Treffer bis zum Tod (fuer die Release Notes, alles gemessen):** Standard-Gegner 1 Granate (0x11 mit genau -1); Gorilla
  2 Granaten; Birkin 600-Form 3 Granaten; Riesenform 4 Granaten / 4 Raketen; G5 4 Raketen (nach 3 Raketen 0 HP, er lebt);
  Gator-Boss 1 Granate / 3 Raketen bis zur Schwaeche. Aus der Unit (nicht im Fenster): G5 3 Granaten, Birkin nach der Mutation
  2 / 2, Form 1 bis zur Mutation 2 Granaten / 3 Raketen, Alligator RE1.5-KI 2 / 3.
* **Endkampf mit der Rakete (H11):** 11 Treffer (Form 1 3, Riesenform 4, G5 4). Wer durchgehend feuert, verbraucht bis zum Tod des
  G5 **19** Raketen (gemessen g5_l_lang: Magazin 200 -> 181 bis F2199): 5 gehen waehrend der Mutation in den Revive (100), 3 in
  die Rueckstoss-Szene. Dass im Raum keine Raketen liegen (GR N3.5), habe ich nicht nachgeprueft.
* **Gitterhaende 0x1A:** die Rakete macht jetzt 40 (RE2-Arme @0x800A52C0) statt 1000 = 7 Raketen bis zum DEATH-Zweig statt 1
  (Unit; im Fenster in meiner Aufstellung kein Kandidat).
* **Ausserhalb von WS (Bauer-Hinweis bestaetigt):** RE2 setzt Bit 0x20 von 0x800cfb74 im Titel (`ori v0,a0,0x20`, Werkzeug-Adresse
  0x80102994 in TITLE.BIN, gelinkt 0x80192994) und beim Laden (MEM_CARD); mit Bit 0x20 waehlt RE2 fuer den G4 900 statt 1000
  (EM34 `80100688 andi v0,v0,0x20` / `80100690 addiu v0,zero,900`). Der Port hat keine Schwierigkeitswahl, das Bit bleibt dort 0;
  fuer den Gator ohne Folge (beide Zweige 240). Der Satz "Bit 0x20 nie gesetzt" in `re15_birkin_formen.h` gilt fuer den Port,
  nicht fuer RE2.
* Kein push, kein merge; geaendert habe ich nur diesen Bericht und `WS_belege/abn1/`.

## 7. Belege

Alle unter `C:/workspace/git/reAi_v2/.claude/worktrees/r41_waffen/analysis/befunde_runde41/WS_belege/abn1/`:

* Bildtafeln (RE15_FRAMEDUMP-Serien, beschriftet mit Bild, Slot, Typ, HP, Zustand aus state.log):
  `abn1_01_zombies1140_leon_echter_wurf.jpg`, `abn1_02_zombie16_1221_elza.jpg`, `abn1_03_hund11c1_elza.jpg`,
  `abn1_04_hund1230_leon.jpg`, `abn1_05_spinne2030_leon.jpg`, `abn1_06_spinne2031_elza.jpg`,
  `abn1_07_gorilla11c0_leon_zwei_granaten.jpg`, `abn1_08_gator2090_leon_eine_granate.jpg`,
  `abn1_09_birkin3070_leon_drei_granaten.jpg`, `abn1_10_riesenform5090_leon_vier_raketen.jpg`,
  `abn1_11_riesenform5090_leon_vier_granaten.jpg`, `abn1_12_riesenform5091_elza_vier_raketen.jpg`,
  `abn1_13_riesenform5091_elza_vier_granaten.jpg`, `abn1_14_g5_5090_leon_vier_raketen.jpg`
* `abn1_logauszug.txt` — Umgebung, Harness-/Ausloeser-Zeilen und HP-Folge je Lauf (30 Laeufe, auch die Fehlversuche gorilla_l, endk_e_gr (falscher Slot) und gitter_l; die Zensus-Laeufe z* liegen nur im Scratchpad)
* `abn1_disassembly.txt` — die eigene Disassembly aus Abschnitt 2 (0x11-HP, Todesregeln, Applier, Hitcodes, Zeile 17 je Typ,
  Gator-Init und Daten, Setter, RE1.5-Gegenstelle, Zombie-Tabelle, G4-HP)
* `abn1_h3_schreiberscan.txt` — 72 Stores auf 0x800cfb74 mit Kontext, Aufrufe mit konstantem Zielzeiger
* `abn1_gegenprobe.txt` — FAIL-/ok-Zeilen der Gegenprobe (alte Engine) und des Kopfes
* `werkzeuge/` — alle eigenen Skripte (Laufhaken, Auswertung, Bildtafeln, Scans, Gegenprobe)

Rohdaten (state.log, debug.log, Framedumps, Testlogs `ctest_unit.log` / `ctest_integ.log` / `ctest_k3elza_1..3.log`, Gegenprobe-Bauten `gegenprobe/`) bleiben im Scratchpad
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ws/abn1/`.
