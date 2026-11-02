# Herkunft Birkin-Modelle (laufendes Dossier)

Stand: fertig, 2026-11-01 ~18:00. Neustart nach Wochenlimit-Abbruch 17:20. Teilstand des abgebrochenen Laufs liegt in
`birkin_modelle/` (abschnitte.txt, blobs.txt, mesh_*.txt, skelett.txt, g2_*.txt, textur_*.txt, rumpf.txt, Blaetter,
Werkzeuge in `birkin_modelle/werkzeuge/`). Die Kernaussagen daraus sind nach dem Neustart selbst nachgeprueft
(`nachpruefung.txt`, `d0_gegen_re2.txt`, Port-Messung `port_messung/`); uebernommene Einzelwerte nennen ihre Teilstand-Datei.
Nebenwirkung der Port-Messung: die exe haengt ihre Zeilen an `re15_port/build/platform/pc/befund.log` an (8 kurze Laeufe
17:31-17:39, Raeume 5080, 5090, 5091, 50E0, 50F1, 50A0, 3070, 3071); nichts anderes ausserhalb dieses Ordners geschrieben.

Kuerzel: D0 = Disc CDEMD0 Typ 0x30, D1 = Disc CDEMD1 Typ 0x30, M0 = Mod CDEMD0 Typ 0x30, M1 = Mod CDEMD1 Typ 0x30
(228460 B), M0x = Mod CDEMD0 Sektor 2012 ohne TOC, D36/M36 = Typ 0x36, R30..R36 = RE2 Retail kind 0x30..0x36 (G1..G5).

## 0. Kurzantwort

- Disc: ein Birkin-Kampf je Figur (3070 Leon, 3071 Elza), Modell D0/D1 (Capcom). 508/509/50E/50F fehlen. Sicher.
- Port, Kampf mit Mutation (5090/5091, davor 5080/5081 mit demselben Modell): vor der Mutation steht **M1** =
  Mod-Modell, CRC 1145F206, `CDEMD1.EMS` Sektor 2015, 228460 B (gemessen). Nicht auf der Disc. Sicher.
  Frueher im Port: 3070/3071, 50E0/50F1 = D0 (Capcom), 50A0 = EM30_HANGAR (Port-Eigenbau aus D0).
- M1 = Skelett, Bewegung und Oberkoerper von Capcoms D0 + Beine, Klaue und Textur von RE2-G2 + neue Teile.
  Also Mod-Eigenbau aus Capcom- und RE2-Daten, nicht frei erfunden, aber auch kein Capcom-Modell. Sicher.

## 1. Kaempfe je Figur

### 1.1 Disc (`Version_40%`)

- Dateiliste: STAGE5 hat 5010/5030/5040/5060/50A0/50C0/5100/5110/5120/5140 (je 0/1), **kein** ROOM5080, 5090, 50E0, 50F0
  (nur `ROOM50D.BSS` ohne RDT). STAGE6 nur 6030/6040. Sicher (`ls Version_40%/PSX/STAGE5`).
- Raumnamen-Tabelle der Disc-EXE (26 Byte je Eintrag, 49 je Stage): 307 `SOUKO` @0x800709e6, 308 `SOUKO ELEVATOR`
  @0x80070a00; 508 @0x800713f4, 509 @0x8007140e, 50E @0x80071490, 50F @0x800714aa sind **leer**. Sicher.
- Sce_em_set-Zensus Typ 0x30..0x37 ueber ALLE 96 RDT der Disc (`werkzeuge/em_zensus2.py`, opcode-exakt + Rohsuche
  in den SCD-Abschnitten, Ergebnis `birkin_modelle/zensus_disc.txt`), genau 6 Funde:

| RDT | Offset | Bytes | Bedeutung |
|---|---|---|---|
| ROOM3070 | 0x3064 | `44 00 30 33 .. 30 75 00 00 30 75` | 0x30 geparkt (30000,30000), Flags (3,1..3)=0 |
| ROOM3070 | 0x30AC | `44 00 30 10 .. cc a2 00 00 50 ce` | 0x30 Kampf (-23860,-12720), Ck(3,3)=1 |
| ROOM3071 | 0x3072 | `44 00 30 21 .. b8 8e 00 00 b6 d1` | 0x30 Elza, vor dem Kampf |
| ROOM3071 | 0x30BE / 0x30D6 | `44 00 30 00 ..` / `44 00 30 10 ..` | 0x30 Elza Kampf, Weiche Ck(3,6) |
| ROOM3080 | 0x78C | `44 02 36 24 .. e0 b1 00 00 70 9a` | 0x36 (Halbkoerper) im Aufzug, mit Ada 0x42 + Marvin 0x40 |

  Folge Disc: **genau ein Birkin-Kampf je Figur** (Leon 3070, Elza 3071, Lagerhalle 307). Danach nur der
  0x36-Auftritt in Leons Aufzug 3080 (kein Kampf, 5 Clips). In keiner STAGE5/6-RDT ein Birkin-Spawn. Sicher.
  **[Gegenprobe] Aber:** Die Disc-`STAGE5.BIN` registriert selbst Typ 0x30 (`sw v0,0x2c6c(at)` @0x8011dd68,
  Handler 0x80116a44). Das Modul 0x80116a44..0x8011bf38 ist das STAGE3-Modul 0x80116230..0x8011b724, nur um
  0x814 verschoben (5437 Woerter: 5323 gleich, 114 nur Sprung/Immediate, 0 echt verschieden). Disc = Mod.
  Capcom hatte Birkin fuer Stufe 5 also im Code vorbereitet, aber in keinem Disc-Raum gesetzt. Ein 0x30-Spawn
  laedt ueber FUN_80022300 die einzige 0x30-TOC-Zeile, also D0/D1. 0x36 ist nur in STAGE3 registriert
  (@0x8011cf50). Sicher.

### 1.2 Mod (`info/Re1.5`)

Zensus (`zensus_mod.txt`), 13 Funde. Tuerziele (`werkzeuge/tuergraph.py`, Door_aot_set pc[22]/pc[23],
`tueren_mod.txt`): 50E/50F haengen an 50D (Labor, STAGE5); 508 nur an 601, 509 an 603/604 (STAGE6-Zuege).

| Figur | Reihenfolge (Mod) | Spawn-Offset | Archiv beim Spawn |
|---|---|---|---|
| Leon | 1. ROOM3070 Lagerhalle | 0x33CE / 0x340C | CDEMD0 (Figurwert) |
| Leon | (ROOM3080 Aufzug, 0x36, kein Kampf) | 0x7D4 | CDEMD0 |
| Leon | 2. ROOM50E0 Labor (README Z. 380 "Added Birkin battle in room 50E") | 0xACE / 0xAEE | CDEMD0 |
| Leon | 3. ROOM5080 U-Bahn | 0x746 | **CDEMD1**: `22 01 00 01` @0x742 davor, `22 01 00 00` @0x75A danach |
| Leon | 4. ROOM5090 Bahnsteig | 0x124A | **CDEMD1**: `22 01 00 01` @0x1246, `22 01 00 00` @0x125E |
| Elza | 1. ROOM3071 Lagerhalle | 0x3434 / 0x347A | **CDEMD0**: `22 01 00 00` @0x3430, zurueck @0x3448 |
| Elza | 2. ROOM50F1 Labor (README Z. 309 "Elza's fight ... changed to room 50F") | 0x9AC / 0x9D4 | **CDEMD0**: `22 01 00 00` @0x9A8 / @0x9D0 |
| Elza | 3. ROOM5081 U-Bahn | 0x742 | CDEMD1 (Figurwert, keine Klammer) |
| Elza | 4. ROOM5091 Bahnsteig | 0x1232 | CDEMD1 (Figurwert) |

Reihenfolge 50E/50F vor 508/509: aus dem Tuergraphen (STAGE5-Labor vor STAGE6-Zug), wahrscheinlich; die Raumwerte
selbst sind sicher. Opcode `22 01 00 v` = Set(Bank 1, Bit 0, v) = Bit 31 von DAT_800aca3c; Lader FUN_80022300
`bltz` @0x80022334: 0 -> CDEMD0 (TOC @0x80072f38), 1 -> CDEMD1 (TOC @0x80073178) (Beleg im Port-Kopf
`re15_port/include/re15_elzaems_r37.h`, dort disassembliert). Sicher.

### 1.3 Port

Port = Mod-Skripte (STAGE5-RDT in `re15_port/shared_assets/PSX` gleiche Funde und Offsets, `zensus_port.txt`) plus
Port-Zusaetze: 50A0/50A1 Hangar (Birkin + Spider-Man, Modell `EMD/EM30_HANGAR.EMD`, `birkin_formen.c:38`), 5090/5091
nach der Mutation `RE2/BAHNSTEIG_G4.EMD` (`birkin_formen.c:44-50`), danach RE2-G5 (Typ 0x36). Reihenfolge Leon im Port:
3070, (3080), 50A0 Hangar, 50E0, 5080, 5090; Elza: 3071, 50A1, 50F1, 5081, 5091 (50A liegt zwischen 407 und 503,
Tuergraph). Sicher fuer die Raeume, wahrscheinlich fuer die Spielreihenfolge.

## 2. Port: Modell im 2. Kampf vor der Mutation

Gemessen an der echten exe `re15_port/build/platform/pc/re15_pc.exe` (Stand 08:21), Karte per
`probe_r36_elza_karte.exe`, CONTINUE, `RE15_EXIT_AT=150#<Raum>`, Protokolle `birkin_modelle/port_messung/r<Raum>/debug.log` (5080: `leon5080/`):

| Raum | debug.log-Zeile | Modell |
|---|---|---|
| 3070 Leon | `[enemy] EM30 loaded: 17 meshes, 16 bones, 21 clips` (keine Archivzeile = CDEMD0-Kette Index 16 = Sektor 1261) | M0 = D0 |
| 3071 Elza | `[elzaems] Typ 0x30 aus EMD/CDEMD0.EMS Sektor 1261 (226404 B)` | M0 = D0 |
| 50A0 Leon | `[enemy] EM30: Ersatzmodell EMD/EM30_HANGAR.EMD (Raum 50A0)` | Port-Eigenbau aus D0 |
| 50E0 Leon | `EM30 loaded` ohne Archivzeile (CDEMD0 Index 16) | M0 = D0 |
| 50F1 Elza | `[elzaems] Typ 0x30 aus EMD/CDEMD0.EMS Sektor 1261 (226404 B)` | M0 = D0 |
| 5080 Leon | `[elzaems] Typ 0x30 aus EMD/CDEMD1.EMS Sektor 2015 (228460 B) crc 1145F206` | **M1** |
| 5090 Leon | dieselbe Zeile, crc 1145F206 | **M1** |
| 5091 Elza | dieselbe Zeile, crc 1145F206 | **M1** |

Lade-Weg: `platform/pc/main.c` `pc_enemy_read_re15_emd` -> `re15_bf_emd_datei` (nur 50A0 -> EM30_HANGAR) ->
`re15_roy_pc_emd` (`platform/pc/src/roy_pc.c`, liest bei Elza-Bit Sektor/Laenge aus der EXE-TOC, Tabelle
`engine/src/gen/ems_toc_r37.inc` Zeile 33 `0x000007df, 0x00037c6c` = Sektor 2015, 228460 B) -> sonst CDEMD0-Kette.
Erst nach der Mutation in 5090 laedt `pc_enemy_load_bahnsteig` die G4-Bank (`re15_bf_bahnsteig_bank_faellig`).
Typ 0x30 ist kein RE2-Typ (`re15_re2_owns_type`, `enemy_ai_re2_dog.c:80`), also kein Hybrid. Sicher.

Hash-Kette: CRC32 1145F206 = md5 819acc8c5ff8ddeb3c66a8235ec94cee = `info/Re1.5/PSX/EMD/CDEMD1.EMS` @0x3EF800
(Sektor 2015, 228460 B) = `re15_port/shared_assets/PSX/EMD/CDEMD1.EMS` gleiche Stelle (beide Archive md5 a3ff26b3).
Mod-EXE-TOC Elza 0x30 @0x80073278 (Datei 0x63a78) `df 07 00 00 6c 7c 03 00`; Disc-EXE dort `f2 04 00 00 64 74 03 00`
(Sektor 1266, 226404 B). Leon-TOC 0x30 @0x80073038: Mod `ed 04 ..` (1261), Disc `ef 04 ..` (1263), beide 226404 B.

**Ergebnis 2:** Im Kampf mit der Mutation (5090/5091, ebenso 5080/5081 davor) steht im Port vor der Mutation **M1**,
das Mod-Modell aus README Z. 92 ("new Birkin model adapted from RE2 to use in Subway area"). Es ist **nicht**
byte-gleich mit der Disc: auf der Disc gibt es keinen Blob mit 228460 B (Inventar `blobs.txt`, beide Disc-Archive
nur die 25 TOC-Blobs). Sicher. Ist mit dem "2. Kampf" dagegen der zweite Raum mit Birkin gemeint, so ist das im
Port Leons 50A0 (EM30_HANGAR, Eigenbau des Ports) bzw. 50E0/50F1 (D0, Capcom); siehe 1.3.

## 3. Vergleich der Fassungen

Quellen: Blob-Inventar `blobs.txt` (Werkzeug `werkzeuge/blobs.py`, EXE-TOC gegen Blobkette), Abschnitte `abschnitte.txt`,
eigene Nachpruefung nach dem Neustart `nachpruefung.txt` (`werkzeuge/nachpruefung.py`) und `d0_gegen_re2.txt`
(`werkzeuge/d0_gegen_re2.py`). RE2 = `re15_port/shared_assets/RE2/CDEMD0.EMS` ueber `gen/re2_ems_toc.inc`
(G2: EMD Sektor 2944, 193124 B; TIM Sektor 2895).

### 3.1 Fassungen, Hash je Abschnitt (md5[:8]; dir 0..8 = d0, EDD1, EMR2, -, -, EDD5, EMR6, MD1, TIM)

| Fassung | Ort | Laenge | EDD1 | EMR2 | EMR6 (Opferbank) | MD1 | TIM |
|---|---|---|---|---|---|---|---|
| D0 | Disc CDEMD0 Sektor 1263 (TOC Leon) | 226404 | c16499a8 | 76d525a2 | 0f564867 | f0a735a7 | a8a42989 |
| D1 | Disc CDEMD1 Sektor 1266 (TOC Elza) | 226404 | c16499a8 | 76d525a2 | **c4675e33** | f0a735a7 | a8a42989 |
| M0 | Mod CDEMD0 Sektor 1261 (TOC Leon) | 226404 | Blob-md5 68eea28c = D0 | | | | |
| M1k | Mod CDEMD1 Sektor 1265, **ohne TOC** | 226404 | Blob-md5 a9b6dee6 = D1 | | | | |
| **M1** | Mod CDEMD1 Sektor 2015 (TOC Elza) | 228460 | **0df2bb9f** | **37c8846a** | c4675e33 (= D1) | **dfc47fab** | **ff05069b** |
| M0x | Mod CDEMD0 Sektor 2012, **ohne TOC** | 228460 | = M1 | = M1 | 0f564867 (= D0) | = M1 | = M1 |
| D36 = M36 | Disc 1374 / Mod 1372 (Typ 0x36) | 128600 | Blob-md5 1cb4b13b in beiden | | | | |

d0 (32f6dfae) und EDD5 (8fc366bc) sind bei D0, D1, M1, M0x gleich. D0 und D1 unterscheiden sich nur in EMR6
(Opfer-Skelettbank Leon/Elza). Die Mod hat die Capcom-Fassungen unveraendert behalten (M0 = D0, M1k = D1, M36 = D36)
und M1/M0x **angehaengt**; die Elza-TOC zeigt seither auf M1, M1k ist verwaist. Sicher.

### 3.2 Kennzahlen

| Fassung | Knochen | Meshes | Tri/Quad | Keyframes x Byte | Clips Bank 0 | TIM |
|---|---|---|---|---|---|---|
| D0/D1/M0/M1k | 16 | 17 | 391/347 | 912 x 84 | 21 | 384x256, 3 CLUT |
| M1/M0x | 16 | 17 | 521/381 | 871 x 84 | 21 | 384x256, 3 CLUT |
| D36/M36 | 16 | 16 | 235/114 | 120 x 84 | 5 | 384x256, 3 CLUT |
| RE2 G1 (0x30) | 16 | 19 | 562/385 | 1005 x 84 | 21 | 384x256 extern |
| RE2 G2 (0x31) | 18 | 18 | 654/311 | 1357 x 96 | 24 | 384x256 extern |
| RE2 G3 (0x33) | 29 | 29 | 696/369 | 883 x 144 | 21 | 384x256 extern |
| RE2 G4 (0x34) | 27 | 27 | 941/486 | 1296 x 136 | 29 | 384x256 extern |
| RE2 G5 (0x36) | 2 | 7 | 358/605 | 1360 x 24 | 11 | 512x256 extern |

### 3.3 Woraus M1 besteht (Knochennamen nach dem Kopf von `re15_port/tools/birkin_formen.py`)

| M1-Teil | Herkunft | Beleg | Grad |
|---|---|---|---|
| Skelett (EMR-Kopf, 16 Rel-Offsets, Eltern) | **Capcom D0** | `nachpruefung.txt` 4: Kopf+Rel D0==M1 True | sicher |
| Keyframes | **Capcom D0**: 871/871 bytegleich in D0 | `nachpruefung.txt` 4 | sicher |
| Clips | D0, ausser Clip 1: 68 -> 27 Bilder, Keyframes = Kopie von Clip 2 | `skelett.txt` ("M1 Clip1 == M1 Clip2 27/27"), Zahlen nachgeprueft | sicher |
| Mesh 8 Brust, 9 Kopf, 10-12 Arm links, 13-14 Arm rechts | **Capcom D0**, Vertices/Normalen/Flaechen/UV bytegleich | `nachpruefung.txt` 2 | sicher |
| Mesh 1-6 Beine und Fuesse | **RE2 G2** Mesh 12-17: gleiche Punkt- und Tri/Quad-Zahl, jeder Punkt mit 1:1-Partner <= 2 Einheiten; Fuesse 3/6 in gleicher Reihenfolge, Beine umsortiert | `nachpruefung.txt` 3 | sicher; Abweichung 1-2 = Neu-Export, wahrscheinlich |
| Mesh 0 Becken/Bauch (86 Punkte) | **neu**: kein Teil in 163 Modellen (Disc, Mod, RE2) naeher als 274 Einheiten | `mesh_suche.txt` | sicher |
| Mesh 15 Klaue rechts (84 Punkte) | **RE2 G2 umgebaut**: Unterarm G2[5] 32/61, Finger G2[6] 7/8, G2[7] 7/8, G2[8] 11/11 Punkte, dazu 25 Punkte ohne Vorlage | `g2_ransac.txt`, `rumpf.txt` | sicher |
| Mesh 16 (bei D0 die Hand mit Rohr, Port `birkin_rohr`) | **Kopie von Mesh 15** (Vertices, Flaechen, UV gleich) | `g2_herleitung.txt` | sicher; Folge: beim Rohr-Tausch zeigt M1 die Klaue, kein Rohr |
| Mesh 7 Bauch | 3-Punkt-Platzhalter, wie in den Mod-Blobs Sektor 1922/1924 | `mesh_suche.txt` | sicher |
| TIM | **neu zusammengesetzt**: CLUTs 0/768 gleich D0, 3/768 gleich G2; Pixelindex 0,5 % = D0, 5,8 % = G2. RGB-16er-Kacheln an derselben Stelle: 105 von 380 mit < 10 Abweichung zu **G2**, 0 zu D0. Kachelsuche mit Verschiebung/Skalierung: 17 + 23 von 98 genutzten 24er-Kacheln aus G2, 4 G1-aehnlich, 54 ohne Vorlage | `nachpruefung.txt` 5, `textur_karte.txt`, `tex_M1_herkunft.png` (gruen G2, gelb G2 skaliert, blau G1, dunkel ohne Treffer) | sicher |
| Bildchen TIM x 301-323, y 7-29 | gelb-blaue Comicfigur (Wolverine-artig), von **keiner** Flaeche benutzt (UV 0/484), in keiner TIM von Disc, Mod oder RE2; Texel (301,7) = `CDEMD1.EMS` @0x40FFF5 | `textur_karte.txt`, `wolverine_zoom.png` | Lage sicher, Deutung wahrscheinlich |

Bild der Teile: `birkin_modelle/M1_herkunft.png` (rot = D0, blau = G2-Beine, gruen = G2-Klaue umgebaut, gelb = neu).

### 3.4 Gegenproben

- D0 gegen RE2 (`d0_gegen_re2.txt`): D0-Fuesse 3 und 6 liegen zu 100 % auf RE2-G1 Mesh 12/15 (<= 2 Einheiten), D0-Beine
  1/4 zu 63 % auf G2 12/15; Becken, Rumpf, Kopf, Arme, Klaue 0 %. D0-TIM an gleicher Stelle: 0/384 Kacheln zu G1 und G2.
  Capcom hat Fuss-/Beingeometrie des 1996er Birkin in RE2 weitergetragen; D0 ist kein RE2-Modell. Sicher.
- D36 (= M36): Mesh 8, 9, 13, 14, 15 bytegleich D0, Rest 3-Punkt-Platzhalter, Skelett-Rel gleich D0, eigene TIM
  (2,3 % Pixel = D0). Capcom-Halbkoerper fuer die Aufzugszene 3080. Sicher.
- M1 gegen RE2 G1, G3, G4, G5: kein Mesh-Treffer (`mesh_vergleich.txt`). Sicher.
- Weitere Mod-Blobs ohne TOC: Sektor 1922/1924 (Alligator, siehe `alligator_40.md`), 2124/2127 = Hund (`blatt_Z0c.png`),
  2206/2209 = Zombie (`blatt_Z0d.png`). Kein weiteres Birkin-Modell. Sicher; Zuordnung zu README Z. 98/97 ("new zombie dog
  model", "new Lab zombie") wahrscheinlich.

## 4. Ansichtsblaetter

Werkzeug `re15_port/tools/emd_ansichtsblatt.py` (Funktionen unveraendert, Aufruf ueber `werkzeuge/blaetter.py`), Bank 0,
Clip 0, Keyframe 0, alle Ansichten im selben Massstab. Dateien `birkin_modelle/blatt_<Fassung>.png`: D0, D1, M1, M0x,
D36, R30, R31, R33, R34, R36, Z0c, Z0d. M0 = D0, M1k = D1, M36 = D36 (md5 gleich, kein eigenes Blatt). Selbst angesehen:

| Fassung | sichtbar |
|---|---|
| D0 (= D1, M0, M1k) | blasse, rosa Haut mit dunkelroten Adern; menschlicher Kopf, blondes Haar, Gesicht; Jeans bis zum Knoechel, dunkle Schuhe; rechter Arm lange, gebogene rote Klinge; Hoehe 3692 |
| M1 (= M0x) | dunkelgrau-rote Haut mit G2-Adern und Augenflecken; Kopf (D0-Geometrie) G2-artig bemalt, kein blondes Haar; Jeans nur bis zum Knie, nackte Unterschenkel und Fuesse wie G2; rechte Hand = G2-Fuenffinger-Klaue; breiteres Becken; Hoehe 3812 |
| D36 (= M36) | nur Oberkoerper, Birkin-Gesicht, ein Arm mit roter Klinge, Bemalung wie D0 |
| R30 G1 | weisser Kittel, menschlicher Kopf, Auge am rechten Oberarm, Rohr, lange Jeans |
| R31 G2 | grau-rot, Auge am Arm, Klaue, kurze Jeans, nackte Fuesse: Vorbild fuer Beine, Klaue und Textur von M1 |
| R33, R34, R36 | G3, G4, G5: keine Teile in M1 |

Unterschied M1 gegen D0 auf einen Blick: gleiche Oberkoerper-Geometrie und gleiche Bewegung, aber Beine, Fuesse, Klaue
und Bemalung von RE2-G2 und ein neues Becken. Herkunftsbild `M1_herkunft.png`.

## 5. Antwort

**Das Modell des U-Bahn-Kampfes (Port 5080/5090 bzw. 5081/5091 vor der Mutation) ist ein Eigenbau der Mod** (README Z. 92,
El Rincon del Lobezno). Es ist zusammengesetzt aus Capcom-Daten der 40%-Disc (Skelett, alle Bewegungen, Oberkoerper,
Kopf, Arme = D0) und RE2-Retail-G2 (Beine, Fuesse, Klaue, grosse Teile der Textur). Neu sind das Becken, Teile der Klaue
und der Textur; das Rohr-Teil ist durch eine Kopie der Klaue ersetzt. Auf der 40%-Disc gibt es dieses Modell nicht und
auch keinen Raum mit einem zweiten Birkin-Kampf. Sicher. [Gegenprobe] Das Birkin-Modul steckt aber auch in der
Disc-STAGE5.BIN (siehe 1.1); ein Auftritt in Stufe 5 war im Code vorbereitet und haette D0/D1 geladen.

Das unveraenderte Capcom-Modell (Lesart a) ist D0: auf der Disc in 3070/3071, im Port in 3070, 3071, 50E0 und 50F1. Die
Mod-Raeume 50E/50F sind selbst Mod-Zusatz (README Z. 309/380). Zu den 80%-Videos siehe `birkin_videos.md` (eigene Spur).

## Aenderungsliste (Gegenprobe 2026-11-01, `gegenprobe.md`)

- 1.1: "In STAGE5/6 kein Birkin" auf RDT-Spawns eingeschraenkt. Neu: Disc-STAGE5.BIN registriert Typ 0x30
  (@0x8011dd68 -> 0x80116a44), Modul = STAGE3-Modul verschoben um 0x814. Ein Stufe-5-Auftritt war vorbereitet.
- 5: Satz zum STAGE5-Modul ergaenzt.
- Alle uebrigen Kernaussagen selbst nachgemessen und bestaetigt (TOC-Bytes, md5 je Abschnitt, Zensus, Set-Klammer,
  Lader-Disasm, Meshes 8-14, Mesh 15 = 16, Beine gegen G2, Keyframes 871/871, Port-Protokolle).
