# Runde 38 — Spur R "spinnen": Abnahme 1 (unabhaengig)

Datum 2026-10-17. Pruefer: unabhaengige Abnahme 1 (Workflow-Unteragent). Baum `.claude/worktrees/r38_spinnen`, Zweig
`r38/spinnen`, gepruefter Code-Stand `08faea87` (Code identisch mit `557a0ece`; danach nur `f584a35e` = Teilbericht
dieser Abnahme beim Sitzungslimit), Basis (merge-base) `e2cc7673`. Vorgaenger: `R_abnahme_0.md` (Stand e7131aca;
Punkt 1 und 3 erfuellt, Punkt 2 teilweise, Maengel 1-3, Hinweis 1). Dieser Bericht ersetzt den Teilbericht
`R_abnahme_1_teil_limit.md` (Sitzungslimit ~09:25).

**Ergebnis: (in Arbeit — Bericht wird fortlaufend geschrieben)**

Messordner (ausserhalb des Baums, eigener Unterordner):
`C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\r38R_abn1\`
(unten `A1\`). Laufskript `A1\lauf_abn1.sh <neu|alt> <name> <exit_at> ENV=...`: "neu" startet die Messkopie
`re15_pc_r38R_abn1.exe` (Kopie von `re15_pc.exe` des Spurbaums, 09:26:35) mit `RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_SPINNEN_LOG=spinnen.log RE15_STATE_LOG=state.log`. Stapel
`A1\batch1.sh` ... (Befehle stehen dort woertlich). Eigener Auswerter `A1\ana_abn1.py` (unabhaengig von
`gift_takt_ana.py` und `pruef_log`): verbindet spinnen.log und state.log desselben Laufs Zeile fuer Zeile (Bildnummer
geprueft, 0 Fehlpaarungen in jedem Lauf), erkennt Gift-Takte (gz 0 -> 120 mit HP -2), zaehlt je Takt im Fenster der
25 Folgebilder Treffer-Reaktionen (Spieler-Kommando `pst == 2`, Clip `mo` 8/9/10 aus state.log oder `phf == 1`) und den
Weg des Spielers, Perioden zwischen Takten ohne Fremdtreffer, und Spinnen-Lagen gegen die SCA-Bodengroesse `bu`.

## 0. Grundlage
- Gelesen: AUFTRAG.md (Wortlaut Z. 68-70 und "Nachtrag Integration" 07:36), VERTRAG.md (Spur R, 1.4), Dossier
  R_spinnen.md vollstaendig (711 Zeilen, inkl. Abschnitt "Nachbesserung 1"), R_abnahme_0.md, `git log --oneline
  master..HEAD` (64 Commits inkl. Spur-B-Merge), `git diff master...HEAD --stat`/`--name-only`. Code-Diff gelesen:
  `git diff e05ec5f7 e2afbd1f^1 -- re15_port` (Nachbesserung M1/H1 vor dem Merge), `git diff e2afbd1f HEAD -- re15_port`
  (M3 nach dem Merge), `git show --cc e2afbd1f` (Merge ohne Handaufloesung), spinnen_r38.c vollstaendig (811 Zeilen),
  game_step_common.c um den Treffer-Detektor (Z. 1286-1328) und den Kommando-1-Prolog (Z. 1627-1637).
- Merge Spur B: `e2afbd1f` hat die Eltern `d026986e` und `2af9d15f`; `r38/szenenschutz` steht auf `2af9d15f`;
  `r38/integration` enthaelt `8877e863` mit dem zweiten Elternteil `2af9d15f` — derselbe Commit wie behauptet.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`, `build` -> "ninja: no work to
  do", `LOCAL-BUILD-OK (build)`. (Im ersten Teil dieser Abnahme vor dem Sitzungslimit waren drei Quellen per `touch`
  neu uebersetzt worden — Inhalt unveraendert, `git status` leer — exe 09:26:35. Diese exe wird gemessen.)

## 1. Gezielte Tests (keine volle Suite)
- Befehl (erster Teil dieser Abnahme, 09:28-09:35, auf genau dieser exe/diesen Test-Binaries — seither "ninja: no work
  to do"): `ctest --test-dir re15_port/build -R "<R>" --timeout 900 --output-on-failure -j 3` mit R = die 109 Tests
  des Bau-Agenten **plus** 47 weitere aus den Testgruppen der vom Diff beruehrten Module (game_step_common.c
  Treffer-Detektor/Kommando 0/Zielen: unit_zombie_10d0_reentry_a/b, unit_r30_android_n3_plc_aim,
  unit_r30_android_r1_toggle, unit_action_msg_gate, unit_aim_all_around, unit_aim_lower_exit, unit_em_status_persist,
  unit_dog_aim_band; Inventar/Kommando 0: unit_r26_inventar, unit_r36_inventar, integration_r36_inventar,
  unit_r35_inventar1050_*, integration_r35_inventar1050; Granaten/Gift-Status: integration_r34_granaten,
  integration_r30_granate_laden, integration_r35_granate, integration_r36_granate; Spur-B-Nachbarn:
  unit_r35_affen_*, unit_r36_zombieki_affen_block, unit_r36_zombieki_szenen). Regex woertlich in `A1\regex.txt`.
  Ergebnis: **"100% tests passed, 0 tests failed out of 156"** (Total 460.88 s), Ausgabe `A1\ctest_gezielt.txt`.
- In dieser Sitzung nachgefahren (`-V`, Ausgabe `A1\ctest_unit_s2.txt`): unit_r38_spinnen, unit_r34_wurf, unit_damage,
  unit_r38_szenenschutz(_alle/_arm) — **6/6 gruen**. Gelesen: `test_r38_spinnen: 0 FAIL`, darin
  `== B4 ... ok stehend: 4 Takte zu -2 HP in Bild 0/121/242/363`, `ok stehend: keine Treffer-Reaktion (Flinch-Bilder 0,
  Clip 8/9/0xa 0, Rueckstoss 0, SE CORE 0 ...)`, `ok Kommando 0: Zaehler 84 -> 0 (@0x8003196c), naechstes Bild Takt HP
  92 -> 90`, `ok zielend: ... 0 Bilder NICHT zielbereit`, `== B5 ... ok Szene haelt: HP 100, status 0x0000, Gift-Treffer
  0, +0x93 vom ESP-Takt veraendert 0 mal`, `ok Gegenprobe frei (player_mode 0): HP 88 = 100 - 10 - 2, status 0x0002`;
  probe_r34_wurf: `6d HOCH vergiftet : Hoehe 1 Wort 8002, Abzug 0, Spawn A+19 ...`, `ALLE PRUEFUNGEN GRUEN`;
  unit_damage: `PASS: test_bleed_gate (0/2000, wie ROOM2060 gemessen: a0 = 0x0A -> nie)`.

## 2. RE-Gate: Stichproben selbst disassembliert (richtige Binaerdatei, Sprungziele mitgelesen)
Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` (RE1.5 `info/Re1.5/PSX.EXE`, `--bin STAGE2.BIN`) und
`re2_disasm.py` (RE2 `info/re2leon/PSX.EXE`, `--bin EMS25.BIN`), aus dem Hauptbaum nur gelesen. Alle Stichproben der
Nachbesserung stimmen mit dem Zitat im Code/Dossier:

| Behauptung (Nachbesserung 1) | selbst gelesen |
|---|---|
| Kommando-0-Handler nullt den Gift-Zaehler `sb zero,-13273(at)` @0x8003196c (H1) | Tabelle `0x80073f90[0] -> 0x800318f8`, `[1] -> 0x80031de8`; im Handler `8003192c sw v0(1),-13736(at)` (Kommando 1), `80031964 sb zero,-13593(at)` (+0x93), **`8003196c sb zero,-13273(at)` = 0x800acc27**, `80031974 sb zero,-13300(at)` (0x800acc0c) |
| Kommando nullen `sb zero,0x800aca58` @0x8001cbdc | `8001cbdc sb zero,-13736(at)` = 0x800aca58 |
| Gift-Takt 0x80031e44-ec8 schreibt NUR Zaehler + HP, kein `jal` (M1) | `80031e48 lhu a0,-13588(a0)` (0x800acaec) / `80031e50 andi v0,a0,0x7` / `80031e54 beq`; Stores nur `80031e70/80/94 sb ...,-13273(at)` (0x800acc27) und `80031eac/80031ec8 sh ...,-13586(at)` (HP +0x9a); kein `jal`, kein Store auf 0x800aca58/+0x93; ab `80031ecc` Dispatch ueber 0x800aca59 |
| RE2-Gift-Takt @0x8003c12c-180 ebenso | `8003c130 lhu 0x800cfe10 / beq`, `8003c14c addiu v0,v1,255 / sb 0x800cfe12`, `8003c15c addiu v0,zero,30`, `8003c174 slti v0,v0,2 / addiu -1 / 8003c180 sh v0,342(s0)`, danach `8003c1a4 jalr` des Zustands — kein Kommando-/Zustandsschreiber |
| Treffer-Reaktion nur durch Kommando-2-Schreiber `sb v0,4(s1)` @0x80012ebc; Tor +0x93 @0x80012e24-30 (M3) | `80012e24 lbu v0,-13593(v0)` (+0x93) / `80012e2c andi v0,v0,0x1` / `80012e30 bne v0,zero,0x80012f00`; `80012eb8 ori v0,zero,0x2` / **`80012ebc sb v0,4(s1)`** (s1 = 0x800aca54); `80012eec sb v0,147(s1)` (+0x93 \|= 1). Sprungziel `80012f00 addiu s4,s4,1` = Trefferzaehler laeuft auch beim gesperrten Spieler (Klumpen zerplatzt) |
| Spuck-Entscheid RE1.5 liest +0x93 @0x801111fc (STAGE2.BIN) | `801111fc lbu v0,-13593(v0)` / `80111204 andi v0,v0,0x1` / `80111208 beq` -> sonst `8011121c sb 1,5` / `8011122c sh 0xb,6` |
| Spuck-Entscheid RE2 liest KEIN Spieler-Tor (EMS25 @0x80100784-8c0) | Leiter liest `+0x237`, `+0x1F4/+0x1F0`, `jal 0x80105190` (3x), `0x800cfbf6`, `jal 0x80015fe8` (rng), `+0x218`; Wort `0x501/0x901/0x701/0x801` nach `4(s0)`. Mitgelesene Sprungziele: `0x801006c4` (Leerlauf-Clip + Zeitgeber, kein Spielerfeld), `0x80105190` (Bogentest, liest nur Lage x/z +56/+64 des Ziels 0x800cfe18) — kein +0x1D3 |
| Set(2,7) sperrt nur das Pad (Dossier N1.M3) | `800304f4 lw g_pauseflags / lui v1,0x100 / and / beq` -> `0x800ac768 &= 0xf000` (Pad-Maske) |

Weitere Pruefungen des Diffs:
- Suchwoerter in allen hinzugefuegten Zeilen `git diff master...HEAD -- re15_port`
  (`deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geschaetzt|vermut|ungefaehr|nach gefuehl|gerat`):
  **0 Treffer**. Einziges neues `getenv` ist die Messschiene `RE15_SPINNEN_LOG` (kein Spielverhalten).
- Neue Konstanten der Nachbesserung: keine Zahl ohne Adresse. Gift-Zaehler 0 @0x8003196c; der Abzug wird aus dem
  Takt selbst zurueckgegeben (keine neue Konstante); Szenenschutz-Haken nutzt Bit 0 von +0x93 (@0x80012e24-30).
- Erklaert der Fix den gemessenen Befund? **Ja**: (M1) Port-Detektor `pl->hp < s_prev_hp` (game_step_common.c:1295)
  sah den HP-Abfall des Takts, weil der Takt (Z. 1633) NACH der Basislinie `s_prev_hp = pl->hp` (Z. 1328) lief; jetzt
  wird die Basislinie um genau den Takt-Abzug gesenkt (Z. 1635-1636), fremde Abzuege im selben Schritt bleiben sichtbar
  (Gegenkontrolle: Spucktreffer reagieren weiter, Abschnitt 3.1). (M3) Routine 25 lief ausserhalb von
  re15_enemy_ai_run_all; jetzt setzt sie fuer ihre Dauer selbst das Tor-Bit, wenn `re15_szenenschutz_haelt()`.

## 3. Maengel der Abnahme 0 — an der exe nachgemessen
### 3.1 Mangel 1 (Gift-Takt loest Treffer-Reaktion aus): BEHOBEN
- Lauf `A1\a1_gift_2030` (Aufbau wie Abnahme-0-Lauf p2b, sonst ohne Eingabe):
  `lauf_abn1.sh neu a1_gift_2030 "1300#2030" RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,2000,3072,3 RE15_GIVE=35:1
  RE15_FRAMEDUMP=150-330/2:fd_`. Auswertung `python ana_abn1.py A1\a1_gift_2030 2030`:
  ```
  1300 Bilder (F1..F1300), Zeilen-Fehlpaarungen 0; HP-Abzuege {2: 10, 10: 4}; Gift-Bit an in F: [19]
  Takt F42  HP 90->88 ... Reaktionsbilder 0 | mo 200,200,...      [fremder Abzug F19]
  Takt F163 HP 88->86 Periode 121 | Reaktionsbilder 0 | Weg 9  | mo 211,211,...
  Takt F284 / F405 / F526: Periode 121, Reaktionsbilder 0, Weg 16/25/0
  Takt F691 Periode 165 (2 Fremdtreffer F580/F639 dazw.) | Takt F812/F933 Periode 121 | F1076 Periode 143 (F1041) | F1197 121
  ERGEBNIS Takte: 10, ohne fremden Abzug +-25: 9, davon mit Reaktion 0; Perioden ohne Fremdtreffer: [121 x7]
  Spucktreffer F19/F580/F639/F1041 HP -10: Reaktionsbilder je 23   (Positivkontrolle: echte Treffer reagieren weiter)
  ```
  Vorher (Abnahme 0, p2b, Stand e7131aca): 10/10 Takte mit 22 Bildern Clip 8/9 und 500 Rueckstoss, Periode 143.
  Jetzt: Spieler-Kommando bleibt 1, Clip bleibt der Ruhe-Clip 200/211/212/213, Weg <= 25 (Koerperschub der Spinne).
  Die Perioden 165/143 entstehen nur, wo echte Spucktreffer (je 22 Bilder Kommando 2) dazwischen liegen — im Original
  laeuft der Takt nur im Kommando-1-Prolog (Handler 0x80031de8 = 0x80073f90[1], selbst gelesen), also ebenso.
- **Elza** `A1\a1_gift_2031` (derselbe Aufbau + `RE15_TITLE_SHOT_CHAR=1`, debug.log `[pl] Spieler-Familie PL04
  (character=4, Elza-Bit=1)`, Raum R2031): Zahl fuer Zahl dasselbe Ergebnis (10 Takte, 0 Reaktionen, Perioden 121).
### 3.2 Mangel 2 (Regression unit_r34_wurf): BEHOBEN
- unit_r34_wurf gruen (Abschnitt 1): `6d HOCH vergiftet : Hoehe 1 Wort 8002, Abzug 0, Spawn A+19` (Abnahme 0: Abzug -1,
  Spawn A+0), `probe_r34_wurf: ALLE PRUEFUNGEN GRUEN`. Der Test steht jetzt in der Auswahl des Bau-Agenten.
### 3.3 Hinweis 1 (Gift-Zaehler ueberlebt Kommando 0): BEHOBEN, Beleg stimmt
- Lauf `A1\a1_h1_menu`: vergiftet (Spucktreffer F19), dann Statusschirm auf/zu OHNE Gegengift:
  `lauf_abn1.sh neu a1_h1_menu "500#2030" RE15_DEBUG_JUMP=2030@250 RE15_PLAYER_POS=6550,2000,3072,3
  RE15_INPUT_SCRIPT=S0.1,W2,S0.1,W5 RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT_BASIS=spiel`.
  Protokoll (spinnen.log + state.log): `F99 gz=63` -> `F100 pad=0008` (Statusschirm auf; gz steht 63, Spiel pausiert,
  Spinne `af=10` bleibt stehen) -> `F163 pad=0008` (zu) -> **`F170 gz=0`** (Kommando 0, `re15_player_cmd_zero`) ->
  **`F177 hp=86 gz=120`** = erster Takt im ersten wieder laufenden Spielbild (Spinne `af=11` erst ab F177). Ohne den
  Haken haette der Takt erst 63 Bilder spaeter gefeuert. Original: `sb zero,-13273(at)` @0x8003196c im
  Kommando-0-Handler (Abschnitt 2), danach `bne v1,zero` @0x80031e74 -> Takt sofort.
### 3.4 Mangel 3 (Giftklumpen ausserhalb des Szenenschutzes): BEHOBEN
- Lauf `A1\a1_szene_2030` = a1_gift_2030 + `RE15_SET_FLAG_AT=2:7@260` (Cutscene-Latch, Port player_mode 2):
  debug.log `[setflag-at] Frame 260: flag(2,7/0x07) = 1`, `[szenenschutz] F261 an (Szene haelt den Spieler)`, dann
  `[szenenschutz] F580 Giftklumpen abgefangen (Absender Slot 1, Spieler HP 80)` (3x), `F638 ...` (3x), `F1041 ... HP 72`
  (3x). Auswertung: player_mode 2 in 1040 Bildern; HP-Abzuege {2: 11, 10: 1} — der einzige 10er ist F19 (vor der
  Szene); Gift-Takte 11, 0 Reaktionen, Perioden 121. **Kontrolle ohne Szene** (a1_gift_2030, sonst gleich): dieselben
  drei Salven kosten F580 80->70, F639 70->60, F1041 54->44.
- Der Gift-TAKT laeuft in der Szene weiter (Kommando 1). Original: der Takt steht im Kommando-1-Prolog, Set(2,7) maskiert
  nur das Pad (`800304f4-51c`, selbst gelesen) — kein Angriff, kein Mangel.
- Tor-Lesen der neuen Angriffswege (Nachtrag 07:36) selbst geprueft (Abschnitt 2): Klumpen -> Resolver-Tor +0x93
  @0x80012e24-30 (Port: Haken setzt Bit 0 fuer die Dauer der Routine, `re15_resolve_attack` + Gift-Tor `frei` lesen es);
  Spuck-Entscheid RE1.5 liest +0x93 @0x801111fc; RE2-Spuck-Entscheid liest kein Tor (vermerkt im Dossier N1.M3, kein
  Port-Tor erfunden); Babybiss liest +0x1D3 & 0x80 (Abnahme 0, `80100e78-84`).

## 4. Nutzerpunkt 2 — "Spinnen verschiessen kein Gift ... einziger Grund fuer das Gegengift"
- Spucken + Vergiftung (alle Laeufe oben): Spucktreffer F19 `HP 100->90 ps 0000->0002` (Gift-Bit), danach Gift-Takte
  -2 HP mit Periode 121 ohne Reaktion (3.1), nie toedlich (Unit B "nie toedlich: HP 1 - 2 -> 1").
- Gegengift-Kette, Lauf `A1\a1_gegengift`: `lauf_abn1.sh neu a1_gegengift "700#2030" RE15_DEBUG_JUMP=2030@250
  RE15_PLAYER_POS=6550,2000,3072,3 RE15_GIVE=35:1 RE15_INPUT_SCRIPT=S0.1,W2,A0.1,W1,D0.1,W0.6,A0.1,W1,A0.1,W2,S0.1,W3
  RE15_INPUT_SCRIPT_START=250 RE15_INPUT_SCRIPT_BASIS=spiel RE15_FRAMEDUMP=260-560/20:fd_` (debug.log `[give] item 35 x1
  -> slot 2`). Messschiene: F19 Gift an, F42/F163 Takte (Periode 121, 0 Reaktionen), **F439 `HP 86 -> 111`, Gift-Bit
  aus** (Auswerter: "Gift-Bit an in F: [19] aus in F: [439]"). Bild `A1\belege\gegengift_F260-560.png`: EKG **"Poison"**
  (violett) F300-F420, ITEM -> Spray -> USE, F440/F460 EKG **"Fine"** (gruen), Spray aus der Liste verschwunden, ab F480
  wieder im Spiel (Leon auf dem Steg, Spinne daneben).
- Beide Szenarien: Elza (`a1_gift_2031`, PL04, R2031) Gift-Bit F19, 10 Takte ohne Reaktion (3.1).
- Echter Weg ROOM2060 (unten 6.1): die Schachtspinne landet und spuckt, `F401 Spieler HP 100 -> 90 (ps 0002)` (Leon und
  Elza), danach Gift-Takt `F424 90 -> 88` ohne Reaktion.
- Original-Einordnung (Dossier 1.4/2.7, Abnahme 0 geprueft): im ausgelieferten RE1.5 setzt der Spucktreffer das Gift-Bit
  nie (a0 = 0x0A in 13/13); Port vergiftet bei jedem Spinnen-Treffer wie RE2 (Op 32 `sh 5400,0x800cfe10` @0x80020130)
  — als NUTZER-VORGABE ("das Gift ist das einzige, das die Charaktere vergiften kann") + Beta->Retail gekennzeichnet.

## 5. Nutzerpunkt 3 — "Minispinnen ... fliegen weitestgehend in der Luft und greifen einen nicht an"
- **Leon** `A1\a1_babys_leon`: `lauf_abn1.sh neu a1_babys_leon "2400#2030" RE15_DEBUG_JUMP=2030@250
  RE15_PLAYER_POS=6550,2000,3072,3 RE15_GIVE=8:30 RE15_EQUIP=8 RE15_INPUT_SCRIPT=M1,MA0.2,M1.2,MA0.2,M1.2,MA0.2,M1.2,
  MA0.2,M1.2,MA0.2,M1.2,MA0.2,W80 RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT_BASIS=spiel RE15_FRAMEDUMP=200-2400/100:fd_`.
  Auswerter: `Baby 0x26: 20025 Bilder (Plaetze 9, max gleichzeitig 9), Boden 19620, Sprung 405, SCHWEBT 0` (schwebt =
  Lage-Y mehr als 100 ueber dem SCA-Boden `bu` ohne RE2-Sprungmodus); `Adult 0x25: 7200 Bilder ..., > 100 ueber bu: 0`;
  HP-Abzuege `{1: 22, 2: 16, 10: 1}` = **22 Bisse** (1 HP), 16 Gift-Takte, 1 Spucktreffer. Bilder
  `A1\belege\babys_leon_2030_F200-1300.png` und Ausschnitt `A1\belege\babys_leon_zoom_F300_F1000.png`: kleine braune
  Spinnen auf dem Stegboden um Leons Fuesse (F300) bzw. am Gelaender auf dem Steg (F1000, anderer Schnitt), die tote
  Spinne daneben — keine Klumpen in Gelaender-/Brusthoehe.
- **Elza** `A1\a1_babys_elza` (dasselbe + `RE15_TITLE_SHOT_CHAR=1`, Ende `1500#2031`, PL04): `Baby 0x26: 11925 Bilder
  (Plaetze 9), Boden 11640, Sprung 285, SCHWEBT 0`; HP-Abzuege `{1: 16, 2: 10, 10: 1}` = **16 Bisse**.
- **Bodenraum ROOM2070** `A1\a1_babys_2070`: `lauf_abn1.sh neu a1_babys_2070 "1500#2070" RE15_DEBUG_JUMP=2070@250
  RE15_GIVE=8:60 RE15_EQUIP=8 RE15_INPUT_SCRIPT=W3,M1,MA0.2,M1.2,(MA0.2,M1.2 x9),MA0.2,W30 RE15_INPUT_SCRIPT_START=20
  RE15_INPUT_SCRIPT_BASIS=spiel` (Skript woertlich in `A1\batch4.sh`): `Baby 0x26: 17391 Bilder (Plaetze 15, max
  gleichzeitig 15), Boden 16791, Sprung 600, SCHWEBT 0`; HP-Abzuege `{1: 28, 2: 7, 10: 1, 20: 1}` = **28 Bisse**;
  Adulte 3000 Bilder, 0 ueber bu.
- Vergleich Abnahme 0 (Stand e7131aca): Leon 0/20025 schwebend, 450 Sprung, 22 Bisse; jetzt 0/20025, 405 Sprung, 22
  Bisse — die kleine Abweichung im Sprung folgt daraus, dass Leon bei den Gift-Takten nicht mehr zuruecktaumelt
  (andere Lagen); der Baby-Code ist seit Abnahme 0 unveraendert (`git diff e7131aca HEAD --
  re15_port/engine/src/enemy_ai_re2_spider.c` ist leer; geaendert seit e7131aca nur game_step_common.c 8 Zeilen und
  spinnen_r38.c/.h (Spur R) sowie die Dateien des Spur-B-Merges).

## 6. Nutzerpunkt 1 — "Spinnen fliegen fast alle in der Luft und kommen teilweise nicht aus den Schaechten raus"
### 6.1 Schachtspinnen ueber den ECHTEN Weg
- **Leon, Tuer 2030 -> 2060** `A1\a1_tuer_2060`: `lauf_abn1.sh neu a1_tuer_2060 "700#2060" RE15_DEBUG_JUMP=2030@250
  RE15_PLAYER_POS=-7700,-7250,2048,9 RE15_PAD_AT=260:A RE15_INPUT_SCRIPT=U1.8,W40 RE15_INPUT_SCRIPT_START=300
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_FRAMEDUMP=280-480/10:fd_`; debug.log `[aot] DOOR FIRE slot=3
  rect=(-7700,-7250,hw=500,hh=900) target_cut=0 spawn=(7600,0,3950)`. Verlauf (`python schacht_trace.py`):
  ```
  F6    Spinne 1/2: st=4 s=1.0.0 g=41 (Lauern im Schacht, y -3600)
  F308  Spinne 1: s=1.0.1 g=42 @(5800,-3600,6200)      (Leon zu Fuss in Cut 1 -> sub02 setzt grid 0x42)
  F338  s=1.0.2 @(5975,-3600,4325)                     (Kriechbahn, Gitter-Ausschiebung x 5975)
  F339  s=1.0.3 @(5975,-2600,4625) rot_z=1024           (an der Wand)
  F379  s=1.0.8 g=05 @(5975,0,3137) bu=0 rot_z=0        (Landung auf dem Boden)
  F380  st=0 -> F381 st=1 (RE2-INIT auf dem Boden), F394 s=8 (Spucken) -> F401 Spieler HP 100 -> 90 (ps 0002)
  ```
  Spinne 2 lauert ohne Ausloeser (Cut 5) weiter. Adulte ausserhalb des Schachts: 321 Bilder, **0 ueber bu**. Bild
  `A1\belege\schacht_2060_tuerweg_F300-460.png`: F310-F340 kriecht die Spinne aus dem Deckenschacht, F350-F370 an der
  Wand abwaerts, ab F380 auf dem Boden neben Leon, F420 Giftwolke an Leon.
- **Elza, Tuer 2031 -> 2061** `A1\a1_tuer_2061` (+ `RE15_TITLE_SHOT_CHAR=1`, PL04, `[aot] DOOR FIRE slot=3 ...`):
  Bild fuer Bild derselbe Verlauf (F308 g=42, F379 Landung (5975,0,3137), F401 HP 100 -> 90 ps 0002).
- **RE1.5-KI** `A1\a1_tuer_2060_re15ki` (+ `RE15_AI_FLAVOR=re15`): derselbe Ausstieg (Clip 16/2/13 statt der RE2-Clips
  15/2/14), F379 Landung (5975,0,3137), RE1.5-Spucken (s=1.7.1) F393 -> F402 HP 100 -> 90 ps 0002, danach Treffer
  ~alle 61 Bilder (F463/F524/F587) wie in der Original-Messung (Dossier 1.4 "alle ~62 Bilder").
- **ROOM20A0** `A1\a1_20a0_kante`: `lauf_abn1.sh neu a1_20a0_kante "600#20A0" RE15_DEBUG_JUMP=20A0@250
  RE15_INPUT_SCRIPT=UX4.15,A0.2,W3,U1.5,W50 RE15_INPUT_SCRIPT_START=20 RE15_INPUT_SCRIPT_BASIS=spiel` (Gang + Kante
  hinunter): F6 beide lauern (st=4/1), F161 Spinne 1 g=42, F192 an der Wand (rot_z 1024), **F249 Landung
  (22732,0,-25194) bu=0**, danach Angriffe (F286 HP 100 -> 80, F478 80 -> 60). Adulte ausserhalb des Schachts 351
  Bilder, 0 ueber bu. Bild `A1\belege\schacht_20a0_F150-330.png` (aus dem Wandschacht, an der Wand hinunter, am Boden
  bei Leon).
### 6.2 Alle Adultspinnen am Boden (Messgroesse bu, am Spieler gegengeprueft)
- 0 Bilder mehr als 100 ueber bu in jedem Lauf dieser Abnahme: ROOM2030 Steg (a1_gift_2030 3900, a1_babys_leon 7200,
  Elza a1_gift_2031 3900, a1_babys_elza 4500), ROOM2060 (321 nach der Landung, Leon/Elza/RE1.5-KI), ROOM20A0 (351),
  ROOM2070 (3000). RE2-Modus +0x222 der Adulten: in allen Zeilen 0 (Boden).

## 7. Vertrag, Nachtrag Integration, Nebenwirkungen
- Pfade (`git diff master...HEAD --name-only`): nur `analysis/befunde_runde38/{R_*,B_*,AUFTRAG.md}` (B_* und AUFTRAG.md
  kommen mit dem Spur-B-Merge bzw. vom Orchestrator), `re15_port/engine|include|platform/pc|tests`. **Keine** Datei unter
  `release/`, `platform/android/`, `shared_assets/PSX/`, keine CMakeLists (Probe-Dateien `tests/unit/probes/*.cmake`
  werden per GLOB geladen; `test_r36_kleidung_hundeweg.cmake` stammt aus dem Spur-B-Merge).
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Assets/gen-Dateien (grep der hinzugefuegten Zeilen nach
  `game_flag_set|MSG_ID|k_port` leer).
- Gemeinsame Dateien, eigene Haken der Spur R (`git diff e2cc7673 d026986e`, vor dem Merge): game_step_common.c 6 Zeilen
  (Kommando-1-Prolog 4, re15_player_cmd_zero 2), re15_esp.c 5, re2_ems.c 2, scd_vm.c 2, main.c 2, re15_damage.c 1
  Codezeile + Kommentar, enemy_ai_common.c 6 einzeilige Haken (VERTRAG 1.4 nennt die Datei fuer B und T — die RE1.5-
  Spinnen-KI liegt dort; mit B ist sie hier bereits automatisch zusammengefuehrt, mit T sind Konflikte beim
  Integrations-Merge moeglich, Dossier N1 "Fuer die Integration"). Alle Haken tragen "Runde 38 Spur R".
- Merge Spur B im Spurzweig: derselbe Commit 2af9d15f, den r38/integration als 8877e863 enthaelt — beim Integrieren
  von R ist B dort schon Vorfahr, es entsteht keine Doppelung. Spur-B-Riegel in diesem Baum gruen
  (unit_r38_szenenschutz/_alle/_arm, integration_r38_szenenschutz, integration_r36_kleidung_hundeweg; Abschnitt 1).
- Riegel je Punkt (selbst laufen lassen, Abschnitt 1 + `A1\ctest_echtlauf_s2.txt`): unit_r38_spinnen A/A2/D (Punkt 1),
  B/B2/B3/B4/B5 (Punkt 2), C (Punkt 3); integration_r38_spinnen_echtlauf (echte exe, Leon 2030 + Elza 2031) heute erneut
  gefahren: **Passed 113.27 s**, L: `900 Bilder, Babys bis 9, Baby-Boden-Bilder 6380 (schwebend 0), Adult-Bilder 2700
  (schwebend 0), Bisse 7, Gift 1`, `Gift-Takte 6 ..., ohne fremden Abzug 5, davon mit Treffer-Reaktion 0; Perioden ohne
  Fremdtreffer 3, davon != 121: 0`; E ebenso bestanden.
- Nachtrag Integration 07:36 (Tor-Lesen neuer Angriffswege): erfuellt, siehe 3.4/2 (Klumpen: Resolver-Tor +0x93
  @0x80012e24-30 + RE2 +0x1D3 @0x800476ec-f4; Babybiss +0x1D3&0x80 @0x80100e78-84 — Spur Bs Sicht deckt Typ 0x26 im
  Zustand 1 ab, `szenenschutz_r38.c re15_szenenschutz_gegen`: Typ < 0x40 und +0x4 == 1; RE2-Spuck-Entscheid ohne Tor
  im Dossier vermerkt).
- Nebenwirkungen: der neue Abzug aus der Detektor-Basislinie wirkt nur, wenn der Gift-Takt HP abzieht (nur bei
  Status & 7, das im Port nur Routine 25 einer Spinne setzt; weitere Schreiber von status_flags: Member_set 17,
  Spielstand, Raumwechsel-Uebernahme — grep `status_flags |=|=`). Fremde Abzuege im selben Bild bleiben Treffer
  (Spucktreffer reagieren mit 23 Bildern, 3.1). `re15_spinnen_r38_gift_cmd0` setzt nur den Spur-R-Zaehler.
