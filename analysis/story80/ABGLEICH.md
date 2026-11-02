# Abgleich: Port-Story gegen 80%-Material und 40%-Disc (Spur ST80-A)

Stand 2026-11-02, Zweig `r41/story80`. Nur gelesen, gemessen und zusammengeführt, kein Code und keine Datei unter `synchro/`,
`pri/`, `Version_40%/`, `Version_80%/` geändert. Grundlage: `KATALOG_80.md` (K001–K101, T01–T34) und `PORT_ZENSUS.md`.
Selbst nachgeprüft: Disc-Zeilen direkt aus den RDT-Bytes (`abgleich/disc_zeilen_geprueft.txt`), ob die Disc sie ruft
(`zensus/disc_zensus.json` und die Schreiber der Bedingungs-Flags), 62 Sprachdateien per Spracherkennung mit zwei Modellen
(`abgleich/spracherkennung.json`), die Text-Id-Kollision im Code. Die 80%-Spalten des Zensus sind gegen den Katalog
nachgezogen: `abgleich/zensus_80_nachgezogen.md`.

**Ihr Maßstab.** Die Räume bleiben, auch die vom Port erfundenen. Geändert wird nur Story, die **nachweislich und gravierend**
von Capcoms Plan abweicht. Ein Beleg ist das 80%-Material oder eine Disc-Zeile, die die Disc tatsächlich aufruft.
Platzhalter-Figuren der Disc und Zeilen in ungerufenen Subs zählen allein nicht. Englischer Mod-Text ist nie ein Beleg.
Ihre Lückenfüller bleiben.

**Bereits entschieden, hier nicht erneut vorgelegt:** Irons ist in 1151 bei Elza von Anfang an tot (Runde 36). Das Ende: Die Frau
im Kittel („Annette“) wechselt nach Bahnsteig-Boss, Generator und Tor von 603 in den hinteren Wagen 5090, dort tötet William in
Riesenform sie, dann Riesenform-Kampf, dann G5, das Ende danach schreiben Sie (Spur AN). Orte: siehe (d).

## Kurzfassung

1. **Zwei Entscheidungspunkte** (a): 30E1 „John wurde gebissen“ (Fehllesung, die Disc sagt „umstellt“) und 50B0 „Ada hat den
   Reaktor gesprengt“ (der Port nennt bei Elza mit Capcoms Wortlaut eine andere Ursache).
2. **Elf Sprachdateien sprechen noch den alten Mod-Text**, nicht die Untertitel der Runde-36-Umschreibung. Darunter
   `room30C1/main03.wav`: Elza sagt hörbar „We got separated back at the RPD“, also das Gegenteil Ihrer Entscheidung „Leon und
   Elza sind sich nie begegnet“. Das braucht keine neue Entscheidung, nur Neuaufnahmen (f.1).
3. **Capcoms sechs englische Sätze** (Dach, Hinterhof) stehen im Port-Text und in Ihren Aufnahmen im selben Wortlaut. Es fehlt
   nur das Wort „Look,“ in 3061 #5 (c.1).
4. Kein Port-Element widerspricht einem 80%-Bild nachweislich. Das Material stützt viel (c).
5. Technisch: Text-Id 20 in ROOM1151 ist doppelt belegt (f.2). Dazu Dateinamen, die nicht der Message-Id folgen (f.3).

## (a) Entscheidungsliste

### A1 · 30E1 (Elza), Halle hinter dem Gitterzaun: John „gebissen“

| | |
|---|---|
| Port heute | #2 „John: This is bad... one of those monsters bit me as I tried to get away!“ · #3 „John: ...And there doesn't seem to be a place with any medical supplies here...“. Die Aufnahmen sprechen genau das. |
| Beleg Disc | D30E #12, `ROOM30E1.RDT @0x0067D`, **gerufen**: sub02 @0x358 `Message_on 12`, sub02 aus sub00 @0x302 bei Ck(3,1)=0 und Ck(3,4)=1, Schreiber `ROOM3071 sub01 @0x3236` `22 03 04 01` Set(3,4)=1. Text: 「まずいぜ…東りを化け物どもに東まれちまった！…東生 東れられそうな所なんか ありゃしねえ…」. 東 ist das Ersatzzeichen der Disc. Lesart: 周りを化け物どもに**囲まれ**ちまった, 畜生 **隠れ**られそうな所なんか ありゃしねえ. Deutsch: „Schlecht… die Bestien haben uns **umstellt**! …Verdammt, nirgends ein **Versteck**…“. Zur Grammatik: Objekt ist 周り („das Umfeld“). Das passt nur zu 囲む (umstellen). 噛む (beißen) bräuchte einen Körperteil. Die Mod las 噛まれ und erfand daraus die Medikamente. |
| Beleg 80 % | T26 (A#059, Raum 309, gleich D309 #9, gerufen): John 「…もう少しでケツにくいつかれてた所だぜ」 „Fast hätten sie mich in den Hintern gebissen.“ John entkommt knapp. |
| Urteil | Widerspruch **ja**. Gravierend **ja**: Ein Biss heißt in dieser Welt Ansteckung, der Port zeigt das selbst an Roy (1011/1021). Danach folgt im Port nichts, John bleibt bis zum Zug gesund (keine weitere Zeile in STAGE 4–6 nennt den Biss). |
| Vorschlag | #2 „John: This is bad... those monsters have us surrounded!“ · #3 „John: ...Damn it, there's no place around here to hide...“ (#1 und #4 bleiben) |
| Sprachdateien | `synchro/STAGE3/room30E1/main02.wav`, `synchro/STAGE3/room30E1/main03.wav` |
| Im selben Zug | Wenn Sie hier neu aufnehmen, lohnt sich B1 (#14–#17, Rollen) in derselben Szene. |

### A2 · 50B0 (Leon), Maschinenhalle: Wer hat den Reaktor zerstört?

| | |
|---|---|
| Port heute | #10 „Leon: Ada, why did you set the reactor to blow up?“ · #11 „Ada: So I can destroy this place. You've seen what's in here.“ · #12 „Ada: You've seen what they can do. I had to do it, understand? It's...“ (Mod-Text, die Aufnahmen sprechen ihn so). |
| Beleg | Capcoms Text D501 #29/#30, `ROOM5011.RDT @0x00B23` und `@0x00B43`: 「怪物がこのしせつの動力を破壊したのよ」 „Das Monster hat die Energieversorgung der Anlage zerstört.“ 「ばく発でウィルスがばらまかれるわ」 „Bei der Explosion wird das Virus freigesetzt.“ **Auf der Disc ungerufen** (ROOM5011 sub06 hat keinen Aufrufer), nach Ihrem Maßstab allein also kein Beleg. **Aber der Port spielt diese Zeile selbst:** 5011 #29 „Elza: It seems like that beast broke the place's power core.“ (`synchro/STAGE5/room5011/main29.wav`, Spracherkennung gleich). Der Port nennt damit zwei Ursachen für dieselbe Katastrophe: bei Elza das Monster, bei Leon Ada. Zu Adas Auftrag sagt die gerufene Zeile D401 #11 nur 「やらなければならない事がある … これだけは私の手で！」 „Ich habe etwas zu erledigen … das muss ich selbst tun!“, ohne Ziel. 80 %: nichts zur Ursache. |
| Urteil | Widerspruch **ja** (im Port selbst, Capcoms Wortlaut steht auf Elzas Seite). Gravierend **ja** (Ursache des Hauptereignisses). Der Capcom-Beleg ist nur mittelbar, deshalb Ihre Wahl: (1) 50B0 an Capcom angleichen oder (2) so lassen. Bei (2) liest sich der Port so: Ada hat ausgelöst, Elza deutet die Durchsage falsch. |
| Vorschlag zu (1) | #10 „Leon: Ada, what's happening to this place?“ · #11 „Ada: That monster wrecked the power core. When it blows, the virus will spread everywhere.“ (= D501 #29/#30) · #12 „Ada: We have to get to the shelter before it goes. It's...“ (Anklang an Adas eigene Zeile D401 #22 「シェルターへ急ぐぞ！」, im Port 4010 #35) |
| Sprachdateien | `synchro/STAGE5/room50B0/main10.wav`, `main11.wav`, `main12.wav` |

**Ohne neue Entscheidung, aber zum Aufnehmen:** die elf Dateien aus (f.1).

## (b) Kenntnisnahme

Widerspruch ja oder möglich, aber nicht gravierend oder nicht belastbar belegt. Dateien nur, falls Sie angleichen wollen.

| Nr | Stelle | Port | Beleg und Urteil | Dateien |
|---|---|---|---|---|
| B1 | 30E1 #14–#17 (Elza) | Elza drängt hinein, John zögert | D30E #20–#22, gerufen. Die Satzenden bestimmen den Sprecher: #20 「…危ねえ」「…しかないぜ」 = John, #21 「…かもしれないのよ？」 = Elza, #22 「…なるさ…行くぜ！」 = John. Auf der Disc drängt John, Elza zögert. Widerspruch ja, nicht gravierend (gleicher Ausgang) | `STAGE3/room30E1/main14–17.wav` |
| B2 | 30E0 #3 (Leon) | „Marvin … can't get out because of all the monsters“ | D30E #3 @0x004CF, gerufen: 「外の化け物どもに東をやられて動けないんだ」 „Die Bestien haben ihn [am Bein] erwischt, er kann nicht laufen.“ Die Wunde fehlt in Leons Bericht, kommt aber in 30E0 #10 und 4010 #0 („that wound“). Nicht gravierend. Dazu #5 „that tunnel“ (Disc この東度 „hier“) und #10 „How careless“ (Disc 「…東当東いな…」 „ziemlich schlimm“) | `STAGE3/room30E0/main03.wav`, `main05`, `main10` |
| B3 | 5011 (Elza), Impfstoff-Szene | #30 „fire's starting to spread“; #35 „should be open“; Sprecher von #0, #23, #24, #26, #33, #37 | D501 ist auf der Disc **ungerufen**, also kein belastbarer Beleg. Inhaltlich: #30 Virus statt Feuer, #35 „Schutzraum Nr. 2 kann ich öffnen“, #23 「どうしたの？」 ist Sherry beim Aufwachen (#24 reagiert darauf), #0 「しっかりしな」 und #37 「…頼むぜ」 sind John, #26 「…したわよ」 ist Elza. Der Ausgang (Schutzraum) bleibt gleich | `STAGE5/room5011/main30.wav`, `main35`, `main00`, `main23`, `main24`, `main26`, `main33`, `main37` |
| B4 | 3070 #0–#4 (Leon) | Leon befiehlt den Aufzug, Marvin fragt, Ada „Underground…“ | D307 #0–#4, gerufen. Die Sprecher sind auf der Disc nur erschlossen: Ada spricht in 1.5 männlich (D11C #3, D401 #2), die Satzenden trennen sie nicht von Leon und Marvin. Nicht nachweislich. „Underground“ ist eine Fehllesung von 地仲さ (der Disc-Font hat 下, 地下 wäre schreibbar gewesen; Lesart „In die Hölle…“) | `STAGE3/room3070/main00–04.wav` |
| B5 | 3071 #6, #8 (Elza) | „only from the inside“, „What are you talking about?“ | D307 #9 「外部の人間には無理みたいね」 „Für Außenstehende ist das wohl unmöglich“, D307 #11 「あなたって一体…」 „Wer bist du eigentlich…“. Sherrys Insiderwissen wird schwächer, steht aber in 3091 #19 und 4001 #7. Nicht gravierend | `STAGE3/room3071/main06.wav`, `main08` |
| B6 | 4001 #6, #9, #10 (Elza) | John fragt Sherry, John ruft „hold on“, „Don't go alone“ | D400 #12, #15, #16, gerufen (4001 sub10 bei Ck(3,33)=0): 「あなたこんな所まで知っているの？」, 「ちょっとシェリー！」, 「ダメよ一人じゃ！」 = Elza. Nicht gravierend | `STAGE4/room4001/main06.wav`, `main09`, `main10` |
| B7 | 1021 #6, 11C0 #7, 3001 #6, 4031 #5, 3080 #0 | einzelne Sprecherwechsel | Zensus §c, gering | je eine Datei |
| B8 | 30E1 #12 (Elza) | „it got scared and hid“ | D30E #19 「…怖を消して」, 怖 ist Ersatzzeichen für 姿: „verschwand plötzlich“. Gering | `STAGE3/room30E1/main12.wav` |
| B9 | 50B0 Adas Tod | Ada stirbt | Kein belastbarer Beleg dagegen. Ada lebend am Zug steht nur in ungesetztem Disc-Material: ROOM6030 sub00 setzt Ada (0x42) nur bei (4,22)=1 und (4,23)=0, kein Schreiber. D401 #21/#22 (Ada kehrt zurück) sind ungerufen, laufen im Port als 4010 #34/#35 und passen zum späteren Tod. 80 %: K086 zeigt in 50B eine weiße, ortsfeste Form neben der Maschine, ob Figur, ist offen. Widerspruch nein | – |
| B10 | 6030 #11 „Where's that biker girl?“ | die Frau im Kittel kennt Elza | Keine Quelle dafür oder dagegen. Im Port begegnet die Frau Elza nie, das ist eine Lücke in der Logik. Ihre Wahl | `STAGE6/room6030/main11.wav` |
| B11 | 6030 #2, 6031 #1 | die andere Hauptfigur kämpft draußen | Vereinbar: Auf der Disc stehen in 604 bei beiden Figuren nur Marvin (Platzhalter 0x40), John und Sherry (6040/6041 sub00, ohne Bedingung), die andere Hauptfigur fehlt. 80 %: Kein Bild zeigt Elza in Stufe 6. Widerspruch nein | (Ton siehe f.1) |
| B12 | 30C1 #2 | Marvin nennt Elza Leons Namen | Die Begegnung zeigt K074. Leons Name fällt im Elza-Lauf auch in 115 (T04 「……レオン」). Widerspruch nein | (Ton siehe f.1) |
| B13 | 1211 | Elza befreit John aus der Zelle | Disc 1211 ohne John, Tür bei Elza zu (`ROOM1211 @0x1E0E`), Flag (3,98) ohne Schreiber. D102 #4 setzt voraus, dass Elza John schon kennt. Wie sie sich kennenlernen, belegt nichts. Widerspruch nein | – |
| B14 | 6030/6031 | Ada-Zweig am Pult gestrichen | nur an ungesetzten Flags (4,22)/(4,23). Widerspruch nein | – |
| B15 | 3061 #5 | „It's here, I can get inside…“ | Capcom-Ton: „It's here! **Look,** I can get inside through this hole!“ Gering | `STAGE3/room3061/main05.wav` |
| B16 | Namen | Irons, Branagh, Kennedy (außer Ausweis), Annette, Elliot, Dr. Birkin, Virus Inhibitor u. a. | Mod-Namen ohne Capcom-Beleg (Zensus M34). Leon S Kennedy ist belegt (T33) | – |

## (c) Bestätigt durch das 80%-Material

Voll: `abgleich/zensus_80_nachgezogen.md`. Gestützt sind:
- Leon allein auf dem Dach mit Capcoms Sätzen (1170 #6/#7, K027), dazu 1170 #16 „report this to the chief“ nach T07.
- Elza mit dem verletzten Roy (1011, K004). Feuer am Transporter nur bei Leon, bei Elza ausgebrannt (1090/1091, K015/K016).
- John warnt Elza vor dem Dach, rotes Riesenwesen (1141, K022/T02, K023). Elza und John in 10B (K018/T03).
- 10D0 #17 „Has everyone been eaten by zombies? or have they turned into zombies?“ = T27.
- Kanaldeckel-Plan mit Leon, Marvin, Ada in 11B (K034/T10/T11): Die auf der Disc toten Zeilen D11B #10–21 liefen im 80%-Stand.
  Ada und die Gorillas in 11C (K035/T13).
- Trennung im Kanal 200: Ada watet zuerst, Marvin folgt, Leon bleibt (2000, K046). Elza, Sherry und die große Uhr (2001, K043/T14).
- Hinterhof 306 wörtlich mit Capcom-Ton (3061, K065/T19–T22). Spielbare Sherry allein in 309 und 30C (K071, K075).
- Birkin aus dem Aufzug in 307 bei beiden (K068, K069). Wiedersehen in 309 (3091, K072/T26). Elza trifft Marvin in 30C (K074).
- Roter Affe in 408 (K079), Birkin im Hangar 50A (K085), Frau im Kittel und Riesenform in 509 (K084), Leon mit der Frau im
  Kittel am Bahnsteig und am Pult (6000, 6030; K094, K096).
- Grundsatz „Leon und Elza begegnen sich nie“: Kein Bild zeigt beide zusammen.

### c.1 Capcoms englischer Ton gegen Port-Text und Ihre Aufnahmen

Spracherkennung Whisper large-v3-turbo und medium.en, Beam 5, beide gleich bis auf die Schreibung (`abgleich/spracherkennung.json`).

| 80%-Ton | Port-Text | Ihre Aufnahme | Ergebnis |
|---|---|---|---|
| F1 Leon: „Oh, that's just freakin' great!“ | 1170 #6 „Oh, that's just freaking great...“ | `STAGE1/room1170/main06.wav`: „Oh, that's just freakin' great!“ | gleich |
| F1 Leon: „Now what am I gonna do?“ | 1170 #7 gleich | `main07.wav`: gleich | gleich |
| F3 Sherry: „Elza, I know a secret way in!“ | 3061 #3 „Elza... I know a secret way in!“ | `STAGE3/room3061/main03.wav`: gleich | gleich |
| F3 Elza: „A secret entrance?“ | 3061 #4 gleich | `main04.wav`: gleich | gleich |
| F3 Sherry: „It's here! Look, I can get inside through this hole!“ | 3061 #5 „It's here, I can get inside through this hole!“ | `main05.wav`: „It's here! I can get inside through this hole!“ | „Look,“ fehlt (B15) |
| F3 Elza: „Sherry, wait!“ | 3061 #6 gleich | `main06.wav`: gleich | gleich |

## (d) Fehlende Räume und Orte

| Ort im Material | Quelle | Stand |
|---|---|---|
| Raum mit rotem Rad (Leon gegen Riesenspinne) | F2 0:10.6–0:15.6 | Ihre Antwort: Generatorraum 2060, erledigt |
| Tunnel mit oranger Tür, Rippen-Tunnel (Leon gegen Zombies in Khaki) | F2 0:15.6–0:21.1, B 4:05.5 u. a. | Ihre Antwort: 2000/2001, erledigt |
| Bahnsteig-Halle mit rotem Zug (Leon und Riesenform) | A#143, #150 | Ihre Antwort: ≈ Port 5080, erledigt |
| Moosgang (Elza gegen Spinnen) | B 5:41–6:01, Disc 2070 | **offen**, der Vorschlag liegt bei Ihnen |
| Vier Trailer-Orte (grüner Gang mit Gittertor, blauer Gang mit Leiche, blauer Gang mit zwei Türen, Moosgang mit Tafel) | B 9:49–10:05.8 | offen, wohl älterer Stand |
| Raumnummern ohne Datei in Port und Disc | 127, 20C–20F, 30F, 406, 40C–40F, 515–517, **605–607** (EXE: Maschinenraum) | Das Finale existiert nirgends (Spur AN) |

## (e) Fakten und Lücken STAGE 3–6, Grundlage für Ihre Dialoge

„Disc“ = Zeile, die die Disc ruft. „Disc, ungerufen“ = Capcom-Text, der auf der Disc nicht läuft. „80 %“ = Katalog. „Lücke“ =
kein Text, unklarer Anlass oder ein Beleg, den der Port nicht zeigt. L = Leon, E = Elza.

### STAGE 3 (Fabrik)

| Raum | | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 300 | L | Disc D300 #2–8: Leon und Marvin, tote Raccoon-Polizisten, „alles Ermittler im Umbrella-Fall“, Ada ist vorgegangen und „hat bei dem Anblick nicht mal die Miene verzogen“. 80 %: K057/T17 Leon und ein hockender Mann an einer Leiche, 「みんな ラクー…」 | Marvin, wörtlich | Wer der Mann im Foto ist (Kleidung eher John) |
| 300 | E | Disc D300 #9–16: Chemiefabrik, „dürfte stillliegen“, Versteck suchen. 80 %: K055 Elza liegt am Boden, John daneben; K056 Kampf am Umbrella-Lkw | John, Sherry, fast wörtlich | Warum Elza am Boden liegt |
| 301, 303, 30A | E (301 auch L) | 80 %: nur Kämpfe | Untersuchen | keine Story |
| 302 / 30D | L | 80 %: K060 Leon mit einer zweiten hellen Figur (unsicher). Disc: Marvin als Spielfigur nur vorbereitet | Marvin wird Spielfigur (Mod) | Anlass des Figurenwechsels |
| 302 / 30D | E | 80 %: K059 Elza am Streifenwagen | Untersuchen | keine Story |
| 304 | L/E | 80 %: K062, K063 Gore-Wesen bei beiden | Mod-Texte | keine Story |
| 305 | E | 80 %: K064/T18 Elza am Rechner: „Now Running…“, „Break“, 「システムエラー」 (Systemfehler) | Notiz „Red Master Keycard“ (Mod) | Die Rechnerszene fehlt |
| 306 | L | Disc D306 #9 „Irgendwann ist das Schloss aufgemacht worden…“. 80 %: K067 Leon und Marvin an der orangen Tür | Marvin | – |
| 306 | E | Disc D306 #1–8, 80 % mit Capcom-Ton (K065); danach Kampf gegen Zombie und Hunde (K066) | wörtlich | – |
| 307 | L | Disc D307 #0–4: Aufzug, „In die Hölle…“ (Sprecher erschlossen). 80 %: K069 Leon gegen Birkin in Menschform, Marvin nicht im Bild, T24 「ホール…」 | Ada, Marvin, Birkin | Die Begegnung mit Birkin hat keinen Text (B4) |
| 307 | E | Disc D307 #5–15: Das Monster kam aus dem Aufzug, Sherry kann ihn bedienen. 80 %: K068 Birkin tritt mit Stange aus dem Aufzug, Elza zeitweise am Boden | John, Sherry, Birkin | Sherrys Insiderwissen (B5) |
| 308 | L | Disc D308 #0/#1, Bruchstücke („…hartnäckiger Kerl…“, „…also doch…“) | Ada, Marvin, Birkin | Anlass der Zeilen |
| 309 | Sherry / E | Disc D309 #1–25. 80 %: K071 Sherry allein in fünf Kameras, T25 「電げんがある」 „Da ist eine Stromversorgung“; K072/T26 Wiedersehen, John knapp entkommen | Sherry spielbar, wörtlich, Mod-Schluss | T25 fehlt; D309 #21/#24 fehlen |
| 30C | E | 80 %: K074 Elza trifft Marvin am Tresen (ohne Text); K075 Sherry allein | Marvin-Szene (Mod, Runde 36 umgeschrieben) | Was Elza und Marvin sich sagen |
| 30C | L | 80 %: K076 Leon am Tresen | Schloss-Text | Leon in 30C ohne Story |
| 30E | L | Disc D30E #0–10: Ada taucht auf, Marvin am Bein verletzt in der Halle, der Haupteingang fällt, „oben ist es vorbei“ | Ada, Marvin | B2 |
| 30E | E | Disc D30E #11–23: Elza sah etwas, „kein Mensch“, umstellt, John drängt hinein. 80 %: K077 | John, Sherry | A1, B1, B8 |

### STAGE 4 (Labor)

| Raum | | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 400 | L | Disc D400 #0–4: Tür mit ID-Karte, „Ada, was ist das hier“. Disc, ungerufen: #5–8 Leon hält auf, „Kümmere dich um Marvin“. 80 %: K078 nur ferne Figur | D400 ganz, Gorilla (Mod) | – |
| 400 | E | Disc D400 #9–18: Papas Labor, Sherry läuft los | Sprecher Elza→John (B6) | – |
| 401 | L | Disc D401 #0–18: Wunde, „Ich gehöre zu Umbrella“, Impfstoff aus dem Virusstamm, Ada geht, Schutzraum. Disc, ungerufen: #19–22 Marvin bricht zusammen, Ada kehrt zurück | alles, im Port laufen auch #19–22 | – |
| 403 | E | Disc D403 #5–8: Sherry verschwunden, aufteilen | #5 John statt Elza | gering |
| 408 | E | 80 %: K079 ein dunkelrotes, affenartiges Wesen springt Elza an | roter Affe | Anlass ohne Text |
| 402, 404, 405, 407, 409, 40A, 40B | | – | Mod-Texte | keine Story |

### STAGE 5 (Tiefes Labor)

| Raum | | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| 501 | E | Disc, ungerufen: D501 #0–37 Sherry vom Virus zerfressen, Elza holt Impfstoff, Sherry erwacht, Durchsage, Monster zerstörte die Energie, Explosion setzt Virus frei, Schutzraum Nr. 2 | Szene läuft, #30 Feuer, Sprecher getauscht | A2, B3 |
| 501 | L | 80 %: K080 Leon | Untersuchen | Leons Weg ohne Story |
| 502, 503 | E | 80 %: K081 (503 nur eine Figur) | Sherry-Kurzszenen (Mod) | Anlass |
| 504, 506, 512, 514 | L/E | 80 %: K082 kriechende Gestalten in 504; K083 Licht wechselt in 506 auf Rot; K089, K091 brennende Zombies; K090 Leon in 514. Disc, nie ausgelöst: D514 #0 Frauenstimme „Es ist offen. Da hinten.“ | Untersuchen | Wer ist die Frau in 514? Was löst das Rotlicht in 506 aus? |
| 508 | L/E | – | Generator, Birkin-Kampf (Mod) | Anlass |
| 509 | L | 80 %: K084 Frau im Kittel und Riesenform im selben Bild, Feuer, kleine Figur an einer Konsole | Endkampf (entschieden) | Szene ohne Text (Spur AN) |
| 509 | E | – | Endkampf ohne die Frau | Elzas Endkampf ohne Beleg |
| 50A | | 80 %: K085 Birkin packt ein gelb-schwarzes Wesen | wie im Film | ohne Text |
| 50B | L | 80 %: K086 Leon, weiße Form neben der Maschine | Adas Tod (Mod) | A2, B9 |
| 50B | E | – | „It's a woman. She's dead.“ | Wo Ada Elza begegnet (Kamiya: kurz), offen |
| 50C | L | 80 %: K087 Leon läuft durch | – | – |
| 50E | L | 80 %: K088 eine große Kreatur fällt herab, Leon flieht | Mod-Birkin | ohne Text |

### STAGE 6 (Bahnsteig und Zug)

| Raum | | Belegt | Port heute | Lücke |
|---|---|---|---|---|
| Bahnsteig-Halle (≈ 5080) | L | 80 %: K093 Leon und die Riesenform, roter Zug | entschieden | – |
| 600 | L | 80 %: K094 Leon und die Frau im Kittel | „Annette“, Tunneltor, Zug fährt | Dialog, Name |
| 600 | E | – | 3 Untersuchen | Elzas Weg zum Zug |
| 601 | L | 80 %: K095 Leon allein | Untersuchen | – |
| 602 | L/E | Disc D401 #16: Schutzraum (gerufen); D501 #35 (ungerufen) | Begleiter im Schutzraum | Anlass ohne Text |
| 603 | L | Disc: Frau (0x4D) am Pult, Ada-Zweig an ungesetzten Flags. 80 %: K096 Frau im Kittel am Pult | „Annette“ startet den Zug | Dialog |
| 603 | E | Disc: Sherry (0x4B) am Pult | Sherry am Pult | Dialog |
| 604 | L | Disc: Marvin (Platzhalter), John, Sherry (6040 sub00). Leon geht vier Wegpunkte (sub06), Blick und John läuft los (sub04, ungerufen). 80 %: K097 ohne Aussage | Gruppe in 6030 | Was Leon und die Gruppe sagen |
| 604 | E | Disc: Marvin an der Klappliege, John, Sherry (6041) | Gruppe in 6031 | Marvins Zustand, Gespräch |
| 605–607 | L/E | nur EXE-Name DOURYOKUSHITSU | – | Finale (Spur AN) |

**Querschnitt.** Kein Bild zeigt Leon und Elza zusammen, keines Elza in Stufe 6. Sherry und Elza nennen sich beim Namen (T19,
T22, mit Ton). Elza und Marvin begegnen sich in 30C ohne überlieferten Text. Die Frau im Kittel ist ab 509 bei Leon, ein Name
fällt nirgends. Birkin: Menschform in 307 bei beiden, im Hangar 50A, Riesenform am Bahnsteig und in 509. Sherry handelt allein:
Gitter in 306, dann allein in 309 und 30C. Trailer: „Zwei Monate nach jener Tragödie begann in ganz Raccoon City der wahre
Albtraum“ (T29–T31).

## (f) Technische Fehler (nicht behoben)

### f.1 Elf Sprachdateien sprechen den alten Mod-Text

Die Runde-36-Umschreibung (`re15_port/engine/src/gen/story_r36_texte.inc`) ändert die Untertitel. Die Aufnahmen (alle elf mit
Dateidatum 30.10., also nach der Umschreibung vom 11./12.10.) sprechen den alten Wortlaut. Beide Spracherkenner sind sich einig.

| Datei | Untertitel heute | Aufnahme spricht |
|---|---|---|
| `synchro/STAGE3/room30C1/main03.wav` | Elza: Leon? I don't know him. I never ran into him at the RPD. | Leon! How is he? We got separated back at the RPD. |
| `synchro/STAGE3/room30C1/main04.wav` | Marvin: He's a rookie, and he's helping me and Ada escape the city. | He's alright, and he's helping me and Ada escape the city. |
| `synchro/STAGE3/room30C1/main07.wav` | Elza: Will you tell them that me and John and Sherry are here … | Will you tell him that me and John and Sherry are here … |
| `synchro/STAGE6/room6030/main01.wav` | Leon: Is anyone else with you? | Where's Elza? |
| `synchro/STAGE6/room6030/main02.wav` | Sherry: Elza is out there fighting the monsters! | She's out there fighting the monsters! |
| `synchro/STAGE6/room6030/main12.wav` | Leon: I've never met her. Can you start this thing? | Keeping the monsters at bay. Can you start this thing? |
| `synchro/STAGE6/room6031/main00.wav` | Elza: Sherry! Is anyone else here? | Where's Leon? |
| `synchro/STAGE6/room6031/main01.wav` | Sherry: A policeman! He's fighting off monsters. He wants us to start the train. | He's fighting off monsters. He wants you to start the train. |
| `synchro/STAGE5/room50B0/main01.wav` | Ada: Some other survivors are down there with her too. | Your other friends are down there with her too. |
| `synchro/STAGE6/room6000/main00.wav` | Annette: You must be that cop. Your friend is already here. | You must be that cop. Your friends are already here. |
| `synchro/STAGE6/room6000/main03.wav` | Annette: Come on. Let's join the others and get out of here. | Come on, let's join your friends and get out of here. |

Folge: Hörbar kennen sich Leon und Elza (30C1 main03, 6030 main01, 6031 main00). Die Untertitel sagen das Gegenteil.

### f.2 Text-Id 20 in ROOM1151 doppelt belegt (nur gelesen)

- `elza_r36.c:272` legt „He has already passed...“ als Id 20 ab (`RE15_ELZA_IRONS_MSG_ID`, `re15_elza_r36.h:58`). Nach der
  Hinbeuge-Szene (Flag (3,0x9D), ROOM1151 sub01 @0x00EA8) wird Irons' Platz zu einem einfachen Untersuchen-Text mit Id 20
  (`elza_r36.c:275-279`).
- `hebetisch_cursor_1150.c:314` schreibt bei jedem Fehlklick am Hebetisch „Nothing happened.“ unter dieselbe Id 20
  (`RE15_HC_TEXT_ID`, `re15_hebetisch_cursor.h:183`). Der Cursor gilt auch in 1151 (`RE15_HC_RAUM_ELZA`, `:148`).
- Der Untersuchen-Weg (`scd_vm.c` `re15_scd_show_message`) liest die Tabelle, ohne neu einzusetzen. Folge: Nach einem
  Fehlklick am Hebetisch zeigt Irons im selben Raumbesuch „Nothing happened.“. Vor der Hinbeuge-Szene tritt der Fehler nicht
  auf, weil `kleinkram_r38.c:140-146` den Irons-Text unmittelbar vor dem Zeigen neu setzt. Beim Wiederbetreten ist er wieder
  richtig. Ton ist nicht betroffen (der Untersuchen-Weg hat keine Stimme, `scd_vm.c:1640-1642`).
- Nicht zur Laufzeit gemessen. Messweg: 1151 nach der Hinbeuge-Szene, Hebetisch daneben klicken, Irons anklicken,
  `RE15_MSG_LOG=1`.

### f.3 Dateinamen, die nicht der Message-Id folgen

Wer diese Zeilen neu spricht, muss die Datei rechts ersetzen. Der Port schaltet die gleichnamige Id dafür stumm
(`stimmen_r39.c:67-74`), doppelt gespielt wird also nichts.

| Raum, Id | Datei | Quelle |
|---|---|---|
| 1050 #22–24 | `room1050/main09–11.wav` | `stimmen_r39.c` |
| 1150 #22–29 | `room1150/main15–22.wav` (z. B. #27 = `main20.wav`, Spracherkennung bestätigt alle acht) | `stimmen_r39.c` |
| 11B0 #20/#21 | `room11B0/main13.wav`, `main14.wav` | `stimmen_r39.c` |
| 1171 #3 | `room1171/main02.wav` | `stimmen_r39.c` |
| 1011 #24 | `room1011/main16.5.wav` | `elza_r36.c:226` |
| 1141 #53–57, #59 (auch 1041, 1131) | `room1141/help01–05.wav`, `door_sound.wav` | `dining_r39.c:550-555` |
| 1100/1101 Rekorder | `room1100/Irons_Rekorder_Sound.wav`, `Irons_Rekorder_Sound2.wav` | `rekorder_r38` |
| 1241 #0–5 | `room1240/main00–05.wav`, solange `room1241/` fehlt | `re15_elzaintro_r38.h` |

### f.4 Dateien ohne Zeile

`synchro/STAGE1/room1141/main42.wav` und `main50.wav`: ROOM1141 hat keine Id 42 und 50, die Dateien spielen nie.

## (g) Offene Stellen

| Stelle | Offen | Nächster Weg |
|---|---|---|
| A2 | Ihre Wahl: Ursache an Capcom angleichen oder Ada als Auslöserin behalten | Ihre Entscheidung |
| B10 | Woher kennt die Frau im Kittel Elza („biker girl“)? | Ihre Entscheidung |
| Moosgang | Wohin damit (Disc 2070) | Ihr Vorschlag liegt vor |
| T04 「……レオン」 (115, Elza) | Sprecher: Elza oder der Liegende | keine Tonspur |
| T05 Japanisch, T03/T07/T12/T15/T17/T24–T27 teils | unlesbar | bessere Bildquelle |
| K086 | weiße Form in 50B: Figur oder Bildteil (Ada trägt Weiß) | bessere Filmquelle |
| K057 | Mann neben Leon im Fabrikhof | – |
| Frau im Kittel | Name nirgends belegt („Annette“ ist Mod) | – |
| D514 #0 | Frauenstimme „Es ist offen. Da hinten.“, Sprecherin | – |
| 3070 #0–#4 | Disc-Sprecher nur erschlossen | Skript-Regie der Disc (Kamera/Bewegung je Zeile) |
| f.2 | Laufzeitmessung der Id-20-Kollision | Messweg oben |
| Finale 605–607 | fehlt überall | Spur AN |
| Andere Zweige | r41_g5auftritt, r41_stage3szenen_a, r41_annette nicht erfasst | nach dem Merge erneut abgleichen |

## Nachprüfen

- `abgleich/disc_zeilen_geprueft.txt` mit `abgleich/werkzeuge/disc_zeilen.py`: Disc-Zeilen mit Byte-Offset und Rohbytes, Schreiber
  der Flags (3,0..5).
- `abgleich/spracherkennung.json` mit `abgleich/werkzeuge/asr_synchro.py`: 62 Dateien, zwei Modelle.
- `abgleich/zensus_80_nachgezogen.md`: die 80%-Spalten des Zensus, Element für Element gegen K/T.
