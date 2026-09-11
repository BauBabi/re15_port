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
