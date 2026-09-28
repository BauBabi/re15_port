# v0.8.16 - 2026-09-28

Das Gelaendertor am Hubschrauberlandeplatz (ROOM1170) oeffnet sich jetzt wie eine Tuer in
Resident Evil 2: mit eigener Tuerszene, Bewegung und dem Ton des RE2-Gittertors. Grundlage
ist das Tuermodell, das ich Ihnen aus Ihren drei Ausschnitten gebaut habe. Suite 362/362.

## Was Sie merken

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

## Wie es gebaut ist

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

## Was bewusst offen blieb

- **Nur dieses Tor** hat eine Sequenz. Die RE1.5-Raumdaten sagen fuer keine Tuer, welches
  Modell sie haette; jede weitere Tuer braucht ein eigenes Modell oder ein passendes
  RE2-Archiv und einen Eintrag in der Zuordnung.
- **PSX-Stand:** die Szene laeuft auf PC, Steam Deck und Android; der PSX-Build geht ohne
  Sequenz durch das Tor wie bisher.

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
