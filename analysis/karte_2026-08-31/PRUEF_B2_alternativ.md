# GEGENPRUEFUNG B2 — ALTERNATIV-ERKLAERER

Geprueft: **[B2]** "Seite 4 Rect 3 (152,89) 48x24 uv(192,16) zeigt DIESELBE Zeichnung wie
Seite 5 Rect 0 ... Folgerung: der Kuenstler hat ROOM1170s zweiten Bereich ZUSAETZLICH auf dem
3F-Blatt gezeichnet, weil er dort geografisch hingehoert."

Rolle: nicht nachrechnen, sondern eine **bessere Erklaerung** suchen. Alle Zahlen unten sind
selbst gemessen (EXE-Byte-Offsets, PIX-Pixel, RDT-Bytes). Sonden im Scratchpad
`.../scratchpad/altB/`: `lib.py, census.py, census2.py, blob.py, geo.py, doorfit.py,
assign.py, propagate.py, shape.py`.

**ERGEBNIS: Die Identifikation haelt.** Ich habe sechs Konkurrenz-Erklaerungen mit demselben
Mass gemessen; alle verlieren um Faktor 2-4. **Widerlegt ist nur die RICHTUNG der Folgerung**
("zusaetzlich auf dem 3F-Blatt") — die Belege zeigen das Gegenteil.

---

## 0. Der Pixel-Befund selbst — unabhaengig reproduziert

`MAP05.PIX` vs `MAP06.PIX` bei uv(192,16), 48x24 (beide Dateien 32768 B = 256x256 4bpp,
128 B/Zeile, kein Header):
**22 von 1152 Pixeln verschieden, ALLE in Richtung p4=4 (grau) -> p5=1 (gruen)**,
Silhouette in allen 1152 Pixeln gleich. Exakt wie behauptet.

Die 22 Pixel sind **zwei geschlossene Nischen-Symbole** im linken Block:
Kachel x11..15 / y1..5 (haengt an der OBEREN Wand) und x10..14 / y16..20 (an der UNTEREN).
Beide sind Trapez-Umrisse in Index 4, vertikal zueinander gespiegelt.

---

## 1. KONKURRENT A — "Der Atlas kopiert Blaetter" (die staerkste Alternative)

Gemessen, nicht vermutet:

* **Seite 3 ist eine woertliche Kopie der Rect-Tabelle von Seite 2.** Seite 2 @0x8007636c
  (11 Eintraege), Seite 3 @0x800763f0 (10 Eintraege): die 10 gemeinsamen Eintraege stimmen in
  **allen sechs Feldern** (x,y,w,h,u,v) ueberein; Seite 3 ist Seite 2 ohne r10.
* **Sieben Rect-Paare verschiedener Seiten teilen uv UND Groesse. Fuenf davon sind
  0 Pixel verschieden:**

  | Paar | uv | Groesse | Diff |
  |---|---|---|---|
  | p2 r9 = p3 r9 = **p4 r0** | (168,40) | 16x16 | **0** / 256 |
  | p2 r10 = **p4 r1** | (168,16) | 24x24 | **0** / 576 |
  | p8 r2 = p9 r13 = p10 r3 | (0,32) | 16x16 | **0** / 256 |
  | p8 r7 = p9 r14 = p11 r3 | (16,32) | 24x24 | **0** / 576 |
  | p10 r2 = p11 r2 | (136,16) | 64x40 | **0** / 2560 |
  | p10 r1 vs p11 r1 (LAB B3/B4) | (40,32) | 48x56 | 28 / 2688, IoU 1,000 |
  | **p4 r3 vs p5 r0** | (192,16) | 48x24 | 22 / 1152, IoU 1,000 |

* Das **naechste Gegenstueck** zu B2 ist p10 r1 / p11 r1 (LABORATORY **B3** vs **B4**):
  gleiche uv, gleiche Groesse, IoU 1,000, 28 von 2688 Pixeln — und diese 28 sind **zwei kleine
  Symbole**, eines nur auf B3 (x1..7,y1..4), eines nur auf B4 (x37..41,y36..42).
  Dieselbe Signatur zwischen zwei anderen Blaettern.

**Was das der Behauptung nimmt:** "gleiche Zeichnung auf zwei Blaettern" ist im Atlas ein
**wiederkehrendes Produktions-Artefakt** (7 Faelle), kein Alleinstellungs-Beleg fuer eine
geografische Aussage. Aus der Identitaet ALLEIN folgt der Schluss nicht.

**Was das der Behauptung NICHT nimmt:** jede reine Leiche im Atlas ist **exakt 0 Pixel**
verschieden (5 von 5 Faellen). B2 ist es nicht — die 22 Pixel sind zwei bewusste Symbol-Edits.
Konkurrent A erklaert die Identitaet, aber nicht die zwei Marken.

---

## 2. KONKURRENT B..G — andere Raeume, mit demselben Mass gemessen

Grundlage: Zonen und Tueren selbst aus den RDTs geparst (SCA-Zellen RDT+0x20, `Door_aot_set`
0x3B; Verschmelzung GAP 1500 wie `gen_map_zones.py`). Massstab 1 px = 2^20/2290 = **457,9**
Welteinheiten (aus den kalibrierten Zeilen @0x800768b0).

### 2a. Tuergraph-Test — 1170 Zone 1 auf JEDES Seite-4-Rechteck gelegt
Fehler = Abstand, in dem die Tuer nach 1130 bzw. 1140 in BEIDEN Raeumen landet
(Basis 1130->r4, 1140z0->r6, 1150->r2):

| Rechteck | 1130 | 1140 | Summe |
|---|---|---|---|
| **r3 (152,89) 48x24** | **2,5** | **7,6** | **10,1 px** |
| r4 (144,80) 32x80 | 12,6 | 30,2 | 42,7 |
| r6 | 29,4 | 41,0 | 70,5 |
| r2 | 36,9 | 35,7 | 72,6 |
| r5 | 47,5 | 48,1 | 95,7 |
| r1 | 55,2 | 46,4 | 101,7 |
| r0 | 60,6 | 48,5 | 109,1 |

Kontrolle: die seiteninternen Kanten 1130-1140 = **4,1 px**, 1130-1150 = **3,3 px**.
r3 liegt also in derselben Guete wie echte, unbestrittene Nachbarschaften — und viermal
besser als der Zweitplatzierte.

### 2b. Vollstaendige Zuordnung, alle 5040 Permutationen
Kosten = Tuerfehler + Groessenfehler (beides px):

```
  99,6  1120->r5 1130->r4 1140z0->r6 1150->r2 1160->r1 1170z1->r3   <- Optimum
 196,5  bestes Ergebnis, in dem 1170z1 NICHT auf r3 liegt
 205,9  bestes Ergebnis mit 1160 -> r3
```
1170z1 liegt in **allen** zwoelf besten Loesungen auf r3.

### 2c. Original-verankerte Ausbreitung (nur Original-Daten)
Anker = die einzige kalibrierte 3F-Zeile, Zeile 21 @0x80076958 (ox=111 oy=130 sx=2296 sy=2312),
Massstab 2290, RDT-Tuerkoordinaten. Kein Port-Wert, keine Annahme "Bbox fuellt Rechteck":

| Raum | vorhergesagte Lage | bestes Rechteck | IoU |
|---|---|---|---|
| ROOM1150 (Anker) | x[118,150] y[80,121] | r2 | 0,852 |
| ROOM1130 | x[140,172] y[78,161] | r4 | 0,728 |
| ROOM1140 z0 | x[151,211] y[109,169] | r6 | 0,737 |
| **ROOM1170 z1** | **x[147,5,191,4] y[85,1,110,8]** | **r3** (152,89,48,24) | **0,607** |
| ROOM1120 | x[147,189] y[91,136] | r4 0,418 / **r3 0,372** | — |

### 2d. Form-Test (SCA-Zellen in die Kachel projiziert)
| Paar | IoU | Zellen in der Zeichnung |
|---|---|---|
| **1170 z1 -> r3** | **0,549** | **78 %** |
| 1150 -> r2 (original-belegt) | 0,429 | 76 % |
| 1140 z0 -> r6 | 0,396 | 58 % |
| 1120 -> r3 (Gegenprobe) | 0,352 | 58 % |
| 1130 -> r4 | 0,264 | 37 % |
| 1170 z1 -> r4 (Gegenprobe) | 0,137 | 35 % |

1170z1->r3 ist der beste Treffer der ganzen Seite — besser als das original-belegte 1150->r2.

### 2e. Die einzelnen Konkurrenten
* **ROOM1160 -> r3:** die einzige Tuerposition des Raums (-300,-16500; zwei Records, dieselbe
  Position, Ziele 1180/1230) projiziert auf Kachel ~(25,9) — dort ist die Kachel **transparent**
  (Index 0), ausserhalb der L-Form. Dazu: eine Tuerposition, aber zwei Marken. Verliert.
* **ROOM1120 -> r3:** einziger ernsthafter Zweitplatzierter (IoU 0,372 gegen 0,607;
  Groessenfehler 28 px gegen 5,9). Verliert auf beiden Massen.
* **ROOM1140 Zone 1:** Bbox liegt vollstaendig in Zone 0, beide Tueren liegen ausserhalb.
  Kein Marken-Kandidat.
* **ROOM1171 (Elza-Variante):** geometrisch identisch zu 1170 (150 Zellen, dieselben zwei
  Zonen), nur eine Tuer mehr. Kein neuer Kandidat.
* **"generische Stock-Kachel":** uv(192,16) wird auch von p2 r3, p3 r3 und p12 r0 benutzt —
  dort aber mit 48-58 % abweichenden Pixeln. Die Atlas-Ecke wird pro Blatt neu gezeichnet;
  nur Seite 4/5 teilen die Zeichnung. Verliert.
* **"p5 r0 ist ein Nachbar des Hofs (10B0/1240)":** die Selbst-Tuer Hof->2. Bereich landet am
  OBEREN Rand von r1 (174,105 bei r1 = y[101,157]), r0 liegt genau darueber (y[80,104]) —
  passt zum 2. Bereich. Die Tuer nach 10B0 landet dagegen UNTEN (179,150). Verliert.

---

## 3. WAS TATSAECHLICH FALSCH IST: die RICHTUNG

Die Behauptung sagt, die 3F-Zeichnung sei die **zusaetzliche**. Die Messungen zeigen das
Gegenteil:

1. **Vollstaendigkeit:** alle 22 Unterschiede laufen 4 -> 1. Die Seite-4-Fassung hat **zwei
   Tueroeffnungen**, die Seite-5-Fassung hat sie **uebermalt**. Detail wird der Primaer-,
   nicht der Kopie-Fassung hinzugefuegt.
2. **Einbindung:** ich habe den 3F-Gesamtplan unabhaengig wiedergefunden — die 7 Rects an ihren
   Bildschirmpositionen zusammengesetzt decken sich bei uv(157,164) zu **8075/8256 = 97,81 %**
   mit dem "unbenutzten" Klecks in MAP05. In diesem Plan ist die L-Form **kein Insel-Bild**:
   ihre linke Wand (Bildschirm x=152) IST die rechte Wand des 1130-Korridors, und die untere
   Wand ihres Arms laeuft durchgehend von Master-x 32 bis 73 in den 1140-Raum hinein.
3. **Ableitung:** die ganze Rect-Tabelle der Seite 5 (2 Eintraege @0x800764bc) benutzt genau
   die beiden uv-Ecken der Seite 4: (192,16) und (0,32).
4. **Funktion:** die Original-Skalenzeile 23 @0x80076968 (ox=100 oy=206 sx=2280 sy=2268) bildet
   den 2. Bereich auf Bildschirm x[107,150] y[174,199] ab — **33/94 px neben** p5 r0
   (x[140,188] y[80,104]). Auf dem Blatt, das das Spiel fuer Raum 23 zeigt, sitzt das Rechteck
   also an der falschen Stelle. So sieht eine mitkopierte Leiche aus.

**Bessere Formulierung:** Der 2. Bereich ist auf dem **3F-Blatt zu Hause** (dorthin fuehren
seine beiden Tueren, dort ist er in den Gesamtplan eingezeichnet); **ZUSAETZLICH** — und ohne
Tuermarken — steht er auf dem 1170-Blatt.
Praktische Folge: **keine.** Fuer Raum 23 schaltet das Original ohnehin immer Seite 5
(@0x8004b75c/68); die Kernaussage des Berichts (kein eigenes Rechteck fuer die untere Etage)
bleibt unberuehrt.

---

## 4. NEBENBEFUNDE, die andere Stellen des Berichts betreffen

* **§7.5/§7.6 (ROOM1160 -> r1 vs r3, r0 ohne Zuordnung):** Seite-4 **r0 und r1 sind
  0-Pixel-Kopien von 1F-Kacheln** (p2 r9 bzw. p2 r10), und zwar als **Gruppe** um denselben
  Betrag verschoben: (109,134)->(127,137) und (119,134)->(136,137), also (+18,+3)/(+17,+3).
  Damit ist "ROOM1160 -> r1" sehr wahrscheinlich falsch; plausibler hat Seite 4 **gar keine**
  Zeichnung fuer ROOM1160 (dessen einzige Tuer fuehrt ohnehin von der Seite herunter, nach
  1180/1230). Nicht anfassen ohne eigene Messung, aber der Bericht sollte das nicht als
  "Patt" stehenlassen.
* **Zwei unvereinbare Massstabs-Konventionen im Karten-Code.** Der Pro-Frame-Rechner
  FUN_800473f8 @0x8004742c-0x8004744c rechnet `((X+32000)*10*sx)>>20` (1 px = 2^20/sx = 458 E).
  Der Aufbauer FUN_80046fd8 rechnet @0x80047010 `addiu a0,a0,0x61a8` (+25000) und @0x80047014
  `div a0,v1` mit v1 = sx aus **derselben** Tabelle @0x800768b4, also `(X+25000)/sx`
  (1 px = 2280 E) — Faktor **4,96**. Beide koennen nicht gleichzeitig gelten; die Karten-
  Tabellen wurden offenbar mitten in der Entwicklung umskaliert. Bericht B benutzt nur die
  erste Variante (was die Hof-Probe empirisch stuetzt), sollte die zweite aber nennen.
* **Kein Laufzeit-Bezug Rechteck<->Raum.** Die Zeichenschleife in FUN_80046fd8 laeuft
  `do {...} while (uVar10 < uVar1)` ueber **alle** `count` Rects, ohne Besucht-Gate. Es gibt im
  Original also keinerlei Code-Zuordnung Rechteck->Raum; die Frage ist rein zeichnerisch und
  nur geometrisch entscheidbar. (Das stuetzt die Methodik des Berichts.)
