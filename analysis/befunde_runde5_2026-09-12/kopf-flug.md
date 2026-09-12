# BEFUND — Gruppe „kopf-flug": Der RE2-Kopf fliegt NICHT als intaktes Objekt — er ZERPLATZT im Zeichner

Nutzer 2026-09-12: „beim Schiessen nach oben fliegt immer noch der Kopf weg — neben der
Platz-Animation. Ich glaube beim Original RE2 fliegt der Kopf nicht weg."

Analyse 2026-09-12, alle Dumps frisch: `re2_disasm.py dis/read/table … --bin EMZ0.BIN`
(info/re2leon/COMMON/BIN) + PSX.EXE (info/re2leon), Decompiles RE2_Quellcode_V2.
Vorbefunde gelesen: befunde_runde3/kopf-wegschiessen.md, schrot_befunde/kopfschuss-nah.md,
befunde_runde4/gore-vollausbau.md.

## Kurzfassung (Wurzelursache)

**Der Nutzer hat im Ergebnis RECHT, aber der Mechanismus ist ein anderer als vermutet.**
Auch im RE2-Original bekommt Part 8 (Kopf) beim Schrot-hoch-Tod das Flug-Flagwort 0x4A und
die Drift-Kinetik — das Routing des Ports ist byte-true und bleibt UNVERÄNDERT. Die
Divergenz sitzt im ZEICHNER: ein Part mit Bit 0x08 wird im Original NIE über den normalen
Mesh-Renderer gezeichnet, sondern über `FUN_8002D3C8`/`FUN_8002D718` — und die zeichnen
jedes Dreieck/Quad des Kopf-Meshes **einzeln entlang seiner Vertex-Normale nach außen
versetzt** (Versatz wächst pro Flug-Frame über die Tabelle `DAT_8009DC28`: 30 → 465) und
**ersetzen alle Vertexfarben durch das dunkelrote Tint-Wort +0x70**. Sichtbar ist also eine
auseinanderberstende, dunkelrote Scherbenwolke, die aufsteigt, wegdriftet und nach 29 Frames
mitten in der Luft verschwindet — **kein intakter Kopf**. Der Port zeichnet stattdessen das
komplette, normal texturierte und beleuchtete Kopf-Mesh auf der Driftmatrix
(main.c:8401 + normaler Tri-Loop) — DAS ist der „wegfliegende Kopf", den der Nutzer sieht.

---

## 1. Frage 1 — Welche Todeszelle läuft beim SCHROT-nah-hoch-Tod? → 92C4 (bestätigt)

DEATH-Tabelle @0x8010CC24, Zeilen frisch gedumpt (`read`, 9 Spalten = 3 Brackets × 3 Zonen):

| Zeile (+0x5) | Spalten [0..8] |
|---|---|
| 5 (Magnum) @0x8010CCD8 | 7438, **92C4**, 0, 7438, **92C4**, 0, 7438, **92C4**, 0 |
| 6 (Magnum) @0x8010CCFC | identisch Zeile 5 |
| 7 (Pump-Schrot W8) @0x8010CD20 | 7438, **66FC**, 0, 7438, 8530, 0, 7438, 8530, 0 |
| 8 (SPAS W13) @0x8010CD44 | 7438, **8BEC**, 0, 7438, 9610, 0, 7438, 8530, 0 |

- Schrot-nah + Rumpf (Spalte 1) → `FUN_801066FC`. Deren Kopf, frisch disassembliert:
  - +0x231-Router: `bne v0,1` @0x80106740 → 0x80109610 (Burst); `beq v0,2` @0x8010675C →
    0x801067B4 → `jal 0x801092C4` @0x801067BC.
  - Phase 0 (+0x6==0, Verzweigung @0x8010677C): `lui 0x800d / lw v0,-1032` @0x8010679C-A0
    = Spieler-word0 @0x800CFBF8; `bgez v0,0x801067CC` @0x801067A8. Bit 31 (ZIELEN-HOCH)
    gesetzt → `sb 2,561(s2)` @0x801067B0 (+0x231=2) → `jal 0x801092C4` @0x801067BC —
    **im SELBEN Tick**. Folgeticks laufen über den +0x231==2-Router @0x8010675C wieder
    in 92C4; dort schaltet der eigene Phasen-Router (+0x6, `sltiu 0x5` @0x80109308,
    Tabelle @0x8010014C → 0x80109330/94F4/9534/9540/9568) durch die 5 Phasen.
- `FUN_80108BEC` (Zeile 8) läuft NUR für SPAS/GL/Rakete — **nie für die Pump-Schrot W8**.
  Das Port-Routing (enemy_ai_re2_zombie.c:5687-5715, re2z_rag231==2 → re2z_death_magnum)
  ist byte-true. **KEINE Routing-Änderung nötig.**

## 2. Frage 2 — Der 8BEC-Kopf-ab-Zweig: Part 8 fliegt dort AUCH (kein bloßes Unsichtbar)

`FUN_80108BEC`, Kopf-Zweig, alle Part-8-Stores (Record-Stride 0xAC, Part 8 = +1376):

```
80108dc8: lw  s0,408(s3)        ; +0x198 Part-Record-Basis
80108dd4: lw  v0,1376(s0)       ; Part-8-Flagwort
80108ddc: ori v0,v0,0x4a        ; |= 0x4A  (0x40 eigene Matrix + 0x08 Drift + 0x02)
80108de0: sw  v0,1376(s0)       ; KEIN Bit-0-Clear, KEIN sw zero!
80108dec: sh  400,1532(s0)      ; +0x9C Vortrieb
80108df4: sh  -100,1530(s0)     ; +0x9A vy (aufwärts)
80108dfc: sh  zero,1536(s0)     ; +0xA0 Flug-Timer = 0
80108e00: sh  s1(=10),1534(s0)  ; +0x9E vy-Zuwachs
80108e04: sh  -50,1540(s0)      ; +0xA4 Vortriebs-Decay
80108e08: sw  0x00101030,1488(s0) ; +0x70 TINT (dunkelrot; 92C4 nimmt 0x00101040)
80108e14: sh  yaw+2048,1528(s0) ; +0x98 Kurs (vom Spieler weg)
80108e18-2c: jal 0x8001bf10  a0=3000 (Blut 0,0,3000), Anker = Part-8-Matrix (s0+1448)
80108e30-3c: jal 0x8001cefc  (5,3,Part-8-Matrix) = Brand-Emitter am Kopf löschen
```

Es gibt **keinen part_mesh-Stempel für Part 8** und **kein Unsichtbar-Schalten** — dieselbe
Flug-Kinetik wie 92C4 (dort @0x80109460-98, Port-Zwilling enemy_ai_re2_zombie.c:5223-5245).
Danach @0x80108E40-94: `+0x21A & 0x20`-Gate, sonst Part 9 (Bein) `|= 0x1062` + Wurf-Felder
+0x38..+0x42 — das Bein fliegt als INTAKTES Mesh (s. §3, 0x1062 trägt Bit 0x08 NICHT).

## 3. Frage 3 — Was der Flug WIRKLICH zeigt: der Bit-0x08-Zeichner zerlegt das Mesh

### 3.1 Bit 0 bleibt während des Flugs gesetzt; Ende = ganzes Flagwort genullt

- Part-Loop `FUN_80027160` (RE2 EXE): verarbeitet einen Part NUR bei
  `(*flags & 1) != 0` (RE2_Quellcode_V2/FUN_80027160.c:67 und :81-85, Stride 0x2B Worte
  = 172 B). Bit 0 = DRAW/AKTIV-Gate; es bleibt beim Flug gesetzt (weder 92C4 noch 8BEC
  fassen es an, §2), sonst liefe die Physik nie.
- Flug-Ende in `FUN_80028DAC`: `lhu v1,160(s0)` @0x80028DC0 (+0xA0 Timer),
  `andi 0x7fff / sltiu v0,v0,0x1d` @0x80028DC8-CC; abgelaufen →
  **`sw zero,0(s0)` @0x80028DDC — das GANZE Flagwort wird genullt** (Bit 0 inklusive):
  der Part verschwindet mitten in der Luft, ohne Landung. Der Port hat das byte-true
  (enemy_ai_re2_zombie.c:4353-4357).

### 3.2 ⛔ DER KERNBEFUND: 0x08-Parts nimmt der normale Mesh-Renderer NIE

`FUN_80027434` (der Part-Zeichner, RE2_Quellcode_V2/FUN_80027434.c):

```
:157  if ((param_3 & 0x4000) == 0) {
:158    if ((param_3 & 0x18) == 0) {          // NUR dann: normaler Mesh-Render
:160      FUN_80027bec(...); FUN_80027dbc(...);   // Tri/Quad, NCCT-beleuchtet
:168    if ((param_3 & 8) != 0) {             // Flagbit 0x08 (im 0x4A enthalten):
:169      FUN_8002d3c8(...);                  //   ZERPLATZ-Renderer Dreiecke
:170      FUN_8002d718(...);                  //   ZERPLATZ-Renderer Quads
:172    if ((param_3 & 0x10) != 0) { FUN_8002da80/8002ddf0 }   // 0x10-Variante
:178-184  danach Physik: &8 -> FUN_80028dac, sonst &0x10 -> FUN_80028ea4
```

Reihenfolge beachten: **erst zeichnen (:169), dann Physik (:184)** — der Zeichner liest
den Timer VOR dem Inkrement dieses Frames.

### 3.3 Was FUN_8002D3C8/D718 zeichnen (der „explodierende Kopf")

Frisch disassembliert (EXE @0x8002D3C8; Quad-Zwilling @0x8002D718 identisch aufgebaut):

- **Timer-indizierte Versatz-Skala**: `lhu a1,160(a2)` @0x8002D3F8 (+0xA0),
  `andi 0x7fff` @0x8002D400 = Index, `srl a1,15` @0x8002D408 = Zeilenwahl;
  Tabellenbasis 0x8009DC28 @0x8002D470-74, Zeile × 60 B @0x8002D478-84
  (FUN_8002d3c8.c:35-39). Tabelle frisch gedumpt:
  - Zeile 0 (Timer-Bit15=0, von 92C4/8BEC gesetzt: `sh zero,+0xA0`):
    `[30, 59, 87, …, 450, 455, 459, 462, 464, 465]` (30 Einträge, @0x8009DC28)
  - Zeile 1: `[180, 354, …, 2790]` (@0x8009DC64)
- **Pro-Dreieck-Versatz entlang der Normale**: `lhu v1,0(t0)` (Normale des ERSTEN
  Prim-Vertex, n0) / `sll 16 / sra 26` (= signed >>10, Q12-Normale → ±4) / `mult v1,a0`
  (× Skala) @0x8002D490-A4; der Vektor wird per GTE rotiert und als Pro-Dreieck-
  Translationsmatrix gesetzt (`gte_ldlv0/cop2/gte_stlvnl/SetTransMatrix`,
  FUN_8002d3c8.c:50-63; Restore der Originalmatrix je Dreieck :78). Versatz frame 0
  = ±120 Einheiten, frame 29 = ±1860 — die Dreiecke des Kopf-Meshes **bersten
  auseinander** (Kopfradius ≈ 250).
- **Alle Vertexfarben = Tint-Wort, FLACH, unbeleuchtet**: `lw t2,112(a2)` @0x8002D508
  (+0x70) und `(tint & 0xFFFFFF) | 0x34000000` (GT3) bzw. `| 0x3C000000` (GT4)
  (FUN_8002d3c8.c:49 + raw @0x8002D504/14; FUN_8002d718.c:45); alle drei/vier
  Gouraud-Farben desselben Prims bekommen dasselbe Wort (FUN_8002d3c8.c:69-71).
  0x101040 = R 0x40, G 0x10, B 0x10 → Textur × (0.5, 0.06, 0.06): **dunkelrote Scherben**.
  Der NCCT-Beleuchtungspfad (@0x80027D10) läuft für diese Parts NICHT.

**Ergebnis im Original**: „Kopf ab" = 3 Hals-FX (Strahl/Brocken/Fontäne, gore-vollausbau
§2) + eine dunkelrote, auseinanderberstende Dreieckswolke, die aufsteigt (vy −100, +10/F),
wegdriftet (400, −50/F) und nach 29 Frames verschwindet. **Ein intakter, erkennbarer Kopf
fliegt zu keinem Zeitpunkt durch die Luft.** Gegenprobe Beine: Flagwort 0x1062 trägt
Bit 0x08/0x10 nicht (0x1062 & 0x18 == 0) → Beine laufen durch den NORMALEN Mesh-Renderer
(FUN_80027434.c:158-160) und fliegen intakt mit Aufprall (FUN_80028AD8) — deckt sich mit
dem RE2-Spielbild (Beine liegen im Raum, Köpfe/Arme zerplatzen).

## 4. Port-IST (die Divergenz)

- Routing, Kinetik, 29-Frame-Ablauf, Flagwort-Nullung: alle byte-true vorhanden
  (enemy_ai_re2_zombie.c:5687-5715, :5223-5245, :4348-4370).
- **ABER**: main.c holt sich nur die Drift-MATRIX (`re15_re2z_gore_part_matrix`,
  main.c:8401) und zeichnet dann das INTAKTE Kopf-Mesh im normalen Tri/Quad-Loop
  (main.c:~8490-8560) — texturiert, beleuchtet, mit Tint nur als NCCT-Modulation
  (RE2_GORE_TINT, main.c:8304-8312). Genau der Pfad, den das Original für 0x08-Parts
  überspringt. → Der Nutzer sieht einen kompletten Kopf durch die Luft fliegen, ZUSÄTZLICH
  zu den (inzwischen gebauten) Platz-FX.
- Betroffen sind ALLE 0x4A-Parts: Kopf aus 92C4 (Schrot-hoch/Magnum), Kopf aus 8BEC
  (SPAS-Zerreißer) und die Arm-Abrisse (Parts 3/5/6, ori 0x4A @0x80107544-48/
  @0x801075B4-B8/@0x80106630-34; Port :6037/:6046/:6054).

## 5. PATCH-PLAN (Datei:Zeile)

**Schritt 1 — Burst-Zustand exportieren** (enemy_ai_re2_zombie.c, bei den Physiken ~:4348):
Neue Query `int re15_re2z_gore_part_burst(const re15_actor_t *e, int bone_slot,
int32_t *out_scale, uint32_t *out_tint)`:
- bone→part via `re2z_bone_to_part`; Rückgabe 1 nur bei `(flags & 0x08) && (flags & 1)`
  (Gate-Beleg FUN_80027434.c:168 / FUN_80027160.c:81-85).
- Skala aus der portierten Tabelle `static const int16_t re2z_burst_scale[2][30]`
  (Werte oben, @0x8009DC28 / @0x8009DC64), Index `life & 0x7fff` (≤29 klemmen),
  Zeile `life >> 15` (@0x8002D3F8-408).
- ⛔ Frame-Ordnung: das Original zeichnet VOR dem Physik-Schritt (FUN_80027434.c:169
  vs :184); `re15_re2z_gore_part_matrix` steppt die Physik aber IM Matrix-Aufruf
  (:4425-4430). Deshalb in `re15_re2z_gore_part_matrix` VOR `re2z_part_phys_dac` den
  Timer nach `e->re2z_part_life_draw[part]` latchen (neues uint16-Feld, re15_actor.h
  neben re2z_part_life); die Query liest den Latch.
- `*out_tint = e->re2z_part_tint[part]` (+0x70, `lw t2,112(a2)` @0x8002D508).

**Schritt 2 — Zerplatz-Zeichnung im Renderer** (main.c, Tri-Loop ~:8490 und Quad-Loop
~:8550): vor der Prim-Emission `re15_re2z_gore_part_burst(npc, nbi, &bscale, &btint)`
abfragen; wenn 1:
- Pro Dreieck/Quad Versatz `d[k] = ((int)n0.k >> 10) * bscale` mit n0 =
  `nm->tri_normals[tri->n0]` (bereits im Loop vorhanden, main.c-Loop-Zeilen n0/n1/n2)
  — auf alle Vertex-Positionen des Prims im PART-Raum addieren, VOR der
  View-Transformation (äquivalent zum Original, das R×disp auf die Translationsmatrix
  legt, FUN_8002d3c8.c:58-63 — dieselbe Rotationsmatrix, identisches Ergebnis).
- Vertexfarben ALLER Ecken = flach `(btint&0xFF, btint>>8&0xFF, btint>>16&0xFF)` —
  OHNE Beleuchtung und OHNE RE2_GORE_TINT-Modulation (das Original ERSETZT die Farben:
  FUN_8002d3c8.c:49/69-71, FUN_8002d718.c:45); UV/Textur unverändert (0x34/0x3C sind
  texturierte Prims). Achtung Farbraum: PSX-Neutral ist 0x80 — die Werte 1:1 als
  Prim-Farbe setzen, `re15_render_textured_tri_lit` ggf. gegen die ungelitete Variante
  tauschen bzw. Lit-Faktor neutral stellen.
- KEINE weiteren Änderungen: Kinetik, 29-Frame-Verschwinden (`re2z_part_flags=0`,
  :4355-4357) und Routing bleiben.

**Schritt 3 — Pins** (tests/unit/test_re2_gore.c erweitern):
- Nach 92C4-P0: `re2z_part_flags[8] & 0x4A`, Burst-Query liefert 1 mit scale 30 bei
  life=0 und 465 bei life=29; nach Ablauf flags==0 → Query 0.
- Negativ: geworfenes Bein (0x1062) → Query 0 (0x1062 & 0x08 == 0, intaktes Mesh).
- Fixture-Verschiebung beachten: falls ein bestehender Pin das „intakte Kopf-Mesh im
  Flug" verankert, neu verankern (Memory reai-v2-pin-fixture-verschiebung).

**NICHT ändern**: rag231-Routing (§1 byte-true), +0x1D2-Stempel, FX-Spawns (Hals-Anker-
Baustelle ist gore-vollausbau §2.2, separat).

## 6. Offen (kein Blocker)

1. `FUN_8002DA80/8002DDF0` (Bit-0x10-Zeichner) nicht disassembliert — im Zombie-Gore wird
   Bit 0x10 allein nie gesetzt (0x4A/0x1062 tragen es nicht); vermutlich Nutzer der
   Tabellen-Zeile 1 (180..2790). OFFEN.
2. GTE-Op `copFunction(2,0x480012)` (MVMVA-Variante) nur strukturell gelesen; für den Port
   irrelevant (Versatz im Part-Raum ist mathematisch äquivalent).
3. `+0x1C0 = 1` @0x8010945C / @0x80108DC4 weiterhin OPEN (wie kopf-wegschiessen §6).
