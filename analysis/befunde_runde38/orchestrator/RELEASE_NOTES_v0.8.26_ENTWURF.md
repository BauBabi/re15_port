# v0.8.26 - 2026-10-?? (ENTWURF, Runde 38)

Ihr Auftrag vom 16.10. (Runde 38) und die zwei Reste der Runde 37 (Zombie-Angriffsgang wie RE2, Verdeckung in den Stages
3 bis 6). Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen; wo
er nicht bestand, wurde nachgebessert und erneut abgenommen. Suite 788 -> ???/??? (volle Suite einmal im
Integrationsbaum, ohne parallele Agenten).

## Was Sie merken

<!-- je integrierter Spur ein Absatz, Nutzer-Sprache, keine Adressen -->

**Spiderman (Stage 4/5) und Griffe aller Gegner.** Der Spiderman wartet jetzt wie im Original versteckt im Schacht, bis
Sie den Ausloeser erreichen, und klettert dann heraus (vorher lief er sofort los); beim Herausklettern und in seinen
Griffen verschwindet er nicht mehr fuer ein Bild. Vor allem: Leon wird beim Griff nicht mehr aus der Karte geworfen
(vorher bis zu 27 000 Einheiten weit). Fuer alle Gegner gilt jetzt: ein Griff kann den Spieler nicht durch eine Wand
versetzen - in ROOM1220 setzte der Kriecher Leon bisher hinter die Wand.

**Ein Skript fuer alle Systeme.** Doppelklick auf `release/release_alles.cmd` (oder `bash release/release_alles.sh`):
Es baut Windows, Android und Linux/Steam Deck, gibt der neuen Version die naechste Nummer (letzte Stelle +1, nach 99
die naechste mittlere: 0.8.99 -> 0.9.0), packt und prueft die Pakete und bringt die bisherige Version ins Archiv; auf
Wunsch auch Release-Commit, Tag und Push. Vorher zeigt ein Trockenlauf, was passieren wuerde. Bricht ein Schritt ab,
nennt das Skript die Zeile zum Weitermachen. Anleitung: `release/README.md`.

**Elza, Toene in ROOM1041 und ROOM1021.** In 1041 faellt statt des Platzhalter-Textes "gunshot" jetzt ein hoerbarer
Schuss, danach dreht sich Elza um ("What was that!?"). In 1021 stoehnt Roy wie ein Zombie, wenn er auf John zugeht.
Johns Schuesse waren schon da, aber mit einer duennen, hellen Aufnahme, die man kaum als Pistole erkannte; jetzt
klingen sie wie eine schwere Pistole (dieselbe Aufnahme, die RE2 fuer solche Skript-Schuesse verwendet). Zum
Gegenhoeren liegen vorher/nachher-Ausschnitte im Analyse-Ordner der Spur.

**Elza auf dem Hubschrauberlandeplatz (ROOM1171).** Elza blieb in ihrer Dachszene stecken: Die Szene wartet darauf, dass
eine Kraehe Elza von hinten packt - diese Kraehe kam im Port nie an (die RE2-KI hatte sie uebernommen, und ihr Signal
erreichte das Szenenskript nicht). Jetzt laeuft die Szene wie im Original durch: die Kraehe packt Elza, John hilft, es
geht hinunter in den Vorhof. Der Griff von hinten kostet wie im Original keine Lebensenergie.

**Verdeckung in den Stages 3 bis 6 (Rest aus Runde 37).** Seit v0.7.19 zeichnete der Port die ergaenzten Vordergrund-
Masken der Stages 3 bis 6 nicht mehr (ein Lader-Fehler) - Figuren liefen dort ueber Maschinen, Moebel und Wandteile, die
sie verdecken muessten. Jetzt verdecken sie wieder. Wo eine ergaenzte Maske eine Figur VOR dem Objekt uebermalen wuerde,
ist sie abgeschaltet (auch der Kranpult-Cursor in ROOM3050 bleibt frei).

**Rekorder-Raetsel (ROOM1100, auch Elza).** Beim Hineinsprechen laeuft jetzt Ihre Aufnahme Irons_Rekorder_Sound.wav;
erst wenn sie zu Ende ist, kommen der Piep (Eingabe angenommen) und das Aufschliessen - zusammen mit der Anzeige ACCEPTED
auf dem Panel, so wie RE2 Ton und Anzeige an seinen Schloessern koppelt. Vorher kam beim Benutzen gar kein Ton.

**Elzas Vorspann.** Elzas Intro spricht jetzt dieselben Zeilen wie Leons (dieselben Aufnahmen; legen Sie eigene
Aufnahmen fuer Elza unter `synchro/STAGE1/room1241/` ab, haben diese Vorrang). Im schwarzen Bild hinter dem Erzaehltext
("We barricaded ourselves...") schien bei Elza der Lageplan 1F durch - ein Fehler seit v0.8.24, als die Lageplaene als
Gegenstaende in die Welt kamen; jetzt ist das Bild schwarz wie bei Leon.

**Inventar mit Ton.** Oeffnen und Schliessen des Statusbildschirms klingen jetzt wie in RE2 (eigene Toene fuer Oeffnen und
Schliessen; wo RE2 den Schirm selbst oeffnet - Kiste, Aufnahme -, bleibt es wie dort still).

**Elza, ROOM1151.** Klickt Elza den toten Irons an, beugt sie sich nur hinunter und es erscheint "He has already
passed..." - die alte Zeile "I'll be fine..." kommt nicht mehr (ein Fehler seit v0.8.24).

**ROOM30E0, Flammenwerfer.** Er wird jetzt normal aufgenommen (Aufnahme-Abfrage, volle Ladung) statt "not available
in this preview". Sagen Sie Nein, koennen Sie ihn spaeter holen.

**Zombies wie in RE2 (Rest aus Runde 37).** Zombies heben jetzt wie in RE2 von sich aus die Arme und kommen im
schnelleren Angriffsgang auf Sie zu - wenn Sie naeher kommen, nach einer Weile von selbst und wenn Sie an ihnen
vorbeirennen; aus diesem Gang greifen sie zu. Das gilt fuer alle Zombies, auch die im Gang nach Irons' Tod. Nach einem
Treffer erholen sie sich wie in RE2 (vorher setzte ihr Gang nach jedem Treffer neu an). Mit dem Messer bleibt es je
Anmarsch etwa gleich schwer (plus/minus ein Treffer, je nach Schlagtakt).

**ROOM11B0, Marvin und der Container.** Marvin sagt jetzt "But, hold on...". Das gemeinsame Verschieben hat einen
Schiebe-Ton - derselbe, den Leon beim Kistenschieben in ROOM1090 hoert (die Tonbank von 11B0 ist an dieser Stelle im
Original leer, die Szene war dort also stumm). Die "Fall"-Bewegung war ein Port-Fehler: Marvin sackte im letzten
Bild des Schiebens noch einmal in die tiefe Schiebehaltung; jetzt bleibt er in seiner Haltung.

## Grenzen

* Verdeckung Stages 3-6: rund ein Fuenftel der ergaenzten Masken bleibt aus, damit keine Figur VOR einem Objekt uebermalt
  wird; volle Verdeckung braeuchte Freistellungen wie in Stage 1. An einer Bodenkante in ROOM5090 bleiben einzelne Pixel.

## Zu entscheiden

* Das Archiv wird nie automatisch gekuerzt (Ihr Wortlaut: "die alte ins Archiv bringen"). Soll es kuenftig nur die
  letzten N Versionen behalten (heute rund 0,9 GB je Version)?

## Neu fuer das Paket / Ihre Dateien

* ROOM1171: Ihre `main00.wav` und `main01.wav` laufen jetzt, weil die Szene durchlaeuft. `main02.wav` ("C'mon, give me a hand!")
  spielt nie - das Original-Skript zeigt diese Nachricht nicht. Fuer "Alright, let's get down from here!" fehlt noch
  `synchro/STAGE1/room1171/main03.wav`.

* Der Erzaehltext vor der Lobby ("We barricaded ourselves...") hat bei Leon und Elza noch keine Aufnahme:
  `synchro/STAGE1/room1170/main08..11.wav` (Leon) bzw. `synchro/STAGE1/room1031/main20..23.wav` (Elza).

* Fuer die neue Marvin-Zeile gibt es noch keine Aufnahme: `synchro/STAGE1/room11B0/main20.wav` ("But, hold on...").
