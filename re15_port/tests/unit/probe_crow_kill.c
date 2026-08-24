/* probe_crow_kill.c — MESSUNG (kein Fix, kein ctest): Nutzer-Report 2026-08-24
 * "Die Finisher Animation wenn man von Kraehen getoetet wird stimmt noch nicht."
 *
 * ORIGINAL (dieser Session roh-disassembliert, STAGE1.BIN + PSX.EXE):
 *   Alle DREI Kraehen-Kill-Sites schreiben IDENTISCH das Spieler-Kommando:
 *     Dive   @0x80113b48 `sb 3 -> 0x800aca58` (+ aca59:=0 @0x80113b6c, aca5a:=0 @0x80113b74)
 *     Grab   @0x80113f20 (+ @0x80113f44 / @0x80113f4c)
 *     Strike @0x80114518 (+ @0x8011453c / ...)
 *   -> cmd 3 = @0x80073f90[3] = 0x800366bc = die GENERISCHE Todes-FSM.
 *   Deren Phase 0 (0x80036764): Clip +0x94 := 7 @0x80036780, Frame 0 @0x80036788,
 *   Blend +0x8f := 7 @0x80036790, +0x8c := 0 @0x80036798, hit_react |= 1 @0x800367a4,
 *   Se_on(0x04030001, &player+0x34) @0x800367a8, aca3c |= 0xC0 @0x800367bc.
 *   Phase 1 (0x800367c8): anim_set(Paar A = [0x800acad8]/[0x800acbc0], 0, 0x200) -> Wrap = Phase 2.
 *   Phase 2 (0x80036804): FUN_80045630(2,0) = Boden-Material-Aufschlag-SE, dann cmd := 7
 *   (sh 7 -> 0x800aca58 @0x80036814) = Blutlachen-Grower 0x8003694c.
 *   PL00.EDD Clip 7 = 113 Frames (Clip-Tabelle u16[i*4] = (fc, off): [7] = (113, 1032)).
 *
 * GEMESSEN WIRD HIER: was der PORT tut, wenn eine Kraehe den Spieler toetet —
 * pl->motion / anim_frame / anim_frac / hp / victim_state / grabbed und die
 * Game-Over-Praesentation (g_death_pool/g_death_cam/g_death_flyin/g_gameover_active)
 * ueber die ersten 200 Ticks nach hp<0. Erwartung bei Byte-Treue: motion springt im
 * Tick des Todes auf 7, laeuft 113 Frames, danach Pool.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_game_step.h"

extern int re15_player_victim_state(void);
extern int g_death_pool, g_death_cam, g_death_flyin, g_gameover_active, g_death_fade;

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

static const char *s_base = NULL;
static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static int s_pl00_ok = 0;

static void load_pl00(void)
{
    char p[600]; size_t a = 0, b = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", s_base);
    uint8_t *edd = read_file(p, &a);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", s_base);
    uint8_t *emr = read_file(p, &b);
    s_pl00_ok = (edd && emr &&
                 re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
                 re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0);
    if (s_pl00_ok) {
        printf("PL00.EDD: clips=%d", s_pl00_anim.clip_count);
        for (int i = 0; i < s_pl00_anim.clip_count && i < 12; i++)
            printf(" [%d]=%d", i, s_pl00_anim.clips[i].frame_count);
        printf("   <- Clip 7 MUSS 113 sein (Todes-Clip @0x80036780)\n");
    } else {
        printf("!! PL00.EDD/EMR nicht ladbar\n");
    }
}

static uint8_t *s_em21_blob = NULL;
static int load_crow_victim_bank(void)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x21);
    if (!eb) eb = re15_enemy_alloc(0x21);
    if (!eb) return 0;
    if (!s_em21_blob) {
        char path[600]; size_t ems_size = 0;
        snprintf(path, sizeof path, "%s/EMD/CDEMD0.EMS", s_base);
        uint8_t *ems = read_file(path, &ems_size);
        if (!ems) { printf("!! CDEMD0.EMS nicht lesbar\n"); return 0; }
        int idx = re15_ems_index_for_type(0x21);
        size_t off = 0, len = 0;
        if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) {
            printf("!! EM021-Blob nicht gefunden\n"); free(ems); return 0;
        }
        s_em21_blob = (uint8_t *)malloc(len + sizeof(size_t));
        memcpy(s_em21_blob, &len, sizeof(size_t));
        memcpy(s_em21_blob + sizeof(size_t), ems + off, len);
        free(ems);
    }
    size_t len; memcpy(&len, s_em21_blob, sizeof(size_t));
    eb->victim_ok = (re15_emd_parse_victim_bank(s_em21_blob + sizeof(size_t), len,
                                                &eb->skel_victim, &eb->anim_victim) == 0);
    eb->ok = 1; eb->buf = NULL;
    return eb->victim_ok;
}

static void setup_room(const re15_rdt_t *rdt, int16_t hp)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_game_flag_set(3, 125, 1);                 /* Re-Entry-Zweig: 7 Pad-Kraehen */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150; pl->hp = hp;
    pl->rot_y = 0; pl->hit_react = 0; pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(4);
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);
    g_scd.combat_active = 1;
    load_crow_victim_bank();
}

static void log_row(int f, const re15_actor_t *pl, const char *tag)
{
    printf("  t=%4d %-6s hp=%4d motion=0x%02x(%3d) frame=%3d frac=%d hit_react=%d "
           "victim=%d grabbed=%d | pool=%d cam=%d flyin=%d fade=%d go=%d | x=%d z=%d\n",
           f, tag, (int)pl->hp, pl->motion, pl->motion, pl->anim_frame, pl->anim_frac,
           pl->hit_react, re15_player_victim_state(), re15_player_is_grabbed(),
           g_death_pool, g_death_cam, g_death_flyin, g_death_fade, g_gameover_active,
           (int)pl->x, (int)pl->z);
}

/* Nach dem Tod 200 Ticks protokollieren + auswerten. */
static void watch_death(re15_game_ctx_t *ctx, re15_actor_t *pl, const char *label)
{
    int motion_changes = 0, saw7 = 0, maxframe = -1;
    uint8_t m0 = pl->motion, prev = pl->motion;
    printf("  --- Todes-Tick erreicht (motion beim Tod = 0x%02x) ---\n", m0);
    for (int f = 0; f < 200; f++) {
        re15_game_step(ctx);
        if (pl->motion != prev) { motion_changes++; prev = pl->motion; }
        if (pl->motion == 7) { saw7 = 1; if (pl->anim_frame > maxframe) maxframe = pl->anim_frame; }
        if (f < 20 || (f % 10) == 0 || f > 190) log_row(f, pl, "dead");
    }
    printf("  == %s: motion-Wechsel nach dem Tod = %d, motion==7 (Todes-Clip) je gesehen = %s"
           " (max frame %d), Endmotion = 0x%02x frame=%d\n",
           label, motion_changes, saw7 ? "JA" : "NEIN", maxframe, pl->motion, pl->anim_frame);
}

/* --- A: Tod durch GRAB (-8 @0x80113e34) -------------------------------------------------- */
static void run_grab_kill(const re15_rdt_t *rdt)
{
    printf("\n===== A: Tod durch KRAEHEN-GRAB (hp=5, Grab zieht -8 @0x80113e34) =====\n");
    setup_room(rdt, 5);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_camera_view_t cam; memset(&cam, 0, sizeof cam);
    re15_game_ctx_t ctx; memset(&ctx, 0, sizeof ctx);
    ctx.rdt = rdt; ctx.rdt_ok = 1; ctx.cam_view = &cam; ctx.active_cut = 0;
    if (s_pl00_ok) { ctx.pl00_skel = &s_pl00_skel; ctx.pl00_anim = &s_pl00_anim; }
    int f;
    for (f = 0; f < 3600 && pl->hp >= 0; f++) {
        re15_game_step(&ctx);
        if (re15_player_victim_state() && (f % 20) == 0) log_row(f, pl, "alive");
    }
    if (pl->hp >= 0) { printf("  !! kein Tod in 3600 Ticks (hp=%d)\n", (int)pl->hp); return; }
    log_row(f, pl, "KILL");
    watch_death(&ctx, pl, "GRAB-KILL");
    g_room_rdt_ok = 0; re15_collision_reset_band();
}

/* --- B: Tod durch DIVE (-4 @0x80113b04) --------------------------------------------------- */
static void run_dive_kill(const re15_rdt_t *rdt)
{
    printf("\n===== B: Tod durch KRAEHEN-DIVE (hp=3, Dive zieht -4 @0x80113b04) =====\n");
    setup_room(rdt, 3);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_camera_view_t cam; memset(&cam, 0, sizeof cam);
    re15_game_ctx_t ctx; memset(&ctx, 0, sizeof ctx);
    ctx.rdt = rdt; ctx.rdt_ok = 1; ctx.cam_view = &cam; ctx.active_cut = 0;
    if (s_pl00_ok) { ctx.pl00_skel = &s_pl00_skel; ctx.pl00_anim = &s_pl00_anim; }
    for (int f = 0; f < 30; f++) re15_game_step(&ctx);
    re15_actor_t *crow = NULL;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x21 && !(g_actors[i].grid_id & 0x10))
            { crow = &g_actors[i]; break; }
    if (!crow) { printf("  !! keine armed Kraehe\n"); return; }
    int f;
    for (f = 0; f < 400 && pl->hp >= 0; f++) {
        if (pl->hp >= 0 && crow->state == 1) {
            crow->x = pl->x + 100; crow->y = pl->y - 1500; crow->z = pl->z + 100;
            crow->sub_state_1 = 11; crow->sub_state_2 = 2; crow->crow_timer = 20;
        }
        re15_game_step(&ctx);
    }
    if (pl->hp >= 0) { printf("  !! kein Tod (hp=%d)\n", (int)pl->hp); return; }
    log_row(f, pl, "KILL");
    watch_death(&ctx, pl, "DIVE-KILL");
    g_room_rdt_ok = 0; re15_collision_reset_band();
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    s_base = base;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    load_pl00();

    re15_actor_init();
    run_dive_kill(&rdt);
    re15_actor_init();
    run_grab_kill(&rdt);
    free(data);
    return 0;
}
