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

## Rezept für Phase 8.6 (Live-AI in einem echten Raum ticken)

Den Spawn-Probe erweitern, um die Live-Zombie-AI zu fahren + die Attack-Kette zu beobachten (statt manuellem Arming in den Unit-Tests):

```c
/* nach dem Spawn (oben): die Live-AI pro 0x10/0x11/0x16-Gegner ticken */
re15_enemy_ai_set_combat_active(1);                 /* DAT_800aca3c & 1 (sonst armt der Lunge nie) */
g_actors[0].x = ...; g_actors[0].z = ...;           /* Spieler in Reichweite des Zombies setzen */
for (int f = 0; f < 1200; f++) {
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        uint8_t t = g_actors[s].type;
        if (g_actors[s].active && (t==0x10||t==0x11||t==0x16))
            re15_enemy_ai_live_step(s);             /* tick -> brain -> arm -> windup -> lunge@300 */
    }
    /* g_actors[0].hp / der Gegner-State (+0x5, ai_attack_timer) pro Frame loggen */
}
/* erwartet: brain committet (+0x5=7) -> arm (ai_attack_timer 600..1110) -> @300 lunge -> HP fällt */
```
Das ist die port-seitige Verifikation des `game_step`-Wirings, BEVOR man es ans echte `game_step` hängt (1170-Risiko) und gegen ein DuckStation-Savestate (`re15-room-capture`) vergleicht.

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
