# BEFUND gator-finisher-sounds — Alligator-Finisher (Leon im Maul), Alligator-Biss-SE, Spinnen-Angriffs-SE

Nutzer 2026-09-19 (befund_2090_F309_marke1): "Der Aligator hat Leon immer noch nicht im Maul
im Finisher, sondern er liegt darunter […] hat der Aligator immer noch nicht den Angriffs/Biss
Sound. Und auch die Spinne hat noch nicht ihren Angriffssound."
Runde 16, NUR Messung + Disassembly, kein Engine-Eingriff. Sonden (kein add_test):
`re15_port/tests/unit/probe_r16_gator_finisher_anker.c`, `probe_r16_gator_se.c`,
`probe_r16_spider_se.c`, registriert in `tests/unit/probes/r16_gator-finisher-sounds.cmake`,
gebaut in `re15_port/build_r16_gator/`.

## 0. Kurzfassung

1. **Finisher:** Leons Wurzel sitzt gator-lokal bei (7892,−1804,295) statt (5264,−2403,198) wie im Original — 2628 zu weit VOR dem Maul und 599 zu TIEF (unter dem Unterkiefer), weil der Port den authored Anker-Versatz (RE2 FUN_80015B94/CB8) ungeskaliert auf einen mit +0x166 = 2731 (2/3) gezeichneten Gator legt und Leons y gar nicht setzt (Spiel: pl.y = 0, gator_boss.log 2026-09-14 = Sonde bitgleich).
2. Der Render-Scale skaliert im Original JEDEN modell-lokalen Versatz um die Entity-Position (ScaleMatrix VOR der Bone-Schleife, FUN_8001e8c8 @0x8001e904-40); der Opfer-Versatz ist derselbe Rig-Raum → **Leon = G + s·RotY·(off_v − off_g), y = G.y − (1−s)·POSy_v**. Damit trifft der Port die Original-Relation auf ±5 Einheiten und 6=1/7=1 in jedem Frame, in dem das Original sie hat — OHNE den Scale zu entfernen (gemessen, Sonde Teil A+B).
3. **Biss-SE:** Der Ruf kommt (gb_se(3) beim Lunge-Start UND beim Biss, Bank-Latch 17 korrekt), wird aber vom PC-Mapper auf RE1.5-snd1 SE 2 gebogen — ein 0,22-s-Impakt (2752 B, ZCR 294). RE2s ANGRIFFS-Laut ist SE 4 (2,40-s-Brüller) an Lunge-P0 @0x80100d84 + Frame-Flags Clip 3 f50/f77; den spielt der Port nirgends, weil sein Lunge Clip 4 (datenstumm) statt Clip 2→3 fährt.
4. **Spinne:** Ihr Angriffs-SE 1 (@0x80105b34-38) wird im Port gerufen (Sonde: je Biss genau 1 Ruf), spielt im Spiel aber aus **Bank 17** (Gator!), weil main.c den Gator in den LETZTEN Slot setzt und die Roster-Schleife die Hooks aufsteigend registriert — der EINE ENEMSE-Latch (audio_pc.c:1007) landet auf 17; Spinnen-ids 8/9 (Schritte) sind dort SILENT, id 1/0 sind Gator-Samples.
5. Port muss: (A) den Opfer-Versatz mit dem Entity-Scale mitführen (3 Zeilen im FRESSEN-Block), (B) SE 4 am Lunge-Start + Clip-3-Frame-Flags (RE2-Lunge Clip 2/3) spielen, (C) die ENEMSE-Wiedergabe je Gegner-Typ an SEINE Bank binden (Mehrbank-Cache statt Ein-Latch).

## 1. Reproduktion/Messung

### 1.1 Finisher (probe_r16_gator_finisher_anker)

Metrik: Leons GERENDERTE Wurzel (Entity + RotY(yaw)·POSE-Kanal, exakt `re15_skel_bone_to_world`)
zurück in den Gator-LOKALEN Rahmen: `local = RotY(−yaw)·(root − G) / s`. Im Original (kein Scale)
ist das exakt `(off_v − off_g) + POS_v` (Anker-Paar §2.1). 6=?/7=? = Punkt in der lokalen AABB
von MD1-Mesh 6 (Oberkiefer) / 7 (Unterkiefer) der geposten Kette. Yaw-invariant (geprüft: yaw 113
und yaw 0 liefern dieselbe lokale Lage ±2).

```
-- P3 Kau-Loop Clip11 f0 / Opfer f119  Original-LOKAL=(5264,-2403,198)
  ORIGINAL s=4096 (y-Leitung)   LOKAL=( 5262, -2403,  197) dRef=(  -2, 0, -1) | 6=1/7=1 min6=214 min7=393
  T heute  s=2731 (pl.y=0)      LOKAL=( 7892, -1804,  295) dRef=(2628, 599, 97) | 6=0/7=0 min6=1298 min7=1870
  Y heute+y-Leitung s=2731      LOKAL=( 7892, -3604,  295) dRef=(2628,-1201, 97) | 6=0/7=0 min6=229 min7=706
  S SKALIERT s=2731             LOKAL=( 5261, -2402,  194) dRef=(  -3, 1, -4) | 6=1/7=1 min6=211 min7=391
   Mesh6 (Oberkiefer)  LOKAL-AABB x[4304..7785] y[-3884..-1044] z[-951..951]
   Mesh7 (Unterkiefer) LOKAL-AABB x[4130..7381] y[-4527..-1776] z[-986..984]
```
P2 (Clip 5 / Opfer-Clip 1), Original vs. S-Variante, alle ±5: sf0 6=1/7=0 (beide), sf30 6=1/7=1
(beide), sf60 6=0/7=0 (beide — Schleuderspitze y −7784, Leon fliegt über dem Maul), sf75 6=1/7=0
(beide), sf90 6=1/7=1 (beide), sf119 6=1/7=1 (beide). Heute (T) überall 6=0/7=0, dRef x +2341..+3572,
y −2090..+1183.

**Teil B (echter Boss-Tick, ROOM2090, RE15_GB_TEST=1)** liefert exakt die Spielwerte des Nutzers:
`LIVE: anker=(-8368,-21962) yaw=113 pl=(-65,0,-23215) scale=2731` — gator_boss.log 2026-09-14:
`FRESS-P2 Anker=(-8368,-21962) yaw=113`, `FSYNC2 sf=120 … pl=(-65,0,-23215)`. LIVE P3:
LOKAL=(7892,−1804,295), 6=0/7=0. **Leon liegt 599 Einheiten (lokal) unter dem Original-Sitz und
2628 vor ihm — unterhalb der Unterkiefer-AABB (y ≥ −1776 heißt: unter dem Kiefer).** Das ist
das "liegt darunter" der Marke.

Datenzensus: Opfer-Clip 1 (kf 30..149) POSE x/z = 0 in ALLEN kf, SPEED y = 0 in allen Gator- und
Opfer-Frames (EM23.EMD EMR3 @0x20E54, kfo 8, kf 80 B, selbst byte-gelesen) → die y-Kompensation
betrifft nur den POSE-y-Kanal, x/z nur den SPEED-Versatz. Opfer-Clip 0 (Hochheben, kf 0..29):
POSE y −1779→+85, SPEED x 2→2096, x/z-POSE 0.

### 1.2 Gator-SE (probe_r16_gator_se)

ROOM2090 SCD (main00+sub00, Szenario 0): spawnt **2× 0x25 (Slots 1, 2)**, KEIN 0x23. Der Gator
ist Port-Ergänzung `main.c:6044-6079`: letzter freier Slot (rückwärts gesucht, hier 15); die
Roster-Schleife `main.c:6082-6088` lädt danach aufsteigend → Hook-Reihenfolge Spinne (Bank 11)
→ Gator (Bank 17) → **Latch = 17** (ein einziger `s_re2se_bank_sel`, audio_pc.c:1007).

Bank 17 (ENEMSE.VBS EDT @0x128000, VBD @0x129000, 7 VAGs): id0 chan4 prio2 vag4 9328 B vol127;
id1 chan2 prio3 vag3 7728 B vol90; id2 chan3 prio3 vag1 8224 B vol120; id3 chan2 prio3 vag2
11888 B vol90; id4 chan4 prio2 vag5 18032 B vol127; id5 chan4 prio2 vag6 7264 B vol127; **id6..31
SILENT (FFFFFFFF)**.
Bank 11 (EDT @0xC9800, VBD @0xCA800, 10 VAGs): id0..10 belegt (id1 = Biss: chan3 prio2 vag2
2016 B vol127; id8/9 Schritte vag1 1984 B vol50/60), id11+ SILENT.

SE-Protokoll des Boss-Laufs (Spieler nah, Lunge → Biss → Finisher):
```
SE F0     kampf    GATOR  id=3 | Latch=Bank 17 -> spielt   (gb_se(3) am LUNGE-Start :1345)
SE F11    kampf    GATOR  id=3 | Latch=Bank 17             (gb_biss_abschluss :389)
SE F28    FRESSEN  GATOR  id=1 | Frame-Flag Clip 5 f2
SE F106   FRESSEN  GATOR  id=3 | Frame-Flag Clip 5 f80
SE F343/429 kampf  GATOR  id=2 | Frame-Flag Clip 0 (Schwimmen)
```
Im Spiel biegt `pc_gator_se_re15` (main.c:734-743) die NICHT-Frame-Flag-id 3 auf
`re15_audio_room_se(2)` = RE1.5-snd1 SE 2 (Rec 00003214: Stimme 4, prio 2, VAG 3 = 2752 B,
22050 Hz, 0,22 s, ZCR 294, Runde-5-Tabelle). Bank-17-SE 4 (Angriffs-Brüller, 18032 B ≈ 2,4 s)
wird an KEINER Stelle des Kampfs gerufen: der Port-Lunge fährt Clip 4 (datenstumm — Zensus:
Frame-Flags nur c0/c1/c2/c3/c5/c6/c7/c8; **Clip 4 hat keines**), RE2 fährt Clip 2→3 mit SE 4
an P0 (§2.2). Die 55-F-Brüll-Sperre wird deshalb auch nie geladen (`if (id == 4)` :284).

### 1.3 Spinnen-SE (probe_r16_spider_se)

EMS25-Frame-Flags: einzig **c11/f5:SE3** (Oberflächen-Wechsel-Clip, RE2 spielt ihn über
FUN_80016028 nur im Tail von Sub 8 @0x80101074 / @0x80102ae8) — kein Angriffs-Frame-SE.
Angriff = **SE 1 in re2s_attack** (@0x80105b34-38, beim Treffer). Sonde: Szenario B
(sub_state_1 = 7 erzwungen) → je Sprung genau `SE id=1` im Biss-Frame (F54/F50/F51/F51, hp
200→180), Latch dort 11, weil die Sonde allein die Spinne registriert. **Im Spiel ist der Latch
17 (1.2):** id 1 → Bank-17-id 1 (= Gator-Schüttel-Laut, 7728 B, vol 90), id 0 → Gator-id 0
(Großreaktion), **id 8/9 (Schritte) → SILENT**. Die Spinne hat also nicht "keinen", sondern den
falschen Angriffs-Laut, und keine Schritte. Szenario A (natürlich, 1500 F, Spieler 900 davor):
sub 7 wird nie erreicht — nur Schritt-ids 0/8/9 (11 Rufe); der Angriff braucht die
Stalk-Bedingungen von Sub 5/6.

## 2. Original-Mechanismus

### 2.1 RE2 Anker-Paar (info/re2leon/PSX.EXE, selbst disassembliert)

FUN_80015B94 (Anker setzen, a0 = Ziel-Entity, aktuelle Entity 0x800CE330 = Gator):
```
80015bd0  jal 0x80015db0            ; Versatz (off) des AKTUELLEN Frames -> sp+48 (kf+6/+8/+10)
80015bd8-1c  lw/sw 0x8009db44 -> sp+16   ; IDENTITAETS-Matrix (kein Entity-Scale!)
80015c24  lh a0,118(s0)             ; Gator-Yaw +0x76
80015c28  jal 0x8008e8b4            ; RotMatrixY(yaw, m)
80015c38  jal 0x8008dba4            ; ApplyMatrix(m, off) -> gedrehter Versatz
80015c40-50  lw v0,56(s0) / lhu v1,48(sp) / subu / sh v0,356(s0)  ; anchor_x = x - off.x  (+0x164)
80015c54-64  lw v0,60(s0) / lhu v1,50(sp) / subu / sh v0,358(s0)  ; anchor_y = y - off.y  (+0x166)
80015c68-78  ...                     sh v0,360(s0)                  ; anchor_z = z - off.z  (+0x168)
80015c7c/88/94  sh a0,356(s3) / sh v0,358(s3) / sh v0,360(s3)      ; Kopie in den Spieler
```
FUN_80015CB8 (Platzierung, a0 = Entity): `80015d58-68 lh v0,356(s1) + lh v1,48(sp) -> sw v0,56(s1)`
(x = anchor_x + off.x), `80015d6c-7c … sw v0,60(s1)` (y), `80015d80-90 … sw v0,64(s1)` (z).
**In RE2 gibt es keinen Entity-Render-Scale; +0x166 ist dort die y-Komponente des Ankers.**
Der Versatz ist ein Rig-Raum-Vektor (kf-Bytes 6..11), gedreht um den Yaw, um die Entity-Position.

### 2.2 RE1.5 Entity-Render-Scale (info/Re1.5/PSX.EXE, FUN_8001e8c8)

```
8001e904  andi v0,v0,0x800          ; Entity-Flag 0x800
8001e91c/28/38  lh v0,358(v1)       ; +0x166 -> VECTOR (x,y,z) = Scale
8001e940  jal 0x80065ff0            ; ScaleMatrix(entity+0x20, scale)   VOR
8001e94c  jal 0x80053fc0            ; Translation (entity+0x34 = Weltposition)
8001e9ec/8001ef54 Schleife je Part (Stride 0xAC) ; Bone-Kette mit der skalierten Matrix
```
(Decompile RE_15_Quellcode_V2/FUN_8001e8c8.c: `RotMatrix(+0x68, +0x20); if (flags & 0x800)
ScaleMatrix(+0x20, {+0x166,+0x166,+0x166}); FUN_80053fc0(+0x34); Bone-Loop`.) Die Weltposition
bleibt, jede Bone-Translation (incl. Wurzel-POSE kf+0..5) wird mit der 3x3 skaliert — genau so
zeichnet es main.c:8256-8261 (`nyaw[k] *= render_scale_q12`). **Folgerung (exakt, keine
Schätzung):** ein Vektor, der im Rig-Raum des Gators liegt, erscheint in der Welt als s·RotY·v um
G. Der Opfer-Versatz (off_v − off_g) + POS_v ist so ein Vektor (§2.1: derselbe Anker, derselbe
Yaw, dieselben Einheiten wie die Gator-Bones) → er muss mit s skaliert werden. Leons eigenes
Rig bleibt 1×; nur seine WURZEL wird an den skalierten Kiefer gehängt.

Der Scale 2731 selbst hat keinen @0x-Beleg (Nutzer-Entscheidung 2026-09-10, Memory runde15) —
mit ihm ist Leon 1,5× so groß wie das Maul; der Sitz der Wurzel ist damit richtig, die
Körperproportion bleibt eine Folge der Entscheidung (Rumpf/Beine ragen aus einem 2/3-Maul).

### 2.3 RE2 EM23 SE-Rufe (EM23_OVL_0000.BIN, jal-0x8005bd6c-Zensus, 4 Stellen)

- **@0x80100d84 a0=4** (Delay-Slot `addiu a0,zero,4` @0x80100d64): Sub 3 LUNGE, Phase 0 — der
  ANGRIFFS-Brüller beim Lunge-Start (danach Clip 2 frac 16 @0x80100dac, Clip 3 @0x80100e20).
- @0x801020c4 a0=5: HURT-Standard (Gate +0x232/+0x233 @0x801020ac-c8).
- @0x801021ec a0=4 (@0x801021c4): R1 Kanister-Explosion.
- @0x80102c10 a0=0 (`addu a0,zero,zero` @0x80102c0c): R4 Großreaktion, Gate +0x232==0.
- Frame-Flags über FUN_80016028 @0x80100438 (`lui v0,0x800 / and / beq / jal 0x8005bd6c` mit
  `srl a0,a0,28` @0x80016040-54): Clip 3 f50/f77 SE 4 (BISS-Clip), Clip 5 f2 SE 1 / f80 SE 3,
  Clip 0 f76/f162 SE 2, Clip 6/7/8 SE 4/2. **Clip 4 (Fress-Schnapp) ist stumm.**
- Bank 17 = Paar-Tabelle @0x800A7400 Zeile 17 {0x16,0x00} (Datei 0x97C22); Sound-Id 0x16 aus
  ROOM40A0.RDT em_set @0x025A0 `44 00 00 23 00 00 01 16` (+7). Zeile 11 {0x10,0x00} = Spinne
  (Sound-Id 0x10, ROOM3020 @0x0AD8 …). Es gibt KEINE Zeile {0x16,0x10}: RE2 hätte für einen
  Raum mit Gator + Spinne gar keine Bank (FUN_80052b38 → 0xFF) — der Konflikt ist port-eigen.

### 2.4 RE2 Spinne (EMS25.BIN)

jal-0x8005bd6c-Zensus (9): @0x80101134 a0=3, @0x8010189c a0=3 (Oberflächen-Anker), @0x80103304
a0=6 / @0x801036e8 a0=6 / @0x801043d0 a0=6 / @0x8010400c 5|6 (Landungen), **@0x80105b38 a0=1
und @0x80105bc0 a0=1** (`addiu a0,zero,1` @0x80105b34/bbc, jeweils nach `jal 0x801059f0`
= Trefferprüfung, im Erfolgszweig) = der BISS, @0x80106050 Schritt-Tabelle {0,8,9}.
FUN_80016028-Rufe nur @0x80101074 (Sub-8-Tail) und @0x80102ae8.

## 3. Port-Ist

**Finisher — enemy_ai_boss_gator.c, `case GBP_FRESSEN`:**
- :1924-1925 P0-Teleport `pl = e + RotY·(10643,−915)` unskaliert (@0x8010106C-84 korrekt zitiert,
  aber im 2/3-Raum 3547/305 zu weit).
- :1939-1940 Anker `re15_clip_anchor_set_pub(e, …, 4, 13); pl->anchor = e->anchor` — xz-only,
  y nicht (Kommentar :1941-1951 begründet das mit der 6=0/7=0-Messung der reinen y-Leitung; die
  Messung war richtig, der Schluss "beides gehört in EINEN Schritt" übersah, dass x/z ebenso
  skaliert werden müssen: Variante Y oben ist tatsächlich schlechter als T in y, weil ohne
  x-Skalierung der Punkt am Kiefer vorbei liegt).
- :1981-1986 P2 `re15_clip_root_motion_abs_pub(e, …, 5, sf)` und `(pl, …victim…, 1, sf)`:
  beide aus dem GEMEINSAMEN Anker unskaliert → Leon-Wurzel lokal 1/s·(off_v−off_g) = +2628 vor
  dem Sitz; `pl->y` wird nie gesetzt → im Spiel 0, Wurzel −2403 statt −1200−2/3·2403 = −2802.
- :2017-2022 P3 dito mit Clip 11 / Opfer f119.
Warum falsch: main.c:8256-8261 skaliert den Gator um G (byte-true zu §2.2), der Anker-Zwilling
(enemy_ai_common.c:620-676, byte-true zu §2.1 im UNSKALIERTEN Original) kennt keinen Scale.

**Biss-SE:**
- :389/:1345/:1409/:1751 `gb_se(3)` an Lunge-Start und Biss → main.c:742 Mapper → RE1.5-snd1
  SE 2 (0,22 s, dumpf). Runde 4 hatte SE 4 hier ENTFERNT ("Peak 1,6 s nach dem Biss"), weil der
  Port-Lunge nur den 45-F-Clip 4 fährt; RE2s Lunge (Clip 2 → Clip 3, Maul offen ab ~f54, 150 F)
  trägt SE 4 an P0 UND als Frame-Flag f50/f77 — dort passt der 1,43-s-Attack zum Maulkontakt.
- Frame-Flag-Spieler :904-918 korrekt (Clip 5 f2/f80, Clip 0, Clip 7).
- Bank-Wahl: Latch 17 nach dem Laden (1.2) — für den Gator RICHTIG.

**Spinnen-SE:** enemy_ai_re2_spider.c:339 `re2s_se(1)` (@0x80105B34-38) feuert (1.3), Schritte
:285 ids 0/8/9, Landung :1828/:2356 ids 5/6, Oberfläche :690/:1061 id 3. Alles über
`re15_audio_re2_enemy_se` (audio_pc.c:1101) mit dem gemeinsamen Latch 17 (main.c:6044-6088
Reihenfolge) → falsche Bank: id 1/0/2/3/4/5 = Gator-Samples, 6..10 SILENT.
Nebenbefund: `re2s_step_se` kennt für `+0x10C != 0` (Wasser) keinen Port-Produzenten → immer
Tabelle {0,8,9}; und der ausgelieferte RE1.5-ROOM2090 stellt beide 0x25 ins WASSER (pos y 0,
Wasserzonen) — RE2s Spinnen kennen kein Wasser (Stempel FUN_80051a3c).

## 4. Fix-Plan

### A) Finisher — enemy_ai_boss_gator.c, `case GBP_FRESSEN` (kein neuer Helfer nötig)

Konstante: `s = e->render_scale_q12 ? e->render_scale_q12 : 4096` (GB_SCALE_Q12 2731, :139;
Skalierungspfad @0x8001e904-40 / main.c:8256). Reihenfolge:
1. **P0 (:1924-1925):** Teleport-Versatz skalieren: `pl = e + RotY·(s·10643>>12, s·(−915)>>12)`
   (= (7096,−611) bei 2731; @0x8010106C-84). Zusätzlich `pl->y = e->y − ((4096−s)·POSy_v0(f))>>12`
   ist hier NICHT nötig (Opfer-Clip 0 wird ohne Platzierung gehalten, :1935); nur die xz.
2. **P1 Anker (:1939-1940):** unverändert (xz-Anker, @0x80101110-34); zusätzlich
   `pl->anchor_y = e->y` ist wirkungslos (SPEED y = 0 in allen Frames, gemessen) — weglassen,
   damit kein toter Wert entsteht.
3. **P2 (:1981-1986) und P3 (:2017-2022):** nach den beiden `re15_clip_root_motion_abs_pub`-Rufen
   den Opfer-Versatz um G skalieren und y setzen:
   ```
   pl->x = e->x + (((int64_t)(pl->x - e->x) * s) >> 12);      /* s·RotY·(off_v − off_g), §2.2 */
   pl->z = e->z + (((int64_t)(pl->z - e->z) * s) >> 12);
   pl->y = e->y - (((int64_t)POSy_v(sf) * (4096 - s)) >> 12);  /* Wurzel = G.y + s·POSy_v; Renderer addiert POSy_v 1× */
   ```
   POSy_v(sf) = `re15_emd_get_keyframe_position(&gb23->skel_victim, kf_of(anim_victim, 1, sf))`.y
   (0x8000-Skip wie enemy_ai_common.c:630-635). Bei s = 4096 ist das identisch mit heute plus
   y-Leitung (pl->y = e->y), also auch für einen späteren Scale-Verzicht korrekt.
4. Sonde-Erwartung nach dem Fix: probe_r16_gator_finisher_anker Teil B muss LOKAL = Original ±5
   und 6=1/7=1 bei P3 liefern (heute 7892/−1804/295, 6=0/7=0). Pin: `probe_gator_fress`
   (unit_gator_fress, CMakeLists.txt:3586) um genau diese Prüfung erweitern (lokale Wurzel bei
   P3 innerhalb Mesh-7-AABB x[4130..7381] y[−4527..−1776] z[−986..984] der Clip-11-f0-Pose).
Risiken: (i) main.c game_step könnte `pl->y` im Victim-Modus 4 anfassen (Boden-Snap) — in
Teil B blieb pl->y unberührt, aber game_step ist in der Sonde nicht gelinkt → nach dem Fix mit
RE15_STATE_LOG die pl-y-Spur während FRESSEN prüfen (Erwartung ≠ 0, z.B. −399 bei f119).
(ii) Leon bleibt 1× groß im 2/3-Maul (Nutzer-Scale) — optisch ragt der Rumpf heraus; das ist
die Grenze dessen, was ohne Scale-Verzicht geht, und ehrlich zu sagen. (iii) Der 1-Tick-
Rücksprung sf=120 (Clip 5 f120%120) besteht weiter (Runde 15, Original ebenfalls 1 Tick).

### B) Alligator-Angriffs-SE — enemy_ai_boss_gator.c

Ziel: der RE2-Angriffslaut. Zwei Stufen, beide mit Beleg:
1. **Minimal:** an den drei LUNGE-Starts (:1345, :1409, :1751) `gb_se(4)` (RE2 Lunge-P0
   @0x80100d84, `addiu a0,zero,4` @0x80100d64) ZUSÄTZLICH zum Mapper-Ruf `gb_se(3)` (→ snd1 SE 2
   am Connect, Nutzer-Mandat Runde 5); `gb_se(4)` lädt die 55-F-Sperre (:284, @0x80100430-34).
   Der Brüller (18032 B, 1,43 s Attack) endet damit ~1 s nach dem 45-F-Schnapp — hörbar als
   Angriff, zeitlich nicht wie RE2.
2. **Byte-näher (runde-6 §7a.2):** LUNGE = Clip 2 (frac 16 @0x80100dac-b4) → Clip 3 (frac 0
   @0x80100e20-28), Biss-Fenster Clip 3 f≥114 (@0x80100e74-a4), Frame-Flags f50/f77 SE 4 kommen
   dann aus den DATEN über :904-918; danach `Yaw += 0x400` (@0x80100eb0-c0). Das ersetzt den
   Port-Clip-4-Lunge und macht Runde-4s SE-3-Mapping überflüssig. Größerer Umbau (Trefferfenster
   des Bosskampfs neu messen: gb_maul_dist, Commit-Gates :1338-1346).
Pin: probe_r16_gator_se → erwartete SE-Folge je Lunge `4` (Start) … `3` (Connect); ctest
`unit_gator_sweep` unverändert grün.

### C) Spinnen-Angriffs-SE (und alle RE2-SEs in Mischräumen) — audio_pc.c + Hooks

Der Defekt ist der Ein-Bank-Latch. Fix (PC-Backend, engine-Hooks unverändert in der Signatur):
1. audio_pc.c: `s_re2se_*` in einen kleinen Cache (z.B. 3 Einträge {bank, edt, map_count, vab,
   decoded[]}) umbauen; `load_re2_enemy_se_pc(bank)` füllt/holt einen Cache-Eintrag statt den
   Vorgänger freizugeben (heute :1031-1044 zerstört die laufenden Stimmen der anderen Bank).
2. `re15_audio_re2_enemy_se(se_id, flag2000)` bekommt die Bank vom RUFER: die Engine-Hooks
   rufen vor jedem `s_*_se_fn` ihr `s_*_bank_fn(BANK)` (Gator :278, Spinne :147, Hund/Krähe/
   Zombie/G5 analog) → der Latch ist dann je Ruf richtig, der Cache vermeidet das Neuladen.
   Kanal-Prioritäten (`s_re2se_prio`, FUN_8005c92c-Gate) bleiben EIN Satz — wie im Original ein
   Bank-Slot; das ist die verbleibende Port-Abweichung (RE2 hätte keine Bank für diesen Raum).
3. Verifikation: probe_r16_gator_se mit beiden Hooks → jeder Ruf "Latch == zuständig"; im Spiel
   RE15_SE_DEBUG=1 (stderr ist bei der GUI-exe tot — RE15_VOICE_LOG-Muster audio_pc.c:1550 als
   Datei-Log für `[re2se]` nachziehen).
Nebenbei (kein Blocker): Baby-Spinnen-Hook zur Laufzeit (main.c WELLE F) flippt den Latch heute
mitten im Kampf auf 11 — mit dem Cache/Per-Ruf-Latch erledigt.

## 5. Offen / nicht belegt

- Ob RE1.5s FUN_8001e9ec die WURZEL-POSE (kf+0..5) durch die skalierte Matrix führt, ist aus
  dem Decompile (local_48.t = part+0x88.. × param) nicht Instruktion für Instruktion belegt;
  für den Port irrelevant, weil main.c:8256 (die Render-Wahrheit des Ports) sie skaliert.
- Der Scale 2731 bleibt ohne @0x (Nutzer-Entscheidung); Fix A ist die exakte geometrische Folge
  des Scale-Pfads, nicht ein Original-Wert.
- Ob RE1.5-snd1 SE 2 im Spiel hörbar durchkommt (Stimme 4/prio 2, Gate @0x80045a34-58 gegen
  snd0-Schritt-Records derselben Stimme) wurde nicht am laufenden Spiel gemessen (kein
  Audio-Dateilog); die Sonde belegt nur, dass der Ruf ankommt und den Mapper erreicht.
- Spinnen-Szenario A erreicht Sub 7 in 1500 F nicht — die Stalk→Angriff-Bedingungen (Sub 5/6
  @0x80100B80) sind hier nicht vermessen; der Angriffs-SE-Pfad selbst ist über Szenario B belegt.
- RE2s Chan/Prio-Maschine bei zwei gleichzeitig aktiven Bänken hat kein Original-Vorbild.

## 6. Umsetzung (Phase 2, 2026-09-19, Branch worktree-wf_074e2f88-24e-3)

Korrekturen der Gegenpruefung (Skeptiker-Datei) galten vor dem Fix-Plan. Alle Adressen in
diesem Abschnitt wurden mit `re2_disasm.py` aus `EM23_OVL_0000.BIN` / `info/re2leon/PSX.EXE`
selbst nachgelesen. Messlogs: `analysis/befunde_2026-09-19/phase2_gator-und-audio/`.

### Commits

- `e3fb82bb fix(gator)`: Opfer-Versatz im Entity-Render-Scale (Helfer `gb_opfer_skalieren`),
  P1-Anker/P2/P3 ueber die dreikomponentigen y3-Zwillinge, P0-Teleport nur 10643 skaliert
  (-915 als Port-Wert kommentiert, Original `sw v1,0x800cfc38` @0x80101078 = absolut),
  `gb_se(4)` an den drei LUNGE-Starts (@0x80100d60/d64/d84), 55-F-Sperre nur noch aus dem
  0x4800-Frame-Wort (@0x80100424-34), FSYNC2/FSYNC3-Log mit `y_vor`.
- `31bd06e3 fix(audio)`: Mehrbank-Cache (3 Eintraege, LRU, nie der aktive) in audio_pc.c,
  Bank je Ruf vom Rufer, `RE15_RE2SE_LOG=<Pfad>`-Dateilog; Hooks Gator/Spinne/Hund/Kraehe/
  Zombie/G5 rufen `bank_fn` vor jedem SE (Tentakel unveraendert, teilt G5s Bank).
- Sonden/Pin/Doku (dritter Commit): `probe_gator_fress` (Pin, add_test unit_gator_fress)
  prueft jetzt Leons lokale Wurzel bei P3 (+-5 zum Original, in Mesh-7-AABB, pl->y != 0);
  `probe_r16_gator_se` zaehlt Latch==zustaendig, SE-4-Rufe, misst VAG-Dauer und die
  Kieferkurve der Clips 2/3/4; `probe_r16_spider_se` registriert den Gator-Hook nach der
  Spinne (Spiel-Reihenfolge) und zaehlt Latch==zustaendig.

### Messwerte vorher / nachher

| Messung | vorher | nachher |
|---|---|---|
| Leon-Wurzel gator-lokal bei P3 (probe_r16_gator_finisher_anker Teil B, echter Boss-Tick) | (7892,-1804,295), dRef (2628,599,97), 6=0/7=0 | (5261,-2402,194), dRef (-3,1,-4), 6=1/7=1 |
| P2 sf 30/60/90/119 dRef (Teil B LIVE) | +2341..+3572 / -2090..+1183 | (-4,1,-1) / (-4,2,-1) / (-5,1,1) / (-3,1,-4); sf=120 = 1-Tick-Wrap (+161, wie Original 1 Tick) |
| Gegen-Sonde Skeptiker (121 Frames, S/F vs O) | - | max dx/dy/dz 3/2/3, Kiefer-6/7-Abweichungen 0/0 (unveraendert bitgleich) |
| pl->y bei P3 | 0 | -399 (Sonde UND Spiel) |
| Spiel (RE15_DEBUG_JUMP=2090@gp, GB_TEST=1, AI_FLAVOR=re2): FSYNC2 y_vor | - | jede Zeile y_vor == Wert des Vortick-Ticks der Sonde (-1007, -761, -761, -793, -844, -868, -625, 1764, 1529, 395, -60, -151, 114, -133, -399) -> game_step ueberschreibt pl->y im Victim-Modus 4 NICHT (Risiko 4A(i) ausgeraeumt; RE15_STATE_LOG traegt kein y, darum die Engine-Zeile) |
| probe_r16_gator_se: SE-Rufe Latch==zustaendig | Ein-Latch (17 fuer alle) | 7/7, MISMATCH 0, bank_fn-Rufe 9, direkte SE-4-Rufe 1 je Lunge |
| probe_r16_spider_se (Gator-Hook NACH Spinne registriert) | Latch 17 -> id 1 = Gator-Sample, 8/9 SILENT | Latch nach Registrierung 17, danach 26/26 Rufe mit Latch 11, Biss-SE 1 x4 |
| Spiel re2se.log | (kein Dateilog) | Bank 11 Cache-Slot 0, Bank 17 Slot 1; jeder Ruf `bank == gewaehlt` (Spinne 0/8/9/1 aus 11, Gator 2/4/1/3 aus 17); SE 4 an beiden Lunges (F199, F306) |
| SE-4-Dauer (Bank 17 id 4, 18032 B) | "2,40 s" vs "1,43 s @22050" | Tone center 85 / min 64 -> Pitch 0x4C1 = 13103 Hz, 31556 Samples = **2,41 s** (Port-Wiedergabe, note2pitch2); die 22050-Hz-Annahme war falsch |
| ctest | 224/224 (Basis) | **305/305 gruen, 226 s** (`100% tests passed, 0 tests failed out of 305`), unit_gator_sweep und unit_gator_fress gruen |

Kieferkurve (Bone 7, Rot-Z, Q12) aus den EM23-Daten, probe_r16_gator_se Teil 5:
Clip 2 (150 F) erster Ausschlag > 64 bei f7, Peak 341 bei f104; Clip 3 (150 F): kleines
Oeffnen f9..f42 (max -131 @f18), das GROSSE Oeffnen ab f54 (-153) -> f60 -329 -> Peak -477
@f96 -> zu bei f132; RE2-Bissfenster f >= 114 (@0x80100e8c `sltiu v0,v0,0x72`) liegt im
Schliessen (-285 @f114 -> -18 @f132). "Maul offen ab ~f54" ist damit als Beginn des grossen
Oeffnens gemessen, nicht mehr geschaetzt. Clip 4 (45 F): Peak -591 @f13, zu @f24.

Prioritaets-Gate FUN_8005c92c selbst nachgelesen (@0x8005c93c lbu 0x800d4ca0[chan*2],
@0x8005c940 andi 7, @0x8005c944 sltu, @0x8005c958 bne, @0x8005c960-64 andi 8/sltu) —
zeilengleich mit dem Port-Kommentar. Sichtbare Folge des EINEN Prioritaetssatzes im Spiel:
`se=0 bank=11 GATE chan=2 prio-nib=1 VERWORFEN (laufend 3)` — ein Spinnen-Schritt wird
verworfen, waehrend Gator-SE 1/3 (Kanal 2, Prio 3) laeuft. Port-Abweichung, benannt.

### Sichtpruefung am FRAMEDUMP (Skeptiker-Abschnitt-4-Punkt 3)

Der offene Gegenpruefungs-Punkt "Renderer-Annahme: Leons Wurzel = pl->(x,y,z) +
RotY(pl->rot_y)*POSE_v ist nur aus main.c belegt, ein Live-Bild fehlt" ist jetzt am Bild
erledigt. Lauf `run2/` (`RE15_DEBUG_JUMP=2090@gp`, `RE15_GB_TEST=1`, `RE15_AI_FLAVOR=re2`,
`RE15_FRAMEDUMP` alle 20 Bilder, `RE15_BOOT_EXIT_AT=2`); die Bildnummer IST der Spiel-Frame
(debug.log `[scd F594]` und state.log tragen denselben Zaehler). Gator-Modus je Frame aus
state.log Slot 15: F199..F241 und F306..F332 mo=4 (Lunge), **F333..F452 mo=5 (FRESSEN P1/P2)**,
**F453..F572 mo=11 (P3 Kau-Loop)**, danach mo=0.

- **`run2/fd_000460.png` (F460 = P3, gaf=7): Leon liegt ZWISCHEN Ober- und Unterkiefer** —
  blaue Jeans und Jacke sind im offenen Maul sichtbar. `run2/fd_000520.png` (Todes-Kamera von
  oben) zeigt dasselbe aus der Gegenrichtung: Jeans direkt an der Schnauze. Das ist genau der
  Zustand des Nutzer-Befunds ("er liegt darunter") — und Leon liegt jetzt drin. Damit ist die
  Renderer-Annahme nicht mehr nur aus main.c hergeleitet, sondern am Bild bestaetigt.
- In P2 (`fd_000340/360/400`) steht der Gator mit dem KOPF am linken Bildrand; Leon haengt am
  Maul und ist dadurch groesstenteils ausserhalb des Bildes. Dort bleibt nur die lokale
  Relation messbar (Sonde, Tabelle oben) — sichtbar wird der Sitz erst in der P3-Nahkamera.
- Gegenprobe zur naheliegenden Fehldeutung: die liegende Gestalt in der Bildmitte ist NICHT
  Leon. Sie liegt schon in `fd_000200` (F200, Spieler lebt, steht sichtbar links am Kopf des
  Gators) an derselben Stelle und ist zwischen F200 und F360 **pixelgleich** (Regionen-Diff
  x 520..820 / y 300..460: 0 abweichende Pixel), waehrend sich die Spielerposition zwischen
  diesen Bildern um ueber 3000 Einheiten aendert (state.log F200 PL(-8432,-26538) vs F360
  PL(-10543,-29779)).

Die drei Belegbilder liegen versioniert unter
`analysis/befunde_2026-09-19/phase2_gator-und-audio/bilder/` (`F460_P3_leon_im_maul.png`,
`F520_todeskamera_jeans_an_der_schnauze.png`, `F200_spieler_lebt_links_am_kopf.png`), die
Spiel-Logs desselben Laufs als `spiel_gator_boss.log` / `spiel_re2se.log`; die vollstaendigen
Laufverzeichnisse `run1..run4` (Savestates der Karte, state.log, alle 20 Bilder) bleiben
unversioniert.

Zwei Zahlen-Praezisierungen zur Tabelle oben: SE 4 sind **31556 Samples nominal**
(18032 B / 16 * 28, Sonde) bzw. **31528 Samples tatsaechlich dekodiert** (re2se.log des
Spiels) — beide bei Pitch 0x4C1 = 2,41 s. Und die ctest-Summe: der Lauf des Vorgaengers war
305/305 (build_p2_ctest1.log); die erste Wiederholung in dieser Sitzung lief 304/305, weil
`integration_save_counter_pin` nach 5,7 s abbrach und allein wiederholt in 28,9 s gruen war —
der Pin startet drei `re15_pc.exe`-Laeufe, und parallel arbeitende Agenten beenden
`re15_pc.exe` ueber den IMAGE-NAMEN. `run_gator.sh` beendet deshalb nur noch die EIGENE PID
(vorher `taskkill //F //IM re15_pc.exe`); der abschliessende Gesamtlauf danach ist wieder
**305/305 gruen in 208 s** (`100% tests passed, 0 tests failed out of 305`,
`integration_save_counter_pin` 30,3 s).

### Offen

- **Stufe B2 (Lunge = Clip 2 frac 16 -> Clip 3, Bissfenster f >= 114, Yaw += 0x400) NICHT
  umgesetzt.** Das Original faehrt bei +0x22C != 0 den Clip-Advance 0x8001a330 ZWEIMAL pro
  Tick (@0x80100da4-dd0 Clip 2, @0x80100e18-e4c Clip 3) — der Doppelschritt gehoert zum
  Umbau. Damit wuerden 300 authored Frames (150+150, bei Doppelschritt 150) den 45-F-Clip-4-
  Schnapp ersetzen, an dem alle Trefferfenster des Nutzer-Bosskampfs (gb_maul_dist, Fenster
  6..34, Commit-Gates) und unit_gator_sweep haengen. Das ist ein eigener Umbau mit neuer
  Fenster-Messung; hier bewusst nur Stufe 1 (SE 4 am Lunge-Start, 2,41 s, Ende ~1 s nach
  dem 45-F-Schnapp) — hoerbar als Angriff, zeitlich nicht wie RE2.
- Leon bleibt 1x gross im 2/3-Maul (Scale 2731 = Nutzer-Entscheidung ohne @0x); der SITZ der
  Wurzel ist jetzt die Original-Relation, die Proportion nicht.
- Kanal-Prioritaeten bleiben EIN Satz fuer beide Baenke (s.o., Gate-Zeile im re2se.log).
- Tentakel (0x37) rufen kein bank_fn (kein eigener Hook); im Endkampf nur Bank 25 aktiv.
- P0-Teleport: -915 als Seitenversatz ist Port-Entscheidung (Original absolut), Wirkung nur
  in P1 (13 Bilder).
