# Kritik: letzte Prüfung von BERICHT.md und TEXTE_40.md

Stand 2026-10-31. Geprüft wurden die sechs Dossiers `texte.md`, `skripte.md`, `daten.md`, `video_a.md`, `video_b.md` und `quellen.md` gegen die Rohdaten. Eigene Skripte und Bilder liegen unter `pruefung/kritik/`. Im Repo wurde nichts geändert. Standbilder heißen #NNN und zählen wie in `video_a.md` von #000 bis #173.

## 0. Ergebnis in Kürze

Die Kernantwort des Berichts hält: Kein Skript, kein Bild und keine Capcom-Aussage zeigt ein Treffen von Leon und Elza. Vier Punkte ändern das Bild aber deutlich.

1. **Die Spuren laufen in beide Richtungen.** Der Satz „Alle drei Weltspuren zeigen in dieselbe Richtung“ ist falsch. In der Fabrik ist Elza zuerst da, am Transporter in 109 Leon. Den Transporter belegen jetzt auch die Fotos: #035 zeigt Elza ohne Feuer, #096 Leon mit Feuer. Das hat bisher niemand verglichen, `video_a.md` hatte die beiden Cuts sogar vertauscht.
2. **Viel weniger Text läuft auf der Disc, als der Bericht annimmt.** Von 291 Zeilen können höchstens 217 laufen. 23 haben keinen Aufruf, 51 stehen nur in totem Code. Dazu gehören der gesamte Dialog in Elzas 501 (Sherry vom Virus zerfressen, Impfstoff, „Schutzraum Nr. 2“), Leons Abschied in 400 und „Marvin, wach auf!!“ in 401. Das hat keiner der sechs Prüfer gefunden.
3. **Im Spätstand fällt Leons Name einmal im Elza-Lauf** (#016, 「……レオン」). Wer spricht, ist offen, wahrscheinlich der Chef.
4. **Elza und Ada waren geplant.** Kamiya bestätigt es im Scan des Guide Book, S. 261: Ada taucht im Szenario der Protagonistin kurz auf.

Verworfen habe ich fünf Überinterpretationen der Prüfer (Abschnitt 4), darunter die Kisten auf dem Dach und das Gitter in 103 als Zeitspuren sowie die Korrektur an 306 #0. Sechzehn Widersprüche zwischen den Prüfern sind an den Rohdaten entschieden (Abschnitt 1). Abschnitt 6 beantwortet die drei Nutzerfragen neu, Abschnitte 7 und 8 enthalten die Korrekturlisten.

## 1. Widersprüche zwischen den Prüfern

| Nr | Punkt | Seite A | Seite B | Rohdaten | Entscheidung |
|---|---|---|---|---|---|
| W1 | Unbenutzte Zeilen | `texte`: 25 bestätigt | `skripte`: 23 | `ROOM5060 @0x2B92` `2c 0d 01 b1 … 07 00 ff ff`, `@0x2BC2` `2c 0f 01 b1 … 09 00 ff ff`. Lange AOT, Nutzlast ab pc+22. Eigener Lauf `kritik/k1_texte.py`: 23, Differenz genau 506 #7, #9 | **23**. `texte` hat den Fehler von `quer.py` übernommen |
| W2 | 306 #0 „Abgeschlossen“ bei Leon | `texte`: nur Elza, Leon sieht es nie | `quellen`: Slot 2 trägt Text 0 | `ROOM3060 @0xF2E` `46 02 01 31 00 00 ff ff 00 00` steht außerhalb beider Zweige. Tot ist nur der Slot-1-Zweig `@0xF0A` | **`quellen`**. „Leon + Elza“ bleibt richtig |
| W3 | Transporter 109 | `skripte`: Elzas Datei zeigt Leons Nachzustand | `quellen`: auf der Disc nicht belegbar, 1091 hat kein Ereignis | `ROOM1091 @0x21BC` `2e 03 03 00 34 06 00 00` ist byte-gleich `ROOM1090 @0x2320`. Fotos #035, #096 | **`skripte`**. `quellen` sah nur, dass kein Ereignis läuft, nicht die zwei Befehle |
| W4 | Raum 103 als Reihenfolge | `video_b`: stützt Elza zuerst | `skripte`: angelegt, Bedeutung offen | 103 ist Elzas Startraum (`0x8001d32c`). Gitter intakt ist ihr Spielbeginn | **Kein Ordnungsbeleg** |
| W5 | Kisten 117 | `skripte`: Leon vor Elza | Video B | B 1:09–1:15: Elza steht neben denselben Kisten (`kritik/k5_kisten.jpg`). 117 ist Leons Startraum | **Kein Ordnungsbeleg** |
| W6 | Quellen von Project Umbrella | `texte`: dieselben Aufnahmen plus mindestens eine weitere | `quellen`: genau Video A und Disc | マーヴィン！ これは… ist Disc 300 #2. L811–812 sind in #082 nicht lesbar (weißer Boden, Enigmatism415 listet sie dort), aber in B 0:19 und Complete Disc Nr. 1 | **Drei bekannte Quellen:** Fotos, Disc, Complete Disc. Keine weitere Aufnahme nötig |
| W7 | Complete-Disc-Herkunft von Video B | `quellen`: nicht belegt | `video_b`: gemessen, r bis 1,00 | Kontaktbögen `kritik/cd/c1.jpg`–`c4.jpg` zeigen dieselben Szenen | **`video_b`** |
| W8 | Dachzeile B 0:19 | `video_b`: おい 危険だぞ！ | `texte`, Project Umbrella, Enigmatism415: おい 冗談だろ？ | Eigener Glyphenkamm entscheidet nicht (0,41 gegen 0,42, Null bis 0,56). Englische Tonzeile laut Project Umbrella L810: „…Oh, that's just freaking great!!“ passt nur zu 冗談だろ | **おい 冗談だろ？** als Lesart Dritter, im Bild nur teilweise lesbar |
| W9 | A 1:24 | `video_a`: nur マーヴィン belegt | `quellen`: マーヴィン００ ２ | Lupe `kritik/sub_105.jpg`: zwei runde Zeichen, Abstand, 2 sind mit bloßem Auge lesbar | **`quellen`**. Die Korrelation von `video_a` versagte an Zahlen |
| W10 | Frau im Kittel | `video_a`: nicht Ada | `quellen`: unklar | #127, #169, #172: dunkles Kurzhaar, nackte Beine. #145–#147, #154: helles Haar, lange dunkle Hose (`q_frames/kittel.jpg`, `q_frames/ada_marvin.jpg`, Nutzerbild `Version_80%/vlcsnap-…00h29m02s830.png`) | **`video_a`**. Kittel trennt nicht, Haar und Beine trennen |
| W11 | Sprecher 「……レオン」 | `quellen`: am ehesten der Chef | `texte`, `video_a`: offen | Disc-Regie: `ROOM1151 @0xE5E Plc_motion` der Sofa-Figur, `@0xE66 Message_on 0`. Enigmatism415: Chief Irons. Project Umbrella: Elza | **Nicht im Bild belegt, Chef wahrscheinlicher** |
| W12 | Was in 604 läuft | `skripte`: ausgeführt ist nur Leon–Sherry in 11B | BERICHT: Leon trifft John und Sherry | `sub00` setzt alle drei immer, `sub06` läuft. Nur die Blickszenen `sub02–05` fehlen | **Beide zu grob.** Die Figuren stehen im Raum, die Blicke laufen nicht |
| W13 | Startpunkt | `daten`: (3940,14167), (−8200,−22500) | Code | `0x8001d2b4 ff.`: Leon (2386, −7200, 13745), Elza (−8888, 0, −12989). Die Werte von `daten` sind die JUMP-Tabelle `0x8007018c`, `0x8006ff84` | **Räume stimmen, Koordinaten aus dem Code** |
| W14 | Cuts #035, #096 | `video_a`: #035 Cut 8, #096 Cut 0/8 | Eigene Lupe | `kritik/k3b_109.jpg`. Rotanteil am Transporter #035 0,000, #096 0,112, Disc Cut 0 0,000, Cut 8 0,096 | **#035 Cut 0, #096 Cut 8** |
| W15 | Zeit von #016 | `texte`: 0:12.83 | `video_a`, `quellen`: 0:12.81 | Containerversatz 0,033 s | Still-Zeit 0:12.81 verwenden |
| W16 | Weggabelung 20B | `daten`: einzige Gabelung | `skripte`: 20B ohne Zugang | `kritik/k7_tueren.py`: kein Door_aot_set führt nach 20B | **Beides wahr.** Die Abweichung liegt in einem Raum ohne Zugang, sie belegt keine Route |

## 2. Nachprüfung der Urteile „falsch“ und „neu“

| Prüfer | Befund | Ergebnis | Eigene Prüfung |
|---|---|---|---|
| texte, video_a, quellen | 「……レオン」 steht in Video A | **Bestätigt** | `q_frames/couch.jpg`, `texte/a013_vergleich.png` |
| texte, quellen | Project Umbrella ist keine andere Fassung | **Bestätigt**, präzisiert | W6 |
| texte, video_a, quellen | Sherry allein #050–#055 | **Bestätigt** | `q_frames/sherry.jpg`. #051 zeigt sie auf der Kiste am Kasten |
| texte | Leon hat im Spätstand zwei Dachzeilen | **Bestätigt** | Lesung siehe W8 |
| texte | jp.py 聞, 接, 意, 、。・ | **Bestätigt** | `texte/z_a.png`, `z_b.png`, `z_kana.png`. 09:1A, 09:1B sind Anführungszeichen oben, nicht 「」 |
| texte | Unbenutzte Einträge 楽, 声, 槽, 兜, 熔, 溶, 遺 | **Bestätigt**, ohne Wirkung | 0B:1E sieht nach 念 aus, Sichtbefund |
| texte | 30E #10 falsch | **Bestätigt** als Lesart | 以, 予, 想, 数, 集 fehlen alle im Font. Derselbe Test, den `texte` bei 確 nutzt |
| texte | 307 #5 falsch | **Bestätigt** als Lesart | 確 liegt im Font, 静 nicht |
| texte | §12 „in TEXTE_40.md markiert“ falsch | **Bestätigt** | 43 Zeilen, 5 markiert. 東: 20 Zeilen, 46 Zeichen, nur 307 und 30E (`kritik/k14_paare.py`) |
| texte | 25 Zeilen bestätigt | **Verworfen** | W1 |
| texte | 306 #0 nur Elza | **Verworfen** | W2 |
| texte | 403 #5–8 nur Elza | **Bestätigt** | `ROOM4030 @0x528`, `@0x52C`, `ROOM4031 sub08 @0x74C`, `sub09 @0x7B4` |
| texte | Alle 25 Paare gleich | **Bestätigt** | Decodierte Zeilen je Paar identisch |
| texte | Indirekte Bezüge nur 306 #9, 309 #0 | **Bestätigt** | Die Regie zu 11B #20 trägt |
| texte | 514 #0 nicht Ada | **Abgeschwächt** | Reines Sprachgefühl, kein Skriptbezug |
| skripte | „Alle drei Weltspuren“ falsch | **Bestätigt**, ohne 117 | Abschnitt 3.2, 3.3 |
| skripte | Gitter und Hunde 306 | **Bestätigt** | 34 Byte `3060 @0xF74` = `3061 @0xFB4`, Hunde `@0xF38–0xF60` = `@0xF78–0xFA0` |
| skripte | Stange 307 | **Bytes bestätigt**, Spur schwach | `3070 @0x3114` = `3071 @0x31AE` |
| skripte | Rolltor 103, (3,134) | **Bytes bestätigt**, keine Ordnung | W4 |
| skripte | Byte 4 ist die Etage | **Bestätigt** | Disasm `0x800420a0` ff. |
| skripte | Marvin in 6041 bei der Klappliege | **Bestätigt**, Pose offen | `ROOM6041_pts.jpg`, Cuts 3, 4, 5 |
| skripte | Marvin-Szene wegen Bank 8 tot | **Bestätigt**, erweitert | Bank-Tabelle `0x80074664`: 7 = `0x800b1038`, 8 = `0x800b1058`. Gorilla `0x8011b8ec`, `0x8011b8f4 jal 0x8004ef90`. STAGE1 hat keinen Bezug auf Bank 8. Mod `0x1836` `21 07 60 01` |
| skripte | Spiegelfall John in 10B0 | **Bestätigt** | `figuren.out` |
| skripte, daten | Startorte 117, 103 | **Bestätigt** | W13 |
| skripte | (3,98), (3,134), (4,22) ohne Schreiber | **Bestätigt** | `kritik/k8_flags.py`, `k13_fix.py` |
| daten | ROOM115U, ROOM506U ohne Text und Figuren | **Bestätigt** | `kritik/k16_rest.py`: 0 Message_on, 0 Sce_em_set, 0 Plc |
| daten | sce 6 setzt `0x800b25c0` | **Bestätigt** | `0x800431a8` |
| daten | Itemtabelle, T- und G-Impfstoff | **Bestätigt** | IDs 58–63, 70, 71 selbst decodiert |
| daten | 604–607 DOURYOKUSHITSU | **Bestätigt** | `0x80071880 ff.`, 動力室 ist Maschinenraum |
| daten | STAGE5/6 registrieren 0x4D | **Bestätigt** | `k16_rest.py`, Roster je Overlay |
| daten | Sherry im Engine-Code | **Bestätigt** | `0x8003258c`, `0x80032898`, `0x80038190` vergleichen `0x800aca5c` mit 0x0F |
| daten | 0x4D: Elzas Körper, Adas Clips | **Bestätigt** | `d_rig.py`, Offsets 15/15 |
| daten | Enzym nur bei Elza | **Übernommen**, unklar | Der Bericht sollte daraus keine Story ableiten |
| video_a | #028 ist nicht Leons Modell | **Bestätigt** | `va/l/vgl028.jpg`. Das Fan-Modell m47E zählt nicht als Beleg |
| video_a | A 1:33 ist 306, nicht 300 | **Bestätigt** | #116 orange Tür, #118 Kamera wie #044 |
| video_a | Diashow, §12 falsch | **Bestätigt** | Dazu `quellen`, YouTube-Beschreibung |
| video_a | Mod-Modelle bilden den Capcom-Stand ab | **Verworfen** | Abschnitt 4 |
| video_a, texte | A 2:16–2:18 weicht ab | **Bestätigt** | Enigmatism415 liest ebenso |
| video_b | Kanal-Szene Leon, Marvin, Ada | **Bestätigt** | B 5:06–5:11 in `kritik/leonclip.jpg` |
| video_b | BEFUND §5 hat vier falsche Figuren | **Stichprobe bestätigt** | B 2:24, 2:27.5, 2:29.2 Leon, 11:40 Leon, 9:20 passt zu Elza. BEFUND ist nicht Gegenstand dieser Prüfung |
| video_b | Raum 103 als vierte Weltspur | **Verworfen** als Reihenfolge | W4 |
| video_b | Mod-Nummern im BEFUND | **Übernommen** | Nicht nachgemessen |
| quellen | Kamiya S. 261 | **Bestätigt** | Scan selbst gelesen |
| quellen | Kamiya 9.2.2019 | **Bestätigt** über Blog-Kopie | `quellen_web/esuteru/page.txt` Z. 64 |
| quellen | Laster auf der Disc nicht belegbar | **Verworfen** | W3 |
| quellen | Forenzitat abgeschnitten | **Übernommen** | Nur Suchauszug, nicht prüfbar |
| quellen | tcrf.net nicht erreichbar | **Überholt** | Lokale Kopie, Abschnitt 3.7 |

## 3. Eigene neue Funde

### 3.1 74 von 291 Zeilen laufen auf der Disc nicht

Keiner der sechs Prüfer hat geprüft, ob die Subs mit den Textaufrufen gestartet werden. `skripte` tat es nur für 603 und 604.

Vorgehen in `kritik/k12_subs.py` und `kritik/k13_fix.py`: Startmenge `main00`, `sub00`, `sub01` (`0x8003efb0`, `0x8003efd4`, `0x8003f080`). Kanten sind Evt_chain (`0x8003f280`, pc[3]), Evt_exec (`0x8003f2b4`, pc[3]), Gosub und AOT sce=3 (`0x80043100`, Nutzlast+3). Danach läuft ein Fixpunkt über Bank 3/4, Bank 5 je Datei und Kill-Bits. Bedingungen, die sonst unbekannt sind, gelten als erfüllbar. Das Ergebnis ist also eine Obergrenze für das, was läuft.

| Klasse | Zeilen | Beleg |
|---|---|---|
| Kein Aufruf | 23: 117 #2, 11C #10, 301 #0, 309 #7, #27, 40A #4, 501 #6–16, 50A #0–3, 512 #1, 514 #0 | W1 |
| Sub ohne Aufrufer | 35: 400 #5–8 (`ROOM4000 sub16 @0x18D8`), 401 #19–22 (`ROOM4010 sub06 @0x908`), 501 #0–5 (`ROOM5011 sub02 @0x4A0`), 501 #17–37 (`ROOM5011 sub06 @0x57A`) | `ROOM5011 sub01 @0x49E` ist leer, `sub00` setzt nur die Figuren |
| Flag nie wahr | 15: 11B #10–21, 11C #7–9 | Abschnitt 2, Bank 8 |
| Spielfigur nie 13 | 1: 300 #1 (`ROOM3000 @0x161A`) | `work_vars[0x10]` schreibt kein Skript |

Für die Frage „was war geplant“ bleiben diese Zeilen Capcom-Text. Für „was läuft auf der Disc“ fallen sie weg. Der Bericht trennt das bisher nicht, und die Spalte „Auslöser“ in TEXTE_40.md täuscht Laufzeit vor.

### 3.2 Der Transporter ist auch im Foto belegt

Raum 109 hat Fassungen ohne Feuer (Cut 0–7) und mit Feuerlicht (Cut 8–15, `ROOM1090_mark.jpg`). Leons Datei schaltet nur bei (3,129)=0 auf die Feuer-Cuts (`@0x21F4`, `@0x21FE`).

- **#096, Leon:** Der Transporter links unten ist rot, davor weiße Flammen. Das ist Cut 8.
- **#035, Elza:** Dieselbe Stelle ist blau, kein Feuer. Das ist Cut 0.

Messung (`kritik/k3b_109.py`, Anteil roter Pixel im Ausschnitt): #035 0,000, #096 0,112, Disc Cut 0 0,000, Disc Cut 8 0,096. Bild: `kritik/k3b_109.jpg`.

Damit gilt Elzas Zustand ohne Feuer auch im Spätstand. Dass ihr Besuch nach Leons liegt, bleibt Deutung, die Fotos stammen aus einer Debug-Sitzung. GamesRadar nennt genau diesen Fall, ohne Quelle: „Leon would find a police truck on fire and put it out … and Elza would find the extinguished truck later.“

### 3.3 Die Kisten auf dem Dach sind keine Zeitspur

`skripte` wertet Leons Kisten als „Leon früher“. Das trägt nicht.

- 117 ist Leons Startraum. Sein Raum zeigt den Zustand bei Spielbeginn.
- B 1:09–1:15 (Complete Disc Nr. 1) zeigt Elza ohne John neben denselben Kisten. Im Disc-Hintergrund Cut 3 fehlen sie, im Video stehen sie bei beiden (`kritik/k5_kisten.jpg`).
- Kisten, Krähen und eine frei laufende Elza ohne John sieht das Skript von 1171 gar nicht vor (`@0x90A`, `@0x992`). Der Film stammt also aus einem anderen Stand dieses Raums.

Für 103 gilt dasselbe spiegelbildlich: Das intakte Gitter ist Elzas Spielbeginn. Beide Stellen sagen nichts darüber, wer zuerst wo ist.

### 3.4 Die Aufstellung in 604 ist nur Disc-Stand

Auf der Disc steht Marvin in 6040 im Bild von Cut 0 neben der Tür (`ROOM6041_pts.jpg`, grüne Linie). #151 zeigt genau diese Kamera mit Leon an der Tür, Marvin steht dort nicht. Das Debug-Menü in #148 zeigt, dass in diesem Raum gesprungen wurde. Deshalb ist das kein Gegenbeleg. Eine Bestätigung für den Spätstand gibt es aber auch nicht.

### 3.5 Der Mann in #118 ähnelt eher John

`video_a` nennt den Mann offen. Die Messung `kritik/k4b_hosen.py` kippt das leicht: Die Hose hat im Median BGR (137, 102, 75), Johns Hose in #044 (171, 145, 100), Marvins in #169 (119, 59, 34). Das Verhältnis Blau zu Rot liegt bei 1,8 gegen John 1,7 und Marvin 3,5. Das ist schwach, die Kamera clippt und die Boxen sind handgesetzt.

Passt der Befund, steht in #118 Johns Modell. Das entspräche dem Disc-Typ 0x49 in 3000 (`@0x157A`) und wäre der Platzhalter, nicht eine Begegnung: Der Text der Szene nennt Marvin (300 #2, #6).

### 3.6 Complete-Disc-Filme

- Die vier Filme habe ich selbst durchgesehen (1 Bild/s, `kritik/cd/c1.jpg`–`c4.jpg`). Film 1 und 4 wechseln zwischen Leon und Elza, ohne gemeinsames Bild. Film 2 zeigt nur Leons Gruppe, Film 3 nur Elzas. Damit stimmt BERICHT §8 „Werbetext zu Clips, die beide getrennt zeigen“.
- Die Titel 「警察署にたてこもるレオンとエルザ」 und 「研究所に踏み込むレオンとエルザ」 kennen wir nur aus Project Umbrella. In den Dateien stehen sie nicht.
- **Die Tonspur ist unausgewertet.** Fandom zitiert aus diesen Filmen englische Sprachzeilen („Elza I know a secret way in!“), Project Umbrella Leons „…Oh, that's just freaking great!!“. Der Hörtext ist bisher nirgends vollständig erfasst. Hier habe ich kein Sprachwerkzeug, das Spektrogramm (`kritik/k15_ton.jpg`) beweist nichts.

### 3.7 TCRF liegt lokal im Repo

`info/Resident_Evil_und_Playstation_Information/information74.txt` ist eine Kopie der TCRF-Seite zum Prototyp. `quellen` fand tcrf.net nicht erreichbar. Daraus:

- Z. 292: „Elza's model here is likely used as a placeholder for Annette.“ (603)
- Z. 251: „Chief Irons uses Leon's model as a placeholder“, passt zu Typ 0x45.
- Z. 197: „Marvin's playable part“ als Vermutung, Z. 144: „Sherry's puzzle“.
- Z. 89: JUMP führt „into any room in the game they wish regardless of scenario“.

Damit ist §7 „Das stand bisher nirgends im Projekt“ falsch.

### 3.8 Fandom stützt zwei Disc-Befunde

`quellen_web/fan_plot/page.txt` Z. 530: „Leon later sees a child named Sherry Birkin crawling down a drain … but is unable to call to her.“ Das deckt sich mit 11B0. Z. 534: Birkin sei „recently knocked out by Elza Walker“, als Leon ihn trifft. Das passt zu 30E #19, belegt aber nichts: Der Abschnitt hat dafür keine Einzelquelle.

## 4. Verworfene Überinterpretationen

1. **Kisten 117 als „Leon vor Elza“** (`skripte`). Startraum, und Elza steht im Film daneben (3.3).
2. **Raum 103 als vierte Weltspur für „Elza zuerst“** (`video_b`). Startraum Elzas. Die Bemerkung von `video_b`, Leons Datei erzwinge dort nichts, trifft den Punkt schon selbst.
3. **306 #0 nur Elza** (`texte`). Text 0 liegt in 3060 auf Slot 2 (W2).
4. **„Einzelne Mod-Modelle bilden den späteren Capcom-Stand ab“** (`video_a`). m40, m4D und m47E passen zu den Fotos, weil Fans nach diesen Fotos gebaut haben: README Z. 180, 274, 275 nennen neue Modelle von Juvenal, Z. 387 baut ausdrücklich nach einem Inflames-Bild. Das ist ein Zirkelschluss.
5. **„Ausgeführt ist nur Leon–Sherry“** (`skripte`). In 604 stehen die Figuren wirklich im Raum (W12). Nur die Blickszenen fehlen.

Abgeschwächt, nicht verworfen:

- **„Hart belegt“ in BERICHT §4.** Die Dateizustände sind hart, die Zeitfolge bleibt Deutung.
- **„0x4D ist Platzhalter“.** Wahrscheinlich: Bauschema wie 0x40, andere Frau in #146, #147 am selben Ort, Registrierung in STAGE5. TCRF nennt dasselbe, ohne prüfbaren Beleg.
- **Ein fester `Set` als Zapping-Beleg.** `skripte` hat recht, das Muster steht auch in 1021, 1030, 4030. Tragend bleiben Text 9 und die Gitterlage.

## 5. Was noch fehlt

| Weg | Warum | Aufwand |
|---|---|---|
| **Laufprobe der Disc im Emulator** | Alle Aussagen zur Erreichbarkeit sind statisch. Lohnend: `BH1.5-40%.bin` per JUMP auf 11C, 11B, 400, 401, 501, 604. Skill `re15-room-capture` | Mittel |
| **Tonspur der vier Complete-Disc-Filme abhören** | Einziger unausgewerteter Capcom-Text mit Figurenrede, rund 230 s. Fällt dort ein Name der Gegenseite, wäre das ein Primärbeleg | Klein, der Nutzer kann es selbst |
| **Resident Evil Archives S. 218–220** | Fandom stützt „did not cross“ darauf (`fan_dev/page.txt` Z. 540, Fußnote 7). Einzige mögliche Capcom-nahe Wortlautquelle für das Nicht-Treffen | Buch nötig |
| **Capcom Design Works S. 22–30** | Fußnote 8: Roy als Leons Vorgesetzter, Annette | Buch nötig |
| **Trial Edition auf Director's Cut** | Laut TCRF-Kopie und Fandom Restdaten des späten 1.5-Stands, BSS vom 7.11.1996. Liegt nicht im Repo | Disc nötig |
| **Bioflames-Originale** | Wasserzeichen verdecken Untertitel, die Fassungen ohne Wasserzeichen kennen wir nicht. Wayback für Automaten gesperrt | Nutzer im Browser |
| **Weitere Fixpunkt-Stufe** | `k13_fix.py` wertet nur `Ck=1`. `Cmp`, `Ck=0` und Bank 5 ohne Schreiber sind noch als erfüllbar angenommen | Klein |

## 6. Die drei Antworten

**Belegstärke:** *hart belegt* heißt Bytes, Scan oder sichtbares Bild. *Wahrscheinlich* heißt mehrere passende Indizien ohne direkten Beweis. *Nicht belegt* heißt keine Quelle.

### Treffen sich Leon und Elza?

**Antwort: Nein, soweit die Quellen reichen. Für das Ende bleibt das offen.**

| Aussage | Stärke | Beleg |
|---|---|---|
| Kein Skript der Disc führt beide zusammen | **Hart belegt** für den Stand 6.11.1996 | 96 RDT-Dateien, keine Szene mit beiden |
| Kein Bild zeigt beide in einer Szene | **Hart belegt** für das vorhandene Material | 174 Fotos, Video B, vier Complete-Disc-Filme. Nur das Menü zeigt beide, in getrennten Feldern |
| Die Stelle 0:22 ist kein Treffen | **Hart belegt** für das Modell | #028: Weste, kurze Ärmel, nackte Unterarme. Leon trägt immer lange Ärmel |
| 0x4D in Leons 603 ist nicht Elza als Figur | **Wahrscheinlich** | #146, #147 zeigen dort eine andere Frau, Bauschema wie 0x40 |
| Capcom plante zwei getrennte Geschichten | **Hart belegt** als Aussage | Kamiya, Guide S. 261 「ひとり1本」. Kamiya 2019: 「パラレルではなかったけどザッピングは無かった…ただの二本立て…」 |
| Sie begegnen sich im fertigen Spiel nie | **Wahrscheinlich** | Nur Sekundärquellen wortgleich (GamesRadar, Fandom, GamePro.de), Archives S. 218–220 ungeprüft |
| Was im Finale passiert | **Nicht belegt** | Disc 603, 604 ohne Text, 605–607 ohne Dateien, Video zeigt kein Finale |

### Treffen sie die Begleiter des anderen?

**Antwort: Ja, wenigstens teilweise. Das war angelegt, nur der Umfang ist unklar.**

| Paar | Stärke | Beleg |
|---|---|---|
| Leon sieht Sherry | **Hart belegt** auf der Disc, läuft auch | `ROOM11B0 sub01 @0xF9A`, `sub06 @0x107E`, `sub13 @0x11D2`, Text 9. Kein Gespräch. Kein Video |
| Elza und Marvin | **Hart belegt** im Foto, Anlass unbekannt | #058 in 30C, ohne Text. Disc: Typ 0x40 in 6041 bei der Klappliege |
| Elza und Ada | **Hart belegt**, dass Ada in Elzas Szenario auftreten sollte. Ein direktes Treffen ist **wahrscheinlich** | Kamiya S. 261 「女性主人公のシナリオにエイダがチラッと登場するシーン」. Disc 6031 Zweig (4,22), Flag nie gesetzt |
| Leon und John | **Wahrscheinlich** | 6040: John im Raum, Blickszene `sub04` ohne Aufrufer. Kein sicheres Bild, #118 offen |
| Marvin, John, Sherry gemeinsam im Zug | **Hart belegt** für die Disc, für den Spätstand **nicht belegt** | 6040, 6041, STAGE6-Roster. #151 zeigt Marvins Platz frei |

GamesRadar nennt „some shared companions“, ohne Quelle. Kamiya nennt Marvin Leons Partner bis zum Schluss (S. 261).

### Erfährt man vom anderen nur über Dialoge?

**Antwort: Nein. Vor allem über den Zustand der Welt. Über Dialog ist nur eine Stelle im Spätstand bekannt.**

| Aussage | Stärke | Beleg |
|---|---|---|
| Keine erreichbare Disc-Zeile nennt die andere Seite | **Hart belegt** | Einzige Treffer 403 #5, #8 im toten Zweig von 4030 |
| Im Spätstand fällt Leons Name im Elza-Lauf | **Hart belegt** für die Zeile, Sprecher **wahrscheinlich** der Chef | #016 |
| エルザ fällt in Leons Fotos nicht | **Hart belegt** für die 91 Leon-Fotos | `video_a.md` Abschnitt 5. Der Spätstand ist nur in Ausschnitten bekannt |
| Weltzustand Elza zuerst | **Hart belegt** als Dateizustand | 306 Tür, Gitter, Hunde, 309 Licht |
| Weltzustand Leon zuerst | **Hart belegt** als Dateizustand, im Foto bestätigt | 109: 1091 Tür offen, #035 ohne Feuer, #096 mit Feuer |
| Das ist eine bewusste gemeinsame Zeitlinie | **Wahrscheinlich** | Kamiya „nicht parallel“, GamesRadar. Technisch keine Übernahme, Bit 31 setzt nur der Titel |
| Mod-Dialoge über die andere Seite | **Fan-Text**, kein Beleg | `kritik/k11_mod.py` bestätigt die Zeilen in 30C1, 6030, 6031 als Mod-Inhalt |

## 7. Korrekturliste BERICHT.md

Alt ist der aktuelle Wortlaut, neu der Ersatz. Zeilen ohne Eintrag bleiben. #-Nummern sind die Standbilder aus `video_a.md`.

### B-01 Kopf

**Alt:** Stand 2026-10-31. Geprüft: die unveränderte 40%-Disc (`Version_40%/`), die zwei 80%-Videos (`Version_80%/`), dazu Project Umbrella und Forenquellen als Nebenquellen. Alle 291 Textzeilen der Disc stehen mit Übersetzung, Sprecher und Auslöser in [TEXTE_40.md](TEXTE_40.md).

**Neu:** Stand 2026-10-31. Geprüft: die unveränderte 40%-Disc (`Version_40%/`), die zwei Videos in `Version_80%/`, die vier Complete-Disc-Filme (`documentation/`), Kamiyas Aussagen von 1998 und 2019, dazu Project Umbrella und Forenquellen als Nebenquellen. Alle 291 Textzeilen der Disc stehen mit Übersetzung, Sprecher und Auslöser in [TEXTE_40.md](TEXTE_40.md). Höchstens 217 davon können auf der Disc laufen (Abschnitt 12). Fotos aus Video A heißen #000 bis #173.

### B-02 §1, erste Antwort

**Alt:** **Treffen sich Leon und Elza?** Nein, soweit die Quellen reichen. Kein Skript der 40%-Disc führt die beiden zusammen, keine Zeile nennt den jeweils anderen, und in den Videos steht nirgends sicher beide in einem Bild. Die einzige heikle Stelle ist das Video bei 0:22 (Abschnitt 8). Beweisen lässt sich nur „nicht belegt“: Die letzten Räume haben auf der Disc keinen Text, und was im fertigen Spiel noch gekommen wäre, kennt niemand.

**Neu:** **Treffen sich Leon und Elza?** Nein, soweit die Quellen reichen. Kein Skript der 40%-Disc führt die beiden zusammen, keine erreichbare Disc-Zeile nennt den jeweils anderen, und kein Bild zeigt beide in einer Szene: nicht die 174 Fotos von Video A, nicht Video B, nicht die vier Complete-Disc-Filme. Kamiya beschreibt 1.5 als 「ひとり1本」 (Guide Book 1998, S. 261) und als „nicht parallel, aber ohne Zapping, nur zwei Geschichten“ (Twitter, 9.2.2019). Eine wörtliche Capcom-Aussage „sie begegnen sich nie“ liegt uns nicht vor, Resident Evil Archives S. 218–220 sind ungeprüft. Beweisen lässt sich nur „nicht belegt“: 603 und 604 haben auf der Disc keinen Text, 605–607 haben keine Dateien, und das Finale zeigt kein Video.

### B-03 §1, zweite Antwort

**Alt:**
```
**Treffen Leon oder Elza den Begleiter des anderen?** Ja, und das ist der wichtigste Fund.
- **Leon sieht Sherry** in der Garage 11B und ruft ihr nach.
- **Leon trifft John und Sherry** im Zug (Raum 604).
- **Elza trifft Marvin** im Zug (604) und laut 80%-Video auch am Tresen.
- **Elza kann Ada treffen**, im Führerstand (603), als Zweig an einem Story-Flag.
- **Marvin, John und Sherry stehen im Zug zusammen**, in beiden Szenarien.
```

**Neu:**
```
**Treffen Leon oder Elza den Begleiter des anderen?** Ja, und das ist der wichtigste Fund. Auf der Disc läuft davon aber nur wenig ab.
- **Leon sieht Sherry** in der Garage 11B und ruft ihr nach. Das läuft auf der Disc, ein Gespräch gibt es nicht.
- **Leon und John, Sherry** stehen im Zug (Raum 604) im selben Raum. Leons Blick zu ihnen ist geschrieben, aber nicht ausgelöst.
- **Elza und Marvin** stehen im Spätstand in einem Foto zusammen (#058, Tresen in 30C, ohne Text). Auf der Disc steht Marvins Typ in Elzas 604 bei der Klappliege.
- **Elza und Ada:** Kamiya bestätigt, dass Ada im Szenario der Protagonistin kurz auftauchte (Guide Book S. 261). Auf der Disc ist das ein Zweig in 603 an einem Flag, das nie gesetzt wird.
- **Marvin, John und Sherry stehen im Zug zusammen**, in beiden Disc-Szenarien. Für den Spätstand ist das nicht belegt: #151 zeigt Marvins Platz frei, das Foto entstand nach Debug-Sprung.
```

### B-04 §1, dritte Antwort

**Alt:** **Erfährt man vom anderen nur über Dialoge?** Auf der 40%-Disc nicht einmal das. Es gibt **keine einzige erreichbare Zeile**, in der eine Seite die andere beim Namen nennt. Man erfährt es über den Zustand der Welt: Leon findet Türen und Strom so vor, wie Elzas Gruppe sie hinterlassen hat (Abschnitt 4). Dazu kommt das Ende im Zug, das auf der Disc noch ohne Text ist.

**Neu:** **Erfährt man vom anderen nur über Dialoge?** Auf der 40%-Disc nicht einmal das. Es gibt **keine einzige erreichbare Zeile**, in der eine Seite die andere beim Namen nennt. Man erfährt es über den Zustand der Welt: Leon findet Tür, Lüftungsgitter und Licht so vor, wie Elzas Gruppe sie hinterlassen hat, Elza findet den Transporter offen und ohne Feuer (Abschnitt 4). Im Spätstand fällt mindestens einmal ein Name: In #016 steht im Elza-Lauf 「……レオン」 (Abschnitt 11). Dazu kommt das Ende im Zug, das auf der Disc noch ohne Text ist.

### B-05 §1, Reihenfolge

**Alt:** **Die Reihenfolge ist erkennbar:** Alle drei Weltspuren zeigen in dieselbe Richtung. Elzas Gruppe ist zuerst da, Leon kommt nach und sieht die Folgen. Am Ende laufen beide Wege am Zug zusammen. Das ist ein Vorläufer des späteren Zapping-Systems.

**Neu:** **Eine Gesamtreihenfolge lässt sich nicht ableiten.** Die Spuren laufen in beide Richtungen: In der Fabrik ist Elzas Gruppe zuerst da, am Transporter in 109 Leon. Leon startet auf dem Dach, Elza in der Halle 103, ihre Wege überlappen also. Am Ende führen beide Wege über dieselben Türen zum Zug. Das Zapping-System ist das nicht: Die Spuren stehen fest in den Raumdateien, nichts wird aus dem anderen Lauf übernommen, und Capcom trennt selbst: nicht parallel, aber kein Zapping.

### B-06 §2, Tabelle

**Alt, Zeile Disc:** `| **40%-Disc** `Version_40%/` | Capcom-Stand, 334 Dateien, 96 Räume, alle Texte Japanisch | Exe-Stempel `CAPCOM-Nov  6 1996-13:09:19-CAPCOM`, ISO-Datum 1996-11-06 | Hart. Stand: Text, Skripte, Modelle |`

**Neu:** `| **40%-Disc** `Version_40%/` | Capcom-Stand, 334 Dateien im Image, 96 RDT-Dateien zu 51 Raumnummern, alle Texte Japanisch. Laut Hidden Palace der Stand der „PlayStation Museum“-Fassung | Exe-Stempel `CAPCOM-Nov  6 1996-13:09:19-CAPCOM`, ISO-Datum 1996-11-06, Hidden Palace | Hart. Stand: Text, Skripte, Modelle |`

**Alt, Zeile Videos:** `| **80%-Videos** `Version_80%/` | Filmaufnahmen eines späteren Capcom-Stands | Bioflames/Complete-Disc-Material | Hart für das, was man sieht. Bildqualität oft schlecht |`

**Neu, drei Zeilen:**
```
| **Video A** `Biohazard 1.5 Final Build.mp4` | Keine Filmaufnahme. Diashow von Enigmatism415 (2013): 173 Bioflames-Fotos und ein SurvivHor-Foto, je 0,8 s, Musik aus dem 40%-Build | 174 Standbilder gemessen, YouTube qnWhK82FzdU | Hart für das einzelne Bild. Bildfolge stammt vom Uploader, die Fotos aus einer Debug-Sitzung (#031, #148, #159) |
| **Video B** `Resident Evil 1.5 (All Footage).mp4` | Spielaufnahmen gemischter Herkunft, Gesamtquelle ungeklärt | Wasserzeichen BIOFLAMES und SURVIVHOR | Hart für das, was man sieht. Bildqualität oft schlecht |
| **Complete-Disc-Filme** `documentation/` | Vier Capcom-Filme, als Fan-Mitschnitt. In Video B enthalten | Abgleich r bis 1,00 | Hart für Bild, Ton noch nicht ausgewertet |
```

### B-07 §2, Mod

**Alt:** Die Mod übersetzt nicht nur. Sie ergänzt 144 Raumdateien und schreibt Szenen neu.

**Neu:** Die Mod übersetzt nicht nur. Ihr Baum hat 144 RDT-Dateien mehr als die Disc (240 gegen 96), wie viel davon aus der Team-IGAS-Basis und wie viel von Fans stammt, ist nicht getrennt. Sie schreibt Szenen neu.

### B-08 §3, Tabelle vollständig ersetzen

```
| Paar | 40%-Disc | Spätstand (Fotos, Filme) | Urteil |
|---|---|---|---|
| **Leon – Elza** | Kein Treffen, kein Name in einer erreichbaren Zeile | Kein gemeinsames Bild. #016: レオン im Elza-Lauf | **Kein Treffen belegt** |
| **Leon – Sherry** | 11B0: Sherry läuft vorbei, Leon: „H-he!“ (`sub06 @0x107E`, `sub07 @0x10B6`, `sub13 @0x11D2`), läuft auf der Disc. 604: Sherry steht im Raum, Leons Blick `sub04` hat keinen Aufrufer | Kein Bild | **Ja, Sichtkontakt ohne Gespräch** |
| **Leon – John** | 604: John steht im Raum. Blick und Abgang (`Plc_neck @0x6EA`, `sub05 @0x74A`) haben keinen Aufrufer. 3000: Modell John, der Text nennt Marvin | Kein sicheres Bild, #118 offen | **Angelegt, ohne Text** |
| **Elza – Marvin** | 604: Typ 0x40 bei der Klappliege (`@0x5EA`). Elzas Gang und Blick (`sub06`, `sub09`, `Plc_neck @0x72E`) haben keinen Aufrufer | #058, 0:46.5, Tresen in 30C, ohne Text | **Ja im Foto**, Anlass unbekannt |
| **Elza – Ada** | 603: Ada statt Sherry, wenn Flag (4,22) gesetzt ist (`@0x388-0x3A8`). Setzer liegt nicht auf der Disc | Kein Bild | **Angelegt**, Kamiya bestätigt Adas kurzen Auftritt im Szenario der Protagonistin (Guide Book S. 261) |
| **Marvin – John – Sherry** | 604, beide Dateien: alle drei im Raum | #151: Marvins Platz frei, Debug-Sprung | **Ja auf der Disc**, Spätstand offen |
| **Leon – Ada, Marvin** | Durchgehend | Mehrfach, im Film B 5:04.5 | Eigenes Szenario |
| **Elza – John, Sherry** | Durchgehend | Mehrfach | Eigenes Szenario |
| **Leon – zweite Frau im Kittel** | 603: Figur 0x4D. Ada nur bei (4,22)=1 und (4,23)=0, das kommt auf der Disc nicht vor | #145–#147, #152, #154: helles Haar, Kittel, lange dunkle Hose | **Ja**, Name nirgends genannt, nach Haar und Beinen nicht Ada |
| **Elza – zweite Frau im Kittel** | Kein Platz in 6031 | – | Nicht belegt |
| **Beide – Polizeichef** | Skripte und Text von 1150 und 1151 gleich, Satz „Mir geht es gut…“ | #089 Leon am Sofa, #016 Elza am Sofa mit 「……レオン」. B 2:54–2:57 Elza im Büro, Sofa leer | Beide |
| **Elza – Roy** | 1021 mit John | #027, #028 wahrscheinlich | Nur Elza |
```

### B-09 §4 vollständig ersetzen

```
## 4. Wie die Szenarien ineinandergreifen

Mehrere Stellen zeigen, dass eine Datei den Zustand fest vorgibt, den das andere Szenario erst herstellt. Die Dateizustände sind hart belegt, die Zeitfolge bleibt Deutung. Ein festes `Set` allein belegt nichts, dieses Muster steht auch ohne Bezug zur Gegenseite im Skript (1021, 1030, 4030). Tragend sind Text und Objektlage.

| Ort | Wer zuerst | Elza-Szenario | Leon-Szenario | Beleg |
|---|---|---|---|---|
| **Hintertür 306** | Elza | Sherry kriecht hinein: „Das Schloss an der Hintertür mache ich von innen auf.“ | Leon liest: **„Irgendwann ist das Schloss aufgemacht worden…“** | `ROOM3060 @0xF20` startet `sub02`, `@0xF9A Message_on 9`. Slot 1 führt nach 309, beide Sätze nutzen dasselbe Verb 外す |
| **Gitter und Hunde 306** | Elza | Lüftungsgitter an der Wand, Sherrys Szene löst es | Gitter liegt am Boden, drei Hunde wie in Elzas Zustand danach | `ROOM3060 @0xF74` = `ROOM3061 @0xFB4` (34 Byte), `@0xF38–0xF60` = `@0xF78–0xFA0` |
| **Schutzschalter 309** | Elza | Licht aus, Schalter muss erst umgelegt werden | Licht an, Text: **„Das Licht ist bereits an…“** | `ROOM3090 @0x1412 Set(4,8)=1`, nur Zweig „Licht an“. Text 0 nutzt auch 3091 nach dem Schalten. Die Kiste steht in 3090 an ihrer Startlage, Leons Raum ist also kein vollständiger Nachzustand |
| **Transporter 109** | Leon | Tür offen, kein Feuer, keine Ada | Transporter brennt, Tür zu, Ada ruft von innen. Nach dem Löschen Tür offen | `ROOM1091 @0x21BC` = `ROOM1090 @0x2320` (8 Byte), Feuer `@0x2204 ff.` nur bei Leon. Foto #035 Elza ohne Feuer, #096 Leon mit Feuer |

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
```

### B-10 §5, Einleitung

**Alt:** Orte aus den 531 dekodierten Disc-Hintergründen ([Übersicht](belege/raeume_40prozent.jpg)). Die Reihenfolge ist aus Flags und Texten erschlossen. „–“ heißt: auf der Disc nichts dazu.

**Neu:** Orte aus den 531 dekodierten Disc-Hintergründen ([Übersicht](belege/raeume_40prozent.jpg)). Die Reihenfolge ist aus Flags und Texten erschlossen. „–“ heißt: auf der Disc nichts dazu. „Tot“ heißt: Der Text steht im Skript, läuft auf der Disc aber nicht (Abschnitt 12).

### B-11 §5, Zeilen ersetzen

| Zeile | Alt | Neu |
|---|---|---|
| neu vor Dach | – | `| **Start** | Dach 117 (`0x8001d2b0`) | Halle 103 (`0x8001d32c`) |` |
| Dach 117 | `| **Dach 117** | Kein Text (Video: Leon auf dem Dach) | John: „Elza, beeil dich! … Wir müssen das Ding runterstoßen!“ |` | `| **Dach 117** | Disc ohne Text. Im Spätstand zwei Zeilen (B 0:19–0:23.6): おい 冗談だろ？ / …クソッなんてこった！ | John: „Elza, beeil dich! … Wir müssen das Ding runterstoßen!“ |` |
| Chefbüro | `| **Chefbüro 115** | „Mir geht es gut. Denk an deine eigene Sicherheit.“ | Derselbe Satz |` | `| **Chefbüro 115** | „Mir geht es gut. Denk an deine eigene Sicherheit.“ | Disc: derselbe Satz. Foto #016: 「……レオン」, Sprecher offen |` |
| Garage | `| **Garage 11B** | **Sieht Sherry**, Marvin: „Leon! Du lebst!“, Rolltor durchbrochen, Plan: Kanaldeckel, „Lass die Frau runter“ | Findet Sherry, „Warte brav im Wagen“ |` | `| **Garage 11B** | **Sieht Sherry.** Marvin: „Leon! Du lebst!“, Rolltor durchbrochen, Plan: Kanaldeckel, „Lass die Frau runter“ (tot: 11C0 fragt Bank 8 ab; im Spätstand zu sehen, #169–#172) | Findet Sherry, „Warte brav im Wagen“ |` |
| Parkgarage | `| **Parkgarage 11C** | Ada: Marvin ist umgekehrt, um dich zu suchen. Kampf, „Was für ein Ungeheuer“ | – |` | `| **Parkgarage 11C** | Ada: Marvin ist umgekehrt, um dich zu suchen. Kampf. Abschluss „Was für ein Ungeheuer“ tot | – |` |
| Kanal | `| **Kanal** | Ohne Text | Ohne Text, John im Skript (20B1) |` | `| **Kanal** | Ohne Text. Im Film Leon, Marvin, Ada (B 5:04.5) | Ohne Text. John steht in 20B1, dorthin führt aber keine Tür |` |
| Fabrikhof | Leon-Spalte `Leon, Marvin: tote Polizisten, Umbrella-Ermittler. Ada schon vorgegangen, „nicht mal die Miene verzogen“` | `Leon, Marvin: tote Polizisten, Umbrella-Ermittler. Ada schon vorgegangen, „nicht mal die Miene verzogen“. Der Spätstand hat einen anderen Wortlaut (#118)` |
| Hinterhof | Leon-Spalte `**Tür schon offen**` | `**Tür schon offen, Gitter am Boden**` |
| Raum 309 | Elza-Spalte `**Sherry spielbar**, rettet Elza und John. John: „Wenn es mein Leben kostet“. Sherry: „Hier hat mein Papa gearbeitet“` | `**Als Sherry-Abschnitt angelegt**, die Figur bleibt auf der Disc Elza. Im Spätstand ist Sherry allein zu sehen (#050–#055). John: „Wenn es mein Leben kostet“. Sherry: „Hier hat mein Papa gearbeitet“` |
| Halle 30E | Leon-Spalte `Marvin am Bein verletzt, Ada taucht auf, Halle fällt` | `Marvin verletzt, kann nicht laufen (Disc 東をやられて, vermutlich das Bein), Ada taucht auf, Halle fällt` |
| Aufzug 307 | Leon-Spalte `Ada: „In die Hölle…“` | `Ada: vermutlich „In die Hölle…“ (Disc 地仲さ)` |
| Labor 400 | Leon-Spalte `Leon hält auf, „Kümmere dich um Marvin!“` | `Geplant: Leon hält auf, „Kümmere dich um Marvin!“ (tot)` |
| 401 / 501 | `| **401 / 501** | **Ada: „Ich gehöre zu Umbrella.“** Impfstoff aus dem Virusstamm, Ada geht eigene Wege. Später: „Marvin, wach auf!!“, „Schnell zum Schutzraum!“ | **Sherry vom Virus zerfressen**, Elza holt Impfstoff, Sherry wacht auf. Monster zerstörte die Energie, Explosion droht, „Schutzraum Nr. 2 kann ich öffnen“ |` | `| **401 / 501** | **Ada: „Ich gehöre zu Umbrella.“** Impfstoff aus dem Virusstamm, Ada geht eigene Wege. „Marvin, wach auf!!“, „Schnell zum Schutzraum!“ sind tot | Alles tot, auf der Disc stehen nur Sherry und John im Raum: **Sherry vom Virus zerfressen**, Elza holt Impfstoff, Sherry wacht auf. Monster zerstörte die Energie, Explosion droht, „Schutzraum Nr. 2 kann ich öffnen“ |` |
| Führerstand | `| **Führerstand 603** | Frau im Kittel oder Ada, lange Szene, WARNING-Konsole blinkt | Sherry oder Ada |` | `| **Führerstand 603** | Figur 0x4D, im Foto die zweite Frau im Kittel. Ada nur als Zweig. Lange Szene, WARNING-Konsole blinkt | Sherry, Ada nur als Zweig |` |
| Waggon | `| **Waggon 604** | Marvin, John, Sherry | Marvin, John, Sherry |` | `| **Maschinenraum 604** | Marvin an der Tür aus 514, John, Sherry | Marvin bei der Klappliege, John, Sherry |` |
| Fußnote | Figuren je Raum: `werkzeuge/figuren.py`. Der Disc-Text nennt Ada schon エイダ, nicht Linda. Project Umbrella führt in einer anderen Build-Fassung „Linda“ und „Brian“. | Figuren je Raum: `werkzeuge/figuren.py`. Der Disc-Text nennt Ada schon エイダ, nicht Linda. „Linda“ und „Brian“ stehen bei Project Umbrella nur im englischen Fließtext, in keiner Spielzeile, ihre Herkunft ist unklar. Beide Figuren haben denselben Türgraphen. Einzige Abweichung ist Slot 2 in 20B (Leon 300, Elza 203), und nach 20B führt keine Tür. |

### B-12 §6

**Alt:** Beide Räume haben auf der Disc **keinen Textblock**, die Choreografie ist aber geschrieben. 603 ist der Führerstand, 604 der Waggon mit Sitzbänken ([Hintergründe](belege/hintergruende_603_604.jpg)).

**Neu:** Beide Räume haben auf der Disc **keinen Textblock**, die Choreografie ist aber geschrieben. 603 ist der Führerstand (`UNTENSHITSU`), 604 laut EXE ein Maschinenraum (`DOURYOKUSHITSU`, 動力室, `@0x80071880`) mit Klappliege ([Hintergründe](belege/hintergruende_603_604.jpg)). 605–607 tragen denselben Namen, haben aber keine Dateien. STAGE6.BIN enthält nur die fünf Figuren-Handler, also weder Gegner noch Abspann.

**Alt:** **604 Leon (`ROOM6040`):** Marvin, John und Sherry stehen an festen Plätzen. In `sub04` dreht Leon den Kopf exakt zu Sherrys Position, dann zu Johns, und John läuft los.

**Neu:** **604 Leon (`ROOM6040`):** Marvin, John und Sherry stehen an festen Plätzen (`sub00`). Beim Betreten läuft `sub06`: Leon geht vier Wegpunkte ab, ohne Blickbefehl. Geschrieben, aber ohne Aufrufer ist `sub04`: Leon dreht den Kopf zu Sherrys Position, dann zu Johns, und John läuft los.

**Alt:** **604 Elza (`ROOM6041`):** Eigene Choreografie, also keine Kopie. Marvin steht anderswo und hat ein anderes Startbyte, Sherry ebenfalls. Elza blickt zu Marvin (`sub06`, `sub09`), in `sub11` zwischen John und Sherry hin und her.

**Neu:** **604 Elza (`ROOM6041`):** Eigene Choreografie, also keine Kopie. Marvin steht auf Etage 1 bei der Klappliege (`@0x5EA`), Sherry steht an anderer Stelle. Beim Betreten läuft `sub11`: Elza blickt dreimal zwischen zwei Punkten hin und her, die grob in Richtung John und Sherry liegen, Sherry dreht sich zu ihr (`sub12`). Elzas Gang zu Marvin (`sub06`, `sub09`) hat keinen Aufrufer. Pose und Anlass stehen nicht auf der Disc.

**Alt:** Flag (4,22) und (4,23) werden in keinem Raum der Disc gesetzt, der Setzer lag in nicht vorhandenen Räumen. Dass Ada überhaupt davon abhängt, heißt: Ihr Überleben oder ein Routenwechsel war als Zustand geplant.

**Neu:** Flag (4,22) und (4,23) werden in keinem Raum der Disc gesetzt, auch nicht über AOT oder Code. Auf der Disc steht in 603 also immer 0x4D bei Leon und Sherry bei Elza. Woher das Flag kommen sollte und was es bedeutet, ist unklar. Eine Vermutung ist ein Zustand zu Adas Überleben oder Route, ebenso möglich ist ein unfertiger Testzweig.

**Alt:** **Der andere Hauptcharakter fehlt in beiden Waggons.** Weder in 6040 noch in 6041 steht er im Skript. Wo Elza in Leons Ende ist und umgekehrt, sagt die Disc nicht. Die Mod beantwortet das mit eigenem Text („She's out there fighting the monsters!“). Das ist Erfindung.

**Neu:** **Der andere Hauptcharakter fehlt in 603 und 604.** In keiner der vier Dateien steht er als Figur im Skript. Typ 0x4D in 6030 trägt Elzas Körper und Skelett mit Adas Clips, Typ 0x40 in 6041 Leons Körper mit Johns Clips. Beides liest sich nach Platzhalter, sicher ist das nicht (Abschnitt 8). Wo Elza in Leons Ende ist und umgekehrt, sagt die Disc nicht, 605–607 fehlen. Die Mod beantwortet das mit eigenem Text („She's out there fighting the monsters!“). Das ist Erfindung.

**Alt:** Einschränkung: Ausgeführt werden im Stand nur `sub06` (6040), `sub11` (6041, 6030) und `sub08` (6031). Die übrigen Szenen sind fertig geschrieben, aber ihr Auslöser fehlt noch.

**Neu:** Einschränkung: Ausgeführt werden im Stand nur `sub06` (6040), `sub11` (6041, 6030) und `sub08` (6031). In 604 legt `main00` drei Zonen an, abgefragt wird nur eine, die übrigen Szenen dort haben keinen Aufrufer. In 603 sind `sub02`, `sub05`, `sub08` Fassungen derselben Szene mit denselben Wegpunkten.

### B-13 §7

**Alt:** Das stand bisher nirgends im Projekt:

**Neu:** Im Projekt war das bisher nicht ausgewertet. Die Fanszene kennt es (THIA-Faden, TCRF-Kopie `information74.txt` Z. 197):

**Alt:** - **Sherry:** Elzas 3091 setzt zwei Türen auf den Text 「はやくエルザたちを助けなければ...」, **„Ich muss Elza und die anderen schnell retten…“** (`@0x1464`, `@0x146E`). Das kann Elza nicht sagen. „Ich komme nicht an den Schalter…“ passt zu einem Kind.

**Neu:** - **Sherry:** Elzas 3091 setzt zwei Türen auf den Text 「はやくエルザたちを助けなければ...」, **„Ich muss Elza und die anderen schnell retten…“** (`@0x1464`, `@0x146E`). Das kann Elza nicht sagen. „Ich komme nicht an den Schalter…“ passt zu einem Kind. Im Spätstand ist Sherry dort allein zu sehen (#050–#055, #051 auf der Kiste am Kasten mit 「電げんがある」). Die EXE kennt sie fest als Spielfigur: `0x8003258c`, `0x80032898`, `0x80038190` vergleichen die Figur mit 0x0F.

**Alt:** - **Technik:** Der Raumlader tauscht die Figur, wenn der Wert sich ändert (`@0x80039768`). Auf der Disc schreibt aber kein Skript diesen Wert. Die Abschnitte sind vorbereitet, nicht verdrahtet.

**Neu:** - **Technik:** Der Raumlader tauscht die Figur, wenn der Wert sich ändert (`@0x80039768`). Auf der Disc schreibt aber kein Skript diesen Wert. Die Abschnitte sind vorbereitet, nicht verdrahtet: In 3091 bleibt die Figur Elza, und Marvins Text in 300 läuft nie.

**Alt:** - **John:** PL0E liegt bereit, eine Skriptstelle dazu gibt es nicht.

**Neu:** - **John:** PL0E liegt bereit, eine Skriptstelle dazu gibt es nicht. Für 0x0D und 0x0E gibt es auch keine Sonderlogik in der EXE.

### B-14 §8, Zeilen ersetzen

| Falle | Alt | Neu |
|---|---|---|
| Neuer erster Eintrag | – | `| **Fotos als Szenenfolge lesen** | Video A ist eine vom Uploader sortierte Diashow. #031, #148, #159 zeigen das Debug-Menü, die Fotos entstanden also teils nach Sprung. Ein einzelnes Foto belegt den Zustand, nicht den Ablauf |` |
| Video 0:22 | `| **Video 0:22: Elza kniet neben einem Polizisten, der aussieht wie Leon** | Typ 0x47 trägt auf der Disc dieselbe R.P.D.-Kunst wie Leon, in der Elza-Szene heißt die Figur Roy. Sehr wahrscheinlich Roy, die Mod führt die Szene auch so. **Das Bild allein beweist es nicht**, es hat keinen Text und einen blauen, unfertigen Hintergrund ([Bild](belege/video_elza_roy_marvin_john.jpg)) |` | `| **Video 0:22: Elza kniet neben einem Polizisten mit Leons Haar** | Das sind zwei Fotos, #027 und #028. Der Mann trägt Weste mit zwei hellen Trägern, rundes Abzeichen und kurze Ärmel, Leon trägt in allen Fotos lange Ärmel. Das schließt Leons Modell aus. Typ 0x47 trägt auf der Disc Leons Kunst, das erklärt dieses Foto nicht. Wahrscheinlich Roy: Typ 0x47 steht in `ROOM1021 @0x1D20` neben John, #026 und #030 sind Raum 102. Der Name steht in keinem Bild, der Hintergrund ist eine blaue Fläche ([Bild](belege/video_elza_roy_marvin_john.jpg)) |` |
| 0x4D | `| **Typ 0x4D in Leons 603 trägt Elzas Anzug** | Auf der Disc ist 0x4D Elzas Kunst auf Adas Rig ([Vergleich](belege/vergleich_4D_gegen_elza.png)). Sieht nach Treffen aus, ist aber ein Platzhalter: Typ 0x40, 0x45, 0x47 teilen sich ebenfalls einen Blob (md5 `0c1f2a`). Im 80%-Video steht an dieser Stelle die Frau im Kittel ([Bild](belege/video_kittel_fuehrerstand.jpg)). Der Raumvergleich bei 1:57 ist Sichtbefund, die Rechnung schlug wegen der Bildqualität nicht an |` | `| **Typ 0x4D in Leons 603 trägt Elzas Anzug** | Auf der Disc ist 0x4D Elzas Körper und Skelett (15/15 Offsets wie PL04) mit Adas Clips ([Vergleich](belege/vergleich_4D_gegen_elza.png)). Sieht nach Treffen aus, ist aber wahrscheinlich ein Platzhalter: Typ 0x40, 0x45, 0x47 sind ebenso gebaut (md5 `0c1f2a`, Leons Körper mit Johns Clips). Im Spätstand steht in 603 eine andere Frau, helles Haar, Kittel, lange dunkle Hose ([Bild](belege/video_kittel_fuehrerstand.jpg)), #146, #147, Ort rechnerisch bestätigt. STAGE5.BIN registriert 0x4D ebenfalls, das passt zu einer Laborfigur. TCRF vermutet Annette, ein Name steht in keiner Capcom-Quelle zu 1.5. Beweisen lässt sich „Platzhalter“ nicht |` |
| Marvin | `| **Marvin sieht im 40%-Stand aus wie ein Polizist in Leons Art** | Typ 0x40 ist Platzhalter. Der Marvin des 80%-Stands (hellblaues Hemd) ist ein eigenes, späteres Modell |` | `| **Marvin sieht im 40%-Stand aus wie ein Polizist in Leons Art** | Typ 0x40 ist Platzhalter: Leons Körper mit Johns Clips. Auch der Polizeichef nutzt ihn. Der Marvin des Spätstands (hellblaues Hemd) ist ein eigenes, späteres Modell. In Elzas 6041 ergibt sich „Marvin“ nur über den Typ |` |
| John in 3000 | `| **John steht in Leons 3000** | Platzhalter. Text und Video 1:33 zeigen Marvin, die Szene ist aus 3001 kopiert |` | `| **John steht in Leons 3000** | Platzhalter, denn der Text nennt Marvin (300 #2, #6). Das Foto hilft nicht: #116 (1:33) ist der Hinterhof 306, im Fabrikhof (#118, 1:34.5) ist der Mann nicht sicher zuzuordnen, seine Hose ähnelt eher Johns. 3000 hat eigene Lage und eigene Texte, eine Kopie aus 3001 ist nicht belegt. Für ein Treffen Leon–John reicht das nicht |` |
| Parkposition | `| **Ada und Marvin in Elzas 11B1, Ada in 1091** | Parkposition (−30000, −30000), kein Skript spricht sie an. Kopier-Rest |` | `| **Ada und Marvin in Elzas 11B1, Ada in 1091, John in Leons 10B0** | Parkposition (−30000, −30000), kein Skript spricht sie an. Kopier-Rest |` |
| Textblock | `| **Beide Dateien tragen denselben Textblock** | Sagt nichts. Entscheidend ist, welche Nummern das Skript aufruft. 501: Nr. 6–16 werden gar nicht aufgerufen |` | `| **Beide Dateien tragen denselben Textblock** | Sagt nichts, das gilt für alle 25 Paare. Entscheidend ist, was läuft. 501: Nr. 6–16 werden gar nicht aufgerufen, Nr. 0–5 und 17–37 stehen in Subs ohne Aufrufer |` |
| Neue Zeile | – | `| **Ein Fan-Modell passt zu einem Foto** | Kein Beleg. Die Mod hat ihre Modelle nach diesen Fotos gebaut (README Z. 180, 274, 275, 387) |` |
| Complete Disc | `| **Complete-Disc-Text „Leon und Elza stürmen das Labor“** | Werbetext zu Clips, die beide getrennt zeigen |` | `| **Complete-Disc-Titel 「研究所に踏み込むレオンとエルザ」, 「警察署にたてこもるレオンとエルザ」** | Werbetext. Film 1 und 4 wechseln zwischen Leon und Elza, kein gemeinsames Bild. Den Titeltext kennen wir nur über Project Umbrella |` |

### B-15 §9

**Alt, Zeile 30C1:** `| 30C1 | „I'm Marvin Branagh, RPD. I'm here with Leon Kennedy.“ | Raum nicht auf der Disc. Video zeigt die Szene ohne lesbaren Text |`

**Neu:** `| 30C1 | „I'm Marvin Branagh, RPD. I'm here with Leon Kennedy.“ | Raum nicht auf der Disc. Foto #058 hat keinen Text |`

**Alt, letzte Zeile:** `| 604 | „Leon and Elza models“ | Laut README Mod-Zusatz, auf der Disc nicht vorhanden |`

**Neu:** `| 604 | „Leon and Elza models“ | Laut README Mod-Zusatz. Auf der Disc steht in 604 weder Leon noch Elza als Figur. Mit Leons Körper steht dort nur Typ 0x40, Marvins Platzhalter |`

**Alt, Einleitung:** Diese englischen Zeilen sind **kein Capcom-Text**. Die Szene Elza–Marvin selbst ist echt (Video 0:46.8), ihr Dialog nicht bekannt.

**Neu:** Diese englischen Zeilen sind **kein Capcom-Text**. Die Szene Elza–Marvin selbst ist echt (Foto #058), ihr Dialog ist nicht bekannt. Die Zeile 「……レオン」 steht in der Mod nicht.

### B-16 §10, Tabelle und Schlusssatz vollständig ersetzen

```
Video A = `Biohazard 1.5 Final Build.mp4`, Video B = `Resident Evil 1.5 (All Footage).mp4`. Bei Video A zählen die Foto-Nummern, die Zeit nennt den Beginn des Fotos. Die Bildfolge sagt nichts über den Ablauf.

| Stelle | Inhalt | Disc |
|---|---|---|
| A #012, 0:09.6 | John: „Elza! Geh nicht aufs Dach!“ | Nicht auf der Disc |
| A #016, 0:12.8 | Elza am Sofa des Chefbüros, Untertitel 「……レオン」 | Nicht auf der Disc, Abschnitt 11 |
| A #027, #028, 0:21.6 | Elza neben verletztem Polizisten, blaue Fläche | Abschnitt 8 |
| A #032, 0:25.6 | Elza: …内に女の子がいるの / 戻らなきゃ | 10B #0 ähnlich, anderer Wortlaut |
| A #035, 0:28.0 | Elza in 109, kein Feuer | Abschnitt 4 |
| A #037, 0:29.6 | Elza kniet vor Sherry: …まあ 大きな時計ね！ | Nicht auf der Disc, Lesung nach Project Umbrella und Enigmatism415 |
| A #048, 0:38.4 | …使われてないはずだ | 300 #12 mit ねぇ statt ない |
| A #049, 0:39.2 | Sherry: …エルザ / 私 ヒミツの入り口を知ってるの | 306 #3 gleich |
| A #050–#055, 0:40.0 | Sherry allein in 309 und 30C, #051 an der Kiste: 電げんがある | Text nicht auf der Disc, Raum 30C fehlt |
| A #058, 0:46.5 | **Elza und Marvin am Tresen**, kein Text | Raum fehlt auf der Disc |
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
```

### B-17 §11 vollständig ersetzen

```
## 11. Nebenquellen

- **Kamiya, Official Guide Book 1998, S. 261** (Scan): 「シナリオは現在のようなザッピングシステムではなく、ひとり1本。女性主人公のシナリオにエイダがチラッと登場するシーンがあったり、…マービンが、レオンの先輩として最後まで行動をともにするパートナーだったりしました。」
- **Kamiya, Twitter 9.2.2019**, auf die Frage nach Leon und Elza: 「1.5の時はパラレルではなかったけどザッピングは無かった…ただの二本立て…」. Original nicht abrufbar, Wiedergabe in blog.esuteru.com. Über ein Treffen sagt er nichts.
- **Project Umbrella** transkribiert unsere Quellen, keine andere Fassung: die Bioflames-Fotos (Video A), die Complete-Disc-Filme und die PlayStation-Museum-Disc, laut Hidden Palace der Stand unserer 40%-Disc. „Linda“ und „Brian“ stehen nur im englischen Fließtext. Die Seite führt im Chefbüro eine Elza-Zeile **「・・・レオン」 („…Leon“)**. Sie steht in Video A (#016, 0:12.8): Elza von hinten am Sofa, daneben der Verletzte. Auf der 40%-Disc fehlt sie (Textblock 115 hat nur eine Zeile, Skripte 1150/1151 sind byte-gleich, ROOM115U.SCD enthält keinen Text). Wer spricht, steht nicht im Bild. Project Umbrella schreibt Elza, der Übersetzer Enigmatism415 Chief Irons. Auf der Disc spricht an dieser Stelle der Verletzte (`ROOM1151 @0xE5E`, `@0xE66`), der Chef ist also wahrscheinlicher. Bei Leon zeigt dieselbe Kamera keinen Text (#089). Belegt ist nur: Im Elza-Lauf des Spätstands erscheint Leons Name. Dass Elza ihn kennt, folgt daraus nicht.
- **GamesRadar** (2018) schreibt, beide träfen sich nie und fänden die Folgen des anderen: Leon lösche den Laster, Elza finde ihn später, Elza besiege Birkin, Leon finde ihn später, dazu „some shared companions“. Eine Quelle nennt der Artikel nicht. Der Laster passt zu Disc und Fotos, die Birkin-Stelle ist nicht geprüft.
- **GamePro.de** (2015) schreibt: „Leon S. Kennedy und Elza Walker begegnen sich im Spielverlauf von Resident Evil 1.5 nicht.“ Das ist eine Bildunterschrift ohne Quelle.
- **Fandom** stützt „did not cross“ auf Resident Evil Archives S. 218–220. Diese Seiten haben wir nicht gesehen.
- **Forum The Horror Is Alive** nennt: „In the final train, everyone is there, Marvin, John, Sherry and Annette. Except Ada. So, maybe Ada is still a spy?“ Das ist eine Fan-Beobachtung aus einem Durchlauf des geleakten Builds, bekannt nur aus Suchauszügen. „Annette“ meint vermutlich Figur 0x4D, die in 603 steht, nicht in 604. Die Disc bestätigt Marvin, John und Sherry in 604.
- **TCRF** liegt als Kopie in `info/Resident_Evil_und_Playstation_Information/information74.txt`. Es nennt 0x4D wahrscheinlich einen Platzhalter für Annette und Irons mit Leons Modell.
```

### B-18 §12 ersetzen

**Alt:** - Viele Räume des Plans fehlen auf der Disc, die Mod musste 144 Raumdateien ergänzen. Darunter ist 30C mit der Marvin-Szene. Was dort lief, zeigt nur das Video.

**Neu:** - Viele Räume des Plans fehlen auf der Disc. Die EXE nennt Räume ohne Datei (10A, 127, 20A, 40B, 505, 605–607). Der Mod-Baum hat 144 RDT-Dateien mehr, darunter 30C. Was dort lief, zeigt nur das Foto #058.

**Alt:** - Sprecher stehen nicht im Text, ich habe sie aus dem Zusammenhang gesetzt.

**Neu:** - Sprecher stehen nicht im Text, ich habe sie aus dem Zusammenhang und der Regie im Skript gesetzt. In 300 gibt es keine Regie je Zeile.

**Alt:** - Fehlende Kanji sind aus dem Zusammenhang ergänzt und in TEXTE_40.md markiert.

**Neu:** - Fehlende Kanji und Ersatzzeichen wie 東 sind aus dem Zusammenhang ergänzt. Das betrifft 43 Zeilen, ihre deutsche Fassung ist Lesart und in TEXTE_40.md markiert.

**Alt:** - 25 Zeilen werden in keinem Skript aufgerufen, vor allem 501 #6–16 und die Texte in 50A, 506, 512, 514.

**Neu:** - Von 291 Zeilen können auf der Disc höchstens 217 laufen. 23 werden in keinem Skript aufgerufen (vor allem 501 #6–16 und 50A). 51 stehen nur in totem Code: 400 #5–8, 401 #19–22, 501 #0–5 und #17–37 in Subs ohne Aufrufer, 11B #10–21 und 11C #7–9 hinter Bank 8 in 11C0, 300 #1 hinter einer Spielfigur, die es nicht gibt. Das zeigt den Plan, nicht den Ablauf der Disc. Die Prüfung ist statisch, ein Emulator lief nicht.

**Alt:** - Die Videos sind stark verzerrt. Figuren-Zuordnungen dort sind Sichtbefund.

**Neu:** - Die Bilder sind stark verzerrt. Figuren-Zuordnungen dort sind Sichtbefund. Die Fotos von Video A stammen aus einer Debug-Sitzung, ein Foto belegt keinen Ablauf.

**Alt:** - Durchsicht: Video A jede Sekunde, Video B alle 3 Sekunden, verdächtige Abschnitte enger. Sehr kurze Einstellungen können durchgerutscht sein.

**Neu:** - Durchsicht: Video A alle 174 Standbilder einzeln, kürzere Einstellungen gibt es dort nicht. Video B zwei Bilder je Sekunde plus Schnittliste, 43,8 s ohne erkennbare Spielfigur. Die Tonspur von Video B und den Complete-Disc-Filmen ist nicht ausgewertet.

### B-19 §13

**Alt:** - **Story-Regel:** „Leon und Elza begegnen sich nie“ bleibt gedeckt. „Elza kann Ada begegnen“ ist jetzt durch 6031 belegt. Neu: Leon sieht Sherry und trifft John, Elza trifft Marvin.

**Neu:** - **Story-Regel:** „Leon und Elza begegnen sich nie“ bleibt gedeckt. „Elza kann Ada begegnen“ ist durch 6031 als Zweig und durch Kamiya (Guide Book S. 261) gestützt. Neu: Leon sieht Sherry, Leon und John stehen in 604 im selben Raum, Elza trifft Marvin (#058). Die Reihenfolge ist nicht einseitig, die alte Aussage „Elza zuerst“ gilt nur für die Fabrik.

**Alt:** - **Unsere Dialoge in 30C1, 6030, 6031** sind Fan-Text der Mod und damit nicht Original. Der Port liest sie über die RDTs aus `shared_assets`.

**Neu:** - **Unsere Dialoge in 30C1, 6030, 6031** sind Fan-Text der Mod und damit nicht Original. Der Port liest sie über die RDTs aus `shared_assets`. Die Zeile 「……レオン」 steht weder in der Mod noch auf der Disc.

### B-20 Nachprüfen

**Alt:** - Skripte: `zensus.py`, `figuren.py`, `quer.py`, `spuren.py`

**Neu:** - Skripte: `zensus.py`, `figuren.py`, `quer.py`, `spuren.py`. `quer.py` liest lange AOT falsch (`pc+14` statt `pc+22`) und muss das korrigieren. Erreichbarkeit: `k12_subs.py`, `k13_fix.py`, nach `werkzeuge/` übernehmen.

Ergänzen unter „Quellen außerhalb der Disc“:

```
- [Kamiya-Tweets, Wiedergabe](http://blog.esuteru.com/archives/9266710.html) und twitter.com/PG_kamiya/status/1094291939426357248
- [Official Guide Book, Scans](https://residentevil.fandom.com/wiki/Famitsu_interview_with_Hideki_Kamiya)
- [Enigmatism415, Biohazard 1.5 Final Build](https://www.youtube.com/watch?v=qnWhK82FzdU), Herkunft Video A, Übersetzungen
- [Hidden Palace, Nov 6 1996 Prototype](https://hiddenpalace.org/Biohazard_2_(Nov_6,_1996_prototype))
- [GamesRadar 2018](https://www.gamesradar.com/how-the-never-released-resident-evil-15-survived-22-years-to-appear-in-resident-evil-2-remake/)
- `info/Resident_Evil_und_Playstation_Information/information74.txt` (TCRF-Kopie)
```

## 8. Korrekturliste TEXTE_40.md

Die Datei wird aus `werkzeuge/texte_bauen.py` gebaut. T-04 und T-05 gehören deshalb in `jp.py`, die übrigen in die Sprecher- und Übersetzungstabelle.

### T-01 Kopf, Zeile 4

**Alt:** Spalte „Auslöser“: welches Skript die Zeile wirklich aufruft (`Message_on` oder Untersuchen-AOT). Xxx0 ist Leon, Xxx1 ist Elza. Beide Dateien eines Raums tragen oft denselben Textblock, das sagt nichts über den Spielablauf.

**Neu:** Spalte „Auslöser“: welche Datei die Zeile aufruft (`Message_on` oder Untersuchen-AOT). Xxx0 ist Leon, Xxx1 ist Elza. Das heißt nur, dass ein Aufruf im Skript steht. „tot“ heißt: Der Aufruf liegt in einem Sub ohne Aufrufer oder hinter einer Bedingung, die auf der Disc nie wahr wird. Die Zeile läuft dann nicht. Beide Dateien eines Raums tragen immer denselben Text (alle 25 Paare, nur 1021 und 2070 sind Einzeldateien), das sagt nichts über den Spielablauf.

### T-02 Kopf, Zeile 5

**Alt:** Leerstellen oder falsche Zeichen im Japanischen sind echt: Dort fehlte der Glyph im Font der Disc (`00`-Bytes oder Ersatzzeichen wie 東, 度). In der Übersetzung sind sie aus dem Zusammenhang ergänzt.

**Neu:** Leerstellen im Japanischen sind echt: Dort fehlte der Glyph im Font der Disc (`00`-Bytes). Echt sind auch Ersatzzeichen, also vorhandene Zeichen für fehlende: 東 für viele Kanji (46-mal, nur in 307 und 30E), 度 für 場, 西 für 命, 社 für 近, 根 für 伝, 女 für 顔, 階 für 運, 怖 für 姿. Warum das so ist, ist unklar. In der Übersetzung sind sie aus dem Zusammenhang ergänzt, solche Zeilen tragen den Vermerk [Lesart].

### T-03 Kopf, Zeile 6

**Alt:** Räume ohne Eintrag haben keinen Text; siehe BERICHT.md Abschnitt 8.

**Neu:** Räume ohne Eintrag haben keinen Text.

### T-04 Satzzeichen, in allen Zeilen

| Code | Alt | Neu | Zeilen |
|---|---|---|---|
| 0x17 | `;` | 、 | 13 |
| 0x18 | `,` | 。 | 6 |
| 0x3C | `.` | ・ | 23 |
| 09:1A, 09:1B | ゛ ゜ | “ ” | 8 |

### T-05 jp.py

| Code | Alt | Neu | Wirkung |
|---|---|---|---|
| 0A:C9 | 閉 | 聞 | 300 #8, 501 #27 |
| 0A:86 | 探 | 接 | 403 #2 |
| 09:29 | 竜 | 意 | 401 #14 |
| 0A:01 | 薬 | 楽 | keine |
| 0A:84 | 戸 | 声 | keine |
| 0A:8E | 増 | 槽 | keine |
| 09:34 | 界 | 兜 | keine |
| 0A:DF | 溶 | 熔, wahrscheinlich | keine |
| 0A:F3 | 溜 | 溶, wahrscheinlich | keine |
| 09:27 | 運 | vermutlich 遺 | keine |
| 0B:1E | 途 | unklar, Sichtbefund 念 | keine |

### T-06 Einzelzeilen

| Zeile | Spalte | Alt | Neu |
|---|---|---|---|
| 109 #4 | Deutsch | Gerettet… danke, ich stehe in deiner Schuld. [Kanji-Lücke] | Gerettet… Danke, mein Dank. [Lesart, Lücke 礼] |
| 11B #10–21 | Auslöser | Leon | Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) |
| 11B #19 | Sprecher | Marvin | Wohl Marvin, keine Regie |
| 11C #7–9 | Auslöser | Leon | Leon, tot (wie 11B #10) |
| 207 #0 | Auslöser | Leon | Leon, Raum ohne Tür zu ihm, nur per JUMP |
| 300 #1 | Auslöser | Leon + Elza | tot (`ROOM3000 @0x161A`, Spielfigur 13 kommt nicht vor) |
| 300 #8 | Japanisch | 閉く事がやまほどありそうだな... | 聞く事がやまほどありそうだな・・・ |
| 306 #9 | Sprecher | Leon | Erzähltext, Leons Sicht |
| 306 #9 | Deutsch | Irgendwann wurde das Schloss entfernt… | Irgendwann ist das Schloss aufgemacht worden… |
| 307 #2 | Deutsch | In die Hölle… vielleicht wäre es besser, sich hier von denen fressen zu lassen… | Vermutlich: In die Hölle… vielleicht wäre es besser, sich hier von denen fressen zu lassen… [Lesart, Disc 地仲さ] |
| 307 #4 | Sprecher | Marvin | Marvin oder Ada, keine Regie |
| 307 #4 | Deutsch | Schnell, sie kommen! | Schnell, sie kommen! [Lesart 奴等] |
| 307 #5 | Deutsch | Stimmt. Es ist bestimmt irgendwo hier…! | Leise! Es ist bestimmt irgendwo hier…! [Lesart 静かにね] |
| 308 #0 | Sprecher | Marvin oder Ada | Marvin oder Leon |
| 308 #1 | Sprecher | Leon oder Ada | Eher Ada, nicht bewiesen |
| 309 #8 | Sprecher | Sherry (spielbar) | Sherry-Abschnitt, auf der Disc bleibt die Figur Elza |
| 30E #3 | Deutsch | …Ä-äh, Marvin ist in der Halle. Die Bestien draußen haben ihn am Bein erwischt, er kann nicht laufen… …Ada, …was hast du die ganze Zeit gemacht!? Was ist in diesem Gebäude passiert? Und die toten Polizisten draußen!? | …Ä-äh, Marvin ist in der Halle. Die Bestien draußen haben ihn wohl am Bein erwischt, er kann nicht laufen… …Ada, …was hast du die ganze Zeit gemacht!? Was ist in dieser Fabrik passiert? Und die toten Polizisten draußen!? [Lesart] |
| 30E #7 | Deutsch | Diese Wunde… haben die dich erwischt? …ziemlich tief… | Diese Wunde… haben die dich erwischt? …ziemlich schlimm… [Lesart, 相当深い oder 相当酷い] |
| 30E #10 | Deutsch | Die von oben strömen zusammen! Bis hier draußen am Stadtrand… Oben ist es vorbei… | Es sammeln sich mehr, als ich erwartet hatte! Bis hier draußen am Stadtrand… Oben ist es vorbei… [Lesart 予想以上の数が集まって] |
| 30E #12 | Deutsch | Elza, hier bist du! Schlecht… die Bestien haben uns umstellt! …Verdammt, nirgends ein Ausweg… Elza, wie sieht es bei dir aus… gab es hinten im Raum was Leckeres zu essen? | Elza, hier bist du! Schlecht… die Bestien haben uns umstellt! …Verdammt, nirgends ein Versteck… Elza, wie sieht es bei dir aus… gab es hinten im Lager was Leckeres zu essen? [Lesart 隠れられそう, 倉庫?] |
| 400 #5–8 | Auslöser | Leon | Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) |
| 401 #14 | Japanisch | ちゅう竜しなくては | ちゅう意しなくては |
| 401 #18 | Sprecher | Leon | Spielfigur, Raumtext in 401 und 501 |
| 401 #19–22 | Auslöser | Leon | Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) |
| 403 #2 | Japanisch | ヒューズはすべて探続されている | ヒューズはすべて接続されている |
| 403 #5–8 | Auslöser | Leon + Elza | Elza. In 4030 tot (`@0x52C`) |
| 501 #0–5 | Auslöser | Elza | Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) |
| 501 #17–37 | Auslöser | Elza | Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) |
| 501 #19 | Deutsch | Schnell! Sie fiebert schon lange. | Schnell! Sie stöhnt schon lange im Schlaf. |
| 501 #27 | Japanisch | ジョン アナウンスは閉いた？ | ジョン アナウンスは聞いた？ |
| 501 #43 | Sprecher | Elza | Spielfigur, Raumtext in 401 und 501 |
| 506 #0 | Deutsch | Ein Spind mit Schutzanzügen. Offenbar kaputt, er geht nicht auf. | Ein Spind mit Arbeitsanzügen. Offenbar kaputt, er geht nicht auf. [Lesart 作業用] |
| 506 #7 | Auslöser | nicht ausgelöst | Leon + Elza (lange AOT `@0x2B92`) |
| 506 #9 | Auslöser | nicht ausgelöst | Leon + Elza (lange AOT `@0x2BC2`) |
| 514 #0 | Sprecher | Frauenstimme, nicht zugeordnet | Frauenstimme, vom Ton her nicht Ada, Sherry denkbar, offen |

**Nicht ändern:** 306 #0 bleibt „Leon + Elza“. Text 0 liegt in 3060 auf Slot 2 (`@0xF2E`).

### T-07 Vermerk [Lesart]

An diese 30 Zeilen zusätzlich anhängen. Die Lesarten stehen in `pruefung/texte.md` Abschnitt 2.5.

- 10B #0
- 11B #1, #18
- 300 #4, #7, #16
- 307 #0, #7, #8, #12, #13, #14, #15
- 309 #9, #11, #13, #17, #19, #21, #22
- 30E #5, #8, #19, #21, #22, #23
- 403 #4
- 506 #2, #5, #8

Dazu kommen acht Zeilen aus T-06 (307 #2, #4, #5, 30E #3, #7, #10, #12, 506 #0). Mit den fünf schon markierten Zeilen (109 #4, 10B #2, #3, 11B #12, #17) ergibt das die 43 betroffenen Zeilen.

## 9. Bleibt offen

1. Sprecher von 「……レオン」.
2. Ob die Spuren eine bewusste Zeitlinie sind oder aufgeräumte Räume.
3. Bedeutung von (3,98), (3,134), (4,22), (4,23).
4. Pose und Anlass von Marvin in 6041.
5. Der Mann in #118.
6. Name der zweiten Frau im Kittel. „Annette“ bleibt Schluss aus RE2, Mod, TCRF und Foren.
7. Inhalt von 600–602 und 605–607, das Finale.
8. Hörtext der Complete-Disc-Filme.
9. Rolle der Taschenuhr. Das Foto #037 mit 「大きな時計ね」 vor Sherry und Item 63 in ROOM115U könnten zusammengehören, belegt ist das nicht.

## 10. Belegdateien

Alle unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/kritik/`:

- `k1_texte.py` Textaufrufe, lange AOT
- `k12_subs.py`, `k13_fix.py` nie gestartete Subs, Fixpunkt
- `k2_bank.py`, `kd.py` Bank-Tabelle, Disassembler
- `k3_109.py`, `k3b_109.py`, `k3_109.jpg`, `k3b_109.jpg` Transporter
- `k4_118.py`, `k4b_hosen.py`, `k4_118.jpg` Fabrikhof
- `k5_kisten.py`, `k5_kisten.jpg`, `roof.jpg` Dach
- `k6_dach.py`, `k6_z1.png`, `k6_z2.png` Dachzeile
- `k7_tueren.py` Türgraph
- `k8_flags.py` Flag-Querverweis
- `k9_still.py`, `sub_105.jpg` Lupe
- `k10_055.py` Zeile 309 #8 in #054, #055, Ergebnis unter dem Nullmodell
- `k11_mod.py` Mod-Texte
- `k14_paare.py` Textblöcke je Paar
- `k15_ton.py`, `k15_ton.jpg` Ton
- `k16_rest.py` Stichproben
- `cd/c1.jpg`–`c4.jpg` Complete-Disc-Filme
- `leonclip.jpg` Stichprobe Video B
