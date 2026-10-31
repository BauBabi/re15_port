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
| 0x0B | GRAB-THROW | **27** | Griff, ein Bild je Tick, Gewicht 0x200 | B[15] @0x80105254, a3 @0x801053b0 |
| 7 | FLINCH | **8 ab Bild 5** | Zucken, ein Bild je Tick, Gewicht 0x200 | @0x80107224, a3 @0x80107244 |
| 0x0D/0x10 | EMERGENCE | **10** | Auftritt nach dem Modellwechsel | T0[6] @0x8010153c |
| 9, 0x0C | Kollaps | entfaellt | Rueckstoss-Szene mit Clip 8 | birkin_formen.c |
| 0x0E | RUN-OFF | entfaellt | in 5090 nicht erreicht | |
| 0x14 | MUTATION | bleibt EM30 | laeuft vor dem Modellwechsel | @0x80118014 |
| neu | | **5 rueckwaerts** | Weichen bei dist < 2000, Gewicht 0x80, Anlauf mit halbem Tempo solange +0x14e >= 21 | B[8] @0x8010419c, a3 0x10080 @0x8010427c-84, @0x80104254-70 |

⛔ Berichtigung: Die erste Fassung las a3 von FUN_8002959c als Takt ("doppelter Takt" bei 0x200, "halber Takt" bei 0x80). Das war falsch. a3 ist das
Ueberblendgewicht je +0x14e-Schritt (`andi v0,a3,0xffff` / `mult v0,t3` @0x800296b8-bc), Bit 16 die Richtung (@0x800295b8-c4); das Bild laeuft in jedem
Aufruf um genau 1 (@0x80029b28-34), und jeder Zustand ruft genau einmal je Tick. Griff 100 Ticks, Auftritt 126, Zucken 25, Weichen 40.

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
| Schaden | RE2 15/20/70 von 200 HP | Hieb/Biss 10, Rennen 5 (0x30-Rollen @0x801177f0, @0x80118b1c); Griff 35, ab 86 HP so viel, dass 50 bleiben (RE2 halbiert, @0x801052a8-b4) | Port-Spieler rechnet mit 100 HP. Griff und Biss toeten, Tod = hp < 0 wie beim G5 |
| Griff | Griff beim Eintritt, Opfer-Spur | Griff bei Bild 8 @0x80105408, Spieler-Haken cmd 6 | Abweichung, s. 6 |
| Kasten | RE2 Versatz 400, Halbmass 1100 | 0x30-Kasten 1000/1440, Versatz 800 | Gestaltung |

## 4. Tests

`tests/unit/test_birkin_bahnsteig.c` (neu), `test_birkin_formen.c`, `test_birkin_szenen.c`. Geprueft: Asset gegen RE2 (Hierarchie, EDD, Topologie,
TIM), tiefster Punkt aller benutzten Clips bei y = 0 (+-25, gemessen 15), Kuppelhoehe 2995, Auftritt, Gang (gemessen 2657 Einheiten je Durchlauf, Spur 2666), Griff
packt, Hieb trifft an den EDD-Bildern, Weichen statt Schieben, Zaehler < 12 ohne Zucken, Rueckstoss, G5. **Leon (5090) und Elza (5091).**

Nebenbefund: `integration_r35_zgirl_5090` war seit a48c4d4f0 (Beta-Formen) rot und damit nicht Folge dieser Spur: der Test erwartet den Spawn `type=0x36`, seither bleibt er 0x30. Er bekam dieselbe Umgebung wie die Unit-Tests des alten Ablaufs (`RE15_ENDKAMPF_NUR_G5=1`, tests/unit/CMakeLists.txt) und ist gruen. Drei Pins (`unit_r17_birkin_1zu1_pin`, `unit_r18_birkin_restluecken_pin`, `unit_r20_birkin_push_pin`) uebersprangen sich aus demselben Grund still ("FEHLT: kein Boss"); sie stehen jetzt in derselben Liste und laufen.

Echter Ablauf im Fenster, beide Szenarien: EM30 -> Clip 20 -> Form -> Griff, Gang, Biss -> Rueckstoss -> G5. Vor dem Weichen schob die Form Leon im Gang von x 6500 bis 12700
durch den Wagen, danach pendelt der Abstand 2030-2350.

⛔ Berichtigung: "Schieben behoben" galt nur fuer den Gang. Im Rennen fehlte das Abdrehen nach dem Treffer, der Koerperschub trug Leon weiter rund 9900 Einheiten
(Abnahme 1 M11). Behoben in Abschnitt 8.

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
- **Griff:** Griff bei Bild 8 mit dem Rueckstoss-Haken statt der G4-Opfer-Sequenz; Leon fliegt, statt von der Kreatur gehalten zu werden. Toedlich laeuft er ueber cmd 3 (Standard-Tod), RE2-cmd 6 Variante 2 @0x801052cc-d0 ist nicht gebaut.
- **Treffer-Handler:** Die Form spielt die Bodenphase, RE2 nimmt dort Handler 2 @0x80107280 (kein Zucken, 25 % Absprung 0xb01/0x501, +0x238 += 20, se(0xd)). Der Port behaelt das Zucken ab Zaehler 12 aus Handler 1 @0x80106d7c und se(2), weil Sub 5/11 nicht gebaut sind und Id 13 in Bank 24 leer ist (FFFFFFFF). Blut kommt aus Handler 2.
- **Leon im Vorderkoerper:** Steht Leon vor der Form, verschwindet sein Oberkoerper in ihr. Das ist das RE2-Verhalten (Abnahme 1, verworfen), bleibt so.

## 7. Was das volle G4-Overlay noch braeuchte

Nicht verlangt, hier als Bestandsliste: Phase 1 mit Clip 2 samt Bodenlage (Sub 1/7), Verwandlungskette Clip 11/12 (Sub 0), Wandsprung und Laufen an Waenden (Sub 5/6/16/17, Clips 6/7/13/14/16), Fressen am toten Spieler (Sub 10, Clip 18), Sprung und Sturz (Sub 11, Clips 19/20), Klammern (Sub 12, Clips 21/22/4; Sub 13, Clips 23/25/9), Tod Clips 17/28, Griff mit Opfer-Bank Paar 3 und Spielerspur FUN_80015cb8, Morph-Block dir[0], Partikel 0x8001bf10 ausser dem Treffer-Blut (gebaut, Abschnitt 8) und Schuetteln 0x80039514/0x8003947c, Blick-Nachfuehrung 0x8010a58c (ruft wie beim G5 FUN_80017fdc), Wegziel 0x8004aab8 (+0x1c4).

## 8. Nachbesserung nach Abnahme 1 (`R_abnahme_1.md`, M1 bis M17)

Riegel: `tests/unit/test_birkin_bahnsteig.c`, `tests/unit/test_birkin_szenen.c`, `integration_r41_birkin_pistole`. Der Riegel-Satz lief zuerst gegen den unveraenderten Code:
124 FAIL-Zeilen, jeder Mangel rot (Messung vorher). M9, Form-HP, Sub 10 und der Integrationslauf wurden nach dem Fix einzeln gegengeprueft: Fix heraus, rot.
Laeufe: echtes Fenster, Exe-Kopie, kein AUTOSHOT, kein SOFTWARE_RENDER, ausser wo der ctest-Lauf genannt ist.

| Nr. | Fix | Beleg | vorher | nachher |
|---|---|---|---|---|
| M1 | Eintritt behaelt den Trefferzustand, Auftritt ist der gesicherte Zustand | @0x80107180-8c | Riegel rot: `state 1 cd 0`, 21 Schuesse ohne Wirkung | Lauf m1_b: Tausch F1296, Treffer F1297 `hit=01 hurt9`, F1306 frei, F1345 HP 135 |
| M2 | Bit 0 je Bild in Phase 1, Phase 2 loescht | STAGE3 @0x8011a39c, @0x8011a3a8, @0x8011a3c0; STAGE5 @0x8011abb0, @0x8011abbc, @0x8011abd4 | HP 155 bis Laufende | s. 8.1 |
| M3 | Tor zu, sobald Sub 6 laeuft (+0x1dd & 8) | @0x801164e8, @0x80118054, @0x801180b0 | Riegel 124 Neustarts, 3000 Ticks | Pistole, Flinte (Id 8), GL (Id 15): je ein Start, Leon F1124 bis F1304 |
| M4 | Riegel im Zweig Tod -> Mutation | @0x8011a458, @0x8011a54c-50 | Riegel `hit=01` | Rakete m4_rak: F413 HP -700, F414 `hit=00 hurt9`, Form, Rueckstoss, G5 |
| M5 | ein bb_anim je Tick | @0x800296b8-bc, @0x80029b28-34 | Auftritt 63, Griff 51, Griff greift nach 4, Weichen 20 Bilder ohne +1 | Auftritt 126 (F280-405), Griff 100 (F643-742), greift nach 8, Zucken 25, Weichen 40 Bilder je +1 |
| M6 | Gewicht je Aufruf: 0x100 Gang, Hieb, Biss, Rennen; 0x200 Griff, Auftritt, Zucken; 0x80 Weichen | a3 @0x80102a34, @0x80101b34, @0x80104ec0, @0x80102d08, @0x801053b0, @0x801015d4, @0x80107244, @0x80104284 | Rate 0, Weichen 40 von 40 Ticks ohne Pose-Aenderung, Gang 7 von 30 | Weichen 0 von 40, Gang <= 1 von 30 |
| M7/M8 | Richtung je Aufruf statt Aktor-Bit | @0x80104248, @0x8010427c-84, @0x800295b8-c4 | Sprung 2022 je Tick, danach vorwaerts | Riegel <= 60; m1_b Treffer F1633 und F1681 im Weichen: Schritt 54 und 18, Bit bleibt |
| M9 | Szene schaltet Zonen ab wie Cut_chg, frei wie Cut_old | @0x800402d4, @0x8004032c, Gate @0x8001cce0-e4, RVD @0x490/@0x4b8/@0x4cc | HALTEN F1410-1477 und F1642-2138 | x 14615: Einzel-HALTEN F430, F496, 8 im Lauf; x 2741-3217 im G5-Auftritt: kein HALTEN ab F496, 6 im Lauf |
| M10/M17 | Spur neu aufsetzen, Schub am Tick-Ende | FUN_80015e7c @0x80101c4c, @0x80101c60, @0x80102348 | Schritt 544 bis 701 | Wechsel F535: Schritt 0, groesster Schritt im Hieb 284 (Summe dx+dz) |
| M11 | Kein Lenken nach dem Treffer, Ruck und 6 x Abdrehen | @0x80102b90-94, @0x80102f8c, @0x80102f68, @0x80103014 | Leon 9896 Einheiten getragen, 2 Treffer | f_fern: ein Treffer F433, Leon 13775 -> 14699, Form faehrt bei z -22718 vorbei |
| M12 | Zuwachs je Waffe ueber die RE2-Id, 19 Eintraege | @0x80107130-4c, @0x8010b0fb bis 4 @0x8010b10d | jede Waffe im Gang +3, im Biss +20 | Waffe 3/4/7/8/12/13/19/20 = 3/3/10/5/2/8/4/10; Pistole bricht den Biss nicht ab |
| M13/M16 | Vier Blutspawns Handler 2, Anker Teil 10, nicht bei +5 = 0x10/0xc | @0x80107414-1c, @0x80107390-b8, @0x80107454/94/b8/f8 | 0 Spawns | f_schuss: 4 Effekte Id 0 je Treffer ueber 11 Bilder (F400-410 ...), Riegel 4 |
| M14 | Keine Klemme fuer die Form, Griff RE2-Schaden halbiert, Hieb-Ende wie @0x80101b60 | @0x801052a8-d0, @0x801024b4, @0x8004027c-8c | HP 1 fuer immer | f_nah: Griff bei HP 10 -> -1, Clip 7, cmd 7, YOU DIED, Titel (Video) |
| M15 | Griff stellt den Spieler mit dem Gesicht zur Form | Haken [0] @0x8011b018, @0x8011b3f0, Richtung @0x80105358-68 | Abstand 2463 -> 970 | f_griff: Gier -537 -> 2129, x 8650 -> 15191, Form bei 6388 |

**Zwei Fixe ausserhalb der Liste**, beide Folge der Messung:

- **Eigene HP der Form.** Mit M3 kam die Form unter Dauerfeuer mit dem Mutationsrest an: Lauf a1_leon HP 30, tot nach 49 Bildern noch im Auftritt, kein Angriff. RE2 schreibt dem G4 im Ctor eigene HP (+0x156 @0x8010067c-b8, 1000/900/1100), wie dem G5 (@0x801003fc). Der Eintritt setzt deshalb 150 (0x96 @0x80118094, das Port-Mass der Waffen). Lauf a2_leon: Form lebt F1304 bis F1627, Griff trifft.
- **ROOM3071, Elzas Kampf.** Birkin blieb nach dem Auftritt hinter dem Tor (SCA 17, x -27055..-26505), ohne Sichtlinie: 0 Treffer bei rund 90 Schuessen, der Kampf war nicht zu gewinnen (Lauf b_3071). Ursache: Sub 10 lief mit 10 Einheiten je Tick ohne den Fuss-Vortrieb FUN_8011b5c4(1,1) (@0x8011960c, Modus 0 @0x80119934), und die Wandklemme griff, obwohl INIT sie abschaltet (+0x0 |= 8 @0x801168d0, FUN_8003b0a4 @0x8003b134-40, geloescht @0x80119b28-34). Gebaut fuer Sub 10: Birkin endet bei (-24079,-10917) in der Halle. WALK & Co. behalten den additiven Teil (Block-Kopf).

### 8.1 Startwaffe, Dauerbeschuss, kein Messhaken (M2)

Debug-Sprung, Ausloeser wie die Tuer, Id 3, RE15_GIVE nur fuer Patronen. Gehalten = Zielen + Feuer durchgehend (alle 22 Bilder), getippt = 0,3 s an, 0,1 s aus (Quadrat-Flanken zaehlen als Ruetteln).

| Raum | Figur | Feuer | Ergebnis |
|---|---|---|---|
| 3070 | Leon | getippt | Tod Birkin F1612 |
| 3070 | Leon | gehalten | Leon stirbt F1310 am Wurf (cmd 6 STAGE3, Wurf-Tod 0x8011b1a8, Original): ohne Ruetteln kommt er nicht aus dem Griff |
| 3071 | Elza | gehalten / getippt | Mutation einmal, Tod Birkin F1730 / F1746 |
| 5080 | Leon | gehalten | Tod Birkin F1697 |
| 5081 | Elza | gehalten | Tod Birkin F1698 |
| 5090 | Leon | gehalten | Mutation F1124, Form F1304, Rueckstoss F1627, G5 F1784 |
| 5091 | Elza | gehalten | identisch |

Vorgabe-KI und `RE15_AI_FLAVOR=re15` liefern in 5090/5091 dieselben Bilder. `integration_r41_birkin_pistole` faehrt 5090 und 5091 mit Software-Renderer als Riegel (165 s).

### 8.2 Offen

- **Verworfen und unveraendert:** Leon steht im Vorderkoerper der Form, das ist RE2 (Abschnitt 6).
- **G5 im gehaltenen Dauerbeschuss:** Leon und Elza bringen ihn bis auf HP 30, dann greift bei F2879 der Masse-Kill (dist < 6000, u >= 9001, 500 Schaden @0x80103ea0-ec4). Es fehlen zwei Treffer; die Pistole erreicht den G5 erst ab Tick 300. Der G5 ist nicht Teil dieser Spur und unveraendert; ob er mit der Startwaffe ohne Ausweichen fallen muss, ist damit nicht geloest.
- **G5 blutet nicht:** `blut=n/0/0/0`, Wache `g->ph == 0` (enemy_ai_boss_g5.c:1612). Nebenbefund der Abnahme, nicht bearbeitet.
- **EM30:** Die 0x30-KI setzt weiter keine anim_blend_rate; an jedem Clip-Eintritt mit frac 15 steht die Pose 7 Bilder (dieselbe Wirkung wie M6). Fuss-Vortrieb nur in Sub 10, WALK bleibt langsamer als das Original.
- **Treffer in der Mutation:** zaehlen wie im Original (Revive @0x8011a43c), die Form bringt danach trotzdem 150 mit. Die Schuesse in Sub 6 bleiben also ohne Wirkung auf den Kampf.
