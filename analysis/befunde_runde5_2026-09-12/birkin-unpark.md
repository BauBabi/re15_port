# REGRESSION v0.7.87: Endkampf-Birkin (0x36, ROOM5090) unsichtbar — der Park-Code frisst das sub04-Pos_set

Statisch geklaert 2026-09-12 (nur Read/Grep + python-SCD-Decode; kein Build, kein Lauf).

## Kernbefund (die Nutzer-Hypothese ist zu korrigieren)

**Die Freigabe (Member_set grid=0x13) KOMMT im Port an.** Was verloren geht, ist die
**Kampfposition**: ROOM5090-sub04 hat zwischen `Pos_set(1200,0,-23350)` und
`Member_set(0x0c,0x13)` ein **`Sleep(1)`** (RDT-Datei-Offset **0x1306**: `09 0a 01 00`).
In diesem 2-Tick-Fenster laeuft die Gegner-KI weiter, `grid_id` ist noch 0x33, und der
neue Park-Code (enemy_ai_common.c:11052-11056) **ueberschreibt jeden Tick x/z mit
(-32000,-32000)** — das Pos_set ist weg, bevor die Freigabe greift. Der Boss wird
anschliessend korrekt freigegeben, steht aber bei (-32000,-32000): Emergence spielt
ausserhalb der Welt, danach 33 km Fussmarsch → fuer den Spieler dauerhaft unsichtbar.

## Belege

### 1. sub01 laeuft als Dauerschleife und startet sub04 (Frage 1+2)

ROOM5090.RDT (`re15_port/shared_assets/PSX/STAGE5/ROOM5090.RDT`, Header: main_scd @0x104C,
sub_scd-Tabelle @0x1220, 5 Subs @0x122A/0x1284/0x12C2/0x12DA/0x12F2; selbst dekodiert mit
den s_opcode_sizes aus scd_vm.c:167):

* **Niemand startet sub01 per Evt_exec** — weder main00 (@0x104E..0x121E: nur 6 Door_aot_set
  + 17 Sce_espr_on + Evt_end) noch sub00. sub01 ist der **Per-Frame-Sub**: der Port reseedet
  Thread-Slot 1 in JEDEM Gameplay-Frame unbedingt auf sub_scd[1]
  (**scd_vm.c:600-601**, byte-true FUN_8003f038 @0x8003f064/70/84; Memory
  reai-v2-scd-per-frame-model). Keine Do/Edwhile/Goto-Schleife noetig — der Waechter ist
  jeden Frame frisch.
* sub01-Bytes @0x1284:
  `06 00 1a 00` (If) / `21 03 2a 00` (Ck Bank3 Flag0x2a ==0) / `06 00 10 00` (If) /
  **`23 00 0a 00 0c 00`** (Cmp work_vars[0x0A]==12; op_cmp scd_vm.c:3052: var=pc[2]=0x0a,
  op=pc[3]=0 "==", imm LE=12) / **`04 ff 18 04`** (Evt_exec) / `22 03 2a 01` (Set Flag 0x2a=1,
  Einmal-Riegel).
* op_evt_exec (scd_vm.c:1006): cond=pc[1]=0xff ≥ SCD_EVENT_SLOT_FIRST(10) → Auto-Slot 10..23,
  sub_id=**pc[3]=4** (pc[2]=0x18 wird nicht gelesen — die (0xff,18,NN)-Form ist damit exakt
  abgebildet). Die Gestrichen-Liste blockt nur sub02 (5090/5091), NICHT sub04
  (scd_vm.c:1027-1029). Slot ≥10 > Spawner-Slot 1 → sub04 laeuft noch im SELBEN Tick
  (Slot-Iteration, scd_vm.c:614ff).
* work_vars[0x0A] ist der ANGEZEIGTE Cut (room_common.c:51-64, @0x800214f8/0x80021bfc);
  befund.log-Spalte `C` = active_cut_idx wird aus demselben Apply gespeist (main.c:4631-4635).
  C12 im Log ⇒ wv[0x0A]==12 ⇒ Cmp wahr ⇒ sub04 startet.

### 2. sub04 trifft den richtigen Slot (Frage 3)

sub04-Bytes @0x12F2: `22 01 1b 01` / `22 02 07 01` / **`2e 02 01`** (Work_set kind=2, idx=1)
/ `00` / **`32 00 b0 04 00 00 ca a4`** (Pos_set 1200,0,-23350) / **`09 0a 01 00`** (Sleep 1!)
/ **`34 0c 13 00`** (Member_set 0x0c,0x13) / `29 0f` (Cut_chg 15) / ... / `40 00 09 2c 01 98 a4`
(Plc_dest) / Do/Edwhile / `46 04 ...` (Aot_reset) / `09 0a 1e 00` / `29 0c` (Kampfkamera) ...

* scd_work_bind (scd_vm.c:2827): kind==2 → slot = idx+1 = **2** (aktiv-Check g_actors[2]).
  Boss-Spawn: sub00 @0x124A `44 01 30 33 00 00 00 ff 94 c6 00 00 ca a4 ...` = Sce_em_set
  em-Slot 1, Typ 0x30 (→Port 0x36), grid 0x33, Pos (-14700,0,-23350) → Aktor-Slot 2
  (em idx+1; in Runde 4 Messlauf B bereits dynamisch bestaetigt: "Work_set(2,1)→Slot 2").
* op_member_set (scd_vm.c:2975): value LE @pc[2]=0x0013, ws=work_slot=2 →
  re15_actor_set_member(2, 12, 0x13) → **case 12: a->grid_id** (actor_common.c:155/165,
  Zeile `case 12: a->grid_id = (uint8_t)value` — actor_common.c:153ff). Die KI liest DENSELBEN
  Struct: `re15_actor_t *e = &g_actors[slot]` (enemy_ai_common.c:11020). Freigabe trifft.

### 3. Pos_set wirkt — und wird vom Park zerstoert (Frage 4, der Defekt)

* op_pos_set (scd_vm.c:1924): schreibt x/y/z LE auf den Work-Slot → g_actors[2].x=1200,
  z=-23350. ✔
* Frame-Ordnung: `scd_vm_tick()` (main.c:4279) laeuft VOR `re15_game_step` (main.c:5503),
  dessen ENDE `re15_enemy_ai_run_all` ruft (game_step_common.c:1896). Ablauf:
  * Tick T: SCD: Work_set/Pos_set (Boss @1200,-23350), Sleep(1)@0x1306 → yield.
    KI danach im selben Frame: grid==0x33 → **PARK: x=z=-32000** (enemy_ai_common.c:11053).
  * Tick T+1: SCD: Sleeping 1→0, yield (op_sleeping return 2). KI: parkt erneut.
  * Tick T+2: SCD: Member_set → grid=0x13. KI: frei — aber bei (-32000,-32000).
* Ohne das RDT-eigene Sleep(1) gaebe es das Fenster nicht (Pos_set+Member_set im selben
  SCD-Tick, KI saehe schon grid 0x13). Das Sleep macht den Per-Tick-Park zum Positionskiller.
* Der Wall-Clamp-Wrapper (enemy_ai_common.c:13641-13646) aendert daran nichts:
  collision_constrain_impl (re15_collision.c:696) ist reines Zellen-Push-out; bei
  (-32000,-32000) ueberlappt keine SCA-Zelle → Parkposition bleibt (empirisch konsistent:
  der Nutzer sieht den Boss ja nicht irgendwo festgeklemmt, sondern gar nicht).

## Patch-Plan (minimal-invasiv, EINE Bedingung)

**re15_port/engine/src/enemy_ai_common.c:11052** — Park nur, solange der Boss am
RDT-Spawn ODER bereits am Parkplatz steht; jede andere Position (= das sub04-Pos_set)
beendet den Park sofort:

```c
if (e->grid_id == 0x33 &&
    ((e->x == -14700 && e->z == -23350) ||   /* RDT-Spawn: ROOM5090.RDT sub00 @0x124A (94 c6 / ca a4) */
     (e->x == -32000 && e->z == -32000))) {  /* schon geparkt: RE2-Parkposition @0x801011d0-dc */
```

Wirkung: vor dem Event parkt der erste KI-Tick den Boss vom Spawn (-14700,-23350) nach
(-32000,-32000) und haelt ihn dort (x==-32000-Zweig). Beim Kampfstart setzt Pos_set
(1200,-23350) → Park-Bedingung ab sofort falsch → die FSM laeuft ab Tick T an der
Kampfmarke (INIT war nie fortgeschritten, nib 3 von grid 0x33 wie 0x13 → EMERGENCE an
der Kampfmarke, exakt das Runde-4-Design); Member_set 2 Ticks spaeter ist reine Formsache.
Kommentar im Block um den Sleep(1)-Befund ergaenzen (Offsets 0x12FE/0x1306/0x130A).

**Test-Hinweis fuer den Hauptprozess:** Zustand mit Flag(3,0x2a)==0 verwenden (frischer
Raumeintritt). Ist das Flag in einem Save schon verbraucht (Event feuerte im regressierten
Build), spawnt sub00 den em1 nicht mehr (If Ck(3,0x2a,0) @0x123E) — Boss fehlt dann
UNABHAENGIG vom Fix. Messung: RE15_BIRKIN_DBG=1; Soll im birkin_dbg.log: pos=-32000 bis
Event, dann pos=(1200,-23350) STABIL, grid 0x33→0x13 zwei Ticks nach dem Pos_set,
EMERGENCE an der Kampfmarke.
