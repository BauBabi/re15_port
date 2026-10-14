# v0.8.25 - 2026-10-14 (Zwischenversion)

Zwischenversion der Runde 37, wie gewuenscht: die Aufzuege sind fertig, dazu fast alles andere. 15 der 17 Spuren sind
gebaut, von einem zweiten Agenten unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen und zusammen
getestet; wo eine Abnahme nicht bestand, wurde nachgebessert und erneut abgenommen. Zwei Spuren folgen am Samstag
(unten). Suite 698 -> 788 (volle Suite einmal im Integrationsbaum, ohne parallele Agenten: 785/788; die drei roten
waren aeltere Pruefungen, die das Karten-Bit ohne die Karte im Inventar setzten - durch die neue Kistenregel
zaehlt das Bit nur noch mit dem Gegenstand; nachgezogen, danach gruen).

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

**Beretta M93R und Glock 18** (Spur G, Ihr Nachtrag). Der Feuerstoss trifft jetzt mit jeder Kugel: drei Patronen,
drei Treffer, je Kugel der Schaden eines Browning-Schusses und eigenes Blut - wie die Custom Handgun in RE2, die ihre
drei Kugeln einzeln auswertet. Das Original wertet den ganzen Stoss nur einmal aus; das war Ihre "Schussverschwendung".
Es gilt fuer Zombies, Hunde, Spinnen, Gorillas, Kakerlaken, Birkin und den Alligator, fuer Leon und Elza und in
allen KI-Einstellungen. Ausnahmen wie in RE2: eine Kraehe nimmt je Stoss nur die erste Kugel (sie stirbt daran), und
der Gitterarm in ROOM1210 nimmt eine Kugel, die beiden anderen gehen an den naechsten Gegner.

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
**Elzas Gegner** (Spur N). Wie im Original laedt Elzas Szenario jetzt JEDEN Gegner aus ihrem eigenen Modell-Archiv -
bisher nur Roy. Sichtbar: in ROOM3091 steht die Figur des Originals statt Ada, in ROOM5081 die zweite Birkin-Form;
mit der RE1.5-KI sitzt der Zombie-Griff auf Elzas Schulter und die Kraehen-Angriffe haben ihre eigenen Bewegungen.
Mit der Standard-KI (RE2) aendert sich an Zombies, Hunden, Kraehen und Spinnen sichtbar nichts. Bei Leon aendert
sich nur der Birkin in ROOM5080 - das Raumskript des Originals schaltet dort selbst auf dieses Archiv um.
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

**Feuerloescher** (Spur I). Es gibt jetzt ein Feuerloescher-Modell (nach dem Item-Bild gebaut - weder RE1.5 noch RE2
haben eines). Leons Loescher haengt nicht mehr im Spind von ROOM1000 ("Nothing unusual."), sondern sichtbar an der
Wand in ROOM1050 rechts der Notausgangstuer nach 1090. Bei Elza liegt in ROOM1091 ein leerer Loescher am Boden. Bei Leon
liegt er in 1090 erst nach dem Loeschen und nur, wenn Sie an der Wegwerf-Frage "Yes" waehlen; bei "No" behaelt Leon
ihn als "Empty Fire Extinguisher" (auch im Spielstand, auch wenn er ihn in die Kiste legt).
**Gegenstaende aus der Kiste** (Spur Q). Wie in RE2 benutzen Leon und Elza einen Gegenstand nur noch, wenn sie ihn
tragen. Bisher genuegte es an elf Stellen, ihn irgendwann aufgehoben zu haben - mit der Kiste liess sich das
ausnutzen: liegt der Loescher in der Kiste, sagt Leon in 1090 jetzt nur "I must hurry up..."; Kartenleser,
Sicherungskasten und die uebrigen Stellen antworten ohne den Gegenstand mit ihrem Original-Text.

**Item-Kiste** (Spur M). Beim Scrollen liegen die Eintraege nicht mehr ueber dem Rahmen: der Rahmen wird wie in RE2
darueber gezeichnet, die Zeilen verschwinden darunter. Die Symbole stehen jetzt buendig am rechten Rand der Liste, kein
Name reicht mehr hinein; die Mengen stehen wie in RE2 unten links im Symbol. Zwei-Felder-Symbole (z.B. die MC51) erscheinen jetzt in der
Kiste, im Inventar-Gitter und im Ausruestungskasten vollstaendig wie in RE2 - vorher war ihre rechte Haelfte verschmiert
bzw. fehlte (Spur R).

**Items auf erhoehten Flaechen** (Spur D). Ein Gegenstand laesst sich nur noch von dem Stockwerk aus aufnehmen, auf
dem er liegt - wie im Original, das jeden Ausloeser an das Band des Spielers bindet. Die Super Redhawk in ROOM3050
bekommen Sie nur noch ueber den vorgesehenen Weg (Kiste, Lift, Kerbe, Klettern), weder von oben noch von unten ohne
Kiste; das gilt fuer Leon und Elza und fuer alle zwoelf so liegenden Items. Dabei nimmt ein Lift den Spieler jetzt
wie im Original auf sein Zielband mit. **Lift A in ROOM2030**: im Original laesst er sich auch von ausserhalb der
Kabine ausloesen, und man sitzt danach hinter einer Wand fest (das hatten Sie im Testlauf gesehen). Im Port faehrt er
jetzt nur, wenn Sie in der Kabine stehen, wie die Hubplattform der RE2-Fassung desselben Raums (ROOM6030). Lift B und
die Lifte in 3050 und 50D0 hatten diese Falle nicht und bleiben wie im Original.

**Ada und Marvin nach Irons' Tod** (Spur O, Ihre Nachtraege 11 und 12). In ROOM11C0 stehen Ada und Marvin jetzt vom
ersten Bild an einander zugewandt. Auf den Knall drehen sie nur die Koepfe; im Gespraech dreht sich niemand, und wer
spricht, sieht den anderen an - auch Marvin bei allen drei Zeilen. Er dreht sich erst, wenn er zur Tuer losrennt. Bei
"Marvin!..." dreht Ada sich ihm einmal nach, ihr Kopf folgt ihm, bis er aus dem Bild ist.


## Grenzen

* **Code-Panel:** In der Raumansicht leuchten die winzigen gemalten Lampen des Lesers nicht (RE2 zeigt seine auch
  im Raum; hier leuchten sie nur im Panel-Bild). Der runde Schein streift die Oberkanten der obersten Tasten. Das
  Panel der Gefaengnistuer (11E0, Yellow Keycard) erscheint im normalen Spiel nie: der Startraum setzt ihr
  Schloss-Flag schon beim Neuspiel (ROOM1240 @0x0055A) - im Original genauso. Schliesst man die Nachrichten so
  schnell wie moeglich, klingt das Aufschliessen die letzten Bilder schon im Raum aus (wie RE2 ROOM2110).
* **Fahrstuehle:** Plattform A in 2030 ist im Port eine Wahl nach RE2 (Kabinenpflicht); ihr Motorton ist aus dem
  RE2-Ton gekuerzt. Klick und Motor starten im selben Bild, RE2 laesst 10 Bilder dazwischen.
* **Zombie-Biss:** Beim Kriecher-Griff an der duennen Wand in ROOM1220 kann Leon in einem Bild durch die Wand
  geschoben werden (schon vorher so; ob RE2 das ebenso tut, ist noch nicht belegt). Im Griff ist Leons Waffe
  ausgeblendet, RE2 zeigt sie.

## Zu entscheiden

* **Feuerloescher:** Fundort (heute ROOM1050, Wand rechts der Notausgangstuer), Groesse (0,47 m), Wortlaut "It's an
  empty fire extinguisher." (am Boden) und "An empty fire extinguisher." (Untersuchen), Spind-Text "Nothing
  unusual." - alles Port-Wahl, aenderbar.

## Folgt am Samstag

* **Zombies wie RE2, Angriffsgang** (Spur L): Zombies heben im Port bisher nie von sich aus die Arme fuer den schnelleren
  Angriffsgang - in RE2 tun sie das in der Naehe, nach Treffern und wenn Sie an ihnen vorbeirennen. Gebaut und in allen
  Punkten abgenommen; offen sind nur Belegtexte und eine dabei gefundene alte Abweichung in der Treffer-Erholung, die
  am Samstag mitbehoben wird. Mit dem Messer wird es dann etwas schwerer, anrueckende Zombies aufzuhalten.
* **Verdeckung in den Stages 3 bis 6** (Spur P): Seit v0.7.19 zeichnete der Port die ergaenzten Vordergrund-Masken
  dieser Stages nie (ein Lader-Fehler) - Figuren liefen dort ueber Moebel und Wandteile, die sie verdecken muessten.
  Gebaut und weitgehend abgenommen; offen sind zwei kleine Punkte (das Kranpult in ROOM3050, die Kopfoberkante).
