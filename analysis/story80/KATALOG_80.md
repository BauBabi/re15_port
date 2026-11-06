# Katalog des 80%-Materials: Story (Spur ST80-K)

Stand 2026-11-02, Zweig `r41/story80`. Reine Auswertung, kein Spielcode geändert. Gegenstück zur Spur ST80-P (Port und 40%-Disc).

**Kürzel.** A#nnn = Foto nnn in Video A. B m:ss = Zeit in Video B. F1 bis F4 m:ss = Complete-Disc-Film 1 bis 4 in der 480p-Fassung unter `Version_80%/`. Disc = 40%-Disc `Version_40%/` (Capcom, 6.11.1996). Port = `re15_port/shared_assets/PSX`. Raumnummern sind Disc-Nummern, Cut = Kamera.
**Belegstärke.** hart = Bild, Ton oder Bytes zeigen es direkt. wahrscheinlich = mehrere passende Indizien. unsicher = Sichtbefund an der Grenze der Auflösung oder Einzelindiz.
**Story-Aussage.** Spalte „Story-Aussage“ nennt je Eintrag, was er hart über wer ist wo, wer kennt wen, wer tut was, was ist passiert sagt. Maßstab für Änderungen am Port ist der nachweisliche Widerspruch zu dieser Spalte. Ein fehlender Eintrag ist kein Widerspruch.
**Nicht verwendet.** Englische Dialoge der Fan-Mod (`info/Re1.5`) sind kein Beleg und werden nicht zitiert. Project Umbrella und Enigmatism415 dienen nur als Lesehilfe und sind so gekennzeichnet.

## 0. Ergebnis in Kürze

1. **Umfang.** Gesamtverzeichnis mit 353 Einträgen (`katalog/daten/eintraege_alle.csv`): 174 Fotos aus Video A, 125 Abschnitte aus Video B, 54 Einstellungen aus den vier Filmen. Daraus 101 story-tragende Katalogeinträge K001 bis K101 (Abschnitt 2), 34 Textstellen T01 bis T34 (Abschnitt 3), 64 Orte (Abschnitt 5).
2. **Neu: Im 80%-Stand sprechen die Figuren Englisch.** Film 1 und Film 3 haben echte Sprachausgabe, gleichzeitig mit den japanischen Untertiteln. Leon auf dem Dach: "Oh, that's just freakin' great!" und "Now what am I gonna do?". Hinterhof 306: Sherry "Elza, I know a secret way in!", Elza "A secret entrance?", Sherry "It's here! Look, I can get inside through this hole!", Elza "Sherry, wait!". Belegt durch Sprachdetektor, zwei Spracherkenner auf zwei Kodierungen, Gleichlauf mit den Untertiteln und Tonhöhe (Abschnitt 6). Die 40%-Disc hat keinen Sprachordner. Außer diesen sechs Sätzen gibt es in 14 Minuten Material keine Sprache.
3. **Raumliste.** Alle 120 Raumnummern 100 bis 607 existieren im Port als RDT, beide Varianten. Als **Ort fehlen im Port** fünf Schauplätze des Spielmaterials: Moosgang (Disc 207, Elza gegen Spinnen), Raum mit rotem Rad, Tunnel mit oranger Tür, Rippen-Tunnel, die Bahnsteig-Halle aus A#143 und A#150 (als Kamera). Dazu kommen vier Orte aus dem Trailer. Auf der Disc fehlen 15 Orte des Materials, die der Port hat (Abschnitt 5).
4. **Neu: Figurenprofile im Auswahlmenü** (B 0:10.5): Leon 21 Jahre, 178 cm, Blutgruppe A. Elza 19 Jahre, 169 cm, 52,4 kg, Blutgruppe 0.
5. **Neu: Trailertafeln** (B 9:33 bis 10:06): 「あの惨劇から2か月後」「ラクーンシティ全土に」「真の悪夢が始まった」「恐怖再び」: Zwei Monate nach jener Tragödie begann in ganz Raccoon City der wahre Albtraum.
6. **Neu: Ablauf der Kanalszene** (F2 0:01 bis 0:10.5): Ada watet zuerst durch den Kanal zur Leiterseite, dann folgt Marvin, Leon bleibt auf dem Sims stehen. Kein Text, kein Ton-Dialog.
7. **Bestätigt aus der Vorarbeit:** Kein Bild zeigt Leon und Elza zusammen. Elza und Marvin stehen zusammen am Tresen in 30C (A#058). Leon steht mit der Frau im Kittel in 603 und am Bahnsteig. Sherry ist in 309 und 30C allein unterwegs.
8. **Quellenklärung:** Die vier Filme stecken vollständig in Video B (Bild r 0,93 bis 0,99, Ton r 0,92 bis 0,95). Die 480p-Fassungen sind dieselben Aufnahmen wie die 360p-Dateien unter `documentation/`, ohne den 6,6 s langen Vorspann. `documentation/documentation.mp4` und die übrigen Clips dort sind Aufnahmen des Ports mit Mod-Text, also ausgeschlossen. Die 28 `vlcsnap`-Bilder stammen aus A#143, A#145 und F4 0:14.25 bis 0:19.65 (Gegnerformen, ohne Story-Inhalt).

## 1. Quellen (a)

| Quelle | Datei | Art | Länge, Format | Story-Gehalt | Bewertung |
|---|---|---|---|---|---|
| Video A | `Version_80%/Biohazard 1.5 Final Build.mp4` | Diashow von Enigmatism415 (2013) aus 173 Bioflames-Fotos und einem SurvivHor-Foto, je 0,8 s | 139,4 s, 640x480 | Figuren und Untertitel in Einzelbildern | Je Foto hart. Fotos teils aus einer Debug-Sitzung (#031, #148, #159). Ein Foto belegt einen Zustand, keinen Ablauf. Die Reihenfolge ist die des Uploaders |
| Video B | `Version_80%/Resident Evil 1.5 (All Footage).mp4` | Sammelschnitt: Bioflames-Aufnahmen, die vier Filme, ein Werbeschnitt, ein Biohazard-2-Trailer | 14:19, 320x240 | Abläufe, zwei Dialogszenen, Trailertafeln | Hart für das Sichtbare. Bildqualität oft schlecht |
| Film 1 bis 4 | `Version_80%/Resident Evil 1.5 Complete Disc Video N (480p Upscale) (480p).mp4` | Capcom-Filme der Complete Disc, Fan-Mitschnitt, hochskaliert | 67,0 / 37,6 / 47,5 / 51,4 s | Abläufe, Ton mit Sprache | Hart. Gleich mit den 360p-Dateien unter `documentation/` (Bild r Median 0,99, Ton r 0,95 bis 0,99, dort 6,6 s Vorspann „Biohaze.com“ davor). Zeitumrechnung: 360p = 480p + 6,6 s |
| Filme in Video B | – | – | – | – | F1 = B − 17,1 s. F2 = B − 5:03,6. F3 = B − 3:17,9. F4 = B − 12:18,1. F4 0:00 bis 0:04.5 ist in B von einer Blende überdeckt (`werkzeuge/cd_align.py`, `ton_vgl.py`) |
| Nutzer-Bilder | `Version_80%/vlcsnap-2026-10-31-*.png` (2) | Bildschirmfotos | 640x480 | Gegnerform am Bahnsteig | = A#143 (r 0,998) und A#145 (r 0,999) |
| Nutzer-Bilder | `Version_80%/vlcsnap-2026-11-01-*.png` (26) | Bildschirmfotos | 640x480 | Kreatur in 50E | = F4 0:14.25 bis 0:19.65 (r 0,997 bis 0,999) |
| Nutzer-Ausschnitte | `Version_80%/` Teile (`face_and_upper_body.png`, `tail.png`, `bein_*.png` usw.) | Freigestellte Teile aus den zwei Oktober-Bildern | klein | keiner | Arbeitsmaterial Gegnerform, kein eigener Beleg |
| Ausgeschlossen | `documentation/documentation.mp4`, `documentation/*.mp4` (missing_*, plot_hole*, sound_missing*, *working_systems*) | Aufnahmen des Ports/der Mod mit englischem Mod-Text („Part 2 Fixing incomplete Systems“, „Part 3 Fixing Plot holes“) | 1080p/720p | – | Kein 80%-Material. `documentation/message.png` ist eine Notiz des Mod-Autors |
| Vorarbeit | `analysis/story_begegnungen/pruefung/video_a.md` §9, `video_b.md` §4 | Schnittlisten mit Raumbestimmung | – | – | Übernommen nach Stichprobe, Abweichungen in Abschnitt 9 |

## 2. Katalog je Raum und Szenario (b)

**Methode Raum.** Übernommen aus der Vorarbeit, wo nicht anders vermerkt: SIFT mit RANSAC gegen 531 Disc- und 1119 Port-Hintergründe (D = Inlier Disc, M = Inlier Port), ECC-Ausrichtung (E, ab 1,0 sicher), Sicht (S). Für die Filme eigene SIFT-Prüfung (`katalog/daten/raumabgleich_filme.json`) und die Bildkorrelation zu Video B. Der Port-Baum ist der Mod-Baum: alle 360 Raumdateien (RDT, BSS) sind md5-gleich mit `info/Re1.5/PSX`.
**Methode Figur.** Nach Aussehen: Leon = blaue Polizeiweste mit Stern/R.P.D., rotbraunes Haar, Jeans. Elza = rot-weißer Anzug, rote Hose, blondes Haar. John = weißes Hemd, Jeans. Sherry = Kind, blond, hellblaues Oberteil, kurze dunkle Hose. Marvin = dunkle Haut, hellblaues Kurzarmhemd mit Ärmelabzeichen, dunkle Hose. Ada = dunkles Kurzhaar, weiße Jacke, kurzer Rock, rote Schuhe. Frau im Kittel = helles/blondes Haar, weißer Kittel, schwarzes Oberteil, lange dunkle Hose. Birkin (Menschform) = blasser Oberkörper mit roten Adern, Kopf mittig, lange Jeans, Stange. Platzhalter der Disc spielen hier keine Rolle: es zählt das Aussehen im Bild.
**Szenario.** Video A: Fotos #001 bis #081 und #173 im Elza-Block, #082 bis #172 im Leon-Block (Spielfigur im Bild, wo nicht anders vermerkt). Video B und Filme: nach Spielfigur im Bild.

### Stufe 1: Revier

| ID | Raum (Beleg) | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K001 | Menü | beide | B 0:10.5–0:18.8 | Leon und Elza je in eigener Bildhälfte | Figurenwahl. Hinter Leon ein Dach mit Geländer und Warnlicht (wie 117), hinter Elza ein blaues Gitter (wie 103) | T01 | Zwei getrennt wählbare Hauptfiguren. Leon 21 J., 178 cm, Blutgruppe A. Elza 19 J., 169 cm, 52,4 kg, Blutgruppe 0 | hart (Text). Hintergründe als Startorte nur Sicht: unsicher |
| K002 | 102 Büro (D49, D7, D36) | Elza | A#001, #026 (Cut 11, Kamera des Disc-Ereignisses Roy), #030 | Elza allein, Blutlache | – | – | Elza ist in 102 | hart |
| K003 | 102 (D18) | Leon | A#091 | Leon allein | – | – | Leon ist in 102 | hart |
| K004 | ohne Hintergrund (blaue Fläche) | Elza | A#027, #028 | Elza kniet, ein Polizist sitzt am Boden: dunkle Weste mit zwei hellen Trägern, rundes Abzeichen, kurze Ärmel, nackte Unterarme | Elza kümmert sich um einen Verletzten | – | Elza trifft einen verletzten Polizisten, der nicht Leon ist. Name nicht im Bild, wahrscheinlich Roy (Disc 1021 Typ 0x47 neben John, Nachbarfotos aus 102) | Figuren hart, Roy wahrscheinlich, Ort offen |
| K005 | 103 (Vorarbeit Cut 6, eigener SIFT M10306) | Elza | F1 0:42.6–0:45.9 = B 0:59.5–1:03 | Elza, Zombies hinter dem Gitter | Elza schießt durch das intakte Gitter | – | Bei Elza steht das Gitter in 103 noch | hart |
| K006 | 103 (Cut 12, 11, 5) | Leon | B 2:12.8–2:33 | Zombies, dann Leon | Zombies brechen durch das Gitter, Leon kämpft, Kopfschuss 2:25.6 | – | Bei Leon bricht das Gitter in 103 durch (Kamerafolge wie Disc `ROOM1030 sub08`) | hart |
| K007 | 103 (Cut 11, 1) | unklar | B 7:49–8:03.7 | Zombies, Explosionen | Gitter zerstört, Spielfigur nicht erkennbar | – | – | unsicher |
| K008 | 103 (Trailer, Cut 9, 0, 5) | unklar | B 9:36–9:40.4 | 3 bis 4 Gestalten in hellen Hemden und dunklen Hosen von hinten, vor Gitter und Wandkarte | Zombies oder Polizisten, nicht unterscheidbar | – | – | unsicher |
| K009 | 104 Gang mit Rolltor (D6, E1,29) | Elza | A#002–#005, F1 0:32.8–0:37 = B 0:50–0:54, B 3:06.5 | Elza, Zombie, kriechendes Wesen, Leiche | Rolltor halb offen, dann zu, große Blutlache | – | Elza ist in 104, das Rolltor bewegt sich | hart |
| K010 | 104 (D6) | Leon | A#095, B 2:39.5, B 6:01–6:58 | Leon, Zombies | Rolltor offen, Leon drückt den Knopf, Tor schließt | T09 | Leon schließt in 104 das Rolltor per Knopf | hart |
| K011 | 104 (Trailer) | unklar / Leon | B 9:56–10:01.6 | Figur wird gebissen, Leon-Modell, Zombie | Trailer | – | – | unsicher |
| K012 | 107 Büro (eigener SIFT M10706) | Elza | F1 0:24–0:32.8 = B 0:41–0:50, B 8:05–8:44 | Elza, Zombies | Elza kämpft, Granaten zerteilen Zombies | – | Elza ist in 107 | hart |
| K013 | 107 | Leon | A#092 (D14), B 2:33, B 11:26–12:21 | Leon, RPD-Zombies | Kampf | – | Leon ist in 107 | hart |
| K014 | 107 (Cut 2) | unklar | B 7:16–7:20 | drei dunkle Figuren, eine fällt | keine Elza im Bild | – | – | unsicher |
| K015 | 109 Hof mit Transporter (M6, E0,85) | Elza | A#035, #173 | Elza allein | Transporter ohne Feuer | – | Bei Elza brennt der Transporter nicht | hart (Bild) |
| K016 | 109 (E1,35, E1,41) | Leon | A#096, #100 | Leon allein | Transporter brennt (#096) | – | Bei Leon brennt der Transporter | hart (Bild) |
| K017 | 10A (E0,98, nicht auf Disc) | Elza | A#020 | Elza allein | Türeinblendung | – | Elza ist in 10A | wahrscheinlich |
| K018 | 10B Zufahrt (E1,25, D11, E1,45) | Elza | A#032, #033, #034 | John (weißes Hemd, Jeans) und Elza. #033 John von hinten, Elza klein am Leiterfuß. #034 Lieferwagen RACCOON POLICE | Gespräch, dann Leiter | T03 | Elza und John sind zusammen in 10B. Elza will zurück, weil drinnen ein Mädchen ist | Figuren hart, Text teils lesbar |
| K019 | 10B (D12, Cut 1, 2, 8) | Leon | A#107, F1 0:59.1–1:05.5 = B 1:16–1:20.6 | Leon allein | Leon läuft zur Leiter und ist an ihr zu sehen (Cut 8) | – | Leon ist allein in 10B | hart |
| K020 | 110 (E1,35, E1,27) | Elza | A#006, #010 | Elza, zwei Gegner in blauen Hemden, Leichen | – | – | Elza ist in 110 | hart |
| K021 | 110 | Leon | A#088 | Leon | – | – | Leon ist in 110 | hart |
| K022 | 114 Konferenzraum (S, E0,83, nicht auf Disc) | Elza | A#012, #018 | Elza und John am Konferenztisch. #018 zusätzlich roter Haufen am Boden (Wesen) | Gespräch | T02 | John warnt Elza, nicht aufs Dach zu gehen, dort sei ein Monster. Elza und John sind zusammen in 114 | Figuren hart, Text wahrscheinlich, Ort wahrscheinlich |
| K023 | wohl 114 | Elza | A#013 | dunkelrotes Riesenwesen, Elza klein | – | – | Bei Elza erscheint ein großes rotes Wesen | Figur hart, Ort unsicher |
| K024 | 115 Chefbüro (Cut 7, E0,81) | Elza | A#016, #017 (D14); B 2:48.5–2:57.5 | #016 Elza von hinten am Sofa, ein Liegender auf dem Sofa | – | T04 | Im Elza-Lauf liegt ein Verletzter im Chefbüro, und Leons Name fällt. Wer spricht, ist offen | Bild hart, Sprecher unsicher |
| K025 | 115 (Cut 5, Cut 7) | Leon | A#087 (E1,41), #089 (E0,91) | Leon von hinten, Liegender auf dem Sofa | gleiche Kamera wie #016, kein Text | – | Bei Leon liegt derselbe Verletzte im Chefbüro | hart |
| K026 | 115 (Trailer, Cut 0, 2, 3) | wohl Leon | B 9:42.5–10:00 | Leon-Modell schießt auf Zombies | Trailer | – | Zombies im Chefbüro (Trailerstand) | wahrscheinlich |
| K027 | 117 Dach (Cut 4, 3, 2, 1) | Leon | F1 0:00–0:13.2 = B 0:18.3–0:31; A#082 (E1,03), #083 (E0,75); B 1:22.5–1:31 | Leon allein am Geländer, dann über das Dach; B 1:22.5 mit Krähen | Leon spricht zwei Sätze (Ton und Untertitel), A#083 ein dritter Untertitel | T05, T06, T07 | Leon ist allein auf dem Dach und flucht über seine Lage. Er will etwas melden, an wen ist unlesbar | hart (Bild, Ton), Wortlaut T05-Japanisch unsicher |
| K028 | 117 Dach (Cut 3, D8) | Elza | A#015; F1 0:51.6–0:59.1 = B 1:08.5–1:16 | Elza allein, Krähen | Krähen greifen an, Elza wehrt sie ab | – | Elza ist auf dem Dach, John ist nicht im Bild | hart |
| K029 | 118 Gang | Leon | F1 0:13.2–0:19.4 = B 0:30.5–0:36.5; B 7:03.5 | Leon, zwei Zombies | Kampf | – | Leon ist in 118 | hart |
| K030 | 119 Schießstand (D9, D21) | Leon | F1 0:37–0:42.6 = B 0:54–0:59.5; A#090; B 8:45–9:10 | Leon, Hunde | Schalter, Kampf | T08 | Leon ist in 119 | hart |
| K031 | 119 (D21) | Elza | A#014, #019; B 2:57.5–3:06.5 | Elza, Hunde | Kampf | – | Elza ist in 119 | hart |
| K032 | 11A (D11) | Elza | A#022 | Elza, dunkle Gestalt vorn rechts | – | – | Elza ist in 11A | hart |
| K033 | 11B Garage (E1,50, D7) | Elza | A#021, #041 | Elza allein | – | – | Elza ist in 11B | hart |
| K034 | 11B (D9, D18) | Leon | A#094; A#169–#172 | #094 Leon allein. #169–#172 Marvin, Leon, Ada | Gespräch über den Fluchtweg nach unten | T10, T11, T12 | Leon, Marvin und Ada sind zusammen in 11B und planen, in den Untergrund zu fliehen | Figuren hart, Texte wahrscheinlich |
| K035 | 11C Parkgarage (D12 bis D34) | Leon | A#119–#129 | Ada und Leon (#119, #127, #128); Umbrella-Lkw (#124); rote Gorillas (#125, #126); Leon kniet (#123) | Treffen, Kampf | T13 | Leon trifft Ada in 11C. Ada fragt, ob er „ihm“ nicht begegnet sei | hart |
| K036 | 11E (D13, E1,20) | Leon | F1 0:19.4–0:24 = B 0:36.5–0:41; A#135, #136; B 6:59.5 | Leon, Zombies, brennendes Wesen | Kampf | T28 | Leon ist in 11E | hart |
| K037 | 11E | Elza | B 1:46.4–1:56.4 | Elza, Zombies | Kampf | – | Elza ist in 11E | hart |
| K038 | 120 Leichenhalle (E1,16, D20) | Leon | F1 0:45.9–0:51.6 = B 1:03–1:08.5; A#130–#134; B 1:33.5 | Leon, Leichen, RPD-Zombies | Leon setzt einen Flammenwerfer ein | – | Leon hat in 120 einen Flammenwerfer | hart |
| K039 | 120 | Elza | B 3:12.5–3:18 | Elza, RPD-Zombies | Kampf | – | Elza ist in 120 | hart |
| K040 | 121 (D7) | Leon | A#097 | Leon, Arme greifen durchs Gitter | – | – | Leon ist in 121 | hart |
| K041 | 122 (D7, Disc nur BSS) | Leon | A#098 | Leon, Leiche | Türeinblendung | – | Leon ist in 122 | hart |
| K042 | 123 (E1,03, Disc nur BSS) | Elza | A#023 | Elza | – | – | Elza ist in 123 | wahrscheinlich |

### Stufe 2: Kanal

| ID | Raum (Beleg) | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K043 | 200 Kanal, Cut 11 an der Leiter (E1,17, D9) | Elza | A#036, #037 | Elza kniet vor Sherry | Gespräch | T14 | Elza und Sherry sind zusammen im Kanal. Elza spricht Sherry auf eine große Uhr an | Figuren hart, Text wahrscheinlich |
| K044 | 200, Cut 13 (D29) | Elza | A#039, #040 | Elza fern, dunkle Großfigur vorn | – | – | – | unsicher |
| K045 | 200, Cut 15 und 11 (D6 bis D8) | Leon | A#103–#105 | Ada (weiß, rote Schuhe), Marvin, Leon an der Leiter | Gespräch | T15 | Leon, Marvin und Ada sind zusammen im Kanal | hart |
| K046 | 200 oder 20B, Cut 2 (beide Räume haben dieses Bild) | Leon | F2 0:01–0:10.6 = B 5:04.5–5:14.5 | Leon, Marvin, Ada | Ada watet zuerst zur Leiterseite, Marvin folgt, Leon bleibt (Abschnitt 4) | kein Text, keine Sprache | Ada geht voran, Marvin folgt ihr, Leon wartet | hart |
| K047 | 200, Cut 13 | Leon | F2 0:21.1–0:27.1 = B 5:24.5–5:30.5 | Leon, Zombies | Kampf im Tunnel | – | Leon ist allein in 200 unterwegs | hart |
| K048 | **Raum mit rotem Rad, fehlt** | Leon | F2 0:10.6–0:15.6 = B 5:14.5–5:19 | Leon, Riesenspinne (gelb-schwarz) | Kampf, Spinne platzt grün | – | Leon kämpft in einem Maschinenraum mit rotem Rad gegen eine Riesenspinne | hart (Bild), Ort fehlt |
| K049 | **Tunnel mit oranger Tür, fehlt** | Leon | F2 0:15.6–0:21.1 = B 5:19–5:24.5 | Leon, Zombies in Khaki, einer kriecht | Kampf | – | Leon kämpft gegen Zombies in Khaki | hart (Bild), Ort fehlt |
| K050 | **Rippen-Tunnel, fehlt** | Leon | B 4:05.5–5:03.5, 7:06.9–7:08, 7:26.5–7:31, 10:10.7–10:12 | Leon, große Zombies in Khaki, Klauenzombie | Kampf | – | wie K049 | hart (Bild), Ort fehlt |
| K051 | **207 Moosgang, fehlt im Port** (eigener SIFT D20704 123, D20707 147, kein Port-Treffer) | Elza | B 5:41–6:01; A#043 (207 oder 20B) | Elza, Spinnen | Kampf | – | Elza kämpft im Moosgang gegen Spinnen | hart |
| K052 | 20A (E0,92) | Elza-Block | A#046 | kleine Figur an einer Leiter | – | – | – | unsicher |
| K053 | 20B grauer Gang, Cut 11–14 und 7 | Leon | F2 0:27.1–0:37.5 = B 5:30.5–5:40; B 7:39 | Leon, Spinne | Hinter Leon bricht eine weiße Wolke aus der Tür (Cut 7) | – | Leon ist in 20B, aus der Tür bricht etwas Weißes hervor | hart (Bild), Art der Wolke offen |
| K054 | 20B, Cut 13 | Elza | B 7:05.2, 10:09 | Elza, Spinne | Kampf | – | Elza ist in 20B | hart |

### Stufe 3: Fabrik

| ID | Raum (Beleg) | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K055 | 300 Fabrikhof (D16, E0,94) | Elza | A#044, #048 | #044 Elza liegt am Boden, John steht daneben. #048 Lkw, Leichen | – | T16 | Elza und John sind zusammen im Fabrikhof. John: die Fabrik dürfte stillliegen | Figuren hart, Text wahrscheinlich |
| K056 | 300, Cut 2 und 1 | Elza | F3 0:12.8–0:17.7 = B 3:30.5–3:35.5; B 7:20 | Elza, Zombies am Umbrella-Lkw | Kampf | – | Elza kämpft im Fabrikhof | hart |
| K057 | 300, Cut 0 (D16) | Leon | A#118 | Leon steht. Ein Mann hockt neben einer Leiche: weißes Hemd, blaue Hose, überstrahlt (Kleidung eher wie John als wie Marvin) | – | T17 | Leon und ein zweiter Mann sind im Fabrikhof. Wer, ist offen | Leon hart, Mann unsicher |
| K058 | 301 (D24, D34) | Elza | F3 0:17.7–0:28.3 = B 3:35.5–3:46; A#068, #069 | Elza, Zombies | Kampf | – | Elza ist in 301 | hart |
| K059 | 302 oder 30D (bildgleich, E0,97, M6) | Elza | A#070, #071 | Elza, Streifenwagen | – | – | Elza ist in 302/30D | wahrscheinlich |
| K060 | 302 oder 30D (E1,15) | Leon | A#115 | Leon und eine zweite, helle Figur, Blaufilter | – | – | – | unsicher |
| K061 | 303 (D8) | Elza | A#057 | Elza | – | – | Elza ist in 303 | hart |
| K062 | 304 (M25, E1,01 bis E1,14) | Elza | A#062–#064, #066 | Elza, Zombies, Gore | Kampf | – | Elza ist in 304 | hart |
| K063 | 304 (M26, M14) | Leon | A#113, #114 | Leon, Gore-Wesen | Kampf | – | Leon ist in 304 | hart |
| K064 | 305 (M14, M12, nicht auf Disc) | Elza | A#060, #061, #065 | Elza; Rechner nah; Bildschirm „Now Running…“, „Break“, „システムエラー“ | Elza am Rechner | T18 | Elza bedient in 305 einen Rechner, er meldet einen Systemfehler | hart (Bild), Ort wahrscheinlich |
| K065 | 306 Hinterhof, Cut 5 (D14) | Elza | F3 0:00–0:12.8 = B 3:19.5–3:30.5; A#049 | Sherry, John, Elza | Sherry spricht Elza an, zeigt das Lüftungsgitter, kriecht hinein. Elza ruft ihr nach. John bleibt stehen | T19–T22 (gesprochen und Untertitel) | Sherry kennt Elza beim Namen und einen geheimen Eingang. Sie geht allein hinein, Elza und John bleiben draußen | hart |
| K066 | 306, Cut 3 | Elza | F3 0:28.3–0:33.6 = B 3:46–3:51.5 | Elza, Zombie, Hunde | Kampf | – | Elza ist danach noch im Hinterhof | hart |
| K067 | 306, Cut 4 (D14) | Leon | A#116 | Leon und Marvin an der orangen Tür | – | – | Leon und Marvin sind zusammen im Hinterhof | hart |
| K068 | 307 Halle mit Lastenaufzug (D6 bis D25) | Elza | A#073–#081; B 1:58.9–2:08; F3 0:33.6–0:47.4 = B 3:51.5–4:03.5; B 7:10.5–7:14 | Elza; Birkin in Menschform mit Stange | Elza an der Steuerbox, das Aufzugtor öffnet sich, Birkin steht darin, Nahaufnahme, Kampf, Elza am Boden | T23 | Birkin kommt bei Elza aus dem Aufzug und kämpft mit ihr | hart |
| K069 | 307 (D12 bis D18) | Leon | A#117, #140, #141, #142; B 13:09–13:56.9 | Leon; Birkin. #142: helle Stehfigur, hockende Figur, helle Großfigur | Leon an der Konsole, Kampf gegen Birkin. Marvin ist nicht im Bild | T24 | Birkin kämpft auch mit Leon in 307 | hart (Kampf), #142 unsicher |
| K070 | 308 (E1,10, E0,81) | Leon-Block | A#166, #167 | kleine Figur, helle Schemen | – | – | – | unsicher |
| K071 | 309 (D11 bis D26, E1,33) | Elza-Lauf, Spielfigur Sherry | A#050–#052, #054, #055 | Sherry allein in fünf Kameras, in #051 an der Kiste beim Kasten | – | T25 | Sherry ist allein in 309 unterwegs | Bild hart, Spielbarkeit wahrscheinlich |
| K072 | 309, Cut 15 (D6) | Elza | A#059 | Elza, John, Sherry nebeneinander | – | T26 | Elza, John und Sherry sind in 309 wieder zusammen. John ist knapp entkommen | hart |
| K073 | 30A (M8, E0,87) | Elza | A#067, #072 | Elza | – | – | Elza ist in 30A | wahrscheinlich |
| K074 | 30C Tresen, Cut 9 (E0,98, nicht auf Disc) | Elza | A#058 | Marvin (dunkle Haut, hellblaues Kurzarmhemd, Ärmelabzeichen) und Elza | stehen sich gegenüber, kein Text | – | **Elza trifft Marvin.** Anlass und Gespräch unbekannt | Figuren hart, Ort wahrscheinlich |
| K075 | 30C, Cut 10 (M33, eigener SIFT M30C10 31) | Elza-Lauf, Sherry | A#053 | Sherry allein an der Treppe | – | – | Sherry ist allein in 30C | hart |
| K076 | 30C Cut 3 oder 30D Cut 10 (bildgleich, E1,28) | Leon | A#139 | Leon allein am Tresen | – | – | Leon ist in 30C/30D | wahrscheinlich |
| K077 | 30E (M13) | Elza | A#056 | Elza | – | – | Elza ist in 30E | hart |

### Stufe 4: Labor

| ID | Raum (Beleg) | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K078 | 400 (D9, E1,23) | Leon-Block | A#164 | Figur fern | – | – | – | unsicher |
| K079 | 408 grauer Metallgang (eigener SIFT M40802 95; Disc nur Rohbau-BSS) | Elza | F4 0:19.7–0:28.7 = B 12:38–12:47 | Elza, dunkelrotes, affenartiges Wesen | Wesen springt Elza an, Elza schießt | – | Elza kämpft in 408 gegen ein rotes Wesen | hart |

### Stufe 5: Untergrund, U-Bahn

| ID | Raum (Beleg) | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K080 | 501 (D14, E1,44) | Leon | A#161 | Leon | Türeinblendung | – | Leon ist in 501 | hart |
| K081 | 503 (D12) | Leon-Block | A#160 | Figur | – | – | – | unsicher |
| K082 | 504 blauer Gang (Sicht gegen Port 50402) | Elza | F4 0:28.7–0:34.4 = B 12:47–12:52.5; B 7:08, 10:12 | Elza, am Boden kriechende braune Gestalten | Kampf | – | Elza ist in 504 | hart |
| K083 | 506 | Elza | B 7:31–7:39, 10:43.5–11:25.5 | Elza, Zombie, ab 10:54 Rotlicht | Kampf | – | Elza ist in 506, das Licht wechselt auf Rot | hart |
| K084 | 509 roter Gang (M24, E0,95, nicht auf Disc) | Leon-Block | A#144, #145, #168 | #144 Leon. #145 Frau im Kittel, dunkles Riesenwesen, Feuer. #168 kleine Figur an einer Konsole | – | – | Die Frau im Kittel und die Riesenform sind zugleich in 509 | Bild hart, Ort wahrscheinlich |
| K085 | 50A Hangar C-2, Cut 12 (eigener SIFT M50A12 23) | – | F4 0:45–0:51.3 = B 13:03–13:07.3 | Birkin, gelb-schwarzes Wesen | Birkin packt das Wesen, wirft es weg, kommt auf die Kamera zu | – | Birkin ist im Hangar und kämpft mit einem anderen Wesen. Keine Spielfigur im Bild | hart |
| K086 | 50B Maschinenhalle, Cut 2 (eigener SIFT M50B02 59, nicht auf Disc) | Leon | F4 0:07–0:10.7 = B 12:25–12:29; A#155 | Leon. Neben der roten Maschine eine weiße, senkrechte, über 3 s ortsfeste Form | Leon läuft in die Halle | – | Leon ist in 50B. Ob die weiße Form eine Figur ist, bleibt offen | Leon hart, Form unsicher |
| K087 | 50C (Sicht gegen 50C04, D12) | Leon | F4 0:00.5–0:07; A#162, #163 | Leon | läuft durch die Halle | – | Leon ist in 50C | hart |
| K088 | 50E (eigener SIFT M50E08 33, M50E05 28, nicht auf Disc) | Leon | F4 0:10.7–0:19.7 = B 12:29–12:38 | Leon, große Kreatur | Leon an der Konsole, die Kreatur fällt von oben vor ihn (Cut 8), Leon rennt weg, die Kreatur steht am Geländer (Cut 5) | – | In 50E wird Leon von einer großen Kreatur überrascht und flieht | hart (Bild). Kreatur laut Nutzerentscheid E17 Birkin in Sichelform, im Bild nicht benannt |
| K089 | 512 (D51206) | Elza | F4 0:41.5–0:45 = B 12:59.5–13:03; B 9:11.7–9:31.5 | Elza, brennender Zombie, Rotfilter | Kampf | – | Elza ist in 512 | hart |
| K090 | 514 Hangar-Gang | Leon | F4 0:34.4–0:41.5 = B 12:52.5–12:59.5; A#156–#158 | Leon, brennende Zombies | Kampf | – | Leon ist in 514 | hart |
| K091 | 514 | Elza | B 7:43–7:47, 10:37.5–10:43.5 | Elza, brennender Zombie | Kampf | – | Elza ist in 514 | hart |
| K092 | roter Gang mit Debug-Menü | Leon-Block | A#159 | – | Debug „…CORRIDOR 2-A“, Spielzeit 0:06:37 | T34 | – | unsicher |

### Stufe 6: Bahnsteig und Zug

| ID | Raum (Beleg) | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K093 | **Bahnsteig-Halle, Kamera fehlt** (kein Treffer in 1650 Hintergründen) | Leon | A#143, #150 | #143 Leon, riesiges dunkles Wesen, roter Zug. #150 Leon allein | – | – | Leon trifft am Bahnsteig auf die Riesenform | Bild hart, Ort als Kamera fehlt |
| K094 | 600 Bahnsteig, Cut 3, 4, 1 (M8, M15, E1,13; eigener SIFT M60003 11; nicht auf Disc) | Leon | A#152, #153, #154 | Leon und die Frau im Kittel (#152, #154), Leon allein (#153) | – | – | Leon und die Frau im Kittel sind zusammen am Bahnsteig | Figuren hart, Ort wahrscheinlich |
| K095 | 601 (M30, eigener SIFT 27; nicht auf Disc) | Leon | A#149 | Leon allein | – | – | Leon ist in 601 | hart |
| K096 | 603 Führerstand, Cut 1 und 0 (D7, E1,12, E1,07) | Leon | A#146, #147 | Frau im Kittel am Pult, Leon | – | – | Leon und die Frau im Kittel sind zusammen im Führerstand, sie steht am Pult | hart |
| K097 | 604 Maschinenraum (D22, E1,36) | Leon | A#148, #151 | #148 Debug-Menü, Spielzeit 0:09:01. #151 Leon allein | Foto nach Debug-Sprung | T34 | Belegt nur den Ort, nicht die Besetzung | hart (Ort) |

### Ohne bestimmbaren Ort

| ID | Ort | Szenario | Quelle | Figuren | Lage, Handlung | Text | Story-Aussage | Stärke |
|---|---|---|---|---|---|---|---|---|
| K098 | offen | Leon-Block | A#084 | Ein Mann hockt am Boden, hellblaues Hemd mit Ärmelabzeichen, eine zweite Gestalt beugt sich über ihn | – | T27 | Jemand sagt: alle wurden von Zombies gefressen oder sind selbst Zombies geworden | Figuren unsicher, Text teils lesbar |
| K099 | offen (Tür mit Fenster) | Leon | A#085 | Leon und ein breiter Mann mit hellem Hemd und dunklem Kopf (wohl Marvin) | – | – | Leon trifft einen Mann, wohl Marvin | wahrscheinlich |
| K100 | offen | Leon-Block | A#165 | zwei blaue Figuren und eine Figur in Weiß | – | – | – | unsicher |
| K101 | Inventar | beide | A#078, #093 | Ausweiskarten | Leon: Karte „POLICE“, „Leon S Kennedy“, Foto, Unterschrift. Elza: Karte mit rotem Kopf, Text unlesbar | T33 | Leon führt einen Polizeiausweis | hart (Leon), Elza unlesbar |

Weitere 27 Fotos ohne sicheren Ort und alle reinen Kampf- und Laufbilder stehen im Gesamtverzeichnis `katalog/daten/eintraege_alle.csv`. Unter ihnen zeigt keines eine weitere Figurenkombination (Vorarbeit `video_a.md` §8, hier für #084, #085, #115, #118, #142, #165 nachgesehen: `katalog/videoA_lupe_084_118_142_165.jpg`, `katalog/videoA_078_093_115.jpg`).

## 3. Textliste (c)

Lesung: Glyphenkamm im Disc-Font (`DATA/TEX.TIM`) mit Nullmodell aus Zufallszeichen gleicher Länge (Vorarbeit `video_a.md` §5, für T05 und T06 eigene Messung `katalog/werkzeuge/kamm.py`, `rang.py`). [ ] = ergänzt oder unlesbar. Sprecher nur, wo belegt, sonst „offen“. Die deutsche und englische Fassung sind wörtlich aus dem Japanischen übersetzt. Wo es Capcom-Ton gibt, steht er in der Zeile darunter und ist Originalwortlaut.

| Nr | Quelle, Zeit | Raum, Szenario | Sprecher | Japanisch (Bild) | Deutsch | Englisch | Lesbarkeit | Disc 40% |
|---|---|---|---|---|---|---|---|---|
| T01 | B 0:10.5–0:18.8 | Menü | – | PLEASE SELECT MAIN CAST / Leon,S,Kennedy / Elza Walker / PROFILE HIGHT 178 Cm AGE 21 BLOOD TYPE A / PROFILE HIGHT 169 Cm AGE 19 WEIGHT 52.4 Kg BLOOD TYPE 0 (Original Englisch, Leons Gewicht vom Logo verdeckt) | Figurenwahl mit Profil | wie Original | hart | `DATA/SELECT.TIM` ähnlich, die Disc-Zeilen „From police department“, „From women's college“ sind nicht zu sehen |
| T02 | A#012, 0:09.61 | 114, Elza | John (wahrscheinlich: Anrede an Elza, Männersprache, John im Bild) | エルザ！屋上に行っちゃダメだ！ / [ヤ]バイ怪物がいやがる | Elza! Du darfst nicht aufs Dach gehen! / Da ist ein übles Monster. | Elza! You mustn't go up to the roof! / There's a nasty monster there. | Z1 0,80 (Null 0,53), Z2 0,65 (Null 0,51), ヤ ergänzt | fehlt |
| T03 | A#032, 0:25.63 | 10B, Elza | Elza (wahrscheinlich: Disc 10B #0 ist Elzas Satz zum selben Inhalt, weibliche Endung の) | [まだ..]内に女の子がいるの / 戻らなきゃ | [Noch] ist drinnen ein Mädchen. / Ich muss zurück. | [There's still] a girl inside. / I have to go back. | Z1 0,80, Z2 0,74, Anfang unlesbar | 10B #0 gleicher Inhalt, anderer Wortlaut |
| T04 | A#016, 0:12.81 | 115, Elza | offen (Elza oder der Verletzte, Lesehilfe Project Umbrella: Elza, Enigmatism415: Chef) | ……レオン | …Leon | …Leon | 0,82 (Null 0,51), Zahl der Punkte unsicher | fehlt (115 #0 ist ein anderer Satz) |
| T05 | F1 0:01.2–0:04.15 = B 0:18.3–0:21.3 | 117, Leon | Leon (hart: allein im Bild, Männerstimme 145 Hz) | おい　冗談だろ？ | He, das ist doch ein Witz, oder? | Hey, you're kidding, right? | おい gemessen. 冗談だろ？ visuell vereinbar, die Messung trennt es nicht von 危険だぞ！ (Kamm der ganzen Zeile 0,49 gegen Null 0,37, Einzelwort unter dem Nullmodell): unsicher. Lesehilfe Project Umbrella: 冗談だろ？ | fehlt |
| T05-Ton | F1 0:02.3–0:04.5 | 117, Leon | Leon | – | Oh, das ist ja echt großartig! (ironisch) | **Oh, that's just freakin' great!** | hart (Abschnitt 6) | – |
| T06 | F1 0:04.17–0:06.24 = B 0:21.3–0:23.4 | 117, Leon | Leon | …クソッなんてこった！ | …Mist, was für eine Bescherung! | …Damn, what a mess! | Zeichen 1 bis 6 Rang 1 von 593, った Rang 2, こ unsicher, ！ vom Geländer überdeckt: wahrscheinlich | fehlt |
| T06-Ton | F1 0:05.0–0:06.3 | 117, Leon | Leon | – | Was soll ich jetzt bloß machen? | **Now what am I gonna do?** | hart | – |
| T07 | A#083, 1:06.47 | 117, Leon | Leon (wahrscheinlich, allein im Bild) | そうだ！[..]にこの[..]報告しないと | Genau! Ich muss [..] diese [Sache] melden. | That's it! I have to report this [..] to [..]. | そうだ！ 0,89 (Null 0,63), die zwei Kanji danach unlesbar (署 fehlt im Disc-Font) | fehlt |
| T08 | F1 0:37–0:42.6 = B 0:54.3–0:55.8 | 119, Leon | System | スイッチがある　おしますか？ ▸Yes　No | Da ist ein Schalter. Drücken? ▸Ja Nein | There's a switch. Press it? ▸Yes No | hart | 119 #0 gleich |
| T09 | B 6:47.8–6:48.8 | 104, Leon | System | ボタンがある / 押しますか？ ▸Yes　No | Da ist ein Knopf. Drücken? | There's a button. Press it? | hart | 104 #0 gleich |
| T10 | A#169, 2:15.34 | 11B, Leon | Marvin (wahrscheinlich, Disc 11B #10 ist Marvins Satz) | 上のさわぎは何だ？ | Was ist das für ein Lärm oben? | What's that commotion upstairs? | 0,75 (Null 0,49) | 11B #10 zweizeilig mit „Leon! Du lebst!“, hier einzeilig |
| T11 | A#170, 2:16.14 | 11B, Leon | Leon (wahrscheinlich, Disc 11B #16) | 地下に逃げるってのはどうだ？ | Wie wär's, wenn wir in den Untergrund fliehen? | How about we escape underground? | 0,66 (Null 0,37) | 11B #16 mit Vorzeile zum Kanaldeckel |
| T12 | A#172, 2:17.74 | 11B, Leon | offen | 下に何かいるかも知れないしー / こんな言[葉]があるだろ？ | Da unten könnte ja auch etwas sein… / Es gibt doch so ein Sprichwort, oder? | There might be something down there, too… / There's a saying like that, right? | Z2 0,71 mit Leerzelle (言 Rang 2), 葉 fehlt im Disc-Font: [葉] ist Lesart | fehlt |
| T13 | A#127, 1:41.70 | 11C, Leon | Ada (wahrscheinlich, Disc 11C #3) | 会わなかったのか？ | Bist du ihm nicht begegnet? | Didn't you run into him? | hart (Sicht klar) | 11C #3 gleich |
| T14 | A#037, 0:29.63 | 200, Elza | Elza (wahrscheinlich: kniet vor Sherry, weibliche Endung ね) | …まあ　大きな時計ね！ | …Oh, was für eine große Uhr! | …Oh my, what a big watch! | 0,81 (Null 0,51). 時計 nicht von 時間, 手紙 trennbar, Lesehilfe Project Umbrella: 時計 | fehlt |
| T15 | A#105, 1:24.08 | 200, Leon | offen | マーヴィン[..] | Marvin… | Marvin… | マーヴィン 0,72 (Null 0,47), Rest unlesbar | fehlt |
| T16 | A#048, 0:38.44 | 300, Elza | John (wahrscheinlich, Disc 300 #12) | [使]われてないはずだ | …dürfte nicht mehr in Betrieb sein. | …shouldn't be in use anymore. | ない 0,76 gegen ねぇ 0,70 (Null 0,52) | 300 #12 mit ねぇ statt ない |
| T17 | A#118, 1:34.49 | 300, Leon | offen | みんな　ラクー[..] / [unlesbar] | Alle [sind] Raccoon-[..] | They're all Raccoon [..] | みんなラクー 0,83 (Null 0,55), Rest unlesbar | 300 #3 lautet anders (Marvin: alles Ermittler im Umbrella-Fall) |
| T18 | A#065, 0:52.05 | 305, Elza | Bildschirm | Now Running…（7×）/ Break / [シス]テムエラー | Systemfehler | System error | Sicht | fehlt |
| T19 | F3 0:01.3–0:03.57 = B 3:19.9–3:21.5; A#049 | 306, Elza | Sherry (hart: Kinderstimme 340 Hz, Inhalt) | …エルザ / 私　ヒミツの入り口を知ってるの | …Elza, ich kenne einen geheimen Eingang. | …Elza, I know a secret entrance. | hart | 306 #3 gleich |
| T19-Ton | F3 0:01.2–0:03.0 | 306 | Sherry | – | Elza, ich weiß einen geheimen Weg hinein! | **Elza, I know a secret way in!** | hart (vier Erkennerläufe gleich, Name gehört als „Elsa“) | – |
| T20 | F3 0:03.57–0:06.07 = B 3:22–3:23.5 | 306 | Elza (Frauenstimme 195 Hz) | ヒミツの入り口？ | Einen geheimen Eingang? | A secret entrance? | hart | 306 #4 gleich |
| T20-Ton | F3 0:03.8–0:05.4 | 306 | Elza | – | Einen geheimen Eingang? | **A secret entrance?** | hart | – |
| T21 | F3 0:06.07–0:09.74 = B 3:24–3:28.5 | 306 | Sherry (327 Hz) | ここよ　この中から入れるの | Hier! Von hier drinnen kommt man rein. | Here! You can get in from inside here. | hart | 306 #5 gleich |
| T21-Ton | F3 0:06.2–0:09.0 | 306 | Sherry | – | Hier ist es! Schau, ich komme durch dieses Loch hinein! | **It's here! Look, I can get inside through this hole!** | hart, „through this hole“ in 3 von 4 Läufen (einmal „Dana's hole“): wahrscheinlich | – |
| T22 | F3 0:11.2–0:12.78 = B 3:29.5–3:30.2 | 306 | Elza (Ausruf, 333 Hz) | シェリー！　待って！ | Sherry! Warte! | Sherry! Wait! | hart | 306 #6 gleich |
| T22-Ton | F3 0:11.5–0:12.8 | 306 | Elza | – | Sherry, warte! | **Sherry, wait!** | hart | – |
| T23 | A#075, 1:00.06 | 307, Elza | System (Untersuchen) | エレベーター[?] | Aufzug… | Elevator… | 0,80 (Null 0,56), Endzeichen unlesbar, wohl Zeile im Aufbau | 307 #15 Z3 vereinbar |
| T24 | A#140, 1:52.11 | 307, Leon | offen | ホール[..] | Die Halle… | The hall… | ホールは 0,71 (Null 0,48), ganze Zeile 0,53 | 30E #8 vereinbar |
| T25 | A#051, 0:40.84 | 309, Sherry | System (Untersuchen, wahrscheinlich) | 電げんがある / 電げ[..] | Da ist eine Stromversorgung. | There's a power supply. | Sicht, Rest hinter dem Wasserzeichen | fehlt |
| T26 | A#059, 0:47.25 | 309, Elza | John (wahrscheinlich, Disc 309 #9) | はあ、はあ…間一ぱつってヤツだな！ / [..]ツにくいつかれてた所だぜ | Puh, puh… das war knapp! / [Fast] hätten sie mich in den [Hintern] gebissen. | Phew, phew… that was a close call! / [They] nearly bit me in the [butt]. | Z1 Sicht, Z2 am Wasserzeichen nicht messbar | 309 #9, das Foto füllt die Disc-Lücken ぱつ und く |
| T27 | A#084, 1:07.27 | offen, Leon-Block | offen | みんなゾンビに[くわれたか] / それとも、ゾンビになっているかだよ。 | Alle wurden von Zombies [gefressen] / oder sie sind selbst zu Zombies geworden. | Everyone's been [eaten] by zombies / or they've turned into zombies. | Z1 0,54, くわれたか ist Lesart der Vorarbeit; Z2 Sicht | fehlt |
| T28 | B 6:59.5–7:01.1 | 11E | Einblendung | 画面は開発中のものです。 | Die Bilder zeigen den Entwicklungsstand. | Screens show a version in development. | hart | – |
| T29 | B 9:33.2–9:36 | Trailer | Tafel | あの惨劇から / 2か月後 | Zwei Monate nach jener Tragödie | Two months after that tragedy | hart | – |
| T30 | B 9:40.4–9:42.5 | Trailer | Tafel | ラクーンシティ / 全土に | in ganz Raccoon City | throughout all of Raccoon City | hart | – |
| T31 | B 9:44.8–9:48.3 | Trailer | Tafel | 真の悪夢が / 始まった | begann der wahre Albtraum | the true nightmare began | hart | – |
| T32 | B 10:02.8–10:05.8 | Trailer | Tafel | 恐怖再び | Das Grauen kehrt zurück | Terror once again | hart | – |
| T33 | A#093, 1:14.47 | Inventar, Leon | Ausweis | POLICE / Leon S Kennedy | Polizeiausweis | Police ID | Sicht | – |
| T34 | A#031, #148, #159 | Debug | Menü | UTILITY MENU / …YOKUSHITSU (動力室) / …CORRIDOR 2-A | Debug-Menü | Debug menu | Sicht | Raumnamen der EXE |

## 4. Szenen mit Ablauf (Video B und Filme)

Video A ist eine Diashow, aus der Fotoreihenfolge folgt kein Ablauf. Die folgenden Abläufe stammen aus bewegtem Bild.

**Film 1 (Stufe 1, 67 s, Kontaktbogen `katalog/film1_1bps.jpg`).** Abwechselnd Leon und Elza, nie beide zusammen.
1. 117 Dach, Cut 4 (0:00–0:07): Leon steht allein am Geländer und spricht zwei Sätze (T05, T06), danach Cut 3, 2, 1: Leon läuft über den Landeplatz.
2. 118 Gang (0:13.2–0:19.4): Leon erschießt zwei Zombies.
3. 11E (0:19.4–0:24): Leon gegen Zombies, Blutlache.
4. 107 Büro (0:24–0:32.8): Elza gegen drei Zombies.
5. 104 (0:32.8–0:37): Elza vor dem Rolltor, ein kriechendes Wesen, Blutlache.
6. 119 (0:37–0:42.6): Leon findet den Schalter (T08).
7. 103 (0:42.6–0:45.9): Elza schießt durch das intakte Gitter.
8. 120 (0:45.9–0:51.6): Leon setzt den Flammenwerfer ein.
9. 117 Dach (0:51.6–0:59.1): Elza wehrt Krähen ab.
10. 10B (0:59.1–1:05.5): Leon läuft die Zufahrt entlang zur Leiter.

**Film 2 (Stufe 2, 37,6 s).** Nur Leon als Spielfigur.
1. Kanal, Cut 2 von 200 oder 20B (0:01–0:10.6), Folge `katalog/film2_kanal_folge.jpg`: Leon und Marvin stehen rechts auf dem Sims. Ada steht schon im Wasser (0:01.8), watet zur linken Seite (0:02.6) und steigt dort auf den Sims an der Leiter (0:03.4, 0:04.2). Danach steigt Marvin ins Wasser (0:04.2), watet hinüber (0:05.0–0:07.4, Spritzer) und steigt links hinauf (0:08.2), zuletzt steht er links (0:09.0–0:09.8). Leon bewegt sich die ganze Zeit nicht. Kein Untertitel, keine Sprache.
2. Raum mit rotem Rad (0:10.6–0:15.6): Leon gegen eine Riesenspinne, sie platzt grün.
3. Tunnel mit oranger Tür (0:15.6–0:21.1): Leon gegen Zombies in Khaki.
4. 200 Cut 13 (0:21.1–0:27.1): Leon gegen Zombies.
5. 20B grauer Gang (0:27.1–0:37.5): Leon gegen eine Spinne, läuft auf die Kamera zu, eine weiße Wolke folgt ihm und bricht in Cut 7 aus der Tür.

**Film 3 (Stufe 3, 47,5 s).** Nur Elza als Spielfigur.
1. 306 Cut 5 (0:00–0:12.8): Sherry, John und Elza stehen am Lüftungsgitter. Sherry spricht Elza an (T19), Elza fragt nach (T20), Sherry zeigt hinein (T21), kniet sich hin und kriecht ins Gitter (ab 0:08). Elza ruft ihr nach (T22) und dreht sich zur Kamera. John bleibt stehen. Ton und Untertitel laufen gleich (`katalog/ton_film3_hinterhof306.jpg`).
2. 300 Cut 2 (0:12.8–0:17.7): Elza gegen Zombies am Umbrella-Lkw.
3. 301 (0:17.7–0:28.3): Elza gegen Zombies.
4. 306 Cut 3 (0:28.3–0:33.6): Elza gegen Zombie und Hunde.
5. 307 Cut 9 (0:33.6–0:36.3): Elza an der Steuerbox des Aufzugs.
6. 307 Cut 8, 10 (0:36.3–0:47.4): Das Aufzugtor öffnet sich, Birkin in Menschform mit Stange steht darin und tritt heraus, Nahaufnahme des Gesichts.

**Film 4 (Stufe 4 und 5, 51,4 s).** Leon und Elza abwechselnd.
1. 50C (0:00.5–0:07): Leon läuft durch die Halle.
2. 50B (0:07–0:10.7): Leon geht zur roten Maschine, weiße Form daneben (K086).
3. 50E Cut 8, 5 (0:10.7–0:19.7): Leon an der Konsole, eine große Kreatur fällt herab, Leon rennt davon, die Kreatur steht am Geländer.
4. 408 (0:19.7–0:28.7): Elza gegen ein dunkelrotes, affenartiges Wesen.
5. 504 (0:28.7–0:34.4): Elza vor am Boden kriechenden Gestalten.
6. 514 (0:34.4–0:41.5): Leon gegen brennende Zombies.
7. 512 (0:41.5–0:45): Elza gegen einen brennenden Zombie, Rotfilter.
8. 50A Cut 12 (0:45–0:51.3): Birkin packt ein gelb-schwarzes Wesen, wirft es und kommt auf die Kamera zu.

**Video B, Teile außerhalb der Filme (Bioflames).** Mit Story-Bezug: 103 Gitterdurchbruch bei Leon (B 2:12.8–2:33, K006), Rolltor 104 bei Leon (B 6:36–6:58, K010), Elza im Moosgang 207 (B 5:41–6:01), Elza in 506 mit Lichtwechsel (B 10:43.5–11:25.5), Leon gegen Birkin in 307 (B 13:09–13:56.9), Trailer mit vier Tafeln (B 9:33–10:06). Sonst Kampf- und Laufszenen.

## 5. Raumliste des 80%-Materials (d)

Spalten: Disc = Datei auf der 40%-Disc (RDT-Varianten, BSS). Port = RDT im Port (alle 120 Raumnummern sind mit beiden Varianten vorhanden, `re15_port/shared_assets/PSX`) und ob ein Port-Hintergrund das Bild trifft. Status „fehlt im Port“ heißt: Der Schauplatz hat kein Bild im Port, auch wenn es die Raumnummer gibt.

| Ort | Szenario im Material | Quellen | Disc 40% | Port | Status |
|---|---|---|---|---|---|
| 102 Büro | Elza, Leon | A#001, #026, #030, #091 | 1020, 1021 | Treffer | vorhanden |
| 103 Halle mit Gitter | Elza, Leon | F1, B 2:12.8 ff., Trailer | 1030, 1031 | Treffer | vorhanden |
| 104 Gang mit Rolltor | Elza, Leon | F1, A#002 ff., B 6:01 ff. | 1040, 1041 | Treffer | vorhanden |
| 107 Büro | Elza, Leon | F1, B | 1070, 1071 | Treffer | vorhanden |
| 109 Hof mit Transporter | Elza, Leon | A#035, #096, #100, #173 | 1090, 1091 | Treffer | vorhanden |
| 10A | Elza | A#020 | keine Datei | E0,98 | **fehlt auf Disc**, im Port |
| 10B Zufahrt | Elza (mit John), Leon | A#032–#034, #107, F1 | 10B0, 10B1 | Treffer | vorhanden |
| 110 | Elza, Leon | A#006, #010, #088 | 1100, 1101 | Treffer | vorhanden |
| 114 Konferenzraum | Elza (mit John) | A#012, #018 | keine Datei | S, E0,83 (schwach) | **fehlt auf Disc**, im Port |
| 115 Chefbüro | Elza, Leon | A#016, #017, #087, #089, B | 1150, 1151 | Treffer | vorhanden |
| 117 Dach | Elza, Leon | F1, A#015, #082, #083 | 1170, 1171 | Treffer | vorhanden |
| 118 Gang | Leon | F1, B 7:03.5 | nur 1180, BSS Rohfassung | Treffer | vorhanden |
| 119 Schießstand | Elza, Leon | F1, A, B | 1190, 1191 | Treffer | vorhanden |
| 11A | Elza | A#022 | 11A0, 11A1 | Treffer | vorhanden |
| 11B Garage | Elza, Leon (mit Marvin, Ada) | A#021, #041, #094, #169–#172 | 11B0, 11B1 | Treffer | vorhanden |
| 11C Parkgarage | Leon (mit Ada) | A#119–#129 | 11C0, 11C1 | Treffer | vorhanden |
| 11E | Elza, Leon | F1, A#135, #136, B | 11E0, 11E1 | Treffer | vorhanden |
| 120 Leichenhalle | Elza, Leon | F1, A#130–#134, B | 1200, 1201 | Treffer | vorhanden |
| 121 | Leon | A#097 | 1210, 1211 | Treffer | vorhanden |
| 122 | Leon | A#098 | nur BSS | Treffer | **fehlt auf Disc als Raum**, im Port |
| 123 | Elza | A#023 | nur BSS | E1,03 | **fehlt auf Disc als Raum**, im Port |
| 200 Kanal | Elza (mit Sherry), Leon (mit Marvin, Ada) | A#036–#040, #103–#105, F2 | 2000, 2001 | Treffer | vorhanden |
| 207 Moosgang | Elza | B 5:41–6:01 | nur 2070, Bilder vorhanden | **kein Treffer** (Port-207 zeigt andere Bilder, die Mod entfernte die Moosräume) | **fehlt im Port** |
| 20A | Elza-Block | A#046 | keine Datei | E0,92 (schwach) | **fehlt auf Disc**, im Port |
| 20B grauer Gang | Elza, Leon | F2, B | 20B0, 20B1 | Treffer (im Port als 207 Cut 0–3 umnummeriert) | vorhanden |
| Raum mit rotem Rad | Leon | F2 0:10.6–0:15.6 | keine | kein Treffer, Fan-Raum 206 ähnlich, anderer Blick | **fehlt im Port und auf Disc** |
| Tunnel mit oranger Tür | Leon | F2 0:15.6–0:21.1 | keine | kein Treffer | **fehlt im Port und auf Disc** |
| Rippen-Tunnel | Leon | B 4:05.5–5:03.5 u. a. | keine | kein Treffer, Fan-Raum 202 bildet die Art nach, andere Bilder | **fehlt im Port und auf Disc** |
| 300 Fabrikhof | Elza (mit John), Leon (mit Mann) | A#044, #048, #118, F3, B | 3000, 3001 | Treffer | vorhanden |
| 301 | Elza | F3, A#068, #069 | 3010, 3011 | Treffer | vorhanden |
| 302 oder 30D | Elza, Leon | A#070, #071, #115 | nur 3020; 30D keine | Treffer | 302 Elza-Fassung **fehlt auf Disc** |
| 303 | Elza | A#057 | 3030, 3031 | Treffer | vorhanden |
| 304 | Elza, Leon | A#062–#066, #113, #114 | 3040, 3041 | Treffer | vorhanden |
| 305 Rechnerraum | Elza | A#060, #061, #065 | keine Datei | M14, M12 | **fehlt auf Disc**, im Port |
| 306 Hinterhof | Elza (mit John, Sherry), Leon (mit Marvin) | F3, A#049, #116 | 3060, 3061 | Treffer | vorhanden |
| 307 Lastenaufzug | Elza, Leon, Birkin | F3, A#073–#081, #117, #140–#142, B | 3070, 3071 | Treffer | vorhanden |
| 308 | Leon-Block | A#166, #167 | 3080, 3081 | Treffer | vorhanden |
| 309 | Sherry allein, Elza (mit John, Sherry) | A#050–#052, #054, #055, #059 | 3090, 3091 | Treffer | vorhanden |
| 30A | Elza | A#067, #072 | keine Datei | M8 | **fehlt auf Disc**, im Port |
| 30C Tresen | Elza (mit Marvin), Sherry allein, Leon | A#053, #058, #139 | keine Datei | M33, E0,98 | **fehlt auf Disc**, im Port |
| 30E | Elza | A#056 | 30E0, 30E1 | Treffer | vorhanden |
| 400 | Leon-Block | A#164 | 4000, 4001 | Treffer | vorhanden |
| 408 grauer Metallgang | Elza | F4 0:19.7–0:28.7 | nur BSS (Rohbau) | M40802, 95 Inlier | **fehlt auf Disc als Raum**, im Port |
| 501 | Leon | A#161 | 5010, 5011 | Treffer | vorhanden |
| 503 | Leon-Block | A#160 | 5030, 5031 | Treffer | vorhanden |
| 504 | Elza | F4, B | 5040, 5041 | Treffer | vorhanden |
| 506 | Elza | B | 5060, 5061 | Treffer | vorhanden |
| 509 roter Gang | Leon, Frau im Kittel, Riesenform | A#144, #145, #168 | keine Datei | M24, E0,95 | **fehlt auf Disc**, im Port |
| 50A Hangar C-2 | Birkin | F4 0:45–0:51.3 | 50A0, 50A1 (Rohfassung) | M50A12, 23 Inlier | vorhanden |
| 50B Maschinenhalle | Leon | F4 0:07–0:10.7, A#155 | keine Datei | M50B02, 59 Inlier | **fehlt auf Disc**, im Port |
| 50C | Leon | F4, A#162, #163 | 50C0, 50C1 | Treffer | vorhanden |
| 50E | Leon | F4 0:10.7–0:19.7 | keine Datei | M50E08 33, M50E05 28 | **fehlt auf Disc**, im Port |
| 512 | Elza | F4, B | 5120, 5121 | Treffer | vorhanden |
| 514 | Elza, Leon | F4, A#156–#158, B | 5140, 5141 | Treffer | vorhanden |
| Bahnsteig-Halle (orange, roter Zug) | Leon, Riesenform | A#143, #150 | keine | kein Treffer in 1650 Bildern; Raum 600 hat andere Kameras | **Kamera fehlt im Port** |
| 600 Bahnsteig | Leon (mit Frau im Kittel) | A#152–#154 | keine Datei | M8, M15, E1,13 | **fehlt auf Disc**, im Port |
| 601 | Leon | A#149 | keine Datei | M30 | **fehlt auf Disc**, im Port |
| 603 Führerstand | Leon (mit Frau im Kittel) | A#146, #147 | 6030, 6031 | Treffer | vorhanden |
| 604 Maschinenraum | Leon | A#148, #151 | 6040, 6041 | Treffer, die Disc-604-Bilder liegen im Port als 603 Cut 6–12 | vorhanden, umnummeriert |
| roter Gang „CORRIDOR 2-A“ | Leon-Block | A#159 | – | Sicht | offen |
| Trailer: grüner Gang mit Gittertor | wohl Leon | B 9:49–9:51 | keine | kein Treffer | **fehlt im Port und auf Disc** (Trailerstand) |
| Trailer: blauer Gang mit Leiche | wohl Leon | B 9:51–9:53.5 | keine | kein Treffer | **fehlt im Port und auf Disc** (Trailerstand) |
| Trailer: blauer Gang mit zwei Türen, gelbe Wand mit Gitterboden | Leon | B 10:00–10:02.8 | keine | kein Treffer | **fehlt im Port und auf Disc** (Trailerstand) |
| Trailer: Moosgang mit Tafel 恐怖再び | offen | B 10:02.8–10:05.8 | kein Treffer, auch nicht gegen Disc 207 | kein Treffer | **fehlt im Port und auf Disc** (Trailerstand) |

Belegbild der fehlenden Orte neben den nächstliegenden Port-Bildern: `katalog/orte_ohne_port.jpg`. Eigene SIFT-Prüfung aller dieser Orte: 0 Inlier gegen 1650 Hintergründe (`katalog/daten/raumabgleich_orte_ohne_treffer.json`, `raumabgleich_trailer.json`).

**Gänzlich fehlende Schauplätze im Port** (für den Nutzer): Moosgang 207 (Elza gegen Spinnen), Raum mit rotem Rad (Leon gegen Riesenspinne), Tunnel mit oranger Tür und Rippen-Tunnel (Leon gegen Zombies in Khaki), die Bahnsteig-Halle der Riesenform-Szene (A#143). Die Trailer-Orte können aus einem früheren Stand stammen.

## 6. Ton-Befund (e)

**Vorgehen.** Tonspuren mit ffmpeg nach 16 kHz mono. Sprachdetektor Silero-VAD (Schwellen 0,5, 0,3, 0,2). Spracherkennung mit Whisper large-v3-turbo und Whisper medium.en, je auf der 480p- und der 360p-Fassung. Tonhöhe per Autokorrelation. Spektrogramme mit eingetragenen Untertitel- und Sprachzeiten. Hören war nicht möglich, also zählt nur, was mehrere unabhängige Messungen zugleich zeigen. Werkzeuge in `katalog/werkzeuge/`, Rohdaten in `katalog/daten/`.

| Quelle | Lautheit | Sprache (VAD) | Inhalt |
|---|---|---|---|
| Film 1 | −17,2 LUFS | 0:02.34–0:04.48, 0:05.19–0:06.46 | Leon auf dem Dach, zwei Sätze. Sonst Musik und Geräusche |
| Film 2 | – | keine (drei Blitzer unter 0,5 s, im Spektrogramm Wasserspritzer und Musik) | Kanalszene ohne Dialog, Kämpfe |
| Film 3 | −17,2 LUFS | 0:01.31–0:09.25, 0:11.46–0:13.34; dazu 0:20.6–0:23.1 Fehlalarm (Spektrogramm zeigt Musik und Schüsse, `katalog/ton_film3_301_musik.jpg`) | Hinterhof 306, vier Sätze |
| Film 4 | – | keine | Musik, Geräusche |
| Video B | −15,0 LUFS | nur 0:19.65–0:23.61 und 3:19.75–3:30.85, also die Filmstellen. Auch bei Schwelle 0,2 sonst nichts außer dem Fehlalarm aus Film 3 und einem Blitzer von 0,4 s | Ton der Filme ist in B enthalten (Hüllkurve r 0,92–0,95) |
| Video A | – | keine (bei Schwelle 0,2 nur Blitzer bis 1,3 s in der Musik) | Musikschleife (laut Vorarbeit aus dem 40%-Stand) |

**Die sechs Sätze** (Sprecherwahl aus dem Untertitelinhalt, Tonhöhe stützt sie):

| Stelle | Sprecher | Gesprochen (Englisch, Capcom) | Untertitel gleichzeitig | Tonhöhe |
|---|---|---|---|---|
| F1 0:02.3–0:04.5 | Leon | Oh, that's just freakin' great! | おい　冗談だろ？ | 145 Hz, Mann |
| F1 0:05.0–0:06.3 | Leon | Now what am I gonna do? | …クソッなんてこった！ | 157 Hz |
| F3 0:01.2–0:03.0 | Sherry | Elza, I know a secret way in! | …エルザ／私 ヒミツの入り口を知ってるの | 340 Hz, Kind |
| F3 0:03.8–0:05.4 | Elza | A secret entrance? | ヒミツの入り口？ | 195 Hz, Frau |
| F3 0:06.2–0:09.0 | Sherry | It's here! Look, I can get inside through this hole! | ここよ　この中から入れるの | 327 Hz |
| F3 0:11.5–0:12.8 | Elza | Sherry, wait! | シェリー！　待って！ | 333 Hz, Ausruf |

Belegbilder: `katalog/ton_film1_dach.jpg`, `katalog/ton_film3_hinterhof306.jpg`. Sprachkennung Whisper: Englisch 0,91 bis 0,99.

**Warum das tragfähig ist.** (1) Der Sprachdetektor schlägt in 14 Minuten nur an diesen Stellen an. (2) Beide Erkenner liefern auf beiden Kodierungen denselben Wortlaut, einzige Abweichung „Dana's hole“ statt „through this hole“ in einem von vier Läufen. (3) Die Sprachabschnitte liegen jeweils im Zeitfenster des passenden Untertitels (Wechsel gemessen bei F1 0:04.17, F3 0:03.57, 0:06.07, 0:09.74). (4) Der Inhalt der englischen Sätze deckt sich mit den japanischen Untertiteln, in 306 Satz für Satz. (5) Die Tonhöhen passen zu Mann, Kind und Frau. **Gegenprobe:** In Musik ohne Sprache erfindet Whisper Floskeln wie „Oh, my God.“, „Thank you.“, „BANG!“ oder 「ご視聴ありがとうございました」 (`katalog/daten/whisper_scan_videoB_mit_halluzinationen.txt`). Solche Stellen haben keinen VAD-Treffer, keinen Untertitel und wechseln zwischen den Läufen. Sie zählen nicht.

**Folgen.**
- Der 80%-Stand hatte englische Sprachausgabe mit japanischen Untertiteln, wie später das japanische Biohazard 2. Für Dach 117 und Hinterhof 306 kennen wir damit Capcoms englischen Wortlaut. Er ist näher am Ziel als jede Übersetzung des Japanischen.
- Die Untertitel übersetzen die Sprache frei: „Oh, that's just freakin' great!“ steht über 「おい 冗談だろ？」.
- Die 40%-Disc hat keinen Sprachordner (`Version_40%/PSX` ohne VOICE). Der Port- und Mod-Baum hat `VOICE/` mit 53 Dateien, darunter `r1170_*`. Die Datei `VOICE/r1170_m05_TRIMMED_DEBUG.wav` ist **nicht** der Capcom-Ton aus Film 1: andere Worte, Wellenform-Korrelation 0,08. Die Herkunft der übrigen Mod-Sprachdateien ist hier nicht geprüft (Spur ST80-P).
- Szenenmusik: Alle vier Filme und die Bioflames-Teile haben durchgehend Musik und Geräusche. Welche Stücke das sind, ist nicht bestimmt (kein Vergleich mit den Musikdateien). Story-Inhalt tragen nur die sechs Sätze.
- Video A hat keinen Ton aus dem 80%-Stand.

## 7. Fakten STAGE3-6 (für die Dialoge des Nutzers)

Nur, was das 80%-Material zeigt. „Nicht gezeigt“ heißt nicht „gab es nicht“.

**300 Fabrikhof.** Elza und John sind zusammen dort, Elza liegt in einem Foto am Boden, John steht daneben (K055). John sagt sinngemäß, die Fabrik dürfte stillliegen (T16). Elza kämpft dort gegen Zombies am Umbrella-Lkw mit Leichen (K056). Leon ist ebenfalls dort, neben einem hockenden Mann bei einer Leiche, Text 「みんな ラクー…」 „Alle [sind] Raccoon-…“ (K057, T17). Wer der Mann ist, ist offen.
**301, 303, 30A, 30E.** Elza ist dort, kämpft, keine weiteren Figuren.
**302/30D.** Elza bei einem Streifenwagen. Leon mit einer zweiten, unbestimmten Figur (unsicher).
**304.** Elza und Leon jeweils dort, Gore-Wesen.
**305.** Elza an einem Rechner, der Bildschirm zeigt „Now Running…“, „Break“ und 「システムエラー」 (Systemfehler) (K064, T18).
**306 Hinterhof.** Sherry, John und Elza am Lüftungsgitter. Sherry spricht Elza beim Namen an und kennt einen geheimen Eingang, sie kriecht allein hinein, Elza ruft ihr „Sherry, wait!“ nach, John bleibt stehen (K065, T19–T22, mit Capcom-Ton). Danach kämpft Elza dort noch gegen Zombie und Hunde (K066). Leon und Marvin stehen zusammen an der orangen Tür (K067).
**307 Lastenaufzug.** Elza an der Steuerbox, Text zum Aufzug (T23). Das Tor öffnet sich, Birkin in Menschform mit Stange tritt heraus, Kampf mit Elza, Elza liegt zeitweise am Boden (K068). Auch Leon kämpft in 307 gegen Birkin, Marvin ist dabei nicht im Bild (K069). Text bei Leon 「ホール…」 „Die Halle…“ (T24).
**308.** Nur Schemen (unsicher).
**309.** Sherry ist allein unterwegs, in fünf Kameras, an einer Kiste beim Kasten mit 「電げんがある」 „Da ist eine Stromversorgung“ (K071, T25). In einem anderen Foto stehen Elza, John und Sherry zusammen, John: knapp entkommen, fast gebissen worden (K072, T26).
**30C Tresen.** **Elza trifft Marvin** am Tresen (K074, kein Text). Sherry ist allein in 30C (K075). Leon steht am Tresen in 30C oder 30D (K076). 30C liegt damit auf Leons, Elzas und Sherrys Weg.
**400.** Nur eine ferne Figur.
**408.** Elza kämpft gegen ein dunkelrotes, affenartiges Wesen (K079).
**501, 503.** Leon in 501. 503 nur eine Figur.
**504, 506, 512.** Elza kämpft dort gegen Zombies, in 506 wechselt das Licht auf Rot, in 512 brennt ein Zombie.
**509.** Leon ist dort. Die Frau im Kittel und die Riesenform sind im selben Bild, mit Feuer (K084). Eine kleine Figur steht an einer Konsole.
**50A Hangar C-2.** Birkin kämpft mit einem gelb-schwarzen Wesen und kommt auf die Kamera zu, keine Spielfigur (K085).
**50B, 50C.** Leon läuft durch beide Hallen. In 50B eine weiße, ortsfeste Form neben der Maschine, Figur nicht belegt (K086).
**50E.** Leon an der Konsole, eine große Kreatur fällt herab, Leon flieht (K088).
**514.** Leon und Elza jeweils gegen brennende Zombies.
**Bahnsteig.** Leon trifft in der Bahnsteig-Halle auf die Riesenform, roter Zug im Hintergrund (K093). In 600 stehen Leon und die Frau im Kittel zusammen (K094).
**601.** Leon allein.
**603 Führerstand.** Leon und die Frau im Kittel, sie steht am Pult (K096).
**604 Maschinenraum.** Leon allein, Foto nach Debug-Sprung, über die Besetzung sagt es nichts (K097).

**Querschnitt STAGE3-6.**
- Kein Bild zeigt Leon und Elza zusammen. Kein Bild zeigt Elza in Stufe 6.
- Wer wen kennt: Sherry nennt Elza beim Namen (T19, Ton). Elza nennt Sherry beim Namen (T22, Ton). Elza und Marvin begegnen sich in 30C, ein Gespräch ist nicht überliefert. Die Frau im Kittel ist ab 509 bei Leon, ein Name fällt nirgends.
- Birkin erscheint in Menschform in 307 bei beiden, im Hangar 50A gegen ein anderes Wesen, die Riesenform am Bahnsteig und in 509.
- Sherry handelt allein: Gitter in 306, dann allein in 309 und 30C.

## 8. Offene und unlesbare Stellen (f)

| Stelle | Was offen ist | Nächster Weg |
|---|---|---|
| T05 Japanisch | 冗談だろ？ gegen 危険だぞ！ und andere: Messung trennt nicht | Bessere Quelle des Films (Complete Disc selbst) |
| T03 Anfang, T07 zwei Kanji, T12 葉, T15 Rest, T17 Zeile 2, T24 Rest, T25 Rest, T26 Zeile 2, T27 Zeile 1 | unlesbar oder nur Lesart | Original-Fotos in höherer Auflösung (Bioflames) |
| T04 「……レオン」 | Sprecher: Elza, der Verletzte oder der Chef | keine Tonspur zu diesem Foto |
| T14 | 時計 gegen 時間, 手紙 | wie oben |
| K004 | Name des Verletzten (wahrscheinlich Roy), Ort ohne Hintergrund | – |
| K057 | Mann neben Leon im Fabrikhof: John-artige Kleidung, nicht sicher | – |
| K086 | weiße Form in 50B: Figur oder Bildteil | bessere Filmquelle |
| K088, K093 | Identität der Kreaturen (Nutzerentscheide E17 und Bahnsteig-Form gelten für die Gestalt) | – |
| K098, K100, K060, K070, K078, K081 | Figuren unklar | – |
| Frau im Kittel | Name nirgends genannt | – |
| A#084 | Sprecher von T27 | – |
| Musik | Stücke der Filme nicht bestimmt | Vergleich mit Disc- und RE2-Musik |
| Trailer-Orte | ob 1.5-Räume oder ältere Stände | – |
| 27 Fotos ohne Ort | siehe Gesamtverzeichnis | – |

## 9. Gegenüber der Vorarbeit: neu und berichtigt

| Punkt | Vorarbeit | Jetzt |
|---|---|---|
| Ton der Filme und von B | „nicht ausgewertet“; BERICHT §12 empfahl Abhören | Englische Sprachausgabe in F1 und F3, sechs Sätze mit Wortlaut. Sonst keine Sprache |
| Dachsätze Leon | Japanisch teils lesbar, „Wen Leon anruft, zeigt das Bild nicht“ | Leon spricht sie allein, Capcom-Wortlaut Englisch bekannt. Japanisch Zeile 2 gemessen (wahrscheinlich), Zeile 1 nur おい gemessen |
| Zeiten Dachuntertitel | B 0:19.2–0:21.6 und ab 0:21.9 | Untertitelwechsel gemessen: F1 0:01.2–0:04.15 und 0:04.17–0:06.24 (= B 0:18.3–0:21.3, 0:21.3–0:23.4) |
| 480p-Filme | nicht im Bestand | Gleiche Aufnahme wie 360p, ohne Vorspann. Alle vier Filme stecken in B |
| Moosgang 207 | „Disc-Treffer 0,84–0,93, keine ROOM2071“ | Auch im Port fehlt der Moosgang als Bild (0 Port-Inlier, die Mod entfernte die Moosräume): **fehlt im Port** |
| 50B | „Port 50B Cut 2, Wert 0,72“ | SIFT 59 Inlier: im Port vorhanden. Neu: weiße Form neben Leon, offen |
| 50E, 408, 50A, 601, 600, 30C | Port-Treffer aus Kantenvergleich | Mit SIFT bestätigt (33/28, 95, 23, 27, 11, 31 Inlier) |
| Kanalszene | „Ada watet zur Leiter, Marvin folgt“ | Bestätigt, Ablauf mit Zeiten, ohne Dialog und ohne Sprache |
| Figurenwahl | nur „PROFILE 178 Cm 21 A“ | Vollständig: Alter, Größe, Blutgruppe, Elzas Gewicht |
| Trailertafeln | im Dossier gelistet, nicht übersetzt | Übersetzt, als Story-Aussage geführt |
| vlcsnap-Bilder | nicht zugeordnet | A#143, A#145, F4 0:14.25–0:19.65 |
| documentation/ | „vier Complete-Disc-Filme“ | Dazu Port-Aufnahmen des Nutzers, ausgeschlossen |
| Mod-Sprachdatei Dach | – | `VOICE/r1170_m05_TRIMMED_DEBUG.wav` ist eine andere Aufnahme als Capcoms Ton |

## 10. Nachprüfen

- Gesamtverzeichnis: `katalog/daten/eintraege_alle.csv` (353 Zeilen, Semikolon).
- Kontaktbögen der Filme, 1 Bild je Sekunde: `katalog/film1_1bps.jpg` bis `film4_1bps.jpg`.
- Story-Fotos Video A: `katalog/videoA_story_1.jpg`, `videoA_story_2.jpg`, Lupen `videoA_lupe_084_118_142_165.jpg`, `videoA_078_093_115.jpg`.
- Figurenwahl `katalog/videoB_figurenwahl.jpg`, Trailertafeln `katalog/videoB_trailertafeln.jpg`, Kanalfolge `katalog/film2_kanal_folge.jpg`, 50B `katalog/film4_50B_weisse_form.jpg`, Dachuntertitel gegen Disc-Font `katalog/untertitel_dach_film1.jpg`, fehlende Orte `katalog/orte_ohne_port.jpg`.
- Ton: `katalog/ton_*.jpg`; `katalog/daten/vad.json`, `words_whisper-large-v3-turbo.json`, `words_whisper-medium.en.json`, `whisper_whisper-large-v3-turbo.json`.
- Werkzeuge `katalog/werkzeuge/`: `cd_align.py` (Filme gegen 360p und B), `ton_vgl.py` (Ton gegen B und 360p), `cd_cuts.py`, `sheet.py`, `avgframe.py`, `kamm.py` und `rang.py` (Glyphenmessung, nutzen `s40/pruefung/va/ocr.py` der Vorarbeit), `vgl.py`, `match.py` (SIFT gegen Disc- und Port-Hintergründe), `vglbg.py`, `vad.py`, `whisper_run.py`, `whisper_words.py`, `whisper_scan.py`, `f0.py`, `spek.py`, `tonbeleg.py`. Pfade zu Zwischendateien zeigen auf das Arbeitsverzeichnis der Sitzung. Whisper-Modelle über Hugging Face (`openai/whisper-large-v3-turbo`, `openai/whisper-medium.en`), Silero-VAD über `torch.hub`.
