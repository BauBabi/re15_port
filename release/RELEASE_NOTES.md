# RE1.5 Port — v0.3.28 (Early Preview)

Drei Meldungen, drei Ursachen — und bei zweien war der Fehler meiner.

## Das Feuer in ROOM1090 knistert wieder

Du hattest das zum zweiten Mal gemeldet, und beim ersten Mal habe ich falsch geprueft: ich
hatte nur nachgesehen, ob die Feuer-Spur korrekt VERSTUMMT, und daraus geschlossen, sie sei
in Ordnung. Sie hat nie geklungen.

Das Feuer ist im Original keine normale Musikspur, sondern ein Sonderfall: eine zweite
Sequenz in der Neben-Musikdatei, die sich per Bank-Wechsel auf die Haupt-Musikdatei
umhaengt und dort drei Knister-Samples anspielt. Das zugehoerige Instrument ist ab Werk
stumm und wird erst vom Raum-Skript aufgedreht.

Im Port hat eine einzige Zeile diese ganze Ebene stillgelegt: der Mischer sprang bei jeder
Sequenz ab, die keine EIGENE Klangbank mitbringt — und genau das ist bei dieser Sequenz
normal, sie leiht sich die des Nachbarn. Die Startfunktion daneben hatte die richtige
Pruefung seit jeher; der Mischer nicht.

Belegen laesst sich das ohne Hoertest: ich habe zweimal aufgenommen, einmal mit brennendem
Raum und einmal mit bereits geloeschtem Feuer. **Vorher waren beide Aufnahmen ueber 1,9
Millionen Bilder bitgleich** — der Schalter hat also nachweislich nichts bewirkt. Nachher
unterscheiden sie sich in 38,9 % aller Samples.

Nebenbei betrifft das nicht nur ROOM1090: drei weitere Raeume haben eine solche zweite
Sequenz mit Noten, und sie waren aus demselben Grund stumm.

## ROOM1210: die Haende kommen jetzt wirklich raus

Hier habe ich in der letzten Version einen echten Fehler gebaut. Die Original-Mechanik
laesst die Kreatur beim Ausloesen **nach vorn schnellen** — ich hatte das weggelassen, mit
der Begruendung, ein im Gitter steckender Arm koenne das nicht.

Genau diese Bewegung IST aber das Aus-dem-Gitter-Kommen. Nachgemessen: an ihren
Ausgangspositionen liegen alle zehn Kreaturen **ausserhalb jedes einzelnen der neun
Kamera-Bereiche** des Raums — und beide Fassungen, Original wie Port, zeichnen einen Gegner
gar nicht erst, wenn er dort nicht drinliegt. Es war also nicht "unauffaellig animiert",
es war ueberhaupt nichts zu sehen. Das Modell selbst haette es auch nicht gerettet: der
Arm misst gerade einmal gut 400 Einheiten, das Gitter ist rund 2700 entfernt.

Jetzt fahren sie die Original-Bewegung: drei Bilder schnell vor, dreissig langsam zurueck,
vier wieder vor — unterm Strich 2420 Einheiten nach vorn. Damit landen sie im Flur, im Bild
und in Griffweite. Die Zahlen stehen alle so im Original; im Port sind exakt dieselben 2420
nachgemessen.

Dein urspruenglicher Kritikpunkt bleibt gewahrt: verworfen hattest du, dass **alle zehn auf
einen Schlag** losgehen. Das entscheidet weiterhin jeder Arm fuer sich. Und weil der Port
das Ausfahren wiederholt ausloest (das Original tut es genau einmal), stellt er die Kreatur
beim Rueckzug wieder an ihren Platz — sonst waere sie nach drei Durchgaengen mitten im Flur.

## Ada bewegt sich beim Folgen richtig

Vier Fehler auf einmal, alle in derselben Ecke:

**Falsche Animations-Bank.** Waehrend des Folgens nimmt das Original ein eigenes
Animations-Paar, das der Port nur fuer einen anderen Zustand kannte. Gezeichnet wurde die
Haupt-Bank — dort ist der Geh-Clip eine voellig andere, kuerzere Bewegung und der
Steh-Clip ein Sturz.

**Eingefrorene Pose.** Der Geh-Zustand hat Clip, Bild und Ueberblendung in *jedem* Bild neu
gesetzt statt nur beim Eintritt. Gemessen stand der Bildzaehler konstant auf 1 — die Beine
bewegten sich nicht, waehrend die Figur durch den Raum glitt.

**Doppelter Takt.** Zusaetzlich hat der allgemeine Animations-Takter mitgezaehlt, also zwei
Bilder pro Bild.

**Fehlende Zustaende.** Zum Folgen gehoeren im Original fuenf Zustaende: stehen, gehen, sich
zum Spieler drehen, nah dabei mit halbem Tempo — und **laufen**, mit eigenem Clip und
eigenem Tempo, ab rund 3000 Einheiten Abstand. Der Port kannte drei davon. Ada blieb deshalb
selbst bei grossem Rueckstand im Geh-Clip.

Drei Zustaende, die nur ueber Story-Flags oder einen gepackten Spieler erreichbar sind, habe
ich bewusst offen gelassen und im Quelltext als offen benannt — ihre Animationsseite ist
noch nicht ausgelesen, und ein Zweig ohne Ziel wuerde die Figur einfrieren.

## Wie das gefunden wurde

Alle drei Punkte sind aus dem Original disassembliert, nicht geraten; jede Zahl in den
Fixes traegt ihre Adresse im Quelltext. Der Feuer-Befund ist zusaetzlich gegen einen
Original-Spielstand geprueft, der ROOM1210-Befund gegen die Kamera-Bereiche des Raums.
