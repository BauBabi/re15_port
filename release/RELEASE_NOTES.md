# v0.8.20 - 2026-09-30

Ihre Hausaufgaben fuer die Nacht sind gebaut — sieben Themen, jedes von einer eigenen Pruefung an der
echten exe abgenommen. Suite 428 -> 463/463. (Die Granaten folgen als v0.8.21 aus der Parallelsitzung.)

## Was Sie merken

**Das Rolltor in ROOM1050 geht nur noch mit eingesetzter Sicherung auf — mit Nahansicht.** Am Schalter
kommt zuerst wie bisher die Frage. Ohne Sicherung zeigt die Nahansicht (Cut 7: leerer Sockel, rote
Leuchte) "I need a fuse to run the shutter." — das Tor bleibt zu. Mit der Sicherung aus Irons'
Hebetisch: "Will you use the Fuse?" -> Ja: die Sicherung verlaesst das Inventar, die Nahansicht zeigt
sie eingesetzt (Cut 8, beide Leuchten gruen) mit "You've used the Fuse."; ab dann oeffnet der Schalter
das Tor wie im Original, mit Frage und Ton. Vorbild ist RE1.5 selbst: dasselbe Raetsel steht fertig in
ROOM2060, die beiden Saetze sind byte-gleich von dort uebernommen. Das Entfernen aus dem Inventar macht
wie in RE2 der Befehl Sce_item_lost (RE1.5 kennt keinen). Gilt auch fuer Elza (ROOM1051). Das Tor ist
der einzige Weg in den Suedteil — die Sicherung ist damit Pflicht; verlieren kann man sie nicht.

**Irons' Hebetisch bedient man jetzt mit dem Cursor.** Aktionstaste an der Westseite des Tisches ->
Nahansicht, und es erscheint der Cursor des Generator-Raetsels (bildgleich), das Steuerkreuz bewegt ihn.
Quadrat auf der Kuppel unten rechts -> Klick-Geraeusch wie am Generator-Schalter, die Kuppel geht auf,
danach wie bisher Sicherung und Granate aus den Faechern. Quadrat daneben -> "Nothing happened.",
danach ist der Cursor wieder frei. Kreuz verlaesst den Cursor; die naechste Aktion bringt ihn zurueck.

**Generator ROOM11F0: frei bewegen, Sperre nur am Ende, zwei gruene Lampen.** Nach jedem Schalter bleibt
der Cursor frei, der Zeiger faehrt nebenher auf den neuen Wert (auch wenn Sie weiterschalten). Erst wenn
die Stellung stimmt (Schalter 1, 3, 5, 7, 9 an), wird die Eingabe gesperrt, der Zeiger faehrt auf 80,
steht 30 Bilder (wie RE2), dann "Power supply OK." und die Flurlichter wie bisher. Die obere Lampe
leuchtet gruen, wenn die linke Spalte (Schalter 1-5) stimmt, die untere fuer die rechte Spalte (6-10);
ein falscher Schalter einer Seite loescht ihre Lampe sofort. RE1.5 hat fuer diese Lampen keine
Leuchtkunst — genommen ist die gruene Schalterlampe desselben Bedienfelds aus RE2 (ROOM2130), mit
RE2s Flimmern. (Alternative, falls lieber: RE1.5s runde gruene Leuchte aus ROOM5060.)

**Ada ruft an der Tuer nach ROOM10A0.** Beim ersten Versuch, von ROOM1050 nach ROOM10A0 zu gehen,
solange Ada nicht gerettet ist, spielt eine Szene: "Woman: Hello? Anyone? Please, get me out of here!",
Leon tritt von der Tuer zurueck (derselbe Rueckschritt wie in ROOM1090 am Feuer), "Leon: Another
civilian survivor." mit der Arm-Geste hin und zurueck, "Leon: I have to help her!" mit dem Arm-Schwung.
Danach steht an der Tuer "I have to help the Survivor first!" — bis zur Rettung in ROOM1090, dann geht
die Tuer normal (mit Tueranimation). Freigegeben wird ueber das Rettungs-Flag (3,0xBB); das naheliegende
(3,0x6E) loescht ROOM1050 beim Wiederbetreten selbst und haette die Tuer fuer immer gesperrt.
Feuerloescher und Ada sind ohne ROOM10A0 erreichbar. Elza hat keine Ada-Rettung und keine Sperre.

**Vier neue Dokumente.** "Police Officer's Final Diary Entry" auf dem Schoss der sitzenden Leiche vor dem
Rolltor (ROOM1050), "Elliot's Diary" auf der Bank in der Umkleide (ROOM1000, Modell wie Irons Diary),
"Marvin's Notes" auf Marvins Schreibtisch (ROOM1020), "Armory Notice" auf dem Verhoertisch (ROOM1010,
neben der Spraydose, die dort im Spiel genau unter Ihrer Markierung liegt). Texte woertlich, Aufnahme
per Aktionstaste, lesbar im FILE-Menue, danach ist das Modell weg; Speichern und Laden behalten alles.
Die Codes stimmen mit den Schloessern im Spiel: 4312 = Communication Room (ROOM10D0), 5632 = Waffenkammer
(ROOM1230).

**Die Leichen in ROOM1110 und ROOM1230 halten Munition.** Evidence Room: "It's a police officer, he's
dead." / "He is holding something." -> Handgun-Munition mit Aufnahme-Abfrage; Nein -> beim naechsten
Untersuchen wieder. Nach dem Ja nur noch "It's a police officer, he's dead." ROOM1230 genauso mit
"A miserable death..." Menge wie die haeufigste Packung in RE1.5 und RE2 (15), mit Ihrer
Halbierungsregel +7 Schuss. Die Original-Zettel mit den Codes, die dort lagen, entfallen — die Codes
stehen jetzt in Marvins Notiz und im Armory Notice.

**Die Leuchtschrift "HEAVEN" in Irons' Buero blinkt.** Cut 2, hinter den Jalousien: das Raumskript
schaltet die Buchstaben ueber den Befehl 0x45 alle 20 Skripttakte (40 VBlanks) an und aus — der Port
kannte den Befehl nicht und zeigte die Schrift immer an. Weil der Befehl allgemein wirkt, zeigen jetzt
auch acht andere Raeume, was das Original zeigt: ROOM3000/3001 und ROOM3010/3011 in der Zombie-Variante
(die gemalten toten Polizisten verschwinden, wenn sie als Zombies aufstehen — am Original in DuckStation
gegengeprueft), ROOM3071 Cut 9 (Lichtleiste aus bis zur Elza-Szene), ROOM1211 Cut 7 und ROOM5060/5061
Cut 11 (nach dem jeweiligen Ereignis). In ROOM1170 blinkt im Original nichts — dort bleibt alles, wie es ist.

## Grenzen

* **Sprachaufnahmen fehlen noch** (RE1.5 hat keine englische Sprachausgabe): Ablage
  `synchro/STAGE1/room1050/main22.wav` (Woman, hoechstens 3,3 s — sonst tritt Leon schon zurueck,
  waehrend sie noch spricht), `main23.wav` ("Another civilian survivor."), `main24.wav` ("I have to help
  her!"). Bis dahin laufen die Zeilen mit Untertitel.
* Rolltor: im normalen Blick (Cut 3) behaelt der kleine Kasten seine rote Leuchte — fuer diesen Cut gibt
  es keine gemalte Variante.
* Hebetisch: das Motorgeraeusch des Tischs kommt jetzt schon beim Druck am Tisch (Folge davon, dass der
  Original-Ablauf unveraendert bleibt).
* Generator: gehaltenes Quadrat legt etwa alle 17 Bilder denselben Schalter erneut um (RE1.5 selbst;
  vorher von der Sperre verdeckt).
* Dokumente: so hell wie die Original-Items der Raeume (byte-true Licht) — wirkt eines zu dunkel, sagen
  Sie Bescheid. Der lange Name "Police Officer's Final Diary Entry" ragt beim Anwaehlen ueber die
  Markierung hinaus; die Ablagemeldung lautet nach RE1.5-Vorlage "The Elliot's Diary has been filed.".
* Leichen: "police"/"holding" klein und "..." statt "…" — so stehen die Woerter bzw. Punkte im
  Original-Zeichensatz (eine Auslassungs-Glyphe gibt es nicht).
* Nur PC-Ziel gebaut; das PSX-Ziel kennt die neuen Teile noch nicht.

# v0.8.19 - 2026-09-29

Ihre vier Auftraege dieser Runde sind gebaut. Suite 416 -> 428/428.

## Was Sie merken

**Speichern nur mit Memory Card.** Wie RE2s Schreibmaschine mit dem Farbband (AOT-Typ 9,
Handler 0x80051AB0; Farbband-Suche nur im INVENTAR, nicht in der Item-Box): ohne Memory Card
kommt nach dem ersten Satz der Stelle "If I had a Memory Card, I could save my progress...", mit
Memory Card "You can save your progress with this. Will you use the Memory Card?" mit Ja/Nein;
Ja oeffnet den Speicherbildschirm. RE2s Wortlaut (0x8009EFCC / 0x8009F01F), nur "Ink Ribbon" ->
"Memory Card". Die Karte wird NICHT verbraucht (Ihr Wunsch; RE2 verbraucht das Farbband).
Gilt an allen 16 Speicherstellen. Aeltere Spielstaende haben noch keine Memory Card — sie liegt
auf Irons' Schreibtisch.

**Karte: nach dem Irons-Hinweis ist 2F waehlbar, der Communication Room bleibt markiert.**
Nach der Szene springt die Karte mit "runter" von 3F auf 2F (vorher uebersprungen, solange 2F
nie betreten war) und zeigt dort den Communication Room im selben Rot/Umriss-Blinken wie im
Hinweis — bis Sie ihn betreten, danach erscheint er als besucht. Das ueberlebt Speichern und
Laden. RE2 selbst hat keine dauerhafte Ziel-Markierung (sein Kartenzeichner FUN_8006E120 kennt nur
besucht / aktuell / unbesucht); gebaut auf Ihren Wunsch, im Stil des RE2-Hinweises.

**ROOM1130 -> ROOM1120 erst nach Irons.** Bis zur ersten Szene mit Chief Irons steht an der
Tuer "I have to report the situation to the chief first..." — kein Raumwechsel, keine
Tueranimation. Die Szene spielt uebrigens in seinem Buero ROOM1150 (Flag 3/94, gesetzt in
ROOM1150 sub08 @0x01110); in ROOM1170 gibt es keine Irons-Szene. RE2 sperrt Story-Tueren genauso,
ohne Ton (ROOM2190 "I have to get back to Ben!"). Elza hat keine Irons-Szene, fuer sie bleibt
die Tuer offen (sonst kaeme sie nie weiter).

**Tueranimationen jetzt fuer 141 von 144 Tueren.** Die 57 Tueren, die es in RE2 nicht genau
so gibt, haben jetzt eigene Tuermodelle: Bewegung, Skripte, Ton und Griff vom naechsten
RE2-Archiv, die Textur so bearbeitet, dass sie der gemalten RE1.5-Tuer entspricht (z.B. die
glatten dunklen Stahltueren im Revier = DOOR07 ohne Lueftungsschlitze, umgefaerbt, Druecker mit
Schild wie gemalt). Die drei Tueren, die RE1.5 auf beiden Seiten verschieden malt, sind jetzt von
beiden Seiten animiert; Treppenhausseiten haben ihre graugruene Farbe. Durchgaenge ohne
Tuerblatt zeigen, was RE2 dort zeigt (Blende mit Ton, Archiv DOOR36). Nicht animiert sind nur
drei Uebergaenge in ROOM1250, die im Spiel nie begehbar sind.

## Grenzen

* Das Hochklettern in eine Nische (T085) nutzt ebenfalls die Blende: RE2 hat fuer Klettern
  nichts Passendes.
* Kleinigkeiten bei den neuen Tueren: Strahlenschild P27S sitzt etwas tiefer als gemalt, Handrad
  P26W bleibt in der Blattmitte, Toiletten-Piktogramme innen gespiegelt, das Gittertor behaelt
  RE2-Rauten, zwei sehr hell gemalte Tueren sind an der Obergrenze der Helligkeit.
* Die Toene sind nach Stimme und Tonhoehe gemessen, nicht angehoert.

# v0.8.18 - 2026-09-29

Ihre drei Befunde zu v0.8.17 sind behoben. Suite 411 -> 416/416.

## Was Sie merken

**Granate und Sicherung liegen jetzt IN den Faechern.** Der hochfahrende Tisch in Irons' Buero
hat unter der Tischplatte zwei offene Faecher mit einer Trennwand. v0.8.17 hatte beide
Gegenstaende oben in die Kuppel zwischen die Papierstapel gelegt — das war falsch. Jetzt liegt die
Granate auf dem Boden des LINKEN Fachs, die Sicherung auf dem Boden des RECHTEN (im Bild
gemessen: Schwerpunkt Granate x 101, Sicherung x 209 von 320). Beide sind zu sehen, sobald der
Tisch aus dem Schreibtisch faehrt, und stehen beim Dialog oben in der Ruhe ganz im Bild. Der
Dialog kommt weiterhin erst, wenn der Tisch oben ruht, erst die Sicherung, dann die Granate.

**Keine Verzerrung mehr beim Oeffnen der Tueren.** RE2 zerlegt die grossen Flaechen eines
Tuerblatts beim Zeichnen in 8 x 8 kleinere Dreiecke (Flag 0x20 im Aufstell-Satz, Routine
DivideGT3, Tiefe 3 @0x80013dc0; Abfrage @0x80014a30). Der Port zeichnete das Blatt als zwei grosse
Dreiecke — deshalb knickten Kanten und Felder beim Aufschwingen zu einem "V". Jetzt wie in RE2
unterteilt: die Kante zwischen Fenster und Feld an DOOR13 weicht in der Mitte der Oeffnung
hoechstens 3,8 statt 27,8 Bildpunkte von der Geraden ab. Nachgeprueft, indem der ORIGINAL-Code aus
der RE2-EXE Befehl fuer Befehl gegen den Port gerechnet wurde (8731 Faelle, 170 864 Teildreiecke
gleich). Betroffen sind die 12 Archive, deren Blatt das Flag traegt; alle anderen Tueren sind
bildgleich wie vorher.

**Das Tor am Landeplatz ist so hell wie das gemalte Tor.** Es war 1,92-fach zu dunkel: die
Tortextur stammt aus dem beleuchteten Hintergrundbild, und das RE2-Tuerlicht hat sie ein zweites
Mal abgedunkelt (Faktor 1,75, Grundhelligkeit 68 @0x800142e8); dazu kamen 9 % Verlust beim
Umrechnen der Farben. Jetzt traegt das Tor RE2s Flag fuer hellere Modelle (Grundhelligkeit 136,
@0x800142b4 — dasselbe Flagwort wie die RE2-Klappe DOOR2B), die Farben werden gerundet, und die
Textur ist auf dieses Licht abgestimmt: gemalt/gezeigt 1,000 an denselben Bildstellen.
Mitbehoben: helle Eckfarben ueber 0x80 hellen jetzt wie auf der PSX auf (Texel x Farbe / 128);
RE2-Tueren werden dadurch nur an wenigen Ecken bis zu 7 Stufen heller — naeher an RE2.

## Grenzen

* Die Granate wirkt im Fach dunkel: das ist das Raumlicht des Schreibtischs, nicht die Lage.
* Das Tor entspricht dem Bild aus Cut 12 (Quelle seiner Textur); Cut 0/11 malen das Schild
  heller.
* Die Aufhellung ueber 0x80 gilt nur in der Tuerszene; Figuren und Raum-Modelle kappen weiter.

## Technik

* `make_package.sh` beendet im Laufzeit-Test nur noch die eigene exe (vorher jede gleichnamige).

# v0.8.17 - 2026-09-29

Ihre drei Auftraege dieser Runde sind gebaut: RE2-Tueranimationen fuer die Tueren, die es in
RE2 genauso gibt, der Hebetisch in Irons' Buero und der Generator in ROOM11F0. Jede Aenderung
hat ein zweiter Agent im echten Spiel gegengeprueft. Suite 405 -> 411/411.

## Was Sie merken

**RE2-Tueranimationen — 83 von 144 Tueren.** Jede Tuer, die RE1.5 aufstellt, wurde im
Hintergrundbild ausgeschnitten und mit den 55 RE2-Tuerarchiven verglichen, Seite fuer Seite am
Bild, jede Zuordnung von einem zweiten Agenten auf Widerlegung geprueft. Wo die gemalte Tuer
DIESELBE ist (gleiche Felder, Fenster, Gitter, Material, Farbe), laeuft beim Durchgehen jetzt die
RE2-Sequenz dieses Archivs — mit dem Ton genau dieses Archivs (35 verschiedene Tonteile in RE2;
der Schliesston klingt wie in RE2 ueber den Raumwechsel weiter). Abgedeckt sind 184 Tuerseiten, u. a.
die Fenstertueren mit Querschild (DOOR13), die Asservaten-Schiebetueren (DOOR19), die X-Tore mit
Warnstreifen in der Fabrik (DOOR15), die Zellentuer (DOOR0A), die Leitern (DOOR16, hinauf und
hinab), die Labortueren (DOOR25/DOOR29), die TYPE-P-Schotts (DOOR26; innen am Kuehlraum das vereiste
DOOR31) und die Hubbuehne (DOOR2D). Ablauf wie in RE2: das Bild dunkelt ab, die Tuer oeffnet
sich, die Kamera faehrt hindurch, dann blendet der neue Raum ein (gut 5 Sekunden je Tuer).
* **Griffe:** der Griff steht in der Animation auf der Seite, auf der er im RE1.5-Bild gemalt ist
  (die Variante wird je Tuerseite danach gewaehlt — RE2 macht es genauso, 24 von 25 geprueften
  RE2-Tueren). Wo die Griff-FORM nicht stimmte, ist der Griff getauscht: ROOM10D0 <-> ROOM10F0
  bekommt den Druecker aus DOOR07 statt des Knaufs, die beiden Kassettentueren ROOM1120/1130/1150
  die Messingstange aus DOOR04.
* **Nicht abgedeckt: 61 Tueren.** 45 davon sind nur AEHNLICH — vor allem die glatten, dunklen
  Stahltueren mit Druecker im Revier (12 Stueck): RE2 hat keine Tuer ohne Lueftung oder Fenster,
  am naechsten kommt DOOR07 mit Lueftungsschlitzen. Weiter: 5 Aufzugseinstiege, 4 im Spiel
  nicht aktive oder unsichtbare Uebergaenge, 3 Durchgaenge ohne Tuer (Schacht, Lueftung),
  3 Tueren, die RE1.5 auf beiden Seiten verschieden malt (dort laeuft die Animation nur von der
  gleichen Seite), und das Tor ROOM1170 (hat seine eigene Sequenz). Diese Tueren wechseln wie
  bisher mit der RE1.5-Blende.
* **ROOM4030 -> ROOM4040 / ROOM4080 funktioniert wieder.** Die zwei schraegen Tueren dort sind
  Vier-Ecken-Saetze (40 Byte); der Port las sie als gewoehnliche Tuer und schickte Sie in einen
  Raum, den es nicht gibt. Jetzt wie im Original gelesen (Trefftest FUN_80014368, Nutzlast
  @0x80042f90).

**Hebetisch in Irons' Buero.** Die Granate liegt LINKS im Fach, die Sicherung RECHTS — im Bild
gemessen, in jedem Bild der Fahrt. Die Sicherung liegt schraeg und reicht zur Haelfte unter den
rechten Deckel: die Oeffnung ist nur 300 breit, anders passen die 406 lange Sicherung und die
Granate nicht nebeneinander. Der Aufnahme-Dialog kommt erst, wenn der Tisch wirklich OBEN RUHT
(nach dem letzten Nachsetzen, Skript `Sleep 30` @0x101A) — vorher kam er mitten im Hochfahren.
Erst die Sicherung, dann die Granate; waehrend der Dialoge steht der Tisch (das Original haelt
die Szene bei einer Aufnahme an, @0x8001db98). "No" bietet den Gegenstand bei der naechsten
Fahrt wieder an.

**Generator ROOM11F0.** Die Abnahme kam bisher schon, als der Zeiger erst bei 62 stand. Jetzt
wie in RE2 (ROOM2130): der Zeiger faehrt auf 80, STEHT dort eine Sekunde (30 Bilder,
@0x0171C), erst dann "Power supply OK.", der Bestaetigungston und danach das Licht. Waehrend der
Zeiger faehrt, nimmt das Bedienfeld wie in RE2 keine Eingabe an (nach jedem Schalter).
Elzas Raum ROOM11F1 hat jetzt ebenfalls den Zeiger.

## Grenzen

* Die Animation gibt es auf PC, Steam Deck und Android; die PSX-Fassung wechselt weiter mit der Blende.
* Tueren, die ein Skript waehrend einer Zwischensequenz ausloest, bekommen keine Animation
  (Port-Entscheidung, sonst liefe sie mitten in der Szene).
* 4 der 184 gebauten Seiten sind im Spiel nie begehbar (ihre Saetze sind im Original abgeschaltet);
  die Tueren selbst sind ueber die Gegenseite abgedeckt.
* Der getauschte Druecker an ROOM10D0 ist im ersten Bild schmal (so zeigt ihn auch RE2 an DOOR07).
* DOOR29 zeigt beim Aufziehen (Variante 1) das Schild von hinten gespiegelt — so steht es in den
  RE2-Daten.
* Die Toene sind nach Stimme, Tonhoehe und Bild gemessen, nicht angehoert (kein Audiogeraet an
  der Bau-Maschine).

## Technik

* Die 24 benoetigten RE2-Tuerarchive liegen unveraendert unter `shared_assets/RE2/DOOR/`
  (bytegleich mit der RE2-Disc); das Paket prueft sie.
* `local_build.sh` beendet vor dem Link nur noch die exe des eigenen Bauverzeichnisses (vorher
  jede `re15_pc.exe` der Maschine).
* Belege: `analysis/befunde_runde31/` (Zuordnung und Vergleichsboegen `tueren_03_zuordnung.md`,
  `tueren_belege/t3_vergleich_*.jpg`).

# v0.8.16 - 2026-09-28

Alle neun Befunde Ihrer letzten Runde sind gebaut, dazu vier Fehler, die erst beim Bauen
aufgefallen sind. Jede Aenderung wurde von einem zweiten, unabhaengigen Agenten gegengeprueft,
der gezielt versucht hat, sie zu widerlegen. Dazu kommt aus einer zweiten, parallelen
Arbeitssitzung die Tuersequenz des Tors am Landeplatz (eigener Abschnitt unten). Suite 360 -> 405/405.

## Was Sie merken

**Sie sterben wieder am Hund.** Der Kehlbiss hat Sie wiederbelebt, sobald waehrend des
Bisses IRGENDEINE Richtungs- oder Aktionstaste fiel — deshalb standen Sie bei 0 Lebenspunkten
wieder auf. In ROOM11D0 mit 20 Lebenspunkten gemessen: vorher 15 von 15 Kehlbissen mit
Tastendruck auferstanden, jetzt 0 von 48, Game Over 48 von 48. Der Port hatte im Original
ein Tastensignal gelesen, das in Wahrheit die Figuren-Nummer des Spielers ist
(`lbu v0,8(s3)` / `andi 0x1` @0x80102010). RE2 entscheidet allein im Schadenseintritt, ob
Sie ueberleben. Kein neuer Fehler: der Code stammt vom ersten Hunde-Port am 16. August; seit
v0.8.14 treffen die Hunde nur zuverlaessiger, deshalb kamen Sie oefter in den Kehlbiss.
Dabei mitbehoben: wer beim Tod gerade zielt, bleibt nicht mehr aufrecht in der Zielpose
stehen (cmd 3 @0x80012ef0).

**Die Karte.** Alle vier Befunde hatten eine eigene Ursache:
* *ROOF, Wand unten blau* und *ROOM1000 blau* — eine fest eingetragene Farbe (16,64,176),
  RE2s Besucht-Blau, war beim Zurueckstellen auf RE1.5-Gruen stehen geblieben. Sie kommt
  jetzt aus der Palette (TEX.TIM @0x0556 / @0x055C). Blau-Zensus ueber alle 13 Blaetter:
  vorher 914 Punkte, jetzt 0. Die Dach-Wandzeile lag zudem auf der gemalten Suedwand und ist
  gestrichen.
* *2F, Tuer die es nicht gibt* — die Marke gehoerte zur Tuer ROOM1090 -> ROOM1100
  (ROOM1090.RDT @0x0213A) und trug eine falsche Rueckfallnummer. Sie sitzt jetzt an der Wand
  zwischen beiden Raeumen; neun weitere Marken ohne Traeger sind gestrichen.
* *Verlust beim Laden* — die Etagen-Angaben wurden nie gespeichert. Bei Ihnen fehlten dadurch
  ROOM1060, ROOM1090, ROOM10A0 und die obere Haelfte von ROOM1170. Rundlauf Speichern/Laden:
  vorher 91 von 102 Orten verlustfrei, jetzt 102 von 102. Ihre vorhandenen Spielstaende
  werden beim Laden gehoben; besuchte Raeume erscheinen dabei auf ihrer Haupt-Etage
  (Port-Entscheidung, weil der alte Stand die Etage nicht kennt). Mit Ihrer Karte vom 27.09.
  nachgeprueft: alle vier Raeume sind wieder da.

**Die Karte oeffnet sich nach Irons.** Wie in RE2 ROOM3010 (Opcode 0x84 @0x800591C4): nach
der Szene blendet die Karte "POLICE STATION 2F" auf, der COMMUNIC. ROOM (ROOM10F0 — nicht
ROOM10D0, das ist der Flur davor) blinkt rot/Umriss alle 0,65 s, mit RE2s Hinweiston alle
1,30 s (gemessen 652 ms / 1306 ms). START schliesst. **RE2 merkt sich danach nichts** — kein
Flag, keine Marke, nichts im Spielstand (0 Bit-Schreiber im ganzen Hinweis-Code); die normale
Karte zeigt den Raum danach nach ihrer gewoehnlichen Regel. So ist es jetzt auch hier. Der Raum
selbst ist zunaechst verschlossen und braucht die Blue Keycard.

**Irons Diary liegt auf seinem Tisch** — an Ihrer roten Marke, genau auf dem gemalten
Klemmbrett. Das Weltmodell ist RE2s Buch (room10E0), die Seiten stehen auf FILE08-Papier:
Titel "IRONS DIARY" und 15 Textseiten mit Ihrem ENGLISCHEN Text, jedes Datum beginnt eine
neue Seite, Wort fuer Wort (408 von 408 Woertern aus den fertigen Seiten zurueckgelesen). Aufheben oeffnet wie in RE2 sofort den Leser,
blaettern geht wie in RE2, nach dem Schliessen kommt "The Irons Diary has been filed." und
das Buch verschwindet vom Tisch. Die Toene sind RE2s (Bank 4, Saetze 4/5/6/8; CORE00 ist in
beiden Spielen bytegleich).
* Die 21 vorinstallierten FILE-Eintraege ("Albert Wesker", "Umbrella File 9" ...) sind weg.
  Die Liste beginnt leer und wird im Spielstand mitgespeichert.
* Jedes Zeichen stammt aus den Glyphen der RE2-Originalseiten; konstruierte Zeichen gibt es
  im englischen Text keine mehr.
* Drei Seiten tragen nur einen Zeilenrest ("earth is going on?", "can.", "hope you make it
  out alive!") — Folge des RE2-Seitenrasters mit 9 Zeilen, in dem jedes Datum neu beginnt.
* Die Blaetterpfeile stehen an RE2s Stelle und in RE2s Gruen; an RE1.5s Stelle haette der
  linke Pfeil den ersten Buchstaben mancher Zeilen verdeckt.

**Die Memory Card liegt daneben** (Ihre blaue Marke). Es ist Item 0x21 — nicht 0x20, das ist
die Incendiary Capsule. RE1.5 hat dafuer Bild und Icon, aber nie eine Platzierung und kein
Weltmodell; das Modell ist die Keycard des Spiels mit einer Textur aus dem Item-Bild. Menge 3,
wie RE2s Farbband (21 von 21 Platzierungen).
* In der Nahaufnahme (Cut 6) waren beide fast schwarz, weil dort ein sehr dunkles Licht gilt.
  Sie nehmen jetzt den Lichtsatz von Cut 2 (ROOM1150.RDT @0x003E8) — der hellste der neun
  vorhandenen, messbar der beste; Port-Wahl.

**Tueren klingen verschlossen.** RE2 hat genau EINEN Verschlossen-Satz (Se_on Bank 2 /
Satz 0x16, @0x80051610), dessen Welle je Raum wechselt; RE1.5 ist an verschlossenen Tueren
stumm. Eingebaut an 54 Stellen: Kartenleser, Pincode und elektronisch verschlossen klingen
nach RE2s Kartenleser-Tuer, mechanisch verschlossen nach RE2s Revier-Tuer, die Tuer ohne
Strom in ROOM5080 nach RE2s Gegenstueck in ROOM7020.
* ⛔ **ROOM2190** hat in RE2 (Leon) weder Kartenleser noch Pincode-Tuer und keinen
  Verschlossen-Ton. Die Kartenleser-Tuer liegt in ROOM2110 — deren Ton ist es geworden.
* Stumm bleiben, wie in RE2: falscher Code am Ziffernfeld, Lesegeraet ohne Karte, reine
  "The door won't open!"-Stellen.
* Zum Vergleichen liegen die Wellen in `analysis/befunde_runde30/tuer-verschlossen_wav/`.
  Die Zuordnung steht an EINER Stelle im Code und laesst sich tauschen.

**Das Titelmenue pulst im Original-Takt.** Die Pulsfolge war richtig, der Takt nicht: der Port
schaltete je Bild IHRES MONITORS weiter, das Original je zwei Bildwechsel der Konsole. An einem
144-Hz-Monitor gemessen: vorher 416 ms, jetzt 2006 ms je Periode, wie das Original (120
VBlanks). Unabhaengig von der Bildrate des Monitors. Die Einblende des Titels dauert jetzt
ebenfalls die Original-Zeit (1,07 s statt 0,46 s), und die Helligkeit der gewaehlten Zeile ist
pixelgleich mit dem Original-Bildpuffer.

**Android: R1 rastet ein.** Einmal tippen hebt die Waffe, sie bleibt oben, Viereck feuert,
nochmal tippen senkt. Der R1-Knopf zeigt den eingerasteten Zustand mit einem zweiten Rahmen.
In Menue, Kiste, Text, Zwischensequenz, Raumwechsel und Tod faellt die Raste von selbst. Nur
auf dem Touch-Overlay — Gamepad und Tastatur bleiben Halte-Tasten.

**Elza.** Alle drei Befunde hatten eine eigene Ursache im Port, keine in den Daten:
* Das kurze Lobby-Bild im Intro und das Wiederholen nach dem Abbruch: ROOM1031 hielt sich
  fuer unbesucht, weil das Vorspann-Flag (3,193) nur fuer Leon gesetzt wurde. Es ist die
  gemeinsame Weiche beider Startraeume (ROOM1031 main00 @0x0204E).
* Leon statt Elza in der Lobby: der Modell-Index wurde beim Start von der SCD-Initialisierung
  wieder genullt; der erste Raumwechsel lud PL00.
* Ihre Szene spielte nicht: die Selbst-Tuer-Pruefung kannte die Elza-Variante der Raum-Id
  nicht (0x1030 != 0x1031). Das Original laedt bei JEDER Tuer neu (@0x8001d988).
Leon ist bitgleich geblieben (15 Bilder, 0 abweichende Pixel).

**Die Sicherung im Hebetisch ist sichtbar — und es ist dieselbe.**
* Nach dem Laden eines Spielstands wurde sie gar nicht angelegt; sie lag ausserhalb des
  Fachs, ein Viertel jeder Modellflaeche blieb offen, und das Licht fiel von innen. Jetzt
  liegt sie im Fach: 348 bis 439 Punkte in jedem Bild der Fahrt, vorher 12 bis 124 bzw. 0.
* Das Item-Bild gehoerte zu einem ANDEREN Gegenstand — bytegleich RE2s "Fuse Case". Bild und
  Icon kommen jetzt aus dem Modell, und das CHECK-Foto, das bisher leer war, zeigt die
  Sicherung.
* Sagen Sie "No", bietet die naechste Fahrt des Tisches sie wieder an (wie im Original, das
  eine abgelehnte Aufnahme scharf laesst, @0x8001e068-0x8001e0ec).

**Neben der Sicherung liegt eine Handgranate** und faehrt mit hoch (Item 0x09 "Hand Grenade",
die einzige Granate mit Wurf, @0x8003368c). Nach dem Modal der Sicherung kommt ihres; jede hat
ihr eigenes Flag, "No" bei der einen laesst die andere unberuehrt, und auch nach dem Laden
eines Spielstands ist sie da. Ein liegendes Granatenmodell gibt es in RE1.5 nicht — das Modell
ist die Granate aus Leons Hand-Netz dieser Waffe (PL00W09.PLW), mit den Original-Farben.
Menge 1 (Port-Wahl: RE1.5 platziert die Granate nirgends).
* ⛔ **Werfen geht noch nicht richtig:** Ausruesten, Wurfanimation und Abzug laufen, aber Flug,
  Abprall und Explosion fehlen im Port (drei Effekt-Routinen des Originals, @0x8001843c /
  @0x80018320 / @0x8001854c, wurden nie nachgebaut); der Schaden faellt bisher schon beim
  Abziehen. Das ist der erste Punkt der naechsten Runde.

**Kein Ein-Bild-Blitz mehr beim Kamerawechsel.** Am Telefon in Irons' Office erschienen Buch
und Karte fuer ein Bild gross und dunkel; derselbe Fehler liess vorher das Spielermodell an
manchen Kamerawechseln ein Bild lang falsch stehen. Projektion und Hintergrund wechseln jetzt
im selben Bild.

**Birkin wartet wieder, bis das Skript ihn freigibt.** Beim Bauen der Tuer-Toene blieb Leon
in ROOM5080 nach der Generator-Folge haengen. Die Ursache war nicht die Folge: Birkin lief ab
dem Betreten des Raums los, ging durch die Nordwand und biss den stehenden Leon. Im Original
ist Birkin bis zur Freigabe EINGEFROREN — seine Wurzel ueberspringt bei gesetztem Bit 0x20 die
ganze KI (STAGE5 @0x80116a88, STAGE3 @0x80116274), und beim Erscheinen laeuft sie genau einmal,
damit er in seiner Ausgangspose steht (@0x8004256c-0x80042608). Das fehlte im Port. Betroffen
waren 9 von 13 Birkin-Auftritten (ROOM3070, 3071, 3080, 5080, 5081, 50E0, 50F1 und die
Endkampf-Raeume). Je Raum gemessen: vorher lief er in 299 von 300 Bildern vor der Freigabe,
jetzt in 0; die Freigabe wird in allen neun Raeumen auf dem Weg eines Spielers erreicht, und
danach kommt sein Auftritt (EMERGENCE) wieder, der vorher uebersprungen wurde. Der Endkampf in
ROOM5090 ist byte-gleich zu vorher (13 Birkin-/G5-Sonden unveraendert).

## Warum der Linux-Bau ueber eine Stunde dauerte

Sie hatten Recht, das war nicht normal. Der Container las jede Datei ueber die
Windows-Freigabe (WSL2, 9p-Mount), und dort kostet JEDE Dateioperation rund 10 ms: 5000-mal
stat 54 s statt 3 s, die Spieldaten lesen 142 s statt 0,17 s. Configure, Uebersetzen und die
Tests bestehen fast nur aus solchen Zugriffen. Jetzt wird der Quellbaum zu Beginn in den
Container kopiert und dort gebaut; das Repo haengt nur noch als Rueckfall darunter, sodass
kein Test still eine Datei verliert (eigener Test-Fingerabdruck als Gate). Die Bau-Werkzeuge
liegen in einem vorgebauten Image, statt bei jedem Lauf neu installiert zu werden.

| | vorher | jetzt |
|---|---|---|
| Linux/Steam-Deck-Bau mit voller Testsuite | 82 min | gut 9 min |
| Windows-Bau | 4,4 min | 1,3-2 min |

Die Programme sind bis auf Bau-Kennung und Zeitstempel byte-gleich. Dieses Paket ist bereits
auf dem neuen Weg gebaut, die Linux-Suite im Container: 405 von 405.

## Das Tor am Landeplatz (ROOM1170) - Tuersequenz nach RE2

Das Gelaendertor am Hubschrauberlandeplatz (ROOM1170) oeffnet sich jetzt wie eine Tuer in
Resident Evil 2: mit eigener Tuerszene, Bewegung und dem Ton des RE2-Gittertors. Grundlage
ist das Tuermodell, das ich Ihnen aus Ihren drei Ausschnitten gebaut habe.

### Was Sie merken

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

### Wie es gebaut ist

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

### Was bewusst offen blieb

- **Nur dieses Tor** hat eine Sequenz. Die RE1.5-Raumdaten sagen fuer keine Tuer, welches
  Modell sie haette; jede weitere Tuer braucht ein eigenes Modell oder ein passendes
  RE2-Archiv und einen Eintrag in der Zuordnung.
- **PSX-Stand:** die Szene laeuft auf PC, Steam Deck und Android; der PSX-Build geht ohne
  Sequenz durch das Tor wie bisher.

## Wo ich mich korrigieren muss

**1. Die Sicherung aus v0.8.15 war schlechter gebaut, als ich gemeldet hatte.** Viereck-
Reihenfolge und Normalen waren falsch, der Sitz lag hinter der Kuppel, und am Lade-Weg fehlte
sie ganz. Gemeldet hatte ich "Suite gruen" — geprueft hatte ich das Bild nicht.

**2. Meine Vorgaben an die Ermittlung enthielten drei falsche Annahmen:** die Memory Card sei
Item 0x20 (es ist 0x21), der Communication Room sei ROOM10D0 (es ist ROOM10F0), und das Blau
der Karte komme aus den Palettenindizes 12/13/14 (widerlegt: 0 Texel in allen Raum-Rechtecken).
Die Agenten haben alle drei an den Daten widerlegt.

## Was bewusst offen blieb

* ⛔ **Der Wurf der Handgranate** (Flug, Abprall, Explosion) fehlt, siehe oben.
* ⛔ **ROOM3071 ist fuer Elza ein Softlock** — gefunden bei der Birkin-Messung, schon vor
  dieser Runde vorhanden. Nach der Freigabe wartet das Skript auf zwei Flags (Ck(5,31) @0x0364D,
  danach Ck(5,30)), die nur Birkins Sturmangriff setzt (@0x80119658 / @0x801197b4). Der Port
  hat diesen Angriff ohne die Flag-Setzer gebaut; Leon bzw. Elza bleibt gesperrt. Leons
  ROOM3070 ist nicht betroffen. Das ist der erste Punkt der naechsten Runde.
* Birkins Griff haelt Sie zu lange fest: ohne Eingabe 142 Bilder, im Original 19 plus die
  Clip-Phasen (Opfer-FSM @0x8011afd8). Schon vorher so, jetzt gemessen.
* In ROOM3080 fehlt Birkins Szenenmodus (grid 4); die Folge endet trotzdem ueber die Tuer.
* Das CHECK-Foto bleibt bei 18 weiteren Gegenstaenden leer, deren Bild im RE2-Format vorliegt
  (u.a. alle Kraeuter-Mischungen 0x24-0x2E). Behoben ist es in dieser Runde nur fuer die
  Sicherung.
* Elzas Lobby-Szene: der Erzaehler tippt sich dort als Schreibmaschine auf, bei Leon steht er
  als Volltext; die Zombies hinter dem Gitter wirken orange. Beides nicht belegt, nicht angefasst.
* Die Bildwiederholung des Originals im Bild eines Kamerawechsels (@0x80021560) ist nicht
  nachgebaut; es bleibt ein Bild Figurbewegung (Port-Wahl).
* Die PSX-Plattform kennt die neuen Toene, das Diary und die Tisch-Gegenstaende nicht.

## Woran ich als naechstes sitze

**Den Granatenwurf** nachbauen und **ROOM3071** (Elzas Birkin-Kampf) spielbar machen, dann Birkins Griff (Opfer-FSM) und danach
Elzas Raumkette weiter nachfahren.

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
