# Befund: 80%-Videos gegen unseren Stand (2026-10-30, Fassung 2)

Quellen (Nutzer-Uploads):
- **V1** `Bio_Hazard_Complete_Disc_-_Resident_Evil_1.5_Footage.mp4` (4:45, 960x720): Movie-Menue der
  japanischen *Biohazard Complete Disc* (1998), „BIOHAZARD2 PROTOTYPE“, Clips NO.1–NO.4. Farbtreu und
  sauber, Spielfenster aber klein (~525x361, oben/unten beschnitten) -> Untertitel unscharf.
- **V2** `Biohazard_1.5_Final_Build.mp4` (2:19, 640x480): vom Fernseher abgefilmt (bioflames.com),
  Blaustich/Ueberstrahlung/Verzerrung, Schnitt im Sekundentakt, Wasserzeichen unten links. **Farben aus V2
  sind kein Beleg.**

## Methode (reproduzierbar)

1. Alle 1119 Kamera-Hintergruende des Auslieferungsstands mit den Engine-Dekodern ausgegeben
   (`tests/unit/probe_bg_dump.c`, Stage 1–6). Dateiname `ROOM<rrr><cc>` = Raum `rrr` (RDT `rrr0`/`rrr1`),
   Cut `cc`.
2. Videos mit 1 bzw. 2 Bildern/s zerlegt, jedes Bild per normierter Korrelation (64x48, Graustufen,
   Spielfenster ausgeschnitten) gegen alle Hintergruende. Score >= 0.80 = sicher; darunter nur nach
   Sichtvergleich mit **allen** Cuts des Kandidatenraums. Pro Szene Vergleichsstreifen
   „3 Videobilder | unser Hintergrund“ angesehen.
3. Gegner je Raum aus allen `Sce_em_set` der SCDs (`tools/scd_dump_room.py`), Objekte/Effekte aus
   `Obj_model_set`/`Sce_espr_on`.
4. Alle 25 Gegner-/NPC-Modelle aus CDEMD0.EMS und alle 16 Spielerfiguren (PLD) als Ansichtsblatt
   gerendert und gegen die Videofiguren gestellt.
5. Alle RDT-Nachrichten dekodiert (`tools/rdt_msgdump.py`), jeder lesbare Videotext dagegen gesucht.

Typ-Namen nach Modell-Ansicht/Port-Modul: 0x10/0x11/0x16/0x18 Zombie-Familie, 0x1A Gitterhaende,
0x20 Hund, 0x21 Kraehe, 0x25 Spinne, 0x27 Gorilla, **0x29 kleiner Affe** (EM029, `affen3_r38.c`; das alte
Label „Cockroach“ in RE15_ENEMY_AI_AUDIT.md ist ueberholt), **0x2B Spider-Man** (`spiderman_r38.c`; Audit-Label
„Tyrant“ ebenfalls ueberholt), 0x30 Birkin,
0x40 Marvin, 0x42 Ada, 0x45 Irons, 0x47 Elliot, 0x49 John, 0x4B Sherry, 0x4D Annette.

---

## 1. Abweichungen (was fehlt / anders ist)

### 1.1 Gegner und Szenen

| # | Video | Raum, Cut | 80%-Material | Unser Stand (Beleg) | Einstufung |
|---|---|---|---|---|---|
| G1 | V1 4:36–4:40 | **50A0/50A1 Cut 12** (Hangar C-2, Score 0.87) | **Birkin** (massig, Jeans) packt einen **Spider-Man (0x2B)** – menschlicher Rumpf, lange orange-braun gestreifte Beine –, stemmt ihn hoch und schleudert ihn zu Boden; danach kommt Birkin auf die Kamera zu. Kein Affe im Bild (Nutzerbefund, Modellvergleich mit em0x2B) | 50A0/50A1: 4x **0x29 (Affe)**, **kein 0x30, kein 0x2B**. Spider-Man setzen wir nur in 4001, 5040/41, 5070/71, 5110/11, 5130/31 | **abweichend**: Birkin fehlt, Spider-Man statt Affe; Birkin-gegen-Spider-Man-Szene fehlt. Modell: `EM30_HANGAR.EMD` |
| G2 | V1 1:34–1:39 | **1200/1201 Cut 2** (Leichenhalle) | Leon verbrennt mit dem **Flammenwerfer** einen Zombie auf der Bahre | Flammenwerfer liegt in ROOM30E0, Msg 15: „…This weapon is not available in this preview.“ | **fehlt**: Flammenwerfer benutzbar |
| G3 | V1 4:11–4:19 | 4080/4081 Cut 1/2 | Elza gegen ein massiges, **tief dunkelrotes** Affenwesen | 4080/4081: 2x 0x29 (Affe) – Gestalt passt; unser Fell ist grau-/braun (0x29) bzw. orangebraun (0x27). Video 1 ist farbtreu (Elzas Rot, Hintergruende) | **wahrscheinlich andere Textur/CLUT** fuer den 4080-Affen |
| G4 | V1 2:32–2:39, 2:06–2:15 | 2070 Cut 0–3, 2000 Cut 2 | mehrfach **weisse, leuchtende Partikelwolken** am Boden (bei den Spinnen in 2070; im Wasserkanal in 2000) | 2070: 12x 0x25, 2 Objekte, **keine** `Sce_espr_on` | **zu pruefen** (Babyspinnen-Schwarm / Giftwolke?) |
| G5 | V1 4:26–4:35 | 5140 Cut 4/5/6, 5120 Cut 6 | brennende Zombies laufen weiter, Bodenfeuer ueberall | 5140: 4x 0x18 + 13 `Sce_espr_on`; 5120: 2x 0x18 + 27 `Sce_espr_on` | Effekte in den Daten vorhanden – **Laufzeit-Abgleich offen** |
| G6 | V2 0:10/0:14 | 1141 (Halle; Dialog belegt, s. §2) | John warnt Elza, grosses rotes Monster im Raum | 1141: 4x 0x27 (Gorilla) + Msg 5 | vorhanden |
| G7 | V1 3:29–3:35 | 3070/3071 Cut 8 | Birkin bricht durch ein **Gittertor mit Querstrebe** am Lastenaufzug | 3070/3071: 2x 0x30, 3 bzw. 5 `Obj_model_set` (Tor vermutlich Objekt) | vermutlich vorhanden; Tor-Objekt nicht einzeln verglichen |
| G8 | V1 1:40–1:47 | 1170/1171 Cut 3 | Elza gegen Kraehen auf dem Dach | 1171: 9x 0x21 | vorhanden |
| G9 | V2 1:35–1:42 | 11C0 Parkgarage | Umbrella-Laster, rote Gorillas, Ada | 11C0: 4x 0x27, 2x 0x42 + Ada-Dialog | vorhanden |
| G10 | V1 4:02–4:10 | 50E0/50E1 Cut 8/5 | grosse Kreatur bei Leon am Pult (im gruenen Raumlicht gelbbraun, im Hintergrund rosa) | 50E0: 2x 0x30 | vorhanden (Birkin) |

### 1.2 Hintergruende, Kameras, Licht

| # | Video | Raum, Cut | Abweichung | Einstufung |
|---|---|---|---|---|
| H1 | V1 3:58–4:01 | 50B0 Cut 2 | im Video **rotes Leuchten / rosa Lichtflecken am Boden** unter den Maschinen; unser Hintergrund hat es nicht, und 50B0 setzt **keine** Effekte | **fehlt** (Licht-/Effekt-Ebene oder anderer Hintergrund) |
| H2 | V1 2:16–2:20, V2 1:29 | 2060 (rote Pumpe, Spinne im Vordergrund) | derselbe Raum, aber **keiner** unserer 13 Cuts zeigt den Blickwinkel (Pumpe links, Durchgang Mitte, Saeule rechts) | **wahrscheinlich andere/fehlende Kamera** |
| H3 | V1 1:21–1:25 | 1040 Cut 1 | Gangende im Video durch **geschlossenes Rolltor** versperrt, davor Blutlache + Leiche; unser Hintergrund zeigt offenen Durchgang | 1040 setzt 4 `Obj_model_set` -> Tor vermutlich Objekt; nicht einzeln verglichen |
| H4 | V1 1:11–1:12 | 11E0 Cut 13 | Blutlache + Leiche auf dem Gang | Leiche/Blut sind Objekte bzw. Gegner; nicht verglichen |
| – | alle anderen Szenen | – | Hintergrund deckungsgleich (Liste §3) | vorhanden |

### 1.3 Figuren, Texturen, UI

| # | Was | Ergebnis |
|---|---|---|
| F1 | Leon (V1 3:52, Ruecken) | Stern + „R.P.D.“ auf dem Ruecken = unser **PL00** |
| F2 | Elza (V1 4:20) | weisses Brustteil, rote Hose = unser **PL04** |
| F3 | Sherry, John (V1 2:54–3:05) | Schuluniform hellblau / Unterhemd + Hosentraeger = unsere **0x4B / 0x49** |
| F4 | Birkin Stage 3 (V1 3:30–3:38) | = unser **EM30** |
| F5 | Inventar (V2 1:02) | Equip Arms / Standard Arms / Item list / ECG „Fine“ / ITEM-MAP-FILE-EXIT = **identisch** mit unserem Inventar (`shots/inv_original.png`) |
| F6 | Wissenschaftlerin im Kittel (V2 1:55–1:58, Stage 5/6) | passt zu unserer **0x4D (Annette)**; bei V2-Qualitaet nicht sicher |
| F7 | Debug-Menue (V2 0:25, 2:07), Einblendung „Door: x-y“ unten rechts (V2 2:01–2:09) | Entwickler-Overlays, kein Spielinhalt; Debug-Menue im Port vorhanden |
| F8 | Titelbild (V2 0:00) | Logo „BIOHAZARD 2“ mit Menue – Zeilen nicht lesbar, nicht verglichen |

**Bahnsteig-Form:** kommt in keinem der beiden Videos vor. Ihr Violett im Screenshot ist
Videoqualitaet; die Form nutzt die EM30-Textur (`tools/birkin_formen.py`, `tint=None`).

---

## 2. Alle Texte der Videos -> Raum -> Englisch

`[?]` = unleserlich/unsicher. „Unser Stand“ = vorhandene Nachricht mit demselben Inhalt.

### 2.1 Menue der Complete Disc (V1, kein Raum)

| Zeit | Japanisch | Englisch |
|---|---|---|
| 0:37 | バイオハザード / 1996年3月22日 発売 | Biohazard – released March 22, 1996 |
| 0:38 | バイオハザード 2 プロトタイプ | Biohazard 2 Prototype |
| 0:40 (NO.1) | 未公開映像 / 警察署にたてこもるレオンとエルザ / 生死をかけた戦いはここから始まった | Unreleased footage / Leon and Elza barricaded inside the police station / Their life-or-death struggle began here. |
| 1:55 (NO.2) | 未公開映像 / 下水道から郊外へ脱出をはかるレオン / はてしなく続く道程をさまよう・・・ | Unreleased footage / Leon tries to escape through the sewers to the outskirts / wandering an endless road… |
| 2:45 (NO.3) | 未公開映像 / 郊外の製薬工場にたどりついたエルザ / そこで一体、何を見たのか!? | Unreleased footage / Elza reaches the pharmaceutical plant on the outskirts / What on earth did she see there!? |
| 3:43 (NO.4) | 未公開映像 / 研究所に踏み込むレオンとエルザ / サバイバルに終わりはあるのだろうか | Unreleased footage / Leon and Elza storm the research facility / Will this fight for survival ever end? |
| durchgehend | 決定 / キャンセル | Confirm / Cancel |

### 2.2 Spieltexte

| Video/Zeit | Raum (Cut) | Lesung | Englisch (Video) | Unser Stand |
|---|---|---|---|---|
| V1 0:49–0:51 | 1170 (Cut 4) | おい 待て[?]！ | Hey, wait! | 1170 Msg 5 „Leon: Hey! Hey wait! Down here!“ |
| V1 0:52–0:55 | 1170 (Cut 4) | …[?]なんてこった！ | …oh, just great! | 1170 Msg 6 „Leon: Oh, that's just freaking great...“ |
| V1 1:26 | 1190 (Cut 6) | スイッチがある 押しますか？ ▸Yes No | There's a switch. Push it? Yes / No | 1190 Msg 0 „There's a switch here. Push it?“ |
| V2 0:10 | **1141** (Halle) | エルザ！屋上に行っちゃダメだ！ / [ヤ]バイ[怪物]がいやがる | Elza! Don't go up to the roof! / There's a nasty monster up there! | 1141 Msg 5 „John: Don't go to the rooftop! …a somewhat dangerous monster!“ |
| V2 1:06.5 | Leon-Szene (Raum nach Sicht unsicher) | …うだ！署長にこの[?]を報告しないと | That's right! I have to report this to the chief. | 1170 Msg 16 „Leon: That's right! I have to report this to the chief.“ |
| V2 1:07.5 | 10D0 (Leon + Marvin) | …[?]ゾンビに[?]たか / それとも、ゾンビになっているかだよ。 | …either they got eaten by zombies, or they've turned into zombies. | 10D0 Msg 17 |
| V2 1:42 | 11C0 Parkgarage | [?]会わなかったのか？ | You didn't run into him? | 11C0 Msg 3 „Ada: You didn't run into him?“ |
| V2 2:15.5 | 11B0 (Cut 5) | 上のさわぎは何だ？ | What was the commotion upstairs? | 11B0 Msg 1 (Marvin) |
| V2 2:16 | 11B0 (Cut 5) | 地下に逃げるってのはどうだ？ | How about escaping underground? | 11B0 Msg 7 (Leon) |
| V2 2:18 | 11B0 (Cut 6) | 下に何かいるかも知れないし— / こんな[?]があるだろ？ | There might be something down there— but what choice do we have? | 11B0 Msg 8 (Marvin) |
| V1 2:54 / V2 0:39.5 | 3060/3061 (Cut 5) | …エルザ / 私、ヒミツの入り口を知ってるの | Elza… I know a secret entrance. | Msg 3 „Sherry: Elza... I know a secret way in!“ |
| V1 2:57 | 3060/3061 (Cut 5) | ヒミツの入り口？ | A secret entrance? | Msg 4 (Elza) |
| V1 2:59 | 3060/3061 (Cut 5) | ここよ この中から入れるの | It's right here – I can get in through here. | Msg 5 (Sherry) |
| V1 3:04 | 3060/3061 (Cut 5) | シェリー！ 待って！ | Sherry! Wait! | Msg 6 (Elza) |
| V2 0:47.5 | 3091 (Elza, John, Sherry) | はあ、はあ…間一ぱつってヤツだな！ / [?]ヤツらにくいつかれてた所だぜ | Huff, huff… that was a close one! Those things nearly got their teeth into me! | 3091 Msg 9 (John) |
| V2 1:00 | 3070/3071 (Cut 9) | エレベーター[?]… | The elevator… | wahrscheinlich 3071 Msg 15 |
| V2 0:52 | Stage-3-Computer | システムエラー (+ „Now Running…“, Datum „…199X“) | System Error | **keine Entsprechung** (3050 nur Msg 3/6) – Vollbild-PC-Screen **wahrscheinlich fehlend** |
| V2 0:38.5 | Stage 3, Leichen beim Lkw (Raum unsicher) | [?]襲われてないはずだ | …shouldn't have been attacked. | **keine Entsprechung gefunden** |
| V2 0:41 | Sherry am Tisch (Raum unsicher) | [?]んがある[?] | unleserlich | – |
| V2 1:24 | Leon + Ada + Marvin an der Leiter (Raum unsicher) | [マ]ーヴィンの[?] 2 | „Marvin's … 2“ (Rest unleserlich) | – |
| V2 1:02 | Inventar | Equip Arms / Standard Arms / Item list / ITEM / MAP / FILE / EXIT / Fine | (bereits Englisch) | identisch |

---

## 3. Szenen -> Raum/Kamera (V1, korrigierte Cuts)

| Zeit | Raum, Cut | Score / Beleg |
|---|---|---|
| 0:49–1:00 | 1170 Cut 4 / 3 / 2 | 0.86–0.95 |
| 1:04–1:07 | 1180 Cut 1/6 (dunkelgruener Gang) | Sicht (1100 verworfen) |
| 1:08–1:12 | 11E0 Cut 13 | 0.73 + Sicht |
| 1:13–1:20 | 1070 Cut 6, Cut 2 | 0.84–0.97 |
| 1:21–1:25 | 1040 Cut 1 | 0.79 + Sicht |
| 1:26–1:30 | 1190 Cut 6 | 0.96 |
| 1:31–1:33 | 1030 Cut 6 | 0.92 |
| 1:34–1:39 | 1200 Cut 2 | 0.75 + Sicht |
| 1:40–1:47 | 1170 Cut 3 | 0.95 |
| 1:48–1:53 | 10B0 Cut 1 / 2 / 8 | 0.87–0.96 |
| 2:06–2:15 | 2000 Cut 2 (= 20B0 Cut 2) | Sicht |
| 2:16–2:20 | 2060, Cut nicht zuordenbar (H2) | Sicht |
| 2:21–2:25 | 2020 Cut 5/6 | Sicht |
| 2:26–2:31 | 2000 Cut 13 | 0.94 |
| 2:32–2:39 | 2070 Cut 0–3 | 0.87–0.99 |
| 2:54–3:05 | 3060 Cut 5 | Sicht + Dialog |
| 3:06–3:10 | 3000 Cut 2 | 0.89 |
| 3:12–3:20 | 3010 Cut 4 / 3 / 2 / 0 | 0.80–0.98 |
| 3:21–3:26 | 3060 Cut 3 | 0.90 |
| 3:27–3:35 | 3070 Cut 9 / 8 | 0.85–0.90 |
| 3:52–3:57 | 50C0 Cut 3 / 2 / 4 | 0.81–0.96 |
| 3:58–4:01 | 50B0 Cut 2 | Sicht |
| 4:02–4:10 | 50E0 Cut 8 / 5 | 0.85–0.87 |
| 4:11–4:19 | 4080 Cut 1 / 2 | 0.95–0.97 |
| 4:20–4:25 | 5040 Cut 1 / 2 | 0.96–0.97 |
| 4:26–4:32 | 5140 Cut 4 / 5 / 6 | 0.80–0.95 |
| 4:33–4:35 | 5120 Cut 6 | 0.74 + Sicht |
| 4:36–4:40 | **50A0/50A1 Cut 12** (Hangar C-2) | 0.87 |

V2 (Auswahl sicherer Treffer, Cut korrekt zerlegt): 1170 Cut 2 (0:12), 1020 Cut 0 (0:24), 10B0 Cut 8
(0:26), 11B0 Cut 5 (0:33), 1190 Cut 4 (1:12), 1090 Cut 10 (1:20), 10B0 Cut 5 (1:26), 11C0 Cut 0 (1:43),
6010 Cut 1 (1:59), 11B0 Cut 5/6 (2:16–2:18). Der Rest von V2 ist zu verzerrt fuer sichere Zuordnung.

## 4. Naechste Schritte (Vorschlag)

1. G1: Hangar 50A0/50A1: Birkin (0x30, Modell `EM30_HANGAR.EMD`) + Spider-Man (0x2B) statt der Affen, Szene
   Birkin-gegen-Spider-Man.
2. G2: Flammenwerfer freischalten (ROOM30E0 Msg 15 ersetzt durch Aufnahme; Waffe 0x0E im Inventar/
   Schaden/ESP).
3. H1/G3/G4: rotes Bodenlicht 50B0, roter Affe 4080, weisse Partikel 2070/2000 – je am Laufzeit-Port
   gegenpruefen (re15-port-visual-verify).
4. H2: zusaetzliche Kamera in 2060 – nur mit neuem Hintergrund moeglich (nicht auf der Disc).
5. PC-Screen Stage 3 („System Error“) – Vollbild-UI fehlt.

---

## 5. Video 3 `Resident_Evil_1.5_All_Footage.mp4` (14:19, 320x240, bioflames/survivhor)

Sammelschnitt mehrerer Quellen (u.a. die Complete-Disc-Clips aus V1 und der Final-Build aus V2), dazu
Szenen, die in V1/V2 nicht vorkommen, und am Ende (9:34-10:06) Tafeln des BIOHAZARD-2-Trailers.
Zuordnung wie §1 (Score >= 0.80, 120 sichere Segmente), Schluesselstellen in 640x480 angesehen.

### 5.1 Neu gegenueber V1/V2
| Zeit | Raum, Cut | Inhalt | Unser Stand |
|---|---|---|---|
| 0:10-0:18 | Spielerwahl | „PLEASE SELECT MAIN CAST“, Leon/Elza mit PROFILE (178 cm/21/A, 169 cm/19/0) | vorhanden (Player-Select) |
| 2:14-2:32 | 1030 Cut 6/11/5/1/9 | Elza im zerstoerten Wachraum, Zombies durch die Gitter | Raum vorhanden |
| 2:40-2:48 | 1040 Cut 4/5 | **Nahschuss: Zombiekopf platzt** (rote Wolke, kopfloser Koerper faellt) | s. 5.2 |
| 2:50-2:57 | 1150 Cut 1/2/3/5 | Elza in Irons' Buero | vorhanden |
| 2:58-3:05 | 1190 Cut 1/3 | Elza gegen Hunde im Schiessstand | vorhanden (1190: 6x 0x20) |
| 3:32-3:45 | 3000 Cut 2, 3010 Cut 4/3/2/0 | Elza am Lkw, Blut ueberall | vorhanden |
| 4:06-5:12 | 2000/2070 | Leon gegen Zombie-Gruppen im Kanal (Zombies in Braun = Arbeitskleidung) | 2000/2070 setzen 0x16 bzw. 0x25 |
| 5:42-6:00 | ?110/11B (gruenes Moos) | Elza in moosbewachsenem Gang mit Spinnen | **nicht sicher zugeordnet** (Score 0.7) |
| 6:02-6:57 | 1040 Cut 6/2/1/4 | Leon; „ボタンがある 押しますか？ ▸Yes No“ | 1040 Msg 0 „It's a shutter switch. Will you push it?“ |
| 7:00 | 11E0 Cut 3 | Einblendung „画面は開発中のものです。“ = **„Screens are from a game in development.“** | Disclaimer, kein Spieltext |
| 7:44, 10:38-10:42, 12:52-13:02 | 5140 Cut 4-7 | brennende Zombies, Bodenfeuer | 5140: 13 Sce_espr_on (Laufzeit offen, s. G5) |
| 8:06-8:42 | 1070 Cut 3/7 | **Elza mit Granaten: Zombies zerplatzen, Ober- und Unterkoerper getrennt, grosse Blutlachen** | s. 5.2 |
| 8:46-9:05 | 1190 Cut 4 | Leon erschiesst Hunde | vorhanden |
| 9:12-9:31 | 5120 Cut 2-6 (rot) | Leon und Zombie im Feuer | vorhanden (Licht/Effekte offen) |
| 11:30-12:20 | 1070 Cut 5/4/3/7 | Leon und Elza (getrennt!) gegen Zombies im Buero | vorhanden |
| 13:10-13:56 | 3070 Cut 1-5/9/12 | **Leon (mit Marvin) gegen den massigen Birkin** im Lastenaufzug-Raum | 3070 = Leon-Seite: 2x 0x30 + Marvin 0x40 + Ada 0x42 - passt |
| 14:00-14:14 | ? (blau, rote Lampe) | Leon von einem Zombiepulk gefressen, Todesszene | **nicht zugeordnet** |

### 5.2 Schadensmodell
Video 3 zeigt eindeutig mehr Zerstoerung als der Auslieferungsstand:
- **Granate (8:10-8:12, 8:22):** Explosion, Zombie wird in Ober- und Unterkoerper getrennt, der Oberkoerper
  kriecht weiter; danach grosse Blutlachen am Boden.
- **Nahschuss (2:44-2:46, 11:36):** der Kopf platzt in einer roten Wolke, der Koerper faellt kopflos.
- Port: der RE2-Flavor kennt Abtrennung/Brocken (enemy_ai_re2_zombie.c, Gore-Bank, schuss_r36.c); ob der
  RE1.5-Flavor beides zeigt, ist **nicht geprueft**. Offen als Aufgabe, nicht umgesetzt.

### 5.3 Treffen Leon und Elza? (Frage des Nutzers zur Wiki-Aussage)
- **In keinem der drei Videos stehen Leon und Elza im selben Bild.** Die blau uniformierten Figuren neben Elza
  (Leichenhalle 3:14-3:16, Buero 0:44-0:47, 7:16, 10:16) sind RPD-Zombies, nicht Leon.
- **Die Begleiter kreuzen sich aber** - belegt in den ausgelieferten RDTs, nicht nur im Video:
  - ROOM30C1 (Elza-Seite) Msg 1-8: Elza trifft **Marvin** („I'm Marvin Branagh, RPD. I'm here with Leon
    Kennedy.“ - „He's all right and he's helping me and Ada escape the city.“ - Elza: „Will you tell him that
    me and John and Sherry are here and we're all right?“). V2 0:46 zeigt genau diese Szene (Theke ROOM30C).
  - ROOM6030 (Leon-Seite, Zug): Leon trifft **John, Sherry und Annette**; John: „She's out there fighting
    the monsters ... She told me to stay and keep an eye on Marvin here.“ / „Up front, trying to start the train.“
  - Spawns: die Leon-Raeume (xxx0) setzen nie John/Sherry (0x49/0x4B) ausser 6030, die Elza-Raeume (xxx1) nie
    Marvin/Ada (0x40/0x42) als Gegner-Spawn.
- **Urteil:** Die Wiki-Aussage stimmt fuer die **Spielfiguren**: Leon und Elza begegnen sich nicht direkt,
  jede Figur hat zwei eigene Begleiter (Leon: Marvin + Ada, Elza: John + Sherry). Sie ist aber zu absolut,
  was die Geschichte angeht: die Wege beruehren sich ueber die Begleiter (Elza-Marvin in 30C1) und laufen am
  Zug (6030) zusammen. Dass „Elza Marvin trifft“, ist also richtig - Elza trifft Marvin, nicht Leon.
- Nebenbefund: ROOM3060.RDT (Leon-Datei) traegt die Elza/Sherry/John-Dialoge ohne Sprecher-Steuerbytes
  (`05:xx` fehlt) - eine Kopie der Elza-Variante 3061, kein Hinweis auf ein Treffen.

### 5.4 Texte (neu)
| Zeit | Original | Englisch | Bezug |
|---|---|---|---|
| 0:10 | PLEASE SELECT MAIN CAST / Leon,S,Kennedy / Elza Walker / PROFILE | (Englisch) | Player-Select |
| 0:20 | (Dach, Untertitel unscharf) | wie V1 0:49 | 1170 Msg 5/6 |
| 0:54 | スイッチがある 押しますか？ | There's a switch. Push it? | 1190 Msg 0 |
| 3:20-3:30 | …エルザ 私、ヒミツの入り口を知ってるの / ヒミツの入り口？ / ここよ この中から入れるの / シェリー！待って！ | wie V1 2:54-3:04 | 3060/3061 Msg 3-6 |
| 6:48 | ボタンがある 押しますか？ ▸Yes No | There's a button. Push it? Yes / No | 1040 Msg 0 (bei uns „shutter switch“) |
| 7:00 | 画面は開発中のものです。 | Screens are from a game in development. | Disclaimer |
| 9:34-10:06 | あの惨劇から 2か月後 / ラクーンシティ全土に / 真の悪夢が始まった / 恐怖再び | Two months after that tragedy / across all of Raccoon City / the true nightmare began / The horror returns | BIOHAZARD-2-Trailer, kein Spieltext |
