# RE1.5 Port — v0.3.22 (Early Preview)

## RE2-Modus: Leon gibt beim Gepacktwerden wieder einen Laut von sich

Gemeldet: *"Bei RE 2 AI - Sowohl die biss sounds als auch der finisher biss sound des zombies
fehlt."*

Der Biss des Zombies selbst war in Ordnung — nachgemessen feuert er sauber im Takt der
Animation. Gefehlt hat **Leons** Laut: wenn ein stehender Zombie zupackt, stoehnt Leon im
Original beim Zugriff, wobei das Spiel per Zufall zwischen zwei Aufnahmen waehlt. Der Port
hatte diesen Laut fuer den stehenden Griff ausdruecklich unterdrueckt — mit einer Begruendung,
die sich beim Nachsehen als falsch herausstellte: die zustaendige Stelle spielt ihn sehr wohl,
nur dreiundzwanzig Befehle weiter. Jetzt ist er da, samt der Zufallswahl.

## RE2-Modus: der Biss waehrend des Gefressenwerdens

Zwischen dem Zusammenbrechen und dem ersten Laut lagen im Port fast fuenf Sekunden voellige
Stille — der erste Biss kam erst mit der Fress-Schleife. Im Original faellt er viel frueher,
auf einem festen Bild der Animation. Genau dieser Biss fehlte und ist jetzt an der richtigen
Stelle.

## Linux/Steam Deck: das Startskript war unbrauchbar

`run.sh` wurde mit Windows-Zeilenenden ins Paket gelegt — auf dem Deck scheitert so ein Skript
schon an der ersten Zeile. Der Paketbau zieht die Datei jetzt beim Kopieren auf Unix-Enden und
prueft das fertige Paket zusaetzlich; ein Paket mit falschen Zeilenenden kann nicht mehr
entstehen.

## Noch offen

Der schwebende Zombie in 10D0 ist **nicht** behoben. Die Ursache ist gefunden — sie liegt
nicht an diesem Raum, sondern am gemeinsamen Skelett-Umbau des RE2-Modus, und der Versatz
faellt in anderen Raeumen genauso gross aus. Das gehe ich als eigenen Punkt an, statt hier
etwas zurechtzubiegen, das an anderer Stelle kippt.

# RE1.5 Port — v0.3.21 (Early Preview)

## Leons letzter Laut beim Gefressenwerden — im RE2-Modus fehlte er ganz

Gemeldet: *"Bei RE 2 AI - finales todes Stoehnen von Leon fehlt beim Zombie Finisher, wenn sie
ihn fressen. Bei den Hunden genauso."*

Der Laut ist im Original ein eigener Eintrag der Kern-Klangbank — mit einer halben Sekunde die
laengste Probe darin, und im ganzen Spiel ausschliesslich in Toetungs-Sequenzen benutzt. Er
haengt an einem festen Bild der Kollaps-Animation. Die beiden RE2-Zweige hatten schlicht
ueberhaupt keine Klangausgabe; jetzt stoehnt Leon in allen vier Faellen (Zombie und Hund,
beide Modi) an genau der Stelle des Originals.

## Tod durch Kraehen: die richtige Animation

Gemeldet: *"Die Finisher Animation wenn man von Kraehen getoetet wird stimmt noch nicht."*

Die Kraehe hat gar keine eigene Toetungs-Sequenz — alle drei Angriffsarten loesen im Original
dieselbe allgemeine Spieler-Todes-Animation aus. Genau diesen Weg kannte der Port nicht.
Er ist jetzt nachgebaut, samt der Feinheit, dass sich die Blutlache erst am Ende der
Animation bildet und nicht sofort.

## Der Zombie im Empfangsflur steht jetzt sauber auf

Gemeldet: *"beim aufstehen wiederholt sich die Eine Animation noch einmal kurz, bis er dann
letzlich steht. Ausserdem wurde mein Schuss bei ihn auf den Boden wieder nicht mit Blut
quittiert."*

Beim Wechsel von der Sitz- in die RE2-Bewegung sackte er noch einmal kurz zusammen, bevor er
endgueltig stand. Der Uebergang traegt jetzt sofort die richtige Animation. Und Treffer auf
ihn am Boden bluten wieder sichtbar.

## Feuer im Ostfluegel: jetzt zuckt Leon wirklich

Gemeldet: *"Bei beiden fehlt noch die Verletzt Animation wenn man in das Feuer reinlaeuft bei
ROOM 1090."* — und das zu Recht, denn der Fix aus v0.3.18 war die falsche Haelfte.

Der Ausloeser hat schon damals funktioniert. Die Verletzt-Haltung wurde aber aus dem falschen
Animations-Vorrat geholt (dem des Raumes statt dem allgemeinen des Spielers) und blieb deshalb
auf einem einzigen eingefrorenen Bild stehen — sichtbar passierte nichts. Ueber vierzig Bilder
gemessen: vorher ein einziges Standbild, jetzt neununddreissig verschiedene. Das betrifft auch
Niederschlag und Sterben in jedem Raum.

## Hund-Finisher: Leon liegt jetzt richtig

Gemeldet: *"Bei RE 1.5 AI, stimmt die positionierung des Hundes noch nicht ganz beim finisher."*

Im Original werden beim Zupacken beide Figuren auf denselben Punkt gesetzt; im Port drifteten
sie auseinander. Der gemessene Maximalabstand faellt von 2433 auf 1583 Einheiten.

## Der Kopf des kriechenden Zombies wird wieder zertreten (RE2-Modus)

Gemeldet: *"Bei RE 2 AI, bspw. ROOM 1030 lobby fehlt der 'smash' sound, wenn ich einen am
boden herankriechenden Zombie der mich beisst der Kopf zertrete. Ausserdem wird der Kopf
nicht zertreten und der Zombie kriecht weiter."*

Der Tritt ist keine eigene Spieler-Aktion, sondern gehoert zur Befreiung aus dem Bein-Biss:
sobald Leon sich losreisst, laeuft eine feste Sequenz ab, in der der Kopf abgetrennt wird.
Der Port hat diese Sequenz viel zu frueh verlassen — nach einem Siebtel ihrer Laufzeit — und
sprang damit ueber den Moment des Zertretens, ueber den Halsstumpf und ueber den Tod des
Kriechers hinweg; er richtete sich wieder auf und kroch weiter. Jetzt laeuft sie vollstaendig:
Kopf ab, Blut, der Aufprall-Klang, und der Kriecher bleibt liegen.

## Ein Punkt, der kein Fehler war

*"Ausserdem reisst er hier den Kopf nicht ab im Gegensatz zu RE 2 AI"* (Hund, RE1.5-Modus) —
richtig beobachtet, aber RE1.5 hat diesen Kopfabriss im Original schlicht nicht; nur die
RE2-Fassung kennt ihn. Er bleibt deshalb dem RE2-Modus vorbehalten, statt ihn dazuzuerfinden.

# RE1.5 Port — v0.3.20 (Early Preview)

## Das Spiel findet seine Dateien jetzt selbst — einfach re15_pc.exe starten

Gemeldet: *"Der 0.3.19 package Build ist definitiv kaputt. Es fehlen die Optionen in config,
die subtitles usw. Da ist vieles kaputt dran, wenn ich ihn direkt ueber die Exe starte wie
immer."*

Der Bericht war komplett richtig, und die Ursache ist aelter als v0.3.19. Das Spiel suchte
seine Dateien an einem Ort, der beim BAUEN festgelegt wurde — auf dem Entwicklungsrechner
zeigte der zufaellig ins Projektverzeichnis, weshalb dort alles lief. Auf jedem anderen
Rechner zeigt er ins Leere. Aufgefangen hat das nur die mitgelieferte Start-Datei, die dem
Spiel die Pfade von aussen mitteilt; wer stattdessen die .exe direkt startete, bekam ein
Spiel ohne Schrift.

Und ohne Schrift faellt sehr viel aus: die Beschriftungen im CONFIG-Bildschirm, die
Untertitel, Dialoge und Gegenstandsnamen — alles unsichtbar. Dazu fehlten Blut, Muendungs-
feuer, Rauch und Huelsen sowie Inventar- und Status-Grafiken.

Jetzt sucht das Spiel seine Dateien **neben der eigenen .exe**: Doppelklick auf
`re15_pc.exe` genuegt, aus jedem beliebigen Verzeichnis, auch ueber eine Verknuepfung.
Die Start-Datei bleibt liegen, wird aber nicht mehr gebraucht.

Zwei Feinheiten sind gleich mit erledigt: Wird das Spiel aus einem fremden Verzeichnis
gestartet, das selbst Spieldateien enthaelt, gewinnen jetzt immer die aus dem eigenen
Ordner (vorher konnten sich zwei Datenbestaende unbemerkt vermischen). Und Ordnernamen mit
Sonderzeichen werden korrekt gelesen.

## Damit das nicht noch einmal passiert

Der Paketbau startet das fertige Paket ab jetzt selbst — zweimal, aus verschiedenen
Verzeichnissen — und prueft fuer 26 wichtige Dateien, WOHER sie tatsaechlich geladen
wurden. Stammt auch nur eine davon aus dem Projektverzeichnis statt aus dem Paket, bricht
der Paketbau ab. Die bisherige Pruefung sah nur nach, ob die Dateien im Paket *liegen* —
und genau daran ist v0.3.19 vorbeigerutscht.

# RE1.5 Port — v0.3.19 (Early Preview)

## RE2-Modus: der Kriech-Biss ist komplett — mit Aufsteh-Phase und treffbarem Kriecher

In v0.3.18 stand noch: *"Die RE2-eigene Bein-Biss-Opferanimation braucht noch ihre
Aufsteh-Phase und folgt in einer spaeteren Version."* — das ist diese Version.

Packt ein RE2-Kriecher Leons Bein, laeuft jetzt die komplette RE2-eigene Opfer-Sequenz:
die Kriech-Biss-Ringkampf-Clips (vorn und hinten), der Abwurf, und danach Leons
Aufsteh-Animation mit der originalgetreuen Drehung. Dazu ist ein unsichtbarer Fehler
behoben, der erst beim Scharfschalten sichtbar wurde: der vom Bein geschuettelte
Kriecher landet im Original direkt in seiner Liege-Phase — im Port galt er dort nicht
als "liegend" und war nach dem Abwurf **unbeschiessbar**. Der Treffer-Aufloeser kennt
jetzt auch die RE2-eigene Liegend-Markierung; der abgeworfene Kriecher laesst sich
wie im Original am Boden erledigen.

## Unter der Haube

- Beide Plattform-Binaries entstehen jetzt vollstaendig im Docker-Cross-Build
  (docker_win_build.sh / docker_linux_build.sh); Testlauf 224/224.

# RE1.5 Port — v0.3.18 (Early Preview)

## RE2-Modus: die Zombies haben wieder Fall- und Aufsteh-Geraeusche

Gemeldet: *"Bei RE 2 AI fehlen die fall sounds der Zombies"*.

Der RE1.5-Aufprall ist kein Klang-Sample, sondern ein Mechanismus: die Sturz-Animationen
tragen an ihren Boden-Kontakt-Bildern eigene Klang-Marker. Der Aufprall-Thud feuert jetzt
am Aufschlag-Uebergang jedes Sturzes (auch beim Todes-Sturz), und das Aufstehen spielt —
wie im RE1.5-Original immer — das Aufsteh-Stoehnen (der RE2-eigene Wuerfelwurf war praktisch
immer stumm).

## RE2-Modus: der Zombie im Empfangsflur SITZT jetzt

Gemeldet: *"der Zombie sitzt immer noch nicht da wie im original. Er liegt."*

Der Hinweis war der Schluessel: die Original-Pose ist ein Wandsitz (Oberkoerper aufrecht),
und die RE2-Animationsbank hat schlicht keinen Sitz-Clip. Der Schlaefer spielt jetzt auch im
RE2-Modus die komplette RE1.5-Sitz-Sequenz (Sitzen, Aufwachen bei Naehe, Aufstehen mit
Stoehnen) und uebergibt erst danach an das RE2-Gehirn.

## Kriechende Zombies: der Bein-Biss sieht und klingt jetzt richtig (RE1.5)

Gemeldet: *"wenn mir die kriechenden Zombies in das Bein beissen stimmt Animation und
Sound noch nicht so ganz."*

Leon spielte beim Kriech-Biss die Steh-Ringkampf-Animation. Im RE1.5-Modus laeuft jetzt der
originale Bein-Biss-Satz (eigene Packen-/Schuetteln-/Abwurf-Clips), dazu der bislang fehlende
Griff-Grunzer, das Bein-Wund-Decal und der Blutspritzer am Bein beim Abschuetteln; der
RE2-Kriecher stirbt nach dem Abwurf (wie im RE2-Original). Die RE2-eigene Bein-Biss-
Opferanimation braucht noch ihre Aufsteh-Phase und folgt in einer spaeteren Version.

## RE2-Modus: Treffer auf liegende Zombies zeigen wieder Blut

Gemeldet: *"wenn ich zombies am boden anschiesse fehlt das blut und somit das treffer
feedback."*

Der Treffer wurde registriert (Schaden lief), aber der Blut-Spawn sitzt im Original im
wieder betretenen Liege-Zustand — genau diese eine Stelle fehlte. Jetzt blutet der liegende
Zombie bei jedem Treffer.

## Feuer im Ostfluegel-Korridor: Leons Reaktion sitzt

Gemeldet: *"Leons getroffen Animation beim Feuer room 1090 stimmt noch nicht ganz."*

Zwei Fehler: die Getroffen-Animation wurde 22 Bilder lang weichgeblendet statt hart zu
schnappen (das Original schneidet hart), und der Ausloeser lief ueber einen Umweg, der bei
sehr niedriger Energie gar nicht mehr feuerte und nach einem geloeschten Feuer die falsche
Reaktion waehlte. Das Feuer staggert Leon jetzt direkt, wie im Original.

## RE2-Modus: tote Hunde liegen in Seitenlage

Gemeldet: *"Die Todes Pose der Hunde ist immer noch falsch."*

Beim Uebergang in den Leichen-Zustand sprang die Pose auf das erste Bild der Kollaps-
Animation zurueck — ein fast aufrecht eingefrorener toter Hund. Die Leiche haelt jetzt das
letzte Bild (flache Seitenlage), wie es das Original ueber seine eingefrorenen Matrizen tut.

## Hund-Finisher: Leon bleibt am Hund

Gemeldet: *"wenn der Hund Leon toetet ist der finisher versetzt"*.

Der Spieler-Handler des Hunde-Griffs ist jetzt vollstaendig nachgebaut: die Opfer-Animation
spielt genau einmal und friert dann in der Endpose ein. Vorher startete sie neu und Leon
sprang dabei ~1,7 Meter zum Ankerpunkt zurueck.

## Nach dem Laden hat die Handfeuerwaffe wieder ihre Textur

Gemeldet: *"nach neu laden fehlt der equipped Handfeuerwaffe die Textur."*

Nach Tod -> Titel -> Laden wurde die Waffen-Textur aus der Koerper-Textur gewaschen, aber
der Neuaufbau-Waechter hielt sich fuer aktuell. Kurios: ein Spielstand MIT Blutflecken
heilte den Fehler selbst — darum trat er so unregelmaessig auf. Jetzt wird der Waffen-
Aufdruck nach jedem Neuladen wieder aufgebaut.

## Unter der Haube

- Der Windows-Build entsteht ab dieser Version reproduzierbar im Docker-Cross-Build
  (release/docker_win_build.sh) — unabhaengig vom Zustand der Entwickler-Maschine.
- Gesamt-Audit des Ports als AUDIT_GESAMT_2026-08-23.md im Repo; 13 RE-Dossiers zu den
  verbleibenden Konstruktions-Punkten unter analysis/konstruktion_2026-08-23/.
- Java-Extraktor: ./gradlew build/test laufen wieder.

# RE1.5 Port — v0.3.17 (Early Preview)

## Linux: das Voiceover ist jetzt im Paket

Gemeldet: *„in den gepackten packages, zumindest bei Linux, fehlt das voiceover"*.

Die Sprachdateien lagen in **keinem** Paket — auch nicht im Windows-Paket. Auf dem
Entwicklungsrechner fand die Suchkette die Dateien zufaellig im Repo nebenan, deshalb fiel
es dort nie auf. Jetzt packt der Paketbau die 37 Sprachdateien (17 MB) mit ein, und ein
neues Pruef-Gate bricht den Paketbau ab, wenn sie je wieder fehlen sollten.

## RE2-Modus: die Zombies klingen jetzt wie RE1.5

Gemeldet: *„Bei RE2AI haben die Zombies nicht den Sound von RE 1.5 AI. Sound usw soll
uebernommen werden, nur KI nicht."* — dazu: *„im dinner room fehlen die fressgeraeusche"*
und *„in der Lobby fehlt das kriech geraeusch"*.

Alle Zombie-Gerauesche des RE2-Gehirns laufen jetzt auf die RE1.5-Klangbank: Schritte,
Stoehnen, Biss, Sturz, Tod. Dazu kamen drei Geraeusche, die vorher ganz fehlten: das
**Fress-Schmatzen** an der Leiche (die RE2-Animationsdaten markieren die Kau-Bilder selbst —
RE2 hat sie nie abgespielt, jetzt loesen sie den RE1.5-Biss aus), das **Schleif-Geraeusch**
des Kriechers (zweimal pro Kriech-Zyklus, wie im RE1.5-Original) und das **Aufwach-Stoehnen**
des gestoerten Fressers (Original: ein Viertel-Wuerfelwurf — der fehlte sogar im RE1.5-Pfad).

## RE2-Modus: der liegende Zombie stirbt jetzt sichtbar

Gemeldet: *„wenn der Zombie auf dem Ruecken liegt und bekommt den finalen Schuss, gibt es
noch keine Todesanimation vor dem auslaufen."*

Der Todes-Handler fuer den am Boden getroffenen Zombie war ein Platzhalter (eine
Kriecher-Zuckung). Jetzt ist der echte RE2-Handler portiert: der Koerper **sinkt langsam in
die Leichenlage** (ueber ~30 Bilder ausgeblendet), je nach Lage auf Ruecken oder Bauch, mit
Blut, Todeslaut und den Sonderfaellen fuer Brand/Saeure-Treffer — erst danach beginnt die Lache.

## RE2-Modus: der Finisher frisst am Hals, nicht am Becken

Gemeldet: *„bei den finisher wenn die Zombies Leon fressen sind sie immer noch auf Becken
Hoehe statt auf Hals Hoehe."*

Die Choreografie selbst war korrekt — aber der Bildschirm wurde **zu frueh schwarz**: Der
Port meldete Leons Tod schon beim Biss, das Original erst **nach** dem Zusammenbruch. Die
RE2-Fress-Choreografie erreicht die Hals-Formation erst bei Bild ~110; das alte Timing
blendete bei Bild 77 ab — man sah also genau nur die Becken-Phase. Jetzt startet die
Todes-Einblendung wie im RE2-Original erst mit dem Ende des Zusammenbruchs, und die
Hals-Formation ist sichtbar.

## Kraehen: kein Phantom-Blut mehr, Zerreissen nur noch bei schweren Waffen

Gemeldet: *„bei den Kraehen sehe ich teilweise Bluteffekte, obwohl weder Leon noch die
Kraehen getroffen worden"* und *„Bei RE1.5 AI fehlen noch die zerreisseffekte …, die
manchmal bei RE 2 AI mit der Pistole auftreten."*

Beides war derselbe Befund von zwei Seiten: (1) Der Platzhalter fuer das RE2-**Feder**-FX
spawnte Blut — bei Abheben, Kreisen und Trudeln, ganz ohne Treffer. Der Blut-Spawn ist raus.
(2) Das Zerreissen bei **Pistolen**-Treffern im RE2-Modus war ein Fehler: Die Trefferzeile
wurde aus dem Flugzustand statt aus der **Waffe** gewaehlt. Beide Originale (RE1.5 wie
RE2) zerreissen Kraehen **nie** mit der Pistole — nur Magnum, Granaten und Rakete. Der
RE1.5-Modus war hier von Anfang an korrekt; jetzt stimmt auch der RE2-Modus.

## RE2-Modus: der Zombie neben Marvin liegt richtig und bleibt verwundbar

Gemeldet: *„der Zombie nach Marvins cutscene liegt falsch. ausserdem ist er teilweise
unverwundbar."*

Zwei getrennte Fehler: (1) Die beiden RE2-Liege-Animationen sind **Spiegelseiten** — und der
Port wuerfelte nie, er nahm immer die falsche. Der Schlaefer liegt jetzt auf der Seite, auf
der ihn das RE1.5-Original zeigt (Kopf und Fuesse waren vorher vertauscht — daher „liegt
falsch"). (2) Wer den **aufwachenden** Zombie anschoss, konnte ihn fuer immer unverwundbar
machen: Beim Wecken wurde nur eine von **zwei** Sperren geloescht, die das Original in
einem Zug loescht. Jetzt fallen beide zusammen.

## RE2-Modus: keine toten Hunde mehr in der Luft

Gemeldet: *„Bei RE2 AI haengen die Zombiehunde nachdem sie gestorben sind mit eingefrorener
Animation in der Luft."*

Die drei Zwinger-Hunde werden vom Raum-Skript **in der Luft geparkt** (bei −3.600 bis
−20.000 Einheiten) und springen im Original erst auf Kommando durchs Fenster. Das
RE2-Gehirn ignorierte das Kommando komplett — die Hunde lebten, kaempften und starben auf
Parkhoehe. Jetzt laeuft der Skript-Sprung auch im RE2-Modus (der Drop ist Raum-Inszenierung,
die Kampf-KI bleibt RE2), und die Bodenhoehe kommt aus der Original-Formel statt aus der
Parkposition.

## Lobby: die Musik spielt wieder

Gemeldet: *„In der Lobby nach der cutscene mit den reinbrechenden Zombies fehlt noch die
Musik."*

Die Lobby-Musik ist ein „stumm laden, per Skript aufdrehen"-Titel. Der Port laedt die
Musikbank aber **verzoegert** (60 Bilder Ausblendung des alten Titels) — das Aufdreh-Kommando
des Raumskripts verpuffte auf der alten Bank, und die neue wurde stumm geladen. Der
Skript-Status wird jetzt gemerkt und nach dem Laden angewendet. Der Raum war damit nicht
nur „nach der Cutscene" still, sondern von Anfang an.

## Raum 1090: Flammen knistern, Feuer tut hoerbar weh

Gemeldet: *„In room 1090 fehlt uns der flammen Sound"* und *„die Flammen schadensanimation
und das schmerzgeraeusch fehlt."*

Drei Punkte: (1) Das Feuer-**Knistern** ist eine Musikspur, deren Lautstaerke in der Datei
auf **null** steht und die nur das Raumskript aufdreht — derselbe Verzoegerungs-Fehler wie in
der Lobby, gleiche Reparatur. (2) Das **Schmerzgeraeusch** beim Flammenkontakt fehlte im
Port komplett — es spielt jetzt an der Original-Stelle (zwei Varianten, je nach
Trefferrichtung). (3) Die **Schadensanimation** lief mechanisch bereits (gemessen: −2 LP pro
Kontakt-Bild, Ruckzuck-Animation ab Bild 6) — ihr Rueckstoss schob aber immer nach hinten;
bei Treffern von hinten schiebt das Original nach **vorn**. Korrigiert.

## Debug-Menue springt an die Original-Stellen

Gemeldet: *„das debug menu schickt mich teilweise an andere Stellen als das Original. so
zum Beispiel bei room105 dem Korridor."*

Das Original liest die Absprung-Position aus der **Debug-Tabelle des Spiels** selbst; der
Port nahm stattdessen den Spawn irgendeiner eingehenden Tuer. Bei room105 (EAST CORRIDOR)
lagen die beiden Punkte 25.850 Einheiten auseinander — das andere Korridor-Ende, mit
falscher Kamera. Jetzt springen alle Raeume auf die Original-Koordinaten (inklusive
Etagen-Hoehe), mit Kamera-Szenario 0 und unveraendertem Blickwinkel — exakt wie die PSX.

---

# RE1.5 Port — v0.3.16 (Early Preview)

## Neue Einstellung: AI MIXED

Auf Wunsch: *„Ich haette gerne eine weitere Option im Optionsmenue **RE AI Mixed**, die
beinhaltet im Prinzip alles von RE 1.5 AI ausser die Hunde, die sollen komplett von
RE 2 AI stammen."*

Der Menuepunkt **AI** hat jetzt drei Stufen: **RE1.5 / RE2 / MIXED**. Oben und Unten
schalten im Kreis durch — mit dem bisherigen Umschalter waere die dritte Zeile gar
nicht erreichbar gewesen.

Die eigentliche Arbeit lag unter der Oberflaeche: Der KI-Schalter war **global** und
entscheidet jetzt **pro Gegnertyp**. Alle 30 Stellen, an denen er abgefragt wird, wurden
einzeln durchgegangen. Eine davon rechnete zwei globale Zustaende vor — im Mischbetrieb
sind beide gleichzeitig wahr, sie haette also zwangslaeufig eine Haelfte verloren.

Nachgewiesen ueber eine Groesse, die nur das RE2-Hundeverhalten ueberhaupt schreibt: Im
Hunderaum verhaelt sich MIXED **identisch zu RE2**, im Zombieraum **identisch zu RE1.5** —
und der Hunderaum laedt tatsaechlich die RE2-Daten, waehrend der Zombieraum byte-identisch
zum RE1.5-Modus bleibt.

## Cutscene: ein Schritt zurueck statt zwei

Gemeldet: *„Im Original laeuft er hin und macht dann noch **einen** Schritt zurueck. Bei uns
macht er noch **zwei**."*

Der Fehler sass nicht im Laufen, sondern **danach**: Bei der Ankunft schaltet das Original
auf eine ruhige Stand-Animation um — im Port lief der Rueckwaerts-Clip einfach weiter. Der
Koerper stand, die Beine liefen.

Die Schrittzahl liess sich dabei nicht schaetzen, sondern **ablesen**: Der Clip markiert
seine Fussaufsetzer bei Bild 8 und Bild 25. Die neun Laufbilder enthalten genau **einen** —
das ist der Schritt aus dem Original. Die sechzehn Bilder Nachlauf brachten den zweiten.
Endposition und Blickrichtung sind unveraendert.

Dabei fiel ein zweiter Fehler auf: In diesem Zustand haben **zwei** Stellen die Animation
weitergeschaltet, also zwei Bilder pro Takt statt einem.

## Der Zombie frisst nicht mehr am Bein (RE2-Modus)

Gemeldet: *„Wenn Leon stirbt und von Zombies gefressen wird, ist der Zombie bei der
Finisher-Position irgendwie am Bein, nicht mehr wie sonst am Oberkoerper/Kopf."*

Fress-Animation und Leons Zusammenbrechen sind ein **aufeinander abgestimmtes Paar**, das
einen **gemeinsamen Bezugspunkt** braucht — und genau der wurde im Port nie gesetzt. Der
Zombie-Clip traegt ueber seine 123 Bilder eine Eigenbewegung von 1.669 Einheiten nach vorn:
Er soll sich ueber die Leiche schieben. Ohne Bezugspunkt blieb er stehen, wo der Todesstoss
ihn hinterliess, und Leon fiel aus einem veralteten Bezugspunkt zusammen.

Gemessen: vorher war der Zombiekopf in **91 %** der Bilder am naechsten an Leons Bein — jetzt
in **99 %** am Oberkoerper, und der Zombie legt exakt die 1.669 Einheiten zurueck, die in den
Animationsdaten stehen.

Es war derselbe Fehler wie beim Hund, nur schlimmer: Dort war der Bezugspunkt am falschen
Bild genommen, hier fehlte er ganz.

---

# RE1.5 Port — v0.3.15 (Early Preview)

## RE2-KI ist jetzt der Standard

Auf Wunsch: *„Ansonsten ist RE 2 AI mittlerweile so weit in Ordnung, dass ich das
gerne als Standard in den Optionen ausgewaehlt haette."* Das Spiel startet also
mit **AI RE2**; RE1.5 bleibt im Menue jederzeit waehlbar.

## Das Feuer im Hof war eine Baby-Spinne (RE2-Modus)

Gemeldet: *„Bei RE2-KI ist im Hof der Flammen-Effekt nicht da, stattdessen gibt es
komische Dreiecke, die da rumschwirren."*

Die interne Nummer dieses Objekts bedeutet in den beiden Spielen etwas
**voellig Verschiedenes**: in RE1.5 ist es der **Feuer-Erzeuger**, in RE2 eine
**Baby-Spinne**. Das RE2-Verhalten hatte die Nummer komplett beansprucht — die
sieben brennenden Truemmerstuecke im Hof bekamen dadurch ein Spinnen-Gehirn.

Deine „Dreiecke" waren also kein kaputtes Modell, sondern die korrekte
Truemmergeometrie, die vom falschen Gehirn durch den Hof gefahren wurde — noch
dazu mit RE2-Animationen auf einem Geruest, das nur einen einzigen Knochen hat.
Gemessen: im RE1.5-Modus 14 Flammen, im RE2-Modus **null**, und fuenf der sieben
Truemmer liefen bis zu 5.000 Einheiten weit davon.

Unterschieden wird jetzt nach **Herkunft** statt nach Nummer: Was der RE2-Spawner
erzeugt, bleibt Spinne; was im Raum steht, ist Feuer. Nach der Korrektur ist der
RE2-Durchlauf **pixelgleich** zum RE1.5-Durchlauf. Ein Zensus ueber alle
Gegnertypen zeigt: Das war der einzige Fall dieser Art.

## Auf der Kiste: kein Sprung mehr am Ende

Gemeldet: *„Ganz zum Schluss kommt noch mal eine Animation, wo Leon irgendwie von
oben auf die Kiste faellt."*

Beim Klettern lief die **Ueberblendung zwischen zwei Posen nie ab**. Sie ist so
gebaut, dass sie mit jedem Animationsschritt schwaecher wird — beim Klettern
wurde dieser Schritt aber uebersprungen, sodass dauerhaft 87 % der *vorherigen*
Pose im Bild blieben. Da diese Ueberblendung auch die Hoehe erfasst, folgte Leon
dem Anstieg nur zu einem Achtel pro Bild: Er stand rechnerisch **1.357 Einheiten
zu hoch** und sank danach elf Bilder lang ab. Genau das sah aus wie ein Fall von
oben.

## Kein Klettern mehr mitten im Kistenschieben

Gemeldet: *„Nach dem Verschieben und Draufklettern kam direkt einmal eine ganz
andere, falsche Animation."*

Der Klettervorgang liess sich **waehrend** des Schiebens starten. Im Original ist
das unmoeglich — die Kletterpruefung wird dort aus vier Bewegungszustaenden
gerufen, das Schieben gehoert nicht dazu, und beide benutzen sogar dasselbe
Speicherbyte. Im Port lief dadurch die Animationsauswahl des Schiebens auf die
Kletter-Nummern und griff die falschen Clips.

## Der Zombie laeuft nicht mehr auf der Stelle (RE2-Modus)

Gemeldet: *„Er wechselt kurz zu einer Animation mit gehobenen Armen und laeuft
dann — immer wenn diese Animation kommt, laeuft er auf der Stelle."*

Der Zombie hat einen **zweiten Gang**, und im Port fehlte darin das Anwenden der
Bewegung: Der Zustand lief, die Animation lief, nur der Koerper blieb stehen.
Die „gehobenen Arme" sind uebrigens nachgemessen — die Haende liegen in diesem
Gang rund 450 Einheiten hoeher als im normalen.

Gemessen: vorher 59 Bilder ohne einen Schritt, jetzt 867 Einheiten in 27 Bildern,
mit Uebergang in den Griff.

## Cutscene: Leon rennt nicht mehr rueckwaerts

Gemeldet: *„Zum Schluss rennt Leon noch ein bisschen komisch, fast auf der
Stelle."*

Das Tempo war korrekt — die **Animation** nicht. Leon geht dort rueckwaerts, und
dafuer gibt es einen eigenen Clip mit **34 Bildern**. Der Port spielte den
**Renn**-Clip aus dem Waffen-Archiv mit **22 Bildern**: Bei gleicher
Laufgeschwindigkeit takten die Beine dadurch **1,55-mal zu schnell**, was genau
wie Rutschen auf der Stelle aussieht. Jetzt legt er pro Beinzyklus 2.231 statt
1.444 Einheiten zurueck.

---

# RE1.5 Port — v0.3.14 (Early Preview)

## Diagnose-Starter fuer den ROOM1090-Cutscene-Fehler

Im Windows-Paket liegt neu **`Diagnose_ROOM1090.bat`**. Doppelklicken, ganz
normal bis in den Hof spielen, den Fehler auftreten lassen, Spiel beenden — und
die entstandene `debug_room1090.log` schicken. Das Spiel laeuft dabei voellig
normal, es wird nur zusaetzlich mitgeschrieben.

Hintergrund: Der gemeldete Fehler („Leon verschwindet am Ende der Cutscene")
liess sich ueber den beschriebenen Weg in vielen Messlaeufen **nicht** ausloesen.
Zwei Zeilen aus diesem Protokoll entscheiden die Sache — ob die Szene ein- oder
zweimal startet, und welche Blickrichtung Leon im Moment des Umschaltens hat.

## Eine falsche Korrektur zurueckgenommen

In der letzten Fassung hatte ich Leon **immer** zeichnen lassen, in der Annahme,
der Sichtbarkeitstest waehle im Original nur zwischen beleuchtet und
unbeleuchtet. Das ist widerlegt: Der Zweig fuer „ausserhalb" enthaelt gar keinen
Zeichenaufruf, und die Funktion, die Geometrie einreiht, wird von dort nie
gerufen. Zwei unabhaengige Gegenproben stuetzen es — RE2 hat an derselben Stelle
nicht einmal einen Gegenzweig, und ein Kameraanker in einem anderen Raum umspannt
die gesamte Spielwelt, was nur als Wegschneide-Steuerung Sinn ergibt.

Die Korrektur ist damit raus. Sie hat den gemeldeten Fehler ohnehin nicht
behoben: Auf dem kompletten Durchlauf hat der Test in **0 von 3.057 Bildern**
zugeschlagen.

## Was dabei ueberprueft und fuer richtig befunden wurde

- **Gegner und NPCs** werden ausserhalb des Kamerabereichs korrekt weggelassen —
  hier waere eine Aenderung ein Fehler gewesen.
- **In der Tuernische** ist Leon auch im Original nicht sichtbar; der
  Kamerabereich endet dort.
- Die **Steuerung ist waehrend der Szene lueckenlos gesperrt** — in 1.829 Bildern
  genau eine Blickrichtungs-Aenderung, und die kommt vom Skript selbst.
- Die Szene kann **nicht doppelt starten**: Die Sperre dagegen faellt im selben
  Bild wie der Start.

---

# RE1.5 Port — v0.3.13 (Early Preview)

## Auf der Kiste bleibt man jetzt auch oben

Gemeldet: *„Ich kann auf die Box klettern, bin dann oben aber nicht auf die Box
beschraenkt — ich kann in der Luft rumlaufen. Und ich komme nicht mehr runter."*

Beides war **derselbe** Fehler: **Leon wurde aus der Kiste hinausgeschoben, auf
der er stand.** Das Original behandelt das Objekt, auf dem man steht, gesondert
— statt der normalen Ausschiebung, die jeden aus einer Kiste herausdrueckt,
laeuft dort eine Eingrenzung, die einen oben auf der Flaeche haelt. Der Port
fuhr auch fuer die eigene Standkiste die Ausschiebung.

Gemessen: In dem Bild, in dem die Kletteranimation Leon freigibt, springt er
**863 Einheiten** zur Seite — genau Kistenbreite plus eigener Radius — behaelt
aber die Hoehe des Kistendeckels. Das war das Laufen in der Luft.

Und der Abstieg war nie kaputt: Mit dem Hinausschieben verliert Leon den Bezug
zum Standobjekt, und genau den verlangt die Abwaerts-Pruefung. Sie wurde also nie
gefragt. Jetzt loest die Aktionstaste oben den Abstieg aus, und Leon steigt ueber
vier Zwischenhoehen sauber wieder herunter.

*Zur „hoeher clippenden" Animation:* dort liess sich keine eigene Abweichung
finden — Hoehenversatz und Zeichenversatz sind beide byte-genau, sichtbare
Oberkante und Kollisionsoberkante liegen auf demselben Wert. Sehr wahrscheinlich
war auch das der Auswurf: Leon stand am Ende der Animation auf Kistenhoehe, aber
**neben** der Kiste. Faellt es weiter auf, bitte nochmal melden.

## Wegstossen nach einem Biss (RE2-Modus)

Beim Losreissen wurde Leons Blickrichtung nie gedreht. Das Original schnappt sie
beim Zupacken auf den Zombie — und merkt sich zusaetzlich eine halbe Drehung,
wenn von hinten gegriffen wurde. Weil der Port den Winkel unangetastet liess,
wurde die Bewegung des Losreiss-Clips mit Leons altem **Laufwinkel** gedreht und
schob ihn rund 550 Einheiten in eine beliebige Richtung. Zusaetzlich fuhr er
dabei eine 180-Grad-Drehung aus dem RE1.5-Zweig mit, die hier gar nicht gilt.

## Der Sturz zu Boden fror kurz ein (RE2-Modus)

Gemeldet: *„Wenn der Zombie zu Boden Richtung Leon springt, stimmt die Animation
nicht ganz."*

Die Ueberblendrate war doppelt so hoch wie im Original. Wie stark die vorige Pose
noch durchschlaegt, ergibt sich aus einem Zaehler mal dieser Rate — mit dem
echten Wert erreicht das Produkt **nie** die Obergrenze. Mit dem doppelten schon,
und dann steht acht Bilder lang zu hundert Prozent die alte Pose im Bild. Genau
dieses Plateau vor dem Sturz war zu sehen.

## Kriechende Zombies koennen wieder angreifen (RE2-Modus)

Das Spiel laesst immer nur **einen** Gegner gleichzeitig zupacken und setzt dafuer
einen Riegel. Die Stellen, die ihn setzen, waren portiert — alle drei Stellen, die
ihn wieder loesen, lagen auf einem anderen Feld. Nach dem ersten Griff im Raum
blieb er also fuer immer geschlossen.

Gemessen: Ein Kriecher lag ab Bild 299 dauerhaft in Reichweite und griff **nie**
an. Jetzt zehn Griffe statt zwei.

---

# RE1.5 Port — v0.3.12 (Early Preview)

## Der fette Zombie faellt nicht mehr jeden zweiten Schuss

Gemeldet: *„Der fette Zombie muss nicht jeden 2. Schuss umfallen. Das ist
unschoen."*

Das war tatsaechlich RE1.5-Original — und der Grund fuer die gefuehlte Zaehigkeit
ist nicht seine Panzerung. **Die Trefferzahl bis zum Tod ist in beiden
KI-Modi gleich.** Was den Unterschied macht, ist eine Endlosschleife: RE1.5 gibt
jedem Zombie eine Standfestigkeit von 4 bis 7, jeder Pistolentreffer zieht 3 ab
— und beim Aufstehen wird sie **auf 4 neu geladen**. Ab dem ersten Umfallen also
zwangslaeufig alle zwei Treffer, immer wieder.

Wie beim Schadensmodell wird die Regel deshalb **aus RE2 uebernommen** statt eine
Zahl nach Gefuehl zu setzen. Und RE2 macht es interessanterweise voellig anders:
Dort kostet jeder Treffer waffenabhaengig von einer Widerstandsleiste — aber
**solange der Zombie mindestens 81 Lebenspunkte hat, wird sie sofort wieder
aufgefuellt**. Ein RE2-Zombie faellt erst im Endspiel um.

Fuer den fetten Zombie heisst das: Sturz alle **sechs** statt alle 2,3 Treffer,
und die Dauer halbiert sich von rund **124 auf 51 Sekunden** — bei unveraenderter
Trefferzahl und unveraenderten Lebenspunkten.

Die beiden normalen Zombies aendern sich dabei fast nicht, und das ist stimmig:
Ihre Lebenspunkte liegen von vornherein unter der RE2-Schwelle, sie sind nach
dieser Regel also von Anfang an im Endspiel.

Wie der Schadensimport haengt das an demselben Schalter und ist damit gemeinsam
abschaltbar; ohne ihn kommt die alte RE1.5-Kadenz exakt zurueck.

---

# RE1.5 Port — v0.3.11 (Early Preview)

## Beim Generator-Raetsel lief Leon mit

Gemeldet: „Nach dem Generator-Raetsel stand man nicht mehr vor dem Generator,
sondern weiter weg davon. Bewegt man sich vielleicht auch **physisch** innerhalb
des Raetsels?"

**Genau das war es** — und es erklaert auch, warum sich die Kamera danach
manchmal nicht mehr aenderte.

Das Skript setzt beim Raetselstart zwei Schalter. Der Port legte sie in ein
Feld fuer Skript-Merker. Im Original loest dieselbe Anweisung sie aber ueber
eine Zeigertabelle auf, und der Eintrag fuer diese Gruppe zeigt gar nicht auf
einen Merker, sondern direkt auf das **Pause-Wort der Engine**. Die beiden
Schalter bedeuten also: *Spieler einfrieren* und *Gegner einfrieren*.

Deshalb setzt das Raetsel auch nur diese zwei und nicht mehr: Tastatur und
Skript muessen weiterlaufen, sonst liesse sich der Cursor nicht bedienen. Im
Port war Leon dadurch nie eingefroren — dieselben Richtungseingaben, die den
Cursor steuern, liefen zusaetzlich in seine Beine. Gemessen wanderte er bis zu
**3.900 Einheiten** vom Panel weg und drehte sich frei.

Und weil ein verpasster Kamerazonen-Uebergang endgueltig ist, blieb das Bild
danach stehen — je nachdem, wohin man beim Raetseln gelaufen war. Daher
„teilweise".

Im echten Spiel gegengeprueft: mit der Korrektur steht Leon bei allen
Richtungseingaben **ohne einen einzigen Einheiten-Versatz** am Panel.

## Kamera blieb beim Klettern und auf Treppen haengen

Sobald ein Klettervorgang oder eine Treppe begann, uebersprang der Port den
**gesamten** Zonen-Suchlauf der Kamera. Im Original wird dabei nur die
Aktionstasten-Pruefung unterdrueckt — der Zonen-Suchlauf laeuft in jedem Bild.
Da ein verpasster Uebergang endgueltig ist, konnte die Kamera dadurch dauerhaft
stehenbleiben. Betraf das neue Klettern und alle Treppenraeume.

## Zwei Kamera-Werte wurden verwechselt

Die letzte Fassung hatte **angeforderte** und **angezeigte** Kameraeinstellung
getrennt. Dabei blieben zwei Stellen an der Anzeige haengen, die im Original die
Anforderung lesen — vorher unsichtbar, weil beide Werte identisch waren. Der
Zonen-Suchlauf durchsuchte dadurch die Gruppe der **alten** Einstellung, solange
die Anzeige noch nicht nachgezogen hatte, und das Skript merkte sich beim
Umschalten den falschen Wert.

Abgesichert ist das jetzt durch einen Nachbau des Original-Suchlaufs, der ueber
**21 Raeume mit 56,7 Millionen Vergleichspunkten** gegen den Port prueft — ohne
eine einzige Abweichung.

---

# RE1.5 Port — v0.3.10 (Early Preview)

## Zuerst: v0.3.9 enthielt unter Windows die Aenderungen NICHT

Gemeldet war, dass vier bereits behobene Fehler weiterhin auftreten — Leons
Drehung in der Cutscene, das Klettern, das abrupte Aufstehen und die sofort
volle Blutlache.

**Der Grund lag nicht im Spiel, sondern im Paket.** Das Skript, das die Pakete
schnuert, **baut nicht** — es kopiert ein fertiges Programm aus einem
Ablageordner. Der Windows-Build lief in ein anderes Verzeichnis, im Ablageordner
lag noch die Fassung von v0.3.8. Nachweisbar bis auf die Pruefsumme: Das
ausgelieferte Programm war mit dem alten **bitgleich**, und der Kletter-Code kam
darin **null mal** vor. Das Linux-Paket war korrekt.

Alle vier Aenderungen wurden anschliessend am richtigen Programm im echten
Spielablauf nachgemessen und wirken. In dieser Fassung sind sie enthalten.

Damit das nicht wiederkehrt, prueft das Paketskript jetzt das **Alter** des
Programms gegen die letzte Code-Aenderung und bricht ab, wenn es aelter ist —
gegen den alten Stand getestet, es bricht ab.

## Zombies waren nach einem Treffer unverwundbar (RE2-Modus)

Gemeldet: „Ich kann teilweise immer noch nicht die Zombies treffen."

Das Original merkt sich pro Gegner einen **Ein-Treffer-Riegel**, damit ein
einzelner Schuss nicht mehrfach zaehlt — und loest ihn in der Trefferpruefung
selbst wieder, **unabhaengig davon, ob der Gegner gerade denkt**. Der Port loeste
ihn im Denk-Takt des Gegners. Der wird aber uebersprungen, solange die KI
eingefroren ist oder ein bestimmtes Zustandsbit steht. Genau dann bleibt der
Riegel haengen: Der erste Treffer setzt ihn, niemand gibt ihn frei, der Zombie
ist unverwundbar.

Ein Zensus ueber alle 37 Raeume der ersten Etage zeigt den betroffenen Fall in
ROOM1200. Die Freigabe haengt jetzt nicht mehr am Takt.

## Beim Tod durch den Hund verschwand Leon

Zweimal derselbe Fehler — der **Ankerpunkt** des Griffs. Erst startete der Griff
die Opfer-Sequenz ohne Anker, was Leon exakt in den Nullpunkt der Welt setzte.
Und dann wurde der Anker auf dem **ersten** Bild der Animation genommen, die
Platzierung aber auf dem **aktuellen** — die Differenz summiert sich mit jedem
Bild auf.

Gemessen war Leon am Ende **389.271 Einheiten** vom Startpunkt entfernt. Jetzt
sind es 2.508, und er landet einen Schritt neben dem Hund. Im Original heben sich
Anker und Pose exakt auf, weil beide denselben Bildindex lesen.

## Tote Kraehen hinterlassen kein graues Feld mehr (RE2-Modus)

Gemeldet: „Platzende Kraehen hinterlassen immer noch Schatten."

Der Schatten ist im Original ein **Platz in einem Pool** von 50 Eintraegen, und
beim Zerplatzen wird dieser Platz **freigegeben** — danach wird dort nichts mehr
gezeichnet. Der Port setzte statt dessen nur die Groesse auf null, und der
Renderer las das als „keine Groesse angegeben" und zeichnete seinen
**Standardschatten**. Genau deshalb blieb der Fleck liegen.

Dabei kam ein zweiter Fehler heraus: Die Blutlache der Kraehe war im Port als
blosse Modellfaerbung eingetragen und damit wirkungslos. Tatsaechlich faerbt das
Original **denselben Schatteneintrag** um — mit derselben Funktion und derselben
Farbe wie beim Zombie. Aus dem grauen Fleck wird also die Lache. Im echten
Fenster nachgeprueft: dunkelrote Blutlache statt grauem Feld.

## Nachgeprueft und unveraendert gelassen

Vier weitere Meldungen haben sich als **Original-Verhalten** erwiesen; hier waere
eine Aenderung ein Fehler gewesen:

- **Zielen hindert die Zombies nicht am Angriff.** Ueber 196 Messreihen: im Stehen
  196 Angriffe, beim Zielen 194. Alle Angriffe sind Griffe, und die haben gar kein
  Bewegungs-Kriterium.
- **Der Sprungangriff des Hundes trifft** — im Stehen, Gehen, Rueckwaerts und beim
  Zielen 36 von 36 Versuchen. Nur beim Rennen verfehlt er gelegentlich, weil man
  ihm davonlaeuft.
- **Der Hundebiss spritzt kein Blut.** Er addiert 50 auf einen Wund-Zaehler des
  getroffenen Koerperteils, und erst ab 120 wird die Wunde sichtbar. Weil das
  Koerperteil jedes Mal neu gewuerfelt wird, dauert es im Schnitt vier bis fuenf
  Bisse. Biss eins und zwei zeigen korrekterweise nichts.
- **Das Umfallen alle zwei Schuesse** ist eine Standfestigkeits-Leiste: beim Spawn
  zufaellig 4 bis 7, jeder Pistolentreffer zieht 3 ab, beim Aufstehen wird auf 4
  neu geladen. Ab dem ersten Sturz faellt er also zwangslaeufig alle zwei Treffer.
  Die Trefferzahl bis zum Tod ist in beiden Modi uebrigens **gleich** — die
  gefuehlte Zaehigkeit kommt allein aus dieser Sturzschleife.

## Die Einstellung "AI RE2 MODELS" ist raus

Auf Wunsch: *„Mittlerweile ist RE 2 AI schon so gut, dass wir diese Option im
Optionsmenue sowie seine eigenen Models entfernen koennen. Wir konzentrieren uns
jetzt nur noch auf RE 1.5 AI und RE 2 AI."*

Der Menuepunkt **AI** hat damit wieder genau zwei Stufen — **RE1.5** und **RE2**.
Oben/Unten schaltet wie auf der SOUND-Seite einfach um; es gibt keinen dritten
Eintrag mehr, durch den man sich hindurchtippen muesste.

**AI RE2** ist unveraendert die Stufe von vorher: RE2-Verhalten, RE2-Animationen,
Modelle aus RE1.5. Wer die Einstellung vorher auf der dritten Stufe hatte, landet
jetzt auf **AI RE2** — der Wert wird nirgends gespeichert, es kann also auch kein
Spielstand mit der alten Einstellung auftauchen.

Die RE2-Dateien im Paket (`shared_assets/RE2/`) werden weiterhin gebraucht: aus
ihnen kommen die RE2-Skelette, -Animationen und -Sounds, die "AI RE2" fahrt. Nur
der Zweig, der zusaetzlich die RE2-*Modelle* geladen hat, ist entfallen.

## Tote Kraehen hinterlassen keinen Schatten mehr (RE2-Modus)

Gemeldet: „Platzende Kraehen durch die Pistole hinterlassen immer noch Schatten,
die muessen dann natuerlich weg."

Der Boden-Schatten ist im Original kein Anhaengsel des Gegners, sondern ein
**Platz in einer festen Liste** von 50 Stueck. Eine zerplatzende Kraehe gibt
ihren Platz zurueck — ein einziges Byte, das die Liste als „frei" markiert.
Danach gibt es schlicht nichts mehr zu zeichnen.

Der Port hat diese Rueckgabe zwar nachgebildet, der Zeichner hat sie aber als
„Groesse unbekannt" gelesen und dafuer seinen **Standardschatten** genommen. So
blieb unter der geplatzten Kraehe genau der Fleck liegen, der eigentlich
verschwunden sein sollte.

Beim Aufraeumen kam ein zweiter Fehler mit heraus: Die *normale* Kraehenleiche
faerbt im Original denselben Platz zur **Blutlache** um. Diese Umfaerbung war im
Port als „nur Optik, spaeter" abgehakt — sie ist es nicht, es ist dieselbe
Funktion und dieselbe Farbe, mit der auch die RE2-Zombieleiche ihre Lache faerbt.
Bis jetzt lag unter jeder toten Kraehe im RE2-Modus ein grauer Fleck statt einer
Lache. Jetzt waechst die Lache wie im Original ueber 60 Bilder von 400 auf 1000.

Die RE1.5-Kraehe ist davon nicht betroffen und bleibt unveraendert.


## Auf die Kiste klettern (Aussenbereich)

Gemeldet: „Man kann noch nicht auf die Kiste draufklettern, was normalerweise
geht." Stimmt — und der Grund, warum es sich im Original so selbstverstaendlich
anfuehlt, steckt in der Reihenfolge: Die Aktionstaste probiert **zuerst das
Klettern und erst danach die Tuer**, und zwar in allen vier Bewegungszustaenden
identisch. Der Port hatte diesen ersten Schritt gar nicht.

Das Original schaut dafuer 800 Einheiten voraus, rastet den Kurs auf die vier
Himmelsrichtungen und prueft getrennt, ob es dort **hoch** oder **runter** geht.
Beides ist jetzt da: Aufstieg wie Abstieg.

Gemessen: Leon steigt exakt im 30. Bild der Animation um 1.800 Einheiten nach
oben und 1.148 nach vorn und landet auf dem Kistendeckel. Der Abstieg fuehrt in
drei von vier Richtungen bewusst ins Leere — die eigene Kiste blockiert — und
funktioniert genau dort, wo im Original auch.

Wichtig war die Gegenprobe: Vor einer Tuer faellt die Kletterpruefung in allen
vier Richtungen sauber durch, die Tuer geht also weiterhin auf.

## Der tote Zombie blutet jetzt langsam aus (RE2-Modus)

Gemeldet: „der am Boden getoetete Zombie laeuft sofort aus, nicht langsam wie
normal. Ausserdem gibt es keine finale Todesanimation."

Beides hatte **dieselbe Ursache**: Die Leiche ist im Original eine Maschine mit
**zwoelf Zustaenden** — der Port fuhr genau einen davon, die Initialisierung, und
hielt danach an. Die uebrigen elf waren als reine Darstellung abgetan. Sie sind
es nicht: In ihnen stecken das Wachsen der Blutlache, das Nachzucken und die
Ruhephase.

Die Lache war sofort voll, weil der Port ihren Fortschritt aus einem Zaehler las,
den eine Zombie-Leiche im RE2-Modus nie beschreibt — der stand dauerhaft auf
null, was die Formel als „fertig gewachsen" liest. Im Original waechst sie ueber
120 Bilder.

Die fehlende Todesanimation ist die Kehrseite: Der zentrale Animationsschritt
laesst Leichen bewusst aus, weil die Leichen-Maschine ihre Animation **selbst**
weitertreibt. Da der Port die Maschine nicht fuhr, hing der Zombie fuer immer im
ersten Bild seiner Endpose.

Gemessen ueber 64 Durchlaeufe: vorher in **allen** die Lache sofort voll und in
keinem einzigen wachsend, der Zombie ueber 900 Bilder regungslos. Jetzt waechst
sie in allen Laeufen und der Zombie durchlaeuft sieben Phasen statt zwei.

## Der Hund toetet wieder (RE2-Modus)

Der Hund kam nie zum toedlichen Griff. Grund: Der Port mass den Abstand zum
**Rumpf** des Hundes, das Original misst zum **Kiefer** — und der liegt gut
tausend Einheiten weiter vorn.

Verschaerft wurde es dadurch, dass der Zubeiss-Radius **schrumpft**, sobald der
Spieler unter 21 Lebenspunkte faellt. Genau ab da war ein Treffer mit der
Rumpf-Distanz praktisch unerreichbar: Die Lebenspunkte froren bei 20 ein, der
Kampf stand still. Vorher vier Bisse und dauerhaft 20 Leben — jetzt sechs Bisse
bis in den Tod, mit Griff und Fressen.

Dass der Hund abbricht, wenn Leon **abgewandt** steht, ist dagegen korrekt: Das
Original unterdrueckt den Griff dann bewusst und laesst ihn am Boden zubeissen.
Toedlich ist das trotzdem.

## Die Sterbeanimation friert nicht mehr ein (RE2-Modus)

Gemeldet: „die Sterbeanimation ist nicht vollstaendig, friert ein vor dem
kompletten Tod."

Der RE2-Animationsmotor kennt kein „auf dem letzten Bild stehenbleiben" — er
springt ans Ende zurueck auf Bild 0 und meldet das nur nach oben. Ob die
Animation aufhoert, entscheidet die aufrufende Stelle, und beim Sterben
ignorieren mehrere diese Meldung bewusst, weil dort ein **Zeitgeber** das Ende
bestimmt. Der Port hatte statt dessen „halte das letzte Bild" eingebaut.

Gemessen: In 14 von 64 Durchlaeufen stand der Zombie bis zu 22 Bilder lang
bewegungslos aufrecht. Jetzt in keinem einzigen.

## Kein Aufspringen mehr nach dem Generator-Raetsel

Gemeldet: „der Zombie steht direkt neben Leon abrupt quasi ohne Animation direkt
vom Liegen zu stehen."

Der Zustand, den der Port als Aufsteh-Animation gefuehrt hat, ist keine: Es ist
der **Taumel-Zustand**, und die drei Animationen, die er waehlen kann, beginnen
alle **im Stehen**. Der Zombie wurde also nicht hochgestellt — er begann einfach
stehend. Die echte Aufsteh-Kette haengt woanders.

Gemessen: Vorher sprang die Brust in **einem einzigen Bild** um bis zu 1.854
Einheiten nach oben, in 128 von 128 Faellen. Jetzt liegt der groesste Sprung bei
394, und das Aufstehen dauert konstant 30 Bilder.

## Leon schaut in der Cutscene wieder zum Transporter

Gemeldet: „In der Cutscene laeuft Leon vor und dreht sich nach rechts um. Im
Original schaut er nach links zum Transporter."

Der **Weg** war korrekt — es ging nur um die Blickrichtung. Das Original kennt
fuer Skript-Bewegungen **Rueckwaerts-Modi**: Der Weg zum Ziel bleibt derselbe,
die Blickrichtung ist exakt gegenlaeufig. Der Port kannte nur vorwaerts und liess
Leon immer in die Laufrichtung schauen — 180 Grad daneben.

Gegenprobe: Das Raum-Objekt, auf das er schauen soll, liegt genau in der neuen
Blickrichtung; die alte zeigte davon weg.

---

# RE1.5 Port — v0.3.8 (Early Preview)

## Transparenz: endlich die Regel statt einer Liste

Gemeldet war „die Flammen haben wieder schwarzen Hintergrund — das hatten wir
schon 40 mal, da muss es etwas Globales geben". Das war genau richtig.

Der Port entschied **pro Textur-Speicherplatz anhand einer Liste**, ob schwarze
Bildpunkte durchsichtig sind — Standardwert: nicht durchsichtig. Jede neue Grafik
fiel also auf schwarz und musste von Hand eingetragen werden. Auf der PSX
entscheidet das die Grafikeinheit an jedem einzelnen Bildpunkt: **Farbwert 0000
heisst durchsichtig, ausnahmslos.**

Der Grund, warum es diese Liste gab — die Loecher in den Gegnermodellen von vor
ein paar Wochen — ist widerlegt: Die korrekte Regel loescht bei Leon, Elza und
22 von 26 Gegnermodellen **exakt null** Bildpunkte. Die Liste war ein Pflaster
gegen einen anderen, ebenfalls falschen Test.

Gemessen im Bild: Der Flammenbereich war zu **72 % schwarz**, jetzt zu 18 % —
und der Rest ist der dunkle Raum selbst. Ueber 120 Raeume und 249 Effektgrafiken
geprueft, alle korrekt, **ohne einen einzigen Listeneintrag**. Rutscht kuenftig
doch etwas durch, meldet der Port eine Warnung statt still schwarz zu zeichnen.

## Zombies fallen jetzt wirklich (RE2-Modus)

Gemeldet: „fallen teilweise immer noch komisch hin, oder haben manchmal eine
kurze Hinfall-Animation, stehen dann aber sofort wieder."

**Sie sind nie gefallen.** Der Port holte bei einem Treffer dieselbe
Animationsnummer aus der falschen Bank — und dort ist sie die 54-Bilder-**Geh**-
animation statt des 60-Bilder-**Sturzes**. Was aussah wie eine abgebrochene
Fallanimation, war ein Zombie, der kurz geht und dann weitermacht.

Ueber 512 Durchlaeufe gemessen: vorher 136 solche Vorfaelle, kuerzestes "Liegen"
**ein Bild** — nachher 8 Vorfaelle, kuerzestes Liegen 101 Bilder.

## Leon verschwindet nicht mehr beim Hundeangriff

**Er wurde teleportiert.** Der Hunde-Griff setzt im Original zwei Ankerpunkte,
der Port nur einen. Im Bild nach dem Zupacken stand Leon **9.628 Einheiten weit
weg** in der Raumecke, waehrend der Hund ins Leere biss.

Das fehlende Blut war eine Folge davon — es haengt an Leon und flog mit ihm weg.
Zusaetzlich wurde sein Ankerpunkt mit dem **Skelett des Hundes** statt mit Leons
eigenem gerechnet, nochmal 1.457 Einheiten daneben.

Dass der normale Hundebiss **kein** Blut spritzt, ist uebrigens korrekt — im
Original setzt er nur eine Wunde. Das war schon richtig.

## Kamera bleibt nicht mehr stehen (Generator-Raetsel)

Im Port waren zwei Felder vertauscht: die **angeforderte** und die **angezeigte**
Kameraeinstellung. Weil beide im selben Durchlauf gleichgesetzt wurden, war der
Vergleich, mit dem sich das Original in jedem Bild selbst korrigiert,
**strukturell tot**. Uebrig blieb ein einmaliges Signal — ging es verloren, stand
das Bild dauerhaft.

Dabei kam ein zweiter Fehler heraus: Beim Betreten eines Raums wurde nur eines
der beiden Felder gesetzt, obwohl das Original beide aus demselben Register
schreibt.

## Ausserdem

**Leon spielt beim Kistenschieben die richtige Animation.** Vorher lief eine
fremde Geste aus dem Raum-Datensatz — alle 27 Bilder der Sequenz hatten die
falsche Laenge.

**Zerreiss-Tode fuer schwere Waffen.** Mit der **Magnum platzt der Kopf ab** (und
in der Haelfte der Faelle laeuft der Kopflose noch bis zu 90 Bilder weiter),
Schrotflinte, Granate und Rakete **zerreissen** den Zombie.

**Gegner weichen an Waenden aus.** Ein Signal dafuer wurde im Port nie erzeugt,
obwohl 14 Stellen es auslesen. Der Gorilla baeumte sich in 237 von 240 Faellen
an der Wand auf, weil er sie nie bemerkt hat.

Testsuite 204 Pruefungen.

---

# RE1.5 Port — v0.3.7 (Early Preview)

## Alle sieben Feuer brennen (ROOM1090)

In v0.3.6 waren drei der sieben brennenden Truemmer noch dunkel. Die Ursache war
nicht eine fehlende Grafik, sondern die **falsche**: Der Port lud fuer diese drei
die **Blutgrafik**. Von den zehn Bildern, die der Feuereffekt dort abruft, sind
darauf neun vollstaendig leer — deshalb sah es aus, als fehle der Effekt.

Die richtige Grafik steckt in einer normalen Spieldatei; ein Emulator war nicht
noetig. Dabei ist eine Notiz im Projekt aufgeflogen, die behauptete, diese Datei
sei umsortiert und lasse sich nicht direkt ausschneiden — sie ist es nicht,
**alle 32768 Halbwoerter stimmen ueberein**.

Huebsche Gegenprobe: Dieselbe Grafik mit einer anderen Farbtabelle gelesen ergibt
exakt dieselben Formen in **Grau** — das ist der Raucheffekt des Spiels.

## Gegner weichen an Waenden aus

Der Port hat ein Feld, an dem der **Ausweich-Zweig** der Gegner-Navigation
haengt, ueberhaupt nie beschrieben. Vierzehn Stellen in drei Gegner-Gehirnen
lesen es; wirksam ist es in **zwanzig Raeumen**. Laeuft ein Zombie dort jetzt
gegen eine entsprechend markierte Wand, dreht er ab, statt dagegen zu laufen.

**Dabei kam ein zweiter Fehler ans Licht:** Der Port fuehrte die Wandklemme nur
aus, wenn sich ein Gegner bewegt hatte — das Original tut es **immer**. Ein
stehender Gegner, der in einer Wand steckt, wird dort herausgeschoben; bei uns
blieb er stecken.

⚠️ Nebeneffekt, der byte-true ist: Der Hund weicht in ROOM1190/1230 jetzt
**nicht mehr** aus. Der bisherige Ersatz im Port liess ihn bei jeder
Wandberuehrung ausweichen — im Original passiert das dort nie.

## Zwei gemeldete Punkte sind KEIN Fehler

Beide in der Lobby (ROOM1030), beide gemessen und byte-true belegt:

- **"Nur fuenf von sechs kriechen unter das Tor."** Das Skript schaltet Zombies
  nur scharf, solange **weniger als vier** in der Zone stehen. Wer als Fuenfter
  ankommt, wird nie mehr aktiviert.
- **"Manchmal laufen sie, ohne voranzukommen."** Die Wegfindung setzt den
  Durchgangspunkt genau **29 Einheiten innerhalb der Wand**. Der Zombie laeuft
  dagegen, wird zurueckgeschoben, laeuft wieder — ein Kreislauf mit einer Periode
  von exakt 100 Bildern. Auch das Original weicht dort nicht aus, weil die
  noetige Markierung in allen 35 Bodenzellen dieses Raums fehlt.
  Gegenprobe: Setzt man denselben Zombie ein Stueck oestlich ab, kommen
  **sechs von sechs** durch.

Testsuite 196 Pruefungen.

---

# RE1.5 Port — v0.3.6 (Early Preview)

## Der Hinterhof brennt jetzt (ROOM1090)

Der gemeldete fehlende Feuereffekt ist gefunden — und der Befund war groesser
als der Fehler. **Die sieben "Baby-Spinnen" in diesem Raum sind sieben
brennende Truemmer.** Der Gegnertyp, den das Projekt seit Monaten als
"Spinnenbaby, stationaerer Netzspucker" fuehrte, ist in Wahrheit der
Feuer-Emitter des Hinterhofs mit dem verunglueckten Polizeitransporter.

Belegt an den Daten: die Effektgrafik zeigt, mit ihrer eigenen Farbtabelle
gerendert, **acht Flammenbilder**. Und das Hintergrundbild des Raums hat
sechzehn statt acht Ansichten — dieselben acht Kamerawinkel noch einmal im
orangen Feuerschein. Die "Netz-Sprites" sind Flammen, der Beruehrungsschaden
ist Verbrennen.

Warum es so lange unentdeckt blieb: Die Effektnummer steht in einer Befehlsform,
die genau **einmal im ganzen Spiel** vorkommt. Erst ein Durchgang durch alle 702
Aufrufstellen der Effekt-Funktionen hat sie sichtbar gemacht.

⚠️ Drei der sieben Feuer holen ihre Grafik aus einer globalen Bank, fuer die der
Port noch keinen Platz im Bildspeicher hat — die bleiben vorerst dunkel.

## Kisten lassen sich schieben (ROOM1090)

Ursache war ein Reihenfolgefehler: Der Port hat den Spieler **zuerst** aus der
Kiste herausgeschoben und **danach** geprueft, ob er hineindrueckt. Ein buendig
ausgeschobener Spieler dringt aber nie ein — das Schieben war damit rechnerisch
immer wirkungslos, die Kiste konnte sich **nie** bewegen.

Der Schub kommt uebrigens nicht aus einer Geschwindigkeit, sondern aus der
Animation selbst: Die Schiebe-Animation traegt in den ersten 15 von 25 Bildern
eine Markierung, ueber die die Figur 523 Einheiten nach vorn wandert.

## Der Cursor im Generator-Raetsel ist durchsichtig (ROOM11F0)

Die Cursor-Grafik besteht zu 93 % aus Bildpunkten, die auf der PSX "nicht
zeichnen" bedeuten. Der Port hat sie **schwarz ausgemalt**. Von den elf anderen
Objekten im Raum benutzt keines diese Farbe — deshalb fiel es nur dort auf.

## Zombies kriechen unter das Tor (ROOM1030, RE2-Modus)

Dafuer wurde das **zweite Gegner-Gehirn** von RE2 nachgebaut — die
Kriech-Zustandsmaschine, die der Port bisher gar nicht hatte, samt der Bruecke
zwischen RE1.5-Skript und RE2-Verhalten. Gemessen kriechen jetzt alle
geflaggten Zombies unter dem Rolltor durch.
⚠️ Einer von sechs bleibt noch haengen; Ursache noch nicht gemessen.

## Leon steht beim Gefressenwerden richtig (RE2-Modus)

Der Port spielte auch im RE2-Modus die **RE1.5-Clipnummern**. In der RE2-Bank
liegen dort die Animationen, in denen Leon sich noch **stehend** wehrt. Gemessen
an der Kopfhoehe: vorher stehend, jetzt am Boden. Nebenbei war der
Todeszeitpunkt falsch und haette bei den laengeren RE2-Animationen nach einem
Drittel zugeschlagen.

Testsuite 194 Pruefungen.

⚠️ Geraeusche und Bild sind hier weiterhin nicht pruefbar.

---

# RE1.5 Port — v0.3.5 (Early Preview)

## Neu: Gliedmassen-Abschuss und RE2-Schadenswerte auch im RE1.5-Modus

Auf Wunsch uebernommen. Im Modus **AI RE1.5** koennen jetzt auch mit der
Handfeuerwaffe Gliedmassen abgeschossen werden, und die Zombies nutzen die
Schadens- und Lebenspunkt-Werte aus RE2.

⚠️ **Das ist eine bewusste Abweichung vom Original**, kein byte-treuer Fix. Die
Original-Werte bleiben im Code erhalten und werden weiterhin geprueft, damit die
Referenz nicht verlorengeht.

Pistolentreffer bis ein Zombie faellt, im RE1.5-Modus: **20 → 5**. Magnum 7 → 5,
Schrotflinten unveraendert bei 1.
Zwei Waffen werden dabei **schwaecher**, weil RE2 sie niedriger bewertet:
**Messer** 16 → 26, **Ingram** 10 → 19.
Der Zombie mit der internen Nummer 17 (in RE2 ist das Brad Vickers) erbt dessen
feste 250 Lebenspunkte und wird auch hier zum zaehen Brocken.

Ein Befund hat die Umsetzung gerettet: Die Knochen-Nummern von RE1.5 und RE2
bezeichnen **verschiedene Koerperteile**. Direkt uebernommen waere bei einem
Schuss auf den Oberschenkel ein Arm abgefallen. Beide Modelle wurden vermessen;
die Umrechnung nutzt dieselbe Zuordnung wie der Modus "AI RE2".

Das **Abriss-Geraeusch** ist mit verdrahtet.
Bekannte Grenze: Die RE1.5-Modelle haben keine Stumpf-Geometrie (15 Meshes
gegenueber 17 bei RE2) — der Oberschenkel bleibt heil, Schienbein und Fuss
fliegen weg.

## Behoben: Sterben sah in beiden Modi falsch aus

**RE1.5 — die Todesanimation lief zweimal.** Der Zombie fiel nach hinten, sprang
etwa ab der Haelfte zurueck und wiederholte das Stueck. Ursache: Der Port fuehrte
einen Programmzweig aus, den es im Original **gar nicht gibt** — die Sprungbefehle
dorthin stehen in der Spieldatei als lauter Nullen. Er existiert nur in der
rueckuebersetzten Lesefassung und ist von dort in den Port gewandert.
Der Fehler trat nur in etwa **jedem achten Fall** auf; deshalb galt er nach einem
frueheren Bericht als behoben. Der neue Test faehrt jetzt 48 Durchlaeufe.

**RE2 — kein Sturz, falsche Leichenpose.** Hier fuhr der Port fuer *jeden* Tod die
falsche Routine: die des **wiederbelebten Kriechers**. Ausserdem lief die ganze
Routine in einem Zweig, der nur einmal ausgefuehrt wird — es gab also gar keinen
Fallverlauf, der Koerper war schlagartig unten (gemessen: 2218 Einheiten in einem
einzigen Bild). Und die Leichenpose wurde nie gesetzt, weshalb sie vom letzten
Niederschlag uebrig blieb — daher "Ruecken-Animation, dann ploetzlich Bauch".
Jetzt faellt der Zombie sichtbar in die Richtung, aus der getroffen wurde, mit
Bluttropfen an den richtigen Bildern und passender Endpose.

## Behoben: der gefressene Zombie stand auf

Im Dining Room stand der am Boden liegende Zombie im RE2-Modus nach kurzer Zeit
auf. Ursache war ein **erfundener Naehe-Wecker** im Port: Kam der Spieler nah
genug, stand der Gegner auf. Im Original ist diese Stelle ein **leerer
Programmteil** — ein von Anfang an liegender Zombie weckt sich nie selbst. Der
einzige Weg waere ein Skriptbefehl, und die Auszaehlung aller Raumdaten zeigt:
andere Raeume nutzen den, der Dining Room **kein einziges Mal**.
Die fressenden Zombies stehen weiterhin korrekt auf.

Testsuite 183 Pruefungen.

⚠️ Geraeusche und Bild sind hier nicht pruefbar (die Entwicklungssitzung hat
weder Audio-Ausgabe noch Fenster-Aufzeichnung).

---

# RE1.5 Port — v0.3.4 (Early Preview)

## Alle RE2-Gegner hatten die falschen Geraeusche

Gemeldet war "die Kraehen haben den falschen Sound". Die Ursache lag tiefer:
Der Port waehlte die Soundbank aus einer Tabelle, die er fuer eine
**Gegnertyp**-Tabelle hielt. Sie enthaelt aber **Sound-Nummern aus den
Raumdaten** — zwei verschiedene Dinge, die zufaellig beide kleine Zahlen sind.

Dass es nur bei der Kraehe auffiel, war Glueck: Beim Zombie liefert die
Verwechslung zufaellig die richtige Bank.

Belegt ueber alle 250 RE2-Raumdateien. Was die Gegner vorher spielten:

| Gegner | vorher | jetzt |
|---|---|---|
| Kraehe | Bank 21 (fremder Gegner) | **Bank 7** |
| Hund | Bank 31 (eine reine **NPC**-Bank) | **Bank 6** |
| Spinne | Bank 53 (fremde Doppelbank) | **Bank 11** |
| Baby-Spinne | Bank 24 (fremder Gegner) | **Bank 11** (teilt sie mit der Grossen) |

**Der Fress-Biss des Hundes war komplett stumm.** In der falschen Bank
existiert dieser Laut gar nicht. Im Code stand dazu die Notiz, er sei "im
Original eben stumm" — auch das war nur eine Folge der falschen Bank.

**Dazu die Stimmenverwaltung.** Im Original hat jeder Laut einen festen Kanal
und eine Prioritaet: Ein neuer Laut loest den alten ab oder wird verworfen. Der
Port legte jeden Laut auf einen freien Kanal, sodass sie sich uebereinander
stapelten. Im Kraehenschwarm gemessen: Von 49 Lauten klangen **35 anders** als
sie sollten — zusaetzlich zu den durchweg falschen Samples.

## Zombies: Todesanimation und Sturz laufen jetzt durch

Der Trefferfilter des Ports hatte **eine von vier** Sperren. Im Original ist ein
Gegner nicht mehr treffbar, wenn er inaktiv ist, gerade reagiert, **tot ist**
oder in einem Sonderzustand steckt — nachgebaut war nur die zweite.

Gemessen: Nach dem toedlichen Schuss kassierte die Leiche noch **sechs weitere
Treffer**, die die Todesphase jedes Mal zurueckstellten. Deshalb landeten
Zombies "animationslos" am Boden und standen mitten in der Fallanimation wieder
auf. Beides ist behoben.

## Zwei gemeldete Punkte sind KEIN Fehler

- **"Manchmal fallen sie einfach so hin."** Das folgt nicht auf einen Treffer,
  sondern auf einen **Griffversuch**: Endet der Griff, faellt der Zombie in 15
  von 16 Faellen hin. Steht so im Original.
- **"Der Sturz nach dem Torkeln fehlt."** Er funktioniert — braucht aber **drei
  Pistolentreffer**. Ein interner Zaehler muss erst unter eine Schwelle fallen,
  bevor das Torkeln ueberhaupt sturzbereit wird. Mit ein oder zwei Schuessen
  passiert nichts.

## Noch offen

Die **wandernden Blutspritzer** (mal am Oberkoerper, mal am Bein) sind nicht
geloest. Zwei Erklaerungen sind widerlegt: die Knochen-Umsortierung im
Hybrid-Modus (beide Modell-Einstellungen zeigen dasselbe) und die Ankerwahl
selbst (die beiden fraglichen Knochen liegen im Modell exakt am selben Punkt).
Eine echte Fehlerquelle wurde gefunden und entfernt — ein Nebeneffekt konnte
einen zweiten Spritzer am Boden zwischen den Fuessen ausloesen —, liess sich im
Test aber nicht ausloesen. Ob es weg ist, zeigt erst das Spielen.

Testsuite 178 Pruefungen.

⚠️ Geraeusche und Bild sind hier nicht pruefbar (die Entwicklungssitzung hat
weder Audio-Ausgabe noch Fenster-Aufzeichnung). Gemessen wurden Bank, Index,
Kanal, Prioritaet und Frame — nicht der Klang.

---

# RE1.5 Port — v0.3.3 (Early Preview)

## Spinnen und Wasser

**In ROOM2090 stehen beide Spinnen im Wasser** — das war bisher unsichtbar,
weil ein internes Zaehlwerkzeug die betroffenen Datensaetze verschluckt hatte.
Der Raum sah aus, als haette er gar keine Spinnen. Der Port hat den Wasserstand
die ganze Zeit korrekt ausgerechnet, das RE2-Gehirn hat ihn nur nie abgefragt.

Jetzt ist die ganze Kette drin: Im Wasser wird die Spinne auf Wasserhoehe
geklemmt, wirbelt keinen Staub mehr auf — und wenn sie dort stirbt, taucht die
Leiche ein und duempelt mit der Original-Wellenbewegung.

RE1.5 selbst kennt fuer Spinnen ueberhaupt keine Wasserreaktion; dort war
nichts kaputt und es aendert sich nichts.

## Der zaehe Zombie ist Brad Vickers

Im RE2-Modus bekommt der Zombie mit der internen Nummer 17 jetzt **250
Lebenspunkte** — rund dreimal so viel wie seine Nachbarn. Das ist kein Fehler:
In RE2 ist dieser Gegner **Brad Vickers**, der einzige absichtlich zaehe
Sonder-Zombie des Spiels. Das Original prueft schlicht die Gegnernummer und
setzt den Wert hart.

Im Spiel faellt das kaum auf, weil die RE2-Waffen ebenfalls dreimal so stark
zuschlagen: vorher 15 Pistolentreffer, jetzt 16. Mit dem **Messer** ist er
allerdings praktisch nicht mehr zu toeten (rechnerisch 84 Treffer).

In RE1.5 bleibt derselbe Gegner ein ganz normaler Zombie mit 71 Lebenspunkten.

## Zwei Korrekturen am RE2-Hund

**Zwei erfundene Zustandswechsel entfernt.** An einer Stelle im Hunde-Code
standen zwei Zustandsaenderungen, die es im Original gar nicht gibt — die
Routine endet dort ohne jeden Zustandswechsel. Eine davon haette den Hund
dauerhaft als Leiche markiert.

**Ein Halbwort-Fehler behoben.** An zwei Stellen schreibt das Original einen
16-Bit-Wert und setzt damit zwei Felder auf einmal; der Port setzte nur eines.

⚠️ Eine frueher gemeldete Vermutung ist damit **widerrufen**: Der RE2-Hund kann
sich NICHT totstellen. Diese Behauptung beruhte auf einem Lesefehler in der
Analyse (ein 32-Bit-Schreibzugriff war als 8-Bit-Zugriff gelesen worden). Zwei
unabhaengige Sperren im Original schliessen das Verhalten aus; 2048 erzwungene
Durchlaeufe erreichen es null Mal.

Testsuite 176 Pruefungen.

⚠️ Weiterhin nicht am Bildschirm geprueft (die Entwicklungssitzung kann keine
Fenster aufzeichnen) — nur rechnerisch und ueber die Testsuite.

---

# RE1.5 Port — v0.3.2 (Early Preview)

## Drei gemeldete Punkte am RE2-Zombie behoben

**Die Zombies stecken nicht mehr zu viel ein.** Der RE2-Modus hat bisher nur
das VERHALTEN aus RE2 uebernommen — Lebenspunkte und Waffenschaden blieben auf
RE1.5-Werten. Man kaempfte also gegen RE2-Gegner mit RE1.5-Zaehigkeit. Jetzt ist
das Modell vollstaendig portiert. Pistolentreffer, bis ein Zombie faellt:

| Zombie | vorher | jetzt |
|---|---|---|
| Typ 0x10 | 20 | **5** |
| Typ 0x11 | 15 | **4** |
| Typ 0x16 | 18 | **6** |

Schrotflinte toetet weiterhin mit einem Schuss.

⚠️ Zwei Waffen werden dadurch **langsamer**, und das ist korrekt: Das **Messer**
braucht 27 statt 16 Treffer, die **Ingram** 20 statt 10. Die RE2-Messer-Zeile
lautet {3, 0, 0} — drei Schadenspunkte, und in den anderen Trefferzonen gar
keine. Das RE2-Messer ist fast nutzlos. Zusammen mit dem unten genannten
Rueckstoss heisst das: Messerkaempfe gegen RE2-Zombies sind eine schlechte Idee.
So war es im Original auch.

**Zombies fallen jetzt um.** Nach ein paar Treffern schwanken sie nach hinten;
trifft man sie in diesem Fenster erneut, stuerzen sie — wie im Original. Bisher
passierte das nie: Der Port sperrte den Gegner waehrend des Schwankens gegen
weitere Treffer, wodurch der Sturz-Zweig in 900 Bildern kein einziges Mal
erreicht wurde. RE2 laesst den Zombie waehrend beider Trefferreaktionen
angreifbar — genau darauf baut der Sturz auf. Dazu gehoert der Original-
Rueckstoss von 450 Einheiten, der den Zombie beim Schwanken nach hinten schiebt.

**Das Blut erscheint nicht mehr am Fuss.** Der Effekt hing an der Position des
Gegners, und die sitzt am Boden — 1166 Einheiten zu tief. Im Original haengt er
an einem Knochen, und welcher, entscheidet die Trefferzone. Alle 28 Stellen im
Zombie tragen jetzt ihren Anker: Brust, Kopf, Arm oder Oberschenkel je nach
Treffer.

## Ausserdem

**Baby-Spinnen froren im RE2-Modus permanent ein.** Nach dem ersten Treffer
waren sie weder tot noch ansprechbar. In RE1.5 sind sie unzerstoerbar (das ist
korrekt so), in RE2 dagegen stirbt eine Baby-Spinne an jedem Treffer — der Port
rechnete mit den RE1.5-Werten und liess sie mit einem Lebenspunkt in eine
Trefferreaktion laufen, die es im RE2-Modul gar nicht gibt.

**Werkzeug-Korrektur mit Folgen.** Das interne Zaehlwerkzeug fuer Raumdaten hat
Eintraege verloren und dabei trotzdem "100 % Abdeckung" gemeldet. Ueber alle 240
Raeume fehlten 34 Gegner-Platzierungen und 9 Tueren. Daraus war unter anderem
der falsche Beleg entstanden, Baby-Spinnen kaemen im Spiel gar nicht vor. Alle
19 betroffenen Belegzahlen sind neu erhoben; keine Schlussfolgerung kippt.

Testsuite 173 Pruefungen.

⚠️ Weiterhin nicht am Bildschirm geprueft (die Entwicklungssitzung kann keine
Fenster aufzeichnen) — nur rechnerisch und ueber die Testsuite.

---

# RE1.5 Port — v0.3.1 (Early Preview)

## Behoben: im RE2-Modus liessen sich Gegner nicht treffen

Gemeldet: "Bei der RE2-AI kann ich keinen Zombie treffen, weder mit
Schusswaffe noch mit Messer." Das stimmte — und es traf **alle vier** Gegner.

Die Ursache lag nicht dort, wo man sie vermutet. Der Treffer war nie das
Problem, die **Zielerfassung danach** war es: Das Spiel markiert einen
getroffenen Gegner intern und nimmt ihn damit fuer den Rest des Treffers aus
der Zielsuche — sowohl fuer Schuesse als auch fuer den Nahkampf. Aufgehoben
wird diese Markierung normalerweise von der Trefferreaktion des Gegners. Der
RE2-Modus bringt aber seine eigenen Reaktionen mit und hat diese eine Aufgabe
nicht mitgeerbt. Ergebnis: der erste Schuss sass, danach war der Gegner
dauerhaft unverwundbar.

Bei der Kraehe hiess das woertlich unsterblich — sie hat zehn Lebenspunkte und
liess sich genau einmal treffen.

Gemessen wurde diesmal der **komplette Weg ab dem Zielen**, nicht wie zuvor ab
dem Schadenseintritt. Genau daran waren die frueheren Pruefungen
vorbeigelaufen: sie loesten den Schaden direkt aus und sahen alles gruen,
waehrend im Spiel der Schuss den Gegner nie erreichte. Vorher/nachher, jeweils
Dauerfeuer auf dasselbe Ziel:

| | vorher | nachher |
|---|---|---|
| Zombie, Pistole | 1 Treffer | 20, stirbt |
| Zombie, Messer | 1 Treffer | 16, stirbt |
| Hund | 1 Treffer | 8, stirbt |
| Kraehe | 1 Treffer, ueberlebt | 6, stirbt |
| Spinne | 1 Treffer, ueberlebt | 10, stirbt |

Der RE1.5-Modus war nie betroffen und ist unveraendert.

## Neu: dritte Einstellung "AI RE2 MODELS"

Auf Wunsch aufgeteilt. Der Menuepunkt AI hat jetzt drei Stufen:

- **AI RE1.5** — alles wie im Original.
- **AI RE2** — RE2-Verhalten und RE2-Animationen, aber die **Modelle aus
  RE1.5**. (Diese Einstellung hat sich geaendert.)
- **AI RE2 MODELS** — zusaetzlich die RE2-Modelle; das entspricht dem
  bisherigen RE2-Modus.

Dass sich RE2-Animationen ueberhaupt auf RE1.5-Modelle legen lassen, liegt an
einer Eigenheit beider Spiele: Die Skelette sind gleich aufgebaut, nur anders
aufgehaengt — RE2 an der Brust, RE1.5 an der Huefte. Der Abstand zwischen
diesen beiden Punkten ist in beiden Spielen exakt null, das Umhaengen kostet
also nichts. Kurios am Rande: die RE2-Reihenfolge ist genau die, die RE1.5
selbst fuer Leon und alle Mitspieler verwendet — nur die RE1.5-Zombies tanzen
aus der Reihe.

Drei Grenzen dieser Kombination sind gemessen und werden nicht beschoenigt:

- **Hund:** RE2 gibt den Vorderbeinen ein Gelenk mehr, das es in RE1.5 nicht
  gibt. Die Pfote bleibt in dieser Einstellung starr.
- **Zerlegen:** Kein RE1.5-Zombiemodell besitzt einen Beinstumpf. Der
  Unterschenkel fliegt weg, der Oberschenkel bleibt heil.
- **Fussrutschen:** Die RE2-Laufanimation ist fuer ein rund zehn Prozent
  laengeres Bein gemacht. Dagegen wurde bewusst **nichts** eingebaut — jede
  Korrektur waere eine erfundene Zahl.

Testsuite 170 Pruefungen.

⚠️ Nicht am Bildschirm geprueft: Diese Version konnte nur rechnerisch und ueber
die Testsuite verifiziert werden, nicht durch eine Bildaufnahme des laufenden
Spiels. Auf verdrehte Gliedmassen in der Einstellung "AI RE2" und auf den Sitz
der dritten Menuezeile bitte besonders achten.

---

# RE1.5 Port — v0.3.0 (Early Preview)

## Neu in v0.3.0 — die letzten offenen RE2-Punkte

**Abgerissene Koerperteile fliegen jetzt.** Bisher blieben Arme und Beine am
Koerper haengen. Im Original bekommen sie eine eigene Bewegung mit Wurf,
Schwerkraft und Bodenaufprall — und erst dieser Aufprall laesst auch die
nachfolgenden Teile verschwinden (ein abgerissenes Bein prallt auf, danach ist
auch der Fuss weg). Diese Kette war zwar schon eingebaut, aber nie ausloesbar,
weil es ohne fliegende Teile keinen Aufprall gab.

**Der Haupt-Trefferhandler spritzte gar kein Blut** — das fehlte im Port
komplett und ist jetzt drin, inklusive Richtung: bei einem Treffer von hinten
spritzt es entgegengesetzt.

**Die Spinne ist vollstaendig**: Decken- und Wandmodus samt Abseilen am Faden,
alle Sonderreaktionen auf Treffer und Tod, und das komplette Verhalten der
**Baby-Spinne** (ein Lebenspunkt, zertretbar, ertrinkt in Wasser) — samt
Nachlegen durch die erwachsene Spinne bis zu einer Obergrenze.

Zwei Befunde, die bewusst NICHT zu Aenderungen gefuehrt haben:
- Decken- und Wandspinnen kommen in RE1.5 gar nicht vor: alle 58
  Spinnen-Platzierungen der 240 Raeume sind Boden-Spinnen. Die neuen Modi sind
  korrekt, unter den ausgelieferten Daten aber unbenutzt.
- Die Entfernungs-Einstufung der Treffer bleibt unvollstaendig, weil RE1.5 die
  noetigen Trefferbox-Daten nicht besitzt. Eine naheliegende "Reparatur" haette
  dem Messer den Schaden komplett genommen — das ist jetzt mit Adressen belegt
  statt als offener Punkt gefuehrt.

Testsuite 167 Pruefungen. Damit ist der RE2-KI-Modus fuer alle vier gemeinsamen
Gegnertypen abgeschlossen.

---

# RE1.5 Port — v0.2.9 (Early Preview)

## Neu in v0.2.9 — die RE2-Spinne

Der RE2-KI-Modus ist damit fuer alle vier gemeinsamen Gegnertypen verfuegbar:
Zombie, Hund, Kraehe — und jetzt die **Spinne**.

Portiert ist der Boden-Modus vollstaendig: Fortbewegung mit drei
Geschwindigkeitsstufen, der neunphasige Sprungangriff, Biss (20 Schaden),
Trefferreaktion, Tod und Leiche. Nachgemessen im echten Raum: beide Spinnen
laufen auf dem RE2-Verhalten, bewegen sich ueber 600 Bilder mit 19
Zustandswechseln ohne Haenger, der Biss zieht korrekt ab.

Nebenbei belegt und damit **nicht** eingebaut: die RE2-Spinne hat KEIN Gift —
der einzige Schadensaufruf des Moduls beruehrt den Vergiftungs-Zustand des
Spielers nachweislich nicht.

**Spinnen an Decke und Wand behalten vorerst das RE1.5-Verhalten.** Das Original
hat dafuer eigene Modi (inklusive Abseilen am Faden); sie sind untersucht, aber
noch nicht umgesetzt. Die Zustaendigkeit ist so gesetzt, dass eine
Decken-Spinne ueber ihre ganze Lebenszeit beim alten Verhalten bleibt — kein
Bruch mitten im Kampf. Ebenfalls noch offen: die Baby-Spinne.

Testsuite 166 Pruefungen; der Raum-Test schlaegt fehl (statt zu ueberspringen),
wenn kein Raum eine Spinne aufstellt.

---

# RE1.5 Port — v0.2.8 (Early Preview)

## Neu in v0.2.8 — RE2-Gore ist sichtbar

Der Zerleger-Zweig der RE2-Zombies war in v0.2.7 zwar berechnet, aber im Bild
nicht zu sehen. Jetzt schon:

- **Abgerissene Gliedmassen verschwinden wirklich.** Das Original laeuft die
  Koerperteile flach ab — ein abgerissener Oberschenkel nimmt Unterschenkel und
  Fuss NICHT automatisch mit; stattdessen gibt es eine Weitergabe-Regel, die
  dasselbe Ergebnis erzeugt. Beides ist nachgebaut.
- **Verkohlung und Saeure-Faerbung** liegen auf den richtigen Koerperteilen. Die
  Faerbung multipliziert im Original die Beleuchtung (neutral ist nicht null) —
  mit dem urspruenglich gesetzten Startwert waere jeder unversehrte Zombie
  schwarz gewesen; das ist abgefangen.
- **Stumpf-Modell**: der Zerleger tauscht jetzt das dafuer vorgesehene
  Reserve-Modell ein (der bisherige Behelf schrieb stattdessen eine Faerbung,
  die das Original an dieser Stelle gar nicht setzt).
- **Trefferrichtung wirkt**: Treffer von hinten loesen andere Taumel- und
  Sturz-Varianten aus als Treffer von vorn (anderer Clip, anderer Rueckstoss),
  und die Zuck-Richtung spiegelt sich entsprechend.

Testsuite 164 Pruefungen; sieben Gegenproben, jede einzeln nachgewiesen.
Der RE1.5-Modus ist dreifach abgesichert unveraendert.

Noch offen: abgerissene Teile fliegen nicht weg, sondern bleiben am Koerper
(der Port hat keine Physik fuer einzelne Koerperteile) — mit Adressen
dokumentiert.

---

# RE1.5 Port — v0.2.7 (Early Preview)

## Neu in v0.2.7 — RE2-Gegner: Zucken, Ragdoll, und JEDE Waffe wirkt

**Jede Waffe loest jetzt eine Trefferreaktion aus.** Im RE2-Modus trafen sieben
von 22 Angriffsarten eine leere Stelle der Reaktionstabelle — der Zombie steckte
den Treffer wortlos weg. Ursache war eine falsche Zuordnung im Port: Die
Reaktionszeile wird im Original ueber die **Gegenstands-Nummer der gefuehrten
Waffe** gewaehlt, der Port stempelte stattdessen seine eigene Waffen-Nummer und
zeigte damit auf Felder, die es im RE2-Sinn gar nicht gibt (Magnum und
Raketenwerfer machen dort 900 Schaden und toeten sofort — die Reaktion kommt
also nie dran). Jetzt ist jede RE1.5-Waffe ihrer RE2-Entsprechung zugeordnet,
zwoelf davon als harte Identitaet (Browning, Ingram und Flammenwerfer sind in
beiden Spielen dieselbe Waffe).

**Ingram, SPAS-12 und MC51 trafen auf Distanz ueberhaupt nicht.** Ihr
Trefferpunkt-Test lief durch den kurzen Nahkampf-Kegel statt durch den
Schuss-Streifen — ein Fehler in einer Waffenliste im Schadenscode, der BEIDE
KI-Modi betraf.

**Zombies zucken beim Treffer.** Das Original dreht dafuer zwei Knochen direkt:
der Oberkoerper bekommt exakt den Gegenwinkel des Rumpfes, wodurch sich die
Drehung oben aufhebt und nur ein Versatz bleibt — das sichtbare Zucken. Vorher
bewegte sich dabei kein einziges Bild, jetzt 19 von 40.

**Zwei neue Reaktionen:** ein Ragdoll-Sturz mit echter Aufprall-Physik (der
Zombie prallt beim Aufschlag ab und steht als Kriecher wieder auf) und eine
Rutsch-Reaktion. Beide kommen mit staerkeren Waffen im Spiel tatsaechlich vor.

Testsuite 162 Pruefungen; ein Zensus faehrt alle 22 Waffen durch einen echten
Schuss und verlangt je eine tatsaechlich gefahrene Reaktion (Animationswechsel,
Zucken, Laut) — kein blosser Tabellen-Check. Im RE1.5-Modus bleibt die
Original-Logik unveraendert; das ist zusaetzlich abgesichert.

---

# RE1.5 Port — v0.2.6 (Early Preview)

## Neu in v0.2.6 — Zombie stoehnt beim Aufstehen wieder, korrekte Liegezeit

Nutzer-Report zu v0.2.5: "Das schrille Moaning kommt aktuell nur beim Sterben.
Im Original macht er es auch beim Aufstehen, nachdem man ihn niedergeschossen
hat."

**Die Ursache war grundsaetzlicher als der Laut selbst: an dieser Stelle wird
im Original gar nicht gewuerfelt.** Der Zufallsgenerator des Spiels ist eine
reine Rechenfunktion seines Eingabewerts — der gespeicherte "Zustand" wird
zwar gelesen, aber nie verwendet. Im Niedergeschossen-Zustand ist dieser
Eingabewert konstant (die komplette Aufrufkette wurde nachverfolgt: niemand
veraendert ihn unterwegs). Damit fallen beide "Wuerfe" beim Aufstehen immer
gleich aus: **der Stoehn-Laut kommt jedes Mal.** Der Port hatte daraus eine
echte Wahrscheinlichkeit von 1:16 gemacht — in 15 von 16 Faellen blieb es
still. Gemessen ueber acht Zufalls-Startwerte: vorher in fuenf von acht
Durchlaeufen gar kein Laut, jetzt in allen acht.

Aus derselben Herleitung mitkorrigiert:
- **Liegezeit nach dem Niederschuss**: immer 360 Ticks (der Port wuerfelte
  zwischen 30 und 750 — mal eine Sekunde, mal 25).
- **Standfestigkeit nach dem Aufstehen**: fester Wert statt Wurf.

Geprueft und ausgeschlossen: die in v0.2.5 eingefuehrte Stimmen-/Vorrang-
Verwaltung verwirft den Laut NICHT (im Test mitgesichert); sie blieb
unangetastet.

Nicht geaendert, weil ohne Beleg: der Grunzer beim Hinfallen bleibt bei 1/4 —
dort laeuft der Code ueber einen anderen Pfad, dessen Eingabewert nachweislich
nicht konstant ist.

Testsuite 160 Pruefungen; der neue Pin faehrt acht Durchlaeufe mit
verschiedenen Zufalls-Startwerten.

---

# RE1.5 Port — v0.2.5 (Early Preview)

## Neu in v0.2.5 — Sound-Stimmen, RE2-Trefferreaktion, RE2-Soundbank

**Gegner klangen falsch, weil die Stimmen-Verwaltung fehlte.** Im Original ist
jeder Laut fest einer von acht Klang-Stimmen zugeordnet, und auf jeder Stimme
klingt immer nur EIN Sample: ein neuer Laut schneidet den laufenden ab — oder
wird komplett verworfen, wenn der laufende Vorrang hat. Der Port legte jeden
Laut auf einem freien Kanal obendrauf. Beim Zombie (Treffer -> Sturz ->
Aufprall) lagen dadurch vier Samples uebereinander statt zwei; der Fall-Laut
faellt im Original weg, weil die Stimme noch den Treffer-Laut fuehrt. Weil die
Aufnahmen je Raum unterschiedlich lang sind, fiel das im 2F-Korridor auf und im
Dinner-Raum nicht.

**RE2-Modus: Gegner reagieren jetzt auf Treffer.** Das Original waehlt die
Reaktion ueber eine Tabelle mit zwei Eingaengen (Trefferart x Zone/Entfernung);
im Port waren BEIDE Achsen falsch belegt — die Spalte kam aus der Zielhoehe und
landete systematisch in einem leeren Tabellenfeld, und das Byte, das die
Reaktions-Phase fuehrt, wurde ueberschrieben. Damit war die Reaktion
strukturell tot. Jetzt portiert: Grunzer beim Treffer, vierphasige Reaktion,
Zurricktaumeln, leichtes Zucken, Rueckstoss, Knockdown bei Schnellfeuer, und
der Widerstandswert sinkt korrekt pro Trefferart.

**RE2-Modus: Soundbank wechselt wieder.** Sie wurde nur beim ersten Mal geladen
— nach einem Zombie-Raum spielte der Hund Zombie-Samples.

**Kleiner Bildfehler beim Sturz behoben:** im Uebergabemoment blitzte fuer ein
einzelnes Bild eine fremde Pose auf.

Als korrekt belegt und bewusst NICHT geaendert: dass der Zombie nach dem
Zuruecktaumeln seine Lauf-Animation neu startet (Original-Verhalten, vorher
durch die falsche Animationsbank verdeckt), und dass die Raeume verschiedene
Zombie-Stimmsaetze haben.

Noch offen im RE2-Modus (mit Adressen dokumentiert): das feine Zucken des
Oberkoerpers (direkt manipulierte Knochen-Matrizen), zwei von sechs
Reaktions-Routinen (Ragdoll-Sturz, Rutsch-Variante) und der Gore-/Zerleger-Zweig.

Testsuite 154 -> 159 Pruefungen, jede Korrektur mit Gegenprobe.

---

# RE1.5 Port — v0.2.4 (Early Preview)

## Neu in v0.2.4 (gegenueber v0.2.3) — drei Animations-/Sound-Fehler

Aufgefallen bei der Untersuchung einer Nutzer-Meldung ("der liegende Zombie im
2F-Korridor klingt beim Getroffenwerden/Hinfallen/Aufstehen anders als die im
Dinner-Raum"). Die Meldung selbst ist **byte-true** — die Raeume liefern im
Original verschiedene Zombie-Stimmsaetze (andere Aufnahmen, andere Tonhoehe:
11 kHz statt 8 kHz), und der Zufallsgenerator laesst die Grunzer je nach
Blickrichtung unterschiedlich fallen. Auf dem Weg dorthin kamen aber drei echte
Fehler ans Licht:

- **Rueckwaerts abgespielte Animationen spielten ihre Geraeusche zum falschen
  Zeitpunkt.** Im Original haengen Pose und Ton zwingend am selben (gespiegelten)
  Bild-Index; der Port zeichnete gespiegelt, las die Toene aber vorwaerts.
  Gemessen: ein Laut feuerte bei Zaehler 42 statt 17.
- **Ein Richtungs-Merker blieb haengen**: Ein Zombie, den man beim Hinknien zum
  Fressen unterbricht, stand danach **rueckwaerts** auf (auch sichtbar).
- **Sichtbares Stocken beim Aufstehen**: ein unbelegter Ueberblendungswert liess
  die ersten acht von 59 Bildern komplett in der alten Pose einfrieren.

Zwei falsche Faehrten sind im Quelltext dokumentiert, damit sie niemand erneut
"repariert": Der Wert, der wie eine Abspielgeschwindigkeit aussieht, steuert die
Ueberblendungsdauer (der Bildvorschub ist immer ein Bild pro Tick); und die
Vorne/Hinten-Auswahl beim Zu-Boden-Gehen gilt nur fuer den Todes-Clip, nicht fuer
den Knockdown.

Testsuite von 150 auf 154 Pruefungen; jede Korrektur mit Gegenprobe abgesichert.

---

# RE1.5 Port — v0.2.3 (Early Preview)

## Neu in v0.2.3 (gegenueber v0.2.2) — vier gemeldete Punkte

**Zombies im Dinner-Raum nach Wiedereintritt** und **Zombie-Sound beim
Hinfallen/Aufstehen** waren derselbe Fehler: Die STAGE1-Zombies beziehen ihre
Posen aus zwei Animationssaetzen, und der Port ordnete drei Zustaenden den
falschen zu. Folgen: Der Schlaefer, der im Dinner-Raum erst auftaucht, nachdem
man in die Menge gelaufen ist (der Raum schaltet dann per Ausloesebereich auf
eine andere Aufstellung um), spielte eine 14-Bilder-Aktionsanimation statt der
75-Bilder-Steh-Animation — der fruehere Anlauf liess lediglich den falschen Clip
sauber wiederholen. Und weil die Schritt- und Aufprallgeraeusche in den
Einzelbildern des richtigen Satzes stecken, war derselbe Zombie rund um den
Sturz stumm. Die Fall-/Aufsteh-Logik selbst war bereits byte-true.

**Irons schaut jetzt "diagonaler"**: Im Original folgt er Leon dauerhaft mit dem
Kopf, in beiden Achsen zugleich. Im Port lief seine Kopfsteuerung ueberhaupt
nicht an, weil ihre Initialisierung an einen Zustand geknuepft war, den das
Raumskript schon im Ladeframe ueberschreibt — im Original ruft das
Spawn-Kommando den NPC-Code dagegen sofort mit auf. Gemessen dreht sein Kopf
jetzt 54 Grad zu Leon statt starr geradeaus. Betrifft game-weit 22 weitere
NPC-Platzierungen, die im Original ebenso mit dem Kopf folgen.

**Speicher-Nummer zaehlt jetzt durch** — das war *kein* Portfehler: Das Original
zaehlt die Speicherungen des Spielstrangs (der Zaehler steckt im Spielstand und
wird beim Laden zurueckgesetzt), nicht die Karten-Slots. Auf Nutzer-Entscheid
weicht der Port hier bewusst ab und zaehlt monoton hoch; alles andere bleibt
byte-true. Die Abweichung ist im Code und in `analysis/save_counter.md`
dokumentiert.

Testsuite von 144 auf 150 Pruefungen gewachsen; jede Korrektur ist mit einer
Gegenprobe abgesichert (Fix zurueckgebaut -> Pruefung schlaegt fehl).

---

# RE1.5 Port — v0.2.2 (Early Preview)

## Neu in v0.2.2 (gegenueber v0.2.1) — START-FIX: Spiel liess sich nicht mehr starten

**Wer v0.2.1 nicht starten konnte, braucht dieses Paket.** Symptom: Doppelklick auf
`re15_pc.exe` (oder `Start_RE15_Port.bat`) erzeugte einen Prozess im Hintergrund, aber
KEIN Fenster — und nicht einmal eine `debug.log`.

Ursache (per Debugger am haengenden Prozess belegt): Die exe war eine **Konsolen**-
Anwendung (PE-Subsystem 3). Ohne bereits offene Konsole muss der Windows-Loader erst
eine erzeugen, und genau dort blockierte er — **bevor** die erste Zeile des Programms
lief:

```
#0  ntdll!ZwCreateFile
#1  KERNELBASE!AttachConsole
#14 ntdll!LdrLoadDll
#19 ntdll!LdrInitializeThunk      <- noch im Loader, vor main()
```

Aus einer bereits offenen Konsole gestartet lief dieselbe exe normal durch (sie erbt
die Konsole) — daher war der Fehler so verwirrend. `start ""` in der .bat legte
ebenfalls eine neue Konsole an und half deshalb nicht.

Fix: Die exe ist jetzt eine **GUI-Anwendung** (Subsystem 2) — ein SDL-Spiel braucht
keine Konsole und umgeht den Loader-Pfad komplett. Damit Kommandozeilen-Laeufe und die
Testsuite ihre Ausgabe behalten, haengt sich das Programm beim Start an eine *bereits
vorhandene* Eltern-Konsole an (erzeugt keine neue). Zusaetzlich: `start ""` aus der
.bat entfernt. Nebeneffekt: beim Doppelklick oeffnet sich kein schwarzes Konsolen-
fenster mehr.

Verifikation ueber genau den Startweg, der vorher hing:
| | v0.2.1 | v0.2.2 |
|---|---|---|
| Threads | 1 | 14 |
| CPU | 0 % | 0,47 % |
| Fenster | keins | sichtbar, 976x759 |
| `debug.log` | wird nicht geschrieben | wird geschrieben |

Neu im Repo: `tools/diag_window.ps1` — listet ALLE Fenster eines Prozesses (auch
unsichtbare oder ausserhalb des Bildschirms), die Bildschirme und die `debug.log`.
Damit laesst sich "kein Fenster erzeugt" von "Fenster unsichtbar" und "Fenster
ausserhalb des Desktops" unterscheiden.

Inhaltlich identisch zu v0.2.1 (alle 15 dort behobenen Abweichungen sind enthalten);
Testsuite 144/144 auf beiden Plattformen.

---

# RE1.5 Port — v0.2.1 (Early Preview)

## Neu in v0.2.1 — 15 gemeldete Abweichungen behoben

Aus einem Spieltest von v0.2 kamen 17 Beobachtungen. 15 davon waren echte
Abweichungen vom Original und sind behoben; 2 waren korrektes Verhalten und
bleiben unveraendert (unten begruendet). Jede Korrektur traegt ihre
Disassembly-Adresse im Quelltext; die Testsuite ist von 132 auf 144 Pruefungen
gewachsen.

**Die Wurzel gleich zweier Meldungen:** Bei Nicht-Spieler-Figuren lief eine
Posen-Ueberblendung nie aus — der Port mischte dauerhaft 7/8 der vorherigen Pose
bei, wodurch die Beine nur ~39 % ihrer Schwingung erreichten. Das war das
"Gleiten/Schlurfen" von Elliot (Intro) und Marvin (2F-Korridor). In allen
Original-Savestates steht der zugehoerige Zaehler auf 0.

Behoben:
- **Elliot-Intro**: Laufanimation (s.o.) und der harte Szenenschnitt — der
  Uebergang laeuft jetzt wie im Original ueber Schwarzblende und Einblendung.
- **Kurz sichtbarer Helipad-Hintergrund** direkt nach der Charakterwahl: der
  Boot-Notbehelf zeichnete eine Datei, die byte-identisch mit dem
  Helipad-Hintergrund ist; jetzt bleibt das Bild schwarz wie im Original.
- **Helikopter-Rotor im Intro**: zwei portseitige Erfindungen (Lautstaerke nach
  Kameraposition, Stummschaltung nach Spielerzustand) entfernt. Die eine hob
  zusaetzlich das Skript-Stop des Sounds jeden Frame wieder auf. Nachgemessen am
  Tonausgang: Rotor jetzt hoerbar mit korrekter Rotorblatt-Frequenz, und still,
  sobald das Skript ihn stoppt.
- **Leons Kopfhaltung** im und nach dem Intro (er verfolgte durch einen
  Rechenueberlauf eine weit entfernte Figur) sowie seine **Blickrichtung** in der
  Marvin-Szene.
- **Leon schaut liegenden Gegnern nach** — diese Automatik fehlte komplett.
- **Texte frieren das Spiel ein**: Beim Untersuchen steht jetzt alles still
  (Gegner, Animationen, Skript) bis der Text weg ist — wie im Original. Die
  Untertitel der Zwischensequenzen frieren weiterhin nicht ein; diese
  Unterscheidung steckt in den Spieldaten selbst.
- **Modelle nach Tod und NEW GAME** sowie **Messer-Animationen trotz Pistole nach
  dem Laden**: beides waren Zustaende, die einen Neustart ueberlebten.
- **Item-Aufnahme-Text** laesst sich jetzt beschleunigen (Taste halten).
- **Zombie-Menge im Dinner-Raum** nach erneutem Betreten (standen als Statue).
- **Hintere Tuer im 2F-Korridor** zeigte die Hintergruende des Helipads: 13
  Hintergrund-Dateien des Evidence-Korridors waren aus dem falschen Quell-Archiv
  geschnitten. Ein neuer Datentest prueft jetzt alle 156 Raeume.
- **Zombie-Positionen im Evidence-Korridor** und **Zombie-Sounds beim
  Hinfallen/Aufstehen** (RE2-Modus: Sounds spielten in falschen Zustaenden und
  dreimal zu haeufig).

Unveraendert, weil im Original genauso:
- **Verletzt-Idle nach dem Laden**: Das Original startet die Idle-Animation nach
  jedem Laden neutral und wechselt erst nach einigen Sekunden in die verletzte
  Variante — und nur unter halber Gesundheit. Die HUD-Warnung "CAUTION" erscheint
  aber schon deutlich frueher. Ein Spielstand in diesem Fenster zeigt also
  CAUTION ohne Verletzt-Animation, im Original wie hier.
- **Irons' Kopfdrehung**: war Teil desselben Klemmen-Fehlers wie Leons
  Blickrichtung und ist damit miterledigt.

---

## Neu in v0.2 (gegenueber v0.1.2) — RE2-KI-Modus fuer Zombies, Hunde, Kraehen

Im OPTIONS-Menue schaltet der Eintrag **AI: RE1.5 / RE2** die drei Gegnertypen,
die es auch im fertigen Retail-RE2 gibt, auf deren **vollstaendiges
RE2-Original-Verhalten** um — Gehirn, Animationen, Sounds und Effekt-Ablaeufe
byte-genau aus den RE2-Overlays re-engineert (EMZ0 / EMD0G_MOD0 / EMOVL21,
jede Konstante mit Disassembly-Zitat im Code, Master-Doku `RE15_RE2_AI.md`):

- **Zombie** (Wellen A+B): RE2-Gangarten-Maschine, Angriffs-Leiter, 10-Phasen-
  Grab mit Biss-Frames/Schaden aus dem Original-Parameterblock, Partner-
  Aufweck-Domino, dreistufige Trefferreaktion, RE2-Modelle/Clips + ENEMSE-SEs.
- **Hund/Cerberus** (Welle C): 17 Zustaende — Stalking mit Aggro-Meter,
  4-Phasen-Verfolgung mit Ermuedung, Sprungattacke mit echter Flugbahn,
  **Kehlen-Latch als Finisher** (nur bei toedlichem Biss; per Tasten-Hammern
  abschuettelbar), Rudel-Heul-Koordination.
- **Kraehe** (Welle D): 15 Zustaende — Schwarm-Koordination (es greift immer
  nur EINE an), Sichtpruefung, Sturzflug-Hieb, Festkrallen mit Pick-Schaden,
  Spiralsturz mit Erholung, zuckende Leiche mit wachsender Blutlache.
- Die **Spinne** folgt in einer spaeteren Version.

Der RE1.5-Modus (Default) bleibt byte-identisch unberuehrt. Qualitaetssicherung:
je Welle ein adversarieller Multi-Agent-Review (alle bestaetigten Divergenzen
vor dem Commit gefixt), 132 Engine-Tests inkl. A/B-Proben RE1.5-vs-RE2 in
ROOM1140/1190/10C0 (Biss exakt 20 HP, Peck exakt 5 HP, keine Doppel-Claims).

Paketierung: `shared_assets/RE2/` (CDEMD0.EMS + ENEMSE.VBS, ~18 MB) ist neu im
Paket; die Startskripte exportieren `RE15_RE2_ASSET_ROOT`, und
`release/make_package.sh` bricht ab, wenn die RE2-Assets fehlen.
Die Dateinamen unten heissen entsprechend `re15_port_v0.2_*`.

---

## Neu in v0.1.2 (gegenueber v0.1.1) — Linux-/Steam-Deck-Paket repariert
Das v0.1.1-Deck-Paket war in vier Punkten falsch geschnuert. Alle vier sind
belegt und behoben; `release/make_package.sh` bricht jetzt ab, statt sie
noch einmal auszuliefern:

| Defekt in v0.1.1 | Wirkung auf dem Deck | Fix |
|---|---|---|
| Binary enthielt `%sBSS/%s/BG%02d.BSS` (kleingeschriebener Raumordner), der Baum heisst `BSS/ROOM1170/` | ext4 ist case-sensitiv -> **kein Raumhintergrund**, alles schwarz | `bg_pc.c` schreibt den Pfad gross (wie der PRI-Lader daneben); Gate `check_binary_paths` |
| `shared_assets/extracted_fx/` fehlte im Paket (0 Dateien) | **Blut, Muendungsfeuer, Rauch, Huelsen rendern nicht** | wird mitpaketiert; Gate `check_tree` |
| `run.sh` war 53 Bytes (`cd` + `exec`) — kein `RE15_CD_ROOT`, kein Render-Backend | Effekt-Texturen unauffindbar; Fades/Cutscene-Balken **weiss statt schwarz** (Mesa kann den subtraktiven Blend nur unter `opengles2`) | `release/pkg_files/linux/run.sh`, jetzt im Repo versioniert |
| Build gegen `ubuntu:22.04` -> `GLIBC_2.34` | startet auf SteamOS 3.4 (glibc 2.33) **gar nicht** | Build auf Debian-11-/sniper-Basis (`GLIBC_2.29`); Gate `check_glibc` |

Ursache dahinter: `pkg-linux/` ist gitignoriert und wurde von Hand befuellt —
Startskript und Paketinhalt hatten keine versionierte Quelle. Jetzt:
`release/pkg_files/` (Startskripte + README-Vorlagen) + `release/make_package.sh`.

Ausserdem enthalten:
- 1-Frame-ROOM1170-BG-Leck zwischen Player-Select und Intro beseitigt.
- `test_em_status_persist` repariert (nur Test, keine Engine-Aenderung). Er stuerzte in
  ~1 von 150 Laeufen ab und blockierte damit das Release-Gate. Ursache (ASan:
  stack-buffer-underflow in `scd_vm_tick`, `scd_vm.c:594`): der Re-Entry-Teil baute den
  Raum-Wiedereintritt mit `re15_actor_init()` nach, liess Slot 1 aber mit dem geparkten
  Thread des vorigen Spawns belegt. `scd_thread_start` verweigert einen belegten Slot
  (`scd_vm.c:433`), also lief Pruefung (5) NIE — sie hielt aus dem falschen Grund —, und
  der geparkte `pc` zeigte auf einen toten Stack-Puffer. Jetzt benutzt der Test die echte
  Kette `scd_room_reenter` (byte-true FUN_8001d600 -> FUN_800396fc -> FUN_8003ef6c); die
  Pruefung laeuft wirklich und haelt (400/400 sauber, ASan-frei).

## Neu in v0.1.1 (gegenueber v0.1)
- **Elliot-Intro (ROOM1170) byte-true:** Der Renn-Glide ist behoben, und Elliot laeuft
  jetzt vollstaendig ueber die NPC-Sub-VM mit seiner EIGENEN EM047-Animationsbank
  (Roadmap 7b komplett; inkl. eines nachgezogenen Original-Details: das
  SCD-Ankunfts-Flag des Turn-Subs @0x80051dd8).
- **Debug-Menue (SELECT) byte-true:** Original-Navigationsgefuehl (Auto-Repeat-Kadenz
  0/6/8/10), Kreuz/SELECT schliessen per Tasten-EDGE (gehaltene Renn-Taste schliesst
  nicht mehr sofort), 8x8-Original-Debugfont aus TEX.TIM statt der Spielschrift,
  subtraktive Hintergrund-Box, Raumnummern mit fuehrender Null. Bestaetigung = ▢.
- Test-Suite-Fix (Use-after-free, nur Tests; Linux-Gate deckte ihn auf).
Die Dateinamen unten heissen entsprechend `re15_port_v0.1.1_*`.

---

# v0.1 (Basis)

Erster eigenstaendiger PC-Release des Resident-Evil-1.5-Ports (MZD-Build-Basis).
Zwei Pakete (je ~135 MB gezippt, ~290 MB entpackt, Assets enthalten).
**Die ZIPs liegen als Standard-SPLIT-ZIPs direkt in diesem Repo-Ordner**
(Volumes `.z01` + `.zip`, je < 100 MB): einfach die `.zip` mit 7-Zip, WinRAR
oder (Steam Deck) Ark oeffnen — die Volumes werden automatisch als EIN Archiv
gelesen. Hinweis: der Windows-Explorer-eigene ZIP-Handler kann KEINE
Split-Zips — 7-Zip/WinRAR verwenden. Pruefsummen: `SHA256SUMS.txt`.

| Paket | Plattform | Start |
|---|---|---|
| `re15_port_v0.3.0_win64.zip` | Windows x64 | `Start_RE15_Port.bat` doppelklicken |
| `re15_port_v0.3.0_linux_steamdeck_x64.zip` | Linux x64 / Steam Deck (SteamOS 3.x) | `./run.sh` (Deck: als Non-Steam-Game hinzufuegen) |

Beide Pakete sind selbst-enthalten: SDL2 statisch, Assets unter `shared_assets/PSX`
(CD-Baum) plus `shared_assets/extracted_fx` (Effekt-Texturen), Savegames als
PSX-Memory-Card-Image `re15_card.mcr` neben der Anwendung. Gamepads via
SDL-GameController (Steam Deck nativ).
**Startskript benutzen, nicht das Binary direkt** — nur `run.sh` bzw.
`Start_RE15_Port.bat` setzen `RE15_ASSET_ROOT`/`RE15_CD_ROOT`, ohne die findet
der Port `extracted_fx/` nicht (Effekte fehlen dann still).

## Steuerung (Tastatur, DuckStation-Layout)
Pfeile/WASD laufen · K/Shift rennen (✕) · J Aktion (▢, haelt Texte schneller) ·
L Abbrechen (◯) · I Inventar (△) · E zielen (R1) + J Schuss · Enter Start ·
Backspace Select · 1/3 = L2/R2.

## Stand v0.1 (Auszug)
- Byte-true re-engineerte Kernsysteme: Spieler/Kampf/Gegner-KI (alle STAGE1-Typen
  inkl. ROOM1030-Kriechtor mit Hand-Lock-Root-Motion), AOT/SCD-VM, Kamera-Zonen,
  Tuersystem (inkl. Fahrstuhl), Save/Load mit Ortsnamen, Inventar/Item-Box,
  Effekt-Pipeline (additive/subtraktive Blends), Blut-Decals, BGM/SE.
- Verifikation: 123 Engine-Tests (ctest) + Hardware-Gegenmessungen (PCSX-Redux)
  + Savestate-Vergleiche; Details in `analysis/` und `ROADMAP_ROOMCHAIN.md`.

## Bekannte Luecken (ehrlich)
- STAGE-2-6-Inhalte in unterschiedlicher Tiefe; diverse dokumentierte OFFEN-Punkte
  (siehe ROADMAP_ROOMCHAIN.md §5 / UNTESTED_IMPLEMENTATIONS.md).
- Steam Deck: Tastatur-Layout ist Default — fuer Gamepad ggf. Steam-Input-Profil.
- v0.1 = Preview: Abstuerze/Regressionen bitte mit Raum + Schrittfolge melden.

## Build-Reproduktion
Windows: `cmake -S re15_port -B build -G Ninja -DRE15_BUILD_PC=ON -DCMAKE_BUILD_TYPE=Release`
(Binary nach `release/win_out/re15_pc.exe`).

Linux/Deck — **Debian-11-Basis (glibc 2.31), nicht ubuntu:22.04**:
```bash
release/build_linux_deck.sh                    # docker/podman, Image debian:11
release/build_linux_deck.sh --distrobox re15-build   # auf dem Deck selbst
```
Paketieren (beide Plattformen, mit den Gates oben):
```bash
release/make_package.sh --version v0.3.0               # --only linux | --only win
```
Ergebnis: `re15_port_v0.3.0_{linux_steamdeck_x64,win64}.{z01,zip}` + `SHA256SUMS.txt`.
