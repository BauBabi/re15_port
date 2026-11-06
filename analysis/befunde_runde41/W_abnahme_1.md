# Runde 41 — Spur W "wasserblink": Abnahme 1

Prüfgegenstand: Zweig `r41/wasserblink`, Stand 82e18d558. Er umfasst den Fix 3f565627f, die Riegel 7ab9ffc16 und die Doku 82e18d558, Basis master aa88a1b6a.
Der Abnehmer hat dem Bau nichts geglaubt. Alles unten ist selbst gebaut, selbst gemessen und selbst disassembliert.

## Urteil

**NICHT BESTANDEN. Grund ist allein Mangel M1, eine falsche Aussage zum Original in Code-Kommentar und Dossier.** Die Code-Änderung selbst nehme ich ab.

| Frage des Auftrags | Ergebnis |
|---|---|
| Blinkt noch etwas (Marke, alle Cuts 2000/2001/20B0/20B1, Leon und Elza)? | **Nein.** Im Kanal blinkt nichts mehr. Übrig sind nur die Figur selbst, das vereinbarte Wasser an der Leiter mit dem Original-Gitterstrahl 0x0B sowie die Blenden und die Flutwelle der 20B0-Szene. Alles drei sieht mit der Nutzer-exe gleich aus. |
| Ursache belegt oder nur das Symptom weg? | **Belegt.** Die Nutzer-exe mit `RE15_KEIN_RAUMLICHT=1` ergibt im Kanal dasselbe Bild wie der Fix, pixelgleich. Die Hash-Konstanten von `rl_hash` stehen nur in der Nutzer-exe. Der Unit-Riegel mit dem alten `raumlicht.c` liefert Licht in 2552 von 6000 Bildern. |
| Aussage zum Original | **Video: stimmt. Skript: falsch dargestellt.** Effekt 6 Sub 0 ist ein **fallender Deckentropfen mit Aufschlag-Spritzer** und steht nicht still bei y -10000. Im Repo-Baum startet main00 die Schleife (@0x0177C). Zwei der acht Tropfpunkte fallen ins Bild von Cut 2, einer davon in den Kanal. Auf der Disc Version_40% liegt die Schleife zwar vor, wird aber nicht gestartet. Einzelheiten unter §3. |
| Wasserlauf an der Leiter vor und nach dem Ablassen | **Wie vereinbart.** Vorher läuft er, bei Leon und Elza gleich der Nutzer-exe. Mit Bit (9,100) oder (9,103) ist er aus (0 Wechsel). In 20B0/20B1 entsteht er nicht (in keinem der 32 Läufe eine Zeile `[wasserlauf41]`). |
| Nebenwirkungen (20B0-Flut, Sprung ins Wasser, andere Räume) | **Keine.** 20B0 Cut 12/13/15 zeigt mit Fix und Nutzer-exe dasselbe Profil. 34 Wasser-Tests sind grün. `re15_raumlicht_liste` hat genau einen Aufrufer, und der 50B0-Schein ist unverändert. |
| Riegel rot ohne den Bau? | **Ja.** Leon und Elza sind mit der Nutzer-exe rot, mit dem Fix grün. Der Unit-Riegel ist mit dem alten `raumlicht.c` rot. |

## 1. Bau und Messweg

* Eigener Bau: `local_build.sh configure` und `build`, beide OK. Vorher entfernt: `raumlicht.c.obj` und `re15_pc.exe`. exe vom 2026-11-01 18:31, md5 `5e4a0526…`.
* „Vorher“ ist die **Nutzer-exe** selbst: Hauptbaum `re15_port/build/platform/pc/re15_pc.exe` vom 08:21, md5 `b8a6d111…`, als Messkopie `re15_wa1_vorher.exe`.
* Binärprobe: Die rl_hash-Multiplikatoren 0x7feb352d und 0x846ca68b stehen in der Nutzer-exe je 1-mal, in der Fix-exe 0-mal.
* Fenster: `W_werkzeug/abnahme1/wa_lauf.py` ist eine Kopie des Musters `w_fenster.py`.
  * Eigene PID, gdigrab `hwnd=` mit **30 Bildern/s**, Fenster 320x240, also 1:1 PSX-Bildpunkte.
  * Beschleunigter Renderer direct3d, **kein AUTOSHOT, kein SOFTWARE_RENDER**.
  * Messkopien `re15_wa1_fix.exe` und `re15_wa1_vorher.exe`. Beendet wurden nur die eigenen PIDs.
* Weg zur Marke: `RE15_DEBUG_JUMP=2000@90` und `RE15_PLAYER_POS=-7000,-25659,1976,1`.
  * Die RVD-Zonen schalten Cut 0 → 1 → 2.
  * Dann `RE15_INPUT_SCRIPT=U1.6` ab Spielbild 130 (`RE15_INPUT_SCRIPT_BASIS=spiel`).
  * Leon steht danach bei (-10552,-1800,-26043), Cut 2. Das liegt 133/384 neben der Marke (-10685,-25659).
  * Ausgewertet werden **16 s (480 Bilder)** im Stand.
* Cut-Übersicht: `RE15_FORCE_CUT=0..15` mit je **11 s (330 Bilder)** im Stand.
* Auswertung `wa_blitz.py`: Ein **Blitz** ist ein Lauf von 1 bis 12 Bildern, in dem ein Bildpunkt mehr als 25 über seinem Median liegt und danach wieder fällt. Der Punkt muss in mindestens 80 % der Bilder ruhig sein. Ein Fleck zählt ab 3 Bildpunkten.
* Zuordnung `wa_tabelle.py`: Blitze im Kasten der Figur aus `befund.log` gelten als Figur. Der Kasten wird um ±12 in x und um -45..+8 in y erweitert, weil der Kasten Hüfte bis Fuß deckt und Kopf und Arme fehlen.
* `wa_blende.py`: Bilder, in denen Blitze zusammen 150 Bildpunkte oder mehr bedecken, sind Blenden.
* `wa_wiederkehr.py`: Ein Ort mit 3 oder mehr Blitzen gilt als ortsfest.
* Zwei Läufe hat ein fremder Eingang gestört: `D_fix_elza_2000_c10_ohneflag` und `G_fix_leon_20B0_c12`.
  * Ein virtueller „Xbox 360 Controller“ eines anderen Prozesses hing in 43 von 112 Läufen am System.
  * Im ersten Fall endete die exe im Titel, im zweiten landete der Lauf in LOAD GAME.
  * Beide habe ich wiederholt, danach waren sie in Ordnung. Mit dem Fix hat das nichts zu tun, denn die Nutzer-exe war ebenso ausgesetzt.

## 2. Messung: blinkt noch etwas?

### 2.1 An der Marke (ROOM2000/2001 Cut 2, echte Zonen, 16 s)

Fenster des Riegels im Kanal, x 104..230, y 100..200 (`wa_fenster_zahl.py`):

| Lauf | Bildwechsel > 8 im Kanal | größter Sprung |
|---|---|---|
| Nutzer-exe, Leon 2000 | **73 / 479** | 128 |
| Nutzer-exe, Elza 2001 | **72 / 479** | 128 |
| Nutzer-exe, Leon, `RE15_KEIN_RAUMLICHT=1` | **0 / 479** | 0 |
| Fix, Leon 2000 | **0 / 479** | 0 |
| Fix, Elza 2001 | **0 / 479** | 0 |

* Ortsfeste Blitze außerhalb der Figur: Nutzer-exe 74 (Leon) und 53 (Elza), an (184,158), (134,144), (172,152) und (123,136). Das sind genau die projizierten `s_glanz`-Punkte, Dauer 3 bis 5 Bilder.
* Fix und Nutzer-exe mit KEIN_RAUMLICHT: **0**. Nur an Leons Kopf (75,61) gibt es Leerlauf-Bewegung, in allen drei Läufen gleich.
* Das Median-Bild des Fixes gleicht dem der Nutzer-exe mit KEIN_RAUMLICHT **pixelgleich**, im ganzen Bild außerhalb der Leon-Spalte (max |d| = 0).
* Die gelben Bodenflecken stehen in allen drei Läufen still: Mittel fern 5,26/5,26, vorn 30,15/30,15 in allen 480 Bildern.

Bildfolgen: `W_belege/wa1_folge_leon_c2.jpg` und `wa1_folge_elza_c2.jpg`, Bild 31..46. In der Nutzer-exe erscheint die Ellipse bei Bild 33..37 rechts und 38..42 in der Mitte, im Fix in keinem Bild.

### 2.2 Alle Cuts (FORCE_CUT, Fix, 11 s je Cut)

| Raum / Figur | Cuts ohne jeden Blitz außerhalb der Figur | Cuts mit Blitzen | Herkunft |
|---|---|---|---|
| ROOM2000 Leon | 0–7, 11, 12, 14, 15 | 8, 9, 10, 13 | Leiterwasser der Spur N und Gitterstrahl 0x0B (§4) |
| ROOM2001 Elza | 0–7, 11, 12, 14, 15 | 8, 9, 10, 13 | dasselbe |
| ROOM20B0 Leon | 0–4, 8, 9 | 5–7, 10–15 | Blenden der Ankunftsszene, Flutwelle, laufender Leon (§5) |
| ROOM20B1 Elza | 0–4, 8, 9 | 5–7, 10–15 | dasselbe |

* Positivkontrolle mit der Nutzer-exe, gleicher Messweg, je 11 s:
  * Leon Cut 2/6/7/15: **39/21/43/8** Blitze außerhalb der Figur.
  * Elza Cut 2/6/7/15: **40/21/42/8**.
  * Fix in denselben Cuts: **0**.
* In Cut 6 und 7 sitzen die Blitze der Nutzer-exe auf der Wand. Das bestätigt die fehlende Regions-Prüfung, die Spur W angegeben hat.
* Karten: `wa1_karten_leon_2000.jpg`, `wa1_karten_elza_2001.jpg`, `wa1_karten_leon_20b0.jpg`, `wa1_karten_elza_20b1.jpg` und `wa1_karten_vorher_kontrolle.jpg`. Rot zeigt jeden Bildpunkt mit Blitz über dem Median-Bild.

## 3. Original — Video und Skript-Bytes selbst geprüft

### 3.1 80%-Video (stimmt)

* `documentation/Biohazard 1.5 Complete Disc Video 2 of 4 (360p).mp4`, Bild 232..516 (7,7..17,2 s). Das ist ROOM2000 Cut 2: Leiter, Kanal und Leon auf dem rechten Sims.
* Die weißen Wolken sind **Spritzer unter den Füßen watender Zombies** und wandern mit der Figur:
  * Zombie 1: Bild 232–270.
  * Zombie 2: Bild 366–426.
* Ohne Figur im Wasser (Bild 432..516) blitzt nichts. Die gelben Bodenflecken stehen im ganzen Abschnitt still.
* Beleg: `wa1_80prozent_kanal.jpg`.

### 3.2 Skript-Bytes (eigener Durchlauf)

Für den Durchlauf habe ich die Längentabelle aus `scd_vm.c` übernommen (`s_opcode_sizes`, @0x800744a8). Anders als in `w_scd.py` ist **0x4C = 18 B** (dort 5). Das Werkzeug ist `wa_scd.py`. Es fand keinen Versatz, und die Opcode-Zahl ist plausibel.

* Gitterstrahlen 0x0B: Repo @0x0174C/@0x0175C/@0x0176C = 40% @0x01568/@0x01578/@0x01588, gleiche Bytes `3a 00 0b 00 00 00 00 19 …`. Die Angabe von W **stimmt**.
* Wasserflächen obj 0..2, Typ 3, Attribut 0x001A, @0x016E6/@0x01708/@0x0172A: **stimmt**.
* Schleife Effekt 6 Sub 0, Repo sub07 @0x01B06..@0x01BAE: `For 0xB4` @0x01B36/@0x01B60/@0x01B8A, `For 0x2D` @0x01BA4, `Goto` @0x01BAE.
  * Ein Umlauf dauert 3·180 + 45 = **585 Bilder**.
  * Gespawnt werden: Punkte 0–2 bei t=0, 3–4 bei t=180, 5–6 bei t=360, 7 bei t=540.

**Was W nicht geprüft hat. Es widerlegt die Aussage „Effekt 6 Sub 0 bei y -10000, in Cut 2 über dem Bildrand“ als Verhalten des Originals:**

1. **Effekt 6 Sub 0 ist ein fallender Tropfen.** Die Zeilen stehen in der Raum-ESP, `wa_esp.py`, Parser wie `re15_esp.c`. Repo-RDT: Bank @0x14608, Effekt 6 @0x147E0, Zeilenblock @0x149F4, Sub 0 1 Strom × 4 Zeilen @0x14A0C.
   * Zeile 0 @0x14A0C, `0a 00 …`: Routine **10** @0x800176b0.
     * Setzt flags := +0x0e = 3 und tpage |= 0x60.
     * Setzt die Anim (+0x6e) := 0x1C.
     * Schaltet sofort weiter.
   * Zeile 1 @0x14A34: Routine **3** @0x80017334. Sie zählt +0x16 = **9** Bilder herunter und schaltet dann weiter (+0x26 = 1).
   * Zeile 2 @0x14A5C, `0d 00 00 00 00 10 00 10 00 00 0a 00 00 00 03 00 … 26 00 … 01 00`: Routine **13** @0x80017990 mit **accel (0,10,0)** (Bytes @0x14A64).
     * Die Physik der Tick-Kette läuft, weil flags = 3 ist und Bit 5 nicht gesetzt ist (Doku `RE15_ESP_ROWMACHINE.md` §1, flags-Bit 5 = Physik-Halt).
     * Routine 13, selbst disassembliert: Sie liest die Weltlage +0x28/+0x2a/+0x2c. `jal 0x8001c6e8` (room_coll) liefert den Boden.
     * Liegt y unter dem Boden, folgen nacheinander:
       * `sb +0x0e → +0x6c`,
       * `+0x38 -= (y - Boden)` (Klemmen auf den Boden),
       * `+0x6e := +0x16 = 0x26`, die Spritzer-Anim (@0x80017a20-28),
       * dann `jal 0x800174e4` (weiterschalten zu Zeile 3, noop).
   * Fallzeit von y -10000 bis zum Boden: etwa 45 Bilder.
   * Version_40%, Sub 0 @0x275C: dieselben Zeilen. Zeile 2 @0x27AC mit accel (0,10,0) @0x27B4, Aufschlag-Anim 0x0A.
2. **Der Repo-Baum startet die Schleife, die 40%-Disc nicht.**
   * Repo main00 @0x0177C `04 ff 18 07`: Evt_exec auf sub07, unbedingt, direkt nach den drei 0x0B.
   * 40% main00 endet @0x01598 mit `01 00`, ohne Evt_exec.
   * Die Byte-Suche `04 ?? 18 03` in 0x1424..0x1670 ergibt **keinen** Treffer.
   * Ob ein anderer Weg (AOT, EXE) sub03 der 40%-Disc startet, ist nicht geprüft.
3. **Zwei Tropfpunkte des Repo-Baums fallen in Cut 2 ins Bild.**
   * Kamera: RID Cut 2 @0xA0 `00 00 3c 68 86 07 00 00 12 ee ff ff b4 9d ff ff b0 cb ff ff ea 00 00 00 b4 af ff ff 3c 09 00 00`.
     * Lage (1926,-4590,-25164), Ziel (-13392,234,-20556), H = 0x683C>>7 = 208.
     * Gegenprobe der Rechnung: Leon (-10552,-1800,-26043) ergibt (83,108). `befund.log` sagt scr=(84,108).
   * Punkt 5 (-14476,·,-23171) aus sub07 @0x01B6A fällt von (118,-27) auf **(125,116), in den Kanal**, an den hinteren gelben Fleck.
   * Punkt 3 (-5540,·,-21205) aus sub07 @0x01B40 fällt von (214,-127) durch die rechte Bildhälfte auf den Sims bzw. die Kanalkante, (197,165) bei y 0.
   * Die übrigen sechs Punkte liegen hinter der Kamera oder hinter der Stirnwand.
   * Bild: `wa1_tropfen_projektion_cut2.jpg`. Blau ist der Fallweg, rot sind die entfernten `s_glanz`-Punkte. Diese liegen zwischen den beiden Aufschlägen.
4. **Der Port lässt die Tropfen nicht fallen.** Lauf `F_fix_leon_2000_c02_fxlog`, `RE15_FX_LOG`, 21 s:
   * Punkt 3 erscheint ab F185, Punkt 5 ab F365, Punkt 7 ab F545. Das passt zum Takt der Schleife.
   * Alle erreichen **A=13**, phys=1, fl=03.
   * Die Weltlage bleibt aber **y -10018..-10000 bis F644**.
   * In `re15_esp.c` hat Routine 13 keinen Fall, sie läuft im `default: break;` (Zeile 1151).

Folge für die Aussagen von Spur W:

* Richtig ist: Ortsfeste Glanzpunkte gibt es nicht. Die 4-Bild-Blitze aus `raumlicht.c` haben **keinen** Bezug zu diesem Mechanismus. Takt, Form und Lage weichen ab, und eine Adresse gibt es nicht. Das Entfernen ist richtig.
* Falsch ist: „Das Original setzt keinen Effekt auf das Kanalwasser … sub07 @0x01B06..@0x01B94 = Effekt 6 Sub 0 bei y -10000“. So steht es im Kommentar in `raumlicht.c` und sinngemäß im Kopf von `test_raumlicht.c`, im Commit 3f565627f und im Dossier §2.1 und „Für den Nutzer“. Die Repo-Daten, die der Port lädt, lassen alle 585 Bilder Tropfen in den Kanal fallen und dort spritzen. „Über dem Bildrand“ beschreibt das **Port-Verhalten** (Routine 13 fehlt) und nicht das Original.
* Für die echte Disc gilt die Aussage nur, weil die Schleife dort nicht gestartet wird. Das steht nirgends.

## 4. Wasserlauf an der Leiter (Spur N), vor und nach dem Ablassen

Weg-Fenster der Spur N in Cut 10, x 72..124, y 146..179 (N-Dossier M2):

| Lauf | Wechsel > 8 | Mittel min/max | `[wasserlauf41]` |
|---|---|---|---|
| Fix, Leon 2000, ohne Bit | 311 / 329 | 36,2 / 49,5 | ja |
| Nutzer-exe, Leon 2000, ohne Bit | 305 / 329 | 36,2 / 49,5 | ja |
| Fix, Elza 2001, ohne Bit | 1137 Blitze außerhalb der Figur (Nutzer-exe 1198) | — | ja |
| Fix, Leon, `RE15_SET_FLAG=9:100` (Leiter frei) | **0 / 329** | 14,7 / 14,7 | nein |
| Fix, Leon, `RE15_SET_FLAG=9:103` (Flut gelaufen) | **0 / 1318** | 14,7 / 14,7 | nein |
| Fix, Elza, `9:100` | **0 / 329** | 14,7 / 14,7 | nein |
| Fix, 20B0/20B1, alle 32 Läufe | — | — | **nein** |

* In Cut 8/9/13 bleiben mit Bit 100 nur die periodischen Spritzer des Gitterstrahls 0x0B, also des Original-Effekts:
  * Cut 8: 32 statt 124 Blitze,
  * Cut 9: 103 statt 244,
  * Cut 13: 33 statt 71.
* Bild: `wa1_wasserlauf_leiter.jpg`. Ergebnis: Der Wasserlauf läuft wie vereinbart bis zur Flut (`re15_wasserlauf_r41_soll`). Der Fix ändert an ihm nichts.

## 5. Nebenwirkungen

* **Code:** Der Diff in `engine/` und `platform/` betrifft nur `raumlicht.c` (−22/+9) und den Kopf von `re15_raumlicht.h`.
  * `re15_raumlicht_liste` hat genau einen Aufrufer, `main.c:6334`.
  * Fall 0x50B0 bleibt unverändert. Einen Fall 0x20B0 gab es vorher nicht und gibt es nachher nicht.
  * main.c ist unverändert, die Messhaken von W sind zurückgenommen.
* **20B0-Flut:** Cut 12/13/15 mit Fix gegen Nutzer-exe, je 11 s:

  | Cut | Wechsel > 8, Fix | Wechsel > 8, Nutzer-exe |
  |---|---|---|
  | 12 | 135 | 139 |
  | 13 | 196 | 197 |
  | 15 | 171 | 171 |

  * Mittel min/max ist gleich, z. B. 37,2/92,9 in Cut 13, und der größte Sprung ist gleich (217/245/92).
  * Dieselben wiederkehrenden Orte, etwa (104,115)x5, (110,116) und (72,155).
  * Es sind die Flutwelle, der vor ihr laufende Leon und die Blenden der Szene. Die Bildfolge Cut 13 Bild 0..15 zeigt mit Fix und Nutzer-exe denselben Inhalt (nur im Scratchpad, nicht eingecheckt).
* **Tests** (eigener Lauf, ctest aus PowerShell mit vorangestelltem `C:\msys64\mingw64\bin`, `-j 3`):
  * `-R "unit_raumlicht|r41_wasserblink|wasser20b0|r38_wasser|nachtrag30_wasser|nachtrag30_laden|unit_r34_wasser|wassersprung"`: **34/34 grün**, 561 s.
  * Darin enthalten: `integration_r38_wasser`, `_wasserzombie`, `integration_r41_wasser20b0`, `_npc` (Sprung ins Wasser, Landungs-Spritzer), `integration_r41_nachtrag30_wasser_leon/_elza`, `_laden_leon/_elza` und `integration_r41_wasserblink_leon/_elza/_gegen`.

## 6. Riegel ohne den Bau

* **Unit:** `test_raumlicht.c` habe ich mit dem alten `raumlicht.c` aus aa88a1b6a gebaut, gcc direkt:
  * Ergebnis rot: `FAIL: Kanal 2000/2001 hat in 2552 Bildern Licht`, rc 1.
  * Mit dem neuen: `0/6000 -> OK`.
* **Integration:** `cmake -P tests/integration/test_r41_wasserblink.cmake`, einmal mit `RE15_PC_EXE` = Nutzer-exe, einmal mit der Fix-exe:

  | TEIL | Nutzer-exe | Fix |
  |---|---|---|
  | leon | **rot**: flacker 16/119 (Sprung 131), gleich 47 bei (171,153) | grün: 0/119, gleich 0 |
  | elza | **rot**: flacker 16/119 (Sprung 131), gleich 47 | grün: 0/119, gleich 0 |
  | gegen | grün (46 bei (43,186)) | grün (46) |

  `gegen` muss in beiden Fällen grün sein, denn 50B0 ist unverändert.

## Mängel

**M1 (Aussage zum Original, nachzubessern):** Folgende Stellen stellen den Tropfen-Effekt als „Effekt 6 Sub 0 bei y -10000“ dar und schließen daraus „das Original setzt keinen Effekt auf das Kanalwasser“:

* `engine/src/raumlicht.c:21` (Block über `re15_raumlicht_liste`),
* `include/re15_raumlicht.h:10`,
* `tests/unit/test_raumlicht.c:4`,
* `tests/integration/test_r41_wasserblink.cmake:6`,
* `W_wasserblink.md` §2.1 und „Für den Nutzer“ (Zeile 171).

Der Commit-Text 3f565627f sagt dasselbe. Er bleibt stehen, die Berichtigung kommt in einem Folge-Commit.

Nach den Bytes ist es ein Deckentropfen: Zeile 2 Routine 13 @0x80017990, accel (0,10,0) @0x14A64, Bodentest room_coll 0x8001c6e8, Aufschlag-Anim 0x26. Im Repo-Baum startet main00 @0x0177C die Schleife, zwei Punkte fallen ins Bild von Cut 2. Auf der 40%-Disc startet kein Evt_exec sie. Der Port zeigt die Tropfen nicht, weil Routine 13 in `re15_esp.c` fehlt (FX_LOG: wpos.y -10018..-10000).

Nachzubessern ist **nur die Doku**, der Code bleibt:

* Original 40% = kein Effekt auf dem Kanal. Die Schleife sub03 ist vorhanden, aber nicht gestartet.
* Repo-Baum (Mod) = Tropfen alle 585 Bilder.
* Port = Tropfen hängen unsichtbar an der Decke.

**M2 (OFFEN 4 von W auflösen, Entscheidung beim Nutzer):** Statt „ob das Original sie fallen lässt, ist nicht geprüft“ gehört der Befund aus §3.2 ins Dossier. Ob der Port die Tropfen bauen soll, entscheidet der Nutzer, und zwar aus drei Gründen:

* Nur der Mod startet die Schleife.
* Der 80%-Videoabschnitt von 9 s zeigt keinen Tropfen. Das ist aber kein Gegenbeleg, denn ein Punkt fällt nur alle 19,5 s.
* Es gilt die Regel „Mod-Inhalt nur mit 80%-Beleg“.

Nebenbei: `RE15_ESP_ROWMACHINE.md` (Korrekturen, Punkt 6) nennt Routine 13 „NUR Room-Drips (11A0/1260)“. ROOM2000/2001 Effekt 6 Sub 0 fehlt dort.

## Für den Nutzer

* Das Blinken im Kanal ist weg, bei Leon und Elza und in allen 16 Kameras von 2000/2001 und 20B0/20B1. Gemessen habe ich im echten Fenster mit 30 Bildern/s. An deiner Marke springt der Kanal in 16 s kein einziges Mal, mit deiner exe vom Morgen 73-mal. Die Ursache war der ausgedachte Glanzpunkt-Effekt in `raumlicht.c`, er ist entfernt.
* Die gelben Flecken bleiben, wie im Original-Video. Ebenso bleibt das Wasser an der Leiter bis zur Flut.
* Eine Einschränkung zur Erklärung von Spur W: Ganz ohne Wasser-Effekt ist der Kanal in den Raumdaten, die der Port lädt, nicht. Dort tropft alle knapp 20 s Wasser von der Decke und spritzt am Boden auf, ein Tropfen davon in Cut 2 hinten in den Kanal. Auf der echten 40%-Disc wird dieser Tropfen nicht gestartet, und der Port zeigt ihn derzeit nicht. Ob er kommen soll, entscheidest du. Die Doku von W muss das richtigstellen.

## Werkzeuge und Belege

* Werkzeuge: `W_werkzeug/abnahme1/`. Dazu gehören wa_lauf, wa_blitz, wa_batch mit den Listen wa_liste1/2.json, wa_tabelle, wa_blende, wa_wiederkehr, wa_fenster_zahl, wa_folge, wa_scd und wa_esp. Pfade auf den Scratchpad sind dort fest eingetragen.
* Bilder: `W_belege/wa1_*.jpg`.
* Messkopien und Mitschnitte sind nach der Abnahme gelöscht. `befund.log` im Bau-Verzeichnis des Baums enthält die Zeilen der Abnahme-Läufe.
