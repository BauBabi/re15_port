# RE1.5 Port — v0.3.25 (Early Preview)

Diese Version raeumt die drei Punkte ab, die seit der letzten Uebergabe offen standen.

## Gegner schweben nicht mehr (RE2-Modus)

Gemeldet: *"der zombie in 10d0 schwebt nach dem aufstehen leicht in der Luft"*.

Beim Nachmessen war es nicht auf diesen einen Raum beschraenkt: im RE2-Modus standen die
Zombies ueberall rund 130 bis 220 Einheiten ueber dem Boden, im RE1.5-Modus dagegen sauber
darauf. Der Grund liegt im Skelett-Umbau, der im RE2-Modus die Original-Figuren mit den
RE2-Bewegungen kombiniert: dabei wurden die Knochenlaengen getauscht, die Hoehe der Huefte
aber nicht mitgerechnet.

Die Korrektur wird jetzt beim Laden fuer **jede einzelne Pose** ausgerechnet, nicht pauschal
angesetzt — ein fester Wert haette liegende, kriechende und fressende Zombies bis zu 160
Einheiten in den Boden gedrueckt. Der RE1.5-Modus bleibt dabei auf das Bit unveraendert;
Hund und Kraehe bewegen sich um hoechstens zwei Einheiten, weil ihre Skelette ohnehin passen.

## Ada folgt jetzt

Gemeldet: *"ausserdem folgt mir ada nicht nach der cutscene"*.

Sie folgte nicht, weil der Port fuer diesen Zustand gar kein Verhalten hatte — die Figur hielt
einfach ihre Pose. Das Original hat dort eine vollstaendige Begleit-Steuerung: sie peilt einen
Punkt anderthalb Meter hinter dem Spieler an, laeuft los, sobald der Abstand zu gross wird,
bleibt stehen, wenn sie nah genug ist, und geht wieder los, wenn man sich entfernt.

Der Fehler war nicht auf ROOM1090 beschraenkt — dieselbe Stelle betrifft acht Raeume und jede
Figur, die aus einer Zwischensequenz uebergeben wird. Alle folgen jetzt.

Einen Sonderfall habe ich bewusst genau so gelassen, wie er im Original steht: solange ein
lebender Gegner in der Naehe ist, entscheidet die Begleiterin weiter, bewegt sich aber nicht.
Das sieht seltsam aus, ist aber der Code des Originals — und es gehoert an einem echten
Spielstand geprueft, bevor jemand daraus mehr macht.

## ROOM1090: nach dem Feuerloescher sind die Flammen weg

Gemeldet: *"nachdem man mit dem [Wort fehlte] die flammen ausgemacht hat, muss der flammen
effekt weg sein."*

Das fehlende Wort steht im Raum selbst — der Feuerloescher. Und das Loeschen ist im Original
kein Abschalten, sondern ein Trick: der Raum schickt den Spieler durch eine Tuer, die auf sich
selbst zeigt. Er wird also neu geladen, und beim Neuaufbau bleiben die brennenden Truemmer
einfach weg.

Im Port lief das schon fast vollstaendig: die Feuerstellen und ihre Schadenszone verschwanden
korrekt. Stehen blieben nur die **Flammen selbst** — vierzehn Effekt-Teilchen, die weiter
loopten, weil der Effekt-Speicher beim Selbst-Wiedereintritt nicht geleert wurde. Das Original
leert ihn bei jedem Raumladen. Jetzt tut es der Port auch: Feuer und Flammen verschwinden
zusammen.

## Noch offen

Die Zombie-Haende am Gitter in ROOM1210 sind inzwischen vollstaendig aufgeklaert, aber bewusst
noch nicht umgebaut. Der Befund ist zweigeteilt:

Im Port stehen die zehn Kreaturen dauerhaft still, obwohl der Raum sie zur Laufzeit scharf
schaltet — das ist ein echter Fehler und die Original-Mechanik ist notiert.

Und die RE2-Szene, an die die Erinnerung anknuepft, gibt es zur Haelfte: RE2 hat sehr wohl
verankerte Zombies, die sich nicht von der Stelle bewegen, stoehnen und den Spieler anschauen.
Was RE2 **nicht** hat, ist ein Zugriff aus dieser Stellung heraus — im ganzen Zombie-Code gibt
es genau eine Stelle, die dem Spieler Schaden zufuegt, und die ist nur aus dem Gehen erreichbar.
Die erinnerten greifenden Haende waeren also eine Kombination aus beidem. Machbar, aber es
waere eine Erweiterung und keine Wiederherstellung — deshalb warte ich damit auf eine
Entscheidung, statt es stillschweigend einzubauen.
