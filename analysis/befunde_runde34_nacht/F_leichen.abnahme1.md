# Spur F (Leichen ROOM1110/ROOM1230) — ABNAHME 1

Stufe: Abnahme 1 (unabhaengig, nach dem Bau). Pruefer hat den Code nicht geschrieben.
Ziel: WIDERLEGEN, dass Spur F fertig ist.

## 0. Urteil

**ABGENOMMEN** (mit drei kosmetischen Maengeln an der Dokumentation, 5.1) — und mit einer BINDENDEN
Integrationsauflage (5.2): F nur zusammen mit Spur E Dok 3 + Dok 4 ausliefern.

Widerlegungsversuche, die alle gescheitert sind (Einzelheiten 3/4):
- jeder Nutzerpunkt (AUFTRAG.md Z. 49, Z. 61-64) an der echten exe mit Spielereingaben: 1110 und 1230
  (1230 ueber den echten Tuerweg aus der Garage) — langer Text, Angebot, "No" -> beim naechsten Untersuchen
  wieder Text + Angebot, "Yes" -> H. Gun Bullets 50 -> 57 in EINEM Platz, danach nur der kurze Text;
- falsche Eingaben (Kreuz an der Ja/Nein-Frage wirkungslos; Kreuz/Schnellvorlauf auf den Textseiten);
- Wiederbetreten nach der Annahme (Tuer raus und wieder rein, ein Prozess): kurzer Text, kein Modal;
- Speichern ueber den Speicherbildschirm des Spiels + Laden per CONTINUE: kurzer Text, kein Modal, 57;
- Raumvarianten ROOM1111 / ROOM1231: identisch;
- Nachbarverhalten: Suite 440/440 inkl. Tastenfeld/Tuer-Riegel, Riegel-Teil `andere` selbst gefahren;
- RE-Gate: Text- und Ereignis-Bytes selbst aus den RDTs gelesen, Open-Guard und Modal-Frage selbst
  disassembliert, keine Konstante ohne Beleg, kein Rate-Tell.

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

Geprueft: `re15_leiche.h`, `leiche_1110_1230.c`, die zwei Haken-Zeilen, `test_r34n_f_leiche.c`,
`probes/r34n_f_leiche.cmake`, die Aenderung an `test_r30_tuer.c`.

| Punkt | selbst nachgeprueft | Ergebnis |
|---|---|---|
| Text-Bytes | Python ueber `shared_assets/PSX/STAGE1`: ROOM1110 msg 0 @0x0D68..0x0DD4, ROOM1230 msg 10 @0x16F4..0x1753, ROOM1011 msg 19 @0x12A1 (`24 41 00 45 4f 00 44 4b 48 40 45 4a 43 00 4f 4b 49 41 50 44 45 4a 43 57 57 57`), " something" ROOM1110 @0x0EC4 | die vier Texte im Code sind Byte fuer Byte diese Stellen (Kopf `04 02`, Umbruch `02 00`, Ende `01 00`, Punkt `57`) |
| Ereignis-Bytes | ROOM1110 @0x0AEE / sub02 @0x0CEE..0x0D23, ROOM1230 @0x0D52 / sub21 @0x14A6..0x14DB | wie im Kopf von `re15_leiche.h` zitiert; 1111 == 1110, 1231 == 1230 an denselben Offsets (bytegleich) |
| Einzige Code-Quellen | alle RDTs nach `10 0f 0d 0e` ("4312") und `11 12 0f 0e` ("5632") durchsucht | nur ROOM1110/1111 @0xDB3 und ROOM1230/1231 @0x1732 — die ersetzten Nachrichten (-> Integrationsauflage 5.2) |
| Open-Guard FUN_80027e68 | `re15_disasm.py dis 0x80027e68`: @0x80027e74 `lbu v0,0(v1)` (v1 = 0x800b8520), @0x80027e7c `andi v0,v0,0x80`, @0x80027e80 `beq v0,zero,0x80027e90`, @0x80027e88 `j 0x800280ac` / @0x80027e8c `addiu v0,zero,-1` | stimmt (Sprungziel selbst gelesen, nicht nur zitiert) |
| Modal-Frage ueber dieselbe Routine | `dis 0x8001df64`/`0x8001dfd8`: @0x8001df6c `ori a1,zero,0x100` -> @0x8001dfe0 `jal 0x80027e68`; @0x8001df88 `ori a1,zero,0x100` / @0x8001df90 `jal 0x80027e68` | stimmt |
| Konstanten ohne Beleg | Suche nach "plausib/interim/tunable/vermutlich/TODO/faithful" in allen neuen Dateien: 0 Treffer. Jede Konstante in `re15_leiche.h` traegt Datei-Offset/@0x oder VERTRAG-Zuteilung; Menge 15 und Modal-Zeitpunkt als PORT-WAHL/NUTZER-VORGABE gekennzeichnet | ok |
| Menge 15 (PORT-WAHL) | RE1.5 vom Gegenpruefer unabhaengig gezaehlt (38 Saetze, 22 x 15); RE2 selbst grob gezaehlt (Roh-Scan `4e .. 02 ..` Id 0x14 in 2553 SCD-Bloecken: 47 Treffer, 41 x 15, 6 x 30) | Modus 15 in beiden Spielen haelt |
| Duration-Zeile (`d < 65535`) | msg_common.c:342-343 macht fuer jede Raumnachricht dasselbe | Muster des Raumladers, kein neues Verhalten |
| Andere Haken im selben Opcode | `gen/discard_sites.inc`, `gen/lock_se_sites.inc` gelesen | kein Eintrag fuer (1110,0)/(1230,10) -> der fruehe Ausstieg verliert nichts |
| Haken-Stelle | scd_vm.c:1775 hinter dem Stimmen-Riegel (:1750-1769), vor Besitz-Gate/Tuerton/Schreibmaschine | wie Gegenpruefung Auflage 1 |
| `test_r30_tuer.c` (+7 Zeilen) | room_boot beantwortet ein offenes Aufnahme-Modal, bevor der naechste Raum gebootet wird; `messen()` bootet je Platz neu | legitime Pruefstand-Anpassung (der Pruefstand tickt das Modal nicht, main.c:7380); ohne sie: "Tabellen-Plaetze 18 statt 51" (Bau-Suite 05:39). Aber undokumentiert (M2) |

Keine erfundene Konstante, kein Stub, kein Rate-Tell gefunden.

## 5. Maengelliste

### 5.1 Maengel der Spur F

| # | Schwere | Mangel | Beleg |
|---|---|---|---|
| M1 | kosmetisch | Dossier nach dem Limit-Abbruch unvollstaendig: 9.5 Suite steht auf "(laeuft)", 9.8 "Abschluss-Commit mit Suite folgt", Kopfzeile noch "Stufe: ERMITTLUNG + BAUPLAN (noch kein Port-Code)". Suite-Ergebnis 440/440 steht nur in dieser Abnahme | `F_leichen.md` Z. 3, Z. 588-589, Z. 649 |
| M2 | kosmetisch | Aenderung an einem FREMDEN Riegel (`tests/unit/test_r30_tuer.c`, Runde 30) nur im wip-Commit `d0611644`, nicht in Dossier 9.1 (Dateiliste) / 9.7 (Abweichungen); sachlich richtig (s. 4) | `git show d0611644`; `build/r34n_f_suite.log` Z. 9350 "Tabellen-Plaetze 18 ... statt 51" (unversioniertes Bau-Protokoll im Baum) |
| M3 | kosmetisch | Die RE-Sonde `probe_r34n_f_leiche` wird weiter in jedem Bau uebersetzt, misst aber laut eigenem Kopf nicht mehr den gebauten Stand (simuliert mit den Original-Ids); Dossier 9.9.6 schlaegt Entfernen bei der Integration vor | `probes/r34n_f_leiche.cmake` Z. 6-10 |

Kein blocker, kein wesentlicher Mangel an Spur F gefunden.

### 5.2 Integrationsauflage (bindend — kein Mangel der Spur F, Folge des Nutzerauftrags)

F entfernt die EINZIGEN Quellen der Tastenfeld-Codes 4312 (Communication Room ROOM10D0 -> 10F0) und 5632
(Weapon Storage ROOM1230) (Byte-Suche in 4). F darf deshalb nur ZUSAMMEN mit Spur E Dok 3 (Marvin's Notes,
4312, ROOM1020) und Dok 4 (Armory Notice, 5632, ROOM1010) auf master/ausgeliefert werden. Stand Spur E
(nur gelesen, 08:5x): Dok 3 gebaut (`bbdd45b5`), Dok 4 noch nicht. Ohne Dok 4 fehlt 5632, ohne Dok 3 fehlt 4312.

### 5.3 Lesarten (geprueft, kein Mangel)

| Nutzer schreibt | Port zeigt | warum haltbar |
|---|---|---|
| "It's a **P**olice officer, he's dead." | "It's a **p**olice officer, he's dead." | wortgleich zur Original-Seite (ROOM1110 @0x0D6A) -> Original-Bytes laut Bau-Auftrag ("Wortgleiche Teile aus den Original-Bytes"); Vorlauf Runde 33 "Report" -> "report" ohne Einwand |
| "He is **H**olding something" (1110 ohne Punkt) | "He is **h**olding something." | "He is holding" wortgleich (@0x0D8D / @0x0170C, am Stueck ROOM1011 @0x012A3); Punkt = Satzende des ersetzten Original-Satzes (@0x0DA1) und die 1230-Schreibung des Nutzers |
| eine Zeile | Seitenumbruch `02 00` zwischen den Saetzen | steht im Original genau dort (@0x0D8B / @0x170A) |
| "A miserable death…." / "A miserable death…" | "A miserable death..." (drei Punkte) | wortgleich zur Original-Seite @0x016F6; RE1.5 hat keine Auslassungs-Glyphe (0 x 0xF2, 0 x vier Punkte in 1227 Nachrichten) |

Sollte der Nutzer die Grossschreibung woertlich wollen, sind es zwei Glyphen je Text (0x4C -> 0x2C "P",
0x44 -> 0x24 "H"; Gross A-Z ab 0x1D, klein a-z ab 0x3D) — keine Mechanik-Aenderung.

### 5.4 Nebenbefunde (nicht Spur F, vorbestehend)

- Plc_motion(1,11,0) der Leichen-Ereignisse zeigt im Port keine sichtbare Haltungsaenderung (auch vor dem
  Bau, `ist_1110_leiche.png`; Dossier 8.1).
- Nach einem Debug-Sprung + `RE15_PLAYER_POS` bleibt die Kamera auf Cut 0 (Leon/Leiche ausserhalb des Bilds);
  ueber den Tuerweg waehlt die Auto-Kamera den richtigen Cut (1230 Cut 8, 1110 Cut 3 im Bau-Durchlauf).
  Messhaken-Eigenschaft, kein Spielweg.
- PSX-Ziel tickt das Aufnahme-Modal nicht (Dossier 9.9.7) — betrifft jede Welt-Aufnahme, die Leichen erben es.

## 6. Belege (Dateien)

| Datei | Inhalt |
|---|---|
| `F_belege/abn1_1110_nein_ja_kurz.png` | 1110: Seite 1/2, Modal, Frage (Kreuz ohne Wirkung), No, Schrumpfen, 2. Untersuchen, 2. Frage, Yes, Kurztext |
| `F_belege/abn1_1110_inventar57.png` | Statusschirm nach Yes: H. Gun Bullets 57 in einem Platz |
| `F_belege/abn1_1230_tuer_nein_ja.png` | 1230 ueber den Tuerweg aus 11B0: Seite 1/2, Modal, No, 2. Untersuchen, Yes |
| `F_belege/abn1_1230_wiederbetreten_kurz.png` | 1230 nach Yes -> Tuer -> 11B0 -> Tuer -> 1230: nur "A miserable death..." |
| `F_belege/abn1_1110_speichern_laden.png` | Stand vom Speicherbildschirm geladen: nur Kurztext, Statusschirm 57 |
| `F_belege/abn1_varianten_1111_1231.png` | ROOM1111 / ROOM1231: lang + Frage, danach kurz |
| `F_belege/abn1_laeufe.log` | Protokollauszug aller sieben Laeufe (Eingaben, `[msg]`, `[leiche]`, Tueren, Speichern, Modal-Zustaende) |

Laufordner (nicht versioniert): `build/abn1_f/{f1_1110,f2a_1230,f2b_1230_wieder,f3_1110_kreuz,s1_1110_save,
s2_1110_laden,v1_1111,v2_1231,kalib}`. Skripte: Scratchpad `abn1_lauf.sh`, `skript.py`, `bogen.py`.
