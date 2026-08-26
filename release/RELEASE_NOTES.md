# RE1.5 Port — v0.3.34 (Early Preview)

Drei Meldungen behoben, eine davon war ein Fehler, den Sie andersherum gesehen haben.

## Ada bleibt wieder stehen

> "Die folgenden NPCs, wie Ada, bewegen sich immer, sie bleiben nie stehen und idlen damit
> nie. Das ist im Original anders."

Dem Port fehlte ein ganzer Unterzustand der Eskorte — der, in dem sich die Begleiterin auf
den Spieler eindreht. Und ausgerechnet der ist der einzige Rueckweg in den Ruhezustand.
Ohne ihn kreist sie ewig zwischen Gehen und Halbtempo-Gehen; stehenbleiben konnte sie gar
nicht. Der Halte-Ausloeser (unter 300 Einheiten Abstand, bei einer Figur unter 100) fehlte
ebenfalls.

Beides ist jetzt drin, samt dem Dreh-Schritt mit Totband, den das Original dafuer benutzt.
Gemessen: sie kommt an, dreht sich ein, geht in den Ruhe-Clip und bewegt sich ueber 30
Bilder um keine Einheit mehr.

## Die Haende im Gitter

**Sie fahren jetzt sichtbar zurueck.** Bisher sprangen sie in einem einzigen Bild an ihren
Platz — man sah es schlicht nicht. Jetzt fahren sie die Ausfahr-Bewegung rueckwaerts ab, mit
deren eigenen Schrittweiten: vier Bilder, exakt dieselbe Strecke zurueck wie hin.

**Man kann sie abschiessen.** Nach zwei bis fuenf Pistolentreffern taucht eine Hand ab und
kommt nicht wieder. Der Grenzwert ist keine erfundene Zahl: das Spiel fuehrt fuer diesen
Gegner ein Trefferbudget von eins bis vier, und jede Waffe zieht ihren eigenen Betrag davon
ab. Die Lebenspunkte spielen dabei keine Rolle — der Zaehler gewinnt vorher, genau wie im
Original.

**Und man trifft sie jetzt ueberhaupt.** Das war der eigentliche Fehler hinter Ihrer
Meldung "die Arme treffen nicht immer" — es war andersherum: nach dem ersten Treffer war
eine Hand fuer alle weiteren Schuesse unsichtbar. Von 19 Pistolenschuessen landete gemessen
genau einer. Das Spiel loescht am Ende der Zuck-Animation eine Markierung, die der Port
stehen liess. Mit dieser einen Zeile: fuenf von fuenf.

## Was noch offen ist

Der Griff zieht Leon im **RE2-KI-Modus** weiterhin in die Wand. Das ist jetzt immerhin
deterministisch reproduzierbar und die Ursache steht fest: die dafuer geliehene
RE2-Ringkampf-Animation traegt eine eigene Bewegung, die ihn ueber mehrere Bilder hinter die
Kante zieht (gemessen: 236 von 308 Griff-Bildern in der Wand, tiefster Punkt 1095 Einheiten
dahinter). Im RE1.5-Modus passiert das nicht — dort landet er statt dessen rund 1900
Einheiten vom Fenster weg, was das schiefe Trefferbild erklaert. Zwei Reparaturversuche
haben nicht gegriffen; ich baue keinen Code ein, der nachweislich nichts tut.

**Tests:** 244/244 gruen (lokal und im Docker-Linux-Build), zwei neue Wachen.

---

# RE1.5 Port — v0.3.33 (Early Preview)

## Stage 6: der Hintergrund war schwarz — und zwar in jedem Raum

> "Die Stage 6 Sachen laedt er irgendwie Raum und Hintergrund nicht, wenn ich den im Debug
> Mode anwaehle."

Dahinter stecken zwei voellig verschiedene Dinge.

### Der Hintergrund — behoben

Der Port lud Raum-Hintergruende nur aus einem vorgeschnittenen Verzeichnisbaum. Der ist
unvollstaendig, und zwar drastisch: Stage 1 hat 78 Verzeichnisse, Stage 2 hat 24, Stage 3
hat 30, Stage 4 hat 22, Stage 5 hat **zwei** — und Stage 6 **gar keins**. Fuer Stage 6 gab
es schlicht nichts zu laden.

Die Originaldateien liegen dagegen fuer alle Stages vollstaendig im Paket, eine je
Raumpaar. Der Lader nimmt sie jetzt als Rueckfall und schneidet den passenden Kamera-Blick
selbst heraus. Das behebt Stage 6, Stage 5 und die Luecken in 2, 3 und 4 auf einen Schlag —
ohne eine einzige neue Datei.

Die Schnittregel ist nicht geraten: ich habe sie gegen den **gesamten** vorhandenen Baum
geprueft. 1688 Schnitte, alle byte-identisch mit dem selbst gerechneten Ausschnitt, null
Abweichungen. Der Rueckfall zeigt also nachweislich dasselbe Bild wie vorher der Baum.

### Der Raum — kein Fehler, sondern der Auslieferungsstand

34 der 240 Raumdateien sind 4-Byte-Platzhalter: Raeume, die es im Prototyp nicht gibt. In
Stage 6 sind das drei der acht Eintraege, die das Debug-Menue anbietet. Wer die anwaehlt,
bekommt zu Recht nichts — dort ist nichts. Der Port meldet das sauber und stuerzt nicht ab.

Eine Wache haelt jetzt beides fest: dass **jeder** der 2188 Kamera-Schnitte aller Stages
eine Quelle hat, und dass es genau 34 Platzhalter sind. Faellt die Zahl, ist eine Raumdatei
verloren gegangen; steigt sie, wurde ein echter Raum ueberschrieben.

**Tests:** 243/243 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.32 (Early Preview)

## Die Dokumente haben jetzt Bilder

Bisher war der FILE-Schirm reiner Text — so, wie RE1.5 ihn baut. RE2 zeichnet dagegen je
Seite ZWEI Bilder: oben die Textseite ueber die volle Breite, darunter die Illustration.
Genau das ist jetzt portiert.

Die Geometrie kommt aus RE2s eigenem Leser und ist Anweisung fuer Anweisung nachgelesen:
die Seitenhoehe H = 256 minus dem y-Versatz aus dem Dokument-Record, die Textseite 256
breit auf den Zeilen 0 bis H-1, die 128 breite Illustration darunter auf H bis 255. Beides
zusammen ergibt die bekannte RE2-Ansicht: Titel oben, Gegenstand darunter.

**Alle 25 Dokumente mit ihren 191 Textseiten und 25 Illustrationen liegen im Paket.**
Sie sind die Vorlage fuer eigene Dokumente: gleiche Namen, gleiches Format, fertig. Die
Seitenhoehe liest der Port aus dem Bild selbst, nicht aus einer Tabelle — ein selbst
gebautes Dokument darf also eine eigene Hoehe haben, ohne dass am Code etwas nachgezogen
werden muss.

Zum Ansehen: `RE15_DOC=0` waehlt Dokument 0 (Chris's Diary), `RE15_DOC=9` ein anderes.
Ohne diese Auswahl aendert sich am FILE-Schirm nichts — er bleibt der Textleser von RE1.5.

Editierbare Vorlagen als PNG erzeugt `python re15_port/tools/re2_files_png.py`.

### Was dabei geprueft wird

Eine Wache vergleicht zwei voellig getrennte Quellen: die Seitenhoehe, die im Bild steht,
gegen die Seitenhoehe, die die RE2-Programmtabelle nennt. Bei allen 25 Dokumenten stimmen
sie ueberein. Dazu: alle 25 haben Titelseite und Illustration, es sind 191 Textseiten, und
bei Dokument 0 liegt das Motiv der Illustration vollstaendig im unteren Bereich — genau da,
wo die Zeichnung es abtastet.

**Tests:** 242/242 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.31 (Early Preview)

Ein Fix, und der war eine Regression von mir aus v0.3.30.

## Der Griff zieht Leon nicht mehr in die Wand

> "Die Zombies in 1210 ziehen Leon immer noch durch die Wand wenn sie ihn grabben. [...]
> Leon darf, wenn er gegrabbed wird, nicht in die Wand gezogen werden, sonst kann man ihn
> im normalen Raum nicht mehr bewegen."

In v0.3.30 hatte ich das Verschwinden beim Griff behoben, indem ich Leon an den Arm ankerte —
so, wie es RE2s Zombie tut. Nur steht RE2s Zombie frei im Raum, und der Arm steckt in der Wand.
Der Anker war damit ein Punkt hinter dem Mauerwerk.

Gemessen: die Lunge endet bei x = -16420, die begehbare Flurkante liegt auf derselben Zeile bei
x = -18164. Der Anker lag also 1744 Einheiten hinter der Wand. Leon landete auf -17312 und
wanderte ueber die naechsten Bilder auf -17731 weiter hinein — jedes Bild auf einem Punkt, den
die Kollision gar nicht erlaubt. Nach dem Loesen stand er dort fest.

Der Anker ist jetzt die **Hand** statt des Rumpfes — derselbe Punkt, an dem das Griff-Tor
ohnehin schon misst. Dort passiert der Griff ja auch: der Arm reicht durch die Gitterstaebe,
sein Koerper bleibt dahinter. Zusaetzlich laeuft die Platzierung fuer diesen einen Gegner-Typ
durch dieselbe Wandklemme wie die Spielfigur selbst, damit sie ihn unter keinen Umstaenden
hinter die Kollisionsgrenze setzen kann.

Nachher gemessen: Leon auf -18983 bis -19402, jedes Bild im begehbaren Flur.

Eine Wache haelt das jetzt fest: von 290 gemessenen Griff-Bildern steht Leon in **null** in der
Wand. Und die aeltere Abstands-Wache, die dabei ansprang, habe ich nicht etwa gelockert — sie
mass gegen den Rumpf, und der Unterschied war exakt der Hand-Versatz (3710 + 1671 = 5381). Sie
misst jetzt gegen die Hand, mit unveraenderter Schranke, plus einer zweiten gegen den Rumpf.

## Was noch NICHT behoben ist

> "Ausserdem clippen sie teilweise vollstaendig durch die Wand [...] es soll lediglich der
> obere Teil bei den Gitterstaeben durchsichtig sein."

Das ist offen. Was ich messen konnte: die Arme verlassen die Wand **geometrisch nie** — ihr
groesster Ueberstand ueber die begehbare Flurkante ist 0, der Rumpf endet 1744 Einheiten davor
und nur das Mesh ragt weiter. Es ist also kein Positions-, sondern ein Verdeckungsproblem.
Die Daten dafuer sind vollstaendig vorhanden: ROOM1210 fuehrt 399 Vordergrund-Masken in 8 von
9 Kamera-Schnitten, und alle acht zugehoerigen Atlanten liegen im Paket.

Warum die Wand den Rumpf trotzdem nicht ueberdeckt, konnte ich in dieser Sitzung nicht
feststellen — dafuer muesste ich das Bild sehen, und die Bildschirmaufnahme liefert hier nur
schwarze Frames. Der naechste Schritt steht fest: die Mask-Rechtecke der betroffenen Schnitte
gegen die Bildschirmflaeche der Arme rechnen.

**Tests:** 241/241 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.30 (Early Preview)

Vier Meldungen, alle vier aus dem letzten Paket. Zwei davon hatte ich in v0.3.29 schon
„behoben" gemeldet — sie waren es nicht, und zwar aus zwei ganz verschiedenen Gründen.

## Die schwarzen Dreiecke über dem Feuer — jetzt an der richtigen Stelle

> „Die Schwarzen Dreiecke sind noch immer über den Feuer"

In v0.3.29 hatte ich die Bank-Wahl aufgeräumt und angenommen, das sei die Ursache. Es war die
falsche Baustelle. Die richtige steht im Original an einer Stelle, an der ich vorher nicht
gesucht hatte: das Spiel entscheidet **pro Körperteil**, ob es überhaupt gezeichnet wird —
Bit 0 der Part-Flags. Der Zeichner steigt ohne dieses Bit aus, *bevor* er ein einziges Dreieck
ausgibt (`andi v0,v1,0x1` / `beq v0,zero,…` @0x8001ecc4-c8), und der Feuer-Emitter löscht sich
das Bit in seinem eigenen INIT (@0x801165d0-e4).

Die sieben Feuer in ROOM1090 sind also **unsichtbare Träger** für ihre Flammen-Effekte. Ihr
eigenes Modell ist ein einzelnes Dreieck — ein Bone, drei Punkte, eine Fläche. Genau das hat
der Port gezeichnet. Jetzt trägt jeder dieser Emitter die Maske und wird übersprungen; die
Flammen selbst bleiben unberührt, weil sie an anderen Aufrufen hängen.

Damit das nicht ein drittes Mal passiert, prüft eine Wache jetzt beides: dass alle sieben
Emitter die Maske tragen — und dass ein normaler Zombie sie **nicht** bekommt.

## Ada und die Kiste

> „Ada kann noch durch die Kiste laufen"

In v0.3.29 hatte ich die **Wand** repariert und gemeldet „Ada kollidiert wieder". Die Kiste ist
ein anderes System, und davon hatte der Port nur die Hälfte.

Der Objekt-Durchlauf des Originals schiebt nämlich nicht nur den Spieler aus den Kisten,
sondern **jeden aktiven Aktor** — die Schleife steht direkt hinter dem Typ-Handler
(@0x8002be0c-4c) und hat keinen Typ-Filter. Ausgenommen ist nur, wer ein bestimmtes Flag-Bit
trägt; die NPC-Familie trägt es nicht. Der Port hatte von dieser Funktion nur den
Spieler-Abschnitt portiert, deshalb lief jeder Gegner und jede Begleiterin durch jede Kiste.

Der Durchgang läuft jetzt mit — und zwar unmittelbar hinter der Gegner-Schleife, weil er im
Original ebenfalls **nach** der Bewegung im selben Bild liegt. Hätte ich ihn an seine
port-interne Stelle gesetzt, wäre er Adas Zug immer ein Bild hinterher gewesen.

## Die Arme im Gitter — Distanz und Verhalten wie in RE2

> „Die Arme kommen raus, wenn man noch zu weit weg ist, das sollte von Distanz und Verhalten
> her wirklich so sein wie bei Resident Evil 2."

Hier ist der Befund unangenehm ehrlich: **das Original hat pro Arm gar kein Abstands-Tor.** Ich
habe die ganze Kette gelesen. Ein raumfestes Rechteck am Flureingang schaltet über
`Member_set(12, 1)` in *einem* Bild alle zehn Arme scharf; der Arm liest diesen einen Wert
(@0x8010c614-28), und seine Logik-Tabelle für den Ausfahr-Zustand ist ein leeres `jr ra`
(@0x8010c70c) — die Ausfahr-Bewegung läuft danach ohne jede Bedingung ab. Der Wurzel-Tick
rechnet den Spielerabstand sogar aus und wirft ihn weg (@0x8010c27c).

Das Tor im Port ist also eine Zutat — es existiert nur, weil Du das Original-Verhalten (alle
zehn auf einen Schlag) verworfen hast. Und **auch RE2 hat kein Ausfahr-Tor**: sein verankerter
Greifer hat genau eine Abstandsprüfung, und die ist der *Zugriff* — Radius 1300
(`sltiu s0,s0,0x514` @0x80102f3c), dazu ein Sektor und der Ein-Angreifer-Riegel.

Was ich daraus gemacht habe: die beiden alten Zahlen fliegen raus. Sie trugen als einzige
Konstanten dieses Blocks keine Fundstelle — 850 war die halbe Tiefe des Raum-Rechtecks, auf
jeden einzelnen Arm umgehängt, und **11000 war schlicht die Flurbreite**. Deshalb reagierte
jeder Arm, egal wie weit Du von *seiner* Wand entfernt warst. Genau das hast Du gesehen.

An ihre Stelle tritt RE2s Zahl in RE2s Form: **ein Radius von 1300**, nicht ein flurbreites
Rechteck. Weil die Arme mit ihrem Ursprung in der Wand sitzen und der begehbare Flur rund 4200
Einheiten daneben liegt, zeigt der Radius auf die **ausgefahrene Hand** statt auf den Ursprung
— sonst ginge er nie auf. Das ist der eine Punkt, an dem ich von der Vorlage abweiche, und er
steht so im Code.

Gemessen an der Wand entlang: der kleinste Abstand zwischen Spieler und Hand liegt bei 72 bis
288 Einheiten, höchstens **zwei** Arme sind gleichzeitig ausgefahren (vorher drei) — und die
Reihe auf der **gegenüberliegenden** Flurseite bleibt jetzt still. Das war mit dem alten Tor
gar nicht möglich.

## Wenn die Arme Leon treffen

> „Wenn Leon von den Armen getroffen wird, verschwindet er und es gibt keine Greif Animation
> oder sowas."

Gemessen: Leon sprang im Moment des Zugriffs von (-18164,-5897) auf (-892,0) — 16508 Einheiten
weit. Ursache war ein fehlender gemeinsamer Ankerpunkt. Die Opfer-Animation wird **absolut**
platziert, ausgehend von einem Anker, den der Greifer setzt und den das Original auf den
Spieler kopiert. Beim Arm wurde er nie gesetzt, also lief die Animation ab Koordinate Null.
Jetzt bekommen Arm und Spieler beim Zugriff denselben Anker; der größte gemessene Abstand
während des Griffs liegt bei 3710 statt 16508 Einheiten.

---

**Tests:** 241/241 grün (lokal und im Docker-Linux-Build), 2 neue Wachen.


---

# RE1.5 Port — v0.3.29 (Early Preview)

Vier Meldungen. Bei einer war der Fehler frisch von mir, bei zweien lag er tiefer, als ich
zuletzt gemessen hatte.

## Die schwarzen Dreiecke über dem Feuer — meine Regression aus v0.3.28

Um Ada beim Folgen die richtige Animation zu geben, hatte ich im Renderer eine Bedingung
erweitert: „Zustand 1 → nimm die eigene Animationsbank". Zustand 1 ist aber der ganz normale
*aktiv*-Zustand fast jedes Gegners. Also haben ab v0.3.28 auch Zombies, Spinnen und die
Feuer-Emitter aus einer Bank posiert, die gar nicht zu ihrem Skelett gehört — und genau das
sind die schwarzen Dreiecke.

Der eigentliche Grund war, dass dieselbe Regel doppelt im Code stand, einmal für die
Animations-Uhr und einmal für den Renderer, und ich nur eine der beiden Kopien erweitert
habe. Die Regel steht jetzt **genau einmal**, beide Seiten fragen dieselbe Funktion, und
zwei neue Wachen prüfen sie von beiden Seiten: ein Nicht-NPC in Zustand 1 darf die eigene
Bank *nie* bekommen, eine folgende NPC *muss* sie bekommen.

## Ada läuft nicht mehr durch Wände

Zwei Ursachen, beide im Original nachgelesen.

Erstens fehlte die Wand-Klemme komplett. Die NPC-Wurzel klemmt jeden NPC direkt nach dem
Zustands-Aufruf gegen die Raumkollision — der Port hatte diesen Aufruf für die NPC-Familie
nirgends, und die neu portierte Eskorte schob die Figur einfach ungebremst weiter.

Zweitens hätte die Klemme allein auch nichts gebracht: sie liest ihren Radius aus der
Trefferbox des Gegners, und für die NPC-Typen war im Port gar keine Box eingetragen — Radius
0. Aus dem Original nachgezogen: 450 Radius, 1530 Höhe.

Eine neue Wache stellt Ada vor eine Wand und verlangt dreierlei — sie bewegt sich, sie kommt
nicht durch, und ihr Fortschritt zur Wand versiegt — plus die Gegenprobe mit einem
erreichbaren Ziel, damit „eingemauert" nicht als Erfolg durchgeht.

## ROOM1210: zu früh, zu leise, zu kurz — alle drei

**Zu früh.** Der Auslöse-Bereich des Raums ist 1700 Einheiten *tief insgesamt*, nicht
±1700 — die beiden Zahlen im Raum sind Ecke und Ausdehnung, was der Rechteck-Test des
Originals eindeutig zeigt. Ich hatte in v0.3.28 auf die volle Tiefe erhöht; das war der
falsche Schluss. Gemessen ging damit jeder Arm rund 22 Bilder zu früh auf und stand
zwei Drittel einer Sekunde fertig ausgefahren da, bevor du auf seiner Höhe warst. Jetzt ist
es wieder die halbe Tiefe.

**Zu leise.** Beim eigentlichen Zupacken hat der Arm bisher gar nichts gespielt; sein
einziger Laut lief in der Ausfahr-Schleife — und ausgerechnet der *leisere* der beiden
Zombie-Laute des Raums. Beide Vorlagen, RE1.5 wie RE2, spielen im Moment des Zugriffs einen
Laut; RE1.5 sogar unbedingt. Der ist jetzt drin, und es ist der mit der höheren Priorität:
beide liegen auf derselben Stimme, und der leise wurde vom lauten ohnehin verdrängt.

Ehrlich dazugesagt: RE2s Wucht an dieser Stelle besteht zu einem guten Teil aus
**Vibration** — eine Rampe auf volle Motorstärke plus vier Stöße beim Biss. Dafür hat der
Port kein Gegenstück; das ist eine benannte Lücke, kein Versehen.

**Zu kurz.** Das war der interessanteste Befund. Die Arme *können* dich mit ihrem Körper
gar nicht erreichen: ihr wird von einer eigenen Kollision im Gitter gehalten, du von deiner
im Flur, und dazwischen steht gut ein Meter Wand. Der kleinstmögliche Abstand zwischen den
beiden Mittelpunkten ist damit hart über 1800 Einheiten — mehr als das Griff-Fenster von
1200 zulässt. Keine zusätzliche Bewegung ändert daran etwas.

Das Modell reicht aber viel weiter als sein Mittelpunkt: die Hand ragt im Ausfahr-Clip 1671
Einheiten nach vorn. Sie steht also sehr wohl im Flur, nur der Körper nicht. Deshalb misst
das Griff-Fenster jetzt **die Hand statt des Mittelpunkts**. Die Schwelle selbst bleibt
unverändert bei 1200 — sie anzuheben wäre eine erfundene Zahl für genau dieses Problem
gewesen.

Gemessen an der Stelle, an die dich die Kollision wirklich lässt: drei Zugriffe, 275 Bilder
im Griff, sechs Bisse zu je 20 Lebenspunkten. In der Flurmitte greift nichts. Das ist genau
das Bild aus RE2, das du beschrieben hattest.

## Was dabei noch aufgefallen ist

Beim Nachmessen hat sich gezeigt, dass meine früheren Angaben zum „begehbaren Boden" in
ROOM1210 **invertiert** waren: die Kollisionsabfrage meldet in diesem Raum *innerhalb* einer
Wandzelle eine 1, gelaufen wird im Gegenstück. Die Wache benutzt jetzt nur noch den echten
Laufweg. Das erklärt auch, warum meine Erreichbarkeits-Zahlen der letzten Runde nicht
zusammenpassten.
