# v0.8.12 - 2026-09-26

Die Wegwerf-Abfrage ist drin — mit der Sperre, die Sie verlangt haben. Beim Birkin-Befund
bin ich einen Schritt weiter, aber noch nicht am Ziel, und das sage ich auch so.

## "You don't need this key any more. Discard it?" — jetzt aktiv

> "das Problem das ich Bei dir sah, war das du die discard Abfrage auch bei Toren mit
> Raetsel panels machst, also wo man einen Code eingeben muss. Da ist das natuerlich erst
> dann korrekt, wenn man den zugriffscode den man braucht einmalig richtig eingegeben hat.
> erst dann darf die Abfrage kommen."

Sie hatten recht, und der Grund steht woertlich in den Raumdaten: an den Kartenlesern faellt
die Zeile "You've used the Blue Keycard." **beim Einstecken**, nicht beim Erledigen. Direkt
dahinter geht erst das Tastenfeld auf. Die Abfrage hing an der falschen Zeile.

Sie haengt jetzt an der Zeile "You've opened the lock.", die das Schloss ausgibt, **nachdem**
die vier Ziffern stimmen — und zusaetzlich an dem Bit, das genau dieses Schloss setzt.
Betroffen sind vier Tueren (zweimal Blaue Keycard, zweimal Rote Keycard). Zwei weitere
Tastenfelder, die ich zuerst mitgezaehlt hatte, sind wieder raus: ihr Erfolgs-Bit steht schon
beim Start eines neuen Spiels, der Leser existiert dort also nie. Die uebrigen zehn Stellen
ohne Tastenfeld sind unveraendert.

Belegt ist das nicht nur auf dem Pruefstand, sondern im laufenden Spiel: nach richtigem Code
steht die Abfrage ab Bild 251, zeigt die Blaue Keycard, tippt ihre 44 Zeichen aus und wirft
die Karte auf "Yes" weg. Mit **einer Ziffer falsch** passiert in 2341 Bildern gar nichts —
kein Schloss, keine Zeile, keine Abfrage.

Der erste Abzugsversuch hatte uebrigens nichts gezeigt, und schuld war nicht die Sperre,
sondern mein Messhaken: die Ziffern-Bits liegen in der raumlokalen Flag-Bank, und die wird
beim Betreten des Raums geloescht. Ich hatte sie beim Spielstart gesetzt — sie waren schon
weg, bevor der Raum stand.

Was ich offen lasse, damit Sie es entscheiden koennen: drei Gegenstaende sind keine
Schluessel (Feuerloescher, Zange, Minidisc-Player). Der Text sagt "this key". Gefragt wird
dort trotzdem, weil die Regel an der Benutzung haengt und nicht am Namen — sagen Sie Bescheid,
wenn die drei draussen bleiben sollen.

## Birkin: die Spur ist enger, die Ursache noch nicht bewiesen

> "Jetzt wurde ich von birkin rausgeschoben ausserhalb des begehbaren BEreiches - siehe
> Marker. Das darf nicht passieren."

Zwei Abweichungen in der Reihenfolge sind jetzt belegt, beide mit Adresse:

1. **Der Tentakel-Schub endet ungeklemmt.** Im Original laeuft die Gegner-Schleife **vor**
   dem Spieler-Schritt, und dessen Wandklemme raeumt den Schub im selben Bild auf. Im Port
   laufen die Gegner danach. Gemessen: 6 von 162 reinen Tentakel-Schueben enden auf einem
   unbegehbaren Punkt und werden so gezeichnet — mit der Original-Reihenfolge null.
2. **Der Positionsspiegel ist verseucht.** Das Original schreibt ihn am Bildende, also immer
   wandaufgeloest; der Port schreibt ihn am Bildanfang, also nach dem Gegner-Durchgang des
   Vorbildes. Steht dort eine Lage in der Wand, haelt die Klemme den Spieler darin fest.

**Und trotzdem ist der Tentakel nicht der Taeter.** Um Ihre Marke zu erreichen, braucht es
1460 Einheiten; der groesste gemessene Tentakel-Schub ist 1331. Ueber rund siebentausend
gemessene Bilder liegt kein einziges auf Ihrer Endlage.

Was Ihre Endlage **exakt** reproduziert, ist der Koerper-Schub aus dem Boss-Zylinder: ein
einziges Bild von (5500,-22600) nach (9083,-19232), danach 1200 Bilder unveraendert —
dasselbe z wie Ihre Marke, dasselbe Kleben. Die Luecke, die bleibt: in keinem meiner Laeufe
ist der Spieler auf natuerlichem Weg in diesen Zylinder geraten **und** im Folgebild dort
gelandet. Drei Bilder im Zylinder habe ich, die Verbindung zur Endlage nicht. Solange die
fehlt, baue ich den grossen Umbau der Bild-Reihenfolge nicht ein — der hat Folgen an vier
weiteren Stellen, und ich will ihn nicht auf eine Vermutung stuetzen.

## Kleinigkeiten

Zwei neue Messhaken fuer Abzuege aus dem laufenden Spiel (Flags nach dem Raum-Aufbau setzen,
Tastendruck an einem bestimmten Bild). Reine Pruefstands-Schalter, im Spiel ohne Wirkung.

Suite 337/337.
