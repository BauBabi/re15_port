# Runde 36 — Spur C "schuss" (2026-10-10)

Baum: `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, Basis master 5a9dbb7b (v0.8.23 + Auftrag), master d0420938 /
78a4a748 (Nachtrag 14:00, PRI-Hashes) gemerged.

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
| unit_r36_blut_pin_eben / _tief | (Nachbesserung 1, §8.3) JEDER Blut-Spawn bis zum Tod + 80 Bilder gegen alle Emitter-Sollpunkte Anker.R*a3 + Anker.T (HURT, Stagger, Flinch, Nachbluten, Aufsteh-Treffer, TOD, Tropfblut, Tod des Liegenden, Kriecher-Tod) und die Defekt-Kandidaten (Anker ohne a3); Versatz im Anker-Rahmen je Familie |
| unit_r36_blut_pin_mg | (Nachbesserung 1, §8.3/§8.8) MG-Blutstoss 0x80107EF0: a3-Band je Zone, Lage gegen Anker.R*a3 + Anker.T, Summe gegen das Welt-Versatz-Modell |
| integration_r36_schuss | (Nachbesserung 1, §8.4/§8.7) zusaetzlich frei_re15 / tisch_re15 / gl_tisch_re15 (treffbare Ziele hinter dem Tisch), Messkopie der exe (8 Laeufe, 129 s) |
| unit_r36_schuss_kraehe | RE2 GIB 0x80102CA0: Felder, Draws, Physik bis zum Verschwinden, Dispatch @0x80104A18 |
Suite: §7; Nachbesserung 1: gezielte Laeufe §8.11.

## 5. OFFEN (mit Adresse und Messweg)
1. **RE1.5-KI-Pfad des Fressers (nicht Vorgabe):** unter RE1.5-KI fuehrt der Port fuer die 0x86/0x06-Fresser keinen
   +0x93-Riegel (RE1.5 hat fuer sie keinen Nibble-Dispatch, INIT-Decoder @0x80100E60/EB0 -> +0x5 = 0xc); ob
   RE1.5 den Fresser im Auslieferungsstand sperrt, ist nicht belegt. Savestate-Befund (Fortsetzung, `stage_saves/
   mzd_stage1_briefing.sav` + `_live.sav`, Enemy-Array DAT_800acc2c Stride 0x1f4, Spieler am Tuerpunkt (-7600,-17600)
   bzw. (-5118,-15817)): die vier Fresser (+0x5 = 0xc, +0x6 = 3/2) tragen +0x93 = 0x00 und word0 = 0x40000001
   (aktiv + LEVEL-Stempel), der Liegende 0x16 dagegen +0x93 = 0x01. Die RE1.5-Kandidatenschleife FUN_80011f50 gatet
   nur `word0 & Spieler-word0 & 0xE0000000` @0x800120d4-ec (Band) und `lw v0,144(s0)` / `and 0x03000000` /
   `beq v0,s4` @0x800120f4-100 (= +0x93 & 3 == 3 -> uebersprungen) vor dem Tester @0x80012110 — ein RE1.5-Fresser ist
   also KANDIDAT und verloere HP. Dass der Nutzer im Original "keinen Treffer" sieht, kann nur aus dem Hurt-Weg des
   0xc-Zustands kommen (RE1.5 unfertig; nicht disassembliert; Messweg DuckStation-GDB: +0x9a des Fressers beim
   Schuss lesen). Beta -> Retail: Sollseite ist RE2 (Gate 2/4), damit ist die Vorgabe mit RE2-KI (Default) erfuellt;
   unter RE15_AI_FLAVOR=re15 bleibt der Fresser treffbar wie im RE1.5-Resolver.
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
7. **(Nachbesserung 1) Stagger-WURZELBEWEGUNG fehlt im Port (Bewegung, nicht Blut; Eigentuemer Zombie-KI):** RE2
   FUN_80105BC0 P1 ruft jeden Tick `jal 0x80015e7c` (a3 = 0) @0x80105F28, Advance @0x80105F3C und `jal 0x800152c8`
   @0x80105F54 — der Zombie wandert um die Wurzelspur des Stagger-Clips (Hauptbank EM010 Clip 4 x -277 .. -1143,
   Clip 3 x 663 .. 1318, gemessen kfspeed.c). Der Port-Zwilling re2z_hit_move bewegt nur, wenn `motion ==
   re2z_walkclip` (Gate aus dem Haupt-Handler @0x80105B84-90) — im Stagger also gar nicht, nur der P0-Schub -450.
   Ebenso fehlt beim MG-Schub die Seitenkomponente +0x148 = 64-(r&0x7f) (@0x8010812C-40; Port nur +0x144).
   Messweg: probe mit RE2-Bank, Lage des Zombies ueber den Stagger gegen die Summe der Wurzel-Deltas.
8. **(N1) Ragdoll-P1-Nachbluten fehlt:** EMZ0 @0x80106A40-7C (Ragdoll-Handler 0x801066FC, P1 nach Advance/Lehne): bei
   `+0x14D != 0 && (+0x14D & 3) == 0` Id (r1<<3)+6000, Winkel r2<<4, Anker Part 0, a3 = +0x144. Der Port-Ragdoll P1
   (re2z_hit_ragdoll case 1) hat keinen Emitter (fehlendes Blut, nicht falsche Lage); die Bildnummer liegt hinter
   dem Advance — die Port-Zaehlung dafuer ist nicht nachgemessen, deshalb nicht blind eingebaut. Messweg: Schrotflinte
   auf ROOM1140-Zombie, Ring re15_schuss_r36_fx_* mitschreiben, Bildnummern gegen +0x14D.
9. **(N1) Feuer-/Funken-/Griff-Emitter (Zensus §8.8 C):** dieselbe a3-Semantik, im Port Welt-Versatz (z.B. Brand
   (0,200,0) an Part 0/3) bzw. kein Spawn (Id 6). Kein Blut am getroffenen Zombie, nicht Teil von Punkt 2.
10. **(N1) 16 Emitter ab 0x8010A1EC (Zensus §8.8 D):** der Port nennt diese Adressen nirgends; welcher Handler
   (Oberkoerper-/Unterhaelfte-Maschinen ab 0x8010A440 / 0x8010B7D4?) sie aufruft und ob sie im Port erreichbar
   sind, ist nicht bestimmt. Messweg: Caller je Funktion (re2_disasm.py scan) und Ring-Mitschnitt im Port.
11. **(N1) Anker-Pose der a3 = 0-Emitter:** re2z_gore_fx_ex (a3 = NULL) ankert ueber re15_enemy_bone_world_pos (rohe
   Keyframe-Pose), re2z_gore_fx_a3 / re2z_blood_fx_scaled ueber die gezeichnete (Crossfade-)Pose wie das Original
   (Record +0x48 des Zeichners). Unterschied nur waehrend eines Crossfades (gemessen bis 98, §2.3); fuer die
   30 Stellen der Gruppe B nicht umgestellt (keine Verhaltensaenderung ohne eigene Messung).

## 6. Fuer den Nutzer
* Keine neuen Assets, keine Sprachdateien, keine Bank-9-Bits/Nachrichten/Ereignisse.
* Verhalten: (1) Kein Schuss (auch Messer/Rohr) trifft mehr durch Klasse-3-Zellen (Tische, Baenke, Saeulen, Waende)
  im eigenen Band — wie im Original; (2) fressende Zombies (RE2-KI) sind unverwundbar, bis sie aufstehen (sie
  wachen auf, sobald man naeher als 4000 kommt); (3) Blut sitzt jetzt an der Trefferzone (Brust bei Rumpf-/
  Kopftreffer, Knie bei Beintreffer, zur Schuetzenseite); (4) Kraehen zerplatzen beim Magnum-Treffer wie in RE2 in
  13 sichtbar wegfliegende Teile und sind danach weg.
* Nachbesserung 1 (Punkt 2, sichtbar): auch das Blut beim TOEDLICHEN Treffer (Brust statt Becken bzw. Knie), beim
  Treffer waehrend des Aufstehens (unter dem Kopf statt im Kopf), das Nachbluten nach einem Taumel-Treffer (5 Stoesse
  auf Brusthoehe statt 10 am Becken), das Tropfblut des fallenden Zombies, der Tod am Boden, das MG-Blut (Ingram/MC51)
  und die Sondertode mit Schrot/Magnum (Hals-Fontaene, Brocken, Tropfblut; beim Arm-Abriss laeuft das Blut den
  Arm entlang statt senkrecht nach unten) sitzen jetzt an der RE2-Stelle. Das Nachbluten zieht dabei die zwei
  Zufallswuerfe des Originals — die Zombie-Zufallsfolge nach einem Taumel-Treffer laeuft jetzt wie in RE2.
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
* Zweiter Suite-Lauf (diese Sitzung, Stand b1f910fd ohne den Kommentar-Commit, `mess_r36c/suite2.log`,
  `local_build.sh test` = Bau + ctest seriell, 4889,67 s): `100% tests passed, 0 tests failed out of 597`,
  woertlich `=== LOCAL-BUILD-OK (test) — Tests 597/597` — ohne einen einzigen Wiederholungslauf, auch die
  Fenster-Haken gruen.
* Dritter Lauf `local_build.sh all` (configure + build + test) auf dem Endstand: s. letzte Zeile dieses Abschnitts.

## 8. Nachbesserung 1 (nach Abnahme 0, `C_abnahme_0.md` §5 M1-M7)

Stand: Fortsetzung im selben Baum ab 1e8cd231. Je Mangel: Ursache, Messung vorher, Beleg, Aenderung, Messung nachher.
(Wird fortlaufend gefuellt; Reihenfolge der Arbeit: M5/M6 (Belege) -> M1/M2 (Blut) -> M3 (Pin) -> M4/M7 (Integration).)

Gemeinsame Messung fuer M1/M2/M3: die neue Sonde `probe_r36_blut` (§8.3) ueber ALLE Spawns bis zum Tod, vorher
(Stand 62cc4289 + Mess-Ring, Emitter unveraendert) und nachher, Ausgaben `mess_r36c/n1/{vorher,nachher}_{eben,tief}.txt`
(unversioniert; die Zeilen unten sind woertlich daraus). "d" = Abstand zum naechsten RE2-Sollpunkt (Anker.R*a3 +
Anker.T aus der gezeichneten Pose T-1/T), "Defekt" = naechster Anker OHNE a3.

### 8.1 M1 — toedlicher Treffer: Blut am Becken statt am RE2-Punkt
* **Ursache:** die Todes-Emitter riefen `re2z_gore_fx(e, part, id)` = Knochen-Weltlage ohne a3 (der Kommentar nannte
  den Versatz, der Code wandte ihn nicht an). Nicht nur der Haupt-Tod: die Sonde fand im Lauf "tief" einen ZWEITEN
  Todes-Handler mit demselben Fehler (Tod des Liegenden FUN_801099E4), und der Zensus (§8.8) das Tropfblut nach dem Tod.
* **Messung vorher:** eben `F414 SPAWN part=0 fx=(-1554,-1976,-19822) a3=ohne [TOD-Bild, zone 1] -> FLINCH/TROPF d=284 |
  Defekt BECKEN(ohne a3) d=5`; danach Tropfblut `F419..F429` je `BECKEN d=0..22` (6 Spawns); tief `F174 SPAWN part=0
  fx=(-1554,-227,-19822) a3=ohne [TOD-Bild] -> KRIECH d=252 | Defekt BECKEN d=47` (= Abnahme F414 d=482 / F174 d=783).
* **Beleg (EMZ0.BIN, selbst disassembliert):**
  ```
  ; TOD FUN_80108590 P0
  8010864c: beq a0,zero,0x80108694 / 80108650: addiu a3,sp,24          ; a3 = sp+24 in beiden Zweigen (Delay-Slot)
  80108654: lw v1,408(s1) / 8010865c: lw v0,1420(v1) / 80108664: sh v0,24(sp)
  80108668: lw v0,1424(v1) / 8010866c: addiu a0,zero,8096 / 80108670: addiu v0,v0,300 / 80108674: sh v0,26(sp)
  80108678: lw v0,1428(v1) / 80108680: sh v0,28(sp) / 80108688: lh a1,118(s1) / 80108690: addiu a2,s2,72   ; Part 0
  80108694: addiu a0,zero,6096 / 8010869c: addiu v0,zero,800 / 801086a0: sh zero,24(sp) / 801086a4: sh v0,26(sp)
  801086a8: sh zero,28(sp) / 801086b0: addiu a2,a2,244 (Part 1) / 801086b4: jal 0x8001bf10
  ; Kriecher-Tod FUN_80108A14 P0
  80108a84: addiu a0,zero,8096 / 80108a98: addiu a3,sp,24 / 80108abc: addiu v0,zero,300 / 80108ac0: sh zero,24(sp)
  80108ac4: sh v0,26(sp) / 80108ac8: sh zero,28(sp) / 80108acc: lh a1,118(s0) / 80108ad0: jal 0x8001bf10 / 80108ad4: addiu a2,a2,72
  ; Tod des Liegenden FUN_801099E4 P0
  80109b30: lw v0,1420(v1) / 80109b38: sh v0,32(sp) / 80109b3c: lw v0,1424(v1) / 80109b40: addiu a0,zero,8096
  80109b44: addiu v0,v0,300 / 80109b48: sh v0,34(sp) / 80109b4c: lw v0,1428(v1) / 80109b50: addiu a3,sp,32
  80109b54: sh v0,36(sp) / 80109b5c: lh a1,118(s0) / 80109b60: jal 0x8001bf10 / 80109b64: addiu a2,a2,72
  ; Tropfblut, Tod P1 (ungerade Bilder in [dir*10+4, dir*10+16])
  8010887c: lw v1,408(s1) / 80108884: lw v0,1420(v1) / 8010888c: sh v0,24(sp) / 80108890: lw v0,1424(v1)
  80108898: addiu v0,v0,500 / 8010889c: sh v0,26(sp) / 801088a0: lw v0,1428(v1) / 801088a8: sh v0,28(sp)
  801088b0: jal 0x80015fe8 / 801088b8: sll s0,v0,3 / 801088bc: jal 0x80015fe8 / 801088c0: addiu s0,s0,4048
  801088c8: sll a1,v0,4 / 801088cc: addiu a3,sp,24 / 801088d0: jal 0x8001bf10 / 801088d4: addiu a2,s2,72
  ```
  => TOD Zone != 0: Part 0, a3 = (P8.x, P8.y+300, P8.z) OHNE den Seitenversatz des HURT; Zone 0: Part 1, (0,800,0);
  Kriecher-Tod: Part 0, (0,300,0); Tod des Liegenden: Part 0, (P8.x, P8.y+300, P8.z) ohne Zonen-Weiche; Tropfblut:
  Part 0, (P8.x, P8.y+500, P8.z), Winkel = r2 << 4 (der Port nahm +0x76).
* **Aenderung (enemy_ai_re2_zombie.c):** neuer Kern `re2z_gore_fx_core(..., a3, ofs)` mit `re2z_gore_fx_a3` (Lage ueber
  re15_schuss_r36_blut_anker = Anker.R*a3 + Anker.T, Dekoder/Praesentation unveraendert); re2z_gore_fx_ex bleibt der
  Weg ohne a3. re2z_death_* P0 (TOD) -> `re2z_blood_a3(e, zone, 0, 300, a3)` + gore_fx_a3 (8096 Part 0 / 6096 Part 1);
  re2z_death_crawler -> (0,300,0); re2z_death_lying -> (P8.x, P8.y+300, P8.z); Tropfblut -> (P8.x, P8.y+500, P8.z) und
  Winkel r2 << 4.
* **Messung nachher:** eben `F354 SPAWN part=0 fx=(-1391,-2484,-19813) a3=(-40,-483,0) [TOD-Bild, zone 1] -> d=1 |
  Defekt PART8 d=300` (Becken y -2004: 480 darueber); Tropfblut `F359..F369 a3=(-40,-283,0) -> FLINCH/TROPF d=0..1`;
  tief (Tod des Liegenden) `F186 SPAWN part=0 fx=(-1072,-773,-19784) a3=(-40,-483,0) [TOD-Bild] -> d=1 | Defekt d=300`.
  (Die Bildnummern verschieben sich gegen vorher, weil das Nachbluten jetzt die zwei Original-Wuerfe je Spawn zieht, §8.2.)

### 8.2 M2 — Treffer beim Aufstehen (Part 8, a3 (0,300,0)) + Nebenbefund Stagger-P1-Nachbluten
* **Ursache:** Aufsteh-Treffer `re2z_blood_fx_scaled(e, 8, ..., NULL)` und die HP<0-Variante `re2z_gore_fx_ex(e, 8,
  0x31F40, ..., 0,0,0)` ohne a3. Das Nachbluten des Stagger-P1 war ein Stand-in: JEDES Bild (10 statt 5 Spawns), am
  Becken ohne a3, Skala 0x1500 ohne Beleg, ohne die zwei Wuerfe des Originals (RE2-Zufallsfolge verschoben).
  Das Dossier §2.3 nannte den Aufsteh-Emitter faelschlich "MG-Emitter" (der MG-Handler ist 0x80107EF0).
* **Messung vorher:** eben `F390 SPAWN part=8 fx=(-1060,-1807,-19527) a3=ohne [Treffer] -> AUF d=300 | Defekt PART8 d=53`,
  `F402 ... AUF d=313`; tief `F162 ... AUF d=230 | Defekt PART8 d=91`. Nachbluten eben/tief `F90..F99` = 10 Spawns
  `a3=ohne -> ... | Defekt BECKEN(ohne a3) d=0`.
* **Beleg (EMZ0.BIN):**
  ```
  ; Aufsteh-Treffer FUN_80107A78
  80107a98: sh zero,24(sp) / 80107a9c: sh zero,26(sp) / 80107aa0: sh zero,28(sp)           ; Prolog nullt den Vektor
  80107b14: addiu a0,zero,6096 / 80107b18: addiu a3,sp,24 / 80107b20: addiu v0,zero,300 / 80107b24: sh v0,26(sp)
  80107b28: lh a1,118(s1) / 80107b2c: jal 0x8001bf10 / 80107b30: addiu a2,a2,1448                ; Part 8, a3 (0,300,0)
  80107ba0: lui a0,0x3 / 80107ba4: ori a0,a0,0x1f40 / 80107ba8: sll a1,v0,4 / 80107bac: addiu a3,sp,24
  80107bb0: jal 0x8001bf10 / 80107bb4: addiu a2,s0,1448                                           ; derselbe Vektor
  ; FUN_8001BF10 liest a3 nur: 8001bfe8: lw v0,0(a3) / 8001bff0: sw v0,44(t0) / 8001bff4: lw v0,4(a3) / 8001bffc: sw v0,48(t0)
  ; Stagger FUN_80105BC0 P1 (Einstieg nur bei +0x222 == 1: 80105458: lbu v0,546(s1) / 80105460: bne v0,a0 / 80105468: jal 0x80105bc0)
  80105f28: jal 0x80015e7c (+0x144/+0x146/+0x148 = Wurzel-Delta) / 80105f3c: jal 0x8002959c / 80105f54: jal 0x800152c8
  80105f5c: lh v0,346(s4) / 80105f64: beq v0,zero / 80105f6c: lb v0,362(s4) / 80105f74: beq v0,zero
  80105f78: andi v0,v1,0x1 / 80105f7c: bne v0,zero,0x80105ffc            ; nur GERADES +0x15A spawnt
  80105f84: lbu v0,546(s4) / 80105f8c: bne v0,zero,0x80105fbc            ; +0x222 != 0 -> Zweig B
  A: 80105f94/9c: 2x jal 0x80015fe8 / 80105fa4: addiu a0,zero,8000 / 80105fa8: addiu a2,s3,72 / 80105fac: addiu a3,s4,324
  B: 80105fbc: lhu v0,326(s4) / 80105fc4: addiu v0,v0,-500 / 80105fcc: sh v0,326(s4) / 80105fc8/d0: 2x jal 0x80015fe8
     80105fd8: addiu a0,zero,5096 / 80105fdc: addiu a2,s3,72 / 80105fe0: addiu a3,s4,324
  80105fe8: lh a1,118(s4) ... 80105fec: sll v0,v0,4 / 80105ff0: addu a1,a1,s0 / 80105ff4: jal 0x8001bf10 / 80105ff8: subu a1,a1,v0
  80105ffc: lhu v0,346(s4) / 80106004: addiu v0,v0,-1 / 8010600c: sh v0,346(s4)
  ```
  Im Stagger-Handler 0x80105BC0-0x80106100 wird +0x222 nur bei 0x80105F84 gelesen und erst in P2 geloescht (`sb zero,546`
  @0x80106034) -> im P1 laeuft Zweig B: Id 5096, a3 = (dx, dy-500, dz) mit (dx,dy,dz) = Wurzel-Delta dieses Ticks.
  dy: die Stagger-Clips 3/4 der Hauptbank (der Stagger posiert aus der Hauptbank, re15_re2z_poses_loco_bank: Gang-Paar
  nur in Zustand 1) tragen in JEDEM Bild sy = 0 (gemessen, Sonde `C_schuss_mess/n1/kfspeed.c`: Clip 3 40 Bilder
  `[0:663,0,29] ... [39:1318,0,6]`, Clip 4 44 Bilder `[0:-277,0,-26] ... [43:-1143,0,-49]`) -> dy = 0. (dx, dz) ist die
  Wurzelbewegung, die 0x800152C8 @0x80105F54 VOR dem Spawn auf die Lage anwendet, waehrend der Anker die Matrix des
  Vorbilds ist; der Term setzt das Blut an den bewegten Koerper. Der Port rechnet den Anker im laufenden Bild
  (nach jeder Bewegung dieses Ticks), der Term ist dort enthalten -> (dx, dz) = 0 (PORT-KONVENTION des Ankers, wie bei
  den HURT-Emittern). Restunterschied: Part0.R*(dx,0,dz) gegen Ryaw*(dx,dz) (Beckenneigung), nicht modelliert.
* **Aenderung:** `s_re2z_a3_aufstehen = {0,300,0}` fuer beide Aufsteh-Spawns; `re2z_stagger_bleed`: Gate gerades +0x15A,
  zwei Wuerfe, Winkel +0x76 + (r1<<4) - (r2<<4), Id 5096 (Zweig B) / 8000 (Zweig A), a3 = (0, -500 | 0, 0) ueber
  re2z_gore_fx_a3 (ersetzt den Stand-in re2z_blood_fx_at mit 0x1500).
* **Messung nachher:** eben `F330/F342 SPAWN part=8 a3=(0,300,0) -> AUF d=6 | Defekt PART8 d=295`; tief `F174 SPAWN
  part=8 fx=(-900,-291,-19903) a3=(0,300,0) -> AUF d=32 | Defekt PART8 d=276`. Nachbluten eben/tief `F90/F92/F94/F96/F98`
  = 5 Spawns `a3=(0,-500,0) -> NACH d=1 | Defekt PART8 d=284..285` (Becken y -1967..-1609, Blut 370..500 darueber).
* **Offen dazu (nicht Blut):** der Port fuehrt die Stagger-WURZELBEWEGUNG nicht (re2z_hit_move bewegt nur im
  Gang-Clip, @0x80105B84-90), das Original wendet im P1 jeden Tick e7c + 0x800152C8 an (@0x80105F28-58; Clip 4 bis
  -1143 Wurzel-x). -> Abschnitt 5 OFFEN 7.

### 8.3 M3 — Pin sieht toedlichen Treffer und Part-8-Spawn nicht
* **Ursache:** probe_r36_blut brach beim Tod ab und wertete nur Part 0/1 aus, und nur den LETZTEN Spawn eines Bilds.
* **Aenderung:** Mess-Ring in schuss_r36.c (`re15_schuss_r36_fx_merke/_zahl/_holen`, je ein Haken in
  re2z_blood_fx_scaled und re2z_gore_fx_core): JEDER Spawn mit Part, Lage, a3. probe_r36_blut neu: Schuss alle 12
  Bilder bis zum TOD + 80 Bilder; jeder Spawn an Part 0/1/8 wird gegen die Sollpunkte aller Emitter-Familien (HURT,
  HURT16A, FLINCH/TROPF, TOD, ZONE0, NACH, AUF, KRIECH, LIEGE (EXEC[5] P6 @0x801035B0-DC, a3 = 0, nur in diesem
  Kontext)) und gegen die Defekt-Kandidaten BECKEN/PART1/PART8 (Anker ohne a3) gemessen. Pin: (1) jeder Spawn <= 40 an
  einem Original-Sollpunkt und naeher als jeder Defekt; (2) Todesbild mit TOD/ZONE0/KRIECH <= 40; (3) Part-8-Treffer =
  AUF; (5) y des Versatzes im ANKER-Rahmen (Anker.R^T * (fx - Anker.T)) im Band der Familie (HURT/TOD/NACH < -400,
  FLINCH/TROPF < -200, ZONE0 > 600, AUF/KRIECH > 200) — unabhaengig davon, ob der Koerper steht oder liegt;
  (4) Zaehler: Tod erreicht, >= 1 HURT, >= 1 NACH, >= 1 AUF, eben >= 1 Tropfblut nach dem Tod.
* **Trennschaerfe (vorher):** dieselbe Regel (1) schlug am Stand ohne die Fixes fuer jeden Defekt an — `probe_r36_blut
  pin eben` exit 1 mit 33 FAIL-Zeilen (Nachbluten F90..F99, AUF F390/F402, TOD F414, Tropfblut F419..F429), `pin tief`
  exit 1 mit 23 (`mess_r36c/n1/vorher_pin_*.txt`).
* **Nachher:** `probe_r36_blut pin eben` -> `Treffer 7, Spawns 18, Tod bei Bild 354, Tropfblut nach dem Tod 6` /
  `Zuordnung: HURT=3 HURT16A=1 FLINCH/TROPF=7 NACH=5 AUF=2` / `probe_r36_blut: OK`; `pin tief` -> `Treffer 7, Spawns 12,
  Tod bei Bild 186` / `Zuordnung: HURT16A=1 ZONE0=4 NACH=5 AUF=1 LIEGE(a3=0)=1` / `probe_r36_blut: OK`
  (HURT16A im Todesbild = TOD-Sollpunkt, identische Koordinaten bei +0x16A = 0).
* **Variante mg (Zensus §8.8, MG-Handler 0x80107EF0):** `probe_r36_blut pin mg` — Ingram 12, Leon von +z (Blick -z),
  damit der Zombie nicht mit Gier 0 steht (sonst fielen Welt- und Anker-Versatz fast zusammen). Der MG-a3 ist ein
  Zufallsvektor (sp+16); gepinnt wird je Stoss: a3 im Ring vorhanden und im Band der Zone (Zone != 0 Part 0
  x,z in [-254,256], y in [-708,312] @0x80108000-38; Zone 0 Part 1 x,z in [-128,127], y in [672,927] @0x8010806C-98),
  Lage <= 100 an Anker.R*a3 + Anker.T (Schranke 100 statt 40: der MG-P0 dreht den Zombie NACH dem Spawn um
  +64-(r&0x7f) @0x8010815C-6C und schiebt ihn 20..83 zurueck @0x80108110-48, die Sonde sieht nur T-1/T), und ueber alle
  Stoesse Summe d(Anker-Modell) < 1/2 Summe d(Welt-Versatz); der Lauf endet, sobald der Zombie Kriecher wird (danach
  nur Griff-/Biss-Blut). Vorher (MG-Aufruf voruebergehend auf den alten Welt-Versatz gesetzt, Messung, zurueck):
  `pin mg` exit 1, `probe_r36_blut: 23 FAILURES` (a3 fehlt im Ring, d 184..598). Nachher: `MG: 22 Stoesse, Summe
  d(Anker-Modell) 590, Summe d(Welt-Versatz) 6665` / `probe_r36_blut: OK` (je Stoss d 11..53, Welt-Modell 64..415).

### 8.4 M4 — integration_r36_schuss: treffbare Ziele hinter dem Tisch
* **Ursache:** unter RE2-KI stehen hinter dem Konferenztisch nur gesperrte Fresser (Plaetze 2..5) und der Liegende;
  die Fresser-Sperre allein haelt jeden Treffer ab (Lauf "fresser"), "tisch"/"gl_tisch" waeren auch ohne Sichtlinie gruen.
* **Aenderung (tests/integration/test_r36_schuss.cmake):** r36_exe bekommt die KI-Variante als Parameter; drei neue
  Laeufe unter `RE15_AI_FLAVOR=re15`, wo die Fresser treffbar sind (RE1.5-Kandidatenschleife FUN_80011f50 gatet nur
  `+0x93 & 3` @0x800120f4-100, die RE1.5-Fresser tragen +0x93 = 0 — Savestate-Befund OFFEN 1):
  `frei_re15` (Leon (200,-18200) hinter dem Tisch, Python) MUSS Platz 4 UND 5 treffen (sonst waere der Tisch-Lauf ohne
  Aussage), `tisch_re15` (Leon (200,-10300), Python) und `gl_tisch_re15` (Granatwerfer 15) — KEIN Platz verliert HP.
* **Messung (ctest -R "^integration_r36_schuss$", LastTest.log):** `[frei_re15]: Platz 4 HP 250 -> -1 (treffbar)`,
  `Platz 5 HP 250 -> -1`; `[tisch_re15]: Platz 1 HP 65 -> 65 ... Platz 4 HP 250 -> 250, Platz 5 HP 250 -> 250`;
  `[gl_tisch_re15]`: alle fuenf unveraendert; die RE2-Laeufe unveraendert (`[tisch]` 79/50/80/250/250 gleich,
  `[frei]: Platz 5 HP 250 -> -1`). `1/1 Test #573: integration_r36_schuss ... Passed 129.42 sec`.
  => Bei TREFFBAREN Zielen hinter dem Tisch sperrt allein der Tisch (Python und Granatwerfer); die Sichtlinie ist damit
  an der exe gepinnt, nicht nur ueber unit_r36_schuss A.

### 8.5 M5 — Zitate (+0x10E 0x4004, a3-Lage RE2)
* **Ursache:** zwei falsche Zitate. (a) Das `sh 0x4004,270` @0x80100A88-8C wurde dem FRESSER zugeschrieben; (b) die
  a3-Lage wurde mit @0x8001a248 belegt (RE1.5-ESP-Maschine, in der RE2-EXE ein Anim-Setter).
* **Beleg (selbst disassembliert, info/re2leon/COMMON/BIN/EMZ0.BIN bzw. PSX.EXE):**
  ```
  ; Spawn-Remap, Deskriptor 7 (= +0x10E & 0x3F)
  80100a64: lhu v0,270(s2) / 80100a68: addiu v1,zero,7 / 80100a6c: andi v0,v0,0x3f / 80100a70: bne v0,v1,0x80100aa8
  80100a74: addiu v0,zero,23 / 80100a7c: sw v0,332(s2)        ; Clip 23
  80100a80: addiu v0,zero,1793 / 80100a84: sw v0,4(s2)        ; 0x701 = EXEC[7]
  80100a88: addiu v0,zero,16388 / 80100a8c: sh v0,270(s2)     ; +0x10E = 0x4004   <- gehoert zu Deskriptor 7
  80100a90: addiu v0,zero,-1 / 80100a94: sh v0,342(s2)        ; HP = -1
  ; Deskriptor 8 = der FRESSER
  80100ac0: lhu v0,270(s2) / 80100ac4: addiu v1,zero,8 / 80100ac8: andi v0,v0,0x3f / 80100acc: bne v0,v1,0x80100ae0
  80100ad0: addiu v0,zero,18 / 80100ad4: sw v0,332(s2)        ; Clip 18
  80100ad8: addiu v0,zero,2049 / 80100adc: sw v0,4(s2)        ; 0x801 = EXEC[8] — KEIN Schreiben auf +0x10E
  ; RE2 FUN_8001BF10: a3 wird nur GELESEN
  8001bfe0: beq a3,zero,0x8001c000 / 8001bfe8: lw v0,0(a3) / 8001bff0: sw v0,44(t0) / 8001bff4: lw v0,4(a3) / 8001bffc: sw v0,48(t0)
  ; RE2-Lagen-Tick 0x8001D894
  8001db74: addiu v1,v0,76 (RT = Record+76, ctc2 0x48cc0000..0x48ce2000) / 8001dba0: addiu v0,v0,44 (V0 = a3, lwc2)
  8001dbb4: MVMVA 0x4a486012 / 8001dbd0: lw v1,96(a0) ... 8001dc14: addu v0,v0,v1 / 8001dc1c: sh v0,56(a0)   => Anker.R*a3 + Anker.T
  ```
  => Der RE2-Fresser wird NUR ueber Gate 2 (+0x1D3 @0x80041270-78) gesperrt. Das 0x4004 des Port-Fressers stammt aus
  der aelteren Port-Abbildung in re2z_init (`e->re2z_f10e = 0x4004u` mit dem Zitat @0x80100A88-8C, nicht von Spur C,
  unveraendert gelassen: das Bit 0x4000 traegt im Port die Wecker-Kette des Fressers, Eigentuemer Spur F). Gate 4
  ist fuer ihn im Port nur zusaetzlich; die Pruefung 52 von unit_r36_schuss (nur +0x1D3 = 0x80, +0x10E = 0x0004) ist
  der Zustand des RE2-Fressers und sperrt bereits.
* **Aenderung (nur Kommentare/Doku):** enemy_ai_re2_zombie.c re15_re2z_hit_filter_apply (Block "Runde 36 Spur C") +
  Kopfkommentar von re2z_blood_fx_scaled; test_r36_schuss.c (Kopf, Teil B, Pruefungen 52/53); test_re2_zombie_hitfilter.c
  (4a); include/re15_schuss_r36.h (Punkt 2: FUN_8001BF10 @0x8001BFE8-FFC + Lagen-Tick @0x8001DB74-DC1C);
  schuss_r36.c (R_part * a3); probe_r36_blut.c (Kopf). Dossier §1.2 und §1.5 bleiben als Verlauf stehen und sind
  durch diesen Abschnitt berichtigt.

### 8.6 M6 — "C-Blut": Bisect / Regressionsaussage
* **Aussage: KEINE Regression aus v0.8.22/23.** Das Blut sitzt seit 2026-08-20 am Becken.
* **Beleg 1 (Abnahme 0, nicht wiederholt):** `git log 9362dd7e..3ec5b1f8 -G "re2z_blood_fx|re2z_blood_anchor|re2z_gore_fx_ex|
  re15_esp_fx_spawn_ex"` ueber enemy_ai_re2_zombie.c/re15_damage.c/re15_esp.c/skeleton_common.c ist leer.
* **Beleg 2 (neu, funktionsgenau, `git log -L :<funktion>:<datei> 9362dd7e..3ec5b1f8`):** 0 Commits fuer JEDE Funktion
  der Blut-Lage: re2z_blood_fx_scaled, re2z_gore_fx_ex, re2z_blood_fx_dir (enemy_ai_re2_zombie.c),
  re15_enemy_bone_world_pos (re15_damage.c), re15_esp_fx_spawn_ex, re15_esp_fx_spawn_rows (re15_esp.c),
  re15_skel_bone_to_world (skeleton_common.c). (Gegenprobe der Abfrage: master..HEAD liefert df7ecdc5.)
* **Herkunft:** `git log -S "Der Stand-in war POSITIONSLOS"` -> 76d63ac0 (2026-08-20, "fix(#re2): drei Nutzer-Befunde —
  Blut am Fuss, ..."): der Anker wurde von der Aktor-Wurzel (Fuss) auf die Knochen-WELTPOSITION gesetzt, OHNE den
  a3-Versatz (damals ausdruecklich "⛔ OPEN: die Original-Geschwindigkeit a3 ... hat im positionslosen Port-Stand-in
  keinen Konsumenten"); `git log -S "re15_enemy_bone_world_pos(e, re2z_part_to_bone(e, part), p)"` -> ee97f515
  (2026-08-20, in v0.3.5 enthalten) und 18d7eff4 (2026-09-12, Dekoder re2z_gore_fx_ex). Der Befund des Nutzers
  ("manchmal") ist also der seit v0.3.5 fehlende a3-Versatz, kein Bruch in v0.8.22/23.

### 8.7 M7 — Messkopie der exe im Integrationstest
* **Aenderung:** test_r36_schuss.cmake kopiert `$<TARGET_FILE:re15_pc>` nach `re15_pc_r36c_haken_<id><ext>` neben die
  exe (Asset-Wurzel = exe-Verzeichnis; `<ext>` = LAST_EXT, ".exe" unter Windows, leer unter Linux), raeumt Reste
  abgebrochener Laeufe vorher weg und loescht die Kopie am Ende (Muster test_r34n_b_cursor.cmake).
* **Messung:** Lauf oben gruen, danach `ls re15_port/build/platform/pc | grep -c r36c_haken` = 0.

### 8.8 Zensus aller FUN_8001BF10-Aufrufe in EMZ0.BIN (Punkt 2 vollstaendig statt drei Emitter)
Die Abnahme fand zwei Emitter-Familien, die der Bau-Agent uebersehen hatte; die neue Sonde fand im Lauf "tief" eine
dritte (Tod des Liegenden). Deshalb ein vollstaendiger Zensus: alle 88 Stellen `jal 0x8001bf10` (Wort 0x0C006FC4) in
info/re2leon/COMMON/BIN/EMZ0.BIN, je Stelle a0/a2/a3 rueckwaerts bis zum vorigen `jal` (Skript und Rohliste
`C_schuss_mess/n1/zensus2.py` / `zensus2.txt`), dann gegen den Port-Aufruf (Adresse im Port-Kommentar) gestellt.

**A — Blut/Gore am getroffenen bzw. sterbenden Zombie mit a3 != 0 (27 Stellen), jetzt alle Anker.R*a3 + Anker.T:**
| Stelle(n) | Handler | a3 im Original (Anker) | Port vorher | Stand |
|---|---|---|---|---|
| 0x8010510C, 0x80105154 | Wurzel-Flinch | Zone 0 P1 (0,800,0); sonst P0 (P8.x, P8.y+500, P8.z) | ohne | Bau-Agent (§2) |
| 0x80105708 | Haupt-Handler P0 | Zone 0 P1 (0,800,0); sonst P0 (P8.x-100s, P8.y+300, P8.z) | ohne | Bau-Agent |
| 0x80105D80, 0x80105DB8 | Stagger P0 | dito, s = +0x16A | ohne | Bau-Agent |
| 0x80105FF4 | Stagger P1 Nachbluten | +0x144-Vektor, y-500 (Zweig B) | jedes Bild, ohne | N1 §8.2 |
| 0x80107B2C, 0x80107BB0 | Aufsteh-Treffer | P8 (0,300,0) | ohne | N1 §8.2 |
| 0x801080C4 | MG-Handler 0x80107EF0 | Zone!=0 P0 / Zone 0 P1, Zufallsvektor sp+16 | Welt-Versatz | N1 (dieser Abschnitt) |
| 0x801086B4 | TOD | Zone!=0 P0 (P8.x, P8.y+300, P8.z); Zone 0 P1 (0,800,0) | ohne | N1 §8.1 |
| 0x801088D0 | Tropfblut Tod P1 | P0 (P8.x, P8.y+500, P8.z), Winkel r<<4 | ohne, +0x76 | N1 §8.1 |
| 0x80108AD0 | Kriecher-Tod | P0 (0,300,0) | ohne | N1 §8.1 |
| 0x80109024, 0x80109054 | RIP 0x80108BEC P0 | P0 (0,-500,0) fuer BEIDE | Welt / ohne | N1 |
| 0x80109108, 0x8010913C | RIP P1 | +0x144 (Bewegung des Ticks) | ohne | Port-Konvention: Anker im laufenden Bild enthaelt die Bewegung (§8.2) — unveraendert |
| 0x801093D4, 0x80109420, 0x80109438 | MAGNUM 0x801092C4 | P0 (P8.x, P8.y+300, P8.z), dann y-400 fuer 2 und 3 | Part 8 + Welt | N1 |
| 0x80109710, 0x80109734 | BURST 0x80109610 | P0 (0,-300,0) / (0,-100,0) | Welt | N1 |
| 0x801097A4 | BURST Fleischbrocken | P0 (256-2r, 56-2r, 256-2r) | ohne ("Start-v") | N1 |
| 0x80109934 | BURST Tropfblut | P0 (P8.x, P8.y+500, P8.z) | Welt (0,+500,0) = 500 UNTER dem Becken | N1 |
| 0x80109B60 | Tod des Liegenden 0x801099E4 | P0 (P8.x, P8.y+300, P8.z) | ohne | N1 §8.1 |
| 0x801076A4, 0x801076C4, 0x801076E4 | Arm-Abriss | P3 (0,0,0) / (0,400,0) / (0,800,0), Winkel 0 / r<<2 / r<<4 | Welt, +0x76 | N1 |
Belege der N1-Zeilen dieser Tabelle, die nicht in §8.1/§8.2 stehen (EMZ0.BIN, selbst disassembliert):
```
; MG 0x80107EF0: Zone!=0 sh v0,16(sp) @0x80108014 / sh v1,18(sp) @0x80108028 / sh s0,20(sp) @0x80108038 / addiu a3,sp,16 @0x80108058
;                Zone 0  sh v0,16(sp) @0x8010807C / sh v1,18(sp) @0x8010808C / sh s0,20(sp) @0x80108098 / addiu a3,sp,16 @0x801080B8
;                addiu a2,s1,244 @0x801080C0 (Zone 0) bzw. addiu a2,s1,72 @0x80108064, jal 0x8001bf10 @0x801080C4
; RIP: 80108fe4: addiu v0,zero,-500 / 80108fe8: sh zero,24(sp) / 80108fec: sh v0,26(sp) / 80108ff4: sh zero,28(sp)
;      80109004: addiu s2,sp,24 / 80109008: addu a3,s2,zero / 80109024: jal 0x8001bf10 / 80109054: jal 0x8001bf10 / 80109058: addu a3,s2,zero
; MAGNUM: 801093a0: lw v0,1420(v1) / 801093a4: addiu s2,sp,24 / 801093a8: sh v0,24(sp) / 801093ac: lw v0,1424(v1)
;      801093b4: addiu v0,v0,300 / 801093b8: sh v0,26(sp) / 801093bc: lw v0,1428(v1) / 801093c4: sh v0,28(sp)
;      801093d0: addiu s1,s1,72 / 801093d4: jal 0x8001bf10 / 80109414: lhu v0,26(sp) / 8010941c: addiu v0,v0,-400
;      80109420: jal 0x8001bf10 / 80109424: sh v0,26(sp) / 80109438: jal 0x8001bf10 / 8010943c: addu a3,s2,zero
; BURST: 801096e4: addiu s1,sp,24 / 801096f0: addiu s5,zero,256 / 801096f8: addiu v0,zero,-300 / 801096fc-704: sh zero/v0/zero,24/26/28(sp)
;      80109710: jal 0x8001bf10 / 80109720: addiu v0,zero,-100 / 80109724-2c: sh / 80109734: jal 0x8001bf10 / 80109738: addu a3,s1,zero
;      80109744-50: sll v0,v0,1 / subu v0,s5,v0 / sh v0,24(sp) ; 80109758-64: addiu v1,zero,56 / subu / sh v1,26(sp)
;      80109768-74: subu v0,s5,v0 / sh v0,28(sp) ; 80109794: addiu a3,sp,24 / 801097a4: jal 0x8001bf10
;      801098e8-90c: lw 1420 / sh 24 / lw 1424 / addiu v0,v0,500 / sh 26 / lw 1428 / sh 28 ; 80109930: addiu a3,sp,24 / 80109934: jal
; Arm-Abriss: 80107684: addu a1,zero,zero / 80107688: addiu s1,sp,24 / 80107694-9c: sh zero,24/26/28(sp) / 801076a0: addiu s0,s0,588
;      801076a4: jal / 801076ac: addiu v0,zero,400 / 801076b4: sh v0,26(sp) / 801076bc: sll a1,v0,2 / 801076c4: jal
;      801076cc: addiu v0,zero,800 / 801076d4: sh v0,26(sp) / 801076dc: sll a1,v0,4 / 801076e4: jal / 801076fc: sll a1,v0,4 / 80107704: jal (a3 0)
```
**B — a3 = 0 bzw. Nullvektor (30 Stellen, Lage = Anker.T, Port ohne Versatz = deckungsgleich):** 0x80104DF4, 0x80105368,
0x801053D0, 0x80105ED0, 0x80105F0C, 0x801061B0, 0x801061F0, 0x80106390, 0x801063C0, 0x801063F0, 0x80106684, 0x801068D8,
0x80106908, 0x80106940, 0x80106D7C, 0x801071A0, 0x801075E0, 0x80107600, 0x8010765C, 0x80107704, 0x8010841C, 0x80108780,
0x80108E28, 0x80109800, 0x80109C1C, 0x8010BF68, 0x8010C07C, 0x8010C514; Nullvektor: 0x801035D8 (EXEC[5] P6, `sh zero,
24/26/28(sp)` @0x801035C8-D0; die Sonde fuehrt ihn als LIEGE), 0x80107938. (Die Port-Zwillinge ankern ueber
re15_enemy_bone_world_pos = rohe Pose statt gezeichneter; Unterschied nur waehrend eines Crossfades, s. OFFEN 11.)
**C — kein Blut am getroffenen Zombie (14 Stellen):** Griff/Biss 0x80102B24, 0x80102BBC; Fressen 0x80103C8C; Feuer
(Id 5) 0x80106180, 0x80106220, 0x80106254, 0x80106440, 0x80106470; Funken/Spark (Id 6, im Port ohne Spawn)
0x80106574, 0x801065C0, 0x8010660C, 0x80106658, 0x801066AC, 0x801066D4. Dieselbe a3-Semantik, im Port als Welt-Versatz
bzw. ohne -> OFFEN 9.
**D — im Port ohne Zwilling (17 Stellen):** 0x80106A7C (Ragdoll P1 Nachbluten alle 4 Bilder, a3 = +0x144, Id
(r<<3)+6000 — fehlt im Port, OFFEN 8) und 16 Stellen ab 0x8010A1EC (0x8010A1EC, 0x8010A318, 0x8010A33C, 0x8010A354,
0x8010AC44, 0x8010AD70, 0x8010B188, 0x8010B244, 0x8010B668, 0x8010B6A8, 0x8010B6D0, 0x8010C1D0, 0x8010C208, 0x8010C240,
0x8010C278, 0x8010C56C), deren Adressen der Port nirgends nennt (OFFEN 10).
Summe 27 + 30 + 14 + 17 = 88.

### 8.9 Gegenmessung an der exe (Punkt 2, derselbe Aufbau wie die Abnahme 0)
Skript `C_schuss_mess/n1/run_n1_p2.sh` (Aufbau = `run_abn_p2.sh` der Abnahme): ROOM1140 per RE15_DEBUG_JUMP, Leon
(-5000,-19600) Blick +x, Pistole, RE2-KI, Dauerfeuer `W1.2,M0.6,MA9.0,M0.5,W2`, RE15_FX_LOG + RE15_STATE_LOG, Messkopie
`re15_r36c_n1.exe` (danach geloescht), `exit=0`. Auswertung `C_schuss_mess/n1/fx_auswertung.py` (erste Zeile je Blut-Effekt id 0 / frame=1);
vorher = Roh-Logs der Abnahme (`abn/p2/tod`, nicht neu gefahren). Die Treffer liegen in beiden Laeufen auf denselben
Bildern und Lagen (state.log Platz 2: F55 50->34, F77 ->18, F99 ->2, F121 2->-14 st 3), die Lauf-Folge ist also gleich:
| Bild | Emitter (state.log) | vorher (Abnahme) | nachher | Befund |
|---|---|---|---|---|
| F55 | Treffer im Aufstehen aus dem Fressen (F54 `st=1 ss1=8 ss2=4` -> F55 `st=2 ss1=3`) = FUN_80107A78 | `w(-1733,-2445,-19605)` | `w(-1781,-2163,-19518)` | 282 tiefer = Part8.R*(0,300,0) — M2 (die Abnahme hielt F55 fuer den Brust-Treffer; es ist der Part-8-Emitter) |
| F77 | Haupt-Handler (HURT) | `w(-1723,-2140,-19787)` | `w(-1723,-2140,-19787)` | gleich (Bau-Agent-Fix, unberuehrt) |
| F99 | Zone-0-Treffer | `w(-1867,-355,-19977)` | `w(-1867,-355,-19977)` | gleich |
| F121 | TOD (2 -> -14, st 3) | `w(-1907,-1808,-20497)` | `w(-1862,-2176,-20446)` | 368 hoeher = Brust statt Becken — M1 |
| F126..F136 | Tropfblut Tod P1 (ungerade Bilder) | y -1747 .. -1053 | y -1934 .. -1199 | je ~190-280 hoeher (P8.y+500 = 283 ueber dem Becken im Becken-Rahmen) |

### 8.10 Bestehende Riegel, die die Nachbesserung rot machte (je Test entschieden, wie §1.5)
Erster gezielter Lauf (188 Tests, Regex `mess_r36c/n1/regex.txt`, -j 2, 648 s): `99% tests passed, 2 tests failed out
of 188` — `unit_re2_zombie_teardeath`, `unit_r26_mg_blut`. Beide aus der Nachbesserung erklaert, keiner ist eine
Regression des Verhaltens, das sie pruefen:
| Test | Rot weil (gemessen) | Entscheidung |
|---|---|---|
| unit_re2_zombie_teardeath (PIN7 w12) | `PIN7 Waffe 12 -> cell=0 ... end(st=7 hp=-1 10E=2001) hits=13 f=2599`: der Kampf endete im KRIECHER-GRIFF-TOD (RE15_T20-Spur: Griff f160, Abwurf, Kriecher 10E 0x2001 f460, Griff, st 7 hp -1 f520 = FUN_801025EC P6 @0x80102BE8, `sw 7,4` @0x80102BF4 / `sh -1,342` @0x80102BFC) — kein Schuss-Tod, also nie Zustand 3. Bisect an der Engine: Nachbluten auf den alten Stand -> gruen (`cell=1 row=15 ... f=471`); neue Wuerfe, aber ohne Anker-Rechnung (gore_fx_ex) -> rot wie oben. Ursache = die zwei Original-Wuerfe des Nachblutens (@0x80105FC8/D0): die w3/w4-Kaempfe davor verbrauchen jetzt mehr Wuerfe, der w12-Kampf beginnt mit einer anderen Zufallslage. MG-Aufruf auf Welt-Versatz zurueck -> weiter rot (nicht der MG-Fix). | Fixture neu verankert: jeder PIN7-Kampf startet mit `re15_re2z_rng_reset()` (Zufallslage des Raumladens, wie actor_common.c beim Raumwechsel) — die Wache haengt nicht mehr davon ab, wie viele Wuerfe ANDERE Pins ziehen. Ein erster Versuch mit Wiederholung bei Griff-Tod (Vorlauf +20) brachte im zweiten Anlauf `hits=0` (verworfen). Ergebnis: alle vier Waffen `cell=1` (w3 f519, w4 f519, w12 `row=15 ... hits=13 f=506`, w19 `row=18 ... f=405`), `test_re2_zombie_teardeath: OK`; Pruefungen unveraendert |
| unit_r26_mg_blut | `(1) MG-Blutstoss belegt 10 Slots ... (5) 64/64 Stoesse sitzen ohne Versatz auf dem Anker ... (6) scale ist konstant (8000)`: der Riegel laeuft OHNE Gegner-Bank; re15_schuss_r36_blut_anker liess a3 ohne Bank ganz weg, der MG-Stoss lag auf der Wurzel und slots_bei() zaehlte fremde Spawns mit. | Engine: ohne Bank Anker.T = Aktor-Wurzel (re15_enemy_bone_world_pos), R_part = Einheit, a3 im Aktor-Rahmen (Ryaw) — im Spiel traegt jeder RE2-Zombie seine Bank, der Zweig betrifft nur Sonden. Riegel: Nullpunkt fuer (5) ist die Wurzel (1000,0,2000) des Treffer-Aktors statt des Pistolenstosses, der seit Runde 36 selbst a3 = P8 + (-100*s, 300, 0) traegt (@0x80105680-B4). Ergebnis: `64 Treffer: 0/64 auf dem Anker, 0/64 ausserhalb des Bandes, 63/63 Wechsel, scale 7024..9016` / `=== RIEGEL r26: GRUEN ===` |

### 8.11 Gezielte Tests (Abschluss Nachbesserung 1, keine volle Suite im Spurbaum)
* Regex (`mess_r36c/n1/regex2.txt`) = eigene Tests (r36) + die Gruppen aller beruehrten Module (enemy_ai_re2_zombie.c:
  zombie|re2z|re2_|gore|blood|blut|hit_|death|crawl|stagger|knockdown|writher|...; schuss_r36.c/re15_damage.c-Haken:
  damage|aim|shootable|waffen|werfer|autofire|1140|...; Raeume 1030/1040/10d0/1100/1210; Riegel r16_/r26_mg/r35_*/
  r30_hund/redhawk/r17_/zfall/situp/fx_region/granate):
  ```
  r36|crow|kraehe|hitfilter|zombie|re2z|re2_|1140|1030|1040|10d0|1100|1210|damage|werfer|dog_hit|spider|gator|birkin|g5_|shootable|hit_|autofire|waffen|blood|blut|gore|feeder|straight_shot|enemy_ai|combat|aim|r16_|rise_|knockdown|stagger|death|crawl|writher|r26_mg|r35_inhalt|r35_raeume|r35_cut1150|r35_zgirl|r30_hund|redhawk|r17_|zfall|situp|fx_region|granate
  ```
  `ctest --test-dir re15_port/build -R "<regex>" --timeout 600 --output-on-failure -j 2` (Endstand, nach §8.10):
  **`100% tests passed, 0 tests failed out of 199`** (`Total Test time (real) = 882.18 sec`, Log `mess_r36c/n1/ctest_gezielt2.log`).
  Darin u.a. integration_r36_schuss (8 Laeufe), unit_r36_blut_pin_eben/_tief/_mg, unit_r36_schuss, unit_r36_schuss_kraehe,
  unit_re2_zombie_teardeath, unit_r26_mg_blut, unit_re2_zombie_hitfilter/_abc, unit_re2_gore*, integration_r35_werfer*.
* Davor (erster gezielter Lauf, 188 Tests, vor §8.10): `99% tests passed, 2 tests failed out of 188` — die zwei Riegel aus §8.10.

### 8.12 Stand je Mangel
| Mangel | Stand |
|---|---|
| M1 toedlicher Treffer | behoben + erweitert: TOD (Zone!=0/0), Kriecher-Tod, Tod des Liegenden, Tropfblut; Sonde + exe (F121 y -1808 -> -2176) |
| M2 Aufsteh-Treffer + Nachbluten | behoben: Part-8-a3 (beide Spawns), Nachbluten byte-true (gerades +0x15A, zwei Wuerfe, Id 5096, a3 y-500); exe F55 282 tiefer |
| M3 Pin | probe_r36_blut neu (Ring, bis zum Tod, alle Familien, Anker-Rahmen), vorher rot (33/23 FAIL), nachher gruen; + Variante mg |
| M4 Tisch-Trennschaerfe | integration_r36_schuss + frei_re15/tisch_re15/gl_tisch_re15 (treffbare Ziele), gruen |
| M5 Zitate | berichtigt (Deskriptor 7/8 @0x80100A64-DC, Lagen-Tick @0x8001DB74-DC1C), selbst disassembliert |
| M6 Regressionsaussage | keine Regression aus v0.8.22/23 (git log -L je Funktion: 0 Commits), Herkunft 76d63ac0/ee97f515 (2026-08-20) |
| M7 Messkopie | re15_pc_r36c_haken_<id> neben der exe, am Ende geloescht |
| dazu (Zensus §8.8) | MG, MAGNUM, RIP, BURST (inkl. Tropfblut 500 UNTER statt 283 ueber dem Becken), Arm-Abriss: a3 im Anker-Rahmen; offen: §5 OFFEN 7-11 |

Commits der Nachbesserung 1 (Zweig r36/schuss): 62cc4289 (M5/M6), a752ff99 (M1/M2/M3), 63d89d9a (M4/M7), c26c5f5a
(Zensus: MG/MAGNUM/RIP/BURST/Arm-Abriss), f20967a4 (Dossier), fc786782 (Riegel teardeath/r26, Anker ohne Bank),
97b41606 (fix, Abschluss Code), ee8eabbb (Mess-Skripte), dieser Abschluss-Commit.

## 9. Nachbesserung 2 (nach Abnahme 1, `C_abnahme_1.md` §7 M1)

Stand: Fortsetzung im selben Baum ab 6fa888c2 (Abnahme 1). Einziger Mangel M1 (Punkt 2): die Emitter der Zensus-Gruppe B
(a3 = 0, §8.8) ankern an der ROHEN Keyframe-Pose (re2z_gore_fx_ex/re2z_gore_fx -> re15_enemy_bone_world_pos,
`g_anim_pose_actor = NULL`), die a3-Emitter an der gezeichneten. Punkte 1 und 3 (von der Abnahme erfuellt) bleiben
unberuehrt; ihre Riegel muessen gruen bleiben. Vorher-Messung = die der Abnahme (`abn1C/blut_w*.txt`, nicht wiederholt);
eigene Mess-Sonde und Logs im Scratchpad `c_nb2_r36/` (Kurzform `nb2/...`).
Gliederung (wird fortlaufend gefuellt): 9.1 Ursache / 9.2 Messung vorher / 9.3 RE-Beleg / 9.4 Aenderung /
9.5 Messung nachher / 9.6 Tests / 9.7 OFFEN + Fuer den Nutzer.

### 9.1 M1 — Ursache (Code gelesen + gemessen)
* `re2z_gore_fx_core` (enemy_ai_re2_zombie.c) nahm fuer a3 == NULL `re15_enemy_bone_world_pos` = ROHES Keyframe der
  Hauptbank (QUERY, `g_anim_pose_actor = NULL`, re15_damage.c:3205-3210) — Zweig fuer re2z_gore_fx_ex / re2z_gore_fx und damit
  fuer alle Gruppe-B-Stellen (§8.8 B) samt RIP-P1-Nachbluten @0x80109108/@0x8010913C (a3 = +0x144, Port-Konvention §8.2).
* Dabei zeigte die eigene Messung (9.2) zwei weitere Abweichungen DESSELBEN Ankers, die die Metrik der Abnahme nicht sehen konnte
  (sie rechnet die "gezeichnete" Pose wie re15_schuss_r36_blut_anker aus der Hauptbank):
  (a) **Bank:** re15_schuss_r36_blut_anker posiert IMMER `b->anim`/`b->skel` (Paar 2) mit clip_override -1; der Renderer
      (platform/pc/main.c:9798-9925) zeichnet in den Paar-1-Phasen die Loco-Bank (`re15_actor_uses_loco_bank`, main.c:9832-9843;
      hit_MAIN P1/P2 @0x80105790/@0x801058C0 u.a., Regel re15_re2z_poses_loco_bank) und loopt bei +0x1C4 & 4 (clip_override,
      re15_actor_anim_select). Folge: auch a3-Emitter (HURT/ZONE0) lagen dort 56-121 neben dem GEZEICHNETEN Koerper.
  (b) **Freifliegendes Teil:** das Aufschlag-Blut der Teile-Physik (FUN_80028AD8 @0x80028C4C, Port re2z_part_phys_ad8 ->
      `re2z_gore_fx(e, i, 2000u)`) nahm den Skelett-Knochen des Rumpfs statt der eigenen Matrix des fliegenden Teils —
      das Blut erschien am Koerper (Armhoehe), das Teil lag 1900-2400 entfernt am Boden.
  (c) **Ragdoll:** im Zweig `re2z_rag_anchor_on` blieb der Anker roh (Crossfade fehlt) — 98 neben dem gezeichneten Becken.

### 9.2 Messung vorher (Stand 6fa888c2 = Code 97b41606)
* **Abnahme 1 (nicht wiederholt, `abn1C/blut_w*.txt`, Metrik "Spawn gegen Hauptbank-Pose des Bilds"):** SPAS-RIP-Kopf
  @0x80108E28 442-443, RIP-P1 @0x80109108/3C 109, Ingram-Gliedabriss @0x80105368/@0x801053D0 168/140, Arm-Abriss 4. Spawn
  @0x80107704 37.
* **Eigene Sonde `nb2/nb2_blut.c`** (Kopie im Repo `C_schuss_mess/n2/`): Aufbau wie abn1_blut.c, aber die Referenz ist ein
  ZWILLING DES PORT-RENDERERS (Bankwahl main.c:9798-9925, Crossfade mit g_anim_pose_actor = Aktor einmal je Bild, Unterhaelfte
  Bones 1..6 ueber re15_re2z_lower_kf, Ragdoll-Bodenanker, freifliegende Teile ueber re15_re2z_gore_part_matrix samt Physik —
  Spawns waehrend des Zeichnens werden mitgezaehlt, Spalte `Z`). d_r = Abstand Spawn gegen R*a3 + T dieses Zwillings im selben
  Bild; d_alt = Metrik der Abnahme. Ausgaben `nb2/vorher/w<waffe>_<seite>_<hoehe>.txt`, woertlich:
  ```
  w13 px eben (SPAS, RIP-Tod F66):
  F66   TOD   ... bank haupt fl 1 | part  8 fx=(-2326,-2528,-19705) a3=(0,0,0) (ohne) | d_r 326 | d_alt 504   <- Kopf @0x80108E28
  F66   TOD   ... bank haupt fl 0 | part  0 fx=(-1886,-1882,-19643) a3=(0,0,0) (ohne) | d_r 80 | d_alt 80     <- RIP-P1 (2x)
  F78       Z ... bank haupt fl 1 | part  9 fx=(-2902,-1853,-20316) a3=(0,0,0) (ohne) | d_r 1902 | d_alt 0  <- Aufschlag @0x80028C4C
  F78       Z ... bank haupt fl 1 | part 12 fx=(-2776,-2217,-19570) a3=(0,0,0) (ohne) | d_r 2280 | d_alt 0
  F84       Z ... part  9 ... d_r 2063 / part 12 ... d_r 2408
  w12 pz eben (Ingram): F76 part 12 (ohne) d_r 204 | F76 part 13 (ohne, fl 1) d_r 108 | F89/F94 Z part 13 d_r 2124/2201
  w8 pz tief (Remington Bein): F66 part 3 a3=(0,0,0) (ohne) d_r 92 (die drei a3-Spawns desselben Parts: d_r 10)
  w8 px eben (Remington, BURST/Ragdoll): F86 rag 1 part 1 (ohne) d_r 98 | d_alt 357
  w3 px eben (Pistole): F66 TREF st 1->2/3/1 bank loco | part 0 a3=(-140,-483,0) | d_r 103 | d_alt 1
                        F78 TREF st 2->2/3/1 bank loco | part 0 a3=(-140,-483,0) | d_r 121 | d_alt 1
  w3 pz tief (Pistole Beine): F66/F78 bank loco | part 1 a3=(0,800,0) | d_r 56 / 110 | d_alt 1 / 1
  ERGEBNIS-Zeilen: w13 px eben "ohne a3 2408 (>40: 7)", w12 pz eben "ohne a3 2201 (>40: 4)", w3 px eben "mit a3 121 (>40: 2)"
  ```
  (MG w12: a3-Spawns d_r 30-72 = d_alt; der MG-P0 dreht/schiebt den Zombie NACH dem Spawn @0x8010810C-6C, Abnahme 1 §3.1 —
  keine Anker-Abweichung. MAGNUM w7: d_r 1.)

### 9.3 RE-Beleg (selbst disassembliert, re2_disasm.py; RE2 info/re2leon/PSX.EXE bzw. COMMON/BIN/EMZ0.BIN)
```
; (1) FUN_8001BF10: a3 = 0 nullt nur den Versatz, der Anker ist derselbe a2 wie bei den a3-Emittern
8001bfe0: beq a3,zero,0x8001c000 / 8001bfe8: lw v0,0(a3) / 8001bff0: sw v0,44(t0) / 8001bff4: lw v0,4(a3) / 8001bffc: sw v0,48(t0)
8001c000: sw zero,44(t0) / 8001c004: sw zero,48(t0)                        ; a3 == 0 -> Versatz 0
8001c03c: lw v0,0(a2) ... 8001c05c: lw v0,16(a2) / 8001c060: lw v1,20(a2) / 8001c064: lw a0,24(a2) / 8001c068: lw a1,28(a2)
8001c04c..8001c078: sw -> slot+76..+104                                    ; 32 Byte = MATRIX{m[3][3]; t[3]} aus a2 (t @+20)
; (2) die Gruppe-B-Stellen uebergeben a2 = Part-Record + 72 wie alle anderen
80108e18: addiu a0,zero,3000 / 80108e1c: sll a1,v0,4 / 80108e20: addiu s0,s0,1448 / 80108e24: addu a2,s0,zero
80108e28: jal 0x8001bf10 / 80108e2c: addu a3,zero,zero                      ; RIP-Kopf, Part 8
8010535c: addiu a0,zero,8000 / 80105364: addiu a2,s0,72 / 80105368: jal 0x8001bf10 / 8010536c: addu a3,zero,zero   ; Gliedabriss
801053c4: addiu a0,zero,7000 / 801053cc: addiu a2,s0,72 / 801053d0: jal 0x8001bf10 / 801053d4: addu a3,zero,zero
80106d6c: lui a0,0x3 / 80106d70: ori a0,a0,0x1000 / 80106d78: addiu a2,s0,244 / 80106d7c: jal / 80106d80: addu a3,zero,zero ; Ragdoll Part 1
80107700: addu a2,s0,zero / 80107704: jal 0x8001bf10 / 80107708: addu a3,zero,zero                           ; Arm-Abriss 4. Spawn
; (3) WER Record+72 schreibt: der ZEICHNER. Part-Walk (Stride 172, a3 = Record+72):
80027374: lw a2,0(s2) / 8002737c: andi v0,a2,0x1 / 8002738c: jal 0x80027434 / 80027390: addiu a3,s2,72 / 800273a4: addiu s2,s2,172
800273b0: addiu s0,s2,72 / 800273d4: jal 0x80027434 / 800273d8: addu a3,s0,zero / 800273f8: addiu s0,s0,172 / 8002740c: addiu s2,s2,172
; FUN_80027434 (a0 Entity, a1 Record, a2 Flags, a3 = Record+72):
8002747c: lw a2,116(s1)                     ; Eltern-Matrix (Record+0x74)
80027498: andi v0,s3,0x40 / 8002749c: bne v0,zero,0x800275e4     ; Flag 0x40 = eigene Matrix -> NICHT neu komponieren
800274a4..800274e0: ctc2 RT/TR <- Eltern-Matrix ; 8002748c: addiu a0,s1,24 (lokale Rotation Record+0x18)
80027504/48/8c: MVMVA 0x4a49e012 ; 8002751c-24 / 80027560-68 / 800275a0-a8: sh t4/t5/t6 -> Record+72.. (R = R_Eltern * R_lokal)
80027590: addiu a1,s1,44 (lokale Translation Record+0x2C) / 800275cc: MVMVA 0x4a480012 (RT*V0+TR)
800275d0: addiu a1,s0,20 / 800275d4-dc: swc2 -> Record+72+20/24/28  ; t = Welt-Translation
80027694: andi v0,s3,0x20 / 800276a0: jal 0x80028ad8 / 800276a4: addu a1,s0,zero   ; Teile-Physik auf DERSELBEN Matrix
; (4) Record+0x18 fuellt der Anim-Durchlauf aus dem (EMR,EDD)-Paar der AUFRUFSTELLE, mit Blende
8002959c..800295f8: Bild aus EDD[+0x14C], +0x14D -> sw a2,376(a0) / 800295fc: jal 0x80029614
800296a8: lbu t3,334(s2) (+0x14E) / 800296b0: beq t3,zero / 800296b8: andi v0,a3,0xffff / 800296bc: mult v0,t3   ; Gewicht a3 * +0x14E
; (5) Aufschlag-Blut des fliegenden Teils (FUN_80028AD8): Anker = die EIGENE Matrix des Teils
80028c28: jal 0x80015fe8 / 80028c30: sll a1,v0,4 / 80028c34: addiu a2,s1,72 / 80028c3c: addu a3,zero,zero
80028c4c: jal 0x8001bf10 / 80028c50: ori a0,a0,0x7d0                       ; t[1] = Boden schon @0x80028BF4 (Port-Kommentar)
```
=> Jeder FUN_8001BF10-Anker (mit oder ohne a3) ist die Matrix, die der Zeichner zuletzt in Record+72 geschrieben hat: Eltern-Kette
mal lokaler Pose aus dem Paar, das der Handler animiert (Paar 1 oder 2), MIT der +0x14E-Blende; fuer ein Teil mit Flag 0x40 die
eigene, von der Physik gefuehrte Matrix. Port-Zwilling = die Pose, die der Port-Renderer zeichnet (main.c: Bankwahl 9798-9925,
Crossfade g_anim_pose_actor 9960, Unterhaelfte 10195-10220, Ragdoll-Anker 10315-10330, eigene Teil-Matrix 10339) — fuer ALLE
Emitter. Die Port-Konvention "laufendes Bild" (§2.3/§8.2, Abnahme B1) bleibt.

### 9.4 Aenderung (Commit ccc46f65)
* `engine/src/schuss_r36.c`: neu `re15_schuss_r36_gezeichnet(e, bone, R, T)` = Zwilling dessen, was der Port-Renderer zeichnet
  (s. 9.3 =>): `zeichen_bank` (Bankwahl main.c:9798-9925 in derselben Reihenfolge — Hauptbank + clip_override bei +0x1C4 & 4,
  Loco-Bank bei re15_actor_uses_loco_bank, Kanaele state 4, eigene Bank, RE1.5-Sitz-Import), Crossfade auf einer Schatten-Kopie
  (wie bisher), Unterhaelfte Bones 1..6 aus re15_re2z_lower_kf mit deren Skelett, Ragdoll-Bodenanker
  (re15_re2z_ragdoll_part_anchor mit der GEZEICHNETEN Part-0-Lage), und fuer ein Teil mit eigener, schon geseedeter Matrix diese
  Matrix (re15_re2z_gore_part_matrix_peek). `re15_schuss_r36_blut_anker` = T + R * a3 daraus — fuer JEDEN Emitter, a3 = NULL/0
  eingeschlossen. Ohne Bank wie bisher Wurzel + Ryaw * a3 (unit_r26_mg_blut, test_re2_gore_decoder).
* `engine/src/enemy_ai_re2_zombie.c` (kleine Haken): `re2z_gore_fx_core` ruft den Anker auch fuer a3 = NULL (1 Zeile statt
  re15_enemy_bone_world_pos) — damit Gruppe B (re2z_gore_fx / re2z_gore_fx_ex) inkl. RIP-P1 und Aufschlag-Blut;
  `re15_re2z_gore_part_matrix_peek` (Lesezugriff neben re15_re2z_gore_part_matrix, kein Seeding, kein Physikschritt).
  re2z_blood_fx_at lief schon ueber den Anker und erbt Bank/Ragdoll-Korrektur.
* `include/re15_schuss_r36.h`: Belegkette (9.3) + Deklarationen.
* Keine neue Konstante. Die Port-Konvention "laufendes Bild" bleibt (Abnahme B1).

### 9.5 Messung nachher
* **nb2_blut (Renderer-Zwilling), Ausgaben `C_schuss_mess/n2/nachher/`, woertlich:**
  ```
  w13 px eben: F66 part 0 (ohne) d_r 0 (vorher 80) | F78 Z part 9 fx=(-3230,0,-20039) d_r 0 (vorher 1902, fx (-2902,-1853,-20316))
               F78 Z part 12 d_r 0 (2280) | F84 Z part 9/12 d_r 0/0 (2063/2408) — das Aufschlag-Blut liegt jetzt AM BODEN beim Glied
               F66 part 8 (Kopf) fx=(-1871,-2745,-19682) d_alt 0: an der Seed-Lage (gezeichnete Skelett-Pose), aus der der
               Kopf abfliegt (d_r 361 = Abstand zum Kopf NACH seinem ersten Driftschritt 400-50 = 350, FUN_80028DAC — im
               Original ebenso: der Anker ist die zuletzt gezeichnete Matrix, die Drift laeuft erst beim naechsten Zeichnen)
  w12 pz eben: F76 part 12 (ohne) d_r 24 (204), part 13 (ohne, fliegt ab) d_alt 57 (93); F89/F94 Z part 13 d_r 0/0 (2124/2201)
               (MG-Bild: Restabstand = Drehung/Schub NACH dem Spawn @0x8010810C-6C, wie die a3-Stoesse d_r 30-73)
  w8 pz tief:  F66 part 3 (ohne, 4. Spawn) fx=(-1301,-1016,-19854) = Seed-Lage (d_alt 0; vorher (-1220,-1031,-19899) d 93)
  w8 px eben:  F86 rag 1 part 1 (ohne) d_r 0 (98)
  w3 px eben:  F66/F78 bank loco part 0 a3=(-140,-483,0) d_r 0/0 (103/121); w3 pz tief F66/F78 part 1 d_r 0/0 (56/110)
  ERGEBNIS: w13 "mit a3 0, ohne a3 361 (>40: 1 = Kopf vs. gedriftet)", w12 "ohne a3 143 (>40: 1 = Glied vs. gedriftet)",
            w8 pz tief "ohne a3 10", w8 px eben "0", w3 px eben "mit a3 24", w3 pz tief "mit a3 32", w7 "0"
  ```
* **Pins (probe_r36_blut, Renderer-Zwilling + (G)), gegen den ALTEN Engine-Stand gelinkt** (libre15_engine.a mit den
  Objekten aus 6fa888c2, `nb2/altlib/`, Ausgaben `C_schuss_mess/n2/pin_alt_engine/`): eben exit 1 (4 FAIL: F66/F78 Loco-Bank
  103/121), tief exit 1 (4: 56/109), rip exit 1 (7: Kopf 504, RIP-P1 80/80, Aufschlag 1902/2280/2063/2408), arm exit 1 (1: 93),
  burst exit 1 (1: 98), glied exit 1 (3: Glied 204, Aufschlag 2124/2201), mg exit 0 (kein Abriss mit dieser Saat).
  **Neuer Stand** (`pin_neu/`): alle sieben `probe_r36_blut: OK` — (G) max d: eben 24, tief 32, mg 88 (MG-Bilder, Schranke 100),
  rip 0, arm 0, burst 0, glied 73 (MG-Bilder); Zaehler rip: Kopf 1, RIP-P1 2, Aufschlag 4; arm: 4. Spawn 1; burst: Ragdoll 1;
  glied: Gliedabriss 2, Aufschlag 2; eben/tief: je 2 Spawns in Loco-Bank-Bildern.

* **exe (echter Renderer), Vorher/Nachher im SELBEN Lauf** (`C_schuss_mess/n2/run_n2_exe.sh`, Aufbau = abn1C/p2_batch.sh:
  ROOM1140 per RE15_DEBUG_JUMP, Leon (-5000,-19600) Blick +x, RE2-KI, Ingram 12 Dauerfeuer `W1.2,M0.6,MA9.0,M0.5,W2`,
  RE15_FX_LOG/RE15_STATE_LOG/RE15_FRAMEDUMP; "alt" = dieselbe exe neu gelinkt gegen libre15_engine.a mit den Objekten
  schuss_r36/enemy_ai_re2_zombie aus 6fa888c2, Messkopien re15_pc_r36c_n2(a).exe, danach geloescht; beide exit=0). Zustandsfolge
  Platz 2 in beiden Laeufen gleich (state.log: F55..F74 MG-Treffer 50 -> 2, F75 Tod st 3/15/1, F136 st 7). Blut-Spawns
  (`C_schuss_mess/n1/fx_auswertung.py`, Ausgaben `n2/exe/mg_{alt,neu}_fx_auswertung.txt`), einzige Lageaenderungen:
  ```
  alt: Blut-Spawn F77 sub=0 w(-2514,-1850,-19270)   neu: Blut-Spawn F77 sub=0 w(-2325,0,-19731)    <- Aufschlag eines Glieds
  alt: Blut-Spawn F81 sub=0 w(-2563,-1821,-19324)   neu: Blut-Spawn F81 sub=0 w(-2333,0,-19727)    <- zweiter Aufschlag
  ```
  = das Aufschlag-Blut lag in der Auslieferung 1,8 m hoch am Rumpf und liegt jetzt am Boden beim Glied. Alle uebrigen Spawns
  +-1..2 (Rundung: R_Welt * a3 in EINEM Schritt wie der Lagen-Tick, RT = Welt-Matrix @0x8001DB74 / MVMVA @0x8001DBB4, statt
  R_part * a3 und danach Ryaw). Bild-Differenz (`n2/exe/mg_bilddiff.txt`, Bogen `n2/exe/mg_vergleich_F76-109.png`): nur
  F61-F163 im linken Bildrand (x 0-55), max. 168 Pixel — der Aufschlag liegt in diesem Kamerabild am Rand, im Blut des
  MG-Feuers; die Zahlen aus fx.log sind hier der Beleg, das Bild nur die Gegenprobe "keine weiteren Aenderungen".
  (Der SPAS-Lauf der Abnahme, `spas_x`, trifft einen AUFSTEHENDEN Fresser -> anderer Todes-Handler ohne Gruppe-B-Spawn;
  nachgefahren: fx.log bis auf +-1 gleich, Bilddiff <= 21 Pixel.)

### 9.6 Tests
* Neu/erweitert (tests/unit/probes/r36_schuss.cmake, Sonde tests/unit/probe_r36_blut.c): `unit_r36_blut_pin_rip`, `_arm`,
  `_burst`, `_glied` (Gruppe B + Aufschlag-Blut + Ragdoll, Pruefung (G) + Zaehler je Emitter), `_eben`/`_tief`/`_mg` jetzt mit
  dem Renderer-Zwilling als Zeichner und (G) fuer jeden Spawn (eben/tief sehen je 2 Spawns in Loco-Bank-Bildern). Gegen den
  alten Engine-Stand rot (9.5), neu gruen.
* Gezielter Lauf (keine volle Suite im Spurbaum), Regex = die von Nachbesserung 1 (`mess_r36c/n1/regex2.txt`, deckt
  enemy_ai_re2_zombie.c, schuss_r36.c, re15_damage.c-Haken und die Raeume) — woertlich:
  ```
  ctest --test-dir re15_port/build -R "r36|crow|kraehe|hitfilter|zombie|re2z|re2_|1140|1030|1040|10d0|1100|1210|damage|werfer|dog_hit|spider|gator|birkin|g5_|shootable|hit_|autofire|waffen|blood|blut|gore|feeder|straight_shot|enemy_ai|combat|aim|r16_|rise_|knockdown|stagger|death|crawl|writher|r26_mg|r35_inhalt|r35_raeume|r35_cut1150|r35_zgirl|r30_hund|redhawk|r17_|zfall|situp|fx_region|granate" --timeout 600 --output-on-failure -j 2
  100% tests passed, 0 tests failed out of 203
  Total Test time (real) = 883.02 sec
  ```
  (Stand ccc46f65; danach nur Kommentar-Zeilennummern in schuss_r36.c (a41911ad) -> neu gebaut und
  `ctest -R "r36_blut|r36_schuss"`: `100% tests passed, 0 tests failed out of 10`, integration_r36_schuss 125.94 s.)
  Darin u.a. die Riegel, die Punkt 1/3 der Abnahme tragen (integration_r36_schuss, unit_r36_schuss, unit_r36_schuss_kraehe,
  unit_crow_death_corpse, unit_re2_zombie_hitfilter/_abc, unit_re2_crow_ai) und die Blut-/Gore-Riegel (unit_r26_mg_blut,
  unit_re2_gore*, test_re2_gore_decoder, unit_re2_zombie_teardeath, probe_re2_zfall, probe_10d0_situp_re2, stagger/knockdown).
* Konfliktpruefung `git merge-tree --write-tree HEAD <zweig>` gegen r36/zombieki, granate, gegnerzahl, elza, klettern,
  kleidung, szenenleon, integration: alle konfliktfrei.

### 9.7 OFFEN (Stand Nachbesserung 2) + Fuer den Nutzer
* **OFFEN 11 (N1, Anker-Pose der a3 = 0-Emitter): erledigt** (9.4) — dazu die drei Faelle, die dieselbe Ursache hatten und die
  Metrik der Abnahme nicht sehen konnte: Loco-Bank-Phasen (a3-Emitter 56-121 neben dem gezeichneten Koerper), Aufschlag-Blut
  der fliegenden Teile (1900-2400 neben dem Glied, jetzt am Boden beim Glied), Ragdoll (98).
* **OFFEN 12 (neu, nicht Lage): Winkel/Id einzelner Gruppe-B-Emitter.** Das Original uebergibt dort a1 = rand << 4: Gliedabriss
  `jal 0x80015fe8` / `sll a1,v0,4` @0x80105354-60 und @0x801053BC-C8, Ragdoll-Emitter @0x80106D64-74 (dazu a0 = 0x00031000
  `lui a0,0x3` / `ori a0,a0,0x1000` @0x80106D6C-70), Aufschlag-Blut @0x80028C28-30 (a0 = Sub aus `lw a0,0(s3)` >> 19 & 0x18
  | 0x7D0 @0x80028C38-50). Der Port zieht diese Wuerfe (Folge unveraendert), gibt aber +0x76 als Winkel weiter
  (re2z_gore_fx / re2z_blood_fx_at; Ragdoll-Emitter Skala 0x1500 = alter Stand-in ohne Beleg). Betrifft die RICHTUNG/Groesse
  des Spritzers, nicht die Lage (Mangel M1); nicht geaendert, damit dieser Schritt nur die Lage bewegt. Messweg: Ring um a1
  erweitern, gegen r<<4 pruefen.
* OFFEN 7-10 (§5) unveraendert.
* **Fuer den Nutzer (Nachbesserung 2):** das Blut sitzt jetzt auch bei den Sonder-Treffern dort, wo der Koerper GEZEICHNET wird:
  Schrot-Kopfschuss (Hals-Blut am Halsansatz, aus dem der Kopf abfliegt; das Nachbluten am Becken), abgeschossene Glieder
  (Ingram/MC51; Arm-Abriss mit der Schrotflinte), der zerrissene Zombie am Boden (Ragdoll), und vor allem: wenn ein
  abgerissenes Bein/ein Arm auf dem Boden aufschlaegt, spritzt das Blut jetzt AM GLIED auf dem Boden statt in Armhoehe am Rumpf
  (vorher bis 2,4 m daneben). Ausserdem sitzt das Treffer-Blut bei den ersten Treffern auf einen GEHENDEN Zombie jetzt am
  gezeichneten Koerper (vorher bis ~12 cm daneben, 121 Einheiten, weil der Anker aus der falschen Animationsbank kam). Keine neuen Assets,
  keine Sprachdateien, keine Bits/Nachrichten/Ereignisse.

### 9.8 Stand je Mangel (Abnahme 1)
| Mangel | Ursache | Vorher (gemessen) | Beleg | Aenderung | Nachher (gemessen) |
|---|---|---|---|---|---|
| M1 Gruppe B an der rohen Pose | re2z_gore_fx_core a3 = NULL -> re15_enemy_bone_world_pos (QUERY, Hauptbank) | RIP-Kopf 442-504, RIP-P1 80-109, Gliedabriss 140-204, Arm 4. Spawn 37-93, Ragdoll 98 | FUN_8001BF10 @0x8001C000-04 / @0x8001C03C-78, Zeichner @0x80027390 / @0x8002751C-DC | Anker = re15_schuss_r36_gezeichnet fuer jeden Emitter | 0 (MG-Bild 24, Restabstand = Drehung nach dem Spawn @0x8010810C-6C) |
| dazu: Aufschlag-Blut | Knochen des Rumpfs statt eigener Matrix des Teils | 1900-2400 (Sonde), exe F77/F81 y -1850/-1821 | @0x80028C34-4C, @0x80027498-9C, @0x800276A0-A4 | re15_re2z_gore_part_matrix_peek | 0 (Sonde), exe y 0 am Glied |
| dazu: Loco-Bank | blut_anker posierte immer Paar 2 | a3-Emitter 56-121 neben dem gezeichneten Koerper | @0x800295F8-FC, main.c:9834-9843 | zeichen_bank wie der Renderer | 0 |
| dazu: Ragdoll | Ragdoll-Zweig ohne Crossfade | 98 | wie M1 | Ragdoll-Anker auf der gezeichneten Pose | 0 |
Punkte 1 und 3 (Abnahme 1: erfuellt) unberuehrt; ihre Riegel im gezielten Lauf gruen (9.6).

Commits der Nachbesserung 2 (Zweig r36/schuss): 3a688104 (Gliederung), 66fea251 (Ursache + Messung vorher), fb3c5932 (RE-Beleg),
ccc46f65 (Code + Pins), 6b140f55 (Dossier 9.4/9.5 + Sonde), ee9842bc (9.7), a41911ad (Zitate), 0c07d2b3 (exe + Tests), dieser
Abschluss-Commit (Flag-Zitate im Lesezugriff, 9.8).

## 10. Nachbesserung 3 (nach Abnahme 2, `C_abnahme_2.md` §6 M1)

Stand: Fortsetzung im selben Baum ab 4adb4890 (Abnahme 2). Einziger Mangel M1 (Punkt 2): Kriecher-Kopfabriss — zwei
Blut-Emitter am Kriecher, den Leon bei der Befreiung aus dem Bein-Biss toetet, mit WELT-Versatz statt Anker-Versatz
(enemy_ai_re2_zombie.c:2028 `re2z_gore_fx_ex(e, 0, 0x00801000u, rot_y, 0, -600, 0)`, :2048 `re2z_gore_fx_ex(e, 8, 4096u,
ang, 0, 100, 0)`); der Zensus §8.8 C fuehrte beide falsch als "Griff/Biss — kein Blut am getroffenen Zombie"; der Pin
`probe_r36_blut pin mg` bricht beim Kriecher ab und sieht den Fall nicht. Punkte 1 und 3 (Abnahme 2: erfuellt) bleiben
unberuehrt; ihre Riegel muessen gruen bleiben. Eigene Mess-Dateien im Scratchpad `c_nb3_r36/` (Kurzform `nb3/...`),
Kopien der urteilstragenden Skripte/Ausgaben im Repo unter `C_schuss_mess/n3/`.
Gliederung (wird fortlaufend gefuellt): 10.1 Ursache / 10.2 Messung vorher / 10.3 RE-Beleg / 10.4 Zensus berichtigt
(alle 88 Stellen mit a0) / 10.5 Aenderung / 10.6 Messung nachher / 10.7 Tests / 10.8 OFFEN + Fuer den Nutzer / 10.9 Stand.

### 10.1 M1 — Ursache (Code gelesen + gemessen)
* enemy_ai_re2_zombie.c (Kriecher-Zweig P5 des Griffs, Leon-Weiche, Bild 22 bzw. 23..41 ungerade) rief
  `re2z_gore_fx_ex(e, 0, 0x00801000u, rot_y, 0, -600, 0)` und `re2z_gore_fx_ex(e, 8, 4096u, ang, 0, 100, 0)`: die
  ofs-Argumente von re2z_gore_fx_ex sind ein WELT-Versatz (p = Anker.T + ofs, re2z_gore_fx_core). Im Original ist a3 ein
  Zeiger auf einen Stapel-Vektor im ANKER-Rahmen (10.3) — am liegenden Kriecher zeigt (0,-600,0) im Becken-Rahmen den
  Ruecken entlang zum Hals, als Welt-Versatz senkrecht nach oben (600 ueber dem Becken, "in der Luft auf Kniehoehe").
* **Dazu (eigener Fund, nicht in der Abnahme): die Id des ersten Emitters ist falsch dekodiert.** `lui a0,0x800` laedt
  0x0800 << 16 = **0x08000000**, mit `ori a0,a0,0x1000` also **0x08001000** = Id 8 (Fleischbrocken; RE2 ROOM1000 id 8 ==
  RE1.5-Raum-Id 5, gore-vollausbau.md §1.2), Sub 0, Skala 4096. Der Port trug 0x00801000 (Id 0 = Blut, **Sub 128**) — genau
  das zeigt fx.log der Abnahme (`id=0 sub=128`). Dieselbe Folge `lui a0,0x800 / ori a0,a0,0x1000` steht beim Arm-Abriss
  (Zensus @0x80107600 und @0x80107704 = 0x08001000); dort fuehrt der Port richtig 0x08001000. Die Aussage der Abnahme
  "Id 0 = Blut" fuer @0x80102B24 ist damit widerlegt (Bytes 10.3); der Lage-Mangel selbst ist richtig.
* Pin-Luecke: `probe_r36_blut pin mg` bricht beim Kriecher ab (probe_r36_blut.c:484-487) — kein Pin sah den Fall.

### 10.2 Messung vorher (Stand 4adb4890 = Code d5c0c14f, exe/Bibliothek 21:43)
* Sonde der Abnahme unveraendert (`abn2C/abn2_blut.exe`, gegen genau diese libre15_engine.a gelinkt), `12 px eben 2`
  (Ausgabe `C_schuss_mess/n3/abn2_blut_vorher_w12_px_eben_2.txt`), woertlich:
  ```
  -> Welt-Versatz (0,-600,0) an Part 0; mit a3-Semantik (T + R*ofs) laege der Spawn bei (568,-114,-19589): Abstand 937
  F192 st 1/1/5->1/1/5 hp 82->82 rag 0 bank 0 | part  0 fx=(-16,-846,-19622) a3=ohne (0,0,0) | d(gezeichnet T)=600 | d(T-1)=711
  -> fliegendes Teil: Versatz zur Matrix des Vorbilds (0,100,0); mit a3-Semantik laege er bei (547,-159,-19613): Abstand 149
  F193 ... part  8 fliegt fx=(644,-48,-19587) a3=ohne (0,0,0) | d(gezeichnet T)=100   (dito F195..F209, ungerade)
  ERGEBNIS waffe 12 px eben platz 2 alle 0: Tod F211, Spawns 14 (ohne a3/Null 10 ...) | max d(T) 600, ohne a3 600
  ```
* Alle 320 Laeufe der Abnahme (`abn2C/blut`), eigene Klassierung `C_schuss_mess/n3/klassen3.py`
  (`klassen3_vorher.txt`): `K1 Kopfabriss Part 0 @0x80102B24 Welt-Versatz: Spawns 25 in 25 Laeufen, d 600` /
  `K2 Hals-Stumpf Part 8 @0x80102BBC Welt-Versatz: Spawns 286 in 32 Laeufen, d 100`. (Die uebrigen 7 Kopfabrisse dieser
  Laeufe stehen bei der Abnahme in K6 mit d 601/663/709 bzw. sind von stderr-Zeilen zerschnitten — s. 10.6 "Anker-Zeitpunkt".)
* exe (echter Renderer), eigene Messkopie `re15_pc_r36c_nb3_alt.exe` (= exe 21:43), Aufbau E2f der Abnahme
  (`RE15_DEBUG_JUMP=1140@250`, `RE15_PLAYER_POS=-3600,-19600,0`, `RE15_AI_FLAVOR=re2`, `RE15_GIVE=12:250`, `RE15_EQUIP=12`,
  `RE15_INPUT_SCRIPT=W1.2,M0.6,MA3.0,M0.3,W16`, `RE15_EXIT_AT=700#1140`, fx/state-Log, `RE15_FRAMEDUMP=215-250/1`,
  `RE15_WINDOW_SCALE=1`; Skript `C_schuss_mess/n3/lauf.sh`), exit=0, fx.log bitgleich mit der Abnahme:
  `id=0 sub=128 eidx=2 frame=1 w(-2732,-849,-19781) ... F=226` und Hals-Stumpf `id=0 sub=0 ... w(-3376,-51,-19641)` F227..F243.

### 10.3 RE-Beleg (selbst disassembliert, `re2_disasm.py --bin EMZ0.BIN`, info/re2leon/COMMON/BIN/EMZ0.BIN)
```
; Kriecher-Zweig P5, Leon-Weiche, +0x14D == 22 (Kopf ab)        Bytes (LE)
80102ac0: addiu v0,zero,22 / 80102ac4: bne v1,v0,0x80102b70
80102ad0: lw v1,408(s1)                    ; Modellblock +0x198      98 01 23 8e
80102ad4: lui a0,0x800                     ; a0 = 0x08000000          00 08 04 3c
80102adc: ori a0,a0,0x1000                 ; a0 = 0x08001000          00 10 84 34   -> Id 8, Sub 0, Skala 4096
80102ae4: sw v0,1376(v1)                   ; Part 8 Flags |= 0x4A
80102aec: addiu a3,sp,24                   ; a3 = ZEIGER auf sp+24    18 00 a7 27
80102b0c: lw a2,408(s1)                    ;                          98 01 26 8e
80102b10: addiu v0,zero,-600               ;                          a8 fd 02 24
80102b14: sh zero,24(sp) / 80102b18: sh v0,26(sp) / 80102b1c: sh zero,28(sp)    ; Vektor (0,-600,0)
80102b20: lh a1,118(s1)                    ; a1 = +0x76 (Gier)        76 00 25 86
80102b24: jal 0x8001bf10                   ;                          c4 6f 00 0c
80102b28: addiu a2,a2,72                   ; a2 = Part 0 (+0x198+72)  48 00 c6 24
; Hals-Stumpf: +0x14D in 23..41, ungerade
80102b78: addiu v0,v1,-23 / 80102b7c: sltiu v0,v0,0x13 / 80102b84: andi v0,v1,0x1
80102b8c: addiu v0,zero,100                ;                          64 00 02 24
80102b90: lw s0,408(s1)
80102b94: sh zero,24(sp) / 80102b98: sh v0,26(sp) / 80102ba0: sh zero,28(sp)    ; Vektor (0,100,0)
80102b9c: jal 0x80015fe8                   ; rand (Winkel)
80102ba4: addiu a0,zero,4096               ; a0 = 0x00001000 -> Id 0 (Blut), Sub 0, Skala 4096   00 10 04 24
80102ba8: addiu a3,sp,24                   ; a3 = ZEIGER auf sp+24    18 00 a7 27
80102bac: lh a1,118(s1) / 80102bb0: sll v0,v0,2 / 80102bb4: addu a1,a1,v0 / 80102bb8: addiu a1,a1,-512
80102bbc: jal 0x8001bf10                   ;                          c4 6f 00 0c
80102bc0: addiu a2,s0,1448                 ; a2 = Part 8 (8*172+72)   a8 05 06 26
; FUN_8001BF10 liest a3 (Abnahme 2 §4 selbst gelesen): beq a3,zero @0x8001BFE0 / lw v0,0(a3) / sw v0,44(t0) /
; lw v0,4(a3) / sw v0,48(t0); Matrix aus a2 (32 Byte) -> slot+76.. @0x8001C03C-78.
; RE2-Lagen-Tick (PSX.EXE, selbst gelesen): 8001db74: addiu v1,v0,76 ; ctc2 RT <- slot+76..92 (0x48cc0000/0x48cd0800/
; 0x48cc1000/0x48cd1800/0x48ce2000 @0x8001DB80-9C) ; 8001dba0: addiu v0,v0,44 ; lwc2 VXY0/VZ0 <- slot+44/48
; (0xc8400000/0xc8410004) ; 8001dbb4: MVMVA 0x4a486012 (sf=1, RT*V0, ohne TR) ; swc2 MAC1-3 @0x8001DBB8-C0 ;
; 8001dbd0-dc1c: lw 96/100/104(a0) (Anker.T) + R*a3 + Lage +52/+54/+56   => Lage = Anker.R * a3 + Anker.T (+ xlat)
```
=> beide Emitter: Lage = Anker.R * a3 + Anker.T; @0x80102B24 ist Id 8 (Fleischbrocken), @0x80102BBC Id 0 (Blut).

### 10.4 Zensus berichtigt (alle 88 `jal 0x8001bf10` mit a0)
Neues Skript `C_schuss_mess/n3/zensus3.py` (a0 aus `addiu a0,zero,imm` / `lui`+`ori`/`addiu`; Register-Fenster ab dem letzten
jal, Stapel-Fenster ueber rand-Aufrufe hinweg; Ausgabe `C_schuss_mess/n3/zensus3.txt`). Grenze des Skripts: bei verzweigtem
Code nimmt es den LETZTEN a0-Schreiber im Fenster (z.B. @0x801086B4 TOD zeigt 6096 = Zone-0-Zweig, der Zone!=0-Zweig setzt
8096 @0x8010866C) — urteilstragend sind nur die selbst disassemblierten Stellen. Berichtigung von §8.8:
* **C -> A:** 0x80102B24 (Kopfabriss, **Id 8** Fleischbrocken, Part 0, a3 (0,-600,0)) und 0x80102BBC (Hals-Stumpf, Id 0 Blut,
  Part 8, a3 (0,100,0)) — Gore am Zombie, den Leon dabei toetet; seit dieser Nachbesserung im Anker-Rahmen.
* **C, Einordnung berichtigt:** 0x80103C8C ist **Blut** (a0 = 0x13D0 -> Id 0, Skala 5072) am FRESSENDEN Zombie (Part 8,
  a3 (200,0,0) @0x80103C70-7C, nur bei Bild-Wort & 0x10000 @0x80103C50-64) — kein Treffer-Blut; im Port fehlt der Spawn
  (enemy_ai_re2_zombie.c "Fress-Tropf-FX ... OPEN") = fehlendes Blut, nicht falsche Lage -> OFFEN 13. Feuer (Id 5):
  0x80106180, 0x80106220, 0x80106254, 0x80106440, 0x80106470; Funken (Id 6 bzw. 4/12, a0 dynamisch): 0x80106574,
  0x801065C0, 0x8010660C, 0x80106658, 0x801066AC, 0x801066D4.
* Gruppen A/B/D sonst unveraendert; Summe 29 (A) + 30 (B) + 12 (C) + 17 (D) = 88.

### 10.5 Aenderung
**(a) Die zwei Emitter (Commit 01445c3e), enemy_ai_re2_zombie.c Kriecher-Zweig P5:**
* @0x80102B24: `re2z_gore_fx_a3(e, 0, 0x08001000u, rot_y, {0,-600,0})` statt `re2z_gore_fx_ex(e, 0, 0x00801000u, rot_y, 0,
  -600, 0)` — a3 im Anker-Rahmen Part 0 und die richtige Id (8 -> Raum-Id 5 ueber den vorhandenen Dekoder `case 8: id = 5`).
* @0x80102BBC: `re2z_gore_fx_a3(e, 8, 4096u, ang, {0,100,0})` statt `re2z_gore_fx_ex(..., 0, 100, 0)`.
* Keine neue Zahl: -600 @0x80102B10, 100 @0x80102B8C, 0x08001000 @0x80102AD4-DC, 4096 @0x80102BA4 (alle im Kommentar).

**(b) Anker-ZEITPUNKT (Commit 3a60ef41) — gefunden bei der Messung nachher (10.6), dieselbe Fehlerklasse "Blut nicht am
gezeichneten Koerper":** Nach (a) lagen 27 der 32 Kopfabrisse der 320-Lauf-Matrix exakt an der gezeichneten Matrix (d 0), 5
aber 41..666 daneben (`vergleich_k1.txt` Zwischenstand: w12_nz_eben_p4 378, w12_nz_tief_p4 41, w12_pz_eben_p5 284,
w19_nz_*_p4 445, w19_pz_*_p5 666; vorher lagen dieselben 5 bei 601/663/709 bzw. 896 statt 600 — die Abnahme fuehrte 3 davon
in K6 ohne Wertung). Gemessen mit einem Debug-Zwilling von re2z_gore_fx_core (Objekt vor die Bibliothek gelinkt,
`C_schuss_mess/n3/patch_engine_fxdbg.py`, Sonde `abn2_blut_dbg2.c`), Lauf 19 pz eben 5:
```
  POS F266 vor (903,-17974) r2917 mo 16 af 21 | nach (889,-17969) r2917 mo 16 af 22 | spawns 1
  FXDBG part 0 a3 (0,-600,0) p (68,-117,-17366) | e (224,0,-17929) r2917 mo 16 af 22 st 1/1/5 | anker rv 1 T (224,-249,-17929)
  DBG F266 post-Anker T=(889,-249,-17969) == gezeichnet T=(889,-249,-17969) | anker(a3) post=(733,-117,-17406) fx=(68,-117,-17366)
```
d.h. im Spawn-Moment stand der Kriecher bei x 224, vor und nach dem Tick bei ~900: der Griff setzt die Lage ABSOLUT —
`jal 0x80015cb8` @0x80102A50 (P5 vor dem Advance @0x80102A64); FUN_80015cb8 (RE2 PSX.EXE, selbst gelesen): Wurzelversatz
des Bilds ueber FUN_80015db0 @0x80015CE8, Gier-Matrix `jal 0x8008e8b4` @0x80015D40, ApplyMatrix `jal 0x8008dba4` @0x80015D50,
dann `lh v0,356(s1)` / `addu` / `sw v0,56(s1)` @0x80015D58-68 (+0x38 = +0x164 + v.x), dito +0x3C/+0x40 aus +0x166/+0x168
@0x80015D6C-90 — und die SCA-Klemme im Schwanz holt ihn zurueck (Port: run_all Zombie-Zweig, re15_enemy_sca_clamp nach den
Schueben). Das Original ruft dieselbe Klemme unbedingt nach dem Handler: EMZ0-Root Zustands-`jalr v0` @0x801004E8, Schwanz
`jal 0x80035530` @0x8010062C, `jal 0x8003567c` mit a1 = 1024 @0x80100638; FUN_8003567c ueberspringt nur bei word0 & 8
(`andi v0,v1,0x8` / `bne` @0x80035694-98) oder +0x1E4 != 0 (@0x800356A0-A8), sonst SCA-Resolver `jal 0x8004c1bc` @0x800356D0;
der Griff-Executor 0x801025EC-0x80102C30 schreibt word0 nur @0x80102778 (`ori v0,v0,0x1000`) und im Tod @0x80102C20, +0x1E4
nie (Liste aller Speicher auf 0(s1)/484(s1) in dem Bereich). Der Anker des Originals ist Record+72 des VORIGEN Zeichnens
= nach der Klemme jenes Ticks. Der Port rechnet ihn (PORT-KONVENTION "laufendes Bild") mitten im Tick, VOR Schub und Klemme.
* Umsetzung (eigene Datei schuss_r36.c + kleine Haken): `re15_schuss_r36_fx_merke` merkt die belegten ESP-Plaetze unmittelbar
  vor dem Spawn, `re15_schuss_r36_fx_nach(e)` (1 Zeile nach dem Spawn in re2z_blood_fx_scaled und re2z_gore_fx_core) legt
  {Aktor, Bild, Lage, neue Plaetze, Ring-Nr., eigene Matrix?} ab; enemy_ai_common.c (2 Zeilen + include):
  `re15_schuss_r36_nachfuehr_marke(e)` im Zombie-Zweig von run_all direkt nach re15_enemy_ai_live_step = Lage am Handler-Ende,
  `re15_schuss_r36_nachfuehren_alle()` als letzte Zeile von run_all = die Plaetze (x/z) und der Mess-Ring werden um die
  Lage-Differenz des SCHWANZES verschoben (Commit 6b21dd7b; zuerst stand der Aufruf direkt hinter re15_enemy_sca_clamp —
  dort fuegt r36/zombieki re15_r36_wurf_klemme ein, eine WEITERE Lage-Korrektur nach der Klemme: textueller merge-Konflikt
  und deren Bewegung waere nicht erfasst worden. Am Ende von run_all ist jeder Gegner fertig geschoben/geklemmt — Schub und
  Klemme bewegen nur den Aktor des laufenden Ticks; run_all hat keinen vorzeitigen Ausstieg).
  Nicht verschoben: Anker mit eigener Matrix (freifliegende Teile, re15_schuss_r36_gezeichnet == 2) und Bewegung IM Handler
  nach dem Spawn (MG-Schub @0x80108110-48 — auch im Original spawnt der Handler vor dem Schub). Schub/Klemme aendern nur x/z,
  nicht die Gier (re15_enemy_sca_clamp, re15_body_push) -> die Verschiebung ist exakt die Lage der gezeichneten Matrix.
  Keine neue Zahl.
* Mess-Ring: zusaetzlich die gepackte RE2-Id (`re15_schuss_r36_fx_id`, 1 Zeile in re2z_gore_fx_core) fuer den Id-Pin.

### 10.6 Messung nachher
* **Sonde der Abnahme (abn2_blut.c unveraendert, neu gegen die Bibliothek dieses Stands gelinkt), 320 Laeufe**
  (`batch_neu.sh`, Klassierung `klassen3_nachher.txt`, Gegenueberstellung je Lauf `vergleich_k1.txt`):
  ```
  K1 Kopfabriss Part 0 @0x80102B24 a3 (0,-600,0): Spawns 32 in 32 Laeufen, d(gezeichnet T) min 0 max 0     (vorher 25x d 600 + 7x 601..896)
  K2 Hals-Stumpf Part 8 @0x80102BBC a3 (0,100,0): Spawns 282 in 32 Laeufen, d(gezeichnet T) min 0 max 0    (vorher d 100)
  K9 alle uebrigen Spawns: Spawns 4470 in 320 Laeufen, d(gezeichnet T) min 0 max 361                         (361 = RIP-Kopf vs. gedriftet, bekannt)
  ```
  Klassen der Abnahme (`klassen.py` der Abnahme auf die neuen Laeufe, `klassen_abn_nachher.txt`), einzige Aenderungen:
  K1/K2 und "K6 w12 ev - part 0 ohne" (601..709) entfallen (jetzt a3, d 0); **Loco-Bank-Treffer w3/w4/w5/w6 TREF part 0/1
  bank 1: d max 30 -> 0** (die Restabstaende der Abnahme 2 kamen von derselben Ursache: Taumel-Schub in die Wand, Klemme im
  Schwanz — z.B. w3 nx eben p4 F102 fx (-740,...) d 25 -> (-715,...) d 0). Alle uebrigen Klassen bitgleich.
  Lauf-Folgen: in allen 320 Laeufen gleiches Todesbild und gleiche Spawn-Zahl wie vorher (`lauf_diff.py`; die 4 Abweichungen
  der Zeilenzahl "part 8 fliegt" sind von stderr zerschnittene Zeilen, keine Verhaltensaenderung).
  Woertlich 12 px eben 2: `F192 ... part  0 fx=(568,-114,-19589) a3=(0,-600,0) | d(gezeichnet T)=0` (= die Original-Lage, die die
  Abnahme ausgerechnet hatte), `F193 ... part  8 fliegt fx=(547,-159,-19613) a3=(0,100,0) | d(gezeichnet T)=0`;
  19 pz eben 5: `F266 ... part  0 fx=(733,-117,-17406) a3=(0,-600,0) | d(gezeichnet T)=0 | d(T-1)=25` (vorher ERGEBNIS max d 896).
* **exe (echter Renderer), derselbe E2f-Aufbau, Messkopie `re15_pc_r36c_nb3_neu.exe`:** Zustandsfolge bitgleich (state.log F226
  beide: Kriecher `[5 ... st=1 ss1=1 ss2=5 ... mo=16 af=22 ... @(-2728,-19774,r2199)] hp=226`, Leon `(-3812,-19450)` hp 70).
  fx.log: `id=5 sub=0 eidx=0 frame=1 w(-3302,-117,-19659) ... F=226` (vorher `id=0 sub=128 ... w(-2732,-849,-19781)`) = Fleischbrocken
  (Raum-Id 5) 117 ueber dem Boden, 585 vor dem Becken Richtung Leon = am Hals des liegenden Kriechers; Hals-Stumpf
  `id=0 sub=0 ... w(-3277,-162,-19642)` F227..F243 (vorher `w(-3376,-51,-19641)`). Bilder (`C_schuss_mess/n3/exe_e2f_vorher_F222-233.png`
  / `exe_e2f_nachher_F222-233.png`, Ausschnitt x0-130/y60-160, 3x, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1): vorher ab F226 der
  rote Blutballen in der Luft auf Kniehoehe Leons; nachher kein Ballen in der Luft, rote Brocken/Spritzer unten am Kopf des
  Kriechers auf dem Boden.

### 10.7 Tests
* Neu (tests/unit/probes/r36_schuss.cmake): `unit_r36_blut_pin_kriecher` (`probe_r36_blut pin kriecher`: Ingram, Platz 2, Typ 0x10)
  und `unit_r36_blut_pin_kriecher5` (`pin kriecher5`: MC51 von +z, Platz 5, Typ 0x11 — der Klemm-Fall). Die Varianten laden die
  Bank WIE DIE EXE (Hybrid, Abnahme 2 B3), zeichnen auch im Vorlauf (wie abn2_blut) und fahren bis zum Kriecher-Tod + 80 Bilder
  (kein Abbruch beim Kriecher). Pruefung: (G) jeder Spawn an der gezeichneten Matrix (Schranke 40, MG-Trefferbilder 100),
  Kopfabriss traegt Id 0x08001000, Hals-Stumpf 0x00001000, Zaehler Kriecher / Kopfabriss >= 1 / Hals-Stumpf >= 1.
  Neu (Ausgaben `C_schuss_mess/n3/pins/neu_*.txt`): `kriecher: F192 KOPFABRISS Id 0x08001000 d=0 ... Kriecher ab Bild 91:
  Kopfabriss 1, Hals-Stumpf 9 / probe_r36_blut: OK`, `kriecher5: F266 KOPFABRISS Id 0x08001000 d=0 ... probe_r36_blut: OK`.
  **Trennschaerfe** (Sonde gegen alte Objekte gelinkt, `pins/probe_v*_*.txt`): Stand vor N3 (enemy_ai_re2_zombie.c +
  enemy_ai_common.c aus 4adb4890): kriecher exit 1 (12 FAIL: `F192 Spawn part=0 ohne a3 liegt 600`, Hals-Stumpf 9x 100, kein
  Kopfabriss-/Hals-Stumpf-Spawn mit a3), kriecher5 exit 1 (`F266 ... liegt 896`). Nur ohne Nachfuehrung (enemy_ai_common.c aus
  4adb4890, Rest neu): kriecher OK, **kriecher5 exit 1 (`F266 Spawn part=0 mit a3 liegt 666`)** — der Pin traegt beide Teile.
* Die sieben bisherigen Pins unveraendert gruen, gleiche Kennzahlen wie §9.5 ((G) max d: eben 24, tief 32, mg 88, rip 0, arm 0,
  burst 0, glied 73).
* Gezielter ctest-Lauf: s. 10.9.

### 10.8 OFFEN (Stand Nachbesserung 3) + Fuer den Nutzer
* **OFFEN 13 (neu, fehlendes Blut, nicht Lage): Fress-Tropfen @0x80103C8C** — EXEC[8] P1 spawnt bei Bild-Wort & 0x10000
  (`lw v0,376(s1)` / `lw v0,0(v0)` / `lui v1,0x1` / `and` / `beq` @0x80103C50-64) Blut Id 0 Skala 5072 (`addiu a0,zero,5072`
  @0x80103C80) an Part 8 (`addiu a2,s0,1448` @0x80103C90) mit a3 (200,0,0) (@0x80103C68-7C), Winkel rand<<4 (@0x80103C84). Der Port
  hat den Spawn nicht ("Fress-Tropf-FX ... OPEN", enemy_ai_re2_zombie.c EXEC[8] P1). Am FRESSENDEN Zombie, nicht am getroffenen —
  nicht Teil von Punkt 2; Messweg: RE2-Bank-Fress-Clip 0x12/0x13 auf Bild-Worte & 0x10000 pruefen, dann a3-Emitter wie hier.
* OFFEN 9 praezisiert: die Feuer-Emitter (Id 5) tragen weiter Welt-Versatz (0,200,0) — Brand-Darstellung (Spur D/Granate), kein Blut.
* OFFEN 7, 8, 10, 12 (§5/§9.7) unveraendert.
* **Fuer den Nutzer:** Wenn Leon sich aus dem Beinbiss eines KRIECHENDEN Zombies befreit und ihm dabei den Kopf zertritt, erschien
  bisher ein roter Blutballen in der Luft auf Kniehoehe ueber dem liegenden Zombie (der Versatz wurde senkrecht statt den Koerper
  entlang gerechnet) — und es war der falsche Effekt (Blut mit verdrehter Unternummer statt RE2-Fleischbrocken). Jetzt spritzen wie
  in RE2 Fleischbrocken am Hals des liegenden Zombies, und der abgetrennte Kopf blutet am Halsende. Dazu: wenn ein Zombie im
  selben Bild, in dem er blutet, von einer Wand zurueckgehalten wird (Kriecher am Tisch, Taumel-Treffer an der Wand), sitzt das
  Blut jetzt am gezeichneten Koerper statt dort, wo die Bewegung ihn ohne Wand hingesetzt haette (gemessen bis 666 bzw. 30
  daneben). Keine neuen Assets, keine Sprachdateien, keine Bank-9-Bits / Nachrichten / Ereignisse.

### 10.9 Stand je Mangel (Abnahme 2), Tests, Commits
| Mangel / Fund | Ursache | Vorher (gemessen) | Beleg | Aenderung | Nachher (gemessen) |
|---|---|---|---|---|---|
| M1 Kopfabriss @0x80102B24 | Welt- statt Anker-Versatz | Sonde 25x d 600 (Abstand zur Original-Lage 937), exe F226 `w(-2732,-849,-19781)` | `addiu a3,sp,24` @0x80102AEC, (0,-600,0) @0x80102B10-1C, a2 Part 0 @0x80102B28 | re2z_gore_fx_a3 {0,-600,0} | 32/32 d 0; exe F226 `w(-3302,-117,-19659)` am Hals |
| M1 Hals-Stumpf @0x80102BBC | dito | 286x d 100 (148-149 zur Original-Lage) | `addiu a3,sp,24` @0x80102BA8, (0,100,0) @0x80102B8C-A0, a2 Part 8 @0x80102BC0 | re2z_gore_fx_a3 {0,100,0} | 282x d 0 |
| dazu: Id @0x80102B24 (Abnahme-Aussage "Id 0" widerlegt) | `lui a0,0x800` als 0x00800000 gelesen | fx.log `id=0 sub=128` | Bytes `00 08 04 3c` / `00 10 84 34` @0x80102AD4/DC = 0x08001000 | 0x08001000 (Id 8 -> Raum-Id 5) | fx.log `id=5 sub=0`; Pin prueft die Id |
| dazu: Anker-Zeitpunkt (Kriecher an der Wand, Taumel an der Wand) | Spawn mitten im Tick vor Schub/SCA-Klemme | 5 Kopfabrisse 601..896, Loco-Bank-Treffer bis 30 | FUN_80015cb8 @0x80015D58-90; RE2-Root `jal 0x8003567c` @0x80100638 | Nachfuehrung (marke + nachfuehren_alle) | alle d 0; Lauf-Folgen bitgleich |
| Zensus §8.8 C | a0 nicht ausgewertet | 2 Blut-Emitter als "kein Blut" | zensus3.txt | §10.4 | berichtigt; OFFEN 13 neu |
| Pin-Luecke | mg-Lauf bricht beim Kriecher ab | kein Pin | — | unit_r36_blut_pin_kriecher/_kriecher5 | gegen alten Stand rot (600/896 bzw. 666), neu gruen |

Punkte 1 und 3 (Abnahme 2: erfuellt): Code unberuehrt (re15_damage.c, enemy_ai_re2_crow.c, Sichtlinie, Fresser-Sperre, GIB);
ihre Riegel im gezielten Lauf (integration_r36_schuss, unit_r36_schuss, unit_r36_schuss_kraehe, unit_crow_death_corpse,
unit_re2_zombie_hitfilter/_abc, unit_re2_crow_ai).

Vertrag: gemeinsame Dateien nur mit Haken — enemy_ai_re2_zombie.c (die zwei Emitter-Zeilen + 3 Haken-Zeilen fx_nach/fx_id in
den Blut-Anker-Helfern, VERTRAG §1.4 "C = Blut-Anker"), enemy_ai_common.c (+3 Zeilen: include, marke, nachfuehren_alle);
eigene Dateien schuss_r36.c / re15_schuss_r36.h / probe_r36_blut.c / probes/r36_schuss.cmake; probe_re2z_stomp.c nur ein
Kommentar (Id). Keine Bank-9-Bits, Nachrichten-IDs, Ereignisse, Assets. `git merge-tree --write-tree HEAD <zweig>` gegen alle
17 r36-Zweige (elza, gegnerzahl, granate, integration, inventar, karte, kartenitems, kleidung, klettern, pri, schluessel, story,
szene1150, szenenleon, tueren, ziel1190, zombieki): alle rc=0, 0 CONFLICT (Stand 6b21dd7b; ab4187c3 aendert nur schuss_r36.c).
Hinweis fuer die Integration: r36/zombieki fuegt hinter re15_enemy_sca_clamp re15_r36_wurf_klemme ein — die Nachfuehrung am
Ende von run_all erfasst diese Lage-Korrektur mit.

Messkopien: re15_pc_r36c_nb3_alt.exe / _neu.exe nach den Laeufen geloescht; zwei Kopien re15_pc_r35c/r35l_haken_* aus
abgebrochenen eigenen ctest-Laeufen geloescht (die aelteren re15_pc_r35e/r35k_haken_* von 15:44/16:49 stammen nicht von hier).
