# Runde 36 — Spur O "pri" (Dossier)

Baum: `.claude/worktrees/r36_pri`, Zweig `r36/pri`, Basis master 94830fb8 (ff von 78a4a748, dazwischen nur Doku/Assets).
Datum: 2026-10-11. Vorgaenger: keiner (Spur nicht begonnen). Bilder: `O_pri_bilder/`.

## Punkte (Wortlaut AUFTRAG.md Z.57/58)
1. "Ich habe dir ein neues PRI erstellt im ROOM 10F0 (02_02.png), weil mit der neuen Cutscene auffaellig ist,
   wie Marvin nicht ueberdeckt wird von von der Absperrung."
2. "Dann habe ich noch mal einen einzelnen Marker gemacht fuer ROOM 1100. Hier wird Leon mit einer
   Halbtransparenz ueberdeckt mit Boden.... Das ist natuerlich falsch." — die Marker-Datei wurde NIE gefunden
   (AUFTRAG.md "Beigelegte Marken": Suche in Wurzel, pri/, masken_vorschau/, shots/ seit 2026-10-03; eigene
   Suche 2026-10-11 ebenso leer) -> der Befund wurde selbst gemessen (alle Cuts, A/B mit RE15_NO_PRI).

Werkzeuge: Hintergruende `probe_bg_dump` (STAGE1, 428 Bilder -> build/bg_ppm), Klemmpfad-Dump
`probe_p2_floor_dump 1000 7000` (build/p2/dump_klemmpfad.txt), Messkopien der exe `re15_pc_r36o_*.exe`,
Framedumps RE15_WINDOW_SCALE=1, Start ueber RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 + RE15_DEBUG_JUMP.

---------------------------------------------------------------------------------------------------------
## 1. ROOM10F0 Cut 2 — Absperrung des Nutzers (pri/STAGE1/10F0/02_02.png)

### 1.1 Messung vorher (Szene, exe-Kopie re15_pc_r36o_a.exe, RE15_DEBUG_JUMP=10F0@250, Framedump 700..2320/10, RE15_STATE_LOG)
- Marvins Weg (Slot t=40, `@(x,z)` aus dem State-Log): F910 Tuer (8400,-350) -> F960 (5939,2078) -> F970 Wegpunkt
  (5528,2510) -> F1000 (4980,4725) -> F1030..F1050 **x=4568 geklemmt** (z 6941 / 7665 / 8365; Zelle 3 Flaeche
  x=4100 + Spielerradius 450 = 4550) -> F1060 (4334,9021) -> F1070 (3941,9668) -> ab F1080 Stand (3823,9862).
- Bild 1200 (Stand, `10F0_C2_Marvin_F1200_vorher_nachher.png` links): Marvins LINKE Schulter/Arm wird ueber die
  ferne Kante der Glas-Absperrung gezeichnet (Spalten 88..97). Sein Standplatz liegt mit x=3823 WESTLICH der
  Scheibenebene (x~4000..4100) und mit z=9862 jenseits ihres Endes (z~8227); der Sehstrahl zu seiner linken
  Koerperhaelfte (x<~3630) schneidet die Scheibe (Strahl vom Kameraort (5874,1851) kreuzt x=4100 bei z<8227) —
  dort gehoert er VERDECKT. Ohne Maske wird er ueberall gezeichnet. Das ist der Befund des Nutzers.
- Waehrend des Laufs (F1030..F1060, x=4568) ist Marvin OESTLICH der Scheibe: kein Strahl zu seinem Koerper
  kreuzt x=4100 vor ihm -> er muss sichtbar bleiben (er ist es, 02_Messung nachher bestaetigt es).

### 1.2 Lage und Geometrie der Freistellung (gemessen)
- `maske_aus_png.platziere(02_02.png, bg Cut 2)`: x=33 y=56 Massstab 1, Uebereinstimmung 100 % (7278 opake
  Punkte, Bbox x33..98 y59..237). Das bestehende Lasso 02.png (Stuhl) liegt bei x=176 y=115 (877 Punkte).
- Scheibenbasis aus der Lasso-Unterkante (welt_am_boden): x=3986..4025 fuer z=5362 (Bild x63,y237) .. 8227
  (x98,y167); die nahe senkrechte Kante laeuft von (33,59) nach (63,237) = Senkrechte zum Nadir (Kamera
  17 Grad geneigt). Die Scheibe steht also 75..115 Einheiten HINTER der Zellenflaeche x=4100 (Polster) und
  endet mit der Zelle (z 8200).
- Zelle: ROOM10F0.RDT SCA-Eintrag 3 @Datei-Offset 0x6B4, Bytes `08 07 20 35 fc 08 e8 ea 01 ff 00 03`
  = (w 1800, d 13600, x 2300, z -5400, Typ 1) -> x 2300..4100, z -5400..8200.

### 1.3 Umsetzung
- `analysis/esp_masken_2026-09-03/auswahl.json` ROOM10F0 "2": zweites Objekt
  `{"name": "10F0 Cut 2 Teil 02 Absperrung (Nutzer-Lasso, Runde 36 Spur O)", "png": "pri/STAGE1/10F0/02_02.png",
  "x": 33, "y": 56, "massstab": 1, "oben": 0, "grow": 0}` (Tiefe ueber die Zellen-Zuordnung der Pipeline);
  Objekt 1 umbenannt in "... Teil 01 Stuhl ...".
- Bau `python re15_port/tools/maske/raum.py ROOM10F0 2 --stufe 16 --statistik max` (Auslieferungs-Parameter,
  analysis/befunde_2026-09-21/pri-runde19c-saum.md Z.100/110). Protokoll:
  - Stuhl: Zelle x6600..8100 z7500..9000 (Typ 3), Hoehe -1050, 720/877 Punkte, Tiefe 96..117 (unveraendert).
  - Absperrung: Zelle x2300..4100 z-5400..8200 (Typ 1, 78 % der Standpunkte, IoU 0.44), Hoehe -3550,
    7278/7278 Punkte getroffen, Tiefe 49..105.
  - Zerlegung Stufe 1 -> 93 Rechtecke, 66 Tiefenstufen 49..117, 19669 Atlaspunkte.
  - Abnahme: Deckung fehlt 0 / zuviel 0; Standplaetze 6424, beruehrt 925; VOR n=331 verdeckt=0 teil=0;
    HINTER n=611 frei=27 (Band-1-Plaetze x=4600 OESTLICH der Scheibe = richtig frei; (0,6400,9000) 0.90 Stuhl).
  - GESCHRIEBEN: MASKS/ROOM10F0.MSK (Sektion Cut 2), ROOM10F0_PRI02.TIM/.PBM/.STAND. Pruefbild `ROOM10F0_C2.png`.
- MASKS/ ist Erzeugnis des Ports (R15M-Container), kein Original-Patch; der Vorrang-Riegel (Original-Sektion
  gewinnt) ist unberuehrt. `pri_stage1_hashes.inc` enthielt 02_02.png bereits (78a4a748) -> unit_pri_hashes gruen.

### 1.4 Messung nachher (exe-Kopie, gleicher Lauf, Framedump 960..1260/10 + State-Log; Marvin bildgleich positioniert)
- Bild 1200: 74 Pixel geaendert, bbox x88..179 y91..150 — Marvins linker Rand (x88..97) jetzt von der
  Scheibenkante verdeckt (`10F0_C2_Marvin_F1200_vorher_nachher.png` rechts). Bild 1070 (Marvin (3941,9668),
  Ecke umrundet): 177 Pixel, linke Seite verdeckt.
- Bilder 1030/1050/1060 (Marvin x=4568 OESTLICH der Scheibe): 3 Pixel Differenz (x178..179 y116..117 — Stuhl-
  Atlas-Rundung, nicht Marvin); Bild 1040: 11 Pixel, davon 8 bei x86..90 y109..132 = die ausschwingende linke
  Hand, die physisch in die Scheibenebene reicht (Strahl durch Spalte 88 kreuzt x=4100 vor ihr).
- Modell (abnahme-Zaehlung, Koerperkasten +-450/1500): F1030 286 Maskenpunkte im Kasten, 0 verdeckend;
  F1040 50/0; F1050 0/0; F1060 0/0; F1070 61/61; Stand (3823,9862) 127/127 (= die linke Viertelseite).
- RE15_PRI_LOG Cut 2: "93 Masken Tiefe 49..117 (Schwelle 3203..7559)".

---------------------------------------------------------------------------------------------------------
## 2. ROOM1100 — "Leon mit einer Halbtransparenz ueberdeckt mit Boden"

### 2.1 Bestand
- ROOM1100.RDT: 13 Cuts, KEINE Original-Masken (alle pri_offset -> gc/mc = 0xFFFF). Cuts 7..12 = identische
  Senkrecht-Stubs (-19628,-17442,22616). Port-Masken nur in Cut 1 und 2 (MASKS/ROOM1100.MSK, R15M): reine
  QUADER-Waende aus den Kollisionszellen (auswahl.json, Marken F595/F205, 2026-09-08/09), Cut 1 105 Rechtecke
  32x32 Tiefe 45..258, Cut 2 105 Rechtecke 20x20 Tiefe 48..163 (`1100_C1_quader_vorher.png`: magenta).
- Zellen (SCA): Westwand x-17600..-15600 z-23200..-12100, Nordwand x-28900..-15600 z-14100..-12100,
  Suedblock x-22700..-15600 z-23201..-21304, Suedwestwand x-23105..-21158 z-28356..-21542.
- Keine Halbtransparenz in den Masken selbst: bg_pc.c pri_publish_tim setzt Alpha 0xFF fuer jeden Index != 0
  (Original: SPRT Code 0x64, ABE=0); render_pc.c blittet den Atlas mit SDL_BLENDMODE_BLEND, Alpha voll.

### 2.2 Messung vorher
a) Modell (abnahme.deckung_und_tiefe + Koerperkasten, Original-Urteil depth < (1023*vz)>>16) ueber ALLE
   begehbaren Standplaetze des Klemmpfad-Dumps (27218 Punkte, Band 0): im offenen N-S-Flur (x -15150..-13050,
   z -22750..-12550) hat KEIN Standplatz (0 von 561) einen verdeckten Koerperpunkt — in Cut 1 und Cut 2.
   Verdeckt werden nur Plaetze HINTER den Waenden (Suedflur hinter dem Suedblock, Nordflur hinter der Westwand)
   und die zwei Eckplaetze (-15500,-23700)/(-15500,-11500) mit der westlichen Koerperseite hinter der Ecke
   (physisch richtig: Strahl schneidet die Zelle).
b) Spiel (exe-Kopien, A/B mit RE15_NO_PRI=1, Eingabeskript "U7,R0.67,U7" = Flur hoch und zurueck, drei Bahnen
   x=-15150 (Westwand), -14100, -13050; Framedumps alle 10 Bilder, 3x2 Laeufe): KEIN Bild zeigt Leons Koerper
   von einer Maske ueberdeckt. Konstante Differenzen: Cut 1 x165..189 y61..144 (186 px) = der Zombie hinter
   der Suedecke, Cut 2 x127..141 y127..139 (65 px) = der Zombie hinter der Nordecke — beide SOLLEN verdeckt sein
   (F595). Leon-bezogen nur ein 1..3 px breiter Streifen am Rand seines SCHATTENS neben der Wand (Cut 2 F100/F110,
   `1100_C2_schatten_vorher_mit_ohne_diff.png`: Standplatz (-15100,-18500), 85 px Differenz, davon die Diagonale
   am Fuss).
c) Was die Masken am Wandfuss sind: die gezeichnete Wandbasis liegt HINTER der Zellenflaeche x=-15600. Gemessen
   am Hintergrund (Boden/Sockel-Kante je Bildspalte, welt_am_boden): Cut 1 x=-15664..-15690 (Spalten 190..246),
   Cut 2 x=-15708..-15737 (Spalten 56..128). Der Quader traegt also einen 85..135 Einheiten breiten BODEN-Streifen
   vor der Wand mit Wandtiefe. Im Schattenquad des Spielers an der Wand lagen 142 (Cut 2) bzw. 100 (Cut 1)
   solche Maskenpixel.
d) Warum sie den Schatten ueberdecken: render_pc.c zeichnete ALLE Figurenschatten in "Step 1.5" vor allen
   Dreiecken und Masken (Kommentar "before the character tris"); jede Maske, auch eine FERNERE, blittete danach
   Boden ueber den Schatten. Der Schatten ist das einzige halbtransparente Element an Leon (ABR2-Subtraktiv-Quad,
   FUN_8001af5c `SetSemiTrans` @0x8001afb4): **"Leon mit einer Halbtransparenz ueberdeckt mit Boden" = sein
   halbtransparenter Schatten, von Bodenpixeln der Wandmaske ueberdeckt.** Die exakte Stelle des Nutzers bleibt
   ohne seine Marker-Datei offen (s. "Fuer den Nutzer"); alles, was in ROOM1100 Leon-bezogen ueberdeckt wird,
   ist dieses Muster.
e) Zweite Folge desselben Bodenstreifens: der kriechende Zombie an der Suedecke (Cut 1, Standplatz (-15500,-23700))
   verliert seinen Kopf unter der Suedblock-Maske (`1100_C1_suedecke_vorher_nachher.png` links).

### 2.3 RE-Belege
- Schatten als OT-Prim (PSX.EXE, ghidra1_V2.txt): FUN_8001b064 (einziger Aufrufer FUN_80031c44 @0x80031dcc):
  `@0x8001b2c8 jal RotAverage4` -> OTZ (AVSZ4), `@0x8001b2d8 srl v0,v0,0x4` (OT-Index = otz>>4, wie die Figur
  @0x8002565c), `@0x8001b2e0 sll v0,v0,0x2`, addPrim `@0x8001b30c lw a0,0x10(a1)` / `@0x8001b310 lw v1,0(v1)
  =>DAT_800aa6d8` / `@0x8001b314-18 and` (Kopf-Einhaengung). ZSF4 = 256: `@0x80066c7c li t0,0x100`,
  `@0x80066c80 gte_ldZSF4 t0` -> otz = (256*(sz0+sz1+sz2+sz3))>>12 = Summe>>4 -> Bucket = Summe der vier
  Ecken-Kamera-z >> 8.
- Masken: FUN_80039590, OT-Index = depth (@0x80039658), AddPrim @0x80039660.
- Bildaufbau (eine Funktion, LAB_8001cb78..LAB_8001d158): `@0x8001ce0c jal FUN_80031c44` (Spieler, am Ende der
  Schatten) -> `@0x8001ce54 jal FUN_80039590` (Masken) -> `@0x8001d09c jal FUN_8001e8c8` (Spieler-Polygone) ->
  Figuren-Schleife `@0x8001d0e8..0x8001d164` (`@0x8001d108 jal FUN_8001e8c8`). ClearOTagR + AddPrim (Kopf):
  im selben Bucket wird das zuletzt eingehaengte Prim zuerst gezeichnet -> Spieler-Schatten liegt bei
  Gleichstand OBEN auf Maske und Spieler-Polygonen; ein nach den Masken eingehaengtes Prim liegt unter ihnen.
- Zellenpolster: Messung 2.2c (Hintergrundbild gegen Zelle), keine Original-Masken in ROOM1100 (2.1).

### 2.4 Umsetzung
1. **Maskendaten ROOM1100 C1/C2 auf die gemessene Wandbasis** (auswahl.json, _warum je Cut):
   Cut 1 Ostflaeche x=-15690 (Westwand w 2000->1910, Suedblock w 7100->7010); Cut 2 x=-15710 (Westwand
   w 2000->1890, Nordwand w 13300->13190). raum.py: die Cuts von P2_UNANGETASTET nach P2_ALTE_KETTE (Quader-
   Silhouette + Kollisionstiefe wie bisher, jetzt mit .PBM/.STAND/.BELEG -> unit_pri_silhouette/kopfschnitt).
   Bau `raum.py ROOM1100 1|2 --stufe 16 --statistik max`: Cut 1 105 Rechtecke, Soll 27450 px, Deckung fehlt 0 /
   zuviel 0 / unbelegt 0; Cut 2 99 Rechtecke, Soll 24358 px, 0/0/0. GESCHRIEBEN: ROOM1100.MSK, _PRI01/_PRI02
   .TIM/.PBM/.STAND/.BELEG. Gegenprobe F595: Zombie 5 (-20275,-10713) in Cut 2 weiter 100 % verdeckt
   (Strahl kreuzt x=-15710 bei z=-17769, innerhalb der Westwand).
2. **Schatten in der OT-Ordnung** (`include/re15_pri_schatten_r36.h`, neue Datei; Haken in render_pc.c und
   main.c, alle "Runde 36 Spur O"):
   - main.c: an beiden Schatten-Aufrufstellen (Spieler 8890, NPC 9770) die AVSZ4-Summe der vier Ecken-Kamera-z
     (SZ-Klemme 0..65535) ueber `re15_render_pc_shadow_tiefe(summe, ist_spieler)` getaggt.
   - render_pc.c: getaggte Schatten laufen nicht mehr in Step 1.5, sondern im Tiefen-Durchlauf
     (`pc_masken_schatten_flush`): Bucket = Summe>>8; Maske UEBER Schatten gdw depth < Bucket (Gleichstand:
     nur Nicht-Spieler); Dreieck UEBER Schatten gdw (1023*vz)>>16 < Bucket (Gleichstand ebenso). Untagged
     Schatten (keine) behielten den alten Weg; RE15_NO_SHADOW wirkt auf beide; Standbild-Kopie traegt die Tags.
   - NPC-Schatten: Original-Emitter nicht belegt (FUN_8001b064 hat nur den Spieler-Aufrufer); fuer sie gilt
     bei Gleichstand "unten" = das bisherige Port-Verhalten, sonst die Bucket-Regel wie fuer jedes OT-Prim.

### 2.5 Messung nachher
- Modell: Maskenpixel im Schattenquad des Spielers an der Wand 142 -> 14 (Cut 2, (-15100,-18500)) und
  100 -> 19 (Cut 1, (-15100,-16900)); davon UEBER dem Schatten (depth < Bucket 65/64): 0 und 0.
  Koerperdeckung offener Flur weiterhin 0 (Eckplaetze: Cut 1 2, Cut 2 3 von 907 = hinter der Ecke).
- Spiel (neue exe + neue Masken, PRI an/aus): Cut 2 (-15100,-18500): 56 px Differenz, ALLE in der Zombie-Ecke
  x127..139 y127..139; Schattenfenster x40..120 y185..240: **0 px** (`1100_C2_schatten_ohne_mit_masken_nachher.png`).
  Cut 1 Suedecke (-15500,-23700): Kopf des kriechenden Zombies wieder sichtbar (`1100_C1_suedecke_vorher_nachher.png`
  rechts; Rest-Differenz 80 px = der Teil hinter der gezeichneten Ecke).
- Alte exe gegen neue exe mit denselben (neuen) Masken: Cut 2 Wandposition 165 px (x67..111 y220..237 = Schatten
  jetzt in der Ordnung), Cut 3 ohne Masken 23 px an den Fuessen (`1100_C3_schattenordnung_alt_neu.png`: der
  Schatten liegt im gleichen Bucket ueber den Fuss-Polygonen — Original-Reihenfolge, 2.3), 10F0 Bild 1200 12 px.

---------------------------------------------------------------------------------------------------------
## 3. Tests (re15_port/tests/unit/probes/r36_pri.cmake, test_r36_pri.c, tests/integration/test_r36_pri_schatten.cmake)
- `unit_r36_pri_absperrung`: ROOM10F0 Cut 2 ueber die echten Leser: > 27 Rechtecke; Scheibenpunkt (93,140) gedeckt
  mit Tiefe < 134; Stuhlpunkt (190,140) gedeckt; Boden (120,200) frei; Marvins Standplatz
  (RE15_CUT10F0_MARVIN_X/Z) >= 60 verdeckende Punkte, < 60 % des Kastens; Wegpunkte (4568,6941/7665/8365),
  (5528,2510): 0 verdeckende Punkte.
- `unit_r36_pri_schatten`: Bucket-/Gleichstand-Regeln an den Grenzen (4*6400 -> 100; Maske 99/100/101; Dreieck
  vz 6400/6410/6471; Deckung mit re15_pri_mask_occludes 6470/6471); ROOM1100.MSK: Schattenquad an der Wand
  (Cut 2 (-15100,-18500), Cut 1 (-15100,-16900)) hat Masken im Quad, aber keine UEBER dem Schatten; Zombie 5
  (F595) voll verdeckt; Spieler an der Wand unverdeckt.
- `integration_r36_pri_schatten` (echte exe, exe-Kopie re15_pc_r36o_haken_*.exe): ROOM1100 Cut 2, Spieler
  (-15100,-18500), Bild 150 mit/ohne Masken -> Schattenfenster 0 geaenderte Pixel, Zombie-Ecke > 0 (sonst leeres
  Gruen). PPM-Zaehlung ueber probe_r34_ppm_beitrag ("n=").
- Bestehende Riegel der beruehrten Daten: unit_pri_hashes, unit_pri_silhouette, unit_pri_kopfschnitt,
  unit_pri_eingemessen, unit_pri_inventar_riegel, unit_sld_atlas, integration_pri_masken.
- Ergebnis: siehe Abschnitt 5 (ctest-Summenzeile).

## 4. OFFEN
- Die Marker-Datei des Nutzers zu ROOM1100 fehlt; die Stelle ist nicht eindeutig belegt. Gemessen ist: der Koerper
  wird nirgends ueberdeckt, der halbtransparente Schatten wurde am Wandfuss von Bodenpixeln der Wandmaske ueberdeckt
  (jetzt nicht mehr), der Zombiekopf an der Suedecke ebenso.
- Gleichstand-Regel Maske/Figur in re15_pri.h ("gleicher Bucket = Figur obenauf", Begruendung "Masken sind die
  letzten Prims"): nach 2.3 werden die Spieler-Polygone @0x8001d09c NACH den Masken @0x8001ce54 eingehaengt,
  im gleichen Bucket laege also die Maske oben. Ein-Bucket-Nuance, nicht Teil dieser Spur, nicht geaendert.
- Schatten ueber den Fuss-Polygonen im gleichen Bucket (2.5, 23 px in Cut 3) folgt der OT-Analyse; am Original
  (DuckStation) nicht nachgemessen. Falls unerwuenscht: in re15_pri_schatten_r36.h die Gleichstand-Regel fuer
  ist_spieler auf "unten" setzen (eine Zeile).
- Die Quaderbasen anderer Raeume (Zellenpolster) sind nicht geprueft; nur ROOM1100 war Auftrag.

## 5. Suite (gezielt, keine volle Suite im Spurbaum)
- `ctest --test-dir re15_port/build -R "r36_pri|unit_pri_|integration_pri_masken|unit_sld_atlas" --timeout 600`
  -> **100% tests passed, 0 tests failed out of 10** (unit_sld_atlas, unit_pri_inventar_riegel, unit_pri_kopfschnitt,
  unit_pri_eingemessen, unit_pri_hashes, unit_pri_silhouette, unit_r36_pri_absperrung, unit_r36_pri_schatten,
  integration_r36_pri_schatten, integration_pri_masken). Erster Lauf von integration_r36_pri_schatten war rot
  (1214 "geaenderte" Pixel): probe_r34_ppm_beitrag zaehlt ohne Schwelle, also jede Atlas-Rundung der Masken-
  Re-Blits; seitdem probe_r36_ppm_diff mit Schwelle 24 (dieselbe Schwelle wie alle Messungen in §1/§2).
- Zweite Gruppe (beruehrte Module: Schattenpfad render_pc.c/main.c, 10F0-Szene mit neuen Masken, ROOM1100):
  `ctest --test-dir re15_port/build -R "crow_shadow|hundeschatten|room1100|r22_10f0|r35_cut10f0|r32_tor|marvin_render_pose|re2_gore_render" --timeout 900`
  -> **100% tests passed, 0 tests failed out of 16** (u.a. integration_r35_cut10f0 240,8 s, integration_r32_tor_hell
  = Standbild-Weg mit der Schattenkopie, unit_r22_10f0_figur, unit_re2_crow_shadow_release, unit_r35_raeume_hundeschatten).
- Volle Suite: nicht im Spurbaum (Vorgabe), faehrt der Orchestrator im Integrationsbaum.

## 6. Fuer den Nutzer
- Neue Assets fuer das Paket-Gate: `re15_port/shared_assets/PSX/MASKS/ROOM10F0.MSK`, `ROOM10F0_PRI02.TIM`,
  `ROOM1100.MSK`, `ROOM1100_PRI01.TIM`, `ROOM1100_PRI02.TIM` (+ Beiblaetter .PBM/.STAND/.BELEG nur fuer Tests).
- Frage: Die Marker-Datei fuer ROOM1100 ("Leon mit einer Halbtransparenz ueberdeckt mit Boden") lag nirgends im
  Repo (Wurzel, pri/, masken_vorschau/, shots/). Wo liegt sie? Mit ihr laesst sich die exakte Stelle gegen die
  jetzige Fassung nachmessen (gemessen und behoben: Schatten/Boden am Wandfuss in Cut 1 und 2, Zombiekopf Suedecke).
- Bedienhinweis: keine neuen Schalter; RE15_NO_PRI / RE15_NO_SHADOW bleiben Messhaken.
