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
