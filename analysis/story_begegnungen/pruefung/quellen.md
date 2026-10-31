# Prüfung Fremdquellen (Agent „quellen“)

Stand 2026-10-31. Geprüft: `analysis/story_begegnungen/BERICHT.md`, `TEXTE_40.md`, dazu alle erreichbaren Fremdquellen. Alle Zeilennummern „PU L…“ beziehen sich auf die Textfassung der Project-Umbrella-Seite in `quellen_web/pu/page.txt`. Standbilder heißen `sNNN` und liegen in `q_frames/stills/` (Liste am Ende).

## 0. Ergebnis in Kürze

1. **Video A ist kein Film.** Es ist die YouTube-Diashow „Biohazard 1.5 Final Build“ von Enigmatism415 (9.8.2013): 173 Bioflames-Fotos plus ein SurvivHor-Foto, je 0,80 s. Gemessen: 174 Standbilder, null Bewegung.
2. **Die Zeile 「……レオン」 steht in Video A**, Still s016, 0:12,81–0:13,61. BERICHT §11 sagt das Gegenteil.
3. **Wer sie sagt, ist unklar.** Project Umbrella schreibt „ELZA“, der Übersetzer Enigmatism415 schreibt „Chief Irons“. Im Spieltext steht kein Sprecher.
4. **„Andere Fassung“ ist falsch.** Project Umbrellas Abschnitt „Bioflames Version & PlayStation Museum Version“ sind genau unsere zwei Quellen: Video A plus unsere 40%-Disc. Alle 48 Spielzeilen der Seite lassen sich zuordnen.
5. **Stärkste neue Quelle:** Kamiya selbst, Scan S. 261 des Official Guide Book: 「ひとり1本」 und 「女性主人公のシナリオにエイダがチラッと登場するシーンがあった」. Das bestätigt Elza–Ada von Capcom-Seite.
6. **Direkte Antwort Kamiyas** auf die Fanfrage nach Leon und Elza (9.2.2019): 「1.5の時はパラレルではなかったけどザッピングは無かった…ただの二本立て…」.
7. **Das Zug-Zitat im Bericht ist abgeschnitten.** Es geht weiter mit „Except Ada. So, maybe Ada is still a spy?“ und stützt sich auf einen Spieldurchlauf des geleakten Builds, nicht auf Capcom.
8. **Eine ausdrückliche Capcom-Aussage „sie begegnen sich nie“ habe ich nicht gefunden.** Sie steht nur in Sekundärquellen. Das Fandom-Wiki beruft sich für „did not cross“ auf Resident Evil Archives S. 218–220, diese Seiten konnte ich nicht einsehen. Das Urteil „nicht belegt“ des Berichts bleibt richtig.

## 1. Die drei Hauptkorrekturen

### 1.1 Was Video A wirklich ist

**Messung** (`schnitt.py`, `stills.py`, Datei `Version_80%/Biohazard 1.5 Final Build.mp4`, 139,42 s):

| Größe | Video A | Video B, 2:40–3:10 zum Vergleich |
|---|---|---|
| Schnitte | 173, also 174 Einstellungen | 12 |
| Einstellungslänge | Median 0,80 s, Minimum 0,80 s, Maximum 0,80 s | 0,70–7,13 s |
| Bilddifferenz innerhalb einer Einstellung | Median 0,000 | Median 0,254 |

Jede Einstellung ist also ein einzelnes Foto gleicher Dauer.

**Herkunft, YouTube-Metadaten** von https://www.youtube.com/watch?v=qnWhK82FzdU (Volltext: `quellen_web/yt/beschreibung.txt`):
- Titel „Biohazard 1.5 Final Build“, Autor Enigmatism415, Länge 139 s, Upload 2013-08-09.
- „a collection of all 173 Bioflames-exclusive images, which depict the final '80%' build“
- „I appended a 174th image of the same build (from Survivhor)“. Unser letztes Still s173 trägt tatsächlich den Schriftzug SURVIVHOR.
- „The music was extracted from the 40% 'PSM' build“. Der Ton von Video A gehört also nicht zu den Bildern.
- „these images were photographed at the turn of the millennium with a cellular camera in Japan“
- Dank an „Kim (Inflames)“ und „Kegluneq (Leuchest)“.

Eine Kopie derselben 139 s liegt unter https://www.youtube.com/watch?v=pDebLo5zFpo (PRESS PLAY, 2023).

**Folgen für den Bericht:**
- §2 „Filmaufnahmen“ stimmt für Video A nicht.
- Reihenfolge und Nachbarschaft der Bilder stammen vom Uploader, nicht vom Spielablauf. Die Beschreibung selbst nennt für die Uhr-Zeile „2:19“, obwohl sie heute bei 0:30 steht, die Sammlung wurde also umsortiert oder der Zeitstempel ist falsch. Aus „A 0:13 folgt auf A 0:12“ folgt nichts.
- §12 „Video A jede Sekunde“ tastet zu grob ab. Bei 0,8 s je Bild fallen rund 35 von 174 Bildern durch. Alle 174 liegen jetzt einzeln vor.
- **Drei Stills zeigen das Debug-Menü** (s031 0:25, s148 1:58, s159 2:07; lesbar u. a. „UTILITY MENU“, „JUMP“, „MEMORY PLAYER“, „XA PLAY TEST“, Zeitzähler 0:15:47 / 0:9:1 / 0:6:37). Der Fotograf sprang also per Debug durch die Räume. Einzelne Bilder belegen deshalb keinen Spielablauf, und Zustände können künstlich sein. Das Bild bei 0:22 zeigt zudem statt eines Raumhintergrunds eine blaue Fläche.
- „Elza und Marvin am Tresen“ bleibt ein echtes Foto (s058), ist aber kein Beleg für den Ablauf.

Video B ist etwas anderes: echte Spielaufnahmen gemischter Herkunft, mit den Wasserzeichen WWW.BIOFLAMES.COM und WWW.SURVIVHOR.COM, dazu der Trailer 「あの惨劇から…」 (laut Fandom-Wiki Hyper PlayStation Re-mix). Die Herkunft des Gesamtvideos konnte ich nicht klären. Es ist nicht das YouTube-Video „Resident Evil 1.5 all footage with elza walker“ (Nithg666, 2010, 598 s gegen unsere 859 s).

### 1.2 Die Zeile 「……レオン」

**Fundstelle auf der Seite:** PU L819–825, Abschnitt POLICE STATION, unter der Überschrift „BIOFLAMES VERSION & PLAYSTATION MUSEUM VERSION“:
> In the Chief's Office, Leon finds Brian injured on the couch. BRIAN 「私なら大丈夫だ」 … 「今は自分の安全をかんがえろ」 … In Elza's scenario, the Japanese subtitles read, ELZA 「・・・レオン」Leon...

**Fundstelle im Bild:** s016, Bereich 0:12,81–0:13,61. Elza steht von hinten im Bild, neben ihr liegt ein Mann auf der Couch. Unten steht eine Punktreihe, dann 「レオン」. Zeitlich gemittelt über 24 Einzelbilder eindeutig lesbar: `q_frames/a_0013_sub_avg.png`. Enigmatism415 listet sie bei 0:12.

**Gegenstück:** s089 (1:11,67) zeigt dieselbe Kamera mit **Leon** an derselben Stelle, ohne Untertitel; s087 (1:10) zeigt ihn von weitem an der Couch. Die Szene gibt es also in beiden Szenarien, in Video A nur bei Elza mit dieser Zeile.

**Sprecher: unklar.** Drei Lesarten:

| Lesart | Quelle | Stütze | Schwäche |
|---|---|---|---|
| Elza sagt es | PU L824 „ELZA“ | Etikett der Seite | Kein Beleg im Spiel. PU-Etiketten sind Zuschreibungen des Kompilators. Die Seite hat nachweisbare Lesefehler und vertauscht Zeilen (Abschnitt 2.3) |
| Der Chef sagt es | Enigmatism415: 「アイアンズ署長：「………レオン」」, „Chief Irons: .........Leon“. Fandom-Plot: „She also meets Chief of Police Brian Irons, who later dies soon after hoping that his officers are safe.“ | Der Verletzte ist die einzige weitere Figur im Bild; der Satz passt zu einem Sterbenden | Ebenfalls Zuschreibung, der Fandom-Satz hat keinen Einzelbeleg |
| Kopierrest aus Leons Szene | eigene Überlegung aus der Disc | Auf der 40%-Disc sind die Skripte 1150/1151 byte-gleich und der Text identisch, Elzas Szene war dort eine Kopie von Leons | Für den 80%-Stand nicht belegt, reine Hypothese |

**Was sich sagen lässt:** Im Spätstand erscheint Leons Name im Elza-Szenario auf dem Bildschirm. Ob Elza ihn kennt, folgt daraus nicht. Am ehesten spricht der Chef; bewiesen ist das nicht.

**Zustand:** Video B zeigt bei 2:54–2:57 Elza im selben Büro, und die Couch ist **leer** (`q_frames/b_sofa.jpg`). Es gibt also mindestens zwei Zustände. Warum, ist unklar. Debug-Sprung, Ablauf vor oder nach der Szene sind alle möglich.

### 1.3 „Andere Fassung“ ist falsch

BERICHT §11: „Szenetexte einer anderen Fassung (‚Bioflames/PlayStation Museum‘)“, §5: „Project Umbrella führt in einer anderen Build-Fassung ‚Linda‘ und ‚Brian‘“.

Belege, dass es dieselben Quellen sind:
- **Bioflames** = Wasserzeichen auf Video A. Die „XX“-Lücken der Seite liegen dort, wo das Wasserzeichen den Text verdeckt, z. B. 「？？？バィ？？？？？？」 und s012.
- **PlayStation Museum** = unsere Disc. Hidden Palace, https://hiddenpalace.org/Biohazard_2_(Nov_6,_1996_prototype): „Build date Nov 6, 1996 13:10:10“, „Ownership Curator / PlayStation Museum (late 2007-2012), IGAS (2012-present)“, Disc-Aufschrift „41 room“. Unser Exe-Stempel: `Nov  6 1996-13:09:19`.
- Forum Assembler, mattjon, 10.5.2008: „The PlayStation Museum copy is an earlier beta than the one on the complete disk and the exclusive images that Inflames has.“ (https://assemblergames.org/viewtopic.php?t=21916)
- Enigmatism415 nennt den 40%-Stand „40% 'PSM' build“.
- PU L996 erwähnt den „Curator“ und „his version of 1.5“.
- **Alle 48 Spielzeilen der Seite** stehen in Video A oder im Disc-Text (Tabelle unten), einschließlich der echten Glyphenlücke 「ウィルスにかかわる械 だろう」 aus Raum 506 #8.

„Linda“ und „Brian“ stehen nur im englischen Fließtext des Kompilators Welsh, in keiner zitierten japanischen Zeile. Sie sind also kein Merkmal einer Fassung. Sugimura sagt sogar, der Chef habe zu Beginn der Entwicklung „Gordon“ geheißen (The PlayStation, Juni 1998). Der Disc-Text nennt nur 署長 und エイダ.

## 2. Die Project-Umbrella-Seite im Detail

https://www.projectumbrella.net/biohazard-15-scenario--description.html, „Compiled by Welsh“. Bilder enthält die Seite keine.

### 2.1 Aufbau
- L788–793 „BIOHAZARD 2: The Story“: Synopsis ohne Quellenangabe. „In the meantime, the main gate of the station is broken through by a girl riding her motorbike.“ Beide Hauptfiguren gleichzeitig am Revier.
- L794–807 „BIOHAZARD COMPLETE DISK“: vier Filmtitel. Capcom-Text.
- L808–920 Bioflames & PlayStation Museum: die Zeilen unten.
- L921ff. Itemliste, Mikami-Zitat zum Spinnenwesen (Quelle: GameFan 1998, bestätigt), Absage des Curators 2009.

### 2.2 Jede Spielzeile, zugeordnet

| PU | Zeile | Gefunden in |
|---|---|---|
| L811–812 | 「おい、冗談だろ？」「・・・クソッなんてこった」 | A laut Enigmatism415 bei 1:05; im Standbild bei mir nicht lesbar |
| L814 | 「ホールは・・・」 | A s140 1:52,5 (Enigmatism415 liest ホールに…) |
| L818 | 「そうだ！部長にこのことを報告しないと」 | A s083 |
| L821–822 | 「私なら大丈夫だ」「今は自分の安全をかんがえろ」 | Disc 115 #0 |
| L825 | 「・・・レオン」 | A s016 |
| L829–830 | 「みんなゾンビにくわれたか」「それとも…」 | A s084 |
| L833–834 | 「ボタソがある」 (Tippfehler für ボタン) | Disc 104 #0, auch B 6:48 |
| L836–837 | 「スイッチがある」 | Disc 119 #0, auch B 0:54 |
| L839 | 「会わなかったのか？」 | A s127, Disc 11C #3 |
| L842 | 「上のさわぎは何だ？」 | A s169 |
| L844 | 「地下に逃げるってのはどうだ？」 | A s170, Disc 11B #16 |
| L847–848 | 「下に何かいるかも知れないし」「こんなX があるだろ？」 | A s172 |
| L851–852 | 「地上に行っちゃダメだ!!」「？？？バィ？？？？？？」 | A s012 |
| L855–856 | 「XX 内に女の子がいるの」「戻らなきゃ」 | A s032 |
| L859 | 「マーヴィン00 2」 | A s105 |
| L861 | 「・・・まあ、大きな時計ね！」 | A s037 |
| L864–873 | Now Running / 「システムエラー」 | A s065 |
| L876–877 | 「マーヴィン! これは...」「ラクーン警さつの警かんじゃないか」 | Disc 300 #2 |
| L878 | 「今じゃXXXわれてないはずだ」 | A s048 |
| L880 | 「みんなXXXX」 | Disc 300 #3; laut Enigmatism415 A 1:34 |
| L884–891 | 「…エルダ」「私、ヒミツの入り口を知ってるの」… | A s049, B 3:21–3:30, Disc 306 #3–6 |
| L892 | 「トビラのところで待ってて！」 | Disc 306 #7 |
| L894 | 「カギがかかっている」 | Disc 306 #0 |
| L896–897 | 「電げんがある」「電げんを入」 | A s051 |
| L900–902, 907–908 | 「ブレーカーが上がつていろ」… | Disc 309 #5, #2 |
| L904–905 | 「間一ぱつってヤツだな！」… | A s059, Disc 309 #9 |
| L910 | 「エレベーターX」 | A s075 |
| L915–916 | 「げん重にロックされている」… | Disc 40A #3 |
| L918–919 | 「ウィルスにかかわる械 だろう」… | Disc 506 #8 |

Prüfskript: `pu_map.py`. Treffer unter 100 % habe ich einzeln von Hand zugeordnet.

### 2.3 Fehler der Seite
Sie zeigen, wie weit man ihren Etiketten trauen kann:
- 「地上に行っちゃダメだ」: Das Bild zeigt 屋上, Enigmatism415 liest ebenfalls 屋上. BERICHT §10 „Dach“ stimmt also.
- 「サツにくいつかれて」 (Polizei): Bild und Disc haben ケツ. Die englische Übersetzung „caught by the police“ ist deshalb falsch.
- 「エルダ」 statt エルザ, 「ボタソ」 statt ボタン, つ/ろ statt っ/る in 「下がつていろ」.
- L878 und L880 sind vertauscht: Die John-Zeile aus Elzas Szenario steht unter Leon, die Marvin-Zeile unter Elza.
- L814 „Leon returns to the rooftop“: s140 zeigt Leon am Schaltpult.
- L810 „a helicopter is flying away“ gegen den von Fandom zitierten Kamiya-Tweet (Absturz, Abschnitt 5.1).

### 2.4 Weitere Kontakte zwischen den Szenarien auf der Seite
- L793: beide gleichzeitig am Revier (Synopsis).
- L797, L806 Capcom-Titel 「警察署にたてこもるレオンとエルザ」, 「研究所に踏み込むレオンとエルザ」.
- L893: „In Leon's scenario, the opposite door is locked. There's also a scene with Marvin who is bitten by a zombie soon after.“ Das widerspricht BERICHT §4 nicht: In `ROOM3060` geht es um eine andere Tür. Slot 1 (Hintertür) führt zu `@0xF9A Message_on 9`, Slot 2 bekommt `@0xF2E Aot_reset` mit Textnummer 0 „カギがかかっている“.
- L895 und L903: „Sherry pushes a crate across the floor, and climbs on top.“ und „She reunites with Elza and John at the entrance.“ Das stützt BERICHT §7.
- L911: „In both scenarios, the player encounters the first form of G here. However, each scene is different.“
- L920: „William was a 'good person' in this version.“

Eine Aussage, dass Leon und Elza sich treffen, steht nirgends auf der Seite.

## 3. Das Zug-Zitat

**Vollständiger Wortlaut** (Suchauszug, Seite selbst nicht abrufbar):
> In the final train, everyone is there, Marvin, John, Sherry and Annette. Except Ada. So, maybe Ada is still a spy? She just knew the lab was going to be burned down.

**Quelle:** The Horror Is Alive, „Resident Evil 1.5 WIP Spoiler Thread“, Archiv Seite 2 (http://www.the-horror.com/forums/archive/index.php/t-9980-p-2.html, neue URL `…/10251-resident-evil-1-5-wip-spoiler-thread-…`). Der Poster erwägt laut Auszug auch, Ada könne im Entwicklungsverlauf gestorben sein. Autor und Datum konnte ich nicht ermitteln.

**Worauf es sich stützt:** Laut Auszügen aus demselben Faden auf Spieldurchläufe des geleakten Builds. Ein Poster schreibt: „it has been said Elza and Leon never cross paths, but in the video I posted, you see Marvin, John and Sherry all in the first train car“, und fragt, ob das Elzas Szene sei. „WIP“ ist Team IGAS' Arbeitsstand (Februar 2013) auf Basis der 40%-Disc. Keine Capcom-Quelle, keine Dialogbasis.

**Annette:** In Raum 604 steht sie auf der Disc nicht. Der Name kommt offenbar aus 603, wo Figur 0x4D steht. Im selben Faden fragt jemand, ob Elzas altes Modell ohne Pferdeschwanz ein Platzhalter für Annette sei; der Auszug zeigt keine Antwort. Das passt genau zu BERICHT §8. Das Forum-„Annette“ ist also eine Deutung des Platzhalters, keine Beobachtung einer Annette.

**„Except Ada“:** Passt zur Disc in dem Sinn, dass Flag (4,22) laut BERICHT §6 nirgends gesetzt wird und 603 deshalb nie Ada zeigt. Der Schluss „Ada ist noch Spionin“ ist Spekulation des Posters.

## 4. Mod-Herkunft „Inflames / Enigmatism415“

**Dokumentiert:**
- `info/Re1.5/README.TXT`: Z. 360 Raum 116 Leon „based on Inflames's screenshot and Enigmatism415's translation“; Z. 363–364 Raum 200 Elza, „reconstructed Sherry's cutscene (based on Inflames's screenshots and Enigmatism415's translation)“; Z. 367 Raum 30C „based on Inflames's screenshot“; Z. 387 „Added Annette and fire effect in room 607 (based on Inflames's screenshot)“; Z. 413 Dank an Enigmatism415.
- Patch-Notizen 25-12-2020, https://pastebin.com/zbEigRMQ, Z. 21: „Enigmatism415 for his Inflames's screenshots translations.“
- THIA, 2012 (Suchauszug): Enigmatism415 möchte alle „Inflames-exclusive images“ übersetzen, braucht aber Bilder ohne Wasserzeichen über den Untertiteln.
- THIA (Suchauszug): Das Video sammelt alle Inflames-Bilder „plus a SurvivHor-exclusive image at the end“ mit Übersetzungen in der Beschreibung. Der Auszug datiert das auf Januar 2018, YouTube auf 2013. Vermutlich wurde es später erneut vorgestellt oder die Beschreibung erweitert; das ist nicht geklärt.
- THIA (Suchauszug): Die Bilder stehen laut Enigmatism415 auf der alten Bioflames-Seite bereits in einem Wayback-Stand vom 6.6.2000. Herkunft laut Forum: ein anonymer Besitzer der 80%-Disc schickte INFLAMES eine Sammlung von Kamerafotos. Das ist Forenwissen aus zweiter Hand.

**Gibt es die Bilder und Übersetzungen noch? Ja.** Video A ist genau diese Sammlung, die Übersetzungen stehen in der YouTube-Beschreibung (`quellen_web/yt/beschreibung.txt`, 2627 Zeichen). Die Originale auf bioflames.com konnte ich nicht prüfen.

**Für BERICHT §9 wichtig:** s058 (Elza und Marvin) hat keinen Untertitel, und README Z. 367 nennt auch keine Übersetzung. Der Dialog in 30C1 hat also keine Capcom-Grundlage. §9 ist bestätigt.

**Wahrscheinliche Zuordnung der Mod-Szenen:** Raum 116 → s084, Raum 200 → s037, Raum 607 → s145. Das ist meine Schlussfolgerung aus Inhalt und README, nicht belegt.

## 5. Aussagen von Capcom-Beteiligten

### 5.1 Treffen und Zusammenhang der Szenarien

**Kamiya, Official Guide Book 1998, S. 261** (Scan geprüft: `quellen_web/scans_guide/p261_q34.jpg`, Kopf S. 257: „本作のディレクターである神谷英樹氏…40の質問にお答えいただいた“). Im Buch ist das Q34, im Fandom-Wiki Q35:
> シナリオは現在のようなザッピングシステムではなく、ひとり1本。女性主人公のシナリオにエイダがチラッと登場するシーンがあったり、警察署でカードキーを渡してくれる警官マービンが、レオンの先輩として最後まで行動をともにするパートナーだったりしました。

Sinngemäß: keine Zapping-Struktur, je Figur ein Szenario. Im Szenario der Protagonistin gab es Szenen, in denen Ada kurz auftaucht. Marvin war Leons älterer Partner bis zum Schluss.

Quelle der Scans: https://residentevil.fandom.com/wiki/Famitsu_interview_with_Hideki_Kamiya (Galerie).

**Kamiya, Twitter @PG_kamiya, 9.2.2019**, Antwort auf einen Fan, der fragt, wie Leon und Elza in 1.5 zueinander standen und ob es parallel wie bei Chris und Jill war:
> 1.5の時はパラレルではなかったけどザッピングは無かった…ただの二本立て…

https://twitter.com/PG_kamiya/status/1094291939426357248, wiedergegeben im Tweet-Einbettungsblock von http://blog.esuteru.com/archives/9266710.html. Die Tweet-ID ergibt 2019-02-09 17:48 UTC. Das Original selbst war nicht abrufbar (x.com: HTTP 402).

Das ist die direkteste Antwort auf die Zielfrage: selbe Wirklichkeit, kein Zapping, zwei getrennte Geschichten. „Parallel“ meint bei Kamiya den Chris/Jill-Fall, sich gegenseitig ausschließende Fassungen (Guide S. 257, Q2: 「実際にはふたりのストーリーはパラレルです」). Über ein Treffen sagt er nichts.

**biohazard archives** (CAPCOM CO., LTD, 31.3.2005, ISBN 4-906582-31-1), Glossar, englische Übersetzung bei Project Umbrella:
> A legendary, canceled version of BIO 2. The protagonists are "Leon" and a woman named "Elza Walker." It featured linear paths and no zapping, with different partner characters from the current version.

https://www.projectumbrella.net/biohazard-archives.html. Offizielles Buch, aber Übersetzung eines Fans.

**Kamiya, Tweet vom 1.3.2014** (nur über Fandom zitiert, Status 439787228265320448, ID ergibt 2014-03-01): Auf die Frage, ob Leon zu Spielbeginn über einen Hubschrauberabsturz flucht „or did he see Elza crash into the station“: „The former.“ Damit ist keine Blickverbindung am Spielstart belegt.

**Mikami, GamePro Feb. 1997:** „RE2 is the story of Leon's and Elza's survival.“ **Mikami, GameFan Sept. 1996:** „Are there more than 2 playable characters?“ – „No, just two.“

### 5.2 Wie das Zapping entstand, was daran neu war

| Quelle | Aussage |
|---|---|
| Kamiya, Guide S. 257 Q2 (Scan) | Die Idee gab es seit der Spätphase von Teil 1, Auslöser war die Teamfrage, ob Chris und Jill auf derselben Zeitlinie liegen |
| Kamiya, The PlayStation Juni 1998 | „When we first started making Resident Evil 2, though, we didn't have a system like that. At a certain point we scrapped everything … and it was then that the Zapping System got added.“ |
| Sugimura, ebenda | Erst nach dem Neustart legte Kamiya sich in einer Besprechung fest. „I suggested that it should be possible to use the data on the memory card to link the stories together“. Er selbst war dagegen |
| Kamiya, Research on Biohazard 2 (1.9.1998) | „The idea of channel surfing was around since the late stages of developing the first game.“ Vorbild „Back to the Future“ |
| Kamiya, Twitter 9.2.2019 | 「ザッピングは先生とのシナリオ会議の最中に生まれたんじゃなかったかな…」, Sugimura warnte davor |
| Project Umbrella (BIOHAZARD 1.5, BIOHAZARD 2) | „conceived by Oishi with inspiration from the movie Back to the Future II“, und zwar **vor** dem Abbruch. Keine Quellenangabe |

**Neu gegenüber 1.5** nach diesen Quellen: Das ist die Verknüpfung über die Memory Card, ein Ereignisablauf aus zwei Blickwinkeln, Haupt- und Nebenroute. 1.5 hatte das laut Capcom nicht. Mikami 1998 zu den Folgen: „I said we should have just made each character route independent.“

**BERICHT §1 „Vorläufer des späteren Zapping-Systems“** ist deshalb eine eigene Deutung. Capcom trennt klar: nicht parallel, aber kein Zapping. Die Weltspuren der Disc sind fest ins Raumskript geschrieben, ein Spielstand verbindet nichts.

### 5.3 Figuren
- **Ada:** Kamiya, The PlayStation 1998: „Ada, on the other hand, was originally a researcher and wore a white lab coat.“ Das betrifft die „Frau im Kittel“ (siehe Abschnitt 6).
- **Marvin:** Kamiya ebenda: „a support character like Ada or Sherry, who would work with you in the final act.“ Research S. 191 (T. Mizuno): „played an active role in various ways and eventually escaped.“ Das stützt 604.
- **Chef:** Kamiya ebenda: „a normal police chief“. Passt zum Verletzten in s016/s089.
- **John:** Der „John“ im Guide, Q33, ist Adas Geliebter aus dem fertigen Spiel, nicht Elzas Begleiter. Der THIA-Faden „Rejected Sub-Plot“ bezieht sich darauf.
- **Spielbare Begleiter:** Project Umbrella nennt Marvin „a playable partner character“ (ohne Beleg). Der THIA-WIP-Faden (Datum nicht gesehen, der WIP-Build erschien im Februar 2013) kennt die Speicherwerte „D for alternate Leon (Marvin placeholder?), E for John, and F for 'SHERLY!'“ (Suchauszug). GameRevolution 2018: „It's not known if Sherry was playable in RE1.5“. Video A zeigt sie: s050–s055 allein in Fabrikräumen, s051 mit 「電げんがある」 an der Kiste. Enigmatism415 listet zudem bei 0:44 「はやくエルザたちを助けなければ…」, im Standbild bei mir nicht lesbar (weißer Boden).

### 5.4 Prozentangaben

„40%“ und „80%“ sind Szene-Etiketten, keine Capcom-Stände:
- Mikami, GameFan 1998: „60%“
- Kamiya, The PlayStation 1998: „about 70% complete“
- Research 1998, S. 191: „80% complete“
- ELEMENTS 2011: „about 70%“
- Disc-Aufschrift der Nov.-1996-Fassung: „41 room“ (Hidden Palace)

## 6. Widersprüche zwischen Quellen

| Punkt | Seite A | Seite B | Stand |
|---|---|---|---|
| Sprecher 「……レオン」 | PU: Elza | Enigmatism415: Chief Irons; Fandom sinngemäß Chef | Unklar, Chef wahrscheinlicher |
| Richtung der Weltspuren | BERICHT §1: alle drei Elza zuerst | GamesRadar (Hurley 2018): „Leon would find a police truck on fire and put it out … Elza would find the extinguished truck later. In return Elza would defeat a mutated William Birkin that Leon would find later“. Fandom-Plot ebenso zu Birkin | Beide Richtungen behauptet; Disc belegt nur Elza-zuerst an drei Stellen |
| Laster | GamesRadar, Fandom: Leon löscht, Elza findet ihn | Disc: 1091 hat kein Ereignis (Skript nur `Work_set`/`Member_set`). THIA: „burning van scene only happens in Leon's scenario“ | Auf der Disc nicht belegbar, Quelle von GamesRadar unbekannt |
| Wer zuerst | THIA: Elza startet früher | THIA: Leon vorn in Revier und Kanal, Elza zuerst im Labor | Fan-Deutungen |
| Zapping-Zeitpunkt | Kamiya/Sugimura: nach dem Neustart | Project Umbrella: schon vorher | Primärquellen gehen vor |
| Zapping-Urheber | Kamiya/Sugimura | Project Umbrella: Oishi | Unklar, keine Primärquelle für Oishi gefunden |
| Ada oder Linda | Fandom, GamesRadar, Wikipedia: Linda, später umbenannt | Disc 6.11.1996 und Kamiya 1998: エイダ | Disc stützt Ada schon im Nov. 1996; Herkunft „Linda“ unklar |
| Frau im Kittel | BERICHT: eigene Figur, Name unbekannt; Mod und Fandom: Annette | Kamiya: Ada trug einen weißen Kittel. PU L840: „black shirt under her lab coat“ | Unklar. Bildvergleich: Ada s127/s172 dunkler Bob; s145–147 Kittel über dunklem Oberteil, Haar heller, aber überbelichtet |
| Hubschrauber | PU: fliegt weg | Fandom mit Kamiya-Tweet: Absturz | Tweet nicht im Original geprüft |
| Datum Enigmatism-Video | YouTube 2013-08-09 | THIA-Auszug Januar 2018 | Ungeklärt |

## 7. Quellenbewertung

| Quelle | URL | Art | Gewicht |
|---|---|---|---|
| Official Guide Book 1998, Q&A Kamiya | Scan über Fandom | **Primär**, Scan geprüft | Sehr hoch |
| Kamiya, Twitter 2019 | twitter.com/PG_kamiya/status/1094291939426357248 | **Primär**, über Blog-Einbettung | Hoch, spät erinnert |
| The PlayStation 6/1998, Kamiya/Sugimura/Mikami | shmuplations.com/residentevil2/, Volltext-Kopie biohaze.com | **Primär**, Fan-Übersetzung | Hoch |
| Research on Biohazard 2, 1.9.1998 | projectumbrella.net/research-on-biohazard-2-final-edition.html | Interviews primär, S. 191 redaktionell | Hoch / mittel |
| biohazard archives 2005 | projectumbrella.net/biohazard-archives.html | **Offiziell**, Fan-Übersetzung | Hoch |
| GamePro 2/1997, GameFan 1996/1998 | residentevil.fandom.com | Primär, Abschrift im Wiki | Mittel, zeitgenössisch |
| Complete-Disc-Titel | PU L794–807 | Capcom-Text, Abschrift | Mittel, Werbung |
| Enigmatism415, YouTube-Beschreibung | youtube.com/watch?v=qnWhK82FzdU | **Fan-Analyse** mit eigener Lesung der Primärbilder | Hoch für Lesung, Sprecher sind Zuschreibung |
| Hidden Palace | hiddenpalace.org/Biohazard_2_(Nov_6,_1996_prototype) | Archiv-Metadaten | Hoch für Herkunft |
| Assembler 2008 | assemblergames.org/viewtopic.php?t=21916 | Forum, zeitnah | Mittel |
| Project Umbrella, Szenario-Seite | projectumbrella.net/biohazard-15-scenario--description.html | **Fan-Transkript** von Video A und Disc, mit Fehlern | Mittel für Text, gering für Sprecher |
| Project Umbrella, BIOHAZARD 1.5 | projectumbrella.net/biohazard-15.html | Fan-Zusammenfassung ohne Belege | Gering bis mittel |
| THIA-Forum | the-horror.com | **Fan-Analyse**, nur als Suchauszug | Gering bis mittel |
| GamesRadar 2018 | gamesradar.com/how-the-never-released-resident-evil-15-survived-… | Sekundär, nennt keine Quelle für die Laster-Stelle | Mittel |
| Wikipedia | en.wikipedia.org/wiki/Resident_Evil_1.5 | **Abgeschrieben**. Der Satz „the actions of one character affected the environment of the other“ steht nicht im zitierten GameRevolution-Artikel (Stichwortsuche ohne Treffer), er klingt nach GamesRadar | Gering |
| Fandom-Wiki | residentevil.fandom.com | Fan, beruft sich für „did not cross“ auf Archives S. 218–220 (nicht geprüft) | Gering bis mittel |
| GamePro.de 2015 (Petersen) | gamepro.de/artikel/… | **Abgeschrieben**, keine Quelle, Satz ist eine Bildunterschrift | Gering |
| GameRevolution 2018, Bloody Disgusting | gamerevolution.com/originals/364269-…, bloody-disgusting.com/editorials/3542480/… | Sekundär | Gering |
| Mod-README | info/Re1.5/README.TXT | Primär nur für die Mod | Keine Story-Aussagekraft |

## 8. Korrekturvorschläge zum Bericht

**§1 (letzter Absatz):** Statt „Das ist ein Vorläufer des späteren Zapping-Systems.“: „Capcom beschreibt 1.5 so: nicht parallel, aber kein Zapping, zwei getrennte Geschichten (Kamiya 2019; Guide 1998). Die drei Weltspuren der Disc passen dazu. Sie sind fest ins Raumskript geschrieben, keine Verknüpfung über Spielstände.“ Dazu: „Die Richtung Elza zuerst gilt für die drei Disc-Stellen. GamesRadar nennt eine Gegenrichtung (Laster), die sich auf der Disc nicht belegen lässt.“

**§1 (erste Antwort):** Ergänzen: „Eine zweite heikle Stelle ist A 0:13 mit der Zeile ‚……レオン‘ (Abschnitt 11). Eine wörtliche Capcom-Aussage zum Nicht-Treffen liegt uns nicht vor; Resident Evil Archives S. 218–220 sind ungeprüft.“

**§2:** Video A: „Diashow aus 173 Bioflames-Fotos und einem SurvivHor-Foto, Handykamera um 2000, zusammengestellt von Enigmatism415 (2013), Musik aus dem 40%-Build.“ Video B: „Spielaufnahmen gemischter Herkunft, Gesamtquelle ungeklärt.“ Disc: „96 RDT-Dateien, 51 Raumnummern, Disc-Aufschrift ‚41 room‘.“

**§3:** Zeile Polizeichef: „Skripte und Text gleich, Dateien nicht (140216 / 179092 Byte, nOmodel 4 / 5). Video: s016 Elza, s089 Leon.“ Zeile Elza–Ada: „Als Zweig angelegt, von Kamiya bestätigt (Guide S. 261).“

**§5 Fußnote:** „Project Umbrella nennt im eigenen Fließtext ‚Linda‘ und ‚Brian‘. In den zitierten Spielzeilen stehen diese Namen nicht.“

**§7:** Ergänzen: „Video A zeigt die Sherry-Stelle: s050–s055, s051 mit 「電げんがある」. Damit ist der Abschnitt im Spätstand sichtbar, auf der Disc bleibt er unverdrahtet.“ „Stand bisher nirgends im Projekt“ bleibt richtig, in der Fanszene war es aber schon bekannt (THIA-WIP-Faden, Speicherwerte D/E/F; Mod-README Z. 23, 84).

**§8:** Neue Zeile: „Debug-Menü in s031, s148, s159: Fotos stammen aus einer Debug-Sitzung, Abfolge ist nicht der Spielablauf.“ Zeile Frau im Kittel: „Kamiya beschreibt Ada als Forscherin im weißen Kittel. Ob die Frau in s145–147 Ada oder eine andere Figur ist, ist offen.“

**§10:** Zeitangaben als Still-Nummern führen. A 1:24: „‚マーヴィン００ ２‘, Platzhaltertext“. Ergänzen: s016, s032, s037, s048, s051, s065, s075, s140, Debug-Stills.

**§11:** Ersetzen durch:
> Project Umbrella transkribiert genau unsere beiden Quellen: die Bioflames-Bilder (Video A) und die PlayStation-Museum-Fassung (unsere 40%-Disc). Die Zeile 「……レオン」 steht in Video A (s016, 0:13) im Chefbüro bei Elza. Project Umbrella schreibt sie Elza zu, der Übersetzer Enigmatism415 dem Chef; im Spiel steht kein Sprecher. Auf der 40%-Disc fehlt sie. Das Forenzitat lautet vollständig „… Annette. Except Ada. So, maybe Ada is still a spy?“ und beruht auf einem Durchlauf des geleakten Builds; „Annette“ ist dort vermutlich Figur 0x4D.

**§12:** „Video A: alle 174 Standbilder einzeln“.

**Quellenliste:** Primärquellen ergänzen: Guide S. 261, Kamiya-Tweet, The PlayStation 6/1998, biohazard archives, YouTube-Beschreibung, Hidden Palace.

## 9. Grenzen dieser Prüfung

- **web.archive.org** antwortet automatisierten Abrufen mit HTTP 429 („suspected abusive bot traffic“), WebFetch lehnt die Domain ab. Das ist eine bewusste Sperre, ich habe sie nicht umgangen. Der Bioflames-Altbestand ist daher ungeprüft.
- **the-horror.com** liefert HTTP 500, auf allen URL-Formen. Alle THIA-Zitate stammen aus Auszügen der Suchmaschine. Volltext, Autor und Datum fehlen. Die Thread-Auszüge habe ich nicht gesehen, nur deren Wiedergabe durch das Suchwerkzeug.
- **Nicht erreichbar:** tcrf.net (HTTP 403 / Verbindungsabbruch), x.com (402), neogaf.com, namu.wiki, psx-place.com (403), playstationmuseum.com/Games/BETA/BTA-076.htm (404), Eurogamer-Artikel (URL nicht gefunden).
- **Nicht gefunden:** Hardcore Gaming 101 zu 1.5; Text von Resident Evil Archives S. 218–220; Capcom Design Works; Biohazard 2 Kaitai Shinsho; Original-Zeitschriften von 1996 (Famitsu, V-Jump, Dengeki, Hyper PlayStation); Capcom-Pressemitteilung vom 25.11.1996. Aussagen von Fujiwara, Tanaka oder Ueda zum Szenario habe ich nicht gefunden.
- **Shmuplations** nur Kopfteil geprüft (Quelle „June 1998 issue of The Playstation magazine“), der Volltext stammt aus der Kopie auf biohaze.com.
- **Übersetzungen** der Interviews sind Fan-Übersetzungen, außer dort, wo ich den Scan gelesen habe. Der Wiki-Text der Guide-Q&A nummeriert anders als das Buch.
- **Pastebin** nur über WebFetch-Zusammenfassung gesehen.
- Die Figurenzuordnungen in Stills sind Sichtbefund.

## 10. Arbeitsdateien

Alle unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/`:

- `q_frames/stills/` alle 174 Standbilder, `q_frames/stills/index.txt` Zeiten
- `q_frames/sb_00.jpg` bis `sb_05.jpg` Kontaktbögen
- `q_frames/a_0013_sub_avg.png` die gemittelte Zeile 「……レオン」
- `q_frames/couch.jpg` s016 und s089 nebeneinander
- `q_frames/kittel.jpg`, `q_frames/ada_marvin.jpg` Figurenvergleich
- `q_frames/sherry.jpg` s050–s055
- `q_frames/debug.jpg` Debug-Menüs
- `q_frames/b_sofa.jpg` leere Couch in Video B
- `quellen_web/yt/beschreibung.txt` YouTube-Beschreibung im Volltext
- `quellen_web/scans_guide/p261_q34.jpg`, `p256_257_klein.jpg`, `p262_263_klein.jpg` Scans
- `quellen_web/esuteru/page.txt` Kamiya-Tweets
- `quellen_web/pu/page.txt` Project-Umbrella-Seite als Text
- `pu_map.py`, `schnitt.py`, `stills.py`, `vgl115.py` Prüfskripte
