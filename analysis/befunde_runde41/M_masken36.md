# Spur M masken36 — Runde 41 (2026-10-24)

Auftrag (AUFTRAG.md, Wortlaut massgeblich):
1. "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher aufweisen."
   -> "danach kannst du deine Masken schliessen in der naechsten Version."
2. "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!" (53 Masken in master
   5924939d) -> Sichtpruefung im Spiel; Orchestrator-Hinweis 2026-10-24 03:21: ROOM2020 Cut 3 Reling ueber Leons
   Beinen an der Kurbel (Pos_set @0x0A8C (3650,-1800,-2925)), dieselbe Pruefung fuer alle 53 Masken.

Baum: .claude/worktrees/r41_masken36 (Zweig r41/masken36, Basis b28c7236 = master-HEAD 4c667c97 + AUFTRAG-Hinweise).

## Stand
- [x] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE R40, P_masken36 R37, Memory Masken)
- [x] Punkt 1: Messung vorher (1.1: Loecher der Uebersicht = Entpacker-Fehler; im Spiel Treppenkanten), Ansatz (1.2),
      Umsetzung (1.3: Saum-Rechtecke + eigener Atlas, Ausschluss R37), Messung nachher (1.4), verworfener Weg (1.5)
- [x] Punkt 2: Sichtpruefung 53 Masken (2.2), ROOM2020 Cut 3 = richtig (2.1), Fehler ROOM20A0 Cut 12 behoben (2.3)
- [x] Tests (probes/r41_masken36.cmake), gezielte Laeufe 41/41
- [x] Nachbesserung 1 (Abnahme 0) und Nachbesserung 2 (Abnahme 1): Abschnitte unten; NB2 = 3D-Urteil fuer Rechtecke mit
      Zellen-Modell + Nahfeld-Sperre + Zuordnung von Hand (164 Rechtecke wieder drin, 956 982 Punkte), Restliste fuer den Nutzer
- [x] Nachbesserung 3 (Abnahme 2): Trennwand 30A0 ganz zu (Zelle 6 + Teilrechtecke je Koerper), Mass/Riegel "zu fern" (1 360 606 ->
      40 276, Stage 3: 0), OFFEN 9 gegangen, 7 Restliste-Gegenstaende gezeichnet (983 624 Punkte), Verdachtsliste gesichtet
- [x] Nachbesserung 4 (Abnahme 3): Ost-Stirn 30A0 mit Messung widerlegt (Kuenstler-Ausschnitt ganz gezeichnet), Saaten-Fehler Tuer-Band
      (Gleisbett 6000 weg), 437 nie platzierte Kuenstler-Komponenten platziert (Gelaender u.a., 983 624 -> 1 159 393 Punkte), Wandecke
      6000 C5 geteilt, Laeufer mit Laufpruefung

## Punkt 1 — STAGE3-6-Masken "ueberall Loecher"

### 1.1 Messung vorher (2026-10-24 08:10-08:40)

**Befund A — die Loecher der Uebersichtsbilder sind ein Fehler des Werkzeugs, nicht der Masken.**
Die Uebersichtsbilder `pri/STAGE3-6/<raum>/<cc>_port.png` (a8be31a7, 2026-10-23 12:59; danach die Nutzer-Aussage
"ueberall Loecher") und die Messung der UEBERGABE R40 ("17,3 % der Rechteck-Texel = Index 0, 3050 C1: 51 %") lasen den
Vordergrund-Atlas ueber `re15_port/tools/maske/original.py atlas()` -> `sld_decompress()`. Dessen langer Rueckverweis
(3-Byte-Form) war falsch:
```
s = raw - 0x1000 if raw & 0x800 else raw        # schon negativ (12-Bit-Vorzeichen)
off = ((s << 2) | (b3 >> 6))                     # Python: korrekt negativ, z.B. -8192
off = off - 0x10000 if off & 0x8000 else off     # bit 15 einer NEGATIVEN Zahl ist gesetzt -> -73728
```
Das Original FUN_800c47e8 (BIN/DEBUG.BIN +0x47E8, Bytes aus der ausgelieferten Datei gelesen):
`+0x4890 80 18 0a 00 sll v1,t2,2` / `+0x4894 25 10 43 00 or v0,v0,v1` / `+0x4898 00 14 02 00 sll v0,v0,16` /
`+0x489C 2a 12 03 08 j 0x800c48a8` / `+0x48A0 03 54 02 00 sra t2,v0,16` = signext16 der UNTEREN 16 Bit (so auch
engine/src/sld_common.c re15_sld_decode, das das Spiel benutzt). Jeder lange Rueckverweis las 65536 Byte zu weit
vorn (negativer Python-Index = Pufferende = Nullen -> Palettenindex 0 = "durchsichtig").
Gemessen (Skript Scratchpad `m41/py/a3_dekoder.py`, `a7_dekoder_alle.py`): Python-Entpacker gegen eine exakte
Nachbildung von re15_sld_decode, ALLE Atlas-Cuts aller Stages: **vorher 0 von 359 gleich** (Stage 1: 0/122, 2: 0/15,
3: 0/77, 4: 0/33, 5: 0/86, 6: 0/26; 39 Cuts brachen sogar mit "Entpacken fehlgeschlagen" ab, z.B. ROOM3040 Cut 0 lieferte
ein Bild der Breite 0); **nachher 359 von 359 bytegleich** (Index UND CLUT). Bekannt war die Abweichung schon in
`tools/r34n_g/schrift1150_masken.py` ("NICHT tools/maske/original.atlas() benutzen ... weicht fuer ROOM1150 Cut 2"),
behoben war sie nie. Fix: `original.py` kuerzt auf 16 Bit vor dem Vorzeichen (Kommentar mit den Adressen).

**Befund B — im Spiel haben die Rechtecke keine Index-0-Loecher.** Mit dem Engine-Dekoder (`a4_loecher.py`, 108
Leon-Sektionen der 15 Stage-3..6-Raeume, Elza-Dateien bytegleich): **Index 0 in den Atlasflaechen aller 3216 Rechtecke
= 0,00 %** (0 von 1 125 178 Texeln). Der Generator gen_masken.py (b8db7b2b) legt nur VOLLE Rechtecke in die gewachsene
Region (`rects_from_mask` = groesstes Vollrechteck), deshalb ist jedes Rechteck vollstaendig deckend. Blit-Pruefung mit
dem Engine-Dekoder: 1 125 178 gemalte Punkte, Abweichung zum Hintergrund im Mittel 12,70 / 765 (wie R37 §2.1: 12,63).
An der exe (Szene h3050c2 aus R37, ROOM3050 Cut 2 (-22000,-20768), Leon hinter der Kiste vorne links, Framedump
RE15_WINDOW_SCALE=1 Bild 120, mit/ohne RE15_NO_PRI): Leon vollstaendig verdeckt, kein Loch sichtbar (1379 Punkte mit
Maskenwirkung).

**Was im Spiel wirklich fehlt (die echten "Loecher"), gemessen ueber alle 108 Sektionen (`a6_luecken.py`):**
| Klasse | Bildpunkte |
|---|---|
| gezeichnet (nach dem Ausschluss der Spur P R37) | 727 561 |
| nur in AUSGESCHLOSSENEN Rechtecken (masken36_r37.c, Figur VOR dem Objekt wuerde uebermalt) | 396 737 |
| Kuenstler-Atlas einer platzierten Komponente, von KEINEM Rechteck gedeckt (Treppenkanten, Saeume, Kleinkram < 16 px) | 156 376 |
| eingeschlossene Luecken zwischen gezeichneten Rechtecken (binary_fill_holes) | 4 584 (130 Komponenten) |
| Atlas-Komponenten ohne bekannte Lage (kein Rechteck beruehrt sie, keine NCC-Verschiebung) | 452 144 Texel (927 Komponenten) |
Das Spiel zeichnet also 57 % der platzierten Kuenstler-Silhouetten (727 561 von 1 280 674); die Objektkanten sind
Treppen aus Vollrechtecken, und wo ein Teil der Rechtecke eines Objekts ausgeschlossen ist, bleibt ein loechriges Objekt.

### 1.2 Ansatz und Belege (2026-10-24 08:40-10:30)

Die Loecher, die im Spiel WIRKLICH zu sehen sind, sind (a) die Treppenkanten/Saeume der Generator-Masken und (b) die
ausgeschlossenen Rechtecke, die Objekte in Splittern stehen lassen. Beides ist aus den vorhandenen Daten schliessbar:

**(a) Saum = verifizierte Generator-Region ohne Rechteck.** Der Generator (analysis/esp_masken_2026-09-03/gen_masken.py
build_cut) findet je Verschiebung eine Region im Kuenstler-Atlas (gen_region.find_blocks block 16 / stride 4 / NCC >= 0.97
als Saat, region_grow tol 0.55 entlang der Silhouette; gemessene Praezision 99,39 % gegen die Kuenstler-Masken, Kopf
gen_masken.py) und ueberdeckt sie mit den groessten VOLLEN Rechtecken (`rects_from_mask`, "Kleinkram < 16 lohnt kein
eigenes Rechteck", Rechtecke halb ausserhalb des Bildes verworfen). Der Rest der Region wurde nie gezeichnet.
Nachbau (Scratchpad `m41/py/b2_grown.py`, `b3_tiefe.py`; im Repo `re15_port/tools/maske/schliessen_r41.py`):
* Region mit dem Generator-Code selbst (gen_region unveraendert), Atlas = Engine-Entpacker, Hintergrund = probe_bg_dump:
  **3239 von 3239 vorhandenen Rechtecken liegen vollstaendig in der Region ihrer Verschiebung, keine Verschiebung fehlt.**
* Tiefenregel des Generators (depth_map_for_cut: je Bildspalte Bodenkontakt der Komponente auf y = 0, DEPTH_SAFETY 0.90,
  Rechteck = int(Median der Fenstertiefen)) mit geom.build_view/vz_at_floor: **3239 von 3239 Rechtecktiefen bitgleich**.
* Saum = Regionspunkte unter keinem vorhandenen Rechteck: **107 118 Bildpunkte** in 108 Sektionen (Region 1 233 573
  Atlaspunkte gegen 1 125 178 Rechteckflaeche). Klassifikation Loch/Zwischenraum: ein Punkt wird nur geschlossen, wenn er
  zur verifizierten Region gehoert (Kuenstler-Atlas Index != 0 UND passt in Farbe zur Verschiebung, zusammenhaengend mit
  einem NCC-belegten Block); alles andere bleibt offen - Gelaender-/Gitterluecken haben im Kuenstler-Atlas Index 0.
  Von den 4 584 eingeschlossenen Lochpunkten der heutigen Deckung liegen 3 285 im Saum (werden geschlossen), 1 299
  ausserhalb der Region (bleiben offen: echte Zwischenraeume oder nicht belegt).
* Warum nicht die ganze Kuenstler-Komponente: eine Atlas-Komponente traegt oft mehrere Verschiebungen (20 Komponenten,
  z.B. ROOM6010 Cut 12 Komponente 13: drei Verschiebungen, Farbabweichung 58/125/128 je Verschiebung) - nur die
  verifizierte Region ist sicher richtig platziert (Saum-Abweichung zum Hintergrund: Median 14, 90 % <= 33, gegen
  gedeckte Punkte Median 11, 90 % <= 24; `a8_abweichung.py`).

**(b) Ausschluss der Spur P (R37) gilt weiter - auch fuer die Saum-Rechtecke.** Ein Saum-Rechteck, das an einem
erreichbaren Stehplatz einen Figurpunkt ohne solide Zelle davor verdecken wuerde, faellt ganz weg (masken36_r37.c,
Tabelle neu erzeugt mit `test_r37_masken36 erzeuge`). Die Schranke fuer nicht pixelgenaue Figurraster (R37 NB2 M2: Kasten
breiter als das Bild / Spalte halb hinter der Kamera) ist beim Iterieren jetzt AUCH Ausschluss-Grund (vorher nur Pruefung -
die neuen Saum-Rechtecke liegen teils dicht vor der Kamera: in Runde 0 7544 / 7958 Faelle "Maske koennte verdecken"), und
der Riegel zaehlt sie am Endzustand mit Tabelle: 0 / 0. Ergebnis: in allen 216 Sektionen sind die ALTEN Rechtecke genau so
ausgeschlossen wie in der R37-Tabelle (bitgleich verglichen), von den 8600 Saum-Rechtecken (Leon + Elza) bleiben 1662 aus.
Das Schliessen ist damit rein additiv: nichts, was vorher gezeichnet wurde, aendert Lage, Tiefe oder Sichtbarkeit.
Ein zweiter Weg (Tiefe aus der Erreichbarkeit statt Ausschluss) wurde gebaut, gemessen und VERWORFEN, s. 1.5.

### 1.3 Umsetzung

| Datei | Inhalt |
|---|---|
| `re15_port/tools/maske/schliessen_r41.py` (neu) | baut je Raumpaar xxx0/xxx1 MSK + 216 `_PRIcc.TIM`. Probe 1: Rechteck in der Region; Probe 2: Deckung == alte Rechtecke + Region bitgenau; Probe 3: alter Teil jeder Sektion bytegleich (Gruppen + Rechtecke, nur Kopfzahlen und Anhang neu). Saum-Rechtecke: Grundstuecke = 8-zusammenhaengende Saumpunkte gleicher Spaltentiefe, gierig zusammengelegt (gleiche Verschiebung, Tiefenspanne <= Stufe, kleinster Flaechenzuwachs) bis 105 Rechtecke je Cut (RE15_PRI_MAX_MASKS_PER_CUT, re15_pri.h) erreicht sind; feinste Stufe aus (0, 1, 2, 3, 4, 6, 8, 12, 16) wie bau_p2.bau_cut. Ergebnis: 87 von 108 Sektionen Stufe 0 (jede Spaltentiefe genau), groesste Abweichung 12 Buckets (1 Sektion). Atlas: alte Rechtecke an ihrer BSS-Stelle (Texel = Hintergrund an der Zielstelle), Saum-Rechtecke an der ersten freien Stelle, je eine Gruppe (Anker = Ziel - Quelle), deckend nur an ihren Saumpunkten; 255 Farben Median-Schnitt, CLUT wie atlas.tim_aus (0,480, STP fuer Schwarz). Deterministisch (zweiter Lauf bitgleich). |
| `re15_port/shared_assets/PSX/MASKS/ROOM3040..ROOM6031.MSK` + `ROOMxxxx_PRIcc.TIM` | 30 MSK (bytegleiche Paare Leon/Elza wie bisher), 216 TIM neu. 4300 Saum-Rechtecke je Variante (15 078 Rechtecke Leon + Elza). |
| `re15_port/engine/src/gen/masken36_ausschluss_r37.inc` (neu erzeugt) | 148 Zeilen (R37: 126); alte Rechtecke bitgleich ausgeschlossen, dazu 1662 Saum-Rechtecke |
| `re15_port/tests/unit/test_r37_masken36.c` | Generator-Sektion = Stage 3..6 (`generator_sektion`/`atlas_generator`, vorher "ohne eigenen Atlas"); Schranke NB2 M2 als Ausschluss-Grund beim Iterieren, Riegel-Zaehlung am Endzustand; Zensus: Stages 3..6 eigener Atlas (vorher BSS), Trennschaerfe-Gegenprobe nur fuer Freistellungen (der alte Teil der Generator-Sektionen passt durch beide Atlanten, Probe 3); Werkzeug `fehl` auch fuer Freistellungen (Punkt 2) |
| `re15_port/tests/integration/test_r37_masken36_verdeckung.cmake` | die Zeile "Port-Masken ohne eigenen Atlas -> BSS-Atlas" darf nicht mehr kommen (vorher Pflicht) |
| `re15_port/tests/unit/test_r41_masken36.c`, `probes/r41_masken36.cmake` (neu) | Riegel `unit_r41_masken36_loecher` (1.4), `_kurbel` (2.1), `integration_r41_masken36_stage2` (2.2) |
| `re15_port/tools/maske/original.py` | Entpacker-Fix (1.1) |
| `re15_port/tools/maske/bilder_r41.py` (neu) | Vorher/Nachher-Bilder je Cut + berichtigte Uebersichtsbilder |

Kein Engine-/Laufzeit-Code geaendert: der Lader nimmt fuer R15M-Sektionen MIT eigenem Atlas den eigenen (re15_pri_atlas_quelle,
bg_pc.c); die Zeile "Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)" erscheint fuer die Stages
3..6 nicht mehr.

### 1.4 Messung nachher (Endstand 6871f2a5)

**Statisch, alle 108 Sektionen (Elza bytegleich), `re15_port/tools/maske/bilder_r41.py` -> `M_bilder/ROOMxxx0_Ccc.png`
(je Cut: Hintergrund | VORHER | NACHHER; magenta = schon vorher, gruen = neu geschlossen, rot = weg, cyan = eingeschlossene
Luecke, die offen bleibt) und `M_bilder/zahlen.txt`:** gezeichnet vorher 727 561 Bildpunkte -> nachher 780 626 (+53 065
Saumpunkte), weg 0. Beispiel ROOM3050 C1: die Treppenkante der Konsole ist zu (+837). Die cyan-Flaechen sind echte
Zwischenraeume oder Stuecke ausgeschlossener Rechtecke (z.B. ROOM6010 C12: der Raum unter der Treppe).
Berichtigte Uebersicht im Format von `pri/STAGE3-6/<raum>/<cc>_port.png`: `M_bilder/uebersicht/STAGEn/<raum>/<cc>_port.png`
(107 Bilder, Leon-Ordner; die Elza-Ordner xxx1 sind bytegleich). pri/ ist laut VERTRAG §1.3 tabu - die alten Bilder dort
sind mit dem kaputten Entpacker gebaut (1.1) und sollten ersetzt werden (Orchestrator/Nutzer, s. "Fuer den Nutzer").

**Offene Kerben (Riegel `unit_r41_masken36_loecher`, ueber die Engine-Leser, Leon + Elza):** Kuenstler-Atlas deckend +
farbgleich direkt an der Kante eines alten Rechtecks, nicht gezeichnet: VORHER 61 672 (alte Rechtecke, BSS-Atlas, gleicher
Ausschluss) -> NACHHER 15 498 (25,1 %). Der Rest sind Saum-Rechtecke, die der Ausschluss nimmt. Blit nachher: max. Mittel
je Sektion 10,66 / 765 (eigener Atlas = Hintergrund an der Zielstelle).

**An der exe (Framedump Bild 120, RE15_WINDOW_SCALE=1, Leon per RE15_DEBUG_JUMP + RE15_PLAYER_POS, RE15_FORCE_CUT):**
* R37-Szenen (Riegel integration_r37_masken36_stage3..6, nb2): s. Tests.
* Sweep ueber ALLE 108 Leon-Cuts (Scratchpad `m41/py/d1_sweep.py`, `d2_vorher.py`, `d3_final.py`; Stellen aus
  `test_r37_masken36 suche 60`; A = mit Masken, B = RE15_NO_PRI; Figurpunkte = |B - Hintergrund| > 24 im Figurfenster -
  der figurlose Spielrahmen ist bitgleich zum probe_bg_dump-Bild, gemessen 0 Abweichung). VORHER = eigene exe des
  Basisstands b28c7236 (git archive, Scratchpad `m41/vorher`, gleiche Szenen):

  | | Szenen | VORHER | NACHHER |
  |---|---|---|---|
  | DAVOR, Punkte uebermalt | 53 | 2 Szenen: v4080_07 9, v5100_07 25 | 3 Szenen: v4040_11 1, v4080_07 11, v5100_07 27 |
  | HINTER, verdeckte Figurpunkte | 44 | 11 782 von 28 457 | 12 664 von 28 457 (+882) |

  DAVOR (berichtigt in Nachbesserung 1, Abnahme 0 Mangel 5d - die fruehere Aussage "keine neue Uebermalung" war so nicht
  belegt): v4080_07 (+2) und v4040_11 (+1) sind Kanten des 3D-Tuermodells im Tuerrahmen, keine Figur; v5100_07 25 -> 27 ist
  Leons Hand am unteren Bildrand. Mechanik-Messung NB1 (RE15_TRILOG, Leon-Dreiecke gegen die Masken-Schluessel): die Hand liegt
  unter dem ALTEN Generator-Rechteck 0 (Tiefe 64, x 0..43 y 154..240), auch die zwei Abnahme-Punkte (38,236)/(39,237) liegen
  NUR unter Rechteck 0 (kein Saum-Rechteck dort); Leon-Dreiecke z 3062..4615 gegen Schluessel 4164 -> 120 Leon-Rasterpunkte
  verdeckt, vor NB1 und nach NB1 gleich (neu 0). Die +2 im Bildvergleich |A-B| > 24 entstehen, weil Rechteck 0 seit Runde 41
  durch den eigenen Atlas (Texel = Hintergrund an der Zielstelle) statt den BSS-Atlas gezeichnet wird - andere Farbe, dieselbe
  Verdeckung. Die Verdeckung der Hand selbst ist ALTBESTAND (Generator 2026-09-03, Tiefe mit DEPTH_SAFETY 0.90 = 10 % naeher
  als der Bodenkontakt); ob die Hand dort vor oder hinter dem Objekt liegt, ist nicht gemessen (Rechteck 0 hat keine
  Zellen-Tiefe) - OFFEN 6, Bild `M_bilder/NB1_M5_ROOM5100_C07_hand.png`. HINTER: kein Szenenwert sinkt (h6000_04 128 -> 127 = Rauschen), groesste Gewinne h6000_07 626 -> 838,
  h3050_02 1116 -> 1237, h3040_02 1057 -> 1169, h3040_13 613 -> 687.

### 1.5 Verworfen: Tiefe aus der Erreichbarkeit statt Ausschluss (gebaut 2cfb1158, gemessen, zurueckgenommen 6871f2a5)

Idee: ein Rechteck mit Figurpunkten ohne Zelle davor nicht wegnehmen, sondern ihm depth = max bucket dieser Punkte geben
(verdeckt keinen von ihnen; Urteil depth < (1023*vz)>>16, @0x8002565c / @0x80066c70 / @0x80039650). 2966 Tabellenzeilen;
statisch +472 219 Bildpunkte, an der exe HINTER 11 782 -> 19 385. Aber das Zelle-davor-Urteil ist ein GRUNDRISS-Urteil: in
ROOM4080 Cut 2 (10475,-13710) blieb von Leon nur der Oberkoerper (Beine und Huefte mit der Wand uebermalt,
`M_bilder/P1_verworfen_erreichbarkeit_h4080_02.png`), in ROOM4040 Cut 5 (-16400,-7622) nur die rechte Haelfte
(`P1_verworfen_erreichbarkeit_h4040_05.png`) - beides
mit "Zelle davor" im Grundriss (die Kameras von 4040 C5 / 4080 C2 / 30A0 C0 stehen IN grossen schraegen Zellen der Gaenge).
Der Ausschluss nimmt solche Rechtecke (Hintergrund in der Generator-Region, R37 §3.2) ganz weg und bleibt damit die sichere
Regel; der Nutzer hat diese Fehlerart schon einmal gemeldet ("Leon verschwindet fast vollstaendig", 6c29d834). Die
Bilder `M_bilder/P1_verworfen_erreichbarkeit_*.png` (Nachbesserung 1: umbenannt, vorher P1_exe_*.png) zeigen diesen
verworfenen Zwischenstand (Spalte NACHHER), nicht den Endstand.

## Punkt 2 — die 53 Nutzer-Masken 11A0 + STAGE2 (Sichtpruefung im Spiel)

Bestand (master 5924939d / be59e679): 53 Sektionen in MASKS/ROOM11A0, 2010, 2020, 2030, 2040, 2050, 2080, 20A0 (.MSK +
_PRIcc.TIM + .PBM/.STAND), Zwillinge fuer Elza ueber gen/pri_masken_zwilling_r37.inc (11A1/2011/.../20A1). Der Bau
pruefte VOR/HINTER nur am Figurkasten 1500 x 900 mit der 95-%-Regel (abnahme.py: "VORverd" zaehlt nur Stellen, die zu
>= 95 % verdeckt sind) - teilweise Verdeckung VOR dem Objekt ("VORteil") war erlaubt.

### 2.1 ROOM2020 Cut 3 an der Kurbel (Orchestrator-Hinweis nach Abnahme E 0, Bild E41a0/zoom_legs.png)

**Messung:** exe, Leon an der Lage nach `Pos_set` @0x0A8C (3650,-1800,-2925) (Band 1), RE15_FORCE_CUT=3, Bild 120
(`M_bilder/P2_ROOM2020_C3_kurbel.png`: mit Masken | ohne | rot = von der Maske verdeckt): Log `[pri] cut=3
pri_offset=0x3FC masks=104 fg_atlas=1`; die Pfosten und der Handlauf der linken Reling (Freistellung 03_01) liegen ueber
Leons Beinen: 86 von 779 Figurpunkten im Figurfenster.

**Beleg (Geometrie, Riegel `unit_r41_masken36_kurbel` ueber die Engine-Leser):** Kamera Cut 3 = P(10456,-6184,956) ->
T(-15424,3868,-3056) (Kamerasatz ROOM2020.RDT), Kameraort aus der Q12-Matrix (10455,-6183,957). Die Grundriss-Strecke
Kamera -> Leon (3650,-2925) kreuzt die solide SCA-Zelle 11 = **x -400..13100, z -2550..-2050, Band 1, u0 0xFF** - das ist
die Suedreling zwischen dem Laufsteg (z -3850..-2550) und dem Becken (Zelle z -2050..1500). Projektion (geom.cut_view):
Leons Fuss (90.9,156.5) vz 8404, Relingfuss an der Kreuzung (93.3,174.0) vz 7304, Relingoberkante (y -2800) (89.8,148.8)
vz 6946 -> die Reling ist der Kamera NAEHER als Leon und liegt im Bild ueber seinen Beinen. **Leon steht an der Kurbel
HINTER der Reling** (die Kamera haengt ueber dem Becken, z 957, der Laufsteg mit der Kurbel liegt jenseits der Reling).
Dass das Original ihn obenauf zeichnet, liegt daran, dass das Original fuer Cut 3 GAR KEINE Masken hat
(ROOM2020.RDT Kamera-Eintrag Cut 3 -> +0x3FC = `FF FF FF FF` = NULL-Sektion, FUN_800392d4 @0x80039324/@0x80039338) - die
Freistellung 03_01 des Nutzers ist genau die fehlende Verdeckung. Der R40-Bau sah das ebenso: Schiene ROOM2020 C3
"VORn 0" (kein begehbarer Platz VOR der Reling), HINTn 1029.
**Ergebnis:** kein Fehler der Maske, keine Aenderung (Tiefe/Ebene stimmen: Band 1 = y -1800, einziges begehbares Band
von ROOM2020, geom.begehbare_baender). Nebenbefund im selben Bild: die rechte Reling-Freistellung (03_02) liegt im Bild auch
ueber dem Rand des dunklen 3D-Objekts in der Beckenmitte (obj 1, `Obj_model_set[1] pos=(-1452,-1784,-584)`) - Objekte
und Effekte prueft die Erreichbarkeits-Schiene nicht (nur Figuren), s. OFFEN.

### 2.2 Sichtpruefung aller 53 Masken an der exe (Leon, Elza-Stichproben)

**Stellen** (Scratchpad `m41/py/e1_stellen.py`): je Cut aus dem begehbaren Boden (Klemmpfad-Dump R40
`build/p2/dump_klemmpfad_r40.txt`, nur begehbare Baender: geom.begehbare_baender + Ebene der Freistellung aus auswahl.json)
die Stelle mit der groessten Ueberdeckung der Maskenflaeche VOR bzw. HINTER der Standlinie der Freistellung (.STAND,
Figurkasten 3000 x +-350). 44 VOR-Stellen (9 Cuts haben keine: 2020 C1/C2/C3, 2040 C1/C2/C5/C6/C13, 2080 C3), 53 HINTER.
**Laeufe** (`e2_sweep.py`): A mit Masken / B RE15_NO_PRI, Bild 120; **Leon allein** (`e4_c.py`): C = Leon ausserhalb des
Regionsvierecks des Cuts (Akteur-Cull FUN_8002c18c, wie R37), Leon-Punkte = |B - C| > 24 (Props bleiben in B und C gleich);
fuer 2010/2020 gibt es keine begehbare Stelle ausserhalb des Vierecks - dort zaehlt |B - Hintergrund|.
Bilder je Cut: `M_bilder/P2_ROOMxxxx_Ccc.png` (VOR/HINTER: mit Masken | ohne | rot = uebermalt), Zahlen `M_bilder/P2_zahlen.txt`.

**Ergebnis VOR (Leon darf nirgends uebermalt sein):** 42 von 44 Stellen mit sichtbarer Figur; was die Masken an ANDEREN
Objekten uebermalen, ist richtig (Spinnen hinter den Relingen in 2030/2050, die Kisten im Becken 2050 C12 hinter dem Pfeiler:
Kiste obj 2 (2052,0,-5552) vz 5315 = Bucket 82, Pfeilermaske Tiefe 42..50). Leon selbst:
| Stelle | Leon-Punkte uebermalt | Befund |
|---|---|---|
| ROOM20A0 C12 (14000,-18900) Band 4 | 30 von 698 | **Fehler**: hinteres Fussgelenk unter dem Bodenstreifen 12_02 -> 2.3 |
| ROOM2030 C15 (4400,3850) Band 3 | <= 8 | Handlauf an der Schulterkante, Leon steht 1 Bucket vor der Reling (Maskentiefe 132..137, Leon 135); Bucket-Rundung |
| ROOM2030 C8 (2200,2650) Band 3 | 6 von 11567 | kein Leon-Punkt: Maskentiefe dort 97, Leon Bucket 19 - kann ihn nicht verdecken; die Punkte sind eine Spinne, die sich in C bewegt hat |
| 2030 C7, 2050 C1/C2/C3 | 12 / 599 / 21 / 116 | Spinnen hinter der Reling (in C an anderer Stelle), nicht Leon (Bilder P2_ROOM2030_C07, P2_ROOM2050_C01..03) |
**HINTER:** Leon 11 552 von 73 067 Punkten verdeckt (46 Stellen mit C); niedrige Werte, wo die Stelle nach dem Stand-Modell
"hinter" liegt, Leon aber auf einem anderen Band steht (z.B. 2050 C9) - kein Fehlerbild.
**Kurbel 2020/2021 C3:** s. 2.1 (Leon 86/779, Elza 124/648 - Beine hinter der Reling, geometrisch richtig).
**Statische Gegenprobe** (Werkzeug `test_r37_masken36 fehl`, jetzt auch fuer Freistellungen): das Grundriss-Modell der R37
meldet in den Mehrebenen-Raeumen (2030/2040/2050/2080) viele Punkte "ohne Zelle davor" - es kennt nur Zellen desselben
Bands, eine Reling auf Band 1 vor einer Figur auf Band 0 zaehlt nicht; als Urteil fuer die Freistellungen ungeeignet
(Beispiel 2080 C3 (-9900,-9100) Band 8: 1814/1817 "frei", an der exe steht Leon sichtbar hinter dem gruenen Rohr, Bild
P2_ROOM2080_C03). Massgeblich ist die exe.
**Probe Bau:** alle 8 Raeume mit `raum.py --p2 --stufe 16 --statistik max --dump build/p2/dump_klemmpfad_r40.txt` (Werte
aus 5924939d) in den Scratchpad gebaut: MSK/TIM/PBM/STAND bitgleich zu MASKS - der Bestand ist reproduzierbar.

### 2.3 Fehler ROOM20A0 Cut 12: Bodenstreifen 12_02 (behoben)

**Messung vorher:** Leon an der Wand (14000,-7200,-18900): 30 von 698 Leon-Punkten uebermalt (hinteres Fussgelenk);
AUF dem Streifen (14000,-7200,-19300): 50 von 678 (Bein und Schatten weg) - exe des Basisstands b28c7236,
`M_bilder/P2_ROOM20A0_C12_bodenstreifen.png` (VORHER | NACHHER | ohne Masken | rot). Der R40-Bau meldete denselben Ort als
"HINTER-Rest" (4,14000,-19300) 0,53 / (4,13600,-19700) 0,92 - als Figur HINTER der Kante, obwohl dort Boden ist.
**Beleg (Nachbesserung 1 berichtigt, Abnahme 0 Mangel 2):** 12_02 ist der Streifen BODEN vor der Schwelle unter dem
ROLLADEN - nicht vor einer "Ziegelwand" (so stand es hier bis Nachbesserung 1; falsch). Lage fein gemessen (94,126) Fehler 1,65
(zweitbeste 2,24 - die grobe R40-Messung hatte "237 gleich gute Lagen"); die Projektion der Vorderkante der Zelle 32
x 8000..14700 z -21000..-20500 (Band 4) auf y -7200 verlaeuft an der OBERKANTE des Streifens (Bild
`M_bilder/P2_ROOM20A0_C12_wandkante.png`, kontrastverstaerkt `M_bilder/P2_ROOM20A0_C12_streifen_kontrast.png`: Bodenfliesen
unter dem Streifen). Was hinter der Kante liegt, steht in Runde 38 (`analysis/befunde_runde38/W_rolltor20a0.md` §3/§6, dort
gemessen, hier nicht wiederholt): Zelle 32 ist "die Wand unter dem Rolladen"; der Rolladen ist das 3D-Objekt **obj 1**
(Obj_model_set @0x01932, Typ 0, Lage (7942,-7200,-20510), in der Welt x 7942..14422 z -20510..-20420) und faehrt per Schalter
auf y -10800 hoch; dann setzt Sca_id_set @0x01E5E-6E Zelle 32 auf u0 = 0, und hinter der Schwelle liegt ein SCHACHT (die
Kiste faellt dort auf Band 2, y -3600, und weiter auf Band 1, y -1800). Das Hintergrundbild zeigt ueber dem Streifen die
gruen gekachelte Oeffnung, die der Rolladen (3D-Modell) verdeckt. Die Profil-Tiefe des Baus (119..151) setzte den
Bodenkontakt an die UNTERKANTE des Streifens, ~1400 vor der Schwelle (Bildpunkt (110,148) -> Boden (13814,-19080)): wer auf
dem Streifen stand, verlor die Fuesse. Die Abnahme rechnete nach, dass 12_02 mit der Tiefe der Schwellen-Zelle 32 baubar
waere, ohne eine Figur auf dem Streifen zu uebermalen (Leon auf dem Streifen vz 9198..9895, Schwelle vz 9633..11527), und dann
Inhalte des Schachts hinter der Kante verdecken wuerde. **Nutzer-Entscheid (AUFTRAG.md Nachtrag 11, 2026-10-24 11:4x):
"Weglassen ist ok"** - 12_02 bleibt aus dem Bau, keine weitere Aenderung.
**Umsetzung:** `analysis/esp_masken_2026-09-03/auswahl.json` ROOM20A0 Cut 12: 12_02 unter `"_r41_nicht_gebaut"` (mit
Begruendung; die PNG unter pri/ bleibt unveraendert), 12_01 (Mauerecke, Tiefe aus der Zelle x14700..15800 z-21000..-20000)
bleibt; `raum.py ROOM20A0 12 --p2 --stufe 16 --statistik max` -> MASKS/ROOM20A0.MSK + _PRI12.TIM/.PBM/.STAND (88 statt 101
Rechtecke; Bau-Schiene VOR 354 verdeckt 0, teil 0). ROOM20A1 nutzt die Masken von 20A0 (Zwilling).
**Messung nachher:** (14000,-18900) 0 von 698, (14000,-19300) 0 von 678; Riegel integration_r41_masken36_stage2 mit Leon
und Elza (20A1) 0.
~~Frage an den Nutzer: was sollte 12_02 verdecken?~~ - beantwortet (Nachtrag 11): "Weglassen ist ok".

## Nachbesserung 1 (2026-10-24, nach Abnahme 0 = analysis/befunde_runde41/M_abnahme_0.md)

Grundlage: Abnahme 0 (Urteil "nicht bestanden", Maengel 1-5), Nutzer-Nachtrag 11 (AUFTRAG.md: 12_02 bleibt weg,
pri-Uebersichtsbilder ersetzt bis auf 10). Keine Messung der Abnahme wird wiederholt; ihre Zahlen werden zitiert.

### Stand Nachbesserung 1
Fortsetzung 2026-10-24 ~14:1x (Vorgaenger vom Wochenlimit ~13:5x beendet; Commits 5d10ccc0, f00fbf3e, a24e2389, 50c72950).
- [x] Mangel 5 (a)-(c) Doku: original.py-Kommentar, Kopf schliessen_r41.py, P1_exe_*.png umbenannt (a24e2389)
- [x] Mangel 5 (d) Dossier 1.4 DAVOR-Aussage berichtigt (5100 C7 Hand unter ALTEM Rechteck 0; 4040 C11/4080 C7 Tuermodell)
- [x] Mangel 2 Begruendung 12_02 richtiggestellt (a24e2389, 2.3 + auswahl.json "r41"), Nutzer-Entscheid "Weglassen ist ok"
- [x] Mangel 4 VOR-Stellen 2030 C7 / 2040 C7: es gibt keine (3D-Sehstrahl, 767 / 2378 Plaetze, 0 VOR); dabei Handlauf-Tiefe
      2030/2040 C7 gefunden und behoben (Leon HINTER dem Gelaender jetzt verdeckt)
- [x] Mangel 3 ROOM2030/2031 C15: widerlegt (Spinne hinter Leon), Riegel-Szenen mit weggestellter Spinne H == 0
- [x] Mangel 1 Zellen-Tiefe Schritt 1 (236 Rechtecke, exe 39 DAVOR / 10 HINTER) + Schritt 2 (131 Rechtecke, Erben wie raum.py
      "zelle"), Restliste mit Bildern `M_bilder/NB1_rest/`
- [x] Auflage Nachtrag 11: 10 Uebersichtsbilder (durchsichtig, Leon + Elza), alle Uebersichtsbilder/Boegen neu erzeugt
- [x] exe-Pruefung Schritt 2 (49 DAVOR neu 0, HINTER +), gezielte Tests (28 + 2 + 5 + 41 + 1, alle gruen), Abschluss-Commit

### NB1 Mangel 1 - ausgeschlossene Objektteile mit der Tiefe ihrer Kollisionszelle

**Ursache (Abnahme 0, Dossier OFFEN 1):** der Ausschluss der Spur P (R37) nimmt Generator-Rechtecke (alt und Saum) GANZ weg, die
an einem erreichbaren Stehplatz einen Figurpunkt ohne solide Zelle davor verdecken wuerden. Grund ist die Tiefe des Generators
(gen_masken.depth_map_for_cut: Bodenkontakt der untersten Regionszeile auf y = 0, DEPTH_SAFETY 0.90) - nicht das Objekt.
**Messung vorher** (Stand vor NB1 = 95b9f38d, Leon-Dateien, `m41nb1c/m1_stat.py`; Objekt = 8-Komponente der Generator-Region je
Verschiebung >= 64 px): ausgeschlossen 722 von 3239 alten und 831 von 4300 Saum-Rechtecken (Leon + Elza: 1662 von 8600 Saum);
gezeichnet 780 626 px, nur in ausgeschlossenen Rechtecken 450 317 px; Objekte 419: 217 ganz gezeichnet, 147 teilweise, 55 gar nicht.
**Weg (wie raum.py "zelle", nicht aus der Erreichbarkeit):** `re15_port/tools/maske/tiefe_zelle_r41.py` (f00fbf3e, Kopf = Regel):
Objekt -> Sperrzelle des Bands, die die Mehrheit seiner Bodenkontakte traegt (geometrie.zelle_zuordnen), Hoehe per IoU
(geometrie.hoehe_messen), Kamera-z je Punkt = Sehstrahl gegen den Quader (geometrie.quader_auf_band); Rechteck-Tiefe = max
bucket(vz) seiner Punkte, bucket(vz) = (1023*vz)>>16 (Figur-OT @0x8002565c `sra v1,v1,4`, ZSF3 = 341 @0x80066c70; Maske verdeckt
gdw. depth < bucket, Masken-OT = depth @0x80039650). Nicht bei Saeulen mit schraeger Zelle und nicht, wenn die Kamera in der
Zelle steht. Danach rechnet `test_r37_masken36 erzeuge` den Ausschluss neu (das Netz der Spur P bleibt); was trotz Zellen-Tiefe
aus bleibt, behaelt seine alte Tiefe (`--zurueck`). Nur das Tiefenfeld (+4 im 12-Byte-Satz) aendert sich, Atlas/Lage/Gruppen nicht.
**Schritt 1 (alle Punkte direkt getroffen):** 3 Iterationen im Scratchpad (`m41nb1/tz1..tz3`, tz2_aus == tz3_aus), MASKS =
tz3 (50c72950), Tabelle = `erzeuge` darauf (13803d3e, bitgleich zu tz3_aus.inc): **236 Rechtecke wieder drin (37 286 px), 0 neu
ausgeschlossen, 0 alte Ausschluesse ohne Kandidat entfallen** - 29 Cuts, groesste ROOM3050 C5 (Maschine links, 11 Rechtecke
10 399 px), 5020 C6/C7, 5100 C7, 5090 C12, 30A0 C3.
**Messung an der exe (Schritt 1 und 2)** (`m41nb1c/m1_szenen.py`, `m2_kette.py`, `sicht_alle.py`; je wieder aufgenommenem
Objekt die DAVOR-Stelle mit der staerksten Ueberdeckung ohne verdeckten Figurpunkt und, wo es eine gibt, die HINTER-Stelle mit
den meisten neu verdeckten Punkten - Python-Figurraster wie test_r37_masken36 auf den `plaetze`; an der exe Bild 120, Dreiecke
des letzten Bilds per RE15_TRILOG. Mass: Punkte, an denen Leon das VORDERSTE Dreieck ist und eine gezeichnete Maske mit
Schluessel (depth+1)*65536/1023 <= Dreieck-z liegt = sichtbar uebermalt (render_pc.c pc_masken_schatten_flush: die Maske wird
nach dem Dreieck gezogen). "vor NB1" = Tabelle 95b9f38d auf denselben Dateien. Hinweis: die erste Auswertung zaehlte ALLE
Leon-Dreiecke (auch von Leon selbst verdeckte Rueckseiten) - das ueberschaetzte; massgeblich ist das vorderste Dreieck):
* **DAVOR, 88 Szenen (39 Schritt 1, 49 Schritt 2) in 42 Cuts: neu sichtbar uebermalte Leon-Punkte 0** (und 0 weggefallen).
  Altbestand (schon vor NB1 gezeichnete Masken, am Leon-Rand 1-2 Spalten): 3040 C5 (-7500,-2000) Band 1 4 Punkte (altes
  Rechteck 50, Tiefe 76); 30A0 C4 (-22243,17763) 2 (Saum-Rechteck 41, Runde 41); 5100 C2 (-19830,-23600) 1; 4080 C8 (-225,-2010)
  2; 5050 C1 (-18100,-22950) 23 (alte Rechtecke 35/36 + Saum 64/72, Tiefe 68) -> OFFEN 7.
* **HINTER, 21 Szenen: sichtbar verdeckte Leon-Punkte vor NB1 389 -> nach NB1 6 190**; z.B. 3050 C5 (-11600,-11668) 2 -> 740
  (Maschine links), 5100 C7 (-14030,-2500) 0 -> 1085, 5100 C2 (-23730,-21100) 34 -> 756, 5020 C7 (-13533,-14125) 98 -> 770,
  5100 C3 13 -> 350, 4040 C9 (-14800,-12722) 0 -> 278, 4040 C7 8 -> 201, 6010 C6 0 -> 198, 30A0 C3 185 -> 336; ohne Wirkung an
  der gewaehlten Stelle: 5090 C12, 4040 C11 (-1500,5778) (Leon dort nicht vorn).
* Bilder `M_bilder/NB1_M1_*.png` (oben ganz: mit | ohne Masken | rot = veraendert; unten Ausschnitt).

**Schritt 2 (wie raum.py "zelle": Fehltreffer erben, `--erben`):** Schritt 1 verlangte, dass die Zelle JEDEN Punkt eines
Rechtecks direkt trifft (262 Rechtecke / 133 733 px scheiterten nur daran). raum.py baut die Nutzer-Freistellungen mit
geometrie.tiefe_zelle: Punkte ohne Treffer erben den naechsten Treffer desselben Objekts (geometrie._erben). Dieselbe Regel
fuer die noch ausgeschlossenen Rechtecke (`m41nb1c/tz4`, `tz4b`, `tz4_kette.sh`): 266 Kandidaten (240 mit geerbten Punkten,
39 578 geerbte Punkte), `erzeuge` -> **131 wieder drin (71 783 px), 0 neu ausgeschlossen**; `--zurueck` gibt den 135 weiter
ausgeschlossenen ihre alte Tiefe, zweiter `erzeuge` bitgleich (stabil); geaendert nur Tiefenfelder (308 Bytes in 24 Dateien,
0 ausserhalb der Tiefenfelder). 38 Cuts, groesste 4040 C9 (7 046 px), 30A0 C0 (6 888), 4080 C8 (6 268), 30A0 C1 (5 204), 4040 C6.
**Ergebnis statisch (Leon-Dateien, Elza bytegleich):**
| | vor NB1 | nach Schritt 1 | nach Schritt 2 |
|---|---|---|---|
| ausgeschlossene alte / Saum-Rechtecke | 722 / 831 | 607 / 710 | 530 / 656 (Leon + Elza Saum 1 312 von 8 600) |
| gezeichnete Generator-Punkte | 780 626 | 817 808 | 889 591 (+14,0 %) |
| Punkte nur in ausgeschlossenen Rechtecken | 450 317 | 413 135 | 341 352 (-24,2 %) |
| Objekte ganz / teilweise / gar nicht gezeichnet (von 419) | 217 / 147 / 55 | 230 / 138 / 51 | 273 / 99 / 47 |

### NB1 Mangel 3 - ROOM2030/2031 Cut 15 (4400,3850) Band 3: die "uebermalten Leon-Punkte" sind die Spinne (widerlegt)

**Ursache der Abnahme-Zahl:** das Mass S = |B - Hintergrund| > 24 im Figurfenster zaehlt JEDE Figur. Direkt hinter Leon (und
hinter der Reling) steht eine Spinne (Akteur 1, EM025 -> TIM-Slot 11, debug.log `[enemy] RE2 EM025 ... -> slot 11`).
**Messung** (Scratchpad `m41nb1c/`, `hs.py`/`ana.py`; exe = Spurbaum, Bild 120, RE15_WINDOW_SCALE=1; Dreiecke des letzten
Bilds per RE15_TRILOG, render_pc.c `re15_trilog`: Slot, z = Mittel der drei Kamera-z, main.c `avg_z`; eine Maske wird vor einem
Dreieck gezogen, wenn ihr Schluessel (depth+1)*65536/1023 > Dreieck-z, render_pc.c `pc_masken_schatten_flush`, = Original-Urteil
depth < (1023*vz)>>16, @0x8002565c `sra v1,v1,4`, ZSF3 = 341 @0x80066c70, re15_pri.h):
| Szene | S | H | Leon-Punkte (Slot 0) | davon unter einer Maske mit Schluessel <= Dreieck-z |
|---|---|---|---|---|
| Leon, Spinne da (= Abnahme) | 1762 | 166 | 1505 | **0** |
| Leon, Spinne weggestellt (RE15_GEGNER_POS 1,-20000,-20000) | 1225 | **0** | 1922 | **0** |
| Elza (2031), Spinne da | 1721 | 216 | 1037 | **0** |
| Elza, Spinne weggestellt | 1017 | **0** | 1320 | **0** |
Die uebermalten Punkte sind Slot 11 (Spinne, Dreieck-z 9596..9964) gegen Masken-Schluessel 8520..8905: die Reling verdeckt die
Spinne dahinter - richtig. Leons Dreiecke liegen bei z 6905..8911; die Reling-Tiefe 131..145 kommt aus der Zelle x 5400..5900
z -9000..8650 (Band 3, Typ 1, Hoehe 1650, auswahl.json/raum.py, Dossier 2.2), 1000 Einheiten HINTER Leon (x 4400). Die Tiefe des
Handlaufs ist richtig, **keine Aenderung**. Bilder `M_bilder/NB1_M3_ROOM2030_C15_mit_spinne.png`, `_spinne_weg.png`,
`NB1_M3_ROOM2031_C15_*.png` (mit | ohne Masken | rot = veraendert). Riegel: `test_r41_masken36_szenen.cmake` `v2030_15`/`v2031_15`
(VOR, Spinne weggestellt: H == 0) und `m2030_15` (Gegenprobe mit Spinne: die Reling verdeckt sie, H > 0).

### NB1 Mangel 4 - VOR-Stellen 2030 C7 / 2040 C7 mit Spielfigur im Bild: es gibt keine

**Ursache:** `e1_stellen.py` nahm die staerkste Ueberdeckung "VOR der Standlinie (.STAND)" und schnitt den Figurkasten nur ans
Bild - ob die Figur im Bild ist, wurde nicht verlangt (2030 C7 (0,-1950): sy 928; 2040 C7 (-17300,-8800): Leon nicht im Bild).
**Messung 1 - Standlinien-Modell mit Figur VOLL im Bild** (`c6_vorstellen.py`: Plaetze aus `test_r37_masken36 plaetze` =
Flutfuellung + Kamerazonen + Akteur-Cull FUN_8002c18c, Kasten 3000 x +-350 mit allen vier Ecken im Bild): 2030 C7 82 Plaetze
(staerkste (4500,9150) Band 3), 2040 C7 1477 (staerkste (-17500,-5600) Band 3). An der exe verdeckt die Maske Leon dort teilweise.
**Beleg - die Plaetze liegen HINTER dem Gelaender** (`c9_vor3d.py`): Sehstrahl Kamera -> Figurpunkt (7 x 13 Punkte des
Kastens) gegen die Gelaenderzelle als Quader (SCA Band 3, Hoehe per geometrie.hoehe_messen gegen die Freistellung):
* 2040 C7: Kamera (-17264,-7310,-9105), Gelaender x -18250..4150 z -6500..-6050, Hoehe 1800 -> von 2378 erreichbaren Plaetzen mit
  Figur voll im Bild und Ueberlappung mit der Maske: **0 VOR**, 807 ganz und 1571 teilweise HINTER (Kopf ueber dem Handlauf).
* 2030 C7: Kamera (-23,-8310,-2043), Ring-Gelaender x -6350..5900 z 8150..8650 und x 5400..5900 z -9000..8650, Hoehe 1875 ->
  von 767 Plaetzen **0 VOR**, 767 teilweise HINTER.
Die Kamera haengt in beiden Cuts VOR dem Gelaender ausserhalb des Stegs; jede erreichbare Stelle mit der Figur im Bild liegt
dahinter. **Ergebnis: keine begehbare VOR-Stelle mit Figur im Bild** (die VOR-Pruefung dieser Cuts ist leer, nicht verfehlt);
die Standlinien-"VOR"-Stellen sind HINTER-Stellen.
**Nebenbefund dabei (Punkt 2, behoben):** an diesen HINTER-Stellen verdeckte der Handlauf Leon NICHT (exe, Leon-Dreiecke):
2040 C7 (-17500,-5600): 355 Leon-Punkte unter der Maskenflaeche, 146 verdeckt (Pfosten, Tiefe 43), **209 nicht** (Handlauf
Tiefe 391/392, Leon z 2713..3169 = Bucket 42..49); 2030 C7 (4500,9150): 297 unter der Maske, 28 verdeckt, **269 nicht** (Tiefe
186..256 an der Ecke, Leon Bucket 179..193; sichtbares Mass - Leon vorderstes Dreieck - 20 von 297). Ursache: Profil-Tiefe (geometrie.tiefe_profil) - der Handlauf hat im Bild keinen
Bodenkontakt, seine Spalten erbten den Bodenpunkt weit hinten (2040 C7 "Profil-Tiefe 43..631"; 2030 C7 07_01 "83..202" fuer
Nord- UND Ostgelaender zusammen). **Aenderung** (auswahl.json, `"r41"`-Vermerk; Nutzer-PNG unveraendert): 2040 C7 `"zelle"`
[-18250,-6500,22400,450]; 2030 C7 07_01 an der Ecke (Bildspalte 173..186 = Projektion des Eckblocks x 5400..5900 z 8150..8650,
den BEIDE Zellen enthalten; die Teilung bei Spalte 179 liegt innerhalb dieser Ecke, links/rechts davon gehoert jede Spalte nur
zu einer Zelle) in zwei Objekte geteilt (`"minus"`), Nordgelaender `"zelle"` [-6350,8150,12250,500], Ostgelaender [5400,-9000,500,17650]; `raum.py ROOMxxxx 7
--p2 --stufe 16 --statistik max --dump build/p2/dump_klemmpfad_r40.txt` -> 2040 C7 33 Rechtecke Tiefe 35..50 (2898 von 2981
Punkten direkt getroffen), 2030 C7 105 Rechtecke Tiefe 78..258 (Nord 130..183, Ost 77..181, 07_02 unveraendert 246..258);
Bau-Schiene VOR verdeckt 0 / teil 0. Bilder vorher `M_bilder/NB1_M4_ROOM2030_C07_hinter.png`, `NB1_M4_ROOM2040_C07_hinter.png`.
**Messung nachher** (exe, dieselben Stellen, Leon = vorderstes Dreieck, RE15_TRILOG): 2040 C7 (-17500,-5600) **355 von 355**
Leon-Punkten unter der Maske verdeckt (vorher 146), Elza (2041) 188 von 188; 2030 C7 (4500,9150) 83 von 297 (vorher 20) - die
214 offenen liegen alle unter 07_02 (Pult an der Nordwand, Zelle x 5400..7750 z 12800..13300, Tiefe 246..256, HINTER Leon) = richtig,
d.h. alle Gelaenderpunkte vor Leon verdeckt; Elza (2031) 57 von 200 (Rest ebenso unter 07_02). Bilder
`M_bilder/NB1_M4_ROOM2040_C07_nachher.png`, `NB1_M4_ROOM2041_C07_nachher.png`, `NB1_M4_ROOM2030_C07_nachher.png`.
Riegel-Szenen `m2030_07` / `m2040_07` (test_r41_masken36_szenen.cmake, Art M: H > 0).

### NB1 Mangel 2 - ROOM20A0 C12, 12_02 (erledigt durch Nutzer-Entscheid)

Nutzer (AUFTRAG.md Nachtrag 11): "Weglassen ist ok" - 12_02 bleibt aus dem Bau. Die Begruendung ist berichtigt (a24e2389,
Abschnitt 2.3 und auswahl.json `"r41"`): 12_02 ist der Boden vor der Schwelle unter dem ROLLADEN obj 1 (Obj_model_set @0x01932,
(7942,-7200,-20510)), Zelle 32 x 8000..14700 z -21000..-20500 Band 4, dahinter der Schacht (Runde 38 W_rolltor20a0.md) - nicht
"Ziegelwand". Keine weitere Aenderung.

### NB1 Mangel 5 - Doku/Nebenbefunde

(a) original.py-Kommentar: "0 von 359 / Stage 3..6 0 von 222 / Generator-Raeume 0 von 116" (a24e2389). (b) Kopf von
schliessen_r41.py ohne den verworfenen Weg (a24e2389). (c) `M_bilder/P1_exe_*.png` -> `P1_verworfen_erreichbarkeit_*.png`
(a24e2389, Vermerk in 1.5). (d) Dossier 1.4 DAVOR-Aussage berichtigt (d5a9201c): 4040 C11 +1 / 4080 C7 +2 = Tuermodell-Kanten;
5100 C7 = Leons Hand unter dem ALTEN Rechteck 0 (Tiefe 64), die Abnahme-Punkte (38,236)/(39,237) liegen nur unter Rechteck 0
(kein Saum-Rechteck), Leon-Dreiecke gegen Schluessel: 120 Rasterpunkte verdeckt vor und nach NB1; +2 im Bildvergleich = andere
Texelfarbe des eigenen Atlas. Altbestand -> OFFEN 6.

### NB1 Auflage Nachtrag 11 - die 10 Uebersichtsbilder ohne berichtigtes Gegenstueck

Die 10 Dateien (pri/STAGE3/3040+3041/01, 3050+3051/06+15, 30E0+30E1/06+07) hatten kein Gegenstueck, weil in diesen 5 Cuts
KEIN Maskenpunkt gezeichnet wird: alle Rechtecke sind ausgeschlossen (3040 C1 19 von 19, 3050 C6 19, 30E0 C6 35, 30E0 C7 53;
3050 C15 = Sektion ohne Stehplatz = Original-NULL-Sektion, R37 NB2 - auch nach NB1 Mangel 1: keine Zellen-Tiefe uebersteht dort
den Ausschluss). Die alten Bilder unter pri/ zeigen Masken, die das Spiel nie zeichnet. `bilder_r41.py` schreibt fuer solche Cuts
jetzt ein volles, ganz durchsichtiges Bild 320 x 240 (RGBA) in den Leon- UND den Elza-Ordner:
`M_bilder/uebersicht/STAGE3/{3040,3041}/01_port.png`, `{3050,3051}/06_port.png`, `{3050,3051}/15_port.png`,
`{30E0,30E1}/06_port.png`, `{30E0,30E1}/07_port.png`. Ausserdem sind alle Uebersichtsbilder und Bogen `M_bilder/ROOMxxxx_Ccc.png`
mit dem Endstand von NB1 neu erzeugt (wieder aufgenommene Rechtecke, Mangel 1) - geaenderte Bilder s. "Fuer den Nutzer".

## Nachbesserung 2 (2026-10-24, nach Abnahme 1 = analysis/befunde_runde41/M_abnahme_1.md)

Grundlage: Abnahme 1 (Urteil "nicht bestanden": Punkt 1 teilweise, Punkt 2 erfuellt), Mangel 1 = Punkt 1 weiter nicht
geschlossen (530 alte + 656 Saum-Rechtecke aus, 341 352 Punkte nur in ausgeschlossenen Rechtecken, 4 Cuts ganz ohne Maske:
3040 C1, 3050 C6, 30E0 C6, 30E0 C7; echte Gegenstaende 3050 C6 Treppengelaender, 5020 C6 X-Kiste, 6000 C5 Lok/Traeger).
Weg der Abnahme: (a) die 135 Rechtecke mit Zellen-Tiefe, die nur das Grundriss-Netz der Spur P nimmt, formgenau in 3D mit
gemessener Hoehe entscheiden; (b) die uebrigen echten Gegenstaende dem Nutzer zur Freistellung vorlegen.
Keine Messung der Abnahme wird wiederholt; ihre Zahlen werden zitiert. Punkt 2 (erfuellt) wird nicht angefasst.

### Stand Nachbesserung 2
- [x] Lektuere (AUFTRAG, VERTRAG, Abnahme 1, Dossier, git log master..HEAD) - Werkzeuge tiefe_zelle_r41.py, test_r37_masken36.c
- [x] Mangel 1 (a): Ausschluss-Urteil fuer Rechtecke mit Zellen-Modell formgenau in 3D (r41_koerper_davor), auch fuer die "nicht
      ferner"-Modelle - 80 Rechtecke (Leon) wieder drin, u.a. die Lok 6000 C5/C7/C8/C9
- [x] Mangel 1: die vier Cuts ohne Maske = Boden/Wand-Regionen (gemessen, richtig ohne Maske); 3050 C6 "Treppengelaender" = Boden vor
      dem Gelaender; X-Kiste 5020 C6 per Zuordnung von Hand geschlossen; 6000 C5 Lok + Gittertraeger oben geschlossen
- [x] Zuordnung von Hand fuer 12 Objekte (zuordnung_r41.json; Nahfeld-Sperre nach der Gegenprobe 5050 C2) -> zusammen 164 Rechtecke
      wieder drin, 956 982 statt 889 591 Punkte (Kette k3)
- [x] Mangel 1 (b): Restliste fuer den Nutzer `M_bilder/NB2_rest/` + Liste echter Gegenstaende ("Fuer den Nutzer")
- [x] exe-Messung nachher (DAVOR 0 neu sichtbar uebermalt; Randpunkte hinter dem Koerper nachgerechnet), gezielte Tests, Abschluss-Commit

### NB2 Zusammenfassung Mangel 1 (Abnahme 1) - Ursache, Messung vorher, Beleg, Aenderung, Messung nachher
* **Ursache:** (1) Das Grundriss-Netz der Spur P (test_r37_masken36.c `zelle_dazwischen`) urteilt ohne Hoehe und nur mit Zellen
  DESSELBEN Bands; Zellen, in deren Grundriss die Kamera steht, zaehlen nicht - Rechtecke mit Zellen-Tiefe fielen deshalb weiter
  weg (135), und NB1 schrieb Zellen-Tiefen nur, wenn sie FERNER als die Generator-Tiefe lagen (104 "nicht ferner": u.a. die Lok
  6000 C5). (2) Die automatische Zuordnung (Mehrheit der Bodenkontakte in einer Sperrzelle) scheitert an Gegenstaenden auf Kufen,
  mit angeschnittener Unterkante, am Bildrand oder knapp unter der Mehrheit (X-Kiste 5020 C6 0,19; Trennwand 30A0 C0/C1 0,49/0,46).
  (3) Die vier Cuts ohne Maske zeigen in der Generator-Region nur Boden/Rueckwand (gemessen, Bild NB2_vier_cuts_region.png).
  (4) Gefunden in NB2 selbst: steht die Kamera UEBER dem Grundriss einer Modellzelle, traegt weder das 3D-Urteil noch das
  Grundriss-Netz die nahe Tiefe (5050 C2: 381 Gitterpunkte ohne Koerper davor) -> Nahfeld-Sperre.
* **Messung vorher (Abnahme 1, eigene Zaehler bitgleich nachgerechnet):** 1186 Rechtecke (Leon) aus, 889 591 Punkte gezeichnet,
  341 352 nur in ausgeschlossenen Rechtecken, Kerben 11 494 von 62 432 (18,4 %); X-Kiste 5020 C6 ganz aus (fehlte im Uebersichtsbild),
  Lok 6000 C5 groesstenteils aus.
* **Beleg (Adressen/Bytes):** Urteil "verdeckt" wie im Original: Figur-OT = otz>>4 @0x8002565c (`sra v1,v1,4`) mit ZSF3 = 341
  @0x80066c70 (`addiu t0,zero,341` / `ctc2 t0,$29`) -> bucket(vz) = (1023*vz)>>16; Masken-OT = depth @0x80039650 (`lh a0,2(s3)` ->
  AddPrim @0x8006b538), Masken zuletzt im Bild @0x8001ce54 (`jal 0x80039590`) -> verdeckt gdw. depth < bucket(vz) (re15_pri.h). Die
  Koerper sind SCA-Zellen der RDT (je Zeile der Modell-Tabelle die 12 Bytes mit RDT-Offset, vom Riegel byte-genau geprueft), ihre Form
  fragt der Kollisions-Resolver des Ports (re15_collision.c, FUN_8003b0a4), Spielerradius 450 (DAT_80073e94[6]). Die Hoehe ist eine
  Messung (IoU-Maximum der Koerper-Silhouette gegen den Gegenstand, wie raum.py "zelle" fuer die Nutzer-Freistellungen) - keine
  Spielkonstante. Original: die Stages 3..6 haben fuer diese Cuts KEINE Masken (NULL-Sektion, @0x80039324/@0x80039338) - die
  Generator-Masken sind eine Port-Ergaenzung (PORT-WAHL seit R37); Ziel ist "keine Figur VOR dem Gegenstand uebermalen".
* **Aenderung:** 3D-Urteil im Ausschluss-Generator (`r41_koerper_davor`, nur zusaetzlich zum Grundriss-Netz; kein Urteil bei
  Kamera ueber dem Grundriss der Zelle), Zellen-Tiefe auch naeher als die alte (`tiefe_zelle_r41.NAEHER`), Zuordnung von Hand fuer 12
  Gegenstaende (`zuordnung_r41.json`), Kette k3 mit `--zurueck` (stabil). Nur Tiefenfelder von 164 Rechtecken in 5 MSK-Paaren +
  Ausschluss-Tabelle + Modell-Tabelle (Test-Daten).
* **Messung nachher (Kette k3):** 1022 aus, **956 982 gezeichnet (+67 391)**, 273 961 nur ausgeschlossen, Kerben **10 178 von 63 546
  (16,0 %)**; 0 Rechtecke neu ausgeschlossen; unabhaengige 3D-Gegenprobe in den geaenderten Cuts 0 Verdeckungen ohne Koerper davor;
  exe DAVOR 0 neu sichtbar uebermalt
  (Randpunkte in 30A0 C0/5020 C0 nachgerechnet hinter dem Koerper), HINTER verdeckt (**berichtigt in NB3: in 30A0 C0/C1 nur unten - die
  Figur hinter der Trennwand blieb oben sichtbar, Abnahme 2 Mangel 1**); FREI-Stellen der R37 (30A0/30A1 C0, "Haar"):
  exe 0 Leon-Punkte uebermalt. X-Kiste 5020 C6 und Lok 6000 C5 gezeichnet (Uebersichtsbilder neu). Rest: Restliste fuer den Nutzer.

### NB2 Ausgangslage (Lektuere, keine neue Messung)
Stand nach NB1 (Bericht `m41nb1c/tz4b/tiefe_zelle_r41.json` des Bau-Agenten, eigene Auszaehlung): von 1317 Rechtecken, die vor NB1
ausgeschlossen waren, haben 370 ein Zellen-Modell (Zelle + gemessene Hoehe): 131 wieder drin (Schritt 2), **135 trotz Zellen-Tiefe
weiter aus** (davon 70 in 3050 C15 = Sektion ohne Stehplatz, Original-NULL, bleiben aus; 26 ohne geerbten Punkt) und **104 mit
"Zellen-Tiefe nicht ferner als die alte"** (NB1 hat nur FERNERE Tiefen geschrieben; 6000 C5 Lok 27 Rechtecke 17 941 px, 6000 C7/C8/C9,
4040 C6/C7/C9/C10, 4080 C7, 3050 C9, 5050 C1). 947 Rechtecke (256 518 px) haben kein Modell (576 x "keine Zelle traegt die Mehrheit
der Bodenkontakte", 45+ "kein Bodenkontakt", 66 "Zelle trifft bei keiner Hoehe", 37 "Saeule mit schraeger Zelle").
Die vier Cuts ohne Maske (3040 C1, 3050 C6, 30E0 C6, 30E0 C7) sind laut Bildern `M_bilder/NB1_rest/` ueberwiegend Boden-/Wandflaechen,
die der Generator in seine Region nahm (30E0 C6/C7: Bodenstreifen vor dem Gelaender; das Gelaender selbst ist NICHT in der Region).

### NB2 Plan (Weg (a) der Abnahme, als Regel im Ausschluss-Generator)
Warum das Grundriss-Netz der Spur P diese Rechtecke nimmt: `zelle_dazwischen` (test_r37_masken36.c) kennt nur Zellen DESSELBEN Bands
und laesst Zellen aus, in deren Grundriss die Kamera steht; die Hoehe kennt es nicht. Fuer ein Rechteck mit Zellen-Modell ist die
richtige Frage die 3D-Frage: liegt der verdeckte Figurpunkt F auf dem Sehstrahl HINTER dem Koerper des eigenen Objekts (Zellform x
[Bandboden, Bandboden + gemessene Hoehe])? Dann zeigt der Hintergrund an diesem Punkt das Objekt VOR der Figur, die Verdeckung ist
richtig. Regel neu: ein verdeckter Figurpunkt ist "mit Gegenstand davor", wenn (alt) die Grundriss-Strecke eine solide Zelle des
Bands kreuzt ODER (neu) die Strecke Kamera -> F den Koerper der Modell-Zelle des verdeckenden Rechtecks vor F schneidet (F und Kamera
nicht im Koerper). Die Regel nimmt nur dazu - kein bisher gezeichnetes Rechteck faellt dadurch weg. Rechtecke mit Modell bekommen die
Zellen-Tiefe (auch naeher als die alte); was danach weiter aus ist, bekommt die alte Tiefe zurueck (wie NB1 --zurueck).

### NB2 Mangel 1 (a) - 3D-Urteil fuer Rechtecke mit Zellen-Modell (Umsetzung, Zwischenstand 18:4x)
**Umsetzung:**
* `re15_port/tools/maske/zellen3d_r41.py` (neu): Kandidaten = alle vor NB1 ausgeschlossenen Rechtecke (Tabelle 95b9f38d), Modell
  wie NB1 Schritt 2 (`tiefe_zelle_r41.kandidaten`, ERBEN) und neu NAEHER (Zellen-Tiefe auch, wenn sie naeher als die alte liegt;
  Flag in `tiefe_zelle_r41.py`, 5 Zeilen). Rechtecke ueber Objekte mit VERSCHIEDENEN Zellen bekommen kein Modell (das 3D-Urteil kennt
  je Rechteck nur einen Koerper; gezaehlt: 1, ROOM5020 C6 R82). Schreibt MSK (nur Tiefenfelder) und die Modell-Tabelle
  `re15_port/tests/unit/gen_r41_masken36_zellen.inc` (je Zeile Zelle, Typ, Band, Hoehe, Saeule, Tiefe, BELEG = RDT-Offset + die 12
  SCA-Bytes dort, Kommentar IoU).
* `re15_port/tests/unit/test_r37_masken36.c`: `r41_koerper_davor` - Strecke Kamera -> Figurpunkt gegen den Koerper (Huellkoerper
  per Achsen-Schichten, Form ueber den Kollisions-Resolver `form_drin` mit den Original-Bytes, Raster 25 wie `form0`); Kamera bzw.
  Figurpunkt im Koerper = kein Urteil. Im Ausschluss (`sektion_pruefen`) zaehlt ein verdeckter Figurpunkt als "Gegenstand davor",
  wenn das Grundriss-Netz (unveraendert) ODER der Koerper des Modells des verdeckenden Rechtecks davor liegt. `figpt_t.h` = Hoehe des
  Punkts ueber dem Bandboden (Spalten-Parameter t * 3000), `raum_t.raum`. Werkzeug-Modus `warum` (Gruende je ausgeschlossenem
  Modell-Rechteck). Riegel-Ausgabe: Zahl der nur per 3D belegten Punkte je Stage.
* Kette (Scratchpad `m41nb2/`): `zellen3d_r41.py` (605 Modelle) -> MASKS -> `erzeuge` t1 -> `zellen3d_r41.py --zurueck t1`
  (446 Modelle, nur die gezeichneten) -> MASKS -> Neubau -> `erzeuge` t2: **t2 == t1 (stabil)**.
**Ergebnis statisch (Leon; Elza bytegleich):** ausgeschlossen 1186 -> **1106**; **80 Rechtecke wieder drin (+80 Elza), 0 neu aus**;
geaendert nur die Tiefenfelder dieser 80 (58 naeher, 22 ferner; 0 Bytes ausserhalb). Je Cut: 3040 C3 R9; 5050 C1 R1/R4/R47;
6000 C5 26 (u.a. R0 = Lok 11 664 px, R1, R3, R23..R37 Gittertraeger), 6000 C7 12, 6000 C8 24, 6000 C9 14.
**Warum der Rest der Modell-Rechtecke aus bleibt** (`test_r37_masken36 warum`): 159 (70 = 3050 C15 ohne Stehplatz); von den
uebrigen 89 verfehlt bei 86 der Sehstrahl zu einem verdeckten Figurpunkt den Koerper (Modell erklaert nicht jeden Punkt des
Rechtecks: geerbte Punkte, Typ-2-Rauten in 4040/4080, deren Tiefe der Python-Weg aus dem Huellrechteck rechnet), bei 3 (5090 C11
Typ-6-Dreieck) liegt ein Figurpunkt im Koerper. Diese bleiben aus - die 3D-Regel nimmt nur, was der Koerper belegt.

**Messung nachher (a), statisch** (eigener Zaehler `m41nb2/py/zaehlen.py`, wie stat_nb1 der Abnahme; Ausgangszahlen der Abnahme
bitgleich nachgerechnet): gezeichnete Generator-Punkte (Leon Stage 3..6) **889 591 -> 920 514** (+30 923), nur in ausgeschlossenen
Rechtecken **341 352 -> 310 429**; ausgeschlossene Rechtecke 1186 -> 1106.
**Unabhaengige 3D-Gegenprobe** (`m41nb2/py/statisch.py`, Python, NICHT der C-Code: alle erreichbaren Stehplaetze aus
`test_r37_masken36 plaetze`, Figur 15 Spalten x 31 Hoehen, Sehstrahl abgetastet; (A) eigener Modellkoerper, (B) irgendein solider
Zellkoerper aller Baender mit Form und pauschal 2400 hoch wie Abnahme 1 scan_form): fuer die NB2-Rechtecke 3040 C3 14 verdeckte
Punkte / 0 ohne Koerper davor, 5050 C1 3 979 / 0, 6000 C5 1 249 905 / 0, 6000 C8 757 365 / 0, 6000 C9 545 122 / 0, 6000 C7
392 073 / **2** (Rechteck 28, Fusspunkt am unteren Bildrand (303,223) an (21633,-20084) Band 0 unter dem Band-2-Bahnsteig, 2 Punkte
des groben Gitters; der pixelgenaue C-Riegel sieht dort keinen). Nebenbefund Altbestand NB1 (unveraendert): 4040 C6 5 094 / 4040 C7
5 608 Punkte, an denen der Kopf ueber der GEMESSENEN Hoehe 3025 des Mittelblocks liegt (Kamera y -3527, Strahl bei s 0,7 auf
y -3110..-3200) - die Masken-Region zeigt dort den Block (Kunst hoeher als das IoU-Modell); Abnahme 1 hatte die Stelle an der exe
(ih4040_06) als "Leon hinter dem Block" bestaetigt. Kein NB2-Rechteck.
**Messung nachher (a), exe** (Spurbaum-exe 18:17, Bild 120, RE15_WINDOW_SCALE=1, A mit / B ohne Masken (RE15_NO_PRI), Leon-Dreiecke
per RE15_TRILOG; sichtbar uebermalt = Leon vorderstes Dreieck UND Masken-Schluessel (depth+1)*65536/1023 <= z, render_pc.c
pc_masken_schatten_flush; vorher = Tabelle vor NB2 auf denselben Laeufen; Szenen `m41nb2/py/szenen.py`: je Cut die Stehplaetze mit
den meisten NEU verdeckten Punkten (HINTER) bzw. der groessten Ueberlappung ohne neue Verdeckung (DAVOR)):
| Szene | Platz (Band) | Leon vorne | sichtbar uebermalt/verdeckt vorher -> nachher |
|---|---|---|---|
| d6000_05_0 DAVOR | (12133,-10084) B2 | 2730 | 0 -> **0** |
| d6000_05_1 DAVOR | (15433,-4184) B2 | 2540 | 0 -> **0** |
| d6000_07_0 DAVOR | (15133,-18684) B2 | 6192 | 0 -> **0** |
| d6000_08_0 / _1 DAVOR | (12733,-18684) / (12033,-19484) B2 | 3054 / 3349 | 0 -> **0** / 0 -> **0** |
| d6000_09_0 / _1 DAVOR | (7233,-18684) / (8833,-18684) B2 | 1101 / 744 | 0 -> **0** / 0 -> **0** |
| h6000_05_0 / _1 HINTER | (14233,-15484) / (14533,-14284) B0 | 1417 / 1321 | 0 -> 1387 / 0 -> 1229 |
| h6000_07_0 / _1 HINTER | (15133,-10584) / (15233,-9184) B0 | 1085 / 958 | 0 -> 925 / 0 -> 921 |
| h6000_08_0 / _1 HINTER | (15033,-12784) / (15333,-14484) B0 | 964 / 936 | 0 -> 925 / 0 -> 910 |
| h6000_09_0 / _1 HINTER | (14033,-14984) / (15233,-14484) B0 | 471 / 396 | 0 -> 437 / 0 -> 385 |
| h5050_01_0 / _1 HINTER | (-21600,-24450) / (-22900,-24050) B0 | 2513 / 1811 | 0 -> 76 / 9 -> 78 |
| h3040_03_0 HINTER | (-21200,-12600) B1 | 1177 | 0 -> 20 |
Die HINTER-Stellen in 6000 liegen auf Band 0 (Gleisebene) im Grundriss der Lok (Zelle x -7857..16380 z -18219..-10574 Band 2,
RDT-Beleg in der Tabelle): Leon steht unter/hinter dem Lokkoerper. **Props:** die neuen Lok-Rechtecke verdecken in C5/C9 einen Teil des
3D-Tors (Slot 5, Obj_model_set[1] (20916,0,-18540), debug.log): der Sehstrahl Kamera C5 (8794,-9403,-7278) -> Tor-Ursprung in den
Hoehen 0..5400 schneidet den Lokkoerper (gerechnet), das Tor steht 4 500 hinter der Lokfront (x 20916 gegen 16380) - die
Verdeckung ist geometrisch richtig (vorher zeichnete das Spiel das Tor ueber die Lok; Bilder `M_bilder/NB2/NB2_M1_d6000_0*_tor.png`).
Bilder: `M_bilder/NB2/NB2_M1_<szene>.png` (oben: mit | ohne Masken | rot = veraendert; unten Ausschnitt).

### NB2 Mangel 1 - die vier Cuts ohne Maske und das "Treppengelaender" 3050 C6 (gemessen: was die Region dort IST)
Bild `M_bilder/NB2/NB2_vier_cuts_region.png` (je Cut: Hintergrund | Generator-Region rot, `m41nb2/py/region_crop.py` aus dem
Regionen-Cache `m41/grown`, Objekte `m41nb2/py/objekte.py`):
* **3050 C6**: zwei Objekte - Verschiebung (184,137) 2 150 px, Bildzeilen 139..182 = der BODEN vor/unter dem Treppengelaender (die
  Pfosten stehen in Zeile 113..150 und sind nur mit ihren Fussenden angeschnitten); (75,-15) 717 px, Zeile 4..44 = Rohre unter der
  Decke (Tiefe 0 = vor allem). Das Gelaender selbst (duenne Pfosten) ist NICHT in der Region.
* **30E0 C6 / C7**: der Bodenstreifen vor dem Gelaender (C6 x 120..320 y 160..208; C7 die Bodenflaeche vorn) - die Gelaenderstaebe sind
  nicht in der Region.
* **3040 C1**: die Rueckwand (Rautengitter links), die Haengeleuchte unter der Decke und ein Bodenfleck am Treppenfuss.
Eine Maske aus Boden oder Raumwand verdeckt nie richtig etwas (eine stehende Figur ist nie HINTER dem Boden, auf dem sie steht, und
nie hinter der Raumbegrenzung) - sie kann nur eine Figur DAVOR uebermalen; das ist der Fehler, gegen den der Ausschluss der Spur P
gebaut ist (masken36_r37.c Kopf: "Bodenflaechen in der Auswahl (ROOM30E0 Cut 7, ROOM3050 Cut 6 ...)"). **Ergebnis:** dass diese vier
Cuts im Spiel keine Generator-Maske zeichnen, ist richtig. Was dort fehlt, sind die GELAENDER (3050 C6, 30E0 C6/C7) - die hat der
Generator nie gefunden; sie brauchen eine Freistellung (Restliste fuer den Nutzer).

### NB2 Mangel 1 - echte Gegenstaende ohne automatisches Modell: Zelle von Hand (mit Beleg je Objekt)
**Ursache (gemessen, `m41nb2/py/sichtung.py`):** die automatische Zuordnung (geometrie.zelle_zuordnen) verlangt, dass die MEHRHEIT der
Bodenkontakte (unterste Regionszeile je Spalte, auf den Boden zurueckprojiziert) in EINER Sperrzelle liegt. Das scheitert an echten
Gegenstaenden, deren Unterkante nicht der Boden ist: die X-Kiste 5020 C6 steht auf zwei Kufen (Anteil 0,19 - die Spalten zwischen
den Kufen schauen auf den Boden HINTER der Kiste), die Buero-Trennwand 30A0 C0/C1 (0,49 / 0,46 - knapp unter der Mehrheit), Kisten
mit angeschnittener Unterkante (5090 C13 0,41) und Teile ueber einem Hohlraum (5020 C0 Treppenwange, 3040 C0 Schaltkasten auf der
Kiste: "kein Bodenkontakt").
**Weg:** je solcher Gegenstand die Zelle VON HAND (`re15_port/tools/maske/zuordnung_r41.json`, **12 Eintraege**; Werkzeug
`zellen3d_r41.py --zuordnung`, `tiefe_zelle_r41.zelle_von_hand`/`koerper_geometrie`). Ausgewaehlt wurde je Objekt die Sperrzelle eines
begehbaren Bands, deren Koerper-Silhouette (gemessene Hoehe, IoU-Maximum wie raum.py "zelle") den Gegenstand SELBST zeigt -
Grundriss unter dem Fuss des Gegenstands, Vorderkante = Unterkante im Bild (Bild je Eintrag `M_bilder/NB2/NB2_zuordnung_*.png`:
links Objekt gelb/rot, Mitte Kandidat 1, rechts Kandidat 2). Die Hoehe wird an ALLEN Objekten derselben Zelle im Cut gemessen
(ein Gegenstand, den die Region in mehrere Komponenten teilt, hat eine Hoehe). Bei der X-Kiste lag Zelle 10 (IoU 0,63, Saeule)
knapp vor Zelle 12 (0,61) - Zelle 10 steht aber HINTER der Kiste (Unterkante im Bild hoeher), die Kufen stehen in Zelle 12
(Grundriss x -4875..-2275 z -15425..-12825, Bytes @RDT+0x00870 `28 0a 28 0a f5 ec bf c3 01 ff 00 03`). Nicht von Hand zugeordnet
(kein passender Koerper, IoU <= 0,23 oder Hoehe 25 = Bodenplatte): 6000 C7/C8/C9 (Lok-Unterteil/Treppe), 5100 C7, 5050 C1 Schreibtisch
links, 30A0 C3 Schraenke (Zelle 6/7 zeigen nicht den Schrank), 30A0 C1 Gegenstand am Boden.
**Nahfeld-Sperre (gemessen, Kette k2 -> k3):** in der ersten Kette (k2) standen 16 Zuordnungen; die unabhaengige 3D-Gegenprobe
(`statisch.py`) fand in **5050 C2** (Pult vorn, Zelle x -27872..-24102 z -25960..-20835, Hoehe 3475) **381 verdeckte Gitterpunkte ohne
jeden Koerper davor** (z.B. Rechteck 55, Tiefe 6, an (-21600,-18350): der Strahl verlaesst den Grundriss oberhalb des Deckels). Ursache:
die Kamera (-24301,-3719,-23493) steht UEBER dem Grundriss der Zelle - der Koerper liegt im Nahfeld, die Rechteck-Tiefe faellt gegen 0,
das 3D-Urteil haengt allein an der gemessenen Hoehe, und das Grundriss-Netz laesst Zellen mit der Kamera darin aus (sein Urteil
kommt dann von fremden Zellen ohne Hoehe). **Regel jetzt:** steht die Kamera ueber dem Grundriss der Modellzelle, gibt das 3D-Urteil
kein Urteil (`r41_koerper_davor` -1) und eine Zuordnung von Hand ist dort unzulaessig (`tiefe_zelle_r41`, Kopf von zuordnung_r41.json).
Raus deshalb: 5050 C1 Pult rechts, 5050 C2 Pult vorn, 30A0 C0 Moebel vorn, 5090 C11 Kiste vorn links (alle mit Kamera ueber der Zelle),
und aus NB2 (a) 3040 C3 R9 sowie 5050 C1 R1/R4/R47 (automatische Modelle mit Kamera ueber der Zelle; das Grundriss-Netz allein nimmt sie).
**3D-Urteil, Feinschliff:** ein Figurpunkt IM Koerper (Kamera draussen) zaehlt als verdeckt - die Strecke Kamera -> Punkt tritt vor
ihm in den Koerper ein (vorher "kein Urteil"; damit 5090 C11 R0/R5/R89 ueber ihr automatisches Dreiecksmodell, Kamera nicht darueber).

### NB2 Mangel 1 - Ergebnis nach Kette k3 (3D-Urteil + Nahfeld-Sperre + 12 Zuordnungen von Hand), Messung nachher
**Kette k3** (`m41nb2/kette2.sh k3`: zellen3d mit Zuordnung -> MASKS -> Neubau -> erzeuge -> --zurueck -> MASKS -> Neubau -> erzeuge):
Modell-Tabelle 705 -> **530 Zeilen**, **zweites erzeuge bitgleich zum ersten (stabil)**, Tabelle eingebaut, exe neu gebaut. Ein dritter
Lauf des letzten Werkzeugschritts (Determinismus, am Stand k2 gemessen) lieferte MSK und Modell-Tabelle bitgleich.
**Statisch (Leon Stage 3..6; Elza bytegleich; Zaehler wie stat_nb1 der Abnahme, Ausgangszahlen bitgleich nachgerechnet):**
| | vor NB2 (= nach NB1) | nach NB2 (a), nur Automatik | nach NB2 (k3, endgueltig) |
|---|---|---|---|
| ausgeschlossene Rechtecke | 1186 | 1106 | **1022** (0 neu ausgeschlossen) |
| gezeichnete Generator-Punkte | 889 591 | 920 514 | **956 982** (+67 391, +7,6 %) |
| Punkte nur in ausgeschlossenen Rechtecken | 341 352 | 310 429 | **273 961** (-19,7 %) |
Geaendert sind nur die Tiefenfelder (+4 im Satz) der 164 wieder aufgenommenen Rechtecke (57 naeher, 107 ferner; 0 Bytes ausserhalb,
`m41nb2/msk_diff.py`) in 5 MSK-Paaren (ROOM3040/3041, 30A0/30A1, 5020/5021, 5090/5091, 6000/6001); Atlas/Lage/Gruppen unveraendert.
Wieder drin (Leon; Elza gleich): 3040 C0 3 (Schaltkasten), 30A0 C0 22 / C1 6 (Trennwand), 5020 C0 24 (Treppenraum), 5020 C6 20 (X-Kiste
+ Kufen), 5090 C11 3 (Kiste vorn links, automatisches Modell), 5090 C13 10 (Kistenstapel), 6000 C5 26 / C7 12 / C8 24 / C9 14 (Lok,
Gittertraeger oben). Das Uebersichtsbild `M_bilder/uebersicht/STAGE5/5020/06_port.png` zeigt die X-Kiste jetzt (Abnahme 1: fehlte).
**Warum der Rest aus bleibt (1022 Rechtecke, Leon):** 682 kein Modell - keine Sperrzelle mit der Mehrheit der Bodenkontakte (ueberwiegend
Waende, Boeden, Decken: 4040/4080 Gangwaende, 5020 C11 Boden, 30E0 Bodenstreifen, 3040 C1 Rueckwand, 5100 Decke; dazu die echten
Gegenstaende der Restliste); 105 mit Modell, aber ein verdeckter Figurpunkt liegt nicht hinter dem Koerper oder die Kamera steht ueber
der Zelle (z.B. 6000 C5 unterer Gittertraeger: IoU 0,86, die Rechtecke reichen ueber die Zelle hinaus); 70 Sektion ohne Stehplatz
(3050 C15, Original-NULL); 66 Zelle trifft bei keiner Hoehe; 62 kein Bodenkontakt; 37 Saeule mit schraeger Zelle.
**Unabhaengige 3D-Gegenprobe ueber ALLE Modell-Cuts** (`m41nb2/py/statisch.py`, Stand k3): verdeckte Figurpunkte ohne jeden Koerper davor
nur noch im Altbestand NB1 (4040 C6 5 094 / C7 5 608: Kopf ueber der gemessenen Blockhoehe, OFFEN 11; 3040 C9 3) und 6000 C7 2 Punkte
des groben Gitters am Bildrand; **0 in jedem durch NB2 geaenderten Cut** (3040 C0, 30A0 C0/C1, 5020 C0/C6, 5090 C11/C13, 6000 C5/C8/C9).
**exe (Szenen je Modellzelle, Leon = vorderstes Dreieck, RE15_TRILOG; vorher = Tabelle vor NB2):**
| Szene | Platz (Band 0) | Leon vorne | sichtbar uebermalt/verdeckt vorher -> nachher | Befund |
|---|---|---|---|---|
| d3040_00 DAVOR (Schaltkasten) | (-22600,-2000) | 1911 | 0 -> 0 | frei |
| d5020_06 DAVOR (X-Kiste) | (-1733,-14625) | 1894 | 0 -> 0 | frei |
| d5090_13 DAVOR (Kistenstapel) x 2 | (20717,-21365) / (20717,-26065) | 1195 / 1700 | 0 -> 0 / 55 -> 55 (Altbestand) | frei |
| d5090_11 DAVOR (Kiste vorn links) | (3117,-25565) | 2909 | 7 -> 7 (Altbestand) | frei |
| d30A0_01 DAVOR (Trennwand) | (-21243,13663) | 4035 | 0 -> 0 | frei |
| d30A0_00 (Trennwand) | (-21143,15863) | 2407 | 14 -> 39 | die 25 neuen Punkte liegen HINTER dem Koerper (Leon z 7112..7220 > Eintritt 5077..5191, Zelle 6) = Rand von Leon hinter der Trennwand |
| d5020_00 (Treppenraum) | (-12033,-18725) | 3139 | 160 -> 295 | 128 der 135 neuen Punkte HINTER dem Koerper (Leon z 8374..8719 > Eintritt 6976..7303), 6 in der Saeule darueber; sichtbar: 0 Punkte geaendert (Bild `NB2_k2_d5020_00_g0_0.png`, magenta = Leons Rueckseite neben dem Treppenraum) |
| f30A0_00k / f30A1_00 (R37-FREI-Stellen, "Haar") | (-25400,13550) / (-24284,13657) | 1102 / 1361 | 0 -> 0 / 0 -> 0 | frei |
| h5020_06 HINTER (X-Kiste) | (-4333,-15925) | 1626 | 25 -> 170 | verdeckt |
| h30A0_00 / h30A0_01 HINTER (Trennwand) | (-21843,15863) / (-19443,15363) | 2010 / 1983 | 42 -> 757 / 85 -> 935 | **nur unten verdeckt** - berichtigt in Nachbesserung 3 (Abnahme 2 Mangel 1): oberhalb der Mittelleiste blieb die Figur sichtbar (Halbfigur), die oberen Felder trugen die Tiefe der Rueckwand |
| h5020_00 HINTER (Treppenraum) | (-13633,-21625) | 1121 | 0 -> 944 | verdeckt |
| h5090_13 HINTER x 2 | (24417,-20365) / (24417,-26365) | 652 / 906 | 0 -> 129 / 0 -> 906 | verdeckt |
(Die Lok-Szenen 6000 stehen im Abschnitt (a). h5090_11: Leon dort 0 neu - die Kiste vorn links verdeckt Teile eines anderen Akteurs
(Slot 50) dahinter.) Bilder `M_bilder/NB2/NB2_k2_*.png` (Stand k2; die gezeigten Objekte sind in k3 unveraendert). Die Python-Szenenwahl
"DAVOR" nimmt den Figurkasten +-350; Leons echte Silhouette ist breiter (OFFEN 7) - deshalb zeigen d30A0_00/d5020_00 Randpunkte, und
jeder davon liegt nachgerechnet hinter dem Koerper.

## Nachbesserung 3 (2026-10-25, nach Abnahme 2 = analysis/befunde_runde41/M_abnahme_2.md)

Grundlage: Abnahme 2 (Urteil "nicht bestanden": Punkt 1 teilweise, Punkt 2 erfuellt). Mangel 1 = ROOM30A0/30A1 Cut 0 (Cut 1
gleiches Muster): die oberen Trennwandfelder (C0 R4/R5/R19, C1 R4) tragen seit NB1 die Tiefe einer Zelle HINTER der Trennwand -
Figur hinter der Trennwand als Halbfigur; dazu fehlt ein Riegel gegen zu FERNE Tiefe, Verdachtsliste `m41abn2/scan_fern_alle.txt`
(127 Modell-Rechtecke). Mangel 2 = Punkt 1 weiter nicht geschlossen (1022 Rechtecke aus, 273 961 Punkte; Restliste; OFFEN 9
"Atlaspunkte ausserhalb des Koerpers durchsichtig / Rechteck teilen" nicht gegangen). Keine Messung der Abnahme wird wiederholt,
ihre Zahlen werden zitiert. Punkt 2 (erfuellt) wird nicht angefasst.

### Stand Nachbesserung 3
- [x] Lektuere (AUFTRAG inkl. Nachtrag 11, VERTRAG, Abnahme 2, Dossier NB1/NB2, git log master..HEAD = 99f85a92)
- [x] Mangel 1: Ursache nachgerechnet (obere Felder auf Rueckwand-/Nordwand-Zelle, dazu Rechteck-Tiefe und Typ-2-Form), Zuordnung
      Zelle 6, Teilrechtecke je Koerper, exe-Messung an den Abnahme-Stellen (0 sichtbar), Riegel unit/integration_r41_masken36_fern
- [x] Mangel 1: Verdachtsliste scan_fern_alle.txt (127 Eintraege, 41 Gruppen) gegen den Endstand gesichtet (Werkzeug verdacht)
- [x] Mangel 2: OFFEN 9 gegangen (Teilrechtecke, Atlaspunkte ausserhalb des Koerpers durchsichtig + Rest-Stueck), Restliste: 7 weitere
      Gegenstaende von Hand zugeordnet und gezeichnet; was bleibt, hat keine Zelle / die Kamera steht darueber / liegt nicht in der Region
- [x] Bau, gezielte Tests (unit 32/32; Integration 27 Riegel: 26 gruen, zellen3d rot an drei NB2-Szenen 6000 C5 -> gemessen HINTER dem
      Wandblock, Szenen berichtigt, Lauf 2 s. Tests NB3), DAVOR-Stellen der Abnahme 2 an der NB3-exe nachgemessen, Abschluss-Commit

### NB3 Zusammenfassung je Mangel (Ursache, Messung vorher, Beleg, Aenderung, Messung nachher)
**Mangel 1 - 30A0/30A1 obere Trennwandfelder, Figur hinter der Trennwand als Halbfigur; Riegel gegen zu ferne Tiefe; Verdachtsliste.**
* Ursache: (a) die oberen Felder (Objekte (232,-24), (59,43); C1 (-32,70)) liegen auf dem unteren Feld auf - ihre zurueckprojizierten
  "Bodenkontakte" fielen hinter die Trennwand, die Automatik nahm Zelle 8 @0x00474 / 0 @0x00414 / 1 @0x00420 (Tiefe 188..207); (b) auch mit
  richtiger Zelle hatte jedes Rechteck EINE Tiefe = max bucket ueber alle seine Punkte - hinter dem nahen Teil einer zurueckweichenden Wand
  blieb die Figur sichtbar (unten R0: 136 von 854 Leon-Punkten bei s30A0_00); (c) das Python-Modell kannte Typ 2 nur als Kasten.
* Messung vorher: Abnahme 2 (s30A0_00 1072, u30A0_00 986, t30A0_00 858, e30A1_00 704, h30A0_01 988 ueber dem oberen Feld sichtbar); neues
  Mass "zu fern" 1 360 606 Figurpunkte (Stage 3: 56 346).
* Beleg: Zelle 6 @RDT+0x0045C `96 19 e2 04 63 98 2d 37 01 ff 00 03`, Hoehe an allen Trennwand-Objekten gemessen 3025 (C0, IoU 0,908) /
  3225 (C1, IoU 0,930); Verdeckt-Urteil des Originals depth < (1023*vz)>>16 (@0x8002565c `sra v1,v1,4`, ZSF3 341 @0x80066c70, Masken-OT
  = depth @0x80039650); Raute Typ 2 = push_diag2 LAB_8003d00c (quadrant_of, cx = x + (breite >> 1)); Spielerradius 450 DAT_80073e94[6].
  Die Masken selbst sind PORT-WAHL (Original-PRI dieser Cuts ff ff ff ff, @0x80039324/@0x80039338).
* Aenderung: zuordnung_r41.json (+5 Trennwand-Eintraege), koerper_r41.py (Eintritt/Austritt mit der Resolver-Form), kacheln_r41.py
  (Teilrechtecke je Koerper, Tiefe je Teil = max bucket(Eintritt) seiner Koerperpunkte - nie eine Figur VOR dem Koerper verdeckt; Grenzen
  nach den erreichbaren Figurpunkten hinter dem Koerper), Ausschluss neu, Riegel unit_r41_masken36_fern (Stage 3 == 0) und
  integration_r41_masken36_fern (Art Z: V == 0), Werkzeug verdacht.
* Messung nachher: an allen Abnahme-Stellen 0 Leon-/Elza-Punkte hinter der Trennwand sichtbar (Haar ueber der Oberkante ausgenommen), DAVOR
  0 verdeckt; zu fern 40 276 (Stage 3: 0; der Rest zu 99 % ROOM6000 Gleisbett Band 0); Verdachtsliste: 27 von 41 Gruppen ohne Wirkung, ein
  echter Fehler gefunden und behoben (6000 C5 Traeger), 12 Gruppen OFFEN 13. An den 12 eigenen DAVOR-Stellen der Abnahme 2 nichts
  verschlechtert (Tabelle "DAVOR-Stellen der Abnahme 2 an der NB3-exe"). Der Riegel integration_r41_masken36_zellen3d meldete drei NB2-Szenen
  6000 C5 rot - gemessen: sie stehen HINTER der Ecke des Wandblocks Zelle 12 @0x007EC (1574 von 1613 Leon-Punkten hinter dem Prisma), die
  NB2-Erwartung "DAVOR" kam aus ihrem falschen Traeger-Modell; Szenen auf Art M berichtigt, DAVOR Cut 5 an der Abnahme-Stelle (Abschnitt
  "Riegel zellen3d").
**Mangel 2 - Punkt 1 weiter nicht geschlossen (Restliste, OFFEN 9).**
* Ursache: echte Gegenstaende ohne automatisches Modell (Spinde, Kisten, Traeger, Container); OFFEN 9: Rechtecke mit Modell, deren Punkte
  nicht alle hinter dem Koerper liegen, fielen ganz weg.
* Messung vorher: 1022 Rechtecke aus, 273 961 Punkte nur in ausgeschlossenen Rechtecken, 956 982 gezeichnet.
* Beleg: je Zuordnung Zellbytes + RDT-Offset + gemessene Hoehe/IoU + Bild (zuordnung_r41.json, Riegel zellen3d prueft 1025 Zeilen byte-genau).
* Aenderung: 7 Gegenstaende von Hand zugeordnet; OFFEN 9 als "Atlaspunkte ausserhalb des Koerpers durchsichtig" + Rest-Stueck; tote
  Rechtecke zusammengelegt.
* Messung nachher: 983 624 gezeichnet (+26 642, 0 weggefallen), 247 319 nur ausgeschlossen; Restliste-Bilder M_bilder/NB3_rest; was bleibt,
  ist begruendet (Kamera ueber der Zelle, keine Zelle, nicht in der Region, Waende/Boeden) - Nutzerfrage fuer die Gelaender.

### NB3 Messung vorher: Mass "zu fern" (neu, test_r37_masken36 fern)
Die Abnahme 2 verlangt einen Riegel gegen zu FERNE Tiefe. Neuer Modus `test_r37_masken36 fern` (dieselben erreichbaren Stehplaetze,
dasselbe pixelgenaue Figurraster und dieselbe Engine-Tabelle wie der Ausschluss): ein Figurpunkt F zaehlt als "zu fern", wenn an
seinem Bildpunkt eine GEZEICHNETE Maske mit Zellen-Modell liegt, deren Koerper die Strecke Kamera -> F VOR F schneidet
(r41_koerper_davor == 1), F aber von keiner gezeichneten Maske verdeckt wird (re15_pri_mask_occludes: depth < bucket(vz),
@0x8002565c otz>>4 / ZSF3 341 @0x80066c70 / Masken-OT = depth @0x80039650). Stand vor NB3 (Spurbaum 99f85a92, Leon + Elza):
**1 360 606 von 13 839 974 Figurpunkten hinter einem Modell-Koerper sind sichtbar** (Stage 3: 56 346, 4: 961 232, 5: 914,
6: 342 114), in 13 Raumpaar-Cuts: 30A0 C0 (R0 27 169 / R22 1 004 - die unteren Trennwandfelder; die oberen R4/R5/R19 zaehlen hier
noch gar nicht, weil ihr Modell die Rueckwand-Zelle ist), 4040 C6/C7/C9/C10/C11, 4080 C7, 5090 C11/C13, 6000 C5/C7/C8/C9.
Ursache 1 (Mangel 1 der Abnahme): falsche Zelle (oberes Feld -> Rueckwand). Ursache 2 (dieselbe Wirkung, systematisch): EINE Tiefe
je Rechteck = max bucket ueber alle Punkte des Rechtecks (NB1/NB2-Regel) - bei einem lang zurueckweichenden Gegenstand (Trennwand
30A0: Eintritt bucket 72..110 im rechten Feld, 104..171 im linken) bleibt eine Figur hinter dem NAHEN Teil sichtbar (Python-Simulation
auf Leons Dreiecken der Abnahme-Laeufe: s30A0_00 R0 136 von 854 Punkten, R4 mit Zelle 6 und Rechteck-Tiefe 107 noch 306 von 1075;
je Bildspalte eigene Tiefe: 0). Ursache 3 (gefunden): das Python-Modell (geom.zellen_solid_test) kennt Zellentyp 2 nicht ("typ 2/8/9
... None" = Huellrechteck), der C-Ausschluss fragt die Raute des Resolvers (push_diag2, LAB_8003d00c, re15_collision.c) - 23
Modell-Zeilen in 4040/4080 haben Tiefe und Hoehe vom Huellrechteck (13 289 x 13 289) statt von der Raute.

### NB3 Plan
1. Zuordnung von Hand: obere Trennwandfelder 30A0 C0 (Verschiebungen (232,-24), (59,43)) + Fussstreifen (57,86), C1 oberes Feld
   (-32,70) + Fussstreifen (0,126) -> Zelle 6 @0x0045C; Hoehe gemessen an ALLEN Trennwand-Objekten (C0: 3025, IoU 0,908 statt
   1450/0,753 nur unten).
2. Koerper-Modell mit der Form des Resolvers auch fuer Typ 2 (Raute, push_diag2), Eintritt UND Austritt je Bildpunkt.
3. Streifen (Weg "Rechteck teilen" aus OFFEN 9): jedes Rechteck eines Koerper-Objekts wird in senkrechte Teilrechtecke derselben
   Atlas-Abbildung zerlegt, Tiefe je Teil = max bucket(Eintritt) seiner Koerperpunkte (keine Figur VOR dem Koerper verdeckt) und die
   Grenzen so, dass hinter dem Koerper (Austritt) moeglichst nichts frei bleibt; Saum-Rechtecke des Objekts werden je Streifen
   zusammengelegt (Platz fuer die Teile, 105 je Cut, re15_pri.h). Simulation Trennwand 30A0 C0 mit 6 Streifen: an s/u/t/h30A0_00
   0/0/0/1 von 1937/1439/1101/1691 Leon-Punkten hinter dem Koerper sichtbar, DAVOR d30A0_00 0 von 2144 verdeckt.
4. Riegel: unit_r41_masken36_fern (Mass oben, Grenze = gemessener Endstand), integration_r41_masken36_fern (exe, Art Z: hinter der
   Trennwand im Fenster des Gegenstands kein Figurpunkt sichtbar; DAVOR 0 uebermalt), Gegenprobe gegen den Stand vor NB3.
5. OFFEN 9 mit derselben Zerlegung (Teile nur aus Koerperpunkten zeichnen), Restliste neu sichten.

### NB3 Mangel 1 - Ursache und Beleg (obere Trennwandfelder 30A0/30A1 Cut 0/1)
* **Ursache (an den Daten nachgerechnet, `m41nb3/py/rinfo.py`, `p30a0.py`):** Die Trennwand besteht in der Generator-Region aus
  FUENF Objekten (C0: Verschiebungen (144,24) rechtes Feld unten, (-5,91) linkes Feld unten, (232,-24) rechtes Feld oben = R4..R9 +
  Saum R62..R74, (59,43) linkes Feld oben = R19..R21 + R51..R57, (57,86) Fussstreifen = R38..R45 + R93..R100). NB2 hat nur die
  beiden unteren Felder von Hand Zelle 6 zugeordnet; die oberen Felder liegen auf dem unteren Feld auf (kein Bodenkontakt) - ihre
  "Bodenkontakte" (Unterkante zurueck auf y = 0 projiziert) fielen hinter die Trennwand und die Automatik nahm Zelle 8 @0x00474
  (Rueckwand-Schraenke, R4/R5 Tiefe 200/199) bzw. Zelle 0 @0x00414 (R19/R20/R21/R51 Tiefe 188..201, IoU 0,16). In C1 ebenso
  (-32,70) = R4 -> Zelle 1 @0x00420 (Nordwand, Tiefe 207).
* **Beleg Zelle 6 (Messung, Bilder `M_bilder/NB3/NB3_zuordnung_30A0_00.png` / `_01.png`):** Koerper SCA-Zelle 6 @RDT+0x0045C =
  `96 19 e2 04 63 98 2d 37 01 ff 00 03` (x -26525..-19975 z 14125..15375, Typ 1, Band 0) - Hoehe an ALLEN Trennwand-Objekten des
  Cuts gemessen (IoU-Maximum, koerper_r41.hoehe_messen): **C0 3025, IoU 0,908, 98 % der Objektpunkte im Koerper** (nur die
  NB2-Teile: 1525, IoU 0,586); **C1 3225, IoU 0,930, 100 %** (nur NB2-Teil: 1925, IoU 0,575). Kamera C0 (-16896,-3600,9440) und
  C1 (-24744,-3300,9840) stehen nicht ueber der Zelle (z < 14125) - keine Nahfeld-Sperre. Eintraege in `zuordnung_r41.json`
  (5 neue, je mit Zellbytes, Messung, Bild).
* **Zweite Ursache derselben Wirkung (gemessen):** auch MIT richtiger Zelle bleibt die Figur hinter dem NAHEN Teil der Trennwand
  sichtbar, wenn ein Rechteck nur EINE Tiefe hat: NB1/NB2 setzen sie auf max bucket(Eintritt) ueber das ganze Rechteck (keine Figur
  VOR dem Koerper verdeckt). Eintritt im rechten oberen Feld R4 bucket 72..107, linkes oberes Feld R19 104..171, unteres R0 75..110.
  Python-Simulation auf Leons Dreiecken der Abnahme-Laeufe (`m41nb3/py/sim30a0.py`): s30A0_00 R4 mit Zelle 6 und Rechteck-Tiefe
  107: 306 von 1 075 Leon-Punkten weiter sichtbar, R0 (Tiefe 110) 136 von 854, t30A0_00 R19 200 von 200; mit Tiefe je Bildspalte:
  0. Die Abnahme-Zahl "unten verdeckt 644" war also auch unten nicht vollstaendig.
* **Dritte Ursache (gefunden beim Bau des Masses):** das Python-Koerpermodell (geom.zellen_solid_test, von NB1/NB2 benutzt) kennt
  Zellentyp 2 nicht ("typ 2/8/9 gibt ebenfalls None (Form hier nicht modelliert)" -> Huellrechteck), der C-Ausschluss fragt die
  RAUTE des Resolvers (form_drin -> push_diag2 = LAB_8003d00c, ghidra1_V2.txt:145450-145916: Quadrant aus quadrant_of mit
  cx = x + (breite >> 1), cz = z + (tiefe >> 1), je Quadrant die Kante zwischen den Kantenmitten). 23 Modell-Zeilen in 4040/4080
  hatten Tiefe und Hoehe vom 13 289 x 13 289-Kasten (z.B. 4040 C6 R4: Kasten 244, Raute 213; 4040 C11 R93: Kasten 151, Raute 190).
  Dazu: geometrie.quader_auf_band hat keine BODENflaeche - ein Koerper ueber der Kamera (3050 C5 R40, Band 4, Kamera y -1781)
  bekam die Tiefe der Austrittsflaeche (282 statt 259).

### NB3 Umsetzung
| Datei | Inhalt |
|---|---|
| `re15_port/tools/maske/koerper_r41.py` (neu) | Koerper einer SCA-Zelle als Prisma, Eintritt UND Austritt je Bildpunkt (Cyrus-Beck mit den Halbebenen der Form + Boden- und Deckelebene; Kreis quadratisch); Formen wie der Resolver: Typ 1 Kasten, Typ 2 Raute (push_diag2), Typ 4..7 Schraege (Belege geom.zellen_solid_test), Typ 3 Kreis; Hoehe per IoU wie geometrie.hoehe_messen. Gegenprobe gegen geometrie.quader_auf_band an allen 498 Typ-1-Zeilen: 480 bitgleich, die 18 anderen sind Saeulen (kollisionstiefe_schnell rechnet in 64er-Stufen) bzw. Koerper ueber der Kamera (Bodenflaeche fehlte dort). |
| `re15_port/tools/maske/kacheln_r41.py` (neu) | Teilrechtecke je Koerper (Regel im Kopf): alte Rechtecke an Spaltengrenzen geteilt (gleiche Atlas-Abbildung, gleiche Gruppe - Texel und Lage unveraendert), Tiefe je Teil = max bucket(Eintritt) seiner Koerperpunkte; Saum-Rechtecke des Koerpers je Streifen zusammengelegt (eigener Atlasplatz, Koerperpunkte und Punkte ausserhalb getrennt) - schafft den Platz fuer die Teile in vollen Cuts (105); vorher ausgeschlossene Rechtecke (OFFEN 9) zusaetzlich in Zeilen nur mit Koerperpunkten und Rest-Zeilen geteilt. Spaltengrenzen per dynamischer Programmierung: Ziel = wenigste erreichbare Figurpunkte hinter dem Koerper, deren Stufe bucket(vz - 450) <= Tiefe des Teils ist (450 = Spielerradius DAT_80073e94[6]: so weit reicht die Figur hoechstens naeher an die Kamera als ihr Mittelpunkt), Daten aus `test_r37_masken36 hinter`; K je Koerper gierig nach dem groessten Gewinn je zusaetzlichem Rechteck. Auswahl: Zuordnungen von Hand, Koerper mit "zu fern"-Punkten, Koerper mit ausgeschlossenen Rechtecken. Rueckholliste: wird ein vorher GEZEICHNETES Rechteck nach der Teilung vom Ausschluss genommen, bleibt es ungeteilt mit alter Tiefe (Kette bis stabil). Modell-Zeilen unveraenderter Rechtecke wandern mit dem neuen Index mit. |
| `re15_port/tests/unit/test_r37_masken36.c` | Modus `fern` (Mass + Riegel), Werkzeug `hinter` (Figurpunkte hinter den Koerpern je Spalte, fuer kacheln_r41.py) |
| `re15_port/tools/maske/zuordnung_r41.json` | +15 Eintraege (30A0 C0/C1 Trennwand oben + Fussstreifen; Restliste: 30A0 C3 Spinde, 3040 C0 Schaltkasten/Kiste, 5090 C13 obere Kiste, 5020 C7 Container), je mit Zellbytes, Messung, Bild `M_bilder/NB3/NB3_zuordnung_*.png` |
| `re15_port/tests/unit/probe_r41_masken36_sicht.c`, `tests/integration/test_r41_masken36_stage2.cmake` | Art Z im exe-Laeufer: dritter Lauf A0 (Masken an, Figur ausserhalb des Regionsvierecks = Masken ohne Figur), V = Figurpunkte mit A != A0 = im Endbild sichtbar |

### NB3 Zwischenstand Kette k1 (nur Austritts-Kriterium, ohne Figurpunkt-DP und ohne die Restliste-Zuordnungen)
`m41nb3/kette3.sh k1`: 2 Runden bis stabil (Rueckholliste 4040 C9 R7, C10 R22..R25), Tabelle 682 -> 663 Zeilen (nur gezeichnete),
Ausschluss mit gefilterter Tabelle bitgleich. Gezeichnete Punkte der bearbeiteten Cuts 224 716 -> 225 041 (0 verloren).
**zu fern 1 360 606 -> 353 986** (Stage 3: 56 346 -> **0**, 4: 961 232 -> 72, 5: 914 -> 136, 6: 342 114 -> 353 778). Der Rest in
ROOM6000 liegt an duennen Koerpern (Zelle @0x00810 Hoehe 25/650, @0x007F8 mit der NB2-IoU-Hoehe 150): der Austritt folgt dort
direkt auf den Eintritt, das Austritts-Kriterium kann die Spalten nicht unterscheiden - die Figuren dahinter stehen im Gleisbett
(Band 0, OFFEN 10). Deshalb die Figurpunkt-DP (Kette k2).

### NB3 Endstand Kette k6 (Messung nachher)
`m41nb3/kette4.sh k6` (Runde 0 Austritts-Kriterium -> `test_r37_masken36 hinter` -> Runden mit Figurpunkt-DP -> Rueckholliste bis stabil
(2 Runden) -> Modell-Tabelle auf gezeichnete Rechtecke (1050 -> 1025 Zeilen) -> Ausschluss mit gefilterter Tabelle bitgleich).
| Mass | vor NB3 (99f85a92) | nach NB3 |
|---|---|---|
| zu fern (`test_r37_masken36 fern`, Leon + Elza) | 1 360 606 | **40 276** (Stage 3: 56 346 -> **0**, 4: 961 232 -> 72, 5: 914 -> 270, 6: 342 114 -> 39 934) |
| gezeichnete Generator-Punkte Leon Stage 3..6 (Zaehler wie stat_nb2 der Abnahme, `m41nb3/py/stat.py`) | 956 982 | **983 624** (+26 642) |
| Punkte nur in ausgeschlossenen Rechtecken | 273 961 | **247 319** |
| geaenderte Cuts (Bogen `M_bilder/NB3/NB3_bogen_*.png`, Zahlen `NB3_zahlen.txt`) | - | 25 Cuts: 306 325 -> 332 967 Punkte, **0 Punkte weggefallen** |
| offene Kerben (unit_r41_masken36_loecher) | 10 178 von 63 546 (16,0 %) | 9 036 von 63 116 (**14,3 %**) |
| Modell-Zeilen (belegt, MSK-Tiefe == Zeile, gezeichnet) | 530 | 1025 (je 12 SCA-Bytes mit RDT-Offset, Riegel zellen3d) |
Der Rest "zu fern" liegt zu 99 % in ROOM6000 auf **Band 0** (Gleisbett unter dem Bahnsteig: C5 17 331, C7 2 636 Punkte je Variante,
`test_r37_masken36 fern -1 6000 5` druckt die Plaetze) - OFFEN 10; dazu 4080 C7 zwei Plaetze an der Zellkante (36) und 5090 C11 sechs
Plaetze am Rand der Dreieckszelle 30 (135, der Randfall der Abnahme-Hinweise d5090_11).
**exe (Spurbaum-exe md5 597ba53a..., Bild 120, RE15_TRILOG, `m41nb3/py/ana.py`: Leon = vorderstes Dreieck, Maskenschluessel (depth+1)*65536/1023
wie der Zeichner; "sichtbar hinter Koerper" = Leon-Punkt, an dem der Koerper einer gezeichneten Modell-Maske VOR ihm beginnt, ohne dass eine
Maske ihn verdeckt):**
| Szene (Stellen der Abnahme 2) | Platz | Leon unter Masken | verdeckt | hinter Koerper sichtbar | Abnahme 2 (vorher) |
|---|---|---|---|---|---|
| s30A0_00 Cut 0 | (-21743,16263) | 1944 | 1943 | **0** | 1072 ueber dem oberen Feld |
| u30A0_00 Cut 0 | (-23000,16300) | 1475 | 1475 | **0** | 986 |
| t30A0_00 Cut 0 | (-24300,16400) | 1131 | 1131 | **0** | 858 |
| h30A0_00 Cut 0 | (-22343,16063) | 1719 | 1717 | **0** | - |
| e30A1_00 Elza Cut 0 | (-21743,16263) | 1382 | 1382 | **0** | 704 |
| h30A0_01 Cut 1 (erzwungen) | (-19143,15863) | 1733 | 1733 | **0** | 988 |
| d30A0_00 DAVOR | (-21943,13663) | 2155 | **0** | (2 Randpunkte) | 0 |
| d30A0_01 DAVOR, Cut 1 wie die Abnahme 2 | (-21343,13663) | 4282 | **0** | (43: Leons Arm 20..60 hinter dem Eintritt, im Zellkoerper, OFFEN 12) | 0 |
| f30A0_00 / f30A1_00 (R37-FREI "Haar") | (-25400,13550) / (-24284,13657) | 503 / 843 | **0 / 0** | 0 | 0 |
Ueber der Trennwand bleibt nur der Haarschopf sichtbar (s30A0_00 36 Punkte oberhalb der Oberkante, "ausser Kopf"); Bild
`M_bilder/NB3/NB3_M1_30A0_trennwand.png` (rot verdeckt, blau sichtbar, magenta sichtbar obwohl hinter dem Koerper = 0). "uebermalt" (|A-B| > 24)
DAVOR 1 / 4 Punkte = dieselben Rasterkanten wie in der Abnahme 2. Eigene Zusatzstelle l30A0_00 (-24843,15863), 488 hinter der Trennwand: 12 von
1050 Punkten sichtbar, alle in EINER Bildspalte (x 208) mit Leon-z zwischen Eintritt und Austritt des Koerpers - Leons Modell ragt dort in den
Zellen-Koerper (OFFEN 12).

### NB3 Riegel zellen3d - die 6000-C5-"DAVOR"-Szenen der NB2 stehen HINTER dem Wandblock (Messung, Ursache, Beleg, Aenderung)
**Messung:** der Integrationslauf (Tests NB3, Lauf 1) meldet `integration_r41_masken36_zellen3d` rot an drei NB2-Szenen: d6000_05 (15433,-4184)
Band 2 550 von 1561 Figurpunkten uebermalt, d6000_05t (14933,-4484) 477 von 1643, e6001_05 (Elza) 265 von 1241 - die NB2 hatte sie als DAVOR
(H == 0) gesetzt. Uebermalt wird Leon von den NB3-Teilen des Gittertraegers R22 (Tiefe 63, 348 Punkte) und R27 (Tiefe 55, 202); Leon hat dort
Bucket 110..124 (`m41nb3/py/ueberm.py`).
**Ursache:** die Plaetze liegen im Gang HINTER der Ecke des Wandblocks, an dem der Traeger sitzt. Die NB2 hatte sie als DAVOR gewaehlt
("staerkste Ueberdeckung ohne neu verdeckten Punkt" in IHREM Modell), und ihr Modell des Traegers war falsch: Quelle-R23 Zelle @0x007D4
(x 19313..22275, IoU 0,08, Tiefe 187 = hinter Leon), R33/R35 Zelle 13 @0x007F8 (NB2-IoU-Hoehe 150, Tiefe 176/173), R13/R14/R19 ausgeschlossen.
NB3 traegt den Traeger mit Zelle 12 (Zuordnung von Hand) - damit verdeckt er Leon hinter der Ecke, so wie es die Geometrie verlangt.
**Beleg (Bild `M_bilder/NB3/NB3_6000_05_ecke.png`):**
* Zelle 12 @RDT+0x007EC = `d8 4f c8 0f 4a e1 d6 e9 01 ff 00 23`: x -7862..12578, z -5674..-1634, Typ 1, Band 2 (Boden y = -2 * 0x708 = -3600).
* Wandblock bis unter die Decke: in Cut 2 (Kamera (13193,-5104,2221), Blick -z) ist die linke Kante der braunen Wandflaeche die Senkrechte
  (12578,-5674) vom Boden (230,189) bis zum oberen Bildrand (223,4) = 7000 ueber dem Boden (4..5 Bildpunkte Abstand: die Zelle ist etwas
  groesser als die Kunst); die Ecke (12578,-1634) laeuft von (252,238) bis (241,11) = 4000.
* Leon steht im Gang zwischen Zelle 12 und Zelle 13 @RDT+0x007F8 (x 15903..20063 z -5734..4175, der Wandblock jenseits des Gangs). Der
  Sehstrahl Kamera C5 (8794,-9403,-7278) -> Leons Fusspunkt kreuzt den Grundriss der Zelle 12 bei x 12235..12578 / z -5674..-5514, 2500..3700
  ueber dem Boden.
* Leon-Dreiecke der NB3-exe (RE15_TRILOG, `m41nb3/py/blockhinter.py`): **1574 von 1613 Leon-Punkten liegen hinter dem Prisma** der Zelle 12
  (Saeule); mit endlicher Hoehe 4100: 1487, 3700: 1062, 3000: 345, 2500: 60, 1800: 0.
* Fuer die Traeger-Masken (gezeichnete Rechtecke mit Zelle 12) gibt es auf Band 2 **keinen** Platz VOR dem Block: 0 von 3302 sichtbaren
  Band-2-Plaetzen (`test_r37_masken36 plaetze 6000 5`, Figur 3 x 18 Punkte, `m41nb3/py/davor6000.py`), 602 Plaetze mit Figurpunkten dahinter.
**Aenderung (nur die Szenendatei `tests/integration/test_r41_masken36_zellen3d_szenen.cmake`, Beleg im Kopf der Datei):** d6000_05 /
d6000_05t / e6001_05 -> m6000_05 / m6000_05t / m6001_05, Art M (Figur HINTER dem Gegenstand, die Maske wirkt: H > 0); neu d6000_05a = DAVOR an
der eigenen Stelle der Abnahme 2 (12233,-9984) Band 2 (Leon 2500, unter Masken 1547, uebermalt 0); Fenster = Leon-Dreiecke (x 203..262
y 132..233) + 4, oben ohne Rand - in y 128..131 liegt die Kante von Slot 5 (z 16535; das 3D-Tor Obj_model_set[1] (20916,0,-18540) hat in Cut 5
vz ~17 100, nach Abnahme 2 hinter der Lokfront), die die Lokmaske dort verdeckt (Lauf 2: mit Rand 3 von 2704 uebermalt, alle drei bei
(219..220,128..129), Leon selbst 0). Masken und Tabellen unveraendert.
**Messung nachher:** zellen3d gruen (Tests NB3, Lauf 3: m6000_05 550 von 1561, m6000_05t 477 von 1643, m6001_05 265 von 1241 verdeckt,
d6000_05a 0 von 2648 uebermalt; die uebrigen 11 Szenen wie Lauf 1). Leon hinter der Ecke ist jetzt teilweise verdeckt (536 der 1574 Punkte hinter dem
Block; NB2: 0). Offen bleibt der Rumpf unter R37 (Quelle-R33, Tiefe 176, NB2-Zeile Zelle 13): seine Zelle-12-Teile nimmt der Ausschluss wegen
Band-0-Plaetzen wieder heraus (Rueckholliste, OFFEN 14).

### NB3 DAVOR-Stellen der Abnahme 2 an der NB3-exe (nicht verschlechtert)
Dieselben Plaetze und Laeufer-Parameter wie `M_abnahme_2.md` 1b (run.sh: RE15_DEBUG_JUMP + RE15_PLAYER_POS + RE15_FORCE_CUT, Bild 120;
`m41nb3/abn_davor.sh`, Auswertung `m41nb3/py/ana.py`, NB3-exe md5 597ba53a...); die Leon-Zahl ist je Stelle gleich der der Abnahme (fuer Elza nennt die Abnahme nur 0 / 0).
| Szene | Platz (Band) | Cut | Leon | verdeckt Abnahme 2 -> NB3 | uebermalt Abnahme 2 -> NB3 |
|---|---|---|---|---|---|
| d30A0_00 | (-21943,13663) B0 | 0 | 2363 | 0 -> **0** | 1 -> 1 (Rasterkante) |
| d30A0_01 | (-21343,13663) B0 | 1 | 6521 | 0 -> **0** | 4 -> 4 (Rasterkante) |
| d5020_00 | (-11933,-18925) B0 | 0 | 1929 | 0 -> **0** | 0 -> 0 |
| d5020_06 | (-1533,-14525) B0 | 6 | 3153 | 0 -> **0** | 0 -> 0 |
| d5090_11 | (3617,-25365) B0 | 11 | 5013 | 115 -> **110** | 110 -> 104 (Randfall Dreieckszelle 30 @0x009E0, Hinweis der Abnahme) |
| d5090_13 | (20817,-25965) B0 | 13 | 1294 | 0 -> **0** | 1 -> 1 |
| d6000_05 | (12233,-9984) B2 | 5 | 2500 | 0 -> **0** | 0 -> 0 |
| d6000_07 | (18233,-21184) B2 | 7 | 1900 | 0 -> **0** | 0 -> 0 |
| d6000_08 | (12833,-18784) B2 | 8 | 1583 | 0 -> **0** | 0 -> 0 |
| d6000_09 | (8033,-18784) B2 | 9 | 755 | 0 -> **0** | 3 -> 3 |
| d3040_00 | (-24400,-4800) B0 | 0 | 7088 | 0 -> **0** | 0 -> 0 |
| e5021_06 (Elza) | (-1533,-14525) B0 | 6 | 1994 | 0 -> **0** | 0 -> 0 |
An d5090_11 liegen 103 Punkte von Leons Unterschenkeln hinter dem niedrigen Koerper der Zelle 30 (R1, Hoehe 625, Tiefe 69; Leon-z 4029..4178,
Eintritt 3356..3973) und bleiben sichtbar - das ist der Rest "zu fern" 5090 C11 (Tabelle oben, Randfall der Dreieckszelle).

### NB3 Mangel 1 - Sichtung der Verdachtsliste der Abnahme 2 (`m41abn2/scan_fern_alle.txt`, 127 Eintraege)
Die Abnahme hat im GRUNDRISS gesucht (>= 50 % der Sehstrahlen eines Modell-Rechtecks betreten vorher eine andere solide Zelle, Huellquader
2400 hoch). Ob das im Spiel wirkt, entscheidet, ob dort eine Figur steht. Werkzeug `test_r37_masken36 verdacht` (Endstand): je gezeichnetem
Modell-Rechteck die erreichbaren Figurpunkte an seinen Bildpunkten, die (i) von keiner Maske verdeckt sind, (ii) NICHT hinter dem eigenen
Koerper liegen und (iii) deren Grundriss-Strecke eine solide Zelle des Bands kreuzt - nur dort kann eine zu ferne Tiefe eine Figur zeigen,
die ein Gegenstand davor verdecken muesste. `verdacht <raum> <cut> <rechteck>` druckt die Stehplaetze. Ergebnis je Gruppe der Abnahme
(Cut, Modellzelle, Zelle davor; Spalten: Abnahme-Rechtecke | Endstand-Rechtecke mit dieser Modellzelle | Verdachtspunkte):
**27 von 41 Gruppen 0 Punkte** (darunter 30A0 C0 R4/R19 und C1 R4: Modellzelle 0/8/1 gibt es nicht mehr, die Felder tragen Zelle 6;
3040 C9, 30A0 C4, 4040 C10/C12, 5020 C11, 5050 C1, 5100 C7, 6010 C6 ...). 14 Gruppen mit Punkten:
| Cut | Modell (Grundriss-Ecke) | Zelle davor | Abnahme | Endstand | Verdachtspunkte |
|---|---|---|---|---|---|
| 3040 C8 | x-21825 z-12075 | @0x0087C | 4 | 7 | 19 |
| 3040 C8 | x-21825 z-12075 | @0x00990 | 3 | 7 | 19 |
| 3050 C1 | x-27225 z-16893 | @0x009D8 | 5 | 9 | 4564 |
| 3050 C1 | x-27225 z-16893 | @0x00C00 | 1 | 9 | 4564 |
| 4040 C6 | x-22741 z-2734 | @0x00818 | 2 | 3 | 8347 |
| 4040 C11 | x-8875 z4500 | @0x00860 | 3 | 3 | 250 |
| 4080 C8 | x1525 z-6775 | @0x00628 | 1 | 1 | 78326 |
| 5020 C6 | x-16433 z-27516 | @0x00870 | 1 | 9 | 7768 |
| 5020 C6 | x-7475 z-19375 | @0x00858 | 3 | 18 | 1906 |
| 5090 C11 | x-5673 z-29750 | @0x009D4 | 6 | 6 | 854 |
| 5090 C11 | x-425 z-27200 | @0x009D4 | 3 | 10 | 3618 |
| 5090 C13 | x21642 z-29765 | @0x009D4 | 1 | 19 | 89377 |
| 5090 C13 | x21642 z-22215 | @0x009C8 | 1 | 3 | 65212 |
| 6000 C5 | x19313 z-25284 | @0x007F8 | 3 | 3 | 5340 |
Davon nachgesehen: **6000 C5 (Gittertraeger)** - echter Fehler, die Mitte des Traegers trug die Zelle 13 @0x007F8 (Wandblock jenseits des Gangs, NB2-IoU-Hoehe 150)
statt der Zelle 12 @0x007EC (Wandblock, an dem der Traeger sitzt); Zuordnung von Hand (Bild `NB3_zuordnung_6000_05.png`), unterer Traeger jetzt gezeichnet (+4 698 Punkte). Die
Teilung der Traeger-MITTE nimmt der Ausschluss wegen Figurpunkten im Gleisbett (Band 0) wieder heraus (Rueckholliste), sie bleibt im
NB2-Stand - der Rest der 5 340 Punkte. **5090 C13 R12 (Kistenstapel rechts, Zelle 26)** - kein Fehler: die Verdachts-Plaetze liegen bei
x 20717..21117 z -26365..-24965 direkt VOR dem Stapel (Zelle 26 beginnt bei x 21642); die "Zelle davor" ist die lange niedrige Kistenreihe
@0x009D4, ueber die der Sehstrahl hinweggeht - die Figur steht vor dem Stapel und gehoert ueber die Maske. Die uebrigen 12 Gruppen sind
nicht einzeln an der exe geprueft (OFFEN 13).

### NB3 Mangel 2 - OFFEN 9 und Restliste
* **OFFEN 9 gegangen ("Atlaspunkte ausserhalb des Koerpers durchsichtig / Rechteck teilen"):** kacheln_r41.py nimmt jetzt auch die Koerper
  der vorher ausgeschlossenen Modell-Rechtecke (Bericht zellen3d_r41.json) auf; fuer Rechtecke mit NEUEM Koerper (vorher ausgeschlossen, ohne
  Modell-Zeile oder mit anderer Zelle) werden die Punkte ausserhalb des Koerpers im eigenen Atlas durchsichtig und kommen in ein Rest-Stueck
  mit der alten Tiefe (nur wo kein anderes Teil den Texel deckend braucht); die Teile zeigen dann nur Koerperpunkte, und das 3D-Urteil traegt
  sie. Dazu: tote Rechtecke (vorher ausgeschlossen, ohne Koerper) je Objekt zusammengelegt - Platz fuer Teile in vollen Cuts (z.B. 30A0 C0:
  die 11 Moebel-Rechtecke vorn, Kamera ueber der Zelle, sind jetzt 1 Rechteck, weiter ausgeschlossen).
* **Restliste der Abnahme 2 (echte Gegenstaende) - jetzt gezeichnet** (Zuordnung von Hand mit Zellbytes, Messung und Bild in
  `zuordnung_r41.json`, Bilder `M_bilder/NB3/NB3_zuordnung_*.png`):
  | Gegenstand | Zelle | Hoehe / IoU | Punkte neu |
  |---|---|---|---|
  | 30A0 C3 Spinde (4 Objekte) | 6 @0x0045C | 3725 / 0,605 | +10 292 |
  | 6000 C5 Gittertraeger (4 Objekte) | 12 @0x007EC (Saeule) | 4100 / 0,804 | +4 698 |
  | 3040 C9 Kiste vorn (OFFEN 9) | Modell aus NB2-Bericht | - | +2 541 |
  | 3040 C0 Schaltkasten oben + Kiste | 8 @0x00870 | 3475 / 0,526 | +2 471 |
  | 5090 C13 Kistenstapel Mitte, obere Kiste | 27 @0x009BC | 3850 / 0,420 | +2 162 |
  | 5020 C7 Container rechts (4 Objekte) | 11 @0x00864 | 3500 / 0,872 | +1 637 |
  | 5020 C12 X-Kiste, unterer Teil | 12 @0x00870 | 1425 / 0,306 | +1 574 |
  Restliste-Bilder des Endstands: `M_bilder/NB3_rest/NB3_rest_ROOMxxxx_Cnn.png` (gruen gezeichnet, rot nur ausgeschlossen).
* **Was bleibt (die Daten tragen keine sichere Tiefe - je Fall gemessen):**
  - Kamera UEBER dem Grundriss (Nahfeld-Sperre der NB2, gemessen an 5050 C2: 381 Gitterpunkte ohne Koerper davor): 5050 C1 Schreibtisch/Pult
    (5 469 + 1 647 + 448), 5050 C2 Pult vorn (3 719 + 2 157), 30A0 C0 Moebel vorn (1 618).
  - kein Koerper in den Daten: 6000 C7 Lok-Unterteil (6 710 + 2 387; die Lok-Zelle 15 @0x00810 liegt auf Band 2, ihr Unterteil reicht ins
    Gleisbett, Band 0 hat dort keine Zelle - Kandidaten `m41nb3/py/kandidaten.py`: beste IoU 0,34, 40 % im Koerper), 50B0 C1/C3 Roboterarm
    (1 182 + 217 / 3 310, haengt ohne Bodenkontakt, keine Zelle), 6000 C5 Restpunkte am Traeger (702 + 2 135, ausserhalb der Zelle 12).
  - nicht in der Generator-Region: die Gelaender 3050 C6, 30E0 C6..C9, 3040 C3 - der Generator hat dort Boden (rot in den Bildern) statt der
    Gelaenderstaebe gefunden; eine Maske braucht hier eine Freistellung wie in STAGE1/2 (Nutzerfrage).
  - der Grossteil der 247 319 Punkte sind Waende, Boeden und Decken (Gruende NB2: keine Zelle mit der Mehrheit der Bodenkontakte) - eine
    Figur steht nie hinter einer Raumwand oder dem eigenen Boden, dort verdeckt eine Maske nie richtig etwas.

## Nachbesserung 4 (2026-10-25, nach Abnahme 3 = analysis/befunde_runde41/M_abnahme_3.md)

Grundlage: Abnahme 3 (Urteil "nicht bestanden": Punkt 1 teilweise, Punkt 2 erfuellt). Mangel 1 = ROOM30A0/30A1 Cut 0: die
Ost-Stirn der Buero-Trennwand (Zelle 6 @RDT+0x0045C, Flaeche x = -19975, z 14125..15375, Bildspalten 282..325) hat keine Maske -
Leon/Elza hinter der Trennwand sind durch die Stirn sichtbar (o1..o4 62..416, Elza o5 272, a30A0_00 634 Punkte). Mangel 2 = Punkt 1
weiter nicht geschlossen (Restliste, 6000 C5 Wandecke OFFEN 14, kein Entscheid). Mangel 3 = der gemeinsame Laeufer
tests/integration/test_r41_masken36_stage2.cmake prueft je Lauf nur EXISTS f.ppm (Boot-Bild 120 vor dem Sprung). Punkt 2 (erfuellt)
wird nicht angefasst; seine Riegel bleiben gruen.

### Stand Nachbesserung 4
- [x] Lektuere (AUFTRAG inkl. Nachtrag 11, VERTRAG, Abnahme 3, Dossier NB3, git log master..HEAD = 1ad2c96e)
- [x] Mangel 3: Laufpruefung im Laeufer (EXIT_AT-Zeile des Zielraums + Framedump davor, 3 Versuche), Gegenprobe-Riegel
- [x] Mangel 1: gemessen - die "Ost-Stirn" ist die Flaeche der Kollisionszelle, nicht der Gegenstand im Bild; der Kuenstler-Ausschnitt der
      Stirnplatte ist ganz gezeichnet (widerlegt mit Messung + Zellbytes), Riegel unit_r41_masken36_stirn + 6 Szenen im Riegel _fern
- [x] Mangel 2: (a) Band-0-Gleisbett 6000 = Fehler der Saaten (Tuer-Band), behoben (OFFEN 10/14); (b) die Gelaender & Co. liegen als
      Kuenstler-Komponenten im Original-Atlas, nur nie platziert -> platzieren_r41.py (437 Komponenten, +160 539 Punkte); (c) Traeger-Mitte
      6000 C5 geteilt (Wandecke); Rest mit Messung (Pulte/Moebel: Kamera ueber der Zelle; Roboterarm: kein Bodenkontakt; 3050 C6 oben)
- [x] Bau (exe md5 46823269...), gezielte Tests, Abschluss-Commit

### NB4 Zusammenfassung je Mangel (Ursache, Messung vorher, Beleg, Aenderung, Messung nachher)
**Mangel 1 - Ost-Stirn der Trennwand 30A0/30A1 Cut 0 "ohne Maske": mit Messung widerlegt.**
* Ursache der Abnahme-Zahl: das Mass nahm die Flaeche der Kollisionszelle 6 (z bis 15375, Rahmenhoehe 3025) als Gegenstand. Die
  Stirnplatte IM BILD endet bei z ~14870 (oben y -2335); dahinter zeigt das vorgerenderte Bild den Boden und die Rueckwand-Schraenke.
* Messung vorher = nachher (keine Masken-Aenderung an dieser Stelle noetig): 0 von 2003 "sichtbar hinter der Ost-Stirn"-Punkten der Abnahme
  liegen im Kuenstler-Ausschnitt; der Ausschnitt ist bitgleich gezeichnet (3344 = 3344).
* Beleg: Zelle 6 @RDT+0x0045C `96 19 e2 04 63 98 2d 37 01 ff 00 03`; SLD-Atlas ROOM30A0.BSS Cut 0 (Engine-Entpacker re15_sld_* = FUN_800c47e8),
  Komponenten 11/9/10 an (232,-24)/(144,24)/(57,86); Boden-Strahlen der Zeilen 150..196; Zelle 8 @RDT+0x00474. Einzelheiten unten.
* Aenderung: nur Riegel (unit_r41_masken36_stirn; Szenen z30A0_o3/_o4/_a0 Art Z und v30A0_o2/_a0, v30A1_o5 Art V im Riegel _fern).
**Mangel 2 - Restliste / Punkt 1 nicht geschlossen.**
* (a) Ursache Gleisbett: die Saaten des Ausschluss-Werkzeugs nahmen fuer Tuer-AOTs das Band aus `slots[].band` (im Port fuer Door_aot_set
  nie gesetzt) statt pc[4]; Beleg AOT-Scan @0x80042cac..0x80042ccc, ROOM6000-Tueren pc[4] = 2. Vorher: 90 437 Band-0-Stehplaetze in
  6000/6001, "zu fern" Stage 6 39 934. Nachher: 0 bzw. 0; 37 Rechtecke je Variante in 6000 C5/C7/C8 wieder gezeichnet (u.a. Lok-Unterteil C7).
* (b) Ursache Gelaender: der Generator saete nur aus vollen 16x16-Bloecken (gen_region.find_blocks `if cnt != n: continue`); duenne
  Kuenstler-Komponenten (Gelaender, Pfosten, Lampen, Saeulen) waren nie platziert. Vorher: 800 unplatzierte Komponenten. Aenderung:
  platzieren_r41.py (Lage per maskierter SSD, Schwellen an 422 Generator-Lagen kalibriert, Tiefe nach der Generator-Regel, Ausschluss R37)
  + Kapazitaet 128 je Cut (re15_pri.h; Original ohne Grenze, FUN_80039270 @0x8003928c lbu v0,7(a0)). Nachher: 437 Komponenten in 74
  Sektionen, 1627 neue Rechtecke (154 vom Ausschluss genommen), gezeichnet 998 854 -> 1 159 393 Punkte; an der exe 0 Uebermalungen
  VOR den neuen Gegenstaenden (16 Stellen), Verdeckung HINTER ihnen (8 Stellen, z.B. Leon hinter der Saeule 6010 C2 274 Punkte).
* (c) Ursache Wandecke 6000 C5: die Zelle-12-Teilung der Traeger-Mitte (Rechteck 37, Tiefe 176 aus der falschen NB2-Zeile Zelle 13) stand
  auch ohne Band-0-Plaetze auf der Rueckholliste: in den Spalten 31..40 wuerde sie Figurpunkte im Gang oestlich der Ecke verdecken, die
  NICHT hinter dem Prisma liegen (192 Band-2-Plaetze); in den Spalten 1..30 keinen. Aenderung: teilen_r41.py - Spalten 1..30 neues Rechteck
  mit Zelle-12-Tiefe 59 (Modell-Zeile 127), Spalten 31..40 behalten 176. Nachher an der exe: Leon hinter dem Block sichtbar k6000_05a
  964 -> 512, k6000_05b 617 -> 371, k6000_05c 876 -> 644, m6000_05 1066 -> 447; davor 0 uebermalt.
* Rest (gemessen, mit Grund): Pulte/Schreibtisch 5050 C1/C2 und Moebel/Lampe 30A0 C0 - Kamera ueber dem Grundriss (Nahfeld-Sperre NB2) bzw.
  alle 15 neuen Rechtecke der Lampe 30A0 C0 vom Ausschluss genommen; Roboterarm 50B0 C1 - die 7 Komponenten haben keinen Bodenkontakt
  (unterster Punkt jeder Spalte ueber dem Horizont, keine Tiefe nach der Generator-Regel); Gelaender 3050 C6 oben - die Komponenten 2/3
  (527/335 Texel, Lage (136,-2)/(142,-2)) ohne Bodenkontakt; 3040 C3 rechter Pfeiler (4743 Punkte) vom Ausschluss genommen.
**Mangel 3 - Laeufer ohne Laufpruefung: behoben** (Ursache, Beleg, Aenderung, Gegenprobe unten).

### NB4 Mangel 3 - Integrations-Laeufer ohne Laufpruefung (behoben)
* **Ursache:** `tests/integration/test_r41_masken36_stage2.cmake` (Laeufer von _stage2/_zelle/_zellen3d/_fern) pruefte je Lauf nur
  `EXISTS f.ppm`. `RE15_FRAMEDUMP=120:f.ppm` gilt fuer Bild 120 JEDES Raums (platform/pc/main.c: `if (f == s_fdd_frame)` mit
  `f = g_engine.frame_count`, der Zaehler beginnt je Raum neu) - der Boot-Raum ROOM1240 schreibt f.ppm, bevor `RE15_DEBUG_JUMP=...@250`
  springt.
* **Messung vorher:** Abnahme 3, d4040_09 (Laufordner `build/tests/integration/r41_masken36_zelle_wd/lauf_3c0f29fe_d4040_09_a`): debug.log
  zeigt `[framedump] 320x240 -> f.ppm` (Zeile 61, Boot), `[room] PC loaded room4040.rdt` (Zeile 70), danach nur `[walk] F120 ...` - kein
  zweiter Framedump, kein EXIT_AT (Abbruch durch das 180-s-Limit unter Last) -> verglichen wurde das Boot-Bild (falsch rot 1185 von 1387).
* **Beleg Mechanik:** main.c `RE15_EXIT_AT="<bild>[#<raum>]"` beendet am ENDE des Bilds, nach `re15_render_end_frame()` (also nach dem
  Framedump desselben Bilds) mit `[flow] EXIT_AT: Bild %u in Raum %04x erreicht -> exit`; render_pc.c schreibt `[framedump] %dx%d -> %s`
  bzw. `[framedump] SDL_RenderReadPixels failed`.
* **Aenderung:** Funktion `r41m_lauf_pruefen` im Laeufer: gueltig nur, wenn debug.log NACH dem letzten `loaded room<zr>.rdt` den Satz
  `[framedump] <w>x<h> -> f.ppm` und danach `[flow] EXIT_AT: Bild 120 in Raum <zr> erreicht` zeigt; sonst bis zu 3 Versuche, dann
  Fehler (TIMEOUT der vier Riegel 1500 -> 2400 s). Neuer Riegel `integration_r41_masken36_laufpruefung` (-DGEGENPROBE=1): erster Lauf mit
  `RE15_EXIT_AT=60#<zr>` (Abbruch im Zielraum VOR dessen Framedump, f.ppm = Boot-Bild) muss abgelehnt, der zweite volle Lauf angenommen
  werden (PASS nur mit "Gegenprobe OK"). Dazu: Szenenfeld `W=<gier>` (Gier der Figur, Vorgabe 0).
* **Messung nachher:** Pruef-Funktion gegen echte Logs (`m41nb4/lp/`): voller Lauf d30A0_00 angenommen; der d4040_09-Abbruch der Abnahme
  abgelehnt ("kein '[flow] EXIT_AT: Bild 120 in Raum 4040' nach dem Laden des Zielraums"); EXIT_AT 60 abgelehnt; ReadPixels-Fehler
  abgelehnt ("kein Framedump im Zielraum vor EXIT_AT"). An der exe: Riegel integration_r41_masken36_laufpruefung (Tests NB4).

### NB4 Mangel 1 - "Ost-Stirn der Trennwand 30A0/30A1 Cut 0 ohne Maske" (mit Messung widerlegt)
* **Messung (Abnahme-3-Laeufe derselben exe md5 597ba53a, eigener Leser `m41nb4/py/stirn10.py`):** alle "sichtbar hinter der Ost-Stirn"-
  Punkte der Abnahme (o1 62, o2 353, o3 416, o4 266, Elza o5 272, a30A0_00 634 = 2003) liegen AUSSERHALB des verifizierten
  Kuenstler-Ausschnitts (0 von 2003; Abstand zur Kante >= 1 Spalte) und auf keiner Maske.
* **Beleg 1 - der Gegenstand im Bild ist gezeichnet:** der Kuenstler-Atlas des Cuts (SLD des BSS-Chunks ROOM30A0.BSS Cut 0, Entpacker
  wie FUN_800c47e8 / re15_sld_*) hat die Trennwand als Komponenten 11/9/10 an den Verschiebungen (232,-24)/(144,24)/(57,86)
  (Farbabstand zum Hintergrund Median 10..11); die verifizierte Region dieser Lagen ist in den Spalten >= 282 bitgleich die gezeichnete
  Deckung (3344 = 3344 Punkte, 0 fehlen, 0 zu viel). Die Komponente 9 an ihrer zweiten Lage (175,-21) passt im Ostbereich NICHT
  (Farbabstand Median 208,5, 1 % <= 24) - dort gehoert sie nicht hin.
* **Beleg 2 - die Zellenflaeche ist nicht die Stirnplatte:** Zelle 6 @RDT+0x0045C (`96 19 e2 04 63 98 2d 37 01 ff 00 03`) reicht bis
  z 15375 und (Rahmen) 3025 hoch. Die Kante der Zeichnung liegt, auf die Ebene x = -19975 projiziert, in JEDER Zeile 118..194 bei
  z 14834..14904 (Bildpunktmitten; Stirnplatte ~750 tief) und oben bei y -2335 (Zeile 118); darueber nur der Pfosten (z 14255..14621).
  Jenseits der Kante zeigt das Bild, was HINTER der Zelle liegt: Zeilen 150..196 den Boden mit Papieren (Sehstrahl kreuzt x = -19975 bei
  z 14913..15295 in 209..1316 Hoehe und trifft den Boden bei z 15376..18289, z.B. (309,192) Boden (-20165,15457) Farbe (150,154,165)),
  Zeilen 88..148 die Rueckwand-Schraenke Zelle 8 @RDT+0x00474 (Strahl bei z 18225 auf x ~-22000 in 321..2385 Hoehe; Pflanze, roter
  Kasten, Schatten). Ein vorgerendertes Bild zeigt den Boden an einem Punkt nur, wenn davor nichts steht -> der Zellenkasten ist dort leer.
* **Folgerung:** Leon/Elza sind an diesen Punkten im Bild VOR dem, was dort zu sehen ist (Boden hinter ihnen, Schraenke bei z >= 18225; die
  Figur steht bei z 16000..16300) - sichtbar ist richtig. Eine Maske ueber der ganzen Zellenflaeche (Vorschlag der Abnahme, Tiefe der Zelle 6)
  wuerde die Figur an einer Kante abschneiden, die es im Bild nicht gibt. Bild `M_bilder/NB4/NB4_M1_30A0_ost_stirn.png` (gruen = Kuenstler-
  Ausschnitt = gezeichnet, magenta = Zellenflaeche, gelb = Kante der Stirnplatte, tuerkis = Leon sichtbar in o3_30A0 / a30A0_00).
* **Riegel:** `unit_r41_masken36_stirn` (Zellbytes; 0 offene Kerben im Fenster x 282..319 y 80..215; Kante der Zeichnung z <= 14950 in den
  Zeilen 118..194; KEIN gezeichneter Punkt mit Sehstrahl durch die Zellenflaeche bei z > 14950; Leon/Elza-Datei gleich) und im Riegel
  `integration_r41_masken36_fern` sechs Szenen an den Abnahme-Stellen: z30A0_o3/_o4/_a0 (Art Z: Figur hinter der Stirnplatte, Fenster ganz in
  der gezeichneten Platte x 288..306 -> V == 0) und v30A0_o2/_a0, v30A1_o5 Elza (Art V: Fenster rechts der Kante -> H == 0, Gegenprobe gegen
  eine Maske ueber der Zellenflaeche).

### NB4 Mangel 2 (a) - die Band-0-Stehplaetze im Gleisbett ROOM6000 waren ein Fehler der Saaten (OFFEN 10/14 geschlossen)
* **Messung:** `test_r37_masken36 saaten 6000` (Stand NB3) lieferte die AOT-Saaten (2800,2750), (-4900,-8350), (1763,-10677) auf Band 0 -
  das sind die Mitten der Tueren Slot 0/1/3 (ROOM6000.RDT @0x00D58 `3b 00 02 31 02 00 d0 07 98 08 40 06 4c 04 ..` Rechteck
  (2000,2200,1600,1100), @0x00D78 Slot 1 (-5350,-9250,900,1800), @0x00DDA Slot 3 (693,-11104,2140,854)); in allen drei Saetzen ist
  pc[4] = 2. Alle Aot_set/Door_aot_set von ROOM6000 tragen pc[4] = 2, es gibt keinen Treppensatz (Typ 12/13), die Pos_set des Raums setzen
  y = -3600 (@0x00FA4 `32 00 d9 15 f0 f1 52 e1`, @0x0100A `32 00 f5 d6 f0 f1 c6 21`), alle Tuer-Ankuenfte kommen auf Band 2 an
  (50B0 (5200,1600), 6010 (-4200,-8450)/(-4200,-20500), 6030 (1693,-10054)).
* **Beleg (RE):** der AOT-Scan FUN_80042bac gated JEDEN Satz (der Satz beginnt bei pc+2, rec[2] = pc[4]) auf das Band des Spielers:
  @0x80042cac `lbu v0,2(s0)` / @0x80042cb4 `andi v0,v0,0x80` (gesetzt = jedes Band, Sprung @0x80042cb8) / @0x80042cc0 `lbu v1,130(s1)`
  (Spieler +0x82) / @0x80042cc4 `lbu v0,2(s0)` / @0x80042ccc `bne v1,v0,0x8004301c` (Satz uebersprungen); fuer das Tuer-Objekt dasselbe
  @0x8002bf38 `lbu v1,846(s6)` / @0x8002bf3c `lbu v0,130(s2)` / @0x8002bf44 `bne` (FUN_8002bd44). Ein Satz sce 0 dispatcht auf
  @0x8004305c (`jr ra` / `addu v0,zero,zero` nach zwei Merkern) - ohne Ereignis. Der Port legt pc[4] fuer Door_aot_set in
  `g_aot.door_params[].band` ab (scd_vm.c op_door_aot_set), `slots[].band` bleibt dort 0; die Saaten-Sammlung des Ausschluss-Werkzeugs
  (test_r37_masken36.c saaten_sammeln, R37 Spur P) las `slots[].band` -> jede Tuer saete auf Band 0.
* **Aenderung:** saaten_sammeln nimmt fuer Tuer-Saetze (Typ DOOR, oder Typ NONE mit door_params.floor != 0 = traeger Tuersatz) das Band
  aus door_params (pc[4]); Option `--saat-alt` = alter Stand (Gegenprobe/Werkzeug); Werkzeuge `saatband` und `netzvergleich`.
* **Messung nachher:** 121 von 1465 AOT-Saaten aendern ihr Band (alle Tueren mit pc[4] != 0). Stehplaetze Stage 3..6 (`netzvergleich`):
  ROOM6000/6001 Band 0 90 437 -> **0** (das ganze Gleisbett-Netz), ROOM3050/3051 Band 0 27 282 -> 18 855, ROOM5090/5091 Band 1 0 -> 42 987
  (Tuer auf Band 1), ROOM6030/6031 Band 0 6 290 -> 6 056; alle anderen Raeume/Baender gleich. Ausschluss (`erzeuge`) mit der Regel auf
  dem NB3-Stand: nur ROOM6000/6001 C5/C7/C8 aendern sich, 37 Rechtecke je Variante wieder gezeichnet, 0 neu ausgeschlossen; gezeichnete
  Punkte 983 624 -> 998 854, "zu fern" 40 276 -> 342 (Stage 6 39 934 -> 0). Die Kette k6 der NB3 mit der Regel nachgefahren
  (`m41nb4/kette_k8.sh`, Zerlegung mit den alten Saaten `--saat-alt`): andere Raeume bitgleich, 6000 C5 etwas mehr gezeichnet (+199), die
  Wandecke aber schlechter (verdeckt 875 statt 900) - nicht uebernommen; der NB3-Stand + Regel bleibt, die Ecke s. (c).

### NB4 Mangel 2 (b) - Gelaender & Co.: Kuenstler-Komponenten, die der Generator nie platziert hat (platzieren_r41.py)
* **Messung vorher:** Die Restliste nannte "Gelaender 3050 C6, 30E0 C6..C9, 3040 C3: nicht in der Generator-Region". Im Original-Atlas der
  Cuts (SLD, `original.atlas`) liegen sie aber: 30E0 C7 hat 31 Komponenten, nur 2 platziert (Bodenplatten), die Pfosten und Handlaeufe nicht
  (Bild `m41nb4/atlas_ROOM30E0_07.png`); 3040 C3 49 Komponenten / 4 platziert; 3050 C6 15 / 2. Ueber alle 216 Generator-Sektionen: 800
  unplatzierte Komponenten mit >= 16 Texeln (410 127 Texel).
* **Ursache (Code):** `analysis/esp_masken_2026-09-03/gen_region.py find_blocks`: `if cnt != n: continue` - nur VOLLSTAENDIG deckende 16x16-Bloecke
  werden als Saat gesucht; `region_grow` waechst nur innerhalb schon belegter Verschiebungen. Eine eigene Komponente ohne vollen 16x16-Block
  (Gelaender, Pfosten, Rohre, Lampen) bekommt nie eine Lage.
* **Regel (Kopf von `re15_port/tools/maske/platzieren_r41.py`):** Lage je Komponente per maskierter SSD (FFT) ueber alle Verschiebungen;
  angenommen, wenn RMS <= 18,91 und die beste nicht benachbarte Lage >= 1,5x schlechter ist. **Kalibrierung** an den 422 vom Generator
  platzierten Teilen (>= 16 Texel) derselben Sektionen (`m41nb4/py/kalib.py`): die SSD findet 410 an der Generator-Lage, RMS dort Median 9,66,
  99 % 18,91, Max 20,24; die 12 abweichenden Faelle haben ein Verhaeltnis <= 1,47. Texel > 48 daneben bleiben weg (R37-Blit-Grenze);
  Punkte unter vorhandenen Rechtecken bleiben weg; Tiefe = Regel des Generators je Komponente (Bodenkontakt je Spalte auf y = 0,
  DEPTH_SAFETY 0,90; ohne Bodenkontakt keine Tiefe -> nicht gezeichnet); Rechtecke wie schliessen_r41 (Spaltentiefe, gierig zusammengelegt);
  Palette je Blatt neu (255 Farben Median-Schnitt ueber alte + neue Texel; alte Texel aendern sich im Mittel <= 4,38/765); danach der
  Ausschluss R37 (`erzeuge`). Alter Teil jeder Sektion bytegleich, Leon/Elza bytegleich.
* **Kapazitaet:** 37 volle Cuts (105 Rechtecke) hatten platzierbare Komponenten. `re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT` 105 -> 128: das
  Original hat keine feste Grenze (FUN_80039270 @0x8003928c `lbu v0,7(a0)` = RDT-Kopfbyte 7 als Anzahl, u8; Puffer daraus @0x80039294..
  @0x800392c8); 128 = Breite der Port-Tabelle des Ausschlusses (masken36_r37.c `uint32_t bits[4]`, `idx >= 128` -> 0). Original-Sektionen
  (hoechstens 105) bleiben unberuehrt.
* **Messung nachher (statisch, eigener Zaehler wie stat_nb3 der Abnahme, Lesekapazitaet 128):** 437 Komponenten in 74 Sektionen platziert,
  1627 neue Rechtecke, 154 davon vom Ausschluss genommen; gezeichnet 998 854 -> **1 159 393** Punkte (+160 539), nur ausgeschlossen
  232 089 -> 265 201 (die genommenen neuen Rechtecke). 72 Cuts geaendert (`M_bilder/NB4/NB4_zahlen.txt`): 648 007 -> 823 776 Punkte,
  **0 weggefallen**. Riegel `unit_r41_masken36_platziert`: Lage jeder Komponente mit dem Spiel-Entpacker bestaetigt (437/437), Blit der neuen
  Rechtecke Mittel 6,15 (je Sektion max 10,72), Gelaender 30E0 C6 1745 / C7 1670 / C8 2013 / C9 3558, 3050 C6 278, 3040 C3 9546 Punkte.
* **Messung nachher (exe md5 d9a03044 / 46823269, eigene Laeufe `m41nb4/runs`, Leon-Dreiecke RE15_TRILOG; Stellen = `test_r37_masken36 suche 5`):**
  | Szene | Cut | Stelle | Leon unter NEUEN Rechtecken | davon von neuen verdeckt | uebermalt |
  |---|---|---|---|---|---|
  | d3040_03 / d3040_02 / d3040_04 / d3040_06 / d3040_07 / d3040_09 / d3040_13 | 3040 C3/C2/C4/C6/C7/C9/C13 | DAVOR | 2460 / 424 / 2887 / 56 / 913 / 2584 / 212 | 0 | 0 (d3040_09: 1 Rand, nicht neu) |
  | d30E0_06 / _07 / _08 / _09 | 30E0 C6/C7/C8/C9 | DAVOR | 925 / 82 / 0 / 368 | 0 | 0 |
  | d3050_06 / d3050_11 / d5090_12 / d6000_02 / d6000_05 | 3050 C6/C11, 5090 C12, 6000 C2/C5 | DAVOR | 71 / 756 / 563 / 1948 / 303 | 0 | 0 |
  | h3040_03 / h3040_07 / h3040_13 | 3040 C3/C7/C13 | HINTER | 475 / 99 / 81 | 36 / 99 / 78 | - |
  | h30E0_07 / h30E0_08 | 30E0 C7/C8 | HINTER (Gelaender) | 37 / 62 | 26 / 47 | - |
  | h50B0_03 / h6010_02 | 50B0 C3, 6010 C2 | HINTER | 69 / 274 | 49 / 274 | - |
  Bilder: `M_bilder/NB4/NB4_exe_h3040_03.png` (Leon hinter dem Gelaender), `NB4_exe_h6010_02.png` (hinter der Saeule), `NB4_exe_d3040_03.png`,
  `NB4_exe_h3040_07.png`; Vorher/Nachher-Boegen `M_bilder/NB4/NB4_bogen_ROOMxxxx_Cnn.png` (30E0 C6..C9, 3050 C6, 3040 C3, 50B0 C1/C3, 5050 C1/C2,
  6000 C5/C7, 30A0 C0). Die Gelaender von 30E0 trennen den Steg von einer Maschinengrube; dahinter gibt es kaum Stehplaetze (`suche 60`:
  keine HINTER-Stelle) - sie wirken vor allem auf Gegner/Effekte in der Grube und auf das Bild.

### NB4 Mangel 2 (c) - Wandecke 6000 C5 (Traeger-Mitte, OFFEN 14)
* **Messung vorher (Abnahme 3, an der exe):** hinter dem Wandblock Zelle 12 @RDT+0x007EC sichtbar k6000_05a 964, k6000_05b 617, k6000_05c 876;
  eigener statischer Zaehler (`m41nb4/py/ecke6000.py`, Leon-Dreiecke der Abnahme-Laeufe) unverdeckt 1039 / 645 / 936, m6000_05 1038 - die
  Punkte unter Rechteck 37 (Tiefe 176, NB2-Zeile Zelle 13 @RDT+0x007F8 = Wandblock JENSEITS des Gangs).
* **Ursache (gemessen):** die NB3 hatte das Rechteck mit Zelle 12 geteilt, die Rueckholliste nahm die Teilung zurueck. Die NB3-Begruendung
  "wegen Band-0-Stehplaetzen" ist widerlegt: auch ohne Band-0-Plaetze (Kette k8) steht Rechteck 33 (= heute 37) auf der Rueckholliste.
  `m41nb4/py/r37ecke.py` (9492 erreichbare Band-2-Plaetze von Cut 5, Figur +-450 x 0..3000 in 25er-Schritten, Tiefe je Spalte = max
  bucket(Eintritt in das Prisma der Zelle 12, Saeule) wie kacheln_r41): in den Spalten 31..40 des Rechtecks wuerden 175..6158 Figurpunkte
  an 192 Plaetzen im Gang oestlich der Ecke (x 14833..15433, z -4984..-4384) verdeckt, deren Sehstrahl das Prisma NICHT vor ihnen trifft
  (z.B. (15233,-4784): Strahl kreuzt z -5674 bei x ~12936 > 12578); in den Spalten 1..30 KEINER.
* **Aenderung:** `re15_port/tools/maske/teilen_r41.py --raum 6000 --cut 5 --rechteck 37 --spalten 30 --zelle 0x007EC --hoehe -4100 --saeule 1`:
  Spalten 1..30 = neues Rechteck 127 (gleiche Atlas-Abbildung, eigene Gruppe) mit Tiefe 59 = max bucket(Eintritt) seiner 960 Koerperpunkte,
  Modell-Zeile Zelle 12 (Saeule, Hoehe 4100, IoU 0,71 wie die anderen Traegerteile); Spalten 31..40 behalten Tiefe 176. Ein Platz wurde mit
  `platzieren_r41.py --reserve 6000:5:1` freigehalten (6000 C5: 26 statt 27 neue Rechtecke, gleiche Punkte). Ausschluss: das neue Rechteck
  bleibt gezeichnet.
* **Messung nachher:** statisch unverdeckt hinter dem Block k6000_05a 1039 -> 407, b 645 -> 179, c 936 -> 501, m6000_05 1038 -> 408, VOR dem
  Block verdeckt 0; an der exe (eigene Laeufe, |A-B| > 24): hinter dem Block sichtbar k6000_05a 512 (Abnahme 964), b 371 (617), c 644 (876),
  m6000_05 447 (1066); VOR dem Block uebermalt 0, d6000_05a 0. Bild `M_bilder/NB4/NB4_exe_k6000_05a.png`. Der Rest liegt unter den Spalten
  31..40 (s. Ursache) und unter weiteren Traeger-Rechtecken.

### NB4 Nebenbefunde der Abnahme 3 (Hinweise)
* Zitat "Akteur-Cull FUN_8002c18c" berichtigt (Szenendateien, Laeufer, test_r37_masken36.c): Figuren-Cull am Regionsviereck FUN_80014368,
  z.B. NPC-Zeichner FUN_80039ca0 @0x80039d68 `jal 0x80014368` / @0x80039d70 `beq v0,zero,0x80039e3c`; FUN_8002c18c ist der Objekt-Zeichner.
* DAT_80073e94[6] = 450 ist der Kollisionsradius des Spielers; als Ausdehnung der Figur zur Kamera hin (test_r37_masken36 hinter /
  kacheln_r41) PORT-WAHL - im Code so gekennzeichnet.
* 6000 C5 Riegel-Szenen in Kamera-Wechselzonen (Hinweis Abnahme 3): die eigenen exe-Stellen k6000_05a/b/c (natuerlicher Cut-5-Bereich) sind oben
  gemessen; die Riegel-Szenen m6000_05/d6000_05a bleiben (geometrisch richtig, s. NB3).


## Tests (gezielt, KEINE volle Suite)

Neu (tests/unit/probes/r41_masken36.cmake, registriert nur dort):
| Test | misst | Ergebnis |
|---|---|---|
| `unit_r41_masken36_loecher` | Punkt 1, Mechanik an allen 216 Generator-Sektionen ueber die Engine-Leser: eigener Atlas da und vom Lader gewaehlt; offene Kerben (Kuenstler-Atlas deckend + farbgleich an der Kante eines alten Rechtecks, nicht gezeichnet) VORHER 61 672 -> NACHHER 15 498 (<= 30 % verlangt, Gegenprobe VORHER > 10 000); Blit eigener Atlas max Mittel 10,66 (<= 12) | PASS |
| `unit_r41_masken36_kurbel` | Punkt 2, ROOM2020 Cut 3: Original-NULL-Sektion (+0x3FC FF FF FF FF), Sichtlinie Kamera -> Leon (3650,-2925) kreuzt SCA-Zelle 11 (Suedreling), Reling-Maske verdeckt 31 von 101 Beinpunkten | PASS |
| `integration_r41_masken36_stage2` | Punkt 2, echte exe, 13 Szenen Leon + Elza: Kurbel 2020/2021 C3 (86/779, 124/648), VOR 11A0 C1, 2030 C11, 2040 C9, 2050 C0 = 0 uebermalt, HINTER 11A0/11A1 C1, 2040 C9, 2080 C3 >= Haelfte, 20A0/20A1 C12 an der Wand und auf dem frueheren Bodenstreifen 0 (Gegenprobe Basisstand: 50) | PASS |

Angepasst (mit Beleg, Dossier 1.3): `unit_r37_masken36_zensus` (Stages 3..6 jetzt eigener Atlas), `unit_r37_masken36_ausschluss`
(Generator-Sektion = Stage 3..6; Schranke NB2 M2 als Ausschluss-Grund, gezaehlt am Endzustand: 0/0), `unit_r37_masken36_verdeckung`
(unveraendert gruen), `integration_r37_masken36_stage3..6` (Spur-P-Zeile darf nicht mehr kommen): Stage 3 HINTER 1208/1238
(R37 1123/1234), Elza 1221/1267, Stage 4 880/883, Stage 5 222/320 (R37 199/322), Stage 6 844/1440 (R37 820/1440), DAVOR
ueberall 0, FREI 0, WEG M1 0; `integration_r37_masken36_nb2` Cut 15 Cursor 0.

Gezielter Lauf (Bau `local_build.sh configure` + `build`, Endstand):
`ctest --test-dir re15_port/build -R "masken36|_pri|pri_|sld_atlas|itemorte|optik|dokumente" -j 4 --timeout 900 --output-on-failure`
-> **`100% tests passed, 0 tests failed out of 41`** (unit_r37_masken36_* 3, integration_r37_masken36_* 5, unit_r41_masken36_* 2,
integration_r41_masken36_stage2, unit_pri_* 5, unit_sld_atlas, unit_r36_pri_* 2, integration_r36_pri_schatten,
unit_r37_pri1100_* 4, integration_r37_pri1100_* 2, integration_pri_masken, unit_r38_itemorte_* 4, integration_r38_itemorte,
unit_r39_optik_* 5, integration_r39_optik_* 2, unit_r34n_e_dokumente, integration_r34n_e_dokumente_bild).
Linux: neue Tests ohne .exe-/Backslash-Pfade, `m` gelinkt, cmake_minimum_required in jedem -P-Skript, eigene Laufordner und
exe-Kopie je Lauf (`re15_pc_r41m2_<id>`).

### Tests Nachbesserung 1 (gezielt, KEINE volle Suite)

Neu/geaendert (nur tests/unit/probes/r41_masken36.cmake + eigene Dateien):
| Test | misst | Ergebnis |
|---|---|---|
| `integration_r41_masken36_zelle` (neu) | Mangel 1, echte exe: Objekte mit Zellen-Tiefe - HINTER-Stellen verdeckt (H >= Haelfte bzw. H > 0), DAVOR-Stellen 0 uebermalt; Szenen `test_r41_masken36_zelle_szenen.cmake`, Laeufer = stage2-Skript (SZENEN/PRAEFIX r41m3, Stages aus den Szenen). Gemessen (S/H): HINTER 3050 C5 546/531, 5100 C7 1436/865, 5020 C7 615/401, 4040 C6 152/72, 30A0 C3 1333/318, 4040 C9 231/177, 5100 C2 496/338, 4040 C7 56/55; DAVOR 3050 C5 5731/0, 5100 C7 3704/0, 5020 C7 8777/0, 30A0 C3 5925/0, 4040 C9 1387/0, 5100 C2 2826/0, 4040 C7 1943/0, 30A0 C0 3809/0 | PASS (Lauf 4) |
| `integration_r41_masken36_stage2` (erweitert) | + Mangel 3: v2030_15/v2031_15 Leon/Elza VOR der Reling mit weggestellter Spinne (G=1,...) H == 0 (gemessen S 1225 / 1017, H 0 / 0), Gegenprobe m2030_15 (Spinne hinter der Reling verdeckt: S 1762, H 166); + Mangel 4: m2030_07 / m2040_07 Leon HINTER dem Gelaender verdeckt (S 362 / 5398, H 77 / 377) | PASS (Lauf 3) |
| `unit_r41_masken36_loecher` (Messwert) | Kerben NACHHER 11 494 von 62 432 = 18,4 % (vor NB1 25,1 %), Schranke <= 30 % unveraendert | PASS |
| `unit_r37_masken36_ausschluss` (unveraendert) | rechnet die Tabelle aus den Dateien nach (== gen/masken36_ausschluss_r37.inc nach Schritt 2), Schranke NB2 M2 0/0 | PASS |

Gezielte Laeufe Nachbesserung 1 (Bau `local_build.sh configure` + `build` nach der letzten Asset-/Tabellen-Aenderung,
Spurbaum, KEINE volle Suite; alle Laeufe auf demselben Endstand der Assets):
1. `ctest -R "unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_r38_itemorte|unit_r39_optik|unit_r34n_e_dokumente|unit_r37_fahrstuhl_lift2030" -j 3 --timeout 1200 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 28`**; nach dem Neubau von test_r41_masken36 (nur Messwert-Text)
   `ctest -R "unit_r41_masken36" --timeout 900 --output-on-failure` -> **`100% tests passed, 0 tests failed out of 2`**
2. `ctest -R "integration_r37_masken36" -j 2 --timeout 1500 --output-on-failure` -> **`100% tests passed, 0 tests failed out of 5`**
   (stage3..6 + nb2: R37-DAVOR 0 / HINTER-Schranken / Cursor 15 auf dem Endstand)
3. `ctest -R "integration_r41_masken36_stage2|bss|itemorte|optik|dokumente|r34n_g|r34n_b_cursor|r19|r22_10f0|stage6_hintergrund|rolltor20a0|zgirl_5090|elza5091|elza50b1|boot_bg_pin|r30_cut_blitz|integration_pri_masken|integration_r36_pri|integration_r37_pri1100" -j 2 --timeout 1500 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 41`** (darin integration_r41_masken36_stage2 mit den NB1-Szenen, 520 s)
4. `ctest -R "integration_r41_masken36_zelle" --timeout 1500 -V` -> **`100% tests passed, 0 tests failed out of 1`**
   ("16 Szenen OK"; ein erster Lauf scheiterte nur an der Plausibilitaet S > 50 der Szene 4040 C6 (-23600,-422): Leon dort
   44 Punkte klein - ersetzt durch die HINTER-Stelle (-15000,-7722) aus Schritt 2, keine Schranke geaendert)
Linux/parallel: neue Szenen nur ueber das bestehende -P-Skript (cmake_minimum_required, keine .exe-/Backslash-Pfade,
LAST_EXT), eigenes exe-Praefix je Riegel (`re15_pc_r41m2_*` / `re15_pc_r41m3_*`), eigene Laufordner.

### Tests Nachbesserung 2 (gezielt, KEINE volle Suite)

Neu/geaendert (nur tests/unit/probes/r41_masken36.cmake + eigene Dateien; test_r37_masken36.c angepasst mit Beleg):
| Test | misst | Ergebnis |
|---|---|---|
| `unit_r41_masken36_zellen3d` (neu, Modus `zellen3d` von test_r37_masken36) | Mangel 1 Mechanik: 530 Modell-Zeilen byte-genau im RDT (12 SCA-Bytes am Offset, solide), MSK-Tiefe (Leon+Elza) == Zellen-Tiefe der Zeile, Engine zeichnet jedes Modell-Rechteck; Ausschluss MIT 3D-Urteil == Engine-Tabelle in 46 Cuts; Gegenprobe OHNE 3D-Urteil: 79 Rechtecke fielen weg (Lok 6000 76, 5090 C11 3), alle mit Modell-Zeile | PASS |
| `integration_r41_masken36_zellen3d` (neu) | Mangel 1 an der exe: 14 Szenen (Lok 6000 C5/C7/C8/C9, 3040 C0, X-Kiste 5020 C6, Treppenraum 5020 C0, Trennwand 30A0 C1, Kistenstapel 5090 C13, Elza 6001 C5): DAVOR H == 0, HINTER H >= Haelfte bzw. H > 0 | s. Lauf unten |
| `unit_r37_masken36_ausschluss` (Regel erweitert) | rechnet die Tabelle MIT 3D-Urteil aus den Dateien nach (== gen/masken36_ausschluss_r37.inc), keine Fehlverdeckung ohne Zelle/Koerper davor, Raumpaare gleich, Schranke 0 | PASS (561 s) |
| `unit_r37_masken36_verdeckung` (FREI angepasst) | FREI-Szenen 30A0/30A1 C0: die Figur steht auf der gezeichneten Modell-Maske (1221 / 590 Punkte) und ist nicht verdeckt - die Zellen-Tiefe schuetzt sie (vorher: der Ausschluss; ungefiltert verdeckt jetzt 0, weil die Trennwand die Tiefe ihrer Zelle traegt) | PASS |
| `unit_r41_masken36_loecher` (Messwert) | Kerben NACHHER 10 178 von 63 546 = 16,0 % (NB1 18,4 %), Schranke <= 30 % unveraendert | PASS |
Laeufe (Spurbaum, Bau `local_build.sh build` nach der letzten Aenderung; KEINE volle Suite):
1. `ctest --test-dir re15_port/build -R "unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_r38_itemorte|unit_r39_optik|unit_r34n_e_dokumente|unit_bss|prop_bss|unit_stage6_hintergrund" -j 3 --timeout 1500 --output-on-failure`
   -> erster Lauf (Stand k2) `97% tests passed, 1 tests failed out of 31` (unit_r37_masken36_verdeckung: FREI-Gegenprobe, s. oben,
   Riegel angepasst), danach `100% tests passed, 0 tests failed out of 31`; **Endstand k3 (Nahfeld-Sperre, nach Neubau):
   `100% tests passed, 0 tests failed out of 31`** (695 s); nach der Messwert-Zeile in test_r41_masken36.c neu gebaut und
   `ctest -R "unit_r41_masken36_loecher"` -> `100% tests passed, 0 tests failed out of 1`.
2. `ctest --test-dir re15_port/build -R "integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r35_zgirl_5090|integration_r38_affen3|integration_r35_granate" -j 2 --timeout 1800 --output-on-failure`
   (Endstand k3; die Mask-Riegel R37/R41 aller Stages, PRI-/Optik-/Itemorte-Riegel, die Verhaltens-Riegel der durch NB2 geaenderten
   Raeume 3040 (affen3 x3) und 5090 (granate, zgirl_5090); 30A0/5020/6000 haben keine eigenen Riegel ausser den Masken-Riegeln)
   -> **`100% tests passed, 0 tests failed out of 21`** (integration_r41_masken36_zellen3d 662 s, _stage2 536 s, _zelle 480 s).
   Ein frueherer Integrationslauf am Stand k2 wurde nach dem Befund 5050 C2 abgebrochen (bis dahin integration_r37_itemband_lift
   1457 s und integration_r36_dachkiste 1648 s gruen - beide Raeume hat NB2 nicht geaendert).
Determinismus: zweiter Lauf von `zellen3d_r41.py` (letzter Schritt der Kette, Stand k2) -> 30 MSK und Modell-Tabelle bitgleich; Kette k3
selbst: zweites erzeuge bitgleich zum ersten.

### Tests Nachbesserung 3 (gezielt, KEINE volle Suite)
Neu/geaendert (nur tests/unit/probes/r41_masken36.cmake + eigene Dateien):
| Test | misst | Ergebnis |
|---|---|---|
| `unit_r41_masken36_fern` (neu, `test_r37_masken36 fern 40276 0 72 270 39934`) | Mangel 1 Mechanik: erreichbare Figurpunkte HINTER dem Koerper einer gezeichneten Modell-Maske, die keine Maske verdeckt - gesamt <= 40 276 und je Stage (Stage 3 == 0); Gegenprobe Stand vor NB3: 1 360 606 | PASS |
| `integration_r41_masken36_fern` (neu, Szenen test_r41_masken36_fern_szenen.cmake, Werkzeug probe_r41_masken36_sicht) | Mangel 1 an der exe: hinter der Trennwand 30A0/30A1 Cut 0/1 an den Abnahme-Stellen im Trennwand-Fenster kein Figurpunkt im Endbild (Art Z, V = A != A0 mit A0 = Masken ohne Figur) - s/u/t/h30A0_00, e30A1_00, h30A0_01; davor 0 uebermalt (d30A0_00/01, f30A1_00) | s. Lauf |
| `unit_r41_masken36_zellen3d` (Messwert) | 1025 Modell-Zeilen byte-genau im RDT, MSK-Tiefe == Zeile, Engine zeichnet jede; Ausschluss MIT 3D == Tabelle (48 Cuts); Gegenprobe ohne 3D-Urteil >= 127 Rechtecke weg (NB2: 79) | PASS |
| `unit_r41_masken36_loecher` (Messwert) | Kerben NACHHER 9 036 von 63 116 = 14,3 % (NB2 16,0 %) | PASS |
Laeufe (Spurbaum, `local_build.sh build` nach der letzten Aenderung):
1. `ctest --test-dir re15_port/build -R "unit_r41_masken36|unit_r37_masken36|unit_pri_|unit_r36_pri|unit_r37_pri1100|unit_sld_atlas|unit_r38_itemorte|unit_r39_optik|unit_r34n_e_dokumente|unit_bss|prop_bss|unit_stage6_hintergrund" -j 3 --timeout 1800 --output-on-failure`
   -> **`100% tests passed, 0 tests failed out of 32`** (unit_r37_masken36_ausschluss 490 s, unit_r41_masken36_fern 253 s, zellen3d 122 s).
2. Integration: s. "Tests NB3 - Integrationslauf" unten.

### Tests NB3 - Integrationslauf
Ausdruck (alle Masken-/PRI-Riegel der Spur und die Nachbar-Riegel, die dieselben MSK/Hintergruende lesen):
`ctest --test-dir re15_port/build -R "integration_r41_masken36|integration_r37_masken36|integration_pri_masken|integration_r36_pri|integration_r37_pri1100|integration_r38_itemorte|integration_r39_optik|integration_r34n_e_dokumente|integration_r35_zgirl_5090|integration_r38_affen3|integration_r35_granate|integration_r37_elzaems_elza5091|integration_r36_dachkiste|integration_r37_beretta|integration_r37_itemband" -j 3 --timeout 2400 --output-on-failure`
1. Lauf 1 (Stand k6): **`96% tests passed, 1 tests failed out of 27`** - rot nur `integration_r41_masken36_zellen3d` (d6000_05 550 von 1561,
   d6000_05t 477 von 1643, e6001_05 265 von 1241 uebermalt); gruen u.a. integration_r41_masken36_stage2 (Punkt 2, die 53 Nutzer-Masken),
   _zelle, _fern, integration_r37_masken36_stage3..6 / _nb2, integration_pri_masken, r37_pri1100, r38_affen3, r39_optik, r36_pri_schatten.
   Ursache und Berichtigung: Abschnitt "NB3 Riegel zellen3d" (die drei Plaetze stehen hinter dem Wandblock; nur die Szenendatei geaendert).
2. Lauf 2 (Szenen berichtigt; Neubau `local_build.sh build` -> `ninja: no work to do`, `=== LOCAL-BUILD-OK (build)`, exe md5 597ba53a
   unveraendert): `ctest --test-dir re15_port/build -R "integration_r41_masken36_zellen3d" --timeout 1500 --output-on-failure`
   -> `0% tests passed, 1 tests failed out of 1` - die drei Art-M-Szenen gruen, rot die neue DAVOR-Szene d6000_05a mit 3 von 2704 Punkten:
   alle drei bei (219..220,128..129) im +4-Rand ueber Leons Kopf (Leon-Dreiecke ab y 132), Kante von Slot 5 (3D-Tor hinter der Lokfront).
   Fenster oben ohne Rand (Beleg in der Szenendatei).
3. Lauf 3 (derselbe Ausdruck): **`100% tests passed, 0 tests failed out of 1`** (507 s; 15 Szenen, d6000_05a 0 von 2648).
   Die uebrigen 26 Riegel aus Lauf 1 lesen dieselbe exe (md5 597ba53a), dieselben MSK/TIM und unveraenderte Szenendateien.

## OFFEN

0. **Stand Nachbesserung 2 (ersetzt die Zahlen von 1):** 1022 Rechtecke (Leon, Elza gleich) bleiben aus, 273 961 Punkte liegen nur
   in ausgeschlossenen Rechtecken (NB1: 341 352). Gruende: 682 kein Modell (keine Sperrzelle mit der Mehrheit der Bodenkontakte -
   ueberwiegend Waende/Boeden/Decken), 105 Modell aber ein verdeckter Figurpunkt nicht hinter dem Koerper bzw. Kamera ueber der Zelle,
   70 Sektion ohne Stehplatz (3050 C15), 66 Zelle trifft bei keiner Hoehe, 62 kein Bodenkontakt, 37 Saeule mit schraeger Zelle.
   Restliste mit Bildern: `M_bilder/NB2_rest/NB2_rest_ROOMxxxx.png` (gruen gezeichnet, blau durch NB2, rot weiter aus, Nummern =
   Objekte) und `NB2_rest_liste.txt` (Cuts >= 300 rote Punkte, 272 296 Punkte). Die echten Gegenstaende darunter stehen unter
   "Fuer den Nutzer".
1. **Weiter ausgeschlossene Objektteile (Punkt 1, Stand Nachbesserung 1):** nach der Zellen-Tiefe (Schritt 1 + 2) bleiben
   530 von 3239 alten und 656 von 4300 Saum-Rechtecken (Leon; mit Elza 1 312 von 8 600 Saum) aus, 341 352 Bildpunkte liegen nur in
   ausgeschlossenen Rechtecken (vor NB1 450 317), 99 Objekte teilweise und 47 gar nicht gezeichnet (vor NB1 147 / 55).
   Restliste mit Bildern: `M_bilder/NB1_rest/NB1_rest_ROOMxxxx.png` (je Raum die Cuts mit >= 300 roten Punkten: gruen =
   gezeichnet, blau = durch NB1 wieder gezeichnet, rot = weiter aus) und `NB1_rest_liste.txt` (52 Cuts, 339 719 Punkte).
   Gruende (tiefe_zelle_r41.json): keine Sperrzelle traegt die Mehrheit der Bodenkontakte (697 Rechtecke) - vor allem Waende,
   Boeden und Hintergrund, die der Generator in seine Region nahm (R37 §3.2; z.B. 4040/4080 Gangwaende, 3050 C7 Bodenflaeche):
   die bleiben zu Recht aus; Zelle trifft das Objekt bei keiner Hoehe / kein Bodenkontakt / Saeule mit schraeger Zelle / Kamera in
   der Zelle; Zellen-Tiefe nicht ferner als die alte; und 135 Rechtecke mit Zellen-Tiefe, die der Ausschluss trotzdem nimmt.
   Wo im Spiel eine Verdeckung an einem echten Objekt fehlt, ist eine Freistellung des Nutzers wie in STAGE1/2 der Weg
   (s. "Fuer den Nutzer"). Eine Tiefe aus der Erreichbarkeit bleibt verworfen (1.5).
2. **Masken wirken auch auf 3D-Objekte und Effekte**, nicht nur auf Figuren: die Erreichbarkeits-Pruefung (R37) und die
   Standplatz-Schiene (R40) kennen nur Figuren. Gemessen: Kanten der 3D-Tuermodelle in 4040 C11 / 4080 C7 werden vom
   Tuerrahmen gedeckt (vorher schon 448 Punkte an der Tuer-Oberkante, Runde 41 +1/+2). ROOM2020 C3 rechte Reling ueber der hellen
   Wolke: von der Abnahme 0 nachgerechnet (Wolke hinter der Reling, kein Fehlerbild).
3. ~~ROOM2030 C15 Handlauf an Leons Schulter~~ - widerlegt (NB1 Mangel 3): es war die Spinne hinter Leon.
4. **PSX:** die Abnahme 0 hat gezeigt, dass platform/psx MASKS/*.MSK gar nicht liest (nur entladen_pc.c) - keine PSX-Folge
   der Saum-Rechtecke oder der Zellen-Tiefen; der PSX-Weg der Port-Masken ist unveraendert offen (R37 OFFEN 5).
5. **Uebersichtsbilder pri/STAGE3-6:** 204 von 214 vom Orchestrator ersetzt (master 919cc1ed); die 10 fehlenden liegen jetzt
   unter `M_bilder/uebersicht/` (durchsichtig, NB1 Auflage). Durch NB1 Mangel 1 haben sich die Uebersichtsbilder der Cuts mit
   wieder aufgenommenen Rechtecken geaendert (git diff --stat analysis/befunde_runde41/M_bilder/uebersicht) - pri/ ist fuer die
   Spur tabu, das Ersetzen bleibt dem Orchestrator. **Nachbesserung 2** hat weitere 11 geaendert (Leon-Ordner; die Elza-Ordner xxx1
   bekommen dasselbe Bild, MSK bytegleich): STAGE3/3040/00, STAGE3/30A0/00+01, STAGE5/5020/00+06 (X-Kiste), STAGE5/5090/11+13,
   STAGE6/6000/05+07+08+09 (Lok).
6. **ROOM5100 C7, Leons Hand am unteren Bildrand** (-12030,-5300): das ALTE Generator-Rechteck 0 (Tiefe 64, x 0..43 y 154..240)
   verdeckt Leon-Dreiecke mit z 4164..4615 (120 Rasterpunkte; Altbestand R37, vor/nach Runde 41 gleich). Ob die Hand dort vor
   oder hinter dem Objekt liegt, ist nicht gemessen (keine Zellen-Tiefe fuer Rechteck 0, es ist nicht ausgeschlossen).
7. **Figurkasten 3000 x +-350 des Ausschlusses (R37) ist am Figurrand zu schmal:** an fuenf der 88 DAVOR-Stellen der
   NB1-Pruefung sind 1..23 Leon-Punkte (1-2 Bildspalten am Rand: Arm/Hand) von Masken uebermalt, die schon VOR NB1 gezeichnet
   wurden: 3040 C5 4 (altes Rechteck 50), 30A0 C4 2 (Saum-Rechteck 41, Runde 41), 5100 C2 1, 4080 C8 2, 5050 C1 23 (alte Rechtecke
   35/36 + Saum 64/72). NB1 hat daran nichts geaendert (neu 0, weg 0). Ein breiterer Kasten oder die Leon-Dreiecke selbst
   (RE15_TRILOG) im Ausschluss waeren der Weg; nicht in NB1 (die Tabelle aendert sich dann fuer alle 216 Sektionen).
8. **HINTER-Deckung der 53 Nutzer-Masken, Nachpruefung (NB1):** an den 43 HINTER-Stellen des Bau-Agenten (e4_c, Leon im Bild)
   mit den Leon-Dreiecken (RE15_TRILOG, alle Dreiecke, `m41nb1c/hinter_ana.py`): 21 Stellen ganz verdeckt (darunter jetzt 2040 C7
   878/878 und 2030 C7 312/312); an den uebrigen liegt die Maskentiefe der offenen Punkte im Median 2..44 Buckets HINTER Leon
   (groesste Mengen 11A0 C6 96 gegen 64, 20A0 C5 64/58, 2030 C2 56/51, 2050 C3 70/62, 2050 C1 63/52, 20A0 C12 164/141) - also
   kein Fall wie der Handlauf 2030/2040 C7 (Tiefe 391 gegen Leon 42..49). Ob an diesen Stellen Leon tatsaechlich HINTER dem
   Gegenstand steht (Standlinien-Modell) oder davor (Maskentiefe), ist nicht je Stelle mit einer Kollisionszelle belegt.
9. **NB2 - Modell da, Rechteck trotzdem aus (125 Rechtecke, 20 778 px):** der Koerper erklaert nicht jeden Punkt des Rechtecks (die
   Generator-Rechtecke sind voll, die Region reicht ueber den Koerper hinaus); ein verdeckter Figurpunkt an einem solchen Punkt liegt
   nicht hinter dem Koerper -> das ganze Rechteck bleibt aus. Groesster Fall 6000 C5 unterer Gittertraeger (Zelle x -7862..12578
   z -5674..-1634 Band 2, IoU 0,86; R13 4 020 px, davon 165 ausserhalb). Weg: die Atlas-Punkte ausserhalb des Koerpers je Rechteck
   durchsichtig machen bzw. das Rechteck teilen - aendert Atlas/Rechtecke (nicht nur Tiefen); nicht in NB2.
10. **[NB4 GESCHLOSSEN: die Band-0-Saaten waren Tueren mit falsch gelesenem Band, s. NB4 Mangel 2 (a)] NB2 - Band-0-Stehplaetze in ROOM6000:** das Netz der Spur P saet Band 0 aus Aot_set-Saetzen mit Band 0 ((2800,2750),
    (-4900,-8350), (1763,-10677), `test_r37_masken36 saaten 6000`) und fuellt die Gleisebene (40 044 Plaetze); ob der Spieler dort je
    steht, ist nicht gemessen. Die HINTER-Szenen 6000 (Band 0, im Grundriss der Lok) pruefen deshalb die Mechanik; im Spiel wirken
    die neuen Lok-Masken vor allem auf das 3D-Tor (OFFEN 2: Tor 4 500 hinter der Lokfront, Verdeckung geometrisch richtig).
11. **Beobachtung Altbestand NB1 (unveraendert):** 4040 C6/C7 - an Stehplaetzen hinter dem Mittelblock liegt Leons Kopf ueber der
    gemessenen Hoehe 3025 des Blocks (Strahl bei y -3110..-3200), die Maske deckt ihn trotzdem (die Region zeigt dort den Block,
    die Kunst ist hoeher als das IoU-Modell); 5 094 / 5 608 Gitterpunkte der strengen Gegenprobe `m41nb2/py/statisch.py`.

12. **NB3 - Figur dicht hinter der Trennwand:** an der Zusatzstelle l30A0_00 (-24843,15863), 488 hinter der Rueckseite der Zelle 6, sind 12
    von 1050 Leon-Punkten sichtbar - alle in einer Bildspalte (x 208), Leon-z zwischen Eintritt und Austritt des Koerpers: Leons Modell ragt
    dort in den Zellenkoerper (der Kollisionsradius 450 haelt nur den Mittelpunkt fern). Die Teilung richtet sich nach den Figurpunkten des
    Rasters (Mittelebene, Stufe bucket(vz - 450)); ein schmalerer Streifen an x 208 waere der Weg, der Platz im Cut (105) ist ausgeschoepft.
13. **NB3 - Verdachtsliste, nicht einzeln geprueft:** 12 der 41 Gruppen (Tabelle in "Sichtung der Verdachtsliste") zeigen Figurpunkte VOR
    dem eigenen Koerper hinter einer soliden Zelle im Grundriss - ob die Zelle davor dort wirklich verdeckt (Hoehe), ist nicht je Fall an der
    exe gemessen. Werkzeug: `test_r37_masken36 verdacht <raum> <cut> <rechteck>` (Stehplaetze), Bilder m41nb3/verdacht.
14. **[NB4: Band 0 geschlossen (Saaten); Traeger-Mitte Spalten 1..30 geteilt, 31..40 mit Grund offen = OFFEN 18] NB3 - ROOM6000 Band 0 (vgl. OFFEN 10):** 39 934 der 40 276 "zu fern"-Punkte liegen im Gleisbett unter dem Bahnsteig; die Teilung der
    Traeger-MITTE (6000 C5) nimmt der Ausschluss wegen eben dieser Plaetze wieder heraus. Ob der Spieler dort je steht, ist weiter nicht
    gemessen; wenn nicht, sind diese Punkte im Spiel ohne Wirkung. Dieselbe Rueckholung laesst im Gang hinter der Wandecke (Cut 5, m6000_05)
    Leons Rumpf unter R37 (Tiefe 176) sichtbar: 536 von 1574 Punkten hinter dem Block sind verdeckt.
15. **NB3 - Mass "zu fern" kennt nur Modell-Koerper:** fuer Rechtecke ohne Modell (Generator-Tiefe) gibt es kein Urteil "zu fern"; die
    Verdachtsliste (Grundriss) deckt diesen Fall nur ohne Hoehe ab.

16. **NB4 - platzierte Komponenten auf hoeheren Baendern:** die Tiefe der neuen Rechtecke folgt der Generator-Regel (Bodenkontakt je Spalte
    auf y = 0). Steht der Gegenstand auf einem hoeheren Band (3050 Band 4, 3040 Band 1, 5090 Band 1), liegt der Bodenpunkt auf y = 0 HINTER
    ihm: die Maske verdeckt dann weniger als moeglich (nie mehr - keine Uebermalung, an der exe gemessen 0 an 16 DAVOR-Stellen). Ein
    Band-genauer Bodenkontakt (Band der Stehplaetze unter der Spalte) waere der naechste Schritt.
17. **NB4 - Komponenten ohne Bodenkontakt:** der Roboterarm 50B0 C1 (7 Komponenten, 3448 Texel), zwei Gelaenderstuecke oben in 3050 C6
    (Komponenten 2/3, 527/335 Texel, Lage (136,-2)/(142,-2)) und Deckenlampen haben in keiner Spalte einen Punkt unter dem Horizont - die
    Generator-Regel gibt ihnen keine Tiefe, sie bleiben ungezeichnet (`m41nb4/pl_v1/platzieren_r41.json`, neu_px 0).
18. **NB4 - 6000 C5 Traeger-Mitte Spalten 31..40 (Rechteck 37, Tiefe 176):** die Zelle-12-Tiefe wuerde an 192 Band-2-Plaetzen im Gang
    oestlich der Ecke (x 14833..15433, z -4984..-4384) Figurpunkte verdecken, deren Sehstrahl das Prisma der Zelle 12 nicht trifft - der
    Traeger selbst hat keine Zelle (er steht an der Ecke und ragt wohl ueber sie hinaus). An der exe bleiben hinter dem Block sichtbar:
    k6000_05a 512, b 371, c 644, m6000_05 447 Leon-Punkte (vorher 964 / 617 / 876 / 1066).
19. **NB4 - weiter ohne Maske (gemessen):** Pulte/Schreibtisch 5050 C1/C2 (Kamera ueber dem Grundriss, Nahfeld-Sperre NB2), Lampe/Moebel vorn
    30A0 C0 (die 15 neu platzierten Rechtecke nimmt der Ausschluss R37: eine Figur VOR der Lampe wuerde ohne Gegenstand davor verdeckt),
    3040 C3 rechter Pfeiler (4743 Punkte, ebenso vom Ausschluss genommen), 154 der 1627 neuen Rechtecke insgesamt. Dort waere eine
    Freistellung des Nutzers (wie STAGE1/2) der Weg.

## Fuer den Nutzer

- **Masken STAGE3-6, Nachbesserung 4:** In den Bildern der Stages 3..6 steckten viele Gegenstaende, die der Masken-Generator vom
  2026-09-03 nie gefunden hatte, obwohl sie im Original-Vordergrundatlas des Spiels liegen - vor allem duenne Teile: Gelaender, Pfosten,
  Rohre, Saeulen, Lampen. Sie sind jetzt platziert und gezeichnet (437 Teile in 72 Kameras, +160 539 Maskenpunkte; zusammen zeichnet das
  Spiel jetzt 1 159 393 Generator-Maskenpunkte statt 983 624): z.B. die Gelaender in ROOM30E0 Kamera 6..9, ROOM3040 Kamera 3, ROOM3050
  Kamera 6, die Saeule in ROOM6010 Kamera 2 (Leon steht jetzt dahinter statt davor). Vorher/Nachher-Boegen:
  `analysis/befunde_runde41/M_bilder/NB4/NB4_bogen_ROOMxxxx_Cnn.png`, Spielbilder `M_bilder/NB4/NB4_exe_*.png`. Im Spiel geprueft: vor den
  neuen Gegenstaenden wird Leon an keiner der 16 gemessenen Stellen uebermalt.
- **ROOM6000 (Bahnsteig):** die Masken nahmen Ruecksicht auf "Stehplaetze" im Gleisbett unter dem Bahnsteig, die es im Spiel gar nicht
  gibt (ein Lesefehler meines Werkzeugs bei den Tueren). Behoben - dadurch sind u.a. der untere Teil der Lok (Kamera 7) und weitere Teile in
  Kamera 5/8 gezeichnet, und Leon ist hinter der Wandecke in Kamera 5 weitgehend verdeckt (ein schmaler Streifen am Traeger bleibt, s. unten).
- **ROOM30A0, Ostende der Buero-Trennwand:** dort fehlt KEINE Maske. Die graue Stirnplatte ist so weit gezeichnet, wie sie im Bild reicht
  (Bild `M_bilder/NB4/NB4_M1_30A0_ost_stirn.png`); rechts davon zeigt das Bild den Boden und die Schraenke HINTER Leon - dort ist Leon zu
  Recht zu sehen. Die Abnahme hatte die Kollisionszelle (die groesser ist als die Platte) als Gegenstand genommen.
- **Frage:** Fuer diese Stellen geben die Spieldaten keine sichere Tiefe her - sollen sie als Ihre Freistellungen (PNG unter
  pri/STAGEn/<raum>/, wie in STAGE1/2) kommen? ROOM5050 Kamera 1/2 (Pulte/Schreibtisch, die Kamera haengt darueber), ROOM30A0 Kamera 0
  (Lampe/Moebel vorn links), ROOM50B0 Kamera 1 (Roboterarm, haengt frei), ROOM6000 Kamera 5 (der Traeger an der Wandecke, schmaler Streifen).
- Die Uebersichtsbilder der 72 durch Nachbesserung 4 geaenderten Kameras liegen unter `analysis/befunde_runde41/M_bilder/uebersicht/`
  (Liste `M_bilder/NB4/NB4_zahlen.txt`) - das Ersetzen in pri/ bleibt dem Orchestrator (pri/ ist fuer die Spur tabu).

- **Masken STAGE3-6, Nachbesserung 3:** Die Buero-Trennwand in ROOM30A0 (Kamera 0 und 1, Leon und Elza) verdeckt jetzt die GANZE Figur
  dahinter - vorher waren die oberen Mattglas-Felder einer Wand weiter hinten zugeordnet, und man sah Leon/Elza ab der Huefte aufwaerts
  "durch" die Trennwand (Bild `analysis/befunde_runde41/M_bilder/NB3/NB3_M1_30A0_trennwand.png`). Lange, schraeg zurueckweichende
  Gegenstaende haben dafuer jetzt schmale Teilmasken mit eigener Tiefe (statt einer Tiefe fuer das ganze Stueck). Neu gezeichnet: die hohen
  Spinde in ROOM30A0 Kamera 3, der Gittertraeger vorn links in ROOM6000 Kamera 5, die Kiste vorn in ROOM3040 Kamera 9, Schaltkasten + Kiste
  in ROOM3040 Kamera 0, die obere Kiste des Stapels in ROOM5090 Kamera 13, der rechte Container in ROOM5020 Kamera 7 und die X-Kiste in
  ROOM5020 Kamera 12 (Vorher/Nachher-Bilder `M_bilder/NB3/NB3_bogen_ROOMxxxx_Cnn.png`, 25 geaenderte Kameras, kein Maskenpunkt
  weggefallen). Was weiterhin ohne Maske ist und warum: `M_bilder/NB3_rest/` (Pulte/Schreibtisch ROOM5050 und Moebel vorn ROOM30A0 Kamera 0 -
  die Kamera haengt direkt darueber; Lok-Unterteil ROOM6000 Kamera 7 und Roboterarm ROOM50B0 - dafuer gibt es keine Kollisionszelle;
  die Gelaender ROOM3050 Kamera 6, ROOM30E0 Kamera 6..9, ROOM3040 Kamera 3 - die hat der Generator nie gefunden).
  **Frage:** Sollen die Gelaender (und ggf. die Pulte in ROOM5050) wie in STAGE1/2 als Ihre Freistellungen (PNG unter pri/STAGEn/<raum>/)
  kommen? Aus den Spieldaten allein ist dort keine sichere Tiefe zu bekommen.
- Uebersichtsbilder pri/STAGE3-6: durch NB3 geaendert sind die Bilder der 25 Kameras in `M_bilder/NB3/NB3_zahlen.txt` (neu unter
  `M_bilder/uebersicht/`, Leon-Ordner; Elza-Ordner nur, wo es sie gab) - das Ersetzen in pri/ bleibt dem Orchestrator (pri/ ist fuer die Spur tabu).
- **Die Loecher in den STAGE3-6-Uebersichtsbildern waren ein Fehler meines Werkzeugs**, nicht der Masken (Entpacker las jeden
  langen Rueckverweis 65536 Byte zu weit vorn; behoben, 359 von 359 Atlanten bytegleich mit dem Spiel). Die 204 Bilder sind
  ersetzt; die letzten 10 (3040/3041 01, 3050/3051 06 und 15, 30E0/30E1 06 und 07) liegen unter
  `analysis/befunde_runde41/M_bilder/uebersicht/` - dort zeichnet das Spiel gar keine Maske, deshalb sind sie leer (durchsichtig).
  Die Uebersichtsbilder der Cuts, in denen Nachbesserung 1 Masken dazugenommen hat, sind ebenfalls neu (gleicher Ordner).
- **Masken STAGE3-6 weiter geschlossen (Nachbesserung 1):** Stuecke von Objekten, deren Masken bisher ganz wegfielen (weil die
  automatische Tiefe eine Figur VOR dem Objekt uebermalt haette), tragen jetzt die Tiefe ihrer Kollisionszelle - so wie Ihre
  STAGE1/2-Freistellungen gebaut werden. 367 Rechtecke sind wieder drin (+109 000 Bildpunkte, z.B. die Maschine links in
  ROOM3050 Kamera 5; die meisten Punkte in 3050, 5020, 5100, 4040, 30A0). An der exe geprueft: an 21 Stellen hinter diesen
  Objekten ist Leon jetzt verdeckt (6 190 statt 389 Punkte), an 88 Stellen davor wird er an keiner zusaetzlich uebermalt.
  Insgesamt zeichnet das Spiel jetzt 889 591 Maskenpunkte der Generator-Masken (Runde 40: 727 561), weggenommen wurde nichts.
- **Was weiter offen ist (Restliste `analysis/befunde_runde41/M_bilder/NB2_rest/NB2_rest_ROOMxxxx.png`, rot = ohne Maske):** das
  meiste davon sind Waende, Boeden und Decken, die der Generator 2026-09-03 faelschlich ausgewaehlt hat - die bleiben zu Recht weg.
  ECHTE Gegenstaende ohne Maske, fuer die die Daten keine sichere Tiefe hergeben (keine passende Kollisionszelle oder der
  Gegenstand ragt ueber seine Zelle hinaus) - hier braeuchte es Ihre Freistellung wie in STAGE1/2 (PNG unter pri/STAGEn/<raum>/):
  * ROOM30A0 Kamera 3: die hohen grauen Spinde rechts (groesster Rest, ~10 000 Punkte)
  * ROOM6000 Kamera 5: unterer Teil des Gittertraegers vorn links; Kamera 7: unterer Teil der Lok links
  * ROOM5050 Kamera 1: Schreibtisch mit Monitor und Stuhl links, Pult vorn rechts; Kamera 2: Pult vorn unten
  * ROOM5090 Kamera 13: oberer Teil des Kistenstapels in der Mitte
  * ROOM3040 Kamera 0: Schaltkasten oben / Kiste unten (Reste); Kamera 9: Kiste vorn
  * ROOM30A0 Kamera 0: Lampe/Stuhl/Moebel vorn links am Bildrand
  * ROOM50B0 Kamera 1/3: der Roboterarm oben
  * ROOM5020 Kamera 7: Kiste rechts oben; Kamera 12: Unterseite der Kiste
  * Gelaender, die der Generator nie erfasst hat: ROOM3050 Kamera 6, ROOM30E0 Kamera 6/7/8/9, ROOM3040 Kamera 3 (Stueck)
- **Masken STAGE3-6, Nachbesserung 2:** weitere Gegenstaende sind jetzt geschlossen - die Lok in ROOM6000 (Kameras 5, 7, 8, 9) samt
  oberem Gittertraeger, die rechte X-Kiste mit Kufen in ROOM5020 Kamera 6 (fehlte im Uebersichtsbild), der Treppenraum in ROOM5020
  Kamera 0, die Buero-Trennwand in ROOM30A0 Kamera 0/1, Kisten in ROOM5090 Kamera 11/13, der Schaltkasten in ROOM3040 Kamera 0.
  Regel: eine Maske wird gezeichnet, wenn jeder Figurpunkt, den sie verdeckt, vom Kamerablick aus HINTER dem Koerper ihres
  Gegenstands liegt (Kollisionszelle des Raums, Hoehe am Bild gemessen); fuer 12 Gegenstaende, deren Zelle die Automatik nicht fand,
  habe ich die Zelle von Hand zugeordnet (mit Bild und Zellbytes: `re15_port/tools/maske/zuordnung_r41.json`). Wo die Kamera direkt
  UEBER dem Gegenstand haengt (Pulte vorn in ROOM5050), traegt das nicht - die bleiben ohne Maske. Im Spiel geprueft: VOR diesen
  Gegenstaenden wird Leon nirgends sichtbar uebermalt, dahinter ist er verdeckt (berichtigt in Nachbesserung 3: an der Buero-Trennwand
  ROOM30A0 nur unten - oben blieb er sichtbar; behoben, s. unten). Zusammen zeichnet das Spiel jetzt 956 982
  Maskenpunkte der Generator-Masken (Nachbesserung 1: 889 591, Runde 40: 727 561).
- **Kameras ohne Maske (3040 C1, 3050 C6, 30E0 C6, 30E0 C7):** dort hatte der Generator nur Boden bzw. die Rueckwand gefunden - als
  Maske nutzlos, deshalb zu Recht nichts. Die GELAENDER dort (3050 C6, 30E0 C6/C7/C8/C9) hat der Generator nie erfasst.
- **Ihre 53 STAGE2-Masken:** an der exe geprueft. Zwei Fehler gefunden und behoben: in **ROOM2040 Kamera 7** und **ROOM2030
  Kamera 7** verdeckte der Handlauf des Gelaenders Leon/Elza HINTER dem Gelaender nicht (der Handlauf bekam eine viel zu ferne
  Tiefe); jetzt kommt die Tiefe aus der Gelaenderzelle, Leon hinter dem Gelaender ist verdeckt (Bilder
  `M_bilder/NB1_M4_ROOM2040_C07_nachher.png`, `NB1_M4_ROOM2030_C07_nachher.png`). In ROOM2030 Kamera 7 ist Ihre Freistellung 07_01
  dafuer an der Gelaenderecke in zwei Teile geteilt (Nord- und Ostgelaender, Ihre PNG bleibt unveraendert). ROOM2030 Kamera 15:
  die Reling uebermalt Leon/Elza NICHT - was die Abnahme dort gezaehlt hatte, war die Spinne hinter Leon. ROOM20A0 Kamera 12,
  12_02 bleibt weg (Ihr Entscheid "Weglassen ist ok"). ROOM2020 Kamera 3 an der Kurbel: Reling ueber Leons Beinen ist richtig.
- Assets fuer das Paket: `re15_port/shared_assets/PSX/MASKS/` - die 30 ROOM3040..6031.MSK (nur Tiefenwerte geaendert, NB1),
  216 ROOMxxxx_PRIcc.TIM (Runde 41), ROOM2030.MSK + ROOM2030_PRI07.TIM/.STAND, ROOM2040.MSK + ROOM2040_PRI07.TIM/.STAND (NB1),
  ROOM20A0.MSK + ROOM20A0_PRI12.TIM/.PBM/.STAND (Runde 41); Nachbesserung 2: ROOM3040/3041, 30A0/30A1, 5020/5021, 5090/5091,
  6000/6001.MSK (nur Tiefenfelder) und `re15_port/engine/src/gen/masken36_ausschluss_r37.inc`. Neu (nur Werkzeug/Test):
  `re15_port/tools/maske/zellen3d_r41.py`, `re15_port/tools/maske/zuordnung_r41.json`, `re15_port/tests/unit/gen_r41_masken36_zellen.inc`.
  Keine neuen Quelldateien in der Engine (Android: kein neuer GLOB-Eintrag), keine Sprachdateien, keine neuen Schalter.
