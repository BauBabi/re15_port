# Runde 36 (2026-10-10) — Auftrag des Nutzers, woertlich

Stand vor der Runde: master 3ec5b1f8 (v0.8.23, Suite 592/592). Vorherige Runde: 35 (14 Spuren A-N,
Dossiers `analysis/befunde_runde35/`). Die Story-Analyse Leon/Elza (`analysis/story_leon_elza/`) war am
Wochenlimit abgebrochen (Teilergebnisse liegen dort) und wird hier fortgesetzt.

## Entscheidungen des Nutzers zu den drei Fragen aus v0.8.22/23

> Zu deinen 3 Fragen - die sind allesamt ok.

(1) Leon wird von der eigenen Granate nicht verletzt (RE2-Regel) — bleibt. (2) Granate/Werfer zuenden an
Tischkanten (RE1.5 kennt keine Hindernis-Hoehe) — bleibt, keine Ausnahmen bestellt. (3) ROOM1030: die
Montage bricht das Tor auf, die Original-Szene kommt danach nicht mehr — bleibt.

## Grenzen aus v0.8.22/23, die der Nutzer angehen laesst

> Zu den grenzen: "Alle Schusswaffen treffen über den Konferenztisch und treffen fressende Zombies. Im Original
> geht beides nicht." --> Das solltest du angehen!
> "Der Port verwaltet höchstens 15 Gegner je Raum, das Original 20. In ROOM1040 und 1030 fehlen deshalb die letzten
> fünf Zombies, das war auch vorher schon so." --> Das auch!

## Der Auftrag (woertlich)

> Mehrere Dinge:
> - Bei normalen Granaten brauche ich keinen Feuer Effekt bei den Zombies, sondern nur das Zerplatzen der Körperteile. Für Würde ich nur bei den incendiary Granaten erwarten.
> - Einige Türen in den Türsequenzen sind noch nicht ganz korrekt. So zum Beispiel der Elevator ROOM 1080 und die große Tür in ROOM 2090. Ich glaube beide liegen auch korrekt in Resident Evil 2 vor.
> - Der von dir ergänzte Text in ROOM 1190 bei der Zielscheibe ist falsch. Die Texte: "This target has a surprisingly large number of bullet holes." und "This target does not have many bullet holes" sollen sich hinten bei den Zielscheiben befinden, wenn man sie anklickt, nicht vorne bei den Knöpfen, die die Zielscheiben bewegen lassen... Bei Elza sind es genau die anderen zwei Zielscheiben, deshalb müssen die Texte da genau vertauscht sein.
> - Bei den Debug Menu, bei den man zwischen die Räume springt, schickst du, wenn man mit Elza spielt Elza in Leons Räume. Wenn man mit Elza spielt, sind die Räume mit "1" hinten korrekt, zum Beispiel ROOM 1071.
> - Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards oder Handgranaten 0 sollen sie aus dem Player inventory verschwinden
> - In der Cutscene in ROOM 11B0 mit Leon, Ada und Marvin klappt das mit der Animation und dem gemeinsamen Wegschieben des Containers mit Marvin noch nicht....
> - Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern - z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich
> - Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.
> - Türen die erfolgreich geöffnet wurden mit Karte oder sowas machen in Resident Evil 2 erst ein "piep" Sound und dann ein unlock Geräusch. Bei uns nicht.
> - Bei Elzas Szenario soll die Scheibe in Room 1120 die bei Leons Szenario zum schluss zerspringt in Stage1, bereits immer kaputt sein, und eine Krähe mehr im Raum.
> - Sobald Irons gegen Ende von Stage1 tot ist, soll er auch nicht mehr den Kopf hinter Leon hinterher bewegen
> - Bei Elzas Szenario soll Irons immer bereits von anfang an tot da liegen. Wenn sie ihn anklickt soll da stehen "He has already passed...".
> - In Leons Szenario, in ROOM 11B0 bei der Cutscene wo Ada, Marvin und Leon die quasi STAGE1 verlassen und runter in den sewer gehen möchte ich am Ende der Cutscene noch, die Cutscene kurz erweitern:
> 	- Marvin: "Hold on"
> 	- Marvin rennt zur Tür die zu Parking Lot ROOM 11C0 führt und führt eine zuschließen Animation mit zuschließen Sound aus. (Die könntest du die aus Resident Evil 2 von Robert Kendo holen, am Anfang vom Spiel, da macht er das auch).
> 	- Danach rennt er zur Innenseite der Tür die zur Werkstatt führt in ROOM 11B0 und macht das gleiche noch mal und schließt die Tür dort. Die Tür soll dann geschlossen bleiben mit der gleichen geschlossen Meldung wie in Elzas Szenario.
> 	- Danach rennt Marvin zurück zu Leon und Ada. Und sagt:
> 	- Marvin: I just want to make sure we don't have any unwanted followers.
> 	- Suche dir wieder geeignete Animationen dafür aus.
> 	- Wenn man die Tür anklickt, soll der gleiche Text kommen, wie in Elzas Szenario dann, wo die Tür ja geschlossen ist.
> - Ich möchte bei Leon das die Tür zum Parking Lot ROOM 11C0 verschlossen bleibt, bis ein Parking Lot Schlüssel im Communication ROOM 10F0 gefunden wurde. Wähle dir einen Schlüssel aus den Resident Evil 2 Items aus, der am ehesten den optischen Eindruck eines Parking Lot Schlüssels macht.
> - In ROOM 1211 in Elzas Cutscene wenn sie Kendo befreit fehlt noch der Unlock sound bei der Tür wenn sie sie aufschließt.
> - In Elzas Cutscene in Room 1021 wo Kendo Roy erschießt fehlen die Schuss Sounds.
> - In Elzas Cutscene in Room 1021 wo Kendo Roy erschießt fehlt noch ein Sterbe/Leid/Hinfall sound von Roy, sobald er erschossen wurde.
> - Ich möchte bei Elza das der prison key nicht in Room 1011 ist, wie aktuell, sondern er soll da sein, wo dann auch der Parking Lot Schlüssel in Leons Szenario ist. Auch hier - suche die ein gutes Model aus Resident Evil 2 aus für einen prison key.
> - Ich habe für Elzas Szenario Synchron Dateien hinzugefügt. In Room 1101 - während Roy bewusstlos wird - möchte ich das du das Voiceover main 16.5 abspielst. Auch wenn es aktuell keinen Text dafür gibt. Erstelle gerne ein Untertitel dafür, wie bei den anderen auch.
> - Manchmal selten löst die Granate bei der Verwendung nicht aus/wird nicht geworfen.
> - Mit der Super Redhawk zerplatzen/verschwinden die Krähen komplett, ist das in Resident Evil 2 auch so?
> - Bei den Monkeys in ROOM 11C0 läuft das Blut nicht aus wenn sie tot sind.
> - Die Monkeys scheinen noch oft blockiert zu sein. Das müssen wir irgendwie verbessern.
> - Bei den Zombies habe ich manchmal das Gefühl, das das Blut nicht an der richtigen Stelle auftaucht wenn sie getroffen werden.
> - Die Tür Textur zwischen ROOM 10C0 und 10D0 bei der Türanimation ist herausstechend billig im Vergleich zum Rest.
> - Ich habe dir ein neues PRI erstellt im ROOM 10F0 (02_02.png), weil mit der neuen Cutscene auffällig ist, wie Marvin nicht überdeckt wird von von der Absperrung.
> - Dann habe ich noch mal einen einzelnen Marker gemacht für ROOM 1100. Hier wird Leon mit einer Halbtransparenz überdeckt mit Boden.... Das ist natürlich falsch.
> - Ansonsten Verhalten sich die Zombies anders als in Resident Evil 2 - sie bleiben hier ewig Beteiligungslos stehen, obwohl ich ziemlich in Ihrer Nähe vor ihnen bin, fast vor ihrer Nase. Auch wenn sie innerhalb eines Raumes sind. Das darf so nicht sein....
> - Dann habe ich mal die Frage - was hat es mit der Kiste in ROOM 1090 auf sich. Hat die irgendeinen Verwendungszweck?
> - Im Treppenhaus 10A0 auf Etage B1 fehlen die Treppen eingezeichnet.
> - In ROOM 11D0 lande ich auf der Map direkt in der Mitte, sobald ich durch die Tür gehe, statt am Türeingang zu stehen...
> - Bei ROOM 1090 auf der Map, gibt es 2 Türen, eine am Ende des Ganges, wo die Hunde durch die Luke kommen, und eine wo man die Weste bekommt. Das muss als 2 separate Bereich in der Map eingeblendet sein und sauber getrennt.
> - Die Tür von ROOM 11C0 ist auf der Map um 90° verdreht.
> - Die kleinen Türbereiche von ROOM 1110 werden schon als besucht angezeigt, obwohl ich dort noch garnicht drin war
> - Die Fahrstuhl Etagen werden als besucht angezeigt, obwohl ich auf den verschiedenen Etagen noch nicht im Fahrstuhl war.
> - Bei der finalen Cutscene mit Irons wo er stirbt, ist das Türknallen noch falsch. Das ist kein Türknallen, das ist einfach nur der Sound von Tür schließen. Suche den Tür Knall Sound von zum Beispiel Resident Evil 2.
> Hier ist eine Liste aller sounds die ich gefunden habe: [RE2-SAP-Liste — abgelegt in `RE2_SOUND_LISTE.md` (Repo-Wurzel)]
> Ich würde bei door Sound, oder Zombies Breaking through Wood suchen ob dort eine Tür Knall Sound/Einbruch Sound zu finden ist. Hinterlege dir die Liste irgendwo, wo du sie immer findest. Die könnte ja auch später noch hilfreich sein.
> - Ebenfalls in der Cutscene wo Marvin und Ada in ROOM 11C0 stehen, ist der Dialog mit Marvin und Ada noch schrott. sie unterhalten sich nicht miteinander, sondern schauen irgendwo hin. Das musst du noch verbessern, das das besser passt. Sie müssen sich schon miteinandern unterhalten.
> - Ebenfalls in der Cutscene kommt das 'Türknallen' Dann die Kamera Ansicht und dann die Zombies aus dem nichts. Das soll so auch nicht sein. Erst 'Türknallen' Dann Zombie Spawnen und dann erst der Kamera Wechsel hin zu den Zombies.
> - In der neuen Cutscene in ROOM 10F0 verfolgt die Kamera wie Marvin und Ada den Raum verlassen. Sie 'ploppen' dann einfach weg. Das soll so nicht sein. Bleibe lieber mit der Kamera wenn Marvin und Ada den Raum verlassen auf Leon fixiert. und mache an der Stelle nur einen 'Tür schließen' Sound zum schluss.
> - Extrahiere mir Karte01 oder Karte02 von deinen Karten hier: "RE2-Weltmodelle: Öffnen Sie extracted_re2_items/uebersicht.html im Browser. Oben stehen die Schlüsselkarten und die drei Lageplan-Karten zum Aufsammeln.". Hänge sie mir beispielhaft an die Stelle von map3F.bmp, und schalte damit die Map für Etage 3F frei, wie das Kartensystem in Resident Evil 2 auch funktioniert. Auch mit den Text.
> - Bei Room 10D0 möchte ich das du die map für 2F da platzierst wo steht map2F.bmp.
> - Bei Room 1030 möchte ich das du die map für 1F da platzierst wo steht map1F.bmp
> - Map B2 soll im Room 1200 sein bei mapB2.bmp.
> - Map B1 soll im Room 11D0 sein bei mapB1.bmp

## Beigelegte Marken (Repo-Wurzel bzw. pri/, unversioniert; Kopien in `analysis/befunde_runde36/marken/`)

Rote Marke je Bild (320x240, Pixel-Bbox gemessen):
* `map3F.bmp` — ROOM1150 (Irons' Buero, Kamera mit Schreibtisch/Tassen rechts, Pinnwand links des Ganges
  zur Tuer): Marke (231..241, 72..81) = auf der PINNWAND rechts neben der Tuer -> 3F-Karte haengt dort.
* `map2F.bmp` — ROOM10D0 (Flur mit WANTED-Aushang rechts): Marke (270..289, 120..135) = unterer Teil des
  WANTED-Aushangs an der rechten Wand -> 2F-Karte dort.
* `map1F.bmp` — ROOM1030 (Wartebereich/Empfang mit Tresen rechts, Aushaenge links): Marke (155..164,
  103..113) = Wandflaeche zwischen den Aushaengen oberhalb der Stuehle -> 1F-Karte dort.
* `mapB1.bmp` — ROOM11D0 (Zwinger: Metalltuer mitte, Schaltkasten links, Schild rechts der Tuer): Marke
  (227..250, 65..93) = Wand rechts neben der Tuer unter dem Schild -> B1-Karte dort.
* `mapB2.bmp` — ROOM1200 (Leichenhalle: Kuehlfaecher, Glasvitrine links): Marke (58..74, 80..93) = an der
  GLASVITRINE links -> B2-Karte dort.
* `pri/STAGE1/10F0/02_02.png` (neu, 2026-10-05) — die vom Nutzer gemalte PRI-Maske der Absperrung in
  ROOM10F0 Cut 2 (Marvin wird nicht von der Absperrung ueberdeckt).
* Marker fuer ROOM1100 ("Leon mit Halbtransparenz vom Boden ueberdeckt"): KEINE Datei gefunden (Suche nach
  neuen png/bmp seit 2026-10-03 in Wurzel, pri/, masken_vorschau/, shots/) — die Spur misst den Befund selbst
  (alle Cuts von ROOM1100 abfahren), der Orchestrator fragt den Nutzer in der Abschlussmeldung nach der Datei.
* Neue Sprachdateien des Nutzers (synchro/STAGE1/, seit 2026-10-04): room1011 main00..17 + **main16.5.wav**,
  room1021 main00..08, room1031 main18/19, room1041 main02, room1141 main00..08 (Elza-Szenario).

## Original-Befund, der die Vorgabe "Irons bei Elza tot" beruehrt

ROOM1151.RDT (Elza) msg 2: "Irons: I'll be fine. Just worry about yourself for now." — im Original
begegnet Elza Irons LEBEND. Der Nutzer bestellt ausdruecklich: bei Elza liegt Irons von Anfang an tot da,
Text "He has already passed..." (NUTZER-VORGABE, Abweichung vom Original; Spur L setzt sie um, Spur P
(Story) beruecksichtigt sie in der Zeitleiste).

## Nachtrag 2026-10-10 14:00 — Story-Entscheidungen des Nutzers (woertlich)

Antwort auf die Story-Beratung (analysis/story_leon_elza/, Spur P Analyse-Teil):

> Ok, das klingt gut. Also:
>
> Ja, wenn es mehr sinn macht, das Roy erst da liegt, wenn der Strom eingeschaltet wurde, dann machen wir das so.
>
> Das Roy Kendo eingesperrt haben soll finde ich nicht so gut, weil er ja möchte das man ihn rettet. Besser fände ich folgendes:
>
> Da der communication room sabotiert wurde, wirft den Verdacht auf Kendo, da er die einzige bekannte Person im Police Office ist, die Kein Polizist ist (und kein Kind). Deshalb sperren sie ihn zur Untersuchungshaft in die Zelle. Kendo kam über Gastausweis mit seinen Freund Roy rein. Da die anderen Police officer wissen, das Kendo ein Freund von Roy ist, entziehen sie ihm quasi die Befugnis ihm aus dem Gefängnis zu befreien, deshalb findet man den Gefängnisschlüssel nicht bei Roy, sondern im Communications Room. Diese Information das das passiert ist, würde ich später noch in einem diary verfassen, das ich in Roys room ablegen werde.
>
> Bei Marvin - ja - in Elzas Szenario ist für mich auch ok, wenn er nur die Werkstatt Tür abschließt, nicht die Parking Lot tür.
>
> Zum Textwiderspruch - ja, da hat der vorherige Modder auf jeden Fall quatsch gebaut. Die dürfen sich niemals begegnet sein, das heißt - alle Textstellen, die darauf hindeuten das sie sich begegnet wären, müssen wir umschreiben.
>
> Bei Elza will ich nach wie vor das Irons tot bleibt und das Fenster 1121 bereits kaputt ist. Das macht Sinn für mich.
>
> bei Leon ein Text an Elzas Zelle („The cell door is open…") – die leere Zelle als Spur. - kannst du machen.
>
> ROOM1090. Elzas 1091 ist bytegleich Leons Zustand nach der Rettung – Leon war zuerst dort. Hinnehmen und eine Spur ergänzen („burnt-out shed") << kannst du machen, - ich weiß nicht was du mit den empty extinguisher meinst....
>
> Zu 11B0 wie gesagt ist kein Widerspruch - klar kann das eine elektronische TÜr sein, aber elektronische türen kann man unter umständen dennoch mit schlüssel separat abschließen. Deshalb kann das Marvin durchaus machen.
>
> Zu: Prison Key in 10F1: Elza ist vor Leon im Funkraum (ihr Satz „I couldn't call for help using the radio" belegt es). Für Leon bietet sich ein leerer Haken „Prison" als Spur an (K3). << würde ich sagen ist das kein Indiz dafür das sie zuerst da war, kann genauso gut sein, das sie es versucht hat, aber es eben nicht ging, weil die Kommunikationsanlage ja zerstört war. In die Richtung würde ich auch gehen.
>
> So, außerdem, setze fort, wo du vorhin als die tokens alle waren abgebrochen hast.

### Folgen fuer die Spuren (Orchestrator)
* **Spur P wird Bau-Spur** (eigener Baum r36_story, Datei engine/src/story_r36.c): (P1) Roys Leiche in Leons
  ROOM1020 erst NACH Einschalten des Stroms; (P2) Leon ROOM1210: Anklick-Text an Kendos/Johns Zelle "The cell
  door is open..." (leere Zelle als Spur; Wortlaut PORT-WAHL); (P3) Elza ROOM1091: Anklick-Text am
  ausgebrannten Schuppen ("burnt-out shed"-Spur, Wortlaut PORT-WAHL) — KEIN "leerer Feuerloescher" (war nur
  ein Vorschlag: Leon loescht das Feuer mit dem Feuerloescher, ein leerer Loescher am Boden waere die Spur);
  (P4) Text-Sweep "nie begegnet": JEDE Textstelle beider Szenarien, die andeutet, dass Leon und Elza sich
  begegnet sind oder einander kennen, wird umgeschrieben (bekannt: ROOM30C1 msg 2-6, ROOM6030 msg 1-5,
  ROOM6031 msg 0-2, ROOM3060 msg 1-8 im Leon-RDT; der Sweep sucht den Rest); (P5) Funkraum-Text bleibt
  ("It's out of commission" = Anlage zerstoert/sabotiert, passt zur Untersuchungshaft-Begruendung); der leere
  Haken "Prison" (K3) wird NICHT gebaut.
* **Spur J**: Marvin schliesst NUR die Werkstatt-Tuer ab (Innenseite, ROOM11B0), NICHT die Parkplatz-Tuer
  11B0->11C0 — Choreografie "Hold on" -> Werkstatt-Tuer -> Zuschliessen -> zurueck -> "I just want to make
  sure we don't have any unwanted followers." Die Parkplatz-Tuer bleibt fuer Elza erreichbar (11C1).
* **Spur Q**: Hintergrund fuer das Dossier: Prison Key liegt im Communications Room, weil die Beamten Roy die
  Befugnis entzogen haben (Kendo in Untersuchungshaft wegen des sabotierten Funkraums; Nutzer schreibt
  spaeter ein Diary fuer Roys Raum). Kein zusaetzlicher Text/Haken fuer Leon.
* **Spur L**: Irons tot + Fenster 1121 kaputt "von Anfang an" bleibt (bestaetigt).

## ⛔ Nachtrag 2026-10-10 abends — NEUE Nutzer-Vorgabe fuer Spur J Punkt 2 (11B0-Szene, Werkstatt-Tuer)

Woertlich:

> bevor der tür schluss sound bei Marvin kommt und die abschließ animation bei der neuen Cutscene in 11B0, muss marvin zunächst die tür Schließen inkl. animation, sonst gibt es ja nichts abzuschließen.

Folge fuer Spur J (gilt fuer Bau, Nachbesserung UND Abnahme — der Wortlaut von Punkt 2 ist damit erweitert):
Reihenfolge an der Werkstatt-Tuer in ROOM11B0 nach Marvins "Hold on": (1) Marvin laeuft zur (offenen) Werkstatt-Tuer,
(2) Marvin SCHLIESST die Tuer — Schliess-Animation (Marvin-Clip; sichtbar, wie die Tuer zugeht, soweit die Tuer im
Bild/als Objekt existiert — belegen, wie die Werkstatt-Tuer im Raum dargestellt ist: Hintergrund, Obj_model_set, ESP,
SCA-Zelle (3,0x0C) @0x00EC8 / 11B0 sub00 @0x0118C) + Tuer-zu-Ton (RE2-Tuerbank, RE2_SOUND_LISTE.md), (3) DANACH
Abschliess-Animation + Abschliess-Ton (RE2 Robert Kendo), (4) Tuer bleibt zu mit Elzas Text, (5) Marvin zurueck:
"I just want to make sure we don't have any unwanted followers." Abnahme: Ereignisfolge im Log mit Bildnummern
(Schliessen < Tuer-zu-Ton < Abschliessen < Abschliess-Ton) + Framedump-Serie.
