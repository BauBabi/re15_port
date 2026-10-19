# v0.8.27 - 2026-10-19

Ihr Auftrag vom 17.10. (Runde 39, 30 Punkte) mit den 17 Nachtraegen bis heute, darunter Elzas Panikphase nach Johns
Hilferuf und die Version im Titelbild. Wie gewohnt hat ein zweiter Agent die Punkte unabhaengig am gebauten Spiel gegen
Ihren Wortlaut nachgemessen - mit einer Ausnahme: Wegen des Guthabens habe ich drei letzte Stellen selbst mit gezielten
Tests abgenommen (die Version im Titelbild, die dritte Nachbesserung der Zombie-Durchbrueche, die letzten beiden
Nachbesserungen von Johns Ruf mit der Tuer in 11B1).
Geprueft unter Windows: alle 913 Unit-, Eigenschafts- und Sonden-Riegel und die 92 Spiel-Laeufe, die diese
Runde beruehrt (Elza, Karte, Tueren, Stimmen, Musik, Titel) - alle gruen. Die volle Suite (1143 Tests) laeuft beim
Bau im Linux-Container; ein roter Test haelt das Release an.

## Was Sie merken

### Elza: Johns Hilferuf und die Panikphase (Nachtraege 10 bis 15)

**Der Ruf in ROOM1041.** Nach der Szene in ROOM1021 ist John verschwunden. Die Panikphase beginnt erst, wenn Elza den
Herzschluessel (Dining Room Key) in ROOM1011 aufgenommen hat: Betritt sie danach den Gang 1041 (der einzige Weg aus
1011/1021), ruft John einmal leise um Hilfe, Elza dreht sich zur Treppe und sagt "Oh, no, this is John! He is in
trouble, I have to help him!" (Ihre Aufnahme, als `synchro/STAGE1/room1041/main40.wav`) - mit Gesten, und zwar
denselben, mit denen sie im Original "There's still a little girl trapped in there!" sagt (ROOM10B1). Die Karte geht auf und
markiert ROOM1141 (3F); sie schliesst nach etwa 4 Sekunden von selbst oder mit START. Danach laeuft in allen Raeumen
Leons Panikmusik nach Irons' Tod (MAIN01), bis beide Gorillas in 1141 tot sind. Im Flur 1131 ruft John noch einmal
"Please help!", deutlich lauter. Lautstaerken wie in RE2: 1041 leise (42 %), 1131 83 %, 1141 voll. In 1141 selbst kommt
Elzas Satz nicht mehr.

**Gesperrte Tueren.** In der Panikphase zeigen die Tuer 1041 -> 1031 und die Tuer 1051 -> 10A1 "I have to rescue John!
He is in danger!" - ein einfacher Raumtext ohne Stimme, wie Leons Tuertexte in seiner Panikphase. Nach Johns Rettung
gehen beide Tueren wieder normal.

**Zombie-Durchbrueche - in beiden Szenarien aus denselben Tueren.**
- Elza, nach Johns Ruf: kurze Pause, Tuerknall, Schnitt ins Treppenhaus 2F: die Zombies, die Sie im Medical Room (10E1)
  am Leben gelassen haben, brechen herein und gehen auf die Kamera zu (haben Sie dort alle getoetet, entfaellt der
  Schnitt). Dann vor dem Lobby Office (1071) die Zombies, die dort noch leben, und das Notfall-Rollo der Lobby (1031)
  bricht wie in der Original-Szene. Im Flur 1131 bricht bei Elza nichts durch (ihr 1141 ist verschlossen).
- Leon, nach Irons' Tod: nach dem Flur vor dem Besprechungsraum kommt derselbe neue Schnitt ins Treppenhaus 2F (die
  Zombies aus 10E0), danach wie bisher Rolltor, Lobby, Parkplatz.
- Im Treppenhaus stehen die Zombies danach moeglichst weit weg an der Seite: von unten am 2F-Absatz unter dem Fenster,
  von oben an der Tuer zur 2F West Side. Sie laufen erst los, wenn Sie am anderen Ende angekommen sind.
- Elza, ROOM1041: Laesst sie die Treppenhaus-Zombies unbesiegt zurueck und betritt 1041 waehrend der Panikphase wieder,
  bricht das Rolltor wie bei Leon.
- Alte Spielstaende (bis v0.8.26), die Irons' Tod schon hinter sich haben, bekommen im Treppenhaus keine Zombies aus dem
  Nichts.

**Parkplatz-Tuer in ROOM11B1 (Ihr Nachtrag 12, korrigiert auf 11B1).** Elzas Tuer von 11B1 zum Parkplatz (11C1) ist
jetzt immer zu - auch bei der Suche nach Sherry. Mit Strom kommt der Original-Text des Raums "The door is locked.",
ohne Strom wie bisher "It's too dark to see anything...". Im Parkplatz 11C1 sind nur die fuenf Hunde, es fehlt also
nichts. Elzas Kanaldeckel in 10B1 bleibt wie im Original.

**Hund durchs Fenster (ROOM1121).** In der Panikphase springt Elza im langen Gang ein Hund durch das zerbrochene Fenster
entgegen (Glassplitter und Glasknalle wie bei Leons Kraehe) und greift an. Nur einmal; Leons 1120 ist unveraendert.

### Elza: ROOM1141, Dach und Funkgeraet

**ROOM1141 (Ihre Nachtraege 4, 7 und 17).** Beim Betreten laeuft die Panikmusik weiter; statt des RE2-Klopfens hoert man
Ihre `door_sound.wav` im Wechsel mit Johns Hilferufen. Elza oeffnet das Rolltor und laeuft danach frei weiter. Erst
wenn sie die Dachtuer entriegelt ("You've opened the lock."), beginnt die Szene: Elza rennt in den Raum, die Kamera
schneidet auf die Ansicht vom Gangende, Elza dreht sich zur Tuer und hebt eine Schusswaffe aus ihrem Inventar (ohne
Schusswaffe das Messer); mit dem Entriegeln endet die Panikmusik, es ist still, und die Gorilla-Musik setzt wie bisher
mit dem Angriff ein. Im Kampf richtet sich John sofort auf einen Gorilla aus und gibt die erste Salve ab, danach etwa
alle 12 Sekunden zwei Schuesse auf den Gorilla, der Elza gerade nicht festhaelt; der getroffene Gorilla wendet sich ihm
bis zu 7 Sekunden zu. Am Ende "Puuuh, that was close! ..." und "Elza,
follow me ..." (Ihre main51/main52) mit dem Heranwinken aus 1171 statt der Waffe auf Elza; danach folgt die Kamera Elza.
Beim Wechsel aufs Dach (1171) klingt die Kampfmusik nicht mehr zwei Sekunden nach.

**Dach (ROOM1171).** Zu "Elza, hurry up!" winkt John Elza heran, statt zu klettern; seine Gesten davor sind die
Original-Gesten. Die Kraehen halten im Schwebeflug nach jedem Fluegelschlag kurz inne und kreisen nach dem Wurf nicht
mehr ununterbrochen - wie im Original. Die erste Kraehe packt Elza dadurch rund 1/3 Sekunde spaeter.

**Funkgeraet (ROOM10F1).** Neue Szene beim ersten Betreten: Elza geht zur Konsole, "Hello, anyone, can you hear me? We
need help!", Umsehen, dann "Uhh, this thing is broken!" (Ihre main40/main42).

**Kleinigkeiten.** In ROOM1031 hebt Elza die Hand nur einmal. Nach Roys Tagebuch geht die Karte direkt nach dem Ablegen
auf. Am Feuerloescher in 1090/1091 (und 1030) klettert sie nicht mehr aufs Autowrack. Im 1141-Kampf kann ein Gorilla
Elza (oder den anderen Gorilla) nicht mehr durch die Wand nach draussen druecken (RE2-Regel).

### Leon

**Stimmen.** Ihre Aufnahmen spielen an ihren Zeilen: ROOM1050 main09-11 (vor Adas Rettung), Irons' Sterbeszene in 1150
bis "SIR, Sir?!" (main22), 11B0 main13-14, 1171 main00-02. Irons' Dateien hebt der Port jetzt 6,9- bis 12,6-fach an
(vorher hoechstens 4-fach).

**Parkplatz (ROOM11C0).** Solange die Gorillas leben, ist die Tuer gesperrt: "I cannot leave Ada alone here with those
monsters!" (einfacher Raumtext ohne Stimme). Nach dem Kampf laeuft die Original-Szene und geht selbst durch die Tuer.

**Spind (ROOM1000).** Erst Handgun Bullets (15 Schuss), danach "Nothing unusual.".

### Kampf und Gegner

- Ueber Tische, Pulte, Betten, Stuehle, Kisten usw. kann man jetzt schiessen; Regale, Saeulen und Waende sperren weiter
  (RE2-Regel). Die Zielhilfe nimmt nur noch Gegner mit freier Sichtlinie.
- Aufstehende Zombies sind wie in RE2 bis kurz vor Ende des Aufstehens nur an den Beinen treffbar (Ihr Entscheid). Tipp:
  D-Pad UNTEN + R1.
- Alle Gorillas: doppelte Energie (360), sie zucken erst beim dritten Handfeuerwaffen-Treffer.
- Hunde: waehrend Ihrer Treffer-Reaktion beisst kein zweiter Hund.
- Liegende Zombies (1010/1011, 1220/1221) kriechen heran, sobald Sie naeher als 3000 vor ihnen stehen.

### Zustand und Inventar

- Volle Heilung stellt die Kleidung wieder her (auch die Weste). Gift zieht etwa jede Sekunde 1 HP ab (nie unter 1), das
  EKG ist dann magenta mit "Poison".
- Alles mit einer Menge stapelt, auch Memory Cards und Granaten, auch in der Kiste (bis 250).
- Elzas Statusschirm zeigt ihren Fuehrerschein statt Leons Bild. Die Yellow Keycard fragt nach dem richtigen Code in B2
  "Will you discard ...?".

### Karten

- 1F, 2F, 3F und B2 nach Ihren map*_correct.png; B1 neu gezeichnet (echter S-Verlauf des Gangs, Parkdeck, Zwinger 1160
  mit Marker). Rot wird nur der Teil eines Raums, in dem Sie stehen. Der Marker in 1090 steht im Raum.
- Bewusste Abweichungen von Ihren Vorlagen: auf 3F drei Treppensymbole (1060 auf x140 wie auf 1F/2F, 1170 auf dem echten
  Lauf, die Treppe bei (168,107) auf y106, weil 1/3 Pixel bei 1x nicht geht); auf B2 die begehbare Suedwest-Zelle in
  ROOM1210, die weder das Original noch Ihre Vorlage zeichnet - sie erscheint, sobald Sie sie betreten haben. Bilder unter
  `analysis/befunde_runde39/G_karten_bilder/`.

### Bild und Ton

- ROOM1100: der Streifen an der Westwand-Ecke deckt die Figur nicht mehr ab.
- Schluessel im Kommunikationsraum (10F0/10F1) hoechstens halb so gross: der Prison Key genau halb, der Parking Lot Key
  etwa ein Drittel (die naechste Stufe waere groesser als die Haelfte). Bei Fensterskala 1 (320x240) sind beide nicht mehr
  zu sehen, bei 960x720 schon.
- Neue Toene: Schublade im Boiler Room (11F0/11F1, RE2), Hunde-Luke und Schrank im Schiessstand (1190/1191; Gitter aus
  Stage 2, Schrank = RE2-Regal). Ihre Raumnummern "10F0" und "1090" meinten diese Raeume.
- Titelbild: oben rechts steht jetzt "Ver.0.8.27" in der Pixelschrift der alten Zeile; "Sample" / "Ver.2025.01.25" ist
  weg. Kuenftig traegt das Release-Skript die Version automatisch ein; eine selbst gebaute exe zeigt "Ver.<letzte
  Version> dev". Das Skript aus dem Vorprojekt (`scripts/patch_save_final.py`) gibt es nicht mehr - erhalten ist nur sein
  Ergebnis, die gepatchte Disc.

## Grenzen

* Irons: "But... I'm not going to make it" und "Please... one last favor" bleiben etwas unter der Musik, weil beide
  Dateien je einen lauten Knall enthalten, der die Anhebung begrenzt. Abhilfe: ohne den Knall neu exportieren.

## Neu fuer das Paket / Ihre Dateien

* Neue Paketdatei: `shared_assets/RE2/TOENE39.VBS` (Schublade, Gitter, Schrank), im Paket-Gate eingetragen.
* Ihre Aufnahmen, umbenannt auf die Nachrichtennummern: `room1141/main50..52` (main50 = Elzas "Oh, no, this is John!",
  als Kopie `room1041/main40.wav` jetzt in 1041 im Einsatz; main51/main52 = John in 1141) und `room10F1/main40`,
  `main42` (`main41.wav` waere optional, z. B. Atmen). Die Rohfassungen liegen unter `synchro/unused/`.
* Keine Aufnahmen noetig (Nachtrag 13): die Tuersaetze in 1041/1051 und 11C0 sind einfache Raumtexte. Fruehere Hinweise
  auf `room1041/main41.wav`, `room1051/main40.wav` und `room11C0/main40.wav` sind hinfaellig.
* Neues Speicher-Bit (Bank 9 Bit 96, Leons Treppenhaus-Umzug); alte Spielstaende laufen weiter.
* Ihre `synchro/STAGE1/room1141/door_sound.wav` ist jetzt im Repo und im Paket und klingt in 1141 im Wechsel mit den
  Hilferufen (eine zweite Fassung unter `synchro/unused/` wird nicht verwendet).

# v0.8.26 - 2026-10-17

Ihr Auftrag vom 16.10. (Runde 38, mit den Nachtraegen zu ROOM20A0 und zu John in ROOM1141) und die zwei Reste der
Runde 37 (Zombie-Angriffsgang wie RE2, Verdeckung in den Stages 3 bis 6). Jeder Punkt wurde von einem zweiten Agenten
unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen; wo er nicht bestand, wurde nachgebessert und erneut
abgenommen. Beim Zusammenfuehren fand die Integration noch drei echte Fehler (der Rueckstoss des schweren Affen-Angriffs,
die im Schacht versteckten Spinnen waren anvisier- und treffbar, ein Bau-Werkzeug haette parallel laufende Tests
beendet) - alle behoben und unabhaengig geprueft. Suite 788 -> 960/960 (volle Suite einmal im Integrationsbaum, jetzt
parallel in gut einer Stunde statt rund vier).

## Was Sie merken

### Elza

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

**Roys Tagebuch (Elza, ROOM1011).** Untersucht Elza den bewusstlosen Roy ein zweites Mal, erscheint statt der
Code-Nachricht jetzt "Roys Diary" - Ihr Text auf RE2-Papier (der FILE16-Hintergrund), der Code 0513 gruen wie in allen
Dokumenten; es wird in den Akten abgelegt und kann dort wieder gelesen werden. Danach markiert die Karte erst den
Communications Room (2F), dann den Prison Room (B2, Johns Zelle) - so wie bei Leons Communications-Room-Hinweis.

**Rolladen-Ton (ROOM1140/1141).** Jedes Hochfahren des Notrolladens hat jetzt die drei Rolladen-Toene aus ROOM1131
(Klick, Motor, Anschlag), bei Elza am Gemaelde-Schalter und bei Leon.

**Elzas Zopf.** Elza hat ihren Pferdeschwanz. Er war in RE1.5 schon angelegt: Elzas Modell traegt vier Zopfglieder,
und das Original rechnet sie mit derselben Pendel-Routine, die RE2 fuer Claires Zopf benutzt - der Port hatte sie nur
nicht gezeichnet. Jetzt haengt der Zopf am Hinterkopf, schwingt beim Gehen und Rennen nach hinten und pendelt beim
Anhalten aus; am Original Bild fuer Bild nachgemessen (235 Bilder rechnet der Port bitgleich nach).

**Elzas Vorspann.** Elzas Intro spricht jetzt dieselben Zeilen wie Leons (dieselben Aufnahmen; legen Sie eigene
Aufnahmen fuer Elza unter `synchro/STAGE1/room1241/` ab, haben diese Vorrang). Im schwarzen Bild hinter dem Erzaehltext
("We barricaded ourselves...") schien bei Elza der Lageplan 1F durch - ein Fehler seit v0.8.24, als die Lageplaene als
Gegenstaende in die Welt kamen; jetzt ist das Bild schwarz wie bei Leon.

**Elza, Toene in ROOM1041 und ROOM1021.** In 1041 faellt statt des Platzhalter-Textes "gunshot" jetzt ein hoerbarer
Schuss, danach dreht sich Elza um ("What was that!?"). In 1021 stoehnt Roy wie ein Zombie, wenn er auf John zugeht.
Johns Schuesse waren schon da, aber mit einer duennen, hellen Aufnahme, die man kaum als Pistole erkannte; jetzt
klingen sie wie eine schwere Pistole (dieselbe Aufnahme, die RE2 fuer solche Skript-Schuesse verwendet). Zum
Gegenhoeren liegen vorher/nachher-Ausschnitte im Analyse-Ordner der Spur.

**Elza auf dem Hubschrauberlandeplatz (ROOM1171).** Elza blieb in ihrer Dachszene stecken: Die Szene wartet darauf, dass
eine Kraehe Elza von hinten packt - diese Kraehe kam im Port nie an (die RE2-KI hatte sie uebernommen, und ihr Signal
erreichte das Szenenskript nicht). Jetzt laeuft die Szene wie im Original durch: die Kraehe packt Elza, John hilft, es
geht hinunter in den Vorhof. Der Griff von hinten kostet wie im Original keine Lebensenergie.

**Elza, ROOM1151.** Klickt Elza den toten Irons an, beugt sie sich nur hinunter und es erscheint "He has already
passed..." - die alte Zeile "I'll be fine..." kommt nicht mehr (ein Fehler seit v0.8.24).

### Szenen und Figuren

**Keine Angriffe mehr in Szenen, in denen Sie sich nicht bewegen koennen.** Solange eine Szene Leon (oder Elza)
festhaelt, greifen Gegner nicht an - sie laufen, kreisen oder knurren, Kraehen im Anflug drehen ab. Das gilt fuer
alle Gegnerarten, nicht nur fuer die Hunde in ROOM1190 und ROOM11D0. Ist der Angriff selbst Teil der Szene (Elzas
Kraehe auf dem Hubschrauberlandeplatz), bleibt er. Nach der Szene greifen die Gegner wieder normal an; die
Zwingerhunde in ROOM1190 beissen wie im Original erst vier Sekunden nach ihrer Landung.

**ROOM11B0, Marvin und der Container.** Marvin sagt jetzt "But, hold on...". Das gemeinsame Verschieben hat einen
Schiebe-Ton - derselbe, den Leon beim Kistenschieben in ROOM1090 hoert (die Tonbank von 11B0 ist an dieser Stelle im
Original leer, die Szene war dort also stumm). Die "Fall"-Bewegung war ein Port-Fehler: Marvin sackte im letzten
Bild des Schiebens noch einmal in die tiefe Schiebehaltung; jetzt bleibt er in seiner Haltung.

**Kendo/John, Ada und Marvin springen Kanten hinunter.** Statt waagrecht in die Luft weiterzulaufen, springen die
Figuren jetzt wie im Original von Stegen und klettern wieder hinauf: John in ROOM10B1 (vom oberen Steg ueber vier
Stockwerke hinunter) und ROOM1261 (Elza), Ada und Marvin in ROOM1260 und in ROOM2000 (Leon). Unten am Westrand des
Blocks in ROOM10B1 springt John noch einmal hinunter und klettert wieder hinauf, solange die Kamera auf dem oberen Steg
bleibt - das macht das Original genauso (gemessen); von der Steg-Kamera aus sieht man es nicht.

**Montage nach Irons' Tod (Leon).** Die Zombies, die in den Gang vor dem Besprechungsraum und vor die Tuer zu ROOM1070
durchbrechen, gehen jetzt auf die Kamera zu - wie die hinter dem Rolltor. Am aufgebrochenen Gittertor in der Lobby
setzen sich die Zombies hinter dem Gitter mit dem Schnitt in Bewegung, gehen zu Boden und kriechen alle unter dem Tor
durch auf die Kamera zu (vorher standen die aufrechten nur hinter dem Gitter, und einer hing an der Torecke fest). Der
Rolltor-Schnitt ist unveraendert. Mit der RE1.5-KI (Menue) dreht sich am Tor ein Zombie anfangs eine Weile auf der
Stelle, bevor er losgeht; mit der Standard-KI gehen alle sofort los.

### Gegner

**Zombies wie in RE2 (Rest aus Runde 37).** Zombies heben jetzt wie in RE2 von sich aus die Arme und kommen im
schnelleren Angriffsgang auf Sie zu - wenn Sie naeher kommen, nach einer Weile von selbst und wenn Sie an ihnen
vorbeirennen; aus diesem Gang greifen sie zu. Das gilt fuer alle Zombies, auch die im Gang nach Irons' Tod. Nach einem
Treffer erholen sie sich wie in RE2 (vorher setzte ihr Gang nach jedem Treffer neu an). Mit dem Messer bleibt es je
Anmarsch etwa gleich schwer (plus/minus ein Treffer, je nach Schlagtakt).

**Spinnen in Stage 2.** Die Schachtspinnen (ROOM2060, ROOM20A0) warten im Schacht, bis Sie den Ausloesebereich
betreten, kriechen dann heraus, klettern die Wand hinunter und landen am Boden; keine Spinne schwebt mehr in der Luft.
Solange sie im Schacht warten oder herauskriechen, kann man sie wie im Original weder anvisieren noch treffen (vorher
drehte die Zielhilfe Leon zur unsichtbaren Spinne, und eine angeschossene Spinne blieb danach in der Luft haengen).
Die Spinnen spucken Gift: Ein Treffer kostet 10 HP und vergiftet (im Statusschirm "Poison"; alle paar Sekunden gehen
2 HP verloren, das Gift toetet aber nicht). Das Antidote Spray (oder ein blaues Kraut) heilt. Nur die Spinne
vergiftet, wie in RE2 - im ausgelieferten RE1.5 setzt der Treffer das Gift nie, dort war das Spiel unfertig. Die
Minispinnen nach dem Tod einer Spinne krabbeln am Boden, bleiben auf ihrem Steg, springen Sie an und beissen (je 1 HP).

**Die kleinen Affen (Stage 3 bis 5) treffen.** Ihr Biss pruefte bisher einen Abstand, den die Koerper-Kollision nie
zuliess - sie bissen ins Leere. Jetzt wie im Original (Bild fuer Bild am laufenden Original gemessen): ein Biss kostet
5 HP und Leon zuckt zurueck; aus groesserer Entfernung greift der Affe schwer an (-10 HP, Leon geht zu Boden) oder
springt. Ein angeschossener Affe springt an die Decke, kriecht ueber den Gang, schlaegt von oben zu und springt Leon
von hinten an; mit schnellem Tastendruecken kommt man frei. Unter 50 HP kann ein Sprung toedlich sein - wie im
Original. Die Affen, die in Stage 4 und 5 erst bei einem Ereignis auftauchen, fallen jetzt aus der Deckenklappe und
greifen vom Boden aus an (vorher liefen sie hoch ueber dem Boden umher).
Beim schweren Angriff wird Leon umgeworfen und rutscht weit zurueck - so weit wie im Original (gemessen); vorher
rutschte er im Port noch ein Drittel weiter und dabei aus dem Raum hinaus. Der Fehler sass im allgemeinen Niederschlag des
Spielers, der jetzt Bild fuer Bild wie das Original ablaeuft - das gilt fuer jeden Gegner, der Leon umwirft.

**Spiderman (Stage 4/5) und Griffe aller Gegner.** Der Spiderman wartet jetzt wie im Original versteckt im Schacht, bis
Sie den Ausloeser erreichen, und klettert dann heraus (vorher lief er sofort los); beim Herausklettern und in seinen
Griffen verschwindet er nicht mehr fuer ein Bild. Vor allem: Leon wird beim Griff nicht mehr aus der Karte geworfen
(vorher bis zu 27 000 Einheiten weit). Fuer alle Gegner gilt jetzt: ein Griff kann den Spieler nicht durch eine Wand
versetzen - in ROOM1220 setzte der Kriecher Leon bisher hinter die Wand.

**Zombies im Wasser (Stage 2) ziehen sich am Rand hoch.** Wie in RE1.5 laufen die Zombies im Kanal zum Rand, greifen
die Kante, stemmen sich hoch und richten sich neben Ihnen auf (ROOM2000 und ROOM2020, Leon und Elza, beide KI-Varianten).
Vorher blieben sie dauerhaft an der Kanalwand stehen.

### Raeume, Bild und Ton

**Cursor ohne Schatten (ROOM1150, ROOM11F0 und alle anderen Cursor-Raetsel).** Der "komische Schatten" war die
Unterseite des Cursor-Kastens: Sie traegt dasselbe Muster, und der Port zeichnete sie durch die durchsichtige
Oberseite mit. Wie das Original zeichnet der Port Rueckseiten jetzt nicht mehr. Das gilt fuer alle 23 Cursor-Raeume
(Generator, Ziffernfelder, Aufzuege, Kranpult usw.) und fuer den Hebetisch in Irons' Buero, bei Leon und Elza.
Nebenwirkungen wie im Original: In Irons' Buero verschwindet ein dunkles Band hinter den Papierstapeln, und eine
Karte an der Wand scheint in ROOM11D0 aus einer Kamera hinter der Wand nicht mehr durch.

**ROOM20A0: der Rolladen steht nicht mehr vor der Kamera (Ihr Nachtrag), dazu durchscheinende Objekte.** Im
Kamerabild ueber der Plattform sieht man beim Kistenschieben jetzt Kiste, Faesser, Regal und die Figur wie im Original
(auch Elza in ROOM20A1): der Zeichner des Originals verwirft Rueckseiten und zu nahe Flaechen, das macht der Port jetzt
genauso - fuer alle Raum-Objekte. Ausserdem mischt der Port halbtransparente Raum-Objekte jetzt wie das Original mit
dem Hintergrund, statt sie deckend zu malen: die Glasscheiben im Labor (ROOM5060) dunkeln das Fenster halb ab, die
Lichtplatte leuchtet rot auf den Boden, das Wasser in den Kanaelen (Stage 2, ROOM11A0, ROOM1260) ist fast schwarz wie
im Original statt einer grauen Flaeche, und der Bodennebel im Kuehlraum (ROOM4090) ist hell und durchscheinend statt
dunkler Schlieren.

**Linux und Steam Deck: dunkle Halbtransparenz wie unter Windows.** Alles, was das Original vom Hintergrund abzieht
(das fast schwarze Kanalwasser, die Figurenschatten, dunkle Effekte), zeichnete der Port unter Linux und auf dem Steam
Deck bisher als gewoehnliche Ueberblendung: Die Grafikschnittstelle, die SDL dort verwendet (OpenGL), lehnte die
Mischart des Ports ab, ohne dass es auffiel. Unter Windows war alles richtig. Jetzt sieht es auf allen Systemen gleich
aus. Gefunden hat das der Linux-Testlauf dieses Release.

**Verdeckung in den Stages 3 bis 6 (Rest aus Runde 37).** Seit v0.7.19 zeichnete der Port die ergaenzten Vordergrund-
Masken der Stages 3 bis 6 nicht mehr (ein Lader-Fehler) - Figuren liefen dort ueber Maschinen, Moebel und Wandteile, die
sie verdecken muessten. Jetzt verdecken sie wieder. Wo eine ergaenzte Maske eine Figur VOR dem Objekt uebermalen wuerde,
ist sie abgeschaltet (auch der Kranpult-Cursor in ROOM3050 bleibt frei).

**Wasser-Effekt ab ROOM1260.** In den gruenlichen Pfuetzen der Kanal-Gaenge (ROOM1260, 2000, 2050, 20B0 und Elzas
Varianten) spritzt bei jedem Schritt am Fuss Wasser auf, dazu der Wasser-Schritt-Ton; im Alligator-Becken (ROOM2090)
und im Wasser von ROOM20A0 spritzt es am Schienbein mit dem Wat-Ton. Laufen hat wie im Original einen eigenen Ton,
und auch in gefuehrten Szenen (Flut-Szene ROOM20B0) spritzt es an derselben Stelle wie im Original. Alle Schritt-Toene
kommen jetzt einen Takt (1/30 s) spaeter - im Gleichtakt mit dem Original.

**Feuerloescher und Schluessel an Ihren Markierungen.** Der Feuerloescher haengt jetzt in der Lobby (ROOM1030) am Pfeiler,
genau an der Stelle aus Ihrem Bild (statt in ROOM1050). Parking Lot Key (Leon) und Prison Key (Elza) haengen gut sichtbar an
der Pinnwand im Communications Room. Der Prison Key ist dafuer jetzt RE2s "Small Key" (heller Bartschluessel mit Ring) -
der bisherige duenne Eisenring war auf dem dunklen Kork kaum zu erkennen. Aufnahme, Inventar-Symbol und Item-Bild zeigen
denselben Schluessel.

**Rekorder-Raetsel (ROOM1100, auch Elza).** Beim Hineinsprechen laeuft jetzt Ihre Aufnahme Irons_Rekorder_Sound.wav;
erst wenn sie zu Ende ist, kommen der Piep (Eingabe angenommen) und das Aufschliessen - zusammen mit der Anzeige ACCEPTED
auf dem Panel, so wie RE2 Ton und Anzeige an seinen Schloessern koppelt. Vorher kam beim Benutzen gar kein Ton.

**Inventar mit Ton.** Oeffnen und Schliessen des Statusbildschirms klingen jetzt wie in RE2 (eigene Toene fuer Oeffnen und
Schliessen; wo RE2 den Schirm selbst oeffnet - Kiste, Aufnahme -, bleibt es wie dort still).

**ROOM30E0, Flammenwerfer.** Er wird jetzt normal aufgenommen (Aufnahme-Abfrage, volle Ladung) statt "not available
in this preview". Sagen Sie Nein, koennen Sie ihn spaeter holen.

### Werkzeug

**Ein Skript fuer alle Systeme.** Doppelklick auf `release/release_alles.cmd` (oder `bash release/release_alles.sh`):
Es baut Windows, Android und Linux/Steam Deck, gibt der neuen Version die naechste Nummer (letzte Stelle +1, nach 99
die naechste mittlere: 0.8.99 -> 0.9.0), packt und prueft die Pakete und bringt die bisherige Version ins Archiv; auf
Wunsch auch Release-Commit, Tag und Push. Vorher zeigt ein Trockenlauf, was passieren wuerde. Bricht ein Schritt ab,
nennt das Skript die Zeile zum Weitermachen. Anleitung: `release/README.md`.

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

* ROOM20A0/20A1: Am Kanalende scheinen gruene Flecken durch das Wasser. Das ist die Effekt-Schleife, die auf Ihren
  Wunsch vom 30.08. ("schlafender Inhalt") aktiviert wurde - das Original startet sie nie. Sie liegt unter dem Wasser;
  seit das Wasser wie im Original durchscheint, sieht man sie. Wenn sie dort stoert, sagen Sie Bescheid.

## Zu entscheiden

* Das Paketarchiv unter C:/workspace/Re15Data ist am 16.10. zwischen 17:19 und 18:26 verschwunden (kein Agent hat
  darauf geschrieben, der Papierkorb ist leer). Alle frueheren Versionen liegen vollstaendig in den Git-Tags. Das
  Release-Skript hat das Archiv mit v0.8.25 und v0.8.26 neu angelegt. Sollen die aelteren Versionen (v0.8.7 bis
  v0.8.14, rund 7,5 GB) aus den Tags wiederhergestellt werden, oder haben Sie das Archiv bewusst entfernt?

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

# v0.8.24 - 2026-10-12

Ihr Auftrag der Runde 36 ist gebaut. Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten
Spiel gegen Ihren Wortlaut nachgemessen; wo er nicht bestand, wurde nachgebessert und erneut abgenommen.
Wo etwas nur teilweise ging, steht es unter "Zu entscheiden" bzw. "Grenzen" (Marker ROOM1100, weitere Tueren,
Parkplatz-Tuer in 11B0). Suite 592 -> 698/698 (volle Suite einmal im Integrationsbaum, ohne parallele Agenten).

## Was Sie merken

**Klettern.** Auf die verschiebbaren Tritte und Leitern kommt man wieder hinauf (ROOM10F0, 11A0, 2040, 2060,
20A0, 5020 und die Elza-Raeume). Sie waren im Port seit dem byte-genauen Klettern (August) nie kletterbar:
die Abfrage liess nur Objekte mit einem Bit zu, das diese Tritte nicht tragen; das Original nimmt jedes aktive
Objekt (PSX.EXE @0x8002d358; das Aktiv-Bit setzt das Spiel bei jedem Objekt, @0x80040990-a4). Dazu kippen
geschobene Kisten jetzt wie im Original ueber Kanten und fallen herunter (ROOM5020/5021: die Kiste faellt auf den
Boden - lautlos, die Tonbank des Raums hat dafuer keinen Ton, im Original genauso -, man kann auf sie klettern;
schiebt man sie unten in die Ecke vor der Ostwand, liegt sie beim naechsten Betreten wieder dort).

**Leons Kleidung** zeigt bei Caution/Danger wieder Blut und Risse. Seit die RE2-KI Standard ist (August),
setzte kein Gegner mehr Wunden. Jetzt wieder: drei ueberlebte Griffe eines Zombies (gleich ob von vorn oder
hinten), Hundebisse (so viele Wundpunkte je verlorenem HP wie in RE1.5), Kraehen. Elzas Flecken sitzen auf ihrer
eigenen Kleidung. Zwei Nebenwirkungen: jeder ueberlebte Zombie-Griff spritzt jetzt auch unter der RE2-KI das Blut
des Originals beim Abwurf; im Kampfbild von ROOM1190 (Leon seitlich) sitzt der erste Hundebiss auf der
Westen-Rueckseite und wird erst sichtbar, wenn Leon der Kamera den Ruecken zeigt.

**Schuesse.** Keine Waffe trifft mehr durch Tische, Baenke, Saeulen oder ueber den Konferenztisch, wenn man davor
steht (wie im Original). Fressende Zombies sind mit der RE2-KI (Standard) unverwundbar, bis sie aufstehen; sie
wachen auf, wenn man naeher als etwa 4 m kommt (mit AI RE1.5 bleiben sie treffbar). Das Blut sitzt an der
Trefferstelle (Brust, Knie, Hals beim Kriecher), ebenso beim toedlichen Treffer, beim Aufstehen, beim Nachbluten
nach einem Taumel-Treffer, am abgerissenen Glied auf dem Boden und beim Kopfzertreten am Kriecher. Ihre Frage zur
Super Redhawk: ja, RE2 laesst Kraehen beim Magnum-Treffer genauso zerplatzen - die 13 Teile fliegen jetzt wie in
RE2 eine halbe bis eine Sekunde sichtbar auseinander (vorher war die Kraehe sofort weg). Das gilt fuer Super
Redhawk, Granatwerfer-Runden und Rakete, nicht fuer Pistole und Schrotflinte - und noch nicht fuer die Colt Python
(s. Grenzen). Tipp: sitzende Kraehen am Boden tief anvisieren.

**Granaten.** Die normale Handgranate (auch Granatwerfer-Sprengrunde und Rakete) setzt Zombies nicht mehr in
Brand - sie zerplatzen nur, die Teile bleiben wie in RE2 russgeschwaerzt. Feuer gibt es nur bei der Brandgranate.
Jeder Druck auf den Abzug wirft jetzt eine Granate (vorher ging sie verloren, wenn R1 zu frueh losgelassen wurde).
Verbraucht wird sie erst, wenn sie wirklich fliegt: wird Leon (oder Elza) in der Wurfbewegung (ca. 0,7 s) gepackt
oder getroffen oder in dieser Zeit der Statusschirm geoeffnet, bricht der Wurf ab und die Granate bleibt im
Inventar; die angezeigte Menge sinkt erst beim Abwurf. Auch ein voller Effektspeicher (viel Gore nach der vorigen
Explosion) kostet keine Granate mehr.

**20 Gegner je Raum** wie im Original (vorher 15): in ROOM1030/1031 und ROOM1040/1041 kommen die letzten fuenf
Zombies jetzt nach. Gleichzeitig stehen weiterhin hoechstens 6 (1030) bzw. 5 (1040) Zombies im Raum - das ist die
Grenze des Raumskripts; die naechsten folgen, wenn die ersten erledigt sind. Alte Spielstaende laden unveraendert.

**Inventar.** Speichern verbraucht eine Memory Card (die letzte verschwindet; Abbrechen behaelt sie, wie in RE2);
ohne Memory Card zeigt das Telefon nur den Hinweis (wie RE2 ohne Farbband). Die letzte Hand-, Saeure- oder
Brandgranate verschwindet nach dem Wurf aus dem Inventar; Leon nimmt dann das Messer, sobald R1 losgelassen und die
Granate explodiert ist (Wahl des Ports - das Original wechselt die Waffe erst im Statusschirm). Das Debug-Menue
springt mit Elza in ihre Raeume (z.B. ROOM1071, ROOM1151).

**Leon, ROOM11B0.** Marvin schiebt den Container mit Leon zusammen weg. Danach: "Hold on." - Marvin laeuft zur
Werkstatt-Tuer, zieht sie zu (Geste und RE2-Tuer-zu-Ton; die Tuer gibt es nur im Hintergrundbild, deshalb springt
sie in einem Bild von offen auf zu - ein Zuschwingen geben die Daten nicht her), schliesst sie ab wie Robert Kendo
im Waffenladen (RE2-Klick; Kendos Nachwippen ist angenaehert, der Klick klingt nicht raeumlich) und kommt zurueck:
"I just want to make sure we don't have any unwanted followers." Die Tuer bleibt von beiden Seiten zu ("The door is
locked."). Die Parkplatz-Tuer schliesst er nicht ab (s. Zu entscheiden).
**ROOM10F0.** Die Kamera bleibt auf Leon, wenn Marvin und Ada gehen; beide gehen unten aus dem Bild, niemand
verschwindet mehr ploetzlich; zum Schluss nur der Tuer-zu-Ton.
**Irons** bewegt nach seinem Tod den Kopf nicht mehr hinter Leon her.

**Irons' Tod / Montage.** Der Tuerknall ist jetzt ein echter Knall aus RE2 (ROOM106 "Zombies breaking in": zwei
schwere Schlaege kurz hintereinander) - an den Durchbruechen am Ende von 1150 (Besprechungsraum -> 1130) und am Ende
von 1040 (1070 -> 1030); am Rolltor 1040 und am Tor 1030 bleibt der RE1.5-Knall. Reihenfolge je Schritt: erst der
Knall, dann sind die Zombies da, dann erst die Kamera auf sie. In ROOM11C0 sprechen Ada und Marvin miteinander
(zugewandt, Blick zum Gebaeude bei "Oh, no, Leon!"). Ihre RE2-Tonliste liegt als RE2_SOUND_LISTE.md in der
Projektwurzel und ist in den Arbeitsregeln verankert.

**Schluessel.** Leon: die Tuer ROOM11B0 -> 11C0 (Parkplatz) ist verschlossen, sobald der Strom an ist ("It's
locked." / "The door is marked "Parking Lot."", RE2-Verschlossen-Ton), bis zum Parking Lot Key; er liegt im Funkraum
ROOM10F0 auf der Konsole links neben der Tuer von 10D0 (RE2-Modell "P. Room Key"). Mit Schluessel: "You've used the
Parking Lot Key." mit RE2-Aufschliess-Ton, danach die Wegwerf-Frage; erst der naechste Druck oeffnet die Tuer.
Elza: der Prison Key liegt an derselben Stelle in ROOM10F1 (RE2-Modell "Cabin Key"), nicht mehr bei Roy; nach der
Befreiung fragt das Spiel, ob Sie ihn wegwerfen wollen. Fuer Elza wird damit der bisherige Nebenweg Pflicht:
Minidisc (1201, hinter dem Draht-Gitter in 11E1) -> Stimmschloss (1101; in 10D1 vorher den Rolltor-Schalter
druecken) -> Blue Keycard (1111) -> Kartenleser (10D1, Code 4312 aus Marvin's Notes in ROOM1021) -> Funkraum (10F1).
Diesen Weg ist ein Pruefer einmal komplett mit Elza gegangen, bis zur Befreiung von John - er ist gangbar (ohne Kampf
rund 10 Minuten, etwa 29 Gegner unterwegs).

**Elza.** ROOM1121: das Fenster ist von Anfang an kaputt, eine Kraehe mehr. ROOM1151: Irons liegt tot da ("He has
already passed..."). ROOM1211: Aufschliess-Ton beim Befreien. ROOM1021: zwei Schuesse und Roys Todeslaut.
ROOM1011: Ihre Aufnahme main16.5 (Roys Stoehnen, Untertitel "Roy: *groans*") laeuft an der Stelle, an der Roy laut
Raumskript bewusstlos wird (zwischen "Please, you have to help him..." und "Roy, wake up!") - die Szene liegt in
ROOM1011, nicht 1101. Roy bleibt dabei im Bild aufrecht (s. Grenzen).

**Story (Leon und Elza begegnen sich nie).** Roy liegt in Leons ROOM1020 erst, wenn der Strom an ist. Leon findet
in ROOM1210 die offene Zelle ("The cell door is open... / Whoever was locked up here is gone."), Elza in ROOM1091
den ausgebrannten Schuppen ("It's a burnt-out shed. / Someone must have put out the fire."). Zehn Textzeilen, die
eine Begegnung andeuteten, und eine Folgezeile sind umgeschrieben (Tabelle unten). Funkraum: "It's out of
commission" (10F0/10F1) und Elzas "I couldn't call for help using the radio" tragen Ihre Lesart (Anlage zerstoert,
Elza hat es versucht); kein Text sagt, Elza sei vor Leon dort gewesen, einen leeren Haken gibt es nicht.
Zu Ihrer Frage nach dem "empty extinguisher": gemeint war, dass Leon in ROOM1090 das Feuer mit dem Feuerloescher
loescht und in Elzas 1091 der leere Loescher als Spur am Boden liegt. Das ist nicht gebaut - der Schuppen-Text sagt
stattdessen "Someone must have put out the fire."

**Karte.** Im Treppenhaus ROOM10A0 sind auf B1 die Treppen eingezeichnet. In ROOM11D0 steht der Marker nach der
Tuer am Eingang, nicht in der Mitte. Der Raum mit Hunde-Luke und Weste ist ROOM1190 (Schiessstand B1), nicht
ROOM1090: Halle und Westen-Kammer sind zwei getrennte Kaesten, jeder leuchtet nur, wenn man drin ist. Die Tuer von
ROOM11C0 liegt waagerecht auf der Wand. Die kleinen Raeume von ROOM1110 und die Fahrstuhlkabine gelten erst als
besucht, wenn man drin war (Fahrstuhl 1F -> 3F: nur 1F und 3F; ebenso der Labor-Fahrstuhl ROOM4020). Damit weicht
die Karte nach Ihrer Vorgabe bewusst von RE2 ab (dort steht die Kabine auf allen Etagen). Dafuer musste ROOM11A0
(Leiter zur Kanalisation) auf das Blatt "DRAINS B2" umziehen - auf B1 gibt es fuer ihn kein Rechteck (Wahl des
Ports; das Original oeffnet die Karte in 11A0 auf B1). In alten Spielstaenden erscheinen die kleinen Raeume von
1110, die Westen-Kammer von 1190 und die Fahrstuhlkabine erst nach erneutem Betreten.

**Tueren.** Der Fahrstuhl ROOM1080 spielt beim Einsteigen die RE2-Fahrstuhltuer (DOOR27: zwei Fluegel schieben
auseinander), die grosse Tuer ROOM2090 <-> 20A0 das zweifluegelige RE2-Schott (DOOR26). Die Tuer 10C0 <-> 10D0 hat
eine neue Textur (helles Holz mit Faser, Drahtglas, Panikstange, Trittblech) statt der "billigen". Karten-,
Ziffernfeld- und Minidisc-Schloesser: das Einstecken der Karte klackt schon bei "You've used the ... Keycard.";
beim Erfolg folgen ein Piep und eine halbe Sekunde spaeter das Aufschliessen, wie in RE2 (ROOM2110); Riegel von der
Innenseite mit dem RE2-Entriegeln. Mechanische Schluessel piepen auch in RE2 nicht.

**Schiessstand ROOM1190.** Die beiden Saetze stehen jetzt hinten an den Zielscheiben (am Tresenende vorbei, die
Bahn entlang, vor der Scheibe Aktionstaste): Leon ganz links und 3. von links "surprisingly large number", die
anderen "does not have many"; bei Elza genau umgekehrt. Die vier Schalter vorn verhalten sich wieder wie
ausgeliefert.
**Ihre Frage zur Kiste in ROOM1090:** die Kiste auf dem DACH (Ankunft aus ROOM10F0) ist die Stufe ueber die
Bruestung: auf die Kiste klettern, einen Schritt zur Mauer, Aktionstaste - das Tor zur Gasse (ROOM1100) geht nur von
der Kiste aus auf (sein Tuer-Record verlangt die Kistenoberkante, Band 6). Die zweite Kiste unten im Hof hat in den
Raumdaten keinen Zweck (oben ist nichts; das Skript rueckt sie nur um 1700 nach Westen) - vermutlich ein unfertiges
Raetsel des Prototyps.

**Masken (PRI).** Ihre Maske fuer ROOM10F0 Cut 2 ist drin: die Absperrung verdeckt Marvin in der neuen Szene.
ROOM1100: der halbtransparente Figurenschatten wurde am Wandfuss (Cut 1 und 2) von Bodenpixeln der Wandmaske
ueberdeckt, ebenso der Zombiekopf an der Suedecke - behoben. Ihre Marker-Datei lag nicht vor; ob das genau Ihre
Stelle war, ist deshalb nicht geprueft (s. Zu entscheiden). Dazu liegt Leons Schatten jetzt in jedem Raum in der
Zeichenordnung des Originals (Schatten-Bucket aus den vier Ecken, PSX.EXE @0x8001b2d8); im gleichen Bucket dunkelt er
den unteren Schuhrand ab. Fuer die Schatten anderer Figuren ist der Original-Weg nicht belegt; bei Gleichstand bleiben
sie wie bisher.

**Lageplaene zum Aufsammeln.** Die RE2-Polizeikarte (karte01) haengt an Ihren fuenf Marken: 3F in ROOM1140
(map3F.bmp zeigt den Konferenzraum, nicht Irons' Buero), 2F in ROOM10D0, 1F in ROOM1030, B2 in ROOM1200, B1 in
ROOM11D0 - fuer Leon und Elza. Aktionstaste davor: "A police 3F map. / Will you take it?" -> Yes -> "You've taken
the / police 3F map." Danach zeigt die Karte die ganze Etage wie in RE2: besuchte Raeume gefuellt, unbesuchte als
Umriss, die Etage ist mit HOCH/RUNTER waehlbar, auch wenn Sie sie nie betreten haben; der Stand liegt im
Spielstand. Auch der Laborplan ROOM5030 macht seine Blaetter jetzt mit HOCH/RUNTER waehlbar (wie in RE2). In
ROOM1200 meldet sich direkt vor der Vitrine weiter die Vitrine; die Karte nimmt man rechts davon. Der Lageplan ist
wie in RE2 kein Inventar-Gegenstand. Die Karte selbst wird wie im Original "flach" ohne Raumlicht gezeichnet
(Attribut-Bit 0x10). Dieser Original-Weg war im Port vorher nicht da - deshalb erscheinen jetzt auch 169 andere
flache Objekte (Rolltor ROOM11E0, Zellentuer ROOM1210, Schubladen ROOM1200, Objekte in 1090/1190/11A0/11B0/11C0 und
spaeteren Stages) in Texturhelligkeit statt fast schwarz.

**Zombies.** Zombies sehen Leon jetzt auch ueber niedrige Moebel (Verhoertisch, Stuehle, Tresen, Terrassenkante,
Gitterstaebe) und kommen um den Tisch herum: nur Zellen der Sicht-Klasse 3 sperren (RE1.5 FUN_8003dcc4), der
Hoehentest ist der von RE2 (FUN_80050858). Ein nach dem Griff weggestossener Zombie faellt nicht mehr in die Ecke
zwischen Stuhl und Schrank (Verhoerraum 1010), aus der er nicht mehr herauskam. Die vier Zombies auf der Terrasse
im Vorhof 10B0 stellen sich an der Kante vor Leon (herunter koennen sie weder in RE1.5 noch in RE2).

**Gorillas ROOM11C0** ("irgendwie verbessern" - im Original wartet der Gorilla an diesen Stellen ebenso; die
Umweg-Suche ist eine bewusste Verbesserung des Ports). Ein Gorilla, der an einem Wagen oder in der engen Gasse
zwischen den Wagen festhaengt, sucht nach rund 12 Sekunden zu Fuss den Weg zu einem freien Platz neben Leon; steht
er in einem Sprungfeld oder weit weg, springt (der Original-Sprung, Path A @0x80117ed4-f74) bzw. laeuft er schon nach
etwa 1,5 Sekunden los. Hinter dem Auto (Ostgasse) greift er jetzt nach 157 Bildern an statt nie; stirbt der
beissende Gorilla, rueckt der wartende sofort nach (105 statt 588 Bilder). An 14 Messstellen sank die Zeit, die
Gorillas feststecken, von 25936 (v0.8.23) auf 6695 Bilder, in der Ferne von 19812 auf 255. Wie im Original wartet im
ersten Kampf nach der Ada/Marvin-Szene der erste Gorilla etwa 12 Sekunden an der Wagenkante (Bild fuer Bild wie im
Original: 363/468/521, Tod +1194). Die Gorilla-Leiche laesst Blut aus (FUN_80116f50 / FUN_8011be54, ~90 Bilder =
0x5a @0x8011be80). Der lebende Gorilla wirft jetzt den grossen Schatten aus den Originaldaten (1700/1800 statt
500 x 600, INIT @0x801171d8-f0) - bitte beim Spielen ansehen (auch Elzas ROOM1141).

**Dach-Kiste ROOM1090/1091 (aus einem Befund der Arbeit am Schiessstand).** Die Kiste auf dem Dach ist die einzige
Stufe zum Tor nach ROOM1100; seit die Objekte fallen koennen, stand die Frage, ob sie vom Dach fallen und den Weg
versperren kann. Sie laesst sich **weder im Original noch im Port bewegen** (am Original mit Haltepunkten in der
Schubkette @0x8002bf14-c0e8 gemessen, im Port mit Leon und Elza ueber den echten Weg aus 10F0/10F1): jeder Schub
setzt sie in die Bruestung oder die Westwand, das Spiel nimmt ihn nicht an (Sperre @0x8002c0b0) - kein Softlock.
Die Kisten in ROOM3050 und ROOM2050/2051 fallen wie im Original (in 3050 ist das der vorgesehene Weg zur Super
Redhawk - im Port liegt sie allerdings auch ohne die Kiste in Reichweite, s. Grenzen). Faellt die 2050-Kiste, steht
sie beim naechsten Betreten wieder auf ihrem Platz (Original und Port, Raumskript @0x01D0E);
ROOM2040 ist ausserdem ueber 2020 -> 2030 -> 2060 erreichbar. Neu dabei, wie im Original: ein ruhendes Objekt wird
jedes Bild aus einer beruehrten Wand geschoben (Eindringtiefe + 18, FUN_8003b558 @0x8002c0d0 / FUN_8003da78
@0x8003b79c) - deshalb laesst sich der Tritt in ROOM2060 jetzt schieben (vorher unbeweglich), und die gefallene
2050-Kiste liegt wie im Original (vorher 593 Einheiten daneben). Dasselbe gilt fuer die Cursor der Schalterfelder
(Generator ROOM11F0 und die anderen Panel-Raetsel): sie bleiben jetzt wie im Original am Rahmen des Feldes stehen,
statt darueber hinauszufahren (am Original gemessen, 14 Cursor). Leon und Elza bleiben in ROOM2050 nicht mehr an
einer Kiste auf einer anderen Hoehe haengen (Hoehentest FUN_8002cabc @0x8002cb84-cbc8). Bedienhinweis ROOM2060: den
Tritt erst von Sueden ganz nach Norden in die Westgasse schieben, dann von Westen in die Kerbe; oben an der Nordkante
mit Blick nach Norden die Aktionstaste - steht er nicht weit genug im Norden, springt die Figur herunter (wie im
Original).

## Zur Freigabe: umgeschriebene Texte

| Raum | alt | neu |
|---|---|---|
| 30C1 (Elza) | Elza: Leon?! How is he? / We got separated back at the RPD. | Elza: Leon? I don't know him. / I never ran into him at the RPD. |
| 30C1 (Elza) | Marvin: He's all right and he's helping / me and Ada escape the city. | Marvin: He's a rookie, and he's helping / me and Ada escape the city. |
| 30C1 (Elza) | Elza: Will you tell him that me and John / and Sherry are here and we're all right? | Elza: Will you tell them that me and John / and Sherry are here and we're all right? |
| 6031 (Elza) | Elza: Where's Leon? | Elza: Sherry! Is anyone else here? |
| 6031 (Elza) | Sherry: He's fighting off monsters. / He wants you to start the train. | Sherry: A policeman! He's fighting off / monsters. He wants us to start the train. |
| 6030 (Leon) | Leon: Where's Elza? | Leon: Is anyone else with you? |
| 6030 (Leon) | Sherry: She's out there fighting / the monsters! | Sherry: Elza is out there fighting / the monsters! |
| 6030 (Leon) | Leon: Keeping the monsters at bay. / Can you start this thing? | Leon: I've never met her. / Can you start this thing? |
| 50B0 (Leon) | Ada: Your other friends are down there / with her too. | Ada: Some other survivors are / down there with her too. |
| 6000 (Leon) | Annette: You must be that cop. / Your friends are already here. | Annette: You must be that cop. / Your friend is already here. |
| 6000 (Leon) | Annette: Come on. Let's join your / friends and get out of here. | Annette: Come on. Let's join the others / and get out of here. |

Jede Zeile ist einzeln aenderbar (re15_port/tools/r36_p/texte_bauen.py). ROOM3060 msg 1-8 (Elza/Sherry/John im
Leon-RDT) blieb unveraendert: der Raum zeigt sie nie, Elza hat ein eigenes ROOM3061.

## Sprachdateien zum Aufnehmen

Ohne Datei laufen die Zeilen stumm mit Untertitel; der Port bindet jede Datei nach Raum und Nachricht.
* synchro/STAGE1/room11b0/ main20.wav ("Hold on.") und main21.wav ("I just want to make sure / we don't have
  any unwanted followers.") - Marvin.
* synchro/STAGE1/room1210/main20.wav (Leon, offene Zelle) und synchro/STAGE1/room1091/main20.wav (Elza, Schuppen).
* synchro/STAGE1/room1190/main06.wav ("This target has a surprisingly large number of bullet holes.") und
  main07.wav ("This target does not have many bullet holes.") - Leon; dieselben Saetze fuer Elza unter
  synchro/STAGE1/room1191/main06.wav und main07.wav.
* optional: synchro/STAGE1/room1151/main20.wav (Elza, "He has already passed...").

## Zu entscheiden

* ROOM1100 ("Leon von halbtransparentem Boden ueberdeckt"): Ihre Marker-Datei lag nirgends im Repo - wo liegt sie?
  Behoben sind der Schatten am Wandfuss (Cut 1/2) und der Zombiekopf an der Suedecke; Ihre genaue Stelle ist ohne
  die Datei nicht geprueft.
* ROOM1011: main16.5 ist ein einzelnes Stoehnen; der Untertitel lautet "Roy: *groans*" (wie Roys Original-Zeile).
  Welcher Wortlaut soll dort stehen?
* Die umgeschriebenen Textzeilen (Tabelle oben) und die zwei neuen Spuren-Texte (offene Zelle 1210, Schuppen 1091).
  Bitte besonders ansehen: 30C1 "...we're all right" klingt nach einer Nachricht an Bekannte; 6031 "A policeman!"
  laesst sich auf Marvin im Wagen beziehen (Alternative "Another policeman!"); 6030 "I've never met her." wirkt
  ausweichend; 6000 (Annette) lieber im Original lassen, weil der Hinweis dort am schwaechsten ist? Ausserdem
  ROOM50B1 "It's Ada." - Elza kennt Ada nicht; so lassen oder z.B. "A woman... Could this be Marvin's 'Ada'?"
* Elza: Im Original gab Roy ihr den Prison Key; jetzt liegt er im Funkraum, aber kein Text im Spiel schickt sie dorthin.
  Soll Roy (oder ein Zettel) einen Hinweis geben, z.B. dass die Beamten den Schluessel in den Funkraum gebracht haben?
* Zwei Vorschlaege der Story-Beratung sind weder gebaut noch abgelehnt: bei Elza die Folgen der Montage (Rolltor 1041
  offen mit Zombies, Tor 1031 aufgebrochen, Rest-Zombies in 1131), passend zu "Irons tot" und "Fenster 1121 kaputt";
  bei Leon eine Blutlache an Roys Platz in 1010 mit Text. Gewuenscht?
* ROOM11B0: Sie schrieben "in Elzas Szenario ist ok, wenn er nur die Werkstatt-Tuer abschliesst". Gebaut ist: auch in
  Leons Szene schliesst Marvin nur die Werkstatt-Tuer ab (die Parkplatz-Tuer bleibt fuer Elza erreichbar). Oder soll
  er in Leons Szene wie urspruenglich bestellt auch die Parkplatz-Tuer abschliessen?
* Werkstatt-Tuer: bei Leon verriegelt Marvin sie von beiden Seiten, bei Elza ist sie nach ihrer Rueckkehr offen
  (Original). Hinnehmen (getrennte Szenarien) oder als Riegel bauen, der sich von der Garagenseite oeffnen laesst?
* Tuerknall: gewaehlt ist RE2 ROOM106 (zwei Schlaege gegen die Tuer). Lieber ROOM205 (Bretter-Barrikade bricht,
  Holz splittert)?
* Texte an der Parkplatz-Tuer und zu den Schluesseln sind eigene Formulierungen in RE2-Form: "It's locked." / "The
  door is marked "Parking Lot."", "You've used the Parking Lot Key.", "Parking Lot Key" / "Prison Key", "It looks
  unique. / Now I can access / the parking lot.", "I can access the / prison cells / with this." - anderer Wortlaut?
* Der Parking Lot Key ist im Eintrittsbild von ROOM10F0 nur etwa 9 Bildpunkte gross, dabei ein Pflicht-Fund.
  Groesser machen (Abweichung vom RE2-Modell 152 x 76) oder so lassen?
* ROOM10F0/10F1 Cut 5/6: die Masken verdecken den ganzen Holzschreibtisch samt Original-Schreibmaschine. Sollen sie
  die Tischplatte freigeben?
* Karte: Sie schrieben "ROOM 1090" fuer den Raum mit Hunde-Luke und Weste - gebaut ist es fuer ROOM1190
  (Schiessstand B1), ROOM1090 ist der Hinterhof. War 1190 gemeint?
* Karte: Wer ein Treppenhaus nur im Erdgeschoss betritt (z.B. ROOM1060), kann schon zu 2F/3F blaettern (leer). Fuer
  den Fahrstuhl ist das abgestellt - soll es fuer Treppenhaeuser genauso gelten?
* Tueren: DOOR27/DOOR26 sind RE2-LABOR-Tueren (oliv-grau mit Bedienfeld / rostrotes Schott "TYPE-P"). Passt die
  Optik im Revier? Sonst wuerden die RE2-Tueren in den gemalten RE1.5-Farben umgefaerbt; die Mechanik bleibt RE2 (die
  alten Archive hatten die falsche Mechanik). Fuer 2090 ist die zweifluegelige Variante gewaehlt, weil die Tuerflaeche
  dort breit ist; RE2s eigenes Kanalschott ist einfluegelig.
* "Einige Tueren ... zum Beispiel": umgestellt sind Fahrstuhl 1080 und Tor 2090. Welche weiteren Tueren meinten Sie?
  Nach unserem Textur-Mass fallen P24B, P06U, P07T und P07G auf.
* ROOM4000 (Blue Master Keycard): Klack, Piep und Aufschliessen liegen ueber dem Tor-Motor-Ton des Originals - beim
  Spielen pruefen, ob das stoert.
* Lageplaene: die 3F-Karte haengt in ROOM1140 (Ihre Marke map3F.bmp zeigt den Konferenzraum) - richtig so? Die Groesse
  ist in Ihre Marken eingepasst (ein Blatt); die RE2-Originalgroesse (1,08 x 0,72 m) waere 1,5- bis 2,8-mal so gross
  (am meisten in ROOM10D0) - lieber die? ROOM1030: im Karten-Rechteck antwortet bis zur Aufnahme die Karte, daneben
  und danach das Muenztelefon - passt das?
* (optional) Marvins Schliess-Geste ist Clip 20 (linke Hand, ebenfalls eine Kendo-Geste); das neue Tuerblatt 10C0 <->
  10D0; Leons Schatten am unteren Schuhrand - nur bei Einwand.

## Grenzen
* Gorillas ROOM11C0: steht ein Gorilla mit der Mitte im West-Sprungfeld #47 und Leon dahinter, springt er einmal und
  bleibt dabei am Start haengen (Port-Landungsklemme; die Original-Landung @0x80118c3c-cc4 ist gegen die
  Flug-Kontaktklemme nicht neu RE'd) - hat Leon < 50 HP, trifft ihn der Original-Sprungangriff trotzdem; danach laeuft
  der Gorilla zu Fuss weiter. In der Nordwest-Ecke und hinter dem ersten Gorilla in der Nordost-Ecke steht der zweite
  an, solange der erste beisst (nur EIN Bissplatz, Geometrie gemessen). Die Umweg-Suche (Rasterscan + A* je Bild) ist
  auf dem PSX-Ziel ungemessen (kein PSn00bSDK).
* RE2-Hunde: der Claim, mit dem in RE2 nur ein Hund zugleich beisst (PL+0x1D3 |= 0x80, @0x801051f8/0x80105200,
  Pruefung @0x80104e5c-68), fehlt im Port - mehrere Hunde koennen in wenigen Bildern nacheinander zubeissen.
* RE2-Zombies weichen beim Taumel-Treffer nicht wie in RE2 entlang der Animation zurueck (@0x80105F28-54), die
  MG-Seitenkomponente fehlt (@0x8010812C-40); Blut waehrend des Griffs (@0x8010C56C) und das Nachbluten des
  zerrissenen Zombies (@0x80106A40-7C) fehlen. Fressende RE2-Zombies tropfen beim Fressen noch kein Blut (@0x80103C8C).
* Mit der Colt Python zerplatzen Kraehen nicht (der Port ordnet die Python bei Kraehen einer anderen RE2-Zeile zu als
  bei Zombies; vorbestehend).
* Kleidung: die Wundstempel der Stages 3-5 (FUN_80112784 / FUN_8010de24 / FUN_8010dfa4) und Birkins eigene Stempel
  (STAGE5) sind weiter nicht portiert; die RE2-Kraehe kennt nur den Griff, nicht Dive/Strike. Wunden bleiben beim
  Laden eines Spielstands erhalten, das Original loescht sie (vorbestehend, FUN_80037c1c).
* Die Feuer-Emitter der Brandgranate tragen einen Welt- statt Anker-Versatz (0,200,0) und brennen bis zum
  Raumwechsel. Der Kopfabriss des Kriechers wirft Fleischbrocken (Id 8) - in Raeumen ohne Raum-Id 5 nur das Hals-Blut.
* Der liegende Spawn-Zombie (EXEC[7]) ist weiter treffbar, bevor er aufsteht (RE2 sperrt ihn).
* Kisten: der Spieler faellt nicht von Objekten mit Bit 0x100 herunter (FUN_8002dc48, Substate 0xd) - er steht an der
  Kante. In ROOM2060 kann die Spinne Leon vom Tritt schieben; er bleibt dann in Trittoehe ohne Standflaeche stehen
  (y -1800) - ob sie ihn im Original dort erreicht, ist ungemessen. Die Fall-Endwerte der 5020-Kiste sind nur am
  Port gemessen; die gefallene 3050-Kiste liegt 70 Einheiten von der Original-Endlage (Fallbahn FUN_8002c444, ohne
  Folge). Nicht portiert: der senkrechte Zweig des Hoehentests (FUN_8002cabc @0x8002cc3c-cce4), der zweite
  Entity-Durchlauf (@0x8002bfec-c034) und Objekt gegen Objekt (FUN_8002cdf4). Der Durchgang 2020 -> 2030 -> 2060 ist
  nur im Port gefahren, nicht am Original.
* Items auf erhoehten Flaechen lassen sich im Port vom falschen Stockwerk aufnehmen - das Original prueft dabei das
  Band der Figur (@0x80042cac-ccc); so liegt die Super Redhawk in ROOM3050 auch ohne die gefallene Kiste in Reichweite
  (12 Items in 2030/2040/2090/3010/3050 betroffen). Der Lift in ROOM3050 setzt im Port das Kollisionsband nicht
  (Member 0x12, Original @0x80041228), der vorgesehene Weg ueber die Kerbe bricht dort ab. Beides war schon in v0.8.23 so
  und wird gemeinsam behoben.
* 20 Gegner je Raum: in ROOM20B0/20B1 schreibt das Original die Slots 20..34 hinter sein Feld; der Port verwirft sie.
  Das PSX-Ziel braucht 5 x sizeof(re15_actor_t) mehr RAM, ungemessen (kein PSn00bSDK).
* Elza ROOM1021: Roy sackt beim Schuss nicht sichtbar zusammen, ein Muendungsfeuer fehlt (RE2 setzt eines, ROOM40A0).
  ROOM1011: Roy bleibt aufrecht, waehrend main16.5 und "Roy, wake up!" laufen (vorbestehend, Original noch nicht
  gemessen).
* Beim Raumwechsel loescht der Port die Sperr-Bits erst nach dem Laden des neuen Raums; das Original loescht sie vorher
  und rettet die des Init-Laufs ueber die Blende (PSX.EXE @0x8001ca44/@0x8001cbcc). In der Irons-Montage ist das
  umgangen, spielweit nicht geprueft.
* Karte ROOM11D0: der Zwinger-Gang noerdlich der Tuerwand hat in der Kachel keinen Platz, der Marker klemmt dort an der
  Oberkante (Platzhalter-Kachel des Prototyps).
* Fahrstuhl: beim AUSSTIEG aus der Kabine (ROOM1080/4020 -> Etage) gibt es noch keine Tuersequenz; RE2 spielt dort
  DOOR27 V0 (ROOM6010 @0x004C2/@0x004E2/@0x00502) - folgt.
* Tueren: geprueft und umgestellt sind nur die zwei genannten Tueren; die uebrigen Port-Tuerarchive sind noch nicht auf
  falsche Mechanik durchgesehen.
* Objekte mit dem Halbtransparenz-Bit 0x2 (FUN_8002c18c @0x8002c2b4 -> FUN_80022f0c, Prim-Code 0x36, STP-Texel-
  Mischung) werden im Port weiter ueber NCCT gezeichnet, nicht halbtransparent - 91 flache Objekte, vorbestehend;
  12 davon ohne halbtransparente Texel (Glas 1040/1041, 20A0/20A1) zeigt das Original deckend und flach, im Port sind
  sie zu dunkel. Lageplaene in den Elza-Raeumen sind nur im Test geprueft; Masken vor der gemalten Wandbasis nur in
  ROOM1100.
* Mit Elza zeigt der Statusschirm weiter Leons Dienstausweis (vorbestehend, nicht beauftragt).

# v0.8.23 - 2026-10-04

Der zweite und letzte Teil Ihres Auftrags der Runde 35 ist gebaut. Mit v0.8.22 sind damit alle
Punkte umgesetzt; jeder wurde von einem zweiten Agenten unabhaengig am gebauten Spiel gegen Ihren
Wortlaut nachgemessen, bei den Affen sieben Mal. Suite 517 -> 592/592.

## Was Sie merken

**Gorillas in ROOM11C0.**
* Ada versteckt sich nach ihrer Szene und kommt zurueck, wenn beide Gorillas tot sind.
* Der Gorilla steigt an derselben Stelle wie im Original aus dem Auto.
* Der bewegliche Teil an der Brust ist weg.
* Die Brust-Schlag-Animation ist da (nach jedem Griff).
* Sie springen erst nach dem dritten Treffer, bei jeder Waffe.
* Die KI ist so zielstrebig wie im Original: ohne Gegenwehr stirbt Leon nach 39,8 s, die Bisse
  fallen bildgleich auf die Bilder des Originals (14 von 14).
* Der Todessprung (Leon unter 50 HP) laeuft wie auf der PlayStation: Leon bricht an seinem Platz
  zusammen, statt durch den Raum zu springen.
* Ein Gorilla, der hinter den Wagen auf der Stelle hochspringt und Leon nicht angreift, tut das im
  Original genauso (am Original gemessen).

**Nach Tod und New Game** bleiben keine PRIs des alten Spielstands mehr stehen. Beim Tod und bei
jedem Raumwechsel werden alle Assets der Raeume davor entladen: Masken, Gegner-Texturen, Stimmen,
Animationsbaenke, Tonbaenke, die Lampen des Generator-Bedienfelds.

**Karte.** Im Fahrstuhl ROOM1080 bewegt sich der Spieler-Marker ueber die ganze gemalte Kabine.
ROOM11F0 und ROOM1200 erscheinen, wenn man drin ist. ROOM1230 zeigt seine eigene Karte. ROOM1210 hat
den richtigen Korridor und alle sechs Tueren.

**ROOM1190.** Die Hunde haben beim Sprung durch die Luke keinen Schatten mehr in der Luft. Die
Zielscheibe ganz links und die dritte von links sagen "This target has a surprisingly large number
of bullet holes.", die anderen beiden "This target does not have many bullet holes".

**ROOM1200.** Der Zombie von der Trage faellt nach dem Minidisc-Player auf die Spielerebene herunter
(die Schwerkraft-Routine des Originals fehlte dem Port).

**ROOM1210.** Leon steht beim Griff der Gitterarme dort, wo er im echten RE2 steht, und haelt den
Kopf wie dort (beim Griff von hinten in der Animationspose, von vorn schaut er den Arm an).

**Super Redhawk an Hunden.** Die Fleisch-Brocken fliegen, schlagen auf und verschwinden nach etwa
zwei Sekunden, statt endlos zu kreisen.

**Inhalt.**
* Die Doppeltueren haben spiegelgleiche Griffe.
* In den eigenen Dokumenten stehen die Codes gruen (4312, 5632).
* In ROOM1010 und ROOM1220 starten die Zombies weiter von der Tuer: man kann sofort umdrehen und
  wieder hinaus.
* ROOM1010: eine Memory Card (x3) im Regal neben der Tuer zu ROOM1020.
* ROOM1090: Schrotflinten-Munition auf dem rechten Aussenluefter.

**Fenster ROOM1120.** Nach Irons' Tod: laeuft Leon in Cut 1 auf das hintere Fenster zu, brechen die
Scheiben mit RE2-Glassplittern und dem RE2-Glasknall, eine Kraehe bricht herein und greift an. Das
Fenster bleibt beschaedigt, auch nach Speichern und Laden.

**Zombie-Maedchen.** Das ist Gegnertyp 0x13 (EM013), eine weibliche Zombie-Variante. Sie kommt nur in
ROOM4050 vor (hinter Tuer 6 und Tuer 7). Sie erschien im Port nie, weil Tueren in denselben Raum nur
in Stage 1 neu luden; jetzt laden sie wie im Original in allen Stages neu. Sie laeuft an, packt,
stirbt und bleibt tot. Der Endkampf ROOM5090 laeuft dabei weiter mit 600 HP.

**Android.** Die Fortschrittsanzeige beim Entpacken bleibt auch auf sehr breiten Displays im Bild. Ein
Datei/Ordner-Konflikt beim Update heilt im selben Start. Das Pruefskript testet seine Urteilslogik
jetzt selbst mit (777 Faelle, 887 erkannte Mutanten, beidseitige Vergleiche).

## RE2-Weltmodelle zum Ansehen

`extracted_re2_items/uebersicht.html` im Browser oeffnen: 87 Item-Modelle aus RE2, oben die "Karten" -
3 Schluesselkarten und die 3 Lageplan-Modelle zum Aufsammeln (Polizeistation, Kanalisation,
Fabrik). Dazu `karten.png`, `kontaktbogen.png`, `katalog.csv`, `lageplaene.csv`.

## Grenzen

* Das Pruefskript erkennt nicht jede denkbare Aenderung an sich selbst: die Funktion apk_pruefen()
  selbst ist noch durch keine Kontrolle abgedeckt (Dossier N_abnahme_3.md).
* Gorillas: nach dem ersten Biss weicht ihre Lage um bis zu 25 Einheiten vom Original ab; der erste
  Angriff kommt ein Bild frueher als im Original.
* Gitterarme ROOM1210: wie im Original ueberschneiden sich Arm und Leon beim Griff in einem Teil der
  Bilder.
* Aus v0.8.22 weiter offen: Sichtlinie und Fresser-Sperre im allgemeinen Schusspfad; der Port fuehrt
  15 statt 20 Gegner je Raum.

# v0.8.22 - 2026-10-04

Der erste Teil Ihres Auftrags der Runde 35 ist gebaut: Handgranate, Werfer und Python, Messer und
Inventar, die neue Szene im Kommunikationsraum und Irons' Tod mit der Knall-Montage. Suite
478 -> 517/517. Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten Spiel gegen
Ihren Wortlaut nachgemessen. Die uebrigen Punkte folgen mit v0.8.23 (siehe unten).

## Was Sie merken

**Neue Szene im Kommunikationsraum (ROOM10F0).** Beim ersten Betreten steht Ada hinten rechts an
den Monitoren (Cut 2), die Kamera wechselt zwischen Ada und Leon, Leon geht zu ihr, und der Dialog
laeuft in Ihrem Wortlaut bis zu Marvins "Alright! Sounds like a plan. Take care Leon!" - mit
Tuerknall, Marvins Auftritt, Vorstellung ("Woman:" bis zur Vorstellung, danach "Ada:") und dem
Abgang der beiden zur Tuer. Danach geht die Karte auf: erst der Parkplatz ROOM11C0, dann Irons'
Buero ROOM1150 (je ca. 4 s); in der normalen Karte blinken beide, bis Sie sie betreten haben.

**Treppenhaus ROOM1060 -> ROOM1040.** Solange Irons nicht geholt ist: "I have to get the Chief
first..." und die Tuer bleibt zu.

**Irons' Tod (ROOM1150).** "Sir!" mit ausgestrecktem Arm, Leon rennt zur Liege, der Dialog in Ihrem
Wortlaut; Irons haelt den Arm wie in der ersten Szene und laesst ihn abrupt fallen - danach ist er
tot und laesst sich nicht mehr ansprechen. "SIR, Sir?!", Leon kniet, schuettelt langsam den Kopf
(2,8 s, Kopf gesenkt) und steht langsam auf (1,6 s).

**Die Knall-Montage.** Leben in ROOM1140 noch Zombies: Tuerknall, genau diese Zahl erscheint in
ROOM1130 bei Cut 0. Dann der Knall wie in der ROOM1030-Szene: in ROOM1040 faehrt das Rolltor hoch
(falls unten) und es kommen immer fuenf Zombies durch - auch wenn Sie dort vorher fast alle
getoetet haben. Leben in ROOM1070 noch Zombies: Tuerknall, sie erscheinen in ROOM1030 bei Cut 7;
noch ein Knall, und die Zombies kriechen unter dem Tor durch (Cut 6). Zum Schluss der Schnitt auf
den Parkplatz (Cut 13): Ada und Marvin drehen sich zum Gebaeude, Marvin "I have to help him,
sorry!" und rennt los, Ada "Marvin!...". Die erschienenen Zombies bleiben in diesen Raeumen.

**MAIN01** laeuft ab dem Ende der Montage durchgehend, in jedem Raum, auch nach dem Laden und auch
dort, wo das Raumskript seine Musik sonst stoppt - bis Leon den Parkplatz erreicht.

**Handgranate.** Sie fliegt nicht mehr durch Waende: sie explodiert an der ersten Wand ihrer
Flugbahn, auch an duennen, schraegen und runden. Die Reichweite ist die der RE2-Explosivrunde
(+-2000 um den Zuendpunkt statt 900 Radius) - auf dieselbe Zombie-Gruppe gemessen 9 statt 1
Treffer. Der Explosionston ist der von RE2. Zombies werden zerrissen und weggeschleudert,
Gliedmassen fliegen durch den Feuerball. Explosionstests fuer Birkin und den Alligator sind dabei.

**Granatwerfer, Raketenwerfer, Flammenwerfer, Colt Python** funktionieren jetzt nach RE2 Retail
(im Original sind die Werfer leere Tabellenplaetze). Granatwerfer mit Explosiv-, Saeure- und
Brandrunden (6 je Magazin); im Inventar mit einer anderen Rundenart kombinieren wechselt die
Munition wie in RE2. Raketenwerfer 4 Schuss, leer klickt er hoerbar. Flammenwerfer mit Strahl, Fuel
und brennenden Zombies. Geschosse enden an allen Wandformen. Die Python laedt mit Magnum-Munition
nach und wirkt wie der Super Redhawk: ein Treffer toetet Zombies und holt einen springenden Affen
herunter.

**Kampfmesser.** Es steht nicht mehr im Inventar und ist immer der Rueckfall, wenn keine Waffe
ausgeruestet ist (R1 zielen, Quadrat stechen). Eine Waffe legen Sie ab, indem Sie die ausgeruestete
Waffe im Statusschirm erneut anwaehlen. Ein Messer in alten Spielstaenden (Inventar oder Kiste)
verschwindet beim Laden.

**ROOM1050 nach der Ada-Szene.** Das Inventar oeffnet wieder. In der Szene redet Leon mit sich
selbst wie in ROOM1170 (im Profil, Hand zur Brust, Geste zur Tuer) statt in die Kamera.

## Sprachdateien zum Aufnehmen

Ohne Datei laufen die Zeilen stumm mit Untertitel.
* synchro/STAGE1/room10F0/ main06..main23.wav: die 18 Zeilen der Kommunikationsraum-Szene in Ihrer
  Reihenfolge (06 Leon "Hey - how did you came in here?" ... 23 Marvin "Alright! Sounds like a plan.
  Take care Leon!"; 08/09 und 21/22 sind je eine Ihrer Zeilen, auf zwei Untertitel geteilt).
* synchro/STAGE1/room1150/ main22..main29.wav: 22 Leon "Sir!", 23/24 Leons lange Zeile, 25/26 Irons,
  27/28 Irons' letzte Bitte, 29 Leon "SIR, Sir?!".
* synchro/STAGE1/room11C0/ main10..main14.wav: Ada/Marvin auf dem Parkplatz.
Die vollstaendige Liste mit Sprecher und Text steht in analysis/befunde_runde35/K_cut10f0.md (§9.10)
und L_cut1150.md (§7).

## Zu entscheiden

* Leon wird von der eigenen Handgranate nicht mehr verletzt (RE2-Regel; RE1.5 toetete ihn unter
  ca. 950 Abstand). So lassen?
* Die RE1.5-Raumdaten kennen keine Hoehe von Hindernissen: Granate, Runden, Rakete und Flamme enden
  deshalb auch an Tischen (Konferenztisch ROOM1140). Soll die Granate ueber bestimmte niedrige
  Hindernisse fliegen, nennen Sie bitte Raum und Stelle.
* Mittlere und hohe Wuerfe ueberfliegen Gegner (kein Kontaktzuender) und zuenden an der Wand.
* ROOM1030: War die Original-Szene des Raums noch nicht gelaufen, bricht das Tor in der Montage auf
  - die Original-Szene kommt danach nicht mehr.

## Grenzen

* Der allgemeine Schusspfad aller Schusswaffen kennt die Sichtlinie des Originals und die
  Fresser-Sperre von RE2 noch nicht: die Python trifft ueber den Konferenztisch und trifft
  fressende Zombies. Gemessen, belegt, noch nicht gebaut.
* Der Port fuehrt 15 Gegner gleichzeitig, das Original 20 - in ROOM1040/1030 erscheinen deshalb im
  normalen Spiel die letzten fuenf Raum-Zombies nie (schon vor dieser Version).
* Die Knall-Toene sind im Mitschnitt belegt, aber noch nicht am Lautsprecher gehoert (der Messrechner
  hat kein Audiogeraet).

## Kommt mit v0.8.23

Gorillas in ROOM11C0 (in Arbeit: Ada versteckt sich und kommt zurueck, Ausstieg aus dem Auto,
Brustteil, Brustschlag und Sprung nach 3 Treffern sind abgenommen; Bisstakt und Wurfbahn werden noch
nachgemessen), Entladen nach Tod und Raumwechsel, Karte (Aufzug 1080, 11F0/1200, 1230, 1210), ROOM1190
(Hunde-Schatten, Zielscheiben), ROOM1200 (Trage-Zombie), ROOM1210 (Gitterarme), Super Redhawk an
Hunden, Doppeltueren, gruene Codes in den Dokumenten, Zombie-Abstaende 1010/1220, Memory Card 1010,
Schrot-Munition 1090, RE2-Weltmodelle, Fenster und Kraehe in ROOM1120, das Zombie-Maedchen und die
drei Android-Punkte.

# v0.8.21 - 2026-09-30

Ihr Auftrag dieser Runde ist gebaut: alle drei Granaten sind voll benutzbar. Suite 463 -> 477/477.

## Was Sie merken

**Handgranate: Wurf, Abprall, Explosion wie im Original.** Leon wirft die Granate in
Blickrichtung (Wurfbogen je nach Zielhoehe hoch/mitte/tief), sie taumelt olivgruen durch die
Luft, springt mehrmals mit Ton ab, bleibt liegen und explodiert gut eine Sekunde spaeter
(36 Bilder) mit Feuerball und aufsteigendem Rauch; Leon wird im Explosionsbild kurz orange
angestrahlt. Der Schaden faellt erst bei der Explosion, im Umkreis der Granate - vorher traf
schon der Abzug sofort. Wurf, Flug, Zuender und Explosion sind die Routinen des Originals.

**Warum das Original abstuerzt - und warum es jetzt nicht mehr passiert.** Ihre Beobachtung
stimmt: im Original haengt das Spiel, sobald die Explosion einen STEHENDEN Zombie trifft. Die
Granate zieht ihm 1000 HP ab und setzt die Reaktion 9; im naechsten Bild springt seine
Todesroutine ueber eine leere Tabellenzelle ins Leere (STAGE1 `80106c00: jalr v0` mit v0 = 0)
- Standbild. Das betrifft auch Zombie-Maedchen, Gitterhaende und G-Birkin. Im Port reagieren
diese Gegner jetzt wie auf RE2s eigene Granatwerfer-Explosion (Sturz-Tod bzw. Treffer).
In ueber 160 Testlaeufen gab es keinen Haenger.

**Saeure- und Brandgranate funktionieren.** Im Original waren beide Attrappen (Wurfanimation,
aber keine Granate). Jetzt fliegen sie wie die Handgranate; beim Aufschlag gibt es RE2s
Saeurespritzer bzw. RE2s Brand mit drei gleitenden Bodenflammen, die rund fuenf Sekunden
brennen und Gegner darin weiter verletzen. Die Toene kommen aus den RE1.5-Baenken
ARMS10/ARMS11 (bytegleich zu RE2).

**Gegnerreaktion bei allen Gegnern.** Zombies fallen um, bei Saeure und Brand mit Veraetzung
bzw. Verkohlung und Schaden ueber Zeit; Hunde zerplatzen, verkohlen oder werden veraetzt;
Kraehen, Spinnen, Gorilla, Kakerlaken, Tyrant, Alligator, G5 und Birkin reagieren nach ihren
Original-Routinen. Ivy bleibt immun, NPCs werden nicht getroffen.

**Vorsicht, Eigenschaden (Original).** Wer naeher als etwa 950 an der eigenen Explosion steht,
stirbt - 1000 Schaden gegen 100 HP. Das tut das Original genauso.

**Granaten beschaffen wie im Original.** Das Item-Debug des Statusschirms ist jetzt eingebaut:
Inventar oeffnen, Cursor auf einen Item-Platz, SELECT, dann R1 (+1), L1 (-1), R2 (+10), L2 (-10)
bis zur gewuenschten Id (Hand Grenade = 9x R1, Acid Grenade 10x, Incendiary Grenade 11x),
Kreis beendet; Menge 255. Die Handgranate im Hebetisch von Irons' Buero (ROOM1150) bleibt.

**Weiteres, das Sie sehen.**
* Effekte (Muendungsblitz, Huelsen, Rauch, Feuer) zeichnen jetzt in voller Helligkeit und mit
  den echten Paletten - vorher halb so hell.
* Beim Zielen dreht Leon wie im Original 24 bzw. 48 je Bild (vorher 72); die automatische
  Zielnachfuehrung folgt dem Original; beim Messerhieb dreht er nicht mehr.
* Im Dauerfeuer werden Figuren kurz vom Muendungsfeuer angestrahlt (Original).
* Der Wasserstrahl in ROOM2000/2001/20B0/20B1 ist wieder ein durchgehender Strahl; Raumeffekte
  starten jetzt auch, wenn man den Raum durch die Tuer betritt.
* Birkin (ROOM3070) bleibt nach dem Tod liegen, statt wieder aufzustehen.
* Das Bauskript beendete bei jedem Build jede laufende re15_pc.exe auf dem Rechner - behoben.

## Grenzen

* Wie im Original fliegt die Granate durch Waende. Auf erhoehten Boeden (Heliport 1170, 1090,
  2030/2050, 20A0, 3040, 5080) landet sie auf der Hoehe, auf der Leon steht; das Original liess
  sie dort durch den Boden fallen.
* Nach dem HE-Tod brennt die Leiche eines RE2-Zombies mit zwei kleinen Flammen weiter
  (RE2-Russ, dort ebenfalls ohne Zeitende).
* Nicht am Bildschirm gemessen: das Zombie-Maedchen (erscheint im Port in ROOM4050 noch nicht),
  Birkin 0x36 in ROOM3080 und der Alligator ausserhalb von ROOM2090 (kein Modell auf der Disc).
* Beim RE2-Hund fehlen die Saeure-Funken, bei der Kraehe fliegen beim Zerplatzen keine Teile.

# v0.8.20 - 2026-09-30

Ihre Hausaufgaben fuer die Nacht sind gebaut — sieben Themen, jedes von einer eigenen Pruefung an der
echten exe abgenommen. Suite 428 -> 463/463. (Die Granaten folgen als v0.8.21 aus der Parallelsitzung.)

## Was Sie merken

**Das Rolltor in ROOM1050 geht nur noch mit eingesetzter Sicherung auf — mit Nahansicht.** Am Schalter
kommt zuerst wie bisher die Frage. Ohne Sicherung zeigt die Nahansicht (Cut 7: leerer Sockel, rote
Leuchte) "I need a fuse to run the shutter." — das Tor bleibt zu. Mit der Sicherung aus Irons'
Hebetisch: "Will you use the Fuse?" -> Ja: die Sicherung verlaesst das Inventar, die Nahansicht zeigt
sie eingesetzt (Cut 8, beide Leuchten gruen) mit "You've used the Fuse."; ab dann oeffnet der Schalter
das Tor wie im Original, mit Frage und Ton. Vorbild ist RE1.5 selbst: dasselbe Raetsel steht fertig in
ROOM2060, die beiden Saetze sind byte-gleich von dort uebernommen. Das Entfernen aus dem Inventar macht
wie in RE2 der Befehl Sce_item_lost (RE1.5 kennt keinen). Gilt auch fuer Elza (ROOM1051). Das Tor ist
der einzige Weg in den Suedteil — die Sicherung ist damit Pflicht; verlieren kann man sie nicht.

**Irons' Hebetisch bedient man jetzt mit dem Cursor.** Aktionstaste an der Westseite des Tisches ->
Nahansicht, und es erscheint der Cursor des Generator-Raetsels (bildgleich), das Steuerkreuz bewegt ihn.
Quadrat auf der Kuppel unten rechts -> Klick-Geraeusch wie am Generator-Schalter, die Kuppel geht auf,
danach wie bisher Sicherung und Granate aus den Faechern. Quadrat daneben -> "Nothing happened.",
danach ist der Cursor wieder frei. Kreuz verlaesst den Cursor; die naechste Aktion bringt ihn zurueck.

**Generator ROOM11F0: frei bewegen, Sperre nur am Ende, zwei gruene Lampen.** Nach jedem Schalter bleibt
der Cursor frei, der Zeiger faehrt nebenher auf den neuen Wert (auch wenn Sie weiterschalten). Erst wenn
die Stellung stimmt (Schalter 1, 3, 5, 7, 9 an), wird die Eingabe gesperrt, der Zeiger faehrt auf 80,
steht 30 Bilder (wie RE2), dann "Power supply OK." und die Flurlichter wie bisher. Die obere Lampe
leuchtet gruen, wenn die linke Spalte (Schalter 1-5) stimmt, die untere fuer die rechte Spalte (6-10);
ein falscher Schalter einer Seite loescht ihre Lampe sofort. RE1.5 hat fuer diese Lampen keine
Leuchtkunst — genommen ist die gruene Schalterlampe desselben Bedienfelds aus RE2 (ROOM2130), mit
RE2s Flimmern. (Alternative, falls lieber: RE1.5s runde gruene Leuchte aus ROOM5060.)

**Ada ruft an der Tuer nach ROOM10A0.** Beim ersten Versuch, von ROOM1050 nach ROOM10A0 zu gehen,
solange Ada nicht gerettet ist, spielt eine Szene: "Woman: Hello? Anyone? Please, get me out of here!",
Leon tritt von der Tuer zurueck (derselbe Rueckschritt wie in ROOM1090 am Feuer), "Leon: Another
civilian survivor." mit der Arm-Geste hin und zurueck, "Leon: I have to help her!" mit dem Arm-Schwung.
Danach steht an der Tuer "I have to help the Survivor first!" — bis zur Rettung in ROOM1090, dann geht
die Tuer normal (mit Tueranimation). Freigegeben wird ueber das Rettungs-Flag (3,0xBB); das naheliegende
(3,0x6E) loescht ROOM1050 beim Wiederbetreten selbst und haette die Tuer fuer immer gesperrt.
Feuerloescher und Ada sind ohne ROOM10A0 erreichbar. Elza hat keine Ada-Rettung und keine Sperre.

**Vier neue Dokumente.** "Police Officer's Final Diary Entry" auf dem Schoss der sitzenden Leiche vor dem
Rolltor (ROOM1050), "Elliot's Diary" auf der Bank in der Umkleide (ROOM1000, Modell wie Irons Diary),
"Marvin's Notes" auf Marvins Schreibtisch (ROOM1020), "Armory Notice" auf dem Verhoertisch (ROOM1010,
neben der Spraydose, die dort im Spiel genau unter Ihrer Markierung liegt). Texte woertlich, Aufnahme
per Aktionstaste, lesbar im FILE-Menue, danach ist das Modell weg; Speichern und Laden behalten alles.
Die Codes stimmen mit den Schloessern im Spiel: 4312 = Communication Room (ROOM10D0), 5632 = Waffenkammer
(ROOM1230).

**Die Leichen in ROOM1110 und ROOM1230 halten Munition.** Evidence Room: "It's a police officer, he's
dead." / "He is holding something." -> Handgun-Munition mit Aufnahme-Abfrage; Nein -> beim naechsten
Untersuchen wieder. Nach dem Ja nur noch "It's a police officer, he's dead." ROOM1230 genauso mit
"A miserable death..." Menge wie die haeufigste Packung in RE1.5 und RE2 (15), mit Ihrer
Halbierungsregel +7 Schuss. Die Original-Zettel mit den Codes, die dort lagen, entfallen — die Codes
stehen jetzt in Marvins Notiz und im Armory Notice.

**Die Leuchtschrift "HEAVEN" in Irons' Buero blinkt.** Cut 2, hinter den Jalousien: das Raumskript
schaltet die Buchstaben ueber den Befehl 0x45 alle 20 Skripttakte (40 VBlanks) an und aus — der Port
kannte den Befehl nicht und zeigte die Schrift immer an. Weil der Befehl allgemein wirkt, zeigen jetzt
auch acht andere Raeume, was das Original zeigt: ROOM3000/3001 und ROOM3010/3011 in der Zombie-Variante
(die gemalten toten Polizisten verschwinden, wenn sie als Zombies aufstehen — am Original in DuckStation
gegengeprueft), ROOM3071 Cut 9 (Lichtleiste aus bis zur Elza-Szene), ROOM1211 Cut 7 und ROOM5060/5061
Cut 11 (nach dem jeweiligen Ereignis). In ROOM1170 blinkt im Original nichts — dort bleibt alles, wie es ist.

## Grenzen

* **Sprachaufnahmen fehlen noch** (RE1.5 hat keine englische Sprachausgabe): Ablage
  `synchro/STAGE1/room1050/main22.wav` (Woman, hoechstens 3,3 s — sonst tritt Leon schon zurueck,
  waehrend sie noch spricht), `main23.wav` ("Another civilian survivor."), `main24.wav` ("I have to help
  her!"). Bis dahin laufen die Zeilen mit Untertitel.
* Rolltor: im normalen Blick (Cut 3) behaelt der kleine Kasten seine rote Leuchte — fuer diesen Cut gibt
  es keine gemalte Variante.
* Hebetisch: das Motorgeraeusch des Tischs kommt jetzt schon beim Druck am Tisch (Folge davon, dass der
  Original-Ablauf unveraendert bleibt).
* Generator: gehaltenes Quadrat legt etwa alle 17 Bilder denselben Schalter erneut um (RE1.5 selbst;
  vorher von der Sperre verdeckt).
* Dokumente: so hell wie die Original-Items der Raeume (byte-true Licht) — wirkt eines zu dunkel, sagen
  Sie Bescheid. Der lange Name "Police Officer's Final Diary Entry" ragt beim Anwaehlen ueber die
  Markierung hinaus; die Ablagemeldung lautet nach RE1.5-Vorlage "The Elliot's Diary has been filed.".
* Leichen: "police"/"holding" klein und "..." statt "…" — so stehen die Woerter bzw. Punkte im
  Original-Zeichensatz (eine Auslassungs-Glyphe gibt es nicht).
* Nur PC-Ziel gebaut; das PSX-Ziel kennt die neuen Teile noch nicht.

# v0.8.19 - 2026-09-29

Ihre vier Auftraege dieser Runde sind gebaut. Suite 416 -> 428/428.

## Was Sie merken

**Speichern nur mit Memory Card.** Wie RE2s Schreibmaschine mit dem Farbband (AOT-Typ 9,
Handler 0x80051AB0; Farbband-Suche nur im INVENTAR, nicht in der Item-Box): ohne Memory Card
kommt nach dem ersten Satz der Stelle "If I had a Memory Card, I could save my progress...", mit
Memory Card "You can save your progress with this. Will you use the Memory Card?" mit Ja/Nein;
Ja oeffnet den Speicherbildschirm. RE2s Wortlaut (0x8009EFCC / 0x8009F01F), nur "Ink Ribbon" ->
"Memory Card". Die Karte wird NICHT verbraucht (Ihr Wunsch; RE2 verbraucht das Farbband).
Gilt an allen 16 Speicherstellen. Aeltere Spielstaende haben noch keine Memory Card — sie liegt
auf Irons' Schreibtisch.

**Karte: nach dem Irons-Hinweis ist 2F waehlbar, der Communication Room bleibt markiert.**
Nach der Szene springt die Karte mit "runter" von 3F auf 2F (vorher uebersprungen, solange 2F
nie betreten war) und zeigt dort den Communication Room im selben Rot/Umriss-Blinken wie im
Hinweis — bis Sie ihn betreten, danach erscheint er als besucht. Das ueberlebt Speichern und
Laden. RE2 selbst hat keine dauerhafte Ziel-Markierung (sein Kartenzeichner FUN_8006E120 kennt nur
besucht / aktuell / unbesucht); gebaut auf Ihren Wunsch, im Stil des RE2-Hinweises.

**ROOM1130 -> ROOM1120 erst nach Irons.** Bis zur ersten Szene mit Chief Irons steht an der
Tuer "I have to report the situation to the chief first..." — kein Raumwechsel, keine
Tueranimation. Die Szene spielt uebrigens in seinem Buero ROOM1150 (Flag 3/94, gesetzt in
ROOM1150 sub08 @0x01110); in ROOM1170 gibt es keine Irons-Szene. RE2 sperrt Story-Tueren genauso,
ohne Ton (ROOM2190 "I have to get back to Ben!"). Elza hat keine Irons-Szene, fuer sie bleibt
die Tuer offen (sonst kaeme sie nie weiter).

**Tueranimationen jetzt fuer 141 von 144 Tueren.** Die 57 Tueren, die es in RE2 nicht genau
so gibt, haben jetzt eigene Tuermodelle: Bewegung, Skripte, Ton und Griff vom naechsten
RE2-Archiv, die Textur so bearbeitet, dass sie der gemalten RE1.5-Tuer entspricht (z.B. die
glatten dunklen Stahltueren im Revier = DOOR07 ohne Lueftungsschlitze, umgefaerbt, Druecker mit
Schild wie gemalt). Die drei Tueren, die RE1.5 auf beiden Seiten verschieden malt, sind jetzt von
beiden Seiten animiert; Treppenhausseiten haben ihre graugruene Farbe. Durchgaenge ohne
Tuerblatt zeigen, was RE2 dort zeigt (Blende mit Ton, Archiv DOOR36). Nicht animiert sind nur
drei Uebergaenge in ROOM1250, die im Spiel nie begehbar sind.

## Grenzen

* Das Hochklettern in eine Nische (T085) nutzt ebenfalls die Blende: RE2 hat fuer Klettern
  nichts Passendes.
* Kleinigkeiten bei den neuen Tueren: Strahlenschild P27S sitzt etwas tiefer als gemalt, Handrad
  P26W bleibt in der Blattmitte, Toiletten-Piktogramme innen gespiegelt, das Gittertor behaelt
  RE2-Rauten, zwei sehr hell gemalte Tueren sind an der Obergrenze der Helligkeit.
* Die Toene sind nach Stimme und Tonhoehe gemessen, nicht angehoert.

# v0.8.18 - 2026-09-29

Ihre drei Befunde zu v0.8.17 sind behoben. Suite 411 -> 416/416.

## Was Sie merken

**Granate und Sicherung liegen jetzt IN den Faechern.** Der hochfahrende Tisch in Irons' Buero
hat unter der Tischplatte zwei offene Faecher mit einer Trennwand. v0.8.17 hatte beide
Gegenstaende oben in die Kuppel zwischen die Papierstapel gelegt — das war falsch. Jetzt liegt die
Granate auf dem Boden des LINKEN Fachs, die Sicherung auf dem Boden des RECHTEN (im Bild
gemessen: Schwerpunkt Granate x 101, Sicherung x 209 von 320). Beide sind zu sehen, sobald der
Tisch aus dem Schreibtisch faehrt, und stehen beim Dialog oben in der Ruhe ganz im Bild. Der
Dialog kommt weiterhin erst, wenn der Tisch oben ruht, erst die Sicherung, dann die Granate.

**Keine Verzerrung mehr beim Oeffnen der Tueren.** RE2 zerlegt die grossen Flaechen eines
Tuerblatts beim Zeichnen in 8 x 8 kleinere Dreiecke (Flag 0x20 im Aufstell-Satz, Routine
DivideGT3, Tiefe 3 @0x80013dc0; Abfrage @0x80014a30). Der Port zeichnete das Blatt als zwei grosse
Dreiecke — deshalb knickten Kanten und Felder beim Aufschwingen zu einem "V". Jetzt wie in RE2
unterteilt: die Kante zwischen Fenster und Feld an DOOR13 weicht in der Mitte der Oeffnung
hoechstens 3,8 statt 27,8 Bildpunkte von der Geraden ab. Nachgeprueft, indem der ORIGINAL-Code aus
der RE2-EXE Befehl fuer Befehl gegen den Port gerechnet wurde (8731 Faelle, 170 864 Teildreiecke
gleich). Betroffen sind die 12 Archive, deren Blatt das Flag traegt; alle anderen Tueren sind
bildgleich wie vorher.

**Das Tor am Landeplatz ist so hell wie das gemalte Tor.** Es war 1,92-fach zu dunkel: die
Tortextur stammt aus dem beleuchteten Hintergrundbild, und das RE2-Tuerlicht hat sie ein zweites
Mal abgedunkelt (Faktor 1,75, Grundhelligkeit 68 @0x800142e8); dazu kamen 9 % Verlust beim
Umrechnen der Farben. Jetzt traegt das Tor RE2s Flag fuer hellere Modelle (Grundhelligkeit 136,
@0x800142b4 — dasselbe Flagwort wie die RE2-Klappe DOOR2B), die Farben werden gerundet, und die
Textur ist auf dieses Licht abgestimmt: gemalt/gezeigt 1,000 an denselben Bildstellen.
Mitbehoben: helle Eckfarben ueber 0x80 hellen jetzt wie auf der PSX auf (Texel x Farbe / 128);
RE2-Tueren werden dadurch nur an wenigen Ecken bis zu 7 Stufen heller — naeher an RE2.

## Grenzen

* Die Granate wirkt im Fach dunkel: das ist das Raumlicht des Schreibtischs, nicht die Lage.
* Das Tor entspricht dem Bild aus Cut 12 (Quelle seiner Textur); Cut 0/11 malen das Schild
  heller.
* Die Aufhellung ueber 0x80 gilt nur in der Tuerszene; Figuren und Raum-Modelle kappen weiter.

## Technik

* `make_package.sh` beendet im Laufzeit-Test nur noch die eigene exe (vorher jede gleichnamige).

# v0.8.17 - 2026-09-29

Ihre drei Auftraege dieser Runde sind gebaut: RE2-Tueranimationen fuer die Tueren, die es in
RE2 genauso gibt, der Hebetisch in Irons' Buero und der Generator in ROOM11F0. Jede Aenderung
hat ein zweiter Agent im echten Spiel gegengeprueft. Suite 405 -> 411/411.

## Was Sie merken

**RE2-Tueranimationen — 83 von 144 Tueren.** Jede Tuer, die RE1.5 aufstellt, wurde im
Hintergrundbild ausgeschnitten und mit den 55 RE2-Tuerarchiven verglichen, Seite fuer Seite am
Bild, jede Zuordnung von einem zweiten Agenten auf Widerlegung geprueft. Wo die gemalte Tuer
DIESELBE ist (gleiche Felder, Fenster, Gitter, Material, Farbe), laeuft beim Durchgehen jetzt die
RE2-Sequenz dieses Archivs — mit dem Ton genau dieses Archivs (35 verschiedene Tonteile in RE2;
der Schliesston klingt wie in RE2 ueber den Raumwechsel weiter). Abgedeckt sind 184 Tuerseiten, u. a.
die Fenstertueren mit Querschild (DOOR13), die Asservaten-Schiebetueren (DOOR19), die X-Tore mit
Warnstreifen in der Fabrik (DOOR15), die Zellentuer (DOOR0A), die Leitern (DOOR16, hinauf und
hinab), die Labortueren (DOOR25/DOOR29), die TYPE-P-Schotts (DOOR26; innen am Kuehlraum das vereiste
DOOR31) und die Hubbuehne (DOOR2D). Ablauf wie in RE2: das Bild dunkelt ab, die Tuer oeffnet
sich, die Kamera faehrt hindurch, dann blendet der neue Raum ein (gut 5 Sekunden je Tuer).
* **Griffe:** der Griff steht in der Animation auf der Seite, auf der er im RE1.5-Bild gemalt ist
  (die Variante wird je Tuerseite danach gewaehlt — RE2 macht es genauso, 24 von 25 geprueften
  RE2-Tueren). Wo die Griff-FORM nicht stimmte, ist der Griff getauscht: ROOM10D0 <-> ROOM10F0
  bekommt den Druecker aus DOOR07 statt des Knaufs, die beiden Kassettentueren ROOM1120/1130/1150
  die Messingstange aus DOOR04.
* **Nicht abgedeckt: 61 Tueren.** 45 davon sind nur AEHNLICH — vor allem die glatten, dunklen
  Stahltueren mit Druecker im Revier (12 Stueck): RE2 hat keine Tuer ohne Lueftung oder Fenster,
  am naechsten kommt DOOR07 mit Lueftungsschlitzen. Weiter: 5 Aufzugseinstiege, 4 im Spiel
  nicht aktive oder unsichtbare Uebergaenge, 3 Durchgaenge ohne Tuer (Schacht, Lueftung),
  3 Tueren, die RE1.5 auf beiden Seiten verschieden malt (dort laeuft die Animation nur von der
  gleichen Seite), und das Tor ROOM1170 (hat seine eigene Sequenz). Diese Tueren wechseln wie
  bisher mit der RE1.5-Blende.
* **ROOM4030 -> ROOM4040 / ROOM4080 funktioniert wieder.** Die zwei schraegen Tueren dort sind
  Vier-Ecken-Saetze (40 Byte); der Port las sie als gewoehnliche Tuer und schickte Sie in einen
  Raum, den es nicht gibt. Jetzt wie im Original gelesen (Trefftest FUN_80014368, Nutzlast
  @0x80042f90).

**Hebetisch in Irons' Buero.** Die Granate liegt LINKS im Fach, die Sicherung RECHTS — im Bild
gemessen, in jedem Bild der Fahrt. Die Sicherung liegt schraeg und reicht zur Haelfte unter den
rechten Deckel: die Oeffnung ist nur 300 breit, anders passen die 406 lange Sicherung und die
Granate nicht nebeneinander. Der Aufnahme-Dialog kommt erst, wenn der Tisch wirklich OBEN RUHT
(nach dem letzten Nachsetzen, Skript `Sleep 30` @0x101A) — vorher kam er mitten im Hochfahren.
Erst die Sicherung, dann die Granate; waehrend der Dialoge steht der Tisch (das Original haelt
die Szene bei einer Aufnahme an, @0x8001db98). "No" bietet den Gegenstand bei der naechsten
Fahrt wieder an.

**Generator ROOM11F0.** Die Abnahme kam bisher schon, als der Zeiger erst bei 62 stand. Jetzt
wie in RE2 (ROOM2130): der Zeiger faehrt auf 80, STEHT dort eine Sekunde (30 Bilder,
@0x0171C), erst dann "Power supply OK.", der Bestaetigungston und danach das Licht. Waehrend der
Zeiger faehrt, nimmt das Bedienfeld wie in RE2 keine Eingabe an (nach jedem Schalter).
Elzas Raum ROOM11F1 hat jetzt ebenfalls den Zeiger.

## Grenzen

* Die Animation gibt es auf PC, Steam Deck und Android; die PSX-Fassung wechselt weiter mit der Blende.
* Tueren, die ein Skript waehrend einer Zwischensequenz ausloest, bekommen keine Animation
  (Port-Entscheidung, sonst liefe sie mitten in der Szene).
* 4 der 184 gebauten Seiten sind im Spiel nie begehbar (ihre Saetze sind im Original abgeschaltet);
  die Tueren selbst sind ueber die Gegenseite abgedeckt.
* Der getauschte Druecker an ROOM10D0 ist im ersten Bild schmal (so zeigt ihn auch RE2 an DOOR07).
* DOOR29 zeigt beim Aufziehen (Variante 1) das Schild von hinten gespiegelt — so steht es in den
  RE2-Daten.
* Die Toene sind nach Stimme, Tonhoehe und Bild gemessen, nicht angehoert (kein Audiogeraet an
  der Bau-Maschine).

## Technik

* Die 24 benoetigten RE2-Tuerarchive liegen unveraendert unter `shared_assets/RE2/DOOR/`
  (bytegleich mit der RE2-Disc); das Paket prueft sie.
* `local_build.sh` beendet vor dem Link nur noch die exe des eigenen Bauverzeichnisses (vorher
  jede `re15_pc.exe` der Maschine).
* Belege: `analysis/befunde_runde31/` (Zuordnung und Vergleichsboegen `tueren_03_zuordnung.md`,
  `tueren_belege/t3_vergleich_*.jpg`).

# v0.8.16 - 2026-09-28

Alle neun Befunde Ihrer letzten Runde sind gebaut, dazu vier Fehler, die erst beim Bauen
aufgefallen sind. Jede Aenderung wurde von einem zweiten, unabhaengigen Agenten gegengeprueft,
der gezielt versucht hat, sie zu widerlegen. Dazu kommt aus einer zweiten, parallelen
Arbeitssitzung die Tuersequenz des Tors am Landeplatz (eigener Abschnitt unten). Suite 360 -> 405/405.

## Was Sie merken

**Sie sterben wieder am Hund.** Der Kehlbiss hat Sie wiederbelebt, sobald waehrend des
Bisses IRGENDEINE Richtungs- oder Aktionstaste fiel — deshalb standen Sie bei 0 Lebenspunkten
wieder auf. In ROOM11D0 mit 20 Lebenspunkten gemessen: vorher 15 von 15 Kehlbissen mit
Tastendruck auferstanden, jetzt 0 von 48, Game Over 48 von 48. Der Port hatte im Original
ein Tastensignal gelesen, das in Wahrheit die Figuren-Nummer des Spielers ist
(`lbu v0,8(s3)` / `andi 0x1` @0x80102010). RE2 entscheidet allein im Schadenseintritt, ob
Sie ueberleben. Kein neuer Fehler: der Code stammt vom ersten Hunde-Port am 16. August; seit
v0.8.14 treffen die Hunde nur zuverlaessiger, deshalb kamen Sie oefter in den Kehlbiss.
Dabei mitbehoben: wer beim Tod gerade zielt, bleibt nicht mehr aufrecht in der Zielpose
stehen (cmd 3 @0x80012ef0).

**Die Karte.** Alle vier Befunde hatten eine eigene Ursache:
* *ROOF, Wand unten blau* und *ROOM1000 blau* — eine fest eingetragene Farbe (16,64,176),
  RE2s Besucht-Blau, war beim Zurueckstellen auf RE1.5-Gruen stehen geblieben. Sie kommt
  jetzt aus der Palette (TEX.TIM @0x0556 / @0x055C). Blau-Zensus ueber alle 13 Blaetter:
  vorher 914 Punkte, jetzt 0. Die Dach-Wandzeile lag zudem auf der gemalten Suedwand und ist
  gestrichen.
* *2F, Tuer die es nicht gibt* — die Marke gehoerte zur Tuer ROOM1090 -> ROOM1100
  (ROOM1090.RDT @0x0213A) und trug eine falsche Rueckfallnummer. Sie sitzt jetzt an der Wand
  zwischen beiden Raeumen; neun weitere Marken ohne Traeger sind gestrichen.
* *Verlust beim Laden* — die Etagen-Angaben wurden nie gespeichert. Bei Ihnen fehlten dadurch
  ROOM1060, ROOM1090, ROOM10A0 und die obere Haelfte von ROOM1170. Rundlauf Speichern/Laden:
  vorher 91 von 102 Orten verlustfrei, jetzt 102 von 102. Ihre vorhandenen Spielstaende
  werden beim Laden gehoben; besuchte Raeume erscheinen dabei auf ihrer Haupt-Etage
  (Port-Entscheidung, weil der alte Stand die Etage nicht kennt). Mit Ihrer Karte vom 27.09.
  nachgeprueft: alle vier Raeume sind wieder da.

**Die Karte oeffnet sich nach Irons.** Wie in RE2 ROOM3010 (Opcode 0x84 @0x800591C4): nach
der Szene blendet die Karte "POLICE STATION 2F" auf, der COMMUNIC. ROOM (ROOM10F0 — nicht
ROOM10D0, das ist der Flur davor) blinkt rot/Umriss alle 0,65 s, mit RE2s Hinweiston alle
1,30 s (gemessen 652 ms / 1306 ms). START schliesst. **RE2 merkt sich danach nichts** — kein
Flag, keine Marke, nichts im Spielstand (0 Bit-Schreiber im ganzen Hinweis-Code); die normale
Karte zeigt den Raum danach nach ihrer gewoehnlichen Regel. So ist es jetzt auch hier. Der Raum
selbst ist zunaechst verschlossen und braucht die Blue Keycard.

**Irons Diary liegt auf seinem Tisch** — an Ihrer roten Marke, genau auf dem gemalten
Klemmbrett. Das Weltmodell ist RE2s Buch (room10E0), die Seiten stehen auf FILE08-Papier:
Titel "IRONS DIARY" und 15 Textseiten mit Ihrem ENGLISCHEN Text, jedes Datum beginnt eine
neue Seite, Wort fuer Wort (408 von 408 Woertern aus den fertigen Seiten zurueckgelesen). Aufheben oeffnet wie in RE2 sofort den Leser,
blaettern geht wie in RE2, nach dem Schliessen kommt "The Irons Diary has been filed." und
das Buch verschwindet vom Tisch. Die Toene sind RE2s (Bank 4, Saetze 4/5/6/8; CORE00 ist in
beiden Spielen bytegleich).
* Die 21 vorinstallierten FILE-Eintraege ("Albert Wesker", "Umbrella File 9" ...) sind weg.
  Die Liste beginnt leer und wird im Spielstand mitgespeichert.
* Jedes Zeichen stammt aus den Glyphen der RE2-Originalseiten; konstruierte Zeichen gibt es
  im englischen Text keine mehr.
* Drei Seiten tragen nur einen Zeilenrest ("earth is going on?", "can.", "hope you make it
  out alive!") — Folge des RE2-Seitenrasters mit 9 Zeilen, in dem jedes Datum neu beginnt.
* Die Blaetterpfeile stehen an RE2s Stelle und in RE2s Gruen; an RE1.5s Stelle haette der
  linke Pfeil den ersten Buchstaben mancher Zeilen verdeckt.

**Die Memory Card liegt daneben** (Ihre blaue Marke). Es ist Item 0x21 — nicht 0x20, das ist
die Incendiary Capsule. RE1.5 hat dafuer Bild und Icon, aber nie eine Platzierung und kein
Weltmodell; das Modell ist die Keycard des Spiels mit einer Textur aus dem Item-Bild. Menge 3,
wie RE2s Farbband (21 von 21 Platzierungen).
* In der Nahaufnahme (Cut 6) waren beide fast schwarz, weil dort ein sehr dunkles Licht gilt.
  Sie nehmen jetzt den Lichtsatz von Cut 2 (ROOM1150.RDT @0x003E8) — der hellste der neun
  vorhandenen, messbar der beste; Port-Wahl.

**Tueren klingen verschlossen.** RE2 hat genau EINEN Verschlossen-Satz (Se_on Bank 2 /
Satz 0x16, @0x80051610), dessen Welle je Raum wechselt; RE1.5 ist an verschlossenen Tueren
stumm. Eingebaut an 54 Stellen: Kartenleser, Pincode und elektronisch verschlossen klingen
nach RE2s Kartenleser-Tuer, mechanisch verschlossen nach RE2s Revier-Tuer, die Tuer ohne
Strom in ROOM5080 nach RE2s Gegenstueck in ROOM7020.
* ⛔ **ROOM2190** hat in RE2 (Leon) weder Kartenleser noch Pincode-Tuer und keinen
  Verschlossen-Ton. Die Kartenleser-Tuer liegt in ROOM2110 — deren Ton ist es geworden.
* Stumm bleiben, wie in RE2: falscher Code am Ziffernfeld, Lesegeraet ohne Karte, reine
  "The door won't open!"-Stellen.
* Zum Vergleichen liegen die Wellen in `analysis/befunde_runde30/tuer-verschlossen_wav/`.
  Die Zuordnung steht an EINER Stelle im Code und laesst sich tauschen.

**Das Titelmenue pulst im Original-Takt.** Die Pulsfolge war richtig, der Takt nicht: der Port
schaltete je Bild IHRES MONITORS weiter, das Original je zwei Bildwechsel der Konsole. An einem
144-Hz-Monitor gemessen: vorher 416 ms, jetzt 2006 ms je Periode, wie das Original (120
VBlanks). Unabhaengig von der Bildrate des Monitors. Die Einblende des Titels dauert jetzt
ebenfalls die Original-Zeit (1,07 s statt 0,46 s), und die Helligkeit der gewaehlten Zeile ist
pixelgleich mit dem Original-Bildpuffer.

**Android: R1 rastet ein.** Einmal tippen hebt die Waffe, sie bleibt oben, Viereck feuert,
nochmal tippen senkt. Der R1-Knopf zeigt den eingerasteten Zustand mit einem zweiten Rahmen.
In Menue, Kiste, Text, Zwischensequenz, Raumwechsel und Tod faellt die Raste von selbst. Nur
auf dem Touch-Overlay — Gamepad und Tastatur bleiben Halte-Tasten.

**Elza.** Alle drei Befunde hatten eine eigene Ursache im Port, keine in den Daten:
* Das kurze Lobby-Bild im Intro und das Wiederholen nach dem Abbruch: ROOM1031 hielt sich
  fuer unbesucht, weil das Vorspann-Flag (3,193) nur fuer Leon gesetzt wurde. Es ist die
  gemeinsame Weiche beider Startraeume (ROOM1031 main00 @0x0204E).
* Leon statt Elza in der Lobby: der Modell-Index wurde beim Start von der SCD-Initialisierung
  wieder genullt; der erste Raumwechsel lud PL00.
* Ihre Szene spielte nicht: die Selbst-Tuer-Pruefung kannte die Elza-Variante der Raum-Id
  nicht (0x1030 != 0x1031). Das Original laedt bei JEDER Tuer neu (@0x8001d988).
Leon ist bitgleich geblieben (15 Bilder, 0 abweichende Pixel).

**Die Sicherung im Hebetisch ist sichtbar — und es ist dieselbe.**
* Nach dem Laden eines Spielstands wurde sie gar nicht angelegt; sie lag ausserhalb des
  Fachs, ein Viertel jeder Modellflaeche blieb offen, und das Licht fiel von innen. Jetzt
  liegt sie im Fach: 348 bis 439 Punkte in jedem Bild der Fahrt, vorher 12 bis 124 bzw. 0.
* Das Item-Bild gehoerte zu einem ANDEREN Gegenstand — bytegleich RE2s "Fuse Case". Bild und
  Icon kommen jetzt aus dem Modell, und das CHECK-Foto, das bisher leer war, zeigt die
  Sicherung.
* Sagen Sie "No", bietet die naechste Fahrt des Tisches sie wieder an (wie im Original, das
  eine abgelehnte Aufnahme scharf laesst, @0x8001e068-0x8001e0ec).

**Neben der Sicherung liegt eine Handgranate** und faehrt mit hoch (Item 0x09 "Hand Grenade",
die einzige Granate mit Wurf, @0x8003368c). Nach dem Modal der Sicherung kommt ihres; jede hat
ihr eigenes Flag, "No" bei der einen laesst die andere unberuehrt, und auch nach dem Laden
eines Spielstands ist sie da. Ein liegendes Granatenmodell gibt es in RE1.5 nicht — das Modell
ist die Granate aus Leons Hand-Netz dieser Waffe (PL00W09.PLW), mit den Original-Farben.
Menge 1 (Port-Wahl: RE1.5 platziert die Granate nirgends).
* ⛔ **Werfen geht noch nicht richtig:** Ausruesten, Wurfanimation und Abzug laufen, aber Flug,
  Abprall und Explosion fehlen im Port (drei Effekt-Routinen des Originals, @0x8001843c /
  @0x80018320 / @0x8001854c, wurden nie nachgebaut); der Schaden faellt bisher schon beim
  Abziehen. Das ist der erste Punkt der naechsten Runde.

**Kein Ein-Bild-Blitz mehr beim Kamerawechsel.** Am Telefon in Irons' Office erschienen Buch
und Karte fuer ein Bild gross und dunkel; derselbe Fehler liess vorher das Spielermodell an
manchen Kamerawechseln ein Bild lang falsch stehen. Projektion und Hintergrund wechseln jetzt
im selben Bild.

**Birkin wartet wieder, bis das Skript ihn freigibt.** Beim Bauen der Tuer-Toene blieb Leon
in ROOM5080 nach der Generator-Folge haengen. Die Ursache war nicht die Folge: Birkin lief ab
dem Betreten des Raums los, ging durch die Nordwand und biss den stehenden Leon. Im Original
ist Birkin bis zur Freigabe EINGEFROREN — seine Wurzel ueberspringt bei gesetztem Bit 0x20 die
ganze KI (STAGE5 @0x80116a88, STAGE3 @0x80116274), und beim Erscheinen laeuft sie genau einmal,
damit er in seiner Ausgangspose steht (@0x8004256c-0x80042608). Das fehlte im Port. Betroffen
waren 9 von 13 Birkin-Auftritten (ROOM3070, 3071, 3080, 5080, 5081, 50E0, 50F1 und die
Endkampf-Raeume). Je Raum gemessen: vorher lief er in 299 von 300 Bildern vor der Freigabe,
jetzt in 0; die Freigabe wird in allen neun Raeumen auf dem Weg eines Spielers erreicht, und
danach kommt sein Auftritt (EMERGENCE) wieder, der vorher uebersprungen wurde. Der Endkampf in
ROOM5090 ist byte-gleich zu vorher (13 Birkin-/G5-Sonden unveraendert).

## Warum der Linux-Bau ueber eine Stunde dauerte

Sie hatten Recht, das war nicht normal. Der Container las jede Datei ueber die
Windows-Freigabe (WSL2, 9p-Mount), und dort kostet JEDE Dateioperation rund 10 ms: 5000-mal
stat 54 s statt 3 s, die Spieldaten lesen 142 s statt 0,17 s. Configure, Uebersetzen und die
Tests bestehen fast nur aus solchen Zugriffen. Jetzt wird der Quellbaum zu Beginn in den
Container kopiert und dort gebaut; das Repo haengt nur noch als Rueckfall darunter, sodass
kein Test still eine Datei verliert (eigener Test-Fingerabdruck als Gate). Die Bau-Werkzeuge
liegen in einem vorgebauten Image, statt bei jedem Lauf neu installiert zu werden.

| | vorher | jetzt |
|---|---|---|
| Linux/Steam-Deck-Bau mit voller Testsuite | 82 min | gut 9 min |
| Windows-Bau | 4,4 min | 1,3-2 min |

Die Programme sind bis auf Bau-Kennung und Zeitstempel byte-gleich. Dieses Paket ist bereits
auf dem neuen Weg gebaut, die Linux-Suite im Container: 405 von 405.

## Das Tor am Landeplatz (ROOM1170) - Tuersequenz nach RE2

Das Gelaendertor am Hubschrauberlandeplatz (ROOM1170) oeffnet sich jetzt wie eine Tuer in
Resident Evil 2: mit eigener Tuerszene, Bewegung und dem Ton des RE2-Gittertors. Grundlage
ist das Tuermodell, das ich Ihnen aus Ihren drei Ausschnitten gebaut habe.

### Was Sie merken

**Das Tor hat eine Tuersequenz.** Gehen Sie am Landeplatz an das Tor und druecken Sie die
Aktionstaste: das Bild dunkelt ab, das Tor erscheint vor Schwarz, oeffnet sich, die Kamera
geht hindurch, dann blendet die andere Seite ein. Von der Laufsteg-Seite zurueck sehen Sie
das Tor von hinten, die Angel links, und es wird zu Ihnen hergezogen. Dauer wie in RE2:
291 Bilder bei 60 Bildern pro Sekunde, also knapp 5 Sekunden, dazu das Abdunkeln davor.

**Der Ton ist der des RE2-Gittertors.** Beim Oeffnen der Anschlag des Gittertors (DOOR2E),
nach dem Ausblenden der Schliesston - beide unveraendert aus RE2, in RE2-Lautstaerke. Die
Datei liegt als `shared_assets/RE2/TORSE.VBS` im Paket.

**Warum RE2 und nicht RE1.5.** RE1.5 hat die Tuermaschine vollstaendig und startet sie bei
jedem Tuerwechsel, aber das einzige Skript im Tuerarchiv ist "Ende" (`01 00` in DOOR00.DO2).
Die Sequenz war nie bespielt - ein unfertiges System, also gilt RE2. Die Einblendung des
Raums nach der Tuer bleibt RE1.5: die ist fertig.

### Wie es gebaut ist

- **Das Modell** kommt aus den Hintergrundbildern: jeder Texel ist ein Pixel aus Cut 12,
  die Masse aus den Raumkameras. Rohrrahmen mit runden Ecken, Warnschild, Laschen, Fuesse,
  ein Pfosten an der Angel; 180 Dreiecke.
- **Die Bewegung** ist die von DOOR2E, Byte fuer Byte: Einblenden, Anschlag in Bild 100,
  Schwenk um 50 Grad in 80 Bildern, Kamerafahrt, Ausblenden. Weggelassen ist nur der Riegel
  von DOOR2E, den das Tor nicht hat; dazu kommt der Pfosten, der mitfaehrt.
- **Die Maschine** ist die RE2-Skriptmaschine der Tuersequenz. Ein neuer Test laesst sie
  Bild fuer Bild gegen einen unabhaengig geprueften Simulator laufen, fuer DOOR2E selbst und
  fuer das Tor, je in beiden Richtungen: 0 Abweichungen in 4 x 291 Bildern. Eine zweite
  Sonde prueft an den echten Raumdaten, dass beide Richtungen die Sequenz anfordern - auch in
  Elzas Variante ROOM1171 - und die Intro-Uebergabe nicht.
- **Zwei Pruefer** haben den Einbau gegengelesen. Ein harter Fehler kam dabei heraus: ohne
  Tonausgabe (kein Audiogeraet) waere das Spiel am Tor stehen geblieben. Behoben - die
  Sequenz laeuft dann stumm durch.
- **Licht, Blende und Takt** sind die aus RE2 (feste Tuerbeleuchtung, subtraktive Blende,
  59,826 Bilder/s). Die Szene ist deshalb eher dunkel - wie die Tuerszenen in RE2.

### Was bewusst offen blieb

- **Nur dieses Tor** hat eine Sequenz. Die RE1.5-Raumdaten sagen fuer keine Tuer, welches
  Modell sie haette; jede weitere Tuer braucht ein eigenes Modell oder ein passendes
  RE2-Archiv und einen Eintrag in der Zuordnung.
- **PSX-Stand:** die Szene laeuft auf PC, Steam Deck und Android; der PSX-Build geht ohne
  Sequenz durch das Tor wie bisher.

## Wo ich mich korrigieren muss

**1. Die Sicherung aus v0.8.15 war schlechter gebaut, als ich gemeldet hatte.** Viereck-
Reihenfolge und Normalen waren falsch, der Sitz lag hinter der Kuppel, und am Lade-Weg fehlte
sie ganz. Gemeldet hatte ich "Suite gruen" — geprueft hatte ich das Bild nicht.

**2. Meine Vorgaben an die Ermittlung enthielten drei falsche Annahmen:** die Memory Card sei
Item 0x20 (es ist 0x21), der Communication Room sei ROOM10D0 (es ist ROOM10F0), und das Blau
der Karte komme aus den Palettenindizes 12/13/14 (widerlegt: 0 Texel in allen Raum-Rechtecken).
Die Agenten haben alle drei an den Daten widerlegt.

## Was bewusst offen blieb

* ⛔ **Der Wurf der Handgranate** (Flug, Abprall, Explosion) fehlt, siehe oben.
* ⛔ **ROOM3071 ist fuer Elza ein Softlock** — gefunden bei der Birkin-Messung, schon vor
  dieser Runde vorhanden. Nach der Freigabe wartet das Skript auf zwei Flags (Ck(5,31) @0x0364D,
  danach Ck(5,30)), die nur Birkins Sturmangriff setzt (@0x80119658 / @0x801197b4). Der Port
  hat diesen Angriff ohne die Flag-Setzer gebaut; Leon bzw. Elza bleibt gesperrt. Leons
  ROOM3070 ist nicht betroffen. Das ist der erste Punkt der naechsten Runde.
* Birkins Griff haelt Sie zu lange fest: ohne Eingabe 142 Bilder, im Original 19 plus die
  Clip-Phasen (Opfer-FSM @0x8011afd8). Schon vorher so, jetzt gemessen.
* In ROOM3080 fehlt Birkins Szenenmodus (grid 4); die Folge endet trotzdem ueber die Tuer.
* Das CHECK-Foto bleibt bei 18 weiteren Gegenstaenden leer, deren Bild im RE2-Format vorliegt
  (u.a. alle Kraeuter-Mischungen 0x24-0x2E). Behoben ist es in dieser Runde nur fuer die
  Sicherung.
* Elzas Lobby-Szene: der Erzaehler tippt sich dort als Schreibmaschine auf, bei Leon steht er
  als Volltext; die Zombies hinter dem Gitter wirken orange. Beides nicht belegt, nicht angefasst.
* Die Bildwiederholung des Originals im Bild eines Kamerawechsels (@0x80021560) ist nicht
  nachgebaut; es bleibt ein Bild Figurbewegung (Port-Wahl).
* Die PSX-Plattform kennt die neuen Toene, das Diary und die Tisch-Gegenstaende nicht.

## Woran ich als naechstes sitze

**Den Granatenwurf** nachbauen und **ROOM3071** (Elzas Birkin-Kampf) spielbar machen, dann Birkins Griff (Opfer-FSM) und danach
Elzas Raumkette weiter nachfahren.

# v0.8.15 - 2026-09-27

Zwei Ihrer Befunde sind erledigt: die Charakterwahl fuehrt endlich in Elzas Zweig, und der
liegende Hund ist untreffbar — letzteres jetzt ueber den Mechanismus, den RE2 wirklich
benutzt. Dabei muss ich drei eigene Aussagen zuruecknehmen. Dazu die Trefferboxen aller
RE2-Gegner, ein dritter Fahrstuhl mit Ton, und ein Raetsel, das ich bewusst NICHT gebaut
habe. Suite 360/360.

## Was Sie merken

**Elza ist ein eigener Zweig — bis heute war sie es nicht.** Sie hatten gemeldet, dass die
Wahl am Anfang in Leons Raeume fuehrt. Die Messung war eindeutig: zwischen einem Durchlauf
mit Leon und einem mit Elza lagen **0 von 691200 Pixeln** Unterschied. Nicht fast gleich —
bitgleich dasselbe Spiel. Die 40 ungeraden RDT je Stage, Elzas Raeume, waren tote Daten.

Jetzt: Wahl Elza laedt `ROOM1241.RDT` und das Modell PL04, und die Kette laeuft weiter nach
`room1031` statt nach Leons `room1170` — der Versatz greift also auch beim Tuerwechsel.
Leon gegen Elza sind es nun 4542033 abweichende Pixel.

Das Original macht es mit **zwei Instruktionen**: es rechnet gar keine Raum-Id aus, sondern
addiert Elzas Bit auf den CD-Dateiindex (`srl a0,a0,31` @0x800397e4, `addu a0,a0,v0`
@0x800397ec). Leons und Elzas RDT liegen als Nachbarn im Verzeichnis.

Den Startraum hat eine Messung entschieden, nicht mein Geschmack: `ROOM1240.RDT` und
`ROOM1241.RDT` unterscheiden sich in **genau einem Byte** — dem Tuerziel bei Datei-Offset
0x0531, `0x17` (HELIPORT) gegen `0x03` (LOBBY). Das sind Zeichen fuer Zeichen die beiden
Raumindizes aus dem Original-Einstieg: `ori 0x17` @0x8001d2a8 und `ori 0x3` @0x8001d324,
die beiden Seiten des `bltz` @0x8001d2a4.

⛔ **Was das NICHT heisst:** Elzas Szenario ist damit nicht durchspielbar. Gefahren ist
genau EIN Uebergang; 119 der 120 ungeraden Raeume sind ungeprueft. Der Zweig steht, der
Inhalt dahinter ist die naechste Arbeit.

**Der liegende Hund ist untreffbar, der stehende nicht.** Schiessen Sie einen Hund um,
liegt er kurz; in dieser Zeit gehen Schuesse durch ihn hindurch. Steht er wieder, trifft
man ihn wieder. Gemessen in Bildern: stehend 1 Treffer, Abschuss-Kette 78 Bilder, waehrend
er liegt **0 Treffer in 77 Bildern**, nach dem Aufstehen wieder 1 Treffer in 60 Bildern.

Das Verhalten gab es schon in der letzten Version. Neu ist, dass es ueber den RICHTIGEN
Mechanismus laeuft (unten mehr) — sichtbar aendert sich dadurch nichts, aber vorher war es
mit dem falschen Werkzeug erreicht.

**Die Trefferpause der Hunde stimmt jetzt auch.** In v0.8.14 hatte ich Ihnen gemeldet, der
doppelte Abzug sei behoben. Das galt fuer den Zombie-Pfad. Der Hund laeuft ueber einen
anderen Weg und war von der Reparatur nie erfasst — bei ihm wurden aus 15 Bildern weiter
effektiv 8. Gemessen mit Pause 40: vorher bis zu 2 Abzuege je Bild und 32 Bilder bis null,
jetzt 1/1/1 und 40 Bilder, wie bei Zombie, Kraehe und Baby-Spinne schon vorher.

**Trefferboxen fuer alle RE2-Gegnertypen.** Bei mehreren Typen waren die Boxen gar nicht
gesetzt.

**Ein dritter Fahrstuhl hat Ton.** ROOM3080/3081 (WAREHOUSE LIFT) faehrt ueber ein anderes
Skript als die beiden bekannten und war deshalb stumm.

## Wo ich mich korrigieren muss

**1. "Das fuenfte Tor ist der Schuss-Pfad" war falsch — es ist der Messer-Pfad.**
Ich hatte die Aufrufstelle @0x80042F94 so beschriftet. Vier Belege sagen etwas anderes:
die Schadenszeile @0x800A412C+0*20 beginnt mit 3 (jede andere mit 11 oder mehr), der
Feuer-Toneffekt @0x800A6F94 ist ein leeres `jr ra`, der Hitcode endet auf 1 @0x80042F64-88,
und der Keil {-200,0,250,125} @0x8001101C ist eine Nahkampf-Form. Die zweite Aufrufstelle
@0x800467C0 gehoert Waffe 12 (Bolzen) — dort ist die "Zielhoehe" die Welt-Hoehe des
fliegenden Bolzens selbst. **Alle Schusswaffen laufen ueber FUN_800410CC, und das liest
die Zielhoehe nie** (Vollscan ueber den ganzen Bereich 0x8004xxxx).

**2. "+0x14D ist nicht aufgeloest" stimmte auch nicht.** Es ist die Bildnummer im Clip
(@0x80029B28-34 / @0x80042D28 / @0x800A6D7A & 0x7f @0x80042DA4-B0).

**3. Damit lief der Schutz des liegenden Hundes bisher ueber das falsche Tor.**
Jetzt laeuft er ueber die HALTUNGSKLASSE `(word0 >> 26) & 7` gegen die Maskentabelle
@0x800A6DB4, gesetzt in FUN_80104088 (liegend 1, stehend 3) — das ist der Weg, den RE2 fuer
Schuesse wirklich nimmt. Der Beweis, dass das Tor umgezogen und nicht bloss abgeschaltet
ist, steckt in einem Zaehlerpaar: **Tor-Urteile beim Schuss 0 fuer jeden Typ, beim Messer
135.** Zwei Gegenproben dazu: nullt man die Trefferbox kuenstlich, darf sich die
Trefferzahl jetzt NICHT mehr aendern (Zombie 28/300, Hund 4/300) — und mit fest
eingestellter Klasse 3 kommen 17 Treffer in 389 Bildern heraus, exakt die alte Zahl.

Kein Gegnertyp ist dauerhaft untreffbar; Zensus ueber 900 Bilder Dauerbeschuss:
Zombie 82, Hund 11, Kraehe 40, Spinne 82, Baby-Spinne 41.

## Das Sicherungs-Raetsel in ROOM1050 — und warum es NICHT gebaut wurde

Sie hatten beobachtet, dass dort ein Raetsel herausgenommen wirkt. **Die Beobachtung
stimmt.** Cut 7 und Cut 8 sind dieselbe Kamera — die ersten 28 Bytes bitgleich, nur der
pri-Versatz unterscheidet sie (0x518 / 0x51C). Cut 7 zeigt einen leeren Sockel mit roter
Leuchte, Cut 8 die eingesetzte Sicherung mit zwei gruenen, 2761 Pixel Unterschied. Und die
Nachricht "I need a fuse to run the shutter." steht in der Datei und wird nie aufgerufen.

Die Sicherung ist aber **kein Gegenstand, sondern ein Flag**:

    ROOM2030 @0x01FF2   22 03 6c 01   Set(3,108,1)   - genommen
    ROOM2060 @0x010A2   21 03 6c 00   Ck(3,108,0)    - der Generator fragt sie ab

Darum hat Item 0x40 "Fuse" null Platzierungen im ganzen Spiel. Und der Zwei-Zustands-Cut
ist keine Ausnahme: in ROOM2060 ist genau diese Bauform voll verdrahtet, drei Paare
(5/11, 6/10, 8/9) mit Cut_replace @0x016C2.

Nichts verbindet ROOM1050 mit dieser Sicherung. Geprueft: AOT-Slots 0..10 lueckenlos
belegt, Flag-Vollscan ueber alle 240 RDT (Bank 3 nutzt 110 Bits, kein einziges verwaist),
Byte-Suche nach einer Ausgabe von Item 0x40 ueber alle RDT und BIN — 0 Treffer, wobei
dieselbe Suche die geparkte Fundstelle in ROOM1051 findet, also funktioniert. Auch RE2
spricht dagegen: dort liegen Main Fuse und Fuse Case beide in ROOM60D0 und werden im selben
Abschnitt gebraucht.

Haette ich den Shutter trotzdem an eine Sicherung gehaengt, waere der Raum **unloesbar**.
Deshalb steht dort jetzt nur eine Herkunftsmarke auf 22 Byte- und Kamerastellen plus ein
live gefahrener Durchspielbarkeits-Nachweis — kein Verhaltens-Code.

## Woran ich als naechstes sitze

**Elzas Raumkette weiterfahren** und zaehlen, wo sie haengt — 119 ungeprueft.
Danach die zwei Cuts ohne Maskenausschnitt (ROOM1000 Cut 3, ROOM11F0 Cut 1) und eine
Schiene fuer die 20 Cuts, die an der Kapazitaetsgrenze von 105 Masken sitzen, ohne dass
etwas sie ueberwacht.

## Was bewusst offen blieb

* Der Klassen-Nuller @0x80103874-A0 — gebaut waere er eine Dauersperre (dieselbe Falle wie
  in Runde 13/14, wo ein Gegner dauerhaft unverwundbar wurde).
* Der Schadensrecord des Hundes @0x800A4424 ist gedumpt, aber nicht verdrahtet: diese Runde
  stellt den Treffer-Mechanismus um, nicht das Schadensmodell.
* Die Bolzen-Aufrufstelle (Waffe 12) hat im Port keinen Produzenten — keine der 22
  RE1.5-Waffen bildet auf diese Id ab.
* Elzas Westen-Variante: Leons R.P.D.-Weste ist PLD-Index 1. Welchen Index Elza dort
  bekaeme, ist NICHT belegt — also steht dort keine erfundene Zahl.
* Vier ROOM1170-Sonderfaelle bleiben auf Leons Raum-Id. Sie gehoeren zu seinem
  Helipad-Vorspann; sie zu oeffnen haette Elza einen Leon-Flag untergeschoben.
* Das PSX-Target hat keine Charakterwahl — dieser Auftrag war der PC-Zweig.
* Der Hund bekommt jetzt den byte-true Zonen-Stempel +0x1D2 statt der bisherigen Naeherung.
  Die Suite bleibt gruen, aber die Wirkung auf die Knockdown-Zweige ist nicht eigens
  gemessen.

# v0.8.14 - 2026-09-27

Ihre fuenf Befunde vom Spieltest, das Kartensystem aus RE2, und zwei Stellen, an denen
ich mich selbst korrigieren muss. Suite 352/352.

## Was Sie sofort merken

**Das Messer.** Sie hatten recht: mit Dauerschlag nach unten kamen die Zombies nie heran.
Die Ursache ist eine Altlast — seit Runde 19 laufen RE2-eigene Zombies durch RE2s
Geometriedaten, und die haben die REICHWEITE mitgenommen. Das Messer reichte gegen sie
3000 bis 3400 Einheiten statt 1500. Das einzige Angriffstor eines Zombies gegen einen
*stehenden* Spieler liegt aber bei 1200 — Sie haben ihn also zweitausend Einheiten davor
dauerhaft in die Trefferreaktion geschlagen. Gemessen: im Dauerschlag vorher **null**
Bisse, jetzt zwei; wer nur zielt, wird unveraendert sechsmal gebissen.
Nebenbei behoben: die Trefferpause wurde zweimal pro Bild heruntergezaehlt, aus 15 wurden
effektiv 8.

**Der Cursor am Schalterraetsel.** Der Ton beim Bewegen ist weg — er kam aus zwei Stellen,
nicht einer. Der Inventar-Cursor toent weiter, das ist eigens abgesichert. Gedrueckte
Taste heisst genau ein Ton, nicht einer je Bild.

**Ihre Schalterwerte liegen drin.** Ein Riegel zaehlt alle 1024 Kombinationen durch: genau
eine erreicht 80, und das ist 1+3+5+7+9. Mit den alten Gewichten waren es **252** — die
Loesung war praktisch beliebig. Huebsch dabei: RE1.5s eigene Loesungspruefung im Raum
verlangt genau dieselben fuenf Schalter wie Ihre Tabelle.

**Der Cursor-Schatten.** Er gehoert dorthin. Er steckt in der Textur, und die echte PSX
zeichnet ihn genauso — das ist jetzt an einem Savestate nachgemessen. Falsch war, dass
unser Kreuz daneben **gar nicht hell** gezeichnet wurde. Details unten.

**Adas Animation** beim Wechsel 1090 -> 1050 wiederholt sich nicht mehr.

**Die Hunde treiben Sie nicht mehr in die Wand.**

**Das Kartensystem aus RE2 ist da.** Im Labor (ROOM5030) haengt ein Plan an der Wand, den
Sie untersuchen und mitnehmen koennen — das ist eine echte Fundstelle aus RE1.5s eigenem
Skript, keine erfundene. Ohne Plan sehen Sie nur den Raum, in dem Sie stehen; mit Plan
erscheint das ganze Labor als schwarze Flaechen mit hellen Wandlinien.

## Wo ich mich korrigieren muss

**Der Cursor-Schatten: mein letzter Fix hat es schlimmer gemacht.** In v0.8.13 hatte ich
die Abtastphase um -0,5 verschoben und das als Verbesserung gemeldet. Diesmal gibt es
eine echte PSX-Grundwahrheit zum Vergleichen, und die sagt:

    Phase -0,5 (mein Fix)   1335 abweichende Pixel
    Phase  0,0 (davor)       785
    Phase +0,375            pixelgleich mit der PSX

Ich habe die Sache also erst verschlechtert und dann behauptet, sie sei besser. Jetzt
steht ein gemessener Wert drin, und ein Riegel prueft ihn gegen die echten Texel.

**Das Grün der besuchten Raeume ist KEINE Erfindung des Ports.** Das hatte ich Ihnen so
berichtet — es stimmt nicht. RE1.5 fuehrt seine Kartenpalette in derselben VRAM-Zeile wie
RE2, und ihr Eintrag ist selbst nachgelesen gruen. Erfunden war nur die Annaeherung, mit
der der Port diesen Wert nachgebildet hat.

Deshalb bleibt es gruen: nach Ihrer eigenen Vorgabe ist RE1.5 massgeblich, wo es ein
System vollstaendig hat, und RE2 nur dort, wo RE1.5 unfertig ist. Die Palette hat RE1.5
vollstaendig — den Kartenbesitz gar nicht. Also Mechanik nach RE2, Farbe nach RE1.5, und
zwar jetzt mit dem exakten Wert. Der aktuelle Raum (dunkelrot) und unbesuchte Raeume
(schwarz) kommen aus RE2, weil RE1.5 fuer beide Zustaende gar keinen Gegenwert hat.
RE2s Blau steht als geprueft Alternative im Code — ein Wort genuegt.

## Was das Kartensystem wirklich kann

RE2 hat **kein Karten-Item**. Karten sind Ereignisse: man untersucht einen Wandplan, und
das Raumskript setzt ein Bit. Deshalb gibt es auch keine Weltmodelle zu extrahieren, wie
Sie es bei den Dokumenten bekommen haben — ein Wandplan ist gemalter Hintergrund plus
Untersuchen-Punkt, kein Gegenstand. Stattdessen liegen die **20 Kartenbilder selbst** in
build/extracted/re2_karten/, je dreimal mit den echten Paletten.

Der Besitz aendert genau zwei Dinge: unbesuchte Raeume werden ueberhaupt erst gezeichnet,
und es erscheinen Gegenstandsmarken. Nicht davon abhaengig sind Grundriss, Spielerpfeil,
Etagenumschaltung und Massstab. Und es sind **nicht** eine Karte je Etage: RE2 hat 20
Bereiche, aber nur 8 Fundstellen, und ein Fund schaltet oft mehrere frei.

## Offen, ehrlich benannt

* **Kein Bildbeleg fuers Messer.** Die Spiel-exe startet in dieser Sitzung zwar, liefert
  aber keinen Abzug mehr — im Protokoll steht "REMOTEDESKTOP-Sitzung". Der Befund ist
  ueber den echten Spielweg gemessen, aber nicht bebildert. Ich bin bewusst nicht auf die
  Ersatzwege ausgewichen, die visuelle Fehler maskieren.
* **Der Laborplan oeffnet drei Kartenblaetter** — das ist meine Setzung. Es sind die drei
  Seiten mit Labor-Raeumen, aber RE1.5 fuehrt dafuer keine Tabelle. Dass ein Fund mehrere
  Bereiche oeffnet, ist immerhin RE2s Muster.
* **Elf der vierzehn Kartenblaetter haben keine Fundstelle** und verhalten sich wie
  bisher. Die drei anderen Wandplaene in RE1.5 sind reine Ansichtssachen ohne Wirkung;
  ihnen ein Bit zu geben waere eine erfundene Fundstelle.
* **Die Hunde-Beobachtung aus Runde 26** ("erst wieder verwundbar, sobald sie stehen")
  bleibt ungeklaert. Der Riegel, den ich dafuer gefunden hatte, trifft den Fall
  nachweislich nicht.
* **Der Fahrstuhl-Ton toent auch im zweiten Fahrstuhl** (A-2). Der Ausloeser kommt aus
  den Daten, die Signatur ist dort bitgleich, und im Original ist er an beiden Stellen
  stumm. Ihre Entscheidung.
