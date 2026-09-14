/**
 * @file probe_gegen_1d3fenster.c
 * @brief GEGENPRUEFUNG eines fremden Befundes am RE2-Zombie.
 *
 * Gemessen werden ZWEI Groessen:
 *  (A) STURZKETTE: Anzahl Bilder vom Eintritt in EXEC[5] (state 1 / sub_state_1 == 5) mit
 *      Phase (+0x6) <= 1 bis zum Erreichen von Phase 2 (dem AUFSCHLAG).
 *      Original (EMOVL10_S0.BIN): P0 setzt Phase 1 + Startframe side*5+10 (10/15) auf Clip
 *      param[side] (1/2, Framecount 60); P1 schaltet nur `+0x6 += advance_ret` @0x80103384-98
 *      und der Advance liefert 1 beim Wrap (@0x80029B28-4C, frame+1 >= fc).
 *      => 50 bzw. 45 Fortschaltungen, mit dem P0-Tick also 51 bzw. 46 Bilder.
 *  (B) +0x1D3-FENSTER: wie lange der Zombie fuer den Kandidatenfilter
 *      (PSX.EXE @0x80047138-40 `lbu v0,467(s0)` / `bne v0,zero,0x8004740C`) unsichtbar ist,
 *      GETRENNT nach "endet lebend" (Sturz) und "endet nicht" (Tod/Leiche).
 *
 * Der RE2-RNG (s_re2_rng, enemy_ai_re2_zombie.c:320) hat KEINEN Seed-Eingang; die Sonde
 * variiert die RNG-Phase, indem sie vor dem Messlauf N Wuerfe verbrennt (re15_re2_rand()).
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
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern int      re15_actor_clip_len(const re15_actor_t *a);
extern uint32_t re15_re2_rand(void);

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

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

/* ---- (A) Sturzketten-Histogramm ---- */
#define MAXD 3000
static int a_hist[MAXD + 1];
static int a_tot = 0, a_max = 0, a_min = 999999;
static int a_side_hist[2][MAXD + 1];

/* ---- (B) +0x1D3-Fenster ---- */
static int b_hist[MAXD + 1];
static int b_tot = 0, b_max = 0, b_min = 999999, b_open_at_end = 0;

int main(int argc, char **argv)
{
    int budget = (argc > 1) ? atoi(argv[1]) : 900;
    int nphase = (argc > 2) ? atoi(argv[2]) : 64;
    int mode   = (argc > 3) ? atoi(argv[3]) : 0;  /* 0 = Dauerfeuer, 1 = Einzelschuesse alle 90 F */
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    const int weapons[3] = { 1, 3, 4 };
    int printed_len = 0;

    for (int wi = 0; wi < 3; wi++) {
        int weapon = weapons[wi];
        for (int ph = 0; ph < nphase; ph++) {
            bringup();
            re15_inv_load_briefing();
            re15_player_set_equipped_weapon(weapon);
            for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
            load_re2_bank(0x10);
            int slot = standing_zombie();
            if (slot < 0) continue;
            for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            pl->x = e->x - ((weapon < 3) ? 1200 : 2600);
            pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
            re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
            for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
            for (int i = 0; i < ph; i++) (void)re15_re2_rand();   /* RNG-Phase variieren */

            if (!printed_len) {
                printed_len = 1;
                re15_actor_t probe = *e;
                printf("CLIP-LAENGEN (re15_actor_clip_len), RE2-Bank EM010:\n ");
                for (int c = 0; c <= 0x18; c++) { probe.motion = (int16_t)c;
                    printf(" [%d]=%d", c, re15_actor_clip_len(&probe)); }
                printf("\n");
            }

            int a_open = 0, a_start = 0, a_side = 0, a_f0 = 0;
            int b_open = 0, b_start = 0;
            for (int f = 0; f < budget; f++) {
                pl->hp = 100;
                { int es = re15_inv_equipped_slot();
                  if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 200; }
                uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
                int want = (mode == 0) ? 1 : ((f % 90) == 10);
                if (want) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
                frame(cur, edge);

                if (mode == 2 && ((e->re2z_self1d3 & 0x80u)
                                  || e->motion == 1 || e->motion == 2))
                    printf("  TRACE f%-4d st=%d s1=%-2u phase=%-2u clip=%-3d anim_frame=%-4d "
                           "frac=%-3u clip_len=%-3d 1D3=%02X side=%u\n",
                           f, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
                           (int)e->anim_frame, (unsigned)e->anim_frac, re15_actor_clip_len(e),
                           e->re2z_self1d3, e->re2z_dir16a);

                /* (A) Sturzkette: EXEC[5], Phase <= 1 */
                int in_fall = (e->state == 1 && e->sub_state_1 == 5 && e->sub_state_2 <= 1);
                if (in_fall && !a_open) {
                    a_open = 1; a_start = f; a_side = (int)(e->re2z_dir16a & 1u);
                    a_f0 = (int)e->anim_frame;
                }
                if (a_open && !in_fall) {
                    a_open = 0;
                    if (e->state == 1 && e->sub_state_1 == 5 && e->sub_state_2 == 2) {
                        int d = f - a_start + 1;
                        if (d < 0) d = 0; if (d > MAXD) d = MAXD;
                        a_hist[d]++; a_side_hist[a_side][d]++; a_tot++;
                        if (d > a_max) a_max = d;
                        if (d < a_min) a_min = d;
                        if (a_tot <= 6)
                            printf("  [Beispiel] w%d ph%d: Sturz %d Bilder, side=%d, "
                                   "anim_frame beim Eintritt=%d, clip=%d\n",
                                   weapon, ph, d, a_side, a_f0, (int)e->motion);
                    }
                }

                /* (B) +0x1D3-Fenster */
                int on = (e->re2z_self1d3 != 0);
                if (on && !b_open) { b_open = 1; b_start = f; }
                if (!on && b_open) {
                    b_open = 0;
                    int d = f - b_start; if (d > MAXD) d = MAXD;
                    b_hist[d]++; b_tot++;
                    if (d > b_max) b_max = d;
                    if (d < b_min) b_min = d;
                }
            }
            if (b_open) b_open_at_end++;
        }
    }

    printf("\n=== (A) STURZKETTE: Bilder von EXEC[5]-Phase<=1 bis Phase 2 (%d Episoden) ===\n", a_tot);
    for (int d = 0; d <= MAXD; d++) if (a_hist[d])
        printf("  %4d Bilder : %-5d   (side0=%d side1=%d)\n", d, a_hist[d],
               a_side_hist[0][d], a_side_hist[1][d]);
    printf("  min=%d max=%d\n", a_min, a_max);
    printf("  ERWARTUNG Original: side0 = 1 + (60-10) = 51 ; side1 = 1 + (60-15) = 46\n");

    printf("\n=== (B) +0x1D3-FENSTER, geschlossene Episoden (%d) ===\n", b_tot);
    for (int d = 0; d <= MAXD; d++) if (b_hist[d]) printf("  %4d Bilder : %d\n", d, b_hist[d]);
    printf("  min=%d max=%d   | Laeufe mit am Budget-Ende NOCH offenem Fenster: %d\n",
           b_min, b_max, b_open_at_end);
    free(buf);
    return 0;
}
