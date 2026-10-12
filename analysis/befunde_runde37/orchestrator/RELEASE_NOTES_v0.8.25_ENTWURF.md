# v0.8.25 - 2026-10-?? (ENTWURF — je bestandener und integrierter Spur ein Abschnitt; beim Release nach release/RELEASE_NOTES.md)

Ihr Auftrag der Runde 37 ist gebaut. Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten
Spiel gegen Ihren Wortlaut nachgemessen; wo er nicht bestand, wurde nachgebessert und erneut abgenommen.
Suite 698 -> ???/??? (volle Suite einmal im Integrationsbaum, ohne parallele Agenten).

## Was Sie merken

**Code-Panels** (Spur E). Im 2F Corridor (Funkraum, Blue Keycard) und am SECUGUARD-Panel (B1, Waffenkammer, Red
Keycard) leuchtet die rot gemalte Lampe rot, solange der Code nicht richtig ist - auch nach "Wrong code, try
again.". Ist der Code richtig, geht sie aus und die andere gemalte Lampe leuchtet in ihrer Farbe: im 2F Corridor
blau, am SECUGUARD-Panel gruen (wie RE2 ROOM4070 und ROOM2130; die Leuchtkunst ist RE2s eigene Lampe). Die Kamera
bleibt dabei auf dem Panel: Piep, eine halbe Sekunde spaeter das Aufschliessen, dazu "You've opened the lock." und
die Frage zum Wegwerfen der Karte; erst danach schaltet sie in den Raum zurueck (RE2 ROOM2110). Die
Wegwerf-Frage steht damit jetzt auf dem Panel statt im Raum. Gilt fuer Leon und Elza.

**Sperre vor dem Ende** (Spur F). Ihre Luecke ist zu: ueber das Lueftungsgitter in ROOM10F0 und den Laufsteg in
ROOM1090 kam man in den Hof und durch ROOM1050 nach 10A0 - an der Sperre in 1060 vorbei (moeglich, seit der Tritt
in 10F0 in v0.8.24 wieder kletterbar ist). Jetzt steht auch an der Tuer von ROOM1050 nach 10A0 "I have to get the
Chief first...", sobald Ada gerettet und die Szene in 10F0 gesehen ist, bis Irons geholt ist. Vor Adas Rettung laeuft
dort unveraendert die Ada-Szene, im Mittelspiel und nach Irons ist die Tuer offen. Laut Pruefung aller Tueren von Leon
ist das der einzige Weg in den Kellerteil.
**Elza, ROOM50B1.** An der Leiche: "It's a woman. She's dead. / It looks like one of those gorillas killed her." -
Elza kennt Ada nicht vom Sehen. Den Namen Ada hoert sie nur von Marvin (ROOM30C1).

**Irons' letzte Worte** (Ihr Nachtrag). Am Ende des Dialogs in ROOM1150 sagt Irons jetzt "Get them out of here...
Leon..." statt "Be a hero... Leon..." - er nimmt Leons Plan mit dem Streifenwagen auf, der Arm faellt wie bisher.
Eine Sprachaufnahme fuer diese Zeile gibt es nicht (optional: synchro/STAGE1/room1150/main28.wav).

**Fahrstuehle** (Spur C). ROOM1080: die Tuer der Fahrstuhl-Sequenz ist jetzt eine eigene Fahrstuhltuer in der Optik
des gemalten Revier-Fahrstuhls (zwei glatte Fluegel) - keine der 55 RE2-Tueren passte; Ton und Ablauf wie die RE2-
Fahrstuhltuer. Beim Aussteigen laeuft jetzt ebenfalls die Tuersequenz (ROOM1080 und Labor-Fahrstuhl ROOM4020, wie RE2
ROOM6010). Der Lastenaufzug ROOM3080 -> 4000 endet jetzt mit der Sequenz der RE2-Hubbuehne (ROOM6170 -> 7000): das
Schacht-Fachwerk faehrt etwa 15 Sekunden hinab, die Fahrt dauert damit laenger als bisher. Auch die Fahrt hinauf
(4000 -> 3070) zeigt jetzt das bewegte Fachwerk statt eines Standbilds. Die Hubplattformen in ROOM2030 haben jetzt den
Ton der RE2-Hubplattform (ROOM6030): Klick, Motor, Stopp-Schlag kurz nach der Ankunft; Plattform A faehrt kuerzer als in
RE2, ihr Motorton ist deshalb gekuerzt.

**ROOM1100 und Elzas Verdeckung** (Spur A). Der liegende Zombie an Ihrer F9-Marke in ROOM1100 wird jetzt von der
Wand verdeckt (beide Marken: kein Pixel mehr ueber der Wand). Die Masken arbeiten je Tiefenstufe statt mit dem
tiefsten Wert je Kachel. Elza bekommt in 20 Raeumen von Stage 1 die Verdeckungsmasken des Leon-Raums mit derselben
Kamera und demselben Hintergrund - bisher hatte sie dort keine (z.B. ROOM1101: der Zombie lag dort ueber der Wand).
In ROOM1221 und ROOM2091 lagen fuer Elza veraltete Kopien (1221 wurde nie gezeichnet, 2091 ohne die zwei Kameras aus
Ihren Freistellungen); sie nutzt dort jetzt ebenfalls Leons aktuelle Masken (22 Raeume).
**Roy** (Spur K). In Elzas Szenario traegt Roy jetzt seine Uniform wie im Original (aus Elzas eigenem Modell-Archiv)
statt Elliots Modell. Er sitzt verwundet da, sackt zusammen und bleibt liegen; in ROOM1021 schlurft er auf John zu und
faellt. Keine Dauerschleife mehr. Elliot bei Leon ist unveraendert.
**Zombie-Biss** (Spur J). Beim Biss liegt der Arm des Zombies jetzt wie in RE2 auf Leons bzw. Elzas Schulter - vorher
steckte er im Koerper. In ROOM1010 bleibt Leon im Griff an seinem Platz (vorher stand er waehrenddessen an einer
falschen Stelle). Einen baumelnden angeschossenen Arm gibt es in RE2 nicht: ein abgeschossener Unterarm verschwindet
nach knapp einer Sekunde, so wie bisher.

**Zombies aktiver** (Spur H). Die Zombies, die RE1.5 selbst als angriffsbereit ablegt, gehen jetzt wie in RE2 schon
beim Betreten des Raums auf Sie zu, statt stehen zu bleiben - in jedem Raum, fuer Leon und Elza (u.a. ROOM1010, 1020,
1070, 1100, 1180, 11E0, 11F0, 1250, die Zombie-Girls in 4050). Nach Irons' Tod kommen die fuenf Zombies im Gang 1130
auf Leon zu, wie die, die in 1040 durchs Rolltor brechen. Liegende, Fresser, Schlaefer und Kriecher bleiben wie bisher.
Der Port ist damit aggressiver als RE2: dort laeuft etwa ein Drittel der Zombies sofort los, hier gut die Haelfte, weil
RE1.5 mehr Zombies als angriffsbereit ablegt. Waehrend der Montage-Einstellung in 1130 selbst stehen die fuenf noch
(Leon ist dort nicht im Bild).

<!-- weitere Spuren: A pri1100, C fahrstuhl, D itemband, F sperre1050, G beretta, H zombieaktiv, I feuerloescher,
     J bissarm, K roy — Abschnitt erst nach bestandener Abnahme + Integration -->

## Grenzen

* **Code-Panel:** In der Raumansicht leuchten die winzigen gemalten Lampen des Lesers nicht (RE2 zeigt seine auch
  im Raum; hier leuchten sie nur im Panel-Bild). Der runde Schein streift die Oberkanten der obersten Tasten. Das
  Panel der Gefaengnistuer (11E0, Yellow Keycard) erscheint im normalen Spiel nie: der Startraum setzt ihr
  Schloss-Flag schon beim Neuspiel (ROOM1240 @0x0055A) - im Original genauso. Schliesst man die Nachrichten so
  schnell wie moeglich, klingt das Aufschliessen die letzten Bilder schon im Raum aus (wie RE2 ROOM2110).
