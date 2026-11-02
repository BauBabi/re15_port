# Stufe 3 Bahnsteig-Form: Bewegung, Angriffe, Einbau (laufendes Dossier)

## Nachbesserung 1 - N0: Ursache "Texturen vollkommen kaputt" (Nutzerbefund zur K3-Vorschau, 2026-11-02 abends)

Befund: im echten Fenster (stufe3/nachbesserung1/loecher_echt_vorher_nachher.png, rechte Spalte) und offline (stufe3/formpruefung/
rundum_hell.png) grau-weiss karierte, fleckige Bloecke an Hals, Schulter, Ellbogen, Knie, Haenden und am Kopf; bei J nicht.

Ursache, gemessen (stufe3/gen/s3_n1_textur.py pruefen, Bytes des Assets):
1. **Gelenkkoerper (Hauptursache).** Die 31 Gelenkkoerper von K3 (varianten/K_alligator/generator/k_naht.py `kugel`, Zeilen 147-152)
   uebernehmen fuer JEDE ihrer 40 Flaechen die UV-Ecken, CLUT und Seite der jeweils naechsten Flaeche des Elternmeshes, unveraendert
   und in beliebiger Drehung. Aus Hautstuecken mit weissem Fleck, Kante oder Naht werden so je Koerper 14-64 verschiedene, gedrehte und
   gestreckte Texturstuecke nebeneinander: ein Schachbrett. Gemessen am K3-Asset: verschiedene UV-Stuecke je Koerper-Mesh 14-64,
   Helligkeitsspanne (p95-p5) je Koerper bis 186. Die Koerper sitzen genau dort, wo der Befund die Bloecke sieht (Hals, Schulter,
   Ellbogen, Knie, Hand, Kopf-Hals-Muffe und Mundwinkel am Kopf).
2. **Texel ausserhalb des TIM (Nebenursache, Kopf).** 35 Flaechen von K3 (Kopf 22: 27, Kiefer 23: 8) reichen auf Seite 3 bis u = 128,
   also Texel-x = 128 + 3*128 = 512 bei einem 512 breiten TIM: eine Spalte hinter dem rechten Rand.
3. **Kein Seiten-/CLUT-Fehler im Port.** Der Port rechnet Texel-x = u + (page & 0xF) * 128 (platform/pc/main.c `page_off`) und waehlt
   die CLUT-Zeile ueber das CLUT-Wort (render_pc.c re15_render_textured_tri: v_offset = CLUT-Index * TIM-Hoehe, Textur 512 x 1024 mit
   4 CLUT-Zeilen, re15_render_pc_upload_tim_slot); die Offline-Buehne (form_buehne) nutzt dieselbe Regel. Jede Flaeche liest Seite und
   CLUT-Zeile ihrer Quellflaeche (Tabelle Seite/Zeile/Texelband je Flaeche: kein Band ausserhalb der Seite ausser Punkt 2). Darum zeigt
   die Offline-Vorschau dieselben Flecken wie das Spiel; sie taeuscht hier nicht.

Behebung (Asset-Spur, stufe3/gen/s3_n1_textur.py, Basis stufe3/basis_n1): jeder Gelenkkoerper bekommt ein zusammenhaengendes,
fleckfreies Hautstueck seiner Umgebung (Breiten-/Laengengrad -> Rechteck 12-32 Texel, gleiche Seite und CLUT-Zeile wie die naechsten
Originalflaechen, Nachbarflaechen teilen ihre UV-Kanten); nur Ellbogen r setzt die Haut darunter fort (er liegt in Bild 1 auf dem weissen
Dreieck Z5). Kopf/Kiefer-UV auf die Seite geklemmt. Gemessen: Texel ausserhalb TIM/ohne CLUT 35 -> 0, Koerperecken mit UV-Sprung > 2 Texel
89 % -> 19 %, Koernung Koerper/Umgebung 1,10 (Mosaik) -> 0,83, Formpruefung unveraendert (Bild 1 0,954 31/31, Bild 2 0,948 7/7).
Verworfene Fassungen (2x2 einfarbig = Plastikbeulen, Projektion ueberall = wieder fleckig) und alle Zahlen: stufe3/nachbesserung1_asset.md
Abschnitt 1a. Beleg im echten Fenster (5080 Schnitt 4 Gier 0/1024/2048/3072, 5090 Schnitt 11 Pose Bild 2 und Gier 802/1826, links K3 vorher,
rechts nachher): stufe3/nachbesserung1/textur_vorher_nachher.png.

## Nachbesserung 1 - Stand (2026-11-02 abends)

Auftrag: K3 ins Spiel (E20/E21), Loecher (M2), Maengel M1-M8 der Abnahme 1 (STUFE3_abnahme_1.md), Kopf-Folgen fuer den Tick. Zwei Spuren:
Asset (Generator, Offline, eigener Bericht stufe3/nachbesserung1_asset.md) und C-Seite (Tick, Szene, Kamera, Ton, Koerper; dieser Abschnitt).
Alle neuen Zahlen sind PORT-WAHL fuer das neue Modell; was aus dem G4-Overlay (CDEMD0.EMS rec 1 @0x80100000) oder RE1.5 kommt, traegt
seine Adresse im Code.

| Punkt | Behebung | vorher -> nachher (gemessen) |
|---|---|---|
| A K3 im Asset | Generator auf K3_fit/basis_n1; Ruhe = J-Ruhe mit Kiefer zu; Kiefer zu in Stehen/Gehen/Platzhaltern, uebrige Maeuler x 0,65 (max. ~31 Grad); Pins Clip 10 Bild 45-52 / Clip 12 Bild 0 = K3-Pose Bild 2, Clip 0 Bild 11 = K3-Pose Bild 1; form_j_posen.h neu | Formpruefung Bild 1 0,948 -> 0,954 (31/31), Bild 2 0,935 -> 0,948 (7/7) = K3 offline |
| B Loecher | Gelenkkoerper K3, Hiebe ohne Huft-Gier (Spalt Bein R - Huefte in den Hieben) | Achsel+Huefte (13 Tick-Clips x 8 Bilder x 10 Ansichten) 4465 -> 319 px (-93 %); alle Naehte ohne Mundwinkel 16534 -> 3183 px |
| M1 Sturz unsichtbar | Sturz (Clip 12) beginnt im Wagen bei x <= -2500 statt -6283; Clip-Spur traegt die Form durch die Tuer (-5683); Taumeln ab x <= 3000 in Cut 11 | Abnahme: Clip 12 nie im Bild (x -6313 -> -14583); jetzt Cut 11 ab Taumeln x 3000, Sturz x -2551 -> Tuer bei Bild ~25 |
| M2 Loecher | s. B | s. B; Fenster-Stichprobe s. N2 |
| M3 Gebruell stumm | EDD-Ids 9-15 (Bank 24 FFFFFFFF) -> Raum-Ton 10 (STAGE5 @0x8011af58-5c) | 5090: 8 SILENT-Rufe (ton5090_re2se.log) -> 0; Raum-Ton 10 4x im Auftritt + 1x Sturz (n1 l5090_feuer debug.log) |
| M4 Kamera-Nahsicht | (a) Auftritt in Cut 11 (Kamera von Bild 2) statt Cut 15; (b) Nahsicht-Wache Cut 12/13 | (a) Cut 15: 89 % Bildflaeche Textur (Buehne x 1200) -> Cut 11 mit ganzer Form; (b) s. N4 |
| M5 Rueckstoss gleitet | neuer Clip 13 Taumeln (40 Bilder, Spur -80/Bild, Fuesse verriegelt) | 147 je Bild auf stehendem Clip 8 -> 80 je Bild aus der Clip-Spur, Gleiten der Fuesse 3,4 |
| M6 Wand / Drehung | Wandklemme erkannt (Wurzel < halber Spur-Schub, 6 Bilder) -> Weichen bricht ab, 90 Bilder Sperre; Flucht: zurueckweichen bis 6000, dann Drehen im Gang (Rate 32), dann Clip 11 | Test: Weichen an der Wand 120 -> <= 30 Bilder; Flucht dreht max. 32/Bild (vorher 128 auf stehenden Fuessen) |
| M7 Spieler in der Form | Koerper-Segmente Huefte/Brust am Gerippe (r 1100/1000, G4-Felder), Wandklemme r 1500, Kasten 1700/2600/1600, Schatten x -1300..4000 +-1500 | Spieler >= 1450 von der Rumpfachse (Test), Schwanz in der Flucht >= 2721 vom Spieler (Test) |
| M8a Blut | Anker Kopf (Teil 22) statt Brust | Blut > 1500 hoch, > 1800 vor der Wurzel (Test) |
| M8b Todeszweig | geprueft, nichts zu schreiben (N7) | - |
| M8c BAHNSTEIG_G4 | tools/birkin_bahnsteig.py schreibt nach build/extracted/bahnsteig_g4, shared_assets abgelehnt | - |
| M8e Spruenge | Asset-Spur | Rennen 106 -> 46 Grad, Drohen 54 -> 43, Krallen 51 -> 38 |
| D Biss am Kiefer | Probe Teil 23 + (911,-75,0), Radius 1500 (@0x80104f88-a4); Entscheid < 0xce4 oder 0xfa0..5000 | Test: alle Treffer mit Kiefer < 1500 vom Spieler; Gegenprobe Abstand 2600-3000: alter Brustpunkt 54 entfernt, kein Treffer |

### N1. Ton (M3)

ENEMSE-Bank 24 selbst gelesen (ENEMSE.VBS, TOC @0x800a7b1c Zeile 24: EDT @0x1d8000, 3752 Byte, SE-Karte 32 Worte; Skript
scratchpad n1/bank24.py): hoerbar sind Id 0-4, 6, 8 (VAG 2-5, 18-20, 0,21-0,72 s); Id 5 und 7 zeigen auf VAG 1 mit 48 Byte (Attrappe); Id
9-15 sind FFFFFFFF. Die G4-Paarzeile @0x800a7400 Zeile 24 = {0x26,0x00}: der G4 ist erster kind, also ohne +0x10 (FUN_8005bd6c
`if (*param_2 & 0x2000) param_1 += 0x10`), die zweite Kartenhaelfte (Id 16-30) erreicht er nie. Damit sind die G4-EDD-Gebruelle (Clip 10
Bild 30/51/81/113, Clip 27 Bild 36/60) auch in RE2 stumm. PORT-WAHL: bb_bild_ton spielt fuer EDD-Ids >= 9 den Birkin-Schrei des Raums,
Raum-Ton 10 (STAGE5 @0x8011af58 `jal 0x800453d0` / @0x8011af5c `ori a0,zero,0xa`, Todesphase des 0x30 bei Bild 0x3c, 0x95 == 0x3c
@0x8011af48-50). Die EDD-Daten der Clips bleiben G4-gleich (Test gegen CDEMD0.EMS unveraendert). Der Ton am Wagenende ohne Riesenform
bleibt re15_audio_room_se(10). Gemessen im echten Fenster 5090 (n1 l5090_feuer, debug.log `[se] Stimme: se=10 ... vag=11`): Auftritt F300,
F330, F360, F390 (= Clip 10 Bild 30/51/81/113) und Sturz; re2se.log ohne SILENT. Vorher (Abnahme ton5090_re2se.log): se 9 x2, 10 x1,
11 x4, 12 x1 SILENT. Testhaken re15_bb_ton_zaehler, Test test_n1_gebruell (4 Gebruelle im Auftritt).

### N2. Rueckstoss und Sturz (M1, M5)

- Phase 1 (birkin_formen.c bf_rs_riesenform_phase1): Clip 13 Taumeln im Kreis, Vortrieb aus der Clip-Spur (bb_spur/bb_schieben), Ruecken zum
  Ausgang (Gier lenkt mit 64 je Bild auf den Spiegelpunkt des Ausgangs), Cut 12 bis x 3000, ab x <= 3000 Cut 11 (Form >= 4600 vor dem
  Auge 7626, von vorn). Bei x <= -2500 und >= 90 Ticks Beschuss Sturz (Phase 3); vorher am Platz zucken (Clip 8 ab Bild 5).
- Gemessen n1 l5090_feuer (vor der Cut-11-Aenderung): HP 0 F708, Taumeln F725-F832 von x 5929 nach -2551 (80 je Bild, 3,6 s), Sturz Clip 12
  ab F832 in Cut 11, Tuer bei Bild ~25, G5 F933. Bildleiste: stufe3/nachbesserung1/m1_m5_rueckstoss_sturz.png.
- Vorher (Abnahme): Phase 1 147 je Bild auf Clip 8, Phase 3 erst bei x -6283, Clip 12 ausserhalb des Bildes.
- Elza 5091 (n1 e5091_feuer, mit Cut 11 ab x 3000): HP 0 F696, Taumeln F718, Cut 11 ab F752 (x 2954), Sturz F822 (x -2566), G5 F923.

### N3. Biss am Kiefer und Kopf-Folgen (D)

- Entscheid Orchestrator (Weg a): Trefferprobe am Kiefer, Teil 23 + (911,-75,0) (Mitte der vorderen Zahnreihe, 228 hinter der Spitze
  (1139,-75,0) von Mesh 23), Radius 1500 von G4 (@0x80104f88-a4); der alte Brustpunkt Teil 10 + (1400,0,0) entfaellt fuer den Biss.
- Entscheid Biss 0xce4 (@0x80102434-54) + 1700 = 5000 (PORT-WAHL). Umgesetzt als Band: Biss bei dist < 0xce4 wie G4 ODER 0xfa0 <= dist <
  5000. Grund: in der Nabe ueberschreibt der Biss den vorher gewaehlten Hieb (k824 umfasst k512); mit "dist < 5000" verdraengte er jeden Hieb
  (Hieb dist < 0xfa0) und damit Doppelschlag/Nachhieb. Gemessen im Fenster (n1 l5090_angriffe, Leon ohne Feuer): Biss aus 4300-5000, Griff,
  Weichen, Hieb links (Clip 4) + Nachhieb (Clip 9), Leon stirbt F1134. Zur Bestaetigung beim Orchestrator: das Band ist meine Auslegung.
- Test test_n1_biss_kiefer (5090/5091): Abstand 2600-6400 in 200er-Schritten; jeder Treffer mit Kieferprobe < 1500 vom Spieler, Treffer
  von 3200 bis 6000; Gegenprobe Abstand 2600-3000: der alte Brustpunkt laege 54-60 vom Spieler (haette getroffen), die gezeichnete Schnauze
  geht ueber ihn hinweg (Kieferprobe 1372-1378), kein Treffer.
- Hiebe: Krallenform unveraendert (Variante K: nur Ton), Einstiegsbilder des Doppelschlags treffen nicht (s3_reichweite, Asset-Spur).
- Kasten (Schuss-/Koerper-Kasten des Aktors): hit_offset_x 800 / r 1000 (0x30 @0x8011ee64) -> Mitte Huefte..Schnauze 1700, laengs 2600,
  quer 1600 (Haende +-1590). Schatten 2300 x 1300 um die Wurzel (G4 @0x801002cc/@0x80100300) -> Rechteck x -1300..4000, quer +-1500
  (re15_bb_schatten, platform/pc/main.c).

### N4. Kamera (M4)

- (a) Kampfstart: die Riesenform tritt in Cut 11 auf (Kamera von Bild 2) statt in Cut 15. Gemessen offline (form_buehne, Clip 10 Bild 30):
  Cut 15 bei x 1200 89 % des Bildes, x 3000 45 % und abgeschnitten; die Blickachse von Cut 15 verlaesst den Gang bei x 3220, eine Lage
  weiter weg gibt es nicht. Halt bis Clip 10 Bild 60 (Pose Bild 2 in Bild 45-52 liegt darin), dann Schnitt der Spielerlage und Zonen frei.
  Fenster: n1 l5090_angriffe Bild 248-266 (Cut 11, ganze Form vor dem Wagenende).
- (b) Nahsicht-Wache Cut 12/13: Form < 4500 vor dem Auge, Spieler dahinter -> Gegenkamera, wenn der Spieler dort >= 1500 vor dem Auge steht.
  Fenster n1 l5090_angriffe: nach dem Griff liegt Leon bei x 14932-16722 (Cut 13), die Form kommt von x 6400; bei F846 schaltet die Wache auf
  Cut 12 (Leon von hinten, die Form vor ihm, Bild 446-512) statt 8 s Form zwischen Kamera und Leon (Abnahme Bild 284-338).
  Rest: als Leon nach dem Hieb bei x 17218 steht (1300 vor dem Auge von Cut 12), greift die Wache nicht; Cut 13 zeigt die Form 5800 vor
  dem Auge gross (Bild 516-560), Leon klein dahinter sichtbar (nicht verdeckt).

### N5. Wand und Flucht (M6)

- Klemme: s_bb.klemm zaehlt Bilder, in denen die Wurzel weniger als die Haelfte des Spur-Schubs kam (gemessen am Tickanfang gegen die Lage
  vor dem Schub; Wandklemme und Koerper-Schub wirken danach). Weichen bricht nach 6 Bildern ab (Sperre 90 Bilder ohne Weichen), Gehen lenkt
  ab 10 Bildern zusaetzlich mit 32 (@0x801018ec-908) zum Spieler. Test test_n1_weichen_klemme: <= 30 Bilder Weichen an einer Wand in 120.
- Flucht (bb_flucht, drei Phasen): zurueckweichen bis 6000 (Schwanz 3520 + Spieler 450 + 2000), Klemme oder 60 Bilder; Drehen im Gang
  (Clip 5, Lenken 32) bis Ziel im 256er-Kegel oder 96 Bilder; dann Clip 11 mit Lenken 32. Test test_flucht (5080/5081): Drehen 56 Bilder
  max. 32 je Bild, Clip 11 max. 32 je Bild (vorher 128 auf stehenden Fuessen), Schwanz (Glieder 4-6) mindestens 2721 vom Spieler, Flag
  (5,0x1c) am Ende, Treffer halten die Flucht nicht auf.
- Fenster n1 l5080_leon: Flucht ab t=510 (Weichen), Drehen t=570-600, Clip 11 ab t=615, Raumtext "The power already has been supplied".

### N6. Koerper, Breite (M7)

- Spieler-Schub und Form-Schub (beide Richtungen wie G4 FUN_80034d0c) ueber zwei Segmente am Gerippe der gezeichneten Pose: Huefte Teil 0
  r 1100 (+0x9a = 0x44c @0x8010076c), Brust Teil 10 r 1000 (+0xba = 0x3e8 @0x80100784), Halbhoehe 2000 (+0x9e/+0xbe = 0x7d0
  @0x80100770/@0x801007a4); Feldfolge wie G5 @0x80100530-88. G4 setzt sie fest an die Wurzel (x 400 / 0); die K3-Form ist laenger, deshalb
  am Gerippe (PORT-WAHL). Ein Kopfsegment (probiert) hielt den Spieler vorn >= 4150 fern: kein Hieb, kein Griff mehr - verworfen. Griff-Paar
  ausgenommen. Test test_n1_koerper: Spieler auf Brust/Huefte gesetzt -> >= 1400 von Teil 0/10.
- Wandklemme der Wurzel r 1500 statt 1000 (Haende +-1590; Gaenge 3200/3500).
- Schwanz in der Flucht: s. N5.

### N7. Kleinkram (M8)

- a Blut: Anker Teil 22 (Kopf) statt Teil 10 (@0x80107390-b8 nimmt Teil 10); die Versaetze bleiben die des Overlays. Test: Blut > 1500
  hoch und > 1800 vor der Wurzel.
- b Todeszweig STAGE5 @0x8011aeb0-d0 selbst disassembliert: `lw v1,-14460(v1)` (g_entity(cur) 0x800ac784) / `ori v0,zero,0x12c` /
  `sh v0,156(v1)` und `ori v0,zero,0x3` / `sb v0,7(v1)` im Delay-Slot von `jal 0x8004ef90` @0x8011aecc. Das sind Felder des sterbenden
  Aktors selbst: +0x9c = ai_timer 300, +0x7 = sub_state_3 = Phase 3 (Port enemy_ai_common.c Todesmaschine Phase 2, `e->ai_timer = 300`,
  Phase 3 wartet auf grid & 0xf == 2). ROOM5080 liest sie nicht (SCD: kein Member_cmp; sub01 @0x0078A fragt nur Flag (5,0x1c)). Die Flucht
  setzt das Flag und nimmt den Aktor aus (active = 0): kein Leser mehr, nichts zu schreiben.
- c tools/birkin_bahnsteig.py schreibt nach build/extracted/bahnsteig_g4 und lehnt shared_assets ab (Probe: Abbruch mit Meldung).
- d Bodenausnahmen: Asset-Spur (Drohen 242 -> 67, Rennen Bild 11 = Pose Bild 1 334 unveraendert).
- e Spruenge: Asset-Spur.

### N8. Loecher im echten Fenster (M2, Stichprobe der Abnahme)

5080 Schnitt 4, RE15_BB_POSE -23500,-18200 (Asset-Spur, stufe3/nachbesserung1/loecher_echt_vorher_nachher.png): Clip 5 Bild 28 Gier 2048
1059 -> 414 px, Gier 0 1074 -> 764 px, Clip 10 Bild 100 Gier 2048 1649 -> 1213 px geschlossene Hintergrundinseln. Die verbliebenen Inseln
grenzen laut Kennungsbild der Buehne an BEIDE Vorderbeine (Arm L 11-13 gegen Oberarm R 16) bzw. BEIDE Hinterbeine (Oberschenkel 8 gegen 9,
Unterschenkel 26): der Blick unter dem Rumpf zwischen den Beinen, keine Naht an Achsel oder Huefte (vorher grenzte die Achsel-Insel an
Oberarm/Unterarm und Hals derselben Seite). Offline (13 Tick-Clips, 10 Ansichten) Achsel+Huefte 4465 -> 319 px; Rest 25 Ansichten zu
3-37 px (Rennen Bild 9-12 Bein L, Hieb links Bild 7, Nachhieb Bild 9, Taumeln Bild 0, Sturz, Drohen von unten). Nicht belegt: ob der Nutzer
mit "Loecher zwischen Bein und Oberkoerper" auch die Luecke zwischen den Beinen meint; schliessen liesse sie sich nur mit einem "Rock"
unter dem Rumpf, der in Bild 1/2 sichtbar waere.

### N9. Tests

- Gezielt (re15_port/build, Stand fbef4bcc7): unit_birkin_ai, _bahnsteig, _formen, _opfer, _szenen, unit_r41_form_emd,
  unit_r41_birkin_wand_rein/_g5/_griff, unit_g5_boss/_morph/_tentakel, probe_p2_birkin_g5, probe_p3_birkin_rest, unit_r17/18/20/30-Birkin-Pins,
  unit_r30_n_room5080_birkin_wand: 19/19 gruen; integration_r41_birkin_pistole gruen (N12; vorher rot: erwartete Clip 8 in der Rueckstoss-Szene,
  jetzt Clip 13 oder 8).
- Neu/angepasst in test_birkin_bahnsteig.c: Rueckstoss (Clip 13, Schritt <= 100 je Bild, Sturz im Wagen x -2500..-3500), Flucht (Phasen,
  Gier <= 40 je Bild, Schwanz > 900 vom Spieler, gemessen 2721), test_n1_biss_kiefer, test_n1_gebruell, test_n1_koerper,
  test_n1_weichen_klemme, test_n1_blut_kopf; Asset-Spur: test_asset/test_masse (Clip 13, Sturz-Spur, Drohen-Bodenausnahme), raum_laden.
- Android-Asset-Gate (release/apk_asset_gate.py --quellbaum): OK, 4282 Dateien, RE2 363. Windows-Paket-Gate (release/make_package.sh
  check_tree): prueft BAHNSTEIG_FORM.EMD/.TIM auf vorhanden, nicht leer, bytegleich - erfuellt (Dateien liegen im Quellbaum, Paket kopiert RE2
  ganz); make_package selbst nicht gelaufen (baut kein Paket ohne Release-Bau).

### N10. Sichtbelege (echtes Fenster, eigene exe-Kopie re15_pc_n1b, gdigrab hwnd=, Renderer des Nutzers; Laeufe re15_port/build/n1_spiel/)

Werkzeug stufe3/gen/s3_n1_spiel.sh (gleiche Eingaben wie die Abnahme), Leisten stufe3/gen/s3_n1_leiste.py. Laeufe: l5090_feuer, l5090_feuer2
(mit Cut 11 ab x 3000), l5090_angriffe, e5091_feuer (Elza), l5080_leon, e5081 (Elza), k3_bild1/k3_bild2 (Posen-Messhaken).

| Blatt (stufe3/nachbesserung1/) | Inhalt |
|---|---|
| m1_m5_rueckstoss_sturz.png | vorher Gleiten + leere Tuer (Abnahme), nachher Taumeln, Cut 11 von vorn, Sturz durch die Tuer |
| m4a_kampfstart.png | vorher Cut 15 nur Textur, nachher Auftritt in Cut 11 |
| m4b_nahsicht.png | vorher Form fuellt Cut 13 8 s, nachher Wache Cut 12 (Leon vorn, Form dahinter), Rest Bild 524-548 |
| m6_m7_5081.png | vorher Elza unter dem Bauch, nachher Elza neben dem Kopf, Blut am Kopf, Flucht mit Drehen |
| k3_spiel_neben_bild1_bild2.png | K3 im Spiel (Endasset) neben Screenshot 2 (Cut 11, Pose Bild 2) und Screenshot 1 (5080 Cut 1, Pose Bild 1) |
| textur_vorher_nachher.png, loecher_echt_vorher_nachher.png, kopf_frontal_spiel.png | Asset-Spur (N0, N8, Klaerung Kopfform) |

Ablauf gemessen (state.log/birkin_dbg.log): 5090 Leon mit Feuer: Auftritt Cut 11 F282-F341, Biss aus 4400-5000 (bis HP 0 bei F708), Taumeln
F725-F832, Sturz F832, G5 F933; Elza 5091 gleich (HP 0 F696, Sturz F822, G5 F923). 5090 Leon ohne Feuer: Biss, Griff (Leon fliegt nach x
14932), Wache Cut 12 F846, Weichen, Hieb links + Nachhieb, Leon stirbt F1134. 5080/5081: Fall, Auftritt, Rennen, Biss/Weichen am Westarm,
Griff, Flucht ab HP 0 (zurueck, Drehen, Clip 11), Raumtext msg 3; Leon und Elza gleich.

### N11. Offen

- Biss-Band 0xfa0..5000 ist meine Auslegung des Entscheids "+1700" (sonst kein Hieb mehr) - Bestaetigung Orchestrator. Folge im Spiel: auf
  4000-5000 beisst die Form, Hiebe erst naeher.
- Kamera Cut 13: steht Leon dicht (1300) vor dem Auge von Cut 12, greift die Wache nicht; die Form fuellt Cut 13 dann gross, Leon bleibt
  klein sichtbar (m4b Bild 524-548).
- Loecher zwischen den Beinen (N8) und Restspalte offline (25 Ansichten, 3-37 px).
- Kopfform ("Schildkroete statt Alligator") zurueckgestellt (E21-Nachtrag), Klaerungsblatt kopf_frontal_spiel.png.
- Integration mit r41/annette: dort Annette-Szene in 5090 vor dem Kampf (Cut 11/12, Hieb links Clip 4 trifft Annette). Diese Nachbesserung
  aendert in 5090 den Kampfstart-Schnitt (Auftritt in Cut 11 bis Clip 10 Bild 60), den Bisszweig der Nabe und den Koerper-Schub; Clip 4
  (Nummer, Bildzahl, Trefferbits 14-17) ist unveraendert, Trefferbild im Asset jetzt 16-17 statt 15-17 (Reichweite).
- Voller ctest (Unit/Prop/Probe): s. N12.

### N12. Testlage Endstand (re15_port/build, Stand fbef4bcc7)

- Gezielt: unit_birkin_bahnsteig, _szenen, _formen, unit_r41_form_emd gruen; integration_r41_birkin_pistole gruen (269 s): Leon und Elza je
  "Mutationen 0, Form-HP 150 bis 0, Rueckstoss 1, Sturz 1, G5 1, G5 tot 1".
- Unit/Prop/Probe voll (`ctest -R "^(unit|prop|probe)_" -j 8 --timeout 900`, 1945 s): **1054 von 1061 gruen**, 7 rot, alle
  Bau-Umgebungs-Skripttests ohne Spielcode: unit_r38_releaseskript_trocken/_ablauf/_fehler_bau/_fehler_umfeld/_cmd und
  unit_r39_titelversion_ablauf (Timeout 1200 s), unit_r35_android_bash_mutanten (rot nach 1444 s). Ursache der Timeouts gemessen: die Tests
  haengen in `df -Pk <Wegwerf-Repo>` (Platz-Pruefung von release_alles.sh) - Git-df.exe kehrt auf dieser Maschine seit dem Abend nicht
  zurueck (auch ein eigenes `df -h /c` um 19:5x hing); dieselben Tests haengen gleichzeitig in r41_moosgang (18:41), r41_gatorfix (19:42) und
  r41_sichel (20:27) an derselben Stelle. Diese Nachbesserung aendert nichts unter release/, android/ oder an diesen Tests (git diff leer).
  Wiederholung mit -j 3 brach mitsamt ctest ab; die haengenden eigenen Testprozesse (40 bash/df, nur Pfade unter r41_form) per PID beendet,
  8 df.exe-Leichen lassen sich nicht beenden (Kernelwarten).

Auftrag (Orchestrator, nach E19): J bekommt Gerippe-Pruefung, eigene Clips nach dem Muster RE2-G4, das G4-Verhalten aus Spur R auf die
neuen Clips, Einbau in 5080 und 5090 unter eigenem Asset-Namen, Tests, Sichtbelege. Bindend: `NUTZER_ENTSCHEIDE.md` E1-E19.
Arbeitsbaum `.claude/worktrees/r41_form`, Zweig `r41/form`. Dieses Dossier wird fortlaufend geschrieben; wer nach einem Abbruch
weitermacht, liest zuerst Abschnitt 0.

## 0. Stand und naechster Schritt

| Schritt | Stand |
|---|---|
| 1 Sichern + `git merge master` | erledigt: 8423920d4 (Formstand, `.gitignore` im Ordner), f89159cf4 (Merge, keine Konflikte) |
| 2 Gerippe pruefen | erledigt (Abschnitt 2): alle Teile haben Knochen, keine Aenderung an Form/Textur noetig |
| 3 Clips | gebaut (Abschnitt 3), Asset `stufe3/asset/BAHNSTEIG_FORM.EMD` = `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD` |
| 4 Angriffsmuster | Tick auf die neuen Clips umgestellt, Flucht 5080, Sturz 5090 (Abschnitt 4) |
| 5 Einbau, Tests | Lader, Paket-Gate, Tests angepasst (Abschnitt 5). Mit dem Endstand (Bau `re15_port/build_s3`): unit_birkin_bahnsteig, unit_birkin_szenen gruen, integration_r41_birkin_pistole gruen (Leon + Elza: keine Mutation, Form-HP 150 bis 0, Rueckstoss, Sturz, G5, G5 faellt) |
| 6 Sichtbelege | erledigt: `stufe3/kontakt/` (00_uebersicht + 12 Clips), `stufe3/spiel/` (7 Blaetter, echtes Fenster) |
| Voller ctest | Lauf in `re15_port/build` (Stand Commit 0e10882f9, Asset waehrend des Laufs getauscht): siehe Abschnitt 8 |
| Abnahme 1 | NICHT BESTANDEN (STUFE3_abnahme_1.md, M1-M8) |
| Nachbesserung 1 | Abschnitte N0-N12 oben: K3 im Spiel, Texturfix, M1-M8, Kopf-Folgen; Commits a452893f1 (Asset), fbef4bcc7 (C-Seite) |

Commits dieser Stufe: 8423920d4 (Sichern), f89159cf4 (Merge master), a7f9922d4 / 79aefb203 (Generator WIP), 0e10882f9 (Einbau,
Tests), 072d1d3f7 (Sichtbelege, Pistolen-Test nachgezogen), 6200b3d35 (Asset-Nachschliff, Uebergabepunkt G5).

Generator dieser Stufe: `stufe3/gen/` (`s3_rig.py` Gerippe/Posen/IK, `s3_clips.py` Asset, `s3_angriffe.py` Clips, `s3_g4.py`
G4-Clipdaten, `s3_pruef.py`/`s3_dreh.py`/`s3_reichweite.py` Zahlenpruefung, `s3_testkopf.py` Testreferenz, `s3_kontakt.py`/
`s3_uebersicht.py`/`s3_blatt.py` Kontaktblaetter, `s3_spielfolge.sh`/`s3_folge_blatt.py` Spielbilder). Zwischenbilder unter
`stufe3/roh/` (nicht versioniert, nicht fuer den Nutzer).

## 1. Bestand vor dieser Stufe (gemessen im Quelltext nach dem Merge)

**5080/5081 (Bahnsteig).** RDT sub00 @0x00746 `44 00 30 33 00 00 00 ff 4c b9 20 ea c8 00 ..` = Typ 0x30, grid 0x33, Lage
(-18100,-5600,200), solange Flag (3,48) = 0 (@0x0072A). grid 0x33 friert die Wurzel ein (`re15_birkin_root`, Bit 0x20), bis sub03
nach der Generator-Folge Pos_set (-18100,-5600,-17800) @0x00822, Speed_set/Add_speed (Fall auf y 0) und Member_set(0x0C,0x13)
@0x0083E faehrt. Port heute: Original-EM30 kaempft (`re15_birkin_ai_tick`), Tod -> Todesmaschine Phase 2 setzt Flag (5,0x1c)
(@0x8011aecc STAGE5), sub01 @0x0078A sieht das, setzt (3,48)/(3,49), Musik, sub04. **Die Bahnsteig-Form kommt in 5080 nicht vor.**

**5090/5091 (Zug).** sub00 @0x0124A Typ 0x30 grid 0x33, sub04 Member_set(0x0C,0x13) @0x0130A. Port heute: EM30 kaempft, toedlicher
Treffer -> Mutation (Sub 6, Clip 0x14) -> `re15_bf_mutation_fertig` laedt die Bank neu -> RE2/BAHNSTEIG_G4.EMD (G4 umgeformt,
27 Knochen) -> `re15_bb_tick` (engine/src/birkin_bahnsteig.c, Spur R) -> beim Tod Rueckstoss-Szene (`re15_bf_rueckstoss_start`,
birkin_formen.c) -> G5 (Typ 0x36). Belegt in R_bahnsteig.md 8.1 (Leon 5090, Elza 5091 gleich).

**Soll nach E5/E6/E17 (Nutzer):** 5080 Riesenform, Kampf endet mit Flucht; 5090 Riesenform, wird aus dem Zug geschossen; dann G5.
Die Mutation zur Riesenform liegt nach E6/E17 am Ende des letzten Sichel-Form-Kampfs (Spur SF, Baum r41_sichel), also VOR dem Bahnsteig.
Folge fuer diese Stufe: in 5080 und 5090 traegt der 0x30 die neue Form ab der Raumladung; die EM30-Runde samt Mutation in 5090 entfaellt.

## 2. Gerippe J

Gelesen aus `varianten/J_dicker/FORM.EMD` (`stufe3/gen/s3_lesen.py`): 32 Knochen (Port-Grenze RE15_EMD_MAX_BONES = 32, voll), 32 Meshes,
Mesh i an Knochen i, Eltern vor dem Kind.

| Knochen | Eltern | Rolle |
|---|---|---|
| 0 | - | Huefte, Wurzel |
| 1-6 | 0,1,..,5 | Schwanz, Kette von sechs Gliedern nach -X (Versaetze 592..678) |
| 7, 10 | 0, 7 | Wirbel, Brust (= BB_TEIL_RUMPF 10) |
| 8/24/25, 9/26/27 | 0/8/24, 0/9/26 | Bein links/rechts: Oberschenkel, Unterschenkel, Fuss (kantig, Sohle) |
| 11/12/13 | 10/11/12 | Arm links: Oberarm, Unterarm, Hand (= Teil 13, Hieb Seite 1); Klauen 14, 15, 28, 30 |
| 16/17/18 | 10/16/17 | Arm rechts: Oberarm, Unterarm, Hand (= Teil 18, Hieb Seite 0); Klauen 19, 20, 29, 31 |
| 21/22/23 | 10/21/22 | Hals, Kopf, Kiefer |

Alle Teile, die eine Bewegung braucht, haben schon einen eigenen Knochen; Teil 10/13/18 sitzen dort, wo der Tick sie erwartet. Neue Knochen
gingen ohnehin nicht (32 von 32). Posen von J: Clip 0 = Bild 1, Clip 1 = Bild 2, Clip 2 = Ruhe (`form.json`).

**Bewegungspruefung.** Die Form ist starr je Knochen (das EMD kennt keine Gewichte, Mesh i haengt an Knochen i): eine Drehung verzerrt nie
ein Glied, sie kann nur an Gelenken Spalten oder Ueberlappungen oeffnen. Die Posen Bild 1 und Bild 2 von J drehen Beine, Arme, Kopf und
Schwanz schon weit aus der Ruhe (Unterschenkel bis 100 Grad, Fuss bis 160 Grad) und bestehen die Formpruefung; die Clips bleiben in diesem
Bereich. An Gerippe, Mesh und Textur ist nichts geaendert (MD1 und TIM bytegleich mit J, Test). Zahlenpruefung je Clip:
`stufe3/gen/s3_pruef.py` (tiefster Punkt je Bild, Gleiten stehender Glieder in der Welt, groesste Drehung je Bild), `s3_dreh.py`
(Spruenge je Knochen), `s3_reichweite.py` (Hiebe gegen die Trefferprobe). Sichtpruefung: Kontaktblaetter, Abschnitt 6.

## 3. Clips (Asset BAHNSTEIG_FORM.EMD, 28 Clips, 760 Bilder)

Generator: `stufe3/gen/s3_clips.py` (ohne Argumente, schreibt `stufe3/asset/`, deterministisch: md5 zweier Laeufe gleich), Posen und
Angriffe `s3_angriffe.py`, Gerippe/IK `s3_rig.py`. Danach `cp stufe3/asset/BAHNSTEIG_FORM.* re15_port/shared_assets/RE2/` und
`python stufe3/gen/s3_testkopf.py` (Testreferenz `re15_port/tests/unit/form_j_posen.h`). `form_emd.py rundlauf` auf dem Asset:
byte-gleich, `FE.pruefe` 0 Fehler, 0 Warnungen.

**Aus G4 (Daten, kein Code):** RE2 CDEMD0.EMS kind 0x34 rec 3 (Sektor 3310, Datei 0x677000; EDD Paar 1 ab 0x67b960, EMR Paar 1 ab
0x67d05c). Je Tick-Clip unveraendert: Bildzahl, EDD-Bits 16..31 (Treffer Bit 16, Ton Bit 27 + Id) und Spur (Keyframe +6/+10). Der Test
vergleicht jedes Bild (Clip 10 ausgenommen, s.u.). Damit gelten im Tick unveraendert: Doppelschlag k_doppel @0x8010b0f0, Trefferbilder
@0x80101cb8 / @0x80104f58, Griff Bild 8 @0x80105408, Zucken ab Bild 5 @0x80107224, Lenkschwellen 21/31 @0x80102924-68, Ueberrennen 201
@0x80102e80, Gang 2666 Einheiten je Durchlauf.

**PORT-WAHL:** alle Posen und Zeitpunkte in den Clips, Fussarbeit, Schwanzwelle, die neuen Clips 2, 11, 12 samt Laenge, Spur und Toenen
(Ton-Ids aus der G4-Bank 24), die Platzhalter 1, 6, 7, 13-25 (je ein Bild Ruhepose, vom Tick nie gespielt).

| Clip | Rolle | Bilder | Treffer | Ton (Bild: Id) | Herkunft |
|---|---|---|---|---|---|
| 0 | Rennen (Sprunggalopp) | 18 | - | 17: 1 | G4 Clip 0; **Bild 11 = Pose Bild 1** (Rueckansicht Bahnsteig) |
| 2 | Stehen/Atmen | 60 | - | - | PORT-WAHL; Bild 0 = Ruhe von J |
| 3 | Hieb rechts (Teil 18) | 60 | 26-30 | 25: 1, 32: 0 | G4 Clip 3 |
| 4 | Hieb links (Teil 13) | 50 | 14-17 | 13: 1, 19: 0 | G4 Clip 4 |
| 5 | Gehen (auch Weichen rueckwaerts) | 89 | - | 43: 0, 73: 0 | G4 Clip 5 |
| 8 | Zucken/Treffer | 30 | - | - | G4 Clip 8 |
| 9 | Nachhieb (beidhaendig auf den Boden, Bruellen) | 63 | - | 1: 0, 24: 6 | G4 Clip 9 |
| 10 | Drohen/Auftritt | 126 | - | 30: 11, 51: 10, 74: 1, 79: 0, 81: 12, 113: 11, 114: 1 | G4 Clip 10; **Bild 45-52 = Pose Bild 2** |
| 11 | Flucht (Ende 5080): Gebruell, Galopp, Sprung nach oben | 57 | - | 5: 10, 37: 1 | PORT-WAHL |
| 12 | Sturz aus dem Zug (Ende 5090) | 56 | - | 2: 10 | PORT-WAHL; **Bild 0 = Pose Bild 2** |
| 26 | Biss | 35 | 16-30 | 19: 5, 27: 5 | G4 Clip 26 |
| 27 | Griff (Packen Bild 8, Schleudern, Bruellen) | 100 | - | 14: 8, 36: 9, 60: 9, 82: 7 | G4 Clip 27 |

Die Posen von J stehen bytegleich im Asset (Wurzel + 32 Winkel; Test `pose_gleich`): Ruhe = Clip 2 Bild 0, Bild 1 = Clip 0 Bild 11
(auch zweimal in der Flucht, die den Galopp wiederholt), Bild 2 = Clip 10 Bild 45-52 und Clip 12 Bild 0. **Formpruefung von J am
Spiel-Asset gelaufen** (`stufe3/formpruefung.json` -> `stufe3/formpruefung/bericht.txt`, `form_pruefung.py pruefe`): Bild 1 IoU 0,948,
Pruefpunkte 31/31, Boden/Verdeckung 5/7; Bild 2 IoU 0,935, Pruefpunkte 7/7, Boden/Verdeckung 9/9 - Zahl fuer Zahl der Bericht von J;
`FE.pruefe` jetzt 0 Fehler, 0 Warnungen (J hatte die Warnung "Tick erwartet Clips, die fehlen").

Abweichung von G4 mit Grund, **Clip 10 Spur**: Pose Bild 2 steht in Bild 45-52 still; G4 bewegt die Figur dort um zusammen rund -400.
Dieses Tempo liegt jetzt in Bild 53-60, die Summe (47) ist die von G4. Der Auftritt bedient keine Schwelle des Ticks.

**Wie gebaut:** Gehen = Seitenfolge-Gang, zwei Zyklen je 89 Bilder, Fuesse und Haende per IK in der Welt verriegelt gegen die G4-Spur
(Gleiten <= 8 je Bild), Fuss rollt ab, Schwanz als Welle. Rennen = Sprunggalopp, Glieder je Bild per IK ueber Stuetzpunkte, Basis Pose
Bild 1. Hiebe: Arm hoch und weit zur Seite, dann quer nach vorn unten; der Einstieg des Doppelschlags (Clip 3 Bild 20, Clip 4 Bild 7)
liegt ausserhalb der Trefferprobe (Teil 13/18 Radius 1700, Versatz (0,600,0) Radius 1500, Spieler 2600 voraus, `s3_reichweite.py`),
getroffen wird in Bild 28-30 bzw. 15-17. Biss: Kopf zurueck, Maul auf, Vorstoss in Brusthoehe des Spielers (Kiefer bei y -1460), Schuetteln. Griff: beide Arme greifen (Krallen zu in
Bild 8), Schleudern, Bruellen mit wechselnden Armen. Drohen: Hocke, Gebruell, Pose Bild 2, Aufbaeumen mit beiden Armen.

**Grenzen, gemessen (`s3_pruef.py`):** tiefster Punkt <= 25 unter dem Boden ausser Rennen Bild 11 (Pose Bild 1 selbst: linker Fuss 334
unter der Ebene, so wurde J an Bild 1 gefittet) und Drohen Bild 33-38 (Schienbein streift auf dem Weg in die Hocke bis 242, Bild 35-38; je Bein eigenes Faltfenster, damit der Rueckweg nicht ueber den Boden schleift: Gleiten 245 -> 23). Schweben bis
90 im Ausfall der Hiebe und im Aufbaeumen. Groesste Drehung je Bild: Gehen 12 Grad, Rennen bis 106 Grad (Fuss beim Abheben). Flucht und
Sturz verlassen absichtlich den Boden.

## 4. Angriffsmuster und Ablauf im Spiel

Tick `engine/src/birkin_bahnsteig.c` (Spur R): Sub-Nummern, Entscheidungen, Reichweiten, Kegel, Sperren, Schaden, Trefferzaehler
unveraendert mit ihren @0x-Adressen aus dem G4-Overlay (CDEMD0.EMS rec 1). Die Clipnummern BB_C_* bleiben, die Form bedient sie.
Neu in Stufe 3 (PORT-WAHL nach E5/E6/E17; RE-Zahlen nur, wo genannt):

| Stelle | Was | Beleg / Kennzeichnung |
|---|---|---|
| birkin_formen.c `re15_bf_riesenform_raum`, `re15_bf_bahnsteig_bank_faellig` | Riesenform in 5080/5081 und 5090/5091 ab der Raumladung; in 5090 keine EM30-Runde, keine Mutation (`re15_bf_mutation_offen` = 0) | E5/E6/E17 |
| birkin_bahnsteig.c `bb_flucht` (Sub 21) | Tod in 5080/5081 -> Clip 11: abdrehen (128 je Bild im Gebruell, Gestaltung; danach 32 wie @0x801018ec-908), Galopp, Sprung nach oben; am Clip-Ende Flag (5,0x1c) wie der Birkin-Tod @0x8011aecc (STAGE5), den ROOM5080 sub01 @0x0078A abfragt; Aktor aus. Treffer halten die Flucht nicht auf. Riegel wie im Todeszweig @0x8011a458 / @0x8011a54c-50 | PORT-WAHL |
| birkin_formen.c Rueckstoss Phase 3, `re15_bb_sturz_tick` | am Zugende Clip 12 einmal (Spur schiebt rueckwaerts hinaus), Kamera Cut 11, dann Phase 2 (unsichtbar, Pause), G5 wie bisher | PORT-WAHL |
| enemy_ai_common.c `re15_birkin_root`, `re15_bb_wartet` | solange grid & 0x20 die Wurzel einfriert (@0x80116a7c-a88), spielt die Riesenform Clip 2 im Kreis (5080: Fall vom Dach, 5090: steht im Wagen) | PORT-WAHL |
| birkin_formen.c Messhaken `RE15_BIRKIN_HP_AT` | gilt jetzt auch in 5080 | Messhilfe |

Unveraendert: Schatten 2300 x 1300 (G4 @0x801002cc / @0x80100300) und Kastenversatz 800 (Gestaltung Spur R); die Form ist ohne Schwanz
rund 3600 lang, Haende bei z +-1590. Opfer-Bank EM30. Teile 10/13/18 = Brust / Hand links / Hand rechts.

**Uebergabe an den G5 (Abgrenzung zur Spur r41/g5auftritt, Vorgabe Orchestrator).** Diese Stufe baut in 5090 die Riesenform bis
einschliesslich ihres Sturzes aus dem Zug. Der Uebergabepunkt ist EINE Funktion in birkin_formen.c:
`bf_rs_form_ist_draussen(e, mit_gebruell)` - aufgerufen genau einmal, wenn die Form den Wagen verlassen hat (Riesenform: Ende von
Sturz-Clip 12 in Rueckstoss-Phase 3; ohne Riesenform, z. B. `RE15_BIRKIN_FORM=original`: am Wagenende wie bisher, mit Gebruell). Sie
setzt Phase 2 (Form unsichtbar, Pause `BF_RS_PAUSE`), danach laufen unveraendert `re15_bf_morph_zu_g5` und die Szenenkamera `s_kam_g5`.
Nicht angefasst: G5-Auftritt, G5-Kamera, `enemy_ai_boss_g5.c`. Wer den neuen G5-Auftritt baut, setzt an `bf_rs_form_ist_draussen` an.
Ablauf der Phasen in 5090: 1 Beschuss/Zurueckdraengen (Clip 8 im Kreis) -> 3 Sturz (Clip 12, Kamera Cut 11) -> Uebergabe -> 2 Pause -> G5.

## 5. Einbau und Tests

- Asset `re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM`; `BAHNSTEIG_G4.EMD/.TIM` aus dem Repo entfernt (nicht mehr geladen;
  `tools/birkin_bahnsteig.py` bleibt als Beleg von Spur R). Lader `pc_enemy_load_bahnsteig` (main.c), debug.log
  `Bahnsteig-Form: 32 Meshes, 32 Knochen, 28 Clips`.
- Paket-Gate `release/make_package.sh`: BAHNSTEIG_FORM.EMD/.TIM statt BAHNSTEIG_G4.* (vorhanden, nicht leer, bytegleich).
- `form_spiel.sh`, `form_emd.py rundlauf` (Vorgabedatei) auf dem neuen Namen.
- `tests/unit/test_birkin_bahnsteig.c`: `test_asset` neu (Formtreue gegen `form_j_posen.h`: MD1/TIM-CRC, drei Posen; Clips gegen RE2-G4:
  Bildzahl, EDD-Bits, Spur; Mindestlaengen 31/21/6/9; Trefferbits 3/4/26; Tempo Gehen >= 31, Rennen + 10 >= 201; Stehen/Flucht/Sturz),
  `test_masse` (Boden mit begruendeten Ausnahmen, Hoehe Clip 5 = 3126 +-60 als Formtreue-Pin), neu `test_flucht` (5080, 5081) und
  `test_wartet` (5080, 5090, 5091). Entfallen: M1/M3/M4 (Tausch EM30 -> Form nach der Mutation in 5090, den es nicht mehr gibt).
  Arrays von 27 auf 32 Knochen.
- `tests/unit/test_birkin_szenen.c`: `test_bahnsteig` ohne EM30-Mutation, prueft den Sturz-Clip; `test_modellwahl` mit 5080/5081/5091/50A0.
- `tests/integration/test_r41_birkin_pistole.cmake`: 32 Meshes, keine Mutation, Sturz (Clip 12) zwischen Rueckstoss und G5.

## 6. Sichtbelege (fuer den Nutzer)

**Kontaktblaetter je Clip** `stufe3/kontakt/clip_NN_name.png` (`stufe3/gen/s3_kontakt.py`, Offline-Buehne `form_buehne.py`, Textur und
Gouraud ohne Raumlicht): Zeilen = rechte Seite, schraeg vorn links, schraeg hinten rechts; Spalten = Bilder des Clips; die Figur laeuft um
ihre Spur weiter, die Kamera folgt, das Bodenraster steht in der Welt (Gleiten waere sichtbar). Marken unter dem Bild: T = Trefferbild,
S<n> = Ton-Id. Flucht und Sturz mit fester Kamera.

**Spiel, echtes Fenster** (`stufe3/gen/s3_spielfolge.sh`: eigene exe-Kopie, Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER,
ffmpeg gdigrab `hwnd=` auf das eigene Fenster, nur die eigene PID beendet; Blaetter mit `s3_folge_blatt.py`) unter `stufe3/spiel/`:

| Blatt | Lauf | Inhalt |
|---|---|---|
| `5090_leon_1_auftritt_angriffe.png` | Debug-Sprung 5090, Leon laeuft in den Wagen, 10 s ohne Feuer | Auftritt (Drohen) im Gang, Gang, Doppelhieb, Nachhieb, Griff |
| `5090_leon_2_rueckstoss_sturz_g5.png` | danach Zielen + Feuer gehalten (Pistole) | Rueckstoss-Szene, Sturz aus dem Wagenende in Cut 11 (Kamera von Bild 2), RE2-G5 kommt herein |
| `5080_leon_1_fall_kampf.png` | Debug-Sprung 5080, Generator-Folge (Skript lauf1 aus Runde 30) | Riesenform faellt nach dem Generator herab, Auftritt, Angriffe an Leon |
| `5080_leon_2_flucht.png` | Messhaken `RE15_BIRKIN_HP_AT=1100` (HP 0 statt langem Beschuss) | Flucht: abdrehen, Galopp aus dem Bild, danach Raumtext msg 3 wie nach dem Birkin-Tod |
| `5091_elza_uebersicht.png`, `5081_elza_uebersicht.png` | dieselben Laeufe mit Elza (`RE15_TITLE_SHOT_CHAR=1`) | gleicher Ablauf |

Gemessen in diesen Laeufen (birkin_dbg.log, state.log je Lauf unter `re15_port/build/s3_spiel/<name>/`): debug.log `Bahnsteig-Form: 32
Meshes, 32 Knochen, 28 Clips` in allen vier Raeumen. 5090/5091: Auftritt 126 Bilder, Gang, Hieb 3 -> Doppel 4 -> Nachhieb 9, Griff (Leon
fliegt), Treffer bis HP 0, Rueckstoss (Clip 8, HP 1), Sturz (Clip 12 bei x -6290 -> -15320), G5 (Typ 0x36). 5080/5081: nach der Generator-
Folge (msg 0/1 F247/F500) Fall vom Dach, Auftritt, Rennen, Weichen, Griff, Biss, Hieb; Flucht (Sub 21, Clip 11) bei F1694, fort bei
F1749, msg 3 bei F1751 (sub04 nach Flag (5,0x1c)). Elza 5081 und 5091 identisch.

Beobachtet: In Cut 12 (Kamera vor Leon) ist die Form im Nahkampf meist nur angeschnitten (sie kommt von hinter der Kamera); im Gang
(Cut 14/15) und am Generator ist sie ganz zu sehen. Die Form ist so gross, wie J nach den Screenshots ist (Hoehe im Gehen 3126); sie
fuellt den Wagen fast aus.

## 7. Offen

- Sturz-Clip 12 nach dem ersten Spiellauf ueberarbeitet (stand auf der Schwanzspitze senkrecht, weil Pose Bild 2 schon 52 Grad aufgerichtet
  ist): jetzt hoechstens 22 Grad dazu, Fall mit Fallkurve. Im Spiel neu zu sehen (Abschnitt 0).
- Drohen Bild 33-38: Schienbein streift bis 242 unter den Boden (Bild 35-38) (Weg in die Hocke von Bild 2), Rennen Bild 11 = Pose Bild 1 mit Fuss 334
  unter der Ebene (so an Bild 1 gefittet). Beides im Test als begruendete Ausnahme.
- Die Flucht laeuft vom Spieler weg; die Wandklemme der Wurzel bleibt an, an einer Wand rutscht die Form seitlich weiter, bis der Sprung
  sie aus dem Bild nimmt (5080 gemessen: von z -22553 bis -14139).
- Integration in master und Abgleich mit Spur SF (r41/sichel: Mutation am Ende der Sichel-Form, Effekte) stehen aus; beide Zweige aendern
  birkin_formen.c, main.c, test_birkin_szenen.c.

## 8. Testlage (gemessen 2026-11-02)

- Voller ctest in `re15_port/build` (Bau von Commit 0e10882f9, `-j 6`, Start 09:55, Dauer 14823 s): **1407 von 1408 gruen, 1 rot**:
  `integration_r41_birkin_pistole`. Ursache gemessen: die Anpassung dieses Tests aus 0e10882f9 war nicht im Commit (Patch lief im
  Hintergrund ohne Wirkung), der Lauf pruefte noch "Mutation erwartet 1" / "27 Meshes". Nachgezogen in 072d1d3f7; mit dem Endstand
  (Bau `re15_port/build_s3`, Asset 6200b3d35) **gruen**: Leon und Elza je Mutationen 0, Form-HP 150 bis 0, Rueckstoss 1, Sturz 1, G5 1,
  G5 tot 1.
- Mit dem Endstand neu gebaut und gelaufen (`build_s3`): `unit_birkin_bahnsteig` OK, `unit_birkin_szenen` OK.
- Gruen im vollen Lauf u. a. alle Birkin-/G5-Tests (`unit_birkin_*`, `unit_r41_birkin_wand_*`, `probe_p2_birkin_g5`, `unit_g5_*`,
  `unit_r30_n_room5080_birkin_wand`, `integration_r35_zgirl_5090`) und die Raumlaeufe `integration_r37_elzaems_leon5080`, `_elza5081`,
  `_elza5091`, `unit_r41_form_emd`.
