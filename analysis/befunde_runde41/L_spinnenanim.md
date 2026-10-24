# Runde 41 — Spur L "spinnenanim" (Dossier, fortlaufend)

Baum `.claude/worktrees/r41_spinnenanim`, Zweig `r41/spinnenanim`, Basis e5c914fe (= master 4c667c97 + AUFTRAG/VERTRAG).

## Nutzer-Punkt (Wortlaut, AUFTRAG.md Zeile 47)

> Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch nicht ganz.

## 0. Stand / Protokoll

- 2026-10-24 ~22:00: Start (kein Vorgaenger-Stand in diesem Zweig). Dossier angelegt (c5c244b7).
- 22:30 Bau des Spurbaums (local_build.sh configure + build, OK). Port-Messung ROOM2060 (1.1/1.2).
- 22:55 RE-Belege Zeichenweg (2.1-2.3) — Original-Messung mit Bildern folgt (1.3).

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

## 3. Umsetzung (Dateien, Konstanten)

(folgt)

## 4. Messung nachher

(folgt)

## 5. Tests (Namen, Ergebnis)

(folgt)

## 6. OFFEN

(folgt)

## 7. Fuer den Nutzer

(folgt)
