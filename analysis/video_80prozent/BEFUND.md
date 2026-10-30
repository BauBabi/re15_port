# Befund: 80%-Videos gegen unseren Stand (2026-10-30)

Quellen (Nutzer-Uploads):
- **V1** `Bio_Hazard_Complete_Disc_-_Resident_Evil_1.5_Footage.mp4` (4:45, 960x720): Movie-Menue der
  japanischen *Biohazard Complete Disc* (1998), Eintrag „BIOHAZARD2 PROTOTYPE“, vier Clips NO.1–NO.4.
  Sauberes Bild, Spielfenster aber klein (~525x430) -> Untertitel unscharf.
- **V2** `Biohazard_1.5_Final_Build.mp4` (2:19, 640x480): vom Fernseher abgefilmt (bioflames.com),
  starker Blaustich/Verzerrung, Schnitt im Sekundentakt, Wasserzeichen unten links.

## Methode (reproduzierbar)

1. Alle 1119 Kamera-Hintergruende des Auslieferungsstands mit den Engine-Dekodern ausgegeben
   (`tests/unit/probe_bg_dump.c`, alle 6 Stages).
2. Videos mit 1 bzw. 2 Bildern/s zerlegt; jedes Bild per normierter Korrelation (64x48, Graustufen,
   Spielfenster ausgeschnitten) gegen alle Hintergruende. **Score >= 0.80 = sicherer Treffer**, 0.7–0.8
   nur mit Sichtpruefung, darunter verworfen. V2 ist fuer den Automaten zu verzerrt -> dort nach Sicht +
   Dialog.
3. Alle RDT-Nachrichten dekodiert (`tools/rdt_msgdump.py`) und jeden lesbaren Videotext gegen sie
   gesucht. Gegner je Raum aus den `Sce_em_set` der SCDs (`tools/scd_dump_room.py`).

„Raum xxx0“ = Leon-Variante, „xxx1“ = Elza-Variante (gleicher Hintergrund).

---

## 1. Was uns fehlt / was abweicht

| # | Video | Raum (Cut) | Was das 80%-Material zeigt | Unser Stand (Beleg) | Einstufung |
|---|---|---|---|---|---|
| 1 | V1 4:36–4:40 | **ROOM50A1** Hangar C-2 (Cut 2, Score 0.87) | Birkin (massige Form) packt Elza, hebt sie hoch, wirft sie zu Boden | 50A0/50A1 setzen nur **4x Typ 0x29 (Kakerlake)**, kein 0x30 | **fehlt** – Begegnung + Modell (`EM30_HANGAR.EMD` gehoert hierhin) |
| 2 | V1 4:11–4:19 | **ROOM4080** (Cut 1/2, 0.94–0.97) | Elza gegen massiges, dunkelrotes Affen-Monster (Gorilla-Typ) | 4080/4081: nur **2x 0x29 (Kakerlake)**. Gorilla 0x27 gibt es bei uns nur in 1141 und 11C0 | **fehlt** – Gorilla-Begegnung in 4080 |
| 3 | V1 1:34–1:39 | **ROOM1200** Leichenhalle (Cut 2) | Leon brennt mit dem **Flammenwerfer** einen Zombie auf der Bahre ab | ROOM30E0 Msg 15: „There's a Flamethrower here. This weapon is not available in this preview.“ | **fehlt** – Flammenwerfer benutzbar |
| 4 | V1 4:26–4:35 | ROOM5140 (Cut 4/5/6), ROOM5120 (Cut 6, 0.67 + Sicht) | Brennende Gegner, Bodenfeuer im ganzen Gang | 5120/5140 setzen Zombies (0x18); ob Feuer-Effekte/brennende Zombies laufen, **nicht geprueft** | **zu pruefen** |
| 5 | V2 0:49–0:52 | Stage 3, Computer (nahe ROOM3040/3050, Sicht) | Vollbild-PC-Oberflaeche: „Now Running…“-Liste, Datum „…199X“, **„システムエラー“** (System Error) | ROOM3050 hat nur Textnachrichten zum Computer (Msg 3, 6: „The system has been successfully restored.“) | **wahrscheinlich fehlend** – PC-Bildschirm |
| 6 | V1 3:30–3:38 | ROOM3070 (Cut 8, 0.89–0.90) | Birkin bricht durch das Rolltor | 3070/3071 setzen 0x30 | vorhanden (Ablauf nicht verglichen) |
| 7 | V1 1:40–1:47 | ROOM1171 Dach (Cut 3, 0.94) | Elza gegen Kraehen | 1171: 9x 0x21 (Kraehe) | vorhanden |
| 8 | V2 1:35–1:42 | ROOM11C0 Parkgarage | Umbrella-Laster, rote Gorillas, Ada | 11C0: 4x 0x27 + Ada-Dialog | vorhanden |
| 9 | V2 0:25, 2:07; 2:01–2:09 | – | Debug-Menue („UTILITY MENU / JUMP …“), Einblendung „Door: x-y“ unten rechts | Debug-Menue im Port vorhanden (`debug_menu_common.c`); „Door:“-Einblendung ist Entwickler-Overlay | kein Spielinhalt |
| 10 | V1 2:18 | **nicht sicher zugeordnet** – wahrscheinlich ROOM2060 (Cut 4 aehnlich: rote Pumpe links, Blickwinkel weicht ab) | roter Maschinenraum, riesige Spinne im Vordergrund | 2060: 4x 0x25 (Spinne) | offen |

Alle uebrigen Spielszenen beider Videos haben bei uns Raum **und** Kamera (Liste in §3).
Die Bahnsteig-Form (Nutzer-Screenshot bioflames) kommt in **keinem** der beiden Videos vor.

**Farbe:** Das Violett der Bahnsteig-Form im Screenshot ist Videoqualitaet (Nutzer); die Form nutzt
jetzt wieder die EM30-Textur (`tools/birkin_formen.py`, `tint=None`).

---

## 2. Alle Texte der Videos -> Raum -> Englisch

„Lesung“ = was im Video steht (`[?]` = unleserlich/unsicher). „Englisch“ = meine Uebersetzung der
Videozeile. „Unser Stand“ = vorhandene Nachricht (Raum, Msg-ID) – sie ist der beste Anker, weil sie
denselben Satz traegt.

### 2.1 Menue der Complete Disc (V1, kein Raum)

| Zeit | Japanisch | Englisch |
|---|---|---|
| 0:37 | バイオハザード / 1996年3月22日 発売 | Biohazard – released March 22, 1996 |
| 0:38 | バイオハザード 2 プロトタイプ | Biohazard 2 Prototype |
| 0:40 (NO.1) | 未公開映像 / 警察署にたてこもるレオンとエルザ / 生死をかけた戦いはここから始まった | Unreleased footage / Leon and Elza barricaded inside the police station / Their life-or-death struggle began here. |
| 1:55 (NO.2) | 未公開映像 / 下水道から郊外へ脱出をはかるレオン / はてしなく続く道程をさまよう・・・ | Unreleased footage / Leon tries to escape from the sewers to the outskirts / wandering an endless road… |
| 2:45 (NO.3) | 未公開映像 / 郊外の製薬工場にたどりついたエルザ / そこで一体、何を見たのか!? | Unreleased footage / Elza reaches the pharmaceutical plant on the outskirts / What on earth did she see there!? |
| 3:43 (NO.4) | 未公開映像 / 研究所に踏み込むレオンとエルザ / サバイバルに終わりはあるのだろうか | Unreleased footage / Leon and Elza storm the research facility / Will this fight for survival ever end? |
| durchgehend | 決定 / キャンセル | Confirm / Cancel |

### 2.2 Spieltexte

| Video/Zeit | Raum (Cut) | Lesung | Englisch (Video) | Unser Stand |
|---|---|---|---|---|
| V1 0:49–0:51 | ROOM1170 Dach (Cut 4) | おい 待て[?]！ | Hey, wait! | 1170 Msg 5 „Leon: Hey! Hey wait! Down here!“ |
| V1 0:52–0:55 | ROOM1170 (Cut 4) | …[?]なんてこった！ | …oh, just great! | 1170 Msg 6 „Leon: Oh, that's just freaking great...“ |
| V1 1:26 | ROOM1190 Schiessstand (Cut 6, 0.96) | スイッチがある 押しますか？ ▸Yes No | There's a switch. Push it? Yes / No | 1190 Msg 0 „There's a switch here. Push it?“ |
| V2 1:06.5 | Leon-Szene (Raum nach Sicht nicht sicher) | …うだ！署長にこの[?]を報告しないと | That's right! I have to report this to the chief. | **1170 Msg 16** „Leon: That's right! I have to report this to the chief.“ |
| V2 1:07.5 | ROOM10D0 (Dialog mit Marvin) | …[?]ゾンビに[?]たか / それとも、ゾンビになっているかだよ。 | …either they got eaten by zombies, or they've turned into zombies. | 10D0 Msg 17 „Leon: Has everyone been eaten by zombies? or have they turned into zombies?“ |
| V2 1:42 | ROOM11C0 Parkgarage | [?]会わなかったのか？ | You didn't run into him? | 11C0 Msg 3 „Ada: You didn't run into him?“ |
| V2 2:15.5 | ROOM11B0 (Cut 5, 0.88) | 上のさわぎは何だ？ | What was the commotion upstairs? | 11B0 Msg 1 „Marvin: … What was the commotion up there?“ |
| V2 2:16 | ROOM11B0 (Cut 5) | 地下に逃げるってのはどうだ？ | How about escaping underground? | 11B0 Msg 7 „Leon: There's a manhole here. How about an underground escape route?“ |
| V2 2:18 | ROOM11B0 (Cut 6) | 下に何かいるかも知れないし— / こんな[?]があるだろ？ | There might be something down there— but what choice do we have? | 11B0 Msg 8 „Marvin: We don't really have any other option than to try…“ |
| V1 2:54 / V2 0:39.5 | **ROOM3060/3061** (Cut 5, Sicht: Lueftungsoeffnung) | …エルザ / 私、ヒミツの入り口を知ってるの | Elza… I know a secret entrance. | 3060/3061 Msg 3 „Sherry: Elza... I know a secret way in!“ |
| V1 2:57 | ROOM3060/3061 | ヒミツの入り口？ | A secret entrance? | Msg 4 „Elza: A secret entrance?“ |
| V1 2:59 | ROOM3060/3061 | ここよ この中から入れるの | It's right here – I can get in through here. | Msg 5 „Sherry: It's here, I can get inside through this hole!“ |
| V1 3:04 | ROOM3060/3061 | シェリー！ 待って！ | Sherry! Wait! | Msg 6 „Elza: Sherry, wait!“ |
| V2 0:47.5 | **ROOM3091** (Elza, John, Sherry) | はあ、はあ…間一ぱつってヤツだな！ / [?]ヤツらにくいつかれてた所だぜ | Huff, huff… that was a close one! Those things nearly got their teeth into me! | 3091 Msg 9 „John: That was a close shave! I almost got my ass handed to me!“ |
| V2 1:00 | ROOM3070/3071 (Cut 9, Sicht) | エレベーター[?]… | The elevator… | wahrscheinlich 3071 Msg 15 „…If the control box worked, the elevator might move, but...“ |
| V2 0:52 | Stage-3-Computer (s. §1 Nr. 5) | システムエラー | System Error | keine Entsprechung (nur 3050 Msg 3/6) |
| V2 0:38.5 | Stage 3, Leichen bei Lkw (Raum nicht sicher) | [?]襲われてないはずだ | …shouldn't have been attacked. | **keine Entsprechung gefunden** |
| V2 0:41 | Sherry am Tisch (Raum nicht sicher) | [?]んがある[?] | unleserlich | – |
| V2 1:24 | Leon + Frau im Kittel (Raum nicht sicher) | [マ]ーヴィンの[?] 2 | „Marvin's … 2“ (Rest unleserlich) | – |
| V2 1:02 | Inventar | Equip Arms / Standard Arms / Item list / ITEM / MAP / FILE / EXIT / Fine | (bereits Englisch) | Layout nicht verglichen |

Nicht gelesen: der Titelbildschirm in V2 0:00 (Menuezeilen zu unscharf).

---

## 3. Szenen -> Raum/Kamera (sichere Treffer, V1)

| Zeit | Raum, Cut | Score |
|---|---|---|
| 0:49–1:01 | 1170 c4 / c3 / c2 | 0.85–0.95 |
| 1:04–1:07 | 1180 c1 (Sicht) | 0.80 |
| 1:08–1:12 | 11E1 c3 (Sicht) | 0.70–0.73 |
| 1:13–1:20 | 1070 c6, c2 | 0.82–0.97 |
| 1:21–1:25 | 1040 c1 | 0.76–0.79 |
| 1:26–1:30 | 1190 c6 | 0.96 |
| 1:31–1:33 | 1030 c6 | 0.92 |
| 1:34–1:39 | 1200 c2 (Sicht) | 0.62 + Sicht |
| 1:40–1:47 | 1170 c3 | 0.94 |
| 1:48–1:53 | 10B0 c1/c2/c8 | 0.87–0.96 |
| 2:06–2:15 | 2000/20B0 c2 (Sicht) | 0.58 + Sicht |
| 2:21–2:25 | 2020 c5 (Sicht) | 0.54 + Sicht |
| 2:26–2:31 | 2001 c3 | 0.90–0.94 |
| 2:32–2:39 | 2070 c0–c3 | 0.78–0.99 |
| 2:54–3:05 | 3060 c5 (Sicht + Dialog) | 0.63 + Sicht |
| 3:06–3:10 | 3000 c2 | 0.82–0.89 |
| 3:11–3:20 | 3010 c4/c3/c2/c0 | 0.80–0.98 |
| 3:21–3:26 | 3060 c3 | 0.75–0.90 |
| 3:27–3:38 | 3070 c9/c8 | 0.85–0.90 |
| 3:52–3:57 | 50C0 c3/c2/c4 | 0.81–0.96 |
| 3:58–4:01 | 50B0 c2 (Sicht) | 0.59 + Sicht |
| 4:02–4:10 | 50E0 c8/c5 | 0.82–0.87 |
| 4:11–4:19 | 4080 c1/c2 | 0.82–0.97 |
| 4:20–4:25 | 5040 c1/c2 | 0.96–0.97 |
| 4:26–4:32 | 5140 c4/c5/c6 | 0.75–0.95 |
| 4:33–4:35 | 5120 c6 (Sicht) | 0.67 + Sicht |
| 4:36–4:40 | **50A1 c2** (Hangar C-2) | 0.84–0.87 |

V2 sichere Treffer (Auswahl): 1170 c2 (0:12), 1020 c0 (0:24), 10B0 c8 (0:26), 2001 c3 (0:31), 11B0 c5
(0:33), 3090 c7/c5 (0:43/0:44), 30E0 c9 (0:45), 3010 c8 (0:54), 3070 c0 (1:01), 1190 c4 (1:12),
1091 c0 (1:20), 10B0 c5 (1:26), 11C0 c0 (1:43), 11E0 c3 (1:49), 6010 c1 (1:59), 5030 c0 (2:08),
5010 c0 (2:09), 11B0 c5/c6 (2:16–2:18).
