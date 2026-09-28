# Runde 30 — Thema E2: Irons Diary, das WELT-PROP auf dem Schreibtisch + Memory-Card-Item

Stand: 2026-09-28, master 437905cb (v0.8.15). Phase ERMITTLUNG — kein Bau, keine Änderung an
`engine/`, `platform/`, `include/`, `shared_assets/`. Schwester-Thema (Leser, FILE-Liste, Töne):
`analysis/befunde_runde30/irons-diary-dokument.md`.

FORTSETZUNG: Ein Vorgänger-Agent wurde am Sitzungslimit abgebrochen. Seine Messdateien
(`build/r30_irons-diary-welt/`), Werkzeuge (`analysis/befunde_runde30/werkzeuge/r30_idw_*`) und die
Sonde wurden übernommen, nachdem die Stichprobe in Abschnitt 2.0 sie bestätigt hat. Ein Dossier gab
es nicht; dies ist die erste Fassung.

---

## Kurzfassung

1. **Das Nutzerbild ist Cut 2 von ROOM1150.** Markenmitten: ROT (152,5 ; 126,5), BLAU
   (140,0 ; 126,5). Auf die Tischplatte (y = −1520, zweifach gemessen) zurückgelegt:
   Dokument **(−23656, −1533, −18291)**, Memory Card **(−23657, −1520, −18649)**, Yaw 3072.
   Kontrollabzug mit der Engine-Matrix: 0,02 px und 0,03 px neben der Marke.
2. **Ohne Gegenmaßnahme sieht man in Cut 2 KEINES der beiden Props** (gemessen im echten
   Renderer: 0 von 1485 bzw. 0 von 972 Pixeln). Über den Ablagen liegen drei Original-Masken der
   Tiefe 87 (ROOM1150.RDT @0x6E8, @0x6F4, @0x714) — die Tischplatte selbst; die Props liegen bei
   Bucket 92. Mit einer Tiefen-Klemme auf Bucket 87 sind sie da (525 / 179 Pixel), und eine Figur
   vor dem Tisch liegt richtig darüber.
3. **Die Memory Card ist Item 0x21, nicht 0x20** (0x20 = Incendiary Capsule). RE1.5 bringt
   Name, Untersuchen-Text, Item-Bild und Icon mit, aber 0 Platzierungen und kein Welt-Modell.
   Vorschlag aus Vorhandenem: das Keycard-Modell des Spiels, unverändert, mit einer Textur, die
   aus dem Item-Bild 0x21 entzerrt ist. Prototyp liegt bei und ist im Renderer gemessen.
4. **Das RE2-Buch lädt unverändert** (274 × 26 × 378, 6 Vierecke, TIM 128×128) und passt im
   Maßstab: das gemalte Klemmbrett unter der roten Marke misst 293 × 395.
5. **Ein ums Prop zentriertes Aufhebe-Rechteck ist kaum oder gar nicht erreichbar.** Richtig ist
   die x-Lage, die das Original auf diesem Tisch selbst benutzt (Telefon-Satz @0x00DA6):
   x = −24000, 1000 breit.
6. **Frei und belegt:** obj_id 5/6, TIM-Slot 9/26, AOT-Slot 7/8, Zone-9-Bit 54/55.
7. **Risiko Helligkeit:** das ausgelieferte Licht lässt in der Nahaufnahme Cut 6 nur rund ein
   Sechstel der Eigenfarbe übrig (ambient 40,40,24). Das ist byte-true und kein Texturfehler.

---

## 1. Symptom / Auftrag

`AUFTRAG.md` Abschnitt E, soweit er die WELT betrifft:

> In Irons Office möchte ich das du hinten auf seinen Tresen ein Dokument hinterlegst […] Das
> Weltmodell was da liegen muss ist wahrscheinlich mesh03_cf9f316d_a.png. Das soll an der Stelle
> sein wie in "Irons item.png" in rot markiert. […] Nach dem auflesen und zumachen, soll es vom
> Schreibtisch verschwinden. Auf "Irons items.png" in blau markiert, soll ein - ich glaube
> iot-item heißt das - der Memory Card liegen, das man aufnehmen kann.

Zu liefern: Lage beider Ablagen in Weltkoordinaten, das Dokument-Prop als Zusatz-Prop ohne
Asset-Patch, das Memory-Card-Item mit allem, was RE1.5 dafür mitbringt, und ein Plan mit allen
Konstanten.

---

## 2. MESSUNG

### 2.0 Stichprobe der übernommenen Teilergebnisse

| Übernommene Aussage | Selbst nachgeschlagen | Ergebnis |
|---|---|---|
| Marken-Mitten rot (152.5,126.5), blau (140.0,126.5) | `irons items.png` über die EXAKTEN Markenfarben gezählt (`build/r30_irons-diary-welt/marken.txt`) | bestätigt, beide Marken sind volle Rechtecke |
| Kameratabelle Cut 2 / Cut 6 | `xxd -s 0xA0 -l 32` und `-s 0x120 -l 32` auf `ROOM1150.RDT` | bestätigt, Bytes unten in 2.2 |
| H = fov >> 7 | PSX.EXE @0x80021e70 und @0x80046128 disassembliert | bestätigt, Instruktionen unten in 3.1 |
| Tischhöhe y = −1520 | unabhängiges zweites Verfahren (Einzelmerkmal, zwei Sehstrahlen) | bestätigt: −1515 … −1523 |
| Item_aot_set: Bit @+18, Prop @+20 | Handler @0x80040644 disassembliert, Satz ROOM5010 @0x41E roh gelesen | bestätigt, 3.4 |
| „Memory Card" = Id 0x21 (nicht 0x20) | Offsettabelle DEBUG.BIN @0x499E, Port `main.c:4218/4254` | bestätigt, 2.6 |

`shared_assets/PSX/STAGE1/ROOM1150.RDT` ist byte-gleich mit dem Auslieferungsstand
`info/Re1.5/PSX/STAGE1/ROOM1150.RDT` (`cmp`, 194 080 B).

### 2.1 Das Nutzerbild: welcher Cut, wo die Marken

Werkzeug: `analysis/befunde_runde30/werkzeuge/r30_idw_marken.py`, Ausgabe `marken.txt`.

`irons items.png` ist 320×240. Differenz gegen jeden der neun Hintergründe von ROOM1150
(mittlere Abweichung je Pixel, Summe über RGB):

| Cut | 0 | 1 | **2** | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|---|
| Abweichung | 123,8 | 124,7 | **7,9** | 125,5 | 142,5 | 105,5 | 132,5 | 125,4 | 181,6 |

Das Bild ist **Cut 2**. (Die Restabweichung 7,9 ist Umkodierung des Screenshots, Median 5.)

Die Marken tragen zwei exakte Farben und sind volle Rechtecke — deshalb Zählung über die Farbe,
kein Schwellwert:

| Marke | Farbe RGB | Pixel | x | y | Größe | Mitte (Pixelmitten) |
|---|---|---|---|---|---|---|
| ROT (Dokument) | (237,28,36) | 169 | 146…158 | 120…132 | 13×13 | **(152,5 ; 126,5)** |
| BLAU (Memory Card) | (0,162,232) | 110 | 135…144 | 121…131 | 10×11 | **(140,0 ; 126,5)** |

### 2.2 Kamera Cut 2 und Cut 6 (RDT @0x60 + 32·Cut)

```
ROOM1150.RDT @0xA0 (Cut 2):
  00 00 3c 68 | b4 b8 ff ff  3c f1 ff ff  3c bb ff ff | fa 87 ff ff  96 03 00 00  f2 b6 ff ff | 6c 06 00 00
  flag=0 fov=0x683C=26684   pos=(-18252,-3780,-17604)   tgt=(-30726,918,-18702)   pri=0x66C
ROOM1150.RDT @0x120 (Cut 6):
  00 00 b2 80 | be a8 ff ff  4e f1 ff ff  12 b8 ff ff | 56 9e ff ff  20 01 00 00  50 b6 ff ff | cc 0b 00 00
  flag=0 fov=0x80B2=32946   pos=(-22338,-3762,-18414)   tgt=(-25002,288,-18864)   pri=0xBCC
```

Sichtmatrix aus der ENGINE (Sonde `probe_r30_irons-diary-welt kamera`, Datei `kamera_1150.txt`) —
also dieselben Q12-Werte, mit denen der Port zeichnet, keine Nachbildung:

```
VIEW 2 R= -359 0 4088 | 1438 3834 126 | -3827 1441 -337   T= 15969 10487 -17172   H= 208
VIEW 6 R= -683 0 4045 | 3372 2274 569 | -2246 3415 -380   T= 14459 23036 -10821   H= 257
```

Der Punkt liegt nur in **Cut 2** und **Cut 6** im Bild (Sonde `projekt`, alle neun Cuts geprüft);
Cut 6 ist die Nahaufnahme des Schreibtischs, auf die das Telefon schaltet (sub06 @0x10EA:
`Cut_chg 6`, `Message_on 1`, `Cut_old`).

### 2.3 Tischhöhe — zwei unabhängige Verfahren

PSX-Up geprüft, nicht angenommen: +Y zeigt nach UNTEN. Beide Kameras stehen bei y ≈ −3770 und
schauen auf Ziele mit y > 0 (nach unten); der Boden ist y = 0, die Platte liegt bei negativem y.

**Verfahren A — Flächenkorrelation** (`r30_idw_tischhoehe.py`, Datei `tischhoehe.txt`): jeder
Bildpunkt der Platte in Cut 2 wird über seinen Sehstrahl auf die Ebene y = h gelegt und von dort
in Cut 6 abgebildet. Stimmt h, zeigen beide Bilder dieselbe Stelle. Kein Schwellwert.

| Ausschnitt in Cut 2 | NCC-Gipfel | NCC | MAD-Tal |
|---|---|---|---|
| ganze Platte x100…212 | h = −1520 | 0,657 | h = −1520 |
| um die Marken x125…170 | h = −1520 | 0,722 | h = −1520 |
| linke Hälfte | h = −1520 | 0,581 | h = −1520 |
| rechte Hälfte | h = −1510 | 0,768 | h = −1510 |

**Verfahren B — Einzelmerkmal** (`r30_idw_tisch_gegenprobe.py`, Datei `tisch_gegenprobe.txt`): der
Helligkeits-Schwerpunkt des gemalten Klemmbretts in Cut 2 und in Cut 6, beide Sehstrahlen
geschnitten. Die Schwelle ist durchgefahren, nicht gewählt:

| Schwelle | Schnittpunkt (x, y, z) | Strahlabstand |
|---|---|---|
| 110 | (−23804, **−1521**, −18294) | 18,0 |
| 120 | (−23807, **−1515**, −18294) | 18,8 |
| 130 | (−23803, **−1517**, −18295) | 21,2 |
| 140 | (−23793, **−1523**, −18292) | 23,1 |

(Schwelle 150 zerfällt: in Cut 6 bleiben 114 von 1078 Pixeln, der Schwerpunkt springt um 10 px.)

**Ergebnis: Platte y = −1520** (Auflösung des Verfahrens A: 10 Einheiten; B streut ±4).
Das Klemmbrett selbst liegt bei (−23804 ± 7, −18294 ± 2) und nimmt in Cut 2 die Pixel
x 146…157, y 122…127 ein — es liegt also VOLLSTÄNDIG in der roten Marke (x 146…158, y 120…132).

### 2.4 Weltkoordinaten der zwei Ablagen — Rechenweg und KONTROLLABZUG

Rechenweg (`r30_idw_geom.py`): Bildpunkt (sx, sy) → Kamerastrahl d = ((sx−160)/H, (sy−120)/H, 1)
→ Welt mit der ECHTEN Inversen der Engine-Matrix (`inv(R)`, nicht die Transponierte) → Schnitt
mit der Ebene y = h.

⛔ Die Transponierte ist hier NICHT die Inverse: die Engine-Matrix ist wegen `SquareRoot0` nicht
exakt orthonormal. Gemessen: der Sonden-Befehl `strahl` (rechnet mit der Transponierten) liefert
für die rote Marke (−23740, −18363), die echte Inverse (−23656, −18291) — 110 Einheiten
Unterschied. Maßgeblich ist, welcher Punkt beim VORWÄRTS-Projizieren mit der Engine wieder auf
der Marke landet (Tabelle).

Ebenen: die Karte liegt flach auf der Platte (y = −1520). Das Buch ist 26 dick und um seinen
Mittelpunkt modelliert (MD1-bbox y −13…+13, Abschnitt 2.7) → Buchmitte y = −1520 − 13 = **−1533**.

| Ablage | Marke (Bild) | Weltpunkt | zurückprojiziert mit der Engine (Sonde `projekt`, Cut 2) | Abstand zur Marke |
|---|---|---|---|---|
| Dokument, **Lage A** = Markenmitte | (152,5 ; 126,5) | **(−23656, −1533, −18291)** | (152,48 ; 126,49) | **0,02 px** |
| Dokument, Lage B = Mitte des gemalten Klemmbretts | — | (−23813, −1533, −18280) | (153,49 ; 124,44) | 2,29 px (liegt IN der Marke) |
| Memory Card = Markenmitte | (140,0 ; 126,5) | **(−23657, −1520, −18649)** | (139,97 ; 126,49) | **0,03 px** |

In Cut 6 landen dieselben Punkte bei (194,01 ; 136,91), (196,51 ; 123,97) und (158,62 ; 132,39).
Kamera-Tiefe vz: Cut 2 = 5895 / 6041 / 5930, Cut 6 = 2569 / 2654 / 2613.

**Yaw.** Der Nutzer hat keinen vorgegeben; er folgt aus dem Raum:

* Der Schreibtisch steht achsparallel (Standlinie x = −22664 über die ganze Tischbreite
  z −19400…−17600, Datei `anlauf.txt`); seine Längsachse ist Welt-Z.
* Yaw-Konvention des Ports (`pc_prop_rot_q12`, reines rot_y): Modell (x,0,z) → Welt
  (c·x + s·z, 0, −s·x + c·z). Die lange Buchkante (Modell-Z, 378) zeigt bei **rot_y = 3072**
  nach Welt −X = vom Betrachter vor dem Tisch weg. Im Rig gemessen: Titelschild und Schließe
  stehen dann für den Spieler vor dem Tisch aufrecht — so wie alle GEMALTEN Gegenstände der
  Platte (Klemmbrett-Schrift, Buch „SPECIALIST").
* Das gemalte Klemmbrett selbst ist um etwa 11° gedreht (`r30_idw_klemmbrett.py`, Hauptachsen
  der auf die Platte gelegten hellen Pixel, Datei `klemmbrett.txt`): rot_y = 2930…2957 je nach
  Schwelle, Mitte (−23815, −18277), helle Fläche 293 × 395. Für Lage B gilt deshalb rot_y = 2944.
  (Das Einzelmerkmal-Verfahren aus 2.3 setzt die Mitte auf (−23804, −18294); die zwei
  Bestimmungen liegen 20 Einheiten auseinander, Lage B nimmt (−23813, −18280).)

Die zwei Maßstäbe (Memory `reai-v2-modellmass-aus-kamera`) sind geprüft: das RE2-Buch ist
274 × 378, das gemalte RE1.5-Klemmbrett 293 × 395 (über Rückprojektion auf die Platte, nicht
über z_view/H) — die Modelle beider Spiele teilen den Maßstab, das Buch braucht KEINE Skalierung.

Python-Kontrollabzug (unbeleuchtet, ohne Masken): `irons-diary-welt/kontrollabzug_python.png`.
Er prüft nur Lage und Größe — was der Port wirklich zeichnet, steht in 2.5.

### 2.5 MESSUNG im echten Renderer: in Cut 2 sind beide Props UNSICHTBAR

Mess-Rig: eine KOPIE der Port-Quellen (`build/r30_irons-diary-welt/rig/`, gepatcht von
`r30_idw_rig_patch.py`), die zwei Props obj_id 5/6 aus Dateien lädt. Das Repo-Spiel ist
unberührt. Lauf: `r30_idw_rig_lauf2.sh`, Bilder über `RE15_FRAMEDUMP` (Readback vor
`SDL_RenderPresent`, beschleunigter Renderer; kein AUTOSHOT, kein SOFTWARE_RENDER).
Auswertung `r30_idw_rig_auswertung.py` gegen einen LEERLAUF desselben Rigs (Props aus) —
nicht gegen das BG-PNG (das weicht nach dem Hochskalieren in 19 791 Pixeln ab und taugt nicht
als Nullbild). Datei `rig_auswertung.txt`.

| Lauf | Cut | Tiefen-Klemme | Dokument sichtbar | Karte sichtbar | Bildmitte Dokument / Karte |
|---|---|---|---|---|---|
| `c2_ohne` | 2 | keine | **0 von 1485 Pixeln** | **0 von 972** | — |
| `c2_ot87` (Lage A) | 2 | Bucket 87 | 525 | 180 | (151,83 ; 126,33) / (139,83 ; 126,33) |
| `c2_B_ot87` (Lage B) | 2 | Bucket 87 | 495 | 180 | (153,33 ; 124,33) / — |
| `c6_ohne` (Lage A) | 6 | keine | 8698 | 3240 | (193,33 ; 137,33) / (158,33 ; 132,83) |
| `c6_B` (Lage B) | 6 | keine | 6147 | 3240 | (197,00 ; 128,67) / — |
| `c6_ot87` | 6 | Bucket 87 | identisch zu `c6_ohne` (0 Pixel Unterschied) | | |

Bilder: `irons-diary-welt/rig_cut2_OHNE_klemme_unsichtbar.png`, `rig_A_cut2_klemme87.png`,
`rig_A_cut6.png`, `rig_B_cut2_klemme87.png`, `rig_B_cut6.png`, `rig_cut2_spieler_vor_tisch.png`.

Abstand Bildmitte ↔ Marke im ECHTEN Renderer (Lage A, mit Klemme): Dokument 0,69 px,
Karte 0,24 px.

**Warum unsichtbar:** über beiden Ablagen liegen in Cut 2 Original-Masken der Tiefe 87
(`r30_idw_masken.py`, Datei `masken_ueber_ablagen.txt`; pri_offset 0x66C, 11 Gruppen, 54 Masken):

```
@0x006E8  38 20 b0 98 57 00 b5 00 18 00 10 00   Bild x 126..149 y 113..128   Tiefe 0x0057 = 87
@0x006F4  50 20 c8 98 57 00 b5 00 18 00 10 00   Bild x 150..173 y 113..128   Tiefe 87
@0x00714  48 30 c0 a8 57 00 b5 00 20 00 18 00   Bild x 142..173 y 129..152   Tiefe 87
```

Tiefe 87 verdeckt Vierecke ab vz ≥ 88·64 = 5632 und Dreiecke ab vz ≥ 88·65536/1023 = 5637,5
(Regel in 3.2). Die Props liegen bei vz 5895…6041, also DAHINTER. Die Maske ist die
Tischplatte selbst — sie soll eine Figur HINTER dem Tisch verdecken und verdeckt deshalb auch
alles, was AUF dem Tisch liegt. Cut 6 hat keine Masken (pri_offset 0xBCC → `ff ff ff ff`).

Mit der Klemme stimmt die Reihenfolge in beiden Richtungen (gemessen, Lauf `c2_ot87_spieler`,
Spieler auf der Standlinie (−22664, −18450), Figur-Tiefe 4328…5852): die Figur VOR dem Tisch
liegt über den Props (Bild `rig_cut2_spieler_vor_tisch.png`). Für eine Figur HINTER dem Tisch
ist es gerechnet, nicht gemessen: Sonde `projekt` gibt für (−24800, −1500, −18400) vz = 6985 →
Bucket 109 > 87, sie läge unter der Maske und damit unter den Props.

### 2.6 Zweiter Messbefund: die Raumbeleuchtung macht die Props dunkel

Sonde `licht` (Engine-Funktionen des Prop-Zeichners: `re15_light_setup_actor` →
`re15_light_ctx_rotate_for_bone` → `re15_light_shade_vertex`), Datei `licht.txt`, und die
Rig-Bilder (Datei `helligkeit.txt`):

| Cut | ambient (RDT-Licht) | Vertexfarbe Deckfläche (Normale −Y) | = Texel × | Dokument im Bild | Textur-Eigenfarbe |
|---|---|---|---|---|---|
| 2 | (110, 90, 84) | (66, 52, 48) | 0,52 / 0,41 / 0,38 | RGB (46, 35, 25) | RGB (95, 92, 70) |
| 6 | (40, 40, 24) | (22, 22, 13) | 0,17 / 0,17 / 0,10 | RGB (18, 17, 8) | RGB (95, 92, 70) |

Rechnung und Bild stimmen überein (95·0,17 = 16 gegen 18; 95·0,52 = 49 gegen 46). In Cut 6 —
der Nahaufnahme, in der man Buch und Karte am besten sähe — bleibt vom Buch **16 %** seiner
Eigenfarbe. Ursache ist das ausgelieferte Licht von Cut 6: ambient (40,40,24), Licht 1 und 2
stehen auf dem Füllwert (2000,2000,2000)/(128,128,128). Der Cut zeigt im Original nie eine
Figur oder ein Objekt; sein Licht wurde offenbar nie eingestellt.

### 2.7 Das Dokument-Prop lädt UNVERÄNDERT

Sonde `re2prop` (Engine-Parser `re15_md1_parse` / `re15_tim_parse`), Datei `re2prop.txt`:

```
MD1 372 B: parse=0 length=276 unknown=0 object_count=2 mesh_count=1
  Mesh 0: tri=0 quad=6 (v=8 n=6)   bbox x -137..137  y -13..13  z -189..189
  alle 6 Vierecke: clut=0x7800 page=0x0080, Normalenlaenge^2 = 16777216 (= 4096^2)
TIM 17440 B: parse=0 bpp=8 clut=1 @VRAM(0,480) Eintraege=512 | Bild 128x128 @VRAM(0,0)
  vom Mesh benutzte Flaeche (v 0..63): 8192 Texel, davon Wert 0x0000 (nicht gezeichnet): 0
```

Maße 274 × 26 × 378, um den Mittelpunkt modelliert. Format 1:1 wie die RE1.5-Raum-Props
(MD1-Kopf, UV-clut 0x7800 / page 0x80, TIM 8bpp + CLUT @(0,480)) — der Rig-Lauf hat die Datei
ohne jede Umwandlung gezeichnet (`[rig] obj 5: MD1 372 B ok=1, TIM 17440 B -> Slot 9`).

### 2.8 Raumzustand ROOM1150/1151 nach dem Hochfahren

Sonde `pool` / `aots` / `bits` (Dateien `pool.txt`, `aots.txt`, `bits.txt`), SCD-Dump
`scd_1150.txt` / `scd_1151.txt`:

| Ressource | belegt | frei |
|---|---|---|
| obj_id / Prop-Pool | 0…3 aus main00 (nOmodel = 4), 4 = Sicherung (Port) → 5 Einträge | **5, 6** (Pool fasst 17) |
| TIM-Slot | `RE15_TIM_SLOT_PROP(0…4)` = 4…8 | **9** (obj 5), **26** (obj 6) — `main.c:156` |
| AOT-Slot | 0…6 (ROOM1150), 0…5 (ROOM1151), 48…63 Kamerazonen | **7, 8** (in beiden Varianten, kein `Aot_reset` darauf) |
| Zone 9 | nach dem Hochfahren 0 Bits gesetzt; der Raum benutzt Bank 9 im SCD gar nicht | siehe 3.6 |

### 2.9 Aufhebe-Rechtecke: gemessene Erreichbarkeit

Sonde `abdeckung` (legt ein Rechteck mit flags 0x31 in einen freien Slot und zählt über ein
50er-Raster gültiger Standorte × 64 Blickrichtungen, wie oft der FORWARD-620-Punkt trifft und
ob ein früherer ACTION-Record denselben Druck abfängt), Datei `abdeckung.txt`; Sonde `druck`
(EIN echter Aktionsdruck über `re15_aot_scan`), Datei `druck.txt`.

| Rechteck (x, z, w, d) | Standorte | Treffer (Standort × Richtung) | verdeckt |
|---|---|---|---|
| Dokument, 1000×1000 um die Prop-Mitte A: (−24156, −18791) | 88 | 541 | 0 |
| Dokument, 1000×1000 um die Prop-Mitte B: (−24313, −18796) | **0** | **0** | 0 |
| Dokument, x wie der Telefon-Record: **(−24000, −18791, 1000, 1000)** | 200 | 1564 | 0 |
| Karte, x wie der Telefon-Record: **(−24000, −19149, 1000, 1000)** | 198 | 1560 | 0 |
| 800×800 um die Prop-Mitte (beide) | 21 | 80 | 0 |

Ein um das Prop ZENTRIERTES Rechteck ist kaum (A) oder gar nicht (B) erreichbar: der Spieler
steht an der Tischkante bei x = −22664, sein Prüfpunkt liegt 620 voraus bei x = −23284, die
Props aber 370…530 dahinter. Echter Druck (Sonde `druck`): Stand (−22664, −18290) Blick −X →
Modal geht auf; Blick +X → nicht; Stand 300 weiter weg → nicht.

### 2.10 Memory Card: was RE1.5 mitbringt — und die Item-Id ist 0x21, nicht 0x20

⛔ **Korrektur zum Auftragstext.** Dort steht „RE1.5-Item 0x20 (Namensliste Index 32)". Die Liste
`analysis/befunde_2026-09-21/belege/re15_item_names.txt` zählt die STRINGS ab 0 („Combat Knife"
= Index 0); die Item-Ids zählen ab 1, weil Id 0 der leere Platz ist. Index 32 der Liste ist
deshalb Id **0x21**. Gegenprobe an drei Nachbarn: `item_aot_zensus.txt` führt 0x22 = First Aid
Spray (Liste Index 33), 0x24 = Green Medicine (35), 0x40 = Fuse (63).

| Was | Beleg | Wert |
|---|---|---|
| Name | DEBUG.BIN Offsettabelle @0x495C + 2·0x21 = **@0x499E** `c2 01` → Blob 0x4A28 + 0x1C2 = **@0x4BEA** (RAM 0x800C4BEA): `29 41 49 4b 4e 55 00 1f 3d 4e 40 07` | „Memory Card" |
| Nachbar 0x20 | Eintrag @0x499C `af 01` → @0x4BD7 | „Incendiary Capsule" |
| Item-Bild des Nachbarn 0x20 | `ITEM/ITPS.ITP` @0x20·0x3000 = Datei 0x60000 | ein Platzhalter (Strichzeichnung „BOSS", japanisch „仮キャラ" = vorläufige Figur), KEINE Karte — `irons-diary-welt/itps_20_platzhalter_x4.png` |
| Untersuchen-Text | Beschreibungsbank @0x800C50DE, Eintrag[0x21] = 0x7B4 → **@0x800C5892** (DEBUG.BIN 0x5892): `25 00 3f 3d 4a 00 4e 41 3f 4b 4e 40 00 49 55 00 4c 4e 4b 43 4e 41 4f 4f 08 53 45 50 44 00 50 44 45 4f 57 01 00` | „I can record my progress / with this." |
| Item-Bild | `ITEM/ITPS.ITP` @0x21·0x3000 = **Datei 0x63000**: `10 00 00 00 09 00 00 00 0c 02 00 00 00 00 e9 01 00 01 01 00`, Bild 112×72 @VRAM(832,256), CLUT @VRAM(0,489) | `irons-diary-welt/itps_21_memory_card_x4.png` |
| Inventar-Icon | `DATA/ITEMALL.PIX` Kachel 0x21 = **Datei 0x9AB0** (40×30, 28 Indizes), CLUT = ST_00 Zeile 0; Port-Zuordnung Kachel = Id (`re15_inv_screen.c:913`) | `irons-diary-welt/itemall_21_icon_x4.png` |
| Eigenschaften | PSX.EXE **@0x80074F34** `fa 00 00 00 88 4c 07 80 01 00 00 00` | Obergrenze 250 (zählbar), Kombinations-Satz = Null-Satz 0x80074C88 |
| Platzierungen | Zensus über 240 RDTs, 1579 SCD-Blöcke, 40 674 Opcodes, 0 desynchron (`zensus.txt`) | **0** `Item_aot_set`, 0 `Sce_key_ck`, 0 `Keep_Item_ck` mit 0x21 |
| Verwendung im Code | `itemverwendung.txt`: FUN_8004dfec (Platz-Suche nach Id) hat 2 Aufrufer, keiner mit 0x21 als Sofortwert; kein `jal` in den Inventarbereich 0x8004DA00…0x8004E400 mit 0x21 in a0…a3 | im Auslieferungsstand **kein Verbraucher gefunden** |
| Verwendung im Port | `platform/pc/main.c:4254-4281`: beim Speichern wird eine Karte verbraucht, WENN eine da ist; gesperrt ist das Speichern ohne Karte nicht | Verbraucher = Speichern |
| Welt-Modell | keines (folgt aus „0 Platzierungen") | siehe 3.9 |

RE2 zum Vergleich (`r30_idw_re2_farbband.py`, Datei `re2_farbband.txt`; 250 RDTs, 2568 Blöcke,
171 davon desynchron — die Zahl ist also eine UNTERGRENZE): das Speicher-Item Ink Ribbon
(Id 0x1E) ist 21-mal platziert, **Menge in allen 21 Fällen 3**, 15-mal mit Welt-Modell.

### 2.11 Sieht der Spieler die Gegenstände, wenn er vor ihnen steht?

Die Props liegen nur in Cut 2 und Cut 6 im Bild. In Cut 1 und Cut 3 steht der Spieler am
Aufhebe-Standort ganz am Bildrand (sx 7…15 bzw. 307…315), die Gegenstände liegen außerhalb
(sx −11 / −25 bzw. 346 / 331; Datei `sicht_cut123.txt`). Entscheidend ist also, welcher Cut dort
aktiv ist. Gemessen mit zwei Sonden-Befehlen:

`reihe` (wo kann der Spieler stehen; Klemmpfad `re15_collision_constrain`, Datei `reihe.txt`):

```
z=-21200 :  x[-26225..-22250]                                     <- suedlich um den Tisch herum
z=-20000 :  x[-26225..-25900]  x[-22750..-22250]
z=-18400 :  x[-26225..-25975]  x[-22675..-22250]                  <- Gang VOR dem Tisch, 425 breit
z=-17000 :  x[-26225..-25900]  x[-22750..-22250]
z=-15800 :  x[-26225..-22250]                                     <- noerdlich um den Tisch herum
```

Vor dem Schreibtisch bleibt ein Gang x −22675…−22250. Von Osten ist er versperrt (x −22225…−17425
ist über die ganze Tischlänge nicht begehbar; der Hebetisch, Prop 0, steht bei x = −20700);
hinein kommt man nur von Norden oder von Süden.

`zonen` (RVD-Kamerazonen, Datei `zonen_standplatz.txt`): genau an diesen zwei Eingängen liegen
die Umschaltzonen nach Cut 2 —

```
ZONE  6  von Cut 1 nach Cut 2   quad (-26808,-16400)(-26808,-15388)(-21608,-16400)(-21508,-17500)
ZONE 12  von Cut 3 nach Cut 2   quad (-26899,-21801)(-26899,-20400)(-21399,-19400)(-21300,-20200)
```

— bei x = −22500 decken sie z −17294…−16226 (Nord) und z −20590…−19630 (Süd). Wer den Gang
betritt, läuft durch eine der beiden und steht danach in **Cut 2**, dem Cut des Nutzerbilds.
Das ist aus Zonen und Kollision ABGELEITET; ein Lauf mit Tasteneingabe durch den Gang wurde
nicht gefahren (der Teleport `RE15_PLAYER_POS` löst keine Zone aus — gemessen: Lauf
`auto_tisch` blieb auf Cut 0).

Hinter dem Tisch gibt es einen zweiten Streifen (x −26225…−25975). Von dort ist keines der
beiden Rechtecke erreichbar: der Prüfpunkt liegt bei x = −25355, die Rechtecke beginnen bei
−24000.

---

## 3. ORIGINAL-MECHANISMUS

### 3.1 Projektion: Bildmitte (160,120), H = fov >> 7

```
PSX.EXE FUN_800460b8
  800460f4: ori  a0,zero,0xa0
  800460f8: jal  0x80066d60           ; SetGeomOffset
  800460fc: ori  a1,zero,0x78         ;   (160, 120)
  80046120: lhu  a0,2(v0)             ; v0 = Kameratabelle + cut*32  -> fov
  80046124: jal  0x80066c30
  80046128: srl  a0,a0,7              ; H = fov >> 7
PSX.EXE FUN_80021bbc (Cut-Wechsel im Spiel)
  80021e6c: jal  0x80066c30
  80021e70: srl  a0,a0,7
  80021e8c: jal  0x80053ca4           ; LookAt-Matrix aus pos/tgt
PSX.EXE 0x80066c30
  80066c30: 0x48c4d000                ; ctc2 a0,$26  = GTE-Register H
  80066c34: jr   ra
```

Die Sichtmatrix baut FUN_80053ca4 ganzzahlig (SquareRoot0, abschneidende Divisionen, MulMatrix) —
`RE_15_Quellcode_V2/FUN_80053ca4.c`; der Port tut dasselbe in `re15_camera_build_view`.

### 3.2 Masken und Objekte teilen EINE Ordnungstabelle — es gibt keinen Objekt-Vorrang

```
Maske (FUN_80039590):
  80039650: lh   a0,2(s3)             ; die Tiefe aus dem Maskensatz
  80039658: sll  a0,a0,2              ; OT-Wortindex = Tiefe
  8003965c: addu a0,a0,s6
  80039660: jal  0x8006b538           ; AddPrim
Objekt-Dreieck (FUN_800254a0):
  80025640: 0x4b58002d                ; AVSZ3
  80025654: sra  v0,v1,6              ; otz < 64 -> Polygon faellt weg
  8002565c: sra  v1,v1,4              ; OT-Wortindex = otz >> 4
Objekt-Viereck (FUN_800256b0):
  8002589c: 0x4b68002e                ; AVSZ4
  800258b0: sra  v0,v0,6
  800258dc: sra  v0,v0,4
Mittelungsfaktoren:
  80066c70: addiu t0,zero,341         ; ZSF3
  80066c7c: addiu t0,zero,256         ; ZSF4
```

Der Objekt-Zeichner FUN_8002c18c (`RE_15_Quellcode_V2/FUN_8002c18c.c`) reicht den Mesh-Zeichnern
genau vier Dinge: die zwei Mesh-Zeiger, die Flächenfarbe pool+0x70 und `bVar2 = (pool[+0x0C] & 2)`
(Halbtransparenz). Ein Tiefen-Versatz je Objekt existiert nicht. **Ein Objekt auf diesem Tisch
wäre also auch im Original von der Tischmaske verdeckt** — die Verdeckung in 2.5 ist kein
Port-Fehler, sondern die Folge davon, dass die Maske nie für ein Objekt auf der Platte
gezeichnet wurde. RE1.5 hat auf diesem Tisch kein Objekt.

### 3.3 Licht je Objekt

FUN_8002c18c ruft vor dem Zeichnen `FUN_80053fc0(pool+0x5C)` (dynamisches Licht am ORT des
Objekts), dann `SetColorMatrix`, `SetLightMatrix(Licht × Weltmatrix)`. Der Lichtsatz ist der des
aktiven Cuts (RDT @0x2C, 40 B je Cut; ROOM1150 @0x398). Der Port bildet das in
`re15_light_setup_actor` ab; die Messung 2.6 ist das Ergebnis genau dieses Wegs.

### 3.4 Obj_model_set (0x2D, 34 B) — RE1.5 LAB_80040914

```
  8004093c: lbu a3,1(a2)   ; obj_id  -> Pool-Index (Schrittweite 148, Basis 0x800b3f98)
  80040940: lbu a0,2(a2)   ; Typ      -> pool+8
  8004096c: lbu v0,4(a2)   ; Band     -> pool+130
  8004097c: lbu a0,5(a2)   ; Eltern (0xC0|n = haengt an Objekt n)
  80040990: lhu v0,6(a2)
  80040998: ori v0,v0,0x1  ; Flags = pc[6..7] | 1 -> pool+0
  800409a8: lh  v0,8(a2)   ; -> pool+12 (Bit 2 halbtransparent, Bit 0x10 anderer Zeichner)
  800409b4: lh  v0,10(a2)  ; x -> pool+52
  800409c0: lh  v0,12(a2)  ; y -> pool+56
  800409cc: lh  v0,14(a2)  ; z -> pool+60
  800409d8: lhu v0,16(a2)  ; rot x -> pool+104      800409e4: +18 rot y     800409f0: +20 rot z
```

Zensus über alle 121 Item-Weltmodelle des Spiels (Obj_model_set des Props, das ein
Item_aot_set nennt): **100 von 121** tragen Typ 0, Band 1, Eltern 0x00, pc[6..7] = 0x000A,
pc[8..9] = 0x0000; **121 von 121** eine Null-Kollisionsbox. Bit 0x2 der Flags schaltet die
Objekt-Kollision ab (`re15_collision.c:909`, @0x8002cff4). Beispiel Blue Keycard,
ROOM1110 @0x00BB4: `2d 02 00 00 01 00 0a 00 00 00 48 0d f8 f8 9b 14 00 00 c9 01 00 00 …`.

### 3.5 Item_aot_set (0x50, 22 B) — RE1.5 @0x80040644

```
  8004065c: lbu  v0,3(a2)
  80040664: andi v0,v0,0x80          ; Langform?
  80040680: lhu  a1,18(a2)           ; Kurzform: Zone-9-Bit
  80040684: lbu  s1,20(a2)           ;           Prop-Index
  80040688: addiu v0,a2,22           ;           Vorschub 22
  800406d0: sw   v0,0(s0)            ; Satz = pc+2 in die AOT-Tabelle 0x800ac9b0 + 4*slot
  800406d8: lw   a0,18056(a0)        ; 0x80074688 -> 0x800B1078 = Zone 9
  800406dc: jal  0x8004efe4          ; Bit gesetzt?
  800406f4: sb   zero,0(v0)          ;   ja: Satz stilllegen
  800406f8..80040718                 ;       pool[prop].flags = 0x80000000 (Modell weg)
```

Satzlage: +1 Slot, +2 sce, +3 sat, +4 floor, +6 x, +8 z, +10 w, +12 d, +14 Item-Id, +16 Menge,
+18 Bit, +20 Prop, +21 action. Zensus (164 Sätze): sat 0x31 160-mal, sce 9 162-mal, floor 0
152-mal, action 0 164-mal; häufigstes Rechteck 1000×1000 (71-mal). Rohbeispiel ROOM5010 @0x0041E:
`50 06 09 31 00 00 99 b0 d0 a3 d0 07 fc 08 22 00 01 00 12 00 01 00`.

Zwei Gegenstände nebeneinander teilen im Original sogar DASSELBE Rechteck: ROOM5010 @0x0041E
(First Aid Spray, Prop 1) und @0x00434 (Shotgun Shells, Prop 2) tragen beide
(−20327, −23600, 2000, 2300), die Modelle liegen 650 auseinander. Der Satz mit dem kleineren
Slot bekommt den Druck zuerst.

### 3.6 Reichweite: 620 voraus

```
  80042bd0: ori  v0,zero,0x26c       ; 620
  80042ea0: lbu  v1,1(s0)            ; sat
  80042ea8: andi v0,v1,0x40          ; 0x40 = Standpunkt, 0x20 = Punkt 620 voraus, 0x10 = Aktionstaste
```

Wie das Original auf DIESEM Tisch ein Aktions-Rechteck legt, zeigt das Telefon: ROOM1150
main00 @0x00DA6 `2c 03 03 31 00 00 40 a2 c0 ae e8 03 e8 03 ff 00 18 06 00 00` = Slot 3,
Rechteck (−24000, −20800, 1000, 1000), ruft sub06 (`Cut_chg 6`, `Message_on 1`). Die
Vorderkante x = −23000 liegt 284 VOR dem Prüfpunkt des Spielers an der Tischkante (−23284).

### 3.7 Zone 9: welche Bits frei sind

Zensus (`zensus.txt`): 85 von 256 Bits belegt (164 Item_aot_set-Nutzlasten + jeder Ck/Set mit
bank = 9). Freie Blöcke: 0…1, 3…5, 7…9, 13, 17, 23…51, **53…84**, 87…89, 93…97, 100…103,
106…107, 110…133, 137, 139, 145, 156…161, 170…179, 199…215, 219…222, 232…251, 254…255.
Portseitig vergeben ist darüber hinaus nur **53** (`RE15_SICHERUNG_TAKEN_BIT`, einziger
Treffer einer Suche nach festen Zone-9-Bits in `engine/`, `platform/pc/`, `include/`).
Bit 0 scheidet aus: das Item-Modal setzt das Flag nur `if (s_taken)` (`item_modal_common.c:340`).
→ **54** (Dokument) und **55** (Memory Card), die nächsten zwei im selben Block.

### 3.8 Lage des Dokument-Props in RE2 (room10E0, „Secretary's diary B")

RE2-Handler Opcode 0x2D: Tabelle 0x800A74C8 + 4·0x2D = @0x800A757C → **0x80055260**:

```
  80055290: lbu t1,1(s2)             ; Objekt-Nummer
  8005530c: lh  a1,12(s2)            ; Attribut -> obj+16
  80055310: lh  a2,14(s2)            ; x -> obj+56
  80055314: lh  a3,16(s2)            ; y -> obj+60
  80055308: lh  v0,18(s2)            ; z -> obj+64
  80055340: lh  a1,20(s2)            ; Drehung x/y/z (+20/+22/+24) -> obj+116/118/120
```

```
ROOM10E0.RDT sub00 @0x01CDA  2d 00 00 00 00 00 00 00 00 00 0a 00 10 00 20 b8 50 fb 76 cb 00 00 00 00 00 00 …
   Objekt 0, Flags 0x000A, Attribut 0x0010, Lage (−18400, −1200, −13450), Drehung (0, 0, 0)
ROOM10E0.RDT sub00 @0x01D26  4e 03 02 31 00 00 a2 b3 50 c9 40 06 40 06 70 00 01 00 69 00 00 00
   Slot 3, sat 0x31, Rechteck (−19550, −14000, 1600, 1600), Item 0x70 = 112, Menge 1, Flag 105, Modell 0
```

Das Modell selbst: ROOM10E0.RDT MD1 @0x002320 (372 B), TIM @0x015224 (17 440 B) — byte-gleich
mit `extracted_re2_dokumente/weltmodelle/mesh03_cf9f316d.md1/.tim` (`cmp`). Auch RE2 legt das
Buch NICHT in die Mitte seines Rechtecks (Rechteckmitte (−18750, −13200), Buch 350/250 daneben)
und gibt ihm die Flags 0x000A wie RE1.5 seinen Items.

### 3.9 Welt-Modell für die Memory Card: es gibt keines — Vorschlag aus Vorhandenem

RE1.5 hat für Id 0x21 kein Welt-Modell und RE2 hat kein Item „Memory Card". Vorhanden ist:

| Baustein | Herkunft | Zustand |
|---|---|---|
| Geometrie „Karte" | RE1.5 Keycard-MD1, 156 B, md5 `93975479cdd85aa9e8e4232f6c8c8983`, ROOM1110.RDT @0x13D8; neunmal platziert (ROOM1011, 1110/1111, 1190/1191 ×2, 3040/3041), vier Texturvarianten | UNVERÄNDERT übernommen: 2 Dreiecke, Punkte (0,0,0) (0,0,270) (−161,0,270) (−161,0,0), Normale (0,−4095,0), UV-Feld u 0…102 / v 0…63 |
| Bild der Karte | RE1.5 Item-Bild ITPS.ITP @0x63000 | Schrägansicht, muss entzerrt werden |

Prototyp (`r30_idw_karte_modell.py`, Datei `karte_modell.txt`): die Deckfläche der Karte wird im
Item-Bild als Viereck bestimmt — Hintergrund ist blau (B > R+20), die Seitenfläche dunkel — und
per Homographie in das UV-Feld des Keycard-Modells gelegt. Das Viereck hängt nicht an der
Schwelle: L ≥ 60 / 70 / 80 liefern (46,7) (87,14) (77,58) (25,47); zwei Ecken sind stabil, die
beiden anderen weichen um 1 bzw. 3 Pixel ab. 6569 von 6592 Texeln sind belegt, die runden Ecken bleiben über
CLUT-Index 0 = 0x0000 durchsichtig. TIM-Kopf 1:1 von der Keycard-TIM (flag 0x09, 128×64, 8736 B).

* `irons-diary-welt/memcard.md1` (156 B, byte-gleich Keycard) und `memcard.tim`
  (8736 B, md5 `885d91614f6ff1378cc0ea41328810b8`)
* Bilder: `itps_21_deckflaeche_x4.png`, `memcard_prototyp_tim_x4.png`,
  im echten Renderer `rig_memcard_cut2_klemme87.png`, `rig_memcard_cut6.png`

Das ist eine **KONSTRUKTION aus vorhandener Kunst**, kein Original-Asset. Die Alternative ohne
jede Konstruktion ist die Keycard mit ihrer eigenen Textur („GATE SYSTEM / RACCOON POLICE",
`keycard_1110_tim_x4.png`) — dann läge auf dem Tisch sichtbar eine Keycard, die als Memory Card
ins Inventar geht.

Das Keycard-Modell hat seinen Ursprung an einer ECKE. Bei rot_y = 3072 ist Modell (x,0,z) →
Welt (−z, 0, x); die Kartenmitte (−80,5 ; 0 ; 135) liegt also (−135, 0, −80,5) neben dem
Ursprung. Damit die MITTE auf der blauen Marke (−23657, −1520, −18649) liegt, steht das Prop bei
**(−23522, −1520, −18568)**. Gemessen im Rig: Bildmitte (139,83 ; 126,33), Marke (140,0 ; 126,5).

---

## 4. URSACHE

Es gibt kein Fehlverhalten zu erklären — beide Gegenstände sind neu. Zu erklären ist, warum der
naheliegende Weg scheitert; alle vier Punkte sind gemessen:

1. **RE1.5 hat keines von beiden.** 0 Platzierungen ≥ 0x48 (Schwester-Dossier 3.2) und 0 für
   0x21; der Raum benutzt Bank 9 nicht. Alles kommt portseitig dazu, ohne Asset-Patch.
2. **Die Tischmaske verdeckt, was auf dem Tisch liegt.** Drei Masken der Tiefe 87 (2.5); Props
   bei Bucket 92. Ohne Gegenmaßnahme: 0 sichtbare Pixel in dem Cut, den der Nutzer markiert hat.
3. **Ein ums Prop zentriertes Aufhebe-Rechteck ist nicht erreichbar** (2.9): die Gegenstände
   liegen 370…530 hinter dem Prüfpunkt des Spielers.
4. **Die Item-Id im Auftrag war um eins verschoben** (2.10): 0x20 wäre die Incendiary Capsule.

---

## 5. UMSETZUNGSPLAN für den Bau-Agenten

Muster: `engine/src/sicherung_1150.c` / `include/re15_sicherung.h` / `engine/src/gen/sicherung_prop.inc`.
Kein Asset-Patch; die RDTs bleiben byte-gleich.

### S1 — Konstanten (neu: `include/re15_irons_tisch.h`)

| Konstante | Wert | Beleg |
|---|---|---|
| Räume | 0x1150, 0x1151 | Kamera-, Licht- und Maskentabellen beider Varianten byte-gleich (`vergleich_1151.txt`) |
| `DIARY_ITEM` | 0x48 | Schwester-Dossier S1; erste FILE-Id u8 @0x800c7370 |
| `DIARY_OBJ_ID` / TIM-Slot | 5 / `RE15_TIM_SLOT_PROP(5)` = 9 | 2.8 |
| `DIARY_AOT_SLOT` | 7 | 2.8 |
| `DIARY_TAKEN_BIT` | 54 | 3.7 |
| `DIARY_POS` | (−23656, −1533, −18291) | 2.4, Marke rot, Rückprojektion 0,02 px |
| `DIARY_ROT_Y` | 3072 | 2.4 (Tischachse) — **NICHT aus dem Original**, Platzierungswahl |
| `DIARY_RECT` (x, z, w, d) | (−24000, −18791, 1000, 1000) | x und w: Telefon-Satz ROOM1150 @0x00DA6; z = Prop-z − 500; Größe 71 von 164 |
| `MEMCARD_ITEM` | 0x21 | 2.10, DEBUG.BIN @0x499E |
| `MEMCARD_AMOUNT` | 3 | RE2: 21 von 21 Platzierungen des Speicher-Items; RE1.5 hat keine — Obergrenze 250 @0x80074F34 |
| `MEMCARD_OBJ_ID` / TIM-Slot | 6 / `RE15_TIM_SLOT_PROP(6)` = 26 | 2.8 |
| `MEMCARD_AOT_SLOT` | 8 | 2.8 |
| `MEMCARD_TAKEN_BIT` | 55 | 3.7 |
| `MEMCARD_POS` | (−23522, −1520, −18568) | 3.9 (Ecken-Ursprung des Keycard-Modells) |
| `MEMCARD_ROT_Y` | 3072 | wie Dokument |
| `MEMCARD_RECT` | (−24000, −19149, 1000, 1000) | wie Dokument, z = Marken-z − 500 |
| Prop-Felder beider | Typ 0, Band 1, Eltern −1, Flags 0x000B, Nullbox | 3.4: 100 von 121 / 121 von 121; 0x000B = 0x000A \| 1 @0x80040998 |
| AOT-Felder beider | sat 0x31, floor 0, action 0 | 3.5: 160 / 152 / 164 von 164 |
| `TISCH_OT_MAX` | 87, nur in Cut 2 | ROOM1150.RDT @0x006EC, @0x006F8, @0x00718 (je `57 00`) |

### S2 — Modelle einbacken (neu: `re15_port/tools/irons_tisch_engine_export.py` → `engine/src/gen/irons_tisch_props.inc`)

* Dokument: `info/re2leon/PL0/RDT/ROOM10E0.RDT` @0x002320 (372 B, md5 `cf9f316dd6ba0ecf599bea18a83c36d1`)
  und @0x015224 (17 440 B, md5 `63bd93f2be1cc97066aa09afd3c67e4d`), unverändert.
* Karte: MD1 aus `STAGE1/ROOM1110.RDT` @0x13D8 (156 B), TIM nach dem Verfahren von
  `r30_idw_karte_modell.py` (Werkzeug nach `re15_port/tools/` übernehmen; der Export soll die
  TIM selbst erzeugen und gegen md5 `885d9161…` prüfen).
* Das Werkzeug prüft beide Quellen gegen die md5 und bricht ab, wenn sie nicht stimmen.

### S3 — Anlegen (neu: `engine/src/irons_tisch_1150.c`, Aufruf in `engine/src/scd_room_setup.c`)

`re15_irons_tisch_install(room_id)` an ZWEI Stellen, jeweils direkt hinter dem Aufruf für die
Sicherung:

* Tür-/Sprung-Weg: `scd_room_setup.c:410` in `scd_room_reenter`.
* Lade-Weg: `platform/pc/main.c`, hinter dem Block, der sub00 als Init-Lauf fährt (Blockende
  Zeile ~4127), vor dem Kamera-Restore. ⛔ Der CONTINUE-Boot läuft NICHT durch
  `scd_room_reenter` — gemessen im Schwester-Dossier `sicherung.md` §2.4: nach LOAD in
  ROOM1150 fehlt die Sicherung ganz (25 Bilder pixelgleich mit dem Lauf ohne Prop). Derselbe
  Ausfall träfe Buch und Karte. `sicherung.md` §5.1 setzt dort den Aufruf für obj 4; dieser
  hier kommt in die Zeile danach.

Je Gegenstand:

1. `re15_game_flag_get(9, bit)` gesetzt → NICHTS anlegen (weder Prop noch Zone).
2. Prop an `g_scd.props[prop_count++]` mit den Feldern aus S1 (Vorlage: `sicherung_1150.c:71-95`);
   vorher prüfen, dass die obj_id noch nicht im Pool steht und der Pool Platz hat.
3. Zone: `re15_aot_set_item_tk_prop(slot, x + w/2, z + d/2, w/2, d/2, item, menge, bit, obj_id)`,
   danach `g_aot.slots[slot].sce_flags = 0x31` und `.band = 0` (wie `op_item_aot_set`,
   `scd_vm.c:3922/3928`).

### S4 — Laden und Zeichnen (`platform/pc/main.c`)

* `pc_load_room_prop_set` (hinter dem Sicherungs-Block, Zeile 1360-1376): MD1 nach `md1[5]` /
  `md1[6]`, TIM nach Slot 9 / 26. Riegel wie dort: Raum 0x1150/0x1151 UND `nprops <= 5`.
* Prop-Zeichner, zwei Stellen (Dreiecke bei `wz_for_sort`, Zeile ~9648; Vierecke bei `wz_avg`,
  Zeile ~9704): für obj_id 5 und 6 im aktiven Cut 2 den Sortierschlüssel auf
  `(int)re15_pri_mask_camera_z(87) − 1` = 5636 begrenzen. Das ist die im Rig gemessene Form
  (`r30_idw_rig_patch.py` Abschnitt 4). Die Klemme ist ein PORT-ZUSATZ; das Original kennt
  keinen Objekt-Vorrang (3.2).
* Android benutzt dieselbe `main.c` (`platform/android/jni/CMakeLists.txt:45`); die neue
  `engine/src/irons_tisch_1150.c` braucht dort ein frisches Configure (GLOB-Cache).

### S5 — Aufheben

* Memory Card: nichts Neues. Die Zone ist Typ ITEM, `aot_common.c:706/1374` ruft
  `re15_item_modal_start(0x21, 3, 55, 8, 6)`; das Modal setzt beim Bestätigen Flag (9,55),
  schaltet Slot 8 ab und blendet obj 6 aus (`item_modal_common.c:340/342/360`).
* Dokument: Zweig des Schwester-Themas (S5 dort, `item_type >= 0x48`). Aus der Zone kommen
  Bit 54, Slot 7, obj 5; abgeräumt wird NACH dem Schließen des Lesers und der Meldung mit
  denselben drei Schritten. Damit verschwindet das Buch „nach dem Lesen und Zumachen" — das ist
  RE2s Reihenfolge (@0x80072b0c-bfc, Schwester-Dossier 3.5).

### S6 — Riegel (`re15_port/tests/unit/probes/r30_irons-tisch.cmake`, als `add_test`)

Vorlage für jeden Punkt ist ein Befehl der Mess-Sonde `probe_r30_irons-diary-welt.c`:

| Riegel | Sonden-Befehl | Soll |
|---|---|---|
| Props liegen richtig | `pool` | ROOM1150 UND 1151: obj 5 bei (−23656,−1533,−18291) rot_y 3072, obj 6 bei (−23522,−1520,−18568), Flags 0x000B |
| Zonen liegen richtig | `aots` | Slot 7 x[−24000…−23000] z[−18791…−17791], Slot 8 z[−19149…−18149], Typ ITEM, flags 0x31 |
| anderer Raum bleibt leer | `pool 1140` | keine obj 5/6 |
| schon genommen | Flag (9,54) bzw. (9,55) vor dem Hochfahren | Prop fehlt, Slot inaktiv — je einzeln |
| Druck trifft | `druck −22664 −18650 2048 …` | Modal aktiv, Item 0x21, Menge 3 |
| Druck trifft nicht | Blick +X (rot 0); Stand x = −22364 | kein Modal |
| Marke getroffen | `projekt` | obj 5 in Cut 2 näher als 1 px an (152,5 ; 126,5); Kartenmitte näher als 1 px an (140,0 ; 126,5) |
| Masken-Konstante | RDT lesen | die Masken über den Bildhüllen in Cut 2 tragen Tiefe 87 (@0x6E8, @0x6F4, @0x714), in BEIDEN RDTs; Cut 6 hat keine |

### S7 — Abnahme-Messung am gebauten Spiel (nicht am Rig)

`RE15_DEBUG_JUMP=1150@240`, `RE15_FORCE_CUT=2` bzw. `6`, `RE15_FRAMEDUMP=400-600/100:f_`,
Auswertung gegen einen Lauf mit gesetzten Flags (9,54) und (9,55) als Nullbild
(`RE15_SET_FLAG="9:54,9:55"`):

| Messung | Soll (Rig-Wert) |
|---|---|
| Cut 2, sichtbare Pixel Dokument / Karte (960×720) | > 0 (525 / 179) |
| Cut 2, Bildmitte Dokument / Karte | (151,83 ; 126,33) / (139,83 ; 126,33), höchstens 1 px von der Marke |
| Cut 6, sichtbare Pixel | 8698 / 3233 |
| Cut 2, Spieler auf (−22664, −18450) rot 2048 | die Figur liegt ÜBER beiden Props |
| nach dem Aufheben, selber Raum | 0 abweichende Pixel gegen das Nullbild |
| Raum verlassen und wieder betreten | Props bleiben weg |
| Spielstand in ROOM1150 laden, NICHTS genommen | beide Props da, beide Zonen aktiv (Lade-Weg, S3) |
| Spielstand in ROOM1150 laden, beides genommen | Props und Zonen fehlen |

---

## 6. Risiken und offene Fragen

1. **Helligkeit (2.6).** Mit dem ausgelieferten Licht bleibt in Cut 6 rund ein Sechstel der
   Eigenfarbe. Die Texturen selbst sind NICHT zu dunkel: Summe RGB im Mittel 205 (Dokument) und
   298 (Karte) gegen 197…321 bei den Original-Props (`helligkeit.txt`). Der Plan lässt das Licht
   byte-true. Wer es heller will, hat zwei Wege, beide Port-Zusatz: die zwei Props in jedem Cut
   mit dem Lichtsatz von Cut 2 beleuchten (RDT @0x398 + 2·40), oder die Textur aufhellen wie bei
   der Sicherung — das hilft aber nur einem der beiden Cuts, sie unterscheiden sich um Faktor 3.
2. **Lage A oder B.** A ist die Markenmitte (0,02 px). B legt das Buch genau auf das gemalte
   Klemmbrett: (−23813, −1533, −18280), rot_y 2944, 2,29 px von der Markenmitte und innerhalb
   der Marke. Bei B muss das Rechteck NICHT geändert werden (es hängt an x = −24000). Bilder
   beider Lagen liegen bei. Der Plan nimmt A, weil A der Marke wörtlich folgt.
3. **Menge 3** folgt RE2, nicht RE1.5. Der Nutzer schrieb „der Memory Card"; wer das als
   „genau eine" liest, setzt `MEMCARD_AMOUNT` auf 1.
4. **Überlappende Rechtecke.** Im z-Bereich −18791…−18149 treffen beide; Slot 7 (Dokument)
   gewinnt. Das ist Original-Praxis (3.5, ROOM5010), aber wer vor der Karte steht und das
   Tagebuch bekommt, mag sich wundern. Reine Alleinbereiche: je 358 breit.
5. **Ladeweg.** Von mir NICHT selbst gemessen, sondern aus `sicherung.md` §2.4 übernommen
   (Stichprobe: `re15_sicherung_install` hat genau eine Aufrufstelle, `scd_room_setup.c:410`;
   `main.c` ruft `scd_room_reenter` im Boot-Block nicht). Modell und Textur stehen am Lade-Weg
   bereit (`pc_load_room_prop_set`, dort gemessen), es fehlt nur das Anlegen. Die Abnahme S7
   enthält deshalb ausdrücklich den Fall „Spielstand in ROOM1150 laden".
6. **Thema H (Sicherung)** arbeitet im selben Raum an obj 4 / Bit 53. obj 5/6, Slot 7/8 und
   Bit 54/55 sind dort nicht vergeben — das gilt für den Stand 437905cb.
7. **PSX-Plattform.** `platform/psx/` kennt schon die Sicherung nicht (keine Fundstelle). Dort
   wäre die Klemme `min(otz >> 4, 87)` vor dem Einhängen in die OT.
8. **Nachgezeichnete Masken (R15M).** Bekäme Cut 6 später Seitendaten-Masken über dem Tisch,
   bräuchte auch Cut 6 eine Klemme. Der Masken-Riegel in S6 fängt das.

---

## 7. Ausdrücklich NICHT belegt

1. **Yaw 3072** — keine Instruktion, kein Datensatz. Begründet allein über die Tischachse und
   die Ausrichtung der gemalten Gegenstände.
2. **Das Welt-Modell der Memory Card** — Konstruktion (3.9). Weder RE1.5 noch RE2 hat eines.
3. **Menge 3** — RE2-Analogie über das Ink Ribbon; RE1.5 sagt dazu nichts.
4. **Die Tiefen-Klemme** — Port-Zusatz ohne Original-Gegenstück. Belegt ist nur ihr WERT (die
   Maskentiefe 87) und ihre Wirkung (Rig).
5. **Tischhöhe −1520** — gemessen aus zwei Bildern mit zwei Verfahren, Auflösung 10 Einheiten.
   Die Kollision trägt keine Höhe (SCA-Satz: Breite, Tiefe, x, z, Form, zwei Flagbytes,
   Etagenstufe; `re15_rdt.h:62-71`), ein Tischmodell gibt es nicht.
6. **Ein Verbraucher für Item 0x21 im Original** — nicht gefunden; das Suchverfahren sieht
   aber keine über Register durchgereichten Ids.
7. **RE2-Zensus Ink Ribbon** — 171 von 2568 SCD-Blöcken liefen desynchron; 21 ist eine
   Untergrenze. Die Aussage „immer Menge 3" gilt für die 21 gefundenen.
8. **Das Verhalten am laufenden Original** — nichts in diesem Dossier wurde in DuckStation
   gegengeprüft; es gibt dort weder das Buch noch die Karte.

---

## 8. Artefakte

| Was | Wo |
|---|---|
| Mess-Sonde (kamera, projekt, strahl, pool, bits, aots, anlauf, abdeckung, druck, re2prop, licht, zonen, reihe) | `re15_port/tests/unit/probe_r30_irons-diary-welt.c`, `probes/r30_irons-diary-welt.cmake` |
| Werkzeuge | `analysis/befunde_runde30/werkzeuge/r30_idw_*.py`, `r30_idw_rig_lauf2.sh` |
| Messdateien | `build/r30_irons-diary-welt/*.txt` |
| Mess-Rig (Quellkopie + Binary + Läufe) | `build/r30_irons-diary-welt/rig/`, `rig_build/`, `rig_lauf/` |
| Bilder und Prototyp zum Dossier | `analysis/befunde_runde30/irons-diary-welt/` |

---

## 9. UMSETZUNG (Bau, Zweig r30/irons-diary-welt)

Stand: aufgesetzt auf master d98e9639, dann die zwei Vorgänger zusammengeführt —
irons-diary-dokument (`worktree-wf_b4b268f3-d12-4`, e2a3b626) und sicherung
(`worktree-wf_b4b268f3-d12-2`, f9fee34e), beide konfliktfrei (Merge de5ae65a). Beide
Vorgänger sind fertig (UMSETZUNG in ihren Dossiers, Suite 362 bzw. 365). Die Suite des
zusammengeführten Stands VOR dem ersten Edit: **367 von 367**.
Commits dieser Spur: 3a78756e (S1–S4), f5bce99b (S5), e16a14df (S6), 20fe9420 (S7) und
dieser Nachtrag.

### 9.1 Vor dem ersten Edit selbst nachgeschlagen (richtige Datei, Sprungziel)

| Behauptung des Plans | nachgeschlagen | Ergebnis |
|---|---|---|
| Obj_model_set: Flags = pc[6..7] \| 1 | `info/Re1.5/PSX.EXE` LAB_80040914: @0x80040990 `lhu v0,6(a2)`, @0x80040998 `ori v0,v0,0x1`, @0x800409a4 `sw v0,0(a1)`; Typ @0x8004095c, Band @0x80040974 | bestätigt |
| Item_aot_set Kurzform: Bit +18, Prop +20, Vorschub 22 | @0x80040680 `lhu a1,18(a2)`, @0x80040684 `lbu s1,20(a2)`, @0x80040688 `addiu v0,a2,22`; Bit gesetzt @0x800406dc `jal 0x8004efe4` → @0x800406f4 `sb zero,0(v0)`, Modell weg @0x800406f8-718 | bestätigt |
| Masken-OT und Objekt-OT | @0x80039650 `lh a0,2(s3)`, @0x80039658 `sll a0,a0,2`, @0x80039660 `jal 0x8006b538`; @0x80025654 `sra v0,v1,6`, @0x8002565c `sra v1,v1,4`; @0x800258b0 `sra v0,v0,6` | bestätigt |
| Reichweite 620 | @0x80042bd0 `ori v0,zero,0x26c` | bestätigt |
| Objekt-Kollision Bit 0x2 | @0x8002cff4 `andi v0,v0,0x2` | bestätigt |
| Masken Tiefe 87, Cut-2/6-Kamera | `xxd` ROOM1150.RDT UND ROOM1151.RDT: @0x6E8/@0x6F4/@0x714 je `57 00` an +4; @0xA0/@0x120 byte-gleich; Cut 6 @0xBCC `ff ff ff ff` | bestätigt, beide RDTs |
| Telefon-Satz | ROOM1150.RDT @0x00DA6 und ROOM1151.RDT @0x00DA6: `2c 03 03 31 00 00 40 a2 c0 ae e8 03 e8 03 ff 00 18 06 00 00` → x −24000, w 1000 | bestätigt, beide RDTs |
| „Memory Card" = 0x21 | DEBUG.BIN @0x499E `c2 01` → @0x4BEA `29 41 49 4b 4e 55 00 1f 3d 4e 40 07`; Eigenschaften @0x80074F34 `fa 00 00 00 88 4c 07 80 01 00 00 00` (0x20: `… 48 4d 07 80 …`) | bestätigt |
| Quellen der Modelle | md5: ROOM10E0 @0x2320 `cf9f316d…`, @0x15224 `63bd93f2…`; ROOM1110 @0x13D8 `93975479…`, Keycard-TIM @0x2BAC8 `5ce475a4…` | bestätigt |
| RE2 Menge 3 | RE2 ROOM1060.RDT @0x00C24 `4e 02 02 31 … 1e 00 03 00 …`; ROOM10E0 @0x1D26 Menge 1 | bestätigt |
| Slot 7/8, obj 5/6 frei | Walk über alle Opcodes von ROOM1150 (317) und ROOM1151 (186): Aot-Opcodes nur Slots 0…6 bzw. 0…5, kein Obj_model_set mit obj 5/6 | bestätigt |
| Zone-9-Bits 54/55 frei | `git grep` über alle r30-Zweige und master: nur Bit 53 (Sicherung) fest vergeben | bestätigt |

Ausgangszustand mit der vorhandenen Sonde (Bau von de5ae65a): `pool 1150/1151` 5 Einträge
(obj 0…3, Sicherung obj 4 bei (−280,−1062,1260) rot 1024), `pool 1140` 1 Eintrag; `aots`
Slot 7/8 leer; `projekt` Lage B Cut 2 (153,49;124,44), Kartenmitte (139,97;126,49).

### 9.2 Gebaut, je Plan-Schritt

| Schritt | gebaut | Datei |
|---|---|---|
| S1 Konstanten | alle mit Beleg bzw. als „PORT-WAHL, KEINE ORIGINAL-ADRESSE" mit Messung (Lage, Rechteck-z/d); Menge 3 mit RE2-Adresse und dem Satz, warum RE1.5 nicht maßgeblich ist | `include/re15_irons_tisch.h` |
| S2 Modelle | Export-Werkzeug prüft alle vier Quellen gegen md5 und erzeugt die Karten-TIM selbst — **md5 885d9161… wie der Prototyp** (6569 Texel belegt) | `tools/irons_tisch_engine_export.py` → `engine/src/gen/irons_tisch_props.inc` |
| S3 Anlegen | `re15_irons_tisch_install` an beiden Raumstart-Wegen, direkt hinter `re15_sicherung_install` (Tür: `scd_room_reenter`; Lade-Weg: CONTINUE-Boot in `main.c` samt Logzeile `[irons-tisch] Boot-Weg`) | `engine/src/irons_tisch_1150.c`, `scd_room_setup.c`, `platform/pc/main.c` |
| S4 Laden + Zeichnen | Lader obj 5/6 → TIM-Slot 9/26 (Riegel Raum + nOmodel); Tiefen-Klemme im Prop-Zeichner (Dreiecke und Vierecke) über `re15_irons_tisch_sort_max(raum, active_cut_idx, oid)` — `active_cut_idx` ist der angezeigte Cut, derselbe, der die Masken wählt | `platform/pc/main.c`, `irons_tisch_1150.c` |
| S5 Aufheben | Karte über das Item-Modal (unverändert), Diary über die Weiche der Dokument-Spur (`aot_item_dokument` → `re15_menu_request_doc(0, 54, 7, 5)`) — kein neuer Code nötig. **Dazu ein Fehler behoben**, s. 9.3 Punkt 3 | `engine/src/inventory_common.c` |
| S6 Riegel | `unit_r30_irons_tisch` (P/Z/G/D/M/K/B), `integration_r30_irons_tisch_laden` (echte exe, CONTINUE 1150/1151, genommen) | `tests/unit/test_r30_irons_tisch.c`, `tests/unit/probe_r30_irons_tisch_karte.c`, `tests/integration/test_r30_irons_tisch_laden.cmake`, `tests/unit/probes/r30_irons-diary-welt.cmake` |
| S7 Abnahme | am gebauten Spiel, s. 9.4 | `werkzeuge/r30_idw_bau_*.{sh,py}` |

Messhaken `RE15_IRONS_LOG=1` (`main.c`, env-gegatet, reine Diagnose): eine `debug.log`-Zeile je
Zustandswechsel am Schreibtisch (Bits 54/55, Zonen 7/8, Props 5/6, Menge 0x21, FILE-Platz 0,
Modal, Menü). Die GUI-exe hat sonst keinen Blick auf diesen Zustand.

### 9.3 Abweichungen vom Plan (mit Grund)

1. **Lage B statt A** — so im Auftrag der Runde festgelegt: (−23813,−1533,−18280), rot_y 2944,
   das Buch liegt auf dem gemalten Klemmbrett (Abzug `bau_cut6.png`).
2. **Rechteck des Diary z = −18780 statt −18791.** Die Regel des Plans ist „z = Prop-z − 500";
   mit Lage B ist Prop-z −18280. x/w bleiben am Telefon-Satz. Überlappung mit der Karte:
   z −18780…−18149 (Slot 7 gewinnt, Original-Praxis ROOM5010, Risiko 4 unverändert).
3. **Halbierung für 0x21 abgeschaltet (nicht im Plan, gemessen).** Erster Lauf im gebauten
   Spiel: Modal Item 0x21, aber `karte_menge=1` statt 3. Ursache: die Nutzer-Entscheidung
   „Munition halbieren" (Runde 17, `re15_pickup_menge_nutzer`) nahm das bloße Id-Fenster
   0x15…0x21 (`sltiu` @0x80047d54 / @0x80049124) — 0x21 ist aber die Memory Card. Die
   Stapelregel derselben Datei schließt 0x21 schon datengetrieben aus (Eigenschaften
   @0x80074F34 → Null-Satz @0x80074C88; 0x15…0x20 tragen eigene Kombinations-Sätze, 0x20
   z. B. 0x80074D48). Die Halbierung nimmt jetzt dieselbe Schranke. Bestand: 0x21 war bis zu
   dieser Runde unerreichbar (0 Item_aot_set), für 0x15…0x20 ändert sich nichts.
   `probe_r17_munition` mitgezogen: dort hieß 0x21 „letzte Munition" — jetzt 0x20 halbiert
   (30 → 15), 0x21 bleibt (3 → 3). Nachher im Spiel: `karte_menge=3`.
4. **Riegel-Datei `probes/r30_irons-diary-welt.cmake`** statt `r30_irons-tisch.cmake` (Vorgabe
   des Auftrags: `probes/r30_<thema>.cmake`).
5. **Marken-Riegel des Diary:** „in der roten Marke und < 2,5 px" statt „< 1 px" — Lage B liegt
   gemessen 2,29 px neben der Markenmitte (Plan-Wert galt für Lage A).
6. **Cut-6-Pixel Diary 7977 statt Rig-Wert 6147:** das Rig zählte in einem festen Feld
   (x 176…212, y 116…159 bei 320), das für Lage A zugeschnitten war und den oberen Teil des
   Buchs in Lage B abschnitt. Gezählt wird jetzt gegen ein Nullbild über das ganze Bild.

### 9.4 Abnahme — gemessen am gebauten Spiel

Framedump 960×720 (`RE15_FRAMEDUMP`, beschleunigter Renderer, kein AUTOSHOT, kein
SOFTWARE_RENDER), Nullbild = derselbe Lauf mit den Bits (9,54)/(9,55) vor dem Raumeintritt.
Einzelne Props über Läufe mit nur einem gesetzten Bit getrennt. Mitte = Mitte der Hülle der
abweichenden Pixel, in 320er-Pixelmitten. Läufe unter `build/r30_irons-diary-welt/bau/`.

| Messung | Soll | Ist |
|---|---|---|
| Cut 2, sichtbare Pixel Diary / Karte | > 0 (Rig 495 / 179) | **495 / 179** (F400, F500, F600 gleich) |
| Cut 2, Mitte Diary → rote Marke (152,5;126,5) | in der Marke | (152,83;123,83), **2,69 px**, Hülle x 148…158, y 121…127 = in der Marke |
| Cut 2, Mitte Karte → blaue Marke (140,0;126,5) | ≤ 1 px | (139,33;125,83), **0,94 px** |
| Cut 6, sichtbare Pixel Diary / Karte | > 0 | **7977 / 3233** |
| Mittel-RGB Cut 2 Diary / Karte | Licht byte-true | **(46,35,25) / (52,41,37)** |
| Mittel-RGB Cut 6 Diary / Karte | Licht byte-true | **(17,16,8) / (17,17,10)** (Dossier 2.6: 16 % der Eigenfarbe) |
| Cut 2, Spieler auf (−22664,−18450) rot 2048 | Figur über den Props | Überschneidung 488 Pixel: **Figur obenauf 488, Prop obenauf 0**; 186 Prop-Pixel außerhalb der Figur sichtbar |
| Karte aufheben (Tür-Weg, Stand (−22664,−18900), Druck F400) | Modal 0x21, Menge 3, Prop weg | Modal ab F401 (Bild 0x21), F521 `flag55=1 zone8=0 prop6=0 karte_menge=3`; Cut 6: Kartenfeld 3233 → **0** Pixel gegen das Nullbild |
| Diary aufheben (Stand (−22664,−17950), Druck F400) | Leser sofort, nach Schließen + Meldung weg, FILE-Liste | Menü F411 (Aufblende), Leser F412 (Satz 25, Titel, `liste0=0`), steht F429; KREUZ F540 → Meldung „The Irons Diary has been filed."; Bestätigen F660 → F661 `flag54=1 zone7=0 prop5=0`; Menü zu F668; Cut 6: Buchfeld 7977 → **0**; START+R1 → FILE-Liste „Irons Diary" auf Zeile 0 (`bau_diary_aufheben.png`) |
| Spielstand in ROOM1150 laden, nichts genommen | Props da, Zonen aktiv | `Boot-Weg obj 5/6`, F0 `zone7=1 zone8=1 prop5=1 prop6=1`, Cut 2 **674** Pixel (gleiche Hülle wie am Tür-Weg) |
| Spielstand in ROOM1150 laden, beides genommen | Props und Zonen fehlen | F0 `flag54=1 flag55=1 zone7=0 zone8=0 prop5=-1 prop6=-1`, kein Zeichnen |
| dasselbe in ROOM1151 | wie 1150 | nichts genommen: 674 Pixel, Zonen aktiv; genommen: fehlen |
| Aufheben nach dem Laden, ROOM1150 und ROOM1151 | wie am Tür-Weg | Karte: F221 `karte_menge=3 prop6=0`; Diary: F361 `flag54=1 zone7=0 prop5=0 liste0=0` — in beiden Räumen |
| Bestand ROOM1150 (Nullbilder gegen die exe von de5ae65a, vor dem Bau) | 0 | Cut 2 **0**, Cut 6 **0**, Spieler vor dem Tisch **0** Pixel |
| Bestand andere Räume (Sprung, neue exe gegen de5ae65a) | 0 | ROOM1140 **0**, ROOM1100 **0**, ROOM1110 **0** Pixel (F400, F600) |
| Mutationsproben | Riegel fallen | Install am Lade-Weg entfernt → `integration_r30_irons_tisch_laden` rot; Halbierung wieder am bloßen Fenster → `unit_r30_irons_tisch` (x1) und `unit_r17_munition_pin` rot |

Abzüge (`analysis/befunde_runde30/irons-diary-welt/`): `bau_cut2_marken.png` (Nutzerbild mit
Marken neben Bau und Nullbild), `bau_cut6.png`, `bau_spieler_vor_tisch.png`,
`bau_karte_aufheben.png`, `bau_diary_aufheben.png`.

### 9.5 Nicht gemessen / offen

1. **Helligkeit (Risiko 1) bleibt, wie verlangt, byte-true:** in Cut 6 tragen Buch und Karte
   Mittel-RGB (17,16,8) / (17,17,10) — beide wirken fast schwarz (`bau_cut6.png`). Ursache ist
   das ausgelieferte Licht von Cut 6 (ambient 40,40,24), kein Texturfehler.
2. **Ein Lauf mit echter Tasteneingabe durch die Kamerazonen** wurde nicht gefahren; die Läufe
   stellen den Spieler per `RE15_PLAYER_POS` bzw. Spielstand hin und erzwingen den Cut
   (`RE15_FORCE_CUT`). Dass man vor dem Tisch in Cut 2 steht, ist aus Zonen und Kollision
   abgeleitet (2.11), nicht gemessen.
3. **Raum verlassen und wieder betreten im Spiel** nicht gefahren; belegt über
   `unit_r30_irons_tisch` Teil G (derselbe `scd_room_reenter` mit gesetzten Bits) und über den
   Lade-Weg mit genommenen Bits.
4. **Die Klemme in `main.c`** deckt kein Unit-Riegel (nur ihr Wert und ihre Schranken über
   `re15_irons_tisch_sort_max`); ihre Wirkung belegt der Framedump (0 → 495/179 Pixel).
5. **Erreichbarkeit des Rechtecks mit z −18780** nicht neu über die Sonde `abdeckung` gezählt
   (11 Einheiten neben dem gemessenen −18791); belegt sind die Drücke von z −17950 (nur
   Diary) und z −18900 (nur Karte).
6. **Hörprobe** nicht gemacht (alle Läufe `RE15_NOAUDIO`).
7. **PSX-Ziel** kennt weder Sicherung noch diese Props (Risiko 7); **Android**:
   `engine/src/irons_tisch_1150.c` ist neu → vor dem Paket frisch konfigurieren (GLOB-Cache).
8. **Suite: 369 von 369** (367 des zusammengeführten Stands + `unit_r30_irons_tisch` +
   `integration_r30_irons_tisch_laden`, ctest -j 4, ohne Wiederholung);
   `RE15_MIN_TESTS` in `tools/local_build.sh` nicht geändert (Vorgabe).
9. Hinweis der Dokument-Spur (10.2, RE2 räumt Effekte am Weltmodell ab): dieses Prop trägt
   keinen Effekt, es gibt nichts abzuräumen.

### 9.6 Nachbesserung nach dem Gegenprüfer

Der Gegenprüfer urteilte MÄNGEL: einer erheblich (kein Riegel auf das sichtbare Ergebnis),
zwei gering (vor der Memory Card gab es das Buch; Lage B stand in keiner Messdatei).
Alle drei sind an der Ursache behoben. Commits: c67dc15c (A: Lage B, Rechtecke), 765c0f5c
(B: Bild-Riegel), 269245cb (C: Abnahme-Werkzeuge, Pixelkonvention), dazu der Abschluss-Commit
mit diesem Nachtrag.

#### 9.6.1 Mangel 1 (erheblich): ein Riegel auf das SICHTBARE Ergebnis

Befund des Gegenprüfers, nachgestellt: `integration_r30_irons_tisch_laden` sucht die Zeile
`[prop-render] pi=5/6`, und die steht in `main.c` VOR der Tiefen-Klemme. Teil K von
`unit_r30_irons_tisch` prüft nur den Rückgabewert von `re15_irons_tisch_sort_max`.

Neu: **`integration_r30_irons_tisch_bild`** (`tests/integration/test_r30_irons_tisch_bild.cmake`,
Auswerter `tests/unit/probe_r30_irons_tisch_bild.c`, angemeldet in
`probes/r30_irons-diary-welt.cmake`). Die echte exe läuft viermal am Lade-Weg (CONTINUE,
ROOM1150, `RE15_FORCE_CUT=2`), je ein Bild über `RE15_FRAMEDUMP=100:bild.ppm` (Readback vor
`SDL_RenderPresent`, beschleunigter Renderer, weder SOFTWARE_RENDER noch AUTOSHOT):
P (nichts genommen, Spieler weit weg), P0 (Bits (9,54)/(9,55) im Spielstand), S und S0
(Spieler vor dem Tisch (−22664, −18450) rot 2048). Der Auswerter verlangt:

* Buch: mehr als 0 Pixel P ≠ P0 in der ROTEN Marke (x 146…158, y 120…132, `marken.txt`),
* Karte: mehr als 0 Pixel in der BLAUEN Marke (x 135…144, y 121…131),
* die Hüllenmitte jedes Props IN seiner Marke (Pixel außerhalb beider Marken gehören der
  näheren Markenmitte),
* in der Überschneidung Figur/Props (S0 ≠ P0 und P ≠ P0) mehr als 0 Pixel, und dort überall
  S = S0: die Figur vor dem Tisch liegt oben.

Gemessen (ctest, 960×720): Buch 525 Pixel (521 in der Marke), Mitte (153,83 ; 124,33),
2,54 px neben der Markenmitte; Karte 179 Pixel, Mitte (139,83 ; 126,33), 0,24 px;
Überschneidung 482, Figur obenauf 482, Prop obenauf 0. Laufzeit rund 60 s.

**Mutationsproben** (im Arbeitsbaum, `main.c` danach per `git checkout` zurück und neu gebaut):

| Mutation | `integration_r30_irons_tisch_laden` | `integration_r30_irons_tisch_bild` |
|---|---|---|
| M1: beide Klemmzeilen entfernt (die des Gegenprüfers) | grün | **rot**: Buch 0 Pixel, Karte 0 Pixel |
| M2: Klemme auf Schlüssel 0 (Props vor allem) | grün | **rot**: Überschneidung 482, Prop obenauf 482 |
| M3: nur die Viereck-Klemme entfernt (Buch = 6 Vierecke) | grün | **rot**: Buch 0 Pixel |

Der Wert der Klemme bleibt belegt durch die Masken ROOM1150.RDT/ROOM1151.RDT @0x006E8,
@0x006F4, @0x00714 (+4 = `57 00`, Tiefe 87). Der Mechanismus bleibt ein gekennzeichneter
Port-Zusatz ohne Original-Gegenstück. Er ist jetzt aber gegen Wegfall UND gegen Überziehen
verriegelt.

#### 9.6.2 Mangel 2 (gering): vor der Memory Card gab es das Buch

Ursache: Die zwei Rechtecke überlappten bei z −18780…−18149, Slot 7 (Buch) gewann. Gemessen
hatte das der Gegenprüfer mit der echten exe: bei z −18649 und −18720 öffnete der Leser.
Umgesetzt ist sein Vorschlag (2), die Teilung in der Mitte. Die Rechtecke bleiben eine
**Port-Wahl, keine Original-Adresse**. Sie folgen jetzt zwei Regeln aus Messwerten:

1. je Gegenstand 500 zu beiden Seiten seiner z-Lage (1000 tief = häufigste Größe, 71 von
   164 Item_aot_set, Zensus `r30_idw_zensus.py`),
2. wo sich die Bereiche überschneiden, teilt die Mitte zwischen den z-Lagen:
   (−18275 + −18649) / 2 = −18462.

Buch: Ecke −18462, Tiefe 687. Der Port rechnet wie `op_item_aot_set` Mitte = z + d/2,
halbe Tiefe = d/2 und deckt damit −18462…−17776. Karte: Ecke −19149, Tiefe 686, deckt
−19149…−18463. x und w bleiben am Telefon-Satz ROOM1150.RDT @0x00DA6 (x −24000, w 1000).
Die Prüfung läuft über den Punkt 620 voraus (@0x80042bd0).

* `unit_r30_irons_tisch` Teil Z: auf der Standlinie (Prüfpunkt x −23284, z −19400…−17600)
  liegen 0 Punkte in beiden Zonen, 687 nur im Buch, 687 nur in der Karte. Teil D drückt
  zusätzlich an der Kartenmitte −18649, bei −18720 und an der Grenze −18463 (jeweils Karte,
  Modal 0x21), an der Buchmitte −18275 und an der Grenze −18462 (jeweils Leser), dazu ROOM1151
  an der Kartenmitte. **Mutationsprobe:** Mit den alten Rechtecken zeigt der Test 14 Fehler,
  darunter Karte bei −18649 → Modal 0x00 (der Leser) und 632 Prüfpunkte in beiden Zonen.
* **Echte exe** (`r30_idw_nb_abnahme.sh druck`, Viereck in F400, Stand (−22664, z), Blick −X):

| z | −18900 | −18800 | −18720 | −18649 | −18463 | −18462 | −18275 | −17950 |
|---|---|---|---|---|---|---|---|---|
| Ergebnis | Karte, Modal F401 | Karte | Karte | **Karte** (vorher Buch) | Karte | Buch, Leser F411/412 | Buch | Buch |

Abzug: `irons-diary-welt/nb_druck_reihe.png` (Bild 420 je Standort).

#### 9.6.3 Mangel 3 (gering): Lage B stand in keiner Messdatei

Ursache: (−23813, −18280) mit rot_y 2944 mischte zwei Bestimmungen (Hauptachsen ≈ (−23815,
−18277), Einzelmerkmale (−23804, −18294)), und das ohne Regel. Jetzt gilt EINE Messung mit
EINER Regel. `r30_idw_klemmbrett.py` gibt zusätzlich das Mittel über die vier Schwellen
110/120/130/140 aus: Mitte = Mittel der Mitten, Achse = Mittel der langen Achsen, normiert.

    LAGE B = Mittel der vier Schwellen: Mitte (-23813.07, -18275.22) -> (-23813, -18275)
             | lange Achse Welt (-0.9804, -0.1972) -> rot_y = 2942.60 -> 2943

(Datei `build/r30_irons-diary-welt/klemmbrett_lage_b.txt` im Arbeitsbaum; die Messdaten
liegen im Hauptbaum, `R30_IDW_AUS` lenkt `r30_idw_geom.py` dorthin um.) **Neue Lage B:
(−23813, −1533, −18275), rot_y 2943.** Das sind 5 Einheiten und 1 Schritt neben dem alten
Wert. Die Buchmitte projiziert auf (153,69 ; 124,47), 2,36 px neben der Markenmitte und in
der Marke (Teil M). Im Framedump liegen 521 von 525 Pixeln in der roten Marke. 4 Pixel fallen
in die 320er-Spalte 159 direkt neben der Marke, das ist der Rand des Buchs auf dem gemalten
Klemmbrett. Die Hüllenmitte liegt in der Marke.

Die Yaw der Karte (3072) steht jetzt wörtlich als „Port-Wahl, keine Original-Adresse" im
Kopf. Unter der blauen Marke ist nichts gemalt, woran sich eine Drehung messen ließe. Gemessen
ist nur die Tischachse (Standlinie x −22664 über z −19400…−17600).

#### 9.6.4 Messfehler der ersten Abnahme: die Pixelmitten-Konvention

Die Marken (`marken.txt`, rot (152,5 ; 126,5), blau (140,0 ; 126,5)) und die Projektion
160 + H·vx/vz rechnen in stetigen 320er-Koordinaten: Pixel p deckt [p, p+1). Der Auswerter
der ersten Abnahme (`r30_idw_bau_auswertung.py`) und `diff.py` des Gegenprüfers rechneten
ein 960er-Pixel c als (c + 0,5)/3 − 0,5. Das ist ein halbes Pixel Versatz in x UND in y.
Korrigiert auf (c + 0,5)/3. Die Werte aus 9.4 berichtigt (alte Lage): Buch-Mitte
(153,33 ; 124,33) = **2,32 px** statt 2,69; Karte (139,83 ; 126,33) = **0,24 px** statt
0,94. Die Karte liegt also so genau auf der Marke, wie es die Projektion (0,02 px) erwarten
lässt.

#### 9.6.5 Abnahme erneut, am gebauten Spiel (Stand dieser Nachbesserung)

`r30_idw_nb_abnahme.sh` (Läufe `build/r30_irons-diary-welt/bau/nb_*`), Auswertung
`r30_idw_nb_auswertung.py` (Ausgaben `build/r30_irons-diary-welt/nb_auswertung_*.txt`).
Framedump 960×720, beschleunigter Renderer. Das Nullbild ist derselbe Lauf mit
(9,54)/(9,55).

| Messung | Soll | Ist |
|---|---|---|
| Cut 2 Tür-/Sprung-Weg, Pixel Buch / Karte (F400/500/600) | > 0 | **525 / 179** (alle drei Bilder gleich) |
| Cut 2 Mitte Buch → rote Marke (152,5;126,5) | in der Marke | (153,83 ; 124,33), **2,54 px**, 521 von 525 in der Marke |
| Cut 2 Mitte Karte → blaue Marke (140,0;126,5) | ≤ 1 px | (139,83 ; 126,33), **0,24 px**, 179 von 179 in der Marke |
| Mittel-RGB Cut 2 Buch / Karte | Licht byte-true | (46,35,25) / (52,41,37) |
| Cut 6 Pixel Buch / Karte | > 0 | **7827 / 3233** |
| Mittel-RGB Cut 6 Buch / Karte | Licht byte-true | **(17,16,8) / (17,17,10)**, s. 9.5 Punkt 1 |
| Spieler vor dem Tisch (−22664,−18450) rot 2048, Cut 2 | Figur über den Props | F400/500/600: Überschneidung 489/487/489, **Figur obenauf alle, Prop obenauf 0**; 215…217 Prop-Pixel außerhalb der Figur bleiben sichtbar |
| Karte aufheben an der Kartenmitte (−18649) | Modal 0x21, Menge 3, Prop weg | F401 `modal=1`, Bestätigen F520 → F521 `flag55=1 zone8=0 prop6=0 karte_menge=3` |
| Karte weg im Bild (Stand −18900, Cut 6) | 0 im Kartenfeld | Kartenfeld (3233 Pixel) F390 3233 abweichend → F650 **0** |
| Buch aufheben an der Buchmitte (−18275) | Leser sofort, nach Schließen + Meldung weg, FILE-Liste | F411 Menü, F412 Leser (`liste0=0`); KREUZ F540, Bestätigen F660 → F661 `flag54=1 zone7=0 prop5=0`, F668 Menü zu; Buchfeld (7827) F390 7827 → F680 **0**; START+R1 → F820 FILE-Liste „Irons Diary" (`nb_diary_aufheben.png`) |
| Lade-Weg ROOM1150, nichts genommen | Props da, Zonen aktiv | Boot-Weg obj 5/6, F0 `zone7=1 zone8=1 prop5=1 prop6=1`, Cut 2 **704** Pixel (525/179 wie am Tür-Weg) |
| Lade-Weg ROOM1150, beides genommen | fehlen | F0 `flag54=1 flag55=1 zone7=0 zone8=0 prop5=-1 prop6=-1` |
| Lade-Weg ROOM1151 (Elza), nichts / beides genommen | wie 1150 | 704 Pixel, gleiche Hüllen / fehlen |
| nach dem Laden aufheben, ROOM1150 und ROOM1151 | wie am Tür-Weg | Karte (−18649): F101 Modal, F221 `karte_menge=3 prop6=0`; Buch (−18275): F111/112 Leser, F361 `flag54=1 zone7=0 prop5=0 liste0=0`. Das gilt in beiden Räumen. |
| Bestand ROOM1150 ohne Props (Nullbilder gegen die exe von de5ae65a) | 0 | Cut 2 **0**, Cut 6 **0**, Spieler vor dem Tisch **0** (F400, F600) |
| Bestand andere Räume (gegen de5ae65a und gegen den ersten Bau) | 0 | ROOM1140 / 1100 / 1110: **0** Pixel (F400, F600) |

Abzüge (`analysis/befunde_runde30/irons-diary-welt/`): `nb_cut2_marken.png`, `nb_cut6.png`,
`nb_spieler_vor_tisch.png`, `nb_druck_reihe.png`, `nb_karte_aufheben.png`,
`nb_diary_aufheben.png`.

#### 9.6.6 Stand der Konstanten ohne Original-Adresse (Liste des Gegenprüfers)

| Konstante | Stand |
|---|---|
| `RE15_IRONS_DIARY_X/Z` | (−23813, −18275): Messwert mit Regel (9.6.3), Port-Wahl der Lage |
| `RE15_IRONS_DIARY_ROT_Y` | 2943, aus derselben Messung (Mittel der langen Achsen) |
| `RE15_IRONS_KARTE_ROT_Y` | 3072, Port-Wahl. Gemessen ist nur die Tischachse, nicht die Drehung (7.1). Unverändert. |
| `…_RECT_Z/_D` | Port-Wahl mit zwei Regeln aus Messwerten, keine Überlappung mehr (9.6.2) |
| Tiefen-Klemme | Wert belegt (@0x6E8/@0x6F4/@0x714), Mechanismus Port-Zusatz. Jetzt verriegelt (9.6.1). |
| `RE15_IRONS_KARTE_MENGE` 3 | RE2-Vorbild ROOM1060.RDT @0x00C24, mit dem Satz, warum RE1.5 nicht maßgeblich ist. Unverändert. |

#### 9.6.7 Suite und Offenes

* **Suite: 370 von 370** (369 des ersten Baus + der neue Riegel; ctest ohne -j, ohne Wiederholung, Rückgabe 0; Bild-Riegel 60,5 s). `integration_r30_irons_tisch_bild` ist angemeldet über
  `probes/r30_irons-diary-welt.cmake`. `tests/unit/CMakeLists.txt` und `RE15_MIN_TESTS`
  sind nicht geändert (Vorgabe).
* Der neue Bild-Riegel startet die echte exe viermal (etwa 60 s). Wie die anderen GUI-Haken
  kann er unter Last beim Fensteraufbau abreißen. Er bricht dann mit einer klaren Meldung ab
  („kein debug.log", „kein Framedump") und nicht mit einem falschen Bildbefund. Rot heißt dann
  zuerst: einzeln wiederholen.
* Cut 6 bleibt wie verlangt byte-true dunkel: Mittel-RGB (17,16,8) / (17,17,10). Der Leser
  zeigt dasselbe Buch hellbeige. Das wird dem Nutzer auffallen (Hinweis des Gegenprüfers,
  Risiko 1). Eine Änderung hier wäre eine Licht-Entscheidung, keine Nachbesserung.
* Nicht gemessen: Hörprobe (alle Läufe `RE15_NOAUDIO`), PSX-Ziel, Android-Paket (neue
  Quelldatei `engine/src/irons_tisch_1150.c` → dort frisch konfigurieren, GLOB-Cache).
