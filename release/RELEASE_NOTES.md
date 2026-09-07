# RE1.5 Port — v0.7.8 (Early Preview)

**Der letzte Kartenfehler aus `error1..3` — und ein Messfehler von mir, der zwei Runden
gekostet hat.**

---

## Die Wand rotete in den fremden Bereich hinein

Du stehst im oberen kleinen Raum von ROOM1110. Der untere ist korrekt grün **gefüllt** —
aber seine linke Kante leuchtet rot mit. Die senkrechte Wand bei x=188 läuft 23 px weit
über **beide** Teile und bekam eine einzige Farbe: den stärksten Zustand ihrer Nachbarn,
also rot.

Jetzt trägt jeder Pixel der Wand den Zustand des Bereichs, an dem er entlangläuft; gleiche
Farben werden zu einem Lauf zusammengefasst. An der Op-Liste des Kartenschirms gemessen:

```
im oberen Raum:   (188,122) 1x12 ROT   + (188,134) 1x11 gruen
im unteren Raum:  (188,122) 1x11 gruen + (188,133) 1x12 ROT
im Evidence Room: (188,122) 1x23 ROT   (grenzt durchgehend an den aktiven Bereich)
```

Als Prüfung festgeschrieben: an keinem der drei Standorte darf ein roter Wandabschnitt
*innen* in einem nicht-aktiven Bereich liegen.

## Was ich falsch gemacht habe

Mein Befund davor — die nicht-aktiven Bereiche würden **gar nicht gemalt** — war falsch.
Ich hatte deine Screenshots über einen Farbfilter gerastert, der nur *helles* Grün erfasste;
die Füllung ist aber `rgb(0,64,40)`. Die Bereiche erschienen dadurch als „leer", und ich
habe daraus eine Überdeckung durch ein Nachbar-Rechteck abgeleitet und dagegen gefixt. An
denselben Bildern, an Stellen die deine Pfeile nicht verdecken, nachgemessen: **die Füllung
war die ganze Zeit richtig.** Deshalb hat sich für dich nichts geändert, obwohl ich zweimal
„gefixt" gemeldet habe.

Der Zwei-Durchgang-Fix bleibt drin — er ist für sich korrekt —, aber er hat nicht behoben,
was du gesehen hast.

**Merksatz, den ich mir notiert habe:** erst die Rohfarbe lesen, dann klassifizieren. Ein
Filter, der eine Farbe nicht kennt, meldet „nichts da" statt „unbekannt".

---

**282/282 Tests**, lokal und im Linux-Container.
