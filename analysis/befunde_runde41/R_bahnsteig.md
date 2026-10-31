# Spur R — Bahnsteig-Birkin (Nachtrag 38)

Auftrag (Wortlaut Nutzer): "Die von dir gebaute neue Birkin Version auf dem Bahnhof sieht nicht ansatzweise so aus, wie auf
den Bildern, nach dem du diese bauen solltest. [...] Es ist eine eigene Form und braucht dann auch eigene Bewegungsanimationen."

Belege: `analysis/befunde_runde41/R_belege/`. Commit, Dateiliste und ctest-Summe: Abschlussmeldung der Spur.

## 1. Messung vorher

### 1.1 Das Video ist eine Diashow

`Biohazard 1.5 Final Build.mp4`, 113.6-118.0 s, Bild fuer Bild verglichen (mittlere Differenz je Folgebild): harte Schnitte bei
Videobild 4, 28, 52, 76, 100, 124, also alle 24 Bilder (0.8 s). Dazwischen d = 0.00 bis 0.47. **Es gibt keine Bewegung der
Kreatur im Video**, nur zwei Standbilder: 1:54.53-1:55.30 und 1:56.14-1:56.90. Bewegungsablaeufe lassen sich aus dem Video
nicht ableiten; die Abnahme der Bewegung kann sich nur auf RE2 stuetzen.

### 1.2 Welcher Raum

Kontrollen zeigen, dass die Zuordnung trennscharf ist. SIFT + RANSAC gegen alle 532 Hintergruende Stage 3-6:

| Videobild | bester Schnitt | Inlier | naechster | Bewertung |
|---|---|---|---|---|
| Kontrolle 1:53.9 | ROOM3070 Cut 4 | 70 | 15 | klar |
| Kontrolle 1:55.6 | ROOM5090 Cut 1 | 100 | 21 | klar |
| Kontrolle 1:57.2 | ROOM6030 Cut 1 | 47 | 25 | klar |
| **Bild 2 (roter Gang)** | **ROOM5090 Cut 11** | **28** | 14 | **Treffer**, doppelt so hoch wie das Rauschen; auch NCC-Bestwert 0.216 |
| **Bild 1 (Bahnsteig)** | keiner | 13 | 13 | **kein Treffer**, Rauschniveau 10-15 |

**Bild 2 = ROOM5090 Cut 11**, RID (7626,-3256,-25360) -> (-1504,-986,-21970), derselbe Blick wie `bf_kamera(0x0B)`. Bestaetigt
im echten Fenster: Tuerrahmen, Streben, Deckenlampe liegen deckungsgleich (ECC 0.617). Die hellen Flecken im Video sind **Feuer**
(dieselbe Flammenstelle im Port), nicht Krallen. Die Frau im Kittel steht im Port an derselben Stelle: Typ 0x4D, Spawn @0x122A
`44 00 4d 40 ... 58 02 00 00 2c 9d` = (600,0,-25300). Der Video-Hintergrund ist eine andere Lichtfassung (rechte Wand mit hellen
Flaechen statt der Kisten): der Raum ist da, sein Stand weicht ab.

**Bild 1 = kein Schnitt der Disc.** Inhaltlich am naechsten: **ROOM5080** (roter T-1-Zug, Gelaender im Vordergrund, Vordach,
Leiter am Aufzug). Zug links plus Vordach rechts verlangt einen Blick, den kein Schnitt 0-5 hat. Die Kamera fehlt also, und die
Szene gibt es im Port nicht: in 5080 kaempft der EM30, die Form erscheint erst in 5090. Beides wurde nicht gebaut.

### 1.3 Groesse

⛔ Der erste Durchgang war falsch: ich hatte Menschen mit 1800 Einheiten angesetzt und die Form auf 0.75 verkleinert. Im echten Spiel
stand die Oberkante unter dem Kopf der Frau. Das Laufprotokoll zeigt den Kopfknochen des Spielers bei y = -2514, **ein Mensch ist
hier rund 2750 hoch**. Die Bild-Verhaeltnisse bleiben gueltig, die absoluten Zahlen nicht:

| Quelle | Verhaeltnis Kuppel / Mensch | absolut |
|---|---|---|
| Bild 1, Fusslinien Leon und Kreatur fast gleich (y 365 / 370), Kuppel 208 px gegen Leon 180 px je Bodentiefe | 1.17 | ~3200 |
| Bild 2, Kuppelscheitel auf dem Sehstrahl von Cut 11, y -2850..-3050 bei Tiefe 3500-7000 | 1.05-1.10 | ~2950 |
| **Form, Clip 5 (Mittel ueber den Gang)** | **1.09** | **2995** |

Verworfene Form und RE2 im selben Mass, tiefster Punkt = Boden:

| | Hoehe | Standbreite | Bauchfreiheit | Oberschenkel |
|---|---|---|---|---|
| EM30_BAHNSTEIG (verworfen), Clip 1 | 3154 | 2951 | 1324 | 498 x 429 |
| RE2-G4 roh, Clip 5 | 2907 | 2917 | 1113 | 914 x 438 |
| **Bahnsteig-Form, Clip 5** | **2989** | **2380** | **1194** | **1271 x 588** |

Der Fit des Umrisses aus Bild 1 gegen G4 (Clip/Bild/Richtung/Massstab) liefert IoU 0.60-0.65; die Richtung ist bei dieser Schaerfe
nicht eindeutig (Ruecken- und Vorderansicht geben dieselbe Silhouette), der helle Kopf fehlt aber im Bild, also Rueckansicht.

## 2. Bildbefund (Lesung geprueft)

Bestaetigt: grosser gerundeter dunkler Buckel, vier massive knotige Glieder, rosa-roter Fleck auf dem Rumpf. Praezisiert:

- "lange segmentierte helle Klaue" in Bild 2 ist kein Sonderteil: Clip 3 Bild 20 zeigt genau so einen langen Arm mit hellen Krallen schraeg
  aus dem Rumpf. Das ist ein Indiz, kein Beweis; die Bildqualitaet gibt mehr nicht her.
- Die Bilder zeigen die Kreatur **von hinten**. G4 von hinten (Clip 5) hat ohne jede Aenderung schon die Kuppel ueber bogenfoermigen
  Pfeilern, die dunkle Haut und die roten Stellen. Das Video-Violett bleibt Videoqualitaet, es gibt keine Tonung.

**Entscheidung: G4 ist die richtige Vorlage**, Massstab 1.0. Der Vergleich fordert drei Abweichungen: engerer Stand, dickere und knotige
Glieder, breiterer Rumpf. Alle drei sind Gestaltung und im Werkzeug so markiert.

## 3. Umsetzung

**Asset** `re15_port/shared_assets/RE2/BAHNSTEIG_G4.EMD` + `.TIM` (⛔ neue RE2-Dateien, Paket-Gate), Werkzeug
`re15_port/tools/birkin_bahnsteig.py`, reproduzierbar (Bytevergleich zweier Laeufe gleich). Quelle CDEMD0.EMS kind 0x34,
rec 3 / rec 2. Unveraendert: Hierarchie, EDD samt Flags, Topologie, UVs, TIM, Paar 2/3. Gestaltung: Dicke 1.35 mit Wulstprofil
(Mitte +30 %, Gelenk -30 %), unterstes Gliedstueck 1.05, Gliedwurzeln 10/14 Grad zur Mitte, Rumpf Z 1.15, Boden je Keyframe nachgefuehrt.

**Verworfen und entfernt:** `EM30_BAHNSTEIG.EMD`, Formeintrag `bahnsteig` in `tools/birkin_formen.py`.

**KI** `re15_port/engine/src/birkin_bahnsteig.c`, Einhaengung `re15_birkin_root`: traegt der 0x30 die G4-Bank (`re15_bb_bank_da`),
laeuft `re15_bb_tick` statt `re15_birkin_ai_tick`. Der EM30-Kampf, die Mutation, die Rueckstoss-Szene und der G5 bleiben. Sub-Nummern sind die
des Overlays (CDEMD0.EMS rec 1 Sektor 3238, 0xB2D4 Byte, @0x80100000; Tabellen @0x8010af60, @0x8010b020, @0x8010b068).

**Lader** `pc_enemy_load_bahnsteig` (platform/pc/main.c). Die Opfer-Bank bleibt EM30 (3 Clips 25/66/5), weil die Spieler-Haken
`birkin_opfer.c` darauf zugeschnitten sind.

**Ton:** G4-Spawn ROOM6170 traegt Ton-Id 0x26, Paar-Tabelle @0x800a7400 Zeile 24 = {0x26,0x00}, also ENEMSE-Bank 24, erste Haelfte.
Bildtoene aus dem EDD: FUN_80016028 @0x80016034-54 (Bit 27 aktiv, Id = Wort >> 28).

**Schatten:** 2300 x 1300 wie G4 (@0x801002cc, @0x80100300) statt des 500x600-Vorgabewerts.

**Korrektur:** `analysis/video_80prozent/BEFUND.md` Zeile 74 berichtigt.

### 3.1 Clip-Tabelle

Clip-Bedeutung aus dem Zensus aller Schreiber von +0x14c (Clip-Wort: Clip | Bild<<8 | Blende<<16), gerendert in `R_belege/g4_clips_a.jpg` / `_b.jpg`.

| 0x30-Clip | Rolle 0x30 | G4-Clip | G4-Rolle | Beleg |
|---|---|---|---|---|
| 1 | WALK, REPOSITION | **5** | Gehen, Bodenphase (Schritt-Ton Bild 43/73) | B[3] @0x801028dc |
| 3 + 0x0A | CLAW + Erholung | **3/4 + 9** | Hieb, Seite aus +0x16a, Doppelschlag, Nachhieb | @0x80101ad0, @0x8010b0f0, @0x8010224c |
| 4 | FAST BITE | **26** | Biss | B[14] @0x80104e84 |
| 5 | TACKLE | **0** | Rennen mit Ueberrennen | B[4] @0x80102b10 |
| 0x0B | GRAB-THROW | **27** | Griff, doppelter Takt | B[15] @0x80105254, @0x801053b0 |
| 7 | FLINCH | **8 ab Bild 5** | Zucken, doppelter Takt | @0x80107224, @0x80107244 |
| 0x0D/0x10 | EMERGENCE | **10** | Auftritt nach dem Modellwechsel | T0[6] @0x8010153c |
| 9, 0x0C | Kollaps | entfaellt | Rueckstoss-Szene mit Clip 8 | birkin_formen.c |
| 0x0E | RUN-OFF | entfaellt | in 5090 nicht erreicht | |
| 0x14 | MUTATION | bleibt EM30 | laeuft vor dem Modellwechsel | @0x80118014 |
| neu | | **5 rueckwaerts** | Weichen bei dist < 2000 | B[8] @0x8010419c |

Clip 2 (RE2-Hauptgang Sub 1) ist **nicht** uebernommen: die Vorderhaende reichen +1019..+1282 unter die Sohle (gemessen je Clip). RE2 hat
zwei Phasen; unter HP 700 wechselt es ueber die Kette 11 -> 12 -> 10 -> 5 in die Bodenform. Der Port nimmt die Bodenphase.

### 3.2 Reichweiten, Trefferbilder

| Wert | Original | Port (Massstab 1.0) | Beleg |
|---|---|---|---|
| Hieb | dist < 0xfa0, Kegel 512 | gleich | A[1] @0x801017a8 |
| Biss | dist < 0xce4, Kegel 824 | gleich | @0x8010243c |
| Griff | dist < 0xed8, Kegel 256 | gleich | @0x8010248c |
| Rennen | dist >= 0x1f41 | gleich | @0x801026a8 |
| Weichen | dist < 0x7d0, Kegel 1024 | gleich | @0x801017d4 |
| Trefferbilder Hieb | EDD Bit 16: Clip 3 Bild 26-30, Clip 4 Bild 14-17 | gleich | @0x80101cb8 |
| Treffer Hieb | Teil 18 / 13, Radius 1700, oder Versatz (0,600,0) Radius 1500 | gleich | @0x80101d38, @0x80101d50 |
| Treffer Biss, Rennen | Teil 10 + (1400,0,0), Radius 1500 | gleich | @0x80102eb4, @0x80104f88 |
| Trefferprobe | sqrt(dx^2+dz^2) < Radius, Y frei | gleich | FUN_80015834 @0x800158c0-f0 |
| Zucken | Zaehler nach Tabelle, ab 12 | gleich | @0x8010b0fb, @0x80107168 |
| Schaden | RE2 15/20/70 von 200 HP | Hieb/Biss 10, Rennen 5 (0x30-Rollen @0x801177f0, @0x80118b1c), Griff 10 | Port-Spieler rechnet mit 100 HP |
| Griff | Griff beim Eintritt, Opfer-Spur | Griff bei Bild 8 @0x80105408, Spieler-Haken cmd 6 | Abweichung, s. 6 |
| Kasten | RE2 Versatz 400, Halbmass 1100 | 0x30-Kasten 1000/1440, Versatz 800 | Gestaltung |

## 4. Tests

`tests/unit/test_birkin_bahnsteig.c` (neu), `test_birkin_formen.c`, `test_birkin_szenen.c`. Geprueft: Asset gegen RE2 (Hierarchie, EDD, Topologie,
TIM), tiefster Punkt aller benutzten Clips bei y = 0 (+-25, gemessen 15), Kuppelhoehe 2995, Auftritt, Gang (gemessen 2657 Einheiten je Durchlauf, Spur 2666), Griff
packt, Hieb trifft an den EDD-Bildern, Weichen statt Schieben, Zaehler < 12 ohne Zucken, Rueckstoss, G5. **Leon (5090) und Elza (5091).**

Nebenbefund: `integration_r35_zgirl_5090` war seit a48c4d4f0 (Beta-Formen) rot und damit nicht Folge dieser Spur: der Test erwartet den Spawn `type=0x36`, seither bleibt er 0x30. Er bekam dieselbe Umgebung wie die Unit-Tests des alten Ablaufs (`RE15_ENDKAMPF_NUR_G5=1`, tests/unit/CMakeLists.txt) und ist gruen. Drei Pins (`unit_r17_birkin_1zu1_pin`, `unit_r18_birkin_restluecken_pin`, `unit_r20_birkin_push_pin`) uebersprangen sich aus demselben Grund still ("FEHLT: kein Boss"); sie stehen jetzt in derselben Liste und laufen.

Echter Ablauf im Fenster, beide Szenarien: EM30 -> Clip 20 -> Form -> Griff, Gang, Biss -> Rueckstoss -> G5. Vor dem Weichen schob die Form Leon von x 6500 bis 12700
durch den Wagen, danach pendelt der Abstand 2030-2350.

## 5. Abnahme-Bilder (`R_belege/`)

| Datei | Inhalt |
|---|---|
| `vergleich_bild2_gang.png` | Video / Port entzerrt / Port roh, ROOM5090 Cut 11, gleiche Kamera, Rueckansicht |
| `vergleich_bild2_gang_flanke.png` | dasselbe, Flanke mit rotem Fleck |
| `vergleich_bild1_bahnsteig.png` | Video / Port ROOM5080 Cut 1 / auf Leons Bildhoehe skaliert. **Nicht dieselbe Kamera**, s. 1.2 |
| `streifen_gehen_hinten.png`, `streifen_gehen_vorn.png` | Clip 5 im echten Fenster |
| `streifen_hieb.png`, `streifen_rennen.png` | Clip 3, Clip 0 im echten Fenster |
| `birkin_bahnsteig_g4.png` | Ansichtsblatt der Form |
| `g4_clips_a.jpg`, `g4_clips_b.jpg` | alle 29 G4-Clips, unveraendert gerendert |

Alle Port-Bilder: echtes Fenster per gdigrab `hwnd=`, kein AUTOSHOT, kein SOFTWARE_RENDER, Spiel in Vollbildgroesse 640x480. ⛔ `title=` lieferte hier 320x240 schwarz,
`hwnd=` das richtige Bild. Die Posen stehen ueber den Messhaken `RE15_BB_POSE` fest (Kamera, Lage, Clip); der freie Ablauf ist mit `RE15_BIRKIN_HP_AT` + `RE15_BIRKIN_CUT` im echten Spiel geprueft.

## 6. Wo es noch abweicht

- **Bild 2:** Kuppel, Masse, Lage und Hoehe stimmen im Vergleich sehr gut. Das helle gebaenderte Glied zeigt im Video nach unten links, im Vergleichsbild nach unten rechts: Pose nicht exakt getroffen, es ist dieselbe Art Glied.
- **Bild 1:** nicht in derselben Kamera vergleichbar (Vergleich: Gang Clip 5 in ROOM5080 Cut 1). Im Video stehen die Glieder steiler und dichter unter dem Rumpf, die Kuppel ueberragt Leon um 17 %, die Form um 9 %. Die Vorderbeine sind im Port schlanker als im Video. Die knotigen hellen Baender sind im Video deutlicher als auf der G4-Textur.
- **Schatten:** auf dem orangen Boden von ROOM5090 wirkt der subtraktive Schatten dunkelrot; in der Groesse 2300 x 1300 faellt das auf (auch Leons Schatten ist dort rot). Das ist das vorhandene Schattenverhalten des Raums.
- **Haut:** G4 hat vorn den hellen Kopf mit Stacheln. Die Videobilder zeigen nur die Rueckseite, ueber die Vorderseite sagen sie nichts. Von vorn sieht man also reines G4.
- **Modellwechsel:** EM30 geht nach Clip 0x14 hart in die Form ueber (ein Bild), es gibt keine Ueberblendung, weil die Skelette verschieden sind.
- **Griff:** Griff bei Bild 8 mit dem Rueckstoss-Haken statt der G4-Opfer-Sequenz; Leon fliegt, statt von der Kreatur gehalten zu werden. Die Form kann dem Spieler wie die 0x30-KI nicht unter 1 HP schlagen (@0x80117810).

## 7. Was das volle G4-Overlay noch braeuchte

Nicht verlangt, hier als Bestandsliste: Phase 1 mit Clip 2 samt Bodenlage (Sub 1/7), Verwandlungskette Clip 11/12 (Sub 0), Wandsprung und Laufen an Waenden (Sub 5/6/16/17, Clips 6/7/13/14/16), Fressen am toten Spieler (Sub 10, Clip 18), Sprung und Sturz (Sub 11, Clips 19/20), Klammern (Sub 12, Clips 21/22/4; Sub 13, Clips 23/25/9), Tod Clips 17/28, Griff mit Opfer-Bank Paar 3 und Spielerspur FUN_80015cb8, Morph-Block dir[0], Partikel 0x8001bf10 und Schuetteln 0x80039514/0x8003947c, Blick-Nachfuehrung 0x8010a58c (ruft wie beim G5 FUN_80017fdc), Wegziel 0x8004aab8 (+0x1c4).
