# RE1.5 Port — v0.3.26 (Early Preview)

## ROOM1210: die Haende kommen jetzt, wenn man vorbeigeht

Gemeldet: *"im room 1210 strecken ab einem gewissen punkt alle Zombies die Haende aus dem
gitter. das ist sau schlecht gemacht ... in Resident Evil 2 gibt es einen Flur, wo man
durchlaeuft, und kommt man am Fenster vorbei, kommen die Haende der Zombies mit stoehnen und
greifen einen. das wuerde ich gerne auch hier so umgesetzt haben wollen."*

Das ist jetzt drin — und es ist ausdruecklich eine **Erweiterung**, keine Wiederherstellung.
Im Original schaltet der Raum alle zehn Arme auf einen Schlag scharf; genau das war ja der
Kritikpunkt. Statt dessen entscheidet jetzt jeder Arm fuer sich, ob du auf seiner Hoehe bist.

Beim Vorbeigehen faehrt der Arm aus, greift nach dir, stoehnt dabei — und zieht sich wieder
zurueck, wenn du weiter bist. Wer hineinlaeuft, wird gepackt und verliert Lebensenergie.
Gemessen reagieren alle zehn Arme im Durchlauf, aber **hoechstens drei gleichzeitig**, und
keiner verlaesst sein Gitter.

Was aus dem Original uebernommen ist: die Rollen der drei Animationen (Ruhe, Ausfahren,
Greifen), das Stoehnen (es ist der Moan aus der Sound-Bank des Raums, mit demselben
Zufalls-Muster wie beim Zombie) und die Reichweite — die 850 Einheiten entlang des Flurs sind
die halbe Tiefe des Ausloese-Rechtecks, das im Raum selbst steht.

Was bewusst **nicht** uebernommen ist: die Vorwaerts-Bewegung. Die Original-Mechanik laesst
die Kreatur rund 2400 Einheiten vorschnellen — das kann ein Arm, der im Gitter steckt, nicht,
und es ist vermutlich genau der Teil, der am Beta-Stand so unfertig wirkte.

Eine Einschraenkung, ehrlich gesagt: es ist ein *Zupacken mit Schaden*, kein Festhalten, aus
dem man sich losschuetteln muss. Der Grund liegt in den Daten — das Arm-Modell hat nur vier
Knochen und bringt keine Opfer-Animation mit, und der Raum laedt keinen anderen Gegner, von
dem man sich eine leihen koennte. Ein Festhalten haette also eine erfundene Animation
gebraucht.

## Kleinigkeit am Rande

Die Musik-Steuerung schrieb bei jedem Skript-Befehl eine Diagnosezeile ins Log — auch im
ausgelieferten Stand. Das ist jetzt abgeschaltet.

## Noch offen

Aus der Vorrunde bleibt nichts liegen: schwebende Gegner, Adas Folge-Verhalten und die
ROOM1090-Flammen sind seit v0.3.25 erledigt. Die Feuer-Tonspur, die ich dort als offen notiert
hatte, war ein Irrtum meinerseits — sie verstummt bereits korrekt, und das ist jetzt
zusaetzlich abgesichert.
