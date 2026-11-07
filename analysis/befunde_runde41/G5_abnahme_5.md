# Abnahme 5 der Spur G5A (G5-Auftritt im Zug, ROOM5090 Leon / 5091 Elza) - Nachbesserungen 5 und 6, unabhaengig

Gegenstand: Nachbesserung 5 (Tunnellampen, Beben, Beben-Ton) und Nachbesserung 6 (Truemmer/Staub sub05, Vibration sub14, Lampenlicht),
nach `G5_abnahme_4.md` ohne neue Abnahme gemergt (Merge `c9297033c`; Commits `b81dd2996`, `7b1c3bf46`, `4855570fe`, `337751f7b`,
`1aa55d23d`, `c2c5228e2`, `7b4949820`; `git diff --stat 44a9e8b7c 7b4949820 -- re15_port release` = 18 Dateien, +1109/-33).
Geprueft im Integrationsbaum `r41/integration`, Kopf `9867e3913` (Baum sauber). Waehrend der Abnahme kamen von anderer Seite `9e0066c26`
(nur Kommentare in test_r41_g5_tod/_g5_westen.cmake) und `8495d82ff` (Pistolenriegel Pruefung 5b) dazu - nur Testdateien, exe und Engine
unveraendert; die exe-Messungen gelten also auch fuer den neuen Kopf, den geaenderten Pistolenriegel habe ich am neuen Kopf nachgefahren (Abschnitt 7).
Pruefer ist nicht der Autor; Dossier Abschnitt 11/12 nur als Hinweis gelesen, jede Zahl unten selbst gemessen. Belege: `G5_belege/abn5/`
(Leisten, `messung_*.txt`, Werkzeuge `abn5/werkzeug/`).

## Urteil: BESTANDEN

Alle beauftragten Punkte der NB5/NB6 sind da und wirken in der echten exe, fuer Leon und Elza gleich:
Tunnellampen (Lage gegen RE2-RAM 20/20, Fleck im Bild median 0,5/0,2 px neben RE2, 31/31 Polygone mit RE2-Eckfarben), Beben (Folge
3,5,7x5,6..1 und Phase zum Schnitt gleich der RE2-Aufnahme), Beben-Ton (Einsatz Schnitt+3 wie RE2), Truemmer/Staub (Bewegungssignal in
beiden Fallsaeulen 95..98 % der RE2-Menge, Korrelation 0,98/0,99, Pool am RE2-RAM-Bild 13 Staub + 5 Brocken wie RE2), Vibration
(Aktuator-Folge der exe = aus der RE2-Disassembly nachgerechnete Folge, 23/23 Werte). Alle RE2-Adressen/Bytes der Belege selbst
nachdisassembliert bzw. nachgelesen - keine falsche Angabe gefunden. Keine Haenger, kein Absturz, kein roter Riegel (72 verschiedene Riegel, dazu der geaenderte Pistolenriegel am neuen Kopf), kein
Schaden am Spieler durch die Szene. Die drei Kleinoffen sind nicht blockierend (Abschnitt 4); "Vibrationsphase 1 Bild" ist aufgeloest:
die exe verhaelt sich wie RE2. Zwei sichtbare Abweichungen ausserhalb von NB5/NB6 (Rakete aus der Rueckstoss-Szene, Szenenbalken)
sind als Restpunkte gefuehrt (Abschnitt 5).

## 1. Laeufe

- exe: `re15_port/build/platform/pc/re15_pc.exe` des Integrationsbaums (2026-11-06 20:12:34, md5 72bf227f..., keine Quelle juenger,
  enthaelt die NB5/NB6-Messzeilen). Nicht gebaut. Messkopie `scratchpad/testint/g5abn5/bin/re15_pc_g5abn5_quelle.exe`, je Lauf eine eigene
  Kopie `lauf/<name>/exe/re15_pc_g5abn5_<name>.exe`, Assets per `RE15_CD_ROOT` = Integrationsbaum `shared_assets/PSX` (debug.log
  `[asset] cd-root`). Kein AUTOSHOT, kein SOFTWARE_RENDER, Ton an (RE15_AUDIO_CAP_SYNC). Nur eigene PIDs beendet.
- Pistolenweg = Eingabe `integration_r41_birkin_pistole` (34 x MA4, W2.4, D12, 60 x MA4/W0.2; GIVE 3:255,21:250; GR-Messhaken
  RE15_R39_MESS_SPIELER_HP=100 wie der Riegel): Vorlauf F3550, Schnitt 16 sichtbar F3551, Armierung F3605 (55 Vorlaufbilder), Szenenende F4767.
  Raketenweg = Eingabe `integration_r41_g5_tod` (MA9; GIVE 18:200), ohne Haken: Vorlauf F737, Armierung F792, Szenenende F1954.

| Lauf | Weg / Figur | Zweck |
|---|---|---|
| fdp_leon, fdp_elza | Pistole, 5090 / 5091 | RE15_FRAMEDUMP F3540..4060 nativ 320x240 (SDL direct3d), Lampen-/FX-Messzeilen, Ton |
| fd_leon, fd_elza | Rakete | RE15_FRAMEDUMP F730..1240 (SDL waehlte direct3d11), Lampenzeitachse gegen RAM |
| gdb_leon, gdb_elza | Rakete, danach Rueckzug D4,5 s + 40 x Raketen, ohne Haken | Vibration (gdb, s. 3.5), HP-Verlauf bis F4200 |
| x3_*, x3d9_*, n1d11_leon | Pistole, Fenster 3x bzw. Renderer erzwungen (SDL_RENDER_DRIVER) | Lampenhelligkeit je Renderer, RE15_G5A_LAMPE_FARBEN=-6000 |
| fenster_leon, fenster_elza | Pistole, Fenster 640x480, per SetWindowPos (40,0) TOPMOST | echtes Fenster (s. 3.7) |

- RE2-Referenz: Aufnahme `re2_lauf1.mp4` der Abnahmen 1/2 (md5 ff38c94c..., 640x480, 59,817 B/s), Savestates s01..s26 der Abnahme 1,
  `info/re2leon/SLUS_007.48`, `info/re2leon/PL0/RDT/ROOM7040.RDT`. Eigene Werkzeuge (capstone-Disassembler `re2dis5.py`, RAM-Leser
  `re2ram5.py`, VRAM-Leser `vram5b.py`).
- Zeitachse selbst bestimmt (`messung_beben.txt`): Schnitt 9 erscheint im Videobild 61,467 s (Bilddifferenz 23,3), jedes Spielbild = 2
  Videobilder. Port: Schnitt 16 sichtbar im Vorlaufbild 1 (F3551 / F738). "Schnitt+k" = k Spielbilder danach (RE2 Videobild 10 + 2k ab 61,30 s).
  Szenenbild b (Armierung = 0): RE2 = Schnitt+58+b (= Anker der Abnahme 4: 63,47 s minus 2,0 Bilder), Port = Schnitt+54+b.

## 2. RE2-Belege der NB5/NB6 - selbst nachgeprueft (`messung_re2_disasm.txt`, `messung_rdt7040_assets.txt`, `messung_re2_ram.txt`)

| Behauptung (Dossier 11/12) | eigene Pruefung | Ergebnis |
|---|---|---|
| Opcode-Tabelle @0x800a74c8: 0x8A/0x8B/0x8C = 0x80059348/0x80059394/0x800593e4, 0x3A = 0x800565a4, 0x5C = 0x80058164, 0x36 = 0x80056428 | Tabelle gelesen | gleich |
| 0x8A: lhu a0,2 / lhu a1,4 -> jal 0x8003947c, pc+6; 0x8B: lhu a0,2 / lbu a1,1 / lhu a2,4 -> jal 0x80039514, pc+6; 0x8C: lhu a0,4 / lbu a1,2 / lbu a2,3 / lhu a3,6 -> jal 0x800395b8, pc+8 | disassembliert @0x80059348..@0x80059420 | gleich |
| sub14 @0x01C02 `8c 00 64 c8 07 00 00 00`, `8b fa 05 00 02 00`, `8c 00 c8 32 11 00 05 00` = Rampe(7,100,200,0), Konstant(5,250,2), Rampe(17,200,50,5) | Bytes + Handler-Feldlage | gleich; Port vibration_sub14 dieselben Argumente |
| Soll-Folge Aktuator 1 `114 128 250x5 173 .. 50 0` | FUN_800395B8 (Akku von<<7 @0x80039668, Schritt ((bis-von)<<7)/dauer `div` @0x80039618), FUN_80039514, FUN_800396FC (Vorlauf ohne Beitrag @0x8003972c, Wert = Summe>>7 @0x80039770) selbst nachgerechnet | gleich (23 Werte) |
| sub04 @0x0169E: Evt_exec(8,sub05), Gosub sub14 `18 0e`, Sce_shake_on fd/fb, Se_on `36 02 0c 00 01 00`, For 5 {f9}, fa..ff | Bytes `04 08 18 05 18 0e 5c 00 fd 02 5c 00 fb 02 36 02 0c ..` | gleich: 13 Bebenbilder -3,-5,-7x5,-6..-1, Ton im sub04-Bild 3 |
| Sce_shake_on: s3 = (s8)pc[2] `lb s3,2(v1)` @0x800581a4, 240 Zeilen `addiu s5,zero,0xf0` @0x800581b0, Quelle Zeile+pc[2] @0x8002ad10, Rand @0x8002ad14, pc+3 | disassembliert | gleich |
| sub05 @0x016DE..@0x01762: 8 x Sce_espr_on Bank 0x19, Lagen, Sleep 5/5/3/5/5/3 -> Bilder 1/1/6/11/14/19/19/24 | Bytes `3a 00 19 00 00 00 00 10 ..` gelesen | gleich |
| Sce_espr_on @0x800565a4: a0 = pc[2]<<24 / pc[3]<<16 / pc[6..7] = 0x19001000, Matrix pc[4] -> Tafel @0x80011470[0] = 0x80056a78 -> 0x8009DB44, jal 0x8001bf10 @0x80056614, pc+0x10 | disassembliert | gleich |
| Skala RID+2>>7: `lhu v0,0x66(v1)` @0x800779ac, `srl v0,v0,7` @0x800779bc; RDT @0x186 = 29623 -> 231, @0x1A6 = 26684 -> 208 | disassembliert + Bytes | gleich (Port-RID 29623/26684) |
| Op 21 @0x8001f5a4: Staub Bank 3 Sub 9, `lui v0,0x309` @0x8001f5e4, `addiu a0,a0,0xb00` @0x8001f5fc, `jal 0x8001cbe8` @0x8001f600 | disassembliert | gleich |
| RE2-RAM s02: 18 lebende FX-Plaetze = 5 Brocken (vel.y 348/312/252/252/192) + 13 Staub (Skala 6912); s01/s03 leer | eigener Pool-Leser 0x800D8CF0 | gleich (Tabelle des Riegels = RAM) |
| sub09/sub11/sub12/sub10: `04 04 18 0b`, `09 0a 0a 00`, `04 03 18 0c`, Pos_set `32 00 00 00 d0 ee 60 be`, Dir_set `33 00 00 0c 00 0c 00 04`, Member_cmp `3e 00 0b 04 e0 b1`, Calc `26 00 01 10 e8 03`, Parken `32 00 00 83 00 00 00 83` | Bytes gelesen | gleich |
| Obj_model_set @0x01240/@0x01266: Attribut pc[0x0C] = 0, Park-Drehung (0,0x0C00,0) | Bytes | gleich (k_lampe_park) |
| Task-Reihenfolge FUN_800536c4 aufsteigend (`sltiu v0,s2,0xa` @0x80053794, `addiu s1,s1,0x174` @0x8005379c), Evt_exec @0x800538a4 -> FUN_80053138, Platz < 10 direkt (@0x80053154) | disassembliert | gleich -> sub04/sub05/sub11 im Bild Cut_chg+2 |
| Sichtpruefung FUN_8002c820; Ankerzonen RVD @0x45C/@0x470 | 4 Kreuzprodukte Schritt fuer Schritt gegen re15_g5a_lampe_zone, Zonenbytes gelesen | gleich |
| Assets: R704_LAMPE.MD1 = RDT[0x28A4:0x3080], R704_LAMPE.TIM = RDT[0x33844:+34336] (= Dateiende), R704.ESP = RDT[0x35D8:0x36FC] (sha256 518c89ad...), GATOR40A0_19.TIM = RDT[0x18984:0x1B1C4], R704_BEBEN.VBS VBD = VAG 17 (EDT-Satz 12 `00 01 13 20`) | eigene Ausschnitte | alle bytegleich |
| Paket-Gate: R704_LAMPE.MD1/.TIM, R704_BEBEN.VBS, R704.ESP in der Runde-41-Liste mit `[[ -s ]]` + `cmp` | make_package.sh:298-299, `bash -n` OK, alle in git | erfuellt |

## 3. Spiel - eigene exe-Laeufe, Leon und Elza

### 3.1 Beben (`messung_beben.txt`, `abn5_beben_schnitt+0_bis_+7.jpg`, `..._+8_bis_+15.jpg`)

Zeilenverschiebung je Spielbild gegen das Schnittbild, 320x240:

| Schnitt+ | 0 | 1 | 2 | 3..7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| RE2 Aufnahme | 0 | 3 | 5 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| Port Leon / Elza (Pistolen- und Raketenweg) | 0 | 3 | 5 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |

Folge, Dauer und Phase (erstes Beben ein Bild nach dem sichtbaren Schnitt) gleich. Restfehler der Bildanpassung 0,1..0,8 Grauwerte.

### 3.2 Beben-Ton (`messung_ton.txt`)

Messzeile `[g5lampe] F3554 Beben-Ton R704_BEBEN.VBS Satz 0` (Leon, Elza). Pegel je Spielbild: Port Schnitt+0..+2 1383..1977, ab Schnitt+3
5858..8439 ueber ~19 Bilder, dann Abklingen; RE2-Tonspur Schnitt+0..+2 6985..9042, ab Schnitt+3 (61,567 s) 16285..23205 ueber ~19 Bilder.
Einsatz und Huellkurve gleich (Ton-/Bild-Versatz der Aufnahme ungeprueft, +-1 Bild).

### 3.3 Truemmer und Staub (`messung_truemmer.txt`, `abn5_truemmer_schnitt+2_bis_+17.jpg`, `..._+20_bis_+35.jpg`, `abn5_aenderungskarte_...png`)

- Aenderungskarte Schnitt+14..+45: die zwei Fallsaeulen (x 88..132 und x 200..255) und der Staub liegen im Port an denselben Stellen wie in RE2.
- Bewegungssignal je Bild (Pixel mit |Bild - Vorbild| > 14), Schnitt+15..+60:

| Saeule | Summe RE2 | Leon | Elza | Korrelation | letztes Bild mit Signal |
|---|---|---|---|---|---|
| A (x 88..132) | 1618 | 1588 (98 %) | 1588 | 0,99 / 0,99 | 47 / 47 / 47 |
| B (x 200..255) | 5593 | 5301 (95 %) | 5267 | 0,98 / 0,98 | 48 / 49 / 49 |

- exe-Pool (RE15_GATOR_FX_LOG): sub05-Bild 1 (F3552) 2 Brocken, Bild 6 3, Bild 11 4 (Takt wie sub05); sub05-Bild 39 (F3590, = RE2-RAM s02)
  **18 Plaetze = 13 Staub + 5 Brocken wie RE2-RAM**; op_unbekannt 0 in allen Laeufen.
- Leiste: Staubwolke Schnitt+5..+11 oben Mitte, Brocken ab +8 im Gitter rechts oben, +14..+35 fallende Brocken - in RE2, Leon, Elza gleich.

### 3.4 Tunnellampen (`messung_re2_ram.txt`, `messung_lampenfleck.txt`, `abn5_lampe_*.jpg/png`)

- Objekt 3/4 der exe (Messzeile) gegen RE2-RAM an s03..s12 (Szenenbilder 31..441): **20/20 gleich**, Raketen- und Pistolenweg, Leon und Elza;
  Parken ab Szenenbild 438 (Schnitt 17), gezeichnet nur in Schnitt 16.
- Fleck im Tuerloch (Grauwert > 120) je Szenenbild -54..110: in 66 Bildern bei RE2 und Port, |dx| Median 0,5 px, |dy| 0,2 px (90 %: 1,1/1,1);
  Abweichungen nur dort, wo die Figur vor dem Loch laeuft (RE2 b -34..-28, Port b -27..-25, Elza b -23..-12) und in den Bebenbildern
  (Beben schnittgleich, Lampe szenenbildgleich, s. 4.3).
- Pakete: RE2-RAM s04 Objekt 4 Puffer 0 (eigener Leser) gegen Port RE15_G5A_LAMPE_FARBEN=-6000: **31 gezeichnete Polygone, 31 gleich in Lage
  und Eckfarben** (Leon, Elza); nicht gezeichnet T05/T17 (hinter der Kamera), T00/T09/T13 (x -14..14, Farbe 36), Q02 (2x3 px, Farbe 36).
- Helligkeit gegen RE2-VRAM s04: s. 4.1.

### 3.5 Vibration (`messung_vibration.txt`)

Es gibt keine Log-Zeile in pad_rumble_tick und kein Pad an der Maschine. Gemessen deshalb mit gdb an der eigenen exe-Kopie: Haltepunkte auf
re15_rumble_ramp/_large/_small (Argumente), danach 40 Bilder auf pad_rumble_tick+0x20 (Ausgaben a0/a1 nach re15_rumble_tick).
- F739 (Vorlaufbild 2 = sub04-Bild 1): RAMP(7,100,200,0), LARGE(5,250,2), RAMP(17,200,50,5) - Leon und Elza.
- Aktuator 1 ab F740: `114 128 250 250 250 250 250 173 164 155 147 138 129 120 111 102 94 85 76 67 58 50 0` = Soll aus der Disassembly;
  Aktuator 0 durchgehend 0. Mit Pad gingen diese Werte an SDL_GameControllerRumble (x257 / an-aus); den spuerbaren Ausgang kann nur ein Spieler mit Pad pruefen.

### 3.6 Haenger, HP, Kampf (`messung_hp_kampf.txt`)

- Kein Lauf haengt: alle 13 Endlaeufe enden am EXIT_AT mit rc 0 (zwei eigene Vorlaeufe habe ich selbst beendet: gdb mit Haltepunkt je Bild zu
  langsam, gdigrab-Fehler); das Szenenende Bild 1162 wird erreicht in gdb_leon/gdb_elza (F1954) und im eigenen Riegel-Lauf (F4767).
- Ohne Messhaken (Raketenweg): **Spieler-HP 100 von F0 bis F4200**, also kein Schaden durch Vorlauf, Brocken, Szene; G5 600 -> 400 (F2129)
  -> 200 -> 0 -> -200 tot (F2224), vier Raketen a 200 nach dem Rueckzug; Leon = Elza.
- Pistolenweg (eigener Riegel-Lauf `lauf_c6ee345a`, mit GR-Haken): 23 Bilder mit HP < 100, alle F562..F3398 (Formkampf), 0 im G5-Auftritt und
  danach; G5 tot F5861 bei Spieler-HP 100.

### 3.7 Echtes Fenster (`messung_fenster.txt`, `abn5_echtes_fenster_printwindow_leon_elza.jpg`)

gdigrab scheitert in dieser Sitzung (hwnd: "Failed to capture image (error 8)", Desktop: "error 5"). Statt dessen PrintWindow
(PW_CLIENTONLY | PW_RENDERFULLCONTENT) nur auf das hwnd der eigenen exe-Kopie (640x480, ~2 Bilder/s): alle 21 Fensterbilder im Szenenfenster
(Leon F3564..F3696, Elza F3555..F3697) sind **pixelgleich** (mittlere |d| 0,00) zum Framedump desselben Bildes - Lampe, Brocken, Figur.

## 4. Die drei Kleinoffen des Dossiers - Einstufung

### 4.1 Lampe im 3x-Fenster 3..7 % dunkler - nicht blockierend, gering (`messung_lampe_helligkeit.txt`)

| Lampe x -6000 gegen RE2-VRAM s04, Fenster x 70..99 / y 115..126 | Rohsumme | ueber 13,3 | ueber 20 | ueber 40 |
|---|---|---|---|---|
| nativ, direct3d (von SDL gewaehlt) und direct3d11 (erzwungen) | 1,012 | 1,01 | 0,97 | 0,98 |
| 3x, direct3d (erzwungen), Rechnung NB6 | 0,991 | 0,97 | 0,93 | 0,94 |
| 3x, direct3d11 (von SDL gewaehlt), Rechnung NB6 | 0,969 | 0,93 | 0,88 | 0,88 |

Die Dossierzahlen (3..7 %) gelten fuer direct3d (gleiche Werte); mit direct3d11 ist die Lampe im 3x-Fenster 7..12 % dunkler. Ursache sind die 1 Pixel
hohen Flaechen (Gehaeuse, Lichtband-Kante) beim Hochskalieren; Lage und Eckfarben sind RE2-gleich (3.4). In den zwei Raketenweg-Laeufen, in denen SDL
selbst auf direct3d11 auswich, fehlte zusaetzlich die linke Gehaeusespalte (x 78, 4 Pixel; 0,94/0,90/0,90), deterministisch je Lampenlage; der
Pistolenweg mit erzwungenem direct3d11 hat sie - Ursache nicht aufgeklaert. Wirkung: ein 30x12-px-Element, je Durchgang ~4 Bilder; kein Blocker.

### 4.2 Vibrationsphase 1 Bild - kein Mangel, aufgeloest

RE2-main FUN_8002aee0 (CRT `jal` @0x800784a8) ruft im Bildumlauf zuerst den Pad-Handler `jal 0x80038bbc` @0x8002b2d0 (tickt beide Ringe,
@0x80038da4/@0x80038db4) und danach den Task-Scheduler `jal 0x80031d8c` @0x8002b320; der Spiel-Task ist FUN_80025794 (Zeiger RAM 0x800d76a8 =
0x80025794 in s02, gesetzt von FUN_80031f6c `sw a1,0x76a8(at)` @0x80031f7c) und ruft das Skript `jal 0x80053644` @0x800263b0. Werte, die sub14
im Skriptbild N in die Ringe legt, erreichen den Aktuator also auch in RE2 erst im Bild N+1 - genau wie die exe (sub14 F739, erster Wert F740).
Die "1 Bild" ist nur die eigene Zeitachse des Riegels (Takt nach dem Skript).

### 4.3 Vorlauf 55 statt 59 - nicht blockierend, gering

Bestaetigt: RE2 Schnitt sichtbar -> Armierung 58 Spielbilder (Cut_chg -> Armierung 59), Port 54 (55). Folgen, gemessen: die Lampen sind per
PORT-WAHL G5A_LAMPE_VORSPRUNG 4 szenenbildgleich (20/20), stehen dafuer nach dem Schnitt 3..4 Bilder frueher im Tuerloch (Port Schnitt+1, RE2 ~+4);
Beben, Brocken, Ton, Vibration sind schnittgleich und liegen damit 4 Bilder spaeter vor der Armierung als in RE2. Sichtbar ist nur der
fruehere erste Lampendurchgang (0,13 s). Behebung waere ein RE2-genauer Vorlauf (59), danach VORSPRUNG 0.

## 5. Weitere Feststellungen

| Nr | Was | Beleg | Bewertung |
|---|---|---|---|
| R1 | **Rakete aus der Rueckstoss-Szene:** mit dem Raketenwerfer feuert die Rueckstoss-Szene (re15_bf_pad, R1+Quadrat) weiter; eine noch fliegende Rakete samt Rauchspur kreuzt Schnitt 16 (Schnitt+0) und explodiert im Tuerloch (Schnitt+3..+12), Leon und Elza. RE2 zeigt dort nichts. Nicht NB5/NB6 (Form-/Rueckstoss-Uebergang); verdeckt beim Raketenweg die ersten Brocken. | `abn5_beobachtung_rakete_raketenweg_schnitt+0_bis_+15.jpg`, FX-Log fd_leon F728..F759 | nicht blockierend fuer G5A; Folgepunkt Form-Spur / Nutzerfrage |
| R2 | **Szenenbalken:** RE2 hat 17 Zeilen oben/unten schon vor dem Schnitt; der Port zeigt die RE1.5-Balken (24 Zeilen), die erst mit dem Schnitt einblenden (Schnitt+0 halbtransparent, oben ab +3, unten ab +5 deckend). Im Beben ist dadurch in Schnitt+1/+2 oben ein 3/5 Zeilen schwarzer Rand sichtbar, den in RE2 der Balken verdeckt; unten ein schwacher Rotschimmer (R 12, G/B 0..2) im noch halbtransparenten Balken, Zeilen 216..222, Schnitt+1/+2. PORT-WAHL seit NB0 (Dossier 2.2), nicht NB5/NB6. | `messung_beben.txt` (Baender), `abn5_beben_rand_oben_...jpg`, `abn5_beben_rand_unten_...jpg` | gering, nicht blockierend; Abweichung von RE2 |
| R3 | Riegel `integration_r41_birkin_pistole`: die G5A-Pruefung "Spieler lebt beim G5-Tod" ist mit dem GR-Haken nur noch gegen den Polaritaetsfehler scharf (Gegenpruefung B1 bestaetigt: 0 Bilder HP < 100 ab Szenenbeginn). Der haken-freie HP-Beleg dieser Abnahme ist der Raketenweg (3.6). | 3.6 | Testgestaltung, nicht Spiel; **waehrend der Abnahme behoben** in `8495d82ff` (Pruefung 5b), am neuen Kopf selbst nachgefahren: gruen, Leon und Elza 0 Treffer vom Szenenende F4767 bis zum G5-Tod F5861 |
| R4 | Gegenpruefung H1 nennt audio_pc.c/audio_psx.c unter den NB5/NB6-Dateien - sie gehoeren nicht dazu (`git diff 44a9e8b7c 7b4949820 -- '*audio*'` leer). | - | nur Doku |
| R5 | Messzeile `[gator-fx] ... reg40A0 1` heisst auch in 5090/5091 so (Merker "Satz registriert", Satz ROOM7040). | debug.log | kosmetisch |
| - | PORT-WAHLEN ohne PSX-Adresse, beide mit Messbasis dokumentiert: G5A_LAMPE_VORSPRUNG 4 (RE2 59 aus RAM-Phase + Aufnahme, Port 55 aus exe; selbst nachgemessen), Abtastphase 0,5 der Lampe (RE2-VRAM, 4.1) | 4.1, 4.3 | zulaessig, an Kleinoffen 1/3 gebunden |

## 6. Maengel und offene Punkte

| Nr | Was | Schwere |
|---|---|---|
| - | **kein blockierender Mangel** | - |
| K1 | Lampe im 3x-Fenster 3..7 % (direct3d) bzw. bis 12 % (direct3d11) dunkler; Raketenweg-Laeufe mit selbst gewaehltem direct3d11 ohne linke Gehaeusespalte | gering |
| K2 | Vibrationsphase | kein Mangel (exe = RE2-Reihenfolge) |
| K3 | Vorlauf 55 statt 59 (erste Lampe 0,13 s frueher nach dem Schnitt) | gering |
| R1 | Rakete aus der Rueckstoss-Szene explodiert in Schnitt 16 | gering fuer G5A, Folgepunkt Form-Spur |
| R2 | RE1.5-Szenenbalken blenden erst mit dem Schnitt ein (RE2: 17 Zeilen, vorher da) | gering |
| R3 | Pistolenriegel prueft Spieler-HP beim G5-Tod nicht scharf | behoben in 8495d82ff (5b), gruen nachgefahren |
| alt | PSX-Bau ohne Lampen/Beben/Brocken; M3/M5/M6, Nebenbefunde 10.6 (Dossier) | unveraendert offen |

## 7. Tests (`messung_tests.txt`; Git-Bash, PATH=/c/msys64/mingw64/bin:$PATH, SDL_ASSERT=always_ignore, --test-dir Integrationsbaum build/)

| Lauf | Ergebnis |
|---|---|
| `-R 'g5|birkin|tentak|granat' -j 3` (32, darin unit_r41_g5auftritt, integration_r41_g5_tod / _g5_westen / _birkin_pistole und vier Granaten-Integrationen) | "100% tests passed, 0 tests failed out of 32", 717,85 s |
| `-R '^unit_r41_g5auftritt$' -V` | "100% tests passed, 0 tests failed out of 1"; NB5/NB6-Zeilen 5090 und 5091 gruen, Quellvergleich gegen ROOM7040.RDT lief (keine "nicht da"-Zeile) |
| Querbezug gator_fx_r41_pc.c / 5090: `-R '^(integration_r41_gatorszene_.*|integration_r41_re2gator|integration_r41_kanister2080|integration_r35_zgirl_5090|integration_r37_elzaems_elza5091|integration_r37_elzaems_leon5080|integration_pri_masken|integration_r36_pri_schatten|unit_r41_kanister2080_fx_.*|unit_r41_re2gator_kanister|unit_r41_restpunkte_gator)$'` (darin die 6 seit den Merges nie gelaufenen gatorszene_tod/_tod_r1/_flucht +_elza) | "100% tests passed, 0 tests failed out of 24", 526,91 s |
| Renderer/FX/Fenster: `-R 'objpoly|rumble|abtast|re2fx|fenster|boot_bg'` | "100% tests passed, 0 tests failed out of 16", 61,30 s |
| am neuen Kopf `8495d82ff`: `-R '^integration_r41_birkin_pistole$' -V` (mit Pruefung 5b) | "100% tests passed, 0 tests failed out of 1", 483,12 s; Leon und Elza: G5 tot F5861, Spieler-HP 100, 5b Treffer 0 in 1095 Bildern, kleinster x-Abstand 10725 |

Kein Test rot, keiner wiederholt (72 verschiedene Riegel; der Pistolenriegel lief einmal am alten und einmal geaendert am neuen Kopf). Parallel liefen fremde Agenten im selben build/ (u.a. ein fremder Pistolen-Lauf 12:11).

## 8. Belege (`G5_belege/abn5/`)

- Leisten RE2 echt | Port Leon | Port Elza (selbst angesehen): `abn5_beben_schnitt+0_bis_+7.jpg`, `abn5_beben_schnitt+8_bis_+15.jpg`,
  `abn5_beben_rand_oben_schnitt-1_bis_+4.jpg`, `abn5_beben_rand_unten_schnitt0_bis_+9.jpg`, `abn5_truemmer_schnitt+2_bis_+17.jpg`,
  `abn5_truemmer_schnitt+20_bis_+35.jpg`, `abn5_aenderungskarte_schnitt+14_bis_+45_re2_leon_elza.png`, `abn5_lampe_bild-44_-41_-4_-1.jpg`,
  `abn5_uebersicht_bild-58_bis_110.jpg`; RE2-VRAM | nativ | 3x: `abn5_lampe_re2vram_nativ_3x.png`; Fenster: `abn5_echtes_fenster_printwindow_leon_elza.jpg`;
  Beobachtungen: `abn5_beobachtung_rakete_raketenweg_schnitt+0_bis_+15.jpg`, `abn5_lampe_raketenweg_gegen_pistolenweg_diff.png`.
- Messungen: `messung_re2_disasm.txt`, `messung_rdt7040_assets.txt`, `messung_re2_ram.txt` (+ `re2_lampe_s04_obj4.json`), `messung_vibration.txt`,
  `messung_beben.txt`, `messung_ton.txt`, `messung_truemmer.txt`, `messung_lampenfleck.txt`, `messung_lampe_helligkeit.txt`, `messung_hp_kampf.txt`,
  `messung_fenster.txt`, `messung_tests.txt`.
- Werkzeuge `werkzeug/` (Pfade auf den Scratch-Ordner dieser Sitzung): `lauf5.sh` (Messkopie, gdb, PrintWindow), `re2dis5.py`, `xref5.py`, `re2ram5.py`,
  `vram5b.py`, `pool5.py`, `rdt7040_5.py`, `assets5.py`, `lampen_ram5.py`, `pakete5b.py`, `pakete_vgl5.py`, `re2_frames5.py`, `re2kante5.py`,
  `port_beben5.py`, `balken5.py`, `truemmer_mass5.py`, `lampe_fleck5.py`, `lampe_hell5.py`, `leiste5.py`, `hp5.py`, `printwindow5.py`, `fenster5.py`.

Nichts committet, nichts gebaut, nichts unter pri/, synchro/, Version_80%/ geaendert; nur eigene Prozesse gestartet und beendet.
