/* probe_ada_escort_anim.c — MESSUNG: welche Animation spielt die Typ-0x42-NPC ("Ada")
 * waehrend der Eskorte (State 1), und aus WELCHER Bank posiert der Renderer?
 *
 * Original-Referenz (disasm, siehe Bericht):
 *   Sub-0-EXEC 0x8004f310: +0x94=2 / +0x95=0 / +0x8f=7 NUR bei +0x6==0 (@0x8004f328-74),
 *                          dann anim_set(+0x170,+0x174,0,0x200) @0x8004f38c
 *   Sub-1-EXEC 0x8004f4e0: +0x94=5 / +0x95=0 / +0x8f=7 NUR bei +0x6==0 (@0x8004f4f8-f544),
 *                          dann anim_set(+0x170,+0x174,0,0x200) @0x8004f5c8
 *   anim_set FUN_8001f3bc: +0x95 += 1 mit Wrap an der Clip-Laenge (@0x8001f618-3c),
 *                          +0x8f -= 1 (@0x8001f5a8-b4)
 *   FUN_80022300: dir[3] -> +0x174 (@0x800224b8), dir[4] -> +0x170 (@0x800224c8) = BANK 1
 *
 * KEIN add_test — reine Messsonde, kein Pass/Fail. */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t            s_blob42[0x80000];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* exakt das, was pc_enemy_load_ex (platform/pc/main.c) an Baenken registriert */
static int load_bank(uint8_t type, uint8_t *blob, size_t blobcap)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0 && len <= blobcap) {
        memcpy(blob, ems + off, len);
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (eb) {
            re15_tim_t tim;
            memset(&tim, 0, sizeof tim);
            if (re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                eb->loco_ok   = (re15_emd_parse_loco_bank  (blob, len, &eb->skel_loco,   &eb->anim_loco)   == 0);
                eb->victim_ok = (re15_emd_parse_victim_bank(blob, len, &eb->skel_victim, &eb->anim_victim) == 0);
                eb->own_ok    = (re15_emd_parse_own_bank   (blob, len, &eb->skel_own,    &eb->anim_own)    == 0);
                ok = 1;
            }
        }
    }
    free(ems);
    return ok;
}

static int s_scd_off = 0;   /* 1 = die Raum-Skript-Threads stillegen (sie greifen die NPC
                             * sonst per Plc_dest/Plc_motion ab und ueberschreiben die Eskorte) */
static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    if (!s_scd_off) scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* Der Bank-Entscheid des RENDERERS, 1:1 nachgebaut aus platform/pc/main.c
 * (re15_actor_anim_select-Default = Container-Bank eb->skel/eb->anim, danach die
 * state-4-Overrides). 0 = Container-Bank (dir[1]/dir[2]), 1 = own/BANK 1
 * (dir[3]/dir[4]), 2 = loco, 3 = victim. */
static int render_bank_of(const re15_actor_t *n, const re15_emd_skeleton_t **sk,
                          const re15_emd_animation_t **an)
{
    re15_enemy_bank_t *b = re15_enemy_find(n->type);
    if (!b) { *sk = NULL; *an = NULL; return -1; }
    *sk = &b->skel; *an = &b->anim;                       /* anim_select-Default */
    if (n->state == 4 && !n->walk_active &&
        (n->sub_state_1 == 1 || n->sub_state_1 == 7 || n->sub_state_1 == 8) && b->loco_ok)
        { *sk = &b->skel_loco; *an = &b->anim_loco; return 2; }
    if (n->state == 4 && !n->walk_active && n->sub_state_1 == 3 && b->victim_ok)
        { *sk = &b->skel_victim; *an = &b->anim_victim; return 3; }
    if (n->walk_active || (n->state == 4 && (n->sub_state_1 == 2 || n->sub_state_1 == 4 ||
        n->sub_state_1 == 5 || n->sub_state_1 == 6 || n->sub_state_1 == 9))) {
        if (b->own_ok && (int)n->motion < b->anim_own.clip_count)
            { *sk = &b->skel_own; *an = &b->anim_own; return 1; }
    }
    return 0;
}

#define MAXB RE15_EMD_MAX_BONES
static re15_skel_pose_t s_poses[MAXB];

static void dump_amp(const char *title, const re15_emd_skeleton_t *sk,
                     const re15_emd_animation_t *an, int clip, const re15_actor_t *proto)
{
    if (!sk || !an || clip < 0 || clip >= an->clip_count) return;
    int fc = an->clips[clip].frame_count;
    int32_t mn[MAXB][3], mx[MAXB][3];
    for (int b = 0; b < MAXB; b++) for (int k = 0; k < 3; k++)
        { mn[b][k] = 0x7fffffff; mx[b][k] = -0x7fffffff; }
    re15_actor_t tmp = *proto; tmp.anim_frac = 0; tmp.motion = (uint8_t)clip;
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    for (int fr = 0; fr < fc; fr++) {
        tmp.anim_frame = fr;
        int kf = re15_compute_actor_kf(an, sk, &tmp, clip, (uint32_t)fr);
        if (kf < 0 || re15_skel_compute_pose(sk, kf, s_poses) != 0) continue;
        for (int b = 0; b < sk->bone_count && b < MAXB; b++)
            for (int k = 0; k < 3; k++) {
                int32_t v = s_poses[b].trans[k];
                if (v < mn[b][k]) mn[b][k] = v;
                if (v > mx[b][k]) mx[b][k] = v;
            }
    }
    g_anim_pose_actor = save;
    printf("\n  -- %s Clip %d, %d Frames, %d Bones --\n", title, clip, fc, sk->bone_count);
    for (int b = 0; b < sk->bone_count && b < MAXB; b++) {
        if (mn[b][0] == 0x7fffffff) continue;
        printf("     bone %2d: dX=%5ld dY=%5ld dZ=%5ld\n", b,
               (long)(mx[b][0]-mn[b][0]), (long)(mx[b][1]-mn[b][1]), (long)(mx[b][2]-mn[b][2]));
    }
}



/* ===================== REACHABILITY-SWEEP ===================== */
static re15_actor_t *find42(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x42) return &g_actors[s];
    return NULL;
}
static void boot_room(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1090;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0;
    re15_collision_set_band(0);
    load_bank(0x42, s_blob42, sizeof s_blob42);
    re15_game_flag_set(3, 0x81, 1);
    re15_game_flag_set(3, 0x84, 1);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, 0, 0, 0);
}
int main(void)
{
    size_t sz = 0; uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!buf) { printf("FAIL RDT\n"); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL parse\n"); return 1; }
    memset(&s_cam,0,sizeof s_cam); memset(&s_ctx,0,sizeof s_ctx);
    s_ctx.rdt=&s_rdt; s_ctx.rdt_ok=1; s_ctx.cam_view=&s_cam;
    s_scd_off = 0;
    printf("ROOM1090 sub_scd_count = %d\n", s_rdt.sub_scd_count);
    printf("SWEEP: jeder Sub einzeln gestartet, 200 Frames — erreicht Typ 0x42 je State 1?\n");
    int any = 0;
    for (int i = 0; i < s_rdt.sub_scd_count; i++) {
        if (!s_rdt.sub_scd[i]) continue;
        boot_room();
        for (int f = 0; f < 5; f++) frame_step();
        /* freien Thread suchen */
        int started = scd_thread_start(6, s_rdt.sub_scd[i]);
        int hit = -1; int st_seen = 0;
        for (int f = 0; f < 200; f++) {
            frame_step();
            re15_actor_t *n = find42();
            if (!n) continue;
            st_seen |= (1 << (n->state & 7));
            if (n->state == 1 && hit < 0) hit = f;
        }
        if (hit >= 0) any = 1;
        printf("  sub%02d start=%d  states={", i, started);
        for (int b=0;b<8;b++) if (st_seen & (1<<b)) printf("%d,", b);
        printf("}  state1@F=%d\n", hit);
    }
    printf("\n=> Typ 0x42 erreicht State 1 in ROOM1090 durch IRGENDEINEN Sub: %s\n",
           any ? "JA" : "NEIN");
    return 0;
}
