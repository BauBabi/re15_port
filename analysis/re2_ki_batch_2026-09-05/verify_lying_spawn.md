# SKEPTIKER-VERIFIKATION — PAKET S4 „Zombies spawnen liegend beim Raumwechsel"

Datum 2026-09-05. JEDE Adresse unten in DIESER Sitzung selbst disassembliert
(`re2_disasm.py` gegen info/re2leon/PSX.EXE bzw. --bin EMZ0.BIN; `re15_disasm.py` gegen
info/Re1.5/PSX.EXE bzw. --bin STAGE1.BIN) bzw. selbst aus den Datei-Bytes geparst
(CDEMD0.EMS, ROOM1030/1050.RDT). Nichts aus dem Diagnose-Dossier übernommen. Kein Build,
keine Repo-Datei geändert.

## (a) @0x800576A4 `sb zero,332(s0)` im Sce_em_set-Handler — **CONFIRMED**

Eigene Kette:
1. SCD-Opcode-Tabelle der RE2-EXE: Basis `@0x800A74C8` (Xref aus dem VM-Loop @0x80053F84/
   @0x80053FC8, ghidra_re2_Leon.txt:172949/172967). Eintrag `@0x800A75D8` = Index
   (0x800A75D8−0x800A74C8)/4 = **0x44** → `0x8005714C`. 0x44 = Sce_em_set.
2. Entity-Allokation im Handler: `lw v1,8516(s4)` / `addu s0,v1,zero` @0x80057228-30
   (Bump-Pointer, Schrittweite 584=0x248 @0x80057244); s0 bleibt bis zum Epilog die frische
   Entity (a0=s0 an die Calls @0x80057674/@0x80057694).
3. Die im Dossier zitierten Stores, alle selbst gelesen:
   `lhu v0,4(v1)` @0x8005734C / `sh v0,270(s0)` @0x80057354 (Record+4 → **+0x10E**);
   `sw zero,4(s0)` @0x800573CC (**Zustand 0 = INIT**); `sh zero,148(s0)` @0x80057464
   (+0x94 = 0); **`sb zero,332(s0)` @0x800576A4** (+0x14C Clip = 0). PC-Advance +22
   @0x800576E4 = 0x44-Recordlänge (Plausibilitäts-Gegenprobe).
4. Einziges +0x10E-Parsing IM SPAWN-Handler: `lhu 270(s0); andi 0x4000; beq` @0x800576C0-CC
   → `sb 146,448(s0)` @0x800576D4-D8. **Kein Clip-Parsing im Spawn** — alles Weitere macht
   der INIT (s. (b)).

## (b) EMZ0-INIT: `sw 1,4` Default-State + Clip-Wort 1 im Haupt-Pfad — **CONFIRMED**

1. `0x8010065C` IST der State-0-Handler: Routine-Tabelle `@0x8010C830` =
   {[0]=0x8010065C INIT, [1]=0x8010114C MAIN, [2]=0x80104F40, [3]=0x80108250} — selbst
   gefunden über Pointer-Suche (LE-Bytes von 0x8010065C @Datei 0xC830) + Tabellen-Dump.
2. `addiu v0,zero,1` @0x801006AC / **`sw v0,4(s2)` @0x801006B8** — direkt nach dem Prolog,
   unbedingt. ✔
3. Deskriptor-Dispatch: `lhu v1,270(s2); andi 0x3f` @0x801009C4-CC. **`addiu a1,zero,1`
   @0x801009B8 ist der Delay-Slot des Typ-30-Vergleichs (@0x801009B4 bne) und
   `sw a1,332(s2)` @0x801009D4 der Delay-Slot des ==2-Vergleichs (@0x801009D0 bne) — BEIDE
   laufen IMMER.** Clip-Wort-Default 1 steht also auf dem Haupt-Pfad, kein Sonderpfad.
4. Kein Umweg möglich: alle Verzweigungen von 0x8010065C bis 0x80100990 selbst verfolgt
   (0x801006D8→0x8010075C HP-Zufallszweige konvergieren @0x801007D8/F0; 0x80100800→0x818,
   0x801008BC→0x8D8, 0x801008E8/0x928→0x954 — nur lokale Vorwärts-Sprünge). Danach:
   ==2→Clip 22+0x701+21A|=4 (@0x801009D8-F0), ==4→23+0x701 (@0x80100A04-10),
   ==5→22+0x701+10E=0x4002+HP−1 (@0x80100A24-40), ==7→23+0x701+10E=0x4004+HP−1
   (@0x80100A74-94), ==6→0x101 ohne Clip-Write (@0x80100AB8-BC, Clip bleibt 1),
   ==8→18+0x801 (@0x80100AD0-DC), &1→23 Kriecher (@0x80100AF8-FC).
5. EMZ0.BIN und EMOVL10_S0.BIN sind **byte-identisch** (voller Vergleich, 0xCF4C Bytes).

## (c) 31 Clips / „Clip 8 = Liege-Clip" — **31 CONFIRMED; „Liege-Clip" REFUTED als Klasse, Frame-0-Wirkung CONFIRMED**

Eigene Kette (unabhängig vom Port-Parser, nur dessen Formatregeln nachimplementiert):
1. TOC selbst aus der RE2-EXE gelesen: `@0x8009ADF4` (Datei 0x8B5F4, Basis 0x80010000):
   kind 0x16 rec3 = Sektor 0x284 → **EMS-Offset 0x142000**, Größe 0x23A5C. ✔
2. EMD-Directory @0x23A3C, 8 Einträge. EDD-Clip-Zählung (Eintrag {u16 fc, u16 tbl},
   Clipzahl = tbl(clip0)/4): **Paar1(dir[1]) = 8, Paar2(dir[3]) = 31, Paar3(dir[5]) = 17**
   — Haupt-Bank = Paar 2 mit 31 Clips (Port-Regel „meiste Clips", re2_ems.c:118-133). ✔
3. Wurzelhöhen (py des Frame-0-Keyframes, Pool dir[4], kf_size 0x50) — eigene Messung:
   Clip 0: −1995 (stehend) · **Clip 8: f0 = −222, f75 = −1935 (80 Frames!)** · Clip 9:
   f0 −227 → −1974 · Clips 5/6/7/14: −175 konst. (Liege-Loops) · **22/23: −227/−222
   (10-Frame-Liege-Idles = die INIT-Seeds)** · 18/19: −947 (KNIEND = Fressen) ·
   11: −1905 (stehend, 5f) · 12: −1929 (stehend, 38f).
4. **Also: Clip 8 IST ein Boden-AUFSTEHER** (beginnt flach, endet stehend) — genau die vom
   Auftrag verdächtigte Klasse. Die Dossier-Kernaussage überlebt trotzdem, weil der
   Freeze `anim_frame` auf 0 pinnt (Slot-memset scd_room_setup.c:275-277 + Action-Freeze,
   s. (e)) → gerendert wird ausschließlich **Frame 0 = flach** (py −222). Detailfehler im
   Dossier: „Clip 8 (67 Frames)" — **falsch, Clip 8 hat 80 Frames**; 67 ist die Länge von
   Clip 18 (Fressen). Die 67 stammen aus einer Tick-Zählung f45→f112.

## (d) Renderer: motion=39 → Modulo → Clip 8 — **CONFIRMED**

1. anim_select_common.c:73-75: `clip_idx = (override>=0)?override:(int)a->motion;` … 
   **`if (clip_idx >= anim->clip_count) clip_idx %= clip_count;`** — der Modulo existiert
   wörtlich (Zeile 75) und ist als Port-Konstrukt unkommentiert-@0x-frei.
2. Bank-Wahl für den frischen Spawn (state=0): anim_select generischer Enemy-Zweig
   (:246-256) → `re15_enemy_find(type)->anim`; `clip_override = (anim_flags&0x04)?motion:-1`
   (:280) und Sce_em_set setzt `anim_flags=0` (scd_vm.c:3358) → override=−1 → clip_idx=39.
3. Kein früherer Clamp auf diesem Pfad: die main.c-Overrides sind alle weg-gegated —
   Loco-Bank nur über `re15_actor_uses_loco_bank` (enemy_ai_common.c:4932ff →
   `re15_re2z_poses_loco_bank`, enemy_ai_re2_zombie.c:6341-6379: **kennt nur state 1/2/3,
   default 0** → state 0 = Action-Bank), RBJ/Own/Victim nur state==4, re15-Pose nur
   `re2z_re15_pose` (erst INIT, sel 0x0E). Die `motion < clip_count`-Guards stehen NUR an
   den Override-Zweigen (main.c:7444/7472/7478/7514/7525), nicht am Default.
4. Bank-Inhalt: `pc_enemy_load_re2` (main.c:523-552) füllt die Registry-Bank aus
   CDEMD0.EMS (`re2_ems_load_bank` → Paar 2 = 31 Clips); der Hybrid (re2_hybrid_apply,
   re2_ems.c:323-375) tauscht NUR md1/Bindlängen, `eb->anim` bleibt die RE2-31er-Bank →
   gilt für beide RE2-Stufen. **39 % 31 = 8.** Skelett = RE2-Skel (Hybrid: RE2-Hierarchie
   mit RE1.5-Bindlängen + rootfix).

## (e) Türwechsel: Rendering läuft, KI+Anim stehen ~6 Frames — **CONFIRMED (mit Original-Timing-Korrektur, s. (g))**

1. Port: `re15_room_transition_present` (room_common.c:132-150): pauseflags=7, Fade
   -0x1800, state=4; `re15_room_transition_tick` (:154-169): state 4 → **`pauseflags |=
   0xFF000000`** (:159), Freigabe erst wenn `re15_fade_done`. Rechnung :114-117: Level
   0x7FFF, Schritt −0x1800 → 6 gezeichnete Frames, Freigabe im 7. Poll — Arithmetik selbst
   nachgerechnet (0x7FFF→0x67FF→…→0x07FF→Unterlauf Bit15). ✔
2. Gates selbst nachdisassembliert: Action-Driver-Gate `lw pauseflags; lui 0x1000; and;
   bne→Ende` **@0x80019E28-40** (RE1.5 EXE) = game_step_common.c:1687; AI-Gate `lui
   0x2000` **@0x8010042C-3C** (STAGE1.BIN Zombie-Root, + Per-Entity `+0x9&0x20`
   @0x80100450-5C) = :1707. Original-Freeze-OR: State-4-Handler **@0x8001CC54-6C**
   (`lui v0,0xff00; or; sw`). ✔
3. Rendering läuft weiter: transition_tick sitzt im Frame-Loop (main.c:3984) VOR
   game_step (:5580) und VOR dem Aktor-Render (:7415ff) — jeder der 6 Fade-Frames zeichnet
   die Aktoren mit eingefrorenem `motion`/`anim_frame`. ✔
4. **ABER (Dossier-Lücke, entscheidend für den Fix):** Im ORIGINAL bekommt der frische
   Spawn VOR dem Freeze einen vollen Subsystem-Durchlauf. Selbst disassembliert
   (RE1.5 EXE): State 1 löscht pauseflags @0x8001CA44, lädt synchron (jal 0x8001D600
   @0x8001CA54), springt in den State-3-Rumpf (`j 0x8001CBBC` @0x8001CA5C) → pauseflags=**7**
   @0x8001CBE4, state=4 @0x8001CC28, `j 0x8001CC98` — und @0x8001CC98ff. laufen IM SELBEN
   FRAME die Subsysteme: SCD-VM @0x8001CDEC (jal 0x8003F038 → Spawns), **Entity-Loop
   @0x8001CE04 (jal 0x8001A50C → die INITs!)**, Action-Driver @0x8001CE2C — alle Gates
   (0x2000_0000/0x1000_0000) sind bei pauseflags=7 FREI. Das 0xFF000000 kommt erst im
   NÄCHSTEN Frame (State-4-Handler). **Das Original rendert also die ganze Blende lang die
   INIT-Pose, nie den Spawn-Seed.** Der Port gibt den Spawns diesen Lade-Frame-Tick nicht
   (apply_pending/present laufen NACH game_step des Türframes, main.c:5580 vs :5908-5921)
   — die RE1.5-Lane kaschiert das mit dem Spawn-Seed (scd_vm.c:3348), die RE2-Lane nicht.

## (f) Remap 0x0D→0x101 + Walk-Clip erst im Folge-Tick — **CONFIRMED (Frame-Zahl präzisiert)**

1. Port: enemy_ai_re2_zombie.c **:7138-7139** `sel==0x0d → set_state_word(0x101)`,
   **:7140-7141** DEFAULT `0x1`; `re15_ai_set_state_word` (enemy_ai_common.c:62-68):
   0x101 → state=1, sub1=1, **sub2=0**. Der 0x701-Zweig (:7136-7137) und der 0x101/0x1-Zweig
   säen KEINEN Clip; nur sel==6 sät Clip 18 (:7078). `re15_enemy_ai_live_init` (im selben
   INIT-Tick, :6964) schreibt motion für 0x0D erneut auf 0x27 (enemy_ai_common.c:1847-1848).
2. Tick-Struktur: `re15_re2z_tick` switch — `case 0: re2z_init; break;` (:7295-7296) →
   INIT-Tick endet ohne EXEC. Nächster Tick: re2z_active → sub1==1 → `re2z_exec_walk`,
   **P0 (`sub_state_2==0`) @:1194-1195 `re2z_clip(e, re2z_walkclip …)`** — re2z_clip
   schreibt e->motion (:857-859). Walk-Clips {0,2} (re2z_param_walk :665) = Loco-Paar-1,
   startet aufrecht.
3. Original identisch: MAIN-Root 0x8010114C dispatcht `+0x10E&0x3F` (Tabelle @0x8010C854,
   gerade→0x8010118C) → `lbu 5(s0)` (routine_1) in Tabellen @0x8010C88C/@0x8010C8CC;
   Walk-Zweitpass 0x80101A40: `lbu v1,6(s1)` (P-Dispatch), **P0 @0x80101A74-8C:
   `routine_2=1; clip = (+0x218)+0xF0000 → sw 332(s1)`** — Walk-Clip erst im Tick NACH dem
   INIT. ✔
4. Präzisierung der Behauptung „motion=39 zwei Render-Frames nach Freeze-Ende sichtbar":
   Freigabe und INIT-Tick liegen im SELBEN Frame (transition_tick :3984 vor game_step
   :5580) → **genau EIN Frame nach Freeze-Ende rendert noch 39→Clip 8** (der INIT-Frame);
   der Folgeframe rendert den Walk-Clip. Gesamtfenster: ~6 Fade-Frames (die ersten davon
   nahe schwarz) + 1 INIT-Frame liegend, dann der Ein-Frame-Pop. „Zwei KI-Ticks" stimmt,
   „zwei Render-Frames nach Freeze-Ende" nicht.

## Raumdaten-Gegenprobe (Dossier §1) — **CONFIRMED**

ROOM1030.RDT selbst geparst: mainScd @0x1C5C, `24 12 06 00` (Save 0x12,6) @Datei **0x1DE2**,
direkt gefolgt von 20 Records `44 xx 16 0d` ab 0x1DE6 (Stride 0x14). ROOM1050: **null**
Zombie-Sce_em_set. Spawn-Kette im Port: scd_vm.c:3252 (0x0D→0x27), :3348
(`a->motion = re15_enemy_spawn_action(...)`), :3358 (`anim_flags=0`), Cap-Gate :3314-3319.
Slot-memset + kein Snapshot: scd_room_setup.c:275-277. ✔

## (g) FIX-RISIKO — der geplante Fix ist in Teil (1) NICHT original-treu

**Der Maßstab:** Das Original (beide Engines derselben Familie; für RE1.5 oben
byte-bewiesen) rendert im Blende-Fenster die **INIT-Pose**, weil der INIT im Lade-Frame
läuft. Der RE2-Spawn-Seed Clip 0 (@0x800576A4) wird NIE sichtbar (Sce_em_set läuft in der
Skript-Phase, der INIT in der Entity-Phase DESSELBEN Frames; erster gezeichneter Frame =
Fade-Level 0x7FFF ≈ schwarz). „Vorbild @0x800576A4" fürs Freeze-Fenster ist deshalb das
falsche Original-Zitat — das richtige sind die INIT-Seeds @0x801009B8/D4 (1),
@0x801009D8ff. (22/23), @0x80100AD0 (18), @0x80100AF8 (23 Kriecher).

**Was Spawn-Seed „Clip 0 für alle re15_re2z_owns_type" sichtbar ANRICHTEN würde**
(Bank-Messwerte aus (c); heutiger Zustand aus scd_vm.c:3243-3253 + Modulo):

| behavior | heute im Freeze (RE2-Bank) | nach Fix „Clip 0" | Original-Bild (INIT-Clip) |
|---|---|---|---|
| 0x0D / 0x00 / 0x02 | 39→8-f0 flach (FALSCH) bzw. 0 | 0 stehend ✔ | 1 stehend (dann Walk/Stand) |
| 0x86 (Fresser 1140/10E0) | 39→8-f0 flach (≈ok-ish) | **stehend = REGRESSION** | 18 kniend-fressend |
| 0x87/0x88 (Lieger 1020/1070/1100/1140…) | 18/19 kniend (falsche Klasse) | **stehend = REGRESSION** | 22/23 flach liegend |
| 0x81/0x83 (Kriecher 1010/1220) | 12 stehend (FALSCH) | stehend (gleich falsch) | 23 flach |
| 0x0E (10D0 Sitzer) | 42→11 stehend (FALSCH) | stehend | Port-Soll: RE1.5-Sitz 0x2A (re2z_re15_pose) |

**Präzisierter Fix:**
1. **Spawn-seitig (RE2-Flavor)** nicht Clip 0, sondern den Clip säen, den `re2z_init` für
   dieses behavior committen wird — die per-behavior-Tabelle der INIT-Seeds:
   sel 6→**18** (@0x80100AD0-DC); sel 8/0x0B/lying_family&0x80→**23** bzw. 22 für das
   +0x21A&4-Pendant (@0x801009D8-F0/@0x80100A04-28/@0x80100A74-7C); sel 1/3&0x80
   (Kriecher)→**23** (@0x80100AF8-FC); sel 0x0E→RE1.5-Sitz-Route (motion 0x2A +
   re2z_re15_pose bereits am Spawn armieren, Port-Option wie :7110-7128); sel 0x0D +
   DEFAULT→**1** (@0x801009B8/@0x801009D4; Clip 0 wäre visuell gleichwertig, aber 1 ist
   das INIT-Byte). Damit zeigt JEDER Spawn-Weg im Freeze-Fenster das Original-Bild.
2. **INIT-seitig** (Dossier-Fix 2) ist korrekt und byte-belegt: Clip-Wort 1 für die Pfade
   0x101/0x1, 22/23 im 0x701-Zweig, 23 im Kriecher-Zweig (18 hat der Port schon, :7078).
   Deckt den INIT-Frame; die Dossier-Variante „Motion-Override im re2z-INIT-Vorfeld"
   alleine deckt das Freeze-Fenster NICHT (INIT läuft erst nach der Blende).
3. **Alternative (mechanismus-treu, aber invasiver):** dem frischen Raum wie im Original
   EINEN Subsystem-Durchlauf im Lade-Frame unter pauseflags=7 geben (@0x8001CC98→
   @0x8001CE04-Reihenfolge). Löst beide Flavors ohne Seed-Tabellen, verschiebt aber die
   gesamte Frame-Buchführung (Testpins „st=0 mo=39 während der Blende",
   re15_re2_hp_sync-Stempel, D15-Wecker-Gates) um einen Tick — Regressionfläche deutlich
   größer als (1)+(2).
4. **Gate auf die geladene Bank, nicht nur den Flavor:** fehlt shared_assets/RE2/CDEMD0.EMS,
   fällt pc_enemy_load_re2 auf RE1.5-Modelle+Bank (43 Clips) zurück (main.c:528-541) —
   RE2-Clipnummern 22/23/18/1 wären dort FALSCHE Posen. Heutiger RE1.5-Seed passt zum
   Fallback; der neue Seed darf nur greifen, wenn die RE2-Bank wirklich steht.
5. **Nicht anfassen:** RE1.5-Flavor-Seed 0x27 (der ist per (e).4 sogar ergebnis-korrekt:
   Original-RE1.5 zeigt in der Blende die INIT-Pose 0x27), Modulo-Zeile
   anim_select_common.c:75 (tragend für andere Konsumenten; das Original erreicht nie
   out-of-bank — Dossier-Punkt 3 bestätigt).
6. **Regressions-Kandidaten für die Abnahme:** 1140 (0x86/0x88 — Fresser/Lieger müssen im
   ERSTEN sichtbaren Frame knien/liegen, nicht stehen), 1100/1020/1070 (0x87/0x88 +
   D15.2-Wecker), 1010/1220 (0x81 Kriecher — liegt nach Fix statt zu stehen = sichtbare
   VERBESSERUNG, im Diff-Bild einplanen), 10D0 (0x0E Sitzer), 1030/1040 (0x0D-Massen),
   1030-Torkriechen (D15.3), plus Headless-Pins, die heute `mo=39` im Fade messen.

## Urteile kompakt

| # | Behauptung | Urteil |
|---|---|---|
| a | sb zero,332(s0) @0x800576A4 im Sce_em_set 0x8005714C, trifft +0x14C der frischen Entity | CONFIRMED |
| b | INIT-Default `sw 1,4` @0x801006AC-B8; Clip-Wort 1 @0x801009B8/@0x801009D4 unbedingt im INIT | CONFIRMED |
| c | RE2-Bank 31 Clips | CONFIRMED |
| c′ | „Clip 8 = Boden-/Liege-Clip (kein Aufsteher)" | REFUTED als Klasse (Aufsteher, 80f, f0 flach −222 → f75 −1935); Frame-0-Renderwirkung „liegend" bleibt richtig |
| d | Modulo anim_select_common.c:75 einziger Clamp; RE2-Action-Bank+Skel für state-0-Zombies | CONFIRMED |
| e | Rendering läuft, KI+Anim ~6 Frames eingefroren | CONFIRMED — aber Original friert erst NACH dem Lade-Frame-INIT (Dossier-Lücke) |
| f | Remap :7138-7141; Walk-Clip erst Folge-Tick (P0) | CONFIRMED; „zwei Render-Frames nach Freeze-Ende" → tatsächlich EIN Frame (INIT-Frame), Pop im zweiten |
| g | Fix-Teil 1 (Clip 0 für alle) | REFUTED als byte-true — Original-Freeze-Bild = INIT-Clip je behavior; Seed-Tabelle s.o. |
