# Entscheide des Nutzers zur Bahnsteig-Form (bindend)

Vorrang vor Formblatt, Abnahmen und Dossiers. Werden nur vom Orchestrator ergaenzt.

## E1 (2026-11-01 16:17): Dunkle Masse rechts neben dem Arm gehoert zur Figur

Frage O1 der Abnahmen: dunkle Masse Bild 1 (405-431, 255-300), teils ausserhalb von `right_arm`.
Wortlaut Nutzer: "Ich wuerde sagen sie gehoert dazu, denn auch Der weisse Fleck geht dann noch ein Stueck weiter."

Folgen:
- Der Arm reicht bis zur Kante x 429-431, wie im Rohbau schon gebaut (rf_a1.py mit o1 an, Abschluss Abnahme 2: Arm bis x 427). `o1=0` entfaellt.
- `b1.rand_rechts` ist damit eine echte Sollkante, kein Vorbehalt mehr. Die Pruefung von `right_arm` zaehlt die Masse als Figur (Maske um diese Zone erweitern).
- Der helle Fleck Z5 endet nicht am Nutzerumriss bei x 412-416: Sichtbar laeuft sein Unterrand bis etwa (417,293), Blatt `nutzer_teile/o1_rand_x6.png`. Beim Malen die Zeichnung bis dorthin fuehren.

## E2 (2026-11-01): Handgroesse richtet sich nach dem Bild

Wortlaut Nutzer: "Betreff der hand, weiss ich nicht. musst du entscheiden was das bild her gib4."

Entscheid des Orchestrators: Die rechte Hand ist so viel groesser als die linke, wie die Masken in der Kamera von Bild 2 belegen, rund das 1,3-Fache (Abnahme 2, M4), nicht 1,5. Die Zahl wird beim Ausbau aus den Masken nachgemessen und im Dossier mit Rechenweg genannt. Was darueber hinausgeht, ist nicht belegt und entfaellt.

## E3 (2026-11-01 16:22): Kerbe im rechten Arm, rechts neben dem weissen Fleck

Wortlaut Nutzer zu rohbau_final/o1_blatt.png: "wird ganz rechts eine komplette Rundung gezeigt. Aber da ist noch eine Kerbe drin beim rechten Arm." und "So ziemlich rechts vom weissen Fleck.."

Gemessen (Y, Gauss 0,7, Blatt nutzer_teile/kerbe_rechts_x12.png): Rechts vom Fleck, der bei x rund 416 endet, liegt erst die dunkle Figur (x 418-422, Y 25-36), dann ein heller Keil x 424-432, y 279-288, hellste Stelle um (428,283) mit Y 58-60. Die Figur daneben hat 25-30. Bei x ab 434 folgt wieder der dunkle Wandsockel (Y 27-37). Oberhalb liegt die Kante der Masse bei x 429-430 (y 274: 41 -> 72 -> 104).

Folgen:
- Der Umriss rechts ist KEIN durchgehender Bogen. Oberarm-Masse O1 endet unten bei y rund 279, dann Keil nach innen mit Spitze um (422,284), danach setzt der Arm tiefer wieder an (ab y rund 290). Tiefe rund 8-10 px, Hoehe rund 10 px, bei Tiefe 6900 also rund 150-170 Einheiten.
- Anatomisch die Beuge am Ellbogen, Z5 sitzt direkt daneben. In der Form bauen, nicht malen: Ring-Radien an Oberarm-Ende und Unterarm-Anfang, in Pose 1 gegen Bild 1 pruefen, neuer Pruefpunkt in form_pruefung.py (Keil frei, Masse darueber und darunter gedeckt).
- Der Bereich unter y 290 rechts ist Figur gegen dunklen Sockel: dort ist keine Kante messbar.

## E4 (2026-11-01 18:55): Urteil zum Rohbau, Kugeln weg, Schwanz glatt

Wortlaut Nutzer zu neben_bild1/2 und rundum_hell: "Ist schwer zu sagen ob es korrekt ist, weil es noch so roh ist. Es besteht auch noch so aus Kugeln, wobei z.B. der tail im Screenshot Vorschau auch eher glatt ist. Und die Texturen fehlen halt so. Also schwer zu sagen ob das in die richtige Richtung gehen wuerde."

Folgen:
- Der Kugel-Eindruck muss weg: Glieder und Rumpf als durchgehende Formen mit weichen Uebergaengen, keine aufgereihten Wuelste, keine Kugel-Oberarme, keine Kugel in Pfanne, wo das Bild eine glatte Linie zeigt.
- Der Schwanz ist glatt wie im Screenshot. Die Lesung "Knollen und Ringbaender am Schwanz" aus rohbau_abnahme_3 gilt fuer den Schwanz NICHT.
- Erst mit Textur ist die Form beurteilbar: naechster Stand fuer den Nutzer ist bemalt und geglaettet, nicht roh.

## E5 (2026-11-01 19:20): Ablauf der Birkin-Kaempfe

Wortlaut Nutzer: "Also machen wir es so: 307 Mensch, 2. Hangar massig, mit Schwanz, Bahnsteig: Riesenform mit Schwanz. Im Zug: Noch mal Riesenform mit Schwanz. Den schiesst man dann raus. Und dann kommt noch mal die finale G5 Form aus Resident Evil 2."

1. ROOM3070/3071: Capcom-Birkin D0 (Mensch).
2. Hangar 50A0/50A1: Hangar-Form EM30_HANGAR (massig, Schwanz), nach der Spider-Man-Szene Kampf.
3. Bahnsteig 5080/5081: Riesenform mit Schwanz (Bahnsteig-Form, Bild 1).
4. Zug 5090/5091: noch einmal die Riesenform (Bild 2 = ROOM5090 Cut 11), wird aus dem Zug geschossen (Rueckstoss-Szene).
5. Danach RE2-G5 (Typ 0x36) als Endform.
Das Mod-Modell M1 entfaellt ganz. Offen beim Nutzer: Labor-Kampf 50E0/50F1 (Mod-Zusatz), Ort der Mutation Mensch -> Riesenform.

## E6 (2026-11-01 19:25): Labor-Kampf bleibt, Mutation mit Effekten

Wortlaut Nutzer: "Zum Labor-Kampf, den gibt es auch bei den 80% Videos habe ich gesehen, also ist der nicht von der Mod ausgedacht. Da braucht es dann glaube ich noch die Schwanz Form? also die nicht vierbeinige?" und zur Mutation: "ja, passt. Am besten schoen mit Effekten wie er mutiert waere toll"

- Labor 50E0 (Elza 50F1) bleibt als Kampf, Modell = Schwanz-Form (EM30_HANGAR, massig, nicht vierbeinig). Beleg: Complete Disc Film 4 um 0:20-0:24 (grosse Gestalt bei Leon am Pult).
- Die Mutation zur Riesenform geschieht am Ende des LETZTEN Kampfes in Schwanz-Form vor dem Bahnsteig (Raumfolge Labor/Hangar beim Bau pruefen), als Szene mit Effekten; Birkin flieht und erscheint am Bahnsteig als Riesenform. Ebenso endet der Bahnsteig-Kampf mit seiner Flucht, damit er im Zug wiederkommt.
- Mutations-Effekte sind Gestaltung: soweit moeglich aus RE2 (Effekte der G-Verwandlungen), sonst nach Bild; als PORT-WAHL markieren.

## E7 (2026-11-01 19:42): Labor-Form ist eine EIGENE Birkin-Form (berichtigt E6)

Wortlaut Nutzer: "Das ist NICHT die Birkin Form aus 307 oder die Form mit Schwanz! Das ist eine ganz andere Birkin Form! Siehe Screenshots in 80% Version."
Belege: Version_80%/vlcsnap-2026-11-01-19h34m20s216.png bis ...19h39m45s404.png (26 Bilder, Labor 50E im gruenen Licht: gebueckte, massige Gestalt mit langen Armen und grosser Klaue, rotbraune Flecken; dazu Leon auf dem Steg mit der Gestalt hinter dem Gelaender), dazu Version_80%/Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p).mp4 (51 s, bewegtes Video, keine Diashow).

Folgen:
- Meine Aussage "die Form passt zum Hangar-Birkin" war geraten und ist falsch. Labor 50E0/50F1 bekommt ein EIGENES neues Modell, nach diesen Bildern, Verfahren wie bei der Bahnsteig-Form.
- Ablauf damit: 307 Mensch -> Hangar 50A Schwanz-Form -> Labor 50E eigene Labor-Form -> Bahnsteig Riesenform -> Zug Riesenform (rausgeschossen) -> RE2-G5. Reihenfolge Hangar/Labor beim Bau an der Raumfolge pruefen. Wo die Mutationen mit Effekten liegen, folgt aus dieser Kette (zwischen den Formen).

## E8 (2026-11-01 21:05): Farbe und Textur weichen weit ab, nicht PSX-Stil

Wortlaut Nutzer zu pose_bild1/2 (gleiche Kamera und Pose, Modell sauber neben dem Screenshot): "Ok, die Farben und Textur weicht noch weit davon ab. Und es wirkt nicht ganz so wie aus einen Guss und psyx style sondern irgendwie höher aufgelöster...."

Folgen:
- PSX-Stil wie die RE2-Gegner: weniger, groessere Flaechen, Texeldichte gleichmaessig und grob, wenige Farben je Teil, gemalte grosse Formen statt Foto-Rauschen; ein Guss = durchgehender Malstil und Lichtauffassung ueber alle Teile, keine aufgestempelten Marken.
- Farbe ist offen: Die Rueckrechnung (farbe.md) ergab dunkles Rotbraun, der Nutzer sieht das als weit weg. Daher Varianten nebeneinander zeigen (Rueckrechnung, Screenshot-Farben direkt, Mitte), der Nutzer waehlt.

## E9 (2026-11-01 21:15): Schwanz optisch aus einem Stueck

Wortlaut Nutzer: "Auf jeden Fall geht es in die richtige Richtung, aber es ist noch nicht gleich auch der Schwanz hat dort mehrere Ebenen, beim screenshot ist es quasi eines optisch gesehen ohne Teilstuecke."

Folge: keine sichtbaren Teilstuecke am Schwanz: keine Stufe im Umriss an den Gelenken, kein Sprung in Helligkeit oder Textur, Normalen ueber die Gelenke gemittelt, Textur laeuft durch. Knochen bleiben fuer die Bewegung.

## E10 (2026-11-01 21:50): Haut und Wunden mehrfarbig und fleischig

Wortlaut Nutzer zu varianten/uebersicht_bild1/2: "Naja, ich glaube A, B, C sind alle in Ordnung. Das ist es nicht. Was es viel mehr kaputt macht, ist das gerade die auffallenden Flecken und Haut und sowas alles so einfarbig ist... So ist es doch nicht im Screenshot! Gerade die Haut hat doch mehr als eine Farbe und die offenen 'Wunden' sehen so richtig fleischig aus im Screenshot. Bei uns wirkt alles wie billig einfarbig bemalt."

Folgen:
- Farbgrundton: A, B, C sind fuer den Nutzer alle in Ordnung; Basis ist C (nach dem Filter am naechsten am Screenshot), Entscheid des Orchestrators.
- Haut mehrfarbig: Toene innerhalb der Haut wechseln (Violett, Rotbraun, Blaugrau, Fleisch), Adern, Flecken, Uebergaenge, nicht eine Farbe je Flaeche.
- Helle Flecken und Baender (Z1-Z9, Q, G, P) als offene, fleischige Wunden: rosa bis rot mit dunkelroten Tiefen, helle feuchte Glanzlichter, Raender aus Haut, keine Einfarbfuellung.
- Malhandwerk: so wie Capcom Fleisch und Wunden bei den RE2-Birkin-Formen malt (G2, G3, G4 nur als Handwerk, nie als Form).

## E11 (2026-11-01 22:10): Farben aus dem Screenshot uebernehmen

Wortlaut Nutzer: "Uebernimm es doch mehr oder weniger aus den Screenshot, da sieht man die unterschiedlichen Farben in den verschiedenen Bereichen doch sehr gut! Vor allem in der Rueckansicht."

Folge: Haut- und Wundfarben je Bereich aus Bild 1 (Rueckansicht) und Bild 2 per Projektion uebernehmen, Video soweit moeglich zurueckgerechnet, Bereichsunterschiede behalten; Verdecktes aus den sichtbaren Bereichen ergaenzen; danach PSX-Form (8 bpp, harte Texelkanten statt Unschaerfe).

## E12 (2026-11-01 23:00): Textur geordnet statt chaotisch

Wortlaut Nutzer zu Variante D (Farben aus den Screenshots): "Scheint besser zu werden, aber die Textur wirkt noch irgendwie kaputt und chaotisch".

Folge: Screenshot-Farben nur grossflaechig je Bereich uebernehmen (geglaettet entlang der Glieder), Detail gemalt mit Richtung (Muskelstraenge, Falten, Adern), gleicher Malstil ueber alle Teile, keine Spiegel- und UV-Naehte, keine Einzeltexel-Ausreisser, kein Feuerschein aus Bild 2 auf der Vorderseite. Mass: Inselanteil und Hochfrequenz wie RE2-Birkin-Texturen.

## E13 (2026-11-01 23:25): D ist die Richtung, nur der Pixel-Mischmasch muss weg

Wortlaut Nutzer zu D gegen E: "D. Ich weiss auch nicht warum du dort an den Stellen so ein Mischmasch bei Pixeln machen musstest an Farben. Ansonsten so gingen die Farben ja FAST in die richtige Richtung."

Folge: Basis D (Farben aus dem Screenshot in voller Kraft), E verworfen (zu leise). Kleine Farbinseln und bunte Einzeltexel den umgebenden Bereichen zuschlagen, Bereichskanten sauber; Wunden und Baender bleiben.

## E14 (2026-11-01 23:55): Keine Farben, die der Screenshot nicht zeigt (hebt E10 teilweise auf)

Wortlaut Nutzer zu F: "Also F ist besser als D, aber immer noch fraglich. Schaue dir doch noch mal den Screenshot an, und dann im Vergleich dazu das Modell. Nehmen wir mal zum Beispiel den weissen Fleck auf den rechten Arm. WO kommen denn die roten Pixel her auf den weissen Fleck? Die gibt es doch im Screenshot ueberhaupt nicht...."

Ursache: E10 wurde vom Orchestrator pauschal als "alle hellen Stellen fleischige Wunden mit dunkelroten Tiefen" umgesetzt. Z5 ist im Screenshot ein flaches blaeulich-weisses Dreieck ohne Rot.
Folge: Jede Stelle bekommt genau die Farben, die der Screenshot dort zeigt; Fleischrot nur wo der Screenshot Rot/Rosa zeigt. Je Stelle Nahvergleich und Mass "Farbton im Modell, den der Screenshot dort nicht hat" nahe 0.

## E15 (2026-11-02 00:10): Kein Cel-Shading, Texturlook wie das Spiel und der Screenshot

Wortlaut Nutzer zu G: "Naja, von den Farben her passt es einigermassen, aber es sieht immer noch komplett gezeichnet aus... es sieht so cell shading maessig aus, und passt nicht zu den restlichen Stil des spiels und auch nicht zum textur Look des Original Screenshots...."

Folge: Farben aus G als weiche Farbkarte, darueber eine Stoff-Schicht mit durchgehenden Tonwerten (Koernung, Poren, Falten, Fleckung), aus hoher Aufloesung heruntergerechnet, CLUT per Median-Cut wie RE2. Varianten H1 (prozedural) und H2 (Hautmaterial aus RE2-Birkin-Texturen, umgefaerbt, nur als Material). Mass gegen Spieltexturen und Stilblatt neben RE2-Birkin und RE1.5-Zombie.

## E16 (2026-11-02 00:30): Textur H2, Fuesse und Beine nach vier neuen Nutzer-Teilen

Wortlaut Nutzer: "Ja, auf jeden Fall sehr viel Besser. Ich kann mich garnicht entscheiden ob H1 oder H2 besser ist. Die Fuesse des Modells passen aber auf jeden Fall noch nicht. Die sind bei uns klumpige ovale Kugeln, im Screenshot sind das schon richtige saubere Fuesse und Beine: Ich habe dir noch mal 4 Screenshots gemacht, wo ich die Zusammensetzung des Beines drin habe...."

- Entscheid Orchestrator: Textur H2 (einzige Variante in allen Massen im Bereich der Spieltexturen), streifige Fasern am Ruecken beheben.
- Neue Nutzer-Teile in Version_80%/ (Bild 1): bein_links (198,335 55x68), fuss_links (192,395 28x53), bein_rechts (295,284 51x131), fuss_rechts (286,403 40x51). Beine gegliedert, Fuesse kantig mit Ferse/Sohle und tuerkisen Querbaendern, keine Ovale.
- Nachtrag Nutzer (00:28): "Fuesse quasi von unten man schaut frontal auf die unterseite in den screenshot." In Bild 1 zeigen die Fusssohlen zur Kamera; die tuerkisen Querbaender sind Sohlenstruktur. Pose 1 der Beine so, dass die Sohlen frontal sichtbar sind; Pose 2 und Ruhe stehen die Fuesse auf.
- Nachtrag Nutzer (00:29): bein_links ergaenzt, neue Lage (198,255) 55x149: linkes Bein vom Oberschenkel bis zum Knoechel.

## E17 (2026-11-02 01:05): Hangar-Birkin = Labor-Birkin = Sichel-Form (berichtigt E7)

Wortlaut Nutzer: "Ah, wenn man ganz genau hinschaut, sieht man im complete Disc Video 4, das der Birkin im Hangar und der Birkin danach der gleiche ist. Beide sind der Birkin mit dem Tail und der scharfe langen Sichel Hand rechts und der Klauen Hand links."

Gesehen (Version_80%/Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p).mp4): Hangar 45-51 s, Labor 12-18 s und Steg 18-22 s. Massiger Birkin, rosa-rot gefleckte Haut, kurze Jeans, Schwanz hinten, rechts eine lange rot-weisse Sichel-Klinge, links Klauenhand.
Folge: Hangar 50A und Labor 50E bekommen dasselbe Modell, die Sichel-Form. EM30_HANGAR (Capcom-D0 + Masse + Schwanz, mit Rohr) reicht dafuer nicht: rechte Hand wird Sichel, linke Hand Klaue, Haut wie im Video.
Ablauf damit: 307 Mensch -> Hangar 50A Sichel-Form -> Labor 50E Sichel-Form (Reihenfolge nach Raumfolge) -> Mutation mit Effekten -> Bahnsteig Riesenform -> Zug Riesenform -> G5.
- Entscheid Orchestrator zu Variante I (01:15): Die alte Zielmaske aus dem groben right_leg-Schnitt widerspricht in der Fusszone den neuen, genaueren Nutzer-Schnitten (rund 1000 px unter und neben dem rechten Fuss). Die neuen Schnitte haben Vorrang; diese Zone gilt als unbestimmt (grau). Damit Bild 1 IoU 0,965 statt 0,98 als Massstab.

## E18 (2026-11-02 01:20): Dicker wie in der Vorlage

Wortlaut Nutzer zu Variante I: "Irgendwie schon ein bisschen duenner als in der Vorlage. In der Vorlage ist es schon etwas dicker alles."
Folge: Breite je Hoehe aus den Masken; Beine, Fuesse und jede andere zu schmale Partie fuellen den Umriss (Breitenverhaeltnis 0,95-1,05), ohne Rueckfall in Ovale.

## E19 (2026-11-02): J ist die Form; naechste Stufe Bewegung und Angriffe nach G4

Wortlaut Nutzer: "Zu Variante J, sieht doch gut aus. Als naechstes Arbeite an Animationen, Angriffsmuster etc. Du kannst dich da gerne wahrscheinlich ein wenig an Birkin G4 orientieren."

Folge: Variante J (varianten/J_dicker, Textur H2) ist das Modell; Form und Textur werden nicht mehr veraendert, ausser eine Bewegung verlangt Gelenke/Gewichte. Animationen (Gehen, Rennen, Hiebe, Biss, Griff, Treffer, Drohen, Flucht usw.) und Angriffsmuster duerfen sich an RE2-Birkin G4 (Typ 0x34) orientieren; die Form bleibt die eigene nach den zwei Screenshots (E1-E18).

## E20 (2026-11-02 14:22): Vierbeiner mit Alligator-Kopf, Maul offen, Krallen aus Screenshot 2

Wortlaut Nutzer: "Ah, ich erkenne gerade noch beim 4 Beinigen - Er hat eher so eine Aligator Form. Schaue mal die Screenshots die ich jetzt gemacht habe: Man hat einen Aligatormaessigen Oberkiefer und dann ist sein Mund offen auf den 2. Screenshot und man sieht den Unterkiefer. Und Naja, ansonsten kann man aus dem 2. Screenshot noch lernen wie seine Krallen aussehen."

Neue Nutzer-Teile (Alpha-Ausschnitte in Version_80%/, nicht auf voller Leinwand), Lage per cv2.matchTemplate (TM_SQDIFF mit Alpha-Maske) in Screenshot 2 (Version_80%/vlcsnap-2026-10-31-00h29m02s830.png = bild2.png, Video A Standbild 145, ROOM5090 Cut 11):
- Oberkiefer.png 121 x 97 bei x 216, y 279 (Rest -0,00006): dunkle, breite, schnauzenfoermige Masse, Oberkante von (216,280) nach (330,316).
- Unterkiefer.png 105 x 92 bei x 239, y 318 (Rest -0,0003): roetliches Band mit 6 hellen Flecken (Abstand 16 px), schraeg von unten links (262,405) nach oben rechts (342,320).
- Leinwand-Masken nutzer_teile/maske_oberkiefer.png, maske_unterkiefer.png (Alpha >= 200 wie die anderen), Lage in nutzer_teile/lage_E20.json (bewusst nicht in lage.json, siehe varianten/K_alligator/K.md Abschnitt 1).
- Ueberlappung: Unterkiefer liegt zu 74 % im alten Teil right_arm_front (2477 von 3355 px). Das gerippte Band, bisher als rechter Unterarm gelesen, ist nach E20 der Unterkiefer; der rechte Unterarm liegt in Bild 2 dahinter.
- Krallen: Nutzer-Teile right_arm_front (Klauenhand unten links) und left_arm_front (Klaue unten rechts) aus Screenshot 2; Lesung in K.md Abschnitt 2.

Folgen: Variante K (varianten/K_alligator) baut auf J nur Kopf, Kiefer, Zaehne und Krallen neu; Rumpf, Beine, Arme, Schwanz und die Textur H2 (Seiten 0-2, CLUT-Zeilen 0-2) bleiben. Nachtrag Nutzer (Loecher zwischen Bein und Oberkoerper) wird in K mit erledigt (Gelenkkoerper, K.md).

## E21 (2026-11-02, uebermittelt im Auftrag "Nachbesserung 1 der Stufe 3"): Kopfvariante K3

Der Nutzer hat aus Variante K (E20, varianten/K_alligator) die Kopfvariante K3 (bester Fit, kurze, vorn breite Schnauze) gewaehlt; der beige
Unterkiefer ohne Raumlicht bleibt (im Spiel macht Licht und Videofilter daraus das rote Band von Screenshot 2, K.md Abschnitt 6).
Folgen: K3 wird ins Spiel-Asset RE2/BAHNSTEIG_FORM.EMD/.TIM eingebaut (Ober-/Unterkiefer an Knochen 22/23, Krallen an den Klauenknochen,
Gelenkkoerper), Kiefer im Stehen/Gehen/Ruhe zu, Posen-Pins mit den K3-Kopfwinkeln; Loecher zwischen Bein und Oberkoerper in keiner Pose
sichtbar (Nutzerbefund zu E20).
Nachtrag Orchestrator (abends): Kopf-Nachformung "Schildkroete statt Alligator" ist zurueckgestellt; erst Klaerungsblatt im echten Fenster.

## E22 (2026-11-02, nach Nachbesserung 1): Krokodilkeil nach der Oberkiefer-Maske, Textur durchgehend

Wortlaut Nutzer: "Die Textur vom Vierbeiner wirkt immer noch kaputt, ausserdem ist mir das Gesicht immer noch zu rundlich... Ich habe es doch markiert vom Oberkiefer her - Es wirkt eher krokodilmaessig. Hier sieht es eher hippomaessig aus...."

Folgen (Auftrag Nachbesserung 2 der Stufe 3):
- Kopf neu als Krokodilkeil: lange, flache, nach vorn spitz zulaufende Schnauze, flache Oberseite ohne Kuppel, Oberkiefer greift seitlich ueber den Unterkiefer, gezackte Zahnreihe seitlich sichtbar, Augenwuelste hinten oben, Nasenhoecker an der Spitze; von vorn schmal, kein rundes Maul. Ziel ist die Oberkiefer-Maske in Screenshot 2 (Nutzer-Teil Oberkiefer.png): IoU deutlich ueber K3 (0,51), Ziel >= 0,70, Unterkiefer >= 0,80, Umriss Bild 1 >= 0,954 31/31 und Bild 2 >= 0,948 7/7 (minimal darunter nur mit Begruendung; das Aussehen geht vor). Proportionsreferenz nur fuer die Anatomie: RE2-Alligator EM23 im Port, dazu K1. Kiefergelenke bleiben Knochen 22/23; Biss-Probe an die neue Spitze.
- E20/E21 neu ausgelegt: K3 (vorn breite, kurze Schnauze) ist ersetzt; die Kopflage folgt weiter Screenshot 2 (offenes Maul, Unterkiefer-Band).
- Textur: die Haut soll im Spiel wie durchgehende Haut im H2-Stil aussehen (E10-E16 gelten: keine erfundenen Farben, kein Cel-Shading). Ursache messen (Heatmap, echtes Fenster), beheben; Gegenprobe Mosaik-Mass <= J.

## E24-E28 (2026-11-05): neue Referenzen des Nutzers, Beschriftung aus Sicht des Wesens

Nutzer 2026-11-05 (Wortlaut-Kern): "Ich bin weder mit der stehenden Birkin Form mit dem Schwanz, noch mit der 4 Beinigen Birkin Form richtig zufrieden. ... Bei der 4 Beinigen Form habe ich auch KI und Tripo AI Unterstuetzung mir geholt und eine 4 Seiten Ansicht erstellen koennen ... Was auch klar auffaellt, diese langen hellen Dinge waren keine Fusssohlen, sondern gehoerten zum Boden. Tripo AI war aber nicht in der Lage den Mund so richtig abzubilden ... Auch ich habe das noch mal EXAKT gemacht im Bild Vorderseite_Beschreibung.png."

Berichtigt 2026-11-06 nach FORMBLATT_V2.md (Pruefer des Workflows "birkin-referenzen-lesen"): Meine erste Lesung der Linienenden war falsch (zwei Enden vertauscht, Maulrichtung falsch). Massgeblich sind die GEMESSENEN Linienenden (rote Pixel zu 100 % belegt, vertauscht nur 18 %), Bild 2 = 640x480 (KI-Bild / 2):
- rechtes Auge (W) (223/271), linkes Auge (W) (288/269), Oberseite Maul Knick (316/303) -> (312/331), offenes Maul (329/326), Spitze obere Vorderseite (245/371), Ende untere Vorderseite (279/400), linke Klaue (W) (452/379), rechte Klaue (W) (281/434).

- **E24 Augen:** `Version_80%/Vorderseite_Beschreibung.png` (KI-hochgerechnetes Bild 2 mit roten Linien, Fassung 11:17:07 mit 8 Linien) - der Nutzer hatte die Augen zuerst aus seiner Betrachtersicht beschriftet und hat die Datei am 2026-11-05 selbst korrigiert. Alle Beschriftungen gelten **aus Sicht des Wesens**: rechtes Auge (W) = links im Bild. Der Kopf zeigt frontal zur Kamera (Augenlinie waagerecht, 63 px Abstand, Oberkieferspitze senkrecht darunter).
- **E25 Klauen:** Nutzer auf Rueckfrage: "Das ist aus Sicht des Wesens." - "linke Klaue" (W) liegt RECHTS im Bild (452/379), "rechte Klaue" (W) links unten (281/434). Das passt zum frontal stehenden Wesen.
- **E26 Fuesse:** Die hellen laenglichen Flecken unten in den Screenshots sind **Boden**, keine Fusssohlen (gemessen 13-37 % der Fussschnitte, fuss_links mit dem Boden verbunden). Die Sohle und die tuerkisen Querbaender aus dem E16-Nachtrag entfallen. Fusskrallen zeigt nur die KI (Tripo); im Original sind je Hand 2-3 Krallenspitzen sichtbar, Zahl je Fuss offen.
- **E27 Maul:** Das Tripo-Maul ist falsch (Nutzer). Massstab fuer Kopf und Maul ist allein `Vorderseite_Beschreibung.png`: Kopf frontal, Augen oben, Schnauze nach UNTEN (Spitze (245/371) 1,9 Grad neben der Senkrechten unter der Augenmitte); das Maul oeffnet sich vom Maulwinkel (329/326) nach links unten im Bild = **nach rechts aus Sicht des Wesens, zur Frau hin** (die Frau steht links im Bild = rechts vom Wesen). Unterkiefer = helles Band mit 5-6 Zahnflecken, Achse 38,5 Grad, Ende (279/400); Maulspalt 23-27 Grad. K4 hat die Maulrichtung richtig, ist aber ein 3/4-Profil (Augen 16,8 px auseinander, verdeckt), rund halb so breit wie gemessen und sitzt an der falschen Stelle (Augenmitte 94 px vom J-Halsende, erreichbar ~40 px). Die Tripo-Ansichten liefern nur Koerperbau/Silhouette.
- **E28 Zweibeiner (Sichel-Form, Spur SF):** Nutzer zur KI-Hochrechnung von Video 4 (`..._slp.mp4`, Freisteller `birkin01-24.png`, Tripo `image_1.png`/`image_2 (2).png`/`image_3 (3).png`): "Man sieht klar Kopf und den langen dicken Schwanz der bis in den oberen Ruecken reicht. So moechte ich das haben dem Original entsprechend." Lesung (SF_LESER_A/B): Schwanzwurzel oben am Ruecken (Buckel), Verlauf erst waagerecht nach hinten, dann Haken nach unten, Spitze frei in Knie-/Oberschenkelhoehe, Wurzel 0,11-0,17 H dick; Port-Wurzel sitzt 0,2-0,3 H zu tief und halb so dick. Original bleibt Massstab, die KI-Bilder sind Lesehilfe.
- Lese-Ergebnis: `FORMBLATT_V2.md` (Riesenform, Fragen F1-F9, Aenderungen AE1-AE12, Regeln R1-R10) und `r41_sichel/analysis/befunde_runde41/SF_FORMBLATT_V2.md` (Sichel).

## E30 (2026-11-07 ~22:20): Riesenform K5c V3 Runde 5 wird ohne Farbaenderung das Spiel-Asset fuer v0.8.30

Vorlage: `C:/workspace/git/reAi_v2_entscheidung_2026-11-06/7_riesenform_runde5_krallen_farben/LIESMICH.txt` (Frage 1 Zweifarbigkeit unter dem
Bahnsteiglicht A/B/C, Frage 2 Leuchtstelle in Bild 1: Becken wie jetzt oder nur das Gesicht).

Wortlaut Nutzer: "Nein, wie lassen es so wie es jetzt ist. Sieht doch gut aus. Auch die Leuchtstelle sieht gut aus."

Lesung:
- Frage 1 -> **A**: so uebernehmen. Keine gemeinsame Hautfarbe (B), kein Verbleib von K4 (C).
- Frage 2 -> **Leuchtstelle bleibt wie jetzt**: Becken in der Bild-1-Lage (F1-Stelle, Knochen 0) und Gesicht am Maul (Bild 2).

Folgen:
- Die Variante `varianten/K5/farbe/K5c_farbe_v3/FORM.EMD` (sha256 090e0b80e214ae455788a383da6dd8f1b44ec6b26f771b281780a24953c5cfb0) und
  `FORM.TIM` (sha256 512828067ea7f0480763110b4820ae311bc5cd7423796d58274b07ef776e7e6d) = Runde 5 ist OHNE jede Farbaenderung die Basis des
  Spiel-Assets `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM` fuer v0.8.30. Keine weitere Gestaltungsrunde.
- Technik ohne Rueckfrage (in der Vorlage angekuendigt): 17 CLUT-Zeilen -> 16, weil der PC-Port je CLUT-Zeile eine Texturkopie untereinander
  legt (`re15_port/platform/pc/src/render_pc.c` re15_render_pc_upload_tim_slot: tex_h = tim->height * n_cluts, keine Maximalpruefung;
  SDL_CreateTexture liefert ueber dem Renderer-Maximum NULL, GLES2 oft 4096) - 17 x 256 = 4352 > 4096. Gemessen: Zeile 2 wird von keiner
  Flaeche gelesen; sie entfaellt, das Bild bleibt gleich (Riegel `varianten/K5/generator/riegel_einbau.py`).
- Clips: das Spiel-Asset braucht die 28 Clips der Stufe 3 (Tick-Clips 0/3/4/5/8/9/10/26/27, `engine/src/birkin_bahnsteig.c`,
  Test `test_birkin_bahnsteig.c` 28 Clips). Die Kette `stufe3/gen/s3_clips.py` baut sie auf der K5c-Basis (`stufe3/basis_k5c`), wie bei K4
  auf `stufe3/basis_n2`. K4 bleibt als Variante unter `varianten/K4/`.
- Dossier: `varianten/K5/FARBE.md` Abschnitt "Einbau v0.8.30 (Nutzerentscheid E30)".
