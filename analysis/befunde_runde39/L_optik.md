# Runde 39 — Spur L "optik" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r39_optik` (Zweig `r39/optik`, Basis 3e49ff87). Stand: angelegt 2026-10-18.

## Punkte (Wortlaut des Nutzers, AUFTRAG.md)
1. "Ich habe noch mal einen Marker mit F9 gemacht. Leon ist immer noch vom Boden überdeckt an den einen Streifen."
2. "Der Schlüssel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran hängt, nicht so riesig."

## Punkt 1 — F9-Marke ROOM1100 Cut 1 (Bodenstreifen deckt Leon)

### Messung vorher
Messordner (ausserhalb des Baums): Scratchpad `L39/` (Laeufer `run.sh`, Skripte `*.py`, Laeufe `runs/<name>/`).
Hintergruende: `probe_bg_dump re15_port/shared_assets/PSX <out> 1` (Engine-Dekoder, 428 STAGE1-Bilder).

**Nutzer-Marke** `nutzer/befund_1100_F741_marke1.bmp` + `befund_marken_2026-10-17.txt` (F741, 2026-10-17 21:49, v0.8.26):
R1100 **Cut 1**, Leon (-16007,0,-23658) rot 3023, vz Fuss/Huefte/Kopf 10829/10699/10570, Kasten x158..180 y108..146,
52 Masken, am Koerper DECKT: 42 (178,59) 7x109 t=124 ... 51 (166,120) 2x28 t=160. Alle Gegner Leichen (st 7), Zombie 2
liegt bei (-15637,-23619). Im Bild ist Leons rechte Koerperhaelfte ab Spalte 166 (320er) abgeschnitten; zwischen dieser
Schnittkante und der hellen Eckleiste der Flurwand (Spalte 173..174) liegt ein 5..6 Spalten breiter Streifen, der
**Boden und Rueckwand des hinteren Gangs** zeigt — das ist "vom Boden ueberdeckt an den einen Streifen".

**Nachgestellt am gebauten Stand (Basis 3e49ff87)**, Messkopie `re15_pc_r39l_mess.exe`, `RE15_DEBUG_JUMP=1100@250
RE15_PLAYER_POS=-16007,-23658,3023 RE15_FORCE_CUT=1 RE15_FRAMEDUMP=150 RE15_BEFUND_MARKE=140`, Zombie 1 per
`RE15_GEGNER_POS=1,-14000,-11000,1024@5#1100` hinter die Kamera (Lauf `v_mit`, Gegenlauf `v_ohne` mit RE15_NO_PRI=1):
befund.log-Zeile **bitgleich zur Nutzer-Marke** (`pos=(-16007,0,-23658) rot=3023 vz=10829/10699/10570 scr=(169,143)
kasten=x158..180,y108..146 masken=52 | [42..51 DECKT]`). Bild mit gegen ohne Masken: 229 Punkte > 24, Spalten 166..179
(166:11 167:26 168:26 169:21 170:15 171:13 172:22 173:46 174:27 175:14 ...). Leon ist ab Spalte 166 weg.

**Was im Hintergrund an der Stelle steht** (BG ROOM1100 Cut 1 = `STAGE1/ROOM110.BSS` Scheibe 1, Grauwerte L):
- Die rechte Flurwand (Westwand-Ostflaeche) endet an einer senkrechten **Eckkante**: Zeilen 40..110 helle Eckleiste
  (Spalte 173: 150..207, Spalte 174: 133..180, Spalte 175: 37..72 = Wand), links davon die Rueckwand des hinteren Gangs
  (Spalten 166..171: 78..118). Halbwert-Kante je Zeile (Mitte Rueckwand gegen Spalte 173): **x = 172,84** (71 Zeilen,
  172,24..173,08; Pixelmitte bei x+0,5) -> Wand ab Spalte **173**.
- Zeilen 111..152 (Sockel): dunkle Kantenlinie **Spalte 171** (Werte 5..9 in Zeilen 112..148; 170: 13..22 Rueckwand-
  Sockel bzw. ab Zeile 130 Boden-Schatten 5..15; 169: Boden 32..51) -> Wand ab Spalte **171**. Die Stufe oben/unten
  (1,8 Spalten) ist die vorstehende Sockelleiste an der Ecke (Sockel-Oberkante der nahen Wand Zeile ~112, der fernen
  Rueckwand Zeile ~101 -> zwei verschiedene Tiefen).
- Zeile 150..152: helle Schwellenlinie des hinteren Gangs (Spalten 150..170), Eckfuss bei (171..172, 150..151).
- Links der Eckkante ist ALLES ferner Gang: Rueckwand (Sockel-Oberkante Zeile 101..102, dieselbe Zeile in allen Spalten
  160..170 = frontale Wand), Boden (Zeilen 130..148, Werte 30..51), Schwelle. Rueckprojektion (Kamera Cut 1, s.u.):
  Schwellenlinie Spalten 100..170 -> Weltboden z -22490..-22610; Rueckwand-Fuss Spalten 100..160 -> z -27040..-27200.

**Was die Masken dort tun**: Die 52 Rechtecke von Cut 1 sind die Westwand als Quader `[-17600,-23200,1910,11100,-4000]`
(auswahl.json, Phase-2-Bau Runde 37). Das Quaderende z=-23200 ist die **SCA-Zelle 2** (ROOM1100.RDT Kollision, x -17600
w 2000, z -23200 d 11100), nicht die gemalte Ecke. Projektion der Quaderecke (-15690, y, -23200) in Cut 1: x **165,9**
(Boden) .. 166,3 (y -4000). Die gemalte Ecke liegt bei 171..173 — der Quader reicht 5..7 Spalten weiter in den
**hinteren Gang** hinein. Die Atlas-Punkte dieser Spalten (Rechtecke 45..51 ganz oder teilweise, MSK Cut-1-Sektion @0x40,
Saetze @0x400..0x453, z.B. 51 `a4 97 a4 97 a0 00 00 00 02 00 1c 00` = Tiefe 160, 2x28) sind Bodenpunkte und Rueckwand des
hinteren Gangs mit der Tiefe der Wand (t 136..160 = Kamera-z 8776..10314). Alles, was dort steht und ferner ist (Leon im
Quergang: vz 10570..10829), wird mit **Boden** uebermalt.

**Seit wann (Bisect ueber die Masken-Dateien, Skript `bisect_streifen.py`, Grenze wie oben = Spalte < 173 / < 171)**:

| Stand | Cut-1-Masken | Maskenpunkte links der gemalten Ecke | kleinste Spalte |
|---|---|---|---|
| 557eac11 (2026-09-08) | keine Datei | - | - |
| 7d236216 (2026-09-09, C1-Quader eingefuehrt, Marke F205) | 105 | 631 | 165 |
| 5118c96e (Runde 36 Spur O, Ostflaeche -15690) | 105 | 475 | 166 |
| f7829ae9 (Runde 37 Spur A, bau_p2) | 52 | 474 | 166 |
| e8b3958f v0.8.24 | 105 | 475 | 166 |
| 2077bb8e v0.8.25 | 52 | 474 | 166 |
| v0.8.26 / Basis 3e49ff87 | 52 | 474 | 166 |

-> **keine Regression**: der Streifen steckt seit der Einfuehrung des Quaders (7d236216) in den Daten. Runde 36 Spur O hat
die OSTFLAECHE auf die gemalte Wandbasis gesetzt (x -15600 -> -15690, "Bodenstreifen vor der Wand"), Runde 37 Spur A die
TIEFE je Rechteck (Kachel-Maximum -> eine Tiefenstufe); das ENDE des Quaders (z -23200 aus der Zelle) hat keiner gegen den
Hintergrund gemessen. Die Runde-36-Meldung "Leon mit einer Halbtransparenz ueberdeckt mit Boden" (Marke nie gefunden) passt
genau auf diesen Streifen.

**Echter Weg** (ohne FORCE_CUT; RVD ROOM1100 @0x208, Satz 3 `1->0` Viereck (-18900,-27800)(-19399,-22100)(-16300,-22400)
(-14899,-28600)): von Cut 1 aus bleibt die Kamera bis zur Kante x = -16016 (bei z -23658) auf Cut 1 — die Nutzerlage
(-16007) liegt 9 Einheiten innerhalb. Lauf `weg2_vorher` (Start (-14700,-21800) rot 1024, `U0.9,R0.5,U0.6`): Leon im
Suedende des N-S-Flurs an der Ecke (-15132,-23033), befund.log `[48:(167,63) t=148 DECKT] [49:(167,71) t=152 DECKT]`
— auch VOR der Ecke greifen die Streifen-Rechtecke in seinen Kasten.

**Zweite Marke** (`befund_1090_F1357_marke1.bmp`, 23:31, R1090 C0, hp=1, "keine Maske am Koerper"): das Bild ist die
**Karte 2F** (Positionsmarke oben rechts ausserhalb des aktuellen Raums) — das ist der AUFTRAG-Punkt "In ROOM 1090 ist
Leons Positionsmarker der Map ausserhalb des aktuellen Raums" (VERTRAG: Spur G). Gehoert nicht zu diesem Befund, hier nicht
behandelt (Hinweis an Spur G/Orchestrator unter "Fuer den Nutzer").

### RE-Belege (Fortsetzung 2026-10-18, alles selbst nachgelesen: `re15_disasm.py dis`, `xxd`, Skripte in `L_optik_bilder/`)

**1. Mechanismus der Maske (PSX.EXE, FUN_80039590, Maskenschleife):**
```
80039628  ori a1,zero,0x1
8003962c  ori a2,zero,0x1
80039630  ori a3,zero,0x95        ; SetDrawMode(.., tpage 0x95 = Vordergrundseite des Hintergrunds)
80039634  jal 0x80069858
80039640  jal 0x8006b958          ; SPRT-Primitiv des Rechtecks (srcX/srcY -> dstX/dstY, w x h)
80039650  lh  a0,2(s3)            ; Tiefe (depth) des Satzes, unveraendert aus der RDT
80039658  sll a0,a0,2             ; x4 Byte = OT-Wort depth
8003965c  addu a0,a0,s6
80039660  jal 0x8006b538          ; AddPrim OT[depth]
```
Figur: `80025654 sra v0,v1,6` / `80025658 beq v0,zero,..` (Nah-Tor), `8002565c sra v1,v1,4` (OT-Wort = otz>>4);
ZSF3 `80066c70 addiu t0,zero,341` (otz = (1023*vz)>>12). Eine Maske ist also ein SPRITE, der die HINTERGRUNDPUNKTE ihres
Rechtecks ueber jede Figur in einem ferneren OT-Eimer legt - ohne Ruecksicht darauf, was diese Punkte zeigen. Ein
Rechteck, das ueber die gemalte Kante des Verdeckers hinausreicht, malt deshalb den BODEN/die Rueckwand dahinter ueber die
Figur (Memory reai-v2-sprite-pri-occlusion; Original-Urteil im Port `re15_pri_mask_occludes`).

**2. Original-Daten ROOM1100.RDT (bytegleich ROOM1101.RDT, `cmp`):** Cut 1 Satz @0x80 `00 00 3c 68 be cc ff ff ...`
(fov 26684, Ort (-13122,-3078,-13608), Ziel (-15912,-954,-25398)), Masken-Offset @0x9C = `0c 06 00 00` -> @0x60C
`ff ff ff ff`; Cut 2 Satz @0xA0, Offset @0xBC = `10 06 00 00` -> @0x610 `ff ff ff ff`. Das Original hat in diesen Cuts
KEINE Masken (NULL-Sektion; FUN_800392d4 @0x80039324 lw / @0x8003932c bne -1 / @0x80039338 sb zero). Die Port-Masken
der Westwand sind NUTZER-VORGABE (Marken F205/F595, Runde 36/37) - strittig ist nur ihre FORM (wo sie enden).

**3. Form-Beleg aus den Masken des Originals (ROOM1030 Cut 3, pri @0xBF8, 4 Gruppen / 40 Masken; `orig_maske_1030.py`):**
Maske 2 (Gruppe 0) dst (203,66) 24x64 Tiefe 127 = die Pfeilerflaeche; ihre letzte Spalte ist **226**. Die GEMALTE
Pfeilerkante (Halbwert-Kante je Zeile, Zeilen 66..129, Engine-Dekoder-Bild) liegt bei **227,18** (224,71..228,61, 64
Zeilen). Die Original-Maske endet also AN der gemalten Kante (Mittel 1,2 px innen, Streuung -1,3..+2,6 px = die
8er-Kachel des Originals), nicht an einer Kollisionszelle. RE2 Retail dasselbe (Memory reai-v2-re2-pri-vorbild: 66 %
frei geformte Rechtecke der Objektform). Die Port-Westwand ragte dagegen 5..7 (Cut 1) bzw. 4..5 (Cut 2) Spalten ueber die
gemalte Ecke hinaus - weil ihr Quaderende die SCA-Zelle 2 ist: ROOM1100.RDT @0x6A4 `d0 07 5c 2b 40 bb 60 a5` (w 2000,
d 11100, x -17600, z -23200). **Regel: eine Maske endet an der GEMALTEN Kante; ein Zellenmass ist kein Bildmass.**
Bild `L_optik_bilder/p1_original_1030_c3_maske2_pfeilerkante_x6.png`.

**4. Masken-Zwilling Elza:** ROOM1101 zeichnet die Masken von ROOM1100 (`re15_pri_masken_raum`, Runde 37 Spur A; RDT
bytegleich, s.o.) - dieselben Fenster gelten fuer Elza.

### Umsetzung (Commit 370793fe, hier gebaut und gemessen)
* `engine/src/optik_r39.c` + `include/re15_optik_r39.h` (NEU): `re15_optik_r39_kappen(masken_raum, cut, pri)` kappt die
  Rechtecke von ROOM1100 Cut 1/2 beim Laden an der gemalten Wand. Fenster `k_fenster[]` (am Engine-Dekoder-Bild gemessen,
  Halbwert-Kanten, Pixelmitte x+0,5): Cut 1 Zeilen 0..110 Wand ab Spalte 173 (Kante x = 172,84), Zeilen 111..239 ab 171
  (Sockel-Kantenlinie); Cut 2 Zeilen 0..106 Wand bis Spalte 133 (Kante 133,94), Zeilen 107..239 bis 134 (Kante 134,86).
  Rechtecke ueber beide Baender werden geteilt (Atlas-srcX/srcY ziehen mit), Rechtecke ganz jenseits entfallen, keine
  Tiefe aendert sich, kein Rechteck waechst. `re15_optik_r39_in_wand()` fuer die Riegel.
* Haken (je 2 Zeilen): `platform/pc/main.c` nach `re15_masken36_filter` (Runde 37 Spur P) und `platform/psx/main.c`
  ebenso. MASKS/ * und shared_assets unveraendert; keine neue Datei unter pri/STAGE1 -> `pri_stage1_hashes.inc`
  unveraendert (Memory reai-v2-pri-hashliste-mitcommitten greift nur bei neuen Nutzer-PRI).
* Log beim Laden (debug.log): `[optik_r39] ROOM1100 Cut 1: Westwand an der gemalten Ecke gekappt: 52 -> 52 Rechtecke,
  1395 Rechteckpunkte jenseits der Ecke entfernt (Runde 39 Spur L)` (1395 Rechteckpunkte, davon 474 im Atlas sichtbar).

### Messung nachher (exe des Baums, gebaut 03:05 mit optik_r39.c, Messkopie `re15_pc_r39l_mess.exe`; Laeufe `runs/n2_*`)
Dieselbe Szene wie "vorher" (`RE15_DEBUG_JUMP=1100@250 RE15_PLAYER_POS=-16007,-23658,3023 RE15_FORCE_CUT=1`, Zombie 1
hinter die Kamera, Bild 150): A = mit Masken, B = ohne Masken (RE15_NO_PRI=1), C = mit Masken, Leon hinter die Kamera
gesetzt (-14000,-11000). Punkte mit Kanaldifferenz > 24, Zeilen 40..170 (`streifen_diff.py`):

| Fenster | A gegen B (Maske greift) | A gegen C (Leon sichtbar) | B gegen C |
|---|---|---|---|
| Streifen x 166..172 (jenseits der gemalten Ecke) | **1** (vorher 134; der eine Punkt: Spalte 171 im Sockelband = Wand) | **140** | 141 |
| Wand x 173..180 | 95 (vorher 95) | **0** (ganz hinter der Wand) | 95 |
| links x 150..165 | 0 (vorher 0) | 179 | 179 |

-> Im Streifen malt die Maske nichts mehr ueber Leon (A == B bis auf den Sockelpunkt), Leon ist dort gezeichnet (A gegen
C 140 Punkte), und hinter der gemalten Wand bleibt er vollstaendig verdeckt (A gegen C 0). Je Spalte A gegen B: vorher
`166:11 167:26 168:26 169:21 170:15 171:13 172:22 173:46 174:27 ...`, nachher `171:1 173:46 174:27 175:14 176:3 177:2
178:2 179:1`. Bild `L_optik_bilder/p1_1100_c1_marke_alt_neu_mit_ohne_x4.png` (alt mit | alt ohne | neu mit | neu ohne).

befund.log an der Lage der Marke (F140, bitgleicher Kopf): `R1100 C1 pos=(-16007,0,-23658) rot=3023 vz=10829/10699/10570
scr=(169,143) kasten=x158..180,y108..146 masken=52 | [42:(178,59)7x109 t=124 z=8007 DECKT] [43:(176,60)6x104 t=128
DECKT] [44:(174,61)6x101 t=132 DECKT] [45:(173,61)5x50 t=136 DECKT] [46:(172,111)6x49 t=136 DECKT] [47:(173,62)3x49
t=140 DECKT] [48:(171,111)5x46 t=140 DECKT] [49:(173,63)1x48 t=144 DECKT] [50:(171,111)3x44 t=144 DECKT] [51:(171,111)
1x42 t=148 DECKT]` - alle Rechtecke am Kasten beginnen jetzt in der Wand (x >= 173 oberhalb Zeile 111, x >= 171 im
Sockelband); vorher begannen 45..51 bei 166..172.

## Punkt 2 — Schluessel an der Pinnwand ROOM10F0/10F1

### Messung vorher
(in Arbeit)

### RE-Belege

### Umsetzung

### Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer
