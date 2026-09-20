# v0.8.7 - 2026-09-20

Acht Rueckmeldungen aus dem Spieltest mit v0.8.6. Sieben sind umgesetzt, eine braucht
noch eine Angabe von Ihnen.

## Die Mehrfachschuss-Pistole verstopfte den ganzen Raum

> "nehme ich die mehrfach schuss Pistole mit in stage 1, bleibt dort die Kugel Animation
> und Sound im loop haengen." — "Ausserdem bleibt genau in diesem Raum der Sound haengen."

Beide Punkte hatten **eine** Ursache. Betroffen sind die Beretta M93R und die Glock 18,
die einzigen beiden Waffen mit Drei-Schuss-Salve. Ihre Effektkette meldet sich im Original
nach dem dritten Schuss selbst ab; im Port blieb der Platz belegt, und die Kette schob in
JEDEM Bild ein weiteres Muendungsfeuer samt Rauch, Huelse und Knall nach — bis alle 96
Effektplaetze voll waren und blieben. Danach war in diesem Raum ueberhaupt kein Effekt mehr
moeglich, und der Knall wurde bis zu achtmal pro Bild auf derselben Stimme neu angestossen.

Gemessen im Vergleichslauf: statt 15 Effekt-Kindern und 30 Knallen entstanden 100 Kinder und
946 Knalle, der letzte davon 300 Bilder nach dem Loslassen des Abzugs und mit leerem
Magazin. Der Effektpool wird nur beim Raumladen geleert — daher hoerte es erst beim
Raumwechsel auf, genau wie Sie es beschrieben haben.

## Munition: nur noch die Haelfte

> "dann ist die Munition die man findet viel zu viel. ich will Das du die Munition auf
> maximal die haelfte begrenzt die man findet."

Was in der Welt liegt, kommt jetzt zur Haelfte im Beutel an, abgerundet, mindestens eine
Einheit. Ueber das ganze Spiel sind das 532 statt 1108 Schuss, in Stage 1 allein 458 statt
950. Kraeuter, Schluessel, Dokumente und die Startausruestung bleiben unangetastet.

Wichtig und bewusst ausgenommen: **das Nachladen aus der Reserve gibt weiter das volle
Magazin** — sonst waere bei jedem Nachladen die Haelfte verschwunden. Ebenfalls nicht
angefasst sind Waffen, die geladen herumliegen (etwa die Ingram mit 100 Schuss): das ist
die Waffe, keine Munitionspackung. Sagen Sie Bescheid, wenn die auch halbiert werden soll.

*Das ist ausdruecklich keine Original-Treue, sondern Ihre Entscheidung.*

## Klick-Laut an den Cursor-Raetseln

> "dann will ich bei den raetseln wo man etwas mit Cursor auswaehlt und klickt einen click
> Sound. den gibt es, wenn nicht in resident evil 1.5 - auf jeden fall in resident evil 2."

In Resident Evil 2 nachgeschlagen: der Laut haengt dort nicht am Tastendruck, sondern
daran, dass der Cursor wirklich auf ein anderes Feld wechselt. Genau so ist es gebaut —
ein Klick beim Feldwechsel, ein zweiter beim Bestaetigen. Beide Laute sind die, die
Resident Evil 1.5 in seinem eigenen Inventar ohnehin benutzt; es ist also nichts erfunden.

Damit es nicht an falschen Stellen klappert, wurden alle zwoelf Raetsel-Raeume je zehn
Sekunden ohne Eingabe laufen gelassen: kein einziger Fehl-Klick. Ein gehaltener Knopf
rattert nicht.

## Untertitel bleiben stehen, solange gesprochen wird

> "koennen wir einfach die Untertitel Einblendung So lange ebenfalls verlaengern?"

Ja. Bisher wartete zwar die naechste Zeile, bis die Aufnahme zu Ende war, aber der
Untertitel selbst verschwand schon vorher. Er bleibt jetzt so lange stehen, wie die
Aufnahme noch laeuft. Der Spieler wird dadurch nicht laenger blockiert als vorher — das
Nachwirken haelt nur die Schrift, nicht die Steuerung.

## Die R.P.D.-Weste bleibt nach dem Laden an

> "dann sammelt man die weste ein speichert und laedt den Spielstand Ist die weste weg."

Der Spielstand kam vollstaendig zurueck, auch die zusaetzliche Energie der Weste — nur das
Modell wurde nicht nachgezogen, Leon stand wieder in der normalen Uniform da. Der Port zog
den Modellwechsel nur beim Betreten eines Raums nach, und das Laden geht an dieser Stelle
vorbei. Im Original haengt der Wechsel am Raumlader, der nach jedem Laden mitlaeuft; so ist
es jetzt auch hier. Nachgesehen an zwei Bildern, und ein Riegel prueft es beim Bauen
automatisch mit.

Bei der Gelegenheit nachgemessen, ob weitere Ausruestung betroffen ist: Blutspuren am
Modell, angelegte Waffe und Kamerawinkel kommen korrekt zurueck. Die Weste war der einzige
Fall.

## Birkin ist jetzt solide

> "Birkin ist nicht solid."

Man konnte drei Meter weit in die Fleischmasse hineinlaufen. Der Port benutzte fuer den
Koerper einen einzelnen Ein-Meter-Zylinder am Ursprung. Resident Evil 2 gibt G5 dort gar
keinen Zylinder, sondern **zwei** Kollisionskoerper: einen grossen fuer die Masse (Radius
sechs Meter, zwei Meter hinter dem Ursprung) und einen kleinen fuer den Kriecher-Oberkoerper
davor, der beim Massen-Biss mitfaehrt. Beide sind gebaut, samt der Regel, dass sie erst
scharf werden, wenn der Kampf beginnt, und dass der Boss selbst nie geschoben wird.
Gemessen: der Spieler wird statt auf 1451 jetzt auf 4451 Einheiten vor dem Ursprung
gehalten — also vor der Masse statt darin.

**Dazu ein vollstaendiger Abgleich des Endkampfs.** Alle Zustandstabellen des
Original-Overlays sind Eintrag fuer Eintrag gegen den Port gestellt: 25 lebende Eintraege,
der Port hat 22. Zwei vermeintliche Luecken sind keine — ein Zustand des Originals wird von
keiner Stelle im Spiel je eingeschaltet, und die Treffer-Routine kommt gar nicht aus dem
Boss-Code, sondern aus dem Waffen-Code des Spiels. **Drei echte Luecken bleiben** und sind
mit Adresse benannt: die Opferanimation beim Verschlungenwerden auf dem RE2-Spielerskelett,
der Blutauswurf an der Trefferstelle, und die Vibrations-Kaskaden. Sie sind nicht
geschlossen, weil dafuer jeweils noch eine Original-Funktion fehlt — geraten wird nichts.

## Ada: ein Fehler gefunden, der gemeldete aber noch nicht

> "dann im Raum der cutscene wo man ada aus dem feuer rettet, und dann mit ihr den Raum
> wechselt, spielt ihre Animation am Anfang doppelt ab."

Beide in Frage kommenden Raeume wurden nachgespielt und Adas Bewegungen Bild fuer Bild
aufgezeichnet. Dabei ist **ein eigener Fehler aufgefallen und behoben**: der Port liess nach
einer Skript-Bewegungsanweisung das Rueckwaerts-Kennzeichen stehen, wodurch die naechste
Animation verkehrt herum lief.

Die gemeldete Doppelung selbst liess sich dagegen **nicht als Fehler nachweisen**: an beiden
Stellen, an denen eine Wiederholung gemessen wurde, macht das Originalskript dasselbe.
*Dafuer braeuchte ich eine Angabe:* passiert es im Raum mit dem Feuer (ROOM1090) oder erst
im naechsten (ROOM1050) — und vor oder nach dem Raumwechsel?

# v0.8.6 - 2026-09-20

Die offenen Punkte aus v0.8.5, auf Zuruf des Nutzers: *"gehe die offenen Punkte an, bei den
offenen pri findings sei kreativ, das es gut aussieht. Ausserdem beim apk, splitte es
ebenfalls in mehrere Teile, das du es ins repo laden kannst."*

## Der Alligator hat Leon jetzt wirklich ganz im Maul

In v0.8.5 sass Leons Wurzel an der richtigen Stelle, aber er wurde in voller Groesse
gezeichnet, waehrend der Alligator auf zwei Drittel verkleinert ist — Rumpf und Beine lagen
quer ueber der Schnauze. Der Spieler-Zeichner ehrt jetzt waehrend der Fress-Phase denselben
Verkleinerungs-Mechanismus, den der Gegner-Zeichner immer schon hatte.

Gemessen im gator-lokalen Rahmen: von Leons 776 Modellpunkten liegen jetzt **alle 776** im
Maulraum statt 620 — genau der Wert, den das Original erreicht, wenn beide in Originalgroesse
stehen. Am Bild aus dem laufenden Spiel liegt er zwischen Ober- und Unterkiefer hinter den
Zaehnen.

## Zwei Untersuchungen, die bewusst nichts geaendert haben

**Die "byte-naehere" Biss-Animation des Alligators bleibt draussen.** Naheliegend waere
gewesen, den kurzen Zuschnapp durch die lange Original-Clipfolge zu ersetzen. Gemessen ueber
72 Kampfverlaeufe: die Zeit vom Anlauf bis zum Treffer stiege von 8 auf 133 Bilder, und die
Totzeit, in der der Alligator mit offenem Maul am Ziel steht, ohne dass das Trefferfenster
offen ist, von 0 auf durchschnittlich 119 Bilder. Der Kampf wuerde spuerbar zaeher, ohne
besser auszusehen. Der Messweg bleibt als Schalter im Code, ausgeliefert wird der bisherige.

**Der Lauer-Zustand der Spinne startet gar keinen Angriff.** Die Annahme, dort fehle der
Angriffs-Laut, war falsch: der Zustand dreht die Spinne nur und meldet zurueck. Der Angriff
beginnt erst ab rund 1200 Einheiten Abstand — darunter passiert nichts, darueber kommen Biss
und Laut zuverlaessig. Gemessen ueber neun Abstaende.

## Kleinere Original-Treue nachgezogen

- Messer und die uebrigen Schusswaffen laufen jetzt ebenfalls ueber die Teile-Maske; die
  Geometrie-Daten aller Waffen wurden aus dem Original gedumpt statt nur der ersten.
- Die Lebenspunkte frisch gespawnter Kriecher werden wie im Original halbiert.
- Ein Zombie, den das Raumskript aufweckt, ist waehrend des Aufstehens wieder gesperrt — der
  Port hatte ihn einen Moment zu frueh freigegeben.
- Befund am Rande: zwei Gegner-Deskriptoren, die bisher als Liegende gefuehrt wurden, gehoeren
  in Wahrheit zur Fresser-Familie.

## Vordergrund-Masken: alle offenen Cuts gebaut — und ein Fehler von mir aufgeklaert

In v0.8.5 waren vier Cuts NICHT geschrieben worden, weil sie die Abnahme nicht bestanden.
Jetzt sind es **null**; 77 Cuts sind gebaut. Kein einziger begehbarer Platz VOR einem
Gegenstand wird mehr verdeckt (vorher einer ganz und zwei teilweise), und hinter den
Gegenstaenden bleiben nur noch 713 von 76015 Plaetzen unverdeckt statt 928.

- **ROOM10E0 Cut 7** passte angeblich nicht ins Texturblatt. Es lag nicht an der Flaeche
  (die belegt nur 78 %), sondern am Packverfahren: das alte legte Streifen regalweise ab
  und wies sechs Kaesten ab. Mit einem Verfahren, das Luecken wiederverwendet, passt der
  Cut bei feiner Tiefenstufung statt grober.
- **ROOM10D0 Cut 1** (Ihre Marke F423) scheiterte an einer einzigen Bildspalte am oberen
  Rand des Klapptischs, deren Sehstrahl den Boden erst 13,5 Meter hinter dem Tisch trifft.
  Diese Spalte erbt ihre Tiefe jetzt von den Nachbarn, und die Freistellung ist in
  Tischplatte und Gestell mit eigenen Tiefen zerlegt. Im Spiel nachgesehen: vor dem Tisch
  steht Leon vollstaendig da, dahinter ist er weg.
- **ROOM10A0 Cut 2** fiel an einem Platz durch, der 9 bis 63 Einheiten von der Standlinie
  entfernt liegt. Die Schranke mass feiner, als der Zeichner ueberhaupt aufloesen kann:
  eine Tiefenstufe des Originals ist 64 Einheiten breit. Mit dieser Toleranz gemessen,
  bleibt kein Fall uebrig.

**Und der Punkt, den ich Ihnen als Frage gestellt hatte, war mein Fehler.** Ich hatte
gefragt, ob drei Freistellungen im vierfachen Massstab so gemeint waren, weil sie nur zu
85 bis 93 Prozent passten. Sie sind nicht vergroessert — **neun Ihrer Freistellungen lagen
schlicht am falschen Kamerawinkel.** Bei einfachem Massstab und richtigem Cut stimmen sie
zu 100,0 Prozent: `1000/01_*` gehoert zu Cut 0, `1000/03_*` zu Cut 2, `11F0/01` zu Cut 0.
Das alte Zuordnungsmass war selbstbestaetigend — es belohnte kleinere Objekte, weil die
ueberall gut passen. Gegen ein Zufallsmodell geprueft hat eine richtig sitzende
Freistellung genau EINE gute Lage, die falsch verkleinerten hatten hunderte. Alle 131
STAGE1-Objekte sind jetzt gegen dieses Mass geprueft.

*Zwei Winkel haben dadurch derzeit gar keine Maske:* ROOM1000 Cut 3 und ROOM11F0 Cut 1.
Ihre bisherigen Sektionen waren aus den falsch verorteten Freistellungen gebaut. Beide
Winkel haetten einen echten Vordergrund-Gegenstand (Schaltschraenke bzw. die rechte
Kabinenwand des Waschraums) — dafuer braeuchte es je eine Freistellung.

*Ebenfalls offen:* in ROOM1000 Cut 4 und ROOM10A0 Cut 5 liegt das Tiefenmodell systematisch
zu fern, dort bleibt hinter dem Gegenstand mehr sichtbar als gewollt. Und ROOM10F0 Cut 4/5
decken hinten schlecht ab, weil ihre Silhouetten noch aus meinen eigenen Quadern bestehen
statt aus Freistellungen.

## Birkin: der Tentakel packt Leon jetzt wirklich

Bisher zog der Tentakel Leon heran, ohne dass an ihm etwas passierte — die Griffe kosteten
nur Leben. Der Grund: die Opferanimationen haengen in Resident Evil 2 nicht am Gegner,
sondern am Spieler, und diese Kette war im Port nicht nachgebaut. Sie ist jetzt
reverse-engineert: der Tentakel meldet sich beim Zupacken selbst als Besitzer der
Spieler-Routine an, und je nach Angriff laufen vier verschiedene Opfermaschinen ab. Bei der
Peitsche aus der Distanz reisst es Leon die Arme hoch, beim Spiess klappt er vornueber und
geht in die Knie, und wer schon festgehalten wird und einen zweiten Spiess abbekommt, liegt
quer im Gang. Die Animationen bringt das Tentakelmodell selbst mit.

Dazu drei kleinere Korrekturen: die Zeitgeber der Auftritts-Kommandos werden wieder gelesen,
bevor sie hochzaehlen (die Sender feuerten ein Bild zu frueh), der Vorzeichen-Dreh-Zweig der
Tentakel-Kollision ist nachgezogen, und die Kopf-Peilung ist vollstaendig aufgeloest — sie
hat keine Distanzgrenze.

**Zwei vermutete Fehler waren keine.** Die Behauptung, das Original verzerre die Normalen
der gestreckten Tentakel mit, ist widerlegt: die betreffende Routine ist der Zeichner, und
die Lichtmatrix entsteht dort vor jeder Streckung. Und eine Klemme in der Kollision, die ich
nachziehen wollte, ist in diesem Aufruf beweisbar unerreichbar.

## Android-Paket liegt jetzt im Repository

Die APK faellt als eine Datei ueber GitHubs Dateigrenze. Sie wird deshalb mit derselben
Volume-Groesse wie die Windows- und Linux-Pakete in einen Split-Satz geschnuert
(`..._android.z01` und `..._android.zip`, zusammen 167 statt 358 MB). Zusammensetzen wie
gewohnt mit `zip -s 0 ..._android.zip --out joined.zip`, dann `unzip joined.zip`; die
Anleitung steht in `re15_port/platform/android/README.md`.

Geprueft und nicht nur behauptet: die aus den Teilen wiederhergestellte APK ist bytegleich
mit der gebauten (Pruefsumme identisch).

# v0.8.5 - 2026-09-19

Zwoelf Rueckmeldungen aus einem Durchlauf mit v0.8.4. Jede davon hat ein eigenes
Dossier unter `analysis/befunde_2026-09-19/` bekommen: erst messen, dann den
Original-Mechanismus im RE1.5- bzw. RE2-Code nachschlagen, dann bauen. Jede Zahl im
Code traegt ihre Adresse im Original.

## Zombies, die nur daliegen, bleiben liegen

> "Die Zombies, die im Original nur am Boden liegen, und die man nicht anschiessen
> koennen sollte und nicht auf uns reagieren sollten, reagieren auf uns."

Diese Zombies (Besprechungsraum und der Raum davor) lassen sich mit der RE2-KI nicht mehr
anschiessen und wachen davon auch nicht mehr auf. Beide Originale sperren sie dreifach
gegen Treffer; der Port hatte die Sperre fuer alle liegenden Zombies aufgehoben, weil die
am Boden FRESSENDEN Zombies treffbar sein muessen. Die bleiben es.

## Ein Treffer wirft den Aufstehenden nicht mehr zurueck

> "Wenn die Zombies im Prozess sind wieder aufzustehen, laesst man sie mit Schuss wieder
> hin fallen, und dann probieren sie wieder aufzustehen usw. Das ist im Original Resident
> Evil 2 anders."

Wer einen Zombie beim Aufstehen trifft, wirft ihn nicht mehr in die Liegepose zurueck. Er
zuckt, dreht den Oberkoerper weg und steht weiter auf. Resident Evil 2 hat dafuer einen
eigenen Treffer-Handler, der die laufende Animation nicht unterbricht und danach den
gesicherten Zustand wiederherstellt. Das gilt auch fuer Zombies, die vom Fressen
aufstehen; die hatten bisher ihre Animation abgebrochen und danach ein falsches
Todes-Verhalten.

## Trefferzonen: was man trifft, haengt von Zielhoehe und Entfernung ab

> "Ich kann gerade schiessen und treffe Zombies am Boden. Das kann so nicht sein, schaue
> noch mal nach was ich wie treffen kann, wenn ich runter, mittel oder hoch ziele."

Der Port hatte nur EINEN Treffertest auf Zielhoehe. Jetzt laeuft der Schuss durch dieselbe
Kette wie in RE2: eine Teile-Maske am Gegner (Beine / Rumpf / Kopf), die beim Hinlegen und
Kriechen auf "nur Beine" umschaltet, und ein Applier, der aus Zielhoehe und Entfernung die
zulaessige Zone waehlt.

- Wer am Boden liegt oder kriecht, ist nur noch mit gesenktem Lauf zu treffen. Geradeaus
  geht ins Leere, weil dort nur die Beine im Schussfeld stehen.
- Ein Zombie, der aufsteht, ist bis zur Mitte der Bewegung geradeaus nicht zu treffen,
  mit gesenktem Lauf durchgehend.
- Gesenkte Schuesse und Schrot nach oben reichen nicht mehr beliebig weit: bis 4600 bzw.
  3600 Einheiten, darueber verfehlen sie. Das ist der Original-Stand, kein neues Limit.
- Die Entfernung entscheidet ueber die Wirkung: Pistole 16 / 15 / 14 Schaden, Schrot
  200 / 60 / 40. Nur ein Treffer aus kurzer Distanz wirft noch um, weiter weg taumelt der
  Zombie.
- Der fressende Zombie am Boden bleibt ein normales Ziel.

## Kriecher in der Eingangshalle: kein Rueckversatz, und sie stellen sich tot

> "Schiesse ich die kriechenden Zombies einmal an, werden sie wieder ein Stueck zurueck
> gesetzt. Mal abgesehen davon kriechen die im Original nicht direkt los, sondern stellen
> sich erst einmal tot!"

- Ein Treffer setzt den Kriecher nicht mehr zurueck. Der Port hatte beim Wiedereinstieg in
  die Kriech-Animation den Bewegungsanker nicht neu gesetzt und rechnete daraus einen
  Sprung von 830 Einheiten in einem Bild; RE2 verankert dort zweimal neu.
- Die beiden Kriecher an der Suedtuer stellen sich tot: sie liegen still, bis man direkt
  vor ihnen steht (dann greifen sie nach dem Bein) oder bis man sie anschiesst (dann
  kriechen sie los).
- Der Beingriff endet wie im Original damit, dass Leon ihm den Kopf zertritt.

## Gegenstaende bleiben da: Leiche, Kisten, Schalter

> "In der paketierten Version fehlen diverse Dinge in ROOM 11F0 mindestens die Schalter vom
> Raetsel, in ROOM 1050 die zerteilte Leiche, in ROOM 1090 die Kiste usw."

Das war kein Fehler des Pakets: springt man direkt in den Raum, sind alle Objekte da. Der
Defekt trat nur im Durchlauf auf. Der Vermerk "dieses Objekt ist schon aufgesammelt" galt
prozessweit statt nur fuer den Raum, in dem das Item lag — und loeschte danach in JEDEM
Raum das Objekt mit derselben Nummer. Das Original leert seine Objekt-Tabelle bei jedem
Raumladen. Aufgesammelte Items verschwinden weiterhin sofort und bleiben weg.

## Keine komische Pose mehr nach der Tuer

> "Ab ROOM 1040, sobald man die Tueren durchlaeuft macht Leon immer am Start im Raum eine
> ganz komische Animation."

Der Port posierte in den ersten fuenf Bildern nach dem Raumwechsel den ersten Clip der
Raum-Animationsbank. ROOM1040 ist der erste Raum der Kette, der so eine Bank hat — und ihr
erstes Bild ist eine Zwischensequenz-Pose mit ausgestreckten Armen. Das Original setzt beim
Eintritt hart die Ruhepose aus der Waffenbank. Leon steht jetzt vom ersten Bild an normal;
bei drei Waffen gibt es wie im Original ein kurzes Ueberblenden.

## Der Alligator hat Leon im Maul, und beide Gegner haben ihre Laute

> "Der Aligator hat Leon immer noch nicht im Maul im Finisher, sondern er liegt darunter,
> mal abgesehen davon, hat der Aligator immer noch nicht den Angriffs/Biss Sound. Und auch
> die Spinne hat noch nicht ihren Angriffssound."

- Leon sitzt im Finisher zwischen den Kiefern. Der Versatz zum Opfer wurde nicht im selben
  Massstab mitgefuehrt wie das im Port verkleinerte Alligator-Modell; er lag dadurch 2628
  Einheiten davor und 599 darunter auf dem Boden.
- Der Alligator bruellt beim Losschnellen, zusaetzlich zum Zubeiss-Geraeusch.
- Die Spinne hat wieder eigene Biss- und Schrittlaute. Das Spiel konnte bisher nur EINE
  Gegner-Soundbank gleichzeitig halten; im Alligator-Raum spielte die Spinne deshalb
  fremde Samples oder blieb stumm. Es haelt jetzt mehrere.

*Noch nicht ganz:* Leon bleibt im verkleinerten Maul in Originalgroesse, Rumpf und Beine
ragen heraus. Und der Biss laeuft weiter als kurzer Schnapp statt als lange RE2-Attacke.

## Die Gitterhaende in ROOM1210 sind jetzt die RE2-Zellenarme

> "Fuer die Arme die durch die Zellen durch kommen im ROOM 1210 MUSST du dich an ROOM
> 205/2050 von Resident Evil 2 orientieren. Dort ist GENAU das gemacht in der Retail
> Fassung, und NUR so funktioniert es."

Genau das ist passiert: der Flur faehrt jetzt die Original-Maschine der RE2-Zellenarme
samt deren eigenem Modell — zwei Arme pro Fenster, sechs eigene Bewegungen und eine eigene
Opfer-Animation fuer Leon. Ein Arm ruht unsichtbar im Fenster, faehrt aus, tastet nach
Leon, packt zu und zieht ihn an die Hand; wer sich losreisst, wird freigegeben, danach
zieht der Arm sich endgueltig zurueck. Jeder Arm greift genau einmal, und solange einer
zupackt, hat der naechste eine Sperrzeit. Die Hoehe stimmt jetzt auch: die Fensterbank
wurde im Original-Hintergrund vermessen, der bisherige Wert liess den halben Arm im
Mauerwerk stecken. Zupacken, Loslassen, Treffer und Ausfahren haben ihre eigenen Laute.

## Birkin im Endkampf: Auftritt, Gesicht, Tentakel

> "Bei Birkin das mit den Tentakeln usw. ist schon ein wenig besser, aber noch weit
> entfernt von gut. 1. Muss er eigentlich hinten durch das Zug Rechteck rein kommen ...
> 2. Sind weder die Augen, noch die Zaehne noch sonst irgendwas von ihm animiert.
> 3. Sind die Tentakel nicht richtig animiert, greifen nicht an usw."

- Er kommt jetzt wirklich von hinten durch das Zug-Rechteck herein und kriecht den Wagen
  herunter auf Leon zu, statt von Anfang an mitten im Bild zu stehen. Das Raumskript
  setzte ihn an eine Stelle, die fuer die RE1.5-Zwischensequenz gedacht war; RE2 setzt
  ihn beim Kampfstart selbst ans Westende. Am Ende des Auftritts steht er auf exakt
  derselben Entfernung wie in Resident Evil 2.
- Sein Gesicht lebt: Kopf und Hals folgen der Animation und drehen sich zu Leon, und die
  beiden Augen auf der Fleischmasse bewegen ihre Pupillen. Beides fehlte komplett — der
  Kopf war starr an den Rumpf geheftet, weil die Verformung ueber zwei Knochen nicht
  gerechnet wurde.
- Die vier Tentakel haengen wieder an der Masse. Vorher schlugen drei von ihnen rund
  16 Kilometer hinter ihm ins Leere, weil der Port den Anker nach dem Auftritt nie
  zuruecksetzte. Sie biegen sich beim Einrollen, strecken sich nur in die Laenge statt
  dick zu werden, und treffen ueber vier Punkte am ganzen Arm statt nur mit der Spitze.
- Der Speer-Angriff kam durch einen vertauschten Vergleich nie vor und ist ab etwa sieben
  Metern wieder dabei. Und Birkin biss nur zu, wenn Leon NICHT vor ihm stand — auch das
  war verdreht.

*Noch offen:* die Opfer-Animation des Tentakel-Zugs auf dem RE2-Rig, und einige
Feinheiten der Original-Verformung.

## Vordergrund-Masken STAGE1: neu aus den Freistellungen

> "Es gibt diverse PRIs die sind unglaublich kaputt. Sie sind komplett ueberdeckend, oder
> halb Transparent wo sie es nicht sein sollen, oder funktionieren gleich mal garnicht.
> Verstehe ich garnicht, wo ich dir doch SAEMTLICHE PRIs die wir haben wollen fuer STAGE1
> in den pri Ordner zur Verfuegung gestellt habe ... Ausserdem hast du teilweise meine
> perfekten Ausschnitte aus dem pri Ordner ueberschrieben mit eigenen / loechrigen."

Der Vorwurf war berechtigt, und zwar dreifach.

**Erstens** wurden die gelieferten Freistellungen nicht punktgenau verwendet: das Werkzeug
weitete jede Maske um vier Zeilen nach oben und einen Punkt ringsum auf. Dadurch bekamen
bis zu 62 % zusaetzliche Hintergrundpunkte die Tiefe des Objekts und wurden ueber die Figur
gemalt. Jetzt gilt die Freistellung punktgenau, und der Bau bricht ab, wenn auch nur ein
Punkt abweicht — geprueft ueber die Leser der Engine, nicht nur im Werkzeug.

**Zweitens** war die Tiefe zu nah gerechnet: ein Abschlag von zehn Prozent liess Masken
Figuren verdecken, vor denen man steht. Der Abschlag ist weg, die Tiefe kommt aus der
Raumgeometrie, und jedes Rechteck traegt nur noch eine Tiefenstufe statt eines Mittelwerts
ueber eine ganze Kachel — das Schachbrett, das wie Transparenz aussah, entsteht dadurch
nicht mehr. Dazu kam eine echte Engine-Korrektur: die Schwelle, ab der eine Maske verdeckt,
lag im Port eine Stufe zu frueh.

**Drittens** wurden Ihre Dateien zurueckgeholt, wo ich sie ueberschrieben hatte. Bei
ROOM10D0 war Ihre 30x46-Freistellung eines Blattes durch eine eigene 65x120-Fassung ersetzt
worden, waehrend die eingemessene Lage fuer Ihre Datei stehen blieb — daher der Boden, der
ueber Leons Bein gezeichnet wurde. Drei weitere Dateien sahen nach Ueberschreibung aus,
waren aber Ihre eigenen Neulieferungen; die blieben unangetastet.

76 Cuts sind neu gebaut, jeder mit punktgenauer Deckung und der Zusage, dass kein
begehbarer Platz VOR dem Gegenstand verdeckt wird. Im Spiel geprueft: vor dem Gegenstand
ist die Figur sichtbar, dahinter verschwindet sie.

*Drei Cuts wurden bewusst NICHT geschrieben, statt sie schoenzurechnen:* ROOM10D0 Cut 1
(Ihre Marke verlangt 16 Einheiten mehr, als die Geometrie hergibt), ROOM1000 Cut 3 und
ROOM10E0 Cut 7 (die Freistellung passt nicht ins Texturblatt). Dort bleibt die bisherige
Maske. Offen ist auch, ob die drei Freistellungen im vierfachen Massstab so gemeint waren —
sie treffen nur zu 85 bis 93 Prozent und verlieren duenne Teile.

## Android-Paket: dasselbe Spiel, bedient ueber ein On-Screen-Pad

> "Erstelle mir zusaetzlich zum Windows/Linux Package ein einfaches Android Package, was
> einfach ueber Overlay Controller bedienbar ist."

Neben Windows und Linux/Steam Deck gibt es jetzt eine Android-APK (arm64 fuer Geraete,
x86_64 fuer den Emulator, ab Android 7.0). Es ist kein zweiter Port: dieselbe Engine und
dieselben PC-Plattformquellen werden mit dem NDK uebersetzt und von SDL2 in derselben
Version wie am PC gestartet. Der Windows- und Linux-Bau bleibt unveraendert.

Die Assets liegen vollstaendig in der APK und werden beim ersten Start mit
Fortschrittsbalken in den App-Speicherordner entpackt. Dieser Ordner ist auf Android das,
was am PC das Verzeichnis neben der exe ist: dort liegen die Protokolle, die Memory-Card
und die entpackten Daten.

Bedient wird ueber ein halbtransparentes Pad ueber dem Bild: D-Pad links mit acht
Richtungen, die vier Aktionstasten rechts, L1/R1 oben, SELECT und START in der Mitte, dazu
ein kleiner Knopf, der wie F9 am PC eine Marke ins Protokoll setzt. Mehrere Finger
gleichzeitig funktionieren; sie erzeugen dieselben Pad-Signale wie Tastatur und Gamepad.
Ein angeschlossener Controller geht weiterhin. Querformat, Bild zentriert, Vollbild.

Abgenommen im Android-Emulator, und das war noetig: auf einem breiten Bildschirm rechnet
SDL die Finger auf den 4:3-Ausschnitt um und klemmt sie an dessen Rand — alles in den
schwarzen Streifen war unerreichbar, das D-Pad also genau dort, wo es sitzt.

Die APK liegt als Split-Zip im Repo, weil sie als eine Datei ueber GitHubs Grenze faellt:
`re15_port_v0.8.5_android.z01` und `...android.zip`, dieselbe Volume-Groesse wie bei den
anderen Paketen. Zusammensetzen wie gewohnt mit `zip -s 0 ...android.zip --out joined.zip`
und danach `unzip joined.zip`; die Anleitung steht in
`re15_port/platform/android/README.md`. Geprueft: die so wiederhergestellte APK ist
bytegleich mit der gebauten.

*Grenzen:* APK und entpackte Daten belegen zusammen rund 730 MB; das Pad hat feste Groessen
und keine Einstellungen; nur Querformat; mit Entwickler-Schluessel signiert (Sideload);
bisher nur im Emulator getestet, nicht auf einem echten Geraet.

# v0.8.4 - 2026-09-14

## Birkin kommt endlich, und die Beine kippen um

- **Birkin taucht im Endkampf wieder auf.** Er war nie weg - er kroch nur aus der
  falschen Ecke heran. Das Raumskript setzt ihn zu Kampfbeginn an eine bestimmte
  Stelle im Zug; der Port hat diesen Wert im selben Bild wieder weggeworfen und ihn
  stattdessen an seinen Spawn 15.900 Einheiten weiter westlich gesetzt. Von dort
  brauchte er rund 74 Sekunden bis zu Leon statt 19, die erste halbe Minute davon
  ausserhalb des Bildausschnitts. Beide Stellen sind behoben.
- **Die Beine abgeschossener Zombies kippen um, statt stehenzubleiben.** Hier hatte der
  Port zweimal danebengelegen: erst liefen die abgetrennten Beine mit der Animation des
  Oberkoerpers weiter, dann standen sie ganz still. Das Original macht keines von beidem
  - die untere Haelfte hat eine EIGENE Animation und kippt ueber zwei Sekunden um,
  zuckt danach noch und friert erst am Ende ein. Genau das laeuft jetzt.
- **Tote Zombiehunde hinterlassen eine Blutlache.** Das Ausrollen nach dem Tod war
  schon richtig; was fehlte, war das, was danach passiert: im Original faerbt sich der
  Schatten unter der Leiche um und breitet sich drei Sekunden lang aus. Im Port lag
  dort der normale graue Schatten, und nach dem Umfallen geschah sichtbar nichts mehr.

## Der Bildschirm wird schwarz geloescht, nicht dunkelblau

Beim Spielstart blitzte kurz ein dunkelblauer Hintergrund auf. Die Loeschfarbe des
Bildpuffers stand auf einem Blauton, der sich auf den Port selbst als Quelle berief.
Das Original loescht in jedem seiner zehn Codepfade mit Schwarz.

## Sprachaufnahmen laufen wieder im richtigen Tempo

Der Port rechnete Sprache und die Rotor-Gerausche fest auf 44100 Hz um - unabhaengig
davon, mit welcher Rate die Soundkarte tatsaechlich laeuft. Auf einem 48-kHz-Geraet
liefen dadurch alle Dialogzeilen 8,8 % zu schnell, und der Riegel, der eine Zeile
ausreden laesst, rechnete mit derselben falschen Zahl und loeste zu frueh. Der
Film-Pfad hatte es laengst richtig gemacht - Sprache und Rotor ziehen jetzt nach.

## Alligator-Finisher: der Kau-Vorgang setzt Leon wieder

Waehrend des Fressens hat der Port den Alligator ueberhaupt nicht mehr positioniert -
er blieb auf der letzten Stellung der vorigen Phase stehen, und die lag wegen eines
Ueberlaufs in der Bildzaehlung 329 Einheiten zu weit hinten. Dazu lief der Kau-Zyklus
mit der Laenge einer anderen Animation. Beides ist behoben; Leon rueckt damit in den
Kopfbereich des Mauls.

*Noch nicht ganz:* vollstaendig im Maul sitzt er erst, wenn die Verkleinerung des
Alligator-Modells entfaellt - die stammt aus einer frueheren Rueckmeldung ("ein wenig
zu gross fuer den Raum") und wuerde den ganzen Bosskampf wieder veraendern. Das bleibt
bewusst fuer eine eigene Runde.

## Noch offen

Leon scheint in ROOM1050 stellenweise durch die Vordergrund-Objekte durch. Die Ursache
ist gefunden (ein gekippt stehender Kaffeeautomat bekommt spaltenweise gestaffelte
Tiefen, die mitten in Leons eigener Tiefe liegen), aber die naheliegende Korrektur
bringt nur die Haelfte - und die Messschiene, mit der das geprueft wird, erfasst nur
14 % der gezeichneten Dreiecke, sobald ein Gegner im Raum ist. Beides braucht eine
eigene Runde; die Befunde stehen in analysis/befunde_2026-09-14/.

# v0.8.3 - 2026-09-14

## Die kurze Trefferpause gilt jetzt fuer alle Gegner

- **Nicht mehr nur der Hund.** In Resident Evil 2 ist ein Gegner nach einem Treffer
  kurz nicht erneut treffbar - eine halbe Sekunde, unabhaengig davon, was er gerade
  fuer eine Animation spielt. Seit v0.8.2 galt das im Port nur fuer die Hunde; jetzt
  auch fuer Kraehen und Spinnen. Die Zombies hatten es ueber ihren eigenen Filter
  schon.
- **Und sie kann nicht mehr haengenbleiben.** An vier Stellen hat der Port die Pause
  nicht heruntergezaehlt - ein wartender Zwingerhund etwa blieb danach dauerhaft
  gesperrt. Alle vier sind geschlossen und nachgemessen. Zusaetzlich gibt der Riegel
  jetzt nur noch frei, statt auch zu sperren: falls irgendwo doch einmal ein Takt
  ausfaellt, ist ein Gegner hoechstens frueher treffbar - nie fuer immer unverwundbar.
- Nebenbei bekamen die Feuer-Emitter in ROOM1090 die Pause ab, obwohl sie gar keine
  Gegner sind. Auch das ist behoben.

### Was sich im Spiel aendert

Wenig, und das ist Absicht: die Pause ist meist kuerzer als der Waffentakt. Spuerbar
wird sie im Nahkampf und bei Waffen, die schnell nachladen - dort verschluckt das
Spiel jetzt nicht mehr Treffer, sondern haelt den Original-Rhythmus ein.

# v0.8.2 - 2026-09-14

## Die Hunde lassen sich wieder treffen

- **Ein angeschossener Hund ist nur noch eine halbe Sekunde unverwundbar statt vier.**
  Bisher konnte man ihn erst wieder treffen, wenn er nach dem Beschuss vollstaendig
  aufgestanden war. In Resident Evil 2 gibt es das nicht: dort sperrt nach einem
  Treffer eine kurze Pause von 15 Bildern, und die hat mit der Animation nichts zu
  tun. Der Port benutzte dafuer einen Riegel aus Resident Evil 1.5, den RE2 gar nicht
  kennt - und gab ihn erst am Ende der ganzen Zuck-Kette frei. Gemessen: 120 Bilder
  vorher, 15 danach. Unter RE1.5-KI bleibt alles wie es war, dort sind die 19 Bilder
  original.
- **Der erste Treffer kommt nicht mehr verzoegert.** Zwei Ursachen, beide behoben:
  Wer nach unten zielte, traf den Hund unter RE2-KI ueberhaupt nie - jeder Schuss
  wurde vor dem Treffertest still verworfen, weil der Port ihn auf "waagerecht"
  festgenagelt hatte. RE2 kennt diese Einteilung gar nicht. Und ein im Ruhezustand
  getroffener Hund zuckte erst eine halbe Sekunde spaeter, weil die Reaktion nach dem
  falschen Merkmal ausgewaehlt wurde (Herkunfts-Zustand statt Waffe).
- **Die F9-Marke sagt jetzt, ob ein Gegner ueberhaupt treffbar ist** - und wenn nicht,
  warum. Damit laesst sich so etwas kuenftig direkt im Spiel nachweisen, statt zu
  raten, ob man danebengeschossen hat.

### Zurueckgenommen

Die Meldung aus v0.8.1, ein niedergeschlagener Zombie sei 7 bis 13 Sekunden lang
unverwundbar, war falsch - der Fehler lag in meiner Messung, nicht im Spiel. Gegen das
Original geprueft ist die Sperre dort voellig unauffaellig.

### Offen

Die kurze Trefferpause ist bisher nur beim Hund scharf. Bei Zombie, Kraehe und Spinne
muss erst nachgemessen werden, in welchen Zustaenden ihr Zaehler ueberhaupt laeuft -
sonst tauscht man "zu lange gesperrt" gegen "nie wieder treffbar".

# v0.8.1 - 2026-09-14

## Die schnellen Zombies laufen jetzt wirklich schneller

- **Der Schnellgang war im Port halb so schnell statt anderthalbmal.** In Resident
  Evil 2 ueberspringt der schnellere Zombie jeden dritten Tick ein Animationsbild und
  legt dafuer den doppelten Weg zurueck. Der Port liess den Schritt an diesen Ticks
  einfach aus - gemessen kam er auf Faktor 0,50 statt 1,50. Jetzt stimmt beides auf
  die Einheit genau mit dem Original ueberein.
- **Damit ist auch der Verdacht vom letzten Mal ausgeraeumt**: nicht die
  Wand-Kollision vertrug das hoehere Tempo nicht - die arbeitet wie im Original und
  laesst mit 443 Einheiten groesstem Schritt gegen 1158 Einheiten duennster Wand
  nichts durch. Der Fehler sass in der Bewegung selbst.
- **Der Zombie, der in ROOM1030 zurueckbleibt, hat einen ganz anderen Grund**: das
  Skript schaltet nur die ersten vier scharf, und er erreicht seine Zone 50 Bilder zu
  spaet. Mit dem Tempo hat das nichts zu tun - er ist nicht einmal einer der
  schnellen. Die falsche Erklaerung von gestern steht jetzt richtig im Code.
- **Alle Sprachaufnahmen tragen wieder eine gueltige Groessenangabe.** 68 der 87
  Dateien hatten eine kaputte (ein Artefakt des Exports). Gehoert hat man davon
  nichts, aber seit der Dialog-Riegel an der Laenge haengt, sollte sie stimmen. Die
  Tondaten sind dabei Byte fuer Byte dieselben geblieben.

### Bekannt und offen

In einem von 64 Durchlaeufen ist ein niedergeschlagener Zombie 7 bis 13 Sekunden
lang nicht treffbar, bevor er sich wieder faengt. Das ist lang; ob das Original sich
dort genauso verhaelt, ist noch nicht nachgemessen.

# v0.8.0 - 2026-09-14

## Jeder dritte Zombie laeuft schneller, und die Dialoge reden aus

- **Etwa jeder dritte Zombie ist schneller unterwegs.** In Resident Evil 2 bekommt
  beim Spawn rund ein Drittel der Zombies einen 1,5-fachen Gang - und dieselben sind
  zugleich die zaeheren, die auf Treffer weniger zucken. Der Port hatte den Schalter
  dafuer, benutzte ihn aber nie, weil er an einem RE2-Spielglobal haengt. Das Global
  ist im Auslieferungsstand ab dem Start IMMER gesetzt und wird nirgends geloescht -
  es gab also nichts abzubilden. Jetzt laufen sie wie im Original.
- **Die Sprachaufnahmen werden nicht mehr abgeschnitten.** Erhoben wurde der ganze
  Bestand: von 87 Aufnahmen brachen 24 mitten im Wort ab, weil die naechste Zeile zu
  frueh startete, sechs weitere waren auf der Kippe. Am schlimmsten traf es Irons in
  ROOM1150, dem ueber drei Sekunden fehlten. Der naechste Satz wartet jetzt genau so
  lange, wie der vorige noch braucht - keine geschaetzte Zahl mehr, sondern die
  tatsaechliche Restlaenge.
- **Und er wartet nur noch dort, wo es Dialog ist.** Save-Telefon, Item-Box und die
  Ja/Nein-Abfragen reagieren wieder sofort; vorher konnte es bis zu drei Sekunden
  dauern, bis ueberhaupt etwas passierte, wenn nebenher noch eine Stimme lief.

### Bekannt und offen

Mit dem schnelleren Gang bleibt in ROOM1030 ein zweiter Kriecher in der Wand haengen
(Laufanimation ohne Fortkommen) - die Wand-Klemme des Ports vertraegt das hoehere
Tempo noch nicht. Das ist gemessen und notiert, aber in dieser Fassung nicht behoben.

# v0.7.99 - 2026-09-14

## Die Gitterhaende greifen, ROOM1090 ist auf der Karte, das Intro redet aus

- **Die Arme im Korridor von ROOM1210 greifen jetzt beim Vorbeigehen - einmal.**
  Gemessen fuhren vorher NULL von zehn aus: das Tor mass den Abstand zur
  ausgefahrenen Hand, und der begehbare Flur ist 109 Einheiten zu weit von ihr
  entfernt. Dazu las der Port den Original-Ausloeser gar nicht - das Skript
  schaltet die Arme scharf (Aot_set im Flur, dann Member_set auf allen zehn),
  und die KI liest dieses Byte. Beides ist zu; der Radius sitzt jetzt am Arm
  selbst und zaehlt den Koerperradius des Spielers mit. Wer gegriffen und wieder
  losgelassen wurde, bekommt denselben Arm im selben Raumbesuch nicht noch
  einmal - im Original kommt er auch nie zurueck.
- **ROOM1090 steht auf der Karte**, beide Ebenen: der Hinterhof mit dem
  verunglueckten Polizeitransporter auf dem 1F-Blatt, die Dachterrasse darueber
  auf dem 2F-Blatt. Dem Raum fehlte keine Kunst, sondern seine Zeile - die
  Etagenangaben zeigten ins Leere, deshalb gab es weder Marker noch Faerbung.
  Die gemalte Kachel gab es laengst; sie war an ROOM10B0 vergeben, der auf dem
  Blatt gar keine Tuer hat, waehrend auf ihr das Tuerblatt nach ROOM1050 sitzt.
- **Im Intro redet Elliot aus.** "They almost caught me" brach nach "They almost
  caught..." ab, weil die naechste Zeile die laufende Aufnahme abwuergt: das
  Skript gibt ihr 101 Bilder, die Aufnahme ist 117 lang. Der naechste Satz
  wartet jetzt, bis der vorige zu Ende ist - nur dort, wo es noetig ist; sechs
  der acht Zeilen passen ohnehin.
- ROOM10B0 hat dadurch vorerst kein eigenes Kartenrechteck mehr. Das Blatt haelt
  eine passende Kachel bereit, die aber in keiner Tabelle steht; sie kommt, wenn
  das Blatt eine eigene Rechteckliste bekommt.

# v0.7.98 - 2026-09-13

## Abgetrennte Beine bleiben liegen, kein Landen in der Wand, Hunde bluten

- **Die abgetrennten Beine bewegen sich nicht mehr mit.** Seit der
  Becken-Trennung behielt die Unterhaelfte die Pose des Rumpfes und zappelte
  deshalb weiter. Sie friert jetzt im Moment der Trennung ein. (Im Original hat
  sie eine eigene, kurze Ausklang-Animation - die fehlt weiterhin und steht so
  im Code.)
- **Beim Gebissen-werden landet man nicht mehr in der Wand.** Die Klemme, die
  genau das verhindert, gab es seit Runde 6 - sie galt aber nur fuer zwei
  Greifer-Typen, und der Zombie war nicht dabei. Gemessen an der Nutzer-Marke:
  der Spieler stand drei Einheiten innerhalb der Wandzelle von ROOM1220.
- **ROOM11F0 Cut 0** hatte als einziger Kamerawinkel des Boiler Rooms keine
  Vordergrund-Maske; Tank und Rohr sind jetzt gezogen.
- **Tote Hunde hinterlassen wieder Blut.** Der Port kannte die Effekt-Tabelle des
  Hundes zwar, benutzte sie aber nicht - er schickte fuer jeden Effekt dieselbe
  Art. Statt Bluttropfen UND Spritzer kamen dreimal Tropfen heraus.

# v0.7.96 - 2026-09-13

## Boiler Room: die Vordergrund-Objekte verdecken jetzt

- **ROOM11F0 hat Masken** - aus den Freistellungen des Nutzers (Cut 1, 6 und 7).
  Vorher hatte der Raum keine einzige, deshalb wurden Rohre, Tank und Gitterrost
  hinter den Spieler gezeichnet: es sah aus, als stuende er obendrauf.

# v0.7.95 - 2026-09-13

## Der Zombie trennt sich am Becken

- **Schrotflinte: der Zombie bleibt nicht mehr ganz.** Im Original faellt nie der
  Oberkoerper ab - das Spiel trennt den Zombie am BECKEN: Huefte und Beine
  werden aus dem Skelett ausgehaengt, rutschen weg und bleiben liegen, waehrend
  der Rumpf zusammensackt. Die erste Haelfte davon hatte der Port seit heute
  frueh (der Bodenanker gegen das Versinken), die zweite fehlte - und schlimmer:
  der Port loeste den Aushang genau dort wieder, wo das Original die
  Wegschieb-Maschine erst startet. Gemessen: der Abstand zwischen Brust und
  Huefte bleibt bis Bild 39 bei null und waechst dann auf rund 550, bevor die
  Unterhaelfte einfriert.
- **Kartenfarbe der ROOM1000-Raeume, zweiter Anlauf.** Mein erster Versuch nahm
  die Zahlen aus dem Kachel-Code - das sind aber Modulations-Faktoren, keine
  Farben: eine gemalte Kachel ist eine Textur, die damit multipliziert wird, das
  Ergebnis ist viel dunkler. Eine einfarbige Flaeche mit demselben Wert leuchtet
  daneben grell. Die Toene sind jetzt am Karten-Abzug des Nutzers GEMESSEN
  (besuchte Kachel 0,64,40 - Linien 48,192,48 - aktuelle Kachel 80,16,0).
- **ROOM1000 Cut 0 und Cut 2 haben endlich Masken.** Fuer diese beiden
  Kamerawinkel gab es keine, deshalb verdeckte dort nichts: die Spindbloecke
  links und rechts sowie die Bank im Vordergrund sind jetzt gezogen.

# v0.7.94 - 2026-09-13

## Der YOU-DIED-Bildschirm kommt jetzt waehrend des Fressens

- **Die Todes-Praesentation wurde jeden Frame wieder abgeraeumt.** Das eingebaute
  Log hat es gezeigt: 241 Mal hintereinander startete sie und wurde im selben
  Moment zurueckgesetzt. Es gibt zwei Schalter dafuer, und gestern hatte ich nur
  einen umgestellt - der zweite prueft, ob der Spieler "tot" ist, und genau das
  ist er waehrend des Fressens absichtlich nicht (der Alligator besitzt ihn).
  Jetzt laufen beide auf derselben Bedingung, und der Bildschirm setzt mitten in
  der Fress-Animation ein.
- **Die ROOM1000-Raeume haben nicht mehr ihre eigene Farbe.** Raeume mit gemalter
  Kachel werden gruen (besucht) bzw. rot (aktuell) eingefaerbt; die gerechnete
  Zeichnung fuer ROOM1000 benutzte einen ganz anderen Satz - blaue Fuellung, weisse
  Kante - und stach als Fremdkoerper heraus. Sie traegt jetzt dieselben Toene.

Weiter offen und gemessen (nicht geraten): Leon wird beim Fressen noch nicht im
Maul gezeichnet, Birkin erscheint noch nicht, und die neun PRI-Marken des
Spiellaufs sind noch nicht abgearbeitet.

# v0.7.93 - 2026-09-13

## Der Raum fror ein, weil mein eigener Fix den Spieler abschaltete

- **ROOM5090 haengt nicht mehr fest, Birkin kommt wieder.** Der gestern
  eingebaute "neuer Kampf = neue Arme"-Reset schaltete beim Betreten des Raums
  als Erstes Leon selbst ab: die Arm-Liste ist beim Start mit Nullen gefuellt,
  und Platz 0 ist der Spieler. Der Raum fror ein (im Log zweimal reproduziert,
  einmal 575 Bilder lang auf derselben Stelle), und weil die Boss-Steuerung
  einen abgeschalteten Spieler las, erschien auch Birkin nie. Der Reset fasst
  jetzt nur noch echte Arme an.
- **Karte ROOM1000: nur noch der Raum, in dem man wirklich steht.** Die Karte
  hatte pro Raum nur zwei Merkzettel fuer "hier war ich schon" - ab dem dritten
  Bereich teilten sich zwei Raeume einen. In ROOM1000 (Ostraum + zwei Toiletten)
  erschienen deshalb beide Toiletten auf einmal. Jeder Bereich hat jetzt seinen
  eigenen; alte Spielstaende bleiben lesbar. ROOM2070 war still davon betroffen
  und ist mit repariert.
- **Fress-Finisher: gemessen statt vermutet.** Die Kette stimmt rechnerisch -
  der Bildschirm setzt 44 Bilder vor dem Ende der Fress-Animation ein. Der
  verbleibende Verdacht ist die Todes-Kamera, die auf den Spieler zoomt,
  waehrend der im Maul haengt. Dafuer gibt es jetzt eine Mess-Schiene
  (RE15_FINISHER_LOG=1), die der naechste Spiellauf beantwortet.

# v0.7.92 - 2026-09-13

## Runde 8: Beine bleiben am Boden, Birkin bewegt sich selbst, der Alligator bruellt

- **Zombies versinken nicht mehr beim Schrotschuss.** Der zweite Anlauf hat den
  richtigen Taeter gefunden: dass der Koerper beim Sturz absinkt, ist korrekt -
  aber im Original sinkt nur der RUMPF. Becken und beide Beine haengt das Spiel
  im Moment des Aufschlags aus der Knochenkette aus und nagelt sie auf den
  Boden; der Port zog die ganze Figur mit nach unten. Gemessen lag der tiefste
  Bein-Knochen bis zu 1,4 m unter dem Boden, jetzt steht er ueber den ganzen
  Sturz bei 21 cm darueber - wie im Original. Nebenbei startet die anschliessende
  Kriech-Animation nicht mehr aus der Stehpose (sie zog die Figur erst 1,6 m
  hoch, bevor sie absackte).
- **Birkins Bewegung haengt nicht mehr am Spieler.** Der Port hatte jeden
  Schritt des Bosses als Abstand zu Leon verbucht statt als eigene Bewegung.
  An den Bewegungsgrenzen kippte das um: beim Heranziehen stand Birkin
  buchstaeblich IN Leon drin (gemessen 243 Bilder am Stueck), beim Rueckzug
  sprang er auf einen festen Abstand. Er bewegt sich jetzt auf seiner eigenen
  Achse, wie im Original.
- **Die vier Tentakel behielten ihre Plaetze.** Beim Raumwechsel oder einem
  zweiten Kampf gaben sie ihre Aktor-Plaetze nie frei - die Arm-Steuerung
  schrieb danach auf Gegner, die laengst jemand anderes waren. Ausserdem haengen
  die Arme jetzt an der richtigen Hoehe (vorher steckten sie zwei bis vier Meter
  unter dem Boden).
- **Der Alligator hat seine Schreie.** Ein Vollzensus des Original-Codes ergab
  zehn Tonquellen, der Port spielte drei. Es fehlte vor allem der grosse
  Schmerzensbrueller - das Original hat zwei getrennte Treffer-Reaktionen mit
  je eigenem Laut, der Port spielte immer den kleinen. Und Leon schreit jetzt
  auch, wenn der Alligator ihn packt.
- **Die Schadensanimation ist staerker.** Drei Dinge aus dem Original
  uebernommen: jeder Treffer wirft die laufende Bewegung zwei Bilder zurueck,
  die Zuck-Animation schneidet hart hinein statt weich zu blenden, und sie
  traegt ihre Rueckwaerts-Bewegung - der Alligator weicht beim Treffer zurueck,
  statt auf der Stelle zu zucken.
- **Gefressen wird jetzt WAEHREND des YOU-DIED-Bildschirms.** Im Original ist
  die Fress-Sequenz kein Vorspiel zum Game-Over, sondern laeuft mittendrin -
  beide Vorlagen starten die Todes-Praesentation in dem Moment, in dem das Maul
  zuschnappt. Der Port machte es umgekehrt: erst fressen, dann Bildschirm.
- **Karte ROOM1000**: Der Raum ist gar nicht die Lobby, sondern drei getrennte
  Bereiche - ein Ostraum und zwei Toiletten. Die Karte pinnte ihn auf die
  Hallenkachel des Nachbarn; der Marker landete mitten in der Halle. Der Raum
  ist auf dem Blatt ueberhaupt nicht gezeichnet und bekommt deshalb drei eigene
  Kaesten, genau an die Flurwand angesetzt.
- **Karte ROOM1050**: Der Marker stand acht Pixel im Raum, obwohl der Spieler an
  der Wand klebte - die Kartenzeile war in der Tiefe daneben. Vier unabhaengige
  Messpunkte legen die neue fest; jetzt steht der Marker an der gemalten Wand.
- **30 neue Vordergrund-Masken.** ROOM1000, ROOM1010 und ROOM1050 bekommen
  erstmals welche, ROOM2090 zwei weitere Ansichten - Waschbecken, Kabinenwaende,
  Getraenkeautomat, Leiterschacht und Betonplattform verdecken Leon jetzt
  richtig.

# v0.7.91 - 2026-09-13

## Birkin bekommt seine Tentakel - und die Fleischmasse lebt

- **Die vier Tentakel sind da.** Im Original sind sie keine Teile des
  Boss-Modells, sondern vier eigene Kreaturen, die Birkin per Befehl steuert -
  deshalb fehlten sie bisher komplett. Jetzt fahren sie beim Auftauchen aus der
  Fleischmasse heraus (zwei je Seite, je einer hoch und einer tief), haengen
  jeden Moment an der Masse und wandern mit ihr mit, peitschen und stechen nach
  Leon (15 Schaden) und ziehen sich am Ende des Auftritts wieder ein. Birkin
  waehlt dabei wie im Original einen gerade freien Arm aus; unverwundbar sind
  sie auch dort.
- **Die Masse pulsiert, holt aus und schnappt zu.** Ihr Verformen ist kein
  Trick, sondern ein eigener Datenblock im Modell: vier gespeicherte Zielposen,
  gegen die das Original jeden Frame interpoliert. Dieser Block ist jetzt
  entschluesselt und wird byte-genau nachgerechnet - die Masse atmet im
  Leerlauf, holt vor dem Biss aus, schnellt vor und zerfliesst im Tod.
- Dabei sind zehn Rechenfehler in der Steuerung der Verformung aufgefallen und
  behoben worden (sie waren unsichtbar, solange nichts verformt wurde): das
  Atmen war zu flach und zu hart, das Zucken der Leiche dreissigmal zu schwach,
  das Aufgehen der Masse beim Heranrobben fehlte ganz.

# v0.7.90 - 2026-09-13

## Runde 7: Maul, Boden, Spinnen-Leiche, Karte 1050, Birkins Auftritt

- **Der Alligator hat Leon jetzt wirklich im Maul**: Der Finisher spielte die
  Opfer-Animation zwar ab, aber die Todes-FSM des Spielers lief parallel weiter
  und riss ihn zurueck (sie loeschte den Opfer-Modus und startete den
  YOU-DIED-Ablauf mitten in der Sequenz). Jetzt BESITZT die Fress-Sequenz den
  Spieler von der Sekunde des toedlichen Bisses an - Leon strampelt im
  Hochhebe-Clip, wird geschleudert und haengt am Ende im kauenden Maul; das
  Game-Over kommt erst danach.
- **Zombies versinken nicht mehr im Boden**: Der Schrot-Sturz merkt sich beim
  Aufschlag die Bodenhoehe. Er nahm dafuer bisher die AKTUELLE Hoehe des
  Zombies - war der schon abgesackt, wurde die Senke festgeschrieben und alles
  Weitere baute darauf auf. Jetzt fuehrt der Port (wie das Original) ein
  eigenes Boden-Register mit.
- **Die tote Spinne bleibt liegen**: Nach dem Tod zeigte der Port fuer eine
  halbe Sekunde das ERSTE Bild der Todesanimation (aufgebaeumt), bevor der
  Liege-Clip sie wieder hinlegte - das Original haelt stattdessen das letzte
  Bild. Ausserdem war eine sterbende Spinne weiter beschiessbar: jeder
  Nachtreffer startete die Todesanimation neu und spuckte weitere Baby-Spinnen
  aus. (Dass ueberhaupt Babys herauskommen, ist original so.)
- **Karte 1F, ROOM1050**: Die drei Tueren zum Empfangsflur fehlten - ihre
  Symbole sassen auf der falschen Kachelkante, weil die Zeichnung dieses Raums
  um 180 Grad gedreht montiert ist. Sie sitzen jetzt auf der richtigen Wand.
  Und das fehlende Wandstueck war ein zwei Pixel breiter Mauerversatz, den die
  automatische Tuerschwenk-Erkennung als Schwenk entfernt hatte.
- **Birkin kommt von der richtigen Seite**: Er startete am Ende des Zuges
  HINTER dem Spieler. Nachgemessen (Tuerdaten, Kollisionsstreifen und der
  Spawn-Eintrag des Auslieferungs-Skripts): Er gehoert weit nach vorn in den
  Zug - dorthin, wo Leon hinlaeuft - und kriecht ihm sein Original-Intro
  entgegen. Der Kampf rechnet ausserdem in Abstaenden zum Spieler, damit die
  Original-Schwellen im laengeren Zugkorridor stimmen.
  (Noch offen: Tentakel und das Pulsieren der Fleischmasse - beides ist
  spezifiziert und in Arbeit.)

# v0.7.89 - 2026-09-12

## Runde 6: die beiden Bosse bekommen ihre echte RE2-Seele

- **Endboss-Birkin kaempft jetzt wie in RE2** (neues Boss-Modul, kompletter
  Overlay-Zensus): das grosse Auftauch-Intro mit echter Kriech-Bewegung aus
  den Animationsdaten, der TENTAKEL-ZUG nach vorn (Clip-Spur x1,5), der
  Rueckzug ans Korridorende, die atmende Fleischmasse mit dem MASSEN-BISS
  (Ausholen, Zuschnappen, Vorstoss, 40 Schaden), der Verschling-Kill in der
  Naehe, ein Treffer-Wut-System mit Stagger und die volle Todes-Sequenz
  (Einbruch, Zucken, Kollaps in halber Geschwindigkeit, Versinken in die
  Masse). Die frueheren Naeherungen (Clip-Umsortierung, 2950er-Hoehenanker)
  sind raus - sie beruhten auf einer Fehldeutung: die vermeintliche
  Kampfhoehen-Rampe ist in Wahrheit die Todes-Phase.
- **Der Alligator-Finisher ist der echte**: Resident Evil 2 wirft Leon nicht
  per Formel durchs Bild - es spielt eine eigens animierte OPFER-ANIMATION
  auf Leons Skelett ab (120 Bilder Rumschleudern, bis 9 Meter hochgerissen,
  seitlich geschleudert), synchron zum Schuettel-Clip des Alligators und nur
  ueber einen gemeinsamen Anker gekoppelt. Genau das laeuft jetzt im Port:
  Leon strampelt, das Maul schnappt bei Bild 13 zu, dann fliegt er die
  authored Bahn und haengt am Ende sichtbar im kauenden Maul.
- **Schrot-Bauchtreffer**: die Ragdoll-Bodenklemme rechnete mit der rohen
  statt der gerenderten Wurzel (Zombie lag eine halbe Koerpertiefe zu tief);
  drei weitere Verdachtsmomente wurden am Original geprueft und als
  byte-gleich belegt. Fuer den seltenen 'taumelt unter dem Boden'-Moment
  liegt jetzt eine Dauer-Messschiene bereit (RE15_RE2_TRACE=1) - ein F9 im
  richtigen Moment benennt den Verursacher exakt.

# v0.7.88 - 2026-09-12

## Runde 5 der Spieltest-Befunde

- **Der Endboss ist zurueck im Kampf** (Regression aus v0.7.87): das
  Kampfstart-Skript setzt erst die Position und einen Tick spaeter die
  Freigabe - der neue Park-Modus ueberschrieb in diesem Fenster die
  Kampfposition und der Boss stand unsichtbar ausserhalb der Welt. Jetzt
  beendet schon die gesetzte Position das Parken; Sichtlauf bestaetigt.
- **Der Kopf zerplatzt wie in RE2, statt davonzufliegen**: abgetrennte
  Koepfe/Arme wurden als intaktes Modell auf der Flugbahn gezeichnet. Das
  Original zeichnet solche Teile NIE als Mesh - es versetzt jedes Dreieck
  entlang seiner Normale nach aussen (ueber 30 Bilder von 30 auf 465
  anwachsend) und faerbt alles dunkelrot: eine auseinanderberstende
  Scherbenwolke, die nach 29 Bildern verschwindet. Beine fliegen weiterhin
  intakt - wie im Original.
- **Lobby-Zombies stehen normal auf**: der Skript-Rueckweg aus dem Kriechen
  schaltete in den STOSS-Executor (rueckwaertsschleudernder Taumel mit
  Sturz) statt in die Boden-Aufstehkette - das war das 'komische Fliegen'
  nach dem Aufstehen.
- **Karte 1F ab ROOM1050 repariert**: der Raum sass auf der falschen Kachel
  und verschmolz mit Empfang/Lobby zu einem braunen Block; jetzt wohnt er
  (spiegelverkehrt eingemessen wie seinerzeit 1010) auf seinem eigenen
  Streifen, die Tuermarken sitzen auf den Waenden.
- **Alligator-Finisher**: Leon steckt jetzt in Laufrichtung des Mauls und
  bleibt beim Rumschuetteln daran gekoppelt (wedelt MIT dem Maul, statt
  frei zu trudeln). Der Zubeiss-Sound kommt jetzt aus der RE1.5-Raum-Bank
  von ROOM2090 (der Angriffs-Impakt des Raums) statt des hellen
  Wasser-Klatschers.
- **Torso abschiessen - Klarstellung nach RE2-Original-Messung**: die
  NORMALE Schrotflinte reisst auch im RE2-Original keinen Rumpf ab (ihre
  Todeszeile hat keinen Zerreisser); das kann nur die aufgeruestete CUSTOM
  Shotgun - deren Gegenstueck im Port die SPAS-12 ist (liegt in ROOM2030).
  SPAS + nach oben zielen zerreisst garantiert, mit Rumpf-Stumpf.

# v0.7.87 - 2026-09-12

## Runde 3/4 der Spieltest-Befunde: Gore-Vollausbau, Spinnen-Tod, Fress-Finisher, Endboss

- **Die RE2-Brutalitaet ist jetzt komplett verdrahtet**: Der Port dekodiert die
  gepackten RE2-Effekt-Kennungen und spielt sie ueber die richtigen
  RE1.5-Effektbaenke ab - die Kopf-Explosion feuert ihre drei Original-Spawns
  am HALS (Blutstrahlen, Fleischbrocken Richtung Spieler, Fontaene), der
  Fleischbrocken-Regen beim Zerreissen/Burst kommt aus den byte-identischen
  Brocken-Sheets der Raeume, Brand-/Aetz-Treffer haben ihre Effektklassen.
- **Torsos abschiessen wie in RE2**: Der SPAS-Zerreisstod zeigt jetzt den
  RUMPF-Stumpf und den Bein-Stumpf - die Original-RE2-Stumpf-Geometrie samt
  ihrer eigenen Textur faehrt als Seitenbank mit (das RE1.5-Modell hat diese
  Meshes nicht). Und wie im Original gilt: SPAS + nach oben zielen zerreisst
  GARANTIERT, sonst wuerfelt das Spiel.
- **Die Spinnen-Todesanimation spielt nur noch EINMAL**: Das Original schaltet
  vom Tod zur Leiche am Markierungs-Bit der Animationsdaten (ab Bild 180 von
  206) - die alte Naeherung wartete das Clip-Ende ab und spielte den Clip
  danach komplett neu. Die schwarzen Dreiecke darueber sind weg: das waren
  RE1.5-Feuer-Anker-Platzhalter, die der Modell-Hybrid faelschlich ueber die
  Baby-Spinnen stuelpte.
- **Der Alligator-Fress-Finisher ist synchron**: Leon fliegt jetzt ins LIVE
  gerenderte Maul (die Bahn folgt den Kiefer-Knochen), haengt beim zweiten
  Zuschnappen zwischen den Kiefern und wird verschlungen - vorher klaffte der
  Zielpunkt um zwei Meter unter dem Maul. Der Biss-Sound ist der kurze
  perkussive Schnapper der Alligator-Bank statt des 2,4-Sekunden-Bruellers,
  dessen Hoehepunkt erst lange nach dem Biss kam.
- **Der Endboss kaempft jetzt richtig**: Er wartet (wie in RE2 geparkt) bis zum
  Kampfstart-Ereignis, statt ab Raumladung gegen die Zugwaende zu laufen; seine
  Angriffe messen zur FLEISCHMASSE-Front statt zum Koerpermittelpunkt (die
  Masse ueberrollte vorher den Spieler, und die Klauen-Schleife fror ein); das
  Auftauchen und die Angriffs-Erholung spielen passende RE2-Clips statt des
  100-Bilder-Idles.

# v0.7.86 — 2026-09-12

## Vier Spieltest-Befunde der zweiten Runde

- **Kopf wegschiessen funktioniert**: nah vor dem Zombie hochzielen und
  abdruecken laesst den Kopf platzen — ueber RE2s echten Mechanismus (die
  Rumpf-Todeszelle prueft das Hochziel-Bit und springt deterministisch in die
  Kopf-Explosion; eine Kopf-"Spalte" hat der RE2-Zombie gar nicht).
- **Der Alligator hat eine Stimme**: Biss beim Zuschnappen, Grunzen bei
  Treffern (mit der originalen 55-Bild-Sperre), dazu die in den Animationsdaten
  kodierten Sounds beim Schwimmen und in der Todesrolle — alles aus der
  RE2-Soundbank des Alligators.
- **Der Endboss sieht aus wie in RE2**: Fleischmasse und wogender Vorderleib
  sind EIN verbundener Koerper — die Masse sitzt am Boden verankert, der
  Kriecher-Vorderkoerper haengt daran und wogt mit der Animation; der Arm ist
  angebaut. (Noch offen: das Pulsieren der Masse per Vertex-Morph und die
  Uebergangs-Naehte — kommt in einer eigenen Runde.)
- **ROOM1010 zeichnet sich auf der Karte**: der Raum war komplett in die Kachel
  des Nachbarraums gespiegelt; jetzt wohnen seine zwei Kammern auf den eigenen
  Kacheln, und die Tuersymbole binden richtig.

# v0.7.85 — 2026-09-12

## Spinnen & Huelsen

- **Die Spinnen sind nicht mehr "immun"**: Sie waren nur mit exakt ebener
  Zielhoehe treffbar — wer (voellig natuerlich) auf das flache Bodentier nach
  unten zielte, verfehlte immer. Jetzt treffen EBEN und TIEF die Bodenspinne,
  HOCH die Deckenspinne — nach den Hoehenfenstern aus der RE2-EXE.
- **Auch die Huelsen der Beretta/Glock-Bursts** bleiben nicht mehr in der Luft
  haengen (zweite fehlende Original-Routine nachgebaut).

# v0.7.84 — 2026-09-12

## Sieben Befunde aus dem Spieltest behoben

- **Schrotflinte: der Nah-Kopfschuss nach oben funktioniert** — wie in RE2: nah
  vor dem Zombie hochzielen trifft den Kopf (mit der Pistole geht das wie im
  RE2-Original nicht, und aus der Ferne laesst sich nicht nach oben snipen).
  Die Hoehenfenster aller Waffen stammen aus der RE2-EXE.
- **Auf den Kopf zielen bringt nicht mehr WENIGER Gore**: leere Zellen der
  RE2-Treffertabellen werden auf die Rumpf-Reaktion geklemmt — im Original sind
  sie unerreichbar, im Port fiel der Treffer seit den Trefferzonen ins Leere.
- **Schrot-Huelsen fliegen nicht mehr ewig auf der Stelle**: die fehlende
  Original-Routine 38 (Haltedauer, Auswurf mit Streuung, Boden-Despawn) ist da.
- **Game Over/Finisher: keine PRI-Schnipsel mehr ueber der Todesszene** — die
  Vordergrund-Masken gehoeren zum Raumbild und gehen jetzt mit ihm.
- **2F-Karte: die frei schwebende Tuermarke ist weg** — eine Marke erscheint nur
  noch, wenn unter ihr wirklich etwas gezeichnet ist.
- **Endkampf-Birkin steht auf dem Boden und ist der richtige Koerper**: das
  RE2-Modell traegt zwei Gestalten — gekaempft wird der animierte Kriecher, die
  starre 7-Meter-Zugkulisse bleibt verborgen. Wurzelhoehe je Animationsbild
  geerdet, Clip-Zuordnung der KI auf den RE2-Clipsatz.
- **Und er ist besiegbar**: die Endform ist im Original waffenimmun — im
  Endkampf erbt sie jetzt die Schadenswerte der Form, die sie ersetzt.

# v0.7.83 — 2026-09-12

## Waffen: Dauerfeuer + eigene Muendungseffekte je Waffe

Grundlage ist die vollstaendige Einzeldisassemblierung aller Waffen-Handler des
Originals (Listings unter analysis/waffen_fsm_2026-09-12/).

- **Ingram M10, Flammenwerfer und H&K MC51 feuern jetzt DAUERFEUER**, solange
  der Abzug gehalten wird — mit den Original-Feuerclips, dem Original-Takt
  (Ingram/MC51: 5 Schuss je 9 Bilder; Flammenwerfer: 12 je 21) und dem
  Abkling-Clip beim Loslassen. Zielhoehe laesst sich mitten im Feuern wechseln,
  ohne dass der Schusstakt neu ansetzt. Diese drei Waffen laden byte-true nie
  nach und klicken auch nicht bei leerem Magazin im Anschlag.
- **Jede Waffe spawnt ihre eigenen Muendungs-, Rauch- und Huelseneffekte**
  (bisher bekamen alle die der Browning HP): der Revolver wirft keine Huelse,
  die Schrotflinten haben ihren Grossblitz und eine eigene Schrothuelse, die
  Beretta/Glock-Bursts ihre eigene Blitzvariante.
- **Der Granatwerfer feuert wieder sein Projektil** — an genau den drei
  Rueckstoss-Bildern des Originals, abhaengig von der Zielhoehe. Wer R1 vor dem
  Abschussbild loslaesst, unterdrueckt die Granate (Munition trotzdem weg) —
  wie im Original.
- Kuriosum aus der Disassemblierung: die Raketen-Klasse (Waffen 15–18) haette
  im Original beim Feuern die PSX zum ABSTURZ gebracht (Null-Handler) — sie
  war schlicht unfertig. Im Port bleibt sie ueber eine dokumentierte Bruecke
  spielbar.
- Neuer Dauertest nagelt die komplette Kadenz fest — bis hin zum Detail, dass
  der Huelsen-Takt nach 126 Schuessen wegen eines vorzeichenbehafteten
  Ueberlaufs um vier Bilder springt (ja, das tut das Original wirklich).

# v0.7.82 — 2026-09-12

## Karte 1F — alle vier gemeldeten Punkte

- Die **Fahrstuhl-Tuer** im 1F-Korridor sitzt jetzt exakt auf der Wand — dieselbe
  Stelle wie auf 2F und 3F.
- Der **hintere kleine Raum** erscheint erst, wenn man die Etage auch betreten
  hat (bisher genuegte ein Schritt in denselben Raum auf einer anderen Etage).
- Die **zwei Strich-Reste** der Original-Tueren (Treppenhaus und Korridor) sind
  weg.

## Waffen

- **Dreier-Burst**: Beretta M93R und Glock 18 ziehen wieder drei Patronen pro
  Abzug statt einer.
- **Trefferzonen Kopf / Rumpf / Beine** sind freigeschaltet. Die Zone stand im
  Port auf einem festen Wert, wodurch zwei Drittel der Treffer-Reaktionen — und
  damit saemtliche daran haengenden Gore-Effekte — unerreichbar waren.

## Endkampf

- Modelle mit **mehr Koerperteilen als Knochen** werden vollstaendig gezeichnet.
  Der finale Birkin hat sieben Teile an zwei Knochen; bisher blieben fuenf davon
  unsichtbar.

# v0.7.81 — 2026-09-12

## Endkampf: kein Abspann mehr, dafuer der FINALE Birkin

- **Der Abspann ist raus.** Er sass nicht in STAGE6, sondern am Ende von
  ROOM5090 ("TRAIN CAR ABC"): dort spawnt der G-Birkin, dort laeuft die
  Cutscene, und von dort fuehrte die einzige Tuer in den Abspann-Raum — eine
  Diashow aus neun Bildern, die zum Titel zurueckwirft. Drei Riegel sorgen
  dafuer, dass daraus jetzt der Endkampf wird; die Cutscene selbst bleibt, weil
  sie den Boss an den Kampfplatz setzt und die Fluchttuer sperrt.
- **Der finale Birkin aus RE2 steht im Endkampf** (EM36 — in RE2 tritt er nur in
  der Endarena an, dem einzigen Raum ohne Ausgang) statt der fruehen Form.
- **Nebenbei repariert:** Der Birkin-Tod setzte sein Fortschritts-Flag nie — im
  Port war der Aufruf faelschlich als Gore-Effekt kommentiert. Acht Raeume haben
  den Tod dadurch nie mitbekommen.

## Waffen

- **Jede Waffe hat wieder ihre eigene Animation.** Bisher liefen Schrotflinte,
  Maschinenpistole und Magnum alle in der Handfeuerwaffen-Animation, weil der
  Port nur zwei Bewegungsbaenke kannte. Jetzt wird die Bank aus der Waffe
  abgeleitet — alle 21 liegen vor, mit deutlich verschiedenen Bewegungen.
- **Die Schrotflinte streut wieder**: vier Treffer je Schuss statt einem.

## Bosskampf ROOM2090

- Der Raum hat jetzt **Vordergrund-Masken** (Kamerawinkel 0–7): der Klaertank
  samt Sockel verdeckt die Spielfigur wieder korrekt.

# v0.7.80 — 2026-09-12

## Karte: der rote Bereich passt wieder zum Raum

- Der aktuelle Raum wurde bisher rot gefuellt, indem ein Schleier ueber das
  GANZE Rechteck des Kartenzeichners gelegt wurde — auch ueber dessen
  unbemalte Ecken. Dadurch wirkte z. B. das Treppenhaus auf allen Etagen viel
  breiter als seine Zeichnung, und grosse Flaechen erschienen rot.
  Gemessen am 3F-Abzug: rot lag auf 40x40 Pixeln, die gemalte L-Form des
  Raums nur auf 32x38.
- Jetzt folgt der Schleier der gemalten Kachel: wo der Zeichner nichts malt,
  bleibt auch kein Rot.
- Die Kartendaten selbst waren nie das Problem — 2F und 3F sind bei
  aufgedeckter Karte pixelgleich zum Stand vor den Kartenarbeiten vom 9.9.

# v0.7.79 — 2026-09-11

## Vier tote Treppen erwachen + Spinnen wieder aktiv

- **ROOM6030-Treppe** (Nutzer-Report "kann sie nicht runter laufen"):
  Der Raum nutzt einen Einzel-Treppen-Record mit dem byte-true
  Band-WILDCARD 0x80 (@0x80042cac-cd4: Bit gesetzt = Band-Vergleich wird
  uebersprungen, EIN Record bedient beide Enden). Der Port verglich das
  Byte roh — die Treppe war in beide Richtungen tot. Der RDT-Vollscan
  fand insgesamt VIER solcher Wildcard-Treppen (ROOM2030, 2080, 3010,
  6030) — alle funktionieren jetzt; normale Treppen unveraendert.
- **Spinnen im Bosskampf-Raum wieder aktiv** (der temporaere Freeze aus
  der Bosskampf-Abstimmung ist entfernt).

# v0.7.78 — 2026-09-11

## Bosskampf ROOM2090: Routenwahl nach Wegkosten

- **Kein Ost-Umweg mehr am kurzen Westweg vorbei** (Nutzer-Marke): Die
  fruehere Verkuerzung "Rampe immer, wenn verfuegbar" liess den Alligator
  von Leon WEG nach Osten drehen und den fast doppelt so langen Weg ueber
  den Rampen-Kletterpfad nehmen. Jetzt entscheiden die tatsaechlichen
  Wegkosten (West-Umlauf vs. Klettern); die Rampe behaelt eine kleine
  Praeferenz, damit er bei aehnlich langen Wegen weiterhin spektakulaer
  drueber klettert statt aussen herum zu schwimmen.

# v0.7.77 — 2026-09-11

## Bosskampf ROOM2090: Fress-Finisher synchron zum Maul

- Der Halbkoerper-Wurf startet jetzt AM MAUL (nicht mehr am Todesort) —
  der Koerper kommt sichtbar aus dem Maul hoch.
- Der Kopf schnellt beim Wurf nach oben und bleibt zum fliegenden Koerper
  gerichtet, bis der Rest im aufgerissenen Maul landet — vorher blieb der
  Kopf gesenkt, waehrend der Koerper losgeloest in der Luft wedelte.
- Die Bahn endet auf Maulhoehe im gehobenen Maul; nach dem Verschlingen
  senken sich Kopf und Kiefer gemeinsam.

# v0.7.76 — 2026-09-11

## Bosskampf ROOM2090: keine Luecke mehr auf gleicher Ebene

- **Neues Messwerkzeug**: Der Dauertest prueft jetzt echte TREFFER statt
  blosser Annaeherung — 80 Konstellationen (Leon statisch, fliehend mit
  Laufgeschwindigkeit, kreisend; unten und oben auf der Plattform).
  Ausgangslage: 4 Luecken, jetzt 0.
- **Rueckwaertsgang** (Nutzer-Freigabe): Klebt Leon seitlich/hinter dem
  Alligator, zieht er sich rueckwaerts raus, statt auf der Stelle zu
  drehen — auch im Biss selbst, wenn Leon zu nah am Koerper steht.
- **Not-Schnapp**: Auch mitten in der Wende beisst er zu, wenn Leon in
  Reichweite ist (vorher toter Winkel, in dem Leon dauerhaft kreisen
  konnte).
- **Koerpermitte zu**: Zwischen den Schiebe-Segmenten war ein Loch — Leon
  konnte direkt AUF dem Alligator stehen; Nahkontakt zaehlt jetzt als
  Treffer.
- **Plattform-Mitte erreichbar**: Er belagert jetzt Leons naechste Kante
  (auch die Westkante) statt stur Nord/Sued, und der Hochbiss reicht bis
  zur Plattform-Mitte.
- Fluessiger: breiteres Treffer-Fenster, kuerzere Fehlschnapp-Pause,
  schnelleres Nachsteuern im Biss, Wand-Pruefung ab dem Maul (Ecken-
  Streifung blockt keine legitimen Bisse mehr).

# v0.7.75 — 2026-09-11

## Bosskampf ROOM2090: Fress-Finisher

- **Der Biss landet jetzt AM Koerper**: Waehrend des Kopf-Senkens schiebt
  sich der Alligator heran, bis das Maul auf Leon liegt.
- **Halbkoerper-Wirbel**: Nach dem Schnapp ist Leons Oberkoerper im Maul —
  Huefte und Beine wirbeln kopfueber durch die Luft (Parabel zum Maul),
  dann reisst das Maul noch weiter auf (Kiefer-Bone), ein zweiter Schnapp,
  und der Rest wird verschlungen. Danach geht er einfach weg.
- **YOU DIED wartet**: Der Gameover-Fade haelt an, bis die Fress-Sequenz
  sichtbar zu Ende gespielt ist (nur im Bosskampf; der byte-true
  Todes-Ablauf sonst unveraendert).

- **Tödlicher Biss endet im Fressen** (Nutzer-Design): Statt Knockdown
  senkt der Alligator den Kopf zu Leon (60 Frames), beißt zu (Schnapp-Clip
  ab F75), Leon verschwindet erst NACH dem sichtbaren Schnapp (F95) —
  und ab F170 geht er einfach weg (der Sieger-Abzug aus v0.7.74
  übernimmt). Timing nach Nutzer-Feedback gestreckt: Sterbe-Moment und
  Todeskamera-Einschwenk kommen jetzt VOR dem Verschwinden.
- Leon wird beim Continue-Respawn wieder sichtbar (no_draw-Reset am Spawn,
  zusätzlich im Spieler-Sichtbarkeits-Gate verdrahtet).
- Klärung zum Report „weggelaufen nach einem Treffer“: Der Verlauf im
  befund.log zeigt hp=15 → −35 — der 50er-Biss (byte-true dmg_table[5]
  @DAT_8006f418) war ein regulärer Tod; der Alligator zog als Sieger ab.
  Der Fress-Finisher macht genau diesen Moment jetzt lesbar.
- Verifikation: gdigrab-Bildserie (Leon am Maul → im Folgebild weg,
  Raum-Leiche bleibt), 4× ph=8-Telemetrie, Konvergenz-Sweep 3342/0,
  Suite 283/283.

# RE1.5 Port — v0.7.74 (Early Preview)

**ROOM2090: würdevoller Sieger — und spürbare Treffer.**

---

Deine zwei Punkte:

1. **Nach deinem Tod** kreiselt er nicht mehr über der Leiche: Er lässt ab
   und gleitet ruhig zu seinem Lauerplatz zurück, während die Todes-Kamera
   läuft.
2. **Schadens-Animation**: Bisher zuckte er nur an 10-%-Schwellen — mit der
   Handfeuerwaffe hieß das rund zwanzig Treffer pro Reaktion. Jetzt flincht
   er zusätzlich nach jeweils sechs Treffern — dein Beschuss ist sichtbar
   wirksam, ohne ihn zum Dauerzucken zu bringen (große Schadenssprünge
   reagieren weiterhin sofort).

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.73 (Early Preview)

**ROOM2090: kompromisslose Jagd — und der Kletter-Treffer wirft dich sauber runter.**

---

Deine zwei Punkte:

1. **Kletter-Treffer**: Erwischte er dich mitten im Hochklettern, hingst du
   in der Luft. Jetzt bricht der Treffer das Klettern ab und du fällst auf
   die unterste Ebene — wie von dir vorgegeben.
2. **Kompromisslose Jagd**: Liegt der Block zwischen euch, klettert er ab
   sofort IMMER direkt drüber (der Außen-Umlauf existiert nur noch als
   Notweg direkt nach einer Querung). Das Zufalls-Wackeln der Nase beim
   Schwimmen ist raus, Wenden sind zackig statt träge, und in deiner Nähe
   hält er stur auf dich — keine leeren Drehungen mehr.

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.72 (Early Preview)

**ROOM2090: jetzt ist er gefährlich.**

---

Dein Urteil („relativ dumm und 0 gefährlich“) hatte drei Ursachen, alle
adressiert:

1. **Wasser-Burst**: Sieht er dich im Wasser vor sich (mittlere Distanz),
   explodiert er in einen 2,5-Sekunden-Sprint mit doppeltem Tempo — das
   Krokodil-Verhalten, das Weglaufen in gerader Linie tödlich macht.
   Danach braucht er eine Verschnaufpause — dein Fenster.
2. **Absprung-Strafe**: In dem Moment, wo du die Rampe verlässt, ist sein
   Sprint sofort bereit — der Abstieg kostet. Zusammen mit den Querungen
   oben (die inzwischen auf deine Spur zielen und mehrfach schnappen) gibt
   es kein bequemes Versteck mehr.
3. **Verfolgungstempo** moderat angehoben — er ist dir im Wasser jetzt
   grundsätzlich überlegen; Lauern und Belagerung bleiben ruhig lesbar.

Im Kontroll-Lauf stellte er einen fliehenden Leon per Sprint und setzte
durch. Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.71 (Early Preview)

**ROOM2090: die letzte Safe-Zone ist zu — und er hält vor.**

---

Dein Marker entlarvte eine unsichtbare Schutzzone: Im West-Drittel der
Rampe konnte dich keine Querung je erreichen, weil eine interne
Sicherheitsklemme die Bahn dort abschnitt. Jetzt darf die Kletterbahn
über der Rampe die volle Breite nutzen — die Klemme sichert nur noch den
Landepunkt im Wasser. Oben gibt es keinen sicheren Fleck mehr.

Dazu die erste „schlauer machen“-Stufe: Bei der Jagd im Wasser zielt er
jetzt mit Vorhalt auf deine vorhergesagte Position statt stur auf deinen
Rücken — wegrennen in gerader Linie ist spürbar riskanter. Wenn du danach
noch Lücken findest, ist die nächste Stufe vorbereitet (Lauern am
Rampen-Aufgang).

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.70 (Early Preview)

**ROOM2090: die Kletterphase hat jetzt Zähne.**

---

Dein Befund war präzise: Während der Querung war er ein hilfloses
Fahrgeschäft. Jetzt gilt oben auf der Rampe: Er schnappt MEHRFACH zu,
sobald du in Maulnähe bist (nicht mehr einmal pro Querung), das Ende
seiner Kletterbahn verfolgt live deine Position (seitliches Wegtippeln
führt nicht mehr ins Leere), und die Querung ist ein Viertel schneller.
Springst du während seiner Belagerungs-Querung herunter, bricht er die
Bahn ab, sobald er überm Wasser hängt, und jagt dich sofort normal —
das Hoch-Runter-Spiel füttert ihn jetzt, statt ihn vorzuführen.

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.69 (Early Preview)

**ROOM2090: die Querung zielt jetzt auf DICH.**

---

Dein Marker zeigte es exakt: Alle Querungen liefen auf einer festen
Ost-Spur — du standest im West-Drittel der Rampe, gut zwei Meter neben
der Bahn, und der Vorbeigeh-Schnapp konnte dich nie erreichen. Darum
wirkte das Hin-und-Her wie Clownerei. Jetzt klettert er DIAGONAL: Der
Einstieg bleibt, wo er physisch hinkommt, aber die Bahn zieht über die
Rampe direkt auf deine Position — wer oben stehen bleibt, bekommt den
Schnapp. Und nach einer Querung bleibt er zehn Sekunden standhaft auf
deiner Seite, statt sich sofort wieder zurückschicken zu lassen.

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.68 (Early Preview)

**ROOM2090: bleib besser nicht auf der Rampe stehen.**

---

Deine Freigabe ist drin: Stehst du oben auf der Rampe und er muss auf
deine Seite, klettert er jetzt QUER über die Rampe — direkt unter dir
durch, mit Schnapp im Vorbeigehen — statt den langen Umlauf zu schwimmen,
den du bisher mit einem Schritt zurück entwerten konntest.

Zu deiner Auflage („ohne dass die Spielfigur hängen bleibt“): Leon wird
beim Drüberklettern ausschließlich von der Körper-Schub-Mechanik mit
Wand-Klemme bewegt — dieselbe, die auch deine eigene Bewegung begrenzt.
Er kann also weggeschoben werden oder die offene Stufe hinabrutschen,
aber nicht durch Wände oder ins Nirgendwo. Falls sich beim Testen doch
irgendwo ein Haken zeigt: F9-Marke reicht, die Telemetrie läuft mit.

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.67 (Early Preview)

**ROOM2090: der Alligator ist jetzt stur — fernsteuern klappt nicht mehr.**

---

Dein Nachtest hat gezeigt: Du hast nicht getänzelt, du bist wirklich die
Seite gewechselt — und er hat jedes Mal brav den Riesen-Umlauf gestartet,
den du mit einem Schritt zurück entwertet hast. Jetzt ist er stur: Die
Belagerungsseite wechselt erst, wenn du rund zehn Sekunden stabil auf der
Gegenseite stehst — vorher bleibt er lauernd an seiner Kante. Und wenn er
sich einmal für den Umlauf entschieden hat, schwimmt er ihn zu Ende, statt
auf halber Strecke umzudrehen.

Anmerkung: Die aggressivste Lösung wäre, dass er bei Seitenwechsel-Bedarf
quer über die Rampe klettert — direkt unter dir durch, mit Schnapp. Das
würde aber deine Regel „klettern erst, wenn er heruntergesprungen ist“
aufweichen — sag Bescheid, wenn du das möchtest.

Sweep 3342/3342 konvergent, Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.66 (Early Preview)

**ROOM2090: der Alligator lässt sich nicht mehr austricksen.**

---

Dein Befund („gehe ich vor, kommt er an; gehe ich zurück, geht er weg“):
Die Belagerungskante folgte jedem deiner Schritte sofort — ein Tänzeln um
die Flächen-Mitte ließ ihn im Sekundentakt die Seite wechseln, inklusive
des langen Wegs außen herum. Jetzt hat seine Seitenwahl Beharrlichkeit:
Ein Totband um die Mitte ignoriert das Tänzeln, und erst wenn du rund
drei Sekunden stabil auf der anderen Hälfte stehst, verlegt er die
Belagerung — dann aber entschlossen.

Der Vorausschau-Test hat dafür Tänzel-Szenarien bekommen (Leon springt
im Test permanent über die Mitte): alle 3342 Konstellationen konvergieren.

Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.65 (Early Preview)

**ROOM2090: die Vorausschau — 3286 Konstellationen durchgerechnet, alle Blocker raus.**

---

Deine Ansage („schau voraus, was ihn noch alles blocken wird“) ist jetzt
Programm: Ein neuer Dauertest lässt den echten Alligator über ein Raster
ALLER Startpositionen gegen 31 Leon-Positionen laufen (Ufer, Ecken,
Nischen, Wasser, oben auf Plattform und Rampe) und verlangt für jede der
3286 Konstellationen, dass er binnen 50 Sekunden in Kontakt- bzw.
Belagerungsnähe kommt.

Der Erstbefund waren 530 Blocker-Konstellationen — sieben Fehlerklassen,
alle in dieser Version behoben, darunter: echte Kürzeste-Wege-Suche statt
kurzsichtiger Eckenwahl (die Rampe macht den Eckenring zur Kette),
Selbst-Befreiung aus Klemm-Fällen an Wänden und Block, ein Etappen-Latch
gegen jedes Ziel-Flattern, und die Belagerung wählt ihre Kante jetzt
überall nach DEINER Position. Endstand: 0 von 3286 blockiert — und der
Test läuft ab jetzt bei jedem Build mit, damit es so bleibt.

Suite 283/283 (lokal + Docker).

---

# RE1.5 Port — v0.7.64 (Early Preview)

**ROOM2090: eine einzige Wegfindung — das Schwanken ist an der Wurzel raus.**

---

Dein Marker („schwankt hin und her“): Er schwamm exakt auf einer internen
Zonengrenze, und zwei konkurrierende Wegfindungs-Systeme der Belagerung
zogen ihn im 30-Frame-Takt in entgegengesetzte Richtungen. Das war der
vierte Widerspruch dieser Art — deshalb jetzt der strukturelle Schnitt
statt eines fünften Flickens: Verfolgung UND Belagerung benutzen ab
sofort dieselbe einzige Weg-Maschine (Ziel → Kanten-Umweg über die freien
Ecken), die konkurrierenden Alt-Systeme sind ersatzlos entfernt, und die
Zonengrenzen haben eine Hysterese, damit ein Schwimmen AUF der Linie
nichts mehr umwirft.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.63 (Early Preview)

**ROOM2090: der Seitenwechsel-Wegpunkt bleibt jetzt unverbogen.**

---

Dein Marker: Der v0.7.62-Seitenwechsel wählte zwar korrekt die West-Ecke
als Zwischenziel — aber das nachgeschaltete Patrouillen-Pendel „korrigierte“
den Wegpunkt mit seiner Rampen-Klemme in die Südost-Sackgasse, und dort
stand er knapp vor dem falschen Ziel still. Wegpunkte sind jetzt vom
Pendel ausgenommen (nur echte Kanten-Ziele pendeln) — die West-Route
Süd → Westecke → Kanal → Nordkante läuft unverbogen.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.62 (Early Preview)

**ROOM2090: die Belagerung kommt auf deine Seite der Rampe.**

---

Deine drei Marker: Du standest oben nahe der NORDkante der Rampe, er
belagerte stur die SÜDkante — dauerhaft außer Reichweite, und einen Weg
auf die richtige Seite kannte die Belagerung nicht. Jetzt richtet sich
die Belagerungsseite nach DEINER näheren Kante: Steht er im falschen
Becken, schwimmt er westlich um die Plattform herum und taucht an der
Kante unter dir auf. Klettern während du oben stehst bleibt wie von dir
festgelegt tabu — er nimmt den Wasserweg.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.61 (Early Preview)

**ROOM2090: er liegt beim Klettern AUF der Rampe — und die Belagerung friert nicht mehr ein.**

---

Dein zweiter Marker (Alligator stand bewegungslos im Westkanal, während du
oben auf der Rampe standest) deckte drei Belagerungs-Fehler auf: Er
lauerte an der falschen Kante (Plattform-West statt Rampen-Seite — jetzt
wechselt er in das nähere Becken und belagert die Rampen-Kante unter
dir), sein Kanten-Pendel war in einem Zahlen-Deadlock eingefroren (jetzt
patrouilliert er sichtbar), und der Anti-Hänger-Wächter galt bisher nur
für die Verfolgung, nicht für die Belagerung (jetzt für beide). Dein
dritter Marker („dreht planlos Runden“) war ein Ziel-Flattern zwischen
Belagerungskante und Umlauf-Ecke — die Belagerung hält ihr Kantenziel
jetzt stabil und weicht nur noch aus, wenn wirklich ein Block im Weg ist.

---

Dein Bild mit dem Lineal war der Schlüssel: Der Körper schwebte am
Scheitel exakt 1392 Einheiten über dem Deck. Die Messung am lebenden
Modell zeigte den Grund — der Ursprung des Alligator-Rigs sitzt an der
Körper-UNTERSEITE, nicht in der Mitte; meine Höhenrechnung aus den
Modelldaten hatte die Richtung invertiert und hob ihn um genau eine
Körperhöhe zu hoch. Der Scheitel liegt jetzt so, dass die Unterseite mit
~100 Einheiten Toleranz auf dem Deck aufliegt — nachgemessen am selben
Mess-Hook, und im Sichtlauf liegt er flach auf der Fläche.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.60 (Early Preview)

**ROOM2090: das Klettern ist jetzt flach.**

---

Dein Feinschliff-Wunsch: Die Kletteranimation war zu umschweifend. Die Höhe
selbst war schon minimal (sein Bauch streift am Scheitel exakt über das
Rampendeck) — das Umschweifende kam aus der starken Körperkrümmung, der
Nase-hoch-Neigung und dem steilen An-/Abstieg. Alle drei sind deutlich
zurückgenommen: Die Wirbelsäule krümmt sich nur noch leicht (~9° statt
23° je Gelenk), die Nase hebt sich dezent (~12° statt 26°), und er steigt
weicher an und ab. Im Sichtlauf gleitet er jetzt flach über die Kante —
Silhouette fast waagerecht, kein Bogen mehr.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.59 (Early Preview)

**ROOM2090: die Rampe zählt jetzt überall als Hindernis — und ein Wächter heilt jeden Rest-Hänger selbst.**

---

Dein Hänger: Er drückte an der Rampen-NORDKANTE südwärts gegen den Block
(du standest dahinter), weil zwei Wegprüfungen nur die Plattform als
Hindernis kannten — die Rampe blockt ihn aber genauso. Beide prüfen jetzt
beide Blöcke, und die Kletter-Route wird nach ihrer Cooldown-Sperre
zuverlässig neu bewertet (die alte Wieder-Freigabe konnte verpuffen).

Dazu die Versicherung gegen die ganze Fehlerklasse: Ein
**Fortschritts-Wächter** erkennt zur Laufzeit, wenn er trotz fernem Ziel
90 Frames lang kaum Strecke macht, und zwingt ihn dann für zwei Sekunden
auf die nächste freie Umlauf-Ecke — jeder künftige Geometrie-Sonderfall
löst sich damit sichtbar von selbst, statt zum Dauerstand zu werden (und
meldet sich im Log, damit ich die Wurzel trotzdem fixen kann).

Kontroll-Lauf im Marker-Szenario: null Stillstände, Bissnähe 26, der
Wächter musste nie eingreifen. Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.58 (Early Preview)

**ROOM2090: ein zentraler Kanten-Umweg für alle Wege.**

---

Dein Hänger (Marke F2063): Der Kletter-Anlauf zur Rampe steuerte sein Ziel
blind an, während der Alligator westlich der Plattform im Körper-Schatten
der Kante stand — die Wandklemme fraß die Bewegung und er kroch auf der
Klemmlinie. Das war der dritte Auftritt derselben Fehlerklasse in
verschiedenen Zweigen der Wegwahl. Statt eines weiteren Einzel-Flicks
prüft jetzt EIN zentraler Schritt jedes Steuerziel, egal aus welchem
Zweig es kommt: Schert die Luftlinie im Kanten-Schatten, läuft er erst
über die günstigste freie Umlauf-Ecke. Die Marker-Situation ist
handgerechnet abgedeckt (SW-Ecke → SO-Ecke → Anlauf → Klettern), der
Kontroll-Lauf an deiner Marker-Position lief ohne einen einzigen
Stillstand durch, und die Telemetrie zeigt künftig jede Umweg-Nutzung.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.57 (Early Preview)

**ROOM2090: wen die Schnauze schiebt, den beißt sie auch.**

---

Deine zwei Marken:

1. **„Schiebt mich immer noch weg“**: Messbar — du klebtest exakt am
   vordersten Punkt seines Körper-Schubs und wurdest mit vollem Schwimmtempo
   vor der Schnauze hergeschoben, während die Bissfreigabe ein Stück WEITER
   INNEN lag: frontal konnte er dich physisch schieben, aber nie beißen.
   Jetzt meldet der Schub den Schnauzen-Kontakt direkt an den Biss — sobald
   er dich vor sich herdrückt, kommt der Schnapp samt Schaden. Im
   Kontroll-Lauf an der Nordwand: reguläre Bisse statt Dauerschieben.
2. **„Bleibt teilweise hängen“**: Standest du an der Ostwand und er westlich
   der Plattform, hielt die Wegwahl die Route für „frei“, obwohl sein
   Körper an der Plattformkante entlangschliff — er kroch mit 2/Frame an
   der Klemmlinie. Der Kanten-Schatten wird jetzt auch auf gleicher
   Beckenseite erkannt und er nimmt die Ecke. Kontroll-Lauf: null
   Stillstände, er stellt dich regulär.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.56 (Early Preview)

**ROOM2090: kein Rauswurf durch die Wand mehr — und er wendet, statt Bögen zu ziehen.**

---

Deine zwei Befunde:

1. **„Ich wurde außerhalb der Wand geschoben“**: Der Körper-Schub des
   Alligators war der einzige Mechanismus, der Leon ohne Wandprüfung
   verschiebt — stand Leon zwischen Maul und Wand, schob der Biss ihn durch
   die Zelle nach draußen (deine Marker: bis 2.400 Einheiten hinter der
   Südwand). Der Schub läuft jetzt durch dieselbe Wand-Klemme wie deine
   eigene Bewegung. Kontroll-Lauf mit Bissen direkt an der Südwand inklusive
   Tod: null Ausbrüche — und ein fest eingebauter Wächter meldet jeden
   künftigen Fall sofort im Log.
2. **„Dreht erst in die entgegengesetzte Richtung“**: Sein Schwimm-Drehwert
   ist träge (bis zu mehrere Sekunden für 180°), und er schwamm während des
   Drehens stur vorwärts — nach jedem harten Zielwechsel Richtung Plattform
   zog er deshalb erst einen Bogen in die alte Richtung. Ab ~67°
   Kurskorrektur wendet er jetzt auf der Stelle und schwimmt erst wieder an,
   wenn die Nase zum Ziel zeigt.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.55 (Early Preview)

**ROOM2090: der Biss trifft jetzt am Maul — und nie mehr durch die Wand.**

---

Deine zwei Punkte, beide per Biss-Fenster-Telemetrie vermessen:

1. **„Trifft mich quasi so gut wie nie“**: Der Treffer-Test maß die Distanz
   zu seinem KÖRPERZENTRUM — aber sein eigener Körper hält dich am KOPF auf
   Abstand. Die Messung zeigte dich wörtlich direkt vorm Maul (Abstand ~900),
   während das Zentrum-Gate meilenweit verfehlte. Jetzt zählt die Distanz
   zum MAUL: Steht Leon vor der Schnauze, sitzt der Biss. Im Kontroll-Lauf
   traf der allererste Schnapp (vorher ein Glückstreffer pro Minute).
2. **Dauerschnappen an der Plattform vorbei**: Zwei Ursachen — das Angriffs-
   Kommit feuerte schon auf 6000 Sichtweite (das Maul kam im 45-Frame-Clip
   nie an: reine Leer-Animation), und die Distanz wurde notfalls DURCH die
   Plattform gemessen. Jetzt startet der Schnapp erst, wenn das Maul es im
   Clip wirklich erreichen kann, und nie, wenn die Wand dazwischen ist —
   dann schwimmt er stattdessen herum. Kontroll-Lauf an der Plattform-Wand:
   null Wand-Schnapps, Leon wurde regulär um die Ecke herum gestellt.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.54 (Early Preview)

**ROOM2090: die Kollision ist jetzt so gross wie der Alligator aussieht.**

---

Du hast den Kern getroffen: Es war die Größe — aber nicht die sichtbare.
Optisch schwimmt er längst im 2/3-Maßstab, seine KOLLISIONSBOX hatte aber
noch die Originalgröße des Full-Size-Alligators. Er stieß deshalb an Kanten
an, an denen er optisch locker vorbeipasst — das war die gemeinsame Wurzel
der Hänger. Jetzt ist die Kollision mit demselben Faktor geschrumpft wie
das Modell, und alle Wegmaße rechnen mit dem echten Körper.

Dazu aus deinen drei Kanten-Marken:

1. **Belagerung an der richtigen Kante**: Standest du oben auf der Rampe,
   kreiste er dauerhaft im Nordbecken an der Plattform-Ecke — seine
   Patrouille wollte um die Rampe herum, um die es keinen Ostumlauf gibt.
   Jetzt schwimmt er an die Kante seiner Seite direkt unter dich, pendelt
   dort sichtbar und schnappt nach oben, sobald du nah an der Kante bist.
2. **Kein Wandschleifen mehr**: Stand Leon für ihn unerreichbar (z. B. in
   der Süd-Türnische), schliff er dauerhaft an der Wand — sein Steuerziel
   wird jetzt auf die erreichbare Fläche geklemmt, er lauert ruhig am
   nächstmöglichen Punkt.

Kontroll-Läufe: Südwand-Szenario null Stillstände (vorher zehn), volle
Jagd-Runde durchs Becken null Stillstände, Bissnähe 261. Suite 282/282.

---

# RE1.5 Port — v0.7.53 (Early Preview)

**ROOM2090: geklettert wird nur noch am kleinen Vorsprung — und die Spinnen sind eingefroren.**

---

Deine vier Marken (F1243–F3088) waren per Telemetrie eindeutig:

1. **„Bleibt in der Welt hängen“**: Zwei alte Wegwahl-Systeme (Randpunkt-Anlauf
   und Ecken-Ring) bewerteten jeden Frame neu und flatterten gegeneinander —
   im Nordbecken stand er 200 Frames quasi still (Marken 1+2), im Westkanal
   pendelte er vor und zurück (Marke 3). Jetzt gibt es ein Zonen-Modell aus
   den drei Wasserzonen des Raums: Süd↔Nord kennt genau zwei Wege — westlich
   um die Plattform oder kletternd über den Vorsprung — und die gewählte
   Route wird gehalten statt je Frame neu gewürfelt. Vier Kontroll-Läufe:
   null Stillstände, er kommt in jedem Szenario bis auf Bissweite heran.
2. **„Klettert irgendwo anders rum als auf den kleinen Vorsprung“**: Die
   freien Kletterbahnen (Marke 4: er hing mitten über der Plattform in der
   Luft) sind raus. Geklettert wird ausschließlich über die flache Ost-Rampe
   — dein „nur HIER“-Punkt — als gerade Nord-Süd-Bahn, und der Bogen trägt
   nur über der Rampe Höhe.
3. **Belagerung nur, wenn du wirklich OBEN stehst**: Bisher reichte die
   Grundriss-Position — standest du im Wasser NEBEN der Rampe, belagerte er
   schon die Kante. Jetzt zählt auch die Höhe.
4. **Spinnen eingefroren** (dein Auftrag, temporär): beide stehen regungslos
   auf ihren Wasserpositionen, bis du sie wieder freigibst. Sag einfach
   Bescheid, dann löse ich den Freeze wieder heraus.

5. **Nachschlag aus deinen drei Abend-Marken (F424/F552/F940)**: Zwei
   Rest-Hänger kamen vom KÖRPERRADIUS — der Alligator ist 2200 Einheiten
   dick, die Wegprüfungen rechneten aber nur mit 1300: „Weg frei“, wo sein
   Rumpf längst an der Plattformkante schliff (deine Marken 1+2: er klemmte
   exakt auf Kante-minus-Radius). Und der Kletter-Anlaufpunkt der Rampe lag
   im Radius-Schatten der Plattform-Südostecke — mit seinem Körper physisch
   unerreichbar, er stand ewig 1470 vor dem Ziel (Marke 3). Alle Wegprüfungen
   rechnen jetzt mit Körpermaß, und der Kletteranlauf liegt außerhalb des
   Schattens. Drei Kontroll-Läufe an deinen Marker-Positionen: null
   Stillstände, Bissnähe in allen Szenarien.

Telemetrie erweitert: jede TICK-Zeile in gator_boss.log trägt jetzt Route,
Zweig und Steuerziel — der nächste Marker ist damit noch schneller zuordenbar.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.52 (Early Preview)

**ROOM2090: nur Wände blocken den Alligator — und die Spinnen sind zurück im Wasser.**

---

Deine zwei Marker („zeitweise geblockt im Pool“ + „kletterte im Nirgendwo
kurz hoch“) führten zu vier Änderungen:

1. **Gegner blocken den Boss nicht mehr**: Der Alligator überspringt jetzt
   den Gegner-Körper-Push — schwimmende Körper, Spinnen usw. sind für ihn
   durchlässig, nur die Wände (SCA) halten ihn. Genau wie von dir
   beauftragt: „Die Körper die im Wasser schwimmen, die spinnen usw. die
   sollten für ihn durchlässig sein.“
2. **Spinnen zurück ins Wasser**: Da sie ihn nicht mehr blocken, sitzen die
   zwei RDT-Spinnen wieder auf ihren Original-Wasserpositionen
   (@0x0AB0/@0x0AC4) — Plattform-Sitz und Wandflucht sind wieder raus.
3. **Nahstand-Schnapp**: Direkt vor Leon setzte die Verfolgung manchmal aus,
   weil der Winkel-Arc knapp verfehlte — unter 2600 Einheiten schnappt er
   jetzt immer.
4. **Sackgassen-Pendel**: Erreicht die Belagerungs-Patrouille ihr Ziel, ohne
   dass du dich bewegst, pendelt er dort statt in der Ecke zu erstarren.

Verifiziert: Gator spawnt und tickt (Telemetrie), Spinnen wandern im Wasser,
Suite 282/282.

---

# RE1.5 Port — v0.7.51 (Early Preview)

**ROOM2090: das Sinken beginnt vor der Kante, und die Belagerung läuft um die Insel statt dagegen.**

---

Deine zwei Marker, beide per Telemetrie aufgeklärt:

1. **„Kletterte im Nirgendwo kurzzeitig nach oben"**: Das war der Ausklang des
   Bogens — die Höhe baute sich erst NACH der Inselkante ab und trug ihn
   600–1200 Einheiten sichtbar hinaus. Jetzt schaut die Bahn 13 Frames
   voraus: Das Sinken beginnt vor der Kante und endet exakt an ihr. Der
   Schwebe-Detektor findet im Regressionslauf null Treffer (vorher zwei).
2. **„Teilweise geblockt"**: Die Belagerungs-Patrouille steuerte stur
   geradeaus auf einen Randpunkt hinter der Rampe (du oben auf der anderen
   Seite) und stand 3 Sekunden gegen die Kollision. Liegt die Insel zwischen
   ihm und dem Patrouillenziel, schwimmt er jetzt per Wand-Following herum.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.50 (Early Preview)

**ROOM2090: er kommt jetzt hinterher — und klettert nur noch bei echter Querung.**

---

Beide Marker waren dank der mitlaufenden Telemetrie in Minuten aufgeklärt:

1. **„Kam nicht hinterher über die Plattform"**: Die Positions-Spur zeigte den
   Kletter-Cooldown (10 s) als Bremse — nach einer Querung musste er den
   langen Ring schwimmen, während du längst zurückgewechselt warst. Der
   Cooldown beträgt jetzt 3 Sekunden.
2. **„Weit weg davon hochgeklettert"**: Seine Bahn streifte nur die
   Nordwest-Ecke des Stegs (~1000 Einheiten Schnitt) und galt trotzdem als
   Querung; zusätzlich klang die Bogenhöhe zu träge aus (~1500 Einheiten
   sichtbares Nachschweben). Die Kletter-Probe verlangt jetzt mindestens
   2000 Einheiten echten Insel-Schnitt, und die Höhe baut sich 2,5× schneller
   auf und ab.

Regression: die legitime Rampen-Querung läuft unverändert, danach exakt
Wasserlinie und Schnapp-Verfolgung.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.49 (Early Preview)

**ROOM2090: der Ecken-Hänger ist gefangen und die ganze Fehlerklasse geschlossen.**

---

Diesmal habe ich den Hänger selbst gejagt: Vollzeit-Telemetrie plus
Stillstands-Detektor haben ihn eingefangen — das Patrouillen-Ziel der
Belagerung saß in der Nordwest-**Poolecke**, jenseits beider Wand-Kollisions-
grenzen: Er schob dort ewig gegen ein Ziel, das sein 2200er-Körperradius nie
erreichen kann. Dieselbe Fehlerklasse wie die früheren Ring-Ecken.

Der Klassen-Fix sitzt jetzt an der zentralen Stelle: **Jedes** Ziel, das die
Boss-Logik erzeugt (Patrouille, Kletter-Bahnenden, Anlauf), wird auf die für
ihn tatsächlich erreichbare Poolfläche geklemmt (Wandabstand 2500) —
unerreichbare Ziele sind konstruktiv ausgeschlossen. Im Nachtest desselben
Szenarios erreicht er sein Ziel und belagert dort mit Schnapp-Zyklen statt in
der Ecke festzuhängen. Außerdem patrouilliert die Belagerung jetzt sichtbar an
der Insel-Kante unter dir her, statt regungslos an der Wand zu stehen, wenn du
außer Reichweite bist.

Die Telemetrie (`gator_boss.log`) schreibt weiter mit — falls doch noch etwas
hängt: F9 drücken und mir die Datei mitgeben, dann sehe ich Phase, Position
und Ziel des Moments.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.48 (Early Preview)

**ROOM2090: Bogen jetzt aus der Positions-Wahrheit — und die Hänger-Ecken sind entschärft.**

---

Deine Session-Telemetrie hat beide Restfälle exakt belegt:

1. **„Bogen obwohl nicht bei der Insel"**: Mehrere deiner Querungs-Bahnen
   führten durch die Wassertasche zwischen Steg und Rampe — das bisherige
   Zeitfenster überspannte sie, er schwebte dort. Der Bogen hängt jetzt nicht
   mehr an der Zeit, sondern an der **Position**: Er steigt nur, solange er
   sich wirklich über der Insel befindet; Höhe fährt mit begrenzter Rate,
   Wirbelbogen folgt der Höhe, Neigung der Steig-/Sinkrate. Schweben ist
   damit konstruktiv unmöglich.
2. **„Bleibt hängen"**: Die Ausweich-Ecken des Wand-Followings lagen mit
   Abstand 1200 innerhalb seiner 2200er-Kollisionszone — unerreichbare Ziele,
   an denen er festlief. Sie liegen jetzt außerhalb (2600). Zusätzlich wird
   das Anlaufziel eingefroren statt jede Frame neu berechnet (kein Zickzack).

Kontrolle: Nach einer Test-Querung steht er exakt auf der Wasserlinie
(y=−1200; vorher hing er bei −2986 in der Luft) und greift direkt an.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.47 (Early Preview)

**ROOM2090: der Bogen läuft nur noch über der Insel — kein Schweben überm Wasser.**

---

Dein Marker („kann völlig frei unten überqueren") zeigte ihn im Kletterbogen
über dem freien Wasser hängend. Ursache: Die Überquerungs-Bahn beginnt und
endet konstruktionsbedingt ein Stück vor der Insel (außerhalb seiner
Kollisionszone), aber der Steig-Bogen lief über die gesamte Bahn — er hob
also schon im Wasser ab und schwebte die Endstücke entlang.

Jetzt berechnet die Bahn beim Start, auf welchem Abschnitt sie wirklich über
der Insel liegt: Nur dort wirken Hub, Wirbelbogen und Körperneigung — die
Endstücke schwimmt er flach im Wasser. Messbeleg aus der Telemetrie: eine
240-Frame-Querung trägt den Bogen exakt im Fenster 57–140, davor und danach
flaches Schwimmen.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.46 (Early Preview)

**ROOM2090: kein Eck-Klettern mehr — geklettert wird nur, wenn die Bahn wirklich hinüberführt.**

---

Dein Marker zeigte ihn im Kletterbogen an der Nordwest-Ecke, während du im
Nordosten standst — ihr wart auf derselben Seite, da gab es nichts zu queren.
Ursache: Streift seine Sichtlinie die Insel nur an einer Ecke, lief die
Kletter-Bahn an der Kante entlang statt darüber. Jetzt prüft er vor dem Start,
ob die Bahn die Insel wirklich substanziell quert — wenn nicht, schwimmt er
außen um die Ecke (Wand-Following) statt zu klettern. Mit exakt deinen
Marker-Koordinaten durchgerechnet: dieser Fall wird jetzt abgefangen; die
legitime Überquerung (du auf der Gegenseite) läuft unverändert.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.45 (Early Preview)

**ROOM2090: Überquerungssystem neu gebaut — kein Stecken, kein Phantom-Klettern.**

---

Dein Marker hat die Systemschwäche endgültig offengelegt: Du standst oben auf
der Rampe, aber die Belagerung lief nie an — der Alligator steckte bewegungslos
im Kletter-**Anlauf** fest, weil sein Anlaufziel mitten in der Kollisionszone
der Rampe lag (die festen „Kletterkanten" vertrugen sich nicht mit der
T-Form aus Steg + Rampe). Daraus folgten alle drei Symptome: Stecken,
Kletterstarts aus dem Nirgendwo, „komisch stehenbleiben".

Das Kanten-System ist ersetzt: Der Alligator berechnet jetzt den **nächsten
Punkt am Rand der gesamten Insel** (Steg + Rampe als Einheit, immer außerhalb
aller Kollisionszonen), schwimmt sichtbar mit aktiver Wand-Kollision dorthin
und startet erst von dort den Bogen — geradewegs hinüber zu dem Randpunkt auf
deiner Seite, Dauer nach Bahnlänge. Steht du oben, hat die Belagerung mit dem
Hochbiss weiter Vorrang.

Nachgestellt mit deiner Marker-Lage: Überquerung läuft sauber durch, kein
Stecken; Belagerungs-Telemetrie unverändert korrekt.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.44 (Early Preview)

**ROOM2090: der Durch-die-Insel-Tunnel ist zu — klettern nur noch von der Kante aus.**

---

Dein Nachtest hatte recht — zwei Löcher waren noch offen:

1. **Der eigentliche Übeltäter**: Der Anlauf zur Überquerung lief seit v0.7.39
   mit abgeschalteter Wand-Kollision (damals ein Patt-Fix). Kreuzte seine
   Anlauf-Route die Insel, glitt er flach QUER DURCH sie hindurch — genau dein
   „er klettert drüber, obwohl er gar nicht dort ist". Strukturell behoben: Die
   Kletter-Startkanten liegen jetzt außerhalb des Kollisionsradius, der Anlauf
   schwimmt wieder MIT Wand-Kollision (kein Patt und kein Tunnel mehr möglich);
   nur der Bogen selbst darf über den Block.
2. Der **Rampen-Kletterstart** feuerte weiter aus beliebiger Ferne (v0.7.43
   hatte nur den Block gedeckelt): jetzt nur noch nahe der Rampen-Kante, von
   weiter weg schwimmt er erst flach heran.

Gegengeprüft mit deinem Marker-Szenario: die legitime Überquerung (Nord→Süd
über die Rampe) läuft weiterhin sauber durch.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.43 (Early Preview)

**ROOM2090: kein Phantom-Klettern mehr — Aufbäumen und Übersteigen nur an der Insel.**

---

Dein Befund („kann über die Insel klettern, obwohl er überhaupt nicht da ist"):
Zwei Quellen derselben Klasse gefunden und geschlossen. Erstens lief die
Aufbäum-Pose der Plattform-Belagerung ab dem Moment, in dem du oben standest —
also auch, während er noch quer durchs Becken anschwamm: die „Kletterpose" im
freien Wasser. Sie baut sich jetzt erst in Kanten-Nähe auf (und sauber wieder
ab). Zweitens durfte die Block-Überquerung aus beliebiger Ferne starten; jetzt
nur noch nahe der Startkante — von weiter weg bringt ihn erst das
Wand-Following heran.

Beleg per Telemetrie (liegt als `gator_boss.log` neben der exe): auf dem ganzen
Anschwimmweg (Distanz 15000→5000) bleibt die Pose flach, erst bei ~3400 baut
sich das Aufbäumen auf, nach deinem Wegschleudern wieder ab.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.42 (Early Preview)

**ROOM2090: Belagerung mit Hochbiss, Geist-Hänger behoben, Wand-Following.**

---

Deine Punkte:

1. **Der „gewaltige Hänger"**: Dein Marker zeigte die wahre Ursache — ein
   Geist: Du hattest den Boss getötet und den Raum neu betreten; der frische
   Alligator übernahm dabei den „tot"-Zustand des Vorgängers und stand nur
   herum (hp=0). Der Neustart wird jetzt sauber erkannt (der Boss kommt bei
   jedem Betreten frisch). Auch eine getötete Spinne hing im Sitz-Anker fest —
   der Anker greift nur noch bei lebenden Spinnen.
2. **Dein Wand-Vorschlag ist umgesetzt**: Statt der „günstigsten" Ecke (die in
   Wandnähe diagonal in die Klemme steuerte) läuft er jetzt die Wand entlang
   bis zur nächsten Ecke in der kürzeren Umlaufrichtung — und dreht erst dort.
3. **Plattform-Belagerung (dein neues Design)**: Solange du oben stehst (Steg
   ODER Rampe), klettert er nicht mehr — er schwimmt an die Kante unter dich,
   richtet den Vorderkörper steil auf und **schnappt nach oben**, mit Schaden
   und Niederschlag, wenn du nah an der Kante stehst (die Plattform-Mitte ist
   sicher). Erst wenn du herunterspringst, kommt die Kletter-Überquerung zur
   Verfolgung. Sichtbilder: der Vorderkörper ragt aus dem Wasser, das Maul
   arbeitet im Schnapp-Takt.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.41 (Early Preview)

**ROOM2090: Lauern ganz hinten, solide Körperlänge, Knockdown-Treffer, sichtbarer Schnapp.**

---

Deine vier Punkte:

1. **Startposition**: Er lauert jetzt ganz hinten — im diagonal fernen Ende des
   Beckens (Süd-West), Blick über das ganze Becken zur Leiter. An der Tür bist
   du sicher; der Kampf beginnt erst, wenn du dich ihm näherst (oder schießt).
2. **Durchclippen**: Die Kollision deckte nur den Rumpf — Kopf und Schweif des
   12-m-Tiers waren durchlässig. Vier zusätzliche Kollisionskreise entlang der
   Körperachse machen ihn jetzt auf ganzer Länge solide.
3. **Schadens-Animation**: Gemessen spielte Leon nur ein 22-Frame-Zucken. Der
   Biss löst jetzt den byte-true **Niederschlag** aus (dieselbe Knockdown-Klasse,
   die der Original-Alligator-Biss anstößt): Leon geht zu Boden und rappelt sich
   auf — im Test dreimal sauber durchlaufen.
4. **Beiß-Animation**: Die Kieferkurven beider Kandidaten-Clips wurden Frame für
   Frame vermessen: Der bisherige Clip öffnet das Maul erst ab Frame 54 — mein
   Abbruch bei 40 zeigte also nur den Ansatz. Der Angriff nutzt jetzt den
   Schnapp-Clip (Maul reißt ab Frame 4 auf, Peak bei 12) mit dem Schadensfenster
   genau auf dem offenen Maul. Sichtbild: er kommt mit aufgerissenem Maul.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.40 (Early Preview)

**ROOM2090: kein Durchclippen mehr — und er steigt jetzt auch über die Rampe.**

---

Deine zwei Punkte:

1. **Durchclippen**: Gemessen an deinen Markern lag es am byte-true Höhenband
   der Körper-Kollision — der Alligator schwimmt bei −1200, Leon watet bei 0,
   und die Engine hielt die beiden deshalb für „übereinander gestapelt" und
   ließ den Schubs aus. Sein Kollisionszentrum sitzt jetzt auf Bodenniveau.
   Praxis-Check: Leon versucht 20 Sekunden per Autopilot durch den Körper zu
   laufen und prallt jedes Mal ab.
2. **Übersteigen an deiner Marker-Stelle**: Dein Marker zeigte ihn exakt an
   der Rampen-Kante festhängend (deine Position jenseits davon). Die Ost-Rampe
   war im Barriere-Modell nicht enthalten — und östlich von ihr steht die
   Außenwand, es gibt dort keinen Umweg. Sie ist jetzt zweite Übersteig-Zone:
   Liegt sie zwischen ihm und dir, steigt er sofort im Bogen darüber (Nord↔Süd,
   auf seiner Höhe). Nachgestellt mit exakt deiner Marker-Lage: der Kontroll-
   Marker fing ihn mitten auf der Rampe im Bogen.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.39 (Early Preview)

**ROOM2090: Lauerstellung zur echten Leiter, natürliches Klettern, Bogen in Bogenform.**

---

Deine drei Punkte:

1. **Startrichtung**: Er lauerte messbar exakt auf meinen falschen Leiter-Anker
   (der lag 5,3 m südlich — eine Bilddeutungs-Panne). Die Leiter ist dein
   Einstieg an der Türnische oben links; der Anker sitzt jetzt dort. Kontrolle:
   Maul zeigt auf die Leiter, keine 180°-Wende mehr beim Kampfstart.
2. **Klettern**: Kein Sprung mehr an die Stegkante — er schwimmt sie jetzt
   sichtbar an (dabei saß er zunächst exakt am Kollisions-Radius fest: die
   Wand-Klemme drückte ihn vom Steg weg, während er hinwollte — behoben).
   Zusätzlich neigt sich der Körper entlang der Bahn: Aufstieg Nase hoch,
   Abstieg Nase runter.
3. **Bogenform**: Die Wirbelkrümmung war seitenverkehrt (beide Enden hoben
   nach oben). Jetzt wie von dir beschrieben: unten Kopf — Rücken oben —
   unten Schweif; im Bild hängt der Kopf vorn über die Kante, der Rücken
   wölbt sich über den Steg, der Schweif hängt hinten nach.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.38 (Early Preview)

**ROOM2090: die Überquerung kommt jetzt im normalen Kampf — und damit auch die Spinnen-Flucht.**

---

Deine Marker („Spinne flieht nicht, Krokodil klettert nicht über die Platform"):
Die Überquerung feuerte bisher nur, wenn Leon exakt AUF dem Plattform-Block
stand — beim Umlaufen des Beckens kam sie nie, und die Spinnen-Flucht hängt an
ihrem Start. Jetzt gilt dein Wortlaut („wenn der Aligator Richtung Platform
kommt"): Sobald die Plattform in der Verfolgung zwischen ihm und Leon liegt,
hebt er den Oberkörper und schiebt sich im Bogen darüber — die Spinnen rennen
in dem Moment zur Wand und klettern hoch. Nach einer Passage umschwimmt er das
Becken erst wieder ~10 s, damit sich beides abwechselt.

Dein Marker zeigte außerdem GAR KEINEN Alligator mehr in der Gegnerliste —
das konnte ich in Kampf-, Dauer- und Todes-Proben nicht reproduzieren (auch
seine Leiche bleibt liegen). Deshalb liegt jetzt ein Watchdog neben der exe:
`gator_boss.log` protokolliert Spawn und jedes Verschwinden mit Frame. Wenn
es dir wieder passiert, schick mir bitte diese Datei mit.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.37 (Early Preview)

**ROOM2090: der Alligator schwimmt wieder mit dem Maul voran.**

---

Dein Befund („läuft mit Hintern Richtung Charakter"): der 180°-Ausrichtungs-
Offset aus v0.7.35 war eine Fehldeutung meines damaligen Standbilds — beim noch
raumgroßen Modell hatte ich Kopf und Schwanz verwechselt. Das RE2-Modell steht
nativ richtig; der Offset ist entfernt, und alle Blick-Setzungen des Bosses
(Lauern zur Leiter, Überquerungs-Bahn) nutzen jetzt exakt dieselbe Engine-
Peilung wie die Verfolgung selbst.

Diesmal als Bewegt-Serie abgenommen (zwei Aufnahmen im Sekundenabstand): er
rückt auf Leon zu, und das Maul führt die Bewegung an.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.36 (Early Preview)

**ROOM2090: der Alligator auf ⅔-Maßstab — er war länger als der Pool.**

---

Dein Befund („ein wenig zu groß für den Raum") nachgemessen: das RE2-Modell ist
in Bindpose 18,3 m lang und 4 m hoch — länger als die kurze Poolseite (16,1 m).
Jetzt ⅔-Maßstab über den byte-true Entity-Render-Scale (derselbe Mechanismus,
mit dem das Original den Gorilla-Boss auf 1,7× zieht): ~12 m lang, ~2,6 m hoch
(≈ 1,5 Leon-Höhen), passt in die Ring-Rinne um die Plattform. Trefferboxen und
Reichweiten bleiben die byte-true 0x23-Werte; der Überquerungs-Bogen wurde auf
die neue Bauchhöhe nachgeeicht (er liegt am Scheitel auf der Plattform auf).

Sichtgeprüft: Lauerstellung an der Leiter und Plattform-Überquerung in
stimmiger Proportion.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.35 (Early Preview)

**ROOM2090: der Alligator-Bosskampf nach deinem Drehbuch.**

---

## Deine 8 Punkte

1. **Spinnen auf der Plattform**: die zwei Raum-Spinnen sitzen jetzt oben auf dem
   Mittelblock und bleiben dort verankert sitzen.
2. **Lauerstellung**: der Alligator startet links oben im Wasser und fixiert die
   Leiter an der Westwand. Die Türnische ist sicher — der Kampf beginnt erst, wenn
   du in den Pool vordringst, ihm direkt vor die Nase läufst oder schießt.
3. **Biss**: byte-true Fenster/Reichweite der 1.5-KI, jetzt mit echtem Schaden
   (50, die byte-true Tabellenzeile) + Knockdown. Ein Biss pro Angriff, danach
   die originale Wieder-Angriffs-Sperre.
4. **Boss-HP 3000** (10× der originale 300er-Wert — Magnum 10 Treffer,
   Shotgun 15, Pistole 150).
5. **Treffer zeigen**: Blutspritzer bei jedem Treffer am vorderen Rumpf; alle
   10% HP zuckt er sichtbar zurück (Flinch-Clip), dazwischen Boss-Panzerung.
6. **Ring-Verfolgung**: er umschwimmt den Plattform-Block auf der kürzeren
   Seite, statt in ihm hängenzubleiben.
7. **Spinnen-Flucht**: sobald er die Überquerung ansetzt, rennen die Spinnen
   zur Wand dahinter und klettern sie hoch.
8. **Plattform-Überquerung**: steht Leon oben, hebt der Alligator den
   Oberkörper, schiebt sich im Halbbogen über die Plattform (Wirbelsäule
   krümmt sich mit, Kopf taucht drüben ein, Schwanz noch diesseits) und
   gleitet auf der anderen Seite ins Wasser.

Sichtgeprüft am laufenden Spiel: Lauerstellung flach im Wasser, Lunge-Biss mit
aufgerissenem Maul, Überquerung mit gehobenem Oberkörper. Das RE2-Modell stand
anfangs 180° verkehrt (RE2-Grundpose blickt −X) — behoben im Renderer.

**Zum Selbsttesten**: RE15_GB_TEST=1 startet den Kampf sofort, =2 erzwingt
sofort die Plattform-Überquerung.

**Bekannt/offen**: Clip-Deutung der RE2-Bank ist Statistik-basiert (Biss/Flinch/
Todesrolle sitzen sichtbar richtig; Feinschliff nach deinem Eindruck). Der
Debug-Sprung direkt nach 2090 lädt je nach Latenz 25–55 s.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.34 (Early Preview)

**Karte: der aktuelle Raum zeigt jetzt seine Zeichnung — rot geschleiert statt schwarz. Plus: der Alligator schwimmt in ROOM2090.**

---

## Deine zwei Nachzügler-Marken + „Kartenstück freischalten"

**„Das soll weg verdammt!" — der Bogen (Blatt 1F):** Der Rest-Türschwenk stand in
der WANDFARBE (Index 4) und klebte an den kurzen Kastenwänden eines Zimmerchens —
das Lauflängen-Wachstum verschmolz beide, und die Größenschranke verwarf dann ALLES.
Zweiter Filterpass nur für solche Reste (Saat ≤3, Wachstum ≤6, Komponente ≤13 px =
Schwenkgröße §27). Gemessen am Blatt: 10 statt 9 Symbole entfernt, Nachbarblätter
unverändert in der dokumentierten Spanne.

**Wandbündige Türbalken:** die synthetischen Querbalken sind jetzt 1 px tief und
liegen AUF der Wandlinie statt 2 px in den Raum zu ragen (deine Marke 1,
„Wandstück links vom Spieler").

**„Wenn ich Room 1010 betrete … Kartenstück freischalten":** Der Raum WURDE
freigeschaltet — aber unsichtbar: die rote Modulation multiplizierte die grüne
Kachel-Kunst zu fast-Schwarz. Jetzt zeichnet der aktuelle Raum seine Kunst im
Besucht-Ton und legt einen halbtransparenten roten Schleier darüber (50/50-Mix,
neu im Rasterizer). Live geprüft (Framebuffer-Dump 1F + 3F): Wände, Auskerbungen
und Türsymbole sind unter dem Rot sichtbar.

## Außerdem (dein Auftrag 3)

**Alligator in ROOM2090:** Das RE2-EM23-Modell (22 Bones, 12 Clips) hängt an der
byte-true RE1.5-KI (Typ 0x23, State-Tabelle @0x80118bc8, Hitbox @0x80118b98) und
spawnt im leeren dritten Wasser-Areal des Pools (−5300, 0, −16300), Grid „Wasser".
Bitte im Spiel ansehen — Clip-Zuordnung der RE2-Bank ist noch nicht abgenommen.

**Bekannt:** Der Debug-Sprung direkt nach 1010/2090 wirft nach ~2 s in den
Vorraum zurück (Sprung-Spawn steht in einem Tür-AOT) — der normale Weg durch
die Tür ist nicht betroffen.

Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.33 (Early Preview)

**1F-Karte: Türsymbole standen über Kreuz — gespiegelte Zeilen werden jetzt erkannt.**

---

## Deine Marken 1–6 (Kartenansicht)

**Marken 3–6** („gehe oben durch die Tür, lande unten"): Engine und Türen waren
unschuldig — die HERGELEITETE Kartenzeile von ROOM1010 war z-gespiegelt (jeder Raum
hat sein eigenes lokales Koordinatensystem; die Herleitung kannte keine Spiegelung,
und bei zwei Türen zum selben Nachbarn paarte sie beide mit demselben Gegen-Record).
Jetzt: Ordnungs-Signatur je Achse (nur ausgelieferte Nachbarzeilen als Zeugen),
Spawn-basierte Gegentür-Paarung, Spiegel-Spalten wirken auch im Zeilen-Pfad.
Prüfmaß Türpaar-Konsistenz game-weit: Median 2,2 → 0,0 px; Ausreißer >12 px 19 → 8;
dein Fall 1010↔1020 von 24 px über Kreuz auf deckungsgleich.

**Marke 2** (Original-Tür „nicht sauber entfernt"): der Rest steht in Palettenindex 2
(orange), nicht in der Wandfarbe — zweite Filterklasse ergänzt (klein + wandnah;
die großen wandfernen Index-2-Flächen auf anderen Blättern sind legitime Kunst
und bleiben).

**Marke 1** (Wand-Stück an der Treppenkachel): Tabellenlage der Symbole ist per
Original-Anker korrekt; die Zeichnung an der Kachelkante braucht noch eine
verlässliche Panel-Messung — offen, kein Blindfix.

## Außerdem (dein Auftrag)

PRI-Aufräumen: 10 unbenutzte Dateien entfernt, 5 auf deine NN_MM-Nomenklatur
umbenannt (10D0/01a+01b→01_01/01_02, 06+06b→06_01/06_02, 10E0-Pult→07_01);
die abgenommenen Masken bleiben byte-identisch. Suite 282/282 (lokal + Docker).

---

# RE1.5 Port — v0.7.32 (Early Preview)

**1F-Karte: das Treppenhaus landet auf seiner Treppenkachel.**

---

## Dein Befund (fehler/MAP03.bmp)

Ankunft unten im Treppenhaus (1F): die Karte markierte den langen Flur statt des
kleinen Treppenbereichs. Gemessen: ROOM1060 saß per Zeilen-Block-Override auf
Rect 2 (Westflur-Zeichnung); die Treppenhaus-Vorgabe war nie wirksam, und auf der
Treppenkachel (Rect 10) saßen stattdessen zwei Flur-Splitter (ROOM1000-z0/1030).

**Fix:** ROOM1060 → Rect 10 (die 24×24-Treppenkachel mit Treppensymbol), die
Splitter → Rect 4 über eine neue NICHT-exklusive Vorgabenliste (ein harter
ZONE_FIX hätte Rect 4 gesperrt und die geeichten Nachbarn verdrängt — die
Kaskade des ersten Versuchs). Nebeneffekt, ebenfalls belegt: die alte
Fahrstuhl-Vorgabe (ROOM1080 → Rect 9) greift jetzt auch.

**Beweis-Anker:** die Tür 1040→1060 projiziert über 1040s AUSGELIEFERTE
Maßstabszeile auf Karte (118,151) = die linke Kante von Rect 10; die Tür
1040→1080 auf (114,145) = Kante von Rect 9. Der alte 1060-Marker rechnete sich
auf ~(112,154) — unten in Rect 2, exakt „der längere Flur" deines Befunds.

Die Gegenprobe im Kartentest wurde getauscht (1000↔1010 war ungültig — ROOM1000
ist ein Flur, seine zweite Zone saß schon immer Rahmen-an-Rahmen mit Rect 1):
neu ROOM1010↔ROOM10A0, Türgraph-Abstand 5, Rahmenlücke 15 px. Suite 282/282
(lokal + Docker).

---

# RE1.5 Port — v0.7.31 (Early Preview)

**Dein Lasso IST die Stuhl-Zuordnung — Kunst trägt jetzt die Tiefe ihres eigenen Stuhls.**

---

## Communication Room, fünfte Runde (deine zwei Marken F697/F1327)

Dein Befund nach v0.7.30: der Stuhl deckt dich weiterhin, obwohl du davor stehst.
Die Messkette: du standest bei z=11816 ZWISCHEN zwei Stühlen — vor dem fernen
(Zelle 12200..13700), hinter dem nahen (8200..9700). Die Kunst des FERNEN Stuhls
über deiner Hüfte trug die Tiefe der NAHEN Zelle (z 5120..6464 statt ~8900).

**Ursache:** die Zuordnung "welche Zellen-Box trifft der Sehstrahl zuerst"
beantwortet nicht, welchen Stuhl das Pixel ZEIGT. Die Kollisionszellen sind
1500×1500, die gezeichneten Stühle ~700 — die Fantasie-Box der nahen Zelle
überlappt im Bild die Kunst des fernen Stuhls.

**Fix:** gemessen liegt jede deiner Lasso-Komponenten zu ≥0,99 in genau EINER
Zellen-Silhouette — also bekommt jedes Lasso als Ganzes den Quader SEINER Zelle,
und die Schwarz-Regel etikettiert: Tiefschwarz ≤7 Pixel neben eigener Kunst in
eigener Silhouette = Lehnen-Lücke (gemessen: Lücken-Klasse 1..7, Pult-Dunkel ab 7),
sonst nächste enthaltende Zelle, ohne Silhouette gar nicht (kein Boden-Blit).

## Gemessen, an neun Marken

```
ungerechtfertigte Deckung (Maske näher als Szene):  0 an allen neun Marken
Sichtproben F697/F1327: Bein wieder sichtbar; nahe Stühle decken weiter korrekt
Suite 282/282 (lokal + Docker); Masken in beiden Paketen hash-verifiziert
```

---

# RE1.5 Port — v0.7.30 (Early Preview)

**Das Atlas-Leck: eine Stuhl-Kachel deckte fremde Pixel mit der eigenen Tiefe.**

---

## Communication Room, vierte Runde (deine zwei Marken)

Dein Befund: die beiden Stühle im Hintergrund decken Leon auch dann, wenn er
davor steht. Die Messkette führte NICHT zu den Tiefen der Lasso-Objekte, sondern
in den Atlas-Packer:

1. **Täter identifiziert:** die deckenden Pixel an F1476 sind Kunst des FERNEN
   Stuhls (−1600, 12200) — sein Quader-Raycast steht dort bei vz≈8900..9280
   (dürfte dich nie decken). Geblittet wurden sie aber von Kacheln des NAHEN
   Bürostuhls (−1800, 8200) mit t=97..103 (z 6208..6592).
2. **Ursache:** `atlas.build` machte jede Kachel mit der VEREINIGUNG aller
   Objektregionen deckend — jede Kachel nahm Fremd-Pixel in ihrem Rechteck mit
   und gab ihnen die eigene (hier: zu nahe) Tiefe. Deshalb blieben die
   Messzahlen über drei Umbauten bitgleich: die Täter-Kacheln gehörten einem
   Objekt, das ich gar nicht anfasste.
3. **Fix:** jede Kachel deckt nur noch Pixel des EIGENEN Objekts (Herkunft wird
   durch Zerlegung und Halbierung verfolgt). Dazu läuft die Szene-Attribution
   der Lassos nur noch über Möbel-Kreiszellen — Schreibtisch-Säulen ohne Deckel
   schnappten sich sonst Pixel, über die man in Wahrheit hinwegsieht.

## Gemessen, an sieben Marken

```
ungerechtfertigte Deckung (Maske näher als Szene):  212 -> 0
Alt-Neu-Diff je Marker-Kasten: weggefallen NUR Leck-Deckung ohne Möbel davor,
0 gerechtfertigte Deckung verloren, 0 neu hinzu
Sichtproben F1254/F1476: Stuhl deckt hinter ihm, gibt frei davor
Suite 282/282 (lokal + Docker)
```

---

# RE1.5 Port — v0.7.29 (Early Preview)

**Die Stuhllehnen decken bis zur echten Höhe — Schwarz zählt als Stuhl.**

---

## Communication Room, dritte Runde (deine vier Marken)

Zwei gemessene Ursachen, warum Leon noch teilweise im Vordergrund war:

1. **Die schwarzen Lehnen sind höher als gedacht:** an drei Schwarz-Säulen aufgelöst
   liegen sie bei h≈−1950 (−1970/−1927/−1970) — meine −1100-Boxen endeten darunter,
   die Lehnenspitzen blieben frei.
2. **Dein Lasso hat schwarz-auf-schwarz-Lücken** (gemessen: bg-Farbsummen 4..32 an
   Pixeln, die nicht im Lasso sind — dieselbe Klasse wie beim 10D0-Klappstuhl). Mit
   „nur Kunst deckt" blieb genau dort nichts übrig.

**Fix:** Höhe −1950, und die Deck-Region ist jetzt *deine Kunst ODER Tiefschwarz*
(Farbsumme < 45). In diesem Raum ist Schwarz eindeutig Stuhl — der Boden ist hell
(Summe 180+), die Boden-Überblendungen aus der zweiten Runde bleiben damit
ausgeschlossen.

## Gemessen, an fünf Marken

```
schwarze Vordergrund-Stuhlfläche:  1790 / 747 / 878 / 1049 / 1711 Punkte
davon ungedeckt:                      0 /   0 /   0 /    0 /    0
Sichtproben F1195/F1321: Stuhlmechanik, dunkle und weiße Lehne sauber VOR der Figur
```

Tests: **282/282** (im Release-Container).
