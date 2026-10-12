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

<!-- weitere Spuren: A pri1100, C fahrstuhl, D itemband, F sperre1050, G beretta, H zombieaktiv, I feuerloescher,
     J bissarm, K roy — Abschnitt erst nach bestandener Abnahme + Integration -->

## Grenzen

* **Code-Panel:** In der Raumansicht leuchten die winzigen gemalten Lampen des Lesers nicht (RE2 zeigt seine auch
  im Raum; hier leuchten sie nur im Panel-Bild). Der runde Schein streift die Oberkanten der obersten Tasten. Das
  Panel der Gefaengnistuer (11E0, Yellow Keycard) erscheint im normalen Spiel nie: der Startraum setzt ihr
  Schloss-Flag schon beim Neuspiel (ROOM1240 @0x0055A) - im Original genauso. Schliesst man die Nachrichten so
  schnell wie moeglich, klingt das Aufschliessen die letzten Bilder schon im Raum aus (wie RE2 ROOM2110).
