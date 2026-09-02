# Grundriss aus Kollision + Türgraph — Machbarkeit gemessen

**Datum:** 2026-09-01 · **Anlass:** Nutzer-Entscheidung „du darfst vom Original abweichen,
wenn du die Original-Symbole verwendest und damit eine nahezu perfekte Karte kreieren
kannst" — und seine Beobachtung davor: *„ein neues Kartenstück schließt in der Map genau
da an, wo man den Raum davor durch die Tür verlassen hat."*

Prototyp: `loeser_prototyp.py`.

## 1. Das Modell funktioniert

Aufbau: Breitensuche durch den Türgraph, jeder Raum an seinem schon gesetzten Nachbarn
ausgerichtet. Maßstab = Median der ausgelieferten Zeilen @0x800768b0 derselben Stage
(STAGE1 459/464 Welteinheiten je Pixel, gemessen).

```
Seite 4 (3F): 5 Räume, 3 Durchgänge   Türversatz 0,0 px   Überlappung  8,1 %
Seite 3 (2F): 6 Räume, 6 Durchgänge   Türversatz 0,0 px   Überlappung 11,6 %
Seite 2 (1F): 12 Räume, 12 Durchgänge Türversatz 12,0 px  Überlappung 11,7 %
```

**Türversatz 0,0 px** heißt: die beiden Enden eines Durchgangs liegen exakt aufeinander.
Genau das, was der Nutzer beschrieben hat, und was die gemalte Karte des Originals
*nicht* leistet (dort liegen sie im Median 7,5 px auseinander — sogar mit den
ausgelieferten Maßstabszeilen).

## 2. Zwei eigene Fehler auf dem Weg dahin

1. **Spawn statt Gegen-Tür als Anker.** `nx/nz` ist die Stelle, an der der Spieler im
   Zielraum *erscheint* — schon ein Stück hinter der Tür. Darauf ausgerichtet schieben
   sich die Räume um genau diesen Abstand ineinander (22–27 % doppelt belegte Fläche).
   Richtig ist der reziproke Tür-Datensatz des Nachbarn.
2. **Nur verschoben, nicht gedreht.** Jeder RDT-Raum hat sein eigenes lokales
   Koordinatensystem. Die Drehung ist ableitbar: die Wandachse steckt im Tür-Trigger
   (längs der Wand gestreckt), der 180°-Rest fällt über die Forderung, dass der Nachbar
   auf der *anderen* Seite der Wand liegt. **Das Verfahren wählt für ROOM1120
   selbständig 180°** — dasselbe Ergebnis, das vorher unabhängig aus dem gemalten
   Türsymbol und der Nachbarposition gemessen wurde.

## 3. Gelöst — der Löser liefert jetzt einen brauchbaren Grundriss

Zwei Schritte fehlten noch:

**AUSRÜCKEN.** Der Türanker heftet zwei Punkte aufeinander, die *beide* im Inneren ihres
Raums liegen (das Trigger-Rechteck deckt den Anlaufbereich ab, nicht nur die Laibung).
Die Räume drangen dadurch entlang der Tür-Normalen ineinander — ein Band längs der
gemeinsamen Wand (ROOM1120 ↔ ROOM1130: 241 px). Jeder Raum wird jetzt genau so weit
zurückgeschoben, bis nichts mehr doppelt liegt; längs der Wand ändert das nichts.

**EINPASSEN.** Ein Stockwerk misst im ausgelieferten Maßstab bis zu 203 × 299 px, das
Kartenfeld ist 132 × 140. Der fertige Grundriss wird als Ganzes gleichmäßig um seinen
Schwerpunkt verkleinert — die relative Lage und damit alle Nachbarschaften bleiben.

| Blatt | Überlappung | Durchgänge berührend | Ausdehnung nach Einpassen |
|---|---|---|---|
| Seite 4 (3F) | 0,0 % | 3 von 3 | passt |
| Seite 3 (2F) | 1,6 % | 6 von 6 | 133 × 106 |
| Seite 2 (1F) | 2,2 % | 12 von 12 | 96 × 141 |

Die verbleibenden 1,6–2,2 % entstehen erst beim Runden auf den kleineren Maßstab.

⛔ Der zuvor gemeldete „Türversatz von 15–17 px" war ein **Messfehler**: er maß den
Abstand der beiden TRIGGER-Mittelpunkte, und die sitzen bauartbedingt im Rauminneren.
Das richtige Maß ist der Abstand der beiden Raumflächen — der ist 0–1,4 px, also
berühren sich alle Durchgänge.

## 4. Was noch fehlt

* Räume ohne reziproken Türpartner werden nicht gesetzt (Seite 4: ROOM1160).
* Die vollständige Suche über alle acht Posen je Raum geht nur bis ~6 Räume; darüber
  entscheidet die greedy Reihenfolge.
* Einbau in den Generator und den Zeichner, mit den Original-Tür- und Treppensymbolen.

**Die Passform ist der eigentliche Befund:** ein ganzes Stockwerk misst im
*ausgelieferten* Maßstab rund 160 × 264 px, das Kartenfeld ist 132 × 140. Das Gebäude
passt bei diesem Maßstab nicht auf ein Blatt — genau deshalb hat der Künstler die Räume
verkleinert und schematisiert, und genau deshalb ist seine Kunst nicht maßstabsgetreu
zur Kollision. Ein maßstabsgetreuer Grundriss braucht also einen eigenen, kleineren
Maßstab je Blatt; die *relative* Lage bleibt dabei erhalten.

Die verbleibende Überlappung von 8–12 % ist die Zahl, an der weitergearbeitet wird.
Kandidaten, noch nicht auseinandergehalten:
* Räume, deren Kollisionsdaten über die eigentliche Raumgrenze hinausreichen,
* Räume ohne reziproken Türpartner (Seite 4: ROOM1160 wird gar nicht gesetzt),
* verbleibende Lage-Fehler in der greedy Reihenfolge (die vollständige Suche über alle
  8 Posen je Raum geht nur bis ~6 Räume je Blatt).

## 4. Was das für die Symbole heißt

Tür- und Treppensymbole liegen bereits als Original-Stempel vor (der 13-Pixel-Nischen-
Stempel aus `re15_inv_screen.c`, gewonnen aus `DATA/MAP0x.PIX`, plus die Treppensprossen).
Sie hängen an Weltpositionen und wandern mit dem Raum mit — der Wechsel des Grundrisses
ändert daran nichts.

---

## 5. Eingebaut — Stand 2026-09-02

Der Löser sitzt jetzt im Generator (`tools/gen_map_zones.py`), die Engine zeichnet die
Grundrisse (`re15_map_zones.c`, `re15_inv_screen.c`). 261 von 261 Pins grün.

### Vier Befunde, die den Einbau erst brauchbar gemacht haben

**(a) Die Kollisionszellen sind der begehbare WEG, nicht der Raum.** ROOM1120 besteht
aus elf Streifen, der breiteste vier Kartenpixel (`[100,90,4,44]`, `[100,90,24,4]`, …) —
der Ring um die Möbel. Ungefüllt gezeichnet sah ein Büro aus wie ein Labyrinth aus
Fluren. Eingeschlossene Löcher werden jetzt auf dem Gitter der Zell-Kanten geschlossen
(exakt, ohne Rasterwahl), zusätzlich die orthogonal-konvexe Hülle — sonst leckt die Flut
durch die Türöffnung und die Möbelfläche bleibt ein Loch (ROOM1150: 18 Streifen → 4).

**(b) Ein RDT-Raum ist nicht ein Ort.** Der Löser arbeitete je RAUM und zeichnete alle
Zonen als einen Klumpen: ROOM1080s Fahrstuhlkabine misst 14×13 px, ihr Grundriss war
34×22 px, und ROOM1140s beide Zonen trugen denselben Kasten. Er arbeitet jetzt je ORT
(Raum, Zone) — Zellen aus der Zonen-Box, Türen aus der Zone, Ziel einer Tür ist der Ort,
in dem ihr Spawn liegt.

**(c) Ein Blatt zerfällt in mehrere Komponenten des Türgraphen.** Seite 8 in
{4000,4010,4030}, {4080,4090,40A0,40B0}, den türlosen 4020 und die Wurzelkomponente. Die
Breitensuche setzte nur die Wurzelkomponente: 3 von 11 Räumen. Die Brücken *sind* da, nur
einseitig (4000→4020, 4080→4030 ohne reziproken Gegen-Datensatz); dazu ein Anbau-Zweig
für Orte ganz ohne Türverbindung. **112 von 112 Orten liegen jetzt.**

**(d) Überlappung allein ist das falsche Maß.** Ein Posen-Nachlauf, der nur sie
minimierte, drückte Seite 1 von 7,3 % auf 3,0 % — und die berührenden Durchgänge von 9
von 9 auf 3 von 9. Gerechnet werden jetzt beide Wege je Blatt und der nach dem Modell des
Nutzers bessere genommen: mehr berührende Durchgänge zuerst, bei Gleichstand weniger
Überlappung.

### Ergebnis

| | vorher (Kunst-Rechtecke) | jetzt (Grundrisse) |
|---|---|---|
| Orte mit Zeichnung | 33 von 112 | **112 von 112** |
| berührende Durchgänge | — (Median 7,5 px auseinander) | **85 von 89** |
| Überlappung je Blatt | — | 0,0 – 5,7 % |

Gemessen an ROOM1120 ↔ ROOM1130, dem Fall aus dem Nutzer-Report *„im Room 1120 lande ich
plötzlich auf der falschen Seite"*: der Punkt, an dem der Spieler erscheint, liegt **2 px**
neben der Tür, durch die er kam (Gegenprobe: die andere Tür desselben Raums 21 px weg).
Festgehalten in `test_map_orient.c`.

### Was noch offen ist

* Die Zweitzeichnung einer Etage wird in das Rechteck des Künstlers *eingepasst* (Platz
  von ihm, Form aus der Kollision, Größe aus seinem Rechteck). Sauberer wäre, den Ort
  auch auf dem Zielblatt vom Löser setzen zu lassen.
* Seite 7 hat 15 von 18 Durchgängen berührend, Seite 6 12 von 13 — dort bleibt Arbeit.

---

## 6. Gesamtblick über alle 13 Blätter — 2026-09-02

Nutzer: *„Ich will im gesamten sehen ob dein Ansatz taugt."* Dafür eine Mess-Schiene
`RE15_MAP_SHOT_SWEEP=<präfix>`: alle 13 Kartenblätter in **einem** Lauf (statt 13 Läufen
à vier Minuten). Abzug: `shots/karte_gesamt/`.

### Was der erste Gesamtblick gezeigt hat

**Der Maßstab schwankte zwischen den Blättern desselben Gebäudes um Faktor 3,3** —
Polizeiwache ROOF 458, 3F 585, 2F 705, B1 799, 1F 1057, B2 1524 Welteinheiten je Pixel.
Das Gebäude änderte beim Blättern seine Größe. Ursache: jedes Blatt wurde für sich ins
Kartenfeld eingepasst.

Dazu Inseln auf B2: mehrere Räume ohne Türverbindung *auf diesem Blatt*, vom Anbau-Zweig
nebeneinandergestellt — was den Maßstab dieses Blattes zusätzlich vergröberte.

### Was daraus wurde

**(a) Gast-Lagen.** Ein Ort, dessen Band auf ein anderes Blatt führt, bekommt dort eine
eigene Lage vom Löser — statt des bisherigen Notbehelfs, seinen Grundriss in ein
Kunst-Rechteck des Künstlers einzupassen. 25 Etagen-Zeilen, 14 Gast-Lagen. Das ersetzt
das Provisorium **und** gibt den Inseln ihren Anker: B2 ging von 8 auf 9 Orte und von
9/9 auf **11/11** berührende Durchgänge.

**(b) Gemeinsamer Rahmen beim Lösen.** Die Blätter, die Orte teilen, bilden Komponenten
({1,2,3,4,5} Polizeiwache, {8,9,10,11} Labor, {0,6}). Innerhalb einer Komponente werden
sie der Reihe nach gelöst und geben die vollständige Lage der geteilten Orte weiter
(`feste_lagen`) — jedes Stockwerk wächst um dieselben Anker. Seite 3 fiel dadurch von
**20,6 % auf 0,5 %** Überlappung.

**(c) ⛔ Gemeinsamer MASSSTAB — gemessen und verworfen.** Ein Maßstab je Gebäude lässt
die Stockwerke stapeln, kostet aber genau die Lesbarkeit: die Vereinigung aller Blätter
der Polizeiwache misst 376 × 324 px, das Kartenfeld 132 × 140 → Faktor 0,32. ROOM1120
fiel von 34 × 36 auf 14 × 15 px, das Dach auf einen 20-px-Kasten in einem leeren Blatt,
jedes Blatt nutzte noch ein Viertel seiner Fläche. Grund: unsere Stockwerke sind **nicht
deckungsgleich** — der Löser kennt nur Nachbarschaft durch Türen; wohin ein Stockwerk als
Ganzes zeigt, sagen die Daten nicht. Der Nutzer hat Stapeln auch nie verlangt.
Geblieben ist der gemeinsame Rahmen beim *Lösen*, eingepasst wird **je Blatt**.

**(d) Entzerren nach dem Einpassen.** Das gröbere Raster rundet neu und erzeugt
Überlappung. `entzerren_komp` schiebt pixelweise auseinander — und nimmt einen Zug nur
an, wenn er **keinen Durchgang kostet**. Ohne diese Bedingung fiel Seite 3 von 20,6 % auf
12,8 % Überlappung und zugleich von 6/7 auf 3/7 berührende Durchgänge: dieselbe Falle wie
beim Posen-Nachlauf (siehe §5 (d)).

### Stand

| | vor der Umstellung | nach §5 | jetzt |
|---|---|---|---|
| Orte mit Zeichnung | 33 | 112 | **126** |
| berührende Durchgänge | — | 85 von 89 | **92 von 100** |
| Überlappung je Blatt | — | 0,0 – 11,9 % | **0,0 – 4,0 %** |
| Maßstabsspanne im Gebäude | — | 458 – 1524 | 488 – 1153 |

Jedes Blatt füllt sein Feld (95–132 px von 132). 262/262 Pins grün.

### Was bleibt

* Seite 7 (Factory) 14 von 18 Durchgängen, Seite 4 (3F) 5 von 7 — dort stehen Zyklen im
  Türgraph, die sich geometrisch nicht schließen lassen.
* Einzelne Gast-Lagen ohne Kante auf ihrem Zielblatt werden angebaut und stehen dann
  abseits (2F oben links, Labor B4).
* Die Stockwerke stapeln nicht. Das ist eine bewusste Entscheidung, kein Versehen — siehe
  (c).

---

## 7. Nutzer-Report 2026-09-02: Lücken, Rotation, Übertragungen

> *„Türen sind oft falsch positioniert und rotiert, die Kartenstücke haben Abstände
> zueinander, was nicht sein kann, die müssen Kante an Kante sein … Und die
> Übertragungen von einem Raum zum anderen sind SAU oft falsch."*

Alle drei Punkte waren berechtigt. Gemessen an v0.3.85:

| | v0.3.85 |
|---|---|
| Türsymbole, die vom Nachbarn **weg** zeigen | 34 von 93 (**37 %**) |
| Flächen-Lücke zwischen Räumen mit gemeinsamer Tür | Median 6 px, **bis 55 px** |
| Verrutschen längs der gemeinsamen Wand | Median 5 px, **bis 72 px** |
| Marker-Sprung beim Durchgang | Median 8 px, bis 74 px |

⛔ Mein bisheriges Maß „92 von 100 Durchgängen berührend" war **zu schwach**: es
erlaubte ±2 px Toleranz auf *Flächen* und sagte nichts über die Stelle, an der man
erscheint. Deshalb sah der Nutzer Fehler, die meine Zahlen nicht zeigten.

### Ursache 1 — der Löser hielt nur einen Spannbaum ein

Die Breitensuche heftet jeden Ort an **genau einen** Nachbarn. Jede weitere Tür — jeder
Ring im Türgraph — war nur ein weicher Zug, den `zusammenziehen` beim ersten drohenden
Überlapp aufgab. Bei 126 Orten und 205 Türen sind rund 80 Türen nie eingehalten worden;
genau die trugen die 30–70-px-Lücken.

Ersetzt durch einen **Ausgleich über alle Kanten zugleich** (Gauss-Seidel, `federn`),
abwechselnd mit einer Trennung (`_trenn_sweep`), die mit den Runden abkühlt.

Drei eigene Fehler auf dem Weg, alle durch Messung entlarvt:
1. **Türpunkte vertauscht.** `_feder_nachbarn` speicherte (Nachbar, *eigener* Punkt)
   statt (Nachbar, *dessen* Punkt). Erkennbar daran, dass der Ausgleich den Kanten-Rest
   *vergrößerte* (Seite 7: Median 14 → 45 px), obwohl Gauss-Seidel ihn nur verkleinern
   kann.
2. **Ruhelänge 0.** Beide Tür-Datensätze liegen im *Inneren* ihres Raums (das
   Trigger-Rechteck deckt den Anlaufbereich ab) — die Feder zog die Räume um genau diese
   Tiefen ineinander, bis 37 % Überlappung.
3. **Ruhelänge „bis die Räume disjunkt sind".** Bei ineinandergreifenden L-Formen sind
   das 10–20 px; der Sprung stieg von 8 auf 20 px im Median.

Richtig ist die Summe der beiden **Wandtiefen**, quer zur Wand gemessen — die Richtung
kommt aus der kurzen Achse des Trigger-Rechtecks, das Vorzeichen aus der Zellgeometrie.

### Ursache 2 — die Wandseite kam aus der Silhouette

`snap_grundriss` bestimmte die Seite daran, wohin der Raum weitergeht. Das benennt die
**Wand**, in der die Tür sitzt, nicht die **Seite**, die zum Nachbarn schaut. Bei einem
Paar ist die Richtung bekannt und schlägt die Silhouette — Achse *und* Vorzeichen.

### Stand

| | v0.3.85 | jetzt |
|---|---|---|
| Türsymbole, die vom Nachbarn weg zeigen | 34 von 93 | **0 von 95** |
| Flächen-Lücke, schlimmste | 55 px | **14 px** |
| Verrutschen, schlimmstes | 72 px | **41 px** |
| Durchgänge, die anstoßen | 92 von 100 | **95 von 101** |
| Überlappung je Blatt | 0 – 4,0 % | 0 – 16,3 % |

**Ehrlich dazu:** die Überlappung ist der Preis. Räume, die eine Tür teilen, dürfen sich
jetzt um einen Streifen überlappen — sonst entsteht genau die Lücke, die gemeldet war.
Auf dem dichten Factory-Blatt summiert sich das auf 16 %. Der Marker-Sprung liegt
weiter bei 9 px im Median; davon ist der größte Teil die **echte Wanddicke** — die
beiden Tür-Datensätze liegen real so weit auseinander.

Festgehalten in `test_map_durchgang.c` (Symbol-Richtung, Lücke, Gegenprobe).

### Neu aufgefallen (nicht in v0.3.86 behoben)

Das Audit zählt nach dem Umbau **10 Markenpaare, die praktisch aufeinander liegen**
(≤ 2 px, gleiche Art) — vorher 0. Aufgeschlüsselt:

* **1 Fall** ist derselbe Durchgang zweimal gezeichnet (Seite 2, zid 16/23 bei
  (165,103) und (165,105)) — die Paarung hat ihn nicht zusammengeführt.
* **9 Fälle** sind **zwei verschiedene Türen**, die auf denselben Wandpunkt schnappen.
  Seit die Räume aneinanderstoßen, liegen ihre Projektionen dichter beieinander, und
  `snap_grundriss` zieht beide auf dasselbe Randpixel.

Beides ist sichtbar: eine Tür verdeckt die andere.

**Behoben 2026-09-02.** Ein Nachlauf schiebt Türsymbole, die sich auf 2 px nähern,
**längs ihrer Wand** auseinander (quer wäre die Marke von der Wand weg) und prüft dabei,
dass die neue Stelle noch auf der gezeichneten Fläche liegt. 10 Paare → **1**; das
verbleibende ist derselbe Durchgang, den die Paarung nicht gefunden hat.

⛔ Nur TÜREN. Ein erster Wurf schob auch Treppenmarken auseinander — damit fielen
Duplikate, die vorher an derselben Stelle zusammenfielen und von der Schlüssel-
Entdopplung geschluckt wurden, wieder auseinander und wurden zweimal gezeichnet: die Zahl
der Treppensymbole stieg von 30 auf 52.

Nebenwirkung, gewollt: 5 Türen, die vorher von einer anderen verdeckt und dabei
weggedoppelt wurden, sind jetzt sichtbar (160 → 165). Und ROOM1130 ↔ ROOM1140 klafft
nicht mehr 9 px, sondern **1 px**.

---

## 8. Live-Debugging, 2026-09-02 (dritter Anlauf)

> *„Springt immer noch durch die Kartenbereiche … Debugge dich selber, indem du dich
> durch die Räume portest mit dem Debug Menu und durch die Türen läufst."*

Der Auftrag war richtig: meine Sonden liefen auf der erzeugten Tabelle und bildeten die
Marker-Rechnung in Python **nach**. Neu ist deshalb
`tests/integration/test_map_uebergang.c` — es lädt jeden Raum wie das Spiel
(`re15_rdt_parse` + `scd_room_reenter`, also mit installierten AOTs), liest die Türen aus
`g_aot` und ruft für beide Seiten die **echte** Engine-Funktion
`re15_inv_map_marker()`. `RE15_UEBERGANG_LISTE=1` druckt jede Tür einzeln.

Erste Live-Messung: **116 Übergänge, Median 9 px, schlimmster 63 px, kein einziger unter
2 px.**

### Vier Ursachen, alle nur live sichtbar

**(a) Der Marker wird mit 4 px Rand-Reserve ins Rechteck geklemmt.** Auf einem
Kunst-Rechteck ist der Rand bloßer Rahmen; auf einem Grundriss ist er die **Wand** — und
dort steht der Spieler beim Durchschreiten. Die Reserve schob ihn auf *beiden* Seiten
4 px von der Wand weg und machte allein bis zu 8 px des Sprungs aus, unabhängig von der
Geometrie. Auf Grundrissen jetzt 1 px.

**(b) Einseitige Türen wurden vom Ausgleich gar nicht erfasst.** Sie standen nur in
`notkanten` und wurden einmal beim Aufbau benutzt. Der schlimmste Fall überhaupt
(ROOM2070 → ROOM2000, 63 px) war so einer.

**(c) Meine Ruhelänge war um eine Größenordnung zu groß.** Der Strahl vom Türpunkt nach
außen läuft bei großen Räumen tief hinein, bevor er austritt — gemessene Ruhelängen
4 bis 22 px, also bis zu 22 000 Welteinheiten „Wand". Die Tiefe steht aber im
**Trigger-Rechteck**: dessen kurze Seite ist die Anlauftiefe, der Türdatensatz sitzt in
deren Mitte. Damit sind es **0,9 px im Median**.

**(d) Die Wandseite der Türsymbole kam aus der Silhouette** (welche *Wand*), nicht aus
der Nachbarrichtung (welche *Seite*) — 34 von 93 zeigten weg. Behoben, Achse und
Vorzeichen kommen jetzt vom Partner.

### Was messbar bleibt — und warum

| | vorher | jetzt |
|---|---|---|
| schlimmster Sprung beim Durchschreiten | **63 px** | **29 px** |
| Türsymbole, die vom Nachbarn weg zeigen | 34 von 93 | **0 von 95** |
| Durchgänge, die anstoßen | 92 von 100 | **96 von 101** |
| Median-Sprung | 9 px | 10 px |

Der **Median bewegt sich nicht** — und das hat einen belegbaren Grund. Getrennt gemessen
über alle 205 reziproken Durchgänge:

* **unvermeidbare Geometrie** (Summe der halben Trigger-Tiefen): Median **0,9 px**
* **Layout-Fehler** (Abstand der beiden Türpunkte auf der Karte): Median **9,0 px**

Der Ausgleich konvergiert nachweislich: auf Blättern ohne Ringe im Türgraph geht der
Fehler gegen die Ruhelänge auf **exakt 0,0**. Auf Blättern mit Ringen bleibt er stehen —
**die Räume von RE1.5 bilden keinen metrisch geschlossenen Grundriss.** Jeder Raum hat
sein eigenes Koordinatensystem; die Türdaten sagen, *wohin* eine Tür führt, nicht dass
die Räume kacheln. Wo ein Rundweg im Türgraph nicht schließt, muss der Fehler verteilt
werden — er lässt sich nicht wegrechnen, nur umverteilen.

### Fünf Einstellungen gemessen, eine gewählt

| Einstellung | schlimmster Sprung | anstoßend | max. Überlappung |
|---|---|---|---|
| **gewählt** (feste Schrittweite, Federanteil ≥ 0,08) | **29 px** | **96/101** | 30,7 % |
| Trennung 2× in der letzten Hälfte | 47 px | 88/101 | 14,8 % |
| Schrittweite wächst mit der Überlappung | 45 px | 88/101 | 9,9 % |
| Mittelweg | 31 px | 91/101 | 21,7 % |
| v0.3.86 (Ausgangsstand) | 63 px | 95/101 | 16,3 % |

Gewählt ist die Einstellung mit den besten **Nutzer-Größen**. Die Überlappung ist der
Preis — im Abzug liest sie sich anders als in der Zahl: überlappende Räume in derselben
Farbe wirken *verbunden*, Lücken wirken kaputt.

### Offen

* Median-Sprung 10 px, davon ~9 px Layout — begrenzt durch die nicht schließenden Ringe.
* Seite 3 (2F) 30,7 %, Seite 7 (Factory) 22,9 % Überlappung.
* 10 Markenpaare liegen praktisch aufeinander (§7).
* Seite 12 nur 1 von 3 Durchgängen anstoßend.

### Die Tür zieht den Marker an — der sichtbare Sprung ohne die unmögliche Geometrie

Der Rest-Sprung lässt sich nicht wegrechnen (die Räume kacheln nicht), der **sichtbare**
schon: ein Durchgang ist EIN Ort und trägt EIN Symbol. Steht der Spieler im Tür-Trigger,
zieht sein Marker auf dieses Symbol — von *beiden* Seiten auf dieselbe Stelle
(`tuer_anziehen` in `re15_inv_screen.c`). Das Gewicht wächst stetig zur Türmitte hin, es
gibt also keinen Sprung beim Betreten des Bereichs; der Zugbereich ist doppelt so groß wie
der Trigger, weil man beim *Ankommen* am Spawn steht und der knapp dahinter liegt.

Gemessen an der echten Abfolge (im Trigger stehen → drücken → am Spawn erscheinen):

| | ohne Zug | mit Zug |
|---|---|---|
| Median-Sprung | 9 px | **5 px** |
| Übergänge unter 2 px | 0 von 102 | **27 von 102** |
| Übergänge unter 8 px | 42 % | **62 %** |
| schlimmster | 63 px (v0.3.86) | **35 px** |

Es ist eine PORT-ERGÄNZUNG: das Original kennt weder Grundrisse noch diese Symbole.

### Zum Türzug: was er kostet

Der Zug ist überall **stetig** — das Gewicht fällt zum Rand des Zugbereichs auf null, es
gibt also nirgends einen Sprung. Wer aber in einem Flur *an* einer Tür vorbeigeht,
durchquert deren Zugbereich, und sein Marker wird dabei weich zur Tür hin und wieder
zurück gelenkt. Quer zur Wand bleibt das Gewicht dabei klein (es ist das Minimum beider
Achsen), die Ablenkung also gering; voll wird der Zug nur, wenn man wirklich vor der Tür
steht. Sollte sich der Marker beim Vorbeilaufen dennoch „klebrig" anfühlen, ist die
Stellschraube das Maximalgewicht in `tuer_anziehen` (derzeit 256 = voll).

---

## 9. Neu gefasst nach der Vorgabe des Nutzers — 2026-09-02

> *„Du brauchst nicht so viel verdammt. Du brauchst: Kollisionsboxen pro Raum pro Etage,
> alle Türen und wo sie hinführen, alle Treppen. … Raum A hat die äußere Kollisionsbox
> von Breite X bis Breite Y. Zack, hast du schon den Grundriss. … Dann gehst du durch
> die Tür: der nächste Raum schließt EXAKT an die Wand an, überlappt die Wand also.
> … Halte dich nicht an solchem Quatsch wie ‚aber die Tür spawnt mich 500 px weiter im
> Raum'. Die Karte ist nur ein SCHEMA."*

Der Löser ist danach **vollständig neu gefasst**. Was wegfällt: die ausgefüllten
Silhouetten, die Federn über alle Kanten, die Ruhelängen aus der Wanddicke, das
Entzerren, das Abkühlen — alles, was ich schrittweise draufgebaut hatte, um zwei
ineinandergreifende L-Formen gleichzeitig zu verheften und überlappungsfrei zu stellen.
Das geht nicht, und das war die ganze Zeit der Fehler im Ansatz, nicht in der Ausführung.

**Was jetzt dasteht:**

* Ein **Ort ist ein Rechteck** — die äußere Bbox seiner Kollisionszellen.
* Ein **Durchgang** legt das nächste Rechteck mit seiner Wand exakt auf die Wand des
  vorigen: die Gegen-Tür muss auf der gegenüberliegenden Wand sitzen, dann fallen die
  Wandlinien zusammen und die Türpunkte liegen längs der Wand übereinander.
* Gemessen wird auf der **Wand**, nicht im Rauminneren — der Tür-Datensatz sitzt in der
  Mitte des Anlauf-Triggers, das Symbol aber auf der Wand. Wer die Rohpunkte vergleicht,
  misst die Anlauftiefe mit und behauptet einen Fehler, den die Zeichnung nicht hat.
* **Rechtecke kacheln immer.** Lücken und Verrutschen können gar nicht erst entstehen.

Dazu zwei Ergänzungen, beide gemessen begründet:
* **Wurzelsuche.** Die Breitensuche bedient eine Tür nur, solange der Zielraum noch nicht
  liegt; welche Türen offen bleiben, hängt allein von der Reihenfolge ab. Es wird jede
  Wurzel durchprobiert und die Karte mit den meisten deckungsgleichen Türen genommen.
* **Nachbesserung.** Für jede offen gebliebene Tür wird probiert, einen der beiden Räume
  stattdessen an *ihr* anzulegen; übernommen wird nur, was insgesamt besser wird. ⛔ Das
  Ziel enthält den **Ausreißer**: ein erster Wurf zählte nur die deckungsgleichen Türen —
  die Zahl stieg von 86 auf 91, und der schlimmste Übergang sprang von 49 auf 109 px.

### Ergebnis

| live gemessen (`integration_map_uebergang`) | v0.3.88 | jetzt |
|---|---|---|
| Übergänge unter 4 px | 45 von 102 | **66 von 102** |
| Übergänge unter 8 px | 62 % | **82 %** |
| mittlerer Sprung | 8 px | **5 px** |
| schlimmster Sprung | 35 px | **34 px** |

Im Generator fallen **91 von 101** Durchgängen exakt auf dasselbe Pixel (Median 0 px);
ROOM1120↔ROOM1130 und ROOM1130↔ROOM1140 klaffen **0 px**.

Die Überlappung ist höher (bis 31 %) — sie ist die gewollte Folge: *„er schließt exakt an
die Wand an, überlappt die Wand also."* Wo ein Rundweg im Türgraph nicht schließt,
überlappen zwei Rechtecke, statt eine Lücke zu lassen.

**Offen:** 18 von 102 Übergängen über 8 px — die Ringe, die auch die Nachbesserung nicht
schließt. Ein Markenpaar liegt noch aufeinander (zwei Durchgänge in denselben Raum, für
die längs der Wand kein Platz mehr war).

## §10 — Nutzer-Report 2026-09-02: „Türen durch die Bank falsch platziert"

> „Die Türen sind durch die Bank weg alle falsch platziert, und oftmals machst du so
> große sprünge.... Nach dem Durchgehen der Tür ist man in der Mitte des Raumes in ein
> Rechteck das nicht an das vorherige anschließt."

### Ursache 1 — die Laufzeit wählte das Türsymbol nach NÄHE

`tuer_anziehen()` zog den Marker auf „die nächstgelegene sichtbare Marke auf dem Blatt".
Nähe ist keine Identität: ein Raum hat bis zu sieben Türen.

Gemessen über alle 261 Türen im Zugbereich einer Marke:

| Befund | Anzahl | Anteil |
|---|---|---|
| zog auf ein **fremdes** Symbol | 70 | 27 % |
| hatte **gar keine** eigene Marke | 62 | 24 % |
| **zusammen falsch** | **132** | **51 %** |

Versatz zum richtigen Symbol: Median 13 px, schlimmster 81 px. Live sichtbar daran, dass
drei **verschiedene** Räume (4080, 4090, 40A0) denselben Markerpixel (212,140) meldeten.

Behoben: die Tür kennt ihr Ziel (`door_params` → `dest_stage`/`dest_room`), die Marke
kennt ihre beiden Zonen (`re15_map_mark_zonen`). Gesucht wird die Marke, die **meine**
Zone mit einer Zone des **Zielraums** verbindet — eindeutig, ohne jeden Abstand.
77 % lösen so exakt auf, der Rest über die ungepaarte Marke der eigenen Zone.
Dazu zwei Folgefehler: die Schleife nahm den *ersten* Tür-AOT im Zugbereich, und eine
Fehlanzeige machte `return` statt `continue`.

### Ursache 2 — die Marke entstand aus ZWEI getrennten Projektionen

Jeder Raum projizierte seinen eigenen Türdatensatz und schnappte ihn an die *nächste*
Wand **seines** Rechtecks. Gemessen: der Türpunkt liegt im Median 3 px von der nächsten
Wand, aber nur 4 px vor der **zweitnächsten**; bei 23 % beträgt der Vorsprung ≤ 1 px.
„Nächste Wand" ist dort ein Münzwurf. Folge: **58 %** der Symbole lagen auf einer anderen
Wand als der, an der der Löser die beiden Räume verheftet hat.

Behoben: die Marke wird aus der **Berührung der beiden Rechtecke** bestimmt. Längs der
Wand gilt das Mittel der beiden **rohen** Projektionen — beide Räume projizieren dieselbe
Tür unabhängig und stimmen auf 0–1 px überein (ROOM1000↔1050: 144/143, 164/164, 167/167).
Ergebnis: **97 %** der gepaarten Marken sitzen auf der Überdeckung beider Rechtecke
(Median 0 px, max 3 px).

⛔ Nur eine **dünne** Überdeckung (≤ 4 px) ist eine Wand. Wo zwei Rechtecke großflächig
überlappen (bis 31 %), gibt es keine gemeinsame Wandlinie; „Mitte der Überdeckung" legte
das Symbol dort quer durch den Raum (ROOM4050: fünf Marken auf y=119 = exakt die
Mittellinie). Dort gilt die rohe Projektion.

### Ursache 3 — das Gütemaß des Lösers kannte „schließt nicht an" nicht

Es zählte nur deckungsgleiche Türpunkte und den schlimmsten Rest. Vier Raumpaare standen
3–45 px auseinander, und genau diese vier hatten **keine** gepaarte Türmarke — der Löser
hatte ihre Tür nie zum Anlegen benutzt, und die Markenpaarung überspringt sie danach wegen
der Entfernung. Ein Rundschluss. Jetzt ist „getrennte Nachbarn" das **erste** Kriterium.

⚠️ Erster Wurf des Kriteriums war falsch geeicht (`<=` statt `<`) und zählte die
*richtige* Lage — Kante exakt auf Kante — als Fehler; auf Seite 2 meldete es 10 getrennte
von 11 Durchgängen bei 10 deckungsgleichen. Der Widerspruch hat das Maß verraten.

### Stand — live gemessen (`integration_map_uebergang`, 102 Übergänge)

| | v0.3.89 | jetzt |
|---|---|---|
| ≤ 2 px | 27 (26 %) | **37 (36 %)** |
| ≤ 4 px | 66 (64 %) | **75 (73 %)** |
| ≤ 8 px | 84 (82 %) | 84 (82 %) |
| schlimmster | 34 px | **31 px** |
| verdrehte Türsymbole | 39 | **0** |
| Lücke 1120↔1130 / 1130↔1140 | 0 / 1 px | **0 / 0 px** |

⛔ Die alten Zahlen waren **geschmeichelt**: wo beide Seiten auf dasselbe *falsche*
Symbol gezogen wurden, ergab das einen kleinen „Sprung". Die neuen stehen auf strengerer
Basis.

### RE2-Kartenstil (Nutzer-Vorgabe `re2_map_style.png`)

Wandlinie hell und **zustandsfrei** (208,208,200), Füllung trägt den Zustand
(blau 28,60,140 besucht / dunkelrot 74,20,20 aktuell), Tür = kurzer gelber Balken
(224,168,40) **in** der Wand. Der Balken kennt nur die **Achse** der Wand — und löst
damit ein Problem der gerichteten RE1.5-Nische: ein Durchgang gehört zwei Räumen und
liegt auf ihrer gemeinsamen Wand, dort zeigt jede Nische zwangsläufig zu einem hin und
vom anderen weg. Der Spieler-Marker bleibt unverändert (Nutzer-Vorgabe).

## §11 — Nutzer-Report 2026-09-02 (2): Sprung ROOM1170 → ROOM1130, schwarzes Quadrat

> „ich springe beim wechsel von room 1170 zu room 1120 von dem Rechteck und der Tür oben
> bis hin zum Rechteck ganz weit weg und der Tür unten. … Außerdem haben jetzt alle
> Karten Hintergründe ein schwarzes square."

### ⛔ Zuerst: die Messschiene hat den gemeldeten Fall nie gesehen

`test_map_uebergang` sammelte Türen mit `for (r = 0; r < 0x100; r += 0x10)` und lud damit
je Stage **nur ROOM?000…ROOM?0F0**. ROOM1130/1140/1170 waren nie dabei; die Blätter 4
und 5 hatten **null** gemessene Übergänge. Dazu wählte die Schiene das Blatt als
`zn->page` (Hauptzeile) — das Spiel wählt es über das **Band**
(`re15_inv_map_page_shown` → `re15_map_floor_lookup`), wofür die GAST-Zeilen existieren.

Beides behoben. Vorher 102 Übergänge / 0 auf Blatt 4 → jetzt **146 Übergänge / 8 auf
Blatt 4**. Neuer Riegel im Test: ≥ 200 Türen, ≥ 8 Blätter, Blatt 4 enthalten.

### Ursache des Sprungs: dieselbe Tür zweimal im Datensatz

Neun Verbindungen führen denselben Durchgang in **zwei** Aot_set-Records (gleiches Ziel,
gleicher Trigger, gleicher Spawn, gleicher Yaw):

| Verbindung | Records |
|---|---|
| ROOM10D0→ROOM10F0, ROOM1140→ROOM1170, ROOM11B0→ROOM1260, ROOM11E0→ROOM1210, ROOM1230→ROOM1190, ROOM2020→ROOM2030, ROOM2070→ROOM11A0, ROOM3040→ROOM3060, ROOM6010→ROOM6000 | je 2 |

Für das Spiel harmlos, für den Löser nicht: er zählte **zwei Kanten** und opferte deshalb
systematisch jede Verbindung mit nur einem Record. So blieb ROOM1130↔ROOM1170 mit 62 px
offen. Nach dem Entduplizieren fällt die Wahl zugunsten des tatsächlich begangenen Wegs.

### Erschöpfende Suche: 5 von 6 ist auf Blatt 4 das Maximum

Diagnose `RE15_VOLLSUCHE=<seite>` durchsucht alle Wurzeln, alle Drehungen und **alle
Anlegekanten** (der erste Wurf verzweigte nur über die erste Kante und war damit
unvollständig — das wäre ein Suchartefakt gewesen). Ergebnis: höchstens 5 von 6 Kanten
gleichzeitig erfüllbar. **Keine Suchschwäche, sondern Geometrie:** ROOM1170s beide Türen
liegen auf *gegenüberliegenden* Wänden (Z-min und Z-max seiner Box), während ROOM1130 und
ROOM1140 selbst aneinandergrenzen. Ein Rechteck kann nicht auf beiden Seiten zweier
benachbarter Räume liegen.

### Keine Pro-Raum-Spiegelung mehr

Die Projektion `map_x = wx, map_y = −wz` kippt die Händigkeit **einmal** für alle Räume.
Eine zusätzliche Spiegelung *einzelner* Räume hat kein Gegenstück in der Welt — ein Raum
ist nicht mal so und mal seitenverkehrt gebaut. Sie war ein freier Parameter, mit dem der
Löser eine Tür lokal befriedigen und das Gefüge global zerstören konnte.

Gemessen (ohne / mit Spiegelung): ≤ 2 px 39/34, ≤ 8 px 91/90, über 8 px 11/12.
⚠️ Die **löser-eigenen** Zahlen wurden dabei schlechter (mehr getrennte Nachbarn, mehr
Überlappung), die **Live-Größe** besser. Bei Widerspruch gilt die Live-Größe.

### Kein schwarzer Kartengrund

Zum RE2-Stil gehört ein schwarzer Grund, aber als fester Kasten über dem RE1.5-Panel
liegt er auf *jedem* Blatt — auch über der Original-Kachelkunst — und liest sich als
aufgeklebtes Quadrat. Der Kontrast kommt jetzt aus der Zeichnung selbst.

Er hatte außerdem einen Pin **verdeckt**: `test_map_re2_system` prüfte „unter der ersten
Marke liegt eine Fläche" und war nur deshalb grün, weil der Grund unter allem lag. Die
Prüfung war zudem sachlich falsch — eine Tür in einen *unbetretenen* Raum hat zu Recht
keine Raumfläche unter sich. Neu und strenger: **keine** Marke darf hinter einer sie
überdeckenden Fläche liegen (6 Paare, 0 verdeckt).

### Ergebnis

| | vorher | jetzt |
|---|---|---|
| ROOM1170 → ROOM1130 | quer über das Blatt (62 px Rechteck-Lücke) | **6 px** |
| ROOM1130 → ROOM1170 | — | **4 px** |
| gemessene Übergänge | 102 (Blatt 4: 0) | **146 (Blatt 4: 8)** |
| ≤ 4 px | — | 103 (70 %) |
| ≤ 8 px | — | 127 (86 %) |
| schlimmster | — | 30 px |

**Offen:** ROOM1140↔ROOM1170 bleibt auf Blatt 4 notwendigerweise offen (bewiesen
unmöglich). Wer diesen Weg geht, sieht dort weiterhin einen Sprung.

### ⚠️ Grenze der Vollsuche (`RE15_VOLLSUCHE`)

Die Suche zählt eine Kante nur als erfüllt, wenn die beiden Wandpunkte **exakt** (≤ 1 px)
zusammenfallen — und zwar **vor** der Einpassung ins Kartenfeld. Der Löser misst nach der
Einpassung, also auf skalierten Koordinaten. Gegenprobe: für Blatt 12 meldet die Suche
„höchstens 2 von 3", der Löser erreicht dort **3/3**.

Die Suche ist damit eine **untere Schranke**, kein Unmöglichkeitsbeweis. Für Blatt 4 hat
das keine Folgen, weil das Ergebnis dort **konstruktiv** ist: nach dem Entfernen der
Phantom-Kante erreicht der Löser tatsächlich 5/5 mit 0,0 px — dafür braucht es keine
Suche. Wo die Suche für eine Behauptung „unmöglich" herangezogen wird, gehört dieser
Vorbehalt dazu.

Blatt 1: Suche 6 von 10, Löser 6/10 — dort ist der Löser also am Optimum der Suche.

## §12 — Nutzer 2026-09-02 (3): ROOM1140↔ROOM1170 weit auseinander, 1F überlappt stark

> „Im ROOM 1140 führt die eine Tür zurück zum ROOM 1170. Laut map ist die Tür von ROOM
> 1170 hin zu ROOM 1140 ganz links oben. Und die Tür von ROOM 1140 hin zu ROOM 1170 ganz
> rechts oben. Weit entfernt von miteinander verbunden."
> „gerade auf Etage 1F fällt auf, das wir bei den Räumen unglaublich viel Überlappung
> haben … vielleicht kommt das daher das die Räume teilweise breiter dargestellt werden
> als sie sind, der Laufkorridor vom Charakter ist z.T. viel schmaler als das Viereck."

### ⛔ Zuerst zurückgenommen: der Phantom-Tür-Filter aus §11

Er stützte sich auf einen Engine-Abzug beim **Raumbetreten**. Eine Tür, die das Skript
erst später scharf schaltet — ROOM1140s Record trägt `sce = 0x00`, sein Slot wird aber
neu bewaffnet — fehlt dort. Der Nutzer **geht** durch diese Tür; seine Beobachtung
schlägt meine Ableitung. Filter abgeschaltet, `tools/engine_tueren.txt` bleibt als
Messwert erhalten.

### Ursache: die Wandwahl war ein Münzwurf

`anlegen()` zwang die Tür auf die **eine** nächste Wand ihres Rechtecks und verlangte die
Gegen-Tür auf der exakt gegenüberliegenden. Gemessen über alle 282 Türen: der Türpunkt
liegt im Median 3 px von der nächsten Wand, aber nur **4 px** vor der zweitnächsten; bei
**23 %** beträgt der Vorsprung ≤ 1 px. Bei ROOM1130s Tür zu ROOM1140 sind es 1800
Welteinheiten ≈ 3 px.

Drei solche Münzwürfe ergaben auf Blatt 4 einen Widerspruch, den **keine** Anordnung
auflöst:

| Raum | sagt |
|---|---|
| ROOM1130 | 1140 ist oben, 1170 ist rechts |
| ROOM1140 | 1130 ist links, 1170 ist unten |
| ROOM1170 z1 | 1130 und 1140 liegen auf **gegenüberliegenden** Wänden |

1130 und 1140 grenzen aber selbst aneinander — sie können nicht auf zwei Seiten von 1170
liegen. Daher die weit auseinander liegenden Türsymbole.

**Behoben:** `waende()` bietet jede Wand an, die höchstens `WAND_RAND` (4 px = der
gemessene Median-Vorsprung) weiter weg liegt; der Löser sucht die global verträgliche
Zuweisung. Das Gütemaß misst entsprechend nicht mehr gegen die geratene Wand, sondern
gegen die **Berührung** der beiden Rechtecke (`kantenrest()`) — dieselbe Größe, aus der
auch die Türmarke gesetzt wird.

### Wirkung

| Blatt | Überlappung vorher | jetzt |
|---|---|---|
| **2 (1F)** | 22,4 % | **7,5 %** |
| 0 | 25,3 % | **3,5 %** |
| 1 | 36,1 % | **14,8 %** |
| 11 | 11,1 % | **1,2 %** |
| 8 | 5,5 % | **2,5 %** |

`ROOM1140 ↔ ROOM1170` hat jetzt **ein gemeinsames Symbol** auf Blatt 4 bei (166,133),
Rest 0,0 px. Blatt 4: alle sechs Kanten Rest 0–9 px, **nichts getrennt**.
Symbole game-weit: 0 verdreht, 0 Paare übereinander, 103 gepaarte Marken (vorher 99).

### Negatives Ergebnis: das Rechteck auf den begehbaren Kern stutzen bringt fast nichts

Die Vermutung des Nutzers ist als Messung **richtig** — der Füllgrad (begehbare Fläche /
Rechteckfläche) liegt im Median je Blatt bei nur 40–73 %, ROOM1230 bei **12 %**,
ROOM10B0 32 %, ROOM1030 34 %. Aber das Stutzen zahlt sich nicht aus, weil die **Türen an
den Rändern sitzen** und das Rechteck festhalten:

| `RE15_KERN_ANTEIL` | Überlappung | getrennte Nachbarn |
|---|---|---|
| 1.0 (aus) | 12,2 % | 12 |
| 0.92 | 12,1 % | 18 |
| 0.85 | 9,8 % | 20 |

Der Code bleibt als `RE15_KERN_ANTEIL` erhalten (Default 1.0 = aus). Der eigentliche
Hebel gegen die Überlappung war die Wandfreiheit, nicht die Rechteckgröße.

## §13 — Nacht-Audit 2026-09-03: jeder Raum gegen fünfzehn Invarianten

> „ROOM 1070 wird nicht rot hervorgehoben wenn ich drin stehe."
> „Nimm dir die Nacht, überprüfe jeden einzelnen Raum auf Fehler und nimm noch Korrekturen
> zur perfekten Verfeinerung vor."

Neues Werkzeug `re15_port/tools/karte_audit.py` — prüft alle 96 Räume, nennt jeden Fall
namentlich (kein Aggregat, das Einzelfälle zudeckt).

### ROOM1070: verschachtelte Zonen

`zones_of()` bildet Zusammenhangskomponenten der Kollisionszellen. Innere Zellen (Pfeiler,
Möbel) liegen oft weiter als `GAP` von der Umfassungswand und werden dadurch zu **eigenen
Zonen** — deren Weltbox steckt aber **vollständig** in der des Raums. `zone_index_at()`
wählt die *kleinste* Zone, die den Spieler enthält, also die Insel; ihr Rechteck (ROOM1070
z1: 11×5 px) wird rot gefärbt und liegt komplett **unter** dem grünen Rechteck der
umgebenden Zone. Der Raum sah nie rot aus.
Betroffen: ROOM1020, ROOM1070, ROOM1110, ROOM1140, ROOM11F0, ROOM30C0 → 126 auf 117 Orte.

### Treppensymbole — zwei getrennte Ursachen

* **ROOM2080**: die Treppe liegt in der **Lücke zwischen** zwei Zonen (x = −19550; Zone 0
  ab −17650, Zone 1 bis −21450). `zone_at()` verlangte strikte Enthaltung → `None` → die
  Marke fiel **still** weg. Die Engine hat dort einen Rückfall auf die nächste Zone; der
  Generator bekommt denselben, sonst zeichnen beide verschiedene Karten.
* **ROOM30D0**: das Symbol *existierte* (Blatt 7, 160,112) — aber ROOM30C0 trug an
  derselben Stelle auch eines ein, und die Dublettenprüfung behielt ROOM30C0s Marke
  **samt Sichtbarkeits-Zone**. Das Symbol hing am falschen Raum. Jetzt: eine Marke, zwei
  Zonen (`zid`/`zid2`).

### Fünf Türen, ein Symbol (ROOM1210 ↔ ROOM1220)

Der Korridor hat **fünf** echte Türen. Ihre Weltkoordinaten sind auf beiden Seiten fast
gleich ((−21450,−6100) gegen (−21750,−6400) …), die Projektionen liefen aber
**gegenläufig** — mein Mittelwert je Paar ergab fünfmal denselben Wert, alle fünf Marken
fielen auf **einen** Punkt (164,100). Live: sechs Übergänge mit 9–21 px Sprung.

Gemittelt wird jetzt nur bei Übereinstimmung (≤ 4 px, gemessen an ROOM1000↔ROOM1050:
144/143, 164/164, 167/167). Weicht es ab, ist das ein **Signal** für gegenläufige Lage.

Dazu: `anheften()` wählte die Lage nur nach geringster Überlappung und sah nur die *eine*
Kante, über die die Breitensuche kommt. Diagnosen `RE15_PAAR_PROBE` / `RE15_PAAR_DETAIL`
zeigen: eine Lage schafft **drei** der fünf, gewählt wurde eine mit **einer**. Jetzt
werden die Lagen aller Kanten des Paares gesammelt und nach miterfüllten Kanten sortiert.
⚠️ Der Korridor ist damit **nicht** gelöst — `beste_lage` probiert jede Wurzel, und die
Gesamtwertung wählt weiterhin eine Lage mit einem Treffer. Offen.

### Einseitige Türen zählten nicht

ROOM5050→ROOM5120 und ROOM5060→ROOM5120 haben **keinen Gegen-Datensatz** und landeten in
`notkanten` — die dienten nur zum Anbauen und kamen im Gütemaß nicht vor. Nichts drückte
die Räume zusammen. Jetzt zählen sie im **Getrennt**-Maß mit.

### Stand

| | vorher | jetzt |
|---|---|---|
| Audit-Fehler | 11 | **2** |
| schlimmster Sprung | 31 px | **22 px** |
| ≤ 2 px | 47 | **52 (35 %)** |
| ≤ 4 px | 98 | **101 (69 %)** |
| gepaarte Marken neben der Kante | 4 | **0** |
| Blätter mit starker Überlappung | 3 | **2** |

**Offen:** ROOM5060↔ROOM5120 (2 px), ein Symbolpaar auf Blatt 6, der Korridor
ROOM1210↔ROOM1220, Überlappung auf Blatt 7 (31 %) und 9 (35 %).

⚠️ Die Prüfung **D2 war zunächst ein Fehlalarm** — sie zählte alle Symbole zwischen zwei
Räumen zusammen, obwohl ROOM1000↔ROOM1050 drei *echte* Türen hat. Ein Prüfer, der falsch
anschlägt, ist wertlos; korrigiert auf Symbole je **Durchgang**.

**Gemessen und nicht übernommen:** mehr Reparaturrunden (6/20/40 identisch — der
Bergsteiger steht im lokalen Optimum), die Kriterien-Reihenfolge „Deckung zuerst"
(91/97 statt 90/97, dafür 17 statt 16 getrennt — im Rauschen), und das Stutzen des
Rechtecks auf den begehbaren Kern (§12).
