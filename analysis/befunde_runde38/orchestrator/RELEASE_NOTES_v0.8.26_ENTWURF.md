# v0.8.26 - 2026-10-?? (ENTWURF, Runde 38)

Ihr Auftrag vom 16.10. (Runde 38) und die zwei Reste der Runde 37 (Zombie-Angriffsgang wie RE2, Verdeckung in den Stages
3 bis 6). Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen; wo
er nicht bestand, wurde nachgebessert und erneut abgenommen. Suite 788 -> ???/??? (volle Suite einmal im
Integrationsbaum, ohne parallele Agenten).

## Was Sie merken

<!-- je integrierter Spur ein Absatz, Nutzer-Sprache, keine Adressen -->

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

## Zu entscheiden

## Neu fuer das Paket / Ihre Dateien

* Der Erzaehltext vor der Lobby ("We barricaded ourselves...") hat bei Leon und Elza noch keine Aufnahme:
  `synchro/STAGE1/room1170/main08..11.wav` (Leon) bzw. `synchro/STAGE1/room1031/main20..23.wav` (Elza).

* Fuer die neue Marvin-Zeile gibt es noch keine Aufnahme: `synchro/STAGE1/room11B0/main20.wav` ("But, hold on...").
