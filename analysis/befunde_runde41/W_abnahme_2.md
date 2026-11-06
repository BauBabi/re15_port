# Runde 41 — Spur W "wasserblink": Abnahme 2

Prüfgegenstand: Zweig `r41/wasserblink`, Stand 222db79bb, Basis master aa88a1b6a. Er umfasst den Fix 3f565627f, die Riegel
7ab9ffc16, die Doku 82e18d558 und die Nachbesserung 1 (f9dca0ee1, 222db79bb) zu den Mängeln M1 und M2 aus `W_abnahme_1.md`.
Ich habe dem Bau und dem Bericht nichts geglaubt: selbst gebaut, selbst im echten Fenster gemessen, Skript-Bytes und EXE
selbst disassembliert, das 80%-Video selbst angesehen und ausgewertet.

## Urteil

**BESTANDEN.** Das Blinken im Kanalwasser ist weg, an der Marke und in allen 16 Cuts, bei Leon und Elza. Gemessen ist in der
Fenstergröße des Nutzers (960x720) und in 320x240. Die Ursache ist belegt. Die Aussagen zum Original stimmen: selbst nachgeprüft
und an einer Stelle enger belegt als bei W (§5.2). Der Wasserlauf an der Leiter läuft wie vereinbart, Nebenwirkungen gibt es
keine, und die Riegel fallen ohne den Bau.
Nachzutragen ist nur die Doku (H1): Den hängenden Tropfen-Fleck, den W für Cut 2 nennt, gibt es auch in Cut 5 und Cut 7. Er ist
kein Wasser und älter als der Fix.

| Frage des Auftrags | Ergebnis |
|---|---|
| Blinkt noch etwas (Marke, alle Cuts 2000/2001/20B0/20B1, Leon und Elza)? | **Im Wasser nein.** An der Marke in 19 s 0 Blitze und 0 unruhige Flecken, die Nutzer-exe hat 64 Blitze und 6 Flecken. In allen 64 Cut-Läufen ist der Kanal ruhig. Übrig bleiben drei Dinge, alle schon in der Nutzer-exe: der Wasserlauf an der Leiter mit den Gitterstrahlen 0x0B (Cuts 8/9/10/13), die Ankunftsszene von 20B0 (Blenden, Flut, Figur) und **ein Bildpunkt an der Decke, der im 8-Bilder-Takt pulsiert**. Das ist der hängende Deckentropfen in Cut 2, 5 und 7 (§4, H1). |
| Ursache belegt oder nur das Symptom weg? | **Belegt.** Die Nutzer-exe mit `RE15_KEIN_RAUMLICHT=1` ist im Median-Bild (960x720) außerhalb der Figur pixelgleich mit dem Fix (max \|d\| = 0). Die `rl_hash`-Konstanten stehen nur in der Nutzer-exe. Unit- und Integrations-Riegel sind mit dem alten Stand rot. |
| Aussage zum Original | **Stimmt.** Die Skript-Bytes von Repo und 40% habe ich selbst durchlaufen, die ESP-Zeilen und Routine 13 selbst disassembliert, alle 9 Aufrufer von `FUN_8003ee3c` geprüft (§5.2) und das Video selbst ausgewertet. Die weißen Wolken sind Spritzer watender Zombies, ein Tropfen ist nicht zu sehen. Die Wertung "schwacher Gegenbeleg" teile ich. |
| Wasserlauf an der Leiter vor und nach dem Ablassen | **Wie vereinbart.** Ohne Bit läuft er (Fix = Nutzer-exe, 1458 Bildpunkte im Weg-Fenster). Mit Bit 100 oder 103 ist er aus (0), in 20B0/20B1 läuft er nie (0 von 32). |
| Nebenwirkungen (20B0-Flut, Sprung ins Wasser, andere Räume) | **Keine.** 20B0 ist im Code unberührt und misst gleich bis auf die Streuung zweier Läufe derselben exe. Unit 23/23 und Integration 11/11 sind grün, darunter Wassersprung, Waten, Wasserlauf und 20B0-Flut. |
| Riegel rot ohne den Bau? | **Ja.** Der Unit-Riegel ist mit dem alten `raumlicht.c` rot (2552 von 6000 Bildern mit Licht). Die Integrations-Riegel leon/elza sind mit der Nutzer-exe rot (16 von 119 Wechseln, Sprung 131; gleich 47) und mit dem Fix grün. |

## 1. Bau, Messkopien, Messweg

* Code-Stand: `git diff aa88a1b6a..HEAD` ändert in `engine/`, `platform/`, `include/` nur `engine/src/raumlicht.c` (Fall `0x2000`
  mit `s_glanz` und `rl_hash` entfernt, `(void)frame`) und den Kopf von `include/re15_raumlicht.h` (nur Kommentar). Die
  Nachbesserung 1 (80826d68e..222db79bb) ändert in `re15_port/` nur Kommentare. `re15_raumlicht_liste` hat genau einen Aufrufer,
  `platform/pc/main.c:6334`. Der Fall `0x50B0` ist unverändert, einen Fall `0x20B0` gab und gibt es nicht. Damit kann der Fix
  außerhalb von ROOM2000/2001 (`room & 0xFFFE == 0x2000`) kein Bild ändern.
* Eigener Bau: `raumlicht.c.obj` und `re15_pc.exe` gelöscht, dann `local_build.sh configure` und `build` (`LOCAL-BUILD-OK (build)`).
  exe vom 2026-11-01 20:39:46, md5 `f72618bb…`. Messkopie `re15_wa2_fix.exe`.
* "Vorher" ist die **Nutzer-exe** selbst (Hauptbaum `re15_port/build/platform/pc/re15_pc.exe`, 2026-11-01 08:21, md5 `b8a6d111…`),
  Messkopie `re15_wa2_vorher.exe`.
* Binärprobe: die `rl_hash`-Multiplikatoren 0x7feb352d und 0x846ca68b stehen in der Nutzer-exe je 1-mal, in der Fix-exe 0-mal.
* Fenster: `W_werkzeug/abnahme2/wa2_lauf.py` (unveränderte Kopie von `abnahme1/wa_lauf.py`, vorher gelesen): eigene PID,
  ffmpeg gdigrab `hwnd=` mit 30 Bildern/s, beschleunigter Renderer, **kein AUTOSHOT, kein SOFTWARE_RENDER**. Beendet werden
  nur die eigenen PIDs. Ein fremder ffmpeg-Mitschnitt eines anderen Agenten (`runs/v_marvin_leon`) lief zeitgleich und blieb
  unangetastet.
* An der Marke läuft das Fenster **in der Größe des Nutzers** (`RE15_WINDOW_SCALE=3`, 960x720 wie `nutzer_marke1_2000_F444.png`),
  für die Cut-Übersicht in 320x240 (1:1 PSX-Bildpunkte).
* Auswertung `wa2_blitz.py` (eigene Fassung):
  * (1) **Blitz** wie Abnahme 1: Lauf von 1 bis 12 Bildern > 25 über dem Median eines sonst ruhigen Punktes, Fleck ≥ 3 Bildpunkte.
  * (2) Zweite, unabhängige Sicht: **Wechselzahl** je Bildpunkt (Bildwechsel mit |d| > 16); ein Fleck mit Wechselzahl ≥ 6 außerhalb
    der Figur gilt als **unruhig**.
  * (3) Oberer Bildrand y 0..3 je Spalte.
  * Figur-Kästen aus den `befund.log`-Zeilen des Laufs, erweitert wie in Abnahme 1.
* `wa2_wechsel.py` listet jeden Bildwechsel > 8 außerhalb der Figur mit Ort. `wa2_folge.py` legt Bildfolgen zweier Läufe
  untereinander. `wa2_batch.py` fährt die Listen `wa2_liste_m.json` (Marke), `wa2_liste_k.json` (alle Cuts) und `wa2_liste_v.json`
  (Gegenproben, Leiter).
* Fehlstart, selbst gefunden und behoben: Die erste Cut-Reihe begann die Auswertung 1 s nach einer festen Wartezeit von 17 s. Der
  Raum erscheint aber erst rund 20 s nach dem Fenster (gemessen in der Marken-Aufnahme: schwarz bis Aufnahmebild 227). Die ersten
  fünf Läufe zählten deshalb die Ladeblende als Blitze (Cut 2: 39, Cut 3: 27). Ich habe die Reihe verworfen. Jetzt setzt
  `wa2_batch.py` den Beginn selbst auf das erste helle Bild nach der schwarzen Ladephase plus 1 s (`ab=auto`). Jeder Lauf wertet
  so 20,7 bis 22,8 s im Raum aus.

## 2. Marke: blinkt noch etwas? (ROOM2000/2001 Cut 2, echte Zonen, 960x720)

Weg wie Abnahme 1: `RE15_DEBUG_JUMP=2000@90`, `RE15_PLAYER_POS=-7000,-25659,1976,1`, `RE15_INPUT_SCRIPT=U1.6` ab Spielbild 130.
Die RVD-Zonen schalten Cut 0 → 1 → 2, Leon steht danach bei (-10552,-1800,-26043), Cut 2 (`befund.log` F195..F780). Aufnahme 34 s,
ausgewertet werden **19 s (570 Bilder) im Stand**, Aufnahme 15..34 s ≈ Raumbild F210..F780.

| Lauf | Blitze außerhalb Figur | unruhige Flecken | Wechsel > 8 außerhalb Figur | Kanal-Fenster x 104..230, y 100..200: Sprünge > 8 im Bild-Max |
|---|---|---|---|---|
| Fix, Leon 2000 | **0** | **0** | 5 (alle bei (83..84, 0..1), Tropfpunkt 7, §4) | **0** (Max 84 in allen Bildern) |
| Fix, Elza 2001 | **0** | **0** | 5 (ebenso) | **0** |
| Nutzer-exe, Leon 2000 | 64 | 6 | 93 | 53 (Max 84..142) |
| Nutzer-exe, Elza 2001 | 64 | 6 | 93 | 53 |
| Nutzer-exe, Leon, `RE15_KEIN_RAUMLICHT=1` | **0** | **0** | 5 (Tropfpunkt 7) | — |

* Die Blitze der Nutzer-exe liegen an (127..141,140..147), (180..187,156..159), (169..174,151..153), (120..126,135..137),
  (134..141,145..147), je 3 bis 5 Bilder lang. Das sind die entfernten `s_glanz`-Punkte.
* **Ursache belegt, nicht nur Symptom weg:** Das Median-Bild des Fixes (960x720, 100 Bilder) gleicht dem der Nutzer-exe mit
  `RE15_KEIN_RAUMLICHT=1` außerhalb der Leon-Spalte **pixelgleich** (max |d| = 0). Das Kanal-Fenster hat in allen 300 geprüften
  Bildern des Fixes und der Nutzer-exe mit KEIN_RAUMLICHT dasselbe Maximum 85, in der Nutzer-exe ohne Schalter 85..151.
* Die gelben Bodenflecken stehen in allen Läufen still (0 unruhige Flecken im Kanal, Median gleich).
* Unit-Riegel gegen den alten Code (gcc direkt, `test_raumlicht.c` mit `raumlicht.c` aus aa88a1b6a): `FAIL: Kanal 2000/2001 hat
  in 2552 Bildern Licht`, rc 1. Mit dem neuen `raumlicht.c`: `Licht in 0/6000 Bildern -> OK`, rc 0.
* Bildfolgen: `W_belege/wa2_folge_kanal_leon.jpg` und `wa2_folge_kanal_elza.jpg` (Kanal-Ausschnitt 960er, je 16 Bilder
  hintereinander, oben Fix, unten Nutzer-exe). Die Nutzer-exe zeigt die Ellipsen in Bild 41..44, 45..48, 49..52, der Fix in keinem.

## 3. Alle Cuts (FORCE_CUT 0..15, Fix, je 20,7..22,8 s im Raum)

Je Lauf 20,7..22,8 s im Raum (Ausnahme: zwei Aufnahmen mit gestreckten Zeitstempeln, 57,5 und 59,0 s, Inhalt gleich). Angegeben ist
Blitze außerhalb der Figur / unruhige Flecken außerhalb der Figur. Kartenblätter (Medianbild, rot = Blitz, gelb = unruhig):
`W_belege/wa2_karten_leon_2000.jpg`, `wa2_karten_elza_2001.jpg`, `wa2_karten_leon_20b0.jpg`, `wa2_karten_elza_20b1.jpg`.

| Raum / Figur | ohne jeden Blitz und ohne unruhigen Fleck | mit Bewegung | Herkunft |
|---|---|---|---|
| ROOM2000 Leon | 0, 1, 2, 3, 4, 6, 7, 11, 12, 14, 15 | 5 (1/1), 8 (249/3), 9 (323/3), 10 (2136/6), 13 (127/5) | Cut 5 = hängender Tropfen (193,1..2), §4. Cuts 8/9/10/13 = Wasserlauf an der Leiter (Spur N) und die drei Gitterstrahlen 0x0B, §6 |
| ROOM2001 Elza | 0, 1, 2, 3, 4, 6, 7, 11, 12, 14, 15 | 5 (1/1), 8 (170/3), 9 (411/3), 10 (2210/6), 13 (129/5) | dasselbe |
| ROOM20B0 Leon | 0..4, 8, 9 (nur 17..30 ganzflächige Wechsel = Ab-/Aufblende der Ankunftsszene) | 5..7, 10..15 | Ankunftsszene: Blenden, Flutwelle, laufende Figur |
| ROOM20B1 Elza | 0..4, 8, 9 (ebenso) | 5..7, 10..15 | dasselbe |

* In Cut 2 und Cut 7 von 2000/2001 bleiben je 30 bis 31 Bildwechsel > 8 außerhalb der Figur. Alle liegen auf einem einzigen Bildpunkt,
  dem hängenden Tropfen (Cut 2 (83,1), Cut 7 (169,15)), §4. Der Kanal selbst ist in allen 16 Cuts ruhig.
* **Gegenprobe mit der Nutzer-exe** (gleicher Messweg; Vergleich der Wechselzahl-Karten mit `wa2_vergleich.py`):

  | Cut | Nutzer-exe Blitze/unruhig | Fix | Nur in der Nutzer-exe unruhig |
  |---|---|---|---|
  | 2000 Cut 2 (Leon, Elza gleich) | 70 / 6 | 0 / 0 | sechs Ellipsen (120..187, 135..159) = `s_glanz` |
  | 2000 Cut 6 | 37 / 3 | 0 / 0 | (9..33, 111..113), Glanz durch die Wand |
  | 2000 Cut 7 (Leon / Elza) | 74 / 6 bzw. 75 / 6 | 0 / 0 | (3..52, 111..116), Glanz durch die Wand |
  | 2000 Cut 15 | 16 / 1 | 0 / 0 | 80 Bildpunkte (77..91, 233..239) |
  | 2000 Cut 5 | 1 / 1 | 1 / 1 | keiner (Jaccard 1,00: derselbe Tropfen-Punkt) |
  | 2000 Cut 8, 9, 10, 13; 2001 Cut 10 | 128..2241 / 3..6 | 127..2210 / 3..6 | keiner (Jaccard 1,00, Korrelation 1,00) |
  | 20B0/20B1 Cut 5..7, 10..15 (12 Paare) | | | Jaccard 0,95..1,00, Korrelation 0,99..1,00 |

* 20B0 ist durch den Code nicht berührbar (kein Fall 0x20B0 in `re15_raumlicht_liste`, vorher wie nachher). Die Restunterschiede
  liegen nur an Bildpunkten mit Wechselzahl ≤ 12, also an den Blenden der Szene. Zwei Läufe **derselben** exe streuen gleich stark:
  Fix gegen Fix Cut 10 Jaccard 0,98, Cut 14 1,00; Nutzer-exe gegen Nutzer-exe Cut 14 0,99 (`R_*`-Läufe).

## 4. Was im Bild noch pulsiert: der hängende Deckentropfen (nicht im Wasser, schon in der Nutzer-exe)

* An der Marke erscheint ab Aufnahmebild 322 (≈ Raumbild F530..F545) ein kleiner Fleck bei x 250..251, y 1..4 im
  960er-Fenster = PSX (83, 0..1). Er wechselt im 8-Bilder-Takt zwischen den Helligkeiten 144, 80 und 112
  (RGB (144,152,136) / (80,88,72) / (112,120,104)) und läuft bis zum Ende der Aufnahme weiter. Fix und Nutzer-exe zeigen ihn gleich
  (erstes Erscheinen: Fix Bild 322, Nutzer-exe Bild 319): `W_belege/wa2_folge_rand_oben.jpg` (Ausschnitt 3-fach).
* Eigene Projektion durch die Kamera Cut 2 (RID @0x00060 + 2·0x20, Lage (1926,-4590,-25164), Ziel (-13392,234,-20556), H 208):
  Punkt 7 (-23020,-10000,-25755) aus sub07 @0x01B94 → (84,2). Die Gegenprobe Leon (-10552,-1800,-26043) ergibt (83,108),
  `befund.log` (84,108). Der Fleck ist also der hängende Tropfen, wie W schreibt.
* **Neu gegenüber W:** Der Fleck steht nicht nur in Cut 2. Mit `W_werkzeug/abnahme2/wa2_tropfen_cuts.py` (Kopie von
  `nb1/wb1_tropfen.py`, Cut als Argument) bestehen in ROOM2000 die Cuts 2, 3, 5, 6 und 7 den Regions-Test für einzelne Punkte.
  Im Bild hängt Punkt 7 bei y -10000 in **Cut 2 bei (84,2), Cut 5 bei (195,2) und Cut 7 bei (170,16)**. Die Cut-Übersicht misst genau
  dort einen pulsierenden Punkt, bei Leon und Elza gleich:
  * Cut 2 (83,1), Wechselzahl 25;
  * Cut 5 (193,1..2), Wechselzahl 27 bzw. 30;
  * Cut 7 (169,15), Wechselzahl 25. Er liegt hier 15 Bildpunkte unter dem Rand im Bild, nicht am Rand.
  Bild: `W_belege/wa2_tropfen_cuts_2_5_7.png`. Die übrigen Punkte projizieren in diesen Cuts über den Bildrand (y < 0).
  Die Nutzer-exe zeigt in Cut 2 und Cut 5 denselben Punkt (Cut 2 Rand 83(25), Cut 5 Rand 193(28), Jaccard 1,00 in Cut 5).
  Der Fleck ist also älter als dieser Fix.
* Der Fleck ist kein Kanalwasser und kein Teil der Nutzer-Marke (dort am oberen Rand über der Decke). W führt ihn als Port-Fehler
  unter OFFEN 4, nennt aber nur Cut 2 (83,0..1). Das ist unvollständig, siehe Hinweis H1.

## 5. Aussage zum Original — Skript-Bytes, EXE und Video selbst geprüft

### 5.1 Skript-Bytes (eigener Durchlauf `wa2_scd.py`, Längen aus `scd_vm.c` `s_opcode_sizes`, 0x2D = 34 wie `scd_vm.c:4437`)

* Repo-Baum ROOM2000: main00 @0x0161E endet mit `@0x0177C 04 ff 18 07` (Evt_exec sub07), `@0x01780 01 00`. Davor die drei
  Gitterstrahlen 0x0B @0x0174C/@0x0175C/@0x0176C und die Wasserflächen 0x2D obj 0..2 Typ 3 Attribut 0x001A @0x016E6/@0x01708/@0x0172A.
  ROOM2001: `@0x0177C 04 ff 18 08`. sub07 (2000) @0x01B06..@0x01BB4 und sub08 (2001) @0x01D32 sind auf 176 Bytes gleich.
* sub07: acht `3a 00 06 00 00 00 00 10` (Effekt 6 Sub 0 Skala 0x1000, y 0xD8F0 = -10000), dazwischen
  `0d 00 04 00 b4 00 / 02 / 00 / 0e 00` (For 180 mit Evt_next) dreimal und `0d 00 04 00 2d 00` (45) einmal, dann
  `@0x01BAE 17 ff ff 00 58 ff` zurück an den Anfang. Ein Umlauf = 3·180 + 45 = 585 Bilder. Punkt 3 @0x01B40 (-5540,-21205),
  Punkt 5 @0x01B6A (-14476,-23171), Punkt 7 @0x01B94 (-23020,-25755). **Stimmt mit W.**
* Version_40% ROOM2000 (= ROOM2001, `cmp` gleich): vollständiger Durchlauf von main00 und sub00..sub03 ohne Versatz:
  * main00 @0x01426..@0x01598: 5× Door_aot_set 0x3B, 3× Aot_set 0x2C (`2c 05 08 …`, `2c 06 08 …`, `2c 07 08 …` = sce 8),
    3× 0x2D, 3× 0x3A Effekt 0x0B, dann `@0x01598 01 00`.
  * sub00 @0x015A4 `01 00`, sub01 @0x015A6 `01 00`, sub02 @0x015A8 `09 0a 14 00 / 2e 02 00 / 00 / 3f 01 0b 00 / 01 00`.
  * sub03 @0x015B6..@0x01664: dieselbe Tropfen-Schleife (andere x/z), `@0x0165E 17 ff ff 00 58 ff`.
  * Im ganzen SCD kein Opcode 0x04 (Evt_exec), 0x18 (Gosub), 0x46 (Aot_reset) oder 0x47 (Aot_on).
* ESP-Zeilen selbst gelesen (Hexdump): Repo @0x14A0C `0a 00 …` (Routine 10), @0x14A34 `03 00 …` (Routine 3, +0x16 = 9),
  @0x14A5C `0d 00 00 00 00 10 00 10 00 00 0a 00 …` (Routine 13, accel (0,10,0) @0x14A64, +0x16 = 0x26); 40% @0x275C/@0x2784/@0x27AC
  dieselben Zeilen, Aufschlag-Anim 0x0A. Routinen-Tabelle @0x80071d40: [3] 0x80017334, [10] 0x800176b0, [13] 0x80017990.
  Routine 13 selbst disassembliert: Weltlage +0x28/+0x2a/+0x2c, `jal 0x8001c6e8` (room_coll) @0x800179c4, `slt` @0x800179d8,
  `sb v0,108(v1)` @0x800179f8, `sw v0,56(v1)` @0x80017a0c, `sb v0,110(a1)` @0x80017a28. **Stimmt mit W.**
* Port-Ursache selbst nachgesehen: `re15_esp.c:470` `f->floor_y = y;` in `esp_fx_spawn_rows_one`; `esp_fx_dispatch` (ab Zeile 674)
  hat Fälle 0, 3, 4, 5, 8..11, 15..18, 22, 23, 30, 31, 38, 41, 42, aber **keinen Fall 13** und endet in `default: break;` (Zeile 1151).
  **Stimmt mit W.**
* Zensus `nb1/r13_zensus.py` selbst laufen lassen: Repo 28 RDTs (11A0/11A1, 1260/1261, 2000..20B1), 40% 6 RDTs
  (2000, 2001, 2030, 2070, 20B0, 20B1), jeweils Effekt 6 Sub 0. **Stimmt mit W.** ROOM20B0/20B1 des Repo-Baums enthalten keinen
  `3a 00 06 00 00 00 00 10` im Skript, dort fällt also kein Tropfen.

### 5.2 Wer startet ein Unterprogramm? (über W hinaus geprüft)

W schreibt: "Von sich aus startet die EXE nur sub00 und jedes Bild sub01." Das habe ich an allen Aufrufern von `FUN_8003ee3c`
(Thread-Start, 9 XREFs in `ghidra1_V2.txt`) nachgeprüft:

| Aufrufer | Argumente | Bedeutung |
|---|---|---|
| @0x8003efb0 (FUN_8003ef6c, Raum-Init) | `0x800b3f70` := RDT+0x40 (main-Tabelle), a0 0, a1 0 | main00 |
| @0x8003efd4 (ebenda) | `0x800b3f70` := RDT+0x44 (sub-Tabelle), a0 1, a1 0 | sub00, danach einmal `jal 0x8003f0a0` (Dispatcher) |
| @0x8003f080 (FUN_8003f038, aus der Hauptschleife @0x8001cdec) | RDT+0x44, a0 1, a1 1 | sub01 in Slot 1, jedes Bild neu |
| @0x8003f2c0 | pc[1], pc[3], pc += 4 | Opcode 0x04 Evt_exec |
| @0x80043104 (LAB_800430f0 = sce-Tabelle @0x8007469c [3]) | Aot-Daten hw[0], byte[3] | AOT sce 3 (EVENT) |
| @0x8004c734, @0x8004cd18 | a0 0xa (Slot ab 10); vor @0x8004c734 setzt @0x8004c720 `0x800b3f70` := 0x80198000 | eigener Spielmodus (Tabelle @0x80074be0 [3]/[6]) mit `d:/bio2/psx/data/room115u.scd` / `room506u.scd`, nicht das Raum-Skript |
| @0x8004d74c, @0x8004d7b0 | Handler @0x80074c74/@0x80074c78 | Opcodes desselben Fremd-Interpreters |

Im Spiel in ROOM2000 der 40%-Disc kann sub03 also nur über Evt_exec oder ein AOT mit sce 3 starten. Beides steht dort nicht:
kein 0x04, die drei Aot_set sind sce 8 (WATER, Handler @0x8004330c), die Türen sce 0 und 2, kein Aot_reset/Aot_on, das umtypen
könnte. **Die Aussage "auf der 40%-Disc wird die Schleife nie gestartet" ist belegt**, enger als W sie belegt hat.

### 5.3 80%-Video (selbst angesehen und ausgewertet)

* `documentation/Biohazard 1.5 Complete Disc Video 2 of 4 (Resident Evil 1.5) (360p).mp4` (320x240, 30 Bilder/s, 1319 Bilder):
  ROOM2000 Cut 2 von Bild 232 bis 517 (9,5 s), ab Bild 518 Raum 2070. Bildvergleich gegen unseren Cut 2: Video und Port liegen fast
  deckungsgleich (Leiter, Lampe, gelbe Flecken innerhalb weniger Bildpunkte), oben fehlen im Video rund 22 Zeilen (schwarzer Rand).
* Die "weißen Wolken" sind Spritzer an den Füßen watender Zombies: Zombie 1 (hell) Bild ≈ 240..283, Zombie 2 (dunkelblau)
  Bild ≈ 360..430; die Spritzer wandern mit der Figur. Belege `W_belege/wa2_80prozent_232_295.jpg`, `wa2_80prozent_296_517.jpg`.
  **Stimmt mit W und Abnahme 1.**
* Tropfen: `wa2_video_tropfen.py` sucht in den drei Fallstreifen (P3 x 185..225, P5 x 108..135, P7 x 76..102, y 22..175)
  helle Punkte > 30 über dem gleitenden Median (±15 Bilder) und verkettet sie zu Fallwegen (y wächst je Bild um 1..40, x < 6).
  Gefunden: Ketten nur bei Bild 279..283 (Zombie 1 im Kanal), 320..331 (Zombie geht hinter Leon), 518..525 (Schnitt auf 2070).
  Kein Tropfen. Bei Bild 434..458 meldet der P7-Streifen einen hellen Fleck bei (86..91, 64..75), der 25 Bilder lang **steht**:
  ein heller Zombie hinten an der Leiter, kein Fall.
* W wertet das als schwachen Gegenbeleg, weil die in Cut 2 sichtbaren Tropfen höchstens 225 Bilder auseinander starten und der
  Abschnitt 286 Bilder lang ist. Die Rechnung stimmt (Starts t = 180, 360, 540, 765; größte Lücke 765 − 540 = 225 < 286). Schwach
  bleibt der Beleg, weil der Tropfen klein ist (der Port zeigt ihn hängend mit 1 Bildpunkt) und das Video ein DivX-Mitschnitt ist.
  **Ich teile die Wertung.**
* `Version_80%/Resident Evil 1.5 (All Footage).mp4` 4:06..5:04 zeigt andere Kanalräume, 5:05..5:14 denselben Clip wie oben. Einen
  längeren Abschnitt von ROOM2000 Cut 2 gibt es in den Videos nicht.

## 6. Wasserlauf an der Leiter vor und nach dem Ablassen (Spur N)

Weg-Fenster der Spur N in Cut 10, x 72..124, y 146..179 (wie N-Dossier M2 und Abnahme 1), aus der Wechselzahl-Karte (je ≈ 21 s):

| Lauf | `[wasserlauf41]` im debug.log | Fenster: Wechselzahl max / Bildpunkte ≥ 6 | ganzes Bild: Bildpunkte ≥ 6 |
|---|---|---|---|
| Fix, Leon 2000, ohne Bit | ja | 552 / 1458 | 11087 |
| Nutzer-exe, Leon 2000, ohne Bit | ja | 549 / 1458 | 11090 |
| Fix, Elza 2001, ohne Bit | ja | 551 / 1458 | 11081 |
| Nutzer-exe, Elza 2001, ohne Bit | ja | 554 / 1458 | 11081 |
| Fix, Leon, `RE15_SET_FLAG=9:100` (Leiter frei) | nein | **0 / 0** | 5253 |
| Fix, Leon, `9:103` (Flut gelaufen) | nein | **0 / 0** | 5252 |
| Fix, Elza, `9:100` / `9:103` | nein | **0 / 0** | 5257 / 5254 |
| Nutzer-exe, Leon, `9:100` | nein | **0 / 0** | 5254 |
| Fix, 20B0/20B1, alle 32 Cut-Läufe | nein (0 von 32) | | |
| Fix, 2000/2001, alle 32 Cut-Läufe ohne Bit | ja (32 von 32) | | |

* Was mit Bit 100/103 in Cut 10 bleibt, ist ein Fleck bei x 118..172, y 50..169 und drei Aufschlagpunkte bei (140,165),
  (153,162), (165,160). Er ist in Fix und Nutzer-exe gleich (Jaccard 1,00) und liegt an den drei Gitterstrahlen 0x0B, die main00
  unbedingt setzt (Repo @0x0174C/@0x0175C/@0x0176C, ohne If davor; Version_40% @0x01568/@0x01578/@0x01588). Das ist Original.
* Ergebnis: Der Wasserlauf läuft wie vereinbart vor dem Ablassen (`re15_wasserlauf_r41_soll`: Raum 2000/2001 und weder Bank 9
  Bit 100 noch Bit 103) und ist danach aus. Der Fix ändert an ihm nichts.

## 7. Nebenwirkungen

* **Code:** Nur `raumlicht.c` Fall `0x2000` ist weg (§1). Kein anderer Raum kann betroffen sein.
* **20B0-Flut:** §3, Fix und Nutzer-exe gleich bis auf die Streuung, die auch zwei Läufe derselben exe zeigen.
* **Wasser beim Hineinspringen und andere Wasser-Riegel** (ctest aus PowerShell mit vorangestelltem `C:\msys64\mingw64\bin`, eigener Bau):
  * Unit, `-R "^unit_(raumlicht|r41_wasserblink|r34_wasser|r38_wasser|r41_wasser20b0)" -j 3`: **23/23 grün**.
  * Integration, echte exe, `-R "^integration_(r38_wasser|r41_nachtrag30_wasser|r41_nachtrag30_laden|r41_wasser20b0|r41_wasserblink)" -j 2`:
    **11/11 grün** (957 s): `integration_r38_wasser`, `_wasserzombie`, `integration_r41_nachtrag30_wasser_leon/_elza`,
    `_laden_leon/_elza`, `integration_r41_wasser20b0`, `_npc`, `integration_r41_wasserblink_leon/_elza/_gegen`.
  * Darin: Sprung ins Wasser mit Landungs-Spritzer (`integration_r41_wasser20b0`, `_npc`), Waten (`integration_r38_wasser`,
    `_wasserzombie`), der Wasserlauf der Spur N (`integration_r41_nachtrag30_wasser_leon/_elza`, `_laden_leon/_elza`) und die
    drei Riegel dieser Spur.

## 8. Riegel ohne den Bau

* **Unit:** `test_raumlicht.c` mit `raumlicht.c` aus aa88a1b6a (gcc direkt): `FAIL: Kanal 2000/2001 hat in 2552 Bildern Licht`, rc 1.
  Mit dem neuen `raumlicht.c`: `Licht in 0/6000 Bildern -> OK`, rc 0.
* **Integration** `cmake -P tests/integration/test_r41_wasserblink.cmake` mit `RE15_PC_EXE` = Messkopie der Nutzer-exe bzw. des Fixes:

  | TEIL | Nutzer-exe | Fix |
  |---|---|---|
  | leon | **rot** (rc 1): `flacker: 16 von 119 Wechseln > 8 (groesster Sprung 131)`, `gleich: groesster Unterschied 47 bei (171,153)` | grün: 0 von 119, Sprung 0; gleich 0 |
  | elza | **rot** (rc 1): dieselben Zahlen | grün: 0 von 119; gleich 0 |

* Der Riegel ist also echt: Er fällt ohne den Bau und hält mit ihm.

## Hinweise (nicht blockierend)

**H1 — hängender Tropfen auch in Cut 5 und Cut 7 (Doku von W unvollständig).** W nennt in OFFEN 4 und unter "Für den Nutzer" nur
den Fleck in Cut 2 bei (83,0..1) "am oberen Bildrand". Gemessen ist er in ROOM2000 und ROOM2001 in drei Kameras, bei Leon und Elza und
ebenso in der Nutzer-exe:
* Cut 2 (83,1),
* Cut 5 (193,1..2),
* Cut 7 (169,15), dort 15 Bildpunkte unter dem Rand im Bild.

Die Projektion von Punkt 7 bei y -10000 sagt genau diese drei Lagen voraus (§4). Für die Entscheidung des Nutzers in OFFEN 4, vor
allem Weg (C) "bleibt wie jetzt", gehört das dazu. Nachzutragen ist nur die Doku. Am Fix und an der Ursache ändert das nichts, und
der Fleck ist kein Kanalwasser.

**H2 — Beleg "40%-Disc startet die Schleife nie" enger als bei W.** W stützt sich auf die Start-Stellen sub00/sub01 der EXE und den
Durchlauf des SCD. Die volle Aufruferliste von `FUN_8003ee3c` (§5.2) schließt auch AOT sce 3 und den Fremd-Interpreter aus. Die
Aussage von W ist richtig. Eine Berichtigung ist nicht nötig, §5.2 kann als Beleg übernommen werden.

**H3 — Messfalle der Abnahme 1.** Die Cut-Läufe von Abnahme 1 werteten je 11 s aus und erreichten Raumbild F545 nicht. Darum fehlte
dort der Tropfen-Fleck (Cut 2/5/7 "0"). Für künftige Läufe in 2000/2001 sollte das Auswertefenster über F545 reichen.

## Für den Nutzer

* Das Blinken im Kanalwasser ist weg. Gemessen habe ich im echten Fenster in deiner Fenstergröße (960x720) an deiner Marke, je 19 s,
  mit Leon und Elza, außerdem in allen 16 Kameras von 2000/2001 und 20B0/20B1 mit je rund 21 s. Mit deiner exe vom Morgen leuchten
  in 19 s 64 Lichtflecken im Kanal auf, mit dem Fix keiner. Deine exe mit abgeschaltetem Raumlicht zeigt dasselbe Bild wie der Fix.
  Damit ist der ausgedachte Glanzpunkt-Effekt als Ursache belegt.
* Die gelben Flecken im Kanal bleiben, sie stehen still und sind auch im 80%-Video zu sehen.
* Das Wasser an der Leiter läuft wie vereinbart bis zum Ablassen und ist danach aus.
* Eine Sache pulsiert noch, aber nicht im Wasser, und das schon in deiner exe: ein einzelner Bildpunkt an der Decke. Das ist ein
  Deckentropfen, den der Port nicht fallen lässt. Er erscheint nach etwa 18 s im Raum in drei Kameras, in deiner Marken-Kamera oben
  am Rand. Ob die Tropfen fallen sollen, steht bei Spur W unter OFFEN 4 zur Entscheidung. Dort fehlen nur noch die beiden anderen
  Kameras (Hinweis H1).

## Werkzeuge und Belege

* Werkzeuge `W_werkzeug/abnahme2/`: `wa2_lauf.py`, `wa2_batch.py`, `wa2_blitz.py`, `wa2_wechsel.py`, `wa2_folge.py`, `wa2_karten.py`,
  `wa2_vergleich.py`, `wa2_scd.py`, `wa2_tropfen_cuts.py`, `wa2_video_tropfen.py`, Listen `wa2_liste_m.json`, `wa2_liste_k.json`,
  `wa2_liste_v.json`, `wa2_liste_r.json` und `wa2_liste_r2.json` (Wiederholungen für die Streuung).
  Der Ablagepfad im Scratchpad steht fest in `wa2_batch.py`.
* Bilder `W_belege/wa2_*`.
* Messkopien und Mitschnitte sind nach der Abnahme gelöscht.
