# Runde 35 Spur G "karte" — Abnahme 0 (unabhaengig)

Baum `.claude/worktrees/r35_karte`, Zweig `r35/karte`, HEAD **8fee1bb4** (Basis 154a73c1).
Gebaut: `bash re15_port/tools/local_build.sh configure` + `build` -> `ninja: no work to do`,
`=== LOCAL-BUILD-OK (build)`; die gemessene exe ist also der Stand 8fee1bb4.
Gegen master 6e1a3771 (inzwischen weiter): `git merge-tree --write-tree master HEAD` konfliktfrei;
die anderen r35-Zweige fassen `re15_map_zones.{c,h}` und `test_map_etagenzeile.c` nicht an.

**Ergebnis: NICHT BESTANDEN.** Punkte 2, 3, 4 erfuellt (am echten Tuerweg gemessen). Punkt 1
(Fahrstuhl-Cursor) ist nicht erfuellt: der Marker bewegt sich ueber die ganze Kabine um 2 px je
Achse - nicht mehr als vorher (vorher x 5 px, y 2 px).

---

## Messwerkzeug (selbst, nicht das des Bau-Agenten)

`scratchpad/abn0/lauf.sh <name> <kartenargs> <skript> <raum> <open_n> <exit_n>`:
1. Spielstand Slot 0 mit `probe_r35_karte karte` (Raum, x, z, Yaw, besucht:..., flag:b:i).
2. Eigene exe-Kopie `re15_pc_abn0g_<name>.exe` (danach geloescht), Umgebung
   `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
   RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
   RE15_INPUT_SCRIPT=<skript> RE15_INV_OPEN_AT=<n>#<raum> RE15_INV_FB_SHOT=karte.bmp
   RE15_INV_FB_SHOT_AT=60 RE15_EXIT_AT=<m>#<raum>`.
3. Auswertung: debug.log (`[aot] DOOR FIRE`, `[room] PC loaded`, `[walk] ... pl pos`), BMP 320x240.

ECHTER WEG fuer 2/3/4: der Spielstand steht im NACHBARRAUM vor der Tuer, die Aktionstaste
(`A0.3`, Bild 90 und 129) loest die Tuer-AOT aus, der Raumwechsel laeuft ueber die
Tuersequenz, die Karte wird erst im Zielraum geoeffnet (anders als `integration_r35_karte` A-D,
die per LOAD GAME direkt im Zielraum starten). Tuer-Datensaetze selbst aus den RDTs gelesen:

| Tuer | Datensatz | Rechteck (Ecke+Groesse) | Spielstand |
|---|---|---|---|
| 11E0 -> 11F0 | ROOM11E0.RDT @0x1512 slot 0 | (-27100,-10300,4000,1500) | 11E0 (-24707,-9442) Yaw 1024 |
| 11E0 -> 1200 | ROOM11E0.RDT @0x1532 slot 1 | (1000,-25000,2000,1500) | 11E0 (2000,-24250) Yaw 3072 |
| 11E0 -> 1210 | ROOM11E0.RDT @0x16C4 slot 3 (Else-Zweig von `06 00 4e 01 21 03 8b 00` @0x1572 = Ck(3,139,0)) | (9700,4500,1500,2000) | 11E0 (10450,5500) Yaw 3072, flag:3:139 |
| 11D0 -> 1230 | ROOM11D0.RDT @0x1206 (Else-Zweig von `06 00 28 00 21 04 f3 00` @0x11DA = Ck(4,243,0); If-Zweig @0x11E2 -> 1180) | (-1300,-17000,2000,1000) | 11D0 (-300,-17100) Yaw 3072, flag:4:243 |
| 1040 -> 1080 | ROOM1040.RDT @0x1096 slot 0 | (-22936,-11000,2300,1400) | 1040 (-21786,-10500) Yaw 3072, flag:4:243 |
| 10C0 -> 1080 | ROOM10C0.RDT @0xE82 slot 1 | (500,5900,2000,1000) | 10C0 (1450,6600) Yaw 1024, flag:4:243 |

Erster 1210-Lauf ohne flag:3:139: Tuer feuerte nicht (debug.log 400 s lang `pl pos=(10450,0,5500)`)
- das ist die Original-Sperre @0x1572, kein Port-Fehler; mit dem Bit faehrt der Weg durch.

---

## Punkt 1 — "Beim Elevator ROOM 1080 bewegt sich auf der Map der Player Cursor nicht."

**Urteil: nicht_erfuellt.**

Messung (6 Laeufe, echter Weg Etagenraum -> Tuer -> Kabine; danach im Raum 1080 gelaufen;
Karte nach 200/230 Bildern in 1080; Marker = Bbox der Nicht-Rot/Nicht-Wand-Pixel im Kabinenkasten
x110..117 y135..142, `scratchpad/abn0/marker.py`):

| Lauf | Skript | Spielerlage beim Oeffnen (debug.log) | Blatt | Marker-Mitte |
|---|---|---|---|---|
| F1_steh | W1,A0.3,W1,A0.3 | (-13650,-900) | POLICE STATION 1F | (114,140) |
| F1_vor | ... ,U2 | (-13650,-3682) | 1F | (114,138) |
| F1_rechts | ... ,R0.6,U2 | (-15282,-518) | 1F | (115,140) |
| F1_links | ... ,L0.6,U2 | (-12018,-518) | 1F | (113,140) |
| F2_steh | W1,A0.3,W1,A0.3 (aus 10C0) | (-13650,-900) | POLICE STATION 2F | (114,140) |
| F2_vor | ... ,U2 | (-13650,-3682) | 2F | (114,138) |

Der Spieler durchmisst 3264 Welteinheiten quer (78 % der Innenbreite 4200) und 3164 laengs -
der Marker bewegt sich dabei **2 px in x (113..115) und 2 px in y (138..140)** im 320x240-Bild.
Das Marker-Symbol selbst ist 5x5 px; es verschiebt sich ueber die GANZE Kabine um weniger als
seine halbe Breite.

Vorher (Stand 154a73c1): alte Zeile `{0x1080, ..., 2, 9, 0, 9, 78, 214, 2080, 2320, 0, 0}`
(`git show 154a73c1:re15_port/engine/src/re15_map_zones.h` Z. 78) mit derselben Formel und
derselben Klemmung (rect 9 (109,134,16,16) + Reserve 4 -> x113..121 y138..146) fuer genau diese
Lagen: steh (114,145), vor (114,146), rechts (113,144), links (118,144) = **x 5 px, y 2 px**;
das Dossier des Bau-Agenten misst vorher dasselbe ("x 113..118 (5 px), y 144..146 (2 px)").

Das ist der Kern: der Nutzer meldet, dass der Cursor sich NICHT BEWEGT. Der Fix aendert WO er
steht (jetzt in der gemalten Kabine statt darunter) und AUF WELCHEM BLATT (Etage des Vorraums,
1F/2F gemessen), aber nicht, DASS er sich kaum bewegt - die Querbewegung ist sogar von 5 auf 2 px
geschrumpft. Die neue Abbildung (ox,oy,sx,sy = 129,117,795,805, PORT-WAHL) staucht das
Kollisions-Innere x -15750..-11550 / z -4150..-50 auf 4 px; der Spieler erreicht davon wegen
seines Radius nur rund die Haelfte -> 2 px. Der Bau-Agent fuehrt das selbst unter OFFEN 2
("4 px Weg je Achse ... hier bewusst nicht gemacht") und meldet trotzdem "fertig"; die
Dossier-Zeile "folgt dem Spieler in beiden Achsen (4 px Fenster ...)" ueberzeichnet - seine
eigenen Ecken SW(116,139) SO(114,139) NW(116,141) NO(114,141) sind ebenfalls 2 px.

Was erfuellt ist (Teilwirkung, nicht der gemeldete Punkt): Blatt = Etage des Einstiegs
(1F aus 1040, 2F aus 10C0 gemessen; 3F aus 1120 im Bild nachher_E des Bau-Agenten), Marker im
gemalten Innenraum, Bewegungsrichtung 180 Grad gedreht (links/rechts gespiegelt, vorwaerts =
Bild oben).

Bilder: `scratchpad/abn0/run_F1_*/karte.bmp`, `run_F2_*/karte.bmp` (nicht im Baum).

## Punkt 2 — "ROOM 11F0 / ROOM 1200 taucht nicht auf der Karte auf, wenn man drin ist."

**Urteil: erfuellt.**

* 11E0 -> Tuer @0x1512 -> 11F0 (`[aot] DOOR FIRE slot=0 rect=(-25100,-9550,hw=2000,hh=750)
  ... spawn=(250,0,250)`, `[room] PC loaded room11f0.rdt`), Karte in 11F0: Blatt
  "POLICE STATION B2", aktuelle Fuellung (gemessene Farbe (48,8,48)/(48,8,64)) **1392 px, Bbox
  (101,102)-(132,154)** = rect 1 (100,101,40,56); ausserhalb davon 0 rote Pixel; Marker oben
  links in der Flaeche. Vorher (Dossier): 11F0 auf rect 0 = Garage von 11E0.
* 11E0 -> Tuer @0x1532 -> 1200 (`DOOR FIRE slot=1 ... spawn=(-20154,0,-25245)`): Blatt B2,
  rot **995 px, Bbox (142,102)-(169,137)** = rect 2 (141,101,32,40); Marker an der Tuernische
  unten rechts.
* Gegenprobe: 11F0 bzw. 1200 sind beim jeweils anderen Lauf (nicht besucht) nicht gezeichnet,
  11E0 gruen (besucht) - die eigene Kachel erscheint, sobald man drin ist.

RE-Pruefung: Massstabszeile 1200 @0x800769b0 = (129,150,3168,2305), 11F0 @0x800769a8 = Stub
(0,0,1,1) (selbst gelesen, `re15_disasm.py read ... --w 2`). Rechtecktabelle Blatt 1 @0x800762f4
(Zaehler 10 @0x80076848): r1 (100,101,40,56) uv(0,32), r2 (141,101,32,40) uv(40,32) - stimmt.

## Punkt 3 — "In ROOM 1230 bekomme ich die Map von ROOM 11E0."

**Urteil: erfuellt.**

11D0 (flag 4:243) -> Tuer @0x1206 -> 1230 (`DOOR FIRE slot=0 rect=(-300,-16500,hw=1000,hh=500)
... spawn=(-4729,0,-16018)`, `[room] PC loaded room1230.rdt`). Karte in 1230: Blatt
**"POLICE STATION B1"** (nicht mehr B2 = Blatt von 11E0), rot **1032 px, Bbox (121,61)-(167,123)**
= der Gang rect 0 (120,60,56,72); Marker am Suedende neben der 11D0-Tuer; 11D0 (besucht) gruen
darunter.

RE-Pruefung: Seiten-Setzer `FUN_8004b568` selbst disassembliert: `lh v1,0x0fe2` @0x8004b56c,
`sltiu v0,v1,0x26` @0x8004b574, Tabelle @0x8001103c: [35] -> 0x8004b854, faellt durch bis
`ori v0,zero,0x1` @0x8004b884, `sb v0,0x260e` @0x8004b88c -> Blatt 1; [24..29] enden in
`sb zero,0x260e` @0x8004b7dc -> Blatt 0. Das ORIGINAL zeigt in 1230 also tatsaechlich B2 (mit
Stub-Zeile @0x800769c8 (0,0,1,1)) - das ist exakt der Nutzerbefund; der Fix weicht bewusst vom
Original ab und begruendet es mit "1230 = Variante von 1180" (Tuer-Datensaetze selbst verglichen:
1180 @0x9EE/0xA12/0xA34/0xA54/0xA74 == 1230 @0xAEE/0xB12/0xB34/0xB54/0xB74, gleiche Rechtecke,
Spawns, Ziele). Gekennzeichnet im Code (re15_map_zones.h, Block "ROOM1230 IST ROOM1180").

## Punkt 4 — "In ROOM 1210 ist der Korridor falsch und so gut wie alle Tueren fehlen"

**Urteil: erfuellt.**

11E0 (flag 3:139) -> Tuer @0x16C4 -> 1210 (`DOOR FIRE slot=3 rect=(10450,5500,hw=750,hh=1000)
... spawn=(-26400,0,-2200)`, `[room] PC loaded room1210.rdt`). Karte in 1210: Blatt B2, rot
**763 px, Bbox (188,71)-(225,128)** = der gemalte T-Korridor in rect 3 (187,70,64,80) (vorher:
1210 auf rect 4 = eine 16x24-Zelle). Gelbe RE2-Tuerbalken (224,168,40) im Abzug, Pixel fuer
Pixel ausgelesen:

| Balken | Lage | Tuer-Datensatz ROOM1210.RDT |
|---|---|---|
| x187 y72..76 | West-Ende Querbalken | @0x1CC6 -> 11E0 |
| x201 y80..84 | Stamm West | @0x1CE6 -> 1220 Zelle |
| x212 y89..93 | Stamm Ost | @0x1D06 -> 1220 |
| x201 y97..101 | Stamm West | @0x1D26 -> 1220 |
| x212 y106..110 | Stamm Ost | @0x1D46 -> 1220 |
| x212 y123..127 | Stamm Ost | @0x1D66 -> 1220 |

= alle sechs Tueren des Raums (Datensaetze selbst geparst, 6 Stueck), jede als Balken auf der
Korridorwand; Marker an der 11E0-Tuer (Ankunft). RE-Pruefung: Zeile @0x800769b8 =
(177,140,2496,2250) selbst gelesen; Tuer @0x1CC6 Mitte (-27250,-2400) -> (188,76) nachgerechnet
(Marke (187,74)).

---

## Gate: Suite

**Gruen.** Eigener Lauf `bash re15_port/tools/local_build.sh test` (nach configure + build, kein
Neubau noetig), 2026-10-04 01:20-01:41:
`100% tests passed, 0 tests failed out of 483` / `=== LOCAL-BUILD-OK (test) — Tests 483/483`.
Keiner der fuenf Fenster-Haken musste nachgefahren werden (alle im Lauf gruen). Darin
unit_r35_karte_fahrstuhl/_b2/_r1230/_r1210 Passed, integration_r35_karte Passed (70 s),
integration_map_raum_live Passed, integration_map_uebergang Passed, unit_map_etagenzeile Passed.
Einzeln nachgefahren: `ctest -R unit_r35_karte` 4/4.

Hinweis: Ein fremder `local_build.sh all` lief bei Beginn dieser Abnahme bereits im selben
build/ (Log 00:47-01:19, 23 Ausfaelle ab Test 461, durchweg Zeitueberschreitungen bzw.
`0xc0000142` unter Last). Gewartet bis zu dessen Ende, dann selbst gebaut und getestet; der fremde
Lauf ist nicht gewertet.

Die Suite-Angabe des Bau-Agenten "484/484" ist mit diesem Baum NICHT erzeugbar:
`ctest -N` meldet `Total Tests: 483` (Basis 478 + 5 neue).

## Gate: @0x / RE-Belege

Stichproben SELBST disassembliert / gelesen (info/Re1.5/PSX.EXE bzw. RDT-Bytes):
1. Seiten-Setzer `FUN_8004b568` (s. Punkt 3) - Behauptung B1 stimmt (0..11 -> 2 @0x8004b684,
   12..17 -> 3 @0x8004b6f8, 18..22 -> 4 @0x8004b758, 24..29 -> 0 @0x8004b7dc, 30..37 -> 1
   @0x8004b884).
2. `FUN_8001d600` Tuer-Zweig: `lhu v0,0x0fe2` @0x8001d92c, `sh v0,0x0fe6` @0x8001d938,
   `sh v0,0x0fe2` @0x8001d95c (aus `lbu v0,9(a0)` @0x8001d94c) - alter Raum wird vor dem neuen
   gesichert; stimmt.
3. Massstabszeilen @0x800768b0+8*Index: 1080 @0x800768f0 = (0,0,1,1) Stub, 11E0 @0x800769a0 =
   (91,157,2304,2240), 11F0/1220/1230/1180 Stubs, 1200/1210/1190 wie im Dossier; stimmt.
4. Rechtecktabellen @0x80076840 (Zaehler 7/10/11 ...), Blatt 0 @0x800762a0, Blatt 1
   @0x800762f4, Blatt 2 r9 @0x800763d8 = (109,134,16,16) uv(168,40); stimmt.
5. RDT-Bytes: ROOM1040 @0x15D6 `22 03 36 01`, @0x15C8 `21 03 36 00`, @0x15D0 `23 00 0a 00 05 00`;
   ROOM10C0 @0x0FEE `22 03 37 01`, @0x1008 `22 03 36 00`, @0x1022 `22 03 38 00`; ROOM1120
   @0x0D6C `22 03 38 01`, @0x0D86 `22 03 36 00`, @0x0DA0 `22 03 37 00`; ROOM1080 @0x08AC/BC/CC
   `21 03 36/37/38 01`; stimmt.

Diff-Suche (gegen Basis 154a73c1) nach deferred/tunable/interim/for now/faithful/plausib/TODO/
vermutlich/getenv: **0 Treffer**. Keine Env-Schalter.

Konstanten ohne @0x, aber als PORT-WAHL gekennzeichnet: 1080-Zeilen (129,117,795,805 /
147,120,795,805, flip 1,1), die fuenf 1180/1230-Abschnitte (Kaesten + Zeilen). Daten aus der
Kunst: Marken-Pixel (gemalte Nischen in MAP01/02.PIX) mit Tuer-Offsets.

Befund zum Gate (kein Ausschluss, aber nachzutragen):
* G1 - 1220-Zellkaesten (re15_map_zones.h, Zeilen 0x1220 idx 0..4): "Teiler aus der SCA:
  z -4500/-11925/-19875, Waende x -21836/-17275" ohne Datei-Byte-Offset der SCA-Eintraege.
* G2 - Die Blattwahl der Kabine nach dem Vorraum (karte_fahrstuhl_1080.c) ist eine Abweichung
  vom Original: der Setzer schickt Raumindex 8 IMMER auf Blatt 2 (Fall 0..11 ->
  `ori v0,zero,0x2` @0x8004b684); das Original liest DAT_800b0fe6 fuer die Karte nicht. Die
  Konstanten 2/3/4 sind belegt, der Mechanismus selbst ist eine PORT-WAHL und im Code nicht so
  gekennzeichnet.

Erklaert der Fix den Befund? Punkte 2/3/4: ja (gemessene Vorbedingung: falsches Rechteck bzw.
falsches Blatt; nach dem Fix gemessen behoben). Punkt 1: **nein** - die im Dossier gemessene
Vorbedingung "Marker y 144..146 UNTER der Kabine" erklaert nicht, dass er sich nicht bewegt; die
Bewegung war vorher 5x2 px und ist jetzt 2x2 px.

## Gate: Vertrag / Pfade

`git diff --name-only 154a73c1 HEAD`: analysis/befunde_runde35/G_karte*, engine/src/
karte_fahrstuhl_1080.c (neu), include/re15_karte_fahrstuhl.h (neu), engine/src/re15_map_zones.c
(1 include + 1 Haken + 2x2 Filterzeilen + angehaengte Tabellenzeilen), engine/src/re15_map_zones.h,
tests/unit/probes/r35_karte.cmake, tests/unit/test_r35_karte.c, tests/integration/
test_r35_karte.cmake, tests/unit/test_map_etagenzeile.c. Keine release/, platform/android/,
shared_assets/PSX/, keine tests/*/CMakeLists.txt. Keine Bank-9-Bits, keine Nachrichten-IDs, keine
AOT-Slots/Ereignisse belegt (Spur G hat laut VERTRAG keine). Neue Kartendaten: zid 102..109
(eindeutig, Zensus ueber 260 Zeilen), Besucht-Zusatzbits 242..250 von 256.

Abweichungen (dokumentiert im Dossier, gewertet als Hinweis, nicht als Ausschluss):
* `re15_map_zones.h` ist gemeinsame Datei; geaendert sind ~120 Datenzeilen statt "1-5 Zeilen
  Haken". Ohne diese Zeilen ist der Auftrag nicht loesbar; kein anderer Zweig fasst die Datei an
  (geprueft fuer cut10f0, cut1150, integration, raeume, affen, entladen, inventar1050).
* Fremder Pin `test_map_etagenzeile.c` gelockert ("Gast-Zeile darf eine blatteigene Zeile
  tragen, wenn der Kasten-Mittelpunkt ins eigene Rechteck projiziert"); die Wirkungsprobe
  (>= 10 Markerpixel) bleibt. Begruendet im Pin.

## Gate: Tests

Vorhanden: unit_r35_karte_fahrstuhl / _b2 / _r1230 / _r1210, integration_r35_karte (SIEHE
SUITE). Messend fuer 2/3/4 (Rechteck-Zustand, Marker auf gemalter Flaeche, 6 Tuer-AOTs mit
sichtbarer Marke <= 3 px). Fuer Punkt 1 misst der Riegel NICHT das, was der Nutzer meldet:
`unit_r35_karte_fahrstuhl` prueft nur, dass die vier Ecken im gemalten Innenraum liegen und
streng geordnet sind (`ecke[1].mx < ecke[0].mx` usw. = >= 1 px); ein Marker, der sich 1 px
bewegt, ist gruen. integration_r35_karte E macht EIN Bild an EINER Stelle und kann Bewegung gar
nicht sehen. integration_r35_karte A-D starten per LOAD GAME direkt im Zielraum (kein Tuerweg).

---

## Urteile

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Fahrstuhl-Cursor 1080 | **nicht_erfuellt** | exe, 6 Laeufe: Marker x 113..115, y 138..140 ueber 3264 x 3164 Welteinheiten; vorher x 5 px / y 2 px |
| 2 11F0 / 1200 erscheinen | erfuellt | Tuerweg aus 11E0: rot nur rect 1 (1392 px) bzw. rect 2 (995 px), Blatt B2 |
| 3 1230 zeigt 11E0-Karte | erfuellt | Tuerweg aus 11D0: Blatt B1, rot nur Gang rect 0 (1032 px) |
| 4 1210 Korridor + Tueren | erfuellt | Tuerweg aus 11E0: rot T-Korridor rect 3 (763 px), 6 gelbe Balken = 6 Tuer-Datensaetze |

Gates: Suite gruen (483/483). @0x-Gate haelt (Stichproben stimmen, keine Rate-Woerter), mit
Nachtraegen G1/G2. Pfad-Gate haelt (dokumentierte Abweichung gemeinsame Datei). Tests vorhanden,
fuer Punkt 1 aber nicht messend (M2).

## Maengel

1. **M1 - Punkt 1 nicht erfuellt: der Fahrstuhl-Cursor bewegt sich weiterhin praktisch nicht.**
   Gemessen an der exe (Laeufe F1_steh/vor/rechts/links, F2_steh/vor; Befehl und Umgebung oben):
   ueber den ganzen begehbaren Kabinenraum (x -15282..-12018, z -3682..-518, aus `[walk] pl pos`)
   wandert die Marker-Mitte nur von x 113 bis 115 und y 138 bis 140 (2 px je Achse, Symbol 5x5 px).
   Vorher (alte Zeile 78,214,2080,2320 an denselben Lagen, sowie Dossier "vorher") x 113..118 /
   y 144..146. Die Querbewegung ist also KLEINER geworden, die Laengsbewegung gleich. Ursache im
   Code: re15_map_zones.h Zeilen `0x1080/0x1081` (Blatt 2/3: 129,117,795,805; Blatt 4:
   147,120,795,805) bilden das KOLLISIONS-Innere (-15750..-11550 / -4150..-50) auf 4 px ab, wovon
   der Spieler nur die Haelfte erreicht; dazu die Klemmung re15_inv_screen.c:856-864
   (Rechteck + Reserve 4). Was fehlt: eine Abbildung, unter der der Marker beim Durchlaufen der
   Kabine sichtbar wandert, und deren Herleitung. Naechster Messweg: wie zeigt RE2 Retail den
   Spieler in einem gleich kleinen Kartenkasten (Beta -> Retail, da die RE1.5-Zeile @0x800768f0
   ein Stub {0,0,1,1} ist) - Marker-Groesse, Klemmung, Massstab -, dann Abbildung auf den
   BEGEHBAREN Bereich statt auf die Kollisionswand; Abnahme mit demselben Lauf (Lagen oben),
   Soll = sichtbar mehr als die 5x2 px des alten Stands.
2. **M2 - Kein Riegel misst das Nutzer-Symptom von Punkt 1.** `unit_r35_karte_fahrstuhl`
   (tests/unit/test_r35_karte.c:297-325) prueft "Ecke im gemalten Innenraum" und eine strenge
   Ordnung (`ecke[1].mx < ecke[0].mx` ...), also >= 1 px - der heutige 2-px-Stand ist gruen, ein
   1-px-Stand waere es auch. `integration_r35_karte` Lauf E macht ein einziges Bild an einer
   Stelle. Noetig: ein Riegel auf die Bewegungsweite ueber den begehbaren Bereich (mindestens
   zwei Lagen am echten Weg, Bilddifferenz oder Markerlage), der am alten UND am heutigen Stand
   ROT ist.
3. **M3 - Suite-Angabe nicht reproduzierbar.** Dossier Zeile 247 und Abschlussmeldung:
   "Tests 484/484"; dieser Baum registriert 483 Tests (`ctest -N`), eigener Lauf 483/483.
   Angabe korrigieren.
4. **M4 - Dossier ueberzeichnet Punkt 1.** G_karte.md "Messung nachher": "folgt dem Spieler in
   beiden Achsen (4 px Fenster ...)" und "Fuer den Nutzer": "bewegt sich mit". Die eigenen
   Riegel-Ecken (116,139)/(114,139)/(116,141)/(114,141) und die exe-Messung zeigen 2 px.
5. **G1 (Nachtrag @0x-Gate)** - 1220-Zellkaesten (re15_map_zones.h, `0x1220` idx 0..4): die
   Teiler "z -4500/-11925/-19875, Waende x -21836/-17275" ohne Byte-Offset der SCA-Eintraege in
   ROOM1220.RDT/ROOM1210.RDT nachtragen.
6. **G2 (Nachtrag Kennzeichnung)** - karte_fahrstuhl_1080.c: die Blattwahl nach dem Vorraum
   weicht vom Original ab (Setzer Fall 8 -> konstant Blatt 2, `ori v0,zero,0x2` @0x8004b684;
   DAT_800b0fe6 wird von der Karte im Original nicht gelesen). Als PORT-WAHL kennzeichnen.

**bestanden = false** (Punkt 1 nicht erfuellt, M1/M2).
