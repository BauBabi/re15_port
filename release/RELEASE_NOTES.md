# RE1.5 Port — v0.3.27 (Early Preview)

## ROOM1210: aus dem Zupacken ist ein echtes Festhalten geworden

Zur letzten Version hatte ich eine Einschraenkung eingeraeumt: die Haende im Gitter *packten
zu und machten Schaden*, aber sie hielten nicht fest. Der Grund lag in den Daten — das
Arm-Modell EM01A bringt keine Opfer-Animation mit, und ohne die gibt es niemanden, der Leon
waehrend eines Griffs bewegt.

Auf die Ansage *"Ja, stell das um, wenn dir da was fehlt, hole die Animation aus Resident
Evil 2"* ist das jetzt umgestellt.

Wer in eine der Haende hineinlaeuft, wird gepackt und **festgehalten**: Leon spielt den
Ringkampf, wehrt sich durch Tastendruecke los, wird dabei gebissen — und kommt wieder frei.
Gemessen dauert ein Griff 96 Bilder, wenn man nichts tut; wer mitdrueckt, ist deutlich
schneller draussen. Der Biss kostet 20 Lebenspunkte.

Die Zahlen sind keine Erfindung. Der Griff ist RE2s Fenster-Mechanik, Zeile fuer Zeile aus
dem Zombie-Overlay uebernommen: die Reichweite (1200 Einheiten), der Greif-Winkel (ein
45-Grad-Kegel nach vorn, aus zwei Halb-Sektoren), das Ringkampf-Budget (148, pro Bild minus 2,
ein Tastendruck zieht 5 zusaetzlich ab) und der Biss (Bild 16 des Greif-Clips, 20 Punkte)
stehen so im Original. Die Adressen dazu stehen im Quelltext an jeder einzelnen Konstante.

**Und die Animation ist geliehen, nicht erfunden.** Das ist der Punkt, an dem es haette
schiefgehen koennen, deshalb kurz das Warum: die Opfer-Animation eines Gegners bewegt nicht
den Gegner, sondern **Leon** — sie steckt in einem eigenen Datenpaar und laeuft auf Leons
eigenem Skelett, nicht auf dem des Angreifers. Ein Arm mit vier Knochen kann sich so etwas
also durchaus von einem Zombie leihen, ohne dass irgendetwas gebogen werden muesste. Genau
das passiert: der Arm borgt sich den Ringkampf des Zombies — im RE1.5-Modus RE1.5s eigenen,
im RE2-Modus den von RE2. Es kommt keine einzige neu erfundene Bewegung dazu.

### Zwei Dinge, die ich beim Nachmessen gefunden habe

**Die Arme kamen bisher immer zu spaet.** Die Reichweite, ab der ein Arm ausfaehrt, stand auf
der halben Tiefe des Ausloese-Rechtecks des Raums. Nachgerechnet reicht das nicht: der
Ausfahr-Clip dauert 30 Bilder, ein gehender Spieler ist bei halber Tiefe aber nur rund 22
Bilder in Reichweite — der Arm war fertig, als der Spieler schon vorbei war. Jetzt steht dort
die volle Tiefe, also die Zahl, die woertlich im Raum steht; die Halbierung war meine Zutat.

**Und die meisten Kreaturen kann man gar nicht erreichen.** Ich habe den begehbaren Boden
abgetastet: acht der zehn stehen zwischen 1300 und 2800 Einheiten *hinter* dem Gitter,
ausserhalb von allem, was man betreten kann — und das Arm-Modell ist mit gut 440 Einheiten
viel zu kurz, um das aufzuholen. Nur zwei Stellen liegen so, dass ein Spieler wirklich
gegriffen werden kann. Fuer die anderen acht bleibt es beim Ausfahren und Stoehnen.

Das ist keine Einschraenkung, die ich weggeredet haben will, sondern schlicht die Geometrie
des Raums — und ehrlich gesagt passt es zu dem Bild, das du beschrieben hast: die Haende
kommen ueberall raus, erwischen tut einen nur, wer zu nah kommt.

Was weiterhin bewusst **nicht** uebernommen ist: die Vorwaerts-Bewegung der Original-Mechanik.
Ein Arm, der im Gitter steckt, kann nicht 2400 Einheiten vorschnellen. Und RE2s toedlicher
Ausgang (der Zombie zieht einen zu Boden und frisst) fehlt aus demselben Grund — der toedliche
Biss wirft hier ab wie jeder andere, und der normale Tod uebernimmt.

## Noch offen

Aus den Vorrunden bleibt nichts liegen.
