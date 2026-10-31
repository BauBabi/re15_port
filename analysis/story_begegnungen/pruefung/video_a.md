# Dossier video_a: Video A Bild für Bild gegen BERICHT.md

Geprüft: `Version_80%/Biohazard 1.5 Final Build.mp4` (Video A), gegen `analysis/story_begegnungen/BERICHT.md` und `TEXTE_40.md`, Disc `Version_40%/`, Hintergründe Disc und Mod.
Arbeitsordner mit allen Skripten und Lupenbildern: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/va`. Bildverweise wie `l/vgl028.jpg` beziehen sich auf diesen Ordner.

## 1. Ergebnis in Kürze

1. **Video A ist eine Diashow aus genau 174 Standbildern**, jedes 24 Videobilder lang (0,8008 s). Es gibt keine kürzeren Einstellungen und keine Bewegung. Alle 174 sind gesehen und unten gelistet. Bei 147 steht der Ort fest, 27 bleiben offen.
2. **Falsch im Bericht:** Die Zeile „…Leon“ ist doch im Video zu sehen. Standbild #016 (0:12.81–0:13.61) zeigt Elza von hinten am Sofa des Chefbüros (115 Cut 7) mit dem Untertitel **…レオン**. Es ist die einzige Nennung des anderen Hauptcharakters in Video A. Der Sprecher ist nicht zu klären.
3. **Der Mann bei 0:22 ist nicht Leon.** Das zweite Standbild #028 zeigt Schutzweste, kurze Ärmel und nackte Unterarme. Leon trägt in jedem Video-Bild lange Ärmel. Der Bericht erklärt die Ähnlichkeit mit Leons Kunst, das stimmt für die Disc, nicht für dieses Bild.
4. **Kein Bild zeigt Leon und Elza zusammen.** Kein Bild zeigt Leon mit Sherry, Elza mit Ada oder sicher Leon mit John. Offen bleibt ein einziges Bild: #118, der Mann neben Leon im Fabrikhof.
5. **Falsch im Bericht:** „A 1:33 Leon und Marvin im Fabrikhof“. Das Bild bei 1:33 ist der Hinterhof 306. Der Fabrikhof ist #118 bei 1:34.5, und dort ist der Mann nicht zuzuordnen. Die Aussage „bestätigt: John in 3000 ist Platzhalter“ hängt damit in der Luft.
6. **Bestätigt:** Elza und Marvin bei 0:46.8 (#058). Ort bei 1:57: Raum 603, jetzt auch rechnerisch. Die Frau im Kittel ist nicht Ada, die blonde Frau trägt dieselbe Kleidung.
7. **Neu:** Sherry allein in sechs Standbildern (#050–#055), sieben Zeilen nur im Video, die bisher im Bericht fehlen, und die Zeile in #118, die von der Disc abweicht.

## 2. Aufbau des Videos

| Messung | Wert | Beleg |
|---|---|---|
| Bilder | 4176 bei 29,97 Bilder/s, 139,34 s, 640x480 | `gruppen.py`, ffprobe |
| Schnitte | 173, Abstand immer exakt 24 Bilder | `schnitte.py`, `raster.py`, `cuts.json` |
| Differenz am Schnitt | Mittel 61,3 Graustufen, sonst höchstens 0,11 | `raster.py`, Phasen 0–23 |
| Innerhalb einer Gruppe | Bild 1–22 gegen Bild 23, Blur 5x5: höchstens 25 Graustufen, 0 Pixel über 40 | `gruppen.py` |
| 5 Bilder/s, wie gefordert | 697 Bilder, jedes trifft sein Standbild, alle 174 getroffen, Restdifferenz höchstens 1,41 | `f5check.py`, Bogen `bogen/f5_*.jpg` |
| Ton | Musikschleife, Periode etwa 16–17 s, keine Sprache erkennbar, nirgends still | `ton.py`, `l/ton.jpg` |
| Quellen | #000–#172 Wasserzeichen BIOHAZARD 1.5 / WWW.BIOFLAMES.COM, #173 SURVIVHOR | Sicht |
| Reihenfolge | Elza #001–#081, Leon #082–#172, #173 wieder Elza | Sicht |

Folgen:
- Zeitangaben wie „0:46.8“ meinen nur ein Intervall von 0,8 s. In der Containerzeit liegt alles 0,033 s später.
- Nachbarn in der Liste sind keine Szene. Die Diashow springt zwischen Räumen.
- Der Ton taugt nicht als Beleg für einen Sprecher.
- Das Video kommt von einem Stand mit Debug-Overlay: Debug-Menü mit Spielzeit in #031 (0:15:47), #148 (0:09:01), #159 (0:06:37), dazu „Door: x.y“ in mehreren Bildern. Nach 9 Minuten Spielzeit im Zug heißt: Der Tester ist dorthin gesprungen. Bildfolgen sagen also nichts über den Story-Ablauf.

## 3. Methode

- **Raum:** jedes Standbild gegen 531 Disc- und 1119 Mod-Hintergründe. Erstens SIFT mit RANSAC (`raumabgleich.py`, Kürzel D und M, Zahl = Inlier, ab 6 gewertet). Zweitens ECC-Ausrichtung mit NCC plus Kanten-NCC gegen alle Schnitte einer Stage (`ecc2.py`, `ecc3.py`, Kürzel E, ab 1,0 sicher, darunter schwach). Der Rest per Sichtvergleich, Kürzel S.
- **Mod nur für den Ort.** 354 von 512 gemeinsamen Hintergründen sind bildgleich, 158 weichen ab (`ppm`-Vergleich).
- **Text:** Lupe, dann Glyphenkamm aus dem Disc-Font `DATA/TEX.TIM` mit Raster aus `werkzeuge/jp.py` (`ocr.py`, `ocr2.py`, `ocr3.py`, `ocr4.py`). Jede Lesung wird gegen Zufallswörter gleicher Länge gemessen. Korrelation bis etwa 0,50 ist Rauschen, die Nullmodelle stehen in Abschnitt 5.
- **Disc-Suche:** eigene Dekodierung aller Textblöcke, dazu rohe Bytesuche nach レオン (`d0 ab d4`) und エルザ (`aa cf da`) in allen Dateien (`textsuche.py`, `textsuche.txt`).
- **Figuren:** Lupe mit Gamma (`lupe.py`, `gamma.py`), Modellblätter mit dem Renderer des Mitprüfers (`render/`).

## 4. Die fünf Prüfaufträge

### (1) Kein Bild zeigt Leon und Elza zusammen: bestätigt

Alle 174 gesehen. Leon und Elza stehen in getrennten Blöcken. Mehrfigurige Elza-Bilder: #012, #018, #027, #032, #033, #036, #037, #044, #049, #058, #059. Dort steht neben ihr John, Sherry, Marvin oder der Mann aus (5). Mehrfigurige Leon-Bilder: #085, #103–#105, #115, #116, #118, #119, #127, #128, #142, #145–#147, #152, #154, #165, #169–#172. In keinem davon zeigt eine Figur Elzas rot-weißen Anzug. Die übrigen roten Bilder in Leons Block sind rote Gorillas (#125, #126), Feuer (#134, #135) und ein brennender Zombie (#157). Die Verdachtsstelle 0:22 fällt weg, siehe (5).

### (2) Keine Szene zeigt Leon mit John oder Sherry, Elza mit Ada: bestätigt, ein Vorbehalt

- **Elza mit Ada:** In Elzas Block gibt es keine Frau im Kittel. Die helle Figur in #018 ist John (weißes Hemd, Gürtel, Lupe `l/j1.jpg`), die blasse Gestalt in #007 hat einen hellen Kopf und helle Arme und wirkt wie ein Zombie.
- **Leon mit Sherry:** kein Bild.
- **Leon mit John, #085:** breiter Mann mit hellem Hemd an der Tür. Sein Kopf ist dunkel rotbraun. John hat in #012 und #059 ein helles Gesicht. Also wohl Marvin, passend zur Zeile in #084 und zur Lesart von Project Umbrella (Leon trifft Marvin im 2. Stock).
- **Leon mit John, #118, der Vorbehalt:** siehe Abschnitt 6, Zeile A 1:33. Hemd und Arme sind in dieser Kamera überstrahlt, Hemdfarbe und Hose trennen nicht. Für Marvin spricht nur eine nicht geclippte braune Stelle rechts neben dem Mann (R157 G120 B128). Johns Arm in derselben Kamera clippt neutral (R175 G171 B173, `haut.py`). Ob das ein Arm oder ein Gegenstand ist, ist nicht zu sehen.

### (3) 0:46.8 ist wirklich Marvin: bestätigt als Sichtbefund

#058, 0:46.45–0:47.25, Raum 30C Cut 9 (E0,98, Zweitbester 0,61), Tresen links, nicht auf der Disc. Elza und der Mann stehen im selben Bild, beide im Vollbild klar erkennbar (`l/c1.jpg`). Merkmale: dunkle Haut, hellblaues Kurzarmhemd mit Ärmelabzeichen, dunkle Hose. Hemdfarbe R125 G172 B193 gegen Marvin in #172 R140 G177 B195 (`hemd.py`, `haut.py`). Derselbe Mann steht in #103–#105, #116, #169–#172 neben Leon, in #105 fällt der Name マーヴィン. Das Modell m40 der Mod zeigt dieselbe Kleidung. Im Bild selbst steht kein Name und kein Text.

### (4) Die Frauen bei 1:56–1:58 und 2:02–2:04, Ort 603: Ort bestätigt, Frau nicht Ada, Name offen

- **Ort:** #146 gegen Disc 603 Cut 1: 7 von 7 Inliern, E1,12, Zweitbester 0,63. #147 gegen Disc 603 Cut 0: 7 Inlier (Mod 9), E1,07, Zweitbester 0,52. Auch im Sichtvergleich gleich (`l/bg60b.jpg`, `l/z3.jpg`). Die EXE nennt Raum 603 `UNTENSHITSU` (`@0x8007186C`), also 運転室, Führerstand.
- **Kittel-Frau, #145–#147:** helles Haar, weißer Kittel, schwarzes Oberteil, lange Hose (Lupe `l/w4.jpg`).
- **Blonde, #154, wohl auch #152:** blondes Haar, weißer Kittel, lange Hose. Dieselbe Kleidung, also sehr wahrscheinlich dieselbe Figur. Der Bericht führt sie getrennt.
- **Ada, zum Vergleich #104, #127, #169, #172:** dunkles Kurzhaar, kurzer Rock, nackte Beine, in #104 rote Schuhe. Das passt nicht.
- **Modell:** m4D der Mod (helles Haar, Kittel, schwarzes Oberteil, lange Hose) passt, Disc-0x4D nicht, das ist Elzas Kunst.
- **Name:** Video A nennt keinen. „Annette“ bleibt Schluss aus RE2 und Forumsquellen.
- **Bahnsteig:** #152, #153, #154 sind Raum 600 Cut 3, 4, 1 (M8, M15, E1,13). Der Raum fehlt auf der Disc. #153 zeigt Leon allein.

### (5) 0:22, der Mann neben Elza: Leon ist widerlegt, Roy bleibt Schluss

Die Szene besteht aus genau zwei Standbildern: #027 (0:21.62) mit Elza kniend, #028 (0:22.42) mit dem Mann allein aus anderer Kamera. Beide ohne Text, blaue Ecke ohne Raum.

| Merkmal | Mann in #027/#028 | Leon in Video A | Disc 0x47 (= Leons Kunst) | Mod 0x47 Elza-Archiv (m47E) |
|---|---|---|---|---|
| Brust | dunkle Weste, zwei helle Träger, rundes Abzeichen | R.P.D. | R.P.D. | Weste, zwei Träger, rundes Abzeichen |
| Ärmel | kurz, helle Kante am Oberarm | lang, blau-lila (#095, #117, #140) | lang | kurz |
| Unterarm | hautfarben, beide Arme | kein Hautton | bedeckt | nackt |
| Schuhe | dunkel, spitz | hell in #085, #117, #137 (unklar #139) | hell | dunkel |

Vergleich nebeneinander: `l/vgl028.jpg`.

**Für Leon spricht nur noch** das Haar. **Gegen Leon** sprechen Weste, Ärmel und Unterarme. Das reicht, um Leons Modell auszuschließen.

**Für Roy spricht:** Typ 0x47 steht in `ROOM1021 @0x1D20` neben John. Die Mod setzt in 1021 dieselben Typen (`@0x1DF6`, `@0x1E0A`). Das Bild davor, #026, ist 102 Cut 11, und genau diese Kamera setzt das Disc-Ereignis `sub02 @0x1D7A`. #030 ist wieder Raum 102. m47E trägt außerdem einen Blutfleck am Oberschenkel, passend zu einem Verletzten.

**Grenzen:** Der Name Roy steht in keinem Bild. Die Nachbarschaft in der Diashow ist schwach. Die Datei m47E liegt in der Mod, das Video belegt die Kleidung, nicht die Herkunft der Datei. Der Mann könnte auch ein anderer Polizist mit diesem Modell sein.

**Weitere Bilder:** keine. In Video A trifft die blaue Ecke nur #027 und #028 (Histogramm 0,00, nächstes 0,25; `suche_roy.py`). In Video B keine Stelle: Histogrammabstand mindestens 0,317. Der Blauanteil erreicht höchstens 0,297, und zwar nur bei den blaugefilterten Trailer-Räumen 9:36–10:01, Selbsttest #028 = 0,686 (`blau_b.py`, `l/b4.jpg`).

## 5. Alle Texte im Bild

Korrelation = beste Zeilenkorrelation, Null = Maximum der Zufallswörter gleicher Länge.

| Bild | Zeit | Lesung | Messung | Disc | Status |
|---|---|---|---|---|---|
| #012 | 0:09.61 | エルザ！屋上に行っちゃダメだ！ / [ヤ]バイ怪物がいやがる | 屋上.. 0,80 (地上 0,78, 階上 0,78; Null 0,53). 上 und に Rang 1/596, 屋 Rang 4, 地 nicht in Top 6. Z2 0,65 (Null 0,51) | 屋上, いやが: 0 Treffer | **nur Video**. „地上“ bei Project Umbrella ist Fehllesung |
| #016 | 0:12.81 | …レオン | …レオン 0,82, ……レオン 0,77, ………レオン 0,73 (Null 0,51). Punkte über etwa drei Zellen, Anzahl unsicher. Vollsuche: レ Rang 1/82 Katakana, ン Rang 1/82, オ Rang 4/82, bestes aller ?オ?-Paare, レモン 0,70, レイン 0,68 | nicht in 115 | **nur Video** |
| #032 | 0:25.63 | [まだ..]内に女の子がいるの / 戻らなきゃ | Z1 0,80 (Null 0,50), Z2 0,74 (Null 0,62) | 10B #0 inhaltlich nah, anderer Wortlaut; 戻らな: 0 Treffer | **nur Video** |
| #037 | 0:29.63 | …まあ 大きな時計ね！ | 0,81 (Null 0,51). 時計 gegen 時間, 手紙 nicht trennbar (0,80), Lesung stützt sich auf Project Umbrella | 時計: 0 Treffer | **nur Video** |
| #048 | 0:38.44 | [使]われてないはずだ | ない 0,76, ねぇ 0,70 (Null 0,52) | 300 #12 …使われてねぇはずだ | **Wortlaut anders**. BEFUND „keine Entsprechung“ ist damit erledigt |
| #049 | 0:39.24 | …エルザ / 私 ヒミツの入り口を知ってるの | エルザ 0,86 | 306 #3 | gleich |
| #051 | 0:40.84 | 電げんがある / 電げ[..] | Sicht, Rest hinter Wasserzeichen | 電げん: 0 Treffer | **nur Video** |
| #059 | 0:47.25 | はあ、はあ…間一ぱつってヤツだな！ / [..]ツにくいつかれてた所だぜ | Sicht, Z2 am Wasserzeichen nicht messbar, ケツ/サツ/ヤツ nicht trennbar | 309 #9 | gleich, das Video füllt die Disc-Lücken ぱつ und く |
| #065 | 0:52.05 | Now Running… (7×), Break, [シス]テムエラー | Sicht | システムエラー: 0 Treffer | **nur Video** |
| #075 | 1:00.06 | エレベーター[?] | 0,80 (Null 0,56), Abschlusszeichen unlesbar | 307 #15 Z3 „エレベーターは…“ vereinbar | unklar, wohl Zeile im Aufbau |
| #083 | 1:06.47 | そうだ！[..]にこの[..]報告しないと | そうだ！ 0,89 (Null 0,63), die zwei Kanji danach nicht trennbar | 報告: 0 Treffer. 署 fehlt im Disc-Font | **nur Video** |
| #084 | 1:07.27 | みんなゾンビに[..] / それとも、ゾンビになっているかだよ。 | Z1 0,54, Rest überstrahlt, Z2 Sicht | ゾンビ: 0 Treffer | **nur Video** |
| #105 | 1:24.08 | マーヴィン[..] | マーヴィン 0,72 (Null 0,47). Folgezeichen Rang über 200, weder の noch 00 belegt | nicht gefunden | **nur Video**, Rest unlesbar |
| #118 | 1:34.49 | みんな ラクー[..] / [unlesbar] | みんなラクー 0,83, みんなアンブ 0,52, みんなゾンビ 0,62 (Null 0,55) | 300 #3 lautet みんな アンブレラ事けんの… | **anderer Text als Disc** |
| #127 | 1:41.70 | 会わなかったのか？ | Sicht klar | 11C #3 | gleich |
| #140 | 1:52.11 | ホール[..] | ホールは 0,71, の 0,70, に 0,68 (Null 0,48), ホ selbst nur Rang 66 | 30E #8 …ホールはまずい！ vereinbar, ganze Zeile aber nur 0,53 | unklar. Ort ist 307 Cut 9, nicht das Dach wie bei Project Umbrella |
| #169 | 2:15.34 | 上のさわぎは何だ？ | 0,75 gegen 上の騒ぎはなんだ？ 0,56 (Null 0,49), einzeilig | 11B #10 レオン！無事だったか / あの上の騒ぎはなんだ？ | **Wortlaut anders** |
| #170 | 2:16.14 | 地下に逃げるってのはどうだ？ | 0,66 gegen そこから地下に… 0,23 (Null 0,37), einzeilig | 11B #16 zweizeilig mit そこから | **Wortlaut anders** |
| #172 | 2:17.74 | 下に何かいるかも知れないしー / こんな言　があるだろ？ | Z2 mit Leerzelle 0,71, ohne 0,57 (Null 0,53). 言 Rang 2/596 | 11B #17 völlig anders | **nur Video**. Die Leerzelle passt zu fehlendem Glyph, 葉 ist nicht im Font, möglich こんな言葉があるだろ？ („es gibt doch so einen Spruch“). Nicht bewiesen |

Weitere Bildtexte: Debug-Menü #031 UTILITY MENU, #148 `…YOKUSHITSU` (`DOURYOKUSHITSU @0x80071886`), #159 `…CORRIDOR 2-A` (`@0x8007138C` oder `@0x800714F8`), Inventar #078 und #093.

**Vollständigkeit:** Sichtung aller 174 unteren Bildstreifen (`str/s*.jpg`), dazu ein Blockfinder (`textfinder.py`, 27 Treffer). Die Treffer ohne bekannten Text (#008, #035, #043, #076, #078, #090, #098, #111, #116, #154, #159, #161) sind nachgesehen: keine weitere Zeile.

**Andere Hauptfigur genannt:** レオン bei Elza genau einmal (#016). エルザ in Leons Stills: nie. Auf der Disc gibt es laut Spalte „Auslöser“ von TEXTE_40.md keine Elza-Zeile mit レオン und nur eine Zeile mit エルザ in einem Leon-Lauf, 403 #8, die §8 des Berichts als toten Code führt. Die rohe Bytesuche fand die Muster nur in RDT-Textblöcken und einmal in `STAGE6/ROOM603.BSS`. Das ist ein 3-Byte-Muster mitten in komprimierten Bilddaten, kein Textblock.

## 6. Befunde je Abschnitt des Berichts

| Abschnitt | Aussage | Urteil | Begründung |
|---|---|---|---|
| §11 | „…Leon“ ist „in unseren Videos nicht zu sehen“ | **falsch** | #016, Abschnitt 5. Der Satz ist zu streichen |
| §11 | „die einzige Stelle, an der Elza Leon kennt“ | **ungenau** | Der Sprecher fehlt im Bild. Drei Lesarten: Elza spricht (nur Project Umbrella), der Chef spricht, oder Kopier-Rest. Auf der Disc sind Skript und Text von 1150 und 1151 gleich, eine Zeile an Leon könnte also auch im Elza-Lauf erscheinen. Das Bild beweist nur, dass der Name im Elza-Lauf fällt |
| §1, §13 | „keine Zeile nennt den jeweils anderen“ | **ungenau** | Gilt für die Disc. Für den 80%-Stand ergänzen |
| §5 | Chefbüro: „Derselbe Satz“ | **ungenau** | Gilt für die Disc. Disc-Ereignis `sub03`, Kamera Cut 7 `@0xE2E`, dann `Message_on 0 @0xE66`. Dieselbe Kamera zeigt im Video bei Elza eine andere Zeile. #089 zeigt Leon dort ohne Text |
| §3 | „1150 und 1151 byte-gleich“ | **ungenau** | Dateien verschieden: 140216 und 179092 B, md5 `1c9b…` und `da5d…`. Gleich sind Skript, Text und die meisten Abschnitte (`d115b.py`). 1151 trägt ein fünftes Objekt: TIM `@0x23974`, MD1 `@0x71EC`, Metallplatte „ASH tex D6 7931 Q55“, ohne Skriptbezug. §11 sagt korrekt „Skripte“ |
| §8 Zeile 0:22 | „Typ 0x47 trägt … dieselbe R.P.D.-Kunst wie Leon“ erklärt das Bild, „Das Bild allein beweist es nicht“ | **ungenau** | Abschnitt 4 (5). Das Bild schließt Leon aus |
| §8, §10 | „Text und Video 1:33 zeigen Marvin“, „A 1:33 Leon und Marvin im Fabrikhof“, „Bestätigt: John in 3000 ist Platzhalter“ | **falsch** | #116 (1:32.89–1:33.69) ist 306 Cut 4, Disc 14 Inlier, orange Tür, Zaun. Der Fabrikhof ist #118 (1:34.49–1:35.30), 300 Cut 0, Disc 16 Inlier, Kamera wie #044. Der Mann dort ist nicht zuzuordnen |
| §10 Schluss | „Kein Video zeigt … John oder Sherry neben Leon“ | **bestätigt, Vorbehalt** | Für Video A, Vorbehalt #118. Stimmt die Disc-Figur 0x49 in 3000 auch im 80%-Stand, wäre #118 Johns Modell |
| §10 Schluss | „das Ende ist in beiden Videos nicht zu sehen“ | **ungenau** | Die Räume sind zu sehen: 603 (#146, #147), 604 (#148, #151), Bahnsteig. Das Finale nicht. In 604 steht Leon allein, ohne Marvin, John, Sherry, mit Debug-Menü |
| §8 | „Der Raumvergleich bei 1:57 ist Sichtbefund, die Rechnung schlug … nicht an“ | **überholt** | Abschnitt 4 (4). Die Rechnung schlägt an |
| §10, §5 | Frau im Kittel und blonde Frau getrennt, 2:02–2:04 | **ungenau** | Dieselbe Kleidung. Bilder: #152 ab 2:01.72, #154 bis 2:04.12. #153 zeigt nur Leon |
| §10 | A 1:55–1:56 als eine Zeile | **ungenau** | Zwei Räume: #143 Bahnsteig (1:54.51), #145 Raum 509 Cut 11 (1:56.12), dazwischen #144 Leon in 509 Cut 1 |
| §10 | A 2:16–2:18: „#10, #16 gleich, #17 … umformuliert“ | **ungenau** | Beginn 2:15.34. Alle drei weichen ab, #172 ist eine neue Aussage |
| §10 | A 1:24 „マーヴィンの… 2“ | **ungenau** | Nur マーヴィン ist belegt |
| §10 | A 0:39 „306 #3–6“ | **ungenau** | Video A zeigt nur #3, #4–6 stehen in Video B |
| §10 | A 0:10, 0:47.4, 1:06.7, 1:07.6, 1:42 | **bestätigt** | #012, #059, #083, #084, #127 |
| §8 | Marvin hellblaues Hemd im 80%-Stand | **bestätigt** | #058, #103–#105, #116, #169–#172 |
| §9 | 30C1 „Video zeigt die Szene ohne lesbaren Text“ | **bestätigt** | #058 hat gar keinen Text |
| §12 | „Video A jede Sekunde … Sehr kurze Einstellungen können durchgerutscht sein“ | **falsch** | 174 Standbilder, Abschnitt 2. Eine Abtastung mit 1 Bild/s hätte rund jedes fünfte Standbild übersprungen. Die Diashow steht schon in `analysis/video_80prozent/BEFUND.md` §1.3 |
| §6 | 604 „Waggon mit Sitzbänken“ | **ungenau** | Bänke gibt es (Cut 3–5). Die EXE nennt 604–607 aber `DOURYOKUSHITSU` `@0x80071886`, 動力室, Maschinenraum |
| §7 | Sherry spielbar | **gestützt, neu** | #050–#055: Sherry allein, 309 Cut 6, 10, 8, 7, 5 und 30C Cut 10, in #051 an der Kiste mit Text. Standbilder beweisen keine Steuerung. Das Video fehlt in §7 und §10 |
| §9 | Vergleich Disc 604 gegen Mod | **neu** | Disc 604 Cut 0–6 ist pixelgleich Mod 603 Cut 6–12 (7 von 7, Differenz 0,0). Gleiche Raumnummer heißt nicht gleicher Raum |
| §2 | Mod ist kein Capcom-Beleg | **Ergänzung** | Bleibt richtig für Text. Beim Modell zeigt #028 aber Kleidung, die der Mod-Datei m47E gleicht, und m40, m4D passen ebenso. Einzelne Mod-Modelle bilden also den späteren Capcom-Stand ab |

Weitere Nebenbefunde:
- #012 spielt am Konferenztisch, Raum 114 (Tisch, Tasse, Doppeltür; #018 E0,83 auf 114 Cut 0). `BEFUND.md` G6 nennt „Halle“.
- #139 zeigt Leon im selben Raum 30C wie Elza und Marvin (#058) und Sherry (#053). Der Raum liegt also auf beiden Wegen.
- #036/#037 (Elza und Sherry) und #104/#105 (Leon, Ada, Marvin) nutzen dieselbe Kamera 200 Cut 11.

## 7. Folgen für die Zielfrage

- **Treffen sich Leon und Elza?** In Video A nein, keine Ausnahme mehr.
- **Treffen sie die Begleiter des anderen?** Elza und Marvin: ja, einziges Bild #058. Leon und John: nicht belegt, #118 offen. Leon und Sherry, Elza und Ada: kein Bild.
- **Erfährt man vom anderen nur über Dialoge?** Der Name Leon fällt im Elza-Lauf einmal. Das ist der erste Bildbeleg für eine Nennung. Ob Elza ihn kennt, folgt daraus nicht.

## 8. Grenzen

- Video A ist vom Fernseher abgefilmt, überstrahlt und klein. Figuren sind Sichtbefund, auch mit Messwerten.
- Die Glyphenmessung nutzt den Font der Disc. Der 80%-Stand kann Zeichen ergänzt haben (署 ist im Video wahrscheinlich, im Disc-Font fehlt es).
- Ein Standbild kann eine Zeile im Aufbau zeigen. Das betrifft #075, #105, #118, #140.
- 27 Standbilder bleiben ohne sicheren Ort, keines davon mit einer strittigen Figurenkombination außer #084 und #085.
- 165 Standbilder wurden im Bogen (510 Pixel breit) gesehen, die strittigen in der Lupe. Eine winzige Figur am Bildrand kann so entgangen sein.

## 9. Lückenlose Schnittliste

Beleg: D = Inlier gegen Disc, M = Inlier gegen Mod, E = ECC, S = Sicht. Raumnummern der Mod sind nur Ortsangaben.

| # | von bis | Ort | Beleg | Figuren | Text im Bild |
|---|---|---|---|---|---|
| 000 | 0:00.00–0:00.80 | Titelbild BIOHAZARD 2 | S | - | Logo, Menü unleserlich |
| 001 | 0:00.80–0:01.60 | 102 Büro, Cut 1 | D49 M45 | Elza |  |
| 002 | 0:01.60–0:02.40 | 104 Gang, Cut 5 | D6 E1.34 | Elza |  |
| 003 | 0:02.40–0:03.20 | 104, Cut 6 | E1.29 | Elza, Zombie |  |
| 004 | 0:03.20–0:04.00 | 104, Cut 6 | E1.29 | Elza, Zombie |  |
| 005 | 0:04.00–0:04.80 | 104, Cut 6 (schwach) | E0.92 | Elza, Blutwolke |  |
| 006 | 0:04.80–0:05.61 | 110, Cut 0 | E1.35 | Elza, 2 Gegner in blauen Hemden |  |
| 007 | 0:05.61–0:06.41 | offen, Tür mit Fenster, ähnlich #085 | - | Elza, blasse Gestalt |  |
| 008 | 0:06.41–0:07.21 | offen, Stage 1, Wegweiser-Schild | - | Elza | Door-Einblendung |
| 009 | 0:07.21–0:08.01 | offen, rote Tür | - | Elza |  |
| 010 | 0:08.01–0:08.81 | 110, Cut 1 | E1.27 | Elza, Leichen |  |
| 011 | 0:08.81–0:09.61 | offen, Aushangwand | - | Elza, liegender Gegner |  |
| 012 | 0:09.61–0:10.41 | 114 Konferenzraum (Tisch, Doppeltür; nicht auf Disc) | S | Elza, John | エルザ！屋上に行っちゃダメだ！ / [ヤ]バイ怪物がいやがる |
| 013 | 0:10.41–0:11.21 | offen, wohl 114 | - | dunkelrotes Riesenwesen, Elza klein |  |
| 014 | 0:11.21–0:12.01 | 119 Schießstand, Cut 5 | D21 M16 | - |  |
| 015 | 0:12.01–0:12.81 | 117 Dach, Cut 2 | D8 M8 | Elza klein |  |
| 016 | 0:12.81–0:13.61 | 115 Chefbüro, Cut 7 (Kamera wie #089) | E0.81 | Elza von hinten, Liegender auf dem Sofa | …レオン |
| 017 | 0:13.61–0:14.41 | 115, Cut 1 | D14 M15 | Elza |  |
| 018 | 0:14.41–0:15.22 | 114, Cut 0 (schwach, plus Sicht) | E0.83 | Elza, John, rotes Wesen als Haufen |  |
| 019 | 0:15.22–0:16.02 | 119, Cut 2 | D21 M18 | Elza |  |
| 020 | 0:16.02–0:16.82 | 10A, Cut 2 | E0.98 | Elza | Door-Einblendung |
| 021 | 0:16.82–0:17.62 | 11B Garage, Cut 0/7/13 (bildgleich) | E1.50 | Elza |  |
| 022 | 0:17.62–0:18.42 | 11A, Cut 0 | D11 M7 | Elza, dunkle Gestalt vorn rechts |  |
| 023 | 0:18.42–0:19.22 | 123, Cut 6 | E1.03 | Elza |  |
| 024 | 0:19.22–0:20.02 | offen, Ziegelgang | - | Elza überstrahlt |  |
| 025 | 0:20.02–0:20.82 | offen, Ziegelwand | - | Elza überstrahlt, 2 rote Hunde |  |
| 026 | 0:20.82–0:21.62 | 102, Cut 11 (Kamera des Roy-Ereignisses @0x1D7A) | D7 M7 | Elza |  |
| 027 | 0:21.62–0:22.42 | blaue Ecke, kein Hintergrund | S | Elza kniet, Mann sitzt (nicht Leon) |  |
| 028 | 0:22.42–0:23.22 | blaue Ecke, andere Kamera | S | Mann allein (Weste, nackte Unterarme) |  |
| 029 | 0:23.22–0:24.02 | offen | - | Elza überstrahlt |  |
| 030 | 0:24.02–0:24.82 | 102, Cut 0 | D36 M29 | Elza, Blutlache |  |
| 031 | 0:24.82–0:25.63 | 103, Cut 6, plus Debug-Menü | M7 | - | UTILITY MENU, Zeit 0:15:47 |
| 032 | 0:25.63–0:26.43 | 10B Zufahrt, Cut 7 | E1.25 | John, Elza | [..]内に女の子がいるの / 戻らなきゃ |
| 033 | 0:26.43–0:27.23 | 10B, Cut 8 | D11 M7 | John (Rücken), Elza klein am Leiterfuß |  |
| 034 | 0:27.23–0:28.03 | 10B, Cut 1 | E1.45 | Elza, Lieferwagen RACCOON POLICE |  |
| 035 | 0:28.03–0:28.83 | 109, Cut 8 | M6 | Elza | Door-Einblendung |
| 036 | 0:28.83–0:29.63 | 200 Kanal, Cut 11 | E1.17 | Elza, Sherry |  |
| 037 | 0:29.63–0:30.43 | 200, Cut 11 | D9 M7 | Elza kniet, Sherry | …まあ 大きな時計ね！ |
| 038 | 0:30.43–0:31.23 | offen, Stage 2 oder 20B | - | Elza |  |
| 039 | 0:31.23–0:32.03 | 200, Cut 13 | D29 M25 | Elza fern, dunkle Großfigur vorn |  |
| 040 | 0:32.03–0:32.83 | 200, Cut 13 | D28 M29 | Elza fern, dunkle Großfigur vorn |  |
| 041 | 0:32.83–0:33.63 | 11B, Cut 5 | D7 E1.47 | Elza |  |
| 042 | 0:33.63–0:34.43 | offen | - | Elza |  |
| 043 | 0:34.43–0:35.24 | 207 Cut 1/10 oder 20B Cut 12 (bildnah) | E1.11 E1.12 | Elza |  |
| 044 | 0:35.24–0:36.04 | 300 Fabrikhof, Cut 0 | D16 M13 | Elza am Boden, John |  |
| 045 | 0:36.04–0:36.84 | offen, Gang mit Blutspur | - | Elza |  |
| 046 | 0:36.84–0:37.64 | 20A, Cut 15 | E0.92 | kleine Figur an Leiter |  |
| 047 | 0:37.64–0:38.44 | offen, Laufsteg | - | Elza |  |
| 048 | 0:38.44–0:39.24 | 300, Cut 1 | E0.94 | Lkw, Leichen | [使]われてないはずだ |
| 049 | 0:39.24–0:40.04 | 306 Hinterhof, Cut 5 | D14 M7 | John, Sherry, Elza | …エルザ / 私 ヒミツの入り口を知ってるの |
| 050 | 0:40.04–0:40.84 | 309, Cut 6 | E1.33 | Sherry allein |  |
| 051 | 0:40.84–0:41.64 | 309, Cut 10 | D11 M12 | Sherry allein, Kiste | 電げんがある / 電げ[..] |
| 052 | 0:41.64–0:42.44 | 309, Cut 8 | D12 M9 | Sherry allein |  |
| 053 | 0:42.44–0:43.24 | 30C, Cut 10 (nicht auf Disc) | M33 | Sherry allein |  |
| 054 | 0:43.24–0:44.04 | 309, Cut 7 | D26 M18 | Sherry allein |  |
| 055 | 0:44.04–0:44.84 | 309, Cut 5 | D17 M17 | Sherry allein |  |
| 056 | 0:44.84–0:45.65 | 30E, Cut 9 | M13 | Elza |  |
| 057 | 0:45.65–0:46.45 | 303, Cut 0 | D8 M8 | Elza überstrahlt |  |
| 058 | 0:46.45–0:47.25 | 30C, Cut 9, Tresen (nicht auf Disc) | E0.98 | Elza, Marvin |  |
| 059 | 0:47.25–0:48.05 | 309, Cut 15 | D6 E0.78 | Elza, John, Sherry | はあ、はあ…間一ぱつってヤツだな！ / [..]ツにくいつかれてた所だぜ |
| 060 | 0:48.05–0:48.85 | 305, Cut 0 | M14 | Elza |  |
| 061 | 0:48.85–0:49.65 | 305, Cut 15 | E0.99 | Rechner nah |  |
| 062 | 0:49.65–0:50.45 | 304, Cut 4 | E1.01 | Elza, Zombies |  |
| 063 | 0:50.45–0:51.25 | 304, Cut 2 | E1.11 | Elza |  |
| 064 | 0:51.25–0:52.05 | 304, Cut 0 | E1.14 | Elza |  |
| 065 | 0:52.05–0:52.85 | 305, Cut 10, Vollbild-Rechner | M12 | - | Now Running… (7×), Break, [シス]テムエラー |
| 066 | 0:52.85–0:53.65 | 304, Cut 6 | M25 | Elza, Gore |  |
| 067 | 0:53.65–0:54.45 | 30A, Cut 0 | M8 | Elza |  |
| 068 | 0:54.45–0:55.26 | 301, Cut 8 | D34 M26 | Figur am Rand |  |
| 069 | 0:55.26–0:56.06 | 301, Cut 3 | D24 M21 | Elza, Zombie |  |
| 070 | 0:56.06–0:56.86 | 302/30D, Cut 5 (bildgleich) | E0.97 | Elza |  |
| 071 | 0:56.86–0:57.66 | 302/30D, Cut 1 | M6 E1.19 | Elza, Streifenwagen |  |
| 072 | 0:57.66–0:58.46 | 30A, Cut 4/5 | E0.87 M9 E0.77 | Elza |  |
| 073 | 0:58.46–0:59.26 | 307, Cut 8 | D6 M8 | Birkin im Tor |  |
| 074 | 0:59.26–1:00.06 | 307, Cut 8 | D8 M8 | Birkin im Tor |  |
| 075 | 1:00.06–1:00.86 | 307, Cut 9 | D13 M14 | Elza an der Kontrollbox | エレベーター[?] |
| 076 | 1:00.86–1:01.66 | 307, Cut 0 | D17 M18 | Elza am Boden |  |
| 077 | 1:01.66–1:02.46 | 307, Cut 10 | D8 M6 | Birkin nah |  |
| 078 | 1:02.46–1:03.26 | Inventar | S | Porträt | Door-Einblendung |
| 079 | 1:03.26–1:04.06 | 307, Cut 12 | D25 M24 | Birkin, Elza |  |
| 080 | 1:04.06–1:04.86 | 307, Cut 1 | D9 M7 | Figur, Blitz |  |
| 081 | 1:04.86–1:05.67 | 307, Cut 12 | D20 M15 | Birkin, Elza |  |
| 082 | 1:05.67–1:06.47 | 117 Dach, Cut 4 | E1.03 | Leon |  |
| 083 | 1:06.47–1:07.27 | 117, Cut 5 (schwach) | E0.75 | Leon | そうだ！[..]にこの[..]報告しないと |
| 084 | 1:07.27–1:08.07 | offen | - | hockender Mann mit Ärmelabzeichen | みんなゾンビに[..] / それとも、ゾンビになっているかだよ。 |
| 085 | 1:08.07–1:08.87 | offen, Tür mit Fenster | - | Leon, breiter Mann mit dunklem Kopf (wohl Marvin) |  |
| 086 | 1:08.87–1:09.67 | offen | - | Figur von hinten |  |
| 087 | 1:09.67–1:10.47 | 115, Cut 5 | E1.41 | Leon |  |
| 088 | 1:10.47–1:11.27 | 110, Cut 1 | E1.27 | Leon |  |
| 089 | 1:11.27–1:12.07 | 115, Cut 7 | E0.91 | Leon von hinten (Stern), Liegender |  |
| 090 | 1:12.07–1:12.87 | 119, Cut 4 | D9 M6 | Figur am Rand |  |
| 091 | 1:12.87–1:13.67 | 102, Cut 0 | D18 M13 | Leon |  |
| 092 | 1:13.67–1:14.47 | 107, Cut 0 | D14 M7 | Leon |  |
| 093 | 1:14.47–1:15.28 | Inventar | S | Karte Leon S. Kennedy |  |
| 094 | 1:15.28–1:16.08 | 11B, Cut 0/13 | D9 E1.51 | Leon |  |
| 095 | 1:16.08–1:16.88 | 104, Cut 0 | D6 M7 | Leon |  |
| 096 | 1:16.88–1:17.68 | 109, Cut 0/8 | E1.35 | Leon |  |
| 097 | 1:17.68–1:18.48 | 121, Cut 4 | D7 E1.05 | Leon, Arme am Gitter |  |
| 098 | 1:18.48–1:19.28 | 122, Cut 0 | D7 M9 | Leon, Leiche | Door-Einblendung |
| 099 | 1:19.28–1:20.08 | offen, blauer Gang | - | Leon |  |
| 100 | 1:20.08–1:20.88 | 109, Cut 2/10 | E1.41 E1.40 | Leon |  |
| 101 | 1:20.88–1:21.68 | offen, blauer Gang | - | Leon |  |
| 102 | 1:21.68–1:22.48 | offen | - | Leon, Kiste |  |
| 103 | 1:22.48–1:23.28 | 200, Cut 15 | D6 M7 | Ada, Marvin, Leon |  |
| 104 | 1:23.28–1:24.08 | 200, Cut 11 | D7 M7 | Ada, Marvin, Leon |  |
| 105 | 1:24.08–1:24.88 | 200, Cut 11 | D8 M7 | Ada, Marvin, Leon | マーヴィン[..] |
| 106 | 1:24.88–1:25.69 | offen, roter Raum | - | Leon, stacheliger Körper |  |
| 107 | 1:25.69–1:26.49 | 10B, Cut 5 | D12 M8 | Leon |  |
| 108 | 1:26.49–1:27.29 | offen, Leiter wie #046 | - | weiße Spritzer |  |
| 109 | 1:27.29–1:28.09 | offen | - | Leon am Boden |  |
| 110 | 1:28.09–1:28.89 | offen | - | Leon, Zahnbogen vorn |  |
| 111 | 1:28.89–1:29.69 | offen | - | Leon, Riesenkörper vorn |  |
| 112 | 1:29.69–1:30.49 | offen | - | Leon, Stachelkörper vorn |  |
| 113 | 1:30.49–1:31.29 | 304, Cut 6 | M26 | Leon, Gore |  |
| 114 | 1:31.29–1:32.09 | 304, Cut 4 | M14 | Leon (Stern, RPD), Gore-Wesen |  |
| 115 | 1:32.09–1:32.89 | 302/30D, Cut 4 | E1.15 | Leon, zweite Figur |  |
| 116 | 1:32.89–1:33.69 | 306 Hinterhof, Cut 4 | D14 M10 | Leon, Marvin |  |
| 117 | 1:33.69–1:34.49 | 307, Cut 9 | D18 M21 | Leon |  |
| 118 | 1:34.49–1:35.30 | 300 Fabrikhof, Cut 0 | D16 M19 | Leon, hockender Mann (nicht zuzuordnen) | みんな ラクー[..] / [unlesbar] |
| 119 | 1:35.30–1:36.10 | 11C Parkgarage, Cut 3 | D12 M8 | Ada, Leon |  |
| 120 | 1:36.10–1:36.90 | 11C, Cut 3 | D19 M12 | - |  |
| 121 | 1:36.90–1:37.70 | 11C, Cut 11 | D14 M13 | Leon |  |
| 122 | 1:37.70–1:38.50 | 11C, Cut 8 | D34 M22 | Leon |  |
| 123 | 1:38.50–1:39.30 | 11C, Cut 7 | D15 M12 | Leon kniet |  |
| 124 | 1:39.30–1:40.10 | 11C, Cut 6, Umbrella-Lkw | D22 M16 | - |  |
| 125 | 1:40.10–1:40.90 | 11C, Cut 4 | D8 M8 | rote Gorillas |  |
| 126 | 1:40.90–1:41.70 | 11C, Cut 12 | S | roter Gorilla |  |
| 127 | 1:41.70–1:42.50 | 11C, Cut 13 | S | Leon, Ada | 会わなかったのか？ |
| 128 | 1:42.50–1:43.30 | 11C, Cut 13 | S | Leon, Ada |  |
| 129 | 1:43.30–1:44.10 | 11C, Cut 0 | D28 M21 | Leon |  |
| 130 | 1:44.10–1:44.90 | 120 Leichenhalle, Cut 1 | E1.16 | Leon, Leiche |  |
| 131 | 1:44.90–1:45.71 | 120, Cut 2 | D6 M7 | Figur, Leiche |  |
| 132 | 1:45.71–1:46.51 | 120, Cut 1 | E1.06 | Leon, Leiche |  |
| 133 | 1:46.51–1:47.31 | 120, Cut 3 | D20 M17 | Leon, Leiche |  |
| 134 | 1:47.31–1:48.11 | 120, Cut 0 | E1.15 | Leon schießt, rotes Feuer |  |
| 135 | 1:48.11–1:48.91 | 11E, Cut 5 | E1.20 | Leon, brennendes Wesen |  |
| 136 | 1:48.91–1:49.71 | 11E, Cut 3 | D13 M11 | Leon |  |
| 137 | 1:49.71–1:50.51 | offen, Schaltschrank | - | Leon |  |
| 138 | 1:50.51–1:51.31 | offen, große Tür | - | Leon |  |
| 139 | 1:51.31–1:52.11 | 30C Cut 3 = 30D Cut 10, Tresen (nicht auf Disc) | E1.28 | Leon |  |
| 140 | 1:52.11–1:52.91 | 307, Cut 9 | D18 M17 | Leon | ホール[..] |
| 141 | 1:52.91–1:53.71 | 307, Cut 5 | D13 M12 E0.99 | Figur, Birkin, Blitz |  |
| 142 | 1:53.71–1:54.51 | 307, Cut 4 | D12 M6 E1.22 | helle Stehfigur, hockende Figur, helle Großfigur |  |
| 143 | 1:54.51–1:55.32 | Bahnsteig, orange Halle, Kamera in keinem Bestand | S | Leon, riesiges dunkles Wesen, roter Zug |  |
| 144 | 1:55.32–1:56.12 | 509, Cut 1 | M24 | Leon |  |
| 145 | 1:56.12–1:56.92 | 509, Cut 11, roter Gang | E0.95 | Frau im Kittel, dunkles Riesenwesen, Feuer |  |
| 146 | 1:56.92–1:57.72 | 603 Führerstand, Cut 1 | D7 E1.12 | Frau im Kittel, Leon |  |
| 147 | 1:57.72–1:58.52 | 603, Cut 0 | D7 M9 E1.07 | Frau im Kittel am Pult, Leon |  |
| 148 | 1:58.52–1:59.32 | Disc 604 Cut 2 (= Mod 603 Cut 8), plus Debug-Menü | E0.99 | - | …YOKUSHITSU, Zeit 0:09:01 |
| 149 | 1:59.32–2:00.12 | 601, Cut 1 (nicht auf Disc) | M30 | Leon |  |
| 150 | 2:00.12–2:00.92 | Halle wie #143 | S | Leon |  |
| 151 | 2:00.92–2:01.72 | Disc 604 Cut 0 (= Mod 603 Cut 6) | D22 M16 E1.36 | Leon | Door: 2.5 |
| 152 | 2:01.72–2:02.52 | 600 Bahnsteig, Cut 3 (nicht auf Disc) | M8 E1.33 | Leon, schlanke Frau im Weiß |  |
| 153 | 2:02.52–2:03.32 | 600, Cut 4 | M15 E1.30 | Leon |  |
| 154 | 2:03.32–2:04.12 | 600, Cut 1 | E1.13 | blonde Frau im Weiß, Leon |  |
| 155 | 2:04.12–2:04.92 | 50B, Cut 2 | M13 E1.17 | helle Figur |  |
| 156 | 2:04.92–2:05.73 | 514, Cut 12 | D14 M15 | Leon |  |
| 157 | 2:05.73–2:06.53 | 514, Cut 4 | D13 M11 | brennender Zombie |  |
| 158 | 2:06.53–2:07.33 | 514, Cut 1 | D8 M8 | Leon |  |
| 159 | 2:07.33–2:08.13 | roter Gang, plus Debug-Menü | S | - | …CORRIDOR 2-A, Zeit 0:06:37, Door |
| 160 | 2:08.13–2:08.93 | 503, Cut 0 | D12 M9 E1.47 | Figur | Door-Einblendung |
| 161 | 2:08.93–2:09.73 | 501, Cut 0 | D14 M9 E1.44 | Leon | Door-Einblendung |
| 162 | 2:09.73–2:10.53 | 50C, Cut 3 | E1.17 | Figur |  |
| 163 | 2:10.53–2:11.33 | 50C, Cut 0 | D12 M10 | Leon |  |
| 164 | 2:11.33–2:12.13 | 400, Cut 0 | D9 E1.23 | Figur fern |  |
| 165 | 2:12.13–2:12.93 | offen | - | zwei blaue Figuren, Figur im Kittel |  |
| 166 | 2:12.93–2:13.73 | 308, Cut 0 | E1.10 | kleine Figur, helles Objekt |  |
| 167 | 2:13.73–2:14.53 | 308, Cut 3 (schwach) | E0.81 | blasse Gestalten |  |
| 168 | 2:14.53–2:15.34 | 509, Cut 4/9/14 | E1.52 E1.47 | kleine Figur an Konsole |  |
| 169 | 2:15.34–2:16.14 | 11B, Cut 1 (Disc) = Cut 14 (Mod) | D18 M13 | Marvin, Leon, Ada | 上のさわぎは何だ？ |
| 170 | 2:16.14–2:16.94 | 11B, Cut 5 | D9 | Leon, Marvin, Ada | 地下に逃げるってのはどうだ？ |
| 171 | 2:16.94–2:17.74 | 11B, Cut 6 | S | Leon, Marvin |  |
| 172 | 2:17.74–2:18.54 | 11B, Cut 6 | S | Leon, Marvin, Ada | 下に何かいるかも知れないしー / こんな言　があるだろ？ |
| 173 | 2:18.54–2:19.34 | 109, Cut 2/10, Wasserzeichen SURVIVHOR | E0.85 E0.84 | Elza |  |
