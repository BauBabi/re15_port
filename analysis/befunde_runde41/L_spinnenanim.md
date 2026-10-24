# Runde 41 — Spur L "spinnenanim" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_spinnenanim`, Zweig `r41/spinnenanim`, Basis e5c914fe (= master 4c667c97 + AUFTRAG/VERTRAG).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Zeile 47)

> Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch nicht ganz.

## 0. Stand / Protokoll

- 2026-10-24 ~22:00: Start (kein Vorgaenger-Stand in diesem Zweig). Dossier angelegt (c5c244b7).
- 22:30 Bau des Spurbaums (local_build.sh configure + build, OK). Port-Messung ROOM2060 (1.1/1.2).
- 22:55 RE-Belege Zeichenweg (2.1-2.3) — Original-Messung mit Bildern folgt (1.3).
- 23:00-23:25 Original gemessen (GDB-Zahlen + Video, 1.3/1.4); Massstab 0xccc gefunden (2.2).
- 23:45 Umsetzung (3) committet 380cf999 (ungebaut), Bau laeuft.

## 1. Messung vorher

### 1.0 Werkzeug
- Port: echte exe als Messkopie `re15_port/build/platform/pc/re15_pc_r41L_mess.exe`, Laufordner
  `re15_port/build/r41L_lauf/<name>/`, Laufskript scratchpad `L/lauf.sh` (Muster R38 `R_belege/lauf.sh`).
  Messschiene `RE15_SPINNEN_LOG` (Runde 38, je Bild Zustand/Clip/Bild/Lage/Drehung je Spinne), `RE15_POSE_DUMP`
  (je Bild der GEZEICHNETE Keyframe `kf` je Figur), Bilder `RE15_FRAMEDUMP` mit `RE15_WINDOW_SCALE=1`.
- Lauf `v2_2060`: `RE15_DEBUG_JUMP=2060@100` (Leon an der Tuerlage aus 2030 (7600,0,3950), Cut 0), ab Raumbild 150
  `RE15_POCC_TP=150,6000,3000,1024` (Spieler gleitet in 11 Bildern auf (6000,3000) = dieselbe Lage, auf die die
  Original-Messung R38 `R_belege/orig_2060_schacht_gift.txt` bei Bild 60 teleportiert), KI = Standard (RE2).
  (Lauf `v1_2060` verworfen: RE15_POCC_TP latcht seinen Start schon im Boot-Raum bei Bild 60.)

### 1.1 Port: Ablauf (Logik) == Original, Bild fuer Bild
Port `v2_2060` Spinne Platz 1 gegen Original R38 (`orig_2060_schacht_gift.txt`, e0; Original-Zeile nN = Zustand NACH
dem Takt von Bild N-1, Port-Zeile FN = Zustand nach dem Takt von FN):
| Phase | Original | Port | gleich? |
|---|---|---|---|
| Ausloeser grid 0x41->0x42 | n62 | F155 | Takt 0 |
| 1 Kriechen Clip 0x10 (RE2 15), z -200/Bild, Gitter-Ausschiebung x 5800->5975 | n63-n73 z 6200..4325 | F155-F165 z 6200..4325 | ja (Werte gleich) |
| 1 Stand bis Bild 0x1f | n74-n93 | F166-F185 | ja |
| 2 y+1000, z+300, rot_z += 1024 | n94 (5975,-2600,4625) r(0,1024,1024) | F186 (5975,-2600,4625) r(0,1024,1024) | ja |
| 3/4 Clip 2, y += 100+(rng&15) bis > -1800 | n95-n102 | F187-F194 | ja (rng-Schritte 102-111) |
| 5/6 Clip 0x0d (RE2 14) bis y >= -500, Bild 0x1f | n103-n133 | F195-F225 | ja |
| 7 rot_z -= 1024, y := 0, z -= 1488, Spieler-Ausschiebung | n134 (5740,0,4332) | F226 (5740,0,4332) | ja |
| 8 Clip 0, ACTIVE | n135 | F227 | ja (danach RE2-Gehirn, Port-Wahl R38) |
=> Die Choreografie (Zustaende, Clips, Bildzaehler, Wurzelbahn, Drehwerte) ist bereits byte-gleich. Die Abweichung
liegt NICHT in der Logik, sondern im ZEICHNEN (1.2, 2.x).

### 1.2 Port: was gezeichnet wird (RE15_POSE_DUMP + Code)
- **(a) rot_z wird nicht gezeichnet.** Der Gegner-Zeichner `platform/pc/main.c` baut die Figur-Matrix fuer alle Typen
  ausser 0x37 nur aus der Gier: `nyaw = {c,0,s, 0,1,0, -s,0,c}` (main.c ~Z. 10168-10176, "Yaw-only (nyaw) ==
  RotMatrix(0, yaw, 0)"). Die Spinne steht in Phase 2-6 (F186-F225, 40 Bilder = 1,3 s) mit rot_z = 1024 im Speicher,
  wird aber WAAGRECHT gezeichnet -> sie "schwebt" mit den Beinen nach unten an der Wand herab statt kopfueber an der
  Wand zu kleben. Dasselbe gilt fuer alle RE2-Wand-/Deckenlagen der Spinne (enemy_ai_re2_spider.c schreibt rot_z = 1024
  @0x80101D34-3C, 2048 @0x8010048C / @0x801026EC-F0, rot_x = Gier @0x80101D24-30) — alle werden aufrecht gezeichnet.
- **(b) Gezeichnetes Bild = Bild NACH dem Vorschub.** `lauer_anim` (spinnen_r38.c) zaehlt anim_frame hoch, der Zeichner
  posiert danach mit diesem Wert: F155 zeichnet Clip 15 Bild 1 (kf 241), das Original zeichnet im selben Takt Bild 0.
  Im Umbruchbild F186 zeichnet der Port Clip 15 **Bild 0** (kf 240 = die Haltepose im Schacht, Bein b13 y -228) statt
  Bild 31 (kf 271): ein Ein-Bild-Sprung zurueck in die Startpose genau beim Uebergang auf die Wand; ebenso F226 Clip 14
  Bild 0 (kf 208) statt Bild 31 (kf 239) beim Absprung von der Wand. (Bilder: Kontaktbogen `L_belege/v2_port_vorher.png`.)
- **(c) Ueberblendung:** +0x8f = 7 wird beim Clipwechsel gesetzt (Phase 0/3/5); der Port zaehlt anim_frac im selben Takt
  schon auf 6 herunter (F155/F187/F195 frac = 6). Wirkung auf die Pose: 2.3.
- **(d) Massstab fehlt:** main.c skaliert mit render_scale_q12 (+0x166), die Spinne setzt ihn nie (enemy_ai_common.c INIT-
  Kommentar "+0x166=0xccc ... unidentified consumer, no port field, OPEN"). Beleg 2.2.
### 1.3 Original: GDB-Zahlen je Takt + Video (DuckStation, MZD-Disc, ROOM2060)
Werkzeuge `L_belege/gdb_shots.py` (Sprung + Teleport wie R38 `gdb_spinne.py`, je Takt Haltepunkt 0x8001a50c, Zeile mit
Zustand/Lage/Drehung/Clip/+0x95/+0x8f, **Figur-Matrix +0x20**, **Weltmatrix Teil 0 (+0x188)+0x40**, Wurzel-Versatz Teil 0
+0x38) und `L_belege/gdb_video.py` (dasselbe + Media Capture FFmpeg/mp4, Hotkey ToggleMediaCapture voruebergehend auf
SDL-0/LeftShoulder; settings.ini byte-genau zurueck, "settings.ini zurueck: True" in jedem Lauf).
- Zahlen: `L_belege/orig_2060_gdb.log` (Takt 1..150). Zwei Laeufe (orig1/orig2) in 141 von 141 Zeilen byte-gleich
  (deterministisch). Zeile nK = Stand NACH Takt K-1 (Haltepunkt am Anfang der Gegner-Schleife); Port-Takt = Original-Takt + 93.
- Video: `L_belege/orig_2060_schachtspinne.mp4` (201 Bilder = 201 VSyncs von Takt 56 bis 150, je VSync genau ein
  Videobild auch im Haltepunkt-Betrieb: `L_belege/orig_2060_video.log` VSync 5628 -> 5829). Zuordnung belegt: Bildwechsel
  nur an geraden Videobildern (2 VSyncs je Takt), erstes Cut-1-Bild = Videobild 30, Phase-2-Sprung (Takt 93) = Videobild 92
  -> Videobild k zeigt das Bild von Takt 62 + (k - 30)/2.
- Screenshots im Haltepunkt (Hotkey Screenshot, beide ScreenshotModes) zeigen den HALB gezeichneten Puffer (Hintergrund +
  Schatten, ohne Figuren; Gegenprobe Savestate-VRAM: Puffer y=0 halb, y=256 voll) - als Beleg verworfen, nur das Video zaehlt.

Original-Werte (Auszug; M = Figur-Matrix +0x20 zeilenweise, P0root = Wurzel-Versatz des posierten Keyframes):

| Takt | Phase | Clip/+0x95 nach | +0x8f nach | Lage | rot | M (+0x20) | P0root |
|---|---|---|---|---|---|---|---|
| 61 (Halten) | 0 | 16/1 | 0 | (5800,-3600,6400) | (0,1024,0) | [0,0,3276, 0,3276,0, -3276,0,0] | (0,-358,0) |
| 62 | 1 | 16/1 | 6 | (5800,-3600,6200) | (0,1024,0) | dto. | (0,-359,0) |
| 91 | 1 | 16/30 | 0 | (5975,-3600,4325) | (0,1024,0) | dto. | (0,606,0) |
| 92 | 1->2 | 16/31 | 0 | dto. | (0,1024,0) | dto. | (0,606,0) |
| 93 | 2 | 16/0 | 0 | (5975,-2600,4625) | (0,1024,1024) | **[0,0,3276, 3276,0,0, 0,3276,0]** | (0,-487,0) |
| 94 | 3/4 | 2/1 | 6 | (5975,-2489,4625) | (0,1024,1024) | dto. | (-19,-488,0) |
| 132 | 6->7 | 13/31 | 0 | (5975,-428,4625) | (0,1024,1024) | dto. | (-81,-1355,0) |
| 133 | 7 | 13/0 | 0 | (5740,0,4332) | (0,1024,0) | [0,0,3276, 0,3276,0, -3276,0,0] | (0,-487,0) |
| 134 | 8 | 0/1 | 0 | (5740,0,4333) | (0,1024,0) | dto. | (-146,-400,2) |

=> (a) **Massstab 0.8**: alle Eintraege 3276 = 0xccc = 0.8 x 4096 in jedem Takt 2..150. (b) **rot_z wird gezeichnet**: von Takt
93 bis 132 ist M = 0.8 x RotMatrix(0,1024,1024) (Kopf nach unten, Bauch zur Wand). (c) Die Wurzel des Clips 0x10 kippt schon
in Bild ~12..30 selbst ueber die Kante (P0W Takt 91: Wurzel lokal um Z gedreht, P0root y +606), Bild 31 ist wieder die
ungekippte Grundpose, die erst ZUSAMMEN mit rot_z (Takt 93) die Wandlage ergibt; Clip 0x0d kippt am Ende zurueck (P0W Takt 132 =
Gier-Lage bei rot_z 1024), Takt 133 nimmt rot_z wieder heraus.

### 1.4 Vergleich Bild fuer Bild (Original-Video gegen Port-Framedump, vorher)
`L_belege/vgl_vorher_takt62-100.png` und `L_belege/vgl_vorher_takt104-136.png` (links Original-Videobild 30+2(T-62), rechts
Port `v2_2060` Bild T+93, Ausschnitt um das Gitter, 2x):
- Port-Spinne durchgehend ~1,25x zu gross (Beine bis weit ueber das Gitter hinaus; Original passt in den Gitterrahmen).
- Takt 92 (Port F185): Port springt fuer ein Bild in eine WAAGRECHTE Pose oben am Gitter (Bild 31 einen Takt zu frueh),
  Original haengt noch kopfueber (Bild 30).
- Takt 93..132 (F186..F225): Port zeichnet die Spinne waagrecht vor der Wand auf Brust-/Kopfhoehe von Leon (Beine sternfoermig,
  "schwebt" herab); Original: Spinne kopfueber an der Wand, Unterleib oben, klettert hinter Leon an der Wand herunter bis zum
  Boden.
- Takt 133 (F226): Port Bild 0 des Clips 14 (Startpose des Wand-Boden-Uebergangs) statt Bild 31.


## 2. RE-Belege (Adressen, Bytes)

### 2.1 Das Original zeichnet JEDE Figur mit der vollen Drehmatrix aus +0x68/+0x6a/+0x6c (PSX.EXE, selbst disassembliert)
Zeichen-Schleife @0x8001d0e8-d164 (je aktive Figur, Stride 500): `andi v0,v0,0x1 / beq` @0x8001d0fc-100,
`jal 0x8001e8c8` @0x8001d108. FUN_8001e8c8:
| Adresse | Bytes / Instruktion | Bedeutung |
|---|---|---|
| 0x8001e8e8 | `lw a1,0(s2)` | a1 = aktuelle Figur (0x800ac784) |
| 0x8001e8f0 | `addiu a0,a1,104` | a0 = &Figur+0x68 = SVECTOR (rot_x, rot_y=Gier, rot_z) |
| 0x8001e8f4 | `26 a0 01 0c` = `jal 0x80068098` | **RotMatrix** (PsyQ-Signatur im Ghidra-Dump ghidra1_V2.txt Z. 97208; Funktionsanfang 0x80068098 `lh t7,r->vx(a0)` Z. 209977) |
| 0x8001e8f8 | `addiu a1,a1,32` (Delay) | Ziel = Figur+0x20 = MATRIX; deren t[] liegt bei +0x34/+0x38/+0x3c = x/y/z der Figur |
| 0x8001e904-940 | `andi 0x800` / ScaleMatrix | optionale Skalierung (+0x166), fuer die Spinne ohne Belang |
| 0x8001e990 | `jal 0x8001e9ec` | je Teil: `lw s0,108(s2)` @0x8001ea0c = Eltern-Matrix, `jal 0x80022da0` @0x8001ea24 (Eltern x lokal -> Teil+0x40) |
Live-RAM-Gegenprobe (`stage_saves/mzd_stage1_engage_live.sav`, 5 Zombies): Teil 0 +0x6c (Eltern-Matrix) == Figur+0x20
in 5 von 5 Figuren (z.B. e0 @800acc2c: part0 +0x6c = 800acc4c = e+0x20) -> der Wurzelteil haengt an der
RotMatrix(+0x68)-Matrix, die ganze Figur dreht mit rot_x/rot_z.
=> Port-Abweichung 1.2(a): main.c nimmt fuer Typ 0x25 nur RotMatrix(0, Gier, 0). Der Port hat den byte-true
RotMatrix-Zwilling bereits (`re15_skel_euler_matrix` = mat3_from_euler, Riegel test_rotmatrix gegen
RE_15_Quellcode_V2/RotMatrix.c) und nutzt ihn fuer Typ 0x37 (G5-Tentakel).
### 2.2 Massstab 0xccc (STAGE2.BIN INIT + PSX.EXE Zeichner, selbst disassembliert)

| Adresse | Bytes/Instruktion | Bedeutung |
|---|---|---|
| STAGE2 0x80110de0 / 0x80110de8 / 0x80110dec | `lw v0,0(v1)` / `ori v0,v0,0x800` / `sw v0,0(v1)` | INIT: Figur-Wort0 oder 0x800 |
| STAGE2 0x80110df8 / 0x80110dfc | `ori v0,zero,0xccc` / `sh v0,358(v1)` | INIT: +0x166 = 0xccc |
| PSX.EXE 0x8001e904 | `andi v0,v0,0x800` | FUN_8001e8c8: Flag-Tor |
| PSX.EXE 0x8001e91c / 0x8001e928 / 0x8001e938 | `lh v0,358(v1)` (3x) | VECTOR (+0x166, +0x166, +0x166) |
| PSX.EXE 0x8001e940 | `fc 97 01 0c` = `jal 0x80065ff0` ScaleMatrix | Figur+0x20 mal 0.8 (RE_15_Quellcode_V2/ScaleMatrix.c: m[i][j] = m[i][j]*v[j] >> 12) |

Zensus aller `sh rt,358(rs)` in STAGE1..5.BIN: in STAGE2 ausser dem gemeinsamen Block @0x80109650-850 (anderer Figur-Code)
nur @0x80110dfc, also im Spinnen-Bereich 0x801109e4-0x801158a0 der einzige Schreiber -> der Massstab gilt das ganze Leben der
Spinne. Laufzeit: M mit 3276-Eintraegen in Takt 2..150 (1.3). RE2 (EMS25.BIN/EMS26.BIN gescannt): kein Zugriff auf +0x176
(RE2-Massstabsfeld: RE2 PSX.EXE `lh v0,0x176(s1)` @0x80027308 vor `jal ScaleMatrix` @0x80027328) und kein `ori 0x800`
-> RE2 setzt fuer seine Spinne keinen Massstab (Entscheid siehe 3).

### 2.3 Gezeichnetes Bild und Ueberblendung (PSX.EXE FUN_8001f314 / FUN_8001f3bc, selbst disassembliert)

| Adresse | Instruktion | Bedeutung |
|---|---|---|
| 0x8001f324 / 0x8001f330 | `lbu v0,148(t0)` / `addu a2,a1,v0` | Clip +0x94 -> Clip-Kopf |
| 0x8001f35c / 0x8001f36c | `lbu v0,149(t0)` / `sw a2,360(t0)` | Bild = +0x95 (VOR dem Hochzaehlen) -> +0x168 |
| 0x8001f38c | `jal 0x8001f3bc` | Pose rechnen |
| 0x8001f408 | `lbu s0,143(v1)` | Ueberblendzaehler +0x8f lesen |
| 0x8001f534 | `jal 0x8001f664` | Keyframe des Bilds +0x95 entpacken |
| 0x8001f598 / 0x8001f5b0-b4 | `mult v0,v1` (0x200 x +0x8f) / `addiu v0,v0,-1; sb v0,143(v1)` | Mischgewicht mit +0x8f VOR dem Herunterzaehlen |
| 0x8001f5d0 | `jal 0x80020510` | Winkel mischen (Gewicht 0x1000 - 0x200 x +0x8f) |
| 0x8001f610-1c | `lbu v0,149 / addiu 1 / sb v0,149` | +0x95 ERST JETZT hochzaehlen |
| 0x8001f624-3c | `sltu v0,v0,s4` / `sb zero,149` | Umbruch auf 0 (Rueckgabe 1) |

=> gezeichnet wird in Takt T das Bild (+0x95 vor dem Takt) mit dem Mischgewicht (+0x8f vor dem Takt). Gegenprobe Laufzeit:
Takt 92 (+0x95 nach = 31) P0root (0,606,0) = Keyframe Bild 30; Takt 93 (+0x95 nach = 0) P0root (0,-487,0) = Bild 31 (1.3).


## 3. Umsetzung (Dateien, Konstanten)

Neue Datei `re15_port/engine/src/spinnenanim_r41.c` + `re15_port/include/re15_spinnenanim_r41.h`. Haken (je 1-3 Zeilen,
Kommentar "Runde 41 Spur L"):

| Datei | Haken | Zweck / Beleg |
|---|---|---|
| platform/pc/main.c (Gegner-Zeichner) | `if (npc->type == 0x37u \|\| re15_spinnenanim_r41_volle_drehung(npc)) re15_skel_euler_matrix(rot_x, Gier, rot_z, nyaw)` | Figur-Matrix RotMatrix(+0x68) wie FUN_8001e8c8 `jal 0x80068098` @0x8001e8f4 (2.1); der folgende Massstab-Block (render_scale_q12, ScaleMatrix @0x8001e940) war schon da |
| platform/pc/main.c | `re15_spinnenanim_r41_zeichnen_beginn(npc, &ncur, target_fps)` vor re15_compute_actor_kf, `..._zeichnen_ende(npc)` direkt nach re15_skel_compute_pose (Ergebnis in `npc_pose_rc`, `continue` danach) | posiertes Bild +0x95 und Ueberblendung +0x8f VOR dem Vorschub (2.3) |
| platform/pc/main.c | `re15_spinnenanim_r41_log_matrix(npc, nyaw)` nach dem Massstab-Block | Messschiene RE15_SPINNENANIM_LOG (env-gegatet, kein Verhalten): die Matrix, die der Zeichner WIRKLICH benutzt |
| engine/src/enemy_ai_common.c | `re15_spinnenanim_r41_init(e)` am Anfang von re15_adult_spider_ai_tick (vor dem RE2-Zweig) | Flag 0x800 + +0x166 = 0xccc (@0x80110de8-dec / @0x80110df8-dfc) -> render_scale_q12 |
| engine/src/enemy_ai_common.c | `re15_spinnenanim_r41_vor_anim(e)` am Anfang von re15_aspider_anim | RE1.5-KI: Bild/+0x8f vor dem Vorschub merken (INIT-Halten @0x80110d60) |
| engine/src/spinnen_r38.c | `re15_spinnenanim_r41_vor_anim(e)` am Anfang von lauer_anim | beide KI: Bild/+0x8f vor dem Vorschub je Phase (@0x80114ec8 ff.) |
| engine/src/spinnen_r38.c | gift_anker: Modell -> Welt mit re15_spinnenanim_r41_figur_matrix statt nur Gier | Teil-0-Weltmatrix *(+0x188)+0x40 kettet an Figur+0x20 (`lw s0,108(s2)` @0x8001ea0c, `jal 0x80022da0` @0x8001ea24) |
| engine/src/scd_room_setup.c | Installer-Zeile am Ende des Blocks: `re15_spinnenanim_r41_reset()` | Zeichen-Merker je Raum loeschen |

Regel des Zeichen-Merkers (spinnenanim_r41.c): jeder anim_set-Zwilling der Spinne merkt (Clip, +0x95, +0x8f, Phase +0x7,
Takt) VOR dem Vorschub; der Zeichner nimmt diese Werte, solange die Spinne im Schacht-Ablauf steht (Zustand 4, +0x5 = 1;
Phase 0 ruft kein anim_set -> die INIT-Pose bleibt stehen wie im Original @0x80114f14-1c) und im Landetakt (Phase 8,
nur derselbe Takt). Alle anderen Zustaende zeichnen unveraendert (Port-Standard).

Konstanten: 0xccc (`ori v0,zero,0xccc` @0x80110df8), Clip-/Bild-Logik unveraendert aus Runde 38 (Phasen @0x80114ec8).

**Massstab in beiden KI-Modi (Entscheid mit Beleg, kein Schalter).** Das Original zeichnet JEDE Adultspinne mit 0xccc:
der INIT @0x80110de0-dfc liegt hinter der grid-Verzweigung (@0x80110c20-d14, anim_set @0x80110d60) und laeuft fuer jede
Spinne; im Spinnen-Code 0x801109e4-0x801158a0 gibt es keinen zweiten Schreiber von +0x166 (Zensus 2.2), gemessen 0.8 in
jedem Takt 2..150. Im Port fehlte der Massstab in BEIDEN Modi -> nach Regel "im Original so, im Port nicht -> Port falsch"
nachgezogen. Die RE2-KI-Option tauscht das GEHIRN (Zustandsmaschine EMS25), nicht die Figur: EMS25/EMS26 schreiben weder das
RE2-Massstabsfeld +0x176 noch Flag 0x800 (2.2) — RE2 enthaelt also keine Anweisung, die den RE1.5-Massstab dieses
RE1.5-Gegnertyps ersetzt; und im RE2-Modus laeuft fuer die Schachtspinnen ohnehin der RE1.5-INIT-Zwilling (Runde 38,
re15_spinnen_r38_lauer_re2), dessen Original-Vorbild den Massstab setzt. Ein Modus-abhaengiger Massstab haette in
ROOM2060/20A0 Spinnen verschiedener Groesse nebeneinander erzeugt (Schacht 0.8, Boden 1.0, z.B. nach Wiedereintritt mit
gesetztem Flag 4/0xFB). Sichtbare Folge fuer den Nutzer: alle Adultspinnen sind 20 % kleiner als bisher im Port (= Original).


## 4. Messung nachher

### 4.1 Echte exe, derselbe Lauf wie vorher (`n2_2060`, Stand 24a27472)
`RE15_DEBUG_JUMP=2060@100 RE15_POCC_TP=150,6000,3000,1024 RE15_FRAMEDUMP=150-300/1 RE15_SPINNENANIM_LOG` (Messschiene = die
Matrix, die main.c WIRKLICH zeichnet, und das gezeichnete Bild): F185 (Takt 92) Clip 15 Bild 30, M [0,0,3276,0,3276,0,-3276,0,0];
F186 (Takt 93) Bild 31, M [0,0,3276,3276,0,0,0,3276,0] (= Original +0x20 Takt 93); F187 Clip 2 Bild 0 Ueberblendung 7;
F226 (Takt 133) Clip 14 Bild 31 mit Boden-Matrix; F227 Landetakt Clip 0 Bild 0 — Zeile fuer Zeile wie die Original-Tabelle.
Bilder: `L_belege/vgl_nachher_takt62-100.jpg`, `L_belege/vgl_nachher_takt104-136.jpg` (links Original-Video, rechts Port,
gleiche Takte wie 1.4): Groesse, Kippen ueber die Kante (Takt 80-92), Kopf-ueber-Lage an der Wand hinter Leon (Takt 93-132),
Landung (Takt 133/134) stimmen ueberein; kein Ein-Bild-Sprung mehr in Takt 92/93 und 133. `L_belege/vgl_nachher_schatten.jpg`:
der grosse Spinnenschatten liegt ab Takt 93 auf dem Boden an der Wand wie im Original (vorher fehlte er dort).
Restunterschied: die Abstiegshoehe je Takt folgt der Zufallszahl (+100 + (rng & 0xf) @0x801150dc-e8 / @0x80115198-bc), der
Port zieht andere Zahlen als die Messung (Endhoehe Port -462, Original -428, Takt 110: -793 / -737) — kein Mechanik-Unterschied.

### 4.2 Echter Weg (Tuer statt Sprung, `t1_2030_2060` und integration_r41_spinnenanim)
ROOM2030 oberer Steg -> Tuer slot 3 (Door_aot_set @RDT 0x01B4C: Stage 2 Raum 6, (7600,0,3950), Blick 2048) -> ROOM2060, Leon
geht 1,2 s geradeaus: Cut 1 in Raumbild 68, sub02 setzt grid 0x42, Spinne kriecht heraus, klettert kopfueber an der Wand
herunter, landet, RE2-Gehirn uebernimmt. Leon RE2-KI, Leon RE1.5-KI und Elza (ROOM2031 -> ROOM2061): Figur-Matrix 72/72 und
gezeichnetes Bild + Ueberblendung 72/72 Takte == Original.

### 4.3 Spucken (RE1.5-KI) — Nebenwirkung des Massstabs geprueft
Erster Giftklumpen Port (5741,-320,4049) = Original Bild 136 (5741,-320,4049) (R38 `orig_2060_schacht_gift.txt`): der
Gift-Anker nimmt jetzt die Figur-Matrix (Massstab 0.8) und die Pose des vorigen Zeichnens (vorher waere er mit Massstab
(5741,-298,4049) gewesen, ohne Massstab hoeher und 100 weiter vorn).


## 5. Tests (Namen, Ergebnis)

Neue Riegel (registriert nur in `re15_port/tests/unit/probes/r41_spinnenanim.cmake`):
- `unit_r41_spinnenanim` (tests/unit/test_r41_spinnenanim.c + test_r41_spinnenanim_orig.inc, erzeugt von
  `L_belege/orig_tabelle.py` aus `L_belege/orig_2060_gdb.log`): ROOM2060 im echten Spielschritt, RE1.5- UND RE2-KI, Original-
  Ablauf nachgestellt (Teleport Bild 60, Cut 1), je Takt 61..134:
  A0 Halten (Bild 0 der INIT-Pose), A1 Figur-Matrix == Original +0x20 (73/73, Wand 40/40), A2 gezeichnetes Bild/Ueberblendung
  == Original (73/73), A3 Wurzelteil wie gezeichnet == Original Teil 0 (Weltmatrix max 6 <= 7, Weltlage relativ max 1 <= 2;
  Toleranz = eine Winkeleinheit aus gpf12/gpl12 @0x800664e0, Herleitung im Testkopf), A4 Gegenprobe alter Zeichenweg verfehlt
  das Original (Wand-Matrix 40/40 falsch, Takt 93 Bild falsch, Ueberblendung in 21 Takten falsch), A5 Schatten 1300 x 1400 auf
  +0x1ba (73/73, an der Wand 40/40 auf dem Boden; vorher 40/40 in der Luft), B Gift-Anker == Original (Abweichung 0 <= 2).
  C ROOM20A0 (zweite Schachtspinne, floor 3, Gier -1024; RE1.5- und RE2-KI) gegen die Runde-38-Messung
  `analysis/befunde_runde38/R_belege/orig_20a0_schacht.txt` (Tabelle test_r41_spinnenanim_20a0.inc, erzeugt von
  `L_belege/orig_tabelle_20a0.py`): Logik 90/90, gezeichnetes Bild 90/90, Figur-Matrix = RotMatrix(rot Original) x 0xccc
  90/90 (Wand 57/57), Schatten-Hoehe 90/90.
  Ergebnis: OK (0 FAIL).
- `integration_r41_spinnenanim` (tests/integration/test_r41_spinnenanim.cmake): echte exe, echter Weg durch die Tuer (4.2),
  Laeufe L2/L15/E2, Auswerter `test_r41_spinnenanim pruef_log`: je 72/72 Takte == Original. Passed (120 s).

### 5.1 Gezielte Laeufe (keine volle Suite)
Testgruppen der beruehrten Module: Spinnen (0x25/0x26/RE2-Spinne/Babys/Spiderman-Griffe/Trefferkaesten/Gift/Zustand),
RotMatrix-Zwilling, Gegner-Schatten (main.c-Schattenblock), Kopflauf (scd_room_setup), Runde-38-Spinnen-Echtlauf, die eigenen.
Erster Lauf (Stand bed1d57c, vor Teil C):
`ctest --test-dir re15_port/build -R "unit_damage|unit_spider_ai|unit_adult_spider_ai|unit_rotmatrix|unit_re2_spider_ai|unit_re2_spider_water_2090|unit_re2_hit_repeat_crow_spider|unit_re2_baby_spider_dmg|probe_spinne_elevation|r31_trefferboxen|unit_r38_spiderman|unit_r38_griff_spiderman|unit_r38_spinnen|integration_r38_spinnen_echtlauf|r39_gegner_schatten|r39_zustand|r41_spinnenanim|integration_headless_run" --timeout 600 -j 4`
-> `100% tests passed, 0 tests failed out of 24`
Endlauf: siehe Abschluss (Abschnitt 8).
Nebenbefund aus dem Runde-38-Echtlauf (ROOM2030/2031, RE2-KI, 9225 Figur-Bilder je Lauf): keine Spinne hat dort rot_x/rot_z != 0
-> die volle Figur-Matrix aendert ausserhalb des Schacht-Ablaufs nichts an der Lage der Figuren.


## 6. OFFEN

1. **Ueberblendung port-weit um bis zu 1 Winkeleinheit zu genau** (alle Figuren, nicht nur die Spinne): das Original mischt
   in LoadAverageShort12 @0x800664e0 mit gte gpf12 (`3d 00 98 4b` @0x80066508, Zwischenergebnis abgeschnitten) und danach
   gpl12; skeleton_common.c re15_blend_angle12 rechnet (prev*wp + neu*wc) >> 12 in einem Schritt. Wirkung <= 0,09 Grad in den
   7 Ueberblendtakten nach einem Clipwechsel (A3 misst max 6/4096). Gemeinsame Datei, nicht Teil dieser Spur.
2. **Gezeichnetes Bild = Bild vor dem Vorschub gilt nur im Schacht-Ablauf** (Zustand 4, +0x5 = 1, INIT-Halten, Landetakt).
   Alle anderen Zustaende der Spinne und alle anderen Gegnertypen zeichnen weiter das Bild NACH dem Vorschub (Port-Standard,
   anim_set-Zwillinge zaehlen vor dem Zeichnen). Beispiel aus der Messung: Takt 135 (Spucken-Beginn setzt Clip 12 ohne
   anim_set) zeigt das Original noch Clip 0 Bild 0, der Port Clip 12 Bild 0 (ein Takt). Ein port-weiter Umbau braucht eine
   eigene Spur (jeder anim_set-Zwilling, Riegel je Typ).
3. **rot_x/rot_z anderer Typen**: der Zeichner nimmt die volle RotMatrix(+0x68) jetzt fuer 0x25 (und wie bisher 0x37);
   alle anderen Typen weiter nur die Gier. Kein Zensus, welche Port-Typen rot_x/rot_z schreiben (z.B. RE2-Minispinne 0x26
   `rot_z += 64` beim Versinken @0x80100D78-84). Das Original dreht jede Figur (FUN_8001e8c8 @0x8001e8f4).
4. Gift-Anker: rechnet die Pose des vorigen anim_set ohne dessen Ueberblendung nach (im gemessenen Spucken +0x8f = 0 ->
   exakt; mit +0x8f > 0 waere die Abweichung Bruchteile einer Einheit pro Knochen).
5. Massstab/Schatten im RE2-Modus fuer Spinnen, die NIE im Schacht waren (Boden-Spinnen 2030/2050/2070/2090): jetzt RE1.5-
   Werte (0.8, 1300 x 1400) — Begruendung in 3. Die RE2-Spinne (EMS25) setzt weder +0x176 noch Flag 0x800; einen RE2-eigenen
   Schattenwert fuer die Spinne habe ich in EMS25.BIN nicht gefunden (Scan der jal-Aufrufe mit a2/a3-Konstanten: keine
   Schatten-Groesse) — falls RE2 ihn generisch setzt (Spawn-Tabelle), waere das der naechste Weg.


## 7. Fuer den Nutzer

* **Neue Assets / Sprachdateien:** keine (nichts unter shared_assets/, kein gen/*.inc). Paket-Gate unberuehrt.
* **Was sich im Spiel aendert (STAGE2, Leon und Elza, beide KI-Einstellungen):**
  * Die Schachtspinne in ROOM2060/2061 (gemessen) und ROOM20A0/20A1 (gleicher Ablauf, Riegel C gegen die Runde-38-Messung) kippt am Ende des Herauskriechens kopfueber ueber die Gitterkante und
    klettert KOPFUEBER, mit dem Bauch an der Wand, an der Wand hinunter — vorher lag sie dabei waagrecht in der Luft und
    "schwebte" vor der Wand herunter. Unten dreht sie sich wieder auf den Boden.
  * Kein kurzes Zurueckspringen in die Ausgangspose mehr beim Uebergang vom Herauskriechen auf die Wand und beim Absprung von der
    Wand; jedes Bild der Animation erscheint im selben Takt wie im Original.
  * Alle grossen Spinnen sind 20 % kleiner als bisher im Port — so gross wie im Original (das Original zeichnet sie mit dem
    Massstab 0.8). Ihr Schatten ist groesser (1300 x 1400 wie im Original) und liegt beim Herunterklettern auf dem Boden unter
    ihr.
  * Der Giftklumpen startet genau dort, wo er im Original startet.
* **Bedienhinweis:** ROOM2060 ueber die obere Tuer aus ROOM2030 betreten und auf die Kamera mit dem Gitter (vordere Raumhaelfte)
  zugehen; die Spinne kommt heraus, sobald diese Kamera zu sehen ist.
* **Entscheidungsfragen:** keine. Hinweis zu deiner Einordnung: Groesse und Schatten gelten auch, wenn in den Optionen die RE2-KI
  gewaehlt ist — die KI-Option tauscht das Verhalten, nicht die Figur (Begruendung Dossier 3).

