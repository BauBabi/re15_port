# Abnahme 3 der Spur G5A (G5-Auftritt im Zug, ROOM5090 Leon / 5091 Elza) - unabhaengig

Zweig `r41/g5auftritt`, Kopf `15b2fd419` (Stand Nachbesserung 3 + Nachtrag 9.10). Pruefer ist nicht der Autor. Massstab laut Nutzer: "nach dem
raus schiessen von Birkin aus dem Zug, sollst du die letzte Stufe GENAUSO machen wie bei Resident Evil 2 ... Grossaufnahme ... zunaechst die
Tentakel ... der zieht ihn dann rein". Belege: `G5_belege/abnahme3/` (Leisten, Messdateien, Werkzeuge `abnahme3/werkzeug/`). Alle Zahlen
unten selbst gemessen; Dossier und Abnahmen 1/2 nur als Hinweis gelesen.

## Urteil: NICHT BESTANDEN

M1 (Masse ohne Aufbaeumen) und M2 (Atmen) der Abnahme 2 sind behoben, im Motor und im echten Fenster, Leon und Elza gleich. Die Szene stimmt
sonst Bild fuer Bild mit der echten RE2-Aufnahme ueberein, der Kampf ist fair und mit der Pistole gewinnbar, die Wandklemme der NB3 wirkt nur im
Birkin-Kampf, alle Tests gruen. Es bleibt ein neuer, sichtbarer Mangel im Kernbild:

**N1 (mittel): die vier Arme werden in [T7] nicht eingezogen.** RE2 setzt beim Wechsel [T6] -> [T7] (Szenenbild 652) alle vier Arme auf
Phase 9 = Einziehen (`addiu v0,zero,9` @0x801015dc, `sb v0,6(a2)` @0x801015e0 / @0x801015ec / @0x801015f8 / @0x80101608 mit a2 = Arm-Slots
`lw a2,0x3c38..0x3c44(s3)`). Der Port (`enemy_ai_boss_g5.c` case 7, Zeile 950) schreibt dort keine Armphase. Folge: in der Grossaufnahme des
Mauls (Schnitt 17, Bild 652..807, rund 5 s) stehen im Port zwei Arme als dunkelrote Stangen vor und ueber der Masse, in RE2 sind sie ab Bild
~660 weg; in Schnitt 5/Cut 12 wachsen sie in RE2 ab [T10] neu aus der Masse, im Port sind sie sofort voll ausgefahren. Nicht in Abnahme 2
gesehen (dort "520/600/700 gleich").

## 1. Bau, Laeufe

- Bau: `bash re15_port/tools/local_build.sh configure` + `build` im Worktree, Baum `re15_port/build`: LOCAL-BUILD-OK, "ninja: no work to do";
  keine Quelle unter engine/platform/include/tests neuer als `re15_pc.exe` (01:11:31, game_step_common.c 01:11:19), also Stand `a53ddd47d`
  (Code) = Kopf `15b2fd419` (danach nur Dossier).
- Laeufe (`werkzeug/lauf.sh`): eigene exe-Kopie `re15_g5abn3_<name>.exe`, kein AUTOSHOT, kein SOFTWARE_RENDER, Ton an, Eingabe bis zur Szene wie
  `integration_r41_birkin_pistole` (Debug-Sprung, Pistole, Zielen + Feuer gehalten).
  - `fd_leon` / `fd_elza`: **RE15_FRAMEDUMP 1540-2900/1** (der fertig komponierte Rueckpuffer jedes Spielbilds vor dem Present, 960x720),
    1361 Bilder je Figur - pixelgenau je Spielbild, unabhaengig von der Last.
  - `vid_leon` / `vid_elza`: gdigrab auf das hwnd der eigenen PID (20 B/s, 145 s) + RE15_AUDIO_CAP_SYNC, nach F2548 keine Eingabe.
  - `steh_leon` / `steh_elza`: Zielen + Feuer gehalten bis zum Tod (Kampf).
  - `sonde_leon` / `sonde_elza`: bis F2300, mit RE15_G5_MORPH_SONDE (Zeichner-Sonde der NB3).
  Protokoll aller Laeufe gleich: `F1558 Vorlauf, Schnitt 16`, `F1613 Armierung nach 55 Vorlaufbildern`, (ausser Sonde) `F2775 Szenenende Bild
  1162: G5 Sub 1 Ph 1, Spieler x 13873, Cut 12`. Szenenbild n = Spielbild F1613 + n (Probe: state.log G5 x bei F1778 = -5126 = RE2 s06, F1926 = -2786 = s09).
- Fenster = Framedump: gdigrab-Bilder an Bild 313/479/700/760 (Zeitachse `port_zeit.py`, Kante 16 -> 17 vorhergesagt 85,04 / gemessen 84,90 s)
  zeigen dasselbe wie die Framedumps (`fenster_gdigrab_gegen_framedump.jpg`).
- RE2-Referenz: Mitschnitt Lauf 1 der Abnahme 1 (640x480, 59,8 B/s). Zeitachse selbst nachgemessen (`werkzeug/re2_kanten3.py`): CD-Standbilder
  67,809..68,261 s, 78,545..79,649, 92,040..92,776, 104,599..104,883 (Abnahme 2: 67,806/78,539/92,030/104,585 - gleich auf 1-2 Videobilder);
  Bild 0 = 63,47 s wie Abnahme 2. RE2-RAM: Savestates s01..s26 der Abnahme 1, **mit eigenem Leser** (`werkzeug/re2_ram.py`, Signatur
  SLUS_007.48[0x800:0x830]): Vertexpuffer 0x80132be0 an s03/05/06/08/09/13/17 je **635/635 Vertices gleich** den Dateien der Abnahme 2;
  Teil-2-Weltlage aus `*(G5+0x198) + 2*172 + 92..100`; RID ROOM7040 Kopf[7] = 0x64, Schnitt 9 @0x184 / 10 @0x1A4 selbst gelesen (Auge/Ziel/H
  wie im Code).

## 2. M1 / M2 nachgeprueft

### 2.1 Gewichte im Spiel (Sonde am Zeichner, Fensterlauf) gegen RE2-RAM - `messung_sonde_gewichte.txt`, `messung_re2_ram_masse.txt`

| Bild (RE2) | RE2-RAM w0 / w1 / Bbox y-max | Port Leon = Elza w0 / w1 / Bbox y-max |
|---|---|---|
| 31 (s03) | 3078 / 0 / 23 | 3078 / 0 / 23 |
| 80 (s04) | 10296 / 0 / 23 | 10296 / 0 / 23 |
| 129 (s05) | 5796 / 566 / -242 | 5796 / 566 / -241 |
| 165 (s06) | -184 / 2895 / -1327 | -184 / 2895 / -1326 |
| 264 (s08) | 9440 / 2865 / -1313 | 9440 / 2865 / -1312 |
| 313 (s09) | -4168 / 2865 / -1313 | -4168 / 2865 / -1312 |
| 479 (s13) | 8000 / 2865 / -1313 | 8000 / 2865 / -1312 |
| 677 (s17) | 3152 / 0 / 23 | 3152 / 0 / 23 |

`[g5sonde] F1558 Ctor ... bank 0 ok -1 pose 0`, `F1613 T0 w1_ok 1 basis -4534` (RE2-RAM @0x80105bb0 selbst gelesen: s01/s02 0, ab s03 -4534). Bbox x/z ebenso auf 1 Einheit
(z.B. s06 x-max 3687 / Port 3688). w0 und w1 bitgleich, die gezeichneten Vertices auf +-1.

### 2.2 Umriss-Probe im Bild (`werkzeug/umriss3.py`, `umriss_re2_leon_elza_bild165/313/479/677.jpg`, `messung_umriss.txt`)

Masse Mesh 2 aus dem RE2-RAM (gemorpht, gruen) und die Grundform (magenta) mit der RE2-Kamera projiziert, auf RE2-Bild, Leon- und Elza-Framedump.
Gemessen wird die Helligkeit in den Flaechen, die nur die gemorphte Masse bzw. nur die Grundform bedeckt: wo RE2 Masse zeigt, muss auch der
Port Masse zeigen, wo RE2 Hintergrund zeigt, Hintergrund.

| Bild | Flaeche "nur gemorpht" L: RE2 / Leon / Elza | Flaeche "nur Grundform" L: RE2 / Leon / Elza | Befund |
|---|---|---|---|
| 165 | 25,9 / 27,6 / 27,6 (Masse) | 4,5 / 3,0 / 3,0 (Tuerloch) | Port folgt der gemorphten Form |
| 313 | 26,7 / 24,1 / 24,1 | 7,1 / 6,9 / 6,9 | Stachelreihe auf der gruenen Diagonale wie RE2 |
| 479 | 33,9 / 36,6 / 36,6 | 16,7 / 15,9 / 15,9 | Masse klein und flach wie RE2 |
| 677 (w1 = 0) | 52,4 / 49,1 / 49,1 | 26,6 / 24,6 / 24,6 | Gegenprobe: beide Umrisse fast gleich, passt |

Die Abnahme 2 sah im Port die Grundform (Stachelreihe senkrecht auf dem magenta Rand); jetzt liegt sie in allen drei Bildern mit w1 > 0 auf der
gruenen Kante. **M1 behoben.** Farbe in der Abnahme-2-Flaeche (x 40..140, y 160..230, 320x240): Bild 180 RE2 35/18/11, Port 30/16/10; 240
31/16/9 gegen 27/15/9; 313 14/6/5 gegen 13/7/6; 600 10/5/4 gegen 10/7/5 (vorher 44/20/12 und 45/21/12) - gleich.

**M2 behoben:** w0 an allen acht RE2-Bildern bitgleich (Tabelle 2.1), Vorkampf-Puls Phase/Tempo laut Riegel ebenso. Selbst geprueft:
Sprungtabelle `sltiu v0,v1,8` @0x80104518, Ph 0 `jal 0x80015fe8` @0x80104544 / `andi 0x1f` / `addiu 0x1e` / Tempo 6 @0x80104558, 162 `slti 0xa2`
@0x801045a8, 8501 `slti 0x2135` @0x801045f8, 192 `slti 0xc0` @0x80104658, -1048 `slti -0x418` @0x801046a8, `sltiu 0x7ff` @0x801046f4, Welle
`lh v0,0x2ad4(at)` @0x801047ec, *24 @0x801047f4-fc, 8000 @0x80104800, 110 -> Ph 6 @0x80104824-2c; Wellentabelle Datei 0x5ad4 (110 x s16)
**gleich** `k_welle[110]` im Port. Ctor-Nachholung: `sw zero,0x14c(s0)` @0x80100590 (`4c 01 00 ae`), `jal 0x8002959c` @0x801005a8
(`67 a5 00 0c`), `lw v0,0x30(a3)` @0x801011f4 (`30 00 e2 8c`) - wie zitiert.

## 3. Szene Bild fuer Bild gegen RE2 (Leisten RE2 echt | Leon | Elza, `vergleich_a3_1..6_*.jpg`)

| Bild | RE2 echt | Port Leon / Elza | Bewertung |
|---|---|---|---|
| -50..5 (Vorlauf) | Tuerloch: alle ~10 Bilder zieht eine Tunnellampe vorbei, Truemmer fallen (`m4_tuerloch_vorlauf_bild-50_bis_5.jpg`) | Tuerloch schwarz | M4, s. 5 |
| -40 / 0 | leere Tuer, Figur steht, Blick zur Tuer | gleich | gleich |
| 12 / 30 | Leon laeuft ab Bild 1, bei 30 aus dem Bild | laeuft ab 12, bei 30 ist ein Bein am rechten Rand | bekannt (Abn. 2: PORT-WAHL gering) |
| 45 / 100 | Arme an Rahmen, Decke, Boden | deckungsgleich | gleich |
| 180 / 240 | Masse hinter dem linken Pfosten, Stachelreihe schraeg | gleich (Pfosten vor der Masse) | gleich (M1 behoben) |
| 265 / 271 / 360 | kein Sprung, Masse aufgebaeumt | gleich | gleich |
| 305..330 | Arm vor dem Kopf zieht ein, bei 313 weg (RE2-RAM s09: Arme 0/2/3 im Einzug, Streckung 3264; Arm 1 nie ausgefahren) | bei 313/320 noch ein kurzer Armstummel rechts vom Kopf, bei 330 weg (`n2_armstummel_bild290_bis_360.jpg`) | N2, gering |
| 440 / 450 / 472 / 479 | Figur rechts vorn von hinten, Speere, Masse klein | gleich | gleich |
| 520 / 600 / 640 / 650 | Masse kriecht heran, zwei Arme ueber ihr | gleich | gleich |
| **655..807** | **Arme ziehen ab Bild 652 ein, ab ~660 nur Masse und Maul** | **zwei Arme (senkrecht ueber dem Kopf links, schraeg rechts oben, dazu links am Rand) bleiben bis 807 stehen** (`n1_arme_oben_bild640_bis_800.jpg`, `vergleich_a3_4/5`) | **N1** |
| 820 / 1000 / 1100 | Schnitt 5: grauer RE2-Wagen; bei 820 keine Arme sichtbar, danach wachsen sie aus | Cut 12 (RE1.5); bei 820 Arme schon voll draussen | PORT-WAHL Kamera (bekannt); Arme = Folge von N1 |
| 1162..1200 | Schnitt 6, Kampf | Cut 12, Figur bei 13873, kein Sprung | PORT-WAHL (bekannt) |

- Spruenge (`werkzeug/spruenge.py`, alle 1361 Framedump-Bilder, 160x120): Median 1,00, 99-%-Quantil 6,8. Ueber 7 nur die Schnittkanten
  F1559 (Rueckstoss -> 16, 38,7), F2051 (16 -> 17, 15,7), F2421 (17 -> Cut 12, 30,0) und das Feuer der Rueckstoss-Szene davor (F1542..F1557,
  7..10,7). Szenenende F2775 und Uebergang in den Kampf ohne Sprung. Elza gleich (`messung_spruenge.txt`).
- Ton (`werkzeug/xa_ton.py` der Abnahme 2 auf meinen bildgenauen Mitschnitten): Xa 0x0B bei Takt 2171 (Korrelation 0,84), 0x0C bei 2477 (0,66),
  Abstand **306 = 438 - 132** wie RE2; Leon und Elza gleich (`messung_xa.txt`).
- Laenge: Port 55 + 1162 Bilder (Vorlauf F1558, Ende F2775) wie Abnahme 2.
- Licht/Farbe der Masse: s. 2.2, gleich.
- Uebergang Rueckstoss -> Schnitt 16: harter Schnitt F1558/F1559 (`uebergang_rueckstoss_F1546_1561.jpg`), wie Abnahme 2.

### 3.1 N1 im Einzelnen (`messung_arme_t7.txt`)

- RE2 `CDEMD0_EM36_ai0.BIN` @0x801015b8-0x80101608 (Datei-Offset = Adresse - 0x80100000), selbst disassembliert: Gewicht 1 = 0 (`sh zero,0x10(v0)`
  @0x801015c8), Clip-Wort 0x1F0002 (`sw v1,0x14c(s0)` @0x801015d0), eigene Phase 8 (`sb v0,6(s0)` @0x801015d4), dann **`addiu v0,zero,9`
  @0x801015dc und `sb v0,6(a2)` @0x801015e0 / @0x801015ec / @0x801015f8 / @0x80101608** fuer a2 = `lw a2,0x3c38/0x3c3c/0x3c40/0x3c44(s3)` = die
  vier Arm-Slots (dieselben Slots, die der Port in [T5] fuer Phase 7 zitiert, @0x80101500-2c), danach `j 0x8010e734` (= 0x80101734, SE 10).
- RE2 `CDEMD0_EM37_ai0.BIN`: sub9-Phasentabelle Datei 0x74 (relokiert +0xD000), Eintrag 9 = 0x80102690 = Einziehen: Schritt `jal 0x8002959c`
  a3 0x40, Streckung -= Zaehler, Zaehler += 4, rot_z += 8, unter 0 -> Streckung 0, Phase 10 (`addiu v0,zero,0xa` @0x801026e4), SE 0.
  Der Port hat genau diese Phase (`enemy_ai_tentakel_g5.c` 1206, "@0x80102690 einziehen"), es setzt sie nur niemand.
- RE2-RAM: s16 (G5 [T6] Clip 4 Bild 158) alle vier Arme Wort 0x00070901 (sub 9 Phase 7); **s17 (Bild 677, G5 [T8] Clip 2 Bild 27) alle vier
  0x000a0901 = Phase 10 (eingezogen)**, ebenso s18, s19 und s20 (Schnitt 5, G5 [T10] Zeitgeber 12, also vor dem ersten 0xA01 bei t = 20).
- Port: Schreiber der Armphase in enemy_ai_boss_g5.c nur [T1] (1, 6), [T4] (8), [T5]-Ende (7) - in [T7] keiner. Sub9 Phase 7 laeuft bis Zaehler
  180 (Bild 652) und geht in Phase 8 (leer, Arme bleiben gestreckt). [T10] schickt 0xA01: sub10 Phase 0 setzt die Streckung nicht zurueck, Phase
  1/2 springen bei 4096 sofort durch - die Arme sind in Cut 12 sofort voll statt in ~64 Bildern auszuwachsen.
- Bild: Rotanteil ueber der Masse (x 180..560, y 40..170 im 640x480-Bild, `werkzeug/arme_oben.py`): RE2 Bild 560..650 3,4..4,4 %, **ab 660
  1,6..2,8 %**; Port Leon = Elza durchgehend 2,4..3,6 % ohne Abfall. Die Kante in RE2 liegt genau bei [T7] = Bild 652.
- Wirkung: ~155 Bilder (5,2 s) im Bild "Maul fuellt das Bild" plus ~60 Bilder in Cut 12. Behebung liegt auf der Hand (vier Phasen-9-Schreiber
  im [T7]-Zweig), ist aber Sache des Autors.

### 3.2 N2 (gering)

In [T4] ziehen die Arme ein (t = 30 `addiu v0,zero,0x1e` / `bne` @0x801013a8-ac, `sb v0,6(a2)` mit v0 = 8 @0x801013bc / @0x801013c8 /
@0x801013d4 / @0x801013e4, selbst gelesen; der Port-Kommentar nennt @0x801013e0 statt @0x801013e4 = Verzoegerungsplatz des `jal` @0x801013e0). RE2-RAM (Streckung = `*(Arm+0x198)+0x8c`, die Groesse,
die der Einzug @0x80101fbc-c8 um 0x40 je Bild senkt): s08 (Bild 264) Arme 0/2/3 Streckung 4096, Arm 1 0; **s09 (Bild 313) Arme 0/2/3 Phase 8
Streckung 3264 (= 13 Bilder Einzug seit Bild ~300), Arm 1 Phase 9 Streckung 0 (in s03..s10 nie ausgefahren)**; s10 alle Phase 9, 0. RE2-Bild
313: vor dem Kopf kein Arm mehr. Port Bild 313 und 320: ein kurzer Armstummel rechts vom Kopf, bei 330 weg - rund 10-15 Bilder spaeter als
RE2. Port-Arm 1 (Slot 4) steht dabei bei (629,-24620) mit Clip 14 wie RE2-Arm 1 (644,-24620, Clip 14); welcher Port-Arm den Stummel zeichnet und
mit welcher Streckung, misst kein vorhandener Haken - Ursache nicht isoliert (der Einzug selbst @0x80101fa8-2020 ist im Port gleich: -64 je
Bild, Gier +-16, unter 2048 -> 0). Kurz und klein im Bild.

### 3.3 Nebenbefund Spielerlage Schnitt 17 (gering)

RE2-RAM s12..s19: Leon (6565 -> 16038, z -23627), s13 x 7715. Port Bild 479..776: x 7758 .. 16081 (+43 durchgehend), z -23803 ab F2060 (RE2
-23627, 176 weiter suedlich; Pos_set beider -23570). Im Bild nicht zu unterscheiden (Leisten 450/472/479); kein Mangel.

## 4. Kampf

### 4.1 Fair und gewinnbar (`messung_kampf.txt`, Leon und Elza bitgleich)

| Spieler nach Szenenende (F2775, x 13873) | erster Treffer | weiter | Ende |
|---|---|---|---|
| keine Eingabe | +11,4 s Biss (45 -> 25), G5 x 9546 | +15,8 s (-> 5), +19,7 s (-> 0) | gefressen +20,4 s, G5 600 |
| steht, Zielen + Feuer gehalten | +11,2 s Biss (45 -> 25), G5 HP 375 | +15,5 s (-> 5), +19,4 s (-> 0) | gefressen +20,1 s, G5 600 -> 240 |
| integration_r41_birkin_pistole (12 s zurueck bis x 23113, dann `MA4,W0.2`, eigener ctest-Lauf) | +34,4 s Peitsche (45 -> 38) | - | **G5 tot F3973 (+39,9 s), Spieler HP 38** |

Abnahme 2 hatte "steht + feuert: erster Treffer +18,6 s (Peitsche)"; mit dem RE2-Puls (eine Ziehung @0x80104544 mehr) beisst er jetzt bei +11,2 s.
Beides ist RE2-Verhalten: der Biss greift jeden Spieler innerhalb 6000 vor dem G5 (`sltiu 0x1770` @0x80103e08), bei +11 s steht der G5 bei
~9500, der Spieler 13985 (RE2-Start 14850 laege ebenfalls innerhalb). Ein Spieler hat in jedem Fall >= 11 s, wer zurueckgeht, gewinnt (6.).
**Fair, gewinnbar.**

### 4.2 Wandklemme der NB3 (game_step_common.c) - Nebenwirkungen

Code gelesen (Diff 77479f1f2..15b2fd419):
- Griff-Zweig (Zeile ~2647): Vorlage `s_klemme_ende` nur wenn `re15_birkin_klemme_aktiv() && s_klemme_ende.da`, sonst wie bisher `ox/oz`
  (Griff-Platzierung, RE1.5 FUN_8003b0a4). `re15_birkin_klemme_aktiv` = ein aktiver Aktor Typ 0x30 oder 0x36. `.da` ist im Griff-Zweig gesetzt:
  der Zweig ruft vorher `re15_player_body_and_walls` -> `spieler_klemme` (Zeile 1665/683).
- Nachklemme: die neue Zeile `s_klemme_ende = pl` steht hinter beiden Ausstiegen (`!re15_birkin_klemme_aktiv()` -> return), wirkt also nur im
  Birkin-Kampf.
- RE2 FUN_8003567c selbst disassembliert: `bnez` @0x80035698 / @0x800356a8 ueberspringen nur den Klemmaufruf `jal 0x8004c1bc` @0x800356d0
  (a0 = s0+0x84); `lw v0,0x84(s0)` / `lw v1,0x8c(s0)` / `sh v0,0x118(s0)` / `sh v1,0x11a(s0)` @0x800356ec-f8 laufen immer - wie zitiert.
- Ausserhalb des Birkin-Kampfs (Zombie-, Hunde-, Affen-, Gorilla-, Spiderman-, Kriecher-Griffe) aendert sich also nichts. Im Birkin-Kampf
  (EM30-Griff/Wurf, G5-Griffe) ist die Vorlage jetzt RE2s +0x118.
- Stichproben gruen (6.): unit_r38_griff_* (11 Griffarten), unit_r37_bissarm_klemme_* (4), unit_r35_affen_griff/_wand, unit_r36_kleidung_griff_*,
  unit_dog_grab_anchor, unit_re2_dog_grab_anchor, unit_re2_victim_collapse, unit_r37_elzaems_griff/_opfer27, unit_r30_n_room5080_birkin_wand,
  unit_r17/r18/r20-Birkin-Pins, unit_birkin_opfer, unit_r41_birkin_wand_rein/_griff/_g5/_g5saat; Integration s. 6.
- Fuzz selbst nachgelaufen: g5 (Saat 0x0badf00d) 424 Faelle je Raum, 0 Bildenden in fester Zelle, 0 eingekeilt; **g5saat 200 Saaten x 5 Lagen
  x 150 Bilder je Raum, 38 517 / 38 799 Griff-Bilder, 0 Bildenden in fester Zelle**.

## 5. Offene Restpunkte - Bewertung

| Punkt | Befund | Bewertung fuer diese Runde |
|---|---|---|
| M4 Tunnellichter / Beben / Vorlauf-Ton | RE2: im Tuerloch zieht alle ~10 Bilder eine Tunnellampe vorbei, Truemmer fallen (Vorlauf und erste ~100 Bilder von Schnitt 9, danach deckt die Masse das Loch zu). Mechanismus im Dossier 9.6 belegt (sub09/0B/0C, sub04/05), Port schwarz | vertretbar als offener Punkt (Atmosphaere, nicht der G5-Auftritt selbst); muss offen gefuehrt bleiben, nicht als fertig |
| M5 Musik MAIN11 | RE2 senkt MAIN11 bei Bild 12, Port spielt RE1.5 SUB_17 | vertretbar (PORT-WAHL, Uebertragung auf ein anderes Stueck unbelegt) |
| M3 Kampfstart 13873 / Cut 12 | RVD-belegt, Kampf fair (4.1) | vertretbar |
| M6 PSX | Szenen-Schnitte, Xa, RE2-Licht nur PC | vertretbar (bekannte PSX-Luecke) |
| N1 Arme in [T7] | s. 3.1 | **nicht vertretbar** - Kernbild, RE2-Mechanismus mit Adressen bekannt |
| N2 Armstummel 313/320 | s. 3.2 | gering |

## 6. Tests (re15_port/build, ctest)

| Lauf | Ergebnis |
|---|---|
| Gezielt (-j 6, 46 Tests): unit_r41_g5auftritt, unit_r41_birkin_wand_rein/_g5/_griff/_g5saat, unit_g5_tentakel/_morph/_boss, unit_birkin_ai/_formen/_bahnsteig/_szenen/_opfer, probe_p2_birkin_g5, probe_p3_birkin_rest, unit_r17/r18/r20-Birkin-Pins, unit_r30_birkin_frost, unit_r30_n_room5080_birkin_wand, unit_r38_griff_* (11), unit_r37_bissarm_klemme_* (4), unit_r35_affen_griff/_wand, unit_r36_kleidung_griff_re2/_re15, unit_r37_elzaems_griff/_opfer27, unit_dog_grab_anchor, unit_re2_dog_grab_anchor, unit_re2_victim_collapse, unit_r39_gegner_querung_wand1141, unit_r41_re2gator_wand | **46/46 gruen** (306 s; wand_griff 306 s, wand_g5 69 s, g5saat 51 s) |
| unit_r41_g5auftritt -V | 5090 und 5091: w0/Phase/w1 an 8 RE2-Bildern gleich, Zeichner-Vertices Teil 2 max 2 / Bbox 1 gegen RE2-RAM; prueft keine Armphase ab [T7] |
| unit_r41_birkin_wand_g5 / _g5saat -V | g5: je Raum 424 Faelle, 0 Bildenden in fester Zelle, 0 eingekeilt; g5saat: je Raum 1000 Faelle, 38 517 / 38 799 Griff-Bilder, 0 Bildenden in fester Zelle |
| Integration (-j 3): integration_r41_birkin_pistole, _g5_tod, _g5_westen, r35_zgirl_5090, r37_bissarm_echtlauf, r37_elzaems_elza1181griff, r37_re2zombie, r38_wasserzombie, r41_zombiefresser_tuer2000, pri_masken, r36_pri_schatten | **11/11 gruen** (pistole 443 s: Leon und Elza G5 tot F3973, Spieler HP 38; g5_westen 422 s; g5_tod 294 s) |
| Alle Nicht-Integrationstests (unit/prop/probe/sonstige) ohne 7 Skripttests (-j 6) | **1068/1068 gruen**, 400 s (parallel lief ein fremder ctest -j10 eines anderen Agenten) |
| unit_r38_releaseskript_{trocken,ablauf,fehler_bau,fehler_umfeld,cmd}, unit_r39_titelversion_ablauf, unit_r35_android_bash_mutanten | nicht gelaufen: **Umgebung** - df.exe haengt maschinenweit (Abnahme 2, 8.); laut Auftrag nicht wiederholt |

Kein Test war rot oder musste wiederholt werden. Kein laufender Riegel haelt N1 oder N2 fest.

## 7. Maengel

| Nr | Was | Beleg | Schwere |
|---|---|---|---|
| N1 | [T7] setzt die vier Arme nicht auf Phase 9 (Einziehen): RE2 @0x801015dc-0x80101608; Port case 7 ohne Schreiber. Arme stehen in Schnitt 17 Bild 652..807 vor/ueber der Masse (RE2: eingezogen, RAM s17 Phase 10), in Cut 12 sofort voll statt auszuwachsen | `n1_arme_oben_bild640_bis_800.jpg`, `vergleich_a3_4_bild640_bis_710.jpg`, `vergleich_a3_5_bild730_bis_840.jpg`, `messung_arme_t7.txt` | mittel (5 s im Kernbild + ~2 s Cut 12) |
| N2 | Armstummel rechts vom Kopf Bild 313/320 (RE2 bei 313 eingezogen, RAM s09 Arm 1 Phase 9) | `n2_armstummel_bild290_bis_360.jpg` | gering |
| M4 | Tunnellichter/Truemmer/Beben/Vorlauf-Ton fehlen | `m4_tuerloch_vorlauf_bild-50_bis_5.jpg` | gering-mittel, offen gefuehrt |
| M5, M3, M6 | wie Abnahme 2 | | vertretbar |

Behoben gegen Abnahme 2 (nachgemessen): M1 Masse mit Aufbaeumen (Umriss, Bbox, Gewichte), M2 Vorkampf-Puls bitgleich, Farbe der Masse.
Weiter gleich: Arme Bild 30..650, Pfosten, kein Sprung, Xa 0x0B/0x0C, Speere, Uebergang in den Kampf, Kampf fair.

Kein Riegel prueft N1: `unit_r41_g5auftritt` vergleicht Gewichte, Masse und Arme bis Bild 479 (Speer), nicht die Armphasen ab [T7].

## 8. Belege (`G5_belege/abnahme3/`)

- Umriss: `umriss_re2_leon_elza_bild165.jpg`, `_bild313.jpg`, `_bild479.jpg`, `_bild677.jpg`; `messung_umriss.txt`, `messung_re2_ram_masse.txt`,
  `messung_sonde_gewichte.txt`.
- Szene: `vergleich_a3_1_bild-40_bis_100.jpg`, `vergleich_a3_2_bild180_bis_440.jpg`, `vergleich_a3_3_bild450_bis_700.jpg`,
  `vergleich_a3_4_bild640_bis_710.jpg`, `vergleich_a3_5_bild730_bis_840.jpg`, `vergleich_a3_6_schnitt5_kampf_bild820_bis_1200.jpg`,
  `fenster_gdigrab_gegen_framedump.jpg`, `uebergang_rueckstoss_F1546_1561.jpg`, `m4_tuerloch_vorlauf_bild-50_bis_5.jpg`; `messung_spruenge.txt`,
  `messung_xa.txt`.
- N1/N2: `n1_arme_oben_bild640_bis_800.jpg`, `n2_armstummel_bild290_bis_360.jpg`, `messung_arme_t7.txt`.
- Kampf: `messung_kampf.txt`.
- Werkzeuge `werkzeug/` (Pfade auf den Scratch-Ordner dieser Sitzung): `lauf.sh` (Fensterlauf, FRAMEDUMP, gdigrab, Sonde), `re2_ram.py`
  (RE2-RAM-Leser), `re2_arme.py`, `umriss3.py`, `leiste3.py`, `re2_bilder.py`, `re2_kanten3.py`, `spruenge.py`, `kampf3.py`, `arme_oben.py`.
  `port_zeit.py`, `port_bilder.py`, `xa_ton.py` unveraendert aus `abnahme2/werkzeug/`.
