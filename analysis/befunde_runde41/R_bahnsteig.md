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
| Schaden | RE2 15/20/70 von 200 HP | Hieb/Biss 10, Rennen 5 (0x30-Rollen @0x801177f0, @0x80118b1c); Griff 35, ab 86 HP so viel, dass 50 bleiben (RE2 halbiert, @0x801052a8-b4) | Port-Spieler rechnet mit 100 HP. Griff und Biss toeten (a1 = 0), Tod = hp < 0. Der G5-Biss ist a1 = 1 und toetet nie, s. 9.4 |
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
| M11 | Kein Lenken nach dem Treffer, Ruck und 6 x Abdrehen | @0x80102b90-94, @0x80102f8c, @0x80102f68, @0x80103014 | Leon 9896 Einheiten getragen, 2 Treffer | f_fern: ein Treffer F433, Leon 13775 -> 14699, Form faehrt bei z -22718 vorbei. ⛔ Nur Mitte und Sueden, die Wand-Spur war noch offen, s. 9.1 |
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
- ⛔ Berichtigt in Abschnitt 9: **G5 im gehaltenen Dauerbeschuss.** Hier stand, der Masse-Kill bei F2879 sei das Original (dist < 6000, u >= 9001, 500 Schaden @0x80103ea0-ec4). Falsch: das Original frisst nur den Spieler mit HP <= 0 (`bgtz` @0x80103e80), die X-Schwelle gehoert zu Pfad 1. Der G5 faellt jetzt unter der Pistole, s. 9.5.
- ⛔ Behoben in 10.4: **G5 blutet nicht.** `blut=n/0/0/0`, die Wache `g->ph == 0` las die KI-Phase statt +6 des Trefferzustands.
- **EM30:** Die 0x30-KI setzt weiter keine anim_blend_rate; an jedem Clip-Eintritt mit frac 15 steht die Pose 7 Bilder (dieselbe Wirkung wie M6). Fuss-Vortrieb nur in Sub 10, WALK bleibt langsamer als das Original.
- **Treffer in der Mutation:** zaehlen wie im Original (Revive @0x8011a43c), die Form bringt danach trotzdem 150 mit. Die Schuesse in Sub 6 bleiben also ohne Wirkung auf den Kampf.

## 9. Nachbesserung nach Abnahme 2

Sieben Meldungen, davon zwei gleich (Kamera nach dem Auftritt). Jeder Punkt lief so: Lauf vorher im echten Fenster, Disassembly, Fix, Lauf nachher. Laeufe: Exe-Kopie im eigenen Ordner, gdigrab hwnd, kein AUTOSHOT, kein SOFTWARE_RENDER, Eingaben wie in der Abnahme. Die Rennen liefen vor dem letzten G5-Fix; Form und Wandklemme hat der nicht beruehrt. Bilder: `R_belege/nb2_rennen_nord.jpg`, `nb2_kamera_ost.jpg`, `nb2_g5_tod.jpg`.

### 9.1 Rennen: Nordspur (M11) und Diagonale

Beide Meldungen sind ein Fehler. Der G4-Wurzel-Schwanz schiebt die FORM aus dem Spieler: `lw a0,-492(a0)` = 0x800CFE14 = Listenkopf = Spieler, `jal 0x80034d0c` @0x8010052c mit a1 = Form, danach erst die Wandklemme `jal 0x8003567c` @0x80100554. FUN_80034d0c bewegt den zweiten Partner (`sw v0,56(s1)` / `sw v1,64(s1)` @0x800350c8-cc). Der Port hatte nur den Spieler-Pass. Trug allein der Spieler die Ueberdeckung, nahm ihn die Form an der Wand mit: seitlich kann er nicht ausweichen, der Rest des Schubs geht in Laufrichtung (gerechnet: Gleichgewicht dx ~ 1120 bei Tempo 200, R 1450, dz 512). An der Diagonalen Box 30 versetzte derselbe Schub ihn bei d 914 ueber die Linie, die Klemme loeste auf der falschen Seite. Die Gegenpeilung aus der Meldung ist Original (FUN_80015558 @0x8001559c-ac), sie ist nicht die Ursache.

Gebaut: `re15_enemy_selfpush_from_player` in `re15_birkin_root`, nur fuer die Form. Eine eigene Klemme gegen die Kollision braucht es nicht mehr, der Schub nach Abnahme 1 (4d4a1b0cc) laeuft schon in jedem Zweig.

Weg Leons vom ersten Treffer bis +40 Bilder:

| Lage, Form rennt | vorher | nachher |
|---|---|---|
| z -22300 nach Osten | 7472 (Abnahme 7455) | 993 |
| z -22800 | 3820 (Abnahme) | 1805 |
| z -23300 | 1696 (Abnahme 1548) | 1738 |
| z -23800 | 755 (Abnahme) | 888 |
| z -24300 | 811 (Abnahme) | 503 |
| z -24800 | 1542 (Abnahme) | 498 |
| rein per Eingabe, Leon (12384,-22206) | 7743 (Abnahme) | 1748 |
| z -24700 nach Westen | 7171, Ende (2874,-27668) in Box 22 | 754, Ende (7750,-24734) |
| z -23550 nach Westen | | 1695 |

Zweiter Treffer im selben Anlauf: bleibt moeglich (Nordspur F739, F762). Das Original sperrt ihn nicht, das Rennen prueft nur Tempo >= 201, +0x16a & 0x40 und den freien Spieler (@0x80102e88-ee8), und die Form steht an der Wand hinter Leon, bis das Clip-Ende den Lauf beendet (F745 bis F767).

### 9.2 Pose am Clip-Ende

FUN_8002959c posiert vor dem +1 und nullt +0x14d erst danach (`sb zero,333(s2)` @0x80029b48, Rueckgabe 1 @0x80029b4c). Der Port zeichnet anim_frame nach dem Vorschub. bb_anim haelt jetzt das letzte Bild, ausser in den Schleifen Clip 5 und Clip 0, wie sm_anim_halten (spiderman_r38.c). Groesste Knochen-Verschiebung im Ende-Tick, Riegel: Biss 1752 -> 0, Griff 1561 -> 0, Hieb 899 -> 0, Auftritt 320 -> 0. Die Abnahme mass mit ihrer Sonde Hieb 4283, Griff 2231, Biss 1885, Zucken 1070.

### 9.3 Kamera (drei Meldungen, zwei Ursachen)

**Schnitt am Szenenende.** Der feste Cut 12 liess Leon oestlich von x 15400 ohne Zone und ohne Bild. Die RVD-Zonen sind Streifen, die nur aus dem passenden Schnitt ausloesen (@0x490 bis @0x530). Cut_old (`lbu a0,0x3f7b(a0)` @0x8004033c, `sh a0,0xfe4(at)` @0x80040364) hilft nicht: der Spieler bewegt sich in der Szene frei, der gemerkte Schnitt stimmt dann auch nicht mehr. Gebaut: `re15_bf_schnitt_aus_lage`, Grenzen x 2750, 14000, 20250. Der Riegel prueft jede Lage gegen die RVD-Bytes beider Raeume: kein Wechselstreifen des gewaehlten Schnitts liegt an dieser Lage.

**Dauer.** GESTALTUNG, keine Original-Szene: die Szenenkamera endet mit [T2] (`ph >= 3`), nicht mit Sub 2.

| Lauf | vorher | nachher |
|---|---|---|
| z_ende, Leon (20748,-26416) | Cut 11 F885-F2101, dann Cut 12, Leon ausser Bild bis Laufende | Cut 11 F885-F1245, ab F1246 Cut 14, Zonen an, Leon im Bild |
| z_fern, Leon (8143,-24704) | Sperre bis F1874 | F1019 Cut 12, Zonen an |
| Sperre ab Morph, ohne Treffer | 1126 Bilder, 37,5 s | 271 Bilder, 9 s |

Den Koerper sieht man noch nach dem Hereinziehen ins Bild kommen; das Fenster fuellt er erst ab [T6], und dann folgt die Kamera schon dem Spieler.

### 9.4 G5: Biss, Fressen, Game Over

Die Meldung traf nur das Ende einer Kette. Lauf g_frei2 vorher: F2351 HP 60 -> -1, kein YOU DIED bis F4000. Alle Glieder aus dem Overlay CDEMD0.EMS Typ 0x36 / 0x37 und der RE2-EXE:

| Glied | Original | Port vorher |
|---|---|---|
| Biss | FUN_800401d4(40, 1): a0 @0x80104118, a1 @0x80104170; `bltz` @0x80040264 -> HP = 0 @0x800402b0. Toetet nie | HP < 0 -> -1, toetet |
| Devour | nur bei HP <= 0: `bgtz v0` @0x801008e0 und @0x80103e80; Boss-HP `bltz` @0x80103e90; Griff-Latch 0x800CFDCB @0x80103e50 | Polaritaet verkehrt (`hp >= 0`), frass den gesunden Spieler; Pfad 2 zusaetzlich mit u >= 9001, das gehoert nur zu Pfad 1 @0x801008a0 |
| Sperre +0x229 | -1 je Bild im Kopf @0x80100188-98 | nur bei Blob 0 Phase 0, lief nie ab: nach dem ersten Biss kein zweiter |
| Segment 1 | +500, Klemme bei 3701 @0x801040c0-e0; -15, Boden 2200 @0x801042b8-d4 | `< 3700`, ueberschoss nach dem Abstellen der Sperre bis 4185 |
| Todes-Latch | FUN_800401d4(500, 0) -> `0x800CFB74 |= 0x04000000` @0x800402d8, Flow-Manager @0x800266c8-34, DIE-Overlay @0x8002671c | fehlte; Opfer-Modus 4 haelt is_dead auf 0 |
| Game-Over-Tor | RE1.5 FUN_8001500c: Kommando 3, 6, 7 (`lbu v1,-13736(v1)` @0x80015014, @0x8001501c-30), selbst geprueft | |
| Kauen | Phase 3 @0x80104b0c-6c: Gewicht -= Rate, unter +0x21e zurueck nach Phase 2 mit Rate 2048 + 4r; Rate += +0x21b. Schwelle aus fuenf Wuerfeln @0x801049c0-f8, Ton @0x801049fc-a08, Kau-Blut @0x80104a64 / @0x80104ab8 | nur Ton, Gewicht stand, Ende-Gate @0x80101d14 fiel nie |
| Ende | `sb zero,536(s2)` @0x80101d20: nur der Blob. PL+0x04 wird nicht geschrieben, Sub 4 und Routine 6 bleiben | gab den Spieler frei |
| Tentakel | a1 = 1 @0x80102d7c, @0x80103734, @0x801037ec; a1 = 0 @0x801016d4 mit Gnade -14..-1 @0x80040288 | alle toedlich, HP < 0 -> -1 |

Das Bit 0x1000 der Gnade fuehrt der Port nicht; es faellt nur beim Heilen (@0x80040308), deshalb gilt die Gnade, solange der Spieler vor dem Treffer HP hatte. Ohne den Tentakel-Fix starb Leon im Lauf bei HP 5 am Arm (F3612), die Fress-Kette lief nie.

Nachher, ohne Taste nach dem Tod der Form, Leon und Elza bildgleich: HP 100 -> 60 Biss F1842 -> 45 Arm F2809 -> 5 Biss F2853 -> 0 Arm F3612 -> -1 Devour F3623. finisher.log: FSM-START, BLACKBG, YOUDIED, jeweils `isdead=0`, also traegt der neue Latch. Danach Titel, zweiter Spiel-Boot. Bild: YOU DIED ueber dem laufenden Fressen. `integration_r41_g5_tod` faehrt das ohne Messhaken.

⛔ Ueberholt durch 10.5: Hier blieb der Biss die Rohzahl 40 von RE2 auf 100 Port-HP. Jetzt gilt die Griff-Regel, Biss 20, Arme 7, 7, 5, 7. Die HP-Reihe oben ist die alte.

### 9.5 Startwaffe bis zum G5-Tod

Dauerfeuer, kein Messhaken, Fenster. Leon und Elza bildgleich:

| Mutation | Form | Griff | Rueckstoss | G5 | G5 tot | Spieler |
|---|---|---|---|---|---|---|
| F1125 | F1305 | F1465 HP 70 -> 35 | F1627 | F1784 | F2903 | lebt, HP 35 |

Der Orchestrator-Punkt aus Runde 1 ist damit erfuellt: jede Form faellt in 5090/5091 durch reinen Dauerbeschuss. `integration_r41_birkin_pistole` prueft das jetzt bis zum G5-Tod (Exit F3100).

### 9.6 Riegel, Gegenprobe

Gegenprobe: je Fix heraus (Objektdatei im Scratch, Baum unberuehrt), Test neu gelinkt.

| Fix heraus | Test | FAIL-Zeilen | erste Meldung |
|---|---|---|---|
| Clip-Ende | test_birkin_bahnsteig | 10 | Biss: Knochen 1752 im Ende-Tick |
| Selbstschub | test_birkin_bahnsteig | 4 | Nordspur: 7233 getragen; Diagonale z -26416 |
| Biss toetet | test_birkin_szenen | 8 | HP faellt nicht auf 0 |
| Polaritaet | test_birkin_szenen | 3 | gefressen bei HP 100 statt 0 |
| Sperre +0x229 | test_birkin_szenen | 9 | nur 1 Biss |
| Kauen | test_birkin_szenen | 2 | Gewicht 10688..10688 |
| Latch | test_birkin_szenen | 1 | Game-Over-FSM startet nicht |
| fester Cut 12 | test_birkin_szenen | 1 | Cut 12 statt 14 |
| Freigabe erst mit Sub 2 | test_birkin_szenen | 1 | Freigabe in Sub 1 |
| Grenze 13000 | test_birkin_szenen | 380 | Lage in Wechselstreifen |

Tentakel a1: kein eigener Unit-Riegel, `integration_r41_g5_tod` (Tod bei HP 5 statt 0, siehe 9.4).

**Geaendert, nicht nur ergaenzt:** `unit_r20_birkin_push_pin`. Der Pin war gruen, weil der Boss nach dem ersten Fressen fuer immer in Sub 4 stand: 7876 Bilder, die meisten gegen einen stummen Boss. Jetzt kaempft er alle 8000 Bilder, das zeigte zweierlei. Der Griff der Arme (Routine 5, Zug-Ruck 0x801042c4, Spiess 0x801046ac) faehrt den Spieler selbst, im Endstand bis 1592 je Bild; alle Wege ueber der Fallen-Schwelle 1250 lagen in Griff-Bildern, Lage begehbar. Und die Kind-Kollision der Arme laeuft nach der Spieler-Klemme des Bildes, das Bildende kann in einer Wand liegen (Zwischenstand 11 von 8000, Endstand 0), die Klemme des Folgebilds loest es (⛔ falsch, s. 10.1: im echten Lauf warf genau diese Klemme jede Eingabe auf die Lage in der Wand zurueck, z_fern 388 Bilder). Der Pin misst deshalb an der Station KLEMME und nimmt Griff-Bilder aus dem Wege-Maximum. Liegt ein Startplatz durch den wandernden Boss nicht mehr im Gang, zaehlt der naechste. M9 in test_birkin_szenen prueft die Sperre jetzt bis [T2].

### 9.7 Offen

- **Verworfen und unveraendert:** Leon steht im Vorderkoerper der Form, das ist RE2 (Abschnitt 6).
- **EM30, Clip-Ende:** derselbe Umlauf in re15_birkin_anim (Abnahme: Clip 16 +2001). Nicht angefasst, dort haengen 53 Aufrufer samt Schleifen; gehoert zusammen mit der fehlenden anim_blend_rate (8.2) in eine eigene Runde.
- **EM30, Selbstschub:** die 0x30-Wurzel ruft ihn im Original ebenfalls (`jal 0x8002aec4` @0x80116cb0), der Port nicht. Der Kommentar in re15_enemy_selfpush_from_player verlangt dafuer eine eigene Runde. Hier nicht gemeldet.
- **Wandzaehler des Rennens:** +0x158 zaehlt Bilder an der Wand, ab 91 steuert die Form 4 Bilder von der Box weg (@0x80102dc8-e7c). Nicht gebaut; das Rennen endet vorher am Clip-Ende.
- **Griff-Treffer:** G5-Biss auf den von einem Arm gehaltenen Spieler (@0x8010411c-64) nicht gebaut, Devour-Blut Clip 9 Bild 16-29 (@0x80101bf0-c60) nicht gebaut.
- ⛔ Erledigt in 10.1: **Griff-Zug der Arme.** Hier stand, die Wandlagen seien nicht durchgetestet. Der Griff lief wirklich in die Wand (Abnahme 3 N2), jetzt Nach-Klemme und Fuzz.
- ⛔ Erledigt in 10.5: **G5-Biss-Zahl.**
- **Rueckstoss-Szene:** haelt Cut 12 bzw. 11 fest; Leon weit im Osten sieht sich bis zum Ende des G5-Auftritts nicht. ⛔ Berichtigt: hier stand "rund 80 Bilder", Abnahme 3 misst ueber 400, rund 14 s. Ob die Szene kuerzer wird, ist eine Gestaltungsfrage.
- ⛔ Erledigt in 10.4: **G5 blutet nicht.**

## 10. Nachbesserung nach Abnahme 3

Sechs Punkte aus `R_abnahme_3.md`, bearbeitet nach Schwere. Jeder lief so: Lauf vorher im echten Fenster, Disassembly, Fix, Lauf nachher, Riegel, Gegenprobe. Laeufe wie in Abschnitt 9, Eingaben der Abnahme. Massgeblich ist `state.log`, das das Spiel am Bildende schreibt. Die Abnahme-Sonde jb.log liest mitten im Bild und zeigt Einzelbilder, die nie gezeichnet werden. Bilder: `R_belege/nb3_westen.jpg`, `nb3_suedwand.jpg`, `nb3_griff_wand.jpg`, `nb3_cut13_nclip.jpg`, `nb3_cut13_tod.jpg`.

### 10.1 N1, N2, N6: Spieler-Klemme aus RE2 Retail

Der Vorschlag der Abnahme, nach dem Schub noch einmal zu klemmen, reicht nicht: die Klemme selbst war der Fehler. Der Port klemmt den Spieler mit der RE1.5-Beta FUN_8003b0a4, und die versagt bei grossen Versaetzen dreifach:

| Fehler | Stelle | Beleg |
|---|---|---|
| 32-Bit-Ueberlauf im Typ-6-Handler: ab rund 740 Tiefe kippt das Vorzeichen, der Spieler wird IN die Diagonale geworfen | LAB_8003cb9c, `mult v1,a2` @0x8003cf88 | f006 F1267 (4542,-25795) -> (4287,-26207), Soll (4287,-25153); z_w4 F1354 |
| Naehere Kante der Zelle, auch die hinter der Wand | FUN_8003bca8 | Box 29 -> z -27668, das ist der Softlock aus N1 |
| Code 0 setzt zurueck auf die Vorlage, auch wenn sie selbst in der Wand liegt | `*lx = prevx` | Bildende in Box 29, jede Eingabe verworfen (N2) |

RE2 Retail hat alle drei behoben. FUN_8003567c ruft FUN_8004c1bc mit Bit 0x01000000 (@0x800356bc-c4), der Zweig nimmt die Tabelle @0x800a737c je Zelltyp. Vorlage ist +0x118/+0x11a, die letzte geklemmte Lage (`sh v0,280(s0)` @0x800356f4). Das Rechteck FUN_8004c900 bleibt auf der Herkunftsseite, Innenfall FUN_8004ca68 mit Grenzen 401 und 801, Diagonalen @0x8004d508 und @0x8004d9b4 schreiben nur bis 400 je Achse (`slti v0,v0,401` @0x8004d878), Raute @0x8004de64 ebenso, Kreis @0x8004ec50 radial. Reihenfolge in RE2: Gegner-Schleife @0x80026570-604, Schub `jal 0x800355c4` @0x80026628, Klemme `jal 0x8003567c` @0x80026634. Damit wird der Weg des Originals gebaut, nicht der Vorschlag.

Gebaut: `re15_collision_constrain_re2`, gilt nur solange ein Birkin (0x30 EM30 und Form, 0x36 G5) aktiv ist, sonst bleibt es Byte fuer Byte FUN_8003b0a4. Der Port zieht die Gegner erst nach dem Spieler, deshalb `re15_player_nachklemme` nach dem Gegner-Pass. Der Wurf braucht `re15_r39_wurf_querung` damit nicht mehr, die Klemme traegt ihn. Das Wand-Bit fuer den Griff-Haken (@0x8011b140) kommt aus der Vor-Klemme.

**Drei PORT-WAHLen, im Code so markiert.** RE1.5 hat in ROOM5090 eine viel engere Bahn als RE2 room7040: Gang 2582 statt 6114, #29, #30 und #31 gibt es dort nicht. Die Regeln decken genau diese Luecke, Details an den Fundstellen:

1. **Rueckstellung auf die Vorlage** (`re15_collision_constrain_re2`, RE15_RE2_RUHE 16): endet die Kette nicht frei, gilt die Vorlage. Kreis #31 drueckt zurueck in #23 und #28, in ROOM5080 laesst Raute #12 eine Tasche. Dieselbe Rueckstellung wie @0x8004d8f0, aber auch fuer diese beiden Faelle.
2. **Vor-Klemme** (`birkin_vorklemme`): die eigene Bewegung wird vor dem Koerper-Schub geklemmt, RE2 klemmt beides zusammen. Sonst verwarf die Rueckstellung an #30 auch die Eingabe (z_w4 F1360 bis F2100).
3. **Nach-Schub verfaellt ganz** (`re15_player_nachklemme`): fuehrt der Arm-Schub in die Wand, gilt er gar nicht, nicht nur in der Wandachse. Der Arm besteht aus vier Kreisen, Leon sass in der Kerbe dazwischen und wurde je Schritt zurueckgeworfen (z_wand_a F1404 bis F1476).

Bildenden in fester Zelle, gemessen im `state.log`:

| Lauf | vorher | nachher |
|---|---|---|
| p_falle (Pistole, Leon) | 2626 | 0 |
| p_falle_e (Elza) | | 0 |
| z_aus_a, z_aus_c | 1347 | 0 |
| z_w3 | 1078 | 0 |
| z_w4 | 747 | 0 |
| z_w4c | | 0 |
| z_fern, z_wand_a | 388 | 0 |
| z_wand_b | | 0 |
| freies Spiel f001 bis f016 | 3085 Bilder in 6 Laeufen (f016 2909, f006 150, f009 12, f004 8, f013 4, f007 2) | 0 in 16 von 16 |

p_falle endet nicht mehr im Softlock: der Koerper traegt Leon ostwaerts bis x 13255, die Arme nehmen ihm die HP, der G5 frisst ihn, Game Over (a_p_falle, a_p_falle_e, HP 0 bei F3000). In z_wand_a laeuft Leon aus der Lage bis x 14500 (`nb3_suedwand.jpg`). N6 f006: Elza bleibt im Griff vor der Wand (`nb3_griff_wand.jpg`).

Fuzz `test_r41_birkin_wand`, Startlagen entlang aller Waende, Mass aus dem Auftrag:

| Modus | Raum | Faelle | Griff-Bilder oder Wuerfe | Bildenden fest | eingekeilt | Kennzahl |
|---|---|---|---|---|---|---|
| g5 | 5090 | 424 je 300 Bilder | 3601 Griff-Bilder | 0 | 0 | kleinster Laufweg 658, 5 Lagen warten bis 15 Bilder auf den Koerper |
| g5 | 5091 | 424 | 3557 | 0 | 0 | 561, 2 Lagen bis 22 Bilder |
| griff | 5090, 5091 | 3392 Griffe je Raum | 1696 Wuerfe je Raum | 0 | 0 | groesster Bildweg 1554 |
| griff | 5080, 5081 | 2864 je Raum | 1432 je Raum | 0 | 0 | 1318 |
| griff | 3070, 3071 | 3824 je Raum | keine, STAGE3 hat nur den Wurf-Tod | 0 | 0 | 1790 |

Der Griff der Form laeuft ueber cmd 6 Variante 0 (`bb_griff`), das faehrt der Modus griff als Wurf mit. Der G5-Griff der Arme laeuft im Modus g5 (Zug, Peitsche, Spiess). N2 gesondert: 90 Bilder Arm-Kontakt an der Suedwand, 0 Bildenden in der Wand, 3 von 3 Laufproben kommen frei. Ecke #29/#30 unter dem Koerper bei gehaltenem G5: 30 Bilder Vorwaerts, Weg 2143. `integration_r41_g5_westen`: Leon und Elza im Westen ohne Feuer, Leon fern, mit Spiel-Exe.

### 10.2 N3: Cut 13 im G5

Erst RE2 geprueft: dieselbe Kamera (room7040 Cut 12 @0x1e4 (11250,-1710,-24912) gegen ROOM5090 Cut 13 (11050,-3110,-24812), RID @0x200), derselbe Zonenweg (Streifen 6 -> 12 @0x40c ab x 16400, hier 12 -> 13 @0x4cc x 14000 bis 15400), und die Overlays EM36 und EM37 schreiben den Schnitt 0x800cfbd8 nicht. RE2 hat also keine Kamera-Regel. Es hat etwas anderes: die Modell-Routinen verwerfen Rueckseiten, FUN_80027bec Dreiecke mit MAC0 <= 0 (@0x80027ca8, `bgtz` @0x80027cc8), FUN_80027dbc Vierecke mit MAC0 < 0, Aufrufer FUN_80019cd0. Der NPC-Zweig in `platform/pc/main.c` hatte das nicht. Steht die Kamera im Koerper, zeigte der Port dessen Innenseite, das waren die 57 Prozent der Abnahme.

Gebaut: `re15_nclip_r41_birkin` (0x30, 0x36, 0x37) und `re15_nclip_r41_weg` im Dreieck- und Viereck-Zweig. Andere Gegner bleiben unberuehrt, s. 10.7.

**Verworfen:** Eine Port-Kamera-Regel, die Cut 13 im G5 sperrt und auf Cut 12 oder Cut 14 legt, war gebaut. Im Fenster stand Leon in Cut 14 nicht im Bild, Cut 14 zeigt erst ab x rund 17000. Komplett zurueckgenommen, keine Kamera-Regel im Stand.

Bildmass Cut 13 im G5 (Pixel mit Abweichung ueber 30 gegen den leeren Hintergrund, jedes 16. Bild):

| Lauf | Median | Bilder ueber 40 Prozent |
|---|---|---|
| vorher g_leon | 43,3 Prozent | 74 Prozent |
| vorher p_leon_pistole | 41,2 Prozent | 87 Prozent |
| nachher n4_leon, Spieler lebt | 13,1 Prozent | 1 von 56 |
| nachher n4_elza, Spieler lebt | 11,9 Prozent | 1 von 56 |
| nachher n4_pist, Pistole bis G5-Tod | 16,8 Prozent | 1 von 82 |

Die uebrigen Bilder ueber 40 Prozent in n4_leon und n4_elza liegen nach dem Tod (HP -1, Weissblende, YOU DIED), das sind 13 von 73. Sichtpruefung: Leon steht ganz im Bild, Arme liegen davor (`nb3_cut13_nclip.jpg`), Bisse, Griff und Weissblende sind zu sehen (`nb3_cut13_tod.jpg`). Riegel: `test_nclip` mit RID-Pin Cut 13 und Vorzeichen, `integration_r41_birkin_pistole` prueft, dass der Zeichner den Verwurf ruft.

### 10.3 N4: Austritt aus dem Weichen

Beide Bit-Loeschungen in `bb_weichen` sind entfernt, wie vorgeschlagen; bb_anim des naechsten Zustands loescht das Bit im selben Zug wie die neue Pose. Original: A[8] setzt 0x201 @0x80104158, die Richtung ist Argument je Aufruf (@0x8010427c-84). Knochen-Sprung im Austritts-Tick: Hieb 1259 -> 65, Gehen 548 -> 55. Riegel `test_n4_weichen_austritt`, Schwelle 300, misst nur den Austritts-Tick; der Folge-Tick ist die regulaere Ueberblendung.

### 10.4 N5: G5 blutet

Die Wache las die KI-Phase. Im Original ist es +6 des Trefferzustands (`lbu v0,6(s3)` / `bnez` @0x801025e8-f0), der Applier nullt es (@0x80047288), also im ersten Treffer-Tick immer 0. Stehen bleibt `g->sub != 0xF`. Dazu Zeilen-Wache 16 und 9 bis 12 (@0x80102658-64) und der fehlende Ton SE 8 (@0x8010293c-40). Biss-Serie @0x801042d8-420 gebaut: `g5_biss_blut` im Schwanz jeder Biss-Phase, 3 Spawns je Bild, 10 Bilder (@0x801041a8), `g->blut_serie = 0` @0x80103f54.

Lauf Pistole nachher: `blut=39/37/37/118`, Abnahme 27/0/0/0. Blut faellt weiter nur, wenn der Spieler nach Westen blickt (`slti v0,a1,-64` @0x80102640), das ist Original. Riegel `test_g5_blut`: Spawns in KI-Phase 1, Zeile 9 ohne Blut, Biss-Serie 30 Spawns.

### 10.5 Aus "Gering", im selben Codeweg

| Punkt | Gebaut | Beleg |
|---|---|---|
| Rennen toetet | Rennen a1 = 1, klemmt auf HP 0 | @0x80102f10 |
| Doppelschlag | zweiter Hieb trifft, Sperre faellt am Clip-Wechsel, Gate HP >= 0 | @0x80101c8c, @0x80101c94-a8 |
| Hieb stumm | Eintritt nullt +0x232 | `sb zero,562(s2)` @0x80101b00 |
| G5-Schaden | halb wie beim Griff: Biss 20, Arme 7, 7, 5, 7 (`RE15_G5_HP`, `RE15_ARM_HP`) | 200 HP RE2 auf 100 Port-HP, Regel @0x801052a8-b4 |
| Mutanten | Tentakel-a1-Riegel, Rate Hieb und Rennen | a1 = 0 @0x801016d4, a3 @0x80101b34, @0x80102d08 |
| Arme versetzen seitlich | geprueft, unveraendert im freien Gang, an der Wand verfaellt der Schub (10.1) | FUN_80034D0C |
| Dossier Rueckstoss | 9.7 berichtigt | Abnahme 3 |

Folge der Halbierung und des Doppelschlags: f001 und f002 sterben jetzt in der Form (F1612, F1703), vorher ueberlebt. Der Doppelschlag trifft wie im Original zweimal. Das ist eine spuerbare Aenderung der Schwierigkeit, gemessen in zwei von 16 freien Laeufen.

### 10.6 Riegel, Gegenprobe

Je Fix heraus, Objektdatei im Scratch, Baum unberuehrt:

| Fix heraus | Test | FAIL-Zeilen | erste Meldung |
|---|---|---|---|
| RE2-Klemme an allen drei Stellen | wand g5 | 8 | N2: 186 Bildenden in der Wand |
| dieselbe | wand griff | 9 | 5090: 4036 Bildenden in fester Zelle |
| Nach-Klemme | wand g5 | 7 | N2: 186 Bildenden in der Wand |
| Nach-Schub bleibt | wand g5 | 2 | N2: Eingabe wirkt nicht (1/7) |
| Vor-Klemme | wand g5 | 2 | N1: Eingabe wirkt in der Ecke nicht (Weg 0) |
| Wand-Bit | wand griff | 6 | nur 78 von 3392 Griffen an der Wand |
| Ruhe-Pruefung | wand griff | 2 | 5080: 3 Faelle eingekeilt |
| beide Proben der Diagonale | wand rein | 1 | Grenze 400: (2812,-25738) statt der Vorlage |
| Herkunftsseite Rechteck | wand rein | 1 | Schub durch Box 29 in Box 22: z -30201 |
| Weichen-Bit, Hieb | bahnsteig | 2 | Weichen -> Hieb springt 1212 |
| Weichen-Bit, Gehen | bahnsteig | 2 | Weichen -> Gehen springt 548 |
| Rennen a1 | bahnsteig | 4 | HP -1 statt 0 |
| Doppelschlag | bahnsteig | 2 | Sperre bleibt, zweiter Hieb ins Leere |
| Hieb +0x232 | bahnsteig | 2 | Sperre steht auf 89 |
| Blut-Wache KI-Phase | szenen | 1 | KI-Phase 1: 0 Spawns statt mindestens 2 |
| Blut-Zeile | szenen | 1 | Zeile 9: 2 Spawns statt 0 |
| Biss-Serie | szenen | 1 | 0 Spawns statt 30 |
| G5-HP roh | szenen | 1 | nur 1 Biss vor dem Fressen |
| NCLIP-Vorzeichen | wand rein | 1 | MAC0 = 0 Dreieck weg (`bgtz` @0x80027cc8) |
| NPC-Verwurf in main.c | birkin_pistole | rot fuer Leon und Elza | Zeichner ruft den Verwurf nicht |

Ueberlebt: die Grenze 401 allein. Die Probe @0x8004d8dc greift frueher (341 gegen 465 Tiefe), mit beiden Proben heraus ist der Riegel rot (Zeile "beide Proben").

**Geaendert, nicht nur ergaenzt:** `probe_g5_tentakel.c` Schaden 7 statt 15 (Halbierung), `probe_r20_birkin_push.c` nur Kommentar (die Klemme des Folgebilds loest nichts, s. Berichtigung in 9.6).

Stand: Zielauswahl 75 von 75 gruen (birkin, g5, tentakel, kollision, collision, nclip, 5090, r20, r21, knock, griff, wand, push, schub), darin `integration_r41_birkin_pistole`, `integration_r41_g5_tod`, `integration_r41_g5_westen`. Keine volle Suite, Auftrag. Die Unit-Tests lagen auf dem Abschlussstand, die Gegenprobe ebenfalls.

### 10.7 Offen

- **Leon unter dem G5 im Westen:** In der Tasche zwischen #29 und #30 kann der Schub ihn nicht mehr ostwaerts tragen, sein Vektor zeigt in die Diagonale, also rollt der G5 ueber ihn. Er ist dann in Cut 12 verdeckt: z_w4 203 Bilder am Stueck, z_aus_a 334 (Abstand zur Koerpermitte unter 5000). Er bleibt frei und kann laufen. Das tritt nur auf, wenn er dort stehen bleibt und nicht schiesst: im freien Spiel 0 Bilder in 16 Laeufen, mit Pistole ebenfalls 0. Bild `nb3_westen.jpg`. Naeher an ihm laege Cut 11 (7626,-3256,-25360, Blick West). Nicht gebaut: keine Kamera-Regel ohne Fall aus dem freien Spiel, Gestaltungsfrage.
- **Eingabe in der Ecke unter dem Koerper:** Lage (4849,-24749), bis 118 Bilder ohne Wirkung (a_z_aus_c), RE2-Code 3 traegt ihn nicht aus der Ecke. Nach Eckdruck springt der Koerper-Schub weit (a_z_aus_a F2231 4235, a_z_aus_c F2188 3385), ohne Tunnel, Ende frei.
- **NPC-Zweig ohne NCLIP** ausser Birkin. Ein globaler Fix gehoert in eine eigene Runde, er aendert alle Gegner.
- **Griff laeuft nach dem Wurf 92 Bilder leer:** Gestaltungsfrage aus der Abnahme, nicht entschieden, nicht angefasst.
- **Treffer-Handler G5:** Zeilen 9 und 17, 10, 11, 14 (0x8010221c, 0x80101d9c, 0x80101ff0, 0x801022d0) nicht gebaut.
- **PORT-WAHL im Blut:** Zeile 1 z und der Biss-Blut-Anker am Spieler-Ursprung, Leons Teile fuehrt der Port nicht engine-seitig.
- **Mutanten:** Tempo-Riegel fuers Rennen fehlt, die uebrigen Punkte der Liste aus der Abnahme sind nicht vollstaendig abgearbeitet (10.5).
- **SDL-Assertion beim Anzeigewechsel:** ohne Bezug, nicht angefasst.
- **Rueckstoss-Szene:** s. 9.7, Gestaltungsfrage.
- Aus 9.7 weiter offen: EM30 Clip-Ende und Selbstschub, Wandzaehler des Rennens, Griff-Treffer, Devour-Blut.
