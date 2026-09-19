/* probe_r16_sk_tuer_animation_1040.c — SKEPTIKER-GEGENSONDE (Runde 16, kein Fix) zum Dossier
 * analysis/befunde_2026-09-19/tuer-animation-1040.md.
 *
 * Unterschiede zur Dossier-Sonde probe_r16_tuer1040.c:
 *   1. Pro Bild wird die RENDER-POSE berechnet (re15_skel_compute_pose mit dem
 *      Composite-Skelett = PL00-Bind + W-Keyframes, exakt wie main.c:3363), d.h. der
 *      FRAC-Crossfade (skeleton_common.c) und der Vorpuffer (anim_prev_valid) laufen mit.
 *   2. Der Fix-Plan §4 wird im TESTCODE simuliert (Variante F/G: nach present
 *      motion=210, anim_frame=0, anim_frac=0) — Engine-Code bleibt unangetastet.
 *   3. Die W-Bank kommt wie in main.c aus PL00W<item>.PLW (dir[0]=EDD, dir[1]=EMR),
 *      damit auch Baenke geprueft werden koennen, in denen Clip 1 Bild 0 != Clip 3 Bild 0
 *      (PL00W0C: c1f0=kf20, c3f0=kf90).
 *
 * Varianten (argv[1]):
 *   A = unveraendert, Messer (Item 1)            [Dossier-Reproduktion]
 *   F = Fix simuliert, Messer (Item 1)
 *   G = Fix simuliert, Item 0x0C (Bank W0C)
 *   H = unveraendert, Item 0x0C (Bank W0C)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_player.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_game_step.h"
#include "re15_anim_select.h"
#include "re15_skeleton.h"

extern int  re15_player_victim_state(void);
extern int  re15_player_aim_active(void);
extern int  re15_room_transition_tick(void);
extern int  re15_room_transition_active(void);
extern void re15_player_set_equipped_weapon(int weapon_id);
extern int  re15_player_equipped_weapon(void);
extern void re15_apply_room_cinematic(const uint8_t *rbj, size_t rbj_size, unsigned room_id,
        const re15_emd_skeleton_t *pl00_base,
        re15_emd_skeleton_t *leon_skel, re15_emd_animation_t *leon_anim,
        const re15_emd_skeleton_t *elliot_base, int elliot_ok,
        re15_emd_skeleton_t *elliot_skel, re15_emd_animation_t *elliot_anim,
        re15_emd_skeleton_t *scratch_skel, re15_emd_animation_t *scratch_anim,
        void (*ensure_enemy)(uint8_t type));

static re15_rdt_t                s_rdt;
static int                       s_rdt_ok = 0;
static const re15_camera_cut_t  *s_active_cuts = NULL;
static int                       s_active_cut_count = 0;
static int                       s_active_cut_idx = 0;
static re15_camera_view_t        s_cam_view;

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

static uint8_t *s_room_buf = NULL;
static int probe_load_rdt(unsigned room_id)
{
    char path[600];
    size_t sz = 0;
    snprintf(path, sizeof path, "%s/STAGE%u/ROOM%04X.RDT",
             RE15_ASSET_PSX_DIR, (room_id >> 12) & 0xF, room_id);
    uint8_t *buf = read_file(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return -1; }
    if (re15_rdt_parse(buf, sz, &g_room_rdt) != 0) { free(buf); return -1; }
    if (s_room_buf) free(s_room_buf);
    s_room_buf        = buf;
    g_current_room_id = room_id;
    g_room_rdt_ok     = 1;
    return 0;
}
static void probe_reset_render(void) { }
static int  probe_load_bg(int cut) { (void)cut; return 0; }

static void make_ctx_room(re15_room_apply_ctx_t *rc)
{
    memset(rc, 0, sizeof *rc);
    rc->rdt              = &s_rdt;
    rc->rdt_ok           = &s_rdt_ok;
    rc->active_cuts      = &s_active_cuts;
    rc->active_cut_count = &s_active_cut_count;
    rc->cam_active_cut   = &s_active_cut_idx;
    rc->cam_view         = &s_cam_view;
    rc->load_rdt         = probe_load_rdt;
    rc->reset_render     = probe_reset_render;
    rc->load_bg_cut      = probe_load_bg;
}

/* ---- Baenke wie main.c ---- */
static re15_emd_skeleton_t  s_pl00_skel, s_w_raw_skel, s_w_skel, s_def_skel, s_scr_skel, s_ell_skel;
static re15_emd_animation_t s_pl00_anim, s_w_anim, s_def_anim, s_scr_anim, s_ell_anim;
static int s_pl00_ok = 0, s_w_ok = 0;
static re15_md1_t s_md1;
static char s_w_name[16] = "W??";

static int load_pl00(void)
{
    char p[600]; size_t esz = 0, msz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", RE15_ASSET_PSX_DIR);
    uint8_t *eb = read_file(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR);
    uint8_t *mb = read_file(p, &msz);
    if (!eb || !mb) return 0;
    if (re15_emd_parse_animation(eb, esz, &s_pl00_anim) != 0) return 0;
    if (re15_emd_parse_skeleton(mb, msz, &s_pl00_skel) != 0) return 0;
    return 1;
}

/* PLW-Bank der Waffe wie main.c:3238-3260 (dir[0]=EDD, dir[1]=EMR) + Composite wie main.c:3363. */
static int load_plw(int wid)
{
    char p[600]; size_t psz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00W%02X.PLW", RE15_ASSET_PSX_DIR, wid);
    uint8_t *plw = read_file(p, &psz);
    if (!plw || psz < 16) return 0;
    uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
    if (diroff + 16 > psz) return 0;
    uint32_t de[4];
    for (int k = 0; k < 4; k++)
        de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                           (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
    if (!(de[0] < de[1] && de[1] < de[2] && de[2] <= psz)) return 0;
    if (re15_emd_parse_animation(plw + de[0], (int)(de[1] - de[0]), &s_w_anim) != 0) return 0;
    if (re15_emd_parse_skeleton (plw + de[1], (int)(de[2] - de[1]), &s_w_raw_skel) != 0) return 0;
    s_w_skel = s_pl00_skel;                       /* Composite: PL00-Bind + W-Keyframes */
    s_w_skel.keyframe_data       = s_w_raw_skel.keyframe_data;
    s_w_skel.keyframe_data_size  = s_w_raw_skel.keyframe_data_size;
    s_w_skel.keyframe_count      = s_w_raw_skel.keyframe_count;
    s_w_skel.keyframe_size_bytes = s_w_raw_skel.keyframe_size_bytes;
    snprintf(s_w_name, sizeof s_w_name, "W%02X", wid);
    return 1;
}
static void noop_ensure(uint8_t t) { (void)t; }

static const char *s_def_name = "PL00";
static void select_def_bank(unsigned room)
{
    s_def_skel = s_pl00_skel; s_def_anim = s_pl00_anim; s_def_name = "PL00";
    if (s_rdt_ok && s_rdt.animation && s_rdt.animation_size > 0) {
        re15_apply_room_cinematic(s_rdt.animation, (size_t)s_rdt.animation_size, room,
                                  &s_pl00_skel, &s_def_skel, &s_def_anim,
                                  NULL, 0, &s_ell_skel, &s_ell_anim,
                                  &s_scr_skel, &s_scr_anim, noop_ensure);
        s_def_name = "RDT@5C";
        printf("[bank] ROOM%04X hat RDT-Animationsblock @0x5C (%d Bytes) -> def=%s clips=%d kf=%d\n",
               room, (int)s_rdt.animation_size, s_def_name, s_def_anim.clip_count,
               s_def_skel.keyframe_count);
    } else {
        printf("[bank] ROOM%04X ohne Animationsblock -> def=PL00 clips=%d\n", room, s_def_anim.clip_count);
    }
}

static re15_game_ctx_t s_ctx;
static int s_frame = 0;
static void tick_game(uint16_t pressed, uint16_t held)
{
    extern void re15_fade_tick(void);
    re15_fade_tick();
    re15_room_transition_tick();
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = s_rdt_ok;
    s_ctx.cam_view = &s_cam_view; s_ctx.active_cut = s_active_cut_idx;
    s_ctx.pad_pressed = pressed; s_ctx.pad_current = held;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    s_frame++;
}

/* Render-Sicht + Render-Pose (wie main.c: anim_select -> compute_actor_kf -> compute_pose). */
static re15_skel_pose_t s_poses[RE15_EMD_MAX_BONES];
static int s_b13[3], s_b9[3];
static void render_frame(const char *tag, int t)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_anim_banks_t banks = {
        .def_mesh = &s_md1, .def_skel = &s_def_skel, .def_anim = &s_def_anim,
        .w01_skel = &s_w_skel, .w01_anim = &s_w_anim, .w01_ok = s_w_ok,
        .pl00_skel = &s_pl00_skel, .pl00_anim = &s_pl00_anim, .pl00_ok = s_pl00_ok,
    };
    re15_anim_view_t av;
    re15_actor_anim_select(p, 1, &banks, &av);
    const char *bank = (av.anim == &s_def_anim) ? s_def_name
                     : (av.anim == &s_w_anim) ? s_w_name
                     : (av.anim == &s_pl00_anim) ? "PL00common" : "?";
    int clip = (av.clip_override >= 0) ? av.clip_override : (int)p->motion;
    int fc = 0;
    if (av.anim && clip >= 0 && clip < av.anim->clip_count) fc = av.anim->clips[clip].frame_count;
    int kf = re15_compute_actor_kf(av.anim, av.skel, p, av.clip_override, (uint32_t)p->anim_frame);
    int prev_valid = p->anim_prev_valid, frac_before = p->anim_frac;
    g_anim_pose_actor = p;
    int rc = re15_skel_compute_pose(av.skel, kf, s_poses);
    g_anim_pose_actor = NULL;
    for (int i = 0; i < 3; i++) { s_b13[i] = (int)s_poses[13].trans[i]; s_b9[i] = (int)s_poses[9].trans[i]; }
    printf("%-5s t=%3d mo=%4d af=%3u frac=%u(prev_valid=%d) st=%u pause=%08x trans=%d pmode=%d | "
           "RENDER bank=%-7s clip=%2d fc=%3d kf=%3d | POSE rc=%d b9=(%d,%d,%d) b13=(%d,%d,%d)\n",
           tag, t, (int)p->motion, p->anim_frame, frac_before, prev_valid, p->state,
           (unsigned)g_re15_pauseflags, re15_room_transition_active(), g_scd.player_mode,
           bank, clip, fc, kf, rc, s_b9[0], s_b9[1], s_b9[2], s_b13[0], s_b13[1], s_b13[2]);
}

static int fire_door_to(unsigned wanted_room, int walk_frames)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
        if (a->half_w == 0 && a->half_h == 0) continue;
        const re15_aot_door_params_t *d = &g_aot.door_params[i];
        unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                         | ((unsigned)d->dest_room << 4)
                         | (g_current_room_id & 0x000Fu);
        if (dest_id != wanted_room) continue;
        for (int r = 0; r < 4 && !g_room_change.pending; r++) {
            int rot = r * 1024;
            int32_t cs = re15_cos_q12(rot), sn = re15_sin_q12(rot);
            int back = walk_frames * 75;
            if (back > 1500) back = 1500;
            pl->x = a->x - ((cs * back) >> 12);
            pl->z = a->z + ((sn * back) >> 12);
            pl->rot_y = (int16_t)rot;
            re15_collision_set_band((int)d->band);
            for (int f = 0; f < walk_frames && !g_room_change.pending; f++)
                tick_game(0, (uint16_t)RE15_PAD_BIT_UP);
            pl->x = a->x; pl->z = a->z;
            for (int f = 0; f < 12 && !g_room_change.pending; f++)
                tick_game((f == 0) ? RE15_PAD_BIT_SQUARE : 0,
                          (uint16_t)(RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_UP));
        }
        if (g_room_change.pending) return i;
    }
    return -1;
}

int main(int argc, char **argv)
{
    char variant = (argc > 1) ? argv[1][0] : 'A';
    int nlog = (argc > 2) ? atoi(argv[2]) : 16;
    int fix  = (variant == 'F' || variant == 'G');
    int wid  = (variant == 'G' || variant == 'H') ? 0x0C : 1;
    printf("##### SKEPTIKER-VARIANTE %c (fix_simuliert=%d, Item=%d) #####\n", variant, fix, wid);

    { char p[600]; size_t sz = 0;
      snprintf(p, sizeof p, "%s/PLD/PL00.MD1", RE15_ASSET_PSX_DIR);
      uint8_t *b = read_file(p, &sz);
      if (b) re15_md1_parse(b, sz, &s_md1); }
    s_pl00_ok = load_pl00();
    s_w_ok    = s_pl00_ok ? load_plw(wid) : 0;
    printf("[bank] PL00 ok=%d clips=%d | %s ok=%d clips=%d kf=%d\n", s_pl00_ok, s_pl00_anim.clip_count,
           s_w_name, s_w_ok, s_w_anim.clip_count, s_w_skel.keyframe_count);
    if (!s_pl00_ok || !s_w_ok) { printf("FAIL: Baenke\n"); return 1; }
    printf("[bank] %s.EDD Clip 1 Bild 0 = kf %u (%u Bilder), Clip 3 Bild 0 = kf %u (%u Bilder)\n", s_w_name,
           (unsigned)(s_w_anim.frames[s_w_anim.clips[1].first_frame] & 0xFFF), s_w_anim.clips[1].frame_count,
           (unsigned)(s_w_anim.frames[s_w_anim.clips[3].first_frame] & 0xFFF), s_w_anim.clips[3].frame_count);
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);

    re15_room_apply_ctx_t rc; make_ctx_room(&rc);

    if (probe_load_rdt(0x1030) != 0) return 1;
    s_rdt = g_room_rdt; s_rdt_ok = 1;
    re15_actor_init();
    scd_vm_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -8200; pl->y = 0; pl->z = -22500; pl->rot_y = 0;
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    for (int f = 0; f < 8; f++) scd_vm_tick();
    g_scd.player_mode = 0;
    select_def_bank(0x1030);
    re15_player_set_equipped_weapon(wid);
    printf("[equip] Waffe=%d (equipped=%d)\n", wid, re15_player_equipped_weapon());

    for (int f = 0; f < 30; f++) { tick_game(0, 0); }
    render_frame("IDLE", 0);

    g_scd.player_mode = 0;
    int slot = fire_door_to(0x1040u, 40);
    if (slot < 0 || !g_room_change.pending) { printf("FAIL: Tuer 1030->1040 nicht gefeuert\n"); return 1; }
    render_frame("PREAP", 0);

    if (!re15_room_apply_pending(&rc)) { printf("FAIL: apply 1040\n"); return 1; }
    re15_room_transition_present();
    select_def_bank(0x1040);
    if (fix) {
        /* Fix-Plan §4 Punkt 1, im TESTCODE simuliert (Engine unveraendert). */
        pl->motion = 210; pl->anim_frame = 0; pl->anim_frac = 0;
        pl->anim_flags &= (uint8_t)~RE15_ANIM_FLAG_REVERSE;
        pl->motion_init_delay = 0; pl->anim_use_pl00 = 0;
        printf("[fix] simuliert: motion=210 af=0 frac=0 (nach present, vor Bild 0)\n");
    }
    s_frame = 0;
    printf("== ZIELRAUM %04X ==\n", g_current_room_id);
    render_frame("ENTRY", -1);
    int b13_t4[3] = {0,0,0}, b13_t12[3] = {0,0,0}, b13_t5[3] = {0,0,0};
    for (int t = 0; t < nlog; t++) {
        tick_game(0, 0);
        render_frame("DEST", t);
        if (t == 4)  memcpy(b13_t4,  s_b13, sizeof b13_t4);
        if (t == 5)  memcpy(b13_t5,  s_b13, sizeof b13_t5);
        if (t == 12) memcpy(b13_t12, s_b13, sizeof b13_t12);
    }
    int d4_12 = abs(b13_t4[0]-b13_t12[0]) + abs(b13_t4[1]-b13_t12[1]) + abs(b13_t4[2]-b13_t12[2]);
    int d4_5  = abs(b13_t4[0]-b13_t5[0])  + abs(b13_t4[1]-b13_t5[1])  + abs(b13_t4[2]-b13_t5[2]);
    printf("\nBEFUND (%c, %s): Bone13 t=4 (%d,%d,%d) t=5 (%d,%d,%d) t=12 (%d,%d,%d) | L1 t4->t5 = %d, t4->t12 = %d\n",
           variant, s_w_name, b13_t4[0], b13_t4[1], b13_t4[2], b13_t5[0], b13_t5[1], b13_t5[2],
           b13_t12[0], b13_t12[1], b13_t12[2], d4_5, d4_12);
    return 0;
}
