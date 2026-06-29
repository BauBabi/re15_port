---
name: re15-room-probe
description: Port-seitige DYNAMISCHE Verifikation OHNE DuckStation — lädt eine echte ROOM####.RDT in die C-Engine, fährt deren SCD (main00+sub00) hoch, tickt N Frames (optional die Live-AI pro Gegner) und inspiziert g_actors + Globals. Verwenden, um zu prüfen, was der Port beim Laden/Ticken eines Raums TUT (Spawns, AI-State-Transitionen, HP-Fall, Kollision, Kamera) — als schneller, deterministischer ctest, statt das echte Spiel zu starten. Die Brücke zwischen reiner Unit-RE und der schweren re15-room-capture-Savestate-Pipeline.
---

# RE1.5 Port-Runtime-Probe (Raum laden + SCD/AI ticken + State lesen, kein DuckStation)

Für byte-true RE gibt es zwei dynamische Wege: das echte Spiel (`re15-room-capture` → DuckStation-Savestate, schwer) und — viel leichter — **den Port selbst** ein echtes RDT laden + ticken lassen und den resultierenden Engine-State auslesen. Letzteres ist deterministisch, sekundenschnell, CI-fest und braucht keine Maschine mit DuckStation. Genau richtig, um zu verifizieren, ob eine portierte Mechanik beim Laden/Ticken eines konkreten Raums das Richtige tut.

## ⚠️ Gotchas (hart erkämpft 2026-06-29)

- **Die PC-Exe ignoriert `argv`** (`platform/pc/main.c`: `(void)argc; (void)argv;`). `re15_pc.exe --headless` tut NICHTS — Steuerung läuft über env-Vars (`RE15_START_ROOM=1140`), und es gibt keinen sauberen „lade Raum + dump State"-CLI-Pfad. → Nicht mit der Exe kämpfen; einen **ctest-Probe** schreiben (unten).
- **`re15_pc.exe` sperrt die eigene Datei**, solange sie läuft (auch ein hängender Hintergrund-Lauf). Ein folgender Build scheitert dann mit `cannot open output file platform\pc\re15_pc.exe: Permission denied` (KEIN Code-Fehler!). → **Vor jedem Build, der nach einem Exe-Lauf kommt: `taskkill //F //IM re15_pc.exe 2>/dev/null; true`.**
- `tools/` ist standardmäßig AUS (`RE15_BUILD_TOOLS=OFF`, alte API) — KEIN neues Tool dort ablegen, es verrottet. Die **Test-Infra** (`tests/unit/`, GLOB-gebaut, ctest) ist der robuste Ort für Probes.
- **Test-State-Kontamination zwischen Probe-Teilen** (Phase 8.9): wenn ein Probe-ctest mehrere Szenarien NACHEINANDER auf demselben `g_actors[]` fährt, schleppt jeder Teil den State des vorigen mit. Konkret biss diese Gotcha: Part-5 ließ einen Zombie mitten im Grab (+0x5=3) zurück → Part-6 (das den −15-HP-Grab eines ANDEREN Zombies prüfte) bekam doppelten Damage, weil der Part-5-Zombie weiter-grabte. → Pro Szenario **den Test-Gegner ISOLIEREN**: alle anderen Gegner weit weg parken + benign setzen, z.B. `for (i) { z=&g_actors[zslots[i]]; z->grid_id=0x86; z->sub_state_1=0; z->sub_state_2=0; z->ai_flags=0; z->x=z->z=30000; }` (feeding-Sub-Mode + dist≫4000 → kein Wake/Grab), und Player-HP/Pos frisch setzen. Sonst misst du Cross-Talk, nicht den getesteten Pfad.

## Muster: ein Raum-Probe-ctest (Vorlage)

Die laufende, getestete Vorlage ist **`re15_port/tests/unit/test_room1140_spawn.c`** + ihr CMake-Eintrag in `tests/unit/CMakeLists.txt` (mit Asset-Pfad-Injektion). Kopieren + anpassen. Kern-Sequenz (= `scd_room_setup.c` `scd_room_reenter` minimal):

```c
/* Asset-Pfad CMake-injiziert: target_compile_definitions(... PRIVATE
 *   RE15_ASSET_PSX_DIR="${CMAKE_SOURCE_DIR}/shared_assets/PSX") */
uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT", &sz);   /* eigenes fopen */
re15_rdt_t rdt; re15_rdt_parse(buf, sz, &rdt);

re15_actor_init();                 /* slot 0 = player */
re15_aot_init();
scd_vm_init();                     /* nullt Flags (re15_game_state_init) -> Ck-Gates default clear */
extern void scd_register_current_rdt(const re15_rdt_t *);
scd_register_current_rdt(&rdt);    /* nötig, falls sub00 via Evt_exec auf sub_scd[] springt */
if (rdt.main_scd)   scd_thread_start(0, rdt.main_scd);    /* main00 (init) */
if (rdt.sub_scd[0]) scd_thread_start(1, rdt.sub_scd[0]);  /* sub00 (Raum-Skript, spawnt Sce_em_set) */
g_scd.work_vars[10] = 0;           /* entry scenario 0 (sub00-Dispatch) */
for (int i = 0; i < 120; i++) scd_vm_tick();

for (int s = 1; s < RE15_ACTOR_MAX; s++)   /* g_actors[1..] inspizieren */
    if (g_actors[s].active) printf("type=0x%02X pos=(%d,%d,%d) state=0x%02X grid=0x%02X motion=%d\n", ...);
```

So wurde 2026-06-29 byte-true verifiziert, dass ROOM1140s sub00 die 5 Briefing-Zombies (0x16/0x10/0x10/0x11/0x11) via `Sce_em_set` spawnt — die alte „kommt aus der Overlay-Entity-Liste"-Annahme widerlegt.

## Rezept: die Live-Zombie-Combat-Kette in einem echten Raum (IMPLEMENTIERT, Phase 8.6–8.9)

Die laufende Combat-Probe ist **`re15_port/tests/unit/test_room1140_combat.c`** (7 Teile, treibt `re15_enemy_ai_run_all` gegen die echte ROOM1140). Die byte-true In-Game-Kette ist: **spawn → tick (INIT→ACTIVE) → feeding-Zombie WACHT auf (dist<4000) → engage → TURN-to-face → GRAB (−10/−5 HP)**. (Der Lunge-Arm ist DORMANT — `re15-savestate-ghidra` §7 bewies `DAT_800aca3c & 1` wird nie gesetzt; der echte Angriff ist der Grab, nicht der Lunge.)

```c
/* nach dem Spawn (oben): die Live-AI pro 0x10/0x11/0x16-Gegner ticken — re15_enemy_ai_run_all IST
 * die typ-gegatete Slice (game_step ruft sie); sie tickt nur die Live-Zombie-Typen. */
re15_actor_t *pl = &g_actors[0];
pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0;
/* feeding-Zombie nah an den Spieler -> er wacht auf (dist<0xfa0=4000) + dreht sich + grabt */
g_actors[wake_slot].x = 1000; g_actors[wake_slot].z = 0;
for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(/*combat_active=*/0);  /* 0: combat-active ist dormant! */
/* erwartet: grid 0x86->0 (wach), state->1, +0x5 läuft 2(engage)->7(turn)->3/4(grab); player HP fällt */
```

Wichtig (Stand 8.9): `combat_active` (= `DAT_800aca3c & 1`) ist **dormant** — der Grab braucht es NICHT (nur der dormante Lunge-Arm liest es). Den Grab-State direkt setzen, um die Execution zu prüfen: `gz->state=1; gz->grid_id=0; gz->sub_state_1=3; gz->sub_state_2=0;` (= committed grab) → run_all → −10 (Impact) dann −5 (Bite) → Exit +0x5=2. Den Turn prüfen: `gz->sub_state_1=7;` + Zombie abgewandt nah → run_all → `rot_y` slewt zum Spieler → Grab-Commit. (DEFERRED, noch nicht beobachtbar: der Forward-Walk +0x5=5/6 = Anim-Root-Motion, FUN_8001ad68 — der Zombie läuft noch nicht über Distanz; im Briefing-Room läuft der Spieler zum Zombie, daher reicht Turn+Grab.) Finaler Vergleich gegen ein mid-Combat DuckStation-Savestate (`re15-room-capture`).

## Build/Run

```bash
taskkill //F //IM re15_pc.exe 2>/dev/null; true        # falls die Exe noch läuft (Lock!)
export PATH="/c/msys64/mingw64/bin:$PATH"
cmake --build re15_port/build                          # Ninja erfasst neue Test-Dateien via GLOB + re-config
./re15_port/build/tests/unit/test_<name>.exe           # direkt, sieht die printf-Diagnose
ctest --test-dir re15_port/build --timeout 30          # alle (CI-Sicht)
```

## Verwandte Skills

- `re15-room-capture` / `re15-savestate-ghidra` — die SCHWERE Dynamik (echtes Spiel via DuckStation); für den FINALEN Port-vs-Original-Vergleich + Werte, die der Port noch nicht produziert (z.B. 0x10/0x11-Hitbox-Dims).
- `re15-psx-disasm` — die statische Quelle der byte-true Konstanten, die der Probe dann dynamisch bestätigt.
- `build-workflow` — CMake/ctest-Mechanik + Build-Fehler-Diagnose.
- Memory `reai-v2-foundation-combat` — die Zombie-AI-RE, die diese Probe-Methode erzwang.
