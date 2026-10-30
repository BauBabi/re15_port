# Leon, Elza und ihre Begleiter: Was in den Capcom-Daten wirklich steht

Stand 2026-10-31. Geprüft: die unveränderte 40%-Disc (`Version_40%/`), die zwei 80%-Videos (`Version_80%/`), dazu Project Umbrella und Forenquellen als Nebenquellen. Alle 291 Textzeilen der Disc stehen mit Übersetzung, Sprecher und Auslöser in [TEXTE_40.md](TEXTE_40.md).

## 1. Die Antworten

**Treffen sich Leon und Elza?** Nein, soweit die Quellen reichen. Kein Skript der 40%-Disc führt die beiden zusammen, keine Zeile nennt den jeweils anderen, und in den Videos steht nirgends sicher beide in einem Bild. Die einzige heikle Stelle ist das Video bei 0:22 (Abschnitt 8). Beweisen lässt sich nur „nicht belegt“: Die letzten Räume haben auf der Disc keinen Text, und was im fertigen Spiel noch gekommen wäre, kennt niemand.

**Treffen Leon oder Elza den Begleiter des anderen?** Ja, und das ist der wichtigste Fund.
- **Leon sieht Sherry** in der Garage 11B und ruft ihr nach.
- **Leon trifft John und Sherry** im Zug (Raum 604).
- **Elza trifft Marvin** im Zug (604) und laut 80%-Video auch am Tresen.
- **Elza kann Ada treffen**, im Führerstand (603), als Zweig an einem Story-Flag.
- **Marvin, John und Sherry stehen im Zug zusammen**, in beiden Szenarien.

**Erfährt man vom anderen nur über Dialoge?** Auf der 40%-Disc nicht einmal das. Es gibt **keine einzige erreichbare Zeile**, in der eine Seite die andere beim Namen nennt. Man erfährt es über den Zustand der Welt: Leon findet Türen und Strom so vor, wie Elzas Gruppe sie hinterlassen hat (Abschnitt 4). Dazu kommt das Ende im Zug, das auf der Disc noch ohne Text ist.

**Die Reihenfolge ist erkennbar:** Alle drei Weltspuren zeigen in dieselbe Richtung. Elzas Gruppe ist zuerst da, Leon kommt nach und sieht die Folgen. Am Ende laufen beide Wege am Zug zusammen. Das ist ein Vorläufer des späteren Zapping-Systems.

## 2. Drei Quellen, die man nicht vermischen darf

| Quelle | Was es ist | Beleg | Aussagekraft |
|---|---|---|---|
| **40%-Disc** `Version_40%/` | Capcom-Stand, 334 Dateien, 96 Räume, alle Texte Japanisch | Exe-Stempel `CAPCOM-Nov  6 1996-13:09:19-CAPCOM`, ISO-Datum 1996-11-06 | Hart. Stand: Text, Skripte, Modelle |
| **80%-Videos** `Version_80%/` | Filmaufnahmen eines späteren Capcom-Stands | Bioflames/Complete-Disc-Material | Hart für das, was man sieht. Bildqualität oft schlecht |
| **Unser Repo-Baum** `info/Re1.5/` | **Fan-Mod „Magic Zombie Door“ von MartinBiohazard** auf Basis Team IGAS, Update 25.01.2025 | `info/Re1.5/README.TXT`, ISO-Datum 2023, 751 Dateien, 254 Dateien und 3630 Exe-Worte anders | **Kein Capcom-Beleg für Story** |

Die Mod übersetzt nicht nur. Sie ergänzt 144 Raumdateien und schreibt Szenen neu. Die README nennt das offen, zum Beispiel: „Added Marvin's cutscene in room 30C (Elza) (based on Inflames's screenshot)“, „Added ending (by Richard Mandel)“, „Added extended cutscene in TRAIN M. CAR (604) (Leon)“, „Added Leon and Elza models in room 604“, „Added helpless animation for Leon when he sees Ada die“.

**Folge:** Die alte Auswertung in `analysis/video_80prozent/BEFUND.md` §5.3 und unsere Story-Regel stützten sich auf englische Dialoge dieser Mod. Die sind Fan-Text (Abschnitt 9).

## 3. Wer wem begegnet

| Paar | 40%-Disc | 80%-Video | Urteil |
|---|---|---|---|
| **Leon – Elza** | Kein Treffen, kein Name | Kein sicheres Bild | **Nicht belegt** |
| **Leon – Sherry** | 11B0: Sherry läuft vorbei, Leon: „H-he!“ (`sub06 @0x107E`, `sub07 @0x10B6`, `sub13 @0x11D2`). 604: Leon blickt sie an | – | **Ja** |
| **Leon – John** | 604: John steht im Waggon, Leon blickt ihn an, John geht (`Plc_neck @0x6EA`, `sub05 @0x74A`) | – | **Ja, ohne Text** |
| **Elza – Marvin** | 604: Marvin an eigener Position, Elza blickt ihn an (`@0x5EA`, `Plc_neck @0x72E`) | 0:46.8 Elza und Marvin am Tresen | **Ja** |
| **Elza – Ada** | 603: Ada statt Sherry, wenn Flag (4,22) gesetzt ist (`@0x388-0x3A8`). Setzer liegt nicht auf der Disc | – | **Als Zweig angelegt** |
| **Marvin – John – Sherry** | 604, beide Dateien: alle drei im Waggon | – | **Ja** |
| **Leon – Ada, Marvin** | Durchgehend | Mehrfach | Eigenes Szenario |
| **Elza – John, Sherry** | Durchgehend | Mehrfach | Eigenes Szenario |
| **Leon – Frau im weißen Kittel** | 603: Figur 0x4D, oder Ada bei (4,22)=1 und (4,23)=0 | 1:57 im Führerstand | **Ja**, Name auf der Disc nicht genannt |
| **Elza – Frau im Kittel** | Kein Platz in 6031 | – | Nicht belegt |
| **Beide – Polizeichef** | 1150 und 1151 byte-gleich, Satz „Mir geht es gut…“ | – | Beide |
| **Elza – Roy** | 1021 mit John | 0:22 wahrscheinlich | Nur Elza |

## 4. Wie die Szenarien ineinandergreifen

Drei Stellen sind im Code hart belegt. In jeder erzwingt Leons Datei den Zustand, den Elzas Szenario erst herstellt.

| Ort | Elza-Szenario | Leon-Szenario | Beleg |
|---|---|---|---|
| **Hintertür 306** | Sherry kriecht hinein: „Das Schloss an der Hintertür mache ich von innen auf.“ | Das Flag ist beim Betreten fest gesetzt, Leon: **„Irgendwann wurde das Schloss entfernt…“** | `ROOM3060 @0xEFE Set(3,160)=1`, `@0xF9A Message_on 9` |
| **Schutzschalter 309** | Licht aus, Schalter muss erst umgelegt werden | Beides fest gesetzt, Text: **„Das Licht ist bereits an…“** | `ROOM3090 @0x1412 Set(4,8)=1`, `@0x1416 Set(4,16)=1` |
| **Zelle 121** | Zellentür startet geschlossen, geht erst mit Flag (3,98) auf | Tür steht fest offen | `ROOM1210` `Obj_model_set` Drehung `0x04C0`; `ROOM1211` Drehung 0, dann `Ck(3,98)` |

Weitere Klammern ohne Code-Zwang:
- **Dieselbe Garage:** Sherry läuft in 11B0 und 11B1 von derselben Stelle `(0xC5DA, 0xA210)` los. Beide Figuren sind also etwa zur selben Zeit dort.
- **Gleiche Handlung, gleiches Ziel:** Leon braucht Impfstoff für Marvin (401), Elza für Sherry (501). Beide haben dieselbe Zeile: 急いでワクチンを持ってこなければ！ („Ich muss den Impfstoff schnell holen!“).
- **Beide werden zu den Schutzräumen geschickt:** Ada zu Leon: „Geh zum unterirdischen Schutzraum“. Sherry zu Elza: „Schutzraum Nr. 2 kann ich öffnen.“

Ein Forenbeitrag nennt die Zellentür ebenfalls und schließt daraus, Elza sei vor Leon da gewesen. Die Disc bestätigt die Tür. John selbst steht in 121 aber nicht im Skript, das bleibt Schluss, kein Beleg.

## 5. Die beiden Wege nebeneinander

Orte aus den 531 dekodierten Disc-Hintergründen ([Übersicht](belege/raeume_40prozent.jpg)). Die Reihenfolge ist aus Flags und Texten erschlossen. „–“ heißt: auf der Disc nichts dazu.

| Ort | Leon | Elza |
|---|---|---|
| **Dach 117** | Kein Text (Video: Leon auf dem Dach) | John: „Elza, beeil dich! … Wir müssen das Ding runterstoßen!“ |
| **Revier** | 109: Ada ruft hinter einer Tür, Leon befreit sie | 102: Roy verwandelt sich, „Das ist nicht mehr Roy!!“ Funk tot, Strom weg |
| **Chefbüro 115** | „Mir geht es gut. Denk an deine eigene Sicherheit.“ | Derselbe Satz |
| **Zufahrt 10B** | – | Elza zu John: Mädchen in der Tiefgarage eingesperrt, John kennt eine Abkürzung |
| **Garage 11B** | **Sieht Sherry**, Marvin: „Leon! Du lebst!“, Rolltor durchbrochen, Plan: Kanaldeckel, „Lass die Frau runter“ | Findet Sherry, „Warte brav im Wagen“ |
| **Parkgarage 11C** | Ada: Marvin ist umgekehrt, um dich zu suchen. Kampf, „Was für ein Ungeheuer“ | – |
| **Kanal** | Ohne Text | Ohne Text, John im Skript (20B1) |
| **Fabrikhof 300** | Leon, Marvin: tote Polizisten, Umbrella-Ermittler. Ada schon vorgegangen, „nicht mal die Miene verzogen“ | Elza, John, Sherry: dieselben Toten, „früher eine Chemiefabrik“ |
| **Hinterhof 306** | **Tür schon offen** | Sherry: geheimer Eingang, öffnet von innen |
| **Raum 309** | **Licht schon an** | **Sherry spielbar**, rettet Elza und John. John: „Wenn es mein Leben kostet“. Sherry: „Hier hat mein Papa gearbeitet“ |
| **Halle 30E** | Marvin am Bein verletzt, Ada taucht auf, Halle fällt | Elza hat etwas gesehen: „Das war doch kein Mensch“ |
| **Aufzug 307** | Ada: „In die Hölle…“ | Monster kam aus dem Aufzug, Sherry kann ihn bedienen |
| **Labor 400** | Leon hält auf, „Kümmere dich um Marvin!“ | Sherry: „Papas Labor“, läuft los |
| **Halle 403** | – | Sherry verschwunden, Suche |
| **401 / 501** | **Ada: „Ich gehöre zu Umbrella.“** Impfstoff aus dem Virusstamm, Ada geht eigene Wege. Später: „Marvin, wach auf!!“, „Schnell zum Schutzraum!“ | **Sherry vom Virus zerfressen**, Elza holt Impfstoff, Sherry wacht auf. Monster zerstörte die Energie, Explosion droht, „Schutzraum Nr. 2 kann ich öffnen“ |
| **Führerstand 603** | Frau im Kittel oder Ada, lange Szene, WARNING-Konsole blinkt | Sherry oder Ada |
| **Waggon 604** | Marvin, John, Sherry | Marvin, John, Sherry |

Figuren je Raum: `werkzeuge/figuren.py`. Der Disc-Text nennt Ada schon エイダ, nicht Linda. Project Umbrella führt in einer anderen Build-Fassung „Linda“ und „Brian“.

## 6. Das Ende im Zug

Beide Räume haben auf der Disc **keinen Textblock**, die Choreografie ist aber geschrieben. 603 ist der Führerstand, 604 der Waggon mit Sitzbänken ([Hintergründe](belege/hintergruende_603_604.jpg)).

**604 Leon (`ROOM6040`):** Marvin, John und Sherry stehen an festen Plätzen. In `sub04` dreht Leon den Kopf exakt zu Sherrys Position, dann zu Johns, und John läuft los.

**604 Elza (`ROOM6041`):** Eigene Choreografie, also keine Kopie. Marvin steht anderswo und hat ein anderes Startbyte, Sherry ebenfalls. Elza blickt zu Marvin (`sub06`, `sub09`), in `sub11` zwischen John und Sherry hin und her.

**603:** Der Eintrag ist symmetrisch aufgebaut:
```
Leon:  (4,22)=1 und (4,23)=0  -> Ada     sonst -> Figur 0x4D
Elza:  (4,22)=1               -> Ada     sonst -> Sherry
```
Flag (4,22) und (4,23) werden in keinem Raum der Disc gesetzt, der Setzer lag in nicht vorhandenen Räumen. Dass Ada überhaupt davon abhängt, heißt: Ihr Überleben oder ein Routenwechsel war als Zustand geplant.

**Der andere Hauptcharakter fehlt in beiden Waggons.** Weder in 6040 noch in 6041 steht er im Skript. Wo Elza in Leons Ende ist und umgekehrt, sagt die Disc nicht. Die Mod beantwortet das mit eigenem Text („She's out there fighting the monsters!“). Das ist Erfindung.

Einschränkung: Ausgeführt werden im Stand nur `sub06` (6040), `sub11` (6041, 6030) und `sub08` (6031). Die übrigen Szenen sind fertig geschrieben, aber ihr Auslöser fehlt noch.

## 7. Spielbare Begleiter

Das stand bisher nirgends im Projekt:

- **PL0D, PL0E, PL0F sind die Begleiter als Spielerfiguren:** Mesh und Textur sind byte-gleich zu Typ 0x40 (Marvin), 0x49 (John), 0x4B (Sherry). Dazu gibt es `PL0FW00.PLW`.
- **Sherry:** Elzas 3091 setzt zwei Türen auf den Text 「はやくエルザたちを助けなければ...」, **„Ich muss Elza und die anderen schnell retten…“** (`@0x1464`, `@0x146E`). Das kann Elza nicht sagen. „Ich komme nicht an den Schalter…“ passt zu einem Kind.
- **Marvin:** `ROOM3000 @0x161A` fragt `work_vars[0x10] == 13` ab. Dieser Wert ist `0x800B0FF0`, die Spielfigur. Bei PL0D erscheint statt der Leiterfrage **„Meine Kräfte reichen nicht mehr. Schnell zu den Gefährten.“** Das passt zum verletzten Marvin.
- **Technik:** Der Raumlader tauscht die Figur, wenn der Wert sich ändert (`@0x80039768`). Auf der Disc schreibt aber kein Skript diesen Wert. Die Abschnitte sind vorbereitet, nicht verdrahtet.
- **John:** PL0E liegt bereit, eine Skriptstelle dazu gibt es nicht.

## 8. Fallen beim Deuten

| Falle | Was wirklich gilt |
|---|---|
| **Video 0:22: Elza kniet neben einem Polizisten, der aussieht wie Leon** | Typ 0x47 trägt auf der Disc dieselbe R.P.D.-Kunst wie Leon, in der Elza-Szene heißt die Figur Roy. Sehr wahrscheinlich Roy, die Mod führt die Szene auch so. **Das Bild allein beweist es nicht**, es hat keinen Text und einen blauen, unfertigen Hintergrund ([Bild](belege/video_elza_roy_marvin_john.jpg)) |
| **Typ 0x4D in Leons 603 trägt Elzas Anzug** | Auf der Disc ist 0x4D Elzas Kunst auf Adas Rig ([Vergleich](belege/vergleich_4D_gegen_elza.png)). Sieht nach Treffen aus, ist aber ein Platzhalter: Typ 0x40, 0x45, 0x47 teilen sich ebenfalls einen Blob (md5 `0c1f2a`). Im 80%-Video steht an dieser Stelle die Frau im Kittel ([Bild](belege/video_kittel_fuehrerstand.jpg)). Der Raumvergleich bei 1:57 ist Sichtbefund, die Rechnung schlug wegen der Bildqualität nicht an |
| **Marvin sieht im 40%-Stand aus wie ein Polizist in Leons Art** | Typ 0x40 ist Platzhalter. Der Marvin des 80%-Stands (hellblaues Hemd) ist ein eigenes, späteres Modell |
| **John steht in Leons 3000** | Platzhalter. Text und Video 1:33 zeigen Marvin, die Szene ist aus 3001 kopiert |
| **Ada und Marvin in Elzas 11B1, Ada in 1091** | Parkposition (−30000, −30000), kein Skript spricht sie an. Kopier-Rest |
| **John-Szene in Leons 4030** | Text aufrufbar, aber `@0x528`/`@0x52C` setzen (5,4) sofort auf 1, toter Code |
| **Beide Dateien tragen denselben Textblock** | Sagt nichts. Entscheidend ist, welche Nummern das Skript aufruft. 501: Nr. 6–16 werden gar nicht aufgerufen |
| **Alle NPC-Modelle liegen in beiden Archiven** | Stimmt (`CDEMD0`/`CDEMD1` byte-gleich), ist aber nur eine Möglichkeit, kein Treffen |
| **Complete-Disc-Text „Leon und Elza stürmen das Labor“** | Werbetext zu Clips, die beide getrennt zeigen |

## 9. Was nur in der Mod steht

Diese englischen Zeilen sind **kein Capcom-Text**. Die Szene Elza–Marvin selbst ist echt (Video 0:46.8), ihr Dialog nicht bekannt.

| Raum | Mod-Text | Status |
|---|---|---|
| 30C1 | „I'm Marvin Branagh, RPD. I'm here with Leon Kennedy.“ | Raum nicht auf der Disc. Video zeigt die Szene ohne lesbaren Text |
| 30C1 | Elza: „Leon?! How is he? We got separated back at the RPD.“ | Fan-Text. Widerspricht nichts, belegt aber nichts |
| 30C1 | „Will you tell him that me and John and Sherry are here…“ | Fan-Text |
| 6030 | „Where's Elza?“ – „She's out there fighting the monsters!“ | Disc: kein Text. Fan-Text |
| 6030 | „Where's that biker girl?“, Annette startet den Zug | Fan-Text |
| 6031 | „Where's Leon?“ – „He's fighting off monsters.“ | Fan-Text |
| 6031 | „I know how to start this! I watched Mom and Dad do it.“ | Fan-Text, passt aber gut zu Sherry im Führerstand |
| 604 | „Leon and Elza models“ | Laut README Mod-Zusatz, auf der Disc nicht vorhanden |

## 10. Videos: was zu sehen ist

Video A = `Biohazard 1.5 Final Build.mp4`, Video B = `Resident Evil 1.5 (All Footage).mp4`.

| Stelle | Inhalt | Disc |
|---|---|---|
| A 0:10 | John: „Elza! Geh nicht aufs Dach!“ | Nicht auf der Disc, späterer Stand |
| A 0:22 | Elza neben verletztem Polizisten | Abschnitt 8 |
| A 0:39 / B 3:21 | Sherry: „…Elza, ich kenne einen geheimen Eingang“ | 306 #3–6, wörtlich gleich |
| A 0:46.8 | **Elza und Marvin am Tresen** | Raum fehlt auf der Disc, also später gebaut |
| A 0:47.4 | Elza, John, Sherry, John: „Das war knapp!“ | 309 #9, gleich |
| A 1:06.7 | Leon: „Ich muss das dem Chef melden“ | Nicht auf der Disc |
| A 1:07.6 | „…von Zombies gefressen oder selbst zu Zombies geworden“ | Nicht auf der Disc |
| A 1:24 | Leon, Ada, Marvin an der Leiter, „マーヴィンの… 2“ | Rest unleserlich |
| A 1:33 | Leon und Marvin im Fabrikhof | Bestätigt: John in 3000 ist Platzhalter |
| A 1:42 | Ada: „Bist du ihm nicht begegnet?“ | 11C #3, gleich |
| A 1:55–1:56 | Riesen-Birkin, Frau im Kittel im roten Gang | Nachtrag 38, Spur R |
| A 1:57 | Leon und Frau im Kittel im Führerstand | 603 |
| A 2:02–2:04 | Leon und eine blonde Frau im Weiß am roten Zug | Bahnsteig, nicht auf der Disc |
| A 2:16–2:18 | Leon, Marvin, Ada: Kanaldeckel-Plan | 11B #10, #16 gleich, #17 im Video umformuliert |
| B 0:12 | Figurenwahl „PLEASE SELECT MAIN CAST“ | Beide getrennt wählbar |
| B 7:21–12:21 | Geprüft auf Leon und Elza zusammen | Nur Zombies |

Kein Video zeigt eine Szene mit John oder Sherry neben Leon. Das muss nichts heißen, denn das Ende ist in beiden Videos nicht zu sehen.

## 11. Nebenquellen

- **Project Umbrella** führt Szenetexte einer anderen Fassung („Bioflames/PlayStation Museum“). Dort steht im Chefbüro eine Elza-Zeile **「・・・レオン」 („…Leon“)**. Das wäre die einzige Stelle, an der Elza Leon kennt. Sie steht **nicht** auf der 40%-Disc (Textblock 115 hat nur eine Zeile, Skripte 1150/1151 sind byte-gleich) und ist in unseren Videos nicht zu sehen. Unbestätigt, ich kenne den Kontext nur aus einer Zusammenfassung der Seite.
- **GamePro** schreibt: „Leon S. Kennedy und Elza Walker begegnen sich im Spielverlauf von Resident Evil 1.5 nicht.“ Das passt, geht aber nicht auf die Begleiter ein.
- **Forum The Horror Is Alive** nennt: „In the final train, everyone is there, Marvin, John, Sherry and Annette.“ Die Disc bestätigt Marvin, John und Sherry im Waggon. Die Seiten waren beim direkten Abruf nicht erreichbar, ich kenne das Zitat nur aus den Suchergebnissen.

## 12. Grenzen

- Die 40%-Disc endet beim Text vor dem Zug. Alle Aussagen zum Ende stammen aus Choreografie ohne Dialog.
- Viele Räume des Plans fehlen auf der Disc, die Mod musste 144 Raumdateien ergänzen. Darunter ist 30C mit der Marvin-Szene. Was dort lief, zeigt nur das Video.
- Sprecher stehen nicht im Text, ich habe sie aus dem Zusammenhang gesetzt.
- Fehlende Kanji sind aus dem Zusammenhang ergänzt und in TEXTE_40.md markiert.
- 25 Zeilen werden in keinem Skript aufgerufen, vor allem 501 #6–16 und die Texte in 50A, 506, 512, 514.
- Die Videos sind stark verzerrt. Figuren-Zuordnungen dort sind Sichtbefund.
- Durchsicht: Video A jede Sekunde, Video B alle 3 Sekunden, verdächtige Abschnitte enger. Sehr kurze Einstellungen können durchgerutscht sein.

## 13. Folgen für unser Projekt

- **Story-Regel:** „Leon und Elza begegnen sich nie“ bleibt gedeckt. „Elza kann Ada begegnen“ ist jetzt durch 6031 belegt. Neu: Leon sieht Sherry und trifft John, Elza trifft Marvin.
- **Unsere Dialoge in 30C1, 6030, 6031** sind Fan-Text der Mod und damit nicht Original. Der Port liest sie über die RDTs aus `shared_assets`.
- **Begriff:** `CLAUDE.md` nennt `info/Re1.5/` „Auslieferungsstand“. Das ist die Mod, nicht Capcoms Stand. Für Verhalten und Assets ist das bisher unser Maßstab, für Story darf es das nicht sein.
- **Nicht übernommen:** spielbarer Marvin, John, Sherry. Wäre eine eigene Aufgabe.

## Nachprüfen

- Texte: `werkzeuge/jp.py` (Zeichentabelle, Raster aus `PSX.EXE @0x800289bc`, `@0x800289ec`, `@0x80028a44`, `@0x80028b00`, Glyphen aus `DATA/TEX.TIM`), `dump40.py`, `texte_bauen.py`
- Skripte: `zensus.py`, `figuren.py`, `quer.py`, `spuren.py`
- Modelle: `ems.py` (TOC `@0x80072f38` Leon, `@0x80073178` Elza)
- Einzelne Räume: `python re15_port/tools/scd_dump_room.py "Version_40%/PSX/STAGE6/ROOM6040.RDT"`
- Hintergründe: `probe_bg_dump.exe "Version_40%/PSX" <ziel> <stage>`

Quellen außerhalb der Disc:
- [Project Umbrella: BIOHAZARD 1.5 Scenario & Description](https://www.projectumbrella.net/biohazard-15-scenario--description.html)
- [GamePro: Vaporware Resident Evil 1.5](https://www.gamepro.de/artikel/vaporware-resident-evil-15-das-untote-resident-evil,3083099.html)
- [The Horror Is Alive: 1.5 WIP Spoiler Thread](http://www.the-horror.com/forums/archive/index.php/t-9980-p-2.html)
