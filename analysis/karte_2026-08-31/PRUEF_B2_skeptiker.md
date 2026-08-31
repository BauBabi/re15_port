# GEGENPRUEFUNG B2 (Rolle: Skeptiker) — 2026-08-31

Behauptung: Seite-4-r3 uv(192,16) 48x24 == Seite-5-r0 uv(192,16) 48x24 (identische Zeichnung,
22 Pixel Unterschied 4->1 an zwei "Tuermarken"), Folgerung: ROOM1170s 2. Bereich ist
ZUSAETZLICH auf dem 3F-Blatt gezeichnet, weil er dort geografisch hingehoert (Tueren zu
ROOM1130/ROOM1140, beide Seite 4).

**ERGEBNIS: NICHT widerlegt. Unabhaengig bestaetigt — und ueber den Bericht hinaus BEWIESEN
(die vom Bericht als "stuetzend, nicht beweisend" eingestufte Tuermarken-Lage ist beweisbar).**
Alles unten selbst gerechnet/gelesen; keine Zahl aus dem Bericht uebernommen.

## 1. Adressen/Tabellen selbst gelesen

* PSX.EXE Header @0x18 = 0x80010000, t_size 0x000af000 -> off(a)=a-0x80010000+0x800. OK.
* Paartabelle @0x80076840 (Datei 0x67040): Seite 4 -> count **7**, ptr **0x80076468** (Datei 0x66c68);
  Seite 5 -> count **2**, ptr **0x800764bc** (Datei 0x66cbc). Wie behauptet.
* Seite 4 r3 = **(152,89) 48x24 uv(192,16)**; Seite 5 r0 = **(140,80) 48x24 uv(192,16)**. Wie behauptet.
* Skalentabelle @0x800768b0: Zeile 21 (ROOM1150) @0x80076958 = ox111 oy130 sx2296 sy2312;
  Zeile 23 (ROOM1170) @0x80076968 = ox100 oy206 sx2280 sy2268. Wie behauptet.

## 2. Kacheln selbst dekodiert (DATA/MAP05.PIX, DATA/MAP06.PIX; je 32768 B = 256x256 4bpp, 128 B/Zeile)

Fenster uv(192,16) 48x24, Pixel fuer Pixel:

* **22 von 1152 Pixeln** verschieden — **alle 22** sind MAP05=4 / MAP06=1. BESTAETIGT.
* Nichtnull-Maske: **0** Unterschiede -> Silhouetten-IoU = **1,000**. BESTAETIGT.
* Zwei Cluster: **uv x[203,207] y[17,21]** (11 px) und **uv x[202,206] y[32,36]** (11 px).
  -> KLEINE KORREKTUR: der Bericht schreibt fuer den oberen Cluster `uv(202..207,17..21)`;
  gemessen ist die linke Spalte **203**, nicht 202. Substanzlos, aber falsch abgeschrieben.
* Kontrolle gegen "MAP06 ist bloss eine Kopie von MAP05": nur **30,7 %** der in einer der beiden
  Dateien nicht-leeren Bytes sind gleich; dieselbe Atlas-Ecke uv(0,32) traegt in beiden Dateien
  voellig VERSCHIEDENE Bilder. Der uv-Atlas ist also nur ein gemeinsames Raster — die
  Uebereinstimmung bei uv(192,16) ist damit KEIN Kopier-Artefakt, sondern eine bewusste Doppelung.

## 3. Seitenzuordnung selbst disassembliert

Switch @0x8004b568 (`lh v1,DAT_800b0fe2`, `sltiu v0,v1,0x26`), Sprungtabelle **@0x8001103c** (38 Eintraege).
Fall-Through-Bloecke (Blockabstand 0x14 = 5 Instr., `j`-Bloecke 0x10 = 4 Instr.):

* [19]=0x8004b710 (ROOM1130, idx 0x13) und [20]=0x8004b724 (ROOM1140, idx 0x14) fallen durch
  bis [22]=**0x8004b74c** `ori v0,zero,0x4` -> 0x8004b888 -> `sb v0,DAT_800b260e` **@0x8004b88c**.
  => **ROOM1130 und ROOM1140 liegen beide auf Seite 4.** BESTAETIGT.
* [23]=**0x8004b75c** `ori v0,zero,0x5` -> ROOM1170 = Seite 5. BESTAETIGT.
* Vollstaendige Bandbreite: idx 0..11 -> S2, 12..17 -> S3, 18..22 -> S4, 23 -> S5, 24..29 -> S0, 30..37 -> S1.

## 4. Tuergraph selbst aus den RDTs gelesen (Door_aot_set 0x3B, 32-B-Records)

ROOM1170.RDT hat **7** 0x3B-Records (Bericht listet nur 6 — der degenerierte Slot 3 @0x12cc,
rect (0,0)+(0,0) -> Selbstziel (2300,14365), fehlt in der Bericht-Tabelle):

* **@0x12f8** `3b 04 02 31 ... 00 13` -> Ziel Stage 0 / Raum **0x13 = ROOM1130**;
  Rect (-22370,-18380)+(2280,910) -> Mitte **(-21230,-17925)**; Ankunft (2650,15550). band=0.
* **@0x1338** `3b 05 02 31 ... 00 14` -> Raum **0x14 = ROOM1140**;
  Rect (-22740,-27610)+(2280,1210) -> Mitte **(-21600,-27005)**; Ankunft (-7250,-1200). band=0.

Gegenrichtung (selbst geprueft):

* **ROOM1130.RDT @0x08ee** `3b 03 02 31 ... 00 17` -> ROOM1170 bei **(-21225,-18870)**.
* **ROOM1140.RDT @0x0a7a/@0x0ab2** `3b 01 .. 00 17` -> ROOM1170 bei **(-21937,-25713)**.

Beide Ankunftspunkte liegen in comp1 (2. Bereich) x[-28900,-8784] z[-28841,-17070], nicht im Hof.
=> Tuergraph und Gegenrichtung BESTAETIGT.
Nebenbefund geprueft: ROOM1150 hat genau EINE Tuer (@0xd5e -> Raum 0x13 = ROOM1130). Stimmt.
ROOM1160 hat zwei Records mit IDENTISCHEM Rect/Ziel-Punkt (@0x7c6 -> 0x18, @0x7ea -> 0x23). Stimmt.

## 5. Kollision selbst geparst (ROOM1170.RDT SCA @0x0a80, 5x30 Zellen, 12 B/Zelle)

Baender-Bboxen exakt wie im Bericht (Band 3 x[-9632,11410] z[-9128,15640]; Band 4 x[-21946,15658]
z[-28734,17746]; Band 2 x[-28900,-22300] z[-28160,-22885]; Band 0 x[-28622,-17876] z[-28841,-17070]).
Union-Find mit GAP 1500: **80 / 70** Zellen; comp1 = x[-28900,-8784] z[-28841,-17070]. Alles BESTAETIGT.

Markerformel FUN_800473f8 selbst nachdisassembliert (@0x8004741c..0x80047528) und implementiert:
`mx = ((((X+32000)*10*sx)>>20)+5)/10 + ox`, `my = (-((((Z+32000)*10*sy)>>20)+5))/10 + oy`.
Damit reproduziert: Hof-Boden -> x[149,194] y[103,157] vs Seite-5-r1 x[148,196] y[101,157] (<=2 px);
comp1 -> x[107,150] y[174,199] vs Seite-5-r0 x[140,188] y[80,104]. Beide Bericht-Zahlen exakt getroffen.

## 6. UNABHAENGIGER BEWEIS, den der Bericht NICHT gefuehrt hat

Eine Kette, die die Kachel-Zeichnung NICHT benutzt — nur (a) die Original-Skalenzeile 21
@0x80076958, (b) die RDT-Tuer-Records, (c) die Rect-Tabelle:

1. ROOM1150 durch die ORIGINALFORMEL -> Bildschirm x[118,150] y[81,121]; Seite-4-r2 = x[120,152]
   y[80,120]. Abweichung <=2 px -> Anker gueltig, Seite 4 ist EIN zusammenhaengender Etagen-Rahmen.
2. Tuer 1150<->1130 (ROOM1150 @0xd5e / ROOM1130 @0x8ce) verschiebt den Rahmen nach ROOM1130:
   vorhergesagt x[139,5;171,5] y[77,9;161,2], Groesse **32,0 x 83,3** -> Seite-4-r4 (144,80) **32x80**.
3. Tuer 1130<->1170(2. Bereich) (@0x8ee / @0x12f8) verschiebt weiter: der 2. Bereich wird
   vorhergesagt bei **x[147,5;191,4] y[85,1;110,8], Groesse 43,9 x 25,7** -> Seite-4-r3 (152,89) 48x24.
   Relativversatz r3-r4 vorhergesagt (+8,0 / +7,2) px, tatsaechlich (+8 / +9) px.
4. Dieselbe Kette sagt die beiden TUERPOSITIONEN auf dem Bildschirm voraus:
   -> ROOM1130 bei **(164,3 / 87,0)**, -> ROOM1140 bei **(163,5 / 106,8)**.
5. Die 22 abweichenden Pixel liegen (r3 sitzt bei (152,89)) auf dem Bildschirm bei
   **x[163,167] y[90,94]** (oben) und **x[162,166] y[105,109]** (unten).
   => waagerecht **0,7 px bzw. 0,5 px** neben der vorhergesagten Tuer.
6. Komposition aller 7 Seite-4-Kacheln an ihren Bildschirmpositionen (5776 gezeichnete Pixel):
   Seite 4 ist ein LUECKENLOSES Puzzle, die Raeume teilen sich 1-2 px dicke Waende.
   r3 teilt sich mit **r4** die Wand **x[152,169] y[89,110]** und mit **r6** die Wand **x[160,168] y=110**.
   Der untere Fleck sitzt direkt AUF dem 9-px-Stueck r3/r6, der obere direkt unter dem oberen
   Lauf von r3/r4.

=> Die beiden Flecke sind exakt die Durchgaenge zu **r4 (=ROOM1130)** und **r6 (=ROOM1140)**.
Damit ist die Bericht-Einschraenkung "waagerechte Lage weicht um ~5-6 px ab -> stuetzend,
nicht beweisend" ein Artefakt der falschen Bezugsecke (comp1-Welt-Bbox statt Rect-Ecke 152,89).

## 7. Was ich VERSUCHT habe zu widerlegen (und was dabei herauskam)

* "MAP06 ist eine Kopie von MAP05, das Tile ist ein Ueberbleibsel" -> WIDERLEGT (Punkt 2:
  30,7 % Nichtnull-Uebereinstimmung, uv(0,32) traegt verschiedene Bilder).
* "r3 ist in Wahrheit ROOM1160" -> WIDERLEGT: ROOM1160s SCA-Grundriss (37x29 px) hat eine
  voellig andere Topologie (linke Vollhoehe-Spalte + Block in der MITTE + Bodenbalken),
  nicht die L-Form (linker Block oben, Arm unten). ROOM1160 bleibt fuer r1/r0 uebrig.
* "Die Original-Projektion widerlegt r0" (Seite-5-Versatz +33/-94 px) -> das trifft nur die
  SEITE-5-Platzierung; auf Seite 4 stimmt die Geografie (Punkt 6) auf ~1 px. Der Versatz
  auf Seite 5 ist eine unfertige Blatt-Platzierung, kein Gegenbeweis gegen die Identitaet.
* Zaehlfehler/gerundete Zahlen: alle nachgerechneten Zahlen des Berichts (22 px, IoU 1,000,
  43,9x25,7, x[107,150] y[174,199], x[149,194] y[103,157], 32,0x83,3) sind reproduzierbar.

## 8. Kleine Fehler im Bericht (aendern die Aussage nicht)

1. §6f: oberer Diff-Cluster ist **uv(203..207,17..21)**, nicht 202..207.
2. §6b listet 6 von **7** ROOM1170-Tuer-Records; der degenerierte Slot 3 @0x12cc fehlt.
3. §6f: "waagerechte Lage weicht um ~5-6 px ab" — falsche Bezugsecke, siehe Punkt 6.
4. §7 mischt Bboxen: dort "Gruppe 0", anderswo alle 5 Gruppen (bei ROOM1150 sind das
   14328x18324 statt der im Bericht genannten 14292x17694 Welteinheiten).
