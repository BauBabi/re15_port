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
- [ ] Lektuere (AUFTRAG, VERTRAG, Abnahme 1, Dossier, git log master..HEAD) - Werkzeuge tiefe_zelle_r41.py, test_r37_masken36.c
- [ ] Mangel 1 (a): Ausschluss-Urteil der 135 Rechtecke formgenau in 3D
- [ ] Mangel 1: die vier Cuts ohne Maske (3040 C1, 3050 C6, 30E0 C6/C7) und die genannten Gegenstaende (3050 C6, 5020 C6, 6000 C5)
- [ ] Mangel 1 (b): Restliste fuer den Nutzer (Freistellung je Kamera)
- [ ] exe-Messung nachher, gezielte Tests, Abschluss-Commit

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
mit angeschnittener Unterkante (5090 C13 0,41), Objekte am unteren Bildrand (5050 C1/C2, 30A0 C0: 0,00) und Teile ueber einem
Hohlraum (5020 C0 Treppenwange, 3040 C0 Schaltkasten auf der Kiste: "kein Bodenkontakt").
**Weg:** je solcher Gegenstand die Zelle VON HAND (`re15_port/tools/maske/zuordnung_r41.json`, 16 Eintraege; Werkzeug
`zellen3d_r41.py --zuordnung`, `tiefe_zelle_r41.zelle_von_hand`/`koerper_geometrie`). Ausgewaehlt wurde je Objekt die Sperrzelle eines
begehbaren Bands, deren Koerper-Silhouette (gemessene Hoehe, IoU-Maximum wie raum.py "zelle") den Gegenstand SELBST zeigt -
Grundriss unter dem Fuss des Gegenstands, Vorderkante = Unterkante im Bild (Bild je Eintrag `M_bilder/NB2/NB2_zuordnung_*.png`:
links Objekt gelb/rot, Mitte Kandidat 1, rechts Kandidat 2). Die Hoehe wird an ALLEN Objekten derselben Zelle im Cut gemessen
(ein Gegenstand, den die Region in mehrere Komponenten teilt, hat eine Hoehe). Bei der X-Kiste lag Zelle 10 (IoU 0,63, Saeule)
knapp vor Zelle 12 (0,61) - Zelle 10 steht aber HINTER der Kiste (Unterkante im Bild hoeher), die Kufen stehen in Zelle 12
(Grundriss x -4875..-2275 z -15425..-12825, Bytes @RDT+0x00870 `28 0a 28 0a f5 ec bf c3 01 ff 00 03`). Nicht von Hand zugeordnet
(kein passender Koerper, IoU <= 0,23 oder Hoehe 25 = Bodenplatte): 6000 C7/C8/C9 (Lok-Unterteil/Treppe), 5100 C7, 5050 C1 Schreibtisch
links, 30A0 C3 Schraenke (Zelle 6/7 zeigen nicht den Schrank), 30A0 C1 Gegenstand am Boden. Ob ein Rechteck gezeichnet wird,
entscheidet weiter der Ausschluss mit dem 3D-Urteil - die Zuordnung liefert nur Koerper und Tiefe.
**3D-Urteil, Feinschliff:** ein Figurpunkt IM Koerper (Kamera draussen) zaehlt jetzt als verdeckt - die Strecke Kamera -> Punkt tritt
vor ihm in den Koerper ein (vorher "kein Urteil"; betraf 5090 C11 R0/R5/R89, je 1..3 Punkte an der Dreieckszelle).

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

## OFFEN

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
   Spur tabu, das Ersetzen bleibt dem Orchestrator.
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

## Fuer den Nutzer

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
- **Was weiter offen ist:** die roten Flaechen in `M_bilder/NB1_rest/NB1_rest_ROOMxxxx.png` (14 Raeume). Viel davon sind Waende,
  Boeden und Hintergrund, die der Generator 2026-09-03 faelschlich ausgewaehlt hat - die bleiben zu Recht weg. Wenn Sie dort an
  einem ECHTEN Gegenstand eine Verdeckung vermissen (Leon laeuft hinter etwas durch und bleibt sichtbar), ist eine Freistellung
  wie in STAGE1/2 der sichere Weg (PNG unter pri/ oder F9-Marke) - bitte die Kamera nennen.
- **Ihre 53 STAGE2-Masken:** an der exe geprueft. Zwei Fehler gefunden und behoben: in **ROOM2040 Kamera 7** und **ROOM2030
  Kamera 7** verdeckte der Handlauf des Gelaenders Leon/Elza HINTER dem Gelaender nicht (der Handlauf bekam eine viel zu ferne
  Tiefe); jetzt kommt die Tiefe aus der Gelaenderzelle, Leon hinter dem Gelaender ist verdeckt (Bilder
  `M_bilder/NB1_M4_ROOM2040_C07_nachher.png`, `NB1_M4_ROOM2030_C07_nachher.png`). In ROOM2030 Kamera 7 ist Ihre Freistellung 07_01
  dafuer an der Gelaenderecke in zwei Teile geteilt (Nord- und Ostgelaender, Ihre PNG bleibt unveraendert). ROOM2030 Kamera 15:
  die Reling uebermalt Leon/Elza NICHT - was die Abnahme dort gezaehlt hatte, war die Spinne hinter Leon. ROOM20A0 Kamera 12,
  12_02 bleibt weg (Ihr Entscheid "Weglassen ist ok"). ROOM2020 Kamera 3 an der Kurbel: Reling ueber Leons Beinen ist richtig.
- Assets fuer das Paket: `re15_port/shared_assets/PSX/MASKS/` - die 30 ROOM3040..6031.MSK (nur Tiefenwerte geaendert, NB1),
  216 ROOMxxxx_PRIcc.TIM (Runde 41), ROOM2030.MSK + ROOM2030_PRI07.TIM/.STAND, ROOM2040.MSK + ROOM2040_PRI07.TIM/.STAND (NB1),
  ROOM20A0.MSK + ROOM20A0_PRI12.TIM/.PBM/.STAND (Runde 41). Keine neuen Quelldateien in der Engine (Android: kein neuer
  GLOB-Eintrag), keine Sprachdateien, keine neuen Schalter.
