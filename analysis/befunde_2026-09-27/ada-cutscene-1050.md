# ROOM1090 -> ROOM1050: "Adas Animation wird am Anfang mehrfach ausgeloest"

Nutzer (nach v0.8.13): „Dann ist beim Start der cutscene mit leon und ada beim Wechsel von
room 1090 zu room 1050 erneut am Anfang adas animation mehrfach ausgeloest."

## 0. Kurzfassung

**Ursache gefunden und behoben.** Die NPC-Executor-Sub-VM des Ports schob den Clip-Cursor
`+0x95` **vor** dem Posieren vor. Das Original posiert das **aktuelle** `+0x95` und schiebt
erst danach vor. Folge im Port: jeder Clip begann bei Bild 1 statt 0 — und ein
**abspiel-einmal-Clip endete auf seinem Bild 0**, sprang also im letzten Bild sichtbar auf
seine ANFANGSPOSE zurueck, bevor der naechste Clip anfing. Genau das sieht der Nutzer bei
Adas Cutscene-Start als „Animation mehrfach ausgeloest".

Riegel `unit_r27_ada_cutscene_1050`, am alten Stand **4 rote Pruefungen** (s. §5).

## 1. Reproduktion — der ECHTE Weg, nicht der Raum-Sprung

`analysis/befunde_2026-09-27/run_ada.sh`, echte exe aus diesem Arbeitsbaum, echter
Renderpfad (kein AUTOSHOT, kein Softwarerenderer):

    RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=… RE15_TITLE_SHOT_AF=2
    RE15_DEBUG_JUMP=1090@gp RE15_SET_FLAG=3:0x6e RE15_AP_CUTSCENE_CLICK=1
    RE15_FIRE_AOT=0@90#1090 RE15_ANIM_TRACE=… RE15_STATE_LOG=…

Der Lauf faehrt das Intro durch, springt nach ROOM1090 und nimmt **Tuer-Slot 0** — dieselbe
Kette, die auch das Hineinlaufen nimmt:

    [fire-aot] slot=0 at F90 (Raum 1090)
    [aot] DOOR FIRE slot=0 rect=(-10080,3060,hw=2350,hh=800) target_cut=6 spawn=(19850,0,-22300)
    [room] PC loaded room1050.rdt (297324 bytes)
    [scd F90] Cut_chg(6)
    [scd] Plc_dest(slot=1 mode=0x09 dest=(16250,-16650)) -> state4/sub9
    [scd] Plc_dest(slot=1 mode=0x05 dest=(16250,-16650)) -> state4/sub5
    [scd] Plc_dest(slot=1 mode=0x05 dest=(16250,-14200)) -> state4/sub5

### 1.1 Der Posen-Strom (`RE15_ANIM_TRACE`) — Adas Aktor, Bild fuer Bild

Spalten `Bild Aktor Typ motion clip fc cur slot …`, Aktor 1 = Ada (Typ 0x42).
Zustandsspur desselben Laufs daneben (`ss1`/`ss2` = `+0x05`/`+0x06`):

| Bild | Clip | Bild-im-Clip (IST, alt) | ss1/ss2 | wer setzt den Clip |
|---|---|---|---|---|
| F0..F5   | 2 | 0            | 9/0 | Tuer-Blende, Pausemaske `0xFF000000` friert KI+VM ein |
| F6..F8   | 5 | **1,2,3**    | 9/1 -> 6/0 | Sub-9-Turn, Phase 0 `+0x94=5` @0x80051d3c |
| F9..F24  | 1 | **1..15,0**  | 6/1 | Sub-6 EVENT-REACH, Phase 0 `+0x94=1` @0x80051844/54 |
| F25..F34 | 2 | **1..10**    | 6/3 | Sub-6 Phase 2 `+0x94=2` @0x800518b4/c8 |
| F35..F64 | 0 | **1..21,0,1..8** | 5/2 | Sub-5 Phase 1 aligned `+0x94=0` @0x8005157c |
| F65..F76 | 0 | **1..12**    | 5/2 | zweiter `Plc_dest` @RDT 0x0E18 |

Rotationsspur der Turn-Phase aus `RE15_STATE_LOG`: `r3072 -> 2976 -> 2880 -> 2821`
(Slew 96 = Cone-Tabelle @0x80076c41, Byte `[0x42-0x40]*2` = 96, roh gelesen:
`0x80076c40: 0,96,0,96,0,96,…`).

**Jeder Clip beginnt bei 1, und Clip 1 endet auf 0.** Das ist der Defekt.

## 2. Das Original — mit Adressen und Bytes

Alles selbst aus `info/Re1.5/PSX.EXE` bzw. `info/Re1.5/PSX/BIN/STAGE1.BIN` disassembliert
(`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`), kein Decompilat.

### 2.1 ROOM1050s Cutscene-Subs (`re15_port/shared_assets/PSX/STAGE1/ROOM1050.RDT`)

    sub00  0x00C8A  21 03 6e 01        Ck(bank 3, bit 110, 1)
           0x00C8E  44 00 42 40 …      Sce_em_set Typ 0x42 @(18550,0,-22300) dir 3072
           0x00CA2  04 ff 18 03        Evt_exec sub03
    sub03  0x00D96  2e 01 00           Work_set(1,0)  = Spieler
           0x00D9A  34 00 8a 4d        Member_set 0 = 19850   (+0x34 = X @0x80041194)
           0x00D9E  34 02 e4 a8        Member_set 2 = -22300  (+0x3C = Z @0x800411a4)
           0x00DA2  34 04 00 0c        Member_set 4 = 3072    (+0x6A = yaw @0x800411b4)
           0x00DA6  2e 02 00           Work_set(2,0)  = Ada
           0x00DAA  40 00 09 21 7a 3f f6 be   Plc_dest mode 9  Flag-Bit 0x21  Ziel (16250,-16650)
           0x00DB2  09 0a 1e 00        Sleep(30)
           0x00DB6  04 ff 18 04        Evt_exec sub04
    sub04  0x00E00  2e 02 00           Work_set(2,0)
           0x00E04  40 00 05 21 7a 3f f6 be   Plc_dest mode 5 (RUN) Ziel (16250,-16650)
           0x00E0C  11 00 08 00 / 02 / 12 04 / 21 05 21 00   do { Evt_next } while(flag(5,33)==0)
           0x00E18  40 00 05 21 7a 3f 88 c8   Plc_dest mode 5 Ziel (16250,-14200)
           0x00E20  11 00 08 00 / 02 / 12 04 / 21 05 21 00
           0x00E2C  34 00 52 03 / 0x00E30 34 02 88 c8   Member_set X=850, Z=-14200 (Ada raus)

Der Verdacht aus Runde 26 („sub04 @Datei 0x0E18") ist damit **bestaetigt als Ausloeser des
zweiten Lauf-Starts bei F65** — aber er ist byte-true (§4), nicht der Defekt.

### 2.2 Wie das Original eine NPC-Animation im Cutscene-Kontext startet

`Plc_dest` @0x80041be4 schreibt `+0x04 = 4` und `+0x05 = mode` und nullt die Phase `+0x06`:

    80041bec: lhu v0,452(a1)     ; +0x1C4
    80041bf8: andi v0,v0,0x4     ; LOOP-Bit
    80041bfc: beq v0,zero,0x80041c14   ; nicht gesetzt -> IMMER neu initialisieren
    80041c04: lbu v0,5(a1)       ; +0x05
    80041c0c: beq v0,a2,0x80041c24     ; LOOP gesetzt UND gleicher Sub -> Reset UEBERSPRINGEN
    80041c14: sb v0(=4),4(a1)    ; +0x04 = 4
    80041c18: sb a2,5(a1)        ; +0x05 = mode
    80041c1c: sb zero,6(a1)      ; +0x06 = 0   PHASE
    80041c4c: sh zero,452(a1)    ; +0x1C4 = 0  (UNBEDINGT)

Ada (Typ 0x42) laeuft damit in den **gemeinsamen State-4-Executor**: ihre Overlay-Wurzel
0x8011cb70 dispatcht `+0x04` ueber die Tabelle @0x80121668, und `[4] = 0x80050be8` ist der
EXE-Executor. Dessen Sub-Tabelle @0x80076ca0: `[5] = 0x80051484` (RUN), `[6] = 0x800517f0`
(EVENT-REACH), `[9] = 0x80051cf8` (TURN).

Phase 0 jedes Subs setzt Clip + Cursor und **faellt im selben Tick in den Body durch**:

    Sub 9  80051d2c sb 1,6(a0)   / 80051d3c sb 5,148(v1)  / 80051d4c sb zero,149(v0)
           80051d5c sb 7,143(v1) -> Body @0x80051d60 (arc_test, steer, f314)
    Sub 5  80051508 sb 1,6(v1)   / 80051518 sb 5,148(v1)  / 80051528 sb zero,149(v0)
           80051538 sb 7,143(v1) -> Body @0x8005153c ; aligned: 8005157c sb zero,148(v0)
                                     (Clip 0 = Lauf) / 8005158c sb zero,149(v0)
    Sub 9 ausgerichtet: 80051dac sb 6,5(v1) (+0x05 = 6 = EVENT-REACH), 80051dbc sb zero,6(v0),
           80051dd8 jal 0x8004ef90(0x800b1028, +0x1c3)  = SCD-Flag Bank 5 setzen

### 2.3 POSE-DANN-VORSCHUB — der Kern

`anim_set` FUN_8001f314 posiert **den aktuellen** `+0x95`:

    8001f324: lbu v0,148(t0)     ; +0x94 = Clip
    8001f35c: lbu v0,149(t0)     ; Index = +0x95, UNVERAENDERT
    8001f368: addu a2,v1,v0
    8001f36c: sw  a2,360(t0)     ; +0x168 = Zeiger auf das Frame-Wort  <<< die POSE

und erst der Keyframe-Integrator FUN_8001f3bc erhoeht ihn an seinem **Ende**:

    8001f610: lbu v0,149(v1)
    8001f618: addiu v0,v0,1
    8001f61c: sb  v0,149(v1)     ; +0x95 += 1
    8001f624: sltu v0,v0,s4      ; s4 = Bildzahl
    8001f628: bne v0,zero,0x8001f640   ; noch drin -> Rueckgabe 0
    8001f63c: sb  zero,149(v1)   ; sonst +0x95 = 0 und Rueckgabe 1 = "Clip zu Ende"

Der erste Tick eines Clips posiert also **Bild 0**, der letzte **Bild fc-1**, und die
Rueckgabe 1 („Clip fertig", die den Sub weiterschaltet) faellt in dem Tick, der fc-1
posiert hat. Eigener Zensus (Runde 26, §2.7 des Vorgaenger-Dossiers): von allen 57
Zugriffen auf Offset 149 inkrementieren genau ZWEI (0x8001F61C, 0x8001FB58) — `+0x95`
bewegt sich ausschliesslich innerhalb eines `anim_set`-Aufrufs.

## 3. Was der Port tat

`re15_port/engine/src/enemy_ai_common.c`, `re15_npc_anim()`:

    int done = (e->anim_frame + 1 >= fc);
    e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);

Der Vorschub lief **vor** dem Posieren (der Renderer liest `a->anim_frame` erst nach dem
KI-Tick — `re15_compute_actor_kf`, anim_select_common.c). Damit:

* erster Tick eines Clips = Bild 1 (Bild 0 wird nie gezeigt),
* letzter Tick eines abspiel-einmal-Clips = das bereits gewrappte **Bild 0** statt fc-1
  — die Animation springt in ihrem letzten Bild auf die Anfangspose zurueck.

Der globale Advancer `re15_actors_anim_advance` (player_common.c) laesst NPC-Familie in
State 4/1 aus, `re15_npc_anim` ist dort also der einzige Stepper — der Fehler betraf jeden
NPC-Executor-Clip (Ada, Elliot, Marvin, Irons …), sichtbar aber vor allem an den kurzen
Gesten-Clips einer Cutscene.

## 4. Was NICHT der Defekt ist (geprueft, nicht vermutet)

* **Runde-26-Aenderung an `op_plc_motion`** (`motion_init_delay`/`anim_freeze` auch bei
  gleicher Clip-Nummer, @0x80041bb4 / @0x80050cec..d0c): betrifft Ada **nicht**. Im ganzen
  ROOM1050-Lauf geht jeder `Plc_motion` an **slot=0** (den Spieler) — Ada wird
  ausschliesslich ueber `Plc_dest` gefahren (debug.log des Messlaufs, §1).
* **Der zweite Lauf-Start bei F65** (`Plc_dest` @RDT 0x0E18) ist **byte-true**: der
  Re-Init-Guard @0x80041bf8-c0c greift nur mit `+0x1C4 & 0x04`, und dieses Bit ist fuer Ada
  0 — `Plc_dest` nullt `+0x1C4` unbedingt @0x80041c4c, im ganzen Sub-5-Body
  (0x80051484..0x800517e8) steht kein Store auf 452, und ROOM1050s sub04 setzt kein
  `Plc_flg`. Das Original faehrt dort also ebenfalls Phase 0 neu (`+0x94=5` @0x80051518,
  `+0x95=0` @0x80051528) und danach Clip 0 ab Bild 0.
* **Die Ankunfts-Flags (Bank 5) sind kein Leck.** Bank 5 liegt @0x800b1028
  (Bank-Basistabelle @0x80074664[5], gelesen vom `Ck`-Handler @0x8003fcf4). Wort 1
  (Bits 32..63, @0x800b102c) wird **am ENDE jedes VM-Durchlaufs** geloescht:
  `jal 0x8003ebf4` @0x8003f18c (hinter der 10-Slot-Schleife des Dispatchers
  FUN_8003f0a0) -> `sw zero,4140(at)` @0x8003ec1c. Ein Ankunfts-Bit lebt also genau ein
  Bild. Der Port erreicht dasselbe, indem `op_plc_dest` das Bit beim Absetzen loescht.
* **Die EVENT-REACH-Geste (Clip 1) nach dem Turn** ist byte-true: der Turn haendet beim
  Ausrichten unbedingt an Sub 6 weiter (`sb 6,5(v1)` @0x80051dac).

## 5. Der Fix + der Riegel

`re15_npc_anim` honoriert jetzt den Saat-Tick (`motion_init_delay`, dieselbe Rolle wie beim
Spieler, game_step_common.c:335) und prueft das Clip-Ende am **posierten** Bild:

    if (e->motion_init_delay > 0) e->motion_init_delay--;   /* Saat-Tick: +0x95 bleibt stehen */
    else e->anim_frame = (uint8_t)((e->anim_frame + 1) % fc);
    int done = (e->anim_frame + 1 >= fc);   /* posiertes Bild == fc-1 -> @0x8001f63c Rueckgabe 1 */

Jeder Clip-Start der Executor-Subs saet ueber `re15_npc_seed_clip()`
(`+0x95 = 0`, `+0x8f = 7`, Saat-Tick): Sub 9 Phase 0 (@0x80051d3c/@0x80051d4c),
Sub 4/5/7/8 Phase 0 (@0x80051518/@0x80051528), Sub 5 aligned (@0x8005157c/@0x8005158c),
Sub 6 Phase 0 und 2 (@0x80051844/@0x800518b4), `re15_npc_clip`.

**Gemessen NACH dem Fix** (derselbe Lauf, dieselbe Schiene):

    F6..F8   Clip 5  0,1,2
    F9..F24  Clip 1  0..15        <- endet auf 15, nicht mehr auf 0
    F25..    Clip 2  0..
    F35..    Clip 0  0..21,0..

**Riegel** `unit_r27_ada_cutscene_1050`
(`re15_port/tests/unit/test_r27_ada_cutscene_1050.c`, Probe-CMake
`re15_port/tests/unit/probes/r27_ada-cutscene-1050.cmake`): faehrt ROOM1050s Cutscene-Start
(Flag(3,0x6e) + `scd_room_reenter`, danach SCD-VM -> Entity-Schleife in der
Original-Reihenfolge @0x8001cdec/@0x8001ce04) und prueft M1 Clip 5 startet auf 0,
M2 Clip 1 startet auf 0, M3 Clip 1 endet auf 15, M4 Clip 2 startet auf 0.

**Gegenprobe am ALTEN Stand gefahren** (Fix zurueckgenommen, gebaut, gemessen,
wiederhergestellt): der Posen-Strom ist `5:1,2,3 / 1:1..15,0 / 2:1..10 / 0:1..` und der
Riegel meldet **4 rote Pruefungen** (M1=1, M2=1, M3=0, M4=1), Exit 1.

## 6. Offen / nicht belegt

* **Keine PSX-Gegenmessung.** Der Beleg ist die Disassembly (§2.3), nicht ein Lauf auf
  DuckStation. Ein Savestate von ROOM1050 mit gesetztem Flag(3,110) existiert im Repo
  nicht, und der Debug-Menue-Sprung setzt das Tor nicht.
* **Nicht gemessen**, wie gross die sichtbare Pose-Differenz zwischen Clip-1-Bild 15 und
  Bild 0 von EM42 ist (die Geste ist 16 Bilder lang; der Ruecksprung ist ein Bild).
* **Nicht Teil dieses Auftrags** (Nebenbefund, §4 dritter Punkt): der Port bildet den
  Bank-5-Reset als „loeschen beim `Plc_dest`" nach statt als „loeschen am Ende jedes
  VM-Durchlaufs" (@0x8003f18c -> @0x8003ec1c). In den gemessenen Raeumen deckungsgleich,
  aber nicht dieselbe Regel.
