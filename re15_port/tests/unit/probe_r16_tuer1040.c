/* probe_r16_tuer1040.c — MESSSONDE + PIN (Runde 16, 2026-09-19 / Phase 2): Nutzer-Report:
 *   "Ab ROOM1040, sobald man die Tueren durchlaeuft, macht Leon immer am Start im Raum eine
 *    ganz komische Animation" (Marken bei Bild 5 und 8 nach Raumeintritt).
 *
 * Faehrt den ECHTEN Tuer-Pfad ROOM1030 -> ROOM1040 (AOT slot 2 in 1030, Door_aot_set) ueber
 * re15_aot_scan / re15_room_request_change / re15_room_apply_pending +
 * re15_room_transition_present (room_common.c) mit dem VOLLEN re15_game_step als Frame-Maschine
 * und dem Transitions-Tick VOR dem Frame (wie platform/pc/main.c:4196). Danach werden die
 * ersten Bilder im Zielraum protokolliert — Spieler-Zustand UND die RENDER-POSE, die main.c
 * daraus posiert (re15_actor_anim_select -> re15_compute_actor_kf -> re15_skel_compute_pose
 * mit denselben Baenken wie main.c: def = PL00-Basis bzw. Raum-Cinematic-Bank @0x5C,
 * W-Bank = Composite PL00-Bind + PL00W<item>.PLW-Keyframes, COMMON = PL00). Danach die Tuer
 * zurueck nach ROOM1030 (Raum OHNE Block) als Kontrolle.
 *
 * ORIGINAL (Dossier analysis/befunde_2026-09-19/tuer-animation-1040.md §2, Skeptiker bestaetigt):
 *   State 3 der Transitions-FSM nullt das Kommando (@0x8001cbdc); der cmd-0-Handler
 *   LAB_800318f8 endet mit +0x94 := 1 @0x80031c10, +0x95 := 0 @0x80031c18, +0x8f := 0
 *   @0x80031c20 und f314(W-Bank DAT_800acbc4/8, a2=0, a3=0x200) @0x80031c24 -> W-Bank Clip 1
 *   Bild 0, HART. Blende (State 4/5) = Spieler-Freeze (bltz @0x80031c78). Freigabe: Idle case 0
 *   @0x80032088 (+0x94 := 3) / @0x8003209c (+0x8f := 7) = 7-Bild-Crossfade nach Clip 3 Bild 0.
 *   Bei 18 der 21 PLW-Baenke ist Clip 1 Bild 0 keyframe-gleich mit Clip 3 Bild 0 (kein sichtbarer
 *   Blend); bei W0F/W10/W11 (Items 15-17) NICHT — dort blendet auch das Original 7 Bilder.
 *
 * PIN (Phase 2, bankabhaengig formuliert):
 *   ENTRY und t=0..4 (Port-Freeze gemessen t=0..4, Freigabe t=5): mo=210, af=0, frac=0,
 *     RENDER = W-Bank Clip 1, kf = W-Clip-1-Bild-0, Render-Pose == statische Pose dieses Keyframes.
 *   t=5: mo=200, frac=7, RENDER = W-Bank Clip 3, kf = W-Clip-3-Bild-0.
 *   t=12: frac=0, Render-Pose == statische Pose W-Clip-3-Bild-0.
 *   Ist die statische Pose c1f0 == c3f0 (W01/W03/…): Pose t=4 == Pose t=12 (kein Blend sichtbar).
 *   Kontrolle Tuer zurueck nach ROOM1030 (kein Block): ENTRY/t=0..4 ebenfalls W Clip 1 Bild 0.
 *
 * Varianten (argv[1]):
 *   A = Messer (Item 1, Bank W01), vorher 40 Bilder Vorwaertsgehen, Tuer per SQUARE
 *   B = Handgun (Item 3, Bank W03), sonst wie A
 *   C = Messer, vorher STEHEN (kein Gehen)
 *   D = Messer, vorher verletzt (hp=40), Gehen
 *   E = Handgun, Tuer aus dem LAUFEN (CROSS gehalten)
 *   F = Item 0x0F (Bank W0F): Clip 1 Bild 0 != Clip 3 Bild 0 -> Blend-Fall des Originals
 *   N = wie A, aber der Renderer bekommt KEINE W-Bank (w01_ok=0): PSX-Rueckfall messen
 * Rueckgabe != 0 bei Pin-Verletzung. */
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

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
    else         {           printf("  ok:   " __VA_ARGS__); printf("\n"); } \
} while (0)

/* --- Port-seitige Sicht auf den Raum (wie platform/pc/main.c) --- */
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

/* ---- Animationsbaenke wie main.c ---- */
static re15_emd_skeleton_t  s_pl00_skel, s_w_raw_skel, s_w_skel, s_def_skel, s_scr_skel, s_ell_skel;
static re15_emd_animation_t s_pl00_anim, s_w_anim, s_def_anim, s_scr_anim, s_ell_anim;
static int s_pl00_ok = 0, s_w_ok = 0, s_w_for_render = 1;
static int s_pl00_best_l1 = 0;      /* gemessen: kleinster L1 eines PL00-Keyframes zur W-Idle */
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

/* PLW-Bank der Waffe wie main.c (dir[0]=EDD, dir[1]=EMR) + Composite wie main.c:3363
 * (PL00-Bind + W-Keyframes). Das Original laedt dasselbe Paar ueber FUN_80036b68 nach
 * DAT_800acbc8 (EDD) / DAT_800acbc4 (EMR) @0x80036be4/@0x80036c04. */
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
    s_w_skel = s_pl00_skel;
    s_w_skel.keyframe_data       = s_w_raw_skel.keyframe_data;
    s_w_skel.keyframe_data_size  = s_w_raw_skel.keyframe_data_size;
    s_w_skel.keyframe_count      = s_w_raw_skel.keyframe_count;
    s_w_skel.keyframe_size_bytes = s_w_raw_skel.keyframe_size_bytes;
    snprintf(s_w_name, sizeof s_w_name, "W%02X", wid);
    return 1;
}
static void noop_ensure(uint8_t t) { (void)t; }

/* def-Bank fuer den aktuellen Raum bestimmen — exakt der main.c-Zweig nach apply_pending:
 * RDT-Animationsblock @0x5C vorhanden -> re15_apply_room_cinematic auf die PL00-Basis,
 * sonst def = PL00-Basis (Arena-Reset @0x80039738). */
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

/* ---- Statische Pose (kein Blend) eines Keyframes: Summe |trans| ueber alle Bones ---- */
#define NB RE15_EMD_MAX_BONES
typedef struct { int32_t t[NB][3]; int nb; } pose_t;
static void pose_static(const re15_emd_skeleton_t *sk, int kf, pose_t *out)
{
    re15_skel_pose_t poses[NB];
    memset(out, 0, sizeof *out);
    g_anim_pose_actor = NULL;
    if (re15_skel_compute_pose(sk, kf, poses) != 0) return;
    out->nb = sk->bone_count;
    for (int b = 0; b < sk->bone_count && b < NB; b++)
        for (int i = 0; i < 3; i++) out->t[b][i] = (int32_t)poses[b].trans[i];
}
static int pose_l1(const pose_t *a, const pose_t *b)
{
    int d = 0, nb = (a->nb < b->nb) ? a->nb : b->nb;
    for (int k = 0; k < nb; k++)
        for (int i = 0; i < 3; i++) d += abs((int)(a->t[k][i] - b->t[k][i]));
    return d;
}
static int clip_kf0(const re15_emd_animation_t *an, int clip)
{
    if (!an || clip < 0 || clip >= an->clip_count) return -1;
    return (int)(an->frames[an->clips[clip].first_frame] & 0xFFFu);
}

/* ---- Log + Render-Pose je Bild ---- */
static re15_game_ctx_t s_ctx;
static int s_frame = 0;
static void tick_game(uint16_t pressed, uint16_t held)
{
    extern void re15_fade_tick(void);            /* fade_common.c (FUN_80021880) — im Spiel
                                                  * aus render_pc; ohne ihn bliebe die
                                                  * Einblendung ewig aktiv und State 5 kaeme nie */
    re15_fade_tick();
    re15_room_transition_tick();                 /* main.c:4196 — VOR allen Subsystemen */
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = s_rdt_ok;
    s_ctx.cam_view = &s_cam_view; s_ctx.active_cut = s_active_cut_idx;
    s_ctx.pad_pressed = pressed; s_ctx.pad_current = held;
    scd_vm_tick();
    re15_game_step(&s_ctx);
    s_frame++;
}

typedef struct {
    int mo, af, frac, clip, kf, fc, trans, frozen;
    const char *bank;
    pose_t pose;
} frame_t;

static void render_frame(const char *tag, int t, frame_t *out)
{
    re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_anim_banks_t banks = {
        .def_mesh = &s_md1, .def_skel = &s_def_skel, .def_anim = &s_def_anim,
        .w01_skel = &s_w_skel, .w01_anim = &s_w_anim, .w01_ok = s_w_ok && s_w_for_render,
        .pl00_skel = &s_pl00_skel, .pl00_anim = &s_pl00_anim, .pl00_ok = s_pl00_ok,
    };
    re15_anim_view_t av;
    re15_actor_anim_select(p, 1, &banks, &av);
    const char *bank = (av.anim == &s_def_anim) ? s_def_name
                     : (av.anim == &s_w_anim) ? s_w_name
                     : (av.anim == &s_pl00_anim) ? "PL00common" : "?";
    int clip = (av.clip_override >= 0) ? av.clip_override : (int)p->motion;
    int fc = 0;
    if (av.anim && av.anim->clip_count > 0) {
        int ci = (clip >= 0 && clip < av.anim->clip_count) ? clip : (clip % av.anim->clip_count);
        fc = av.anim->clips[ci].frame_count;
    }
    int kf = re15_compute_actor_kf(av.anim, av.skel, p, av.clip_override, (uint32_t)p->anim_frame);
    int frac_before = p->anim_frac, prev_valid = p->anim_prev_valid;
    re15_skel_pose_t poses[NB];
    g_anim_pose_actor = p;
    int rc = re15_skel_compute_pose(av.skel, kf, poses);
    g_anim_pose_actor = NULL;
    memset(&out->pose, 0, sizeof out->pose);
    if (rc == 0) {
        out->pose.nb = av.skel->bone_count;
        for (int b = 0; b < av.skel->bone_count && b < NB; b++)
            for (int i = 0; i < 3; i++) out->pose.t[b][i] = (int32_t)poses[b].trans[i];
    }
    out->mo = (int)p->motion; out->af = p->anim_frame; out->frac = frac_before;
    out->clip = clip; out->kf = kf; out->fc = fc; out->bank = bank;
    out->trans = re15_room_transition_active();
    out->frozen = (g_re15_pauseflags & 0x80000000u) ? 1 : 0;
    printf("%-5s t=%3d F=%4d mo=%4d af=%3u frac=%u(pv=%d) afl=0x%02x st=%u s=%u/%u/%u dly=%u pl00=%u | "
           "pause=%08x trans=%d pmode=%d | RENDER bank=%-10s clip=%2d fc=%3d kf=%3d | "
           "b9=(%d,%d,%d) b13=(%d,%d,%d) | aim=%d vs=%d hp=%d pos=(%ld,%ld) rot=%d\n",
           tag, t, s_frame, out->mo, p->anim_frame, frac_before, prev_valid, p->anim_flags,
           p->state, p->sub_state_1, p->sub_state_2, p->sub_state_3, p->motion_init_delay,
           p->anim_use_pl00, (unsigned)g_re15_pauseflags, out->trans,
           g_scd.player_mode, bank, clip, fc, kf,
           (int)out->pose.t[9][0], (int)out->pose.t[9][1], (int)out->pose.t[9][2],
           (int)out->pose.t[13][0], (int)out->pose.t[13][1], (int)out->pose.t[13][2],
           re15_player_aim_active(), re15_player_victim_state(), (int)p->hp,
           (long)p->x, (long)p->z, (int)p->rot_y);
}

/* Tuer aus dem AKTUELLEN Raum feuern: Spieler in die Rect stellen (Blick auf die Tuer),
 * optional vorher N Bilder gehen, dann SQUARE halten (Autopilot haelt ebenfalls). */
static int fire_door_to(unsigned wanted_room, int walk_frames, uint16_t walk_extra)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    frame_t fr;
    for (int i = 0; i < RE15_AOT_MAX; i++) {
        const re15_aot_t *a = &g_aot.slots[i];
        if (!a->active || a->type != RE15_AOT_TYPE_DOOR) continue;
        if (a->half_w == 0 && a->half_h == 0) continue;
        const re15_aot_door_params_t *d = &g_aot.door_params[i];
        unsigned dest_id = (((unsigned)d->dest_stage + 1u) << 12)
                         | ((unsigned)d->dest_room << 4)
                         | (g_current_room_id & 0x000Fu);
        if (dest_id != wanted_room) continue;
        printf("[door] slot=%d rect c=(%ld,%ld) h=(%ld,%ld) band=%d dest=%04X cut=%d "
               "spawn=(%ld,%ld,%ld) yaw=%d\n",
               i, (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h,
               (int)d->band, dest_id, d->target_cut,
               (long)d->spawn_x, (long)d->spawn_y, (long)d->spawn_z,
               (int)d->spawn_yaw_4096);
        for (int r = 0; r < 4 && !g_room_change.pending; r++) {
            int rot = r * 1024;
            int32_t cs = re15_cos_q12(rot), sn = re15_sin_q12(rot);
            int back = walk_frames * 75;    /* WALK 75/Bild (DAT_80073ea5-Umfeld, s. player_common.c) */
            if (back > 1500) back = 1500;
            pl->x = a->x - ((cs * back) >> 12);
            pl->z = a->z + ((sn * back) >> 12);
            pl->rot_y = (int16_t)rot;
            re15_collision_set_band((int)d->band);
            for (int f = 0; f < walk_frames && !g_room_change.pending; f++) {
                tick_game(0, (uint16_t)(RE15_PAD_BIT_UP | walk_extra));
                if (f >= walk_frames - 3) render_frame("PRE", f - walk_frames, &fr);
            }
            pl->x = a->x; pl->z = a->z;
            for (int f = 0; f < 12 && !g_room_change.pending; f++) {
                tick_game((f == 0) ? RE15_PAD_BIT_SQUARE : 0,
                          (uint16_t)(RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_UP | walk_extra));
                render_frame("HOLD", f, &fr);
            }
        }
        if (g_room_change.pending) return i;
        printf("[door] slot=%d NICHT gefeuert (4 Rotationen)\n", i);
    }
    return -1;
}

/* Die Pin-Pruefung fuer einen Raumeintritt: ENTRY + Bilder 0..nlog-1 (Freeze t=0..4 gemessen,
 * Freigabe t=5 — Port-Blende room_common.c @0x8001cc34-94). */
static void check_entry(const char *room, int nlog, const pose_t *c1f0, const pose_t *c3f0,
                        int kf_c1, int kf_c3, int expect_w)
{
    frame_t fr[64];
    int n = (nlog > 64) ? 64 : nlog;
    render_frame("ENTRY", -1, &fr[0]);
    frame_t entry = fr[0];
    for (int t = 0; t < n; t++) { tick_game(0, 0); render_frame("DEST", t, &fr[t]); }

    printf("-- PIN %s (Bank %s, kf c1f0=%d c3f0=%d, statisch L1(c1f0,c3f0)=%d) --\n",
           room, s_w_name, kf_c1, kf_c3, pose_l1(c1f0, c3f0));
    /* ENTRY: cmd-0-Endzustand @0x80031c10/@0x80031c18/@0x80031c20 */
    CHECK(entry.mo == 210 && entry.af == 0 && entry.frac == 0,
          "%s ENTRY mo=%d af=%d frac=%d (Soll 210/0/0: +0x94:=1 @0x80031c10, +0x95:=0 @0x80031c18, "
          "+0x8f:=0 @0x80031c20)", room, entry.mo, entry.af, entry.frac);
    if (expect_w) {
        CHECK(strcmp(entry.bank, s_w_name) == 0 && entry.clip == 1 && entry.kf == kf_c1,
              "%s ENTRY RENDER bank=%s clip=%d kf=%d (Soll %s/1/%d: W-Bank @0x80031bfc-c04)",
              room, entry.bank, entry.clip, entry.kf, s_w_name, kf_c1);
        CHECK(pose_l1(&entry.pose, c1f0) == 0, "%s ENTRY Render-Pose == statisch c1f0 (L1=%d)",
              room, pose_l1(&entry.pose, c1f0));
    }
    /* t=0..4: Freeze (Pause 0xff000000, Dispatcher bltz @0x80031c78) — Pose steht hart */
    for (int t = 0; t <= 4 && t < n; t++) {
        CHECK(fr[t].frozen && fr[t].trans && fr[t].mo == 210 && fr[t].af == 0 && fr[t].frac == 0,
              "%s t=%d frozen=%d trans=%d mo=%d af=%d frac=%d (Soll Freeze, 210/0/0)",
              room, t, fr[t].frozen, fr[t].trans, fr[t].mo, fr[t].af, fr[t].frac);
        if (expect_w)
            CHECK(strcmp(fr[t].bank, s_w_name) == 0 && fr[t].clip == 1 && fr[t].kf == kf_c1
                  && pose_l1(&fr[t].pose, c1f0) == 0,
                  "%s t=%d RENDER %s clip %d kf %d, Pose==c1f0 (L1=%d)", room, t, fr[t].bank,
                  fr[t].clip, fr[t].kf, pose_l1(&fr[t].pose, c1f0));
    }
    /* t=5: Freigabe, Idle case 0 @0x80032088 (+0x94:=3) / @0x8003209c (+0x8f:=7) */
    if (n > 5) {
        CHECK(!fr[5].frozen && fr[5].mo == 200 && fr[5].af == 0 && fr[5].frac == 7,
              "%s t=5 frozen=%d mo=%d af=%d frac=%d (Soll frei, 200/0/7)",
              room, fr[5].frozen, fr[5].mo, fr[5].af, fr[5].frac);
        if (expect_w)
            CHECK(strcmp(fr[5].bank, s_w_name) == 0 && fr[5].clip == 3 && fr[5].kf == kf_c3,
                  "%s t=5 RENDER %s clip %d kf %d (Soll %s/3/%d)", room, fr[5].bank, fr[5].clip,
                  fr[5].kf, s_w_name, kf_c3);
    }
    /* t=12: Crossfade (7 Bilder) abgelaufen -> statische Pose von W Clip 3 am aktuellen Bild.
     * Clip 3 hat bei W01/W03 EIN Bild (Bild 0 = kf 22), bei W0F/W10/W11 aber 50 Bilder — dort
     * steht der Spieler bei t=12 auf Bild af (Clock laeuft ab der Freigabe t=5), nicht auf Bild 0. */
    if (n > 12) {
        CHECK(fr[12].frac == 0, "%s t=12 frac=%d (Soll 0: 7-Bild-Blend @0x8003209c abgelaufen)",
              room, fr[12].frac);
        int d4_12 = pose_l1(&fr[4].pose, &fr[12].pose);
        if (expect_w) {
            int fc3 = s_w_anim.clips[3].frame_count;
            int slot = (fr[12].af < fc3) ? fr[12].af : (fc3 - 1);
            int kf12 = (int)(s_w_anim.frames[s_w_anim.clips[3].first_frame + slot] & 0xFFFu);
            pose_t c3s; pose_static(&s_w_skel, kf12, &c3s);
            CHECK(strcmp(fr[12].bank, s_w_name) == 0 && fr[12].clip == 3 && fr[12].kf == kf12
                  && pose_l1(&fr[12].pose, &c3s) == 0,
                  "%s t=12 RENDER %s clip %d kf %d (Soll %s/3/Bild %d = kf %d), Render-Pose == statisch (L1=%d)",
                  room, fr[12].bank, fr[12].clip, fr[12].kf, s_w_name, slot, kf12, pose_l1(&fr[12].pose, &c3s));
            if (pose_l1(c1f0, c3f0) == 0 && fc3 == 1)
                CHECK(d4_12 == 0, "%s Bank %s: c1f0==c3f0, Clip 3 = 1 Bild -> Pose t=4 == Pose t=12 "
                      "(L1=%d, kein sichtbarer Blend)", room, s_w_name, d4_12);
            else
                printf("  info: %s Bank %s: c1f0!=c3f0 (statisch L1=%d, Clip 3 fc=%d) -> Blend wie im "
                       "Original, L1 t4->t12 = %d\n", room, s_w_name, pose_l1(c1f0, c3f0), fc3, d4_12);
        } else {
            /* Rueckfall ohne W-Bank (Variante N): der Renderer muss trotzdem eine IDLE-Pose
             * liefern — Soll ist die COMMON-Bank PL00 mit dem gemessen naechsten Keyframe zur
             * W-Idle (s. [posediff] PL00-Scan), nicht die Raum-Cinematic-Bank (def @0x5C). */
            /* Soll: COMMON-Bank Clip 22 = der Idle-Clip, den das Original selbst aus PL00 spielt
             * (Idle-Fall 9/a: `ori v0,zero,0x16` @0x80032284, `sb v0,0x800acae8` @0x8003228c,
             * PL00-Paar @0x800322c0/@0x800322c8, f314 @0x800322cc). Vorher: def-Bank Clip 210%5=0
             * bzw. 6%5=1 (Cutscene-Clips der RDT@5C-Bank). */
            int kf22 = clip_kf0(&s_pl00_anim, 22);
            CHECK(strcmp(fr[4].bank, "PL00common") == 0 && fr[4].clip == 22 && fr[4].kf == kf22
                  && strcmp(fr[12].bank, "PL00common") == 0 && fr[12].clip == 22,
                  "%s ohne W-Bank: RENDER t=4 %s clip %d kf %d / t=12 %s clip %d (Soll PL00common/22/%d, "
                  "nicht %s)", room, fr[4].bank, fr[4].clip, fr[4].kf, fr[12].bank, fr[12].clip, kf22, s_def_name);
            printf("  info: %s ohne W-Bank: L1 zur W-Idle t=4 %d / t=12 %d (bester PL00-Keyframe %d)\n",
                   room, pose_l1(&fr[4].pose, c3f0), pose_l1(&fr[12].pose, c3f0), s_pl00_best_l1);
        }
        printf("  info: %s b13 t=4 (%d,%d,%d) t=5 (%d,%d,%d) t=12 (%d,%d,%d) | L1 t4->t12 (alle Bones) = %d\n",
               room, (int)fr[4].pose.t[13][0], (int)fr[4].pose.t[13][1], (int)fr[4].pose.t[13][2],
               (int)fr[5].pose.t[13][0], (int)fr[5].pose.t[13][1], (int)fr[5].pose.t[13][2],
               (int)fr[12].pose.t[13][0], (int)fr[12].pose.t[13][1], (int)fr[12].pose.t[13][2], d4_12);
    }
}

int main(int argc, char **argv)
{
    char variant = (argc > 1) ? argv[1][0] : 'A';
    int nlog = (argc > 2) ? atoi(argv[2]) : 20;
    printf("##### VARIANTE %c (A=Messer+Gehen, B=Handgun+Gehen, C=Messer+Stehen, "
           "D=Messer+verletzt, E=Handgun+Laufen, F=Item 0x0F/W0F, N=ohne W-Bank im Renderer) #####\n",
           variant);

    /* Baenke */
    { char p[600]; size_t sz = 0;
      snprintf(p, sizeof p, "%s/PLD/PL00.MD1", RE15_ASSET_PSX_DIR);
      uint8_t *b = read_file(p, &sz);
      if (b) re15_md1_parse(b, sz, &s_md1); }
    s_pl00_ok = load_pl00();
    int weapon = (variant == 'B' || variant == 'E') ? 3 : (variant == 'F') ? 0x0F : 1;
    s_w_ok = load_plw(weapon);
    s_w_for_render = (variant != 'N');
    printf("[bank] PL00 ok=%d clips=%d | %s ok=%d clips=%d (Renderer sieht W-Bank: %d)\n",
           s_pl00_ok, s_pl00_anim.clip_count, s_w_name, s_w_ok, s_w_anim.clip_count, s_w_for_render);
    if (!s_pl00_ok || !s_w_ok) { printf("FAIL: Baenke nicht ladbar\n"); return 2; }
    printf("[bank] PL00.EDD Clip-Laengen:");
    for (int c = 0; c < s_pl00_anim.clip_count; c++) printf(" %d:%d", c, s_pl00_anim.clips[c].frame_count);
    printf("\n[bank] %s.EDD Clip-Laengen:", s_w_name);
    for (int c = 0; c < s_w_anim.clip_count; c++) printf(" %d:%d", c, s_w_anim.clips[c].frame_count);
    printf("\n");
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);

    /* Statische Sollposen (Original: Blende = W Clip 1 Bild 0; Idle = W Clip 3 Bild 0) */
    pose_t c1f0, c3f0, pl00c6, pl00c0;
    int kf_c1 = clip_kf0(&s_w_anim, 1), kf_c3 = clip_kf0(&s_w_anim, 3);
    pose_static(&s_w_skel, kf_c1, &c1f0);
    pose_static(&s_w_skel, kf_c3, &c3f0);
    pose_static(&s_pl00_skel, clip_kf0(&s_pl00_anim, 6), &pl00c6);
    pose_static(&s_pl00_skel, clip_kf0(&s_pl00_anim, 0), &pl00c0);
    printf("[posediff] %s clip1 f0 kf=%3d b13=(%d,%d,%d) | clip3 f0 kf=%3d b13=(%d,%d,%d) | L1(c1f0,c3f0)=%d\n",
           s_w_name, kf_c1, (int)c1f0.t[13][0], (int)c1f0.t[13][1], (int)c1f0.t[13][2],
           kf_c3, (int)c3f0.t[13][0], (int)c3f0.t[13][1], (int)c3f0.t[13][2], pose_l1(&c1f0, &c3f0));
    printf("[posediff] PL00 clip6 f0 kf=%3d fc=%d b13=(%d,%d,%d) L1 zu %s c3f0 = %d | PL00 clip0 f0 kf=%d L1 zu c3f0 = %d\n",
           clip_kf0(&s_pl00_anim, 6), s_pl00_anim.clip_count > 6 ? s_pl00_anim.clips[6].frame_count : -1,
           (int)pl00c6.t[13][0], (int)pl00c6.t[13][1], (int)pl00c6.t[13][2], s_w_name,
           pose_l1(&pl00c6, &c3f0), clip_kf0(&s_pl00_anim, 0), pose_l1(&pl00c0, &c3f0));
    /* Welche PL00-Pose kommt der W-Idle (Clip 3 Bild 0) am naechsten? Scan ueber ALLE
     * PL00-Keyframes (fuer den Rueckfall ohne W-Bank) und ueber jedes Clip-Bild-0. */
    {
        int best_kf = -1; s_pl00_best_l1 = 0x7fffffff;
        for (int k = 0; k < s_pl00_skel.keyframe_count; k++) {
            pose_t pp; pose_static(&s_pl00_skel, k, &pp);
            int d = pose_l1(&pp, &c3f0);
            if (d < s_pl00_best_l1) { s_pl00_best_l1 = d; best_kf = k; }
        }
        printf("[posediff] PL00-Keyframe-Scan (%d kf): naechster zu %s c3f0 = kf %d, L1 = %d\n",
               s_pl00_skel.keyframe_count, s_w_name, best_kf, s_pl00_best_l1);
        printf("[posediff] PL00 Clip-Bild-0 L1 zu %s c3f0:", s_w_name);
        for (int c = 0; c < s_pl00_anim.clip_count; c++) {
            pose_t pp; pose_static(&s_pl00_skel, clip_kf0(&s_pl00_anim, c), &pp);
            printf(" c%d(kf%d,fc%d)=%d", c, clip_kf0(&s_pl00_anim, c), s_pl00_anim.clips[c].frame_count,
                   pose_l1(&pp, &c3f0));
        }
        printf("\n");
    }

    re15_room_apply_ctx_t rc; make_ctx_room(&rc);

    /* ===== PHASE 0: ROOM1030 booten (Debug-JUMP-Spawn (-8200,0,-22500)) ===== */
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

    re15_player_set_equipped_weapon(weapon);
    printf("[equip] Waffe=%d (equipped=%d) -> Bank %s\n", weapon, re15_player_equipped_weapon(), s_w_name);
    if (variant == 'D') pl->hp = 40;

    /* etwas settlen (Idle) */
    frame_t fr;
    for (int f = 0; f < 30; f++) tick_game(0, 0);
    render_frame("IDLE", 0, &fr);

    int walk = (variant == 'C') ? 0 : 40;
    uint16_t extra = (variant == 'E') ? RE15_PAD_BIT_CROSS : 0;
    g_scd.player_mode = 0;
    int slot = fire_door_to(0x1040u, walk, extra);
    if (slot < 0 || !g_room_change.pending) { printf("FAIL: Tuer 1030->1040 nicht gefeuert\n"); return 1; }
    printf("[queue] room=%04X spawn=(%ld,%ld,%ld) yaw=%d cut=%d\n",
           g_room_change.room_id, (long)g_room_change.x, (long)g_room_change.y,
           (long)g_room_change.z, (int)g_room_change.yaw_4096, g_room_change.target_cut);
    render_frame("PREAP", 0, &fr);

    /* ===== PHASE 1: Tuer anwenden — exakt die main.c-Reihenfolge ===== */
    if (!re15_room_apply_pending(&rc)) { printf("FAIL: apply 1040\n"); return 1; }
    re15_room_transition_present();
    select_def_bank(0x1040);
    s_frame = 0;
    printf("== ZIELRAUM %04X — Zustand direkt nach apply+present (vor Bild 0) ==\n", g_current_room_id);

    /* ===== PHASE 2: die ersten nlog Bilder im Zielraum + Pin ===== */
    check_entry("1040", nlog, &c1f0, &c3f0, kf_c1, kf_c3, variant != 'N');

    /* ===== PHASE 3: Kontrolle — Tuer zurueck nach ROOM1030 (Raum OHNE Block) ===== */
    if (variant != 'N') {
        for (int f = 0; f < 60; f++) tick_game(0, 0);   /* Idle ausklingen lassen */
        g_scd.player_mode = 0;
        int slot2 = fire_door_to(0x1030u, 40, 0);
        if (slot2 < 0 || !g_room_change.pending) { printf("FAIL: Tuer 1040->1030 nicht gefeuert\n"); return 1; }
        if (!re15_room_apply_pending(&rc)) { printf("FAIL: apply 1030\n"); return 1; }
        re15_room_transition_present();
        select_def_bank(0x1030);
        s_frame = 0;
        printf("== ZIELRAUM %04X (Kontrolle, kein Block) ==\n", g_current_room_id);
        check_entry("1030", nlog, &c1f0, &c3f0, kf_c1, kf_c3, 1);
    }

    printf("\n%s (%c, %s): %d Pin-Verletzung(en)\n", g_fail ? "DEFEKT" : "OK", variant, s_w_name, g_fail);
    return g_fail ? 1 : 0;
}
