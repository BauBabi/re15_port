# Birkin G5 (ROOM5090): Auftritt durch den Zug, Gesichts-/Koerper-Animation, Tentakel — Runde 16 (2026-09-19)

Nutzer: "Bei Birkin das mit den Tentakeln usw. ist schon ein wenig besser, aber noch weit
entfernt von gut. 1. Muss er eigentlich hinten durch das Zug Rechteck rein kommen, genauso wie
in Resident Evil 2. 2. Sind weder die Augen, noch die Zaehne noch sonst irgendwas von ihm
animiert. 3. Sind die Tentakel nicht richtig animiert, greifen nicht an usw."

Quellen (alles in dieser Runde selbst gelesen/gemessen):
* RE2-Overlays: `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN` (G5, gelinkt @0x80100000, EMS
  @0x006C0000) und `CDEMD0_EM37_ai1.BIN` (Tentakel, EMS-Sektor 0xE04 = @0x00708000);
  Volldisasm in der Sitzung (`re2_disasm.py dis 0x801000bc 5600` / `dis 0x80100178 5700`).
* RE2-EXE `info/re2leon/PSX.EXE` (Datei = Adresse − 0x80010000 + 0x800), Decompiles
  `RE2_Quellcode_V2/FUN_800197f4.c`, `FUN_80019628.c`, `FUN_80019cd0.c`, `FUN_800171d0.c`,
  `FUN_800172f8.c`, `FUN_80034d0c.c`, `FUN_80035408.c`, `FUN_80015910.c`, `FUN_800401d4.c`.
* RE2-Raum `info/re2leon/PL0/RDT/ROOM7040.RDT` (+ die schon extrahierten `room7040/scd/*.scd`),
  eigener RE2-SCD-Disassembler (Laengentabelle = `information73.txt`, Opcodes 0x8A/0x8B/0x8C ueber
  die Sprungtabelle @0x800A74C8 aufgeloest: 0x80059348 = `FUN_8003947C`, 0x80059394 =
  `FUN_80039514`, 0x800593E4).
* RE1.5-Raum `re15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT` (SCD mit der Port-Laengentabelle
  `scd_vm.c s_opcode_sizes`, RVD @0x260, RID @0x60).
* Sonde `re15_port/tests/unit/probe_r16_birkin_g5.c` (Registrierung
  `tests/unit/probes/r16_birkin-g5.cmake`, Build `re15_port/build_r16_birkin-g5`).
* Vorbefunde: `analysis/befunde_runde6_2026-09-12/birkin-g5-ki.md`,
  `befunde_runde7_2026-09-13/em037-tentakel.md`, `g5-morph.md`,
  `befunde_runde3_2026-09-12/g5-optik.md`.

---

## 0. Kurzfassung

1. **Auftritt:** Der Port setzt den Boss auf den RE1.5-Skriptwert `Pos_set(1200)` @0x12FE (fuer den
   RE1.5-Humanoiden 0x30 und dessen Nahaufnahme Cut 15 geschrieben); gemessen steht die 7,3-m-Masse
   damit ab Bild 0 mitten im Cut-12-Bild, 11,4 m vor dem Westende, ihre Front 7,7 m vor dem Spieler,
   und ueberholt den Spieler bei Bild 527. RE2 positioniert G5 NICHT per Skript, sondern das Overlay
   selbst beim Armieren ([T0] `X=-9000/Z=-23400` @0x801011d0-dc; Skript-Spawn parkt bei −32000),
   armiert per `Member_set(0x17,1)` = `+0x1D4` (Sprungtabelle @0x80011228[23] → `sh a2,468(a0)`
   @0x80055d8c), und die Masse endet nach dem Intro 12890 vom Spieler (Origin 1960, Spieler 14850).
   Der Port muss G5 beim Kampfstart selbst an das Westende stellen (Front auf der Wandebene,
   `x = -5673 - 4494 = -10167`; RE2-Distanz-Transposition liefert unabhaengig −10250) und den
   Ursprungs-Cull (main.c:7839) fuer 0x36 auf die Massenfront beziehen, sonst bleibt er dort unsichtbar.
2. **Gesicht/Koerper:** RE2 animiert an G5 pro Bild (a) Bone 1 (Kopf) aus den Clips PLUS
   Kopf-Tracking auf den Spieler (@0x801002f8-370), sichtbar nur ueber das **2-Bone-Skinning von
   Mesh 0** (`FUN_800197f4`, 186 Vertices @0x801056f0: 30 Kopf/34 Hals-Blend/122 Rumpf, 13 Arm
   @0x80105a10) — der Port zeichnet Mesh 0 starr an Bone 0, Bone 1 bewegt nur ein 1-cm-Quad;
   (b) **Augen = UV-Scroll** zweier Primitiv-Listen von Mesh 2 (Records @0x80105610/0x8010562C,
   Wanderer 0x80105064, Ziele alle 16 Bilder @0x801001b0-2b0, ±15 Texel, `FUN_800171d0/2f8`) —
   im Port nicht vorhanden; (c) den dir[0]-Morph — im Port gebaut und messbar (Δ bis 1350 im Intro).
3. **Tentakel:** gemessen 4 Treffer in 3000 Bildern bei 6,5 m Abstand; **drei von vier Armen stehen
   16 km hinter dem Boss** (Anker x≈3839/4627/4013 bei Boss x=20172), weil der Port den
   Ankermodus 1 aus sub8 ph1 (@0x80101da8) nie zuruecksetzt — RE2 tut das in sub10 ph0
   (`sb zero,537` @0x801027f4), das im Port ein Timer-Stub ist. Dazu fehlen: Per-Part-Einrollwinkel
   (+0x6C je Part, `FUN_80104C30`/`FUN_80104BA4`), Entity-RotX/RotZ im Renderer, das
   Schlauch-Skinning (3 Nahttabellen @0x801058b8/0x80105940/0x801059b8), die Kind-Kollision
   (4 Segmente r=800/600, `FUN_80034D0C`) und die Spitzensonde (@0x80102c70-d48).

---

## 1. Reproduktion / Messung

### 1.1 Sonde und Aufbau
`re15_port/tests/unit/probe_r16_birkin_g5.c` faehrt den ECHTEN Ablauf (ROOM5090.RDT → SCD-VM →
`re15_game_step` → `re15_enemy_ai_run_all` → `re15_g5_boss_tick`/`re15_g5_tentakel_tick`) mit
geladenen RE2-Baenken EM036/EM037, Spieler an der Tuer 4 (@0x10CE), Lauf nach Westen bis Kamera 12.
Build: `cmake --build re15_port/build_r16_birkin-g5 --target probe_r16_birkin_g5`; Ausgabe unten
woertlich (Lauf 2026-09-19, deterministisch, RNG-Seed 0x0badf00d).

```
Cut-12-Ankerviereck (RVD @0x04A4): (-5600,-29700) (-5475,-16975) (28100,-17100) (27900,-29700)
  Start Skript Pos_set(1200) @0x12FE   : Ursprung 1200 (im Viereck=1) Front 5694 (1) Front-Abstand Westwand=11367  nach Intro (+10960) Ursprung=12160 Front=16654
  Start Front am Westende (-5673-4494) : Ursprung -10167 (im Viereck=0) Front -5673 (0) Front-Abstand Westwand=0  nach Intro (+10960) Ursprung=793 Front=5287
EM036: 2 Bones 11 Clips 7 Meshes morph_ok=1 | EM037: 4 Bones 24 Clips 4 Meshes
KAMPF-GATE: Kamera 12 bei Bild 180, Spieler x=13399, Boss pos=(1200,-23350) grid=13 clip=1
```

### 1.2 A) Auftritt — Boss je Bild ab Kampfgate (Spieler steht bei x=13399)

| Bild | Boss x | Clip/af | Front x | Front−Spieler | Ursprung–Spieler | Morph Δmax |
|---|---|---|---|---|---|---|
| 0 | 1200 | 1/0 | 5694 | −7705 | 12199 | 3 |
| 120 | 1392 | 1/33 | 5886 | −7513 | 12007 | 247 |
| 180 | 6248 | 1/93 | 10742 | −2657 | 7151 | 694 |
| 270 | 8214 | 3/2 (Aufrichten) | 12708 | −691 | 5185 | 1350 |
| 527 | 8929 | 4/59 | 13423 | **+24 (Front ueberholt den Spieler)** | 4470 | 529 |
| 630 | 12341 | 4/162 | 16835 | +3042 | 1452 | 1214 |
| 660..1110 | 12160 | 2/0/2/0 (Intro-Rest) | 16654 | +2861 | 1633 | 409..975 |
| 1123 | 12160 | **5/0 = sub1 ZUG, Intro fertig** | 16654 | +2861 | 1633 | 887 |
| 1260 | 16460 | 5/137 | 20954 | +3081 | 1413 (Spieler auf 17873 geschoben) | 515 |

```
ERGEBNIS A: Ursprung erstmals im Cut-12-Viereck bei Bild 0, Front bei 0; Intro fertig (Clip 5) bei
1123; Front ueberholt den Spieler bei Bild 527; kleinster |dx| Ursprung-Spieler = 1336
```
Befund: (1) die Masse steht ab dem ersten Bild vollstaendig im Bild (11,4 m Luft bis zur Westwand) —
sie "kommt" nirgends "herein"; (2) das Intro schiebt sie 10960 nach Osten, die Front ueberholt bei
Bild 527 den Spielerstandort; ab Bild 660 wird der Spieler von der Kollision vor der Masse hergeschoben
(13399 → 13793 → 17873). RE2 endet das Intro mit Ursprung 1960 / Spieler 14850 (§2.1), Abstand 12890.

### 1.3 B) Animation

```
ERGEBNIS B: Morph-Puffer NULL in 0/1500 Bildern, groesste Vertex-Verschiebung im Lauf = 1350;
Bone-1-Pose (rot[0]) aenderte sich in 1195 Bildern; Clips c0=173 c1=268 c2=389 c3=200 c4=181 c5=289
Konsumenten im Port: Boss rot_x=0 rot_z=0 render_scale=0 (Kopf-Tracking/Augen-UV/2-Bone-Naht:
kein Feld, kein Konsument)
```
* Der **Morph** wird vom Modul jedes Bild geliefert (Puffer nie NULL; Δ 3→1350 waehrend Clip 1 =
  die [T2]-Kopplung, 409..1214 im Puls) — die Blob-Maschine laeuft. Ob er auf dem Bildschirm
  ankommt, haengt am Zeichner `main.c:8596` (liest `re15_g5_morph_verts`), das ist Codepfad, nicht
  hier gemessen (§5).
* **Bone 1 (Kopf)** aendert seine Pose in 1195/1500 Bildern (Rot/Trans aus den Clips) — die
  Clip-Daten animieren den Kopf. Aber Bone 1 traegt im Port nur Mesh 1 (4 Vertices, 1 Quad, 1 cm);
  Mesh 0 (Kopf+Rumpf, 199 V) haengt starr an Bone 0 (`main.c:8386` `npc_poses[nbi<npc_bones?nbi:0]`,
  `bone_mesh_index`). ⇒ **kein sichtbares Gesicht/Kiefer**.
* Keine Kopf-Nachfuehrung, keine Augen-UVs, keine Naht im Port (kein Feld, kein Konsument).

### 1.4 C) Tentakel — 3000 Bilder, Spieler jedes Bild 6500 oestlich des Bosses, HP je Bild zurueck

```
Boss-Clips in C: c0=1088 c2=696 c5=1216            (sub0 / sub3 / sub1 rotieren)
Spieler-HP-Treffer in 3000 Bildern: 4
Arm 0: Clips c0=2456 c7=94 c8=94 c23=356 | Spitze min-Abstand Spieler 13100
Arm 1: Clips c0=2542 c7=81 c8=115 c21=262 | Spitze min-Abstand Spieler 1250 | Treffer bei Clip c21=4
Arm 2: Clips c0=2432 c7=47 c8=97 c21=424 | Spitze min-Abstand Spieler 12450
Arm 3: Clips c0=2402 c7=31 c8=97 c23=470 | Spitze min-Abstand Spieler 12900
Zustaende erreicht: Peitsche(sub3)=JA Spiess(sub13)=JA Wedeln(sub10)=NEIN
GEO f 149 arm3 c23 s4096 anker=(4013,-2494,-21426) spitze=(12274,..) bossUrspr=(20172,0,-23350) spieler=(25024,-23350)
GEO f 197 arm2 c21 s4096 anker=(4627,-609,-20747)  spitze=(12888,..) bossUrspr=(20172,..)
GEO f 204 arm1 c 8 s4096 anker=(23587,-1882,-24570) spitze=(31848,..) bossUrspr=(20172,..)
GEO f 220 arm0 c 8 s4096 anker=(3839,-2313,-25319) spitze=(12100,..) bossUrspr=(20172,..)
```
Befund: Kommandos kommen an (Peitsche 7/8 und Spiess 21/23 werden gespielt, Maske 0x50..0xF0), aber
**Arme 0/2/3 sind an der Intro-Position (x≈3839..4627 = Boss-Stand bei Bild 120..150) stehen
geblieben** und schlagen 16 km hinter dem Kampf ins Leere; nur Arm 1 (der nie ausgefahrene, Modus 2)
haengt an der Masse und landet die 4 Treffer. Ursache in §3.3. Ausserdem: `rot_x`/`rot_z` der Arme
sind gesetzt (896/1088/2976/3200, −576), werden aber vom NPC-Zeichner nicht gelesen (§3.3).

---

## 2. Original-Mechanismus

### 2.1 Auftritt in RE2 room7040 — und was RE1.5 ROOM5090 vorgibt

**RE2-Skript (`ROOM7040.RDT`, SCD-Sub-Tabelle @0x1120, sub00 @0x1140, sub02 @0x138C, sub06
@0x1778):**
* sub00 @0x11AC `44 00 00 36 00 80 00 27 00 ff 00 83 00 00 00 83 00 00 00 00 00 00` = Sce_em_set
  Slot 0 Typ 0x36, Position **(−32000, 0, −32000)** = Parkplatz; Slots 1..4 Typ 0x37 ebenso.
  Kein Skript-Pos_set fuer G5 im ganzen Raum (alle `32`-Opcodes in sub06/07 gelten dem Spieler
  (Work_set kind 1) bzw. den Objekten (kind 4); die einzigen Enemy-Pos_sets sind sub06+0x2C
  `2e 03 01 | 32 00 00 ce 88 e1 ee a3` = Tentakel-Slot 1 auf **(−12800, −7800, −23570)** — 7,8 m
  UEBER dem Boden, also ueber dem Dach — und sub07 (Todesszene, G5 auf 9000/11000).
* sub02 @0x138C+0x0E: `11 00 0a 00 | 02 | 12 06 | 3e 00 0b 02 90 e8` = Do{Evt_next}
  while(Spieler.member 0x0B **X ≥ −6000**) → `03 00 18 06` Evt_chain(sub06): **das Intro startet,
  wenn der Spieler nach Westen unter X=−6000 laeuft** (RE2 Member 0x0B → `sw a2,56(a0)` = +0x38 = X,
  Sprungtabelle @0x80011228[11] → 0x80055d2c).
* sub06 (Intro-Cutscene) Zeitachse: `Cut_chg(3)` [cam3 (−14796,−1422,−24696)→(−5076,−3204,−22770),
  blickt nach OSTEN]; Spieler `Pos_set(−7500,0,−23570)`; `Evt_exec(7,sub03)` = Deckenplatte
  **Obj 2** (MD1 @0x24F0: 20 V, flache Platte x −1097..1112 / y −69..−1 / z −462..574, gesetzt in
  sub00 @0x1DA auf (−11990, **−4800**, −23470), Decke bei −5166) faellt und tumbelt (Speed_set/
  Add_speed/Add_aspeed-Choreo, 752 Byte); `Gosub sub13` (Rumble `8a 00 0d 00 01 00` = FUN_8003947C(13,1),
  Shake `8b 96 02 00 00 00` = FUN_80039514(2,150,0)); Tentakel-Slot 1 (+0x10E=1, sub15 Clip 13)
  auf (−12800,−7800,−23570) + `34 17 01 00` **Member_set(0x17,1)**; Spieler-Neck/Motion-Choreo
  (schaut hoch); `Cut_chg(4)`; Spieler auf −3800; `Cut_chg(9)` [cam9 (2700,−882,−29052)→
  (−1584,−4608,−14850): flach nach oben an die Decke]; sub09→sub11/12 (Objekte 3/4 = 5,1 m ×
  3,6 m Tafeln, MD1 @0x28A4, fliegen mit +1000/Bild an z=−16800 (ausserhalb der Suedwand) vorbei =
  Tunnel zieht am Fenster vorbei); sub04/sub05 Funken an der Decke (y −5040, x 500..3000) + Shake;
  Spieler `Plc_dest(5, →1400)` rennt nach Osten, wartet auf Flag 5:32; dann **sub06+0x162
  (`2e 03 00 | 34 17 01 00 | 34 07 00 00`, RDT-Datei 0x18DE): G5 `Member_set(0x17,1)` =
  ARMIERUNG** und Member 0x07 (= +0x10E, [7] → `sh a2,270(a0)` @0x80055d0c) = 0; BGM; Xa_on;
  Sleep 42; Spieler auf 6500 + `Cut_chg(10)` [cam10 (8946,−936,−25902)→(−1710,−3546,−21690), nach
  WESTEN]; `Cut_chg(5)` [cam5 (14220,−1854,−24606)→(3582,…)], Spieler 11340, Sleep 355;
  `Cut_chg(6)` [cam6 (21042,−882,−24390)→(11898,…)], Spieler **Pos_set(14850)**, Yaw 2048,
  `Plc_ret`, Cutscene-Flags aus, `Evt_exec(5,sub07)` (Todes-Watcher: wartet auf G5.member 0x17 & 2).
* `Member_set` 0x34: Handler 0x80055C00 → 0x80055CB0, `sltiu v0,a1,0x2c` (44 Member) und
  Sprungtabelle @0x80011228; **[23]=0x80055d8c `sh a2,468(a0)` = entity+0x1D4** — genau das Bit,
  das EM036 [T0] (`+0x1D4 & 1`) und EM037 sub15 (@0x801040F0-08) abfragen. Damit ist die Kette
  Skript → Armierung byte-belegt.
* **[T0] (0x80101164) setzt beim Armieren selbst**: `801011d0: addiu v0,zero,-9000 / sw v0,56(s0)`
  (X) und `801011d8: addiu v0,zero,-23400 / sw v0,64(s0)` (Z), `sb v0=1,6(s0)` (ph 1),
  `sh zero,344(s0)` (+0x158), Baseline `sh v0,23472(at)` → 0x80105BB0, `sb v0=3,536(a1)`
  (+0x218 = Vorkampf-Blob). Der Spawn-/Skriptwert wird ueberschrieben.
* Kamerasicht: G5 wird zuerst von **cam 10** gezeigt (18 m Abstand zum Ursprung bei Armierung, aber
  bis dahin sind ≥ 263 Bilder vergangen: 11+60+60+Xa+42 → er steht schon bei [T2]-Ende/[T3]
  ≈ X −2000, Front ≈ 2500 = 6,4 m vor der Kamera und richtet sich auf). Cuts 5 und 6 zeigen ihn
  danach beim Kriechen (Clip 4, +3946) von 14220 bzw. 21042 aus. Intro-Ende: Ursprung
  −9000+7014+3946 = **1960**, Spieler **14850** → Abstand **12890**, Massenfront (Ursprung+4494)
  8,4 m vor dem Spieler. Die Kappe des Zug-/Lunge-Vortriebs liegt absolut bei X=12000
  (@0x80100fd0-e4, @0x80104074) = 2850 vor dem Spieler-Parkwert; Hitbox A r=6000 (@0x80100534-48,
  Push-out `FUN_80034D0C`) schiebt den Spieler ab dann vor der Masse her.
* Optik "Zug-Rechteck": der Wagen ist der Korridor z −26884..−19870 (7014 quer), die Masse ist
  7296 breit (Mesh-2-Bbox z ±3648) — sie fuellt den Wagenquerschnitt wandfuellend, deshalb sieht
  man sie in cam 10/5/6 als "durch das Rechteck hereinkommende" Wand aus Fleisch.

**RE1.5-Skript (ROOM5090.RDT):** sub00 @0x124A Sce_em_set Slot 1 Typ **0x30** (RE1.5-Humanoid!)
grid 0x33 auf (−14700,0,−23350); sub01 @0x1290 `23 00 0a 00 0c 00` Cmp(work[0x0A]==12) →
sub04; sub04 @0x12FA `2e 02 01`, @0x12FE `32 00 b0 04 00 00 ca a4` **Pos_set(1200,0,−23350)**,
@0x1306 Sleep 1, @0x130A `34 0c 13 00` Member_set(0x0c,0x13) (Armierung), **@0x130E `29 0f`
Cut_chg(15)**, Plc_dest(9 = drehen) → Flag 5:32, @0x133E Sleep 30, @0x1342 `29 0c` Cut_chg(12).
RID @0x60: **Cut 15** = (46, −1236, −24302) → (5190, −2246, −20090): eine Kamera AM WESTENDE, 1,2 m
hinter dem Skriptpunkt 1200, nach Osten — die Nahaufnahme eines HUMANOIDEN bei x=1200. **Cut 12** =
(18526, −2656, −24660) → (1496, 1514, −20370): 17,3 m nach Westen; RVD @0x04A4 (from 12, to 0)
Ankerviereck x **−5600**..28100. RVD @0x04F4 (13→12) x 12249..13873 = wo der Spieler beim
Kamerawechsel steht (gemessen 13399/13699, Nutzer-Marke 13600).
Westende: SCA-Zelle [23] (g5-optik.md §5) x=−5673, das Ankerviereck endet 73 Einheiten davor.
Nichts im Raum ist ein "Zug-Rechteck"-Objekt: `nOmodel = 0` (RDT 0x02), das Rechteck ist die
Korridor-Endwand im BSS-Hintergrund von Cut 12.

### 2.2 Gesicht / Koerper — was RE2 pro Bild an G5 animiert

**(a) Skelett/Clips.** EM036-EMD (`CDEMD0.EMS` @0x6E6800): EMR dir[2] `{20,32,bones=2,kf_size=24}`,
Bone 0 (0,−4534,0) Wurzel, Bone 1 (2146,807,0) Kind = Kopf; 11 Clips, alle Frame-Woerter ohne Flags.
Selbst geparst: Bone 1 dreht in jedem Clip (Clip 0 Idle: rx 0..84, ry ±, rz 0..100 = leichtes
Nicken; Clip 1/4/5/9 ueber den ganzen Kreis). ⇒ Kiefer/Kopf-Bewegung STECKT in den Clips.

**(b) 2-Bone-Skinning von Mesh 0 = der Konsument der Kopfbewegung.** Pro-Frame-Callback
0x80103af4: `jal 0x800197f4(entity, parts, 0x801059d8)` und `(…, 0x80105a44)` (g5-optik.md §2).
Tabellenformat (aus `FUN_80019628`/`FUN_800197f4`): `+0 u8 part_a, +1 u8 part_b, +2 u16 count,
+0x14 SVECTOR off_a, +0x2C SVECTOR off_b, +0x34 ptr Paarliste {u16 vertex, u16 weight}`, dazu die
vom Init gefuellten Pufferzeiger (+4/+8/+0xC/+0x10/+0x1C/+0x20/+0x24).
* @0x801059d8: `01 00 ba 00 … 9e f7 d9 fc` = part_a=1 (Kopf), part_b=0, count=**186**,
  off_a = (−2146, −807, 0) = −Bind von Bone 1, off_b = 0, Paare @0x801056f0 (186×4 B, endet exakt
  am Tabellenanfang). **Gewichts-Histogramm (selbst gezaehlt): 0→122 V, 4096→30 V, 682/1026/1366/
  2048/2730/3414 → 34 V** (Vertex-Indizes 0..198 = ALLE Mesh-0-Vertices ausser 13).
* @0x80105a44: `01 03 0d 00` = part_a=1, part_b=3 (Arm), count=13, Paare @0x80105a10
  `(86..91, 104..108, 193, 192) × weight 0` = die 13 Restvertices haengen am ARM-Part 3.
* `FUN_80019628` (Init, @0x80103ac0/0x80103ad8): kopiert je gelistetem Vertex `v+off_a` in Puffer A,
  `v+off_b` in Puffer B, Normale in Puffer N. `FUN_800197f4` (jeden Frame): Puffer A mit
  part_a-WELTmatrix (+0x48) rotiert + Welt-Translation (+0x5C/60/64), Puffer B mit part_b-Weltmatrix;
  Ergebnis pro Vertex `w·A + (4096−w)·B` (gte_gpf12/gpl12; w==0x1000 → nur A, w==0 → nur B) wird
  **in das MD1-Vertexarray von Mesh 0 zurueckgeschrieben** (`*piVar16` = *(part0+8)[0]), Normalen
  ebenso. Der Part traegt Flag 0x4000 (Ctor @0x80100628) = Weltraum-Vertices; der Zeichner
  `FUN_80027434` (`param_3 & 0x4000` Zeile 152/157) laesst dann die Part-Matrix weg.
  ⇒ Kopf (30 V) und Hals (34 V, weich) folgen Bone 1, der Rumpf (122 V) Bone 0.

**(c) Kopf-Tracking (jeden Frame, Main @0x801002f8-370):**
```
80100300: lh   a1,118(s3)          ; a1 = Entity-Yaw (+0x76)
80100308/14: sw v0=0x800cfbf8,436(s3) ; +0x1B4 = Spieler-Struktur
80100310: jal  0x80017fdc          ; (e, yaw, &delta): Peilung Part→Spieler-Part (liest +0x1C1
                                   ;  Part-Index beider Seiten, Welt-Translation parts+92)
80100320-34: andi 0xfff / slti 2049 / addiu -4095 ; delta auf −2047..2048 wickeln
80100344/354: lhu v0,278(s0); addu; sh v0,278(s0)   ; part1+0x6A (lokaler Yaw) += delta
80100350: jal 0x8008e1f4(parts+276, parts+196)      ; Part-1-Lokalmatrix aus den Winkeln
8010036c: jal 0x80035408(e, e+56)                   ; Part-Weltpositionen
80100370: sh v0,278(s0)  (v0 = alt − delta)          ; Winkel wieder zurueck
```
Der Kopf dreht sich also jedes Bild VOLL auf den Spieler (bis ±180°), ohne dass die Entity dreht.
(Gate: `!(+0x226 & 8)` — im Tod aus.)

**(d) Augen = UV-Scroll auf Mesh 2 (der Masse).** Zwei "Wanderer"-Records @0x80105610 und
@0x8010562C (28 B: `+0 s8 v_pos, +1 s8 u_pos, +2 s8 v_ziel, +3 s8 u_ziel, +4 s16 v_vel, +6 s16
u_vel, +8 s16 v_step, +10 s16 u_step, +12 ptr Tri-Liste, +16 ptr Quad-Liste, +20 u8 Part=2,
+21 s8 +15, +22 s8 −15, +23 s8 +15, +24 s8 −15`; Bytes `02 0f f1 0f f1` @0x80105624/0x80105640).
* Ziele: `0x8010503C(idx, a1→+2, a2→+3)` mit `15 − (rng%15)` (@0x801001c4-230: `multu 0x88888889`
  = /15), Auge 0 bei `(+0x14D & 0xF) == 0`, Auge 1 bei `== 7` (@0x801001b0-2b0); Wanderer
  `0x80105064(e, idx)` beide jedes Bild (@0x801002bc/0x801002c8).
* Algorithmus 0x80105064: `step = sign(ziel − pos)` je Achse (@0x801050ac-138); Richtungswechsel
  → `vel = vel·2/3` (`mult 0x55555556` @0x8010513c-1a8); `pos == ziel && |vel| < 3` → Einrasten
  (@0x801051ac-228); `vel += step; pos += vel; pos in [−15, +15] klemmen` (@0x8010524c-2ec);
  `delta = ((pos_v − alt_v) << 16) | (pos_u − alt_u)` (@0x80105314-328) →
  `FUN_800171d0(part+4, e[+0x104]+3, delta, e[+0x105]+482, Tri-Liste)` und
  `FUN_800172f8(…, Quad-Liste)` (@0x8010533c/0x80105360).
* `FUN_800171d0`/`2f8`: Liste = `{u8 count, u8 prim_index…}`; addiert `(char)delta` auf u und
  `(char)(delta>>16)` auf v der GT3-/GT4-Pakete (+0xC/+0x18/+0x24 (+0x30), Doppelpuffer +0x28/+0x34,
  Stride 0x50/0x68) und stempelt CLUT/TPage neu. Die UVs AKKUMULIEREN im Paket (pos ist die Summe).
* Listen (selbst gelesen): Auge 0 Tris @0x801055EC `09: 03 04 7a 7b 85 96 c2 c9 cd`, Quads
  @0x801055F8 `02: da db`; Auge 1 Tris @0x801055FC `08: 00 01 02 3b 3c 61 bd be`, Quads
  @0x80105608 `04: 27 35 5b 5c` — alle in Mesh 2, UV-Cluster u 71..109 / v 200..238 (CLUT 0x7880,
  TPage 0x83), Auge 0 bei Mesh-2-Vertex x 369..1057 y −3290..−2717 z 880..1910, Auge 1 bei
  x −578..862 y −3522..−2555 z −2880..−1547 — zwei Augen links/rechts oben auf der Masse.
  Effekt: das Augen-Texel gleitet in einem 15×15-Fenster ueber die Iris-Region = "die Augen schauen".

**(e) Morph** (g5-morph.md; im Port gebaut): 4 Segmente, Konsument `FUN_8004BF90`, Gewichte durch
die Blob-Maschine — Biss w0→−4048/w3→5573 (Δ bis 2980), Devour w0→10001, Tod w2→4097 (Δ 5212).
Das ist die Maul-/"Zaehne"-Bewegung der MASSE; das Maul des Kriechers ist (a)+(b).

**(f) Rest:** Farb-Fade part0/part2 → 0x403030 nur im Tod ([T20] @0x80103680-72c); Ooze-Partikel
nur bei `(+0x226&6)==6` (tot); Wund-Sprays bei Treffern — Effekte, keine Dauer-Animation.

### 2.3 Tentakel (EM037) — was dem Port fehlt, mit Adressen

**Ankermodus +0x219 — Vollzensus aller Schreiber (Disasm, `sb …,537(`):**
| Adresse | Wert | Zustand |
|---|---|---|
| 0x8010056c | 0 | Ctor |
| 0x801008cc | 1 | Ctor, nur `+0x10E==1` (Dach-Tentakel, sub15) |
| 0x80101d78 | 2 | sub8 ph0, nur idx 1 |
| **0x80101da8** | **1** | **sub8 ph1 (Austritt) — "frei"** |
| 0x8010210c | 2 | sub9 ph0 |
| 0x80102370 | 1 | sub9 ph4 |
| **0x801027f4** | **0** | **sub10 ph0 (`sb zero,537` direkt vor `jal 0x8005bd6c` SE 0) — voller Anker** |
| 0x80102b04 | 0 | sub11 ph0 |
| 0x80103b1c | 1 | sub14 (Tod) |
| 0x80104130 | 0 | sub15 ph2 |
sub3 (Peitsche) und sub13 (Spiess) setzen den Modus NICHT — sie verlassen sich darauf, dass das Intro
ueber [T10] `0xA01` (sub10) alle vier auf Modus 0 zurueckgestellt hat.

**Per-Part-Winkelkette:** `FUN_80104C30(e, parts)` @0x80104c30: `0x8008E1F4(part+104, part+24)`
fuer Parts 0..3 (Winkel +0x68/+0x6A/+0x6C → lokale Matrix +24); `FUN_80104BA4(e, parts)`
@0x80104ba4: `0x8008E1F4(e+116, e+36)` (Entity RotX/Yaw/RotZ → Entity-Matrix), dann
`0x8002CE94(e+36, part0+24, part0+72)`, `(part0+72, part1+24, part1+72)`, `(part1+72, part2+24,
part2+72)`, `(part2+72, part3+24, part3+72)` = Weltmatrizen der Kette.
* sub9 ph5 @0x801023d4-2438: `part0.rz(+108) += +0x15A; part1.rz(+280) −= 15A; part2.rz(+452) −= 15A;
  part3.rz(+624) −= 15A>>1; 15A −= 2·(+0x16A)` (bei 0 → `+0x158=0`); dann `FUN_80104C30`;
  Gesamtwinkel-Klemme @0x80102458-4fc: `(RotZ(+0x78) + Σ part.rz) & 0xfff ∈ [2049, 4095]` →
  Schritt zuruecknehmen, `15A=0`, `16A=0`.
* sub9 ph7 @0x80102508-2664: `t<10`: part0 −2t, part1/2 +2t, part3 +t (@0x8010251c-64);
  `8≤t≤174`: Zittern X/Y/Z `±(rng&0xF)` (@0x80102574-614); `t≥166`: part0 +4, part1/2 −4, part3 −2
  (@0x80102618-58).
* sub11 ph3 @0x80102c30-44: `+0x78 −= +0x15A` (ph3: `15A −= 4` @0x80102c54-60); `FUN_80104BA4`;
  Spitzensonde @0x80102c70-cb4: `FUN_8008DBA4(part3+72, (1500,0,0), out)` + part3-Welt-Y/Z
  (parts+612/616); Grenzen `tipY > 0` (@0x80102cb8), `< −5000` (@0x80102cdc), `tipZ < −27000`
  (@0x80102d04), `≥ −19799` (@0x80102d2c) → `+0x06 = 4` (nur wenn `+0x16A==0`).
  GRIFF-Fenster @0x80102d4c-dec: `+0x220 & 2` und `0x800CFDCB == 0` → `FUN_800401d4(15,1)`,
  `0x800CFBFC = (!FACING<<8)+5` (`FUN_80015910`: `((yaw_spieler − yaw_e + 0x400) & 0xfff) < 0x800`),
  Griff-Globals, `FUN_8003947C(3,0)`, `FUN_80039514(5,250,0)`.
* sub11 ph6 @0x80102f9c-3004: `+0x220 |= 1`; part-Flags `(~0x400)|0x2000`, part1 `&~0x800`;
  **`scaleX(+0x8C) = (+0x21C · (((+0x21A − X) << 8) >> 13)) >> 8`** = 21C·(21A−X)/8192;
  ph7 @0x80103010-31c8: dieselbe Einroll-/Zitter-/Ausroll-Folge wie sub9 ph7 mit Fenstern `t<10`,
  `6≤t≤79`, `t≥81`, `t==90 → ph 8`, danach jedes Bild die Laengenformel; ph9 @0x80103210-88:
  `+0x78 −= 1; scaleX += 128` bis `+0x21C`, dann Flags `(|0x400)&~0x2000`, part1 `|0x800`, ph 10,
  `+0x220 &= ~1`, SE 0.
* **Schlauch-Skinning (Callback 0x801004B4 → 0x80104CF0, wenn part0-Flag Bit 0):**
  `FUN_800197f4(e, parts, 0x801058b8)`, `(…, 0x80105940)`, `(…, 0x801059b8)`, dann
  `FUN_80019CD0(e, parts)`. Tabellen: @0x801058b8 `{0,1,30, off_b=(−3750,0,0), Paare @0x80105840}`,
  @0x80105940 `{1,2,20, off_a=(−3750,0,0), off_b=(−6753,0,0), Paare @0x801058f0}`,
  @0x801059b8 `{2,3,16, Paare @0x80105978}`. Paare (selbst gelesen): Ringe von je 5 Vertices
  mit Gewichten 4096/3296/2496/1600/800/0 (Segment 0→1), 3072/2048/1024/0 (1→2 und 2→3) —
  **alle 66 Vertices des Strangs sind gewichtet zwischen benachbarten Bones**, der Schlauch biegt
  sich weich entlang der Kette.
* **scaleX** wirkt NICHT auf die Kette: `FUN_80019CD0` (Flag 0x400 des Parts) baut
  `Welt · S(scale) · Weltᵀ · T(−t)` (Zeilen 33-55 des Decompiles) — eine Skalierung UM DEN
  PART-0-RAHMEN, die auf die Weltraum-Vertices des Schlauchs wirkt (der Zeichner `FUN_80027434`
  nimmt bei 0x400 `ScaleMatrix` Z. 123-139). Deshalb bleibt die Spitzensonde (part3-Welt) bei jeder
  Streckung an derselben Stelle — die Zustaende pruefen sie nur bei scaleX 4096.
* **Kind-Entity (+0x10E=8) = die Kollision.** Ctor @0x80100674-c4: `+0x1E8 = 4` (Segmente),
  Segment 0 Radius/Hoehe **800** (`sh 800 → +0x9A/+0x9C/+0x90/+0x92`), Segmente 1..3 **600**
  (`+0xBA/BC/B0/B2`, `+0xDA/DC/D0/D2`, `+0xFA/FC/F0/F2`). Main @0x801001a0-c8: `+0x10E == 8` →
  `jal 0x80104f64`, `e->flags &= ~1` (nicht zeichnen). 0x80104F64: Elternteil = `*(0x800CFE10 +
  idx·4)` (= Slot-Liste[idx−3]); Segment 0 (+0x84) = Eltern-**part3-Welt** (parts+608/612/616),
  Segment 1 (+0xA4) = part2 (436..444), Segment 2 (+0xC4) = part1 (264..272), Segment 3 (+0xE4)
  = Mitte(part1,part2) (@0x80104ffc-5074); `+0x0D = 255`; **`Eltern[+0x220] &= ~6; r =
  FUN_80034D0C(kind, 0x800CFBF8); Eltern[+0x220] |= 6·r`** (@0x80105078-a4).
  `FUN_80034D0C` (Decompile): fuer jedes Segmentpaar Kreis-Test `dist < r_a + r_b` (SquareRoot0)
  und Hoehen-Ueberlappung `|dy| < h_a + h_b`, Spieler wird herausgeschoben, Rueckgabe 1.
  Verbraucher: Peitsche `+0x220 & 4` @0x80101684-94, Spiess `+0x220 & 4` @0x801036f4-fc,
  Zug-Griff `+0x220 & 2` @0x80102d4c.

---

## 3. Port-Ist (Datei:Zeile) und warum falsch

### 3.1 Auftritt
* `enemy_ai_boss_g5.c:630-655` (Kampfstart bei grid 0x13): **keine Positionszuweisung** — "es gilt,
  was das SKRIPT gesetzt hat" (Pos_set 1200). Das ist RE1.5-skripttreu fuer den Humanoiden, aber die
  RE2-Masse ist damit ab Bild 0 mitten im Bild (§1.2). RE2 ueberschreibt den Skriptwert in [T0].
* `enemy_ai_boss_g5.c:654`: Yaw einmalig auf den Spieler; `:495-500` Intro-Root-Motion ohne Kappe
  → die Front ueberholt den Spieler (Bild 527) und der Kampf beginnt mit dem Spieler IN der Masse
  (Ursprung 1633 statt RE2 12890 vom Spieler).
* `enemy_ai_boss_g5.c:209` `u = 12000 − |dx|`: die RE2-Kappe X=12000 (2850 vor dem Spieler-Park
  14850) wird auf `|dx| = 0` abgebildet.
* `platform/pc/main.c:7839-7841`: Gegner-Cull ueber den URSPRUNG im Cut-Viereck — bei einem Start
  am Westende (x < −5600) waere G5 unsichtbar, obwohl seine Front 4,5 m im Raum steht (Sonde:
  "Ursprung −10167 im Viereck=0").
* Cut 15 (@0x130E) bleibt im Skript: Kamera bei x=46 nach Osten; mit der Masse am Westende ist sie
  HINTER der Kamera (nicht im Bild), der Cut zeigt den Spieler aus Boss-Sicht — unkritisch.

### 3.2 Gesicht/Koerper
* `platform/pc/main.c:8386` `np = &npc_poses[nbi < npc_bones ? nbi : 0]` + `bone_mesh_index`:
  Mesh 0 (199 V) starr an Bone 0, Mesh 1 (4 V) an Bone 1 → das ganze Gesicht/der Kiefer steht.
  Kein `FUN_800197f4`-Gegenstueck (keine Paarlisten, keine Weltraum-Vertexpuffer).
* Kein Kopf-Tracking (grep `Kopf|track|head` in enemy_ai_boss_g5.c: nur der Kommentar).
* Kein Augen-UV-Zustand, kein UV-Offset-Pfad im Zeichner (`main.c`/`render_pc.c`: kein
  `uv_shift`-Feld; `re15_md1_tri_uv_t` wird direkt aus dem residenten EMS gelesen).
* Morph: `enemy_ai_boss_g5.c:180-200` + `emd_morph.c` + `main.c:8596` vorhanden; Gewichte laufen
  (§1.3). Nicht gemessen: ob der GL-Pfad die ersetzten Vertices auch fuer die Quads nimmt (`nmq = mv`
  gesetzt, Codepfad ok).

### 3.3 Tentakel
* `enemy_ai_tentakel_g5.c:267` sub8 ph1 → `ankermodus = 1`; `:439-443` **default (sub 9/10/12):
  "ueber Timer laufen lassen"** ohne `ankermodus = 0` → nach dem Intro bleiben Arme 0/2/3 "frei"
  und `tent_anker` (`:167-170` `if (ankermodus == 1) return;`) fuehrt sie nie nach: gemessen 16 km
  hinter dem Boss (§1.4). Arm 1 (Modus 2 aus `:264`) folgt.
* `:201-209 tent_spitze`: Spitze = Anker + 8261·scale entlang **rot_y** — RotX/RotZ/Per-Part-
  Winkel unberuecksichtigt; `:221-228 tent_trifft`: nur die Spitze, r=1500, statt 4 Segmente
  r=800/600 (§2.3). Ein Spieler zwischen Anker und Spitze wird nie getroffen.
* `:311` `e->rot_x = …`, `:452` `e->rot_z = t->rot_z`: gesetzt, aber `main.c:8250-8256` baut fuer
  NPCs nur `nyaw` aus `rot_y` — RotX/RotZ werden nicht gezeichnet; kein Per-Part-Winkel (+0x6C),
  kein Skinning (Mesh 0 starr an Bone 0, Meshes 1..3 = Stummel), Streckung = uniformer
  `render_scale_q12` (`:450`, dick UND lang) statt Skalierung entlang Part-0-X.
* `:402-415` sub11 (Zug): nur Ausfahren/Clip-Ende, ohne ph3 (Griff), ph5/6/7 (Laengenformel,
  Einrollen/Zittern), ph9/10; `:439` sub9 (Speer) und sub10 (Wedeln) als Timer-Stub; keine
  Spitzensonde/Raumgrenzen; kein `+0x220`.

---

## 4. Fix-Plan (Phase 2)

Reihenfolge nach Sichtbarkeit fuer den Nutzer; jede Konstante mit Beleg.

### 4.1 Auftritt (enemy_ai_boss_g5.c, main.c)
1. **Selbstplatzierung beim Kampfstart** (wie [T0] @0x801011d0-dc, das den Skript-/Spawnwert
   ueberschreibt): in `re15_g5_boss_tick` beim Setzen von `g->gestartet` (`:630`)
   `e->x = RE15_G5_X0; e->z = -23400` (Z @0x801011d8; Streifen-C-Mittellinie −23377, Skript −23350).
   **X0 = −10167** = Westwand −5673 (SCA-Zelle [23], g5-optik.md §5) − 4494 (Blob-Bind 1800
   @0x80100680 + Mesh-2-Bbox x_max 2694, g5-morph.md §2.3): die Massenfront steht exakt in der
   Wandebene = im "Zug-Rechteck" von Cut 12. Gegenprobe ohne Geometrie: RE2-Intro-Endabstand 12890
   (§2.1) auf die 5090-Spielermarke 13600 (RVD @0x04F4) transponiert und 10960 Intro-Root-Motion
   zurueckgerechnet ergibt −10250 (83 Einheiten Differenz). Dokumentierte Port-Entscheidung:
   RE1.5s `Pos_set(1200)` @0x12FE galt dem Humanoiden 0x30 und dessen Nahaufnahme Cut 15.
   Yaw: RE2 laesst Yaw 0 (+X); der Port rastet einmalig auf den Spieler (`:654`) — beibehalten
   (der Spieler steht oestlich, Ergebnis ≈ 0).
2. **Cull auf die Massenfront** (`main.c:7839`): fuer `type == 0x36` den Punkt
   `(x + 4494·cos(yaw), z − 4494·sin(yaw))` statt des Ursprungs gegen das Cut-Viereck testen
   (Port-Entscheidung; RE2 kennt keinen Ursprungs-Cull fuer G5, der Wert 4494 s.o.). Alternative
   (schlechter): Start bei −5600 → die Front stuende 4,5 m im Raum (Sonde) und "poppt".
3. **Kappe**: RE2 setzt Zug/Lunge bei X=12000 aus (@0x80100fd0-e4 / @0x80104074), das sind 2850
   VOR der Spieler-Parkposition 14850 (sub06 @0x01F4); Port `u = 12000 − |dx|` (`:209`) klemmt bei
   |dx|=0. Auf `u = 12000 − (|dx| − 2850)` umstellen, damit "12000" wieder 2850 vor dem Spieler
   liegt; das Devour-Gate `u ≥ 9001` (@0x801008a0) wird damit `|dx| ≤ 5849` (RE2: 14850−9001)
   statt 2999. Risiko: Balance der Rueckzugs-/Zug-Entscheidungen (7000/8000/10000/10001/11001)
   verschiebt sich konsistent mit — das ist gewollt, sie sind in RE2 absolute X-Werte relativ zum
   festen Spielerpark.
4. Kein Eingriff in Cut 15/12 (RDT-Skript bleibt); die RE2-Cuts 3/4/9/10/5/6 sind
   room7040-spezifisch (Dach-Tentakel, Deckenplatte Obj 2, Tunnel-Tafeln) und ohne 5090-Gegenstueck
   → nicht portierbar, dokumentieren.
5. Pin: `probe_r16_birkin_g5` Teil A als ctest verschaerfen: Front bei Kampfstart innerhalb ±100 der
   Westwand (−5673), Front-in-Viereck-Test des neuen Culls, Intro-Ende Ursprung ≈ 793 (< Spieler −
   10000), "Front ueberholt den Spieler" = nie.

### 4.2 Gesicht/Koerper (neu: `re15_g5_skin.c` o.ae., Zeichner main.c)
1. **2-Bone-Skinning von Mesh 0** nach `FUN_800197f4`/`FUN_80019628`: Tabellen als Daten aus dem
   Overlay lesen (`CDEMD0.EMS` @0x6C0000 + (0x801059d8/0x80105a44 − 0x80100000), Paarlisten
   @0x801056f0 (186) / @0x80105a10 (13)) oder einmalig extrahieren; pro Bild
   `v' = (w·(R1·(v+off_a)+t1) + (4096−w)·(R0·v+t0)) >> 12` mit `off_a = (−2146,−807,0)` und den
   Welt-Posen von Bone 1/0 (die `npc_poses` des Zeichners), Ergebnis in einen Weltraum-Vertexpuffer
   fuer Mesh 0 (wie der Morph-Puffer fuer Mesh 2: Zeigertausch `nmv/nmq` @main.c:8596, Part ohne
   Matrix = Flag 0x4000). Die 13 Arm-Vertices (Gewicht 0, part_b=3) an die Arm-Pose (Mesh 3 =
   Bone 0 ⊕ (800,500,0) @0x801006ac-c4).
2. **Kopf-Tracking**: jedes Bild (ausser Tod, `+0x226&8`) `delta = wrap(±2048)(atan2(Spieler −
   Kopf-Part) − yaw)` (@0x80100318-34) auf den lokalen Yaw von Bone 1 addieren, NUR fuer die Pose
   dieses Bildes (das Original addiert vor und subtrahiert nach der Matrix @0x80100354/0x80100370).
   Offen: das Innere von 0x80017FDC (welcher Part-Punkt als Ziel/Quelle) — s. §5.
3. **Augen-UV**: zwei Wanderer-Zustaende (Records §2.2d) im G5-Modul, Ziel-Neuwurf bei
   `(anim_frame & 0xF) == 0` bzw. `== 7` mit `15 − rng%15`, Schritt/Daempfung/Klemme wie
   0x80105064; UV-Offset (u,v) je Auge als Modul-Ausgabe; im Zeichner fuer Mesh 2 die
   Primitive der vier Listen (Indizes §2.2d) mit `u += pos_u, v += pos_v` zeichnen (Port-Feld
   pro Tri/Quad statt Paket-Patch). Gate: Todesphase setzt Ziele (0,0,0)/(1,0,0) (@[T18]).
4. Morph: kein Aenderungsbedarf laut Messung; beim Umbau des Zeichners sicherstellen, dass die
   Augen-Prims die gemorphten Mesh-2-Vertices nutzen (sie tun es, gleiches Array).
5. Pin: Test, der fuer Clip 0 Frame 50 die Kopf-Vertices (Gewicht 4096, z.B. Index 43/44) an der
   Bone-1-Pose und Rumpf-Vertices (Gewicht 0) an Bone 0 verlangt; Augen-Offset bleibt in [−15,15]
   und aendert sich in ≥ 1 von 16 Bildern.

### 4.3 Tentakel (enemy_ai_tentakel_g5.c, main.c)
1. **Sofort (Nutzer-sichtbar, kleinste Aenderung):** sub10 (0xA01) als echten Zustand mit
   `ankermodus = 0` in ph0 (@0x801027f4) implementieren — damit haengen nach dem Intro alle vier
   Arme wieder an der Masse (Sonde: Anker 3839..4627 → Boss ±3000). Ebenso sub11 ph0
   `ankermodus = 0` (@0x80102b04) und sub9 ph0 `= 2` (@0x8010210c) / ph4 `= 1` (@0x80102370).
2. **Kollision wie das Kind**: 4 Punkte je Arm (Bone 3 = Spitze r 800, Bone 2, Bone 1, Mitte(1,2)
   r 600; @0x80100674-c4) gegen den Spieler-Zylinder (Kreis + Hoehe wie `FUN_80034D0C`), Ergebnis
   als `+0x220`-Bits 2|4 je Bild (@0x80105078-a4); Peitsche/Spiess/Zug-Griff lesen die Bits
   (@0x80101684, @0x801036f4, @0x80102d4c). Ersetzt `tent_trifft` (r 1500 nur Spitze).
3. **Winkelkette + Rendering**: pro Arm Entity-RotX/Yaw/RotZ (+0x74/76/78) UND Per-Part-RotZ
   (+0x6C) fuehren; Weltmatrizen wie `FUN_80104BA4` (Entity × lokal, Kette); Zustandslogik
   sub9 ph5/ph7 und sub11 ph3/5/7/9 mit den Raten aus §2.3 (`+0x15A` Start 64, `−= 2·16A`, Klemme
   [2049,4095], Ausrollen −2t/+2t/+t, +4/−4/−4/−2, Fenster t<10 / 8..174 / ≥166 bzw. 6..79 / ≥81 /
   ==90). Der NPC-Zeichner muss fuer 0x37 die volle Euler-Matrix (rot_x/rot_y/rot_z) statt nur
   `nyaw` nehmen (`main.c:8250`) und die Bone-Posen aus Keyframe ⊕ Per-Part-Winkel bauen.
4. **Schlauch-Skinning + Streckung**: die drei Nahttabellen (§2.3) auf Mesh 0 anwenden (66 V,
   Ringgewichte), danach Skalierung um den Part-0-Rahmen entlang lokal X mit `scaleX/4096`
   (`FUN_80019CD0` Flag 0x400) statt `render_scale_q12` uniform; Y/Z bleiben 4096.
5. **Spitzensonde + Raumgrenzen** (sub9 ph3 / sub11 ph3): `Spitze = W3·(1500,0,0) + t3`,
   Abbruch bei `Y > 0`, `Y < −5000`, `Z < −27000`, `Z ≥ −19799` (@0x80102cb8-d2c) — Streifen C
   liegt bei z −29765..−16864 (gleicher Wagenquerschnitt), die Konstanten passen ohne Umrechnung.
6. **Zug-Griff** (sub11 ph3) und Peitschen-/Spiess-Griff: Schaden 15 (`FUN_800401d4(15,1)`), Griff-
   Modus 5/0x105/0x205/0x305/0x405 setzt RE2-Spieler-Opfer-Anims (EDD-Paar 3, 15-Bone-PL0-Rig) —
   Schaden + Grab-Latch portieren, Opfer-Anim bleibt DEFERRED (wie Devour).
7. Pin: Sonde Teil C als ctest — nach dem Intro alle vier Anker innerhalb 6000 vom Boss; bei
   Spieler 6500 oestlich ≥ 1 Treffer pro 300 Bilder; Spitzen-Minimalabstand < 1500 fuer alle vier.

### 4.4 Risiken
* Zeichner-Umbau (Weltraum-Vertexpuffer, Euler-NPCs) beruehrt den gemeinsamen NPC-Pfad — nur fuer
  0x36/0x37 gaten. * Push-out/Kappe-Aenderung (4.1.3) veraendert die Kampfdistanz — mit der Sonde
  (Teil A, Abstand nach Intro ≈ 12890) gegenpruefen. * Cull-Aenderung darf keinen anderen Typ
  betreffen.

---

## 5. Offen / nicht belegt
* Inneres von `FUN_80017FDC` (Kopf-Peilung): nur der Rahmen ist gelesen (liest `+0x1C0` Bit 0,
  `+0x1B4` Zielstruktur, Part-Indizes `+0x1C1` beider Seiten, Welt-Translation `parts+92`);
  welche Punkte genau angepeilt werden und ob eine Distanzgrenze gilt, ist nicht disassembliert.
* Sichtbarkeit des Morphs im GL-Fenster ist nicht per gdigrab verifiziert (nur die Modul-Ausgabe
  gemessen).
* EM036-Bone-1-Trans (2146,−3727) in der Sonde stammt aus `re15_skel_compute_pose`; ob die Port-Pose
  die Keyframe-Root-Translation identisch zum Original (`FUN_80029614`) fuehrt, wurde nicht
  gegen Savestate-RAM geprueft.
* RE2-Eltern-Part-Feld `+0x1C1` = 3 (Ctor @0x80100554-84 "Kollisionssegmente") vs. Kind `+0x1E8=4`:
  die Rolle von `+0x1C1` bei `FUN_80034D0C` (param_1[0x7a] = +0x1E8) ist die Segmentzahl des
  Kindes; das Elternfeld bleibt ungeklaert.
* `FUN_8003947C`/`FUN_80039514`/0x800593E4 (Rumble/Shake-Queues) — Semantik der Argumente nicht
  disassembliert; die Werte sind zitiert.
* Die Cut-15-Wirkung (Spieler aus Boss-Sicht) ist Schluss aus den RID-Werten, nicht per Bild
  verifiziert.
* RE2-Deckenplatte/Dach-Tentakel/Tunnel-Tafeln (Obj 2/3/4, sub03/sub11/sub12) sind bewusst nicht
  Teil des Plans (kein 5090-Gegenstueck).
