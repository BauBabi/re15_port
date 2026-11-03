# Spur AN: unabhaengige Abnahme 1 (Runde 41)

Gepruefter Stand: Arbeitsbaum `.claude/worktrees/r41_annette`, Zweig `r41/annette`, Kopf 93d5ed2c1 (Code-Stand e2c85f4ab), Basis
`r41/form` 62f0be3ac. Bau: `bash re15_port/tools/local_build.sh build` -> `LOCAL-BUILD-OK`, "ninja: no work to do", die exe
(2026-11-02 23:49) entspricht dem Kopf, denn 93d5ed2c1 aendert nur Dokumentation. Ich bin nicht der Autor. Alle Aussagen unten
stammen aus eigenen Laeufen im echten Fenster, aus eigenen Logs oder aus eigenen Byte-Pruefungen.

## 0. Urteil

**NICHT BESTANDEN.**

Ablauf, Reihenfolge-Sicherheit, Toene, Modell, Elza und Tests sind in Ordnung. Durchgefallen ist die Abnahme an der Bildwirkung
der Kernszene in 5090: Waehrend Annettes Satz "W-William! You're alive!" verdeckt der Koerper der Riesenform in Cut 15 rund 3,3 der
4 Sekunden fast das ganze Bild. Annette sieht man in dieser Zeit nur als Beine am rechten Rand (Mangel M1). Dazu kommen zwei mittlere
Maengel: Annette verschwindet in 6030 mitten im Bild (M2), und Annettes Tod ist derzeit voellig stumm (M3).

## 1. Maengel

| # | Schwere | Was | Beleg |
|---|---|---|---|
| M1 | **schwer** | **Cut 15 wird von der Riesenform zugestellt.** (a) Satz F1436..F1535: ab etwa 0,5 s nach Satzbeginn fuellt der dunkle Koerper der Form 70 bis 80 % des Bildes, bis zum Schnitt auf Cut 11 bei F1554. Annette ist nur noch als Beine am rechten Rand zu sehen. (b) Landung F1346..~F1370: das Bild ist fast schwarz. Zum Vergleich RE2 Cut 0x0B: Annette steht waehrend des ganzen Satzes frei im Bild. Das Dossier behauptet in N1.2/N1.7 "Cut 15 Satz mit Annette rechts frei im Bild". Das stimmt fuer den Kopfstand nicht. Die Form steht in L (500,-22600) nur rund 1760 vom Auge in Cut 15 (46,-1236,-24302) entfernt. Die Kasten-Projektion aus N1.2 (Form bis Bild-x 155) gibt die echte Pose (Drohen-Clip 10 ab Bild ~89, danach Stehen-Clip 2) nicht wieder. | `AN_belege/abnahme1/szene5090_w8.jpg` Felder 5, 8, 9; `vergleich_re2_port_5090.jpg` Zeilen 5, 7, 8. Zweimal reproduziert (Laeufe w5, w8; die Szene laeuft bildgenau wie bei den Autoren: Sturz +163, Landung +178, Hieb +386, Treffer +401, Schnitt +432, Kampfstart +534). **Der eigene NB2-Lauf der Autoren zeigt dasselbe:** `re15_port/build/an_lauf/leon_nb2/bild_210..248` haben im oberen Bilddrittel eine mittlere Helligkeit von 13 (davor und danach 28..31), z. B. Bild 224 und 240. |
| M2 | mittel | **6030: Annette verschwindet mitten im Bild.** Nach msg 30 geht sie in Cut 1 auf die Kamera zu (Plc_dest Modus 4 auf Z_TUER (-3873,-22550)) und ist bei F~1465 gross im Vordergrund. Im naechsten Bild ist sie weg: das Pos_set "versteckt" (6000 sub02 @0x0100A-Bytes) greift, solange sie sichtbar ist. Ein Schnitt, eine Tuer oder ein Ton gibt es dabei nicht. | `zugstart6030_w8.jpg` Felder 10 und 11. Zweimal gesehen (Laeufe zugvoll und w8). |
| M3 | mittel (haengt an Nutzer-Aufnahmen, wirkt aber im Spiel) | **Annettes Tod ist stumm.** Der Schrei hat absichtlich keinen Untertitel (F9). Die Datei `synchro/STAGE5/room5090/main41.wav` fehlt aber. Log: `[voice] ROOM5090 msg 41: keine Aufnahme ... Zeile bleibt stumm`. Damit zeigt Cut 13 nur Leon, ohne Text und ohne Ton. In RE2 traegt der Schrei Xa 0x32 diesen Schnitt. Alle Sprechzeilen sind ebenfalls stumm (40/42/43 in 5090, 27-30 in 6030), haben aber Untertitel. | debug.log w8; Ton-Mitschnitt w8 (Abschnitt 3). |
| M4 | mittel/leicht | **Treffer ohne Reaktion im Bild.** Vom Treffer F1569 bis zum Schnitt F1599 steht Annette 1,0 s aufrecht mit erhobenen Armen (Clip 4), voll sichtbar in Cut 11. In RE2 sieht man sie im Moment des Hiebs nicht, weil die Kamera an ihrer Stelle steht. Im Port ist sie sichtbar und reagiert nicht. | `szene5090_w8.jpg` Felder 11 und 12. |
| M5 | leicht | **Blut zu schwach im Bild.** Die Kette ist RE2-korrekt: 30 Aufrufe F1569..F1583, Effekt 0/1/0x2000, Klauen 14/28, 99 Plaetze, Pool 96/96, Byte-Pruefung in Abschnitt 4. Im Bild ergibt das aber nur einen kleinen dunkelroten Fleck am Boden um Annettes Fuesse, etwa 0,9 s lang und zwischen dem Bodenfeuer kaum abgesetzt. RE2 zeigt eine grosse Wolke vor der Kamera. Das Dossier vermerkt das in N2.4/N2.8 selbst ("ginge nur ueber die Kamera"). Weil der Pool voll ist, koennen in diesem Zeitraum keine anderen ESP-Effekte nachspawnen (Bodenfeuer). Sichtbar gestoert hat das im Lauf nicht. | `szene5090_w8.jpg` Feld 11; `vergleich_re2_port_5090.jpg` Zeilen 10 und 11. |
| M6 | leicht | **Untertitel bleiben zu lange stehen.** "Annette: William." steht 3,3 s (F1252..F1351, RE2-Xa 1,17 s), laeuft ueber den Schnitt auf Cut 15 und steht noch waehrend des Sturzes. "Leon: Annette!" (F1642..F1741) steht noch ueber Cut 15 und Cut 12 des Kampfstarts sub04. | state.log w8 (msg-Spalte); `szene5090_w8.jpg` Felder 3 und 4. |
| M7 | leicht/Info | **Tempo knapper als RE2.** Von Krach bis Steuerung: Port 17,8 s, RE2 24,4 s (Tabelle in Abschnitt 2). Am deutlichsten beim Hieb: Ausholen bis Treffer dauert im Port 0,5 s, in RE2 1,3 s. Der Hieb kommt dadurch ohne sichtbares Ausholen. Die Sleeps zaehlen wie RE2 in Bildern (PORT-WAHL 30 statt PAL 25 Bilder/s, dokumentiert), der Weg ist kuerzer. | debug.log w8 gegen `re2_claire/zeitachse.md`. |
| M8 | Info | **Wiedereintritt nach der Szene: keine Leiche.** Das Spawn-Tor verlangt (3,42)=0, die Leiche ist also weg. RE2 legt Annette bei spaeteren Besuchen liegend hin (MAIN sub00 else-Zweig @0x00D16). Erreichbar ist das nur ueber Laden oder Debug-Sprung, weil sub04 die Tuer Slot 4 zu Wagen B schliesst. | Unit-Test (Ausgabe "(3,42)=1 verworfen"). |
| M9 | Info, Form-Spur | Der Kampfstart sub04 zeigt in Cut 15 nur die Haut der Form (bekannt, Dossier 10/11). In Cut 15 steht waehrend des Satzes ausserdem Leon im Hintergrund; RE2 Cut 0x0B zeigt nur Annette. | `szene5090_w8.jpg` Feld 15. |

Hinweis fuer die Nachbesserung, ohne Wertung der Loesung: Fuer den Satz und die Landung eine Kamera oder Lage waehlen, bei der die
Form nicht zwischen Auge und Annette steht, und das nur im echten Fenster abnehmen, nicht ueber eine Kasten-Projektion.

## 2. Ablauf im echten Fenster und Vergleich mit RE2

### 2.1 Laeufe

Alle Laeufe liefen mit einer eigenen exe-Kopie `re15_pc_abn1_*` im Renderer des Nutzers (kein AUTOSHOT, kein SOFTWARE_RENDER).
Aufgenommen wurde per ffmpeg gdigrab `hwnd=` direkt nach mp4, der Ton mit `RE15_AUDIO_CAP` aus der echten Mischung. Beendet habe
ich nur die eigene PID. Werkzeug: `AN_belege/abnahme1/werkzeug/abn_lauf.sh`. Rohdaten liegen in `re15_port/build/abn1/<lauf>/`
(nicht versioniert).

| Lauf | Weg | Ergebnis |
|---|---|---|
| voll1 | Debug-Sprung 6000 vor den Schalter, Flags Bosssieg/Generator (3,48)(3,49), 6000-Szenen (3,100)(3,239), Fuehrerstand-Dialog (4,23); Aktion am Schalter, Frage mit "Yes" | sub07 laeuft echt: msg 5, Frage, Cut 12/8, das Tor oeffnet sich, (3,89) setzt das Spiel selbst (@0x011B4) |
| w6 | Tor-Stand, ab der 6000-Zugtuer echt: Tuer, Wagen, Tuer nach 5090 | **msg 10 "I have to get this thing moving first!"** (94 Bilder), Log `Zugstart nicht gelaufen -> Ausgang = Tuer` |
| w8 | **durchgehend**: 6000-Zugtuer -> Wagen -> msg 10 -> Treppe hoch -> Tuer Slot 1 -> Fuehrerstand: **Zugstart** -> Tuer Slot 0 -> Treppe runter -> Tuer nach 5090 (jetzt offen) -> Wagen A -> B -> C -> **Szene** -> Kampfstart -> Kampf | Bit (9,133) setzt das Programm selbst, es wird NICHT per Flag vorgegeben. Annette spawnt in 5090 und die Szene laeuft. Eingabe: `werkzeug/eingabe_w8.txt` |
| w3 | Zustand nach dem Zugstart ((9,133) per Flag), 6000-Zugtuer -> Wagen -> Ausgang | **msg 20 "You can't get off the train right now."** (94 Bilder); die Tuer nach 5090 oeffnet sich |
| elza | 5091 mit (3,89),(9,133), Weg wie integration_r41_annette | keine 0x4D im state.log (0 Zeilen `t=4d`), keine `[annette]`-Zeile, Kampfstart wie bisher |

Nicht im Fenster gelaufen bin ich nur den Weg vom Torschalter die Plattformtreppe hinunter zur Zugtuer in 6000. Der Zustand an der
Uebergabe ist derselbe: genau die Flags, die sub07 setzt. (Der Autopilot des Ports kommt in 6000 und im 6030-Wagen nicht durch;
das ist ein Problem des Messwerkzeugs, nicht des Spiels. Gelaufen bin ich deshalb mit Tastenskripten.)

### 2.2 5090 Bild fuer Bild gegen RE2 (Lauf w8, Port-Bilder ab Krach F1168)

| Schritt | RE2 (Aufnahme Lauf 1, s ab Krach) | Port (s ab Krach, 30 Bilder/s) | Kamera RE2 / Port | Befund |
|---|---|---|---|---|
| Krach | 0 (Se_on @0x01040) | 0 (Ton 0) | 7 / 12 | ok, beide im Bild, Annette klein links |
| "William." | +3,53 | +2,80 | 7 / 12 | ok, nur Untertitel (stumm), steht 3,3 s (M6) |
| Annette geht, Schnitt | +8,07 (Cut 2) | +4,83 (Cut 15) | 2 / 15 | ok |
| Sturz | +9,30 | +5,43 (Ton 1) | 2 / 15 | Form faellt von oben ins Bild, ok |
| Landung | +10,21 | +5,93 | 2 / 15 | **Bild fast schwarz (M1b)**; in RE2 Ruecken der Figur, Annette dahinter sichtbar |
| Bruellen, Anstarren | +11,77 (Ton +11,43) | +6,90 (Ton 2) | 0A / 11 | gut: Annette vorn, Form dahinter (wie #145) |
| Satz | +14,80, 3,95 s | +8,93, Untertitel 3,3 s | 0B / 15 | **verdeckt (M1a)** |
| Hieb | +20,37, Ausholen 0,8 s | +12,87, Treffer 0,5 s danach | 0A / 11 | Hieb sichtbar, kein Ausholen (M7) |
| Blut | +21,69, ~1,1 s gross vor der Kamera | +13,37, ~0,9 s klein am Boden | 0A / 11 | zu schwach (M5); Annette reglos (M4) |
| Schnitt weg + Schrei | +22,80, Xa 0x32 1,39 s | +14,40, Schrei stumm | 6 / 13 | **kein Ton (M3)** |
| Steuerung / Kampf | +24,39 | +15,8 "Leon: Annette!", +17,8 sub04 | 6 / 13 -> 15 -> 12 | Ruf ist PORT-WAHL (freigegeben); Cut 15 nur Haut (M9) |

Kameralogik: Die Zuordnung 7->12, 2->15, 0A->11, 6->13 passt inhaltlich. 0B->15 passt nicht, weil die Form im Bild steht (M1).

### 2.3 6030 Zugstart (Lauf w8, ab Eintritt F704)

Die Reihenfolge stimmt mit dem Nutzerentscheid ueberein: Cut 3 Pult (F720) -> Cut 4 Pult, Ton 3 "Zug an" (F800) -> Cut 1 ->
msg 27 (F890) -> **Krach von hinten** Ton 0 (F1010) -> beide drehen sich zur Wagentuer -> msg 28 "What was that? / ...Sherry is
back there!" (F1100) -> msg 29 Leon (F1220) -> msg 30 (F1340) -> sie geht ab (M2) -> Steuerung zurueck F1534 (27,7 s nach dem
Eintritt). Die Untertitel sind lesbar, die Sprecher stimmen (Annette rot, Leon gruen), die Gesten passen zum jeweiligen Sprecher.
Waehrend der Szene antwortet Leon nicht auf Eingaben. Danach reagiert er sofort: L-Taste ab F832 im Lauf zugsteuer gemessen.

### 2.4 Modell und Clipping

- Annette (`RE2/ANNETTE_4D.EMD`, Log `Ersatzmodell RE2/ANNETTE_4D.EMD`) sieht in den Nahaufnahmen in 6030 aus wie #145 bis #147:
  helles Haar, weisser Kittel, schwarzes Oberteil, Guertel, lange dunkle Hose (`zugstart6030_w8.jpg` Felder 2, 3, 6, 10). Beim
  Gehen, bei den Gesten und am Pult habe ich keine Gliederfehler gesehen. Das Log meldet `16 meshes, 15 bones`. Ein Netz ohne
  Knochen ist im Bild nicht aufgefallen; das ist nur ein Hinweis.
- Nach dem Treffer wird die Blut-Textur geladen (`Ersatzmodell RE2/ANNETTE_4D_BLUT.EMD`). Von hinten (Cut 11) sieht man kein
  Blut auf dem Kittel, die Leiche selbst ist nie im Bild.
- Annette steckt nicht in der Form: Die Hand ist im Trefferbild 87 vom Ziel entfernt, Annette steht davor. Die Form steckt dagegen
  in der Kamera von Cut 15 (M1). Waende: Im Kampf springt die Form von (1123,-23369) nach (11578,-22942). Eine Wand habe ich dabei
  nicht durchdrungen gesehen, gezielt geprueft habe ich das aber nicht (Form-Spur).
- Die Leiche bleibt im Kampf liegen: state.log w8 F1700..F2460 `t=4d ... @(5290,-24183) hp=-1` unveraendert.

## 3. Ton (Ton-Mitschnitt w8, Kreuzkorrelation gegen die Wellen aus ANNETTE_SE.VBS, `werkzeug/ton_korr.py`)

| Satz | Einsatz im Mitschnitt | r (zweitbeste Stelle) | Abstand im Port | Abstand in RE2 |
|---|---|---|---|---|
| 3 Zug an (6030) | 49,319 s | 0,784 (0,042) | | |
| 0 Krach (6030) | 56,750 s | 0,855 (0,048) | Zug an -> Krach 7,43 s (F800->F1010 = 7,0 s) | |
| 0 Krach (5090) | 158,012 s | 0,842 (0,043) | | |
| 1 Sturz | 163,677 s | 0,897 (0,085) | Krach -> Sturz 5,67 s | 9,27 s |
| 2 Bruellen | 165,140 s | 0,741 (0,079) | Sturz -> Bruellen 1,46 s | 2,13 s |

Alle vier Toene sind in der echten Mischung eindeutig hoerbar (die zweitbeste Stelle liegt jeweils unter 0,09). Stimmen und Schrei
fehlen (M3). Der Musikwechsel beim Tor kommt aus dem Original: in den Port-Programmen steht kein 0x54, der Unit-Riegel prueft das.
Hoerbelege: `szene5090_w8_mit_ton.mp4`, `zugstart6030_w8_mit_ton.mp4`. Bild und Ton sind dort ueber den Krach-Einsatz synchronisiert,
der Versatz Ton -> Video betraegt 7,45 s.

## 4. Code-Stichproben (eigene Byte-Pruefung)

- **RE2 Claire ROOM6141** (`info/re2clair/PL1/RDT/ROOM6141.RDT`), 27 zitierte Stellen, alle gleich: z. B. @0x01040 `36 03 0b 00`,
  @0x0104C `09 0a 19 00`, @0x0107A `59 00 30 00`, @0x010CA `34 17 01 00`, @0x01110 `09 0a 3c 00`, @0x0111A `59 00 31 00`,
  @0x01142 `34 17 02 00`, @0x01150 `59 00 32 00`, @0x01162 `32 00 f4 cf 84 03 3c c4` (-12300, 900, -15300), @0x0118A `22 03 42 01`.
- **FUN_80104C24 direkt vom Claire-Abbild** (`Resident Evil 2 Claire.bin`, Sektor 62154, nur gelesen; unabhaengig vom
  Savestate-RAM der Autoren): @0x80104CFC `24040003`, @0x80104D08 `0c015da4`, @0x80104D10 `2402000f`, @0x80104D14 `a202016b`,
  @0x80104D18/1C `3c118010 86316330`, @0x80104D2C `26250001`, @0x80104D38 `0c04162f`, @0x80104D44 `26250003`,
  @0x80104D4C `0c04162f`, @0x80104D5C `2442ffff`, @0x80104D60 `a202016b`. Das stimmt mit dem Code (AN_BLUT_*) und mit
  `blut_emitter_ram.txt` ueberein. In Sektor 62141 liegt eine zweite Fassung derselben Routine mit anderer Ladeadresse
  (`0c044a2f`, [0x80113330]).
- **RE1.5** (shared_assets), 23 Stellen, alle gleich: ROOM6000 @0x011B4 `22 03 59 01`, @0x0100A `32 00 f5 d6 f0 f1 c6 21`;
  ROOM6030 @0x00C72 (32 Byte Tuer), @0x0106A `46 03 01 31 0a 00 ff ff 00 00`, @0x01098 `46 02 01 31 14 00 ff ff 00 00`, @0x010F8,
  @0x01132/@0x010C4 `04 ff 18 05`, @0x0108E `21 03 59 01`, @0x01210, @0x01364; ROOM5090 @0x0122A, @0x0124A, @0x01296
  `04 ff 18 04`, @0x0129A `22 03 2a 01`; ROOM5080 @0x00822 `32 00 4c b9 20 ea 78 ba` (y -5600), @0x0082E `2f 01 90 01`,
  @0x00832 `0d 00 06 00 0e 00`.
- Die PORT-WAHL-Kennzeichnung ist vollstaendig: Lagen A0/W1/W2/S/L, Kamerazuordnung, Clipwahl 4/6, Knochen 14/28, Z_TUER, Wortlaut
  27-30/42. Kleinigkeit: k_prog setzt Sperre und Balken in umgekehrter Reihenfolge zu sub04 @0x012F2/@0x012F6; fuer das Verhalten
  spielt das keine Rolle.
- `annette_modell.py --pruef`: ANNETTE_4D.EMD und ANNETTE_4D_BLUT.EMD bytegleich (md5 e03878e6..., c89c0ad0...).

## 5. Reihenfolge-Sicherheit

| Fall | Ergebnis | Beleg |
|---|---|---|
| 5090 vor dem Zugstart betreten | Im Spiel nicht erreichbar: die Tuer nach 5090 zeigt msg 10. Mit Debug-Sprung wird die 0x4D verworfen und es gibt keine Szene. | w6; integration `leon_ohne_zug`; Unit |
| Tor offen, nie im Fuehrerstand | Der Wagen bleibt im Vor-Start-Zustand (Ausgang ist eine Tuer, nach 5090 msg 10). Der Zugstart kommt nur aus dem Fuehrerstand. | w6, w8 |
| Zugstart ein zweites Mal | nein ((9,133)) | Unit "6030 Wiedereintritt" |
| 5090-Szene ein zweites Mal / Raum neu laden | nein ((3,42), Spawn-Tor); bei Wiedereintritt keine Leiche (M8) | Unit "einmalig", "(3,42)=1 verworfen" |
| Leiche im Kampf | bleibt liegen, hp -1 | state.log w8 |
| Elza 5091 | keine Frau im Kittel, keine Szene | Lauf elza, integration `elza` |

## 6. Tests (eigene Laeufe in diesem Arbeitsbaum)

| Test | Ergebnis |
|---|---|
| unit_r41_annette, unit_r41_annette_se_bank/_toene/_lagen, unit_r36_kartenbits | gruen |
| integration_r41_annette (4 Laeufe) | gruen, 248 s |
| integration_r41_birkin_pistole | gruen, 280 s |
| integration_r35_zgirl_5090 | gruen, 178 s |
| Suite `^unit_\|^prop_\|^probe_` ohne `releaseskript\|android\|r37_masken36_ausschluss\|paket\|titelversion`, `-j 4` | **1039/1039 gruen** (372 s) |

Die 26 ausgeschlossenen Release-, Titelversion-, Android- und Paket-Skripttests sind ein Umgebungsproblem (`df.exe` haengt
maschinenweit). Laut Auftrag habe ich sie nicht wiederholt.

Die Tests decken M1 bis M6 nicht ab, weil sie Logik und Zaehler pruefen, aber nicht das Bild.

## 7. Belege

`analysis/befunde_runde41/AN_belege/abnahme1/`:

| Datei | Inhalt |
|---|---|
| `szene5090_w8.jpg` | 15 Felder der 5090-Szene aus Lauf w8 (durchgehender Lauf ab der 6000-Zugtuer) |
| `vergleich_re2_port_5090.jpg` | 13 Zeilen: RE2 Claire (Aufnahme der Autoren) links, Port w8 rechts |
| `zugstart6030_w8.jpg` | 12 Felder Zugstart 6030 inklusive Abgang (M2) |
| `tor_und_tueren.jpg` | Torschalter und Tor (voll1), msg 10 vor dem Zugstart (w6), msg 20 danach (w3), Elza 5091 |
| `szene5090_w8_mit_ton.mp4`, `zugstart6030_w8_mit_ton.mp4` | Fensteraufnahme mit dem Ton-Mitschnitt des Ports |
| `werkzeug/` | `abn_lauf.sh` (Lauf), `eingabe_w3/w6/w8.txt` (Tastenskripte), `leiste.py`, `dicht.py`, `ton_korr.py`, `sca_bild.py` |

Keine Datei unter `pri/`, `synchro/`, `Version_80%/` oder `info/` veraendert, Code nicht geaendert. Alle exe-Kopien `re15_pc_abn1_*`
sind beendet und geloescht.
