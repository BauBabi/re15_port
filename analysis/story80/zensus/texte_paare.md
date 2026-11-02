# Textpaare Port (Mod-Baum) gegen 40%-Disc — Arbeitsgrundlage

Erzeugt von texte_paare.py. LEBT/TOT = statische Erreichbarkeit des aufrufenden Subs (scd_zensus.py).

## A. Raeume mit Text auf der 40%-Disc

### Raum 102 — Büro mit Schreibtischen und Blutspuren
Disc (TEXTE_40.md):
  - D102 #0 [Elza | John] / そんな まさか…ロイ ウソだろ？  =  Nein, das kann nicht… Roy, das ist doch nicht wahr?
  - D102 #1 [Elza | Roy] ジョン…助けて… / 俺を 殺してくれ  =  John… hilf mir… töte mich.
  - D102 #2 [Elza | Elza] ジョン はやく撃って / それはもう ロイじゃないのよ！！  =  John, schieß schnell! Das ist nicht mehr Roy!!
  - D102 #3 [Elza | John] / ロイ 俺だ… 分かるだろ？  =  Roy, ich bin es… du erkennst mich doch?
  - D102 #4 [Elza | Elza] ジョン 通信で助けはよべなかったわ / ビルの電ゲンが落ちてしまって…  =  John, über Funk konnte ich keine Hilfe rufen. Der Strom im Gebäude ist ausgefallen…
  - D102 #5 [Elza | Elza] 私一人じゃどうすればいいのか / 分からないのよ  =  Allein weiß ich nicht, was ich tun soll.
  - D102 #6 [Elza | Elza] 残った私たちで何とかしないと / このままじゃ私たちだって…  =  Wir, die noch übrig sind, müssen etwas unternehmen. Sonst werden wir auch noch…
  - D102 #7 [Elza | John] 頼む… 一人に… / …してくれ…  =  Bitte… lass mich… allein…
Port 1020 (Leon):
  - 1020 #0: "It's the RPD officer, Roy.He appears to have died recently."  [LEBT sub02@0x1F86 Message_on]
  - 1020 #1: "There are only reports among / other office objects.Nothing that can be of help."  [LEBT main00@0x1ED2 AOT7-sce1; LEBT main00@0x1EE6 AOT8-sce1]
  - 1020 #2: "It's empty."  [LEBT main00@0x1EFA AOT9-sce1]
  - 1020 #3: "It's Lieutenant Branagh's desk."  [LEBT main00@0x1F0E AOT10-sce1]
  - 1020 #4: "There is something saved here."  [kein Aufruf]
  - 1020 #5: "There are only reports of various / cases."  [LEBT main00@0x1F22 AOT11-sce1; LEBT main00@0x1F36 AOT12-sce1]
Port 1021 (Elza):
  - 1021 #0: "John: No way, this can't be... Roy, this / is a joke, right?"  [LEBT sub02@0x2000 Message_on]
  - 1021 #1: "Roy: John... Help me... Please kill me."  [LEBT sub02@0x2038 Message_on]
  - 1021 #2: "Elza: John, shoot! That thing isn't / Roy anymore!"  [LEBT sub02@0x205E Message_on]
  - 1021 #3: "John: Roy, it's me... You do recognize / me, right?"  [LEBT sub02@0x2082 Message_on]
  - 1021 #4: "Elza: John, I couldn't call for help / using the radio."  [LEBT sub02@0x2122 Message_on]
  - 1021 #5: "Elza: The building's electric power / is starting to fail..."  [LEBT sub02@0x213A Message_on]
  - 1021 #6: "John: What am I supposed to do alone? / I don't get it!"  [LEBT sub02@0x215A Message_on]
  - 1021 #7: "Elza: We're the only ones left. If we / don't do anything, we'll also be..."  [LEBT sub02@0x2162 Message_on]
  - 1021 #8: "John: Please... I... I'd like to be / alone now..."  [LEBT sub02@0x21A4 Message_on]
  - 1021 #9: "He's dead.Poor Roy..."  [LEBT sub06@0x2202 Message_on]
  - 1021 #10: "There are only reports among / other office objects.Nothing that can be of help."  [LEBT main00@0x1F30 AOT8-sce1; LEBT main00@0x1F44 AOT9-sce1]
  - 1021 #11: "It's empty."  [LEBT main00@0x1F58 AOT10-sce1]
  - 1021 #12: "It's Lieutenant Branagh's desk."  [LEBT main00@0x1F6C AOT11-sce1]
  - 1021 #13: "There is something saved here."  [kein Aufruf]
  - 1021 #14: "There are only reports of various / cases."  [LEBT main00@0x1F80 AOT12-sce1; LEBT main00@0x1F94 AOT13-sce1]

### Raum 104 — Gang
Disc (TEXTE_40.md):
  - D104 #0 [Leon + Elza | System] ボタンがある / 押しますか？ [Ja/Nein]  =  Da ist ein Knopf. Drücken?
Port 1040 (Leon):
  - 1040 #0: "It's a shutter switch. / Will you push it? "  [LEBT sub02@0x1616 Message_on; LEBT sub07@0x1926 Message_on; LEBT sub08@0x199C Message_on]
  - 1040 #1: "It's an elevator.Pressing the button has no effect.It seems the power isn't turned on."  [LEBT main00@0x107E AOT0-sce1 wenn Ck(4,243)=0]
Port 1041 (Elza):
  - 1041 #0: "It's a shutter switch. / Will you push it? "  [LEBT sub02@0x1640 Message_on; LEBT sub07@0x1950 Message_on; LEBT sub08@0x19C6 Message_on]
  - 1041 #1: "It's an elevator.Pressing the button has no effect.It seems the power isn't turned on."  [LEBT main00@0x107E AOT0-sce1 wenn Ck(4,243)=0]
  - 1041 #2: "Elza: What was that!?"  [LEBT sub11@0x2224 Message_on]
  - 1041 #3: "gunshot"  [LEBT sub11@0x2200 Message_on]

### Raum 109 — Hofbereich mit großem Behälter, verschlossene Tür
Disc (TEXTE_40.md):
  - D109 #0 [Leon | Ada] 誰か！ 誰かここを開けてくれ！  =  Jemand! Jemand soll hier aufmachen!
  - D109 #1 [Leon | Leon] おい！ 中に誰かいるのか？  =  He! Ist da drin jemand?
  - D109 #2 [Leon | Ada] 誰か！ 誰かいないのか！ / いたら開けてくれ！  =  Jemand! Ist denn niemand da! Falls doch, macht auf!
  - D109 #3 [Leon | Leon] さあ！ 大丈夫か？  =  So! Alles in Ordnung?
  - D109 #4 [Leon | Ada] 助かった… / ありがとう を言う  =  Gerettet… Danke, ich danke dir. [Lesart, Lücke 礼]
  - D109 #5 [Leon | Leon] 話は後だな… ここはまずい / それに傷の手当ても必要だろう / 行こう  =  Reden wir später… hier ist es gefährlich. Und die Wunde muss versorgt werden. Gehen wir.
Port 1090 (Leon):
  - 1090 #0: "Woman: Anyone! Can someone please / get me out of here!?"  [LEBT sub02@0x2438 Message_on]
  - 1090 #1: "Leon: Hey! Who's that?!"  [LEBT sub02@0x248A Message_on]
  - 1090 #2: "Woman: Someone, anyone! If out there, / open the door!"  [LEBT sub02@0x24B6 Message_on]
  - 1090 #3: "Leon: There! Are you okay?"  [LEBT sub03@0x2586 Message_on]
  - 1090 #4: "Woman: You saved me... Thank you, / you have my gratitude."  [LEBT sub03@0x25E8 Message_on]
  - 1090 #5: "Leon: We can talk later... It's not safe / here."  [LEBT sub03@0x2640 Message_on]
  - 1090 #6: "Leon: Besides, you need to get that / wound treated, right? Let's go."  [LEBT sub03@0x2670 Message_on]
  - 1090 #7: "I must hurry up and get something to / put out this fire to save that woman!"  [LEBT sub00@0x230E AOT2-sce1 wenn Ck(3,133)=0; LEBT sub06@0x2702 Message_on]
  - 1090 #8: "Will you use the Fire Extinguisher? "  [LEBT sub06@0x2708 Message_on]
  - 1090 #9: "You've used the Fire Extinguisher."  [LEBT sub03@0x2502 Message_on]
Port 1091 (Elza):
  - 1091 #20: PORT-NEU story_r36: "It's a burnt-out shed.Someone must have put out the fire."

### Raum 10B — Zufahrt mit Transporter „RACCOON POLICE“
Disc (TEXTE_40.md):
  - D10B #0 [Elza | Elza] ジョン 待って まだ 内に女の子が一人 / 取り残されているのよ  =  John, warte! Im Revier ist noch ein Mädchen zurückgeblieben. [Lesart]
  - D10B #1 [Elza | John] 何だって？  =  Was?
  - D10B #2 [Elza | Elza] 地下の 度に じ められていたのよ / 置いては行けないわ  =  Sie war in der Tiefgarage eingesperrt. Ich kann sie nicht zurücklassen. [Lesart, Lücken 駐車場, 閉, 込]
  - D10B #3 [Elza | John] 地下 度か… だったらこっちの方が社 だぜ / く来な  =  Die Tiefgarage… dann ist es hier entlang kürzer. Komm schnell. [Lesart]
Port 10B0 (Leon):
  (kein Text)
Port 10B1 (Elza):
  - 10B1 #0: "Elza: John, wait!"  [LEBT sub04@0x199E Message_on]
  - 10B1 #1: "Elza: There's still a little girl / trapped in there!"  [LEBT sub04@0x19B6 Message_on]
  - 10B1 #2: "John: Say what!?"  [LEBT sub04@0x19DA Message_on]
  - 10B1 #3: "Elza: I found her in the basement / garage! We can't just leave her behind!"  [LEBT sub04@0x19FE Message_on]
  - 10B1 #4: "John: The garage in the basement, / huh...?"  [LEBT sub04@0x1A2A Message_on]
  - 10B1 #5: "John: If that's the case, we can go / through the sewer tunnels. Follow me."  [LEBT sub04@0x1A42 Message_on]

### Raum 115 — Büro des Polizeichefs mit Stadtmodell
Disc (TEXTE_40.md):
  - D115 #0 [Leon + Elza | Polizeichef] 私なら大丈夫だ / 今は自分の安全をかんがえろ  =  Mir geht es gut. Denk jetzt an deine eigene Sicherheit.
Port 1150 (Leon):
  - 1150 #0: "There's a small sign in one corner. / It reads:Raccoon City 21st Century, / Hillman-Grayson Architects LLC.""  [LEBT main00@0xDBA AOT4-sce1]
  - 1150 #1: "It's a phone.You can save your progress / with this.Save is not available in / this preview."  [LEBT sub06@0x10EC Message_on]
  - 1150 #2: "Irons: I'll be fine. Just worry about / yourself for now."  [LEBT sub03@0xF1C Message_on]
  - 1150 #3: "Itembox is not available in / this preview."  [LEBT sub07@0x10FA Message_on]
  - 1150 #4: "Leon: Sir... the helicopter is / gone... it was our only way out..."  [LEBT sub08@0x117E Message_on]
  - 1150 #5: "Irons: Calm down Rookie, there / must be a way."  [LEBT sub08@0x1196 Message_on]
  - 1150 #6: "Leon: The building is besieged by / those things, I don't see any option."  [LEBT sub08@0x119E Message_on]
  - 1150 #7: "Irons: Maybe using the communication / system... you can ask for outside help."  [LEBT sub08@0x11B6 Message_on]
  - 1150 #8: "Leon: But the system is damaged."  [LEBT sub08@0x11DE Message_on]
  - 1150 #9: "Irons: I know, I know... but some / officers were trying to repair it, / you may be luckier than them."  [LEBT sub08@0x11FA Message_on]
  - 1150 #10: "Leon: Ok, I'll go see if I can / achieve something with that."  [LEBT sub08@0x1236 Message_on]
  - 1150 #11: "Irons: Take this card... it will allow / you to enter the communications room... / now go..."  [kein Aufruf]
  - 1150 #12: "Irons: And remember this Leon, never / let your guard down... uniform or not,"  [LEBT sub08@0x1252 Message_on]
  - 1150 #13: "those things... are no longer what / they were, don't make my mistake..."  [LEBT sub08@0x1262 Message_on]
  - 1150 #14: "Leon: Yes sir..."  [LEBT sub08@0x127E Message_on]
Port 1151 (Elza):
  - 1151 #0: "There's a small sign in one corner. / It reads:Raccoon City 21st Century, / Hillman-Grayson Architects LLC.""  [LEBT main00@0xDBA AOT4-sce1]
  - 1151 #1: "It's a phone.You can save your progress / with this.Save is not available in / this preview."  [LEBT sub06@0x10CA Message_on]
  - 1151 #2: "Irons: I'll be fine. Just worry about / yourself for now."  [LEBT sub03@0xEFA Message_on]
  - 1151 #3: "Itembox is not available in / this preview."  [LEBT sub07@0x10D8 Message_on]

### Raum 117 — Dach, Hubschrauber-Landeplatz
Disc (TEXTE_40.md):
  - D117 #0 [Elza | John] / エルザ はやくしろ！  =  Elza, beeil dich!
  - D117 #1 [Elza | John] こいつをここからおとすんだ / てつだってくれ！  =  Wir müssen das Ding hier runterstoßen! Hilf mir!
  - D117 #2 [nicht ausgelöst | John] / さあ てつだってくれ！  =  Los, hilf mir!
  - D117 #3 [Elza | John] / さあ ここをおりるんだ！  =  Los, hier runter!
Port 1170 (Leon):
  - 1170 #0: "Elliot: We must go! There is no / time to lose!"  [LEBT sub02@0x146E Message_on]
  - 1170 #1: "Pilot: Is there no one else left / inside?"  [LEBT sub02@0x1486 Message_on]
  - 1170 #2: "Elliot: There is no one left, we / must go!"  [LEBT sub02@0x148E Message_on]
  - 1170 #3: "Elliot: Those things are already / close, they almost caught me!"  [LEBT sub02@0x14A6 Message_on]
  - 1170 #4: "Pilot: Alright get on board we're / off."  [LEBT sub02@0x14C6 Message_on]
  - 1170 #5: "Leon: Hey! Hey wait! Down here!"  [LEBT sub02@0x1534 Message_on]
  - 1170 #6: "Leon: Oh, that's just freaking great..."  [LEBT sub02@0x15DC Message_on]
  - 1170 #7: "Leon: Now what am I gonna do?"  [LEBT sub02@0x15EC Message_on]
  - 1170 #8: "We barricaded ourselves / inside the police station..."  [LEBT sub11@0x16AC Message_on]
  - 1170 #9: "Where we thought it would / be safe."  [LEBT sub11@0x16B4 Message_on]
  - 1170 #10: "It was no use, and so began a / life and death struggle..."  [LEBT sub11@0x16BC Message_on]
  - 1170 #11: "To escape."  [LEBT sub11@0x16C4 Message_on]
  - 1170 #12: "It's locked from the other side."  [LEBT main00@0x1320 AOT5-sce1 wenn Ck(4,195)=0]
  - 1170 #13: "Leon: Damn it, there must be another / way out of here."  [LEBT sub14@0x175E Message_on]
  - 1170 #14: "Leon: If only I could contact the / outside."  [LEBT sub14@0x178E Message_on]
  - 1170 #15: "Leon: ..."  [LEBT sub14@0x179A Message_on]
  - 1170 #16: "Leon: That's right! I have to report / this to the chief."  [LEBT sub14@0x17A2 Message_on]
Port 1171 (Elza):
  - 1171 #0: "John: Elza, hurry up!"  [LEBT sub02@0x14BA Message_on]
  - 1171 #1: "John: We need to get this thing / off of here! Help me out!"  [LEBT sub02@0x151E Message_on]
  - 1171 #2: "John: C'mon, give me a hand!"  [kein Aufruf]
  - 1171 #3: "John: Alright, let's get down from here!"  [LEBT sub02@0x1586 Message_on]
  - 1171 #4: "It's locked from the other side."  [LEBT main00@0x12B6 AOT4-sce1 wenn Ck(4,195)=0]

### Raum 119 — Flur am Schießstand (Bahnen 3/4)
Disc (TEXTE_40.md):
  - D119 #0 [Leon + Elza | System] スイッチがある おしますか？ / [Ja/Nein]  =  Da ist ein Schalter. Drücken?
  - D119 #1 [Leon + Elza | System] ドライバーがある とりますか？ / [Ja/Nein]  =  Da liegt ein Schraubendreher. Nehmen?
Port 1190 (Leon):
  - 1190 #0: "There's a switch here. Push it? "  [LEBT sub02@0x24BA Message_on; LEBT sub03@0x2532 Message_on; LEBT sub04@0x25AA Message_on; LEBT sub05@0x2622 Message_on; LEBT sub12@0x2892 Message_on]
  - 1190 #1: "There's a driver here. Take it? "  [kein Aufruf]
  - 1190 #2: "I have nothing else to do / here."  [LEBT main00@0x227A AOT0-sce1 wenn NICHT[Ck(4,234)=0]; LEBT main00@0x228E AOT1-sce1 wenn NICHT[Ck(4,234)=0]; LEBT main00@0x22A2 AOT2-sce1 wenn NICHT[Ck(4,234)=0]; LEBT main00@0x22B6 AOT3-sce1 wenn NICHT[Ck(4,234)=0]; LEBT sub10@0x27B4 Aot_reset0-sce1; LEBT sub10@0x27BE Aot_reset1-sce1; LEBT sub10@0x27C8 Aot_reset2-sce1; LEBT sub10@0x27D2 Aot_reset3-sce1]
  - 1190 #3: "No response...The power is not supplied."  [LEBT sub12@0x28A0 Message_on wenn Ck(12,31)=0]
  - 1190 #4: "There is one R.P.D. armor / that should fit you.Will you equip it? "  [LEBT sub15@0x2DAE Message_on wenn Ck(3,117)=0]
  - 1190 #5: "Will you unequip the armor? e         /   4      4   /"  [LEBT sub15@0x2DB8 Message_on wenn NICHT[Ck(3,117)=0]]
Port 1191 (Elza):
  - 1191 #0: "There's a switch here. Push it? "  [LEBT sub02@0x24BA Message_on; LEBT sub03@0x2532 Message_on; LEBT sub04@0x25AA Message_on; LEBT sub05@0x2622 Message_on; LEBT sub12@0x2892 Message_on]
  - 1191 #1: "There's a driver here. Take it? "  [kein Aufruf]
  - 1191 #2: "I have nothing else to do / here."  [LEBT main00@0x227A AOT0-sce1 wenn NICHT[Ck(4,234)=0]; LEBT main00@0x228E AOT1-sce1 wenn NICHT[Ck(4,234)=0]; LEBT main00@0x22A2 AOT2-sce1 wenn NICHT[Ck(4,234)=0]; LEBT main00@0x22B6 AOT3-sce1 wenn NICHT[Ck(4,234)=0]; LEBT sub10@0x27B4 Aot_reset0-sce1; LEBT sub10@0x27BE Aot_reset1-sce1; LEBT sub10@0x27C8 Aot_reset2-sce1; LEBT sub10@0x27D2 Aot_reset3-sce1]
  - 1191 #3: "No response...The power is not supplied."  [LEBT sub12@0x28A0 Message_on wenn Ck(12,31)=0]
  - 1191 #4: "There is one R.P.D. armor / that should fit you.Will you equip it? "  [LEBT sub15@0x2DAE Message_on wenn Ck(3,117)=0]
  - 1191 #5: "Will you unequip the armor? e         /   4      4   /"  [LEBT sub15@0x2DB8 Message_on wenn NICHT[Ck(3,117)=0]]

### Raum 11B — Garage mit Gittertor, Rolltor, grüner Boden, Kanaldeckel
Disc (TEXTE_40.md):
  - D11B #0 [Elza | Elza] あなた！ こんな所でどうしたの？  =  Du! Was machst du denn an so einem Ort?
  - D11B #1 [Elza | Elza] ここに じこめられているのね？ / こっちへおいで…  =  Du bist hier eingesperrt, nicht wahr? Komm her… [Lesart]
  - D11B #2 [Elza | Elza] あなた なまえは？  =  Wie heißt du?
  - D11B #3 [Elza | Sherry] …シェリー  =  …Sherry.
  - D11B #4 [Elza | Elza] シェリーっていうの？カワイイなまえね / …私はエルザよ  =  Sherry heißt du? Ein süßer Name. …Ich bin Elza.
  - D11B #5 [Elza | Sherry] エルザ…  =  Elza…
  - D11B #6 [Elza | Elza] そう！…いい？シェリー  =  Genau! …Hör zu, Sherry.
  - D11B #7 [Elza | Elza] ここで待ってなさい / 後できっと助けてあげるから  =  Warte hier. Ich hole dich später ganz sicher raus.
  - D11B #8 [Elza | Elza] くるまの中でおとなしく待ってるのよ / いいわね？  =  Warte brav im Wagen, ja?
  - D11B #9 [Leon | Leon] お おい！  =  H-he! (ruft der vorbeilaufenden Sherry nach)
  - D11B #10 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] レオン！無事だったか / あの上の騒ぎはなんだ？  =  Leon! Du lebst! Was war das für ein Lärm oben?
  - D11B #11 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Leon] シャッターが破られた / 奴らつぎつぎと入ってきやがる  =  Das Rolltor wurde durchbrochen. Sie kommen einer nach dem anderen rein.
  - D11B #12 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] 長は？  =  Und der Chef? [Lesart, Lücke 署]
  - D11B #13 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] そうか…チクショウ！  =  Verstehe… verdammt!
  - D11B #14 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] 見ただろ？パトじゃもう出られない / どうする？  =  Hast du gesehen? Mit dem Streifenwagen kommen wir nicht mehr raus. Was nun?
  - D11B #15 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Leon] マーヴイン 来てくれ！  =  Marvin, komm her!
  - D11B #16 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Leon] この下にマンホールがあるんだ / そこから地下に逃げるってのはどうだ？  =  Hier drunter ist ein Kanaldeckel. Wie wäre es, wenn wir dort in den Untergrund fliehen?
  - D11B #17 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] やってみる はあるな / よし こいつをどかそう  =  Einen Versuch ist es wert. Gut, schieben wir das Ding weg. [Lesart, Lücke 価値]
  - D11B #18 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] レオン 手根え！  =  Leon, hilf mit! [Lesart]
  - D11B #19 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Wohl Marvin, keine Regie] さぁ 急げ！  =  Los, schnell!
  - D11B #20 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Leon] マーヴィン 俺が先に降りる / つぎに女を降ろせ！  =  Marvin, ich gehe zuerst runter. Danach lass die Frau runter!
  - D11B #21 [Leon, tot (11C0 `@0x180A` fragt Bank 8 ab) | Marvin] OK / 気をつけろよ！  =  OK. Pass auf dich auf!
  - D11B #22 [Leon + Elza | System] 1人では押せそうにない  =  Allein lässt sich das nicht schieben.
Port 11B0 (Leon):
  - 11B0 #0: "Leon: H-hey!"  [LEBT sub04@0x1338 Message_on]
  - 11B0 #1: "Marvin: Leon, you're okay! What / was the commotion up there?"  [LEBT sub06@0x14EE Message_on]
  - 11B0 #2: "Leon: Those things broke through the / shutters and started getting inside."  [LEBT sub06@0x150E Message_on]
  - 11B0 #3: "Marvin: What about the chief?"  [LEBT sub06@0x152E Message_on]
  - 11B0 #4: "Marvin: Oh, I see... Damn it!"  [LEBT sub06@0x1574 Message_on]
  - 11B0 #5: "Marvin: Well you saw it, right? / We can't get out using the patrol / cars anymore. What should we do?"  [LEBT sub06@0x1590 Message_on]
  - 11B0 #6: "Leon: Marvin, please come here!"  [LEBT sub06@0x15B8 Message_on]
  - 11B0 #7: "Leon: There's a manhole here. How / about an underground escape route?"  [LEBT sub06@0x1622 Message_on]
  - 11B0 #8: "Marvin: We don't really have any other / option than to try. Okay, let's see if / we can move this!"  [LEBT sub06@0x166E Message_on]
  - 11B0 #9: "Marvin: Leon! give me a hand!"  [LEBT sub06@0x168A Message_on]
  - 11B0 #10: "Marvin: Okay, let's hurry!"  [LEBT sub06@0x176E Message_on]
  - 11B0 #11: "Leon: Marvin, I'll climb down first, / you help her get down!"  [LEBT sub06@0x178E Message_on]
  - 11B0 #12: "Marvin: Okay! Be careful down there!"  [LEBT sub06@0x17AE Message_on]
  - 11B0 #13: "It doesn't seem like you could move / this on your own."  [LEBT sub03@0x126E Message_on wenn Ck(3,131)=1 / Member_cmp(3e000e000000) / Cmp(V[0] op0 3) / Sce_key_ck(010100); LEBT sub03@0x1298 Message_on wenn NICHT[Ck(3,131)=1] / Member_cmp(3e000e000000) / Cmp(V[0] op0 2) / Sce_key_ck(010100)]
  - 11B0 #14: "The shutter is tightly sealed in place."  [LEBT main00@0x106C AOT7-sce1]
  - 11B0 #15: "It's too dark to see anything...I must find way to activate the energy."  [LEBT sub01@0x1216 Aot_reset1-sce1 wenn Ck(4,243)=0; LEBT sub01@0x1220 Aot_reset4-sce1 wenn Ck(4,243)=0; LEBT sub01@0x122A Aot_reset7-sce1 wenn Ck(4,243)=0]
Port 11B1 (Elza):
  - 11B1 #0: "Elza: Hey! What are you doing in a / place like this?"  [LEBT sub02@0x1234 Message_on]
  - 11B1 #1: "Elza: You got trapped in here, right?"  [LEBT sub02@0x1278 Message_on]
  - 11B1 #2: "Elza: What's your name?"  [LEBT sub02@0x12B8 Message_on]
  - 11B1 #3: "Girl: ...Sherry."  [LEBT sub02@0x12D4 Message_on]
  - 11B1 #4: "Elza: Sherry, huh? That's a cute name. / I'm Elza."  [LEBT sub02@0x12F0 Message_on]
  - 11B1 #5: "Sherry: Elza..."  [LEBT sub02@0x1320 Message_on]
  - 11B1 #6: "Elza: That's right! Okay Sherry, listen / up..."  [LEBT sub02@0x133C Message_on]
  - 11B1 #7: "Elza: Just wait there. / I'll come rescue you in a bit."  [LEBT sub02@0x1368 Message_on]
  - 11B1 #8: "Elza: So just hold tight and wait for / me quietly in that car over there. Okay?"  [LEBT sub02@0x1394 Message_on]
  - 11B1 #9: "It doesn't seem like you could move / this on your own."  [LEBT sub04@0x1512 Message_on wenn Member_cmp(3e000e000000) / Cmp(V[0] op0 3) / Sce_key_ck(010100)]
  - 11B1 #10: "Elza: Sherry!?"  [LEBT sub03@0x1480 Message_on]
  - 11B1 #11: "Elza: Where has she gone!?"  [LEBT sub03@0x14CE Message_on]
  - 11B1 #12: "The door is locked."  [LEBT sub00@0x112A AOT8-sce1 wenn NICHT[Ck(3,131)=1]]
  - 11B1 #13: "The shutter is tightly sealed in place."  [LEBT main00@0x1030 AOT7-sce1]
  - 11B1 #14: "It's too dark to see anything...I must find way to activate the energy."  [LEBT sub01@0x11C0 Aot_reset1-sce1 wenn Ck(4,243)=0; LEBT sub01@0x11CA Aot_reset4-sce1 wenn Ck(4,243)=0; LEBT sub01@0x11D4 Aot_reset7-sce1 wenn Ck(4,243)=0; LEBT sub01@0x11DE Aot_reset8-sce1 wenn Ck(4,243)=0]

### Raum 11C — Parkgarage mit Streifenwagen
Disc (TEXTE_40.md):
  - D11C #0 [Leon | Leon] エイダ！マーヴィンは？ / 一緒じゃなかったのか？ /  =  Ada! Wo ist Marvin? War er nicht bei dir?
  - D11C #1 [Leon | Ada] かれならと中でひきかえして / お前を捜しに行った /  =  Er ist unterwegs umgekehrt und wollte dich suchen.
  - D11C #2 [Leon | Ada] さっき上の階で / すごいおとがしたときにな /  =  Vorhin, als es oben so laut krachte.
  - D11C #3 [Leon | Ada] 会わなかったのか？ /  =  Bist du ihm nicht begegnet?
  - D11C #4 [Leon | Leon] なんだって！ /  =  Was!
  - D11C #5 [Leon | Leon] エイダ君はパトカーの中にかくれていろ /  =  Ada, versteck dich im Streifenwagen.
  - D11C #6 [Leon | Ada] そ・そうしよう・・・ /  =  J-ja, das tue ich…
  - D11C #7 [Leon, tot (wie 11B #10) | Leon] さぁ もうおわったぜ /  =  So, das war es.
  - D11C #8 [Leon, tot (wie 11B #10) | Ada] ・・・とんでもない奴がいるな /  =  …was für ein Ungeheuer hier herumläuft.
  - D11C #9 [Leon, tot (wie 11B #10) | Leon] あぁ とにかくここはもう出よう /  =  Ja. Hauptsache, wir kommen hier raus.
  - D11C #10 [nicht ausgelöst | Leon] あぁ とにかくここはもう出よう /  =  Ja. Hauptsache, wir kommen hier raus. (Dublette)
Port 11C0 (Leon):
  - 11C0 #0: "Leon: Ada! Where's Marvin? Wasn't / he with you?"  [LEBT sub02@0x18A0 Message_on]
  - 11C0 #1: "Ada: He turned around all of a sudden / to go look for you."  [LEBT sub02@0x18D4 Message_on]
  - 11C0 #2: "Ada: There was this really loud noise / from upstairs earlier."  [LEBT sub02@0x18EC Message_on]
  - 11C0 #3: "Ada: You didn't run into him?"  [LEBT sub02@0x1904 Message_on]
  - 11C0 #4: "Leon: What!?"  [LEBT sub02@0x1928 Message_on]
  - 11C0 #5: "Leon: Ada, you hide inside that patrol / car."  [LEBT sub02@0x1A5E Message_on]
  - 11C0 #6: "Ada: O-okay, I'll do that..."  [LEBT sub02@0x1A80 Message_on]
  - 11C0 #7: "Ada: Okay, it's over now."  [LEBT sub03@0x1B68 Message_on]
  - 11C0 #8: "Ada: ...There are some outrageous / monsters out there."  [LEBT sub03@0x1B7A Message_on]
  - 11C0 #9: "Leon: Yeah... Anyway, let's get / out of here."  [LEBT sub03@0x1B86 Message_on]
Port 11C1 (Elza):
  (kein Text)

### Raum 207 — Kanaltunnel
Disc (TEXTE_40.md):
  - D207 #0 [Leon, Raum ohne Tür zu ihm, nur per JUMP | System] シャッターのスイッチがある。 / 押しますか？ [Ja/Nein]  =  Ein Schalter für das Rolltor. Drücken?
Port 2070 (Leon):
  - 2070 #0: "You've opened the lock."  [LEBT sub02@0xCE8 Message_on]
Port 2071 (Elza):
  - 2071 #0: "You've opened the lock."  [LEBT sub02@0xCE8 Message_on]

### Raum 300 — Hof mit Umbrella-Lkw vor der Fabrik
Disc (TEXTE_40.md):
  - D300 #0 [Leon + Elza | System] マンホールの下に ハシゴがのびている / 降りますか？ [Ja/Nein]  =  Unter dem Kanaldeckel führt eine Leiter hinab. Hinabsteigen?
  - D300 #1 [tot (`ROOM3000 @0x161A`, Spielfigur 13 kommt nicht vor) | Spielfigur PL0D (Marvin)] もう体力がもちそうにない / 仲間のもとへ急ごう  =  Meine Kräfte reichen nicht mehr. Schnell zu den Gefährten.
  - D300 #2 [Leon | Leon] マーヴィン！ これは・・・ / ラクーン警さつの警かんじゃないか！  =  Marvin! Das sind doch… Polizisten aus Raccoon!
  - D300 #3 [Leon | Marvin] ああ こいつはひどい・・・ / みんな アンブレラ事けんの調査たい員だ  =  Ja, furchtbar… alles Ermittler im Umbrella-Fall.
  - D300 #4 [Leon | Marvin] このはいこう度 何かあるぜ・・・ / レオン 気をつけろ！  =  In dieser stillgelegten Fabrik ist etwas… Leon, sei vorsichtig! [Lesart]
  - D300 #5 [Leon | Leon] / マーヴィン あの女は？  =  Marvin, wo ist die Frau?
  - D300 #6 [Leon | Marvin] / エイダか・・・ 奴なら先に行った  =  Ada? Die ist schon vorgegangen.
  - D300 #7 [Leon | Marvin] あの女 / この光けいに女いろひとつ変えなかったぜ  =  Bei diesem Anblick hat sie nicht mal die Miene verzogen. [Lesart]
  - D300 #8 [Leon | Leon] 今度会ったときは / 聞く事がやまほどありそうだな・・・  =  Beim nächsten Treffen werde ich sie einiges fragen müssen…
  - D300 #9 [Elza | —] / ！！  =  !!
  - D300 #10 [Elza | Elza] / な 何 ここは・・・？  =  W-was ist das hier…?
  - D300 #11 [Elza | John] / 見ての通り 警かんの死体さ  =  Wie du siehst: tote Polizisten.
  - D300 #12 [Elza | John] ここはむかし 化学薬品こうじょうだった / て話だが今じゃ使われてねぇはずだ・・・  =  Das hier soll mal eine Chemiefabrik gewesen sein, aber die dürfte längst stillliegen…
  - D300 #13 [Elza | John] / いったい 何があったんだ？  =  Was ist hier bloß passiert?
  - D300 #14 [Elza | Sherry] / エルザ・・・  =  Elza…
  - D300 #15 [Elza | Elza] / 大丈夫よ しっかりくっついてなさい  =  Keine Angst, bleib dicht bei mir.
  - D300 #16 [Elza | John] 行くぜ / とりあえず かくれ度所さがしだ  =  Gehen wir. Erst mal ein Versteck suchen. [Lesart]
Port 3000 (Leon):
  - 3000 #0: "A ladder leads down the manhole. / Descend it? "  [LEBT sub02@0x1658 Message_on wenn NICHT[Cmp(V[16] op0 13)]]
  - 3000 #1: "My strength is running out. / I've got to find the others!"  [LEBT sub02@0x164E Message_on wenn Cmp(V[16] op0 13)]
  - 3000 #2: "Leon: Marvin! Aren't these... RPD / officers!?"  [LEBT sub03@0x16B6 Message_on]
  - 3000 #3: "Marvin: Oh, man... They must be from the / squad investigating on the Umbrella / incident."  [LEBT sub03@0x16E2 Message_on]
  - 3000 #4: "Marvin: We don't know what's up ahead... / Stay alert, Leon!"  [LEBT sub03@0x1712 Message_on]
  - 3000 #5: "Leon: Marvin, what about that woman?"  [LEBT sub03@0x1732 Message_on]
  - 3000 #6: "Marvin: Ada...? She went on ahead."  [LEBT sub03@0x176A Message_on]
  - 3000 #7: "Marvin: She didn't seem to be at all / disturbed by all this massacre."  [LEBT sub03@0x1786 Message_on]
  - 3000 #8: "Leon: There's a ton of things I need / to ask her the next time I see her..."  [LEBT sub03@0x17AE Message_on]
Port 3001 (Elza):
  - 3001 #0: "A ladder leads down the manhole. / Descend it? "  [LEBT sub02@0x1690 Message_on wenn NICHT[Cmp(V[16] op0 13)]]
  - 3001 #1: "My strength is running out. / I've got to find the others!"  [LEBT sub02@0x1686 Message_on wenn Cmp(V[16] op0 13)]
  - 3001 #2: "Elza: !!"  [LEBT sub03@0x16F6 Message_on]
  - 3001 #3: "Elza: Wh-what is this place...?"  [LEBT sub03@0x16FE Message_on]
  - 3001 #4: "John: These corpses are definitively / police officers."  [LEBT sub03@0x1722 Message_on]
  - 3001 #5: "John: This used to be a chemical plant, / but it was supposedly not in use anymore..."  [LEBT sub03@0x1748 Message_on]
  - 3001 #6: "Elza: What happened here?"  [LEBT sub03@0x177C Message_on]
  - 3001 #7: "Sherry: Elza..."  [LEBT sub03@0x1794 Message_on]
  - 3001 #8: "Elza: It'll be okay, just stick close to / me."  [LEBT sub03@0x179E Message_on]
  - 3001 #9: "John: Let's go. We should find / somewhere to hide for now."  [LEBT sub03@0x17BE Message_on]

### Raum 301 — Straße, Blutlachen
Disc (TEXTE_40.md):
  - D301 #0 [nicht ausgelöst | System] ロックされている / ドアを開けるには カードが必要だ  =  Verriegelt. Zum Öffnen braucht man eine Karte.
  - D301 #1 [Leon + Elza | System] / IDカードを使って ロックを外した  =  Mit der ID-Karte entriegelt.
Port 3010 (Leon):
  - 3010 #0: "It's locked.An ID card is required to open it."  [LEBT main00@0x20B2 AOT6-sce1 wenn Ck(3,59)=0]
  - 3010 #1: "You've used the Green Keycard."  [LEBT sub02@0x2306 Message_on]
Port 3011 (Elza):
  - 3011 #0: "It's locked.An ID card is required to open it."  [LEBT main00@0x20B2 AOT6-sce1 wenn Ck(3,59)=0]
  - 3011 #1: "You've used the Green Keycard."  [LEBT sub02@0x233A Message_on]

### Raum 306 — Hinterhof am Zaun
Disc (TEXTE_40.md):
  - D306 #0 [Leon + Elza | System] / カギがかかっている  =  Abgeschlossen.
  - D306 #1 [Elza | Elza] / ちょっと シェリー！  =  He, Sherry!
  - D306 #2 [Elza | Elza] どうしたの？ / 急にはしり出したりして…  =  Was ist denn? Rennst einfach los…
  - D306 #3 [Elza | Sherry] …エルザ / 私 ヒミツの入り口を知ってるの  =  …Elza, ich kenne einen geheimen Eingang.
  - D306 #4 [Elza | Elza] / ヒミツの入り口？  =  Einen geheimen Eingang?
  - D306 #5 [Elza | Sherry] / ここよ この中から入れるの  =  Hier. Da drin kommt man rein.
  - D306 #6 [Elza | Elza] / シェリー！ 待って！  =  Sherry! Warte!
  - D306 #7 [Elza | Sherry] 裏口にかかっているカギは / 中から外すわ / トビラのところで待ってて！  =  Das Schloss an der Hintertür mache ich von innen auf. Wartet an der Tür!
  - D306 #8 [Elza | John] お おいガキ！ 危ねえぞ / 戻ってこい  =  He, Kleine! Das ist gefährlich, komm zurück!
  - D306 #9 [Leon | Erzähltext, Leons Sicht] いつの間にか、 / カギがはずされている・・・  =  Irgendwann ist das Schloss aufgemacht worden…
Port 3060 (Leon):
  - 3060 #0: " / It's locked."  [LEBT sub00@0xF2C Aot_reset1-sce1 wenn Ck(3,160)=0]
  - 3060 #1: "Elza: Sherry, come back!"  [kein Aufruf]
  - 3060 #2: "Elza: What's wrong? Why did you start / running...?"  [kein Aufruf]
  - 3060 #3: "Sherry: Elza... I know a secret / way in!"  [kein Aufruf]
  - 3060 #4: "Elza: A secret entrance?"  [kein Aufruf]
  - 3060 #5: "Sherry: It's here, I can get inside / through this hole!"  [kein Aufruf]
  - 3060 #6: "Elza: Sherry, wait!"  [kein Aufruf]
  - 3060 #7: "Sherry: I'll unlock the back door from / the other side!Just wait by the door, okay?"  [kein Aufruf]
  - 3060 #8: "John: H-hey, you brat! That's dangerous, / get back here!"  [kein Aufruf]
  - 3060 #9: "For some reason the door isn't / locked anymore..."  [LEBT sub02@0x1034 Message_on]
  - 3060 #10: "It's locked from the other side."  [LEBT main00@0xEBE AOT2-sce1 wenn Ck(4,196)=0]
Port 3061 (Elza):
  - 3061 #0: " / It's locked."  [LEBT sub00@0x1016 Aot_reset1-sce1 wenn Ck(3,160)=0; LEBT sub02@0x1040 Message_on]
  - 3061 #1: "Elza: Sherry, come back!"  [LEBT sub02@0x10A8 Message_on]
  - 3061 #2: "Elza: What's wrong? Why did you start / running...?"  [LEBT sub02@0x10FC Message_on]
  - 3061 #3: "Sherry: Elza... I know a secret / way in!"  [LEBT sub02@0x1104 Message_on]
  - 3061 #4: "Elza: A secret entrance?"  [LEBT sub02@0x110C Message_on]
  - 3061 #5: "Sherry: It's here, I can get inside / through this hole!"  [LEBT sub02@0x113C Message_on]
  - 3061 #6: "Elza: Sherry, wait!"  [LEBT sub02@0x1144 Message_on]
  - 3061 #7: "Sherry: I'll unlock the back door from / the other side!"  [LEBT sub02@0x117C Message_on]
  - 3061 #8: "Sherry: Just wait by the door, okay?"  [LEBT sub02@0x1184 Message_on]
  - 3061 #9: "John: H-hey, you brat! That's dangerous, / get back here!"  [LEBT sub02@0x118C Message_on]
  - 3061 #10: "It's locked from the other side."  [LEBT main00@0xEBE AOT2-sce1 wenn Ck(4,196)=0]

### Raum 307 — Halle mit Lastenaufzug
Disc (TEXTE_40.md):
  - D307 #0 [Leon | Ada] このリフトに東れ！ / …もうそれしかない  =  Steigt in diesen Aufzug! …Etwas anderes bleibt nicht. [Lesart]
  - D307 #1 [Leon | Leon] これに？ / …一体どこに続いているんだ？  =  Hier rein? …Wohin führt der denn?
  - D307 #2 [Leon | Ada] 地仲さ… / まだここで奴東に食われた方がましかもな…  =  Vermutlich: In die Hölle… vielleicht wäre es besser, sich hier von denen fressen zu lassen… [Lesart, Disc 地仲さ]
  - D307 #3 [Leon | Leon] / 何だって？  =  Was?
  - D307 #4 [Leon | Marvin oder Ada, keine Regie] / 急げ 奴東が来る！  =  Schnell, sie kommen! [Lesart 奴等]
  - D307 #5 [Elza | Elza] / 東かにね きっとどこかにいるわ…！  =  Leise! Es ist bestimmt irgendwo hier…! [Lesart 静かにね]
  - D307 #6 [Elza | John] / OK エルザ 後ろは俺に任せときな  =  OK Elza, den Rücken decke ich dir.
  - D307 #7 [Elza | Elza] / このリフトから東れたのよ  =  Es kam aus diesem Aufzug. [Lesart]
  - D307 #8 [Elza | John] / ！…でっけえリフトだな… よっぽど大東の東物を / 階んでいたんだろう / …動かせるか？  =  …Riesiges Ding. Damit wurden wohl sehr große Lasten transportiert. …Kriegst du ihn in Gang? [Lesart]
  - D307 #9 [Elza | Elza] / だめよ…外部の人間には無理みたいね…  =  Nein… für Außenstehende ist das wohl unmöglich…
  - D307 #10 [Elza | Sherry] / 私 動かし方わかるわ！  =  Ich weiß, wie man ihn bedient!
  - D307 #11 [Elza | Elza] / シェリー？…あなたって一体…  =  Sherry? …Wer bist du eigentlich…
  - D307 #12 [Elza | John] おい 話は後だ / 奴東この中まで入ってくるつもりだ…！ / リフトが動くなら 下に降りようぜ  =  He, reden wir später. Die wollen bis hier rein…! Wenn der Aufzug läuft, fahren wir runter. [Lesart]
  - D307 #13 [Elza | Elza] / ちょっと 何言ってるのよ！ 得体の知れない怪物がここから / 東れたのよ！？  =  Was redest du da! Von hier ist ein unbekanntes Monster gekommen!? [Lesart]
  - D307 #14 [Elza | John] / ここにいたら確実に奴東のえさだ！ もう どうする事も出来ないぜ / さあ 行こう  =  Bleiben wir hier, sind wir deren Futter! Es geht nicht anders. Los. [Lesart]
  - D307 #15 [Leon + Elza | System] 電東が入っていないようだ・・・ / コントロールBOXさえ動けば ▼ / エレベーターは作動するようだが・・・  =  Anscheinend kein Strom… Liefe die Steuerbox, würde der Aufzug wohl funktionieren… [Lesart]
Port 3070 (Leon):
  - 3070 #0: "Leon: Get on the lift! It's our only / option..."  [LEBT sub03@0x369C Message_on]
  - 3070 #1: "Marvin: This thing...? / But where will it take us?"  [LEBT sub03@0x36D4 Message_on]
  - 3070 #2: "Ada: Underground... Though we might / be better off staying here and get / eaten by those things..."  [LEBT sub03@0x36DC Message_on]
  - 3070 #3: "Marvin: What?"  [LEBT sub03@0x3714 Message_on]
  - 3070 #4: "Leon: Hurry up! They're coming!"  [LEBT sub03@0x371C Message_on]
  - 3070 #5: "The power seems to be off... / If the control box worked,the elevator might move, but..."  [LEBT sub00@0x33F6 Aot_reset0-sce1 wenn Ck(4,215)=0; LEBT sub00@0x3420 Aot_reset0-sce1 wenn NICHT[Ck(4,215)=0] / Ck(4,236)=0; LEBT sub00@0x3478 Aot_reset0-sce1 wenn NICHT[Ck(4,215)=0] / NICHT[Ck(4,236)=0] / Ck(3,180)=1]
Port 3071 (Elza):
  - 3071 #0: "Elza: I know it has to be somewhere...!"  [LEBT sub05@0x3773 Message_on]
  - 3071 #1: "John: Okay, Elza! I've got your back!"  [LEBT sub05@0x37A7 Message_on]
  - 3071 #2: "Elza: This lift, this is where it / came from."  [LEBT sub11@0x3A1D Message_on]
  - 3071 #3: "John: ...! That's a damn huge lift..."  [LEBT sub11@0x3A71 Message_on]
  - 3071 #4: "John: They probably used it to move / some heavy equipment."  [LEBT sub11@0x3A91 Message_on]
  - 3071 #5: "John: ...Can we get it to work?"  [LEBT sub11@0x3AA9 Message_on]
  - 3071 #6: "Elza: No... It looks like it can be / operated only from the inside..."  [LEBT sub11@0x3AC9 Message_on]
  - 3071 #7: "Sherry: I know how to make it work!"  [LEBT sub11@0x3B49 Message_on]
  - 3071 #8: "Elza: Sherry? What are you talking / about...?"  [LEBT sub11@0x3B6F Message_on]
  - 3071 #9: "John: Save the discussion for later! / Those things are trying to get in here...!"  [LEBT sub11@0x3BB5 Message_on]
  - 3071 #10: "John: Let's get this lift moving and go / down there!"  [LEBT sub11@0x3BF3 Message_on]
  - 3071 #11: "Elza: Are you insane!?"  [LEBT sub11@0x3C07 Message_on]
  - 3071 #12: "Elza: I told you that weird monster / came from here!"  [LEBT sub11@0x3C23 Message_on]
  - 3071 #13: "John: If we stay here, we'll be eaten / for sure!"  [LEBT sub11@0x3C4B Message_on]
  - 3071 #14: "John: We've got no choice but to take / our chances down there! Let's go!"  [LEBT sub11@0x3C67 Message_on]
  - 3071 #15: "The power seems to be off... / If the control box worked,the elevator might move, but..."  [LEBT sub00@0x3460 Aot_reset0-sce1 wenn Ck(4,75)=0; LEBT sub00@0x3492 Aot_reset0-sce1 wenn NICHT[Ck(4,75)=0] / Ck(4,239)=0; LEBT sub00@0x34EA Aot_reset0-sce1 wenn NICHT[Ck(4,75)=0] / NICHT[Ck(4,239)=0] / Ck(3,180)=1]

### Raum 308 — Stahlträger-Halle, grüne Tür
Disc (TEXTE_40.md):
  - D308 #0 [Leon | Marvin oder Leon] / …しつこい奴だぜ…  =  …hartnäckiger Kerl…
  - D308 #1 [Leon | Eher Ada, nicht bewiesen] / …あれは…やはり…  =  …das ist… also doch…
Port 3080 (Leon):
  - 3080 #0: "Ada: You just don't give up..."  [LEBT sub02@0x9E4 Message_on]
  - 3080 #1: "Ada: That's... got to be..."  [LEBT sub02@0xA1A Message_on]
Port 3081 (Elza):
  (kein Text)

### Raum 309 — Grün gestrichener Raum mit Aushängen
Disc (TEXTE_40.md):
  - D309 #0 [Leon + Elza | System] / 電気はすでについている・・・  =  Das Licht ist bereits an…
  - D309 #1 [Elza | System] 電気がつかない・・・ / ブレーカーが上がっているのだろうか？  =  Das Licht geht nicht an… Ist der Schutzschalter oben?
  - D309 #2 [Leon + Elza | System] / ブレーカーは下がっているようだ・・・  =  Der Schalter scheint unten zu sein…
  - D309 #3 [Elza | System] / ブレーカーに手がとどかない・・・  =  Ich komme nicht an den Schalter…
  - D309 #4 [Leon + Elza | System] / ブレーカーは下がっているようだ・・・  =  Der Schalter scheint unten zu sein…
  - D309 #5 [Elza | System] ブレーカーが上がっている・・・ / 下ろしますか？ [Ja/Nein]  =  Der Schalter ist oben… Umlegen?
  - D309 #6 [Elza | System] かんぬきがはまっているので / ドアはあかない ▼ / これをはずすには カギが必要だ・・・  =  Ein Riegel liegt vor, die Tür geht nicht auf. Zum Lösen braucht man einen Schlüssel…
  - D309 #7 [nicht ausgelöst | System] かんぬきがはまっているので / ドアはあかない  =  Ein Riegel liegt vor, die Tür geht nicht auf.
  - D309 #8 [Elza | Sherry-Abschnitt, auf der Disc bleibt die Figur Elza] / はやくエルザたちを助けなければ・・・  =  Ich muss Elza und die anderen schnell retten…
  - D309 #9 [Elza | John] はあ、はあ…間一 ってヤツだな！ / もう少しでケツに いつかれてた所だぜ / 大丈夫か、エルザ  =  Puh… das war knapp! Fast hätten sie mir in den Hintern gebissen. Alles klar, Elza? [Lesart]
  - D309 #10 [Elza | Elza] はあ、はあ…何とかね！ / でも、もうダメかと思ったわ！ /  =  Puh… geht so! Ich dachte schon, das war es!
  - D309 #11 [Elza | John] おい、お ちゃん 助かった、 / にきるぜ！ /  =  He, Kleine, danke! Das vergesse ich dir nicht! [Lesart]
  - D309 #12 [Elza | Elza] / ホントよ、ありがとうはあ、助かったわ /  =  Wirklich, danke. Puh, du hast uns gerettet.
  - D309 #13 [Elza | John] / お ちゃん、 前なんてえんだ？ /  =  Wie heißt du denn, Kleine? [Lesart]
  - D309 #14 [Elza | Sherry] / シェリー… /  =  Sherry…
  - D309 #15 [Elza | John] シェリーか！俺はジョンだこれから俺が / 何があっても守ってやるから安心しな！ /  =  Sherry also! Ich bin John. Ab jetzt beschütze ich dich, egal was kommt!
  - D309 #16 [Elza | Elza] / ずいぶんな気に入り様ね /  =  Du bist ja ganz vernarrt.
  - D309 #17 [Elza | John] そりゃそうさ、西の 人だぜ？ / 俺の西に えてもシェリーは俺が守る！ /  =  Na klar, sie hat mir das Leben gerettet! Sherry beschütze ich, und wenn es mein Leben kostet! [Lesart]
  - D309 #18 [Elza | Elza] 単純ね…。それよりシェリー、あんた / なぜあんな入り口を知ってるの？ /  =  Du bist einfach gestrickt… Aber Sherry, woher kennst du so einen Eingang?
  - D309 #19 [Elza | Sherry] ここは…私のパパが いていた所なの。 / …だから… /  =  Hier… hat mein Papa gearbeitet. …Deshalb… [Lesart]
  - D309 #20 [Elza | Elza] / あなたの…パパが…？ /  =  Dein… Papa…?
  - D309 #21 [Elza | John] おい、そろそろ行こうぜ。 / こんな所にいたら、奴 またゾロゾロ / まってくる。  =  He, gehen wir langsam. Wenn wir hierbleiben, kommen die wieder in Scharen. [Lesart]
  - D309 #22 [Elza | Elza] そうね。手分けして、どこか / 安全な度所を しましょうか /  =  Stimmt. Teilen wir uns auf und suchen einen sicheren Ort. [Lesart]
  - D309 #23 [Elza | John] そうだな、あまり無理するなよ。 / …シェリー、お前は俺と来な！ /  =  Gut. Übernimm dich nicht. …Sherry, du kommst mit mir!
  - D309 #24 [Elza | Elza] / ちょっと、大丈夫でしょうね？ /  =  Moment, geht das auch gut?
  - D309 #25 [Elza | John] / 任せとけ！ /  =  Verlass dich drauf!
  - D309 #26 [Elza | System] / かんぬきのかぎをひろった  =  Riegelschlüssel aufgehoben.
  - D309 #27 [nicht ausgelöst | System] / かんぬきのかぎを使ってドアを開けた  =  Tür mit dem Riegelschlüssel geöffnet.
Port 3090 (Leon):
  - 3090 #0: " / The power is already on..."  [LEBT sub02@0x14C6 Message_on wenn Ck(4,8)=1]
  - 3090 #1: " / This breaker switch is flipped down..."  [LEBT sub03@0x14D8 Message_on wenn Ck(4,8)=1]
  - 3090 #2: " / This breaker switch is flipped down..."  [LEBT sub04@0x14EA Message_on wenn Ck(4,8)=1]
  - 3090 #3: "The door is latched shut and won't open."  [LEBT sub00@0x1442 Aot_reset1-sce1 wenn Ck(4,8)=0]
Port 3091 (Elza):
  - 3091 #0: " / The power is already on..."  [LEBT sub03@0x15B6 Message_on wenn Ck(4,8)=1]
  - 3091 #1: "The power is off... Has a circuit breaker / tripped?"  [LEBT sub03@0x15C0 Message_on wenn NICHT[Ck(4,8)=1]]
  - 3091 #2: " / This breaker switch is flipped down..."  [LEBT sub04@0x15D2 Message_on wenn Ck(4,8)=1]
  - 3091 #3: " / I can't reach the breaker switch..."  [LEBT sub04@0x15DC Message_on wenn NICHT[Ck(4,8)=1]]
  - 3091 #4: " / This breaker switch is flipped down..."  [LEBT sub05@0x15EE Message_on wenn Ck(4,8)=1]
  - 3091 #5: "This breaker switch is flipped up... Flip / it back down? "  [LEBT sub05@0x15F8 Message_on wenn NICHT[Ck(4,8)=1]]
  - 3091 #6: "The door is latched shut and won't open.I'd need a key to open this..."  [LEBT sub02@0x159A Message_on wenn Ck(4,16)=0 / Ck(9,21)=0]
  - 3091 #7: "The door is latched shut and won't open."  [kein Aufruf]
  - 3091 #8: " / I need to hurry up and save the others..."  [LEBT sub00@0x149E Aot_reset0-sce1 wenn Ck(4,16)=0 / Ck(4,8)=0; LEBT sub00@0x14B2 Aot_reset2-sce1 wenn Ck(4,16)=0 / Ck(4,8)=0; LEBT sub00@0x14BC Aot_reset3-sce1 wenn Ck(4,16)=0 / Ck(4,8)=0]
  - 3091 #9: "John: That was a close shave! / I almost got my ass handed to me!"  [LEBT sub07@0x17A8 Message_on]
  - 3091 #10: "John: Elza, are you okay?"  [LEBT sub07@0x17D6 Message_on]
  - 3091 #11: "Elza: Yes, somehow! I thought I was / done for!"  [LEBT sub07@0x1804 Message_on]
  - 3091 #12: "John: Hey, you really saved my butt / there!"  [LEBT sub07@0x184A Message_on]
  - 3091 #13: "Elza: You really did save us, / thank you!"  [LEBT sub07@0x187C Message_on]
  - 3091 #14: "John: What's your name, kid?"  [LEBT sub07@0x18C2 Message_on]
  - 3091 #15: "Sherry: It's Sherry..."  [LEBT sub07@0x18E6 Message_on]
  - 3091 #16: "John: Sherry, huh? I'm John, and you / can rest assured that I'll keep you / safe from now on!"  [LEBT sub07@0x190E Message_on]
  - 3091 #17: "Elza: You seem to have taken a / liking to her."  [LEBT sub07@0x1946 Message_on]
  - 3091 #18: "John: Well, of course, she saved my / life, you know? That's why I'll protect / her with my life!"  [LEBT sub07@0x1978 Message_on]
  - 3091 #19: "Elza: How simplistic... Anyway, Sherry, / how did you know about that entrance?"  [LEBT sub07@0x19BE Message_on]
  - 3091 #20: "Sherry: This is... where my dad used to / be..."  [LEBT sub07@0x1A16 Message_on]
  - 3091 #21: "Elza: Your... dad...?"  [LEBT sub07@0x1A3E Message_on]
  - 3091 #22: "Sherry: What was that?"  [LEBT sub07@0x1AEE Message_on]
  - 3091 #23: "John: I dunno. Sounds like somebody's / in there."  [LEBT sub07@0x1B26 Message_on]
  - 3091 #24: "Sherry: There wasn't anybody before."  [LEBT sub07@0x1B5A Message_on]
  - 3091 #25: "Elza: Let me deal with it. / John, take Sherry somewhere safe."  [LEBT sub07@0x1BC2 Message_on]
  - 3091 #26: "John: Will do!"  [LEBT sub07@0x1BEE Message_on]
  - 3091 #27: "John: You stay with me now, you hear?"  [LEBT sub07@0x1C0A Message_on]
  - 3091 #28: "Sherry: Yes sir."  [LEBT sub07@0x1C34 Message_on]
  - 3091 #29: "Sherry: Good luck!"  [LEBT sub07@0x1C76 Message_on]
  - 3091 #30: "Elza: Thanks!"  [LEBT sub07@0x1CA8 Message_on]
  - 3091 #31: "You've taken the Latch Key."  [LEBT sub06@0x16BC Message_on wenn Ck(9,21)=0]
  - 3091 #32: "You've used the Latch Key."  [LEBT sub07@0x16CA Message_on]

### Raum 30E — Halle hinter Gitterzaun
Disc (TEXTE_40.md):
  - D30E #0 [Leon | Ada] / 一体何の騒ぎだ 今のは？  =  Was war das für ein Lärm eben?
  - D30E #1 [Leon | Leon] / お お前は…！  =  D-du bist…!
  - D30E #2 [Leon | Ada] …？あの男はどうした？ / 一緒じゃないのか？  =  …? Wo ist der Mann? Ist er nicht bei dir?
  - D30E #3 [Leon | Leon] …あ ああマーヴィンならホールにいる / 外の化け物どもに東をやられて動けないんだ… / …エイダ …君は今まで何をしていたんだ！？ この東度では一体何があった？ / 外の警東東の死体は何だ！？  =  …Ä-äh, Marvin ist in der Halle. Die Bestien draußen haben ihn wohl am Bein erwischt, er kann nicht laufen… …Ada, …was hast du die ganze Zeit gemacht!? Was ist in dieser Fabrik passiert? Und die toten Polizisten draußen!? [Lesart]
  - D30E #4 [Leon | Ada] / …ホール…？  =  …Halle…?
  - D30E #5 [Leon | Ada] まずいぞ あんな所にいたら… / 今 外の奴東がだんだん東東し東めている あんなガラス東りでは / あの人東東手には持たないぞ！  =  Schlecht… wenn er dort bleibt… Die da draußen sammeln sich allmählich. Hinter so viel Glas hält man dieser Menge nicht stand! [Lesart]
  - D30E #6 [Leon | Leon] / マーヴィン！！ 大丈夫か？  =  Marvin!! Alles in Ordnung?
  - D30E #7 [Leon | Leon] この傷…奴東にやられたのか？ / …東当東いな…  =  Diese Wunde… haben die dich erwischt? …ziemlich schlimm… [Lesart, 相当深い oder 相当酷い]
  - D30E #8 [Leon | Marvin] …ホールはまずい！ 外の奴東が / 東面東関を破って中に押し寄せてきた！  =  …Die Halle ist verloren! Die da draußen haben den Haupteingang durchbrochen und drängen herein! [Lesart]
  - D30E #9 [Leon | Leon] / 何だって？  =  Was?
  - D30E #10 [Leon | Marvin] 東東東上の東が東まってきてるぜ！ / こんな東外まで…地上はもう ダメだな…  =  Es sammeln sich mehr, als ich erwartet hatte! Bis hier draußen am Stadtrand… Oben ist es vorbei… [Lesart 予想以上の数が集まって]
  - D30E #11 [Elza | Elza] / ジョン！  =  John!
  - D30E #12 [Elza | John] / エルザ こんな所にいたか まずいぜ…東りを化け物どもに / 東まれちまった！ …東生 東れられそうな所なんか / ありゃしねえ… エルザ そっちはどうだ…奥の東東には / うまい食べ物でもあったか？  =  Elza, hier bist du! Schlecht… die Bestien haben uns umstellt! …Verdammt, nirgends ein Versteck… Elza, wie sieht es bei dir aus… gab es hinten im Lager was Leckeres zu essen? [Lesart 隠れられそう, 倉庫?]
  - D30E #13 [Elza | Elza] / それどころじゃないわ…  =  Das ist jetzt nicht das Problem…
  - D30E #14 [Elza | John] ど どうした？ / …そっちもあの死体どもが…  =  W-was ist? …Waren da auch diese Leichen…
  - D30E #15 [Elza | Elza] 違うわ…そんなのじゃないわ… / あれは あれは… / 一体何だったのかしら…  =  Nein… nicht so was… Das… das war… was war das nur…
  - D30E #16 [Elza | John] 何を言ってる？ / この奥には何がいるんだ？  =  Was redest du? Was ist da hinten?
  - D30E #17 [Elza | Elza] 分からない…分からないけど / あれは…まさか…人間じゃ…  =  Ich weiß es nicht… aber das… das war doch kein… Mensch…
  - D30E #18 [Elza | John] / エルザ そいつはまだ中にいるのか？  =  Elza, ist das Ding noch drin?
  - D30E #19 [Elza | Elza] ええ 多分…でも無東東中で / 攻撃してたら急に怖を消して… / それきりどこに行ったかは分からないわ  =  Ja, wohl schon… Aber als ich in Panik schoss, verschwand es plötzlich… wohin, weiß ich nicht. [Lesart]
  - D30E #20 [Elza | John] …とにかく外は死体どもが多すぎて / 戻るのは危ねえ / …中に入るしかないぜ  =  …Draußen sind jedenfalls zu viele Leichen, zurück ist zu gefährlich. …Wir müssen rein.
  - D30E #21 [Elza | Elza] でも…あの怪物がどこかに / 東んでいるかもしれないのよ？  =  Aber… das Monster lauert vielleicht irgendwo! [Lesart]
  - D30E #22 [Elza | John] 東手が一東なら まだ何とかなるさ… / …行くぜ！  =  Wenn es nur eines ist, schaffen wir das schon… …Los! [Lesart]
  - D30E #23 [Elza | System] 外は危険だ・・・ / 東くエレベーターを動かさなければ  =  Draußen ist es gefährlich… Ich muss schnell den Aufzug in Gang bringen. [Lesart]
Port 30E0 (Leon):
  - 30E0 #0: "Ada: What was that noise?"  [LEBT sub02@0xD00 Message_on]
  - 30E0 #1: "Leon: Y-you...!"  [LEBT sub02@0xD24 Message_on]
  - 30E0 #2: "Ada: ...? Where's that other guy? / Wasn't he with you?"  [LEBT sub02@0xD48 Message_on]
  - 30E0 #3: "Leon: Y-yeah, Marvin is out in the / hall. He can't get out because of / all the monsters outside..."  [LEBT sub02@0xD6C Message_on]
  - 30E0 #4: "Leon: Ada... What have you been doing, / anyway!?"  [LEBT sub02@0xDA0 Message_on]
  - 30E0 #5: "Leon: And what happened in that / tunnel, what's with all the police / officer corpses?"  [LEBT sub02@0xDB8 Message_on]
  - 30E0 #6: " / Manhole...?"  [kein Aufruf]
  - 30E0 #7: "Ada: That's really bad... Those things / should already be trying to bust in / there."  [LEBT sub02@0xDE4 Message_on]
  - 30E0 #8: "Ada: That glass won't hold up if they / keep pushing at it!"  [LEBT sub02@0xE10 Message_on]
  - 30E0 #9: "Leon: Marvin!! Are you okay!?"  [LEBT sub03@0xEBE Message_on]
  - 30E0 #10: "Leon: That wound... Did those things / get you? How careless..."  [LEBT sub03@0xED6 Message_on]
  - 30E0 #11: "Marvin: The hall is no good...! Those / things broke through the main gate / and got inside!"  [LEBT sub03@0xEFE Message_on]
  - 30E0 #12: "Leon: Say what!?"  [LEBT sub03@0xF36 Message_on]
  - 30E0 #13: "Marvin: The entire place is overrun! / And if they're already all the way out / here... the surface might just be done for..."  [LEBT sub03@0xF5A Message_on]
  - 30E0 #14: "It's too dangerous outside...We need to hurry and get that / elevator working."  [LEBT sub03@0xE72 Aot_reset0-sce1]
  - 30E0 #15: "There's a Flamethrower here.This weapon is not available / in this preview."  [LEBT sub04@0xFEE Message_on]
Port 30E1 (Elza):
  - 30E1 #0: "Elza: John!"  [LEBT sub02@0xB9A Message_on]
  - 30E1 #1: "John: Elza! So this is where you were?"  [LEBT sub02@0xBBE Message_on]
  - 30E1 #2: "John: This is bad... one of those monsters / bit me as I tried to get away!"  [LEBT sub02@0xBF2 Message_on]
  - 30E1 #3: "John: ...And there doesn't seem to be a / place with any medical supplies here..."  [LEBT sub02@0xC1A Message_on]
  - 30E1 #4: "John: Elza, how are things on your end? / Did you find any tasty food or something?"  [LEBT sub02@0xC32 Message_on]
  - 30E1 #5: "Elza: No, nothing like that..."  [LEBT sub02@0xC6A Message_on]
  - 30E1 #6: "John: Wh-what's wrong? Are those / walking corpses over there as well..."  [LEBT sub02@0xC98 Message_on]
  - 30E1 #7: "Elza: No... not at all... That's..."  [LEBT sub02@0xCD0 Message_on]
  - 30E1 #8: "John: What is that...?"  [LEBT sub02@0xCF4 Message_on]
  - 30E1 #9: "John: What are you talking about? / Is there something in there?"  [LEBT sub02@0xD0C Message_on]
  - 30E1 #10: "Elza: I'm not sure what it is, but... / That can't be... human, can it...?"  [LEBT sub02@0xD44 Message_on]
  - 30E1 #11: "John: Elza, is that thing still in there?"  [LEBT sub02@0xD7C Message_on]
  - 30E1 #12: "Elza: Yes, I think so... But I believe / it got scared and hid when it was / attacked..."  [LEBT sub02@0xDA0 Message_on]
  - 30E1 #13: "Elza: I don't know where it could've / gone, though."  [LEBT sub02@0xDCC Message_on]
  - 30E1 #14: "Elza: ...Anyway, there are too many / walking corpses outside. We can't / really turn back."  [LEBT sub02@0xDE4 Message_on]
  - 30E1 #15: "Elza: We have no choice but to go / inside..."  [LEBT sub02@0xE06 Message_on]
  - 30E1 #16: "John: But... that beast might still be in / there, right?"  [LEBT sub02@0xE2A Message_on]
  - 30E1 #17: "Elza: We can probably deal with it if / we stay alert... Let's go!"  [LEBT sub02@0xE62 Message_on]
  - 30E1 #18: "It's too dangerous outside...We need to hurry and get that / elevator working."  [LEBT sub02@0xB6A Aot_reset0-sce1]

### Raum 400 — Eingangshalle des Labors
Disc (TEXTE_40.md):
  - D400 #0 [Leon | System] ドアはロックされている / パネルにひょうじが出ている ▼ ”エマージェンシー発令によりへいさ / ロックかい除にはIDカードが必要です”  =  Die Tür ist verriegelt. Auf dem Panel steht: „Wegen Notfall-Ausrufung gesperrt. Zum Entriegeln ist eine ID-Karte nötig.“
  - D400 #1 [Leon | System] IDカードを使いますか？ [Ja/Nein]  =  ID-Karte benutzen?
  - D400 #2 [Leon | System] ”IDかく認 ロックかい除します”  =  „ID bestätigt, entriegle.“
  - D400 #3 [Leon | Leon] / エイダ ここは…？  =  Ada, was ist das hier…?
  - D400 #4 [Leon | Ada] / いいから急げ！ こっちの部屋だ！  =  Egal, beeil dich! Hier in den Raum!
  - D400 #5 [Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) | Leon] / 先に行け！ 俺がかたづける！  =  Geht vor! Ich erledige das!
  - D400 #6 [Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) | Ada] / レオン…！  =  Leon…!
  - D400 #7 [Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) | Leon] / いいからはやく！  =  Egal, schnell!
  - D400 #8 [Leon, tot (`ROOM4000 sub16 @0x18D8` ohne Aufrufer) | Leon] / マーヴィンを頼む！  =  Kümmere dich um Marvin!
  - D400 #9 [Elza | Elza] / 何？ ここは…!?  =  Was? Was ist das hier…!?
  - D400 #10 [Elza | John] / こいつはすげぇな…  =  Nicht schlecht…
  - D400 #11 [Elza | Elza] / シェリー ここはどこなの？  =  Sherry, wo sind wir hier?
  - D400 #12 [Elza | Elza] / あなたこんな所まで知っているの？  =  Du kennst sogar diesen Ort?
  - D400 #13 [Elza | Sherry] ここはパパの研究所よ / どこかにパパがいるはずだわ  =  Das ist Papas Forschungslabor. Irgendwo muss Papa sein.
  - D400 #14 [Elza | Sherry] / 私 捜してくる！  =  Ich suche ihn!
  - D400 #15 [Elza | Elza] / ちょっとシェリー！  =  He, Sherry!
  - D400 #16 [Elza | Elza] / ダメよ一人じゃ！  =  Nicht allein!
  - D400 #17 [Elza | Elza] / シェリー！  =  Sherry!
  - D400 #18 [Elza | John] / エルザ おうぞ！  =  Elza, hinterher!
Port 4000 (Leon):
  - 4000 #0: "The door is locked. The text on the / panel reads:Door is under emergency lockdown. / An ID card is required to unlock it.""  [LEBT main00@0x112C AOT6-sce1 wenn Ck(3,101)=0; LEBT sub02@0x143A Message_on wenn Ck(3,32)=0]
  - 4000 #1: "Will you use the Blue Master Keycard? "  [LEBT sub02@0x1440 Message_on wenn Ck(3,32)=0]
  - 4000 #2: "You've used the Blue Master Keycard."  [LEBT sub02@0x144A Message_on wenn Ck(3,32)=0]
  - 4000 #3: "Leon: Ada, what is this place...?"  [LEBT sub05@0x15C4 Message_on]
  - 4000 #4: "Ada: Just hurry up, okay?! / Let's get him to this room here!"  [LEBT sub05@0x160A Message_on]
  - 4000 #5: "Leon: Go on ahead! / I'll take care of this!"  [LEBT sub09@0x1904 Message_on]
  - 4000 #6: "Ada: Leon...!"  [LEBT sub09@0x1936 Message_on]
  - 4000 #7: "Leon: Just get going already!"  [LEBT sub09@0x1952 Message_on]
  - 4000 #8: "Leon: Take care of Marvin!"  [LEBT sub09@0x196E Message_on]
  - 4000 #9: "An elevator. Pressing the / button has no effect.It hasn't been used for a / long time."  [LEBT main00@0x1062 AOT0-sce1 wenn Ck(3,78)=0]
Port 4001 (Elza):
  - 4001 #0: "The door is locked. The text on the / panel reads:Door is under emergency lockdown. / An ID card is required to unlock it.""  [kein Aufruf]
  - 4001 #1: "Will you use the Blue Master Keycard? "  [kein Aufruf]
  - 4001 #2: "You've used the Blue Master Keycard."  [kein Aufruf]
  - 4001 #3: "Elza: What? What is this place...!?"  [LEBT sub10@0x18C0 Message_on]
  - 4001 #4: "John: Wow, this sure is something..."  [LEBT sub10@0x1908 Message_on]
  - 4001 #5: "Elza: Sherry, what is this place?"  [LEBT sub10@0x1966 Message_on]
  - 4001 #6: "John: Do you know anything about / this place?"  [LEBT sub10@0x198A Message_on]
  - 4001 #7: "Sherry: This is dad's research center. / He should be around here somewhere."  [LEBT sub10@0x19AE Message_on]
  - 4001 #8: "Sherry: I'll go look for him!"  [LEBT sub10@0x19CA Message_on]
  - 4001 #9: "John: Sherry, hold on!"  [LEBT sub10@0x1A24 Message_on]
  - 4001 #10: "John: Don't go alone!"  [LEBT sub10@0x1A3C Message_on]
  - 4001 #11: "Elza: Sherry!"  [LEBT sub10@0x1A54 Message_on]
  - 4001 #12: "John: Elza, let's go after her!"  [LEBT sub10@0x1AC8 Message_on]
  - 4001 #13: "An elevator. Pressing the / button has no effect.It hasn't been used for a / long time."  [LEBT main00@0x104A AOT0-sce1 wenn Ck(3,78)=0]
  - 4001 #14: "Security Room"It's locked."  [LEBT main00@0x110C AOT8-sce1]

### Raum 401 — Laborbüro mit Liege und Monitoren
Disc (TEXTE_40.md):
  - D401 #0 [Leon | Ada] / レオン この傷… / 急がないとまずい  =  Leon, diese Wunde… wenn wir uns nicht beeilen, wird es schlimm.
  - D401 #1 [Leon | Leon] / エイダ…君は一体何者だ？ / まさか…  =  Ada… wer bist du eigentlich? Etwa…
  - D401 #2 [Leon | Ada] / そうだ 私はアンブレラの人間だ  =  Ja. Ich gehöre zu Umbrella.
  - D401 #3 [Leon | Ada] / レオン…今は話している時間は無い / 急がないとこいつは死んでしまうぞ  =  Leon… jetzt ist keine Zeit zum Reden. Wenn wir uns nicht beeilen, stirbt er.
  - D401 #4 [Leon | Leon] / どうすればいいんだ？  =  Was soll ich tun?
  - D401 #5 [Leon | Ada] やつらのすがたはウィルスによるものだ / ワクチンさえうてば 発びょうはしない  =  Deren Zustand kommt vom Virus. Mit einer Impfung bricht es nicht aus.
  - D401 #6 [Leon | Leon] / そのワクチンはどこに？  =  Wo ist der Impfstoff?
  - D401 #7 [Leon | Ada] ワクチン自体はここにはないが / ”ウィルスかぶ”から作ることは出来る / しかし…  =  Der Impfstoff selbst ist nicht hier, aber man kann ihn aus dem „Virusstamm“ herstellen. Aber…
  - D401 #8 [Leon | Leon] / どうした？ エイダ？  =  Was ist, Ada?
  - D401 #9 [Leon | Ada] / レオン 私はきょう力出来ない  =  Leon, ich kann nicht helfen.
  - D401 #10 [Leon | Leon] / 何だって！？  =  Was!?
  - D401 #11 [Leon | Ada] / やらなければならない事がある お前たちにはいろいろと助けられたが / これだけは私の手で！  =  Ich habe etwas zu erledigen. Ihr habt mir viel geholfen, aber das muss ich selbst tun!
  - D401 #12 [Leon | Leon] / 俺は… / 一人でもやるぜ！  =  Ich… mache es notfalls allein!
  - D401 #13 [Leon | Leon] / おしえてくれ！ どうすればいい？  =  Sag mir, wie!
  - D401 #14 [Leon | Ada] この研究所のちょうていおん実験室に / ウィルスかぶが保存されているはずだ それを装置にセットしてプログラムを / はしらせば後は自動でやってくれる だが ウィルスかぶは / ひどくデリケートだ 取りあつかいにちゅう意しなくては / 全てがダメになるぞ  =  Im Ultratiefkühl-Labor hier muss der Virusstamm lagern. Setz ihn ins Gerät, starte das Programm, der Rest läuft automatisch. Aber der Stamm ist äußerst empfindlich. Geh falsch damit um, und alles ist hin.
  - D401 #15 [Leon | Leon] / やるしかない / マーヴィン… 必ず助けてやる  =  Es bleibt nichts anderes. Marvin… ich rette dich.
  - D401 #16 [Leon | Ada] レオン 全てがおわったら / 地下のシェルターに行け！ / でないといのちの保証は出来ない ここはアンブレラの研究所だ / くれぐれも気をつけろよ  =  Leon, wenn alles vorbei ist, geh zum unterirdischen Schutzraum! Sonst kann ich für dein Leben nicht garantieren. Das hier ist ein Umbrella-Labor, sei bloß vorsichtig.
  - D401 #17 [Leon | Leon] / おい エイダ！ / いのちの保証ができない…？  =  He, Ada! Nicht für mein Leben garantieren…?
  - D401 #18 [Leon + Elza | Spielfigur, Raumtext in 401 und 501] / 急いでワクチンを持ってこなければ！  =  Ich muss den Impfstoff schnell holen!
  - D401 #19 [Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) | Leon] / どうした？ マーヴィン目をさませ！！  =  Was ist? Marvin, wach auf!!
  - D401 #20 [Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) | Leon] / エイダ！  =  Ada!
  - D401 #21 [Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) | Ada] レオン！ 何をしている！ / ここもウィルスにやられるぞ！  =  Leon! Was machst du! Das Virus erreicht auch diesen Ort!
  - D401 #22 [Leon, tot (`ROOM4010 sub06 @0x908` ohne Aufrufer) | Ada] / シェルターへ急ぐぞ！  =  Schnell zum Schutzraum!
  - D401 #23 [Leon + Elza | System] / モニターが並んでいる ▼ 電力が来ていないのか / 完全に死んでしまっている  =  Eine Reihe Monitore. Kein Strom offenbar, völlig tot.
  - D401 #24 [Leon + Elza | System] / 机の上には何も置かれていない  =  Auf dem Tisch liegt nichts.
  - D401 #25 [Leon + Elza | System] / この装置は使われていなかったようだ  =  Dieses Gerät wurde wohl nicht benutzt.
  - D401 #26 [Leon + Elza | System] / メモがはられている ▼ マイケルへ / 左の机の上にIDカードを置いておく ▼ 今度こそ無くすなよ / ヘンリー  =  Eine Notiz: „An Michael: Ich lege die ID-Karte auf den linken Tisch. Verlier sie diesmal nicht. Henry“
  - D401 #27 [Leon + Elza | System] / このロッカーには別に何も無い  =  In diesem Spind ist nichts Besonderes.
  - D401 #28 [Leon + Elza | System] このフリーザーも電力が来ていない / 少しつんとしたイヤなにおいがする  =  Auch dieser Gefrierschrank hat keinen Strom. Es riecht unangenehm stechend.
Port 4010 (Leon):
  - 4010 #0: "Ada: Leon, that wound..."  [LEBT sub02@0x764 Message_on]
  - 4010 #1: "Leon: Yeah, we need to hurry."  [LEBT sub02@0x7AE Message_on]
  - 4010 #2: "Leon: Ada... Just who are you, anyway?"  [LEBT sub02@0x7CA Message_on]
  - 4010 #3: "Leon: Don't tell me..."  [LEBT sub02@0x7F6 Message_on]
  - 4010 #4: "Ada: That's right. / I'm with Umbrella."  [LEBT sub02@0x816 Message_on]
  - 4010 #5: "Ada: Leon... There's no time to talk / right now."  [LEBT sub02@0x888 Message_on]
  - 4010 #6: "Ada: If we don't hurry, he's going / to die."  [LEBT sub02@0x8BE Message_on]
  - 4010 #7: "Leon: What should we do?"  [LEBT sub02@0x8F2 Message_on]
  - 4010 #8: "Ada: It's a virus that makes people / turn into those things. If we can / get him the vaccine, it won't break out."  [LEBT sub02@0x916 Message_on]
  - 4010 #9: "Leon: And where's this vaccine?"  [LEBT sub02@0x94E Message_on]
  - 4010 #10: "Ada: The vaccine itself isn't here, / but we can make one from a strain of / the virus."  [LEBT sub02@0x972 Message_on]
  - 4010 #11: "Ada: However..."  [LEBT sub02@0x9A2 Message_on]
  - 4010 #12: "Leon: Ada? What's wrong?"  [LEBT sub02@0x9C2 Message_on]
  - 4010 #13: "Ada: Leon, I can't help you."  [LEBT sub02@0xA20 Message_on]
  - 4010 #14: "Leon: Wait, what!?"  [LEBT sub02@0xA44 Message_on]
  - 4010 #15: "Ada: There's something I have to do."  [LEBT sub02@0xA8C Message_on]
  - 4010 #16: "Ada: You've helped me out a lot, but / I have to do this myself! "  [LEBT sub02@0xAAC Message_on]
  - 4010 #17: "Leon: I'll..."  [LEBT sub02@0xAE0 Message_on]
  - 4010 #18: "Leon: ...do it on my own if I have to!"  [LEBT sub02@0xAFC Message_on]
  - 4010 #19: "Leon: So, please tell me! / What should I do!?"  [LEBT sub02@0xB14 Message_on]
  - 4010 #20: "Ada: The viral strain should be stored / in this research center's ultra-low / temperature laboratory."  [LEBT sub02@0xB48 Message_on]
  - 4010 #21: "Ada: As long as you can start up the / program for it, it should handle the / rest automatically."  [LEBT sub02@0xB64 Message_on]
  - 4010 #22: "Ada: However, the Virus Inhibitor is / very delicate."  [LEBT sub02@0xB90 Message_on]
  - 4010 #23: "Ada: If you don't handle it with / extreme care, it'll all be for naught."  [LEBT sub02@0xBA8 Message_on]
  - 4010 #24: "Leon: Well, I have to give it a go."  [LEBT sub02@0xBDC Message_on]
  - 4010 #25: "Leon: Marvin... I'll save you!"  [LEBT sub02@0xC26 Message_on]
  - 4010 #26: "Ada: Leon. When all this is over, / head to the underground shelter!"  [LEBT sub02@0xC52 Message_on]
  - 4010 #27: "Ada: If you don't, you probably won't / survive."  [LEBT sub02@0xCB0 Message_on]
  - 4010 #28: "Ada: And keep in mind that this is an / Umbrella research facility. Be careful."  [LEBT sub02@0xCDC Message_on]
  - 4010 #29: "Leon: Ada, wait!"  [LEBT sub02@0xD52 Message_on]
  - 4010 #30: "Leon: I probably won't survive...?"  [LEBT sub02@0xD6A Message_on]
  - 4010 #31: "I need to hurry up and get that / vaccine!"  [LEBT main00@0x68C AOT12-sce1 wenn Ck(3,72)=0]
  - 4010 #32: "Leon: What's wrong, Marvin? Open your / eyes!!"  [LEBT sub04@0xDF2 Message_on]
  - 4010 #33: "Leon: Ada!"  [LEBT sub04@0xE52 Message_on]
  - 4010 #34: "Ada: Leon! What are you doing!? The / virus is on the loose here as well!"  [LEBT sub04@0xED6 Message_on]
  - 4010 #35: "Ada: Hurry to the shelter!"  [LEBT sub04@0xF14 Message_on]
  - 4010 #36: "It's a row of monitors.They're completely blank, maybe / they're not getting any power?"  [LEBT main00@0x506 AOT2-sce1]
  - 4010 #37: " / The desk is completely empty."  [LEBT main00@0x51A AOT3-sce1]
  - 4010 #38: "This equipment seems to be unusable."  [LEBT main00@0x52E AOT4-sce1]
  - 4010 #39: "There's a memo here.To Michael: I left an ID card on the / right desk.Don't lose it again, okay? - Henry"  [LEBT main00@0x542 AOT5-sce1]
  - 4010 #40: " / There's nothing useful in this locker."  [LEBT main00@0x556 AOT6-sce1]
  - 4010 #41: "This freezer isn't on. It smells pretty / bad."  [LEBT main00@0x56A AOT7-sce1]
  - 4010 #42: "Itembox is not available in / this preview."  [LEBT sub07@0xF94 Message_on]
  - 4010 #43: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT sub08@0xFA2 Message_on]
Port 4011 (Elza):
  - 4011 #0: "It's a row of monitors.They're completely blank, maybe / they're not getting any power?"  [LEBT main00@0x4F2 AOT1-sce1]
  - 4011 #1: " / The desk is completely empty."  [LEBT main00@0x506 AOT2-sce1]
  - 4011 #2: " / This equipment seems to be unusable."  [LEBT main00@0x51A AOT3-sce1]
  - 4011 #3: "There's a memo here.To Michael: I left an ID card on the / right desk.Don't lose it again, okay? - Henry"  [LEBT main00@0x52E AOT4-sce1]
  - 4011 #4: " / There's nothing useful in this locker."  [LEBT main00@0x542 AOT5-sce1]
  - 4011 #5: "This freezer isn't on. It smells pretty / bad."  [LEBT main00@0x556 AOT6-sce1]
  - 4011 #6: "Itembox is not available in / this preview."  [LEBT sub02@0x636 Message_on]
  - 4011 #7: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT sub03@0x644 Message_on]

### Raum 403 — Achteckige Schalthalle
Disc (TEXTE_40.md):
  - D403 #0 [Leon + Elza | System] ”メインヒューズ” / 大がかりなブレーカーシステムだ / ▼ 誰かが作動させたらしく / ヒューズ部分が全て外れている /  =  „Hauptsicherung“. Eine große Schalteranlage. Jemand hat sie wohl ausgelöst, alle Sicherungen sind herausgesprungen.
  - D403 #1 [Leon + Elza | System] リペアープラグを使いますか？ / [Ja/Nein]  =  Reparaturstecker benutzen?
  - D403 #2 [Leon + Elza | System] ヒューズはすべて接続されている / 今まで行けなかった所にも行けるはずだ /  =  Alle Sicherungen sind verbunden. Jetzt müsste man auch dorthin kommen, wo es bisher nicht ging.
  - D403 #3 [Leon + Elza | System] このフットランプのおかげで / 下の方だけはかろうじて見える /  =  Dank dieser Bodenlampen sieht man unten gerade noch etwas.
  - D403 #4 [Leon + Elza | System] ときおりランプの光が / 品まっているようだが・・・ /  =  Das Lampenlicht scheint zeitweise schwächer zu werden… [Lesart]
  - D403 #5 [Elza. In 4030 tot (`@0x52C`) | Elza] シェリー！ /  =  Sherry!
  - D403 #6 [Elza. In 4030 tot (`@0x52C`) | John] 何てこった！ どこへ行ったんだ？ /  =  Verdammt! Wo ist sie hin?
  - D403 #7 [Elza. In 4030 tot (`@0x52C`) | Elza] 手分けして捜しましょう！ /  =  Teilen wir uns auf und suchen!
  - D403 #8 [Elza. In 4030 tot (`@0x52C`) | John] OK！ 俺はこっちだ / エルザは向こうを頼むぜ /  =  OK! Ich gehe hier lang, Elza, du dort!
Port 4030 (Leon):
  - 4030 #0: "Main Fuse" / It's a large circuit breaker system.It looks like someone has removed all / the fuses."  [LEBT main00@0x4FE AOT6-sce1 wenn Ck(3,87)=0; LEBT sub02@0x62C Message_on]
  - 4030 #1: "Will you use the Spark Plug? "  [LEBT sub02@0x63A Message_on wenn Ck(5,3)=1]
  - 4030 #2: "The fuses are all connected. I should / be able to get to some new places now."  [LEBT sub05@0x680 Message_on]
  - 4030 #3: "Thanks to the footlights, I can just / barely see the ground."  [LEBT sub03@0x668 Message_on]
  - 4030 #4: "The lamps seem to flicker now and / then, but..."  [LEBT sub04@0x674 Message_on]
Port 4031 (Elza):
  - 4031 #0: "Main Fuse" / It's a large circuit breaker system.It looks like someone has removed all / the fuses."  [LEBT main00@0x4FE AOT6-sce1 wenn Ck(3,87)=0; LEBT sub02@0x66E Message_on]
  - 4031 #1: "Will you use the Spark Plug? "  [LEBT sub02@0x67C Message_on wenn Ck(5,3)=1]
  - 4031 #2: "The fuses are all connected. I should / be able to get to some new places now."  [LEBT sub05@0x6C2 Message_on]
  - 4031 #3: "Thanks to the footlights, I can just / barely see the ground."  [LEBT sub03@0x6AA Message_on]
  - 4031 #4: "The lamps seem to flicker now and / then, but..."  [LEBT sub04@0x6B6 Message_on]
  - 4031 #5: "John: Sherry!"  [LEBT sub08@0x7BE Message_on]
  - 4031 #6: "John: What the? / Where did she go!?"  [LEBT sub08@0x7C6 Message_on]
  - 4031 #7: "Elza: Let's split up and look for / her!"  [LEBT sub08@0x7F4 Message_on]
  - 4031 #8: "John: Okay! I'll go this way, you / go that way, Elza!"  [LEBT sub08@0x818 Message_on]

### Raum 40A — Medizinraum mit CT
Disc (TEXTE_40.md):
  - D40A #0 [Leon + Elza | System] スイッチがある・・・押しますか？ / [Ja/Nein]  =  Da ist ein Schalter… Drücken?
  - D40A #1 [Leon + Elza | System] “火気げんきん”のマーキングがある / 危険なもののようだ  =  Markierung „Feuer verboten“. Offenbar etwas Gefährliches.
  - D40A #2 [Leon + Elza | System] 救急用のベッドだ / あまり使った跡が無い  =  Ein Notfallbett, kaum benutzt.
  - D40A #3 [Leon + Elza | System] げん重にロックされている / 開けるにはIDカードが必要だ  =  Streng verriegelt. Zum Öffnen braucht man eine ID-Karte.
  - D40A #4 [nicht ausgelöst | System] [研究所のIDカード]を使いますか [Ja/Nein]  =  [ID-Karte des Labors] benutzen?
  - D40A #5 [Leon + Elza | System] / CTスキャンのたぐいだ ▼ 研究員の体調をかんりするためだろう / ひんぱんに作動していたようだ  =  Eine Art CT-Scanner. Wohl zur Gesundheitskontrolle der Forscher, wurde häufig benutzt.
  - D40A #6 [Leon + Elza | System] / 棚があるが中は暗くて分からない  =  Ein Regal, innen zu dunkel, um etwas zu erkennen.
  - D40A #7 [Leon + Elza | System] / 手さぐりで調べるのは危険だ  =  Blind herumzutasten wäre gefährlich.
  - D40A #8 [Leon + Elza | System] / 暗くてよく分からない  =  Zu dunkel, nichts zu erkennen.
  - D40A #9 [Elza | System] / 暗すぎてよく見えない  =  Viel zu dunkel.
  - D40A #10 [Leon + Elza | System] / 手術台のようだが暗くてよく分からない  =  Offenbar ein OP-Tisch, aber zu dunkel, um Genaues zu sehen.
  - D40A #11 [Leon + Elza | System] / 血のようなものがこびりついている  =  Etwas wie Blut klebt daran.
Port 40A0 (Leon):
  - 40A0 #0: "There's a switch here... Press it? /  "  [LEBT sub02@0xFBA Message_on]
  - 40A0 #1: "There's a Flammable" mark on it. / Looks dangerous."  [LEBT main00@0xE26 AOT5-sce1]
  - 40A0 #2: "The infirmary bed shows no signs of / recent use."  [LEBT main00@0xE3A AOT6-sce1]
  - 40A0 #3: "It's securely locked. An ID card is / required to open it."  [LEBT sub03@0x106E Message_on]
  - 40A0 #4: "Use the Research Center ID Card? "  [kein Aufruf]
  - 40A0 #5: " / It's a CT Scanner.The researchers here must be rather / unhealthy, because it looks like it's / seen a lot of use."  [LEBT sub05@0x109C Message_on wenn Cmp(V[10] op5 4)]
  - 40A0 #6: " / It's too dark to see what's on the shelf."  [LEBT main00@0xED4 Aot_reset7-sce1 wenn Ck(4,19)=0; LEBT sub02@0x1046 Aot_reset7-sce1 wenn Ck(12,31)=0 / NICHT[Ck(4,19)=0]]
  - 40A0 #7: " / It's too dangerous to dig around with / my bare hands."  [LEBT main00@0xEDE Aot_reset8-sce1 wenn Ck(4,19)=0; LEBT sub02@0x1050 Aot_reset8-sce1 wenn Ck(12,31)=0 / NICHT[Ck(4,19)=0]]
  - 40A0 #8: " / It's too dark to tell what it is."  [LEBT main00@0xEE8 Aot_reset9-sce1 wenn Ck(4,19)=0; LEBT sub02@0x105A Aot_reset9-sce1 wenn Ck(12,31)=0 / NICHT[Ck(4,19)=0]]
  - 40A0 #9: " / It's too dark to see anything."  [kein Aufruf]
  - 40A0 #10: "An operating table? It's too dark to / tell for sure."  [LEBT sub04@0x107E Message_on wenn Ck(4,19)=0]
  - 40A0 #11: " / There's what seems to be blood on it..."  [LEBT sub04@0x1088 Message_on wenn NICHT[Ck(4,19)=0]]
  - 40A0 #12: "There is one U.S.S. armor / that should fit you.Will you equip it? "  [LEBT sub06@0x10AE Message_on wenn Ck(3,122)=0]
  - 40A0 #13: "Will you unequip the armor?  "  [LEBT sub06@0x10B8 Message_on wenn NICHT[Ck(3,122)=0]]
Port 40A1 (Elza):
  - 40A1 #0: "There's a switch here... Press it? /  "  [LEBT sub02@0xFBA Message_on]
  - 40A1 #1: "There's a Flammable" mark on it. / Looks dangerous."  [LEBT main00@0xE26 AOT5-sce1]
  - 40A1 #2: "The infirmary bed shows no signs of / recent use."  [LEBT main00@0xE3A AOT6-sce1]
  - 40A1 #3: "It's securely locked. An ID card is / required to open it."  [LEBT sub03@0x106E Message_on]
  - 40A1 #4: "Use the Research Center ID Card? "  [kein Aufruf]
  - 40A1 #5: " / It's a CT Scanner.The researchers here must be rather / unhealthy, because it looks like it's / seen a lot of use."  [LEBT sub05@0x109C Message_on wenn Cmp(V[10] op5 4)]
  - 40A1 #6: " / It's too dark to see what's on the shelf."  [LEBT main00@0xED4 Aot_reset7-sce1 wenn Ck(4,19)=0; LEBT sub02@0x1046 Aot_reset7-sce1 wenn Ck(12,31)=0 / NICHT[Ck(4,19)=0]]
  - 40A1 #7: " / It's too dangerous to dig around with / my bare hands."  [LEBT main00@0xEDE Aot_reset8-sce1 wenn Ck(4,19)=0; LEBT sub02@0x1050 Aot_reset8-sce1 wenn Ck(12,31)=0 / NICHT[Ck(4,19)=0]]
  - 40A1 #8: " / It's too dark to tell what it is."  [LEBT main00@0xEE8 Aot_reset9-sce1 wenn Ck(4,19)=0; LEBT sub02@0x105A Aot_reset9-sce1 wenn Ck(12,31)=0 / NICHT[Ck(4,19)=0]]
  - 40A1 #9: " / It's too dark to see anything."  [kein Aufruf]
  - 40A1 #10: "An operating table? It's too dark to / tell for sure."  [LEBT sub04@0x107E Message_on wenn Ck(4,19)=0]
  - 40A1 #11: " / There's what seems to be blood on it..."  [LEBT sub04@0x1088 Message_on wenn NICHT[Ck(4,19)=0]]
  - 40A1 #12: "There is one U.S.S. armor / that should fit you.Will you equip it? "  [LEBT sub06@0x10AE Message_on wenn Ck(3,122)=0]
  - 40A1 #13: "Will you unequip the armor?  "  [LEBT sub06@0x10B8 Message_on wenn NICHT[Ck(3,122)=0]]

### Raum 501 — Computerraum
Disc (TEXTE_40.md):
  - D501 #0 [Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) | John] / シェリー しっかりしな！  =  Sherry, halt durch!
  - D501 #1 [Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) | John] / だめだ… 気ぃ失なってるぜ  =  Nichts zu machen… sie ist bewusstlos.
  - D501 #2 [Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) | Elza] / あぁ… シェリー  =  Ach… Sherry.
  - D501 #3 [Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) | Elza] ジョン この研究所のファイルを見てて / おそろしい事が分かったの  =  John, in den Akten dieses Labors habe ich etwas Furchtbares gefunden.
  - D501 #4 [Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) | Elza] この子の体は ウィルスに / むしばまれているのよ…  =  Der Körper dieses Kindes wird vom Virus zerfressen…
  - D501 #5 [Elza, tot (`ROOM5011 sub02 @0x4A0` ohne Aufrufer) | John] / エルザ こいつを見てみな  =  Elza, sieh dir das an.
  - D501 #6 [nicht ausgelöst | Elza] / 何て事…  =  Wie furchtbar…
  - D501 #7 [nicht ausgelöst | John] / ワクチンをうたねぇとシェリーは…  =  Ohne Impfung wird Sherry…
  - D501 #8 [nicht ausgelöst | Elza] / いいわ 私が行く  =  Gut, ich gehe.
  - D501 #9 [nicht ausgelöst | John] / 何？  =  Was?
  - D501 #10 [nicht ausgelöst | Elza] こうしている間にもシェリーの体は / ウィルスにむしばまれているわ  =  Auch jetzt gerade wird Sherrys Körper vom Virus zerfressen.
  - D501 #11 [nicht ausgelöst | Elza] / イチかバチか やってみるしかないわ  =  Alles oder nichts, ich muss es versuchen.
  - D501 #12 [nicht ausgelöst | John] / エルザ…  =  Elza…
  - D501 #13 [nicht ausgelöst | Elza] / ジョン しっかりみててね  =  John, pass gut auf sie auf.
  - D501 #14 [nicht ausgelöst | Elza] / 今のシェリーにはあなたが必要よ  =  Sherry braucht dich jetzt.
  - D501 #15 [nicht ausgelöst | John] / 頼むぜエルザ  =  Ich verlasse mich auf dich, Elza.
  - D501 #16 [nicht ausgelöst | John] / この子だけはぜっ対助けてやりてぇ  =  Dieses Kind will ich unbedingt retten.
  - D501 #17 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / ジョン ワクチンよ！  =  John, der Impfstoff!
  - D501 #18 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / シェリーは？  =  Was ist mit Sherry?
  - D501 #19 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] / はやく！ もう長いことうなされてる  =  Schnell! Sie stöhnt schon lange im Schlaf.
  - D501 #20 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / シェリー！ 目を開けて！  =  Sherry! Mach die Augen auf!
  - D501 #21 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] シェリー！ さぁどうした / へん事をしてくれ！  =  Sherry! Komm schon, antworte!
  - D501 #22 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / お願い… シェリー 起きて…  =  Bitte… Sherry, wach auf…
  - D501 #23 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Sherry] / どうしたの？  =  Was ist denn?
  - D501 #24 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] / シェリー！ 気がついたか！  =  Sherry! Du bist wach!
  - D501 #25 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / よかった…  =  Gott sei Dank…
  - D501 #26 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / 本当 心配したわよ  =  Du hast uns wirklich Sorgen gemacht.
  - D501 #27 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / ジョン アナウンスは聞いた？  =  John, hast du die Durchsage gehört?
  - D501 #28 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] / あぁ 一体何が起こってんだ？  =  Ja. Was ist hier eigentlich los?
  - D501 #29 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / 怪物がこのしせつの動力を破壊したのよ  =  Das Monster hat die Energieversorgung der Anlage zerstört.
  - D501 #30 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / ばく発でウィルスがばらまかれるわ  =  Bei der Explosion wird das Virus freigesetzt.
  - D501 #31 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] / 何だって!?それじゃぁ俺たちもやばいぜ  =  Was!? Dann sind wir auch dran!
  - D501 #32 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] そうよ ここはまずいわ / はやくシェルターへ行きましょう！  =  Genau, hier ist es gefährlich. Schnell zum Schutzraum!
  - D501 #33 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Elza] / シェリー シェルターは分かる？  =  Sherry, kennst du den Schutzraum?
  - D501 #34 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Sherry] / う、うん  =  J-ja.
  - D501 #35 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | Sherry] / 2号シェルターなら開けられるわ  =  Schutzraum Nr. 2 kann ich öffnen.
  - D501 #36 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] / よし じゃぁ行こうぜ  =  Gut, dann los.
  - D501 #37 [Elza, tot (`ROOM5011 sub06 @0x57A` ohne Aufrufer) | John] / シェリー よろしく頼むぜ  =  Sherry, ich verlasse mich auf dich.
  - D501 #38 [Leon + Elza | System] / あわてて中をひっかき回した跡がある ▼ / 役に立ちそうなものは無い  =  Hier wurde hastig alles durchwühlt. Nichts Brauchbares.
  - D501 #39 [Leon + Elza | System] / 机の上はひどくほこりっぽい ▼ / 特に何も無い  =  Der Tisch ist staubig. Nichts Besonderes.
  - D501 #40 [Leon + Elza | System] / 少し前まで使われていたようだ  =  Bis vor Kurzem wurde das noch benutzt.
  - D501 #41 [Leon + Elza | System] さまざまな機器の後ろに / メモなどがはられたボードがある ▼ / 研究所内での仕事のおぼえ書きのようだ  =  Hinter den Geräten hängt eine Tafel mit Notizen. Offenbar Arbeitsvermerke des Labors.
  - D501 #42 [Leon + Elza | System] ゴミばこの中にはくしゃくしゃの / 紙くずが無造作に放りこまれている  =  Im Papierkorb liegen achtlos zerknüllte Zettel.
  - D501 #43 [Leon + Elza | Spielfigur, Raumtext in 401 und 501] / 急いでワクチンを持ってこなければ！  =  Ich muss den Impfstoff schnell holen!
Port 5010 (Leon):
  - 5010 #0: "There's a bunch of footprints going / in circles.There doesn't seem to be anything / useful here."  [LEBT main00@0x3BA AOT1-sce1]
  - 5010 #1: " / This desk is very dusty.There's nothing useful on it."  [LEBT main00@0x3CE AOT2-sce1]
  - 5010 #2: "It looks like this was used a little / while ago."  [kein Aufruf]
  - 5010 #3: "There's a memo board behind a bunch / of machinery.It seems to be mostly notes written / down by the research staff."  [LEBT main00@0x3F6 AOT4-sce1]
  - 5010 #4: "The trashcan is full of crumpled / up papers."  [LEBT main00@0x40A AOT5-sce1]
  - 5010 #5: "Itembox is not available in / this preview."  [LEBT sub02@0x4EA Message_on]
  - 5010 #6: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT sub03@0x4F8 Message_on]
Port 5011 (Elza):
  - 5011 #0: "Elza: Sherry! Wake up!"  [LEBT sub02@0x5DA Message_on]
  - 5011 #1: "John: It's no good... / She's passed out."  [LEBT sub02@0x5F2 Message_on]
  - 5011 #2: "Elza: Ahh... Sherry..."  [LEBT sub02@0x624 Message_on]
  - 5011 #3: "Elza: John. I read some files in this / research center, and I found out / some terrifying things."  [LEBT sub02@0x640 Message_on]
  - 5011 #4: "Elza: She's been infected by what / they were working on here."  [LEBT sub02@0x690 Message_on]
  - 5011 #5: "John: Elza, have a look at this."  [LEBT sub02@0x6AC Message_on]
  - 5011 #6: "Elza: What is this...?"  [LEBT sub02@0x72C Message_on]
  - 5011 #7: "John: If we can't get the vaccine, / Sherry will..."  [LEBT sub02@0x750 Message_on]
  - 5011 #8: "Elza: Okay. I'll go get it."  [LEBT sub02@0x788 Message_on]
  - 5011 #9: "John: What?"  [LEBT sub02@0x7AC Message_on]
  - 5011 #10: "Elza: While we're talking, the virus / is spreading inside Sherry's body."  [LEBT sub02@0x7D0 Message_on]
  - 5011 #11: "Elza: Even if it's just a chance, / I have to take it."  [LEBT sub02@0x800 Message_on]
  - 5011 #12: "John: Elza..."  [LEBT sub02@0x834 Message_on]
  - 5011 #13: "Elza: John, keep a close eye on her."  [LEBT sub02@0x858 Message_on]
  - 5011 #14: "Elza: Sherry needs you right now."  [LEBT sub02@0x874 Message_on]
  - 5011 #15: "John: Elza, please..."  [LEBT sub02@0x894 Message_on]
  - 5011 #16: "John: ...Please save this girl, even / if it's just her."  [LEBT sub02@0x8B0 Message_on]
  - 5011 #17: "Elza: John, I've got the vaccine!"  [LEBT sub04@0x9C6 Message_on]
  - 5011 #18: "Elza: How's Sherry?"  [LEBT sub04@0x9E2 Message_on]
  - 5011 #19: "John: Quick! She doesn't have long!"  [LEBT sub04@0x9FA Message_on]
  - 5011 #20: "Elza: Sherry! Wake up!"  [LEBT sub04@0xA36 Message_on]
  - 5011 #21: "John: Sherry, what's wrong!? / Talk to me!"  [LEBT sub04@0xA4E Message_on]
  - 5011 #22: "Elza: Sherry, please... Wake up...!"  [LEBT sub04@0xA7E Message_on]
  - 5011 #23: "John: What's wrong...?"  [LEBT sub04@0xA9A Message_on]
  - 5011 #24: "Elza: Sherry! You're awake!"  [LEBT sub04@0xAC4 Message_on]
  - 5011 #25: "Elza: What a relief..."  [LEBT sub04@0xAF6 Message_on]
  - 5011 #26: "John: Yes, I was really worried!"  [LEBT sub04@0xB1C Message_on]
  - 5011 #27: "Elza: John, did you hear that / announcement?"  [LEBT sub04@0xB46 Message_on]
  - 5011 #28: "John: Yeah. What's going on!?"  [LEBT sub04@0xB76 Message_on]
  - 5011 #29: "Elza: It seems like that beast broke / the place's power core."  [LEBT sub04@0xC0A Message_on]
  - 5011 #30: "Elza: And fire's starting to spread / all over the place."  [LEBT sub04@0xC3A Message_on]
  - 5011 #31: "John: What!? That means we're also / in danger!"  [LEBT sub04@0xC5A Message_on]
  - 5011 #32: "Elza: That's right! We can't stay / here, we need to hurry to the shelter!"  [LEBT sub04@0xC92 Message_on]
  - 5011 #33: "John: Sherry, do you know where the / shelter is?"  [LEBT sub04@0xD04 Message_on]
  - 5011 #34: "Sherry: Y-yes."  [LEBT sub04@0xD3C Message_on]
  - 5011 #35: "Sherry: The second shelter should be / open."  [LEBT sub04@0xD44 Message_on]
  - 5011 #36: "John: Alright, let's get going!"  [LEBT sub04@0xD82 Message_on]
  - 5011 #37: "Elza: Lead the way, Sherry!"  [LEBT sub04@0xDA6 Message_on]
  - 5011 #38: "There's a bunch of footprints going / in circles.There doesn't seem to be anything / useful here."  [LEBT main00@0x3CE AOT2-sce1]
  - 5011 #39: " / This desk is very dusty.There's nothing useful on it."  [LEBT main00@0x3E2 AOT3-sce1]
  - 5011 #40: "It looks like this was used a little / while ago."  [kein Aufruf]
  - 5011 #41: "There's a memo board behind a bunch / of machinery.It seems to be mostly notes written / down by the research staff."  [LEBT main00@0x40A AOT5-sce1]
  - 5011 #42: "The trashcan is full of crumpled / up papers."  [LEBT main00@0x41E AOT6-sce1]
  - 5011 #43: " / I need to hurry up and get that vaccine!"  [LEBT main00@0x508 AOT7-sce1 wenn Ck(3,58)=1]
  - 5011 #44: "Itembox is not available in / this preview."  [LEBT sub07@0xE28 Message_on]
  - 5011 #45: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT sub08@0xE36 Message_on]

### Raum 504 — Blauer Laborgang
Disc (TEXTE_40.md):
  - D504 #0 [Leon + Elza | Schild] ”左:Pー4レベル実験室” / ”右:コンピュータールーム”  =  „Links: P-4-Labor“, „Rechts: Computerraum“
Port 5040 (Leon):
  - 5040 #0: "Left: P-4 Level Laboratory / Right: Computer Room"  [LEBT main00@0x16EE AOT3-sce1]
Port 5041 (Elza):
  - 5041 #0: "Left: P-4 Level Laboratory / Right: Computer Room"  [LEBT main00@0x16EE AOT3-sce1]

### Raum 506 — P-4-Labor, rotes Licht
Disc (TEXTE_40.md):
  - D506 #0 [Leon + Elza | System] 作 用のスーツの入ったロッカーだ / こわれてしまっているのか開かない  =  Ein Spind mit Arbeitsanzügen. Offenbar kaputt, er geht nicht auf. [Lesart 作業用]
  - D506 #1 [Leon + Elza | System] 中には何も無い ▼ あわてて開け放しにしたように見える  =  Nichts drin. Sieht aus, als sei er hastig offen gelassen worden.
  - D506 #2 [Leon + Elza | System] バイオハザードレベルを している ▼ 今は安全レベルのようだ  =  Zeigt die Biohazard-Stufe an. Im Moment offenbar eine sichere Stufe. [Lesart]
  - D506 #3 [Leon + Elza | System] 安全キャビネットだ ▼ しっかりとみっぺいされている  =  Ein Sicherheitsschrank, fest versiegelt.
  - D506 #4 [Leon + Elza | System] 固定ボルトが外れかかっているようだ ▼ 中のものが危険なもので / 無ければいいのだが  =  Die Halteschrauben lösen sich. Hoffentlich ist der Inhalt nicht gefährlich.
  - D506 #5 [Leon + Elza | System] 作 中のハプニングによるものか / キズのような跡がある  =  Spuren wie Kratzer, wohl von einem Zwischenfall bei der Arbeit. [Lesart]
  - D506 #6 [Leon + Elza | System] この流し台もクリーンに保たれている  =  Auch dieses Spülbecken wird sauber gehalten.
  - D506 #7 [Leon + Elza | System] 室内にガスが発生したとき / 動作するドラフトチャンバーだ ▼ これもクリーンに保たれていて / 作動した事があったのかはわからない  =  Ein Abzug, der bei Gas im Raum anspringt. Ebenfalls sauber, ob er je lief, ist nicht zu erkennen.
  - D506 #8 [Leon + Elza | System] ウィルスにかかわる機 だろう / 特に必要とするものはない  =  Wohl ein Gerät für Virusarbeiten. Nichts Nötiges. [Lesart]
  - D506 #9 [Leon + Elza | Entwicklervermerk] このROMではまだ / ”調べる”ができません  =  In dieser ROM ist „Untersuchen“ noch nicht möglich.
Port 5060 (Leon):
  - 5060 #0: "A locker used to hold containment suits.It's broken, and can't be opened."  [LEBT main00@0x2B44 AOT5-sce1; LEBT main00@0x2B6C AOT7-sce1]
  - 5060 #1: "There's nothing inside.It looks like it was emptied in a hurry."  [LEBT main00@0x2B58 AOT6-sce1]
  - 5060 #2: "Current Biohazard Level:It seems to be at a safe level right now."  [LEBT main00@0x2B80 AOT8-sce1]
  - 5060 #3: "Security CabinetIt's locked and airtight."  [LEBT main00@0x2B94 AOT9-sce1]
  - 5060 #4: "The security bolt has been removed.I hope nothing dangerous was inside."  [LEBT main00@0x2BA8 AOT10-sce1]
  - 5060 #5: "Something must've happened inside / this building. These footprints look / like those of an injured person."  [LEBT main00@0x2BBC AOT11-sce1]
  - 5060 #6: "The sink is very clean."  [LEBT main00@0x2BD0 AOT12-sce1]
  - 5060 #7: "It's a draft chamber, for use if a gas / outbreak occurs.It's very clean. I can't tell if it's ever / been used."  [LEBT main00@0x2BE4 AOT13-sce1]
  - 5060 #8: "It looks like a virus injector. I don't / really need to use it."  [LEBT main00@0x2C00 AOT14-sce1]
  - 5060 #9: "I can not investigate" this ROM yet."  [LEBT main00@0x2C1C AOT15-sce1 wenn Ck(3,79)=0]
  - 5060 #10: "A vaccine synthesis machine.A sample of the virus must / go inside.Will you push the switch? "  [LEBT sub06@0x30C6 Message_on]
  - 5060 #11: "Processing..."  [LEBT sub06@0x31D4 Message_on wenn Ck(12,31)=0]
  - 5060 #12: "Processed!"  [LEBT sub06@0x31DC Message_on wenn Ck(12,31)=0]
  - 5060 #13: "You've taken the T-Vaccine."  [LEBT sub06@0x322C Message_on wenn Ck(12,31)=0]
  - 5060 #14: "There is nothing left for me to do / here."  [LEBT main00@0x2C58 AOT15-sce1 wenn NICHT[Ck(3,79)=0] / NICHT[Ck(3,77)=0]; LEBT sub06@0x325A Aot_reset15-sce1 wenn Ck(12,31)=0]
Port 5061 (Elza):
  - 5061 #0: "A locker used to hold containment suits.It's broken, and can't be opened."  [LEBT main00@0x2B44 AOT5-sce1; LEBT main00@0x2B6C AOT7-sce1]
  - 5061 #1: "There's nothing inside.It looks like it was emptied in a hurry."  [LEBT main00@0x2B58 AOT6-sce1]
  - 5061 #2: "Current Biohazard Level:It seems to be at a safe level right now."  [LEBT main00@0x2B80 AOT8-sce1]
  - 5061 #3: "Security CabinetIt's locked and airtight."  [LEBT main00@0x2B94 AOT9-sce1]
  - 5061 #4: "The security bolt has been removed.I hope nothing dangerous was inside."  [LEBT main00@0x2BA8 AOT10-sce1]
  - 5061 #5: "Something must've happened inside / this building. These footprints look / like those of an injured person."  [LEBT main00@0x2BBC AOT11-sce1]
  - 5061 #6: "The sink is very clean."  [LEBT main00@0x2BD0 AOT12-sce1]
  - 5061 #7: "It's a draft chamber, for use if a gas / outbreak occurs.It's very clean. I can't tell if it's ever / been used."  [LEBT main00@0x2BE4 AOT13-sce1]
  - 5061 #8: "It looks like a virus injector. I don't / really need to use it."  [LEBT main00@0x2C00 AOT14-sce1]
  - 5061 #9: "I can not investigate" this ROM yet."  [LEBT main00@0x2C1C AOT15-sce1 wenn Ck(3,79)=0]
  - 5061 #10: "A vaccine synthesis machine.A sample of the virus must / go inside.Will you push the switch? "  [LEBT sub06@0x30C6 Message_on]
  - 5061 #11: "Processing..."  [LEBT sub06@0x31D4 Message_on wenn Ck(12,31)=0]
  - 5061 #12: "Processed!"  [LEBT sub06@0x31DC Message_on wenn Ck(12,31)=0]
  - 5061 #13: "You've taken the G-Vaccine."  [LEBT sub06@0x322C Message_on wenn Ck(12,31)=0]
  - 5061 #14: "There is nothing left for me to do / here."  [LEBT main00@0x2C58 AOT15-sce1 wenn NICHT[Ck(3,79)=0] / NICHT[Ck(3,77)=0]; LEBT sub06@0x325A Aot_reset15-sce1 wenn Ck(12,31)=0]

### Raum 50A — Laborgang mit Lichtrahmen
Disc (TEXTE_40.md):
  - D50A #0 [nicht ausgelöst | System] / シャッターのパネルだ ▼ 特別なIDカードが無ければ / 開かない様な事が書かれている  =  Ein Panel für das Rolltor. Offenbar öffnet es nur mit einer besonderen ID-Karte.
  - D50A #1 [nicht ausgelöst | System] / 右のシャッターがハンガーのようだ  =  Das rechte Rolltor führt offenbar zum Hangar.
  - D50A #2 [nicht ausgelöst | System] メンテナンス用のパネルがあるが / あまり使用していないように見える  =  Ein Wartungspanel, sieht wenig benutzt aus.
  - D50A #3 [nicht ausgelöst | System] 研究員が迷わないように / 書かれているのだろう  =  Steht wohl da, damit sich die Forscher nicht verlaufen.
Port 50A0 (Leon):
  - 50A0 #0: "It's the shutter control panel.I can't use it without an ID Card."  [LEBT main00@0x158A AOT4-sce1]
  - 50A0 #1: "It looks the shutter on the right / leads to a hangar."  [LEBT main00@0x159E AOT5-sce1]
  - 50A0 #2: "There's a maintenance panel here, but / it doesn't look like it has seen any use."  [LEBT main00@0x15B2 AOT6-sce1]
  - 50A0 #3: "Emergency Staircase"They probably wrote this so that / the researchers wouldn't get lost."  [LEBT main00@0x15C6 AOT7-sce1]
Port 50A1 (Elza):
  - 50A1 #0: "It's the shutter control panel.I can't use it without an ID Card."  [LEBT main00@0x158A AOT4-sce1]
  - 50A1 #1: "It looks the shutter on the right / leads to a hangar."  [LEBT main00@0x159E AOT5-sce1]
  - 50A1 #2: "There's a maintenance panel here, but / it doesn't look like it has seen any use."  [LEBT main00@0x15B2 AOT6-sce1]
  - 50A1 #3: "Emergency Staircase"They probably wrote this so that / the researchers wouldn't get lost."  [LEBT main00@0x15C6 AOT7-sce1]

### Raum 512 — Brennender Laborgang
Disc (TEXTE_40.md):
  - D512 #0 [Leon + Elza | Schild] ”左:Pー4レベル実験室” / ”右:コンピュータールーム”  =  „Links: P-4-Labor“, „Rechts: Computerraum“
  - D512 #1 [nicht ausgelöst | System] / 火事でロックされてしまっている！  =  Durch das Feuer verriegelt!
Port 5120 (Leon):
  - 5120 #0: "Left: P-4 Level Laboratory / Right: Computer Room"  [LEBT main00@0x1426 AOT1-sce1]
  - 5120 #1: "The door is locked because of / the fire!"  [LEBT main00@0x147A AOT4-sce1; LEBT main00@0x148E AOT5-sce1]
Port 5121 (Elza):
  - 5121 #0: "Left: P-4 Level Laboratory / Right: Computer Room"  [LEBT main00@0x1426 AOT1-sce1]
  - 5121 #1: "The door is locked because of / the fire!"  [LEBT main00@0x147A AOT4-sce1; LEBT main00@0x148E AOT5-sce1]

### Raum 514 — Laborgang
Disc (TEXTE_40.md):
  - D514 #0 [nicht ausgelöst | Frauenstimme, vom Ton her nicht Ada, Sherry denkbar, offen] / 開いたわ この奥よ  =  Es ist offen. Da hinten.
Port 5140 (Leon):
  - 5140 #0: " / It's open on this end."  [kein Aufruf]
Port 5141 (Elza):
  - 5141 #0: " / It's open on this end."  [kein Aufruf]

## B. Raeume ohne Disc-Text (Disc-Datei ohne Text oder nur im Mod-Baum)

### Raum 100 — NUR MOD-BAUM
Port 1000 (Leon):
  - 1000 #0: "Nothing unusual."  [LEBT sub00@0xCAA AOT6-sce1; LEBT sub00@0xCBE AOT7-sce1; LEBT sub00@0xCD2 AOT8-sce1; LEBT sub00@0xCE6 AOT9-sce1]
Port 1001 (Elza):
  - 1001 #0: "Nothing unusual."  [LEBT sub00@0xCAA AOT6-sce1; LEBT sub00@0xCBE AOT7-sce1; LEBT sub00@0xCD2 AOT8-sce1; LEBT sub00@0xCE6 AOT9-sce1]

### Raum 101 — NUR MOD-BAUM
Port 1010 (Leon):
  - 1010 #0: "Several books of testimonies / and cases.Nothing useful."  [LEBT main00@0x8EE AOT5-sce1]
  - 1010 #1: "An old typewriter.Why use it if there are / computers?"  [LEBT main00@0x902 AOT6-sce1]
  - 1010 #2: "A camera to record the / interrogations."  [LEBT main00@0x916 AOT7-sce1]
Port 1011 (Elza):
  - 1011 #0: "Officer: groans"  [LEBT sub02@0xB12 Message_on]
  - 1011 #1: "Elza: Hang in there! Are you the / only officer left in the building?"  [LEBT sub02@0xB62 Message_on]
  - 1011 #2: "Officer: Who... who are you?"  [LEBT sub02@0xB8E Message_on]
  - 1011 #3: "Elza: My name's Elza. / What's yours?"  [LEBT sub02@0xBB6 Message_on]
  - 1011 #4: "Officer: Elza, huh? I'm Roy."  [LEBT sub02@0xBE6 Message_on]
  - 1011 #5: "Elza: That wound... did one of / those things get you? How horrible..."  [LEBT sub02@0xC02 Message_on]
  - 1011 #6: "Roy: This precinct isn't safe anymore."  [LEBT sub02@0xC46 Message_on]
  - 1011 #7: "Roy: Those things broke through the / rear shutters and got inside."  [LEBT sub02@0xC62 Message_on]
  - 1011 #8: "Roy: Those bastards at Umbrella / caused all of this..."  [LEBT sub02@0xC6E Message_on]
  - 1011 #9: "Elza: ...What do you mean?"  [LEBT sub02@0xC92 Message_on]
  - 1011 #10: "Roy: groans"  [LEBT sub02@0xCAE Message_on]
  - 1011 #11: "Elza: Are you okay?"  [LEBT sub02@0xCB6 Message_on]
  - 1011 #12: "Roy: We don't have time to talk / right now."  [LEBT sub02@0xCCE Message_on]
  - 1011 #13: "Roy: You have to rescue the survivors / in the other rooms."  [LEBT sub02@0xCEA Message_on]
  - 1011 #14: "Elza: But..."  [LEBT sub02@0xD02 Message_on]
  - 1011 #15: "Roy: My friend John should still be / in the underground cell block."  [LEBT sub02@0xD1E Message_on]
  - 1011 #16: "Roy: Please, you have to help him..."  [LEBT sub02@0xD3A Message_on]
  - 1011 #17: "Elza: Roy, wake up! Please wake up!"  [LEBT sub02@0xD5C Message_on]
  - 1011 #18: "He has passed out from his injuries.He is holding a slip.The numbers 0513" are / printed on the slip."  [LEBT sub07@0xE64 Message_on]
  - 1011 #19: "He is holding something..."  [LEBT sub06@0xE20 Message_on]
  - 1011 #20: "You've taken the Prison key."  [LEBT sub06@0xE26 Message_on]
  - 1011 #21: "Several books of testimonies / and cases.Nothing useful."  [LEBT main00@0x996 AOT6-sce1]
  - 1011 #22: "An old typewriter.Why use it if there are / computers?"  [LEBT main00@0x9AA AOT7-sce1]
  - 1011 #23: "A camera to record the / interrogations."  [LEBT main00@0x9BE AOT8-sce1]

### Raum 103 — Disc-Datei vorhanden, ohne Text
Port 1030 (Leon):
  - 1030 #0: "This should keep those things out...For now..."  [LEBT main00@0x1FA0 AOT17-sce1 wenn Ck(4,15)=0]
  - 1030 #1: "Guess it was only a matter of time..."  [LEBT main00@0x1FB8 AOT17-sce1 wenn NICHT[Ck(4,15)=0]; LEBT sub08@0x276C Aot_reset17-sce1]
  - 1030 #2: "The emergency shutters have / already been activated."  [LEBT main00@0x1D1A AOT7-sce1]
  - 1030 #3: "A payphone.The line appears to be down..."  [LEBT main00@0x1D2E AOT8-sce1]
  - 1030 #4: "Some sort of reference chart for / identifying emergency severity.""  [LEBT main00@0x1D42 AOT9-sce1]
  - 1030 #5: "This emergency severity" chart / gives more confusion than it prevents."  [kein Aufruf]
  - 1030 #6: "There's nothing useful on the counter."  [LEBT main00@0x1D56 AOT10-sce1]
  - 1030 #7: "Seems like the crash broke the axle.Poor thing..."  [kein Aufruf]
  - 1030 #8: "Seems like whoever rode this crashed / it through the front gate.Wonder if they're ok..."  [LEBT main00@0x1D6A AOT11-sce1]
  - 1030 #9: "It's too dark to see if there's / anything inside.Would you like to reach inside?"  [kein Aufruf]
  - 1030 #10: "Found Green Medicine. / Take it?"  [kein Aufruf]
  - 1030 #11: "Better not dig through other's trash..."  [kein Aufruf]
  - 1030 #12: "There's glass from the front door / all over the floor."  [LEBT main00@0x1D7E AOT12-sce1]
  - 1030 #13: "Desks for filling out inquiry forms."  [LEBT main00@0x1D92 AOT13-sce1]
  - 1030 #14: "Seems like even the emergency / broadcast has gone offline..."  [LEBT main00@0x1DA6 AOT14-sce1]
  - 1030 #15: "A map of dowtown Raccoon City. / Population est. 100,000 or more..."  [LEBT main00@0x1DBA AOT15-sce1]
  - 1030 #16: "An emergency alarm.The red light works, but the alarm / bell appears to be malfunctioning."  [LEBT main00@0x1DCE AOT16-sce1]
  - 1030 #17: "There are some words carved here... / I    C  A  N  '  T    E  S  C  A  P  E  !  !  !"The door is locked, I can't open it."  [kein Aufruf]
Port 1031 (Elza):
  - 1031 #0: "This should keep those things out...For now..."  [LEBT main00@0x201C AOT17-sce1 wenn Ck(4,15)=0]
  - 1031 #1: "Guess it was only a matter of time..."  [LEBT main00@0x2034 AOT17-sce1 wenn NICHT[Ck(4,15)=0]; LEBT sub08@0x2858 Aot_reset17-sce1]
  - 1031 #2: "The emergency shutters have / already been activated."  [LEBT main00@0x1D88 AOT7-sce1]
  - 1031 #3: "A payphone.The line appears to be down..."  [LEBT main00@0x1D9C AOT8-sce1]
  - 1031 #4: "Some sort of reference chart for / identifying emergency severity.""  [LEBT main00@0x1DB0 AOT9-sce1]
  - 1031 #5: "This emergency severity" chart / gives more confusion than it prevents."  [kein Aufruf]
  - 1031 #6: "There's nothing useful on the counter."  [LEBT main00@0x1DC4 AOT10-sce1]
  - 1031 #7: "Seems like the crash broke the axle.Poor thing..."  [LEBT main00@0x1DD8 AOT11-sce1]
  - 1031 #8: "Seems like whoever rode this crashed / it through the front gate.Wonder if they're ok..."  [kein Aufruf]
  - 1031 #9: "It's too dark to see if there's / anything inside.Would you like to reach inside?"  [kein Aufruf]
  - 1031 #10: "Found Green Medicine. / Take it?"  [kein Aufruf]
  - 1031 #11: "Better not dig through other's trash..."  [kein Aufruf]
  - 1031 #12: "There's glass from the front door / all over the floor."  [LEBT main00@0x1DEC AOT12-sce1]
  - 1031 #13: "Desks for filling out inquiry forms."  [LEBT main00@0x1E00 AOT13-sce1]
  - 1031 #14: "Seems like even the emergency / broadcast has gone offline..."  [LEBT main00@0x1E14 AOT14-sce1]
  - 1031 #15: "A map of dowtown Raccoon City. / Population est. 100,000 or more..."  [LEBT main00@0x1E28 AOT15-sce1]
  - 1031 #16: "An emergency alarm.The red light works, but the alarm / bell appears to be malfunctioning."  [LEBT main00@0x1E3C AOT16-sce1]
  - 1031 #17: "There are some words carved here... / I    C  A  N  '  T    E  S  C  A  P  E  !  !  !"The door is locked, I can't open it."  [kein Aufruf]
  - 1031 #18: "Elza: That was close, this must / contain them for some time..."  [LEBT sub13@0x2A08 Message_on]
  - 1031 #19: "Elza: Hey, is anyone here?!"  [LEBT sub13@0x2A24 Message_on]
  - 1031 #20: "We barricaded ourselves / inside the police station..."  [LEBT sub15@0x2A7A Message_on]
  - 1031 #21: "Where we thought it would / be safe."  [LEBT sub15@0x2A82 Message_on]
  - 1031 #22: "It was no use, and so began a / life and death struggle..."  [LEBT sub15@0x2A8A Message_on]
  - 1031 #23: "To escape."  [LEBT sub15@0x2A92 Message_on]

### Raum 105 — NUR MOD-BAUM
Port 1050 (Leon):
  - 1050 #0: "It's a shutter switch. / Will you push it? "  [LEBT sub02@0xCAC Message_on]
  - 1050 #1: "Various posters of missing / persons.It is better not to think about / it..."  [LEBT main00@0xBD2 AOT8-sce1]
  - 1050 #2: "I need a fuse to run the shutter."  [kein Aufruf]
  - 1050 #3: "Just scattered papers and books.Nothing relevant."  [LEBT main00@0xBE6 AOT9-sce1]
  - 1050 #4: "A blackboard full of reports of riots / and cannibalism at points in the city."  [LEBT main00@0xBFA AOT10-sce1]
  - 1050 #5: "He's got a big bite on his neck, he / didn't give him a chance to avoid it.The weapon can be useful.Sorry, I'll just borrow this..."  [kein Aufruf]
  - 1050 #6: "Ada: Give me a moment, I need a / break."  [kein Aufruf]
  - 1050 #7: "Leon: Relax, I'll take you to the / medical room."  [kein Aufruf]
  - 1050 #8: "Leon: Hey, wait!"  [LEBT sub03@0xDD6 Message_on]
Port 1051 (Elza):
  - 1051 #0: "It's a shutter switch. / Will you push it? "  [LEBT sub02@0xCC8 Message_on]
  - 1051 #1: "Various posters of missing / persons.It is better not to think about / it..."  [LEBT main00@0xBD2 AOT8-sce1]
  - 1051 #2: "I need a fuse to run the shutter."  [kein Aufruf]
  - 1051 #3: "Just scattered papers and books.Nothing relevant."  [LEBT main00@0xBE6 AOT9-sce1]
  - 1051 #4: "A blackboard full of reports of riots / and cannibalism at points in the city."  [LEBT main00@0xBFA AOT10-sce1]
  - 1051 #5: "He's got a big bite on his neck, he / didn't give him a chance to avoid it.The weapon can be useful.Sorry, I'll just borrow this..."  [LEBT sub03@0xDB6 Message_on]

### Raum 106 — Disc-Datei vorhanden, ohne Text
Port 1060 (Leon):
  - 1060 #0: "Only the lights of the / buildings are visible."  [LEBT main00@0xE12 AOT16-sce1]
Port 1061 (Elza):
  - 1061 #0: "Only the lights of the / buildings are visible."  [LEBT main00@0xE12 AOT16-sce1]

### Raum 107 — Disc-Datei vorhanden, ohne Text
Port 1070 (Leon):
  - 1070 #0: "The number of desks far outnumbers / the amount of office chairs...Were these their solution?"  [LEBT main00@0x147E AOT1-sce1]
  - 1070 #1: "The seats contra chairs almost makes  / musical chairs seem like the most fairmethod of ensuring everyone had / an even chance getting good seat for sits."  [kein Aufruf]
  - 1070 #2: "A cabinet full of binders containing / police reports.Nothing useful."  [LEBT main00@0x1492 AOT2-sce1]
  - 1070 #3: "These boxes contain nothing of use."  [LEBT main00@0x14A6 AOT3-sce1]
  - 1070 #4: "The lockers all appear to be locked."  [LEBT main00@0x14BA AOT4-sce1]
  - 1070 #5: "A work schedule of some sort. / Guess "L.S.K." lucked out this week."  [kein Aufruf]
  - 1070 #6: "The work schedule for this week. / This really wasn't my week."  [kein Aufruf]
  - 1070 #7: "A lot of notes and reminders regarding / ongoing cases.Most appears to be missing people / or random assault cases."  [LEBT main00@0x14CE AOT5-sce1]
  - 1070 #8: "A messy desk. Nothing useful here."  [LEBT main00@0x14E2 AOT6-sce1]
  - 1070 #9: "I'd hate sitting this close to the door, / even if I had that computer at my desk."  [LEBT main00@0x14F6 AOT7-sce1]
  - 1070 #10: "During times like these, I almost miss / filling out paperwork at a desk like this."  [kein Aufruf]
  - 1070 #11: "A tidy desk with an electric fan."  [LEBT main00@0x150A AOT8-sce1]
  - 1070 #12: "These desks are tidy, but the fan on / the other desk is clearly a neusance."  [LEBT main00@0x151E AOT9-sce1]
  - 1070 #13: "Going by the shelf, whoever used this / desk probably enjoyed privacy."  [kein Aufruf]
  - 1070 #14: "Like the other, this desk too contains / documents for ongoing investigations."  [kein Aufruf]
  - 1070 #15: "These desks all appear quite messy / and seem to contain nothing of use."  [kein Aufruf]
  - 1070 #16: "The desks on this row all seem to / be filled with paperwork."  [kein Aufruf]
  - 1070 #17: "These desks look strangely empty. / Guess they were unoccupied."  [kein Aufruf]
  - 1070 #18: "You can get a good overview of the / lobby area from here."  [LEBT main00@0x1532 AOT10-sce1; LEBT main00@0x1546 AOT11-sce1]
  - 1070 #19: "A police officer. He's dead..."  [kein Aufruf]
  - 1070 #20: "It's an phone.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0x155A AOT12-sce1]
Port 1071 (Elza):
  - 1071 #0: "The number of desks far outnumbers / the amount of office chairs...Were these their solution?"  [LEBT main00@0x147E AOT1-sce1]
  - 1071 #1: "The seats contra chairs almost makes  / musical chairs seem like the most fairmethod of ensuring everyone had / an even chance getting good seat for sits."  [kein Aufruf]
  - 1071 #2: "A cabinet full of binders containing / police reports.Nothing useful."  [LEBT main00@0x1492 AOT2-sce1]
  - 1071 #3: "These boxes contain nothing of use."  [LEBT main00@0x14A6 AOT3-sce1]
  - 1071 #4: "The lockers all appear to be locked."  [LEBT main00@0x14BA AOT4-sce1]
  - 1071 #5: "A work schedule of some sort. / Guess "L.S.K." lucked out this week."  [kein Aufruf]
  - 1071 #6: "The work schedule for this week. / This really wasn't my week."  [kein Aufruf]
  - 1071 #7: "A lot of notes and reminders regarding / ongoing cases.Most appears to be missing people / or random assault cases."  [LEBT main00@0x14CE AOT5-sce1]
  - 1071 #8: "A messy desk. Nothing useful here."  [LEBT main00@0x14E2 AOT6-sce1]
  - 1071 #9: "I'd hate sitting this close to the door, / even if I had that computer at my desk."  [LEBT main00@0x14F6 AOT7-sce1]
  - 1071 #10: "During times like these, I almost miss / filling out paperwork at a desk like this."  [kein Aufruf]
  - 1071 #11: "A tidy desk with an electric fan."  [LEBT main00@0x150A AOT8-sce1]
  - 1071 #12: "These desks are tidy, but the fan on / the other desk is clearly a neusance."  [LEBT main00@0x151E AOT9-sce1]
  - 1071 #13: "Going by the shelf, whoever used this / desk probably enjoyed privacy."  [kein Aufruf]
  - 1071 #14: "Like the other, this desk too contains / documents for ongoing investigations."  [kein Aufruf]
  - 1071 #15: "These desks all appear quite messy / and seem to contain nothing of use."  [kein Aufruf]
  - 1071 #16: "The desks on this row all seem to / be filled with paperwork."  [kein Aufruf]
  - 1071 #17: "These desks look strangely empty. / Guess they were unoccupied."  [kein Aufruf]
  - 1071 #18: "You can get a good overview of the / lobby area from here."  [LEBT main00@0x1532 AOT10-sce1; LEBT main00@0x1546 AOT11-sce1]
  - 1071 #19: "A police officer. He's dead..."  [kein Aufruf]
  - 1071 #20: "It's an phone.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0x155A AOT12-sce1]

### Raum 10C — NUR MOD-BAUM
Port 10C0 (Leon):
  - 10C0 #0: "It's an elevator.Pressing the button has no effect.It seems the power isn't turned on."  [LEBT main00@0xE6A AOT1-sce1 wenn Ck(4,243)=0]
  - 10C0 #1: "Only the lights of the / buildings are visible."  [LEBT main00@0xEC4 AOT3-sce1; LEBT main00@0xED8 AOT4-sce1; LEBT main00@0xEEC AOT5-sce1; LEBT main00@0xF00 AOT6-sce1]
  - 10C0 #2: "Wanted posters and reports, / nothing relevant."  [LEBT main00@0xF14 AOT7-sce1]
  - 10C0 #3: "The emergency channel is / down..."  [LEBT main00@0xF28 AOT8-sce1]
  - 10C0 #4: "Nothing useful."  [LEBT main00@0xF3C AOT9-sce1]
  - 10C0 #5: "Best Midwest Winner K-9 Unit / for R.P.D."  [LEBT main00@0xF50 AOT10-sce1]
  - 10C0 #6: "Papers of various cases and / forms, nothing important."  [LEBT main00@0xF64 AOT11-sce1]
  - 10C0 #7: " / ..."  [LEBT sub02@0x1030 Message_on]
Port 10C1 (Elza):
  - 10C1 #0: "It's an elevator.Pressing the button has no effect.It seems the power isn't turned on."  [LEBT main00@0xE6A AOT1-sce1 wenn Ck(4,243)=0]
  - 10C1 #1: "Only the lights of the / buildings are visible."  [LEBT main00@0xEC4 AOT3-sce1; LEBT main00@0xED8 AOT4-sce1; LEBT main00@0xEEC AOT5-sce1; LEBT main00@0xF00 AOT6-sce1]
  - 10C1 #2: "Wanted posters and reports, / nothing relevant."  [LEBT main00@0xF14 AOT7-sce1]
  - 10C1 #3: "The emergency channel is / down..."  [LEBT main00@0xF28 AOT8-sce1]
  - 10C1 #4: "Nothing useful."  [LEBT main00@0xF3C AOT9-sce1]
  - 10C1 #5: "Best Midwest Winner K-9 Unit / for R.P.D."  [LEBT main00@0xF50 AOT10-sce1]
  - 10C1 #6: "Papers of various cases and / forms, nothing important."  [LEBT main00@0xF64 AOT11-sce1]
  - 10C1 #7: " / ..."  [LEBT sub02@0x1030 Message_on]

### Raum 10D — NUR MOD-BAUM
Port 10D0 (Leon):
  - 10D0 #0: "Enter the first number."  [LEBT sub17@0x18E2 Message_on]
  - 10D0 #1: "Enter the second number."  [LEBT sub18@0x190A Message_on wenn Ck(5,17)=0]
  - 10D0 #2: "Enter the third number."  [LEBT sub18@0x1924 Message_on wenn NICHT[Ck(5,17)=0] / Ck(5,18)=1]
  - 10D0 #3: "Enter the fourth number."  [LEBT sub18@0x193A Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / Ck(5,19)=1]
  - 10D0 #4: "Wrong code, try again."  [LEBT sub18@0x1950 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / NICHT[Ck(5,19)=1] / Ck(5,20)=0]
  - 10D0 #5: "You've opened the lock."  [LEBT sub19@0x199E Message_on]
  - 10D0 #6: "Communication Room"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0x102A AOT0-sce1 wenn Ck(3,50)=0]
  - 10D0 #7: "A card reader.You have to use the Blue Keycard / and insert four digits tounlock the door.Will you operate the / card reader? "  [LEBT sub20@0x19AE Message_on]
  - 10D0 #8: "You have not the Blue Keycard / to operate it."  [LEBT sub20@0x19D2 Message_on wenn Ck(12,31)=0 / NICHT[Ck(9,52)=1]]
  - 10D0 #9: "You've used the Blue Keycard."  [LEBT sub20@0x19C4 Message_on wenn Ck(12,31)=0 / Ck(9,52)=1]
  - 10D0 #10: "Left: First Aid Room, Communication / Room. Right: Archive Room."  [LEBT main00@0x11F6 AOT17-sce1]
  - 10D0 #11: "It's a shutter switch. / Will you push it? "  [LEBT sub21@0x19DE Message_on]
  - 10D0 #12: "???: Freeze!"  [LEBT sub21@0x1A3E Message_on wenn Ck(12,31)=0]
  - 10D0 #13: "Leon: Wait don't shoot!"  [LEBT sub21@0x1A60 Message_on wenn Ck(12,31)=0]
  - 10D0 #14: "Marvin: Leon... You're still alive!"  [LEBT sub21@0x1A92 Message_on wenn Ck(12,31)=0]
  - 10D0 #15: "Leon: I'm glad to see that you're / still alive Marvin."  [LEBT sub21@0x1B0C Message_on wenn Ck(12,31)=0]
  - 10D0 #16: "Leon: Where are the rest of the / officers?"  [LEBT sub21@0x1B34 Message_on wenn Ck(12,31)=0]
  - 10D0 #17: "Leon: Has everyone been eaten by / zombies? or have they turned into / zombies?"  [LEBT sub21@0x1B3C Message_on wenn Ck(12,31)=0]
  - 10D0 #18: "Marvin: You've already seen him, / I thought he was the only one left at"  [LEBT sub21@0x1B5C Message_on wenn Ck(12,31)=0]
  - 10D0 #19: "the station, tell me, are there other / survivors?"  [LEBT sub21@0x1B78 Message_on wenn Ck(12,31)=0]
  - 10D0 #20: "Leon: You are the only one who I found, / the chief is injured, one of those / things bit him."  [LEBT sub21@0x1B98 Message_on wenn Ck(12,31)=0]
  - 10D0 #21: "Marvin: Damn, the situation is terrible, / we have to find a way out of here."  [LEBT sub21@0x1BE2 Message_on wenn Ck(12,31)=0]
  - 10D0 #22: "Leon: The chief told me about using the / communication system and asking for / outside help."  [LEBT sub21@0x1C1E Message_on wenn Ck(12,31)=0]
  - 10D0 #23: "Marvin: But the system is damaged, if / the other officers could not repair it / as only we can?"  [LEBT sub21@0x1C56 Message_on wenn Ck(12,31)=0]
  - 10D0 #24: "Leon: I don't know, it remains to try, / the helicopter abandoned us, the phones"  [LEBT sub21@0x1C8E Message_on wenn Ck(12,31)=0]
  - 10D0 #25: "are down and with the building besieged / by those things, I don't see any more / options."  [LEBT sub21@0x1CBE Message_on wenn Ck(12,31)=0]
  - 10D0 #26: "Marvin: Okay, I'll meet with the chief / first and we'll start looking for a way / to fix it."  [LEBT sub21@0x1CF2 Message_on wenn Ck(12,31)=0]
  - 10D0 #27: "Leon: I agree, he's in his office."  [LEBT sub21@0x1D2A Message_on wenn Ck(12,31)=0]
Port 10D1 (Elza):
  - 10D1 #0: "Enter the first number."  [LEBT sub17@0x18CC Message_on]
  - 10D1 #1: "Enter the second number."  [LEBT sub18@0x18F4 Message_on wenn Ck(5,17)=0]
  - 10D1 #2: "Enter the third number."  [LEBT sub18@0x190E Message_on wenn NICHT[Ck(5,17)=0] / Ck(5,18)=1]
  - 10D1 #3: "Enter the fourth number."  [LEBT sub18@0x1924 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / Ck(5,19)=1]
  - 10D1 #4: "Wrong code, try again."  [LEBT sub18@0x193A Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / NICHT[Ck(5,19)=1] / Ck(5,20)=0]
  - 10D1 #5: "You've opened the lock."  [LEBT sub19@0x1988 Message_on]
  - 10D1 #6: "Communication Room"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0x102A AOT0-sce1 wenn Ck(3,50)=0]
  - 10D1 #7: "A card reader.You have to use the Blue Keycard / and insert four digits tounlock the door.Will you operate the / card reader? "  [LEBT sub20@0x1998 Message_on]
  - 10D1 #8: "You have not the Blue Keycard / to operate it."  [LEBT sub20@0x19BC Message_on wenn Ck(12,31)=0 / NICHT[Ck(9,52)=1]]
  - 10D1 #9: "You've used the Blue Keycard."  [LEBT sub20@0x19AE Message_on wenn Ck(12,31)=0 / Ck(9,52)=1]
  - 10D1 #10: "Left: First Aid Room, Communication / Room. Right: Archive Room."  [LEBT main00@0x11F6 AOT17-sce1]
  - 10D1 #11: "It's a shutter switch. / Will you push it?         /   ,      3   "  [LEBT sub21@0x19C8 Message_on]

### Raum 10E — NUR MOD-BAUM
Port 10E0 (Leon):
  - 10E0 #0: "It's the body of a doctor.He has no vital signs."  [LEBT main00@0xC90 AOT5-sce1]
  - 10E0 #1: "An old typewriter.Why use it if there are / computers?"  [LEBT main00@0xCA4 AOT6-sce1]
  - 10E0 #2: "September 1998."  [LEBT main00@0xCB8 AOT7-sce1]
  - 10E0 #3: "Various medicines and bandages."  [LEBT main00@0xCCC AOT8-sce1; LEBT main00@0xCE0 AOT9-sce1]
  - 10E0 #4: "It's a photo of the Arklay / Mountains."  [LEBT main00@0xCF4 AOT10-sce1]
Port 10E1 (Elza):
  - 10E1 #0: "It's the body of a doctor.He has no vital signs."  [LEBT main00@0xC90 AOT5-sce1]
  - 10E1 #1: "An old typewriter.Why use it if there are / computers?"  [LEBT main00@0xCA4 AOT6-sce1]
  - 10E1 #2: "September 1998."  [LEBT main00@0xCB8 AOT7-sce1]
  - 10E1 #3: "Various medicines and bandages."  [LEBT main00@0xCCC AOT8-sce1; LEBT main00@0xCE0 AOT9-sce1]
  - 10E1 #4: "It's a photo of the Arklay / Mountains."  [LEBT main00@0xCF4 AOT10-sce1]

### Raum 10F — NUR MOD-BAUM
Port 10F0 (Leon):
  - 10F0 #0: "A large communications device.It's out of commission.I can't send any messages from / here."  [LEBT main00@0xF72 AOT2-sce1; LEBT main00@0xF86 AOT3-sce1]
  - 10F0 #1: "Nothing suspicious nor / out of the ordinary."  [LEBT main00@0xF9A AOT4-sce1; LEBT main00@0xFAE AOT5-sce1]
  - 10F0 #2: "There isnt anything / useful on this shelf."  [LEBT main00@0xFC2 AOT6-sce1]
  - 10F0 #3: "There are various devices.This must be where they / gathered information."  [LEBT main00@0xFD6 AOT7-sce1]
  - 10F0 #4: "Several files from past / cases.Nothing useful here."  [LEBT main00@0xFEA AOT8-sce1]
  - 10F0 #5: "It's a small office.It's locked."  [LEBT main00@0xFFE AOT9-sce1]
Port 10F1 (Elza):
  - 10F1 #0: "A large communications device.It's out of commission.I can't send any messages from / here."  [LEBT main00@0xF72 AOT2-sce1; LEBT main00@0xF86 AOT3-sce1]
  - 10F1 #1: "Nothing suspicious nor / out of the ordinary."  [LEBT main00@0xF9A AOT4-sce1; LEBT main00@0xFAE AOT5-sce1]
  - 10F1 #2: "There isnt anything / useful on this shelf."  [LEBT main00@0xFC2 AOT6-sce1]
  - 10F1 #3: "There are various devices.This must be where they / gathered information."  [LEBT main00@0xFD6 AOT7-sce1]
  - 10F1 #4: "Several files from past / cases.Nothing useful here."  [LEBT main00@0xFEA AOT8-sce1]
  - 10F1 #5: "It's a small office.It's locked."  [LEBT main00@0xFFE AOT9-sce1]

### Raum 110 — Disc-Datei vorhanden, ohne Text
Port 1100 (Leon):
  - 1100 #0: "It's electronically locked."  [LEBT sub00@0xAEA AOT3-sce1 wenn Ck(3,103)=0; LEBT sub00@0xB28 AOT3-sce1 wenn NICHT[Ck(3,103)=0] / Ck(4,232)=0]
  - 1100 #1: "It's a control panel.A type of voice device is required / to open the lock."  [LEBT sub00@0xAD6 AOT2-sce1 wenn Ck(3,103)=0; LEBT sub07@0xD50 Message_on]
  - 1100 #2: "You've opened the lock."  [LEBT sub02@0xCBA Message_on]
  - 1100 #3: "Will you use the Minidisc Player w/ Disc? "  [LEBT sub07@0xD56 Message_on]
  - 1100 #4: "You've used the Minidisc Player w/ Disc."  [LEBT sub02@0xC90 Message_on]
  - 1100 #5: "I don't need to use this anymore."  [LEBT sub00@0xB8A AOT2-sce1 wenn NICHT[Ck(3,103)=0] / NICHT[Ck(4,232)=0]; LEBT sub02@0xCD2 Aot_reset2-sce1]
Port 1101 (Elza):
  - 1101 #0: "It's electronically locked."  [LEBT sub00@0xAEA AOT3-sce1 wenn Ck(3,103)=0; LEBT sub00@0xB28 AOT3-sce1 wenn NICHT[Ck(3,103)=0] / Ck(4,232)=0]
  - 1101 #1: "It's a control panel.A type of voice device is required / to open the lock."  [LEBT sub00@0xAD6 AOT2-sce1 wenn Ck(3,103)=0; LEBT sub07@0xD50 Message_on]
  - 1101 #2: "You've opened the lock."  [LEBT sub02@0xCBA Message_on]
  - 1101 #3: "Will you use the Minidisc Player w/ Disc? "  [LEBT sub07@0xD56 Message_on]
  - 1101 #4: "You've used the Minidisc Player w/ Disc."  [LEBT sub02@0xC90 Message_on]
  - 1101 #5: "I don't need to use this anymore."  [LEBT sub00@0xB8A AOT2-sce1 wenn NICHT[Ck(3,103)=0] / NICHT[Ck(4,232)=0]; LEBT sub02@0xCD2 Aot_reset2-sce1]

### Raum 111 — NUR MOD-BAUM
Port 1110 (Leon):
  - 1110 #0: "It's a police officer, he's dead.He is holding a slip.The numbers 4312" are / printed on the slip."  [LEBT sub02@0xD04 Message_on]
  - 1110 #1: "Files on various cases.None appear to be useful."  [LEBT main00@0xC0C AOT11-sce1]
  - 1110 #2: "Visual materials."  [LEBT main00@0xC20 AOT12-sce1]
  - 1110 #3: "A pile of investigation reports."  [LEBT main00@0xC34 AOT13-sce1; LEBT main00@0xC48 AOT14-sce1; LEBT main00@0xC5C AOT15-sce1]
  - 1110 #4: "Weapons seized, those that remain / are unusable."  [LEBT main00@0xC70 AOT16-sce1]
  - 1110 #5: "An unarmed carbine, I have no idea / how to fix it."  [LEBT main00@0xC84 AOT17-sce1]
  - 1110 #6: "A rocket launcher, has something / written on it:Evidence of the Arklay incident, / do not manipulate."  [LEBT main00@0xC98 AOT18-sce1]
  - 1110 #7: "Ammunition boxes seized, they've / all taken it."  [LEBT main00@0xCAC AOT19-sce1]
  - 1110 #8: "Will you take the Time Bomb? "  [LEBT sub03@0xD24 Message_on]
  - 1110 #9: "You've taken the Time Bomb."  [LEBT sub03@0xD40 Message_on wenn Ck(12,31)=0]
Port 1111 (Elza):
  - 1111 #0: "It's a police officer, he's dead.He is holding a slip.The numbers 4312" are / printed on the slip."  [LEBT sub02@0xD04 Message_on]
  - 1111 #1: "Files on various cases.None appear to be useful."  [LEBT main00@0xC0C AOT11-sce1]
  - 1111 #2: "Visual materials."  [LEBT main00@0xC20 AOT12-sce1]
  - 1111 #3: "A pile of investigation reports."  [LEBT main00@0xC34 AOT13-sce1; LEBT main00@0xC48 AOT14-sce1; LEBT main00@0xC5C AOT15-sce1]
  - 1111 #4: "Weapons seized, those that remain / are unusable."  [LEBT main00@0xC70 AOT16-sce1]
  - 1111 #5: "An unarmed carbine, I have no idea / how to fix it."  [LEBT main00@0xC84 AOT17-sce1]
  - 1111 #6: "A rocket launcher, has something / written on it:Evidence of the Arklay incident, / do not manipulate."  [LEBT main00@0xC98 AOT18-sce1]
  - 1111 #7: "Ammunition boxes seized, they've / all taken it."  [LEBT main00@0xCAC AOT19-sce1]
  - 1111 #8: "Will you take the Time Bomb? "  [LEBT sub03@0xD24 Message_on]
  - 1111 #9: "You've taken the Time Bomb."  [LEBT sub03@0xD40 Message_on wenn Ck(12,31)=0]

### Raum 112 — NUR MOD-BAUM
Port 1120 (Leon):
  - 1120 #0: "It's an elevator.Pressing the button has no effect.It seems the power isn't turned on."  [LEBT main00@0xC9E AOT1-sce1 wenn Ck(4,243)=0]
  - 1120 #1: "A painting, titled: / The Music Lesson""  [kein Aufruf]
  - 1120 #2: "A painting of 5 women's."  [kein Aufruf]
  - 1120 #3: "Only the lights of the / buildings are visible."  [kein Aufruf]
  - 1120 #4: "Various reports and case / information.Nothing useful."  [kein Aufruf]
  - 1120 #5: "A strange Christmas toy."  [kein Aufruf]
  - 1120 #6: "It's a phone.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0xCF8 AOT3-sce1]
  - 1120 #7: "..."  [kein Aufruf]
  - 1120 #8: "An advertisement for an arm / wrestling contest.The winner will be named / Chief for a Day of theRaccoon Police Department!""  [kein Aufruf]
Port 1121 (Elza):
  - 1121 #0: "It's an elevator.Pressing the button has no effect.It seems the power isn't turned on."  [LEBT main00@0xC9E AOT1-sce1 wenn Ck(4,243)=0]
  - 1121 #1: "A painting, titled: / The Music Lesson""  [kein Aufruf]
  - 1121 #2: "A painting of 5 women's."  [kein Aufruf]
  - 1121 #3: "Only the lights of the / buildings are visible."  [kein Aufruf]
  - 1121 #4: "Various reports and case / information.Nothing useful."  [kein Aufruf]
  - 1121 #5: "A strange Christmas toy."  [kein Aufruf]
  - 1121 #6: "It's a phone.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0xCF8 AOT3-sce1]
  - 1121 #7: "..."  [kein Aufruf]
  - 1121 #8: "An advertisement for an arm / wrestling contest.The winner will be named / Chief for a Day of theRaccoon Police Department!""  [kein Aufruf]

### Raum 113 — NUR MOD-BAUM
Port 1130 (Leon):
  - 1130 #0: "It's a shutter switch. / Will you push it? "  [LEBT sub02@0xA28 Message_on]
  - 1130 #1: "It's not necessary to go back."  [LEBT sub01@0xA1C Aot_reset3-sce1]
  - 1130 #2: "A photo of the members of / S.T.A.R.S."  [LEBT main00@0x974 AOT5-sce1]
  - 1130 #3: "An emblem of the Raccoon / S.W.A.T Team."  [LEBT main00@0x988 AOT6-sce1]
  - 1130 #4: "A painting of a man with a hook, / seeing it makes me uncomfortable."  [LEBT main00@0x99C AOT7-sce1]
  - 1130 #5: "A shelf with trophies.Among them is the award for best / shooter, Chris Redfield."  [LEBT main00@0x9B0 AOT8-sce1]
Port 1131 (Elza):
  - 1131 #0: "It's a shutter switch. / Will you push it? "  [LEBT sub02@0xA22 Message_on]
  - 1131 #1: "It's not necessary to go back."  [kein Aufruf]
  - 1131 #2: "A photo of the members of / S.T.A.R.S."  [LEBT main00@0x97A AOT5-sce1]
  - 1131 #3: "An emblem of the Raccoon / S.W.A.T Team."  [LEBT main00@0x98E AOT6-sce1]
  - 1131 #4: "A painting of a man with a hook, / seeing it makes me uncomfortable."  [LEBT main00@0x9A2 AOT7-sce1]
  - 1131 #5: "A shelf with trophies.Among them is the award for best / shooter, Chris Redfield."  [LEBT main00@0x9B6 AOT8-sce1]

### Raum 114 — NUR MOD-BAUM
Port 1140 (Leon):
  - 1140 #0: "You've opened the lock."  [LEBT sub02@0xC94 Message_on]
  - 1140 #1: "A beautiful painting.There's a switch below. / Will you push it? "  [LEBT sub03@0xCA6 Message_on; LEBT sub05@0xD66 Message_on]
  - 1140 #2: "It's a big table."  [LEBT main00@0xAD4 AOT3-sce1]
  - 1140 #3: "The streets are infested with / zombies!"  [kein Aufruf]
  - 1140 #4: "The streets are infested with / zombies!"  [LEBT main00@0xAE8 AOT4-sce1]
  - 1140 #5: "The switch is not responding..."  [LEBT sub05@0xD74 Message_on wenn Ck(12,31)=0]
Port 1141 (Elza):
  - 1141 #0: "Elza: John, wait!"  [LEBT sub02@0xC80 Message_on]
  - 1141 #1: "John: Elza! sorry, I'm glad to see / you're still alive..."  [LEBT sub02@0xCB0 Message_on]
  - 1141 #2: "Elza: Same here... are you okay? / what's happened?"  [LEBT sub02@0xD0A Message_on]
  - 1141 #3: "John: Somehow... Elza, listen to me!"  [LEBT sub02@0xD42 Message_on]
  - 1141 #4: "Elza: What is wrong?"  [LEBT sub02@0xD7A Message_on]
  - 1141 #5: "John: Don't go to the rooftop! / ...there's something...a somewhat / dangerous monster!"  [LEBT sub02@0xD9E Message_on]
  - 1141 #6: "Elza: What was that?!"  [LEBT sub02@0xDCA Message_on]
  - 1141 #7: "John: Shit! that is what I was / talking about!"  [LEBT sub02@0xE14 Message_on]
  - 1141 #8: "Elza: what the hell is that?!"  [LEBT sub02@0xE64 Message_on]
  - 1141 #9: "John: Looks like some kind of gorilla, / that would explain how they managed / to enter through the windows!"  [LEBT sub02@0xEBC Message_on]
  - 1141 #10: "Elza: John! what will we do now?!"  [LEBT sub02@0xEEC Message_on]
  - 1141 #11: "John: Elza, I'll try to distract them, / while you try to get out of here, hurry!"  [LEBT sub02@0xEFC Message_on]
  - 1141 #12: "You've opened the lock."  [LEBT sub05@0xFD6 Message_on]
  - 1141 #13: "A beautiful painting.There's a switch below. / Will you push it? "  [LEBT sub06@0xFE8 Message_on]
  - 1141 #14: "It's a big table."  [LEBT main00@0xAD4 AOT4-sce1]

### Raum 118 — Disc-Datei vorhanden, ohne Text
Port 1180 (Leon):
  - 1180 #0: "Weapon Storage"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0xAB4 AOT5-sce1]
  - 1180 #1: "A card reader.It seems that it will not / work if power is not restored."  [LEBT sub02@0xB3E Message_on]
  - 1180 #2: " / ..."  [LEBT sub03@0xB4C Message_on]
Port 1181 (Elza):
  - 1181 #0: "Weapon Storage"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0xAB4 AOT5-sce1]
  - 1181 #1: "A card reader.It seems that it will not / work if power is not restored."  [LEBT sub02@0xB3E Message_on]
  - 1181 #2: " / ..."  [LEBT sub03@0xB4C Message_on]

### Raum 11A — Disc-Datei vorhanden, ohne Text
Port 11A0 (Leon):
  - 11A0 #0: "There's a shutter switch here. Press it? "  [LEBT sub02@0x12CE Message_on]
  - 11A0 #1: "It's locked."  [LEBT main00@0xFBC Aot_reset2-sce1]
  - 11A0 #2: "It's locked from the other side."  [LEBT main00@0xF42 AOT0-sce1 wenn Ck(3,151)=0; LEBT main00@0xFEE AOT4-sce1 wenn Ck(3,206)=0]
Port 11A1 (Elza):
  - 11A1 #0: "There's a shutter switch here. Press it? "  [LEBT sub02@0x12E2 Message_on]
  - 11A1 #1: "It's locked."  [LEBT main00@0xFBC Aot_reset2-sce1]
  - 11A1 #2: "It's locked from the other side."  [LEBT main00@0xF42 AOT0-sce1 wenn Ck(3,151)=0; LEBT main00@0xFEE AOT4-sce1 wenn Ck(3,206)=0]

### Raum 11D — NUR MOD-BAUM
Port 11D0 (Leon):
  - 11D0 #0: "A panel to control the cells of / the dogs.It does not work."  [LEBT main00@0x1248 AOT2-sce1]
  - 11D0 #1: "The road is blocked and the knob / is broken."  [LEBT main00@0x125C AOT3-sce1]
  - 11D0 #2: "Boxes and dog food.Nothing useful."  [LEBT main00@0x1270 AOT4-sce1]
  - 11D0 #3: "Raccoon Police wins the award for / the best K-9 unit in the Midwest."  [LEBT main00@0x1284 AOT5-sce1]
  - 11D0 #4: "-Warning-Sewer access area. / Authorized personal only."  [LEBT main00@0x1298 AOT6-sce1]
Port 11D1 (Elza):
  - 11D1 #0: "A panel to control the cells of / the dogs.It does not work."  [LEBT main00@0x1248 AOT2-sce1]
  - 11D1 #1: "The road is blocked and the knob / is broken."  [LEBT main00@0x125C AOT3-sce1]
  - 11D1 #2: "Boxes and dog food.Nothing useful."  [LEBT main00@0x1270 AOT4-sce1]
  - 11D1 #3: "Raccoon Police wins the award for / the best K-9 unit in the Midwest."  [LEBT main00@0x1284 AOT5-sce1]
  - 11D1 #4: "-Warning-Sewer access area. / Authorized personal only."  [LEBT main00@0x1298 AOT6-sce1]

### Raum 11E — Disc-Datei vorhanden, ohne Text
Port 11E0 (Leon):
  - 11E0 #0: "Enter the first number."  [LEBT sub17@0x1ECA Message_on]
  - 11E0 #1: "Enter the second number."  [LEBT sub18@0x1EF2 Message_on wenn Ck(5,17)=0]
  - 11E0 #2: "Enter the third number."  [LEBT sub18@0x1F0C Message_on wenn NICHT[Ck(5,17)=0] / Ck(5,18)=1]
  - 11E0 #3: "Enter the fourth number."  [LEBT sub18@0x1F22 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / Ck(5,19)=1]
  - 11E0 #4: "Wrong code, try again."  [LEBT sub18@0x1F38 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / NICHT[Ck(5,19)=1] / Ck(5,20)=0]
  - 11E0 #5: "You've opened the lock."  [LEBT sub19@0x1F86 Message_on]
  - 11E0 #6: "Prisons"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0x157A AOT3-sce1 wenn Ck(3,139)=0]
  - 11E0 #7: "A card reader.You have to use the Yellow Keycard / and insert four digits tounlock the door.Will you operate the / card reader? "  [LEBT sub20@0x1F96 Message_on]
  - 11E0 #8: "You have not the Yellow Keycard / to operate it."  [LEBT sub20@0x1FBA Message_on wenn Ck(12,31)=0 / NICHT[Ck(9,138)=1]]
  - 11E0 #9: "You've used the Yellow Keycard."  [LEBT sub20@0x1FAC Message_on wenn Ck(12,31)=0 / Ck(9,138)=1]
  - 11E0 #10: "If I had some kind of pliers maybe I / could cut that wire and disable thesafety lock."  [LEBT sub00@0x179C AOT6-sce1 wenn Ck(3,127)=0; LEBT sub26@0x20AC Message_on]
  - 11E0 #11: "Will you use the Pliers? "  [LEBT sub26@0x20B2 Message_on]
  - 11E0 #12: "You've used the Pliers."  [LEBT sub21@0x1FEE Message_on]
Port 11E1 (Elza):
  - 11E1 #0: "Enter the first number."  [LEBT sub17@0x1ECA Message_on]
  - 11E1 #1: "Enter the second number."  [LEBT sub18@0x1EF2 Message_on wenn Ck(5,17)=0]
  - 11E1 #2: "Enter the third number."  [LEBT sub18@0x1F0C Message_on wenn NICHT[Ck(5,17)=0] / Ck(5,18)=1]
  - 11E1 #3: "Enter the fourth number."  [LEBT sub18@0x1F22 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / Ck(5,19)=1]
  - 11E1 #4: "Wrong code, try again."  [LEBT sub18@0x1F38 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / NICHT[Ck(5,19)=1] / Ck(5,20)=0]
  - 11E1 #5: "You've opened the lock."  [LEBT sub19@0x1F86 Message_on]
  - 11E1 #6: "Prisons"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0x157A AOT3-sce1 wenn Ck(3,139)=0]
  - 11E1 #7: "A card reader.You have to use the Yellow Keycard / and insert four digits tounlock the door.Will you operate the / card reader? "  [LEBT sub20@0x1F96 Message_on]
  - 11E1 #8: "You have not the Yellow Keycard / to operate it."  [LEBT sub20@0x1FBA Message_on wenn Ck(12,31)=0 / NICHT[Ck(9,138)=1]]
  - 11E1 #9: "You've used the Yellow Keycard."  [LEBT sub20@0x1FAC Message_on wenn Ck(12,31)=0 / Ck(9,138)=1]
  - 11E1 #10: "If I had some kind of pliers maybe I / could cut that wire and disable thesafety lock."  [LEBT sub00@0x179C AOT6-sce1 wenn Ck(3,127)=0; LEBT sub26@0x20AC Message_on]
  - 11E1 #11: "Will you use the Pliers? "  [LEBT sub26@0x20B2 Message_on]
  - 11E1 #12: "You've used the Pliers."  [LEBT sub21@0x1FEE Message_on]

### Raum 11F — NUR MOD-BAUM
Port 11F0 (Leon):
  - 11F0 #0: "-Reserve Power / Control Panel-Power can be routed to / the areas withinsufficient power using / these ten switches.Using all the power may / cause a short andcut the power supply."  [LEBT sub16@0x15A2 Message_on]
  - 11F0 #1: "Will you push them? "  [LEBT sub16@0x15A8 Message_on]
  - 11F0 #2: "Power supply OK."  [LEBT sub00@0x101E AOT1-sce1 wenn NICHT[Ck(4,238)=0]; LEBT sub18@0x1742 Message_on; LEBT sub18@0x1776 Aot_reset1-sce1]
  - 11F0 #3: "Power supply incorrect."  [kein Aufruf]
Port 11F1 (Elza):
  - 11F1 #0: "-Reserve Power / Control Panel-Power can be routed to / the areas withinsufficient power using / these ten switches.Using all the power may / cause a short andcut the power supply."  [LEBT sub16@0x15A2 Message_on]
  - 11F1 #1: "Will you push them? "  [LEBT sub16@0x15A8 Message_on]
  - 11F1 #2: "Power supply OK."  [LEBT sub00@0x101E AOT1-sce1 wenn NICHT[Ck(4,238)=0]; LEBT sub18@0x1742 Message_on; LEBT sub18@0x1776 Aot_reset1-sce1]
  - 11F1 #3: "Power supply incorrect."  [kein Aufruf]

### Raum 121 — Disc-Datei vorhanden, ohne Text
Port 1210 (Leon):
  - 1210 #20: PORT-NEU story_r36: "The cell door is open...Whoever was locked up here is gone."
Port 1211 (Elza):
  - 1211 #0: "Elza: Hey! You must be John right? / Get up now!"  [LEBT sub02@0x1FD2 Message_on]
  - 1211 #1: "John: What do you want? / I'm trying to sleep here!"  [LEBT sub02@0x2008 Message_on]
  - 1211 #2: "John: Ughh... my head is killing me."  [LEBT sub02@0x2028 Message_on]
  - 1211 #3: "John: Who the hell are you anyway?"  [LEBT sub02@0x2062 Message_on]
  - 1211 #4: "Elza: My name's Elza. / Roy sent me to get you out of here."  [LEBT sub02@0x2098 Message_on]
  - 1211 #5: "Elza: Listen. We're in serious trouble. / The whole station is crawling with / zombies!"  [LEBT sub02@0x218C Message_on]
  - 1211 #6: "John: Say what!?"  [LEBT sub02@0x21C4 Message_on]
  - 1211 #7: "Elza: Roy was injured by one of them. / I left him in the Interrogation Room, but..."  [LEBT sub02@0x21E8 Message_on]
  - 1211 #8: "John: The interrogation room? / Alright. I'm going there."  [LEBT sub02@0x2224 Message_on]
  - 1211 #9: "Elza: John. Wait!"  [LEBT sub02@0x226A Message_on]
  - 1211 #10: "It's locked.It seems that there is someone / inside..."  [LEBT sub00@0x1DEE AOT8-sce1 wenn Ck(4,236)=0]

### Raum 123 — NUR MOD-BAUM
Port 1230 (Leon):
  - 1230 #0: "Enter the first number."  [LEBT sub17@0x13AA Message_on]
  - 1230 #1: "Enter the second number."  [LEBT sub18@0x13D2 Message_on wenn Ck(5,17)=0]
  - 1230 #2: "Enter the third number."  [LEBT sub18@0x13EC Message_on wenn NICHT[Ck(5,17)=0] / Ck(5,18)=1]
  - 1230 #3: "Enter the fourth number."  [LEBT sub18@0x1402 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / Ck(5,19)=1]
  - 1230 #4: "Wrong code, try again."  [LEBT sub18@0x1418 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / NICHT[Ck(5,19)=1] / Ck(5,20)=0]
  - 1230 #5: "You've opened the lock."  [LEBT sub19@0x1466 Message_on]
  - 1230 #6: "Weapon Storage"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0xB9C AOT4-sce1 wenn Ck(3,137)=0]
  - 1230 #7: "A card reader.You have to use the Red Keycard / and insert four digits tounlock the door.Will you operate the / card reader? "  [LEBT sub20@0x1476 Message_on]
  - 1230 #8: "You have not the Red Keycard / to operate it."  [LEBT sub20@0x149A Message_on wenn Ck(12,31)=0 / NICHT[Ck(9,136)=1]]
  - 1230 #9: "You've used the Red Keycard."  [LEBT sub20@0x148C Message_on wenn Ck(12,31)=0 / Ck(9,136)=1]
  - 1230 #10: "A miserable death...He is holding a slip.The numbers 5632" are / printed on the slip."  [LEBT sub21@0x14BC Message_on]
  - 1230 #11: " / ..."  [LEBT sub22@0x14DE Message_on]
Port 1231 (Elza):
  - 1231 #0: "Enter the first number."  [LEBT sub17@0x13AA Message_on]
  - 1231 #1: "Enter the second number."  [LEBT sub18@0x13D2 Message_on wenn Ck(5,17)=0]
  - 1231 #2: "Enter the third number."  [LEBT sub18@0x13EC Message_on wenn NICHT[Ck(5,17)=0] / Ck(5,18)=1]
  - 1231 #3: "Enter the fourth number."  [LEBT sub18@0x1402 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / Ck(5,19)=1]
  - 1231 #4: "Wrong code, try again."  [LEBT sub18@0x1418 Message_on wenn NICHT[Ck(5,17)=0] / NICHT[Ck(5,18)=1] / NICHT[Ck(5,19)=1] / Ck(5,20)=0]
  - 1231 #5: "You've opened the lock."  [LEBT sub19@0x1466 Message_on]
  - 1231 #6: "Weapon Storage"It's electronically / locked.There's a card reader / on the right."  [LEBT main00@0xB9C AOT4-sce1 wenn Ck(3,137)=0]
  - 1231 #7: "A card reader.You have to use the Red Keycard / and insert four digits tounlock the door.Will you operate the / card reader? "  [LEBT sub20@0x1476 Message_on]
  - 1231 #8: "You have not the Red Keycard / to operate it."  [LEBT sub20@0x149A Message_on wenn Ck(12,31)=0 / NICHT[Ck(9,136)=1]]
  - 1231 #9: "You've used the Red Keycard."  [LEBT sub20@0x148C Message_on wenn Ck(12,31)=0 / Ck(9,136)=1]
  - 1231 #10: "A miserable death...He is holding a slip.The numbers 5632" are / printed on the slip."  [LEBT sub21@0x14BC Message_on]
  - 1231 #11: " / ..."  [LEBT sub22@0x14DE Message_on]

### Raum 124 — NUR MOD-BAUM
Port 1240 (Leon):
  - 1240 #0: "A bizarre incident occurred in the / outskirts of an American suburb called / Raccoon City."  [LEBT sub02@0x57E Message_on]
  - 1240 #1: "It was later revealed that the terrible / disaster had been caused by the T-Virus,"  [LEBT sub02@0x598 Message_on]
  - 1240 #2: "a mutagenic toxin created by the / international corporation Umbrella / incorporated."  [LEBT sub02@0x5A0 Message_on]
  - 1240 #3: "The Raccoon police department special / S.T.A.R.S Unit began an immediate / investigation."  [LEBT sub02@0x5BA Message_on]
  - 1240 #4: "The case was apparently closed thanks / to the efforts of S.T.A.R.S members / Chris Redfield and Jill Valentine."  [LEBT sub02@0x5DA Message_on]
  - 1240 #5: "But the umbrella corporation's / experiments were far from finished."  [LEBT sub02@0x5FA Message_on]
Port 1241 (Elza):
  - 1241 #0: "A bizarre incident occurred in the / outskirts of an American suburb called / Raccoon City."  [LEBT sub02@0x582 Message_on]
  - 1241 #1: "It was later revealed that the terrible / disaster had been caused by the T-Virus,"  [LEBT sub02@0x59C Message_on]
  - 1241 #2: "a mutagenic toxin created by the / international corporation Umbrella / incorporated."  [LEBT sub02@0x5A4 Message_on]
  - 1241 #3: "The Raccoon police department special / S.T.A.R.S Unit began an immediate / investigation."  [LEBT sub02@0x5BE Message_on]
  - 1241 #4: "The case was apparently closed thanks / to the efforts of S.T.A.R.S members / Chris Redfield and Jill Valentine."  [LEBT sub02@0x5DE Message_on]
  - 1241 #5: "But the umbrella corporation's / experiments were far from finished."  [LEBT sub02@0x5FE Message_on]

### Raum 200 — Disc-Datei vorhanden, ohne Text
Port 2000 (Leon):
  - 2000 #0: "Leon: Ada, Marvin, hold on."  [LEBT sub02@0x1832 Message_on]
  - 2000 #1: "Marvin: What's up Leon?"  [LEBT sub02@0x184A Message_on]
  - 2000 #2: "Leon: There is a little girl that / came in here, we have to find her!"  [LEBT sub02@0x186E Message_on]
  - 2000 #3: "Marvin: A little girl, are you sure?"  [LEBT sub02@0x189E Message_on]
  - 2000 #4: "Leon: Yes, before meeting Ada I saw / her coming in this way, if I hadn't"  [LEBT sub02@0x18C2 Message_on]
  - 2000 #5: "seen her maybe we wouldn't have / found this way out."  [LEBT sub02@0x18EC Message_on]
  - 2000 #6: "Leon: She must have thought I was / a zombie."  [LEBT sub02@0x1904 Message_on]
  - 2000 #7: "Ada: So there is no time to lose, / we do not know what things are in"  [LEBT sub02@0x1926 Message_on]
  - 2000 #8: "the sewers, zombies are not the only / things we should worry about here."  [LEBT sub02@0x197C Message_on]
  - 2000 #9: "Leon: Okay, it will be better to / separate and cover more ground."  [LEBT sub02@0x19A0 Message_on]
  - 2000 #10: "Leon: Marvin sees Ada up front."  [LEBT sub02@0x19DE Message_on]
  - 2000 #11: "Marvin: Understood. If something / happens, this area will be to meet."  [LEBT sub02@0x19FE Message_on]
Port 2001 (Elza):
  - 2001 #0: "John: Elza, over here!"  [LEBT sub02@0x18BE Message_on]
  - 2001 #1: "John: Through this staircase you can / access the basement garage."  [LEBT sub04@0x19B4 Message_on]
  - 2001 #2: "Elza: Okay, I'll go for her."  [LEBT sub04@0x19D8 Message_on]
  - 2001 #3: "John: Meanwhile I will take a look / around, maybe she is down here too."  [LEBT sub04@0x19FC Message_on]
  - 2001 #4: "Elza: Good idea, but be careful."  [LEBT sub04@0x1A34 Message_on]
  - 2001 #5: "John: You too."  [LEBT sub04@0x1A58 Message_on]
  - 2001 #6: "Elza: Sherry... thank god you're / fine! I was so worried!"  [LEBT sub05@0x1AD2 Message_on]
  - 2001 #7: "Elza: What happened? why didn't you / wait for me?"  [LEBT sub05@0x1B28 Message_on]
  - 2001 #8: "Sherry: It was because of my daddy, / I've heard him call me!"  [LEBT sub05@0x1B5C Message_on]
  - 2001 #9: "Sherry: I went down here to find him, / but I couldn't..."  [LEBT sub05@0x1B78 Message_on]
  - 2001 #10: "Elza: I understand, don't worry honey, / if he's here we'll find him, okay?"  [LEBT sub05@0x1BC8 Message_on]
  - 2001 #11: "Elza: My, that's a nice pocket watch!"  [LEBT sub05@0x1C26 Message_on]
  - 2001 #12: "Sherry: My mom gave it to me, she says / it's for good luck."  [LEBT sub05@0x1C42 Message_on]
  - 2001 #13: "Elza: I see, I guess your mom is right."  [LEBT sub05@0x1C7E Message_on]
  - 2001 #14: "Elza: Come on Sherry, let's look for / John and get out of here!"  [LEBT sub05@0x1CAE Message_on]

### Raum 201 — NUR MOD-BAUM
Port 2010 (Leon):
  - 2010 #0: "Will you take the Crank? "  [LEBT sub02@0x8BC Message_on]
  - 2010 #1: "You've taken the Crank."  [LEBT sub02@0x8E4 Message_on wenn Ck(12,31)=0]
  - 2010 #2: "Itembox is not available in / this preview."  [LEBT sub03@0x8F0 Message_on]
  - 2010 #3: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0x7BC AOT6-sce1]
  - 2010 #4: "An old typewriter.Why use it if there are / computers?"  [LEBT main00@0x7D0 AOT7-sce1]
Port 2011 (Elza):
  - 2011 #0: "Will you take the Crank? "  [LEBT sub02@0x8DA Message_on]
  - 2011 #1: "You've taken the Crank."  [LEBT sub02@0x902 Message_on wenn Ck(12,31)=0]
  - 2011 #2: "Itembox is not available in / this preview."  [LEBT sub03@0x90E Message_on]
  - 2011 #3: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0x7BC AOT6-sce1]
  - 2011 #4: "An old typewriter.Why use it if there are / computers?"  [LEBT main00@0x7D0 AOT7-sce1]

### Raum 202 — NUR MOD-BAUM
Port 2020 (Leon):
  - 2020 #0: "It's a square hole."  [LEBT sub00@0x958 AOT3-sce1 wenn Ck(3,114)=0; LEBT sub02@0xA5C Message_on]
  - 2020 #1: "Will you use the Crank? "  [LEBT sub02@0xA62 Message_on]
  - 2020 #2: "You've opened the lock."  [LEBT sub03@0xB2A Message_on]
  - 2020 #3: "It's locked."  [LEBT main00@0x8C0 AOT5-sce1]
Port 2021 (Elza):
  - 2021 #0: "It's a square hole."  [LEBT sub00@0x958 AOT3-sce1 wenn Ck(3,114)=0; LEBT sub02@0xA5C Message_on]
  - 2021 #1: "Will you use the Crank? "  [LEBT sub02@0xA62 Message_on]
  - 2021 #2: "You've opened the lock."  [LEBT sub03@0xB2A Message_on]
  - 2021 #3: "It's locked."  [LEBT main00@0x8C0 AOT5-sce1]

### Raum 203 — Disc-Datei vorhanden, ohne Text
Port 2030 (Leon):
  - 2030 #0: "It's locked from the other side."  [LEBT main00@0x1B12 AOT2-sce1 wenn Ck(3,219)=0]
  - 2030 #1: "The elevator is not responding...Maybe it should be activated / from somewhere."  [LEBT sub01@0x1E18 Aot_reset5-sce1 wenn Ck(3,126)=0; LEBT sub01@0x1E22 Aot_reset7-sce1 wenn Ck(3,126)=0]
  - 2030 #2: "Will you take the Fuse? "  [LEBT sub06@0x1FE4 Message_on]
  - 2030 #3: "You've taken the Fuse."  [LEBT sub06@0x2000 Message_on wenn Ck(12,31)=0]
Port 2031 (Elza):
  - 2031 #0: "It's locked from the other side."  [LEBT main00@0x1B12 AOT2-sce1 wenn Ck(3,219)=0]
  - 2031 #1: "The elevator is not responding...Maybe it should be activated / from somewhere."  [LEBT sub01@0x1E18 Aot_reset5-sce1 wenn Ck(3,126)=0; LEBT sub01@0x1E22 Aot_reset7-sce1 wenn Ck(3,126)=0]
  - 2031 #2: "Will you take the Fuse? "  [LEBT sub06@0x1FE4 Message_on]
  - 2031 #3: "You've taken the Fuse."  [LEBT sub06@0x2000 Message_on wenn Ck(12,31)=0]

### Raum 204 — NUR MOD-BAUM
Port 2040 (Leon):
  - 2040 #0: "Notice of damage door:While doing maintenance and / construction work on the Raccoonsewage system we found a problem / with a door, it seems that itrusted due to water leaks, we tried / everything and nothing works!Unless we use explosives or something / heavy it doesn't seem I'm going togive in, I'll have to ask the manager / for options for this problem.Bill Johnson, / Knights Construction"  [LEBT sub00@0x141E AOT8-sce1 wenn Ck(3,146)=0; LEBT sub07@0x16E8 Message_on]
  - 2040 #1: "An old typewriter, perhaps someone will / find it useful even in these times."  [LEBT sub00@0x14BC AOT10-sce1]
  - 2040 #2: "This old console is useless."  [LEBT sub00@0x14D0 AOT11-sce1]
  - 2040 #3: "The body of a sewer worker... / He has something in his hands."  [LEBT sub08@0x172A Message_on]
  - 2040 #4: "Bomb activated, I have 10 seconds / before it detonates!"  [LEBT sub02@0x1636 Message_on]
  - 2040 #5: "Will you use the Time Bomb?    e         /            4   2  /           /      3  x  x                    "  [LEBT sub07@0x16EE Message_on]
Port 2041 (Elza):
  - 2041 #0: "Notice of damage door:While doing maintenance and / construction work on the Raccoonsewage system we found a problem / with a door, it seems that itrusted due to water leaks, we tried / everything and nothing works!Unless we use explosives or something / heavy it doesn't seem I'm going togive in, I'll have to ask the manager / for options for this problem.Bill Johnson, / Knights Construction"  [LEBT sub00@0x141E AOT8-sce1 wenn Ck(3,146)=0; LEBT sub07@0x16E8 Message_on]
  - 2041 #1: "An old typewriter, perhaps someone will / find it useful even in these times."  [LEBT sub00@0x14BC AOT10-sce1]
  - 2041 #2: "This old console is useless."  [LEBT sub00@0x14D0 AOT11-sce1]
  - 2041 #3: "The body of a sewer worker... / He has something in his hands."  [LEBT sub08@0x172A Message_on]
  - 2041 #4: "Bomb activated, I have 10 seconds / before it detonates!"  [LEBT sub02@0x1636 Message_on]
  - 2041 #5: "Will you use the Time Bomb?    e         /            4   2  /           /      3  x  x                    "  [LEBT sub07@0x16EE Message_on]

### Raum 205 — NUR MOD-BAUM
Port 2050 (Leon):
  - 2050 #0: "A lever to fill and drain water. / Will you move it? "  [LEBT sub02@0x220E Message_on; LEBT sub04@0x22E6 Message_on]
  - 2050 #1: "The water is blocking the / entrance."  [LEBT sub00@0x1EC6 AOT7-sce1 wenn Ck(3,109)=0 / NICHT[Ck(3,104)=0]; LEBT sub00@0x1F74 AOT7-sce1 wenn NICHT[Ck(3,109)=0]; LEBT sub02@0x222A Aot_reset7-sce1 wenn Ck(12,31)=0; LEBT sub04@0x233C Aot_reset7-sce1 wenn Ck(12,31)=0 / Ck(3,104)=0]
  - 2050 #2: "I have nothing else to do / here."  [LEBT sub00@0x1F60 AOT6-sce1 wenn NICHT[Ck(3,109)=0]; LEBT sub02@0x2220 Aot_reset6-sce1 wenn Ck(12,31)=0]
Port 2051 (Elza):
  - 2051 #0: "A lever to fill and drain water. / Will you move it? "  [LEBT sub02@0x220E Message_on; LEBT sub04@0x22E6 Message_on]
  - 2051 #1: "The water is blocking the / entrance."  [LEBT sub00@0x1EC6 AOT7-sce1 wenn Ck(3,109)=0 / NICHT[Ck(3,104)=0]; LEBT sub00@0x1F74 AOT7-sce1 wenn NICHT[Ck(3,109)=0]; LEBT sub02@0x222A Aot_reset7-sce1 wenn Ck(12,31)=0; LEBT sub04@0x233C Aot_reset7-sce1 wenn Ck(12,31)=0 / Ck(3,104)=0]
  - 2051 #2: "I have nothing else to do / here."  [LEBT sub00@0x1F60 AOT6-sce1 wenn NICHT[Ck(3,109)=0]; LEBT sub02@0x2220 Aot_reset6-sce1 wenn Ck(12,31)=0]

### Raum 206 — NUR MOD-BAUM
Port 2060 (Leon):
  - 2060 #0: "The generator does not appear to / be on.There are four switches...Will you push them? "  [LEBT sub12@0x15BA Message_on]
  - 2060 #1: "I need to insert the missing / fuse before I can operate this."  [LEBT sub12@0x15D0 Message_on wenn Ck(12,31)=0 / Ck(3,144)=0]
  - 2060 #2: "It seems that the generator is / working now."  [LEBT sub00@0x1082 AOT8-sce1 wenn NICHT[Ck(3,126)=0]; LEBT sub13@0x1632 Aot_reset8-sce1; LEBT sub13@0x1640 Message_on]
  - 2060 #3: "One of the fuses is missing..."  [LEBT sub00@0x10A6 AOT9-sce1 wenn Ck(3,108)=0; LEBT sub18@0x1678 Message_on]
  - 2060 #4: "Will you use the Fuse? "  [LEBT sub18@0x167E Message_on]
  - 2060 #5: "You've used the Fuse."  [LEBT sub19@0x16C8 Message_on]
  - 2060 #6: "The fuse is in place."  [LEBT sub00@0x10DE AOT9-sce1 wenn NICHT[Ck(3,108)=0] / NICHT[Ck(3,144)=0]; LEBT sub19@0x16B8 Aot_reset9-sce1]
Port 2061 (Elza):
  - 2061 #0: "The generator does not appear to / be on.There are four switches...Will you push them? "  [LEBT sub12@0x15BA Message_on]
  - 2061 #1: "I need to insert the missing / fuse before I can operate this."  [LEBT sub12@0x15D0 Message_on wenn Ck(12,31)=0 / Ck(3,144)=0]
  - 2061 #2: "It seems that the generator is / working now."  [LEBT sub00@0x1082 AOT8-sce1 wenn NICHT[Ck(3,126)=0]; LEBT sub13@0x1632 Aot_reset8-sce1; LEBT sub13@0x1640 Message_on]
  - 2061 #3: "One of the fuses is missing..."  [LEBT sub00@0x10A6 AOT9-sce1 wenn Ck(3,108)=0; LEBT sub18@0x1678 Message_on]
  - 2061 #4: "Will you use the Fuse? "  [LEBT sub18@0x167E Message_on]
  - 2061 #5: "You've used the Fuse."  [LEBT sub19@0x16C8 Message_on]
  - 2061 #6: "The fuse is in place."  [LEBT sub00@0x10DE AOT9-sce1 wenn NICHT[Ck(3,108)=0] / NICHT[Ck(3,144)=0]; LEBT sub19@0x16B8 Aot_reset9-sce1]

### Raum 208 — NUR MOD-BAUM
Port 2080 (Leon):
  - 2080 #0: "From here maybe I can control the / water level.Will you operate the computer? "  [LEBT sub02@0xC86 Message_on]
  - 2080 #1: "The water level is very high...I can't get down to the floor below."  [LEBT sub00@0xC06 AOT3-sce1 wenn Ck(3,245)=0]
Port 2081 (Elza):
  - 2081 #0: "From here maybe I can control the / water level.Will you operate the computer? "  [LEBT sub02@0xC86 Message_on]
  - 2081 #1: "The water level is very high...I can't get down to the floor below."  [LEBT sub00@0xC06 AOT3-sce1 wenn Ck(3,245)=0]

### Raum 20A — NUR MOD-BAUM
Port 20A0 (Leon):
  - 20A0 #0: "There's a shutter switch here. Press it? "  [LEBT sub05@0x1E04 Message_on wenn Ck(5,0)=0]
  - 20A0 #1: "It's locked."  [LEBT main00@0x189E AOT8-sce1]
  - 20A0 #2: "You've opened the lock."  [LEBT sub10@0x1FF4 Message_on]
Port 20A1 (Elza):
  - 20A1 #0: "There's a shutter switch here. Press it? "  [LEBT sub05@0x1E04 Message_on wenn Ck(5,0)=0]
  - 20A1 #1: "It's locked."  [LEBT main00@0x189E AOT8-sce1]
  - 20A1 #2: "You've opened the lock."  [LEBT sub10@0x1FF4 Message_on]

### Raum 302 — Disc-Datei vorhanden, ohne Text
Port 3020 (Leon):
  - 3020 #0: "Leon: Dammit! the door does not open!"  [LEBT sub02@0xCB8 Message_on]
  - 3020 #1: "Marvin: There's an open window, maybe / we could enter through there?"  [LEBT sub02@0xD12 Message_on]
  - 3020 #2: "Marvin: Leon give me an hand!"  [LEBT sub02@0xD32 Message_on]
  - 3020 #3: "It's locked from the other side."  [kein Aufruf]
  - 3020 #4: "The car is locked, I can't get inside."  [LEBT main00@0xB8E AOT4-sce1; LEBT main00@0xBA2 AOT5-sce1]
  - 3020 #5: "The panel is smashed and doesn't work."  [LEBT main00@0xBB6 AOT6-sce1]
  - 3020 #6: " / It's locked."  [LEBT main00@0xB5A AOT1-sce1]
Port 3021 (Elza):
  - 3021 #0: "It's locked from the other side."  [kein Aufruf]
  - 3021 #1: "The car is locked, I can't get inside."  [LEBT main00@0xB6E AOT3-sce1; LEBT main00@0xB82 AOT4-sce1]
  - 3021 #2: "It's an open window. / I could boost Sherry in there.Not a good idea.Too dark, and there may be zombies / inside."  [LEBT sub00@0xBE0 AOT2-sce1 wenn Ck(4,199)=1]
  - 3021 #3: "The panel is smashed and doesn't work."  [LEBT main00@0xB96 AOT5-sce1]
  - 3021 #4: " / It's locked."  [LEBT main00@0xB5A AOT1-sce1]

### Raum 304 — Disc-Datei vorhanden, ohne Text
Port 3040 (Leon):
  - 3040 #0: "You've opened the lock."  [LEBT sub02@0x14F2 Message_on]
  - 3040 #1: "There's a button here. Push it? "  [LEBT sub03@0x1504 Message_on; LEBT sub04@0x15AA Message_on]
  - 3040 #2: "The boxes are blocking the way. / I must find a way to move them."  [LEBT sub00@0x1362 AOT8-sce1 wenn Ck(3,176)=0; LEBT sub00@0x13E0 AOT8-sce1 wenn NICHT[Ck(3,176)=0] / Ck(3,177)=0]
  - 3040 #3: "The control panel is not / responding...Maybe it should be activated / from somewhere."  [LEBT sub04@0x15B8 Message_on wenn Ck(12,31)=0]
Port 3041 (Elza):
  - 3041 #0: "You've opened the lock."  [LEBT sub02@0x14F2 Message_on]
  - 3041 #1: "There's a button here. Push it? "  [LEBT sub03@0x1504 Message_on; LEBT sub04@0x15AA Message_on]
  - 3041 #2: "The boxes are blocking the way. / I must find a way to move them."  [LEBT sub00@0x1362 AOT8-sce1 wenn Ck(3,176)=0; LEBT sub00@0x13E0 AOT8-sce1 wenn NICHT[Ck(3,176)=0] / Ck(3,177)=0]
  - 3041 #3: "The control panel is not / responding...Maybe it should be activated / from somewhere."  [LEBT sub04@0x15B8 Message_on wenn Ck(12,31)=0]

### Raum 305 — NUR MOD-BAUM
Port 3050 (Leon):
  - 3050 #0: "There's a button here. Push it? "  [LEBT sub06@0x2414 Message_on; LEBT sub07@0x2498 Message_on]
  - 3050 #1: "The control panel is not / responding...Maybe it should be activated / from somewhere."  [kein Aufruf]
  - 3050 #2: "The elevator is not responding...Maybe it should be activated / from somewhere."  [kein Aufruf]
  - 3050 #3: "The computer seems to be working. / There is a note:In case the machinery stops / working, it will be necessary touse the Red Master Keycard to / restore the system.""  [LEBT main00@0x1F94 AOT10-sce1 wenn Ck(3,178)=0; LEBT sub10@0x2578 Message_on]
  - 3050 #4: "Will you use the Red Master Keycard? "  [LEBT sub10@0x257E Message_on]
  - 3050 #5: "You've used the Red Master Keycard."  [LEBT sub15@0x2608 Message_on]
  - 3050 #6: "The system has been successfully / restored."  [LEBT sub15@0x2616 Message_on]
  - 3050 #7: "I don't need to use this anymore."  [LEBT main00@0x2002 AOT10-sce1 wenn NICHT[Ck(3,178)=0] / NICHT[Ck(3,179)=0]; LEBT sub15@0x261C Aot_reset10-sce1]
Port 3051 (Elza):
  - 3051 #0: "There's a button here. Push it? "  [LEBT sub06@0x2414 Message_on; LEBT sub07@0x2498 Message_on]
  - 3051 #1: "The control panel is not / responding...Maybe it should be activated / from somewhere."  [kein Aufruf]
  - 3051 #2: "The elevator is not responding...Maybe it should be activated / from somewhere."  [kein Aufruf]
  - 3051 #3: "The computer seems to be working. / There is a note:In case the machinery stops / working, it will be necessary touse the Red Master Keycard to / restore the system.""  [LEBT main00@0x1F94 AOT10-sce1 wenn Ck(3,178)=0; LEBT sub10@0x2578 Message_on]
  - 3051 #4: "Will you use the Red Master Keycard? "  [LEBT sub10@0x257E Message_on]
  - 3051 #5: "You've used the Red Master Keycard."  [LEBT sub15@0x2608 Message_on]
  - 3051 #6: "The system has been successfully / restored."  [LEBT sub15@0x2616 Message_on]
  - 3051 #7: "I don't need to use this anymore."  [LEBT main00@0x2002 AOT10-sce1 wenn NICHT[Ck(3,178)=0] / NICHT[Ck(3,179)=0]; LEBT sub15@0x261C Aot_reset10-sce1]

### Raum 30A — NUR MOD-BAUM
Port 30A0 (Leon):
  - 30A0 #0: "Itembox is not available in / this preview."  [LEBT sub02@0x878 Message_on]
  - 30A0 #1: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0x76C AOT3-sce1]
  - 30A0 #2: "A detailed map of product / distribution in the city."  [LEBT main00@0x780 AOT4-sce1]
  - 30A0 #3: "It's a disaster."  [LEBT main00@0x794 AOT5-sce1]
  - 30A0 #4: "Wait, there's something here."  [kein Aufruf]
  - 30A0 #5: "There is nothing useful."  [LEBT main00@0x7A8 AOT6-sce1]
  - 30A0 #6: "There are only papers lying / around."  [LEBT main00@0x7BC AOT7-sce1]
  - 30A0 #7: "It's empty."  [LEBT main00@0x7D0 AOT8-sce1]
  - 30A0 #8: "There is nothing more than / papers thrown everywhere.I do not see that it is useful."  [LEBT main00@0x7E4 AOT9-sce1]
  - 30A0 #9: "A calendar and several sheets.It must be where they had part / of the control of the orders."  [LEBT main00@0x7F8 AOT10-sce1]
Port 30A1 (Elza):
  - 30A1 #0: "Itembox is not available in / this preview."  [LEBT sub02@0x878 Message_on]
  - 30A1 #1: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [LEBT main00@0x76C AOT3-sce1]
  - 30A1 #2: "A detailed map of product / distribution in the city."  [LEBT main00@0x780 AOT4-sce1]
  - 30A1 #3: "It's a disaster."  [LEBT main00@0x794 AOT5-sce1]
  - 30A1 #4: "Wait, there's something here."  [kein Aufruf]
  - 30A1 #5: "There is nothing useful."  [LEBT main00@0x7A8 AOT6-sce1]
  - 30A1 #6: "There are only papers lying / around."  [LEBT main00@0x7BC AOT7-sce1]
  - 30A1 #7: "It's empty."  [LEBT main00@0x7D0 AOT8-sce1]
  - 30A1 #8: "There is nothing more than / papers thrown everywhere.I do not see that it is useful."  [LEBT main00@0x7E4 AOT9-sce1]
  - 30A1 #9: "A calendar and several sheets.It must be where they had part / of the control of the orders."  [LEBT main00@0x7F8 AOT10-sce1]

### Raum 30B — NUR MOD-BAUM
Port 30B0 (Leon):
  - 30B0 #0: "Itembox is not available in / this preview."  [LEBT sub02@0x98C Message_on]
  - 30B0 #1: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [kein Aufruf]
Port 30B1 (Elza):
  - 30B1 #0: "Itembox is not available in / this preview."  [LEBT sub02@0x98C Message_on]
  - 30B1 #1: "It's a computer.You can save your progress / with this.Save is not available in / this preview."  [kein Aufruf]

### Raum 30C — NUR MOD-BAUM
Port 30C0 (Leon):
  - 30C0 #0: "You've opened the lock."  [kein Aufruf]
  - 30C0 #1: " / It's locked."  [LEBT main00@0xB9A AOT2-sce1]
Port 30C1 (Elza):
  - 30C1 #0: "You've opened the lock."  [kein Aufruf]
  - 30C1 #1: "Elza: Who are you?"  [LEBT sub03@0xCB2 Message_on]
  - 30C1 #2: "Officer: I'm Marvin Branagh, RPD. / I'm here with Leon Kennedy."  [LEBT sub03@0xCD6 Message_on]
  - 30C1 #3: "Elza: Leon?! How is he? / We got separated back at the RPD."  [LEBT sub03@0xD0E Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Elza: Leon? I don't know him. I never ran into him at the RPD."
  - 30C1 #4: "Marvin: He's all right and he's helping / me and Ada escape the city."  [LEBT sub03@0xD46 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Marvin: He's a rookie, and he's helping me and Ada escape the city."
  - 30C1 #5: "Elza: Ada?"  [LEBT sub03@0xD7E Message_on]
  - 30C1 #6: "Marvin: Someone we rescued from the / RPD."  [LEBT sub03@0xDA2 Message_on]
  - 30C1 #7: "Elza: Will you tell him that me and John / and Sherry are here and we're all right?"  [LEBT sub03@0xDC6 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Elza: Will you tell them that me and John and Sherry are here and we're all right?"
  - 30C1 #8: "Marvin: Will do. See you soon!"  [LEBT sub03@0xDFE Message_on]
  - 30C1 #9: " / I need to hurry up and save the others..."  [LEBT sub01@0xC12 Aot_reset2-sce1 wenn Ck(4,16)=0; LEBT sub01@0xC1C Aot_reset3-sce1 wenn Ck(4,16)=0]
  - 30C1 #10: " / It's locked."  [LEBT main00@0xB9A AOT2-sce1]

### Raum 30D — NUR MOD-BAUM
Port 30D0 (Leon):
  - 30D0 #0: "The car is locked, I can't get inside."  [LEBT main00@0x12CE AOT5-sce1; LEBT main00@0x12E2 AOT6-sce1]
  - 30D0 #1: "The panel is smashed and doesn't work."  [LEBT main00@0x12F6 AOT7-sce1]
Port 30D1 (Elza):
  - 30D1 #0: "The car is locked, I can't get inside."  [LEBT main00@0x12CE AOT5-sce1; LEBT main00@0x12E2 AOT6-sce1]
  - 30D1 #1: "The panel is smashed and doesn't work."  [LEBT main00@0x12F6 AOT7-sce1]

### Raum 402 — NUR MOD-BAUM
Port 4020 (Leon):
  - 4020 #0: " / There's a map of the entire area."  [LEBT sub11@0xB08 Message_on]
  - 4020 #1: " / The button doesn't respond..."  [LEBT sub12@0xB44 Message_on]
Port 4021 (Elza):
  - 4021 #0: " / There's a map of the entire area."  [LEBT sub11@0xB08 Message_on]
  - 4021 #1: " / The button doesn't respond..."  [LEBT sub12@0xB44 Message_on]

### Raum 404 — NUR MOD-BAUM
Port 4040 (Leon):
  - 4040 #0: "Left: Emergency Staircase / Right: Private Rooms"  [LEBT main00@0xF9E AOT7-sce1]
Port 4041 (Elza):
  - 4041 #0: "Left: Emergency Staircase / Right: Private Rooms"  [LEBT main00@0xF9E AOT7-sce1]

### Raum 405 — NUR MOD-BAUM
Port 4050 (Leon):
  - 4050 #0: "There's nothing useful in this / locker."  [LEBT main00@0x1D16 AOT14-sce1; LEBT main00@0x1D2A AOT15-sce1; LEBT main00@0x1D3E AOT16-sce1; LEBT main00@0x1D52 AOT17-sce1; LEBT main00@0x1D66 AOT18-sce1]
  - 4050 #1: "A researcher's bed."  [LEBT main00@0x1D7A AOT19-sce1; LEBT main00@0x1D8E AOT20-sce1; LEBT main00@0x1DA2 AOT21-sce1; LEBT main00@0x1DB6 AOT22-sce1; LEBT main00@0x1DCA AOT23-sce1; LEBT main00@0x1DDE AOT24-sce1; LEBT main00@0x1DF2 AOT25-sce1; LEBT main00@0x1E06 AOT26-sce1]
  - 4050 #2: "A cabinet full of books and / reference materials.Most appear to be about / chemistry and biology."  [LEBT main00@0x1E1A AOT27-sce1; LEBT main00@0x1E2E AOT28-sce1; LEBT main00@0x1E42 AOT29-sce1]
  - 4050 #3: "There's a memo here.To David: I found your ID card / in the hallway.Please don't lose it again. / - Eric"  [kein Aufruf]
Port 4051 (Elza):
  - 4051 #0: "There's nothing useful in this / locker."  [LEBT main00@0x1D16 AOT14-sce1; LEBT main00@0x1D2A AOT15-sce1; LEBT main00@0x1D3E AOT16-sce1; LEBT main00@0x1D52 AOT17-sce1; LEBT main00@0x1D66 AOT18-sce1]
  - 4051 #1: "A researcher's bed."  [LEBT main00@0x1D7A AOT19-sce1; LEBT main00@0x1D8E AOT20-sce1; LEBT main00@0x1DA2 AOT21-sce1; LEBT main00@0x1DB6 AOT22-sce1; LEBT main00@0x1DCA AOT23-sce1; LEBT main00@0x1DDE AOT24-sce1; LEBT main00@0x1DF2 AOT25-sce1; LEBT main00@0x1E06 AOT26-sce1]
  - 4051 #2: "A cabinet full of books and / reference materials.Most appear to be about / chemistry and biology."  [LEBT main00@0x1E1A AOT27-sce1; LEBT main00@0x1E2E AOT28-sce1; LEBT main00@0x1E42 AOT29-sce1]
  - 4051 #3: "There's a memo here.To David: I found your ID card / in the hallway.Please don't lose it again. / - Eric"  [kein Aufruf]

### Raum 407 — NUR MOD-BAUM
Port 4070 (Leon):
  - 4070 #0: "The door won't open!"  [LEBT main00@0x1222 AOT1-sce1 wenn NICHT[Ck(3,76)=0]]
Port 4071 (Elza):
  - 4071 #0: "The door won't open!"  [LEBT main00@0x1222 AOT1-sce1 wenn NICHT[Ck(3,76)=0]]

### Raum 408 — NUR MOD-BAUM
Port 4080 (Leon):
  - 4080 #0: "Cell Cultivation Room"It's electronically locked."  [LEBT main00@0xAE4 AOT2-sce1 wenn Ck(3,124)=0]
Port 4081 (Elza):
  - 4081 #0: "Cell Cultivation Room"It's electronically locked."  [LEBT main00@0xAE4 AOT2-sce1 wenn Ck(3,124)=0]

### Raum 409 — Disc-Datei vorhanden, ohne Text
Port 4090 (Leon):
  - 4090 #0: "An ultra low temperature freezer / used to store viral samples.There is a hole to set the Enzyme  / in."  [LEBT main00@0xAB6 AOT1-sce1]
  - 4090 #1: "The enzyme is already in place. / I need to operate the machine tosynthesize a sample of the T-Virus."  [kein Aufruf]
  - 4090 #2: "A control box for operating the / robotic arm.Will you use it? "  [kein Aufruf]
  - 4090 #3: "There's a slot for swiping a keycard."  [kein Aufruf]
  - 4090 #4: "The keypanel has already been / activated."  [kein Aufruf]
  - 4090 #5: "A numerical keypanel.An ID card is required to operate / it."  [LEBT main00@0xACA AOT2-sce1]
  - 4090 #6: "A numerical keypanel.If I had the codes, I could use the / machine, but..."  [kein Aufruf]
  - 4090 #7: "It looks like a computer for organizing / data files."  [LEBT main00@0xADE AOT3-sce1]
Port 4091 (Elza):
  - 4091 #0: "An ultra low temperature freezer / used to store viral samples.There is a hole to set the Enzyme  / in."  [LEBT main00@0xAB6 AOT1-sce1]
  - 4091 #1: "The enzyme is already in place. / I need to operate the machine tosynthesize a sample of the T-Virus."  [kein Aufruf]
  - 4091 #2: "A control box for operating the / robotic arm.Will you use it? "  [kein Aufruf]
  - 4091 #3: "There's a slot for swiping a keycard."  [kein Aufruf]
  - 4091 #4: "The keypanel has already been / activated."  [kein Aufruf]
  - 4091 #5: "A numerical keypanel.An ID card is required to operate / it."  [LEBT main00@0xACA AOT2-sce1]
  - 4091 #6: "A numerical keypanel.If I had the codes, I could use the / machine, but..."  [kein Aufruf]
  - 4091 #7: "It looks like a computer for organizing / data files."  [LEBT main00@0xADE AOT3-sce1]

### Raum 40B — NUR MOD-BAUM
Port 40B0 (Leon):
  - 40B0 #0: "It looks like a large vat used to / store research specimens butthere's nothing inside..."  [LEBT main00@0x622 AOT1-sce1]
  - 40B0 #1: "It looks like the remains of / a failed experiment..."  [LEBT main00@0x636 AOT2-sce1]
  - 40B0 #2: "It's an operating table.There's what seems to be blood / on it..."  [LEBT main00@0x64A AOT3-sce1]
  - 40B0 #3: "It's been broken from the inside."  [LEBT main00@0x666 AOT4-sce1]
Port 40B1 (Elza):
  - 40B1 #0: "It looks like a large vat used to / store research specimens butthere's nothing inside..."  [LEBT main00@0x622 AOT1-sce1]
  - 40B1 #1: "It looks like the remains of / a failed experiment..."  [LEBT main00@0x636 AOT2-sce1]
  - 40B1 #2: "It's an operating table.There's what seems to be blood / on it..."  [LEBT main00@0x64A AOT3-sce1]
  - 40B1 #3: "It's been broken from the inside."  [LEBT main00@0x666 AOT4-sce1]

### Raum 500 — NUR MOD-BAUM
Port 5000 (Leon):
  - 5000 #0: "An elevator. Pressing the / button has no effect.It hasn't been used for a / long time."  [LEBT main00@0x88C AOT3-sce1 wenn Ck(3,78)=0]
Port 5001 (Elza):
  - 5001 #0: "An elevator. Pressing the / button has no effect.It hasn't been used for a / long time."  [LEBT main00@0x88C AOT3-sce1 wenn Ck(3,78)=0]

### Raum 502 — NUR MOD-BAUM
Port 5021 (Elza):
  - 5021 #0: "Sherry must have crawled through here, / but the hole is too small for me.There must be another way to / get back there."  [LEBT sub00@0x1302 AOT6-sce1 wenn Ck(4,192)=0]
  - 5021 #1: "Elza: Sherry..."  [LEBT sub02@0x1392 Message_on]

### Raum 503 — Disc-Datei vorhanden, ohne Text
Port 5030 (Leon):
  - 5030 #0: "A map of the lab is available here.Will you file the laboratory map? "  [LEBT sub02@0x10AC Message_on]
  - 5030 #1: "You've taken the laboratory map."  [LEBT sub02@0x10CC Message_on wenn Ck(12,31)=0]
  - 5030 #2: "It's a row of monitors.They appear to be used for showing / security footage, but they're onlydisplaying static..."  [LEBT main00@0xEE6 AOT4-sce1]
Port 5031 (Elza):
  - 5031 #0: "A map of the lab is available here.Will you file the laboratory map? "  [LEBT sub02@0x110A Message_on]
  - 5031 #1: "You've taken the laboratory map."  [LEBT sub02@0x112A Message_on wenn Ck(12,31)=0]
  - 5031 #2: "It's a row of monitors.They appear to be used for showing / security footage, but they're onlydisplaying static..."  [LEBT main00@0xEE6 AOT4-sce1]
  - 5031 #3: "Elza: Sherry!"  [LEBT sub03@0x11EC Message_on]

### Raum 505 — NUR MOD-BAUM
Port 5050 (Leon):
  - 5050 #0: "Will you operate the / computer? "  [LEBT sub02@0x86E Message_on]
  - 5050 #1: "Culture Experiment / RoomStaff Registry."  [LEBT sub02@0x882 Message_on wenn Ck(12,31)=0]
  - 5050 #2: "Commencing with temporary / registration."  [LEBT sub02@0x888 Message_on wenn Ck(12,31)=0]
  - 5050 #3: "Register your fingerprint."  [LEBT sub02@0x88E Message_on wenn Ck(12,31)=0]
  - 5050 #4: "Will you register? "  [kein Aufruf]
  - 5050 #5: "Registration complete."  [LEBT sub03@0x8E0 Message_on]
  - 5050 #6: "Guest registration is valid / for 24 hours."  [LEBT sub03@0x8E6 Message_on]
  - 5050 #7: "I don't need to use this anymore."  [LEBT sub00@0x7BA AOT1-sce1 wenn NICHT[Ck(3,123)=0]; LEBT sub03@0x8F4 Aot_reset1-sce1]
Port 5051 (Elza):
  - 5051 #0: "Will you operate the / computer? "  [LEBT sub02@0x86E Message_on]
  - 5051 #1: "Culture Experiment / RoomStaff Registry."  [LEBT sub02@0x882 Message_on wenn Ck(12,31)=0]
  - 5051 #2: "Commencing with temporary / registration."  [LEBT sub02@0x888 Message_on wenn Ck(12,31)=0]
  - 5051 #3: "Register your fingerprint."  [LEBT sub02@0x88E Message_on wenn Ck(12,31)=0]
  - 5051 #4: "Will you register? "  [kein Aufruf]
  - 5051 #5: "Registration complete."  [LEBT sub03@0x8E0 Message_on]
  - 5051 #6: "Guest registration is valid / for 24 hours."  [LEBT sub03@0x8E6 Message_on]
  - 5051 #7: "I don't need to use this anymore."  [LEBT sub00@0x7BA AOT1-sce1 wenn NICHT[Ck(3,123)=0]; LEBT sub03@0x8F4 Aot_reset1-sce1]

### Raum 507 — NUR MOD-BAUM
Port 5070 (Leon):
  - 5070 #0: "Left: Shelter 02 / Right: Shelter 01"  [LEBT main00@0xA1E AOT5-sce1]
  - 5070 #1: "Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [LEBT main00@0xA32 Aot_reset1-sce1]
  - 5070 #2: "Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [LEBT main00@0xA3C Aot_reset2-sce1]
Port 5071 (Elza):
  - 5071 #0: "Left: Shelter 02 / Right: Shelter 01"  [LEBT main00@0xA1E AOT5-sce1]
  - 5071 #1: "Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [LEBT main00@0xA32 Aot_reset1-sce1]
  - 5071 #2: "Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [LEBT main00@0xA3C Aot_reset2-sce1]

### Raum 508 — NUR MOD-BAUM
Port 5080 (Leon):
  - 5080 #0: "A high yield generator / that supplies power forthe inside facilities / and the train.Will you turn on the / switch? "  [LEBT sub02@0x7AE Message_on]
  - 5080 #1: "The emergency mode has / been activated.The power supply will be / cut temporarily.The emergency train will / be activated uponrestoration of the power.""  [LEBT sub02@0x7BC Message_on wenn Ck(12,31)=0]
  - 5080 #2: "The door won't open until / the power is restored!"  [LEBT sub02@0x7C2 Aot_reset0-sce1 wenn Ck(12,31)=0]
  - 5080 #3: "The power already has been / supplied to the train!"  [LEBT sub00@0x768 AOT1-sce1 wenn NICHT[Ck(3,48)=0]; LEBT sub04@0x84C Message_on; LEBT sub04@0x85C Aot_reset1-sce1]
Port 5081 (Elza):
  - 5081 #0: "A high yield generator / that supplies power forthe inside facilities / and the train.Will you turn on the / switch? "  [LEBT sub02@0x7A6 Message_on]
  - 5081 #1: "The emergency mode has / been activated.The power supply will be / cut temporarily.The emergency train will / be activated uponrestoration of the power.""  [LEBT sub02@0x7B4 Message_on wenn Ck(12,31)=0]
  - 5081 #2: "The door won't open until / the power is restored!"  [LEBT sub02@0x7BA Aot_reset0-sce1 wenn Ck(12,31)=0]
  - 5081 #3: "The power already has been / supplied to the train!"  [LEBT sub00@0x760 AOT1-sce1 wenn NICHT[Ck(3,48)=0]; LEBT sub04@0x844 Message_on; LEBT sub04@0x854 Aot_reset1-sce1]

### Raum 509 — NUR MOD-BAUM
Port 5090 (Leon):
  - 5090 #0: "THANK YOU FOR PLAYING !!!"  [LEBT sub02@0x12CE Message_on]
  - 5090 #1: "I can't put my friends / in danger.I have no choice but to / stop that creature here!"  [LEBT sub04@0x1334 Aot_reset4-sce1]
Port 5091 (Elza):
  - 5091 #0: "THANK YOU FOR PLAYING !!!"  [LEBT sub02@0x12B2 Message_on]
  - 5091 #1: "I can't put my friends / in danger.I have no choice but to / stop that creature here!"  [LEBT sub04@0x1318 Aot_reset4-sce1]

### Raum 50B — NUR MOD-BAUM
Port 50B0 (Leon):
  - 50B0 #0: "Ada: We're all set. Annette is waiting / for us below, and I left Marvin with her."  [LEBT sub02@0x616 Message_on]
  - 50B0 #1: "Ada: Your other friends are down there / with her too."  [LEBT sub02@0x642 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Ada: Some other survivors are down there with her too."
  - 50B0 #2: "Leon: That's good. Who's Annette?"  [LEBT sub02@0x662 Message_on]
  - 50B0 #3: "Ada: Dr. Birkin's wife and my boss. / She's going to help us with the train."  [LEBT sub02@0x69A Message_on]
  - 50B0 #4: "Leon: Who's Dr. Birkin, and where is he?"  [LEBT sub02@0x6D2 Message_on]
  - 50B0 #5: "Ada: He..."  [LEBT sub02@0x70A Message_on]
  - 50B0 #6: "Ada: He's dead. Umbrella killed him. / That's why Annette is helping us."  [LEBT sub02@0x726 Message_on]
  - 50B0 #7: "Leon: What's with those?"  [LEBT sub02@0x776 Message_on]
  - 50B0 #8: "Ada: They're part of Umbrella's / experiments here."  [LEBT sub02@0x7A8 Message_on]
  - 50B0 #9: "Ada: They were to be shipped out for / use as biological weapons."  [LEBT sub02@0x7C0 Message_on]
  - 50B0 #10: "Leon: Ada, why did you set the reactor / to blow up?"  [LEBT sub02@0x80E Message_on]
  - 50B0 #11: "Ada: So I can destroy this place. / You've seen what's in here."  [LEBT sub02@0x846 Message_on]
  - 50B0 #12: "Ada: You've seen what they can do. / I had to do it, understand? It's..."  [LEBT sub02@0x86A Message_on]
  - 50B0 #13: "Ada: Aaaaaaaagggghhh!"  [LEBT sub03@0x9A0 Message_on]
  - 50B0 #14: "Leon: Ada..."  [LEBT sub04@0xA30 Message_on]
  - 50B0 #15: "Ada: I... I'm sorry, Leon. I'm not... / going... to make it... am I?"  [LEBT sub04@0xA4C Message_on]
  - 50B0 #16: "Ada: I... I so wanted... to escape... / you... will have... to do that... for me..."  [LEBT sub04@0xA54 Message_on]
  - 50B0 #17: "I can't leave Ada alone!"  [LEBT sub02@0x5B4 Aot_reset0-sce1; LEBT sub02@0x5BE Aot_reset1-sce1]
  - 50B0 #18: "She's dead."  [LEBT sub08@0xB36 Message_on]
Port 50B1 (Elza):
  - 50B1 #0: "It's Ada. She's dead.It looks like one of those / gorillas killed her."  [LEBT sub02@0x52E Message_on]
      PORT-OVERRIDE story_r36 (untersuchen): "It's a woman. She's dead.It looks like one of those gorillas killed her."

### Raum 50C — Disc-Datei vorhanden, ohne Text
Port 50C0 (Leon):
  - 50C0 #0: "An elevator. Pressing the / button has no effect.It hasn't been used for a / long time."  [LEBT main00@0x932 AOT1-sce1 wenn Ck(3,78)=0]
Port 50C1 (Elza):
  - 50C1 #0: "An elevator. Pressing the / button has no effect.It hasn't been used for a / long time."  [LEBT main00@0x932 AOT1-sce1 wenn Ck(3,78)=0]

### Raum 50D — NUR MOD-BAUM
Port 50D0 (Leon):
  - 50D0 #0: "There's a button here. Push it?  e         /            4   2  /           /      3  x  x                    "  [LEBT sub02@0x1398 Message_on; LEBT sub04@0x1410 Message_on]
Port 50D1 (Elza):
  - 50D1 #0: "There's a button here. Push it?  e         /            4   2  /           /      3  x  x                    "  [LEBT sub02@0x1398 Message_on; LEBT sub04@0x1410 Message_on]

### Raum 513 — NUR MOD-BAUM
Port 5130 (Leon):
  - 5130 #0: "Left: Shelter 02 / Right: Shelter 01"  [kein Aufruf]
  - 5130 #1: "Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [kein Aufruf]
  - 5130 #2: "Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [kein Aufruf]
Port 5131 (Elza):
  - 5131 #0: "Left: Shelter 02 / Right: Shelter 01"  [kein Aufruf]
  - 5131 #1: "Shelter 01"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [kein Aufruf]
  - 5131 #2: "Shelter 02"The door is tightly locked.It doesn't look like it will open / unless there's an emergency..."  [kein Aufruf]

### Raum 600 — NUR MOD-BAUM
Port 6000 (Leon):
  - 6000 #0: "Annette: You must be that cop. / Your friends are already here."  [LEBT sub06@0x10C2 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Annette: You must be that cop. Your friend is already here."
  - 6000 #1: "Annette: Where's Ada?"  [LEBT sub06@0x10FA Message_on]
  - 6000 #2: "Annette: I see..."  [LEBT sub06@0x113A Message_on]
  - 6000 #3: "Annette: Come on. Let's join your / friends and get out of here."  [LEBT sub06@0x1156 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Annette: Come on. Let's join the others and get out of here."
  - 6000 #4: "Annette: Let's see if we can get / this thing moving."  [LEBT sub02@0xFC4 Message_on]
  - 6000 #5: "A device to open the / tunnel gate.Will you turn on the / switch? "  [LEBT sub07@0x11A6 Message_on]
  - 6000 #6: "It's locked from the other side."  [LEBT main00@0xDA0 AOT2-sce1 wenn Ck(3,51)=0]
  - 6000 #7: "A device to open the / tunnel gate.The power is not supplied."  [LEBT main00@0xE02 AOT5-sce1 wenn Ck(3,49)=0]
Port 6001 (Elza):
  - 6001 #0: "A device to open the / tunnel gate.Will you turn on the / switch? "  [LEBT sub02@0xF18 Message_on]
  - 6001 #1: "It's locked from the other side."  [LEBT main00@0xDA0 AOT2-sce1 wenn Ck(3,51)=0]
  - 6001 #2: "A device to open the / tunnel gate.The power is not supplied."  [LEBT main00@0xE02 AOT4-sce1 wenn Ck(3,49)=0]

### Raum 601 — NUR MOD-BAUM
Port 6010 (Leon):
  - 6010 #0: "You've opened the lock."  [LEBT sub02@0x10A8 Message_on]
Port 6011 (Elza):
  - 6011 #0: "You've opened the lock."  [LEBT sub02@0x10A8 Message_on]

### Raum 602 — NUR MOD-BAUM
Port 6020 (Leon):
  - 6020 #0: "Itembox is not available in / this preview."  [TOT sub02@0xBC6 Message_on]
Port 6021 (Elza):
  - 6021 #0: "Itembox is not available in / this preview."  [TOT sub02@0xBC6 Message_on]

### Raum 603 — Disc-Datei vorhanden, ohne Text
Port 6030 (Leon):
  - 6030 #0: "Leon: Hang on, Marvin. / We're getting out of here."  [LEBT sub02@0xDC2 Message_on]
  - 6030 #1: "Leon: Where's Elza?"  [LEBT sub02@0xE5C Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Leon: Is anyone else with you?"
  - 6030 #2: "Sherry: She's out there fighting / the monsters!"  [LEBT sub02@0xE80 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Sherry: Elza is out there fighting the monsters!"
  - 6030 #3: "John: Yeah! She's trying to keep / the outside clear so we can escape!"  [LEBT sub02@0xE9A Message_on]
  - 6030 #4: "Leon: Why aren't you out there?"  [LEBT sub02@0xEC6 Message_on]
  - 6030 #5: "John: She told me to stay and keep / an eye on Marvin here."  [LEBT sub02@0xEEA Message_on]
  - 6030 #6: "Leon: Where's Annette?"  [LEBT sub02@0xF22 Message_on]
  - 6030 #7: "John: Up front, trying to start / the train."  [LEBT sub02@0xF46 Message_on]
  - 6030 #8: "Leon: I see. Thanks."  [LEBT sub02@0xF7E Message_on]
  - 6030 #9: "Itembox is not available in / this preview."  [LEBT sub04@0xFF8 Message_on]
  - 6030 #10: "I have to get this thing moving / first!"  [LEBT sub06@0x106A Aot_reset3-sce1 wenn Ck(3,89)=0]
  - 6030 #11: "Annette: Where's that biker girl?"  [LEBT sub08@0x1174 Message_on]
  - 6030 #12: "Leon: Keeping the monsters at bay. / Can you start this thing?"  [LEBT sub08@0x1198 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Leon: I've never met her. Can you start this thing?"
  - 6030 #13: "Annette: Certainly."  [LEBT sub08@0x11D0 Message_on]
  - 6030 #14: "Annette: If I remember the startup / sequence correctly..."  [LEBT sub08@0x1200 Message_on]
  - 6030 #15: "Leon: What's wrong?"  [LEBT sub08@0x125E Message_on]
  - 6030 #16: "Annette: Not enough power! / The train's batteries are too low!"  [LEBT sub08@0x1296 Message_on]
  - 6030 #17: "Leon: I'll take care of it!"  [LEBT sub08@0x12C2 Message_on]
  - 6030 #18: "Annette: I'll be ready to start the / train when you do!"  [LEBT sub08@0x12E6 Message_on]
  - 6030 #19: "Leon: Good luck to both of us!"  [LEBT sub08@0x130A Message_on]
  - 6030 #20: "You can't get off the / train right now."  [LEBT sub06@0x1098 Aot_reset2-sce1 wenn Ck(3,89)=1]
Port 6031 (Elza):
  - 6031 #0: "Elza: Where's Leon?"  [LEBT sub02@0xDB2 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Elza: Sherry! Is anyone else here?"
  - 6031 #1: "Sherry: He's fighting off monsters. / He wants you to start the train."  [LEBT sub02@0xDD2 Message_on]
      PORT-OVERRIDE story_r36 (umschreiben): "Sherry: A policeman! He's fighting off monsters. He wants us to start the train."
  - 6031 #2: "Elza: I'll do it!"  [LEBT sub02@0xDDA Message_on]
  - 6031 #3: "Itembox is not available in / this preview."  [LEBT sub04@0xE1E Message_on]
  - 6031 #4: "I have to get this thing moving / first!"  [LEBT sub06@0xE90 Aot_reset3-sce1 wenn Ck(3,89)=0]
  - 6031 #5: "Sherry: I know how to start this! / I watched Mom and Dad do it."  [LEBT sub08@0xF96 Message_on]
  - 6031 #6: "Sherry: Let's see, you push this / here and throw this switch there."  [LEBT sub08@0xFC4 Message_on]
  - 6031 #7: "Elza: What's wrong?"  [LEBT sub08@0x102A Message_on]
  - 6031 #8: "Sherry: It's doesn't have enough power!"  [LEBT sub08@0x106A Message_on]
  - 6031 #9: "Sherry: This says the train's main / batteries need to be fueled!"  [LEBT sub08@0x1082 Message_on]
  - 6031 #10: "Elza: Let me take care of that. / You start the train once it's ready."  [LEBT sub08@0x109E Message_on]
  - 6031 #11: "You can't get off the / train right now."  [LEBT sub06@0xEBE Aot_reset2-sce1 wenn Ck(3,89)=1]

