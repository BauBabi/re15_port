/*
 * test_support.c — Host-/Plattform-Symbole für Test-Executables.
 *
 * Die Engine-Lib (libre15_engine.a) referenziert eine Handvoll Symbole, die
 * normalerweise das PLATTFORM-Backend liefert (PC: platform/pc/src, PSX:
 * platform/psx/src). Test-Executables linken aber nur re15_engine — daher
 * fehlten beim Link re15_sin_q12/cos_q12 (Trig), g_engine, re15_debug_text und
 * re15_audio_start_room_bgm. Diese Datei (+ das echte skeleton_trig_pc.c, das
 * über re15_test_support mitgelinkt wird) schließt die Lücke, OHNE das schwere
 * SDL/Audio-PC-Backend hereinzuziehen.
 *
 * - g_engine: nullinitialisierte Engine-Zustands-Instanz. Tests prüfen den
 *   VM-/Parser-Zustand, nicht die Frame-Schleife; 0-Init genügt.
 * - re15_debug_text / re15_audio_start_room_bgm: No-op (Tests brauchen weder
 *   On-screen-Debug-Text noch echtes BGM).
 * - re15_sin_q12 / re15_cos_q12: NICHT hier — das echte PC-Backend
 *   skeleton_trig_pc.c wird in re15_test_support mitkompiliert, damit die
 *   Mathematik-Tests die TATSÄCHLICHE Trig-Implementierung prüfen.
 */
#include "re15_engine.h"
#include "re15_audio.h"

re15_engine_state_t g_engine;

void re15_debug_text(int x, int y, int z, const char *text)
{
    (void)x; (void)y; (void)z; (void)text;
}

void re15_audio_start_room_bgm(int stage, int room)
{
    (void)stage; (void)room;
}

/* Audio stubs for engine unit tests (the real impls live in the PC/PSX backends,
 * not linked into test binaries). enemy_ai_common.c's death FSM calls re15_audio_room_se. */
void re15_audio_room_se(int se_id) { (void)se_id; }
void re15_audio_weapon_se(int se_id) { (void)se_id; }
