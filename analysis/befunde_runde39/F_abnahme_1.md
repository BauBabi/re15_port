# Runde 39 — Spur F "zustand" — Abnahme 1 (unabhaengig, 2026-10-18, nach Nachbesserung 1)

Pruefer: Abnahme-Agent 1 (glaubt dem Bau-Agenten nichts Ungemessenes). Baum `.claude/worktrees/r39_zustand`,
Zweig `r39/zustand`, HEAD 8c9c104e, Merge-Basis mit master c71fa144 (selbst bestimmt: `git merge-base master HEAD`).
master steht inzwischen auf 3fa61a8d (Spur A Nachbesserung 1 + Doku voraus) — `git diff master` zeigt deshalb die
Spur-A-Dateien (stimmen_pegel_r39.*, audio_pc.c, A_*.md ...) als Rueckbau; das ist KEINE Aenderung der Spur F,
sondern der nicht nachgezogene Zweig. Alle Diff-Aussagen unten beziehen sich auf die Merge-Basis c71fa144.
Bericht fortlaufend geschrieben (Sitzungslimit).

## 0. Pruefstand
- `git status --short` (Baum): leer — Index UND Arbeitsbaum sauber (H1 der Abnahme 0 erledigt).
- Commits seit der Merge-Basis: baf9d602 .. 8c9c104e (12), davon Nachbesserung 1 = 8a6ea425, 53132013, 8c9c104e.
- Code-Diff gegen die Merge-Basis (`git diff c71fa144 --stat -- re15_port`, 13 Dateien, +1137/-44):
  zustand_r39.c (neu 130), re15_zustand_r39.h (neu 55), **platform/pc/src/zustand_r39_pc.c (neu 65)**,
  game_step_common.c (+3/-4 ... 10 Zeilen), item_use_common.c (+2), re15_inv_screen.c (+2), spinnen_r38.c (+4/-1 ... 6),
  **platform/pc/main.c (+2/-0, `git diff master --numstat` = 2 0)**, Tests: test_r39_zustand.c (neu 506),
  test_r38_spinnen.c (99 Zeilen), probes/r39_zustand.cmake (neu), test_r39_zustand_gift.cmake (neu),
  test_r39_zustand_kleidung.cmake (neu). Vollstaendig gelesen.
- Bau (Abnahme selbst): `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `... build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` — die exe
  `re15_port/build/platform/pc/re15_pc.exe` (13:05:50, 7596844 B) IST der HEAD-Stand (Baum sauber, nichts neu zu bauen).
  Messkopien der Abnahme: `re15_pc_r39f_abn1.exe`, Werkzeug `test_r39_zustand_abn1.exe`
  (Logs/Laeufe im Scratchpad `.../scratchpad/abn1_*.log`, `.../scratchpad/laeufe/<name>/`).

## 1. RE-Gate (Adressen SELBST disassembliert)
Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (info/re2leon/PSX.EXE) und `re15_disasm.py`
(info/Re1.5/PSX.EXE) — Rohbytes, nicht der Ghidra-Text.

| Behauptung (Code/Dossier) | Selbst gelesen (dis/bytes) | Urteil |
|---|---|---|
| RE2 Pause-Tor FUN_8003bfac: `DAT_800cfbdc < 0 -> return` @0x8003bfac-c0 | `8003bfb0 lw v0,-1060(v0)` = 0x800cfbdc, `8003bfc0 bltz v0,0x8003c1fc` | bestaetigt |
| RE2 Gift-Takt @0x8003c12c-180: Giftwort 0x800cfe10 != 0, Zaehler 0x800cfe12 u8 -1, bei 0 := 30, `slti 2`, HP -1 | `8003c130 lhu v0,-496(v0)`=0x800cfe10 / `8003c138 beq v0,zero,0x8003c184` / `8003c144 lbu v1,-494(v1)`=0x800cfe12 / `8003c14c addiu v0,v1,255` / `8003c154 sb` / `8003c158 bne v1,zero,0x8003c184` / `8003c15c addiu v0,zero,30` (Delay-Slot) / `8003c164 sb v0,0x800cfe12` / `8003c168 lh v0,342(s0)` (HP +0x156) / `8003c174 slti v0,v0,2` / `8003c178 bne v0,zero,0x8003c184` / `8003c17c addiu v0,v1,-1` / `8003c180 sh v0,342(s0)`; danach `8003c188 lbu v0,4(s0)` = Zustands-Dispatch | bestaetigt: Periode 31 (30..0), -1 HP, nie < 1, keine weiteren Stores |
| RE2 Op 32 @0x80020120-40: Giftwort := 0x1518, Zaehler := 30 | `80020120 addiu v0,zero,5400` (=0x1518) / `80020130 sh v0,-496(at)`=0x800cfe10 / `80020138 addiu v1,zero,30` / `80020140 sb v1,-494(at)`=0x800cfe12 (Zweig hinter `andi v0,v0,0x8` @0x80020118) | bestaetigt |
| RE2 Statusschirm-Klassifikator FUN_80069538: Giftwort != 0 -> 4 vor allem anderen | `80069544 lhu v0,-496(v0)`=0x800cfe10 / `80069550 bne v0,zero,0x80069588` mit `80069554 addiu v0,zero,4`; sonst `srl a0,1 / sltu` -> 0, `sltiu 0x29` -> 1, `sltiu 0x15` -> 2, sonst 3 | bestaetigt |
| RE2 EKG-Farbtabelle @0x800ab160, 6 Bytes je Zustand, POISON @0x800ab178 = `ff 20 ff / 08 01 08` | Rohbytes 800ab160: `20 ff 20 01 08 01 ff ff 20 08 08 01 ff 80 20 08 04 01 ff 20 20 08 01 01` **`ff 20 ff 08 01 08`** (= Offset 0x18 = 0x800ab178) | bestaetigt |
| RE2 Zeichner FUN_800754ac: Farbe = base - step * s1, s1 = 31..0 | `800755a4 addiu s1,s1,-1` / `800755a8 subu a2,v0,s1` (x = sweep - s1) / `800755c0 lbu v0,-20125(at)`=0x800ab163 (Schritt) / `800755c8 mult v0,s1` / `800755d4 lbu v0,-20128(at)`=0x800ab160 (Basis) / `800755d8 mflo a3` / `800755dc subu v0,v0,a3` / `800755e0 sb` | bestaetigt; Port-Linie i = 0 (Kopf, x = sweep-(i+1)) <-> s1 = 0 |
| RE1.5 Heilspray-Wert 100 @0x8004ae14 (`ori s0,zero,0x64`, Rohbytes `64 00 10 34`) | `8004ae14 ori s0,zero,0x64`; bytes `64 00 10 34` | bestaetigt (H2 der Abnahme 0 korrekt nachgezogen) |
| RE1.5 Gift loeschen @0x8004af8c `andi 0xfffd` | `8004af84 lhu v0,0(v1)` / `8004af8c andi v0,v0,0xfffd` / `8004af94 sh v0,0(v1)` | bestaetigt |
| RE1.5 EKG-Farbtabelle @0x8007622c, Eintrag 3 (Gift) = `20 ff 20 02` = FINE | Rohbytes: `20 ff 20 02 / 80 80 20 07 / 20 ff 20 07 / 20 ff 20 02 / 20 20 20 07 / 20 20 20 07` | bestaetigt (Beta: Gift = FINE-Gruen) |
| RE1.5 Klassifikator FUN_8004ed6c: Bit 1 von 0x800acaec -> 3 vor den HP-Schwellen | `8004ed8c lhu v0,-13588(v0)`=0x800acaec / `8004eda8 andi v0,v0,0x2` / `8004edac bne v0,zero,0x8004edf0` / `8004edb0 ori v0,zero,0x3`; Schwellen `lb 0x800112b4/b5` | bestaetigt |
| RE1.5 Beta-Gift-Takt @0x80031e44-ecc (-2 HP, Zaehler 0x78, HP < 0 -> 1) — die ersetzte Form | `80031e50 andi v0,a0,0x7` / `80031e60 lbu v1,0x800acc27` / `80031e78 ori v0,zero,0x78` / `80031ea4 addiu v0,v0,-2` / `80031ec0 ori v0,zero,0x1` | bestaetigt (Beta-Form; Ziel ist RE2, Beta -> Retail) |
| RE1.5 Pause-`bltz` @0x80031c78 auf 0x800aca40 | `80031c54 lw a0,-13760(a0)` / `80031c78 bltz a0,0x80031da8` | bestaetigt |
| RE1.5 Familie PL%02X: `andi v0,a0,0xf` @0x8003976c, Leser DAT_800aca5c @0x800314d4 | `8003976c andi v0,a0,0xf` / `80039770 beq v0,v1,...`; `800314d4 lbu v0,-13732(v0)`=0x800aca5c | bestaetigt |
| "Haut-Upload FUN_8004ee78 Slot 0x17 **@0x800316c8**" (re15_zustand_r39.h, zustand_r39_pc.c, Dossier 1.2/NB1) | **@0x800316c8 ist `jal 0x80037c1c` = der WUND-BUILDER** (Dossier 1.2 nennt dieselbe Adresse korrekt fuer den Builder). Die Haut-Upload-Aufrufe `jal 0x8004ee78` in FUN_800314b0 liegen bei **@0x800315b0** und **@0x80031814** (a0 aus Tabelle 0x801bd820 + s0) | Mechanismus bestaetigt (beide Aufrufe liegen in FUN_800314b0), **Adress-Zitat fuer den Upload falsch** -> Hinweis H6 (Zitat, keine Verhaltenskonstante) |

Guess-Tells im Code-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|approx|geraten|vermut`): **keine**.
Env-Schalter im neuen Code: nur die Messschiene `RE15_R39_GIFT_AT` / `RE15_R39_LOG` (zustand_r39.c env_init), kein
Verhaltensschalter. Verhaltenskonstanten im Diff und ihr Beleg: 30 (@0x80020138-40 / @0x8003c15c-64), Untergrenze 2
(@0x8003c174), -1 (@0x8003c17c-80), 100 (@0x8004ae14), Farbe ff/20/ff + Schritt 8/1/8 (@0x800ab178-7d), cond 3
(FUN_8004ed6c), Gift-Bit 0x2 (@0x800acaec / @0x8004af8c). Punkt 1 ist korrekt als NUTZER-VORGABE (PORT-WAHL) gekennzeichnet.

**Erklaert der Fix den gemessenen Befund (Absturz mit Weste)?** Ja, aus dem Code gelesen: die alte
`pc_player_skin_reload` (87180698, `git show 87180698:re15_port/platform/pc/main.c` Z. 1395-1413) rief `free(tb)` auf den
Rueckgabezeiger von `pc_read_pl_part`; dieser liefert fuer eine fehlende Teildatei `cb + off` = einen Zeiger IN den
resident gehaltenen Containerpuffer (main.c Z. 653-684, Kommentar "⛔ nicht freigeben"). PL01 liegt im ausgelieferten
Baum NUR als `PLD/PL01.PLD` (selbst gelistet: `shared_assets/PSX/PLD/` hat PL00.TIM und PL04.TIM lose, PL01 nur .PLD)
-> `free()` auf einen Innenzeiger = `STATUS_HEAP_CORRUPTION` (0xC0000374) bei der ersten Vollheilung mit Weste.
Die neue `re15_pc_zustand_r39_haut_neu` liest selbst (Teildatei, sonst `re15_pld_part` auf dem Container mit
Grenzpruefung `off + len <= sz`), `re15_render_pc_upload_tim_slot` kopiert in einen eigenen RGBA-Puffer (render_pc.c
2443ff), danach `free(puffer)` nur auf den gelesenen malloc-Puffer. Eigene Reproduktion: Abschnitt 3.

## 2. Vertrag (VERTRAG.md)
- Pfade (`git diff c71fa144 --name-only`): keine release/, platform/android/, shared_assets/PSX/, keine
  tests/{unit,integration}/CMakeLists.txt (Registrierung ueber probes/r39_zustand.cmake, GLOB). Keine Bank-9-Bits,
  keine Nachrichten-Ids, keine Assets, keine synchro/-Dateien.
- Gemeinsame Dateien: item_use_common.c (+2, Kommentar "Runde 39 Spur F"), re15_inv_screen.c (+2, F zugeteilt),
  game_step_common.c (+3/-4: eigener r38-Haken ersetzt), spinnen_r38.c (+4/-1, Spur-R-Datei der Vorrunde),
  **platform/pc/main.c +2/-0** (nur der Haken im Blut-Decal-Sync, Kommentar "Runde 39 Spur F") — **M1 der Abnahme 0 behoben**:
  die 19-Zeilen-Funktion liegt jetzt in `platform/pc/src/zustand_r39_pc.c` (eigene Datei, GLOB platform/pc/CMakeLists.txt).
- Tests je Punkt vorhanden und messend: unit_r39_zustand (A Kleidung / B Gift im Spielschritt / C EKG) +
  integration_r39_zustand_kleidung (6 Laeufe inkl. Weste/PL01-Container, Pixelmass + Log-Zeilen) +
  integration_r39_zustand_gift (hp-/ekg-Auswerter). Linux-/Parallel-Festigkeit (gelesen): `cmake_minimum_required` in
  beiden -P-Skripten, exe-Kopien mit Praefix r39fk/r39fg, eigene Laufordner, `m` fuer Nicht-WIN32, keine Backslash-Pfade,
  Umgebung ueber spiel_lauf.cmake gesetzt/geloescht.

## 3. Punkt 1 — volle Heilung = ganze Kleidung
Messschiene der Abnahme (Scratchpad `abn1_lauf.sh`, eigene Kopien `re15_pc_r39f_abn1.exe` / `test_r39_zustand_abn1.exe`,
Laufordner `scratchpad/laeufe/<name>/`): Spielstand per Werkzeug `karte` (Lage (-1024,0,-3336) rot 856, Cut 2, HP 20,
Platz 0 = Heilspray 0x22; Weste ueber Story-Flag 3:0x75), CONTINUE (RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0, RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1), Spray ueber den ECHTEN Statusschirm
(RE15_PAD_AT=200:S,260:A,300:A,340:A,500:S), Framedumps Bild 150 (vor) / 650 (nach) (RE15_FRAMEDUMP=150-650/500:fd_,
RE15_EXIT_AT=700#<raum>), RE15_STATE_LOG fuer HP-Verlauf und Pad-Zensus; je Figur einmal mit allen 8 Wund-Panels Stufe 2
(`_w`) und einmal ohne (`_o`); Mass = `test_r39_zustand bild` im Figurenkasten x177..247 / y81..205.
Pad-Zensus (alle Laeufe): 696 x `pad=0000`, 2 x `pad=0008` (START 200/500), 3 x `pad=8000` (SQUARE 260/300/340),
debug.log `[pad] kein Controller gefunden` — keine fremden Eingaben (die Messfalle der Abnahme 0 lag hier nicht vor).
HP-Verlauf (state.log, alle Laeufe): 375 Bilder hp=20, ab Bild 375 hp=100 (Weste: 105) bis zum Ende.

| Lauf | Renderer | Bild 150 w vs o | Bild 650 w vs o (nach dem Spray) | debug.log (w) |
|---|---|---|---|---|
| leon_w / leon_o (ROOM1130, PL00) | Software | 737 Pixel, Kasten (192,96)-(231,179), ausserhalb 0 | **0 Pixel** | `[r39-zustand] Heilung 0x22: HP 100 >= 100 voll -> Wund-Tabelle genullt (hatte Wunden), Spielerhaut neu` / `Spielerhaut PL00.TIM neu geladen (384x256, Teildatei PLD/PL00.TIM) - Kleidung ganz` |
| elza_w / elza_o (ROOM1131, Charakter 4, PL04) | Software | 462 Pixel, (193,110)-(223,180), ausserhalb 0 | **0 Pixel** | dito, `Spielerhaut PL04.TIM neu geladen (384x256, Teildatei PLD/PL04.TIM)` |
| weste_w / weste_o (ROOM1130, Flag 3:0x75 -> PL01) | Software | 1093 Pixel, (190,96)-(233,176), ausserhalb 0 | **0 Pixel** | `[pld] Spielermodell -> PLD/PL01.PLD (Mesh 17 Teile, Textur 384x256) [R.P.D.-Ruestung an]` / `Heilung 0x22: HP 105 >= 105 voll` / `Spielerhaut PL01.TIM neu geladen (384x256, Container PLD/PL01.PLD) - Kleidung ganz` / EXIT_AT 700, exit 0 |
| weste_gl_w / weste_gl_o (**GL-Renderer**, ohne RE15_SOFTWARE_RENDER) | `[render] SDL-Renderer direct3d, Videotreiber windows, ABR2 (B-F) ok` | 1102 Pixel, (191,97)-(233,176), ausserhalb 0 | **0 Pixel** | dito (Container PLD/PL01.PLD), exit 0 |
| teil_w / teil_o (**Gruenes Kraut 0x24**, Negativprobe) | Software | 737 Pixel | **806 Pixel** (Wunden bleiben), KEINE `[r39-zustand]`-Zeile, HP 20 -> 45 | Teilheilung stellt NICHT wieder her = "vollstaendig" der Vorgabe |

Gegenprobe im Spray-Lauf selbst (leon_w Bild 150 vs 650): 4526 Pixel verschieden (Pose + Kleidung) — nach dem Spray
ist die Figur pixelgleich mit der nie verwundeten Figur, vor dem Spray sind die Wunden sichtbar (Wund-Pfad der Runde 36
unveraendert). Sichtkontrolle (PNG-Zooms `laeufe/weste_gl_w/zoom_150.png` / `zoom_650.png`, `laeufe/teil_w/zoom_650.png`):
Bild 150 Weste und Hose voller Blut, Bild 650 saubere Weste/Uniform; nach dem Kraut bleibt das Blut.

**Absturz mit Weste (Nachbesserung 1) selbst reproduziert:** Lauf `weste_alt_w` auf der ALTEN exe
`re15_pc_r39f_nb1_vorher.exe` (sha256 db9df92a4c9101e8... = `re15_pc_r39f_stand3.exe` = Code 87180698, Kopie
`re15_pc_r39f_abn1_alt.exe`), gleiche Karte/Umgebung: **exit 127**, debug.log 96 Zeilen, Ende nach
`[walk] F360` / `Heilung 0x22: HP 105 >= 105 voll` / `[pl-part] PL01.TIM aus PLD/PL01.PLD geschnitten (+91012, 99872 B)` /
`[r39-zustand] Spielerhaut PL01.TIM neu geladen (384x256) - Kleidung ganz`, **kein `fd_000650.ppm`, kein EXIT_AT**,
state.log endet bei F375 (hp=105). Dieselbe Karte auf der neuen exe: exit 0, EXIT_AT 700, 0 Pixel (Tabelle oben).
Der Fix (nur der malloc-Puffer wird freigegeben, nie der Container-Schnitt) erklaert den Befund (Abschnitt 1).

Nebenwirkung (gemessen im Gift-Lauf g_cure, Abschnitt 4): Gegengift bei HP 91 -> 116 >= 100 zaehlt als volle Heilung
(`Heilung 0x23: HP 116 >= 100 voll -> Wund-Tabelle genullt, Spielerhaut neu`) — konsistent mit der Form "HP erreichen
den Wert des Heilsprays" (additive Heilung ueber die Schwelle, wie in Abnahme 0 festgestellt).

## 4. Punkt 2 — Gift zieht Energie wie RE2, EKG zeigt es
Messschiene: Leon ROOM1130 (Karte HP 100 bzw. 5, Inventar leer bzw. Gegengift), Gift per Messhaken RE15_R39_GIFT_AT=300
(Bit 0x2 + Zaehler 30 wie Op 32), RE15_R39_LOG=gift.log (je Spielbild hp/status/Zaehler/cond/menu/abzug), Statusschirm
per START (RE15_PAD_AT=1500:S), Framedumps 1540..1600/20, Auswertung `test_r39_zustand hp` / `ekg` (Kasten x26..103 /
y118..140 = die 32 EKG-Linien).

| Lauf | Ergebnis (selbst ausgewertet) |
|---|---|
| g100 (HP 100, Software) | `hp: 38 Takte, Summe 38, Abzug min 1 max 1, erster Takt F330, Abstaende 31: 37 von 37, HP-Minimum 62, Giftbilder 1200, letztes Bild F1499` (Takte F330 100->99, F361 99->98, F392 98->97 ... F1477 63->62; im Statusschirm ab F1500 kein Spielerschritt). EKG: Bild 1540 `hell=23 gruen=0 gelb=0 rot=0 magenta=23`, 1560 `hell=33 ... magenta=27 sonst=6` (dunkle Schwanzlinien), 1580 `hell=19 ... magenta=19`, 1600 `hell=5 ... magenta=5` |
| g100_gl (HP 100, **GL-Renderer** direct3d) | hp-Zeile identisch (38 Takte, F330, 37/37, Minimum 62, letztes Bild F1499); EKG-Zensus identisch (23/27/19/5 magenta, gruen = gelb = rot = 0) — der Software-Renderer maskiert hier nichts |
| g5 (HP 5, Untergrenze) | `hp: 4 Takte ... erster Takt F330, Abstaende 31: 3 von 3, HP-Minimum 1, letztes Bild F900`: F330 5->4, F361 4->3, F392 3->2, F423 2->1, danach bis F900 kein Abzug (`slti v0,v0,0x2 / bne` @0x8003c174-78) |
| g_cure (HP 100, Gegengift 0x23 Platz 0, Statusschirm F600-F913, USE) | 9 Takte bis F578 (HP 91), im Statusschirm keine gift.log-Zeile (kein Spielerschritt); F914: `hp=116 status=0x0000 ... cond=0` (Bit geloescht @0x8004af8c, +25 @0x8004af78 ohne Kappung), bis F1000 kein weiterer Abzug; EKG-Zustand danach cond 0 (FINE) |
| **Echter Weg** (Giftklumpen der Spinne, integration_r38_spinnen_echtlauf aus meinem Modul-ctest-Lauf, Logs `build/tests/integration/r38_spinnen_wd/lauf_cb42f591_E/spinnen.log` und `_L/`) | selbst ausgezaehlt (erster hp=/gz=-Token je Zeile): Spuck-Treffer F19 100->90, danach RE2-Takte (gz 0 -> 30 mit HP -1) ab **F50** (= 31 Bilder nach dem Treffer, Op 32 im ESP-Takt NACH dem Spielerschritt), F81, F112, F143 ... — **27 Takte je Szenario, 25 Perioden = 31, eine 62** (Takt fiel bei F484 72->70 mit einem fremden -1 zusammen); fremde Abzuege (Spucken) F189/243/388/460/484/814/845; **E und L bytegleich** |

Beobachtung zur Phase: im Messhaken-Lauf kommt der erste Takt 30 Bilder nach dem Setzen (der Haken setzt 30 im selben
Spielerschritt VOR der Abfrage: Bild 300 `z2=29`), am echten Spinnentreffer 31 Bilder (Zaehler im ESP-Takt nach dem
Spielerschritt gesetzt) — beides die Folge `lbu / addiu -1 / sb / bne` @0x8003c144-58 mit Zaehler 30 (@0x80020138-40).
Sichtkontrolle (`laeufe/g100_gl/ekg_zoom_1560.png`, `fd_001560.png`): magenta EKG-Spur mit hellem Kopf und abgedunkeltem
Schwanz, Wort "Poison" in Magenta — die RE2-POISON-Farbe @0x800ab178 (im GL-Renderer gesehen).

Alle Zahlen des Dossiers 2.4 (38 Takte, F330, 37/37, magenta 23/27/19, Untergrenze 1) unabhaengig reproduziert;
GL-Renderer, Gegengift und der echte Spinnenweg sind zusaetzliche Proben der Abnahme.

## 5. Tests
- `ctest --test-dir re15_port/build -R "r39_zustand" --timeout 900 --output-on-failure` (Abnahme, 13:24-13:29):
  **`100% tests passed, 0 tests failed out of 3`** — unit_r39_zustand 0.02 s, integration_r39_zustand_kleidung 177.13 s
  (6 Laeufe inkl. Weste/PL01-Container), integration_r39_zustand_gift 98.69 s; Total 275.87 s (Scratchpad
  abn1_ctest_r39_zustand.log).
- Modulgruppe `-R "r38_spinnen|r36_kleidung|unit_item_use|unit_inv_fsm|unit_wound_accumulator|r39_inventar_status|unit_inv_grant|unit_em_status_persist|unit_r36_blut_pin|unit_scd_zone2_pauseflags|r38_zopf|weste|ruestung|unit_r26_mg_blut|unit_cam_selfheal"`
  (Abnahme, parallel zu den Messlaeufen): **`100% tests passed, 0 tests failed out of 41`**, Total 421.21 s — darunter
  integration_r38_spinnen_echtlauf 113.48 s (RE2-Auswerter gz 0 -> 30 / HP -1 / Periode 31), integration_r38_zopf 118.42 s,
  integration_r36_kleidung_bild/spielweg/hundeweg, integration_weste_load_pin, integration_r39_inventar_status,
  unit_r38_spinnen (B/B3/B4/B5 in RE2-Form), unit_item_use, unit_inv_fsm, unit_wound_accumulator, unit_weste_effekt,
  unit_1190_weste/ruestung (Scratchpad abn1_ctest_module.log). Keine Wiederholung noetig, kein Flattern.
- Tests je Punkt messend: Punkt 1 = unit_r39_zustand A (7 Pruefungen) + integration_r39_zustand_kleidung (Pixelmass 150/650,
  Log-Zeilen, Westen-Containerpfad); Punkt 2 = unit_r39_zustand B/C + integration_r39_zustand_gift (hp-/ekg-Auswerter) +
  der umgestellte pruef_log der integration_r38_spinnen_echtlauf.

## 6. Maengel / Hinweise
**Maengel: keine.** M1 der Abnahme 0 ist behoben (main.c +2/-0, Funktion in platform/pc/src/zustand_r39_pc.c), H1 (Messreste
versioniert, Baum leer) und H2 (Mnemonic `ori s0,zero,0x64`) sind erledigt; der dabei gefundene Westen-Absturz ist
behoben, selbst reproduziert (alt: Heap-Abbruch, neu: exit 0 / 0 Pixel) und per Riegel gesichert.

**H6 (Hinweis, Zitat — kein Verhaltenswert).** `include/re15_zustand_r39.h` Z. 8-9, `engine/src/zustand_r39.c` Z. 21-23,
`platform/pc/src/zustand_r39_pc.c` Z. 9 und Dossier "Nachbesserung 1 / Beleg" (Z. 297-298) zitieren den Haut-Upload als
"FUN_8004ee78 Slot 0x17 @0x800316c8". Selbst
disassembliert: @0x800316c8 ist `jal 0x80037c1c` (der Wund-Builder — Dossier 1.2 nennt dieselbe Adresse dort korrekt);
die beiden Aufrufe `jal 0x8004ee78` in FUN_800314b0 liegen bei **@0x800315b0** und **@0x80031814** (`lw a0,-10208(at)` =
Tabelle 0x801bd820 + s0, Delay-Slot `addu a0,a0,s0`). Der Mechanismus (Spieler-Laden = Builder + Haut-Upload) stimmt, nur
die Adresse hinter "Haut-Upload" ist die des Builders. Behebung bei der Integration: Kommentar an den drei Stellen auf
@0x800315b0/@0x80031814 (Upload) bzw. @0x800316c8/@0x800318cc (Builder) trennen. Kein Code-Verhalten betroffen.

**H3 / H4 / H5 (unveraendert aus Abnahme 0, kein Mangel).** H3 EKG-Wellenform unter Gift = RE1.5-Welle (Tabelle 3, bytegleich
mit 1/2/4/5) in RE2-Farbe; die RE2-POISON-Welle @0x800aad58 (80 Spalten, Hub 4..36) passt nicht ohne erfundene Skalierung in
den RE1.5-Monitor (77 Spalten, Hub 39..55) -> Entscheidungsfrage an den Nutzer (Dossier 5), byte-true-konform offen gelassen.
H4 PSX-Backend pollt den Haut-Neuaufbau nicht (PSX-Build-Luecke, hier nicht baubar). H5 Takt-Zaehler ohne Produktions-Reset —
RE2 kennt keinen Schreiber (Op 32 @0x80020140, Takt @0x8003c154/64, Poison_clr), ohne Folge.

**H7 (Hinweis, nicht Teil des Diffs).** Dass der Gift-Takt im Statusschirm steht, folgt im Port aus dem vorhandenen
`re15_menu_gameplay_frozen -> return` vor dem Spieler-Dispatcher (game_step_common.c:1343); fuer RE2 ist das Inventar-Verhalten
des Spielertasks nicht separat disassembliert (Dossier 2.4 sagt das offen). Unveraendertes Port-Verhalten, gemessen (g100: kein
Takt ab F1500; g_cure: Zaehler steht im Menue), kein Mangel dieser Spur.

## 7. Urteil
| Nutzer-Punkt | Urteil | Kern-Beleg (Abnahme 1, selbst gemessen) |
|---|---|---|
| 1. Volle Heilung = ganze Kleidung | **erfuellt** | Nach dem Heilspray (HP -> 100 / 105 mit Weste) 0 Restpixel w vs o: Leon 737 -> 0, Elza 462 -> 0, Leon mit R.P.D.-Weste 1093 -> 0 (Software) und 1102 -> 0 (GL-Renderer); Teilheilung (Gruenes Kraut, HP 45) laesst 806 Pixel Wunden stehen (= "vollstaendig"). Westen-Absturz der Vorfassung reproduziert (alt exit 127 ohne Bild 650) und auf der neuen exe weg. NUTZER-VORGABE, Form byte-true (Spieler-Lade-Pfad FUN_800314b0: Builder @0x800316c8, Haut-Upload @0x800315b0; s0 = 100 @0x8004ae14). Visuell bestaetigt (GL). |
| 2. Gift zieht Energie wie RE2, EKG zeigt es | **erfuellt** | -1 HP alle 31 Bilder (38 Takte, erster F330, 37/37 Abstaende 31; HP-Untergrenze 1; Gegengift beendet; kein Takt im Statusschirm), identisch in Software- und GL-Renderer; echter Spinnenweg: Treffer F19, Takte ab F50 alle 31 in beiden Szenarien. Byte-true RE2 FUN_8003bfac @0x8003c12c-180 + Op 32 @0x80020138-40 (selbst disassembliert). EKG unter Gift magenta (ff 20 ff / 08 01 08 @0x800ab178) mit "Poison", gruen = gelb = rot = 0; nach dem Gegengift FINE. Visuell bestaetigt (GL). |

**Gates:** RE-Gate haelt (13 Stellen selbst aus beiden PSX.EXE disassembliert; alle Verhaltenskonstanten @0x... oder als
NUTZER-VORGABE/PORT-WAHL gekennzeichnet; keine Guess-Tells; nur Mess-Env; der Fix erklaert den gemessenen Absturz).
Vertrag haelt (M1 behoben: main.c +2 Zeilen Haken; eigene PC-Datei; keine verbotenen Pfade; keine Bank-9-Bits/Ids/Assets).
Tests je Punkt messend und gruen (3/3 Spur, 41/41 Modulgruppe, kein Flattern). Nebenwirkungen: beide Figuren (Kleidung), beide
Szenarien (Spinnengift E/L), beide Renderer, Teilheilung/Gegengift/Untergrenze geprueft.

**bestanden = true.** Offen bleiben nur Hinweise (H6 Zitat-Adresse des Haut-Uploads; H3 Entscheidungsfrage Wellenform an den
Nutzer; H4 PSX; H5/H7 ohne Folge). Integrationshinweis: der 2-Zeilen-Haken in main.c (Blut-Decal-Sync ~Z. 9439) gegen den
inzwischen vorausgelaufenen master (Spur A Nachbesserung 1, 6926409c) mergen — A/B liegen in der Basis c71fa144, F haengt an
anderen Code-Stellen, Risiko gering.
