# Abnahme 2 — Spur P masken36 (Runde 37, 2026-10-16)

Unabhaengige Abnahme nach Nachbesserung 2. Baum `.claude/worktrees/r37_masken36` (Zweig r37/masken36, Basis bddd6b31,
HEAD ed180493 = 2cb1ad19 + Teilbericht). Gemessen nur am selbst gebauten Stand (`local_build.sh configure` + `build` ->
"ninja: no work to do", `=== LOCAL-BUILD-OK (build)`) mit eigenen exe-Kopien `re15_pc_abnP2_*.exe` und eigenem Scratchpad
`abnP2/`. Die erste Abnahme-2-Sitzung brach am Sitzungslimit ab (Teilbericht ed180493: §1, §3, §5); diese Sitzung hat
deren Kernaussagen stichprobenartig selbst nachgeprueft (unten markiert "nachgeprueft") und §2, §4, §6, §7 ergaenzt.

## Stand der Abnahme (fortlaufend)

- [x] Pflichtlektuere: AUFTRAG.md (Orchestrator-Auftrag P), VERTRAG.md, Dossier P_masken36.md inkl. NB1/NB2,
      P_abnahme_0.md/P_abnahme_1.md, `git log master..HEAD`, `git diff master...HEAD` (Code gelesen)
- [x] Bau (§1)
- [x] gezielte Tests (§2): 48/48 gruen
- [x] RE-Gate (§3)
- [x] Messung an der exe (§4): M1/M2 der Abnahme 1, Stage 1/2, Regression gegen master, Tuerweg, Kosten, neue
      Zufallswege mit Rennen/Zielen (990 Bilder), Endkampf 5090, Je-Cut-Stichprobe
- [x] Vertrag (§5)
- [x] Urteil (§6), Maengel/Hinweise (§7): **bestanden** (0 Maengel, Hinweise H1-H6)

Scratchpad dieser Abnahme (nicht im Baum): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/
c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abnP2/` (lauf.sh, sh.py, py/, runs/, rw/, Bilder).
Vergleichs-exes (Kopien in `abnP2/exe/`, md5): `neu` = re15_pc.exe des Baums (1718938c..., HEAD), `gegen` =
re15_pc_nb2gegen.exe (gemergter Code + NB1-Tabelle, 1bd18d4b...), `master` = Hauptbaum-exe v0.8.25 (bf27fdb9...; Code ==
Merge-Basis e2cc7673, `git diff v0.8.25 master -- re15_port` leer), `alt` = re15_pc_nb2alt.exe (Stand Abnahme 1).

## 1. Bau und Code-Diff

- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.`,
  `=== LOCAL-BUILD-OK (build)`. `git status --short` leer -> die exe entspricht HEAD (md5 re15_pc.exe == `abnP2/exe/neu.exe`).
- Tabelle in der exe nachgeprueft (Skript `abnP2/py/tab_in_exe.py`: die .inc-Zeilen als
  `struct {u16 raum; u8 cut; u8 anzahl; u32 bits[4]}` gepackt und in der exe gesucht): **re15_pc.exe enthaelt die HEAD-Tabelle
  (126 Zeilen + Endzeile, 1444 Bits, anzahl == popcount in jeder Zeile) bei Datei-Offset 0x49D9E0.** Die Vergleichs-exe
  des Bau-Agenten `re15_pc_nb2gegen.exe` enthaelt an DERSELBEN Stelle die NB1-Tabelle von fb4be428 (124 Zeilen, 1224 Bits),
  `re15_pc_nb2alt.exe` die NB1-Tabelle an anderer Stelle (Stand vor dem master-Merge). Kennungs-Strings: re15_pc.exe und
  nb2gegen tragen "Runde 37 Spur P" und "[kistenregel]" (gemergter Code), nb2alt nur "Spur P", die Hauptbaum-exe
  (v0.8.25) nur "[kistenregel]".
- **Tabellenvergleich NB1 -> NB2 (nachgeprueft, eigenes Skript `abnP2/py/tab_cmp.py`, `git show fb4be428:...inc` gegen HEAD):**
  124 -> 126 Zeilen, 1224 -> 1444 Bits; **kein Rechteck wieder an (0)**, 220 neu aus = 72 (ROOM3050/3051 Cut 15, je 36 von 36)
  + 148 (74 je Variante: 3040 C0{1,4,5,6} C1{1} C3{9} C8{19,26,39,43} C9{25,29}; 3050 C2{37} C3{12} C13{16}; 30A0 C0{5,20,21}
  C1{14,19} C3{32..35,39,43,45} C4{8,10,12}; 30E0 C6{6} C7{25,26} C8{7,8} C9{24}; 4040 C11{15}; 4080 C7{7}; 5020 C6{1,8,9,13,
  14,46,47,51} C12{6}; 5050 C2{39}; 5090 C11{0,5,17,19,32} C12{19,47} C13{0,26,29}; 50B0 C3{24}; 5100 C2{16} C4{7,43} C6{19,23}
  C7{5,6,7}; 6000 C5{23,35} C9{10}; 6010 C6{3,6,8,9}); Elza-Varianten jeweils gleich. = Dossier NB2.4.
- Laufzeit-Code (`git diff e2cc7673 HEAD -- re15_port`, gelesen): `bg_pc.c` (Lader: R15M ohne eigenen Atlas -> BSS-Atlas,
  Weg 3 nur fuer Original-Sektionen), `platform/pc/main.c` (+2 Zeilen: `re15_masken36_filter` im R15M-Zweig),
  `platform/psx/main.c` (+2, Gleichlauf), neu `engine/src/masken36_r37.c` + `gen/masken36_ausschluss_r37.inc` +
  `include/re15_masken36_r37.h`. Seit Abnahme 1 (fb4be428) aendert sich am Laufzeit-Code nur die Tabelle und ein Kommentar in
  `masken36_r37.c`. Generator/Riegel `tests/unit/test_r37_masken36.c` (+518 seit NB1): `figur_raster` pixelgenau (Spaltenzahl
  = Kastenbreite in Bildpunkten, je Spalte jede Bildzeile Kopf..Fuss), Grobsprung ueber Summentabelle, Schranken "Kasten
  breiter als das Bild" / "Spalte halb hinter der Kamera", `sektion_pruefen`: Sektion ohne Stehplatz -> alle Rechtecke aus;
  Werkzeuge `punkt`/`zeuge`. Neu `tests/integration/test_r37_masken36_nb2.cmake` (registriert in `probes/r37_masken36.cmake`).
- Keine Asset-Aenderung gegen die Merge-Basis (`git diff e2cc7673 HEAD --stat -- re15_port/shared_assets` leer).
- master ist seit dem Merge 2bb1bbe3 nur um Doku-Commits weiter (`git diff HEAD...master --name-only` nur `analysis/`);
  `git merge-tree --write-tree master HEAD` -> exit 0, ohne Konflikt (Teilbericht).

## 2. Gezielte Tests (KEINE volle Suite)

`ctest --test-dir re15_port/build -R "r37_masken36|r37_pri1100|_pri_|pri_hashes|pri_masken|r35_entladen|sld_atlas|maskgrp|boot_bg_pin|bss_chunk|bss_room_source|zgirl_5090|elzaems_elza5091|stage6_hintergrund" --timeout 900 --output-on-failure -j3`
= Ausdruck des Bau-Agenten (45 Tests) **+ die drei Tests, die in den Raeumen der Spur laufen** (`ctest -N | grep` nach
3040..6031: integration_r35_zgirl_5090, integration_r37_elzaems_elza5091, unit_stage6_hintergrund)
-> **`100% tests passed, 0 tests failed out of 48`**, Total 1228.9 s (Log `abnP2/ctest2.log`; Maschine teils durch
Runde-38-Abnahmen belastet). Laufzeiten: integration_r37_masken36_nb2 384.4 s, stage3 471.8 s, stage4 220.0 s, stage5
216.3 s, stage6 245.0 s, unit_r37_masken36_ausschluss 390.7 s, zensus 4.4 s, verdeckung 0.1 s, integration_r35_zgirl_5090
202.9 s, integration_r37_elzaems_elza5091 16.9 s; alle unit/integration_r37_pri1100_*, integration_pri_masken,
integration_boot_bg_pin, unit/integration_r36_pri_*, unit_pri_*, unit_sld_atlas, unit_r34n_g_maskgrp,
unit/integration_r35_entladen_* (a..l), prop_bss_chunk, unit_bss_room_source gruen.

Werte aus meinem Lauf (LastTest.log), gleich den Angaben des Bau-Agenten:
- nb2: `[pri] cut=15 pri_offset=0x84C masks=0 fg_atlas=1` (Leon und Elza); Cursor Leon 210 / uebermalt 0 / A-B 0, Elza
  210 / 0 / 0; ROOM30A0 Bild 380: 328 / 0, Bild 400: 355 / 0.
- Stage 3 HINTER 1123/1234 (Elza 1158/1260), DAVOR 0/684 (0/1067), FREI 0/883 (0/997), WEG M1 Elza 0/464, Leon 0/602;
  Stage 4 ROOM4080 C5 879/883 (621/627), 0/8374 (0/6165); Stage 5 ROOM5020 C9 199/322 (224/309), 0/5007 (0/5050);
  Stage 6 ROOM6010 C12 820/1440 (639/1105), 0/9489 (0/7931); alle `fg_atlas=1`.
- ausschluss: `Saaten 2626 | Sektionen 216, Masken 6478, davon ausgeschlossen 1444 | Abweichungen 0 | ohne Stehplatz 2`,
  `Figurraster: 924866 Kaesten pixelgenau ... breiter als 320: 28484 (davon Maske koennte verdecken: 0), Spalten halb hinter
  der Kamera: 65038 (davon ... 0)`, `ROOM3050/3051 Cut 15: Original-Sektion +0x84C = FF FF FF FF`, Raumpaare 15/15, bytegleich
  8/8, Tabellenzeilen 126 == 126; 10 PASS.

## 3. RE-Gate (selbst disassembliert: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE =
Auslieferungsstand, `@0x80026e4c = 08 00 e0 03 21 10 00 00` — in dieser Sitzung erneut gelesen)

| zitiert (NB2) | gefunden | haelt |
|---|---|---|
| FUN_800392d4 @0x80039324 lw / @0x8003932c bne -1 / @0x80039338 sb zero = "keine Maske" | **nachgeprueft:** `800392d4 lui a0,0x800b` / `800392d8 lw a0,-14472(a0)` (RDT-Zeiger 0x800ac778), `800392e0 lh v0,4068(v0)` (Cut 0x800b0fe4), `80039310 lw v1,36(a0)` (RDT+0x24), `80039314 sll v0,v0,5`, `8003931c lw t5,28(v0)` (+0x1C), `80039324 lw v1,0(t5)`, `80039328 addiu v0,zero,-1`, `8003932c bne v1,v0,0x8003933c`, `80039330 srl t2,v1,16` (Delay), `80039334 j 0x8003955c`, `80039338 sb zero,0(a0)` (Delay); **Sprungziel 0x8003955c = Epilog** (`lw ra,100(sp)`, `lw fp,96(sp)`, `lw s7,92(sp)` ...). Sonst `80039358 sb t2,0(a0)` (Zahl = obere Haelfte des ersten Worts), danach Aufbau ab 0x8003935c | ja |
| ROOM3050/3051 Cut 15: Kamera-Eintrag +0x25C -> 0x84C = FF FF FF FF | **nachgeprueft** (`abnP2/py/rdt_cut_mask.py`, shared_assets/PSX/STAGE3, ROOM3050.RDT `cmp` == info/Re1.5/PSX/STAGE3): RDT+0x24 = 0x60, Cut 15 Eintrag @0x240, +0x1C @0x25C = 0x84C, Wort `ff ff ff ff` in beiden RDTs | ja - **nicht Cut-15-spezifisch:** in ROOM3050/3051 sind ALLE 16 Cuts NULL-Sektionen (0x810..0x84C je `ff ff ff ff`); jede Generator-Sektion liegt per Konstruktion auf einer NULL-Sektion (Dossier §1.1 "Original nicht NULL 0"). Der NULL-Beleg zeigt, dass "Cut 15 ganz aus" dem Original entspricht - die Auswahl GERADE dieses Cuts traegt allein die Port-Regel "Sektion ohne Stehplatz" (Hinweis H1, §7) |
| Atlas-Kette (Kopf re15_masken36_r37.h, unveraendert seit Abnahme 0) | `80021de8 ori v0,zero,0x15` + `80021dec jal 0x8004ee78` + `80021df0 sh v0,0(s0)` (s0 = 0x800aca4c); FUN_8004ee78: `8004eea8 sll a0,v1,6`, `8004eeb8 addiu a0,a0,-1024` (x = 320), `8004eed0 sll v0,v0,8` (y = 256), `8004eee0 jal 0x80068c88` (= LoadImage, ghidra1_V2.txt Z. 211128/211156); `80039630 ori a3,zero,0x95` + `80039634 jal 0x80069858` (= SetDrawMode, Z. 212249/212278) (Teilbericht) | ja |
| SCD ROOM3050 (Aufbau der Messung): K8 @0x01F90, Pruef-AOT @0x01FB4, Cursor @0x01FDC, Cut_chg @0x02596, sub11..14 | `21 03 b2 00` Ck(3,0xB2)=0; `2c 0a 03 31 .. d4 9a fc bd e8 03 d0 07 ff 00 18 0a` (Slot 10, (-25900,-16900) 1000x2000, sat 0x31, Ereignis 0x0A); `2d 03 04 .. 9e b3 00 00 9c 58` Obj 3 Typ 4 (-19554,0,22684); `29 0f`; `2e 03 03 00 2f 02 c8 00 30 02 01 00` / `.. 2f 02 38 ff ..` / `.. 2f 00 c8 00 ..` / `.. 2f 00 38 ff ..` (Teilbericht; die Rechteck-Bytes in dieser Sitzung fuer den echten Ausloeser §4.1 benutzt) | ja |

Konstanten im Diff: neu sind nur Generator-/Messgroessen im Riegel (SP_MAX 321 = Bildbreite + 1, Messaufbau; Schwelle 24,
Fenster); FIGUR_HOCH 3000 / FIGUR_HALB 350 (an der exe gemessen, Dossier §3.3) und PR 450 (= DAT_80073e94[6], in Abnahme 1
nachgeprueft) unveraendert. Die Tabelle selbst ist Port-Massnahme auf NUTZER-VORGABE-Masken (R15M), im Kopf von
masken36_r37.c so begruendet, nicht als Original verkauft. Rate-Woerter (deferred|tunable|interim|for now|faithful|plausib|
TODO|vorlaeufig) in den +Zeilen von re15_port: **0**; in analysis/ nur die Meta-Erwaehnungen der frueheren Abnahmen.
Neue Env-Schalter (`getenv(` in +Zeilen): **0**. Der Fix erklaert die Befunde der Abnahme 1: Cut 15 hat nach der Tabelle
0 Rechtecke (Log `masks=0`), die Kopfoberkante wird abgetastet (Gegenprobe-exe mit NB1-Tabelle reproduziert 60 bzw. 4/7, §4).

## 4. Messung an der exe

Werkzeug: `abnP2/lauf.sh <exe> <name> <ziel_raum> <framedump> <exit_bild> ENV...` (exe-Kopie `re15_pc_abnP2_<quelle>.exe`
neben re15_pc.exe = Asset-Wurzel, eigener Arbeitsordner `runs/<name>`, `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_FRAMEDUMP=... RE15_EXIT_AT=<bild>#<raum>`); Zaehlung mit eigenem
`abnP2/sh.py` (S = |B-C| > 24 Kanalmaximum, H = davon |A-B| > 24; ohne Fenster = ganzes Bild). A = mit Masken, B =
`RE15_NO_PRI=1`, C = ohne das gemessene Objekt (Figur/Cursor).

### 4.1 Abnahme-1-M1: ROOM3050/3051 Cut 15 Konsolen-Cursor

Aufbau wie Riegel: `RE15_SET_FLAG=3:178 RE15_DEBUG_JUMP=3050@250 RE15_PLAYER_POS=-24757,-15900,3072 RE15_FORCE_EVENT=10@300
RE15_PRESS=square@420,square@422,...,square@960,square@962`, Bild 900; A/B mit `RE15_GIVE=0x46:1` (Kistenregel K8), C ohne
Karte (= Bild ohne Cursor).

| Lauf | Log | Cursor-Punkte S | uebermalt H | A/B ganzes Bild |
|---|---|---|---|---|
| Leon neu | `[kistenregel] ROOM3050 @0x01F90 Ck(3,0xB2) K8: Item 0x46 im Inventar -> wie Original`, `[scd F783] Cut_chg(15)`, `[pri] cut=15 pri_offset=0x84C masks=0 fg_atlas=1`, keine Spur-P-Zeile | 210 | **0** | **0 (A bitgleich B)** |
| Leon gegen (NB1-Tabelle) | `... Cut_chg(15)`, `[pri] ROOM3050 Cut 15: Port-Masken ohne eigenen Atlas -> BSS-Atlas ...`, `masks=36 fg_atlas=1` | 210 | 60 | 138 |
| Elza neu (`RE15_TITLE_SHOT_CHAR=1`, ROOM3051) | `ROOM3051 @0x01F90 ... im Inventar`, `Cut_chg(15)`, `masks=0 fg_atlas=1` | 210 | **0** | **0** |
| **Leon neu, ECHTER Ausloeser** (kein FORCE_EVENT: Lage (-24757,-15900) Blick 2048, `square@300`) | `[scd F783] Cut_chg(15)`, `masks=0 fg_atlas=1` | (2095*) | **0** | **0** |
| Leon gegen, echter Ausloeser | `masks=36 fg_atlas=1` | (2095*) | 60 | - |

(* C stammt aus dem FORCE_EVENT-Lauf; der echte Ausloeser zeigt den Cursor in einer anderen Lage, S zaehlt daher mehr als den
Cursor; H und A/B sind davon unabhaengig.) Der echte Ausloeser: der Vorwaertspunkt (pos + 620 in Blickrichtung, aot_common.c) liegt bei Blick 2048 in Slot 10 ((-25900,-16900) 1000 x 2000, sat 0x31) - Square loest sub10 aus wie im
Spiel (Reichweite 620 = `80042bd0 ori v0,zero,0x26c`, selbst disassembliert); bei Blick 3072 (Aufbau des Riegels)
nicht, deshalb dort FORCE_EVENT. B neu == B gegen bitgleich. Bild
`abnP2/k15_gegen_neu.png` (gegen A | neu A | neu B): gegen fehlt dem Cursor die dunkle Innenkante, neu == ohne Masken.
**Abnahme-1-M1 behoben** (Leon und Elza, auch ueber den echten Ausloeser).

### 4.2 Abnahme-1-M2: Kopfoberkante ROOM30A0 Cut 0

Zufallsweg der Abnahme 1 (`RE15_DEBUG_JUMP=30A0@250 RE15_INPUT_SCRIPT_START=520 RE15_INPUT_SCRIPT=R0.69,U1.12,R0.23,U1.15,
R0.19,U0.98,R0.44,U0.88,L0.14,U1.02,L0.16,U2.18,R0.67,U1.89`), Bilder 360..440/20, C = Zwangs-Cut 0 bei (-19800,19763) ohne
Figur. `[walk]`-Zeilen neu A == neu B == gegen A (F360 (-26046,13582), F390 (-25605,13657), F420 (-24897,13261)).
Log neu `[pri] cut=0 pri_offset=0x3C0 masks=18 fg_atlas=1`, gegen `masks=21`.

| Bild | neu: S / H | gegen (NB1-Tabelle): S / H |
|---|---|---|
| 360 | 495 / 0 | 495 / 0 |
| 380 | 328 / **0** | 328 / 4 |
| 400 | 355 / **0** | 355 / 7 |
| 420 | 666 / 0 | 666 / 0 |
| 440 | 995 / 0 | 995 / 0 |

Bild `abnP2/w30_kopf.png` (Bild 400, 6-fach: gegen A | neu A | neu B). **Abnahme-1-M2 behoben** (die Zahlen der Abnahme 1
reproduziert die Gegenprobe-exe genau).

### 4.3 Stage 1/2 (eigener Atlas, Spur A) unveraendert — neu gegen master (v0.8.25 = Merge-Basis-Code)

Gleicher Lauf mit beiden exes, Framedump Bild 300, `cmp`:

| Lauf | Framedump | `[pri]`-Zeile (neu == master) | Spur-P-Zeilen neu |
|---|---|---|---|
| Leon ROOM1100 C1 (-14914,-25645) rot 4488 (F9-Marke Spur A), `RE15_FORCE_CUT=1` | **bitgleich** | `cut=1 pri_offset=0x60C masks=52 fg_atlas=1` | 0 |
| Leon ROOM1220 Sprung-Spawn | **bitgleich** | `cut=0 pri_offset=0x4DC masks=20 fg_atlas=1` | 0 |
| Leon ROOM2090 Sprung-Spawn | **bitgleich** | `cut=0 pri_offset=0x670 masks=56 fg_atlas=1` | 0 |
| Elza ROOM1101 C1 (Zwilling -> 1100) | **bitgleich** | `cut=1 pri_offset=0x60C masks=52 fg_atlas=1` | 0 |
| Elza ROOM2091 (Zwilling -> 2090) | **bitgleich** | `cut=0 pri_offset=0x670 masks=56 fg_atlas=1` | 0 |

(Abnahme 1 hatte 2091 gegen die alte vorher-exe als "verschieden" vermerkt - Ursache war be2dbd26 der Integration; gegen
master v0.8.25, das be2dbd26 enthaelt, ist auch 2091 bitgleich.)

### 4.4 Regression behoben: Stages 3..6 neu gegen master (v0.8.25), Leon und Elza

Zwangs-Cut, Bild 120, Lagen der Riegel; C = Figur ausserhalb des Regionsvierecks; Fenster der Riegel.

| Szene | exe | Log | S / H | Bild |
|---|---|---|---|---|
| Leon ROOM3050 Cut 2 (-22000,-20768) HINTER der Maschine, C (-23996,1523), Fenster -9 122 55 206 | master | `cut=2 pri_offset=0x818 masks=62 fg_atlas=0` | 1234 / **0** | master A == neu B (ohne Masken) **bitgleich** |
| | neu | `masks=59 fg_atlas=1` + Spur-P-Zeile | 1234 / **1123** | Leon hinter der Maschine verdeckt |
| Elza ROOM6011 Cut 12 (-18612,-7920) HINTER dem Treppenpfeiler, C (-27015,-25588), Fenster 187 81 217 176 | master | `cut=12 pri_offset=0x7A4 masks=53 fg_atlas=0` | 1105 / **0** | master A == neu B **bitgleich** |
| | neu | `masks=53 fg_atlas=1` | 1105 / **639** | rechte Haelfte hinter dem Pfeiler verdeckt |

Bilder `abnP2/h3050_master_neu.png`, `abnP2/h6011_master_neu.png` (master A | neu A | C, 3-fach): in v0.8.25 steht die
Figur VOR der Maschine bzw. dem Pfeiler, obwohl sie dahinter steht; neu verdeckt das Objekt sie. **Der Befund des Auftrags
(Masken der Stages 3..6 fuer niemanden gezeichnet) ist in v0.8.25 reproduziert und mit HEAD behoben.**

Nebenbefund (Hinweis H2, kein P-Fehler): in Elza ROOM6011 Cut 12 scheinen 14 Figurpunkte durch den Pfeiler - Zeile 152
(x 202-208, rote Hose) und Zeilen 159-160 (x 204-208). Eigener Leser der R15M-Sektion (`abnP2/py/msk_cover.py`, wie
pri_common.c re15_pri_parse_section): (205,151) liegt in Rechteck #38 [199,140 14x12 d=54], (205,153) in #41 [199,153 18x6],
(205,161) in #37 [199,161 20x48]; **(205,152) und (205,159)/(205,160) deckt KEIN Rechteck**, und Cut 12 hat keine
Tabellenzeile (nichts ausgeschlossen). Die Fugen liegen in den Generator-Daten vom 2026-09-03 (Abdeckung 74 %, Dossier §1.2)
= "wie gebaut"; Bild `abnP2/h6011_zoom.png` (neu A | C | B, 8-fach).

### 4.5 Echter Weg durch die Tuer (Leon ROOM3090 -> ROOM30A0)

`RE15_DEBUG_JUMP=3090@250 RE15_PLAYER_POS=-13135,-9065,3072 RE15_PRESS=square@60,square@62 RE15_INPUT_SCRIPT_START=600
RE15_INPUT_SCRIPT=L0.233,U1.07`, Bilder 300..420/20 in ROOM30A0, A / B (`RE15_NO_PRI=1`), C = Zwangs-Cut 0 ohne Figur.
Log: `[room] PC loaded room3090.rdt` -> `[tuer] Sequenz Archiv 2 DOOR06 ... 311 Bilder` -> `[room] PC loaded room30a0.rdt` ->
`[pri] ROOM30A0 Cut 0: Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)` -> `[pri] cut=0
pri_offset=0x3C0 masks=18 fg_atlas=1`; Kamera schaltet selbst auf Cut 0, `[walk] F330..F420 pl pos=(-24284,0,13657)`.
Leon VOR der Buero-Trennwand: S = 488 / 571 / 601 / 601 / 601 / 601 / 601, **H = 0 in allen 7 Bildern**; A/B im ganzen Bild je
2 Punkte (statischer Blit-Rest, nicht an der Figur). Bild `abnP2/tuer30a0.png` (A | B | C). (Elza 3091 -> 30A1 ueber die Tuer:
Abnahme 1 §4.1 und Riegel stage3 WEG M1, in meinem ctest 0/464.)

### 4.6 Was die 148 zusaetzlichen Ausschluesse kosten (Stichprobe ROOM5100 Cut 7, Stage 5)

ROOM5100 Cut 7 verliert in NB2 die Rechtecke {5,6,7} (Stage 5 = die Stage mit dem groessten Modell-Verlust, -11 %).
HINTER-Szene h5100c7 (Dossier §4.2) Leon (-16130,-2500), C (-16130,-15000) ausserhalb des Regionsvierecks ((-14600,-12300)
(-17100,2200) (-5500,1800) (-5297,-12303), Werkzeug `test_r37_masken36 cut 5100 7`), Fenster aus `proj 5100 7 -16130 -2500`
= 20 101 50 187:

| exe | Log | S / H |
|---|---|---|
| neu (NB2) | `cut=7 pri_offset=0x5A0 masks=4 fg_atlas=1` | 1228 / **471** |
| gegen (NB1-Tabelle) | `masks=7 fg_atlas=1` | 1228 / **471** |

Gleich dem Dossier (§4.2: 471 von 1228). Die drei neu ausgeschlossenen Rechtecke tragen an dieser Stelle nichts bei; der
Modell-Verlust von -11 % zeigt sich in dieser Szene nicht. Bilder `abnP2/h5100.png` (gegen A | neu A | B | C),
`abnP2/h5100_beine.png`.

### 4.7 Neue Zufallswege mit Rennen und Zielen (Seed 38016, 30 Raumvarianten, 990 Bildpaare)

Die Zufallswege der Abnahme 1 / NB2.5 (Seed 37014) kannte der Bau-Agent; NB2.4 sagt selbst "Ob eine andere Haltung (Drehung,
Zielen mit ausgestrecktem Arm) dort trifft, ist nicht gemessen". Deshalb neue, unabhaengige Wege: `abnP2/py/rw_gen2.py`
(Seed 38016, ~20 s je Weg: Drehen L/R 0.1-0.7 s, Gehen U 0.6-2.2 s, 15 % Rennen `XU`, 10 % Zielen `M` = R1), Leon xxx0 und
Elza xxx1 in allen 15 Raumpaaren, Browning ausgeruestet (`RE15_GIVE=0x03:15 RE15_EQUIP=0x03`, Log `[equip] ... W03`),
`RE15_DEBUG_JUMP=<xxx0>@250 RE15_INPUT_SCRIPT_START=520`, natuerliche Kamera, Bilder 260..900/20; A mit Masken, B
`RE15_NO_PRI=1` (Skripte `abnP2/rw_run2.sh`, `rw_batch2.sh`, Liste `rw_plan2.txt`). **60 von 60 Laeufen rc=0, je 33 Bilder,
`[walk]`-Lagen A == B in allen 30 Raumvarianten, jeder Lauf bleibt im Raum.** Auswertung `abnP2/py/rw_eval3.py` (Punkte
|A-B| > 24, die nicht in mindestens der Haelfte der Bilder desselben Cuts stehen = was sich mit der Figur bewegt; Ausgabe
`abnP2/rw_eval2_out.txt`), danach JEDER Fund angesehen (`abnP2/py/diffbild.py`: A | B | B mit rot markierten Punkten,
`abnP2/kand/`, `abnP2/rw2_*.png`):

| Raum Cut | Bilder | Fund | Urteil |
|---|---|---|---|
| 30A0, 30E0/E1, 4040/41, 4080/81, 40A1, 50B1, 5090/91, 6000, 6030/31 | alle | keine bewegten Punkte | - |
| 5050 C0, Leon rennt mit der Waffe dicht rechts neben der blauen Maschine (-25979,-20115)/(-25979,-20367) | F860 / F880 | 535 / 171 Punkte: Waffenhand und vorderer Fuss verschwinden an der rechten Kante der Maschine bzw. hinter dem Papierstreifen (`rw2_5050_860_zoom.png`, `rw2_5050_880_zoom.png`) | **tiefenrichtig**: Leon steht 720 noerdlich der Maschinen-Zelle #4 (x -27872..-24102, z -25960..-20835), 1877 hinter deren der Kamera zugewandter Flaeche; Arm und Fuss ragen ueber z -20835 IN die Maschine (Maskentiefe 61 = vz ~3900, Leon ~6100); Koerper neben der Maschine unverdeckt. Modell `punkt 5050 0 -25979 -20115`: 3248 Punkte, ohne Zelle davor 0 (H5) |
| 3040 C0, Leon rennt gegen die Holzkiste (-24768,-252) | F740 (+ F540/F580 20-25) | 59 Punkte am vorderen Fuss (`rw2_3040_740.png`) | tiefenrichtig: Leon 468 westlich der Kisten-Zelle #8 (x -24300..-21850, z -1500..500), 1248 hinter ihrer Vorderflaeche (Kamera (-25704,-8010) blickt +z); der Fuss ragt in die Kiste (Maske #14 Tiefe 103 = vz ~6600, Leon ~7560) |
| 30A1 C3, Elza zielt (-21121,17756), Blick -2304 | F640-F740 | 174-295 Punkte: ein Karton liegt ueber ihrem Oberkoerper/Waffe, Beine darunter frei (`rw2_30A1_680.png`, `rw2_30A1_680_ganz.png`) | tiefenrichtig: Kartonmaske Tiefe 50 (vz ~3200) gegen Elza vz ~5240, solide Rund-Zelle #11 auf der Sichtstrecke; Modell `punkt 30A1 3 -21121 17756` 4060 Punkte, ohne Zelle davor 0 (H4) |
| 5051 C0, Elza Stiefelspitze am blauen Pfosten | F500 / F760 | 23 / 39 Punkte (`kand/k_5051_760.png`) | wie Abnahme 1 (dort 8-9): uneindeutig, Modell an beiden Lagen 0 |
| 3050 C0 (Hand am roten Kanister), 5021 C0 (Pfostenkante), 5100/5101 C3 (Fuesse am Plattformrand), 6010 C1 | einzelne | 1-6 Punkte | Saum, wie Abnahme 1 |
| 5020 C1 (linker Pfeiler 608), 50B0 C0 (Tank 131), 5100 C3 F760 (Gelaender 170), 6001 C0 (Tuer 20), 6010/6011 C0 (Gelaender 49-79), 40A0 C0 (24), 30A1 C2 F620/F880 (Cut-Wechsel) | einzelne | statischer Blit-Rest (Palette gegen MDEC), keine Figur im markierten Bereich (`kand/*.png`) | kein Befund |

**Ergebnis: auf 990 natuerlichen Bildern mit Gehen, Rennen und Zielen wird keine Figur, die VOR einem Objekt steht,
uebermalt.** Wo Masken Figurteile wegnehmen, steht die Figur hinter der Vorderflaeche des Objekts bzw. ragt mit Arm/Bein
hinein (tiefenrichtig, Hinweis H5) - sichtbar ist das an der exe als "Waffe/Fuss verschwindet an der Kante" (5050 C0 bis
535 Punkte), was der Nutzertext §7 ("einzelne Bildpunkte an Maskenkanten") untertreibt.

### 4.8 Groessere Figur: Endkampf ROOM5090 (Aufbau des Riegels integration_r35_zgirl_5090, ohne Software-Renderer)

`RE15_DEBUG_JUMP=5090@gp RE15_PLAYER_POS=500,-9175,2048,0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100
RE15_INPUT_SCRIPT=A0.1,W1,U12 RE15_SPAWN_DIAG=1 RE15_STATE_LOG=state.log`, Bilder 200..1200, A / B. Log: `DOOR FIRE slot=2 ...
target_cut=14`, `Sce_em_set type=0x36`, `[pri] ROOM5090 Cut 14/13/12/11: Port-Masken ohne eigenen Atlas -> BSS-Atlas`
(`masks=58/37/50/29 fg_atlas=1`). `[walk]`-Lagen A == B == master-exe.
- Boss (Typ 0x36) und Annette (Kittel, Cut 11): Teile des Boss-Koerpers werden von Wand-/Gelaender-/Regal-Masken abgedeckt
  (der Boss ist breiter als der Gang und ragt seitlich in Wand und Gelaender; ohne Modell des Boss nicht abschliessend
  beurteilbar = Dossier OFFEN "groessere Figuren ungeprueft"); Annette und Leon VOR dem Boss bleiben frei (`boss_f380.png`,
  `boss_f480_zoom.png`, `boss_f760_zoom.png`); die Flammen-Sprites am Boss deckt das rechte Regal teilweise ab.
- **Leon verschwindet ab Bild 887 ganz** (Cut 12, Wandmaske links; A/B im Fenster 40 30 140 140: F890 700, F950 1038, F1100
  1017, F1190 722 Punkte; `boss_f900_zoom.png`, `boss_f1100.png`). Ursache: der anrueckende Boss schiebt den stehenden Leon
  (pad 0000, mo 211) von (2983,-25106) bis F884 (4647,-25297), in F887 springt er auf (4588,-26182) und bleibt dort bis
  mindestens F1170 (`state.log`, `[walk]`). Diese Lage liegt IN einer soliden Zelle: `RE15_PLAYER_POS=4588,-26182` +
  Zwangs-Cut 12 -> die Kollision schiebt ihn sofort auf (5768,-26182) (+1180 > Radius 450). SCA von ROOM5090 (eigener Leser
  `abnP2/py/zellen.py`, wie rdt_common.c RDT+0x20): die Lage liegt in Zelle #30 (type 6, solide, x -425..5300, z
  -27200..-25200, Band 0 = der seitliche Wandstreifen); die Sichtstrecke von der Kamera Cut 12 (18526,-2656,-24660) kreuzt
  zusaetzlich Zelle #29 (type 1, solide, x 5300..20250, gleiche z-Spanne; Gegenprobe `RE15_PLAYER_POS=8000,-26182` ->
  Kollision schiebt auf (8000,-24732) = Rand von #29 + 450). Modell `punkt 5090 12 4588 -26182`: 637 Punkte, ohne Zelle
  davor 0. **Die Lagen sind mit der master-exe (v0.8.25) bitgleich** (Schub vorbestehend,
  nicht Spur P); dort ist Leon in der Wand sichtbar (`cut=12 ... masks=57 fg_atlas=0`), mit Spur P verdeckt ihn die Wand.
  -> Hinweis H3 (eigene Spur: Schub in die Zelle; P erfuellt hier "Figur in/hinter dem Objekt", aber der Nutzer sieht im
  Endkampf einen unsichtbaren Leon).

### 4.9 Je-Cut-Liste an der exe (Stichprobe 14 Sektionen, Soll unabhaengig aus MSK + .inc)

Soll = Rechtecke der R15M-Sektion (eigener Leser `abnP2/py/msk_cover.py`) minus Bits der .inc (Liste `abnP2/sek_soll.txt`);
Ist = `[pri] cut=<c> ... masks=<n> fg_atlas=<f>` nach `loaded room<raum>` (`RE15_DEBUG_JUMP=<xxx0>@250 RE15_FORCE_CUT=<c>`,
Elza `RE15_TITLE_SHOT_CHAR=1`, Bild 40; `abnP2/sek_ist.txt`):

| Sektion | Soll | Ist | Spur-P-Zeile |
|---|---|---|---|
| 3040 C1, 3041 C1, 30E0 C6, 30E1 C7, 6010 C6, 6011 C6, 3050 C6 (die LEER-Sektionen) | 0 | `masks=0 fg_atlas=1` | nein (Weg einer Original-NULL-Sektion) |
| 30A1 C3 / 4081 C5 / 5020 C6 / 5090 C11 / 5091 C13 / 5100 C7 / 6000 C5 | 35 / 5 / 14 / 29 / 37 / 4 / 21 | gleich, `fg_atlas=1` | ja |

14 von 14 == Soll, gleich `P_masken36_alle_cuts.txt` (NB2.6).

## 5. Vertrag (statisch)

- Pfade (`git diff e2cc7673 HEAD --name-only` ohne analysis/, in dieser Sitzung erneut gelistet, 12 Dateien):
  engine/src/masken36_r37.c, engine/src/gen/masken36_ausschluss_r37.inc, include/re15_masken36_r37.h, platform/pc/src/bg_pc.c
  (Masken-Lader: VERTRAG §1.4 erlaubt P genau das), platform/pc/main.c + platform/psx/main.c (je 2 Zeilen, Kommentar
  "Runde 37 Spur P"; platform/pc/main.c steht in der Liste §1.4), tests/unit/probes/r37_masken36.cmake,
  tests/unit/test_r37_masken36.c + _szenen.inc, tests/unit/probe_r37_masken36_ppm.c, tests/integration/
  test_r37_masken36_verdeckung.cmake + _nb2.cmake (beide `cmake_minimum_required(VERSION 3.16)`, je 1 Treffer). analysis/:
  nur P_* (Dossier, Abnahmen, alle_cuts.txt). Keine release/, platform/android/, shared_assets/ (Asset-Diff leer), keine
  Test-CMakeLists, keine MASKS-Dateien. (`git diff master --name-only` zeigt zusaetzlich analysis/befunde_runde38/* - die
  Doku-Commits, die master voraus ist, nicht Arbeit der Spur.)
- Keine Bank-9-Bits, Nachrichten-Ids, Ereignisse, Slots. Kein popen/_DEFAULT_SOURCE noetig. Neue Logik in neuen Dateien
  (masken36_r37.c / re15_masken36_r37.h) wie §1.4 verlangt.
- Tests je Punkt vorhanden und messend: M1/M2 der Abnahme 1 als exe-Riegel `integration_r37_masken36_nb2` (Cursor muss im
  Bild sein, S > 30 - kein leer wahres A == B; die Gegenprobe des Bau-Agenten mit der NB1-Tabelle faellt durch, von mir an der
  exe reproduziert: 60 bzw. 4/7, §4.1/§4.2), Stage 3..6 HINTER/DAVOR/FREI/WEG Leon + Elza, Unit-Riegel Tabelle == Nachrechnung.
- Nebenwirkungen: Stage 1/2 bitgleich zu master (§4.3); Raum-Tests in den Raeumen der Spur (zgirl_5090, elzaems_elza5091,
  stage6_hintergrund) gruen (§2); Endkampf 5090 siehe H3.

## 6. Urteil je Punkt (gegen den Wortlaut)

| # | Punkt | Urteil | Beleg |
|---|---|---|---|
| 1a | "diese Masken verdecken die Figuren wieder wie am 2026-09-03 gebaut (BSS-Atlas)" | erfuellt | v0.8.25 (master-exe): `fg_atlas=0`, Bild == ohne Masken, Leon/Elza VOR Maschine bzw. Pfeiler gezeichnet, obwohl dahinter; HEAD: `fg_atlas=1` + Spur-P-Zeile, Leon ROOM3050 C2 1123/1234, Elza ROOM6011 C12 639/1105 verdeckt (§4.4); Riegel Stage 3..6 Leon + Elza in meinem ctest (§2); Je-Cut-Stichprobe 14/14 == Soll (§4.9). Offengelegte Grenzen: 1444 von 6478 Rechtecken aus (22 %), sechs Sektionen je Variante ohne Generator-Maske (eine davon Cut 15 = Original-NULL), Fugen in den Generator-Daten (H2) |
| 1b | "ohne die Raeume mit eigenem Atlas (Stage 1/2, Spur A) zu veraendern" | erfuellt | 5 Laeufe (Leon 1100 an der F9-Lage, 1220, 2090; Elza 1101, 2091) bitgleich zu master v0.8.25, gleiche `[pri]`-Zeilen, 0 Spur-P-Zeilen (§4.3); Spur-A-Riegel gruen (§2) |
| 1c | "Figuren VOR einem Objekt werden nicht verdeckt" | erfuellt | Abnahme-1-M1 behoben (Cursor 60 -> 0, Leon/Elza, auch ueber den echten Ausloeser, §4.1); Abnahme-1-M2 behoben (Kopf 4/7 -> 0, §4.2); Tuerweg 3090 -> 30A0: H = 0 in 7 Bildern (§4.5); Riegel DAVOR 0 in Stage 3..6 (Leon + Elza), FREI 0, WEG 0; 990 neue Zufallsbilder mit Rennen und Zielen ohne Uebermalung einer Figur vor einem Objekt (§4.7). Reste wie dokumentiert (5090 C14 2/5 Punkte am Fuss) plus tiefenrichtiges Abschneiden von Arm/Bein, die in ein Objekt ragen (H5) |
| 1 | Punkt 1 gesamt | **erfuellt** | 1a/1b/1c an der exe gemessen; die Befunde der Abnahme 1 sind mit Gegenprobe behoben |
| 2 | Bezug: Verdeckung fehlte in den Stages 3..6 unbemerkt | erfuellt | Dossier §7 "seit v0.7.19 (2026-09-08, Commit 53741fe5) ... Masken seit v0.3.96"; nachgeprueft `git tag --contains 53741fe5 \| sort -V \| head -1` = v0.7.19 (2026-09-08 14:55), b8db7b2b = v0.3.96 (2026-09-03 10:30); Regression an der master-exe reproduziert (§4.4) |

Gates: RE-Gate haelt (§3: NULL-Pruefung @0x80039324..38 + Sprungziel 0x8003955c = Epilog, Vorwaerts-Reichweite
@0x80042bd0, RDT-Bytes 3050/3051 selbst gelesen; 0 Rate-Woerter, 0 neue Env-Schalter; jede Konstante @0x, Messaufbau oder
Port-Massnahme auf NUTZER-VORGABE-Masken). Vertrag haelt (§5). Tests 48/48 gruen (§2).

## 7. Maengel

**Keine.**

Hinweise (keine Maengel; fuer Orchestrator/Nutzer):
- **H1 (Begruendung Cut 15):** "Original: keine Maske in diesem Bild" stimmt, gilt aber fuer JEDE Generator-Sektion (in
  ROOM3050/3051 sind alle 16 Cuts NULL, §3). Dass gerade Cut 15 ganz aus ist, traegt die Port-Regel "Sektion ohne Stehplatz
  -> alle Rechtecke aus" (Dossier NB2.1 sagt das richtig; der Kopf von masken36_r37.c liest sich, als waere die NULL-Sektion
  der Grund). Der Riegel haelt die Liste ohne Stehplatz fest (jede weitere Sektion faellt durch).
- **H2 (Fugen in den Generator-Masken):** ROOM6011 Cut 12 (Riegel-Szene Stage 6): 14 Punkte von Elzas Hose scheinen als
  Linie durch den Pfeiler (Zeilen 152 und 159-160 deckt kein Rechteck, nichts ausgeschlossen; §4.4). "Wie gebaut" (Abdeckung
  74 %, Dossier §1.2), aber sichtbar; im Nutzertext waere ein Satz dazu ehrlich ("einzelne Zeilen der Figur koennen durch
  Fugen der automatisch erzeugten Masken scheinen").
- **H3 (Endkampf ROOM5090, eigene Spur):** der anrueckende Boss schiebt den stehenden Leon in die solide Zelle #30
  (Wandstreifen, Lage (4588,-26182); vorbestehend, Lagen mit master bitgleich); dort verdeckt ihn seit Spur P die Wandmaske ganz (F887 bis
  mindestens F1170, bis 1048 Punkte), vorher war er in der Wand sichtbar. Ursache ist der Schub, nicht der Lader; Vorschlag:
  als eigener Befund (Kollision beim Boss-Schub) aufnehmen, sonst meldet der Nutzer "Leon unsichtbar im Endkampf". Bilder
  `abnP2/boss_f900_zoom.png`, `abnP2/boss_f1100.png`.
- **H4 (ROOM30A1 Cut 3, Elza zielt hinter dem Karton):** der Karton liegt ueber Oberkoerper und Waffe (174-295 Punkte).
  Nachgerechnet tiefenrichtig: die Kartonmaske ist #51 [109,100 25x24, Tiefe 50] (`msk_cover.py`), Tiefe 50 = vz ~3200
  (Generator-Band 0.90 -> Objekt ~3560), Elzas Oberkoerper vz ~5240 (Kamera Cut 3 (-26044,-3300,18040) -> Ziel
  (-16206,1110,15596)); die Kartontiefe entspricht ~68 % der Sichtstrecke, und genau dort kreuzt sie die solide Rund-Zelle
  #11 (type 3, x -24419..-22784, z 17438..19073; `zellen.py`) - der Karton steht also zwischen Kamera und Elza. Kein Mangel;
  Bild `abnP2/rw2_30A1_680_ganz.png`.
- **H5 (Arm/Bein in Objekten):** wenn die Figur dicht neben einem Objekt rennt oder zielt, schneiden die Masken Waffe, Hand
  oder Fuss an der Objektkante ab (ROOM5050 C0 bis 535 Punkte, ROOM3040 C0 59 Punkte) - tiefenrichtig: in ROOM5050 steht
  Leon (-25979,-20115) 720 noerdlich der Maschinen-Zelle #4 (type 1, x -27872..-24102, z -25960..-20835), deren Vorderflaeche
  (x -24102) 1877 naeher an der Kamera (-19814,-620,-20846) liegt; Arm und Fuss ragen ueber z -20835 IN die Maschine
  (Maskentiefe 61 = vz ~3900 gegen Leon ~6100). Das ist mehr als die "einzelnen Bildpunkte" des Nutzertexts §7 - ein Satz
  dazu waere ehrlich ("Arme/Beine, die in ein Objekt ragen, werden an dessen Kante abgeschnitten").
- **H6 (Kosten):** in der Stichprobe ROOM5100 Cut 7 aendern die drei neu ausgeschlossenen Rechtecke die Verdeckung nicht
  (471/1228 wie NB1, §4.6); den Modell-Verlust Stage 5 -11 % habe ich an der exe nicht gesehen, aber auch nicht gezielt
  gesucht (kein Szenenfinder fuer "Verdeckung nur mit NB1-Tabelle").
- Android: Tabelle erneut geaendert -> APK neu bauen (Dossier). PSX: Freistellungs-Atlas fehlt dort weiter (vorbestehend).

**Ergebnis: BESTANDEN.** Abnahme-1-M1 (Cut-15-Cursor) und -M2 (Kopfoberkante) sind an der exe behoben, die Regression ist
gegen v0.8.25 belegt behoben, Stage 1/2 bitgleich, keine Figur vor einem Objekt uebermalt (Riegel, Tuerweg, 990 neue
Zufallsbilder mit Rennen/Zielen), Gates halten, 48/48 gezielte Tests gruen.
