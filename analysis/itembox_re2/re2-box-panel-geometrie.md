# RE2s Box-Panel — Geometrie vollstaendig ausgelesen (2026-08-30)

Anlass: Nutzer — "ich moechte gefaelligst das du das ORIGINAL von RE 2 sauber reverse
engineerst und portest".

## 1. Die beiden Tabellen und ihre PAARUNG (belegt, nicht angenommen)

- **UV/Groesse** `@0x800a9bc4`, Schrittweite 4: `{u, v, w, h}` (je 1 Byte), 25 Eintraege.
- **Position** `@0x800a9c38`, Schrittweite 4: `{x, y}` (je s16).

Die Paarung ergibt sich aus den Laufrichtungen:

- Template-Builder `FUN_80070e58`: `iVar6 = 0x60`, Schritt **-4**, liest
  `(&UNK_800a9bc4)[iVar6]` — laeuft die UV-Tabelle **rueckwaerts**, waehrend der
  Prim-Zeiger vorwaerts laeuft (`puVar7 += 0x14`, zwei Prims je Runde = Doppelpuffer).
- Zeichner `FUN_800710dc`: `iVar8 = 0x40`, Schritt **-4**, liest
  `*(short *)(&DAT_800a9c34 + iVar8)` — laeuft die Positionstabelle **ebenfalls
  rueckwaerts**, Prim-Zeiger vorwaerts (`puVar22 += 0x28`).

Beide Laeufe sind gleichsinnig ⇒ **UV[i] gehoert zu XY[i]**.

## 2. Die 16 Rahmenteile (Panel-Ursprung (7,14), Ausdehnung 211x153)

| XY | UV | Groesse | Rolle |
|---|---|---|---|
| (3,0) | (0,248) | 128x4 | obere Leiste, linker Teil |
| (131,0) | (0,240) | 77x4 | obere Leiste, rechter Teil |
| (3,149) | (0,252) | 128x4 | untere Leiste, links |
| (131,149) | (0,244) | 77x4 | untere Leiste, rechts |
| (0,0) | (12,60) | 3x153 | linker Pfosten |
| (208,0) | (15,60) | 3x153 | rechter Pfosten |
| (4,6) | (8,60) | 2x142 | innere Senkrechte links |
| (196,6) | (10,60) | 2x142 | innere Senkrechte rechts |
| (6,6) | (0,0) | 128x20 | Kopfband links |
| (134,6) | (0,40) | 62x20 | Kopfband rechts |
| (6,127) | (0,20) | 128x20 | Fussband links |
| (134,127) | (62,40) | 62x20 | Fussband rechts |
| (199,11) | (0,60) | 8x130 | Schiene des Scrollbalkens |
| (199,4) | (0,202) | 8x6 | Pfeilfeld oben |
| (199,142) | (0,214) | 8x6 | Pfeilfeld unten |
| (211,0) | (0,228) | 39x12 | Beschriftungsfeld rechts aussen |

Dazu:
- **L1/R1-Anzeigen**: (94,5) und (94,127), je **20x20** (UV (20,78) / (20,116)).
  Helligkeit haengt am Tastenzustand (0x50/0x3c fuer HOCH|L1, 0x78/0x64 fuer RUNTER|R1).
- **5 Scrollbalken-Marken**: UV (0,190) **6x2**, Position (200,12) plus
  `((DAT_800d5c14 + k) & 0x3f) * 2` in y — 2 px je Ring-Platz.
- Zwei Sonder-Prims bei (248,0) und (252,1) mit eigenen CLUTs
  (`GetClut(0,0x1e4)` bzw. `GetClut(0x100,0x1f0)`).

## 3. Zeilen-Symbole

RE2 zeichnet je Zeile **25x19** (2-Feld-Waffen **49x19**) — passt in das 20-px-Raster.
Eine 40x30-Kachel wie unsere ueberlappt dort zwangslaeufig die Nachbarzeile
(genau der gemeldete Fehler "die Item Images ueberlagern sich").

## 4. ⛔ Was NICHT bestimmbar war: die Bildquelle

Die Rahmen sind **SPRT** (Code 0x64) und setzen **keinen eigenen TPage** — sie erben
den Texturbereich aus dem Speicherzustand des Menues. `FUN_80070e58` setzt fuer sie
nur CLUT (`GetClut(0,0x1e7)` = VRAM (0,487)), Groesse und Farbe.

Ausgeschlossen (mit den echten UVs offline gerendert und angesehen): **ST0.TIM /
ST1.TIM** — dort liegen an diesen Stellen die Item-Symbole, keine Rahmenteile.
Ebenfalls dagegen: ST0/ST1 bringen nur die CLUT-Reihen 480..483 mit, die Rahmen
brauchen 487.

⇒ Ohne einen **Speicherauszug aus laufendem RE2** (Box geoeffnet, VRAM sichern) ist
die Quelle nicht zu bestimmen. Bis dahin zeichnet der Port die Teile in ihren
Original-MASSEN als Flaechen in Menuefarben: die Form stammt aus RE2, die Fuellung
nicht. Das ist im Code so benannt und wird nicht als RE2-Textur ausgegeben.

## 5. Emulator-Sitzung 2026-08-31 — was gefunden wurde, was noch fehlt

Der Nutzer hat das RE2-Abbild freigegeben; DuckStation-Lauf mit virtuellem Pad.

**Neuer harter Befund — die vom Item-Schirm geladenen CD-Dateien.**
Alle Aufrufe des CD-Laders `FUN_80012fb8(datei_id, ziel, …)` im Menuecode
(0x80068000-0x80080000) ausgelesen; Datei-Tabelle @0x800988a8, Stride 8
(Bytes 0..2 = Startsektor, Bytes 4..7 = Groesse):

| Datei-ID | Sektor | Groesse | Aufrufer | Identitaet |
|---|---|---|---|---|
| **210** | 47057 | 77536 | @0x8006855c | **ST0.TIM** (sha256 gegen COMMON/DATA verifiziert) |
| 169 | 184594074-Rec | 1228800 | @0x800685a8, @0x80068a38 | Item-Symbol-Cache (Ziel 0x8019c000) |
| 172 | 45681 | 102944 | 5 Stellen | TIM, nicht im DATA-Ordner |
| 223 | 47321 | 9824 | 3 Stellen | TIM 8bpp 128x144, 3 CLUT-Reihen |
| 166 | 42049 | 153620 | @0x8006d530 | TIM (TYPE00-Groesse) |

Aus dem CD-Abbild extrahiert (`Track 1.bin`, Rohsektor 2352, Nutzdaten ab +24)
und per sha256 zugeordnet — **Datei 210 IST ST0.TIM**, byte-identisch.

**Aber: die Panel-Koordinaten treffen in ST0 NICHT auf Rahmenteile.**
Die sechs markanten Regionen der UV-Tabelle ((0,248) 128x4 "Leiste", (12,60) 3x153
"Pfosten", (0,60) 8x130 "Schiene", (0,0) 128x20 "Kopfband", …) wurden aus ST0
vergroessert gerendert (`scratchpad/st0_regions.png`): dort liegen Item-Symbole
(Pistole, Messer) und leere Flaechen — keine Leisten.
⇒ Entweder gehoert die UV-Tabelle @0x800a9bc4 zu einem anderen Schirm, oder die
Rahmen-Sprites samplen eine Textur, die erst zur Laufzeit an anderer VRAM-Stelle
steht. Die GEOMETRIE (§2) bleibt davon unberuehrt — sie stammt aus dem Zeichner.

**Der Ausleseweg steht:** Ein DuckStation-Savestate traegt den kompletten VRAM;
er beginnt unmittelbar hinter dem laengenpraefixierten ASCII-Tag `GPU-VRAM`
(1024x512x2 B, RGB555 linear). Werkzeug: `scratchpad/vram.py` (dekomprimiert die
.sav via zstd, schneidet einen Bereich aus, schreibt PNG). Live-Savestates lassen
sich per Pad-Knopf ziehen (`[Hotkeys] SaveSelectedSaveState = SDL-0/LeftShoulder`),
ohne den Emulator zu schliessen — damit hat man waehrend eines Laufs "Augen".
⚠️ Der Screenshot-Hotkey feuerte unzuverlaessig; der Savestate-Weg ist robuster.
⚠️ `re15_ss.py` bricht bei RE2 ab (sucht die RE1.5-EXE-Signatur) — nur der
GPU-VRAM-Tag wird gebraucht, nicht die RAM-Basis.

**Was fehlt:** ein Savestate mit GEOEFFNETEM Item-/Box-Schirm. Die Navigation
scheitert bisher an RE2s Attract-Demo: sie uebernimmt den Titelbildschirm, spielt
Spielszenen (Eingangshalle) und faellt danach zurueck, sodass ein automatisierter
Tastenlauf wiederholt im Titel oder in der Speicherkarten-Abfrage endet.
Sobald ein solcher Savestate vorliegt, ist die Rahmen-Textur in einem Schritt
lesbar: VRAM dumpen, die Leisten-Formen (128x4 / 3x153 / 8x130) darin suchen und
die echte Sampling-Stelle ablesen.

## 6. ✅ GELOEST — aus dem laufenden Original gemessen (2026-08-31)

Der Nutzer hat die Item-Box im Emulator geoeffnet. Aus dem Speicherzustand wurde
der Bildspeicher gelesen (Tag `GPU-VRAM`) und der **fertig gezeichnete Schirm**
vermessen. Referenzbild: `referenz/re2_itembox_original.png`.

**Was der Schirm zeigt** (bestaetigt die Rekonstruktion aus dem Code):
Liste mit **fuenf Zeilen**, Namen links, Symbole rechts, **Auswahl in der Mitte**,
Scrollbalken mit Pfeilen rechts, Inventar-Feld rechts daneben, Namensfeld unten.

**Gemessene Geometrie** (Bildschirmkoordinaten; senkrechter Schnitt x=60,
waagerechte Schnitte y=15/60/80/90):

| Bereich | Lage | Farbe |
|---|---|---|
| obere Rahmenleiste | y 14..17 | Grauverlauf 56 -> 88 |
| Kopfband ("ITEM BOX") | y 20..39 | Verlauf 32 -> 96 |
| Listenfeld | y 40..140, x 13..203 | **(24,24,40)** durchgehend |
| Zeilen | y 41/61/81/101/121 | **Raster 20 px** |
| **Auswahlbalken** | **y 80 und y 100, x 13..203 (190 breit)** | **(48,32,8) / Kern (128,32,32)** |
| Fussband | y 141..160 | Verlauf |
| untere Rahmenleiste | y 163..166 | Grauverlauf |
| linker Pfosten | x 7..9 | (48,48,48) |
| Zeilen-Symbol | ab x 155 | — |
| Scrollbalken-Schiene | x 207..212 | (40,40,40) |
| Text normal / ausgewaehlt | — | (104,104,96) / (168,168,152) |

⇒ Der **Zwei-Linien-Auswahlbalken** ueber die volle Listenbreite ist damit
unabhaengig bestaetigt (er stand so schon im Zeichner-Dossier), ebenso das
20-px-Raster und die feste Auswahl auf der mittleren Zeile.
⇒ RE2 hat **keine** Streifen je Zeile, sondern einen **durchgehenden** Listengrund.

**Uebernommen in den Port:** Rahmenleisten, Baender, Pfosten, Listengrund,
Auswahlbalken und Scrollbalken-Schiene mit genau diesen Farben und Hoehen; die
Breite ist auf unsere linke Bildschirmhaelfte gestaucht (145 statt 190 px), weil
rechts das Inventar-Gitter steht. Hoehen 1:1.

**Der Ausleseweg fuer kuenftige Fragen** (funktioniert, dokumentiert):
1. `[Hotkeys] SaveSelectedSaveState = SDL-0/LeftShoulder` in DuckStations
   settings.ini — damit laesst sich mitten im Spiel ein Zustand sichern, ohne den
   Emulator zu schliessen (der Screenshot-Hotkey feuerte unzuverlaessig).
2. Zustand per virtuellem Pad ausloesen, `.sav` wegkopieren.
3. `scratchpad/vram.py <sav> <png> [x y w h]` — dekomprimiert (zstd), sucht den
   Tag `GPU-VRAM`, schneidet aus, schreibt PNG. Der sichtbare Schirm liegt bei
   (0,0) 320x240 bzw. (0,256).
