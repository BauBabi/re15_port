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
