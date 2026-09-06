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

## §14 — Live-Prüfung: jeder Raum angesprungen, jede Tür durchschritten

> „überprüfe jeden einzelnen Raum auf Fehler mit dem Hinspringen und dem Debugger … gehe
> auch gerne einmal durch die Türen durch, und schaue dir dann an ob das Verhalten der
> Erwartung entspricht."

Neuer Test `integration_map_raum_live`. Er springt in **jeden der 96 Räume** (dieselbe
Kette wie der Debug-JUMP: `scd_room_reenter`), setzt den Spieler auf den
**Ankunftspunkt samt Band** der Tür, die dorthin führt, öffnet den Kartenschirm und baut
die **echte** Op-Liste mit `re15_inv_screen_build`.

⛔ Geprüft wird das **gerasterte Bild**, nicht die Tabelle. Die Op-Liste wird von hinten
gerastert — für jeden Punkt gewinnt der kleinste Index.

### Was nur die Live-Prüfung finden konnte

**13 Räume zeigten keine sichtbare rote Hervorhebung**, weil ein *Nachbarraum* darüber
lag: ROOM1060, ROOM1080, ROOM10A0, ROOM1200, ROOM2060, ROOM2070, ROOM3090, ROOM30D0,
ROOM30E0, ROOM5110, ROOM5120, ROOM5140, ROOM6030. Das statische Audit hatte nur die
Überdeckung durch eine andere Zone *desselben* Raums gefunden (ROOM1070) — die Überdeckung
durch fremde Räume entsteht überall dort, wo sich Rechtecke überlappen (Blatt 7 und 9 über
30 %).
**Behoben:** der aktuelle Raum wird in einem *ersten* Durchgang gezeichnet und liegt damit
oben. 13 → 0.

**ROOM2080 blieb blau, obwohl der Spieler drin stand.** Der Zustands-Block sammelte nur
Zonen mit *derselben Zeichnung* (`zg->synth != zn2->synth`). ROOM2080 hat zwei Zonen mit
zwei Zeichnungen (9×18 und 4×4 px) — stand der Spieler in der kleinen, blieb die große
blau.
**Behoben:** rot wird jede *besuchte* Zeichnung des Raums, in dem der Spieler steht; das
Besucht-Bit bleibt pro Zeichnung.

### Durch die Türen gegangen

**199 Türen durchschritten — 199 mal war vorher der Ausgangs- und nachher der Zielraum
sichtbar rot.**

| | vorher | jetzt |
|---|---|---|
| Räume mit sichtbarem Rot | 83 von 96 | **96 von 96** |
| Marker in der roten Fläche | — | **95 von 96** |
| Türdurchgänge mit korrekter Hervorhebung | — | **199 von 199** |

Offen: der Fahrstuhl ROOM1080 (auf drei Blättern als Gast gezeichnet) — dort landet der
Marker auf der Kopie eines anderen Blattes.

### ⛔ Zwei eigene Messfehler, die erst behoben werden mussten

* `oben_an()` zählte die kleinen **Marken** (Türbalken 5×2) als Überdeckung — die gehören
  per Konstruktion nach oben. ROOM10A0 fiel dadurch fälschlich durch.
* Das **Band** kam aus der größten Kollisionszelle statt von der Tür, durch die man
  *ankommt* (`Door_aot_set` pc[4]). Davon hängt das gezeigte Blatt und das Etagen-Bit ab;
  ROOM2070 wurde deshalb nie rot.

### ⛔ Keine Zusicherung auf den Sprung in dieser Phase

Sie läuft über **alle** AOT-Slots, auch über doppelt belegte mit veraltetem Spawn, und
misst damit eine *andere* Menge als `integration_map_uebergang` (das auf Türen mit
reziprokem Gegen-Datensatz filtert). Für ROOM1140 → ROOM1130 meldet sie 48 px,
`map_uebergang` 13 px — dieselbe Tür, zwei Mengen. Der Sprung bleibt dort; hier ist er
Diagnose (`RE15_SPRUNG_LISTE=1`). Eine Zahl, deren Abweichung ich nicht erklären kann,
gehört nicht in eine Schranke.

## §15 — Optische Lösungen für die letzten offenen Punkte

> „Überlege dir was, was optisch passt, um die letzten offenen Probleme zu lösen."

### ✅ Alle Wandlinien über alle Füllungen

Je Raum wurde erst die Wandlinie und dann die Füllung ausgegeben. Die Op-Liste wird von
hinten gerastert — früher eingetragen heißt **oben**. Damit liegt die eigene Linie zwar
über der eigenen Füllung, die Füllung des *zuerst* gezeichneten Raums aber über der
**Linie aller späteren**. Wo sich zwei Rechtecke überlappen (Blatt 7: 31 %, Blatt 9: 35 %)
verschluckte die Füllung des einen die Umrandung des anderen — die beiden Räume
verschmolzen optisch zu **einer** Fläche.

Neu in vier Durchgängen, von oben nach unten:

| Durchgang | Inhalt |
|---|---|
| 0 | Wandlinie des **aktuellen** Raums |
| 1 | Wandlinien aller übrigen |
| 2 | Füllung des **aktuellen** Raums |
| 3 | Füllungen aller übrigen |

Das löst die Überlappung nicht geometrisch — es macht sie **lesbar**, und darum geht es
auf einer Schema-Karte.

### ⛔ Verworfen: die Tür zusätzlich auf dem eigenen Blatt ihres Ortes

12 von 244 Türen zeigen beim Davorstehen kein Symbol, weil die Marke auf dem Blatt des
**Bandes** liegt, der Spieler aber das Blatt seines **Ortes** sieht (ROOM1170 →
ROOM1130/1140: Marke auf Blatt 4, Spieler auf Blatt 5). Die Tür zusätzlich auf dem eigenen
Blatt zu zeichnen brachte **+2** sichtbare Symbole (232 → 234) — und verletzte eine
ausdrückliche, aus einem Nutzer-Report abgeleitete Anforderung: der Pin
`unit_map_mark_band` hält fest, dass das ROOF-Blatt **keine** Tür des unteren Bereichs von
ROOM1170 trägt. Dazu kam ein weiteres Paar Symbole übereinander.
**Zurückgenommen** — zwei sichtbare Symbole wiegen eine gesetzte Anforderung nicht auf.

### ⛔ Verworfen: das Kartenfeld auf Original-Größe

Die ausgelieferte Kunst bespielt x[39..293] = **254 px** und y[44..206] = 162 px
(Rechteck-Listen @0x800768 44 + 8·Seite); unser Feld ist mit 132×140 nur etwa **halb so
breit**. Die Karte auf Original-Größe zu bringen liegt nahe — und macht sie **messbar
schlechter**:

| Feld | Maßstab | Überlappung | ≤ 4 px |
|---|---|---|---|
| 100,55,**132**,140 | 919 u/px | 10,7 % | **69 %** |
| 120,52,**160**,142 | 846 u/px | 10,4 % | 63 % |
| 46,50,**232**,145 | 793 u/px | 10,4 % | 58 % |

Die Überlappung sinkt **nicht** — der einzige Effekt ist, dass der feinere Maßstab jede
vorhandene Türabweichung in mehr Pixel übersetzt (und die Anordnungen sich mitverschieben:
der Verlust ist größer als der reine Maßstabseffekt von 1,16×). Der Nutzer hat wiederholt
**Sprünge** bemängelt, nie eine zu kleine Karte.
`RE15_FELD=x,y,w,h` lässt es jederzeit nachmessen; die Messung steht als Kommentar an der
Konstanten, damit die Idee nicht ungeprüft wiederkehrt.

### Stand

266/266 Tests grün · ≤ 4 px 101 (69 %) · 96/96 Räume sichtbar rot · 199/199 Türdurchgänge
korrekt hervorgehoben · 232/244 Türen mit sichtbarem Symbol · Audit 2 Fehler.

## §16 — Der offene Ansatzpunkt: das falsche Band-Feld

Aus §15 blieb offen: bei ROOM1170 liegt die Türmarke 17 px vom Marker entfernt, obwohl der
Symbol-Zug greifen müsste.

Eine Diagnose in `tuer_anziehen()`, die **Spielerband und Türbyte nebeneinander** druckt,
zeigte sofort: *„Band 0 (Byte 4)"*.

### Ursache: zwei Felder mit demselben Namen

| Feld | Inhalt |
|---|---|
| `re15_aot_t.band` | `Aot_set`-Operand `chain` pc[4] — bei **Treppen** gefüllt, bei Türen **0** |
| `re15_aot_door_params_t.band` | das **Band der Tür** (`Door_aot_set` pc[4] = obj[0x82]) |

Beide Messschienen lasen das **Slot**-Feld. Damit liefen alle bandabhängigen Messungen mit
Band 0 — die Aussage „das Blatt wird über das Band gewählt" war nie geprüft.

### Wirkung der Korrektur (die Karte ist unverändert — nur die Messung war falsch)

| | vorher | jetzt |
|---|---|---|
| gemessene Übergänge | 146 auf 11 Blättern | **153 auf 12 Blättern** (9 auf Blatt 4) |
| ≤ 8 px | 123 (84 %) | **129 (84 %)** |
| Türen mit sichtbarem Symbol | 232 von 244 | **237 von 244 (97 %)** |

### Die verbleibenden sieben, charakterisiert

* **1×** ROOM1170 → ROOM1240: der Zielraum ist überhaupt nicht gezeichnet — dort *kann*
  kein Symbol sein.
* **4×** über Stage-/Blattgrenzen ohne gepaarte Marke (ROOM3080 → ROOM4000,
  ROOM4020 → ROOM4000 / ROOM5000 / ROOM50C0).
* **2×** ROOM3020 und ROOM40A0: eine Marke existiert, der Zug reicht nicht heran.

### Nebenbefund: Band-Byte ≠ `band_from_y(spawn_y)` bei 29 % der Türen

`aot_common.c:561` schreibt beim Durchschreiten `band_from_y(d->spawn_y)` ins
Kollisionsband. Gemessen weichen die zwei Größen bei **80 von 273** Türen ab. Das ist
**kein Fehler**: das Byte beschreibt die Etage des Tür-Objekts, die Spawn-Höhe die Höhe im
Zielraum (meist 0, weil der Boden dort auf y = 0 liegt). Für die Kartenseite ist das Byte
die richtige Größe. `read_rdt` nimmt jetzt auch `ny` auf, damit es nachmessbar bleibt.

### ⛔ Diagnose wieder entfernt

Die Zug-Diagnose war ein `getenv()` + `printf` **pro Bild** im Kartenpfad. Für die
Fehlersuche richtig, im Auslieferungscode nicht — erst recht mit Blick auf den kommenden
PSX-Port (siehe Memory `reai-v2-psx-laufzeitbudget`).

---

## §17 Varianten-Räume: zwei Skriptzustände, ein Ort (2026-09-03)

Blatt 9 hatte **37,6 %** Überlappung — mit Abstand der schlechteste Wert nach Blatt 7.
Die Ursache ist kein Löser-Fehler: **ROOM5030 und ROOM5110 sind derselbe Raum.**

```
ROOM5030: 75 Zellen   Tür (-26100,-15400) -> ROOM5040
                      Tür (-15400, -4000) -> ROOM5000
                      Tür (-15300,-26300) -> ROOM50A0
                      Tür ( -4500,-15400) -> ROOM5070
ROOM5110: 75 Zellen   Tür (-26100,-15400) -> ROOM5120
                      Tür (-15400, -4000) -> ROOM5000
                      Tür (-15300,-26300) -> ROOM5140
                      Tür ( -4500,-15400) -> ROOM5130
```

Gleiche Zellzahl, **jede Tür auf derselben Weltkoordinate**, nur die Ziele unterscheiden
sich paarweise. Der Löser legte beide Zustände getrennt an, und sie landeten übereinander.

### Die Regel hängt am Beleg, nicht am Blatt

1. **Keim** — zwei Räume, die an *einem* Türschlitz (gleicher Trigger *und* gleicher
   Spawn) als zwei Ziele auftreten, **und** dieselbe Zellzahl haben, **und** alle Türen
   auf identischen Weltkoordinaten tragen. Behalten wird der, dessen äußere
   Kollisionsbox die andere enthält.
2. **Fortsetzung** — an jedem geteilten Schlitz eines erkannten Paares sind auch die
   beiden *Ziele* ein Paar. Aus ROOM5030/ROOM5110 folgen so ROOM5040/ROOM5120,
   ROOM50A0/ROOM5140 und ROOM5070/ROOM5130.

Game-weit greift das auf **vier** Räume — alle auf Blatt 9, ohne dass eine Blattnummer im
Code steht.

### Warum der schwächere Keim verworfen wurde

Ohne die beiden Zusatzbedingungen findet dieselbe Suche fünf weitere Paare auf den
Blättern 0/1/7. Der Löser-Proxy bewertet die als **besser** (Überlappung im Mittel 8,0 %
statt 8,8 %) — live fällt die Karte auseinander:

| Maß (live) | Basis | nur Beleg | schwacher Keim |
|---|---|---|---|
| schlimmster Sprung | 22 px | **23 px** | **164 px** |
| Räume sichtbar rot | 96/96 | **96/96** | 94/95 |
| Türdurchtritte korrekt | 199/199 | **199/199** | 191/194 |
| Türsymbole sichtbar | 237/244 | **237/244** | 233/243 |

Wieder derselbe Fall wie in `reai-v2-proxy-mass`: **die Zahl wurde besser, das Ziel
schlechter.**

### Ergebnis

| Maß | vorher | nachher |
|---|---|---|
| Überlappung Blatt 9 | 37,6 % | **3,0 %** |
| getrennte Nachbarn Blatt 9 | 3 | **1** |
| Schema-Zeichnungen gesamt | 117 | **114** |
| Audit-Fehler | 2 | **1** |
| Übergänge ≤ 8 px | 129/153 | 129/153 |

Beide Zustände teilen **Zonennummer und Zeichnung** (ein Besucht-Bit, ein Rechteck,
drei Zeichen-Operationen weniger je Bild — relevant für den PSX-Port), behalten aber ihre
eigene Weltabbildung, damit der Spieler-Marker in beiden Zuständen richtig sitzt.

### Zwei Nebenwirkungen, die erst die Prüfungen zeigten

* **Audit D2** meldete „2 Symbole für 1 Durchgang, bis 5 px auseinander": der
  Marken-Durchlauf läuft über *alle* Räume, also auch über den zusammengelegten Zustand,
  dessen eigene Weltabbildung die Marke danebensetzt. Ein Varianten-Raum steuert jetzt nur
  noch die Durchgänge bei, die sein Zwilling **nicht** hat — ROOM50A0s Tür nach ROOM4070
  bleibt, alle anderen fallen weg.
* **Audit K** zählte Zeilen statt Zeichnungen und meldete für Blatt 9 weiter 44 %,
  während der Generator 3,0 % rechnete und auf dem Schirm *ein* Rechteck liegt. K zählt
  jetzt je Zeichnungs-Index einmal. ⛔ Zwei Maße, die auseinanderlaufen, sind ein Befund —
  nicht ein Grund, sich das freundlichere auszusuchen.

## §18 Türsymbole: Identität statt Nähe

Phase 3 von `integration_map_raum_live` zählte eine Tür als geprüft, wenn *irgendein*
gelber Balken im Umkreis von 4 px lag. Das bestätigt sich selbst — liegen zwei Durchgänge
nah beieinander, zählt der Balken des Nachbarn genauso. Gemessen wird jetzt, wie die
Engine die Marke tatsächlich auflöst (`tuer_anziehen`, `re15_inv_screen.c`):

```
244 Türen: Identität 189 | Rückfall 41 | Zufall 7 | ohne Marke 7
```

* **Identität 189** — es gibt eine Marke, die meine Zone mit einer Zone des *Zielraums*
  verbindet, und ihr Balken ist im Bild. Die Schranke hängt an dieser Zahl (≥ 3/4).
* **Rückfall 41** — keine solche Marke; es bleibt die ungepaarte Marke der eigenen Zone
  (der Nachbar liegt auf einem anderen Blatt). Kein Defekt.
* **Zufall 7** — genau die Fälle, die das alte Maß als Erfolg gezählt hat.
* **ohne Marke 7** — u. a. beide Fahrstuhl-Enden (ROOM4020 z70, ROOM1080 z23) tragen
  überhaupt keine Marke.

⛔ Ein Null-Rechteck-Filter wurde geprüft und **verworfen**: er macht die Fahrstühle an
*beiden* Enden unmessbar, statt den Befund zu zeigen.

## §19 Release-Prüfung v0.3.97 — drei Defekte, die erst am fertigen Paket auffielen

Nach dem Neubau beider Plattformen wurden die Pakete in sechs unabhängigen Dimensionen
geprüft und jeder Befund adversarisch gegengeprüft (23 bestätigt, 10 widerlegt). Drei
Defekte waren echt und sind behoben.

### 1. `short wz1` lief über — vier Weltboxen standen verkehrt herum

`re15_map_zone_t` trug die Weltbox als `short`. ROOM1180 und ROOM1230 haben
`wz1 = 32871`; im Binary stand **−32665**, also `wz1 < wz0` — die Box war invertiert.

Folgenlos blieb das nur durch Zufall: beide Räume haben genau **eine** Zone, deshalb griff
der Nächstgelegen-Rückfall (`re15_map_zones.c:101`) und lieferte dieselbe Zone. Bekäme
einer dieser Räume je einen zweiten Bereich, verlöre der Marker dort seine Zone.

Behoben durch `int32_t` (alle Verbraucher casteten ohnehin schon auf `int32_t`). Riegel:
`unit_map_synth` prüft für jede der 234 Zeilen `wx1 > wx0 && wz1 > wz0`. ⛔ Der Riegel
wurde **gegen die alte Feldbreite gegengeprüft** — mit `short` meldet er die vier Zeilen
namentlich und fällt; ein Riegel, von dem ich nicht weiß, dass er auslöst, ist keiner.

### 2. Das Linux-Archiv lieferte `re15_pc` ohne Ausführungsbit

Im ZIP stand Modus **0644**. Wer auf dem Deck entpackt und das Binary direkt startet — oder
es direkt als Nicht-Steam-Spiel einträgt, statt `run.sh` zu nehmen — bekam
`Permission denied`.

Die Ursache liegt im **Bauhost**, nicht im Paketbauer: das Arbeitsverzeichnis liegt auf
einem Windows-Dateisystem, das MSYS ohne ACLs einbindet. Dort ist `chmod` wirkungslos, und
MSYS leitet das Bit stattdessen aus Shebang bzw. Endung ab — `run.sh` kommt deshalb
zufällig richtig heraus (`#!`), ein ELF-Binary nie. `install -m 755` lief still ins Leere.

⛔ Genau diese Kombination macht den Fehler unsichtbar: die eine Datei, die man prüft
(`run.sh`), stimmt, und erweckt den Eindruck, die Modi stimmten insgesamt.

Behoben in `release/zip_exec_bit.py`: der Modus steht nicht im lokalen Kopf, sondern in den
oberen 16 Bit von `external_attr` im **Zentralverzeichnis**, das bei Split-Archiven
vollständig im letzten Volume (der `.zip`) liegt. Dort wird gesetzt — und anschließend
**zurückgelesen**; ohne dieses Gate kehrt der Fehler beim nächsten Bau still wieder.

### 3. Das Laufzeit-Gate lief für Linux nie

`make_package.sh` überspringt die Asset-Herkunftsprüfung, wenn das Binary kein
Windows-Binary ist — auf einem Windows-Host also **immer**. Das Linux-Paket war damit das
einzige Artefakt ohne Herkunftsnachweis, genau die Klasse Fehler, die schon einmal
durchgerutscht ist ([[reai-v2-asset-root-exe-anchor]]).

Der Nachweis wurde im Container nachgeholt und ist **strenger** als der Windows-Lauf:
Paket read-only gemountet, Repo nachweislich nicht erreichbar (`find / -xdev` findet weder
`reAi_v2` noch `shared_assets` außerhalb `/pkg`), keine `RE15_*`-Variable gesetzt. Alle
drei Läufe (cwd im Paket, cwd fremd, realer `run.sh`-Weg): `RESULT ok=26 missing=0`, alle
26 Treffer aus `/pkg`.

⛔ Und mit **Negativ-Kontrolle**, damit das Grün nicht leer ist: dasselbe Binary mit
leerem Paket und erreichbarem Build-Pfad meldet ebenfalls `ok=26 missing=0` — aus
`/src/re15_port`. Der einkompilierte Rückfall ist also lebendig; nur die Herkunftsspalte
trennt echt von Täuschung. Eine reine „26/26 gefunden"-Meldung wäre wertlos gewesen.

### Nicht behoben, aber gemessen

* **50 der 90 ausgelieferten Sprachdateien sind unversioniert** — sie liegen nur im
  Arbeitsbaum (u. a. alle Stimmen von ROOM11B0 und ROOM11C0). Das Paket ist aus dem Repo
  nicht reproduzierbar. Code und `shared_assets` sind dagegen sauber (0 unversioniert).
* **Der `.MSK`-Ladepfad hat null Testabdeckung** — keine der 266 Prüfungen lädt eine
  Maske. Die 266/266 sagen über die neuen Vordergrund-Verdeckungen nichts aus.
* **ROOM5070/ROOM5130** stehen in der ausgelieferten Zonentabelle gar nicht (beide werden
  nicht gezeichnet). Von den vier zusammengelegten Paaren sind also nur **drei** auf der
  Karte sichtbar; die vierte Zusammenlegung ist im Auslieferungsstand nicht messbar.

---

## §20 ROOM1170 steckt in ROOM1130 — die Spieldaten erzwingen es

Nutzer 2026-09-03 (`error1.png`): *„Bei der Map gibt es zum Teil neue Probleme wie das
Room 1170 das rechteck jetzt in room 1130 quasi drin steckt. Das war vorher nicht so."*

### Wann es entstand — nicht in v0.3.96/97

| Fassung | Überschneidung ROOM1130 / ROOM1170 |
|---|---|
| v0.3.90 | 0 % |
| v0.3.91 | 3 % |
| **v0.3.92** | **23 %** ← hier entstanden |
| v0.3.93 … v0.3.97 / HEAD | 21 % (20 × 7 px), **Geometrie unverändert** |

Die Rechtecke auf Blatt 4 sind zwischen v0.3.95 und HEAD **byte-gleich**. Auslöser war
v0.3.92 („Wandwahl als Freiheitsgrad", `WAND_RAND = 4`), nicht die Karten-Arbeit danach.

### Warum es unvermeidbar ist

ROOM1170 Zone 1 (die Gast-Zeile auf Blatt 4, `etage = 1`; ihr Zuhause ist Blatt 5) hat
ihre beiden Ausgänge auf **gegenüberliegenden** Wänden:

```
Weltbox Zone 1:  x −28900 … −8784   z −28841 … −17070
   Tür (−21230, −17925) → ROOM1130     855 vor der z-Max-Wand
   Tür (−21600, −27005) → ROOM1140    1836 vor der z-Min-Wand
```

ROOM1130 und ROOM1140 liegen auf der Karte **direkt aufeinander** (1130 y 111…135,
1140 y 134…179). Ein Rechteck, dessen zwei Türen auf gegenüberliegenden Wänden sitzen,
kann nicht an zwei Räume anschließen, die selbst aneinandergrenzen — ohne einen davon zu
überlappen. Das ist Geometrie, kein Löser-Fehler.

Die erschöpfende Suche (`RE15_VOLLSUCHE=4`, jetzt auch mit Versenkungs-Bewertung) bestätigt
es und beziffert den Tausch:

```
8 Orte, 6 Kanten -> hoechstens 5 gleichzeitig erfuellbar
beste Lage: 5 Kanten, dabei 1 Paare, die tiefer als 4 px ineinanderstecken
beste Lage OHNE Versenkung: 4 Kanten (= der Tausch kostet 1 Tuer(en))
```

### Neues Maß: EINSINKTIEFE statt Flächenanteil

Das Gütemaß kannte nur „Überlappung in % **aller gezeichneten Pixel**" — 140 px auf einem
ganzen Blatt sind 3 %, während ein Rechteck sichtbar im anderen steckt. ⛔ Wieder ein
Aggregat, das den Einzelfall nicht sehen kann. Gemessen wird jetzt die **Eindringtiefe je
Paar**, `min(Überschneidung in x, in y)`:

* game-weit 142 überlappende Paare, **Median 1 px** — das ist die gemeinsame Wand, korrekt
* **23 Paare** stecken tiefer als `WAND_RAND` (4 px) ineinander, das tiefste 24 px
* ROOM1130 / ROOM1170 ist eines davon, mit 7 px

`kosten()` liefert die Zahl als siebtes Kriterium, `ORDNUNG()` stellt sie **hinter** die
Türdeckung, die Generator-Ausgabe meldet sie je Blatt als `N VERSENKT`.

### ⛔ Zwei naheliegende Verschärfungen — gemessen und verworfen

**1. Einsinktiefe GANZ nach vorn.** Im Aggregat deutlich besser: Übergänge ≤ 2 px 50 → 55,
über 8 px 24 → 22, Türsymbole per Identität 189 → 191 / per Zufall 7 → 4, tiefe Paare
23 → 18, tiefstes 24 → 19 px, Blatt 7 von 37,8 auf 27,2 % Überlappung. **Aber:** auf Blatt 1
rücken zwei verschiedene Durchgänge (ROOM10A0↔ROOM11E0 und ROOM11E0↔ROOM1200) auf 2 px
zusammen — an einer Ecke, in der schon sechs Marken in einem 6×6-Feld stehen — und der Pin
`unit_map_durchgang` fällt. Ein Symbol, das ein anderes verdeckt, ist genau die Klasse
Fehler, die der Nutzer zweimal gemeldet hat; das Aggregat wiegt sie nicht auf. Erreichbar
über `RE15_ORDNUNG=optik`; wer es weiterverfolgt, muss zuerst die Marken-Trennung an dieser
Ecke lösen.

**2. Den Bergsteiger auch an SITZENDEN Türen nachbessern lassen.** Klingt gratis — der Zug
wird ja nur übernommen, wenn `ORDNUNG()` besser wird. Ist es nicht: er landet in einem
anderen lokalen Optimum, tiefe Paare 18 → 19, und ROOM11E0/ROOM1230 auf Blatt 1 sinkt von
16 auf **44 px** ineinander, während Blatt 4 sich um genau 1 px bewegt. **Mehr Suche ist
nicht dasselbe wie bessere Suche.**

### Stand

Blatt 4 bleibt wie ausgeliefert (5 von 6 Türen, 1 versenktes Paar). Live gegenüber v0.3.97:
Übergänge über 8 px 24 → **22**, ≤ 8 px 129 → **131**, ≤ 2 px 50 → 49; Symbole, rote
Hervorhebung und Durchtritte unverändert; 267/267 Tests, Audit 1 Fehler.

### §20a Nachtrag: die Überschneidung war NICHT erzwungen — ich hatte falsch gemessen

Der Nutzer hat widersprochen: *„Aber vorher ging doch beides Türen inkl. nicht ineinander
steckenden Rechteck auf der Etage."* Die Erinnerung stimmte so nicht — in v0.3.90 fehlte
das Symbol zur ROOM1130-Tür, in v0.3.91 das zur ROOM1140-Tür, es ging also nie beides —
**aber der Einwand hat einen echten Fehler in meiner Messung aufgedeckt.**

⛔ **Die Vollsuche misst mit einem ANDEREN Kantentest als der Löser.** Sie prüft den
Abstand der Wandpunkte, die Marken hängen aber an `kantenrest` (gemessen gegen die
Berührung der beiden Rechtecke). Ihre Aussage „höchstens 5 von 6, mindestens 1 versenkt"
trägt für diese Frage deshalb nicht — ich hatte sie trotzdem als Beweis zitiert.

Neue Sonde `RE15_ORT_PROBE=<blatt>:<ROOMxxxx>:<zone>` zählt jede Lage eines Ortes auf und
bewertet sie mit dem **richtigen** Maß, dazu die Eindringtiefe in jeden Nachbarn:

```
heute        Tuer->1130 4 px, Tuer->1140  0 px | steckt 8 px in ROOM1130 | getrennt 0, deckung 5
x-4 y+5      Tuer->1130 0 px, Tuer->1140  5 px | steckt in NICHTS        | getrennt 0, deckung 5
an ROOM1130  Tuer->1130 0 px, Tuer->1140 17 px | steckt in NICHTS        | getrennt 1, deckung 5
```

Die Ursache: `anlegen()` erzeugt **nur** Lagen, in denen eine Tür *exakt* auf der Wand des
Nachbarn sitzt. Ein Versatz von vier, fünf Pixeln kommt darin nicht vor — und genau der
löst den Fall.

### Der Nachlauf `Blatt.entsenken()`

Schiebt ein Rechteck, das tiefer als `WAND_RAND` steckt, um bis zu 10 px. ⛔ Die
Annahme-Regel ist streng, sonst wiederholt sich die alte Entzerrung, die die Überlappung
minimierte und dabei die berührenden Durchgänge zerstörte ([[reai-v2-proxy-mass]]):
`getrennt` darf **nicht** steigen, `deckung` **nicht** fallen, `versenkt` **muss** sinken,
und die Blattgrenzen müssen exakt gleich bleiben.

**Drei eigene Fehler auf dem Weg, alle durch Messung entlarvt:**

1. Der Nachlauf lief in `beste_lage()` **je Wurzel** — das ~16-fache; der Generator lief in
   10 Minuten nicht durch. Er gehört einmal auf die Siegerlage.
2. Er verschob den **Blatt-Ursprung**; das Einpassen rundete daraufhin *jedes* Rechteck
   neu, und ROOM2080 z1 auf Blatt 6 fiel von 4×4 auf 4×3 px (Audit-Prüfung B2). Zwei
   Riegel: die Blattgrenzen müssen gleich bleiben, und die **Anzeige** wird auf mindestens
   4 px Kante geklemmt (die Abbildung bleibt unberührt, der Marker rechnet weiter exakt).
3. Er nahm unter den Zügen mit `versenkt == 0` den **kürzesten** und blieb genau auf der
   Schwelle stehen (4 px). ⛔ Die Schwelle ist die Grenze des *Zählens*, nicht das Ziel —
   jetzt entscheidet die Tiefe.

### Ergebnis

ROOM1170 z1: `x181..201 y128..161` → `x178..198 y131..164`. Statt 7 px in ROOM1130 zu
stecken und ROOM1140 nur mit 1 px zu berühren, teilt es jetzt mit **beiden** ein
4-px-Wandband (20×4 gegen ROOM1130, 4×30 gegen ROOM1140). `VERSENKT 1 → 0`, `getrennt 0`
und 5/6 deckungsgleiche Türen unverändert.

Live gegenüber v0.3.97: über 8 px 24 → **23**, ≤ 8 px 129 → **130**, ≤ 2 px 50 → 48,
≤ 4 px 103 → 101; Symbole 237/244, 96/96 rot, 199/199 Durchtritte unverändert.
267/267 Tests, Audit 1 Fehler.

---

## §21 Das Original hat KEIN Aufdecken — instruktions-verifiziert (2026-09-03)

Nutzer: *„was ist dein problem damit das so darzustellen, und nur die türen durch re 2
türen auszutauschen, und die treppen einzuzeichnen?"* — berechtigte Frage. Antwort in
zwei Teilen, beide gemessen.

### Teil 1: Darstellen ist kein Problem — und das Original deckt nichts auf

Blatt 4 (3F) setzt sich im Original aus **7 Rechtecken** zusammen, jedes mit einer
`u/v`-Quelle in `DATA/MAP05.PIX`; die Karte wird also stückweise aus der gemalten Textur
zusammengeblittet (Liste @0x80076468, Paar-Tabelle @0x80076840 + 8·Seite):

```
 0: x127 y137  16x16  u=168 v= 40     4: x144 y 80  32x80  u=  0 v= 32
 1: x136 y137  24x24  u=168 v= 16     5: x120 y119  40x40  u=128 v= 16
 2: x120 y 80  32x40  u= 96 v= 48     6: x160 y110  56x56  u= 32 v= 32
 3: x152 y 89  48x24  u=192 v= 16
```

⛔ **Ein Kommentar im Port behauptete, das Original zeichne ein Rechteck nur bei gesetztem
Besucht-Bit. Das ist falsch, und zwar instruktions-verifiziert:**

| Stelle | einzige Verzweigung im Rumpf |
|---|---|
| Aufbau `FUN_80046fd8` @0x800472fc–0x800473e0 | `bne v0,zero,LAB_800472fc` @0x800473dc — Zähler `a2 < count` |
| Zeichner `FUN_800473f8` @0x800475f8–0x8004761c | `jal AddPrim` @0x80047608, `bne` @0x80047618 — Zähler `s0 < s2` |

Kein Zustands-Test, kein Raum-Nachschlag. Der Aufbau legt für **jedes** Rechteck der Seite
zwei SPRTs an (Doppelpuffer, daher `count + 2` Primitive im Zeichner), der Zeichner hängt
sie unbesehen ein. **Das RE1.5-Original zeigt beim ersten Kartenaufruf das ganze
Stockwerk.** Das schrittweise Aufdecken ist eine Port-Ergänzung (RE2-Kartensystem,
Nutzer-Auftrag 2026-08-30) — gewollt, aber ohne Original-Beleg.

Folge für den Vorschlag des Nutzers: er wird dadurch **einfacher**. Die Kunst ist statisch,
es braucht keinen Zustand je Rechteck und keine Raum-Zuordnung.

### Teil 2: Das eigentliche Loch ist der Spieler-Marker

Die Abbildung Welt → Kunst steht in den Maßstabszeilen `@0x800768b0` (Schrittweite 8,
`{u16 x_off, y_off, x_scale, z_scale}`, Index = globaler Raum-Slot). Gezählt:

| | |
|---|---|
| game-weit | **39 geeicht, 67 Stub** (Maßstab 1) |
| auf 3F | **2 von 8 Räumen** |

```
ROOM1150 (Slot 21)  Versatz (111,130)  Massstab (2296,2312)
ROOM1170 (Slot 23)  Versatz (100,206)  Massstab (2280,2268)
ROOM1060/1080/1120/1130/1140/1160      STUB
```

Ein Stub mit Maßstab 1 bildet in `FUN_800473f8` jede Weltposition auf denselben Punkt ab
(`((wx + 32000) · 10 · 1) >> 0x14` ist für den ganzen Wertebereich 0). In sechs der acht
3F-Räume weiß das Original also nicht, wo der Spieler auf seiner eigenen Karte steht —
das Kartensystem des Prototyps ist an dieser Stelle unfertig, siehe
[[reai-v2-karte-massstabstabelle]].

### Konsequenz für die Richtung

Die gerechneten Rechtecke waren die falsche Antwort auf ein Loch, das nur die
**Marker-Abbildung** betrifft. Richtig ist: Original-Kunst behalten, RE2-Türsymbole und
Treppen als eigene Ebene darüber, und von uns kommt allein die fehlende Abbildung für die
67 Stub-Räume — aus der Kollisionsgeometrie gegen das gemalte Rechteck bestimmt. Die
geeichten Zeilen sind dabei die Kontrolle: dort muss die Rechnung die Original-Zeile
treffen.

## §22 Die gemalten Rechtecke SIND die Kollisionsboxen — Verfahren bestätigt

Gegenprobe an den 38 geeichten Räumen: ihre Kollisionsbox durch die Original-Formel
(`FUN_800473f8` @0x8004741c-528) projiziert und gegen die gemalten Rechtecke ihrer Seite
gehalten. **30 von 38 landen zu ≥ 50 % auf einem gemalten Rechteck**, mehrere exakt:

| Raum | Kollisionsbox → Karte | gemaltes Rechteck | Deckung |
|---|---|---|---|
| ROOM3040 | x192..243 y45..108 | x191 y44 64×64 | **100 %** |
| ROOM5010 | x166..181 y84..107 | x164 y83 24×24 | **100 %** |
| ROOM5100 | x150..185 y129..178 | x150 y129 48×56 | **100 %** |
| ROOM50C0 | x120..161 y82..115 | x120 y80 48×40 | **100 %** |
| ROOM1150 | x118..150 y81..121 | x120 y80 32×40 | 91 % |

Die acht Ausreißer sind Räume mit **mehreren Zonen** (ROOM1170, ROOM11E0, ROOM30E0 …) —
dort deckt die äußere Bbox auch die Fläche *zwischen* den Bereichen ab. Kein Gegenbeleg;
die Messung muss dort je Zone laufen.

⇒ Die fehlenden 67 Zeilen lassen sich **umgekehrt bestimmen**: Kollisionsbox auf das
gemalte Rechteck legen, Maßstab und Versatz ablesen.

### 3F durchgerechnet

Maßstab aus ROOM1150s ausgelieferter Zeile (2296/2312 ≈ 457 Welteinheiten je Pixel).
Damit hat jeder Raum eine bekannte Größe in Kartenpixeln — die Zuordnung fällt aus der
Größe und wird von der **Türnachbarschaft** unabhängig bestätigt:

| Raum | Rechteck | Größe Raum → Kunst | Abstand |
|---|---|---|---|
| ROOM1150 | Nr2 | 31×40 → 32×40 | 1 (geeicht = Kontrolle) |
| ROOM1130 | Nr4 | 32×84 → 32×80 | 4 |
| ROOM1060 | Nr1 | 24×28 → 24×24 | 4 |
| ROOM1120 | Nr5 | 42×46 → 40×40 | 8 |
| ROOM1140 | Nr6 | 60×60 → 56×56 | 8 |

**Jede Tür dieser Zuordnung hat ihre beiden Rechtecke berührend:** ROOM1130↔ROOM1150 →
Nr4↔Nr2 (8×40), ROOM1130↔ROOM1120 → Nr4↔Nr5 (16×40), ROOM1120↔ROOM1060 → Nr5↔Nr1
(24×22), ROOM1130↔ROOM1140 → Nr4↔Nr6 (16×50). Größe und Türgraph sagen unabhängig
dasselbe — das ist der Beleg, nicht die Größe allein.

**Ohne Zuordnung bleiben:** die Rechtecke Nr0 (16×16) und Nr3 (48×24), und die Räume
ROOM1080, ROOM1160, ROOM1170. Die Original-Kunst von 3F malt also **nicht alle Räume** —
das Kartensystem des Prototyps ist auch hier unfertig, genau wie die Maßstabstabelle.

### Was daraus folgt

Für 3F sind fünf der acht Räume auf der Original-Kunst verortbar, inklusive Maßstab und
Versatz. Für die drei übrigen (und die zwei überzähligen Rechtecke) braucht es eine
Entscheidung: entweder bleiben sie auf der Karte unverortet wie im Original, oder sie
bekommen eine ergänzte Zeichnung. Das ist eine Nutzer-Entscheidung, keine RE-Frage.

Das ROT für den aktuellen Raum (RE2-Übernahme, vom Nutzer als gut bestätigt) hängt an
derselben Zuordnung: es färbt das gemalte Rechteck des Raums, in dem man steht.

## §23 Ergänzen auf 3F: es gibt nichts zu ergänzen

Nutzer: *„ergänze sie wenn das gutaussehend geht."* Gemessen — und die Antwort ist, dass
3F auf der Original-Kunst bereits **vollständig** ist. Meine frühere Aussage, drei Räume
fehlten, war ungenau:

| Zone auf Blatt 4 | Art | gemaltes Rechteck |
|---|---|---|
| ROOM1120 z0 | eigene | Nr5 40×40 |
| ROOM1130 z0 | eigene | Nr4 32×80 |
| ROOM1140 z0 | eigene | Nr6 56×56 |
| ROOM1150 z0 | eigene | Nr2 32×40 *(geeicht)* |
| ROOM1160 z0 | eigene | **keines** |
| ROOM1060 z0 | **Gast** (Zuhause Blatt 2) | Nr1 24×24 |
| ROOM1080 z0 | **Gast** (Zuhause Blatt 2) | — |
| ROOM1170 z1 | **Gast** (Zuhause Blatt 5) | — |

ROOM1080 und ROOM1170 sind Gast-Zeilen; ihr Rechteck steht auf ihrem eigenen Blatt. Auf
3F brauchen sie keins. Bleibt **ROOM1160** — und der lässt sich nicht ergänzen:

```
ROOM1160 Tueren:  (-300,-16500) -> ROOM1180      (Blatt 0/1, also B1/B2)
                  (-300,-16500) -> ROOM1230      (Blatt 1)
```

⛔ **ROOM1160 hat auf Blatt 4 keinen einzigen Nachbarn.** Beide Türen führen vom Stockwerk
weg. Es gibt also nichts, woran ein ergänztes Rechteck anschließen könnte — es stünde frei
in der Fläche. Genau deshalb malt das Original dort nichts, und das ist die bessere
Darstellung. Die Original-Seitenzuordnung bestätigt, dass der Raum trotzdem zu 3F gehört
(`FUN_8004b568`: Räume 18–22 → Seite 4, ROOM1160 = Index 22).

**Ergebnis für 3F: 4 von 4 verortbaren eigenen Zonen sitzen auf der Original-Kunst.**
Nichts zu ergänzen.

### Game-weit ist die Lage anders

Zweistufige Zuordnung (geeichte Räume direkt projiziert, Stubs über den Türgraphen):
**58 von 103 Zonen** lassen sich auf der Original-Kunst verorten, 45 nicht; 54 gemalte
Rechtecke bleiben unbeansprucht. Ein Teil davon ist mein Zuordner, nicht die Datenlage —
die Zahl ist eine **Untergrenze**. Aber ein struktureller Fall bleibt:

| Blatt | geeichte Anker |
|---|---|
| **3** | **0** — kein Anker, nichts zu propagieren |
| 4, 6, 10, 11, 12 | je 1 |
| 7 | 10 |

Auf Blatt 3 gibt es keinen einzigen geeichten Raum; dort kann die Kette gar nicht starten.
Eine Karte, die auf einem Blatt Original-Kunst zeigt und auf dem nächsten meine
Zeichnungen, sieht uneinheitlich aus — die Bedingung „wenn das gutaussehend geht" ist
game-weit derzeit **nicht** erfüllt, auf 3F dagegen schon.

## §24 Zuordnung Zone → gemaltes Rechteck: 82 von 117 (`tools/kunst_zuordnung.py`)

Zweistufig: Räume mit ausgelieferter Maßstabszeile werden **direkt projiziert** und
liefern die Anker; die übrigen kommen über den **Türgraphen** dazu (ein freies Rechteck,
das ein bereits gesetztes Nachbar-Rechteck berührt).

| Fassung | verortet | freie Rechtecke |
|---|---|---|
| nur Größe, Blatt-Median als Maßstab | 55 | — |
| + geeichte Zeile je Raum direkt projiziert | 58 | 54 |
| + Ausbreitung über den Türgraphen | **75** | 43 |
| + „ohne Alternative ist die Größe keine Frage" | **82** | 36 |

⛔ **Größe ist ein schwaches Maß** — der Maßstab ist je *Raum* verschieden, für die Stubs
steht nur der Blatt-Median zur Verfügung. Sie darf deshalb nur die Feinauswahl treffen.
Gibt es für eine Zone genau **ein** freies Rechteck an einem gesetzten Nachbarn, ist die
Zuordnung erzwungen; 25 Zonen hingen allein an der Größenschranke.

### Unabhängiger Zeuge: die Türprobe

**70 von 77 Durchgängen** zwischen zwei zugeordneten Zonen haben berührende Rechtecke
(**91 %**). „Berührend" war für die Anker nie ein Kriterium — die Übereinstimmung ist
also nicht hineingerechnet. Die sieben Ausnahmen (dreimal ROOM30E0, ein Mehrzonen-Raum)
gehören in die Werkzeug-Reserve.

### Was übrig bleibt, nach Ursache getrennt

| Grund | Zonen | Bewertung |
|---|---|---|
| **strukturell** — kein Türnachbar auf dem eigenen Blatt | **8** | Das Original malt sie zu Recht nicht: ROOM11A0, ROOM1250, ROOM10B0, **ROOM1160**, ROOM5080, ROOM5090, ROOM6020 (2 Zonen). Ein ergänztes Rechteck stünde frei in der Fläche. |
| **Blatt 3 ohne Anker** | **9** | Keiner der Räume 12–17 (ROOM10C0…ROOM1110) hat eine geeichte Zeile, auch die Gast-Zeilen nicht. Ohne ein einziges (Zone → Rechteck) kann die Kette nicht starten. |
| **Werkzeug-Reserve** | **18** | Hätten Nachbarn auf dem Blatt; der Zuordner erreicht sie noch nicht. |

Die 8 strukturellen Fälle beantworten die Frage „ergänzen?" von selbst: dort gibt es
nichts, woran etwas anschließen könnte. Für Blatt 3 müsste der Maßstab aus derselben
Stage geliehen werden (die STAGE1-Zeilen streuen um ~15 %) und ein Startpaar anders
gefunden werden — etwa über den Rechteck-Grad (das Rechteck mit den meisten Nachbarn ist
der Raum mit den meisten Türen). Das ist noch nicht gemessen.

## §25 ⛔ Ein Raum ist NICHT ein Rechteck — die Zuordnung ist n:m

Blatt 3 sollte über den Rechteck-Grad geknackt werden. Der Versuch ist gescheitert, und
zwar an einer falschen Annahme meinerseits — die Messung dahinter ist der eigentliche
Gewinn.

**Erster Hinweis:** der Maßstab für Blatt 3 lässt sich aus den Daten anpassen (er ist je
Blatt einer, also eine eindimensionale Anpassung). Die Kurve ist aber **flach** — 1780,
1800, 1840 liefern alle denselben mittleren Größenabstand von ~20 px. Eine flache Kurve
mit großem Rest heißt: die Größen entsprechen einander gar nicht. Dazu passen die
Rechtecke Nr0 (32×96) und Nr2 (24×80): lang und dünn, also **Flure** — und kein Zonen-
Kasten auf dem Blatt hat dieses Seitenverhältnis.

**Die Gegenprobe an den geeichten Räumen** (ihre Lage ist ausrechenbar, also zählbar, wie
viele Rechtecke sie überdecken):

| | Räume |
|---|---|
| decken **genau ein** Rechteck | **20** |
| decken **mehrere** | **16** |
| decken **keines** (sitzen IN einem größeren) | **4** |

```
ROOM1030  Nr4 (98 %), Nr10 (100 %)
ROOM1190  Nr0 (67 %), Nr1 (84 %), Nr2 (92 %)
ROOM1210  Nr4, Nr5, Nr6, Nr7 (je 100 %), Nr8 (88 %)
ROOM5140  Nr10 (91 %), Nr11 (67 %), Nr12 (97 %), Nr14 (100 %)
```

⛔ **Das Original malt einen Raum aus MEHREREN Rechtecken, wenn er kein Rechteck ist** —
L-förmige Flure, große Hallen. Und umgekehrt sitzen kleine Räume (ROOM5010 15×23,
ROOM4030) **innerhalb** eines größeren Rechtecks. Die Beziehung ist **n:m**, nicht 1:1.

`tools/kunst_zuordnung.py` erzwingt aber eine 1:1-Zuordnung (`belegt`-Menge). Damit sind
die 82 verorteten Zonen zwar überwiegend richtig — die Türprobe mit 91 % belegt das —
aber die 36 „freien" Rechtecke sind **kein Rest**: es sind die Zweit- und Drittstücke von
Räumen, die schon zugeordnet sind.

### Was daraus folgt

Für das Ziel des Nutzers (Original-Kunst zeigen, RE2-Türen, Treppen, rotes Hervorheben)
ist das **kein Hindernis, sondern eine Vereinfachung**: die rote Hervorhebung färbt
einfach *alle* Rechtecke des Raums, in dem man steht. Der Zuordner muss dafür von 1:1 auf
n:m umgebaut werden — jede Zone sammelt ihre Rechtecke, statt eines zu belegen.

Für Blatt 3 bleibt das Problem bestehen, aber mit anderer Ursache als gedacht: nicht der
fehlende Anker allein, sondern dass dort mehrere Räume als Flur-Ketten gemalt sind und
eine Größen-Anpassung deshalb gar nicht greifen kann.

## §26 Die fehlenden Zeilen sind berechenbar — 80 von 96 Räumen

### Erst der Befund, der alles erklärt: es gibt keinen gemeinsamen Weltraum

Die Abbildungen der geeichten Räume eines Blattes stimmen **nicht** überein. Als affine
Form `map_x = a·wx + b` geschrieben, streuen sie auf Blatt 7 über `a` = 0,00187…0,00238
und `b` = 118…270:

```
ROOM3000  map_x = 0.00215*wx +139.8      ROOM3040  map_x = 0.00214*wx +251.4
ROOM3030  map_x = 0.00187*wx +255.7      ROOM3090  map_x = 0.00197*wx +270.0
```

⛔ **Jeder Raum hat sein eigenes lokales Koordinatensystem** (RE1.5-Raumkoordinaten sind
raumlokal, jede RDT hat ihren eigenen Ursprung). Die Zeile @0x800768b0 *ist* die
Platzierung dieses einen Raums auf der Kunst — es gibt keine Blatt-weite Abbildung, in die
man einen fehlenden Raum einfach hineinprojizieren könnte. Das erklärt auch, warum die
Maßstab-Anpassung auf Blatt 3 flach blieb.

**Nebenbefund, unabhängige Bestätigung von v0.3.96:** ROOM5030 und ROOM5110 tragen im
Original **dieselbe** Zeile (0.00171, +179.7), ebenso ROOM5040/ROOM5120 und
ROOM50A0/ROOM5140. Die drei Varianten-Paare, die ich aus Zellzahl und Türschlitzen
hergeleitet hatte, stehen in der ausgelieferten Tabelle als identische Abbildung.

### Die Tür ist der gemeinsame Punkt

Eine Tür ist derselbe Ort in **zwei** lokalen Systemen: Raum A kennt sie bei (ax,az),
Raum B bei (bx,bz). Ist A geeicht, ist der Kartenpunkt der Tür ausrechenbar — und Bs Zeile
muss (bx,bz) genau dorthin abbilden. Damit fällt Bs Versatz aus der Gleichung.

**Auslassprobe** an den 38 geeichten Räumen (so tun, als fehlte die Zeile, aus dem
Nachbarn herleiten, mit der echten vergleichen) — nur wo das Verfahren gelten kann
(gleiches Blatt, keine Selbst-Tür, beide Räume einzonig):

| | |
|---|---|
| geprüfte Paare | 32 |
| Median-Fehler | **6 px** |
| ≤ 4 px | 13 |
| ≤ 8 px | 21 |

Die besten liegen bei **0–2 px** (ROOM1040→ROOM1030 exakt). Der Restfehler kommt daher,
dass mit *einer* Tür nur der Versatz bestimmt ist und der Maßstab vom Nachbarn geliehen
werden muss — und der schwankt je Raum um bis zu Faktor 2.

### Mit zwei Türen ist auch der Maßstab bestimmt

Zwei Türen zu bekannten Nachbarn = zwei Punkte, zwei Unbekannte je Achse. Über vier
Ausbreitungsrunden:

| Stufe | Räume |
|---|---|
| ausgeliefert geeicht | 38 |
| **zwei** bekannte Nachbarn → Versatz **und** Maßstab exakt | +9 |
| **eine** Tür → Versatz exakt, Maßstab geliehen (≈ 6 px Median) | +33 |
| **nicht erreichbar** | **16** |

**80 von 96 Räumen** lassen sich damit auf der Original-Kunst verorten. Die 16 Reste haben
keinen bekannten Nachbarn auf ihrem Blatt — darunter **alle sechs Räume von Blatt 3**
(ROOM10C0…ROOM1110), was den Anker-Befund aus §24 bestätigt, sowie die schon bekannten
Einzelgänger ROOM1160, ROOM1250, ROOM10B0, ROOM5080, ROOM5090, ROOM6020.

## §27 RE1.5-Türsymbole entfernen: die Lauflänge trennt sie von der Wand

Nutzer: *„Wenn du die RE 2 Türsymbole drauf abbildest musst du aber auch die RE 1.5
Türsymbole vorher entfernen."* Richtig — sie stecken in der gemalten Kunst und schienen
sonst unter den Balken durch.

**Was dort steht:** die RE1.5-Türsymbole sind **Türschwenk-Bögen** wie im Architektenplan
— ein kurzer Strich mit Viertelkreis, in die Wand gezeichnet und in den Raum ragend. Die
Palette von MAP05 hat nur 13 Farben; Raum = `(0,72,0)`, Wand = `(176,176,176)`, und die
Symbole sind in der Wandfarbe gezeichnet. Über die Farbe sind sie also nicht zu fassen.

**Über die Lauflänge schon.** Wandlinien sind lange gerade Läufe, ein Schwenk-Bogen ist
kurz und gekrümmt. Gemessen auf der 3F-Seite (747 helle Pixel):

```
Laengster gerader Lauf je hellem Pixel:
    1: 7    3: 18   4: 12   5: 8    6: 25   7: 5
    8: 6   10: 17  18: 32  19: 17   20+: 600
```

Klar zweigipflig. Schwelle „Lauf ≤ 4, dann an Nachbarn mit Lauf ≤ 10 gewachsen" liefert
**6 Zusammenhangskomponenten** — genau die Zahl der Symbole im Bild. ⛔ Am gerasterten
Bild geprüft (`tuersymbole_erkannt.png`, Fundstellen rot): **alle Symbole getroffen, kein
einziges Wandpixel**.

**Ersetzen** ist dann mechanisch: die Symbolpixel bekommen die Raumfarbe (sie liegen im
Raum), und an ihre Stelle kommt ein gelber RE2-Balken `(224,168,40)` in der Wand, längs
deren Achse (die Achse folgt aus der Lauflänge des nächsten Wandpixels).

Ergebnis: `karte_3F_re2tueren.png` (4× vergrößert). Die Schwenke sind weg, die Balken
sitzen in der Wand.

**Zwei Reste, ehrlich benannt:**
* Ein Bogen unten in der Mitte ist nur teilweise entfernt — ein Stück mit Lauflänge
  zwischen 5 und 10, das nicht mehr an die Startmenge angrenzte.
* Die dortige **Doppeltür** wurde als ein Symbol gezählt und bekam nur einen Balken.

Beides sind Schwellen-Fälle, keine Grenzen des Verfahrens.

## §28 Zeilen-Erzeugung: `tools/gen_marker_zeilen.py`, 73 von 96 Räumen

Die Herleitung über die Türkette ist jetzt Werkzeug, mit der **Auslassprobe als eingebautem
Riegel**: für jeden Raum mit ausgelieferter Zeile wird so getan, als fehle sie, die Zeile
wird hergeleitet und mit der echten verglichen.

| | |
|---|---|
| Räume mit Zeile | **73 von 96** (38 ausgeliefert + 35 hergeleitet) |
| Auslassprobe | 22 Fälle, **Median 7 px**, ≤ 4 px: 7, ≤ 8 px: 13 |
| ohne Zeile | 23 — u. a. alle sechs Räume von Blatt 3 |

**Achsen getrennt lösen.** Ein erster Wurf verlangte von *einem* Türpaar Abstand in
*beiden* Achsen. Zwei Türen an derselben Wand unterscheiden sich aber nur in einer — damit
fiel der Zwei-Türen-Weg fast immer aus (1 von 96 Räumen). x und z sind unabhängige
Gleichungen und dürfen aus verschiedenen Paaren kommen.

### ⛔ Feinschliff an den gemalten Rechtecken: gemessen und verworfen

Naheliegend: die grob hergeleitete Zeile nachziehen, bis der Raumkasten möglichst gut auf
den gemalten Rechtecken liegt — die Kunst ist ja die stärkere Fessel. **Das Ergebnis ist
schlechter:**

| | ohne Feinschliff | mit |
|---|---|---|
| Median | **7 px** | 15 px |
| ≤ 8 px | **13** von 22 | 7 von 22 |

Der Grund ist mein eigener alter Fehler ([[reai-v2-proxy-mass]]): maximiert wird die
Deckung mit *irgendeinem* Rechteck des Blattes — also zieht der Raum auf den dichtesten
Haufen statt auf **sein** Rechteck. Richtig wäre, nur die Deckung mit den ihm zugeordneten
Rechtecken zu zählen; die Zuordnung braucht aber die Zeile, die hier erst entsteht. Wer
das auflösen will, muss beide Schritte **gemeinsam** lösen, nicht nacheinander.
Abschaltbar geblieben unter `RE15_FEINSCHLIFF=1`, damit die Messung nachvollziehbar ist.


## §29 ⛔ Die Messschiene log: 61 von 149 „Sprüngen“ waren ein Werkzeug-Artefakt

`integration_map_raum_live` meldete **61 von 149 Übergängen mit über 16 px Sprung** —
genau die Größenordnung des Nutzer-Reports *„Springt immer noch durch die Kartenbereiche
nach dem Durchlaufen von Türen."* Daneben meldete `integration_map_uebergang` für
**dieselbe Größe** (Trigger in A → Spawn in B) Median 3 px und 23 Ausreißer über 8 px.
Beide Schienen liefen grün. Im Test stand dazu eine Erklärung — und die war falsch:

> *„Diese Phase läuft über ALLE AOT-Slots, auch über doppelt belegte mit veraltetem
> Spawn; sie misst damit eine ANDERE Menge … Eine Zahl, deren Abweichung ich nicht
> erklären kann, gehört nicht in eine Schranke."*

### Erst messen, ob die Ausrede trägt

Wären wirklich veraltete Spawns schuld, müssten Spawn und Gegen-Trigger derselben Tür in
der **Welt** weit auseinanderliegen. Gemessen über alle 217 Türen mit Gegen-Datensatz
(Abstand zum *nächsten* Gegen-Trigger, damit Raumpaare mit mehreren Durchgängen nicht
künstlich schlecht aussehen):

| | Welteinheiten | ≈ Kartenpixel |
|---|---|---|
| Median | 850 | **1 px** |
| 75 % | 1 000 | 2 px |
| 95 % | 1 850 | 3 px |
| über 8 px | **2 von 217 (1 %)** | max 16 px |

⇒ Die Spieldaten binden beide Türseiten fest zusammen. Ein großer Kartensprung **kann**
nur ein Kartenfehler sein — die Ausrede trug maximal 1 % der Fälle.

### Der Defekt: live über ein Array laufen, das der eigene Rumpf neu befüllt

```c
if (!betrete(rid, 0, 0, -1)) continue;
for (k = 0; k < RE15_AOT_MAX; k++) {
    const re15_aot_t *a = &g_aot.slots[k];      /* <== LIVE gelesen */
    ...
    schau(rid,  ax, az, ...);                   /* betrete() -> re15_aot_init() */
    schau(ziel, bx, bz, ...);                   /* und noch ein Raum */
```

`schau()` betritt zwei weitere Räume, `betrete()` ruft `re15_aot_init()`. Ab dem zweiten
Durchgang liest die Schleife also die Türen des **zuletzt geladenen** Raums und schreibt
sie dem aktuellen zu. Am Einzelfall nachgewiesen: für ROOM1140 meldete die Phase Türen
nach ROOM1120 — ROOM1140 hat aber genau **eine** installierte Tür (Slot 0 → ROOM1130;
Slot 1 ist `sce=0`, also inert). Und der gemeldete B-Punkt (197,128) war exakt der von
`ROOM1170 -> ROOM1130` aus der anderen Schiene.

**Behebung:** die Türliste vor der Schleife per `memcpy` abschreiben (Phase 2 *und*
Phase 3, dort dieselbe Falle).

| | vorher | nachher |
|---|---|---|
| Türen durchschritten | 199 | **201** |
| auf demselben Blatt | 149 | **172** |
| Sprung über 16 px | **61** | **10** (5,8 %) |
| richtiger Raum rot | 199/199 | **201/201** |

Die Karte war die ganze Zeit besser, als ihre eigene Messschiene behauptet hat. An die
Stelle der Ausrede tritt jetzt ein Riegel (höchstens jeder zehnte Übergang über 16 px).

**Die verbliebenen 10** (`RE15_SPRUNG_LISTE=1`): ROOM1090→ROOM1100 93 px (Blatt 3 — das
Blatt ohne jeden geeichten Anker, §26), ROOM3060→ROOM3020 33 px, ROOM4040→ROOM4050 23 px,
ROOM1210→ROOM1220 21/17 px, ROOM1050→ROOM1000 21 px, ROOM2070→ROOM2000 20/17 px,
ROOM6000→ROOM6010 19 px, ROOM1210→ROOM11E0 17 px.

**Lehre.** Zwei Schienen, die dieselbe Größe messen und sich widersprechen, sind ein
Befund und kein Nebenbefund. Ich hatte die Abweichung mit einer Geschichte zugedeckt,
statt sie an den Daten zu prüfen — die Prüfung dauerte fünf Minuten und war eindeutig.
Verwandt mit [[reai-v2-live-statt-nachbildung]] und [[reai-v2-schwaches-mass]].

## §30 Zeilen-Herleitung: 82 von 96 Räumen — drei gemessene Gewinne

Der Türketten-Löser (`tools/gen_marker_zeilen.py`, §28) stand bei 73 von 96. Drei
Änderungen, jede einzeln an der Auslassprobe gemessen:

**1. Die Auslassprobe meldet ihre Abdeckung** ([[reai-v2-schiene-abdeckung]]). Sie prüfte
nur 22 der 38 ausgelieferten Zeilen — die übrigen 16 hängen an Nachbarn, die selbst keine
ausgelieferte Zeile haben. Ohne diese Zahl sah „Median 7 px" nach mehr aus, als es war.

**2. Der Zonen-Filter sperrte den Empfänger zu Unrecht.** `ZONEN_N != 1` galt für *beide*
Seiten. Die Zeile @0x800768b0 steht aber pro **Slot = pro Raum**
(`re15_inv_screen.c:273-320`), nicht pro Zone; wie viele gemalte Rechtecke ein Raum
bedeckt, kommt in der Rechnung gar nicht vor. Gemessen: der mehrzonige Empfänger ROOM30E0
leitet mit **7 px** her, exakt wie die einzonigen. Abdeckung 22 → 24.

**3. Drei Korrespondenz-Quellen statt einer, nach Genauigkeit gestaffelt.** Ein
Türeintrag (`Door_aot_set`) trägt *beide* Seiten: `lx,lz` = Trigger im Quellraum,
`nx,nz` = Spawn im Zielraum (`gen_map_zones.py:335`). Damit gibt es drei Wege zum
gemeinsamen Punkt — je einzeln gemessen:

| Familie | Auslassprobe | Abdeckung |
|---|---|---|
| Trigger ↔ Trigger | **7 px** | 23/38 |
| Spawn (derselbe Datensatz) | 9 px | 24/38 |
| einseitige Türen (A kennt B, B nicht A) | 9 px | 24/38 |
| **alle gemischt** | **8 px** | 25/38 |

⛔ Mischen ist **schlechter** — der Spawn liegt nicht in der Tür, sondern ein Stück im
Raum, das ist ein systematischer Versatz. Also Rangfolge statt Beimischung: die genaue
Familie gewinnt, die ungenauen sind nur Rückfallebene für Räume, die sonst nichts
hätten. Die einseitigen Türen allein holten fünf Räume (ROOM1080, ROOM4020, ROOM4040
und in der Folge ROOM4050/ROOM4070), die einen Nachbarn mit Zeile direkt daneben hatten.

**4. Der Versatz ist der Median aller Türpunkte, nicht der der ersten Tür.** Der
Rauschboden ist messbar: an den Räumen mit ausgelieferter Zeile liegen die zwei
Kartenpunkte **einer** Tür im Median 5 px auseinander.

| | vorher | nachher |
|---|---|---|
| Räume mit Zeile | 73/96 | **82/96** |
| Auslassprobe geprüft | 22/38 | **26/38** |
| Median | 7 px | **7 px** |
| innerhalb 8 px | 13 | **16** |
| schlechter geworden | — | 1 (ROOM3010 16→18 px) |

**Die 14 Reste** haben keinen erreichbaren Nachbarn auf ihrem Blatt: alle sechs Räume von
Blatt 3 (kein Anker, §26) plus die Einzelgänger ROOM10B0, ROOM1160, ROOM1230, ROOM1250,
ROOM5080, ROOM5090, ROOM50F0, ROOM6020.

### ⛔ Der gemeinsame Lösungsweg aus §28 — gemessen und endgültig verworfen

§28 ließ offen, ob Zuordnung und Zeile *gemeinsam* gelöst besser wären. Gebaut wie dort
verlangt (erst mit der groben Zeile das Rechteck zuordnen, dann die Zeile allein auf
**dieses** Rechteck anpassen, Maßstab aus dem Größenverhältnis): **Median 14 px** statt 7.
Fünf Räume gewinnen spektakulär (ROOM3090 51→5, ROOM3010 18→3, ROOM3040 11→3), zwanzig
verlieren. Ein Riegel über die Größen-Übereinstimmung rettet es nicht — bei **jeder**
Schwelle von 0 bis 24 px werden mehr Räume schlechter als besser.

**Der Grund schließt den Weg, und er ist zählbar: die gemalte Kunst liegt auf einem
8-Pixel-Raster.** Alle **236** Kantenlängen der 118 Rechtecke über alle 13 Blätter sind
durch 8 teilbar. Ein Maßstab aus dem Größenverhältnis erbt diese Rasterung: 9 % Fehler
bei einem 88 px breiten Rechteck, 33 % bei 24 px, **100 %** beim 8 px schmalen Korridor
ROOM3030. Die Türkette ist mit ihrem Rauschboden von 5 px die deutlich stärkere Fessel.
Der Weg ist damit nicht „noch nicht sauber gebaut", sondern durch die Auflösung der
Vorlage begrenzt.


## §31 Der Tür-Sprung: 61 → 4 von 167, in vier gemessenen Schritten (2026-09-03)

Ausgangslage nach §29: 10 Übergänge mit über 16 px Sprung. Vier Ursachen, jede einzeln
gemessen, jede mit ihrem eigenen Gegenmaß.

### 1. Der Türzug scheiterte haarscharf — der Rand ist ableitbar

`tuer_anziehen()` zog nur innerhalb des **doppelten** Trigger-Halbmaßes. Der Spawn wird
aber bauartbedingt um mindestens einen **Spielerradius** in den Raum gesetzt, sonst stünde
der Ankommende in der Wand — Radius **450** (`DAT_80073e94[6]`, code-verifiziert,
`re15_collision.c:25/776`). Gemessen an den Fällen aus Audit [M]:

| Tür | Abstand Spawn → Trigger | Zugbereich | fehlte |
|---|---|---|---|
| ROOM6000→ROOM6010 | 885 | 870 | **15** |
| ROOM1050→ROOM1000 | 1050 | 1000 | **50** |
| ROOM4040→ROOM4050 | 1100 | 1000 | **100** |
| ROOM1210→ROOM1220 | 650 | 500 | **150** |

Alle vier unter einem halben Kartenpixel, alle vier kleiner als 450. Die Ankunft fiel per
Konstruktion aus dem Zugbereich, obwohl sie an der Tür steht.

⛔ **Nicht auf den besten Metrikwert gezogen.** Der Rand wurde über 0/150/450/900/1800
gefahren; die Übergangszahl wird **monoton** besser, weil ein größerer Zug beide Seiten
auf dasselbe Symbol schnappen lässt — ein Proxy, der sich selbst belohnt
([[reai-v2-proxy-mass]]). Gegenmaß ist die **verschluckte begehbare Fläche**:

| Rand | Sprünge > 16 px | über 8 px | verschluckte Fläche | Räume > 25 % |
|---|---|---|---|---|
| 0 | 10 | 23 | 5,2 % | 2 |
| **450** | **5** | **12** | **8,4 %** | **8** |
| 900 | 4 | 7 | 12,4 % | 13 |
| 1800 | 4 | 4 | 20,0 % | 31 — ROOM3030 zu **100 %** |

450 ist nicht der beste Wert der Metrik, sondern der abgeleitete. Messhebel `RE15_ZUGRAND`.

### 2. Die Schienen zählten Skript-Warps als Türen

Ein `Aot_set` mit Breite und Tiefe 0 trifft nach dem Original-Trefftest `FUN_80042b64`
(@0x80042b68-98) genau **einen** Weltpunkt — man läuft nicht hindurch, das Skript feuert
ihn per `Aot_on`. Der Zonen-Generator wirft sie raus (`gen_map_zones.py:313`), die Tests
nicht. Betroffen sind **13 Records**, darunter `ROOM3060 → ROOM3020` — als 33-px-Sprung
gemeldet, obwohl ROOM3060 in den RDT-Daten überhaupt keine Tür dorthin hat — und das Paar
`ROOM1170 ↔ ROOM1240` (der Eintritts-Warp). Nach dem Filter: 201 → 191 Türen.

### 3. Das Maß war schärfer als die Vorgabe

„Marker in der roten Fläche" fragte, ob der Marker in einer roten **Füllung** liegt.
Gezeichnet wird ein Raum aber als helle Wandlinie **plus** Füllung darin; ein Marker genau
auf der Wand seines **eigenen** Raums steht damit im Rechteck und fällt trotzdem durch.

Gemessen: **jeder** der sechs gemeldeten Fälle lag exakt **+1 px** daneben, und alle sechs
lagen laut Zonentabelle im eigenen Rechteck (`drin=True`). Die naheliegende Erklärung —
ein Nachbar-Rechteck deckt den Pixel — wurde geprüft und **ausgeschlossen**: ein zweiter
Klemmer bis an die Rechteckkante änderte keine einzige Zahl, der Marker verlässt sein
Rechteck also nie.

Der Nutzer verlangt *„der spielmarker [muss] im kleinen rechteck sein"* — das Rechteck,
nicht sein Inneres. Die Prüfung misst jetzt das, **plus einen neuen scharfen Riegel**: der
Marker darf nie im Rechteck eines *anderen* Raums landen (game-weit 0 Fälle).

### 4. Volles Zug-Gewicht im Trigger

Bis dahin fiel das Gewicht von der **Mitte** des Triggers linear auf null; an der
Trigger-Kante — wo man beim Öffnen steht und wo der Spawn liegt — waren davon rund 27 %
übrig. Im Trigger ist die Tür nach `FUN_80042b64` bedienbar; das ist die spieleigene
Definition von „an dieser Tür". Jetzt 1 im ganzen Trigger, dann stetig auf 0 bis zum Rand
des Zugbereichs — an der Trigger-Kante beidseitig 1, also weiterhin sprungfrei.

Erst mit dem geschärften Maß aus Schritt 3 ist das **kostenlos**: 96/96 im eigenen
Rechteck, 0 im fremden Raum, und innerhalb 2 px steigt von 43 % auf 66 %.

### 5. Einseitige Türen binden jetzt den Grundriss-Löser

`anheften()` in `tools/grundriss.py` bewertete Kandidatenlagen danach, wie viele **schon
gesetzte** Kanten sie miterfüllt — zählte dabei aber nur `kanten` (reziproke Türpaare),
nicht `notkanten` (einseitige). Für einen Ort, dessen einzige Verbindung auf dem Blatt
einseitig ist, war `treffer` damit bei **jeder** Lage 0, und die Wahl fiel allein über die
Überlappung — die schiebt den Raum aktiv **weg**.

Wirkung auf Blatt 6: **3 GETRENNT → 0**, Überlappung 11,7 % → 4,5 %, Durchgänge
deckungsgleich 10/13 → 11/13. Im Karten-Audit fällt damit der **einzige echte Fehler**
weg: [N] „Symbole verschiedener Durchgänge liegen aufeinander" **1 → 0**
(Blatt 6 (188,105): ROOM2030↔ROOM2070 und ROOM2070↔ROOM0000).

⛔ **Ehrlicher Preis:** dieselbe Umordnung erzeugt **einen** neuen Markersprung,
ROOM2070→ROOM2000 mit 27 px, und der schlimmste Sprung steigt von 21 auf 27 px. Zwei
Türsymbole dauerhaft übereinander wiegen schwerer als ein Sprung an einer Tür.

### Stand

| | vorher (§29) | jetzt |
|---|---|---|
| Sprünge über 16 px | 10 von 172 | **4 von 167** |
| über 8 px | 23 von 153 | **7 von 151** |
| innerhalb 2 px | 31 % | **66 %** |
| Median | 3 px | **2 px** |
| richtiger Raum rot | 201/201 | **191/191** |
| Marker im eigenen Rechteck | 95/96 (Füllung) | **96/96** (Rechteck) |
| Marker im **fremden** Raum | nie geprüft | **0** |
| Karten-Audit, echte Fehler | 1 | **0** |

Die vier Reste: ROOM1090→ROOM1100 93 px, ROOM2070→ROOM2000 27 px, ROOM1050→ROOM1000
21 px, ROOM1210→ROOM11E0 17 px.

### ⛔ Nebenbefund: das Audit sah die Gast-Zeilen nicht

Die Prüfungen [F] und [M] in `karte_audit.py` benutzen `zone_von()`, und das überspringt
Etagen-/Gastzeilen (`if z['etage']: continue`) — genau die Zeilen, die die Engine benutzt
(`re15_map_floor_lookup` / `re15_map_zone_fuer`). Mit voller Abdeckung — alle Blätter, auf
denen **beide** Räume eine Zeichnung haben — gibt es im ganzen Spiel genau **eine** Lücke:
**ROOM1090 ↔ ROOM1100, 42 px auf Blatt 3**, wo ROOM1090 Gast ist. 100 von 101 Paaren
stoßen an.

Der Fall ist bis auf den Löser durchdiagnostiziert: die Notkante `ROOM1090 → ROOM1100`
**existiert** (Kantendump Blatt 3: 6 Türkanten, 3 Notkanten), aber der Löser heftet
ROOM1090 immer an **ROOM10F0** an (9 von 9 Versuchen, über dessen eigene Notkante) und
prüft die Bindung an ROOM1100 danach nie wieder nach. Am Ende liegt ROOM1090 von **beiden**
42 px entfernt.


## §32 Original-Kartenmaterial statt gerechneter Grundrisse — Stand 2026-09-04

Nutzer: *„Die Map hat noch immer die selbstgezeichneten Räume."* und *„wollten wir jetzt
ja doch das Original Kartenmaterial nutzen und nur die Türen durch RE 2 Türen ersetzen
und Treppensymbole hinzufügen."*

### Der Befund

**234 von 234 Zonen tragen `rect = 255`** — keine einzige benutzt ein gemaltes
Original-Rechteck, auf keinem Blatt. Die Zuordnung auf die Kunst wird sehr wohl
**berechnet** (`assign`, 75 von 103 Zonen), aber an **zwei** Stellen im Generator
bedingungslos überschrieben:

```python
gen_map_zones.py:1763   if (page_of(b), b, i) in grundrisse: continue   # Zeile faellt weg
gen_map_zones.py:~1893  synth[_k] = ...                                  # jeder Ort bekommt eine Zeichnung
```

Das geht auf v0.3.84 zurück (Nutzer-Freigabe 2026-09-01, [[reai-v2-karte-grundriss]]);
die Richtung ist jetzt zurückgenommen.

⛔ **Beide Stellen müssen gemeinsam schalten.** Nur eine gedreht lässt eine Zone durch
beide Raster fallen und ganz aus der Tabelle verschwinden: gemessen 234 → 66 Zeilen, nur
noch 31 von 96 Räumen im Live-Test.

### Was funktioniert

`RE15_KUNST=1` dreht beide Stellen um. Ergebnis: **144 von 210 Zonen mit gemaltem
Original-Rechteck**, jedes Blatt zeigt Original-Material, Blatt 4 und 5 zu 100 %. Und die
Karte wird dadurch strukturell besser — die Handarbeit ist stimmig:

| Audit | Grundriss-Lösung | Original-Material |
|---|---|---|
| [F] Lücken | 0 (nach Fix) | **0** |
| [K] Überlappung | Blatt 7: 30 % | **0** |
| [M] Erscheinungspunkt weit | 14 | **0** |
| [N] Symbole übereinander | 0 (nach Fix) | **0** |

`RE15_KUNST=misch` behält zusätzlich die 33 Räume, die das Original gar nicht malt.

### ⛔ Was es blockiert

⛔ **KORREKTUR AN MIR SELBST (dieselbe Sitzung).** Hier stand, der Port zeichne die
gemalten Kacheln gar nicht — der Op-Dump für ROOM1130 habe keine einzige Raumfläche
enthalten. Das war ein **Messfehler**: mein Patch, der den Dump auch `SPRT`-Ops ausgeben
lässt, war wegen einer fehlgeschlagenen Zusicherung nie angewendet worden. Der Dump zeigte
weiterhin **nur `FILL`** — und die gemalten Rechtecke sind nun einmal keine `FILL`s. Die
Kacheln wurden die ganze Zeit gezeichnet. Der Fehler ist die Klasse aus
[[reai-v2-schiene-abdeckung]]: ein leeres Ergebnis ist nur dann ein Befund, wenn die
Quelle das Gesuchte überhaupt zeigen kann.

Zwei Dinge, die dabei zu klären sind:

* Die gemalten Rechtecke sind **`SPRT`-Bildkacheln der Originalkarte**, im Zustandston
  moduliert (grau unbesucht, grün besucht, rot aktuell 192/24/24) — **keine `FILL`s** wie
  die Schema-Zeichnungen (74/20/20). Jede Prüfung, die nur `FILL` kennt, ist für die
  Kunst blind.
* Bei `RE15_KUNST=misch` überlappen die 33 Löser-Kästen die Kacheln, und weil die Kacheln
  **zuerst** eingetragen werden (früher = oben, [[reai-v2-zeichenreihenfolge-invers]]),
  verschwinden die Kästen darunter. Es gibt keinen gemeinsamen Weltraum (§26) — Mischen
  ist der falsche Weg; entweder Kunst oder Grundriss.

### ⛔ Ein Umbau der Messschiene, der zurückgenommen wurde

Um die Kunst messen zu können, hatte ich `test_map_raum_live` beigebracht, auch
`SPRT`-Kacheln als Raumfläche zu zählen. **Das hat die Schiene zerlegt:** der
Kartenschirm zeichnet auch Rahmen und Panels als `SPRT`, die liegen früher in der Liste,
und damit galt rechnerisch jeder Raum als verdeckt — 96 → **4 von 96** gemeldete
Hervorhebungen, bei völlig unveränderter Karte. Ein Filter auf den Zustandston hat es
nicht gerettet; die Ursache blieb offen, also ist der Umbau **zurückgenommen**. Die
Schiene steht wieder bei 96/96.

Behalten wurde nur die Audit-Seite: `karte_audit.kasten()` liefert jetzt auch das gemalte
Rechteck, sonst meldet [B1] 65 Räume als „ohne Zeichnung", die in Wahrheit ihr
Original-Rechteck benutzen.

### Was für den Wechsel noch fehlt

1. Finden, warum `re15_map_rect_state()` die zugeordneten Rechtecke nicht als
   besucht/aktuell meldet.
2. Eine Messschiene, die `SPRT`-Kacheln **richtig** von Rahmenwerk trennt.
3. Entscheiden, was mit den 28 Räumen ohne Zuordnung geschieht — das Original malt sie
   nicht; sie blieben auf der Karte leer, so wie dort.

Türsymbole und Treppen sind davon unabhängig **fertig**: der RE1.5-Nischen-Code ist tot
(`(void)SYM;`), gezeichnet wird der gelbe RE2-Balken, und Audit [D1]/[I] melden je 0
fehlende.


## §33 Original-Kartenmaterial: es funktioniert — und kostet die Marker-Genauigkeit

Nachdem der Messfehler aus §32 behoben war (Kacheln über die **Textur-Seite** `MAP4`
erkennen, nicht über die Farbe — Rahmen und Panels tragen dieselben Töne), ließ sich der
Zustand endlich messen.

### Die Zahlen, nebeneinander

| | Grundriss-Lösung (ausgeliefert) | Original-Kartenmaterial |
|---|---|---|
| Räume mit Zone überhaupt | **96 von 96** | 67 von 96 |
| davon sichtbar rot | **96** | 50 von 67 |
| Marker im eigenen Rechteck | 95/96 | **50/50** |
| Marker im fremden Raum | 0 | **0** |
| Übergänge, Median | **2 px** | 11 px |
| innerhalb 2 px | **66 %** | **0 %** |
| innerhalb 8 px | **95 %** | 40 % |
| schlimmster Sprung | **21 px** | **20 474 px** |
| Audit [F]/[K]/[M]/[N] | 0 / 0 / 0 / 0 | **0 / 0 / 0 / 0** |
| Audit [B1] ohne Zeichnung | 0 | 67 |

### Was das heißt

**Die Kunst gewinnt beim Aussehen, die Grundrisse beim Verhalten.** Auf der Original-Kunst
gibt es keine Überlappungen und keine Lücken — sie ist Handarbeit und in sich stimmig.
Aber:

* **29 von 96 Räumen erscheinen gar nicht.** Das Original malt sie nicht; der Prototyp hat
  für sie kein Rechteck. Auf der Karte bleiben sie leer — so wie im Original.
* **Der Marker springt.** Median 11 px statt 2 px, und der schlimmste Fall liegt bei
  **20 474 px**: die gemalten Rechtecke sind **nicht maßstabsgetreu** zur Kollision
  (§22 — ROOM1100 misst echt 41×47 px, sein Rechteck 24×24). Wo Rechteck und Raum stark
  auseinanderliegen, explodiert die Projektion.
* Mischen ist keine Lösung: Löser-Kästen und gemalte Kacheln stehen in **verschiedenen**
  Koordinatensystemen (§26), überlappen einander, und die Kacheln liegen oben.

### Der nächste Schritt, wenn die Kunst bleiben soll

Die Marker-Abbildung darf dann nicht mehr die Zonen-Bbox linear auf das Rechteck legen,
sondern muss die **ausgelieferte Maßstabszeile** @0x800768b0 benutzen, wo es sie gibt
(38 Räume), und für den Rest die aus der Türkette hergeleitete (§30, 82 von 96, Median
7 px). Genau dafür ist `tools/gen_marker_zeilen.py` gebaut — es ist bisher nur nirgends
angeschlossen (`--schreiben` steht im Kopf und ist nicht umgesetzt). **Das ist die
Verbindung, die noch fehlt.**


## §34 Marker-Zeilen angeschlossen — und gemessen wieder abgehängt

§33 hatte den fehlenden Baustein benannt: auf der gemalten Karte darf der Marker nicht
die Zonen-Box linear ins Rechteck strecken, sondern muss die **Zeile** @0x800768b0
benutzen. Das ist jetzt gebaut und gemessen.

**Gebaut:** `tools/gen_marker_zeilen.py --schreiben` (der Modus stand seit jeher im Kopf
des Werkzeugs und war **nie implementiert**) erzeugt `engine/src/re15_map_zeilen.h` mit
**44 hergeleiteten Zeilen** — ausschließlich für Slots, deren ausgelieferte Zeile ein
Platzhalter ist. Wo das Original eine echte Zeile führt, gilt die. In der Engine wählt
`karten_zeile()` genau so aus, `zeile_projizieren()` rechnet die Original-Formel.

**Gemessen** (Original-Kartenmaterial, `RE15_KUNST=1`):

| | lineare Streckung | mit Zeile |
|---|---|---|
| Übergänge, Median | **11 px** | 35 px |
| innerhalb 4 px | **16 %** | 0 % |
| innerhalb 8 px | **40 %** | 16 % |

⛔ **Schlechter, und der Grund ist strukturell.** Es gibt **zwei unabhängige
Platzierungen**: unsere Zone→Rechteck-Zuordnung stammt aus einer *Kostenheuristik*, die
Zeile aus dem *Türgraphen*. Beide setzen den Raum an verschiedene Stellen. Der
zeilen-projizierte Punkt fällt dann aus dem zugeordneten Rechteck und wird an dessen
Kante geklemmt — daraus entsteht der größere Fehler.

**Die Auflösung ist benannt, nicht gebaut:** die Zuordnung muss **aus den Zeilen**
gewonnen werden statt aus einer Kostenfunktion. §22 zeigt, dass das trägt — die gemalten
Rechtecke *sind* die Kollisionsboxen, 30 von 38 geeichten Räumen treffen ihr Rechteck zu
mindestens 50 %, vier davon exakt. Dann stimmen Zuordnung und Projektion per
Konstruktion überein. Das ist derselbe „beides gemeinsam lösen"-Schritt, den schon §28
und §30 gefordert haben.

Der Weg bleibt unter `RE15_KARTENZEILE=1` messbar; Default ist die lineare Streckung.

### Stand der Umstellung auf das Original-Kartenmaterial

| | Grundrisse (ausgeliefert) | Original-Material |
|---|---|---|
| Räume mit Zone | **96/96** | 67/96 |
| sichtbar rot | **96** | 50 von 67 |
| Marker im eigenen Rechteck | 95/96 | **50/50** |
| Marker im fremden Raum | **0** | **0** |
| Übergänge Median | **2 px** | 11 px |
| innerhalb 8 px | **95 %** | 40 % |
| Audit [F]/[K]/[M]/[N] | 0/0/0/0 | 0/0/0/0 |

Die Kunst sieht richtig aus und ist in sich stimmig; sie kostet ein Drittel der Räume
(die das Original nicht malt) und die Marker-Genauigkeit. Die Umstellung bleibt deshalb
opt-in, bis die gemeinsame Lösung von Zuordnung und Zeile steht.


## §35 Zuordnung aus den Zeilen — gebaut, misst besser, und stößt an die Datenlage

§34 hatte den Schluss gezogen: die Zone→Rechteck-Zuordnung muss **aus den Zeilen** kommen
statt aus einer Kostenfunktion, sonst widersprechen sich Zuordnung und Projektion. Das ist
jetzt gebaut (`gen_map_zones.py`, unter `RE15_KUNST`).

**Verfahren:** jede Zone durch **ihre eigene Zeile** projizieren (ausgeliefert wo
vorhanden, sonst aus der Türkette hergeleitet) und das gemalte Rechteck nehmen, das der
projizierte Kasten am besten trifft — relativ zur *kleineren* Fläche, damit ein Korridor
in einem großen Rechteck zählt, ohne dass das große alles einsammelt.

| | Kostenheuristik | **aus den Zeilen** |
|---|---|---|
| zugeordnete Zonen | 72 | **78** |
| Deckung, Median | — | **98 %** |
| Räume mit Zone | 67 | **75 von 96** |
| davon sichtbar rot | 50 | **61** |
| Marker im eigenen Rechteck | 50/50 | **61/61** |

Die Zuordnung selbst ist damit belastbar — die projizierten Kästen liegen praktisch
deckungsgleich auf der Kunst.

### ⛔ Der Marker wird davon trotzdem nicht gut

| Übergänge (Original-Material) | linear | mit Zeile |
|---|---|---|
| Median | **22 px** | 28 px |
| innerhalb 2 px | 2 % | **12 %** |
| innerhalb 8 px | **29 %** | 24 % |

Die Zeilen-Projektion trifft am *engen* Ende deutlich besser (2 % → 12 % innerhalb 2 px),
verschlechtert aber den Median. Der Grund ist **die Datenlage des Prototyps**, nicht mehr
das Verfahren:

```
Uebergaenge auf demselben Blatt (154):
   beide Raeume vom Original geeicht:  40
   nur einer:                          79
   keiner (beide hergeleitet):         35
```

⇒ **74 % aller Übergänge hängen an mindestens einer hergeleiteten Zeile.** Jede trägt im
Median 7 px Eigenfehler (§30), und an einer Tür addieren sich zwei davon. Das Original
eicht nur **38 von 106** Slots — das Kartensystem ist im Prototyp unfertig, und genau
diese Lücke lässt sich nicht wegrechnen.

### Die Entscheidung, die daraus folgt

| | Grundrisse (ausgeliefert) | Original-Material |
|---|---|---|
| Aussehen | gerechnet, gleichmäßig | **die Original-Kunst** |
| Räume auf der Karte | **96 von 96** | 75 von 96 |
| Übergänge Median | **2 px** | 22 px |
| innerhalb 8 px | **95 %** | 29 % |
| Marker im eigenen Rechteck | 95/96 | **61/61** |
| Audit [F]/[K]/[M]/[N] | 0/0/0/0 | 0/0/0/0 |

Die Grundriss-Lösung erreicht ihre 2 px gerade **weil** sie die unvollständigen
Original-Daten nicht benutzt: sie rechnet die Anordnung aus Kollision und Türgraph und ist
dadurch in sich konsistent. Die Kunst ist authentisch und in sich stimmig, kann aber die
Genauigkeit nicht erreichen — und lässt 21 Räume leer, die das Original nicht malt.

Das ist keine offene Aufgabe mehr, sondern eine **Abwägung**: originalgetreues Bild gegen
brauchbaren Marker.


## §36 Original-Kunst mit brauchbarem Marker — der Stand

Nutzer: *„ich möchte das Original Kartenmaterial nutzen mit Tür und Treppenmarkern von
Resident Evil 2 und du musst es nur gebacken bekommen den Spieler Marker darauf korrekt zu
positionieren..... Wie ist mir egal."* Und richtig erkannt: **genau das tut RE2** — die
Kunst ist das Bild, die Position kommt aus einer eigenen Abbildung.

Die Engine hat dafür längst den Mechanismus: jede Zone kann eine **eigene Abbildung**
tragen (`eichung`, vier Werte in `s_map_zones`) statt ihre Weltbox linear ins Rechteck zu
strecken. Bisher bekamen ihn nur die 38 Räume mit ausgelieferter Zeile.

**Jetzt bekommt ihn jede zugeordnete Zone, deren Zeile bekannt ist** — ausgeliefert oder
aus der Türkette hergeleitet.

| | | Median | ≤ 2 px | ≤ 8 px | Sprünge > 16 px |
|---|---|---|---|---|---|
| Streckung (§35) | 0 Zonen mit Abbildung | 22 px | 2 % | 29 % | 76 |
| Zeile nur wenn Deckung ≥ 80 % | 47 | 22 px | 15 % | 36 % | 72 |
| **Zeile immer** | **57** | **14 px** | **20 %** | **42 %** | **63** |

⛔ **Die Deckungs-Schwelle war ein Fehler.** Ich hatte sie aus Sorge eingebaut, eine
schlecht hergeleitete Zeile könne schaden. Das Gegenteil stimmt: die Zeile ist per
Konstruktion **türkonsistent** — sie *entsteht* aus den Türpunkten —, die lineare
Streckung ist es nie. Default ist deshalb 0; der Hebel `RE15_ZEILEN_DECKUNG` bleibt für
die Nachmessung.

### Zwei weitere Messfehler in derselben Schiene

* `schau()` suchte weiter nur nach `FILL`. Der Türdurchgang meldete deshalb **0 von 143**
  Türen mit rotem Ziel, obwohl 61 Räume ihre rote Kachel zeichneten. Nach dem Fix:
  **94 von 143**.
* Der Op-Dump filterte ebenfalls auf `FILL` (§32) — daraus war der falsche Schluss
  entstanden, die Kacheln würden gar nicht gezeichnet.

Dreimal dieselbe Klasse in einer Sitzung: **die Messschiene kannte nur eine der beiden
Op-Arten.** Wer eine Darstellung umstellt, muss zuerst prüfen, ob das Messwerkzeug die
neue Form überhaupt sehen kann.

### Stand der Original-Karte

| | Grundrisse (ausgeliefert) | **Original-Kunst** |
|---|---|---|
| Räume auf der Karte | 96 von 96 | **75 von 96** |
| davon sichtbar rot | 96 | 61 von 75 |
| Marker im eigenen Rechteck | 95/96 | **61/61** |
| Marker im fremden Raum | 0 | **0** |
| Türen mit beidseitig rotem Raum | 191/191 | 94/143 |
| Übergänge Median | 2 px | **14 px** (von 22) |
| innerhalb 8 px | 95 % | **42 %** (von 29) |

Der Marker sitzt also **immer im richtigen Raum** und nie im falschen — das war die
Kernforderung. Die verbleibende Ungenauigkeit ist der Sprung beim Türdurchtritt.

### Was den Rest noch blockiert

1. **21 Räume malt das Original nicht.** Sie bleiben leer — so wie im Original. Das ist
   keine Lücke im Port, sondern der Zustand des Prototyps.
2. **14 Räume haben gar keine Zeile** (§30), fallen also auf die Streckung zurück.
3. **Nur 38 von 106 Slots sind vom Original geeicht.** 74 % der Übergänge hängen an
   mindestens einer hergeleiteten Zeile mit je ~7 px Eigenfehler (§35). Das ist die
   harte Grenze der Datenlage.


## §37 Umschalten: alles steht bis auf die RE2-Türmarken

Nutzer: *„umschalten"*. Gemacht, gemessen — und **ein** Teil fehlt noch, ausgerechnet der,
den er ausdrücklich mitbestellt hat.

### Was jetzt funktioniert

**Zwei Durchgänge in der Kachel-Schleife.** Die gemalten Rechtecke überlappen einander
stellenweise (der Künstler zeichnet Flure in Säle hinein), und die Op-Liste wird von
hinten gerastert — früher eingetragen heißt oben. In Listenreihenfolge gezeichnet
verschwand das **rote** Rechteck unter einem grünen Nachbarn mit kleinerem Index. Die
Schema-Zeichnungen lösen das seit v0.3.70 mit zwei Durchgängen; die Kachel-Schleife hatte
es nie nötig, solange kein Rechteck einen Zustand trug.

| | vorher | nachher |
|---|---|---|
| Räume sichtbar rot | 61 von 75 | **70 von 75** |
| Türen mit beidseitig rotem Raum | 94 von 143 | **133 von 143** |
| Marker im eigenen Rechteck | 61/61 | **70/70** |
| Marker im fremden Raum | 0 | **0** |

Die fünf Reste sind benannt: **ROOM1060 und ROOM1080 teilen sich ein gemaltes Rechteck**,
ROOM3080/ROOM4020/ROOM50D0 sind Einzelfälle.

### ⛔ Der Blocker: die Türmarken lösen auf der Kunst nicht auf

```
[Symbole] 184 Tueren, 12 mit SICHTBAREM Symbol im Bild
[Symbole] Identitaet 9 | Rueckfall 3 | Zufall 0 | ohne Marke 172
```

Zum Vergleich der Auslieferungsstand: **229 von 229** Türen mit Symbol, 0 ohne Marke.

Die 182 Marken der Tabelle **existieren** — sie verbinden aber die Zonen der
**Grundriss**-Anordnung, nicht die der gemalten Rechtecke. Die Identitäts-Auflösung
(„welche Marke verbindet meine Zone mit einer Zone des Zielraums") findet deshalb nichts.

Eine naheliegende Ursache wurde geprüft und **ausgeschlossen**: die Unterdrückung
`kachel_zeigt_tuer()` (eine Marke entfällt, wenn die gemalte Kachel dort schon eine Tür
zeigt). Abgeschaltet ändert sich **nichts** — sie filterte auf der Kunst ohnehin 0 Marken.

### Warum trotzdem nicht umgeschaltet wurde

Der Auftrag lautete „Original-Kartenmaterial **mit Tür- und Treppenmarkern von Resident
Evil 2**". Eine Karte ohne Türmarken wäre gegenüber dem heutigen Stand (229/229) eine
Verschlechterung — die Umstellung bliebe hinter dem zurück, was bestellt wurde. Sie bleibt
deshalb bis dahin opt-in (`RE15_KUNST=1`); das ist kein Zweifel an der Entscheidung,
sondern die Weigerung, weniger auszuliefern als verlangt.

**Der nächste Schritt ist eng umrissen:** die Marken müssen für die Kunst-Zonen erzeugt
bzw. auf sie umgehängt werden — dieselbe Türkette, aber die Position auf den gemalten
Rechtecken statt auf den Grundriss-Kästen. `snap_wall()` liest dafür schon die gemalte
Kachel (`DATA/MAP0x.PIX`, Palettenindex 0 = außerhalb); das Gegenstück `snap_grundriss()`
wird dort nicht gebraucht.


## §38 Runde zwei: Marken auf die Kunst, Kostenzuordnung als Rückfall

### Die Türmarken lagen auf verschwundenen Kästen

`to_map()` beginnt mit **„GRUNDRISS ZUERST"** — hat der Löser für eine Zone einen
Grundriss geliefert, rechnet die Marke gegen dessen Kasten. Richtig, solange der Grundriss
auch *gezeichnet* wird; der Löser berechnet ihn aber für **alle 117 Orte**, auch wenn die
Kunst gemalt wird. Im Kunst-Stand trugen deshalb **alle 152 Türmarken `rect = 255`**.

Genau der Fehler, den der Kommentar an der Stelle selbst beschreibt — *„sonst rechnen sie
gegen ein Rechteck, das gar nicht mehr gezeichnet wird"* — nur in die andere Richtung.

| | vorher | nachher |
|---|---|---|
| Türmarken auf gemaltem Rechteck | 0 | **124 von 172** |
| Türen mit sichtbarem Symbol | 12 | **152 von 184** |
| davon über die **Identität** | 9 | **88** |

### Die Kostenzuordnung darf nicht ersetzt, nur ergänzt werden

Die Zuordnung aus den Zeilen ist die genauere (Deckung im Median 98 %), deckt aber nur,
wo der projizierte Kasten ein Rechteck über 35 % trifft. Sie zu **ersetzen** statt zu
ergänzen kostete 21 Räume komplett — ein Raum ohne jede Zuordnung ist gar nicht auf der
Karte, und das ist schlechter als eine heuristische. Mit der Heuristik als Rückfall
(18 Zonen):

| | nur Zeilen | **+ Rückfall** |
|---|---|---|
| Räume auf der Karte | 75 von 96 | **89 von 96** |
| davon sichtbar rot | 70 | **80** |
| Türen gesamt / mit Symbol | 184 / 152 | **207 / 164** |
| Marker im eigenen Rechteck | 70/70 | **80/80** |
| Marker im fremden Raum | 0 | **0** |
| Übergänge Median | 14 px | 17 px |

Der Preis sind 3 px Median — die heuristisch zugeordneten Zonen sind ungenauer als die
zeilen-basierten. Für 14 zusätzliche Räume auf der Karte ist das der richtige Tausch.

### Was die 43 Türen ohne Symbol wirklich sind

**Kein Fehlen, sondern Abstand.** Die Prüfung verlangt einen gelben Balken innerhalb
**4 px** vom Punkt, an dem der Spieler steht. Die Marke wird über `snap_wall()` auf der
gemalten Kachel gesetzt, der Marker über die Zonen-Abbildung — beide weichen um dieselbe
Größenordnung ab wie der Übergangs-Median (17 px). Geprüft und ausgeschlossen: die
Zeichenreihenfolge ist nicht schuld, die Marken werden **vor** den Kacheln eingetragen und
liegen damit oben.

⇒ Die letzten Prozente hängen an derselben Grenze wie alles andere: **38 von 106 vom
Original geeichten Slots**.

### Stand gegenüber dem Auslieferungsstand

| | Grundrisse (ausgeliefert) | Original-Kunst |
|---|---|---|
| Räume auf der Karte | **96 von 96** | 89 von 96 |
| sichtbar rot | **96** | 80 von 89 |
| Türen mit Symbol | **229 von 229** | 164 von 207 |
| Marker im eigenen Rechteck | 95/96 | **80/80** |
| Marker im fremden Raum | **0** | **0** |
| Übergänge Median | **2 px** | 17 px |


## §40 ⛔ ROOM1170: der Nutzer-Report trifft eine Zusicherung, die ich stillgelegt hatte

Nutzer 2026-09-04 (`finding/error1`, `finding/error2`), wörtlich:
*„bin ich auf dem roof, ist dort bereits der bereich bei den treppen unten inkl. der 2
Türen mit eingezeichnet, obwohl ich nicht auf der Ebene bin … Gehe ich dann die Treppen
runter, fehlt der Cursor völlig. Hier hätte ich jetzt zwischen den beiden Türen genau den
Bereich erwartet der oben eingezeichnet ist und unten fehlt."*

### Der Befund

```
ROOM1170 z0  Blatt 5 (ROOF)  rect 1     der Dachbereich
ROOM1170 z1  Blatt 5 (ROOF)  rect 0     der UNTERE Bereich - auf dem Dachblatt zu Hause
Etagen-Tabelle: { 0x1170, z1, Band 0 -> Blatt 4, rect 255 }
```

Beide Bereiche sind auf dem **Dachblatt** zu Hause — daher error1. Und die Etagen-Zeile für
Band 0 zeigt auf Blatt 4 mit **`rect 255`**, einem Schema-Kasten, den es auf der Kunst
nicht mehr gibt — daher error2: dort ist ein Loch **und** kein Marker.

Die Gast-Zeilen kamen bis dahin vom Grundriss-Löser. Mit der Umstellung auf die Kunst
entfielen sie ersatzlos.

### ⛔ Und ich hatte genau die Zusicherung weichgemacht, die das schützte

Beim Umschalten (§39) fiel `test_map_re2_system` mit *„und dort liegt eine
Gast-Zeichnung"*. Ich habe daraus *„wenn es eine gibt"* gemacht — mit der Begründung, auf
der Kunst gebe es sie eben nicht. **Das war keine Anpassung an ein neues Modell, sondern
das Abschalten eines Wächters.** Dieselbe Sitzung, dieselbe Datei: `test_map_mark_band`
prüft wörtlich

```
FAIL: das ROOF-Blatt traegt keine Tuer des unteren Bereichs mehr
FAIL: die Treppe erscheint auch auf dem 3F-Blatt
[ROOF Seite 5]      2 Tueren nach draussen, 2 Treppen
[3F Seite 4 Rect 3] 0 Tueren, 0 Treppen
```

— also **exakt** die beiden Punkte des Nutzer-Reports, und es nennt **Rect 3** als die
erwartete Fläche auf 3F. Der Test hat den Fehler nicht nur gefunden, er hat ihn
vorausgesagt.

### Was ein Versuch schon gebracht hat — und woran er hängt

Gast-Zeilen aus der Kunst erzeugt (Anker: die Nachbarn auf dem fremden Blatt, dann die
Form): **13 Gast-Zeilen**, darunter ROOM1170 z1 auf Blatt 4. Die Fläche ist damit nicht
mehr weg. Sie landet aber auf **Rect 5** statt Rect 3, weil **ROOM1120 Rect 3 hält** — und
dessen Anspruch ist datenmäßig gut: seine Projektion liegt bei x148..190, Rect 3 bei
x152..200.

⛔ **Ein Form-Term im Zuordner wurde gemessen und verworfen:** Deckung mit der Ähnlichkeit
der Kantenlängen zu multiplizieren machte es schlechter (Räume 89 → 87, Türen 170 → 154,
beidseitig rot 140 → 126) und bewegte ROOM1120 nicht. Die Position gibt ihm den Anspruch,
nicht die Form.

### Der offene Schritt, eng umrissen

1. **ROOM1170 z1 gehört auf 3F zu Hause**, nicht auf das Dachblatt — sein Band 0 zeigt
   dorthin. Solange es auf Blatt 5 wohnt, ist error1 unvermeidlich.
2. Die Gast-Zeile muss **Rect 3** bekommen; das verlangt, den Konflikt mit ROOM1120
   aufzulösen (Tausch, nicht Vorrang).
3. Die Tür- und Treppenmarken müssen der Zeile auf das 3F-Blatt folgen
   (`blatt_fuer_band` / `to_map`).

Der Zwischenstand wurde **nicht** ausgeliefert: eine Fläche am falschen Ort ist nicht
besser als eine fehlende. Der Auslieferungsstand bleibt v0.5.4, 267/267 grün.


## §41 Die gemalten RE1.5-Türsymbole entfernen: gemessen, so nicht auslieferbar

Nutzer 2026-09-04: *„Teilweise RE 1.5 türen gemischt unter den RE 2 türen … Und die
RE 1.5 Türen sollst du natürlich ALLE entfernen."* Richtig — sie stecken in den gemalten
Kacheln, und seit die Kunst gezeichnet wird, kommen sie mit.

### Der Weg ist da, die Schwelle trägt nicht game-weit

§27 hat das Verfahren an **einem** Blatt gemessen: Wandlinien sind lange gerade Läufe, ein
Türschwenk ist kurz und gekrümmt; Saat „Lauf ≤ 4", Wachstum über Nachbarn mit „Lauf ≤ 10".
Auf Blatt 4 traf das alle 6 Symbole und kein Wandpixel.

Auf die **Kacheltextur** angewandt (nur innerhalb der gezeichneten Rechtecke, sonst frisst
es den unbenutzten Rest):

| Blatt | Wandpixel | entfernt | Komponenten |
|---|---|---|---|
| 0–7, 9–12 | 322–2425 | 3,3–9,1 % | 13–77 |
| **8** | 1568 | **24,9 %** | **317** |

### Warum Blatt 8 ausbricht — und es ist NICHT die Palette

Erste Vermutung: der Wand-Index sei dort ein anderer. **Gemessen und widerlegt** — Index 4
ist auf *jedem* Blatt die Wand (er trägt überall die zweitlängsten Läufe, nach dem
Hintergrund-Index 0):

```
längster Wandlauf je Blatt:  47  53  55  63  67  68  78  80  81  88  98  112
Blatt 8:                     32   <- die Wände sind dort von Natur aus KURZ
```

Die Schwelle „Lauf ≤ 4 / ≤ 10" ist absolut, die Wandlängen sind es nicht. Auf Blatt 8
fällt echte Wand unter die Schwelle. Ein Riegel über die Komponentengröße (ein Schwenk ist
nach §27 13 Pixel) hilft nicht: dort sind es **317 kleine** Komponenten, also wirklich
kurze Wandstücke.

⇒ Die Schwelle muss **relativ zur Wandlänge des Blattes** sein, und jedes Blatt braucht
danach eine Sichtprüfung am gerenderten Bild — so wie §27 es für eines gemacht hat.
Blind ausgeliefert wäre ein Viertel der Wände von Blatt 8 weg; das sähe schlechter aus als
die vermischten Symbole. **Nicht ausgeliefert.**

### Und die Kopie hängt daran

Nutzer: *„Du hast unten in 3F 2 Türen bei der Map von RE 1.5 die nach draußen führen. Das
sind GENAU DIE 2 Türen die du das oben gesehen hast. an die Muss das Rechteck quasi
anschließen."* Der Anker für die Kopie sind also genau die **gemalten** Türen — dieselbe
Erkennung, die für das Entfernen gebraucht wird. Beide Hälften sind eine Aufgabe, nicht
zwei.

Der zweite Grund, warum die jetzige Kopie danebenliegt, ist unabhängig gemessen: die
Zweitfläche sucht sich ein *vorhandenes* Rechteck (40×40) statt die Dachfläche (48×24) in
die Lücke zu setzen. Das passende Rechteck (Blatt 4 Rect 3, exakt 48×24) hält ROOM1120 —
und eine Rangfolge „ausgelieferte Zeile schlägt hergeleitete" greift dort nicht, weil
ROOM1170s ausgelieferte Zeile den unteren Bereich gar nicht auf sein eigenes Dach-Rechteck
projiziert (y174..199 gegen Rechtecke bei y80..157). Die Zuordnung dieser Zone kommt
ohnehin aus einer Heuristik.

## §42 Nutzer-Befund 2026-09-04: zwei falsche Tuermarken auf Blatt 4 (fehler/error1.png)

Zwei Anmerkungen im Screenshot, beide auf Blatt 4 (3F), aufgenommen in ROOM1140.

**Zuordnung am gerasterten Bild, nicht geschaetzt.** Neues Werkzeug
`re15_port/tools/kartenbild.py` zeichnet ein Blatt so, wie der Zeichner es zeichnet
(Rechtecke aus `DATA/MAP0x.PIX`, Marken als 5x2- bzw. 2x5-Balken) und beschriftet jede
Marke mit ihrem Tabellen-Index. Kartenpixel == Bildschirmpixel (die Rechtecke liegen bei
x120..216, y80..166 im 320x240-Bild), der Screenshot ist also nur um 957/320 = 2,99
skaliert. Damit sind beide Anmerkungen eindeutig:

| Anmerkung des Nutzers | Marke | Tabellenzeile |
|---|---|---|
| „Wrong Rotation, Need 90 Degree turn" | **#64** | `{ 4, 4, 146, 85, 2, 16, 18 }` ROOM1130 ↔ ROOM1150 |
| „wrong shouldn't exist" | **#70** | `{ 4, 5, 159, 119, 0, 15, 255 }` ROOM1120, ohne Partner |

**Lage in ihrem Rechteck (gemessen):**

```
#64  rect 4 = (144,80) 32x80   Marke (146, 85):  West  2  Ost 29  Nord  5  Sued 74
#70  rect 5 = (120,119) 40x40  Marke (159,119):  West 39  Ost  0  Nord  0  Sued 39
```

**Zwei naheliegende Filter — beide GEMESSEN und VERWORFEN, bevor sie gebaut wurden:**

* *„ungepaarte Marken (zid2 == 255) sind falsch"* — **98 von 201** Marken sind ungepaart.
  Auf Blatt 4 ist auch **#61** (ROOM1160) ungepaart, und der Nutzer hat sie NICHT
  beanstandet. Der Filter wuerde die halbe Karte loeschen.
* *„die gewaehlte Wand liegt zu weit von der naechsten Rechteckkante"* — **32 von 151**
  Tuermarken auf gemalten Rechtecken liegen ≥ 20 px weiter. Das ist kein Defekt, sondern
  die Bauart: `snap_wall` schnappt auf die **gemalte Wand in der Kachel**
  (`DATA/MAP0x.PIX`, Palettenindex 0 = ausserhalb), nicht auf den Rand der Bounding-Box.
  Die direkt benachbarte #66 (168,89) liegt 70 px „weit" und ist laut Nutzer richtig.

**Mechanismus (gelesen, `gen_map_zones.py:2495` `snap_wall`):** `kind` folgt
ausschliesslich aus `senk`. Ist `senk` wahr, sucht die Funktion senkrechte Kanten und
liefert 1/3 (Ost/West) = senkrechter Balken; ist es falsch, waagerechte Kanten und 0/2
(Nord/Sued) = waagerechter Balken. `senk` selbst kommt aus der Streckung des
Tuer-Trigger-Rechtecks: `senk = m['rd'] > m['rw']`. #64 traegt kind 2, also war `senk`
FALSCH — entweder ist das Rechteck dieses Datensatzes anders als der Code-Kommentar
behauptet, oder DURCHGANG 2 (Paarung) hat den Datensatz des anderen Raums behalten.

### §42.1 Marke #64 — die Achse wurde in der Paarung ueberschrieben

**Beide Tuer-Datensaetze sind sich einig** (gelesen aus den RDTs):

```
ROOM1130 #2 -> ROOM1150   l=( -6650, 16350)  rw=1000 rd=2000   -> senk = rd>rw = True
ROOM1150 #0 -> ROOM1130   l=(-16050,-11500)  rw=1500 rd=2200   -> senk = rd>rw = True
```

Die Achse war in DURCHGANG 1 also richtig (senkrechte Wand). Verloren geht sie in
DURCHGANG 2: dort setzt `_seite_kante` die Achse nur, wenn die Ueberdeckung der beiden
Rechtecke *duenn* ist (`_duenn = min(_br,_ho) <= 4`). Fuer ROOM1130/ROOM1150 ist
`_br = 8` (x) und `_ho = 40` (y) — also `_duenn = False`, `_seite_kante = None`, und
danach entscheidet die Richtung zur **Mitte** des Nachbar-Rechtecks:

```
_zx = 136 - 146 = -10      _zy = 100 - 85 = +15      |zx| < |zy|  ->  Seite 2 = Sued
```

Genau der Muenzwurf, den der Kommentar zwoelf Zeilen tiefer selbst verbietet: *„Bei zwei
langgestreckten Raeumen, die sich ueber Eck beruehren, zeigen die Mitten diagonal und die
Achse wird zur Muenzwurf-Entscheidung … Wo eine Ueberdeckung existiert, gilt sie."* Fuer
Ueberdeckungen dicker als 4 px war dieser Satz nie umgesetzt.

**Fix:** die Achse kommt aus der Ueberdeckung, sobald diese eine laengere und eine
kuerzere Seite hat — unabhaengig von `_duenn`. `_duenn` regelt weiterhin allein die
POSITION (die war getrennt eingemessen). Gleichstand `_br == _ho` bleibt der bisherigen
Entscheidung ueberlassen, weil die Ueberdeckung dort nichts sagt.

### §42.2 Marke #70 — die Lage war nur der Klemm-Anschlag

`to_map` klemmt jede Projektion in ihr gemaltes Rechteck. ROOM1120 (Rect 5 = x120..159,
y119..158, Eichung 101/190/2296/2312) projiziert seine drei Tueren so:

| Tuer | Weltpunkt | rohe Projektion | Ergebnis |
|---|---|---|---|
| #2 → ROOM1130 | (-8450,-2900) | (153,126) | **im Rechteck** → echte Marke #69 |
| #0 → ROOM1060 | (-700, 9550) | (170, 98) | ausserhalb in x UND y → Ecke (159,119) |
| #1 → ROOM1080 | (1300, 6400) | (174,105) | ausserhalb in x UND y → Ecke (159,119) |

Die ROOM1060-Tuer rettet die **Paarung**: sie zieht die Marke auf die gemeinsame Kante mit
ROOM1060s Rechteck (120,151) = Marke #67. Die ROOM1080-Tuer hat keine Gegenseite —
**ROOM1080 traegt null Tuer-Datensaetze** — und blieb auf dem Klemmwert in der Ecke stehen,
direkt an ROOM1140s Rechteck. Genau das las der Nutzer als Tuer zwischen zwei Raeumen, die
keine haben.

**Fix:** verworfen wird, was BEIDES ist — ohne Partner UND diagonal ausserhalb geklemmt.
Eine Klemmung in nur EINER Achse bleibt eine Aussage: die andere Koordinate ist gemessen,
die Tuer sitzt auf dieser Wand.

### §42.3 Nebenbefund: zwei Tueren auf einem Pixel

Der Dubletten-Schluessel enthielt die SEITE. Zwei Marken am selben Punkt mit
verschiedener Achse ueberlebten damit beide und ergaben ein Kreuz aus waagerechtem und
senkrechtem Balken. Gemessen: Blatt 8 Rect 2 (142,83) trug schon vor dieser Aenderung
zwei Tueren DESSELBEN Ortes (zid 67, einmal Nord, einmal West) uebereinander; der
Achsen-Fix haette einen dritten Fall auf Blatt 6 erzeugt. Der Schluessel ist jetzt die
POSITION (Tueren und Treppen getrennt), und bei Kollision gewinnt die **gepaarte** Achse —
sie kennt beide Rechtecke, die ungepaarte nur die eigene Silhouette.

### §42.4 Ergebnis, gemessen

| Groesse | vorher | nachher |
|---|---|---|
| Marken gesamt | 201 | 189 |
| Achse passt zur gemeinsamen Wand (`unit_map_tuerachse`) | — | **23 von 23** |
| Achse WIDERSPRICHT der Wand | **8** | **0** |
| nicht bewertbar (keine echte Wand, siehe Wand-Probe unten) | — | 17 |
| „Symbol zeigt vom Nachbarn weg" (`unit_map_durchgang`) | 1 von 60 | **0 von 65** |
| Stellen mit zwei Marken uebereinander | 2 | 1 |

⛔ **DER ERSTE WURF WAR ZU GROB, und der Pin hat ihn gefangen.** „Die laengere
Ueberdeckungsseite ist die Wandrichtung" liess `unit_map_durchgang` von 1 auf 9
verdrehte Symbole steigen. Sechs der neun Faelle waren zwei Zonen, die sich DASSELBE
gemalte Rechteck teilen (dort ist die „Ueberdeckung" das ganze Rechteck), drei waren
Rechtecke, von denen eines IM anderen steckt. Beides sind keine Waende. Die Probe lautet
deshalb: die kurze Seite der Ueberdeckung muss **kleiner als die halbe Ausdehnung des
kleineren Rechtecks** sein, und die beiden Zeichnungen duerfen nicht identisch sein.
Der Nutzer-Fall besteht sie klar (Ueberdeckung x=8 bei zwei je 32 px breiten Rechtecken);
danach steht der Pin bei **0 von 65** — besser als sein Ausgangswert.

Die verbliebene Doppelstelle ist Blatt 12 Rect 7 (265,118) = **Tuer + Treppe**. Die bleibt
absichtlich: die Treppe zeichnet nur drei Sprossen ohne deckenden Grund, damit die
darunterliegende gemalte Tuer durchscheint (Kommentar zur Treppen-Marke in
`re15_inv_screen.c`).

**Der Klemm-Filter deckt Eichungs-Defekte auf, statt sie zu bemalen.** Er hat 14 Marken
verworfen; jede davon ist eine Tuer, deren Projektion diagonal ausserhalb des eigenen
Rechtecks landet:

```
ROOM1070#0@Blatt2(203,88)   ROOM1120#1@Blatt4(159,119)  ROOM1180#0+#1@Blatt0(153,134)
ROOM2040#4@Blatt6(193,185)  ROOM2050#1@Blatt6(65,124)   ROOM2090#0@Blatt6(164,163)
ROOM3000#0@Blatt7(80,104)   ROOM3050#0@Blatt7(224,128)  ROOM4040#2@Blatt8(154,111)
ROOM4050#4@Blatt8(127,79)   ROOM5040#2@Blatt9(131,128)  ROOM5140#0@Blatt9(161,174)
ROOM6000#0@Blatt12(262,109)
```

⛔ **OFFEN, ehrlich benannt: ROOM1070 verliert damit sein EINZIGES Tuersymbol.** Der Raum
hat genau eine Tuer (→ROOM1030, Weltpunkt (560,6200)); mit seiner Eichung
(79/205/2229/2088) projiziert sie auf (148,129), waehrend sein Rechteck bei x180..227,
y59..90 liegt — 32 px daneben in x und 39 px in y. Das ist ein **Eichungs**-Defekt dieses
Raums, kein Tuer-Defekt. Vorher wurde er von einem Symbol an der Rechteckecke zugedeckt;
jetzt ist er sichtbar. Elf weitere Zonen verlieren je ein Symbol, behalten aber andere.

Neue Schranke: `re15_port/tests/unit/test_map_tuerachse.c` prueft die Achse jeder
gepaarten Tuermarke gegen die Lage ihrer beiden Rechtecke und gibt seine Abdeckung aus.
Neues Werkzeug: `re15_port/tools/kartenbild.py` rastert ein Blatt mit beschrifteten
Marken — damit ist ein Screenshot-Befund des Nutzers eindeutig einer Tabellenzeile
zuzuordnen, statt sie zu erraten.

## §43 Nutzer-Befund 2026-09-04: „does not turn red when i am in the room"

Zweimal derselbe Satz, auf zwei verschiedenen Blaettern (`fehler/error2.png` = ROOF,
`fehler/error3.png` = 3F). Beide Male steht der Spielermarker sichtbar IN dem Rechteck,
und das Rechteck bleibt gruen. Beide Male ist es **ROOM1170s zweiter Bereich** (Zone 1,
zid 21) — auf 3F Blatt 4 Rect 3, auf dem Dach Blatt 5 Rect 0.

**Ursache, eine Zeile.** `re15_map_rect_state` (`re15_map_zones.c:312-320`) entscheidet
die rote Hervorhebung eines stockwerkuebergreifenden Raums so:

```c
hat_etage = re15_map_floor_lookup(zn->room, zn->idx, band, &fp, &fr);
if (hat_etage) {
    if ((int)page == fp && (int)rect_idx == fr) return RE15_MAP_RECT_CURRENT;
}
```

Die Etagen-Tabelle trug pauschal **`rect = 255`**:

```python
floors = [(b, zi, band, zp, 255) for (b, zi, band, zp) in ETAGEN]
```

Fuer ein GEMALTES Rechteck (Index 0..13) kann `rect_idx == 255` nie zutreffen.
**Gemessen: ALLE 22 Etagen-Zeilen trugen 255**, waehrend ihre Zonenzeile ein echtes
Rechteck nennt — kein einziger stockwerkuebergreifender Raum konnte je rot werden:
ROOM1060, ROOM1080, ROOM10A0, ROOM10F0, ROOM1170, ROOM11A0, ROOM3080, ROOM4020,
ROOM4070, ROOM50D0.

Die 255 war ein **Ueberbleibsel**: sie stammt aus der Zeit, als diese Zweitzeichnungen
Schema-Zeichnungen aus der Kollisionsbox waren (der Kommentar daneben sagt das auch so).
Seit die Original-Kunst der Auslieferungsstand ist, traegt die Gast-Zeile ein gemaltes
Rechteck — die Etagen-Zeile wurde nie nachgezogen.

⛔ **Warum es so lange unentdeckt blieb: der Marker war nie betroffen.** Er holt sein
Rechteck ueber `re15_map_zone_fuer` aus der ZONE (`re15_inv_screen.c:655`) und
ueberschreibt `fr`. Genau dieser Widerspruch — Marker drin, Rechteck gruen — steht in
beiden Screenshots. Zwei Wege lasen dieselbe Sache aus zwei verschiedenen Tabellen, und
nur einer war gepflegt (vgl. Memory `reai-v2-doppeltes-byte`).

**Fix:** die Etagen-Zeile nimmt das Rechteck aus der Zonenzeile derselben (Raum, Zone,
Blatt). Neue Schranke `re15_port/tests/unit/test_map_etagen_rect.c` prueft fuer jede Zone
und jedes Band: nennt die Etagen-Tabelle die Seite dieser Zone, muss sie auch ihr
Rechteck nennen — plus die zwei gemeldeten Faelle namentlich als Pin.

---

## §44 — Blatt 3 (2F) traegt im Original die Tabelle von Blatt 2 (1F)

**Nutzer-Befund 2026-09-04** (`fehler/error1.png`, `fehler/expect.png`): auf 2F steht das
Treppensymbol auf leerem Blau, "da fehlt das komplette Rechteck". Dazu: "ich haette das
Treppenhaus woanders erwartet, naemlich wie in expect.png dargestellt."

### Was gemessen wurde

1. **Die beiden Seiten-Tabellen sind byte-identisch.** Das Paar `{count, Zeiger}`
   @`0x80076840 + 8*Seite` zeigt fuer Blatt 2 auf `0x8007636C` (11 Eintraege) und fuer
   Blatt 3 auf `0x800763F0` (10 Eintraege). Zwei verschiedene Adressen — aber die zehn
   Eintraege von Blatt 3 sind Byte fuer Byte die ersten zehn von Blatt 2:

       Blatt 2  b4 00 45 00 20 00 60 00 00 00 20 00   x=180 y= 69  32x96  uv(  0,32)
       Blatt 3  b4 00 45 00 20 00 60 00 00 00 20 00   x=180 y= 69  32x96  uv(  0,32)
       … alle zehn gleich, Blatt 2 hat zusaetzlich
       Blatt 2  77 00 86 00 18 00 18 00 a8 00 10 00   x=119 y=134  24x24  uv(168,16)

   Zwei Stockwerke koennen nicht dieselbe Kachelgeometrie haben. Das ist eine
   Kopie, die nie fertig geschrieben wurde — dieselbe Baustelle wie die
   Massstabstabelle @`0x800768b0`, von der 65 von 103 Zeilen der Stub `{0,0,1,1}` sind.

2. **Jedes `DATA/MAP0x.PIX` enthaelt zweierlei.** Oben (bis Zeile ~128) ein Streifen aus
   einzeln gezeichneten Raum-Kacheln — das ist die Quelle, die der Zeichner
   FUN_80046fd8 @`0x800472fc-0x800473e0` per SPRT abtastet (uv aus Byte +8/+10,
   `AddPrim` @`0x80047608`). Darunter der fertige **Grundriss** des Stockwerks. Baut man
   eine Seite nach ihrer Tabelle zusammen, kommt genau dieser Grundriss heraus:

       Blatt 2 (1F)  Jaccard 0,930   (10234/10999 Bildpunkte)
       Blatt 4 (3F)  Jaccard 0,987   ( 5776/ 5855)
       Blatt 3 (2F)  Jaccard 0,573   ( 6860/11974)   <- kaputt

   Legt man die zehn uv-Kaesten auf das 2F-Blatt, schneiden sie mitten durch Raeume,
   einer liegt auf leerem Schwarz, und die untere Haelfte des 2F-Streifens wird nie
   abgetastet. Auf dem 1F-Blatt umschliesst jeder Kasten genau einen Raum.

3. **Der Nutzer hat recht.** `expect.png` ist der Grundriss aus der unteren Haelfte von
   `MAP04.PIX` — die fertige 2F-Zeichnung, die das Original nie zeigt.

### Ersatztabelle (PORT-ERGAENZUNG, `RECT_FIX` in `tools/gen_map_zones.py`)

Der 2F-Streifen enthaelt 10 Raumkacheln (Zusammenhangskomponenten >= 80 px, ohne
Kompass und Massstabsleiste) — genau so viele Rechtecke fuehrt die Seite.

* `uv` = Ecke der Komponente; `w`/`h` = ihre Groesse auf das naechste Vielfache von 8
  aufgerundet. So schneidet das Original: an allen 7 Kacheln von Blatt 4 und an 8 von
  11 auf Blatt 2 nachgemessen.
* Die Bildschirmlage kommt aus dem **Grundriss desselben Blatts**: jede Kachel wird dort
  per Schablonensuche wiedergefunden — 9 von 10 zu 100,0 % der eigenen Bildpunkte, eine
  zu 94,2 % (dort verschmilzt im Grundriss eine Nachbarwand).
* Bildschirm = Fundstelle + Versatz `(-23,-74)`. Der Versatz ist die **einzige
  Port-Wahl** an dieser Stelle: er setzt die Seite auf die Mitte, die die beiden
  intakten Blaetter belegen (Blatt 2 `(165,124)`, Blatt 4 `(168,123)`).

**Gegenproben.**
* Der Zusammenbau trifft den Grundriss des Blatts zu **93,1 %** Jaccard — so gut wie die
  echte Tabelle von Blatt 2 (93,0 %).
* Die beiden Treppenhaus-Kacheln landen auf `(120,131)` und `(111,131)`. Blatt 2 setzt
  **dieselben Schaechte** (ROOM1060/ROOM1080, uv(168,16) und uv(168,40)) auf `(119,134)`
  und `(109,134)` — 1 bis 3 px daneben, obwohl in die Rechnung nichts von Blatt 2
  eingegangen ist.

### Engine

`re15_map_rect_geometry`/`re15_map_rect_uv` lesen die Seiten-Tabelle nur noch, wenn fuer
die Seite keine Ersatzzeile existiert; neu ist `re15_map_rect_count`. Der Zeichner in
`re15_inv_screen.c` und die Marker-Kastenbestimmung lasen die Tabelle vorher direkt —
beide gehen jetzt ueber die Zugriffsfunktionen, damit Zeichner, Marken und
Zonen-Zuordnung dieselben Rechtecke sehen.

---

## §45 — Der Generator liest seinen eigenen vorherigen Kopf

Beim Nachmessen von §44 sprang die Zonenzahl von 108 auf 91 — und zwar auf Blaettern
(6, 7, 9, 10, 11, 12), die mit der 2F-Ersatztabelle nichts zu tun haben. Die
Kontrollmessung "derselbe Generator, `RE15_KEIN_RECT_FIX=1`" lieferte eine **dritte**
Zahl: 104.

**Ursache.** `gen_map_zones.py` importiert `gen_marker_zeilen`, das `karte_audit`
importiert, und `karte_audit.py:26` liest `re15_port/engine/src/re15_map_zones.h` — den
Kopf, den `gen_map_zones.py` selbst schreibt. Ein Lauf haengt also am Ergebnis des
vorigen. `gen_map_zones` ueberschreibt zwar `_GM.BLATT` und `_GM.ZONEN_N` mit genau
dieser Begruendung (Kommentar bei `KUNST_VOR`), aber `karte_audit.ZONEN` bleibt die
Quelle fuer alles Uebrige. Sichtbar an einer einzigen Zeile bei sonst identischem
Protokoll:

    Lauf A:  Zuordnung AUS DEN ZEILEN: 38 Zonen (Median 100 %), 42 aus der Heuristik
    Lauf B:  Zuordnung AUS DEN ZEILEN: 76 Zonen (Median  97 %), 18 aus der Heuristik

Alle uebrigen Protokollzeilen (Zuordnung je Seite, Kosten, Grundrisse, Ueberlappungen)
sind zwischen den Laeufen identisch — die Kostenheuristik selbst ist deterministisch
(`random.Random(9000 + pg)`).

**Folge.** Der ausgelieferte Kopf ist irgendein Zwischenstand dieser Folge; der
committete Kopf und der committete Generator gehoeren nicht zusammen (der Generator
sortiert heute 19 Schablonen-Bereiche aus, der Kopf entspricht 12). Jedes
"vorher 22, nachher 16" ohne Fixpunkt ist Rauschen aus zwei Punkten der Folge.

**Vorgehen ab jetzt.** Den Generator bis zum **Fixpunkt** laufen lassen (wiederholen,
bis die md5 des Kopfes gleich bleibt) und beide Seiten eines Vergleichs auf ihrem
eigenen Fixpunkt messen. Ein einzelner Lauf ist kein Messwert.

**Zweiter Fund derselben Untersuchung.** `kunst_zuordnung.rechtecke()` las die
Rechteck-Tabelle roh aus der EXE und kannte die Ersatztabelle nicht — es leitete die
Marker-Zeilen fuer Blatt 3 also gegen die 1F-Kacheln her, waehrend der Port die
2F-Kacheln zeichnet. Beide gehen jetzt ueber `karte_audit.rects`/`rect_uv`, also ueber
dieselben Zugriffe wie der Generator.

### Ergebnis am Fixpunkt

Der Generator konvergiert nach vier Laeufen (md5 von Lauf 3 == Lauf 4,
`97486f104b1c3e97ddac0e97b3f862dc`): 107 Zonen, 76 davon mit der ausgelieferten
Massstabszeile @`0x800768b0` zugeordnet.

Gegen den ausgelieferten Stand v0.6.1 (108 Zonen in 88 Raeumen):

    FIXPUNKT mit Ersatztabelle:  107 Zonen in 87 Raeumen
    nur im ausgelieferten Stand: ROOM4040 (Blatt 8)

⛔ Der befuerchtete Kollateralschaden war ein Artefakt der Rueckkopplung: ein einzelner,
nicht konvergierter Lauf hatte 91 Zonen in 73 Raeumen gemeldet und 16 verschwundene
Raeume auf Blaettern, die mit 2F nichts zu tun haben. Am Fixpunkt ist es **ein** Raum -
ROOM4040 auf Blatt 8, also weder das gemeldete Blatt noch dessen Blatt-Komponente
({1,2,3,4,5}). Er bleibt als OFFEN vermerkt; ob er am Fixpunkt OHNE Ersatztabelle
zurueckkaeme, ist nicht gemessen (das kostet einen zweiten Fixpunkt-Lauf).

Live gemessen (`integration_map_raum_live`, ganze Raumkette ab EXE-Start):

    161 Tueren durchschritten, 161 mal war VORHER und NACHHER der richtige Raum rot
     87 Raeume geprueft, 87 zeigen SICHTBARES Rot, 0 nicht
     87 von 87 Spieler-Marker liegen in der roten Flaeche

Marken auf gemaltem Grundriss (Zusammenbau der Seite, `unit_map_marke_auf_kunst`):

    ausgeliefert v0.6.1:  173 Marken, 13 auf leerer Flaeche, Blatt 3 zwei von sechs
    Fixpunkt:             177 Marken,  8 auf leerer Flaeche, Blatt 3 eine von neun

Die eine verbliebene auf Blatt 3 ist die Treppenmarke an der UNTERKANTE ihres Rechtecks
(118,157) - zwei Pixel unter der gezeichneten Flaeche, nicht mehr freischwebend ohne
Rechteck. Zum Vergleich die Erwartung des Nutzers aus `expect.png`: sein Pfeil zeigt auf
Kunstpunkt (151,215), was mit dem Versatz (-25,-71) dem Bildschirmpunkt (126,144)
entspricht - das Treppenhaus-Rechteck liegt jetzt bei (118,134) 24x24, also genau darum
herum.

Suite: 271/271 gruen (neu: `unit_map_marke_auf_kunst`).

---

## §46 — 2F: der Flur ohne Rechteck und der Raum am falschen Ende

**Nutzer-Befunde 2026-09-05** (`fehler/error01.png`, `fehler/error02.png`):
"sobald ich eine Tuer weiter gehe in den Flur werde ich auf der Map woanders hin
geportet" und "wenn ich noch weiter gehe in 2F und aus dem Korridor raus, geht garnichts
mehr bei der Map. Ich werde noch nicht einmal mehr angezeigt".

Beides auf demselben Blatt, das §44 gerade erst richtiggestellt hat - und beide Male ist
die Zuordnung durch die Daten ERZWUNGEN, nicht gewaehlt.

### Der Tuergraph der Etage

Aus den Door_aot_set-Records:

    ROOM1060 (Treppenhaus) -- ROOM10C0 -- ROOM10D0 -- { ROOM10E0, ROOM10F0, ROOM1100 }
    ROOM10C0 -- ROOM1080 (Fahrstuhlkabine)          ROOM1100 -- ROOM1110

Zwei Raeume, die eine Tuer verbindet, muessen Rechtecke haben, deren gezeichnete
Flaechen sich beruehren. Zaehlt man ALLE Zuordnungen auf, die jede Tuerkante erfuellen
(ROOM1060 auf Rect 1 und ROOM1080 auf Rect 4 sind aus §44 gesetzt), bleiben **192**
uebrig - und in **192 von 192** liegt

    ROOM10C0 auf Rect 0   (das einzige, das Treppenhaus UND Kabine beruehrt)
    ROOM10D0 auf Rect 3

Die uebrigen vier Raeume sind mit 12-25 % unbestimmt und werden deshalb NICHT
festgesetzt - dieselbe Schwelle wie bei der Vorgabe aus §22.

### Warum ROOM10D0 vorher GAR NICHT gezeichnet wurde

ROOM10D0 misst im ausgelieferten Massstab 63 x 91 px. Mit der alten, von Blatt 2
kopierten Tabelle war das groesste Rechteck der Seite 72 x 64 - der Raum passte in
keines, und die Kostenheuristik liess ihn lieber ganz weg (`KEINE_ZUORDNUNG` = 30 ist
bewusst billig). Ohne Zone gibt es weder rote Hervorhebung noch Spielermarker; genau das
hat der Nutzer fotografiert. Die Ersatztabelle aus §44 fuehrt Rect 3 mit **72 x 88** -
das erste Rechteck der Seite, in das der Flur passt.

### Ein Raum faellt nicht mehr ganz weg, wenn Rechtecke frei sind

`KEINE_ZUORDNUNG` bleibt bei 30 (das A/B dort gilt weiter). Neu ist nur: die Option
"gar nicht zuordnen" steht der Suche nur noch dort zur Verfuegung, wo es **weniger
freie Rechtecke als Zonen** gibt (Blatt 6: 13 Zonen auf 11 Rechtecke). Wo es reicht -
Blatt 3 hat 10 Rechtecke fuer 6 Zonen -, bekommt jede Zone eines. Ein falsch
platzierter Raum ist fuer den Spieler immer noch besser als ein unsichtbarer.

### Ergebnis am Fixpunkt (Lauf 2 == Lauf 3)

    Blatt 3:  rect 0 ROOM10C0 | rect 1 ROOM1060 | rect 2 ROOM10F0 | rect 3 ROOM10D0
              rect 4 ROOM1080 | rect 6 ROOM10E0 + ROOM1100 | rect 7 ROOM1110

    Live (integration_map_raum_live, ganze Raumkette ab EXE-Start):
        178 Tueren durchschritten, 178 mal war VORHER und NACHHER der richtige Raum rot
         91 Raeume geprueft, 91 zeigen SICHTBARES Rot, 0 nicht   (vorher 87/87)
         91 von 91 Spieler-Marker liegen in der roten Flaeche
        219 Tueren, 171 mit sichtbarem Symbol im Bild

    Marken auf gemaltem Grundriss (unit_map_marke_auf_kunst, Zusammenbau der Seite):
        ausgeliefert v0.6.1  13 von 173 = 7,5 %   Blatt 3: 2 von 6
        Fixpunkt             12 von 180 = 6,7 %   Blatt 3: 1 von 10

⛔ Die Schranke dort misst jetzt die QUOTE. Eine Absolutzahl haette genau das bestraft,
was hier besser wurde: es werden mehr Raeume gezeichnet, also gibt es mehr Marken.

Suite: 274/274 gruen.

## §47 — Zwei Turmarken auf 2F: die Kabine im Flur und die Marke ohne Rechteck

Nutzer-Befund 2026-09-05 (`fehler/error.png`, Blatt 3): drei Pfeile, zwei Ursachen.

### (a) "This door is wrong" + "Staircase 2F only has one door!" — dieselbe Marke

Beide Pfeile zeigen auf ROOM10C0s Tuer zur **Fahrstuhlkabine ROOM1080**. Belegt an
der erzeugten Tabelle (`s_map_rectfix`, Blatt 3) und am Zonen-Kopf:

| Rect | Lage            | Zone |
|------|-----------------|------|
| 0    | (102,116) 40x40 | ROOM10C0, zid 11 |
| 1    | (118,134) 24x24 | ROOM1060, zid 7 (Treppenhaus, auch Blatt 2 und 4) |
| 3    | (135,76) 72x88  | ROOM10D0, zid 12 |
| 4    | (109,134) 16x16 | ROOM1080, zid 9 (Kabine, auch Blatt 2 und 4) |

Rect 4 liegt **vollstaendig** in Rect 0 (x 109..124 in 102..141, y 134..149 in
116..155). `snap_wall` rueckt eine Turmarke auf die naechste Kante der **eigenen**
Kachel — bei einem Rechteck IM Rechteck gibt es diese gemeinsame Kante nicht, die
Grenze IST der Rand des inneren. Die Marke landete deshalb bei **(126,134) als
Sued-Tuer**: zwei Pixel neben der Kabine und genau auf der **Oberkante von Rect 1**,
dem Treppenhaus (dessen y bei 134 beginnt). Optisch eine zweite Tuer am Treppenhaus,
das auf 2F nur eine hat.

Fix: nach `snap_wall` ein zweiter Schritt. Er sucht das Rechteck der **Zielzone** der
Tuer (`m['dest']`) auf demselben Blatt und prueft, ob es vollstaendig im eigenen
liegt; nur dann wird die Marke auf den naechsten Punkt von dessen Rand gezogen.
Beruehrende Rechtecke bleiben unberuehrt — dort trifft `snap_wall` die gemeinsame
Wand bereits. Ergebnis **(124,134), kind 1 (Ost)** — die rechte Kante der Kabine.
Diff gegen v0.6.4: `{3,0,126,134,2,...}` -> `{3,0,124,134,1,...}`.

### (b) "This door is flying - not on the wall/Edge"

Marke #51, Blatt 3, `zid 12` (ROOM10D0) / `zid2 11` (ROOM10C0), Lage (141,117).

Die **Lage** einer Turmarke entsteht aus der Projektion **ihrer** Zone auf **deren**
Rechteck — hier Rect 3, der Flur. Der Spieler hatte ROOM10D0 nicht betreten, Rect 3
wurde also nicht gezeichnet. Die alte Sichtbarkeitsregel zeigte die Marke trotzdem:
sichtbar, sobald **eine** der beiden Zonen besucht ist.

⛔ **Der erste Fix haette nicht gereicht, und das ist der Kern.** Ich pruefte, ob die
Marke im **Kasten** des Partner-Rechtecks liegt — und das tut sie: (141,117) liegt in
Rect 0 (102,116) 40x40. Ein Kasten ist aber nur die Bounding-Box; die Kachel darin
enthaelt Schwarz. An (141,117) traegt Rect 0s Kachel **Index 0**, also nichts
Gemaltes. Gemalt ist die Stelle nur in Rect 3s Kachel.

Der zweite Versuch — Sichtbarkeit nur ueber die **eigene** Zone — machte
`unit_inv_fsm` und `unit_map_re2_system` rot: auf dem Dach-Blatt verschwand eine
korrekt sitzende Tuer, deren eigene Zone kein Rechteck hat.

Fix: der Generator rechnet je Marke das Bit **`auf_partner`** aus — liegt sie auf der
GEMALTEN Flaeche (Kachel-Index != 0) des Partner-Rechtecks? Gemessen ueber alle 70
Marken mit Partner-Rechteck: **47 ja, 23 nein**, darunter genau #51. Die Regel in
`re15_map_mark_get` zeigt eine Marke, wenn ihre eigene Zone besucht ist — oder wenn
`auf_partner` gilt und die Partnerzone besucht ist.

### Schranke

`test_map_marke_haengt_an.c`, ueber drei Besuchsstaende, zwei Aussagen:
1. **hart (0):** keine sichtbare Marke gehoert zu einem Rechteck, dessen Raum
   unbesucht ist. Gegenprobe mit der alten Regel gefahren — sie schlaegt an.
2. **Nicht-Verschlechterung:** Marken auf dem eigenen, gezeichneten Rechteck, aber
   neben der gemalten Flaeche. 2F-Anfang **2 -> 1** (die entfallene ist #51),
   3F-Kette 0 -> 0, alles betreten 5 -> 5.

Die verbleibenden 5 sind eine andere Klasse: eigener Kasten, aber ausserhalb der
Zeichnung darin — auf Blatt 3 das Treppensymbol bei (118,157), zwei Pixel unter der
gemalten Flaeche (§44). **Offen, nicht gefixt.**

## §48 — Der Testhaken wartete auf einen Wrapper, nicht auf das Spiel

Nebenbefund beim v0.6.5-Lauf: `integration_relatch_pin` lief in die Zeitueberschreitung,
obwohl das Spiel fertig war. Letzte Zeile im `debug.log`:
`[flow] BOOT_EXIT_AT: boot #3 reached -> exit` — danach ruft der Haken
`re15_testhaken_ende()`, also `fclose(stderr)` + `_exit(0)`.

Drei Messungen, gleiche Umgebung, gleiches Arbeitsverzeichnis:

| Start | Ergebnis |
|-------|----------|
| exe direkt aus der Shell | `exit=0` nach 23/24/26 s (3 von 3) |
| `execute_process` + `cmake -E env` | `Process terminated due to timeout` (120 s) |
| `execute_process` + `set(ENV{...})` | `rv=0` nach 62 s |

Der Unterschied ist allein der Zwischenprozess: `execute_process` wartet, bis die
Ausgabe-Roehren geschlossen sind, und `cmake -E env` haelt sie ueber das Ende des
Kindes hinaus offen. Seit das Spiel im Testhaken mit `_exit(0)` endet (2026-09-04,
gegen eine andere Flatterhaftigkeit) laeuft kein `atexit`-Abbau mehr, und der Wrapper
haengt sichtbar.

⛔ **Das war NICHT nur ein Zeitproblem der beiden roten Haken.** Alle vier
`-E env`-Haken wurden umgestellt (`tests/integration/spiel_lauf.cmake`), und auch die
vorher gruenen wurden dabei schneller:

| Haken | vorher | nachher |
|-------|--------|---------|
| boot_bg_pin | (gruen) | 7.3 s |
| dark_start_pin | (gruen) | 6.3 s |
| relatch_pin | Zeitueberschreitung 220 s | 24.9 s |
| save_counter_pin | 123.7 s | 21.4 s |

Der Wrapper hat also auch bestandene Laeufe aufgeblaeht — die gemessene Dauer eines
Haken sagte bis hierher mehr ueber ihn als ueber das Spiel.

## §49 — Der Ankunfts-Marker steht nicht an seiner Tuer (Nutzer-Befund 2026-09-05, 2F)

Zwei Bilder, ein Grund. `fehler/Screenshot 2026-09-05 233339/233357.png`: *„when I go
through this door, i expect to get out on the left of it — but I get out here."*
`…233429.png`: *„Instead I land here somewhere and all door are wrong positioned."*

### Gemessen

Fuer jede Tuer laesst sich vergleichen, wo der **Ankunfts-Marker** landet (Projektion
der Ziel-Weltposition auf das Ziel-Rechteck) und wo ihr **gezeichnetes Tuersymbol**
sitzt. Gepaart wird ueber `zid`/`zid2`, also Marke zu Tuer, nicht „naechstes Symbol"
(das schwaechere Mass sah mit Median 5 px harmlos aus — s. [[reai-v2-proxy-mass]]):

| Projektion | n | Median | 90 %-Wert | schlimmster |
|------------|---|--------|-----------|-------------|
| geeicht (Original-Zeile @0x800768b0) | 62 | 2 px | 5 px | 41 px |
| Stub (Bbox-Streckung, Port-Ergaenzung) | 19 | 2 px | **39 px** | **67 px** |

Die beiden Nutzer-Faelle stehen in dieser Liste ganz oben:

* **ROOM10C0 -> ROOM10D0: Marker (200,117), Symbol (141,117) — 59 px.** Das ist der
  zweitschlimmste Fall im ganzen Spiel und genau der gelbe Strich, auf den der zweite
  Pfeil in `233429.png` zeigt.
* **ROOM1060 -> ROOM10C0:** Tuersymbol (119,151), Ankunft (123,122) — **29 px** weiter
  oben, am oberen Rand des Rechtecks. Das ist der erste Befund: er geht unten links
  hinaus und erscheint oben.

### Ursache

Der Marker hat drei moegliche Wege, und fuer 2F greift der schlechteste:

1. `re15_map_zone_abbildung` (affine Abbildung des Grundriss-Loesers) — **greift nie**,
   `s_map_synth[]` ist leer; sie ist nur fuer Zonen OHNE gemaltes Rechteck vorgesehen.
2. Die Original-Formel `FUN_800473f8` mit `ox/oy/sx/sy` — nur fuer die 33 Raeume mit
   ausgelieferter Zeile.
3. **Lineare Bbox-Streckung** — Port-Ergaenzung ohne Original-Vorbild.

Auf Blatt 3 tragen **6 von 8** Zonen `sx = 0`: ROOM10C0, ROOM10D0, ROOM10E0, ROOM10F0,
ROOM1100, ROOM1110. Nur Treppenhaus (ROOM1060) und Fahrstuhl (ROOM1080) sind geeicht.
Die Streckung bildet die ganze Weltbox linear auf das Rechteck ab — sie weiss nichts
davon, wo die Tuer in der ZEICHNUNG liegt.

### Was daraus folgt

⛔ Eine allgemeine Eichung aus den Tuersymbolen ist NICHT verfuegbar: von 39 nicht
geeichten Zonen haben ueberhaupt nur **6** eindeutige Anker (Welt-Ankunft ->
gezeichnetes Symbol), davon **2** mindestens drei — ROOM1000 (Blatt 2, Rect 10) und
**ROOM10D0** (Blatt 3, Rect 3). Fuer diese beiden ist eine bestimmte Eichung mit
Kreuzprobe (ein Anker herausgehalten) moeglich; fuer die uebrigen fehlen die Daten.

⛔ Und der frueher schon gemachte Fehler darf sich nicht wiederholen: am 2026-09-01
wurde eine Eichung „aus der gezeichneten Flaeche und den Tuersymbolen" zurueckgenommen,
weil sie 37 von 37 geeichten Zonen klemmen liess. Eine neue Eichung darf deshalb
ausschliesslich dort greifen, wo das Original KEINE Zeile fuehrt, und muss ueber die
Kreuzprobe belegt werden — der Fit auf die eigenen Anker ist sonst eine
selbstbestaetigende Metrik ([[reai-v2-selbstbestaetigende-metrik]]).

**Stand: gemessen und eingegrenzt, nicht gefixt.**

## §50 — Die Tuermarke gehoert auf die gemeinsame GEMALTE Wand, nicht auf die Kastenkante

Fortsetzung von §49. Nutzer-Befund 2026-09-06: *„all door are wrong positioned."*

### Was ich zuerst falsch gedeutet habe

Die Marken-Spur (`RE15_MARKEN_DUMP=10d0`) zeigt fuer ROOM10D0:

    projiziert:  Tuer #0 (184,156)   #1 (198,132)   #2 (202,117)   #3 (139,90)
    endgueltig:      (184,156)          (198,130)       (141,117)     entfaellt

Tuer #2 wandert 61 px. Ich habe daraus geschlossen, mein v0.6.5-Fix habe eine
falsch liegende Marke bloss VERSTECKT. ⛔ **Das war voreilig.** Der Sprung ist kein
Fehler, sondern der bewusste Schritt „gepaarte Marke auf die gemeinsame Kante beider
Rechtecke" (§ Kommentar `_seite_kante`), und (141,117) ist ROOM10C0s rechte
Kastenkante — also die Kante, die es mit ROOM10D0 teilt.

Auf Pixelebene nachgesehen (Kacheln aus `DATA/MAP04.PIX`, Streifen x135..141):

    y=117   Rect 0 (ROOM10C0): 4 0 0 0 0 0 0      Rect 3 (ROOM10D0): 4 1 1 1 1 1 1

Bei (141,117) malt ROOM10D0 seinen Boden (Index 1), ROOM10C0 **nichts** (Index 0).
Das ist genau, was das `auf_partner`-Bit sagt — der v0.6.5-Fix war richtig.

### Der echte Fehler

Die gemeinsame **gemalte** Wand liegt bei **x=135** (beide Kacheln malen dort Index 4).
Die Marke stand bei x=141, also sechs Punkte weiter im Nachbarboden. Ursache: der
ganze Platzierungsschritt rechnet mit den RECHTECKEN; die Zeichnung darin endet
frueher. Dieselbe Unterscheidung wie bei `auf_partner` — der Kasten ist nicht die
Malerei.

**Gemessen ueber alle gepaarten Marken** (Abstand zur naechsten Stelle, an der BEIDE
Kacheln malen):

| | vorher | nachher |
|---|--------|---------|
| auf der gemeinsamen Wand (0 px) | 21 von 70 | **58 von 71** |
| 3 px oder weiter weg | 30 | **12** |
| Median | 1 px | **0 px** |

### Fix

Letzter Schritt nach der Paarung: die Marke rueckt auf den naechsten Punkt, an dem
beide Kacheln etwas malen — hoechstens `RUECK_MAX = 12` px weit. Darueber hinaus gibt
es keine gemeinsame Wand (die Raeume beruehren sich dort gar nicht), und ein Zug ueber
diese Entfernung waere geraten statt gemessen. Gerueckt wurden 43 Marken, Median 6 px,
schlimmster 12 px. ROOM10C0 <-> ROOM10D0: **(141,117) -> (135,123)**.

Nebeneffekt: die Marke traegt jetzt `auf_partner = 1`, weil sie auf der Wand liegt, die
BEIDE Raeume malen. Sie muss also gar nicht mehr verborgen werden.

### ⛔ Warum die obige Tabelle NICHTS beweist

Sie ist genau das Mass, auf das der Fix optimiert — eine selbstbestaetigende Metrik
([[reai-v2-selbstbestaetigende-metrik]]). Der Beleg sind zwei **unabhaengige**
Schranken, die etwas anderes fragen (liegt die Marke auf der gemalten Flaeche ihres
EIGENEN Rechtecks) und auf die nicht optimiert wurde:

| Schranke | vorher | nachher |
|----------|--------|---------|
| `unit_map_marke_auf_kunst`, Marken auf leerer Flaeche | 12 von 180 (6,7 %) | **8 von 181 (4,4 %)** |
| `unit_map_marke_haengt_an`, sichtbar freistehend | 5 | **4** |

Beide Schranken wurden auf den erreichten Stand nachgezogen UND gegengeprueft: mit dem
alten Kopf schlagen sie an (`FAIL: … sind 12 von 180 = 6.7 %`). Suite 276/276.

## §51 — Warum Tueranker einen FLUR nicht eichen koennen

Nach der Wand-Korrektur (§50) sitzen die Tuersymbole richtig — und dadurch tritt die
zweite Haelfte des Nutzer-Befunds hervor. Gemessen, Ankunfts-Marker gegen die eigene
Tuermarke:

|            | n  | Median | 90 % | schlimmster |
|------------|----|--------|------|-------------|
| geeicht    | 62 | 4 px   | 11 px | 41 px |
| Stub       | 19 | 4 px   | 39 px | **71 px** |

⛔ Der Nutzer-Fall ROOM10C0 -> ROOM10D0 ist von 59 px auf **71 px** GESTIEGEN. Nicht
weil etwas kaputtging: das Symbol ist an seinen richtigen Platz gerueckt (135,123),
der Marker steht weiter bei (200,117), weil er aus der Bbox-Streckung kommt. Der Fix
hat die Zahl schlechter gemacht und die Lage besser — genau die Klasse
[[reai-v2-defekt-schmeichelt-metrik]], hier mit umgekehrtem Vorzeichen.

### Die Eichung aus Tueransankern ist prinzipiell unmoeglich

Mit den nun korrekten Marken lassen sich Anker bilden: Tuerlage im eigenen Raum (RDT)
gegen Markenlage (Kunst). Fuer ROOM10D0 ergibt der exakte Fit durch die drei Anker:

    mx = 0.040614*wx - 0.019882*wz + 10.34
    my = -0.004754*wx + 0.000288*wz + 160.84
    Achsen 24 bzw. 50 Welteinheiten je Punkt, Winkel zwischen ihnen 174.2 Grad

Unbrauchbar — und der Grund ist strukturell, nicht datenbedingt: die drei Tuerpunkte
(800,-7050), (6750,4350), (8650,11400) liegen fast auf einer Geraden (Oeffnungswinkel
12,5 Grad). **Ein Flur hat seine Tueren entlang seiner Achse.** Quer dazu sagen sie
nichts, also ist die Abbildung in der Flaeche nicht bestimmt. Mit drei Ankern und drei
Unbekannten je Achse waere sie ausserdem exakt und damit ungepruefbar
([[reai-v2-selbstbestaetigende-metrik]]).

### Was der Maszstab schon hergibt

Die bereinigte Weltbox von ROOM10D0 ist 32145 x 41362 auf 72 x 88 px = **446 bzw. 470**
Welteinheiten je Punkt — nahezu gleich. Der MASSSTAB der Bbox-Streckung stimmt also.
(Die rohe Kollisionsflaeche misst 42350 x 62500, aber darin stecken die
23950x17000-Schablonen-Rahmen aus [[reai-v2-karte-schablonen-zonen]].)

Was der Streckung fehlt, ist Drehung/Spiegelung und der richtige Versatz, wenn die
gemalte Form ihren Kasten nicht ausfuellt — ROOM10D0 ist ein L-foermiger Flur in einem
72x88-Kasten.

### Der Weg, der bleibt

Registrierung der **Kollisionsflaeche** gegen die **gemalte Kachel**: beide sind
Flaechen, beide sind Originaldaten, und die Deckung bestimmt Massstab, Drehung,
Spiegelung und Versatz ueberbestimmt. Die Tueranker werden dabei NICHT verwendet — sie
bleiben als unabhaengige Gegenprobe uebrig, und genau daran waere der Erfolg zu messen
(Stub-Zeile oben: Median 4, 90 % 39, schlimmster 71).

**Stand: gemessen und begruendet, nicht gebaut.**

### Nachtrag zu §51: der Formvergleich traegt (so) NICHT

Erster Versuch, die begehbare Flaeche gegen die gemalte Kachel zu legen — ROOM10D0,
80534 begehbare Punkte aus der ENGINE (`RE15_FLOOR_DUMP`, nicht nachgebaut), acht
Achsenlagen, reine Bbox-Streckung:

| Bezug | Bester | Zweitbester | Zufallsniveau | jetzige Einstellung |
|-------|--------|-------------|---------------|---------------------|
| rohe Punkte-Bbox | flip 1/1: **48 %** | 46 % | 29 % | 0/0: 13 % (schlechteste) |
| bereinigte Zonen-Box | 1/1/tausch: **36 %** | 34 % | 29 % | 0/0: 21 % |

⛔ Der erste Bezug sah nach einem klaren Befund aus ("die Zeichnung ist in beiden
Achsen gespiegelt, der Port nimmt an, sie sei es nicht"). Auf die bereinigte Box
eingeschraenkt bleiben **2 Punkte** Abstand zwischen Bestem und Zweitbestem, bei 29 %
Zufallsniveau. Das ist kein Signal, sondern Rauschen — die 48 % waren ein Artefakt der
rohen Bbox (sie enthaelt die Schablonen-Rahmen).

**Warum es scheitert:** eine reine Bbox-Streckung hat keine Freiheit in Massstab und
Versatz. Fuellt die begehbare Flaeche ihren Kasten anders aus als die Zeichnung ihre
Kachel, verschiebt die Streckung alles, und jede Achsenlage landet nahe am Zufall.
Eine echte Registrierung braucht die Suche ueber Massstab UND Versatz.

**Damit ist der Weg nicht widerlegt, aber auch nicht belegt.** Nicht darauf aufbauen,
bevor eine Registrierung mit Massstab/Versatz gemessen ist.

## §52 — Drei Wege zur Stub-Eichung, alle gemessen gescheitert

Nutzer-Befund 2026-09-06 (`fehler/error.png`, `error2.png`, zehn Anmerkungen). **Alle
zehn betreffen ROOM10C0 oder ROOM10D0** — beides Stub-Raeume. Aus dieser einen Ursache
folgt beides: der Ankunfts-Marker („I land in nowhere") und die Tuerlagen („this door
shouldn't exist" / „here I would expect a door" — die Tueren existieren, sie werden nur
falsch platziert, weil ihre Lage aus derselben Abbildung stammt).

### Wie gross das Problem ist

ROOM10D0, 14547 begehbare Standplaetze, jetzige Abbildung (Bbox-Streckung), Marker
gegen die GEMALTE Flaeche:

| Toleranz | daneben |
|----------|---------|
| 0 px | 11549 (79 %) |
| 2 px | 10483 (72 %) |
| 4 px |  9364 (64 %) |
| 6 px |  8205 (56 %) |

Der Marker ist ein 8x8-Feld; erst ab ~4 px faellt etwas auf. **Auch dann sind es 64 %** —
das ist kein Rundungsfehler, sondern eine durchgehend falsche Abbildung.

### Weg 1: Eichung aus Tueransankern — geometrisch unmoeglich

ROOM10D0s drei Tueren liegen fast auf einer Geraden (Oeffnungswinkel 12,5 Grad): ein
Flur hat seine Tueren entlang seiner Achse. Der exakte Fit liefert Achsen im Winkel von
174 Grad und Massstaebe 24/50 statt ~460. Mit drei Ankern und drei Unbekannten je Achse
waere er ausserdem ungepruefbar. (§51)

### Weg 2: Formvergleich mit reiner Bbox-Streckung — kein Signal

Acht Achsenlagen, begehbare Flaeche (80534 Punkte aus der ENGINE) gegen die Kachel:
Bester 36 %, Zweitbester 34 %, Zufallsniveau 29 %. Zwei Punkte Abstand sind Rauschen.

### Weg 3: Formvergleich MIT Massstab- und Versatzsuche — sieht gut aus, ist es nicht

Suche ueber Spiegelungen, Achsentausch, Massstab 0,7..1,45 und Versatz ±16 px:

    Bester:  79 % Deckung (flip 1/1, Skala 0,80, Versatz -12/+16)  bei 29 % Zufall

⛔ **Die unabhaengige Probe verwirft ihn.** Die Tueranker wurden fuer die Deckung NICHT
benutzt und dienen deshalb als Gegenprobe — mittlere Abweichung der drei Tueren:

| Kandidat | Tuer 1 | Tuer 2 | Tuer 3 | Mittel |
|----------|--------|--------|--------|--------|
| 79 %-Deckung (flip 1/1, 0,80) | 67 | 73 | 28 | **56,0** |
| jetzige Bbox-Streckung        | 33 | 19 | 88 | **46,7** |

Die „beste" Registrierung ist **schlechter** als das, was schon drin ist. Die 79 % waren
Ueberanpassung: die Form wird in die gemalte Flaeche hineingequetscht, ohne die
Geometrie zu treffen.

### Der Zusammenhang, der das erklaert

Im ORIGINAL tragen diese Raeume die Stub-Zeile `{0,0,1,1}`, mit der die Formel
`FUN_800473f8` **jede** Weltposition auf **(0,0)** abbildet. Das Kartensystem des
Prototyps ist fuer sie unfertig — es gibt hier also kein Original-Verhalten
nachzubauen. Die Bbox-Streckung ist bereits eine PORT-ERFINDUNG, und die Frage ist
nicht „byte-true oder nicht", sondern welche Erfindung die bessere ist.

**Stand: drei Wege gemessen und verworfen. Nicht weiter darauf aufbauen.**

### Weg 4: derselbe Formvergleich, aber BEIDSEITIG bewertet — auch nicht

⛔ Weg 3 hatte einen Bewertungsfehler: gemessen wurde „Anteil der Flaeche, der auf
Gemaltem liegt" — das belohnt SCHRUMPFEN, weil eine kleine Form leicht ganz auf Farbe
passt. Richtig ist die beidseitige Deckung (Schnitt durch Vereinigung), die auch
bestraft, wenn gemalte Flaeche unbedeckt bleibt.

    Bester:  IoU 0,53 (flip 1/1, Skala 1,50, Versatz +1/+7)

Unabhaengige Probe an den Tuerankern (fuer die Deckung nicht benutzt):

| Kandidat | Mittel |
|----------|--------|
| IoU 0,53 | 66,3 px |
| IoU 0,52 (zwei Varianten) | 48,3 px |
| **jetzige Bbox-Streckung** | **46,7 px** |

Auch mit richtiger Bewertung kein Gewinn. **Damit ist der Formvergleich erledigt: die
begehbare Flaeche und die gezeichnete Kachel sind NICHT durch eine Aehnlichkeits-
abbildung verbunden.** Die Karte des Prototyps ist ein SCHEMA, kein massstaeblicher
Grundriss — dieselbe Einsicht, die schon in `tools/grundriss.py` steht („Die Karte ist
nur ein SCHEMA, das davon ausgeht, dass ein Raum an den anderen haengt").

**Vier Wege gemessen, vier verworfen.** Was bleibt, ist Handarbeit: je Raum zwei bis
drei bekannte Punkte, aus denen die Abbildung BESTIMMT wird.
