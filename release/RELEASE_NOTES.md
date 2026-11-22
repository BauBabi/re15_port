# v0.8.31 - 2026-11-22

Ihre Auftraege fuer diese Fassung: die Story nach Ihrem freigegebenen Konzept (8.11.: "Ja, rein story technisch inst der
Rest fantastisch."; Konzept mit Ihren Korrekturen K-01 bis K-15:
C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md), am 14.11. der Auftrag, damit parallel zu
beginnen, "damit ich danach die letzten Voiceovers aufnehmen kann" (beide Zitate: Memory reai-v2-runde41-laufend.md);
Ihr Wort vom 7.11. zum Finale ohne G5; Ihre Rangfolge vom 16.11. fuer Gore und Schaden; Ihre Entscheide vom 16. bis
18.11. zu Gegenstaenden, ungenutzten Inhalten und unbenutzten Hintergruenden; und Ihre Wuensche vom 18.11.: Lageplaene
wie in Resident Evil 2, die Karte fuer STAGE2 bis 6, Leichenhalle und Gefaengnis wie im Original, der RE2-Film zu
Birkins Mutation, die Sprechernamen und das Ende ohne "THANK YOU FOR PLAYING". Der Wortlaut steht je Abschnitt.

Die Fassung enthaelt zehn Gruppen: (1) Story Teil 1 bis 3 (in v0.8.30 als Vorschau angekuendigt) mit der
Spider-Man-Szene "von aussen" und der Option VOICE ON/OFF, (2) Gegenstaende, (3) Gore und Schaden, (4) ungenutzte
Inhalte und Restpunkte, (5) den RE2-Film "Birkins Mutation", (6) Lageplaene wie in RE2, (7) Leichenhalle und Gefaengnis,
(8) das Finale nach dem RE2-Ende, (9) Sprechernamen und Untertitel, (10) die Karte fuer STAGE2 bis 6. Alle zehn Gruppen
samt den Nachtraegen zum Finale und zu den Untertiteln sind im Sammelzweig r42/story zusammengefuehrt (19.11.). Dazu
kamen am 20. und 21.11.: (11) Ton wie auf der PlayStation (sieben Stufen), (12) der Fabrikplan, den Marvin selbst von
der Wand nimmt, und die Zug-Szene 6031 mit Ada, (13) Ihre Sprachaufnahmen vom 20.11. und (14) die Klaerung von neun
roten Tests vor der Freigabe.

**So wurde gearbeitet.** Jede Gruppe lief auf einem eigenen Zweig und in Stufen: Ein Bauer baut, danach misst ein
zweiter Agent unabhaengig nach - mit eigenem Bau, eigenen Laeufen im echten Spielfenster und einer Gegenprobe (derselbe
Test muss auf dem alten Stand durchfallen). Faellt eine Runde durch, wird nachgebessert und neu geprueft. Die meisten
Runden brauchten das Gefaengnis und die Karten-Tafeln (je acht), der Kern des Finales (fuenf) und der Wurf (vier); die
Zahlen stehen je Abschnitt.

**Nicht in dieser Fassung.** Fuer v0.8.32 vorgesehen: die Laborkette des Konzepts (Strom, ID-Karte, Enzym, Probe,
Impfstoff) mit Alarm, Durchsage, Feuer, Schutzraum-Szenen und Hangar-Tor; das Feuerschott 514 (Ihr "ja, find ich gut"
vom 18.11.); Marvin als Werfer im Finale. Nicht gebaut und ohne Termin: das U-Bahn-Kartenblatt. Ihre Modelle fuer alle
Zwischensequenzen sind am 19.11. angekommen; Leon- und Elza-Intro, der Vorspann vor dem Titel, die Endvideos und der
Abspann im Stil von Resident Evil 1 entstehen fuer v0.8.32 (der Platz fuer das Endvideo ist im Spiel schon da).

**Pruefstand.** Der Ausgangsstand (v0.8.30 mit Story Teil 1 bis 3, Hangar-Szene und VOICE) wurde zweimal geprueft:
zuerst ein roter Punkt (der Naht-Test der Hangar-Szene zaehlte 35 statt 0 Punkte - Ursache war der Grafikadapter der
Fernsitzung, die Szene ist bitgleich), dann "BESTANDEN (kein roter Punkt)", 206 von 206 Tests gruen. Nach den
Zusammenfuehrungen vom 18.11. (Gegenstaende, Gore, Restpunkte, Birkin-Film, Lageplaene) endete die Teilsuite mit 468 von
474 gruen; fuenf Fenster-Tests liefen einzeln wiederholt gruen, einer war echt rot und ist angepasst (er erwartete in
3010 die Schrotpatronen beim ersten Druck, dort liegt jetzt zuerst der Zuender; d7285c8de), ebenso zwei Schubladen-Tests
im Kesselraum, wo jetzt zuerst das Rohr kommt (93821acef). Am 19.11. folgten Leichenhalle und Gefaengnis (Teilsuite 76
von 76 gruen), der Kellerplan und das Finale Stufe 1 bis 5 (im Probe-Baum 155 von 155 gruen). Volle Suite ohne parallele
Laeufe und Abnahme nach dem letzten Code-Commit: siehe "Abnahme des Endstands" unten; Linux-Suite im Container und
Release-Bau: siehe "Technisch".

**Abnahme des Endstands (22.11.).** Nach dem Zusammenfuehren von Ton, Fabrikplan, Ihren Aufnahmen und der Test-Klaerung
lief die volle Testsuite auf dem letzten Stand 4b9d26f34 ohne parallel laufende Agenten: 1774 von 1774 Tests gruen
(17008 s). Der Lauf davor auf faa27a6ce hatte 9 rote Tests (1765 von 1774); keiner davon war ein Spielfehler (siehe "Vor
der Freigabe: neun rote Tests geklaert").

## Was Sie merken

### Story Teil 1 bis 3: Kanalisation, Leiter, Fabrik und Zug (STAGE2, STAGE1, STAGE3, STAGE6)

Ihr Wortlaut vom 8.11. (Korrektur K-01): "3.2 L1 - nein, ich würde sagen, wenn man das Wasser ablässt um zu ROOM 2080 zu
gehen steht dann noch ein Text da wie - Neben dem Computer ist noch ein Schlüsselloch (Auf englisch natürlich). Mit den
Schlüssel muss man hoch gehen zu ROOM 2080 um das Tor zu öffnen. Dabei spielt die Cutscene automatisch ab mit dem Marvin
Dialog.", berichtigt: "Die Reihenfolge stimmt bei dir nicht. man muss in 2080 das Wasser ablassen um überhaupt zum
Aligator nach 2090 zu kommen. Dabei kann der Hinweis auf das Schlüsselloch kommen". Zum Weg mit der Zeitbombe (K-02):
"die finale Tür die ist verschlossen und kann nur von der anderen Seite aus geöffnet werden, also von den Raum aus, der
aus STAGE2 heraus führt." (Memory reai-v2-runde41-laufend.md). Ihre Korrektur K-07 vom 11.11. steht im Konzept: Die
Dialoge in 3000 bleiben, die Begleiter warten in 2080/2081 und holen vor der Leiter auf.

**Kanalisation (2080/2081, 2090/2091).** Beim Ablassen des Wassers erscheint der Hinweis "Next to the computer...
there's a keyhole.". Nach dem Alligator liegt neben dem Water Key noch etwas ("There's something else lying next to
it..."): Sherrys Taschenuhr mit der Gravur "For Sherry. Love, Mom.". Zurueck in 2080 oeffnet der Schluessel das Tor, und
dabei laeuft genau einmal das Nachgespraech. Bei Leon kommt Marvin und bleibt zurueck ("Go on ahead, Leon. I'll wait
here and watch your back."); bei Elza kommt John, dann Sherry die Treppe herauf ("So you're Sherry. Stick close, kid."),
und John wartet mit ihr. Der Weg mit der Zeitbombe umgeht den Alligator nicht mehr: Die Tuer im Korridor 2070/2071 nach
11A0/11A1 ist "locked from the other side" und geht nur von der 11A-Seite auf, die Tuer zum Belohnungsraum 2040/2041 nur
von innen; danach sind beide als Abkuerzung offen.

**Vor der Leiter (11A0/11A1).** Im schmalen Abschnitt vor der Leiter kommen die Begleiter angelaufen: bei Leon Marvin
("Leon, wait up!"), bei Elza John und Sherry ("Elza! Wait for us!"). Hat Elza die Uhr mitgenommen, kniet sie sich hin
und gibt sie Sherry zurueck ("Sherry, wait. I think this is yours."); ohne Uhr entfaellt dieser Teil. Die Szene laeuft
einmal; die Dialoge in 3000/3001 sind unveraendert.

**Fabrik, Leon (3020 bis 3070).** Marvins Abschnitt folgt dem RE2-Muster eines Figurwechsels: Marvin hat eigene
Gegenstaende, und Leon ist nach dem Rueckwechsel nicht geheilt. In der Fensterszene von 3020 geht Marvin allein weiter
("Wait here. I'll find a way to open this from the inside."). Sein Abschnitt endet in 3060, wenn die Hunde ihn angreifen
(Schwarzbild, drei Schuesse); Leon hoert sie in 3020 ("Gunshots...? Marvin!"). Spaeter kaempft Leon in 3060 selbst gegen
die Hunde und findet Marvin am Bein verletzt; Marvin uebergibt die Green Keycard, den Fabrikplan und die Redhawk und
bleibt zurueck. Erst diese Uebergabe oeffnet die Kartentuer in 3010. Im Lagerraum 30E0 laeuft die Szene mit Ada und
Marvin in einem Besuch statt in zweien (das Glas bricht, "That came from the lobby... Marvin!", Ada: "Someone's
coming.") und endet erst nach der letzten Zeile samt Aufnahme. In 3070 kommt, wenn Birkin zusammenbricht, die neue Zeile
"That came from the storage room... Ada?"; Birkin richtet sich bei seinem Auftritt nur noch einmal auf, und im Aufzug
3080 springt der Arm zwischen den zwei Stoessen nicht mehr zurueck. Bei Elza aendert sich in diesen Raeumen nichts.

**Zug (6030/6031).** Leon: Das Gespraech im Wagen hat die neuen Zeilen des Konzepts (John: "We got split up in the fire.
One second she was right behind us..."); der alte Satz, Elza kaempfe draussen und komme nach, ist weg. Elza: Sherry
erklaert, warum Leon nicht mitkommt ("A policeman brought Marvin down here. Then he went back into the fire."), und der
Zug faehrt jetzt auch bei ihr an ("It's moving! Elza, we did it!", dann das Krachen von hinten). Marvin auf der Liege
wird nicht mehr von einem viereckigen Ausschnitt verdeckt. Dazu drei Kleinigkeiten: Ada steht nach ihrem Tod nicht mehr
lebend im Schutzraum 6020, die Blue Master Keycard nennt in der Beschreibung ihren Besitzer Michael, und
Anfuehrungszeichen in Inventartexten werden richtig gezeichnet.

**Wie geprueft.** Je Stufe ein unabhaengiger Pruefer mit eigenen Durchlaeufen im echten Fenster, Leon und Elza:
Kanalisation zwei Runden (mit echtem Alligator-Kampf), Leiter zwei, Marvin drei (beide Faelle der Wegwerf-Frage an der
Red Master Keycard), Lagerraum und Birkin drei, Zug eine, Kleinigkeiten zwei - jeweils zuletzt "bestanden".

**Offen.** Die neuen Zeilen haben noch keine Aufnahmen (siehe "Fuer Sie zum Aufnehmen"). Die Zug-Zeilen sprechen schon
vom Feuer und vom Reaktor, obwohl beides erst mit der Laborkette in v0.8.32 kommt. Der Glasbruch in Elzas Lauf (Frage
F-07 des Konzepts) ist nicht gebaut. In 11A0/11A1 streift der Begleiter in einer Kamera optisch die Wand.

### STAGE5: Spider-Man-Szene "von aussen" - nur aus dem Monitorraum, kein Kampf im Gang (50A0/50A1)

Ihr Wort vom 16.11.: "Bei Szene aussen muss das aber vom Monitor Raum aus erfolgen, wenn man zurueck laeuft, sonst fragt
man sich - wenn er Richtung Kamera zum anderen Ausgang hin geht wo er denn hin ist..." (Memory
reai-v2-hangar-szene-monitorbild.md).

**Was jetzt anders ist.** Die Szene laeuft nur noch, wenn man aus dem Monitorraum 5030/5031 durch die Nordtuer in den
Gang zurueckgeht. Sie ist sofort im Bild, die eigene Figur ist nicht zu sehen; der Inhalt ist Bild fuer Bild die
abgenommene Szene. Danach steht man an der Nordtuer, Birkin ist fort, es gibt keinen Kampf; der Spider-Man liegt tot im
suedlichen Teil der Halle und richtet keinen Schaden an. Wer von Westen (4070) kommt, findet einen leeren Gang, ebenso
beim Wiederkommen und nach dem Laden. Wer Birkin vorher im Kraftraum besiegt hat, bekommt keine Szene. Der Kampf im Gang
und das "bleibt liegen" aus v0.8.30 entfallen damit.

**Wie geprueft.** Eine Runde: "Bestanden, kein roter Punkt." Die Videos hat der Pruefer selbst im echten Fenster
aufgenommen (die zwei Affen des Gangs sind fuer die Aufnahme ausgeschaltet).
* Leon: F:/Resident Evil
  1.5-Project/Entscheidungen/reAi_v2_entscheidung_2026-11-06/39_hangar_szene_von_aussen/1_leon.mp4
* Elza: F:/Resident Evil
  1.5-Project/Entscheidungen/reAi_v2_entscheidung_2026-11-06/39_hangar_szene_von_aussen/2_elza.mp4

### Optionen: VOICE ON/OFF

Ihr Wunsch vom 17.11.: eine Option, die Voiceovers an- und auszuschalten (ein woertliches Zitat steht in den Protokollen
nicht).

**Was jetzt anders ist.** Im Optionsschirm gibt es unten rechts die Zeile "VOICE ON", ab Werk an; die Wahl wird sofort
gespeichert. Bei OFF sind alle gesprochenen Dialogzeilen stumm; die Untertitel bleiben, und jede Szene laeuft mit der
Lesedauer ihres Textes - nichts haengt, keine Zeile faellt weg. Musik, Geraeusche und Filmton bleiben. Laute ohne
Untertitel bleiben ebenfalls hoerbar (Johns Hilferufe, Annettes Schrei in 5090, Elzas Stoehnen), weil der Spieler dort
sonst keinen Hinweis mehr bekaeme - ein Entscheid waehrend der Runde. Bei ON ist alles wie vorher.

**Wie geprueft.** Runde 1 unabhaengig: "Bestanden, ohne roten Punkt." Der Pruefer hat jede Sprachdatei im Tonmitschnitt
gesucht (bei ON alle gefunden, bei OFF keine), der Ton bei ON ist bitgleich mit dem alten Stand, 28 von 28 Tests gruen.
Die Regel fuer Laute ohne Untertitel kam danach dazu; sie hat einen Test mit Gegenprobe, aber keinen eigenen
Pruefbericht. Offen: Auf dem Stand der Untertitel-Stufe 5 verschwinden bei OFF einzelne zweite Seiten (siehe
"Untertitel").
* Menue und Vorspann mit VOICE OFF: F:/Resident Evil
  1.5-Project/Entscheidungen/reAi_v2_entscheidung_2026-11-06/47_option_voice/1b_menue_off.png und F:/Resident Evil
  1.5-Project/Entscheidungen/reAi_v2_entscheidung_2026-11-06/47_option_voice/2_szene_off.mp4

### Ton wie auf der PlayStation (Strang ton, sieben Stufen)

Ihr Befund vom 19.11.: Der Ton klang im Port "dumpfer" und anders als auf der PlayStation. Der Grund waren fehlende
Teile der Sony-Tonbibliothek. Jede Stufe wurde am Original-Code und an Aufnahmen aus DuckStation gemessen, und ein
zweiter Agent hat jede Stufe unabhaengig nachgemessen.
* **Lautstaerke je Stimme wie im Original:** Der Port rechnet die Lautstaerke jetzt wie die Tonbibliothek der
  PlayStation (Produkt und Quadrat, SpuVmKeyOnNow @0x80055d10). Geraeusche liegen jetzt auf etwa 1 dB am Original,
  vorher bis zu 11 dB zu leise. Musik ist dadurch 8 bis 10 dB lauter als in v0.8.30, Ihre Sprachaufnahmen sind
  unveraendert.
* **Schleifen:** Geraeusche, die im Original in Schleife laufen (z. B. der Rolladen im Speisesaal), laufen jetzt auch im
  Port so lange wie dort.
* **Hall je Musikstimme:** Der Hall liegt jetzt auf den Stimmen, die ihn im Original haben.
* **Ausklang:** Der Rotor am Hubschrauberplatz klingt so schnell aus wie im Original.
* **Glaettung:** Der Port glaettet die Toene wie der Tonchip der PlayStation (4-Punkt-Gauss). Dadurch klingt es weniger
  hart.
* **Lage der Geraeusche:** Links, rechts und Entfernung werden wie im Original gerechnet, auch bei sehr grossen
  Entfernungen (die Wurzel-Funktion des Spiels rechnet dort anders, das ist jetzt nachgebaut).
* **Huellkurven, Regler, Musikstuecke:** Die Geraeusche schwellen an und klingen ab wie im Original, der
  Lautstaerkeregler rundet nicht mehr. Musikstuecke ohne Wiederholmarke spielen einmal und beginnen wieder von vorn,
  wenn das Spiel sie erneut startet. In der Eingangshalle wird das kurze Zusatzstueck nicht mehr alle 18 Sekunden neu
  gestartet.
* Hoerbeispiele (erst Original, Piepton, dann Port): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ton/hoerbeispiele/

### Fabrik: Marvin nimmt den Fabrikplan selbst (3060, 30C0, 30D0)

Ihr Befund vom 20.11.: "Im Story Zweig - room 3060 übergibt Marvin ein plan of the plant. Das macht natürlich nur sinn
wenn er ihn auch wirklich irgendwo aufnimmt..."
* Der Fabrikplan haengt jetzt auch in Leons Eingangshalle 30C0, an derselben Wand wie bei Elza in 30C1. Marvin kommt
  durchs Fenster genau dort an und kann ihn als Spielfigur von der Wand nehmen.
* Nur dann uebergibt er ihn beim Wiedersehen in 3060 ("And this. A floor plan of the plant."). Ohne Plan fallen genau
  diese zwei Zeilen weg, alle anderen Saetze bleiben, Ihre Aufnahmen passen also weiter.
* Hat Marvin den Plan nicht genommen, haengt er weiter, und Leon kann ihn selbst holen - auch in der Halle 30D0 nach der
  Szene in 30E0. Der Plan ist freiwillig wie jeder Plan in RE2: Wer weder Marvin noch Leon den Plan nehmen laesst,
  bekommt ihn nicht.

### Zug 6031 (Elza): Ada bringt Marvin zum Zug

Ihre Frage vom 20.11.: "Auf wen soll Leon denn suchen?" und "sie ist ja tot bei Leon, warum sollte er noch mal nach ihr
suchen?" - Ihre Wahl: "Ada bringt Marvin".
* Sherry sagt jetzt "A woman brought Marvin down here. Then she went back into the fire." und "She said she had to find
  someone. She told us not to wait for her." Mit "someone" ist Leon gemeint. Das passt zu Leons Fassung, in der Ada
  Marvin zum Zug bringt (50B0: "I left Marvin with her.").

### Vor der Freigabe: neun rote Tests geklaert (Strang riegel)

Ihr Wunsch vom 21.11.: "Kannst du tests die rot sind stabilisieren, oder tests die sowies nicht mehr gelten gleich ganz
entfernen?" Keiner der neun roten Tests zeigte einen Spielfehler, geloescht wurde keiner.
* Vier prueften einen bewusst geaenderten Stand und sind umgestellt: Gore nach Ihrer Rangfolge vom 16.11., Untertitel
  mit hoechstens zwei Zeilen je Seite, eine Wurzelrechnung wie im Original.
* Drei wackelten nur, wenn nebenher Blender die Grafikkarte belegte. Sie warten jetzt auf den richtigen Zustand und
  pruefen dasselbe wie vorher.
* Zwei hingen an der Umgebung (eine Datei ausserhalb des Projekts, das Programm git) und sind davon jetzt unabhaengig.

### Bekannt, nicht behoben

* Wenn Windows der Grafikkarte kurz den Zugriff entzieht (gesehen nur unter starker Last, etwa wenn nebenher Blender
  rendert), kann das Spiel abstuerzen. Sie haben das beim Spielen noch nie erlebt; die Behebung ist fuer die naechste
  Runde eingeplant (Liste 29_naechste_punkte, Punkt 14).

### Gegenstaende: jeder bekommt Fundort und Verwendung (Story Teil 4)

Ihr Wortlaut vom 16.11.: "Aktuell haben wir auch noch quest Items, wie zum Beispiel eine Mutter in den Menu, die wir
nicht verwenden. Wir sollten überlegen wo alle items die wir haben Verwendung finden, wenn noch nicht drin." und zu den
Handgranaten: "man findet halt die Handgranate in Irons office. Wenn man noch die Munitionsteile findet, kann man noch
eine bauen, aber Handgranaten sollte es nicht zu viele geben, wenn sie so stark sind." (strang.js, Memory
reai-v2-gegenstaende-verwendung.md). Zu den Rechner-Vollbildern am 17.11.: "Ja, einbauen".

**Was wo liegt (Leon und Elza, wo nichts anderes steht).**
* **Mutter:** im Pausenraum 2010/2011 beim Erste-Hilfe-Spray. Am Bruecken-Pult in 2030/2031 wird sie ueber das Inventar
  eingesetzt, danach laeuft das Brueckenraetsel wie bisher (das "Ventil 13" aus dem Plan gibt es seit Ihrem
  Druck-Raetsel nicht mehr). Sie heilt nicht mehr - im Original-Code zaehlte sie als Heilmittel.
* **Colt Python:** im Belohnungsraum 2040/2041 an der Stelle des Head of Akuma, 6 Schuss.
* **Glock 18 (Leon):** beim toten Polizisten in 1050, 15 Schuss, mit dem bisher nie gezeigten Nahbild der Leiche
  ("Sorry, I'll just borrow this..."). Sie bleibt beim Nachladen eine Glock (im Original-Code wurde sie zur SIG).
* **Rohr:** am Schrank im Kesselraum 11F0/11F1.
* **Granatwerfer (Elza):** in einem Spind in 1071 ("One of the lockers isn't locked. There's a grenade launcher
  inside."), 6 Schuss.
* **Granaten-Bausatz der 40%-Disc:** leere Granathuelsen (Elza: Waffenkammer 1191 und 30E1), Kapseln (Nitro: Leon 30E0,
  Elza 30E1; Saeure: Elza 40B1 und Spind 4051; Brand: Elza 5021) und genau ein Zuender (im Laster 3010/3011: "Something
  is lying next to the crates. It's a detonator."). Huelsen und Kapsel ergeben Granatwerfer-Munition, Zuender und Kapsel
  eine Handgranate - nach Capcoms eigener Kombi-Tabelle. Elza kann so bis zu 6 Schuss und eine Granate bauen, Leon die
  eine Granate.
* **Flammenwerfer-Brennstoff (Leon):** im ersten Spind in 4050.
* **Handgranaten:** je Figur hoechstens zwei im ganzen Spiel - die fertige in Irons' Buero und eine aus dem einzigen
  Zuender.

Die neuen Gegenstaende haben kein eigenes Modell in der Welt. Sie liegen deshalb an Stellen, an denen schon etwas
Sichtbares ist: Beim Druecken kommt zuerst der neue Gegenstand, danach wie bisher der alte. Der Raketenwerfer bleibt
Ausstellungsstueck in 1110; im Finale bringt John ihn mit.

**Bilder, Texte, Rechner.** Die Kapseln haben ein eigenes Bild statt Capcoms Platzhalter-Karte, der Zuender heisst
"Detonator", die Python hat ihre eigene Beschreibung. Auf der Blue Master Keycard steht "MICHAEL" statt "GORO MACIDA",
passend zum Text (geaendert sind nur die 221 Bildpunkte des Namens; die Schrift ist so winzig wie vorher). Die Rechner
zeigen beim Benutzen ihr Vollbild: in 3050/3051 zu "The system has been successfully restored.", in 5050/5051 zu
"Commencing with temporary registration." und zu "Registration complete.", in 2080/2081 nach dem "Yes" fuer etwa 0,7
Sekunden.

**Wie geprueft.** Drei Pruefungen (Stufe 1, Stufe 2, Nacharbeit der gelben Punkte), alle "bestanden". Der Pruefer hat
jede Fundstelle an den Bytes der Raumdateien nachgelesen und die Handgranaten selbst nachgezaehlt (alle 240 Raumdateien
und der Quelltext); zuletzt 694 von 695 verwandten Einzeltests im ersten Lauf gruen (der eine nur, weil im Prueflauf git
im Pfad fehlte) und 31 von 31 Laeufe mit dem echten Spiel.
* Leon nimmt die Glock und baut die Handgranate: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gegenstaende_nach/video/ERGEBNIS_leon_glock_granate/ERGEBNIS_leon_glock_granate.mp4
* Elza nimmt den Werfer am Spind und baut Saeure-Munition: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gegenstaende_nach/video/ERGEBNIS_elza_werfer_saeure/ERGEBNIS_elza_werfer_saeure.mp4
* Kartenbild Michael: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gegenstaende/s2/bilder/ERGEBNIS_karte_michael.png
* Rechner 2080 mit dem kurzen Vollbild: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gegenstaende/s2/bilder/ERGEBNIS_2080_rechner.mp4

**Offen.** Die Videos starten per Sprung in den Raum; die Bausatzteile kamen dort per Hilfsschalter, und in Elzas Video
sind die Zombies ausgeschaltet. Im echten 1071 liegt gut einen Meter vor dem Spind ein Zombie, der auf dem Weg dorthin
aufsteht. Dass der gruene Zielkreis des Fingerabdruck-Spiels in 505 nicht mehr auf dem Vollbild liegt, hat kein Pruefer
im echten Fenster gesehen (nur per Test mit Gegenprobe belegt). Das Bild in 2080 steht nur 0,7 Sekunden, weil dort keine
Original-Meldung steht, an die es sich binden liesse (siehe "Zu entscheiden"). Auf der PSX gibt es die Vollbilder ohne
Kamera und die Gegenstandsbilder nicht.

### Im ganzen Spiel: Gore und Schaden nach Ihrer Rangfolge (80 % vor 40 % vor RE2)

Ihr Wortlaut vom 16.11.: "Gore und Schaden - ganz klar - wenn ich früher was anderes behauptet habe, falsch - 80% build
ist ausschlaggebend, dann 40% build und wenn äquivalente Gore Effekte bei 40% build und wenn dort Dinge noch fehlen dann
kommt Resident Evil 2 zum Einsatz." Ihre Ausnahme: "Allerdings sollen Zombies nicht - wie in der Mod Version alle 2
Schuss umfallen, dann wieder aufstehen dann wieder umfallen und das 100x bis si wirklich tot sind. ... In dem Fall dann
doch eher Resident Evil 2". Zur Handgranate: "wenn weniger reicht um die zombies zu töten, gib der handgranate weniger.
Nur genau so viel wie benötigt." (strang.js).

**Zombies.** Der Granatwerfer zerteilt keinen Zombie mehr, er trennt den Oberkoerper ab. Steht der Zombie, bleiben die
Hosenbeine knapp eine Sekunde stehen, kippen um und bleiben liegen; der Oberkoerper faellt und kriecht weiter. Liegt er
oder kriecht er noch mit Beinen, bleiben die Beine liegen, wie sie lagen. Die Handgranate zerteilt jeden Zombie in ihrem
Wirkbereich; ihr Schaden war schon "genau so viel wie benoetigt" (ein Punkt ueber dem zaehesten Zombie mit 250
Lebenspunkten) und ist unveraendert. Reisst die Schrotflinte aus der Naehe den Oberkoerper ab, bleiben die Beine an
ihrem Platz (bisher rutschten sie gut zwei Meter weg). Abgetrennte Beine drehen sich nicht mehr mit dem kriechenden
Oberkoerper mit und klappen nicht mehr in einem Bild herum - ein Fehler, der erst beim Filmen auffiel. Zaehigkeit und
Umfallen der Zombies bleiben nach Ihrer Ausnahme wie in RE2.

**Tiere und Bosse.** Die Kraehe zerfaellt bei Pistole, Schrotflinte und SPAS in schwarze Fetzen, die nach knapp einer
Sekunde weg sind. Platzt die Spinne, fliegt der Koerper in Brocken, die Beine bleiben gespreizt am Boden, und der
Kadaver liegt still - sie dreht sich nicht mehr auf den Ruecken. Der Hund wird von der Handgranate zerrissen, beim
Granatwerfer stirbt er ganz (beides wie RE2). Birkin in Menschform hat 300 statt 600 Lebenspunkte (21 statt 41
Pistolenschuesse), der Gorilla 180 statt 360 (16 statt 31): Das sind die Werte der Disc; die doppelten Werte hat Ihre
Rangfolge aufgehoben. Die Riesenform nimmt ausserhalb des Finales den Schaden des RE2-G4 (praktisch auf dem Bahnsteig
5080). Ein Feuerstoss von Beretta oder Glock zaehlt gegen Gorilla, kleinen Affen, Tyrant und Birkin in Menschform als
ein Treffer wie auf der Disc; gegen Zombies, Hunde, Kraehen und Spinnen trifft weiter jede Kugel. Im Gefaengnis gehen
Schuesse und Messer wieder durch die Gitterarme, Explosionen treffen sie weiter. Der Alligator ist unveraendert, er
stirbt nur durch den Kanister.

**Wie geprueft.** Zombies und Granaten drei Runden: Die ersten beiden fielen durch, weil der Granatwerfer zunaechst nur
stehende Zombies trennte und die Videos die Beine nicht zeigten; die dritte "Bestanden", 218 von 218 Tests gruen,
darunter ein Test, der das echte Spiel startet und mitliest, wo die Beine gezeichnet werden. Tiere und Bosse zwei
Runden: Die erste fiel an der Spinne durch (acht Beine standen senkrecht hoch und zappelten), die zweite "bestanden",
404 Einzeltests und 26 Laeufe mit dem echten Spiel gruen (zwei davon erst in der Einzelwiederholung). Zu jedem Video
liegt im selben Ordner eine Zeitlupe.
* Granatwerfer: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gore_bau/videos_r3/granatwerfer_ganz.mp4
* Handgranate: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gore_bau/videos_r3/handgranate_ganz.mp4
* Spinne: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/gore_bau/s2r2/videos/spinne_platzt_ganz.mp4

**Offen.** Im Granatwerfer-Video sitzt der Oberkoerper noch etwa 0,6 Sekunden auf den stehenden Beinen und faellt dann
herunter; im 80%-Video fliegt er als Blutsaeule weg - die Abfolge Ihres Wortlauts stimmt, das Bild weicht ab. Die
Handgranate im Video ist per Testschalter am Zombie gezuendet, nicht geworfen. Die neue Blutlache unter einem Zombie,
der eine Granate am Boden ueberlebt, kommt fast nie vor, weil ein stehender Zombie nach der RE2-Regel dabei nicht
umfaellt. Von der geplatzten Spinne bleiben nur die Beine als Boegen, kein Rumpf (so die Disc; im 80%-Video nicht zu
entscheiden), und beim Magnum-Treffer schluepfen weiter kleine Spinnen (RE2-Regel). Platzt eine Spinne an Wand oder
Decke, bleibt der alte Sturz auf den Ruecken; gesehen hat den Fall niemand. Das Kraehen-Video stufte der Pruefer als
nicht vorlegbar ein, ein neues fehlt; ebenso fehlen Bilder vom Hund an der Granate, von der Riesenform und von den
Gitterarmen.

### Ungenutzte Inhalte, nie gezeigte Kameras und Restpunkte (STAGE1 bis 6)

Ihr Wortlaut vom 17.11.: "Gibt es noch anderen ungenutzten Code, Dialoge oder rätsel in stage 2-6 die mit rein können in
den story strang?" (Memory reai-v2-ungenutztes-stage2-6.md). Ihre Antworten vom 18.11.: zur Tuer 401 fuer Elza "ja,
warum nicht?", zu Leons Zug "Ja, na klar, wenn du die ruckeln lassen kannst während der Zug fährt lass sie auch
ruckeln.", zu Zombies im Gang 11A "nein, da ist kein Zombie sichtbar", zur Szene in 3091 "Zu deiner Frage - passt, aber
warum zielen sie denn jetzt auf die falsche Tür?" und zu den Masken "Die PRIs in Stage 4 habe ich dich gefragt, ob du
die großteils von Resident Evil 2 übernehmen kannst." (strang.js, INTEGRATION_NOTIZEN.md).

**Nie gezeigte Kamerabilder.** Funkraum 10F0/10F1: Wer die Funkanlage untersucht, sieht sie in Nahaufnahme zum Satz
"It's out of commission". Verhoerraum 1010/1011: Beim Nehmen der Memory Card zeigt das Spiel die Ecke mit dem Regal. Zug
6031: Elzas Gespraech beginnt wie bei Leon mit dem Gruppenbild. Hof 1090 (Leon): Wer den Feuerloescher benutzt, sieht
zuerst kurz den Transporter im Feuer. Alles mit Original-Befehlen; die Raumdateien sind unveraendert.

**Ungenutzte Capcom-Inhalte.**
* **3091:** Nach den vier Schlaegen sagen John ("Hey, let's get going. If we stay here, they'll come swarming back.")
  und Elza ("Hey... you'll be all right, won't you?") zwei Saetze von der Capcom-Disc, John antwortet "Leave it to me!".
  Blicke, Zielhaltung und Pochen gehen jetzt auf die Lobby-Tuer, durch die Elza danach geht.
* **309:** Sherry sagt am Sicherungskasten zuerst "There's a power supply." (Satz aus dem 80%-Video; nach Ihrem
  Entscheid ein Systemtext ohne Namen), dann wie bisher "I can't reach the breaker switch...".
* **501:** Vor dem Speichern am Rechner kommt Capcoms Satz "It looks like this was used a little while ago.".
* **6031:** Elza schaut vor dem Gespraech erst zu Marvin auf der Liege - so hat Capcom es gebaut, aber nie benutzt.
* **402:** Der Wandplan gibt das Kartenblatt des Labors ("You've taken the laboratory map.").
* **2011:** Sherry steht nicht mehr doppelt; sie verschwindet aus dem Pausenraum, sobald sie in 2081 auftaucht.
* **401 (Elza):** Die Tuer aus der Halle 4001 in den Sicherheitsraum oeffnet wie auf der Capcom-Disc (die Mod hatte sie
  mit einem einzigen Byte totgestellt). Elza findet dort die Kiste, den Speicherpunkt, ein Erste-Hilfe-Spray und
  Schrotpatronen; keine Gegner, keine Szene.

**Zug, Masken, Kleineres.** Elzas und Leons Fahrt (6031, 6030) ruckeln, solange der Zug faehrt: Das Bild springt nach
dem RE2-Muster kurz um einen Bildpunkt und zurueck; sonst ist Leons Szene Byte fuer Byte unveraendert. In vier Kameras,
deren Hintergrund in RE2 derselbe ist, verdeckt der Vordergrund die Figur jetzt richtig (4090 Kamera 2, 50D0 Kamera 10,
5100 Kamera 7, 6000 Kamera 1; Leon und Elza, 203 Masken aus RE2 umgerechnet). Mehr gibt RE2 nicht her: In STAGE4 kam nur
eine Kamera dazu, Ihr "großteils" ist dort also nicht erfuellt; die 38 Kameras, die Sie selbst freistellen muessten,
stehen in der Bestandsaufnahme vom 18.11. Der lange Aushang in 2040 bricht nicht mehr nach 192 Zeichen mitten im Satz
ab, und in 3070 steht Birkin zwischen Auftritt und Kampf nur noch 3 statt 12 Bilder still.

**Wie geprueft.** Sieben Stufen, jede unabhaengig geprueft und zuletzt "bestanden". Zwei brauchten eine zweite Runde:
Bei den Kamerabildern hatte der Bauer das Feuerbild im Hof zu Unrecht als widerlegt gemeldet, bei den Capcom-Inhalten
war ein alter Zug-Test rot, und ein Video zeigte nicht, was behauptet war. Die Masken hat der Pruefer selbst aus RE2
gelesen (0 Abweichungen) und im laufenden Spiel nachgemessen.
* 3091, Pochen und Blicke: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/rest/s7/vid/S7_3091_Pochen_Lobbytuer_mit_Ton.mp4
* Tuer 401 (Elza): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/rest/s5/video/Elza_401_Tuer_Kiste_Telefon.mp4
* Leons Zug ruckelt: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/rest/s4/Leon_6030_Zugstart_ruckeln.mp4
* Feuerbild im Hof: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/rest/s2r2/vid/A3_1090_Leon_Cut14_Loeschen.mp4
* Masken vorher/nachher, Beispiel 4090: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/rest/pri/vorher_nachher_4090.png

**Offen.** Nicht gebaut sind das U-Bahn-Kartenblatt mit den Wagenkacheln, Leons Blick im Zugwagen (in der Mod stehen die
Figuren anders als bei Capcom), der Gitter-Schreck in 400 fuer Leon (braucht eine zweite Tonbank), fuenf Laute aus den
Zugraeumen (kein eindeutiges Zuggeraeusch), die Glasfront in 30C und der leere Schrank in 30E. Das Feuerbild im Hof
steht nur etwa eine Drittelsekunde (siehe "Zu entscheiden"). Die neuen Kamerabilder haben keine Verdeckungsmasken. Den
Ort des Pochens in 3091 hoert man nicht, weil der PC-Port Geraeusche ohne Richtung abspielt; ob man die leisen Schlaege
als Klopfen erkennt, muss ein Mensch hoeren. John sagt dort kurz hintereinander "Will do!" und "Leave it to me!" (Ihr
"passt" vom 18.11.). Das Ruckeln ist mit blossem Auge kaum zu sehen. Auf der Capcom-Disc liegt in 401 ein dritter Fund,
den die Mod entfernt hat; er ist nicht eingebaut.

### STAGE5/6: RE2-Film "Birkins Mutation" (Leon in 6000, Elza in 5011)

Ihr Wortlaut vom 18.11.: "Zusätzlich möchte ich das du die Birkin Mutation Erklärungscutscene aus Resident Evil 2
irgendwo sinnvoll mit einbaust." (strang.js). Den Ort haben Sie freigestellt.

**Was jetzt anders ist.** Leon sieht den Film bei seinem ersten Treffen mit Annette in 6000, nach "Where's Ada?" und "I
see...". Annette stellt sich mit Saetzen aus RE2 vor, gesprochen von der RE2-Stimme ("Annette Birkin. My husband is the
man responsible for the creation of the T-virus. William Birkin." - "And it's all Umbrella's fault. ..."). Dann wird das
Bild in gut 2 Sekunden dunkel, der Film laeuft (rund 131 Sekunden, englische Originalstimmen, keine Untertitel, mit dem
Rand oben wie in RE2), das Bild wird wieder hell, und Annette schliesst mit "As a result of his virus-induced
transmutation, William should have lost any prior memories he had as a human by now."; danach geht die Szene weiter wie
bisher. Elza sieht den Film im Monitorraum 5011 zwischen "John. I read some files ... terrifying things." und "She's
been infected by what they were working on here.", ohne neue Saetze. Der Film kommt einmal je Spielstand;
herausgeschnitten ist nur der 12-Sekunden-Einschub mit Annette in der Gegenwart.

**Wie geprueft.** Leon zwei Runden (die erste fiel nur am Video durch, das zu langsam lief), dann "bestanden"; Elza:
"Bestanden." Der Pruefer hat den Filmton aus der Datei ausgepackt und im Video wiedergefunden (hoechstens 2 ms
Abweichung ueber den ganzen Film).
* Leon: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/birkinfilm/Leon_6000_Birkinfilm_s1r2.mp4
* Elza: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/birkinfilm/Elza_5011_Birkinfilm_s2r1.mp4

**Offen.** Zwischen Abdunkeln und Film bleibt das Bild etwa 0,4 Sekunden schwarz (Ladezeit). Elzas Video und Test
starten per Sprung in den Raum; ihre Ankunft ueber 5021 und 5000 ist nicht durchgespielt. In Leons Video stehen Annettes
zweite Satzhaelften noch ohne Namen (siehe "Untertitel"). Auf der PSX gibt es keinen Filmspieler.

### Lageplaene wie in Resident Evil 2: ein Plan je Gebiet

Ihr Wortlaut vom 18.11.: "Ich habe für STAGE1 entschieden das jede Etage ihre eigene Map zu finden hat. Jetzt bin ich
mir nicht mehr sicher wie das im Original Resident Evil 2 war. War das auch eine Map pro Etage, oder hat man mit einer
Map beispielsweise das ganze RPD zusammen gefunden?" - und nach der Antwort (RE2 hat eine "police station map", eine
eigene Keller-Karte und je Gebiet eine): "Dann will ich das nicht so. Ich will das es mehr funktioniert wie in Resident
Evil 2.", "Überlege dir auch gleich mit für die anderen STAGES wo die Map noch fehlt, und wo wir sie am besten sichtbar
platzieren sollten." und "nicht benötigte maps kommen weg." Zur ersten neuen Stelle des Revierplans: "Bei 1030 und 1031
passt das nicht. Das der Revierplan so weit über den Spieler hängt, das geht so nicht." (strang.js, Memory
reai-v2-karten-wie-re2.md).

**Was jetzt anders ist.**
* **Revier:** Ein einziger Revierplan in der Eingangshalle 1030/1031 schaltet 1F, 2F, 3F und das Dach frei (Text wie in
  RE2: "police station map"). Er haengt links neben der Ausgangstuer auf Kopfhoehe der Figuren, so hoch wie die
  WANTED-Plakate.
* **Keller:** Ein einziger Kellerplan in der Leichenhalle 1200/1201 schaltet B1 und B2 frei ("police basement map"). Er
  ist doppelt so gross wie bisher und haengt auf Kopfhoehe an der Wand neben der Eingangstuer; an Ihrer frueheren Marke
  neben der Vitrine ist dafuer kein Platz (siehe "Zu entscheiden").
* **Weg sind** die Etagenplaene im Besprechungsraum 1140, im Flur 10D0 und im Keller 11D0; die Waende sehen dort wieder
  aus wie im Original. Alte Spielstaende gelten weiter.
* **Kanalisation:** Plan an der Wand links neben der Leiter am Ostende von 2000/2001 ("There's a sewer map. Will you
  take it?"). **Fabrik:** Elza findet ihn in der Eingangshalle 30C1 rechts der Treppe, Leon bekommt ihn von Marvin.
  **U-Bahn:** in 6000/6001 gleich nach der Ankunft, an der Wand oben an der Treppe. **Forschungszentrum:** der Wandplan
  in 402 (siehe "Ungenutzte Inhalte").

Zu den Orten in Kanalisation und U-Bahn sagten Sie am 18.11. "ja" (INTEGRATION_NOTIZEN.md). Nach dem Nehmen zeigt die
Karte das ganze Gebiet, auch die noch nicht betretenen Raeume.

**Wie geprueft.** Fuenf Pruefungen (Revier und Keller; Gebietsplaene; Sichtbarkeit und Dach; Augenhoehe; Kellerplan),
jede in der ersten Runde "bestanden". Die Texte stimmen Byte fuer Byte mit RE2 ueberein; 143 verwandte Tests gruen bei
den Gebietsplaenen, 127 von 127 beim Kellerplan. Beim Bauen gefunden und behoben: Leon kletterte beim Druecken auf das
Plakat, statt die Frage zu bekommen.
* Revierplan mit Leon und Elza daneben: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/lageplaene_nach2/tafeln/MESSBILD_Leon_Elza_am_Revierplan.png
* Revierplan, Video Leon: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/lageplaene_nach2/video/leon_revier/leon_revier.mp4
* Kellerplan mit Leon und Elza daneben: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/kellerplan/messbild_leon_elza.png
* Kellerplan, Video Leon: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/kellerplan/video_leon_1200.mp4

**Offen.** Die Plaene in Kanalisation, Fabrik und U-Bahn haengen etwa auf Brusthoehe, tiefer als der Revierplan. Im
Ankunftsbild der Leichenhalle ist der Kellerplan 35 x 26 Bildpunkte gross, richtig gross erst in der Kamera direkt
davor. Am U-Bahn-Plan steht bei Leons erstem Betreten Annette dicht davor (Szene der Disc). Die Videos starten ueber
einen Spielstand vor dem Plan, nicht aus einem Durchlauf.

### STAGE1: Leichenhalle 1200/1201 - das Kuehlfach klappert wieder, der Zombie kriecht heraus

Ihr Wortlaut vom 18.11.: "Wo du gerade in der Vorschau in dem Raum bist. Im Original unmodifzierten 40% build klappert
dort noch die eine Tür in der Leichenhalle und der Zombie kriecht dann raus. Das ist in der Mod Version aus irgendeinen
Grund nicht mehr so.... Das soll wieder rein!" (strang.js, Memory reai-v2-leichenhalle-zellenarme.md).

**Was jetzt anders ist.** Tritt Leon oder Elza vor die Kuehlfaecher, wackelt die eine Fachtuer hin und her und schwingt
auf; der Zombie im Fach wacht auf, faellt heraus, kriecht und greift an - Befehl fuer Befehl der Ablauf der echten
40%-Disc. Gefehlt hat er, weil die Mod den Ausloeser am Boden und das Klappern weggeschnitten und das Oeffnen an das
Aufheben des Minidisc-Players gehaengt hatte. Der Player, die Musik und der Hinterhalt der Mod bleiben (drei Zombies
liegen still und stehen auf, wenn man den Player nimmt): Wer das Klappern ausloest, den Player liegen laesst und
wiederkommt, findet die drei weiter liegend und den Kriecher am Boden. Wer zuerst den Player nimmt, bekommt das Oeffnen
der Mod, und der Disc-Ablauf startet nicht noch einmal. Einen eigenen Ton hat das Klappern auf der Disc nicht (im
Raumskript steht kein Tonbefehl); zu hoeren ist der Zombie.

**Wie geprueft.** Drei Runden: Die erste fiel am Wiederbetreten durch (der Hinterhalt der Mod war weg), die zweite an
der Reihenfolge "Player zuerst" (die Tuer klapperte ein zweites Mal und drehte sich fast einmal ganz herum), die dritte
"bestanden", 126 von 126 verwandten Tests gruen.
* Leon, Klappern und Kriecher: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/original/leon_1200_r3_mit_ton.mp4
* Elza: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/original/elza_1201_r3_mit_ton.mp4
* Wiederbetreten: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/original/leon_1200_wiederbetreten_mit_ton.mp4

**Offen.** Mit der Standard-KI (RE2) beisst der Kriecher einmal zu und bleibt dann liegen. Dass das Klappern stumm ist,
ist am Skript der Disc belegt, nicht am laufenden Original angehoert. Die Videos beginnen mit einem Sprung in den Raum.

### STAGE1: Gefaengnis 1210/1211 - RE1.5-Zombies hinter den Gittern, Griff wie in RE2

Ihr Wortlaut vom 18.11.: "Und unten in den Gefängnis in STAGE1 hast du die Arme aus Resident Evil 2 übernommen. Aber du
solltest EIGENTLICH NUR die funktionalität aus Resident Evil 2 übernehmen, nicht das Modell. Das Modell darf schon noch
das aus Resident Evil 1.5 sein...." (strang.js, Memory reai-v2-leichenhalle-zellenarme.md).

**Was jetzt anders ist.** Hinter den Zellengittern stehen wieder die Zombies aus RE1.5 statt der Arme aus RE2. Sie
kommen ans Gitter und wiegen sich, und ihre Arme wachsen durch das Fenster in den Flur - genau wie im Original, das die
Arme selbst streckt (laut Code bis zum 2,04-Fachen). Aus RE2 kommt nur die Funktion: Kommt die Figur der gezeichneten
Hand nahe genug, packt der Arm zu und haelt sie fest. Danach sinkt der Arm, wartet unten und kommt langsam wieder hoch,
wie es im Original-Code steht. Ein Griff kostet keine Lebenspunkte, und jeder Arm greift nur einmal. Schuesse und Messer
gehen durch die Arme, Explosionen treffen sie (siehe Gore).

**Wie geprueft.** Acht Runden. Die frueheren scheiterten unter anderem an ueberstreckten Armen (bis zum 2,38-Fachen), an
einer falschen Vorgabe des Orchestrators ("keine Streckung" - dabei laesst das Original die Arme selbst wachsen), an
einem Standbild von 1,3 Sekunden beim Zugriff und an einem Sprung nach dem Absinken. Runde 8: "BESTANDEN", 75 von 75
verwandten Tests gruen; der Pruefer ist dreimal selbst an der Wand entlang gelaufen (zweimal Leon, einmal Elza), und
jedes Mal griff der Arm.
* Leon: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/original/s2r8/video/leon_mit_ton.mp4
* Elza: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/original/s2r8/video/elza_mit_ton.mp4

**Offen (bitte am Video ansehen, siehe "Zu entscheiden").** Nach seinem Griff greift ein Arm nicht noch einmal. Der
Zombie wiegt sich aber weiter, sein Arm waechst wieder und schiebt die Figur dabei in etwa 8 Sekunden um rund 300
Einheiten zur Seite - das tun weder RE1.5 noch RE2. Beim Griff wird die Figur in einem einzigen Bild an die Hand
gesetzt, ein Sprung bis etwa 500 Einheiten; RE2 macht es genauso, man sieht es aber. Wer nur vor dem Fenster stehen
bleibt, wird nicht immer gegriffen: In 20 Laeufen griff der Arm zehnmal, sonst griff er sichtbar daneben, weil der
RE1.5-Arm weniger weit reicht als der RE2-Arm.

### STAGE5/6: Finale wie in Resident Evil 2 - Raketenwerfer statt G5 (Zug 6030/6031, Wagen 5090/5091)

Ihr Wortlaut vom 7.11.: "Für Die Version 31 möchte ich dann, das du die Form Birkin G5 aus dem Finale wieder entfernst.
Die passt nicht... Stattdessen, machen wir ein finale wie beim Golem am Ende von Resident Evil 2. Das jemand Leon den
Raketenwerfer zuwirft, und man dann damit episch von mehreren Wiederholungen sieht, wie die Rakete angeflogen kommen und
den Gegner zersprengt." Vom 17.11.: "Beim erschießen des endgegners mit dem raketenwerfer möchte ich das du das game
over voiceover von Leon übernimmst und das voiceover von Claire.", zum Werfer "Aber ein Spruch wäre schon gut beim
zuwerfen, so etwas wie: Leon, send him to hell, oder Leon, let's finish this oder sowas." und zum Zugfilm "Ja, die
Zugfahrt ist große Klasse!". Vom 18.11. zum Wurf: "Die Wurf Animation ist aktuell noch Schrott. - zumindest in der
Vorschau. Orientiere dich da mehr an Resident Evil 2.", "Außerdem würde ich John eher am Eingang stehen lassen, als das
er kurz da ist und dann mitten im Gang plötzlich verschwindet." und "John kann danach gerne wieder raus laufen. Sollte
er auch, aber das geht schneller vom Eingang aus."; zum Ende: "Passt nicht - das thank you for Playing fliegt
vollständig raus, und es kommt ein richtiges Endvideo. Der Rest klingt erst einmal gut." (strang.js, Memory
reai-v2-story-ende-annette-g5.md und reai-v2-zugfilm-600.md). Zum ersten Video des Abschusses sagten Sie am 18.11.: "Na,
ist doch erst mal super! Endcutscene kommt dann später noch." (INTEGRATION_NOTIZEN.md).

**Der Ablauf jetzt.**
* **Abfahrt.** Nach dem Fahrpult laeuft im Fuehrerstand Ihr Zugfilm (405 Bilder), bei Leon und bei Elza; danach wird aus
  Schwarz eingeblendet, und erst dann kommt "There! The train's moving." bzw. "It's moving! Elza, we did it!". Unter dem
  Film schweigt die Raummusik, der "Zug an"-Ton kommt nur noch einmal. Kurz nach der Abfahrt kracht es hinten, und im
  Fuehrerstand blinkt dreimal das rote WARNING.
* **Kampf.** Im hinteren Wagen stirbt die Riesenform an keiner normalen Waffe mehr (die Regel des RE2-Finales: 20200
  Lebenspunkte, nie unter 10000). Den G5 gibt es im Finale nicht mehr.
* **Wurf.** Nach der RE2-Regel (genug Schaden oder 60 Sekunden; die Form geht und ist weit genug entfernt) erscheint
  John am Eingang von Wagen C, ruft "Leon, catch! Send him to hell!" bzw. "Elza, catch! Let's finish this!", wirft den
  Raketenwerfer im Stehen und geht durch die Tuer hinter sich hinaus. Der Werfer landet neben der Figur; aufgehoben
  liegt er mit 2 Raketen im Inventar und wird dort ausgeruestet.
* **Abschuss.** Wer mit dem Werfer auf die Form zielt und feuert, loest den Schluss aus: Schnitt auf die Figur, etwa
  eine halbe Sekunde spaeter die Schlusszeile "Leon: Game over." bzw. "Elza: You lose, big guy!" (Claires Stimme aus
  RE2), dann drei Wiederholungen der anfliegenden Rakete aus drei Abstaenden, Zeitlupe, Explosion und Todesschrei. In
  den ersten beiden Wiederholungen friert die Rakete wie in RE2 je gut eine Sekunde ein. Nach der Explosion steht das
  Bild etwa 4 Sekunden still: Das Spiel wartet wie RE2, bis der Schrei ausgeklungen ist - es haengt nicht.
* **Ende.** "THANK YOU FOR PLAYING !!!" ist weg. Das Bild wird in gut 2 Sekunden schwarz; dann ist Platz fuer Ihr
  Endvideo (MOVIE/ENDE5090.STR fuer Leon, MOVIE/ENDE5091.STR fuer Elza - solange die Datei fehlt, geht es direkt
  weiter), danach folgen Abspann und Titelbild. Nach dem Kampf sitzt man nicht mehr fest.

**Loch in Wagen B: gebaut und wieder herausgenommen.** Nach Ihrem "Loch ist einfach da" vom 17.11. zeigte Wagen B nach
der Abfahrt zuerst das Bild mit dem aufgerissenen Boden. Gemessen: Das Loch reicht fast von Wand zu Wand (daneben
bleiben 58 bzw. 83 cm, die Figur braucht 90 cm), Wagen B ist der einzige Weg zum Finale in Wagen C, und eine Sperre
laesst sich mit den Original-Befehlen des Raums nicht anlegen. Ihr Wort vom 18.11.: "Aber natürlich soll man nicht
einfach über das Loch laufen können. Wenn das Loch so groß war, das man da nicht drüber laufen kann, war dort
offensichtlich was anderes geplant...." und "Ja, man muss irgendwie vor und zurück kommen. Zurück evtl. auch später über
die Cutscene ... Aber irgendwo muss das Loch her kommen. Finden wir keinen Weg, muss es halt raus..." (Memory
reai-v2-story-ende-annette-g5.md). Deshalb ist das Loch wieder draussen: Wagen B zeigt den heilen Boden, das Krachen mit
dem WARNING bleibt.

**Wie geprueft.** Fuenf Stufen, jede zuletzt "bestanden": der Kern mit Leon in Runde 5 (317 von 317 Einzeltests, 36 von
36 Laeufen mit dem echten Spiel), die Abfahrt in Runde 2 (230 von 230 und 15 von 15), der Wurf in Runde 4, Elza und die
Durchlaeufe in Runde 3 (223 von 223 im ersten Lauf), das Ende in Runde 1 (87 Einzeltests und 12 Laeufe gruen). Der
Pruefer hat die RE2-Werte an den Original-Bytes nachgelesen und beide Schlusszeilen im fertigen Video wiedergefunden.
Ein Test probiert jede freie Stelle von Wagen C durch (43.376 Faelle je Raum): Die Landestelle des Werfers ist immer im
Bild. Die frueheren Runden scheiterten unter anderem an einem sichtbaren Sprung der Figur beim Abdruecken, am Wurfbild,
an einer Form, die zu Szenenbeginn verschwand, und an einem Werfer, der bei Elza im Flug nicht im Bild war.
* Abfahrt mit Zugfilm und WARNING (Leon): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/s2r2/video_leon/zug_s2r2_leon_mit_ton.mp4
* Durchlauf vom Fuehrerstand bis zum Titel, Leon (vor der letzten Stufe aufgenommen, am Ende noch mit "THANK YOU FOR
  PLAYING !!!"): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/video/finale_s4r3_leon_durchlauf_mit_ton.mp4
* Durchlauf Elza (ebenso): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/video/finale_s4r3_elza_durchlauf_mit_ton.mp4
* Ende ab dem Abdruecken, so wie es ohne Ihr Endvideo ausgeliefert wuerde (Bildabzug des Spiels, keine Fensteraufnahme): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/s5r1/video/ende_s5r1_leon_ohne_mit_ton.mp4
* Ende mit einem Platzhalter-Film an der Stelle des Endvideos (Bildabzug): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/s5r1/video/ende_s5r1_leon_platzhalter_mit_ton.mp4

**Offen.**
* Johns Zuruf hat noch keine Aufnahme, er steht nur als Untertitel da (siehe "Fuer Sie zum Aufnehmen").
* Das Wurfbild ist ein einhaendiger Wurf wie bei einer Handgranate. Waehrend der Wurfszene steht die Form still; bei
  Leon sieht man das etwa 1,3 Sekunden lang (RE2 haelt den Gegner dort ebenfalls an, zeigt ihn aber in keiner Kamera).
  Beides ist Ihr Urteil am Video (siehe "Zu entscheiden").
* Bei Elza verdeckt die Form in Johns Kamera die linke Bildhaelfte; John und der Werfer sind klein rechts im Bild. Fuer
  ihr Video bekam Elza Waffe und Munition per Hilfsschalter: Ohne Waffe toetet die Form sie, bevor die 60 Sekunden um
  sind. Beide Durchlaeufe starten per Sprung kurz vor dem Fahrpult.
* Die erste und die dritte Wiederholung teilen sich eine Kamera, die dritte ist klein, die zweite dunkel (Ihr "erst mal
  super").
* "Musik anhalten" ist im Port ein Stummschalten: Nach dem Zugfilm setzt die Musik etwa 13,5 Sekunden spaeter im Stueck
  wieder ein. Claires Todesschrei bei Elzas Explosion hat kein Pruefer angehoert. Ob das Bild in RE2 nach der Explosion
  ebenfalls stillsteht, ist nicht gemessen.

### Finale, Nachtrag: Annette steht beim Ausholen am Boden, faellt mit dem Hieb und bleibt liegen (Leon, 5090)

Ihr Wortlaut vom 18.11.: "Wenn Anette am Ende von den 4 beinigen Birkin nieder geschlagen wird darf sie in der Animation
nicht noch mal aufstehen. Sondern mit den Schlag direkt liegen bleiben." Und am 19.11. zum Video dieser Stufe: "Beim
zurückweichen fliegt Anette irgendwie komisch in der Luft." (strang.js, Memory reai-v2-story-ende-annette-g5.md).

**Was jetzt anders ist.** Solange die Riesenform ausholt, steht Annette ruhig am Boden (etwa 1,4 Sekunden); mit dem Hieb
geht sie zu Boden und bleibt bis nach dem Abschuss liegen - kein Aufstehen, keine Zwischenhaltung, kein Sprung an eine
andere Stelle. Sie liegt jetzt dort, wo sie getroffen wird, leicht gedreht, damit sie weder in einem Bodenfeuer noch in
der Wand liegt; die fruehere Liegestelle ist damit ersetzt. Den Sturz selbst verdeckt die Blutwolke wie in RE2.

**Die Ursachen.** Zum Aufstehen: Annettes Bewegung wurde doppelt weitergeschaltet (einmal von der Szene, einmal vom
allgemeinen Weiterschalter), dadurch lief die Bewegung "Hinfallen und Aufstehen" bis zum Ende durch, und beim
Kameraschnitt sprang sie gut einen Meter an die alte Liegestelle. Zum Schweben: Die Bewegung, die bisher als
"Zurueckweichen" lief, ist im Original die Flugphase eines Sprungs nach unten; den Fall dazu rechnet dort der Code. Ohne
ihn hing sie 10 bis 15 cm ueber dem Boden, ein Bein hochgezogen, und fror so ein, bis der Hieb kam. RE2 laesst Annette
an dieser Stelle ihre normale Steh-Bewegung spielen (derselbe Befehl laeuft jetzt im Port); dort ist sie in diesem
Moment nicht im Bild, bei uns sieht man sie von hinten.

**Wie geprueft.** Zwei Stufen, jede in der ersten Runde "bestanden". Die Pruefer haben die Videos Bild fuer Bild
angesehen und die Hoehe von Becken, Kopf und Fuessen je Bild nachgemessen: beim Ausholen steht der tiefste Knochen auf
Standhoehe, nach dem Treffer steigt nichts wieder an. Die Tests schlagen auf dem alten Stand an. Zum Video der zweiten
Stufe sagten Sie am 19.11.: "passt".
* Annette vom Satz vor dem Hieb bis nach dem Abschuss: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/s8r1/rec/video/annette_s8r1_leon_mit_ton.mp4

**Offen.** In den fuenf Uebergangsbildern nach dem Treffer taucht ein Fuss kurz unter den Boden; das liegt unter der
Blutwolke und ist im Video nicht zu sehen.

### Abspann, Nachtrag: Tafel "Thank you for playing this reconstruction" ist raus

Ihr Wort vom 18.11.: "das thank you for Playing fliegt vollständig raus, und es kommt ein richtiges Endvideo." Das
betrifft nach dem Entscheid des Orchestrators auch die erste Tafel der Abspann-Diashow. Sie sagte "Thank you for playing
this reconstruction of Resident Evil 1.5 ..." und behauptete ausserdem, Alligatoren und spaetere Birkin-Formen fehlten -
das stimmt im Port nicht mehr.

**Was jetzt anders ist.** Diese eine Tafel kommt bei Leon und Elza nicht mehr; alle uebrigen sieben Tafeln stehen in
derselben Reihenfolge und gleich lange, danach folgt das Titelbild. Der Abspann ist dadurch 25 Sekunden kuerzer. Die
Bilddatei und die Raumdateien sind unveraendert, es fehlt nur der eine Aufruf.

**Wie geprueft.** Eine Runde: "bestanden". Der Pruefer hat die Original-Befehle der Diashow an den Bytes nachgelesen;
der neue Test schlaegt auf dem alten Stand an.
* Ende und Abspann bis zum Titel, Leon: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/s7r1/video/abspann_s7r1_leon_mit_ton.mp4
* Elza: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/finale/s7r1/video/abspann_s7r1_elza_mit_ton.mp4

**Offen.** Die letzte Tafel sagt "Thank you everyone! (MartinBiohazard)" mit dem Capcom-Hinweis darunter. Das ist der
Dank des Mod-Autors an seine Helfer; sie bleibt (siehe "Zu entscheiden").

### Untertitel: jeder gesprochene Satz mit Sprechernamen, Lesezeit, hoechstens zwei Zeilen

Ihr Wortlaut vom 18.11.: "Du hast bei den übernommenen Sätzen aus Resident Evil 2 nicht den Namen in den Dialog mit
davor geschrieben wie bei den rest auch....", auf Nachfrage "Ich meinte bei Anette. kann aber auch sein das es nur so
aussah." und zu Sherrys Satz am Sicherungskasten "Nein - das theres a power supply ist ohne Name ein Systemtext"
(strang.js, Memory reai-v2-sprechername-vor-satz.md). Dieser Abschnitt liegt auf dem Zweig r42/namen und war beim
Schreiben noch nicht im Sammelzweig.

**Was gefunden wurde.** Eine Zaehlung aller Untertitel ergab: Annette traegt auf jeder ersten Seite ihren Namen. Im
neuen Birkin-Film waren aber je zwei RE2-Saetze zu einer Nachricht mit Seitenwechsel zusammengelegt; die zweite Seite
erschien ohne Namen, waehrend Annette weitersprach - das sah aus wie ein Satz ohne Namen. Dazu kamen fuenf Zeilen der
Mod (in 10D0, 1150 und 2000), bei denen ein Satz mittendrin auf zwei Untertitel verteilt war und nur der erste den Namen
trug. 472 gesprochene Untertitel trugen den Namen schon; der Erzaehler im Vorspann bleibt wie in RE2 ohne.

**Was jetzt anders ist (Stufen 1, 2 und 4).** Im Birkin-Film beginnt jede Seite, die Annette spricht, mit "Annette:";
ebenso beginnt Leons zweiter Satz im Zugwagen 6030 mit "Leon:". Die fuenf Mod-Zeilen (Marvin und Leon in 10D0, Irons in
1150, Leon und Ada in 2000) sind am Satzende der jeweiligen Aufnahme geteilt: Jeder Untertitel zeigt genau den Satz, den
man hoert, und beginnt mit dem Namen; der Wortlaut ist gleich, nur steht an der Trennstelle ein Punkt statt eines
Kommas. Leons zweiter Satz in 6030 steht jetzt gut 3 Sekunden wie die Original-Zeilen des Raums (vorher 20 Bilder).
Annettes lange Saetze haben je zwei Seiten zu hoechstens zwei Zeilen, keine Zeile steht mehr auf dem schwarzen Balken
unten. Die Regel stammt von der echten Disc: Dort hat keine von 700 Seiten drei Zeilen.

**Lesbarkeit (Stufe 3, nichts geaendert).** In 5090 ist der orange Name "Annette:" vor dem Feuer schlecht zu lesen. Die
Untersuchung ergab: Das Original zeichnet Untertitel ohne Schatten und ohne Kasten, nur mit einem dunklen Rand von einem
Bildpunkt, und der Port zeichnet genau so (im Bild von 5090 stimmen 1525 von 1525 Schriftpunkten). Deshalb bleibt es wie
im Original - ein Entscheid des Orchestrators; diese Stufe hat keinen Commit und keinen eigenen Pruefbericht.

**Wie geprueft.** Stufe 1 zwei Runden (die erste fiel nur am fehlenden Video durch), Stufe 2 und Stufe 4 je eine, alle
"bestanden". Der Pruefer der Stufe 2 hat die zehn Sprachdateien mit einem zweiten Spracherkenner abgehoert; in Stufe 4
waren 153 von 153 Einzeltests gruen und die Tests auf den alten Staenden rot.
* Annette im Birkin-Film: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/namen/video/s4_6000/video.mp4
* Marvin und Leon in 10D0: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/namen/video/s4_10d0/video.mp4
* Leon und Ada in 2000: C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/namen/video/s4_2000/video.mp4

**Hoechstens zwei Zeilen je Seite im ganzen Spiel (Stufe 5).** Die Disc-Regel gilt jetzt fuer alle Untertitel, auch fuer
alte Mod-Szenen, bei denen die dritte Zeile auf dem schwarzen Balken stand (Entscheid des Orchestrators). Eine Zaehlung
ueber alle Raeume fand zuerst 44 Seiten mit drei Zeilen (35 Raumtexte der Mod, 9 Port-Texte), die zweite Runde vier
weitere Port-Texte; alle sind auf hoechstens zwei Zeilen bzw. zwei Seiten verteilt, der Wortlaut ist gleich, und
gesprochene Zeilen tragen auf jeder Seite den Namen. Der Seitenwechsel liegt bei gesprochenen Zeilen an einer gemessenen
Sprechpause der Aufnahme. Der Vorspann des Erzaehlers (1240/1241) zeigt seine Texte jetzt Seite fuer Seite. Systemtexte
bleiben ohne Namen; drei von ihnen brauchen einen Tastendruck mehr (die Warnung in 2030, die Fussspuren in 5060, das
Pult ohne Mutter). Geprueft in zwei Runden: Die erste fiel durch (der Vorspann zeigte verklebte Woerter, vier Texte
fehlten in der Zaehlung, und bei VOICE OFF verschwanden einzelne zweite Seiten), die zweite "bestanden", 336 von 336
Einzeltests und 33 von 33 Laeufe mit dem echten Spiel gruen. Offen: Bei VOICE OFF wartet der naechste Satz, bis ein
geteilter Satz ganz ausgelaufen ist; solche Stellen dauern dann bis zu 5 Sekunden laenger als mit Stimme.

**Veraltete Aufnahmen bleiben stumm (Stufen 6 und 7).** 15 Saetze haben seit der Story v0.8.31 einen neuen Untertitel,
ihre Datei in synchro/ war aber noch die alte Aufnahme - in 6030 sogar mit anderem Sprecher (Sherrys alte Stimme unter
Leons "Where is she now?", Johns alte unter Sherrys "Elza's still out there...!"). Das Spiel erkennt diese Dateien jetzt
an ihrem Inhalt (Fingerabdruck aus dem Stand v0.8.30) und behandelt sie wie eine fehlende Aufnahme: Der Untertitel steht
mit seiner Lesezeit, ohne Stimme. Sobald Sie eine Datei durch Ihre neue Aufnahme ersetzen, spielt sie von selbst. Zwei
der 15 sprechen den neuen Satz praktisch woertlich und spielen deshalb weiter: Marvins "Leon, give me a hand" in 3020
(wortgleich) und Johns Vorstellung in 6030 ("We made it down here" unter dem Text "We came down here"). Stumm sind damit
13 Saetze in 3020, 30E0, 6030 und 6031 (Entscheid des Orchestrators, siehe "Zu entscheiden"). Geprueft: Stufe 6 und 7 je
eine Runde "bestanden". Die Pruefer haben alle 15 Dateien mit mehreren Spracherkennern abgehoert, im echten Spiel mit
Tonmitschnitt gemessen (die 13 sind nicht zu hoeren, alle anderen Stimmen spielen, eine ersetzte Datei spielt) und den
Fingerabdruck aus dem Stand v0.8.30 nachgerechnet; zwei neue Durchlauf-Tests pruefen den unveraenderten Bestand in 6030
und 3020.
* Leon 6030, Gespraech im Wagen (Johns Vorstellung spricht, die veralteten Saetze sind stumm): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/namen/video/s7_6030_d/s7_6030_wagen.mp4

### Karte: STAGE2 bis 6 so sauber wie STAGE1 - jeder Raum mit Kartenstueck und Positionspunkt

Ihr Wortlaut vom 18.11.: "Parallel kannst du dich auch mit der Map auseinander setzen, das die für STAGE2-6 genauso
sauber funktioniert wir bei STAGE1. Das ist aktuell noch nicht der Fall. Inklusive dem - wo befindet sich Leon -
Marker." Und auf die Frage, ob zwei Raeume ohne Original-Kartenstueck leer bleiben sollen: "selbstverständlich sollen
alle Räume auch eIn stück karte mit navigationspunkt haben." (strang.js, Memory reai-v2-karten-wie-re2.md). Der
Abschnitt liegt auf dem Zweig r42/karte; er war beim Schreiben fertig geprueft, aber noch nicht im Sammelzweig.

**Was jetzt anders ist.** In STAGE2 bis STAGE6 liegt jeder Raum auf seinem eigenen gemalten Feld, und der Positionspunkt
laeuft mit der Figur mit. Er laeuft in keinem Raum mehr seitenverkehrt (gemessen zum Beispiel in 30A0, 3050 und 20A0),
die Treppen- und Tuerzeichen sitzen auf den gemalten Symbolen, und der T-foermige Gang 5070 (spaeter 5130) ist wieder
als Raum zu sehen. Jeder Raum, in dem die Karte aufgeht, zeigt ein Stueck Karte mit Punkt: 200 von 200, fuer Leon und
Elza. Wo das Original nichts hat (zum Beispiel 10B0, 1260, 20B0, das Labyrinth 4050 und der Gang in 6000), zeichnet der
Port einen Grundriss aus der Kollision im Stil der Karte - eine Port-Ergaenzung nach Ihrem Wort, wie schon in STAGE1. Im
schmalen Schraeggang von 4080/4081, der auf der Karte genau so breit ist wie der Punkt, steht der Punkt jetzt im Gang
(nie auf einer Wandlinie, nie ueber dem blauen Grund) und wechselt in der Gangmitte auf seine zweite Lage.

**Wie geprueft.** Drei Stufen: STAGE2 und 3 in zwei Runden, STAGE4 bis 6 in zwei, der Durchlauf aller Bereiche mit den
Bildtafeln in acht. Durchgefallen sind fruehere Runden unter anderem an gespiegelten Achsen in vier Raeumen, an
Restfehlern im Schraeggang und an einer Korrektur, die den Punkt in 48 Raeumen springen liess (zurueckgenommen). Runde
8: "Bestanden, mit einer Bedingung fuer den Orchestrator", 66 von 66 Kartentests und 22 von 22 Laeufe mit dem echten
Spiel gruen; der Kartenlauf deckt 645 Lagen ab. Die Bedingung betraf den Schraeggang: An einigen Stellen lag in v0.8.30
mehr Rot um den Punkt - dort stand der alte Punkt aber jedes Mal auf einer Wandlinie und 6 bis 10 Bildpunkte neben der
Figur. Der Orchestrator wertet das nicht als Verschlechterung und hat den Stand angenommen, wie vom Pruefer empfohlen.
* 52 Bildtafeln, je Kartenblatt fuer Leon und Elza: Ordner
  C:/workspace/git/reAi_v2/.claude/worktrees/r42_karte/analysis/karte_r42/s3r7/tafeln
* Schraeggang, v0.8.30 gegen jetzt (Leon):
  C:/workspace/git/reAi_v2/.claude/worktrees/r42_karte/analysis/karte_r42/s3r8/gang_4080_leon.png
* Vier Aufnahmen aus dem echten Fenster:
  C:/workspace/git/reAi_v2/.claude/worktrees/r42_karte/analysis/karte_r42/s3r8/fenster_echt.png

**Offen.** Im Zug 5090/5091 liegen drei Lagen ausserhalb der gemalten Flaeche, und wer im Zug war, sieht danach im
T-Gang Striche, die zum Zug gehoeren - beides verschwindet erst mit dem U-Bahn-Kartenblatt, das nicht gebaut ist. 20A0
erscheint auf seinem Blatt zweimal rot. Die neuen Kartenstuecke haben keine Tuermarken. Im Schraeggang eilt der Punkt
der Figur bis etwa 5 Bildpunkte voraus. Fuer einen Teil des Gangs 30E0 nimmt der Port bewusst nicht die Umrechnung des
Originals, weil sich dort zwei Original-Werte widersprechen (Pruefung der ersten Stufe).

## Zu entscheiden

So ist es jetzt gebaut; ein Wort von Ihnen genuegt, um etwas zu aendern. Die Bilder und Videos stehen im jeweiligen
Abschnitt.

* **Ton: RE2-Geraeusche lauter.** Tueren, Hunde, Kraehen und Spinnen aus den RE2-Tonbaenken sind jetzt 8 bis 12 dB
  lauter als in v0.8.30, so wie der RE2-Code sie rechnet. Ein Hundelaut ist damit etwa 3 dB lauter als ein
  Pistolenschuss. Passt das so? Hoerbeispiele (erst vorher, Piepton, dann neu): C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ton/hoerbeispiele/S1_10_RE2_Gegnerlaute_Schuss_Hund_Hund_Kraehe_Kraehe_Spinne_Spinne_Schuss__erst_PORT_VORHER_Piepton_dann_PORT_NEU.wav
  und C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/ton/hoerbeispiele/S1_11_RE2_Tuer_auf_Tuer_zu_Verschlossen_Verschlossen_Fahrstuhl_Kartenhinweis__erst_PORT_VORHER_Piepton_dann_PORT_NEU.wav
* **Ton: Eingangshalle.** Die Musik stoesst dort kurz vor dem Schlussakkord noch etwas oefter an die
  Lautstaerke-Obergrenze als im Original (1 bis 2 dB hoehere Spitzen). Die Ursache ist noch offen und kommt in die
  naechste Runde.
* **Kellerplan in der Leichenhalle: neue Stelle.** Der Plan haengt doppelt so gross an der Wand neben der Eingangstuer,
  nicht mehr an Ihrer Marke neben der Vitrine: Dort sind nur etwa 1000 Einheiten frei, der grosse Plan ist 1490 breit.
  So lassen heisst: gross und beim Hereinkommen sofort zu sehen. Aendern heisst: zurueck an Ihre Marke, dann nur in der
  alten, kleinen Groesse.
* **Revierplan: Hoehe.** Er haengt jetzt auf Kopfhoehe links neben der Ausgangstuer der Eingangshalle. Passt das so? Und
  sollen die Plaene in Kanalisation, Fabrik und U-Bahn, die etwa auf Brusthoehe haengen, ebenfalls hoeher?
* **Gefaengnis: der Arm nach dem Griff.** Nach seinem einen Griff schiebt der weiterwachsende Arm die Figur zur Seite;
  dazu kommen der sichtbare Sprung an die Hand und die Fehlgriffe, wenn man nur vor dem Fenster steht. So lassen heisst:
  RE1.5-Aussehen mit dem RE2-Griff und diesen drei Eigenheiten. Aendern heisst: Sie sagen, was davon stoert, dann wird
  genau das in einer eigenen Runde angegangen.
* **Finale: Johns Wurf.** Der Wurf ist jetzt ein einhaendiger Wurf im Stehen vom Eingang aus, und die Form steht
  waehrend der Szene sichtbar still. Zweimal haben Sie die Wurfbewegung zurueckgewiesen; die jetzige hat der Pruefer
  durchgelassen, das Urteil aber ausdruecklich Ihnen ueberlassen. Bitte am Durchlauf-Video ansehen.
* **Veraltete Aufnahmen stumm.** 13 Saetze, deren Aufnahme noch den alten Text spricht, laufen bis zu Ihrer neuen
  Aufnahme ohne Stimme (Entscheid vom 19.11.; am 16.11. war eine aehnliche Stummschaltung zurueckgenommen worden, weil
  Sie nicht gefragt waren). So lassen heisst: kein falscher Satz mehr unter einem Untertitel, dafuer an diesen Stellen
  Stille. Aendern heisst: Die alten Stimmen sind wieder zu hoeren - der Rueckbau ist klein. Johns Vorstellung in 6030
  spielt weiter, obwohl sie "made it" statt "came" sagt.
* **Zwei sehr kurze Bilder.** Das Rechner-Vollbild in 2080 steht etwa 0,7 Sekunden, das Feuerbild im Hof 1090 etwa eine
  Drittelsekunde. Beide Zeiten kommen aus dem Skript des Raums an dieser Stelle; laenger waere eine eigene Zahl des
  Ports. So lassen, oder sollen sie laenger stehen - und wie lange?

Waehrend der Runde entschieden (Orchestrator; Sie koennen widersprechen):
* **Finale:** Das Loch in Wagen B ist raus (nach Ihrem "muss es halt raus"). Die Tafel "Thank you for playing this
  reconstruction" ist aus dem Abspann raus; die letzte Tafel "Thank you everyone! (MartinBiohazard)" bleibt.
* **Gore:** Die Handgranate bleibt bei 251 Schaden; damit faellt die Riesenform ausserhalb des Finales nach 4 Wuerfen
  statt der angekuendigten 12 (es gibt aber nur zwei Granaten im Spiel). Ein Zombie, der eine Granate ueberlebt, faellt
  nicht um (RE2), deshalb ist die neue Blutlache selten. Die abgetrennten Beine stehen knapp eine Sekunde, im 80%-Video
  etwa eine halbe allein - sagen Sie, wenn es kuerzer sein soll.
* **Gegenstaende:** Die Mutter gehoert ans Bruecken-Pult. Leon bekommt nur die Nitro-Kapsel, Saeure und Brand nur Elza.
  Der Werfer liegt im Spind und der Zuender im Laster, weil an den geplanten Stellen Zombies liegen, die aufstehen.
* **Lageplaene und Birkin-Film:** der Name "police basement map", der Fabrikplan fuer Elza an der Wand in 30C1, das Dach
  beim Revierplan; beim Film die Orte (Leon 6000, Elza 5011), der herausgeschnittene Einschub und keine Untertitel.
* **VOICE OFF und Gefaengnis:** Laute ohne Untertitel bleiben hoerbar. Die Arme im Gefaengnis wachsen wie im Original,
  und Schuesse gehen durch sie hindurch, weil Ihre Rangfolge die anschiessbaren Gitterhaende aufhebt.
* **Untertitel und Karte:** Die Schrift bleibt wie im Original (kein Schatten, kein Kasten); die Disc-Regel "hoechstens
  zwei Zeilen" gilt fuer alle Untertitel. Der Schraeggang 4080/4081 gilt als nicht schlechter als v0.8.30. Den frueheren
  Entscheid, zwei Raeume wie im Original ohne Kartenstueck zu lassen, haben Sie am 18.11. verworfen; er ist
  zurueckgenommen.

## Fuer Sie zum Aufnehmen

Fuer v0.8.31 ist nichts mehr offen. Ihre Aufnahmen vom 20.11. sind alle im Spiel: die 75 Saetze der Teile A bis C aus
C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/FEHLENDE_AUFNAHMEN_2026-11-20.md. Die Saetze fuer
v0.8.32 (Teile D, E, F) liegen schon bereit. Am 20.11. um 18:34 geprueft: Jede Datei liegt unter ihrem Namen, im Format
WAV, 16 Bit, Mono, 32000 Hz. Keine alte Aufnahme mit falschem Text liegt mehr unter ihrem Namen. Die 13 Saetze, die bis
dahin stumm liefen, sprechen jetzt.
* **Irons in Elzas Buero (1151):** Der Satz "I'll be fine. Just worry about yourself for now." braucht keine Aufnahme.
  Das Spiel zeigt ihn bei Elza nicht, denn dort steht seit Runde 38 auf Ihren Wunsch "He has already passed...".
* **Saetze fuer v0.8.32 (Teil D):** Die 35 Dateien liegen schon unter synchro/ und kommen deshalb mit ins Paket, das
  Spiel ruft sie in v0.8.31 aber nicht auf.
* **Zug 6031 (Elza):** Die neuen Saetze "A woman brought Marvin down here..." und "She said she had to find someone..."
  haben Sie schon mit dem neuen Text aufgenommen.
* **Schon im Spiel:** synchro/STAGE5/room5090/main02.wav - Leon: "Game over." und synchro/STAGE5/room5091/main02.wav -
  Claire fuer Elza: "You lose, big guy!", nach Ihrer Freigabe vom 18.11. ("selbstverständlich! Sollst du sogar!") aus
  Ihren RE2-Stimmen. Annettes Saetze im Birkin-Film spricht die RE2-Stimme.
* Fund- und Systemtexte ("There's a power supply.", die Plaene, die neuen Gegenstaende) brauchen keine Aufnahme.

## Technisch

* **Stand v0.8.31 (Entwurf):** Sammelzweig r42/story, HEAD b2d4296bc (19.11.), auf v0.8.30 (f3c0a7e70, eingemischt mit
  3bf780233). Ausgangsstand aller Straenge: aaea5f8c2 = v0.8.30 mit Story Teil 1 bis 3 (afc9b3797 bis 2e430ade5, Test
  ddf245147) und den Zusammenfuehrungen vom 17.11.: cdde4804e (r41/release_0830, 44cc5b7d5), 0bc4a44cc
  (r42/hangar_aussen, fe5436f55), ac2c93f5b (r41/stimmen, 73aee131a), dazu ecaf85b1a und 9d808d948.
* **Zusammenfuehrungen vom 18./19.11. in dieser Reihenfolge:** 6d7b19166 r42/gegenstaende (5515f2601), 102feb2fb
  r42/gore (b8b879d54), d8a5b378a r42/rest (3e1a14809), 0d5ba0cfe r42/birkinfilm (b5bb0edd0), b3193d2b8 r42/lageplaene
  (c35ab51a5), 8db089f5d (Kommentar), d7285c8de und 93821acef (zwei Testanpassungen), 364bf10a9 r42/original
  (115ec88e6), 9cf52876f r42/kellerplan (2a2d45a1a), b2d4296bc r42/finale (35965debe, Stufen 1 bis 5; sechs Konflikte
  geloest, der G5-Code faellt weg, Gore- und Gefaengnis-Teile bleiben).
* **Im Sammelzweig (Stand 22.11.):** alle r42-Zweige. Nach dem 19.11. kamen dazu: r42/karte, r42/namen (Stufen 1 bis 7),
  Finale Stufe 6 und 7, r42/ton (Merge bcd81b1ae; einziger Konflikt die Include-Zeilen in audio_pc.c, beide behalten),
  r42/fabrikplan (Merge faa27a6ce, konfliktfrei; enthaelt Ihre Aufnahmen, Commit ce5ec405f) und r42/riegel (Merge
  4b9d26f34). master steht auf 4b9d26f34.
* **Geprueft im Sammelzweig:** die Zahlen unter "Pruefstand" (206 von 206; 468 von 474 mit fuenf Einzelwiederholungen
  und der Anpassung von integration_r37_itemband; integration_r39_toene_schublade und _elza angepasst; 76 von 76; 155
  von 155). Volle Suite auf dem Endstand 4b9d26f34 ohne parallele Agenten: 1774 von 1774 (17008 s), das ist zugleich die
  Abnahme nach dem letzten Code-Commit. Linux-Suite im Container und Release-Bau fuer Windows, Linux/Steam Deck und
  Android laufen mit release_alles.sh; ihr Ergebnis steht im Release-Commit.
* **Neue Dateien im Paket:** MOVIE/R600ZUG.STR (Zugfilm) und MOVIE/R409BIRK.STR (Birkin-Film), je mit Herkunftstext;
  MASKS/ROOM4090, 4091, 50D0, 50D1, 5100, 5101, 6000 und 6001 je _RE2.MSK mit Bild; RE2/R409_XA09.XAS, R409_XA11.XAS und
  R409_XA14.XAS (Annettes RE2-Stimme); RE2/JOHN_49_WURF.EMD; RE2/R702.ESP, R702.EDT, R702.VH, R702.VB, vier R702-Bilder,
  R702_WERFER.MD1/.TIM, R702_XA03_LEON.XAS und R702_XA03_CLAIRE.XAS (Finale); synchro/STAGE5/room5090/main02.wav und
  room5091/main02.wav. Geaendert: MASKS/ROOM6030.MSK und ROOM6031.MSK (Tiefe der Maske ueber Marvins Liege). Entfernt:
  die R704-Dateien des G5-Auftritts. Erwartet, aber noch nicht vorhanden: MOVIE/ENDE5090.STR und MOVIE/ENDE5091.STR (Ihr
  Endvideo).
* **Speicher-Bits:** Bank 9 (alle im Original frei, Zensus und vollstaendige Liste in
  analysis/befunde_runde42/VERTRAG.md), unter anderem: Story und Finale 118 bis 124, 129, 130, 137; Plaene 34, 35, 37,
  Laborblatt 36, Dach 42; Gegenstaende 40, 41, 43 bis 51, 170. Der Birkin-Film haengt an Bits, die die Raumskripte schon
  fuehren ((3,100) bei Leon, (4,237) bei Elza). Die Optionsdatei re15_options.cfg bekommt bei VOICE OFF die Zeile
  voice=off.
* **PSX:** nicht gebaut und nicht geprueft. Dort gibt es keinen Filmspieler (Zugfilm, Birkin-Film, Endvideo),
  Nachrichten mit Nummern ab 32 werden nicht angezeigt, Texte ueber 128 Zeichen bleiben abgeschnitten (2040, 6030), und
  es fehlen die Vollbild-Standbilder, die Gegenstandsbilder und die vier RE2-Masken.
* **Titelbild:** Die Videos dieser Runde zeigen im Titel "Ver.0.8.29 dev" (Stand der Zweige). Nach dem Release-Bau
  pruefen, dass "Ver.0.8.31" dasteht.
* **Messhaken (nur Diagnose, kein Spielweg):** RE15_VOICE=0, RE15_FINALE_WERFER (Werfer ohne Wurfszene),
  RE15_U42_ORIGINAL (3091 wie in der Mod, fuer einen alten Test), RE15_GORE_ZEICHEN_LOG, RE15_FRAMEDUMP_TAKT,
  RE15_AUDIO_CAP_SYNC und RE15_AUDIO_CAP_VSYNC (Bildabzug und Ton im Spieltakt fuer Videos bei getrennter Sitzung).
* **Grenzen der Pruefung:** Viele Laeufe und Videos beginnen mit einem Sprung in den Raum und gesetzten Spielstand-Bits
  (je Abschnitt genannt). Mehrere Videos entstanden bei getrennter Windows-Sitzung als Bildabzug statt als
  Fensteraufnahme (Finale Stufe 3 und 5). Unter Last flattern Fenster-Tests; rot gilt erst nach der Einzelwiederholung.

# v0.8.30 - 2026-11-17

Ihr Auftrag vom 30.10. (Nachtrag 34, nach v0.8.29): "Ok, ziehe die neuen branches mit rein, und schliesse den Rest ab."
(AUFTRAG.md Z. 1091) - dazu Ihre Nachtraege 35 bis 38 vom 30./31.10. (Alligator-Befunde, Sirene, Panel-Freigabe,
Bahnsteig-Birkin) und Ihre Befunde und Auftraege vom 1.11. bis 17.11. zu den Gore-Effekten, dem Kanalwasser in 2000, den
Zombie-Armen, dem Sicherungskasten und dem Panel in STAGE2, der Handgranate und dem Waffenschaden, den Story-Texten nach
dem 80%-Stand, dem G5-Auftritt im Zug, dem Alligator, der Annette-Szene, den beiden Birkin-Formen und ihren Kaempfen,
den Gorilla-Filmen, den Monitoren, der Zielhilfe, der Pipe und den Optionen; der Wortlaut steht je Abschnitt. Diese
Fassung enthaelt vier Gruppen: erstens die fuenf Spuren, die v0.8.29 unter "Ab Samstag (neue Tokens)" angekuendigt hat
und die seit dem 31.10. (Nachtrag 30, Key Items, Alligator-Reste) bzw. 1.11. (Restpunkte, Bahnsteig-Birkin) auf master
liegen; zweitens die erste Welle der Spuren vom 1.11. bis 7.11. (Gore, Kanalwasser, Zombie-Arme, Kasten und Panel,
Handgranate, Story, G5-Auftritt, Waffenschaden) - seit dem 7.11. auf master; drittens die zweite Welle (Alligator,
Annette-Szene, Riesenform mit Bewegung, Kamera und Texturen) - seit dem 8.11. auf master, am 9.11. ergaenzt um zwei
Korrekturen (gestreckte Sherry und Annette, Labor-Birkin); viertens alles vom 9.11. bis 17.11.: Ihre Alligator-Befunde
vom 9.11., die Sichel-Form V8 mit der ueberarbeiteten Spider-Man-Szene und dem Hangar danach, die Riesenform mit neuem
Hals, rechter Hand, Vorderzaehnen und Haut K5g3, Birkins Klauenhieb und Aufrichten, die Gorilla-Filme in 1141 und 11C0,
die flimmernden Monitore, die Zielhilfe, die Pipe und die Optionen - jede Arbeit auf einem eigenen Zweig gebaut und
geprueft und am 17.11. im Zweig r41/release_0830 zusammengefuehrt -, dazu eine Korrektur an den Boden-Schatten aus der
Abnahme. Die Sichel-Form V7 und die Riesenform-Haut K5e, die am 9.11. dazukamen, sind damit wieder ersetzt.

**Warum v0.8.30 erst jetzt kommt:** Der Release-Bau vom 9.11. brach zweimal im Linux-Schritt ab - einmal an der
Linux-Portabilitaet neuer Tests und Messhaken, einmal an Ihren umbenannten PRI-Freistellungen, die der Linux-Lauf aus
dem Arbeitsbaum las. Beides ist behoben (b8e3ad700, 0b86e36c6; die Umbenennung ist nach Ihrem Entscheid "Umbenennung
committen" im Repo, 0fb3d8247); ein Tag wurde nicht gesetzt (Memory reai-v2-runde41-laufend.md, 9.11.). Am selben Tag
kamen Ihre Alligator-Befunde und Ihr Urteil zur Riesenform K5e im Spiel (siehe "Riesenform - Hals, rechte Hand,
Vorderzaehne, Haut K5g3"). Den Vorschlag, v0.8.30 am 11.11. ohne die beiden Birkin-Nachbesserungen zu veroeffentlichen,
haben Sie mit "Nein" beantwortet (Protokoll vom 11.11., Protokollfassung). Am 14.11. kam Ihr Auftrag, parallel mit der
Story fuer v0.8.31 zu beginnen: "parallel schon mal mit den Story Implementierungen anfangen, damit ich danach die
letzten Voiceovers aufnehmen kann." (Protokoll vom 14.11.).

Wie gewohnt hat ein zweiter Agent jeden Punkt unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen; wo eine
Abnahme nicht bestand, wurde nachgebessert und erneut abgenommen. Eine Ausnahme steht unten offen: Fuer den
Bahnsteig-Birkin (Spur R) gibt es nach der dritten Abnahme (Liste N1 bis N6 mit einem Blocker; die Datei traegt kein
Urteil) keine vierte Abnahme-Datei; die Behebung der sechs Punkte ist nur im Dossier der Spur belegt, das Protokoll
fuehrt "Runde 3 bestanden".

**Umfang dieser Fassung - Ihr Entscheid vom 7.11.:** Auf den Vorschlag, v0.8.30 sofort ohne die Birkin-Arbeiten zu
bauen, antworteten Sie "Nein" und auf die Rueckfrage "Mit allem in Arbeit"; dazu: "Aligator anette und die Formen
gehoeren da auch rein, aber nach der 500. Iteration muss es ja mal gut und abgenommen sein...." (Protokoll vom 7.11.,
17:30). Deshalb endet jede laufende Schleife der zweiten Welle mit ihrer letzten Runde; danach wird abgenommen oder
Ihnen zum Ja/Nein gezeigt, nicht weiter poliert. Ihr Wort vom 6.11. zu den Birkin-Modellen gilt weiter: "wir machen noch
2 weitere Versuche danach die Birkin Modelle vernuenftig hin zu bekommen. Wenn ich das Gefuehl habe wir machen
ueberhaupt kein Fortschritt in die richtige Richtung, drehen wir das alles zurueck und nutzen wieder den Birkin aus der
Mod Version und entfernen die Szene mit dem Spiderman wieder." (Memory reai-v2-birkin-zwei-versuche-sonst-rueckbau.md Z.
8). Einen Rueckbau gibt es nicht: Die Sichel-Form V8 haben Sie am 15.11. angenommen ("Naja, so ist es in Ordnung, so
kann ich damit leben.", Protokoll vom 15.11.), bei der Riesenform den neuen Hals (14.11.), die rechte Hand (15.11.) und
die Vorderzaehne (16.11.) - siehe "Sichel-Form" und "Riesenform - Hals, rechte Hand, Vorderzaehne, Haut K5g3". Offen ist
bei der Riesenform die Farbe; zu den Vergleichsbildern vom 17.11. sagten Sie: "Naja, die Form sieht doch gut aus. Bei
der Farbe weiss ich es nicht." (Memory reai-v2-birkin-zwei-versuche-sonst-rueckbau.md) - siehe "Zu entscheiden".

**Vormerkung fuer v0.8.31 - Ihr Wort vom 7.11.:** "Fuer Die Version 31 moechte ich dann, das du die Form Birkin G5 aus
dem Finale wieder entfernst. Die passt nicht... Stattdessen, machen wir ein finale wie beim Golem am Ende von Resident
Evil 2. Das jemand Leon den Raketenwerfer zuwirft, und man dann damit episch von mehreren Wiederholungen sieht, wie die
Rakete angeflogen kommen und den Gegner zersprengt." (Memory reai-v2-story-ende-annette-g5.md, Nachtrag 7.11.). In
v0.8.30 bleibt der G5 im Finale; der Umbau ist fuer v0.8.31 vorgesehen.

**Vorschau auf v0.8.31 (nicht in dieser Fassung):** Grundlage der Story ist das Konzept vom 9.11.
(C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md) mit Ihren Korrekturen; es schliesst die
Luecken der Mod in STAGE3 bis STAGE6 mit einer gemeinsamen Linie fuer Leon und Elza. Schon gebaut und von Pruefern
abgenommen, aber bewusst nicht in v0.8.30:
* Story Teil 1 bis 3 auf dem Zweig r42/story: STAGE2 (Schluesselloch und Nachgespraech in 2080/2081, die Uhr neben dem
  Schluessel in 2090), STAGE1 (Szene vor der Leiter in 11A0/11A1 mit der Rueckgabe der Uhr), STAGE3 (Marvin, 30E0 in
  einem Besuch, eine neue Zeile in 3070) und STAGE6 (neue Gespraeche im Zug 6030/6031, Elzas Zugstart). Die
  Aufnahmeliste dazu haben Sie am 14.11. bekommen (11_story_v0831/AUFNAHMELISTE.md).
* Die Spider-Man-Szene "von aussen" (Zweig r42/hangar_aussen, Ordner 39): Sie laeuft, wenn man aus dem Monitorraum
  5030/5031 durch die Nordtuer in den Hangar-Gang zurueckgeht; im Gang gibt es keinen Kampf mehr. Ihr Wort vom 16.11.:
  "Bei Szene aussen muss das aber vom Monitor Raum aus erfolgen, wenn man zurueck laeuft, sonst fragt man sich - wenn er
  Richtung Kamera zum anderen Ausgang hin geht wo er denn hin ist..." (Memory reai-v2-hangar-szene-monitorbild.md).
* Die Option VOICE ON/OFF (Zweig r41/stimmen, Ordner 47): Stimmen stumm, Untertitel bleiben; Laute ohne Untertitel
  bleiben hoerbar.
Diese drei werden gerade mit v0.8.30 zur Basis fuer v0.8.31 zusammengefuehrt. Entschieden, aber noch nicht gebaut:
* Gore und Schaden nach Ihrer Rangfolge vom 16.11. (woertlich): "Gore und Schaden - ganz klar - wenn ich frueher was
  anderes behauptet habe, falsch - 80% build ist ausschlaggebend, dann 40% build und wenn aequivalente Gore Effekte bei
  40% build und wenn dort Dinge noch fehlen dann kommt Resident Evil 2 zum Einsatz." Fuer die Zombies gilt Ihre Ausnahme
  "In dem Fall dann doch eher Resident Evil 2, einfach weil es spielerisch unspassig ist auf diese Weise." (beides
  Memory reai-v2-re2-ki-nur-angriffsmuster-gore-max.md). Fruehere Einzelentscheide zu Gore und Schaden, die dem
  widersprechen, sind damit aufgehoben - laut Memory u. a. die doppelten Lebenspunkte fuer Gorilla und Birkin und die
  anschiessbaren Gitterhaende. Dazu Ihre Entscheide zum Bauplan: Der Granatwerfer trennt den Oberkoerper ab, die
  Handgranate zerteilt; es gibt hoechstens zwei Handgranaten (eine in Irons' Buero, hoechstens eine weitere aus dem
  Bausatz), und ihr Schaden ist nur so hoch, wie der zaeheste Zombie braucht ("Nur genau so viel wie benoetigt.", Datei
  gore_matrix/NUTZERENTSCHEIDE_1116.md im Protokollordner). Bis dahin gelten in v0.8.30 die Werte unter "Handgranate"
  und "Waffenschaden".
* Gegenstaende ohne Verwendung (Konzept K-10, 11_story_v0831/GEGENSTAENDE.md): nach Ihren Antworten vom 16.11. die
  Mutter am Ventil in 2030, der Granaten-Bausatz der 40%-Disc und der Colt Python als Belohnung in 2040; nach dem Plan
  dazu ein Fundort fuer die Pipe im Kesselraum 11F0/11F1 und fuer den Raketenwerfer in der Asservatenkammer 1110.
* Unbenutzte Hintergruende (Ihre Entscheide vom 17.11.): Das Loch im Boden von Wagen B (ROOM509 Bild 10) ist nach dem
  Anfahren "einfach da", die Rechner zeigen beim Benutzen ihre Vollbilder (305, 2080, 505), ROOM1250/1251 bleiben liegen
  (Memory reai-v2-unbenutzte-hintergruende.md).
* Das Finale nach dem RE2-Ende mit dem Raketenwerfer statt des G5 (Vormerkung oben) und der gerenderte Film "Zug faehrt
  an" (Raum 600). Ihr Urteil zur ersten Fassung vom 17.11.: "Insgesamt von Kamerafuehrung und Inszenierung und Sound her
  und so genial, aber die Umgebung ist viel zu leer. Da fehlt die Ganze Raum Deko." (Memory reai-v2-zugfilm-600.md); die
  zweite Fassung mit Raumdeko ist in Arbeit.
* Ihre 13 naechsten Punkte vom 15.11. (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/29_naechste_punkte/LISTE.md)
  kommen nach Birkin und Story.

Regel der Runde (Abschlussplan zu Nachtrag 34, AUFTRAG.md Z. 1110-1112): "Abnahmen fallen nur bei blockierenden Fehlern
oder einem nicht erfuellten Nutzer-Punkt durch; Feinheiten kommen als "offen" in die Release Notes; hoechstens EINE
Nachbesserung je Spur." Den letzten Teil hat diese Runde mehrfach ueberschritten: Gore drei Nachbesserungen, Handgranate
drei, G5-Auftritt sechs, Bahnsteig-Birkin drei, Kanalwasser zwei; je eine Nachbesserung hatten Nachtrag 30, Key Items,
Alligator-Reste, Restpunkte, Kasten/Panel und Story; die Zombie-Arme bekamen nach der Abnahme nur Berichtigungen an
Kommentar, Dossier und ein Vorher/Nachher-Bild. Die zweite Welle liegt weit darueber (Alligator neun Abnahmen, Annette
fuenf, Riesenform-Kamera vier, Riesenform-Texturen drei Versuche - der letzte in fuenf Runden -, Sichel mehrere
Modellfassungen und Kopf-Runden) - siehe "Noch offen". Nach dem 9.11. kamen dazu: Ihre Alligator-Befunde vier Abnahmen
(zwei nicht bestanden), die Sichel-Form V8 sieben Modell-Runden mit eigener Nacken-Spur und weiteren Runden fuer
Handfarbe und Schwanz, die Spider-Man-Szene sechs Runden, der Schwanz in der Szene fuenf, die Riesenform vier Runden am
Hals, neun an der rechten Hand und drei an der Haut K5g3, der Klauenhieb mehrere Runden mit zwei verworfenen Ansaetzen
fuer die Schwanz-Gestalt.

Pruefstand: Die fuenf Spuren vom 31.10./1.11. sind auf master integriert (Merge O+P: 1033 Unit-, Eigenschafts- und
Sonden-Riegel und 192 Integrationstests gruen; Integration Q+R laut Protokoll vom 1.11.: 1044 Unit-, Eigenschafts- und
Sonden-Riegel, Zielgruppen gruen, Gegenpruefung ohne Befund). Die erste Welle ist seit dem 7.11. auf master (Unit-,
Eigenschafts- und Sonden-Riegel 1067/1067 gruen, 42 Waffenschaden-Integrationsriegel und 16 Zug-/Querbezugs-Riegel
gruen, Gegenpruefung in Ordnung; Protokoll vom 7.11., 17:00). Die zweite Welle ohne die Sichel-Form ist seit dem 8.11.
auf master; dafuer liefen im Integrationsbaum drei Laeufe (A, A2, B) mit allen Querbezuegen zur ersten Welle. Der letzte
Lauf B endete gruen: 1098/1098 Unit-, Eigenschafts- und Sonden-Riegel; Alligator 48 Unit- und 20 Spiel-Riegel;
Riesenform 20; Annette und Story 30; Waffen mit Alligator-Querbezug 110 (zwei davon scheiterten am Bildmitschnitt des
Grafiktreibers und liefen danach einzeln je zweimal gruen); Klettern mit Kette A+C 31. Die Gegenpruefung der Merges fand
keine unerklaerte Zeile, nur eine fehlende Alligator-Datei in der Paket-Pruefung - nachgetragen. Am 9.11. folgten im
Integrationsbaum Lauf C (Sichel V7), der Labor-Fix und Lauf D (Riesenform K5e): jeweils 1100/1100 Unit-, Eigenschafts-
und Sonden-Riegel, gezielte Gruppen 108/108, 40/40 und 115/115, alle Gegenpruefungen ohne Befund. Die exe im Hauptbaum
stammt vom 9.11., 10:30 (nicht 07:03 wie in der Fassung vom 9.11.) und enthaelt nichts von dem, was danach kam.

Nach dem 9.11. lief jede neue Arbeit zuerst auf einem eigenen Zweig, mit eigenen Riegelgruppen und unabhaengiger
Pruefung (Zahlen je Abschnitt und unter "Technisch"). Am 17.11. wurden alle Zweige im Zweig r41/release_0830
zusammengefuehrt: r41/integration (Alligator-Befunde), r41/zwischensequenz_114, r41/monitore, r41/zielhilfe, r41/pipe,
r41/optionen, r41/sichel_szene, r41/birkin_aufrichten, r41/form und r41/birkin_kralle. Die erste Abnahme-Suite darauf
(Stand ea630d75f) endete mit "99% tests passed, 12 tests failed out of 1567" (16366,56 s). Neun der roten Riegel waren
gewollte Folgen der neuen Zweige (Pipe-Reichweite, Zielhilfe, Riesenform-Modell K5g3, Form-Lebenspunkte 1000, eine neue
Protokollzeile) und sind mit Begruendung angepasst (16d9dffdd, 8c54123dd, 5ae09b47b); zwei hingen an der Umgebung; einer
zeigte einen echten Port-Fehler. Die erste unabhaengige Pruefung der Anpassungen liess das nicht durch, weil eine
Anpassung diesen Fehler nur umging ("NICHT BESTANDEN - 1 roter Punkt"); jetzt ist er behoben (29312b011, siehe
"Boden-Schatten nahe der Kamera"), die zweite Pruefung: "BESTANDEN - kein roter Punkt" (Gruppe "100% tests passed, 0
tests failed out of 291"). Zuletzt kam der Klauenhieb ohne Ausholen dazu (44cc5b7d5). Finale Abnahme auf 44cc5b7d5:
"99% tests passed, 1 tests failed out of 1569" (18411,03 s), rot nur unit_r39_titelversion_cmake. Danach kam am 17.11. abends Ihr Wunsch dazu, dass der Alligator nur durch den Kanister stirbt (ec0d39bb9, siehe "STAGE2: Alligator stirbt nur durch den Kanister"): eigener Riegel, Alligator-Gruppe "100% tests passed, 0 tests failed out of 167", unabhaengige Abnahme "BESTANDEN". Im Release-Bau kamen zwei reine Test-Korrekturen dazu, das Spiel selbst aendern sie nicht: zwei Pruefprogramme der Sichel-Szene binden unter Linux die Mathe-Bibliothek (a42e30c07, der Linux-Bau brach sonst beim Linken ab), und der Schwanz-Nahttest bekam die Grenze 100 statt 0 (4da90e138, aus der v0.8.31-Basis 9d808d948): ob an der Naht ein paar Einzelpunkte erscheinen, haengt von der Rasterung des Grafikadapters ab (lokale Konsole 0, NVIDIA in der Fernsitzung 35, Linux-Container 6), die Szene selbst ist bitgleich. Bekannt umgebungsbedingt rot: unit_r39_titelversion_cmake (git fehlt im PATH von local_build.sh);
unit_r38_releaseskript_ablauf ist nur bei langem Build-Pfad rot ("Filename too long"), die Abnahme baut deshalb mit
kurzem Pfad. Beim Release wird master auf ec0d39bb9 vorgezogen und die Hauptbaum-exe neu gebaut; die volle Suite laeuft
beim Release-Bau zusaetzlich im Linux-Container, ein roter Test haelt das Release an.

## Was Sie merken

### STAGE1: Zombies mit Original-Armen (ROOM1140, ROOM1250)

Ihr Befund vom 2.11.: "wtf, warum haben denn die dicken Zombies jetzt ploetzlich so einen duennen Stummel Arm?" und "Den
sollen sie nicht haben. Die sollen schon den original Arm haben...." - dazu Ihre Regel "Ja, das Aussehen der Modelle bei
Resident Evil 1.5 ist entscheidend! Haben wir im Port jetzt Stummel Aermchen oder irgendeinen anderen Quatsch gemacht ist
das falsch." und "Ist ok, auch fuer die dicken Zombies. Wenn du zu 100% sicher nachweisen kannst, dass das auftaucht mit
den duennen Aermchen, dann lag ich wohl falsch." (Dossier ZA_zombiearm.md Z. 6-11). Abnahme 1 bestanden.

**Die duennen Glieder gibt es nicht mehr.** Ursache war eine Spawn-Variante aus einem Zwischenstand nach v0.8.29 (nicht in
v0.8.29 selbst), die bei einem Viertel aller Zombies ein Glied auf 37 % Dicke skalierte und verfaerbte. Der Nachweis fiel
gegen die Variante aus: Das Original wuerfelt sie zwar, setzt aber am Ende jeder Zombie-Initialisierung jedes Teil wieder
auf volle Groesse - sichtbar wird sie nie. 27 Spielstaende (26 aus dem Mod-Baum, einer von der 40%-Disc) mit 131 Zombies,
davon 99 mit dem Rest des Wurfs: alle Teile voll. Deshalb ist die Variante fuer alle Zombie-Typen abgeschaltet, unter
beiden Gehirnen und in beiden Modellmodi (Riegel je Typ: 400 Saaten, kein veraendertes Teil). Im Besprechungsraum 1140
(Leon) haben beide dicken Zombies (weisses Kurzarmhemd, nackte dicke Unterarme) wieder volle Arme, die Uniform-Zombies den
Aermel mit dem RPD-Abzeichen durchgehend; ebenso die Gruppe in 1250. Sonst aendert sich nichts: Die beiden Zufallswuerfe
bleiben, der Zufallsstrom ist mit dem vorherigen Stand identisch, Trefferzonen, Lebenspunkte und alles Treffer-/Tod-Gore
laufen wie bisher. Elza: Die Abschaltung haengt an keiner Figur; 1141 hat keine Gegner, in 1251 zeigt das Dossier-Bild
die Zombies klein am linken Rand. Nicht belegt: Die Abnahme hat Elza nicht eigens im Fenster gemessen; ihr Sichtbeleg ist Leon in 1140 und 1250.

### STAGE1: Gorilla-Film im Dialog - Elza in ROOM1141, Leon in ROOM11C0 (Zwischensequenzen 114 und 11C)

Die beiden Filme sind Ihr eigenes Projekt: Sie haben die Szenen in Blender mit Ihren Modellen aus Cutscenes/ nachgebaut,
Helfer haben Bild und Ton nach Ihren Vorgaben gerendert (Memory reai-v2-cutscene-blender-raum114.md). Ihr Wortlaut zum
Einbau vom 13.11.: "Ich wuerde im Dialog bei John und Elza vor 'What was that?' diesen Schrei-Sound von dem Gorilla
einbauen, aus 11C Sekunde 6. Und dann kurz warten, bevor ich das Video abspiele. Nach dem Video geht es normal weiter
mit 'Shit! that is what I was talking about!'" (Commit f0d4cfac4), zur Blende: "Wie ist es in Resident Evil 2 gemacht,
wenn die Filmsequenzen starten? So machen wir das auch." (bddc94eec) und zu 11C: "Diese Version des Videos sieht super
aus! Auch da brauchen wir vor dem "What!" Von Leon wieder ein Bruellen vom dem Gorilla. Dann kurze Pause, dann das
Video, dann geht es weiter mit "Ada, you hide in that patrol car"" (b5ee75c27). Zum Einbau von 114 im Spiel: "einfach
super" (Protokoll vom 13.11.).

**Elza, ROOM1141 (John und Elza).** Wenn John ausgesprochen hat, bruellt der Gorilla (Satz 4 der Raumbank von 11C0, gut
1,2 s), kurze Pause, dann "Elza: What was that?!"; Elza dreht sich zum Fenster, kurzes Warten, harter Schnitt in den
Film (367 Bilder, gut 12 s), danach 0,5 s Einblenden aus Schwarz wie in RE2 und "John: Shit! that is what I was talking
about!". Der Glasbruch der Runde 39 vor "What was that?!" entfaellt, weil der Film das Zerspringen selbst zeigt; die
Musik startet wie im Original nach Johns Zeile.

**Leon, ROOM11C0 (Leon und Ada).** Nach "Ada: You didn't run into him?" bruellt der Gorilla, kurze Pause, "Leon:
What!?", beide drehen sich zum Anhaenger, dann der Film (362 Bilder, 12,07 s). Den Original-Auftritt der Gorillas in
Kamera 12 (Anhaenger wackelt, Gorillas werden gesetzt) spult das Spiel im schwarzen Bild unsichtbar vor; eingeblendet
wird mit der Original-Kamera 5 und "Leon: Ada, you hide inside that patrol car."; die Kampfmusik startet erst nach dem
Film.

START ueberspringt den Film; fehlt die Datei, laeuft die Szene ohne ihn weiter. Leon hat in 1140 und Elza in 11C1 diese
Dialoge nicht und sieht keinen Film (ROOM11C1 hat keinen Nachrichtenblock). Die Filme liegen als MOVIE/R114GORI.STR und
R11CGORI.STR im Format von CAPCOM.STR vor (320 x 240); auf der PSX laufen die Zeilen ohne Schrei und Film.

**Wie geprueft:** Riegel fuer beide Raeume (unit_r41_zws114_*, unit_r41_zws11c_*, darunter "600 Bilder ohne Schrei/Film"
fuer Elza in 11C1) und der Durchlauf Elza 1131 -> 1141 -> 1171 ("Film zu Ende: 367 von 367 Bildern gezeigt"); in 11C0 im
echten Fenster gemessen: harter Schnitt in den Film, keine Kamera 12 im ganzen Lauf, der Stand nach dem Film mit und
ohne Vorspulen "0 von 118 Bildern abweichend". Drei aeltere Riegel sind auf die laengeren Szenen nachgezogen (Zielen und
Nachladen in 1141: 33 Bilder, Granate in 11C0: 38 Bilder, ein Sicht-Riegel in 1141). Einen unabhaengigen Pruefer gab es
fuer den Einbau nicht (siehe "Noch offen").

### Im ganzen Spiel: Key Items ueber das Inventar, Kurbel ohne Fliegen (Spur O)

Ihr Nachtrag 34: "Auch das mit der Kurbel scheint noch nicht gefixed - leon "fliegt" quasi immer noch an die richtige
Position bis das drehen dann los geht. Bei Resident Evil 2 ist die Kurbel Animation offenbar deshalb nicht
kaputt/fliegend, weil man EXPLIZIT die Items im Player Inventory auswaehlen muss und dann genau richtig steht bei der
Nutzung. Das moechte ich bei Key Items, die nicht Schluessel sind bei unseren Port auch haben!" und zu den Karten:
"Schaue wie es in Resident Evil 2 gemacht wird, und mache es genauso." (AUFTRAG.md Z. 1094-1101). Abnahme 1 bestanden,
keine Maengel.

**Bedienung.** START, ITEM, Gegenstand waehlen, USE. Steht die Figur an der richtigen Stelle (dieselbe Pruefung wie die
Aktionstaste: Ebene und Blickpunkt), schliesst das Inventar und die Handlung startet sofort, ohne Text und ohne Frage;
sonst "You can't use it here.", und das Inventar bleibt offen. Die Aktionstaste zeigt an diesen Stellen nur noch die
Beschreibung, keine Frage mehr. Das gilt fuer Feuerloescher (1090), Zange (11E0), Minidisc-Spieler (1100), Kurbel (2020),
Zeitbombe (2040), die Sicherung (1050, 2010, 2060), die Zuendkerze (4030) und die Karten unten - 14 Stellen, alle an der
echten exe mit Leon und, wo es den Raum gibt, mit Elza durchgespielt.

**Kurbel in 2020/2021.** Das Spielbild kommt erst zurueck, wenn Leon an der Kurbel steht und schon dreht: Lage und Blick
werden im schwarzen Bild gesetzt, das kurze Stehen und die Ueberblendung in die Kurbel-Pose (das "Fliegen") entfallen,
wie in RE2, wo die Kurbel ohne Ueberblendung startet. Aus dem Stand, im Gehen, im Rennen und aus drei Blickrichtungen ist
das erste Kurbelbild dasselbe (die Lage, die das Original aus dem Stand erreicht), Elza ebenso. Am Ende der Szene kippt
er nicht mehr zurueck, der Wechsel faellt in den Schnitt. Waehrend der Kurbel-Szene hat Leon wie in RE2 die Waffe nicht
in der Hand, danach ist sie wieder da (RE2 legt sie erst beim naechsten Oeffnen des Inventars wieder an; nicht
uebernommen, weil in dem Raum vier Zombies warten - Port-Wahl).

**Neu im Inventar.** Kurbel und Zeitbombe gab es im Original nur als Flag. Die Kurbel hat Name, Bild, Symbol und Text
aus RE2 ("The end is square-shaped"), die Bombe das ausgelieferte Bild ("Timer Bomb" im Inventar, "Time Bomb" in
Skripten und CHECK - beides ausgeliefert). Beide verschwinden bei der Benutzung. Das gilt auch fuer laufende
Spielstaende. Volles Inventar beim Aufnehmen: "You can't carry any more items", der Gegenstand bleibt liegen; ein alter
Spielstand mit gesetztem Flag und vollem Inventar behaelt an Kurbel, Bombe und Sicherung die alte Frage samt
Original-Ueberblendung, bis ein Platz frei ist.

**Karten wie in RE2.** Karte am LESER = USE aus dem Inventar (Blue/Yellow/Red Keycard in 10D0, 11E0, 1230, Red Master
Keycard 3050, Blue Master Keycard 4000; RE2-Vorbild Red Card Key). Karte als TUERSCHLOSS = Aktionstaste wie bisher (Green
Keycard 3010; RE2-Vorbild Lab Card Key). Schluessel, Latch Key 3091 und "You don't need this key any more" bleiben
unveraendert. Am Kartenleser endet die Beschreibung jetzt vor der Frage-Seite; der Satz "You have not the ... Keycard to
operate it." ist nicht mehr erreichbar (RE2-Form).

**Nebenwirkung, gemessen:** Die RE2-Regel "keine Ueberblendung" wirkt jetzt auch auf die Ventil-Drehung in 2030; gemessen
entsteht beim Loslassen des Ventils kein Pose-Sprung.

### STAGE1/2: Gore unter der Standard-KI (Spinne, Kraehe, Hund, MC51)

Ihre Regel vom 1.11.: "Die spinne und die anderen uebernommenen resident evil 2 kis sollten im wesentlichen nur
angriffsmuster, Treffer usw uebernehmen, weil die resident evil 1.5 ki das teilweise noch etwas doof ist. Der gore darf
gerne der maximale aus beiden sein. Das muss nicht stringent aus resident evil 2 uebernommenen werden." und "Genau
genommen sollte der gore sogar das maximale aus allen 3en sein, inkl den 80% Videos." (Memory
reai-v2-re2-ki-nur-angriffsmuster-gore-max.md Z. 11 und 13). Ihr Befund, dass die Spinne beim Magnum-Schuss nicht
platzt, ist in den Dossiers nur sinngemaess ueberliefert. Nicht belegt: Der Wortlaut Ihres Spinnen-Befunds steht in keiner Quelle. Gemessen: Unter der Standard-KI (RE2-Gehirn) kam der toedliche Treffer an, aber das Platzen blieb aus - die
Spinne platzte nur unter "AI RE1.5". Ausserdem loeschte ein Port-Fehler die Farbe des Spinnenbluts (rot statt gruen).
Vier Abnahmen, die vierte bestanden.

**Spinnen (ROOM2070 Leon, 2071 Elza; Spinnen gibt es nur in STAGE2).** Jeder Treffer, auch mit der Pistole, spritzt
jetzt gruen am Fusspunkt der Spinne (vorher rot), unter beiden KI-Einstellungen. Schrot trennt wie in RE2 ein Beinpaar
ab: gelbgruene Beinstuecke fliegen weg, die Spinne laeuft mit einem Beinpaar weniger weiter. Ein toedlicher Treffer mit
Magnum (Super Redhawk, Colt Python), Schrot, SPAS, Granaten, Rakete oder ein kritischer Treffer laesst Rumpf und
Hinterleib in gelbgruene Scherben zerplatzen, alle noch haengenden Beine fliegen ab und fallen zu Boden, dazu eine
gruene Blutwolke (im Fenster gemessen mit Redhawk und Python, die uebrigen Waffen ueber Riegel). Pistole und Browning:
kein Platzen, nur gruenes Todesblut. Einzelne Scherben fliegen bis vor die Kamera - Mechanik des Originals, nicht falsch
gezeichnet. Unter der toten Spinne waechst eine gruene Lache (RE2-Tinte; unter "AI RE1.5" dunkel und gruenstichig wie in
RE1.5); steht Leon darin, faerbt sich sein Schuh gruen - Zeichenreihenfolge des Originals, durch die groessere
Port-Lache deutlicher als in RE2 (siehe "Zu entscheiden"). Unter "AI RE1.5" folgt der Spinnentod jetzt der Waffentabelle
des Originals: Magnum, Schrot, SPAS, Granaten usw. platzen immer, nicht nur bei kritischem Treffer, und die Beine
fliegen. Babyspinnen: Der Schatten lebender Babys ist kleiner als in RE2; stirbt eines, spritzt es gruen und laesst
einen kleinen gruenen Fleck bis zum Raumwechsel - im Fenster kaum zu erkennen, und Babys sterben im Port praktisch nie
(siehe "Noch offen"). Erreichbarkeit: Die Magnum liegt in ROOM3050/3051 (STAGE3), die Remington in 10F0/10F1, die SPAS
in 2030/2031. Nicht belegt: Welche Waffe im normalen Durchlauf das Platzen zuerst ausloest; ob man mit der Magnum nach STAGE2 zurueckkommt, wurde nicht geprueft.

**Kraehen (ROOM10C0 Leon, 10C1 Elza).** Beim Tod platzt die Kraehe wie bisher in dunkelrote Teile; neu entstehen 8 bis 11
Federn am Koerper, die quer durch den Gang pendeln, zu Boden sinken und dort verschwinden (RE2 laesst keine Feder liegen).
Ein Pistolentreffer im Flug loest 2 bis 3 Federn, der Sturz danach 2 weitere. Die Federn sind blaugrau bis dunkelviolett
(Farbtabelle des RE2-Effektbilds). Beim reinen Fliegen (Abheben, Kreisen, Trudeln, Vorbeiflug) verliert die Kraehe im Port
keine Federn - RE2 tut das; Port-Wahl wegen Ihres frueheren Befunds "Bluteffekte ohne Treffer bei den Kraehen" (Dossier
S_spinnengore.md Z. 999-1000). Unter "AI RE1.5" und bei der Ereigniskraehe gibt es keine Federn. Beleg: Leon Bild 356,
Elza Bild 294 platzt, beide bitgleich bei gleicher Saat. Nebenwirkung der Federn: Jeder Federwurf zieht Zahlen aus dem
gemeinsamen RE2-Zufallsstrom (in RE2 genauso) - nach einem Kraehentreffer verschiebt sich die Zufallsfolge der uebrigen
RE2-Gegner im Raum.

**Hunde (ROOM1190 Leon, Standard-KI).** Beim Tod fliegen viele dunkelrote Fetzen weg, danach liegen rote Fleischbrocken am
Kadaver: der RE2-Teilewurf (sieben Teile), der bisher nur nicht gezeichnet wurde. Zwei der sieben RE2-Teile haben kein
RE1.5-Netz und zeichnen nichts. Nicht belegt: Hunde mit Elza nicht gemessen.

**Zombies, MC51 beim Tief-Zielen (Standard-KI).** Ein toedlicher MC51-Treffer in die Beine toetet meist nicht mehr: Die
Beine platzen, der Zombie kriecht mit 10 Lebenspunkten weiter (RE2-Weg; RE1.5 laesst ihn mit 30 kriechen); liegen bleibt
er nur in einem von vier Faellen. Beleg: Messhaken und Riegel in ROOM1140 (stirbt Bild 74, steht Bild 127 mit 10 HP
wieder auf); ein freier Fensterbeleg fehlt.

**Schalter.** Mit dem Schalter gegen das 80%-Gore (siehe "Technisch") bleiben das Platzen der Spinne und die
MC51-Beinzelle aus (die Spinne baeumt sich auf, dreht sich auf den Ruecken und bleibt ganz). Das gruene Blut und die
Lache bleiben, weil beide Originale sie haben; Kraehenfedern und Hund-Teilewurf bleiben, weil sie RE2-eigenes Gore sind,
das bisher nur nicht gezeichnet wurde (Dossier S_spinnengore.md Z. 297, 781, 1001).

### STAGE1 bis 5: Handgranate - Zerfetzen und doppelte Birkin-Lebenspunkte (Spur GR)

Ihr Auftrag vom 2.11.: "Und die Granate sollte laut den Videos der 80% Version die Zombies zerfetzen, also die normale,
richtige Handgranate. Ausserdem ist sie noch zu schwach, die sollte auch die dicken Zombies zerfetzen" (Dossier
GR_granate.md Z. 656-657). Ihre Berichtigung zur ersten Fassung: "Das stimmt nicht, bei complete disc video 2 bei 15
Sekunden wirft Leon eine Granate. Und auch die Hunde und andere Gegner die zerfetzen koennen sollen natuerlich
entsprechend zerfetzen von einer Granate." (Z. 407-408), "Nein, nicht die Spinne ist das was zersprengt wird durch die
Granate, sondern die Zombies im Sewer" (Z. 409) und "Bei der Granate das es garkeine Koerperteile mehr hinterher gibt
die uebrig bleiben ist unwahrscheinlich. Wahrscheinlich wuerden die einzelnen Koerperteile nach der Explosion dann da
liegen. Man sieht es nur in dem Video Ausschnitt nicht so richtig." (Z. 410-412). Zur Staerke: "Naja, die Granate hat
halt einen Schaden, sind die Gegner damit tot, sind sie tot. Der Gorilla ist ein Endgegner, wenn der mit einer Granate
stirbt ist das etwas schwach." (Z. 234-235), "Ja, ist gut. Gorilla, Handgranate und ein paar Schuss, und ja auch Birkin
und Aligator, wobei die an sich schon staerker sein muessen als der Gorilla." (Z. 236-238) und "1. ja, 2. ja, 3. huh,
wie jetzt das ist doch ein Boss fight. Warum soll die Granate nicht wirken ?... zu 4. Ja, nicht einfach mit einer
Granate beenden." (Z. 8-9). Zwei Abnahmen, beide bestanden, beide mit je drei nicht blockierenden Maengeln. Gilt fuer
Leon und Elza gleich.

**Was davon bleibt und was der Waffenschaden (naechster Abschnitt) am 7.11. ersetzt hat:** Das Zerfetzen, der Teilewurf
und die neuen Lebenspunkte der Birkin-Formen bleiben. Die Schadenswerte dieser Spur (Handgranate 1000; Bosse mit Rest und
"danach genau N Pistolenschuesse") sind nach Ihrem Entscheid vom 7.11. durch die RE2-Regel ersetzt - die Zahlen stehen
unter "Waffenschaden".

**Zombies, auch die dicken (ROOM1140 gemessen; Elza ROOM1041 per Log).** Eine Handgranate (nicht Saeure- oder
Brandgranate) toetet jeden Zombie im Wirkradius, auch die dicken (die vorher weiter angriffen). Der Zombie zerfliegt in
sieben Stuecke (Rumpf, Becken, Kopf, beide Arme, beide Beine), die bis an den oberen Bildrand fliegen, an Waenden
abprallen, zu Boden fallen und dort verstreut in Blut liegen bleiben bis zum Raumwechsel; kein heiler Koerper bleibt
stehen; der schwarze Rauch steht etwa 1,5 s (vorher 0,3 s). Gilt unter beiden KI-Einstellungen, bei Handgranate und
Granatwerfer-Explosivrunde. Im Kanal 2000 (der Zombie-Typ des Videos) werden Feuerball und Rauch gezeichnet. Vorlage: der
einzige Handgranatenwurf in allen 80%-Videos, Complete Disc Video 2, Wurf bei 16,95 s, Detonation bei 19,25 s am
Kanal-Zombie - Teile bis zur Decke, Rauch, am Ort nichts mehr.

**Hund (ROOM1230).** Eine Handgranate toetet; der Hund wirft seine sieben RE2-Teile, die liegen bleiben; der Rumpf bleibt
wie in RE2.

**Spinne (ROOM2030, Standard-KI).** Eine Granate toetet: Die Spinne platzt, sechs Babyspinnen erscheinen. Unter "AI RE1.5"
platzt sie jetzt auch an der Werfer-Explosivrunde.

**Birkin hat doppelt so viele Lebenspunkte (Ihre Freigabe "1. ja"):** Start 600 statt 300, Mutations-Tor unter 200,
Mutation 300, Revive 100; die Riesenform (5080 und 5090) hat 1000 statt 150 (RE2-G4-Wert, Ihre Freigabe "2. ja"), der G5
600. Nur mit der Pistole hatte die Abnahme im alten Ablauf gezaehlt: Form 1 27 Schuesse bis zur Mutation (vorher 14),
der ganze Endkampf 166 Schuss und etwa 144 s Dauerfeuer; ein stehender Schuetze ohne Ausweichen und Heilen stirbt an der
Riesenform - Ihr Wort dazu: "ist ja der Endkampf" (Protokoll vom 2.11.). Seit der Riesenform-Stufe 3 (Welle 2) kaempft
in 5090 ab dem Betreten die Riesenform mit 1000 Lebenspunkten, ohne Mensch-Birkin und ohne Mutation im Zug. Der
Pistolen-Riegel dieses Ablaufs (Leon und Elza gleich, mit Rueckzug an die Ostwand) endet mit dem Tod des G5 bei vollen
Lebenspunkten des Spielers und 157 verbrauchten Patronen (255 auf 98). ROOM3080 (G-Form im Aufzug) bleibt eine Szene
ohne Kampf (Ihr Entscheid laut Protokoll vom 2.11., Protokoll-Paraphrase). Elzas 3071-Kampf ist nur per Riegel gemessen,
kein Fensterkampf (nach einem Debug-Sprung nach 3070 nimmt Leon keine Eingabe an; ein alter Messweg-Fehler).

**Unveraendert:** Saeure- und Brandgranate, die Werfer-Arten, der Pistolenschaden (Birkin 15, Gorilla 12, Alligator 7).
Mit dem Schalter gegen das 80%-Gore (siehe "Technisch") entfallen Zerfetzen, Teile-Wurf, Liegenbleiben, Rauch-Nachlauf
und der Werfer-Platzer der RE1.5-Spinne; der Schaden bleibt auch dann.

### Im ganzen Spiel: Waffenschaden wie in RE2 - Handgranate und Raketenwerfer (Spur WS)

Ihr Entscheid vom 7.11.: "mach wie resident evil 2. Auch bei der Granate. mach sie so stark, dass sie gerade so die
Standard Gegner mit einer Granate toeten kann." (Dossier WS_waffenschaden.md Z. 10-11). Abnahme 1 bestanden, zwei
geringe Maengel an Kommentaren und Dossier-Belegen (veraltete Zahlen; ein ungenau beschriebener Beleg-Scan), seit dem
7.11. auf master. Jede Zahl unten ist von der Abnahme im echten Spiel mit Leon und Elza (Zombies, Hunde, Spinnen,
Gorilla, Alligator, Birkin in 3070 mit Leon, Endkampf) bzw. per Riegel (Kraehe, Affe, Tyrant, Ivy, Gitterhaende)
nachgemessen oder aus dem RE2-Code gelesen.

**Handgranate: ein Wert fuer alle Gegner.** Die Granate macht an jedem Gegner 251 Schaden - genau einen Punkt mehr als
der staerkste Standard-Gegner (der dicke Zombie, 250 Lebenspunkte) aushaelt. Damit stirbt jeder Standard-Gegner an einer
Granate: Zombies, auch die dicken ("gerade so", gemessen Rest -1 beim echten Wurf in 1140), Hunde, Kraehen, Spinnen, der
kleine Affe, der Tyrant. Die Boss-Regel der Handgranaten-Spur ("Rest, danach genau N Pistolenschuesse") ist weg; Bosse
nehmen denselben Wert: Gorilla 2 Granaten (vorher 1 Granate + 4 Schuss), Birkin in 3070 3 Granaten (vorher 1 + 8), die
Riesenform 4 Granaten, der G5 3 Granaten (vorher einen Sonderwert von 80). Im Endkampf 5090 sind das seit der
Riesenform-Stufe 3 (Riesenform ab dem Betreten, keine Mensch-Birkin und keine Mutation mehr im Zug) 4 + 3 = 7 Granaten;
die Abnahme hatte im alten Ablauf mit der Mensch-Birkin davor 2 + 4 + 3 = 9 gezaehlt.

**Raketenwerfer: Schaden je Gegnertyp aus RE2.** Bisher machte die Rakete ueberall 1000. Jetzt gilt je Gegner die
RE2-Tabelle: Zombies 900, Hund, Kraehe, kleiner Affe und Ivy 300, Spinnen 200, Gorilla 200 (RE2 hat keinen Affen;
Vorlage ist der Licker), Tyrant 500, Birkin als Mensch, Form 1 und nach der Mutation 200, Riesenform 301 (RE2-G4), G5
200, Alligator 110, die Gitterhaende in 1210 nur 40. Im Spiel gemessen: Riesenform 4 Raketen (699, 398, 97, tot - im
Integrationslauf vom 8.11. mit der Annette-Szene davor dieselbe Folge), G5 4 Raketen (nach 3 Raketen steht er mit 0
Lebenspunkten noch - in RE2 lebt ein Gegner mit 0). Der Endkampf 5090 braucht damit 8 Treffer (4 + 4). Die Abnahme hatte
im alten Ablauf (Mensch-Birkin davor, 3 Raketen bis zur Mutation) 11 Treffer gezaehlt und bei Dauerfeuer 19 verbrauchte
Raketen, davon 5 waehrend der Mutation und 3 in der Rueckstoss-Szene ins Leere (WS_abnahme_1.md Z. 209-211); den
Verbrauch bei Dauerfeuer im neuen Ablauf hat niemand gemessen. **Der Raketenwerfer liegt in keinem Raum des Spiels**
(Protokoll vom 7.11.: in keinem der 206 Raeume); diese Zahlen sind nur mit den Debug-Waffen erreichbar - siehe "Zu
entscheiden".

**Alligator-Boss in 2090/2091 (Orchestrator-Zusatz "Original oder nicht").** Seine Lebenspunkte sind jetzt der RE2-Wert
25240 statt 25300 (aus dem RE2-Alligator-Modul gelesen; der Port laeuft wie RE2 mit gesetztem Spielstart-Bit). Folge mit
der neuen Granate: EINE Handgranate schwaecht ihn, und die Kanister-Szene startet sofort (gemessen Leon und Elza, Bild 300
Treffer, Bild 302 Szene) - vorher 1 Granate + 8 Pistolenschuesse. Mit der Pistole allein braucht es 35 statt 43 Schuss bis
zur Schwaeche, mit der Rakete 3 Treffer. Toeten kann ihn weiter nur der Kanister. Die Abnahme vermerkt, dass das Ihrem
frueheren Wunsch "nicht einfach mit einer Granate beenden" widerspricht, aber aus "mach wie resident evil 2" folgt - siehe
"Zu entscheiden".

**Unveraendert:** Saeure- und Brandgranate, die Werfer-Arten, die Spielerseite (eine Granate am Spieler), die Lebenspunkte
aller Gegner ausser dem Alligator-Boss.

### Im ganzen Spiel: Zielhilfe - beim Waffe-Heben dreht die Figur ganz zum Gegner (RE2)

Ihr Wortlaut vom 16.11. (Sitzungsprotokoll): "Ein weiteres Element, was unendlich nervt, aber ich weiss das es schon ein
Fehler im Original ist - wenn ich die Waffe hebe, und der Auto Anvisieren Mechanismus kommt, klappt er nur wenn der
Gegner in einen nicht zu hohen Winkel ist. Ist der Gegner zum Beispiel 180 Grad hinter mir, dreht sich Leon nicht
komplett bis zum Gegner sondern nur teilweise." Eine Pruefrunde: "Ergebnis: bestanden. Kein roter Punkt." (Ordner 40).

**Was jetzt anders ist.** Beim Heben der Waffe dreht die Figur bei jeder Waffe und in jedem Winkel genau auf den Gegner,
auch 180 Grad hinter sich; bei 90 Grad seitlich steht sie nach 4 statt 6 Bildern auf dem Ziel. Vorher fehlten bei 180
Grad mit der Pistole 21,7 Grad, mit dem Messer beim Wiederheben 28,0 Grad und mit der Super Redhawk 56,9 Grad (die
Redhawk nur bei Leon; Messer ziehen, Schrotflinte und MG drehten schon ganz). Ursache: Das Original dreht nur, solange
das Anheben dauert - bei der Pistole fehlen dort 4,2 Grad -, und der Port drehte zusaetzlich im ersten Bild des Anhebens
nicht. Einen Winkel, ab dem kein Gegner erfasst wird, gibt es nicht. Jetzt gilt die Schrittweite von RE2 Retail (Ihre
Regel "Beta -> Retail", Memory reai-v2-beta-zu-retail.md), und das erste Bild dreht mit. Unveraendert: welcher Gegner
gewaehlt wird, keine Zielerfassung durch Waende, Zielwechsel mit L1, Drehen von Hand; eine Leiche wird nicht anvisiert.
Leon und Elza teilen diesen Teil des Spiels; die Messung lief fuer beide ueber alle Waffen (am Ende 0 Grad daneben).

**Wie geprueft:** Im echten Fenster in ROOM1140 mit einem Zombie direkt hinter Leon: Der alte Stand bleibt 21,7 Grad
daneben stehen, der neue steht auf dem Zombie. Riegelgruppe (Zielen, Waffen, Spieler) "100% tests passed, 0 tests failed
out of 199". Mit der Zielhilfe trifft Leon in mehreren Laeufen frueher - drei Riegel waren deshalb anzupassen, und einer
davon zeigte einen alten Port-Fehler (siehe "Pruefstand" und "Boden-Schatten nahe der Kamera").

### Im ganzen Spiel: Boden-Schatten nahe der Kamera wie im Original (Abnahme v0.8.30)

Gefunden in der Abnahme von v0.8.30, kein Befund von Ihnen: Der Port warf jeden Boden-Schatten (Schatten von Spieler und
Gegnern, die Lache unter einer toten Spinne, den Fleck einer toten Babyspinne) ganz weg, sobald eine Ecke sehr nahe an
der Kamera lag. Diese Grenze stammt aus dem Juli (09cab1418), hatte keinen Beleg im Original und steckte auch in
v0.8.29. Sichtbar wurde sie mit der neuen Zielhilfe: Im Spinnen-Riegel (Leon, ROOM2070) stirbt die Spinne jetzt schon an
ihrem Austritt nahe der Kamera, und ihre Lache wurde nie gezeichnet. Das Original prueft dort keine Naehe (RE1.5
FUN_8001b064, RE2 FUN_8001699c); was nahe oder hinter der Kamera bleibt, entscheiden die Rechengrenzen der PSX
(GTE-Saettigung und die Groessengrenze 1023 x 511 Bildpunkte je Dreieck). So rechnet jetzt auch der Port; ist nur eine
Haelfte des Vierecks erlaubt, wird nur sie gezeichnet. Vor der Kamera bleiben es dieselben Bildpunkte.

Gemessen: Die Lache der Spinne am Austritt ist sichtbar (vorher nie), die Flecken toter Babyspinnen im Riegel 2600 von
2600 statt 1600 von 2600; eine lebende Spinne direkt vor der Kamera ist in 319 Bildern ganz weg, wie es die
Groessengrenze verlangt. Zwei unabhaengige Pruefungen: Die erste fand den Fehler hinter einer Testanpassung ("NICHT
BESTANDEN - 1 roter Punkt"), die zweite hat die Disassembly selbst gelesen ("BESTANDEN - kein roter Punkt").

### Im ganzen Spiel: Pipe als Nahkampfwaffe - Rohr in der Hand, mehr Reichweite, keine Anzahl, Birkins Rohr-Ton

Ihr Wortlaut vom 16.11. (Sitzungsprotokoll): "Mir ist ausserdem noch aufgefallen das wir die Pipe als Waffe im Inventar
spawnen koennen, aber sie ist noch nicht nutzbar. Eigentlich sollte die Pipe so aehnlich funktionieren wie ein Messer,
aber mit mehr Reichweite. Ich weiss nicht ob schon irgendwas fuer die Pipe vorhanden ist. Modell oder Animation oder so.
Ausserdem sollte pipe auch wie Messer keine Anzahl haben, sondern halt einfach unendlich da sein." und zum Ton: "Ok, die
sounds muessten natuerlich noch angepasst werden. Natuerlich soll die Pipe nicht klingen wie ein Messer. Sondern eher
ein dumpfes metallenes Aufschlag Geraeusch haben. So wie bei Birkin zum Beispiel. Der hat ja auch eine Pipe." Je eine
Pruefrunde, beide bestanden ("Ergebnis: bestanden. Kein roter Punkt." bzw. "Ergebnis: bestanden, vorlegbar. Es gibt
keinen roten Punkt."), Ordner 42.

**Was die Disc hat (Berichtigung der ersten Auskunft).** Im Code ist die Pipe eine Nahkampfwaffe wie das Messer, mit
eigenem Schaden (gegen Zombies 24, Messer 6). Ein Rohr-Modell gibt es nicht - die Waffendatei der Pipe auf der Disc ist
die Kopie einer Schusswaffen-Datei -, und die Pipe reicht im Original sogar etwas weniger weit als das Messer (1000
gegen 1100). Im Port war sie bisher eine Kopie des Messers (Messer in der Hand, "255" im Inventar, Toene aus der
Pistolen-Bank).

**Was jetzt anders ist (Leon und Elza).** Die Figur haelt ein sechskantiges, metallgraues Rohr, doppelt so lang wie das
Messer, selbst gebaut aus der Messer-Datei; die Bewegungen sind die des Messers. Reichweite 1355 statt 1100 beim Messer
(aus der Rohrlaenge, Ihr Wunsch): Gemessen trifft Leon mit dem Messer bis 2700 Abstand, mit der Pipe bis 3000, Elza bis
2500 bzw. 2800. Keine Zahl in Inventar, Ausruestfeld und Kiste, kein Verbrauch; die Zahl entfaellt auch beim Messer
(sichtbar war sie dort hoechstens in der Kiste). Ton: Treffer = Birkins Rohr-Aufschlag aus STAGE3, Schwung = Birkins
tiefes "Wusch", auch das Ziehen klingt nach Rohr; die Pipe ist lauter als das Messer (Birkins Original-Lautstaerke 127
gegen 70), das Messer klingt unveraendert. Schaden gegen Zombies vorerst wie das Messer (siehe "Noch offen").

**Wie geprueft:** Der Pruefer hat ueber das Menue ausgeruestet und getroffen (Leon in 1140, Elza in 1181, je zwei
Treffer; auf 2800 Abstand trifft die Pipe 2 von 2, das Messer 0 von 2); Riegelgruppe "100% tests passed, 0 tests failed
out of 318", Ton-Gruppe "100% tests passed, 0 tests failed out of 13", der Ton bei Leon und Elza nachgemessen.
**Fundort:** Die Pipe liegt in keinem Raum - weder auf der Disc noch im Mod; in v0.8.30 bekommt man sie nur ueber das
Item-Debug. Ein Fundort im Kesselraum 11F0/11F1 ist fuer v0.8.31 geplant (siehe "Vorschau").

### Optionen: AI MIXED entfernt, neue Option ULTIMATE 1.5

Ihr Wortlaut vom 16.11.: "Ausserdem wuerde ich im Konfigurationsmenu das AI Mixed herausnehmen wollen. RE 2 AI wuerde
ich behalten wollen und bleibt unser Standard mit dem wir arbeiten und RE 1.5 AI koennen wir auch behalten, beinhaltet
die Strunzdumme KI vom Original. Zusaetzlich wuerde ich gerne eine weitere Option hinzufuegen wollen: Ultimate 1.5 zum
ein oder ausschalten. Wenn das eingeschaltet ist, bekomme alle Items im Player Inventory Menu und auch die Sachen die
man in der Welt aufnehmen kann, ihre haesslichen Icons/Models von der unmodifizierten Resident Evil 1.5 Version."
(Memory reai-v2-mixed-ist-das-ziel.md und reai-v2-ultimate15-option.md). Zwei Pruefrunden: Die erste fand einen roten
Punkt (bei AN zeigte die Rote Master-Keycard einen Schluessel), die zweite: "Ich habe keinen roten Punkt gefunden."
(Ordner 44).

**KI-Wahl.** Im KI-Schirm stehen nur noch AI RE1.5 und AI RE2 (Standard). AI MIXED (RE1.5 fuer alle Gegner, RE2 nur fuer
den Hund) ist weg; jeder alte Weg dorthin landet bei RE2. Die KI-Wahl wird wie bisher nicht gespeichert, jeder Start
beginnt mit RE2.

**ULTIMATE 1.5.** Neue Zeile "ULTIMATE 1.5 OFF/ON" neben der KI-Zeile, ab Werk AUS; die Wahl wird sofort gespeichert
(re15_options.cfg). Erreichbar ueber die Optionen im Titel; die Weltmodelle wechseln beim naechsten Raumladen. Bei AN
zeigt das Spiel, was die unveraenderte 40%-Disc an echten Bildern hat: 61 Inventar-Symbole, die Aufheben- und
Untersuchen-Bilder von sechs Gegenstaenden, die Breitbilder und alle Misch-Symbole, dazu vier Fundstuecke mit eigenem
Disc-Modell (Pistolen- und Schrotmunition, Erste-Hilfe-Spray und die Zange - sie ist auf der Disc eine stehende Ampulle
mit der Aufschrift VIRUS und wurde so uebernommen). Zugeordnet ist nach Bildinhalt: Die Rote Master-Keycard behaelt ihre
Karte, der Schluessel der Disc unter ihrer Nummer geht an den Water Key. Wo die Disc nur Platzhalter hat (65
Untersuchen-Bilder mit einer "BOSS"-Karte, 6 Symbole als Kopie der Browning), bleibt das heutige Bild - Ihre Wahl vom
16.11. auf die Rueckfrage: "Nein, nur echte Disc-Bilder". Ebenso bleiben Gegenstaende, die nur ins Hintergrundbild
gemalt sind, und das Statusbild. Bei AUS ist alles pixelgleich wie vorher. Die Disc-Dateien liegen unveraendert in einem
eigenen Ordner (shared_assets/PSX/DISC40); die Mod-Dateien sind nicht ueberschrieben.

**Wie geprueft:** Riegelgruppe (Optionen, Inventar, KI) in beiden Runden "100% tests passed, 0 tests failed out of 182";
bei AUS 0 von 76800 Bildpunkten anders, die Optionsdatei bitgleich.

### STAGE2: ROOM2000/2001 - Kanalwasser ohne Blinken, Deckentropfen aus (Spur W)

Ihr Befund vom 1.11.: "Das blinken im Wasser ist auf jeden Fall fehlerhaft und quatsch, siehe Marker." (Dossier
W_wasserblink.md Z. 5; Ihre Marke: ROOM2000 Kamera 2, Bild F444, Hauptbaum-exe vom Morgen des 1.11.). Drei Abnahmen: die
zweite bestanden (Blinken), die dritte bestanden (Deckentropfen, nach dem Merge nachgeholt).

**Das Blinken ist weg.** Es war ein ausgedachter Port-Effekt aus einem Zwischenstand nach v0.8.29 (sechs feste
Glanzpunkte auf dem Kanalwasser, ohne Vorlage). Kein Skript des Originals - weder im Repo-Baum noch auf der 40%-Disc -
legt Licht auf das Kanalwasser, und im 80%-Video sind die weissen Flecken im Kanal die Spritzer an den Fuessen watender
Zombies; ortsfest blinkt dort nichts. An Ihrer Marke in 19 s: 0 Blitze und 0 unruhige Flecken (Ihre exe vom 1.11.: 64
Blitze und 6 Flecken); in allen 64 Kamera-Laeufen (2000/2001/20B0/20B1, Leon und Elza) ist der Kanal ruhig. Wer v0.8.29
spielt, hat das Blinken nie gesehen. Seit dem 15.11. schimmert das Wasser dagegen leicht wie im Original
(Paletten-Animation, siehe "Monitore flimmern, Wasser schimmert"); das ausgedachte Blinken bleibt weg.

**Was bleibt:** die gelben Flecken im Kanal (der Kanalboden mit den Lampenflecken, der durch die halbdurchsichtige
Wasserflaeche scheint, wie im 80%-Video und unveraendert seit v0.8.29), der Wasserlauf an der Leiter bis zur Flut samt den
drei Gitterstrahlen aus dem Original-Skript, und die Flutszene in 20B0/20B1 unveraendert.

**Deckentropfen (Abnahme 3 bestanden).** Ein einzelner Bildpunkt an der Decke pulsierte (nach etwa 18 s im Raum in drei
Kameras, bei Leon und Elza, auch in Ihrer exe): ein Deckentropfen des Mod-Skripts, den der Port an der Decke haengen
liess, weil ihm die Fallroutine fehlt. Nach Ihrer Regel "Mod-Inhalt nur mit 80%-Beleg" ("Wenn das Birkin-Modell Mod
Arbeit ist, ist das ok, so lange nachweisbar ist, das es auch im 80% Build wirklich so vor kommt, und nicht ausgedacht
ist.", 1.11.) hat der Orchestrator entschieden, die Tropfen-Schleife in 2000/2001 wie auf der 40%-Disc nicht zu starten
(Variante B). Die Abnahme hat den Beleg an den Skript-Bytes beider Discs selbst nachgelesen: Die 40%-Disc startet die
Tropfen nie. Im Spiel (Leon 2000, Elza 2001, mit Raumwechsel hin und zurueck und nach dem Laden eines Spielstands) faellt
kein Tropfen, und am Bildrand ist kein Fleck mehr; Kanalwasser, Wasserlauf und Gitterstrahlen unveraendert. Ob die Tropfen
weg bleiben sollen, fragt die Abnahme Sie - siehe "Zu entscheiden".

### STAGE5 und STAGE2: Monitore flimmern, Wasser schimmert wie im Original (Paletten-Animation)

Ihr Wortlaut vom 15.11.: "Die Monitore im Original laufen uebrigens, also grieseln, bei uns sind die Bilder statisch."
(Memory reai-v2-hangar-szene-monitorbild.md). Ihr Urteil zu Ordner 33 am selben Abend: "super" (Protokoll vom 15.11.).
Zwei Runden: Die erste Pruefung bestaetigte Mechanismus, Ablauf und "sonst nichts veraendert", fiel aber durch, weil das
Elza-Video in Wahrheit Leon in 5030 zeigte; in Runde 2 ist der Elza-Lauf in 5031 echt aufgenommen (Code unveraendert).

**Was das Spiel auf der Disc macht (im Emulator gemessen).** Die Monitorbilder bewegen sich nicht selbst; das Spiel
schaltet alle 4 Spielbilder die Farben der Monitorflaechen weiter und beginnt nach 7 Schritten (28 Bilder, knapp eine
Sekunde) von vorn. Bei den Streifen-Monitoren laeuft ein heller Balken durch, die Rausch-Monitore springen und flackern
dunkel. Eigenheit des Originals, genau so uebernommen: Die Tabelle hat 8 Plaetze, der Raum 11 Monitore - deshalb zeigt
der Monitor unten in der Mitte Rauschfarben statt Streifenfarben, und zwei Monitore (unten links und rechts) bleiben
unsichtbar. Leon (5030) und Elza (5031) gleich. Die Monitor-Objekte gibt es nur im Mod-Baum (auf der 40%-Disc hat 5030
keine); die Technik dahinter ist auf beiden Discs dieselbe.

**Dieselbe Technik bewegt das Wasser.** In STAGE2 (u. a. Kanal 2000, 2020, 2040, 2080, Alligator-Becken 2090, 20A0,
20B0) und in 11A0 und 1260 schimmert das Wasser jetzt leicht (Farbsprung hoechstens 16, alle 4 Bilder). Das ist das
Original-Schimmern (im Original-Speicher gemessen), nicht das ausgedachte Glanzpunkt-Blinken, das Sie am 1.11. abgelehnt
haben (Spur W oben); in v0.8.29 war das Wasser still. Der Kanal-Riegel der Spur W erlaubt jetzt genau diesen
Paletten-Schritt.

**Wie geprueft:** Takt und Folge gegen das Original (alle 4 Bilder, 7 Schritte); 32 Bildpaare im Abstand eines Umlaufs
ohne einen Bildpunkt Unterschied; Riegelgruppen "100% tests passed, 0 tests failed out of 186" (Runde 1) und "100% tests
passed, 0 tests failed out of 7" (Runde 2); Gegenproben mit eingebauten Fehlern rot.

### STAGE2: Nachtrag 30 - Schrank 2010, Panel-Raetsel 2030, Notiz und Plakat, Plattform-Ton, Sirene, Lautstaerke, Wasser an der Leiter (Spur N)

Ihr Nachtrag 30 vom 25.10. (in v0.8.29 angehalten, jetzt gebaut): "In ROOM 2010, beim Anklicken des Schranks, mache das
er aufklappt und dann so aussieht wie in 2060 der Fuse Kasten - siehe fuse_box.bmp. Ab da soll auch der Mechanismus
gleich - das Einsetzen der Sicherung etc. Dann - in ROOM 2030 am Kasten, damit die Platform runter faehrt, erstelle ein
Raetsel, das so aussieht wie das Panel in Nahansicht, anstatt das komische - dreh an Valve 13. Dann - beim Text soll nur
drin stehen das gruen und rot verwechselt wurde, nicht black, das soll raus. Bei der verwechselten Nummer soll drin
stehen das sie ausserdem die Nummern verwechselt haben von "Schwarz" und "Gruen". Hier dienen die Anfuehrungszeichen als
Ironie fuer die falschen Farben auf den Poster. Natuerlich auf englisch. Und wenn die Plattform runter faehrt, fehlt mir
noch der Original Sound von Resident Evil 2. Und waehrend des Alerts und der Alert message fehlt mir noch zusaetzlich
durchweg der Alert sound aus Resident Evil 2." und "Und bei der reduzierten Volume sollen ALLE Volumes reduziert werden,
nicht nur ein paar." (AUFTRAG.md Z. 947-964). Dazu Ihre Wahl des Plakats in Nachtrag 30d ("Plakat NEU: 1. GREEN 01 - 2.
RED 02 - 3. BLACK 09 - 4. RED 08 -> Loesung 09 -> 02 -> 01 -> 08", Z. 1008), Nachtrag 30e: "und eine Sache fehlt mir
noch - in room 2010 soll - bis zum ablassen der Wassers, auch ein Wasser Effekt die Leiter runter kommen und ueber den
Weg bis zum Wasser Pool in der Mitte laufen." mit Ihrer Wahl "Bis zur Flut in 2030 (Empfohlen)" (Z. 1013-1015), Nachtrag
36: "bei der Minute bis zum Wasser voll laufen in STAGE2 solltest du ebenfalls die Alarm Sirene mit einbauen nach der
Ansage. Die findest du zum Beispiel auch bei room6000." (Z. 1143-1144), Ihre Freigabe des Panel-Entwurfs in Nachtrag 37
("A Druckspeicher (Empfohlen)", Z. 1153) und zur Lautstaerke in Nachtrag 34: "Betreff der Lautstaerke, die Option
scheint doch zu funktionieren, aber selbst mit Lautstaerke bei 10 ist es immer noch zu laut. Da muss wahrscheinlich
doppelt so viel gedreht werden und Standard sollte dann 50 sein." (Z. 1091-1093). Abnahme 1 fand im Spurbaum alle sieben
Punkte erfuellt, bestand aber nicht, weil das Zusammenspiel mit den Key Items der Spur O (USE der Sicherung am
geschlossenen Schrank verpuffte) im Merge nicht gefangen war; beides ist in der Integration behoben und mit Riegeln
belegt (siehe "Technisch"). Leon und Elza (2011, 2031, 2001) gleich gemessen.

**Schrank in ROOM2010/2011.** Die Aktionstaste am Schaltschrank schaltet die Kamera auf die Schrank-Kamera, die Tuer
klappt in 16 Bildern um 90 Grad auf (Takt des Original-Hebels in 11F0), und innen steht der Sicherungskasten aus
ROOM2060: Ihr fuse_box.bmp ist der Blick von Kamera 6 in 2060, Innenflaeche und Tuer-Innenseite sind aus diesem Bild auf
die Rueckwand bzw. die Tuerebene gerechnet, die Aussenseite aus dem Hintergrund von 2010, darum deckt sich die
geschlossene Tuer mit dem Hintergrund (geschlossen ist der Kasten bitgleich der Original-Hintergrund). Die Aktionstaste
allein zeigt danach nur "A fuse box for the valve controls." / "One of the fuses is missing..." - eingesetzt wird ueber
das Inventar (Spur O): vor dem Schrank START, ITEM, Sicherung, USE; das Inventar schliesst, der Schrank klappt auf (ist
er offen, entfaellt das), dann wie in 2060: Nahansicht der Sicherungen, Cursor mit dem Steuerkreuz auf die Luecke in der
dritten Reihe (ein Schritt nach rechts reicht), Aktionstaste. Nicht belegt: Ob es in dieser Nahansicht wie im Original keinen Abbruch gibt, ist in keiner Quelle belegt; belegt ist nur der Abbruch mit Kreuz am Pult 2050 und im Panel 2030.,
Nahansicht mit eingesetzter Sicherung und gruener Lampe, "You've used the Fuse.", dann das Standbild der Ventile mit dem
Aufschliess-Ton aus v0.8.29, zuletzt zurueck zur alten Kamera. Der Cursor-Schritt ist Bild fuer Bild der von 2060.
Steckt die Sicherung schon oder sitzt sie im Generator, gilt USE dort nicht: "You can't use it here.", das Inventar
bleibt offen. Die offene Tuer ist nur unter Kamera 3 zu sehen (unter Kamera 4 stuende der Fluegel direkt vor der Kamera,
dafuer gibt es kein Bild - Port-Wahl); beim naechsten Betreten ist sie wieder zu. Herausnehmen und der Rest des
Mechanismus sind unveraendert aus v0.8.29.

**Panel-Raetsel in ROOM2030/2031 statt Ventil 13 (Ihr Entwurf A "Druckspeicher").** Am Pult: "Will you operate the
panel?" (seit Spur PF auf zwei Seiten, siehe dort), Ja, und das Panel fuellt das Bild: drei Tanks A (fasst 100), B (50),
C (30), die Anzeige liest Tank A und steht bei 100, rote Marke bei 80. Steuerkreuz bewegt den Cursor, die Aktionstaste
waehlt die Quelle (gruene Lampe), die Aktionstaste auf einem zweiten Tank fuellt um - es fliesst, bis die Quelle leer
oder das Ziel voll ist ("The tank is empty." / "The tank is full."), jede Fahrt dauert 16 Spielbilder wie der
Original-Hebel in 11F0, mit den Toenen des RE2-Panels aus ROOM2130. Loesung: A nach B (50/50/0), B nach C (50/20/30), C
nach A (80/20/0) - die Nadel muss von 100 auf 80 herunter, und beim Probieren bleiben in B genau 20 zurueck. Bei 80
wartet das Panel kurz, dann folgt "The pressure gauge reads 80. / Lower the bridge?", mit Ja faehrt die Bruecke in der
Draufsicht herunter, "The bridge is down." EXIT oder die Abbruchtaste verlassen das Panel, beim naechsten Oeffnen steht
es wieder auf 100/0/0; es gibt keine Sackgasse (alle 16 Staende durchgesucht, von jedem hoechstens 4 Zuege bis 80).
START oeffnet im Panel kein Inventar. Ventil 13 am Tank ist weg. Rahmen und EXIT-Feld sind pixelgleich aus den
Original-Nahansichten 2050 Kamera 15 und 11F0 Kamera 10, der Rest ist nach dem entzerrten Echtbild des Pults neu
gezeichnet (Port-Wahl). Nach der Loesung laeuft die Kette wie in v0.8.29 weiter (Sued-Landeplatz, Ventile, Flut).

**Notiz und Plakat.** Die "Technician's Note" sagt jetzt auf Seite 1 und 2: "They mixed up / green and red, and on top
of" / "that they swapped the / numbers of "black" and / "green"! What are they? / Colorblind? How is anyone / supposed
to work with / something like this...." (77 Woerter wortgleich am Artefakt gelesen; die Anfuehrungszeichen sind in der
RE2-Dokumentschrift so gesetzt, wie RE2 sie selbst setzt). Das Plakat an der Suedwand zeigt "1. GREEN 01 - 2. RED 02 -
3. BLACK 09 - 4. RED 08". Rechenweg: Zeile 1 GREEN heisst rot, Nummer aus der BLACK-Zeile 09 = 09 rot; Zeile 2 RED
heisst gruen, 02 gruen; Zeile 3 BLACK bleibt, Nummer aus der GREEN-Zeile 01 = 01 schwarz; Zeile 4 RED heisst gruen, 08
gruen. Richtig ist also 09 -> 02 -> 01 -> 08, und jede Zeile trifft ein echtes Rad; die alte Folge 01/02/09/08 setzt
zurueck.

**Plattform-Ton.** Wenn die Bruecke faehrt, laeuft der Motor der RE2-Mittelbruecke aus ROOM4080 bis zum Halt, und der
Halt-Ton schneidet ihn wie in RE2 (Toene bytegleich aus der RE2-Raumdatei).

**Sirene in der Flut-Minute.** Nach der Ansage ("WARNING ...", Warntext samt Ihrer Aufnahme) heult zusaetzlich zum
RE2-Alarm die Sirene aus ROOM6000: RE1.5 spielt dort ein Stueck, das bytegleich RE2s MAIN11 ist; die Sirene darin (0,80 s
an, 0,14 s aus) liegt in RE2 auch ohne Musik vor (SUB_34, Schleife 14,3 s), und die spielt der Port. Sie laeuft
durchgehend bis zur Flut, bis zum Tod oder bis zum Verlassen des Raums, bei Leon und Elza, und endet im Bild des
Flut-Beginns.

**Lautstaerke (Nachtrag 34, Entscheid N-E2).** Der Regler wirkt jetzt quadratisch: 100 = voll, 50 = ein Viertel, 30
entspricht dem alten 10, 10 = ein Hundertstel; ohne Options-Datei gilt 50. Eine vorhandene Options-Datei behaelt ihren
Wert (20 heisst jetzt 0,04). Gemessen am ganzen Mischweg: jeder einzelne Abtastwert folgt der Einstellung, Tuersequenz
und Sprung ins Wasser eingeschlossen (2 499 000 Werte, 0 falsch) - es gibt nur einen Ausgabeweg, Ihr Eindruck aus
Nachtrag 30b ("Die Tuersequenzen und Sounds der Reinsprung SOunds ins wasser etc, scheint davon nicht betroffen.",
AUFTRAG.md Z. 987-988) liess sich an der exe nicht bestaetigen.

**Wasser an der Leiter (Nachtrag 30e) - gebaut in ROOM2000/2001, nicht in 2010.** ROOM2010/2011 ist das Buero mit
Schreibmaschine und Computer; es hat keine Leiter, keinen Weg und kein Becken. Ihr Wortlaut ("die Leiter runter kommen und
ueber den Weg bis zum Wasser Pool in der Mitte laufen") trifft Wort fuer Wort den Kanalraum 2000/2001 (Leiter nach 2050,
West-Steg, Kanal in der Mitte) - und Ihr eigener Entwurf vom 23.10. nennt genau diese Leiter. Entscheid des Orchestrators:
dort gebaut. Bis zur Flut in 2030 stroemt Wasser ueber die ganze Leiter (sechs Dauerstrahlen vom Gitter-Effekt des
Raumskripts), spritzt von der Leiter bis zur Kante, laeuft sichtbar fliessend ueber den Steg (derselbe Effekt wie die
Flutwelle in 20B0) und schlaegt im Kanal ein; die Leiter bleibt gesperrt wie bisher. Nach der Flut ist es aus. Elza in
2001 gleich.

### STAGE2: ROOM2010/2030 - Sicherungskasten entzerrt, Panel in der Spielschrift, Frage ohne Ueberlappung (Spur PF)

Ihr Befund vom 2.11. (00:26): "Der Kasten mit der Sicherung in ROOM 2010 sieht schon klasse aus, aber das Bild ist / der
Kasten ist bei der Kamerasicht irgendwie komisch gestaucht.... das ist noch falsch. Auch das Raetsel in 2030 das du
gebaut hast ist Klasse! Aber Zahlen und Texte auf den Panel in Nahansicht sind kaum lesbar/verschwommen. Und der Text am
Anfang vor dem Start des Raetsels ueberlappt mit dem yes / no Auswahl." (Dossier PF_panelfix.md Z. 5-7). Nach dem ersten
Bau (09:24): "Das Panel ist immer noch schwer lesbar/verpixelt. Vor allem das A 100, und das 80 links, das Dreh panel,
der BRIDGE und PRESSURE 80 Text, der B 50 und der C30 Text und auch der Caution High Pressure Text." (PF_abnahme_1.md Z.
14-15; Abnahme 1 nicht bestanden). Zur Schrift am Vergleichsbild vom 3.11.: "ja, ist super." (Dossier Z. 180). Danach
Nachbesserung 1 (Kasten-Scherung, Drehscheiben) und Abnahme 2 bestanden. Leon und Elza ueberall bildgleich.

**Sicherungskasten ROOM2010/2011.** Geschlossen ist der Kasten unveraendert der Original-Hintergrund. Nach der
Aktionstaste klappt die Tuer auf, und der Inhalt (Rueckwand mit den Sicherungen, offener Fluegel mit zwei Schildern) hat
jetzt dieselben Proportionen wie der Fuse-Kasten in 2060 - vorher war er waagrecht um 21 % (Rueckwand) bzw. 29 %
(Fluegel) gestaucht, und ein Knick lief quer durch den Kasten. Jedes Bildteilchen wird jetzt ueber die Kamera gerechnet,
mit der das Original den 2060-Kasten zeigt, und unter Kamera 3 in 2010 gezeigt: Sicherungsroehren senkrecht, Rahmen und
Schilder waagrecht, die Schilder Rechtecke statt Parallelogramme. Dafuer fehlt links und rechts ein schmaler Rand des
Kastens - die Tuer in 2010 ist schmaler als der Kasten in 2060 - und die Tuer zeigt etwas mehr von ihrem unteren Teil.
Keine Naht, kein Versatz, keine andere Helligkeit; gleich in jeder Fenstergroesse, am Kasten auch im Vollbild gemessen.
Die Nahansichten beim Einsetzen der Sicherung bleiben die Original-Hintergruende aus 2060.

**Pult und Panel ROOM2030/2031.** Die Frage am Pult kommt jetzt auf zwei Seiten: "A control panel for the bridge." mit
Seitenpfeil, dann "Will you operate the panel?" allein auf der ersten Zeile, darunter frei "Yes   No" mit dem Cursor -
ein Tastendruck mehr, wie bei den Original-Fragen mit Erklaerung (z. B. am Kasten in 2060). Vorher lief die zweite Zeile
in die Auswahl ("Will you operate the Yesel?   No"). Das Panel in Nahansicht zeigt die Beschriftung jetzt in der kleinen
Pixelschrift des Spiels: A 100, B 50, C 30, die rote 80 am Tank A, BRIDGE / SET 80, CAUTION / HIGH PRESSURE - alle acht
von Ihnen genannten Texte werden Zeichen fuer Zeichen erkannt (42 von 42), vor und nach der Loesung. Das Schild oben
rechts heisst "BRIDGE / SET 80" statt "PRESSURE 80" (das Feld ist fuer "PRESSURE 80" in dieser Schrift zu schmal), das
rote Schild ist breiter, die winzigen Zahlen im runden Zeiger-Instrument sind weg - das rote Feld 80 bis 100 und die
rote 80 am Tank zeigen das Ziel. Drehscheiben, Zeiger-Instrument und Zeiger sind ganze, scharfe Pixel ohne graue
Verlaeufe; der Zeiger flimmert beim Umfuellen nicht mehr. Der Querbalken der Drehscheiben steht etwas flacher als vorher
(siehe "Zu entscheiden"). Die Nachrichten im Panel stehen auf einem dunkleren Band, damit die breitere Schrift des roten
Schilds nicht mehr sichtbar ueber dem Text liegt (ein schwaches Durchschimmern bleibt, der Text ist lesbar). Der Ablauf
des Raetsels ist unveraendert (Spur N, oben). Im Standardfenster ist jeder Bildpunkt des Panels ein 3x3-Block ohne
Filter - die Pixelschrift des Spiels, kein Weichzeichner. Nicht belegt: Das Panel wurde im Vollbild nicht gemessen.

### STAGE2: Alligator - Kampfbeginn, Schluessel, Putz, Finisher, Leiche und Kanister im Maul (Spur P)

Ihr Nachtrag 35 vom 30.10.: "Bei den Aligator fight gab es noch einige findings: - Zum einen kam der Putz immer noch
nicht so runter wie beim Aligator fight in Resident Evil 2, das heisst der Teil funktioniert da noch nicht 1 zu 1 - Nach
dem Aligator Tot fand man den Schluessel nicht, ich weiss nicht, ob der In der Wand haengen geblieben ist oder so, aber
ich kam auf jeden Fall nicht ran - Ich konnte beim Aligator nach seinen Tot drauf klettern - Der Spieler ist beim Tod
durch den Aligator in seinen Finisher noch halb haengend in der Luft sichtbar, nicht wie in Resident Evil 2. Er muss ja
irgendwann mal verschwinden, das es so aussieht als ob er wirklich gefressen wird. - Bei der Zombie Leiche die der
Aligator am Anfang Frisst ist es noch schlimmer. Der Haengt nicht mal sauber im Maul und bewegt sich dynamisch mit,
sondern haengt darunter.... - Der Kanister haengt auch nicht gerade im Maul wie bei Resident Evil 2 und der Aligator
kann sich mit Kanister danach offenbar auch nicht mehr richtig bewegen....." (AUFTRAG.md Z. 1116-1122). Dazu aus v0.8.29
"Ab Samstag" Ihre Antwort in Nachtrag 31 Punkt 2, "Aendern: nicht vor der Kamera" (Z. 1040), und die Lagespruenge mit
dem Kanister im Maul. Abnahme 1 bestanden, keine Maengel, sechs Hinweise; Leon und Elza gemessen (ROOM2091 ist
bytegleich ROOM2090).

**Kampfbeginn nicht mehr vor der Kamera.** Die Zombie-Leiche treibt weiter vorn im Becken; nach dem Fressen und dem Schrei
schwimmt der Alligator von rechts durch die Beckenmitte auf Leon bzw. Elza zu, statt rund 100 Bilder lang gross die rechte
Bildhaelfte zu fuellen (rechte Bildhaelfte im Mittel 26 % statt 45 %, kein Bild ueber 50 %). Abstand beim Kampfbeginn
und erster Biss liegen im RE2-Band aus v0.8.29. Im ersten Bild ragen Kopf und Hals von rechts ins Becken, der Rumpf folgt
nach gut 100 Bildern: mehr lassen das RE2-Band, die Kamerazone und Kamera 0 nicht zu. Der blinkende Schluessel auf der
Leiche ist in der Ankunft rund 8 x 7 Bildpunkte gross; mindestens so hell wie die abgenommene Fassung (Ihre Antwort in
Nachtrag 31 Punkt 3: "Ja, gut genug").

**Der Water Key ist nach jedem Tod erreichbar (Nachtrag 35 Punkt 1, Pflicht fuer das Release).** Vorher lag er bei 202
von 1688 gerechneten Todeslagen im Maschinenblock - das war Ihr "in der Wand haengen geblieben". Jetzt liegt er immer
auf einem Punkt, den die Figur wirklich erreicht, und die Kamera der Einstellung nach dem Tod meidet Stellen, an denen
Leon ihn verdeckt. Die Mindestgroesse von 10 Bildpunkten, die in der Spur P dazu galt, gilt seit Ihren Befunden vom
9.11. nicht mehr: Der Schluessel liegt jetzt naeher an der Leiche und ist dafuer kleiner (siehe den Abschnitt zu den
Nutzerbefunden 9.11.). Ihr "drauf klettern": Der Schluessel lag auf der Kletter-Ebene und war damit fuer die
Aktionstaste eine Stufe; jetzt liegt er wie die schwimmenden Objekte des Raums auf der Wasser-Ebene, die Aktionstaste
hebt ihn auf. Gemessen in der Spur P (vor Ihren Befunden vom 9.11.; die Erreichbarkeit ist danach nicht neu ausgezaehlt,
der Riegel dazu lief auf dem neuen Stand in der Alligator-Gruppe gruen): 1688 Todeslagen und 4,7 Millionen Stellungen,
nicht erreicht 0, Klettern 0; dazu natuerliche Kaempfe bis zum Aufheben, Leon und Elza, auch mit der Leiche zwischen
Figur und Schluessel.

**Die Leiche des Alligators hat wie in RE2 keine Kollision mehr.** Man laeuft durch sie hindurch, sie sperrt den Westkanal
nicht mehr (vorher stand Elza bei der Leiche fest), und Leon zielt nicht mehr auf sie.

**Putz wie in RE2.** Er faellt jetzt bei jedem starken Stoss des Alligators, an die Stelle gebunden, an der RE2 ihn
ausloest (das Schuetteln): fuenf Brocken mit Staubspur, zufaellig in der Beckenseite der laufenden Kamera, mit dem
RE2-Geraeusch (bytegleich aus der RE2-Raumdatei). Der alte Staub ueber Leon beim Dash war der falsche Effekt: In RE2
gehoert er zum Rueckzug des Alligators nach Beschuss, den es hier nicht gibt. Der Ort je Beckenseite ist Port-Wahl (RE2
hat drei Beine, 2090 einen Ring aus vier Seiten); ein Bildvergleich mit dem laufenden RE2 war nicht moeglich (kein Abbild
auf der Maschine), der Vergleich steht auf den Skript- und Code-Bytes.

**Finisher: Die Figur verschwindet.** Stirbt Leon oder Elza durch den Alligator, ist die Figur mit dem Ende des
Schuettelns weg (RE2 setzt dort ein Zeichen-Bit), danach kaut der Alligator allein; auch Elzas Zopf ist weg.

**Fress-Szene am Anfang.** Die Zombie-Leiche liegt quer im Maul, Rumpfmitte zwischen den Kiefern, die Enden ragen seitlich
heraus; sie folgt dem Kopf Bild fuer Bild durch das ganze Schuetteln und ist mit dem Ende des Schuettelns weg.

**Kanister im Maul.** Er liegt quer im Maul wie in RE2 und ragt nach einer Seite heraus, aus welcher Richtung der
Alligator auch kommt. Mit dem Kanister im Maul springt der Alligator nicht mehr: Der Bildschritt ist auf den groessten
echten Schritt seiner Bewegungen begrenzt, an der Wand weicht er sichtbar zurueck, dabei kann der Kopf kurz in die Wand
ragen. In 306 Trage-Laeufen (275 400 Bilder) kein Sprung. Weil der Kanister in RE2-Groesse am kleineren Alligator sitzt,
ragt er 1,5-mal so weit zur Seite wie in RE2 (siehe "Noch offen").

**Ausserdem:** Die Uebergabe von der Ankunftsszene (2090) zum Knall mit den acht Zombies in 2080 ist jetzt ueber den
echten Weg als Riegel belegt (Leon und Elza), nicht mehr nur von Hand.

### Restpunkte aus v0.8.29: toter Alligator bleibt tot, ausgelieferter Raum, Treppenwange 6010/6011, Effekt-Quader 20B0 (Spur Q)

Fuenf Punkte des Orchestrators aus den offenen Listen von v0.8.29, ohne eigenen Nutzer-Wortlaut. Abnahme 1 bestanden,
keine Maengel.

**Der tote Alligator bleibt tot.** Vorher war er beim naechsten Betreten von 2090 mit voller Energie wieder da und ohne
zweiten Kanister nicht mehr zu toeten. Jetzt bleibt er tot - nach dem Verlassen des Raums, ueber Spielstand laden und
nach dem Aufheben des Water Key, Leon und Elza. RE2 sperrt seinen Alligator schon beim Kampfbeginn; der Port setzt die
Sperre erst beim Tod (Port-Wahl), weil der Alligator hier den Water Key traegt: Wer vor seinem Tod ueber die Leiter
flieht, trifft ihn beim Zurueckkommen wieder, sonst waere der Schluessel weg.

**Ausgelieferter Raum.** Mit dem Schalter fuer den ausgelieferten Raum (RE1.5-Alligator, keine Port-Szenen; siehe
"Technisch") zeigt 2090 jetzt auch die zwei Spinnen des ausgelieferten Raums, die der Port dort bisher verwarf.

**Treppenwange ROOM6010/6011 Kamera 5.** Das Restloch aus v0.8.29 (Kopf und Schulter ragten unter der Treppe ueber die
Wange) ist zu: Die Tiefe des Teils kam aus einer Kollisionszelle unter den Stufen, die Wange selbst liegt deutlich naeher
an der Kamera. An allen gemessenen Stellen, Leon und Elza, ist kein Figurpunkt auf dem Teil sichtbar, auch auf dem echten
Weg ueber die Treppe. Im Spiel steht an diesen Plaetzen nie Kamera 5, die Aenderung sieht man nur mit erzwungener Kamera.

**Ihre Freistellungs-Liste STAGE3-6** (Dossier M, "Fuer den Nutzer") ist vollstaendig: Die acht kleinen Stellen aus
v0.8.29 sind eingetragen, je mit Restbild; der Teile-Bericht stammt jetzt aus dem ausgelieferten Lauf; ROOM4040/4041
Kamera 12 hat keine Stelle hinter dem Torrahmen (jenseits des Tors gibt es keinen Stehplatz).

**Effekt-Quader in 20B0/20B1 und die stillen Testenden.** Die stillen Enden des Ketten-Riegels aus v0.8.29 waren
Abstuerze: Der Zeichner rechnet bei riesigen Dreiecken ueber (die Effekt-Quader des Wassers kurz vor der Kamera nach der
Flutszene). Der Port klemmt die Ecken jetzt wie die PSX-Hardware (Polygone ueber 1023 x 511 Bildpunkte zeichnet die
Hardware nicht) - auch im Spiel fuer die Effekt-Quader: Die schirmfuellenden grau-gruenen Bloecke im Westgang nach der
Flut (bis 90 % des Bildes) sind weg, Quader unter der Grenze bleiben im Bild. Fuer die Elza-Kette 2031 -> 20B1 -> 2051
gibt es jetzt einen eigenen Riegel. Kein Ende der exe ist mehr still: Ein Absturz schreibt Adresse und Stapel ins
debug.log.

### STAGE2: Alligator Nachbesserungen - oben nicht sicher, kein Haengen, Trefferreaktion und Biss wie RE2 (Spur G2)

Nach v0.8.29 und der Spur P meldeten Sie weitere Alligator-Befunde; die Spur G2 hat sie auf dem gelobten Stand vom 6.11.
nachgebessert. Neun Abnahmen; die neunte (8.11.) ist bestanden, seit dem 8.11. auf master. Unterwegs fielen unter
anderem durch: die Abnahme 5 (Absturz und ein Wurf der Figur aus dem Raum), die Abnahme 7 (ein Biss traf im selben
Schnappen ein zweites Mal) und die Abnahme 8 (die Sperre dagegen stand auf einem RE2-Beleg, der in RE2 den Spurt sperrt,
nicht den Biss).

**Ihre Entscheide vom 6.11. (woertlich, als Vorgaben der Nachbesserung 3):** Zur Frage F1 (Kameragrenze fuer den Kopf,
wenn der Alligator den Kanister holt): "noe, wozu wenn es gut funktioniert?" - keine Kameragrenze. Zu F3: "Er soll nur den
Spieler oben auf der Rampe auch rammen koennen, das der Spieler oben nicht ausweichen kann. Alternativ kann er auch hoch
beissen oder sowas, aber er darf halt oben nicht sicher sein." (zum Takt: "Das kann er gerne machen."). Zu F4: "Wenn er
haengt muessen wir nachbessern denke ich. Er darf nicht zu lange haengen. Wenn es kurz ist, kann man mit dem tauchen
abhilfe schaffen, aber das hilft natuerlich nicht bei der Platform." (G2_abnahme_6.md Z. 13-17).

**Nachbesserung 3 - bestanden (Abnahme 6, 7.11.), Leon und Elza, mit und ohne Kanister.** Oben auf der Rampe ist die
Figur nicht mehr sicher: In jeder gemessenen Lage (zehn eigene Lagen der Abnahme in allen Blickrichtungen, stehend und
gehend, dazu die Lagen der frueheren Abnahmen) wird sie gerammt, hochgebissen oder von der Querung gebissen, jeder
Treffer 50 Schaden, 18 bis 25 Treffer in den ersten zwei Minuten nach Kampfbeginn; der erste Treffer kommt spaetestens
etwa 13 s nach dem Kampfbeginn bzw. nach dem Hinaufsteigen; in den Fensterlaeufen keine Pause ueber 15 s (hoechstens 447
Bilder), im Harness bis 492 Bilder, Schranke der Vorgabe 600 Bilder. Einzige Ausnahme war das Kanister-Holen zu
Kampfbeginn (knapp 28 s ohne Treffer, solange er den Kanister aus dem Becken holt) - mit Nachbesserung 4 weg, siehe
unten. An der Plattform- und Rampen-Suedkante haengt er nicht mehr: Haenge-Fenster 0, laengste Zeit ohne Fortschritt 326
Bilder (Grenze 450 = 15 s, Port-Wahl nach Ihrem "nicht zu lange"), kein Rueckfall; an denselben Lagen beisst er jetzt 20
bis 32 Mal statt 13 bis 23 Mal mit bis zu 1123 Bildern Pause im gelobten Stand. Der Absturz und der Wurf der Figur durch
die Suedwand aus der Abnahme 5 sind behoben (46 eigene Laeufe: kein Bild mit der Figur ausserhalb von Deck oder Raum;
ein "Raumhalt" greift nur in einer Trefferreaktion, nie beim Gehen, Klettern oder Absteigen). Ein Biss waehrend des
Hochkletterns schob die Figur danach aber noch hinter die Ostwand (Ihr Befund 1 vom 9.11.; an diesem Stand nachgestellt
in drei Laeufen mit je 1149 Bildern in fester Wand; die bisherigen Riegel sahen den Fehler nicht) - behoben, siehe den
Abschnitt zu den Nutzerbefunden 9.11. Ausserhalb der beiden Orte (oben auf der Rampe, Wasser nahe einer Blockkante) ist
der Kampf bitgleich zum gelobten Stand. Riegel: 28/28 Unit, 20/20 exe-Riegel gruen.

**Nachbesserung 4 in drei Runden - bestanden (Abnahme 9, 8.11.).** Ihre Antworten vom 7.11. (woertlich,
G2_MESSBLATT_NB4.md Z. 11-15): "er kann den Spieler ruhig In das Wasser stossen." - ein Treffer oben an der Rampenkante
darf die Figur ins Wasser stossen (vorher taumelte sie nur an der Kante, Frage F3 der Abnahme 6). Und zum Kanister-Holen:
"Die Frage stellt sich garnicht, da leon mit den alligator ganz normal seine runden drehen soll. Das heisst der alligator
sollte Ganz natuerlich irgendwann zum Kanister kommen, waehrend er leo. Auf dem rundweg verfolgt....". Dazu der Entscheid
des Orchestrators ("Original oder nicht"): Fuer jeden Alligator-Angriff, der die Figur trifft (Biss im Wasser, Biss von
der Querung, Rammen, Hochbiss, Biss mit Kanister), gilt die Trefferreaktion der Figur aus RE2 statt des RE1.5-Niederwurfs,
der die Figur bis zu gut 13 000 Einheiten weit warf (Frage F2 der Abnahme 6).

**Was Sie jetzt sehen (Leon und Elza).** Jeder Treffer laeuft als RE2-Trefferreaktion; in ueber 70 000 gemessenen
Bildern kam kein RE1.5-Niederwurf mehr vor. Rammt der Alligator die Figur oben an der Rampenkante, faellt sie ins
Wasser; beisst er von unten hinauf, taumelt sie und bleibt oben (siehe "Zu entscheiden"). Einen eigenen
Kanister-Holmodus gibt es nicht mehr: Er nimmt den Kanister nur auf, wenn sein Kopf auf dem Verfolgungsweg in Reichweite
kommt - seit den Nutzerbefunden vom 9.11. auch mitten im Biss und im Spurt (Abschnitt weiter unten); bleibt die Figur an
der Rampe, lag der Kanister 3300 bis 4400 Bilder (Stand vor den Nutzerbefunden 9.11. mit der alten Kanisterlage, nicht
neu gemessen), und die 28-s-Pause beim Kanister-Holen ist weg (erster Treffer oben nach 300 Bildern). Jeder Biss-Anlauf
trifft hoechstens einmal, wie in RE2: Ein einzelnes Schnappen toetet die Figur nicht mehr in 20 Bildern aus voller
Gesundheit (Abnahme 7 hatte 400 von 895 Treffern als Zweitbiss gemessen; jetzt 0 Zweitbisse in 523 Treffern des
Testprogramms und in allen Spiellaeufen). Nach dem Intro beisst er im ersten Anlauf, sobald sein Maul an der Figur ist,
wie in RE2 (gemessen Bild 290; vorher zwei leere Anlaeufe bis Bild 416). Mit echten Trefferpunkten: Biss bei Bild 942
(100 auf 40), der naechste Biss 107 Bilder spaeter - Leon und Elza gleich. Riegel: 48/48 Unit, die exe-Riegel der
Abnahme 5/5, in der Integration 20/20.

### STAGE2: Alligator - Klettern, Kanister in der Kanalmitte, Schluessel bei der Leiche (Nutzerbefunde 9.11.)

Nach Ihrem Spieltest vom 9.11. meldeten Sie (woertlich): "Den Aligator fight habe ich jetzt auch einmal getestet. Ist
cool! Es gibt nur 3 winzige kleinigkeiten die mir aufgefallen sind: 1. Wenn ich direkt an der Kante hochklettere auf die
Platform und dann vom Aligator erwischt werden, ist es moeglich das er mich in die Wand schiebt. 2. Ich musste diverse
runden mit dem Aligator drehen, bis er letzendlich den Behaelter aufnahm. Eigentlich sollte er den Behaelter aufnehmen,
sobald die Position erreicht ist, wo der Behaelter liegt. Wenn das so nicht so einfach ist, lege den Behaelter zentraler
in die Mitte, damit er ihn auf jeden Fall aufnehmen muss, sobald der Spieler den Aligator dort hingelockt hat. 3. Ich
bin ausversehen nach dem Tot des Aligators die Leiter hoch und wieder runter. Ich konnte den Schluessel sehen, aber
aufnehmen konnte ich ihn nicht mehr." (INT_gator_nutzerbefunde.md Z. 6-14). Zur Lage des Schluessels schrieben Sie
danach, ebenfalls am 9.11.: "na er soll naeher bei der Leiche liegen, immerhin hat der Aligator ja den Schluessel samt
Zombie gefressen." (ebd. Z. 298).

Vier unabhaengige Abnahmen am 9. und 10.11. Die Abnahme 0 (9.11.) fiel durch: Im echten Kampf zeigte die Einstellung
nach dem Tod den Schluessel weiter in einer Kamera, die an seinem Platz nicht laeuft (4 von 4 Laeufen des Pruefers), und
die Kanister-Aufnahme brach einen laufenden Biss ab, was RE2 nicht tut. Die Abnahme 1 (9.11.) ist bestanden. Danach
kamen drei Nachtraege: Kanister in die Kanalmitte, Schluessel naeher an die Leiche und ein weicher statt harter
Haltungswechsel, wenn der Kanister einen Spurt abbricht. Die Abnahme 2 (10.11.) fiel durch: In einer Todeslage (der
Todesrolle) lag der Schluessel nach dem Nachtrag hinter der Leiche, zu sehen war genau ein Bildpunkt; der zustaendige
Riegel war rot und nach dem Nachtrag nicht gefahren worden, und bei den Spiel-Riegeln, die das Bild der Einstellung
pruefen (zwei Pruefdateien mit mehreren Laeufen), stand die Bildschranke seit dem Nachtrag auf 0 x 0 statt 10 x 9,
sodass sie den einen Bildpunkt durchliessen. Die Abnahme 3 (10.11.) ist bestanden.

Der Stand ist in v0.8.30 (Zweig r41/integration 799594460, gemergt mit 762ebfda4). "Alter Stand" heisst im Folgenden:
der Stand vor diesen Befunden (master vom 9.11., 0b86e36c6).

Der gelobte Kampf ist ausserhalb dieser Punkte unveraendert: Stellt man mit zwei Messhaken die alte Wurfrichtung und den
harten Haltungswechsel wieder her, sind 9 von 9 Laeufen der Abnahme 3 ueber die volle Laenge in den Zustandsprotokollen
bitgleich mit dem Stand der Abnahme 1 (Vergleich der Protokolle, kein Bildvergleich).

**Ein Biss beim Hochklettern schiebt die Figur nicht mehr in die Wand.** Bisher lief das Klettern nach dem Treffer
weiter und trug die Figur laengs der Trefferrichtung hinter die Ostwand - so endete auch Ihre Sitzung. Jetzt beendet der
Treffer das Klettern: Nach einem Biss am Kletteransatz bleibt die Figur im Wasser und wird dort von der Trefferreaktion
geschoben; hinaufgetragen wird sie nicht mehr. Ins Wasser stossen kann der Alligator sie also weiterhin, auch oben von
der Kante des Decks. Beleg: Im Original ersetzt der Treffer die laufende Spieler-Routine - so in RE2 beim Treffer des
Alligators (@0x801015b4) und ebenso in RE1.5 (Stelle unter "Technisch"). Keine neue Zahl. Gemessen: In der Abnahme 1
steht die Figur in fuenf von fuenf Laeufen mit Biss waehrend des Kletterns 60 Bilder (2 s) nach dem Biss im Wasser. Im
Riegel liegen 0 von 612 eingespeisten Kletter-Lagen in oder hinter der Wand (alter Stand, im Riegel ueber einen
Pruefhaken nachgestellt: 248 von 612); in der Abnahme 3 acht von acht Laeufen ohne ein Bild in fester Wand (der alte
Stand in drei Laeufen der Abnahme 1: je 1149 Bilder in fester Wand, das sind etwa 38 s, Ende an der Stelle Ihrer
Sitzung). Klettern ohne Biss ist in den Protokollen bitgleich mit dem alten Stand.

**Der Alligator nimmt den Kanister in der ersten Passage auf, in der er im Aufnahmefenster liegt - auch mitten im Biss
und im Spurt.** Bisher galt das Aufnahmefenster nur, solange der Alligator die Figur schwimmend verfolgte oder
aufgetaucht lauerte; im Biss und im Spurt galt es nicht. In 15 Laeufen des alten Stands blieben 13 von 27 Passagen am
Kanister ohne Aufnahme: 4 im Spurt, 1 im Biss, 7 seitlich vorbei, 1 der Laenge nach ausserhalb. Jetzt gilt das Fenster
auch im Biss, wie in RE2: Dessen Fenster-Abfrage (@0x80100ab8-b1c) fragt den laufenden Biss nicht ab. Der Biss laeuft
dabei wie in RE2 zu Ende und trifft; gesperrt ist nur ein neuer Biss. Im Spurt ist das Fenster eine Port-Wahl nach Ihrem
Wort "sobald die Position erreicht ist, wo der Behaelter liegt": RE2 kennt im Spurt weder Fenster noch Abbruch. Das
Fenster selbst ist unveraendert. Sein fernes Ende (voraus, vom Alligator aus gesehen) stammt aus RE2. Dass es im Spiel
der Laenge nach schon am Alligator selbst beginnt (0 bis etwa 6000 voraus statt nur das letzte Stueck davor), ist seit
der Spur G2 Port-Wahl nach Ihrem Punkt 8 vom 1.11. (Liste Ihrer Punkte in G2_gatorfix.md Z. 13: "erst wenn er beim
Kanister ankommt, aufnehmen"); zur Breite siehe "Zu entscheiden". Die uebrigen RE2-Stellen stehen unter "Technisch".
Gemessen (Abnahme 1, noch mit der alten Kanisterlage): In jeder der neun Lagen faellt die Aufnahme in die erste Passage,
in der der Kanister im Fenster liegt; die Passagen bis zur Aufnahme sinken in vier Vergleichslaeufen von 3/3/2/3 auf
1/2/1/1. In einem Elza-Lauf nahm er den Kanister im alten Stand in 3600 Bildern (2 Minuten) nicht auf, in der Abnahme 1
bei Bild 627 (etwa 21 s), im Endstand mit der neuen Kanisterlage bei Bild 608 (etwa 20 s; Abnahmen 2 und 3). Aus dem
Riegel zum Biss (Zahlen aus dem Bericht des Fixers, keine Messung einer Abnahme): 9 von 9 Fenstertreffern im Biss, der
Biss laeuft in 44 von 44 Bildern gleich wie ohne Kanister.

**Bricht der Kanister einen Spurt ab, wechselt der Alligator weich ins Schwimmen (Port-Wahl).** Trifft das Fenster im
Spurt, bricht der Alligator den Spurt ab und schwimmt weiter. Die Form dieses Abbruchs ist Port-Wahl, weil RE2 den
Abbruch nicht kennt: ohne die Vierteldrehung, die in RE2 nur zum normalen Spurt-Ende gehoert, und mit der Ueberblendung
32, mit der RE2 ins Schwimmen eintritt (@0x80100c38). Der Kopf senkt sich dabei ueber mehrere Bilder, das Maul schliesst
allmaehlich. Einen harten Wechsel in einem Bild (vom Anlauf mit Kopf hoch und Maul offen ins flache Schwimmen) gab es
nur in den Zwischenstaenden dieser Runde vor dem Nachtrag, also in den Staenden der Abnahmen 0 und 1; im alten Stand gab
es ihn nicht, weil es dort keinen Abbruch gab. Gemessen (Abnahme 3): Bildunterschied im ersten Bild nach dem Abbruch
2,09, danach 1,92 bis 2,16 (kein Sprung); mit dem Messhaken, der den Zwischenstand herstellt, 3,28 (Sprung). Der
Bewegungsablauf ist in den Protokollen derselbe; eine Zeile weicht ab (die Lage des Kanisters im Maul beim Anhaengen).
Das normale Spurt-Ende ist unveraendert.

**Der Kanister landet auf der Mittellinie des Westkanals.** Port-Wahl nach Ihrem Wort "lege den Behaelter zentraler in
die Mitte". Die Kanalmitte ist an der Kollision des Raums gemessen (der Kanal ist zwischen Westwand und Plattform 7200
breit; Stellen unter "Technisch"). Der Wurf ist dieselbe RE2-Bahn (RE2 ROOM7020, @0x088E-08A2), nur anders gedreht:
Wurfrichtung 3342 statt 3072 (Nordwurf) und 888 statt 1024 (Suedwurf). Der Kanister liegt, wo er aufschlaegt. Beim
Nordwurf sind das bei Leon 216, bei Elza 244 oestlich der Kanalmitte; im Stand der Abnahme 1 lag er 1570 westlich davon.
Beim Suedwurf - nur mit Messhaken gemessen - sind es 45 bzw. 73 oestlich der Mitte. Gemessen (Abnahme 3): 20 von 20
Standardlaeufen (10 Wege je Leon und Elza) mit Aufnahme, in keinem schwimmt der Alligator vorher ausserhalb des Fensters
am Kanister vorbei. Wurf-Szene, Text und Fall sind dieselben Ablaeufe (Marvin "Hey, Leon, try with this!" bzw. John
"Hey, Elza, try with this!"). Eine Nebenwirkung im Bild: Marvin bzw. John wirft den Kanister oben vom Gelaender. Steht
Ihre Figur (Leon bzw. Elza) dabei vorn im Bild der tiefen Wurf-Einstellung - so in den Wurf-Aufnahmen der Abnahme 2 -,
zieht der fallende Kanister kurz hinter ihrem Kopf und ihrer Schulter durch; mit der alten Richtung fiel er laut Abnahme
2 rechts neben der Figur (siehe "Noch offen"). Den Rest an der Westwand finden Sie unter "Zu entscheiden".

**Der Schluessel liegt bei der Leiche und laesst sich nach "Leiter hoch und wieder runter" aufnehmen.** Zu Ihrem Punkt 3
ergab die Messung: Die Aufnahme selbst ging nicht verloren (200 von 200 Tastendrucken am Schluessel wirkten); dass er
sich nicht aufnehmen liess, liess sich so nicht nachstellen. Nachgestellt ist mit der Leiche Ihrer Sitzung: Der
Schluessel lag im Bild einer Kamera, die nur im Westkanal laeuft; an seinem Platz lief eine andere Kamera. Jetzt liegt
er immer im Gebiet der Kamera, die ihn zeigt (Kamera-Umschaltung des Originals, RE1.5 @0x80014230; Einzelstellen unter
"Technisch"), und nach Ihrem Wort am der Leiche naechsten Punkt, den die Figur erreichen kann (Port-Wahl; keine neue
Zahl). Gemessen (Abnahme 3, echter Kampf bis zum Tod, Leon und Elza): Schluessel 762 von der Leiche (Stand der Abnahme
1: 3528 bis 4910; davor 1214), als Schluesselring erkennbar (Ring etwa 9 x 11 Bildpunkte); nach Leiter hoch und runter
genau einmal aufgenommen (Leon Bild 151, Elza Bild 156, Abfrage "Will you take the Water Key?"), nach einem zweiten
Leitergang liegt kein Schluessel mehr da. Ueber 208 gerechnete Todeslagen: mittlerer Abstand zur Leiche 834, groesster
2560 (Stand der Abnahme 1: 3808 und 6036); 207 Lagen naeher, 1 gleich, keine weiter. Der Preis ist die Bildgroesse: Die
Angabe "mindestens 10 Bildpunkte gross" aus dem Abschnitt zur Spur P gilt nicht mehr. Die Bildgroesse entscheidet die
Lage nicht mehr, sie wird nur noch gemessen - siehe "Zu entscheiden".

**Der Schluessel liegt nicht mehr hinter der Leiche (Durchfall der Abnahme 2).** In der Todesrolle war nach dem Nachtrag
nur ein Bildpunkt zu sehen. Ursache: Die Verdeckung wurde am Suchpunkt geprueft, der Schluessel wird beim Ablegen aber
noch angehoben und gekippt und lag danach hinter der Leiche. Jetzt wird die Endlage geprueft, mit derselben Kugel wie
bisher (halbe Breite 800 des RE1.5-Alligator-Kastens @0x80118b98; keine neue Zahl). Gemessen (Abnahme 3): Todesrolle
jetzt 789 von der Leiche, 8 Bildpunkte (Kasten 8 x 5) in jedem Bild der Einstellung, zu Fuss erreichbar und aufnehmbar,
Leon und Elza - dieser Lauf begann mit dem Testhaken der Todesrolle, nicht mit einem natuerlichen Kampf, und ohne
Leitergang. Die Bildschranke der Riegel verlangt wieder mindestens 2 x 2 Bildpunkte an der Stelle des Schluessels und in
der Kamera der Einstellung; ein einzelner Bildpunkt faellt damit durch.

Riegel: Alligator-Gruppe "100% tests passed, 0 tests failed out of 154" (Abnahme 3, 10.11., 5019,56 s unter Fremdlast,
kein Riegel nachgefahren). Die volle Suite lief erst mit der Abnahme von v0.8.30 (siehe Pruefstand); dort war ein
Alligator-Riegel (Tragen) an die neue Zielhilfe anzupassen (8c54123dd).

### STAGE2: Alligator stirbt nur durch den Kanister (ROOM2090/2091)

Ihr Wunsch vom 17.11.: "ueberpruefe noch mal das der alligator wirklich nur mit den gascontainer getoetet werden kann.
Auf keinen Fall ander."

**Bis jetzt stimmte das nicht ganz**, obwohl es die Notizen zu v0.8.29 schon versprachen. Der Alligator hat 25 240
Lebenspunkte. Fielen sie unter null, starb er auch ohne Kanister, mit seiner alten Todesrolle aus dem ersten Bosskampf.
Mit normaler Munition ist das kaum zu schaffen, mit Messer oder Rohr (beide ohne Mengengrenze) aber moeglich. Dabei kam
ein zweiter Fehler heraus: Hatte er nur noch wenige Lebenspunkte, loeste ein Treffer auf den Kanister im Maul die alte
Todesrolle aus statt der Explosion.

**Jetzt:** Seine Lebenspunkte fallen nie unter null. Jeder Treffer bleibt ein gewoehnlicher Treffer mit Blut, Bruellen
und Grossreaktion. Sterben kann er nur, wenn er den Kanister im Maul hat und Sie ihn treffen; dann explodiert er wie
bisher. RE2 selbst kennt den Tod durch Beschuss; den hatte der Port nie eingebaut. Der Punkt "300 Lebenspunkte, stirbt
an jeder Waffe" aus dem Gore-Plan vom 16.11. ist damit gestrichen.

Neuer Riegel unit_r41_gator_nur_kanister: 1353 Treffer mit zusammen 42 374 Schadenspunkten (Raketen, Granaten,
Einzeltreffer) ohne Tod, danach der Kanister bei 1 Lebenspunkt: Explosion und Leiche. Auf dem Stand davor faellt er in
beiden Teilen durch. Alle 167 Alligator- und Kanister-Riegel gruen; unabhaengige Abnahme bestanden. Nur Testhaken
(Umgebungsvariablen, nicht im Spiel erreichbar) koennen ihn noch anders sterben lassen.

### Story nach dem 80%-Stand: Texte und Stimmen (ROOM1151, 30E1, 30C1, 6030)

Ihre Regel vom 2.11.: "Ja, aber die Raeume, auch wenn sie vom Port erfunden wurden bleiben so! Lediglich Story soll der
80% Version entsprechend angepasst werden. Bei der 40% Version vorsicht! Es gibt auch Platzhalter Charaktere, da nicht
alles fertig war! Sage mir auch, wenn gaenzlich Raeume fehlen und sowas!" und "wenn ein Story snipped nachweislich und
gravierend von der urspruenglich geplanten Story abweicht, dann muss ich den halt noch mal aufnehmen. Aber ich musste in
STAGE1 und STAGE2 sowieso mir viele zusaetzliche Dialoge und Cutscenes ausdenken um Sinn in der Story zu geben. Das
werde ich auch noch fuer STAGE3-6 machen muessen." (Memory reai-v2-story-nach-80-raeume-bleiben.md Z. 11 und 20). Die
Spur hat das 80%-Material katalogisiert (101 Story-Eintraege, 34 Texte, 64 Orte), den Port gegen die 40%-Disc gezaehlt
und den Abgleich mit zwei Entscheidungspunkten vorgelegt; umgesetzt ist, was Sie entschieden haben. Keine Raumdatei
wurde veraendert: Die Szenen sind Umleitungen beim Start des Original-Unterprogramms, die Texte Port-Texte. Zwei
unabhaengige Abnahmen am 7.11.: Abnahme 1 nicht bestanden (ein Blocker: im Zug 6030 war die Reihenfolge "Fuehrerstand
zuerst" erreichbar, dann nannte Leon Sherry, bevor er sie getroffen hatte - ein Rueckschritt gegen Runde 36);
Nachbesserung 1, Abnahme 2 bestanden (ST80_abnahme_2.md: "In ROOM6030 laufen beide Reihenfolgen widerspruchsfrei.").

**Elza, ROOM1151 (Chefbuero, Irons tot auf der Couch).** Drueckt man am Hebetisch daneben ("Nothing happened.") und klickt
danach Irons an, zeigt er wieder "He has already passed..." - vorher stand im selben Raumbesuch "Nothing happened.", weil
zwei Port-Texte dieselbe Nummer trugen. Leon in 1150 unveraendert.

**Elza, ROOM30E1 (John hinter dem Gitterzaun) - Ihr Entscheid A1.** Johns Zeilen 2 und 3 lauten jetzt "John: This is
bad... those monsters / have us surrounded!" und "John: ...Damn it, there's no place / around here to hide..." - statt
"one of those monsters bit me as I tried to get away!" und "...And there doesn't seem to be a place with any medical
supplies here...". Der Biss war eine Fehllesung des Mods: Die 40%-Disc sagt in derselben, dort gerufenen Zeile
"umstellt", und im 80%-Material entkommt John knapp. John gilt damit nicht mehr als gebissen. Die uebrigen Zeilen
bleiben.

**Elza, ROOM30C1 (Marvin am Tresen) - Ihre kurze Fassung.** Genau fuenf Zeilen: "Elza: Who are you?" / "Officer: Marvin
Branagh, RPD." / "Elza: Are there any others still alive?" / "Marvin: A few. There are other / survivors out there." /
"Elza: Then I hope they make it out." Dann Marvins Abgang wie bisher (Winken, Weglaufen, Kamera 3). Die alten Zeilen mit
"Leon Kennedy", Ada und "me and John and Sherry" laufen nicht mehr; Ada wird in Elzas ganzem Lauf nicht mehr genannt, was
50B1 "It's a woman. She's dead." stuetzt - Ihr Grundsatz aus Runde 36: "Die duerfen sich niemals begegnet sein".

**Leon, ROOM6030, hinterer Wagen - Ihr freigegebener Vorschlag.** 13 Zeilen in dieser Folge: Leon "Hang on, Marvin. /
We're getting out of here." - Leon "Who are you two?" - John "Name's John. This is Sherry. / We made it down here with /
a girl named Elza." - Sherry "She's out there fighting / the monsters!" (ein Original-Satz des Raums) - John "She's
trying to keep the outside / clear so we can escape." - John "She said she'd catch up with us." (Ihre Variante (a) zu
Elzas Verbleib) - Leon "Why aren't you out there?" - John "She told me to stay with the kid... / and look after your
friend here." - Leon "Where's Annette?" - John "The woman in the lab coat? / Up front, trying to start the train." -
Sherry "A lab coat...? Is it Mom?!" (Sherry dreht sich dabei zu Leon) - John "Easy, kid. Stay with me." - Leon "I'll
find out. Stay here." Nicht mehr zu hoeren: "Where's Elza?", "Is anyone else with you?", "I see. Thanks.".

**Leon, ROOM6030, Fuehrerstand - und die Reihenfolge (Nachbesserung 1).** Gehen Sie zuerst in den Wagen und dann nach
vorn: "Annette: Is everyone aboard?" - "Leon: Everyone we could find. There's a / little girl in the back... Sherry." -
"Annette: Sherry...? My daughter is / on this train? ...Thank God." - "Leon: Can you start this thing?" - danach wie
bisher "Certainly." und die uebrigen Zeilen. Gehen Sie zuerst nach vorn, sagt Leon dort nur "Everyone we could find. Can
you start this thing?"; erst nach dem Gespraech im Wagen folgt beim naechsten Besuch vorne der Nachtrag "There's a
little girl in the back... Sherry." mit Annettes Antwort. Jede Zeile laeuft hoechstens einmal, beide Wege setzen
dieselben Flags. Kein "Where's that biker girl?" und kein "I've never met her." mehr (laut Protokoll vom 2.11. ergibt
"biker girl" in 6030 fuer Sie keinen Sinn - Protokollfassung, kein Wortlaut). Annette erfaehrt hier von Sherry - nach
Ihrem Entscheid das Motiv fuer ihren Gang nach hinten beim Krachen; die Szene dazu steht unter "Annette". Zwei neue
Leon-Zeilen des Nachtrags haben noch keine Aufnahme - siehe "Fuer Sie zum Aufnehmen".

**Zug 6030 nach dem Tor (Integration mit der Annette-Szene, 8.11.).** Gemessen wurden durchgehende Laeufe mit Leon:
erster Besuch im Zug, Torschalter in 6000, zweiter Besuch. Ein Weg war widerspruechlich: zuerst der Fuehrerstand, dann
der Wagen, dann ohne Rueckkehr nach vorn zum Tor - beim naechsten Betreten rief Annette beim Zugstart "...Sherry is back
there!", obwohl ihr niemand von Sherry erzaehlt hatte, und Leons Nachtrag kam nie. Behoben: Auf diesem Weg kommt beim
ersten Betreten des Fuehrerstands nach dem Tor zuerst Leons Nachtrag ("There's a little girl in the back... Sherry." mit
Annettes Antwort), der Zugstart folgt beim naechsten Betreten. Die Wege "Wagen zuerst" und "vorne, Wagen, zurueck nach
vorn, dann Tor" waren schon widerspruchsfrei und bleiben, wie sie sind. Jede Zeile laeuft hoechstens einmal (28 Zeilen).
Zwei weitere Wege kann kein Beleg entscheiden, weil das Original-Skript nach dem Tor beide Zug-Szenen ueberspringt - siehe
"Zu entscheiden". Elza: In 6031 gibt es keine Frau im Kittel; ihr Zug ist davon nicht beruehrt.

**Bewusst unveraendert (Ihr Entscheid A2):** In 50B0 loest bei Leon Ada den Reaktor aus; Elzas 5011 "beast broke the power
core" bleibt als Elzas Vermutung (Capcom-Text). Die Kenntnisnahme-Punkte B1 bis B16 des Abgleichs sind nicht angefasst
(siehe "Zu entscheiden"). Capcoms sechs englische Saetze des 80%-Stands (Dach 1170, Hinterhof 3061) stehen im Port und in
Ihren Aufnahmen schon im selben Wortlaut; nur "Look," fehlt in 3061 Zeile 5.

**Stimmen.** Jede Zeile sucht ihre Datei unter synchro/STAGEn/roomXXXX/mainNN.wav, ohne Umleitung; welche Dateien neu
gesprochen sind, steht mit Dateinamen unter "Fuer Sie zum Aufnehmen". Laut Protokoll vom 3.11. haben Sie die
Aufnahmeliste eingesprochen ("alle voiceovers ergaenzt bis auf Anette 5090"); Annettes drei Zeilen fuer 5090 sind aus
der RE2-Claire-Disc geschnitten. Die Abnahmen haben mit Ihren neuen Aufnahmen aus dem Hauptbaum gemessen: Stimmen
synchron. Die Dateien sind seit dem 9.11. im Repo (siehe "Fuer Sie zum Aufnehmen").

**Lieferung ausserhalb des Spiels.** Die drei Dossiers (Katalog, Zensus, Abgleich) sind die Grundlage fuer Ihre eigenen
STAGE3-6-Dialoge, mit der Liste der Luecken je Raum. Fehlende Orte nach Ihren Antworten: Raum mit rotem Rad = 2060, Tunnel
mit oranger Tuer und Rippen-Tunnel = 2000/2001, Bahnsteig-Halle etwa 5080. Der Moosgang der 40%-Disc ist im Mod derselbe
Gang wie ROOM20A0, mit Ziegeln uebermalt; nach Ihrem Entscheid vom 3.11. wird kein neuer Moosgang eingebaut. Das Finale
(Disc-Raeume 605-607) fehlt in jedem Material: Der Port hat nach dem G5-Kampf kein Ende, die Disc auch nicht - Ihr Wort:
"Sobald er Tot ist, muss ich mir eben ein Ende ausdenken." (Memory reai-v2-story-ende-annette-g5.md Z. 11); Ihre
Vormerkung fuer v0.8.31 steht in der Einleitung.

### STAGE3 und 5: Birkin-Kette bis zum Game Over - Angriffsmuster aus dem RE2-G4, Spieler-Klemme nach RE2, G5-Biss und Fressen (Spur R)

Ihr Nachtrag 38 vom 31.10.: "Die von dir gebaute neue Birkin Version auf dem Bahnhof sieht nicht ansatzweise so aus, wie
auf den Bildern, nach dem du diese bauen solltest. Also wie bei: vlcsnap-2026-10-31-00h28m45s774.png und
vlcsnap-2026-10-31-00h29m02s830.png. Es ist eine eigene Form und braucht dann auch eigene Bewegungsanimationen."
(AUFTRAG.md Z. 1161-1163). Die Spur hat daraufhin die Riesenform auf dem RE2-G4 aufgebaut (dickere, knotige Glieder,
engerer Stand, breiterer Rumpf; Bewegungen, Reichweiten und Trefferbilder aus dem G4). Diesen G4-Ansatz haben Sie noch
am 31.10. als Form verworfen: "nein, ist es nicht. Es gibt nur diese 2 Screenshots des modells" (Memory
reai-v2-bahnsteig-nur-zwei-screenshots.md Z. 11). Die eigene Form (Abschnitte "Riesenform" unten) hat die G4-Fassung der
Spur R im Spiel ersetzt: In 5080/5081 und 5090/5091 steht die eigene Riesenform (seit Ihrem Entscheid vom 7.11. abends;
in v0.8.30 in der Fassung K5g3). Was von Spur R unabhaengig vom Modell im Spiel bleibt, steht hier. Drei Abnahmen: die
erste (drei Blocker, u. a. Form mit der Pistole gar nicht erreichbar oder unverwundbar) und die dritte (Liste N1 bis N6
mit einem Blocker; die Datei traegt kein Urteil) sind als Dateien belegt, die Behebung der sechs Punkte nur im Dossier
der Spur; das Protokoll fuehrt "Runde 3 bestanden". Nicht belegt: Eine vierte Abnahme-Datei gibt es nicht.

**Die Birkin-Kette laeuft mit der Startwaffe bis zum Ende, Leon und Elza bildgleich.** 5090/5091 im Dauerfeuer:
Mutation, Riesenform, Griff, Rueckstoss-Szene, G5, G5-Tod; 3070 (Leon getippt), 3071 (Elza), 5080/5081: Birkin stirbt
(Stand der Spur R; seit der Riesenform-Stufe 3 beginnt 5090 gleich mit der Riesenform, und in 5080 flieht sie am Ende -
siehe "Riesenform"). Vorher hing die Kette an einem Treffer im ersten Bild nach dem Modelltausch (Form dauerhaft
unverwundbar), die Form kam mit dem Mutationsrest statt eigener Lebenspunkte an, und in 3071 blieb Birkin nach dem
Auftritt hinter dem Tor ohne Sichtlinie - alles behoben. Die Lebenspunkte je Form stehen unter "Handgranate" (Riesenform
1000).

**Riesenform (Angriffsmuster aus dem G4, mit der neuen Form unveraendert):** Gehen in der Bodenphase, Hieb und
Doppelschlag (beide Hiebe treffen, wie im Original), Biss, Rennen mit Ueberrennen (toetet nie), Griff (Spieler mit dem
Gesicht zur Form), Zucken, Auftritt, Weichen bei Naehe - Bildzahlen, Reichweiten und Trefferzonen wie im G4; Schaden wie
die bisherigen Birkin-Rollen (Hieb und Biss 10, Rennen 5), der Griff 35 nach der auf 100 Port-HP halbierten RE2-Regel.
Nach einem Treffer im Rennen dreht die Form ab statt Leon durch den Wagen zu tragen; sie schiebt sich selbst aus dem
Spieler (RE2-Reihenfolge), Posen springen nicht mehr.

**Spieler-Klemme nach RE2 Retail im Birkin-Kampf.** Die RE1.5-Beta-Klemme versagte bei grossen Versaetzen - Leon landete
in der Suedwest-Nische von 5090 fest in der Wand, ohne Eingabe, ohne Treffer: Softlock. Im Birkin-Kampf gilt jetzt die
Klemme von RE2 Retail, mit drei als Port-Wahl markierten Regeln fuer die engere Bahn von 5090 gegen RE2s Zugwagen.
Gemessen: 0 Bildenden in fester Zelle in allen Laeufen, die vorher 388 bis 3085 hatten; tausende Griffe und Lagen je
Raum: 0 fest, 0 eingekeilt. Ausserhalb des Birkin-Kampfs bleibt die Klemme Byte fuer Byte die alte.

**G5: Biss, Fressen, Game Over.** Der Biss toetet nie (wie der Original-Aufruf), gefressen wird nur ein Spieler mit 0 HP
(die Logik war verkehrt: der Port frass den gesunden Spieler), die Biss-Sperre laeuft ab (vorher nach dem ersten Biss kein
zweiter), das Kauen laeuft mit Gewicht und Ton: "YOU DIED" ueber dem laufenden Fressen, dann der Titel. Arme: einer toetet
mit Gnade wie das Original, die anderen nie. G5-Schaden auf Port-HP: Biss 20, Arme 7, 7, 5, 7. Der G5 blutet bei Treffern
und spielt die Biss-Blutserie; Blut faellt wie im Original nur, wenn der Spieler nach Westen blickt.

**Kamera.** Die Rueckseiten der Birkin-Formen werden wie in RE2 verworfen: Steht Kamera 13 im Koerper des G5, sieht man
den Spieler, Arme und Bisse statt der Innenseite des Koerpers. Nach der Rueckstoss-Szene schaltet das Bild aus der Lage
des Spielers auf den passenden Schnitt (vorher stand Leon oestlich ohne Zone und ohne Bild), und die Sperre nach dem
Morph dauert 271 statt 1126 Bilder.

### STAGE5: Birkin Sichel-Form im Hangar und Labor (Spur SF)

Ihr Auftrag (ueber den Orchestrator, Dossier SF_sichel.md Z. 11-13): "Der Hangar Birkin der ist als Grundlage da, den
musst du nur noch mal gegen die 80% Version im Video genaustens analysieren und im Zweifel korrigieren." Die zweibeinige
Form aus dem Hangar (50A0) und dem Labor (50E0) ist kein Original-Modell: Sie existiert auf der ausgelieferten Disc
nicht und ist eine Nachbildung nach dem 80%-Video (Complete Disc Video 4). Nach mehreren Modellfassungen
(Nachbesserungen 1 und 2, Neuaufbau, Referenz 2 - von Ihnen am 3.11. gelobt -, V3 und V4) und Ihrem Entscheid vom 6.11.
(zwei weitere Versuche) wurde der Koerper auf die gelobte Referenz 2 zurueckgesetzt; Schwanz und Kopf werden nach Ihren
beschrifteten Beschreibungen neu gebaut ("Ich will EXAKT diese Umsetzung und auch EXAKT seine Farben.", SF_sichel.md Z.
1205-1207). Den Kopf gestalten Sie seit dem 7.11. selbst mit; Ihre Hinweise zu Farbe, Auge, Haaren, Maul, der Form ueber
dem Maul, dem Schwanz und dem Ruecken flossen Runde fuer Runde ein. Neue Grundlage seit dem 8.11. ist der Kopf des
Original-RE1.5-Birkin - Ihr Wort (Protokoll vom 8.11.): "Orientiere dich an den Original Birkin in Resident Evil 1.5.
... Also nutze das Original Resident Evil 1.5 Birkin Modell als Ausgangsgrundlage, damit solltest du nicht viel falsch
machen." und "Immerhin ist dieses Modell ja eine Mutation/Weiterentwicklung des anderen Models! Dementsprechend muss ein
Teil des Vormodells erkenntbar drin sein."

Die Runden am 8.11. folgten Ihren Saetzen einzeln: grauer Kopf nach dem Original-Birkin mit geschlossenem Mund und
grauem Haar, Schwanz dick und grau ohne Huckel, Rot nur auf der Brust und am Oberarm, kein roter Fleck auf der rechten
Schulter, und zuletzt die linke Hand, Ihr Wort (woertlich): "Beim 2 Beiner Birkin - fast perfekt, aber die linke Hand von
ihn stimmt immer noch nicht. Erstens sieht man auf den Original Bildern ganz klar, das alle Krallen bis auf die Daumen
Kralle rot sind, die Daumen Kralle ist grau. und 2. Sieht man auf den Original Bildern das Offenbar JEDE der Krallen im
Original laenger ist...." Daraus wurde V7: Daumenkralle grau, drei Fingerkrallen rot, alle auf die Laenge der Fotos.

**Zwischenstand vom 8.11. - V7, Ihr Wort (woertlich):** "Naja, gehen wir so erst mal ins rennnen." V7 war das Modell der
Fassung vom 9.11. (die Szene im Spiel Bild fuer Bild wie das Video, das Sie gesehen haben; unabhaengige Abnahme der Spur
bestanden). In v0.8.30 ist V7 durch V8 ersetzt.

**V8 nach Ihrer Skizze vom 8.11.** Ihr Wort (woertlich): "Das Gesicht vom 2 beinigen Birkin ist jetzt schon gut, aber
mit Schwanz und sowas bin ich immer noch 0 zufrieden.... Ich verstehe das auch nicht. Es ist doch eigentlich KLAR
erkennbar. Siehe Darstellung.png und Darstellung_Vorlage.png. Man sieht ganz klar den Koerper Aufbau...." und "und wenn
du schon dabei bist, am ueberarbeiten, man sieht ebenfalls beim vorlage Bild, das die Rechte Seite auch mehr rottoene
hat, also quasi mehr adern an verschiedenen Stellen, am Arm und Koerper, nicht nur grau. Uebernehme die die Textur
Sachen die du dafuer brauchst vom originalen Birkin von Resident Evil 1.5". Gemessene Ursache: Die Schwanzwurzel sass zu
tief und seitlich, und die L-Biegung steckte fest in allen Bewegungsbildern. Danach Ihre Punkte vom 9. und 10.11.
(woertlich): "Die Unterseite des grossen Schwanzes ist fleischig wenn man in die Vorlagen Bilder schaut. Nur die
oberseite ist durchweg grau." - "es gibt immer noch eine Luecke zwischen Ruecken und Kopf beim 2 beiner Birkin. Das ist
in der Vorlage so wie es aussieht nicht so." - "Die Vorlage hat eine breite Hand mit breiten Unterarmt. Dadurch wirkt es
natuerlich. Bei uns ist die Hand ganz klein und zierlich und wirkt dadurch verkuemmert." - "Bei dem 2 beinigen Birkin in
der Vorschau sieht es so aus als waere die linke Hand verkehrt herum gedreht, Kann das sein=?" (Dossier SF_sichel.md und
Memory reai-v2-runde41-laufend.md). Am 14.11. zur Handfarbe "wie in der Vorlage." und zum Vorschauvideo mit der neuen
Szene: "Nee, nicht die Hautfarbe aendern. Fast Perfekt." Zum Schwanz: "weder mit dem rot der Schwanzfarbe noch mit dem
etwas zu steifen grossen Schwanz zufrieden" (Commit c94847a31), zur Farbprobe "Ich wuerde auch sagen b, wobei das
Original Video der Schwanz noch etwas heller ist und nicht ganz so rot" (804c62601), zum Endstand der Szene "Im prinzip
fast perfekt, aber - ich weiss nicht wo du die Streifen bei den Schwanz Abschnitten von Birkin her nimmst, die gehoeren
da nicht hin. Schau doch auf das Original Video, der Schwanz wirkt auch ueber die Abschnitte hinweg einheitlich."
(6af3e3324) und am 15.11., als die Oberseite dabei ihre Farbe verlor: "NUR DIE UNTERSEITE WAR KOMISCH MIT DEN
ABSCHNITTEN!!!!" (83350f359).

**Endstand - V8 mit Schwanz 8g, angenommen am 15.11.** Ihr Wort zu Ordner 32 (woertlich): "Naja, so ist es in Ordnung,
so kann ich damit leben." (Protokoll vom 15.11.). Im Hangar (50A0/50A1) und im Labor (50E0/50F1) steht jetzt V8:
* Nacken: Die Luecke zwischen Ruecken und Kopf ist zu, eine glatte Linie fuehrt vom roten Huegel zum Kopf, kein Kragen;
  das Gesicht ist das von V7.
* Hand und Unterarm breit und richtig herum (Handruecken zur Kamera, Daumen innen, Krallen nach unten); die Hand grau
  aus dem Originalmodell wie der Unterarm, die Krallen rot; der linke Unterarm haengt tiefer, wie in der Vorlage.
* Schwanz: breit aus dem oberen Ruecken, flach (von vorn schmal), mit Gliedern und Stachelspitze; er biegt sich und
  folgt der Bahn aus dem Video. Oberseite, Ruecken und Flanken dunkel wie im Stand 8c (mit den weichen Ringen), nur die
  Unterseite einheitlich fleischfarben ohne Abschnitte; der Schwanz haengt in jedem Bild am Koerper.
* Die rechte Seite traegt Adern aus der Original-Textur. Alle Farben stammen aus Original-EM30-Texeln, keine Eichung am
  Video.

**Wie geprueft:** Runde 7 des Modells haben zwei unabhaengige Pruefer bestanden ("BESTANDEN (kein ROT), mit
GELB-Punkten" und "Bestanden (keine Bewertung der dritten Klasse)"). Die Nacken-Stelle lief als eigene Spur; deren
dritter Kandidat war der beste Stand und wurde vom Orchestrator genommen, obwohl ihn beide Pruefer formal nicht bestehen
liessen. Die Handfarbe 7e liess der Pruefer knapp nicht bestehen (Hand zu hell, Rot nur an den Krallen); ein roterer
Fingerteil (7e2) wurde verworfen, weil die Hand dann deutlich roeter als die Vorlage waere. Schwanz 8g: die Oberseite in
0 von 384 Flaechen anders als 8c, die Unterseite ganz im Bauchfeld, keine Naht. Die Szene mit dem Schwanz brauchte fuenf
Runden, die letzte bestanden ("Videos ehrlich: JA. Vorlegbar: JA", Gruppe "100% tests passed, 0 tests failed out of
30"). Die Sichtpruefungen der Szene liefen mit Leon in 50A0; Elza ist ueber bitgleiche Protokolle belegt.

### STAGE5: Hangar 50A0/50A1 - Spider-Man-Szene ueberarbeitet, Kampf, Birkin bleibt liegen, Sherry-Ruf

Ihre Befunde zur Szene vom 13. und 14.11. (woertlich): "was da raus kommt beim Spiderman waehrend Birkin ihn hoch haelt
oder auf den Boden wirft sieht aus wie gruenes Gas oder sowas. Aber EIGENTLICH ist es gruenes Blut. So wie jedes andere
Blut im Spiel auch." - "Im Original Video gleitet der Spiderman mit gehobenen Armen zum Angriff Linear auf Birkin zu,
und wird dann von Birkin abgefangen und hochgehoben. Bei uns springt er in den Vorschau Videos mehr oder weniger direkt
hoch auf Birkins Arm." - "Ausserdem struggled der Spiderman im Original Video viel mehr oben auf den Arm bis er
letztendlich tot ist. Bei uns bleibt er aktuell mehr oder weniger statisch." - "Im Original dreht sich Birkin da mehr
ein, und schmeisst ihn so eindrehend hin. Bei uns schmeisst er ihn einfach lustlos gerade auf den Boden." - "Was viel
eher auffaellt ist, das birkin dann zum schluss im video kurzzeitig einmal nach vorne gleitet statt zu gehen." (Commits
f6deac461, 64702bdfd, 16924ee0d, 2cdcf1c8f, b84fcc83c).

**Die Szene (Leon 50A0, Elza 50A1).** Das Blut ist jetzt das Original-Spinnenblut (deckende dunkelgruene Spritzer wie
bei den Spinnen in 2070), kein Gas. Der Spider-Man gleitet schraeg durch die Luft auf Birkin zu, wird an der Brust
abgefangen, zurueckgedrueckt und weich hochgehoben; er zappelt oben deutlich, wird schwaecher und haengt dann tot; sein
Schatten liegt am Boden. Birkin holt aus, dreht sich in den Wurf ein und steht danach seitlich, der Spider-Man schwingt
seitlich heraus; am Ende geht Birkin mit sichtbaren Schritten (Original-Gang) statt zu gleiten. Der Schwanz bewegt sich
mit (Rolle, Bahn je Videobild, Biegsamkeit). Die Szene gibt es nur im 80%-Video: Die Bewegungen sind Original-Clips von
Birkin und Spider-Man, Reihenfolge, Drehung und Takt sind am Video gemessen (Port-Szene). Sechs Pruefrunden, die sechste
bestanden ("springt nach meiner Messung und Sichtung nichts mehr sofort als Fehler ins Auge"); der Wurf bestand eine
eigene Pruefung.

**Nach der Szene: Kampf (Ihre Wahl vom 15.11.).** Am 15.11. sagten Sie (woertlich): "diese Szene wo der Birkin den
Spiderman schmeisst, ist 100%ig kein Boss Kampf. Sie wirkt nicht so. Ich wuerde eher sagen, das Leon oder Elza Birkin
auf dem Monitor sehen mit der Szene, wo auch immer er danach hin laeuft." Die Szene als Monitorbild im Monitor Room
5030/5031 wurde gebaut, bestand beide Pruefungen und wurde von Ihnen abgelehnt: "wenn die Birkin Szene ueber den Monitor
anders aussieht, und ja offenbar anders aussehen muss - das sieht man ja bei Sherrys Szene - dann ist das im Original
doch nicht ueber Monitor gezeigt. Denn man sieht ja EINDEUTIG, das das normale Spiel grafik ist" (Memory
reai-v2-hangar-szene-monitorbild.md). Auch die Variante, in der Birkin nach der Szene verschwindet, haben Sie verworfen
("man steht mitten im Raum ... unlogisch", ebd.); Ihre Wahl: "Im Hangar, dann Kampf". Das Monitorbild ist vollstaendig
zurueckgebaut (Monitor Room und Elzas Sherry-Szene wie vorher). Neu: Das Spiel merkt sich im Spielstand, dass die Szene
gesehen ist - nach dem Laden oder beim Wiederbetreten kommt sie nicht noch einmal, Birkin greift sofort an. Der
Hangar-Birkin hat dieselbe verdoppelte Lebenspunkte-Skala wie die anderen Birkin-Formen (600 statt 300).

**Nach dem Kampf: Birkin bleibt liegen (Ihr Wort vom 16.11.).** "Das er aufsteht und wegrennt ist daemlich. Besser er
bleibt einfach liegen..." (Commit 98391e747). Besiegt bricht Birkin zusammen und bleibt liegen (Original-Todesablauf;
gemessen 40 Sekunden ohne jede Bewegung). Das Spiel merkt sich den Sieg im Spielstand: Wer hinausgeht oder spaeter einen
Spielstand laedt, findet den Hangar leer. Hat man Birkin zuerst im Kraftraum besiegt, zeigt der Hangar weder Szene noch
Birkin; der Kraftraum-Birkin bleibt in jedem Fall. Der Pruefer hat einen echten Kampf ohne Messhaken gefuehrt (12
Raketen): "Bestanden." Einen Gegenstand oder eine geoeffnete Tuer bringt der Kampf in v0.8.30 nicht; fuer v0.8.31 haben
Sie entschieden, dass die Szene "von aussen" laeuft und es im Gang keinen Kampf mehr gibt (siehe "Vorschau").

**Sherry-Ruf (Ihr Wunsch vom 16.11.).** "Ah, und ich moechte das der 4 Beiner nicht mehr "Sherry" ruft, denn das ist
wirklich weird! Allerdings kann 2 Beiner Birkin mit Schwanz waehrend er Richtung Kamera laeuft nachdem er den Spiderman
weg gehauen hat noch mal Sherry rufen." (Commit 84b36f0d2). In der Hangar-Szene ruft Birkin jetzt einmal "Sherry", wenn
er nach dem Wurf auf die Kamera zugeht - Birkins eigener Ton von der Disc (derselbe wie in ROOM3070); sonst ist die
Szene Bild fuer Bild unveraendert. Die Riesenform in 5080/5090 ruft nicht mehr; das Gebruell bleibt. Der Ruf der
Riesenform war eine fruehere Port-Wahl (Birkins Ruf an stummen Stellen des RE2-G4) und ist zurueckgenommen. Pruefer:
"Bestanden." (Gruppe "100% tests passed, 0 tests failed out of 54"). Auf der PSX gibt es den Ruf nicht.

### STAGE5/6: Annette im hinteren Zugwagen - Szene wie in RE2 (Spur AN)

Ihr Auftrag vom 2.11. (woertlich): "Sobald man den Boss aussen besiegt hat, den Generator eingeschaltet und das Tor
geoeffnet, ist Anette nicht mehr im Fahrer Raum. Sie ist hinten im Zugabteil. In Resident Evil 2 gibt es eine Szene wo
Anette zu ihren Mutierten William spricht, und er sie dann umbringt. Sowas wuerde ich dann im hinteren Zugabteil auch
machen inklusive dem Dialog. Betreff der G5 Birkin Form am Ende - das ist eine Entscheidung die ich gerne so drin lassen
moechte - also erst die Tierform von Birkin noch mal die wir erstellt haben und dann die G5 Form." (Dossier AN_annette.md
Z. 6-10).
Fuenf Abnahmen; die fuenfte (7.11.) ist bestanden, seit dem 8.11. auf master. Die Abnahme 4 fiel wegen eines Blockers
durch: Die lebende Annette im Fuehrerstand und in 6000 bekam den RE2-Leichenkasten, Leon kam ihr bis 802 statt 900 nahe -
ein Rueckschritt gegen das Original in RE1.5-Raeumen. Nachbesserung 6 gibt ihr in 6000/6030 und in 5090 vor der Szene
wieder ihren RE1.5-Kasten (Leon haelt bei 901 bis 903); das kleinere RE2-Mass gilt nur ab dem Szenenstart und fuer die
Leiche (sie haelt Leon bei etwa 800 auf). Abnahme 5 hat das im Fenster nachgemessen.

**Ablauf (von den Abnahmen 1 bis 5 bestaetigt):** Nach Bahnsteig-Boss, Generator und Tor ist Annette nicht mehr im
Fuehrerstand, sondern im hinteren Wagen 5090. Beim Zugstart in 6030 eine kurze Szene mit vier Zeilen ("There! The
train's moving." / "What was that? ...Sherry is back there!" / "I'll check it out." / "No. She's my daughter. Stay here
and keep an eye on the controls."). In 5090 die Szene nach dem RE2-Vorbild von der Claire-Disc (dort gefunden und im
Emulator aufgenommen; auf der Leon-Disc gibt es sie nicht): Annette spricht die Riesenform an ("W-William! You're
alive!" und "William." aus der Claire-Disc geschnitten, Schrei ohne Untertitel), die Form setzt an, trifft sie, sie
sackt im Trefferbild zusammen, Blut in RE2-Bildgroesse, dann liegt die Leiche flach mit dem RE2-Liegeclip; Leon ruft
"Annette!" (Ihre Aufnahme). Danach beginnt der Kampf. Beim Wiedereintritt liegt die Leiche weiter da; Schuesse gehen
durch sie und durch lebende Begleiter hindurch auf den Gegner (RE2-Regel, Ihr Entscheid). Bei Elza gibt es wie
vorgesehen keine Szene.

**Zusammen mit dem Rest des Endkampfs (Integration, 8.11.):** Ein Lauf mit Leon in 5090, mit den Speicher-Bits von Tor
und Zugstart wie im echten Weg, zeigt die ganze Folge - Szene, Annettes Tod, Kampfstart, Riesenform mit vier Raketen,
Rueckstoss, Sturz aus dem Zug, G5-Auftritt - ohne Absturz; er ist als Riegel festgehalten.

### STAGE6: Sherry (6031) und Annette (6030) beim Sprechen nicht mehr auseinandergezogen (Integration)

Ihre Marke vom 8.11. (befund.log, ROOM6031 Bild 164, Bild befund_6031_F164_marke1.bmp): Sherry war beim Sprechen
auseinandergezogen - der Kopf oben versetzt, die Oberarme seitlich abgeloest. In Ihrer Story-Liste stand dasselbe fuer
Annette in 6030.
Ursache: Der Port hielt das Skelett einer Szenenfigur je Platz in der Figurenliste. Setzt eine Szene im selben Raum eine
andere Figur auf denselben Platz, behielt die neue Figur die Knochenlagen der alten. In 6031 steht dort erst ein
Erwachsener aus der Wagen-Szene, dann Sherry; in 6030 erst John, dann Annette. Das Original bindet bei jedem Erscheinen
die Knochenlagen des neuen Modells (@0x800224a8 "sw v0,0x84(s2)"). Die Szenen-Bewegungen bringen keine eigenen
Knochenlagen mit (ROOM6031.RDT @0x13C8 und ROOM6030.RDT @0x17BC, Kopf o_arm 0).
Behoben: Wechselt die Figur auf einem Platz, bindet der Port ihre eigenen Knochenlagen neu (Commit 2c1c0eb1a). Zwei neue
Riegel halten beide Szenen fest; der alte Stand macht beide rot. Die unabhaengige Pruefung hat die Ursache mit eigenem
Leser nachgerechnet und beide Szenen im Fenster gesehen: Sherry und Annette bleiben zusammen (Bericht ebfc5f0cf).

### STAGE5: Labor-Birkin (50E0/50F1) trifft wieder (Integration)

Gefunden bei der Abnahme der Sichel-Form und dann am echten Weg nachgemessen (Leon in 50E0, Elza in 50F1, Start an der
Tuer aus 50D0): Nach seinem ersten Hieb blieb der Birkin knapp vor dem Spieler in einer Hiebschleife stehen. Einen
stehenden Spieler traf er nie wieder. Das kam mit der Aenderung "Angriffsbox" nach v0.8.29.
Ursache: Der Port pruefte die Reichweite am aktuellen Bewegungsbild. Das Original prueft an der zuletzt gezeichneten Lage
der Klaue, die zwei Bilder zurueckliegt: FUN_8001a5e0 liest die gezeichnete Weltmatrix (@0x8001a5ec bis @0x8001a638),
und das Bild wird vor dem Hochzaehlen gesetzt (@0x8001f35c, @0x8001f61c). Die Klaue erreicht den Spieler genau zwei
Bilder vor dem Trefferfenster; der Port pruefte erst, als sie schon zurueckschwang.
Behoben (Commit 145316fc9): Die Box prueft jetzt die gezeichnete Klaue. Am echten Weg trifft der Biss wieder, ein
stehender Spieler stirbt, und der Birkin ist mit genug Treffern zu besiegen; danach geht es durch die Tuer weiter. Der
Code ist in STAGE3 und STAGE5 gleich, die Aenderung gilt fuer alle Birkin dieses Typs (3070/3071, 5080, 50E0/50F1 und
den Hangar 50A0). Ein neuer Riegel prueft den echten Weg fuer Leon und Elza; der alte Stand macht ihn rot. Die
unabhaengige Pruefung ist bestanden.
Folge fuer Sie: Der Laborkampf ist wieder deutlich schwerer. Mit der Pistole hat Elza den Birkin in 50F1 in einem
Testlauf von 9000 Bildern nicht besiegt.

### STAGE3 und STAGE5: Birkins Klauenhieb und Rueckzug, Aufrichten ohne zweites Buecken

Ihre Befunde (woertlich): am 15.11. "Beim regulaeren Birkin fight, in Stage3 faellt mir auf, macht er manchmal
wiederholt so eine rueckzieh/nach Schlag Animation, die er im Originalen nicht macht. Das macht auch unsere veraenderte
Version mit Schwanz." und am 16.11.: "Aktuell ist es ja eher so - er schlaegt, zieht die Hand danach zurueck, steht
wieder normal da, und dann kommt dieses Rueckstoss Animation im Anschluss. Das ist auf jeden Fall falsch." (Auftraege
der Spur). Ihr Entscheid zur Schwanz-Gestalt vom 16.11.: "Das mit dem zurueckziehen wuerde nur sinn machen, wenn er
gegen eine Wand schlaegt und deshalb die Hand dann zurueck faehrt. Das muesste dann auch synchron dazu passieren. Bei
allen anderen Faellen macht das eigentlich keinen Sinn." (Commit 8ce27af6b). Am 17.11. auf die Frage, ob der Rueckzug
auch zaehlen soll, wenn die Klaue schon beim Ausholen die Wand beruehrt: "Nein" (Commit 83eab1010), und zum Ergebnis:
"Sieht fuer mich in Ordnung aus. Das merkt kein Mensch." (Protokoll vom 17.11.). Zum Aufrichten am 16.11.: "Auch jetzt
in der Vorschau wieder, wenn Birkin im Power Room runter springt und sich aufrichtet wiederholt sich noch mal die
runterbeugen Animation wie bei anderen Birkin in Stage3 auch... Warum? Das soll so NATUERLICH nicht sein...." (Auftrag
der Spur).

**Normaler Kampf mit dem Capcom-Modell (Leon 3070, Elza 3071).** Nach einem fertigen Hieb kommt kein Rueckstoss mehr,
Birkin greift direkt wieder an. Ursache war ein Port-Fehler: Der Port startete nach jedem Hieb die Erholung, das
Original nur, wenn die Klauenspitze im Schlagfenster in einer Wandzelle steckt (@0x80117834). Gemessen vorher 8 von 12
Hieben mit Rueckstoss, nachher 0 von 17; steht der Spieler direkt an einem Block, kommt der Rueckzug wie im Original (18
von 19). Ohne Rueckzug ist die Pause bis zum naechsten Angriff kuerzer (0x1e statt 0x3c Bilder) - im Vergleichsvideo von
3070 stirbt Leon im neuen Stand etwa 4 s frueher. Waehrend des Hiebs dreht Birkin nicht mehr zum Spieler nach.

**Schwanz-Gestalt (Kraftraum Leon 50E0 / Elza 50F1, Hangar 50A0/50A1).** Birkin zieht die Klaue nur zurueck, wenn die
gezeichnete Klaue wirklich gegen eine Wand oder unter die Oberkante eines Gelaenders schlaegt, und zwar im naechsten
Bild - auch wenn die Stelle ausserhalb des Bildes oder hinter Leon bzw. Elza liegt; ohne Wandkontakt nie. Das Ausholen
zaehlt nicht (Ihr "Nein"): Der Schlag beginnt bei Bild 32; steht Birkin dicht an einer Wand, faehrt die Klaue beim
Ausholen bis zu 11 Bilder in die Wand, der Rueckzug kommt erst mit dem Schlag. Trifft ein Schuss genau im Bild nach dem
Wandschlag, kommt der Rueckzug trotzdem (der Treffer zaehlt); ein toedlicher Schuss geht vor - Birkin faellt, die Klaue
gleitet aus der Wand. Die Treffer der Kralle gegen den Spieler rechnet der Port am Capcom-Gerippe des EM30, nicht am bis
590 Einheiten laengeren Arm des Ersatzmodells; die Regel aus "Labor-Birkin trifft wieder" (die Box prueft die
gezeichnete Lage, zwei Bilder zurueck) gilt weiter. Zwei fruehere Ansaetze sind verworfen: die Wandsonde des
Capcom-Modells allein (im Kraftraum und im Hangar zog Birkin damit auch zurueck, wenn man die Klaue an keiner Wand sah)
und eine Rechnung nach Sichtbarkeit im Bild (in 51 Bildern steckte die Klaue sichtbar in der Wand, ohne dass er
zurueckzog).

**Clip-Enden und Aufrichten (Leon 50E0, Elza 50F1, 3070, 3071, Aufzug 3080).** Am Ende einer Bewegung sprang der Port
auf das erste Bild der beendeten Bewegung zurueck und hielt es 7 bis 9 Bilder - beim Landen und Aufrichten ist das die
Hocke, im Kraftraum zweimal hintereinander. Das Original laesst das letzte Bild stehen und blendet weich in die naechste
Bewegung (@0x8001f35c, @0x8001f618). So ist es jetzt beim Auftritt und Aufrichten und - mit dem Klauenhieb - auch am
Ende von Hieb, Biss und Wurf; vor dem ersten Ansprung steht Birkin nicht mehr gut eine Viertelsekunde still
(@0x80117644). Original und deshalb geblieben: In Elzas 3071 senkt Birkin den Kopf zweimal (der erste Durchgang stark
gedaempft), im Aufzug 3080 stoesst die Hand zweimal zu. Die Hangar-Szene, 5080 und 5090 sind bitgleich zum vorherigen
Stand.

**Wie geprueft:** Den normalen Kampf bestaetigte schon die erste Pruefung ("Regulaerer Kampf 3070/3071: behoben und
belegt."). Fuer die Schwanz-Gestalt fielen zwei Ansaetze durch; der dritte (Weltkontakt der gezeichneten Klaue) mass in
16 Laeufen 92 Wandkontakte und 91 Rueckzuege (der fehlende: Tod im selben Bild), 0 Rueckzuege ohne Kontakt; dazu 744
Clip-Wechsel in der Schwanz-Gestalt und 26 im normalen Kampf, je ohne Ruecksprung und ohne Stillstand (der Pruefer
bemaengelte nur ein fehlerhaftes Vergleichsvideo). Ausholen: "Ihr Entscheid ist richtig umgesetzt." - 160 Ausholbilder
mit der Klaue in der Wand, keines mit Rueckzug (Gruppe "100% tests passed, 0 tests failed out of 53"). Aufrichten: zwei
Runden, die zweite "bestanden, kein roter Punkt" (Gruppe "100% tests passed, 0 tests failed out of 44"). In Ordner 45
fehlt das Vergleichsvideo zum Clip-Ende im normalen Kampf (es war fehlerhaft); die Videos 2 bis 4 zeigen noch den Stand
mit Rueckzug beim Ausholen, Video 5 den Endstand.

### STAGE5: Riesenform - Bewegung, Angriffe und Rueckstoss-Kamera (Spur Form, Stufe 3)

Die eigene Riesenform (Form J; Kopf nach Ihrem Entscheid E22 krokodil- statt nilpferdartig, woertlich "Es wirkt eher
krokodilmaessig. Hier sieht es eher hippomaessig aus....") ist seit dem 8.11. auf master. Der Ablauf folgt Ihren
Entscheiden E5/E6 (Port-Wahl): In 5080/5081 kaempft sie auf dem Bahnsteig, und wenn ihre Lebenspunkte verbraucht sind,
flieht sie mit Abdrehen, Galopp und Sprung - Treffer halten die Flucht nicht auf. In 5090/5091 steht sie ab dem
Betreten im Wagen (eine Mensch-Birkin und eine Mutation gibt es im Zug nicht mehr), wird zurueckgedraengt und aus dem Zug
geschossen; danach folgt der G5-Auftritt. Sie hat in beiden Raeumen 1000 Lebenspunkte (Waffenwerte siehe
"Waffenschaden"). START waehrend ihres Griffs oeffnet den Statusschirm nicht mehr, nichts friert ein (Leon und Elza
gemessen).

Vier Abnahmen; die vierte (7.11.) ist bestanden. Abnahme 2 fand die Rueckstoss-Szene nach dem Treffer mit der falschen
Kamera (Leons Ruecken fuellt das Bild, die Form dahinter, bis 100 % fast schwarz). Abnahme 3 fand die Szene behoben, aber
eine zusaetzliche Kamerawache machte den Kampf davor schlechter (38 statt 7 fast schwarze Bilder, Flattern zwischen zwei
Kameras); Runde 2 hat diese Wache auf den vorherigen Stand zurueckgesetzt. Abnahme 4: In 13 Szenenlaeufen mit Leon und
Elza ist die Form in jedem Taumelbild zu sehen und taumelt erkennbar; kein Szenenbild ist ueber 60 % fast schwarz
(hoechstens 54,6 %), der Spieler bedeckt hoechstens 7,6 % des Bildes, 2 bis 3 Schnitte je Lauf ohne Flattern; die
Kampfkamera vor der Szene ist Bild fuer Bild die vorherige. Zwei Kamerastellen zeigt Ihnen die Abnahme zur Wahl - siehe
"Zu entscheiden".

**Nach dem Zusammenfuehren (8.11.).** Mit 1000 Lebenspunkten und der neuen Textur wurde alles noch einmal gemessen; die
Riegel pruefen den Kampf jetzt mit dem Raketenwerfer und echten Spieler-Lebenspunkten. Drei Dinge koennen Sie merken:
Erstens gehen in der Suedwestecke von 5090 Schuesse auf die Form ins Leere, wenn eine Wandzelle des Raums zwischen Ihnen
und der Form liegt - das ist die Sichtlinien-Pruefung des Originals (in der EXE und in der Raumdatei nachgelesen), kein
Port-Fehler; wer von den Angriffen der Form dorthin geschoben wird, muss sich neu stellen. Zweitens sind einige
Kampfbilder dunkler als mit dem alten Kopf K4 - allein wegen der dunkleren Textur, bei gleichem Schnitt und gleicher
Lage (beim Auftritt 602 bis 628 statt 559 bis 596 Promille fast schwarze Bildpunkte). Drittens stuerzte mit der neuen
Textur der Software-Renderer ab, den nur die Testlaeufe benutzen; Ursache war ein Zahlenueberlauf im Zeichner der
Grafikbibliothek bei der 4096 Zeilen hohen Textur. Behoben: Nur im Software-Pfad werden die betroffenen Dreiecke
geteilt; Ihr beschleunigter Renderer zeichnet Bild fuer Bild wie vorher (0 Pixel Abweichung).

### STAGE5: Riesenform - Texturen nach Ihrer Vorlage (Spur Form, Farbe)

Die Kopfform K5c haben Sie am 6.11. gewaehlt ("eher c"); die Farben und die Struktur lehnten Sie ab ("wie ein Pokemon",
FARBE.md Z. 33). Drei Versuche am 7.11.: Versuch 1 und 2 (Malen aus den Screenshots) bestanden die Pruefer nicht; zu
Versuch 3 (Uebertragung der Original-RE2-G4-Haut) sagten Sie: "Das koennte schon passen mit der riesen Form ... schwarz
rot ... verbrannt ... Gesicht ... in flammenfarben" und nach Runde 3: "Naja, passt von den farben her nicht ganz der
vierbeiner, zu viel rot im Vergleich zur Vorlage, aber immerhin bedrohlich...." (Protokoll vom 7.11., 16:50). Ihr Auftrag
dazu (17:10, woertlich): "bist du noch an der Verbesserung von k5c ran, und entfernst die uebertriebenen roten
feuerrillen wieder, und stellst es wieder nach vorlage um?" und (17:30): "die Kopfform und alles von den neuen Feuerding
ist gut, aber die Texturen sind eben uebertrieben und falsch und entsprechen nicht der Vorlage. Ich will schon die
Vorlage darstellen!"

**Zwischenstand vom 7.11. abends (E30, woertlich):** "Nein, wie lassen es so wie es jetzt ist. Sieht doch gut aus.
Auch die Leuchtstelle sieht gut aus." Damit kam Versuch 3 Runde 5 (K5c) ins Spiel: die uebertragene Original-RE2-G4-Haut
ohne Feuerrillen, der Rotanteil je Koerperregion nach Ihren beiden Vorlagebildern, die Leuchtstelle am Becken. Der Kopf
K4 ist als Variante gesichert.

**Feinschliff vom 8.11. (K5d, nicht eingebaut).** Ihr Auftrag (woertlich): "Bei der 4er Form fehlen jetzt wieder ein paar
Details, die vorher da waren, da wuerde ich noch mal ran. Im Vergleichsfoto ist die Schwanzspitze auch so fleisch
roetlich, bei uns ist es einheitlich grau/blau. An den Verbindungsstuecken der Beine und Arme ist es im Vergleichsfoto so
weisslich, bei uns einfach grau/blau. Die Lippe hat teilweise komische roetliche Stellen statt durchweg grau/blau zu sein.
Auf der Vorderseite sieht man eine rote fleischige Stelle auf dem huckel, die fehlt. Und ich wuerde es insgesamt einw
wenig heller machen, das es zumindest die Helligkeit der Vergleichsbilder hat." und "Und die krallen bei der
Vorderseite stimmen nicht. Weder optisch - da sind es jetzt irgendwie zu viele - noch farblich - wo kommt denn da jetzt
ploetzlich dieses rot her im Vergleich zum Original Bild?" Die Variante K5d setzte die Details als Flecken an die hellen
Stellen der Videobilder. Ihr Urteil (woertlich): "Das einzige was wahrlich furchtbar ist.... Die Flecken an der Textur
sehen aus wie drauf gemalt.... nicht so sauber wie bei den rest des spieles" / "Alles wirkt unruhig und so frei
verteilt." / "Wie als haette das Modell Hautprobleme. Alles bunt." und "Nein, unterkiefer soll aussen natuerlich auch nicht
leuchten sondern dunkel/grau sein." Die Ursache lag bei mir: K5c gab neun Koerpergruppen je einen eigenen, am Video
ausgerichteten Farbton, und K5d legte am Video geeichte Flecken darueber. Beides widerspricht der Regel, Texturen nicht
an Videobildern zu eichen.

**Stand 9.11. - K5e "ruhige Haut" (E32).** Ihr Wort zur Vorlage (woertlich): "Warum hast du es jetzt beim 4 beiner um
den Fleck herum hinten am hintern schwarz gemacht? also um den roten Fleck herum? ansonsten passt, baue das ruhig
einfach mal alles so ein." In der Fassung vom 9.11. stand K5e im Spiel (in v0.8.30 ersetzt durch K5g3, siehe den
naechsten Abschnitt):
* Eine Hautfarbe fuer den ganzen Koerper statt der Gruppenfarben. Die Schattierung ist die Original-Haut des RE2-G4, kein
  Bildpunkt ist neu gemalt. Farbfremde Flecken je 10.000 Bildpunkte: 0 (K5c 16, K5d 17, Original-G4 3,5).
* Ihre Wuensche als ganze Flaechen: hellere Gelenke, fleischroetliche Schwanzspitze, fleischrote Stelle vorn auf dem
  Buckel, Lippe und Unterkiefer grau ohne Leuchten, Krallen vorn eingezogen (in Ihrem Bild 2 rechts eine, links zwei) und
  in Handfarbe, insgesamt heller (Ihr Bild 2 so hell wie die Vorlage).
* Der schwarze Ring um die rote Stelle am Becken ist weg: Der Rand bekam die neue Hautfarbe, der rote Kern blieb.
  Nachgemessen war der Ring vorher 7,9 L* dunkler als die Haut ringsum, jetzt ist er 4,5 L* heller.
* Die Textur hat 10 statt 16 Farbzeilen (im Port 512 x 2560).
Einbau-Pruefung (Zahlen und Bild) bestanden; Form-Riegel 11/11, Birkin-Riegel 4/4, Unit 1014/1014; die Form laedt in 24
Spiellaeufen ohne Fehlzeile, auch mit dem Software-Renderer.

### STAGE5: Riesenform - Hals, rechte Hand, Vorderzaehne, Haut K5g3 (Spur Form, 9. bis 16.11.)

Ausgangspunkt war Ihr Urteil zu K5e im Spiel am 9.11. (woertlich): "Und der 4 Beiner sieht von der Textur her jetzt
absolut furchtbar aus. Komplett augeblasst ... Null wie die restlichen Gegner im Spiel. Es wird doch irgendwie moeglich
sein den passendere Texturen zu geben wo er nicht ganz so plastisch blass aussieht...." und "Und auf der Unterseite am
Hals hat er komische rote streifen, die sollten da auch nicht sein." (Memory reai-v2-runde41-laufend.md, 9.11.). Danach
haben Sie das Modell selbst Stueck fuer Stueck beschrieben ("So, jetzt habe ich alle Abschnitt die Sichtbar sind
vollstaendig beschrieben") und zur rechten Vorderhand gesagt: "vorne rechts hat er wahrscheinlich auch 4 krallen. Man
sieht es nur nicht wegen der Schrift." (ebd.).

Der Weg in Kuerze: Eine Variante "nicht blass" (K5f) bestand die Pruefung nicht und kam nicht ins Spiel. Eine
Farbeichung ueber Leon, Annette und den Raumhintergrund zeigte, dass die Videofarben des Wesens zu 75 bis 80 %
Streulicht sind und als Zielfarben nicht taugen; belastbar blieb nur "Haut wenig bunt, Ton waermer". Ihre Zerlegung
wurde auf das Modell uebertragen (Grundlage v2, unabhaengig geprueft). Daraus wurden vier Teile:
* **Hals und Nacken - Ihr Ja vom 14.11.: "ja, lass so".** Die grossen roten Platten und Streifen an der Hals-Unterseite
  und die graugruenen Keile von der Schulter in den Kopf sind weg (neue Halsgeometrie B4). Dafuer bleiben kleine Reste
  (siehe "Noch offen"). Vier Bauer-Runden; die letzte liess der Pruefer nicht bestehen, empfahl aber, Sie am fairen
  Vergleichsblatt (Ordner 25) entscheiden zu lassen.
* **Rechte Vorderhand - Ihre Wahl vom 15.11.: "Variante1".** Nach neun Runden standen zwei Varianten zur Wahl (Ordner
  31); Ihr Hinweis vom 14.11. dazu: "ok. Orientiere dich einfach mehr an die linke Hand....". Variante 1: Pfote mit vier
  Spitzen wie links (eine Kralle an der Spitze, drei auf der langen Seite), Schulter und Kopf wie vorher; die Pfote
  kommt schraeg unter dem Kiefer hervor, oben in Ihrer roten Armzone ist kein Arm, rechts neben dem Kiefer steht etwas
  ueber. Beim Einbau behoben: In einer Bewegung steckte die Hand im Boden, in einer anderen der Kiefer tief in der Hand.
  Die rechten Krallen haben wieder die lange Form von K5c statt der Stummel.
* **Vorderzaehne - Ihre Frage vom 16.11.: "Warum hat denn der vierbeiner Birkin ganz vorne im Maul keine Zaehne?", Wahl
  "Oben und unten".** 16 neue Zaehne: oben je 5 links und rechts und einer in der Mitte, unten je 2 und einer in der
  Mitte; die Zahnreihen laufen ohne Luecke um die Schnauzenspitze wie beim RE2-G4, mit Form, Farbe und Licht der
  Seitenzaehne. Die unteren sind bewusst kurz (ohne Sie entschieden): Laengere stachen von schraeg unten in mehr als der
  Haelfte der geschlossenen Posen durch die Oberlippe. Ihr Wort: "Sieht doch in Ordnung aus mit den Zaehnen" (Memory
  reai-v2-birkin-zwei-versuche-sonst-rueckbau.md).
* **Haut K5g3.** Auf der neuen Geometrie neu texturiert aus der Original-Haut des RE2-G4, ohne Eichung am Video: dunkel
  rotbraun mit sichtbarer Hautstruktur statt grau-violett und flach, die braune Musterung mit Maserung, kein rotes
  Fleisch mehr an der Hals-Unterseite, keine fast weissen Punkte, zwei kleine helle Augenpunkte statt grosser gemalter
  Augen. Drei Runden; die dritte haben beide Pruefer bestanden ("kein roter Punkt" bzw. "bestanden"). Diese Haut kam
  zusammen mit Ihrer Hand-Wahl ins Spiel - ein eigenes Ja von Ihnen zur Farbe gibt es nicht. Zu den Vergleichsbildern
  mit den beiden 80%-Screenshots (Ordner 46) sagten Sie am 17.11.: "Naja, die Form sieht doch gut aus. Bei der Farbe
  weiss ich es nicht." - siehe "Zu entscheiden".

**Wie geprueft:** Den Einbau der Hand haben zwei Pruefer bestanden ("BESTANDEN (kein roter Punkt)" und "BESTANDEN - kein
roter Punkt"; Geometrie-Riegel 14 von 15 und Textur-Riegel 48 von 48 gruen, die Gegenproben rot; ctest 1408 von 1411,
die drei Roten haengen nicht am Einbau). Die Vorderzaehne bestanden in Runde 2 beide Pruefer ("Bestanden, kein roter
Punkt."); in keiner der 771 Posen sticht ein neuer Zahn durch Kinn oder Schnauze. Das Modell hat jetzt 15249 statt 6386
Dreiecke; damit keines verworfen wird, hat der PC-Port seine Dreiecksliste von 8192 auf 16384 Plaetze vergroessert
(Puffer des Ports, kein Spielwert) - im Kampf in 5090 hoechstens 7770 Dreiecke je Bild, keines verworfen. Die Textur hat
jetzt 15 Farbzeilen (im Port 512 x 3840). Im Vergleich mit den beiden 80%-Screenshots deckt sich der Umriss in Bild 1 zu
77 % bei fast gleicher Groesse, in Bild 2 nur zu 56 % (dort Feuer und ein dunkler Schattenriss im Video); fuer einen
Farbvergleich taugen die Tafeln nicht.

### ROOM5090/5091: G5-Auftritt im Zug nach RE2

Ihr Auftrag vom 2.11.: "Die finale Mutationsstelle beim Endboss trifft es noch nicht ganz. nach dem raus schiessen von
Birkin aus dem Zug, sollst du die letzte Stufe GENAUSO machen wie bei Resident Evil 2. Bei Resident Evil 2 ist es so -
man hat eine Grossaufnahme an seitlich der offenen Stelle des Zuges, und dann kommen da zunaechst die Tentakel vom
finalen Birkin G5, und der zieht ihn dann rein. Schau es dir an!" (Dossier G5_auftritt.md Z. 3-6). Vorlage ist die
RE2-Szene im Zug (Leon-Disc), gegen eine echte RE2-Aufnahme und 26 RE2-Spielstaende nachgemessen; fuenf Abnahmen, die
vierte (Szene) und die fuenfte (Nachbesserungen 5 und 6: Tunnellampen, Beben, Truemmer, Vibration - nach dem Merge
nachgeholt) bestanden. Leon und Elza verhalten sich gleich; einziger Unterschied: In 5090 steht die Frau im Kittel, in
5091 nicht.

**Ablauf.** Sobald die Riesenform aus dem Zug geschossen ist, bebt das Bild kurz (mit Beben-Ton aus RE2, von der Decke
fallen Brocken und Staub wie in RE2, mit Gamepad vibriert es nach dem RE2-Muster), und das Bild schneidet hart in die
RE2-Grossaufnahme: schraeg von unten auf die offene Wagentuer am Westende, mit RE2-Hintergrund und RE2-Masken. Durch das
Tuerloch ziehen RE2-Tunnellampen mit Lichtband, bis die Masse das Loch zudeckt. Ihre Figur laeuft wie in RE2 von hinten
zur offenen Tuer; kurz darauf beginnt die Armierung, und bis zum Ende der Szene koennen Sie nichts eingeben. Die Figur
steht kurz und laeuft nach rechts aus dem Bild; der erste Tentakel greift von links ueber ihren Kopf; drei eingerollte
Arme liegen an Rahmen, Decke und Boden deckungsgleich mit RE2; Birkin-Laute wie RE2. Die Masse schiebt sich unten links
hinter dem Tuerpfosten herein und waechst nach rechts, mit schraeger Stachelreihe, baeumt sich auf und wirft oranges
Bodenlicht wie RE2. Die Arme ziehen ein und stossen als lange Speere vor. Zweite RE2-Einstellung, niedrig nach Westen
auf das Wagenende; die Figur geht rueckwaerts aus dem Bild, der G5 kommt aus der Tiefe heran, bis nur Masse und Maul das
Bild fuellen. Dritte Einstellung: RE1.5-Kamera 12 statt der RE2-Schnitte 5 und 6 (Port-Wahl, es werden keine
Hintergruende erfunden); die Figur geht rueckwaerts auf die Kamera zu, die Arme wachsen in derselben Zeitfolge wie in
RE2 nach. Dann Sperre aus, kein Bildsprung in den Kampf. Gesamt gut 20 Sekunden. Die bisherige Pause vor dem Typwechsel
entfaellt - RE2 hat zwischen dem Verschwinden der Masse und der Armierung keine.

**Kampf.** Fair und gewinnbar: Wer wie in RE2 direkt vor dem G5 stehen bleibt, wird gebissen - in RE2 wie im Port.
Pistolentest (12 s zurueckgehen, dann Pistole): G5 tot, Spieler-HP 45, bei Leon und Elza. Stehend mit gehaltenem Feuer:
Nach etwa 7,5 s schiebt ein Arm die Figur ein Stueck nach Osten ohne Schaden, erster Treffer nach 25,7 s, G5 tot nach
29,6 s bei HP 18. Die Musik bleibt RE1.5 (RE2 senkt dort seine Musik; Port-Wahl). Die Lebenspunkte des G5 (600) und die
Raketen- und Granatenzahlen stehen unter "Waffenschaden".

**Zwei Reste aus der Abnahme 5 (nicht blockierend):** Wer mit dem Raketenwerfer aus der Rueckstoss-Szene heraus feuert,
sieht eine noch fliegende Rakete samt Rauchspur in die Grossaufnahme kreuzen und im Tuerloch explodieren (etwa 0,4 s nach
dem Schnitt); RE2 zeigt dort nichts - siehe "Zu entscheiden". Die schwarzen Szenenbalken blenden erst mit dem Schnitt ein
(RE2 hat sie schon davor); im Beben ist dadurch zwei Bilder lang oben ein schmaler schwarzer Rand sichtbar.

## Zu entscheiden

So ist es jetzt gebaut; ein Wort von Ihnen genuegt, um etwas zu aendern.

Fragen an Sie (je mit dem, was Ihre Wahl fuer das Spiel aendert):
* **Drehscheiben im Panel 2030 (Spur PF, Hinweis H2 der Abnahme 2).** Der Querbalken der Drehscheiben steht jetzt bei
  26,6 Grad statt etwa 30 Grad - eine Folge der scharfen, filterlosen Pixel (Wortlaut der Abnahme: "Flach mit hartem
  dunklem Rand, Teilstriche als ganze Texel, Querbalken 1:2 (26,6 Grad) statt rund 30 Grad ... ob sie dem Nutzer gefaellt,
  kann nur er beurteilen."). Lassen Sie es so, bleibt das Panel wie abgenommen; wollen Sie den alten Winkel, wird die
  Scheibe neu gezeichnet und das Panel noch einmal gemessen.
* **Zug 6030, Weg "vorne, ohne Wagen, dann Tor" (Integration, Weg F0).** Gehen Sie zuerst nach vorn zu Annette, nicht in
  den Wagen, und schalten dann das Tor in 6000: Beim naechsten Betreten des Fuehrerstands sagt Leon "There's a little
  girl in the back... Sherry.", obwohl er John und Sherry nie getroffen hat - die Wagen-Szene laeuft nach dem Tor nie
  mehr, so schaltet es das Original-Skript am Torschalter. Danach folgt der Zugstart. So lassen heisst: ein schiefer
  Satz auf diesem Weg; aendern heisst: Auf diesem Weg entfaellt der Nachtrag oder bekommt eine eigene Zeile, die Sie
  aufnehmen muessten.
* **Zug 6030, Zug erst nach dem Tor betreten (Integration, Weg N).** Betreten Sie den Zug zum ersten Mal erst nach dem
  Tor, laufen weder die Wagen- noch die Fuehrerstand-Szene (Original-Skript), und der Zugstart kommt sofort: Annette
  ruft "...Sherry is back there!", ohne zu wissen, dass Sherry im Zug ist. So lassen oder fuer diesen Weg eine eigene
  Fassung?
* **Zug 6030, Zugstart erst beim naechsten Betreten (Integration, Weg F, behoben).** Auf dem Weg "vorne, Wagen, ohne
  Rueckkehr zum Tor" kommt nach dem Tor zuerst Leons Nachtrag, der Zugstart erst beim naechsten Betreten des
  Fuehrerstands. So lassen heisst: ein Betreten mehr bis zum Zugstart; sonst kaeme der Zugstart direkt nach dem Nachtrag.
* **Annette-Radius (Spur AN, Abnahmen 4 und 5).** Die lebende Annette hat in 6000, im Fuehrerstand und in 5090 vor der
  Szene ihren RE1.5-Abstand (Leon kommt ihr bis etwa 900 nahe); erst ab der Szene und als Leiche gilt das kleinere
  RE2-Mass (Leon kommt der Leiche bis etwa 800 nahe; sie haelt ihn erst dort auf, die lebende Annette schon bei 900).
  Bestaetigen Sie diese Zweiteilung, bleibt es so; wollen Sie ueberall das RE2-Mass, kommt Leon der lebenden Annette
  naeher als im Original.
* **Deckentropfen 2000/2001 = Variante B (Spur W, Abnahme 3).** Die Tropfen-Schleife laeuft wie auf der 40%-Disc nicht;
  damit ist auch der pulsierende Bildpunkt am Bildrand weg. Zur Wahl standen (Dossier W_wasserblink.md Z. 492-497):
  "(A) Tropfen bauen wie im Repo-Baum ..." - "(B) Wie die 40%-Disc: Die Schleife ... laeuft nicht. Damit verschwindet auch
  der Fleck am Bildrand." - "(C) Bleibt wie jetzt: Die Tropfen haengen, und der Fleck am Bildrand bleibt." Bestaetigen Sie
  B, bleibt der Kanal ohne Tropfen; waehlen Sie A, fallen in 2000/2001 Tropfen von der Decke wie im Mod-Skript, und
  dieselbe Fallroutine wuerde fuer 28 weitere Raeume gebaut.
* **Rakete fliegt in die G5-Szene (Abnahme 5 des G5-Auftritts, R1).** Feuern Sie mit dem Raketenwerfer, waehrend die
  Riesenform aus dem Zug geschossen wird, kreuzt die Rakete noch etwa 0,4 s lang die Grossaufnahme und explodiert im
  Tuerloch; RE2 zeigt dort nichts. So lassen heisst: nur mit Raketenwerfer sichtbar und nur bei Dauerfeuer; aendern
  heisst: die Szene raeumt fliegende Geschosse beim Schnitt weg.
* **Vibration mit Gamepad (Abnahme 5 des G5-Auftritts).** Die Vibrationsfolge beim Beben ist aus der RE2-Disassembly
  nachgerechnet und an der exe gemessen (23 von 23 Werte), aber kein Pruefer hatte ein Gamepad in der Hand. Bitte beim
  Spielen mit Pad pruefen, ob das Beben spuerbar ankommt; ohne Pad passiert nichts.
* **Riesenform, zwei Kamerastellen in 5090 (Abnahme 4 der Stufe 3, "dem Nutzer zeigen").** (a) Steht Leon nach dem Wurf
  der Form nah vor Kamera 12 im Wagen B, zeigt der Raum 53 bis 62 Bilder lang (rund 2 s) seine Zonenkamera 13 mit der
  Form gross im Bild; die andere Kamera des Raums (12) zeigt dort Leons Ruecken ueber ein Drittel des Bildes. So lassen
  heisst: die Original-Zonenkamera (Port-Wahl); aendern heisst: Kamera 12 mit dem grossen Spielerruecken. (b) Weicht der
  Spieler im Kampf mehr als etwa 2,5 s zurueck, schaltet der Raum auf seine Aussenkamera 14 (Blick von ausserhalb des
  Wagens, Figuren klein, Bild dunkel) - die Original-Regel des Raums. So lassen oder eine eigene Kamera-Wache dafuer
  (eigene Runde)?
* **Alligator an der Rampenkante (Spur G2, Fragen F1/F2 der Abnahmen 7 und 8).** Rammt der Alligator die Figur oben auf
  der Rampe, stoesst er sie jetzt ins Wasser (wie gewuenscht) - in den Laeufen der Abnahme jedes Mal; beisst er von
  unten hinauf, taumelt sie nur und bleibt oben. RE2 hat im Alligator-Raum keine Rampe, die Stossrichtung ist eine
  Port-Wahl ohne Ihren Wortlaut. Ist sie fuer Sie in Ordnung? Und: Waehrend des Sturzes taumelt die Figur in der
  RE2-Haltung weiter (kein eigener Fall-Clip, weil RE2 dort keinen Sturz kennt) - ist dieser Anblick fuer Sie in
  Ordnung?
* **Alligator-Boss mit einer Granate geschwaecht (Spur WS, Abnahme 1).** Mit dem RE2-Wert und der neuen Granate startet
  eine Handgranate sofort die Kanister-Szene; mit der Pistole sind es 35 statt 43 Schuss. Das folgt aus "mach wie
  resident evil 2", widerspricht aber Ihrem frueheren "Ja, nicht einfach mit einer Granate beenden." (Handgranate, Punkt
  4). So lassen heisst: die Granate ist im Alligator-Kampf die Abkuerzung; aendern heisst: ein eigener Granatenwert nur
  fuer den Alligator-Boss, dann waeren es wieder zwei Treffer. Ab v0.8.31 gilt Ihre Rangfolge vom 16.11. auch fuer den
  Schaden (siehe "Vorschau"); dann wird dieser Wert neu gesetzt und seine Folge fuer den Alligator ausgewiesen.
* **Alligator dicht an der Westwand schwimmt einmal am Kanister vorbei (Nutzerbefunde 9.11., Punkt K-1).** Steht die
  Figur dicht an der Westwand, schwimmt der Alligator ihr nach und kommt seitlich 2292 bis 2354 am Kanister vorbei;
  aufgenommen wird nur bis 1733 seitlich. Er nimmt ihn dann beim naechsten Anlauf, je nach Lauf bei Bild 526 bis 1651 -
  das sind etwa 18 bis 55 s nach Beginn des Messlaufs. Gemessen (Abnahme 3): 9 von 18 gezielten Laeufen, alle an der
  Westwand, 0 von 7 an der Plattformseite, 0 von 20 Standardlaeufen; "diverse Runden" liessen sich nicht mehr erzeugen.
  In einem weiteren Lauf an der Westwand verpasst er keine Passage, nimmt den Kanister aber erst bei Bild 2348 (etwa 78
  s): Die Bisse ziehen die Figur nach Sueden, der Alligator folgt ihr und kommt erst spaet zum Kanister. Das Fenster ist
  3466 breit, der Kanal 7200. Herkunft des Fensters: Aus RE2 belegt ist nur sein fernes Ende voraus (@0x80100ae4-e8).
  Sein Anfang am Alligator selbst - im Spiel gilt es der Laenge nach von 0 bis etwa 6000 voraus - ist seit der Spur G2
  Port-Wahl nach Ihrem Punkt 8 vom 1.11. (G2_gatorfix.md Z. 13: "erst wenn er beim Kanister ankommt, aufnehmen"). Die
  Breite zur Seite rechnet der Port aus Koerperradius des Alligators plus Kanisterradius (1466 + 267) und fuehrt sie im
  Code als Port-Wahl vom 23.10., weil RE2 den Alligator auf einer festen Spur fuehrt; die Abnahme 3 nennt das ganze
  Fenster "RE2-Wert". So lassen heisst: an der Westwand kann eine zweite Runde noetig sein; aendern heisst: ein
  breiteres Fenster zur Seite - dann nimmt er den Kanister auch auf, wenn er neben ihm vorbeischwimmt.
* **Schluessel bei der Leiche ist klein - bitte im Spieltest ansehen (Nutzerbefunde 9.11., Punkt K-2).** Am Todesort des
  echten Kampfs ist er als Schluesselring erkennbar (etwa 9 x 11 Bildpunkte). An den anderen ausgezaehlten Todeslagen
  ist er sichtbar und blinkt, aber fuer einen Spieler nicht als Schluessel zu erkennen: Todesrolle 8 Bildpunkte (Kasten
  8 x 5), Tod an der Plattform 4 bis 5. Zwei weitere Lagen hat nur die Abnahme 2 ausgezaehlt (dieselben Lagen, in der
  Abnahme 3 nicht neu ausgezaehlt): Tod im Kanal (Lauf tod_kanal_n) etwa 7 helle Bildpunkte, Tod an der Westwand 2 bis 3
  pulsierende Bildpunkte. Die kleinste Lage der Riegel zeigt 3 x 2 Bildpunkte; der Bericht fasst zusammen: "Schluessel 2
  bis 8 Bildpunkte breit". Gerechnet ueber 208 Todeslagen liegen 205 unter der frueheren Bildschwelle von 10,4
  Bildpunkten; das mittlere Bild ist von 9,8 auf 4,8 Bildpunkte geschrumpft, das kleinste von 6,3 auf 2,9. Das ist die
  Kehrseite Ihres "naeher bei der Leiche". So lassen heisst: nahe bei der Leiche, dafuer klein; aendern heisst: Sie
  nennen eine Mindestgroesse, dann liegt der Schluessel in manchen Lagen wieder weiter von der Leiche weg.
* **Endkampf mit dem Raketenwerfer (Spur WS, H11).** Der Endkampf in 5090 braucht 8 Raketentreffer (Riesenform 4, G5 4;
  die Abnahme zaehlte im alten Ablauf mit der Mensch-Birkin davor 11), der Raketenwerfer liegt aber in keinem Raum des
  Spiels - diese Zahlen erreicht nur, wer die Debug-Waffen nutzt. Lassen Sie es so, aendert sich fuer den normalen
  Durchlauf nichts; wollen Sie den Werfer im Spiel (Ihre Vormerkung fuer v0.8.31 sieht ihn im Finale vor), braucht er
  einen Fundort und Munition. Der Plan fuer v0.8.31 legt ihn fuers Finale in die Asservatenkammer 1110
  (11_story_v0831/GEGENSTAENDE.md).
* **"Where's Annette?" bei "vorne zuerst" (ST80, Abnahme 2, Frage 2).** Gehen Sie im Zug zuerst nach vorn zu Annette und
  dann in den Wagen, fragt Leon dort weiterhin "Where's Annette?", obwohl er gerade bei ihr war (Runde 36 und Abnahme 1
  hatten das vermerkt). So lassen: ein schiefer Satz in dieser einen Reihenfolge; aendern: eine eigene Wagen-Zeile fuer
  diese Reihenfolge - die Sie dann aufnehmen muessten.
* **Nachrichten-Id 40 auf PSX nicht anzeigbar (ST80, Abnahme 2, N2).** Leons Nachtrag-Zeile "There's a little girl in the
  back... Sherry." traegt die Nummer 40; die PSX-Fassung kennt in diesem Raum nur Nummern bis 31, und alle kleineren sind
  belegt. Auf dem PC ist alles zu sehen; auf der PSX wuerde in dieser Reihenfolge nur Annettes Antwort erscheinen. Die PSX
  baut derzeit ohnehin nicht; wenn sie es wieder tut, muss die Nummerntabelle dort wachsen.
* **Riesenform - die Farbe (K5g3, Ordner 46).** Im Spiel steht die Haut K5g3 (dunkel rotbraun, aus der Original-Haut des
  RE2-G4). Sie kam mit Ihrer Hand-Wahl ins Spiel; zur Farbe sagten Sie am 17.11.: "Naja, die Form sieht doch gut aus.
  Bei der Farbe weiss ich es nicht." Im Video ist das Wesen lila-blau mit rosa Stellen - laut Farbeichung zu 75 bis 80 %
  Streulicht der Umgebung, als Farbvorlage also nicht brauchbar. So lassen heisst: K5g3 bleibt; aendern heisst: Sie
  sagen, was an der Farbe stoert (z. B. heller, dunkler, roeter), und es wird aus der Original-Haut umgesetzt, ohne
  Eichung am Video.

Entscheide waehrend der Runde (Orchestrator bzw. Koordinator; Sie koennen widersprechen):
* **Umfang v0.8.30 = "Mit allem in Arbeit"** (Ihr Entscheid vom 7.11.): Alligator, Annette und die Riesenform sind
  abgenommen bzw. von Ihnen angenommen; am 11.11. Ihr "Nein" zu einer Veroeffentlichung ohne die beiden
  Birkin-Nachbesserungen. Drin sind die Sichel-Form V8 (Ihr Ja vom 15.11.) und die Riesenform-Teile vom 14. bis 16.11.;
  nicht drin sind die Story fuer v0.8.31, die Szene "von aussen" (Ihr Entscheid fuer v0.8.31) und die Option VOICE.
* **Alligator (Spur G2):** keine Kameragrenze fuer den Kopf (Ihr "noe, wozu wenn es gut funktioniert?"); oben auf der
  Rampe rammt und beisst er (Ihr F3); Haengen hoechstens 15 s ohne Fortschritt (Port-Wahl nach Ihrem "nicht zu lange");
  Trefferreaktion der Figur aus RE2 statt des RE1.5-Niederwurfs ("Original oder nicht"); kein eigener Kanister-Holmodus
  (Ihr "ganz normal seine runden drehen"); ein Treffer an der Rampenkante darf ins Wasser stossen (Ihr "er kann den
  Spieler ruhig In das Wasser stossen."); ein Treffer je Biss-Anlauf wie in RE2; das Rammen trifft einmal je
  Ueberquerung (Port-Wahl, RE2 trifft die Figur nur ueber den Biss).
* **Alligator (Nutzerbefunde 9.11.):** Kanister-Aufnahme auch im Spurt (Port-Wahl nach Ihrem "sobald die Position
  erreicht ist, wo der Behaelter liegt"; RE2 hat im Spurt weder Fenster noch Abbruch); der Spurt, den der Kanister
  abbricht, endet ohne Vierteldrehung (Port-Wahl; in RE2 gehoert sie nur zum normalen Spurt-Ende) und mit Ueberblendung
  32 (RE2 @0x80100c38, dort beim Eintritt ins Schwimmen); Kanister auf die Kanalmitte ueber die Wurfrichtungen 3342 und
  888 (Port-Wahl nach Ihrem "zentraler in die Mitte"; gewaehlt ist je Richtung die Landestelle mit dem kleinsten Abstand
  zur gemessenen Kanalmitte); Schluessel immer im Gebiet der Kamera, die ihn zeigt (Port-Wahl nach Ihrem Punkt 3), und
  am der Leiche naechsten erreichbaren Punkt (Ihr "naeher bei der Leiche"); die Bildschwelle 10,4 Bildpunkte entscheidet
  die Lage nicht mehr, sie wird nur noch gemessen; die Bildschranke der Riegel steht auf 2 x 2 statt frueher 10 x 9
  Bildpunkten (nur die Untergrenze "nicht ein einzelner Bildpunkt").
* **Waffenschaden (Spur WS):** Handgranate 251 fuer jeden Gegner, Rakete je Gegnertyp nach RE2 (Ihr "mach wie resident
  evil 2"); Alligator-Boss 25240 statt 25300 ("Original oder nicht"); Gorilla zaehlt als Boss, sonst waere die Granate
  361; Zuordnungen ohne RE2-Gegenstueck: Gorilla wie der Licker (200), kleiner Affe wie die kleinen Tiere (300),
  Gitterhaende wie die RE2-Arme (40).
* **Wasser an der Leiter (Nachtrag 30e) in ROOM2000/2001 statt 2010/2011** - 2010 ist das Buero ohne Leiter, Weg und
  Becken; Ihr Wortlaut und Ihr Entwurf vom 23.10. treffen 2000.
* **Spawn-Variante der Zombies fuer ALLE Zombie-Typen abgeschaltet** (Regel des Koordinators; Ihr Wort galt den dicken).
* **Gore-Port-Wahlen:** gelbgruenes RE1.5-Platzen statt der dunklen RE2-Drift im RE2-Gehirn; der RE1.5-Blutcode steht
  fuer die RE2-Blut-Effekte (RE2: olivgelb bis gruen, nie rot); Lache groesser als in RE2 (deshalb faerbt sie Leons Schuh
  gruen) - Abnahme 3: "Das gehoert in die Release Notes oder in die Entscheidung ueber die PORT-WAHL"; Federn nur an
  Treffer-, Tod- und Griff-Stellen; der Schalter gegen das 80%-Gore gilt nur fuer das Platzen und die
  MC51-Beinzelle.
* **Birkin-Lebenspunkte (Spur GR, Ihre Freigaben "1. ja" und "2. ja"):** Birkin-Skala verdoppelt, Riesenform 1000
  (RE2-G4-Wert). Die Boss-Regel der Granate ist durch WS ersetzt.
* **Key Items:** Die Welt laeuft an den 14 Stellen schon in der Einblende (noetig fuer die Kurbel); die Waffe kommt bei
  der Kurbel nach der Szene zurueck, nicht erst beim naechsten Oeffnen des Inventars wie in RE2 (vier Zombies im Raum);
  die Kurbel hat Name, Bild und Text aus RE2.
* **Panel 2030:** "SET 80" statt "PRESSURE 80", die Ziffern im Zeigerblatt entfallen, das Nachrichtenband ist dunkler
  (Port-Wahlen der Nachbesserung); Rahmen und EXIT aus den Original-Nahansichten, der Rest nach dem Echtbild gezeichnet.
* **Alligator (Spur P):** Lage der Leiche und Anlaufrichtung am Kampfbeginn (Messung im RE2-Band), Putz je Beckenseite der
  laufenden Kamera, Leiche im Maul mit der Rumpfmitte auf der Maullage, begrenzter Bildschritt mit Kanister, Kanister in
  RE2-Groesse (keine Umskalierung).
* **Toter Alligator:** Sperre erst beim Tod statt wie RE2 beim Kampfbeginn (der Water Key haengt an ihm).
* **Effekt-Quader 20B0:** Die PSX-Hardware-Grenze gilt auch im Spiel fuer die Effekt-Quader.
* **Bahnsteig-Birkin:** drei Regeln der Spieler-Klemme fuer die engere Bahn von 5090; Riesenform-Schaden und G5-Schaden
  auf 100 Port-HP halbiert; der Port haelt beim Kampfstart aus der Lage den passenden Schnitt (RE2 hat dort keine
  Kamera-Regel).
* **Riesenform Stufe 3:** Riesenform in 5080 (mit Flucht) und 5090 (ab dem Betreten, keine Mutation im Zug) nach Ihren
  Entscheiden E5/E6; die Rueckstoss-Szene waehlt ihren Schnitt nach der Lage von Form und Spieler aus den vorhandenen
  Kameras des Raums, keine erfundene Kamera; die Kampfkamera vor der Szene bleibt die bisherige (Runde 2 nach Abnahme 3);
  wo kein Schnitt passt, bleibt die Original-Zonenkamera (siehe Frage oben).
* **Riesenform Texturen:** Nach Ihrem E32 die ruhige Haut K5e: Original-RE2-G4-Haut mit einer Farbe je Material, die
  Adern als dunkle Struktur unter der Haut (Ihr "Feuerrillen raus"), keine Eichung je Region oder Fleck am Video; die
  Leuchtstelle am Becken bleibt (E30), ihr Rand folgt der Haut. In v0.8.30 ersetzt durch K5g3: Original-RE2-G4-Haut auf
  der neuen Geometrie, Braun mit Struktur, Hals-Unterseite ohne Fleisch, keine Hautstelle heller als L* 85; die
  Aufhellung je Seite bleibt groesser als die Original-Seitenhelligkeit, damit kein schwarzer Sattel entsteht. Die Haut
  kam mit Ihrer Hand-Wahl ins Spiel, ohne eigenes Ja zur Farbe. Die unteren Vorderzaehne sind kurz (Variante A, ohne Sie
  entschieden). Die Dreiecksliste des PC-Ports hat 16384 statt 8192 Plaetze (Puffer des Ports, kein Spielwert).
* **Software-Renderer:** Gegen den Ueberlauf werden Dreiecke nur im Software-Pfad geteilt; der beschleunigte Renderer
  bekommt die Dreiecke unveraendert.
* **Zug 6030 (Integration):** Ist auf dem Weg "vorne, Wagen, Tor" Leons Nachtrag noch offen, laeuft er nach dem Tor vor
  dem Zugstart - kleinster belegter Fix, keine neue Zeile, kein neues Speicher-Bit.
* **Sichel-Form:** Koerper der gelobten Referenz 2 als Grundlage; Schwanz und Kopf nach Ihren Beschreibungen, der Kopf
  seit dem 8.11. auf Basis des Original-RE1.5-Birkin-Kopfs; eingebaut in der Fassung V8 mit Schwanz 8g nach Ihrem Ja vom
  15.11.; der Kopf von V7 bleibt Grundlage, am Hinterkopf darf angefuegt werden; der Nacken-Kandidat R3 wurde genommen,
  obwohl ihn beide Pruefer formal nicht bestehen liessen (bester Stand); Handfarbe 7e statt eines roteren Fingerteils;
  Farben nur aus Original-EM30-Texeln, keine Eichung am Video.
* **Annette:** Szene nach dem Claire-Disc-Vorbild; Schuesse gehen durch lebende Begleiter und die Leiche hindurch
  (RE2-Regel); Leiche liegt flach mit dem RE2-Liegeclip; bei Elza keine Szene.
* **G5-Auftritt:** dritte Einstellung in RE1.5-Kamera 12 statt der RE2-Schnitte 5 und 6; die Musik bleibt RE1.5 -
  Abnahme 3: "vertretbar (PORT-WAHL, Uebertragung auf ein anderes Stueck unbelegt)"; Lampen laufen 4 Bilder vor, damit sie
  szenenbildgleich mit RE2 sind.
* **Gorilla-Filme:** In 11C0 spult das Spiel den Original-Auftritt der Kamera 12 unsichtbar vor und blendet erst mit der
  Kamera 5 ein (Orchestrator); der Film kommt vor dem Musikstart (Abweichung von der woertlichen Folge des Skripts); in
  1141 ersetzt der Schrei den Glasbruch der Runde 39.
* **Monitore und Wasser:** Die Eigenheiten des Originals (8 Plaetze fuer 11 Monitore) sind uebernommen; im Port bekommt
  jede Unterpalette einen eigenen Farbstreifen der Textur (SDL hat keinen Bildspeicher wie die PSX); der Kanal-Riegel
  der Spur W erlaubt Farbspruenge bis 16 (mit Ordner 33 von Ihnen abgenommen).
* **Zielhilfe:** Schrittweite von RE2 Retail (Beta -> Retail); die grosse Schrittweite fuer die Super Redhawk erkennt
  der Port an deren Clip-Laenge.
* **Boden-Schatten:** im Code behoben statt als bekannter Fehler vermerkt; die Diagonale der Schattenvierecke (PC 0-3,
  PSX 1-2) bleibt, weil eine Aenderung jeden Schatten aendern wuerde.
* **Pipe:** Rohr-Modell selbst gebaut aus der Messer-Datei, doppelt so lang wie das Messer (Port-Wahl, die Disc hat kein
  Rohr); Reichweite 1355 (Ihr Wunsch); Toene mit Birkins Welle und mit Stimme und Vorrang des Messers, Lautstaerke wie
  bei Birkin (127), auch das Ziehen klingt nach Rohr (Orchestrator).
* **Optionen:** ULTIMATE 1.5 ab Werk AUS, die Disc-Dateien in einem eigenen Ordner (Orchestrator); Zuordnung der
  Disc-Bilder nach Bildinhalt statt nach Nummer (nach dem roten Punkt der ersten Pruefung); die Zange als VIRUS-Ampulle,
  weil das der Disc-Stand ist.
* **Spider-Man-Szene und Hangar:** Blut = Original-Spinnenblut; die abgespreizten toten Glieder sind der Original-
  Sterbeclip und bleiben; Wurf mit Seitenlage und Gang am Ende nach Messung am Video (Port-Szene); "Szene gesehen" und
  "Hangar-Birkin besiegt" als neue Speicher-Bits 38 und 39 (Port-Wahl, im Original frei); der Original-Todesablauf haelt
  Birkin liegen, das Raumsignal, das den Kraftraum-Birkin streichen wuerde, bleibt im Hangar aus; der Monitor Room als
  Ort fuer das Monitorbild war ein Entscheid des Orchestrators (von Ihnen abgelehnt, zurueckgebaut).
* **Klauenhieb:** massgeblich ist der Weltkontakt der gezeichneten Klaue, keine Rechnung nach Sichtbarkeit im Bild
  (Orchestrator); die Treffer der Kralle rechnen am EM30-Gerippe (Port-Wahl); der Rueckzug hat Vorrang vor dem Zucken
  (Port-Wahl); die Gelaender-Oberkante 1900 und der Schlagbeginn bei Bild 32 sind gemessen, keine Werte des Originals.
* **Aufrichten:** die Clip-Ende-Regel aus dem Story-Zweig woertlich uebernommen und fuer den Kraftraum freigeschaltet.

Weitere Fragen aus der Runde, die noch niemand entschieden hat (so ist es jetzt):
* **Gore allgemein:** Ihre Rangfolge vom 16.11. (80% > 40% > RE2 fuer Gore und Schaden, Zombies spielerisch wie RE2)
  gibt die Richtung fuer die folgenden Gore- und Granaten-Fragen vor; umgesetzt wird sie mit dem Gore-Umbau in v0.8.31.
* **Gore, Tabelle 5b:** Was die RE2-Gehirne ueber Angriff und Treffer hinaus ersetzen sollen - vor allem das Klettern der
  Spinne, das unter der Standard-KI nie laeuft, und Fressen/Schlafen der Zombie-Familie.
* **Gore, Wuergearm:** Ob die abfliegende RE1.5-Hand in den RE2-Zellenarm gehoben werden soll.
* **Gore, Federn im Flug:** Ob die Kraehe wie in RE2 auch beim reinen Fliegen Federn verlieren soll (jetzt nur bei
  Treffer, Tod und Griff).
* **Gore, Federn unter "AI RE1.5" und bei der Ereigniskraehe:** nach Ihrer Regel "Gore = Maximum" eine Frage an Sie.
* **Gore, kopflos mit Nicht-Magnum-Waffe (Video):** erst die Waffe im Video sichern, dann Port-Wahl.
* **Granate, Kraehe:** Ihre Teile verschwinden nach dem Original-Ablauf, nicht "liegen bleiben" wie beim Zombie.
* **Granate, Werfer-Explosivrunde:** Der dicke Zombie und der Kanal-Zombie ueberleben sie, der RE2-Hund auch -
  unveraendert gelassen; Ihre Antworten galten der Handgranate.
* **Kanalwasser, ROOM50B0:** roter Bodenschein in Kamera 2 aus demselben Zwischenstand wie das Blinken (das Video zeigt
  dort ein rotes Band) - bleibt, bis Sie entscheiden.
* **Zombie-Modelle (Berichtigung):** Die Fassung vom 9.11. nannte hier eine Voreinstellung "RE2 AI + Model". Die gibt es
  seit dem 21.08. nicht mehr (Commit 215ed3198, Ihr damaliges Wort: "Mittlerweile ist RE 2 AI schon so gut, dass wir
  diese Option im Optionsmenue sowie seine eigenen Models entfernen koennen."); seither zeichnet auch die RE2-KI die
  RE1.5-Modelle. Nichts zu entscheiden.
* **Zufallsgenerator der Disc** als eigene Runde (aendert jeden Wurf).
* **Bahnsteig-Birkin, Gestaltung:** Soll der Griff der Form nach dem Wurf abgekuerzt oder die RE2-Opfer-Sequenz (Leon
  wird gehalten) gebaut werden? Soll die Rueckstoss-Szene kuerzer werden, wenn Leon weit oestlich steht und sich rund 14
  s nicht sieht? Soll fuer Leon unter dem G5 im Westen eine naehere Kamera gelten?
* **Story, Kenntnisnahme-Punkte B1 bis B8, B15:** "Widerspruch ja oder moeglich, aber nicht gravierend oder nicht
  belastbar belegt. Dateien nur, falls Sie angleichen wollen."
* **Story, Trailer-Orte:** Ihre Antwort laut Protokoll vom 2.11.: Bild 1 und 4 nicht noetig, Bild 2 vielleicht ein Gang in
  B1 oder B2 hinter dem Tor, Bild 3 einer der Flure 1F bis 3F - nicht gebaut.
* **Klauenhieb, toedlicher Schuss direkt nach dem Wandschlag:** Der Tod geht vor - Birkin faellt, die Klaue gleitet in
  10 Bildern aus der Wand (wie im Original); der Pruefer empfahl, das zu entscheiden.
* **ULTIMATE 1.5, Zange:** Bei AN ist die Zange eine stehende Ampulle mit der Aufschrift VIRUS - so steht es auf der
  Disc; aendern liesse es sich mit einer Zeile.

## Fuer Sie zum Aufnehmen

Ohne Datei laufen die Zeilen stumm mit Untertitel; der Port bindet jede Datei nach Raum und Nachricht
(synchro/STAGEn/roomXXXX/mainNN.wav, keine Umleitung). Laut Protokoll vom 3.11. haben Sie die Aufnahmeliste der
Story-Spur eingesprochen ("alle voiceovers ergaenzt bis auf Anette 5090"); die Abnahmen der Story-Spur haben mit diesen
Dateien gemessen. Sie sind seit dem 9.11. im Repo (Commit 010f833b9: die neuen Aufnahmen fuer STAGE2 bis 6 und
synchro/unused, Ihre geaenderte synchro/STAGE3/room3061/main05.wav, dazu die RE2-Claire-Stimmbank als Quelle der
Annette-Zeilen in 5090) und kommen ins Paket (es nimmt synchro/STAGE*, ohne unused/). Nicht im Repo ist weiter der
Ordner voice/Merged mit 18 neuen Dateien (der Commit nahm ihn ausdruecklich nicht mit) - bitte sagen Sie, ob er mit ins
Release soll. Was seit dem 9.11. dazukam, braucht keine Aufnahmen: Die Gorilla-Filme bringen ihren Ton mit, der
Sherry-Ruf und der Pipe-Ton sind Birkins Toene von der Disc. Die Aufnahmeliste fuer v0.8.31 (Story) haben Sie am 14.11.
bekommen (11_story_v0831/AUFNAHMELISTE.md); sie gehoert nicht zu dieser Fassung.

**Neu - fehlen noch (stumm mit Untertitel):**
* **Zug 6030, Nachbesserung der Story-Spur (Abnahme 2, Frage 1):** synchro/STAGE6/room6030/main31.wav - Leon: "Everyone
  we could find. Can you start this thing?" (wenn Sie zuerst nach vorn gehen) und main40.wav - Leon: "There's a little
  girl in the back... Sherry." (der Nachtrag beim naechsten Besuch vorne). Beide Saetze stammen aus Ihrer Zeile main12
  bzw. Ihrem ersten Wortlaut.
* **Annette-Szene, hinterer Wagen 5090 (Wahl, nicht fehlend):** synchro/STAGE5/room5090/main40.wav - Annette:
  "W-William! You're alive!"; main41.wav - Annette: Schrei ohne Untertitel; main43.wav - Annette: "William." - alle drei
  liegen seit dem 3.11. als Schnitte aus der Claire-Disc vor; wenn Sie sie selbst sprechen wollen, ersetzt Ihre Aufnahme
  den Schnitt. main42.wav - Leon: "Annette!" ist Ihre Aufnahme.

**Zur Kontrolle - die Liste vom 3.11. (schon eingesprochen; "bleibt" = die vorhandene Aufnahme spricht schon diesen
Wortlaut):**
* **Annette-Szene (Spur AN), Zugstart 6030 - eingesprochen am 3.11., seit dem 9.11. im Repo:**
  synchro/STAGE6/room6030/main27.wav - Annette: "There! The train's moving."; main28.wav - Annette: "What was that?
  ...Sherry is back there!"; main29.wav - Leon: "I'll check it out."; main30.wav - Annette: "No. She's my daughter. Stay
  here and keep an eye on the controls."
* **Alte Aufnahmen mit geaendertem Text:** synchro/STAGE3/room30E1/main02.wav - John: "This is bad... those monsters
  have us surrounded!"; main03.wav - John: "...Damn it, there's no place around here to hide...";
  synchro/STAGE6/room6031/ main00.wav - Elza: "Sherry! Is anyone else here?"; main01.wav - Sherry: "A policeman! He's
  fighting off monsters. He wants us to start the train."; synchro/STAGE5/room50B0/main01.wav - Ada: "Some other
  survivors are down there with her too."; synchro/STAGE6/room6000/main00.wav - Annette: "You must be that cop. Your
  friend is already here."; main03.wav - Annette: "Come on. Let's join the others and get out of here."
* **Elza trifft Marvin, ROOM30C1:** synchro/STAGE3/room30C1/main01.wav - Elza: "Who are you?" (bleibt); main02.wav -
  Marvin: "Marvin Branagh, RPD."; main03.wav - Elza: "Are there any others still alive?"; main04.wav - Marvin: "A few.
  There are other survivors out there."; main05.wav - Elza: "Then I hope they make it out." (main06 bis main08 werden
  nicht mehr gespielt).
* **Leons Zug, Wagen, ROOM6030 (Spielreihenfolge):** synchro/STAGE6/room6030/main00.wav - Leon: "Hang on, Marvin. We're
  getting out of here." (bleibt); main01.wav - Leon: "Who are you two?"; main21.wav - John: "Name's John. This is
  Sherry. We made it down here with a girl named Elza."; main02.wav - Sherry: "She's out there fighting the monsters!"
  (bleibt); main03.wav - John: "She's trying to keep the outside clear so we can escape."; main22.wav - John: "She said
  she'd catch up with us."; main04.wav - Leon: "Why aren't you out there?" (bleibt); main05.wav - John: "She told me to
  stay with the kid... and look after your friend here."; main06.wav - Leon: "Where's Annette?" (bleibt); main07.wav -
  John: "The woman in the lab coat? Up front, trying to start the train."; main23.wav - Sherry: "A lab coat...? Is it
  Mom?!"; main24.wav - John: "Easy, kid. Stay with me."; main08.wav - Leon: "I'll find out. Stay here."
* **Leons Zug, Fuehrerstand, ROOM6030:** main11.wav - Annette: "Is everyone aboard?"; main12.wav - Leon: "Everyone we
  could find. There's a little girl in the back... Sherry."; main25.wav - Annette: "Sherry...? My daughter is on this
  train? ...Thank God."; main26.wav - Leon: "Can you start this thing?" (main13 bis main19 bleiben).
* **Optional:** synchro/STAGE3/room3061/main05.wav - Sherry: "It's here! Look, I can get inside through this hole!"
  (genauer Capcom-Wortlaut aus dem 80%-Ton). Die geaenderte Datei ist seit dem 9.11. im Repo (010f833b9); ist das die
  neue Aufnahme, ist der Punkt erledigt. Die Panel-Texte von 2030 sind wie alle Port-Texte stumm; v0.8.29 hat die
  Port-Texte des Ventilkastens 2010 unter derselben Form (main40.wav bis main44.wav) als optional gelistet.
* Die uebrigen Punkte dieser Fassung brauchen keine Aufnahmen.

## Noch offen

Alligator (Spur G2)
* Takt zwischen zwei Biss-Anlaeufen (Punkt O1 der Abnahme 9, nicht blockierend): RE2 kann bei andauerndem Kontakt nach dem
  Ende der Trefferreaktion gleich wieder einen Biss-Anlauf beginnen; der Port wartet laenger (gemessen 107 Bilder zwischen
  zwei Bissen). In einer eigenen Runde gegen RE2 zu belegen.
* Ein Riegel des Alligator-Testprogramms setzt den Platz des Alligators in der Gegnerliste fest als 35 voraus (gering,
  Gegenpruefung vom 8.11.; im Spiel ohne Wirkung).
* 2 von 523 Treffern im Testprogramm (Biss von der Querung, danach Rammen) brechen eine liegende Trefferreaktion ab - nach
  RE2 zulaessig. Der Sturz von der Rampe hat keinen eigenen Fall-Clip (siehe "Zu entscheiden").

Alligator (Nutzerbefunde 9.11.)
* Auf dem Endstand lief nur die Alligator-Riegelgruppe (154 Riegel, gruen). Die Unit-, Eigenschafts- und Sonden-Riegel
  (ohne die Riegel fuer Releaseskript, Android und Titelversion, ohne Integrations-Riegel) liefen laut Bericht zuletzt
  auf dem Zwischenstand f8c71298a vom 9.11.: "99% tests passed, 5 tests failed out of 1104", alle fuenf
  Zeitueberschreitungen unter Last, einzeln 5 von 5 gruen. Die volle Suite lief erst mit der Abnahme von v0.8.30 (siehe
  Pruefstand).
* Grenzen der Abnahme 3: Die Aufnahme des Schluessels an der Todesrolle begann mit dem Testhaken der Todesrolle, nicht
  mit einem natuerlichen Kampf, und ohne Leitergang. Zwei weitere Todeslagen der Riegel (dort 3 x 2 und 7 x 5
  Bildpunkte) hat der Pruefer nur ueber den gruenen Riegel geprueft und nicht eigens am echten Weg nachgefahren; fuer
  beide nennt der Bericht des Fixers exe-Laeufe am echten Weg (aus den Protokollen seiner Riegel, keine Messung einer
  Abnahme).
* Schluessel aus der Ferne: Geprueft sind die Kamera der Einstellung nach dem Tod und die Kamera am Platz des
  Schluessels. Ob er aus den anderen Kameras von weitem zu sehen ist, ist nicht gemessen. Die Abnahme 1 hat das fuer
  zwei Kameras (Cut 6 und 7) als Hinweis offen gelassen - in ihren Aufnahmen waren dort Wasserspritzer an den Fuessen
  nicht sicher vom Schluessel zu trennen -, damals noch mit der frueheren Lage des Schluessels; die Abnahme 3 hat an der
  neuen Lage wieder nur die Kamera der Einstellung und die Kamera am Schluessel geprueft.
* Der Suedwurf (Richtung 888) ist nur mit Messhaken belegt: Im echten Ablauf fiel die Wahl in den Abnahmen immer auf den
  Nordwurf; ein Riegel waehlt ihn in zwei von fuenf Laeufen von selbst, setzt die Lage der Figur aber per Messhaken. Ein
  Lauf mit Suedwurf ganz ohne Messhaken fehlt.
* Eine von 208 gerechneten Todeslagen: Steht die Figur zwischen Kamera und Leiche, liegt der Schluessel 2560 statt 938
  von der Leiche, weil er naeher im Bild hinter der Figur laege.
* Der Riegel zeigt, dass der Schluessel in jedem Bild der Einstellung zu sehen ist, nicht, dass er als Schluessel
  erkennbar ist (siehe "Zu entscheiden").
* Mit der neuen Wurfrichtung zieht der fallende Kanister in der tiefen Wurf-Einstellung kurz hinter Kopf und Schulter
  Ihrer Figur durch, wenn sie dort vorn im Bild steht. Geworfen hat Marvin bzw. John oben vom Gelaender; verdeckt wird
  der Kanister von Leon bzw. Elza unten im Bild vor dem Tor. Die Aussage stammt aus der Abnahme 2. In ihren Aufnahmen
  ist der Kanister bei Leon in den Bildern 1319 bis 1321 teilweise verdeckt (wurf_leon_fall_gross.png; drei Bilder, das
  ist eine Zehntelsekunde; in Bild 1317 noch frei, Bild 1318 fehlt auf der Tafel, in Bild 1322 nicht mehr zu sehen), bei
  Elza in Bild 2018 (wurf_elza_szene.png; in Bild 2016 frei, in Bild 2020 nicht mehr zu sehen). Mit der alten Richtung
  fiel er laut Abnahme 2 rechts neben der Figur. Die Abnahme 3 hat den Hinweis uebernommen und nicht neu vermessen; ihr
  Bericht schreibt "des Werfers", nach den Bildern ist die Spielfigur gemeint. In ihrer Aufnahme des natuerlichen Kampfs
  (s_l2_leon_wurf.png, Bilder 1014 bis 1034) steht in dieser Einstellung keine Figur im Bild, der Kanister faellt dort
  frei. Nicht geaendert, weil eine andere Richtung die Landung in der Kanalmitte aendert.
* Bleibt die Figur an der Rampe, lag der Kanister nach dem Abschnitt zur Spur G2 "3300 bis 4400 Bilder" (etwa 110 bis
  147 s), bis der Alligator ihn nahm. Das ist der Stand vor den Nutzerbefunden vom 9.11. (alte Kanisterlage, kein
  Fenster im Biss und im Spurt) und mit dem neuen Stand nicht neu gemessen.
* Das normale Spurt-Ende wechselt im Port weiter ohne Ueberblendung ins Schwimmen (RE2 blendet, @0x80100c38). Nicht
  geaendert (gelobter Kampf).
* Seitliche Passagen am Kanister ausserhalb des Fensters bleiben ohne Aufnahme (siehe "Zu entscheiden").
* Ausserhalb des Alligators: Der RE1.5-Schadenspfad des Ports beendet ein Klettern nicht; das Original taete es
  (@0x80012ed4). Nicht angefasst.
* Zwei geplante Spiel-Riegel sind nicht gebaut (einer zum Kletterbiss, einer zu Tod, Leiter und Rueckkehr). Beide Wege
  pruefen Riegel im Testprogramm, und die Abnahmen haben beide am gebauten Spiel gefahren; Namen unter "Technisch".
* Ein Testteil (Teil K von unit_r41_gatorfix_nb4) legt den Kanister weiter an die alte Liegestelle; im Spiel ohne
  Wirkung.
* Formal: In zwei Pruefdateien nennen Kopfkommentare noch die alte Regel "nie unter 10,4 Bildpunkten" (Namen unter
  "Technisch"); die Pruefung selbst folgt der neuen Regel. Im Spiel ohne Wirkung.

Annette (Spur AN)
* Die Wege F0 und N im Zug 6030 (siehe "Zu entscheiden").
* Annettes drei Zeilen in 5090 sind Schnitte aus der Claire-Disc (siehe "Fuer Sie zum Aufnehmen").
* Ein Zweig im Annette-Code ("erster Besuch erst nach dem Tor") ist im echten Spiel nicht erreichbar (nur mit
  Debug-Flags), weil der Torschalter die Fuehrerstand-Szene als gelaufen markiert; unveraendert gelassen.

Riesenform (Stufe 3 und Modell K5g3)
* Wagen B nach dem Wurf (Zonenkamera 13) und die Aussenkamera 14 bei weitem Zurueckweichen: siehe "Zu entscheiden".
* Weicht der Spieler 3 bis 4 s zurueck, steht beim Schnitt in die Rueckstoss-Szene ein Bild lang (33 ms) noch das
  gehaltene, dunkle Kampfbild.
* Kurz vor dem Wechsel auf Kamera 12 liegt das Szenenbild knapp unter der Dunkel-Grenze der Pruefung (bis 595 von 600
  Promille).
* Mit 1000 Lebenspunkten zeigt die Szenenkamera in der Standardlage die Form kurz (rund 15 Bilder) klein am Wagenende
  hinter Leon bzw. Elza, bevor Kamera 11 kommt; die Riegel halten diesen Stand fest, damit ein Rueckschritt auffaellt.
* Die Haltezeit der Kampfkamera (30 Bilder) prueft nicht nach, ob der Spieler noch vor ihrem Auge steht (gemeldet, nicht
  geaendert).
* Der Bisspunkt der Form wurde am alten Kiefer K4 gemessen; am neuen Kiefer liegt er 21 Einheiten vor der Kieferspitze
  statt in der Zahnreihe, die Reichweite des Bisses ist unveraendert. Nicht angepasst.
* Im Stehen hat die Form das Maul offen (Ruhe-Clip mit dem neuen Kiefer); so haben Sie es in der Vorlage gesehen und
  angenommen.
* Die vier K5e-Punkte der Fassung vom 9.11. betrafen die Haut K5e, die in v0.8.30 nicht mehr im Spiel ist.
* Hals B4 (von Ihnen angenommen): kleine rote Zacken an den Unterkanten der roten Halsfelder (beim Bewegen leicht
  flimmernd), ein dunkler Sprenkelfleck im linken roten Halsfeld, feine Fransen im Licht an Schulter und Kopf; an der
  Schulter oben war der vorige Stand sauberer.
* Rechte Hand Variante 1 (so gewaehlt): Oben in Ihrer roten Armzone ist kein Arm, rechts neben dem Kiefer steht etwas
  ueber. In einer Bewegung dreht der Unterarm schneller weg als auf dem Blatt (bis 16,7 statt 12,4 Grad je Bild), damit
  nichts ineinandersteckt; in einer Pose steckt eine Kralle 81 Einheiten im Kopf (im Bild nicht zu sehen).
* Vorderzaehne: Die unteren sind kurz (sichtbar 25 bis 86 Einheiten, die alten Seitenzaehne 144). Im Fallen stecken die
  unteren Vorderzaehne bis etwa 100 Einheiten in der Hand; nah von vorn kreuzen sich obere und untere in X-Form; drei
  neue Zaehne beruehren den anderen Kiefer bis 1,5 Einheiten tief (weniger als ein Bildpunkt). Die alten Seitenzaehne
  treten bei geschlossenem Maul in 378 Posen seitlich aus der Schnauze (vorher schon so, nicht angefasst).
* Haut K5g3 unter weissem Licht (im Spiellicht nicht zu sehen): Haut kupfern, Braun oliv-ocker, die Flecken heben sich
  wenig ab, ein kleiner oranger Fleck im Maulwinkel und ein rosaroter unter der Zahnreihe; die Augen sind zwei kleine
  graue Punkte statt hell-rosa wie im Video; die Leuchtstelle am Becken wirkt lila-grau mit grauen Bloecken (nicht
  bearbeitet).
* Die Aufhellung je Seite der Haut geht ueber die Original-Seitenhelligkeit hinaus (Entscheid gegen einen schwarzen
  Sattel); Ihr Urteil am Bild steht aus (siehe "Zu entscheiden", Farbe).
* Gegen die 80%-Screenshots (Ordner 46): In Bild 1 reicht das vordere Bein rund 40 Bildpunkte tiefer, zwischen den
  Beinen ist eine Luecke; in Bild 2 ist das Maul offen und rot, wo das Video nur dunklen Schattenriss zeigt, und der
  rechte Arm ragt in eine helle Stelle.
* Technik: 225 Dreiecke ohne Texturflaeche (K5e: 28); die Bildzeit mit dem groesseren Modell ist nicht gemessen.
* Software-Renderer: An einer Teilungskante kann ein Randpixel anders fallen (nur Software-Pfad, nicht Ihr Renderer);
  nicht gemessen ist, ob die Grafikbibliothek bei sehr grossen Fensterskalen an anderer Stelle ueberlaeuft.

Sichel-Form und Hangar
* Restpunkte, die Ihnen mit Ordner 28 genannt wurden: Stachelspitze als duenne graue Nadel, Wurzelglied im Bogen etwas
  breit, der Bogen rollt bei h100-h102 schnell ein, Birkin steht nach dem Wurf etwas frueh seitlich und stark gebueckt.
* In zwei Einzelbildern (h35, h72) ist der Schwanz hinter dem Koerper verborgen (im Original ist dort unter dem Schritt
  ebenfalls nichts zu sehen); frei haengt er in h81 (am fliegenden Spider-Man), h146/h148 (Kameraschnitt) und h159
  (Bildrand).
* Nur im direkten Vergleich: Austrittsknick in h30/h31 etwa 63 Grad, ein Sprung von h29 auf h30, die rosa Unterseite
  grenzt scharf an die dunkle Oberseite, und die Ringe der Oberseite enden an der Bauchkante.
* Hand: heller als in der Vorlage, Rot nur an den Krallen, Daumen grau-weiss, harte Kante am Handgelenk.
* Aus Runde 7: In der Griffphase steht rechts neben dem Kopf die helle Rueckenplatte recht hoch; kurz nach dem Wurf
  steht der Unterarm etwa eine Drittelsekunde seitlich ab; einige eigene Riegel des Modells sind rot (riegel_v8 18 von
  24) - die Pruefer stuften das als nicht stoerend ein.
* Spider-Man: im ganzen Raum dunkler und etwas kleiner als im Video, Arme im Anflug eher seitlich, Flugpose starr,
  Zappeln anfangs etwas schwaecher, im Griff weniger Blut; Birkin dreht nach dem Wurf spaeter zurueck als im Video.
* Die Sichtpruefungen der Szene liefen mit Leon (50A0); Elza ist ueber bitgleiche Protokolle belegt.
* Die Zahl der Fingerkrallen loest das Video nicht auf; das Modell behaelt drei Fingerkrallen und die Daumenkralle.
* Hangar: Was der Kampf bringt, bleibt in v0.8.30 offen (kein Gegenstand, keine Tuer); in v0.8.31 entfaellt der Kampf im
  Gang. Den Ausloeser ueber die Westtuer (aus 4070) zeigt kein Video. Welcher Ton beim Hangar- und Laborkampf an den
  Original-Rufstellen des Birkin klingt (die Hangar-Raumbank hat dort einen anderen Ton als 3070), ist nicht gemessen.
* Sherry-Ruf: Im Video kann das Bild um etwa 3 Bilder gegen den Ton verschoben sein; 5080 ist nur per Riegel geprueft.

Labor-Birkin
* Der Vortrieb am Standfuss waehrend der Angriffe (FUN_8011bdd8) fehlt im Port. Er aendert Anlauf und Kampfabstand in
  allen Raeumen dieses Birkin-Typs und braucht eine eigene Abnahme (vorgesehen fuer v0.8.31).
* Beim Rammen ist nicht geprueft, ob die Box ebenfalls die gezeichnete Lage nimmt. Die Riesenform (Grundlage RE2) ist auf
  dieselbe Bildfrage noch nicht gegen RE2 geprueft.
* Die Raeume 50E0/50F1 gibt es nur im Mod-Baum, nicht auf der 40%-Disc.

Klauenhieb und Aufrichten
* Waehrend des Rueckzugs steckt die Klaue kurz tiefer in der Wand (Capcoms Bewegung zeigt "haengen bleiben, ruetteln,
  herausziehen"); auch beim Zucken mitten im Rueckzug (7-mal gemessen) bleibt sie tief in der Wand.
* Ausserhalb des Hiebs (Gang, Biss, Wurf, Aufrichte-Szene in 50E0) steckt die Klinge oft in Waenden - nicht Teil der
  Regel.
* Treffer und Bild koennen auseinanderfallen: Die Treffer der Kralle rechnen am EM30-Gerippe, der Rueckzug an der
  gezeichneten Klinge - die Klaue kann sichtbar in Leon ragen, ohne zu treffen.
* Die Gelaender-Oberkante 1900 ist nur an drei Gelaendern gemessen (Streuung 1500 bis 2100); einige Zellen in 50E0/50F1
  sind unsicher eingeordnet.
* Fuer diese Kaempfe gibt es keinen Original-Beleg: Der Laborkampf stammt aus dem Mod, den Hangar-Kampf hat der Port
  eingefuehrt, 3070 ist nie im Original gemessen.
* Bekannt, nicht angefasst: Der Port zeichnet einen Clip ab Bild 1 statt 0; der Biss prueft sein Tor nach statt vor dem
  Bildwechsel; die Todesphasen ziehen weiter vor dem Zeichnen ab; im Kraftraum-Auftritt fehlt ein Aufruf des
  Fussschritts (@0x80119cd4), und der Vortrieb am Standfuss (FUN_8011bdd8) fehlt weiter (siehe "Labor-Birkin").
* Ihre Rueckmeldung zu Ordner 43 (Aufrichten) steht aus.

Gorilla-Filme 114 und 11C
* Einen unabhaengigen Pruefer fuer den Einbau gab es nicht; belegt sind die Riegel und die Messlaeufe. Ihr "einfach
  super" galt dem Einbau von 114, Ihr "sieht super aus" dem Film 11C.
* Schrei: Satz 4 der Raumbank von 11C0, laut Protokoll von Ihnen vorerst so angenommen (Protokollfassung); er laesst
  sich an einer Stelle tauschen (Ihr "aus 11C Sekunde 6" kann auch die Landung meinen, Satz 2, 0,51 s).
* Im 11C-Film: rechter Fuss des Gorillas 5 bis 7 cm ueber dem Boden (Original-Clip), Knick im Handgelenk in den Bildern
  204-212; die Brustschlaege sind mit Satz 8 vertont (kein Beleg im Spiel); der Schutt ist heller als im Original.
* Die Filme haben 320 x 240 Bildpunkte (der MDEC-Dekoder kann hoechstens 256 Zeilen), die Quelle 640 x 480.
* Vor dem Android-Bau ist der .cxx-Cache zu leeren (Modul umbenannt).

Monitore und Wasser
* Monitor unten in der Mitte: Farben und Takt stimmen, das Muster nicht - im Original grobe Bloecke, im Port feinere
  Streifen (schon vorher so).
* Der ueberzaehlige Eintrag der 8-Platz-Tabelle schreibt im Original in weitere Speicherstellen (u. a.
  Ton-Lautstaerken); die Folgen sind nicht untersucht.
* Vom Wasser gibt es kein Original-Video, nur den Speicherbeleg; welche Objekte in 1260, 2050, 2070 und 2080 beim
  normalen Eintritt angemeldet werden, ist im Original nicht gemessen.
* 5050, 50B0, 50D0 und 6000 haben einen anderen Mechanismus - nicht untersucht.
* Die PSX-Fassung zeichnet die Streifen nicht.
* Fuer Runde 2 gibt es keinen eigenen Pruefbericht; die Protokolle des Elza-Laufs hat der Orchestrator selbst geprueft.

Zielhilfe
* Der Port folgt hier RE2 Retail, nicht RE1.5 (Beta -> Retail).
* Bei kleinen Winkeln rastet die Figur ein Bild frueher ein (Endwinkel gleich).
* Die grosse Schrittweite fuer die Super Redhawk erkennt der Port an deren Clip-Laenge (Mod-Bank mit 8 Bildern; auf der
  40%-Disc 10).
* Automatisches Drehen und Drehen von Hand laufen beim Heben in umgekehrter Reihenfolge als in RE1.5 (schon vorher so).
* Ihre Rueckmeldung zu Ordner 40 steht aus.

Boden-Schatten
* Den Spielerschatten hat niemand eigens im Spiel gemessen.
* Die Diagonale der Schattenvierecke: PC 0-3, PSX 1-2; das Eckenmass weicht ab (Original -X-2, Port -X) - beides alt,
  eigener Auftrag.
* Weitere Nahgrenzen ohne Beleg (vz < 64) in main.c und gator_re2_pc.c - jede braucht ein eigenes RE.

Pipe
* Gegen Zombies so stark wie das Messer. Ordner 42 kuendigte fuer den Schadens-Umbau den Disc-Wert 24 an; nach Ihrer
  Ausnahme vom 16.11. (Zombies wie RE2) behaelt der Bauplan fuer Zombies den RE2-Schaden aller Waffen - welcher Wert
  dann fuer die Pipe gilt, ist offen.
* Das Rohr ist ein Nachbau; der Pruefer: "Der Nutzer sollte es am Video abnehmen."
* Drehen beim Zielen bleibt beim Port-Wert 24 (Original 0).
* Elzas Netz ergaebe die Reichweite 1326; der Port nimmt fuer beide 1355.
* Kein Fundort in v0.8.30 (nur Item-Debug). Ihre Rueckmeldung zu Ordner 42 steht aus.

Optionen
* Die unterste Menuezeile (AI und ULTIMATE) ist am Bildrand ein paar Bildpunkte abgeschnitten (schon vorher so).
* Rote und Blaue Master-Keycard: Fuer die Disc-Kacheln gibt es zwei Lesarten; gewaehlt ist die nach Bildinhalt.
* Ob die Disc-Dateien (DISC40) im Paket ankommen, ist am Paket nicht geprueft.

Zweite Welle allgemein
* Die Linux-Laeufe der neuen Riegel stehen bis zum Release-Bau aus (die Paketierung faehrt die Linux-Suite) - das gilt
  auch fuer alles seit dem 9.11. Ein Kamera-Riegel der Riesenform (Warten mit Leon) misst genau an seiner Schranke ohne
  Abstand und kann auf einem anderen Rasterer kippen, ohne dass sich das Spiel aendert.
* Nur am PC gebaut; PSX- und Android-Bau nicht geprueft (die Android-Texturgrenze von 4096 Zeilen haelt die Riesenform
  mit 15 Farbzeilen (3840; Stand 9.11.: 10 Farbzeilen, 2560) ein, auf einem Geraet ist das nicht gemessen).

Zombie-Arme
* Der Zufallsgenerator des Ports bleibt der Port-eigene; die 40%-Disc rechnet mit einem Zustands-Generator (Startwert je
  Raum). Das aendert jeden Wurf und ist eine eigene Runde - nicht in v0.8.30.
* Die Reihenfolge der Spawn-Wuerfe weicht vom Original ab - ohne Einfluss auf das Aussehen.

Key Items
* Auf der Dreh-Lage der Kurbel selbst (Blick Nord) meldet USE "You can't use it here."; es gilt das Original-Rechteck
  westlich der Kurbel, wo auch "It's a square hole." kommt. RE2 hat an seiner Kurbel dieselbe Form.
* Zwei Wege bei Karten: 3010 (Green Keycard, Tuerschloss) per Aktionstaste, 4000 (Blue Master Keycard, Panel) per USE -
  beide mit RE2-Vorbild.
* "You've used ..." beginnt an allen 14 Stellen auf fast schwarzem Bild, weil die Welt schon in der Einblende laeuft
  (Port-Wahl).
* Wer die Sicherung vom Hebetisch in 1150 noch traegt, hat mit der aus 2030 zwei Eintraege - reicht fuer Rolltor und
  Generator, keine Sackgasse. Bei vollem Inventar kennt die Frage zum Herausnehmen am Kasten 2010 das volle Inventar
  nicht: "You've taken the Fuse." ohne Eintrag, bis ein Platz frei ist (keine Sackgasse).

Gore
* Spinne: Wandflecken und die verstreuten Bodenflecken nach dem Platzen (80%-Video) fehlen; die Lache ist ein wachsendes
  Viereck (RE1.5 und RE2 kennen keine Wandflecken).
* Treffer-Spritzer und alle RE2-Partikel sitzen am Fusspunkt der Spinne, nicht am getroffenen Teil.
* Babyspinnen sterben im Port praktisch nie (das Zertreten verlangt einen Spieler-Zustand, den die Zielhilfe nicht
  liefert); ohne Tod kein Fleck.
* Nach der Landung verschwinden einige Oberschenkel der Spinne, die Unterschenkel bleiben liegen (Eigenheit der Beta).
* Die Leiche nach dem Python-Schuss bleibt beschiessbar; ein zweiter Schuss gibt eine zweite gruene Wolke ohne Scherben.
* In Elzas Kamera von 2071 war die RE1.5-Lache nie sichtbar, weil ein Schattenviereck nahe der Kamera ganz wegfiel;
  diese Ursache ist behoben (siehe "Boden-Schatten nahe der Kamera"), die Kamera von 2071 ist danach nicht eigens
  gemessen.
* Unter der Standard-KI fehlen weiter RE1.5-Inhalte jenseits von Angriff und Treffer: das Klettern der Spinne, Fressen
  und Schlafen der Zombie-Familie, Schritt-Toene (siehe "Zu entscheiden", Tabelle 5b).
* Kraehe: Liegende Federn und Fetzen wie im 80%-Video gibt es in RE2 nicht - nicht gebaut. Sitzt die Kraehe am Boden,
  faellt die Haelfte der Federn sofort unter den Boden (wie im Original).
* Hund: Zwei der sieben RE2-Teile haben kein RE1.5-Netz. Mit Elza nicht gemessen.
* MC51-Beinzelle: freier Fensterbeleg fehlt (nur Messhaken und Riegel).
* Weitere Video-Punkte nicht gebaut: Kopf weg mit Nicht-Magnum-Waffe, Wandspritzer, grosse Lache unter dem lebenden
  liegenden Zombie; ob eine brennende Leiche weiterbrennt, ist am Port nicht geprueft.

Handgranate und Waffenschaden
* 3070/3071: kein Fensterbeleg fuer Elzas 3071-Kampf (nur Riegel); die 600-Form ist im Fenster nur fuer Leon in 3070
  belegt.
* Behoben in der Riesenform-Spur: START waehrend des Riesenform-Griffs in 5090 fror Form und Leon dauerhaft ein; jetzt
  oeffnet START im Griff den Statusschirm nicht (Abnahme 4 der Stufe 3, Leon und Elza).
* Nebenbefund 11C0: Der Gorilla trifft Leon in der Aufstellung der Abnahme alle 57 bis 58 Bilder aus rund 2100 Einheiten
  Abstand (nicht untersucht).
* Aeltere Nebenbefunde (eigene Spur): Dauerflamme an toten RE2-Hunden; ein echter Wurf zuendet in 2000, 1230 und 2030 an
  manchen Standorten sofort an der Wand; Zombies bleiben nach der Werfer-Explosion mit 0 HP aktiv.
* Nicht gebaut: dunkle Flecken an Wand und Decke (Video); der Blitz bleibt weiss-orange (Video weiss-gruenlich); die
  Werfer-Explosivrunde ist unveraendert; Saeure- und Brandgranate im RE1.5-Gehirn behalten "Oberkoerper ab, kriecht
  weiter"; der Rumpf bleibt als Torso in der Lache liegen; das Kanal-Zombie-Modell des Mods ist weiss-grau, im Video
  khaki.
* Munition: Handgranaten liegen in keinem Raum aus; Pistolenpatronen nur in STAGE1 - der Pistolen-Riegel des neuen
  Endkampf-Ablaufs verbraucht 157 Patronen; ob ein Spieler so viele in 5090 hat, haengt am Verbrauch vorher. Der
  Raketenwerfer liegt in keinem Raum.
* Gitterhaende in 1210: Die Rakete macht dort nur noch 40 statt 1000 (RE2-Arme) - 7 Raketen statt 1 bis zum Abfall des
  Arms (nur per Riegel; im Fenster waren sie kein Ziel). Waehrend einer Mutation von Birkin wirken Raketen nicht (Revive),
  wie vorher; im Zug 5090 gibt es seit der Riesenform-Stufe 3 keine Mutation mehr.


Kanalwasser
* Die Fallroutine der Tropfen fehlt dem Port weiter; in 28 anderen Repo-Raeumen stehen Tropfen-Zeilen - ob deren Skripte
  den Effekt starten, ist nicht geprueft; wo sie es tun, haengt der Tropfen ebenso.
* ROOM50B0 Kamera 2: roter Bodenschein aus demselben Zwischenstand, blinkt nicht, unveraendert (siehe "Zu entscheiden").
* Ob die Port-Zombies im Kanal von 2000/2001 beim Waten spritzen wie im 80%-Video, ist nicht gemessen.
* Kein DuckStation-Lauf; die Aussage zum Original stuetzt sich auf die Skript-Bytes beider Discs, die EXE und das
  80%-Video (von zwei Abnahmen als ausreichend bewertet).

Nachtrag 30 (Spur N)
* Die offene Schranktuer in 2010/2011 hat keine Kollision: Die Figur kann durch den Fluegel gehen.
* Die Schrank-Innenseite zeigt nach dem Einsetzen weiter die Rueckwand mit fehlender Sicherung und roter Lampe; nur die
  Nahansicht zeigt den Wechsel auf die gruene Lampe.
* Die Tuer ist nur unter Kamera 3 zu sehen (unter Kamera 4 zeigt der Hintergrund den Schrank geschlossen).
* Stirbt die Figur bei offenem Panel, liegt "YOU DIED" ueber dem Panel; danach geht es normal zum Titel.

Sicherungskasten und Panel (Spur PF)
* Die Lesbarkeit misst kein Test; sie ist allein durch Ihr "ja, ist super." abgenommen. Das Panel ist im Vollbild nicht
  gemessen.
* Waehrend der Panel-Nachrichten schimmert das rote Schild schwach durch das Band (Port-Wahl, Text lesbar). Nicht belegt: Ob Sie das Durchschimmern stoert.
* Der Kasten ist nur unter Kamera 3 eine Port-Flaeche; unter Kamera 4 bleibt der Original-Hintergrund.

Alligator (Spur P und G2)
* Beim Kampfbeginn ragen Kopf und Hals von rechts ins Bild, der Rumpf folgt nach gut 100 Bildern - mehr lassen das
  RE2-Band, die Kamerazone und Kamera 0 nicht zu.
* Mit dem Kanister im Maul ragt der Kopf bzw. das Kanister-Ende kurz ueber die Wand, dann gleitet der Alligator zurueck -
  sichtbar als Abdruecken von der Wand, kein Sprung.
* Der Kanister bleibt in RE2-Groesse am kleineren Alligator: Die Ueberlaenge zur Seite ist 1,5-mal so lang wie in RE2.
  Eine Umskalierung beim Biss waere erfunden.
* In RE2 wird die Alligator-Leiche 10 bis 15 % kleiner gezeichnet; der Port nicht.
* Putz, Finisher und Kanister-Sitz stehen auf den RE2-Bytes, nicht auf einem Bild aus dem laufenden Original (kein
  Abbild).

* Ein einmaliger Absturz in einem Elza-Riegel der Spur P (21-mal nicht reproduziert) ist ohne Ursache; seit Spur Q
  schreibt ein Haken beim naechsten Mal Adresse und Stapel ins debug.log.

Restpunkte (Spur Q)
* Effekt-Quader in 20B0 unter der Hardware-Grenze bleiben im Bild; ob das Original sie an dieser Stelle zeigt, ist ohne
  DuckStation-Vergleich nicht belegt.
* Treppenende ROOM6010/6011 (vorbestehend): Je nach Spur schiebt das Treppenende die Figur seitlich aus der Treppe; bei
  Elza ist nur ein schmaler Streifen in der Treppenmitte sicher. Mit dem Original nicht verglichen.
* Teil 8 der Treppenwange deckt nicht die ganze Wange - nur mit erzwungener Kamera 5 zu sehen.

Story
* Die unveraenderten Dateien 6030 main00/02/04/06/13 bis 19: per Spracherkennung nur main02 geprueft.
* Dateinamen, die nicht der Nachrichtennummer folgen (1050, 1150, 11B0, 1171, 1011, 1141, 1100, 1241): Wer neu spricht,
  ersetzt die Datei rechts; room1141/main42.wav und main50.wav spielen nie.
* Die dritte Zeile von 6030 Zeile 21 liegt auf dem unteren Kinobalken (lesbar).
* PSX: nur Syntaxpruefung, nicht gelaufen; Nachrichten-Id 40 ist auf der PSX nicht anzeigbar (siehe "Zu entscheiden").
* Zur Kenntnis, nicht geaendert (B1 bis B16): 30E1 Zeilen 14 bis 17 Rollen vertauscht (Disc: John draengt, Elza zoegert);
  30E0 verschweigt Marvins Beinwunde; 5011 Feuer statt Virus und erschlossene Sprecher; 3070 Sprecher erschlossen,
  "Underground" Fehllesung; 3071 Sherrys Insiderwissen; 4001 Sprecher Elza statt John; 3061 Zeile 5 ohne "Look,"; Adas
  Tod ohne Beleg dagegen; John in der Zelle 1211; Mod-Namen (Irons, Branagh, Kennedy ausser dem Ausweis, Annette, Elliot,
  Dr. Birkin, Virus Inhibitor) ohne Capcom-Beleg.
* Luecken des Materials: Sprecher von "......Leon" in 115, unlesbare Untertitel, der Name der Frau im Kittel ist nirgends
  belegt, das Finale 605 bis 607 fehlt ueberall.

Bahnsteig-Birkin (Spur R)
* Das Modell: Die G4-Fassung ist ersetzt; im Spiel steht die eigene Riesenform (seit Ihrem Entscheid vom 7.11. abends;
  in v0.8.30 K5g3). Eine vierte Abnahme-Datei der Spur R gibt es nicht.
* Die Szene vom Bahnsteig (Ihr Bild 1, ROOM5080 aus einem Blick, den keine Kamera des Raums hat) existiert im Spiel nicht;
  in 5080 kaempft seit der Stufe 3 die Riesenform und flieht am Ende.
* Griff der Form: Leon fliegt weg statt von der Form gehalten zu werden (RE2-Opfer-Sequenz nicht gebaut). Steht Leon vor
  der Form, verschwindet sein Oberkoerper in ihr (RE2-Verhalten, bleibt so).
* Auf dem orangen Boden von 5090 wirkt der grosse Schatten dunkelrot. (Der harte Modellwechsel bei der Mutation im Zug
  entfaellt, weil es dort seit der Stufe 3 keine Mutation mehr gibt.)
* Leon unter dem G5 im Westen (nur wenn er dort stehen bleibt und nicht schiesst): Der G5 rollt ueber ihn, er bleibt frei;
  in der Ecke unter dem Koerper kurz ohne Wirkung der Eingabe. Im freien Spiel in 16 Laeufen nie.
* Im Rueckstoss und G5-Auftritt ist Leon, wenn er weit oestlich steht, rund 14 s nicht im Bild (Gestaltungsfrage).

G5-Auftritt
* Die Lampe ist im 3x-Fenster 3 bis 12 % dunkler als in RE2 (Rasterkante beim Hochskalieren); Vorlauf 55 statt 59 Bilder
  (erste Lampe 0,13 s frueher). Beides gering, von der Abnahme 5 so eingestuft.
* Kampf: Ein Arm schiebt die Figur ein Stueck nach hinten; ob RE2 im Kampf genauso schiebt, belegt kein RE2-Kampf-RAM.
* Port-Wahlen: dritte Einstellung in RE1.5-Kamera 12 statt der RE2-Schnitte 5 und 6; Musik RE1.5 statt der
  RE2-Absenkung; die RE2-CD-Standbilder sind nicht nachgebildet; die RE2-Spieler-Gesten (Umdrehen, Hochschauen) nicht
  uebertragen; der Rueckzug des G5 im Kampf ist vereinfacht; die RE2-Zugfahrt-Vibration vor der Szene ist nicht gebaut.
* PSX: Szenen-Schnitte, RE2-Licht, Lampen, Beben und Brocken nur auf dem PC.
* Vormerkung v0.8.31: Der G5 soll aus dem Finale wieder heraus (Ihr Wort vom 7.11., siehe Einleitung); dann entfaellt
  dieser Auftritt, und das Finale wird nach dem RE2-Ende neu gebaut (siehe "Vorschau auf v0.8.31").

Umgebung und Pruefstand
* Die Skript-Riegel des Release-Skripts laufen in der Abnahme von v0.8.30 wieder mit; unit_r38_releaseskript_ablauf
  ist nur bei langem Build-Pfad rot ("Filename too long"), mit kurzem Pfad "71/71 Pruefungen gruen";
  unit_r39_titelversion_cmake ist rot, wenn git nicht im PATH liegt (so unter local_build.sh).
* Alle Spuren dieser Fassung sind nur am PC gebaut; PSX- und Android-Bau sind fuer keine Spur geprueft.
* Am 7.11. um 18:40 brach ein Wochenlimit des Kontos fuenf laufende Pruefungen ab, am 8.11. gegen 00:15 ein weiteres
  Kontingent die Alligator-Abnahme 8; alle wurden auf dem neuen Modell fortgesetzt (Protokoll vom 7./8.11.). Die Berichte
  der abgebrochenen Pruefer wurden von den Nachfolgern nur als Rohmaterial gelesen und jede Messung neu gemacht.
* Am 10.11. abends und am 11.11. frueh brachen Kontingentgrenzen erneut alle laufenden Arbeiten ab; sie wurden am 11.11.
  bzw. ab dem 13.11. mit den Notizen der abgebrochenen Agenten fortgesetzt.

## Technisch

* **Stand v0.8.30:** Zweig r41/release_0830, HEAD 44cc5b7d5 (17.11.), Basis master 0b86e36c6 (9.11.). Merges vom 17.11.
  in dieser Reihenfolge: 762ebfda4 r41/integration (799594460, Alligator-Nutzerbefunde), b1b7e8cfb
  r41/zwischensequenz_114 (00011e063), a465189c7 r41/monitore (dce5427d2), bbb00c08e r41/zielhilfe (d12b972fb),
  dae306b17 r41/pipe (b417700a4), 0968140a2 r41/optionen (ecd2a5c70), f0f9b0749 r41/sichel_szene (84b36f0d2), 198284284
  r41/birkin_aufrichten (f7057a708), b5e557c2e r41/form (649bbbf39; Konflikte in .gitattributes und
  tests/unit/CMakeLists.txt, beide Bloecke behalten), ea630d75f r41/birkin_kralle (32b4575e6); danach die
  Testanpassungen 16d9dffdd, 8c54123dd, 5ae09b47b, der Fix 29312b011 und der Nachzug 44cc5b7d5 (r41/birkin_kralle
  83eab1010, Ausholen) und die Alligator-Sperre ec0d39bb9. Beim Release wird master auf ec0d39bb9 vorgezogen. Nach 53667af13 lagen auf master: 010f833b9
  (Aufnahmen synchro/ und RE2-Dateien), b8e3ad700 (Linux-Portabilitaet fuer den Release-Bau), 0fb3d8247 (Ihre neuen
  PRI-Freistellungen, Entscheid "Umbenennung committen") und 0b86e36c6 (Linux-Warnungen in drei Tests).
* **Stand master vom 9.11. (Vorgeschichte):** 53667af13 (9.11., Fast-Forward aus r41/integration). Seit b27d96891 neu:
  2c1c0eb1a (Sherry und Annette, Bericht f6204ab0a, Pruefung ebfc5f0cf); Lauf C Merge 91636662f (r41/sichel_nb2, Sichel
  V7, Abnahme 0 5ea6d94f0), Bericht 154e59a89, Gegenpruefung a3051c902; Labor-Fix 145316fc9 (Bericht d3dcddf1a, Pruefung
  e181dfac7); Lauf D Merge db52dd8bf (r41/form, Riesenform K5e, Einbau E32 bestanden), Riegel-Fixes 48263fc5f und
  c3a6b2057, Bericht 37f7a8ecb, Gegenpruefung 53667af13. Hauptbaum-exe neu gebaut 9.11. (Dateizeit 10:30). Der Stand
  b27d96891 (8.11.) enthielt: 8f464d57c (7.11., Merge WS) = 68f3f5f13 (6.11., v0.8.29 d682200f + Birkin-Zweig + Merges
  47b5b3280, b539a5337 + Spuren vom 31.10./1.11.: R d0d3e10d7, O 2a0e2ff44, P 361017e92, Integration O+P 91cd9ec82, N
  c5d7ba18a, Q 852b43581, R 6c34c132a, Integration Q+R 746958956, 0dd07032d, 2bcf4d0f1) + Welle 1 (Merges 6.11. 19:56
  bis 20:11 in der Reihenfolge S 83eab5b59, W 35773161a, ZA fb3d4c4f2, PF 66f436807, GR 54e0b06e7, ST80 7d8634051, G5A
  c9297033c; danach 8cf96bad1 Pistolentest, 0df13de33) + nachgeholte Abnahmen und Nachbesserungen vom 7.11. (W Abnahme
  3, G5A Abnahme 5, ST80 Abnahme 1 nicht bestanden -> Nachbesserung 1 45174ae1f/ceb920b22/8ffad09ab -> Abnahme 2
  bestanden 7587d3e57; VERTRAG 1.1 0b25c2703: Bank-9-Bit 128 an ST80, Nachrichten-Ids 6030 31 und 40) + Spur WS (Zweig
  r41/waffenschaden, sieben Commits 57d416401..ac72cd3cb, Abnahme 97349ae5e, Merge 8f464d57c ohne Konflikt). Dazu Welle
  2: Lauf A Merges 10bb4bc79 (r41/form_nb4, Stufe 3, Abnahme 4 f9d9a96f0, 9 Konfliktdateien) und 2a0fe07bc (r41/annette,
  Abnahme 5 e0137de38, 3 Konfliktdateien, VERTRAG 1.2), Fixes aa534a122, a9f77838a, a28502957, d99419679, d5f4f5914
  (Riegel auf die Stufe-3-Zeitachse), Kette A+C 3163f6045; Lauf A2 275a57906 (r41/form, Asset K5c, E30), bf5001400
  (veraltete Kommentare, u. a. 43 -> 35 Pistolenschuesse am Alligator), 2312b9e0a/dabac8d22 (Wagenweg-Riegel 1000 HP),
  8f09f8fe0 (Messhaken), 3b2eec448 (Zug 6030), c752d9a73 (Riegel Zug-Weg und 5090-Kombilauf), 72c80256f (Dossier),
  4dde57452/b0623e4d6/89586a617 (Absturz Software-Renderer, Riegel unit_r41_uv_textur); Lauf B c19f8aaf3 (r41/gator_re2,
  Abnahme 9 00c6b3bad, 4 Konfliktdateien), 198980793, 548b8f3e7, eef7bc87b, 05015ad01, 8d3d840a9, b27d96891 (Paket-Gate
  um GATOR40A0.RBJ). Nicht belegt: Fuer die drei Merges vor den Spuren (47b5b3280, b539a5337, Birkin-Zweig) gibt es kein
  Leseblatt und keinen Abschnitt in diesen Notes.
* **Alligator-Nutzerbefunde 9.11. (Abnahmen 9./10.11.; in v0.8.30 ueber den Merge 762ebfda4):**
  Zweigstand r41/integration 799594460, Basis master 0b86e36c6. Befund 1 (Klettern): 8d165ee58. Befund 2 (Kanister im
  Biss und im Spurt): 80c7394ab, Biss laeuft zu Ende 16f7b402b. Befund 3 (Schluessel im Gebiet seiner Kamera):
  6d7a9e121, f8c71298a (Rueckfall auf die Bildschwelle), ersetzt durch bf345ea49 (kein Rueckfall). Nachtrag K1
  (Kanalmitte) e5cd784dc, K2 (naechster Punkt zur Leiche) ae9e17c9e, H3 (Spurt-Abbruch) 3061fe397, Riegel-Nachzug
  05c8851e6. Nachbesserung zur Abnahme 2: 61aeffeb8 (Endlage mit vier Ecken, verworfen: zu streng, 172 von 208 Lagen
  weiter weg) und cceb11646 (auf die Mitte zurueckgenommen), Riegel 190c4b1da, 04aa0e7db, 3a532ee7e. Berichte 4d68d2549,
  077481839, 3f12a9b4e, 696a42f42, 5116d63ed; Abnahmen 0 bis 3: 6b221f0d6 (nicht bestanden), b29263046 (bestanden),
  dd5010b5d (nicht bestanden), 799594460 (bestanden).
  - Abnahme 0 (Stand 077481839): Der Lauf der ganzen Gruppe (150 Riegel) wurde nach 3 fertigen Riegeln abgebrochen (zu
    langsam neben den eigenen Laeufen des Pruefers). Gefahren ist ein eigens gewaehlter Ziel-Satz: "100% tests passed, 0
    tests failed out of 96" (510,53 s). Durchgefallen nicht an den Riegeln: der Schluessel an den Laeufen im echten
    Kampf, der Biss-Abbruch an der selbst gelesenen RE2-Disassembly (gemessen dazu ein Riegel-Lauf des Fixers).
  - Abnahme 1 (Stand 3f12a9b4e): "100% tests passed, 0 tests failed out of 152" (5410,25 s).
  - Abnahme 2 (Stand 696a42f42): "99% tests passed, 1 tests failed out of 154" (5436,62 s), rot unit_r41_gatorszene_tod
    ("tod: 6 Fehler", einzeln 6 von 6 rot, rot seit ae9e17c9e).
  - Nachbesserung (Stand 3a532ee7e): "100% tests passed, 0 tests failed out of 154" (4451,95 s).
  - Abnahme 3 (Stand 5116d63ed): "100% tests passed, 0 tests failed out of 154" (5019,56 s); unit_r41_gatorszene_tod
    einzeln 3 von 3 "tod: 0 Fehler". Aufruf der Gruppe: ctest -R "gator|kanister|alligator|2090|2080|re2gator".
  - Unit-, Eigenschafts- und Sonden-Riegel (Bericht, Stand f8c71298a): ctest -R "^(unit_|prop_|probe_)", ausgenommen
    unit_r38_releaseskript_*, unit_r30_android_*, unit_r35_android_*, unit_r39_titelversion_cmake und
    unit_r39_titelversion_ablauf; Zeitueberschreitungen: unit_gator_sweep, unit_r37_masken36_ausschluss,
    unit_r41_alligator2_erreichbar, unit_r41_birkin_wand_g5, unit_r41_birkin_wand_griff.
  - Neue Riegel: unit_r41_gatorfix_kletterbiss(_elza), unit_r41_gatorfix_kanbiss(_elza),
    integration_r41_gatorfix_kanister_vorbei(_elza), integration_r41_gatorfix_kanister_mitte(_elza),
    unit_r41_gatorszene_tod_gebiet(_elza).
  - Angepasste Riegel: unit_r41_kanister2080_landung; unit_r41_gatorszene_tod (drei Staende);
    unit_r41_alligator2_schluessel (Becken-Scan); unit_r41_alligator2_weg (jede Lage beginnt mit frischem Boss-Zustand,
    f8c71298a - behebt ein Leck des Testprogramms, Gegenprobe gegen 0b86e36c6 im Bericht);
    integration_r41_kanister2080_kampf (Wurfrichtungen 3342/888, 05c8851e6); die exe-Riegel
    integration_r41_gatorszene_tod* (test_r41_gatorszene.cmake) und integration_r41_alligator2_k3(_elza)
    (test_r41_alligator2.cmake): Bildschranke 2 x 2 an der Endlage und in der Kamera der Einstellung, dazu lesen sie die
    neue Protokollzeile "Schluessel-Bild".
  - Nicht gebaut (Plan des Untersuchers): die exe-Riegel test_r41_gatorfix_kletterbiss.cmake (Befund 1) und
    integration_r41_restpunkte_gator_wiederkehr (Befund 3). Im Testprogramm faehrt unit_r41_gatorfix_kletterbiss den
    Engine-Weg mit den Kletter-Baenken und unit_r41_gatorszene_tod_gebiet Tod, Leiter, Rueckkehr und Kamerawechsel. Am
    gebauten Spiel gefahren: Kletterbiss in der Abnahme 1 (8 Laeufe in der Einzeltabelle des Berichts, davon 5 mit Biss
    waehrend des Kletterns; die Uebersicht desselben Berichts nennt 9) und in der Abnahme 3 (8 Laeufe); Tod, Leiter hoch
    und runter und Aufnahme in den Abnahmen 1 und 3 (Leon und Elza).
  - Veraltete Kopfkommentare, Code der Pruefung richtig: tests/unit/probes/r41_alligator2.cmake Z. 5 ("Becken-Scan: nie
    unter 10,4 Bildpunkten") und Z. 9 ("Schluessel >= 10,4 Bildpunkte und sichtbar"),
    tests/integration/test_r41_alligator2.cmake Z. 17 (">= 10,4 Bildpunkte ... Kasten >= 10 x 9"). Dieselbe Datei sagt
    in Z. 284-285, dass die Bildschwelle 10,4 hinter Ihr Wort zuruecktritt und die Bildgroesse nur noch gemessen wird.
  - Belegstellen im Wortlaut. Klettern: RE2 EM23_OVL_0000.BIN @0x801015b4 `addiu v0,zero,0x205` / @0x801015c4 `sw
    v0,-0x404(at)`; RE1.5 PSX.EXE @0x80012ebc `sb v0,4(s1)` / @0x80012ed4 `sb v0,5(s1)` / @0x80012ee0 `sb zero,6(s1)`;
    Endlage Ihrer Sitzung und der drei Laeufe des alten Stands: (11948,-1800,-18568), hinter der Ostwand.
    Kanister-Fenster: RE2-Abfrage @0x80100ab8-b1c mit @0x80100ae4 `addiu -8601` / @0x80100ae8 `sltiu 0x18f` (RE2: 8601
    bis 8999 voraus, mal Modellmass - das ferne Ende; im Spiel gilt es fuer den Kanister der Wurf-Szene seit der Spur
    G2, e139e9b6e vom 1.11., schon ab 0 - gator_re2_r41.c Z. 627-632, unveraendert gegenueber 0b86e36c6; Abnahme 2:
    "wirksam laengs 0 bis etwa 6000, |quer| < 1733") und @0x80100af4 `lbu +0x226`; den Kiefer-Lauf +0x224 fragt sie
    nicht ab. Biss laeuft zu Ende: @0x80100328 `jal 0x80101388` (Kiefer-Ablauf in jedem Takt; im Bereich
    0x80101388-0x80101578 kein Zugriff auf +0x226); +0x226 sperrt nur den neuen Biss (@0x8010133c `lb v0,0x224(a0)`,
    @0x80101360 `lbu v1,0x226(a0)`, @0x8010137c `sb v0,0x224(a0)`). Spurt: RE2 Sub 3 0x80100d30-0x80100ee8 ohne Fenster
    und ohne Abbruch; normales Spurt-Ende @0x80100ea8 `beqz s0,0x80100ee8` (Endflag des Spurt-Clips), @0x80100ebc `addiu
    v0,v0,0x400` (Vierteldrehung). Eintritt ins Schwimmen: @0x80100c38 `addiu a2,zero,0x20` / @0x80100c3c `jal
    0x8001a330`. Kanalmitte: ROOM2090.RDT, Kollision @0x6DC; Westwand-Kasten [0] @0x6F4 x -10900 bis -8900 (Innenkante
    -8900), Plattform-Kasten [4] @0x724 x -1700 bis 1850 (Kante -1700); Kanal 7200 breit, Mitte x -5300. Wurfbahn: RE2
    ROOM7020 @0x088E-08A2, v (25,20,10) / a (5,20,0). Kamera-Umschaltung: RE1.5 PSX.EXE FUN_80014230, @0x80014254 `lbu
    v1,22(a0)` / @0x8001425c `bne v1,v0`, @0x80014298 `jal 0x80014368`, @0x800142a8 `lbu a0,1(s0)` -> @0x800142ac `jal
    0x800142f4`. Verdeckung durch die Leiche: Kugel mit 800, der halben Breite des RE1.5-Alligator-Kastens
    (@0x80118b98).
  - Geaendert unter re15_port: 22 Dateien, +1360 / -86 Zeilen samt Tests; Spielcode in climb_common.c,
    gator_treffer_r41.c, enemy_ai_boss_gator.c, gator_re2_r41.c, gatorszene_r41.c, kanister2080_r41.c.
* **Messhaken der Alligator-Nutzerbefunde (nur Diagnose, kein Spielweg):** Neu sind RE15_G2_KAN_PHASEN_ALT=1
  (Kanister-Fenster wie im alten Stand 0b86e36c6: nur in der Verfolgung und beim aufgetauchten Lauern, nicht im Biss und
  nicht im Spurt) und RE15_G2_DASH_BLEND_ALT=1 (Spurt-Abbruch ohne Ueberblendung, wie von 80c7394ab bis vor 3061fe397;
  der Code nennt dafuer den Stand b29263046). Nicht neu ist RE15_K2080_WURF_GIER=<Zahl> aus der Kanister-Spur: Der Haken
  erzwingt eine feste Wurfrichtung, gleich welche - 3072 stellt den alten Nordwurf her, 888 erzwingt den neuen Suedwurf.
  Neue Protokollzeile im debug.log: "[gatorszene] Schluessel-Bild: Kamera ..., Ecken-Kasten ...".
* **Nicht in v0.8.30:** r42/story (Story Teil 1 bis 3; wird gerade mit v0.8.30, r42/hangar_aussen und r41/stimmen zur
  Basis fuer v0.8.31 zusammengefuehrt), r42/hangar_aussen (fe5436f55, Szene "von aussen"), r41/stimmen (73aee131a,
  Option VOICE); Abbruchreste auf r41/sichel_v4r6_abbruch, r41/form_rot_r4_abbruch und r41/form_k5f_r2_abbruch
  (9dfd05b25, "nicht verwendet").
* **Geprueft (Summenzeilen der jeweils letzten Abnahme, Windows):**
  - Abnahme v0.8.30 (Zweig r41/release_0830): erster Lauf auf ea630d75f "99% tests passed, 12 tests failed out of 1567"
    (16366,56 s; Einordnung unter "Pruefstand"); Pruefung der Anpassungen R1 "NICHT BESTANDEN - 1 roter Punkt" (Gruppe
    "100% tests passed, 0 tests failed out of 270"), R2 "BESTANDEN - kein roter Punkt" (Gruppe "100% tests passed, 0
    tests failed out of 291", abgenommene Assets unveraendert); finale Abnahme auf 44cc5b7d5 siehe "Pruefstand".
  - Gorilla-Filme: Unit 114 "100% tests passed, 0 tests failed out of 86"; Durchlauf Elza 1131 -> 1141 -> 1171 "100%
    tests passed, 0 tests failed out of 1" (383,14 s); 11C nach dem Vorspulen "100% tests passed, 0 tests failed out of
    1147" (Teilmenge).
  - Monitore und Wasser: Pruefer R1 Teilsuite "100% tests passed, 0 tests failed out of 186", R2 "100% tests passed, 0
    tests failed out of 7"; Gegenproben mit eingebauten Fehlern rot.
  - Zielhilfe: "100% tests passed, 0 tests failed out of 199" (Pruefer).
  - Pipe: "100% tests passed, 0 tests failed out of 318" (Pruefer); Pipe-Ton "100% tests passed, 0 tests failed out of
    13".
  - Optionen: Bauer und Pruefer in beiden Runden "100% tests passed, 0 tests failed out of 182".
  - Sichel V8: Runde 7 Gruppe "100% tests passed, 0 tests failed out of 8", Unit 1013 von 1013; Schwanz 8g "100% tests
    passed, 0 tests failed out of 18"; Szene mit Schwanz Runde 5 "100% tests passed, 0 tests failed out of 30" (1719,30
    s).
  - Hangar: Rueckbau mit Bit 38 "100% tests passed, 0 tests failed out of 32"; Liegenbleiben "100% tests passed, 0 tests
    failed out of 33" (Pruefer, 1851,68 s); Sherry-Ruf "100% tests passed, 0 tests failed out of 54".
  - Klauenhieb: Runde 3 "100% tests passed, 0 tests failed out of 67"; Ausholen "100% tests passed, 0 tests failed out
    of 53"; Aufrichten "100% tests passed, 0 tests failed out of 44".
  - Riesenform: K5g3 Form-Gruppe je Runde "100% tests passed, 0 tests failed out of 12", Textur-Riegel 47 von 47 (41
    Gegenproben rot); Hand-Einbau Geometrie-Riegel 14 von 15, Textur-Riegel 48 von 48, ctest 1408 von 1411 (die drei
    Roten ohne Bezug zum Einbau); Vorderzaehne Form-Gruppe 27 von 27, Riegel 5 von 5.
  - Integration Welle 1 + WS auf master: Unit-/Eigenschafts-/Sonden-Riegel 1067/1067; 42 Waffenschaden-Integrationsriegel
    (1 Lastausreisser, einzeln 3 x gruen) und 16 Zug-/Querbezugs-Riegel gruen; Gegenpruefung 62 Hunks, 0 gemeinsame
    Dateien (Protokoll 7.11.). WS-Abnahme: "100% tests passed, 0 tests failed out of 1066" (Unit), Integration 42/42;
    Gegenprobe mit selbst uebersetzter alter Engine rot an genau den erwarteten Stellen.
  - Spuren vom 31.10.: Merge O+P "1033 unit/prop/probe, 192 Integrationstests gruen"; Integration Q+R "100% tests passed,
    0 tests failed out of 1051" bzw. 1044 am 1.11., Zielgruppen gruen.
  - Nachtrag 30 (Abnahme 1): "ctest -R r41_nachtrag30 -j3: 11/11 gruen"; "1024/1024 gruen, 3 uebersprungen"; Integration
    mit Spur O: Unit "14", Schrank-Riegel Leon/Elza "2/2", Ventilkette-Wege gruen.
  - Key Items (Abnahme 1): "100% tests passed ... out of 21"; breit "100% tests passed ... out of 1139"; Querbezug 45 von
    46 (rot nur eine Altlast der Basis).
  - Alligator-Reste (Abnahme 1): "22 von 24 gruen", die zwei unter Last roten einzeln 3 x gruen; Nachbargruppe 189 von 190
    (Altlast); Spur selbst "100% ... out of 81"; Trage-Laeufe 306, Sprung ueber 340: 0.
  - Restpunkte (Abnahme 1): "16/16", "97/97", "15/15", "92/92", "1056/1056".
  - Bahnsteig-Birkin: Abnahme 1 nicht bestanden (drei Blocker), Abnahme 3 mit Liste N1 bis N6 (ein Blocker, kein
    Urteilswort); danach Nachbesserung 3 der Spur selbst: "Zielauswahl 75 von 75 gruen". Nicht belegt: Keine Abnahme-4-Datei.
  - Zombie-Arme (Abnahme 1): je Typ "400 Seeds ... Teil veraendert 0"; Gruppe "113/113 Passed".
  - Gore (Abnahme 4): "Unit-Zielauswahl: 326/326 gruen"; "Integration einzeln nacheinander: 14/14 gruen".
  - Handgranate (Abnahme 2): "2/2 gruen"; Integration "4/4 gruen"; volle Suite "1069/1070 gruen" (rot nur ein Zeitlimit
    unter Last, direkt ohne Zeitlimit in Ordnung).
  - Kanalwasser: Abnahme 2 Unit "23/23 gruen", Integration "11/11 gruen"; Abnahme 3 (Deckentropfen) "alle 12 Riegel
    gruen", Gegenprobe mit gestarteter Schleife zeigt Tropfen und Fleck.
  - Kasten/Panel (Abnahme 2): "1066/1066 gruen", PF-beruehrt "44/44", Integration 10/10, 8/8, 13/13, 1/1, 10/10, "Gruppe B
    gesamt (42 Integrationstests) 42/42 gruen".
  - Story: Abnahme 1 (nicht bestanden, M1) und Abnahme 2 (bestanden): Riegel P4a bis P4k gruen, Gegenprobe gegen den alten
    Stand rot; Spur selbst "1078 Tests, 1070 gruen" (rot nur die sieben Skript-Riegel und ein inzwischen gruener Riegel).
  - G5-Auftritt: Abnahme 4 "1068/1068 gruen, 334,8 s", Integration "11/11 gruen"; Abnahme 5 (NB5/NB6): "72 verschiedene
    Riegel ... kein roter Riegel", Lampen gegen RE2-RAM 20/20, Vibration 23/23.
  - Welle 2, letzte Abnahmen: Alligator Abnahme 9 "100% tests passed, 0 tests failed out of 48" (Unit) und 5/5 exe-Riegel
    (1284,8 s); Annette Abnahme 5: Annette-Riegel 4/4, Suite 1040/1040, Integration 8/8, 28/28 und 8/8; Riesenform Stufe 3
    Abnahme 4: Satz D 13/13, Satz A 19/19; Riesenform-Einbau: Form-Gruppe 11/11, Asset-Gruppe 4/4, Einbau-Pruefung Zahlen
    und Sicht bestanden; Sichel V7: Einbau 8/8, Abnahme 0 bestanden (Spur 11/11, Dossier 20/20, Unit 1013/1013,
    Szene 901 von 901 Bildern gleich).
  - Integration Lauf A2 (8.11.): Wagenweg auf 1000 HP 6/6 (zweimal); Zug-Riegel torweg 3/3, annette_g5 1/1,
    Annette/Story-Gruppe 25/25, unit/prop/probe 1083/1083; nach dem Absturz-Fix Pistolen-Riegel gruen (477,88 s), Form
    19/19, unit 1068/1068.
  - Integration Lauf B (8.11., INT_belege/w2b_test/testlauf_summen.txt): unit/prop/probe "100% tests passed, 0 tests
    failed out of 1098" (Kopf eef7bc87b; der erste Lauf hatte 1097/1098 - zwei Teile eines Alligator-Riegels schrieben
    unter -j 6 dieselbe Logdatei, Fix 548b8f3e7); G2 48/48 + 20/20; Form-Satz A 20/20; AN/ST80 30/30; WS/Gator 108/110
    (2 x DEVICELOST im Bildmitschnitt, einzeln je 2 x gruen); Klettern + Kette A+C 31/31; Raum-Querbezug 2090/2091/2080
    8/8; main.c-Fensterriegel 5/5; Form-Asset-Gruppe 4/4; F-Riegel nach Neuvermessung 4/4. Gegenpruefung G2-Merge (908
    Dateien, 1008 Hunks) und Form-Merge (281 Dateien, 3825 Hunks): 0 unerklaerte Zeilen.
  - Kette A+C, Messung 3 (8.11., Kopf 198980793): Leon 2030 -> 20B0 -> 2050 und Elza 2031 -> 20B1 -> 2051, je alle 31
    Pruefungen, Bild fuer Bild wie Messung 2.
  - Integration Lauf C, Sichel V7 (Merge 91636662f, 9.11.): unit/prop/probe 1100/1100; gezielt (Birkin, Form, Bahnsteig,
    G5, Alligator, Waffen, Annette) 108/108; Sichel-Gruppen 12/12 und 21/21; Hangar-Szene mit Software-Renderer ohne
    Absturz; Gegenpruefung: kein Hunk verloren.
  - Labor-Birkin (Fix 145316fc9, Pruefung e181dfac7): Birkin/Labor/Sichel/Form/G5 40/40, unit/prop/probe 1100/1100.
  - Integration Lauf D, Riesenform K5e (Merge db52dd8bf, 9.11.): unit/prop/probe 1100/1100; gezielt 115/115 (zwei Riegel
    mit belegter Ursache auf K5e nachgezogen); Software-Renderer in 5090 ohne Absturz; Gegenpruefung ohne Befund.
  - Fix Sherry 6031 / Annette 6030 (Pruefung ebfc5f0cf): Riegel 2/2 gruen, Gegenprobe rot (Abweichung 311 bzw. 129);
    unit/prop/probe 1100/1100; Annette-, Story-, Torweg-, Kartenbit-, G5-Auftritt- und Zug-Riegel 31/31.
  - Release-Skript-Riegel: laufen in der Abnahme von v0.8.30 wieder mit (siehe "Noch offen", Umgebung).
* **Neue Assets (Paket-Gate):** Gore: GLAS1090_0C.TIM (RE2-Effektbild der Kraehenfedern). G5-Auftritt: R704_CUT09/10
  (Hintergruende, Masken), R704_XA0B/0C (Ton), R704_LAMPE (Modell), R704_BEBEN (Ton), R704.ESP. Nachtrag 30: SUB_34.BGM
  (Sirene), BRUECKE4080 (Bruecken-Motor und Halt); geaendert FILE31_p01/p02 (Notiz). Alligator-Reste: GATOR40A0.VBS
  geaendert (Putz). Restpunkte: MASKS/ROOM6010.MSK, ROOM6011.MSK und zwei Maskenbilder geaendert. Welle 2:
  RE2/GATOR40A0.RBJ (RE2-Raumbank mit den Trefferreaktions-Clips 17-20; seit b27d96891 im Paket-Gate),
  RE2/ANNETTE_4D.EMD, RE2/ANNETTE_4D_BLUT.EMD und RE2/ANNETTE_SE.VBS (Annette), RE2/BAHNSTEIG_FORM.EMD/.TIM (Riesenform
  K5e mit F1-Rand, EMD sha256 f277916d..., TIM 31a35e66..., 512 x 256 mit 10 Farbzeilen, 28 Clips; ersetzt das entfernte
  BAHNSTEIG_G4 der Spur R; K5c gesichert unter varianten/K5/einbau_k5c). EMD/EM30_HANGAR.EMD (Sichel V7, sha256
  319bbd17..., 13 Farbzeilen, Pin release/em30_hangar.sha256, nachbaubar mit re15_port/tools/r41_sichel/v7_kette.sh).
  Keine neuen Paketdateien bei Key Items, Kanalwasser, Zombie-Armen, Kasten/Panel, Handgranate, Waffenschaden und Story.
  Dazu Ihre Aufnahmen unter synchro/ (seit 010f833b9 im Repo, siehe "Fuer Sie zum Aufnehmen"). Seit dem 9.11.:
  MOVIE/R114GORI.STR (7 669 088 Byte) und MOVIE/R11CGORI.STR (8 152 640 Byte) mit den Herkunftstexten
  R114GORI.TXT/R11CGORI.TXT (Gorilla-Filme, Format wie CAPCOM.STR); EMD/EM30_HANGAR.EMD jetzt Sichel V8 mit Schwanz 8g
  (sha256 a02a6df3..., 286 420 Byte, Pin release/em30_hangar.sha256 nachgezogen, nachbaubar mit
  re15_port/tools/r41_sichel/v8_kette.sh); RE2/BAHNSTEIG_FORM.EMD jetzt Riesenform K5g3 mit Hals B4, rechter Hand und
  Vorderzaehnen (sha256 115f8721..., 588 764 Byte) und RE2/BAHNSTEIG_FORM.TIM (sha256 c00823b3..., 138 784 Byte, 15
  Farbzeilen; K5e_f1 gesichert unter varianten/K5/einbau_k5e_f1); PLD/PL00W02.PLW und PLD/PL04W02.PLW (Rohr fuer Leon
  und Elza, sha256 d544eb73... bzw. 3c48af24..., erzeugt mit re15_port/tools/pipe_r41/gen_pipe_plw.py); DISC40/ mit
  sechs unveraenderten Dateien der 40%-Disc fuer ULTIMATE 1.5 (DATA/ITEMALL.PIX, DATA/MIXITEM.PIX, ITEM/ITPS.ITP,
  STAGE1/ROOM1020.RDT, STAGE1/ROOM11B0.RDT, STAGE4/ROOM4010.RDT; in .gitattributes "DISC40/** -text"). Keine neue
  Tondatei: Pipe-Ton und Sherry-Ruf kommen zur Laufzeit aus der Raumbank von ROOM3070. Ein eigenes Paket-Gate haben
  davon nur EM30_HANGAR.EMD (Pin) und BAHNSTEIG_FORM (Liste).
* **Speicher-Bits und Text-Ids:** Bank 9 Bit 101 heisst jetzt "Druck 80 erreicht" (Nachtrag 30), Bit 125 (Sicherung im
  Kasten) unveraendert; Restpunkte nutzen das vorhandene Bit 111 (Alligator tot); neu Bit 128 (Story, Zug 6030:
  Fuehrerstand vor dem Wagen besucht, Nachtrag steht aus; VERTRAG 1.1) und Bit 133 (Annette: Zugstart gesehen).
  Port-Texte: 2030/2031 Ids 40 (zwei Seiten), 41, 42, 47 neu belegt (vorher Ventil 13); 1150/1151 Hebetisch Id 21 statt
  20; 6030 Ids 21 bis 26, 31 und 40 (Story), 27 bis 30 (Annette); 5090 Ids 40 bis 43 (Annette). Neu seit dem 9.11.: Bank
  9 Bit 38 (Hangar-Szene gesehen) und Bit 39 (Hangar-Birkin besiegt), im Original frei; die Optionsdatei
  re15_options.cfg bekommt bei ULTIMATE 1.5 AN die Zeile "ultimate=on".
* **Titelbild:** Es zeigt die Versionsnummer statt "Sample Ver.2025.01.25" (seit v0.8.27); die Nummer kommt beim
  Release-Bau aus release_alles.sh (CMake RE15_VERSION), ein lokaler Bau zeigt den letzten Tag mit "dev". Nach dem Bau im
  Titelabzug pruefen, dass "Ver.0.8.30" dasteht - Windows, Linux und Android.
* **Nicht geprueft:** PSX-Bau fuer alle Spuren dieser Fassung (Story und Annette nur Syntaxpruefung), Android-Bau,
  Linux-Suite im Container (laeuft beim Release-Bau), Bildvergleich mit dem laufenden RE2 fuer Putz, Finisher und
  Kanister-Sitz (kein Abbild), DuckStation-Lauf fuer das Kanalwasser und die Effekt-Quader 20B0, Gamepad-Vibration im
  G5-Auftritt mit echtem Pad. Seit dem 9.11. ausserdem nicht geprueft: der Einbau der Gorilla-Filme durch einen
  unabhaengigen Pruefer und ob DISC40 und die Filme im Android-Paket ankommen; auf der PSX gibt es die Gorilla-Filme,
  die Monitor-Streifen und den Sherry-Ruf nicht.
* **Schalter (Umgebung bzw. Datei):** RE15_KEIN_80PROZENT_GORE=1 = Schalter gegen das 80%-Gore (Platzen der Spinne und
  MC51-Beinzelle aus; bei der Handgranate Zerfetzen, Teile-Wurf, Liegenbleiben, Rauch-Nachlauf und Werfer-Platzer aus,
  der Schaden bleibt); RE15_GATOR_ORIGINAL = ausgelieferter Raum 2090 mit RE1.5-Alligator ohne Port-Szenen; die
  Options-Datei der Lautstaerke ist re15_options.cfg. RE15_ULTIMATE15=0/1 ueberschreibt beim Laden die gespeicherte
  Einstellung ULTIMATE 1.5; RE15_CONFIG_AI=2 und RE15_AI_FLAVOR=m/mixed landen seit v0.8.30 bei RE2.
* **Messschienen (nur Diagnose):** RE15_KEYITEMS_LOG, RE15_USE_TEST (Key Items); RE15_SE_DEBUG (Sirene); RE15_ZGORE_TEST
  (MC51-Beinzelle); RE15_G5A_SPUR (nur vom Testprogramm des G5-Auftritts gelesen); RE15_BB_POSE, RE15_BIRKIN_HP_AT,
  RE15_BIRKIN_CUT (Bahnsteig-Birkin); RE15_GATOR_ORIGINAL (ausgelieferter Raum); RE15_SLOTDECKUNG (Riesenform Kamera);
  RE15_SWTRI_LOG (Software-Renderer, Ueberlauf je Dreieck); fuer durchgehende Laeufe ueber mehrere Raeume
  RE15_FLAG_WATCH, RE15_FIRE_AOT als Liste, RE15_EXIT_AT mit "!k" und Eingabeskripte mit einem Abschnitt je Raum; Zeilen
  [keyitems], [schrank2010], [panel2030], [wasserlauf41], [tropfen41], [gpu-grenze], [sw-teilung], [tim] (Upload-Fehler
  einer Textur ueber der Grenze des Renderers), [flow] AUSNAHME im debug.log. Seit dem 9.11.:
  RE15_ZWS_VORSPULEN_SICHTBAR=1 (Vorspulen in 11C0 sichtbar, nur Test), RE15_PIPE_LOG, RE15_BILD_HUD (Bildnummern im
  Fenster), RE15_BIRKIN_DBG, RE15_POSE_LOG, RE15_SCHWANZ_MASKE, RE15_SCHWANZ_LOG, RE15_HANGAR_BIRKIN_HP,
  RE15_TRILISTE_GRENZE, RE15_LACHE_LOG (jetzt mit dreiecke=); Zeilen [clutanim], [zws114]/[zws11c] und [tri-liste]
  UEBERLAUF/ENDE im debug.log; die Messhaken der Alligator-Befunde stehen oben.

# v0.8.29 - 2026-10-26

Ihr Auftrag vom 23.10. (Runde 41): "Gehe die von vorhin besprochenen noch offenen Punkte an, aslo: Runde 41 startet mit
dem Wasserschwall in 20B0 im Original. Danach folgen die Ventil- und Brueckenkette in STAGE2 und das Schliessen der
STAGE3-6-Masken, wie mit Ihnen vereinbart." - dazu Ihre weiteren Punkte vom selben Tag und 33 Nachtraege bis zum 25.10.
Wie gewohnt hat ein zweiter Agent jeden Punkt unabhaengig am gebauten Spiel gegen Ihren Wortlaut nachgemessen; wo eine
Abnahme nicht bestand, wurde nachgebessert und erneut abgenommen. Fuer den Alligator und die Masken galt in der letzten
Abnahme Ihr Wort vom 25.10. ("gut ist erst einmal gut genug" und "mit neuen Tokens am Samstag die Sachen nachtraeglich
zu verbessern"): Aufgehalten haette dort nur noch ein blockierender Fehler (Absturz, Haenger, Rueckschritt, ein Punkt
fehlt oder ist im Spiel kaputt). Es wurde keiner gefunden. Die letzte Masken-Abnahme lautet trotzdem "nicht
bestanden", weil sie drei kleinere Maengel fand; keiner davon ist blockierend, nachgebessert wird dort erst ab Samstag.
Was noch nicht stimmt, und Ihr angehaltener Nachtrag 30 stehen unter "Ab Samstag (neue Tokens)".
Geprueft unter Windows im Integrationsbaum: alle 1003 Unit-, Eigenschafts- und Sonden-Riegel und die gezielten Spiel-Laeufe
jeder Spur und ihrer Querbezuege - gruen (die vier Schluessel-Riegel nach dem Alligator-Tod nach Entscheid FG-E7 umgestellt).
Die volle Suite laeuft beim Bau im Linux-Container; ein roter Test haelt das Release an.
Die volle Suite laeuft beim Bau im Linux-Container; ein roter Test haelt das Release an.

## Was Sie merken

### STAGE2-Kanalisation: Ventile, Flut und Strom

Ihr Entwurf vom 23.10. ("Meine Idee war zunaechst in ROOM 2000 die Leiter zu ROOM 2050 mit herunterlaufenden
Wasserstrom zu blockieren mit einer Message wie "I cannot climb up the ladder because of the water pressure".") ist als
Kette eingebaut, mit allen Nachtraegen dazu bis auf Nachtrag 30 (siehe "Ab Samstag"); dieser Abschnitt beschreibt den
Stand von v0.8.29. Jeder Punkt der Kette wurde mit Leon und mit Elza (Raeume 20x1) ueber die echten Tueren nachgemessen.
Eine zusaetzliche Tuer gibt es nicht (Ihr Nachtrag 6: "Nein, du darfst keine zusaetzliche Tuer oeffnen um die Kette zu
loesen.").

**ROOM2000: Leiter gesperrt.** Bis die Flut gelaufen ist, laeuft Wasser die Leiter nach 2050 herunter; die Aktionstaste
zeigt Ihren Satz "I cannot climb up the ladder because of the water pressure."

**ROOM2030: Bruecke, Pult und Ventil 13.** Die Bruecke steht zu Beginn oben, auf Hoehe der Ringebene. Das Pult an der
Nordwand (Kamera 2, Ihr schaltschrank.png) meldet "A control panel for the bridge. The pressure gauge reads zero." und
"Intake valve 13 is closed.". Die Treppe hinab, am Tank: "Intake valve 13. Will you open it?" -> "You've opened valve 13.
The pressure is rising.". Zurueck am Pult: "The pressure gauge reads 80. Lower the bridge?" - mit Ja faehrt die Bruecke in
der Draufsicht herunter ("The bridge is down."). Die Kurbel braucht es dafuer nicht, wie Sie es wollten.

**Sued-Landeplatz: Sicherung, Notiz und Plakat.** Ueber die gesenkte Bruecke erreichen Sie den Sued-Landeplatz. Dort liegt
die Sicherung an ihrem Original-Platz; daneben haengt ein beschriebener Zettel an der Wand: das Dokument "Technician's
Note" mit Ihrem Text woertlich (im Dokumentleser eine Titelseite und zwei Textseiten; den Gedankenstrich schreibt die
RE2-Dokumentschrift als Bindestrich, wie RE2 selbst). An der Suedwand zwischen dem gelb-schwarzen Pfosten und Ventil 09
haengt das Plakat. Die Aktionstaste zeigt es als Nahaufnahme im Stil der Nahaufnahmen des Spiels (Stahlwand, vergilbtes
Papier, Schreibmaschinenschrift, Flecken, Klebeband - Ihr Nachtrag 18) mit der absichtlich falschen Anleitung
"1. GREEN 09 - 2. RED 01 - 3. BLACK 02 - 4. RED 07". Am Ostende des Landeplatzes liegen Shotgun Shells x14 - nehmen Sie
sie vor der Flut mit (siehe unten).

**ROOM2010/2011: der Ventil-Sicherungskasten (Ihr Nachtrag 16: "Kannst du evtl. noch einen zweiten Sicherungskasten
irgendwo einbauen - ausserhalb des Raumes, wo man die Sicherung zuerst einsetzen muss.").** An der Ostwand haengt ein
Schaltschrank mit kleinem Tastenfeld (Kamera 3). 2010 erreichen Sie vor der Flut ueber 2000, danach ueber 20B0, jeweils
durch die Tuer im Westgang. Aktionstaste: "A fuse box for the valve controls." / "One of the fuses is missing..." ->
"Will you use the Fuse?" -> Ja -> eine kurze Einstellung auf die Ventile in 2030 (das Hintergrundbild der Ventil-Kamera
mit den Raedern 09/08/07 und dem Plakat) mit dem Aufschliess-Ton aus RE2 -> "You've used the Fuse.". Herausnehmen geht
jederzeit, auch nach der Flut: "The fuse is in place." / "Will you take the Fuse?" -> "You've taken the Fuse."; danach
sind die Ventile wieder gesperrt, und die Sicherung gehoert wie im Original in den Generator-Kasten von 2060. Solange sie
im Ventilkasten steckt, meldet der Kasten in 2060 "One of the fuses is missing...". Bis auf "A fuse box for the valve
controls." (Ihre Freigabe, Nachtrag 25) sind alle Saetze aus dem Spiel.

**Das Ventilraetsel (Ihre Nachtraege 13, 15 und 18).** Im unteren Bereich von 2030 stehen sechs Ventile, je ein Rad unter
einem Nummernschild: Nordwand 03 rot, 02 gruen, 01 schwarz; Suedwand 09 rot, 08 gruen, 07 schwarz. Vor einem Rad fragt
das Spiel z. B. "Valve 09, red wheel. Will you turn it?". Mit Ja spielt die Figur die Ventil-Dreh-Animation aus RE2 (das
Wasserdruck-Ventil am Hubschrauber-Landeplatz, RE2 ROOM1080) mit dem RE2-Drehgeraeusch, je Drehung etwa 11,5 s wie in
RE2. Erst nach der vierten Drehung wird geprueft: richtig ist 09 -> 07 -> 02 -> 01 (Ihre Freigabe vom 24.10.,
Nachtrag 15); bei falscher Folge kommt "The valves turned back to their original position." mit Fehlerton, und alle
Drehungen sind zurueckgesetzt. Drehen
lassen sich die Ventile erst, wenn die Bruecke unten ist (Ihr Nachtrag 9) und die Sicherung im Kasten in 2010 steckt;
sonst sagen sie "It's locked." (ein Satz aus ROOM20A0) - auch wenn die Figur die Sicherung bei sich traegt.

**Warnung, Countdown und Alarm (Ihre Nachtraege 18, 19 und 25).** Nach der richtigen Folge erscheint "WARNING: The water
will rise in one minute. Please leave the room immediately!" (Ihr Wortlaut, drei Zeilen) mit Ihrem Voiceover. Zugleich
startet RE2s Selbstzerstoerungs-Alarm (SUB_25, wie in RE2s Labor) zusaetzlich zur Raummusik, und oben in der Mitte laeuft
die Countdown-Anzeige von RE2s Selbstzerstoerung: " 0'59"98" bis " 0'00"02", im Augenblick der Flut " 0'00"00", in der
RE2-Schrift und rot (RE2 zeigt erst ab 60 Sekunden gruen). Die letzten zwei Stellen zaehlen wie in RE2 je Bild (98, 95,
92, 88, ...); die Anzeige wurde Pixel fuer Pixel gegen das echte RE2 verglichen. Der Alarm endet, wenn das Wasser zu
steigen beginnt, wenn die Figur stirbt oder wenn sie den Raum verlaesst (dann blendet er mit der Raummusik aus). Waehrend
des Countdowns zeigen die Ventile die Warnung erneut.

**Flucht oder Ertrinken.** Von den letzten Ventilen an der Nordwand sind es zu Fuss ueber Treppe und Ring bis zur
Ost-Tuer etwa 27 s. Wer im Raum bleibt, sieht in der Draufsicht das Wasser ueber Bruecke und Ring steigen und ertrinkt
mit Spritzer und blauem Schleier (ohne eigenen Ton). Zum Ansehen vorher die drei Spinnen des Raums erledigen, sonst
toeten sie die Figur zuerst. Verlaesst man 2030 waehrend des Countdowns durch eine andere Tuer, gilt die Flut ebenfalls
als gelaufen.

**ROOM20B0/20B1: der Wasserschwall, im Original gemessen (Ihr Hinweis: "Uebrigens ist das in unseren Port noch
kaputt!").** Durch die Ost-Tuer landen Leon bzw. Elza in 20B0/20B1: aus der Ost-Tuer schiesst das Wasser (zwei Kaskaden
und sechs grosse Spritzer je Front), die Front laeuft den Gang hinunter, die Westtuer fliegt auf, und die Szene laeuft bis
zum Ende - die Figur ist danach frei. Vorher (20B0 war nur per Debug-Sprung erreichbar) war dort kein Wasser zu sehen,
und die Figur blieb unter der Szenensperre vor der Westtuer stehen. An sechs Zeitpunkten der Original-Szene
(DuckStation-Savestates) sind Kameraschnitt, Lage, Blickrichtung und Zustand der Figur gleich, ebenso die Zahl der
Spritzer zum selben Zeitpunkt (22 = 22). Hinweis fuer eigene Vergleiche: ein DuckStation-Video glaettet die
Spritzerwolke (Skalierung und Videokompression) zu einer helleren, dichteren Flaeche; gleich ist der Port am
Bildspeicher eines Savestates.

**Danach: 20B0 statt 2000.** Im Kanal laeuft kein Wasser mehr die Leiter herunter; sie fuehrt jetzt nach 2050. Jede Tuer,
die nach 2000 fuehrte, fuehrt von nun an nach 20B0. Solange 2030 geflutet ist, sind die Tueren in seinen Ring (Ost-Tuer
von 20B0, Tueren aus 2070) gesperrt: "I cannot open the door because of the water pressure."; die oberen Lift-Rufzonen
in 2030 melden in dieser Zeit den Original-Satz "The elevator is not responding...Maybe it should be activated from
somewhere.".

**ROOM2050/2051: erst das Pult, dann der Hebel.** Solange das Pult nicht auf 80 V steht, meldet der Hebel fuer den
Wasserstand "The pump has no power. The switchboard must be set first." - das gilt auch fuer den Hebel nach dem
Kistenraetsel. Am Pult an der Saeule: "A high voltage switchboard. The pump needs 80 volts." und "Will you push them?".
In der Bedienansicht waehlt das Steuerkreuz einen Hebel, Viereck legt ihn um, Kreuz verlaesst das Pult. Die Lampen
unten zeigen den Zustand, und der umgelegte Hebel kippt jetzt sichtbar in 16 Bildern nach unten (beim Zuruecklegen
wieder hoch; Takt der Original-Hebel in 11F0) - Ihr Nachtrag 18: "bei den ausgewaehlten hebel, muss dann auch der Heben
runter gehen wenn er ausgewaehlt wurde.". Bei 80 V folgt "Power supply OK."; danach arbeitet der Hebel wie im Original,
und das Pult meldet "The switchboard is set to 80 volts. The pump is powered.".

**ROOM2020: die Kurbel (Ihr Punkt "Leon hat eine komische herumfliegende Animation im ROOM 2020 am Anfang wenn er die
Kurbel nutzt." und Nachtrag 14: "Ja, direkt am Anfang springt Leon so komisch zu der Kurbel. So halb fliegend.").** Nach
dem "Yes" bleibt das Bild jetzt so lange stehen wie im Original, das in dieser Zeit den Hintergrund von CD laedt - am
Fenster gemessen 250-267 ms (Original-Video 284 ms, vorher 75 ms) -, erst dann steht Leon an der Kurbel, ohne
Zwischenbild. Der Sprung selbst ist Original: das Skript setzt Leon an die Kurbel. Ausserdem kurbelt Leon jetzt
durchgehend, bis die Buehne unten ist (vorher spielte er die Kurbel einmal und blieb vorgebeugt stehen), und waehrend der
beiden Fragen bleibt er in der Pose, in der er angekommen ist. Kommt er gerannt, rutscht er nach dem "Yes" noch ein
Stueck weiter und kurbelt etwa 35 cm daneben - wie das Original. Steht Leon im Gang genau auf der Grenze zweier
Kamerazonen (z. B. bei x 9000), flackert das Bild nicht mehr zwischen zwei Kameras.

**Strom aus 2060: 2030 laeuft bis knapp unter die Absperrung leer (Ihre Nachtraege 7, 8 und 12: "Wasserstand nach dem
Strom:: unterhalb der Absperrung, sodass es nicht drueber "rein" laufen wuerde.").** Nach der Flut holen Sie die
Sicherung aus dem Kasten in 2010 und setzen sie wie im Original in 2060 ein. Mit dem Strom steht das Wasser in 2030 nur
noch 50 Einheiten unter dem Ringboden, knapp unter der Absperrung: der Ring ist trocken und begehbar (Lifte und
Ring-Tueren wieder offen), Bruecke, Landeplaetze, Treppe und Tank bleiben unter Wasser. Die Treppe hinab sagt "I cannot go
down the stairs because of the water.". **Nach der Flut nicht mehr erreichbar:** die Shotgun Shells x14 am
Sued-Landeplatz sowie Plakat und Notiz, falls noch nicht gelesen.

**Weg nach der Flut (zu Fuss gemessen, Leon und Elza):** 20B0 -> 2010 (Sicherung heraus) -> 20B0 -> Leiter -> 2050
(Pult, Kisten) -> 2020 (Kurbel) -> 2030 Nordgalerie -> 2060 (Sicherung, Generator) -> zurueck ueber 2020, 2050 und die
Leiter nach 20B0 -> Ost-Tuer -> 2030 Ring (trocken) -> Lift B -> Suedgalerie -> 2080 -> Rechner (wie im Original: senkt
das Wasser in 2080) -> Leiter -> 2090. Dort geht es weiter wie unter "Alligator" beschrieben: Ankunftsszene, Kampf, dann
mit dem Water Key durch das Tor nach 20A0.

### Wasser: Sprung und NPCs

Ihr Punkt: "In STAGE2 fehlt noch der Wassereffekt vom rein springen von der Kante in das Wasser. Auch die NPCs teilen
die gleichen Effekte wenn sie durch das Wasser gehen." und Ihr Nachtrag 3: "ja alles: Effekte wenn sie sich im Wasser
bewegen fussschritt sounds wenn sie laufen. All das muss auch fuer die NPCs gelten."

**Sprung ins Wasser.** Springt die Figur von einer Kante ins Wasser (z. B. im Kanal von 2000 oder ins Becken von
2090/2091), spritzt es an der Landestelle (acht Spritzer im Faecher). Das Tor von 20A0 nach 2090 oeffnet jetzt erst der
Water Key (siehe Alligator).

**NPCs im Wasser und an Land.** Alle NPCs (u. a. Ada, Marvin, Irons, Roy/Elliot, John, Sherry) machen beim Gehen, Laufen,
Rueckwaertsgehen und Drehen auf der Stelle Schrittgeraeusche und spritzen im Wasser wie der Spieler - in Szenen und im
freien Lauf, in beiden Szenarien. Zu sehen z. B. in der Szene in ROOM2000, wenn Ada und Marvin durch den Kanal gehen (in
ROOM2001 liegen Johns Kanal-Schritte hinter der Kamera). Der Ton ist der NPC-Ton des Originals, wo der Raum einen hat
(15 von 280 Boden-Faellen der 52 NPC-Raeume), sonst - wie in RE2 - derselbe Bodenton wie beim Spieler; stumm bleiben nur
Bodenstellen, an denen auch der Spieler keinen Ton hat (ROOM6030/6031). Lautstaerke und Seite folgen der Kamera wie im
Original: ein NPC weit hinten ist leiser (ROOM2000: Abstand 41 -> Pegel 104, Abstand 125 -> Pegel 56 von 127), ein NPC
links oder rechts klingt auf dieser Seite staerker. Ihre eigenen Schritte klingen noch wie bisher (siehe "Noch offen").

### Alligator: Ankunft, Kampf und Kanister (ROOM2080/2081, ROOM2090/2091)

Ihr Wortlaut: "Generell: Du kannst fast alles vom Aligator Fight von Resident Evil 2 uebernehmen." und "Ich will das du
den Finisher und fehlende Animationen aus Resident Evil 2 extrahierst!" - und am 25.10.: "gut ist erst einmal gut genug".
Was am Alligator noch nicht stimmt, steht unter "Ab Samstag".

**Ankunft in 2090: die neue Szene (Ihr Punkt "Ich moechte eine neue Cutscene: Wenn man Room 2090 betritt, soll der
Charakter einen Zombie am Boden liegen sehen mit einen blinkenden Schluessel ..." und Ihre Nachtraege 6, 17, 20, 21 und
29).** Zum Alligator kommt man ueber die Leiter aus 2080, sobald der Rechner dort das Wasser gesenkt hat (Nachtrag 21:
"man kommt ueber die Leiter zum Aligator, nicht ueber das Tor"). Die Szene laeuft in der Folge Ihres Nachtrags 6: Leon
bzw. Elza steht an der Leiter, vorn im Becken nahe der Suedwand treibt die Zombie-Leiche, auf ihrem Ruecken der
Schluessel, der blinkt wie der Feuerloescher in 1030. Dann knallt es von oben - derselbe Knall wie am Ende der
STAGE1-Szene nach Irons (Nachtrag 17) -, ein Schnitt zeigt 2080, wo acht Zombies oben auf dem Laufgang laufen, nicht
unten an der Treppe (Nachtrag 20), danach wieder die Leiche. Der Alligator springt am Ostende aus dem Wasser, stoesst auf
die Leiche zu und frisst sie samt Schluessel (von vorn zu sehen), schuettelt sie, schluckt, dreht sich zu Leon bzw. Elza
und schreit. Dann geht die Kamera zurueck an die Leiter, und der Kampf beginnt mit der Kampfmusik des RE2-Alligators.
Damit man am Start des Kampfes eine Chance hat (Nachtrag 29: "... muss weiter weg sein, damit man am Start des Kampfes
eine Chance hat"), treibt die Leiche rund 8000 Einheiten von der Leiter entfernt: Beim Kampfbeginn ist der Alligator rund
11450 Einheiten weg, wie in RE2 am Ende seines Alligator-Intros (11430). Wer stehen bleibt, wird nach etwa 4,4 s gebissen;
wer sofort nach Norden durch den Westkanal rennt, hat gut 6 s bis zum ersten Biss. Gemessen mit Leon und Elza. Die
Spinnen sind aus 2090/2091 entfernt ("Entferne die Spinnen aus dem ROOM 2090, die nutzen wir nicht mehr."). Noch nicht
umgesetzt ist Ihre Antwort "Aendern: nicht vor der Kamera" (Nachtrag 31) - siehe "Ab Samstag".

**Verfolgung wie RE2.** Der Alligator schwimmt und paddelt mit den RE2-Animationen und dem RE2-Tempo (Paddeln etwa 149 je
Bild), beschleunigt an denselben Stellen wie in RE2 (Dash mit Bruellen) und blendet zwischen den Animationen wie RE2
ueber. Beim Dash wackelt das Bild wie in RE2 (der Hintergrund ruckt), und an den RE2-Stellen - beim Start eines Dashs -
faellt sichtbar Putz um Leon (sechs Runden zu je neun Brocken). Ueber die Plattform hinten klettert er weiterhin. Wo er
Leon nicht erreicht (Tuernische, oben auf der Rampe), bleibt er an der Wand und schaut zu ihm, statt sich ruckartig
umzudrehen.

**Kameraschnitte wie RE2 (Ihr Nachtrag 4: "Ja, ich moechte RE2-artige Kameraschnitte").** Fuer das Fressen in der
Ankunftsszene nimmt der Port die Kamera von 2090, die den Alligator von vorn zeigt. Das RE2-Intro mit seinen Schnitten
(erst von vorn, dann seitlich und zum Angriff erneut, danach zurueck auf Leons Kamera) laeuft nur noch, wenn Sie 2090
nach der Szene erneut betreten und der Alligator noch lebt. Bei der Kiefer-Explosion bleibt die Kamera, wenn das laufende
Bild Kopf und Kiefer zeigt; sonst schneidet sie auf eine Kamera von 2090, in der Kopf und geplatzter Kiefer waehrend der
ganzen Todesanimation zu sehen und von keiner Maske verdeckt sind (unter diesen auf die, die RE2s Bild am naechsten
kommt), und am Ende zurueck. Beim Finisher schneidet es immer. Neue Hintergruende werden dafuer nicht erfunden - wo 2090
keine Kamera vor dem Kopf hat, ist die Sicht seitlich.

**Vor seinem Auftritt** ist der Alligator nicht zu sehen und nicht zu treffen - wie in RE2 liegt er unter Wasser, bis er
auftaucht (beim ersten Mal in der Ankunftsszene).

**Treffer.** Wie in RE2 toeten Schuesse allein ihn nicht mehr. Nach einem Treffer zeigt er mit 25 % Wahrscheinlichkeit
die grosse Treffer-Reaktion aus RE2; danach kommt sie wie in RE2 fuer 120 bis 183 Bilder nicht erneut. Sterben kann er
nur ueber den Gaskanister. Ab 300 Schaden kommt dafuer die Kanister-Szene (unten).

**Die Kanister-Szene in 2080/2081 (Ihr Punkt "Hat man den Aligator oft genug angeschossen, soll eine Cutscene Triggern."
und Ihre Nachtraege 5, 22, 23, 24b und 26).** Hat der Alligator 300 Schaden genommen (die Schwelle aus RE2, Ihre Wahl in
Nachtrag 22; nach seiner Schadenstabelle etwa 43 Treffer mit der Pistole, 11 mit der Schrotflinte, 7 mit der Magnum, 5
mit dem Granatwerfer), schneidet das Bild, sobald er Leon bzw. Elza verfolgt und nicht gerade beisst, nach 2080 bzw.
2081: Mit dem Ton der Stahltuer (Nachtrag 22: "Stahltuer-Ton lassen") kommt Marvin (Leon) bzw. John (Elza) herein und
erschiesst die acht Zombies oben mit je einem Kopfschuss - der Kopf platzt, der Zombie faellt. Dann rennt er zum
Gaskanister, der von Anfang an am Rand der Reling steht wie auf Ihrem Gas_Can.bmp, kniet sich hin, nimmt ihn mit beiden
Haenden (der Kanister kippt dabei sichtbar in die Haende, wie in RE2, Nachtrag 26) und steht damit auf; die
Kletter-Animation davor ist weg (Nachtrag 24b). Mit dem Gesicht zur Kamera, wie Ada in RE2, sagt er "Hey, Leon, try with
this!" bzw. "Hey, Elza, try with this!" (gut 3 s; liegt Ihre Aufnahme vor, wartet er, bis sie zu Ende ist) und wirft den
Kanister ueber die Reling. Ein Schnitt zeigt aus Kamera 12 in 2090 (von unten an der Wand unter dem Balkon, Ihre Wahl in
Nachtrag 26), wie der Kanister ins Wasser faellt, mit dem Aufschlag-Ton aus RE2; eine halbe Sekunde danach steuern Sie
wieder Leon bzw. Elza. Eine Kamera wie bei Adas Raketenwerfer-Wurf in RE2 (Nachtrag 5), die die Werferin von unten
zeigt, gibt es in 2080/2090 nicht; Marvin bzw. John selbst ist in Kamera 12 nicht zu sehen. Der Kampf geht mit demselben
Alligator weiter, Lebenspunkte und Lage bleiben (Nachtrag 23: "Kampf geht weiter, Aligator nimmt irgendwann in dem
weiteren Rundlauf den Kanister auf"). Der Kanister fliegt in das Becken, das weiter vom Alligator entfernt ist; der
Alligator schwimmt zu ihm und schnappt ihn etwa 3 bis 5 Sekunden nach dem Wurf. Wer vor der Kanister-Szene die Leiter
wieder hinaufsteigt, hoert beim ersten Betreten von 2080 bzw. 2081 den Knall, und die acht Zombies stehen oben; die
Treppe gehen sie nicht hinunter.

**Kanister und Kiefer (RE2-Mechanik).** Liegt der Kanister im Wasser, schwimmt der Alligator darauf zu, richtet Hals und
Kopf auf ihn (RE2-Lenkung, Ihre Wahl in Nachtrag 26) und nimmt ihn schwimmend auf; bis dahin schnappt er nicht nach Leon
bzw. Elza. Der Kanister sitzt in jedem Bild zwischen den Kiefern. Mit ihm im Maul steckt der Alligator Kopf und Kanister
nicht mehr durch die Beckenwand und kommt dafuer an einer Wand etwas weniger weit an die Figur heran (beissen kann er
weiterhin). Ein Treffer, waehrend er den Kanister im Maul traegt, loest den RE2-Tod aus: halbe Bildrate (Zeitlupe, etwa
2,5 s) und die ganze RE2-Explosion (Ihr Nachtrag 28: "Beim Ausschnitt gerade eben fehlte noch der Explosionseffekt vom
Gaskanister vom Original.") mit grau-weisser Wolke, dunkelrotem Blutausbruch mit gluehenden Brocken und Fleischfetzen,
Wasserspritzern und einer Blutfontaene, die gut zwei Sekunden aus dem Kopf steigt; der Kanister zerplatzt in dunkle
Scherben. Die Explosion ist jetzt in jedem gemessenen Verlauf zu sehen, auch an der Wand. Der Unterkiefer platzt in
dunkelroten Scherben, danach liegt die Leiche mit geplatztem Unterkiefer im Wasser; durch die Beckenwand rutscht sie nicht
mehr. Neue Toene: die RE2-Raumbrueller fuer Tod und Explosion.

**Tod durch den Alligator (RE2-Finisher).** Stirbt Leon oder Elza durch den Alligator - auch durch den Hochbiss auf der
Rampe oder dem Plattformblock -, schneidet das Bild immer auf eine Kamera, die den Alligator von vorn und nah zeigt. Der
Ort ist fest (Westkanal mit Kamera 6; laeuft gerade Kamera 6, das Suedostbecken mit Kamera 1), so wie RE2 seinen Finisher
an einem festen Ort spielt. Die Figur steht bis zur Huefte aus dem Wasser, der Alligator packt zu, schuettelt, wirft sie
hoch und behaelt sie im Maul, bis das Spiel zum Game Over wechselt (der RE2-Finisher endet nicht); keine Weissblende und
keine kreisende Todeskamera. Elzas Zopf bleibt dabei am Kopf.

**Nach seinem Tod: Schluessel und Tor (Ihr Punkt "Wenn der Aligator danach tot ist, soll der Schluessel in der Naehe
seine Leiche sein und blinken." und Nachtrag 21: "Das Tor soll verschlossen sein, und dann mit den Schluessel den man vom
Aligator bekommt geoeffnet werden!").** Nach der Todesanimation schneidet das Bild fuer 1,5 s auf die Kamera, in der der
blinkende Schluessel neben der Leiche am groessten zu sehen ist (Balken oben und unten, Leon bzw. Elza stehen still),
dann zurueck. Aufheben mit der Aktionstaste an der gezeigten Stelle: "Water Key" - ein Schluessel, den das Original in
seinen Daten hat, aber nie auslegt (Beschreibung: The label says: "Sewer Mainteinance Room.", so geschrieben im
Original); im Wasser liegt er als RE2-Modell "Platform Key". Das Tor zwischen 2090 und 20A0 ist bis dahin verschlossen
("It's locked."); mit dem Water Key kommt "You've opened the lock." und die Frage, ob Sie ihn wegwerfen, danach ist das
Tor offen. Von 20A0 aus bleibt es bis dahin ebenfalls verschlossen. Beide Saetze sind Original-Saetze von 20A0.

### Zombies am Boden (ROOM2000/2001, 3000/3001, 3010/3011)

Ihr Punkt: "In ROOM 2000 und ROOM 2001 ist die Animation/Aktion der Zombies durchgehend falsch. Sind sind am Boden und
fressen, anstatt aufzustehen wenn man ihnen naeher kommt."

**Liegen statt fressen.** Diese Zombies sind im Original Liegende mit Naehe-Wecker, keine Fresser. Jetzt liegen sie
flach - in 2000/2001 im Abwasserkanal, in 3000/3010 auf dem Boden - und fressen nicht mehr. Kommt die Figur naeher als
4000 Einheiten, stehen sie nach hoechstens einer halben Sekunde auf und greifen an; im Kanal ziehen sie sich dazu an der
Kante hoch. Solange sie liegen, sind sie wie im Original nicht zu treffen; weiter entfernte bleiben liegen. Die echten
Fresser im Speisesaal (1140) und in 10E0 bleiben Fresser. Mit der RE2-KI (Voreinstellung) schnappt ein Kanal-Zombie, der
direkt unter der Figur steht, nach oben (RE2-Verhalten); einen Schritt vom Kanalrand weg laeuft er zur Kante und
klettert hoch. Mit "AI RE1.5" liegen diese Zombies (und die Liegenden in STAGE1, z. B. 1070/1140) vor dem Aufstehen in
der Original-Liegepose (flach, Arme abgespreizt).

**Laden in 2000/2001.** Ein Spielstand, der in ROOM2000 (Leon) bzw. ROOM2001 (Elza) geladen wird, zeigt die beiden
Kanal-Zombies jetzt wie beim Betreten durch die Tuer (vorher fehlten sie). Dieselbe Korrektur gilt fuer alle Raeume, deren
Raumskript beim Betreten sofort eine Szene startet (z. B. 1021, 1031, 4010, 4031, 5011): beim Laden laeuft der Raum wie
beim Betreten an, wie im Original. Nur der Hubschrauber-Vorspann in 1170 haelt den Raumablauf weiter bis zum Ende der
Erzaehlung zurueck.

### STAGE3-Szenen (3060, 3061, ab 30C1, 3091)

**ROOM3060 (Leon) - Ihr Punkt "Auch die Animation von Marvin, am Ende seine Abschnitten in STAGE3, bevor zurueck
gewechselt wird zu Leon ist falsch in ROOM 3060."** Marvin dreht sich zu den Hunden, hebt die Arme mit der Waffe vor die
Brust und haelt die Haltung bis zur Tuer (vorher kniete er sich hin); danach steht Leon in 3020 wie im Original. Die
Bildfolge deckt sich mit dem Original-Video.

**ROOM3061 (Elza) - "In Elzas Szenario, in ROOM 3061 in der cutscene klappt das rein krabbeln von Sherry in den Schacht
nicht. Am Ende von dieser Cutscene hat Elza auch erneut eine falsche Animation."** Nach der Aktionstaste an der Wand beim
Gitter zieht Sherry das Gitter mit beiden Haenden heraus, legt es links ab, geht auf alle Viere und kriecht in die
Oeffnung, bis sie verschwunden ist (vorher lag sie vor der Wand). Am Ende dreht sich Elza zum Hund und hebt die Arme mit
der Waffe (vorher kniete sie), John ebenso; dann geht es als Sherry in 30C1 weiter.

**Ab 30C1: Sherry als Spielfigur - "Ab 30C1 wird es richtig wild... Sherrys Charakter Modell ist voellig falsch und
kaputt. Ausserdem sollte sie keine Waffe haben."** Wie im Original spielen Sie nach der Kriech-Szene Sherry - jetzt mit
ihrem echten Kind-Modell, eigenem Skelett und eigenen Bewegungen, ohne Waffe (eine vorher ausgeruestete Schusswaffe wird
beim Wechsel abgelegt, Ausruesten ist als Sherry gesperrt), ohne Zielen und im Tempo des Originals (Gehen 50, Rennen 120
je Bild; nach dem Loslassen rutscht sie wie im Original kurz aus).

**ROOM3091 - "Die Cutscene im ROOM 3091 hat STAENDIG falsche Animationen...."** In der Szene ist Sherry die Spielfigur,
Elza steht als NPC mit ihrer Pistole daneben. Alle drei Figuren spielen dieselben Gesten in derselben Reihenfolge wie das
Original (Bild fuer Bild gegen eine Mitschrift des Originals verglichen), und Sherrys Haltung zwischen den Gesten bewegt
sich durchgehend, statt nach 40 Bildern einzufrieren.

**Figurwechsel wie im Original.** Beim Wechsel bekommt die neue Figur volle Energie (100) und ist entgiftet; nach der
3091-Szene hat Elza das Messer in der Hand, die Pistole bleibt im Inventar. Dasselbe beim Wechsel Leon -> Marvin (3020)
und zurueck (3060). Marvin hat als Spielfigur jetzt seinen eigenen Koerper (laengere Arme) statt Leons Skelett. Weste und
U.S.S.-Ruestung heilen weiter nicht (Ihr Entscheid vom 30.08.).

**Szenen-Animationen in weiteren Raeumen.** Welche Animationsdaten eine Szene einer Figur zuordnet, bestimmt der Port
jetzt wie das Original; das betrifft zwoelf Raeume (2000, 3060, 3091, 30E0, 4000, 4001, 4010, 5011, 6030, 6031, 1040,
1041). Sichtbar z. B. in ROOM4010: Marvin liegt verwundet auf der Liege, statt im Vordergrund zu stehen.

### Spinnen

**Schachtspinne in 2060/2061 und 20A0/20A1 - Ihr Punkt "Die Spinnen Animation beim herausklettern vom Gitter und runter
klettern an der Wand stimmen noch nicht ganz."** Am Ende des Herauskriechens kippt die Spinne kopfueber ueber die
Gitterkante und klettert kopfueber, mit dem Bauch an der Wand, hinunter (vorher lag sie waagrecht in der Luft und schwebte
vor der Wand herab); unten dreht sie sich wieder auf den Boden. Sie springt nicht mehr kurz in die Ausgangspose zurueck,
und jedes Bild erscheint im selben Takt wie im Original (gemessen mit Leon in beiden KI-Einstellungen und mit Elza). Zum
Ansehen: ROOM2060 ueber die obere Tuer aus 2030 betreten und auf die Kamera mit dem Gitter (vordere Raumhaelfte) zugehen.

**Alle grossen Spinnen sind 20 % kleiner** - so gross wie im Original (Massstab 0.8), in beiden KI-Einstellungen. Ihr
Schatten ist groesser (1300 x 1400 wie im Original) und liegt beim Herunterklettern auf dem Boden unter ihr; der
Giftklumpen startet genau dort, wo er im Original startet.

**Giftklumpen bei Danger - Ihr Punkt "Der Spieler kann nicht sterben, wenn er noch mal mit Gift von den Spinnen
angeschossen wird, aber bereits auf Danger ist...".** Mit der RE2-KI (Voreinstellung) nimmt ein Giftklumpen wie in RE2
20 HP: bei Danger (unter 20 HP) ist jeder Treffer toedlich (Todesanimation, Neuladen); mit genau 20 HP bleibt die Figur
mit 0 am Leben. Mit "AI RE1.5" bleibt der Wert des Originals (10 HP, Tod erst unter 0).

### NPCs und Kisten

**NPCs sind fest - Ihr Punkt "Die NPCs z.B. Ada und Marvin koennen ineinander rein laufen, das ist falsch, die sind beide
fest."** Zwei NPCs laufen nicht mehr ineinander, auch nicht in Gegner; laeuft die Figur in eine NPC hinein, weicht diese
aus - wie im Original (dort gemessen: Ada und Marvin in ROOM3070 nie naeher als 901). Zum Ansehen: ROOM3070 (Ada und
Marvin folgen Leon), ROOM30E0 nach der Szene, ROOM3001/3011 (John und Sherry folgen Elza). Wie im Original kommt eine
folgende NPC nicht mehr durch die Figur hindurch, sondern bleibt vor ihr stehen.

**Kisten in ROOM2050/2051 - "In ROOM 2050 lassen sich die Kisten in einander schieben, das ist falsch".** Die Kisten
sperren sich jetzt gegenseitig, mit derselben Pruefung wie im Original. Das Raetsel bleibt loesbar, braucht aber
Genauigkeit: die drei Kerben an der Westwand liegen 1900 bzw. 1850 auseinander, die Kisten sind 1800 tief. Erprobter
Weg: (1) die mittlere Kiste gerade nach Westen in die suedliche Kerbe; (2) die suedoestliche Kiste nach Norden bis knapp
hinter die Linie der Kerben-Mitte, dann nach Westen in die mittlere Kerbe; (3) die noerdliche Kiste nach Sueden nur bis
zur Linie der noerdlichen Kerbe (schiebt man sie zu weit, stoesst sie beim Westschub an die mittlere Kiste), dann nach
Westen. Steckt eine Kiste fest, beruehrt sie eine andere.

### Bewegung und Kamera (im ganzen Spiel)

- **Kamerawechsel:** Bei jedem Kamerawechsel steht das alte Bild kurz (gut 1/4 s), wie im Original, das in dieser Zeit den
  Hintergrund von CD laedt; die Spiellogik laeuft dabei nicht weiter. Das erste Bild eines Raums ist ausgenommen.
- **Rennen:** Das Renn-Tempo folgt dem Schritt (im Mittel 196 statt 200 je Bild). Laesst man Rennen oder die Richtung
  los, rutscht die Figur 6 Bilder aus (zusammen etwa 360-450 Einheiten), statt sofort zu stehen; dreht man direkt aus dem
  Rennen, gilt in den ersten zwei Bildern die Renn-Drehrate; drueckt man waehrend des Ausrutschens wieder Oben + Rennen,
  geht sie ein Bild, bevor sie wieder rennt. Nach einer Treppe, einem Griff oder einer Szene rutscht sie nicht nach.
- **Texte:** Nach dem Schliessen eines Textes laeuft die Figur ein Bild spaeter wieder los, wie im Original.

### Kleinigkeiten

- **Lautstaerke - "Mir fehlt in den Optionen eine Lautstaerke Option bei der ich alles lauter oder leiser machen
  kann."** OPTIONS -> SOUND: Links/Rechts stellt die Lautstaerke 0-100 in Zehnerschritten (sofort hoerbar, auch der
  Menueton), Kreis/Viereck speichert zusammen mit Stereo/Mono, Kreuz verwirft. Die Zeile "VOLUME <Zahl>" steht im freien
  Streifen unter dem Options-Kasten, damit keine Original-Beschriftung verdeckt wird. Gespeichert wird in
  `re15_options.cfg` neben `re15_card.mcr` (Spielordner); Loeschen der Datei = 100 %, Stereo. Damit bleibt jetzt auch
  Stereo/Mono nach einem Neustart erhalten.
- **Feuerloescher ROOM1030 - "Kannst du den Feuerloescher in room 1030 einen leichten Blink Effekt geben, das man sieht,
  das der aufnehmbar ist? Ich glaube so etwas existiert bereits in Resident Evil 2, uebernehme den."** Der Loescher
  pulsiert jetzt in der Helligkeit (Periode 2 s, bis +48 %). RE2 hat kein Blinken aufnehmbarer Gegenstaende (im RE2-Code
  nachgelesen); die Form ist deshalb der Helligkeitspuls der Titelzeile dieses Spiels. Nur in 1030 bei Leon (Elzas 1031
  hat keinen Loescher; der liegende in 1090/1091 ist nur zum Untersuchen).
- **Karte B1 - "... bevor man in den vorderen Teil der Waffen Armory - room 1090 - geht - also die vordere Tuer - macht
  der Spieler auf der Map einen Sprung zum hinteren Teil, obwohl man garnicht da ist."** Gemeint ist ROOM1190: vor der
  vorderen Tuer aus dem Gang 1230 in die Ostkammer wanderte der Marker um 9 Bildpunkte zur Halle. Jetzt bleibt er auf der
  Ostkammer - auch mit Elza und im Rennen.
- **Werkstatt-Tuer ROOM11B1 (Elza) - "Ich wollte das sie dauerhaft geschlossen bleibt in Elzas Szenario."** Nach der
  Sherry-Suche ist die Werkstatt-Tuer dauerhaft verschlossen ("The door is locked."), auch beim Wiederbetreten; zurueck
  geht es durch den Bodenschacht (Kanal 1261 -> Zwinger 11D1 / Kanaldeckel 10B1 / Kanalisation 2001). Leons 11B0 ist
  unveraendert.
- **Granatwerfer (im ganzen Spiel):** Muendungsblitz und die rot gluehenden Granatkoerner aus RE2 sind jetzt in jedem
  Raum zu sehen - RE2 haelt diese Effekt-Farben ab dem Start geladen. Bisher liess der Port sie beim Zeichnen weg, weil
  er nur einen Teil der RE2-Effekt-Farben geladen hatte. Gegen ein Bild aus RE2s eigenen Daten verglichen: dieselben
  Formen und Farben.

### Masken

**STAGE3-6: Loecher geschlossen (Ihr Auftrag "... und das Schliessen der STAGE3-6-Masken, wie mit Ihnen vereinbart";
vereinbart war "danach kannst du deine Masken schliessen in der naechsten Version.").** Die Loecher in den alten
Uebersichtsbildern unter pri/ kamen von einem Fehler des Werkzeugs, das diese Bilder erzeugt (behoben). Im Spiel fehlten
unter anderem Gelaender, Pfosten, Saeulen und Lampen: Sie liegen im Vordergrund-Atlas des Originals, der
Masken-Generator hatte sie nur nie gefunden. 267 dieser Original-Teile sind jetzt in 61 Kameras an ihrer Original-Stelle
eingesetzt, jedes mit belegter Lage (Bildvergleich mit dem Hintergrund) und Tiefe (Kollisionszelle des Raums), u. a. die
Steg-Gelaender in ROOM30E0 Kamera 6-9, das Treppengelaender in ROOM3050 Kamera 6 und das Laufsteg-Gelaender in ROOM3040
Kamera 3. Neu verdeckt auch das Unterteil der Lok in ROOM6000 Kamera 7 (ein Lesefehler des Masken-Werkzeugs bei den
Tueren ist behoben). Weitere Gegenstaende verdecken jetzt mit der Tiefe ihrer Kollisionszelle: die Lok in ROOM6000
(Kameras 5, 7, 8, 9) samt oberem Gittertraeger und dem Gittertraeger vorn links in Kamera 5, die Buero-Trennwand in
ROOM30A0 Kamera 0/1 (sie verdeckt jetzt die ganze Figur; vorher sah man Leon bzw. Elza ab der Huefte durch die oberen
Mattglasfelder), die hohen Spinde in ROOM30A0 Kamera 3, Kisten und Container in ROOM5020 Kamera 6, 7 und 12 und in
ROOM5090 Kamera 11 und 13, Schaltkasten und Kisten in ROOM3040 Kamera 0 und 9, die Maschine links in ROOM3050 Kamera 5.
An Stichproben im Spiel geprueft, mit Leon und Elza: Hinter den Gegenstaenden ist die Figur verdeckt (bis auf ein
Restloch an der Treppe in ROOM6010/6011 Kamera 5, siehe "Ab Samstag"), davor uebermalen die neuen Teile sie nicht; keine
bisherige Maske ist weggefallen. Was ohne Original-Teil oder ohne belegte Tiefe bleibt, zeichnen Sie selbst (Nachtrag
27: "Ich kuemmere mich selbst drum.") - die Liste steht unter "Ab Samstag".

**STAGE2: Ihre 53 Freistellungen am Spiel geprueft (Ihr Punkt "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die
sollen alle sauber eingebaut werden!").** Sie sind als 53 Kameramasken drin (ROOM11A0, 2010, 2020, 2030, 2040, 2050,
2080, 20A0; Elzas Raeume 11A1, 2011 usw. zeichnen dieselben Masken). Die Pruefung am Spiel (Leon, Stichproben mit Elza)
fand zwei Fehler, beide behoben: In ROOM2030 Kamera 7 und ROOM2040 Kamera 7 verdeckte der Handlauf des Gelaenders die
Figur dahinter nicht (er hatte eine viel zu ferne Tiefe, jetzt die der Gelaenderzelle; in 2030 ist Ihre Freistellung
07_01 dafuer an der Gelaenderecke in zwei Teile geteilt, Ihre PNG bleibt unveraendert). Ihre Freistellung 12_02 in
ROOM20A0 Kamera 12 ist der Bodenstreifen vor der Schwelle unter dem Rolladen; wer darauf stand, verlor die Fuesse. Sie
ist wie entschieden nicht mehr eingebaut (Nachtrag 11: "Weglassen ist ok"), 12_01 bleibt. An der Kurbel in 2020
(Kamera 3) liegt Ihre Freistellung 03_01 zu Recht ueber Leons Beinen: Leon steht dort hinter der Reling (die Kamera
haengt ueber dem Becken); das Original hat in dieser Kamera gar keine Maske und zeichnet ihn deshalb obenauf. In ROOM2030
Kamera 15 uebermalt die Reling die Figur nicht - was dort zuerst gezaehlt wurde, war die Spinne hinter Leon.

**Uebersichtsbilder zum Zeichnen.** Die Bilder `pri/STAGEn/<raum>/<cc>_port.png` der Stages 3-6 sind ersetzt (Ihr
"Ja, ersetzen" vom 24.10. und Nachtrag 33 vom 25.10.): am 24.10. 204 von 214; am 25.10. kamen 113 Bilder auf den
Maskenstand dieser Version - darunter die zehn, die am 24.10. noch fehlten -, eines kam neu dazu (ROOM4040 Kamera 5),
22 hatten diesen Stand schon. Damit zeigen alle Bilder in Leons Ordnern (z. B. pri/STAGE3/3040) den Stand dieser
Version, und auch die 57 betroffenen Bilder in Elzas Ordnern (z. B. pri/STAGE3/3041) sind nachgezogen - Elzas Raeume
zeichnen dieselben Masken wie Leons. Ihre Freistellungen in pri/ sind unberuehrt.

## Zu entscheiden

So ist es jetzt gebaut; ein Wort von Ihnen genuegt, um etwas zu aendern.

Entscheide des Orchestrators waehrend der Runde:
* **A-E4** - NPC-Schritte werden wie im Original nach Abstand und Seite zur Kamera leiser bzw. seitlich gespielt (nur
  NPCs; Ihre eigenen Schritte siehe "Noch offen").
* **B1** - Alligator-Tempo = RE2 ohne Deckel (Schwimmen 24,5 je Bild in der Biss-Sperre, Paddeln etwa 149 je Bild,
  Zonen-Dashes); bitte im Becken testen.
* **B2** - Schuesse allein toeten den Alligator nicht (RE2-Sockel 25300), er stirbt nur ueber den Kanister; grosse
  Treffer-Reaktion mit 25 % je Treffer wie RE2.
* **B3** - Putz faellt nur an den RE2-Stellen (Dash-Start), nicht bei jedem Wackeln.
* **B6** - Beim Tod durch den Alligator bleibt die Finisher-Kamera bis zum Game Over; die RE1.5-Todeskamera (Weissblende,
  kreisende Kamera) entfaellt dort.
* **B7** - Der Durchbruch beginnt dort, wo die RE2-Wende ins Becken passt und auf Leon zeigt (Lage gewaehlt, Bewegung =
  RE2); der Kopf ragt nicht in die Wand. Das gilt jetzt fuer das RE2-Intro beim erneuten Betreten von 2090; den ersten
  Auftritt zeigt die Ankunftsszene.
* **B8** - Explosion und Finisher bleiben in den vorhandenen Kameras von 2090 (teils seitlich); neue Hintergruende werden
  nicht erfunden.
* **B9** - Vor seinem Auftritt ist der Alligator unsichtbar und nicht zu treffen (RE2: unter Wasser).
* **C-a** - Pult 2050: Hebelgewichte 30/20/40/10, Loesung 80 V = Hebel 1 + 3 + der kleine.
* **C-b** - Die Bruecke in 2030 steht "oben" auf Hoehe der Ringebene.
* **C-c** - Verlaesst man 2030 waehrend des Countdowns durch eine andere Tuer, gilt die Flut ebenfalls als gelaufen (die
  20B0-Szene laeuft beim naechsten Betreten von 2000).
* **C-d** - Solange 2030 geflutet ist, melden die oberen Lift-Rufzonen den Original-Satz "The elevator is not
  responding...Maybe it should be activated from somewhere.".
* **C-e** - Ertrinken = Tod mit blauem Schleier und Spritzer, ohne eigenen Ton (RE2 hat keinen).
* **C-E2** (angepasst an Ihren Nachtrag 7) - Solange 2030 geflutet ist (Flut gelaufen, Strom aus), sind die Ring-Tueren
  aus 20B0 und 2070 mit "I cannot open the door because of the water pressure." gesperrt.
* **C-E4** - Der RE2-Alarm laeuft vom Warntext bis zur Flut bzw. bis zum Verlassen des Raums, zusaetzlich zur Raummusik.
* **D1** - Pulsieren nur bei Port-Gegenstaenden (Feuerloescher 1030 und jetzt der Water Key in 2090); die
  Original-Gegenstaende der Raeume pulsieren nicht.
* **D2** - Der Giftklumpen hat keine RE2-Rettung "einmal je Raum": Danger + Treffer = Tod.
* **D3** - Die VOLUME-Zeile steht im freien Streifen unter dem Options-Kasten.
* **E-E1** - Kamerawechsel und Texte laufen im ganzen Spiel im Takt des Originals: beim Kamerawechsel bleibt das alte
  Bild gut 1/4 s stehen, nach einem Text laeuft die Figur ein Bild spaeter los.
* **E-E2** - Der Renn-Stopp des Originals (Ausrutschen nach dem Rennen) gilt im ganzen Spiel.
* **Texte** (Entscheid vom 23.10.: Port-Texte gelten bis zu Ihrer Freigabe als Vorschlag). Noch nicht freigegeben:
  2030 Id 40 "A control panel for the bridge. / The pressure gauge reads zero. | Intake valve 13 is closed.", Id 41
  "Intake valve 13. / Will you open it?", Id 42 "You've opened valve 13. / The pressure is rising.", Id 43 "The pressure
  gauge reads 80. / Lower the bridge?", Id 44 "The bridge is down.", Id 47 "Intake valve 13 is open."; 2050 Id 40 "The
  pump has no power. / The switchboard must be set first.", Id 43 "A high voltage switchboard. / The pump needs 80
  volts.", Id 44 "The switchboard is set to 80 volts. / The pump is powered."; 20B0 Id 41 = 2070 Id 40 "I cannot open
  the door / because of the water pressure."; 2081 Id 42 "John: Hey, Elza, try with this!" (Elzas Fassung Ihres
  Satzes). Von Ihnen oder freigegeben: 2000/20B0 Id 40, 2030 Id 45, 46, 48 (vorerst), 50-55, 2010 Id 40, 2080 Id 42
  "Marvin: Hey, Leon, try with this!", Plakat, Loesung und "Technician's Note". Original-Saetze aus ROOM20A0 sind
  2090/2091 Id 42 "It's locked." und Id 43 "You've opened the lock.".
* **2030 Id 49** (Ventile ohne Sicherung im Kasten) ist jetzt der Original-Satz "It's locked." aus ROOM20A0; der
  Ventil-Satz, den Sie nach Nachtrag 12 spaeter aendern wollten, ist damit ersetzt.
* **F-E2** - Kampfbeginn wie in RE2 am Ende des Alligator-Intros, nicht weiter weg: Abstand 11430 bis 13924, erster
  Angriff 121 bis 158 Bilder nach dem Kampfbeginn (gemessen 11450 und 131). Die Leiche liegt dafuer im vorderen Becken;
  der Schluessel ist dort mindestens so hell wie in der zuerst abgenommenen Fassung (die etwas kleinere Groesse haben Sie
  in Nachtrag 31 angenommen), das Fressen ist von vorn zu sehen.
* **FG-E1** - Die acht Zombies der 2080-Einstellung in der Ankunftsszene sind eigene Szenen-Figuren; danach stehen acht
  Zombies an denselben Stellen oben in 2080 - die, die Marvin bzw. John erschiesst.
* **FG-E3** - Nach der Kanister-Szene kaempft derselbe Alligator weiter (Lebenspunkte, Lage und Zustand bleiben, kein
  neuer Durchbruch); der Kanister landet so, dass er ihn im normalen Rundlauf aufnimmt.
* **G-E1/G-E3** - Todeskamera am Kanister: Zuerst muessen Kopf, Kiefer und die ganze Todesanimation im Bild und
  unverdeckt sein, erst danach zaehlen die Obergrenze der Bildgroesse (der groesste RE2-Schnitt) und die Naehe zu RE2s
  Bildausschnitt. Eine Mindestgroesse gibt es nicht; in manchen Lagen ist der Alligator klein im Bild (siehe "Ab
  Samstag").
* **G-E2** - Marvin bzw. John steht bei der Zeile und beim Wurf mit dem Gesicht zur Kamera wie Ada in RE2; der Kanister
  fliegt unabhaengig davon zur Landestelle.
* **G-E4** - Die RE2-Effekt-Farben sind wie in RE2 ab dem Start geladen; RE2-Effekte sehen damit in allen Raeumen gleich
  aus, mit und ohne Besuch im Alligator-Becken (sichtbar am Granatwerfer).
* **G-E5** - Mit dem Kanister im Maul und beim Kanister-Tod bleiben Kopf und Kanister innerhalb der Beckenwand - eine
  Klemme, die RE2 nicht hat (Wahl des Ports); Nebenwirkung siehe "Ab Samstag".
* **FG-E7** - Die vier Tests der Schluessel-Einstellung nach dem Tod pruefen jetzt eine Todeslage, an der die Kopf-Klemme
  nicht greift; die Lage an der Suedwand, an der der Schluessel danach sehr klein wird, steht unter "Ab Samstag".
* **M-E2** - Original-Teile des Vordergrund-Atlas gehoeren an ihre Original-Stelle: In den Stages 3-6 sind sie
  eingesetzt, wo Lage und Tiefe belegt sind, auch fuer Gegenstaende Ihrer Liste aus Nachtrag 27; wo es kein
  Original-Teil gibt oder seine Tiefe nicht belegt ist, baut der Port keine Maske - das zeichnen Sie (Liste unter "Ab
  Samstag").
* (Nicht aufgefuehrt sind FG-E2 - widerrufen -, FG-E4 und FG-E5 - von Ihnen in Nachtrag 26 entschieden -, FG-E6 - von
  Ihrem Nachtrag 32b berichtigt, die Regel "gut genug" steht in der Einleitung -, M-E1 - Ihre Antwort in Nachtrag 27,
  fuer Original-Teile durch M-E2 ersetzt - sowie F-E1 und G-E6, die nur Tests bzw. Code betreffen.)

Fragen aus der Runde, die noch niemand entschieden hat (so ist es jetzt):
* Alligator: Ein Granaten-Treffer am Lauerplatz weckt den untergetauchten Alligator; RE2 kennt diesen Fall nicht.
* Alligator: Steht Leon unerreichbar hinter einer Wand (Tuernische, Rampe oben), bleibt der Alligator an der Wand und
  schaut zu ihm, statt sich abzuwenden und neu anzuschwimmen.
* Alligator: Der Finisher spielt immer am selben Ort (Westkanal mit Kamera 6, sonst Suedostbecken mit Kamera 1); Alligator
  und Figur werden dafuer hinter dem Kameraschnitt umgesetzt. Naeher am Ort des Bisses gewuenscht?
* Alligator: Nach seinem Tod schneidet das Bild 1,5 s auf den blinkenden Schluessel; diese Form haben Sie noch nicht
  bestaetigt.
* Alligator: Beim Kauen nach dem Schuetteln haengt die Leiche sichtbar neben dem offenen Maul statt zwischen den Kiefern
  (sie haelt fest am Kopf in der Lage des Zubeissens - so haelt RE2 den Kanister).
* Alligator: Nach seinem Tod spielt wieder die Raummusik von 2090; RE2 blendet in seinem Alligator-Raum die Musik aus und
  bleibt still. Nicht nachgebaut, weil Sie nur den Kampf mit RE2-Musik verlangt hatten.
* 20A0: Die Schachtspinnen in 20A0/20A1 bleiben; entfernt sind nur die Spinnen in 2090/2091.
* 2030: Die Ost-Tuer fuehrt nach der Flut dauerhaft nach 20B0 (vorher in den Mittelgang von 2070); soll sie nach dem
  Strom wieder nach 2070 fuehren?
* 2030: Ertrinkt die Figur bei den Ventilen, bleibt die Kamera auf der Flut-Draufsicht; ihren Spritzer sieht man dort
  nicht.
* 2030: Der Alarm laeuft mit dem festen RE2-Wert beim Betreten von ROOM6040 (99); RE2 staffelt die Lautstaerke je Kamera
  seiner Raeume.
* Sherry: Der Statusschirm zeigt bei "Equip Arms" das Messer, obwohl sie keine Waffe fuehrt (im Original genauso).

## Fuer Sie zum Aufnehmen

Ohne Datei laufen die Zeilen stumm mit Untertitel; der Port bindet jede Datei nach Raum und Nachricht.
* **Drin:** `synchro/STAGE2/room2030/main46.wav` - Ihr Voiceover zum Warntext "WARNING: The water will rise in one
  minute. Please leave the room immediately!" (Aufnahme vom 24.10., unveraendert); fuer Elza dieselbe Datei als
  `synchro/STAGE2/room2031/main46.wav`. Ihre zweite Fassung liegt unbenutzt unter
  `synchro/unused/STAGE2/room2030/main46_02.wav`.
* **Optional** - die Texte des neuen Ventil-Sicherungskastens (Spielfigur): `synchro/STAGE2/room2010/main40.wav` bis
  `main44.wav` (Leon) bzw. `synchro/STAGE2/room2011/main40.wav` bis `main44.wav` (Elza):
  - main40: "A fuse box for the valve controls." / "One of the fuses is missing..."
  - main41: "Will you use the Fuse?"
  - main42: "You've used the Fuse."
  - main43: "The fuse is in place." / "Will you take the Fuse?"
  - main44: "You've taken the Fuse."
* **Neu - die Kanister-Szene:** `synchro/STAGE2/room2080/main42.wav` - Marvin: "Hey, Leon, try with this!" (Leon) und
  `synchro/STAGE2/room2081/main42.wav` - John: "Hey, Elza, try with this!" (Elza). Die Ordner `room2080` und `room2081`
  gibt es unter `synchro/STAGE2/` noch nicht. Liegt eine Datei dort, wartet Marvin bzw. John mit dem Wurf, bis sie zu
  Ende ist (wie RE2 auf Adas Stimme).
* Die uebrigen Punkte dieser Runde (auch die Ankunftsszene in 2090 und die Masken) brauchen keine Aufnahmen.

## Noch offen

Ton
* Ihre eigenen Schritte (Leon/Elza) klingen weiter mittig und unabhaengig vom Abstand; das Original rechnet sie mit
  derselben Routine wie die NPC-Schritte nach Lage. Die Umstellung aendert die Schritt-Lautstaerke in jedem Raum und ist
  ein eigener Auftrag.
* Mono: Der Port mischt Links und Rechts zur Haelfte, das Original nimmt fuer beide Kanaele das Lautere; stark seitliche
  NPC-Schritte klingen in Mono deshalb leiser als im Original.
* Die Lautstaerkekurve aller Toene ist im Port linear, im Original quadratisch (spielweit, spaetere Runde).
* Hubschrauber-Rotor (Raumton): Der Port bestimmt die Richtung mit einer Winkeltabelle statt wie das Original gerechnet
  (bis eine Pan-Stufe Abweichung); links/rechts sind gegenueber dem Original moeglicherweise vertauscht (nicht geprueft).

Bild und Takt
* Beim Kamerawechsel haelt der Port das Bild einheitlich gut 1/4 s; im Original haengt die Dauer am CD-Laufwerk (gemessen
  0,2-0,25 s, nach einem Waffenwechsel bis etwa 0,6 s).
* Abweichungen von je 1-3 Bildern: Im ersten Bild nach einem Kamerawechsel zeichnet der Port Effekt-Sprites, die das
  Original dort weglaesst; in 20B0 schaltet die Kamera 12 -> 13 2-3 Bilder frueher; Gegner werden beim Betreten schon
  gezeichnet, bevor ihr erster Takt gelaufen ist - in 2060/2061 haengen dadurch im letzten Einblendbild fuer ein Bild die
  grossen Spinnenbeine aus dem Schacht.
* Schneidet die Kamera waehrend der Kiefer-Explosion des Alligators, steht das gehaltene Bild etwas zu lange (16 statt der
  erwarteten etwa 14 Sechzigstelsekunden); vor einer Aenderung wird RE2 an dieser Stelle nachgemessen.

Bewegung
* Geh-Tempo: Der Port geht fest mit 75 je Bild; das Original rechnet es wie das Renn-Tempo aus dem Schritt.
* Elza und Marvin (als Spielfigur) nehmen noch Leons Tabellenwerte (Gehen 75, Rueckwaerts 70, Rennen 200); das Original
  hat fuer beide 70/65/210. Beim Gehen und Rennen ersetzt das Original den Tabellenwert durch die Schrittrechnung (der
  Port beim Rennen jetzt auch).
* Beim Wechsel Stehen -> Gehen/Rennen und Gehen -> Rennen nimmt der Port fuer ein Bild eine andere Drehrate als das
  Original.
* Bei Texten, die das Spiel ohne Spieler-Pause zeigt, bleibt die Figur im Port stehen; im Original laeuft sie weiter
  (schon vor dieser Runde so).
* Gorillas in ROOM11C0 koennen an drei Stellen haengen bleiben (gefunden mit einer Zufallsgang-Sonde; an allen drei
  haengt auch die Regel des Originals) - Folgeauftrag.

Alligator
* Der Kanister ist im dunklen Becken kaum zu sehen (graue RE2-Textur im Licht von 2090); eine Aufhellung ist nicht
  eingebaut.
* Bei der Belagerung an der Rampe und beim Klettern ueber die Plattform spielt der Alligator weiter die Animation Ihres
  Designs (Clip 0), keine aus RE2.
* Beim Hochbiss von der Rampe (Ihr Belagerungs-Design prueft nur den Abstand) schnappt der Alligator im Bild der laufenden
  Kamera seitlich an Leon vorbei; der Finisher danach zeigt den Biss wie RE2.
* Die offenen Punkte der Ankunftsszene und der Kanister-Szene (u. a. der Kampfbeginn vor der Kamera und Lagespruenge mit
  dem Kanister im Maul) stehen unter "Ab Samstag".

Zombies
* Nach dem Loesen aus dem RE2-Griff gleiten die Kanal-Zombies in 2000/2001 etwa 2500 Einheiten weg - eigener Auftrag,
  falls gewuenscht.

STAGE3
* Am Ende der 3061-Szene laeuft der Hund im Port direkt zu Elza; im Original geht er langsam nach rechts vorn (eigener
  Auftrag empfohlen).
* Ein Spielstand aus dem Sherry-Abschnitt (30C1, 3071, 3081, 3091) laedt als Elza.
* Weste (1190/1191) und U.S.S.-Ruestung (40A0/40A1): Das Original setzt beim Anlegen auch Waffe, Wunden und Status
  zurueck; der Port behaelt alles (gedeckt ist nur "heilt nicht", Ihr Entscheid vom 30.08.).
* Nach dem Rueckwechsel auf Elza kann das Inventar die vorher ausgeruestete Waffe noch als ausgeruestet zeigen, obwohl
  das Messer in der Hand ist; dieselbe Waffe wieder auszuruesten geht erst ueber eine andere Waffe (im Original genauso;
  aus dem Code abgeleitet, nicht am Spiel gemessen).
* Lichtton in 3091: Original gelbgruen, Port neutral.

Spinnen und Gift
* Die Spinne wirkt im Port heller als im Original-Video (Beleuchtung, nicht Animation).
* Die Trefferweite des Giftklumpens ist die des RE1.5-Geschosses (500), nicht die von RE2 (800).
* Der Biss der Spinne (RE2-KI) zieht 20 HP ueber die RE2-Regel mit der Rettung "einmal je Raum" und loest an der Figur
  keine Treffer- bzw. Todesfolge aus - schon vor dieser Runde so (wie beim Hundebiss).

NPCs
* An einer Wand koennen zwei NPCs noch leicht ineinander stehen: die Wandklemme laeuft wie im Original nach dem Schub (in
  einem gestellten Aufbau in 3011 Abstand 664-670 statt 750; auf dem echten Weg nicht).
* Eine folgende NPC kann Elza vor sich herschieben (3001, zweiter Besuch: etwa 570 Einheiten in etwa 100 Bildern); gegen
  das Original nicht gemessen.

Masken
* Was an den Masken noch fehlt (Ihre Freistellungen fuer STAGE3-6, ein Restloch in ROOM6010/6011 Kamera 5), steht unter
  "Ab Samstag".
* An wenigen Stellen VOR einem Gegenstand uebermalt eine Maske einzelne Randpunkte der Figur (Hand oder Arm am Rand,
  z. B. ROOM5090 Kamera 12); das kommt von Masken, die schon vor den neu eingesetzten Original-Teilen da waren.

## Ab Samstag (neue Tokens)

Ihr Plan vom 25.10.: "Nein, Ziel ist langsam mit allen fertig zu werden, den build zu bauen, und mit neuen Tokens am
Samstag die Sachen nachtraeglich zu verbessern....". Vorgemerkt sind diese Punkte; keine der hier genannten Aenderungen
ist in v0.8.29 enthalten.

**Ihr Nachtrag 30 (Ventilkette, Toene, Lautstaerke) - am 25.10. angehalten, nicht in v0.8.29.** Im Spiel gilt fuer diese
Punkte deshalb noch der vorherige Stand: der Schaltschrank mit Tastenfeld in 2010, Pult und Ventil 13 fuer die Bruecke
in 2030, die Notiz mit Ihrem Text aus Nachtrag 13, das Plakat "1. GREEN 09 - 2. RED 01 - 3. BLACK 02 - 4. RED 07" und
die Loesung 09 -> 07 -> 02 -> 01. Verlangt haben Sie:
* Schrank 2010: "In ROOM 2010, beim Anklicken des Schranks, mache das er aufklappt und dann so aussieht wie in 2060 der
  Fuse Kasten - siehe fuse_box.bmp. Ab da soll auch der Mechanismus gleich - das Einsetzen der Sicherung etc."
* Panel-Raetsel in 2030 statt Ventil 13: "in ROOM 2030 am Kasten, damit die Platform runter faehrt, erstelle ein Raetsel,
  das so aussieht wie das Panel in Nahansicht, anstatt das komische - dreh an Valve 13." Zuerst kommen zwei bis drei
  Entwuerfe mit Bildern zu Ihrer Freigabe (Ihre Wahl: "C entwirft, ich gebe frei"), jeder mit dem Ziel Druck 80 ("Zum
  2030er Raetsel ist noch wichtig das die Pressure irgendwie 80 betragen muss, wenn du das schon so hin schreibst.");
  gebaut wird erst nach Ihrer Freigabe.
* Notiz: "beim Text soll nur drin stehen das gruen und rot verwechselt wurde, nicht black, das soll raus. Bei der
  verwechselten Nummer soll drin stehen das sie ausserdem die Nummern verwechselt haben von "Schwarz" und "Gruen". Hier
  dienen die Anfuehrungszeichen als Ironie fuer die falschen Farben auf den Poster." Vorschlag fuer den Satz: "They mixed
  up green and red, and on top of that they swapped the numbers of "black" and "green"!"
* Plakat neu nach Ihrer Wahl (Nachtrag 30d): "1. GREEN 01 - 2. RED 02 - 3. BLACK 09 - 4. RED 08", Loesung
  09 -> 02 -> 01 -> 08. Im angehaltenen Stand (Zweig r41/nachtrag30, bb7a0df6) sind das neue Plakat und diese Loesung
  schon eingetragen, aber ungeprueft; die uebrigen Punkte dieser Liste sind dort noch nicht umgesetzt.
* Plattform 2030: "Und wenn die Plattform runter faehrt, fehlt mir noch der Original Sound von Resident Evil 2."
* Alarm 2030: "Und waehrend des Alerts und der Alert message fehlt mir noch zusaetzlich durchweg der Alert sound aus
  Resident Evil 2." In v0.8.29 laeuft waehrend des Countdowns RE2s Alarm SUB_25 neben der Raummusik.
* Lautstaerke: "Und bei der reduzierten Volume sollen ALLE Volumes reduziert werden, nicht nur ein paar." und "Die
  Tuersequenzen und Sounds der Reinsprung SOunds ins wasser etc, scheint davon nicht betroffen." Eine erste, noch
  ungepruefte Messung fand im Mischer keine Quelle, die an der Einstellung vorbeigeht (Tuersequenz und Sprung ins Wasser
  waren bei 50 % genau halb so laut); woher Ihr Eindruck kommt, ist noch nicht gefunden.
* Wasser in 2010 (Nachtrag 30e): "in room 2010 soll - bis zum ablassen der Wassers, auch ein Wasser Effekt die Leiter
  runter kommen und ueber den Weg bis zum Wasser Pool in der Mitte laufen." Nach Ihrer Wahl bis zur Flut in 2030, wie der
  Strom an der Leiter in 2000.

**Alligator.**
* Kampfbeginn nicht vor der Kamera (Ihre Antwort in Nachtrag 31: "Aendern: nicht vor der Kamera"). In v0.8.29 beginnt
  der Alligator den Kampf an der Suedwand gut 1800 Einheiten vor der Leiter-Kamera (Kamera 0): rund 120 Bilder (4 s) lang
  fuellen Ruecken und Schwanz gross die rechte Bildhaelfte, erst dann schwimmt er im Becken vor Leon bzw. Elza (gemessen
  mit beiden). Ziel: eine Startlage, in der er beim Kampfbeginn im Becken vor der Figur zu sehen ist, mit Abstand und
  erstem Angriff weiter wie in RE2; dazu ein Riegel, der Abstand zur Kamera und Bildanteil misst.
* Mit dem Kanister im Maul springt der Alligator stellenweise in einem Bild ein Stueck zur Seite: an der Westwand neben
  dem Plattform-Block um bis zu etwa 1500 Einheiten, an der Suedwand um bis zu knapp 600. Die neue Klemme, die Kopf und
  Kanister vor der Wand haelt, und die bisherige Klemme des Koerpers (an der Westwand die am Plattform-Block) arbeiten
  gegeneinander. In den gemessenen Bildern liegt er dabei meist am Bildrand oder wirkt wie ein schneller Schlag (gemessen
  im Stand der Kanister-Szene, vor der Zusammenfuehrung mit der Ankunftsszene).
* Ohne Kanister (beim Verfolgen, Beissen und Belagern) kann der Kopf des RE2-grossen Modells weiter ueber eine
  Aussenwand ragen; die neue Klemme wirkt nur mit dem Kanister im Maul und beim Kanister-Tod.
* Beim Kanister-Tod ist der Alligator in manchen Lagen klein im Bild (in den Kameras 6 und 7 etwa halb so gross wie in
  RE2). Die Regel fuer die Todeskamera (siehe "Zu entscheiden") verlangt Kopf, Kiefer und die ganze Todesanimation im
  Bild, aber keine Mindestgroesse.
* In Marvins bzw. Johns Haenden ragt der Kanister quer vor der Brust weit zur Seite: Er ist so gross wie in RE2, ein
  eigener Massstab ist nicht erfunden.
* Beim Schnitt auf die Wurf-Kamera in 2080 zeigte die Kanister-Szene vor der Zusammenfuehrung ein Bild lang noch den
  alten Hintergrund mit dem schon gedrehten Werfer; ob das Standbild beim Kamerawechsel (siehe "Bewegung und Kamera")
  das jetzt abdeckt, ist im Gesamtstand nicht gemessen.
* Nach dem Laden eines Spielstands, in dem der Alligator tot und der Schluessel nicht aufgehoben ist, liegt der Schluessel
  an einer Ersatzstelle (Mitte der gefressenen Leiche), ohne die Einstellung auf ihn.
* Stahltuer- und Aufschlag-Ton der Kanister-Szene sind ueber die Ton-Zeilen und den Pegel im Mitschnitt belegt, nicht ueber
  die Wellenform - bitte beim Spielen darauf achten.
* Schluessel nach dem Tod an der Suedwand (Entscheid FG-E7): Stirbt der Alligator mit dem Kanister nahe der Suedwand
  (gemessen mit der Kanister-Lage (4430,-26160), 840 vor der Wand), verschiebt die neue Kopf-Klemme seine Leiche um rund
  1000 Einheiten, und die Einstellung auf den Schluessel waehlt danach eine Kamera, in der er nur etwa 3 x 3 Bildpunkte
  gross ist (sonst 10 bis 16). In den fuenf natuerlich gespielten Kampflaeufen trat das nicht auf. Behebung: die Wahl der
  Schluessel-Lage sucht weiter, wenn die beste Kamera ihn zu klein zeigt.
* Die Uebergabe von der Ankunftsszene in 2090 zum Teil in 2080 (Knall und acht Zombies nach der Rueckkehr ueber die
  Leiter) prueft noch kein automatischer Test ueber den echten Weg; von Hand an der exe gemessen haelt sie fuer Leon und
  Elza.

**Masken.**
* Restloch an der Treppe in ROOM6010/6011 Kamera 5: Steht die Figur unter der Treppe an ihrer Westkante, ragen Kopf und
  Schulter ueber die neu eingesetzte Treppenwange (gemessen: 82 von 96 Bildpunkten sichtbar). Die Kamera nutzt alle 128
  Maskenplaetze, die der Port je Kamera hat; das Original erlaubt 255. Vorher hatte die Stelle gar keine Maske.
* Die Datei `analysis/befunde_runde41/M_bilder/NB5/teile_nb5.txt` mit Teilnummern und Gruenden stammt aus einem
  frueheren Lauf; bei 21 Teilen steht dort ein anderes Ergebnis als im Spiel. Die Tabelle im Dossier
  `analysis/befunde_runde41/M_masken36.md` ("Fuer den Nutzer") passt zum Spiel. Ein Pruefstueck (ROOM4040/4041 Kamera 12)
  misst statt der Figur ein 3D-Tor; im Spiel ohne Folge, die Figur ist dort von den alten Masken ganz verdeckt.

**Ihre Freistellungen STAGE3-6 (Ihre Antwort "Ich kuemmere mich selbst drum.", Nachtrag 27).** Ohne Maske bleibt nur,
was kein Original-Teil mit belegbarer Tiefe hat - meist Gelaender ohne eigenen Boden, Teile ohne Bodenkontakt (Lampen,
Rohre, der Roboterarm), Teile, deren Hoehe nicht eindeutig ist, oder Stellen, ueber denen die Kamera direkt haengt.
Teile, die nie vor einer Figur stehen koennen (Bodenmarkierungen, Lampen hoch an der Wand), brauchen keine Freistellung.
Je Kamera zeigt `analysis/befunde_runde41/M_bilder/NB5_rest/NB5_rest_ROOMxxxx_Cnn.png` rot die fehlenden und gruen die
eingesetzten Teile; Bildpunkte und Teilnummern stehen in der Tabelle des Dossiers. Zum Zeichnen liegen die
Uebersichtsbilder unter `pri/STAGEn/<raum>/<cc>_port.png`; nehmen Sie die aus Leons Ordnern (Stand dieser Version, siehe
"Masken"), Elzas Raeume (xxx1) zeichnen dieselben Masken.
* ROOM3040: Kameras 1-9 und 11-13 (in Kamera 3 der Pfeiler rechts und Teile des Laufsteg-Gelaenders)
* ROOM3050: Kameras 0, 4, 5, 7, 8, 9, 11, 12, 13
* ROOM30A0: Kamera 0 (Lampe, Stuhl, Moebel vorn links; dazu der rote Ordner auf dem Schrank am Ostende der Trennwand,
  fuer den es kein Original-Teil gibt) und Kamera 1
* ROOM30E0: Kameras 6-9 (Pfosten und Handlauf des Stegs, soweit noch offen, Bodenstreifen am Stegrand, Lampen)
* ROOM4040: Kameras 7 und 12; ROOM4080: Kameras 2, 3, 8
* ROOM5020: Kameras 2, 3, 5, 7, 9, 11; ROOM5050: Kameras 1 und 2 (Schreibtisch und Pulte vorn, die Kamera haengt direkt
  darueber); ROOM5090: Kameras 11-14
* ROOM50B0: Kameras 1-3 (Roboterarm oben, Maschine); ROOM5100: Kameras 2, 3, 5, 6, 7
* ROOM6000: Kameras 2, 3, 8, dazu in Kamera 5 die Mitte des Gittertraegers an der Wandecke; ROOM6010: Kameras 2, 3, 5,
  6, 7, 10-13 (in Kamera 2 Treppen- und Plattform-Gelaender, in Kamera 5 Treppenwange und Gelaender)
* In der Tabelle fehlen noch acht kleine Stellen (je 106 bis 275 Bildpunkte, ebenfalls ohne Tiefe): ROOM3040 Kamera 10,
  ROOM3050 Kamera 1 und 6, ROOM5050 Kamera 1 und 2, ROOM6000 Kamera 7 und 9, ROOM6030 Kamera 12.

**Pruefungen und weitere Folgeauftraege.**
* Der Ketten-Riegel `integration_r41_ventilkette_kette` (Leon: Ventile -> Flut -> 20B0 -> 2050) endete im
  Integrationsbaum viermal still nach etwa 220 s und einmal mit einer SDL-Meldung, danach viermal gruen; die Ursache ist
  offen (Verdacht: fremde Last auf der Maschine). Fuer die Elza-Kette gibt es noch keinen eigenen Riegel. Im Spiel ist
  die Kette mit Leon und Elza gemessen.
* Ankunftsszene und Kanister-Szene sind getrennt abgenommen: die Kanister-Szene mit ihren Kampf-Zahlen (Aufnahme nach 3
  bis 5 s, Explosion in jedem Verlauf, Lagespruenge) noch ohne die Ankunftsszene und mit den Spinnen in 2090, die
  Ankunftsszene ohne die Kanister-Szene. Das Zusammenspiel prueft die Integration (siehe "Technisch").
* Der PSX-Bau der neuen Alligator-Teile ist nicht geprueft (kein PSn00bSDK auf der Maschine).
* Weitere Folgeauftraege, die Sie im Spiel merken koennen (beschrieben unter "Noch offen"; ob Samstag oder spaeter, ist
  nicht festgelegt): Ihre eigenen Schritte nach Lage und Abstand, die Lautstaerkekurve, die Richtung des
  Hubschrauber-Rotors, die Gorilla-Haenger in ROOM11C0, das etwas zu lange gehaltene Bild beim Explosionsschnitt des
  Alligators, die Abweichungen von 1-3 Bildern nach einem Kamerawechsel.

## Technisch

* **Stand:** r41/integration 74c9d1d9 = master 91ae1a79 (Spuren A, B, C, D, E, H, I, J, K, L; Merges D db7205c2,
  A 5705fafb, I ce081e16, B 9f10bdb0, H 6bbda67a, E b23e4887, J 7c61ed8b, L 1fcfde5a, C 2d20306d, K 4b313429) + Doku bis
  e81d99e0 + Merges F aa8bac53, G 93f64b0a, M 74c9d1d9. F und G sind nach FG-E6/Nachtrag 32b bestanden (Abnahme F 5,
  G 4); M ist mit dem Stand der Nachbesserung 5 integriert (Abnahme M 5 "nicht bestanden", alle drei Maengel nicht
  blockierend). Auf master kommen dazu 76b50a0c (Paket-Gate) und 1e0d0618 (Uebersichtsbilder pri/).
* **Integration F+G+M:** Bau ohne neue Warnung, gezielte Unit-Riegel F/G/M 60/60; tests:FGM: 1003 unit/prop/probe gruen, alle Spur- und Querbezugsgruppen gruen bis auf vier F-Riegel (Kopf-Klemme x Schluessel-Lagewahl) - nach FG-E7 auf eine Todeslage ohne Kopf-Klemme umgestellt.
* **Integrations-Fixes:**
  - bf470083 - enemy_common.c wieder in der Fassung der Spur J (nur Kommentar; gleicher Patch wie Ks Cherry-Pick
    299b6deb, Entscheid A-E5).
  - 40afa677 - Querbezug J x E: die Spieler-Schleife (plcloop_r41.c) misst die Cliplaenge an dem Record, den der
    RBJ-Binder in den Spieler-Kanal schreibt (@0x8001b450); vorher abweichende Cliplaengen in 3091, 4001, 4010, 5011,
    6030, 6031.
  - 45cc6246 - Querbezug E x K: Sherry (Figur 0x0F) im Renn-Tempo/Renn-Stopp (Tempo 120 @0x80073f42, ohne Fussweg
    @0x80032898, Drehen @0x80073f43 im Unterzustand 2); Riegel unit_r41_kleinkram_b_takt_sherry.
  - d1edabbc - Riegel unit_r36_g2_cursor_rahmen prueft ROOM2050 obj 8 mit Bank-9-Bit 106 (Pult geloest); der Port-Zusatz
    der Spur C widerlegte den alten Riegel.
  - Konfliktloesungen in den Merges: 7c61ed8b enemy_common.c rbj_resolve_slot = Seite J (der Spur-A-Haken d1809dbf geht
    darin auf); 2d20306d scd_room_setup.c beide Seiten (L vor C); 4b313429 player_common.c (Ks Waffe-0-Sperre vor Es
    Drehbedingung).
  - Merges F/G/M (je beide Seiten wortgleich, keine neue Konstante): aa8bac53 scd_room_setup.c (Installer L, C, dann F),
    scd_vm.c (Ereignis-Kette C, dann F; Raeume disjunkt), main.c (Prop-Lader C, dann F), audio_pc.c re15_bgm_load_track
    (Cs Alarm-Zeile ersetzt Fs gleichlautende Lesezeile, Fs Umleitung auf MAIN1A danach), test_r36_kartenbits.c (C, dann
    F); 93f64b0a scd_room_setup.c (C, F, dann G), main.c Init (F, dann G), k_port[] (C, F, G ohne Doppelbelegung),
    enemy_ai_boss_gator.c automatisch in der von Abnahme G 4 geforderten Reihenfolge (Wurzel-Klemme, Kopf-Klemme G,
    nachtakt B, nachtakt F), render_pc.c RE15_TIM_SLOT_MAX 57 (G-E6); 74c9d1d9 ohne Code-Konflikt (re15_pri.h 105 -> 128,
    alle Verbraucher ueber die Konstante, auch gator_fx_r41_pc.c).
* **Kette A+C im Integrationsbaum gemessen** (analysis/befunde_runde41/INT_belege/, 7cf41fc9): Leon 2030 -> 20B0 -> 2050
  und Elza 2031 -> 20B1 -> 2051, alle Pruefungen des Ketten-Riegels erfuellt.
* **Vor dem Release:** integration_r41_ventilkette_kette (nur Leon) endete im Integrationsbaum viermal still nach 20B0
  F750 (~220 s) und einmal mit der SDL-Assertion WIN_AddDisplay, danach viermal gruen; das debug.log der roten Laeufe ist
  bytegleich mit gruenen. Ursache offen (Verdacht Fremdlast) - in der vollen Suite auf ruhiger Maschine pruefen. Die
  Elza-Kette hat keinen registrierten Riegel (nur der eigene Lauf). Auch unter "Ab Samstag".
* **A wasser20b0** - NEU `engine/src/wasser20b0_r41.c` (+ `include/re15_wasser20b0_r41.h`): EM024-Partikel-Sequencer grid
  0/1 (Typ 0x24) der Flut 20B0/20B1. NEU `engine/src/wassersprung_r41.c` (+ `.h`): Landespritzer und NPC-Schritte
  (Aufsetzer aus der OWN-Bank, Retail-Zeile nach RE2 FUN_8005c040 wo die Original-Zeile leer ist, Lage/Seite
  FUN_80045a64). RE15_ACTOR_MAX 21 -> 36 (35 Emitter in 20B0, Entscheid E2); der Alligator steht damit auf Platz 35.
  Haken: aot_common.c (AUTO-Ereignis in 20B0/20B1 trotz Szenensperre, ENV-Stempel WATER/RAMP ohne Szenen-Gate),
  enemy_ai_common.c (EM024-Zweig; NPC-Schritt in re15_npc_sub_walk), climb_common.c (Landespritzer), wasser_r38.c
  (Stempel-Getter), scd_vm.c (EM024 ohne Zeichnen in op_sce_em_set), platform/pc/main.c (kein Figurenschatten fuer Typ
  0x24; 4. Wert von RE15_PLAYER_POS = Band), audio_pc.c (4 Zeilen Schritt-Lage, A-E4). Riegel probes/r41_wasser20b0.cmake,
  integration_r41_wasser20b0 und _npc.
* **B re2gator** - NEU `engine/src/gator_re2_r41.c` (+ `include/re15_gator_re2_r41.h`): RE2-EM23-Ablauf (Zonen-Dash,
  Hysterese, Ereignis-Schuetteln, Putz, Kanister-Aufnahme, Kanister-Tod, Treffer-Selektor, HP-Sockel 25300,
  Finisher-Buehne). NEU `platform/pc/src/gator_re2_pc.c` (Kanister-Prop, Raumbank),
  `engine/src/gen/r41_gator40a0_bank.inc` aus `tools/r41_re2gator/gator40a0_se_cut.py`. Eigentuemer-Datei
  enemy_ai_boss_gator.c. Haken: main.c (Hintergrund-Versatz beim Schuetteln, Kanister-Zeichner, Bank-Init, Bildbudget
  der Zeitlupe, untergetauchter Alligator, Elzas Zopf im Maul-Massstab), enemy_ai_re2_zombie.c (Part-Bruecke fuer Typ
  0x23 = geplatzter Kiefer), re2_fx.c (Op 11/73 fuer den Putz). API re15_gr2_* fuer die Szenen. Riegel
  probes/r41_re2gator.cmake, integration_r41_re2gator.
* **C ventilkette** - NEU `engine/src/ventilkette_r41.c` (+ `include/re15_ventilkette_r41.h`, alle Konstanten mit Beleg
  im Kopf), Daten `engine/src/gen/ventilkette_r41_{texte,original,programme,wasser,plakat,hebel}.inc` aus
  `tools/r41_ventilkette/`; NEU `platform/pc/src/ventilkette_r41_pc.c` (Ventil-Standbild, Plakat, Hebel, Countdown).
  Haken: scd_room_setup.c, scd_vm.c, game_step_common.c, main.c (Boot/CONTINUE-Installer, Prop-Lader, Zeichner, zwei
  RBJ-Haken), audio_pc.c (Alarm-Schalter re15_audio_r41_alarm, C-E4/C-E5). Riegel unit_r41_ventilkette,
  integration_r41_ventilkette, _b, _weg, _weg_elza, _sperre, _alarm, _kette (nur mit Spur A registriert, C-E1).
* **D kleinkram_a** - NEU `engine/src/lautstaerke_r41.c` + `platform/pc/src/lautstaerke_r41_pc.c` (VOLUME-Zeile, Datei
  re15_options.cfg, Override RE15_OPTIONS_FILE); NEU `engine/src/blink_r41.c` (Puls = Titelzeilen-Puls FUN_801028ec,
  API re15_blink_r41_setzen); NEU `engine/src/giftkugel_r41.c` (RE2 Op 32, Schaden 20). Haken: audio_pc.c (Endstufe),
  main.c, feuerloescher_r37.c, spinnen_r38.c, game_step_common.c. Riegel unit_r41_kleinkram_a,
  integration_r41_kleinkram_a_lautstaerke / _blink / _gift.
* **E kleinkram_b** - NEU `werkstatt11b1_r41.c` (Port-Ereignis 24 in 11B1), `karte_r41.c` (Kartenmarke ueber die Zielzone
  der Tuer), `plcloop_r41.c` (Spieler-Schleife beim Kurbeln), `kamerahalt_r41.c` + `platform/pc/src/bildhalt_r41_pc.c`
  (Apply-Bild ungezeichnet gehalten, 1 + 6 Bilddauern), `msgtakt_r41.c` (Nachrichten-Takt hinter dem Spiel-Takt),
  `rennstopp_r41.c` (Renn-Tempo und Renn-Stopp nach 0x80032604/0x80032778). Haken: scd_room_setup.c, re15_inv_screen.c,
  player_common.c, game_step_common.c, scd_vm.c, msg_common.c, main.c, render_pc.c. Riegel probes/r41_kleinkram_b.cmake,
  integration_r41_kleinkram_b_* (kurbel, karte, werkstatt_szene/_tuer, kante, geh, renn, spawn_kante, treppe, losl).
* **H kollision** - NEU `engine/src/kollision_r41.c`: NPC-Schub vor der Wandklemme (aec4/b544 in allen 28 NPC-Wurzeln),
  Kisten-Pruefung FUN_8002cdf4 im Objekt-Pass. Haken: enemy_ai_common.c, aot_common.c, game_step_common.c. Riegel
  probes/r41_kollision.cmake, integration_r41_kollision_* (kiste, raetsel_leon/_elza, npc3070, npc3001).
* **I zombiefresser** - NEU `engine/src/zombiefresser_r41.c`: Liege-Wecker der Deskriptoren 0x84/0x85 (Wecker-Distanz
  0xFA0, Wartezeit rand & 0xf). Haken: enemy_ai_re2_zombie.c, enemy_ai_common.c, player_common.c, anim_select_common.c,
  main.c (Ladepfad startet sub00 wie FUN_8003ef6c). Werkzeuge `tools/r41_zombiefresser/`. Riegel
  probes/r41_zombiefresser.cmake, integration_r41_zombiefresser_* (8 Laeufe, u. a. laden2000/2001).
* **J stage3szenen_a** - NEU `rbj_binder_r41.c` (RBJ-Marker-Binder FUN_8001b3f8, Commit e4a9f39c; 1040/1041 ohne
  Spieler-Record -> PL00/PL04-Basis), `wclip_r41.c` (W-Bank-Clip fuer Plc_motion(2,c)), `fussanker_r41.c` (Glied-Anker
  FUN_800369f8), `superobj_r41.c` (Super_set/Super_reset 0x48/0x49). Haken: enemy_common.c, enemy_ai_common.c, scd_vm.c
  (J-E1), main.c (pc_prop_world, Spieler-Kanal), dining_r39.c, platform/psx/src/asset_psx.c. Werkzeuge `tools/r41_j/`.
  Riegel probes/r41_stage3szenen_a.cmake und _rbj.cmake, integration_r41_stage3szenen_a_3060 / _3061.
* **K stage3szenen_b** - NEU `engine/src/sherry_r41.c` + `platform/pc/src/sherry_pc_r41.c` (+ `.h`): Figurwechsel wie
  FUN_800314b0 (EDD/EMR, Waffe, HP, Status), Sherry PL0F/PL0FW00 ohne Waffe, Tempo Zeile 15, Kletter-Ausloesebild 0x63;
  Binder per Cherry-Pick (299b6deb). Haken: player_common.c, climb_common.c, re15_damage.c, main.c. Werkzeuge
  `tools/r41_k/`. Riegel probes/r41_stage3szenen_b.cmake, integration_r41_stage3szenen_b_sherry / _3091 / _marvin.
* **L spinnenanim** - NEU `engine/src/spinnenanim_r41.c`: Figur-Matrix aus der vollen Drehung (+0x68), Massstab 0xccc,
  im Schacht-Ablauf das Bild vor dem Vorschub, Schatten 1300 x 1400. Haken: main.c, enemy_ai_common.c, spinnen_r38.c,
  scd_room_setup.c. Riegel probes/r41_spinnenanim.cmake, integration_r41_spinnenanim.
* **F gatorszene** - NEU `engine/src/gatorszene_r41.c` (+ `include/re15_gatorszene_r41.h`,
  `platform/pc/src/gatorszene_r41_pc.c`): 2090-Ankunftsszene als Programme aus Original-Befehlen (Ereignis 18: P1
  Ankunft + Knall = Knall-Bank Satz 0, RE2 ROOM1060 Se_on 0x0a @0x00F48; P2 2080-Montage mit acht Sce_em_set auf Band 8;
  P3 Rueckkehr, Bit 110), Gator-Ablauf der Szene (Clip 6 bis Bild 22, dann Clips 4/5/11/0/10, Leiche als Kind der
  Part-6-Matrix), Kamera-Uebergabe auf den Eintritts-Cut (Cut_chg-Zwilling LAB_800402a0), Einstellung auf den Schluessel
  nach dem Tod (90 Bilder, Form ROOM2010 sub03 Cut_chg/Cut_old/Cut_auto), Tor 2090/20A0 (RE2-Tuer-Handler LAB_80051514,
  Ereignisse 19/20), Water Key = Item 0x34 mit Name/Text/Icon/Bild des nie ausgelegten Original-Eintrags 0x45 (DEBUG.BIN
  @0x800c495c / @0x800c50de), Weltmodell RE2 Platform Key (ROOM7040 MD1 @0x00354C), eingebacken in
  `engine/src/gen/gatorszene_r41_schluessel.inc`; Spinnen 0x25 in 2090/2091 verworfen. Haken ("Runde 41 Spur F"):
  scd_vm.c, scd_room_setup.c, enemy_ai_boss_gator.c, platform/pc/main.c, audio_pc.c (BGM-Weiche 0xFF3C -> MAIN-Platz 0x3C
  aus RE2/SOUND/MAIN1A.BGM), item_prompt_common.c, item_icon_common.c, itps_common.c, item_discard_common.c,
  re15_inv_screen.c, inv_render_pc.c. Riegel probes/r41_gatorszene.cmake: 12 unit_r41_gatorszene_* (u. a. _abstand mit
  dem RE2-Band 11402..13890 und 121..158 Bilder, aus den RE2-Daten gerechnet), integration_r41_gatorszene_leon / _elza /
  _flucht / _flucht_elza / _tod / _tod_elza / _tod_r1 / _tod_r1_elza / _tod_explosion / _tod_explosion_elza. Werkzeuge
  `tools/r41_gatorszene/`.
* **G kanister2080** - NEU `engine/src/kanister2080_r41.c` (+ `include/re15_kanister2080_r41.h`): Installer 2080/2081/
  2090/2091; P1 Knall + acht Zombies auf Band 8 (Tot-Bits 236..243); Szene nach der RE2-Schwaeche (HP < 25000
  @0x80101ebc-dc): Kopfschuss ueber den Spielerschuss (RE2-Zeile 5, FUN_801092C4), Aufheben nach ROOM11B0 sub06, Kippen
  nach RE2 ROOM40A0 sub10, Zeile 42 mit Wsleep wie RE2, Wurf und Fall nach RE2 ROOM7020 sub12/13, Rueckkehr mit demselben
  Alligator (FG-E3), Anschwimmen im RE2-Zielmodus (FUN_800177c0). NEU `platform/pc/src/kanister2080_pc.c`
  (Kanister-Zeichner, Knall, Tuer-Ton DOOR06 Satz 1, Aufschlag KANISTER2080.VBS, NPC-Schuss) und
  `platform/pc/src/gator_fx_r41_pc.c` (Raum-ESP GATOR40A0 der Explosion; RE2-Kern-Zeilen 485..498 ab dem Start wie RE2
  Boot 0x8002b6d4 / CORE00.ESP FUN_8001babc, G-E4). Daten `engine/src/gen/kanister2080_r41_texte.inc` und
  `gen/r41_kanister2080_bank.inc`. Haken: re2_fx.c (Ops 4/6/37/38/41/42/69/78, Nachtrag 28), gator_re2_r41.c +
  re15_gator_re2_r41.h (Todeskamera G-E1/G-E3, Kopf-Klemme G-E5, Fontaene, Scherben, Hals-Lenkung, Kippen; FG-E3/FG-E5),
  enemy_ai_boss_gator.c (G-E1-Anker, Kopf-Klemme, Anschwimmen), gator_re2_pc.c (Scherben, Regionstest @0x80036534),
  scd_room_setup.c, scd_vm.c, platform/pc/main.c, render_pc.c (RE15_TIM_SLOT_MAX 57, G-E6). Riegel
  probes/r41_kanister2080.cmake: 20 unit_r41_kanister2080_*, integration_r41_kanister2080 / _kampf / _raumfremd.
  Werkzeuge `tools/r41_kanister2080/`.
* **M masken36** - Daten `shared_assets/PSX/MASKS/` (216 neue `ROOMxxxx_PRIcc.TIM` der Stages 3-6; 40 geaenderte
  Dateien: 30 MSK der Stages 3-6, ROOM2030/2040 Kamera 7, ROOM20A0 Kamera 12 ohne 12_02), Ausschluss-Tabelle
  `engine/src/gen/masken36_ausschluss_r37.inc`; gemeinsame Datei `include/re15_pri.h` RE15_PRI_MAX_MASKS_PER_CUT
  105 -> 128 (Original: Maskenzahl als Byte @0x8003928c, Grenze 255; 128 = Breite der Ausschluss-Tabelle). Werkzeuge
  `tools/maske/` (NEU platzieren_r41.py: Original-Atlas-Teile mit Lage per Bildvergleich und Tiefe aus der Kollisionszelle
  mit RDT-Offset, Kamera des Spiels mit SquareRoot0 0x80065f60; dazu schliessen_r41.py, tiefe_zelle_r41.py,
  zellen3d_r41.py, kacheln_r41.py, koerper_r41.py, rueckschritt_r41.py, zuordnung_r41.json; teilen_r41.py nicht
  angewandt, M-E2), Tuer-Band der Saaten nach dem AOT-Scan @0x80042cac..0x80042ccc;
  `analysis/esp_masken_2026-09-03/auswahl.json`: ROOM20A0 12_02 unter "_r41_nicht_gebaut". Riegel
  probes/r41_masken36.cmake: unit_r41_masken36_loecher / _kurbel / _zellen3d / _fern / _stirn / _platziert / _teile /
  _ordner, integration_r41_masken36_stage2 / _zelle / _zellen3d / _fern / _laufpruefung / _teile_a1..d2.
* **pri/** (Nachtrag 33, master 1e0d0618): 113 Uebersichtsbilder `pri/STAGE3-6/<raum>/<cc>_port.png` ersetzt und eines
  neu (STAGE4/4040/05_port.png), 22 schon gleich; Quelle `analysis/befunde_runde41/M_bilder/uebersicht/` (Stand M
  Nachbesserung 5, 284b0202: 136 Bilder, davon 28 in Elza-Ordnern); nur *_port.png, keine Nutzer-Freistellung beruehrt.
  Vorher 919cc1ed (Nachtrag 11, 204 von 214). Dazu 91aa781d: 57 Elza-Bilder (xxx1) bekommen das neue Leon-Bild, jeweils
  nur wo das alte Elza-Bild bytegleich mit dem alten Leon-Bild war (Masken Leon/Elza bytegleich).
* **Nicht geprueft:** PSX-Bau der Module F und G (kein PSn00bSDK auf der Maschine); die PSX-Fassung liest die
  Port-Masken (MASKS) weiterhin nicht.
* **Neue Assets:**
  - `shared_assets/RE2/KANISTER40A0.MD1` (4964 B) und `.TIM` (33824 B) - byteidentisch aus RE2 ROOM40A0.RDT (Modell 1).
  - `shared_assets/RE2/GATOR40A0.VBS` (34296 B) - RE2-Raumbrueller aus ROOM40A0 (Satz 0 Tod, Satz 1 Explosion),
    geschnitten mit tools/r41_re2gator/gator40a0_se_cut.py.
  - `shared_assets/RE2/VENTIL1080.RBJ` (13128 B), `.EDT`, `.VH`, `.VB` - bytegleich aus RE2 ROOM1080 (Dreh-Animation
    und Drehgeraeusch; tools/r41_ventilkette/re2_ventil_export.py).
  - `shared_assets/RE2/SOUND/SUB_25.BGM` (100688 B) - bytegleich `info/re2leon/COMMON/SOUND/SUB_25.BGM` (Alarm).
  - `shared_assets/RE2/FILES/FILE31_title_page.TIM`, `FILE31_p00_page.TIM`, `FILE31_p01_page.TIM`, `FILE31_p02_page.TIM`,
    `FILE31_title_paper.TIM` - Dokument "Technician's Note", gesetzt mit tools/r41_ventilkette/dokument_satz.py
    (title_paper = Kopie von FILE17).
  - `shared_assets/RE2/SOUND/MAIN1A.BGM` (228152 B) - RE2-Kampfmusik des Alligators, bytegleich aus
    `info/re2leon/COMMON/SOUND/MAIN1A.BGM` (md5 01835748815e388e2519d4977d48aab5).
  - `shared_assets/RE2/GATOR40A0.ESP` (3100 B) und `GATOR40A0_08/09/16/18/19/1A/26.TIM` - Effekt-Datei der
    Kanister-Explosion und ihre sieben Texturen, bytegleich aus RE2 ROOM40A0.RDT.
  - `shared_assets/RE2/KANISTER2080.VBS` (9864 B, md5 848c40f6c077d937fb025df9ae1bc62b) - RE2 ROOM40A0 Raumbank Satz 11
    (der Kanister faellt ins Wasser), geschnitten mit tools/r41_kanister2080/kanister2080_se_cut.py.
  - `shared_assets/PSX/MASKS/`: 53 Kameramasken aus Ihren Freistellungen (5924939d); dazu die Dateien der Spur M (216
    neu, 40 geaendert, siehe oben).
  - Eingebacken, ohne Paketdatei: Plakat, Zettel, Hebelbilder, Wasserflaeche (aus ROOM2080), Water Key (Modell, Textur,
    Name, Icon, Bild), Texte der Kanister-Szene. Die Countdown-Schrift ist die schon gepackte `shared_assets/RE2/TEX.TIM`,
    der Knall die schon vorhandene `engine/src/gen/knall_bank.inc`.
  - Paket-Gate (release/make_package.sh check_tree, master 76b50a0c): die 23 neuen RE2-Dateien der Runde 41 (C:
    FILE31-Seiten, SOUND/SUB_25.BGM, VENTIL1080.*; B: GATOR40A0.VBS, KANISTER40A0.*; F: SOUND/MAIN1A.BGM; G:
    GATOR40A0.ESP + 7 TIM, KANISTER2080.VBS) muessen im Paket liegen, nicht leer und bytegleich sein; ebenso jede Datei
    unter shared_assets/PSX/MASKS (am Stand 76b50a0c 474 Dateien, mit Spur M 690). Die Release-Skript-Tests nutzen eine
    Attrappe und sind nicht betroffen.
* **Speicher-Bits Bank 9** (eingetragen in tests/unit/test_r36_kartenbits.c k_port[]): Spur C 100 Leiter frei,
  101 Ventil 13 offen, 102 Bruecke unten, 103 Flut gelaufen, 106 Pult 2050 auf 80 V, 107 "Technician's Note" genommen,
  125 Sicherung im Ventilkasten 2010; Spur F 110 Ankunftsszene gesehen, 111 Water Key liegt, 112 Water Key genommen,
  113 Tor 2090/20A0 aufgeschlossen; Spur G 114 2080 nach der Szene betreten (Knall), 115 Kanister-Szene gelaufen,
  116 Kanister liegt im Becken, 117 Wurf ins Suedbecken.
* **Port-Text-Ids** (Spur C): 2000/2001/20B0/20B1 Id 40; 2030/2031 Ids 40-55 (49 = Original-Satz); 2050/2051 Ids 40-44;
  2010/2011 Ids 40-44; 20B0/20B1 Id 41; 2070/2071 Id 40. Spur F: 2090/2091 Ids 42/43 (Original-Saetze aus ROOM20A0).
  Spur G: 2080/2081 Id 42.
* **Angenommene Haken in gemeinsamen Dateien:** B5, A-E3, C-E3, C-E4/C-E5, E-E1/E-E5, H-E1, I-E1, J-E1. Formal noch
  offen: Haken der Spur I in anim_select_common.c und platform/pc/main.c (Abnahme I 1, V2) und der Spur L in
  platform/pc/main.c und scd_room_setup.c (Abnahme L 0, H4), je ausserhalb der Liste VERTRAG 1.3. Dazu FG-E3/FG-E5
  (Spur G in Spur Bs Modulen), Nachtrag 28 (re2_fx.c), G-E6 (render_pc.c); die markierten Haken der Spur F (auch in
  audio_pc.c und den Item-/Inventar-Dateien) bewertet Abnahme F 5 als vertragsgemaess; Spur M aendert in re15_pri.h nur
  eine Konstante.
* **Nachgezogene fremde Riegel** (neue Regel widerlegt alten Riegel, je mit Beleg): unit/integration_r36_gegnerzahl
  (36 Plaetze, 35 Emitter), integration_r35_granate (Alligator-Platz aus re15_actor.h), probe_gator_sweep /
  probe_r34_reaktion / probe_gator_fress, test_r36_dachkiste.cmake / test_r37_itemband.cmake (Querbezugs-Flags, C-E3),
  unit_r38_spinnen teil_b (RE2-Giftkugel 100 - 20), unit_r36_zombieki_affen_block (E-E4: Haenger-Mass je Seed, drei
  benannte Stellen), test_r39_dining1141_kampf (WARTE 60 -> 240, H-E1), unit_r36_g2_cursor_rahmen (d1edabbc). Spur F
  (F-E1 und Querbezuege): unit_r36_tueren_schott, unit_r38_spinnen (2090 ohne Spinnen), unit_r30_tuer_verschlossen
  (54 Plaetze, je ein Ton), integration_r41_re2gator (Kampf-Laeufe mit Bits 110/113, sechster Lauf ueber die
  Ankunftsszene), integration_r38_wasser, integration_r37_itemband, integration_r41_wasser20b0_npc (je mit Bit 110 bzw.
  110/113), probe_r26_inventar; Spur G: test_r41_re2gator.c um die Teile kamera, scherben und kopfklemme erweitert.
* **Tests:** 14 Registrierdateien `tests/unit/probes/r41_*.cmake`, 38 Ablaufskripte `tests/integration/test_r41_*.cmake`
  (mit F, G, M; F, G und M registrieren zusammen 67 Riegel, ctest -N im Integrationsbaum). Die sechs Ventilketten-Riegel
  der Spur C (ohne _kette) brauchen zusammen etwa 92 min (RUN_SERIAL), _kette etwa 4 min, integration_r41_kanister2080_kampf
  etwa 21 min (Abnahme G 4: 1240 s); fuer die Release-Suite einplanen (Abnahme C 6 (d), INT_belege/riegel_kette_laeufe.txt).
* **Messschienen (nur Diagnose):** RE15_NPCSCHRITT_LOG, RE15_FOOT_DEBUG (NPC-Schritte); gator_boss.log, Testhaken
  RE15_GATOR_KANISTER und RE15_GB_TEST=5 (Alligator); RE15_KAMERAHALT_LOG (Standbild); RE15_ZOMBIEFRESSER_LOG;
  RE15_SPINNENANIM_LOG; RE15_FUSSANKER_LOG; RE15_KOLLISION_LOG; RE15_R41_HP_AT (Gift); RE15_K2080_GATOR_HP
  (Alligator-HP beim Raumeintritt, Kanister-Szene nach einem Treffer), RE15_K2080_WURF_GIER (Wurfrichtung),
  RE15_GATOR_FX_LOG (Explosions- und Kern-Quads je Bild); Zeilen [gatorszene] und [k2080] im debug.log.

# v0.8.28 - 2026-10-23

Ihr Auftrag vom 23.10. (Runde 40): "Also, bei ROOM 1120/1121: 1, 3 und 4 koennen wir gerne zuordnen. 1070/1071 wuerde
ich erst einmal so lassen."
Geprueft unter Windows: alle Unit-, Eigenschafts- und Sonden-Riegel und die Spiel-Laeufe durch 1120/1121. Die volle
Suite laeuft beim Bau im Linux-Container; ein roter Test haelt das Release an.

## Was Sie merken

**ROOM1120/1121 (Buero mit Aufzug, Leon und Elza): drei neue Untersuchungsstellen.** Die Texte standen schon in der
Original-Texttabelle des Raums, das Skript hat sie aber nie gezeigt. Jetzt:
- das bunte Gemaelde an der Wand gegenueber dem Schreibtisch (nicht das braune ueber dem Sofa): "A painting, titled:
  The Music Lesson"
- das Jalousiefenster an der Ostwand (mit den erleuchteten Fenstern dahinter): "Only the lights of the buildings are
  visible."
- der Schrank mit den Ordnern in der Ecke hinter dem Schreibtisch: "Various reports and case information. Nothing
  useful."
Wie alle Untersuchungstexte des Originals ohne Stimme. Telefon (Speichern), Aufzug und Tueren sind unveraendert.
Die Vorschlaege fuer 1070/1071 bleiben wie gewuenscht liegen (Bilder: `analysis/untersuchungstexte_2026-10-23/`).

**Ihre neuen Aufnahmen vom 19.10. sind drin (45 Dateien, unveraendert).** Elza und Sherry im Kanal (ROOM2001,
`main06`-`main14`), die Ankunft im Labor (ROOM3001 `main03`-`main09`), Sherrys Geheimgang (ROOM3061 `main01`-`main09`),
Leon/Marvin/Ada am Lift (ROOM3070 `main00`-`main04`), Ada in ROOM3080 (`main00`, `main01`) und die Szene in ROOM30E0
(`main00`-`main05`, `main07`-`main13`). Jede Datei gehoert zu der Zeile mit derselben Nummer; die Sprecher passen.

**Ihre Aufnahmen vom 23.10. ebenfalls (22 Dateien, unveraendert):** John, Elza und Sherry nach der Rettung in ROOM3091
(`synchro/STAGE3/room3091/main09`-`main30`), von "That was a close shave!" bis Elzas "Thanks!". Jede Datei gehoert
zu der Zeile mit derselben Nummer der Texttabelle des Raums (Zeilen 9-30 sind genau diese Szene).

**Nahaufnahmen, die das Original nie zeigt (Ihre Frage zu ROOM11D0 und "Wenn ja, baue die ruhig mit ein").** Ich
habe alle STAGE1-Kameras gesucht, auf die nichts schaltet (keine Kamerazone, kein Skriptbefehl, keine Tuer). Wo eine
Nahaufnahme vor einer Untersuchungsstelle mit passendem Text steht, zeigt sie jetzt beim Untersuchen - bei Leon und
Elza, danach geht die Kamera zurueck:
- ROOM11D0 Zwinger: das Schaltpult mit den Hundenamen (BOCKER, COURIER, GUTS, ROCKEY, DOGMEAT, JoJo) zu "A panel to
  control the cells of the dogs. It does not work." Ein Raetsel dazu gibt es in den Daten nicht (kein Text mit den
  Namen, kein Flag, kein Gegenstand); die Hunde kommen weiter ueber die Falle im Lager.
- ROOM1110 Asservatenkammer: die Waffenspinde zu "Weapons seized, those that remain are unusable." und der Spind mit
  "EXPLOSIVE" bei "Will you take the Time Bomb?".
- ROOM1100 Stimmschloss (VUS): ohne Minidisc Player zeigt das Display jetzt "SYSTEM READY", "SAY HELLO", "INVALID" und
  "TRY AGAIN", dann der Satz "A type of voice device is required to open the lock." Die beiden Fehleranzeigen lagen
  im Original fertig vor, wurden aber nie geschaltet. Mit Minidisc Player laeuft alles wie bisher.
- ROOM11F0 Notstrom: das Pult an der Wand waehrend der Beschreibung und der Frage "Will you push them?"; mit "Ja" geht
  es wie bisher in die Bedienansicht, mit "Nein" zurueck in den Raum.
Ohne passende Stelle und deshalb nicht eingebaut: Messlatte in 1010, ausgebranntes Fahrzeug in 1090, Bett in 10E0,
Schild auf dem Dach (1171), Bahnfenster im Schiessstand (1190).

## Technisch

* Neues Modul `engine/src/untersuchen_r40.c` (+ `include/re15_untersuchen_r40.h`): Text-Plaetze 5/6/7 in der Satzform
  des Telefon-Platzes (ROOM1120 main00 @0x00CF8), Orte aus den Kamerabildern zurueckgerechnet. Die RDTs bleiben
  unveraendert.
* Riegel `unit_r40_untersuchen1120`: beide Raeume, Aktion vor Bild/Fenster/Schrank oeffnet genau diese Texte; Telefon
  bleibt Speicherpunkt; andere Raeume bekommen nichts.
* Neues Modul `engine/src/nahaufnahmen_r40.c` (+ `include/re15_nahaufnahmen_r40.h`): kurze Programme aus Original-
  Befehlen (Vorbild: Elzas Leichen-Nahaufnahme ROOM1051 sub03 und die VUS-Folgen ROOM1100 sub02/sub08); Zeitbombe und
  Notstrom-Pult umhuellen die Original-Abschnitte Byte fuer Byte. Riegel `unit_r40_nahaufnahmen` (53 Pruefungen).

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
Beim Wechsel aufs Dach (1171) klingt die Kampfmusik nicht mehr zwei Sekunden nach. Nach der Endszene hat Elza wieder
ihre normale Gehanimation (die Szene gab die Steuerung ohne den Original-Abschluss zurueck, ROOM1141 sub06 @0x010D2).

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
* Ihre neuen Aufnahmen fuer ROOM3020 (Stage 3): `synchro/STAGE3/room3020/main00..02.wav` - Leon "Dammit! the door
  does not open!" und Marvins zwei Zeilen; sie spielen automatisch an ihren Zeilen.

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
