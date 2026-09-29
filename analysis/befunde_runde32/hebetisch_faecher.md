# Runde 32 — Hebetisch ROOM1150/1151: Granate + Sicherung in die unteren Fächer

Stand: 2026-09-29, Zweig `r32/hebetisch-fach`, Baum `.claude/worktrees/r32_hebetisch`, Basis 48d43b34.
Werkzeuge `analysis/befunde_runde32/hebetisch_faecher_werkzeug/`, Belege `analysis/befunde_runde32/hebetisch_belege/`,
grosse Zwischenausgaben `build/r32_hebetisch/` (nicht versioniert).

## 0. Befund (wörtlich, AUFTRAG.md)

> "Die Granate und die Sicherung die im hochfahrenden Modell in ROOM 1170 rein soll, liegt aktuell
> oben drauf. Aber sie sollen unten, in den hochfahrenden Fach liegen - ein item links ein item rechts."

"ROOM 1170" = Hebetisch ROOM1150/1151 (Runde 30 bestätigt).

## 1. Ausgangslage (Runde 31)

- Sicherung obj 4 (Item 0x40): (-280,-1062,1280) rot_y 1440 — oben in der Kuppel.
- Granate obj 7 (Item 0x09): (-260,-1091,1140) rot_y 1792 — oben in der Kuppel.
- Ziel: Granate LINKES unteres Fach, Sicherung RECHTES unteres Fach (Bildschirm, Cut 4),
  auf dem Fachboden, fährt mit (parent_obj=0).

## 2. Geometrie der unteren Faecher (ausgelieferte Bytes, nur gelesen)

Werkzeug `hebetisch_faecher_werkzeug/faecher.py` (sucht in Prop 0 die achsparallelen Vierecke, die von
der Vorderfront x=2 bis zur Rueckwand x=-1258 durchgehen; Viereck-Face-Offset = MD1-Kopf Feld 11 `qf`),
`p0_dump.py` (volle Punkt-/Flaechenliste mit Datei-Offsets). ROOM1150.RDT == `info/Re1.5/PSX/STAGE1/ROOM1150.RDT`
(cmp). Plattform-Koordinaten, +Y nach unten.

Prop 0 (MD1 @0x11E40, 1 Mesh, 163 Punkte, 11 Dreiecke, 120 Vierecke) ist unten ein Kasten
x[-1258..2] y[-901..-1] z[5..1805] (Tischplatte y=-901, das Kuppel-Podest y -886..-1036 sitzt darauf).
Darin zwei durchgehende Faecher, Trennwand z 861..950:

| Fach | z | Boden | Decke | Seitenwaende | Rueckwand x=-1258 | vorn x=2 |
|---|---|---|---|---|---|---|
| A | 96..861 | y=-90, Viereck 89 @0x12EE0 (`6b00 8a00 6b00 8b00 6b00 7e00 6b00 7f00` = Punkte 138/139/126/127) | y=-810, Viereck 87 @0x12EC0 | z=96 Viereck 86 @0x12EB0, z=861 Viereck 88 @0x12ED0 | volle Tafel Viereck 119 @0x130C0 | offen (Rahmen um y -810..-90) |
| B | 950..1715 | y=-91, Viereck 93 @0x12F20 (`6f00 8e00 6f00 8f00 6f00 8200 6f00 8300` = Punkte 142/143/130/131) | y=-811, Viereck 91 @0x12F00 | z=950 Viereck 90 @0x12EF0, z=1715 Viereck 92 @0x12F10 | volle Tafel Viereck 82 @0x12E70 | offen (Rahmen um y -811..-91) |

Bodenpunkte (Datei-Offset, Bytes): 138 @0x122D4 `0200 a6ff 5d03` (2,-90,861), 139 @0x122DC `0200 a6ff 6000`
(2,-90,96), 126 @0x12274 (-1258,-90,861), 127 @0x1227C (-1258,-90,96); 142 @0x122F4 `0200 a5ff b306`
(2,-91,1715), 143 @0x122FC `0200 a5ff b603` (2,-91,950), 130 @0x12294 (-1258,-91,1715), 131 @0x1229C
(-1258,-91,950). Keine andere Flaeche von Prop 0 ragt ins Innere eines Fachs (Clip-Test, 0 von 131).
ROOM1151.RDT (Prop 0 @0x13EB8): dieselben Werte, Boden A Viereck 89 @0x14F58, Boden B Viereck 93 @0x14F98.

Kamera Cut 4 (@Datei 0xE0, pos (-21942,-2160,-18378), tgt (-19980,-1566,-18396)): Plattform rot_y 2048 bei
(-20700, y, -17460) -> Kamera in Plattform-Koordinaten x=+1242, z=+918 (genau vor der Trennwand z 861..950),
y = -2160 - Plattform-y (Ruhe oben -1205: y=-955, also UEBER der Tischplatte -901 -> Blick von oben
schraeg in beide Faecher). +z = rechts auf dem Schirm (Runde 31 gemessen) -> **Fach A (z 96..861) links,
Fach B (z 950..1715) rechts.** Planung (`fach_schirm.py`, Rechnung, Messung folgt): Ruhe oben Boden A
vorn Schirm-x 9..147 y 204, hinten 77..152 y 130; Boden B vorn 163..299 y 203, hinten 160..234 y 129.

## 3. Sitzwahl (⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE)

Das Original hat im Hebetisch keine Beute; fuer den Sitz gibt es keine Instruktion. Jede Zahl kommt aus
den Geometrie-Bytes von §2:

| | Fach | POS_X | POS_Y | POS_Z | ROT_Y |
|---|---|---|---|---|---|
| Granate (obj 7) | A (links) | -628 = (2 + -1258)/2 | -145 = Boden -90 - halbe Hoehe 55 (granate_prop.inc y -55..55) | 478 = (96+861)/2 = 478,5 abgerundet | 1024 |
| Sicherung (obj 4) | B (rechts) | -628 = (2 + -1258)/2 | -117 = Boden -91 - Rohrradius 26 (sicherung_prop.inc y -26..26) | 1332 = (950+1715)/2 = 1332,5 abgerundet | 1024 |

* **Mitte des Fachbodens**: der Gegenstand liegt "im Fach", nicht an dessen Rand; von der Kamera aus ist
  die Mitte waehrend der Hubfahrt und in der Ruhe durch die eigene Oeffnung zu sehen (§4.2, Rechnung + Messung).
* **rot_y 1024**: Modell-Laengsachse X auf Plattform-z = parallel zur offenen Vorderseite, quer im Bild
  (groesste Ansicht). Fuer die Granate geht die offene Ecke (Modell +x/-z, Kopf `re15_granate.h`) dabei nach
  Plattform (-x,-z) = hinten-links, weg von der Kamera (x=+1242, z=+918); die offene Unterseite liegt auf.
* Parent (Elternmatrix obj 0, pc[5]=0xC0-Form), Dialog-Zeitpunkt (Ruhe oben, sub04 Sleep 30 @0x101A) und
  Reihenfolge (Sicherung, dann Granate) **unveraendert** — nur die vier Konstanten je Gegenstand
  (`include/re15_granate.h`, `include/re15_sicherung.h`, Kommentar in `engine/src/sicherung_1150.c`).

Pruefung `sitz_fach.py` (alle Modellpunkte, Q12-Drehung; `hebetisch_belege/sitz_fach_gewaehlt.txt`),
ROOM1150 = ROOM1151 Zahl fuer Zahl:

| | tiefster Punkt | vorn (x=2) | hinten (x=-1258) | Wand z0 | Wand z1 | Decke | Huelle |
|---|---|---|---|---|---|---|---|
| Granate in A | +0,00 = liegt auf | 575 | 575 | 305 | 306 | 610 | x -683..-573 y -200..-90 z 401..555 |
| Sicherung in B | +0,00 = liegt auf | 604 | 604 | 179 | 180 | 668 | x -654..-602 y -143..-91 z 1129..1535 |

Kein Durchstoss durch Boden/Waende/Decke/Rueckwand (alle Abstaende > 0; der Fach-Kasten ist konvex), keine
Ueberschneidung (verschiedene Faecher, Trennwand z 861..950 dazwischen, Huellen-Abstand in z 574), und
keine andere Flaeche von Prop 0 ragt in ein Fach (0 von 131, §2).

## 4. Messung im echten Spiel (Tuerweg ROOM1150)

Echte exe `re15_port/build/platform/pc/re15_pc.exe` dieses Baums, beschleunigter Renderer,
`RE15_FRAMEDUMP` (Ruecklesen vor Present), 320x240, KEIN AUTOSHOT/SOFTWARE_RENDER. Lauf
`hebetisch_faecher_werkzeug/lauf_fahrt.sh` (Kopie Runde 31, Pfade r32, + `RE15_HEBETISCH_LOG`): Debug-Sprung
ROOM1150, Spieler (-21000,-18500), Viereck Tick 30 = F230, Cut_chg(4) F236. Kill-Switch MIT / OHNE-G
(`RE15_SET_FLAG=9:56`) / OHNE-S (`9:53`) / OHNE-beide (`9:53,9:56`). Auswertung `links_rechts.py` (Runde 31).

### 4.1 Links/rechts (`hebetisch_belege/links_rechts_tuerweg_1150.txt`)

| F | Plattform y | Granate Punkte / Schwerpunkt-x / bbox | Sicherung Punkte / Schwerpunkt-x / bbox | Trennung |
|---|---|---|---|---|
| 230..302 | -305..-475 | 0 | 0 | — |
| 306 | -515 | 10 / 106,5 (erste Zeile ueber dem Schreibtischrand) | 0 | — |
| 314 | -595 | 146 / 105,5 | 73 / 205,8 | +100,3 |
| 322 | -675 | 246 / 105,1 / x95..114 y199..213 | 306 / 206,6 / x182..230 y205..211 | +101,5 |
| 350 | | 274 / 102,8 | 353 / 207,7 | +104,9 |
| 374 | | 279 / 101,0 | 374 / 208,8 | +107,8 |
| 386 (letztes Bild vor der Ruhe) | -1205 | 272 / 100,8 / x90..111 y141..156 | 363 / 209,4 / x184..235 y148..154 | +108,6 |

In JEDEM Bild ab F314 mit beiden: 100 % der Granaten-Punkte links vom Sicherungs-Schwerpunkt, 100 % der
Sicherungs-Punkte rechts vom Granaten-Schwerpunkt; die bboxen ueberlappen nie (Granate endet bei x<=115,
Sicherung beginnt bei x>=182; Trennwand-Mitte in der Rechnung bei Schirm-x 155). Planung (`sitz_fach.py`)
gegen Messung in der Ruhe: Granate 273 / 272 Punkte, Schwerpunkt 101,1 / 100,8; Sicherung 367 / 363, 209,7 / 209,4.

### 4.2 Sichtbarkeit waehrend der Fahrt und im Ruhebild

* Vor der Hubfahrt (Plattform -305, Kuppel zu/oeffnend, F230..F302): **0 Punkte** beider Gegenstaende — der
  Kasten steckt noch im gemalten Schreibtisch, dessen Vorderkante (PRI-Maske) alles unter Schirm-y ~215
  verdeckt (die bbox-Unterkante der Granate bleibt F306..F318 bei y=215, waehrend sie auftaucht).
* Beide Faecher sind von der Kamera aus offen einzusehen (Kamera ueber der Tischplatte, Blick schraeg von
  oben durch die Vorderseite): Granate ab F306 (y=-515) teilweise, ab F318 voll (225..279 Punkte);
  Sicherung ab F314 (y=-595), ab F318 voll (254..376 Punkte), bis zur Ruhe oben. Kein Fach ist verdeckt.
* **Erstes Ruhebild F387** (Sicherungs-Dialog geht in diesem Bild auf, zeichnet ab F388; Lauf `ja_ja`, ungerade
  Bilder): F387 ist Punkt fuer Punkt gleich F386 aus dem MIT-Lauf und weicht vom OHNE-beide-Lauf in
  **635 = 272 + 363 Punkten** ab — beide Gegenstaende voll zu sehen, wenn der Dialog aufgeht.
* Waehrend des Sicherungs-Dialogs bleibt die Granate links sichtbar (259..272 Punkte, F398..F438); das
  Dialogbild liegt ueber der Bildmitte und dem rechten Fach.
* Rechnung (Riegel, §6): jede Sichtlinie jedes Modellpunkts zur Kamera schneidet die Vorderebene x=2 innerhalb
  der Oeffnung des EIGENEN Fachs, bei Plattform -1205 und -905 (0 von 587 / 0 von 1720 ausserhalb) —
  weder Trennwand noch Rahmen noch Tischplatte verdecken.

### 4.3 Bilder (angesehen)

* `hebetisch_belege/fahrt_bogen.jpg` — F306 (Granate taucht am Schreibtischrand auf), F314, F322, F340, F360,
  F380, F386 (Ruhe oben: Granate links im linken Fach, Sicherung quer im rechten Fach, beide auf dem
  Fachboden), F400 (Sicherungs-Dialog).
* `hebetisch_belege/fahrt_lupe_x4.jpg` — Faecher x4 in F322/F340/F386: Granate (Rautenmuster) links, Rohr rechts.
* `hebetisch_belege/ja_ja_bogen.jpg` — F387 erstes Ruhebild mit beiden, F391 Dialog-Zoom, F505 "Will you take
  the Fuse?" (Granate links noch da), F513 Granaten-Zoom, F601 "Will you take the Hand Grenade?" (rechtes Fach
  leer), F641 beide Faecher leer, F801/F861 Statusschirm mit Fuse und Hand Grenade x1.
* `hebetisch_belege/laden1151_bogen.jpg` — Lade-Weg ROOM1151 (F170 Granate taucht auf, F190/F220 beide, F246
  Ruhe, F250/F260/F300 Sicherungs-Dialog).

## 5. Abnahme

| Fall | Lauf | Ergebnis |
|---|---|---|
| Hubfahrt Tuerweg 1150 | `f_mit`/`f_ohneG`/`f_ohneS`/`f_ohneB` | Granate links, Sicherung rechts, in den Faechern, fahren mit (§4) |
| Dialog-Zeitpunkt | `hebetisch.log` | unveraendert: F386 letztes Setzen, F387 `ruht=1 pc=0x101B` Dialog auf, F388 zeichnet |
| Yes/Yes | `ja_ja` (`hebetisch_belege/ja_ja_debug_auszug.txt`) | `[sicherung] Yes: ... Flag (9,53) ... Platz 3`, `[granate] Yes: ... Flag (9,56) ... Platz 4`; F641/F653/F661/F669 gegen OHNE-beide F386: **0 Punkte** (Modelle weg), gegen MIT: 635; Statusschirm beide |
| No/No, Abfahrt | `nein_nein` gegen `ende_ohneB`, Versatz 329 (`ende_vergleich.py`, `hebetisch_belege/ende_nein_nein_mit_gegen_ohne.txt`) | beide liegen, sichtbar bis der Kasten in den Schreibtisch faehrt (F814 344, F822 36 Punkte), ab F830 (y=-455) bis zur Parklage **0 Punkte** — kein Durchscheinen/Durchstoss; `Fahrt zu Ende ... Sperre geloest` fuer beide |
| Lade-Weg 1150 | `laden1150_*` (CONTINUE, `RE15_FIRE_AOT=1@90`) | `Boot-Weg: Prop obj_id=4 / 7 im Pool`, Dialog `sub04-PC @0x101B`; links/rechts F170..F240 wie Tuerweg (F240: 273 / 100,9 gegen 352 / 208,8) |
| ROOM1151 (CONTINUE) | `laden1151_*` | Boot-Weg beide, Dialog `sub04-PC @0x0FF9`; F90..F250 Zahl fuer Zahl gleich 1150 (`diff` leer); Engine-Riegel unit_r32_hebetisch_faecher prueft 1151 eigens |

Messfalle: drei Laeufe endeten still mit rc=1 (debug.log bricht ohne Meldung ab, kein Eintrag im
Anwendungsprotokoll): der erste Basislauf bei F186 (Runde-31-Stand, vor jeder Aenderung) und zweimal `nein_nein`
um F611; die Wiederholung lief jeweils sauber bis EXIT_AT. Waehrend dessen bauten zwei andere Agenten. Dasselbe
Bild hatte Runde 31 (§3.6 dort). Als Last-Flattern gewertet, nicht als Befund — offen notiert (§7).

## 6. Riegel

**Neu `unit_r32_hebetisch_faecher`** (`tests/unit/probe_r32_hebetisch_faecher.c`,
`tests/unit/probes/r32_hebetisch_faecher.cmake`), je ROOM1150 und ROOM1151, Ausgabe
`hebetisch_belege/unit_r32_hebetisch_faecher.txt`, ALLES BESTANDEN:

| # | prueft | gemessen (1150 = 1151) |
|---|---|---|
| 1 | genau zwei Faecher aus den Prop-0-Bytes, je Rueckwand | A z 96..861 Boden -90 (Viereck 89, @0x12EE0 / 1151 @0x14F58), B z 950..1715 Boden -91 (Viereck 93) |
| 2 | Pool nach Raumstart = Konstanten, parent 0, Plattform rot_y 2048 | (-628,-145,478)/(-628,-117,1332) rot 1024 |
| 3 | liegen auf dem eigenen Fachboden | +0,00 / +0,00 |
| 4 | ganz im Fach (alle Abstaende > 0) | G 575/575/305/306/610, S 604/604/179/180/668 |
| 5 | Fach sonst leer | 0 von 131 Flaechen |
| 6 | Sichtlinien Cut 4 durch die eigene Oeffnung, Plattform -1205 und -905 | 0 von 587 / 0 von 1720 ausserhalb |
| 7 | Schirm: Granate ganz links, Sicherung ganz rechts der Trennwand | Ruhe: G bis 112,3 / Trennwand 155,3 / S ab 184,1 |

**Bewusst nachgezogen** (die alten Pruefungen nagelten den Runde-31-Sitz OBEN in der Kuppel fest — genau den
Sitz, den der Nutzer jetzt verwirft):

* `unit_r31_hebetisch`: Pruefungen 2..7 (Kuppelboden -1036, Luft unter geschlossener Kuppel/offenen Deckeln,
  Achteck, Granate<->Sicherung, links/rechts) **entfallen**; 1 (Ruhe-Fenster) und 8..13 (Zeitpunkt, Yes/Yes,
  Negativ-Kontrolle) unveraendert, Nummern beibehalten.
* `unit_r30_granate`: 4 "auf dem Kuppelboden -1036" -> "auf dem Boden von Fach A (Viereck 89, -90)";
  6 "Luft unter der Kuppel" -> "unter der Fachdecke (Viereck 87), zwischen x=2 und x=-1258"; 7 "Mitte in der
  Oeffnung 1110..1410" -> "ganz zwischen den Waenden z=96/861 (Viereck 86/88), links der Sicherung". 5 und
  8..14 unveraendert.
* `unit_r30_sicherung_sitz`: Fachboden Vierecke 79-81 (Achteck) -> Boden Fach B Viereck 93 (+ Waende 90/92);
  POS_X und jetzt auch POS_Z = Mitte des Fachbodens; "rechts der Naht" -> "rechts der Trennwand (Viereck 88)";
  Achse "naeher an z" -> "genau auf z"; "Mitte in der Oeffnung" gestrichen. Deckelweg-Bytes (150) und
  Pool-Werte bleiben.
* `unit_sicherung_1150`: nur der Text von Pruefung 4 (y=-117 = -91 - 26).
* `RE15_MIN_TESTS` 411 -> 412 (`tools/local_build.sh` Z. 63 und 334/335).

**Mutationsproben** (Header geaendert, gebaut, gefahren, per `git checkout` zurueck):

| Mutation | Ergebnis |
|---|---|
| M1: Runde-31-Sitze (Kuppel) | unit_r32 3/4/6/7 ROT; unit_r30_granate 4/6/7 ROT; unit_r30_sicherung_sitz POS_X/POS_Y/POS_Z/ROT_Y ROT |
| M2: gespiegelt (Granate z 1332 y -146, Sicherung z 478 y -116) | unit_r32 3/4/6/7 ROT; unit_r30_granate 4/7 ROT; unit_r30_sicherung_sitz POS_Y/POS_Z ROT |

## 7. Offen

* **Messfalle stiller Abbruch** (§5): 3 von rund 25 Laeufen endeten mit rc=1 ohne Meldung; Wiederholung sauber.
  Nicht untersucht (kein Absturzeintrag); Runde 31 sah dasselbe.
* **Granate dunkel**: im Fach wirkt die Granate dunkelgruen auf dunklem Grund (Raumlicht Cut 4, unveraendert);
  erkennbar (Rautenmuster, 259..279 Punkte), aber kontrastarm. Licht ist nicht Teil des Auftrags.
* **Sitz ist PORT-WAHL** ohne Original-Adresse; belegt sind die Geometrie-Bytes (§2), Messung (§4) und Riegel (§6).
* **Tuerweg ROOM1151** wie Runde 31 nur ueber CONTINUE und Engine-Riegel (Debug-Sprung waehlt 1150).
* **PSX-Ziel** wie Runde 30/31: kein Lader fuer die Zusatz-Props. Keine neue Engine-Quelldatei (nur Header,
  Kommentar, Tests); die einzige neue .c-Datei ist die Probe unter `tests/unit/` (eigene probes-cmake).
* `tests/unit/probe_r30_sicherung_variante.c` (nur mit `-DRE15_R30_SICHERUNG_VARIANTE=ON`, nicht in der Suite)
  liest die Konstanten und folgt dem neuen Sitz ohne Aenderung.

## 8. Suite / Commits

(folgt)
