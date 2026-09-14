/* probe_re2_kfjump.c — MESSSONDE (kein Fix): was passiert im PORT an einem Tick, an dem der
 * RE2-Zombie einen Keyframe UEBERSPRINGT (FUN_8002A9C8 @0x80101D54 / @0x80102460)?
 *
 * ORIGINAL (info/re2leon/PSX.EXE, selbst disassembliert):
 *   FUN_80015E7C @0x80015FC4-FE4 :  +0x144 = root(kf_jetzt) - *(s16*)(self+0x20C)
 *                                   *(self+0x20C) = root(kf_jetzt)
 *   Der Bezug ist der ZULETZT GESPEICHERTE Wurzelstand. Ein uebersprungener Keyframe macht das
 *   Delta im naechsten e7c also DOPPELT so gross (Summe zweier Keyframe-Differenzen).
 * PORT: re15_clip_root_motion_delta (enemy_ai_common.c) verlangt fr_now == fr_prev + 1 und
 *   kehrt sonst OHNE Bewegung zurueck (`re2z_root144 = 0`).
 *
 * Die Sonde zaehlt fuer jeden lebenden RE2-Zombie in ROOM1030 die Ticks mit
 *   Bildsprung +1 / +2 / Wrap  und  ob sich die Position in diesem Tick geaendert hat.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define MAXF 2000

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint8_t *s_re2 = NULL; static size_t s_re2n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2) s_re2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2n);
    if (!s_re2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2, s_re2n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

typedef struct { uint16_t fr; uint8_t mo; int32_t x, z; int16_t r144; uint8_t s1; } rec_t;
static rec_t s_rec[RE15_ACTOR_MAX][MAXF];
static int   s_live[RE15_ACTOR_MAX];

int main(void)
{
    size_t n = 0; uint8_t *rdtbuf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!rdtbuf || re15_rdt_parse(rdtbuf, n, &s_rdt) != 0) { printf("ROOM1030 fehlt\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->state = 0; pl->motion = 0;
    pl->floor = 0; pl->y = 0; pl->x = -6000; pl->z = -21000;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);

    memset(s_live, 0, sizeof s_live);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) s_live[s] = 1;

    for (int f = 0; f < MAXF; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        scd_vm_tick();
        re15_game_step(&s_ctx);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            const re15_actor_t *e = &g_actors[s];
            s_rec[s][f].fr = e->anim_frame; s_rec[s][f].mo = (uint8_t)e->motion;
            s_rec[s][f].x = e->x; s_rec[s][f].z = e->z;
            s_rec[s][f].r144 = e->re2z_root144; s_rec[s][f].s1 = e->sub_state_1;
        }
    }

    printf("ROOM1030 / RE2-Geschmack / %d Ticks\n", MAXF);
    printf("slot typ  | Bildsprung +1 (bewegt/still) | Bildsprung +2 (bewegt/still) | sonstige\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        int p1m=0,p1s=0,p2m=0,p2s=0,oth=0;
        for (int f = 1; f < MAXF; f++) {
            rec_t *a = &s_rec[s][f-1], *b = &s_rec[s][f];
            if (a->mo != b->mo) { oth++; continue; }
            int d = (int)b->fr - (int)a->fr;
            int moved = (a->x != b->x) || (a->z != b->z);
            if (d == 1)      { if (moved) p1m++; else p1s++; }
            else if (d == 2) { if (moved) p2m++; else p2s++; }
            else             oth++;
        }
        printf(" z%-2d 0x%02x |  %6d / %6d              |  %6d / %6d              | %d\n",
               s, g_actors[s].type, p1m, p1s, p2m, p2s, oth);
    }
    printf("\nLESART: jeder Tick in der Spalte '+2 / still' ist ein Tick, an dem das Original\n"
           "        (FUN_80015E7C @0x80015FCC-FE4) den DOPPELTEN Wurzel-Delta angewendet haette\n"
           "        und der Port GAR NICHT bewegt.\n");
    printf("\nSONDE-OK\n");
    return 0;
}
