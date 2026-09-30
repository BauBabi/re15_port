# Spur F (Leichen ROOM1110/ROOM1230) — ABNAHME 1

Stufe: Abnahme 1 (unabhaengig, nach dem Bau). Pruefer hat den Code nicht geschrieben.
Ziel: WIDERLEGEN, dass Spur F fertig ist.

## 0. Urteil

(offen — wird am Ende gesetzt)

## 1. Stand (git log, Dossier, Gegenpruefung)

- Zweig `r34n/leichen`, Kopf `d0611644` (wip beim Limit-Abbruch) auf `51bb6d3d` (Bau-Fix cherry-pick).
  Bau-Commits `67404748`..`3763f50d` (Modul, zwei Haken, Riegel 12 Teile, Mutationsprobe, Abnahmelaeufe).
- Code: `re15_port/include/re15_leiche.h`, `re15_port/engine/src/leiche_1110_1230.c` (neu);
  Haken `scd_vm.c` op_message_on (1 Anweisung hinter dem Stimmen-Riegel, Z. 1775) und
  `game_step_common.c` (1 Anweisung nach `re15_granate_tick`, Z. 1056).
- Dossier `F_leichen.md` Abschnitt 9: 9.5 "Suite" steht noch auf "(laeuft)" — der letzte Suite-Lauf des
  Bau-Agenten (`build/r34n_f_suite.log`, 05:39) hatte 2 Ausfaelle: `integration_r30_granate_laden`
  (exit=1 ohne Meldung = Fremd-Kill-Muster) und `unit_r30_tuer_verschlossen` ("Tabellen-Plaetze 18 statt
  51"). Den zweiten hat der Bau-Agent durch eine Aenderung an `tests/unit/test_r30_tuer.c` behoben
  (Commit `d0611644`, im Dossier NICHT erwaehnt, s. 5 M3).
- Gegenpruefung (`F_leichen.gegenpruefung.md`): 7 Auflagen, laut Dossier 9.2 alle umgesetzt.

## 2. Eigener Bau + Suite-Zeile

`bash re15_port/tools/local_build.sh` im Baum (08:17-08:31, parallel liefen die Suiten von r34n_rolltor
und r34n_generator):

    100% tests passed, 0 tests failed out of 440
    === LOCAL-BUILD-OK (all) — Tests 440/440

= Basis 428 + 12 neue Teile `unit_r34n_f_leiche_*` (#406-#417, alle gruen). Die zwei Ausfaelle des
Bau-Agenten sind im eigenen Lauf gruen: `integration_r30_granate_laden` (143 s) und
`unit_r30_tuer_verschlossen` (18,5 s); ebenso `integration_keypad`, `unit_r26_inventar` (Stapeln),
`unit_r33_tuer1120_*` (anderer Text-Platz-Haken). Exe `re15_pc.exe` md5 `0501f987...` = Stand HEAD
(Ninja baute nur `probe_r34n_f_leiche` neu), fuer die Messlaeufe kopiert nach
`build/abn1_f/bin/re15_pc_abn1f.exe` (eigener Name, local_build.sh beendet sie nicht).
Syntax-Probe des neuen Moduls ohne und mit `RE15_PLATFORM_PC` (`gcc -std=c99 -Wall -Wextra
-fsyntax-only`): 0 Warnungen, 0 Fehler (PSX-Zweig uebersetzbar).

## 3. Nutzerpunkte an der echten exe

Alle Laeufe: eigene Kopie der Bau-exe, `RE15_WINDOW_SCALE=3`, `RE15_NO_INTRO`, Titel-Autovorlauf,
Eingaben per `RE15_INPUT_SCRIPT` (Tasten wie ein Spieler, Generator `skript.py` im Scratchpad: Bild ->
Token), Framedumps ANGESEHEN (Bogen-Bilder unter `F_belege/abn1_*.png`), Protokollauszug aller Laeufe
`F_belege/abn1_laeufe.log` (debug.log-Zeilen `[msg]`/`[leiche]`/`DOOR FIRE`/`[save]` + Zustaende der
modal.log). Nutzerwortlaut: AUFTRAG.md Z. 49 und Z. 61-64.

### 3.1 ROOM1110 — Text + Angebot, Nein -> wiederholt (Lauf `f1_1110`)

Sprung `1110@240` + `RE15_PLAYER_POS 10580,2650,0,0` (Kollision schiebt auf den Standplatz vor der
Leiche), KEIN `RE15_FORCE_CUT`. Eingaben: F330 Viereck, F470 Viereck, F560 Viereck, **F689 Kreuz**
(falsche Taste an der Frage), F700 Rechts, F712 Viereck, F850 Viereck, F990, F1080, F1210 Viereck,
F1350 Viereck, F1480 Viereck, F1560 Start.

| Bild | gesehen (Framedump) | Protokoll |
|---|---|---|
| F460 | "It's a police officer, he's dead." mit Pfeil | `[msg] room=1110 id=0` -> `[leiche] ... Port-Nachricht 20 (Bit (9,61)=0)` |
| F540 | "He is holding something." | |
| F570 | Munitionsbild zoomt ein (Tony's-Arms-Packung) | `Text zu -> Aufnahme-Modal H. Gun Bullets x15`; modal.log F561 Zustand 2 |
| F690 | "Will you take the H. Gun Bullets?" Cursor auf Yes; Kreuz F689 bewirkt NICHTS | modal.log F588..F711 durchgehend Zustand 6 |
| F710/F720 | Cursor auf No, Bild schrumpft | modal.log F712..F728 Zustand 8 (kein Einfuegen), `[leiche] No/voll ... Bit (9,61)=0` |
| F900/F1040/F1190 | 2. Druck: WIEDER beide Seiten + dieselbe Frage | `Port-Nachricht 20` zum zweiten Mal, zweites Modal F1081.. |

Beleg: `F_belege/abn1_1110_nein_ja_kurz.png` (obere zwei Reihen). -> Nutzerpunkt "Text immer wiederholt,
bis man annimmt" ERFUELLT.

Zusatz `f3_1110_kreuz`: Seitenwechsel und Schliessen mit KREUZ statt Viereck (Endwarten 0xc000,
msg_common.c:471-473) und Schnellvorlauf (Viereck 20 Bilder gehalten) -> Modal kommt trotzdem im
Schliess-Bild, "Yes" 50 -> 57, danach `Port-Nachricht 21`.

### 3.2 ROOM1110 — Ja -> Munition ins Inventar, danach nur Kurztext (Lauf `f1_1110`)

| Bild | gesehen | Protokoll |
|---|---|---|
| F1190 -> F1220 | Frage, Viereck auf Yes -> Bild weg ohne Schrumpfen | `[leiche] Yes: Bit (9,61) gesetzt, H. Gun Bullets 50 -> 57` |
| F1440 | 3. Druck: NUR "It's a police officer, he's dea..." (eine Seite, kein Pfeil) | `Port-Nachricht 21 (Bit (9,61)=1)`, modal.log ohne drittes Modal |
| F1690 | Statusschirm: Munitionspackung **57** in EINEM Platz (gestapelt), Pistole 15, Messer | `F_belege/abn1_1110_inventar57.png` |

-> "einmal Munition" + "danach nur noch It's a Police officer, he's dead." ERFUELLT (Stapeln wie Runde 26:
50 + 7 = 57, 15 halbiert nach `re15_pickup_menge_nutzer`).

### 3.3 ROOM1230 — echter Tuerweg, Nein/Ja (Lauf `f2a_1230`)

Sprung `11B0@240` (Garage) + Standplatz vor der Tuer, Flags wie im Bau-Durchlauf ((4,243) Garagentuer ->
1230, (3,130) Marvin-Szene gesehen, (7,136)/(7,137) Hunde des Gangs erledigt — sonst ist der Weg im
Auslieferungsstand zu bzw. toedlich, Herleitung Dossier 9.6). Viereck an der Tuer (Tick 543) -> `DOOR FIRE
slot=0` -> Tuersequenz DOOR1D (311 Bilder) -> ROOM1230 Cut 8 automatisch; Gang U/R/U bis (3900,26850), ohne
PLAYER_POS/FORCE_CUT nach dem Eintritt.

| Bild | gesehen | Protokoll |
|---|---|---|
| F650 | "A miserable death..." mit Pfeil; Leon und Leiche im Bild | `[msg] room=1230 id=10` -> `Port-Nachricht 22 (Bit (9,62)=0)` |
| F760 | "He is holding something." | |
| F780-F920 | Bild zoomt ein, Frage, Rechts -> No, Bild schrumpft | modal.log F775 Z2 / F802 Z6 / F916 Z8; `No/voll ... (9,62)=0` |
| F1120-F1410 | 2. Druck: wieder beide Seiten + Frage | zweites `Port-Nachricht 22`, zweites Modal |
| F1420 | Yes -> Bild weg | `Yes: Bit (9,62) gesetzt, H. Gun Bullets 50 -> 57` |

Beleg: `F_belege/abn1_1230_tuer_nein_ja.png`.

### 3.4 Gegenproben

**Wiederbetreten nach der Annahme (Lauf `f2b_1230_wieder`, ein Prozess, globaler Skripttakt):**
Garage -> Tuer -> 1230 -> Leiche -> Yes (50 -> 57) -> Drehung, Gang nach +X -> Viereck an Tuer-Platz 3
(`DOOR FIRE slot=3 ... spawn=(-26483,0,-18094)`) -> ROOM11B0 -> Drehung ~180 Grad -> Viereck an Platz 0 ->
wieder ROOM1230 (Tuersequenz, RDT neu geladen) -> Gang zur Leiche -> Viereck:
`Port-Nachricht 23 (Bit (9,62)=1)`, im Bild F610/F660 NUR "A miserable death..." (kein Pfeil), F690 zu,
modal.log hat genau EIN Modal (das des ersten Besuchs). Beleg `F_belege/abn1_1230_wiederbetreten_kurz.png`.

**Speichern -> Laden ueber den ECHTEN Speicher- und Ladeweg (Laeufe `s1_1110_save`, `s2_1110_laden`):**
s1: Leiche 1110 -> Yes (50 -> 57) -> Kurztext -> Speicherbildschirm (`RE15_SAVE_TEST_AGAIN=1100`, Karte per
`RE15_CARD_AUTO`) -> `[save] saved (room 1110) slot n=0`. s2: neue exe, `RE15_CONTINUE_TEST` mit GENAU
dieser Karte -> `[save] CONTINUE: resumed in room 1110` -> Viereck -> `Port-Nachricht 21 (Bit (9,61)=1)`,
Bild "It's a police officer, he's dead." einseitig, modal.log LEER, Statusschirm 57.
Beleg `F_belege/abn1_1110_speichern_laden.png`. (Der Bau-Agent hatte nur eine vom Werkzeug geschriebene
Karte geladen; hier traegt der vom Spiel selbst geschriebene Stand das Bit.)

**Raumvarianten ROOM1111 / ROOM1231 (Laeufe `v1_1111`, `v2_1231`):** Karte mit Stand vor der Leiche
(`probe_r34n_f_karte k1111.mcr 1111` / `1231`), CONTINUE -> RDT `ROOM1111`/`ROOM1231` geladen:
lang (20 bzw. 22) + Angebot -> Yes 50 -> 57 -> kurz (21 bzw. 23). Beleg `F_belege/abn1_varianten_1111_1231.png`.
Einschraenkung: der Stand fuehrt die Spielfigur Leon (das Werkzeug setzt keine Elza-Figur); geprueft ist die
RDT-Variante, nicht die Figur.

**Falsche Eingaben:** Kreuz an der Ja/Nein-Frage -> keine Wirkung (Zustand 6 bleibt, byte-true: nur
virt. 0x4000 bestaetigt, item_modal_common.c:296); Kreuz auf den Textseiten -> blaettert/schliesst wie
Viereck, Angebot kommt trotzdem (s. 3.1 Zusatz).

**Nachbarverhalten:** Suite gruen inkl. `integration_keypad`, `unit_r30_tuer_verschlossen` (alle
Text-/Ereignis-/Tuer-Plaetze beider Raeume einzeln angefahren) und Riegel-Teil `andere` (1110/1111 9 von 9,
1230/1231 11 von 11 uebrigen Nachrichten unveraendert, selbst gefahren). Tabellen anderer Haken im selben
Opcode (`gen/discard_sites.inc`: 1230 nur msg 5 / Leser-Sub 20; `gen/lock_se_sites.inc`: 1230 nur msg 6)
haben keinen Eintrag fuer (1110, 0) / (1230, 10) — selbst gelesen. Exe-Gegenprobe Kartenleser/Tastenfeld/Regal
liegt vom Bau vor (`gegenprobe_andere_texte.png`, angesehen: "Enter the first number." unveraendert) — nicht
wiederholt, kein Zweifel.

**Voll-Fall:** an der exe nicht nachgefahren — `RE15_GIVE` kann das Briefing-Inventar nicht ueberschreiben,
und mit vorhandener H.-Gun-Munition stapelt der Runde-26-Weg auch bei vollem Inventar (allgemeines
Modal-Verhalten, nicht Spur F). Engine-Riegel `voll` (selbst gefahren): "can't carry", Bit 0, Runde 2
identisch.

**Vorher/Nachher:** vorher (Bau-Beleg `ist_1110_leiche.png`, angesehen) drei Seiten inkl. 'The numbers
"4312" are printed on the slip.', kein Angebot; nachher zwei Seiten + Angebot, kein Code mehr.

## 4. Code gegen RE-Gate

## 5. Maengelliste

## 6. Belege (Dateien)
