# Runde 33 / Thema T — Restliche Tueren: Plan (Stufe 1)

Zweig `r33/tueren`, Arbeitsbaum `.claude/worktrees/r33_tueren`. Stand 2026-09-29, laufend fortgeschrieben.
Pilot-Ergebnis: `tueren_rest_pilot.md`.

## 0. Auftrag

Nutzer woertlich: "Ich möchte das du - für alle Türen die jetzt noch fehlen mit der Türanimation die
Türen baust und Animationen hinzufügst, das wir da komplett sind."

Stufe 1 = Plan je nicht abgedeckter Tuer (61) + Generator fuer port-eigene Archive + Pilot.

## 1. Bestand (gelesen, nicht neu gemessen)

- `analysis/befunde_runde31/tueren_03_zuordnung.md` + `tueren_03/zuordnung.json`: 144 physische Tueren, 83
  abgedeckt (RE2-Archiv GLEICH), 61 nicht: 45 aehnlich, 3 einseitig, 5 Aufzug, 3 keine Tuer, 1 Tor, 4 inert.
  Je Seite: Griffseite/-form, Merkmale, Abweichung zum naechsten Archiv.
- `tueren_02_re2.md`: Variantenregel 2.4 (gemalter Griff links V0 / rechts V1; Doppeltuer V2/V3;
  Leiter V4 hinauf / V5 hinab), Griff-Formfamilien 3, Tonfamilien 4 (F1..F7 = bytegleiche Tonteile).
- `tueren_04_bau.md`: Anschluss (Maschine, Laeufer `door_scene_pc.c`, Tabelle `gen/tuer_zuordnung.inc`,
  Griff-Tausch ueber Spender-Archiv, Pruefhaken `RE15_TUER_SEITE`/`RE15_TUER_BOGEN`/`RE15_TUER_SCHNELL`,
  Kontaktbogen `tools/tueren/tuer_kontaktbogen.py`).
- Tor-Vorbild `analysis/tor_1170/09_sequenz.md` + Runde 32 `analysis/befunde_runde32/tor_helligkeit.md`:
  gezeigt = Texel * c / 128 (psx-spx GPU:1438-1446), Tuerlicht BK 68 (@0x800142e8) -> Blattfarbe c = 73 an der
  zur Kamera gewandten Blattflaeche; Regel "gezeigt = gemalt an derselben Stelle" (F = P) -> Texel = P * 128/c.

## 2. Leitlinie und Messungen, auf denen der Plan steht

### 2.1 Alle Standardblaetter haben dieselbe Geometrie und dieselbe UV (gemessen)

`re15_port/tools/tueren/r33_ana_uv.py` (do2_format.Md1 auf `info/re2leon/COMMON/DOOR/DOORxx.DO2`): Mesh 0 (Blatt,
8 Ecken, 12 Dreiecke, u 0..127 v 0..217, tpage 0x80, CLUT 0x7800) ist geometrie- UND UV-gleich in
**00 01 04 06 07 08 0C 13 15 1A 1B 1D 22 23 24 25** (alle gemessenen Standard-/Doppeltuer-Archive).
Alle TIMs: flags 9 (8 bit + CLUT), CLUT (0,480) 256x1, Bild (0,0) 64 Worte x 256 = 128x256, 33 312 B.
Folge: **jede Blatt-Textur passt auf jedes dieser Archive**; die Griffe lesen nur den Beschlagstreifen v 218..255
(DOOR24 auch v 113..217, s. 3.3) ihres eigenen Archivs.

### 2.2 Bau-Strategie

1. **Basis-Archiv** (RE2, MD1 + Skripte + Tonteil UNVERAENDERT) nach gleicher Bewegungsart UND gemalter
   Griff-Form - dann braucht es keinen Griff-Tausch. Ton = Tonteil des Basis-Archivs, also dessen Tonfamilie
   (Material; tueren_02_re2.md 4: F3 = 06 07 08 22 2F Blech/Stahl, F2 = 01 04 09 11 Holz, F4 = 15 1A 23,
   F5 = 1B 30 Doppeltuer, F6 = 24 29, F7 = 26 31 Schott; eigene Tonteile 0C 14 16 1D 1E 25 27 2D).
2. **Textur neu**: Blatt (v 0..217) aus der naechsten RE2-Gestaltung, bearbeitet (Merkmal weg, Merkmal gemalt,
   Farbe auf den gemessenen RE1.5-Wert) - RE2-scharf 128x256. Aus Hintergrundpixeln wie beim Tor nur, wo keine
   RE2-Vorlage taugt; die RE1.5-Ausschnitte der Tueren sind meist 30..95 px hoch (gemessen T1:
   `re15_seiten/*_aus.png`), ihre Entzerrung (`*_entz.png`, 128x218) ist vergroesserte Unschaerfe -> sie liefern
   nur die FARBE (Tiefpass), nicht die Zeichnung.
3. **Helligkeit**: Ziel wie beim Tor (Runde 32, vom Nutzer abgenommen): gezeigt = gemalt, Texel = P * 128 / c,
   c = NCCT der Blattflaeche mit den RE2-Konstanten (`tor_sequenz_bauen.ncct_eckfarbe`, BK 68 -> 73). Flag 0x1000
   nur, wo Texel ueber 255 muessten (dann Skriptaenderung -> nicht im Pilot, Stufe 2).
4. **Variante** je Seite nach tueren_02_re2.md 2.4 aus der gemalten Griffseite (Runde 31), Zwilling wie seine
   Hauptseite, unsichtbar -> Komplement der Gegenseite, sonst Seite A V0 / B V1.
5. **Eine Tuer, ein Archiv** (RE2 238/238). Ausnahme wie T131 (Runde 31): die drei einseitigen Tueren, deren eine
   Seite schon GLEICH einem RE2-Archiv gebaut ist - die andere Seite bekommt ihr eigenes Port-Archiv.

### 2.3 Durchgaenge ohne Blatt: was RE2 zeigt (gemessen)

`build/r31_tueren/t2/re2_tueren.json` (534 RE2-Tuerseiten, Runde 31): die objektlosen Archive
20/21/32/34/36 (Blende + Ton, keine Objekte, tueren_02_re2.md 1.1) stehen AUSSCHLIESSLICH an Skript-Uebergaengen
mit Null-/Fernrechteck - ROOM2080 -> 2080 DOOR32 (Rechteck 20000,20000,1,1), ROOM2140 -> 2140 DOOR34 (0,0,1,1),
ROOM4100 -> 4040 DOOR20 und DOOR36 (25000,25000). **Keine einzige begehbare RE2-Seite (Rechteck am Boden) ist
ohne Tuerobjekt.** Was RE2 bei einem Uebergang ohne Tuer zeigt, ist also Blende + Ton eines objektlosen Archivs.
Leiter/Luke/Klappe (16/28/1E/33/35) sind Objekte und passen nur, wo eine Leiter/Luke/Klappe gemalt ist.
-> G12 (4 Tueren): objektloses RE2-Archiv, unveraendert (kein Port-Archiv). Welches der fuenf (Ton 20: 1,84 s;
21: 4,68 s; 32: 4,43 s; 34: 4,51 s; 36: zwei Se_on Bild 80/140 je 0,96 s) - Stufe 2 nach Huellkurve/Einsatz, hier
NICHT geraten.

### 2.4 Ausgenommen (belegt)

- **T041 Tor ROOM1170**: eigene Sequenz (v0.8.16), bleibt.
- **T064/T065/T066 ROOM1250 Slot 0/1/2 = inert**: Saetze `ROOM1250.RDT @0xCB2 3b 00 00 31 ...`,
  `@0xCD2 3b 01 00 31 ...`, `@0xCF2 3b 02 00 31 ...` (Zensus T1, ROOM1251 gleich): pc[2] = **sce 0**. Der Scan
  springt `jalr @0x8007469c[sce]` (@0x80042f7c), Eintrag 0 = Handler @0x8004305C = nichts
  (memory reai-v2-aot-sce-dispatch, `shots/aot_sce_census.md`); die 16 umgetypten sce-0-Tueren enthalten
  ROOM1250 nicht ("1250 slots 0-2 permanently INERT in the original"). Ziel-Nutzlast nur Nullen. Im Spiel nie
  begehbar - nichts zu animieren.
- **T019 ist NICHT inert** (Runde 31 fuehrte sie so): `ROOM1090.RDT @0x213A 3b 01 02 31 06 ...` = sce 2 (Tuer),
  Band 6, Ziel ROOM1100 Cut 5, Engine stellt sie auf (`engine: true`). Nur ist dort kein Blatt gemalt
  (Gebaeudeecke) -> G12.

## 3. Plan je Tuer (61)

Quelle: `re15_port/tools/tueren/tuer_rest_plan.py` -> `analysis/befunde_runde33/tueren_rest/plan.json`
(Bauquelle fuer Generator + Tabelle). "Stufe" = 1 Pilot, 2 Folgestufe.

### 3.1 Gruppen

| Gruppe | Tueren | Port-Archiv | Basis (Bewegung, Ton, Griff) | Blatt-Textur aus / Bearbeitung |
|---|---|---|---|---|
| G1 glatte dunkle STAGE1-Stahltuer | **12** (T000 T001 T002 T003 T004 T006 T012 T013 T015 T016 T021 T022) | P07G | DOOR07, F3 Blech, Druecker flach | DOOR07 ohne Lueftungsschlitze/Flecken, Farbe RE1.5 |
| G2 Fabrik-Stahltuer 2 Felder + Stange | 6 (T094 T098 T103 T105 T106 T107) | P06F | DOOR06, F3, Buegel-/Stangengriff | DOOR08 ohne Nieten/Rost, Randnut + gerahmte Felder, hellgrau |
| G2b dito ungleiche Felder | 1 (T096) | P06U | DOOR06 | DOOR08, Felder oben hoch / unten quadratisch |
| G3 DOOR23-Doppeltuer | 4 (T071 T076 T081 T082) | P1B3 | DOOR1B beide Fluegel, F5 | DOOR23-Blatt; Riegelstange per Griff-Tausch (Spender DOOR23) |
| G4 glatte Stahl-Doppeltuer, 2 Druecker | 2 (T026 T054) | P1DG | DOOR1D V2/V3, eigener Blechton, Druecker flach | DOOR1D ohne Lueftungsgitter, graugruen |
| G4b Stahlrahmen-Doppeltuer mit Feldern | 2 (T014 T045) | P1DK | DOOR1D V2/V3 | Felder nach DOOR1A ohne Glas |
| G5 Holz-Doppeltuer Kassetten + Messingstangen | 1 (T035) | P04B | DOOR04 V2, F2 Holz | DOOR04 blau -> braunes Holz |
| G5b helle Holz-Doppeltuer Drahtglas | 1 (T025) | P0CD | DOOR0C, Holzton 0C | helles Holz, hohe schmale Drahtglasfenster, Panikstange |
| G6 Messingleiter | 3 (T023 T052 T053) | P16M | DOOR16 V4/V5, eigener Ton (4 Sprossen) | Silberrohr -> Messing |
| G6b Rostleiter | 1 (T047) | P16R | DOOR16 | -> rostbraun |
| G7 Lamellen-Lueftung | 2 (T029, T078/S154) | P1EL | DOOR1E, eigener Ton | Staebe -> waagerechte Lamellen |
| G7b Lueftung Staebe unten | 1 (T080) | P1EU | DOOR1E | Staebe nur unten |
| G8 Aufzugtuer (Kabine ROOM1080) | 3 (T011 T017 T018) | P25G | DOOR25 Hubtuer, eigener Ton | ohne Schild/Aufkleber/Rippen |
| G9 Lastenaufzug-Gittertor | 1 (T102) | P14A | DOOR14 Schiebetor | groebere Rauten, hellgrauer Rahmen |
| G10 Lastenaufzug-Schacht | 1 (T112) | P2DS | DOOR2D Hubbuehne V4 | Tafel + Lampe weg (Texel 0) |
| G11 Einzelgestaltungen | 12 (T049; T050 T072 T073; T074; T084; T095; T123; T126; T142; T161/S322; T048/S087) | P07R P1AP P1DO P26W P24B P07D P07H P27S P1AZ P07M | je Zeile 3.2 | je Zeile 3.2 |
| G12 Durchgang ohne Blatt | 4 (T019 T085 T086 T117) | - | RE2 objektlos (2.3) | - |
| G13 Tor (fertig) | 1 (T041) | - | - | - |
| G14 inert | 3 (T064 T065 T066) | - | - | - |
| **Summe** | **61** | 25 Port-Archive | | |

### 3.2 Tabelle je Tuer

(erzeugt: `tueren_rest/plan_tabelle.md`)

| Gruppe | Tuer | Raeume | Seiten (Variante) | Port-Archiv | Basis (Bewegung/Ton/Griff) | Blatt aus | Stufe |
|---|---|---|---|---|---|---|---|
| G1 | T000 | ROOM1000 <-> ROOM1050 | S001 V1; S019 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T001 | ROOM1000 <-> ROOM1050 | S002 V0; S018 V1 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T002 | ROOM1000 <-> ROOM1050 | S000 V1; S020 V1 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T003 | ROOM1010 <-> ROOM1020 | S004 V0; S005 V1 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T004 | ROOM1010 <-> ROOM1020 | S003 V1; S006 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T006 | ROOM1030 <-> ROOM1040 | S010 V1; S014 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T012 | ROOM1040 <-> ROOM1060 | S015 V0; S025 V1 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T013 | ROOM1050 <-> ROOM10A0 | S021 V0; S033 V1 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T015 | ROOM1060 <-> ROOM1120 | S023 V1; S056 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T016 | ROOM1060 <-> ROOM10C0 | S024 V1; S039 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T021 | ROOM10A0 <-> ROOM11E0 | S036 V1; S102 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G1 | T022 | ROOM10A0 <-> ROOM1180 <-> ROOM1230 | S034 V1; S035 V1; S078 V0; S119 V0 | P07G | DOOR07: F3 (06 07 08 22 2F, Blechtuer); Druecker flach (DOOR07 Mesh 1, gemalt: Druecker auf Rechteckschild) | DOOR07 | 1 |
| G2 | T098 | ROOM3050 <-> ROOM30E0 | S189 V0; S213 V1 | P06F | DOOR06: F3 (Stahltuer); Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G2 | T103 | ROOM3070 <-> ROOM30E0 | S196 V1; S211 V0 | P06F | DOOR06: F3 (Stahltuer); Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G2 | T105 | ROOM3090 <-> ROOM30C0 <-> ROOM30D0 | S198 V0; S199 V0; S205 V1; S208 V1 | P06F | DOOR06: F3 (Stahltuer); Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G2 | T106 | ROOM3090 <-> ROOM30B0 | S202 V1; S204 V0 | P06F | DOOR06: F3 (Stahltuer); Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G2 | T107 | ROOM3090 <-> ROOM30A0 | S201 V1; S203 V0 | P06F | DOOR06: F3 (Stahltuer); Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G2 | T094 | ROOM3030 <-> ROOM30C0 <-> ROOM30D0 | S184 V0; S185 V0; S206 V1; S209 V1 | P06F | DOOR06: F3 (Stahltuer); Buegel-/Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G2b | T096 | ROOM3040 <-> ROOM3050 | S187 V0; S188 V1 | P06U | DOOR06: F3 (Stahltuer); Stangengriff senkrecht (DOOR06 Mesh 1) | DOOR08 | 2 |
| G3 | T071 | ROOM2000 <-> ROOM2070 | S136 V3; S155 V2 | P1B3 | DOOR1B: F5 (1B 30, Doppeltuer); Riegelstange quer (Spender DOOR23 Mesh 1 am 1B-Anhaengepunkt) | DOOR23 | 2 |
| G3 | T076 | ROOM2030 <-> ROOM2070 | S141 V3; S156 V2 | P1B3 | DOOR1B: F5 (1B 30, Doppeltuer); Riegelstange quer (Spender DOOR23 Mesh 1 am 1B-Anhaengepunkt) | DOOR23 | 2 |
| G3 | T081 | ROOM2070 | S157 V3 | P1B3 | DOOR1B: F5 (1B 30, Doppeltuer); Riegelstange quer (Spender DOOR23 Mesh 1 am 1B-Anhaengepunkt) | DOOR23 | 2 |
| G3 | T082 | ROOM2070 | S158 V3 | P1B3 | DOOR1B: F5 (1B 30, Doppeltuer); Riegelstange quer (Spender DOOR23 Mesh 1 am 1B-Anhaengepunkt) | DOOR23 | 2 |
| G4 | T026 | ROOM10D0 <-> ROOM1100 | S045 V2; S049 V3 | P1DG | DOOR1D: eigen DOOR1D (Blech-Doppeltuer, V2/V3); Druecker flach (DOOR1D Mesh 1) | DOOR1D | 1 |
| G4 | T054 | ROOM11E0 <-> ROOM11F0 | S100 V2; S104 V3 | P1DG | DOOR1D: eigen DOOR1D (Blech-Doppeltuer, V2/V3); Druecker flach (DOOR1D Mesh 1) | DOOR1D | 1 |
| G4b | T014 | ROOM1050 <-> ROOM1090 | S022 V2; S030 V3 | P1DK | DOOR1D: eigen DOOR1D; Druecker flach (DOOR1D Mesh 1) | DOOR1A | 2 |
| G4b | T045 | ROOM1180 <-> ROOM11B0 <-> ROOM1230 | S080 V2; S092 V3; S093 V3; S121 V2 | P1DK | DOOR1D: eigen DOOR1D; Druecker flach (DOOR1D Mesh 1) | DOOR1A | 2 |
| G5 | T035 | ROOM1130 <-> ROOM1140 | S059 V2; S063 V2 | P04B | DOOR04: F2 (01 04 09 11, Holz); Stangengriff lang Messing (DOOR04 Mesh 1, gemalt gleich) | DOOR04 | 2 |
| G5b | T025 | ROOM10C0 <-> ROOM10D0 | S041 V1; S044 V0 | P0CD | DOOR0C: eigen DOOR0C (Holz-Doppeltuer mit Glas); Stangengriff senkrecht (DOOR0C Mesh 1) | DOOR0C | 2 |
| G6 | T023 | ROOM10B0 <-> ROOM1260 | S038 V5; S130 V4 | P16M | DOOR16: eigen DOOR16 (4 Sprossen, kein Schliesston); - | DOOR16 | 1 |
| G6 | T052 | ROOM11B0 <-> ROOM1260 | S095 V5; S128 V4 | P16M | DOOR16: eigen DOOR16 (4 Sprossen, kein Schliesston); - | DOOR16 | 1 |
| G6 | T053 | ROOM11D0 <-> ROOM1260 | S099 V5; S131 V4 | P16M | DOOR16: eigen DOOR16 (4 Sprossen, kein Schliesston); - | DOOR16 | 1 |
| G6b | T047 | ROOM11A0 <-> ROOM3000 | S088 V4; S173 V5 | P16R | DOOR16: eigen DOOR16; - | DOOR16 | 2 |
| G7 | T029 | ROOM10F0 | S048 V0 | P1EL | DOOR1E: eigen DOOR1E (Klappe); - | DOOR1E | 2 |
| G7 | T078 | ROOM2040 <-> ROOM2060 | S146 (gebaut R31); S154 V0 | P1EL | DOOR1E: eigen DOOR1E (Klappe); - | DOOR1E | 2 |
| G7b | T080 | ROOM2040 <-> ROOM2050 | S145 V0; S152 V0 | P1EU | DOOR1E: eigen DOOR1E; - | DOOR1E | 2 |
| G8 | T011 | ROOM1040 | S013 V0 | P25G | DOOR25: eigen DOOR25 (Hubtuer); - | DOOR25 | 2 |
| G8 | T017 | ROOM10C0 | S040 V0 | P25G | DOOR25: eigen DOOR25 (Hubtuer); - | DOOR25 | 2 |
| G8 | T018 | ROOM1120 | S057 V0 | P25G | DOOR25: eigen DOOR25 (Hubtuer); - | DOOR25 | 2 |
| G9 | T102 | ROOM3070 | S195 V0 | P14A | DOOR14: eigen DOOR14 (Schiebe-Gittertor); Griffplatte (DOOR14 Mesh 1) | DOOR14 | 2 |
| G10 | T112 | ROOM4000 | S218 V4 | P2DS | DOOR2D: eigen DOOR2D (Hubbuehne); - | DOOR2D | 2 |
| G11a | T049 | ROOM11A0 | S089 V1; S090 V1 | P07R | DOOR07: F3 (= DOOR22-Ton, bytegleich); Druecker flach (DOOR07 Mesh 1) | DOOR22 | 2 |
| G11b | T050 | ROOM11A0 <-> ROOM20A0 | S091 V1; S167 V1 | P1AP | DOOR1A: F4 (15 1A 23 = DOOR23-Ton); Druecker auf Kasten (DOOR1A Mesh 1) | DOOR23 | 2 |
| G11b | T072 | ROOM2020 <-> ROOM2030 | S140 V0; S143 V1 | P1AP | DOOR1A: F4 (15 1A 23 = DOOR23-Ton); Druecker auf Kasten (DOOR1A Mesh 1) | DOOR23 | 2 |
| G11b | T073 | ROOM2020 <-> ROOM2050 | S139 V1; S151 V0 | P1AP | DOOR1A: F4 (15 1A 23 = DOOR23-Ton); Druecker auf Kasten (DOOR1A Mesh 1) | DOOR23 | 2 |
| G11c | T074 | ROOM2030 <-> ROOM2060 | S144 V0; S153 V1 | P1DO | DOOR1D: eigen DOOR1D; Druecker flach im Griffkasten (DOOR1D Mesh 1) | DOOR1D | 2 |
| G11d | T084 | ROOM2090 <-> ROOM20A0 | S164 V3; S165 V2 | P26W | DOOR26: F7 (26 31, Schott); Handrad (DOOR26 Mesh 3) | DOOR26 | 2 |
| G11e | T095 | ROOM3030 <-> ROOM30E0 | S183 V1; S214 V0 | P24B | DOOR24: F6 (24 29); Druecker schraeg (DOOR24 Mesh 1) | DOOR24 | 2 |
| G11f | T123 | ROOM4050 | S236 V0; S239 V1 | P07D | DOOR07: F3; Druecker flach (DOOR07 Mesh 1) | DOOR07 | 2 |
| G11g | T126 | ROOM4050 | S232 V0; S240 V1 | P07H | DOOR07: F3; Druecker flach (DOOR07 Mesh 1) | DOOR07 | 2 |
| G11h | T142 | ROOM5040 <-> ROOM5060 <-> ROOM5120 | S269 V0; S273 V0; S274 V0; S305 V0 | P27S | DOOR27: eigen DOOR27 (Labor-Schiebetuer); - | DOOR27 | 2 |
| G11i | T161 | ROOM6000 <-> ROOM6030 | S314 (gebaut R31); S322 V1 | P1AZ | DOOR1A: F4; Druecker (DOOR1A Mesh 1) | DOOR1A | 2 |
| G11j | T048 | ROOM11A0 <-> ROOM2070 | S087 V1; S159 (gebaut R31) | P07M | DOOR07: F3; Druecker flach (DOOR07 Mesh 1) | DOOR14 | 2 |
| G12 | T019 | ROOM1090 | S031 | OBJEKTLOS | RE2 objektlos (DOOR20/21/32/34/36, Wahl in Stufe 2 nach Huellkurve) | - | - |
| G12 | T085 | ROOM20A0 | S166 | OBJEKTLOS | RE2 objektlos (DOOR20/21/32/34/36, Wahl in Stufe 2 nach Huellkurve) | - | - |
| G12 | T086 | ROOM20B0 | S171 | OBJEKTLOS | RE2 objektlos (DOOR20/21/32/34/36, Wahl in Stufe 2 nach Huellkurve) | - | - |
| G12 | T117 | ROOM4030 <-> ROOM4080 | S226; S248 | OBJEKTLOS | RE2 objektlos (DOOR20/21/32/34/36, Wahl in Stufe 2 nach Huellkurve) | - | - |
| G13 | T041 | ROOM1170 | S068; S074 | - | - | - | - |
| G14 | T064 | ROOM1250 | S126 | - | - | - | - |
| G14 | T065 | ROOM1250 | S127 | - | - | - | - |
| G14 | T066 | ROOM1250 | S125 | - | - | - | - |

### 3.3 Hinweise je Gruppe (Stufe 2)

- G2/G2b: Basis DOOR06 statt DOOR08 + Griff-Tausch: DOOR06 hat den senkrechten Buegelgriff (Formfamilie
  Stangen-/Buegelgriff 06/0B/0C/2F, tueren_02_re2.md 3) und dieselbe Tonfamilie F3 wie DOOR08.
- G3: 1B V2/V3 bewegt beide Fluegel; ob beide Fluegel ein Griff-Objekt tragen, per Simulator pruefen
  (`tuerkatalog.py`), sonst Riegelstange nur am gehenden Fluegel.
- G11b: Basis DOOR1A (Druecker + Kartenleser-Kasten = "Druecker auf dunkler Platte", Ton F4 = Tonfamilie von
  DOOR23) - vermeidet den Griff-Tausch.
- G11d: DOOR26 Handrad sitzt in Blattmitte (Mesh 3, Anker [450,-3040,2000]); gemalt ist ein kleines Rad an der
  Kante - Rad-Lage bleibt RE2 (MD1 unveraendert), nur Farbe rot.
- G11e: DOOR24 Griff-Mesh liest auch v 113..217 (Mesh 1 u 6..115 v 113..253) - diese Texel sind geschuetzt
  (Generator: Schutzmaske aus den UV aller Nicht-Blatt-Meshes).
- G10/G7b: Teile ausblenden = Texel 0 (GPU zeichnet Wert 0x0000 nie, psx-spx "Texture Color Black
  Limitations"; do2_format.tim_aus_bild) - MD1 bleibt unveraendert.

## 4. Werkzeug-Entscheidung

(folgt in Abschnitt 5 nach dem Bau)

## 5. Stand

- [x] Plan (dieses Dokument, plan.json)
- [ ] Generator `tools/tueren/tuer_archiv_bauen.py`
- [ ] Laufzeit: Kennung Port-Archiv neben RE2-Nummer
- [ ] Pilot G1 (12 Tueren) + G4 T054 + G6 T053
- [ ] Riegel `probes/r33_tueren.cmake`, RE15_MIN_TESTS
