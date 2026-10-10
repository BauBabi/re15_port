# Runde 36 — Spur C "schuss" (2026-10-10)

Baum: `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, Basis master 5a9dbb7b (v0.8.23 + Auftrag).

## 0. Auftrag (Wortlaut, AUFTRAG.md / VERTRAG.md Spur C)

1. Grenze 1 (v0.8.22/23, vom Nutzer bestellt): "Alle Schusswaffen treffen ueber den Konferenztisch und treffen
   fressende Zombies. Im Original geht beides nicht." -> beheben: Sichtlinie (RE1.5 FUN_80011f50 -> FUN_8001b9b4 ->
   FUN_8003dcc4) + Fresser-Sperre (RE2 +0x1D3 / +0x10E im Resolver FUN_800410CC @0x80041270-9c) fuer ALLE Schusswaffen.
2. "Bei den Zombies habe ich manchmal das Gefuehl, dass das Blut nicht an der richtigen Stelle auftaucht, wenn sie
   getroffen werden." -> Original-Mechanismus des Trefferpunkts belegen, Port angleichen, Pin mit Lage-Differenz.
3. "Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in Resident Evil 2 auch so?" ->
   RE2-Kraehen-Handler (ENEMY07/EM_TYPE?) auf die Magnum-Todesreaktion disassemblieren, mit dem Port vergleichen,
   Antwort ins Dossier, Abweichung beheben.

Vorrunde: `analysis/befunde_runde35/B_werfer.md` §8.1/§8.4/§8.7 OFFEN 16/17 (Belege fuer Punkt 1 bereits
disassembliert, dort zitiert — hier selbst nachgeprueft, s. §1.2).

## 1. Punkt 1 — Sichtlinie + Fresser-Sperre fuer alle Schusswaffen

### 1.1 Messung vorher (exe-Stand master 5a9dbb7b, eigener Bau, `mess_r36c/run_p1.sh vorher 20`)
Harness = r35 (`test_r35_werfer.cmake`): ROOM1140 per `RE15_DEBUG_JUMP=1140@250`, RE2-KI, Python (20) mit
6 Schuss, Eingabe `M0.6,MA1.5,M1.0,W4`, `RE15_STATE_LOG`, `RE15_EXIT_AT=200#1140`; Messkopie `re15_r36c.exe`
neben der exe. Plaetze (state.log F6): 1 (-800,-20600) ss1=7 Liegender hp 79; 2 (-1800,-19600) ss1=8 Fresser
hp 50; 3 (-1800,-21600) Fresser hp 80; 4 (200,-21600) Fresser hp 250; 5 (200,-19600) Fresser hp 250.
* **Tisch** — Leon (200,-10300) Blick -z (rot 1024); Konferenztisch = SCA-Zelle 6 {x -4650..7450,
  z -17450..-11350, Typ 1, Wort+10 0x0300} liegt auf der Linie x=200 zwischen Leon und den Plaetzen 4/5:
  `F19 Platz 5 250 -> -1` (d=9272) und `F44 Platz 4 250 -> -1` (d=11264) — ZWEI Fresser HINTER dem Tisch
  getoetet (beide Defekte in einem Lauf).
* **Fresser** — Leon am Tuerpunkt (-7600,-17600) Blick +x (rot 0), alle Fresser > 4000 entfernt (kein Wecken):
  Platz 2 hp 50 -> -1 (d 6122) und Platz 5 hp 250 -> -1 (d ~8050) — FRESSENDE Zombies getoetet.
Logs: `mess_r36c/vorher/{tisch,fresser}/state.log` (unversioniert, Zahlen hier zitiert).

### 1.2 RE-Belege (selbst disassembliert, `re15_disasm.py` / `re2_disasm.py`)
**(a) RE1.5 Sichtlinie — FUN_80011f50 (Spieler-Schuss-Resolver, alle 22 Waffen via Tester-Tabelle @0x8006e548):**
```
80012158: bne a0,zero,0x800120d0      ; Kandidatenschleife (naechster Platz)
80012160: beq s1,zero,0x8001253c      ; kein Gewinner -> return 0
80012164: addiu s0,s1,52              ; s0 = Gewinner + 0x34 (Lage x/y/z)
80012168: jal 0x8001b9b4              ; SICHTLINIE(Lage des Gewinners)
8001216c: addu a0,s0,zero
80012170: bne v0,zero,0x80012540      ; blockiert -> return 0 (KEIN Rueckgriff auf den 2. Kandidaten)
80012174: addu v0,zero,zero
80012178: lbu v0,8(s1) / 80012180: sltiu v0,v0,0x40 / 80012184: beq -> 0x80012540   ; Typ >= 0x40 -> 0
```
Reihenfolge: Schleife -> Sichtlinie -> Typ-Pruefung -> Hoehen-/Zielpunkt-Block (0x8001218c..) -> LAB_80012370
Kritklasse -> `+0x93 & 1` Latch/Rekursion @0x800123fc-0x80012418. Die Sichtlinie steht also VOR dem Latch:
ein geblockter Gewinner beendet den Schuss ohne zweiten Kandidaten.
**FUN_8001b9b4(a0 = Zielvektor):** `sw zero,436(cur)` (@0x8001b9cc, cur = DAT_800ac784 +0x1b4 := 0);
delta = Ziel - cur(+0x34/+0x38/+0x3c) @0x8001b9d8-ba10; Schleife s0 = 3..0: `ori a2,zero,0xf00` /
`jal 0x8003dcc4` / `ori a3,zero,0x300` @0x8001ba1c-24, `or s1,s1,v0` @0x8001ba28; Rueckgabe s1 & 0xff.
DAT_800ac784 = die aktuelle Entity = beim Spielerschuss der SPIELER (FUN_80031c44 setzt DAT_800ac784 = player,
RE15_FUN_CATALOG.md Z. 34; FUN_80011f50 wird aus den Spieler-Zustaenden @0x80033554/880/96c/a34/b10/c50 gerufen).
**FUN_8003dcc4(delta, region, 0xf00, 0x300):** Ursprung = cur +0x34/+0x3c (`lw t2,52(v0)` @0x8003dd00 /
`lw t0,60(v0)` @0x8003dd10), alle Koordinaten /0x12 (mult 0x38e38e39, mfhi, sra 2, -sign); Region =
`*(DAT_800ac778 +0x20) + 8 + region*4` (@0x8003dd34-44, Start/Ende-Zeiger, `sltu fp,v0` @0x8003dd94 leer ->
0); Zellfilter `lhu a0,0(s7)` (Wort+10) / `lbu v1,130(cur)` (+0x82 = BAND DES SPIELERS) / `sll v0,a0,16` /
`sra v0,v0,28` / `bne v1,v0,0x8003e0a8` @0x8003de5c-6c und `and v1,t5,a0` (0xf00) / `bne v1,v0` (0x300)
@0x8003de7c-94; danach zwei Diagonal-Straddle-Tests (OuterProduct0 `jal 0x800663ac`) — KEINE y-Komponente;
Treffer: `ori v0,zero,0x1` / `sw fp,436(v1)` @0x8003e09c-a4 (cur +0x1b4 = Zelle, sofortiger Ausstieg);
sonst `addiu fp,fp,12` / `sltu` / `bne -> 0x8003ddc8` @0x8003e0b4-bc, Ende `addu v0,zero,zero` @0x8003e0c4.
Zelle 6 von ROOM1140 traegt `01 ff 00 03` = Band 0 / Klasse 3 (B_werfer §8.1, RDT @0x5d0) -> sperrt.
Port: `re15_los_ray_blocked` (enemy_ai_common.c, byte-true FUN_8003dcc4, bisher nur Gegner-Sensor/Kraehe).

**(b) RE2-Fresser-Sperre — FUN_800410CC (Schuss-Resolver aller Schusswaffen, `jal 0x800410CC` @0x80043AFC),
Kandidatenschleife selbst disassembliert (info/re2leon/PSX.EXE):**
```
80041254: lw s2,0(fp)                 ; Kandidat
8004125c: lw v0,0(s2) / 80041264: andi v0,v0,0x1 / 80041268: beq v0,zero,0x80041300   ; Gate 1 aktiv
80041270: lbu v0,467(s2)              ; +0x1D3
80041278: bne v0,zero,0x80041300      ; Gate 2: +0x1D3 != 0 -> naechster Kandidat
80041280: lh v0,342(s2) / 80041288: bltz v0,0x80041300                               ; Gate 3 HP < 0
80041290: lhu v0,270(s2) / 80041298: andi v0,v0,0xc000 / 8004129c: bne v0,zero,0x80041300  ; Gate 4 +0x10E
```
Fresser: EXEC[8] P0 `lbu v1,467(s1)` / `ori v1,v1,0x80` / `sb v1,467(s1)` @0x80103c04-14 (EMZ0.BIN) setzt
+0x1D3 Bit 0x80, Spawn-Remap `sh 0x4004,270` @0x80100A88-8C setzt +0x10E; geloescht im Aufsteher EXEC[8] P3
`andi 0x7f` @0x80103CE4-FC und im Wecker `andi 0xbfff`/`sh 270` @0x80104F0C-10 (B_werfer §8.4, Port
enemy_ai_re2_zombie.c re2z_exec_feeding). => In RE2 ist ein fressender Zombie fuer KEINE Schusswaffe ein Ziel.
Port-Stand: `re15_re2z_hit_filter_apply` (enemy_ai_re2_zombie.c:8987-8990) fuehrt die vier Gates, nimmt aber
die SPAWN-POSE (`spawn_pose` = EXEC[7]/EXEC[8] ohne passives 0x87..0x8A) von Gate 2 und 4 AUS — eine
Nutzer-Entscheidung von v0.3.5 ("Im Dining Room trifft man den am Boden fressenden Zombie nicht"), die der
Nutzer mit dem Runde-36-Auftrag fuer die FRESSER aufhebt ("Im Original geht beides nicht -> angehen").

### 1.3 Umsetzung
* NEU `re15_port/engine/src/schuss_r36.c` + `include/re15_schuss_r36.h`: `re15_schuss_r36_sichtlinie_blockiert(pl,
  ziel)` = FUN_8001b9b4-Zwilling: Regionen 3..0 (`ori s0,zero,0x3` @0x8001ba08, Schleife @0x8001ba34-38) je
  `re15_los_ray_blocked(pl, ziel, region, 0)` (= FUN_8003dcc4 mit 0xf00/0x300, Ursprung + Band des SPIELERS);
  ohne SCA (g_room_rdt_ok == 0) 0 (leere Region `sltu fp,v0` @0x8003dd94). Mess-Zaehler `verworfen`.
* Haken `re15_damage.c` re15_player_weapon_fire, direkt nach `if (best < 0) return 0;` (= @0x80012160):
  `if (re15_schuss_r36_sichtlinie_blockiert(pl, &g_actors[best])) return 0;` (= `jal 0x8001b9b4` /
  `bne v0,zero,0x80012540` @0x80012168-74) — VOR Kritklasse und +0x93-Latch/Rekursion, fuer ALLE 22 Waffen
  (auch Messer/Rohr: @0x80012168 liegt hinter der Schleife, die Tester-Tabelle @0x8006e548 aendert daran nichts).
* Haken `enemy_ai_common.c`: `re15_los_ray_blocked` nicht mehr `static` (Kommentar "Runde 36 Spur C").
* `enemy_ai_re2_zombie.c` re15_re2z_hit_filter_apply: `spawn_pose = in_pose && !passive_lyer && sub_state_1 != 8`
  — die v0.3.5-Ausnahme gilt nicht mehr fuer EXEC[8] (Fresser); Gate 2 (+0x1D3 @0x80041270-78) und Gate 4
  (+0x10E & 0xC000 @0x80041290-9c) sperren ihn, bis P3 (`andi 0x7f` @0x80103CE4-FC) / der Wecker (`andi 0xbfff`
  @0x80104F0C-10) sie oeffnen. Der LIEGENDE EXEC[7] (nicht passiv) behaelt die Ausnahme (nicht Teil des Auftrags).
* Port-Semantik, die bleibt (dokumentiert): ein gesperrter Fresser traegt +0x93 Bit 0 und wird wie in RE1.5 als
  Gewinner gewaehlt, dann ueber `|= 2` / Rueckgriff ausgeschlossen (re15_damage.c-Block "WARUM NICHT IN DER
  SCHLEIFE"). Die Sichtlinie steht byte-true VOR diesem Latch: ist der naechste Kandidat ein gesperrter Fresser
  HINTER einer Klasse-3-Zelle, endet der Schuss ohne 2. Kandidaten (RE1.5-Verhalten fuer +0x93-Kandidaten;
  RE2 haette ihn schon in der Schleife uebersprungen). Praktisch nur, wenn ein Fresser naeher als jeder wache
  Zombie UND hinter Moebeln liegt.
* Byte-true-Eigenheit des Zellentests (gemessen am Messer, Test 20/21): FUN_8003dcc4 prueft nur, ob die Strecke
  eine der beiden ZELLDIAGONALEN kreuzt (@0x8003df28/@0x8003df94, @0x8003e02c/@0x8003e08c). Eine Strecke, die nur
  150 in die Zelle ragt (Messer 1500 Reichweite an der Tischkante), kreuzt keine Diagonale -> frei; bis hinter
  die Nebendiagonale (z -14500) -> blockiert.

### 1.4 Messung nachher
* Unit `unit_r36_schuss` (test_r36_schuss.c, echte ROOM1140.RDT): Pistole 3 / Redhawk 7 / Schrot 8 / Python 20 /
  Ingram 12 ueber den Tisch -> hit=0, hp 250, verworfen=1; neben dem Tisch (x 9000) -> Treffer; Zombie vor dem
  Tisch (z -11000) -> Treffer; Spieler-Band 1 gegen Band-0-Zelle -> Treffer (Bandfilter @0x8003de60-6c);
  Diagonale ueber die Tischecke (-6000,-10000)->(0,-19000) blockiert, links vorbei frei; ohne SCA Treffer.
  Fresser (RE2-KI, ss1 8, 1D3=80, 10E=4004): Pistole/Redhawk/Schrot/Python -> hit=0; zwei Fresser -> hit=0;
  nach dem Aufstehen (1D3 &= 0x7f, 10E &= ~0x4000) -> Treffer; je Gate einzeln; Liegender 0x08 frei, 0x88 gesperrt.
  Ergebnis `test_r36_schuss: OK` (41 Pruefungen).
* exe (`run_p1.sh nachher 20`, dieselben Laeufe wie 1.1, je 6 Schuesse laut wf.log `SE arms_rec=0`):
  Tisch: Plaetze 1..5 hp 79/50/80/250/250 UNVERAENDERT (vorher 4/5 -> -1); Fresser vom Tuerpunkt: alle fuenf
  unveraendert (vorher 2/5 -> -1).
* `integration_r36_schuss` (test_r36_schuss.cmake, 3 Laeufe): tisch alle Plaetze HP gleich, fresser alle Plaetze
  gleich, GEGENPROBE frei (Leon (200,-18200) hinter dem Tisch, Platz 5 in 1400 -> geweckt, Python) Platz 5
  250 -> -1. Passed 46.96 s.

## 2. Punkt 2 — Blut-Anker bei getroffenen Zombies

### 2.1 Messung vorher (`probe_r36_blut mess eben|tief`, ROOM1140, RE2-KI, RE2-Bank CDEMD0.EMS, echter game_step)
Leon 1800 vor Platz 2 (-1800,-19600) (alle anderen Plaetze aus), Pistole 3, Schuss alle 12 Bilder, sobald der
Zombie steht. Je Bild wird die GEZEICHNETE Pose (g_anim_pose_actor = Aktor, wie main.c) berechnet und die
Part-Weltlagen 0/1/8 gemerkt; beim Treffer-Spawn (re15_re2z_last_fx_pos, Part 0/1, HP-Verlust im selben Bild)
wird die Blutlage gegen den RE2-Sollpunkt (2.2) aus der Pose des Vorbilds (T-1, Original) und dieses Bilds (T)
verglichen:
* EBEN (Zone 1 Rumpf): `F66 part=0 fx=(-1468,-2010,-19720)` = 14 Einheiten vom BECKEN (Part 0, y -2011); Soll
  (-1518,-2513,-19705) -> **d = 505** (503 gegen T). Zweiter Treffer F390: d = 500.
* TIEF (Zone 0 Beine): `F66 part=1 fx=(-1468,-2010,-19720)` = 14 vom Becken (Part 1 liegt in RE2 bei (0,0,0) =
  Becken); Soll (-1357,-1225,-19663) = Kniehoehe -> **d = 794**. F306: d = 824.
=> Das Blut erschien IMMER am Becken: beim Rumpf-/Kopftreffer ~500 zu tief, beim Beintreffer ~800 zu hoch —
der Befund des Nutzers ("manchmal nicht an der richtigen Stelle" = immer, wenn die Zone nicht die Mitte ist).
Nebenbefund: der Port laesst HURT P0 im SELBEN Spielbild wie den Treffer laufen; Soll(T-1) und Soll(T) liegen
12-20 auseinander (ein Bild Bewegung).

### 2.2 RE-Belege (RE2 EMZ0.BIN, selbst disassembliert; RE2 = Vorgabe fuer die Zombie-KI)
Die drei Treffer-Emitter rufen FUN_8001BF10(a0 = id, a1 = Gier, a2 = Part-WELTMATRIX, a3 = VERSATZ):
```
Haupt-Handler P0 (0x80105560, Blut @0x80105650-708):
80105650: lbu v1,466(s1)  ... multu 0xaaaaaaab ... 80105678: beq v1,zero,0x801056dc  ; Zone = +0x1D2 % 3
8010567c: addiu a0,zero,6096           ; id (Delay-Slot, beide Zweige)
80105680: lh v1,346(s1)                ; +0x15A (= 1 @0x801055BC, -1 bei +0x1D0 & 0x20 @0x80105624-38)
80105684: lw a1,408(s1)                ; +0x198 Modellblock
80105688-9c: v0 = 100 * v1
80105698: lw v1,1420(a1) / 801056a0: subu v1,v1,v0 / 801056a4: sh v1,16(sp)   ; a3.x = P8.t.x - 100*seite
801056a8: lw v0,1424(a1) / 801056b0: addiu v0,v0,300 / 801056b4: sh v0,18(sp) ; a3.y = P8.t.y + 300
801056b8: lw v0,1428(a1) / 801056c4: sh v0,20(sp)                              ; a3.z = P8.t.z
801056c8: lh v0,118(s1) / 801056d0: subu a1,v0,a1                              ; Gier = +0x76 - s3 (2048 bei Ruecken)
801056d8: addiu a2,s2,72                                                       ; a2 = Part 0 (+0x198 + 72)
801056dc-704 (Zone 0): a3 = (0, 800, 0) (`sh zero,16 / sh 800,18 / sh zero,20`), a2 = +0x198 + 244 = Part 1
80105708: jal 0x8001bf10
```
1420/1424/1428 = 8*172 + 0x2C/0x30/0x34 = die lokale Translation (MATRIX +0x18, t @+0x2C) von Part 8 = sein
Bind-Versatz (RE2 EM010 Part 8 = (-40,-783,0), aus der Bank gelesen; Messung 2026-08-20 "8 mittig oben").
Stagger P0 @0x80105D14-DBC: wortgleich, seite = `lb v1,362(s4)` = +0x16A (@0x80105D2C), y + 300 (@0x80105D5C).
Wurzel-Flinch @0x801050B0-158: Zone 0 -> Part 1 (0,800,0) id 6096 (@0x801050F0-110); sonst Part 0, a3 =
(P8.x, P8.y + 500, P8.z) ohne Seite, id 4096 (@0x8010511C-58).
Lage der ESP-Maschine = Anker.R * a3 + Anker.T (re15_esp.c:1455, `jal 0x800661c0` @0x8001a248 + `addu`
@0x8001a258-2a4); die Anker-Matrix ist die des ZEICHNERS im Vorbild (Record +0x48, J_affen.md N1).
Zone: Maskentabelle @0x800A6DB4, `srl s7,v1,1` @0x8004149C -> 0 Beine / 1 Rumpf / 2 Kopf (re15_damage.c-Block
"DIE MASKENTABELLE"). => RE2 setzt Rumpf-/Kopfblut 483 ueber dem Becken (Brustbereich, 100 zur Schuetzenseite)
und Beinblut 800 unter dem Becken (Knie); der Port setzte beides AM Becken (kein a3).
RE1.5 (Nebenbefund, nicht Vorgabe): FUN_80105b7c P0 spawnt FUN_80019700(0x2000, +0x6a, Bone +0x40, a3 =
@0x8011fe84 = (200,0,0)) am Gore-Bone LAB_8011f784[Typ] (Zombie 14 = Brust) bei +0x6 == 1 (@0x80105c48-a0),
an Bone 2 bei +0x6 == 0 (@0x80105cb0-d0); der Port-RE1.5-Pfad (re15_enemy_hurt_fx) spawnt an der Aktor-WURZEL
(Fusshoehe) — OFFEN 2 (Abschnitt 5).

### 2.3 Umsetzung
* `schuss_r36.c` `re15_schuss_r36_blut_anker(e, bone, a3, out)`: Lage = Welt(t_part) + Ryaw * (R_part * a3)
  (ApplyMatrix-Zwilling FUN_800661c0 @0x8001a248 + Ryaw wie re15_skel_bone_to_world). Die Pose kommt wie beim
  Zeichner CROSSFADE-geblendet (FUN_8001f3bc): ueber einen Schatten-Aktor (Kopie, g_anim_pose_actor = &sh,
  Muster enemy_ai_re2_zellenarm.c:462-82) — dieselbe Pose, die der Renderer in diesem Bild zeichnet, ohne den
  Blend-Zustand des echten Aktors zu veraendern (roh vs. gezeichnet gemessen bis 98 Einheiten im zweiten
  Treffer-Clip). Ragdoll (re2z_rag_anchor_on): Anker wie bisher ueber re15_enemy_bone_world_pos.
* `enemy_ai_re2_zombie.c`: `re2z_blood_a3(e, zone, seite, dy)` baut a3 aus `bone_relative_pos[Part 8]` (lw 1420/
  1424/1428), `- 100*seite` (subu), `+ dy` (addiu 300/500); `re2z_blood_fx_dir(e, yaw, seite, dy)` reicht es an
  `re2z_blood_fx_scaled(..., a3)` -> `re15_schuss_r36_blut_anker`. Aufrufer: Haupt-Handler P0 seite = +-1 aus
  +0x1D0 & 0x20 (= +0x15A @0x801055BC/@0x80105638), dy 300; Stagger P0 seite = (int8)+0x16A, dy 300, Gier =
  +0x76 - 2048 bei Ruecken (@0x80105D68-7C); Wurzel-Flinch seite 0, dy 500. Die uebrigen Nebenpfad-Emitter
  (re2z_blood_fx_at, MG-Emitter Part 8) unveraendert (a3 = NULL). Die Praesentation (Skala 0x2000, 8 Splatter,
  RE1.5-Mandat) bleibt.

### 2.4 Messung nachher (dieselbe Sonde)
* EBEN: `F66 part=0 fx=(-1503,-2512,-19712)` Soll(T-1) d = 16, Soll(T) d = 1; F390 d = 19 / 1 — Blut 501 ueber
  dem Becken (Brust), 100 zur Schuetzenseite.
* TIEF: `F66 part=1 fx=(-1345,-1224,-19664)` d = 12 / 1; F306 d = 4 / 0 — Blut 800 unter dem Becken (Knie).
* exe (`run_p2.sh nachher`, ROOM1140, Leon (-5000,-19600) Blick +x, Pistole, Hybrid-Modell): fx.log `id=0 ...
  w(-1733,-2445,-19605) F=55`, state.log `F55 Platz 2 50 -> 34`; Bild `C_schuss_mess/p2_nachher_bild54_57.png`
  (Bilder 54..57, Ausschnitt): der Blutstoss sitzt auf Brusthoehe an der dem Schuetzen zugewandten Seite.
* Pins `unit_r36_blut_pin_eben` / `unit_r36_blut_pin_tief` (probe_r36_blut pin ...): Abstand zum RE2-Sollpunkt
  <= 40 (Schranke; vorher 500-800) UND Mechanismus: Rumpfblut > 300 ueber, Beinblut > 300 unter dem Becken.

## 3. Punkt 3 — Super Redhawk gegen Kraehen (RE2-Vergleich)

### 3.1 Messung vorher
(folgt)

### 3.2 RE-Belege (RE2 Retail)
(folgt)

### 3.3 Umsetzung
(folgt)

### 3.4 Messung nachher
(folgt)

## 4. Tests
(folgt)

## 5. OFFEN
(folgt)

## 6. Fuer den Nutzer
(folgt)
