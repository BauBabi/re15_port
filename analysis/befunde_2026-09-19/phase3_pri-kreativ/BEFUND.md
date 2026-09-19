# Phase 3: die offenen Punkte der Vordergrund-Masken STAGE1

Nutzer-Auftrag vom 2026-09-19: "gehe die offenen Punkte an, bei den offenen pri findings
sei kreativ, das es gut aussieht."

Grundlage: `pri-masken-audit.md` §5 (Offen), `pri-masken-audit.skeptiker.md` und
`pri-masken-phase2/BEFUND.md` §4/§6. Werkzeuge `re15_port/tools/maske/*`, Abnahme je Cut
wie in Phase 2 (Deckung == Freistellung bitgenau, VORverd == 0, HINTER-Rest, Prüfbild).
Messskripte und Protokolle in `protokoll/`.

## 0. Kurzfassung

| # | Offener Punkt (Phase 2) | Ergebnis |
|---|---|---|
| 1 | ROOM10D0 C1 nicht geschrieben (Marke F423 verlangt Tiefe < 180 in x156..176) | **gelöst** — die Freistellung trägt jetzt zwei Tiefen-Objekte; Spalte 176 hat 172 statt 319, `unit_pri_eingemessen` grün |
| 2 | ROOM1000 C3 nicht geschrieben (1 von 805 VOR-Plätzen verdeckt) | **anders gelöst als vermutet** — die vier Freistellungen gehören zu Cut **2** bei Maßstab **1**; C3 bekommt keine Maske |
| 3 | ROOM10E0 C7 passt nicht ins 256x256-Atlasblatt | **gelöst** — nicht die Fläche war zu groß (78 %), das Regalverfahren zu schwach; MaxRects packt es bei feinerer Stufe |
| 4 | Maßstab-4-Freistellungen treffen nur 85–93 % | **Ursache widerlegt und behoben** — sie verlieren nichts beim Verkleinern; sie lagen am falschen Cut |
| 5 | ROOM11F0 C1 berührt 0 Standplätze | **erledigt** — Artefakt der alten (invertierten) Bodensonde; die Freistellung gehört zu Cut 0 |
| 6 | HINTER-Abdeckung (die Figur verschwindet nicht hinter dem Gegenstand) | **verbessert**: 928 → 713 freie Plätze (1,2 % → 0,9 %) über die 72 gemeinsamen Cuts |
| 7 | Standlinie 10C0 C3 / 1130 C3 (Zelle oder unterste PNG-Zeile?) | **entschieden: die Zelle** — beide laufen über Regel (i), Abnahme sauber, im Spiel nachgesehen |
| 8 | ROOM10D0 07_01: das Nutzer-Original statt meiner Fassung? | **Prämisse widerlegt** — der Unterschied sind keine Löcher zwischen den Stuhlrohren |
| 9 | ROOM10F0 C4/C5, ROOM1100 C1/C2 mit derselben Abnahme prüfen | siehe §7 |

**Stand nach dem Bau:** 77 Cuts geschrieben, 3 Sektionen entfernt, 0 abgelehnt.
`ctest --test-dir re15_port/build_p3 --timeout 120`: **100% tests passed, 0 tests failed
out of 319** (180,7 s).

---

## 1. ROOM10E0 C7 — das Blatt war nicht zu klein, das Packverfahren zu schwach

Phase 2 meldete: "passt nicht in das 256x256-Atlasblatt: 41235 Sollpunkte, mindestens
51097 Atlaspunkte auch bei der gröbsten Stufe. LAUT gemeldet statt vergröbert."

51097 von 65536 sind 78 % — die Fläche passt. Gemessen (`protokoll/mess_pack.py`,
`mess_pack2.py`) über alle Tiefenstufen:

| Stufe | Fläche | Regalverfahren | MaxRects |
|---|---|---|---|
| 6 | 58213 px (89 %) | – | **passt** |
| 8 | 53744 px (82 %) | 14 abgewiesen | passt |
| 12 | 51182 px (78 %) | 9 abgewiesen | passt |
| 16 | 51097 px (78 %) | 6 abgewiesen | passt |
| 48 | 48472 px (74 %) | 5 abgewiesen | passt |

Das Regalverfahren kann die Restbreite einer Zeile nur noch für Kästen gleicher Höhe
nutzen. MaxRects (Best-Short-Side-Fit) führt die freien Restflächen als eigene Rechtecke
weiter und bringt dieselbe Liste unter — bei Stufe **6** statt 16.

Die Packung ist rein werkzeug-intern: die Engine liest je Record ein eigenes
(srcX, srcY) — `@0x80039408 lbu s7,0(a2)` / `@0x80039418 lbu fp,-9(s2)` — und weiß nicht,
wo die Kopie im Blatt liegt. Am Bild ändert sich nur der Verschnitt. **Kein zweites Blatt**:
der Zeichner setzt TPage 0x95 fest (FUN_80039590), alle Masken eines Cuts kommen aus EINER
Seite; ein zweites Blatt wäre nicht mehr im Original-Format ausdrückbar.

`atlas.packen()` nimmt erst das Regalverfahren und nur bei Abweisung MaxRects, Gleichstand
ans Regalverfahren — ein Cut, der bisher passte, bleibt bitgleich.

ROOM10E0 C7 jetzt: 103 Rechtecke, **32 Tiefenstufen** (50..154), Deckung fehlt 0 / zuviel 0,
VOR n=42 verdeckt=0. Bild `ROOM10E0_C7.png`.

**Nebenwirkung, die genau auf Punkt 6 einzahlt:** 16 weitere Cuts erreichen dadurch eine
feinere Tiefenstufe (z.B. ROOM1160 C3 von 48 auf 87 Stufen, ROOM11D0 C3 von 48 auf 83), und
ihr HINTER-Rest fällt mit.

---

## 2. ROOM10D0 C1 — eine Freistellung, zwei Tiefen-Objekte

Die F9-Marke F423 des Nutzers (Welt 3228/-4568, er steht HINTER der Liege) verlangt eine
wirksame Maske über x156..176; wirksam heißt Tiefe < 180 (Kopf-vz 11579 → Bucket 180).

**Gemessen** (`protokoll/mess_10d0c1c.py`), Tiefe je Bildspalte für das Nutzer-Original als
EIN Objekt:

| Spalten | Punkte je Spalte | unterste Zeile | Weltpunkt | Tiefe |
|---|---|---|---|---|
| 126..146 | 1..14 | 102/103 | (6453,-7297) … (4954,-7163) | 225..234 |
| 147..175 | 6..53 | 118..146 | (3522,-354) … (2667,-3674) | 109..172 |
| **176** | **2** | **90** | **(3243,-13512)** | **319** |

Nur die Spalte 176 fiel durch — zwei Punkte an der obersten Ecke, deren Sehstrahl fast
waagerecht läuft und die Bodenebene erst 13512 Einheiten hinter dem Tisch trifft. Der
Restfehler der Kalibrierung ("16 Einheiten") war nicht die Ursache.

**Die Fußlinie ist messbar:** der Weltpunkt springt zwischen 146→147 um 6958 und 175→176 um
9855 Einheiten, während der größte Schritt innerhalb von 147..175 bei 1044 liegt (6,7-fach).
Also ist `"bodenkante": [147, 175]` die gemessene Fußlinie; 176 erbt den Standpunkt von 175
und trägt **172**.

**Und die Platte ist kein stehender Gegenstand.** Die Freistellung enthält eine waagerechte
grüne Tischplatte und ein Metallgestell (Bild `10D0_C1_ansicht.png`). Als EIN Objekt bekommt
die Platte eine *invertierte* Rampe: ihr fernes linkes Ende liest 234, ihr nahes rechtes Ende
319 (`protokoll/mess_10d0c1f.py`). Getrennt wird jetzt am Hintergrundbild über die grüne
Plattenfarbe (g > r+10 und g > b+10), **um 1 Punkt geweitet** — ohne die Weitung fällt der
Antialias-Saum der Platte ans Gestell und schleppt dessen Fußlinie um 6958 Einheiten weg.
Ergebnis: Platte 476 + Gestell 545 = **1021 Punkte = bitgenau die Freistellung des Nutzers**.

Die Platte trägt EINE Tiefenstufe (`"aufrecht": true`, gemessen 201..206). ⛔ Das ist eine
*Entscheidung*, kein Messwert — gemessen ist nur, dass die Spaltenrampe dort nachweislich
verkehrt herum läuft.

Das Gestell bekommt zusätzlich `"zelle": false`: die beste SCA-Zelle trifft die Freistellung
mit IoU **0,06** bei Höhe -75 (ein 75 Einheiten hoher Quader für einen Klapptisch), während
die tragenden Zellen-Objekte in STAGE1 bei IoU 0,6..0,92 liegen (69 gemessene Zuordnungen).

**Ergebnis:** 69 Rechtecke, 57 Tiefenstufen (111..206), Deckung fehlt 0 / zuviel 0,
VOR n=108 verdeckt=0, HINTER n=507 frei=94. `unit_pri_eingemessen` meldet für F423
*"21 Spalten mit Maske auf Körperhöhe, Maske wirkt bis Tiefe 179; 0 Spalten ohne wirksame
Maske"* — und die Gegenbedingung F731 (rechter Rand ≤ 182, gemessen 178) hält weiter.
Bilder `ROOM10D0_C1.png`, `sicht_ROOM10D0_C1_{vor,hinter}.png`.

---

## 3. Der eigentliche Befund: neun Freistellungen lagen am falschen Cut

Die Punkte 2, 4 und 5 haben eine gemeinsame Ursache.

### 3.1 Die alte Lagemessung war eine selbstbestätigende Metrik

`maske_aus_png.platziere` nahm als Güte den BESTWERT der Farb-Übereinstimmung über alle
Verschiebungen. Der wächst zwangsläufig, je kleiner das Objekt wird — gemessen für
`1000/03_01`: s=1 → 54 %, s=2 → 80 %, s=3 → 87 %, s=4 → **89 %**. Die Zahl hat "Maßstab 4"
selbst erzwungen.

⛔ Die Erklärung aus Phase 2 ("verlieren beim BOX-Verkleinern dünne Teile") ist **falsch**.
Gemessen (`protokoll/mess_massstab4.py`): 14061 Alphapunkte / 16 = 879 erwartet, im Bild
sind es **877**. Nichts geht verloren. Auch die Phase (Vorschnitt 0..3 px vor dem
Verkleinern) ist nicht die Ursache — Phase (0,0) ist bereits die beste. Der Rest von 7–15 %
ist ein reiner FARBunterschied im Inneren (mittlerer Abstand 13..18 gegen 1,2..2,9 bei den
1x-Freistellungen), und 77–94 % der Fehltreffer liegen ≥ 2 Punkte von der Silhouettenkante
entfernt (`protokoll/mess_massstab4b.py`).

### 3.2 Das Nullmodell zeigt es sofort

`protokoll/mess_lage_null.py` rechnet die Verteilung der Übereinstimmung über ALLE Lagen.
Eine richtig sitzende Freistellung hat GENAU EINEN Gipfel:

| PNG | Maßstab | best | Gipfelhöhe | Lagen ≥ 95 % des Bestwerts |
|---|---|---|---|---|
| 10E0/07_01 (Gegenprobe) | 1 | 100,0 % | 16,8 σ | **1** von 10878 |
| 11F0/06_02 (Gegenprobe) | 1 | 99,9 % | 14,7 σ | **1** von 10858 |
| 1000/03_01 | 4 | 89,5 % | 2,9 σ | **778** von 59100 |
| 1000/01_01 | 4 | 92,6 % | 2,1 σ | 134 von 57915 |
| 11F0/01 | 4 | 85,2 % | 2,1 σ | **1709** von 58667 |

Ihre Lage war durch die Daten überhaupt nicht bestimmt.

### 3.3 Die Suche über alle Cuts bei Maßstab 1 findet sie eindeutig

`protokoll/mess_lage_suche.py`:

| PNG | eingetragen war | gefunden | best | Lagen ≥ 95 % |
|---|---|---|---|---|
| 1000/01_01 | C1, s=4 | **C0**, x=0 y=47 | 100,0 % | 4 |
| 1000/01_02 | C1, s=4 | **C0**, x=99 y=155 | 100,0 % | **1** |
| 1000/01_03 | C1, s=4 | **C0**, x=152 y=159 | 100,0 % | **1** |
| 1000/01_04 | C1, s=4 | **C0**, x=280 y=112 | 100,0 % | 7 |
| 1000/03_01 | C3, s=4 | **C2**, x=0 y=62 | 100,0 % | 2 |
| 1000/03_02 | C3, s=4 | **C2**, x=147 y=206 | 100,0 % | 4 |
| 1000/03_03 | C3, s=4 | **C2**, x=185 y=204 | 100,0 % | 2 |
| 1000/03_04 | C3, s=4 | **C2**, x=259 y=64 | 100,0 % | 2 |
| 11F0/01 | C1, s=4 | **C0**, x=191 y=88 | 100,0 % | 3 |

**Selbst angesehen** (`neulage_ROOM1000_C0.png`, `neulage_ROOM1000_C2.png`,
`neulage_ROOM11F0_C0.png`): ROOM1000 C0 = die beiden Spindblöcke links und rechts plus die
Bank in der Bildmitte; ROOM1000 C2 = die beiden Spindreihen und die Bank unten; ROOM11F0 C0
= die Schaltschränke rechts. Alles genau die Vordergrund-Möbel, punktgenau auf der Kante.

⛔ **Und genau diese drei Cuts liefen bisher über MEINE eigenen Polygone**, weil "kein
Nutzer-PNG" angenommen wurde (Audit §1.3: "1000 C0/C2, 11F0 C0 — polygon (meins)"). Der
Nutzer hatte sie geliefert; sie lagen nur unter der falschen Nummer. Die Polygone sind raus.

### 3.4 Was mit den drei Cuts geschieht, die dadurch leer werden

ROOM1000 C1, ROOM1000 C3 und ROOM11F0 C1 haben damit keine Freistellung mehr. Ihre
Sektionen waren aus den falsch verorteten PNGs gebaut (ROOM1000 C3 z.B. vier Farbflecken
quer über eine Tür, `ROOM1000_C3_objekte.png` in der alten Messung) — sie sind **entfernt**
statt stehengelassen. Eine falsch verortete Maske ist schlechter als keine.

⛔ **Offen und ehrlich benannt:** ROOM11F0 C1 zeigt dieselben Schaltschränke aus einem
anderen Winkel, dort sind sie ebenfalls Vordergrund. Für diesen Winkel gibt es keine
Freistellung — der Cut bleibt jetzt ohne Maske. Dasselbe gilt für ROOM1000 C3 (die rechte
Kabinenwand des Waschraums).

### 3.5 Riegel gegen Wiederholung

`protokoll/pruef_alle_lagen.py` rechnet das Nullmodell für JEDES STAGE1-PNG-Objekt.
Nach der Korrektur: **131 Objekte, 115 mit exakt 100,0 % Bestwert, alle 131 auf dem Gipfel
(Abstand 0..2 Punkte)**. Die 13 Objekte mit mehreren gleich guten Lagen liegen auf dunklen,
gleichförmigen Flächen (Treppenhaus ROOM10A0, ROOM1160/1180/1220) und sitzen trotzdem
richtig (Abstand 0..2).

---

## 4. ROOM10A0 C2 — die Schranke maß feiner, als der Zeichner sehen kann

Phase 2: "1 von 2 VOR-Standplätzen (Treppenband 3, Welt 22250/22150) voll verdeckt."

**Gemessen** (`protokoll/mess_10a0c2.py`): der Körperkasten dieses Standplatzes läuft über
x57..80. Sein Fuß-vz auf der Bezugsebene des Objekts ist 6544, die Standlinie der Maske in
den berührten Spalten 6316..6607. Die Abweichung ist 9 bis 63 Einheiten. Die alte Toleranz
von EINER Einheit machte daraus drei "VOR"-Spalten (x77..79), von denen zwei überhaupt
keinen Maskenpunkt tragen — ein einziger Punkt in Spalte 77 ergab die Quote 1,00.

So fein kennt der Zeichner die Entfernung nicht:

| Adresse | Instruktion | Bedeutung |
|---|---|---|
| `@0x8002565c` | `sra v1,v1,4` | Figur-OT-Index = `otz>>4` |
| `@0x80066c70` | `addiu t0,zero,341` | ZSF3 = 341 |
| `@0x80066c74` | `ctc2 t0,cr29` | → `otz = (1023*vz)>>12` |
| `@0x8001ce54` | `jal 0x80039590` | Masken sind die LETZTEN Prims; AddPrim hängt vorne ein → im GLEICHEN Bucket liegt die Figur obenauf |

Ein OT-Bucket ist 65536/1023 = 64,0625 Einheiten breit. Innerhalb eines Buckets gibt es kein
"davor" und "dahinter"; es gibt nur die Reihenfolge, und die entscheidet für die Figur. Die
Toleranz ist jetzt BUCKET statt 1 — in `abnahme.py` und in `test_pri_kopfschnitt.c`.

Wirkung über die 78 Cuts (`protokoll/bau_pruef1.log` gegen den Lauf danach):

| | VOR-Plätze | voll verdeckt | teilweise | HINTER-Plätze | frei |
|---|---|---|---|---|---|
| vorher | 7111 | **1** | **2** | 77040 | 1144 |
| nachher | 7030 | **0** | **0** | 76940 | 916 |

ROOM10A0 C2 selbst: VOR n=2 verdeckt=1 → **VOR n=0**, HINTER 589 frei=**0**.
Bild `ROOM10A0_C2.png`.

---

## 5. Punkt 6 — die HINTER-Abdeckung

Gesamtbilanz über die 72 Cuts, die es in beiden Phasen gibt
(`pri-masken-phase2/protokoll/bau2.log` gegen `protokoll/bau_schreiben.log`):

| | Rechtecke | Tiefenstufen | VOR voll verdeckt | HINTER frei |
|---|---|---|---|---|
| Phase 2 | 5462 | 2630 | 1 (+ 2 teilweise) | 928 von 75924 = **1,2 %** |
| Phase 3 | 5802 | 2855 | **0** (0 teilweise) | 713 von 76015 = **0,9 %** |

Die größten Einzelverbesserungen:

| Cut | HINTER frei vorher → nachher |
|---|---|
| ROOM1160 C3 | 57 → 7 |
| ROOM11D0 C3 | 57 → 7 |
| ROOM10A0 C7 | 28 → 3 |
| ROOM11F0 C6 | 21 → 0 |
| ROOM1000 C6 | 22 → 5 |
| ROOM1000 C2 | 19 → 4 (mit den Freistellungen des Nutzers statt meiner Polygone) |
| ROOM1000 C5 | 14 → 2 |
| ROOM11F0 C0 | 10 → 0 |

Zwei Ursachen, beide gemessen: die feinere Tiefenstufe aus §1 (mehr Stufen je Objekt, näher
an den Künstler-Masken mit median 19 Stufen je Cut) und die Bucket-Toleranz aus §4.

⛔ **Nicht gelöst:** ROOM1000 C4 bleibt bei 63 von 153 freien HINTER-Plätzen und ROOM10A0 C5
bei 30 von 259. Dort liegt das Tiefenmodell systematisch zu FERN (Kalibrierung Phase 2:
Medianfehler +11 Buckets mit Statistik MAXIMUM) — das ist der bewusst gewählte Preis für
"nie jemanden verdecken, der davor steht" und steht unverändert offen.

---

## 6. Punkte 7 und 8

**Punkt 7 (Standlinie 10C0 C3 und 1130 C3).** Beide Cuts leiten ihre Tiefe gar nicht aus der
untersten PNG-Zeile ab, sondern aus der SCA-Zelle (Regel i):

* ROOM10C0 C3 "Holzbank": Zelle x-9600..8750 z-8250..-6250, Höhe -800, IoU 0,51, Oberkante
  1,7 px; "Sitzreihen mit Trennwand": Zelle x-4850..1150 z-3300..-1300, Höhe -700, IoU 0,75.
  Abnahme: VOR n=490 verdeckt=0, HINTER n=207 frei=40.
* ROOM1130 C3 "Wandkante mit Lampe": Zelle x-3950..6000 z4200..14700, Säule, 10258 von 10289
  Punkten getroffen. Abnahme: VOR n=39 verdeckt=0, **HINTER n=1506 frei=0**.

Damit ist die Frage entschieden: **die Zelle**. Im Spiel nachgesehen (§8): in ROOM10C0 C3
steht Leon vor der Bank unverdeckt und hinter ihr verschwinden seine Beine.

**Punkt 8 (ROOM10D0 07_01).** Die Frage war, ob mit der neuen Tiefenregel das Nutzer-Original
verwendbar ist, weil "Löcher Bodentiefe bekommen". **Die Prämisse hält nicht.** Gemessen
(`protokoll/mess_10d0c7.py`): der Unterschied zwischen dem Nutzer-Original und meiner Fassung
sind 168 Punkte, die beim Nutzer opak sind — und das ist ein 1 Punkt breiter Saum ENTLANG der
Silhouettenkante (x0..64, y123..239), nicht die Öffnung zwischen den Stuhlrohren. Die große
dreieckige Öffnung im Gestell ist in BEIDEN Fassungen durchsichtig (Bild
`protokoll/../10D0_C7_luecken.png` in `build/p3`). Die 168 Saumpunkte bekämen Objekttiefe
52..61, während der Boden an denselben Bildpunkten 61..185 liegt — sie lägen also 1 bis 124
Buckets vor dem Boden, den sie zeigen. Meine Fassung bleibt eingebaut; eine Trennung per
Farbe ist bei einem 1 Punkt breiten Antialias-Saum nicht messbar.

---

## 7. Punkt 9 — ROOM10F0 C4/C5 und ROOM1100 C1/C2

Beides geprüft, in zwei Varianten (`protokoll/unangetastet.log`,
`protokoll/pruef_unangetastet*.py`):

**(a) Die Sektionen, wie sie im Baum liegen**, gegen die Abnahme der Phase 3:

| Cut | Rechtecke | Stufen | Deckung fehlt / zuviel | VOR verdeckt | HINTER frei |
|---|---|---|---|---|---|
| ROOM10F0 C4 | 105 | 49 (26..174) | 19689 / 0 | 0 von 936 | 959 von 1178 |
| ROOM10F0 C5 | 105 | 41 (44..185) | 12184 / 0 | 0 von 547 | 303 von 979 |
| ROOM1100 C1 | 105 | 70 (45..258) | 0 / 0 | 0 von 170 | **0 von 1515** |
| ROOM1100 C2 | 105 | 55 (48..163) | 0 / 0 | 0 von 188 | **0 von 9308** |

**(b) Neu gerechnet aus denselben Auswahl-Einträgen** mit der Phase-3-Kette: alle vier
bestehen (fehlt 0 / zuviel 0, VOR verdeckt 0), ROOM10F0 C4 mit 98 Rechtecken und 51 Stufen,
C5 mit 104 und 51.

Lesart, ehrlich: die "fehlenden" 19689 bzw. 12184 Punkte sind **kein** nachgewiesener Defekt.
Die Sollfläche in (a) ist die der Phase-3-Quaderrechnung (`quader_auf_band`, Basis = Band),
die alte Sektion wurde mit `geom.quader_tiefe` (Basis fest y=0) gebaut. Beide Silhouetten
stammen von MEINEN Bürostuhl-Quadern, nicht von einer Freistellung — "richtig" ist dort nicht
definiert. Was in beiden Varianten gilt: **VOR verdeckt = 0** (niemand wird verdeckt, der
davor steht), und die HINTER-Abdeckung ist in ROOM10F0 C4/C5 schlecht (959 von 1178 bzw. 303
von 979 frei), weil die vielen einzelnen Stuhl-Quader nur kleine Flecken abdecken.

**Nicht angetastet**, wie in Phase 2: der Nutzer hat ROOM10F0 C4/C5 am 2026-09-09 abgenommen
("Ok, einwandfrei"). Für ROOM1100 C1/C2 gibt es keine Freistellung; sie bestehen die Abnahme
im Bestand.

---

## 8. Sichtprüfung im laufenden Spiel

14 Bilder, mit `RE15_FORCE_CUT` + `RE15_POCC_TP` an gemessenen Standplätzen erzeugt,
**jedes selbst angesehen**. Das Urteil ist zusätzlich gemessen
(`protokoll/sicht_pruef3.py`: Figurpunkte = Abweichung vom reinen Hintergrund > 40; dazu
die Maskenpunkte im Körperkasten, die an dieser Stelle nach der Original-Regel verdecken).

⛔ Zwei Fallen, beide gemessen: `RE15_CONTINUE_TEST` treibt den Titel nur mit einer
Speicherkarte im Exe-Verzeichnis (ohne sie hängt der Lauf im ROOM1170-Vorspann und der Sprung
`@gp` feuert nie); und `sicht_pruef.py` projizierte den Fuß fest auf y=0 — im Treppenhaus
ROOM10A0 steht der Spieler auf Band 8, der Körperkasten lag dadurch außerhalb des Bildes.

| Cut | Bild | Welt | Figurpunkte | Maskenpunkte am Körper | davon verdeckend | mein Urteil am Bild |
|---|---|---|---|---|---|---|
| ROOM10C0 C3 | vor | 200/-5850 | 15 | 551 | **0** | Leon links am Rand, vollständig sichtbar |
| ROOM10C0 C3 | hinter | -5400/-2250 | 23 | 294 | 294 | steht hinter den Bankreihen, Beine verdeckt |
| ROOM10D0 C1 | vor | 2300/950 | 93 | 163 | **0** | steht neben dem Klapptisch, ganz sichtbar |
| ROOM10D0 C1 | hinter | 3500/-4650 | 19 | 310 | 92 | hinter dem Tisch — **nicht mehr zu sehen** |
| ROOM1050 C6 | vor | 23100/-19950 | 75 | 568 | **0** | nur ein Zipfel sichtbar (s. unten) |
| ROOM1050 C6 | hinter | 24300/-22550 | 36 | 74 | 74 | hinter dem Kaffeeautomaten, verdeckt |
| ROOM1000 C0 | vor | 21400/-12050 | 0* | 0* | **0** | steht VOR dem linken Spindblock, ganz sichtbar |
| ROOM1000 C0 | hinter | 16600/-11050 | 0 | 1607 | 1152 | hinter dem rechten Spindblock, verschwunden |
| ROOM1000 C2 | vor | 21400/-3250 | 730 | 439 | **0** | rechts vor der Spindreihe, ganz sichtbar |
| ROOM1000 C2 | hinter | 16600/-2050 | 0 | 1369 | 1304 | hinter der linken Spindreihe, verschwunden |
| ROOM10A0 C1 | vor | 24250/21350 (Band 8) | 367 | 139 | **0** | auf der Treppe, ganz sichtbar |
| ROOM10A0 C0 | hinter | 24450/22150 (Band 8) | 32 | 918 | 918 | hinter der Treppenbrüstung, verdeckt |
| ROOM11F0 C0 | vor | 300/-4300 | 711 | 1640 | **0** | vor den Schaltschränken, ganz sichtbar |
| ROOM11F0 C0 | hinter | 4300/-1100 | 8 | 1063 | 1063 | hinter den Schränken, verschwunden |

**In allen sieben VOR-Bildern verdeckt die Maske NULL Punkte**, in allen sieben
HINTER-Bildern verdeckt sie.

(*) ROOM1000 C0 "vor": der Teleport gleitet den Spieler zum Ziel, im Bild bei Frame 660
stand er noch nicht ganz dort — der gerechnete Koerperkasten (x109..138) liegt daneben und
zaehlt deshalb 0/0. Das BILD ist eindeutig: Leon steht links vor dem Spindblock und ist
vollstaendig gezeichnet. Ich fuehre die Nullen als das, was sie sind: eine Messung am
falschen Ort, kein Beleg.

⛔ Zwei Einschränkungen, die ich nicht schönrede:
* **ROOM1050 C6**: in beiden Bildern ist die Figur kaum zu sehen (75 bzw. 36 gezeichnete
  Punkte). Das Paar belegt hier nichts, was ich mit dem Auge prüfen könnte — es gilt nur die
  Messung (0 von 568 verdeckenden Maskenpunkten "vor", 74 von 74 "hinter").
* **ROOM10A0 C2** hat auf dem Band, auf dem der Spieler nach dem Sprung steht (Band 8),
  überhaupt keinen berührenden Standplatz, und nach der Bucket-Toleranz auch gar keinen
  VOR-Platz mehr (VOR n=0). Für den Raum zeigen stattdessen ROOM10A0 C1 (vor) und C0
  (hinter) beide Fälle — im selben Winkel ist das hier nicht zu haben.

---

## 9. Offen

1. **ROOM11F0 C1** und **ROOM1000 C3** haben jetzt keine Maske. Beide Winkel hätten einen
   Vordergrund-Gegenstand (Schaltschränke bzw. Kabinenwand); dafür fehlt eine Freistellung.
2. **ROOM1000 C4** (63 von 153 HINTER-Plätzen frei) und **ROOM10A0 C5** (30 von 259): das
   Tiefenmodell liegt dort systematisch zu fern. Das ist der bewusste Preis der Statistik
   MAXIMUM aus Phase 2 und unverändert offen.
3. **ROOM10F0 C4/C5**: HINTER-Abdeckung schlecht (959 von 1178 bzw. 303 von 979 frei), weil
   die Silhouetten aus meinen einzelnen Bürostuhl-Quadern bestehen. Freistellungen dafür?
4. Die Entscheidung "die Platte in ROOM10D0 C1 trägt EINE Tiefenstufe" ist eine gestalterische
   Wahl, kein Messwert (§2).
5. Alpha-Schwelle 110 und die Standplatz-Schranke 95 % bleiben Heuristiken ohne Beleg.
6. Der PSX-Farbschlüssel (CLUT 0x0000 → 0x8000) ist weiterhin nur aus psx-spx abgeleitet, nicht
   auf Hardware gemessen (das PSX-Target baut hier nicht).
