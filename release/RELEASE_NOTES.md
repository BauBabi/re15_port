# RE1.5 Port — v0.3.24 (Early Preview)

## Gegenstands-Namen fehlten in der Aufnahme-Abfrage

Gemeldet: *"beim nehmen des Feuerloeschers room 1000 steht noch 'will you take ?' statt
'will you take fire extinguisher'."*

Die Namensliste, aus der die Abfrage den Gegenstand einsetzt, war im Port bei 48 Eintraegen
abgeschnitten. Der Feuerloescher steht in den Raumdaten an Position 49 und fiel damit
heraus — und weil der Port in so einem Fall stillschweigend gar nichts einsetzt, blieb
zwischen "the" und "?" schlicht eine Luecke.

Die Liste des Originals hat 102 Eintraege. Sie wird jetzt vollstaendig aus den Spieldaten
erzeugt, samt aller acht Abfrage-Texte statt bisher vier. Betroffen war nicht nur der
Feuerloescher: alles ab Position 49 war namenlos — Zangen, saemtliche Keycards, die Akten,
die Minidisc, der Timer, die Impfstoffe. Eine neue Pruefung faehrt jetzt alle Raeume des
Spiels ab und verlangt fuer jeden Gegenstand, den man wirklich aufheben kann, einen Namen.

## Combine: Sound nachgeruestet

Gemeldet: *"beim Combine von items fehlen Sound und Effekte ... schaue wie das echte re2 das
macht und ruestet Sound und Effekt nach."*

Beim Nachsehen hat sich der Wunsch geteilt. Der **Sound** fehlte tatsaechlich: RE1.5 spielt
beim Zusammensetzen nichts ab, obwohl dasselbe Menue an anderer Stelle laengst Toene benutzt.
Nach RE2-Vorbild gibt es jetzt einen Ton fuer die gelungene Kombination, einen anderen fuer
die misslungene, dazu Abbruch- und Cursor-Ton im Kombinations-Bildschirm. Alle vier stammen
aus RE1.5s eigener Tonbank — es wurde nichts importiert.

Der **Effekt** fehlte dagegen nicht: RE1.5 hat einen eigenen 17-Bilder-Puls auf dem
Ergebnis-Feld, und der ist im Port vollstaendig vorhanden. RE2 hat an derselben Stelle
etwas anderes (ein Verschieben ueber zehn Bilder) — kein zusaetzlicher Effekt, sondern ein
anderer. Deshalb wurde nur der Ton nachgeruestet und der vorhandene Effekt nicht angetastet.

## RE2-Modus: Zombies waren zeitweise unverwundbar

Gemeldet: *"bei re 2 ai sind die Zombies immer noch manchmal unverwundbar bis sie mich
einmal gebissen haben."*

Der Verdacht lag auf einer alten Trefferstelle — der war es nicht, die ist dicht. Gemessen
wurde etwas anderes: Wird ein Zombie im Kampf zum Kriecher umgebaut, behielt er eine
Markierung "liegt am Boden". Der Port zieht daraus die Zielhoehe, und mit normal gehaltener
Waffe ging jeder Schuss ueber ihn hinweg. Betroffen war rund jeder elfte Kampf, und die
Phase endete genau dann, wenn der Kriecher zubiss — daher der beschriebene Eindruck.

Gemessen: vorher null Treffer bei normalem Zielen, jetzt alle. Der Kriecher aus dem
RE1.5-Modus bleibt unveraendert, der arbeitet ueber einen anderen Eintrag.

## RE2-Modus: Zombies beim Aufstehen anschiessen

Gemeldet: *"bei der aufsteh Animation vom Zombie ist er unverwundbar. original kann man da
bereits anschliessen. auch bei re 2."*

Stimmt. RE2s Treffer-Pruefung kennt ueberhaupt keine Zielhoehe und gibt den Gegner schon
frei, waehrend er sich aufrichtet. Der Port hat ihn dagegen bis zum letzten Bild der
Aufsteh-Bewegung gesperrt — ueber eine Hilfs-Markierung, die es im Original gar nicht gibt.

Jetzt trifft man ihn, sobald er sich zu erheben beginnt. Der am Boden liegende Zombie, der
erst geweckt werden muss, bleibt dagegen geschuetzt — das ist im Original so und bleibt so.
Ein Nebeneffekt, der auffallen wird: erschiesst man einen Zombie beim Aufstehen, faellt er
jetzt in der Boden-Variante um statt in der Stand-Variante. Auch das ist Original-Verhalten.

## ROOM1220: die Kriecher waren gar nicht da

Gemeldet: *"im room 1220 sollten die Zombies eigentlich kriechen. auch bei re 2 ai."*

Die Kriech-Mechanik aus der letzten Version funktioniert dort — der Raum war schlicht leer.
Die zwei Kriecher hinter der ersten Tuer teilen sich ihre Kennung mit den Kreaturen im
Vorraum ROOM1210. Der Port merkt sich den Tod jedes Gegners unter dieser Kennung; wer also
im Vorraum aufgeraeumt hatte, loeschte damit die Kriecher aus dem Nachbarraum, ohne sie je
gesehen zu haben. Im Original merkt sich diese eine Kreaturenart ihren Tod nicht.

Wer durch die anderen vier Tueren kommt, trifft weiterhin auf aufrecht gehende Zombies —
so steht es in den Raumdaten, das ist kein Fehler.

## Effekte hingen nicht mehr in der Luft

Gemeldet: *"alle effekte wie strom, feuer etc. ueberdecken nicht sichtbare bereiche. zum
beispiel wenn sie noch um die ecke hinter der kamera sind."*

Zwei Ursachen. Erstens fehlte den Effekten der Sichtbarkeits-Test, den Spieler, Gegner und
Objekte laengst haben: das Original zeichnet einen Effekt nur, wenn sein Ursprung im Bereich
der aktiven Kamera-Einstellung liegt. Zweitens lag ihre Tiefe auf einer sechzehnfach zu
kleinen Skala — dadurch lag praktisch jeder Effekt vor jeder Wand und vor jeder Figur.

Gemessen an den echten Raumdaten: von rund 1500 Kombinationen aus Effekt und
Kamera-Einstellung wurden vorher etwa 940 faelschlich gezeichnet. Strom, Feuer, Blut,
Muendungsfeuer und Rauch verschwinden jetzt hinter Waenden und um Ecken, und sie bleiben
dort sichtbar, wo sie hingehoeren.

## Noch offen aus dieser Runde

Die Zombie-Haende am Gitter in ROOM1210 sind untersucht, aber bewusst noch nicht umgebaut.
Der Befund: im Port stehen die zehn Kreaturen dauerhaft still, obwohl das Original sie
ausfahren und zucken laesst — der Raum schaltet sie zur Laufzeit scharf, und der Port
uebersieht diesen Schalter. Die Original-Mechanik ist inzwischen vollstaendig aufgeschluesselt
und dokumentiert.

Der gewuenschte Umbau nach RE2-Vorbild (Haende kommen erst, wenn man am Fenster vorbeilaeuft,
mit Stoehnen und Zugriff) steht dagegen noch aus: die entsprechende RE2-Szene liess sich
bisher nicht als Mechanik nachweisen, und es wird nichts erfunden, was sich nicht belegen
laesst. Die Suche laeuft an zwei konkreten Stellen weiter.

Ebenfalls weiter offen aus der Vorrunde: der leicht schwebende Zombie im RE2-Modus, das
Entfernen der Flammen in ROOM1090 nach dem Loeschen, und Adas Folge-Verhalten nach der
Zwischensequenz.
