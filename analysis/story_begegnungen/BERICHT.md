# Leon, Elza und ihre Begleiter: Was in den Capcom-Daten wirklich steht

Stand 2026-10-31, nach unabhängiger Prüfung durch sechs Prüfer und einen Kritiker ([Dossiers](pruefung/)). Geprüft: die unveränderte 40%-Disc (`Version_40%/`), die zwei Videos in `Version_80%/`, die vier Complete-Disc-Filme (`documentation/`), Kamiyas Aussagen von 1998 und 2019, dazu Project Umbrella und Forenquellen als Nebenquellen. Alle 291 Textzeilen der Disc stehen mit Übersetzung, Sprecher und Auslöser in [TEXTE_40.md](TEXTE_40.md). Höchstens 217 davon können auf der Disc laufen (Abschnitt 12). Fotos aus Video A heißen #000 bis #173.

Belegstärke: **hart belegt** heißt Bytes, Scan oder sichtbares Bild. **Wahrscheinlich** heißt mehrere passende Indizien ohne direkten Beweis. **Nicht belegt** heißt keine Quelle.

## 1. Die Antworten

**Treffen sich Leon und Elza?** Nein, soweit die Quellen reichen, für das Ende bleibt es offen.

| Aussage | Stärke | Beleg |
|---|---|---|
| Kein Skript der Disc führt beide zusammen | Hart belegt für den Stand 6.11.1996 | 96 RDT-Dateien, keine Szene mit beiden |
| Kein Bild zeigt beide in einer Szene | Hart belegt für das vorhandene Material | 174 Fotos, Video B, vier Complete-Disc-Filme. Nur das Auswahlmenü zeigt beide, in getrennten Feldern |
| Capcom plante zwei getrennte Geschichten | Hart belegt als Aussage | Kamiya, Guide Book S. 261: 「ひとり1本」. Kamiya 2019: „nicht parallel, aber ohne Zapping, nur zwei Geschichten“ |
| Sie begegnen sich im fertigen Spiel nie | Wahrscheinlich | Wörtlich nur in Sekundärquellen, Resident Evil Archives S. 218–220 ungeprüft |
| Was im Finale passiert | Nicht belegt | 603 und 604 ohne Text, 605–607 ohne Dateien, kein Video zeigt ein Finale |

**Treffen Leon oder Elza den Begleiter des anderen?** Ja, das ist der wichtigste Fund. Auf der Disc läuft davon aber nur wenig ab.
- **Leon sieht Sherry** in der Garage 11B und ruft ihr nach. Das läuft auf der Disc, ein Gespräch gibt es nicht.
- **Leon, John und Sherry** stehen im Maschinenraum 604 im selben Raum. Leons Blick zu den beiden ist geschrieben, aber nicht ausgelöst.
- **Elza und Marvin** stehen im Spätstand in einem Foto zusammen (#058, Tresen in 30C, ohne Text). Auf der Disc steht Marvins Typ in Elzas 604 bei der Klappliege.
- **Elza und Ada:** Kamiya bestätigt, dass Ada im Szenario der Protagonistin kurz auftauchte (Guide Book S. 261). Auf der Disc ist das ein Zweig in 603 an einem Flag, das nie gesetzt wird.
- **Marvin, John und Sherry** stehen in 604 zusammen, in beiden Disc-Szenarien. Für den Spätstand ist das nicht belegt: #151 zeigt Marvins Platz frei, das Foto entstand nach einem Debug-Sprung.

**Erfährt man vom anderen nur über Dialoge?** Nein, vor allem über den Zustand der Welt.
- Es gibt **keine erreichbare Disc-Zeile**, in der eine Seite die andere beim Namen nennt.
- Leon findet Tür, Lüftungsgitter und Licht so vor, wie Elzas Gruppe sie hinterlassen hat. Elza findet den Transporter offen und ohne Feuer (Abschnitt 4).
- **Im Spätstand fällt Leons Name einmal im Elza-Lauf:** Foto #016 zeigt im Chefbüro den Untertitel 「……レオン」 (Abschnitt 11). Wer spricht, ist offen, wahrscheinlich der Chef. Dass Elza Leon kennt, folgt daraus nicht.
- エルザ fällt in keinem der 91 Leon-Fotos.

**Eine Gesamtreihenfolge lässt sich nicht ableiten.** Die Spuren laufen in beide Richtungen: In der Fabrik ist Elzas Gruppe zuerst da, am Transporter in 109 Leon. Leon startet auf dem Dach, Elza in der Halle 103, ihre Wege überlappen also. Am Ende führen beide Wege über dieselben Türen zum Zug. Das Zapping-System ist das nicht: Die Spuren stehen fest in den Raumdateien, nichts wird aus dem anderen Lauf übernommen, und Kamiya trennt selbst: nicht parallel, aber kein Zapping.

## 2. Quellen, die man nicht vermischen darf

| Quelle | Was es ist | Beleg | Aussagekraft |
|---|---|---|---|
| **40%-Disc** `Version_40%/` | Capcom-Stand, 334 Dateien im Image, 96 RDT-Dateien zu 51 Raumnummern, alle Texte Japanisch. Laut Hidden Palace der Stand der „PlayStation Museum“-Fassung | Exe-Stempel `CAPCOM-Nov  6 1996-13:09:19-CAPCOM`, ISO-Datum 1996-11-06, Hidden Palace | Hart. Stand: Text, Skripte, Modelle |
| **Video A** `Biohazard 1.5 Final Build.mp4` | **Keine Filmaufnahme.** Diashow von Enigmatism415 (2013): 173 Bioflames-Fotos und ein SurvivHor-Foto, je 0,8 s, Musik aus dem 40%-Build | 174 Standbilder gemessen, YouTube qnWhK82FzdU | Hart für das einzelne Bild. Die Bildfolge stammt vom Uploader, die Fotos aus einer Debug-Sitzung (#031, #148, #159) |
| **Video B** `Resident Evil 1.5 (All Footage).mp4` | Spielaufnahmen gemischter Herkunft, Gesamtquelle ungeklärt | Wasserzeichen BIOFLAMES und SURVIVHOR | Hart für das, was man sieht. Bildqualität oft schlecht |
| **Complete-Disc-Filme** `documentation/` | Vier Capcom-Filme, als Fan-Mitschnitt, in Video B enthalten | Abgleich r bis 1,00 | Hart für Bild, Ton noch nicht ausgewertet |
| **Unser Repo-Baum** `info/Re1.5/` | **Fan-Mod „Magic Zombie Door“ von MartinBiohazard** auf Basis Team IGAS, Update 25.01.2025 | `info/Re1.5/README.TXT`, Disc-Image von 2023, 751 Dateien, 254 Dateien und 3630 Exe-Worte anders | **Kein Capcom-Beleg für Story** |

Die Mod übersetzt nicht nur. Ihr Baum hat 144 RDT-Dateien mehr als die Disc (240 gegen 96), wie viel davon aus der Team-IGAS-Basis und wie viel von Fans stammt, ist nicht getrennt. Sie schreibt Szenen neu. Die README nennt das offen, zum Beispiel: „Added Marvin's cutscene in room 30C (Elza) (based on Inflames's screenshot)“, „Added ending (by Richard Mandel)“, „Added extended cutscene in TRAIN M. CAR (604) (Leon)“, „Added Leon and Elza models in room 604“, „Added helpless animation for Leon when he sees Ada die“.

**Folge:** Die alte Auswertung in `analysis/video_80prozent/BEFUND.md` §5.3 und unsere Story-Regel stützten sich auf englische Dialoge dieser Mod. Die sind Fan-Text (Abschnitt 9).

## 3. Wer wem begegnet

| Paar | 40%-Disc | Spätstand (Fotos, Filme) | Urteil |
|---|---|---|---|
| **Leon – Elza** | Kein Treffen, kein Name in einer erreichbaren Zeile | Kein gemeinsames Bild. #016: レオン im Elza-Lauf | **Kein Treffen belegt** |
| **Leon – Sherry** | 11B0: Sherry läuft vorbei, Leon: „H-he!“ (`sub06 @0x107E`, `sub07 @0x10B6`, `sub13 @0x11D2`), läuft auf der Disc. 604: Sherry steht im Raum, Leons Blick `sub04` hat keinen Aufrufer | Kein Bild | **Ja, Sichtkontakt ohne Gespräch** |
| **Leon – John** | 604: John steht im Raum. Blick und Abgang (`Plc_neck @0x6EA`, `sub05 @0x74A`) haben keinen Aufrufer. 3000: Modell John, der Text nennt Marvin | Kein sicheres Bild, #118 offen | **Angelegt, ohne Text** |
| **Elza – Marvin** | 604: Typ 0x40 bei der Klappliege (`@0x5EA`). Elzas Gang und Blick (`sub06`, `sub09`, `Plc_neck @0x72E`) haben keinen Aufrufer | #058, 0:46.5, Tresen in 30C, ohne Text | **Ja im Foto**, Anlass unbekannt |
| **Elza – Ada** | 603: Ada statt Sherry, wenn Flag (4,22) gesetzt ist (`@0x388-0x3A8`). Setzer liegt nicht auf der Disc | Kein Bild | **Angelegt**, Kamiya bestätigt Adas kurzen Auftritt im Szenario der Protagonistin |
| **Marvin – John – Sherry** | 604, beide Dateien: alle drei im Raum | #151: Marvins Platz frei, Debug-Sprung | **Ja auf der Disc**, Spätstand offen |
| **Leon – Ada, Marvin** | Durchgehend | Mehrfach, im Film B 5:04.5 | Eigenes Szenario |
| **Elza – John, Sherry** | Durchgehend | Mehrfach | Eigenes Szenario |
| **Leon – zweite Frau im Kittel** | 603: Figur 0x4D. Ada nur bei (4,22)=1 und (4,23)=0, das kommt auf der Disc nicht vor | #145–#147, #152, #154: helles Haar, Kittel, lange dunkle Hose | **Ja**, Name nirgends genannt, nach Haar und Beinen nicht Ada |
| **Elza – zweite Frau im Kittel** | Kein Platz in 6031 | – | Nicht belegt |
| **Beide – Polizeichef** | Skripte und Text von 1150 und 1151 gleich, Satz „Mir geht es gut…“ | #089 Leon am Sofa, #016 Elza am Sofa mit 「……レオン」. B 2:54–2:57 Elza im Büro, Sofa leer | Beide |
| **Elza – Roy** | 1021 mit John | #027, #028 wahrscheinlich | Nur Elza |

## 4. Wie die Szenarien ineinandergreifen

Mehrere Stellen zeigen, dass eine Datei den Zustand fest vorgibt, den das andere Szenario erst herstellt. Die Dateizustände sind hart belegt, die Zeitfolge bleibt Deutung. Ein festes `Set` allein belegt nichts, dieses Muster steht auch ohne Bezug zur Gegenseite im Skript (1021, 1030, 4030). Tragend sind Text und Objektlage.

| Ort | Wer zuerst | Elza-Szenario | Leon-Szenario | Beleg |
|---|---|---|---|---|
| **Hintertür 306** | Elza | Sherry kriecht hinein: „Das Schloss an der Hintertür mache ich von innen auf.“ | Leon liest: **„Irgendwann ist das Schloss aufgemacht worden…“** | `ROOM3060 @0xF20` startet `sub02`, `@0xF9A Message_on 9`. Slot 1 führt nach 309, beide Sätze nutzen dasselbe Verb 外す |
| **Gitter und Hunde 306** | Elza | Lüftungsgitter an der Wand, Sherrys Szene löst es | Gitter liegt am Boden, drei Hunde wie in Elzas Zustand danach | `ROOM3060 @0xF74` = `ROOM3061 @0xFB4` (34 Byte), `@0xF38–0xF60` = `@0xF78–0xFA0` |
| **Schutzschalter 309** | Elza | Licht aus, Schalter muss erst umgelegt werden | Licht an, Text: **„Das Licht ist bereits an…“** | `ROOM3090 @0x1412 Set(4,8)=1`, nur Zweig „Licht an“. Text 0 nutzt auch 3091 nach dem Schalten. Die Kiste steht in 3090 an ihrer Startlage, Leons Raum ist also kein vollständiger Nachzustand |
| **Transporter 109** | Leon | Tür offen, kein Feuer, keine Ada | Transporter brennt, Tür zu, Ada ruft von innen. Nach dem Löschen Tür offen | `ROOM1091 @0x21BC` = `ROOM1090 @0x2320` (8 Byte), Feuer `@0x2204 ff.` nur bei Leon. [Foto #035 Elza ohne Feuer, #096 Leon mit Feuer](belege/transporter_109_vergleich.jpg) |

GamesRadar nennt den Transporter ebenso, ohne Quelle: Leon löscht, Elza findet ihn später.

Keine Zeitaussage tragen:
- **Zelle 121:** Bei Leon steht die Tür offen (`ROOM1210 @0x1E30`, Drehung `0x04C0`), bei Elza zu (`ROOM1211 @0x1E0E`). Das Flag (3,98) würde sie bei Elza öffnen, es hat aber keinen Schreiber, und die geschlossene Tür hat keine Kollision. Die Ursache ist also nicht belegt. Ein Forenbeitrag schließt daraus, Elza sei vor Leon da gewesen. John steht in 121 nicht im Skript.
- **Kisten 117:** Bei Leon stehen sie, Elza und John stoßen sie hinunter. 117 ist aber Leons Startraum, und im Film steht auch Elza neben ihnen (B 1:09–1:15).
- **Gitter 103:** Leon erlebt den Durchbruch (`ROOM1030 sub08`). Elzas Datei könnte ihn übernehmen (`ROOM1031 @0x1EAE`), das Flag (3,134) hat aber keinen Schreiber. 103 ist Elzas Startraum.
- **Stange 307:** `ROOM3070 @0x3114` = `ROOM3071 @0x31AE`, die Szene gibt es aber in beiden Dateien.

Weitere Klammern ohne Code-Zwang:
- **Dieselbe Garage:** Sherry läuft in 11B0 und 11B1 von derselben Stelle `(0xC5DA, 0xA210)` los. Über die Zeit folgt daraus nichts: Bei Leon rennt sie, bei Elza steht sie.
- **Gleiche Handlung, gleiches Ziel:** Leon braucht Impfstoff für Marvin (401), Elza für Sherry (501). Beide Räume tragen denselben Untersuchen-Text 急いでワクチンを持ってこなければ！ („Ich muss den Impfstoff schnell holen!“). Er steht in beiden Dateien beider Räume, ist also Raumtext und keine Figurenzeile. Enzym liegt nur in Elzas Datei (`ROOM40A1 @0xED0`), T- und G-Impfstoff sind getrennte Items.
- **Beide werden zu den Schutzräumen geschickt:** Ada zu Leon: „Geh zum unterirdischen Schutzraum“. Sherry zu Elza: „Schutzraum Nr. 2 kann ich öffnen“, diese Zeile läuft auf der Disc nicht.

## 5. Die beiden Wege nebeneinander

Orte aus den 531 dekodierten Disc-Hintergründen ([Übersicht](belege/raeume_40prozent.jpg)). Die Reihenfolge ist aus Flags und Texten erschlossen. „–“ heißt: auf der Disc nichts dazu. „Tot“ heißt: Der Text steht im Skript, läuft auf der Disc aber nicht (Abschnitt 12).

| Ort | Leon | Elza |
|---|---|---|
| **Start** | Dach 117 (`0x8001d2b0`) | Halle 103 (`0x8001d32c`) |
| **Dach 117** | Disc ohne Text. Im Spätstand zwei Zeilen (B 0:19–0:23.6): おい 冗談だろ？ / …クソッなんてこった！ | John: „Elza, beeil dich! … Wir müssen das Ding runterstoßen!“ |
| **Revier** | 109: Ada ruft hinter einer Tür, Leon befreit sie | 102: Roy verwandelt sich, „Das ist nicht mehr Roy!!“ Funk tot, Strom weg |
| **Chefbüro 115** | „Mir geht es gut. Denk an deine eigene Sicherheit.“ | Disc: derselbe Satz. Foto #016: 「……レオン」, Sprecher offen |
| **Zufahrt 10B** | – | Elza zu John: Mädchen in der Tiefgarage eingesperrt, John kennt eine Abkürzung |
| **Garage 11B** | **Sieht Sherry.** Marvin: „Leon! Du lebst!“, Rolltor durchbrochen, Plan: Kanaldeckel, „Lass die Frau runter“ (tot: 11C0 fragt Bank 8 ab; im Spätstand zu sehen, #169–#172) | Findet Sherry, „Warte brav im Wagen“ |
| **Parkgarage 11C** | Ada: Marvin ist umgekehrt, um dich zu suchen. Kampf. Abschluss „Was für ein Ungeheuer“ tot | – |
| **Kanal** | Ohne Text. Im Film Leon, Marvin, Ada (B 5:04.5) | Ohne Text. John steht in 20B1, dorthin führt aber keine Tür |
| **Fabrikhof 300** | Leon, Marvin: tote Polizisten, Umbrella-Ermittler. Ada schon vorgegangen, „nicht mal die Miene verzogen“. Der Spätstand hat einen anderen Wortlaut (#118) | Elza, John, Sherry: dieselben Toten, „früher eine Chemiefabrik“ |
| **Hinterhof 306** | **Tür schon offen, Gitter am Boden** | Sherry: geheimer Eingang, öffnet von innen |
| **Raum 309** | **Licht schon an** | **Als Sherry-Abschnitt angelegt**, die Figur bleibt auf der Disc Elza. Im Spätstand ist Sherry allein zu sehen (#050–#055). John: „Wenn es mein Leben kostet“. Sherry: „Hier hat mein Papa gearbeitet“ |
| **Halle 30E** | Marvin verletzt, kann nicht laufen (Disc 東をやられて, vermutlich das Bein), Ada taucht auf, Halle fällt | Elza hat etwas gesehen: „Das war doch kein Mensch“ |
| **Aufzug 307** | Ada: vermutlich „In die Hölle…“ (Disc 地仲さ) | Monster kam aus dem Aufzug, Sherry kann ihn bedienen |
| **Labor 400** | Geplant: Leon hält auf, „Kümmere dich um Marvin!“ (tot) | Sherry: „Papas Labor“, läuft los |
| **Halle 403** | – | Sherry verschwunden, Suche |
| **401 / 501** | **Ada: „Ich gehöre zu Umbrella.“** Impfstoff aus dem Virusstamm, Ada geht eigene Wege. „Marvin, wach auf!!“, „Schnell zum Schutzraum!“ sind tot | Alles tot, auf der Disc stehen nur Sherry und John im Raum: **Sherry vom Virus zerfressen**, Elza holt Impfstoff, Sherry wacht auf. Monster zerstörte die Energie, Explosion droht, „Schutzraum Nr. 2 kann ich öffnen“ |
| **Führerstand 603** | Figur 0x4D, im Foto die zweite Frau im Kittel. Ada nur als Zweig. Lange Szene, WARNING-Konsole blinkt | Sherry, Ada nur als Zweig |
| **Maschinenraum 604** | Marvin an der Tür aus 514, John, Sherry | Marvin bei der Klappliege, John, Sherry |

Figuren je Raum: `werkzeuge/figuren.py`. Der Disc-Text nennt Ada schon エイダ, nicht Linda. „Linda“ und „Brian“ stehen bei Project Umbrella nur im englischen Fließtext, in keiner Spielzeile, ihre Herkunft ist unklar. Beide Figuren haben denselben Türgraphen. Einzige Abweichung ist Slot 2 in 20B (Leon 300, Elza 203), und nach 20B führt keine Tür.

## 6. Das Ende im Zug

Beide Räume haben auf der Disc **keinen Textblock**, die Choreografie ist aber geschrieben. 603 ist der Führerstand (`UNTENSHITSU`), 604 laut EXE ein Maschinenraum (`DOURYOKUSHITSU`, 動力室, `@0x80071880`) mit Klappliege ([Hintergründe](belege/hintergruende_603_604.jpg)). 605–607 tragen denselben Namen, haben aber keine Dateien. STAGE6.BIN enthält nur die fünf Figuren-Handler, also weder Gegner noch Abspann.

**604 Leon (`ROOM6040`):** Marvin, John und Sherry stehen an festen Plätzen (`sub00`). Beim Betreten läuft `sub06`: Leon geht vier Wegpunkte ab, ohne Blickbefehl. Geschrieben, aber ohne Aufrufer ist `sub04`: Leon dreht den Kopf zu Sherrys Position, dann zu Johns, und John läuft los.

**604 Elza (`ROOM6041`):** Eigene Choreografie, also keine Kopie. Marvin steht auf Etage 1 bei der Klappliege (`@0x5EA`), Sherry steht an anderer Stelle. Beim Betreten läuft `sub11`: Elza blickt dreimal zwischen zwei Punkten hin und her, die grob in Richtung John und Sherry liegen, Sherry dreht sich zu ihr (`sub12`). Elzas Gang zu Marvin (`sub06`, `sub09`) hat keinen Aufrufer. Pose und Anlass stehen nicht auf der Disc.

**603:** Der Eintrag ist symmetrisch aufgebaut:
```
Leon:  (4,22)=1 und (4,23)=0  -> Ada     sonst -> Figur 0x4D
Elza:  (4,22)=1               -> Ada     sonst -> Sherry
```
Flag (4,22) und (4,23) werden in keinem Raum der Disc gesetzt, auch nicht über AOT oder Code. Auf der Disc steht in 603 also immer 0x4D bei Leon und Sherry bei Elza. Woher das Flag kommen sollte und was es bedeutet, ist unklar. Eine Vermutung ist ein Zustand zu Adas Überleben oder Route, ebenso möglich ist ein unfertiger Testzweig.

**Der andere Hauptcharakter fehlt in 603 und 604.** In keiner der vier Dateien steht er als Figur im Skript. Typ 0x4D in 6030 trägt Elzas Körper und Skelett mit Adas Clips, Typ 0x40 in 6041 Leons Körper mit Johns Clips. Beides liest sich nach Platzhalter, sicher ist das nicht (Abschnitt 8). Wo Elza in Leons Ende ist und umgekehrt, sagt die Disc nicht, 605–607 fehlen. Die Mod beantwortet das mit eigenem Text („She's out there fighting the monsters!“). Das ist Erfindung.

Einschränkung: Ausgeführt werden im Stand nur `sub06` (6040), `sub11` (6041, 6030) und `sub08` (6031). In 604 legt `main00` drei Zonen an, abgefragt wird nur eine, die übrigen Szenen dort haben keinen Aufrufer. In 603 sind `sub02`, `sub05`, `sub08` Fassungen derselben Szene mit denselben Wegpunkten.

## 7. Spielbare Begleiter

Im Projekt war das bisher nicht ausgewertet. Die Fanszene kennt es (THIA-Faden, TCRF-Kopie `information74.txt` Z. 197):

- **PL0D, PL0E, PL0F sind die Begleiter als Spielerfiguren:** Mesh und Textur sind byte-gleich zu Typ 0x40 (Marvins Typ), 0x49 (John), 0x4B (Sherry). Dazu gibt es `PL0FW00.PLW`.
- **Sherry:** Elzas 3091 setzt zwei Türen auf den Text 「はやくエルザたちを助けなければ...」, **„Ich muss Elza und die anderen schnell retten…“** (`@0x1464`, `@0x146E`). Das kann Elza nicht sagen. „Ich komme nicht an den Schalter…“ passt zu einem Kind. Im Spätstand ist Sherry dort allein zu sehen (#050–#055, #051 auf der Kiste am Kasten mit 「電げんがある」). Die EXE kennt sie fest als Spielfigur: `0x8003258c`, `0x80032898`, `0x80038190` vergleichen die Figur mit 0x0F.
- **Marvin:** `ROOM3000 @0x161A` fragt `work_vars[0x10] == 13` ab. Dieser Wert ist `0x800B0FF0`, die Spielfigur. Bei PL0D erscheint statt der Leiterfrage **„Meine Kräfte reichen nicht mehr. Schnell zu den Gefährten.“** Das passt zum verletzten Marvin.
- **Technik:** Der Raumlader tauscht die Figur, wenn der Wert sich ändert (`@0x80039768`). Auf der Disc schreibt aber kein Skript diesen Wert. Die Abschnitte sind vorbereitet, nicht verdrahtet: In 3091 bleibt die Figur Elza, und Marvins Text in 300 läuft nie.
- **John:** PL0E liegt bereit, eine Skriptstelle dazu gibt es nicht. Für 0x0D und 0x0E gibt es auch keine Sonderlogik in der EXE.

## 8. Fallen beim Deuten

| Falle | Was wirklich gilt |
|---|---|
| **Fotos als Szenenfolge lesen** | Video A ist eine vom Uploader sortierte Diashow. #031, #148, #159 zeigen das Debug-Menü, die Fotos entstanden also teils nach Sprung. Ein einzelnes Foto belegt den Zustand, nicht den Ablauf |
| **Video 0:22: Elza kniet neben einem Polizisten mit Leons Haar** | Das sind zwei Fotos, #027 und #028. Der Mann trägt Weste mit zwei hellen Trägern, rundes Abzeichen und kurze Ärmel, Leon trägt in allen Fotos lange Ärmel. Das schließt Leons Modell aus. Wahrscheinlich Roy: Typ 0x47 steht in `ROOM1021 @0x1D20` neben John, #026 und #030 sind Raum 102. Der Name steht in keinem Bild, der Hintergrund ist eine blaue Fläche ([Bild](belege/video_elza_roy_marvin_john.jpg)) |
| **Typ 0x4D in Leons 603 trägt Elzas Anzug** | Auf der Disc ist 0x4D Elzas Körper und Skelett (15/15 Offsets wie PL04) mit Adas Clips ([Vergleich](belege/vergleich_4D_gegen_elza.png)). Sieht nach Treffen aus, ist aber wahrscheinlich ein Platzhalter: Typ 0x40, 0x45, 0x47 sind ebenso gebaut (md5 `0c1f2a`, Leons Körper mit Johns Clips). Im Spätstand steht in 603 eine andere Frau, helles Haar, Kittel, lange dunkle Hose ([Bild](belege/video_kittel_fuehrerstand.jpg)), #146, #147, Ort rechnerisch bestätigt. STAGE5.BIN registriert 0x4D ebenfalls, das passt zu einer Laborfigur. TCRF vermutet Annette, ein Name steht in keiner Capcom-Quelle zu 1.5. Beweisen lässt sich „Platzhalter“ nicht |
| **Marvin sieht im 40%-Stand aus wie ein Polizist in Leons Art** | Typ 0x40 ist Platzhalter: Leons Körper mit Johns Clips. Auch der Polizeichef nutzt ihn. Der Marvin des Spätstands (hellblaues Hemd) ist ein eigenes, späteres Modell. In Elzas 6041 ergibt sich „Marvin“ nur über den Typ |
| **John steht in Leons 3000** | Platzhalter, denn der Text nennt Marvin (300 #2, #6). Das Foto hilft nicht: #116 (1:33) ist der Hinterhof 306, im Fabrikhof (#118, 1:34.5) ist der Mann nicht sicher zuzuordnen, seine Hose ähnelt eher Johns. 3000 hat eigene Lage und eigene Texte, eine Kopie aus 3001 ist nicht belegt. Für ein Treffen Leon–John reicht das nicht |
| **Ada und Marvin in Elzas 11B1, Ada in 1091, John in Leons 10B0** | Parkposition (−30000, −30000), kein Skript spricht sie an. Kopier-Rest |
| **John-Szene in Leons 4030** | Text aufrufbar, aber `@0x528`/`@0x52C` setzen (5,4) sofort auf 1, toter Code |
| **Beide Dateien tragen denselben Textblock** | Sagt nichts, das gilt für alle 25 Paare. Entscheidend ist, was läuft. 501: Nr. 6–16 werden gar nicht aufgerufen, Nr. 0–5 und 17–37 stehen in Subs ohne Aufrufer |
| **Alle NPC-Modelle liegen in beiden Archiven** | Stimmt (`CDEMD0`/`CDEMD1` byte-gleich), ist aber nur eine Möglichkeit, kein Treffen |
| **Ein Fan-Modell passt zu einem Foto** | Kein Beleg. Die Mod hat ihre Modelle nach diesen Fotos gebaut (README Z. 180, 274, 275, 387) |
| **Complete-Disc-Titel 「研究所に踏み込むレオンとエルザ」, 「警察署にたてこもるレオンとエルザ」** | Werbetext. Film 1 und 4 wechseln zwischen Leon und Elza, kein gemeinsames Bild. Den Titeltext kennen wir nur über Project Umbrella |

## 9. Was nur in der Mod steht

Diese englischen Zeilen sind **kein Capcom-Text**. Die Szene Elza–Marvin selbst ist echt (Foto #058), ihr Dialog ist nicht bekannt. Die Zeile 「……レオン」 steht in der Mod nicht.

| Raum | Mod-Text | Status |
|---|---|---|
| 30C1 | „I'm Marvin Branagh, RPD. I'm here with Leon Kennedy.“ | Raum nicht auf der Disc. Foto #058 hat keinen Text |
| 30C1 | Elza: „Leon?! How is he? We got separated back at the RPD.“ | Fan-Text. Widerspricht nichts, belegt aber nichts |
| 30C1 | „Will you tell him that me and John and Sherry are here…“ | Fan-Text |
| 6030 | „Where's Elza?“ – „She's out there fighting the monsters!“ | Disc: kein Text. Fan-Text |
| 6030 | „Where's that biker girl?“, Annette startet den Zug | Fan-Text |
| 6031 | „Where's Leon?“ – „He's fighting off monsters.“ | Fan-Text |
| 6031 | „I know how to start this! I watched Mom and Dad do it.“ | Fan-Text, passt aber gut zu Sherry im Führerstand |
| 604 | „Leon and Elza models“ | Laut README Mod-Zusatz. Auf der Disc steht in 604 weder Leon noch Elza als Figur. Mit Leons Körper steht dort nur Typ 0x40, Marvins Platzhalter |

## 10. Bilder und Filme: was zu sehen ist

Video A = `Biohazard 1.5 Final Build.mp4`, Video B = `Resident Evil 1.5 (All Footage).mp4`. Bei Video A zählen die Foto-Nummern, die Zeit nennt den Beginn des Fotos. Die Bildfolge sagt nichts über den Ablauf. Die vollständigen Schnittlisten stehen in [video_a.md](pruefung/video_a.md) und [video_b.md](pruefung/video_b.md).

| Stelle | Inhalt | Disc |
|---|---|---|
| A #012, 0:09.6 | John: „Elza! Geh nicht aufs Dach!“ | Nicht auf der Disc |
| A #016, 0:12.8 | Elza am Sofa des Chefbüros, Untertitel 「……レオン」 ([Bild](belege/foto016_leon_zeile.jpg)) | Nicht auf der Disc, Abschnitt 11 |
| A #027, #028, 0:21.6 | Elza neben verletztem Polizisten, blaue Fläche | Abschnitt 8 |
| A #032, 0:25.6 | Elza: …内に女の子がいるの / 戻らなきゃ | 10B #0 ähnlich, anderer Wortlaut |
| A #035, 0:28.0 | Elza in 109, kein Feuer | Abschnitt 4 |
| A #037, 0:29.6 | Elza kniet vor Sherry: …まあ 大きな時計ね！ | Nicht auf der Disc, Lesung nach Project Umbrella und Enigmatism415 |
| A #048, 0:38.4 | …使われてないはずだ | 300 #12 mit ねぇ statt ない |
| A #049, 0:39.2 | Sherry: …エルザ / 私 ヒミツの入り口を知ってるの | 306 #3 gleich |
| A #050–#055, 0:40.0 | Sherry allein in 309 und 30C, #051 an der Kiste: 電げんがある | Text nicht auf der Disc, Raum 30C fehlt |
| A #058, 0:46.5 | **Elza und Marvin am Tresen**, kein Text ([Bild](belege/video_elza_roy_marvin_john.jpg)) | Raum fehlt auf der Disc |
| A #059, 0:47.3 | John: はあ、はあ…間一ぱつってヤツだな！ | 309 #9, das Foto füllt die Disc-Lücken ぱつ und く |
| A #065, 0:52.1 | Rechner: Now Running, システムエラー | Nicht auf der Disc |
| A #082, 1:05.7 | Leon auf dem Dach neben den Kisten | Abschnitt 4 |
| A #083, 1:06.5 | Leon: そうだ！部長にこのことを報告しないと | Nicht auf der Disc |
| A #084, 1:07.3 | …みんなゾンビにくわれたか / それとも、ゾンビになっているかだよ。 | Nicht auf der Disc, Sprecher offen |
| A #096, 1:16.9 | Leon in 109, Transporter brennt | Abschnitt 4 |
| A #103–#105, 1:22.5 | Leon, Ada, Marvin in 200. #105 Platzhaltertext マーヴィン００ ２ | Kein Text, kein NPC in 2000 |
| A #116, 1:32.9 | Leon und Marvin im Hinterhof 306 | Raum vorhanden |
| A #118, 1:34.5 | Leon und ein hockender Mann im Fabrikhof, みんなラクー… | 300 #3 lautet anders. Mann nicht sicher zuzuordnen |
| A #127, 1:41.7 | Ada: 会わなかったのか？ | 11C #3 gleich |
| A #143, #145, 1:54.5 | #143 Riesen-Birkin am Bahnsteig. #145 zweite Frau im Kittel und Riesen-Birkin im roten Gang 509 | Nachtrag 38, Spur R |
| A #146, #147, 1:56.9 | Leon und zweite Frau im Kittel im Führerstand | 603, rechnerisch bestätigt |
| A #148, #151, 1:58.5 | 604 mit Debug-Menü, Spielzeit 0:09:01. Leon allein | Die Disc-Aufstellung ist nicht zu sehen |
| A #152, #154, 2:01.7 | Leon und dieselbe Frau am Bahnsteig | Raum 600 fehlt auf der Disc |
| A #169–#172, 2:15.3 | Leon, Marvin, Ada: Kanaldeckel-Plan | 11B #10, #16 abgewandelt, #172 ist eine neue Aussage |
| B 0:10.5 | Figurenwahl „PLEASE SELECT MAIN CAST“ | Beide getrennt wählbar |
| B 0:19–0:23.6 | Leon auf dem Dach: おい 冗談だろ？ / …クソッなんてこった！ | Nicht auf der Disc. Lesung Dritter, Bild nur teilweise lesbar |
| B 1:08.5–1:16 | Elza auf dem Dach neben den Kisten, Krähen | Das Skript von 1171 sieht das so nicht vor |
| B 2:12.8–2:33 | Gitter in 103 bricht, dann Leon | `ROOM1030 sub08` |
| B 3:19.5–3:30.5 | Elza, John, Sherry in 306 | 306 #3–6, wörtlich gleich |
| B 5:04.5–5:14.5 | **Leon, Marvin, Ada im Kanal**, kein Text | Kein NPC in 2000, 2001, 20B0 |
| B gesamt | Geprüft auf Leon und Elza zusammen | Nur das Menü zeigt beide, in getrennten Feldern |

Kein Video zeigt sicher John oder Sherry neben Leon, offen bleibt #118. Kein Bild zeigt Elza mit Ada. Das muss nichts heißen: Die Räume 603, 604 und der Bahnsteig sind in Video A zu sehen, das Finale nicht.

## 11. Nebenquellen

- **Kamiya, Official Guide Book 1998, S. 261** ([Scan](belege/kamiya_guide_s261.jpg)): 「シナリオは現在のようなザッピングシステムではなく、ひとり1本。女性主人公のシナリオにエイダがチラッと登場するシーンがあったり、警察署でカードキーを渡してくれる警官マービンが、レオンの先輩として最後まで行動をともにするパートナーだったりしました。」 Sinngemäß: Das Szenario war nicht das heutige Zapping-System, sondern je Figur eines. In der Geschichte der Protagonistin gab es eine Szene, in der Ada kurz auftaucht, und der Polizist Marvin, der im Revier die Keycard übergibt, war als Leons älterer Kollege sein Partner bis zum Schluss.
- **Kamiya, Twitter 9.2.2019**, auf die Frage nach Leon und Elza: 「1.5の時はパラレルではなかったけどザッピングは無かった…ただの二本立て…」. Original nicht abrufbar, Wiedergabe in blog.esuteru.com. Über ein Treffen sagt er nichts.
- **Project Umbrella** transkribiert unsere Quellen, keine andere Fassung: die Bioflames-Fotos (Video A), die Complete-Disc-Filme und die PlayStation-Museum-Disc, laut Hidden Palace der Stand unserer 40%-Disc. „Linda“ und „Brian“ stehen nur im englischen Fließtext. Die Seite führt im Chefbüro eine Elza-Zeile **「・・・レオン」 („…Leon“)**. Sie steht in Video A (#016, 0:12.8): Elza von hinten am Sofa, daneben der Verletzte. Auf der 40%-Disc fehlt sie (Textblock 115 hat nur eine Zeile, Skripte 1150/1151 sind byte-gleich, ROOM115U.SCD enthält keinen Text). Wer spricht, steht nicht im Bild. Project Umbrella schreibt Elza, der Übersetzer Enigmatism415 Chief Irons. Auf der Disc spricht an dieser Stelle der Verletzte (`ROOM1151 @0xE5E`, `@0xE66`), der Chef ist also wahrscheinlicher. Bei Leon zeigt dieselbe Kamera keinen Text (#089). Belegt ist nur: Im Elza-Lauf des Spätstands erscheint Leons Name.
- **GamesRadar** (2018) schreibt, beide träfen sich nie und fänden die Folgen des anderen: Leon lösche den Laster, Elza finde ihn später, Elza besiege Birkin, Leon finde ihn später, dazu „some shared companions“. Eine Quelle nennt der Artikel nicht. Der Laster passt zu Disc und Fotos, die Birkin-Stelle ist nicht geprüft.
- **GamePro.de** (2015) schreibt: „Leon S. Kennedy und Elza Walker begegnen sich im Spielverlauf von Resident Evil 1.5 nicht.“ Das ist eine Bildunterschrift ohne Quelle.
- **Fandom** stützt „did not cross“ auf Resident Evil Archives S. 218–220. Diese Seiten haben wir nicht gesehen.
- **Forum The Horror Is Alive** nennt: „In the final train, everyone is there, Marvin, John, Sherry and Annette. Except Ada. So, maybe Ada is still a spy?“ Das ist eine Fan-Beobachtung aus einem Durchlauf des geleakten Builds, bekannt nur aus Suchauszügen. „Annette“ meint vermutlich Figur 0x4D, die in 603 steht, nicht in 604. Die Disc bestätigt Marvin, John und Sherry in 604.
- **TCRF** liegt als Kopie in `info/Resident_Evil_und_Playstation_Information/information74.txt`. Es nennt 0x4D wahrscheinlich einen Platzhalter für Annette und Irons mit Leons Modell.

## 12. Grenzen

- Die 40%-Disc endet beim Text vor dem Zug. Alle Aussagen zum Ende stammen aus Choreografie ohne Dialog.
- Viele Räume des Plans fehlen auf der Disc. Die EXE nennt Räume ohne Datei (10A, 127, 20A, 40B, 505, 605–607). Der Mod-Baum hat 144 RDT-Dateien mehr, darunter 30C. Was dort lief, zeigt nur das Foto #058.
- Sprecher stehen nicht im Text, sie sind aus dem Zusammenhang und der Regie im Skript gesetzt. In 300 gibt es keine Regie je Zeile.
- Fehlende Kanji und Ersatzzeichen wie 東 sind aus dem Zusammenhang ergänzt. Das betrifft 43 Zeilen, ihre deutsche Fassung ist Lesart und in TEXTE_40.md markiert.
- Von 291 Zeilen können auf der Disc höchstens 217 laufen. 23 werden in keinem Skript aufgerufen (vor allem 501 #6–16 und 50A). 51 stehen nur in totem Code: 400 #5–8, 401 #19–22, 501 #0–5 und #17–37 in Subs ohne Aufrufer, 11B #10–21 und 11C #7–9 hinter Bank 8 in 11C0, 300 #1 hinter einer Spielfigur, die es nicht gibt. Das zeigt den Plan, nicht den Ablauf der Disc. Die Prüfung ist statisch, ein Emulator lief nicht.
- Die Bilder sind stark verzerrt. Figuren-Zuordnungen dort sind Sichtbefund. Die Fotos von Video A stammen aus einer Debug-Sitzung, ein Foto belegt keinen Ablauf.
- Durchsicht: Video A alle 174 Standbilder einzeln, kürzere Einstellungen gibt es dort nicht. Video B zwei Bilder je Sekunde plus Schnittliste, 43,8 s ohne erkennbare Spielfigur. Die Tonspur von Video B und den Complete-Disc-Filmen ist nicht ausgewertet.

**Offene Fragen:** Sprecher von 「……レオン」. Ob die Spuren eine bewusste Zeitlinie sind oder aufgeräumte Räume. Bedeutung von (3,98), (3,134), (4,22), (4,23). Pose und Anlass von Marvin in 6041. Der Mann in #118. Name der zweiten Frau im Kittel: „Annette“ bleibt Schluss aus RE2, Mod, TCRF und Foren. Inhalt von 600–602 und 605–607 und damit das Finale. Rolle der Taschenuhr.

**Was am meisten bringen würde:**
1. **Tonspur der vier Complete-Disc-Filme abhören**, rund 230 s. Das ist der einzige unausgewertete Capcom-Text mit Figurenrede. Fällt dort ein Name der Gegenseite, wäre das ein Primärbeleg. Das kann der Nutzer selbst.
2. **Resident Evil Archives S. 218–220:** die einzige mögliche Capcom-nahe Wortlautquelle für das Nicht-Treffen. Dazu Capcom Design Works S. 22–30 zu Roy und Annette.
3. **Laufprobe der Disc im Emulator**, weil alle Aussagen zur Erreichbarkeit statisch sind (Skill `re15-room-capture`).
4. **Trial Edition auf dem Director's Cut:** laut TCRF-Kopie Restdaten des späten 1.5-Stands. Liegt nicht im Repo.

## 13. Folgen für unser Projekt

- **Story-Regel:** „Leon und Elza begegnen sich nie“ bleibt gedeckt. „Elza kann Ada begegnen“ ist durch 6031 als Zweig und durch Kamiya gestützt. Neu: Leon sieht Sherry, Leon und John stehen in 604 im selben Raum, Elza trifft Marvin (#058). Die Reihenfolge ist nicht einseitig, „Elza zuerst“ gilt nur für die Fabrik.
- **Unsere Dialoge in 30C1, 6030, 6031** sind Fan-Text der Mod und damit nicht Original. Der Port liest sie über die RDTs aus `shared_assets`. Die Zeile 「……レオン」 steht weder in der Mod noch auf der Disc.
- **Begriff:** `CLAUDE.md` nennt `info/Re1.5/` „Auslieferungsstand“. Das ist die Mod, nicht Capcoms Stand. Für Verhalten und Assets ist das bisher unser Maßstab, für Story darf es das nicht sein.
- **Nicht übernommen:** spielbarer Marvin, John, Sherry. Wäre eine eigene Aufgabe.

## Nachprüfen

- Texte: `werkzeuge/jp.py` (Zeichentabelle, Raster aus `PSX.EXE @0x800289bc`, `@0x800289ec`, `@0x80028a44`, `@0x80028b00`, Glyphen aus `DATA/TEX.TIM`), `dump40.py`, `texte_bauen.py`
- Skripte: `zensus.py`, `figuren.py`, `quer.py`, `spuren.py`. Erreichbarkeit: `k12_subs.py`, `k13_fix.py`
- Modelle: `ems.py` (TOC `@0x80072f38` Leon, `@0x80073178` Elza)
- Einzelne Räume: `python re15_port/tools/scd_dump_room.py "Version_40%/PSX/STAGE6/ROOM6040.RDT"`
- Hintergründe: `probe_bg_dump.exe "Version_40%/PSX" <ziel> <stage>`
- Prüf-Dossiers samt Korrekturliste: `pruefung/`. Ihre Pfade zu Zwischendateien zeigen auf ein Arbeitsverzeichnis der Sitzung

Quellen außerhalb der Disc:
- [Project Umbrella: BIOHAZARD 1.5 Scenario & Description](https://www.projectumbrella.net/biohazard-15-scenario--description.html)
- [Kamiya-Tweets, Wiedergabe](http://blog.esuteru.com/archives/9266710.html), twitter.com/PG_kamiya/status/1094291939426357248
- [Official Guide Book, Scans](https://residentevil.fandom.com/wiki/Famitsu_interview_with_Hideki_Kamiya)
- [Enigmatism415, Biohazard 1.5 Final Build](https://www.youtube.com/watch?v=qnWhK82FzdU), Herkunft Video A, Übersetzungen
- [Hidden Palace, Nov 6 1996 Prototype](https://hiddenpalace.org/Biohazard_2_(Nov_6,_1996_prototype))
- [GamesRadar 2018](https://www.gamesradar.com/how-the-never-released-resident-evil-15-survived-22-years-to-appear-in-resident-evil-2-remake/)
- [GamePro: Vaporware Resident Evil 1.5](https://www.gamepro.de/artikel/vaporware-resident-evil-15-das-untote-resident-evil,3083099.html)
- [The Horror Is Alive: 1.5 WIP Spoiler Thread](http://www.the-horror.com/forums/archive/index.php/t-9980-p-2.html)
- `info/Resident_Evil_und_Playstation_Information/information74.txt` (TCRF-Kopie)
