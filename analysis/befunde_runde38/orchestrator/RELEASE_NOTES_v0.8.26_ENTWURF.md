# v0.8.26 - 2026-10-?? (ENTWURF, Runde 38)

Ihr Auftrag vom 16.10. (Runde 38) und die zwei Reste der Runde 37 (Zombie-Angriffsgang wie RE2, Verdeckung in den Stages
3 bis 6). Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen; wo
er nicht bestand, wurde nachgebessert und erneut abgenommen. Suite 788 -> ???/??? (volle Suite einmal im
Integrationsbaum, ohne parallele Agenten).

## Was Sie merken

<!-- je integrierter Spur ein Absatz, Nutzer-Sprache, keine Adressen -->

**Feuerloescher und Schluessel an Ihren Markierungen.** Der Feuerloescher haengt jetzt in der Lobby (ROOM1030) am Pfeiler,
genau an der Stelle aus Ihrem Bild (statt in ROOM1050). Parking Lot Key (Leon) und Prison Key (Elza) haengen gut sichtbar an
der Pinnwand im Communications Room. Der Prison Key ist dafuer jetzt RE2s "Small Key" (heller Bartschluessel mit Ring) -
der bisherige duenne Eisenring war auf dem dunklen Kork kaum zu erkennen. Aufnahme, Inventar-Symbol und Item-Bild zeigen
denselben Schluessel.

**Kendo/John, Ada und Marvin springen Kanten hinunter.** Statt waagrecht in die Luft weiterzulaufen, springen die
Figuren jetzt wie im Original von Stegen und klettern wieder hinauf: John in ROOM10B1 (vom oberen Steg ueber vier
Stockwerke hinunter) und ROOM1261 (Elza), Ada und Marvin in ROOM1260 und in ROOM2000 (Leon). Unten am Westrand des
Blocks in ROOM10B1 springt John noch einmal hinunter und klettert wieder hinauf, solange die Kamera auf dem oberen Steg
bleibt - das macht das Original genauso (gemessen); von der Steg-Kamera aus sieht man es nicht.

**Elza: von Roys Tod zu John in den Besprechungsraum (Ihr Ablauf).** Nach der Szene in ROOM1021 zeigt Roys Leiche
nach "He's dead. / Poor Roy..." den Hinweis "There's an empty key tag on his belt. / It reads "Dining Room".". In
ROOM1011 liegt an Roys alter Liegestelle der Dining Room Key (RE2s Herz-Schluessel). Bis dahin ist die Tuer zum
Besprechungsraum (ROOM1141) verschlossen ("It's locked." / "A heart is etched under the key hole." - RE2s eigener Text
fuer die Herz-Tuer). Im Raum hoert man hinter dem geschlossenen Notrolladen Klopfen und Johns "Help me out." - nur als
Ton, ohne Text - zur Kampfmusik des Raums. Das Gemaelde hebt den Rolladen; John steht dahinter und wird von den Fuessen
an sichtbar (Bild fuer Bild geprueft: er ploppt nirgends herein), dann direkt "Are you okay? What's happened?" ohne
"John, wait" und ohne "sorry". Solange die Gorillas leben, sagt jede Tuer "I cannot leave him alone with those
monsters.". Danach geht John zur Dachtuer - steht Elza im Weg, wartet er, statt sie zu schieben - und wartet auf dem
Dach (ROOM1171).

**Rolladen-Ton (ROOM1140/1141).** Jedes Hochfahren des Notrolladens hat jetzt die drei Rolladen-Toene aus ROOM1131
(Klick, Motor, Anschlag), bei Elza am Gemaelde-Schalter und bei Leon.

**Roys Tagebuch (Elza, ROOM1011).** Untersucht Elza den bewusstlosen Roy ein zweites Mal, erscheint statt der
Code-Nachricht jetzt "Roys Diary" - Ihr Text auf RE2-Papier (der FILE16-Hintergrund), der Code 0513 gruen wie in allen
Dokumenten; es wird in den Akten abgelegt und kann dort wieder gelesen werden. Danach markiert die Karte erst den
Communications Room (2F), dann den Prison Room (B2, Johns Zelle) - so wie bei Leons Communications-Room-Hinweis.

**Spinnen in Stage 2.** Die Schachtspinnen (ROOM2060, ROOM20A0) warten im Schacht, bis Sie den Ausloesebereich
betreten, kriechen dann heraus, klettern die Wand hinunter und landen am Boden; keine Spinne schwebt mehr in der Luft.
Die Spinnen spucken Gift: Ein Treffer kostet 10 HP und vergiftet (im Statusschirm "Poison"; alle paar Sekunden gehen
2 HP verloren, das Gift toetet aber nicht). Das Antidote Spray (oder ein blaues Kraut) heilt. Nur die Spinne
vergiftet, wie in RE2 - im ausgelieferten RE1.5 setzt der Treffer das Gift nie, dort war das Spiel unfertig. Die
Minispinnen nach dem Tod einer Spinne krabbeln am Boden, bleiben auf ihrem Steg, springen Sie an und beissen (je 1 HP).

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

**Elzas Zopf.** Elza hat ihren Pferdeschwanz. Er war in RE1.5 schon angelegt: Elzas Modell traegt vier Zopfglieder,
und das Original rechnet sie mit derselben Pendel-Routine, die RE2 fuer Claires Zopf benutzt - der Port hatte sie nur
nicht gezeichnet. Jetzt haengt der Zopf am Hinterkopf, schwingt beim Gehen und Rennen nach hinten und pendelt beim
Anhalten aus; am Original Bild fuer Bild nachgemessen (235 Bilder rechnet der Port bitgleich nach).

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

**Cursor ohne Schatten (ROOM1150, ROOM11F0 und alle anderen Cursor-Raetsel).** Der "komische Schatten" war die
Unterseite des Cursor-Kastens: Sie traegt dasselbe Muster, und der Port zeichnete sie durch die durchsichtige
Oberseite mit. Wie das Original zeichnet der Port Rueckseiten jetzt nicht mehr. Das gilt fuer alle 23 Cursor-Raeume
(Generator, Ziffernfelder, Aufzuege, Kranpult usw.) und fuer den Hebetisch in Irons' Buero, bei Leon und Elza.
Nebenwirkungen wie im Original: In Irons' Buero verschwindet ein dunkles Band hinter den Papierstapeln, und eine
Karte an der Wand scheint in ROOM11D0 aus einer Kamera hinter der Wand nicht mehr durch.

**Keine Angriffe mehr in Szenen, in denen Sie sich nicht bewegen koennen.** Solange eine Szene Leon (oder Elza)
festhaelt, greifen Gegner nicht an - sie laufen, kreisen oder knurren, Kraehen im Anflug drehen ab. Das gilt fuer
alle Gegnerarten, nicht nur fuer die Hunde in ROOM1190 und ROOM11D0. Ist der Angriff selbst Teil der Szene (Elzas
Kraehe auf dem Hubschrauberlandeplatz), bleibt er. Nach der Szene greifen die Gegner wieder normal an; die
Zwingerhunde in ROOM1190 beissen wie im Original erst vier Sekunden nach ihrer Landung.

**ROOM11B0, Marvin und der Container.** Marvin sagt jetzt "But, hold on...". Das gemeinsame Verschieben hat einen
Schiebe-Ton - derselbe, den Leon beim Kistenschieben in ROOM1090 hoert (die Tonbank von 11B0 ist an dieser Stelle im
Original leer, die Szene war dort also stumm). Die "Fall"-Bewegung war ein Port-Fehler: Marvin sackte im letzten
Bild des Schiebens noch einmal in die tiefe Schiebehaltung; jetzt bleibt er in seiner Haltung.

## Grenzen

* Beobachtung (nicht beauftragt, schon in v0.8.25): Untersucht Elza den liegenden Roy, beugt sie sich nicht zu ihm
  hinunter, obwohl das Original-Skript eine Geste setzt. Wenn Sie das aendern moechten, sagen Sie Bescheid.

* Verdeckung Stages 3-6: rund ein Fuenftel der ergaenzten Masken bleibt aus, damit keine Figur VOR einem Objekt uebermalt
  wird; volle Verdeckung braeuchte Freistellungen wie in Stage 1. An einer Bodenkante in ROOM5090 bleiben einzelne Pixel. Rennt Elza
  von der Seite gesehen, kann ihre Zopfspitze an einer solchen Maskenkante einzelne Punkte verlieren (dieselbe Art wie
  der ausgestreckte Arm beim Zielen).

* Bekannt, nicht beauftragt (alte Abweichung, auch in v0.8.25): Bei kurzen Skript-Bewegungen ohne Text (z.B.
  ROOM1230, Leon hockt sich eine Sekunde vor der Nachricht hin) kann Leon im Port schon laufen, im Original nicht.
  In diesen Momenten greifen Gegner deshalb normal an. Die Korrektur braucht eine eigene Durchsicht aller
  Skript-Bewegungen.

* Der Schluessel heisst nach Ihrer Vorgabe "Dining Room Key"; das Schild ueber der Tuer im Hintergrundbild zeigt
  "Briefing Room". Der Tuertext nennt deshalb kein Schild. Der Schluessel liegt flach auf Roys Blutlache; aus den
  Raumkameras ist er nur ein heller Punkt (Cut 3: 2 Bildpunkte bei Skala 1). Aufgehoben wird er an Roys Liegestelle mit
  der Aktionstaste. Leons Gemaelde in ROOM1140 meldet wie im Original "The
  switch is not responding...".

## Zu entscheiden

* Das Archiv wird nie automatisch gekuerzt (Ihr Wortlaut: "die alte ins Archiv bringen"). Soll es kuenftig nur die
  letzten N Versionen behalten (heute rund 0,9 GB je Version)?

## Neu fuer das Paket / Ihre Dateien

* ROOM1171: Ihre `main00.wav` und `main01.wav` laufen jetzt, weil die Szene durchlaeuft. `main02.wav` ("C'mon, give me a hand!")
  spielt nie - das Original-Skript zeigt diese Nachricht nicht. Fuer "Alright, let's get down from here!" fehlt noch
  `synchro/STAGE1/room1171/main03.wav`.

* Der Erzaehltext vor der Lobby ("We barricaded ourselves...") hat bei Leon und Elza noch keine Aufnahme:
  `synchro/STAGE1/room1170/main08..11.wav` (Leon) bzw. `synchro/STAGE1/room1031/main20..23.wav` (Elza).

* ROOM1141 (Elza): `synchro/STAGE1/room1141/main40.wav` ("Are you okay? What's happened?", geschnitten aus
  main02.wav) und `main42.wav` ("Help me out.", geschnitten aus Johns Dachszene room1171/main01.wav, klingt deshalb
  ruhig). Eine eigene Aufnahme unter demselben Namen ersetzt sie ohne weiteren Schritt. Das Klopfen ist RE2s
  Klopfgeraeusch (neu im Paket: shared_assets/RE2/KLOPF1060.*).

* Fuer die neue Marvin-Zeile gibt es noch keine Aufnahme: `synchro/STAGE1/room11B0/main20.wav` ("But, hold on...").
