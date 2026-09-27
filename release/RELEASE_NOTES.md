# v0.8.15 - 2026-09-27

Diese Version bringt eine einzige Sache sauber zu Ende — warum ein liegender Hund nicht
getroffen werden kann — und dabei muss ich drei eigene Aussagen zuruecknehmen. Dazu die
Trefferboxen aller RE2-Gegner, ein dritter Fahrstuhl mit Ton, und ein Raetsel, das ich
bewusst NICHT gebaut habe. Suite 359/359.

## Was Sie merken

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

**Die Charakterwahl fuehrt nicht in Elzas Zweig.** Sie haben es gemeldet, und es stimmt.
`re15_gameflow.c:38-39` merkt sich die Wahl und setzt trotzdem unbedingt denselben
Startraum; `g_gameflow.character` wird im ganzen Port an drei Stellen gelesen, in der
Raumwahl an keiner. Die 40 ungeraden RDT je Stage — Elzas Raeume — sind damit tote Daten.

Dabei schon belegt: `DAT_800aca5c` traegt **0 fuer Leon und 4 fuer Elza** (Index in die
CORE-Bank-Tabelle @0x80073a88: Index 0 -> 0xA1 = CORE00, Index 4 -> 0xA9 = CORE04). Der
Port speichert den durch vier geteilten Wert als 0/1, testet ihn aber weiter mit der
ungeteilten Maske `& 4` — das ist fuer beide Charaktere null, Elzas Zweig des
Kriech-Grab-Abwurfs ist unerreichbar. Das ist in Arbeit.

## Was bewusst offen blieb

* Der Klassen-Nuller @0x80103874-A0 — gebaut waere er eine Dauersperre (dieselbe Falle wie
  in Runde 13/14, wo ein Gegner dauerhaft unverwundbar wurde).
* Der Schadensrecord des Hundes @0x800A4424 ist gedumpt, aber nicht verdrahtet: diese Runde
  stellt den Treffer-Mechanismus um, nicht das Schadensmodell.
* Die Bolzen-Aufrufstelle (Waffe 12) hat im Port keinen Produzenten — keine der 22
  RE1.5-Waffen bildet auf diese Id ab.
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
