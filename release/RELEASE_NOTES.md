# v0.8.13 - 2026-09-26

Ihr Zwoelfer-Batch. Neun Befunde sind behoben, einer ist ausdruecklich nicht gebaut
worden, und bei einem weiss ich die Ursache noch nicht — das sage ich lieber, als es zu
verschweigen. Suite 346/346.

## Was Sie sofort merken

**Der Fahrstuhl faehrt.** Der Knopf war nie das Problem: 2F stempelt die Etage, startet
das richtige Unterprogramm und meldet den Zielraum an. Im *selben Bild* hat der Port
zusaetzlich die 1F-Tuer gefeuert und die Anforderung ueberschrieben. Schuld war ein
Zweig, den der Port erfunden hatte und den das Original nicht kennt: er liess jede
rechteckslose Tuer ohne Tastendruck losgehen, sobald eine Zwischensequenz lief — und die
Schleife nahm immer die erste, also 1F. Der Zweig ist raus. Der zweite Fahrstuhl hatte
denselben Defekt und faehrt jetzt auch.

**Die Beretta M93R laedt wieder nach.** Der Port hatte im Nachlade-Tor eine Bedingung,
die es im Original nicht gibt: er merkte sich beim Zieleintritt, ob Sie ein Messer in der
Hand hatten, und diese Merkung blieb haengen. Danach verschluckte er jedes Nachladen
stumm — die Waffe feuerte 15, 12, 9, 6, 3, 0 und dann passierte gar nichts mehr. Das
Original merkt sich nichts, es liest die angelegte Waffe jedes Bild neu.

**Die erste Cutscene-Animation wiederholt sich nicht mehr.** Der Port parkte die
Raum-Eintritts-Pose als Dauerbewegung und spielte sie im Kreis. Das Original setzt sie
genau einmal und gibt im selben Durchlauf weiter. Sichtbar war es nur bei Cutscenes, weil
der Spieler dort die Pose nicht selbst ersetzt — deshalb genau Ihre beiden Faelle,
ROOM1170 nach der Tuer und ROOM1050 beim Eintritt.

**Die komischen Bluteffekte in ROOM11D0 sind weg.** Die Kaefig-Freigabe der Hunde
schreibt einen Zustand, den es in der RE1.5-Tabelle als "aufstehen, dann jagen" gibt — in
der RE2-Tabelle steht an derselben Stelle aber ein Zustand, der Blut spritzt. Der Port
faehrt fuer Hunde die RE2-Tabelle und landete deshalb im falschen Eintrag. Betroffen war
nicht nur der Zwinger, sondern auch zwei Raeume in Stage 3.

**Munition stapelt beim Aufnehmen**, und beim Ja der Aufnahme kommt der
Bestaetigungston.

**Das Maschinengewehr macht deutlich mehr Blut.** RE2 gibt SMG und Gatling einen eigenen
Trefferhandler mit zufaelliger Groesse statt der festen der Pistole; der Port hatte dort
den kleinsten Blutstoss des Spiels.

**Der Fahrstuhl hat seinen Fahrton** — aus RE2 importiert, weil RE1.5 ihn nachweislich
nirgends hat. Und das Tastenfeld im Heizungsraum klickt jetzt wie RE2s Schalterraetsel,
mit rotem Leistungszeiger, der bei richtiger Loesung auf 80 steht.

## Wo ich Ihnen widersprechen muss

**Der Bestaetigungston ist keine Regel in RE2.** Sie hatten gesagt: wenn RE2 das so macht,
machen wir das auch — und genau das habe ich nachgezaehlt. Ergebnis: von 128 Ja/Nein-
Abfragen toenen beim Ja nur **49**. Der Ton haengt nicht an der Abfrage-Routine, sondern
an einem Byte, das *pro Nachricht im Text* steht. Der Beweis ist huebsch: "A police
station map. Will you take it?" ist stumm, das wortgleiche "A police B2 map. Will you
take it?" toent.

Fuer Ihre Stelle ist RE2 aber eindeutig: die Aufnahme-Abfrage toent, also toent sie jetzt
auch bei uns. Die Wegwerf-Abfrage ist in RE2 stumm und bleibt es. Und die uebrigen 90
Abfragen im Spiel bekommen nichts — dafuer gibt es kein Vorbild, das waere erfunden.

**Munition stapelt in RE2 auch nicht.** Ich habe es zweimal unabhaengig nachgezaehlt: es
gibt genau einen Einfuegepfad, und der nimmt stur den ersten freien Platz — in RE1.5 wie
in RE2. Sie wollen es trotzdem, das ist voellig in Ordnung, und es ist gebaut. Aber im
Code steht es als Ihre Entscheidung und nicht als Original, und die Rechnung dahinter
(Summe, Deckel, Rest auf einen neuen Platz) ist aus RE2s Menue-Zusammenlegung geborgt
statt erfunden.

**Die Wegwerf-Zeile "item" gibt es nirgends.** Weder RE1.5 noch RE2 haben eine Variante
ohne "key". Gebaut ist sie, aber ueber einen Test am ausgelieferten Namen — keine Liste
von Gegenstandsnummern im Code.

## Was ich nicht gebaut habe, und warum

**Die Hunde bleiben, wie sie sind.** Ihr Befund lautete: erst wieder verwundbar, sobald
sie stehen. Ich hatte einen Riegel im RE2-Hund gefunden, der genau das tut — und war
ueberzeugt, dass er Ihren Fall trifft. **Er tut es nicht.** Gemessen: der Hund geht beim
gewoehnlichen Kugeltreffer sehr wohl zu Boden, traegt dabei aber kein Riegel-Bit; in 240
Bildern nach einem Pistolentreffer steht es kein einziges Mal. Der Riegel gehoert zum
Sprung und zu Feuertreffern.

Haette ich ihn trotzdem scharf geschaltet, waere es schlimm geworden: im Zwangstest loest
er sich nur beim Hund. Bei Kraehe und beiden Spinnen in je 600 Bildern nie — die drei
waeren dauerhaft unverwundbar geworden. Genau in diese Falle ist dieses Projekt schon
einmal gelaufen.

Behoben sind stattdessen drei echte Luecken im Hunde-Verhalten gegenueber RE2. Drei
weitere, die ich einbauen wollte, waren **falsch verortet** — ihr Schreibzugriff geht gar
nicht auf den Gegner, sondern auf sein Opfer bzw. auf den Spieler. Als Gegner-Code gebaut
haetten sie die Unverwundbarkeit selbst erzeugt.

**Ihre Beobachtung ist damit nicht erklaert, nur eine falsche Erklaerung ausgeschlossen.**
Der naechste Verdacht ist die Laenge der Hinfall- und Aufsteh-Animation gegenueber dem
13-Bilder-Zaehler: ist unsere laenger als RE2s, wird der Hund sichtbar im Liegen wieder
verwundbar, obwohl der Zaehler stimmt. Das messe ich als Naechstes.

## Zwei Fragen an Sie

1. **Der Fahrstuhl-Ton toent auch im zweiten Fahrstuhl** (A-2 ELEVATOR). Der Ausloeser
   kommt aus den Daten, nicht aus einer Raumnummer, und die Signatur ist dort bitgleich.
   Im Original ist er an beiden Stellen stumm. Soll er dort spielen oder nicht?
2. **Der Zeiger im Heizungsraum** ist eine 2D-Marke, kein 3D-Objekt wie in RE2 — RE1.5
   hat dort kein passendes Modell, es haette erfunden werden muessen. Die Mechanik (ein
   Punkt je Bild, Ziel 80) ist 1:1 RE2. Die Gewichtung je Schalter musste ich waehlen,
   weil RE1.5 die Loesung ganz anders prueft als RE2. Sagen Sie Bescheid, wenn es anders
   aussehen soll.

## Nebenbefunde, nicht repariert

Drei Integrationstests, die die echte Spiel-exe starten, fallen in Volllaeufen sporadisch
aus — jedes Mal ein anderer, einzeln laufen sie durch. Sieht nach einem Wettlauf beim
Fensteraufbau aus. Ausserdem: die Munitions-Halbierung erfasst auch die Memory Card, und
Ada startet in ROOM1050 ihren Laufzyklus mittendrin neu.
