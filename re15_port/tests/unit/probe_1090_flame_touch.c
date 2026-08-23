/* probe_1090_flame_touch.c — ROOM1090 Flammen-Kontakt (Nutzer-Report 2026-08-23:
 * "die Flammen schadensanimation und das schmerzgeraeusch fehlt").
 *
 * MISST auf dem ECHTEN Bild-Ablauf (scd_vm_tick -> msg -> cam -> re15_game_step, Muster
 * probe_1090_trigger_walk.c), ob der byte-true Feuer-Kontakt-Pfad feuert:
 *   - Kontakt-Test im Root-Tail von FUN_80116288: `jal 0x8002aec4` @0x80116368, Hitbox
 *     600/720 @0x80121258, tangibel ab Phase >= 13 (@0x80116328-2c)
 *   - HP -2 pro Ueberlapp-Frame (Gate `slti v0,v0,4` @0x801163c0-c4, Store @0x801163c8-d0
 *     — Feuer kann nicht toeten)
 *   - Stagger cmd 2 (@0x8011638c-a4) -> Flinch-Clip 8/9 + CORE-SE 1/2 (@0x80035e80 /
 *     @0x80036004; Port: game_step_common.c Flinch-Detector)
 *
 * Der Spieler wird 900 Einheiten vor die naechste Flamme gesetzt und laeuft mit dem
 * Tank-Pad hinein (echte Kollision, echter cmd-Pfad).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_fade.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern scd_vm_t         g_scd;
extern re15_aot_state_t g_aot;
extern uint32_t         g_re15_pauseflags;
extern int g_test_core_se_last, g_test_core_se_count;   /* test_support.c re15_audio_core_se */

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fail  = 0;

#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); s_fail++; } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int s_cine_was_active = 0;
static void frame(uint16_t held, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = held;
    s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

int main(void)
{
    size_t rsz = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &rsz);
    if (!raw) { printf("FAIL: ROOM1090.RDT fehlt\n"); return 1; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_pauseflags_clear();
    g_current_room_id = 0x1090; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;   /* Tuerspawn (1050-Tuer) */
    re15_collision_set_band(re15_collision_band_from_y(pl->y));
    re15_inv_load_briefing();
    s_shown = 0;
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, pl->x, pl->z, /*entry cut*/ 3);
    g_scd.cut_auto_enabled = 1;

    /* Feuer-FSM hochfahren (tangibel erst ab Phase >= 13, @0x80116328-2c). */
    for (int f = 0; f < 90; f++) frame(0, 0);

    int fire = -1; long long bd = 0x7fffffffffffLL;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type != 0x26) continue;
        long long dx = e->x - pl->x, dz = e->z - pl->z, d2 = dx * dx + dz * dz;
        if (d2 < bd) { bd = d2; fire = s; }
    }
    CHECK(fire >= 0, "kein 0x26-Feuer-Aktor gespawnt");
    if (fire < 0) { printf("probe_1090_flame_touch: %d FAILURES\n", s_fail); return 1; }
    re15_actor_t *fe = &g_actors[fire];
    printf("Feuer slot %d @(%d,%d,%d) sub=%d/%d, Spieler hp=%d\n",
           fire, fe->x, fe->y, fe->z, fe->sub_state_1, fe->sub_state_2, pl->hp);

    /* 900 Einheiten vor die Flamme setzen, Blick auf die Flamme, dann reinlaufen. */
    {
        double ang = atan2((double)-(fe->z - pl->z), (double)(fe->x - pl->x))
                   * 4096.0 / 6.283185307179586;
        int yaw = ((int)(ang + 0.5)) & 0xfff;
        pl->x = fe->x - (int32_t)((cos(ang * 6.283185307179586 / 4096.0)) * 900.0);
        pl->z = fe->z + (int32_t)((sin(ang * 6.283185307179586 / 4096.0)) * 900.0);
        pl->rot_y = (int16_t)yaw;
    }
    int hp0 = pl->hp, first_dmg = -1, first_flinch = -1, se0 = g_test_core_se_count;
    for (int f = 0; f < 300; f++) {
        frame(RE15_PAD_BIT_UP, 0);
        if (first_dmg < 0 && pl->hp < hp0) first_dmg = f;
        if (first_flinch < 0 && (pl->motion == 8 || pl->motion == 9)) first_flinch = f;
        if (first_dmg >= 0 && first_flinch >= 0 && g_test_core_se_count > se0) break;
    }
    printf("hp %d -> %d (erster Schaden f%d), Flinch-Clip f%d (mo=%d), CORE-SE +%d (last=%d)\n",
           hp0, pl->hp, first_dmg, first_flinch, (int)pl->motion,
           g_test_core_se_count - se0, g_test_core_se_last);
    CHECK(first_dmg >= 0, "Feuer-Kontakt muss HP senken (-2/Frame @0x801163c8-d0)");
    CHECK(pl->hp >= 4 - 2, "Feuer darf nicht toeten (slti 4 @0x801163c0-c4), hp=%d", pl->hp);
    CHECK(first_flinch >= 0, "Stagger cmd 2 muss Flinch-Clip 8/9 spielen (@0x80035e38/@0x80035fbc)");
    CHECK(g_test_core_se_count > se0, "Schmerz-SE CORE 1/2 muss feuern (@0x80035e80/@0x80036004)");
    if (g_test_core_se_count > se0)
        CHECK(g_test_core_se_last == 1 || g_test_core_se_last == 2,
              "CORE-SE muss 1/2 sein, war %d", g_test_core_se_last);

    if (s_fail == 0) printf("probe_1090_flame_touch: OK\n");
    else             printf("probe_1090_flame_touch: %d FAILURES\n", s_fail);
    return s_fail ? 1 : 0;
}
