# Abnahme 0 — Spur P masken36 (Runde 37, 2026-10-13)

Unabhaengige Abnahme. Baum `.claude/worktrees/r37_masken36` (Zweig r37/masken36, Basis bddd6b31, HEAD 7495dc73).
Gemessen wurde nur am selbst gebauten Stand (`local_build.sh configure` + `build` -> "ninja: no work to do",
`LOCAL-BUILD-OK (build)`) und mit eigenen Werkzeugen im Scratchpad (`abn/abn_masken.c` = Kopie des Riegels
test_r37_masken36.c mit Zusatz-Modi `liste`, `dbg`, `frei`, `proj`; `abn/lauf.sh` Messlauf mit eigener exe-Kopie
`re15_pc_abnP0_*.exe`). Nichts davon liegt im Baum.

## Stand der Abnahme (fortlaufend)

- [x] Pflichtlektuere: AUFTRAG.md (Orchestrator-Auftrag P), VERTRAG.md (§0 P, §1.4, §1.6), Dossier P_masken36.md, Code-Diff
- [x] Bau
- [x] gezielte Tests: 30/30 gruen (§5)
- [x] Messung an der exe
- [x] RE-Gate (5 zitierte Stellen selbst disassembliert, §2)
- [x] Vertrag (statisch)
- [x] Urteil (§6), Maengel (§7)

## 1. Code-Diff (gelesen)

- `platform/pc/src/bg_pc.c` re15_pri_load_cut_atlas_ex: im R15M-Zweig wird der eigene Atlas `MASKS/ROOM%04X_PRI%02d.TIM`
  gesucht; liegt er vor -> unveraendert (publish + return). Fehlt er -> Logzeile `Port-Masken ohne eigenen Atlas ->
  BSS-Atlas des Cuts (Runde 37 Spur P)` und weiter mit Weg 1 (SLD-Auszug) / Weg 2 (BSS/ROOM%04X/PRI%02d.TIM). Weg 3
  (MASKS-Atlas) nur noch fuer Original-Sektionen (`!nachgezeichnet`) — fuer sie identisch zu vorher.
  Folge: Stage-1/2-Sektionen mit eigenem Atlas und Original-Sektionen laufen durch denselben Code wie vorher.
- `include/re15_masken36_r37.h`: `re15_pri_atlas_quelle(nachgezeichnet, eigener_atlas_da)` (inline, eigen nur wenn beides).
- `engine/src/masken36_r37.c` + `gen/masken36_ausschluss_r37.inc` (85 Zeilen + Endzeile): je (Masken-Raum, Cut) ein
  Bitfeld; `re15_masken36_filter` entfernt die markierten Rechtecke (Reihenfolge bleibt, draw_count folgt).
- `platform/pc/main.c` (2 Zeilen, Kommentar "Runde 37 Spur P"): Filter im R15M-Zweig nach `re15_pri_parse_section`,
  Schluessel `re15_pri_masken_raum(g_current_room_id)` (Zwilling der Spur A). `platform/psx/main.c` (2 Zeilen, Gleichlauf;
  `re15_pri_masken_raum` ist dort ueber `re15_pri1100_r37.h` Zeile 37 deklariert).
- Tabelle enthaelt nur Raeume der Stages 3/5/6 (grep: 3040/41, 3050/51, 30A0, 30E0/E1, 5020/21, 5050/51, 5090/91, 50B0/B1,
  5100/01, 6000/01, 6010/11, 6030/31) — kein Stage-1/2-Raum. **Auffaellig: 30A0 hat 3 Zeilen (Cut 0/1/3: 25+21+14
  Rechtecke), 30A1 keine** — s. §4.4.
- Cherry-pick eab5d03c: `git patch-id` von be2dbd26 (r37/integration) und eab5d03c identisch
  (399867011ddc22aabd63a6a22165b446075227fc) -> die MASKS-Loeschungen (ROOM1221.MSK, ROOM2091.MSK + _PRI00..07.TIM) sind
  inhaltsgleich der Integrations-Commit, keine Arbeit der Spur P.

## 2. RE-Gate

Selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py dis`, `info/Re1.5/PSX.EXE`), Namen der
Bibliotheksaufrufe aus `ghidra1_V2.txt`:

| zitiert | gefunden | haelt |
|---|---|---|
| @0x80021dec jal 0x8004ee78, @0x80021de8 ori v0,zero,0x15 | `80021de8: ori v0,zero,0x15`, `80021dec: jal 0x8004ee78`, `80021df0: sh v0,0(s0)` (s0 = 0x800aca4c @0x80021de0) | ja |
| @0x8004eea8 sll a0,v1,6 / @0x8004eeb8 addiu a0,a0,-1024 / @0x8004eed0 sll v0,v0,8 / @0x8004eee0 LoadImage | `8004eea0 lbu v1,0(s1)` (0x800aca4c = 0x15), `8004eea8 sll a0,v1,6` (1344), `8004eeb0 bne` (<16), `8004eeb8 addiu a0,a0,-1024` (320), `8004eebc sh a0,0(v0)`, `8004eed0 sll v0,v0,8` (256), `8004eed4 sh v0,2(v1)`, `8004eee0 jal 0x80068c88`; ghidra1_V2.txt: `22 a3 01 0c jal LoadImage` (= 0x80068c88), `jal 0x8006bbbc` = OpenTIM | ja |
| u/v @0x80039408/@0x80039418 -> @0x800394dc/e0 | `80039408 lbu s7,0(a2)`, `80039418 lbu fp,-9(s2)` mit `800393d4 addiu s2,a2,10` (-> Byte 1), `800394dc sb s7,-2(s0)`, `800394e0 sb fp,-1(s0)` | ja |
| CLUT 0x7800 @0x80039498 | `80039498 ori v0,zero,0x7800`, `800394fc sh v0,0(s1)`, `80039500 sh v0,0(s0)` | ja |
| tpage 0x95 @0x80039630 | `80039630 ori a3,zero,0x95`, `80039634 jal 0x80069858` (ghidra: `16 a6 01 0c jal SetDrawMode`); 0x95 = 8 bpp, x 5*64 = 320, y 256 | ja |

Der Mechanismus "eine Maske tastet den BSS-Atlas des Cuts bei VRAM (320,256) ab" ist damit belegt. Die uebrigen Werte im
Diff: Spielerradius 450 (`re15_collision.c:39 #define PR 450`, Kopf "DAT_80073e94[6], code-verified"), Band 0x708, das
Verdeckungs-Urteil `re15_pri_mask_occludes` (bestehender Code) — Messaufbau, keine neuen Spielkonstanten. FIGUR_HOCH/HALB
sind als Messung gekennzeichnet. Der Ausschluss selbst ist Port-Massnahme (die R15M-Masken sind NUTZER-VORGABE; das Ziel
"Figuren VOR einem Objekt werden nicht verdeckt" steht im Auftrag) — im Kommentar begruendet, nicht als Original verkauft.
Suche im Diff nach deferred/tunable/interim/for now/faithful/plausibel/TODO: 0 Treffer.

Erklaert der Fix den Befund? Statisch: `git show 53741fe5:.../bg_pc.c` hat `if (!buf) { set_pri_atlas(NULL); return 0; }`
im R15M-Zweig, `b721702f` (Vorgaenger) hat den Zweig nicht, `v0.8.21` hat ihn (Zeile 189). Genau diese Bedingung trifft jede
R15M-Sektion ohne eigenen Atlas — der Fix setzt an genau dieser Stelle an.

## 3. Vertrag (statisch)

- Pfade (`git diff bddd6b31 --name-only`): analysis/ (Dossier), engine/src/masken36_r37.c + gen/masken36_ausschluss_r37.inc
  (neu), include/re15_masken36_r37.h (neu), platform/pc/src/bg_pc.c (Lader — VERTRAG §1.4 erlaubt P genau das),
  platform/pc/main.c (2 Zeilen) + platform/psx/main.c (2 Zeilen) mit Kommentar, tests/unit/probes/r37_masken36.cmake,
  tests/integration/test_r37_masken36_verdeckung.cmake (`cmake_minimum_required(VERSION 3.16)`), tests/unit/*r37_masken36*;
  dazu die inhaltsgleiche Uebernahme von be2dbd26 (MASKS-Loeschungen, pri_masken_zwilling_r37.inc, test_r37_pri1100.c,
  zwilling_zensus.py). Keine release/, platform/android/, keine Test-CMakeLists, keine neuen/umgeschriebenen MASKS-Dateien
  durch P.
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Ereignisse, keine Slots.
- Kein popen in den neuen Tests (Linux-Portabilitaet ok).

## 4. Messung an der exe (Abschnitte 4.2-4.4 entfallen; Nummern wie beim Messen vergeben)

Alle Laeufe: eigene exe-Kopie `re15_pc_abnP0_<quelle>.exe` neben re15_pc.exe, Arbeitsordner Scratchpad `abnP0/runs/<lauf>`,
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
RE15_FRAMEDUMP=<bild>:f.ppm RE15_EXIT_AT=<bild>#<raum>`, Elza mit `RE15_TITLE_SHOT_CHAR=1`. Messwerkzeug = das der Spur
(`probe_r37_masken36_ppm`, S = Figurpunkte |B-C| > 24, H = davon uebermalt |A-B| > 24), A = mit Masken, B = `RE15_NO_PRI=1`,
C = Figur ausserhalb des Regionsvierecks (Akteur-Cull), Fenster aus der Projektion der Figur (`abn_masken proj`).
exe: P = `re15_pc.exe` (HEAD), vorher = `re15_pc_p37vorher.exe` (Basis bddd6b31: enthaelt die Spur-A-Zeichenkette
`k_r37a_zwilling`, nicht die Spur-P-Logzeile), v0.8.24 = `release/pkg-win/re15_port_v0.8.24/re15_pc.exe` (Kopie, Assets
des Spurbaums).

### 4.1 Regression an der exe (Lader)
`RE15_DEBUG_JUMP=3040@250`, Bild 60, Logzeile nach `loaded room3040.rdt`:

| exe | Leon ROOM3040 Cut 0 | Elza ROOM3041 Cut 0 |
|---|---|---|
| v0.8.24 (Release) | `[pri] cut=0 pri_offset=0x784 masks=19 fg_atlas=0` | `masks=19 fg_atlas=0` |
| P | `[pri] ROOM3040 Cut 0: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)` + `masks=12 fg_atlas=1` | dito ROOM3041, `masks=12 fg_atlas=1` |

Statisch (§2): 53741fe5 fuehrt `return 0` ohne eigenen Atlas ein, v0.8.21 traegt es. Befund und Ursache bestaetigt, der
Lader zeichnet jetzt mit dem BSS-Atlas (19 - 7 ausgeschlossene = 12).

### 4.5 ⛔ Elza ROOM30A1: Figur VOR dem Objekt wird uebermalt (Mangel M1)
Die Tabelle hat fuer ROOM30A0 Cut 0/1/3 Zeilen (25/21/14 Rechtecke), fuer ROOM30A1 KEINE — obwohl ROOM30A1.RDT und
ROOM30A1.MSK bytegleich zu ROOM30A0 sind (`cmp` ohne Ausgabe; MSK laut Dossier §1.1 15 Paare bytegleich). Ursache im
Generator (`abn_masken liste`/`dbg`, Kopie des Riegels): ROOM30A1 hat 10 Saaten, alle in soliden Zellen
(`frei 0`); die einzige Saat, die in ROOM30A0 die Flutfuellung traegt, ist die Tuer-Ankunft (-25500,11700) aus ROOM3090
(`TUER ROOM3090 -> 30A0`), und ROOM3091 setzt nach main00+sub00 keine Tuer nach 30A1 -> **ROOM30A1: 0 Stehplaetze in
allen 6 Cuts** ("ohne Stehplatz", von den 32 des Dossiers) -> kein Ausschluss. ROOM30A1 ist Elzas Raum (Sewer Control
Room; re15_room_spawns.h:143 Inbound-Tuer (-25500,11700), Itemkiste re15_itembox.c:65, Speicherpunkt re15_savepoint.c:49,
Kartenzone re15_map_zones.h:361).

Modell (`abn_masken frei 30A0 <msk> 0 T`, Erreichbarkeit aus ROOM30A0, Masken mit Tabelle): MSK 30A0 -> 21 Masken, 0
Fehlverdeckungen; MSK 30A1 -> 46 Masken, z.B. (-24443,13463) 58 von 81 Figurpunkten ohne Zelle davor verdeckt.

An der exe, ROOM30A0/30A1 Cut 0 (`RE15_DEBUG_JUMP=30A0@250 RE15_PLAYER_POS=-24443,13463,0 RE15_FORCE_CUT=0`, Bild 120,
C = (-19800,19763), Fenster 167 75 195 153):

| Lauf | Log | S | H (uebermalt) |
|---|---|---|---|
| Leon P (ROOM30A0) | `ROOM30A0 Cut 0: Port-Masken ohne eigenen Atlas -> BSS-Atlas`, `masks=21 fg_atlas=1` | 793 | **0** |
| Elza P (ROOM30A1) | `ROOM30A1 Cut 0: Port-Masken ohne eigenen Atlas -> BSS-Atlas`, `masks=46 fg_atlas=1` | 612 | **342 (56 %)** |
| Elza vorher (bddd6b31) | (fg_atlas=0) | 612 | 0 — Bild bitgleich zu Elza P mit `RE15_NO_PRI=1` (cmp) |

Bild: Scratchpad `abnP0/e1_montage.png` (Leon A | Leon B | Elza A | Elza B, 2-fach): Leon steht links vor der
Buero-Trennwand und bleibt ganz sichtbar; Elza an derselben Stelle verliert Oberkoerper und Kopf an die Trennwand, nur die
Beine bleiben — sie steht VOR der Wand (Bild B). Das ist genau die Fehlverdeckung, die der Ausschluss fuer Leon entfernt,
und die Fehlerklasse "Leon verschwindet" vom 2026-09-03. Vor P war Elza dort unverdeckt (keine Masken). **Spur P fuehrt
damit fuer Elza in ROOM30A1 eine neue Fehlverdeckung ein**; der Wortlaut "Figuren VOR einem Objekt werden nicht verdeckt"
ist fuer Elza in ROOM30A1 Cut 0 (und nach Modell Cut 1: 21, Cut 3: 14 weitere Rechtecke) verletzt. Die Dossier-Aussage
§4.2 "Leon und Elza verhalten sich gleich" stimmt fuer ROOM30A1 nicht; der Riegel prueft Elza nur in 3051/4041/5021/6011.

**Echter Weg (gelaufen, kein Teleport an die Stelle):** Elza/Leon an der Tuer-Ankunft aus ROOM3090/3091 (-25500,11700)
(re15_room_spawns.h:143), `RE15_PLAYER_POS=-25500,11700,3425 RE15_INPUT_SCRIPT=U1.07 RE15_INPUT_SCRIPT_START=420`, ohne
FORCE_CUT, Bild 240: beide stehen bei `PL(-24284,13657)` mit `cam=0` (Kamera schaltet von Cut 1 selbst auf Cut 0).
Leon `masks=21 fg_atlas=1` -> S=602, **H=0**; Elza `masks=46 fg_atlas=1` -> S=464, **H=265 (57 %)**
(Fenster 173 75 201 154, C = e1_*_C). Bild `abnP0/w2_montage.png`: von Elza bleibt nur ein Stiefel unter der Trennwand.

### 4.6 Elza ROOM5021 Cut 6/12: dieselbe Modell-Luecke, an der exe NICHT als echter Fehler belegt (Hinweis)
ROOM5021.RDT weicht von ROOM5020 ab (gleich viele SCA-Zellen 205, Zonen 42); in 5021 fehlt die Saat (-1800,-14050) der
Leon-Variante, die in 5020 den Bereich von Cut 4/5/6/12 flutet -> 5021 Cut 4/5/6/12 "ohne Stehplatz", kein Ausschluss
(5020 Cut 6: 33 von 55 Rechtecken aus, 5021: 0; Cut 12: 1 bzw. 0). Mit Zwangs-Cut (`RE15_FORCE_CUT=6`, Lage
(-3933,-16525)) uebermalen die Masken Elza (S=837, H=457) und Leon nicht (S=1007, H=0), Bild `abnP0/e2_montage.png`.
ABER: gelaufen (von Sueden bzw. von Osten) zeigt die Kamera an dieser Stelle Cut 0 bzw. Cut 12, nicht Cut 6; an der
gelaufenen Stelle (-943,-14782) Cut 12 ist Elza unverdeckt (S=3149, H=0). Ein natuerlicher Cut-6-Fehlfall wurde nicht
gefunden -> kein Mangel, nur Hinweis auf dieselbe Ursache wie M1 (Saaten je Raumvariante).

### 4.7 Wiederverdeckung HINTER / DAVOR an Szenen ausserhalb der Riegel (Leon, Elza)
Zwangs-Cut, Bild 120; V = vorher-exe (bddd6b31) an derselben Lage.

| Szene | Raum Cut Lage, C, Fenster | P: S / H | vorher: H | Log P / vorher |
|---|---|---|---|---|
| HINTER Leon | 30A0 C3 (-19800,19763), C (-24443,13463), 17 37 74 148 | 1539 / **724 (47 %)** | 0 | `masks=42 fg_atlas=1` / `masks=56 fg_atlas=0` |
| HINTER Elza | 30A1 C3 (-19800,19763), dito | 1180 / **747 (63 %)** | 0 | `masks=56 fg_atlas=1` / `fg_atlas=0` |
| HINTER Leon | 6010 C13 (-18812,-20920), C (-18612,-5000), 103 84 132 174 | 1250 / **903 (72 %)** | 0 | `masks=43 fg_atlas=1` / `fg_atlas=0` |
| DAVOR Leon | 30A0 C4 (-21843,17763), C (-24443,13463), 178 41 286 240 | 8210 / **0** | 0 | `masks=14 fg_atlas=1` / `fg_atlas=0` |

Bild `abnP0/h_montage.png` (30A0 C3 A | B, 6010 C13 A | B): Leon hinter dem Schreibtisch-Stapel bis zur Brust verdeckt,
Kopf/Schultern frei; hinter dem Treppenpfeiler bis auf den Rand verdeckt. Wiederverdeckung an der exe bestaetigt.

### 4.8 Stage 1/2 (eigener Atlas) und Spur-A-Zwilling unveraendert
P-exe gegen vorher-exe (bddd6b31), gleicher Lauf, Framedump Bild 120 per `cmp`:

| Lauf | Framedump | Log (beide) | Spur-P-Zeile |
|---|---|---|---|
| Leon ROOM1100 C1 (-14914,-25645) rot 4488 (F9-Marke Spur A) | **bitgleich** | `cut=1 pri_offset=0x60C masks=52 fg_atlas=1` | 0 |
| Leon ROOM1220 C0 (eigener + BSS-Atlas) | **bitgleich** | `cut=0 pri_offset=0x4DC masks=20 fg_atlas=1` | 0 |
| Leon ROOM2090 Sprung-Spawn C0 | **bitgleich** | `cut=0 pri_offset=0x670 masks=56 fg_atlas=1` | 0 |
| Elza ROOM1101 C1 (Zwilling -> 1100) | **bitgleich** | `cut=1 ... masks=52 fg_atlas=1` | 0 |

### 4.9 Echter Weg durch die Tuer (Maskensatz beim Raumeintritt)
`RE15_DEBUG_JUMP=3040@250 RE15_PLAYER_POS=-26925,-4975,2048 RE15_PRESS=square@60,square@62` (Tuer-AOT ROOM3040 -> 3050,
Slot 1; rot 0/1024/3072 loesen nicht aus, 2048 schon): Leon `loaded room3050.rdt`, danach
`[pri] ROOM3050 Cut 0: Port-Masken ohne eigenen Atlas -> BSS-Atlas` + `cut=0 pri_offset=0x810 masks=25 fg_atlas=1`;
Elza (`RE15_TITLE_SHOT_CHAR=1`) `loaded room3041.rdt` -> `loaded room3051.rdt`, dieselben zwei Zeilen fuer ROOM3051.

## 5. Tests

`ctest --test-dir re15_port/build -R "r37_masken36|r37_pri1100|pri_masken|r36_pri|pri_hashes|unit_pri_|sld_atlas|maskgrp|boot_bg_pin|unit_r35_entladen" --timeout 900 --output-on-failure`
-> **100% tests passed, 0 tests failed out of 30** (darunter unit_r37_masken36_zensus/ausschluss/verdeckung,
integration_r37_masken36_stage3..6 208.9/153.2/128.5/135.8 s, unit/integration_r37_pri1100_*, integration_pri_masken,
integration_boot_bg_pin, unit_sld_atlas, unit_r34n_g_maskgrp, unit_r35_entladen_*). Die Integrationswerte aus LastTest.log
sind die des Dossiers (z.B. Stage 3 HINTER 1123/1234, DAVOR 0/684, Elza 1158/1260 und 0/1067, FREI 0/883 und 0/997).
`test_r37_masken36 zensus` direkt: 52 Dateien, 307 Sektionen, eigen 91, BSS 216, tot 0, Blit passt 307, BSS-Mittel max
23.05, alte Regel 216 tot, Gegenprobe ROOM1220 55.87..85.32. `ausschluss` direkt: Saaten 1979, 216 Sektionen, 6478
Masken, 944 aus, 0 Abweichungen, **32 ohne Stehplatz**, 85 Zeilen.

Riegel-Luecke: der Check "mit der Tabelle verdeckt keine Generator-Maske eine Figur ohne Zelle davor" ist fuer Sektionen
ohne Stehplatz leer wahr; ROOM30A1 (alle 6 Cuts ohne Stehplatz) faellt so durch. Kein Riegel prueft Elza in ROOM30A1 oder
die Gleichheit der Tabellenzeilen bytegleicher Raumpaare (30A0/30A1: 3 Zeilen gegen 0).

## 6. Urteil je Punkt (gegen den Wortlaut)

| # | Punkt | Urteil | Beleg |
|---|---|---|---|
| 1a | Masken der Stages 3..6 verdecken die Figuren wieder (BSS-Atlas) | erfuellt | §4.1 (v0.8.24 `fg_atlas=0` -> P `fg_atlas=1`), §4.7 (HINTER 47/63/72 %, vorher 0), §4.9 echte Tuer, RE-Gate §2, Zensus/Blit §5 |
| 1b | ohne die Raeume mit eigenem Atlas (Stage 1/2, Spur A) zu veraendern | erfuellt | §4.8: 4 Laeufe bitgleich zur vorher-exe; Code-Pfad fuer eigenen Atlas unveraendert (§1) |
| 1c | Figuren VOR einem Objekt werden nicht verdeckt | **nicht erfuellt fuer Elza ROOM30A1** | §4.5: gelaufen, Kamera schaltet selbst auf Cut 0, Elza 265 von 464 Figurpunkten uebermalt (vorher 0), Leon an derselben Stelle 0. Leon-Szenen: DAVOR 0 (§4.7, Riegel); bekannte Saumpunkte 3040 C1 / 5090 C14 (Dossier OFFEN 2) |
| 1 | Punkt 1 gesamt | **teilweise** | 1a + 1b erfuellt, 1c fuer Elza in ROOM30A1 verletzt — NEU durch P (vorher keine Masken) |
| 2 | Bezug: Verdeckung in Stages 3..6 fehlte unbemerkt | erfuellt (inhaltlich), Versionsangabe falsch | Ausfall an der Release-exe v0.8.24 gemessen (§4.1). `git tag --contains 53741fe5` -> erstes Release **v0.7.19** (2026-09-08 15:19), nicht v0.6.x; die Masken kamen mit v0.3.96 (`--contains b8db7b2b`). Dossier §7 "Fuer den Nutzer" schreibt "Seit v0.6.x (2026-09-08)" (M3) |

Gates: RE-Gate haelt (§2: 5 zitierte Stellen selbst disassembliert, alle stimmen; keine Rate-Woerter). Vertrag haelt (§3).
Tests 30/30 gruen (§5), aber mit Riegel-Luecke fuer M1.

## 7. Maengel

**M1 (Punkt 1c, Elza): Generator-Fehlverdecker in ROOM30A1 nicht ausgeschlossen — Elza wird VOR der Buero-Trennwand
geloescht.** ROOM30A1.RDT/.MSK sind bytegleich zu ROOM30A0; die Tabelle `gen/masken36_ausschluss_r37.inc` schliesst in 30A0
Cut 0/1/3 25+21+14 Rechtecke aus, in 30A1 keines, weil das Erreichbarkeitsmodell fuer 30A1 keine Saat findet (die
Tuer-Ankunft (-25500,11700) kommt nur aus ROOM3090 -> 30A0; ROOM3091 setzt nach main00+sub00 keine Tuer nach 30A1; alle 10
AOT-Saaten liegen in soliden Zellen). Nachpruefbar: `RE15_TITLE_SHOT_CHAR=1 RE15_DEBUG_JUMP=30A0@250
RE15_PLAYER_POS=-25500,11700,3425 RE15_INPUT_SCRIPT=U1.07 RE15_INPUT_SCRIPT_START=420`, Bild 240 -> `PL(-24284,13657)
cam=0`, `masks=46` (Leon 21), H=265/464 (Leon 0/602); Zwangs-Cut-Variante (-24443,13463) H=342/612. Vor P war die Stelle
unverdeckt. Weg (Vorschlag, nicht gebaut): fuer bytegleiche Raumpaare die Zeilen des Leon-Raums verwenden bzw. die Saaten
je Raumpaar vereinigen (Tuer-Ankuenfte beider Varianten, re15_room_spawns.h), Tabelle neu erzeugen; Riegel: Elza-Szene
ROOM30A1 Cut 0 an der exe (H == 0) und "bytegleiche Paare haben gleiche Tabellenzeilen".

**M2 (Riegel/Dossier): `unit_r37_masken36_ausschluss` prueft Sektionen ohne Stehplatz nicht (leer wahr), und die
exe-Liste `P_masken36_alle_cuts.txt` vergleicht nur mit der Tabelle (Soll 46 == Ist 46 fuer 30A1 Cut 0).** Deshalb blieb
M1 trotz "je Raum/Cut nachgewiesen" unentdeckt. Die Dossier-Aussagen §4.2 "Leon und Elza verhalten sich gleich (die
Elza-Dateien der Stages 3..6 sind bytegleich ...)" und §7 "Steht die Figur VOR einem Objekt, wird nie etwas von ihr
uebermalt" sind fuer ROOM30A1 falsch.

**M3 (Nutzertext): "Seit v0.6.x (2026-09-08)" (Dossier §7) ist falsch** — 53741fe5 ist zuerst in v0.7.19 enthalten
(`git tag --contains 53741fe5 | sort -V | head -1`). Richtig: "seit v0.7.19 (2026-09-08)".

Hinweise (keine Maengel): §4.6 (ROOM5021 Cut 6/12 dieselbe Saat-Luecke, natuerlicher Fehlfall nicht gefunden);
OFFEN 1 des Dossiers (32 Sektionen ohne Stehplatz) umfasst neben 30A1 auch Leon-Raeume (z.B. ROOM3040 Cut 6/12) — mit
manueller Saat (-11600,-6500) Band 0 findet das Modell in 3040 Cut 6 Fehlverdeckungen (z.B. (-12400,-5900) 25/81); an der
exe zeigt die Kamera dort gelaufen aber Cut 0, nicht 6 (kein Beleg fuer einen sichtbaren Fehlfall). HINTER 30A0 C3 liegt
mit 47 % knapp unter der Riegelschwelle "Haelfte" (die Figur ragt ueber den Schreibtisch; Bild richtig verdeckt — kein
Mangel). Saumpunkte 3040 C1 (9) und 5090 C14 (2/5) sind im Dossier offen gelegt.

**Ergebnis: NICHT bestanden** (Punkt 1 teilweise: M1; dazu M2, M3).
