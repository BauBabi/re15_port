# Runde 36 — Spur C "schuss": Abnahme 2 (unabhaengig, 2026-10-10)

Baum `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, geprueft am Commit d5c0c14f (Nachbesserung 2 nach
Abnahme 1, `C_abnahme_1.md` §7 M1). Nur gelesen/gemessen, kein Code im Baum geaendert. Eigene Sonden/Skripte/Logs
liegen im Scratchpad dieser Abnahme (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn2C_r36/`
(Kurzform `abn2C/...`).

(Bericht wird fortlaufend geschrieben — Stand: in Arbeit.)

## 0. Gelesen / gebaut
* AUFTRAG.md (Wortlaut der drei Punkte), VERTRAG.md (Spur C, §1.4, §2), Dossier C_schuss.md §9 (Nachbesserung 2),
  C_abnahme_1.md (M1), `git log master..HEAD`, `git diff master --stat` (73 Dateien, +4973/-124),
  `git diff 6fa888c2 HEAD -- re15_port` (Code der Nachbesserung 2) gelesen.
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  re15_pc.exe 2026-10-10 21:43:37, keine Quelle unter engine/include/platform/tests juenger als die exe;
  `git status re15_port` leer.

## 1. Punkt 2 — Blut an der richtigen Stelle (Gegenstand der Nachbesserung 2)

### 1.1 Code gelesen (Diff 6fa888c2..d5c0c14f) gegen den echten Zeichner platform/pc/main.c
* `re15_schuss_r36_gezeichnet` (schuss_r36.c) bildet main.c nach: Bankwahl 9799-9925 (re15_actor_anim_select-Gegnerzweig
  = eb->skel/anim, clip_override = motion nur bei +0x1C4 & 4, anim_select_common.c:256-291; danach Loco-Bank, Kanal-Kette
  state 4, eigene Bank, Sitz-Import in derselben Reihenfolge), Keyframe mit anim_frame (main.c:9954, target_fps 30 = Default
  main.c:3732), Crossfade mit g_anim_pose_actor (9960) auf einer Schattenkopie (der Blend-Zustand liegt komplett im Aktor:
  anim_frac/anim_prev_valid/anim_blend_rate/prev_root/prev_angles, skeleton_common.c:217-331; die Neck-FSM, die
  `a - g_actors` liest, laeuft nur fuer neck_bone != 0 = Spieler/NPC 0x40..0x4D, nicht fuer Zombies), Unterhaelfte
  (10198-10217), Yaw + Wurzel (10269-10289), Ragdoll-Anker (10316-10329), eigene Teil-Matrix (10338-10341) per
  `re15_re2z_gore_part_matrix_peek` (nur Lesen; Flag 0x40 + Bit 0 + geseedet). re2z_gore_fx_core ruft den Anker jetzt
  auch fuer a3 = NULL (enemy_ai_re2_zombie.c:1131). Keine neue Zahl im Diff.
* Abweichungen des Zwillings vom Zeichner, die ich gefunden habe, sind fuer RE2-Zombies ohne Wirkung: die Sentinel-Zweige
  von anim_select (motion 100/105/200..235, anim_select_common.c:396-487) fehlen im Zwilling — Zombie-Clips liegen weit
  darunter; Teile mit `nbi >= npc_bones` (Wurzelpose) — beim Hybrid-Rig gleich viele Bones in allen Baenken.

### 1.2 Eigene Sonde mit WOERTLICHEM Zeichner-Zwilling (`abn2C/abn2_blut.c`, nicht im Repo)
Unterschiede zur Sonde des Bau-Agenten (probe_r36_blut/nb2_blut): (a) Bank wie die exe = RE2-Bank + HYBRID
(re2_hybrid_apply mit der RE1.5-EMD, main.c:797-905 nachgebaut; die Sonde des Bau-Agenten laedt die reine RE2-Bank);
(b) der Zeichner ist main.c 9791-10341 Zeile fuer Zeile (re15_actor_anim_select, Remap-/Bone-Zahl-Regeln, gore_resolve,
Zellenarm-Maske, **Zeichen-Skip fuer !gore_draw — ein nicht gezeichnetes Teil bekommt wie in main.c:10172 KEINEN
Physikschritt** (die Sonde des Bau-Agenten ruft re15_re2z_gore_part_matrix fuer jeden Knochen), g_anim_pose_actor bleibt
wie in main.c nach dem Zeichnen gesetzt), danach frame_count++ (main.c:11483); (c) Referenz = die im SELBEN Bild gezeichnete
Matrix des Parts (Spawn im Spielschritt -> Zeichnung nach dem Schritt; Spawn waehrend des Zeichnens -> Matrix nach dem
Physikschritt), zusaetzlich T-1 (Original-Zeitpunkt, nur Information). Gebaut mit den Flags aus build.ninja gegen
engine/libre15_engine.a (21:43) + tests/libre15_test_support.a.
Lauf-Matrix: Waffen 3/4/5/6/7/8/12/13/19/20 x Seite px/pz/nx/nz x eben/tief x Platz 2/3/4/5 = 320 Laeufe (ein Zombie aktiv,
bis Tod + 150 Bilder), 4784 Spawns (`abn2C/blut/*.txt`, Klassierung `abn2C/klassen.py` -> `abn2C/klassen.txt`):

| Klasse | Spawns | d(gezeichnet, Bild T) | Einordnung |
|---|---|---|---|
| Treffer-/Todes-/Aufsteh-/Tropf-/RIP-/Magnum-/Burst-/Arm-Emitter (a3 und Gruppe B) | ~3000 | 0..3 (Loco-Bank-Treffer bis 30, AUF Part 8 4x 42-43) | Anker = gezeichnete Matrix — **bestaetigt** |
| Aufschlag-Blut fliegender Teile (Spawn beim Zeichnen, @0x80028C4C) | 112 | **0** | am Glied auf dem Boden — **bestaetigt** |
| Teil fliegt in diesem Bild ab (Seed-Pose vs. 1. Physikschritt): Arm Part 3 / RIP-Kopf Part 8 / MG-Glied Part 10,13 | 256 / 16 / 24 | <=10 / 361 / 150-185 | Mechanismus wie Original (Anker = zuletzt gezeichnete Matrix, das Teil fliegt erst beim Zeichnen) |
| MG-Trefferbilder (Ingram/MC51) | 1250 | <=103 | Drehung/Schub NACH dem Spawn @0x8010810C-6C (T-1-Abstand <=79) |
| **Kriecher-Kopfabriss (Bein-Biss-Befreiung), Part 0, Welt-Versatz (0,-600,0)** | 25 (in 25 von 80 MG-Laeufen) | 600 | **s. 1.3 — Mangel** |
| **Hals-Stumpf-Spruehen, Part 8, Welt-Versatz (0,+100,0)** | 286 | 100 | **s. 1.3 — Mangel** |

Gegenprobe VORHER mit derselben Sonde: `abn2C/abn2_blut_alt.exe` = abn2_blut.c gelinkt mit schuss_r36.c und
enemy_ai_re2_zombie.c aus 6fa888c2 (`git show`, eigene Objekte `abn2C/alt/*.o`) vor der aktuellen libre15_engine.a; dieselben
320 Laeufe (`abn2C/blut_alt/`, `abn2C/klassen_alt.txt`): Aufschlag-Blut d bis **2259** (jetzt 0), Loco-Bank-Treffer 129-153
(jetzt <=30), Arm-Abriss Part 3 bis 176 (jetzt <=10), MG-Glied 212/238 (jetzt 150-185 = erster Physikschritt), SPAS-RIP-P1
104 (jetzt 0), Hals-Stumpf 681 (am Rumpf-Knochen statt am abgeflogenen Kopf; jetzt 100 Welt-Versatz am Kopf). Der Kriecher-
Kopfabriss (K1) ist vorher wie nachher 600 — von der Nachbesserung nicht beruehrt.
Die Pins des Bau-Agenten selbst nachgefahren (`abn2C/pins/`): neu alle sieben `probe_r36_blut: OK` ((G) max d eben 24, tief 32,
mg 88, rip 0, arm 0, burst 0, glied 73); gegen die alten Objekte gelinkt (`abn2C/alt/probe_alt.exe`, Objekt
probe_r36_blut.c.obj aus dem Bau): eben 4, tief 4, rip 7, arm 1, burst 1, glied 3 FAIL, mg 0 — exakt die Zahlen des Dossiers §9.5,
die Pins sind trennscharf.

### 1.3 Befund: zwei Blut-Emitter am getoeteten Kriecher mit Welt- statt Anker-Versatz (nicht von M1 erfasst)
In 25 der 80 MG-Laeufe wird der Zombie zum Kriecher (+0x10E Bit 0), packt Leon am Bein, und die Befreiung (Leon-Zweig,
enemy_ai_re2_zombie.c:2002-2049) trennt ihm den Kopf ab. Der Port spawnt dort Blut mit WELT-Versatz:
`re2z_gore_fx_ex(e, 0, 0x00801000u, rot_y, 0, -600, 0)` (:2028) und `re2z_gore_fx_ex(e, 8, 4096u, ang, 0, 100, 0)` (:2048).
Selbst disassembliert (re2_disasm.py, info/re2leon/COMMON/BIN/EMZ0.BIN; s1 = Zombie: `lhu v0,270(s1)` / `andi s5,v0,0x1`
@0x8010266C-74 = Kriecher-Bit +0x10E):
```
80102aec: addiu a3,sp,24                 ; a3 = ZEIGER auf einen Stapel-Vektor (Anker-Rahmen), kein Welt-Versatz
80102b0c: lw a2,408(s1)                  ; Modellblock +0x198 des ZOMBIES
80102b10: addiu v0,zero,-600 / 80102b14: sh zero,24(sp) / 80102b18: sh v0,26(sp) / 80102b1c: sh zero,28(sp)   ; a3 = (0,-600,0)
80102ad4: lui a0,0x800 / 80102adc: ori a0,a0,0x1000                                ; Id 0x00801000 -> id 0 = BLUT
80102b24: jal 0x8001bf10 / 80102b28: addiu a2,a2,72                                ; Anker Part 0
80102b8c: addiu v0,zero,100 / 80102b94: sh zero,24(sp) / 80102b98: sh v0,26(sp) / 80102ba0: sh zero,28(sp)  ; a3 = (0,100,0)
80102ba4: addiu a0,zero,4096 / 80102ba8: addiu a3,sp,24                            ; Id 4096 -> id 0 = BLUT
80102bbc: jal 0x8001bf10 / 80102bc0: addiu a2,s0,1448                              ; Anker Part 8 (8*172+72)
```
Dieselbe a3-Semantik wie Zensus-Gruppe A (Lage = Anker.R * a3 + Anker.T, FUN_8001BF10 + Lagen-Tick). Der Zensus des
Bau-Agenten (Dossier §8.8 C) fuehrt 0x80102B24/0x80102BBC als "Griff/Biss — kein Blut am getroffenen Zombie"; das ist
falsch: beide sind Blut (Id 0) am Modellblock des Zombies, der dabei stirbt. Gemessen (abn2_blut, z.B. w12 px eben Platz 2,
F192): Port-Spawn `fx=(-16,-846,-19622)` = 600 senkrecht ueber dem Becken des am Boden liegenden Kriechers; mit der
Original-Semantik T + R*(0,-600,0) laege er bei `(568,-114,-19589)` — am Hals (Kopf fliegt ab bei (644,-48,-19587)):
**Abstand 936-937 in allen 25 Faellen.** Hals-Stumpf: Port Kopf + (0,100,0), Original Kopf + R*(0,100,0): **148-149** in
286 Spawns. Das ist genau der Wortlaut "Blut nicht an der richtigen Stelle" (Kriecher wird von Leon getoetet).
Warum es die Pins nicht sehen: `probe_r36_blut pin mg` bricht beim Kriecher ab (`F604  Kriecher (10E=2001) — MG-Lauf endet
hier`, probe_r36_blut.c:485), die uebrigen Varianten erzeugen keinen Kriecher.

### 1.4 exe (echter Renderer), eigene Messkopie `re15_pc_abn2c.exe`, `abn2C/p2_batch.sh` + Einzellaeufe
Aufbau: `RE15_DEBUG_JUMP=1140@250`, RE2-KI, `RE15_GIVE=12:250`, `RE15_EQUIP=12`, `RE15_FX_LOG`/`RE15_STATE_LOG`,
`RE15_FRAMEDUMP`, `RE15_WINDOW_SCALE=1`; alle Laeufe exit=0 (e2e: 124, Zeitgrenze, nicht ausgewertet).
* **E1 = Aufbau des Bau-Agenten** (Leon (-5000,-19600) Blick +x, `RE15_INPUT_SCRIPT=W1.2,M0.6,MA9.0,M0.5,W2`, Exit 300):
  Zustandsfolge Platz 2 F55..F74 50 -> 2, F75 Tod st 3/15/1, F136 st 7. Blut-Spawns (`fx_auswertung.py` des Bau-Agenten ->
  `abn2C/p2/e1_fx.txt`) bitgleich mit seinem `n2/exe/mg_neu_fx_auswertung.txt` bis zu dessen Ende, darunter das Aufschlag-Blut
  **`F77 sub=0 w(-2325,0,-19731)`** und **`F81 sub=0 w(-2333,0,-19727)`** (am Boden; vorher laut Bau-Agent y -1850/-1821).
  Bild (`abn2C/p2_e1_links.png`, F72-F89, Ausschnitt x0-110/y60-200, 3x): der Aufschlagpunkt liegt in diesem Kamerabild im
  MG-Blut am linken Rand und ist nicht einzeln zu erkennen — Beleg sind die fx-Zahlen (wie im Dossier §9.5 angegeben).
* **E2f = Kriecher-Kopfabriss an der exe** (Leon (-3600,-19600) Blick +x, `W1.2,M0.6,MA3.0,M0.3,W16`, Exit 700): Platz 5 kriecht
  heran, packt Leon (state.log F226 `[5 t=11 st=1 ss1=1 ss2=5 ... mo=16 af=20 ... @(-2734,-19776,r2199)] hp=226`, Leon hp
  100 -> 70), Befreiung, Kopf ab. fx.log: **`id=0 sub=128 ... frame=1 w(-2732,-849,-19781) ... F=226`** (Id 0x00801000) =
  senkrecht ueber der Kriecher-Wurzel (-2734,-19776) in 849 Hoehe; danach Hals-Stumpf `id=0 sub=0 ... w(-3376,-51,-19641)`
  F227..F243 jedes zweite Bild am Kopf (Boden). Bild `abn2C/p2_e2f_crop.png` (F222-F233, Ausschnitt x0-130/y60-160, 4x):
  F222-F225 Leon tritt zu, ab F226/F227 erscheint der rote Blutballen **in der Luft auf Kniehoehe Leons**, nicht am Hals des am
  Boden liegenden Kriechers. Damit ist 1.3 auch am echten Renderer reproduziert.

## 2. Punkt 1 — "Alle Schusswaffen treffen ueber den Konferenztisch und treffen fressende Zombies. Im Original geht beides nicht."
Code seit Abnahme 1 unveraendert (`git diff 6fa888c2 HEAD` leer fuer re15_damage.c, enemy_ai_common.c, enemy_ai_re2_crow.c,
re15_actor.h, test_r36_schuss.c/.cmake, test_r36_schuss_kraehe.c; schuss_r36.c nur im Blut-Teil). Nachgemessen an der exe
dieses Stands (eigene Messkopie `re15_pc_abn2c.exe`, `abn2C/lauf.sh` + `abn2C/p13_batch.sh`, Aufbau wie Abnahme 1:
`RE15_DEBUG_JUMP=1140@250`, `RE15_PLAYER_POS`, `RE15_AI_FLAVOR`, `RE15_GIVE=<w>:6`, `RE15_EQUIP=<w>`,
`RE15_INPUT_SCRIPT=M0.6,MA1.5,M1.0,W4`, `RE15_EXIT_AT=200#1140`, state.log/wf.log; Auswertung `abn1C/hp.py`), alle 42 Laeufe exit=0:
| Aufstellung | Waffen | Ergebnis |
|---|---|---|
| tisch_re15: Leon (200,-10300) Blick -z, RE1.5-KI (Fresser dort treffbar) | 3,4,5,6,7,8,12,13,19,20 + 14,15,16,17,18 | alle 15: Munition faellt (z.B. w3 6->3, w12 6->0, w15 6->4), KEIN Platz verliert HP (P1 65, P2 75, P3 65, P4 250, P5 250) |
| diag_re15: Leon (6000,-10300) rot 1387 (Diagonale durch die Tischzelle) | 3, 8, 12, 20 | kein HP-Verlust |
| Kontrolle frei_re15: Leon (200,-18200), RE1.5-KI | 3, 8, 12, 20 | Treffer: w3 P5 250->218 (F19); w8 P5/P4 -> -1 (F20/F23); w12 P5/P4 -> 242; w20 P5/P4 -> -1 (F19/F44) |
| fresser_tuer: Leon am Tuerpunkt (-7600,-17600) Blick +x, RE2-KI | alle 15 | alle 15: Munition faellt, KEIN Platz verliert HP (P2..P5 st1/ss1 8 = EXEC[8], P1 st1/7) |
| Kontrolle weck_re2: Leon (200,-18200), Fresser geweckt | 3, 8, 12, 20 | Treffer: w3 P5 -> 202; w8 P2/P3/P4/P5 -> -1/-120/-150/-1; w12 P5 -> 226; w20 P5/P4 -> -1 |
Zahlen bitgleich mit Abnahme 1 (§4.1 dort). **Urteil Punkt 1: erfuellt** (exe dieses Stands; zweiter Raum ROOM10E0 und
echter Tuerweg hat Abnahme 1 gemessen, Code seither unveraendert).

## 3. Punkt 3 — "Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in RE2 auch so?"
Code seit Abnahme 1 unveraendert. Beleg (Abnahme 1 selbst gelesen): EMOVL21_S0.BIN Tabelle @0x80104A18, Zeile 5 (Magnum) ->
0x80102CA0 GIB. exe dieses Stands (`RE15_DEBUG_JUMP=10C0@250`, `RE15_PLAYER_POS=3522,5556,581`, `RE15_FORCE_CUT=1`, RE2-KI,
`RE15_INPUT_SCRIPT=MD0.6,MDA0.4,MD1.0,W3`, `RE15_FRAMEDUMP=14-70/1`, `RE15_WINDOW_SCALE=1`, `RE15_EXIT_AT=110#10C0`), state.log Platz 3:
* w7 Super Redhawk: `st=1 ss1=0 ... hp=10` -> `st=7 ss1=1 ... hp=-1` (GIB-Zweig). Bild `abn2C/p3_w7_crop.png` (F17-F40, Ausschnitt
  x100-300/y170-240, 2x): F17/F18 Kraehe sitzt, F19 Treffer, F20-F28 rote Teile/Spritzer fliegen auseinander, F29-F31 verblassend,
  ab F32 nichts mehr — kein Kadaver, keine Lache.
* Gegenprobe w3 Browning: `st=3 ss1=3 ... hp=-5` -> Leiche; Bild `abn2C/p3_w3_crop.png`: Fluegelschlag, Sturz, liegt mit Lache.
* w20 Colt Python: `st=3 ss1=20 ... hp=-190` -> Leiche (kein GIB; Abnahme 1 B2: Kraehen-Zeile 13 statt 5, vor Runde 36, nicht
  Teil des Wortlauts "Super Redhawk").
**Urteil Punkt 3: erfuellt** (Frage beantwortet und am RE2-Dispatch belegt; der Port zeigt das RE2-Zerplatzen).

## 4. RE-Gate (selbst disassembliert, Hauptbaum-Skill `re2_disasm.py`; RE2 info/re2leon/PSX.EXE bzw. COMMON/BIN/EMZ0.BIN)
| Stelle | Behauptung (Commit/Dossier §9.3) | Selbst gelesen | Urteil |
|---|---|---|---|
| PSX.EXE 0x8001BFE0-C004 | a3 == 0 nullt nur den Versatz | `beq a3,zero,0x8001c000` / `lw v0,0(a3)` / `sw v0,44(t0)` / `lw v0,4(a3)` / `j` / `sw v0,48(t0)`; `8001c000: sw zero,44(t0)` / `8001c004: sw zero,48(t0)` | wie behauptet |
| PSX.EXE 0x8001C03C-78 | Matrix fuer alle aus a2 (32 Byte) | `lw v0..a1, 0/4/8/12(a2)` -> `sw 76/80/84/88(t0)`, `lw 16/20/24/28(a2)` -> `sw 92/96/100(..)` | wie behauptet |
| PSX.EXE 0x80027374-90, 0x800273B0-D8 | Part-Walk Stride 172, a3 = Record+72 | `andi v0,a2,0x1` / `jal 0x80027434` / `addiu a3,s2,72` (Delay) / `addiu s2,s2,172`; zweite Schleife `addiu s0,s2,72` ... `jal 0x80027434` / `addu a3,s0,zero` (Teile mit 0x9000 gehen an `jal 0x80027ff0`, ebenfalls a3 = Record+72) | wie behauptet |
| PSX.EXE 0x80027498-9C | Flag 0x40 = eigene Matrix | `andi v0,s3,0x40` / `bne v0,zero,0x800275e4` | wie behauptet |
| PSX.EXE 0x8002751C-DC | Komposition -> Record+72 | nach MVMVA 0x4A49E012 `sh t4,0(s0)` / `sh t5,6(s0)` / `sh t6,12(s0)` (dito @0x80027560-68, @0x800275A0-A8), `addiu a1,s0,20` / 3x `swc2` @0x800275D4-DC | wie behauptet |
| PSX.EXE 0x80027694-A4 | Teile-Physik auf derselben Matrix | `andi v0,s3,0x20` / `beq` / `addu a0,s1,zero` / `jal 0x80028ad8` / `addu a1,s0,zero` | wie behauptet |
| PSX.EXE 0x80028AD8, 0x80028C28-50 | Aufschlag-Blut an der eigenen Matrix des Teils | Prolog `addu s1,a0,zero` (a0 = Record, aus 0x8002769C); `jal 0x80015fe8` / `sll a1,v0,4` / `addiu a2,s1,72` / `addu a3,zero,zero` / `jal 0x8001bf10` / `ori a0,a0,0x7d0` | wie behauptet |
| PSX.EXE 0x800295F8-FC, 0x800296A8-BC | Anim-Paar / Blende a3 * +0x14E | `sw a2,376(a0)` / `jal 0x80029614`; `lbu t3,334(s2)` / `beq t3,zero` / `andi v0,a3,0xffff` / `mult v0,t3` | wie behauptet |
| EMZ0.BIN 0x80108E18-2C | RIP-Kopf a2 = Part 8, a3 = 0 | `addiu a0,zero,3000` / `sll a1,v0,4` / `addiu s0,s0,1448` / `addu a2,s0,zero` / `jal 0x8001bf10` / `addu a3,zero,zero` | wie behauptet |
| EMZ0.BIN 0x80105354-6C, 0x801053BC-D4 | Gliedabriss a2 = Record+72, a3 = 0 | `jal 0x80015fe8` / `addiu a0,zero,8000` / `sll a1,v0,4` / `addiu a2,s0,72` / `jal 0x8001bf10` / `addu a3,zero,zero`; dito 7000 | wie behauptet |
| EMZ0.BIN 0x80106D64-80 | Ragdoll Part 1, a0 0x00031000 | `jal 0x80015fe8` / `lui a0,0x3` / `ori a0,a0,0x1000` / `sll a1,v0,4` / `addiu a2,s0,244` / `jal 0x8001bf10` / `addu a3,zero,zero` | wie behauptet |
Ergebnis: alle 11 Stichproben enthalten genau das Behauptete. Rate-Woerter (deferred/tunable/interim/for now/faithful/plausib/
TODO/approx) in den hinzugefuegten re15_port-Zeilen (master..HEAD): keine. Neue getenv: nur RE15_ASSET_DIR in der Sonde
probe_r36_blut.c. Die Nachbesserung 2 fuehrt keine neue Zahl ein. Bekannte Alt-Zahl ohne Beleg im beruehrten Pfad:
re2z_blood_fx_at Skala 0x1500 (Code-Kommentar "⛔ OPEN", Original am Ragdoll-Emitter 0x00031000 @0x80106D6C-70 — Dossier
OFFEN 12, betrifft Groesse/Sub, nicht die Lage). Gegenstand des Fixes erklaert den Befund M1: ja (s. 1.2, Gruppe B jetzt
an der gezeichneten Matrix).

## 5. Vertrag / Tests
* Pfade (`git diff master --name-only`, ohne analysis/): engine/src enemy_ai_common.c, enemy_ai_re2_crow.c, enemy_ai_re2_zombie.c,
  re15_damage.c, schuss_r36.c (neu); include re15_actor.h, re15_schuss_r36.h (neu); tests: integration/test_r36_schuss.cmake,
  unit/probes/r36_schuss.cmake, probe_r36_blut.c, test_r36_schuss(_kraehe).c + 6 angepasste Riegel. Keine release/,
  platform/android/, shared_assets/PSX/, RELEASE_NOTES, tests/unit/CMakeLists.txt, tests/integration/CMakeLists.txt — ok.
* Nachbesserung 2 beruehrt gemeinsame Dateien nur in enemy_ai_re2_zombie.c (1 Zeile in re2z_gore_fx_core + Lesezugriff
  re15_re2z_gore_part_matrix_peek, +26/-? Zeilen; VERTRAG §1.4: C = "Blut-Anker"), sonst eigene Dateien schuss_r36.c/.h.
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine (Diff-Suche re15_game_flag_set / re15_msg_install_text / scd_event_fire leer).
* Konfliktpruefung `git merge-tree --write-tree HEAD <zweig>` gegen alle 17 r36-Zweige (zombieki 517a7dca, granate, gegnerzahl
  1e4e373d, elza, klettern cfc2a1b6, kleidung, szenenleon f8be5533, integration 6bb56610, inventar, karte, kartenitems, pri,
  schluessel, story, szene1150, tueren, ziel1190): alle rc=0, 0 CONFLICT. r36/zombieki liest re15_enemy_bone_world_pos nur
  fuer die Kopf-Sicht, keine Beruehrung des Blut-Ankers.
* Tests je Punkt vorhanden und messend: Punkt 1 integration_r36_schuss/unit_r36_schuss, Punkt 2 unit_r36_blut_pin_{eben,tief,mg,
  rip,arm,burst,glied} (trennscharf, §1.2), Punkt 3 unit_r36_schuss_kraehe. Luecke: kein Pin fuer den Kriecher-Kopfabriss (§1.3).
* Gezielter ctest-Lauf: s. 5.1.

### 5.1 ctest (gezielt im Spurbaum, keine Voll-Suite)
(laeuft)

## 6. Maengel (nummeriert, nachpruefbar)
* **M1 (Punkt 2) — Kriecher-Kopfabriss: zwei Blut-Emitter mit Welt- statt Anker-Versatz.**
  enemy_ai_re2_zombie.c:2028 `re2z_gore_fx_ex(e, 0, 0x00801000u, rot_y, 0, -600, 0)` und :2048 `re2z_gore_fx_ex(e, 8, 4096u, ang,
  0, 100, 0)`. Original (EMZ0.BIN, selbst gelesen §1.3): a3 = Zeiger auf Stapel-Vektor (0,-600,0) @0x80102AEC/0x80102B10-1C bzw.
  (0,100,0) @0x80102B8C-A8, `jal 0x8001bf10` @0x80102B24 (a2 = Part 0 +72) bzw. @0x80102BBC (a2 = Part 8 +72), Id 0 = Blut — also
  Lage = Anker.R * a3 + Anker.T wie Zensus-Gruppe A. Gemessen: Sonde 25 Faelle 936-937 neben der Original-Lage (Blut 600 senkrecht
  ueber dem Becken des liegenden Kriechers statt am Hals), Hals-Stumpf 286 Spawns 148-149; exe E2f F226
  `id=0 sub=128 ... w(-2732,-849,-19781)` + Bild `abn2C/p2_e2f_crop.png` (Blutballen in der Luft auf Kniehoehe). Der Zensus
  (Dossier §8.8 C) fuehrt beide falsch als "Griff/Biss — kein Blut am getroffenen Zombie". Nachpruefen:
  `abn2C/abn2_blut.exe 12 px eben 2` -> Zeile `-> Welt-Versatz (0,-600,0) an Part 0; ... Abstand 937`. Fix-Weg ohne neue Zahl:
  re2z_gore_fx_a3 mit den belegten Vektoren; Pin: Kriecher-Variante in probe_r36_blut (der mg-Lauf bricht heute beim Kriecher ab).

## 7. Ergebnis
(nach 5.1)
