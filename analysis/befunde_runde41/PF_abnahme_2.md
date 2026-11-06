# Runde 41 — Spur PF "panelfix": Abnahme 2 (Nachbesserung 1)

Pruefer: unabhaengiger Abnahme-Agent. Baum `.claude/worktrees/r41_panelfix`, Zweig `r41/panelfix`, Kopf e6f9bf240 (Code bb82c0a7c), Gegenprobe 422048234
(Stand vor NB1). Alle Messungen mit eigenen Werkzeugen (`PF_belege/abnahme2/werkzeuge/`), keine Zahl des Autors uebernommen.

Massstab (Nutzer woertlich): 2026-11-02 00:26 "... der Kasten ist bei der Kamerasicht irgendwie komisch gestaucht ... Zahlen und Texte auf den Panel in
Nahansicht sind kaum lesbar/verschwommen ... der Text am Anfang vor dem Start des Raetsels ueberlappt mit dem yes / no auswahl."; 2026-11-02 09:24 "Das Panel
ist immer noch schwer lesbar/verpixelt. Vor allem das A 100, und das 80 links, das Dreh panel, der BRIDGE und PRESSURE 80 Text, der B 50 und der C30 Text und
auch der Caution High Pressure Text."; 2026-11-03 zur Schrift: "ja, ist super." (= gelobter Stand, darf nicht schlechter werden).

## Urteil

**BESTANDEN** (Tests siehe §5).

| Punkt | Urteil | Kernbeleg (eigene Messung im Fenster) |
|---|---|---|
| Kasten 2010/2011, Scherung | behoben | Achsenwinkel des Inhalts gegen 2060: vor NB1 95,1 / 94,7 Grad, nach NB1 89,8 / 89,9 Grad (Rueckwand / Fluegel) |
| Kasten, Seitenverhaeltnis | gut | V nach NB1 0,990 (Rueckwand), 0,983 (Fluegel; 0,97..1,00 je nach Fluchtpunkt 129,8..132,8) |
| Kasten im Raumbild | sauber | keine Naht, Kanten = Projektion (<= 2 Fensterpunkte), geschlossen bitgleich BG03, Helligkeit -0,5 / -1,2 Stufen |
| Kasten, andere Kameras | nur Cut 3 | Cut 0/1/2 nicht im Bild, Cut 4: Spalten 0..27 = BG04 (100 %), Cut 5: keine RVD-Zone fuehrt hinein |
| Panel-Schrift (8 genannte Texte) | lesbar, Tinte bitgleich | 1334/1334 Tintentexel = Spielschrift, Erkennung 42/42 Zeichen, vor/nach Loesung, Leon + Elza |
| Panel-Schrift gegen 422048234 | Tinte bitgleich, Grund fast | 0 Tintentexel verschieden; 73 Grundtexel in den Zellen von B 50 / C 30 / 80 um <= 16/255 (H1) |
| Drehscheiben, Instrument, Zeiger | scharf, kein Flackern | 285/288 Stempeltexel im Fenster (Rest unter dem Zeiger), Zeiger-Zwischentoene 0 (vorher bis 51), A-B-A 0 |
| Frage 40 gegen Ja/Nein | frei | Zeile 2 traegt rechts von x 150 nur Cursor 161..163, Yes 175..196, No ..259; vor/nach NB1 bitgleich |

Maengel: keine. Hinweise H1..H4 in §6.

## 1. Bau und Messweg

* **Eigener Bau** `re15_port/build_abn2` im Arbeitsbaum: `RE15_BUILD_DIR=<wt>/re15_port/build_abn2 bash re15_port/tools/local_build.sh configure`, dann
  `... local_build.sh build` -> `=== LOCAL-BUILD-OK (build)` (1710 Schritte). SDL-Quelle ueber einen vorbelegten Cache-Eintrag
  `FETCHCONTENT_SOURCE_DIR_SDL2=<wt>/re15_port/build/_deps/sdl2-src` (kein Download; anfangs ~250 MB frei auf C:). exe `build_abn2/platform/pc/re15_pc.exe`
  2026-11-03 10:17:25, 8 693 825 Bytes. Fuer die ersten Messlaeufe nur die Ziele re15_pc, test_r41_nachtrag30, test_r41_ventilkette, probe_bg_dump mit
  demselben PATH wie local_build.sh (`werkzeuge/bau_ziele.sh`); die Voll-Bauung danach liess die exe unveraendert (gleicher Zeitstempel).
* **Gegenprobe-exe 422048234**: `git archive 422048234` (CMakeLists, cmake, engine, include, platform) in den scratchpad, `shared_assets` als Junction auf den
  Arbeitsbaum, Tests aus, eigener Bau -> `re15_pc.exe` 10:23, 8 691 094 Bytes. Unterschied der Quellen 422048234 -> e6f9bf240 im Spiel: nur
  `gen/panel2030_r41.inc`, `gen/schrank2010_r41.inc`, `panel2030_r41.c`, `re15_panel2030_r41.h`.
* **Fensterlaeufe** (`werkzeuge/lauf.py`): eigene exe-Kopie `re15_pc_pfabn2_*`, Fenster per EnumWindows der EIGENEN PID, `ffmpeg -f gdigrab -i hwnd=<h>`
  verlustfrei als Video (libx264rgb -qp 0, Auswertung ueber eine rawvideo-Pipe, keine Einzelbilder auf der Platte), Renderer `direct3d`
  (`[render] SDL-Renderer direct3d`), Fenster 960x720 (Standard), **kein** RE15_AUTOSHOT / RE15_SOFTWARE_RENDER / RE15_FRAMEDUMP, SDL_ASSERT=always_ignore.
  Logisches Bild = Mitte jedes 3x3-Blocks.
  * Kasten (`k_*`, 4 Bilder/s, 60..80 s): Spielstand `test_r41_nachtrag30 karte` ROOM2010/2011 (-4600,-4950) + RE15_CONTINUE_TEST/RE15_CARD_AUTO, dann zu Fuss
    per RE15_WEGPLAN (`plan_k_leon.txt` / `plan_k_elza.txt`): quer durch den Raum, nach (3807,-942) unter Cut 3, Blick 2048 (Tuer ZU), zum Kasten, Aktion
    (Tuer klappt auf, Text 40, zwei Seiten weg), zurueck nach (3807,-942) (Tuer AUF), nach (3500,-3600) = Cut 4, zurueck unter Cut 3.
  * Panel (`p_*`, 30 Bilder/s, 75 s): Spielstand ROOM2070/2071 mit den Spinnen-Bits 7:155..157, Ring-Tuer per RE15_FIRE_AOT (`[aot] DOOR FIRE slot=1`,
    `loaded room2030.rdt`/`room2031.rdt`), zu Fuss zum Pult (`plan_p_*.txt`), ab Spielbild 400 die Tasten `skript_p.txt` (Aktion, Seite, Ja, Text 41, A->B,
    B->C, C->A = 80, Text 43, Bruecke). Je Lauf: Frage 40, Panel offen, Loesung `GELOEST: A 80 B 20 C 0`, Text 43/44.
  * Je Figur und Stand ein Lauf: Leon/Elza x vor/nach NB1 (8 Laeufe). Elza: `Spieler-Familie PL04` im debug.log.
* Grenze des Wegs wie in Abnahme 1: Laden ueber RE15_CONTINUE_TEST statt Titelmenue, Ring-Tuer per RE15_FIRE_AOT. Raum, Kamera, Kasten, Pult, Frage und Panel
  laufen ueber den Spielweg.

## 2. Kasten ROOM2010/2011

### 2a. Kamera und Sichtbarkeit (eigene Projektion, `werkzeuge/kam.py`)

Kamera aus der RDT: Kopf +0x24 -> Tabelle @0x60, 32 Bytes je Satz (u16 Kennung, u16 fov, i32 Auge, i32 Ziel, u32); Achsen wie `re15_camera_build_view`
(FUN_80053ca4 @0x80053ca4), sx = 160 + H*x/z, sy = 120 + H*y/z, H = fov >> 7 = 26684 >> 7 = 208. ROOM2010 = ROOM2011 (alle sechs Saetze gleich).
Cut 3 @0xC0: Auge (2930,-2827,-6385), Ziel (7022,962,10672). ROOM2060 Cut 6 @0x120:
`00 00 3c 68 e6 eb ff ff 74 f5 ff ff 46 fe ff ff 89 18 00 00 42 0b 00 00 28 49 00 00 60 06 00 00` (Auge (-5146,-2700,-442), Ziel (6281,2882,18728)).
Port-Flaechen (`schrank_bild.py` X_WAND 5400, Z -2000..-3300, Y -3650..-50; Fluegel offen = rot_y 1024 um die Angel): je 81 Punkte projiziert.

| Cut | Rueckwand | Fluegel offen | im Fenster gemessen |
|---|---|---|---|
| 0 @0x60, 1 @0x80, 2 @0xA0 | 0/81 | 0/81 | — |
| 3 @0xC0 | 81/81, x 214..265, y 22..220 | 81/81, x 165..223, y 34..193 | Messung 2b..2d |
| 4 @0xE0 | 17/81 (Rand links, x 1..23) | 43/81 (Rueckseite) | Spalten 0..27 = BG04: zu 12/14 und auf 19/20 Bilder 100 % (Leon nach), die uebrigen = Figur im Bild bzw. Schnittbild beim Cut-Wechsel; vor/nach, Leon/Elza gleich |
| 5 @0x100 (Kennung 1) | 81/81 in 13 x 31 Punkten | 81/81 in 6 x 32 Punkten | keine Kamerazone fuehrt hinein: RVD (Kopf +0x28 -> @0x148, 18 Zonen zu 20 Bytes, Byte 3 = Ziel-Cut) nennt nur die Ziel-Cuts 0..4, ROOM2010 = ROOM2011; im Lauf nicht erreicht |

Der Kasten ist also nur unter Cut 3 als Port-Flaeche zu sehen; unter Cut 4 bleibt der Original-Hintergrund (PORT-WAHL der Spur N), unveraendert durch NB1.

### 2b. Was NB1 im Fenster aendert

* Tuer ZU (Cut 3, t = 18,25 s bzw. Tuer-auf - 5 s): vor NB1 = nach NB1 **bitgleich** (0 Punkte verschieden), Leon und Elza; die Schrankflaeche ist gleich dem
  Original-Hintergrund BG03 (mittlere Abweichung 0,0).
* Tuer AUF: vor/nach verschieden nur in logisch x 164..265, y 22..218 = genau die Projektion der zwei Innenflaechen (93 892 Fensterpunkte). Leon gegen Elza
  (nach NB1): verschieden nur in x 123..160 (die Figur). Die entzerrten Flaechen sind fuer Leon und Elza bildgleich (vor und nach).
* Bytes: `re15_r41n_schrank_innen_md1`, `_aussen_md1`, `_tuer_md1` und `_aussen_tim` 422048234 = e6f9bf240; geaendert nur `_innen_tim`, `_tuer_tim`.

### 2c. Scherung und Seitenverhaeltnis (eigenes Werkzeug)

Verfahren (`kasten_mass.py`, `referenz.py`, `affin.py`, `mass_affin.py`):
1. Fensterbild (960x720) ueber Cut 3 auf die Port-Flaechen entzerrt, 4 Welteinheiten je Punkt (bilinear, Texelmitten).
2. Vorlage: ROOM2060 BG06 (Engine-Dekoder `probe_bg_dump`) ueber Cut 6 entzerrt. Rueckwand: Ebene z = 4500 (achsparallel). Fluegel: senkrechte Ebene durch
   (-4830, *, 4500), Richtung aus dem **eigenen** Fluchtpunkt der Schildkanten (`fluchtpunkt2.py`, Kanten je Spalte subpixel, Gerade per kleinster Quadrate,
   Schnitt mit dem Horizont y 67,975 der Kamera): weiss unten 131,7 (Rest 0,11), gelb oben 130,1..130,9 (0,60), gelb unten 130,7..130,8 (0,94),
   weiss oben 139,7..141,0 (0,60; die obere Kante des weissen Schilds ist rechts oben verdeckt) -> **Median 131,3**. (Autor 130,8; Abnahme 1 aus den
   Handkanten 139,7.) Senkrechter Fluchtpunkt der Kamera (160, 951,6).
3. Affine Registrierung Port -> Vorlage (eigene NCC + Powell, Startwert aus einem Massstabsgitter): Port (u,v) -> x = a u + b v + tx, y = c u + d v + ty.
   Neigung der Senkrechten atan(b/d), der Waagrechten atan(c/a), Achsenwinkel, V = |(a,c)| / |(b,d)| (1 = unverzerrt).
   Gegenprobe: das Nachher-Bild kuenstlich um 5 Grad geschert -> gemessen +4,99 / -4,91 Grad Unterschied (Rueckwand / Fluegel).

| Flaeche | Stand | NCC | Senkrechte | Waagrechte | Achsenwinkel | V |
|---|---|---|---|---|---|---|
| Rueckwand | vor NB1 (422048234) | 0,940 | -1,08 Grad | **-4,00 Grad** | **95,08 Grad** | 0,981 |
| Rueckwand | nach NB1 | 0,950 | +0,30 Grad | -0,08 Grad | 89,78 Grad | 0,990 |
| Fluegel | vor NB1 | 0,982 | **-6,50 Grad** | +1,79 Grad | **94,71 Grad** | 1,063 |
| Fluegel | nach NB1 | 0,985 | +0,11 Grad | -0,00 Grad | 89,89 Grad | 0,983 |

(Leon; Elza bildgleich.) Die Gegenprobe zeigt die Scherung vor NB1 deutlich (Rueckwand-Waagrechte 4,0 Grad, Fluegel-Senkrechte 6,5 Grad, je rund 5 Grad
Achsenfehler); nach NB1 liegt sie unter 0,3 Grad.
Fluegel-V haengt am Fluchtpunkt der Vorlage: 129,8 -> 0,998; 131,3 -> 0,983; 132,8 -> 0,968; 139,7 -> 0,905 (dort vor NB1 0,977). Mit dem gemessenen Bereich
130..132 liegt V bei 0,97..1,00. Die Waagrechte des Fluegels nach NB1 geht genau bei 131,3 durch 0 (-0,00), passend zum Fluchtpunkt der Schilder.
Grobmass (Projektionsprofil der Senkrechten, `kasten_mass.winkel_profil`): vor 2,2 / 3,3 Grad, nach 0,5 / 0,35 Grad, 2060 allein 0,65 / 0,05 Grad; fuer die
Waagrechten liefert das Profil auf diesem dunklen Inhalt keinen klaren Gipfel, darum gilt die Registrierung.
Bild: `PF_belege/abnahme2/kasten_entzerrt_vor_nach_2060.png` (Gitter 200 Welteinheiten; vor NB1 laufen Rahmen und Roehren schraeg durchs Gitter, die
Schilder auf dem Fluegel sind Parallelogramme; nach NB1 wie 2060).

### 2d. Sitz im Raumbild (`werkzeuge/sitz.py`, Fenster 960x720, Tuer auf, Cut 3)

| Mass | vor NB1 | nach NB1 |
|---|---|---|
| Innenpunkte gleich BG03 (Hintergrund scheint durch), Rueckwand | 113 von 55 278 | 126 von 55 278 |
| dto. Fluegel | 308 von 70 662 | 408 von 70 662 |
| davon auf den Gitterlinien 4 x 8 (Flaechenanteil der Linien 20 % / 17 %) | 20 % / 17 % | 12 % / 23 % |
| groesste zusammenhaengende Gruppe | — | 36 Punkte, Ausdehnung <= 13 Fensterpunkte |
| Abweichung vom BG03 ausserhalb der projizierten Flaechen (x >= 500) | 200 Punkte | 136 Punkte, alle <= 2,0 Fensterpunkte (Median 1,0) von der Kante |
| mittlere Helligkeit Rueckwand / Fluegel (0..255) | 30,3 / 22,2 | 29,8 / 21,0 |

Die BG03-gleichen Punkte liegen verstreut in dunklen Stellen und nicht gehaeuft auf den Gitterlinien: **keine Naht**. Die Kanten liegen auf der
Projektion der unveraenderten Geometrie (Rasterung <= 2 Fensterpunkte = 0,67 logische): **kein Versatz**. Helligkeit: -0,5 / -1,2 Stufen, die Flaechen
tragen dieselbe Beleuchtung wie vorher: **keine falsche Helligkeit**. Bild `kasten_cut3_vor_nach.png` (oben offen, unten geschlossen; Leon und Elza).

## 3. Panel 2030/2031 in der Nahansicht

`werkzeuge/panel_mass.py` (Lauf, inc des Stands). Panel-Bilder = Bilder, deren logisches Bild zu > 60 % gleich dem Grundbild `re15_r41n_panel_grund` des
Stands ist (Leon nach 661, vor 662; Elza nach 926, vor 662). Der erste Elza-Nachher-Lauf lief unter Last langsamer (300 Spieltakte in 14,0 s statt 10,1 s;
gleiche Spielbilder) und endete vor Text 44; ein zweiter Lauf (85 s) zeigt den ganzen Ablauf bis `id=44 "The bridge is down."` mit denselben Werten
(729 Panel-Bilder, 1334/1334, Erkennung 42/42, Zeiger-Zwischentoene 0, A-B-A 0, Winkel 134,2 -> 2,6 -> 80,9 Grad). In allen Laeufen ist jeder 3x3-Block
einfarbig (Anteil 1,0000): nearest, 1:1 in die logische Groesse. Ergebnis in jedem Lauf: `GELOEST: A 80 B 20 C 0`, Text 43, Text 44.

### 3a. Die genannten Texte, vor und nach der Loesung

Je Glyphe aus der Tabelle `re15_r41n_panel_glyphen[42]` (Lage, Zeichen, CLUT; Tabelle 422048234 = e6f9bf240): Tintentexel im Fenster gegen das
Grundbild, und **eigene Zeichenerkennung** gegen die selbst dekodierte 8x8-Schrift aus `DATA/TEX.TIM` (`schrift.py`: 4 bpp, Zeichen c bei
u = 256 + ((c-0x20) & 0x1f)*8, v = ((c-0x20) & 0xe0) >> 5 * 8, FUN_80029214 @0x8002931c..@0x8002932c; je Glyphen-Index genau eine Farbe, Nachbarglyphen
ausgeblendet, CLUT-transparente Indizes = Grund).

| Lauf | Zustand | Tinte = Spielschrift | erkannt |
|---|---|---|---|
| Leon nach NB1 | A 100, vor der Loesung (t 43,50) | 1334/1334 | `A100 B50 C30 80 BRIDGE SET80 CAUTION HIGHPRESSURE` |
| Leon nach NB1 | A 80, geloest (t 54,87) | 1334/1334 | dito |
| Elza nach NB1 | vor / nach der Loesung | 1334/1334 / 1334/1334 | dito |
| Leon / Elza vor NB1 | vor / nach der Loesung | je 1334/1334 | dito |

Alle acht vom Nutzer genannten Texte (A 100, 80 links, BRIDGE, PRESSURE 80 = "SET 80", B 50, C 30, CAUTION / HIGH PRESSURE) werden in jedem Bild Zeichen fuer
Zeichen erkannt. Bild `panel_schrift_vor_nach_zoom6.png` (je Text vor | nach NB1, 6-fach).

### 3b. Bitgleich mit dem Stand vor NB1 (422048234)?

Fensterbild vor NB1 gegen nach NB1, gleicher Zustand (Leon und Elza je vor und nach der Loesung):

| Text | Tintentexel verschieden | Grundtexel in den 8x8-Zellen verschieden | Kontrast Tinte/Grund vor -> nach |
|---|---|---|---|
| A 100 | 0 | 0 | 184,8 -> 184,8 |
| B 50 | 0 | 27 (max 8/255) | 186,8 -> 186,7 |
| C 30 | 0 | 35 (max 16/255) | 190,3 -> 190,0 |
| 80 links (rot) | 0 | 11 (max 8/255) | 90,5 -> 90,5 |
| BRIDGE, SET 80, CAUTION, HIGH PRESSURE | 0 | 0 | gleich |

Die Schrift selbst ist bitgleich; drei Zellen haben leicht andere Grundtexel (H1). Insgesamt verschieden: 7776 Texel in zwei Bereichen — Instrument
x 91..151 y 62..111 (1663) und Drehscheiben x 96..257 y 128..192 (5720) — plus kleine Bloecke daneben; dieselbe Verteilung wie im Grundbild der inc
(7798 Texel).

### 3c. Drehscheiben, Instrument, Zeiger, Loesungsfahrt

* Gestempelte Texel (`re15_r41n_panel_stempel[288]`) im Fenster: nach NB1 285/288 (A 100) bzw. 283/288 (A 80), die fehlenden liegen alle unter Zeiger oder
  Zeigerschatten (Abstand 9,9..12,7 vom Drehpunkt); vor NB1 0/288.
* Zeigerbereich (Kreis r 13,5 um 124,5/87,5): Texel, die weder Zeigerfarbe (16,15,15), Nabe (96,96,96) noch Grund mit Schatten 87/256 sind: nach NB1
  **max 0** in allen 661 bzw. 926 Panel-Bildern; vor NB1 max 51 (Mittel 39,9).
* Flackern: A-B-A-Muster (Bild t-1 = t+1 != t) im Zeigerbereich 0, in den Drehscheiben 0; die Drehscheiben haben ueber alle Panel-Bilder genau EINE Fassung
  (vor und nach NB1, Leon und Elza). Texelweise ueber das ganze Panel (`aba2.py`/`aba3.py`, doppelte Aufnahmebilder zusammengefasst): Wechsel A-B-A gibt es
  nur, wo sich etwas bewegt — Zeiger waehrend der zwei Fahrten (47,13..47,60 und 53,97..54,43 s, der Zeiger streicht ueber die Texel), Cursor
  (x 70..189, y 108..128) und der Seitenpfeil von Text 41 (x 155..165, y 222..228) — vor NB1 in denselben Bereichen und gleicher Menge (8047 / 7723 Texel).
  Im Stillstand wechselt nichts. **Nichts Neues flackert.**
* Springen: der Zeiger laeuft je Fahrt in 16 Spielbildern, A -> B 134,2 -> 2,6 Grad (Schritte 5,9..9,5), C -> A 2,6 -> 80,9 Grad (2,6..6,5), genau ein
  Richtungswechsel (zwischen den Fahrten), bei B -> C steht er. Elza: dieselbe Winkelfolge (mit doppelten Aufnahmebildern durch den langsameren Lauf).
* Bilder: `panel_vor_nach_leon_elza.png` (ganzes Panel, vor | nach NB1, je vor und nach der Loesung), `panel_scheiben_vor_nach_zoom5.png`,
  `zeiger_fahrt_vor_nach.png` (9 aufeinanderfolgende Spielbilder der Fahrt A -> B, vor | nach NB1, Leon und Elza).

## 4. Text vor dem Raetsel gegen die Ja/Nein-Auswahl

Fragebild Seite 2 (Bilder mit "Yes" und "No": Leon 13, Elza 18 bzw. 14), Textpunkte = hell (min > 150) und verschieden vom Bild vor dem Text:
Zeile 1 (y 176..191) "Will you operate the panel?" x 35..216; Zeile 2 (y 192..207): Cursor 161..163, "Yes" 175..196, "No" bis 259; daneben nur
x 65..66 und 100..102 = Unterlaengen von "y" und "p" aus Zeile 1. Seite 1 "A control panel for the bridge." mit Seitenpfeil. Vor NB1 = nach NB1 bitgleich
(Leon, Elza). **Keine Ueberlappung.** Der helle Fleck links vor "No" (x 232..244, y 194..205) ist eine Lampe des Raumbilds (98 % der Punkte gleich dem
Bild vor dem Text), keine Schrift. Bild `frage40_vor_nach.png`.

## 5. Tests

Nachgeliefert 2026-11-06 (Auftrag PF, Abnahme 2 Par. 5). Bau `re15_port/build` im Arbeitsbaum (Stand 4dae40c04 = Abnahme-Stand 4bd229ead + Riegel K),
`RE15_BUILD_DIR=<wt>/re15_port/build bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`; ctest aus Git-Bash mit
`/c/msys64/mingw64/bin` vorn im PATH, **-j 3** (andere Agenten bauten parallel). Kein Test rot, keiner musste einzeln wiederholt werden.

| Testgruppe | Ergebnis | Dauer | Bemerkung (Last / einzeln) |
|---|---|---|---|
| Alle Nicht-Integrationstests ohne die 7 df-haengenden Skripttests (`-E "^integration_\|releaseskript_(trocken\|ablauf\|fehler_bau\|fehler_umfeld\|cmd)$\|titelversion_ablauf$\|android_bash_mutanten$"`, -j 3, --timeout 900) | **1066/1066 gruen** | 448,8 s (17:03:34..17:11:03) | unter Last (Parallelbau anderer Agenten); nichts rot, nichts wiederholt |
| davon PF-beruehrt (unit): `unit_r41_nachtrag30` (G, Q, S, neu K), `unit_r41_ventilkette`, `unit_r41_keyitems_*` (8), `unit_r37_codepanel_*` (4), `unit_r26_panel_11f0`, `unit_r27_panel_schalterwerte`, `unit_r39_toene_schrank`, Karten-Riegel `unit_*karte*` / `unit_r36_kartenbits` / `_kartenitems` / `unit_r41_kleinkram_b_karte_pin` (26) | **44/44 gruen** | Summe 15,9 s (nachtrag30 3,31 s, ventilkette 2,34 s) | im Lauf oben enthalten |
| `integration_r41_nachtrag30_{panel,schrank,laden,wasser}_{leon,elza}`, `_ton`, `_laut` | **10/10 gruen** | Summe 1903,0 s (schrank_elza 361,4 s, schrank_leon 235,2 s, panel_elza 136,2 s, panel_leon 104,8 s) | Gruppe B, -j 3, ton/laut seriell (RUN_SERIAL) |
| `integration_r41_ventilkette{,_b,_alarm,_kette,_kette_elza,_weg,_weg_elza,_sperre}` (Text 40/41/43, Frage 40 auf zwei Seiten) | **8/8 gruen** | Summe 5944,5 s (weg_elza 1680,2 s, weg 1456,5 s) | Gruppe B |
| `integration_r41_keyitems_*` (Sicherung 2010, Karte, Aktion) | **13/13 gruen** | Summe 365,7 s | Gruppe B |
| `integration_r37_codepanel` | **1/1 gruen** | 96,8 s | Gruppe B |
| Raeume 2010/2011 und 2030/2031 im Lauf: `integration_r36_dachkiste`, `r37_fahrstuhl_n1`, `r37_itemband`, `r37_itemband_lift`, `r37_kiste`, `r38_spinnen_echtlauf`, `r41_kleinkram_a_gift`, `r41_kleinkram_a_lautstaerke`, `r41_masken36_stage2`, `r41_spinnenanim` | **10/10 gruen** | Summe 5735,8 s (dachkiste 1502,5 s, itemband_lift 1398,6 s) | Gruppe B |
| **Gruppe B gesamt** (42 Integrationstests, -j 3, --timeout 3000) | **42/42 gruen** | 9086,2 s (17:11:23..19:42:49) | unter Last; nichts rot, nichts einzeln wiederholt |
| nicht gelaufen: `unit_r38_releaseskript_{trocken,ablauf,fehler_bau,fehler_umfeld,cmd}`, `unit_r39_titelversion_ablauf`, `unit_r35_android_bash_mutanten` | — | — | Umgebung: df.exe haengt maschinenweit (wie G5-Abnahmen 3/4); release/ in diesem Zweig unberuehrt |

**Riegel der PF-Aenderung** (Ausgaben dieses Laufs):

| Riegel | Test | Schranke (Quelle) | Wert |
|---|---|---|---|
| Panel-Tinte = Spielschrift | `unit_r41_nachtrag30` G | 42 Glyphen, alle Tintentexel gleich, **genau 1334** (neu, Abnahme 2 Par. 3a); versetzt < Haelfte | `42 Glyphen, 1334 von 1334 ..., um 1 Bildpunkt versetzt 450 von 1334` |
| dto. im Fenster-Framedump | `integration_r41_nachtrag30_panel_{leon,elza}` | F780/F900 >= 1000 Texel, gleich = gesamt, versetzt < Haelfte | Leon und Elza F780/F900: je 1334 von 1334, versetzt 450 |
| Drehscheiben-Stempel, Zeiger-Zwischentoene | `unit_r41_nachtrag30` S | **genau 288** Stempel-Texel (neu, Abnahme 2 Par. 3c), sichtbare alle gleich, (2,2)-versetzt < Viertel; Zeiger in jedem Bild der Fahrt A 100 -> 50 -> 80 **0 Zwischentoene**, >= 12 Zeiger-Texel (Abnahme 2 Par. 3c: max 0, vor NB1 bis 51) | `288 Stempel-Texel, 279 von 279 gleich, um (2,2) versetzt 0 von 279; Zeiger 55 Bilder (A 80), Zwischentoene max 0, Zeiger-Texel min 32` |
| Kasten-Scherung im Framedump (Cut 3) gegen ROOM2060 Cut 6 | `integration_r41_nachtrag30_schrank_{leon,elza}` Lauf pf | NCC >= 600, sx/sy 940..1060, Gegenprobe Massstab <= 880, \|W\|, \|S\| <= 100 (1 Grad), Gegenprobe 5 Grad -> 4..6 Grad | Leon = Elza: innen NCC 753, V 1000, W 25, S -50, Gegenprobe -475; Fluegel NCC 883, V 1018, W -25, S -50, Gegenprobe -550 (Hundertstel Grad) |
| **neu K**: Kasten-Scherung der Texturen ohne Fenster | `unit_r41_nachtrag30` K | Textur k 5 / k 7 aus `gen/schrank2010_r41.inc` gegen BG06 ueber ROOM2060 Cut 6 @0x120 (H 208), Weltflaechen aus dem inc-Kopf; \|W\|, \|S\| <= 0,5 Grad (Abnahme 2 Par. 2c: nach NB1 hoechstens 0,30, vor NB1 4,00 / 6,50), NCC >= 0,9 (Par. 2c: 0,950 / 0,985), Gegenprobe 5 Grad -> 5 +- 0,5 | Rueckwand W 0,00 S 0,00 NCC 0,955, Gegenprobe -5,00; Fluegel W 0,00 S 0,00 NCC 0,965, Gegenprobe -5,00 |

Gegenprobe des neuen Riegels K (Werkzeug `test_r41_nachtrag30 kastentex <innen\|tuer> <tim>` auf den Texturen frueherer Staende, aus der inc
herausgeschnitten): 422048234 (vor NB1) innen W -2,75 S -1,25 NCC 0,511, Fluegel W -8,00 (Suchrand) S -6,00 NCC 0,303; 789f6e681 (vor PF) innen
W -3,50 S -1,00 NCC 0,376, Fluegel W -8,00 S -3,75 NCC 0,247 -> beide Staende rot (Scherung und NCC). Grenze: K prueft die Textur gegen die Kamera,
nicht die Abbildung auf die Port-Geometrie im Fenster; die deckt der Fenster-Riegel `_schrank_*` (Framedump, RE15_SOFTWARE_RENDER) ab.

Commits: `4dae40c04 test(r41-panelfix): Riegel K - Scherung der Kasten-Texturen 2010 gegen ROOM2060 Cut 6 ohne Fenster` (dazu G auf genau 1334,
S auf genau 288 geschaerft).

## 6. Hinweise (keine Maengel)

* **H1 — Grund der Schriftzellen B 50, C 30, 80 nicht bitgleich.** Das Dossier sagt "Schrift unveraendert (Glyphen, CLUTs und Lagen gleich)". Das stimmt fuer
  Glyphen, CLUTs, Lagen und alle 1334 Tintentexel. 73 Grundtexel in den Zellen von B 50 (27), C 30 (35) und der roten 80 (11) weichen um bis zu 16/255 ab:
  Instrument und Scheiben laufen nicht mehr durch die JPEG-Kette des Generators, darum aendern sich die Nachbarbloecke mit. Kontrast Tinte/Grund unveraendert
  (Abweichung <= 0,3 Stufen). Keine Verschlechterung der Lesbarkeit messbar.
* **H2 — Aussehen der Drehscheiben geaendert (PORT-WAHL der NB1).** Flach mit hartem dunklem Rand, Teilstriche als ganze Texel, Querbalken 1:2 (26,6 Grad)
  statt rund 30 Grad (`panel_scheiben_vor_nach_zoom5.png`). Das ist die angekuendigte Aenderung; ob sie dem Nutzer gefaellt, kann nur er beurteilen.
* **H3 — Fluegel-Seitenverhaeltnis haengt am Fluchtpunkt.** Mit dem gemessenen Fluchtpunkt 130..132 ist V 0,97..1,00 (Autor: 1,014 bei 130,8). Die obere
  Kante des weissen Schilds zeigt auf 139,7..141 (dort verdeckt); mit diesem Wert waere V 0,905. Drei saubere Kanten stuetzen 131.
* **H4 — Hauptbaum neu bauen.** Der Nutzer spielt aus `C:\workspace\git\reAi_v2\re15_port\build`; ohne Neubau nach dem Merge sieht er weder PF noch NB1.

## Belege

`PF_belege/abnahme2/`: `kasten_cut3_vor_nach.png`, `kasten_entzerrt_vor_nach_2060.png`, `panel_vor_nach_leon_elza.png`, `panel_schrift_vor_nach_zoom6.png`,
`panel_scheiben_vor_nach_zoom5.png`, `zeiger_fahrt_vor_nach.png`, `frage40_vor_nach.png`; `werkzeuge/` (Treiber, Plaene, Tastenskript, Messskripte,
Messwerte `p_*_mass.json`, `mass_affin_131.3.json`). Alle Fensterbilder: gdigrab per hwnd der eigenen PID, beschleunigter Renderer, 960x720.
Videos und Einzelbilder nach dem Auswerten geloescht.
