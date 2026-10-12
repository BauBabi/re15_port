# Runde 37 — Auftrag des Nutzers (2026-10-12, Antwort auf die Abschlussmeldung von v0.8.24)

## Wortlaut (unveraendert)

> Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker. Die Marker Datei liegt da wo immer die DAteien liegen mit F9.  Bei Leon ist die blutlache unpassend wenn roy noch nicht tot ist....
>
> Werkstatt abschluss ist perfekt.
>
> Elza bekommt die Info über den prison key später über roys Tagebuch, das passt schon.
>
> Ja, neuen Texte sind super. Bezug zu ada bei elza kann raus.
>
> Die Fahrstuhl Tür in der Tür Animation ist noch nicht korrekt. DA musst du noch mal schauen ... wenn es keine passende Fahrstuhl Tür gibt, müssen wir die wieder bauen....
>
> Beim Aussteigen aus dem Fahrstuhl fehlt noch die Türsequenz. << stimmt.
>
> Items auf erhöhten Flächen lassen sich vom falschen Stockwerk aufnehmen, z.B. die Super Redhawk in ROOM3050 ohne die Kiste. Das war schon in v0.8.23 so. << muss gefixed werden.
>
> Außerdem hätte ich gerne noch:
> bei den Code panels möchte ich das, so lange die Code noch falsch eingegeben ist unten das blaue Licht leuchtet. Ist er dann richtig eingegeben das grüne Licht. Die Kamera darf noch auf den Panels bleiben wenn der Piep und unlock sound kommt.
>
> Und - bei Leon, bevor dem Ende wo er noch zu Irons muss habe ich eine Lücke gefunden - man kann über die Luke bis nach F1 und kann den "Ich muss zuerst irons holen" Blocker bei der Tür überspringen. Ich schlage vor - du machst den Text zusätzlich noch in ROOM 1050 bei der Tür, wo wir auch die Ada Cutscene triggern, wenn wir Ada noch nicht gerettet haben.

## Lesart je Satz (Orchestrator)

| # | Wortlaut (kurz) | Lesart | Spur |
|---|---|---|---|
| 1 | ROOM1100 Problem immer noch da, Zombie auf dem F9-Marker | PRI/Verdeckung in ROOM1100 Cut 1 noch falsch. Marker = Hauptbaum `re15_port/build/platform/pc/befund_1100_F892_marke1.bmp` (2026-10-12 10:07; befund.log MARKE 1 Zeilen ~15850ff: Leon (-14914,0,-25645) rot 4488, Zombie 2 liegend (-14269,0,-25091), 105 Masken des Winkels mit Tiefen) und `befund_1100_F240_marke1.bmp` (2026-10-05: nur der Kopf eines liegenden Zombies ueber der rechten Wand sichtbar). ⛔ Spur O (Runde 36) hatte die Datei nicht gefunden, weil sie nicht im exe-Ordner gesucht hat. | A pri1100 |
| 2 | Blutlache bei Leon unpassend, solange Roy nicht tot ist | Story-Vorschlag R2 (Blutlache an Roys Platz in 1010) wird NICHT gebaut. | — (abgelehnt) |
| 3 | Werkstatt-Abschluss perfekt | 11B0: Marvin schliesst nur die Werkstatt-Tuer ab — bleibt. | — |
| 4 | Elza: Prison-Key-Info spaeter ueber Roys Tagebuch | kein Hinweis-Text noetig (Nutzer schreibt das Tagebuch). | — |
| 5 | Neue Texte super; Ada-Bezug bei Elza kann raus | Texte der Runde 36 freigegeben. ROOM50B1 msg 0 (Elza) "It's Ada. She's dead.It looks like one of those / gorillas killed her." ohne Ada-Bezug umschreiben. | F |
| 6 | Fahrstuhl-Tuer in der Tuer-Animation noch nicht korrekt; sonst neu bauen | ROOM1080: DOOR27 (RE2-Labor-Fahrstuhl) passt nicht. In RE2 eine passende Fahrstuhl-Tuer suchen; gibt es keine, eine eigene bauen (Port-Archiv wie Runde 33/36, Optik des gemalten RE1.5-Fahrstuhls, Mechanik eines Fahrstuhls). | C fahrstuhl |
| 7 | Beim Aussteigen fehlt die Tuersequenz — stimmt | Ausstieg aus der Kabine (ROOM1080/4020 -> Etage) bekommt eine Tuersequenz (RE2: ROOM6010 spielt DOOR27 V0) — mit der Tuer aus #6. | C fahrstuhl |
| 8 | Items auf erhoehten Flaechen vom falschen Stockwerk aufnehmbar — muss gefixed werden | NACHLAUF Runde 36 #73/#74: Band-Gate fuer Item-AOTs (@0x80042cac-ccc) + Member_set 0x12 setzt das Kollisionsband (@0x80041228) — nur gemeinsam. | D itemband |
| 9 | Code-Panels: blaues Licht unten solange falsch, gruen wenn richtig; Kamera bleibt beim Piep + Unlock auf dem Panel | NEU (Port-Wunsch, RE2 als Vorbild pruefen). | E codepanel |
| 10 | Leon vor dem Ende (zu Irons): Luecke ueber die Luke nach F1 umgeht die "Chief first"-Sperre; Text zusaetzlich an der ROOM1050-Tuer, wo die Ada-Szene triggert | Sperre "I have to get the Chief first..." (re15_tuer1060.h, Bedingung (9,73)=0 und ((9,71)=1 oder (3,94)=0)) zusaetzlich an ROOM1050 Slot 4 (-> 10A0, re15_adaruf.h), solange Ada noch nicht gerettet ist bleibt die Ada-Logik. NUTZER-VORGABE. | F sperre1050 |

Nicht beantwortete Fragen aus v0.8.24 ("Zu entscheiden": Tuerknall 106/205, Groesse Parking Lot Key, Tischmasken 10F0,
Lageplaene 1140/Groesse/1030, Treppenhaus-Blaettern, weitere Tueren, ROOM4000-Tonfolge, Story-Vorschlag V6 Montage-Folgen
bei Elza, Karte 1090/1190, Untertitel main16.5) bleiben wie gebaut; V6 wird nicht gebaut, solange der Nutzer nicht zustimmt.

## Nachtrag 2026-10-12 (waehrend des Aufbaus der Runde, woertlich)

> Weitere findings - bei der Beretta M93R muss - wenn sie schon mehrfach schießt, das auch gelten das die Gegner mehrfach getroffen werden. Werden sie nur ein einziges mal getroffen bei 3 Schuss, ist es nur Schussverschwendung.

Lesart: Der 3-Schuss-Feuerstoss der Beretta M93R (Waffe 5; Glock 18 = Waffe 6 hat denselben Burst-Handler 0x800338A8)
verbraucht 3 Schuss, trifft den Gegner aber nur einmal. Jeder Schuss des Feuerstosses soll treffen (Schaden + Treffer-
reaktion/Blut je Schuss). Zuerst messen, dann am Original belegen, ob RE1.5 je Schuss trifft (dann Port = Original
nachziehen) oder nur einmal (dann NUTZER-VORGABE). -> Spur G beretta.

## Bestaetigung 2026-10-12 (woertlich)

> ja, perfekt.

Antwort auf die Zusammenfassung der Lesart (Spuren A-G wie oben; Sperre ROOM1050: vor Adas Rettung bleibt die Ada-Szene,
danach der Chief-Text nur in der Endphase — nach der 10F0-Szene und bevor Irons geholt ist; Fruehspiel ungesperrt;
Werkstatt bleibt, neue Texte uebernommen, keine Blutlache, kein Elza-Hinweis, V6 nur auf ausdruecklichen Wunsch).

## Nachtrag 2 — 2026-10-12 (woertlich)

> Ein weiteres Problem das ich immernoch habe - die Zombies sind immer noch zu passiv. Beispielsweise, am Ende von STAGE1 Leon, wenn wir die ganzen Zombies spawnen nach Irons tod - die Zombies bleiben einfach nur Stumpf alle im Gang stehen. So sind sie keine Bedrohung .... In Resident Evil 2 bewegen sie sich doch auch teilweise aktiv im Gang.

Lesart: Die Zombies der Montage nach Irons' Tod (re15_irons_tod.h: 1130 / 1040 / 1030, Spawn-Bits (9,74)/(9,75)/(9,76), dazu
1070) stehen danach nur im Gang. Ziel (Beta -> Retail): Verhalten wie RE2 — Zombies bewegen sich teilweise aktiv im Gang auf
Leon zu. Runde 36 Spur F Punkt 1 (Sicht ueber niedrige Moebel) galt als erfuellt; hier gezielt die Montage-Zombies und das
RE2-Gangverhalten messen. -> Spur H zombieaktiv.

Ergaenzung des Nutzers zu Nachtrag 2 (woertlich, unmittelbar danach):

> das soll nicht nur da gelten, das darf generell für die Zombies gelten

> schaue einfach nach wie Resident Evil 2 das macht

Lesart (ersetzt die enge Lesart oben): Das RE2-Verhalten gilt GENERELL fuer alle Zombies (jeder Raum, jede Spawn-Art), die
Montage nach Irons' Tod ist nur das Beispiel. Vorlage ist die RE2-Retail-Zombie-KI (wie sich Zombies dort im Gang bewegen,
wann sie loslaufen, wandern, auf Leon zukommen) — aus RE2 belegt nachbauen.

## Nachtrag 3 — 2026-10-12 (woertlich, Korrektur zu #9 Code-Panels)

> Nein, beim Panel in korridor 2 ist falsch - dort soll natürlich rot leuchten - richtig eingegeben - dann blau....

> rot = geschlossen

Lesart: Die Lampen richten sich nach den am Panel gemalten Lampen. Panel in "Korridor 2" (laut Original-Raumnamen ROOM10D0
"2F CORRIDOR" oder ROOM11E0 "B2 CORRIDOR" — Spur E bestimmt es an den gemalten Lampen): ROT solange zu (Code falsch/nicht
eingegeben), BLAU sobald richtig. Regel: rot = geschlossen. Die uebrigen Panels: blau solange falsch, gruen wenn richtig
(soweit so gemalt). Dem laufenden Bau-Agenten der Spur E am 2026-10-12 per Nachricht uebermittelt (mit der Bitte, den
Nachtrag in AUFTRAG.md seines Baums zu committen).

## Nachtrag 3b — 2026-10-12 (woertlich, zu den Code-Panels)

> Es muss schon in der richtigen Farbe leuchten das Panel... hintergrund rot - rot leuchten - hintergrund blau - blau leuchten...

Lesart: Die Leuchtfarbe ist die Farbe der im Panel-Hintergrund gemalten Lampe (rot gemalt -> leuchtet rot, blau gemalt ->
leuchtet blau), keine fremde Einheitsfarbe. Dem Bau-Agenten E per Nachricht uebermittelt.

## Nachtrag 4 — 2026-10-12 (woertlich)

> Bitte erstelle mir zusätzlich auch ein Modell des Feuerlöschers. Ich finde deine Idee, bei elza einen Leeren Feuerlöscher am Boden zu haben gut. Der müsste dann bei Leon nach der Nutzung erstmalig auftauchen.
>
> Außerdem finde ich den FEuerlöscher im Spind bei Leon noch unglücklich und würde ihn gerne woanders platzieren....

Lesart -> Spur I feuerloescher:
1. 3D-Modell des Feuerloeschers BAUEN (weder RE2 noch RE1.5 haben eines — extracted_re2_feuerloescher/README.md: nur das
   Item-Bild ITPS 0x31; Basis-Kandidaten = Spraydosen-Netze RE1.5 ROOM1010/ROOM4050, RE2 ROOM1010/60D0).
2. Elza ROOM1091 (ausgebrannter Schuppen): leerer Feuerloescher liegt von Anfang an am Boden (die frueher vorgeschlagene Spur).
3. Leon ROOM1090: derselbe leere Loescher liegt erstmalig dort, NACHDEM Leon das Feuer geloescht hat ((3,0x84) := 1 in sub06
   @0x02722 / "You've used the Fire Extinguisher."), danach dauerhaft.
4. Leons Feuerloescher liegt heute im Spind von ROOM1000 (Bath-Lockers; sub00 @0x00C24 Item 0x31, Taken (9,0x86); sub01
   @0x00D0C Set(3,0x85)) — an einen anderen, passenden Ort verlegen (sichtbar mit dem neuen Modell; Ort begruendet waehlen,
   der Nutzer kann ihn danach verschieben).

## Nachtrag 4b — 2026-10-12 (woertlich, zu Spur I)

> ah, it can only lay there - if leon when he is asked to discard it select "yes".

Lesart: Bei Leon liegt der leere Loescher in ROOM1090 NUR, wenn er an der Wegwerf-Abfrage nach dem Loeschen (Port-Abfrage
gen/discard_sites.inc `{ 0x1090, 9, 0x31, 0, 0 }`) "Yes" waehlt; bei "No" bleibt er im Inventar und am Boden liegt nichts.
Elza 1091 unveraendert (liegt immer). Dem Bau-Agenten I per Nachricht uebermittelt.

## Nachtrag 4c — 2026-10-12 (woertlich, zu Spur I)

> If he doesn't he has the text empty fire extinguisher in the player inventory.

Lesart: Bei "No" an der Wegwerf-Abfrage behaelt Leon den Loescher; im Inventar heisst er dann "Empty Fire Extinguisher"
(Name/Untersuchen-Text zeigen, dass er leer ist; Zustand bleibt im Spielstand). Dem Bau-Agenten I per Nachricht uebermittelt.

## Nachtrag 5 — 2026-10-12 (woertlich, zu Spur C)

> ja, dann mache den noch. Mir ist auch aufgefallen in den Stage2 kleineres aufzügen fehlt noch das sound. Den sollte man auch aus Resident Evil 2 extrahieren.

Lesart: (a) "den" = der Ausstieg aus dem Lastenaufzug ROOM3080 (WAREHOUSE LIFT) -> ROOM4000, der in Spur C noch ohne
Tuersequenz ist (Null-Rechteck-Tuer @0x0072A, Aot_on @0x00A22) — Sequenz aus RE2 herleiten und bauen. (b) Die kleineren
Aufzuege in STAGE2 haben keinen Ton — den passenden Aufzug-Ton aus RE2 extrahieren (RE2_SOUND_LISTE.md, shared_assets/RE2/
ELEVSE.VBS u.a.) und dort abspielen, wo RE2 ihn spielt.

## Nachtrag 6 — 2026-10-12 (woertlich)

> Ja, wie ich gerade gesehen habe fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom Zombie gebissen wird.

Lesart (beide pruefen, keine F9-Marke dazu): (1) der Arm des Zombies, der beim Biss/Griff an Leon haengt (Griff von vorn/hinten),
fehlt in manchen Faellen; (2) der angeschossene, noch "dranhaengende" (baumelnde) Arm eines Zombies fehlt beim Biss. Messen,
welcher Fall auftritt, dann wie RE2 bauen. -> Spur J bissarm.

## Nachtrag 7 — 2026-10-12 (woertlich)

> Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen. Außerdem wiederholen sich seine Animationen wieder ständig, wie wir schon ungefähr 500x vorher bei Leon gefixed haben.....

Lesart -> Spur K roy: Roy (Typ 0x47, im Original dasselbe Typ-Byte wie Elliot bei Leon; B_figuren.md §1.1) in Elzas Szenario
(ROOM1011 verwundet / bewusstlos, ROOM1021 als Zombie, den John erschiesst): Modell und alle Animationen wie im Original;
keine Dauerschleife der Animationen (dieselbe Fehlerklasse wie die frueheren NPC-Schleifen bei Leon).

## Nachtrag 8 — 2026-10-12 (woertlich, zu #5 / Spur F)

> Elza kann theoretisch ada begegnen in ihren durchlauf. Ich kenne zumindest keine Aussage der Entwickler die sagt, das sie sich nicht begegnen können. Nur das Elza und Leon sich niemals begegnen. Aber wenn Elza nur eine Leiche von Ada findet, kennt sie ada nicht. Dann ist es halt nur - a woman.

Lesart (bestaetigt und praezisiert #5): ROOM50B1 msg 0 (Elza) nennt die Tote nicht "Ada", sondern "a woman" — Elza findet
nur die Leiche und erkennt sie nicht. Story-Regel: NUR Leon und Elza begegnen sich nie; dass Elza und Ada sich begegnen, ist
NICHT ausgeschlossen. Die Begruendung "Elza hat Ada nie gesehen" (Runde 36 P_story.md) ist damit keine Regel: kein Text darf
behaupten, Elza und Ada koennten sich nicht begegnen; dass Elza den NAMEN Ada kennt (Marvin, ROOM30C1 msg 4/6), bleibt.
Beim Sweep "sagt/liest Elza 'Ada'" nur dort aendern, wo Elza Ada am Aussehen erkennen muesste. Der Bau der Spur F traegt
genau diesen Wortlaut ("It's a woman. She's dead." / "It looks like one of those" / "gorillas killed her.", F-Baum 6bf10b9b);
die Abnahme prueft ihn gegen diesen Nachtrag, die Begruendung in Code/Dossier soll den Grund des Nutzers tragen.

## Orchestrator-Auftrag L — 2026-10-12 (zu Nachtrag 2 + Ergaenzung, "schaue einfach nach wie Resident Evil 2 das macht")

Grund: Die Abnahme H#0 (H_abnahme_0.md, Hinweis H1) und das Dossier der Spur H (H_zombieaktiv.md "OFFEN") belegen drei
VORBESTEHENDE Abweichungen der RE2-Zombie-KI des Ports vom RE2-Original (EMZ0.BIN). Der Nutzer verlangt das RE2-Verhalten
GENERELL fuer alle Zombies; diese Abweichungen gehoeren dazu und werden in dieser Runde geschlossen (Spur L re2zombie, Basis =
abgenommener Stand der Spur H 9c2a825d):
1. Die drei Produzenten des Arme-hoch-Gangs `+0x4 = 0x201` (EXEC[2]) im Gang-Executor sind seit 2026-08 abgeschaltet:
   (1) Timer @0x80101D68-DBC, (2) Nahbereich @0x80101EC4-F10, (3) Spieler rennt @0x80101F14-F60. Mit (1)+(2) fiel die
   Positiv-Kontrolle Nahkampf in test_re2_zombie_abc von 3523 auf 0 (Mechanismus nie aufgeklaert). -> aufklaeren und
   byte-true scharf schalten; in RE2 heben die Zombies dabei die Arme und kommen auf Leon zu.
2. DECISION[0] Block 3: Original committet bei (rand & 1) == 1 (`beq v0,zero,0x801013d4` @0x801013c8), der Port bei == 0.
3. EXEC[0] P1: nach dem Wander-Wurf (`sb v0,6(s0)` @0x80101540) laeuft im Original im selben Takt der Selbstwecker
   @0x80101544-a0 und P2 erst im naechsten Takt (`j 0x801016ac` @0x8010159c); der Port springt im selben Takt nach P2/P3.
Dazu ein Zensus der uebrigen OFFEN-Marken der RE2-Zombie-KI, die Bewegung/Angriff betreffen (je Marke: Adresse, wirkt auf die
Aktivitaet ja/nein); was die Aktivitaet betrifft, mitschliessen oder mit Adresse begruenden, warum nicht.

## Nachtrag 9 — 2026-10-12 (woertlich, Item-Kiste)

> In der Kiste müssen die Icons offenbar so weit wie möglich rechts liegen, damit sie nicht vom Text überdeckt werden. Außerdem ist mir aufgefallen, das beim scrollen/bewegen in der Kiste währendessen die Items oberhalb des Kastens liegen. Das darf nicht sein, der Kasten muss layertechnisch über den Items liegen.

Lesart -> Spur M kiste (Item-Box-Bildschirm, RE2-Layout seit 2026-08-30, re15_inv_screen.c Block 7b "DIE BOX ALS LISTE"):
1. In der Box-Liste ueberdecken lange Item-Namen (links ab NAME_X 16) die Item-Symbole (ICON_X 158). Die Symbole sollen so weit
   wie moeglich nach rechts (bis vor die Mengen-Ziffern/den Scrollbalken), so dass kein Name ein Symbol ueberdeckt — Zensus
   aller Item-Namen (Pixelbreite im Port-Zeichensatz) gegen die Symbol-Lage; RE2-Vorlage pruefen (RE2: Symbol x 155 in seinem
   breiteren Panel; RE2 kuerzt/setzt Namen wie?).
2. Beim Scrollen gleiten die Zeilen (box_pixoff) und liegen waehrenddessen UEBER dem Rahmen ("Kasten") des Listen-Panels. Der
   Rahmen muss ueber den Items liegen: Zeilen, die beim Gleiten aus dem Fenster ragen, werden vom Rahmen verdeckt bzw.
   abgeschnitten — wie RE2 (Listen-Zeichenschleife FUN_800710dc, Rahmen-Sprites @0x800a9c38).

## Nachtrag 10 — 2026-10-12 (woertlich, Irons-Szene ROOM1150 bei Leon)

> Der Text bei Leon zum Schluss bei Irons - "be a hero leon" ist mir irgendwie zu trashig.... hast du einen weniger trashigen Text als Abschluss für den Dialog?

Vorschlaege (je mit "Irons: " in einer Zeile <= 271 px, gemessen mit tools/r35_l/texte_bauen.py): "Get them out of here... Leon..."
(empfohlen), "It's up to you now... Leon...", "Now go... Leon... Go...". Antwort des Nutzers (woertlich):

> ja, get them out of here leon gefällt mir besser

Lesart: ROOM1150 msg 28 (Port-Text der Runde 35, irons_tod_1150.c k_msg28) wird "Irons: Get them out of here... Leon..."
(NUTZER-VORGABE). Keine Sprachaufnahme vorhanden (synchro/STAGE1/room1150/main28.wav fehlt) -> nur Text; der Szenen-Takt
haengt nicht an der Textlaenge (99-Bilder-Halt `01 63`, danach OP_SLEEP(100) + Armfall). Umsetzung im Integrationszweig.

## Nachtrag 11 — 2026-10-12 (woertlich, Szene ROOM11C0 Ada/Marvin nach Irons' Tod)

> Und die Dialogszene mit Ada/Marvin nach Irons Tod ist auch noch nicht so geil von den Animationen....
>
> Erst schauen sie komplett in die falsche Richtung. Dann drehen sie sich einmal um 180 grad um. Dann dreht sich Ada noch mal um 90 grad um. Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada.... da musst du noch mal ran, das ist noch nicht so gut.

Lesart -> Spur O szene11c0: Programm 4 der Irons-Szene (engine/src/irons_tod_1150.c k_p_11c0, Runde 35 Spur L + Runde 36
Spur K2) neu choreografieren: (a) zu Beginn stehen beide schon sinnvoll (einander zugewandt bzw. so, dass keine grosse
Drehung noetig ist) — kein "komplett falsche Richtung"; (b) keine 180-Grad-Drehung zum Gebaeude (die Tuer liegt HINTER der
Kamera Cut 13): auf den Knall reagieren Kopf/Oberkoerper, nicht der ganze Koerper um 180 Grad; (c) keine zweite grosse
Drehung Adas; (d) wer spricht, sieht den anderen an — Marvin sieht Ada an, wenn er spricht ("Yes!....", "Oh, no, Leon!",
"I have to help him, sorry!"), bis er losrennt; Ada sieht ihm nach. Zuerst die heutige Szene Bild fuer Bild messen
(Framedumps, Gierwinkel + Kopfdrehung je Bild beider Figuren) und gegen die Beschreibung des Nutzers stellen; pruefen,
ob Plc_neck bei Marvin (Typ 0x40) ueberhaupt sichtbar wirkt. Vokabular wie bisher aus dem Original (Plc_neck Modus 1,
Plc_dest Modus 9, Ankunfts-Warten, ROOM11C0 sub02/sub03) und RE2-Dialogszenen als Vorbild; Winkel aus den Positionen
(Gierwinkel = Richtung zum Gegenueber), Takte PORT-WAHL. Beats der Runde 35/36 (Knall, Frage, Antwort, "Oh, no, Leon!",
"I have to help him", Losrennen, "Marvin!...") bleiben.

## Orchestrator-Auftrag N — 2026-10-12 (zu Nachtrag 7 "Bei Elzas Szenario stimmt weder das Modell von Roy ...")

Grund: Spur K (K_roy.md OFFEN 1, §6.1) und Spur J (Folge-Kandidat) belegen dieselbe Fehlerklasse ueber Roy hinaus: das
Original laedt in Elzas Szenario JEDEN Gegner-/NPC-Typ aus CDEMD1.EMS (FUN_80022300: Elza-Bit `bltz` @0x80022334 -> Datei
0x27 @0x800223ac, TOC @0x80073178), der Port ausser Roy weiter Leons CDEMD0-Daten. Unterschiede je Typ laut K: 0x30 Birkin
ANDERES MODELL (ROOM3071/5081/5091/50F1), 0x42 ANDERES MODELL (ROOM11B1 @0x01058, ROOM3091 @0x014F4), Zombies 0x10-0x18
eigene Aktions- + Opfer-Bank (Griff-Animation am Spieler), Kraehe/0x27/0x29/0x2b Opfer-Bank. -> Spur N elzaems: Elzas
Szenario laedt jeden Typ wie das Original aus CDEMD1 (K-Mechanismus re15_ems_toc / roy_pc.c verallgemeinern), je Typ am
Original und an der exe gemessen; Leon bleibt bytegleich. Basis = abgenommener Stand der Spur K.
