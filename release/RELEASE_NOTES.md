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
