# v0.8.24 - 2026-10-12

Ihr Auftrag der Runde 36 ist gebaut. Jeder Punkt wurde von einem zweiten Agenten unabhaengig am gebauten
Spiel gegen Ihren Wortlaut nachgemessen; wo er nicht bestand, wurde nachgebessert und erneut abgenommen.
Wo etwas nur teilweise ging, steht es unter "Zu entscheiden" bzw. "Grenzen" (Marker ROOM1100, weitere Tueren,
Parkplatz-Tuer in 11B0). @@SUITE@@

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
