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

void re15_audio_load_room_banks(void)
{
}

/* Audio stubs for engine unit tests (the real impls live in the PC/PSX backends,
 * not linked into test binaries). enemy_ai_common.c's death FSM calls re15_audio_room_se. */
/* SNDL1-SE-SPION (nur Test-Harnisch): protokolliert jeden FUN_800453d0-Aufruf mit dem
 * vom Test gesetzten Tick-Stempel, damit SE-Kadenz/Frames byte-genau gepinnt werden koennen. */
int g_test_room_se_log[2048];
int g_test_room_se_tickof[2048];
int g_test_room_se_n    = 0;
int g_test_room_se_tick = 0;
void re15_audio_room_se(int se_id)
{
    if (g_test_room_se_n < 2048) {
        g_test_room_se_tickof[g_test_room_se_n] = g_test_room_se_tick;
        g_test_room_se_log[g_test_room_se_n++]  = se_id;
    }
}
void re15_audio_weapon_se(int se_id) { (void)se_id; }
/* FILE wave: spy on the CORE00-bank SE calls (FUN_80045024 bank 4) so the FSM tests
 * can assert the byte-cited call sites (SE(4,4)/(4,5)/(4,6)/(4,8)). */
int g_test_core_se_last = -1;
int g_test_core_se_count = 0;
void re15_audio_core_se(int se_id) { g_test_core_se_last = se_id; g_test_core_se_count++; }
void re15_audio_prime_weapon(int weapon_id) { (void)weapon_id; }
/* game_step_common.c haengt seit der Knockdown-Klasse an den Test-Links (enemy_ai_common
 * ruft re15_player_knockdown_begin) — die zwei restlichen Plattform-Audio-Symbole stubben. */
void re15_audio_footstep(int foot, int sound_type) { (void)foot; (void)sound_type; }
void re15_audio_rotor_update(const int32_t cam_eye[3], const int32_t cam_tgt[3],
                             const int32_t heli_pos[3]) { (void)cam_eye; (void)cam_tgt; (void)heli_pos; }

/* Real asset reader for tests (the engine's item_icon_common.c reads ITEMALL.PIX through this; the PC
 * backend provides the game one). Resolves a "shared_assets/PSX/..." path via RE15_ASSET_PSX_DIR so the
 * icon test finds the in-repo asset. Returns a malloc'd buffer (+ size) or NULL. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[512];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0)
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else
        snprintf(buf, sizeof buf, "%s", path);
    FILE *f = fopen(buf, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (int)sz;
    return b;
}
