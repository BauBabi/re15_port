# RE1.5 Port — v0.6.0 (Early Preview)

**Deine zwei Befunde aus dem `fehler`-Ordner sind behoben.**

## Die Karte, 3F

**Das verdrehte Türsymbol** (ROOM1130 ↔ ROOM1150) steht jetzt senkrecht. Die Achse war
übrigens von Anfang an richtig *berechnet* — beide Türdatensätze sagen „senkrechte Wand".
Verloren ging sie erst beim Zusammenführen der zwei Raumseiten: dort griff die
Achsen-Korrektur nur, wenn sich die beiden Rechtecke ganz knapp berühren (≤ 4 px). Bei
diesen beiden überlappen sie 8 px breit und 40 px hoch — also übersprungen, und dann
entschied die Richtung zur *Mitte* des Nachbarraums, die hier diagonal zeigt und damit
ein Münzwurf ist.

**Das Türsymbol, das es nicht geben durfte**, war ROOM1120s dritte Tür (nach ROOM1080).
Ihre Position war nie eine Messung: sie projiziert weit außerhalb des eigenen Rechtecks,
und die Klemmung parkte sie in der Ecke — direkt an ROOM1140s Rechteck, wo sie sich als
Tür zwischen zwei Räumen las, die keine haben. Verworfen wird jetzt nur, was **beides**
ist: ohne Gegenseite *und* diagonal ausgeklemmt. („Ohne Gegenseite" allein wäre falsch —
das sind 98 von 201 Marken.)

Über die ganze Karte gemessen: Türsymbole, die quer zu ihrer gemeinsamen Wand stehen,
**8 → 0**. Der bestehende Prüfpunkt „Symbol zeigt vom Nachbarn weg" steht jetzt bei
**0 von 65** — vorher 1 von 60.

Ehrlich dazugesagt: **ROOM1070 hat dadurch gar kein Türsymbol mehr.** Der Raum hat genau
eine Tür, und seine Kartenlage bildet sie 32 bzw. 39 px neben das eigene Rechteck ab. Das
ist ein Eichungsfehler dieses Raums — vorher war er von einem Symbol an beliebiger Stelle
zugedeckt, jetzt ist er sichtbar. Der ist noch offen.

## Die Kamera in ROOM1140

Die Maske war da und wurde geladen — sie lag nur **zu weit weg**. Eine Maske verdeckt nur
etwas, das weiter hinten steht; die Stativbeine waren nah genug eingetragen, der
Kamerakörper nicht. Deshalb lagen im Bild die Beine vor Leon und das Gehäuse hinter ihm.

Der Grund war eine fehlende Zeile in den Objektdaten: ohne sie wird die Entfernung
*spaltenweise* aus dem untersten sichtbaren Punkt geschätzt. Für die schmalen Beinspalten
stimmt das, für die breiten Gehäusespalten nicht — dort ist der unterste Punkt die
Gehäuse-Unterkante, also scheinbar viel weiter weg. Nachgetragen; alle drei Teile des
Stativs tragen jetzt dieselbe Entfernung. Gemessen an Leons Standort aus deinem
Screenshot: vorher 617 Maskenpixel ohne Wirkung, jetzt keiner mehr.

Die linke Kamera bleibt wie besprochen ohne Maske.

Keine der bestehenden Prüfungen konnte diesen Fehler sehen — sie prüfen, ob eine Maske da
ist und ob sie zu *viel* verdeckt, nie ob sie zu wenig tut. Das Werkzeug warnt jetzt beim
Bauen, wenn ein Objekt ohne Entfernungsangabe eingetragen wird.

Tests: 269/269.

---

# RE1.5 Port — v0.5.9 (Early Preview)

**Der Intro-Absturz ist gefunden — und er war ein anderer als der von gestern.**

Deine Frage, ob etwas im Hintergrund die Anwendung killt, hat sich beantworten lassen:
**nein.** Windows protokolliert jeden Absturz mit dem verursachenden Modul, und dort steht
in allen Fällen `re15_pc.exe` selbst — kein Virenscanner, kein Fremdprozess.

Dasselbe Protokoll hat aber gezeigt, dass ich gestern zu früh „behoben" gesagt habe. Es
waren **drei verschiedene** Abstürze:

| Version | Stelle | Status |
|---|---|---|
| v0.5.7 | Kartenbildschirm | behoben in v0.5.8 |
| v0.5.7 **und v0.5.8** | Audio-Mixer | **behoben in v0.5.9** |
| v0.5.0 | Modell-Animation, einmal gesehen | offen, siehe unten |

Der v0.5.8-Fix war richtig und nötig — aber er war nicht der, der dein Intro traf.

**Was es diesmal war:** Beim Raumwechsel wirft das Spiel die geladenen Sprachaufnahmen
weg. Der Ton-Mixer läuft in einem eigenen Thread und las dabei *weiter aus genau diesen
Aufnahmen* — also aus Speicher, den es nicht mehr gab. Das erklärt beides, was du gesehen
hast: **„teilweise"**, weil es nur kracht, wenn das System den Speicher inzwischen anders
vergeben hat, und **„im Intro"**, weil dort Sprachzeilen laufen *und* der Raum wechselt
(1240 → 1170). Am längsten offen steht das Fenster, wenn der neue Raum für die
angeforderte Zeile gar keine Aufnahme hat — dann zeigt der Mixer bis zum Ende der alten
Aufnahme ins Leere.

Der Fehler ist **älter als die ganze Kartenarbeit** — er steckt schon in v0.5.0.

Behoben: der Mixer wird sauber gelöst, bevor die Aufnahme verschwindet, und das Ganze
unter der Ton-Sperre. Die übrigen Ton-Puffer waren bereits richtig abgesichert; das war
die einzige Lücke.

**Warum 267 grüne Tests das nicht gesehen haben:** die Testschiene erreicht den
PC-Ton-Code überhaupt nicht — er gehört zur Plattform, nicht zur Engine. Genau deshalb
ging der Fehler zweimal durch. Es gibt jetzt einen Test, der den Sperr-Vertrag am
Quelltext prüft und seine Abdeckung ausgibt, damit diese Klasse nicht ein drittes Mal
durchrutscht.

**Noch offen:** ein einzelner Absturz in der Modell-Animation, einmal in v0.5.0 gesehen
und seither nicht wieder. Ich habe ihn dokumentiert statt ihn zu raten — ohne
Reproduktion wäre jede Änderung dort geraten. Falls er dir begegnet: sag mir bitte, was
gerade zu sehen war.

Am Spielinhalt ändert sich gegenüber v0.5.8 nichts.

---

# RE1.5 Port — v0.5.8 (Early Preview)

**Absturz behoben — bitte diese Fassung nehmen.**

Die Abstürze kamen von mir, und sie stecken schon in v0.5.6 und v0.5.7. Beide bitte
löschen.

**Was es war:** meine neue Routine, die die gemalten RE1.5-Türsymbole entfernt, hat die
Tabelle der Kartenrechtecke aus dem **falschen Speicherbereich** gelesen — 556 Bytes
hinter dessen Ende. Was sie dort fand, war Müll, und der nächste Zugriff darauf griff ins
Leere. Deshalb stürzte die Anwendung beim Öffnen der Karte ab — und manchmal schon im
Intro, weil die Bedingung, unter der die Routine läuft, dort zufällig zutreffen kann,
bevor der Kartenbildschirm überhaupt initialisiert ist.

Behoben: die Rechtecke werden jetzt über die Zugriffe der Engine gelesen, die den
richtigen Bereich kennen. Eine ungültige Seite liefert dann schlicht null Rechtecke, und
es wird nichts entfernt statt abzustürzen.

Inhaltlich ändert sich nichts gegenüber v0.5.7 — die Türsymbole sind weiter draußen und
ROOM1170 sitzt weiter auf seinem gemalten Rechteck.

---

# RE1.5 Port — v0.5.7 (Early Preview)

**Die gemalten RE1.5-Türen sind weg, und ROOM1170 hat das Rechteck, das der Künstler
dafür gemalt hat.**

Beides, was du gemeldet hast — und dein Hinweis mit den zwei Türen war der Schlüssel.

**Die RE1.5-Türschwenke sind raus.** Sie stecken in den gemalten Kacheln und kommen mit,
seit die Karte das Original-Material zeichnet. Über die Farbe sind sie nicht zu fassen
(sie sind in der Wandfarbe gemalt), über die Lauflänge schon: Wände sind lange gerade
Läufe, ein Schwenk ist kurz und gekrümmt.

Drei Dinge mussten dazu, und jedes habe ich **am Bild abgenommen** statt behauptet:

* nur in den Kacheln suchen, die eine Seite wirklich zeichnet
* Diagonalen über die Ecke verbinden — eine schräge Wand ist ein Treppchen aus kurzen
  Läufen, und ohne das sah jedes Treppchen-Pixel wie eine Tür aus (auf einem Blatt wären
  so ein Viertel aller Wände verschwunden)
* eine Größengrenze: ein Schwenk ist eine feste Zeichenkonvention, 13 Pixel

Ergebnis: 3 bis 9 Prozent der Wandpixel je Blatt, 4 bis 16 Symbole — und auf 3F exakt die
sechs, die dort im Bild stehen.

**Und ROOM1170s unterer Bereich sitzt jetzt richtig.** Du hattest recht: die zwei Türen
auf 3F, die nach draußen führen, sind genau die vom Dach. Nachgemessen — das Rechteck mit
diesen zwei Türen ist **48×24 Pixel groß, exakt die Größe der Dachfläche**. Der Künstler
hat den Bereich dort selbst gezeichnet; er war nur dem falschen Raum zugeordnet. Der ist
auf sein passendes Rechteck umgezogen.

Dieselbe Verwechslung steckte auf vier weiteren Blättern und ist jetzt überall behoben.

**Der ehrliche Preis:** der umgezogene Raum sitzt auf einem quadratischeren Rechteck,
seine zwei Türen liegen dadurch 13 statt 18 Pixel auseinander. Das steht so in der
Prüfung, damit es nicht untergeht.

---

# RE1.5 Port — v0.5.6 (Early Preview)

**Einstellung 2 und 3 sind jetzt komplett aus deinen Freistellungen gebaut.**

Alle fuenf neuen Teile sitzen mit **100 %** (Lage gemessen, nicht aus dem Dateinamen):
Konferenztisch und beide Kamerastative in Einstellung 2, US-Fahne, Rednerpult und blaue
Fahne in Einstellung 3.

**Vier davon brauchten den Fusspunkt** — deine Freistellung endet jeweils oberhalb der
Stelle, an der der Gegenstand den Boden beruehrt. Ohne den Hinweis kaeme die Tiefe aus
einem zu fernen Punkt und der Gegenstand wuerde gar nichts verdecken. Am Hintergrund
gemessen:

| Gegenstand | Freistellung endet | Bodenkontakt |
|---|---|---|
| US-Fahne | Zeile 179 | gelber Teller bis 194 → **192** |
| Blaue Fahne | Zeile 171 | Teller bis 185 → **182** |
| Rednerpult | Zeile 169 | Sockel trifft Teppich → **196** |
| Kamerastativ links | Zeile 196 | Stange laeuft bis → **210** |

Beim **rechten Kamerastativ** habe ich bewusst **nichts** gesetzt: seine Beine
verschwinden bei Zeile 164 hinter dem Tisch, der Bodenkontakt ist im Bild gar nicht zu
sehen. Die automatische Regel nimmt dort einen Punkt an der fernen Tischkante — das
entspricht ungefaehr der Stelle, an der es steht. Wenn im Spiel jemand dahinter **nicht**
verschwindet, sag Bescheid, dann trage ich die Zeile nach.

**Eine Sache zum Nachziehen, falls du magst:** beim Rednerpult fehlt das untere Drittel
(Zeile 170 bis 196) — deine Freistellung endet dort. Die Tiefe stimmt, aber wer unten
dahinter steht, wird an dieser Stelle nicht verdeckt.

**Treue: 26 526 / 17 602 / 4 822 Punkte — je 0 fehlen, 0 zuviel.**

---

# RE1.5 Port — v0.5.6 (Early Preview)

**ROOM1170: der untere Bereich ist jetzt auf 3F, und die zwei Türen sind vom Dach runter.**

Du hattest beides genau beschrieben, und beides stimmte:

* Auf dem Dach war der Bereich bei der Treppe samt seiner zwei Türen schon eingezeichnet,
  obwohl du gar nicht auf der Ebene warst.
* Gingst du die Treppe runter, fehlte der Marker völlig — und genau dort war ein Loch in
  der Karte.

**Was es war.** Ein Ort, dessen Band auf ein anderes Stockwerk führt, braucht dort eine
eigene Zeile. Die kam bisher aus der selbstgezeichneten Karte; mit der Umstellung auf das
Original-Material fiel sie ersatzlos weg, und die Etagen-Tabelle zeigte ins Leere. Dazu
kam: die zwei Türen tragen in den Spieldaten **Band 0**, gehören also auf 3F — die
Marke fragt das auch korrekt ab, aber die Projektion lieferte trotzdem immer das
Heimatblatt.

```
Dach:  2 Türen des unteren Bereichs  ->  0   (nur noch die Treppe)
3F:    0 Marken auf der Kopie        ->  3 Türen + 3 Treppen
```

**Und dein Wort war der Unterschied.** Ich hatte den Bereich zuerst *verschoben* statt
*kopiert*. Das ist nachweislich falsch — es steht seit dem 31. August als
zurückgenommener Versuch im Code: der Bereich ist ein zusammenhängendes Stück aus kleinem
Raum oben, Treppe und Absatz unten. Wer ihn ganz nach unten legt, schaltet die Karte schon
beim Betreten des oberen Raums um. Dein „Kopie, nicht Umzug" hat mich davor bewahrt, denselben
Fehler ein zweites Mal zu machen.

⛔ **Eine Sache muss ich zugeben:** beim Umschalten auf das Original-Material hatte ich
genau die Prüfung abgeschwächt, die diesen Fehler schützt — sie schlug fehl, und ich habe
sie leiser gestellt statt hinzusehen. Sie hat den Fehler nicht nur gefunden, sie hat ihn
vorausgesagt. Sie steht wieder scharf, zusammen mit vier weiteren Messstellen, die die
gemalten Kacheln bisher gar nicht sehen konnten.

Am Rest der Karte ändert sich nichts: 89 der 96 Räume, Marker immer im richtigen Raum und
nie im falschen.

---

# RE1.5 Port — v0.5.5 (Early Preview)

**Deine neuen Freistellungen sind drin: Konferenztisch und Kamerastativ in Einstellung 2.**

Zuordnung wie immer gemessen, nicht aus dem Dateinamen gelesen:

| Datei | Einstellung | Lage | Übereinstimmung |
|---|---|---|---|
| `00.png` | 0 | x=0 y=122 | **100,0 %** (zweitbeste 81,4 %) |
| `02_01.png` | 2 | x=124 y=144 | **100,0 %** (zweitbeste 66,4 %) |
| `02_02.png` | 2 | x=96 y=152 | **100,0 %** (zweitbeste 97,9 %) |

**Beim Stativ musste ich etwas nachhelfen** — und das ist genau der Fall, den ich dir
beschrieben hatte: seine Stange läuft im Bild bis Zeile 210 weiter, deine Freistellung
endet bei 196. Ohne Hinweis käme die Tiefe aus einem zu fernen Punkt und das Stativ
würde überhaupt nichts verdecken. Ich habe den Bodenkontakt auf Zeile 210 gesetzt.

Beide Tische laufen mit derselben Bezugsebene wie in Einstellung 0 (die untere Kante ist
die Tischplatte, nicht der Boden).

**Treue: 26 526 von 26 526 und 16 940 von 16 940 Punkten gedeckt — 0 fehlen, 0 zuviel.**
Einstellung 0: 78 Rechtecke / 34,5 % des Bildes. Einstellung 2: 79 Rechtecke / 22,1 %.

Zum Nachsehen liegen die fertigen Masken als PNG in `masken_vorschau/ROOM1140/`.

---

# RE1.5 Port — v0.5.4 (Early Preview)

**Die wackligen Tests sind repariert — und sie hatten drei echte Befunde im Bauch.**

`integration_save_counter_pin` und `integration_relatch_pin` fielen sporadisch aus
(gemessen: 1 von 12 bzw. 2 von 8). Ich habe die Fehlerfaelle jeweils MIT Log eingefangen
statt zu raten:

1. **Der Fehler lag nach `exit(0)`.** Im Fehlerfall ist das Spiel-Log vollstaendig — es
   speichert korrekt, erreicht seinen Testhaken und beendet sich sauber. Der Prozess
   meldet trotzdem 1. Dort laufen noch atexit-Handler und der Abbau der SDL- und
   Treiber-Bibliotheken. Die Haken beenden jetzt sofort, ohne diesen Abbau.
2. **Ein Restprozess pflanzte den Fehler fort.** Ein noch nicht ganz beendeter Prozess
   des vorherigen Aufrufs haelt die Dateien im Arbeitsverzeichnis; der naechste Lauf
   kann sein Log nicht anlegen und stirbt sofort — und hinterlaesst wieder einen
   Prozess. Derselbe Effekt hat einmal still einen Rebuild scheitern lassen. Jeder
   Testaufruf bekommt jetzt ein eigenes Verzeichnis. (Fremde Prozesse schiesse ich
   NICHT ab — das koennte dein laufendes Spiel treffen.)
3. **Das Spiel selbst ist in Ordnung.** Dieselbe Datei, dieselbe Umgebung, direkt
   gestartet: 40 von 40 sauber; unter dem Debugger 25 von 25. Der Rest trat nur unter
   der Teststeuerung auf. Dafuer meldet der Test jetzt im Fehlerfall den Code UND den
   kompletten Log und wiederholt **genau einmal** — aber nur, wenn das Spiel gar nicht
   ueber das Hochfahren hinauskam. Lief es und lieferte ein falsches **Ergebnis**, wird
   nicht wiederholt; sonst wuerde der Riegel echte Fehler verschlucken. Jede
   Wiederholung steht laut in der Ausgabe.

**Ergebnis: 0 Fehler in je 10 Runden — und das Netz aus Punkt 3 hat kein einziges Mal
gegriffen.** Die beiden strukturellen Fixes allein reichen.

Am Spiel selbst aendert sich nichts; dein Tisch aus v0.5.3 ist unveraendert drin.

---

# RE1.5 Port — v0.5.4 (Early Preview)

**Der wacklige Test ist repariert — und er hatte einen echten Befund im Bauch.**

`integration_save_counter_pin` und `integration_relatch_pin` fielen sporadisch aus
(gemessen: 1 Fehler in 12 Laeufen). Ich habe den Fehlerfall MIT Log eingefangen, und der
zeigt: das Spiel macht alles richtig. Es speichert korrekt, erreicht seinen Testhaken und
ruft `exit(0)` — der Prozess meldet **trotzdem** 1.

Der Fehler liegt also nicht im Spiel, sondern im **Aufraeumen nach `exit()`**: dort laufen
noch atexit-Handler, statische Destruktoren und der Abbau der SDL- und
Grafiktreiber-Bibliotheken. Einer davon reisst den Prozess gelegentlich mit einem anderen
Code ab. Dass genau die beiden Tests wackeln, die den Prozess ueber einen Testhaken
beenden, war die Bestaetigung.

Behoben: die Testhaken beenden jetzt sofort (`_exit`), ohne Bibliotheks-Abbau. Das ist fuer
einen Testhaken das Richtige — die Speicherkarte ist an der Stelle bereits geschrieben und
geschlossen, und das Log wird unmittelbar davor gespuelt.

---

# RE1.5 Port — v0.5.3 (Early Preview)

**Die Ecke.** Ich habe nachgemessen, was dort passiert: an der rechten Tischecke wird
dein **Bodenschatten** ueber die Tischkante gemalt. Der Bildpunkt gehoert zur Maske und
ist auch undurchsichtig — die Maske zeichnet den Rand also, aber der Schatten gewinnt
die Sortierung.

Ursache war eine falsche Annahme in meiner Tiefenrechnung: sie schneidet den Sehstrahl
mit dem **Boden**. Fuer die untere Kante eines Tisches ist das falsch — dort ist die
sichtbare Flaeche die **Tischplatte**, also eine erhoehte Ebene. Der Bodenschnitt landet
dahinter, die Maske wird zu fern. In der Bildmitte faellt das nicht auf (dort laeuft die
Tischkante unten aus dem Bild), an der gerundeten Ecke schlaegt es durch.

Gemessen statt geraten — noetig war eine Tiefe unter 85,9:

| Bezugsebene | Tiefe an der Ecke | Schatten verdeckt |
|---|---|---|
| Boden | 97 | nein |
| −400 | 87 | nein |
| −600 | 82 | ja |
| **−700** | **75** | **ja, mit Abstand** |

−700 passt auch zur Groessenordnung: du bist 1500 Einheiten hoch, ein Konferenztisch
also knapp die Haelfte.

Deine Freistellung ist unveraendert punktgenau eingebaut: 26 526 von 26 526 Punkten.

---

# RE1.5 Port — v0.5.2 (Early Preview)

**Du hattest recht — der Fehler lag in meinem Einbau, nicht in deiner Freistellung.**

Die Maske wird in Rechtecke zerlegt (so verlangt es das PSX-Format). Meine Zerlegung
nahm immer das groesste Rechteck, das **ganz innerhalb** der Flaeche liegt. An einer
schraegen Kante bleibt dabei zwangslaeufig ein Saum aus duennen Dreiecken uebrig, den
kein volles Rechteck mehr fasst — der fiel raus und wurde nicht gezeichnet. Deshalb war
die Tischkante im Spiel eine grobe Treppe aus 8-10 Bildpunkten, obwohl deine
Freistellung eine glatte Diagonale ist.

Behoben: ein Rechteck **darf** ueber die Flaeche hinausragen, die Feinmaskierung macht
der Atlas. Jetzt wird der umschliessende Kasten gekachelt und jede Kachel behalten, die
etwas von der Flaeche enthaelt. Aus 59 Rechtecken wurden 78, und:

**26 526 von 26 526 Punkten deiner Freistellung sind gedeckt — 0 fehlen, 0 zuviel.**
Die Maske ist jetzt punktgenau dein Umriss.

Das wird ab sofort nach jedem Bauen nachgezaehlt und gemeldet, damit so etwas nicht
wieder unbemerkt durchgeht.

Und ja: **nummeriere die Dateien gern nach Cut**, das ist mir recht. Ich messe die Lage
trotzdem weiter mit — falls mal eine Datei verrutscht, faellt es dadurch sofort auf.

---

# RE1.5 Port — v0.5.1 (Early Preview)

**Dein Tisch ist drin.** Erste Maske im neuen Ablauf.

Eingang war `pri/1140/01.png`. Die Zuordnung habe ich nicht angenommen, sondern gemessen
— deine Freistellung gegen alle zehn Kamera-Einstellungen des Raums gehalten:

| Einstellung | Übereinstimmung |
|---|---|
| **0** | **100,0 %** (Maßstab 1, x=0 y=122) |
| 6 | 81,4 % |
| 2 | 81,1 % |
| 8 | 73,5 % |

Der Dateiname hätte Einstellung 1 nahegelegt — die Messung sagt 0. Deshalb lese ich die
Lage grundsätzlich aus dem Bild und nicht aus dem Namen; du musst dich um Nummerierung
und Ausrichtung also nicht kümmern.

Tiefe: hier war nichts anzugeben, die Silhouette läuft unten aus dem Bild, also ist der
Bodenkontakt die Vorderkante des Tischs selbst. Daraus 59 Rechtecke mit acht
verschiedenen Tiefen (der Tisch läuft ja in die Tiefe), 34,0 % des Bildes.

Zum Nachsehen: `masken_vorschau/ROOM1140/ROOM1140_cut00_x3.png` — das ist genau das, was
im Spiel über dich gemalt wird. Die Kante ist jetzt deine, kein Saum Teppich mehr.

Die übrigen Einstellungen von ROOM1140 haben weiterhin keine Maske.

---

# RE1.5 Port — v0.5.0 (Early Preview)

**ROOM1140 ist komplett zurueckgezogen.** Es ist keine von mir gezogene oder automatisch
geschnittene Maske mehr im Build. Die Masken der uebrigen Raeume bleiben — deren Umrisse
stammen aus den Atlanten der Original-Kuenstler, nicht aus meiner Zeichnung.

Warum: gerade, kontrastreiche Kanten wurden sauber (der Konferenztisch in Einstellung 0
deckte 97 % seiner Kante, nichts auf dem Teppich), aber Pult mit Mikrofonen, die beiden
Schraenke und die Stuehle behielten einen Saum Teppich bzw. Wand. Drei Verfahren, drei
Grenzen: das Auswahlraster ist flaechig, der Bildschnitt nach Kontrast verliert Dunkles
vor Dunklem, und ein getipptes Polygon trifft nur auf rund ±4 Bildpunkte. Die einzigen
exakt sitzenden Stuecke im ganzen Raum waren deine beiden freigestellten Fahnen.

**Fuer deine Freistellungen liegt alles bereit:**

* `masken_vorschau/ROOM1140_hintergrund/` — alle zehn Hintergrundplatten als PNG, je
  320x240 und 960x720 (derselbe Massstab wie deine Bildschirmfotos).
* Du stellst frei, ich baue ein: **ausrichten musst du nichts.** Die freigestellten
  Punkte sind Hintergrundpunkte, also gibt es genau eine Stelle, an der sie passen —
  Lage und Massstab messe ich (bei deinen beiden Fahnen 86,7 % bzw. 99,6 %
  Uebereinstimmung).
* Ein PNG je Gegenstand, Dateiname egal. Was ich daraus mache: Lage, Tiefe aus dem
  Bodenkontakt, Rechteckzerlegung, Atlas, Paket.
* Zum Ansehen jederzeit: `re15_port/tools/maske/vorschau.py <RAUM>` schreibt die fertige
  Maske als RGBA-PNG nach `masken_vorschau/`.

---

# RE1.5 Port — v0.5.0 (Early Preview)

**Die Karte springt nicht mehr durch die Bereiche.**

Du hattest das dreimal gemeldet. Meine eigene Messschiene sagte dazu „61 von 149
Übergängen springen über 16 Pixel" — und log. Die Schleife las die Türliste live aus
demselben Speicher, den sie im selben Durchgang mit zwei weiteren Räumen überschrieb; ab
der zweiten Tür bekam Raum A die Türen des zuletzt geladenen Raums zugeschrieben. Für
ROOM1140 meldete sie Türen nach ROOM1120, den es dort gar nicht gibt.

Die Karte war also die ganze Zeit besser als ihre eigene Prüfung. Was danach übrig blieb,
waren vier echte Ursachen:

* **Der Türzug griff um Haaresbreite nicht.** Wo du erscheinst, liegt bauartbedingt eine
  Spielerbreite hinter der Wand — sonst stündest du in ihr. Es fehlten 15 bis 150
  Einheiten, alles unter einem halben Kartenpixel.
* **Skript-Warps wurden als Türen gezählt.** Dreizehn Einträge mit Nullfläche, durch die
  man gar nicht läuft. Einer davon meldete einen 33-Pixel-Sprung durch eine Tür, die es
  in den Raumdaten überhaupt nicht gibt.
* **Beim Öffnen stehst du in der Tür, der Marker aber nur zu einem Viertel dort.** Das
  Zug-Gewicht fiel von der Mitte des Auslösers ab statt von seinem Rand.
* **Einseitige Türen banden den Grundriss nicht.** Ein Raum, der nur so hängt, wurde von
  der Anordnung aktiv weggeschoben statt angelegt. Auf einem Blatt lagen deshalb zwei
  Türsymbole übereinander — das war der letzte echte Fehler im Karten-Audit, jetzt null.

| | vorher | jetzt |
|---|---|---|
| Sprünge über 16 Pixel | 10 von 172 | **4 von 167** |
| Übergänge innerhalb 2 Pixel | 31 % | **66 %** |
| Marker im eigenen Rechteck | 95 von 96 | **96 von 96** |
| Marker im **fremden** Raum | nie geprüft | **0** |

**Was ich dabei nicht gemacht habe:** den Zugbereich größer drehen, bis die Zahl schön
ist. Sie wird nämlich immer schöner — weil ein weiter Zug beide Türseiten auf dasselbe
Symbol schnappen lässt. Bei 1800 verschwindet ein ganzer Korridor im Zugbereich. Der
eingebaute Wert ist der abgeleitete, nicht der schmeichelhafteste.

**Ein Rest bleibt ehrlich stehen:** ROOM1090 → ROOM1100 springt weiter um 93 Pixel. Das
ist die einzige echte Lücke im ganzen Spiel — zwei verbundene Räume, die auf ihrem Blatt
42 Pixel auseinanderliegen. Die Ursache ist gefunden, die Reparatur fasst die Anordnung
des ganzen Blattes an; das mache ich nicht nebenbei.

---

# RE1.5 Port — v0.4.9 (Early Preview)

**Du hast recht, und deine Freistellungen sind ab jetzt der Weg.**

Nein, du siehst das nicht falsch: mehr als das brauche ich nicht. Das Maskenformat IST
eine Freistellung — der Atlas hat einen Palettenplatz, der gar nicht gezeichnet wird.
Dein Alphakanal ist also 1:1 der Inhalt. Beide Fahnen kommen in dieser Fassung direkt
aus deinen beiden PNGs.

**Die Lage muss ich dich nicht fragen**, die messe ich: die freigestellten Punkte sind
Hintergrundpunkte, also gibt es genau eine Stelle im Bild, an der sie passen. Gefunden:
US-Fahne bei x=120/y=67 im Maßstab 3 (86,7 % Übereinstimmung), blaue Fahne bei x=203/y=63
(99,6 %).

**Warum meine Automatik es nicht konnte** — drei Verfahren, drei Sackgassen, alle drei
hast du als Fehler gesehen:
* Superpixel sind flächig. Eine 20 Punkte breite Fahne bekommt einen 55×117-Block samt
  Fenster und Teppich. (Das war die „überdeckende Transparenz".)
* Der Bildschnitt nach Kontrast verliert Dunkles vor Dunklem — die untere Hälfte der
  blauen Fahne. (Das war „Flagge rechts noch nicht".)
* Kästen treffen keine fransigen Formen.

Ein Mensch mit dem Lasso hat keins dieser Probleme.

**Damit ist der Ablauf ab jetzt:** du stellst frei, was verdecken soll — ich messe die
Lage, rechne die Tiefe aus dem Bodenkontakt und baue Rechtecke plus Atlas daraus. Wenn
du magst, sammle die Freistellungen einfach im `fehler`-Ordner; ein Bild je Gegenstand,
Dateiname egal.

Pult, die beiden Holzschränke und das Metallgestell sind weiter als Kasten bzw.
Bildschnitt drin — bei denen stimmt es. Die Pflanzen bleiben unmarkiert.

---

---


# RE1.5 Port — v0.4.8 (Early Preview)

Deine drei Punkte aus v0.4.7:

* **Rechte Flagge.** Der Bildschnitt hat ihre untere Haelfte verloren — das Tuch ist dort
  fast schwarz vor dunklem Teppich, es gibt schlicht keinen Kontrast zu schneiden. Sie
  haengt gerade, also ist sie jetzt direkt als Kasten angegeben (Tuch, Stange, Fussteller).
* **Die beiden Holzschraenke rechts und links** waren ueberhaupt nicht markiert. Auch sie
  sind dunkel vor dunklem Teppich, aber nahezu rechteckig — jetzt als Kasten mit
  **schraeger Unterkante**, weil ihre Vorderkante im Bild diagonal laeuft.
* Das Metallgestell links ist ebenfalls dazugekommen.

Die beiden Pflanzen auf den Schraenken bleiben absichtlich unmarkiert: fransiges Laub,
jeder Kasten und jeder Bildschnitt nahm dort Wand mit. Zu wenig Nutzen fuer das Risiko.

---

# RE1.5 Port — v0.4.7 (Early Preview)

**Die Maske ist jetzt die Silhouette des Gegenstands, nicht mehr ein Block darum herum.**

Du hattest recht, und der Grund war groesser als gedacht. Nachgemessen: die US-Fahne ist
rund 20 Bildpunkte breit — ihre Maske war ein Block von rund 55 x 117 Punkten. Das
Vier- bis Fuenffache des Gegenstands, samt Fenster, Sockelwand, Teppich und Schrank.
Alles, was dahinter stand, bekam diese Pixel uebergemalt. Genau das siehst du als
halbdurchsichtigen Fleck.

Zwei Ursachen:

1. **Eine Fuellung, die Luecken schliessen sollte.** Sie fuellte je Bildspalte alles
   zwischen oberstem und unterstem Punkt eines Gegenstands. Bei einer Fahne, deren
   Fussteller 110 Zeilen tiefer sitzt, wird daraus ein voller Block. Sie ist jetzt aus —
   sie war ohnehin die schlechtere Wahl.

2. **Das Auswahlraster ist flaechig.** Ein Rasterfeld hat rund 700 Bildpunkte; eine
   20 Punkte breite Fahne passt da nicht hinein, ohne die Umgebung mitzunehmen. Feiner
   rastern half nicht (bei 900 Feldern zerfaellt das Verfahren in ein 4x4-Gitter, das
   den Kanten gar nicht mehr folgt). Duenne Dinge — beide Fahnen, die Mikrofone —
   werden jetzt direkt aus dem Bild geschnitten: je Bildzeile wird der Hintergrund links
   und rechts daneben gemessen, und behalten wird nur, was sich davon abhebt. Massive
   Dinge wie das Pult und die Anschlagtafel sind direkt als Kasten angegeben.

Ergebnis im Sitzungssaal mit dem Pult: die Maskenflaeche faellt von 12,3 % auf 9,6 % des
Bildes bei gleicher Wirkung — der Rest war Umgebung, die nichts verdecken durfte.

Das Pult verdeckt weiterhin bis zur Pultplatte hinauf (aus v0.4.6).

---

# RE1.5 Port — v0.4.6 (Early Preview)

Nachtrag zu v0.4.5, zwei Sachen:

**Das Pult verdeckt jetzt hoeher.** Du hattest recht — nachgemessen an deinem Screenshot:
die Maske begann erst an der Oberkante des Pult-KORPUS (Bildzeile 132), die Pultplatte
mit den Mikrofonen reicht aber bis Zeile 108 hinauf. Deine Arme und dein Bauch wurden
deshalb ueber die Pultplatte gezeichnet, obwohl du dahinter stehst. Platte und Mikrofone
gehoeren jetzt zur Maske; sie beginnt bei Zeile 103.

**Die Maskenflaeche war systematisch etwas zu dunkel.** Die Maske malt Hintergrundpixel
zurueck auf den Hintergrund; beim Umrechnen auf die PSX-Farbtiefe (5 Bit je Kanal) wurde
immer abgerundet statt gerundet. Ueber alle 130 812 Maskenpunkte gemessen: mittlerer
Kanalfehler -3,2 von 255 — ein leichter dunkler Fleck genau dort, wo eine Maske liegt.
Jetzt sind es +0,5, also die Grenze dessen, was 5 Bit ueberhaupt koennen (und damit
besser als die Original-Atlanten des Prototyps).

Die uebrigen Bilder aus deinem Ordner habe ich einzeln nachgerechnet: bei den beiden
Fahnen und am Tischrand steht die Figur tatsaechlich neben bzw. vor dem Gegenstand,
die Zeichenreihenfolge stimmt dort. Wenn dir eine davon trotzdem falsch vorkommt, sag
welche — dann sehe ich mir genau die an.

---

# RE1.5 Port — v0.4.5 (Early Preview)

**Deine sieben Screenshots aus dem `fehler`-Ordner — durchgemessen, und ROOM1140 komplett
neu gezeichnet.**

Du hattest recht: es waren viele. Ich habe jeden Screenshot auf den Hintergrund
ausgerichtet (93-99 % Deckung) und Bildpunkt fuer Bildpunkt nachgerechnet, welche Maske
mit welcher Tiefe wo lag. Dahinter steckten **zwei Fehler im Verfahren**, nicht sieben
Einzelfehler:

**1. Der Maske fehlte der Fusspunkt des Gegenstands.**
Die Tiefe einer Maske kam bisher aus dem untersten *sichtbaren* Punkt der markierten
Flaeche. Bei der Fahne im Sitzungssaal ist das die Unterkante des Tuchs — die duenne
Stange darunter war nicht markiert. Ergebnis: die Fahne galt als 1,5 m weiter hinten als
sie steht, und du wurdest davor gezeichnet statt dahinter.
Und umgekehrt: Fensterglas, Fensterbank, Wand, Teppich und Fahne hingen als EIN Stueck
zusammen, das bis zum Fahnenfuss hinunterreichte — also erbte der ganze Streifen die
nahe Tiefe der Fahne. Deshalb war von dir nur noch der Kopf zu sehen.

**2. Wand und Gegenstand lagen in derselben Flaeche.**
Das Auswahlraster war zu grob. Gemessen: in einer Einstellung lagen **500 von 504**
Maskenpunkten auf **Fensterglas**, in einer anderen **649 Punkte auf blankem Teppich**
unter der Tischkante, dazu Wandbild und Wand hinter der Sitzbank.

**Was jetzt anders ist**

* Jeder Gegenstand ist ein **eigenes Objekt** mit eigener Tiefe aus seinem eigenen
  Fusspunkt. Wo der Fusspunkt im Bild verdeckt ist (Fahnen auf Stangen, die Anschlagtafel
  im Rollgestell), wird er ausdruecklich angegeben.
* Schraege Kanten: ein Tisch endet im Bild diagonal — dafuer gibt es jetzt eine schraege
  Unterkante statt rechteckiger Naeherungen.
* **Was sich nicht sauber abgrenzen laesst, bleibt unmarkiert.** Die Stuhlreihen sind
  farblich fast nicht vom Teppich zu unterscheiden; sie haben jetzt keine Maske mehr.
  Lieber verschwindest du hinter einem Stuhl nicht, als dass ein Stueck Teppich ueber
  dir liegt.

**Neue Messschiene.** Das Spiel gibt jetzt auf Wunsch alle **begehbaren** Bodenpunkte
aus (51 260 in diesem Raum, aus der echten Kollision der Engine). Damit wird die Figur
rechnerisch auf jeden dieser Punkte gestellt und gemessen, wieviel von ihr hinter der
Maske verschwindet. Der Anteil stark verdeckter Stehplaetze faellt in der
Sitzungssaal-Einstellung von 51 % auf 14 %.

**Worauf es weiter ankommt:** liegt irgendwo noch ein Stueck Hintergrund ueber dir, wo du
davor stehst? Melde es bitte weiter so — mit Screenshot. Fehlende Verdeckung (du stehst
auf statt hinter etwas) ist der harmlosere Fall und kommt danach dran.

**ROOM1130 bleibt ohne Masken.**

---

# RE1.5 Port — v0.4.4 (Early Preview)

**Deine drei Befunde aus v0.4.2 (error01/02/03) sind behoben — und die Abnahme, die sie
durchgelassen hat, ist ersetzt.**

| Dein Bild | Einstellung | Was da los war | Jetzt |
|---|---|---|---|
| error01 | 2 (Pult/Fenster) | Ein Streifen **Wand** lag ueber deiner Schulter | Wandband ueber der Sitzbank entfernt, Gemaelde frei |
| error02 | 1 (Tisch von hinten) | Ein **schwarzer Kasten** lag ueber Brust und Oberschenkel im Gang | Der Gang zwischen den Stuehlen wird nicht mehr mitmarkiert |
| error03 | 5 (Flur/Anschlagtafel) | Du wurdest **ueber die Anschlagtafel** gezeichnet | Tafel, Schrank und Schreibtisch verdecken jetzt |

**Warum das passiert ist.** Zwei verschiedene Ursachen, beide inzwischen abgestellt:

1. *Wand und Objekt lagen in derselben Flaeche.* Das Auswahlblatt arbeitete mit 90
   Flaechen; bei dieser Groebe waren Wandbild, Wand, Sitzbank und Kamerastativ EIN
   Stueck — entweder alles markieren oder nichts. Und weil die Tiefe einer Flaeche aus
   ihrem untersten Punkt kommt, erbte die Wand die nahe Tiefe des Stativfusses. Eine
   Wand mit der Tiefe eines Stativs verdeckt dich. Jetzt: 220 Flaechen, und Wand- oder
   Bodenanteile lassen sich gezielt herausschneiden.
2. *Die Nachbearbeitung mauerte Luecken zu.* Sie fuellte je Bildspalte alles zwischen
   oberstem und unterstem markierten Punkt — bei einer Stuhlreihe also auch den Teppich
   zwischen den Stuehlen, genau dort, wo du gehst. Jetzt wird nur noch INNERHALB eines
   zusammenhaengenden Objekts gefuellt.

**Und die eigentliche Lehre:** geprueft wurde bisher die *Auswahl*, nicht das *Ergebnis*.
Alle drei Fehler entstehen erst nach der Auswahl oder sind eine schlichte Auslassung —
im Deckungsbild sieht man sie sofort. Ab jetzt wird je Einstellung das Deckungsbild
angesehen, also das, was der Renderer wirklich ueber dich legt.

Beim Durchsehen sind drei weitere Stellen derselben Art aufgefallen und mitbehoben
worden, bevor du sie melden musstest: Wandband in Einstellung 4, Fensterwand in 7,
Wand ueber dem Bildschirm in 9.

**ROOM1130 bleibt weiter ohne Masken** — er ist fast durchgehend Flur.

**Worauf es ankommt:** liegt irgendwo noch ein Stueck Hintergrund ueber dir, wo du
davor stehst? Das ist der schlimmere Fall; melde ihn bitte mit der Kamera-Einstellung.
Fehlende Verdeckung (du stehst auf statt hinter etwas) ist der harmlosere und kommt
danach dran.

---

# RE1.5 Port — v0.4.2 (Early Preview)

**ROOM1140 (Briefing Room), zweiter Anlauf — diesmal nur freistehende Objekte.**

Nach deinem Befund ("ueberall ueberlagernde Grafikfehler beim Vorbeilaufen an den
Waenden") habe ich beide Raeume zurueckgezogen und ROOM1140 neu gezeichnet. Die Regel,
die aus dem Fehler folgt: **Waende bleiben unmarkiert.** Eine Wandflaeche grenzt entlang
einer langen Diagonale direkt an den Boden — genau dort laeufst du, also landet jede
Ungenauigkeit unmittelbar in deinem Weg. Ein Tisch oder Schreibtisch beruehrt den Boden
nur an einer kurzen Kante.

Diesmal ist **jede** Einstellung einzeln durchgesehen worden (Auswahl mit Nummern ueber
dem Bild), nicht nur gezeichnet:

| Einstellung | verdeckt jetzt | Korrektur |
|---|---|---|
| 0 | Tisch + Stuehle | sauber |
| 1 | Tisch + Stuehle | sauber |
| 2 | Tisch + Kamerastative | 1 Flaeche entfernt |
| 3 | Podium, Fahnen, Mikrofone | **11 entfernt** (das war dein Screenshot) |
| 4 | Flipchart, Bank, Tisch | 3 entfernt |
| 5 | Tisch + Schreibtisch | ganze linke Wand entfernt |
| 6 | — | keine Maske: reiner Flur |
| 7 | Tisch | 4 entfernt |
| 8 | — | keine Maske |
| 9 | Schreibtisch, Rechner, Muelleimer, Stuhl | sauber |

**365 Masken in 8 von 10 Einstellungen.**

**ROOM1130 bleibt bewusst ohne Masken** — er ist fast durchgehend Flur, dort gaebe es
ueberwiegend Waende zu markieren.

**Worauf es ankommt:** verschwindest du sauber hinter Tisch, Podium und Schreibtisch —
und liegt umgekehrt NIRGENDS ein Stueck Hintergrund ueber dir, wo du davor stehst?
Der zweite Fall ist der, den wir jagen; melde ihn bitte mit der Kamera-Einstellung.

---

# RE1.5 Port — v0.4.1 (Early Preview)

**Sauberer Stand: die handgezeichneten Masken fuer ROOM1130 und ROOM1140 sind wieder
draussen.**

Du hattest gemeldet, dass es beim Vorbeilaufen an den Waenden ueberall ueberlagernde
Grafikfehler gibt. Der Befund ist bestaetigt und die Ursache ist meine: ich hatte
**Seitenwaende** markiert. Eine Wandflaeche grenzt entlang einer langen Diagonale direkt
an den Boden — genau dort, wo du laeufst. Jeder Ueberstand der Nachbearbeitung landet
also unmittelbar in deinem Laufweg. Dazu kam in mehreren Einstellungen, dass ich
Teppichflaechen fuer Moebel gehalten habe; Boden zu markieren verdeckt die Figur
ueberall, weil der Boden sich hinter sie zieht.

Bei der Durchsicht von 5 der 9 Einstellungen in ROOM1140 fanden sich in **vier** davon
Fehler. Statt dir das weiter zuzumuten, sind beide Raeume auf den Original-Stand
zurueckgesetzt — dort gibt es keine Verdeckung, aber auch keine falsche.

**Was drin bleibt:** die 232 automatisch erzeugten Einstellungen in 32 Raeumen. Die
stammen aus der Vordergrund-GRAFIK der Original-Kuenstler und sind von dieser
Fehlerklasse nicht betroffen — dort wurde nichts von Hand ausgewaehlt.
Ebenso die Reparatur aus v0.3.98: 188 Einstellungen, deren Masken im Original vorliegen
und im Port bis dahin gar nicht geladen wurden.

Ich zeichne die beiden Raeume neu — dann ohne Waende, nur freistehende Objekte, wo die
Fehlerquote nachweislich niedrig ist.

---

# RE1.5 Port — v0.4.0 (Early Preview)

**Testpaket: ROOM1130 (Flur vor dem Briefing Room) ist nachgezeichnet.**

Alle zehn Kamera-Einstellungen, **298 Masken**. Ueberwiegend Seitenwaende — dort gilt die
Konvention der Original-Kuenstler durchgehend: senkrechte Flaechen vom Boden aufwaerts,
Decke und Boden bleiben frei. Dazu die grossen Vitrinen an den Bildraendern, die sehr nah
an der Kamera stehen.

Zusammen mit v0.3.99 sind damit **zwei Raeume** fertig: ROOM1140 (Briefing Room, 316
Masken) und ROOM1130 (614 Masken insgesamt).

**Worauf du achten kannst:** ob du beim Durchlaufen des Flurs sauber hinter den
Wandkanten und Vitrinen verschwindest — und ob umgekehrt irgendwo ein Stueck Wand ueber
dir liegt, wo du davor stehst. Der zweite Fall waere schlimmer als der alte Zustand;
melde ihn bitte mit der Kamera-Einstellung.

Sonst unveraendert (gleiche Binaries wie v0.3.98, nur neue Maskendaten).

---

# RE1.5 Port — v0.3.99 (Early Preview)

**Testpaket: ROOM1140 (Briefing Room) ist von Hand nachgezeichnet.**

Der Raum aus deinem urspruenglichen Report — *"Leon steht auf dem Tisch"* — hat jetzt
Vordergrund-Verdeckungen. Im Original hatte er **keine**: Header-Byte 0, alle zehn
Einstellungen zeigen auf `FF FF FF FF`, kein Vordergrundbild. Das war eine der Luecken,
die der Prototyp offen gelassen hat.

| Einstellung | Masken | was verdeckt |
|---|---|---|
| 0 | 31 | Konferenztisch + Stuehle |
| 1 | 17 | Tisch und Stuehle an der linken Wand |
| 2 | 34 | Tisch, Kamerastative |
| 3 | 50 | Podium, beide Fahnen, seitliche Baenke |
| 4 | 57 | Flipchart, linke Bank, rechter Tisch |
| 5 | 24 | nahe linke Wand, Tisch und Schreibtisch |
| 6 | 12 | beide Flurwaende (nur die nahen Abschnitte) |
| 7 | 36 | Tisch links, nahe rechte Wand |
| 8 | — | **bewusst leer** |
| 9 | 55 | Schreibtisch, Muelleimer, Stuhl, Eckwand |

**316 Masken.** Einstellung 8 bleibt leer, weil dort Pflanze, Beistelltisch und Sockel
alle *an* der Wand stehen und der Spieler davor laeuft — es gibt schlicht nichts
zwischen Kamera und Figur. Eine Maske haette dort nur schaden koennen.

**Worauf du achten kannst:** ob die Fuesse sauber hinter Tisch- und Wandkanten
verschwinden, und ob umgekehrt irgendwo ein Stueck Hintergrund ueber Leon liegt, wo er
davor steht. Der zweite Fall waere schlimmer als der alte Zustand — melde ihn bitte mit
der Kamera-Einstellung, dann ist er gezielt zu finden.

Sonst unveraendert gegenueber v0.3.98 (gleiche Binaries, nur neue Maskendaten).

---

# RE1.5 Port — v0.3.98 (Early Preview)

**Die fehlenden Verdeckungen.** Du hattest gemeldet: „so gut wie alle ESP fehlen — Leon
steht auf dem Tisch statt dahinter". Diese Version behebt den technischen Teil davon und
legt offen, was am Prototyp selbst fehlt.

## Was dahintersteckte

Der Mechanismus heißt `sprite.pri`: pro Kamera-Einstellung liegen im Raum Rechtecke, die
Vordergrund-Pixel über die Figur zurückmalen. Beim Nachmessen kamen zwei Defekte heraus —
und ein Befund, der keiner ist.

**Defekt 1 — 188 Einstellungen waren tot.** Der Port holte die Vordergrund-Textur aus
vorextrahierten Dateien. Die gab es für 209 von 359 Einstellungen; fehlte eine, fiel die
Verdeckung für die *ganze* Einstellung aus — obwohl alle Daten auf der CD liegen. Betroffen
waren **6530 echte Maskenrecords** in 24 Räumen, darunter ROOM1020/1030/1040/1070 und
STAGE5 komplett. Der Port liest die Textur jetzt so, wie das Original es tut: über einen
Trailer am Ende der geladenen Cut-Daten.

**Defekt 2 — beim Raumwechsel wurde nichts erneuert.** Der Maskensatz hing allein am
Kamerawechsel. Betrat man einen Raum, blieben die Rechtecke des **Vorgängerraums** stehen,
bis zufällig die Kamera schwenkte.

**Kein Defekt — ROOM1140.** Der Briefing Room, dein Beispiel, hat im Original *keine*
Maskendaten: Header-Byte 0, alle zehn Einstellungen zeigen auf `FF FF FF FF`, kein
Vordergrundbild im BSS. Zum Vergleich, gleiche Engine, gleiches Format:

| | Einstellungen mit Masken | Räume mit ≥1 Maske |
|---|---|---|
| RE2 Retail | 67 % | 95 % |
| RE1.5 | **22 %** | **33 %** |

Die Maskenarbeit war im Prototyp zu etwa einem Drittel fertig. Der Tisch in 1140 ist eine
dieser Lücken.

## Was neu dazugekommen ist

Nach deiner Ansage („fehlende Masken müssen wir nachzeichnen") sind **232 Einstellungen in
32 Räumen mit 6740 nachgezeichneten Masken** dazugekommen — überall dort, wo der Prototyp
die Vordergrund-*Grafik* hinterlassen hat, aber die Geometrie fehlte. Die Grafik musste
nicht erfunden werden: sie steht im Hintergrundbild.

Die Ergänzungen liegen als eigene Dateien neben den Originaldaten. **Die Originaldaten
haben immer Vorrang** — jede Einstellung, die die Künstler damals bearbeitet haben, bleibt
unangetastet.

## Was ich nicht gemacht habe

Für **1468 Einstellungen** fehlt beides, Geometrie *und* Grafik. Ich habe zwei Wege
gemessen, sie automatisch zu füllen: aus der Kollisionsgeometrie (26–28 % richtig) und mit
einem gelernten Bildverfahren (20–26 % richtig). Meine vorher festgelegte Grenze war 95 %.
Eine automatische Füllung hätte also in drei von vier Fällen Hintergrund über Leon gemalt —
**schlechter als der Zustand, den du behoben haben wolltest.** Deshalb bleiben sie leer.

Stattdessen liegt ein Werkzeug bei (`re15_port/tools/maske/`): Bereiche werden im
Hintergrundbild **angeklickt** statt gemalt, die Tiefe wird vorgeschlagen, die Ausgabe geht
ins selbe Format. Damit sind diese Einstellungen Handarbeit — aber verlässliche.

## Zahlen zur Einordnung

* Verdeckung in ROOM3040 gemessen: vorher 0 von 12 erreichbaren Positionen, jetzt greift sie
* Platzierung der nachgezeichneten Masken: **99,7 %** der markierten Pixel sitzen richtig
* Tiefen-Übereinstimmung mit den Künstlerwerten: 73,7 %, bei 4,6 % zu früher Verdeckung
* 267/267 Prüfungen grün, Windows und Linux/Deck beide frisch gebaut

## Für Steam Deck / Linux

Unverändert: entpacken, `run.sh` starten. Die glibc-Anforderung liegt bei 2.29.

---

# RE1.5 Port — v0.3.89 (Early Preview)

**Die Karte ist nach deiner Vorgabe neu gebaut.**

Du hattest recht, und der Fehler lag im Ansatz, nicht in der Ausführung. Weg ist alles,
was ich schrittweise draufgebaut hatte, um zwei ineinandergreifende Raumformen
gleichzeitig an der Tür zu verheften *und* überlappungsfrei zu stellen. Das geht nicht.

Jetzt gilt genau, was du beschrieben hast:

* Ein Raum ist ein **Rechteck** — seine äußere Kollisionsbox.
* Ein Durchgang legt das nächste Rechteck mit seiner Wand **exakt auf die Wand** des
  vorigen. Tür auf Tür.
* Eine Treppe führt auf ein anderes Blatt, dort wird neu angesetzt.

Rechtecke kacheln immer — Lücken und Verrutschen können gar nicht erst entstehen.

| live gemessen an 102 Durchgängen | v0.3.88 | jetzt |
|---|---|---|
| Übergänge unter 4 px | 45 | **66** |
| Übergänge unter 8 px | 62 % | **82 %** |
| mittlerer Sprung | 8 px | **5 px** |

Im Generator fallen **91 von 101** Durchgängen exakt auf dasselbe Pixel. ROOM1120↔ROOM1130
und ROOM1130↔ROOM1140 klaffen **0 px**.

Zwei Ergänzungen, die spürbar tragen: welche Türen offen bleiben, hängt allein von der
Reihenfolge ab — es wird jetzt **jede Wurzel durchprobiert** und die beste Karte genommen;
und für jede offen gebliebene Tür wird probiert, einen der beiden Räume stattdessen an
*ihr* anzulegen.

**Was du sehen wirst:** die Räume überlappen sich stellenweise. Das ist gewollt — *„er
schließt exakt an die Wand an, überlappt die Wand also"*. Wo ein Rundweg im Türgraph
nicht schließt, überlappen zwei Rechtecke, statt eine Lücke zu lassen.

**Offen:** 18 von 102 Übergängen liegen über 8 px — die Ringe, die auch die Nachbesserung
nicht schließt.

---

# RE1.5 Port — v0.3.88 (Early Preview)

**Der Marker springt beim Durchgehen nicht mehr weg — und keine Tür verdeckt mehr eine
andere.**

Aufbauend auf v0.3.87 (live gegen die Engine debuggt) zwei weitere Sachen:

**Die Tür zieht den Marker an.** Ein Durchgang ist EIN Ort und trägt EIN Symbol — stehst
du in der Tür, sitzt dein Marker auf diesem Symbol, von beiden Seiten auf derselben
Stelle. Der Zug wächst stetig, je näher du kommst, es ruckt also nichts.

| gemessen an 102 Durchgängen | v0.3.86 | jetzt |
|---|---|---|
| mittlerer Sprung | 9 px | **5 px** |
| Übergänge unter 2 px | 0 | **26** |
| Übergänge unter 8 px | 41 % | **62 %** |
| schlimmster Sprung | 63 px | **35 px** |

**Zwei verschiedene Türen liegen nicht mehr aufeinander.** Seit die Räume
aneinanderstoßen, schnappten benachbarte Türen auf dasselbe Wand-Pixel — eine verdeckte
die andere. Sie werden jetzt längs ihrer Wand auseinandergeschoben: 10 Paare → 1, und
**fünf Türen, die vorher verdeckt waren, sind wieder da**. ROOM1130 und ROOM1140 klaffen
statt 9 px nur noch 1 px.

**Offen bleibt:** 38 von 102 Übergängen liegen über 8 px. Der Grund ist gemessen und
nicht behebbar: die Räume von RE1.5 bilden keinen metrisch geschlossenen Grundriss — wo
ein Rundweg im Türgraph nicht schließt, lässt sich der Fehler nur verteilen. Auf
Blättern ohne solche Rundwege geht er auf exakt null.

---

# RE1.5 Port — v0.3.87 (Early Preview)

**Live gegen die Engine debuggt, wie du es verlangt hast.**

Dein Auftrag war richtig: meine bisherigen Prüfungen liefen auf der erzeugten Tabelle
und bildeten die Marker-Rechnung nur *nach*. Neu ist ein Werkzeug, das den **echten
Code-Pfad** fährt — jeder Raum wird geladen wie im Spiel, die Türen kommen aus den
installierten Daten, und für beide Seiten läuft dieselbe Funktion, die auch den
Spielermarker zeichnet.

Erste Live-Messung: 116 Durchgänge, **schlimmster Sprung 63 Pixel, kein einziger unter
zwei**. Vier Ursachen, die nur live sichtbar waren:

| | vorher | jetzt |
|---|---|---|
| schlimmster Sprung beim Durchschreiten | **63 px** | **29 px** |
| Türsymbole, die vom Nachbarn weg zeigen | 34 von 93 | **0 von 95** |
| Durchgänge, die anstoßen | 92 von 100 | **96 von 101** |

**Der Marker wurde von der Wand weggeschoben.** Er wird mit vier Pixeln Sicherheitsabstand
ins Rechteck geklemmt. Bei den gemalten Rechtecken des Originals ist der Rand bloßer
Rahmen; auf einem Grundriss ist er die **Wand** — und genau dort stehst du, wenn du durch
eine Tür gehst. Das allein machte bis zu acht Pixel Sprung aus, völlig unabhängig davon,
wie genau die Räume liegen.

**Einseitige Türen wurden beim Ausrichten übergangen.** Der schlimmste Fall überhaupt
(63 Pixel) war so einer.

**Meine Wanddicke war zehnmal zu groß.** Ich hatte sie gemessen, indem ich vom Türpunkt
nach außen lief — bei großen Räumen läuft dieser Strahl erst tief hinein. Die Tiefe steht
aber in den Türdaten selbst.

**Ehrlich: der mittlere Sprung bewegt sich nicht** (9 → 10 Pixel). Getrennt gemessen sind
davon 0,9 Pixel unvermeidbare Geometrie und 9 Pixel Layout. Das Ausrichten konvergiert
nachweislich — auf Blättern **ohne Rundwege** im Türgraph geht der Fehler auf exakt null.
Auf Blättern mit Rundwegen bleibt er stehen: **die Räume von RE1.5 bilden keinen metrisch
geschlossenen Grundriss.** Jeder Raum hat sein eigenes Koordinatensystem, und die
Türdaten sagen, *wohin* eine Tür führt — nicht, dass die Räume kacheln. Wo ein Rundweg
nicht schließt, lässt sich der Fehler nur verteilen, nicht beseitigen.

Ich habe fünf Einstellungen gemessen und die mit den besten sichtbaren Werten genommen.
Der Preis ist mehr Überlappung (bis 31 % auf 2F). Im Bild liest sich das anders als in
der Zahl: überlappende Räume in derselben Farbe wirken *verbunden*, Lücken wirken kaputt.

---

# RE1.5 Port — v0.3.86 (Early Preview)

**Die Kartenstücke stoßen aneinander, und die Türen zeigen in die richtige Richtung.**

Dein Bericht war in allen drei Punkten berechtigt. Gemessen an v0.3.85:

| | vorher | jetzt |
|---|---|---|
| Türsymbole, die vom Nachbarn **weg** zeigen | 34 von 93 (37 %) | **0 von 95** |
| größte Lücke zwischen Räumen mit gemeinsamer Tür | 55 px | **14 px** |
| größtes Verrutschen längs der gemeinsamen Wand | 72 px | **41 px** |
| Durchgänge, die anstoßen | 92 von 100 | **95 von 101** |

**Warum die Stücke klafften.** Der Löser heftete jeden Raum an *genau einen* Nachbarn.
Jede weitere Tür — jeder Rundweg im Türgraph — war nur ein weicher Zug, den er beim
ersten drohenden Überlapp aufgab. Bei 126 Orten und 205 Türen sind rund 80 Türen nie
eingehalten worden; genau die trugen die 30–70-Pixel-Lücken. Jetzt werden **alle
Durchgänge zugleich** ausgeglichen.

**Warum die Türen verdreht waren.** Die Wandseite kam daraus, wohin der Raum weitergeht.
Das benennt die *Wand*, in der die Tür sitzt — nicht die *Seite*, die zum Nachbarn
schaut. Bei einem Durchgang ist die Richtung bekannt und schlägt jetzt die Form.

**⛔ Mein bisheriges Maß war zu schwach.** „92 von 100 Durchgängen berührend“ erlaubte
2 Pixel Toleranz auf *Flächen* und sagte gar nichts über die Stelle, an der du im
nächsten Raum erscheinst. Deshalb sahst du Fehler, die meine Zahlen nicht zeigten.

**Ehrlich zum Preis:** die Überlappung ist gestiegen (0–4 % auf 0–16 % je Blatt). Räume,
die eine Tür teilen, dürfen sich jetzt um einen Streifen überlappen — sonst entsteht
genau die Lücke, die du gemeldet hast. Am dichtesten ist es auf dem Factory-Blatt.

Und: der Marker springt beim Durchgang weiter um etwa neun Pixel. Davon ist der größte
Teil die **echte Wanddicke** — die beiden Tür-Datensätze des Originals liegen real so
weit auseinander, je einer auf seiner Seite der Wand. Das lässt sich nicht wegrechnen,
ohne die Geometrie zu fälschen.

---

# RE1.5 Port — v0.3.85 (Early Preview)

**Alle 13 Kartenblätter, alle Räume, jedes Blatt geprüft.**

Du wolltest im Ganzen sehen, ob der Ansatz taugt. Dazu sind die restlichen Punkte
erledigt und jedes einzelne Blatt gemessen.

| | v0.3.84 | jetzt |
|---|---|---|
| Orte mit einer Zeichnung | 112 | **126** |
| Durchgänge, die aneinanderstoßen | 85 von 89 | **92 von 100** |
| Überlappung je Blatt | 0 – 11,9 % | **0 – 4,0 %** |

**Ein Raum wird jetzt auf jeder Etage gezeichnet, die er erreicht.** Treppenhaus,
Fahrstuhl und der Bereich hinter ROOM1170s Treppe bekommen auf jedem ihrer Blätter eine
eigene, richtig gesetzte Zeichnung — vorher wurde ihr Grundriss notdürftig in ein
Rechteck des Original-Künstlers eingepasst. Nebenwirkung: die losen Inseln auf
**Police Station B2** haben damit ihren Anker bekommen und hängen jetzt zusammen
(9 von 9 auf **11 von 11** Durchgänge).

**Blätter, die sich einen Raum teilen, werden gemeinsam gelöst.** Die Stockwerke der
Polizeiwache wachsen um dieselben Anker, ein geteilter Raum liegt überall gleich herum.
Police Station 2F ist dadurch von 20,6 % Überlappung auf **0,5 %** gefallen.

**Was ich probiert und wieder verworfen habe:** ein gemeinsamer Maßstab für das ganze
Gebäude. Damit stapeln die Stockwerke exakt übereinander — aber alles wird um Faktor 3
kleiner, ROOM1120 fällt von 34×36 auf 14×15 Pixel und das Dach auf einen Kasten in einem
leeren Blatt. Unsere Stockwerke sind nicht deckungsgleich, und danach hattest du auch nie
gefragt. Eingepasst wird deshalb weiter je Blatt — die Blätter füllen ihr Feld.

**Geprüft über alle 13 Blätter** (`tools/karte_audit.py`): kein Blatt verlässt das
Kartenfeld, alle 186 Tür- und Treppensymbole liegen auf ihrem Raum, kein Symbol wird
doppelt gezeichnet, und jede Etagen-Zweitzeichnung hat ihr eigenes Besucht-Bit.

**Offen:** Factory zeigt 14 von 18 Durchgängen berührend, 3F 5 von 7 — dort schließen
sich Ringe im Türgraph geometrisch nicht. Und einzelne Räume ohne Tür auf ihrem Zielblatt
stehen abseits (2F oben links, Laboratory B4).

---

# RE1.5 Port — v0.3.84 (Early Preview)

**Die Karte ist neu gebaut — als Grundriss, nicht mehr als Kachel-Puzzle.**

Du hattest recht mit deinem Modell: *"ein neues Kartenstück schließt in der Map genau
da an, wo man den Raum davor durch die Tür verlassen hat."* Genau so wird die Karte
jetzt gerechnet. Jeder Ort wird aus seinen echten Kollisionsdaten gezeichnet und über
den Tür-Datensatz an seinen Nachbarn geheftet. Die **Original-Tür- und
Treppensymbole** sitzen weiterhin auf den Wänden — nur liegen sie jetzt auf einer
Zeichnung, die zum Raum passt.

| | vorher | jetzt |
|---|---|---|
| Orte mit einer Zeichnung | 33 von 112 | **112 von 112** |
| Durchgänge, die aneinander stoßen | — | **85 von 89** |

Der Fall aus deinem Bericht — *"im Room 1120 lande ich plötzlich auf der falschen
Seite, wenn ich vom ROOM 1130 aus reinkomme"*: der Punkt, an dem du in ROOM1120
erscheinst, liegt jetzt **2 Pixel** neben der Tür, durch die du gekommen bist. Zur
Gegenprobe: die andere Tür desselben Raums liegt 21 Pixel weg — es fällt also nicht
einfach alles auf einen Punkt zusammen.

**Was dabei sonst noch schiefstand und jetzt stimmt**

* Die Kollisionsdaten beschreiben den **begehbaren Weg**, nicht den Raum. ROOM1120
  besteht aus elf schmalen Streifen — dem Gang um die Möbel herum. Ungefüllt sah ein
  Büro aus wie ein Labyrinth. Eingeschlossene Flächen werden jetzt geschlossen.
* Ein Raum ist nicht immer **ein Ort**. Vorher wurden alle Bereiche eines Raums zu
  einem Klumpen: die Fahrstuhlkabine war doppelt so groß wie in Wirklichkeit, und
  ROOM1140s zwei Bereiche trugen dieselbe Zeichnung.
* Ein Kartenblatt zerfällt oft in **mehrere getrennte Gruppen** von Räumen. Bisher
  wurde nur die erste gesetzt — auf einem Blatt 3 von 11 Räumen. Jetzt liegen alle.
* Die Umrandung folgt der **Raumform**, nicht mehr einem Kasten drumherum. Der Kasten
  verdeckte die Form und ließ Nachbarräume übereinanderliegen, obwohl sie es nicht tun.

Das ist ausdrücklich eine **Abweichung vom Original**, die du am 01.09. freigegeben hast:
die handgemalte Karte des Prototyps ist unfertig (nur 33 von 72 Kartenräumen tragen
überhaupt eine Maßstabszeile) und nicht maßstabsgetreu zur Spielwelt. Die
**Symbole** sind original, die **Anordnung** ist gerechnet.

---

# RE1.5 Port — v0.3.69 (Early Preview)

**Etagenwechsel hinter der Treppe.** Ein Raum kann über zwei Ebenen reichen — ROOM1170s
zweiter Bereich liegt hinter der Treppe und gehört zur Ebene von ROOM1150. Die Karte
folgt jetzt dem **Bereich**, in dem du stehst, statt der Raumnummer; nach dem Treppenlauf
schaltet sie auf die richtige Etage um. Das passende Rechteck dort trägt denselben
Grundriss-Ausschnitt wie auf der Etage darüber — genau so, wie die Karte Treppenhäuser
auf beiden Ebenen zeigt.

**Treppen sind wieder eingezeichnet — diesmal mit belegten Positionen.** Sie stammen aus
den Zonen, über die der Spieler tatsächlich die Ebene wechselt, und laufen durch dieselbe
Projektion wie der Positionsmarker, inklusive der gespiegelten z-Achse. Genau die fehlte
vorher — deshalb saßen die alten Marken daneben.

Die **Tür-Striche bleiben draußen**: Ihre Positionen sind weiterhin nicht belegbar.

---

# RE1.5 Port — v0.3.68 (Early Preview)

**Der Marker lief senkrecht verkehrt herum — jetzt belegt und behoben.**

Die Markerformel des Originals **negiert die z-Achse**: Läuft man im Spiel nach hinten,
wandert der Punkt auf der Karte nach oben. Meine Bereichsprojektion tat das nicht — im
Code stand sogar ausdrücklich das Gegenteil. Genau das hast du gesehen.

Gegenprobe gegen die Original-Formel, die für den großen Bereich funktioniert:

| Punkt | Original | jetzt | vorher |
|---|---|---|---|
| Tür nach 1130 | (175,103) | (174,105) | (174,152) |
| Richtung 1140 | (154,139) | (158,138) | (158,119) |

Die alten Werte lagen an der Spiegelachse verkehrt — das erklärt die falsche
Laufrichtung.

**Zum kleinen Bereich:** Er bleibt unsere Korrektur eines Original-Fehlers (das Spiel
selbst legt ihn außerhalb beider Rechtecke ab). Falls er dort weiterhin danebensteht,
habe ich schon den nächsten Ansatzpunkt gemessen: Der gezeichnete Grundriss füllt sein
Rechteck nur zur Hälfte aus, der Marker kann also innerhalb des Rechtecks auf einer
leeren Stelle landen. Das ließe sich auf die tatsächlich gezeichnete Fläche einschränken.

---

# RE1.5 Port — v0.3.67 (Early Preview)

**Positionsmarker springt nicht mehr ins falsche Rechteck.**

Ursache gemessen: Die Bereichsgrenzen sind Rechtecke um unregelmäßige Grundrisse und
decken sie nicht lückenlos ab — zwischen den beiden Bereichen von ROOM1170 liegt ein
Streifen, der in keinem von beiden liegt. Dort fiel die Zuordnung pauschal auf den
*ersten* Bereich des Raums zurück, bei 1170 also immer auf den großen. Beim Durchgehen
sprang der Marker deshalb in beide Richtungen ins falsche Rechteck. Jetzt gewinnt der
Bereich, dessen Grenze am **nächsten** liegt.

**Ein Befund zum Mitnehmen:** Die Markerformel des Originals legt den *zweiten* Bereich
von 1170 selbst außerhalb beider Rechtecke der Kartenseite ab — dort ist die Karte im
Original kaputt, wie du vermutet hattest. Unsere Bereichsprojektion setzt ihn stattdessen
in das obere Rechteck. Ob das die gemeinte Stelle ist, kann ich nicht belegen: Die Seite
hat nur zwei Rechtecke, und beide zeigen auf echte Grundrisse. Wenn du siehst, wo er
hingehört, sag es mir — dann ziehe ich es nach.

---

# RE1.5 Port — v0.3.66 (Early Preview)

- **Abbruchton auf der Karte kommt jetzt sofort.** Er stand am Ende der Ausblendung und
  kam dadurch hörbar zu spät — jetzt beim Tastendruck.
- **Spielermarker im Nirgendwo (2. Abschnitt von 1170).** Ursache war eine
  Ungleichbehandlung: Der Marker nutzt einen Ersatzweg, wenn der Bereich noch nicht
  nachgeführt ist (direkt nach einem Bereichswechsel), die Einfärbung des Rechtecks aber
  nicht. Der Marker wurde dann gezeichnet, sein Rechteck aber als unbesucht behandelt und
  gar nicht gemalt — er schwebte über Schwarz. Beide nehmen jetzt denselben Weg.

Zur Einordnung, was ich dabei überprüft habe: Die Bereiche von ROOM1170 stimmen mit der
Raumgeometrie überein (die begehbaren Flächen zerfallen in genau zwei zusammenhängende
Teile — exakt die eingetragenen Bereiche), beide Kartenrechtecke zeigen auf echte
Grundrisse, und die Markerpositionen liegen rechnerisch alle in ihren Rechtecken.

---

# RE1.5 Port — v0.3.65 (Early Preview)

- **Abbruchton beim Verlassen der Karte** ergänzt — der Ausstieg war als einziger
  Menüweg noch stumm.
- **Die gelben Striche auf der Karte sind raus.** Das waren meine abgeleiteten Tür- und
  Treppenmarken. Sie standen auf unsicherem Grund: aus dem Türgraphen hergeleitet und
  über eine Formel auf die Kartenblätter projiziert, deren Raum-Zuordnung nur teilweise
  belegt ist — und RE1.5 zeichnet auf seinen Blättern selbst weder Türen noch Treppen.
  Es waren also erfundene Striche an ungesicherter Stelle. Lieber nichts zeichnen als
  etwas Falsches; der Unterbau bleibt, falls die Zuordnung später belastbar wird.

---

# RE1.5 Port — v0.3.64 (Early Preview)

**Menütöne: eine Ursache für alle Stellen.**

Die Sound-Pumpe — die Funktion, die angestoßene Töne weiterführt — lief **nur in der
Spielschleife**. Titel, Optionen, Speicher- und Ladebildschirm haben eigene Schleifen und
blieben deshalb stumm, obwohl dort teilweise schon Ton-Aufrufe standen (im Titelmenü etwa
war der Bewegungston längst im Code, er wurde nur nie hörbar). Die Pumpe läuft jetzt in
allen neun Menü-Schleifen.

Ergänzt, wo tatsächlich kein Aufruf stand:

- **Optionen**: Bewegung, Auswahl und Abbruch — für alle Unterbildschirme.
- **Inventar-Reiter** (Karte / Gegenstände / Akten): Bewegung und Auswahl.
- **Inventar öffnen**: Auswahlton.

Die **Töne selbst sind die originalen** aus der Sound-Bank des Spiels — dieselben, die das
Inventar an seinen übrigen Stellen längst spielt. Ergänzt habe ich nur die *Stellen*: An
diesen Bildschirmen ist RE1.5 selbst stumm (ich habe die gesamte ausführbare Datei
durchsucht — 41 Ton-Aufrufe, keiner davon im Titel, im Reiterwechsel oder in den
Optionen). Das ist im Code und im Test ausdrücklich als Ergänzung gekennzeichnet.

---

# RE1.5 Port — v0.3.63 (Early Preview)

**Je ein Pixel an beiden Kanten nachgezogen.** Box-Seite jetzt 25–219: Panel 25–181,
Textbox 181–219 — die zwei Pixel kommen wieder ausschließlich aus dem Innenraum der
Textbox, die Ränder bleiben unangetastet.

---

# RE1.5 Port — v0.3.62 (Early Preview)

**Box-Seite jetzt exakt auf den Rahmenkanten des ITEM-LIST-Panels.**

Ich habe in deinem neuen Bild beide Panels an ihren Kanten vermessen: Das ITEM-LIST-Panel
reicht von Spielzeile **26,7 bis 218**, meine Box lag bei **27,7 bis 215** — also ein
Pixel zu tief oben und drei zu kurz unten. Die roten Markierungen der letzten Runde lagen
etwas *innerhalb* der Kanten, daher der Restversatz.

Korrigiert: Panel eine Zeile höher, Textbox vier Pixel höher — wieder nur im Innenraum,
die Ränder bleiben unangetastet. Nachgemessen: Panel 26–182, Textbox 182–218, zusammen
**26–218**.

---

# RE1.5 Port — v0.3.61 (Early Preview)

**Box-Panel und Textbox füllen jetzt exakt die Höhe des ITEM-LIST-Panels.**

Dein Screenshot mit den roten Markierungen war der entscheidende Hinweis — ich habe sie
ausgemessen: Sie liegen im Spielbild bei Zeile **27** und **215**, der Bereich ist also
188 Pixel hoch. Mein Fehler davor: Ich hatte die **Gitterzellen** als Bezug genommen
(58–208), aber der Rahmen des ITEM-LIST-Panels reicht deutlich weiter — deshalb saß
meine Box immer zu tief.

Aufgeteilt wie von dir vorgeschlagen: Das **Panel behält seine Originalhöhe** (27–183),
die **Textbox wurde angepasst** (183–215) — und dort nur der leere Innenraum, die
Original-Ränder oben und unten bleiben vollständig.

Nachgemessen: Panel 27–183, Textbox 183–215. Bündig.

---

# RE1.5 Port — v0.3.60 (Early Preview)

**Box-Panel liegt jetzt auf der Höhe des Item-Gitters.** Ich habe nachgemessen statt
geschätzt: Die Zellen des Gitters liegen bei Bildzeile 58–208, mein Panel stand bei
16–172 — also deutlich darüber.

Dabei kam ein Größenkonflikt heraus: Panel und Textbox brauchen zusammen 215 Pixel, der
Bereich des Inventars ist kleiner. Gelöst, **ohne einen Rand zu opfern**: Die Textbox ist
kürzer, aber nur ihr **leerer Innenraum** wurde schmaler — die Original-Ränder oben und
unten sind vollständig erhalten, dazwischen bleibt Platz für die Textzeile.

Ergebnis: Panel deckt denselben Bereich ab wie das Gitter, die Textbox schließt direkt
darunter an, und beides steht im Bild.

---

# RE1.5 Port — v0.3.59 (Early Preview)

- **Die Textbox-Ränder** waren im Bild vorhanden — ich habe nachgemessen — lagen aber
  unten aus dem sichtbaren Bereich heraus. Das Panel rückt sechs Pixel höher, damit die
  Box komplett im Bild steht.
- **Die EXIT-Zeile ist raus.** Der Ausstieg über Abbrechen funktioniert unverändert.

---

# RE1.5 Port — v0.3.58 (Early Preview)

Zwei Ursachen für den Text außerhalb der Textbox — beide gefunden:

- **Ich hatte die falsche Stelle geändert.** Es gibt zwei Stellen, an denen der Item-Name
  gezeichnet wird: eine im Box-Bildschirm, eine im normalen Statusbildschirm. Ich hatte
  die *normale* auf die Textbox-Position gesetzt und die im Box-Bildschirm unverändert
  gelassen — dort stand weiter die feste Grundlinie, also **oberhalb** der Box. Jetzt
  genau umgekehrt.
- **Die Textbox war zu kurz geschnitten.** Ihr unterer Rahmen liegt im Original tiefer,
  als ich beim Ausschneiden angenommen hatte — deshalb fehlten die Ränder. Neu
  geschnitten, jetzt komplett mit allen vier Rändern; das Panel rückt dafür vier Pixel
  höher, damit die größere Box unten hineinpasst.

Die Grundlinie des Namens habe ich ebenfalls im Original abgemessen statt geschätzt.

---

# RE1.5 Port — v0.3.57 (Early Preview)

**Panel und Textbox sind jetzt getrennt** — das war die eigentliche Ursache.

Bisher steckten beide in **einem** Bild. Damit war die Höhe starr: Schob ich das Panel
nach unten, damit die Oberkante frei wird, rutschte die Textbox aus dem Bild — und der
Item-Name landete auf dem Rahmen statt darin.

Jetzt sind es zwei Teile, beide aus dem Original geschnitten:

- **Listen-Panel** sitzt auf der **Höhe des Item-Gitters** rechts, wie von dir
  vorgeschlagen.
- **Textbox** liegt eigenständig darunter, mit ihren Original-Rändern rundum.
- Der **Item-Name landet in der Textbox**.
- Die Innengeometrie (Zeilen, Scrollschiene) wandert automatisch mit dem Panel mit —
  keine handgezählten Abstände mehr, die beim nächsten Verschieben wieder brechen.

---

# RE1.5 Port — v0.3.56 (Early Preview)

**Panel wieder etwas höher.** Die 13 Pixel, mit denen ich es unter den oberen Kino-Balken
geschoben hatte, waren zu viel — der Text rutschte unten aus der Box. Mit 7 Pixeln bleibt
die Oberkante samt orangem Pfeil frei, und die Zeilen sitzen wieder im Listenfeld.

---

# RE1.5 Port — v0.3.55 (Early Preview)

Deine vier Punkte zur Box:

- **„…/64" unten ist raus.** Der Scrollgriff zeigt die Position — im Original steht dort
  nichts.
- **Der Zeilenbalken schnitt die Mengenangabe an.** Die Ziffern saßen genau auf der
  Zeilenoberkante; jetzt sitzen sie 5 Pixel tiefer, mittig in der Zeile.
- **Die abgeschnittene Oberseite** lag daran, dass RE2 sein Panel bei Bildzeile 13
  ansetzt — bei uns liegt dort der obere Kino-Balken darüber. Panel und Inhalt sind um
  13 Pixel nach unten gerückt, damit Rahmen und oranger Pfeil frei stehen.
- **Der Textrahmen unten ist jetzt dabei.** Ich habe das Panel neu aus dem Original
  geschnitten — die Region reicht jetzt bis unter das Namensfeld.

---

# RE1.5 Port — v0.3.54 (Early Preview)

**Die Item-Box zeigt jetzt das echte RE2-Panel — nicht mehr meinen Nachbau.**

Ich habe bisher Rahmen, Schriftzug, Pfeile und Schiene *nachgebaut* und dabei in jeder
Runde ein Detail verfehlt. Das war der falsche Ansatz. Jetzt wird **das Original
gezeichnet**: Ich habe es aus dem laufenden Spiel geschnitten (aus dem Speicherabzug,
den du mir mit der geöffneten Box ermöglicht hast) und liefere es als Bild mit.

Damit stehen im **Original-Aussehen**: Rahmen und Pfosten, der **„ITEM BOX"-Schriftzug
mit den grauen Pfeilen**, Kopf- und Fußband, die **Scrollbalken-Schiene samt ihren
orangen Pfeilen** und der Listengrund.

Darüber liegt unser eigener Inhalt: Namen, Symbole, Mengen, der Auswahlbalken und der
gelbe Scrollgriff, dessen Position anzeigt, wo man sich in den 64 Plätzen befindet.

Das Panel-Bild ist als Asset im Paket, und der Paketbau prüft es mit — sonst wäre es
still weg.

---

# RE1.5 Port — v0.3.53 (Early Preview)

Deine vier Punkte zur Box — alle geklärt, drei davon hatten dieselbe Ursache.

- **„ITEM BOX"-Schriftzug und die grauen Pfeile** standen im Zeichenablauf **hinter** den
  Panel-Flächen — und weil ein früherer Befehl oben liegt, hat das Kopfband sie
  zugedeckt. Jetzt stehen sie davor. Zusätzlich habe ich die **Zeichen-Kodierung am
  Spiel selbst verifiziert** statt sie anzunehmen (aus „COMBAT KNIFE" abgelesen): Mein
  Leerzeichen war falsch kodiert und erzeugte ein Müll-Zeichen.
- **Orange Pfeile waren schräg.** Ich habe die Form im Original ausgezählt: ein
  **kompaktes Dreieck** mit den Breiten 2/4/4/6/6 Pixel — vorher eine flache Schräge.
- **Items und Mengen überdeckten die Zeilenmarkierung.** Der Auswahlbalken stand hinter
  den Zeileninhalten und lag damit unter ihnen. Jetzt läuft er durchgehend darüber.

---

# RE1.5 Port — v0.3.52 (Early Preview)

**Warum das Labor-Bild „schon wieder" da war — und es lag nicht an der Logik.**

Der Build der betroffenen Datei ist mehrfach **still fehlgeschlagen**: Meine eigenen
Prüfprogramme hielten die Bibliothek offen, der Compiler kam nicht dran, und mein
Fehlerfilter hat die Meldung nicht erkannt. Die **alte** Fassung lief also weiter — meine
Korrektur war im Quelltext, aber nie im Programm. Ich prüfe den Build jetzt am
Rückgabewert statt am Text, und ich habe ein **Messwerkzeug** eingecheckt, das die
Bildfolge durchspielt und für jedes Bild sagt, ob es sichtbar wird. Vorher: Bild 7
sichtbar. Jetzt: bleibt dunkel, das Logo erscheint zuerst, das Labor kommt darunter.

**Item-Box — deine Punkte, alle mit Farben aus dem laufenden RE2 gemessen:**

- **Scrollbalken** ist jetzt ein **gelber Griff**, der die Position im Ring zeigt, mit
  **orangen Pfeilen** oben und unten (gemessen: Griff 232/248/72, Pfeile 240/56/24).
- **„ITEM BOX"-Schriftzug** im Kopfband, mit den beiden grauen Dreiecken daneben.
- **Abgeschnittene Item-Bilder:** Die Kacheln wurden **beschnitten statt skaliert** — bei
  25×19 Zielgröße war nur die linke obere Ecke der 40×30-Kachel zu sehen. Jetzt wird
  skaliert.
- **Mengen-Zahlen** ragten in die nächste Zeile (ihre Grundlinie lag 20 Pixel tiefer, bei
  20 Pixel Zeilenabstand). Jetzt stehen sie in ihrer eigenen Zeile neben dem Symbol.
- **Panel nutzt den Platz:** 192 statt 145 Pixel breit — bis kurz vor das Item-Gitter,
  praktisch die Originalbreite (190).

---

# RE1.5 Port — v0.3.51 (Early Preview)

**Die schwarze Box — echte Ursache gefunden, und diesmal habe ich nachgesehen statt
geraten.** Ich habe mir einen Abzug unseres *eigenen* Box-Bildschirms erzeugt und ihn
Pixel für Pixel geprüft: Der Auswahlbalken war gar nicht da — an seiner Stelle stand die
Grundfarbe. Der Grund: Die Zeichenbefehle werden **von hinten nach vorne** abgearbeitet,
ein früher Befehl liegt also **oben**. Mein Panel stand bei den Rahmen (früh), die Liste
erst später — der Panel-Hintergrund hat damit **Namen, Symbole und Auswahlbalken
zugedeckt**. Das Panel liegt jetzt hinter der Liste; in der Gegenprobe sind Balken,
Scrollbalken und Zeileninhalte sichtbar.

**Pre-Intro: das Labor-Bild kommt nicht mehr vorher.** Auch hier hattest du recht. Im
Original räumt das Spiel den Schirm, dann erscheint **das Logo allein auf Schwarz** und
zoomt, und **erst 96 Frames später** legt sich das Labor-Standbild darunter. Es tritt
also nie für sich auf. Der Port hält es jetzt genauso zurück.

---

# RE1.5 Port — v0.3.50 (Early Preview)

**1. Die dunklen Felder — Ursache gefunden.** Das war kein Farbfehler, sondern ein
Zeichenfehler von mir: Ich habe die Panel-Flächen über einen Operator gemalt, der
**additiv in 5 Bit** arbeitet. Eine gemessene Farbe kommt darüber nur als Sechzehntel
an — praktisch schwarz, und mehr als halbe Helligkeit ist damit überhaupt nicht
erreichbar. Deshalb war auch der Auswahlbalken unsichtbar. Es gibt jetzt einen
**deckenden** Operator; Panel, Auswahlbalken (in RE2s Rot), Scrollbalken-Marken (in
RE2s Gelb) und Pfeile laufen darüber und stehen in den gemessenen Farben.

**Das Item-Gitter rechts ist unangetastet** — es hat seine eigene Basis, seinen eigenen
Rahmen und Cursor; dort wurde nichts geändert.

**2. Umbrella-Logo: Reihenfolge korrigiert.** Du hattest recht — es zoomt **zuerst auf
schwarzem Grund** hinein, und **erst danach** kommt das Labor-Standbild darunter, während
das Logo weiterzoomt. Genau so steht es im Original: Das Logo wird gesetzt, und das
Standbild folgt **96 Frames später**. Bei mir stand das Labor von Anfang an — die
Reihenfolge war vertauscht.

---

# RE1.5 Port — v0.3.49 (Early Preview)

**Der Box-Bildschirm ist jetzt aus dem laufenden RE2 gemessen.**

Danke fürs Öffnen der Box im Emulator — damit hatte ich endlich das Original vor mir.
Ich habe aus dem Speicherzustand den **Bildspeicher** ausgelesen und den fertig
gezeichneten Bildschirm Pixel für Pixel vermessen. Das Referenzbild liegt im Projekt
(`analysis/itembox_re2/referenz/`).

Übernommen mit den echten Werten:

- **Rahmenleisten** oben und unten mit ihrem Grauverlauf, **Kopf- und Fußband**, die
  **Pfosten** an den Seiten.
- **Listengrund** in RE2s Dunkelblau — und hier hat die Messung mich korrigiert: Ich
  hatte je Zeile einen dunklen Streifen gezeichnet, das Original hat einen
  **durchgehenden** Grund. Die Streifen sind raus.
- **Auswahlbalken**: zwei Linien an Ober- und Unterkante der mittleren Zeile über die
  volle Listenbreite, in RE2s Braun-Rot. Das bestätigt unabhängig, was ich aus dem
  Code rekonstruiert hatte.
- **Scrollbalken-Schiene** mit ihren Pfeilfeldern.
- Zeilenraster und feste Auswahl in der Mitte: ebenfalls bestätigt.

Die Höhen sind 1:1 übernommen; die Breite ist auf unsere linke Bildschirmhälfte
gestaucht (145 statt 190 Pixel), weil rechts unser Inventar-Gitter steht.

Nebenbei ist damit auch der Weg dokumentiert, wie ich künftig jede Frage zum
RE2-Aussehen beantworten kann, ohne zu raten: Speicherzustand ziehen, Bildspeicher
auslesen, nachmessen.

---

# RE1.5 Port — v0.3.48 (Early Preview)

**1. Umbrella-Logo: jetzt zoomt es ÜBER dem stehenden Labor-Bild.** Du hattest recht, und
mein Clear aus v0.3.47 war an der falschen Stelle. Im Original bleibt das Standbild
**stehen** und das Logo zoomt darauf — belegt: Das Standbild wird auf „einblenden"
gesetzt, ohne irgendetwas zu verdrängen, und das Logo blendet erst **165 Frames später**
aus. Beide sind die ganze Zeit gleichzeitig zu sehen. Genau so läuft es jetzt.

**2. Item-Box: die Symbole überlappen nicht mehr.** Das war ein klarer Fehler von mir —
RE2 zeichnet je Zeile ein **25×19**-Symbol (breite Waffen 49×19), das in den
20-Pixel-Zeilenabstand passt. Ich habe unsere 40×30-Kachel gemalt, also 30 Pixel hoch bei
20 Pixel Abstand: die Zeilen mussten sich überlagern.

**3. Item-Box: das Panel hat jetzt RE2s Original-Maße.** Ich habe die beiden
Geometrie-Tabellen des Originals ausgelesen **und ihre Zuordnung am Code belegt** (beide
Tabellen werden rückwärts durchlaufen, deshalb gehören die Einträge paarweise zusammen —
in v0.3.47 hatte ich sie falsch gepaart). Daraus die 16 Rahmenteile mit exakten Maßen:
obere und untere Leiste (je zweiteilig, 128×4 + 77×4), Pfosten 3×153, innere Senkrechte
2×142, Kopf- und Fußband, **Scrollbalken-Schiene 8×130**, Pfeilfelder, die L1/R1-Anzeigen
20×20 und die fünf Marken.

**Was weiterhin fehlt — und ich sage es klar:** Die *Bildquelle* der Rahmenteile. Sie sind
im Original Sprites **ohne eigenen Texturverweis** — sie erben ihn aus dem Speicherzustand
des Menüs. ST0/ST1 sind es nachweislich nicht: Ich habe das Panel mit den echten
Koordinaten aus ihnen gerendert und nachgesehen, dort liegen die Item-Symbole; außerdem
bringen sie nicht einmal die passende Farbtabelle mit. Ohne einen **Speicherauszug aus
laufendem RE2** ist die Quelle nicht bestimmbar. Bis dahin haben die Teile RE2s Form und
Maße, aber unsere Menüfarben als Füllung.

---

# RE1.5 Port — v0.3.47 (Early Preview)

**1. Umbrella-Logo blendet nicht mehr ins Labor-Bild.** In RE2 nachgesehen: Vor dem
Logo-Kapitel steht ein **harter Clear** — das Bild davor wird nicht ausgeblendet, sondern
ist schlagartig weg. Erst danach erscheint das Logo und zoomt hinein; das *folgende*
Standbild legt sich dann bewusst darüber (diese Überlagerung bleibt). Beim
S.T.A.R.S.-Abzeichen gibt es keinen Clear — dort setzt das Original Logo und Ausblenden
des Vorgängers absichtlich in denselben Moment.

**2. Box-Bildschirm: RE2s Panel-Aufbau.** Ich habe RE2s Box-Panel zerlegt — es besteht aus
25 Einzelteilen mit exakt bekannter Lage und Größe: obere und untere Leiste (jeweils
zweiteilig), Seitenpfosten, innere Senkrechte, Kopf- und Fußband, die **Schiene des
Scrollbalkens**, Pfeile oben und unten, und fünf Marken. Der Port zeichnet jetzt diesen
Aufbau statt des dünnen Umrisses von v0.3.45; die Scrollbalken-Marken sitzen in ihrer
Schiene.

**Was dabei ehrlich fehlt:** Die *Bildquelle* der RE2-Teile ist nicht übernommen. Die
Panel-Sprites erben in RE2 ihren Texturbereich aus dem Speicherzustand des Menüs — die
naheliegenden Dateien (ST0/ST1) sind es nachweislich nicht, ich habe sie mit den echten
Koordinaten offline gerendert und nachgesehen: dort liegen die Item-Symbole. Gezeichnet
wird der Aufbau daher in unseren Menü-Farben, nicht mit RE2-Texturen.

---

# RE1.5 Port — v0.3.46 (Early Preview)

**Pre-Intro: der Mechanismus, den ich übersehen hatte.**

Du hattest recht — und der Fehler saß tiefer als die Zuordnung der Effekte. In RE2 ist der
Zustand eines Bildes ein **Bitfeld**, und beim Übergang von „einblenden" auf „ausblenden"
kippt **nur das Blend-Bit — das Bewegungs-Bit bleibt gesetzt**. Dazu erscheint das nächste
Standbild schon *während* das Logo noch zoomt: zwischen dem Auftauchen des nächsten Bildes
und dem Beginn des Ausblendens liegen **110 Frames**, in denen beide gleichzeitig zu sehen
sind und das Logo durchgehend weiterzoomt.

Bei mir wurde die Bewegung beim Bildwechsel zurückgesetzt — daher „das Logo wird plötzlich
wieder groß". Jetzt trägt **jedes Bild seine eigene Bewegung und führt sie über den Wechsel
hinaus fort**, bis es ausgeblendet ist.

Die Zuordnung ist jetzt die, die du beschrieben hast:

| Bild | Bewegung |
|---|---|
| Zombie | Standbild |
| T-Virus | wandert von oben nach unten |
| S.T.A.R.S.-Abzeichen | zoomt heraus — **bis ins nächste Standbild hinein** |
| Villa, Helikopter, Gruppe, Labor | Standbilder |
| Umbrella-Logo | zoomt hinein — ebenfalls über den Bildwechsel hinaus |

---

# RE1.5 Port — v0.3.45 (Early Preview)

**Der Box-Bildschirm sieht jetzt aus wie in RE2** — das fehlte, damit die neue Bedienung
überhaupt Sinn ergibt.

Bisher steckte RE2s Ring-Mechanik (fester Auswahlrahmen, scrollender Inhalt) noch im
alten Symbol-Gitter — man konnte gar nicht sehen, was das Scrollen tut. Jetzt ist es
RE2s **Liste**:

- **Fünf Zeilen**, die mittlere ist die feste Auswahl (genau der Platz, den die
  Transfer-Logik nimmt), markiert durch ein helles Band.
- **Name links, Symbol und Menge rechts** in jeder Zeile — statt namenloser Kacheln.
- Die nicht gewählten Zeilen liegen auf dunklen Streifen, wie im Vorbild.
- **Scrollbalken am rechten Rand**: eine Marke je sichtbarem Platz, zwei Pixel pro
  Ring-Position — man sieht also, wo man sich in den 64 Plätzen befindet.
- Beim Scrollen gleiten die Zeilen weiter (die Kadenz aus v0.3.43 bleibt).

Die **Symbole bleiben unsere eigenen**: RE2s Icons zeigen RE2-Gegenstände — für die
Dinge aus RE1.5 wären sie schlicht falsch. Übernommen ist das Layout, das die Bedienung
lesbar macht.

---

# RE1.5 Port — v0.3.44 (Early Preview)

Nachbesserung auf deine drei Befunde zu v0.3.43.

- **Kartenmarkierungen schlugen ins normale Inventar durch.** Das war ein echter Fehler:
  Türen und Treppen wurden gezeichnet, ohne zu prüfen, ob überhaupt die Karte offen ist.
  Sie hängen jetzt am selben Riegel wie die Karte — **sichtbar erst, wenn du MAP drückst**.
- **Spielmarker im kleinen Raum (ROOM1170).** Der Marker wird jetzt **hart in sein
  Rechteck geklemmt** (mit Rand, weil er als kleines Quadrat um seinen Mittelpunkt
  gezeichnet wird), und er erscheint nur noch, wenn der Bereich, in dem du stehst, auch
  zu dem Kartenblatt gehört, das gerade angezeigt wird — sonst gar nicht, statt
  irgendwo.
- **Pre-Intro: jetzt exakt RE2s Choreographie.** Du hattest recht, ich hatte mich da
  ausgetobt. RE2 ist viel sparsamer: In der ganzen Sequenz gibt es **drei** Bewegungen —
  **genau ein** Standbild schwenkt, und **zwei Logos** bilden eine Zoom-Klammer (eines
  fährt zu, eines auf). Alle übrigen Bilder blenden nur. Genau so liegt es jetzt auf
  unseren neun Bildern: der Zombie-Kopf schwenkt, das S.T.A.R.S.-Abzeichen fährt zu,
  das Umbrella-Logo fährt auf, der Rest blendet. Die Geschwindigkeiten waren schon
  vorher aus RE2 übernommen und bleiben unverändert.

---

# RE1.5 Port — v0.3.43 (Early Preview)

Zwei große Baustellen: die **Item-Box läuft jetzt komplett nach Resident Evil 2**, und
die **Karte ist grundlegend repariert** (Türen, Treppen, Marker, Ebenen).

## Item-Box — der komplette RE2-Mechanismus

Bisher war unsere Box ein Zwitter: RE2s Transfer-Regeln, aber ein selbst erfundenes
Ablagesystem mit vier Seiten à acht Plätzen. Jetzt ist es RE2s echtes Modell:

- **64 Plätze statt 32** — doppelt so viel Stauraum.
- **Ring statt Seiten:** Der Auswahlrahmen steht fest, der Inhalt scrollt darunter durch
  (genau wie in RE2). Hoch/Runter bewegt um einen Platz, die **Schultertasten springen
  um fünf**.
- **Weiches Durchscrollen** statt Springen: Die Liste gleitet über sechs Bilder, dann
  rastet sie ein — RE2s exakte Kadenz. Bei gehaltener Taste läuft es gleichmäßig weiter.
- Statt „Seite 1/4" steht jetzt die Ringposition („12/64") am Feld.
- Vorhandene Spielstände laufen weiter: Inventar, Fortschritt, Wunden, Karte und die
  ersten 32 Box-Plätze werden übernommen.

Der alte Stand ist gesichert (Tag `itembox-hybrid-v1`), falls etwas fehlt.

## Karte — von Grund auf repariert

Dein Bericht war goldrichtig, und die Ursache war grundsätzlicher als gedacht:
**Ein Raum ist im Spiel nicht immer ein Ort.** ROOM1170 zum Beispiel besteht aus zwei
räumlich getrennten Bereichen, die über eine Tür verbunden sind, die auf den eigenen
Raum zeigt. Unsere Karte konnte das nicht abbilden — deshalb blieb der Marker unten im
großen Rechteck und das kleine wurde nie hervorgehoben. **26 der 103 Räume** sind so
gebaut.

- **Zonen statt Räume:** Jeder zusammenhängende Bereich bekommt jetzt sein eigenes
  Kartenstück und seine eigene Markerberechnung. Der Marker liegt damit **immer in dem
  Rechteck, das auch hervorgehoben ist**.
- **Der Wechsel wird sofort erkannt** — auch wenn man den Raum dabei gar nicht verlässt.
- **Ebenen stimmen:** Weil Zonen auf verschiedenen Kartenseiten liegen dürfen, wechselt
  die Karte beim Treppengang automatisch die Etage.
- **Türen und Treppen sind eingezeichnet** — 181 Marken, davon 22 Treppen. RE1.5 malt so
  etwas gar nicht, deshalb sind die Symbole nach RE2s Vorbild gebaut: Türen als kurzer
  Strich in RE2s Türgelb, Treppen als kleine Leiter. Sie erscheinen nur dort, wo du
  schon warst. Für ROOM1170 heißt das: **drei Türen und vier Treppen-Marken links im
  kleinen Rechteck** — genau da, wo du sie erwartet hast.

**Ehrlich dazu:** Welches Kartenstück zu welchem Bereich gehört, ist eine Optimierung
aus Grundriss, Größenverhältnis und Tür-Verbindungen — das Original gibt diese Zuordnung
nirgends her. Sie kann im Einzelfall danebenliegen; was nicht sicher zuzuordnen war,
bleibt neutral und färbt sich nie falsch. Sag mir, wo es noch hakt.

---

# RE1.5 Port — v0.3.42 (Early Preview)

Das Pre-Intro sieht jetzt aus wie in Resident Evil 2.

## Pre-Intro (die Standbild-Montage nach NEW GAME)

RE2 spielt praktisch dieselbe Erzähl-Montage aus Standbildern — nur deutlich schöner
inszeniert. Ich habe RE2s Präsentation auseinandergenommen (das Opening liegt dort als
eigenes Programm-Modul vor) und ihre Mechanik auf unsere Bilder übertragen:

- **Kein Hartschnitt mehr:** Die Bilder **glühen aus dem Schwarz auf** und wieder aus,
  über eine additive Helligkeitsrampe — exakt RE2s Kadenz (1,6 Sekunden je Richtung).
  Weil das alte Bild noch ausblendet, während das neue aufblendet, entsteht eine echte
  **Überblendung** statt eines Sprungs.
- **Die Bewegung, die du gesehen hast:** RE2 lässt einzelne Bilder ganz langsam
  durchwandern (1 Pixel alle 11 Frames) — dafür ist das Bild dort absichtlich größer als
  der Bildschirm. Unsere Bilder bekommen denselben Überhang und dieselbe Geschwindigkeit;
  in der Montage wandert das Helikopter-/Explosions-Bild.
- **Sanfter Zoom** auf drei Motiven mit Bildtiefe (Zombie-Kopf, Villa-Szene,
  Umbrella-Labor): 2 Pixel alle 4 Frames, um die Bildmitte zentriert — auch das ist
  RE2s Rate. Die Logos (S.T.A.R.S., Umbrella) stehen ruhig, wie im Vorbild.

**Unverändert byte-true bleibt alles Inhaltliche:** Bildreihenfolge, Standzeiten,
Erzähler-Texte samt Sprachausgabe, das Überspringen mit gehaltener ▢-Taste und der
Übergang zum Hubschrauber-Deck. Wer den Original-Hartschnitt sehen will:
`RE15_MONTAGE_STOCK=1`.

---

# RE1.5 Port — v0.3.41 (Early Preview)

Nachbesserung nach deinem Gegentest: die Karte ist jetzt aus den ECHTEN Raumdaten
gebaut, und die Gorilla-Bosse sind gross, treffbar und werfen wieder.

## Karte — aus den Raumdaten repariert

- **Neue Zuordnung aus der Kollisions-Geometrie:** Jeder Raum wird jetzt ueber seinen
  echten Kollisions-Grundriss (dieselben Daten, gegen die du beim Laufen klemmst) und
  den Tuer-Graphen (beide Tuerseiten tragen Positionen) seinem Kartenstueck zugeordnet
  — **63 von 120 Raeumen** sind jetzt verifiziert gefaerbt (vorher 28), jedes einzeln
  visuell am gerenderten Karten-Composite geprueft. Was nicht sicher belegbar war,
  bleibt bewusst neutral und faerbt sich NIE falsch (die Liste steht im Dossier —
  sag gern Bescheid, welche Raeume dir zuerst fehlen).
- **Positionsmarker repariert:** Die Beta liefert fuer 65 Raeume nur eine
  Platzhalter-Kalibrierung aus — der Stock-Marker zeigt dort stur in die Ecke. Fuer
  zugeordnete Raeume rechnet der Port jetzt reparierte Parameter derselben
  Original-Formel (32 Zeilen). `RE15_MAP_STOCK=1` zeigt weiter den byte-true
  Auslieferungszustand.

## Gorilla-Bosse (ROOM11C0) — vier Original-Mechanismen nachgeruestet

- **1,7x-Groesse:** Das Original rendert die Bosse mit Entity-Scale 1,7 (im Savestate
  nachgemessen) — der Port kannte den Mechanismus nicht. Jetzt skalieren Modell,
  **Angriffs-Reichweite** und **Krabbel-/Sprung-Tempo** wie im Original: gross,
  schnell, weite Schlaege.
- **Treffbarkeit:** Nach unten zielen trifft die Bosse jetzt (Hoehenband wie im
  Original: am Boden LEVEL+DOWN, im Sprung NUR UP — vorher war nach-unten ein
  stilles Vorbeischiessen, dieselbe Bug-Klasse wie beim Hund).
- **Koerper-Schub:** Der Boss wird jetzt pro Tick aus Leon herausgeschoben (vorher
  flog Leon — Mit-Ursache fuer das Clippen in unerreichbare Bereiche).
- **Der Wurf:** Der Griff spielt jetzt die Original-Choreo — EIN Wurf-Clip, Leon
  fliegt ab, steht mit SEINEN eigenen Aufsteh-Animationen auf und ist frei
  (vorher: endloser Halte-Loop am Boss).

---

# RE1.5 Port — v0.3.40 (Early Preview)

Das RE2-Kartensystem, uebertragen auf unsere Map (Nutzer-Wunsch 2026-08-30).

## Karte (MAP-Tab im Status-Screen)

- **Aktueller Raum ROT, besuchte Raeume GRUEN, unbesuchte SCHWARZ** — nach dem
  Mechanismus des RE2-Retail-Kartenzeichners (unbesuchte Raum-Stuecke werden gar nicht
  gezeichnet; der aktuelle Raum bekommt das stetige Highlight — bei RE2 blinkt im
  Map-Tab nur der Spieler-Pfeil, nicht der Raum; unser pulsierender Positionsmarker
  bleibt). Tueren/Durchgaenge sind bei RE1.5 in die Karten-Grafik der Raum-Stuecke
  eingemalt und faerben mit.
- **Besucht-Speicher wandert ins Savegame** (Save-Format v6; aeltere Staende laden
  weiter und starten mit leerer Karte).
- **Ehrliche Grenze:** Nur 28 der 120 Raeume liessen sich bisher belastbar ihren
  Karten-Rechtecken zuordnen (die Zuordnung existiert im Original nirgends; 65 Raeume
  haben dort nicht einmal eine funktionierende Marker-Kalibrierung — der Stock-
  Positionsmarker ist in diesen Raeumen defekt und zeigt auf die Ecke). Nicht
  zugeordnete Raum-Stuecke bleiben im bisherigen Neutral-Look und faerben sich nie
  falsch. Melde gern Raeume, deren Faerbung dir wichtig ist — die lassen sich gezielt
  nachziehen. Details: `analysis/nutzer_batch_2026-08-30b/map-zuordnung.md`.

---

# RE1.5 Port — v0.3.39 (Early Preview)

Der Nachfolge-Batch vom 2026-08-30: Weste ohne Heilung, schlafender Content aktiviert,
Item-Box ueberall erreichbar, und zwei Archaeologie-Antworten.

## Geaendert auf Wunsch

- **R.P.D.-Weste heilt nicht mehr.** Anlegen gibt nur noch +5 Energie, Ablegen nimmt +5
  (kann nie toeten). Die Vollheilung, die das Original beim Modell-Wechsel schreibt
  (HP:=100), ist bewusst nicht mehr uebernommen. Der "+1 Biss"-Deckel bleibt: mit Weste
  heilen Sprays auf 105.

## Schlafender Content — jetzt aktiv

- **ROOM1150 (Briefing-Raum): die verlorene Geraete-Szene.** An der Westwand gibt es
  jetzt einen Untersuchen-Punkt: ein verstecktes, voll choreographiertes Requisit faehrt
  mit Motor-Geraeuschen herab, arbeitet und faehrt wieder weg — die komplette Szene lag
  fertig im Skript, wurde aber von nichts im Spiel je aufgerufen. (Was das Geraet
  darstellt, wissen wir noch nicht sicher — schau es dir an!)
- **ROOM20A0 (Kanalraum der grossen Spinne): die deaktivierte Ambient-Effektschleife**
  (11 Emitter) laeuft jetzt ab Betreten des Raums.
- Bewusst NICHT aktiviert: Waisen-Texte, die einen Eingriff in die Skript-Bytes
  braeuchten (z.B. der geschnittene Muell-Pickup in 1030) — die RDTs bleiben byte-true.

## Item-Box

- **Untersucht: Hatte das Original eine deaktivierte Item-Box?** Als CODE: nein (Menue-
  System, sce-Handler und Save-Format komplett geprueft — kein Box-Code). Als CONTENT:
  ja, dreifach — die "not available in this preview"-Punkte sind in 14 von 16 Raeumen
  echte verdrahtete Interaktionspunkte, der Save-Block traegt vier ungenutzte
  Box-foermige Arrays, und (Neufund) **die Box-Bildschirm-GRAFIK ist ausgeliefert**:
  ein vorgerendertes "Item Storage / Item list"-Mockup liegt als Kamera-Hintergrund in
  allen 8 Box-Raeumen. Unsere RE2-portierte Box bleibt damit der richtige Weg.
- **ROOM6020 (Save-Raum STAGE6) hat jetzt auch eine erreichbare Box** — es war das
  einzige der 8 Box-Raum-Paare, dessen Box-Skript im Original ohne Trigger blieb.
  (Position des Untersuchen-Punkts ist eine Port-Wahl — Feedback willkommen.)
- **Das Mockup-Standbild ist raus:** beim Oeffnen der Box blitzte bisher fuer einen
  Wimpernschlag das vorgerenderte Platzhalter-Bild auf; die Kamera bleibt jetzt auf der
  Spielansicht. (Mit RE15_BOX_PREVIEW_MSG=1 gibt es weiterhin den byte-true
  Auslieferungszustand samt Meldung und Mockup.)

## Beantwortet

- **"Fehlt eine Marvin+Ada-Szene VOR der Gorilla-Szene?" — Nein.** Hart belegt: die
  Gorilla-Kammer ist eine Sackgasse, die nur ueber den Manhole-Raum erreichbar ist,
  dessen gemeinsame Szene wiederum am Sieg ueber BEIDE Gorillas haengt (einziger
  Flag-Schreiber). Leons "Wasn't he with you?" ist Off-Screen-Erzaehlung — und Ada
  beantwortet sie in derselben Szene: "He turned around all of a sudden to go look
  for you."

# RE1.5 Port — v0.3.38 (Early Preview)

Der komplette Nutzer-Batch vom 2026-08-29: neun Fixes, eine gewuenschte Erweiterung,
vier beantwortete Untersuchungen. Details je Punkt in
`analysis/nutzer_batch_2026-08-29/ERGEBNIS_v0.3.38.md`.

## Behoben

- **Item-Stapel**: Liegen mehrere Items uebereinander, braucht das zweite wieder einen
  EIGENEN Quadrat-Druck. Der Bestaetigungs-Druck des ersten Items lief bisher im selben
  Bild noch in die Aufhebe-Pruefung — die Modal-Maschine tickt jetzt wie im Original NACH
  dem Spieler-Schritt.
- **Hund, erster Treffer**: Der Hund traegt jetzt das Ziel-Hoehenband des Originals.
  Wer auf den niedrigen Hund NACH UNTEN zielt, trifft jetzt (vorher: stiller Fehlschuss —
  das war die gefuehlte Verzoegerung). Original-getreu dazu: ein sitzender (IDLE-)Hund ist
  NUR noch mit Nach-unten-Zielen treffbar, und jenseits von 4000 Einheiten gar nicht.
- **RE2-KI, Hunde-Tod**: Das finale laute Quieken (SE 7) spielt wieder bei jedem
  Schusswaffen-Kill. Die Todes-Variante wurde faelschlich nach der Verhaltensphase statt
  nach der Waffe gewaehlt — ein Kill waehrend Ruhe/Fressen/Kreisen war stumm.
- **Sherry**: Laeuft nach ihrer Verschwinde-Szene nicht mehr durch den Raum. Die
  NPC-Wand-Klemme des Originals sitzt jetzt an der Wurzel (fuer ALLE NPC-Zustaende) —
  die geparkte Sherry bleibt wie im Original hinter der Wand, unsichtbar.
- **ROOM1210, Arme anschiessen**: Treffer registrieren jetzt auch beim Nach-unten-Zielen
  auf die bodennahen Arme, und der Arm reagiert mit dem Original-Zucken samt Blut
  (der Flinch-Code existiert im Original vollstaendig — er war dort mangels Arm-HP nur
  nie erreichbar).
- **ROOM1210, Griff links**: Der Halte-Anker der West-Reihe lag 287 Einheiten IM
  Mauerwerk (Ost: 73) — der Griff zog Leon dorthin. Der Anker wird jetzt auf die
  begehbare Kante geklemmt; der Halt findet wie bei RE2s Fenster-Griff an der Wand statt.
  Bitte gegentesten — die Messsonde konnte das Live-Bild nicht vollstaendig nachstellen.
- **ROOM1090, letzte Kamera**: Das "Zeug ueber den Feuern" waren die Feuer-Effekte
  selbst: opak statt ADDITIV gezeichnet (deckende dunkle Glut-Fetzen), am Boden
  festgefroren statt auf den brennenden Truemmern reitend, und ohne das
  Groessen-Flackern. Alle drei Punkte laufen jetzt ueber die Original-Effektroutinen.
- **Affen (ROOM11C0/1141)**: Treffen wieder. Der Trefferpruefer ist jetzt das
  Original-Quadrat um den Angriffs-Knochen (Biss: Kiefer ±1000) — der alte Naeherungstest
  hatte ein garantiertes Fehlschuss-Band und einen erfundenen Winkel. Zusaetzlich wird
  der Pin-Wurf jetzt gegen die Waende geklemmt: Leon landet nicht mehr in unerreichbaren
  Bereichen.

## Neu (auf Wunsch)

- **R.P.D.-Weste wirkt**: Anlegen heilt voll (das tut sie im Original wirklich — der
  Modell-Reload schreibt HP:=100) und gibt +5 Max-HP = exakt EIN Standard-Zombie-Biss
  (5 Schaden) mehr. Vollheilungs-Sprays erhalten den Bonus; Save/Load merkt sich die
  Weste jetzt auch am Modell.

## Beantwortet (keine Code-Aenderung)

- **Timer-Bomb (ROOM1110)**: Hat einen Verwendungsort — ROOM2040/2041 in der
  Kanalisation: an der verrosteten Tuer ("Unless we use explosives or something")
  sprengt sie den Weg frei. Reine Skript-Mechanik, das Item landet nie im Inventar.
- **ROOM10F0, kleine Tuer**: Laesst sich NICHT oeffnen — es ist gar keine Tuer, nur ein
  Text-Punkt. Kein Zielraum, kein Schluessel, kein Flag existiert dafuer.
- **Ada + Marvin zusammen**: Die Szene EXISTIERT und ist aktiv — ROOM11B0 (Manhole),
  sobald BEIDE Gorilla-Bosse in ROOM11C0 tot sind. Danach treten die beiden auch in
  ROOM1260/2000 u.a. gemeinsam auf. Eine Chief-Todesszene existiert dagegen nirgends;
  die 1050-Zeilen ("I need a break"/"medical room") sind verwaiste Texte ohne Trigger —
  die Rahmen-Szene dort haengt an der Ada-Rettung aus dem brennenden Lager (ROOM1090,
  Feuerloescher aus ROOM1000).
- **Weiterer schlafender Content**: Ja, einiges — u.a. eine komplette verlorene Szene in
  ROOM1150 (sub04, sofort per RE15_FORCE_EVENT=4 abspielbar), die Meldung "Itembox is
  not available in this preview" (ROOM6020 sub02), ein geschnittener Muell-Duchsuch-
  Pickup (ROOM1030) und die Texte einer gestrichenen T-Virus-Synthese-Maschine
  (ROOM4090). Vollstaendige Liste: `analysis/nutzer_batch_2026-08-29/schlafender-content.md`.

## Offen / brauche Rueckmeldung

- **ROOM1210 "ganzer Koerper clippt durch die Wand"**: Nicht reproduzierbar verortet.
  In RE1.5-KI existiert gar kein Koerper-Modell (der Arm hat 4 Meshes/4 Knochen), und
  alle 9 Kameras des Raums tragen Masken. Bitte: welcher KI-Modus, und ein
  Screenshot/kurzes Video der Stelle.

# RE1.5 Port — v0.3.35 (Early Preview)

## Der Griff zieht Leon nicht mehr in die Wand

> "wenn ich zu oft hintereinander gegriffen werde, werde ich trotzdem noch in die Wand
> gezogen"

Behoben — und dahinter steckten zwei Fehler von mir.

Der erste: die Wandklemme, die ich in v0.3.31 eingebaut hatte, war an den falschen Gegner
gebunden. Die Arme leihen sich ihre Ringkampf-Animation vom Zombie, und der Port merkte sich
dabei den **Leihgeber** statt den **Greifer**. Meine Klemme fragte nach dem Arm — und bekam
den Zombie. Sie lief also in keinem einzigen Bild.

Der zweite war ein Messfehler, den ich Ihnen als Tatsache verkauft habe: ich meldete, die
Klemme feuere "nie, 0 von 12131 Bildern". Diese Zahl sagte in Wahrheit gar nichts ueber das
Spiel — meine Messsonde hatte der Klemme die Raumdaten nie gegeben, also konnte sie dort
nicht arbeiten. Sonde und Wache pruefen das jetzt ausdruecklich mit.

Die Ursache selbst: der Ankerpunkt stimmt jedes Mal. Die Abdrift entsteht **waehrend** des
Haltens, weil die geliehene RE2-Animation eine eigene Bewegung mitbringt. Die Klemme haelt
Leon jetzt an seinem letzten nachweislich begehbaren Standpunkt fest.

Gemessen, an derselben Wandkante:

```
vorher   306 Griff-Bilder, 231 in der Wand, bis zu 1095 Einheiten dahinter
nachher  306 Griff-Bilder,   0 in der Wand
```

Im RE1.5-Modus greift die Klemme kein einziges Mal ein — dort war nie etwas kaputt, und es
bleibt so.

## Kein Biss-Geraeusch mehr

> "Wir haben ein Biss sound. Im Original das passt hier nicht. Im Original RE 2 ist es auch
> kein Biss Sound. Uebernehme den Sound vom Original RE 2 beim greifen dort."

Stimmt — ein Arm im Gitter beisst nicht. Dort lief bisher der Biss-Laut des Zombies.

RE2s verankerter Greifer spielt beim Zupacken statt dessen einen von **zwei** Lauten, per
Zufall gewaehlt, mit einer Sperre von 150 Bildern dazwischen. Genau das ist jetzt uebernommen,
inklusive der Sperre und ueber dasselbe Feld, das das Original dafuer benutzt.

**Tests:** 244/244 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.34 (Early Preview)

Drei Meldungen behoben, eine davon war ein Fehler, den Sie andersherum gesehen haben.

## Ada bleibt wieder stehen

> "Die folgenden NPCs, wie Ada, bewegen sich immer, sie bleiben nie stehen und idlen damit
> nie. Das ist im Original anders."

Dem Port fehlte ein ganzer Unterzustand der Eskorte — der, in dem sich die Begleiterin auf
den Spieler eindreht. Und ausgerechnet der ist der einzige Rueckweg in den Ruhezustand.
Ohne ihn kreist sie ewig zwischen Gehen und Halbtempo-Gehen; stehenbleiben konnte sie gar
nicht. Der Halte-Ausloeser (unter 300 Einheiten Abstand, bei einer Figur unter 100) fehlte
ebenfalls.

Beides ist jetzt drin, samt dem Dreh-Schritt mit Totband, den das Original dafuer benutzt.
Gemessen: sie kommt an, dreht sich ein, geht in den Ruhe-Clip und bewegt sich ueber 30
Bilder um keine Einheit mehr.

## Die Haende im Gitter

**Sie fahren jetzt sichtbar zurueck.** Bisher sprangen sie in einem einzigen Bild an ihren
Platz — man sah es schlicht nicht. Jetzt fahren sie die Ausfahr-Bewegung rueckwaerts ab, mit
deren eigenen Schrittweiten: vier Bilder, exakt dieselbe Strecke zurueck wie hin.

**Man kann sie abschiessen.** Nach zwei bis fuenf Pistolentreffern taucht eine Hand ab und
kommt nicht wieder. Der Grenzwert ist keine erfundene Zahl: das Spiel fuehrt fuer diesen
Gegner ein Trefferbudget von eins bis vier, und jede Waffe zieht ihren eigenen Betrag davon
ab. Die Lebenspunkte spielen dabei keine Rolle — der Zaehler gewinnt vorher, genau wie im
Original.

**Und man trifft sie jetzt ueberhaupt.** Das war der eigentliche Fehler hinter Ihrer
Meldung "die Arme treffen nicht immer" — es war andersherum: nach dem ersten Treffer war
eine Hand fuer alle weiteren Schuesse unsichtbar. Von 19 Pistolenschuessen landete gemessen
genau einer. Das Spiel loescht am Ende der Zuck-Animation eine Markierung, die der Port
stehen liess. Mit dieser einen Zeile: fuenf von fuenf.

## Was noch offen ist

Der Griff zieht Leon im **RE2-KI-Modus** weiterhin in die Wand. Das ist jetzt immerhin
deterministisch reproduzierbar und die Ursache steht fest: die dafuer geliehene
RE2-Ringkampf-Animation traegt eine eigene Bewegung, die ihn ueber mehrere Bilder hinter die
Kante zieht (gemessen: 236 von 308 Griff-Bildern in der Wand, tiefster Punkt 1095 Einheiten
dahinter). Im RE1.5-Modus passiert das nicht — dort landet er statt dessen rund 1900
Einheiten vom Fenster weg, was das schiefe Trefferbild erklaert. Zwei Reparaturversuche
haben nicht gegriffen; ich baue keinen Code ein, der nachweislich nichts tut.

**Tests:** 244/244 gruen (lokal und im Docker-Linux-Build), zwei neue Wachen.

---

# RE1.5 Port — v0.3.33 (Early Preview)

## Stage 6: der Hintergrund war schwarz — und zwar in jedem Raum

> "Die Stage 6 Sachen laedt er irgendwie Raum und Hintergrund nicht, wenn ich den im Debug
> Mode anwaehle."

Dahinter stecken zwei voellig verschiedene Dinge.

### Der Hintergrund — behoben

Der Port lud Raum-Hintergruende nur aus einem vorgeschnittenen Verzeichnisbaum. Der ist
unvollstaendig, und zwar drastisch: Stage 1 hat 78 Verzeichnisse, Stage 2 hat 24, Stage 3
hat 30, Stage 4 hat 22, Stage 5 hat **zwei** — und Stage 6 **gar keins**. Fuer Stage 6 gab
es schlicht nichts zu laden.

Die Originaldateien liegen dagegen fuer alle Stages vollstaendig im Paket, eine je
Raumpaar. Der Lader nimmt sie jetzt als Rueckfall und schneidet den passenden Kamera-Blick
selbst heraus. Das behebt Stage 6, Stage 5 und die Luecken in 2, 3 und 4 auf einen Schlag —
ohne eine einzige neue Datei.

Die Schnittregel ist nicht geraten: ich habe sie gegen den **gesamten** vorhandenen Baum
geprueft. 1688 Schnitte, alle byte-identisch mit dem selbst gerechneten Ausschnitt, null
Abweichungen. Der Rueckfall zeigt also nachweislich dasselbe Bild wie vorher der Baum.

### Der Raum — kein Fehler, sondern der Auslieferungsstand

34 der 240 Raumdateien sind 4-Byte-Platzhalter: Raeume, die es im Prototyp nicht gibt. In
Stage 6 sind das drei der acht Eintraege, die das Debug-Menue anbietet. Wer die anwaehlt,
bekommt zu Recht nichts — dort ist nichts. Der Port meldet das sauber und stuerzt nicht ab.

Eine Wache haelt jetzt beides fest: dass **jeder** der 2188 Kamera-Schnitte aller Stages
eine Quelle hat, und dass es genau 34 Platzhalter sind. Faellt die Zahl, ist eine Raumdatei
verloren gegangen; steigt sie, wurde ein echter Raum ueberschrieben.

**Tests:** 243/243 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.32 (Early Preview)

## Die Dokumente haben jetzt Bilder

Bisher war der FILE-Schirm reiner Text — so, wie RE1.5 ihn baut. RE2 zeichnet dagegen je
Seite ZWEI Bilder: oben die Textseite ueber die volle Breite, darunter die Illustration.
Genau das ist jetzt portiert.

Die Geometrie kommt aus RE2s eigenem Leser und ist Anweisung fuer Anweisung nachgelesen:
die Seitenhoehe H = 256 minus dem y-Versatz aus dem Dokument-Record, die Textseite 256
breit auf den Zeilen 0 bis H-1, die 128 breite Illustration darunter auf H bis 255. Beides
zusammen ergibt die bekannte RE2-Ansicht: Titel oben, Gegenstand darunter.

**Alle 25 Dokumente mit ihren 191 Textseiten und 25 Illustrationen liegen im Paket.**
Sie sind die Vorlage fuer eigene Dokumente: gleiche Namen, gleiches Format, fertig. Die
Seitenhoehe liest der Port aus dem Bild selbst, nicht aus einer Tabelle — ein selbst
gebautes Dokument darf also eine eigene Hoehe haben, ohne dass am Code etwas nachgezogen
werden muss.

Zum Ansehen: `RE15_DOC=0` waehlt Dokument 0 (Chris's Diary), `RE15_DOC=9` ein anderes.
Ohne diese Auswahl aendert sich am FILE-Schirm nichts — er bleibt der Textleser von RE1.5.

Editierbare Vorlagen als PNG erzeugt `python re15_port/tools/re2_files_png.py`.

### Was dabei geprueft wird

Eine Wache vergleicht zwei voellig getrennte Quellen: die Seitenhoehe, die im Bild steht,
gegen die Seitenhoehe, die die RE2-Programmtabelle nennt. Bei allen 25 Dokumenten stimmen
sie ueberein. Dazu: alle 25 haben Titelseite und Illustration, es sind 191 Textseiten, und
bei Dokument 0 liegt das Motiv der Illustration vollstaendig im unteren Bereich — genau da,
wo die Zeichnung es abtastet.

**Tests:** 242/242 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.31 (Early Preview)

Ein Fix, und der war eine Regression von mir aus v0.3.30.

## Der Griff zieht Leon nicht mehr in die Wand

> "Die Zombies in 1210 ziehen Leon immer noch durch die Wand wenn sie ihn grabben. [...]
> Leon darf, wenn er gegrabbed wird, nicht in die Wand gezogen werden, sonst kann man ihn
> im normalen Raum nicht mehr bewegen."

In v0.3.30 hatte ich das Verschwinden beim Griff behoben, indem ich Leon an den Arm ankerte —
so, wie es RE2s Zombie tut. Nur steht RE2s Zombie frei im Raum, und der Arm steckt in der Wand.
Der Anker war damit ein Punkt hinter dem Mauerwerk.

Gemessen: die Lunge endet bei x = -16420, die begehbare Flurkante liegt auf derselben Zeile bei
x = -18164. Der Anker lag also 1744 Einheiten hinter der Wand. Leon landete auf -17312 und
wanderte ueber die naechsten Bilder auf -17731 weiter hinein — jedes Bild auf einem Punkt, den
die Kollision gar nicht erlaubt. Nach dem Loesen stand er dort fest.

Der Anker ist jetzt die **Hand** statt des Rumpfes — derselbe Punkt, an dem das Griff-Tor
ohnehin schon misst. Dort passiert der Griff ja auch: der Arm reicht durch die Gitterstaebe,
sein Koerper bleibt dahinter. Zusaetzlich laeuft die Platzierung fuer diesen einen Gegner-Typ
durch dieselbe Wandklemme wie die Spielfigur selbst, damit sie ihn unter keinen Umstaenden
hinter die Kollisionsgrenze setzen kann.

Nachher gemessen: Leon auf -18983 bis -19402, jedes Bild im begehbaren Flur.

Eine Wache haelt das jetzt fest: von 290 gemessenen Griff-Bildern steht Leon in **null** in der
Wand. Und die aeltere Abstands-Wache, die dabei ansprang, habe ich nicht etwa gelockert — sie
mass gegen den Rumpf, und der Unterschied war exakt der Hand-Versatz (3710 + 1671 = 5381). Sie
misst jetzt gegen die Hand, mit unveraenderter Schranke, plus einer zweiten gegen den Rumpf.

## Was noch NICHT behoben ist

> "Ausserdem clippen sie teilweise vollstaendig durch die Wand [...] es soll lediglich der
> obere Teil bei den Gitterstaeben durchsichtig sein."

Das ist offen. Was ich messen konnte: die Arme verlassen die Wand **geometrisch nie** — ihr
groesster Ueberstand ueber die begehbare Flurkante ist 0, der Rumpf endet 1744 Einheiten davor
und nur das Mesh ragt weiter. Es ist also kein Positions-, sondern ein Verdeckungsproblem.
Die Daten dafuer sind vollstaendig vorhanden: ROOM1210 fuehrt 399 Vordergrund-Masken in 8 von
9 Kamera-Schnitten, und alle acht zugehoerigen Atlanten liegen im Paket.

Warum die Wand den Rumpf trotzdem nicht ueberdeckt, konnte ich in dieser Sitzung nicht
feststellen — dafuer muesste ich das Bild sehen, und die Bildschirmaufnahme liefert hier nur
schwarze Frames. Der naechste Schritt steht fest: die Mask-Rechtecke der betroffenen Schnitte
gegen die Bildschirmflaeche der Arme rechnen.

**Tests:** 241/241 gruen (lokal und im Docker-Linux-Build).

---

# RE1.5 Port — v0.3.30 (Early Preview)

Vier Meldungen, alle vier aus dem letzten Paket. Zwei davon hatte ich in v0.3.29 schon
„behoben" gemeldet — sie waren es nicht, und zwar aus zwei ganz verschiedenen Gründen.

## Die schwarzen Dreiecke über dem Feuer — jetzt an der richtigen Stelle

> „Die Schwarzen Dreiecke sind noch immer über den Feuer"

In v0.3.29 hatte ich die Bank-Wahl aufgeräumt und angenommen, das sei die Ursache. Es war die
falsche Baustelle. Die richtige steht im Original an einer Stelle, an der ich vorher nicht
gesucht hatte: das Spiel entscheidet **pro Körperteil**, ob es überhaupt gezeichnet wird —
Bit 0 der Part-Flags. Der Zeichner steigt ohne dieses Bit aus, *bevor* er ein einziges Dreieck
ausgibt (`andi v0,v1,0x1` / `beq v0,zero,…` @0x8001ecc4-c8), und der Feuer-Emitter löscht sich
das Bit in seinem eigenen INIT (@0x801165d0-e4).

Die sieben Feuer in ROOM1090 sind also **unsichtbare Träger** für ihre Flammen-Effekte. Ihr
eigenes Modell ist ein einzelnes Dreieck — ein Bone, drei Punkte, eine Fläche. Genau das hat
der Port gezeichnet. Jetzt trägt jeder dieser Emitter die Maske und wird übersprungen; die
Flammen selbst bleiben unberührt, weil sie an anderen Aufrufen hängen.

Damit das nicht ein drittes Mal passiert, prüft eine Wache jetzt beides: dass alle sieben
Emitter die Maske tragen — und dass ein normaler Zombie sie **nicht** bekommt.

## Ada und die Kiste

> „Ada kann noch durch die Kiste laufen"

In v0.3.29 hatte ich die **Wand** repariert und gemeldet „Ada kollidiert wieder". Die Kiste ist
ein anderes System, und davon hatte der Port nur die Hälfte.

Der Objekt-Durchlauf des Originals schiebt nämlich nicht nur den Spieler aus den Kisten,
sondern **jeden aktiven Aktor** — die Schleife steht direkt hinter dem Typ-Handler
(@0x8002be0c-4c) und hat keinen Typ-Filter. Ausgenommen ist nur, wer ein bestimmtes Flag-Bit
trägt; die NPC-Familie trägt es nicht. Der Port hatte von dieser Funktion nur den
Spieler-Abschnitt portiert, deshalb lief jeder Gegner und jede Begleiterin durch jede Kiste.

Der Durchgang läuft jetzt mit — und zwar unmittelbar hinter der Gegner-Schleife, weil er im
Original ebenfalls **nach** der Bewegung im selben Bild liegt. Hätte ich ihn an seine
port-interne Stelle gesetzt, wäre er Adas Zug immer ein Bild hinterher gewesen.

## Die Arme im Gitter — Distanz und Verhalten wie in RE2

> „Die Arme kommen raus, wenn man noch zu weit weg ist, das sollte von Distanz und Verhalten
> her wirklich so sein wie bei Resident Evil 2."

Hier ist der Befund unangenehm ehrlich: **das Original hat pro Arm gar kein Abstands-Tor.** Ich
habe die ganze Kette gelesen. Ein raumfestes Rechteck am Flureingang schaltet über
`Member_set(12, 1)` in *einem* Bild alle zehn Arme scharf; der Arm liest diesen einen Wert
(@0x8010c614-28), und seine Logik-Tabelle für den Ausfahr-Zustand ist ein leeres `jr ra`
(@0x8010c70c) — die Ausfahr-Bewegung läuft danach ohne jede Bedingung ab. Der Wurzel-Tick
rechnet den Spielerabstand sogar aus und wirft ihn weg (@0x8010c27c).

Das Tor im Port ist also eine Zutat — es existiert nur, weil Du das Original-Verhalten (alle
zehn auf einen Schlag) verworfen hast. Und **auch RE2 hat kein Ausfahr-Tor**: sein verankerter
Greifer hat genau eine Abstandsprüfung, und die ist der *Zugriff* — Radius 1300
(`sltiu s0,s0,0x514` @0x80102f3c), dazu ein Sektor und der Ein-Angreifer-Riegel.

Was ich daraus gemacht habe: die beiden alten Zahlen fliegen raus. Sie trugen als einzige
Konstanten dieses Blocks keine Fundstelle — 850 war die halbe Tiefe des Raum-Rechtecks, auf
jeden einzelnen Arm umgehängt, und **11000 war schlicht die Flurbreite**. Deshalb reagierte
jeder Arm, egal wie weit Du von *seiner* Wand entfernt warst. Genau das hast Du gesehen.

An ihre Stelle tritt RE2s Zahl in RE2s Form: **ein Radius von 1300**, nicht ein flurbreites
Rechteck. Weil die Arme mit ihrem Ursprung in der Wand sitzen und der begehbare Flur rund 4200
Einheiten daneben liegt, zeigt der Radius auf die **ausgefahrene Hand** statt auf den Ursprung
— sonst ginge er nie auf. Das ist der eine Punkt, an dem ich von der Vorlage abweiche, und er
steht so im Code.

Gemessen an der Wand entlang: der kleinste Abstand zwischen Spieler und Hand liegt bei 72 bis
288 Einheiten, höchstens **zwei** Arme sind gleichzeitig ausgefahren (vorher drei) — und die
Reihe auf der **gegenüberliegenden** Flurseite bleibt jetzt still. Das war mit dem alten Tor
gar nicht möglich.

## Wenn die Arme Leon treffen

> „Wenn Leon von den Armen getroffen wird, verschwindet er und es gibt keine Greif Animation
> oder sowas."

Gemessen: Leon sprang im Moment des Zugriffs von (-18164,-5897) auf (-892,0) — 16508 Einheiten
weit. Ursache war ein fehlender gemeinsamer Ankerpunkt. Die Opfer-Animation wird **absolut**
platziert, ausgehend von einem Anker, den der Greifer setzt und den das Original auf den
Spieler kopiert. Beim Arm wurde er nie gesetzt, also lief die Animation ab Koordinate Null.
Jetzt bekommen Arm und Spieler beim Zugriff denselben Anker; der größte gemessene Abstand
während des Griffs liegt bei 3710 statt 16508 Einheiten.

---

**Tests:** 241/241 grün (lokal und im Docker-Linux-Build), 2 neue Wachen.


---

# RE1.5 Port — v0.3.29 (Early Preview)

Vier Meldungen. Bei einer war der Fehler frisch von mir, bei zweien lag er tiefer, als ich
zuletzt gemessen hatte.

## Die schwarzen Dreiecke über dem Feuer — meine Regression aus v0.3.28

Um Ada beim Folgen die richtige Animation zu geben, hatte ich im Renderer eine Bedingung
erweitert: „Zustand 1 → nimm die eigene Animationsbank". Zustand 1 ist aber der ganz normale
*aktiv*-Zustand fast jedes Gegners. Also haben ab v0.3.28 auch Zombies, Spinnen und die
Feuer-Emitter aus einer Bank posiert, die gar nicht zu ihrem Skelett gehört — und genau das
sind die schwarzen Dreiecke.

Der eigentliche Grund war, dass dieselbe Regel doppelt im Code stand, einmal für die
Animations-Uhr und einmal für den Renderer, und ich nur eine der beiden Kopien erweitert
habe. Die Regel steht jetzt **genau einmal**, beide Seiten fragen dieselbe Funktion, und
zwei neue Wachen prüfen sie von beiden Seiten: ein Nicht-NPC in Zustand 1 darf die eigene
Bank *nie* bekommen, eine folgende NPC *muss* sie bekommen.

## Ada läuft nicht mehr durch Wände

Zwei Ursachen, beide im Original nachgelesen.

Erstens fehlte die Wand-Klemme komplett. Die NPC-Wurzel klemmt jeden NPC direkt nach dem
Zustands-Aufruf gegen die Raumkollision — der Port hatte diesen Aufruf für die NPC-Familie
nirgends, und die neu portierte Eskorte schob die Figur einfach ungebremst weiter.

Zweitens hätte die Klemme allein auch nichts gebracht: sie liest ihren Radius aus der
Trefferbox des Gegners, und für die NPC-Typen war im Port gar keine Box eingetragen — Radius
0. Aus dem Original nachgezogen: 450 Radius, 1530 Höhe.

Eine neue Wache stellt Ada vor eine Wand und verlangt dreierlei — sie bewegt sich, sie kommt
nicht durch, und ihr Fortschritt zur Wand versiegt — plus die Gegenprobe mit einem
erreichbaren Ziel, damit „eingemauert" nicht als Erfolg durchgeht.

## ROOM1210: zu früh, zu leise, zu kurz — alle drei

**Zu früh.** Der Auslöse-Bereich des Raums ist 1700 Einheiten *tief insgesamt*, nicht
±1700 — die beiden Zahlen im Raum sind Ecke und Ausdehnung, was der Rechteck-Test des
Originals eindeutig zeigt. Ich hatte in v0.3.28 auf die volle Tiefe erhöht; das war der
falsche Schluss. Gemessen ging damit jeder Arm rund 22 Bilder zu früh auf und stand
zwei Drittel einer Sekunde fertig ausgefahren da, bevor du auf seiner Höhe warst. Jetzt ist
es wieder die halbe Tiefe.

**Zu leise.** Beim eigentlichen Zupacken hat der Arm bisher gar nichts gespielt; sein
einziger Laut lief in der Ausfahr-Schleife — und ausgerechnet der *leisere* der beiden
Zombie-Laute des Raums. Beide Vorlagen, RE1.5 wie RE2, spielen im Moment des Zugriffs einen
Laut; RE1.5 sogar unbedingt. Der ist jetzt drin, und es ist der mit der höheren Priorität:
beide liegen auf derselben Stimme, und der leise wurde vom lauten ohnehin verdrängt.

Ehrlich dazugesagt: RE2s Wucht an dieser Stelle besteht zu einem guten Teil aus
**Vibration** — eine Rampe auf volle Motorstärke plus vier Stöße beim Biss. Dafür hat der
Port kein Gegenstück; das ist eine benannte Lücke, kein Versehen.

**Zu kurz.** Das war der interessanteste Befund. Die Arme *können* dich mit ihrem Körper
gar nicht erreichen: ihr wird von einer eigenen Kollision im Gitter gehalten, du von deiner
im Flur, und dazwischen steht gut ein Meter Wand. Der kleinstmögliche Abstand zwischen den
beiden Mittelpunkten ist damit hart über 1800 Einheiten — mehr als das Griff-Fenster von
1200 zulässt. Keine zusätzliche Bewegung ändert daran etwas.

Das Modell reicht aber viel weiter als sein Mittelpunkt: die Hand ragt im Ausfahr-Clip 1671
Einheiten nach vorn. Sie steht also sehr wohl im Flur, nur der Körper nicht. Deshalb misst
das Griff-Fenster jetzt **die Hand statt des Mittelpunkts**. Die Schwelle selbst bleibt
unverändert bei 1200 — sie anzuheben wäre eine erfundene Zahl für genau dieses Problem
gewesen.

Gemessen an der Stelle, an die dich die Kollision wirklich lässt: drei Zugriffe, 275 Bilder
im Griff, sechs Bisse zu je 20 Lebenspunkten. In der Flurmitte greift nichts. Das ist genau
das Bild aus RE2, das du beschrieben hattest.

## Was dabei noch aufgefallen ist

Beim Nachmessen hat sich gezeigt, dass meine früheren Angaben zum „begehbaren Boden" in
ROOM1210 **invertiert** waren: die Kollisionsabfrage meldet in diesem Raum *innerhalb* einer
Wandzelle eine 1, gelaufen wird im Gegenstück. Die Wache benutzt jetzt nur noch den echten
Laufweg. Das erklärt auch, warum meine Erreichbarkeits-Zahlen der letzten Runde nicht
zusammenpassten.
