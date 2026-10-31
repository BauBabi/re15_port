# Prüfdossier „daten“: Was auf der 40%-Disc noch nicht ausgewertet war

Stand 2026-10-31. Geprüft wurde `analysis/story_begegnungen/BERICHT.md` und `TEXTE_40.md` gegen die Rohdaten in `Version_40%/`. Alle Zahlen sind selbst nachgerechnet. Adressen sind RAM-Adressen der Disc-EXE (`t_addr 0x80010000`, Datei-Offset = Adresse − 0x80010000 + 0x800) oder der Overlays (geladen @`0x80100000`, ohne Kopf). Skripte liegen unter `scratchpad/s40/pruefung/d_*.py`.

## 0. Ergebnis in Kürze

**Die Antwort des Berichts bleibt stehen.** Nichts außerhalb der RDTs nennt ein Treffen von Leon und Elza. Die Disc enthält keinen weiteren Text, keine gelöschten Reste und keine A/B-Mechanik.

Die Prüfung ergibt aber neue harte Fakten und ein paar Korrekturen:

1. `ROOM115U.SCD` und `ROOM506U.SCD` sind Klick-Rätsel ohne Text und ohne Figuren. Sie können also nicht die Quelle von `・・・レオン` sein.
2. Die EXE trägt eine Raumnamen-Tabelle. Danach heißt 604 „Antriebsraum“, und 605–607 waren als weitere Räume dieser Art geplant. Die Disc zeigt also nur einen Teil des Endes.
3. Leon startet ein neues Spiel fest in 117, Elza fest in 103. `SELECTH.TIM` zeigt genau diese zwei Orte.
4. Es gibt eine Itemtabelle mit 72 Einträgen, darunter T- und G-Impfstoff, Enzym, Zeitbombe, Taschenuhr.
5. Typ 0x4D ist **nicht** „Elzas Kunst auf Adas Rig“: Das Skelett ist Elzas (15/15 Offsets), nur die Clips sind Adas. STAGE5.BIN registriert 0x4D ebenfalls, obwohl kein Stage-5-Raum sie nutzt.
6. Ada trägt selbst einen weißen Kittel, auf der Disc und im Video. „Frau im Kittel“ trennt also nicht. Die zweite Frau unterscheidet sich durch dunkle Hose und dunkle Front.
7. Die EXE kennt Sherry als Spielfigur fest im Code (0x0F). Für Marvin und John (0x0D, 0x0E) gibt es dort keine Sonderlogik.
8. Bit 31 setzt nur der Titel. Ein Szenariowechsel im Lauf oder eine Spielstand-Übernahme ist im Code nicht vorgesehen. „Vorläufer des Zapping“ gilt also nur erzählerisch, nicht technisch.

## 1. ROOM115U.SCD und ROOM506U.SCD

### Wer lädt sie

| Schritt | Beleg |
|---|---|
| Dateitabelle: Index 0x1A = `ROOM115U.SCD` (396 B, LBA 713), 0x1B = `ROOM506U.SCD` (390 B, LBA 714) | `0x8006f50c`, `0x8006f514`, Tabelle `0x8006f43c` (Größe, LBA) |
| Pfad-Strings `d:/bio2/psx/data/room115u.scd`, `…room506u.scd` | `0x8001122c`, `0x8001124c`. Sie werden in `0x8004c55c`–`0x8004c668` nur auf den Stack kopiert und dort nie benutzt. Geladen wird über den Index |
| Stage entscheidet: Stage 0 lädt 0x1A, Stage 4 lädt 0x1B, jeweils nach `0x80198000` | `0x8004c6cc`–`0x8004c6fc`, `jal 0x80013b60` |
| Skript wird als eigene Eventklasse gestartet | `0x800b3f7a=2`, `0x800b3f70=0x80198000` (`0x8004c714`–`c720`), Event 10 Routine 0 (`0x8004c734`), Routine 1 je Bild (`0x8004cd18`) |
| Einstieg: AOT-Handler sce 6 setzt Modus 1 | `0x80043194`, Tabelle `0x8007469c[6]` = `0x800746b4`. Setzt `0x800b25c0=1`, `DAT_800aca3c |= 0x8000`, `0x800b25cc = payload[0]` |
| Modus-Dispatch | `0x80074be8[1]=0x8004c54c`, `0x80074bf4[1]=0x8004cca4` |
| Kamera | `jal 0x800142f4` mit `0x800b25cc` (`0x8004c688`) |

Die einzige Stelle, die `0x800b25c0` ungleich 0 setzt, ist `0x800431a8`.

### Was sie tun

Modus 1 ist ein **Klick-Bildschirm**: Ein Cursor (`0x800b25f8/fa`, Grenzen @`0x8004d08c`–`d0dc`) fährt über rechteckige Hotspots. Die Hotspots legt Opcode 0x4C an (`0x80040858`, 18 Byte), 0x4D ändert sie (`0x800408a8`). Der Treffertest ist `0x8004d5f0`, die Typtabelle `0x80074c68`:
- Typ 3 (`0x8004d728`): startet Routine C.
- Typ 4 (`0x8004d768`): startet Routine C nur, wenn das gewählte Inventar-Item die ID E hat.

Der Name `Sce_espr_kill` für 0x4C in `scd_dump_room.py` passt also nicht.

**ROOM115U:** ein Hotspot (x160, y155, 80×30). Routine 3: Objekte 1 und 2 fahren 15×±10 auseinander, danach wird der Hotspot zu Typ 4 mit **Item 63 (かいちゅうどけい, Taschenuhr)**. Routine 4: Objekt 0 fährt 86×−10, dann 10×+1. Routine 5: alles zurück. Flags (13,0), (13,1), (2,10). Vollständiger Dump: `scd115.txt`.

Das RDT passt dazu:
- `ROOM1150 @0xD1A`: `Aot_set` Slot 1, sce=0 (inert), Rechteck (−21800, −20000, 1500, 3300), Payload `04 00 …`. Byte 0 wäre bei sce 6 die Kamera.
- Kamera 4 von Raum 115 (`bg40/ROOM11504.ppm`) zeigt das **Stadtmodell mit offener Grube**.
- `sub04 @0xEE6` enthält dieselbe Sequenz am Stück. Kein `Evt_exec` ruft sub04 oder sub05 auf (nur sub02, sub03).

Das Stadtmodell im Chefbüro sollte also einen versteckten Mechanismus mit Taschenuhr haben. Die README der Mod nennt das „hidden feature in room 115“.

**ROOM506U:** vier Hotspots, Objekt 6 dreht ∓15 × 70, Objekt 7 wird ein- und ausgeblendet, dazu Effekte. Item-Hotspot: **E=44 (0x2C)**. Das ist in der Disc-Tabelle `調合した薬GGB`, also eine Kräutermischung, was hier sinnlos ist. Die Mod-Fassung (630 B) prüft 60 (Enzym), ihre README nennt „vaccine machine in room 506“. Kamera 11 (`bg40/ROOM50611.ppm`) zeigt die Maschine. Das passt zu 401 #14 („setz ihn ins Gerät, starte das Programm“). Die Datei stammt vom 26.9.1996, die Tabelle ist neuer; wahrscheinlich ist die ID veraltet, belegt ist das nicht.

### Figuren und Text

Beide Dateien enthalten **kein** `Message_on` (0x2B), **kein** `Sce_em_set` (0x44), **kein** `Plc_*`. Es kommt also weder Text noch eine Figur vor. Die Zeile `・・・レオン` kann nicht aus ROOM115U stammen.

### Erreichbarkeit

Kein RDT der Disc installiert sce 6 (`Aot_set`, `Door_aot_set`, `Item_aot_set`, `Aot_reset`). Kein RDT benutzt 0x4C oder 0x4D. Zensus der Disc: sce 0:27, 1:56, 2:207, 3:4, 4:20, 5:90, 7:4, 8:16, 9:25, 12:6, 13:6. Auch im Mod-Baum ist sce 6 = 0. Beide Dateien sind im Stand tot. In Raum 506 gibt es nicht einmal ein inertes AOT dafür, 5060 und 5061 sind byte-gleich.

`ROOM115U.SCD` ist identisch mit der Mod-Datei, `ROOM506U.SCD` nicht. Was das „U“ im Namen bedeutet, ist unklar.

Die Projektnotiz `reai-v2-aot-sce-dispatch` nennt sce 6 „EVENT-SCHEDULE“. Das trifft nicht zu, es ist dieser Klick-Modus.

## 2. Texte außerhalb der RDTs

### Adressen

Der Auftrag nennt Stringtabelle `0x800c4a28` und Index `0x800c495c`. Auf der Disc gibt es darauf **keine Referenz**, die Adressen liegen hinter dem Image. Sie gelten nur für die Mod-EXE (`0x80028848 ff.` dort `lui 0x800c`/`0x495c`). **Die Disc benutzt Index `0x80073a88` und Basis `0x80073820`**, Lesefunktion `0x80028840`, Code `06 xx` @`0x80028aa0`.

Kein RDT-Text benutzt Code 06 (0 Treffer in `texte40.txt`). Nur die EXE-Sätze wie „… を使った“ tun das.

Die RDT-Zeigerprüfung ergibt: `0x3C` ist der einzige Textblock, `0x38` ist laut `RE15_KNOWLEDGE.md` `blockStart` (Zahlenblock, keine Strings; 66 Räume gemessen).

### Itemnamen (72 Einträge, Ende 0x07)

ID = Tabellenindex, bestätigt durch die Icons in `ITEMALL.PIX` (72 × 40×30) und durch den Handler `0x8004d768`, der die Inventar-ID direkt vergleicht.

| ID | Name | ID | Name |
|---|---|---|---|
| 01 | コンバットナイフ | 25–27 | Granaten: 通点弾, 硫酸弾, 火炎弾 |
| 02 | パイプ | 28, 29 | 信(管), グレネード空薬(莢) |
| 03 | ブローニングHP (Leon Start) | 30–32 | Nitro-, Säure-, Zündkapsel |
| 04 | SIG・P228 (Elza Start) | 33 | メモリーカード |
| 05–08 | ベレッタM93R, グロッグ18, Sーレッドホーク, レミントンM870 | 34–38 | 救急スプレー, 索毒スプレー, Grün/Rot/Blau |
| 09–11 | Handgranaten: normal, Säure, Feuer | 39–46 | 調合した薬 GR…GRB |
| 12–14 | イングラムM10, スパス12, 火炎放射器 | 47, 48 | ナット, プライヤー |
| 15–17 | グレネードガン (dreimal) | 49, 50 | **未定** (noch offen) |
| 18, 19 | ロケットランチャー, H&K NC51 | 51, 52, 68 | MDメディア, MDプレイヤー, MDプレイヤーメディア入り |
| 20 | しょうかき | 53 | **じげんばくだん** (Zeitbombe) |
| 21–24 | 9mm, Schrot, Magnum, Treibstoff | 54–57 | IDカード1–4 |
| 58, 59 | **Tウイルス, Gウイルス** | 63 | **かいちゅうどけい** (Taschenuhr) |
| 60 | **こうそ** (Enzym) | 64, 65 | ヒューズ, リペアープラグ |
| 61, 62 | **Tワクチン, Gワクチン** | 66, 67 | キーディスク, つうしんききどうカード |
| 69 | 水キー | 70, 71 | **IDカードーM, IDカードーL** |

Für die Story relevant:
- Es gibt getrennt T- und G-Impfstoff sowie Enzym. Welcher Impfstoff in welchem Szenario gedacht war, steht nirgends.
- **Enzym liegt nur in Elzas Datei:** `ROOM40A1 sub00 @0xED0` `Item_aot_set` Slot 10, ID 0x3C. ROOM40A0 hat diesen Slot nicht.
- M und L sind vermutlich Kartenklassen und keine Initialen von Marvin oder Leon: ITPS-Bild 54 zeigt eine Umbrella-Karte „TYPE M“ eines Mannes „GORO MACIDA“. Das ist Sichtbefund und eine Deutung.
- Auf dem Boden liegen im Stand nur die IDs 21, 22, 23, 34, 38, 48, 60.
- Es gibt keine Datei-Items. Die Tabelle endet bei 71; die Mod hat dahinter „Chris' Diary“ usw.

### Systemtexte (`0x80073490`–`0x80073820`)

- Speicherkarten-Meldungen. `0x800735c2`: `Leon/00/` (Index 24), `Elza/00/` (Index 25), `バイオ2のデータ`.
- `0x800736fc`: このままではアイテムを入手できない. `0x80073711 ff.`: を使った, を置きますか, を使いますか, をファイルにとじました. もうこのカギは必要ないようだ, シャッターのスイッチがある, いまは使えない.
- `0x800737aa ff.`: ゛003゜と刻まれている, 机を開けるカギのようだ, 変わったマークが刻まれている, 特別なコーティングがされている かなり重要なカギのようだ, どこかにクローゼットがあるはずだ.
- `0x80073628 ff.` (Shift-JIS): 「館 小部屋 1F」「館 ホール 1F」「中庭 部屋 B1」「寄宿舎 1F」「研究所 B3」「館 物置 1F」「中庭 通路 B1」. Das sind Orte aus dem ersten Teil. Die Schlüsseltexte davor sind vermutlich ebenfalls solche Reste. Der Lesestub `0x80026e4c` liefert immer 0.
- `0x800107cc`, `0x800107f8` (Shift-JIS): `BIO HAZARD 「Elza /00/`, `BIO HAZARD 「Leon /00/`.
- `0x80073b18 ff.`: Tastenbelegung und STEREO/MONO.

### Namenssuche über alle Dateien

Font-kodierte Suche in EXE, Overlays, allen RDTs, SCDs, DOOR, ITP. `アネット`, `バーキン`, `アイアンズ`, `リンダ`, `ブライアン`, `ウィリアム`: **0 Treffer**. レオン, エルザ, エイダ, マーヴィン, ジョン, シェリー kommen nur in RDT-Textblöcken vor. Kein Treffer in 1150/1151. Das stützt Bericht §11.

Overlays und TITLE.BIN enthalten keinen Font-Text. TITLE.BIN hat nur ASCII: `NEW GAME`, `LOAD DATA`, `CONFIG`, `LEON` (`0x801000a0`), `ELZA` (`0x801000a8`), Debug-Strings.

### Raumnamen-Tabelle (`0x8006ff36`, je 26 Byte: x, z, Kamera, Name)

Das ist die wichtigste neue Datenquelle für den Plan. Schrittweite 637 Halbworte je Stage, 13 je Raum (`0x8001d39c`–`0x8001d3d8`).

| Raum | Name | Raum | Name |
|---|---|---|---|
| 102, 107 | OFFICE B, OFFICE A | 300–302, 306 | SHIKICHI A–D (Gelände) |
| 103, 104 | LOBBY, LOBBY ROUKA | 303, 304 | WATARI ROUKA, KOUJYOU LINE A |
| 106, 10A | KAIDAN 1, KAIDAN 2 | 307, 308 | SOUKO, SOUKO ELEVATOR |
| 109, 10B | SOTO B, SOTO A | 309, 30E | ROUKA, FORKURIFUTO |
| 110 | SIRYOUSITU MAE | 400, 401 | LABO ENTRANCE, GUARDHOUSE |
| 115 | SHOCHO ROOM | 403, 404 | MEINSYAFUTO, MIGITSUURO |
| 116, 117 | HELI PORT MAE, OKUJYOU | 409, 40A, 40B | JIKKENSHITSU, KENSASHITSU, BAIYOUSHITSU |
| 118, 11E, 127 | B1 ROUKA, B2 ROUKA, L B1 ROUKA | 501, 503 | GUARDHOUSE, KANSEISHITSU |
| 119, 11A | SHOOTING RANGE, BUKI SOUKO | 504, 505, 506 | CORRIDOR 2-A, DENSANSHITSU, P4 LABORATORY |
| 11B, 11C | CHIKA CYUU B, A | 50A, 50C, 510 | 2-C, 3-A, 3-B TSUURO |
| 11F | BOIRA SHITSU | 50D | **TEST** |
| 120, 121 | SHITAI OKIBA, RYUUCHI SYO | 511, 512, 514 | F KANSEISHITSU, F CORRIDOR 2-A, F 2-C TSUURO |
| 122–126 | DOKUBOU 1–5 | 603 | UNTENSHITSU (Führerstand) |
| 200, 201, 203 | L TSUURO, ZONBIE TSUURO, L TSUURO 2 | **604–607** | **DOURYOKUSHITSU (Antriebsraum), viermal** |
| 207–20A | KUMO TSUURO, WANI BOSS ENT, WANI BOSS BEYA (2×) | | |

Name ohne Datei auf der Disc: 10A, 127, 20A, 40B, 505, 605, 606, 607. Datei ohne Name: nur BSS 408. Die Einträge 600–602 sind leer.

## 3. Bildtexte

| Datei | Inhalt |
|---|---|
| `SELECT.TIM`, `AAA.TIM` | `Leon,S,Kennedy` / `From police department`; `Elza Walker` / `From women's college` (`…dor`). PLEASE SELECT MAIN CAST |
| `SELECTH2/3.TIM` | Dasselbe mit `from college dormitory`. Profile: Leon 178 cm, 21 Jahre, 70.2 kg, Blutgruppe A; Elza 169 cm, 19 Jahre, 52.4 kg, Blutgruppe 0 |
| `SELECTH.TIM` | PLEASE SELECT MAIN CAST, links nächtlicher Hubschrauberlandeplatz, rechts Halle mit Gitter und Scherben (Sichtbefund: entspricht 117 und 103 Kamera 0) |
| `TITLEJ.TIM`, `TITLEU.TIM` | `BIOHAZARD 2`, `RESIDENT EVIL 2` |
| `YOUDIED.TIM` | `YOU DIED` |
| `ST_00.TIM` | Waffen- und Spraybilder, Messer, Karte `POLICE / Leon.S.Kennedy` mit R.P.D.-Abzeichen, Menüleiste. Eine Elza-Karte gibt es nicht |
| `CONFIG.TIM`, `C_BACK2.TIM` | Optionen, Tasten, keine Story |
| `MAP01–0D.PIX` (256×256, 4 bpp) | Nur Bereichstitel: POLICE STATION B1, B2, 1F, 2F, 3F, POLICE STATION (ohne Stockwerk), DRAINS B2, FACTORY, LABORATORY B1–B4, **SUBWAY**. Keine Raumnamen |
| `ITEMALL.PIX`, `MIXITEM.PIX` | 72 und 14 Icons. Viele tragen `仮` (vorläufig), mehrere sind Pistolen-Platzhalter |
| `ITEM/ITPS.ITP` | 71 Bilder 112×72. **65 sind der Platzhalter „BOSS 仮キャラ“.** Echt sind 8 (Schrotflinte), 9 (Granate), 21, 22 (Munition), 54 (ID-Karte `TYPE M`, `GORO MACIDA`, Umbrella-Logo), 62 (goldene Taschenuhr) |

**Welche Karte für wen:** Die Karten hängen nur an der Stage (`0x80074c0c[stage]`, sechs Funktionen `0x8004b568`–`0x8004bf70`, Aufruf `0x8004996c`–`0x80049994`). Keine liest das Spielerbit. Beide Figuren bekommen dieselben Karten.

Bei ITPS-Nummer und Item-ID ist unklar, warum die Uhr auf 62 liegt, während 8, 9, 21, 22 und 54 ohne Versatz passen.

## 4. STAGE5.BIN, STAGE6.BIN, Enden

**Registrierung der Figuren.** Jedes Overlay trägt seine Handler in `0x80072bac[typ]` ein. Das Ergebnis ist neu:

| Overlay | NPC-Typen | Beleg |
|---|---|---|
| STAGE1 | 40, 42, 45, 47, 49, 4B | `0x8011e91c`–`e96c` |
| STAGE2, 3, 4 | 40, 42, 49, 4B | |
| STAGE5 | 40, 42, 49, 4B, **4D** | `0x8011dd78`–`ddb8` |
| STAGE6 | 40, 42, 49, 4B, 4D, **und sonst nichts** | `0x80101dac`–`0x80101dfc`: `0x80072cac, cb4, cd0, cd8, ce0` |

Die Overlays gelten für beide Szenarien, es gibt je Stage nur eines.

STAGE6.BIN enthält ausschließlich diese fünf gleich gebauten NPC-Handler und Tabellen, also weder Gegner noch einen Abspann. Es liest weder `DAT_800aca3c` noch `DAT_800aca5c`, weder Flags noch Stage. Das ist ein zweiter, unabhängiger Beleg für die fünf Figuren im Zug.

**Figur 0x4D in Stage 5:** STAGE5.BIN hat einen Handler, aber kein Disc-Raum der Stage 5 setzt 0x4D. Diese Figur war also auch im Labor vorgesehen, in Räumen, die auf der Disc fehlen. Auf Stage 1–4 kommt sie nicht vor.

**Figurenlogik in den Overlays:** STAGE1–5 haben je genau eine Lesestelle der Spielfigur, derselbe Trefferhandler (STAGE5 `0x80104134`, `andi 4`). Er behandelt Leon und Elza in einem Trefferablauf verschieden (`andi 4`, je ein eigener Zweig), mehr nicht. Sprungziele nach dem Spielerbit gibt es nicht. Vergleiche auf 0x0D, 0x0E, 0x0F in Overlays gibt es nicht.

**(4,22), (4,23):** Nur `Ck` in `ROOM6030 @0x396, @0x39E`, `ROOM6031 @0x38C`. Kein Setzer auf der Disc. Geprüft: `Set` per Walker und Rohsuche, AOT sce 4 (22 Stück, nur Bank 5, Bits 32–35), `Aot_reset` auf 4, EXE und Overlays. Bank 4 = `0x800b1018`, einziger Code-Bezug `0x8003e560`, eine Nullschleife. Der Bericht stimmt, und zwar weiter als seine Rohsuche belegt.

**Enden:** Es gibt keine Abspann- oder Enddatei, kein `STAFF`, `ROLL`, `ENDING` in den Strings. MOVIE enthält nur das Capcom-Logo. 6030, 6031, 6040, 6041 enthalten 0 `Message_on` und `0x3C=0`. Ihre Flags sind nur Bank 1 Bit 27 und Bank 5. Ein Ende löst keines dieser Skripte aus.

## 5. Spielerbit und zwei Szenarien

**Bit 31 von `DAT_800aca3c`** (Bank 1 Bit 0; Bitformel @`0x8003fd40`, `0x80000000 >> (bit&31)`).

Setzer: nur TITLE.BIN `0x801016c4`–`0x801016ec` und `0x801024ac`–`0x801024ec`. Maschinelle Prüfung aller 95 Stores auf das Wort in EXE und Overlays: Nur `0x8001d544` (`& 0x80000000`, behält) und `0x80039730` (`& 0xffff0000`, behält) berühren es sonst.

Leser in der EXE (maschinell geprüft):

| Stelle | Wirkung |
|---|---|
| `0x8001d29c` | Startraum |
| `0x8002232c` | Archiv: Datei 0x26 CDEMD0 oder 0x27 CDEMD1, Tabellen `0x80072f38`, `0x80073178` (bestätigt die Bericht-Adressen) |
| `0x800397b8`/`0x800397e4` | RDT-Index + Bit 31 |

Kein SCD enthält `Ck(1,0)` oder `Set(1,0)`, auch keinen Flagwechsel per AOT. Skripte fragen die Figur nie ab, die Trennung läuft allein über die Datei.

**Start (`0x8001d284`–`0x8001d38c`):** Bei `DAT_800aca38 & 0x20000`, gesetzt in TITLE `0x80102490`:
- Leon: Stage 0, Raum 0x17 = **117 OKUJYOU**, (3940, 14167), Kamera 4.
- Elza: Stage 0, Raum 3 = **103 LOBBY**, (−8200, −22500), Kamera 0.

Der Bericht nennt das nicht; §5 beginnt mit „Dach 117“ für beide. `SELECTH.TIM` zeigt die zwei Orte.

**Titel:** NEW GAME, LOAD DATA, CONFIG, dann nur LEON oder ELZA (`0x801023d0`–`0x801024f0`, `xori 1`). Kein dritter Eintrag, kein A/B, kein zweiter Durchgang.

**Speicherstand:** Dormant. Nur ein Dateiname `BISLPS-00222*`. Titel `Leon/NN/` oder `Elza/NN/` nach `[2]&3` (`0x8002685c`) bzw. `0x800b0fbe&1` (`0x80026e70`). Das passt nicht zu Elza = 4 (`0x801024cc`), diese Logik ist veraltet. Kein Feld für ein zweites Szenario.

**Weltspuren:** Die drei Klammern aus Bericht §4 sind in Leons Dateien fest gesetzt, nicht aus Elzas Lauf übernommen. Technisch ist das kein Zapping. Dass der Bericht das „Vorläufer“ nennt, ist als erzählerische Beobachtung haltbar.

**`work_vars[0x10]` = `0x800b0ff0`:** Code-Schreiber nur `0x8001d558` (Startwert aus `0x800aca5c`). Leser `0x80039768`, `0x8003977c`. SCD-Schreiber sind 0x24, 0x25, 0x26, 0x27, 0x3D; auf der Disc schreiben sie nur die Indizes 4, 5, 6, 7, 0x12, **nie 0x10**. Lesend nur `Cmp` in `ROOM3000 @0x161A`, `ROOM3001 @0x1630`. Bericht §7 stimmt.

**Neu zu §7, Sherry steckt im Engine-Code:**

| Stelle | Wirkung für Spielfigur 0x0F |
|---|---|
| `0x800316b8` | Waffe = 0 (sonst Leon 3, Elza 4) |
| `0x8003258c` | Ein Codezweig wird übersprungen (Zweck nicht näher bestimmt) |
| `0x80032890` ff. | Derselbe Vergleich in einem zweiten Zweig |
| `0x80038190` | Anderer Vergleichswert, 0x63 statt 0x1D |

Für 0x0D und 0x0E gibt es keinen solchen Vergleich in der EXE. Marvin hat nur die Skriptabfrage in 300, John gar nichts. Daraus lese ich, dass Sherry als Spielfigur am weitesten gediehen war; das ist Deutung.

## 6. BSS, RDT und fehlende Räume

96 RDT, 61 BSS.

| Kategorie | Räume |
|---|---|
| BSS ohne RDT | 11F, 122–126, 201, 208, 404, 408, 50D |
| RDT ohne BSS | 106 |
| Nur Leon-RDT | 1060, 1180, 2030, 2070, 2090, 3020 |
| Nur Elza-RDT | keine |
| Im Mod-Baum, nicht auf der Disc | 144 RDT, 59 BSS |

1060 (1404 B) und 1180 sind Stubs ohne Main-Skript vom 11.9.1996. 1060, 1180, 2090 haben keine Tür. Ihr Fehlen bei Elza sagt also nichts über die Story.

**ROOM50D:** Ein einziges Bild (65536 B, 18.9.1996), graue untexturierte Blockgeometrie, Tabelleneintrag `TEST`. Das ist kein Story-Raum. Der Mod-Baum hat unter gleichem Namen eine andere Datei (1 MB).

Die fünf Zellen 122–126 sind fertig gerendert (27.10.1996), 121 hat dorthin inerte Türen (sce 0). Der Bericht schreibt „Zelle 121“; 121 heißt in der EXE RYUUCHI SYO, die Einzelzellen sind 122–126.

**Türgraph.** Vergleich der Türziele aller Raumpaare: **Leon und Elza haben überall dieselben Türziele, außer in Raum 20B.** Slot 2 führt in 20B0 nach 300, in 20B1 nach 203. Das ist die einzige belegte Weggabelung im Code.

Tür-Ziele ohne passende Datei:
- 201, 202 aus 2000/1 und 20B0/1
- 203 (Elza, 2031 fehlt)
- 302 (Elza, 3021 fehlt)
- 305, 406 aus 3040/1
- 123–126, 513 inert

Der Weg ans Ende ist für beide gleich: 511 → 514 → 604 → 603. Keine Disc-Tür führt zu 600–602 oder 605–607. Diese Räume sind also vorgesehen, aber auf der Disc nicht angebunden.

## 7. Modelle

Aufbau eines NPC-Blobs: 9 Abschnitte, `[1]` EDD0, `[2]` EMR0, `[3]` EDD1, `[4]` EMR1, `[7]` MD1, `[8]` TIM.

| Vergleich | EDD/Keyframes | Knochen-Offsets | Mesh | Textur |
|---|---|---|---|---|
| 0x40 = 0x45 = 0x47 | Blob byte-gleich, md5 `0c1f2a` | | | |
| 0x40 gegen PL0D | | gleich | gleich (`8eb23f`) | gleich (`543a5a`) |
| 0x40 gegen PL00 (Leon) | anders | **15/15 gleich** | **15 Körper-Meshes byte-gleich** | 65 % innerhalb Toleranz 8 |
| 0x40 gegen 0x49 (John) | **Bank 0 gleich** (`59aa95`, `245e74`), Bank 1 anders | 15/15 anders | anders | anders |
| 0x4D gegen 0x42 (Ada) | **beide Bänke gleich** | **14/15 anders** | anders | anders |
| 0x4D gegen PL04 (Elza) | anders | **15/15 gleich** (`588e4a`, Wurzel −1709) | 10/15 byte-gleich, Knochen 1, 2, 3, 5, 6 gleiche Topologie, 20–30 Vertices verschoben | 62 % innerhalb Toleranz 8, gleiches Layout |
| PL0F gegen 0x4B | EMR0 byte-gleich | gleich | gleich | gleich |
| PL0E gegen 0x49 | PL0D und PL0E teilen EDD `ed8fc6`, 12 Clips | gleich | gleich | gleich |

Ergebnisse:
- „0x40/0x45/0x47 ein Blob“ **stimmt**. Genauer: Es ist Leons Körper mit Johns Clips aus Bank 0.
- „0x4D ist Elzas Kunst auf Adas Rig“ ist **ungenau**. Skelett und fast das ganze Mesh sind Elzas. Von Ada stammen nur die Clips und Keyframes.
- Das Muster ist symmetrisch: Beide Hauptfiguren existieren als NPC-Variante mit den männlichen bzw. weiblichen NPC-Clips.

Warum Adas Clips: `ROOM6030 @0x3A2` und `@0x3BA` sind bis auf den Typ byte-gleich. Dieselbe Szene muss beide Figuren bewegen können.

**Hauptfigur-NPC oder andere Figur?** Für einen Platzhalter sprechen:
- Dasselbe Bauschema wie bei 0x40, das sicher Marvin, Irons und Roy vertritt.
- Video 1:56–1:58 zeigt eine andere Frau.
- 0x4D ist auf Stage 5 und 6 beschränkt, passt also zu einer Laborfigur.
- Dieselbe Szene wechselt zwischen Ada und 0x4D.

Beweisen lässt sich das aus den Daten nicht. Clip-Sätze und Skelett unterscheiden „Elza als NPC“ nicht von „Elza-Körper als Stellvertreter“. Das Urteil „Platzhalter“ ist wahrscheinlich, nicht belegt.

Dieselbe Vorsicht gilt für Elzas 604: Dort steht Typ 0x40, und dessen Körper ist auf der Disc Leon. Marvin ergibt sich allein aus der Typzuordnung; 0x40 heißt in Leons Szenario per Text Marvin. Für Elza–Marvin gibt es unabhängig das Video 0:46.8.

**Ada auf der Disc:** Das Modell 0x42 zeigt weißen Kittel, braun gestreiftes Oberteil, kurzen Rock, nackte Beine, dunkles Kurzhaar (`figuren_render.png`). Das Video zeigt dieselbe Figur bei 1:23.5–1:24, 1:42–1:43, 2:16. Die zweite Frau bei 1:56–1:58 hat weißen Kittel, dunkle Front, dunkle Hose, helles Haar (`sh_kittel_zoom.jpg`).

**Mod-Stand:** Die Modelle sind laut README Fan-Arbeit: „new Ada“, „new Annette“, „new Marvin“, „new Irons“ (Juvenal), „new John“ (Rusty). Sie taugen also nicht als Capcom-Beleg. In CDEMD1 ist 0x42 eine Elza-Figur mit Pferdeschwanz und Waffe, deren EMR0 byte-gleich zu Disc-0x4D ist (`d1e52b`), 0x47 nach der Textur eine Leon-Figur (Sichtbefund). Das sind die „Leon and Elza models“ der README. Rosters der Overlays sind in Mod und Disc gleich.

## 8. Disc-Image

- 52849 Sektoren. 334 Dateien im ISO, 16 Verzeichnisse, Pfadtabelle stimmt damit überein.
- **333 Dateien byte-gleich zum Ordner.** Fehlend im Ordner: nur `PSX/MOVIE/CAPCOM.STR` (LBA 5619, 1125 Sektoren, 29.7.1996). Vermutliche Ursache: Die Datei hat Audio- und Video-Sektoren (Submode 0x42, 0x64), die ein normales Kopieren nicht mitnimmt. Inhalt: nur das Capcom-Logo (`str/sheet.jpg`).
- Die EXE-Dateitabelle hat genau 334 Einträge in Disc-Reihenfolge, keine Einträge auf verschwundene Dateien.
- Lücken: nur LBA 18–21 (Pfadtabellen) und die letzten 150 Sektoren (null). Datei-Slack: bei keiner Datei Nicht-Null-Bytes. Verzeichnis-Sektoren: keine Reste, keine versteckten Einträge.
- `ZNULL.DAT`: 37 044 000 Byte, alle null.

Es gibt also keine gelöschten Dateireste.

## 9. Korrekturen am Bericht

| Stelle | Vorschlag |
|---|---|
| §8 Zeile 0x4D | „Elzas Körper und Skelett (15/15 Offsets wie PL04) mit Adas Clips.“ |
| §3, §5, §6, §10 „Frau im Kittel“ | Ada trägt ebenfalls Kittel. Besser: „zweite Frau im Kittel (dunkle Hose, helles Haar)“. Video-Stellen 1:56–1:58 ihr zuordnen, 1:23, 1:42, 2:16 Ada |
| §3 Elza–Marvin 604 | Hinweis: Typ 0x40 ist auf der Disc Leons Körper, Zuordnung nur über den Typ |
| §5 | Startpunkte ergänzen: Leon 117, Elza 103. Weggabelung 20B ergänzen |
| §6 „Waggon“ | Antriebswagen (`DOURYOKUSHITSU`, Maschinen und Bank). 605–607 sind geplant, ohne Dateien. „Der andere Hauptcharakter fehlt“ gilt nur für 603 und 604 |
| §1 „Vorläufer des Zapping“ | Als erzählerisch kennzeichnen. Im Code gibt es keine Übernahme |
| §3 „1150 und 1151 byte-gleich“ | Nur Skripte (836 B) und Text. Dateien sind verschieden: 140216 und 179092 B, nOmodel 4 und 5. Das fünfte Objekt in 1151 wird nicht benutzt |
| §7 | Sherry im EXE-Code ergänzen, 0x0D und 0x0E haben nichts |
| §8 Parkposition | Auch John in 10B0 |
| §12 „Mod musste 144 Raumdateien ergänzen“ | 144 ist die Differenz der Bäume. Die Basis ist Team IGAS, wie viele davon Capcom-Material sind, ist unklar |
| §5 Gleiches Ziel | Enzym liegt nur in 40A1; T- und G-Impfstoff sind getrennte Items |

## 10. Nicht geklärt

- Was „U“ in ROOM115U bedeutet.
- Warum ROOM506U Item 44 prüft.
- Welche Handlung zur Taschenuhr gehört. Die Mod nennt „Sherry's clock“, das ist Fan-Text.
- Ob `TYPE M`, `GORO MACIDA` Story-Figur oder Entwicklerscherz ist.
- Warum das Uhrenbild auf ITPS-Nummer 62 liegt.
- Ob 0x4D im Stand von November 1996 als Platzhalter oder als NPC-Elza gedacht war.
- Was in 605–607 und den Räumen 600–602 geplant war. Die Mod-README bezieht „Annette in room 607“ auf einen Screenshot, den habe ich nicht gesehen.
- Ob „L“ in L TSUURO, L B1 ROUKA für L-förmig steht.

## 11. Belegdateien

Alles unter `scratchpad/s40/pruefung/`: `scd115.txt`, `scd506.txt`, `scd506_mod.txt`, `strings_all.txt`, `rmw.txt`, `aca3c.txt`, `figuren_render.png`, `tex_cmp.png`, `tex_mod.png`, `sh_kittel.jpg`, `sh_kittel_zoom.jpg`, `sh_50d.jpg`, `sh_nurbss.jpg`, `sh_115_506.jpg`, `sh_604.jpg`, `img/maps.png`, `img/tims.png`, `img/itps.png`, `img/ITEMALL.png`, `str/sheet.jpg`, `iso_files.json`, `ftab.json`, `doors.json`.
