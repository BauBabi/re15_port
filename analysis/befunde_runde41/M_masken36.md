# Spur M masken36 — Runde 41 (2026-10-24)

Auftrag (AUFTRAG.md, Wortlaut massgeblich):
1. "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher aufweisen."
   -> "danach kannst du deine Masken schliessen in der naechsten Version."
2. "Ich habe diverse neue PRIs fuer STAGE2 erstellt. die sollen alle sauber eingebaut werden!" (53 Masken in master
   5924939d) -> Sichtpruefung im Spiel; Orchestrator-Hinweis 2026-10-24 03:21: ROOM2020 Cut 3 Reling ueber Leons
   Beinen an der Kurbel (Pos_set @0x0A8C (3650,-1800,-2925)), dieselbe Pruefung fuer alle 53 Masken.

Baum: .claude/worktrees/r41_masken36 (Zweig r41/masken36, Basis b28c7236 = master-HEAD 4c667c97 + AUFTRAG-Hinweise).

## Stand
- [ ] Schritt 0: Pflichtlektuere (AUFTRAG, VERTRAG, UEBERGABE R40, P_masken36 R37, Memory Masken)
- [ ] Punkt 1: Messung vorher (Loecher je Cut), Ansatz, Umsetzung, Messung nachher
- [ ] Punkt 2: Sichtpruefung 53 Masken (11A0 + STAGE2), ROOM2020 Cut 3
- [ ] Tests (probes/r41_masken36.cmake), gezielte Laeufe

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

**(b) Tiefe aus der Erreichbarkeit statt Ausschluss.** Der Ausschluss (R37 masken36_r37.c) nimmt ein Rechteck GANZ weg,
sobald es an einem erreichbaren Stehplatz einen Figurpunkt ohne solide Zelle davor verdeckt. Gemessen (`b6_teilobjekte.py`):
398 Objekte (>= 64 px), 247 ganz gezeichnet, 57 ganz aus, **94 teilweise ausgeschlossen (358 358 px) = loechrige Objekte**.
Original-Urteil (R37 §3.3): verdeckt gdw. `depth < (1023*vz)>>16` (Figur-OT otz>>4 @0x8002565c, ZSF3 = 341 @0x80066c70,
Masken-OT = depth @0x80039650..60). Je Rechteck m ueber ALLE seine deckenden Punkte und alle erreichbaren Figurpunkte
darunter: falsch = keine Zelle davor (darf nicht verdecken -> depth >= bucket), echt = Zelle davor (soll verdecken ->
depth < bucket). Neue Tiefe = max bucket(falsch), wenn groesser als die Generator-Tiefe: verdeckt keinen falsch-Punkt und
jeden Punkt mit groesserem Bucket - also nie weniger als der Ausschluss (der verdeckt gar nichts). Wo das Figurraster
nicht pixelgenau ist (Kasten breiter als das Bild, Spalte halb hinter der Kamera) dieselbe Schranke wie R37 NB2 M2 als
Untergrenze. Werkzeug `test_r37_masken36 erzeuge_tiefe`, Laufzeit-Tabelle `engine/src/gen/masken36_tiefe_r41.inc`.

### 1.3 Umsetzung

| Datei | Inhalt |
|---|---|
| `re15_port/tools/maske/schliessen_r41.py` (neu) | baut je Raumpaar xxx0/xxx1 MSK + 216 `_PRIcc.TIM`. Probe 1: Rechteck in der Region; Probe 2: Deckung == alte Rechtecke + Region bitgenau; Probe 3: alter Teil jeder Sektion bytegleich (Gruppen + Rechtecke, nur Kopfzahlen und Anhang neu). Saum-Rechtecke: Grundstuecke = 8-zusammenhaengende Saumpunkte gleicher Spaltentiefe, gierig zusammengelegt (gleiche Verschiebung, Tiefenspanne <= Stufe, kleinster Flaechenzuwachs) bis 105 Rechtecke je Cut (RE15_PRI_MAX_MASKS_PER_CUT, re15_pri.h) erreicht sind; feinste Stufe aus (0, 1, 2, 3, 4, 6, 8, 12, 16) wie bau_p2.bau_cut. Ergebnis: 87 von 108 Sektionen Stufe 0 (jede Spaltentiefe genau), groesste Abweichung 12 Buckets (1 Sektion). Atlas: alte Rechtecke an ihrer BSS-Stelle (Texel = Hintergrund an der Zielstelle), Saum-Rechtecke an der ersten freien Stelle, je eine Gruppe (Anker = Ziel - Quelle), deckend nur an ihren Saumpunkten; 255 Farben Median-Schnitt, CLUT wie atlas.tim_aus (0,480, STP fuer Schwarz). |
| `re15_port/shared_assets/PSX/MASKS/ROOM3040..ROOM6031.MSK` + `ROOMxxxx_PRIcc.TIM` | 30 MSK (bytegleiche Paare Leon/Elza wie bisher), 216 TIM neu. 4300 Saum-Rechtecke (15 078 Rechtecke gesamt). |
| `re15_port/engine/src/masken36_r41.c`, `include/re15_masken36_r41.h` (neu) | Tiefen-Tabelle anwenden (Index vor dem Ausschluss) |
| `re15_port/engine/src/gen/masken36_tiefe_r41.inc` (neu, erzeugt) | 2966 Zeilen |
| `re15_port/engine/src/masken36_r37.c` | Haken (1 Zeile + include) am Anfang von re15_masken36_filter: Tiefe vor Ausschluss |
| `re15_port/engine/src/gen/masken36_ausschluss_r37.inc` (neu erzeugt) | nur noch ROOM3050/3051 Cut 15 (kein Stehplatz = Original-NULL-Sektion, R37 NB2 M1) |
| `re15_port/tests/unit/test_r37_masken36.c` | Generator-Sektion = Stage 3..6 (`generator_sektion`/`atlas_generator`, vorher "ohne eigenen Atlas"); Tiefen-Tabelle in der Nachrechnung wie der Lader; Gegenprobe "ohne Ausschluss" jetzt auch ohne Tiefen-Tabelle; Zensus: Stages 3..6 eigener Atlas (vorher BSS), Trennschaerfe-Gegenprobe nur fuer Freistellungen (der alte Teil der Generator-Sektionen passt durch beide Atlanten, Probe 3); Werkzeug `erzeuge_tiefe`, Riegel `tiefe` |
| `re15_port/tests/unit/probes/r41_masken36.cmake` (neu) | `unit_r41_masken36_tiefe` |
| `re15_port/tools/maske/original.py` | Entpacker-Fix (1.1) |

Der Lader braucht keine Aenderung: R15M MIT eigenem Atlas -> eigener Atlas (re15_pri_atlas_quelle, bg_pc.c), die Zeile
"Port-Masken ohne eigenen Atlas -> BSS-Atlas des Cuts (Runde 37 Spur P)" erscheint fuer die Stages 3..6 nicht mehr.

### 1.4 Messung nachher (2026-10-24, Bau 2cfb1158 + Riegel ded95f4b)

**Statisch, alle 108 Sektionen (Elza bytegleich), `re15_port/tools/maske/bilder_r41.py` -> `M_bilder/ROOMxxx0_Ccc.png`
(je Cut: Hintergrund | VORHER | NACHHER; magenta = schon vorher, gruen = neu geschlossen, rot = weg, cyan = eingeschlossene
Luecke, die offen bleibt) und `M_bilder/zahlen.txt`:** gezeichnet vorher 727 561 Bildpunkte -> nachher 1 199 780
(+472 219 = Saum + vorher ausgeschlossene Rechtecke mit Tiefe aus der Erreichbarkeit), weg 0. Die cyan-Flaechen sind echte
Zwischenraeume (Beispiel ROOM6010 C12: der Raum unter der Treppe zwischen Pfeiler, Wange und Bodenleiste; ROOM5020 C6:
Luecken zwischen den Kistenfuessen). Beispiele: ROOM3050 C1 (Konsole links: Treppenkante -> glatte Kante), ROOM6010 C12
(Treppe: Kanten zu, Wange oben links dazu), ROOM5020 C6 (die beiden Kisten waren GANZ ausgeschlossen, jetzt gezeichnet).
Berichtigte Uebersicht im Format von `pri/STAGE3-6/<raum>/<cc>_port.png`: `M_bilder/uebersicht/STAGEn/<raum>/<cc>_port.png`
(107 Bilder; pri/ ist laut VERTRAG §1.3 tabu - Ersetzen der alten, mit dem kaputten Entpacker gebauten Bilder ist eine
Orchestrator-/Nutzer-Entscheidung, s. "Fuer den Nutzer").

**Offene Kerben (Riegel `unit_r41_masken36_loecher`, ueber die Engine-Leser):** Kuenstler-Atlas deckend + farbgleich
direkt an der Kante eines alten Rechtecks, nicht gezeichnet: VORHER 67 242 (alte Rechtecke, BSS-Atlas, ohne Ausschluss -
das Beste, was bis Runde 40 je gezeichnet wurde) -> NACHHER 3 946 (5,9 %), Leon- und Elza-Dateien. Blit nachher: 2 399 560
Punkte, max. Mittel je Sektion 10,66 / 765 (eigener Atlas = Hintergrund an der Zielstelle).

**Tiefe aus der Erreichbarkeit:** 2966 Tabellenzeilen (Leon + Elza); davon 1372 alte Rechtecke, die ALLE vorher
ausgeschlossen waren (von 1444; die uebrigen 72 = ROOM3050/3051 Cut 15 bleiben ganz aus), 1594 Saum-Rechtecke; KEIN
vorher gezeichnetes Rechteck hat eine neue Tiefe (gegen die alte Tabelle geprueft). 768 Zeilen verdecken weiter Figuren
hinter Zellen, 2184 Rechtecke haben keinen erreichbaren Figurpunkt hinter einer Zelle (zeichnen, verdecken aber nichts
Erreichbares). Ausschluss-Tabelle jetzt nur noch ROOM3050/3051 Cut 15.

**An der exe (Framedump Bild 120, RE15_WINDOW_SCALE=1, Leon per RE15_DEBUG_JUMP + RE15_PLAYER_POS, RE15_FORCE_CUT):**
* R37-Szenen (Riegel integration_r37_masken36_stage3..6, nb2, alle gruen): HINTER 3050 C2 1203/1234 Figurpunkte verdeckt
  (R37: 1123/1234), 5020 C9 224/322 (199), 6010 C12 844/1440 (820), 4080 C5 880/883 (879); Elza 1228/1274, 240/316,
  656/1109, 633/636; DAVOR 0 in allen vier Stages (Leon und Elza), FREI 3050 C5 0, WEG M1 30A0/30A1 0.
* Sweep ueber ALLE 108 Leon-Cuts (Scratchpad `m41/py/d1_sweep.py`, Stellen aus `test_r37_masken36 suche 60`, A = mit
  Masken, B = RE15_NO_PRI; Figurpunkte = |B - Hintergrund| > 24 im Figurfenster - der figurlose Spielrahmen ist bitgleich
  zum probe_bg_dump-Bild, gemessen 0 Abweichung): **DAVOR 53 Szenen, 49 mit 0 uebermalten Punkten**; 4 mit wenigen
  Punkten, die NICHT Leon sind: v4040_11 (7 von 5915) und v4080_07 (34 von 5977) = Kanten des 3D-Tuermodells im Tuerrahmen
  (der Rahmen deckte die Tuer-Oberkante schon VOR Runde 41: 448 Punkte aus alten, gezeichneten Rechtecken), v5050_03
  (4 von 3316, Rand eines runden 3D-Objekts), v5100_07 (27 von 1890: Leons Hand am unteren Bildrand unter einem ALTEN,
  schon vorher gezeichneten Rechteck - kein Runde-41-Effekt). HINTER 44 Szenen: 19 385 von 28 457 Figurpunkten verdeckt.

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
