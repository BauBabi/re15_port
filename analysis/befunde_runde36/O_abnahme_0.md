# Runde 36 — Spur O "pri": Unabhaengige Abnahme 0

Pruefer: Abnahme-Agent 0 (2026-10-11). Baum `.claude/worktrees/r36_pri`, Zweig `r36/pri`, HEAD f028af56,
Basis master 94830fb8 (merge-base). Massstab: Wortlaut AUFTRAG.md (die zwei PRI-Punkte), VERTRAG.md.
Geglaubt wird nur, was hier selbst am gebauten Stand gemessen ist.

Stand dieses Berichts: ABGESCHLOSSEN (2026-10-11, ~13:10) — BESTANDEN: beide Punkte erfuellt, Gates halten; keine Maengel, Hinweise H1-H8 am Ende.

Messordner (ausserhalb des Baums, Scratchpad dieser Sitzung):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/`
(im Folgenden `<M>`). Messkopie der exe: `re15_port/build/platform/pc/re15_pc_abn0.exe` (Kopie von re15_pc.exe,
HEAD-Stand). Vorher-Stand: `<M>/before/re15_pc_abn0_before.exe` = Kopie der Hauptbaum-exe
`C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_pc.exe` (2026-10-04 18:37, v0.8.23-Bau; render_pc.c/main.c
und MASKS/ auf master seit 3ec5b1f8 unveraendert: `git diff 3ec5b1f8 master --stat -- ...` leer), gefahren mit
`RE15_CD_ROOT=C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX` (= die ALTEN Masken; der Hauptbaum wird nur gelesen,
Logs landen im Scratchpad). Laeufer `<M>/run_abn0.sh` (RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=1
RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 SDL_AUDIODRIVER=dummy + Lauf-Umgebung), Werkzeuge `<M>/abn0_tools.py`
(eigene PPM-Differenz: Kanaldifferenz > 24 wie die Messungen des Dossiers, mit Bbox und A|B|Diff-Bild),
`<M>/abn0_lasso_check.py` (Lasso gegen die gebaute Maske).

## 0. Diff-Lage, Bau, gezielte Tests

* `git log --oneline master..HEAD`: 7a1ffadc, 171a90af, 5118c96e, f028af56 (4 Commits). `git diff 94830fb8 --name-only`
  = 33 Dateien: Dossier + 10 Bilder, auswahl.json, `include/re15_pri_schatten_r36.h` (neu), `platform/pc/main.c` (+8),
  `platform/pc/src/render_pc.c` (+115/-20), `tools/maske/raum.py` (+11/-2), MASKS/ROOM10F0.MSK + _PRI02.{TIM,PBM,STAND},
  MASKS/ROOM1100.MSK + _PRI01/_PRI02.{TIM,PBM,STAND,BELEG}, Tests `tests/unit/probes/r36_pri.cmake`,
  `tests/unit/test_r36_pri.c`, `tests/unit/probe_r36_ppm_diff.c`, `tests/integration/test_r36_pri_schatten.cmake`.
  Die in `git diff master --stat` zusaetzlich erscheinenden Loeschungen in UEBERGABE.md / RELEASE_NOTES-Entwurf sind
  master-Fortschritt nach der Basis, nicht die Spur (`git log 94830fb8..HEAD -- <datei>` leer).
* Code-Diff vollstaendig gelesen (Header, main.c-Haken, render_pc.c-Ordnung, raum.py, Tests, auswahl.json, STAND/BELEG).
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work
  to do.` / `=== LOCAL-BUILD-OK (build)`. Aktualitaet: Quellen render_pc.c 12:02:30, main.c 12:02:55, Header 12:00:01 <
  re15_pc.exe 12:04:02 < letzter Commit 12:19; `git status --short` nur `?? build/`. Die exe ist der HEAD-Stand.
* Gezielte Tests (`<M>/ctest_g1.log`, `<M>/ctest_g2.log`):
  `ctest -R "r36_pri|unit_pri_|integration_pri_masken|unit_sld_atlas"` -> `100% tests passed, 0 tests failed out of 10`
  (unit_r36_pri_absperrung 0.01 s, unit_r36_pri_schatten 0.02 s, integration_r36_pri_schatten 28.2 s, unit_pri_hashes,
  unit_pri_silhouette, unit_pri_kopfschnitt 5.3 s, unit_pri_eingemessen, unit_pri_inventar_riegel, unit_sld_atlas,
  integration_pri_masken);
  `ctest -R "crow_shadow|hundeschatten|room1100|r22_10f0|r35_cut10f0|r32_tor|marvin_render_pose|re2_gore_render"` ->
  `100% tests passed, 0 tests failed out of 16` (u.a. integration_r35_cut10f0 240.7 s, integration_r32_tor_hell 2.8 s =
  Standbild-Weg mit Schattenkopie, unit_r22_10f0_figur 8.1 s, unit_lane_b1_t45_room1100, unit_re2_crow_shadow_release,
  unit_r35_raeume_hundeschatten). Keine volle Suite im Spurbaum (Vorgabe).
  Stabilitaet des Fensterhakens: integration_r36_pri_schatten zwei weitere Male einzeln gefahren (unter Last durch
  parallele Agenten-exes re15_pc_h36abn/re15_pc_nb6j): Passed 28.02 s / 28.09 s -> 3 von 3 gruen.
* Sonde direkt gefahren (`build/tests/unit/test_r36_pri.exe absperrung|schatten`): "Marvin (3800,9900): Kasten x91..113
  y111..146, Maskenpunkte 123, verdeckend 123", Wegpunkte (4568,6941/7665/8365) verdeckend 0; "ROOM1100 Cut 2 Spieler
  (-15100,-18500): Schatten-Bucket 65 (Summe 16738), Maskenpunkte im Quad 21, davon UEBER dem Schatten 0", Cut 1 Bucket 64
  (Summe 16554) 23/0, "Zombie 5: Kasten 360 Punkte, verdeckend 360". Beide "OK". (Schwaeche: der Wegpunkt (5528,2510)
  projiziert auf y427..240 = leerer Kasten, die Pruefung "0 verdeckend" ist dort trivial.)

---

## Punkt 1 — "Ich habe dir ein neues PRI erstellt im ROOM 10F0 (02_02.png), weil mit der neuen Cutscene auffaellig ist, wie Marvin nicht ueberdeckt wird von der Absperrung."

### 1.1 Lasso gegen die gebaute Maske (Daten, ohne Spiel)

`python re15_port/tools/maske/vorschau.py ROOM10F0 2 --out <M>/vorschau` (= exakt die Pixel, die der Port blittet):
"ROOM10F0 Cut 2: 10.6 % des Bildes, 93 Rechtecke". `<M>/abn0_lasso_check.py` gegen `pri/STAGE1/10F0/02_02.png` (183x67,
alpha>0 7446 / alpha>=128 7262 / alpha==255 7029 Punkte) an x=33 y=56 und `02.png` (Stuhl) an x=176 y=115:

```
Maske: 8155 Punkte; Lasso Absperrung bbox (33,56,100,239); Stuhl-Lasso bbox (176,115,211,174)
Lasso-Punkte (alpha>=128) OHNE Maske: 0   (alpha>0: 168 = Antialias-Saum)
Maskenpunkte ausserhalb beider Lassos (zuviel): 0
beste Verschiebung des Lassos gegen die Maske im Umkreis +-4: dx=0 dy=0
```
Die Lage x=33/y=56 ist die beste; die Absperrung ist vollstaendig (jeder opake Lasso-Punkt) und ohne Zugabe gebaut.
Bild `<M>/vorschau/ROOM10F0/ROOM10F0_cut02.png` (Scheibe links, Stuhl rechts).

### 1.2 Szene an der exe (A = Masken, B = RE15_NO_PRI=1, VORHER = v0.8.23-exe mit alten Masken)

```
run_abn0.sh p1_a      re15_pc_abn0.exe        RE15_DEBUG_JUMP=10F0@250 RE15_STATE_LOG=state.txt RE15_FRAMEDUMP=900-1300/10:f RE15_EXIT_AT=1310#10F0
run_abn0.sh p1_b      re15_pc_abn0.exe        ... + RE15_NO_PRI=1
run_abn0.sh p1_before re15_pc_abn0_before.exe RE15_CD_ROOT=<hauptbaum>/shared_assets/PSX ...
debug.log (alle drei): "[debug-menu] AUTO-JUMP -> ROOM10F0 (Frame 250)", "[cut10f0] ROOM10F0: Szene gestartet (Ereignis 20,
Faden 10, Flag (9,71)=0)", "[flow] EXIT_AT: Bild 1310 in Raum 10f0 erreicht -> exit"; je 41 Framedumps, 1561 State-Zeilen.
p1_before/debug.log: "[asset] cd-root: C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX", "[pri] cut=2 pri_offset=0x61C
masks=27" (= alte Sektion, nur der Stuhl); p1_a: cd-root des Spurbaums.
```
State-Log (Bildnummern = Raum-Bilder, `g_engine.frame_count` wird beim Raumwechsel genullt, main.c:8115):
F1040 Marvin (Slot 2, t=40) @(4568,7665,r2861) = OESTLICH der Scheibenebene (Zelle 3 @0x6B4 x 2300..4100, Radius 450 ->
4550); F1200 @(3823,9862,r3423) = Stand (RE15_CUT10F0_MARVIN_X/Z = 3800/9900), Leon (4792,11354), Ada (6000,11500), cam=2,
Untertitel 11 "Hey Marvin, glad you made it!".

`python abn0_tools.py serie runs/p1_a runs/p1_b f 24` (Pixel mit Kanaldifferenz > 24, bbox):
```
f000920..f001030: n=4  bbox=(178,116)-(179,118)        = Stuhl-Atlas-Rundung, nicht Marvin
f001040:          n=12 bbox=(86,109)-(179,132)         = 8 Punkte x86..90 (ausschwingende linke Hand an der Scheibenkante) + Stuhl
f001050, f001060: n=4                                   (Marvin x=4568 vor/oestlich der Scheibe: frei)
f001070:          n=178 bbox=(85,85)-(179,153)         Marvin (3941,9668), Ecke umrundet: linke Seite hinter der Kante
f001080:          n=245 ; f001090: n=145 ; f001100..f001300: n=69..75, bbox (88,91)-(179,150)  = Stand: linke Schulter/Arm/Fuss
```
`python abn0_tools.py serie runs/p1_before runs/p1_b f 24` (v0.8.23 mit alten Masken gegen ohne Masken):
```
f000920..f001300: n=0..13, bbox um (119..136, 129..132)  = KEIN Marvin-Bereich; die alte Sektion (27 Rechtecke, Stuhl) deckt ihn nie
```
Bilder: `<M>/p1_F1200_A_B_diff.png`, `<M>/p1_F1070_A_B_diff.png`, `<M>/p1_F1040_A_B_diff.png` (A | B | Diff magenta):
in B (ohne Masken) steht Marvins linke Koerperhaelfte VOR der fernen Scheibenkante (der Befund des Nutzers), in A ist sie
von der Kante verdeckt, der Rest seines Koerpers bleibt sichtbar; waehrend des Laufs oestlich der Scheibe (F1030..F1060)
bleibt er frei (4 Punkte Stuhlrundung). Bild 1200 in A: Marvin steht hinter der Scheibe, Leon und Ada frei.

**Urteil Punkt 1: ERFUELLT.** Das Nutzer-Lasso 02_02.png ist an seiner Lage ohne Verschiebung und ohne Zugabe gebaut; an
der exe wird Marvin am Ende der Szene (Stand 3823/9862) von der Absperrung verdeckt (Bild 1200: 75 Punkte linke
Schulter/Arm/Fuss hinter der Kante), auf dem Weg oestlich der Scheibe nicht. Hinweis: der Spurbaum traegt die
10F0-Szene der Runde 35 (Basis master); die Kamera-Aenderung der Spur J (Kamera bleibt auf Leon) liegt nur im
Integrationszweig — Cut 2 und die Maske sind davon unabhaengig, die Integration misst das Zusammenspiel.

---

## Punkt 2 — "Dann habe ich noch mal einen einzelnen Marker gemacht fuer ROOM 1100. Hier wird Leon mit einer Halbtransparenz ueberdeckt mit Boden.... Das ist natuerlich falsch."

Vorbemerkung: die Marker-Datei des Nutzers liegt nirgends (eigene Suche im Hauptbaum: Bilder seit 2026-10-03 in Wurzel,
pri/, finding/, fehler/, shots/, masken_vorschau/, Cutscenes/ — nur map*.bmp, shotgun.bmp, 02_02.png und Cutscenes-
Material; `git log -- fehler/error.png finding/` nur cc2dde11). Die exakte Stelle ist damit nicht pruefbar; gemessen wird
der Wortlaut an ALLEN erreichbaren Cuts. Vokabel-Beleg: Commit bd4cfa4c (2026-09-22, ROOM10F0) — "Leon ist da grossteils
transparent" hiess beim Nutzer: Maskenpixel (Hintergrund-Re-Blit) ueber der Figur. "Halbtransparenz ... mit Boden"
passt demnach auf Bodenpixel einer Maske ueber etwas Leon-Zugehoerigem.

### 2.1 Bestand (Daten)
* ROOM1100.RDT: 13 Cuts, Kamera-Tabelle @0x60, ALLE pri_offset -> gc=mc=0xFFFF (keine Original-Masken, eigenes Python).
  Port-Masken (MASKS/ROOM1100.MSK) nur Cut 1 und 2 (`vorschau.py ROOM1100`: "2 Cuts geschrieben") -> in den Cuts 0, 3..12
  kann nichts ueberdeckt werden.
* Alte gegen neue Maskenpixel (vorschau.py aus dem Hauptbaum = master-Masken, gegen den Spurbaum), `<M>/vorschau_alt`
  vs `<M>/vorschau`: Cut 1 alt 28160 / neu 27450 px, entfernt 710, hinzu 0; Cut 2 alt 25211 / neu 24358, entfernt 853,
  hinzu 0. Die entfernten Pixel bilden einen 1..3 px breiten Streifen am Fuss der Westwand/Nordwand/Suedblock AUF DEN
  BODENFLIESEN (Bilder `<M>/p2_masken_alt_neu_cut1_zoom.png`, `..._cut2_zoom.png`: magenta = entfernt, cyan = neu;
  Hintergrund derselben Stelle `<M>/p2_bg_cut1_zoom.png`/`..._cut2_zoom.png`): die neue Maskenkante liegt auf der gemalten
  Sockelunterkante, die alte lag davor auf dem Boden. Das bestaetigt die Zellenpolster-Diagnose des Dossiers (2.2c)
  qualitativ; die Zahlen -15690/-15710 sind Messwerte der Spur (eigene Kantenmessung am Hintergrund war wegen des
  geringen Kontrasts Boden/Sockel nicht belastbar, Bild-Zoom entscheidet: [[reai-v2-tabelle-vs-bild]]).

### 2.2 Spiel: drei Bahnen durch den N-S-Flur, A = Masken, B = RE15_NO_PRI=1, je neue exe und v0.8.23-exe (alte Masken)
Vorwaerts-Konvention gemessen (Kalibrierlauf rot=0: Leon lief von x=-15150 nach -13068, Kehre auf rot 1920 lief nach
-x/-z) -> Vorwaerts = (cos rot, -sin rot), Nord (+z) = rot 3072. Laeufe (`<M>/p2_runs2.sh`, `p2_runs3.sh`):
```
RE15_DEBUG_JUMP=1100@250 RE15_PLAYER_POS=<x>,-25000,3072 RE15_INPUT_SCRIPT=U7,R0.67,U7 RE15_INPUT_SCRIPT_BASIS=s
RE15_INPUT_SCRIPT_START=60 RE15_STATE_LOG=state.txt RE15_FRAMEDUMP=60-520/10:f RE15_EXIT_AT=530#1100  (x = -15150 Westwand,
-14100 Mitte, -13050 Ostwand); Bahn: z -24925 -> -10168 (Cuts 1 -> 2 -> 3), Kehre, zurueck bis (-13068,-23679) (Cuts 3 -> 2 -> 1)
```
Auswertung `<M>/abn0_lane2.py`: je Bild Leons Koerperkasten (Projektion ueber die RDT-Kamera des geloggten Cuts, Fuss..Kopf
1500, halbe Breite 450 + 8 px), Differenzpunkte A/B (> 24) nach Herkunft in B: "Schatten" (B = Hintergrund minus v in allen
Kanaelen gleich, v 8..120 = subtraktiver Blob) / "Koerper" (alles andere); die festen Gegner-Rechtecke hinter den Ecken
(Cut 1 x165..189 y61..144, Cut 2 x127..141 y127..139 — in den B-Bildern als Zombies bestaetigt, sie SOLLEN verdeckt sein)
sind aus dem Kasten herausgenommen.
```
                                   im Koerperkasten            ausserhalb (= die Gegner hinter den Ecken)
NEU  Westbahn  x=-15150 (A/B)      Schatten 0   Koerper 0      96 / 3064
NEU  Mitte     x=-14100            Schatten 0   Koerper 0      126 / 4221
NEU  Ostbahn   x=-13050            Schatten 0   Koerper 0      126 / 4221
ALT  Westbahn  x=-15150 (v0.8.23)  Schatten 61  Koerper 53     113 / 3423   Bilder F90, F150 (Cut 1), F160..F210 (Cut 2)
NEU  Westbahn  RE15_NO_SHADOW=1    Schatten 0   Koerper 0      96 / 3064
ALT  Westbahn  RE15_NO_SHADOW=1    Schatten 0   Koerper 0      113 / 3423
```
Lesart: im Auslieferungsstand lagen an der Westwand (Cut 2, F170: 23 Schatten- + 12 "Koerper"-Punkte bei x97..106
y181..191; F180 11+16; F200 8+8; F210 9+4) Bodenpixel der Wandmaske ueber Leons halbtransparentem Schatten und ueber dem
vom Schatten abgedunkelten Schuhrand (Bilder `<M>/p2_bilder/p2n_lane_-15150_alt_a_f000170_c2.png`, `..._f000180_c2.png`:
magenta links neben Leons Fuss an der Wand). OHNE Schatten (RE15_NO_SHADOW=1) deckt auch die alte Maske KEINEN
Koerperpunkt — das Ueberdeckte war ausschliesslich der Schatten (und sein Rand auf dem Schuh). Das ist der einzige
Leon-bezogene Befund dieses Musters im Raum, und er deckt sich mit dem Wortlaut (Halbtransparenz = der ABR2-Schatten,
Boden = die Bodenpixel der Wandmaske). Mit dem neuen Stand: in allen drei Bahnen, beiden Richtungen, Cuts 1/2/3,
0 Punkte im Koerperkasten (Schatten und Koerper), die Gegner hinter den Ecken bleiben verdeckt.

### 2.3 Standplaetze an der Westwand (wie integration_r36_pri_schatten), Bild 150, RE15_FORCE_CUT
```
Cut 2 (-15100,-18500,rot -1024): NEU  Kasten Schatten 0 Koerper 0; Schattenfenster x40..120 y185..240: 0 px; ganzes Bild 56 px = Zombie-Ecke (127..139,127..139)
                                 ALT  Kasten Schatten 13 Koerper 7 (x64..66 y227..229); Schattenfenster 20 px (bbox 60,227..67,236); ganzes Bild 85 px
                                      (= die "85 Punkte" des Dossiers 2.2b, reproduziert)   Bilder <M>/p2_stand_c2_ALT_A_B_diff.png / ..._NEU_...
Cut 1 (-15100,-16900,rot -1024): NEU  0 / 0 (ausserhalb 161 = Gegner);  ALT 0 / 0 (ausserhalb 179)
```
Hinweis zur Auswertung: EXIT_AT beendet Bild 150, bevor dessen State-Zeile geschrieben ist; der Cut der Standlaeufe wurde
aus debug.log ("[walk] F60 cut=2") genommen (--cut), nicht aus state.txt.

### 2.4 Nebenwirkung der Schattenordnung (alle Raeume): alte exe ohne Masken gegen neue exe ohne Masken, gleiche Bahn
`abn0_tools.py serie runs/p2n_lane_-15150_alt_b runs/p2n_lane_-15150_b`: 1..111 Punkte je Bild an Leons Fuessen (F160 74,
F250 111, F320 81; Cut 3 bis y=236). Zoom `<M>/p2_schattenordnung_F000250_zoom.png`, `..._F000160_zoom.png` (links alt,
rechts neu): der untere Teil der Schuhe wird jetzt vom Schattenblob abgedunkelt (Polygone im selben OT-Bucket wie der
Schatten liegen unter ihm, 2.3 des Dossiers / RE-Gate 4). Das ist eine sichtbare Aenderung in JEDEM Raum und folgt
der OT-Analyse; am Original wurde sie von der Spur nicht gemessen (Dossier OFFEN). Eigene Gegenprobe am Original s. 2.5.

### 2.5 Gegenprobe am Original (DuckStation-Savestates, VRAM ueber re15_ss.py, Anzeigepuffer x448..768 y0..240)
Posen aus dem RAM (player 0x800aca54: x +0x34 = 0x800aca88, z +0x3c = 0x800aca90, rot +0x6a = 0x800acabe):
doorA_square.sav (-1300,-13950) rot 2048 = Flur vor dem Briefing Room, room1140_entry.sav (-7600,-17600) rot -96,
orig_lamp_start.sav (57536,-58536) rot 1024. Port (neu und alt) an denselben Posen (`<M>/p3_runs.sh`: RE15_DEBUG_JUMP
1130/1140/1170@250 + RE15_PLAYER_POS, Bild 150), Auswertung `<M>/p3_eval.py`:
```
doorA  Port alt vs neu: 21 Punkte, bbox (181,154)-(190,157) = Unterkante des Schuhs; gleicher Cut wie das Original (Lagekontrolle)
e1140  Port alt vs neu: 0 Punkte (Pose ohne Bucket-Gleichstand, nicht unterscheidend)
lamp   Port zeigt eine andere Szene als der Savestate (Intro-Ablauf 1170) -> nicht vergleichbar
```
Pixelraster doorA, Spalten 176..195, Zeilen 148..161 (Helligkeit): in der Differenzzone (x181..190, y154..157) ist das
ORIGINAL dunkel (Mittel 14,1; Zeilen 154..157 bei x176..187 ueberwiegend 0 = Blob), der Port ALT hell (26,0 = Schuh/
Boden ueber dem Blob), der Port NEU dunkel (1,3 = Blob ueber der Schuhunterkante). Das Original liegt also beim NEUEN
Stand (Bild `<M>/p3_doorA_orig_alt_neu_zoom.png`: Original | Port alt | Port neu). Vorbehalt: 1..3 px Versatz von
Blob und Pose (Atemzyklus, Blob-Zentrum +0x34/+0x3c) — eine Pose, kein Beweis fuer alle Buckets, aber die einzige
Originalmessung, die es zu dieser Regel gibt, widerspricht ihr nicht, sondern stuetzt sie.

### 2.6 Zusatz: Zombiekopf an der Suedecke (Dossier 2.2e)
`<M>/p2_suedecke_F60_zoom.png` (links v0.8.23, rechts neu; Cut 1, Leon (-15150,-24925)): der Kopf des kriechenden
Zombies an der Suedecke ist rechts vollstaendiger (17 Differenzpunkte, bbox 147..179 / 83..143), wie im Dossier.

### 2.7 Eckplaetze (Koerper teils hinter der gemalten Ecke), Bild 150, A/B je alte und neue exe (`<M>/p4_runs.sh`)
```
Suedecke (-15500,-23700) rot 3072, Cut 1 (Spielwahl): Leons Kasten x143..176 y111..150; Differenz A/B im Kasten ALT 109 / NEU 90 = ausschliesslich
    der Gegner hinter der Ecke (x165..175) — Leon selbst steht im Bild rechts der gemalten Ecke frei (Zoom <M>/p4_sued_zoom_altA_altB_neuA_neuB.png)
Nordecke (-15500,-11500) rot 1024, RE15_FORCE_CUT=2 (im Spiel zeigt die Zone dort Cut 3; Daten-Probe): Kasten x129..162 y105..143;
    Differenz A/B ausserhalb des Gegner-Rechtecks: ALT 185 Punkte (x131..140 y83..126) / NEU 147 (x131..138 y88..126) = Leons westliche
    Koerperhaelfte hinter der Ecke (physisch richtig), die ALTE Maske nahm 38 Punkte MEHR von ihm (den Streifen VOR der gemalten Ecke =
    Zellenpolster), die neue Kante liegt auf der gemalten Ecke (Zoom <M>/p4_nord_zoom_altA_altB_neuA_neuB.png: alt A | alt B | neu A | neu B)
```

**Urteil Punkt 2: ERFUELLT** (mit Vorbehalt H5: die Marker-Datei fehlt, die exakte Stelle des Nutzers ist nicht pruefbar).
Gemessen am Wortlaut: im Auslieferungsstand (v0.8.23-exe + alte Masken) wurde Leon-Zugehoeriges von BODENPIXELN der
Wandmasken ueberdeckt — sein halbtransparenter Schatten am Wandfuss (2.2/2.3: Westbahn F170..F210 bis 35 Punkte je Bild,
Standplatz Cut 2 20 Punkte im Schattenfenster, 85 im Bild) und an der Ecke 38 Koerperpunkte vor der gemalten Wand (2.7).
Im neuen Stand: in allen drei Bahnen beider Richtungen (Cuts 1/2/3), ohne und mit Schatten, an beiden Wandstandplaetzen
und beiden Ecken 0 Punkte Schatten/Koerper unter einer Maske, die nicht hinter der gemalten Wand liegen; die Gegner hinter
den Ecken bleiben verdeckt (F595/F205), der Zombiekopf an der Suedecke ist frei (2.6). Beide Ursachen sind behoben und
belegt: Maskenkante auf der gemalten Wandbasis (2.1, Daten) und Schatten in der OT-Ordnung (RE-Gate 1-4, Original
gestuetzt 2.5). Cuts 0 und 3..12 tragen keine Masken (2.1) — dort kann nichts ueberdecken.


---

## RE-Gate (selbst disassembliert, `info/Re1.5/PSX.EXE` ueber `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`)

1. **Schatten-Prim in die OT @0x8001b2c8..0x8001b320 (FUN_8001b064)**: `8001b2c8 jal 0x80065d10` = RotAverage4 (Ghidra-Label,
   XREF[1] FUN_8001b064:8001b2c8; Koerper 0x80065d10: 3 lwc2-Paare, rtpt 0x4a280030, 4. Vertex rtps 0x4a180001,
   `0x4b68002e` = AVSZ4, `0x48023800` = mfc2 OTZ) -> `8001b2d8 srl v0,v0,4` -> `8001b2e0 sll v0,v0,2` -> `8001b30c lw a0,16(a1)`
   / `8001b310 lw v1,0(v1)` (DAT_800aa6d8 + frameflip<<12 + idx*4) / `8001b314 and` / `8001b318 and` / `8001b31c or` /
   `8001b320 sw a0,16(a1)` = Kopf-Einhaengung (inline addPrim). OT-Index des Schattens = otz>>4: BESTAETIGT.
   Einziger Aufrufer: Ghidra `FUN_8001b064 XREF[1]: FUN_80031c44:80031dcc(c)`, Disasm `80031dcc jal 0x8001b064`: BESTAETIGT.
2. **ZSF3/ZSF4 @0x80066c70..0x80066c80**: `80066c70 addiu t0,zero,341` / `80066c74 .word 0x48c8e800` (ctc2 t0,cr29 = ZSF3) /
   `80066c7c addiu t0,zero,256` / `80066c80 .word 0x48c8f000` (ctc2 t0,cr30 = ZSF4): BESTAETIGT. Damit otz(AVSZ4) =
   (256*(sz0+..+sz3))>>12 = Summe>>4, Bucket = (Summe>>4)>>4 = Summe>>8 (re15_schatten_bucket).
3. **Masken-OT @0x80039650..0x80039660 (FUN_80039590)**: `80039650 lh a0,2(s3)` (depth) / `80039658 sll a0,a0,2` /
   `8003965c addu a0,a0,s6` / `80039660 jal 0x8006b538` (AddPrim): Wortindex = depth: BESTAETIGT.
4. **Einhaenge-Reihenfolge im Bildaufbau**: `8001ce0c jal 0x80031c44` (Spieler, am Ende der Schatten) < `8001ce54 jal 0x80039590`
   (Masken; uebersprungen nur, wenn Bit 0x100000 in 0x800aca38 gesetzt) < `8001d09c jal 0x8001e8c8` (Spieler-Polygone) <
   Figuren-Schleife `8001d0e8..8001d164` (`8001d108 jal 0x8001e8c8`). Zwischen 0x8001ce5c und 0x8001d0a0 nur Vorwaerts-
   Spruenge (beq 0x8001d088 / cf38 / cf74 / cfb0), keine Rueckspruenge: die Reihenfolge haelt. Mit ClearOTagR + Kopf-
   Einhaengung (Memory sprite-pri-occlusion: ClearOTagR n=0x400 @0x80020c6c, DrawOTag &ot[1023] @0x800215d0) wird im
   selben Bucket das zuletzt eingehaengte Prim zuerst gezeichnet -> Spieler-Schatten liegt bei Gleichstand ueber Maske
   und Spieler-Polygonen. Die Port-Regeln in re15_pri_schatten_r36.h entsprechen genau dem.
5. ABR2/SetSemiTrans: `8001afa8 jal 0x8006b5ec` (SetSemiTrans, a1=1) / `8001afb4 ori a1,zero,0x2` (ABR=2 fuer GetTPage) —
   im Header als "SetSemiTrans @0x8001afb4" zitiert; die Adresse ist das ABR-Argument, nicht der SetSemiTrans-Aufruf
   (0x8001afa8). Substanz (halbtransparent, subtraktiv) stimmt (Memory reai-v2-char-shadow): Zitier-Unschaerfe, kein Defekt.
6. SCA-Zelle ROOM10F0 @Datei-Offset 0x6B4: `xxd` -> `0807 2035 fc08 e8ea 01ff 0003` = w 1800, d 13600, x 2300, z -5400:
   BESTAETIGT. SCA-Zellen ROOM1100 (geom.sca_zellen): (-17600,-23200,2000,11100) Westwand, (-28900,-14100,13300,2000)
   Nordwand, (-22700,-23201,7100,1897) Suedblock, (-23105,-28356,1947,6814): BESTAETIGT. Original-Masken ROOM1100: alle 13
   Cuts pri_offset -> gc=mc=0xFFFF (eigenes Python ueber die Kamera-Tabelle @0x60): BESTAETIGT, keine Original-Sektion.
7. Suche im Code-Diff nach deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME|approx: keine Treffer; getenv nur
   der bestehende Messhaken RE15_NO_SHADOW (jetzt einmal gecacht). Keine neuen Env-Schalter.
8. Konstanten ohne @0x: Wandbasis -15690 (C1) / -15710 (C2) und Lasso-Lage x=33/y=56 sind MESSWERTE (Hintergrund bzw.
   Nutzer-PNG), nicht Code-Konstanten; Lage selbst nachgemessen (1.1), Wandbasis s. Punkt 2.

## Vertrag

* Bank-9-Bits, Nachrichten-IDs, Ereignis-Nummern: keine verwendet (Spur O hat laut VERTRAG keine; Diff enthaelt keine).
* Pfade: keine release/, platform/android/, Test-CMakeLists. `shared_assets/PSX/MASKS/*` ist geaendert: das sind die
  R15M-Seitendaten des Ports (die Original-CD `info/Re1.5/PSX/` hat kein MASKS/; der Vorrang der Original-Sektion bleibt),
  die einzige Schreibstelle der pri-Pipeline (raum.py CD=re15_port/shared_assets/PSX), deren Eigentuemer O laut VERTRAG
  1.4 ist — kein Original-Patch. Fuer das Paket-Gate (Dossier 6) sind die 5 Dateien genannt.
* Gemeinsame Dateien: main.c nur zwei 2-Zeilen-Haken (Tag vor dem Schattenaufruf). render_pc.c: +115/-20 Zeilen (neue
  statische Flush-Funktion + Umbau der Masken/Dreieck-Schleife) — mehr als "kleine Haken"; der Ordnungs-Durchlauf liegt
  naturgemaess in end_frame. Merge-Risiko: `r36/integration` aendert render_pc.c ebenfalls bei @66 / @706 / @723 / @926
  (dieselben Stellen: Schatten-Statik, re15_render_shadow_quad_c, end_frame) und main.c (+71) -> Konflikte beim
  Zusammenfuehren wahrscheinlich, beide Seiten behalten (s. Hinweise).
* Tests je Punkt vorhanden und messend: P1 unit_r36_pri_absperrung (echte Leser, Marvins Stand >= 60 verdeckende Punkte,
  Wegpunkte 0); P2 unit_r36_pri_schatten (Bucket-Grenzen + ROOM1100.MSK) und integration_r36_pri_schatten (exe, A/B).

---

## Gates und Gesamturteil

| Gate | Befund |
|---|---|
| Punkt 1 (ROOM10F0 Absperrung) | ERFUELLT (1.1 Lasso 0 fehlend / 0 zuviel an x=33 y=56; 1.2 Marvin am Szenenende hinter der Scheibenkante, auf dem Weg frei) |
| Punkt 2 (ROOM1100 Halbtransparenz/Boden) | ERFUELLT (2.2 Bahnen 0/0 in Leons Kasten, 2.3 Schattenfenster 0 px, 2.7 Ecken; Vorher-Stand mit alter exe reproduziert: Schatten 20/85 px, Ecke +38 Koerperpunkte) |
| RE-Gate | haelt: alle zitierten Adressen selbst disassembliert und bestaetigt (RE-Gate 1-6), keine Guess-Tells, keine neuen Env-Schalter; Messwerte (Lasso-Lage, Wandbasis) als Messwerte gekennzeichnet und nachgemessen |
| Vertrag: Bits/IDs/Ereignisse | haelt (keine verwendet) |
| Vertrag: Pfade | haelt mit zwei dokumentierten Abweichungen (H1, H2) |
| Vertrag: Tests je Punkt | haelt (unit_r36_pri_absperrung, unit_r36_pri_schatten, integration_r36_pri_schatten 3/3 gruen) |
| Nebenwirkungen | Schattenordnung in allen Raeumen (2.4) — durch Disassembly belegt und an einer Originalpose gestuetzt (2.5); Gegner hinter den Ecken bleiben verdeckt; Standbild-Weg (r32_tor) gruen |

### Hinweise (kein Mangel, fuer Integration/NACHLAUF)
* **H1 render_pc.c-Umfang.** +115/-20 Zeilen in einer gemeinsamen Datei (VERTRAG 1.4: "nur kleine Haken 1-5 Zeilen"):
  neue statische Funktion `pc_masken_schatten_flush`, Tag-Felder, Umbau der Masken/Dreieck-Schleife. Inhaltlich gehoert
  der Ordnungs-Durchlauf in end_frame; aber `r36/integration` (Spur E, d047fa97: SHADOW_QUAD_MAX -> RE15_ACTOR_MAX,
  `s_shadow_verworfen`, Messhaken RE15_SCHATTEN_LOG) aendert dieselben Stellen (@66 Statik, @706/@723 Quad-Funktionen,
  @926 end_frame) und main.c (+71). Beim Zusammenfuehren: beide Seiten behalten; O's Felder `s_shadow_vz/s_shadow_spieler
  [SHADOW_QUAD_MAX]` und `sh_order[SHADOW_QUAD_MAX]` folgen dem E-Makro automatisch; E's Zaehler-Zeilen in den Quad-
  Funktionen VOR O's Tag-Zeilen lassen. Danach unit_r36_pri_* + integration_r36_pri_schatten + integration_r36_gegnerzahl
  Lauf e (21 Schatten) fahren.
* **H2 MASKS/ unter shared_assets/PSX.** Fuenf Dateien neu/geaendert (ROOM10F0.MSK, ROOM10F0_PRI02.TIM, ROOM1100.MSK,
  ROOM1100_PRI01.TIM, ROOM1100_PRI02.TIM; Beiblaetter .PBM/.STAND/.BELEG nur fuer Tests). Port-eigene R15M-Seitendaten
  (Original-CD ohne MASKS/, Original-Sektion hat Vorrang), einzige Schreibstelle der pri-Pipeline, Spur O ist deren
  Eigentuemer — kein Original-Patch, aber ins Paket-Gate aufnehmen (Pakete Windows/Linux/Android muessen die neuen
  MSK/TIM tragen; die Android-Asset-Gate-Pins aus Runde 34a pruefen).
* **H3 Schattenordnung global.** Die Regel (Schatten-Bucket = Summe der vier Ecken-vz >> 8, Gleichstand nach Einhaenge-
  Reihenfolge) ist disassembliert (RE-Gate 1-4) und an EINER Originalpose gestuetzt (2.5); sie veraendert in jedem Raum
  die Schuhunterkante im gleichen Bucket (2.4, bis 111 Punkte je Bild in ROOM1100 Cut 3). Empfehlung fuer den
  Release-Notes-Entwurf: als Aenderung nennen ("Figurenschatten liegt in der Zeichenordnung des Originals"); bei einem
  Nutzer-Einwand ist die Gegenregel eine Zeile (re15_pri_schatten_r36.h, ist_spieler-Gleichstand "unten").
* **H4 Zitat 0x8001afb4.** Im Header als "SetSemiTrans @0x8001afb4" zitiert; dort steht `ori a1,zero,0x2` (ABR-Argument),
  der SetSemiTrans-Aufruf ist `8001afa8 jal 0x8006b5ec`. Substanz richtig, Adresse eine Zeile daneben.
* **H5 Marker-Datei ROOM1100 fehlt** (Nutzerfrage bleibt, Dossier 6). Was es im Raum Leon-bezogen zu ueberdecken gab,
  war der Schatten am Wandfuss (2.2); liefert der Nutzer die Datei, ist die Stelle gegen 2.2/2.3 in Minuten nachpruefbar.
* **H6 Gleichstand Maske/Figur in re15_pri.h** ("gleicher Bucket = Figur obenauf", Begruendung "Masken sind die letzten
  Prims"): nach RE-Gate 4 werden die Spieler-Polygone @0x8001d09c NACH den Masken @0x8001ce54 eingehaengt — im gleichen
  Bucket laege die Maske OBEN. Vorbestand (Runde 19c), nicht Teil des Wortlauts; Ein-Bucket-Nuance fuer NACHLAUF.
* **H7 Szene 10F0 im Spurbaum = Runde-35-Fassung** (Kamera-Aenderung der Spur J nur im Integrationszweig); Cut 2 und die
  Maske sind davon unabhaengig, die Integration misst das Zusammenspiel (integration_r35_cut10f0 dort fahren).
* **H8 Unit-Test-Schwaeche:** Wegpunkt (5528,2510) in unit_r36_pri_absperrung ergibt einen leeren Kasten (y427..240) —
  die Pruefung ist dort trivial; die drei Wegpunkte x=4568 tragen den Riegel.

### Gesamturteil
**BESTANDEN.** Punkt 1 erfuellt, Punkt 2 erfuellt (Vorbehalt H5), RE-Gate haelt, Vertrag haelt mit den dokumentierten
Abweichungen H1/H2 (Integration: Konflikte mit Spur E in render_pc.c/main.c von Hand, beide Seiten behalten; Paket-Gate:
fuenf MASKS-Dateien). Keine Maengel. Ergaenzung fuer den Release-Notes-Entwurf: PRI-Absatz (10F0-Absperrung, 1100-Wandfuss)
+ Satz zur Schattenordnung (H3).
