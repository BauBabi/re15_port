# Spur M masken36 — Abnahme 5 (unabhaengig, 2026-10-25, nach Nachbesserung 5)

Baum `.claude/worktrees/r41_masken36` (Zweig r41/masken36, merge-base master 4c667c97, Kopf e4ba4d22; Nachbesserung 5 = c9fca410..e4ba4d22, darin
23570b48 = Orchestrator-Eintrag Nachtrag 32b). Pruefer: Abnahme 5. Massstab: Wortlaut AUFTRAG.md (Punkte 1/2, Nachtrag 11, Orchestrator-Hinweis 03:21,
Nachtrag 27 / M-E1, **M-E2** 14:29, **Nachtrag 32b** "M: laufende Nachbesserung 5 + Abnahme 5 (keine Nachbesserung 6) ... Integration F/G/M ('gut
genug': nur blockierende Fehler halten auf)"), VERTRAG.md §1.3/§2, CLAUDE.md RE-Gate, Maengel 1/2 der Abnahme 4 (`M_abnahme_4.md`, args_m_nb5.json).
Messordner (Scratchpad): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/M41abn5/`
(Skripte `run5.sh`/`szene4.sh`/`tuer5.sh`/`py/*.py`, Laeufe `runs/`, Bilder `bilder/`).

## Ergebnis in Kuerze
**Nicht bestanden** (Punkt 1 teilweise, Punkt 2 erfuellt) - **alle drei Maengel sind nicht blockierend** im Sinn von Nachtrag 32b (kein Absturz, kein
Rueckschritt, keine Uebermalung einer Figur VOR einem Gegenstand; ein Restloch + Dokumentation/Riegel-Aussage). Was die NB5 liefert, haelt in den Kernpunkten:
M-E2 ist umgesetzt (267 Original-Atlas-Teile in 61 Kameras mit Lage + Zellen-Tiefe; eigene exe-Messung Leon und Elza: HINTER verdeckt, DAVOR 0
uebermalt; natuerlicher Weg durch die Tuer 3030 -> 30E0 mit Kamerawechsel 6 -> 7 -> 8 -> 9), Tuer-Band und Kapazitaet sind belegt (selbst
disassembliert, dazu der Door_aot_set-Handler), statisch **0 Punkte weg, 0 Tiefen geaendert**, roter Ordner zu Recht in der Nutzer-Liste (selbst
belegt: kein Atlas-Teil), Tests unit 45/45 und Integration 41/41. Offen: (1) Restloch OFFEN 18 an der exe sichtbar (6010/6011 C5, Kopf unter der
Treppenwange), Ursache die Port-Kapazitaet 128 statt der Original-Grenze 255; (2) der Teile-Bericht `teile_nb5.txt`, auf den die Nutzer-Liste
verweist, ist von einem frueheren Lauf (21 Teile mit anderem Ergebnis); (3) der HINTER-Beleg fuer ROOM4040/4041 Cut 12 misst ein 3D-Tor statt der
Spielfigur (Neubewertung aller 40 HINTER-Szenen: die uebrigen 38 messen die Figur und zeigen sie verdeckt).

## Stand
- [x] Lektuere AUFTRAG (Spurbaum = master bis Nachtrag 32b: `git diff e4ba4d22 master -- AUFTRAG.md` leer), VERTRAG, Dossier NB5 (Z. 1059-1290, OFFEN 17-21,
      "Fuer den Nutzer"), Abnahme 4, `orchestrator/args_m_nb5.json`; `git log master..HEAD`; Code-Diff NB5 `2ea86d24..HEAD -- re15_port` gelesen
- [x] Bau: `local_build.sh configure` OK, `build` -> "ninja: no work to do" (exe 17:25:22, Baum sauber, md5 84fd9c2f)
- [x] Gezielte Tests: unit 45/45, Integration 41/41 (Regex des Bau-Agenten + Raumgruppen r36_story/r38_kleinkram_flammenwerfer/r37_sperre1050)
- [x] Punkt 1: RE-Gate, statischer Rueckschritt-Vergleich, 19 eigene Szenen (Leon + Elza), natuerlicher Weg, Neubewertung aller 40 HINTER-Szenen des
      Bau-Agenten, OFFEN 18 an der exe, roter Ordner, Nutzer-Liste
- [x] Punkt 2: unveraendert, pri/ unveraendert, eigene Elza-Stichprobe
- [x] Vertrag, Urteil

## Lektuere und Diff
- `git log master..HEAD`: NB5 = c9fca410 .. e4ba4d22 (13 Spur-Commits + Orchestrator 23570b48). `git diff master` zeigt wegen des weitergelaufenen
  master viele Loeschungen fremder Spuren; massgeblich ist der Zweig-Diff `git diff 4c667c97..HEAD` (907 Dateien, davon 291 ausserhalb analysis/):
  `engine/src/gen/masken36_ausschluss_r37.inc`, `include/re15_pri.h`, `shared_assets/PSX/MASKS/` (256), `tests/integration/test_r41_masken36_*.cmake`
  + Laeufer `test_r41_masken36_stage2.cmake`/`test_r37_masken36_verdeckung.cmake`, `tests/unit/{test_r37_masken36.c, test_r41_masken36.c,
  gen_r41_masken36_*.inc, probe_r41_masken36_sicht.c, probes/r41_masken36.cmake}`, `tools/maske/*`. NB5 selbst (`2ea86d24..HEAD -- re15_port`):
  150 MASKS-Dateien (keine neue Datei, nur geaendert), Ausschluss-Tabelle, re15_pri.h, Tabellen/Riegel/Szenen, platzieren_r41.py, rueckschritt_r41.py.
- Code gelesen: `re15_pri.h` (105 -> 128 mit Beleg), Verbraucher der Konstante (`pri_common.c:125`, `main.c:6031..6095`, `render_pc.c:287/1096/2288`,
  `optik_r39.c:86/107/126`, `masken36_r37.c` (Index < 128), PSX `main.c:253`/`render.c:464`, `masken_gruppen.c:127` RE15_MG_MAX 256), Laeufer
  (Art L, SMIN), `test_r37_masken36.c` (teile, neustellen, aussen, Saaten-Vorgabe `g_saat_tuer = 1`, binaere Modellsuche), `test_r41_masken36.c`
  (platziert, ordner), `platzieren_r41.py` (Kopf-Regeln 1-9, ansicht_spiel/sqrt0, koerper_teil, oberkante, tiefe_teil, cut_bearbeiten/Stufen).

## Bau und Tests (Spurbaum, KEINE volle Suite)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
  (`M41abn5/configure.log`, `build.log`). Baum sauber (`git status` leer), exe vom 17:25:22, Asset-Pfad = Spurbaum (`RE15_ASSETS_PATH` im CMakeCache).
- Unit: `ctest --test-dir re15_port/build -R "^(unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_bss_room_source|prop_bss_chunk|unit_r38_itemorte|unit_r39_optik|unit_r39_titelversion_maske|unit_r34n_e_dokumente|unit_stage6_hintergrund|unit_r19_|unit_r19b_|r19_marke1_10d0_c1|unit_r22_10f0_figur|unit_r30_irons_tisch|unit_1210_gitterhaende|unit_1050)" -j 2 --timeout 2400 --output-on-failure -V`
  -> **`100% tests passed, 0 tests failed out of 45`** (196,7 s; `M41abn5/ctest_unit.log`). Ausgaben selbst gelesen: `teile` "neue Rechtecke 1804:
  gezeichnet 1795 (Tiefe == Modell-Zeile 1795), ausgeschlossen 9 ... deckende Punkte 101280, davon Sehstrahl trifft den Koerper 97032 (95.8 %),
  Eintritt hinter der Tiefe 0"; `platziert` "Sektionen 61 (Rechteckzahl stimmt 61, Elza bytegleich 61), Komponenten 267 (Lage mit dem Spiel-Entpacker
  bestaetigt 267) ... 101280 ... Gelaender der Restliste: 30E0 C6 189, C7 1670, C8 1741, C9 1447, 3050 C6 1224, 3040 C3 5969"; `ordner` (s. Mangel 2 der
  Abnahme 4); `fern` "zu fern 3360 (Stage 3: 2, 4: 72, 5: 270, 6: 3016)" mit ROOM6010/6011 C5 1508 (R113..R124, Zelle @0x0092C); `loecher` 8428 von
  64424 = 13,1 %; `zellen3d` "Beleg im RDT 2820/2820".
- Integration (Regex des Bau-Agenten + die Raumgruppen der geaenderten Raeume ROOM6000/6030/50B0/30E0: `integration_r36_story`,
  `integration_r38_kleinkram_flammenwerfer`, `integration_r37_sperre1050`): `ctest ... -R "^(integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r35_zgirl_5090|integration_r38_affen3|integration_r35_granate|integration_r37_elzaems_elza5091|integration_r36_dachkiste|integration_r37_beretta|integration_r37_itemband|integration_r36_story|integration_r38_kleinkram_flammenwerfer|integration_r37_sperre1050)" -j 3 --timeout 2700 --output-on-failure -V`
  -> **`100% tests passed, 0 tests failed out of 41`** (4 282,6 s; `M41abn5/ctest_integ.log`), parallel zu eigenen exe-Laeufen (Last), kein Riegel rot,
  keine Wiederholung noetig. Szenen-Gruppen: stage2 18, zelle 16, zellen3d 15, fern 15, teile_a1..d2 11/11/13/13/12/12/12/11/11 = **106** Szenen
  (Dossier/Commit sagen 108), alle "Szenen OK"; in allen L-/V-Szenen H = 0.

## RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)
1. **Kapazitaet je Cut FUN_80039270** (Beleg fuer `RE15_PRI_MAX_MASKS_PER_CUT` 105 -> 128): `80039270 lui a1,0x800b` / `80039274 addiu a1,a1,-14468`
   (0x800ac77c) / `80039280 lw a0,-14472(a0)` (0x800ac778) / **`8003928c lbu v0,7(a0)` = `07 00 82 90`** / `80039294 sll v0,v0,2` / `800392a4 lbu v1,7(a0)` /
   `800392ac sll v1,v1,5` / `800392bc lbu v0,7(a0)` / `800392c4 sll v0,v0,5` / `800392d0 sw v0,0(a1)`: Maskenzahl = BYTE, kein Vergleich gegen eine
   Konstante (Grenze 255). 128 ist im Header als PORT-WAHL begruendet (Breite `bits[4]` der Ausschluss-Tabelle). **stimmt.**
2. **Kamera des Spiels / SquareRoot0** (Tiefen von platzieren_r41.py `ansicht_spiel`): `80053d60 jal 0x80065f60` und `80053dd8 jal 0x80065f60` in FUN_80053ca4;
   0x80065f60: `0x4884f000` (mtc2 a0 -> LZCS) / `0x4802f800` (mfc2 LZCR) / `and t2,v0,-2` / `sllv` bzw. `0x01646007` (srav) / `addi t4,t4,-64` /
   **`80065fc4 lh t5,-9852(t5)` = Tabelle @0x8007d984** (`00 10 1f 10 3f 10 ...`) / `sllv t5,t5,t1` / `srl v0,t5,12`. Python `sqrt0()` bildet das nach. **stimmt.**
3. **OT-Stufen** (Regel 6): `80025640 0x4b58002d` (AVSZ3) / `80025648 0xe9870000` (swc2 OTZ) / `80025654 sra v0,v1,6` / `80025658 beq` /
   **`8002565c sra v1,v1,4` (`03 19 03 00`)**; **`80066c70 addiu t0,zero,341`** / `80066c74 0x48c8e800` (ctc2 ZSF3); Masken-OT **`80039650 lh a0,2(s3)`
   (`02 00 64 86`)** / `80039658 sll a0,a0,2` / `8003965c addu a0,a0,s6` / `80039660 jal 0x8006b538`. **stimmt.**
4. **Tuer-Band der Saaten**: `80042cac lbu v0,2(s0)` / `80042cb4 andi v0,v0,0x80` / `80042cb8 bne v0,zero,0x80042cd4` / `80042cc0 lbu v1,130(s1)` /
   `80042cc4 lbu v0,2(s0)` / **`80042ccc bne v1,v0,0x8004301c` (`d3 00 62 14`)**. Zusaetzlich selbst: warum Satz[2] = pc[4]: Opcode-Tabelle
   0x800744a8 (0x800745bc = Eintrag 0x45 -> 0x800428d4) -> Eintrag 0x3B @0x80074594 -> **Door_aot_set 0x800405bc**: `800405bc lw v0,28(a0)` (pc) /
   `800405c4 lbu v0,1(v0)` / `800405cc addiu v1,v1,-13904` (0x800ac9b0) / **`80040608 addiu v0,v0,2` / `8004060c sw v0,0(v1)`** = Satzzeiger pc+2,
   also Satz[2] = pc[4] (Port `scd_vm.c` `door_params[slot].band = t->pc[4]`). **stimmt.**
- Zellbytes selbst im RDT gelesen (eigener SCA-Leser `py/geo.py`): ROOM30E0 @0x005E8 `e4 57 d0 07 ef 06 ee 1b 01 ff 00 03`, ROOM6010 @0x0092C
  `31 0b 2d 19 31 b2 dc a1 01 ff 00 13`, ROOM3040 @0x009C0 `d0 07 d0 07 94 c6 0c e5 05 ff 00 13` = Modell-Zeilen der Tabelle.
- Diff NB5 (`2ea86d24..HEAD -- re15_port`, ohne Assets) auf deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|geschaetzt|vorerst|ungefaehr|
  sieht richtig|platzhalter|heuristi: **0 Treffer**; Dossier NB5 ebenso 0; **0 neue getenv** im ganzen Zweig. Neue Konstanten: 128 (belegt, PORT-WAHL),
  Riegel-Schwellen = gemessene Endstaende (R41_PLATZ_MIN, R41_TEILE_TREFFER_MIN, R41_ORDNER_GRUEN_MIN, fern 3360/2/72/270/3016), Werkzeug-Schwellen
  RMS_MAX 18,91 / EINDEUTIG 1,5 (an 422 Generator-Teilen kalibriert, Kopf), FARBE_MAX 48 (R37), IM_BILD 0,9 / MIN_TEXEL 16 (Werkzeug-Wahl ohne
  Herleitung, kein Spielverhalten).
- **RE-Gate haelt.**

## Vertrag (VERTRAG.md §1.3/§2)
- Pfade (`git diff 4c667c97..HEAD --name-only`): nur Spur-Dateien, `shared_assets/PSX/MASKS/` (fuer M erlaubt), keine `release/`, `platform/android/`,
  `pri/`, `synchro/`, keine Test-CMakeLists (Registrierung nur in `tests/unit/probes/r41_masken36.cmake`). Gemeinsame Datei `include/re15_pri.h`:
  1 Konstante + Kommentar "Runde 41 Spur M". Keine Bank-9-Bits, Nachrichten-Ids, Slots, Ereignisse (M hat keine Zuteilung).
- Leon/Elza: alle 123 Dateipaare ROOMxxx0/xxx1 der Stages 3..6 in MASKS bytegleich (selbst verglichen). Keine neue MASKS-Datei in NB5 (Paket-Gate
  unveraendert gegenueber NB0..NB4).
- Neue Riegel parallel-/Linux-fest: eigenes exe-Praefix `r41m8<g>` je Gruppe, eigene Laufordner, `m` gelinkt, Laeufer mit `cmake_minimum_required`.
- **Vertrag haelt.**

## Punkt 1 - "Masken fuer STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" (gemessen an M-E2 und den Maengeln 1/2 der Abnahme 4)

### 1.1 Auflagen aus M-E2 / Abnahme 4 Mangel 1 - was am gebauten Stand gilt
| Auflage | Befund (selbst geprueft) |
|---|---|
| Teile des Original-Atlas einsetzen, je Teil Lage (Bildvergleich) + Tiefe (Zelle mit RDT-Offset) | 267 Teile / 61 Kameras / 1 804 Rechtecke; Riegel `platziert` (Lage mit dem Spiel-Entpacker, 267/267) und `teile` (C-Gegenrechnung mit der Engine-Kamera: 0 Verstoesse an 97 032 Treffern) gruen; 3 Zellbelege selbst im RDT gelesen; an der exe s. 1.3/1.4 |
| Tuer-Band der Saaten mitnehmen | `g_saat_tuer = 1` (Vorgabe); RE selbst belegt inkl. Door_aot_set (RE-Gate 4); 6000/6001 C5/C7/C8 je 37 alte Rechtecke wieder gezeichnet (statisch: 6000 C7 +9 139 Punkte neben 1 352 Teile-Punkten) |
| Kapazitaet je Cut belegt | @0x8003928c Byte (RE-Gate 1); 128 = PORT-WAHL, 35 Sektionen voll |
| HINTER- und DAVOR-Stelle je Kamera an der exe (Leon, Elza wo erreichbar) | 106 Szenen gruen; eigene Neubewertung aller 40 HINTER-Szenen (1.4): 38 messen die Spielfigur und zeigen sie verdeckt, **4040 C12 (Leon + Elza) misst keine Spielfigur** (Mangel 3) |
| Rueckschritt-Suche wie NB3 | eigener statischer Vergleich (1.2): 0 Punkte weg, 0 Tiefen geaendert |
| Riegel | teile, platziert, ordner, teile_a1..d2, fern/loecher/zellen3d nachgezogen - alle gruen |
| teilen_r41.py NICHT | nicht angewandt: ROOM6000 C5 alte Rechtecke bytegleich (1.2), R37 Tiefe 176 unveraendert |
| Freistellungs-Liste neu | vorhanden, Zeilen passen zum ausgelieferten Stand; der verwiesene Teile-Bericht nicht (Mangel 2) |

### 1.2 Statischer Rueckschritt-Vergleich NB3 (2ea86d24) -> NB5 (HEAD), eigener Leser (`M41abn5/py/rueck.py`, mskio wie Lader + Renderer, Kapazitaet 128)
Alle 216 Generator-Sektionen der Stages 3..6 (Leon + Elza), je Bildpunkt gezeichnete Deckung und vorderste Tiefe vorher/nachher:
**weg 0**, neu 233 020 (= 2 x 116 510 = Zahl der NB5 je Variante: 101 280 Teile + 15 230 Lok-Rechtecke 6000 durch das Tuer-Band), **ferner 0, naeher 0**,
122 Sektionen geaendert; alte Rechtecke (Index < erstes) **Saetze bytegleich**; Farbe alter Texel nach dem Paletten-Neubau (Regel 8): 20 von 1 967 248
Punkten > 24 (vernachlaessigbar). Ergebnis `M41abn5/rueck.txt`.

### 1.3 Eigene Szenen an der exe (Messkopie `M41abn5/exe/re15_pc_m41abn5.exe`, md5 84fd9c2f = Bau-exe)
Laeufer `M41abn5/run5.sh` (RE15_DEBUG_JUMP=<raum>@250, RE15_PLAYER_POS, RE15_FORCE_CUT, RE15_FRAMEDUMP=120, RE15_EXIT_AT=120#<raum>; gueltig nur mit Framedump +
EXIT_AT NACH dem letzten Laden des Zielraums; alle Laeufe im 1. Versuch gueltig). Je Szene 4 Laeufe: A mit Masken, B ohne (RE15_NO_PRI), A0 mit Masken +
Figur ausserhalb des RVD-Vierecks, D ohne Masken + Figur draussen. Auswertung `py/szene5.py`: F (Spielfigur) = |B - D| > 24, sichtbar |A - A0| > 24,
uebermalt F & |A - B| > 24, N = Deckung NUR der neuen Rechtecke; Kontrolle |D - A0| ausserhalb jeder Maske = 0 in allen Szenen. Stellen aus den
erreichbaren Stehplaetzen (`test_r37_masken36 plaetze`, Netz der Spur P) mit EIGENER Projektion gegen N (`py/stellen5b.py`). Bilder `M41abn5/bilder/<name>.png`.

| Szene | Raum/Cut, Gegenstand | Platz, Band | Art | Figur | Figur auf N | davon verdeckt | davon sichtbar | uebermalt (alle / durch N) |
|---|---|---|---|---|---|---|---|---|
| h30E0_08 (Platz t30E0_08) | 30E0 C8 Steg-Gelaender | (25300,10700) 0 | HINTER | 566 | 135 | 135 | 0 | 134 / 134 |
| e30E0_08h | 30E0 C8 | (26700,11900) 0 | HINTER | 428 | 111 | 111 | 0 | 108 / 108 |
| e30E0_08v | 30E0 C8 | (20700,6500) 0 | DAVOR | 2544 | 343 | 5 (farbgleich) | 338 (Figur davor) | **0** / 0 |
| e30E1_08h **Elza** | 30E1 C8 | (26900,11200) 0 | HINTER | 313 | 76 | 76 | 0 | 76 / 76 |
| e30E1_08v **Elza** | 30E1 C8 | (21700,6600) 0 | DAVOR | 1321 | 140 | 0 | 140 | **0** / 0 |
| g3040_03h1 | 3040 C3 Laufsteg-Gelaender | (-12000,-6698) 1 | HINTER | 746 | 80 | 80 | 0 | 79 / 79 |
| g3040_03v1 (Gegner Akteur 2 weg) | 3040 C3 | (-11400,-13598) 1 | DAVOR | 424 | 66 | 0 | 66 | **0** / 0 |
| e3050_06h | 3050 C6 Treppengelaender | (-4700,-7668) 4 | HINTER | 248 | 130 | 130 | 0 | 125 / 125 |
| e6000_07v | 6000 C7 Lok (Tuer-Band) | (18333,-20584) 2 | DAVOR | 3342 | 269 | 1 | 268 | 3 / **0** |
| e6010_02h | 6010 C2 Saeulen der Plattform | (-21912,-9520) 0 | HINTER | 374 | 335 | 335 | 0 | 335 / 335 |
| e6010_02v | 6010 C2 | (-14712,-6320) 0 | DAVOR | 1746 | 836 | 2 | 834 | **0** / 0 |
| e6010_05h | 6010 C5 | (-18812,-19320) 0 | HINTER | 500 | 143 | 143 | 0 | 139 / 139 |
| e5090_12v | 5090 C12 | (16617,-22265) 0 | DAVOR | 8377 | 2650 | 3 | 2647 | 74 / **0** |
| e30A0_04h | 30A0 C4 Buerostuhl | (-25443,17663) 0 | HINTER | 2483 | 176 | 153 | 23 (Leon VOR dem Rueckwand-Tisch, s.u.) | 186 / 146 |
| e30A0_04v | 30A0 C4 | (-22443,17663) 0 | DAVOR | 9362 | 1127 | 32 | 1095 | 4 / **0** |
| e30A1_04v **Elza** | 30A1 C4 | (-22443,17663) 0 | DAVOR | 7355 | 1301 | 2 | 1299 | 4 / **0** |
| e40A0_07v | 40A0 C7 Schrank unten | (-18900,-25301) 0 | DAVOR | 7750 | 769 | 5 | 764 | **0** / 0 |
| e5050_02v | 5050 C2 Pult (Kamera ohne DAVOR laut NB5) | (-21400,-23550) 0 | DAVOR | 7020 | 398 | 1 | 397 | **0** / 0 |
| **o6010_05** (OFFEN 18) | 6010 C5 Treppenwange Teil 8 (R113..R124) | (-19912,-19720) 0 | HINTER | 138 | 96 auf Teil 8 | 14 | **82** | 14 / 14 |

- **HINTER:** an allen eigenen Stellen ausser o6010_05 ist jeder Figurpunkt, vor dem ein neues Teil steht, verdeckt (Leon und Elza). e30A0_04h: die 23
  sichtbaren Punkte liegen auf R42/R46/R50 (Zelle @0x00474 Rueckwand-Tische, Tiefe 133..138), Leon hat dort Stufe 85..91 = Leon steht VOR diesem Teil
  (richtig sichtbar); der Stuhl davor verdeckt 146 Punkte seiner Beine.
- **DAVOR:** an allen eigenen Stellen 0 Punkte der Spielfigur von NEUEN Teilen uebermalt. Die wenigen uebermalten Punkte (5090 C12 74, 30A0/30A1 C4 4,
  6000 C7 3) liegen unter ALTEN Rechtecken (5090: R0/1/4/10/57/67; 30A0: R40/41) - statisch seit NB3 unveraendert (Altbestand, OFFEN 7 der Spur);
  einzige NB5-Folge: 1 Randpunkt in 6000 C7 unter R13, den das Tuer-Band wieder zeichnet (vorher ausgeschlossen) - vernachlaessigbar.
- 3040 C3: der erste DAVOR-Lauf zaehlte 6 "uebermalte" Punkte - das war der Gegner EM29 (Akteur 2, reagiert auf Leon); mit `RE15_GEGNER_POS=2,...`
  weggestellt: 0. Zwei eigene Band-0-Kandidaten in 3040 C3 sind laut Netz nicht erreichbar - verworfen.
- 3050 C6 Band 4 (-5500,-8968) ("e3050_06v", kein gueltiger DAVOR-Fall): R19 (Tiefe 175, Zelle @0x00A68) malt 39 Punkte ueber Leons Fuesse. Leon steht auf
  der oberen Ebene, die Kamera (-11786,-2482,-18936) schaut von unten, die Band-4-Ebene reicht von Leon aus >= 2 400 Richtung Kamera (erreichbare Plaetze
  durchgehend) - die Deckenplatte liegt VOR seinen Fuessen; kein Mangel (Bild `bilder/e3050_06v_zoom.png`; geerbte Tiefe s. Hinweis H2).

### 1.4 Neubewertung aller 40 HINTER-Szenen (Art M) des Bau-Agenten - nur die Spielfigur
Der Laeufer zaehlt bei Art M "Figurpunkte" als S = |B - C| mit C = statischer Hintergrund (probe_bg_dump) und verlangt nur H > 0. Ich habe jede der 40
M-Szenen (teile_a1..d2, Leon + Elza) mit eigenem Laeufer neu gefahren (`M41abn5/batch_m.sh`: mit/ohne/leer0, A0 aus den aussen-Listen des Bau-Agenten,
120 Laeufe gueltig) und im Szenenfenster getrennt: S_fig = |B - D| > 24 (nur Spielfigur), fremd = |D - C| > 24 (Objekte/Akteure, die nicht die
Spielfigur sind). Ergebnis `M41abn5/m_fenster.txt`, Tiefen-Einordnung `m_tiefe.txt` (`py/m_auswertung.py`, `m_auswertung3.py`):
- **38 Szenen** messen die Spielfigur und zeigen sie verdeckt (H_fig > 0, z.B. t30E0_08 291/133, t3050_12 380/364, t6010_02 684/519, t6010_02e 510/434).
  Sichtbare Figurpunkte auf neuen Teilen sind erklaert: Figur VOR einem anderen Teil desselben Cuts (Tiefe > Stufe+6, z.B. t3040_03 154, t6010_06 446),
  Fremdpixel (3040: zweiter EM29 = Akteur 1; mit `RE15_GEGNER_POS=1,...` weggestellt zeigt t3040_05 234/234 verdeckt, 0 sichtbar; 5090 C11: die
  Feuer-Effekte - an allen 313/180 Punkten zeichnet die Maske (A != B), Leon ist dort nicht zu sehen, Bild `bilder/M_t5090_11_zoom.png`) oder
  hoechstens 4 Randpunkte (t3040_02, t6010_02/e).
- **t4040_12 und t4040_12e (ROOM4040/4041 C12): S_bau 440 / H_bau 20, aber S_fig = 0, fremd = 440.** Die 440 "Figurpunkte" des Fensters sind das
  3D-Tormodell links (im Hintergrund-Bild ist das Tor offen, im Spiel steht das geschlossene 3D-Tor davor - Bild `bilder/b4040_12_DC.png`); die Spielfigur
  liegt nicht im Fenster. An der Stelle (-4000,4678) ist Leon ganz von ALTEN Masken verdeckt (377/377), das neue Teil (schmaler Streifen am Torrahmen)
  beruehrt ihn nicht (Figur auf N = 0, Leon und Elza). Die NB5-Zeile "4040 | 12 | 361/9 ... M 361/9" belegt also KEINE verdeckte Spielfigur (Mangel 3).

### 1.5 Natuerlicher Weg durch die Tuer (ohne RE15_FORCE_CUT)
Tuer ROOM3030 -> ROOM30E0 selbst aus dem RDT gelesen: Door_aot_set @RDT+0x1D2 `3b 00 02 31 00 00 cc 93 98 bd a0 0f e8 03 71 1b 00 00 8c 0a 00 0c 02 0e 06 ...`
(AOT x -27700 z -16996 4000 x 1000; Ziel (7025,0,2700), Gier 3072, Raum 0x0E, Cut 6). `M41abn5/tuer5.sh`: Sprung nach 3030 (-25700,-16500) Gier 3072,
Skript `W1,A0.3,W6,R0.36,U8.5,L0.36,U4.2,W2`, Framedump alle 10 Bilder; einmal mit, einmal ohne Masken - bildgleiche Wege (`[walk]`-Zeilen). Log: `loaded
room3030`, `loaded room30e0`, Kamera **Cut 6 -> 7 -> 8 -> 9** (`[pri] cut=6 masks=22`, `cut=7 masks=75`, `cut=8 masks=85`, `cut=9 masks=58`). In Cut 8
(F420..F580, C = eigener Hintergrund `bg3/ROOM30E08.ppm`): je Bild 115..253 Figurpunkte auf den neuen Gelaender-Teilen, **alle verdeckt, 0 sichtbar** -
hinter dem Steg-Gelaender stehen ein Zombie (nur ueber die Tuer, nicht beim Sprung) und ab F550 Leon (25895, 8359..10609). Bild `bilder/nat1_30E0_C8.png`
(oben ohne, unten mit Masken).

### 1.6 OFFEN 18 an der exe bestaetigt (6010/6011 C5)
Leon auf dem erreichbaren Band-0-Platz (-19912,-19720) unter der Treppe an ihrer Westkante (Platz gefunden mit `py/loch18b.py`): von 96 Figurpunkten auf
Teil 8 (R113..R124, Zelle @0x0092C x -19919..-17054 z -24100..-17655 Band 1, Hoehe 2311) sind **82 sichtbar** - Kopf und Schulter liegen im Bild ueber der
neuen Treppenwange (`bilder/o6010_05_voll.png`, `o6010_05_zoom.png`). Ursache aus dem Lauf pl_a3 des Bau-Agenten nachgelesen: 6010 C5 hat 14 alte + 114 neue
= 128 Rechtecke (Stufe 0, je Bildspalte EINE Tiefe = Maximum der Spalte, Teil-8-Tiefen 112..126); die naeheren Punkte einer Spalte bekommen die ferne
Spaltentiefe, Teilen der Spalten verhindert die PORT-Kapazitaet 128 - das Original erlaubt 255 (@0x8003928c Byte). Kein Rueckschritt (vorher keine
Maske), aber ein sichtbares Loch in einem NEU eingesetzten Teil (Mangel 1).

### 1.7 Mangel 2 der Abnahme 4 (roter Ordner ROOM30A0/30A1 Cut 0) - erledigt
- Original ohne Masken: `info/Re1.5/PSX/STAGE3/ROOM30A0.RDT` und `ROOM30A1.RDT` (beide md5 5c3baa6e, bytegleich mit shared_assets): Kamerasatz @0x60, Feld
  +0x1C der Cuts 0..3 = 0x3C0/0x3C4/0x3C8/0x3CC, dort je `ff ff ff ff`; Kopfbyte 7 = 0.
- Eigener Abzug des Kuenstler-Atlas ueber den Engine-Entpacker (`probe_p2_atlas_dump` -> `M41abn5/atlas3/ROOM30A0_ORIG00.TIM`): 40 rote Bildpunkte im Fenster
  x 310..317 y 106..111 (sehr dunkles Rot). Suche ueber ALLE Verschiebungen, die Atlas-Texel farbgleich (<= 48) auf diese Punkte legen: beste (161,-4) /
  (162,-8) mit 32 von 40 Treffern, beide Komponente 9 (7 028 Texel), die dort nur zu 16,7 % auf den Hintergrund passt (RMS 97,9; Werkzeug-Schwelle 18,91)
  = zufaellige dunkle Texel. **Kein Atlas-Teil zeigt den roten Ordner** - die Aussage der NB5 stimmt; Eintrag in der Nutzer-Liste (40 Punkte, Tiefe aus
  Zelle 6 @RDT+0x0045C) vorhanden; Riegel `unit_r41_masken36_ordner` gruen (Ausgabe gelesen: "roter Ordner: 40 ..., im Kuenstler-Ausschnitt 0, eingesetzte
  Teile dort 0, gezeichnet (Leon + Elza) 0"). Die Riegel-Szene "Art Z, V == 0" der Abnahme-4-Erwartung entfaellt folgerichtig (setzt eine Maske voraus).

### 1.8 Nutzer-Liste "Fuer den Nutzer: Freistellungen" und Teile-Bericht
- Die ausgelieferten Masken stammen aus dem Lauf **pl_a3** des Bau-Agenten (Scratchpad des Bau-Agenten
  `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/m41nb5/pl_a3`, kurz `m41nb5/pl_a3`:
  150 von 150 Dateien bytegleich mit MASKS/; Kamera
  des Spiels). Der Teile-Bericht im Repo `analysis/befunde_runde41/M_bilder/NB5/teile_nb5.txt` (Kopf: "Lauf pl_a2") stammt aus dem Lauf davor mit der alten
  Kamera `geom.cut_view` (pl_a2: 138 von 148 Dateien weichen ab). Vergleich je Teil (`pl_a3/platzieren_r41.json` gegen die Datei): **21 Teile haben im
  ausgelieferten Stand das andere Ergebnis**: 3040 C1 T1, C3 T6/T21/T42, C7 T14/T18, C9 T17, C11 T8, C13 T13; 3050 C5 T14, **C11 T2** (3 031 Punkte: Datei
  "Zelle @0x00A98", im Spiel NICHT eingesetzt, Nutzer-Liste "Z"); 30A0 C3 T5; **40A0 C7 T4** (Datei "Zelltyp 8 Kapsel", im Spiel mit Zelle @0x00D18
  eingesetzt); 5090 C12 T17, C14 T3/T5; 6000 C8 T7; **6010 C5 T8** (Datei "OHNE", im Spiel eingesetzt = das Teil von OFFEN 18), C7 T15, C12 T9, C13 T4.
  Summen: Datei 266 mit / 278 ohne Koerper, Dossier und Spiel 267/277; 34 der 61 Sektionskoepfe der Datei stimmen nicht. Die Nutzer-Liste verweist fuer
  Nummern und Gruende ausdruecklich auf diese Datei ("Nummern T.. wie in M_bilder/NB5/teile_nb5.txt", OFFEN 17 "Grund je Teil in ...teile_nb5.txt").
- Die 62 Zeilen der Nutzer-Liste selbst passen zu pl_a3 (Punkte und T-Nummern gleich); 8 Sektionen mit Teilen ohne Koerper fehlen ohne Angabe einer
  Schwelle (alle < 300 Punkte): 3040 C10 (218), 3050 C1 (106), 3050 C6 (209), 5050 C1 (275), 5050 C2 (174), 6000 C7 (172), 6000 C9 (108), 6030 C12 (139).
- Ursache der Abweichung (selbst nachgerechnet): zwischen pl_a2 und pl_a3 aendert sich am Werkzeug nur die Kamera (`git diff d789f76e..13e8b150`:
  `geom.cut_view` -> `ansicht_spiel`). Die beiden Kameras liegen mehrere Bildpunkte auseinander (3050 C11: Rotation 0,006, Verschiebung (-132, 0, 71);
  40A0 C7 (75, 54, -106)) - mehr als die "eine OT-Stufe" aus OFFEN 19. pl_a3 rechnet mit der Kamera des Spiels (C-Gegenrechnung `teile` mit der
  Engine-Kamera: 0 Verstoesse); die ausgelieferten Teile sind also richtig gerechnet - nur der Bericht ist veraltet.

## Punkt 2 - "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung im Spiel, Abweichungen melden)
- NB5 beruehrt keine STAGE1/2-Maske (`git diff 2ea86d24..HEAD --name-only -- re15_port/shared_assets/PSX/MASKS | grep ROOM[12]` leer); seit der merge-base
  nur 2030 C7, 2040 C7, 20A0 C12 (NB0/NB1, in den Abnahmen 1-4 abgenommen). 5924939d ist Vorfahr des Spurbaums; `git log 5924939d..master -- pri/` = nur
  919cc1ed (Orchestrator), `git status pri/` im Hauptbaum leer: keine neuen Nutzer-PRIs.
- Die Kapazitaetsaenderung 105 -> 128 wirkt auf STAGE1/2 nicht: Original-Sektionen <= 105, Port-Sektionen der Stages 1/2 selbst gezaehlt (144 Sektionen
  in MASKS/, Summe der Gruppenzahlen) hoechstens 105 - nichts wurde bisher gekappt, nichts zeichnet jetzt mehr; `integration_pri_masken` "groesste
  Sektion 128" (Stage 3..6), 0 unparsbar, 0 ueberschreiben ein Original.
- Riegel `integration_r41_masken36_stage2` gruen ("18 Szenen OK", 463,8 s) und `unit_r41_masken36_kurbel` gruen.
- Eigene Stichprobe mit **Elza** (ROOM2041 Cut 9, Zwillingsmasken ROOM2040, eigener Laeufer + `py/szene5.py`, Hintergrund `bg2`): HINTER (4500,-16000) Band 3:
  Figur 604, sichtbar 19, uebermalt 548 (Elza hinter dem Holzgestell verdeckt, Bild `bilder/s2041_09h.png`); DAVOR (5700,-10200) Band 3: Figur 3023,
  **uebermalt 0**. Wie Abnahme 4 (Hinweis H2 zu 2050 C0 gilt unveraendert).
- **Urteil Punkt 2: erfuellt** (Stand unveraendert, keine neue Abweichung).

## Urteile je Punkt (gegen den Wortlaut; AUFTRAG.md massgeblich inkl. M-E1/M-E2/Nachtrag 32b)
| Punkt | Urteil | Kern |
|---|---|---|
| 1 "Masken fuer STAGE3-6 ... ueberall Loecher" -> "Masken schliessen" | **teilweise** | M-E2 umgesetzt und an der exe getragen (eigene Szenen Leon/Elza: HINTER verdeckt, DAVOR 0 uebermalt; natuerlicher Weg durch die Tuer mit Kamerawechsel; statisch 0 weg / 0 Tiefen geaendert; Tuer-Band, Kapazitaet, Door_aot_set selbst belegt; roter Ordner zu Recht beim Nutzer). OFFEN: sichtbares Restloch in einem neu eingesetzten Teil (6010/6011 C5, OFFEN 18, Mangel 1); der Teile-Bericht der Nutzer-Liste ist veraltet (Mangel 2); der HINTER-Beleg 4040 C12 misst keine Spielfigur (Mangel 3) |
| 2 "neue PRIs fuer STAGE2 ... alle sauber eingebaut" (Sichtpruefung) | **erfuellt** | unveraendert seit NB1, pri/ unveraendert, Riegel stage2 gruen, eigene Elza-Stichprobe HINTER verdeckt / DAVOR 0 |

## Gates
- RE-Gate: **haelt** (4 Stellen + Door_aot_set selbst disassembliert, Zellbytes selbst gelesen, keine Rate-Woerter, keine neuen Schalter).
- Vertrag: **haelt** (Pfade, keine Zuteilungen noetig, gemeinsame Datei nur 1 Konstante mit Kommentar).
- Tests: unit 45/45, Integration 41/41.

## Maengel (nummeriert, nachpruefbar) - alle NICHT blockierend im Sinn von Nachtrag 32b
1. **Restloch in einem neu eingesetzten Teil (Punkt 1, OFFEN 18 an der exe bestaetigt).** ROOM6010/6011 Cut 5, Teil 8 (Rechtecke 113..124, Zelle @0x0092C):
   Leon auf dem erreichbaren Band-0-Platz (-19912,-19720) unter der Treppe - 82 von 96 Figurpunkten auf der neuen Treppenwange sind sichtbar (Kopf/Schulter
   ueber der Wange; `M41abn5/bilder/o6010_05_voll.png`, Szene `runs/o6010_05_*`). Ursache: je Bildspalte eine Tiefe (Spaltenmaximum) und der Cut ist mit
   128/128 voll - die Port-Kapazitaet 128 (PORT-WAHL, Breite `bits[4]`) liegt unter der Original-Grenze 255 (@0x8003928c Byte). Nachpruefbar: run5.sh
   `o6010_05 6010 5 -19912 -19720 0` mit/ohne/leer/leer0, Teil-8-Punkte |A - A0| > 24. Weg: Ausschluss-Tabelle auf `bits[8]` und Kapazitaet bis 255,
   Spalten von Teil 8 teilen; sonst die Treppenwange 6010/6011 C5 in Nutzer-Liste und Release Notes. Kein Rueckschritt (vorher keine Maske).
2. **Teile-Bericht der Nutzer-Liste ist von einem frueheren Lauf (Punkt 1, Dokumentation fuer den Nutzer).** `analysis/befunde_runde41/M_bilder/NB5/teile_nb5.txt`
   stammt aus Lauf pl_a2 (alte Kamera), die Masken aus pl_a3: 21 Teile mit anderem Ergebnis (Liste 1.8, u.a. 3050 C11 T2 "Zelle @0x00A98" laut Datei, im
   Spiel nicht eingesetzt; 6010 C5 T8 und 40A0 C7 T4 laut Datei "OHNE", im Spiel eingesetzt), Summen 266/278 statt 267/277, 34 von 61 Sektionskoepfen falsch.
   Die Nutzer-Liste verweist fuer T-Nummern und Gruende auf diese Datei. Dazu: 8 Sektionen mit Teilen ohne Koerper (106..275 Punkte) fehlen in der Liste
   ohne genannte Schwelle; Dossier/Commit nennen 108 exe-Szenen, die Szenendateien enthalten 106. Nachpruefbar: Vergleich `teile_nb5.txt` gegen
   `m41nb5/pl_a3/platzieren_r41.json` (Skript `M41abn5/py/teile_vergleich.py`, Ausgabe `M41abn5/teile_vergleich.txt`). Weg: Bericht aus pl_a3 neu
   schreiben, Schwelle nennen oder die 8 Sektionen aufnehmen.
3. **HINTER-Beleg ROOM4040/4041 Cut 12 misst keine Spielfigur (Punkt 1, Riegel-/Dossier-Aussage).** Szenen `t4040_12`/`t4040_12e`
   (`test_r41_masken36_teile_szenen_b*.cmake`, Platz (-4000,4678)): im Fenster x 72..88 y 70..96 sind die 440 "Figurpunkte" (|B - C|) das 3D-Tormodell
   (|D - C| = 440 auch ohne Figur, |B - D| = 0); H > 0 entsteht dort, nicht an Leon/Elza. An der Stelle ist die Figur ganz von ALTEN Masken verdeckt, das neue
   Teil beruehrt sie nicht (Figur auf N = 0). Die Zeile "4040 | 12 | 361/9 ... M 361/9" im Dossier und "jede HINTER-Stelle verdeckt" gelten fuer 4040 C12
   nicht. Weg: Art M wie Art L gegen den figurlosen Lauf D zaehlen (S = |B - D|), 4040 C12 in die Liste "ohne HINTER-Stelle" (OFFEN 21) oder eine gueltige Stelle.

## Hinweise (keine Maengel)
- H1 (OFFEN 19): die alte Python-Kamera `geom.cut_view` weicht von der des Spiels um mehrere Bildpunkte ab (Verschiebung bis 132 Einheiten) - mehr als die
  im Dossier genannte "eine OT-Stufe"; die Tiefen der NB1..NB3-Werkzeuge sind damit gerechnet und nur durch den Ausschluss (C, Engine-Kamera) gegen
  Uebermalung geprueft.
- H2 (Regel 6, geerbte Tiefen): Punkte eines Teils, deren Sehstrahl den Koerper verfehlt (4 248 von 101 280), erben die Tiefe des naechsten Treffers - z.B.
  3050 C6 R19 an Leons Fuessen auf der oberen Ebene; dort ist die Deckenplatte tatsaechlich davor, ein Fehlbild habe ich nicht gefunden.
- H3: an DAVOR-Stellen uebermalen ALTE Rechtecke wenige Randpunkte (5090 C12 74 Punkte unter R0/1/4/10/57/67, 30A0/30A1 C4 4 unter R40/41, 6000 C7 2) -
  Altbestand seit NB3 (OFFEN 7); neu durch das Tuer-Band nur 1 Punkt unter R13 in 6000 C7.
- H4: Laeufer Art L/M: Gegner reagieren auf die Spielfigur (3040: zwei EM29, Akteur 1 und 2), Feuer-Effekte animieren (5090) - |B - D| enthaelt dann
  Fremdpixel. Bei L macht das die Pruefung nur strenger; bei M kann es H > 0 vortaeuschen (Mangel 3 mit einem 3D-Tor). Fuer die Integration: Gegner per
  `RE15_GEGNER_POS` wegstellen (nur ein Akteur je Lauf moeglich).
- H5: `platform/pc/src/render_pc.c:287` Kommentar "= 105, the data maximum" ist veraltet (jetzt 128); PSX bekommt +23 x 14 Byte statisches `s_pri_cut`
  ohne Nutzen (platform/psx liest MASKS nicht, OFFEN 4).
- H6: ROOM30E0 Cut 8: (26700,11900) liegt in der Umschaltzone 8 -> 9 (RVD z 11500..12500); die Szene t30E0_08 (25300,10700) ist eine natuerliche Cut-8-Stelle.

**Ergebnis: nicht bestanden** - Punkt 1 teilweise (Maengel 1-3, alle nicht blockierend: kein Absturz, kein Rueckschritt, keine Uebermalung einer Figur VOR
einem Gegenstand), Punkt 2 erfuellt. Fuer die Integration nach Nachtrag 32b ("gut genug") gibt es aus Sicht dieser Abnahme keinen blockierenden Fehler; die
Maengel 1-3 gehoeren in die Release Notes ("Ab Samstag") bzw. sind kleine Doku-Nacharbeiten.
