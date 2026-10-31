# Prüfung TEXTE_40.md: Dossier „texte“

Stand 2026-10-31. Geprüft wurden Zeichentabelle, Übersetzungen, Sprecher und indirekte Bezüge, jeweils aus den Rohdaten neu hergeleitet: `Version_40%/PSX.EXE`, `PSX/DATA/TEX.TIM`, alle RDTs, beide Videos. Arbeitsdateien und Belegbilder liegen im Unterordner `pruefung/texte/` (Abschnitt 10).

## 0. Ergebnis in Kürze

1. **Hauptfund, widerspricht BERICHT §11:** Die Zeile 「……レオン」 steht in unserem eigenen Video A bei **0:12.83–0:13.63** lesbar im Bild, in Raum 115, Einstellung 7 (Sofa), mit der rot-weißen Figur im Vordergrund. Der Bericht sagt, sie sei „in unseren Videos nicht zu sehen“. Wer spricht, ist offen.
2. Project Umbrellas „Bioflames Version“ ist **keine andere Fassung**, sondern dasselbe Material wie Video A und B (Wasserzeichen und wortgleiche Zeilen).
3. Die Sherry-Spielszene ist nicht nur aus dem Skript erschlossen: **A 0:40.2–0:44.2 zeigt eine kleine blonde Figur allein** in mehreren Räumen, dazu 「電げんがある」.
4. Zeichentabelle: Das Raster stimmt. Drei benutzte Kanji sind falsch gelesen (聞, 接, 意), dazu drei Satzzeichen und ein Quote-Paar. Vier Einträge sind sicher, zwei weitere wahrscheinlich falsch und unbenutzt. Die Übersetzungen hatten die drei Kanji schon richtig erraten, nur die Spalte Japanisch ist falsch.
5. Übersetzung: **zwei echte Fehler** (30E #10, 307 #5), sechs Ungenauigkeiten. Der Rest ist sinngetreu.
6. Sprecher: Keine falsche Zuordnung bei den wichtigen Szenen, vier Nachschärfungen (308 #0/#1, 307 #4, 514 #0).
7. Indirekte Bezüge: Nur **306 #9 und 309 #0** sind exklusiv für Leon und zeigen auf Elzas Gruppe. Kein Hinweis auf Frau, Polizist oder Motorrad als den jeweils anderen. „Die Frau“ in 11B #20 ist Ada, durch Regie und Video belegt.

## 1. Zeichentabelle jp.py

### 1.1 Raster aus der Disc-EXE: bestätigt

Eigene Disassemblierung von `Version_40%/PSX.EXE` (Load 0x80010000, Datei-Offset 0x800):

| Zeichen | Adresse | Befehl | V |
|---|---|---|---|
| Einzelbyte | @0x80028ad8 | `addiu v0,v0,0x1c` @0x80028afc | Zeile·14 + 28 |
| Präfix 09 | @0x800289bc | `addiu v0,v0,-0x2e` @0x800289e8 | Zeile·14 + 210 |
| Präfix 0A | @0x800289ec | kein Offset | Zeile·14 |
| Präfix 0B | @0x80028a44 | `addiu v0,v0,-0x3c` @0x80028a94 | Zeile·14 + 196 |

U ist `(c%18)*14` (@0x80028b64–b88). Zeile = c/18 über Magic 0x38E38E39 (@0x80028910). Einzelbyte und 09 hängen an Liste 0x800b829c (@0x80028b20), 0A und 0B an 0x800b82a8 (@0x80028a18, @0x80028a70).

### 1.2 Steuercodes: bestätigt, mit einer Klärung

Sprungtabellen @0x8001096c (Vorschub) und @0x8001099c (Zeichnen). Das Parameterbyte hinter `01` wertet jp.py nicht aus. **Es ist ein Timer, keine Verkettung:** @0x800282c0: `01 00` = Zustand 5 (Tastendruck), `01 xx` = Zustand 6 mit Zähler xx (@0x80028728). Damit kann keine Zeile eine andere nachziehen, die Spalte „nicht ausgelöst“ bleibt gültig.

`04 xx` ist Textgeschwindigkeit, `05 xx` Farbe, `06 xx` Einfügung aus der EXE-Tabelle @0x80073a88 + 0x80073820 (72 Item-Namen), `07` Rücksprung, `08` Zeilenumbruch, `02 xx` Seitenwechsel oder Wartezeit.

### 1.3 Methode

Gerendert mit der echten CLUT-Zeile 0 (@VRAM 256,480). `sheet.py` hatte Index·255/max benutzt, das kehrt die Helligkeit um, denn Index 1 ist der hellste Wert. Jede Zelle zusätzlich gegen alle JIS-X-0208-Kanji per Korrelation in fünf Schriftgrößen, strittige Fälle als Bild gegen benannte Kandidaten. Der Abgleich ist bei gepixelten Kanji nur ein Hinweis, bei Kana unbrauchbar. Entschieden hat das Bild zusammen mit dem Sinn aller Fundstellen.

### 1.4 Falsche Einträge, die in Texten vorkommen

| Code | jp.py | Richtig | Beleg | Betroffene Zeilen |
|---|---|---|---|---|
| 0A:C9 | 閉 | **聞** | Tor mit 耳 im Bild. 閉 fehlt im Font: 10B #2 und 11B #1 haben dort `00` | 300 #8 聞く事が, 501 #27 アナウンスは聞いた |
| 0A:86 | 探 | **接** | Bild, Korrelation 接 .57 gegen 探 .51. 探 fehlt im Font: 309 #22 hat `00` | 403 #2 接続されている |
| 09:29 | 竜 | **意** | Unten 心 sichtbar | 401 #14 ちゅう意しなくては |
| 0x17 | `;` | **、** | Komma unten links, A 0:47.5 zeigt はあ、はあ | 13 Zeilen |
| 0x18 | `,` | **。** | Kreis unten links | 6 Zeilen |
| 0x3C | `.` | **・** | Punkt auf halber Höhe | 23 Zeilen |
| 09:1A, 09:1B | ゛ ゜ | **Anführungszeichen** | Zwei Striche oben, Gebrauch ゜メインヒューズ゜ | 8 Zeilen |

Die Kopfnotiz in TEXTE_40.md „Leerstellen oder falsche Zeichen im Japanischen sind echt“ gilt für diese vier Sätze nicht: 閉く, 閉いた, 探続, ちゅう竜 sind Lesefehler von jp.py, nicht von der Disc. Die deutschen Sätze sind in allen vier Fällen richtig.

### 1.5 Falsche Einträge ohne Wirkung auf Texte

| Code | jp.py | Richtig | Beleg |
|---|---|---|---|
| 0A:01 | 薬 | **楽** | 楽 .78 gegen 薬 .59. 薬 steht schon auf 0A:D9, 楽譜 mit 0A:C2 |
| 0A:84 | 戸 | **声** | 声 .81 gegen 戸 .68, Bild eindeutig |
| 0A:8E | 増 | **槽** | Linke Seite 木, rechts 曹 |
| 09:34 | 界 | **兜** | Bild. Gehört zu 剣, 鎧, 盾 |
| 0A:DF | 溶 | **熔** (wahrscheinlich) | 熔 .77 gegen 溶 .65, links 火 |
| 0A:F3 | 溜 | **溶** (wahrscheinlich) | Bild, bildet mit 0A:F4 溶液 |
| 09:27 | 運 | nicht 運, vermutlich 遺 | 運 .50, 遺 .62. 3070 #8 ersetzt 運 durch 階, also fehlt 運 im Font |
| 0B:1E | 途 | unklar | Kein Kandidat über .46 |

Die vom Auftrag genannten übrigen Fälle 0A:12, 13, 26, 3C, 4B, 4D, 4F, 56, 70–72, 78, 82, 88, C8, D5, F1, F5 und 0B:10, 11, 15, 26, 32, 33, 09:28 stimmen. 0A:12, 26, 4B, 4D, 4F, 70, 78, 82, 88, 8E, C8, F1, F3, F5 und alle hier genannten 0B-Fälle werden in keinem Text benutzt.

### 1.6 Ersatzzeichen sind ein eigenes Phänomen

Die Bytes sind echt, die Glyphen auch. Der Text benutzt ein vorhandenes Zeichen für eines, das der Font nicht hat:

| Glyph | Steht für | Zeilen | Daneben auch echt benutzt |
|---|---|---|---|
| 東 0B:3A | viele verschiedene | 20 Zeilen, 46 Zeichen, nur 307 und 30E | nein |
| 度 0B:1C | 場 | 10B #2, #3, 300 #4, #16, 309 #22, 30E #3 | 今度 (300 #8, 401 #26) |
| 西 0B:39 | 命 | 309 #17 (zweimal) | nein |
| 社 0B:0A | 近 | 10B #3 | nein |
| 根 0B:2B | 伝 | 11B #18 | nein |
| 仲 0B:18 | wohl 獄 | 307 #2 | 仲間 (300 #1) |
| 女 0B:06 | 顔 | 300 #7 | 女 (10B, 11B, 300) |
| 階 0B:01 | 運 | 307 #8 | 階 (11C #2) |
| 品 0B:20 | wohl 弱 | 403 #4 | 薬品 (300 #12) |
| 怖 0B:21 | 姿 | 30E #19 | nein |

Dasselbe Muster steht in den Item-Namen der EXE: 索毒スプレー (解毒, 0B:05, @0x80073955), グレネード通点弾 (通常, 0B:1A, @0x800738f5). Innerhalb von 300 #7 steht 女 für 女 und für 顔, also ist das keine Umcodierung je Raum oder Fontstand. Der Mechanismus ist unklar.

**Kennzeichnung:** BERICHT §12 sagt, fehlende Kanji seien in TEXTE_40.md markiert. Tatsächlich tragen mindestens 43 Zeilen Lücken oder Ersatzzeichen, von Hand gezählt: 109 #4; 10B #0, #2, #3; 11B #1, #12, #17, #18; 300 #4, #7, #16; 307 zehn Zeilen; 309 #9, #11, #13, #17, #19, #21, #22; 30E zehn Zeilen; 403 #4; 506 #0, #2, #5, #8. Markiert sind 5 (109 #4, 10B #2, #3, 11B #12, #17).

## 2. Übersetzungen

Alle 291 Zeilen sind gelesen. Die vom Auftrag genannten Räume 300, 306, 307, 309, 30E, 400, 401, 403, 501, 11B, 11C, 109, 102, 10B sind Satz für Satz gegen die Rohbytes geprüft.

### 2.1 Fehler

**30E #10.** Disc: `東東東上の東が東まってきてるぜ！` Bericht: „Die von oben strömen zusammen!“ Das ist falsch. 東東東上 ist ein Vierer-Kompositum auf 上: **予想以上の数が集まってきてるぜ！** Richtig: „Es sammeln sich mehr, als ich erwartet hatte!“ Von „oben“ ist nicht die Rede. Der zweite Satz bleibt: こんな郊外まで… 地上はもう ダメだな…

**307 #5.** Disc: `東かにね きっとどこかにいるわ…！` Bericht: „Stimmt.“ (確かにね). Das geht nicht, denn 確 liegt im Font und im selben Textblock: 0A:04 in 307 #14 確実. Für 確 hätte niemand 東 gesetzt. Passend ist **静かにね**, das folgende „OK Elza, den Rücken decke ich dir“ fügt sich ein. Richtig: „Leise! Es ist bestimmt irgendwo hier…!“

### 2.2 Ungenau

| Zeile | Bericht | Besser | Grund |
|---|---|---|---|
| 30E #12 | „nirgends ein Ausweg“ | „nirgends ein Versteck“ | 東れられそう = 隠れられそう. 逃 liegt im Font (0A:AC, 11B #16), John sucht seit 300 #16 ein Versteck |
| 30E #12 | „hinten im Raum“ | vermutlich „hinten im Lager“ | 奥の東東. 部 und 屋 liegen im Font (0A:C7, 0A:DA), also nicht 部屋. Wohl 倉庫, nicht sicher |
| 30E #3 | „in diesem Gebäude“ | „in dieser Fabrik“ | 東度 = 工場, 度 steht für 場 wie in はいこう度 |
| 306 #9 | „wurde das Schloss entfernt“ | „ist das Schloss aufgemacht worden“ | はずされている ist dasselbe Verb wie Sherrys 中から外すわ (306 #7). Die Übersetzung verdeckt genau die Klammer, um die es geht |
| 109 #4 | „ich stehe in deiner Schuld“ | „danke, mein Dank“ | ありがとう □を言う, die Lücke ist 礼 |
| 506 #0 | „Schutzanzüge“ | „Arbeitsanzüge“ | 作□用 = 作業用 |
| 501 #19 | „Sie fiebert schon lange“ | „Sie stöhnt schon lange im Schlaf“ | うなされる |

### 2.3 Lesarten, die als sicher auftreten, es aber nicht sind

- **307 #2 „In die Hölle…“:** Die Disc hat 地仲さ. 地獄 ist naheliegend, aber erschlossen. 地下 scheidet aus, denn 下 liegt im Font. BERICHT §5 zitiert den Satz ohne Vorbehalt.
- **30E #3 „am Bein“:** Disc 東をやられて. 足 ist erschlossen. BERICHT §5 „Marvin am Bein verletzt“ steht ohne Vorbehalt.
- **30E #7:** 東当東いな = 相当深いな oder 相当酷いな. „Tief“ ist eine von zwei Lesarten.

### 2.4 Von den Videos gestützt

- **309 #9:** A 0:47.5 zeigt 間一ぱつってヤツだな！ und …にくいつかれてた所だぜ. Damit ist die Lücke bewiesen, „in den Hintern gebissen“ stimmt. Der Bericht nennt die Zeile „gleich“, tatsächlich füllt das Video zwei Lücken mit Kana.
- **10B #0:** A 0:25.9 zeigt zwei unscharfe Zeichen (vermutlich まだ, Project Umbrella: XX), dann Abstand, dann 内に女の子がいるの / 戻らなきゃ. Die Lücke für 署 ist auch dort anscheinend noch leer.
- **300 #12:** A 0:38.8 zeigt …われてないはずだ. Die Disc hat ねぇ, das Video ない.

### 2.5 Neu gelesene Stellen

| Zeile | Disc | Lesart |
|---|---|---|
| 300 #4, #16 | はいこう度, かくれ度所 | 廃工場, 隠れ場所 |
| 300 #7 | 女いろ | 顔色 |
| 307 #0, #7, #13 | 東れ | 乗れ, 現れた, 現れた |
| 307 #8 | 大東の東物を階んでいた | 大型(大量)の荷物を運んでいた |
| 307 #15 | 電東 | 電源 |
| 309 #11, #13 | お□ちゃん, □にきる, □前 | お嬢ちゃん, 恩にきる, 名前 |
| 309 #17 | 西の□人, 俺の西に□えても | 命の恩人, 俺の命に代えても |
| 309 #19, #21, #22 | □いていた, □まって, 度所を□し | 働いていた, 集まって, 場所を探し |
| 30E #5 | 東東し東めて, ガラス東り, 人東東手 | 集結し始めて, ガラス張り, 人数相手 |
| 30E #8 | 東面東関 | 正面玄関 |
| 30E #12 | 東り, 東まれ, 東生 | 周り, 囲まれ, 畜生 |
| 30E #19 | 無東東中, 怖を消して | 無我夢中, 姿を消して |
| 30E #21, #22, #23 | 東んで, 東手, 一東, 東く | 潜んで, 相手, 一匹, 早く |
| 403 #4 | 品まって | 弱まって |
| 506 #0, #2, #5, #8 | 作□, □して, 機□ | 作業, 示して, 機器 |

## 3. Sprecher

Geprüft über eigene Regie-Sichten mit aufgelöstem Gosub (`pruefung/texte/regie_*.txt`). Achtung: Die Szenen liegen in vielen kleinen Subs, ein einzelner Message_on sagt wenig, entscheidend ist die Folge.

**Durch das Skript belegt:**
- 11C: #1–3, #6 liegen in sub04 (`Work_set EM 2 ADA`), #8 in sub06 (ADA, Plc_neck), #0, #4, #5, #7, #9 in Leons Abschnitten. Alles wie im Bericht.
- 11B Elza: #0–8 in sub05–17 mit EM 0 Sherry. Wie im Bericht.
- 11B Leon: #9 nach `sub06` mit EM 0 Sherry und sub13. #12/#13 mit einer Kopfbewegung Leons dazwischen in sub19 (Plc_neck @0x129E), wirkt als stumme Antwort. #16 Leon, #17 nach Marvins Motion in sub27.
- 115: EM 0x45 bekommt `Plc_motion` direkt vor #0 (1150 @0xE56).
- 109: #0, #2 Ada hinter der Tür, sub12 führt EM 0 ADA heraus, #3 Leon, #4 Ada, #5 Leon.
- 30E Leon, 30E Elza, 306 Elza, 501, 400: Reihenfolge und Register passen, EM-Slots stimmen.
- 3000 und 3001: Es gibt keine Regie je Zeile, alle Message_on stehen hintereinander (@0x1660–0x1690). Zuordnung nur aus Inhalt und Register. Plausibel, aber nicht bewiesen, zum Beispiel 300 #8 Leon.

**Nachschärfungen:**

| Zeile | Bericht | Befund |
|---|---|---|
| 308 #0 | Marvin oder Ada | **Marvin oder Leon.** Ada benutzt in keiner Zeile der Disc 〜ぜ. Ihre Satzenden sind だ, ぞ, な, か, さ |
| 308 #1 | Leon oder Ada | **Eher Ada** („…das ist… also doch…“ passt zur Insiderin), danach macht Leon eine Kopfbewegung (sub18 Plc_neck 03 01, dieselbe wie Sherrys Reaktion in 11B1 sub17). Nicht bewiesen |
| 307 #4 | Marvin | **Unklar**, Marvin oder Ada. Keine Regie (sub07 @0x336C). Beide sagen sonst 急げ |
| 11B #19 | Marvin | Wahrscheinlich, aber ohne Regie. Ada hat 400 #4 いいから急げ |
| 306 #9 | Leon | **Erzähltext aus Leons Sicht**, gleiche Aufrufklasse wie #0 (Flags 80 FF, `01 00` Tastendruck) |
| 401 #18, 501 #43 | Leon, Elza | Untersuchen-AOT, also Gedanke. In 4011 und 5010 kopiert (main00 identisch), dort ohne Sinn |
| 514 #0 | Frauenstimme | **Nicht Ada**: 開いたわ, この奥よ ist weiblich, Ada spricht durchweg männlich-knapp. Sherry ist Kandidatin (501 #35 2号シェルターなら開けられるわ, Sherry-Register わ). Nur 5140 hat ein zweites Objekt und ein unbenutztes Schiebe-Sub02, das spricht eher gegen sie. Offen |

**Sherry in 309:** Die Einträge #1, #3, #5, #8 gehören zur Sherry-Passage, die Disc nennt sie „System“ außer #8. In sub07 ist die Spielfigur wieder Elza, John ist EM 0, Sherry EM 1, also Spielerwechsel nicht verdrahtet, wie der Bericht sagt. Stark neu: A 0:40.2–0:44.2 zeigt die Figur (Abschnitt 6).

## 4. Spalte „Auslöser“

Eigener Rohbyte-Scan über die SCD-Bereiche aller RDTs: Die 25 als „nicht ausgelöst“ geführten Zeilen stimmen. Der einzige Treffer (5010 @0x3BC) sind AOT-Daten von #43. Kein Raum ohne Textblock ruft Message_on.

Zwei Einträge sind trotzdem irreführend, weil die Spalte nur prüft, ob ein Aufruf existiert:

- **306 #0 „Leon + Elza“:** In 3060 ist der Zweig tot. Set(3,160)=1 @0xEFE läuft immer, Ck(3,160)=0 @0xF06 wird nie wahr. Leon sieht „Abgeschlossen“ nie. Das ist gerade der Kern von BERICHT §4.
- **403 #5–8 „Leon + Elza“:** In 4030 tot (Set(5,4)=1 @0x52C, John ist dort gar nicht gesetzt). BERICHT §8 sagt das richtig, TEXTE_40.md widerspricht.

Weitere Kleinigkeiten: Die Kopfnotiz sagt „oft denselben Textblock“, tatsächlich sind **alle 25 Paare byte-gleich**. Der Verweis „siehe BERICHT.md Abschnitt 8“ für Räume ohne Text zeigt ins Leere, Abschnitt 8 behandelt Fallen.

## 5. Indirekte Bezüge

Byte-Suche über alle 333 Dateien der Disc nach kodierten Namen und Schlüsselwörtern (`tx_namen.py`). Namen kommen nur in RDT-Textblöcken vor, die Treffer in BSS sind Zufallsfolgen. Keine Treffer für アネット, バーキン, リンダ, ブライアン, バイク (außer Zufall in BSS), オートバイ, ライダー, 学生, 大学, 生存者, 生き残り, あの子, かのじょ.

| Zeile | Datei | Bezug | Urteil |
|---|---|---|---|
| **306 #9** いつの間にか、カギがはずされている | nur Leon | Echo von Sherrys 中から外すわ, Rückbezug auf Elzas Gruppe | **Ja**, stärkste Zeile |
| **309 #0** 電気はすでについている | Leon fest, Elza erst nach dem Schalter | すでに setzt Vorgänger voraus | **Ja** |
| 11B #20 つぎに女を降ろせ | Leon | Ada: EM 1 in sub15, sub24, A 2:18.1 zeigt Leon, Marvin, Frau im Weiß | **Kein Bezug auf Elza** |
| 300 #5, #7 あの女 | Leon | Ada, in #6 namentlich | Kein Bezug |
| 30E #2 あの男 | Leon | Marvin, #3 nennt ihn | Kein Bezug |
| 11B #10, 11C #2 Lärm von oben | Leon | #11 erklärt ihn mit dem Rolltor. Verbindung zu 117 („runterstoßen“) nicht belegbar | Unklar |
| 403 #0 誰かが作動させたらしく | beide | Gleicher Text in beiden Dateien, also nicht der jeweils andere | Unbestimmt |
| 501 #38, #40, 506 #1 hastig durchwühlt, bis vor Kurzem benutzt | beide | Klingt nach Flucht des Personals | Unbestimmt |
| 102 #5, #6 私一人じゃ, 残った私たち | Elza | Elza kennt keine weiteren Überlebenden | Schwaches Indiz, dass sie Leon noch nicht kennt |
| 401 #16 gegen 501 #35 | Leon / Elza | 2号シェルター setzt mehrere Schutzräume voraus | Getrennte Ziele möglich, nicht belegt |
| 514 #0 開いたわ この奥よ | beide, nie aufgerufen | Abschnitt 3 | Offen |

**Name auf der Disc:** Bestätigt. Die einzigen Aufrufe in Leon-Dateien mit Elza-Seite sind 4030 #5, #8, beide im toten Zweig. In Elza-Dateien keiner mit Leon-Seite.

## 6. Videos: was im Bericht fehlt

### 6.1 A 0:12.83–0:13.63: 「……レオン」

- **Text:** Eine Punktreihe (Anzahl nicht auszählbar), dann レオン. Vergleich mit einer aus TEX.TIM gesetzten Zeile: `pruefung/texte/a013_vergleich.png`.
- **Dauer:** 24 Bilder, Schnitte bei 12.83 und 13.63 s (Bilddifferenz 85 und 76).
- **Ort:** Disc-Hintergrund `bg40/ROOM11507.ppm` zeigt dieselbe Einstellung: Wand oben links, Sofa diagonal, Teppich unten rechts (`a013_kamera.jpg`). Das Skript beginnt die Szene mit `Cut_chg 7` (1151 @0xE2E). Die Folgeeinstellung 0:13.75 ist Einstellung 1 mit Stadtmodell.
- **Vordergrund:** Rotes Oberteil, weißer Rumpf wie Elza bei 0:46.8. Die Folgeeinstellung zeigt dieselbe Figur. Leon trägt in allen Bildern dunkle Uniform.
- **Auf dem Sofa:** Rosa Kopfhaut mit grauer Partie, dunkle Kleidung (`a013_sofa.jpg`). Nicht die Figur mit vollem rotbraunem Haar. Die Leon-Lesart scheidet damit aus, soweit das Bild reicht.
- **Project Umbrella:** „In Elza's scenario, the Japanese subtitles read,“ ELZA 「・・・レオン」, direkt nach den Sätzen im Chefbüro.

**Wer spricht, ist unklar.** Project Umbrella schreibt Elza. Die Disc hat in dieser Szene genau eine Zeile, MSG#0, und die Figur auf dem Sofa bekommt direkt davor `Plc_motion` (1151 @0xE5E, Message_on @0xE66). Wenn der Stand von 80% nur den Text getauscht hat, spricht der Verletzte. Dann würde er Elza gegenüber Leon nennen, und Elza müsste Leon nicht kennen.

**Folgen:** §11 ist falsch, §3 und §5 sind für 80% unvollständig (Chefbüro bei Elza ist nicht „derselbe Satz“). Die Aussage „Auf der 40%-Disc nicht einmal das“ bleibt richtig. Für den Stand von 80% gilt aber: Leons Name fällt im Elza-Szenario mindestens einmal.

Das berührt auch die Reihenfolge in §1. Spricht der Verletzte, ist Leon aus seiner Sicht schon im Spiel, bevor Elza kommt. Dazu kommt die Garage: Leon sieht Sherry in 11B0 allein laufen, von derselben Stelle wie in 11B1. „Alle drei Weltspuren zeigen in dieselbe Richtung“ stimmt für 306, 309, 121, die Gesamtaussage „Elzas Gruppe ist zuerst da“ ist damit aber nicht gesichert.

### 6.2 Project Umbrella ist keine andere Fassung

Die Seite überschreibt die Liste „BIOFLAMES VERSION & PLAYSTATION MUSEUM VERSION“. Video A trägt durchgehend „WWW.BIOFLAMES.COM“, B teilweise ebenso. Wortgleich mit unseren Videos sind unter anderem:

| Project Umbrella | Video |
|---|---|
| おい、冗談だろ？ | B 0:19 |
| ・・・クソッなんてこった | B 0:21 |
| ・・・レオン | A 0:13 |
| XX 内に女の子がいるの / 戻らなきゃ | A 0:25.9 |
| ・・・まあ、大きな時計ね！ | A 0:29.7 |
| 電げんがある | A 0:41 |
| 間一ぱつってヤツだな！ | A 0:47.5 |
| システムエラー | A 0:52 |
| そうだ！部長にこのことを報告しないと | A 1:06.7 |
| みんなゾンビにくわれたか… | A 1:07.6 |
| 会わなかったのか？ | A 1:42 |
| 上のさわぎは何だ？ | A 2:15.5 |
| 地下に逃げるってのはどうだ？ | A 2:16 |
| 下に何かいるかも知れないし / こんなX があるだろ？ | A 2:18.1 |

Die Seite enthält auch Zeilen, die ich in A und B nicht gefunden habe (zum Beispiel マーヴィン！ これは...), also vermutlich noch weitere Aufnahmen. In ihrer Liste japanischer Zeilen kommen リンダ und ブライアン nicht vor, „Linda“ und „Brian“ sind Etiketten der Seite. Die Formulierung „in einer anderen Build-Fassung“ in §5 trägt das nicht. Einschränkung: Die Seite habe ich über einen Zusammenfassungs-Abruf gelesen, die Wortlaute sind damit Zweitquelle. Der Primärbeleg ist unser Video.

### 6.3 Sherry als Spielfigur

A 0:40.2–0:44.2: Kleine blonde Figur mit hellem Oberteil und dunklem Rock läuft allein durch mehrere Räume (`fa2.jpg`, besonders 0:42.80 an der Treppe). 0:41.0: 「電げんがある」, zweite Zeile vom Wasserzeichen verdeckt. Project Umbrella: „(Sherry and crate event)“. Dadurch ist BERICHT §7 für den Stand von 80% direkt belegt, im Bericht stand das nur aus dem Skript.

### 6.4 Fehlende Einträge für BERICHT §10

| Stelle | Inhalt | Disc |
|---|---|---|
| B 0:19–0:23 | Leon auf dem Dach: おい 冗談だろ？ / …クソッなんてこった | Nicht auf der Disc. §5 sagt zum Dach: „Kein Text (Video: Leon auf dem Dach)“ |
| A 0:25.9 | Elza, John: (まだ?) □内に女の子がいるの / 戻らなきゃ | 10B #0, #2 umformuliert |
| A 0:29.7 | …まあ、大きな時計ね！ | Nicht auf der Disc. EXE-Item #63 かいちゅうどけい |
| A 0:38.8 | …われてないはずだ | 300 #12 mit ない statt ねぇ |
| A 0:41 | Sherry: 電げんがある | Nicht auf der Disc |
| A 1:00.2 | Elza am Kasten: エレベーターを… | Vgl. 307 #15, Rest verdeckt |
| A 1:34.7 | ロケット… | Vermutlich Item, unlesbar |
| A 1:52.2 | Leon am selben Kasten: …ールに… | Vgl. 307 #15, unlesbar |
| A 2:18.1 | Leon, Marvin, Frau im Weiß: 下に何かいるかも知れないしー / こんな□ があるだろ？ | 11B #17 ersetzt, der Bericht nennt das „umformuliert“ |
| B 0:54 | スイッチがある おしますか？ Yes No | 119 #0 |
| B 6:48 | ボタンがある 押しますか？ Yes No | 104 #0 |
| B 7:00 | 画面は開発中のものです。 | Einblendung |

Kleinere Korrekturen in §10: A 0:47.4 ist nicht „gleich“ (2.4). A 2:15.5 ist nicht „gleich“: 上のさわぎは何だ？ gegen あの上の騒ぎはなんだ？ A 1:24: Project Umbrella liest マーヴィン00 2, das wäre eher ein Debug-Etikett als „マーヴィンの…“.

**Vorsicht beim Wort „Kittel“:** Die Frau in A 1:24, 1:42, 2:18 trägt ebenfalls einen weißen Kittel und spricht bei 1:42 Adas Zeile 11C #3. Sie hat dunkles Haar. Die Frau bei A 1:57 wirkt hell. Der Kittel allein trennt die beiden also nicht, in §3 und §5 sollte das Merkmal Haar genannt werden.

## 7. Bestätigt

- Raster, Steuercodes, 291 Zeilen, 573 Dateieinträge.
- Keine erreichbare Zeile der Disc nennt die andere Seite.
- 25 nicht ausgelöste Zeilen.
- Adressen in §3, §4, §7, §8: 11B0 @0x107E, @0x10B6, @0x11D2; 3060 @0xEFE, @0xF9A; 3090 @0x1412, @0x1416; 3091 @0x1464, @0x146E; 3000 @0x161A; 4030 @0x528, @0x52C.
- 1150 und 1151: SCD byte-gleich (844 Byte), trotz verschiedener Dateien.
- 401 #18 und 501 #43 wortgleich.
- 11B #15 マーヴイン mit großem イ ist echt.

## 8. Offen

1. Sprecher von 「……レオン」.
2. 地仲 in 307 #2, 奥の東東 in 30E #12.
3. Sprecher 307 #4, 308 #1, 514 #0.
4. Mechanismus der Ersatzzeichen.
5. 09:27 und 0B:1E.

## 9. Grenzen

Videos sind Filmaufnahmen eines Bildschirms, Untertitel teils vom Wasserzeichen verdeckt. Video A habe ich in 0,5-s-Schritten durchgesehen, B in 1-s-Schritten, jeweils nur die Textzone. Einstellungen unter 0,5 s können fehlen. Bildzuordnungen zu Figuren bleiben Sichtbefund. Sprachgefühl-Urteile zu Register und Lesarten sind als solche gekennzeichnet.

## 10. Dateien

Alle unter `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/texte/`:

- `a013_vergleich.png`, `a013_kamera.jpg`, `a013_sofa.jpg`: Beleg 「……レオン」
- `fa2.jpg`: Sherry allein
- `fa3.jpg`, `fa4.jpg`: weitere Stellen
- `z_a.png`, `z_b.png`, `z_kana.png`, `z_kand_0.png`, `z_kand_1.png`, `b09.png`, `b0A_1.png` bis `b0A_3.png`, `b0B.png`: Glyphen
- `ocr.json`: Vorlagenabgleich
- `scd/ROOM*.txt`, `regie_*.txt`: Skripte, Regie
- `tx_*.py`: Werkzeuge
- `code0A.txt`: alle Fundstellen der 0A-Codes
