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
