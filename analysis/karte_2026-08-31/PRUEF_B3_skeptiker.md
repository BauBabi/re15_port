# GEGENPRUEFUNG B3 (Rolle: Skeptiker) — "Seite 4 Rect 3 gehoert NICHT ROOM1140 Zone 1"

Datum 2026-08-31. Auftrag: die Behauptung WIDERLEGEN. Ergebnis: **nicht widerlegt** —
unabhaengig bestaetigt, und zwar auf staerkeren Belegen, als der Bericht benutzt.
Alle Zahlen unten habe ich selbst gemessen (eigener RDT-Parser, eigener EXE-Leser,
eigene C-Sonden gegen `libre15_engine.a`). Keine Zahl aus B uebernommen.

Sonden im Scratchpad: `comp1140.py`, `scd1140.py`, `exe.py`, `scale.py`,
`p1140.c`, `p1140b.c`, `p1140c.c`, `p1140d.c`.

---

## 1. WAS ICH SELBST GEMESSEN HABE

### 1a. ROOM1140.RDT Kollisionsblock (Datei-Offset 0x0570)

`ROOM1140.RDT` +0x20 -> collisionStart = **0x0570**. Kopf: ceil=(-9648,-13554),
counts = **(19,19,19,19,19)**, also 95 Eintraege a 12 B — aber nur **19 eindeutige**;
die 5 Quadranten-Gruppen sind byte-identische Kopien (deckt sich mit dem Motor-Kommentar
`re15_collision.c:178` "the 5 quadrant groups are byte-identical duplicates").

**ALLE 19 Zellen tragen `floor = 0x03`, also Band 0.** Gegenprobe mit dem Motor selbst:
`re15_collision_band_range()` liefert **0..0**. In ROOM1140 gibt es nur EINE Etage —
eine Zone dieses Raums KANN gar keine andere Etage sein.
=> Bs Teilaussage "beide Zellgruppen sind ausschliesslich Band 0" ist richtig,
aber zu schwach formuliert: der **ganze Raum** ist Band 0.

### 1b. Die Zerlegung (union-find, GAP=1500) — selbst nachgerechnet

3 Komponenten; die dritte (0,5 %) faellt unter `MIN_FRAC=0.03` weg.

| Komponente | Bbox | Anteil | eindeutige Zellen |
|---|---|---|---|
| Zone 0 | x[-10600,16700] z[-25600,1850] | 90,2 % | 14 (Aussenwaende + Einbauten) |
| **Zone 1** | **x[-5250,7450] z[-17450,-11350]** | 9,3 % | **2** |
| verworfen | x[10500,11500] z[-16250,-12500] | 0,5 % | 3 |

Zone 1 besteht aus **genau zwei** Zellen:

* `#6  x=-4650 z=-17450 w=12100 d=6100  typ=1 u0=0xff floor=0x03` (Rechteck)
* `#15 x=-5250 z=-14400 w=1000  d=1000  typ=3 u0=0xff floor=0x03` (Kreis)

Containment selbst geprueft: -10600 <= -5250, 7450 <= 16700, -25600 <= -17450,
-11350 <= 1850 — **Zone 1 liegt vollstaendig in Zone 0**. (B stimmt.)

### 1c. Tuergraph + Treppen aus ROOM1140s eigenem SCD — Zone 1 hat NICHTS

Eigener SCD-Walker ueber mainScd/subScd:

```
mainScd sub00 @0x0002  DOOR rect=(-9100,-19750,1000,4000) Mitte(-8600,-17750) -> ROOM1130
mainScd sub00 @0x002a  DOOR rect=(-8250,  -750,2000,1000) Mitte(-7250,  -250) -> ROOM1170
mainScd sub00 @0x0062  DOOR rect=(-8250,  -750,2000,1000) Mitte(-7250,  -250) -> ROOM1170  (Dublette)
```

* **0 Tueren** in Zone 1 (alle drei Mitten liegen in Zone 0).
* **0 Aot_set Typ 12/13** (Treppen-Bandwechsel) im ganzen Raum.
* 10 `Sce_em_set`-Spawns, **alle y=0 -> Band 0**, keiner im Zone-1-Block.

Die echten ANKUNFTS-Punkte in 1140 stehen in den Tuersaetzen der NACHBARN
(nicht in 1140s eigenen — dort steht die Zielposition im Zielraum):

* `ROOM1170.RDT mainScd @0x0144` -> ROOM1140 bei **(-7250, 0, -1200)**
* `ROOM1130.RDT mainScd @0x0002` -> ROOM1140 bei **(-7600, 0, -17600)**

Beide sind kollisionsfrei und liefern **Zone 0 -> Rect (4,6)** (Motor-Sonde `p1140c`).
Es gibt keinen Weg, der in "Zone 1" hineinfuehrt.

### 1d. Zone 1 ist ein MOEBELKLOTZ — Motor-Sonde, nicht Modell

Mit `re15_collision_set_band(0)` + `re15_collision_constrain` (byte-true FUN_8003b0a4):

```
Rasterpunkte in Zone-1-Bbox unveraendert (= frei):   90 von 7936
Lauf von (0,-20000) 200x nach Sueden  -> stoppt (0,-17918)   = -17450 - 468
Lauf von (0, -9500) 200x nach Norden  -> stoppt (0,-10882)   = -11350 + 468
```

468 = Spielerradius PR 450 + Skin 0x12. Der Spieler wird auf **beiden** Seiten exakt an
Zelle #6 abgewiesen: Zone 1 ist der zentrale Klotz (Tisch/Podest) des Briefing-Rooms,
kein Ort.

Vollsweep ueber alle erreichbaren Punkte (100er-Raster, 18189 freie Punkte):

```
Zone 0 -> Rect (4,6): 13068
Zone 1 -> Rect (4,3):  5121   Bereich x[-6700,8900] z[-18900,-9900]
ohne Zone           :     0
```

x[-6700,8900] z[-18900,-9900] ist **exakt Zone-1-Bbox +- ZONE_SLACK(1500)**: der
"zweite Bereich" ist der **Schlauch RINGS UM den Klotz**, luecklos zusammenhaengend mit
Zone 0 (ASCII-Karte in der `p1140c`-Ausgabe). 28 % der begehbaren Flaeche des Saals
schaltet die Karte auf ein voellig anderes Rechteck um. Zwei der zehn Zombie-Spawns
((2850,-10300) und (-6650,-10300)) stehen bereits darin.

=> "Sind die zwei Komponenten im Spiel wirklich verbunden (begehbar)?" — **Ja, sie sind
sogar dasselbe begehbare Stueck Boden.** Getrennt sind nur zwei WANDKLOTZ-Gruppen; der
Generator verschmilzt Hindernisse, nicht Boeden.

### 1e. Der Split haengt an einer unbelegten Konstante

`GAP = 1500` in `gen_map_zones.py:36` traegt nirgends eine `@0x...`-Adresse. Die
massgebliche Luecke ist 2150 (Suedkante #6 z=-11350 -> Nordkante #9 z=-9200):

| GAP | ROOM1140 | ROOM1170 |
|---|---|---|
| 1500 | 2 Zonen | 2 Zonen (echt) |
| 2150 | 2 Zonen | 2 Zonen |
| **2200** | **1 Zone** | **2 Zonen (bleibt)** |

700 Einheiten mehr an einer geratenen Konstante loeschen "ROOM1140 Zone 1" ersatzlos,
**ohne** ROOM1170s echte Zwei-Bereichs-Struktur anzutasten.

### 1f. EXE-Tabellen selbst nachgelesen

PSX.EXE-Kopf: t_addr=0x80010000, t_size=0xaf000 (= Auslieferungsstand, nicht die
gepatchte 0xb1000-Fassung).

* Paartabelle @0x80076840 (Datei 0x67040): Seite 4 count=7 ptr=0x80076468 (Datei 0x66c68),
  Seite 5 count=2 ptr=0x800764bc (Datei 0x66cbc).
* **Seite 4 r3 = (152,89) 48x24 uv(192,16)**, r6 = (160,110) 56x56 uv(32,32);
  Seite 5 r0 = (140,80) 48x24 uv(192,16), r1 = (148,101) 48x56 uv(0,32). (B stimmt exakt.)
* Marker-Skalentabelle @0x800768b0: **ROOM1140 = idx 20 @0x80076950 (Datei 0x67150) =
  ox0 oy0 sx1 sy1 = STUB.** Das Original hat fuer ROOM1140 ueberhaupt keine kalibrierte
  Markerabbildung — jede 1140-Rechteck-Zuordnung ist ohnehin reine Port-Ergaenzung.

---

## 2. WO DER BERICHT SCHWAECHER IST, ALS ER KLINGT (aendert das Ergebnis nicht)

1. **"sx/sy ~ 2280..2300 ist der feste Weltmassstab"** (B §5) ist ueberzogen. Selbst
   gemessen ueber die wirklich kalibrierten Zeilen: sx = **1776** (ROOM11A0 @0x80076984)
   bis **3168** (ROOM1200 @0x800769b4) = Spanne **1,78x**; sy = 1954..2320.
   Da ROOM1140s eigene Zeile ein STUB ist, ist "27,7 x 13,3 px" eine **angenommene**,
   keine gemessene Groesse. Ehrlich waere: Delta **15..31 px** je nach geliehenem sx
   (bei sx=3168 -> 38,4 x 18,4 gegen 48x24). Die Schlussfolgerung haelt bei jedem
   Massstab der Tabelle — 12700x6100 wird mit keinem davon zu 48x24 — aber die Zahl
   "31" ist nicht belastbar und traegt den Befund auch nicht: er haengt an Band, Tuer,
   Treppe und Begehbarkeit (Abschnitt 1a-1d), nicht an der Groesse.
2. B §5 listet 13 kalibrierte Zeilen und nennt die uebrigen 25 Stubs. Uebersehen:
   **idx 28 (ROOM11C0) @0x80076990 = ox=174 oy=196 sx=1 sy=1** — halb gefuellt, weder
   Stub noch kalibriert. Kosmetisch.
3. "beide Zellgruppen sind ausschliesslich Band 0" ist richtig, aber der staerkere Satz
   ist "der ganze Raum ist Band 0" (Motor: band_range 0..0).

---

## 3. DER PIN: zementiert eine FALSCHE Zuordnung, schuetzt keine Regression

`re15_port/tests/unit/test_map_re2_system.c`

**Abschnitt (3), 4 Zusicherungen (Z. 99-109)** — misst an **unerreichbaren** Punkten:

| Zeile | Punkt | Kommentar im Test | gemessen |
|---|---|---|---|
| 100 | (0, 0) | "eindeutig im GROSSEN Bereich" | **im Festkoerper** (Zelle #3), constrain -> (0, 2218) |
| 105 | (0, -14400) | "eindeutig im ZWEITEN Bereich" | **im Festkoerper** (Zelle #6), constrain -> (0, -10882) |

Beide Positionen kann der Spieler nie einnehmen. Der Pin haelt Kartenverhalten an
Koordinaten fest, die es im Spiel nicht gibt — und zementiert damit genau die
Zuordnung 1140-Zone-1 -> (4,3), die nach Abschnitt 1 falsch ist.

**Abschnitt (7), Z. 188-190** — die Haelfte, die wirklich schuetzt, ist die ANDERE:

* `"der kleine Bereich liegt auf Seite 5 Rect 0 (NICHT auf Seite 4)"` — **echte Wache**
  gegen die v0.3.69-Regression. Behalten.
* `"Seite 4 Rect 3 gehoert weiter ROOM1140, nicht 1170"` — **leer erfuellbar**:
  `re15_map_rect_state()` (`re15_map_zones.c:150-177`) liefert fuer ein Rect ohne jede
  Zone `RE15_MAP_RECT_UNMAPPED = 0`, und `0 != RE15_MAP_RECT_CURRENT (3)`. Die
  Zusicherung besteht also **auch dann**, wenn man die Zuordnung 1140-Zone-1 -> (4,3)
  ersatzlos loescht. Sie belegt nicht, was ihr Kommentar behauptet.

**Antwort auf die Auftragsfrage:** Der Pin schuetzt in Abschnitt (3) **keine** echte
Regression, sondern zementiert eine falsche Zuordnung an zwei unmoeglichen Positionen.
Nur die Seite-5-Zusicherung aus Abschnitt (7) ist echter Regressionsschutz.

Nebenbedingung, die man beim Entfernen mitdenken muss (die den Fehler aber nicht
rechtfertigt): ein Rect **ohne** Zone wird grau als "unbekannt" gezeichnet
(Memory `reai-v2-zeichenreihenfolge-invers`). Seite 4 hat aber **jetzt schon** zwei
solche Rects — r0 und r4 sind in `re15_map_zones.h` keiner Zone zugeordnet — das
Argument schuetzt r3 also nicht besonders.

---

## 4. SEITENBEFUND (ausserhalb B3, aber gemessen)

Die Kollision des Ports **versperrt den Suedkorridor von ROOM1140 vollstaendig**:
Zelle `#13 x=-9000 z=-7200 w=3700 d=500` sperrt mit PR+Skin x[-9468,-4832], der
Korridor ist aber nur x[-8132,-6218] breit.

```
von der 1170-Tuer (-7250,-1200) 400x nach Norden -> (-7250,-6232)
vom Saal        (-7250,-9000) 400x nach Sueden  -> (-7250,-7668)
```

Der Spieler kommt im Port zu Fuss nicht vom Briefing-Room zur ROOM1170-Tuer. Entweder
ein echter Port-Bug oder ein Hinweis, dass "Band == Spielerband => solide" fuer diesen
Zellentyp nicht stimmt. **Auf B3 hat das keinen Einfluss**: Band-, Tuer- und Treppen-
Befund sind reine RDT-Daten, und ob #6 solide oder begehbar ist, macht Zone 1 in beiden
Lesarten zu einem willkuerlichen Teilrechteck mitten im selben Raum auf derselben Etage.

---

## 5. FAZIT

**refuted = false.** B3 haelt. Belegt durch: Band 0..0 im ganzen Raum
(ROOM1140.RDT @0x0570, alle 19 Zellen floor=0x03), 0 Tueren und 0 Treppen-AOTs in
Zone 1 (SCD @0x0002/0x002a/0x0062), beide echten Ankuenfte ((-7250,-1200) aus
ROOM1170 @0x0144, (-7600,-17600) aus ROOM1130 @0x0002) in Zone 0, und der Motor-Sweep,
der Zone 1 als reinen ZONE_SLACK-Ring um einen Festkoerper ausweist (5121 von 18189
begehbaren Punkten, luecklos mit Zone 0 verbunden). Zusaetzlich: GAP >= 2200 loescht die
Zone ersatzlos, ohne ROOM1170 anzutasten. Die "31 px"-Zahl des Berichts ist die
schwaechste seiner Begruendungen und sollte als 15..31 px gelesen werden.
