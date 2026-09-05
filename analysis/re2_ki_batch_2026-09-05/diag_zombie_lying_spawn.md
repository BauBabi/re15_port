# DIAGNOSE — SYMPTOM 4: RE2-Modus, Raumwechsel 1050→1030: Zombies starten LIEGEND, stehen dann „mit einem Clip“ plötzlich

Datum 2026-09-05. Alle @0x-Zitate unten habe ich in dieser Sitzung SELBST disassembliert
(`re2_disasm.py` gegen `info/re2leon/COMMON/BIN/EMZ0.BIN` bzw. `info/re2leon/PSX.EXE`;
`re15_disasm.py` gegen `info/Re1.5/PSX/BIN/STAGE1.BIN`) bzw. selbst aus den Datei-Bytes
gelesen (ROOM1030.RDT, CDEMD0.EMS). Port-Zitate sind datei:zeile des aktuellen Arbeitsbaums.
Kein Build, keine Datei geändert.

---

## Kernbefund (ein Satz)

Die 1030-Zombies liegen nie LOGISCH — sie werden nur LIEGEND GERENDERT: der Port sät beim
Sce_em_set-Spawn den byte-true **RE1.5**-Posen-Clip `0x27` (39) in `motion`, die RE2-Anim-Bank
hat aber nur **31 Clips**, und der port-erfundene Modulo-Fallback des Renderers macht daraus
`39 % 31 = 8` = den **RE2-Boden-/Liege-Clip 8** — sichtbar während des Türwechsel-Freezes
(Fade ~6 Frames + INIT-Tick), bis EXEC[1] im zweiten KI-Tick den Walk-Clip setzt = der
„plötzlich stehen“-Pop.

---

## 1. RAUMDATEN ROOM1030 (selbst geparst)

`re15_port/shared_assets/PSX/STAGE1/ROOM1030.RDT`, mainScdStart @Header 0x40 = 0x1C5C,
main00 @Datei 0x1C5E. Vollständig linear disassembliert (SCD-Opcode-Tabelle des Skills):

* @0x1DE2: `24 12 06 00` = **Save(0x12, 6)** — Gleichzeitig-Cap 6 (byte-true Gate
  @0x80042214-3C, s. scd_vm.c:3294-3319). Von 20 Records spawnen also 6.
* @0x1DE6..0x1F76: **20× Sce_em_set (0x44), ALLE `type=0x16, behavior=0x0D`**, ungegated
  (kein Ck davor — läuft bei JEDEM Betreten identisch, Erstbesuch wie Re-Entry).
  Beispiel Record 0 (roh): `44 00 16 0d 00 00 00 01 00 ee dc 00 00 1a 93 00 00 00 0c 00`.
* sub00 (@0x1FE8): IF `Ck(4,15,1)` → For-Schleife über 20 Slots, positioniert die ersten 6
  um (Pos_set) und setzt `Member_set(4, 0x000C)` = **rot_y := 12** (Member 4 = +0x6A rot_y,
  actor_common.c:145 `case 4: a->rot_y` — KEIN Posen-/Clip-Write). ELSE-Zweig analog mit den
  main00-Koordinaten. Keine weiteren Zombie-Spawns in sub00..sub11 (alle 12 subs
  disassembliert; die 0x44-Kandidaten außerhalb der SCD-Regionen sind Grafikdaten-Rauschen).
* ROOM1050 hat **null** Zombie-Spawns (Zensus unten) — passt zum Report: Der Nutzer kommt aus
  einem zombiefreien Raum und sieht die 1030-Spawns beim Betreten.

**Was RE1.5 aus behavior 0x0D macht** (STAGE1.BIN, selbst disassembliert):

```
80100f20: lbu v0,9(a0)         ; +0x9 behavior
80100f24: ori v1,zero,0xd
80100f28: andi v0,v0,0x1f
80100f2c: bne v0,v1,0x80100f60
80100f30: ori v0,zero,0x27     ; (Delay-Slot)
80100f34: sb v0,148(a0)        ; +0x94 = Clip 0x27
80100f40: ori v0,zero,0x201
80100f44: sw v0,4(v1)          ; Zustandswort 0x201 = ENGAGE (stehend, läuft an)
80100f54: sb zero,9(v0)        ; +0x9 = 0
```

→ RE1.5-Flavor: **stehend/anlaufend** (ENGAGE). Der Port bildet das ab in
`re15_enemy_ai_live_init` (enemy_ai_common.c:1847-1848: `sel==0x0d → sub_state_1=0x02,
grid_id=0, motion=0x27`) und sät den Clip zusätzlich schon beim Spawn
(`re15_enemy_spawn_action`, scd_vm.c:3252: `sel==0x0D → clip 0x27`; angewendet scd_vm.c:3348
`a->motion = re15_enemy_spawn_action(...)`).

**Was der RE2-INIT-Remap daraus macht** (enemy_ai_re2_zombie.c:7138-7139):
`else if (sel == 0x0d) { re15_ai_set_state_word(e, 0x101); }` = **WALK sub 1** — also
ebenfalls stehend. **Der Zustands-Remap ist NICHT die Ursache.** Die Zombies liegen nie im
Zustandsmodell; nur die RENDER-Seite zeigt Liegen (s. §3-Mechanismus unten).

## 2. PORT-REMAP-TABELLE (Zitat) und der Original-Default

`re2z_init` @ enemy_ai_re2_zombie.c:6932-7143, Remap ab :7054:

| RE1.5 behavior | Port-Ziel | Zeile |
|---|---|---|
| (sel 1 od. 3) && beh&0x80 | Kriecher `re15_re2z_enter_crawler` | :7058-7074 |
| sel 6 | `0x801` FEEDING + `+0x10E=0x4004` (sh @0x80100A88-8C) + Clip-Seed 18 (@0x80100AD0-DC) | :7075-7081 |
| sel 8, 0x0B, 0x0E, oder lying_family {1,3,4,5,7,8,9,0xA} && beh&0x80 | `0x701` LIEGEND + `+0x10E=0x4002` (sh @0x80100A34-38); 0x0E → RE1.5-Sitz-Import | :7082-7137 |
| sel 0x0D | `0x101` WALK | :7138-7139 |
| **DEFAULT** | **`0x1` = IDLE sub 0 (stehend)** — `sw 1,4(s2)` | :7140-7141 |

**Der Default ist korrekt NICHT „liegend“.** Original-Beleg, selbst disassembliert
(EMZ0.BIN, INIT @0x8010065C): das ERSTE, was der INIT nach dem Prolog schreibt, ist

```
801006ac: addiu v0,zero,1
801006b8: sw v0,4(s2)          ; Zustandswort = 1 (state 1 / sub 0 IDLE) — DEFAULT
```

Danach dispatcht der INIT auf den **Spawn-Deskriptor `+0x10E & 0x3F`** (nicht auf ein
RE1.5-behavior — RE2-Raumdaten schreiben +0x10E im Spawn-Handler, s.u.):

```
801009c4: lhu v1,270(s2)       ; +0x10E
801009cc: andi v1,v1,0x3f
801009d0: bne v1,v0(=2),...    ; ==2 -> Clip 22, 0x701 (sw @0x801009e8), +0x21A|=4
801009d4: sw a1,332(s2)        ; DELAY-SLOT, laeuft IMMER: +0x14C Clip-Wort = 1  (a1=1 @0x801009b8)
  ==4 @0x801009f4: Clip 23 (sw @0x80100a08), 0x701 (@0x80100a0c-10)
  ==5 @0x80100a14: Clip 22, 0x701, +0x10E=0x4002 (@0x80100a34-38), HP=-1 (@0x80100a3c-40)
  ==7 @0x80100a64: Clip 23, 0x701, +0x10E=0x4004 (@0x80100a88-8c), HP=-1
  ==6 @0x80100aa8: 0x101 (sw 257 @0x80100ab8-bc)     ; WALK
  ==8 @0x80100ac0: Clip 18 (@0x80100ad0-d4), 0x801   ; FEEDING
  &1  @0x80100ae0: Kriecher-Zweig (Clip 23 @0x80100af8-fc, ...)
```

**Tragend: JEDER Original-Pfad sät im SELBEN Tick das Clip-Wort `+0x14C`** (Default 1 via
Delay-Slot @0x801009D4; 22/23/18 in den Spezialzweigen). Und schon der **RE2-Spawn-Handler**
(RE2 PSX.EXE, Sce_em_set-Handler 0x8005714C, selbst disassembliert) sät:

```
8005734c: lhu v0,4(v1) / 80057354: sh v0,270(s0)   ; Record+4 -> +0x10E (der Deskriptor)
800573cc: sw zero,4(s0)                            ; Zustand 0 = INIT
80057464: sh zero,148(s0)                          ; +0x94 = 0
800576a4: sb zero,332(s0)                          ; +0x14C CLIP = 0  <<< Spawn-Seed = Idle-Clip 0
```

→ Im Original-RE2 rendert ein frisch gespawnter Zombie in JEDEM Freeze-Fenster **Clip 0 =
stehendes Idle**. Ein Clip-Index außerhalb der Bank existiert im Original nie.

**Die Lücke im Port:** für die Pfade `sel 0x0D → 0x101` und DEFAULT `0x1` sät `re2z_init`
KEINEN Clip; `motion` bleibt der RE1.5-Wert 0x27 (39) aus dem Spawn (scd_vm.c:3348) bzw. wird
von `re15_enemy_ai_live_init` (enemy_ai_common.c:1848) im INIT-Tick erneut auf 0x27 gesetzt.
Der Clip wird erst im FOLGETICK von EXEC[1] P0 geschrieben
(enemy_ai_re2_zombie.c:1194-1196, `re2z_clip(e, e->re2z_walkclip, 0, 0xF, 0x100, 1)`
= @0x80101A74-8C).

## 3. DER MECHANISMUS „LIEGEND“ — vollständige Kette

1. **Spawn** (Türdurchtritt 1050→1030, `re15_room_apply_pending` → Raum-SCD): 6 Zombies
   Typ 0x16, `state=0`, `motion=0x27` (=39, scd_vm.c:3348+3252, byte-true RE1.5-Seed
   @0x80100F30-34), `anim_frame=0` (Slot-memset, scd_room_setup.c:275-277).
2. **Türwechsel-Freeze**: `re15_room_transition_present` → State 4 →
   `g_re15_pauseflags |= 0xFF000000` (room_common.c:159, byte-true @0x8001CC5C/@0x8001CC6C).
   Damit stehen KI (`RE15_PAUSE_AI` Gate game_step_common.c:1707, Original-Zwilling
   @0x8010042C-3C) und Anim-Advance (game_step_common.c:1687, @0x80019E28-40) — **gerendert
   wird aber jeden Frame**, während die −0x1800-Blende in **6 Frames** aufzieht
   (re15_fade_config room_common.c:147, Rechnung room_common.c:114-117).
3. **Bank im RE2-Modus**: `pc_enemy_load_re2` ersetzt die Bank durch die RE2-EMD-Bank
   (main.c:523-552, `re2_ems_load_bank`); Haupt-Anim-Bank = Paar 2. **Selbst aus
   CDEMD0.EMS+RE2-EXE-TOC (@0x8009ADF4, Datei 0x8B5F4) gezählt: kind 0x16 → EMD @EMS 0x142000,
   Paar-Clips = [8, 31, 17] → Haupt-Bank 31 Clips** (identisch für 0x10/11/12/13/18).
   Gilt für BEIDE RE2-Stufen (Hybrid nutzt dieselbe RE2-Anim-Bank).
4. **Modulo-Fallback des Renderers** (PORT-ERFINDUNG, ohne @0x-Beleg):
   anim_select_common.c:75 `if (clip_idx >= anim->clip_count) clip_idx %= clip_count;`
   → **39 % 31 = 8**. Für state=0 posiert der RE2-Zombie die Action-Bank
   (`re15_re2z_poses_loco_bank` :6341-6358 kennt nur state 1/2 → 0;
   enemy_ai_common.c:4964-4968).
5. **RE2-Clip 8 = Boden-/Liege-Clip**: EXEC[7] P2 setzt genau `re2z_param_clips[4+back]` =
   Clips **8/9** als Boden-Clips (@0x80103840-80, enemy_ai_re2_zombie.c:2232-2237);
   FK-gemessen (Code-Kommentar :7205-7206): Clip 8/9 Frame 0 Brust **−150/−129** (= flach am
   Boden) vs. aufrechte Clips −2004…−2685. `anim_frame=0` + Anim-Freeze → es steht exakt die
   flach liegende Pose. **Alle 6 Zombies liegen.**
6. **Freigabe nach der Blende** (room_common.c:162-166): KI-Tick 1 = `re2z_init` →
   `0x101`; `motion` bleibt 39 (live_init schreibt es erneut, RE2-Zweig fasst es nicht an).
   KI-Tick 2 = EXEC[1] P0 → Walk-Clip 0/2 der Loco-Bank (startet aufrecht; Root-Kommentar
   :1271-1272) → **Ein-Frame-Sprung von flach-liegend auf stehend-gehend** = „mit einem Clip
   stehen sie dann plötzlich“.

Sichtbares Fenster: ~6 Fade-Frames (aus Schwarz aufziehend) + 2 KI-Ticks ≈ 0,25-0,3 s mit
6 flach liegenden Zombies, dann der Pop. (Beim Re-Entry positioniert sub00s IF-Zweig die
sechs zusätzlich in den Hof um — sie liegen also voll im Bild.)

## 4. WARUM ES KEIN RE-ENTRY-SNAPSHOT-PROBLEM IST

Der Port hat **keinen** Gegner-Snapshot: `scd_room_reenter` wischt Slots 1..N komplett
(`memset`, scd_room_setup.c:275-277) und der (Wieder-)Eintritt führt main00 neu aus — 1030
spawnt ungegated immer dieselben 20 Records (nur das Kill-Flag pc[7] via Zone 7/8 und der
Cap 6 filtern, scd_vm.c:3284-3319). `+0x10E`-Limpet-Bits können nicht überleben: die
re2z-Felder liegen im Aktor-Struct und werden vom memset + `re2z_init` (`e->re2z_f10e = 0`
:7057) genullt. Der D15.2-Wecker (:7234-7245) und die Kriech-Brücke D15.3 (:7282-7293)
feuern nur bei grid-Nibble 9/10 bzw. `+0x1C4`-Bits — in 1030 beim Betreten beides nicht aktiv.

**Das Symptom ist trotzdem generisch für Raumwechsel**, weil die Kette in §3 bei JEDEM
Betreten jedes Raums läuft. Zensus (eigener SCD-Walk über alle STAGE1-RDTs, Zombie-Typen):

| behavior | RE1.5-Spawn-Clip (scd_vm.c:3243-3253) | ×31-Bank gerendert | Räume |
|---|---|---|---|
| 0x0D | 0x27=39 | 39%31=**8 = LIEGEND** | **1030/1031 (20×), 1040/1041 (40×), 1200/1201 (4×)** |
| 0x06 | 0x27=39 | 8 = LIEGEND (bis INIT Clip 18 setzt) | 10E0/10E1 |
| 0x86 | 0x27=39 | 8 = LIEGEND (dann Feeding) | 1140 |
| 0x87/0x88 | 0x12/0x13=18/19 | 18/19 = RE2-**FRESS**-Clips (kniend statt liegend) | 1020/1070/1100/1110/11F0/1200… |
| 0x0E | 0x2A=42 | 42%31=11 = Knockdown-Clip | 10D0/10D1 |
| 0x00/0x02 | 0 | 0 = Idle stehend (korrekt) | 1010/1091/10B0/1180/11E0/1220/1250… |
| 0x81/0x83 | 0x0C=12 | 12 = in-Bank (Kriecher-Familie) | 1010/1011/1220/1221 |

→ 1050→1030 ist nur die auffälligste Instanz (alle sichtbaren Zombies betroffen, Clip 8 =
flach liegend). 1040 (Cap 5, 40 Records) müsste dasselbe zeigen; die 0x87/0x88-Räume zeigen
die mildere Variante (Liegende flackern beim Betreten kurz als Kniende).

## 5. AUFSTEH-POP / EXEC[7]-Kette (Task 3)

* Sub 7 P4 → `0x101` ist **byte-true**: `addiu v0,zero,257 / sw` @0x80103900-0C (Port
  :2250-2251). Es fehlt dort KEIN Aufsteh-Clip: P2 setzt Clip 8/9 (@0x80103838-8C), und
  **Clip 8 IST der Boden-Aufsteher** (67 Frames, steigt von Brust −150 auf stehend; Messung
  im Code-Kommentar :2156-2159 „f45 Clip 8 → f112 P4 → f113 0x101, glatter Verlauf“).
  Sub 8 (Feeding) hat separat den Aufsteh-Clip 0x15 in P3 (@0x80103CD8-EC) — beides im Port
  vorhanden.
* Der vom Nutzer gesehene Pop kommt NICHT aus EXEC[7] — die 1030-Zombies durchlaufen Sub 7
  nie (Zustands-Remap = 0x101). Der Pop ist der EXEC[1]-P0-Clip-Write (:1194-1196), der die
  nur-gerenderte Liege-Pose ersetzt. (Der historische EXEC[9]-0x901-Pop war ein anderer,
  2026-08-21 gefixter Fall, :7192-7216.)
* enemy_ai_common.c:2028-2033 („stehen nicht sauber auf“, Clip 0x29) ist der **RE1.5**-Feeding-
  Wake — der RE2-Flavor ersetzt den RE1.5-Zombie-Dispatch komplett (RE15_RE2_AI.md:341-343),
  dieser Pfad läuft im RE2-Modus nicht.

## 6. FIX-SKIZZE (kein Code von mir)

Die Original-Regel ist an ZWEI Stellen belegt: der RE2-**Spawn** sät `+0x14C = 0`
(`sb zero,332(s0)` @0x800576A4) und der RE2-**INIT** sät in jedem Pfad ein in-Bank-Clip-Wort
(Default 1 @0x801009B8/@0x801009D4; 22/23/18 in den Zweigen). Deshalb:

1. **Spawn-Seite (deckt das Freeze-Fenster):** im RE2-Flavor für `re15_re2z_owns_type(type)`
   den RE1.5-Clip-Seed nicht in `motion` stehen lassen — Mapping auf den RE2-Spawn-Seed
   **Clip 0** (`@0x800576A4`), z.B. als Flavor-Zweig an der `re15_enemy_spawn_action`-
   Anwendung (scd_vm.c:3348) oder als Motion-Override im re2z-INIT-Vorfeld. Damit rendert
   das Fade-Fenster stehendes Idle — exakt das Original-Spawn-Bild.
2. **INIT-Seite (deckt den INIT→EXEC-Tick):** in `re2z_init` die fehlenden Clip-Seeds der
   Pfade `0x101` (sel 0x0D) und DEFAULT `0x1` nachziehen — Clip-Wort 1 per
   `@0x801009B8/@0x801009D4` (bzw. den Walk-Clip, den EXEC[1] ohnehin im Folgetick lädt);
   analog dürfen die Liege-/Fress-Zweige ihre bereits zitierten Seeds (22/23 @0x801009D8-A08,
   18 @0x80100AD0) im INIT-Tick setzen statt erst in EXEC-P0.
3. **Nicht** am Modulo (anim_select_common.c:75) „reparieren“ — der ist port-erfunden und
   für andere Konsumenten tragend; das Original erreicht nie einen Out-of-Bank-Index. Ein
   zusätzlicher Flavor-Guard dort wäre Symptomdeckel, nicht Mechanismus.
4. `re15_enemy_ai_live_init`s RE1.5-Motion-Writes (enemy_ai_common.c:1846-1875) werden von
   `re2z_init` als „overridden below“ deklariert (:6961-6963) — für 0x0D/DEFAULT stimmt das
   heute nicht; der Fix aus (2) macht die Aussage wieder wahr.

## 7. REPRO/MESSUNG für den Hauptagenten (ohne dass ich baue)

1. **Statisch sichtbarer Kern** (kein Build nötig): motion=39 vs. clip_count=31 —
   scd_vm.c:3252/3348, anim_select_common.c:75, CDEMD0.EMS-Zählung (dieses Dossier §3.3).
2. **`probe_laned_1030_spawn` mit `RE15_AI_FLAVOR=re2`**: zeigt im Roster-Dump nach
   main00+8 VM-Ticks `st=0 mo=39` für alle Spawns und in den ersten AI-Ticks
   `st 0→1, s1 0x00→0x01` mit `mo=39` im INIT-Tick und `mo=0|2` erst einen Tick später.
   ⚠ Die Sonde lädt die **RE1.5**-EM016-Bank (43 Clips) — sie beweist die motion-Persistenz,
   aber NICHT den Modulo-Render; dafür Bank-Load wie in `test_re2_room1140_ab` auf die
   RE2-Bank umstellen oder (3) nutzen.
3. **Der eigentliche Beweis am Artefakt**: echte exe, `RE15_AI_FLAVOR=re2`,
   `RE15_ANIM_TRACE=<datei>` (anim_select_common.c:124-134), Tür 1050→1030 laufen
   (re15-parity-verify-Harness). Erwartung in der Trace: für die Zombie-Slots in den
   Eintritts-Frames `motion=39, clip_idx=8, fc=<Clip-8-Länge>`, ab dem 2. KI-Tick
   `motion=0|2` — und nach dem Fix durchgehend in-Bank-Clips (0 bzw. Walk).
4. **Visuell** (Nutzer-Sicht): re15-port-visual-verify (gdigrab) auf das Eintrittsfenster —
   vorher: 6 flach Liegende, Pop; nachher: stehende Idles, die anlaufen. Gegenprobe
   RE15-Flavor: dort rendert dasselbe Fenster RE1.5-Clip 39 der 43er-Bank (byte-true wie
   Original-Spawn-Seed @0x80100F30-34) — unverändert lassen.
5. **Regression-Räume** nach dem Fix: 1040 (40×0x0D), 1200 (0x0D+0x87/0x88 gemischt),
   1140 (0x86/0x88 — Fresser/Lieger müssen weiter fressen/liegen), 10D0 (0x0E Sitz-Import),
   1010 (0x81 Kriecher), 1030-Torkriechen (D15.3 unangetastet).

## Anhang: geprüfte, NICHT ursächliche Kandidaten

* RE2-Zustands-Remap-Default „liegend“? — Nein: DEFAULT `0x1` (:7141) == Original
  `sw 1,4(s2)` @0x801006B8. sel 0x0D → `0x101` stehend.
* Re-Entry-Snapshot restauriert falsche Zustände? — Es gibt keinen (memset
  scd_room_setup.c:275-277; main00 ungegated).
* EXEC[7]-Kette ohne Aufsteher? — Widerlegt, Clip 8 ist der Aufsteher (Messung :2156-2159);
  P4→0x101 byte-true @0x80103900-0C.
* enemy_ai_common.c:2032-Getup — RE1.5-Pfad, im RE2-Flavor nicht erreicht.
* sub00 `Member_set(4, …)` als Posen-Write — ist rot_y (actor_common.c:145).
