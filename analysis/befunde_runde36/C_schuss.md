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
  250 -> -1. Passed 46.96 s. (Fortsetzung: + gl_tisch/gl_fresser mit dem Granatwerfer 15, Passed 79.00 s.)

**Wortlaut "ALLE Schusswaffen" — Messung je Waffe an der exe** (`C_schuss_mess/run_p1_alle.sh alle`, Tabelle
`C_schuss_mess/hp_tabelle.py mess_r36c/alle`; dieselben zwei Aufstellungen wie 1.1, RE2-KI, 6 Schuss je Lauf,
Eingabe `M0.6,MA1.5,M1.0,W4`, `RE15_EXIT_AT=200#1140`; alle 30 Laeufe exit 0). Jede Waffe hat nachweislich
gefeuert (state.log `mg=` 6 -> Rest, wf.log `SE arms_rec=0 bank=<id>` bzw. `SE re2fx code=0x011x0001` mit der
Explosionslage): 3/4/13 6->3, 5/6/12/14/19 6->0, 7/8/15/16/17/18/20 6->4. Plaetze (st 1): 1 Liegender hp 79, 2..5
Fresser hp 50/80/250/250.
| Id | Waffe | Pfad im Port (Original) | tisch: HP-Verlust | fresser: HP-Verlust | Beleg des Feuerns |
|---|---|---|---|---|---|
| 3 | Browning HP | FUN_80011f50 + Sichtlinie @0x80012168 / RE2-Gates ueber +0x93 | keiner | keiner | mg 6->3, `arms_rec=0 bank=3` |
| 4 | SIG P228 | dito | keiner | keiner | mg 6->3, bank=4 |
| 5 | Beretta M93R | dito (Feuerstoss) | keiner | keiner | mg 6->0, bank=5 |
| 6 | Glock 18 | dito (Feuerstoss) | keiner | keiner | mg 6->0, bank=6 |
| 7 | Super Redhawk | dito (Kritklasse @0x80012380-b8) | keiner | keiner | mg 6->4, bank=7 |
| 8 | Remington M870 | dito | keiner | keiner | mg 6->4, bank=8 |
| 12 | Ingram M10 | dito (Dauerfeuer) | keiner | keiner | mg 6->0, bank=12 |
| 13 | SPAS-12 | dito | keiner | keiner | mg 6->3, bank=13 |
| 14 | Flammenwerfer | RE2-Strahl 0x031D1200 (werfer_r35.c) -> Applier FUN_800470C0 Gate 2/4 @0x80047138-64 | keiner | keiner | mg 6->0, `re2arms ARMS10 satz=0/11` — Reichweite ~1,5 Koerperlaengen: von beiden Standorten erreicht der Strahl keinen Platz (Mechanik: unit_r35_werfer 104-107, Zellen-Sperre B §8.1) |
| 15 | GL Explosiv | RE2-Projektil (re2_fx.c, Zellen-Sperre `wandzelle_im_band`) -> Applier | keiner — Explosionen an der TISCHKANTE @(-102,-2101,-11760), (-233,-2101,-11760), (-34,-1961,-11570) (Kante z -11350) | keiner — Explosionen ZWISCHEN den Fressern @(-4897,-497,-19161), (-4745,-499,-18720), (-928,-1892,-20295) (Platz 1 (-800,-20600) 330 entfernt, Platz 2 (-1800,-19600) 1120) | mg 6->4, `ARMS0F Satz 10` |
| 16 | GL Saeure | dito | keiner — @(-165,-1797,-11262) | keiner — @(2444,-1595,-21462) (Platz 4 (200,-21600) 2250 entfernt) | mg 6->4, `ARMS10 Satz 10` |
| 17 | GL Brand | dito | keiner — @(-165,-1797,-11262) | keiner — @(2444,-1595,-21462) | mg 6->4, `ARMS11 Satz 10` |
| 18 | Raketenwerfer | dito | keiner — @(-174,-2530,-10668) (vor der Kante, wie unit_r35_werfer 111) | keiner — @(4165,-3056,-22211) / (3532,-2754,-21817): die Rakete fliegt an den Fressern vorbei (naechster Platz 4 in ~4000), der Lauf prueft hier nur "kein Treffer"; die Gate-Mechanik der Rakete pinnt unit_r35_werfer 104-107 (Applier gegen 1D3=80/10E=4000) | mg 6->4, `RE2 ARMS11 Satz 20` |
| 19 | H&K MC51 | FUN_80011f50 (Dauerfeuer) | keiner | keiner | mg 6->0, bank=19 |
| 20 | Colt Python | FUN_80011f50 (PORT-WAHL Streifen) | keiner | keiner | mg 6->4, bank=20 |
Vorher (1.1, Python): tisch Plaetze 4/5 250 -> -1, fresser Plaetze 2/5 -> -1. Handgranaten 9/10/11 sind Wurfwaffen
(Spur A r35, A_granate.md) und nicht Teil des Wortlauts "Schusswaffen". Messer 1 / Rohr 2 (Nahkampf): Sichtlinie
byte-true dabei (@0x80012168 hinter der Schleife), Fresser-Sperre im Port ueber denselben +0x93-Riegel (Info-Zeile
`unit_r36_schuss`: w1/w2 gegen Fresser in 1200 hit=0 hr=03) — RE2s Messer-Resolver nicht disassembliert (OFFEN 5).

### 1.5 Bestehende Tests, die durch Punkt 1/3 rot wurden (Suite-Lauf mess_r36c/suite1.log, je Test entschieden)
Der erste Suite-Lauf (Stand 37d796f3, 7310 s; ab Test 407 brach die Maschine mit Timeouts/0xc0000142 weg —
Umgebung, nicht Code, s. §7) zeigte fuenf echte Rot-Faelle. Keiner deckt eine Regression auf, jeder pinnte das
ALTE Verhalten, das der Auftrag aufhebt:
| Test | Rot weil | Entscheidung (Commit b19cc82e / cd06ef46) |
|---|---|---|
| unit_re2_zombie_hitfilter | Pruefung 4a pinnte "Fresser in der Spawn-Pose muss TREFFBAR sein" (v0.3.5, Sollseite RE1.5 FUN_80011F50 @0x80011FFC-38) | Umgedreht auf GESPERRT mit RE2-Beleg Gate 2 @0x80041270-78 / Gate 4 @0x80041290-9c (+0x1D3 aus EXEC[8] P0 @0x80103C04-14, +0x10E aus dem Spawn-Remap @0x80100A88-8C); Latches bleiben stehen (zweite Pruefung unveraendert) |
| unit_re2_zombie_abc / probe_re2z_block (Sweeps) | "UNSTERBLICH"-Zaehler: ein Zombie mit hp >= 0 >= 200 Bilder vom Kandidatenfilter gesperrt — der Fresser ist das jetzt per Regel, bis er aufsteht/geweckt wird | Der Fresser in der Spawn-Pose (st 1, ss1 8, +0x1D3 & 0x80) zaehlt nicht als Haenger (Zeile mit Beleg im Test); alle anderen Haenger-Pruefungen unveraendert |
| unit_re2_crow_ai | Pruefung "GIB verbirgt das Mesh (Part-Scatter-Stand-in)" pinnte das crow_hide des alten Stand-ins | Pin auf das RE2-Verhalten: 13 Zeilen mit eigener Matrix (Bit 0x40), Zeile 0 Wurf 0x4A (@0x80102CE8-F0 / Wurf 0x80102EF4), crow_hide 0 |
| unit_crow_death_corpse (probe_crow_death.c, RE1.5-KI) | Die Sonde schoss von (530,-5150) durch die THEKE von ROOM1170 (RDT @0xb10: x -12422..9682, z -10910..-8498, Wort 0x43 = Band 4 / Klasse 3) auf die fliegende Kraehe (1719,-10522,-10983): FUN_8003dcc4 ohne y-Komponente (liest nur cur+0x34/+0x3c @0x8003dd00/10, delta+0/+8 @0x8003dd20/4c; Band = Spieler +0x82 @0x8003de60) sperrt den Schuss — gemessen `fire=0 ... los=1`. Alle sieben Kraehen nach 60 Bildern: sechs sitzen (y -7600, Band 4), eine fliegt (Band 5) hinter der Theke; die zweite fliegende (10106,-10522,-6729) sitzt IN der Zelle x 5848..13624 / z -9776..-5672 (@0xb1c) | Standort bleibt; TIEFZIELEN (re15_player_set_aim_elevation_for_test(-1): Boden-Kraehe vert 400 < 800 = DOWN-Band des ACTIVE-Tail-Stempels @0x80112560-c8) auf die naechste Kraehe mit freier Sichtlinie (Slot 4 (-2770,-7600,-100), `fire=5`); Treffer-Pruefung (fire != 0) und "keine Kraehe gefunden" sind jetzt FAIL statt stiller Rueckkehr. Beide Laeufe (w3 Normal-Lane: Land-Clip 0x0a Frame 12, Lache 375+510, flags 0x42; w7 GIB: Pool-WIPE 1x1, hide) erreichen dieselben Erwartungen wie vorher |
| unit_pri_hashes | war seit master 5a9dbb7b rot (Spur B) | master 78a4a748 (Hashes neu erzeugt) in den Zweig gemerged, gruen |

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

### 3.0 Antwort fuer den Nutzer
**Ja — in RE2 ist das auch so.** Der Magnum-Treffer (RE2-Zeile 5 = Super Redhawk) schickt die Kraehe in den
GIB-Handler 0x80102CA0: sie zerplatzt in ihre 13 Koerperteile, es bleibt KEIN Kadaver und KEINE Blutlache (GIB-
Leiche 0x80103C0C gibt den Schatten-Record frei). Der Unterschied zum bisherigen Port: in RE2 fliegen die 13
Teile ~0,5-1 s sichtbar auseinander (dunkelrote Scherben + wirbelnde Teile, die vom Boden abprallen) und
verschwinden dann; der Port liess das Netz SOFORT verschwinden (crow_hide) und setzte 13 Blut-Splatter als
Ersatz. Jetzt laufen die 13 Teile ueber dieselbe Teile-Physik wie der Zombie-Zerleger. Pistole/Schrot gibben
NICHT (Zeilen 2/3/4 -> Flug-Treffer 0x80102934, 7/8 -> Launch 0x8010327C), nur Magnum, GL-Runden und Rakete.

### 3.1 Messung vorher (Port-Stand, Code gelesen + r34 mess_geg.md 3.2)
enemy_ai_re2_crow.c re2c_hurt_gib: `crow_hide = 1` (main.c ueberspringt das Netz ab dem GIB-Bild) +
`re15_esp_fx_splatter(.., 13, ..)` + 3 Federbursts; die Wirbel-Zeilen wurden nur als RNG-Draws nachgebildet
(re2c_gib_swirl_draws). r34 mess_geg.md 3.2: "st 7 (Leiche) sofort, Modell weg" — der Koerper verschwindet im
Treffer-Bild komplett (das ist der Befund des Nutzers).

### 3.2 RE-Belege (RE2 Retail, EMOVL21_S0.BIN, selbst disassembliert mit re2_disasm.py --bin EMOVL21_S0.BIN)
* HURT-Zeilen-Dispatch @0x80104A18 (`table 0x80104a18 19`): [0..4] 0x80102934 Flug-Treffer, [5] [6] 0x80102CA0
  GIB, [7] [8] 0x8010327C Launch, [9] [10] [11] 0x80102CA0, [12] 0x80102FB4 Boden, [13..16] 0x80102934,
  [17] 0x80102CA0, [18] 0x80102934. Index = +0x5 = RE2-Waffen-Id (Hitscan `sb hi16+1,5` @0x80041AB4); der
  Super Redhawk (RE1.5 w7) ist RE2-Zeile 5 (Magnum, re2z_row_from_weapon / s_re2c_row_from_weapon).
* GIB 0x80102CA0: `sh zero,326(s0)` @0x80102CB8 (+0x146 = 0); `jal 0x800154ac` @0x80102CD4 mit (PL.x, PL.z,
  self+0x38, self+0x40) = Peilung s1; `jal 0x80015fe8` @0x80102CDC = Maske s2; dann die 13 Zeilen
  ([+0x198] + i*172):
```
80102cec: jal 0x80102ef4  (a0 = +0x198 + 0)        ; Zeile 0 IMMER Wurf
80102cf4: andi v0,s2,0x1 / beq -> 0x80102d24        ; Bit 1: 1,2 Wurf (+172/+344) sonst Wirbel 0x80102f34
80102d40: andi v0,s2,0x2 / beq -> 0x80102d80        ; Bit 2: 3,4,5 (+516/+688/+860)
80102dac: andi v0,s2,0x4 / beq -> 0x80102dec        ; Bit 4: 6,7,8 (+1032/+1204/+1376)
80102e20/e30: jal 0x80102ef4 (+1548 / +1720)        ; 9, 10 IMMER Wurf
80102e38: andi v0,s2,0x8  / beq -> 0x80102e58       ; Bit 8: 11 (+1892)
80102e64: andi v0,s2,0x10 / beq -> 0x80102e84       ; Bit 16: 12 (+2064)
80102e90-ea4: 3x jal 0x8010459c (Federburst); 80102ea8: jal rand / andi 0x3 / ori 0x8 -> 0x80104678 (Federn)
80102ec0: jal 0x80018fb0 (Kill-Buchhaltung); 80102ec8-d4: 0x80104078(self, 7, 1) = GIB-Leiche
```
* WURF 0x80102EF4: `lui a2,0x10 / ori a2,a2,0x1040` -> `sw a2,112(a0)` (+0x70 Tinte 0x00101040), `sh 800,156`
  (+0x9C), `sh 10,158` (+0x9E), `sh a1,152` (+0x98), `sh zero,160` (+0xA0), `sh zero,154` (+0x9A),
  `sh -100,164` (+0xA4), `ori v1,v1,0x4a / sw v1,0(a0)` (@0x80102EF4-F30). Flag 0x08 -> FUN_80028DAC (Drift):
  Tempo 800 - 100/Bild, Fall +10/Bild, Lebensdauer `sltiu 0x1d` @0x80028DCC -> nach 29 Bildern `sw zero,0`
  @0x80028DDC = das Teil VERSCHWINDET. Flag 0x08 zeichnet ausserdem ueber den Scherben-Renderer FUN_8002D3C8
  (Tinte flach, Dreiecke entlang der Normalen versetzt — re15_re2z_gore_part_burst).
* WIRBEL 0x80102F34: `ori v0,v0,0x1062 / sw v0,0(s0)` @0x80102F50-58; `jal rand` @0x80102F54 -> `sll 2 / addu /
  sh s1,56` (+0x38 = Peilung + rand<<2); `sh zero,134` (+0x86 = 0 -> INIT); `jal rand` @0x80102F68 -> `andi 0x7f /
  addiu 100 / sh v0,60` (+0x3C); `sh zero,58` (+0x3A); `addiu v0,zero,20 / sb v0,121` (+0x79 = 20); `sh zero,62`;
  `jal rand` @0x80102F88 -> `sll 4 / sh v0,64` (+0x40); `sh zero,66` (@0x80102F7C-98). Flag 0x20 -> FUN_80028AD8:
  Fall +20/Bild, Aufschlag v/4, Bodenprobe FUN_8004FBA0 an der Teil-Lage @0x80028B74, beim ZWEITEN
  Bodenkontakt `and v0,v0,-2` @0x80028CA0 = Bit 0 weg = das Teil VERSCHWINDET.
* GIB-Leiche 0x80103C0C: +0x1D3 = 128, word0 |= 2, `sb zero,14(rec)` @0x80103C34 = Schatten-/Lachen-Record frei.

### 3.3 Umsetzung
* `schuss_r36.c`: `re15_schuss_r36_kraehe_parts_seed(e)` (Modellblock wie re15_re2z_part_reset: 13 Zeilen Bit 0,
  Tinte 0x808080, Mesh = i; Marker `re2c_gib_parts = 1`), `re15_schuss_r36_kraehe_gib(e, a1, m)` = der GIB
  @0x80102CE8-E8C mit `kraehe_wurf` (0x80102EF4) / `kraehe_wirbel` (0x80102F34, drei Draws in Original-Folge).
* `enemy_ai_re2_crow.c`: re2c_init ruft den Seed; re2c_hurt_gib: Peilung (0x800154AC(PL, self) in der Port-
  Konvention von re2c_hurt_flight), Maske = rand @0x80102CDC, `re15_schuss_r36_kraehe_gib`, `crow_hide = 0`;
  die 13 Splatter-Stand-ins und re2c_gib_swirl_draws entfallen; Federbursts/Federn/Zustand (7,1) wie bisher.
  Test-Einstieg `re15_re2crow_hurt_test`.
* `enemy_ai_re2_zombie.c` (2 Haken): re15_re2z_gore_active gilt fuer 0x21 unter RE2-KI mit `re2c_gib_parts`
  (eigener Marker, weil Zeile 0 der Kraehe selbst geworfen wird und nach 29 Bildern Flags 0 traegt — beim
  Zombie ist das Becken der Marker); re2z_part_phys_ad8 nimmt fuer 0x21 den Boden +0x1C2 (`dog_floor_y`, die
  Boden-Probe der Kraehe) statt y (die Kraehe fliegt) — das Original probt den Boden an der Teil-Lage
  (FUN_8004FBA0 @0x80028B74), der Port hat dafuer die Aktor-Bodenhoehe (PORT (1) im Block ueber den Physiken).
* `re15_actor.h`: neues Feld `re2c_gib_parts` (Marker).
* Renderer unveraendert: main.c zeichnet die 13 Teile ueber die vorhandene Part-Bruecke (re15_re2z_gore_resolve
  / re15_re2z_gore_part_matrix / re15_re2z_gore_part_burst), die seit Welle G fuer den Zombie gilt.

### 3.4 Messung nachher
* `unit_r36_schuss_kraehe`: Seed (13 Zeilen Bit 0, 13..15 leer); Maske 0x1F: 0 Draws, alle 13 Zeilen Flags 0x4B,
  Tinte 0x101040, 800/10/Peilung/0/0/-100; Maske 0: 30 Draws (10 Wirbel x 3), 0/9/10 Wurf, Rest 0x1063 mit
  +0x3C 100..227, +0x79 20, +0x40 = rand<<4; Maske 0x02: 21 Draws; Physik ueber re15_re2z_gore_part_matrix
  (Kraehe bei y -1500, Boden 0): Wurf-Zeilen Flags 0 im Bild 30 (= nach 29 Bildern), alle 13 Teile weg bis Bild
  30, Wirbel-Teile liegen bei y 0 (Boden +0x1C2, nicht Flughoehe); Dispatch: w7 -> GIB (7,1), w3 -> Flug-Treffer.
  `test_r36_schuss_kraehe: OK`.
* exe (`run_p3.sh nachher`, ROOM10C0, Kraehe Slot 3 sitzt bei (5100,3600), Leon 3522,5556,581 + FORCE_CUT 1,
  Redhawk, Eingabe `MD0.6,MDA0.4,MD1.0` = TIEF zielen, weil die sitzende Kraehe im DOWN-Band liegt): state.log
  `F19 Platz 3 10 -> -1 st=7 ss1=1`; Bilder `C_schuss_mess/p3_nachher_kraehe_bild20_44.png` (Ausschnitt Bilder
  20/22/24/28/34/44): dunkelrote Scherbenwolke und Teile fliegen von Leon weg ueber den Boden, bei Bild 34 ist
  nichts mehr da; `p3_nachher_kraehe_bild20.png` das ganze Bild 20. Vorher (r34 mess_geg.md 3.2): "Modell weg"
  im Treffer-Bild.

## 4. Tests (alle in tests/unit/probes/r36_schuss.cmake registriert)
| Test | Misst |
|---|---|
| unit_r36_schuss | Sichtlinie FUN_8001b9b4 ueber den Konferenztisch (echte ROOM1140.RDT, alle zehn Hitscan-Waffen 3/4/5/6/7/8/12/13/19/20 + Messer-Diagonalregel), Fresser-Sperre je Gate fuer alle zehn, Aufstehen gibt frei (63 Pruefungen) |
| integration_r36_schuss | exe ROOM1140: Python UND Granatwerfer 15 — Tisch sperrt (5 Plaetze HP gleich), Fresser gesperrt, Gegenprobe (Python) trifft (5 Laeufe, 79 s) |
| unit_crow_death_corpse (bestehend, angepasst) | RE1.5-Kraehen-Tod in ROOM1170 — jetzt mit Tiefzielen auf eine Kraehe mit freier Sichtlinie; Treffer-Pruefung fire != 0 (§1.5) |
| unit_re2_zombie_hitfilter / _abc, probe_re2z_block, unit_re2_crow_ai (bestehend, angepasst) | §1.5 |
| unit_r36_blut_pin_eben / _tief | Blut-Anker = Anker.R*a3 + Anker.T (RE2-Sollpunkt <= 40, Rumpf ueber / Beine unter dem Becken) |
| unit_r36_schuss_kraehe | RE2 GIB 0x80102CA0: Felder, Draws, Physik bis zum Verschwinden, Dispatch @0x80104A18 |
Suite: §7.

## 5. OFFEN (mit Adresse und Messweg)
1. **RE1.5-KI-Pfad des Fressers (nicht Vorgabe):** unter RE1.5-KI fuehrt der Port fuer die 0x86/0x06-Fresser keinen
   +0x93-Riegel (RE1.5 hat fuer sie keinen Nibble-Dispatch, INIT-Decoder @0x80100E60/EB0 -> +0x5 = 0xc); ob
   RE1.5 den Fresser im Auslieferungsstand sperrt, ist nicht belegt (Messweg: Savestate ROOM1140 unter DuckStation,
   +0x93 der Fresser je Bild lesen). Die Vorgabe des Nutzers ist mit RE2-KI (Default) erfuellt.
2. **RE1.5-KI-Pfad des Bluts:** re15_enemy_hurt_fx spawnt an der Aktor-Wurzel; RE1.5 FUN_80105b7c P0 spawnt am
   Gore-Bone LAB_8011f784[Typ] (+0x6 == 1 @0x80105c48-a0) bzw. Bone 2 (+0x6 == 0 @0x80105cb0-d0) mit a3 =
   (200,0,0) @0x8011fe84; die uebrigen +0x6-Zweige (@0x80105ce4 ff.) sind nicht disassembliert. Messweg: probe_r36_blut
   mit RE15_AI_FLAVOR=re15 und RE1.5-Bank. Nicht Teil der Vorgabe (RE2-KI).
3. **Sichtlinie bei gesperrtem Fresser als naechstem Kandidaten** (1.3): RE1.5-Reihenfolge (Sichtlinie vor dem
   +0x93-Latch) statt RE2-Schleife; praktisch nur relevant, wenn ein Fresser naeher als jeder wache Zombie UND
   hinter Moebeln liegt.
4. **Kraehen-Teile:** die Drift-Physik FUN_80028DAC hat keine Bodenprobe (Original ebenso), die Wurf-Physik
   FUN_80028AD8 probt im Original den Boden an der TEIL-Lage (FUN_8004FBA0 @0x80028B74) — der Port nimmt die
   Boden-Probe der Kraehe (+0x1C2); der Wandtest @0x80028CAC-D20 bleibt wie beim Zombie nicht modelliert.
5. **Messer/Rohr gegen Fresser:** der Port sperrt sie ueber denselben +0x93-Riegel wie die Schusswaffen (gemessen
   hit=0, hr=03); welche Gates RE2s Messer-Weg fuehrt (FUN_800410CC hat sechs Aufrufer: @0x80043afc, 4x in
   0x80047ef4..0x80048d48, @0x80062bd4 — nicht zugeordnet), ist nicht disassembliert. Nicht Teil des Wortlauts
   ("Schusswaffen").
6. **Werfer-Laeufe 16/17/18 am Fresser-Standort:** die Runden explodieren 2250 (Saeure/Brand) bzw. ~4000 (Rakete)
   vom naechsten Fresser entfernt — der exe-Lauf belegt fuer sie nur "kein Treffer", die Gate-Mechanik der
   Werfer belegt unit_r35_werfer 104-107 (Applier FUN_800470C0 Gate 2/4) und der GL-Explosiv-Lauf (Explosion 330
   neben Platz 1).

## 6. Fuer den Nutzer
* Keine neuen Assets, keine Sprachdateien, keine Bank-9-Bits/Nachrichten/Ereignisse.
* Verhalten: (1) Kein Schuss (auch Messer/Rohr) trifft mehr durch Klasse-3-Zellen (Tische, Baenke, Saeulen, Waende)
  im eigenen Band — wie im Original; (2) fressende Zombies (RE2-KI) sind unverwundbar, bis sie aufstehen (sie
  wachen auf, sobald man naeher als 4000 kommt); (3) Blut sitzt jetzt an der Trefferzone (Brust bei Rumpf-/
  Kopftreffer, Knie bei Beintreffer, zur Schuetzenseite); (4) Kraehen zerplatzen beim Magnum-Treffer wie in RE2 in
  13 sichtbar wegfliegende Teile und sind danach weg.
* Entscheidungsfrage (optional): der LIEGENDE Spawn-Zombie (EXEC[7], nicht passiv 0x87..0x8A) ist weiterhin
  treffbar (v0.3.5-Ausnahme) — soll auch er wie in RE2 bis zum Aufstehen gesperrt sein?
* Bedienhinweis: Kraehen am Boden (ROOM1170, sitzend) liegen im DOWN-Band (vert < 800 @0x80112560-c8) — tief
  zielen; eine Kraehe hinter der Theke (oder ueber einem Moebel in der Schusslinie) ist wie im Original nicht
  treffbar, solange man im selben Band davor steht.

## 7. Suite / Abschluss
* Erster Suite-Lauf (Vorgaenger, `mess_r36c/suite1.log`, 7310 s): 196 rot — davon 5 echte (§1.5) und 191
  Umgebungsabbrueche (ab Test 407 Timeouts in Reihe, danach 314 x exit 0xc0000142 STATUS_DLL_INIT_FAILED
  auch fuer reine Unit-Tests ohne Fenster — die Maschine unter der Last der parallelen Spuren, nicht der Code;
  dieselben Tests laufen einzeln gruen).
* Zweiter Suite-Lauf (diese Sitzung): s. unten (woertliche Zeile).
