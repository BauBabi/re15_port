# B — „Die Tueren sind auf beiden Seiten eingezeichnet statt nur einmalig"

Nutzer-Report 2 vom 2026-09-01. Vollstaendiger Zensus aller Durchgaenge des Spiels,
Messung der Doppelungs-Mechanismen, Gegenprobe am Original, Regelvorschlag mit Zahlen.

---

## 0. Messstand, Werkzeuge, Reproduzierbarkeit

**⛔ Der Arbeitsbaum hat sich WAEHREND der Messung geaendert.** `re15_map_zones.h`
wurde um 12:19:39 von einem parallelen Lauf neu erzeugt (134 → 128 Marken; zwischendurch
war eine 120-Marken-Fassung im Baum). Ohne Einfrieren misst man zwei Staende gegeneinander,
deshalb ist alles hier gegen einen **Schnappschuss** gerechnet:

| Datei | sha256 | Stand |
|---|---|---|
| `analysis/karte_0901/snapshot/re15_map_zones.h` | `95aee223c53c8c598ad7e13fb5d7e7b40200aa69a458a3a75c0b54b71badb5d5` | == Arbeitsbaum 2026-09-01 12:19:39 |
| `analysis/karte_0901/snapshot/gen_map_zones.py` | `bda9028a4d0b3d6863c472df843d00c9a56b5b71f8bafb5f9f930691c4b5f7dc` | == Arbeitsbaum, ungestaged |

Skripte (alle gegen den Schnappschuss, `analysis/karte_0901/`):
`sonde2.py` (Lader), `validate.py` (Gegenprobe), `zensus.py`, `regeln.py`,
`ueberlapp.py`, `spiegel.py`, `vorschlag.py`, `bilanz.py`.

### 0.1 Gegenprobe des Nachbaus — Pflicht vor jeder Zahl

Jede Zahl unten stammt aus einem **Nachbau** der Generator-Pipeline (Durchgang 1–3).
Der Nachbau ist verifiziert, nicht angenommen — `validate.py`:

```
verschmolzene Paare: 18   Paare auf verschiedenen Blaettern (continue): 1
Marken nachgebaut: 128   im Schnappschuss-Header: 128
IDENTISCH: True
  davon Tueren: 102  Treppen: 26
  vom seen-Dedup verworfen: 38
```

Die 128 nachgebauten Tupel `(page, rect, mx, my, kind, zid, zid2)` sind **elementweise
identisch** mit `s_map_marks[]`. Damit ist jede Simulation unten eine Aussage ueber den
echten Generator, nicht ueber ein Modell davon.

### 0.2 Datenquellen und ihre Belege

| Groesse | Herkunft |
|---|---|
| Tuer-Datensatz | SCD `Aot_set` 0x3b (kurz) / 0x68 (lang); Trefftest Ecke+Ausdehnung `FUN_80042b64` @0x80042b68–98 |
| Band einer Tuer | `Door_aot_set` pc[4] = `obj[0x82]`; Interaktions-Gate `FUN_8002bd44` @0x8002bf38 |
| Karten-Rechtecke | Anzahl @0x80076840 + Zeiger @0x80076844, 12 B/Eintrag |
| Kachel-uv | Byte +8 / +10 desselben 12-B-Eintrags |
| Kachel-Grafik | `DATA/MAP0x.PIX`, id-Tabelle @0x80074c4c, 256×256 4bpp |
| Marker-Projektion | `FUN_800473f8` @0x8004741c–0x80047528, Skalenzeile @0x800768b0 (8 B/Raum) |
| Port-Tuerstempel | `re15_port/engine/src/re15_inv_screen.c`, `static const signed char SYM[13][2]` |
| Marken-Sichtbarkeit | `re15_port/engine/src/re15_map_zones.c:437` (`re15_map_mark_get`), `zid2` @Zeile 452 |
| gemalte Symbole | `analysis/kartensymbole/symbolkatalog.csv`, Typ `TUER`/`TUER2` |

---

## 1. Zensus aller Durchgangs-Paare

```
Basis-Raeume in re15_room_list.h ........ 120
  davon mit RDT ......................... 103
  davon mit Karten-Zone im Header ....... 72
Tuer-Datensaetze (Aot_set 0x3b/0x68, ohne 0x0-Warps, ohne sce==0) ... 302
   ERZEUGT MARKE                      134
   faellt aus: raum_ohne_zone         110
   faellt aus: kachel_malt_schon       31
   faellt aus: punkt_ausserhalb_aller_zonen  27
```

Ein **Durchgang** = ein Paar reziproker Datensaetze (`A.dest == B.room` und
`B.dest == A.room`), greedy nach der Summe der beiden Ziel-/Ort-Abstaende gebunden
(genau die Regel des Generators, `gen_map_zones.py:931–987`):

```
Durchgangs-PAARE ..................................................... 132
   davon in der Generator-Toleranz (<=4000 Welteinheiten je Richtung) . 131
   Toleranz gerissen ................................................. 1
Ungepaarte Datensaetze ................................................ 38
      Zielraum fuehrt keinen Rueckweg-Datensatz             22
      Rueckweg vorhanden, aber greedy schon vergeben        13
      Zielraum hat kein RDT / nicht in der Raumliste         2
      SELBST-Tuer (dest == eigener Raum), ohne Partner       1
```

**Wo liegen die beiden Enden?**

```
   beide Enden auf einem Karten-Rechteck ............ 38
      davon DASSELBE Blatt .......................... 36
      davon VERSCHIEDENE Blaetter ................... 2
   nur ein Ende ueberhaupt gezeichnet ............... 73
   kein Ende gezeichnet ............................. 21
```

**Was macht Durchgang 2 daraus?**

```
   VERSCHMOLZEN (eine Marke, zid2 gesetzt) .......... 18
   Paar erzeugt 2 Port-Marken ....................... 2
   Paar erzeugt 1 Port-Marke ........................ 96
   Paar erzeugt 0 Port-Marken ....................... 34
   Summe Port-Marken aus Paaren ..................... 100
   Port-Marken aus ungepaarten Datensaetzen ......... 16
   zid2-Eintraege im Header (Gegenprobe) ............ 18   <-- stimmt mit den 18 ueberein
```

**Ergebnis Teil 1:** Die Verschmelzung funktioniert — sie greift bei 18 Paaren, und der
Header traegt exakt 18 `zid2`-Eintraege. Nur **2** Paare erzeugen ueberhaupt noch zwei
Port-Marken. Die Doppelung, die der Nutzer sieht, ist damit **nicht** die fehlende
Verschmelzung zweier Port-Marken. Siehe Teil 5.

---

## 2. Warum wurde ein Paar nicht verschmolzen?

```
   ein Ende faellt in Durchgang 1 aus: raum_ohne_zone              53
   ein Ende faellt in Durchgang 1 aus: kachel_malt_schon           15
   beide Enden ausgefallen (raum_ohne_zone / raum_ohne_zone)       12
   ein Ende faellt in Durchgang 1 aus: punkt_ausserhalb_aller_zonen 10
   beide Enden ausgefallen (raum_ohne_zone / punkt_ausserhalb)      5
   beide Enden ausgefallen (raum_ohne_zone / kachel_malt_schon)     5
   beide Enden ausgefallen (kachel_malt_schon / punkt_ausserhalb)   3
   beide Enden ausgefallen (kachel_malt_schon / kachel_malt_schon)  3
   beide Enden ausgefallen (punkt_ausserhalb / raum_ohne_zone)      3
   beide Enden ausgefallen (kachel_malt_schon / raum_ohne_zone)     2
   beide leben, aber VERSCHIEDENE BLAETTER (continue)               2
   beide Enden ausgefallen (punkt_ausserhalb / punkt_ausserhalb)    1
```

Das „continue mit dem Kommentar verschiedene Blaetter" (`gen_map_zones.py:962`) ist mit
**2 Faellen** praktisch bedeutungslos. Der eigentliche Grund, warum nicht verschmolzen
wird, ist **die Reihenfolge**: der Kachel-Filter laeuft in **Durchgang 1**
(`gen_map_zones.py:921`, `continue`), die Paarbildung erst in **Durchgang 2** (Zeile 931).
Ein Ende, das die Kachel schon malt, ist zum Zeitpunkt der Paarung **nicht mehr da** —
das andere Ende bleibt als einzelne, freie Marke stehen. **15 Paare** landen so.

---

## 3. Gegenthese geprueft: liegen die beiden Enden ueberhaupt an derselben Stelle?

Pixel-Abstand der beiden projizierten Tuerpositionen, nur Paare mit beiden Enden auf
DEMSELBEN Blatt (n = 36):

```
   Median  16.2 px   Mittel  27.3 px   Min  0.0   Max 187.9
   p10 =   1.0 px
   p25 =   2.2 px
   p50 =  14.0 px
   p75 =  31.1 px
   p90 =  49.4 px
   <=2 px: 8    <=5 px: 11    <=10 px: 15    >20 px: 15
```

Die zehn schlimmsten:

```
   2000 z0 S6r0  ( 90,131)  <->  2010 z0 S6r10 (277,149)    187.9 px
   3090 z0 S7r9  (220,189)  <->  30B0 z0 S7r8  ( 65,168)    156.4 px
   1060 z0 S3r9  (118,138)  <->  10C0 z0 S3r8  (180, 80)     84.9 px
   1030 z0 S2r4  (145,134)  <->  1070 z1 S2r0  (197,136)     52.0 px
   3090 z0 S7r9  (244,155)  <->  30A0 z0 S7r3  (204,126)     49.4 px
   2000 z0 S6r0  ( 95,103)  <->  2050 z0 S6r2  (130, 71)     47.4 px
   1120 z0 S4r5  (120,149)  <->  1130 z0 S4r4  (152,125)     40.0 px
   1210 z0 S1r3  (212,126)  <->  1220 z0 S1r4  (201, 95)     32.9 px
   3090 z0 S7r9  (215,137)  <->  30C0 z0 S7r10 (247,133)     32.2 px
   4050 z0 S8r8  (177,164)  <->  4050 z0 S8r8  (200,143)     31.1 px
```

**Die Gegenthese trifft zu — teilweise.** Die Verteilung ist zweigipflig: 11 der 36 Paare
liegen ≤ 5 px auseinander (das ist „dieselbe Stelle"), 15 liegen > 20 px auseinander (das
sind **zwei Symbole an zwei Stellen**). Dass der Generator bei so weit auseinander liegenden
Enden **nicht mittelt**, sondern das Ende mit dem kleineren Abstand zum Nachbar-Rechteck
nimmt (`gen_map_zones.py:966–981`), ist damit belegt richtig — der Mittelwert laege bei
187,9 px Abstand ausserhalb beider Rechtecke.

**Aber:** die grossen Abstaende sind nicht die Doppelung, die der Nutzer meldet. Bei ihnen
verschmilzt der Generator ohnehin auf EINE Marke. Der Nutzer sieht zwei Zeichnungen an
**derselben** Stelle — und die kommen aus einer anderen Quelle (Teil 5).

---

## 4. Was macht das ORIGINAL? Malt es einen Durchgang einmal oder zweimal?

Das ist die entscheidende Frage: malt das Original selbst zwei Symbole, waere der
Nutzerwunsch eine **Abweichung vom Original**.

### 4.1 Bestand

```
Symbole (TUER + TUER2) im Katalog: 75  auf 60 Rechtecken
Symbole je Rechteck: 48x 1 Symbol, 9x 2 Symbole, 3x 3 Symbole
```

### 4.2 Die Signatur „zweimal gemalt" — und sie kommt NIE vor

Ein Durchgang, den der Kuenstler in **beiden** Raeumen malt, muss eine harte,
tolerenzarme Signatur haben: zwei Stempel **auf derselben Wandlinie**, mit
**gegenueberliegender Wandseite** (die Nische zeigt jeweils in den eigenen Raum). Auf
einer senkrechten Wand heisst das: fast gleiche y-Koordinate, wenige Pixel Versatz in x.

```
Signatur: gegenueberliegende Wand UND dieselbe Wandlinie
  (senkrechte Wand: |dy|<=2 und |dx|<=10 ; waagerechte: |dx|<=2 und |dy|<=10)
  Treffer: 0 von 2775 Symbolpaaren
```

**Null.** Kein einziges Mal im ganzen Spiel.

Die schwaechere Gegenprobe (nur Mittelpunktabstand, ohne Wandseite) bestaetigt es:

```
     d<= 4 px:  0 Symbolpaare auf einer Seite, davon gegenueberliegende Wand: 0
     d<= 6 px:  0 Symbolpaare auf einer Seite, davon gegenueberliegende Wand: 0
     d<= 8 px:  1 Symbolpaare auf einer Seite, davon gegenueberliegende Wand: 0
     d<=10 px:  1 Symbolpaare auf einer Seite, davon gegenueberliegende Wand: 0
     d<=14 px:  5 Symbolpaare auf einer Seite, davon gegenueberliegende Wand: 1
     d<=20 px: 22 Symbolpaare auf einer Seite, davon gegenueberliegende Wand: 10
```

Der naechste Symbol-Nachbar auf einer Seite liegt bei 6,3 px, und der ist nicht gespiegelt
(S7 r6 (220,126) Wand S gegen r9 (218,132) Wand W). Der einzige gespiegelte Fall unter 14 px
(S1 r3 (190,74) W gegen r4 (198,82) E) hat dx = 8 **und** dy = 8, sitzt also nicht auf
einer gemeinsamen Wandlinie.

### 4.3 Je Durchgang gezaehlt

```
Fuer jeden Durchgang mit beiden Enden auf einem Rechteck (n = 38):
     genau ein Ende trifft ein Symbol        15
     zwei Enden -> DASSELBE Symbol           11
     kein Ende trifft ein Symbol              9
     zwei Enden -> zwei Symbole               3
```

**11 Durchgaenge treffen mit BEIDEN Enden dasselbe Symbol** — genau das erwartet man,
wenn ein Durchgang einmal gemalt ist und beide Raum-Datensaetze darauf projizieren.
Die 3 „zwei Symbole" sind keine Doppelmalung, ihre Symbole liegen 19,2 / 31,1 / 46,7 px
auseinander:

```
       1210 S1r3 (212, 92)->Sym r6 (215, 91) | 1220 S1r4 (201, 86)->Sym r4 (198, 82) | 19.2 px
       1210 S1r3 (212,126)->Sym r8 (215,125) | 1220 S1r4 (201, 95)->Sym r5 (198, 99) | 31.1 px
       3090 S7r9 (244,155)->Sym r9 (242,158) | 30A0 S7r3 (204,126)->Sym r3 (208,126) | 46.7 px
```

Das sind zwei **verschiedene** Tueren, denen die Greedy-Paarung bzw. die Projektion
denselben Durchgang zuordnet (ROOM1210 fuehrt fuenf Datensaetze mit Ziel ROOM1220,
ROOM1220 fuenf mit Ziel ROOM1210 — nachgezaehlt im RDT).

### 4.4 Gegenprobe ohne jede Projektion

```
Tuer-Datensaetze in gezeichneten Zonen: 165
gemalte Symbole auf deren Rechtecken:    58
-> 0.35 Symbole je Tuer-Datensatz
```

Bei „zweimal gemalt" muessten es ≥ 2 sein. Das Original malt nicht einmal jede Tuer
**einmal** — das Kartensystem des Prototyps ist unfertig (deckt sich mit
`reai-v2-karte-massstabstabelle`: nur 33/72 Raeume haben eine echte Skalenzeile).

### 4.5 Antwort auf Frage 4

> **Das Original malt einen Durchgang NIE zweimal.** 0 von 2775 Symbolpaaren tragen die
> Signatur. Der Nutzerwunsch „eine Tuer = ein Symbol" ist **keine Abweichung vom Original**,
> sondern die Wiederherstellung des Originalverhaltens.

Wo das Original ueberhaupt eine Tuer malt, malt es sie **einmal**, in **einem** der beiden
Raum-Rechtecke — und beide Raum-Datensaetze projizieren auf genau dieses eine Symbol
(11 Durchgaenge nachgemessen).

---

## 5. Die tatsaechliche Doppelung: Port-Marke gegen GEMALTES Symbol

Hier liegt der Fehler, nicht bei der Paarung.

### 5.1 Der Mechanismus

`kachel_zeigt_tuer(pg, r, mx, my)` (`gen_map_zones.py:127`) prueft in Zeile 139
ausschliesslich `GLYPHEN[(pg, r)]` — die Symbole des **eigenen** Rechtecks. Die Rechtecke
einer Seite **ueberlappen sich auf dem Schirm** (der Kommentar der Funktion sagt das
selbst: „Seite 4: Rect 4 ueberlappt 1, 2, 3, 5 und 6"). Ein Durchgang zwischen A und B
wird vom Kuenstler in **einem** der beiden Rechtecke gemalt. Steht die Port-Marke im
**anderen** Rechteck, sieht der Filter das Symbol nicht — und der Port malt daneben
noch einmal.

Gemessen, IST-Stand, 134 lebende Tuer-Datensaetze vor Verschmelzung/Dedup:

```
   tol=0 px:   9 Marken auf einem gemalten Symbol  (eigenes Rect 0, NACHBAR-Rect 9)
   tol=2 px:  38 Marken auf einem gemalten Symbol  (eigenes Rect 0, NACHBAR-Rect 38)
   tol=4 px:  40 Marken auf einem gemalten Symbol  (eigenes Rect 0, NACHBAR-Rect 40)
   tol=6 px:  46 Marken auf einem gemalten Symbol  (eigenes Rect 0, NACHBAR-Rect 46)
   tol=8 px:  55 Marken auf einem gemalten Symbol  (eigenes Rect 3, NACHBAR-Rect 52)
```

**„eigenes Rect: 0"** ueber alle Toleranzen bis 6 px — der Filter arbeitet fehlerfrei,
**auf seinem Rechteck**. Und **40 von 134** Marken sitzen ≤ 4 px vor einem Symbol des
**Nachbar**-Rechtecks, wo er nicht hinsieht.

### 5.2 Der Nutzer meint es woertlich: „auf beiden Seiten"

Die Nische zeigt in den eigenen Raum. Wird derselbe Durchgang zweimal gezeichnet, stehen
zwei Nischen **beidseits derselben Wandlinie**, mit gegenueberliegender Wandseite. Genau
diese Signatur, die im Original **null**mal vorkommt (4.2), gemessen zwischen Port-Marke
und gemaltem Symbol:

```
Signatur (gegenueberliegende Wand UND dieselbe Wandlinie):
  Treffer: 17 von 102 Tuermarken
```

Mit der lockereren Fassung (naechstes Symbol ≤ 8 px, Wandseite gegenueberliegend):

```
   d<=8 px: SPIEGELWAND                25
   d<=8 px: quer                        6
   d<=8 px: GLEICHE WAND                2
   d 9..16 px                          25
   d > 16 px                           43
```

**25 von 102 Tuermarken (24,5 %)** stehen einem gemalten Symbol auf der Spiegelwand
gegenueber. Die dichtesten Faelle:

```
     S8  Marke r7 (142,164) Wand W  |  Symbol r4 (141,163) 4x4 Wand E   |  1.4 px
     S4  Marke r2 (145, 85) Wand E  |  Symbol r4 (147, 84) 5x5 Wand W   |  2.2 px   (1150 <-> 1130)
     S4  Marke r4 (152,125) Wand W  |  Symbol r5 (150,126) 5x5 Wand E   |  2.2 px   (1130 <-> 1120)
     S8  Marke r8 (177,164) Wand W  |  Symbol r11(175,163) 4x7 Wand E   |  2.2 px
     S11 Marke r0 (162,134) Wand W  |  Symbol r1 (160,133) 4x7 Wand E   |  2.2 px
     S11 Marke r3 (158,158) Wand W  |  Symbol r0 (156,157) 4x4 Wand E   |  2.2 px
     S7  Marke r1 (167,115) Wand E  |  Symbol r6 (170,115) 5x5 Wand W   |  3.0 px   (3010 <-> 3060)
     S8  Marke r1 (156, 64) Wand E  |  Symbol r0 (159, 64) 4x4 Wand W   |  3.0 px
     S1  Marke r3 (212, 92) Wand E  |  Symbol r6 (215, 91) 5x5 Wand W   |  3.2 px
     S4  Marke r4 (160,150) Wand E  |  Symbol r6 (163,151) 5x9 Wand W   |  3.2 px   (1130 <-> 1140)
     ... (vollstaendig in _spiegel_out.txt)
```

Dass das optisch **nicht** unterscheidbar ist, ist ebenfalls belegt: der Port zeichnet je
Marke 13 Pixel in **genau dem Wandton des Originals** (`re15_inv_screen.c`,
`static const signed char SYM[13][2]`, Farbe = CLUT-Index 4 = 0x5ad6, mit dem
Zustands-Ton moduliert). Der Stempel ist eine Kopie der gemalten Nische.

### 5.3 Pixelbild — ROOM1130 ↔ ROOM1140 (Seite 4)

Kachel + Header-Marken, `'#'` = Palettenindex 4 (gemalte Linie/Tuerblatt),
`'M'` = Port-Tuermarke, `'T'` = Port-Treppenmarke:

```
     11111111111111111111111111111
     55555555556666666666777777777
     01234567890123456789012345678
 147 ..#.......######.............
 148 ..#.......#....#.............
 149 ..#.......#....#.............
 150 ..#.......M...#..............
 151 ..#.......####...............
 152 ..#.......#...#..............
 153 ..#.......#....#.............
 154 ..#.......#....#.............
 155 ..#.......######.............
```

Das Original malt hier **eine** Doppeltuer (5×9-Stempel, x 160..165 / y 147..155, Rect 6 =
ROOM1140). Die Port-Marke `M` bei (160,150) gehoert zu **ROOM1130s** Datensatz desselben
Durchgangs — sie sitzt auf der Westseite derselben Wandlinie. ROOM1140s Datensatz wurde
korrekt vom Filter geschluckt (Symbol liegt auf **seinem** Rect 6). ROOM1130s Datensatz
nicht, weil das Symbol nicht auf **Rect 4** liegt.

### 5.4 Pixelbild — ROOM1130 ↔ ROOM1150 und ROOM1130 → ROOM1170

```
     111111111111111111111111111111111111111
     334444444444555555555566666666667777777
     890123456789012345678901234567890123456
  80 ################################
  81 ......#........................#
  82 ......######...................#
  83 ......#....#...................#
  84 ......#....#...................#
  85 ......#M..#....................#
  86 ......####.....................#
  ...
  89 ......#.......################M#
  90 ......#.......#..........#...#.#
  91 ......#.......#..........#...#.#
  92 ......#.......#..........#...#.#
  93 ......#.......#...........#..#.#
  94 ......#.......#............###.#
```

Oben: gemaltes 5×5-Symbol bei x 144..149 / y 82..86 (Rect 4 = ROOM1130) — die Port-Marke
`M` von **ROOM1150** liegt bei (145,85) **mitten drin**.
Unten: gemaltes Symbol bei x 163..167 / y 89..94 (Rect 3) — die Port-Marke von ROOM1130
(Ziel ROOM1170) steht bei (168,89), 4,2 px daneben auf der Spiegelwand.

### 5.5 Pixelbild — ein VERSCHMOLZENES Paar hilft auch nicht

ROOM1130 ↔ ROOM1120, im Header `{ 4, 4, 152, 125, 3, 19, 18 }` — `zid2 = 18`, also
verschmolzen:

```
     111111111111111111111111111
     444444445555555555666666666
     234567890123456789012345678
 124 ......######......#.......#
 125 ......#...M#......#.......#
 126 ......#....#......#.......#
 127 .......#...#......#.......#
 128 ........####......#........
```

Der Kuenstler hat den Durchgang bereits als 5×5-Nische bei x 148..152 / y 124..128
(Rect 5 = ROOM1120) gemalt. Die verschmolzene Port-Marke sitzt bei (152,125)
**innerhalb** dieser Nische. **Die Verschmelzung loest das Problem nicht**, weil das
Problem nicht zwei Port-Marken sind, sondern Kachel + Port-Marke.

### 5.6 Schlussbilanz: was sieht der Spieler je Durchgang?

Gezaehlt je Blatt: getroffene gemalte Symbole (nach Identitaet dedupliziert) plus
Port-Marken aus `s_map_marks[]`.

```
   1 Zeichnung(en) auf dem vollsten Blatt      67
   2 Zeichnung(en) auf dem vollsten Blatt      40
   3 Zeichnung(en) auf dem vollsten Blatt       4
   nichts gezeichnet                           21
```

**44 von 111 gezeichneten Durchgaengen (40 %) zeigen ≥ 2 Tuerzeichnungen auf einem Blatt.**
In 38 der 44 Faelle ist das Muster exakt **1 gemaltes Symbol + 1 Port-Marke**.

Je Kartenseite (gemessen, nicht geschaetzt):

```
Seite  gezeichnete Durchgaenge  davon >=2 Zeichnungen
   0         13                   4     Police Station
   1          8                   6     Police Station   75 %
   2         11                   6     Police Station   55 %
   3          6                   0     Police Station
   4          8                   6     Police Station   75 %  <-- 3F, wo der Nutzer laeuft
   5          1                   0     Police Station
   6         15                   3
   7         14                   5
   8         13                   6
   9          9                   3
  10          2                   1
  11          3                   2
  12          8                   2
 ges        111                  44
```

Police Station (Seiten 0–5): 47 Durchgaenge, davon **22 mit ≥ 2 Zeichnungen**.
Auf den Blaettern 1, 2 und 4 — dem Teil, den der Nutzer bespielt hat — sind es
**18 von 27 (67 %)**. Das ist die „so ziemlich alle Faelle" des Reports.

Die Liste (Auszug, vollstaendig in `_bilanz_out.txt`):

```
   Raum A  ->  Raum B          Blatt  gemalte Symbole  Port-Marken
   11B0 <-> 11C0                S0         1              1
   1210 <-> 1220                S1         2              1
   1030 <-> 1040                S2         1              1
   1120 <-> 1130                S4         1              1
   1130 <-> 1140                S4         1              1
   1130 <-> 1150                S4         1              1
   1130 <-> 1170                S4         1              1
   1140 <-> 1170                S4         1              1
   3000 <-> 3010                S7         1              1
   3010 <-> 3060                S7         1              1
   4050 <-> 4050                S8         1              2
   5040 <-> 5060                S9         1              1
```

Zum Vergleich das Original allein:

```
   0 gemaltes Symbol / Durchgang:  57 Durchgaenge
   1 gemaltes Symbol / Durchgang:  51 Durchgaenge
   2 gemaltes Symbol / Durchgang:   3 Durchgaenge   (siehe 4.3 - keine Doppelmalung)
```

---

## 6. Vorschlag mit Zahlen

Ausgangsmenge: 165 Tuer-Datensaetze mit Rechteck, 74 davon zu 37 Paaren gebunden.
Simuliert wird **dieselbe** Pipeline, nur die Regel getauscht; Treppen unveraendert.
Regel (a) reproduziert den Header exakt (102 Tuermarken) — die Simulation ist geeicht.

```
   (a) HEUTE: Kachel/eigenes Rect, DANN Paarung  [= Header]   102 Tuermarken
   (b) nur Paarung vorziehen                                   89 Tuermarken
   (c) Paarung vorziehen + Kachel-Test eig.+Partner-Rect       86 Tuermarken
   (d) Paarung vorziehen + SPIEGELWAND-Test (8 px)             67 Tuermarken
   (e) Paarung vorziehen + Partner-Rect + SPIEGELWAND          67 Tuermarken
   (f) Paarung vorziehen + Kachel-Test ganze Seite (4 px)      64 Tuermarken
```

Restdoppelung nach jeder Regel — **die unabhaengige Gegengroesse**: Zahl der Marken, die
noch ≤ 8 px vor einem Symbol der **Spiegelwand** stehen (die Wandseite geht in keine der
Regeln (a)–(c) ein, ist also fuer diese nicht optimiert):

```
   (a) heute      27 Restfaelle          Marke-gegen-Marke <=8 px:  16 Paare
   (b)            20 Restfaelle                                     16 Paare
   (c)            18 Restfaelle                                     16 Paare
   (d)             0 Restfaelle                                     13 Paare
   (e)             0 Restfaelle                                     13 Paare
   (f)             1 Restfall
```

### 6.1 Empfehlung: Regel (e)

Zwei Aenderungen in `re15_port/tools/gen_map_zones.py`:

**(1) Paarung VOR den Kachel-Filter ziehen.** Der Block „DURCHGANG 2" (Zeile 931) muss
vor die Kachel-Pruefung in Zeile 921. Ein Durchgang wird als **Einheit** behandelt: faellt
ein Ende durch den Filter, faellt der ganze Durchgang, statt das andere Ende freizugeben.
Der Kachel-Test laeuft dann ueber **eigenes + Partner-Rechteck**.

**(2) SPIEGELWAND-Test ergaenzen.** Eine Tuermarke faellt weg, wenn auf derselben
Kartenseite ein gemaltes Symbol in ≤ 8 px liegt, dessen Wandseite die
**gegenueberliegende** ist (`symbolkatalog.csv`, Spalte `wand`; Port-Wandseite = `kind`
0=N/1=O/2=S/3=W). Begruendung: das ist die Signatur „derselbe Durchgang, vom Nachbarraum
aus gemalt" — und sie kommt im Original **0 von 2775 Mal** vor, ist also fuer sich allein
schon der Beweis, dass die Marke ueberzaehlig ist.

**Wirkung: 102 → 67 Tuermarken, 35 Marken fallen weg** (34 %). Restdoppelung gegen
gemalte Symbole: **0**. Neue Marken, die (e) gegenueber heute **zusaetzlich zeigt**:
**keine** — die Regel kann also keine Tuer neu erfinden.

Die 35 wegfallenden Marken vollstaendig (17 mit Spiegelsymbol-Beleg, 8 durch die
Paar-Behandlung, Rest ueber das Partner-Rechteck):

```
   S0  r0  (150,124) S   Paar/Partner-Rect      S4  r4  (168, 89) S   Spiegel r3(165,92) N   4.2 px
   S0  r1  (143, 76) N   Spiegel r0(139,73)  S  S4  r5  (120,149) W   Paar/Partner-Rect
   S0  r3  (168,100) W   Paar/Partner-Rect      S4  r6  (166,110) N   Spiegel r3(164,107) S  3.6 px
   S0  r5  (129,124) N   Spiegel r0(127,121) S  S6  r0  ( 90,131) W   Spiegel r1(88,131) E   2.0 px
   S1  r3  (201, 84) W   Spiegel r4(198,82)  E  S6  r3  (107,144) W   Spiegel r0(105,141) E  3.6 px
   S1  r3  (201,101) W   Spiegel r5(198,99)  E  S6  r4  (118,111) W   Paar/Partner-Rect
   S1  r3  (212, 92) E   Spiegel r6(215,91)  W  S7  r0  (128,117) E   Spiegel r1(131,114) W  4.2 px
   S1  r3  (212,108) E   Spiegel r7(215,108) W  S7  r1  (167,115) E   Spiegel r6(170,115) W  3.0 px
   S1  r3  (212,126) E   Spiegel r8(215,125) W  S7  r8  ( 65,168) W   Paar/Partner-Rect
   S2  r0  (197,136) W   Paar/Partner-Rect      S7  r10 (247,133) E   Paar/Partner-Rect
   S2  r4  (119,134) W   Spiegel r2(115,130) E  S8  r1  (156, 64) E   Spiegel r0(159,64) W   3.0 px
   S2  r4  (186,153) E   Spiegel r0(190,149) W  S8  r7  (142,164) W   Spiegel r4(141,163) E  1.4 px
   S4  r0  (136,141) E   Paar/Partner-Rect      S8  r8  (177,164) W   Spiegel r11(175,163) E 2.2 px
   S4  r2  (145, 85) E   Spiegel r4(147,84)  W  S9  r1  (168, 90) W   Spiegel r0(165,87) E   4.2 px
   S4  r4  (160,150) E   Spiegel r6(163,151) W  S9  r6  ( 95,141) W   Spiegel r4(91,137) E   5.7 px
                                                S9  r14 (138,179) W   Spiegel r10(135,177) E 3.6 px
                                                S10 r0  (126,114) S   Spiegel r1(124,117) N  3.6 px
                                                S11 r0  (162,134) W   Spiegel r1(160,133) E  2.2 px
                                                S11 r3  (158,158) W   Spiegel r0(156,157) E  2.2 px
                                                S12 r5  (194,126) W   Spiegel r0(191,133) E  7.6 px
```

### 6.2 Warum NICHT Regel (f) (blanker Seiten-Test)

(f) bringt 64 Marken, also nur 3 weniger als (e), verwirft aber **ohne** das
Wandseiten-Merkmal. Genau das ist der Fehler, den `kachel_zeigt_tuer` in seinem
Kommentar dokumentiert: bis v0.3.70 meldete ein seitenweiter Pruefer mit ±7 px an allen
vier ROOM1130-Tueren „malt die Kachel schon" — der Nutzer sah fehlende Tueren
(„die Tuer Richtung Ausgang 3F fehlt in der Zeichnung"). (f) verwirft z. B.
S1 r3 (212,108) allein wegen eines 3 px entfernten Symbols in ROOM1240s Rechteck, ohne
zu pruefen, ob das ueberhaupt dieselbe Wand ist. Die Wandseite ist die Wache dagegen.

### 6.3 Was (e) NICHT loest — ehrlich benannt

1. **13 Marke-gegen-Marke-Paare ≤ 8 px bleiben.** Sie sind fast alle ROOM4050 (Seite 8
   Rect 8): der Raum fuehrt **14** Tuer-Datensaetze, davon 12 mit `dest == 4050` — ein
   Selbst-Warp-Labyrinth, dessen Marken auf ein einziges Rechteck gedraengt werden. Das
   ist ein Massstabsproblem, kein Doppelungsproblem.
2. **ROOM1210/1220 (Seite 1) ist upstream verdaechtig.** ROOM1210 fuehrt 5 Datensaetze
   mit Ziel ROOM1220, ROOM1220 5 mit Ziel ROOM1210 (im RDT nachgezaehlt), aber die
   Zonentabelle gibt ROOM1220 nur EIN Rechteck (S1 r4), waehrend der Symbolkatalog die
   Rechtecke 4..8 als eigene Raeume fuehrt. Fuenf der 35 Streichungen liegen dort. Wenn
   die Zuordnung dort falsch ist, ist die Streichung dort ebenfalls fragwuerdig —
   das gehoert in Dossier A (Zonen/Rechteck-Zuordnung), nicht hierher.
3. **57 Durchgaenge malt das Original gar nicht** und 21 werden ueberhaupt nicht
   gezeichnet (kein Ende hat eine Zone). Das ist der unfertige Prototyp-Zustand der
   Karte, keine Regression des Ports.

---

## 7. Zusammenfassung der Zahlen

| Frage | Antwort |
|---|---|
| Tuer-Datensaetze im Spiel | 302 |
| Durchgangs-Paare | 132 |
| beide Enden auf demselben Blatt | 36 |
| verschmolzen (Durchgang 2) | 18 — Header traegt 18 `zid2` |
| Paare, die 2 Port-Marken erzeugen | 2 |
| Paare, die wegen des Kachel-Filters nicht verschmolzen wurden | 15 |
| Paare, die wegen „verschiedene Blaetter" nicht verschmolzen wurden | 2 |
| Pixel-Abstand der Enden (gleiches Blatt) | Median 16,2 px; 11 ≤ 5 px, 15 > 20 px |
| gemalte Tuersymbole im Original | 75 |
| Symbolpaare mit der Signatur „zweimal gemalt" | **0 von 2775** |
| Durchgaenge, deren beide Enden DASSELBE Symbol treffen | 11 |
| Port-Marken auf einem Symbol des Nachbar-Rechtecks (≤4 px) | 40 von 134 |
| Port-Marken mit Spiegelwand-Signatur gegen ein Symbol | 17 von 102 (streng) / 25 (≤8 px) |
| Durchgaenge mit ≥2 sichtbaren Zeichnungen auf einem Blatt | **44 von 111** |
| Wirkung Regel (e) | 102 → 67 Tuermarken, Restdoppelung 0, keine neue Marke |
