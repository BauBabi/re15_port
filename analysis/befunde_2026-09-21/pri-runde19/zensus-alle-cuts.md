# Zensus B — alle geschriebenen PRI-Cuts: welche sind noch "zu falsch"

Runde 19, 2026-09-21. Reine MESSUNG — es wurde kein einziges Masken-Asset geschrieben.
Bauverzeichnis `re15_port/build_r19_zensusallecuts`, Skripte in
`analysis/befunde_2026-09-21/pri-runde19/`, Zahlen in `build/p2/z*.txt|json`,
Bilder in `analysis/befunde_2026-09-21/pri-runde19/zensus-alle-cuts/`.

---

## 0. Kurzfassung

Nach drei Zwischenständen, von denen zwei sich selbst widerlegt haben, bleiben **fünf
Cuts mit einem belegten Tiefen-Defekt** und **vier Inventar-Befunde**. Alles andere, was
die bisherigen Messschienen als "zu falsch" gemeldet haben, war ein Artefakt der Schiene.

| # | Cut | Symptom am Bild | belegte Ursache | Schwere |
|---|---|---|---|---|
| B1 | ROOM2090 C3 + ROOM2091 C3 | ein durchgehender Betonsockel verdeckt rechts und verdeckt links nicht; Bruch bei x=223 | Tiefe bräuchte eine Bezugsebene **1588 Einheiten UNTER dem Boden** (54,4 % der 237 Spalten unmöglich) | hoch |
| B2 | ROOM2090 C2 + ROOM2091 C2 | dasselbe, gespiegelte Kamera | 48,1 % der 206 Spalten unmöglich, linker Teil +66..+69 Buckets zu fern | hoch |
| B3 | ROOM2090 C9 | die Figur bekommt mitten im Kanal ein Gitter-Stück ins Modell gestanzt, obwohl sie meterweit davor steht | EINE Tiefe 104 für das ganze Objekt; Boden-Geometrie sagt 173; **327 begehbare Standplätze** zu ≥ 50 % übermalt | hoch |
| B4 | ROOM2090 C8 + C9 (Silhouette) | die Maske ist 28..30 % größer als die Freistellung des Nutzers | zwei Schlüssel fehlen im Auswahl-Eintrag → `STD` liefert `oben=4, grow=1`; **bitgenau bewiesen** | hoch |
| B5 | ROOM10D0 C6 "Pflanze links" | die Blätter der Pflanze werden von der Figur durchschnitten | `"aufrecht": 170` setzt den Fuß 52..67 Bildzeilen über die unterste Silhouettenzeile → Ebene **+1963 bis +2576 unter dem Boden** | mittel |
| B6 | 224 von 323 R15M-Sektionen | die Maske zeichnet nichts | keine Atlasdatei `MASKS/ROOM####_PRI##.TIM` → `has_fg = 0` | mittel (kein Bildfehler, aber toter Bestand) |
| B7 | ROOM2091 C8/C9 | der Spiegelraum des Alligators hat zwei Masken weniger als sein Zwilling | `auswahl.json` hat für ROOM2091 **keinen** Eintrag | mittel |
| B8 | 15 Cuts, bis −23,35 % | dünne Gegenstände (Treppengeländer) sind 1 Punkt schmaler als freigestellt | `ALPHA_SCHWELLE = 110` (im Code als Heuristik gekennzeichnet) | niedrig bis mittel |
| B9 | 5 Cuts, bis 100 % | Kasten statt Freistellung | eigene `quader`-Objekte (ROOM1100 C1/C2 zu Recht, ROOM10F0 C4/C5 und ROOM10D0 C7 nicht) | mittel |

**Kern-Erkenntnis:** die 18 STAGE2-Cuts (ROOM2090 ×10, ROOM2091 ×8) sind eine
Werkzeug-Generation zurück. `raum.py:622` schickt **nur** `rid < 0x2000` durch die
Phase-2-Kette (Geometrie-Tiefe + Abnahme mit `VORverd == 0` als Schranke). STAGE2 läuft
weiter über die alte Kette: 20×20-Kachelraster mit Fenster-Median (`anwenden.py:145`),
`DEPTH_FACTOR = 0.90` (`geom.py:43`), Silhouetten-Aufweitung aus `STD` — und **ohne
Abnahme**. Alle drei schweren Befunde B1..B4 liegen dort.

---

## 1. Reproduktion

```bash
# Bauverzeichnis (nur Sonden, keine Tests geändert)
cmake -S re15_port -B re15_port/build_r19_zensusallecuts -G Ninja \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON
cmake --build re15_port/build_r19_zensusallecuts \
      --target probe_r16_pri_masken_audit probe_bg_dump

# Hintergründe (Engine-MDEC-Dekoder, 586 PPM)
re15_port/build_r19_zensusallecuts/tests/unit/probe_bg_dump.exe \
      re15_port/shared_assets/PSX build/bg_ppm

# Live-Dump: Kameramatrix, begehbare Bodenpunkte, Deckung und nächste Tiefe je Punkt
re15_port/build_r19_zensusallecuts/tests/unit/probe_r16_pri_masken_audit.exe 1000 2000 \
      > build/p2/dump_vorher.txt     # 35,8 MB, STAGE1
re15_port/build_r19_zensusallecuts/tests/unit/probe_r16_pri_masken_audit.exe 2000 7000 \
      > build/p2/dump_rest.txt       # 30,2 MB, STAGE2..6

# Messungen (Reihenfolge beliebig, jede schreibt ihr build/p2/z*.txt)
python analysis/befunde_2026-09-21/pri-runde19/z1_bestand.py            # Bestandsaufnahme
python analysis/befunde_2026-09-21/pri-runde19/z2_bitgenau.py           # Freistellung == Deckung
python analysis/befunde_2026-09-21/pri-runde19/z3_schiene.py            # Standplatz-Schiene, 2 Körper
python analysis/befunde_2026-09-21/pri-runde19/z9_gegen_kuenstler.py    # Nullmodell Künstler
python analysis/befunde_2026-09-21/pri-runde19/z15_vor_ohne_tor.py      # VOR ohne 95-%-Tor
python analysis/befunde_2026-09-21/pri-runde19/z16_hinten_mit_staffel.py
python analysis/befunde_2026-09-21/pri-runde19/z17_spanne_und_befund.py
python analysis/befunde_2026-09-21/pri-runde19/z19_2090_grow.py         # Beweis B4
python analysis/befunde_2026-09-21/pri-runde19/z20_alpha_verlust.py     # Beweis B8
python analysis/befunde_2026-09-21/pri-runde19/z25_tote_sektionen.py    # Beweis B6
python analysis/befunde_2026-09-21/pri-runde19/z26_unmoegliche_ebene.py # Beweis B1/B2/B5
python analysis/befunde_2026-09-21/pri-runde19/z27_aufrecht_diagonal.py
```

Alle Urteile benutzen die Original-Regel aus `re15_pri.h`:
**Maske verdeckt Figurpunkt ⟺ `depth < (1023*vz) >> 16`** (`@0x8002565c` `sra v1,v1,4`,
ZSF3 = 341 `@0x80066c70`/`@0x80066c74`; Masken hängen als letzte Prims vorne ein
`@0x8001ce54 jal 0x80039590`, gleicher Bucket ⇒ Figur obenauf).
Ein Bucket = 65536/1023 = **64,0625** Kamera-z.

---

## 2. Bestandsaufnahme (Auftrag 1)

### 2.1 Geschriebene Dateien (`build/p2/z1.txt`)

| Größe | Zahl |
|---|---|
| Cuts mit vollständigem Satz PBM+STAND+TIM | **77** |
| Cuts mit unvollständigem Satz (nur TIM) | 22 — ROOM10F0 C4/C5, ROOM1100 C1/C2, ROOM2090 ×10, ROOM2091 ×8 |
| `.TIM`-Dateien insgesamt | 99 |
| `.MSK`-Container | 54 |
| Sektionen in allen Containern | **323** |
| davon mit Atlasdatei (also überhaupt zeichenbar) | **99 (30,7 %)** |
| Räume mit geschriebenen Dateien | 23 |

### 2.2 Was speist sie (`auswahl.json`)

| Größe | Zahl |
|---|---|
| Cut-Einträge | 94 |
| Objekte insgesamt | 146 = **128 Nutzer-PNG + 18 eigene `quader`** + 0 polygon |
| Einträge mit leerer Objektliste (Sektion bewusst entfernt) | 3 — ROOM1000 C1, ROOM1000 C3, ROOM11F0 C1 |
| Räume ohne jeden Eintrag, aber mit Sektionen | ROOM2091 (8 Sektionen) und die 32 STAGE3–6-Räume (216 Sektionen) |

Die 18 Kästen: ROOM10D0 C7 (1), ROOM10F0 C4 (8), ROOM10F0 C5 (4), ROOM1100 C1 (3),
ROOM1100 C2 (2).

### 2.3 Original-Künstlermasken gegen nachgezeichnete (`z1.txt` §1c)

| Größe | Zahl |
|---|---|
STAGE1-Cuts mit Kamerasatz | 850 |
| nur ORIGINAL-Masken (dort ist das Original die Vorgabe) | **228** |
| nur NACHGEZEICHNETE Masken (dort ist die Freistellung die Vorgabe) | **89** |
| beides (Original gewinnt, nachgezeichnet tot) | **0** |
| gar keine Maske | 533 |

Räume mit Original-Masken (14 Paare): 1020/1021, 1030/1031, 1040/1041, 1070/1071,
1090/1091, 10B0/10B1, 1150/1151, 1170/1171, 1190/1191, 11B0/11B1, 11C0/11C1, 11E0/11E1,
1200/1201, 1210/1211.

Die Null in der Zeile "beides" ist kein Zufall, sondern Code:
`platform/pc/main.c:4936` fragt den R15M-Container **nur**, wenn das Original eine
NULL-Sektion führt (`pri_n == 0`). Eine nachgezeichnete Sektion kann eine
Künstler-Maske also nicht verdrängen.

### 2.4 Tote Sektionen (Befund B6, `build/p2/z25.txt`)

`platform/pc/src/bg_pc.c:182-190`: ist die Sektion nachgezeichnet, **muss** der Atlas
`MASKS/ROOM####_PRI##.TIM` sein (srcX/srcY sind u8, `@0x80039408`/`@0x80039418`). Fehlt
die Datei, wird `re15_render_pc_set_pri_atlas(NULL,0,0)` gesetzt und 0 zurückgegeben; in
`main.c:4976` verhindert `if (has_fg && pri_n > 0 …)` dann jedes Overdraw.

| Gruppe | Sektionen | mit Atlas | tot |
|---|---|---|---|
| STAGE1 (21 Räume) | 81 | 81 | 0 |
| ROOM1221 (Spiegel von ROOM1220) | 8 | 0 | **8** |
| ROOM2090 / ROOM2091 | 18 | 18 | 0 |
| STAGE3–6 (32 Räume) | 216 | 0 | **216** |
| **Summe** | **323** | 99 | **224 (69,3 %)** |

Tote Sektionen malen keine falschen Punkte — sie sind kein Bildfehler, aber der
Container behauptet Masken, die es nicht gibt. ROOM1221 ist der einzige
Spiegelraum in STAGE1, der überhaupt Sektionen hat; die 21 übrigen `####1`-Räume haben
gar keinen Container.

---

## 3. Bitgenauer Abgleich Freistellung ⟷ geschriebene Deckung (Auftrag 2)

Gelesen wird mit demselben Blit-Pfad, den die Engine benutzt (Sektion + Atlas,
`bg_pc.c pri_publish_tim` → `render_pc.c SDL_RenderCopy`), verifiziert gegen die Engine
mit 0 Abweichungen in 84 Cuts.

**Abdeckung der Messung** (`build/p2/z2.txt`): 91 geschriebene Cuts mit Auswahl-Objekten,
davon 77 mit `.PBM` (Soll), **81 mit Engine-Deckung im STAGE1-Dump**, 89 mit mindestens
einem Nutzer-PNG; die 18 STAGE2-Cuts in einem eigenen Lauf (`z10`).

### 3.1 Ergebnis STAGE1

**In allen 84 PNG-gespeisten STAGE1-Cuts ist die Deckung bitgenau die Vereinigung der
Freistellungen: `A\C = 0` und `C\A = 0`.** Keine einzige Abweichung. Auch `PBM` gegen
Engine-Deckung: `B\C = C\B = 0` überall.

Die einzigen `C\A > 0` kommen von **eigenen Kästen** (Befund B9):

| Cut | Freistellung A | Engine-Deckung C | C\A (Kasten) | Anteil |
|---|---|---|---|---|
| ROOM1100 C1 | 0 | 28160 | **28160** | 100 % |
| ROOM1100 C2 | 0 | 25211 | **25211** | 100 % |
| ROOM10F0 C4 | 2131 | 12068 | **9937** | 82 % |
| ROOM10F0 C5 | 3576 | 9285 | **5709** | 61 % |
| ROOM10D0 C7 | 3641 | 5435 | **1794** | 33 % |

ROOM1100 C1/C2 sind echte Wände — ein Kasten ist dort die richtige Form. Die
Stühle in 10F0 C4/C5 und der Klapptisch in 10D0 C7 sind es nicht.

### 3.2 Ergebnis STAGE2 — Befund B4, bitgenau bewiesen (`build/p2/z19.txt`)

```
   Cut  Schluessel im Eintrag     A=Freistellung  A'=oben4+grow1  C=Engine  A\C  C\A  A'\C  C\A'
     0  oben=0 grow=0                       2106            2106      2106    0    0     0     0
   ...  (C1..C7 ebenso, alle 0)
     8  oben=FEHLT grow=FEHLT               1082            1409      1409    0  327     0     0
     9  oben=FEHLT grow=FEHLT               2483            3181      3181    0  698     0     0
```

`A'\C == C\A' == 0` heißt: die geschriebene Maske **ist** bitgenau die aufgeweitete
Freistellung. Die Kette:

* `raum.py:92` — `STD = {"segments": 220, "oben": 4, "grow": 1, …}`
* `raum.py:447` — `oben = o.get("oben", e["oben"]); grow = o.get("grow", e["grow"])`
* `raum.py:448-454` — `oben` schiebt die Maske 1..4 Zeilen nach oben und vereinigt,
  `grow` weitet mit `ndimage.binary_dilation(iterations=1)`
* `raum.py:622` — `if rid < 0x2000: return main_p2(...)` → STAGE2 läuft nicht über die
  punktgenaue Phase-2-Kette
* `auswahl.json` — ROOM2090 C0..C7 tragen **je Objekt** `"oben": 0, "grow": 0`; C8 und C9
  (in Runde 8 ergänzt) tragen sie **nicht**

**Fix:** `"oben": 0, "grow": 0` in die zwei Einträge ROOM2090 C8/C9 (besser: `STD` auf
0/0 umstellen, wie Phase 2 es ohnehin tut) und die beiden Cuts neu bauen. Erwartete
Wirkung: −327 bzw. −698 Punkte.

---

## 4. Standplatz-Schiene über alle Cuts (Auftrag 3)

### 4.1 Abdeckung (`build/p2/z3.txt`)

| Größe | Zahl |
|---|---|
| Cuts mit nachgezeichneter Sektion UND Engine-Deckung (STAGE1-Dump) | **81 von 81** |
| davon mit `.STAND`-Sidecar (Definition S) | 77 |
| Definition E (aus dem Engine-Dump abgeleitet) deckt | 81 von 81 |
| STAGE2 in eigenem Lauf | 18 |
| Standplätze insgesamt (stehend, S) | 440 973, davon 55 484 mit Maskenberührung |
| Selbsttest gegen `abnahme.standplatz_schiene` (ROOM1000 C0) | plaetze/beruehrt/VORn/VORverd/HINTn/HINTfrei je **identisch** |

### 4.2 VOR-Verdeckung: **keine** in STAGE1 (`build/p2/z15.txt`)

Das 95-%-Tor in `abnahme.py` ist dort selbst als Heuristik gekennzeichnet. Ohne Tor, mit
Toleranz-Staffel, über alle 81 Cuts:

| Toleranz | Plätze mit q ≥ 0,5 (stehend) | mit q ≥ 0,5 (liegend) |
|---|---|---|
| 1 Bucket | 11 | 3 |
| 2 Buckets | 1 | 1 |
| 3 Buckets | **0** | **0** |
| 5 Buckets | **0** | **0** |

Bei Toleranz 3 gibt es in allen 81 Cuts **keinen einzigen** Platz mit q > 0,25. Die zwei
Cuts, die im 95-%-Tor auffielen (ROOM10A0 C2 mit 6, ROOM10D0 C6 mit 2 Plätzen, je
q = 1,00), liegen 1..2 Buckets von der Standlinie — genau dort, wo nach dem belegten
Mechanismus nicht die Tiefe, sondern die Prim-Reihenfolge entscheidet.

### 4.3 HINTER-Seite mit Spannen-Filter (`build/p2/z16.txt`, `z17.txt`)

Rohe "HINTER-frei %"-Quoten sind als Rangfolge unbrauchbar (§7, R2). Mit der Pflicht
*Kastendeckung ≥ 0,9*, *durchsichtiger Anteil ≥ 0,5*, *Figur > 5 Buckets hinter der
Standlinie* und *Tiefenspanne der Maske im Körperkasten ≤ 9 Buckets* (die an 107
Künstler-Cuts gemessene Streuung INNERHALB eines Original-Rechtecks):

| Cut | stehend | liegend | Defizit Median / Max (Buckets) |
|---|---|---|---|
| ROOM10D0 C8 | 0 | **270** | 13 / 18 |
| ROOM10A0 C1 | 62 | 133 | 13 / 69 (stehend) |
| ROOM10C0 C2 | 45 | 110 | 10 / 27 |
| ROOM10E0 C7 | **64** | 57 | 8,5 / 24 |
| ROOM10F0 C5 | 0 | 45 | 19,5 / 38 |
| ROOM10F0 C6 | 7 | 45 | 10 / 12 |
| ROOM10F0 C4 | 0 | 44 | 9 / 37 |
| ROOM1000 C0 | 4 | 31 | 15 / 16 (stehend) |
| ROOM1220 C0 | 28 | 0 | 5 / 7 |
| ROOM1100 C1 | 24 | 26 | 5 / 18 |
| ROOM10C0 C3 | 0 | **23** | **51 / 62** |
| ROOM1100 C2 | 10 | 8 | 4 / 11 |
| ROOM10D0 C6 | 0 | 2 | **26,5 / 36** |
| Summe | **292** | **809** | — |

Die Spannen-Staffel (5 / 9 / 20 Buckets) ist mitgemessen: 171 / 292 / 428 Treffer
(stehend). Die Rangfolge hängt also an der Grenze, die Spitze nicht.

⚠️ Auch diese Liste ist **noch keine Befundliste**: die Standlinie ist EINE Zahl je
Spalte und stammt aus derselben Tiefe, die geprüft wird. Der Nachweis läuft über §5.

---

## 5. Der belastbare Riegel: welche Cuts brauchen eine UNMÖGLICHE Ebene

Jede Schiene, die die Figur gegen die Standlinie stellt, hängt an einer Toleranz und an
einer Annahme über die Bodenebene. Beides hat in diesem Lauf Befunde erzeugt, die keine
waren. Dieses Maß dreht die Frage um und braucht **keine** Toleranz:

Die Kamera-z eines Punkts auf einer Ebene `y0` ist reine Geometrie (Kameramatrix aus dem
Engine-Dump, `re15_camera_build_view`):

```
vz(y0) = (4096*y0 + R1*t0 + R4*t1 + R7*t2) / (R1*sx + R4*sy + R7)
sx = (px+0,5-160)/H      sy = (py+0,5-120)/H
```

`vz` ist **linear** in `y0`. Zu jeder geschriebenen Tiefe `d` gehört also genau eine Ebene,
bei der die Geometrie diese Tiefe ergibt. Gefragt wird am **untersten Maskenpunkt jeder
Spalte** — dem Punkt, an dem ein stehender Gegenstand seine Standfläche berührt:

```
y0_nötig = ((d+0,5)*65536/1023 − vz(y0=0)) * n / 4096
```

* `y0_nötig ≤ 0` → der Gegenstand müsste auf dem Boden oder **höher** stehen: möglich
  (Treppe, Geländer, Plattform, Tischplatte). **Kein Befund.**
* `y0_nötig > 0` → er müsste **unter** dem Boden stehen. Das gibt es nicht. **Befund.**

### 5.1 Selbstprüfung des Maßes

Die zwölf Cuts mit der tiefsten benötigten Ebene sind genau die, deren Auswahl-Eintrag
eine erhöhte Ebene **deklariert** — und die Zahlen treffen sich:

| Cut | y0 nötig (gemessen) | `"ebene"` in auswahl.json | Differenz |
|---|---|---|---|
| ROOM1060 C0 | −15 519 | −15 400 | 119 |
| ROOM1060 C1 | −13 735 | −13 600 | 135 |
| ROOM10A0 C1 | −10 886 | −10 800 | 86 |
| ROOM1060 C2 | −8 275 | −8 200 | 75 |
| ROOM10A0 C2 | −7 067 | −7 200 | −133 |
| ROOM10A0 C5 | −5 433 | −5 400 | 33 |
| ROOM10A0 C0 | −17 200 | −14 400 | 2 800 |
| ROOM1060 C4 | −4 311 | −2 800 | 1 511 |

Sechs von acht treffen die deklarierte Ebene auf unter 140 Einheiten. Ein Maß, das das
leistet, ist keine Erfindung.

### 5.2 Ergebnis (`build/p2/z26.txt`, Abdeckung 99 Cuts)

| Raum | Cut | Spalten | med | p90 | max | Anteil y0 > +900 |
|---|---|---|---|---|---|---|
| ROOM2090 / ROOM2091 | **3** | 237 | **+1588** | +1908 | +2681 | **54,4 %** |
| ROOM10D0 | **6** | 67 | **+1963** | +2576 | +2576 | **53,7 %** |
| ROOM2090 / ROOM2091 | **2** | 206 | +187 | +1617 | +2475 | **48,1 %** |

**5 von 99 Cuts** haben mindestens eine unmögliche Spalte, **3** einen unmöglichen Median.
Alle übrigen 94 Cuts — inklusive aller STAGE1-Cuts außer ROOM10D0 C6 — sind mit einer
möglichen Ebene erklärbar.

Empfindlichkeit der Schranke (`build/p2/z26_staffel.txt`):

| Schranke | Cuts mit unmöglichem MEDIAN | Cuts mit ≥ 1 unmöglicher Spalte |
|---|---|---|
| +300 | 3 (10D0 C6, 2090 C3, 2091 C3) | 15 |
| +450 | 3 | 9 |
| +600 | 3 | 8 |
| +900 | 3 | 5 |
| +1800 | 1 (10D0 C6) | 5 |

Die Spitze ist über einen Faktor 3 der Schranke unverändert.

---

## 6. Die drei schlimmsten Fälle als Bild (Auftrag 5)

### 6.1 ROOM2090 C3 — ein Sockel, zwei Tiefenwelten
`zensus-alle-cuts/ROOM2090_C03_tiefenfarbe.png` (rot = nah, blau = fern),
`ROOM2090_C03_kopf1500.png` (Deckung + Rechtecke + beanstandete Standplätze)

Gemessen (`build/p2/z22.txt`):

```
   Spalte | unterste Zeile | geschr. Tiefe | bucket(vz Boden) | Differenz
       96 |            158 |           214 |              142 |       +72
      160 |            162 |           211 |              135 |       +76
      208 |            165 |           211 |              129 |       +82
      224 |            190 |            95 |               98 |        -3
      304 |            194 |            90 |               95 |        -5
```

Rechts von x = 223 stimmt die Tiefe auf 2..5 Buckets. Links davon trägt dieselbe
Betonstruktur 209..214 statt 129..142, also **4 600 bis 5 250 Kamera-z zu fern**. Im Bild
ist der Sprung als harte Farbgrenze (blau/rot) mitten durch einen durchgehenden Sockel zu
sehen; das 20×20-Kachelraster der alten Kette ist an den Rechteckkanten ablesbar.

**Was der Nutzer sieht:** er läuft hinter dem Sockel nach links — rechts verdeckt der
Sockel ihn korrekt, ab der Mitte läuft er plötzlich sichtbar *vor* dem Sockel, obwohl er
dahinter steht. Wirkung (Standlinie aus der Geometrie, Toleranz 5 Buckets): **704
stehende / 802 liegende** Standplätze, Körperkasten voll maskiert, ≥ 50 % sichtbar.

### 6.2 ROOM2090 C9 — ein Gitter, 4 400 Einheiten zu nah
`zensus-alle-cuts/ROOM2090_C09_tiefenfarbe.png`, `ROOM2090_C09_kopf1500.png`

Das ganze Objekt (3 181 Punkte, x 92..167, y 78..144) trägt **eine** Tiefe 104
(Schwelle vz 6 727), die Boden-Geometrie an seiner Unterkante sagt 173 (vz 11 116).
Gegenprobe (`z24`): das würde eine Ebene −2192 verlangen (1,2 Bänder hoch). Die erhöhte
Betonplattform des Raums liegt auf −1800; selbst gegen **die** ist die Tiefe noch
**13 Buckets = 833 Kamera-z zu nah**.

**Was der Nutzer sieht:** er geht mitten durch den Kanal, mehrere Meter vor dem Gitter an
der Rückwand — und bekommt dessen L-förmigen Umriss aus dem Körper gestanzt. Wirkung:
**327 stehende / 148 liegende** Standplätze, die meisten mit q = 1,00 (die Figur ist an
der Stelle vollständig übermalt).

⛔ Bemerkenswert und der Grund für §5: die bisherige Schiene meldet für diesen Cut
`VORverd = 0`. Sie leitet die Standlinie aus der geschriebenen Tiefe ab — ist die falsch,
wandert die Standlinie mit und die Schiene ist blind.

### 6.3 ROOM10D0 C6 — die Pflanze, deren Fuß 67 Zeilen zu hoch sitzt
`zensus-alle-cuts/ROOM10D0_C06_kopf1500.png`, `ROOM10D0_C06_kopf175.png`

Zwei Objekte (`build/p2/z18`-Lauf):

| Objekt | Lage | Punkte | Tiefe |
|---|---|---|---|
| "Stuhlkante (Nutzer-Original 06.png)", `aufrecht: true` | x 35..62, y 173..214 | 245 | 70..73 |
| "Pflanze links", `aufrecht: 170` | x 2..68, y 72..237 | 1 997 | 70..116 |

Die unmöglichen Spalten sind x 34..66 in den Zeilen 222..237 — das sind die **untersten
Blätter der Pflanze**, nicht die Stuhlkante:

```
   Spalte | Zeile | Tiefe | vz(y0=0) | Bucket | noetiges y0
       34 |   237 |   116 |     3996 |     62 |       +2576
       50 |   237 |   116 |     3996 |     62 |       +2576
       66 |   222 |   114 |     4416 |     68 |       +1963
```

`"aufrecht": 170` erzwingt EINEN Standpunkt in Zeile 170, während die Silhouette bis
Zeile 237 reicht — 67 Zeilen tiefer. Das ganze Objekt bekommt die Entfernung des
Bodenpunkts von Zeile 170 (vz ≈ 7 400) statt der von Zeile 237 (vz ≈ 4 000), also
**rund 50 Buckets zu fern**.

**Was der Nutzer sieht:** er steht hinter der Pflanze am linken Bildrand; die Blätter
werden von seinem Körper durchschnitten, statt ihn zu verdecken. Wirkung mit dem
liegenden Körper: 2 Plätze, Defizit-Median 26,5 und Maximum 36 Buckets.

Drei voneinander unabhängige Maße zeigen auf denselben Cut: das Nullmodell gegen die
Künstler (Median +46, 52,2 % der Spalten außerhalb der Künstler-Streuung), der
Ebenen-Riegel (§5) und die einzige VORverd-Meldung in STAGE1.

**Bezug zur Nutzer-Marke:** Marke 4 ist ROOM10D0 **Cut 7** (F3843). ROOM10D0 C6 ist die
Nachbarkamera derselben Ecke desselben Raums, und sein defektes Objekt steht unmittelbar
neben Tisch und Klappstuhl. Das ist kein Beweis, dass der Nutzer C6 gesehen hat — aber es
ist der einzige belegte Tiefen-Defekt in ganz STAGE1, und er liegt in dem Raum, aus dem
die Meldung kommt.

---

## 7. Was diese Messung WIDERLEGT hat

Diese Liste ist der wichtigste Teil des Dossiers: sieben Zwischenbefunde haben sich unter
einer schärferen Messung aufgelöst. Sie dürfen nicht wiederkehren.

**R1 — Die VOR-Seite von STAGE1 ist kein Befund.**
Die einzigen zwei Cuts mit `VORverd > 0` (ROOM10A0 C2: 6 Plätze, ROOM10D0 C6: 2 Plätze,
je q = 1,00) verlieren diesen Status bei einer Toleranz von 3 Buckets. Bei Toleranz 3
existiert in allen 81 Cuts **kein** Platz mit q > 0,25. Beide liegen 1..2 Buckets von der
Standlinie, wo nach `@0x8002565c` / `@0x8001ce54` die Reihenfolge und nicht die Tiefe
entscheidet. (`build/p2/z15.txt`)

**R2 — Die Rangfolge nach "HINTER-frei %" (z3/z4) ist keine Befundliste.**
Der schlimmste der 640 beanstandeten Plätze ihres Spitzenreiters ROOM1140 C0 liegt bei
Welt (−1200, −13400): Fußreferenz 3 573 gegen Standlinie 3 507, also **+66 Kamera-z =
+1,03 Buckets** "HINTER" — die Einordnung kippt an **zwei** Einheiten von 64,0625.
Gleichzeitig liegt die Figur an **jeder** ihrer 30 Bildzeilen näher als die Maske
(Figur-Bucket 51..54, Maskentiefe 54..64). Die richtige Darstellung ist: Figur davor.
Mit dem Spannen-Filter fallen ROOM1140 C0, ROOM1140 C2, ROOM11F0 C7 und ROOM10D0 C7 auf
**0** Treffer. (`build/p2/z14`-Lauf, `z17.txt`)

**R3 — Die "Bodenprobe" (z8) ist als Tiefenmaß disqualifiziert.**
Sie verglich den Bodenkontakt eines Gegenstands mit der nächsten **begehbaren** Bodenzelle
an derselben Bildstelle — Gegenstände stehen aber gerade dort, wo man nicht laufen kann.
Ergebnis: 57 von 59 Cuts systematisch negativ (Median −11 Buckets). Das Maß misst
Begehbarkeit. (`build/p2/z8.txt`)

**R4 — Die großen Nullmodell-Mediane von ROOM10A0 C0/C1/C2/C5 und ROOM1060 C0/C1/C2/C4
sind keine Defekte.**
Diese Objekte sind auf erhöhten Bändern deklariert (`ebene` −14400, −10800, −7200, −5400
bzw. −15400, −13600, −8200, −2800), während das Nullmodell die Ebene für alle Cuts bei
y0 = 0 rechnet, damit der Vergleich mit den Künstlern fair bleibt. Mit der deklarierten
Ebene fallen ROOM10A0 C0 von 1 821 auf 199 beanstandete Plätze und ROOM1060 C1 auf 0.
§5.1 zeigt die Übereinstimmung von gemessener und deklarierter Ebene auf < 140 Einheiten.
(`build/p2/z13`-Lauf, `z23_stage1.txt`, `z26.txt`)

**R5 — Eine Abweichung von der Bodenebene ist per se kein Defekt: die Künstler machen es
selbst.**
Unter derselben Regel haben **98,2 % der 228 Original-Künstler-Cuts** einen |Median| ≥ 2
Buckets; ihre Verteilung: Median −26, p10 −213, p90 +28, MAD 30 (36 244 Spalten).
Unsere 81 Cuts: Median −13, p10 −98, p90 +1, MAD 13, `|f| ≤ 1` in 20,8 % der Spalten
gegen 2,5 % bei den Künstlern. Unsere Tiefen liegen also **näher** an der Geometrie als
die der Künstler. (`build/p2/z9.txt`)

**R6 — Die Alpha-Schwelle schneidet keine ganzen Stücke weg.**
Über alle 89 Cuts mit Nutzer-PNG: die verworfene Menge hat **maximale Dicke 1** und es
gibt in **keinem** Cut eine abgesetzte Komponente ≥ 8 Punkte. Es ist ein 1-Punkt-Saum,
kein verlorenes Teil. (Er kostet trotzdem bis 23,35 % der Punkte eines Cuts, weil ein
Saum um ein 2–3 Punkte breites Geländerrohr fast das ganze Rohr ist — deshalb steht das
als B8 in der Befundliste, aber nicht als "fehlendes Stück".) (`build/p2/z20.txt`)

**R7 — An Marke 4 (ROOM10D0 C7, F3843) ist die Maske NICHT übermalt.**
Hintergrundbild und Bildschirmabzug stimmen außerhalb der Figur bitgenau (Median-Differenz
0 über alle 76 800 Punkte). Die 3D-Schicht überschrieb 956 Punkte (Schwelle 40), davon
liegen **0** innerhalb der Maskendeckung; bei Schwelle 8 sind es 94, alle im Helligkeits-
und Lagebereich des Figurschattens. Die Maskendeckung reicht von Spalte 0 bis 72, die
Kunst des Stuhls endet bei Spalte 72, rechts davon ist Boden. Von den 189 hellen
Metallrohr-Punkten der Stuhlecke sind 153 gedeckt; die 36 ungedeckten liegen in
Spalten 23..47 (Tischkante) und 84..91 (blauer Eimer), nicht am Stuhl.
Der messbare Fehlanteil dieses Cuts ist mein eigener Kasten (1 794 Punkte = 33 % der
Deckung), nicht ein fehlendes Stück der Freistellung.
Bilder: `ROOM10D0_C07_marke4_soll_gegen_ist.png`, `ROOM10D0_C07_zuschnitt_0_115.png`,
`ROOM10D0_C07_hintergrund_gegen_deckung.png`.

**R8 — Die Freistellungen des Nutzers sind nicht überschrieben worden.**
In allen 84 PNG-gespeisten STAGE1-Cuts ist die Engine-Deckung bitgenau die Vereinigung
der Freistellungen (§3.1).

**R9 — "Cut existiert im RDT nicht" für die 18 ROOM2090/2091-Cuts war mein eigener
Messfehler.** `z1_bestand.py` liest nur `dump_vorher.txt` (0x1000..0x2000);
`dump_rest.txt` enthält alle 18 Cuts mit Kameramatrix und Deckung.

**R10 — "Der Standpunkt liegt außerhalb der Silhouette" ist allein kein Defekt.**
6 von 8 `aufrecht`-Objekten haben ihren Einzel-Standpunkt außerhalb der eigenen
Silhouette (ROOM10C0 C2 +33 Zeilen, ROOM10D0 C1 +5, ROOM10D0 C7 +17, ROOM10D0 C9 +11,
ROOM10E0 C3 +11, ROOM10D0 C6 "Pflanze" −67). Nur der Fall mit −67 (der Fuß liegt ÜBER der
Silhouette) fällt im Ebenen-Riegel durch. (`build/p2/z27.txt`)

**R11 — "ROOM1000 C4 und ROOM10A0 C5 haben ein systematisch zu fernes Tiefenmodell" ist
so nicht belegt.** Nachgemessen: ROOM1000 C4 braucht Ebene −6 (also ≈ Boden, 0 %
unmöglich), Nullmodell-Median 0,0 mit 0 % außerhalb der Künstler-Streuung; ROOM10A0 C5
braucht −5 433 gegen deklarierte −5 400 (0 % unmöglich, 0 % außerhalb). Keines der vier
unabhängigen Maße bestätigt das Etikett. Was bleibt: ROOM1000 C4 hat 5 (stehend) bzw. 3
(liegend) Plätze mit Defizit-Median 1 Bucket.

**R12 — "ROOM10F0 C4/C5 decken hinten schlecht ab" ist bestätigt, aber die Ursache ist
die Kastenform, nicht die Tiefe.** Beide sind im Ebenen-Riegel unauffällig
(0 % unmöglich); C4 besteht zu 82 %, C5 zu 61 % aus eigenen `quader`-Kästen, und die
Treffer stehen mit dem liegenden Körper (44 bzw. 45) weit über denen mit dem stehenden
(0 bzw. 0).

**R13 — Eine nachgezeichnete Sektion kann keine Künstlermaske verdrängen.**
`main.c:4936` fragt den R15M-Container nur bei `pri_n == 0`. Gemessen: 0 Cuts mit
"beides". Die 224 toten Sektionen (B6) malen daher keine falschen Punkte.

---

## 8. Zweiter Körper: was die liegende Figur ändert (Auftrag 4)

Messgrößen: stehend Fuß..Kopf 1500, halbe Schulterbreite 450
(`test_pri_kopfschnitt.c:49-55`); liegend Fuß..Rücken **175**, halbe Breite 450 — die 175
sind die gemessene Wurzelhöhe der Liege-Pose (Clip 0x12 Frame 0 = kf400 `py = -175`
gegen Frame 97 = kf449 `py = -1744`, selbst geparste Keyframe-Rohdaten CDEMD0.EMS Blob
0xD5800, `enemy_ai_common.c:3904-3907`).

Der liegende Körper ist **nicht** nur "weniger": er sieht andere Cuts.

| Cut | stehend | liegend | warum |
|---|---|---|---|
| ROOM10D0 C8 ("Tischkante") | 0 | **270** | Kastendeckung stehend nur 0,294 — eine Tischkante kann einen stehenden Körper nie zu 90 % bedecken, einen liegenden schon |
| ROOM10C0 C3 | 0 | 23, Defizit-Median **51** Buckets | dieselbe Ursache |
| ROOM10F0 C5 | 0 | 45, Defizit-Median 19,5 | dieselbe Ursache |
| ROOM10F0 C4 | 0 | 44 | dieselbe Ursache |
| ROOM1120 C2 | 0 | 4 | dieselbe Ursache |
| ROOM1060 C1, ROOM1010 C6, ROOM10D0 C1, ROOM10D0 C6 | 0 | 2 / 2 / 1 / 2 | dieselbe Ursache |
| ROOM1220 C0 | 28 | 0 | umgekehrt: hohe, schmale Maske |
| Summe (Spannen-Grenze 9, Toleranz 5) | **292** | **809** | |

Ein Zensus, der nur den stehenden Spieler prüft, übersieht also die **acht** Cuts oben —
und mit ROOM10D0 C8 den mit Abstand plätzereichsten Fall überhaupt. Leichen und
kriechende Gegner liegen genau in dieser Höhe.

Für STAGE2 dreht sich das Bild teilweise: ROOM2090 C3 802 (liegend) gegen 704 (stehend),
ROOM2090 C9 dagegen 148 gegen 327.

---

## 9. Fixvorschläge (nichts davon ist in diesem Lauf geschrieben worden)

| # | Fix | Aufwand | erwartete Wirkung |
|---|---|---|---|
| B4 | `"oben": 0, "grow": 0` in die zwei Einträge ROOM2090 C8/C9 (oder `STD` global auf 0/0) und die Cuts neu bauen | klein | Silhouette wieder bitgenau die Freistellung: −327 / −698 Punkte |
| B1/B2/B3 | STAGE2 über `bau_p2`/`main_p2` bauen: Schranke `rid < 0x2000` in `raum.py:622` entfernen, Ebene je Objekt messen und eintragen, Abnahme (`VORverd == 0`, Deckung bitgenau) greifen lassen | mittel | die unmöglichen Ebenen verschwinden per Konstruktion; 704 + 485 + 327 Standplätze betroffen |
| B5 | `"aufrecht": 170` bei "Pflanze links" (ROOM10D0 C6) entfernen und die Spaltenregel nehmen, oder den Fuß auf die gemessene Topfzeile setzen | klein | Tiefe der unteren Blätter von 114..116 auf 62..68 |
| B7 | ROOM2091-Eintrag in `auswahl.json` anlegen (Spiegel von ROOM2090) und C8/C9 mitbauen | klein | zwei fehlende Masken im Spiegelraum |
| B6 | entweder die 224 Sektionen samt Atlas bauen oder die Container der 32 STAGE3–6-Räume und ROOM1221 entfernen | mittel | ehrlicher Bestand; kein Bildfehler |
| B8 | `ALPHA_SCHWELLE` je Objekt zulassen und für dünne Gegenstände (ROOM1060 C0/C1/C2/C4/C7, ROOM10D0 C1/C8) auf 1 setzen | klein | 1 Punkt Saum zurück; bei einem 3-Punkt-Rohr ein Drittel seiner Breite |
| B9 | ROOM10F0 C4/C5 und ROOM10D0 C7: Freistellung des Nutzers für die Stühle erbitten, Kästen ersetzen | Rückfrage | 1 794 / 5 709 / 9 937 Punkte, die heute Kastenform haben |

### Riegel

Dieser Lauf hat **keinen Port-Code geändert**, deshalb ist kein Riegel registriert worden.
Der Riegel, der hier hingehört, ist der Ebenen-Test aus §5: er braucht keine Toleranz,
keine Heuristik und keine Zielmenge, und er hätte alle drei schweren Befunde gefunden.
Als `ctest` formuliert:

> Für jeden Cut mit R15M-Sektion und Atlas: am untersten Maskenpunkt jeder Bildspalte die
> Ebene `y0_nötig` bestimmen. Der Test fällt, wenn der Median über die Spalten
> `> +900` liegt (ein halbes Band unter dem Boden).

Heutiger Stand dieses Riegels: **3 von 99 Cuts fallen** (ROOM10D0 C6, ROOM2090 C3,
ROOM2091 C3). Er kann deshalb erst grün registriert werden, wenn B1/B2/B5 behoben sind —
bis dahin wäre er ein absichtlich roter Test.

---

## 10. Vollständige Zahlentabellen

Alle `build/p2/z*.txt|json` sind zusätzlich unter
`analysis/befunde_2026-09-21/pri-runde19/messungen/` mitcommittet (1,4 MB); die beiden
Dumps (66 MB) und die 586 Hintergrund-PPM nicht — sie werden mit den Befehlen aus §1 in
wenigen Minuten neu erzeugt.

* `build/p2/z1.txt` — Bestandsaufnahme, Quellen je Cut, Rechteck- und Atlaszahlen
* `build/p2/z2.txt` — bitgenauer Abgleich A/B/C je Cut
* `build/p2/z3.txt` — Standplatz-Schiene, 2 Standlinien × 2 Körper, 81 + 77 Cuts
* `build/p2/z4.txt` — Ursachentrennung Bucket-Defizit / Kastendeckung
* `build/p2/z5.txt` — harter Spaltentest (6 245 + 4 008 Spalten)
* `build/p2/z6.txt` — Reichweite je Cut (0 tote Masken nach diesem Maß)
* `build/p2/z8.txt` — Bodenprobe (disqualifiziert, s. R3)
* `build/p2/z9.txt` — Nullmodell gegen 228 Künstler-Cuts
* `build/p2/z10.txt`, `z15_stage2.txt`, `z23_stage2.txt` — STAGE2
* `build/p2/z11.txt` — Alpha-Schwelle je Freistellung, die bekannten Punkte nachgemessen
* `build/p2/z12.txt` — Tiefen-Ausreißer gegen Wirkung
* `build/p2/z15.txt` — VOR-Seite ohne Tor, Toleranz-Staffel
* `build/p2/z16.txt`, `z17.txt` — HINTER-Seite mit Staffel und Spannen-Filter
* `build/p2/z19.txt` — Beweis B4
* `build/p2/z20.txt` — Beweis B8
* `build/p2/z21.txt`, `z22.txt`, `z24.txt` — STAGE2-Tiefenmodell im Detail
* `build/p2/z25.txt` — Beweis B6
* `build/p2/z26.txt`, `z26_staffel.txt` — Ebenen-Riegel und seine Empfindlichkeit
* `build/p2/z27.txt` — `aufrecht`-Standpunkte
