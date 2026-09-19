/* probe_r16_tuer1040.c — MESSSONDE (Runde 16, kein Fix): Nutzer-Report 2026-09-19:
 *   "Ab ROOM1040, sobald man die Tueren durchlaeuft, macht Leon immer am Start im Raum eine
 *    ganz komische Animation" (Marken bei Bild 5 und 8 nach Raumeintritt).
 *
 * Faehrt den ECHTEN Tuer-Pfad ROOM1030 -> ROOM1040 (AOT slot 2 in 1030, Door_aot_set) ueber
 * re15_aot_scan / re15_room_request_change / re15_room_apply_pending +
 * re15_room_transition_present (room_common.c) mit dem VOLLEN re15_game_step als Frame-Maschine
 * und dem Transitions-Tick VOR dem Frame (wie platform/pc/main.c:4196). Danach werden die
 * ersten 40 Bilder im Zielraum protokolliert — Spieler-Zustand UND das, was der Renderer
 * daraus posieren wuerde (re15_actor_anim_select + re15_compute_actor_kf mit denselben
 * Baenken wie main.c: def = PL00-Basis bzw. Raum-Cinematic-Bank, W-Bank = PL00W<item>,
 * COMMON = PL00).
 *
 * Varianten (argv[1]):
 *   A = Messer (Item 1, Default), vorher 40 Bilder Vorwaertsgehen, Tuer per SQUARE
 *   B = Handgun (Item 3), sonst wie A
 *   C = Messer, vorher STEHEN (kein Gehen)
 *   D = Messer, vorher verletzt (hp=40), Gehen
 *   E = Handgun, Tuer aus dem LAUFEN (CROSS gehalten)
 * Keine der Varianten aendert Engine-Code — reine Beobachtung. */
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
static re15_emd_skeleton_t  s_pl00_skel, s_w01_skel, s_w03_skel, s_def_skel, s_scr_skel, s_ell_skel;
static re15_emd_animation_t s_pl00_anim, s_w01_anim, s_w03_anim, s_def_anim, s_scr_anim, s_ell_anim;
static int s_pl00_ok = 0, s_w01_ok = 0, s_w03_ok = 0;
static re15_md1_t s_md1;

static int load_bank(const char *edd, const char *emr, re15_emd_skeleton_t *sk, re15_emd_animation_t *an)
{
    char p[600]; size_t esz = 0, msz = 0;
    snprintf(p, sizeof p, "%s/PLD/%s", RE15_ASSET_PSX_DIR, edd);
    uint8_t *eb = read_file(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/%s", RE15_ASSET_PSX_DIR, emr);
    uint8_t *mb = read_file(p, &msz);
    if (!eb || !mb) { fprintf(stderr, "WARN: Bank %s/%s nicht lesbar\n", edd, emr); return 0; }
    if (re15_emd_parse_animation(eb, esz, an) != 0) return 0;
    if (re15_emd_parse_skeleton(mb, msz, sk) != 0) return 0;
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

/* ---- Log ---- */
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

static void dump_render_view(const char *tag, int t)
{
    const re15_actor_t *p = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int w = re15_player_equipped_weapon();
    re15_anim_banks_t banks = {
        .def_mesh = &s_md1, .def_skel = &s_def_skel, .def_anim = &s_def_anim,
        .w01_skel = (w >= 3 && s_w03_ok) ? &s_w03_skel : &s_w01_skel,
        .w01_anim = (w >= 3 && s_w03_ok) ? &s_w03_anim : &s_w01_anim,
        .w01_ok   = (w >= 3) ? s_w03_ok : s_w01_ok,
        .pl00_skel = &s_pl00_skel, .pl00_anim = &s_pl00_anim, .pl00_ok = s_pl00_ok,
    };
    re15_anim_view_t av;
    re15_actor_anim_select(p, 1, &banks, &av);
    const char *bank = (av.anim == &s_def_anim) ? s_def_name
                     : (av.anim == &s_w01_anim) ? "W01"
                     : (av.anim == &s_w03_anim) ? "W03"
                     : (av.anim == &s_pl00_anim) ? "PL00common" : "?";
    int clip = (av.clip_override >= 0) ? av.clip_override : (int)p->motion;
    int fc = 0; uint16_t fe = 0;
    if (av.anim && clip >= 0 && clip < av.anim->clip_count) {
        fc = av.anim->clips[clip].frame_count;
        if (fc > 0) {
            int slot = (p->anim_frame < fc) ? p->anim_frame : (fc - 1);
            fe = av.anim->frames[av.anim->clips[clip].first_frame + slot];
        }
    }
    int kf = re15_compute_actor_kf(av.anim, av.skel, p, av.clip_override, (uint32_t)p->anim_frame);
    printf("%-5s t=%3d F=%4d mo=%4d af=%3u frac=%u afl=0x%02x st=%u s=%u/%u/%u dly=%u pl00=%u | "
           "pause=%08x trans=%d pmode=%d | RENDER bank=%-10s clip=%2d fc=%3d slot->kf=%3d fe=0x%04x "
           "aim=%d vs=%d hp=%d pos=(%ld,%ld) rot=%d\n",
           tag, t, s_frame, (int)p->motion, p->anim_frame, p->anim_frac, p->anim_flags,
           p->state, p->sub_state_1, p->sub_state_2, p->sub_state_3, p->motion_init_delay,
           p->anim_use_pl00, (unsigned)g_re15_pauseflags, re15_room_transition_active(),
           g_scd.player_mode, bank, clip, fc, kf, fe,
           re15_player_aim_active(), re15_player_victim_state(), (int)p->hp,
           (long)p->x, (long)p->z, (int)p->rot_y);
}

/* Tuer aus dem AKTUELLEN Raum feuern: Spieler in die Rect stellen (Blick auf die Tuer),
 * optional vorher N Bilder gehen, dann SQUARE halten (Autopilot haelt ebenfalls). */
static int fire_door_to(unsigned wanted_room, int walk_frames, uint16_t walk_extra)
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
        printf("[door] slot=%d rect c=(%ld,%ld) h=(%ld,%ld) band=%d dest=%04X cut=%d "
               "spawn=(%ld,%ld,%ld) yaw=%d\n",
               i, (long)a->x, (long)a->z, (long)a->half_w, (long)a->half_h,
               (int)d->band, dest_id, d->target_cut,
               (long)d->spawn_x, (long)d->spawn_y, (long)d->spawn_z,
               (int)d->spawn_yaw_4096);
        for (int r = 0; r < 4 && !g_room_change.pending; r++) {
            /* Spieler etwas VOR die Rect stellen und hineingehen (so wie ein Spieler / der
             * Autopilot ankommt), Blick = Rotation r. Blick (cos, -sin). */
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
                if (f >= walk_frames - 3) dump_render_view("PRE", f - walk_frames);
            }
            pl->x = a->x; pl->z = a->z;
            for (int f = 0; f < 12 && !g_room_change.pending; f++) {
                tick_game((f == 0) ? RE15_PAD_BIT_SQUARE : 0,
                          (uint16_t)(RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_UP | walk_extra));
                dump_render_view("HOLD", f);
            }
        }
        if (g_room_change.pending) return i;
        printf("[door] slot=%d NICHT gefeuert (4 Rotationen)\n", i);
    }
    return -1;
}

int main(int argc, char **argv)
{
    char variant = (argc > 1) ? argv[1][0] : 'A';
    int nlog = (argc > 2) ? atoi(argv[2]) : 40;
    printf("##### VARIANTE %c (A=Messer+Gehen, B=Handgun+Gehen, C=Messer+Stehen, "
           "D=Messer+verletzt, E=Handgun+Laufen) #####\n", variant);

    /* Baenke */
    { char p[600]; size_t sz = 0;
      snprintf(p, sizeof p, "%s/PLD/PL00.MD1", RE15_ASSET_PSX_DIR);
      uint8_t *b = read_file(p, &sz);
      if (b) re15_md1_parse(b, sz, &s_md1); }
    s_pl00_ok = load_bank("PL00.EDD",    "PL00.EMR",    &s_pl00_skel, &s_pl00_anim);
    s_w01_ok  = load_bank("PL00W01.EDD", "PL00W01.EMR", &s_w01_skel,  &s_w01_anim);
    s_w03_ok  = load_bank("PL00W03.EDD", "PL00W03.EMR", &s_w03_skel,  &s_w03_anim);
    printf("[bank] PL00 ok=%d clips=%d | W01 ok=%d clips=%d | W03 ok=%d clips=%d\n",
           s_pl00_ok, s_pl00_anim.clip_count, s_w01_ok, s_w01_anim.clip_count,
           s_w03_ok, s_w03_anim.clip_count);
    if (s_pl00_ok) {
        printf("[bank] PL00.EDD Clip-Laengen:");
        for (int c = 0; c < s_pl00_anim.clip_count; c++) printf(" %d:%d", c, s_pl00_anim.clips[c].frame_count);
        printf("\n[bank] W01.EDD Clip-Laengen:");
        for (int c = 0; c < s_w01_anim.clip_count; c++) printf(" %d:%d", c, s_w01_anim.clips[c].frame_count);
        printf("\n");
        re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);
    }

    re15_room_apply_ctx_t rc; make_ctx_room(&rc);

    /* ===== POSEDIFF: die Kandidaten-Posen der ersten Bilder, Render-Ebene (Bone 9 = Kopf,
     * Bone 13 = rechter Unterarm, wie RE15_POSE_DUMP). Original haelt waehrend der Blende
     * W-Bank Clip 1 Bild 0 (cmd-0 @0x80031c10) und blendet dann 7 Bilder nach W-Bank Clip 3
     * (Idle case0 @0x80032088/@0x8003209c). Der Port haelt def-Bank Clip 0 Bild 0. ===== */
    {
        struct { const char *n; const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an; int clip; } cand[] = {
            { "W01 clip1 f0 (Original Blende)",  &s_w01_skel,  &s_w01_anim,  1 },
            { "W01 clip3 f0 (Original Idle)",    &s_w01_skel,  &s_w01_anim,  3 },
            { "W03 clip1 f0 (Original, Gun)",    &s_w03_skel,  &s_w03_anim,  1 },
            { "W03 clip3 f0 (Original Idle Gun)",&s_w03_skel,  &s_w03_anim,  3 },
            { "PL00 clip0 f0 (Port, Raum ohne Block)", &s_pl00_skel, &s_pl00_anim, 0 },
        };
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        for (unsigned i = 0; i < sizeof cand / sizeof cand[0]; i++) {
            if (cand[i].clip >= cand[i].an->clip_count) continue;
            int kf = (int)(cand[i].an->frames[cand[i].an->clips[cand[i].clip].first_frame] & 0xFFFu);
            g_anim_pose_actor = NULL;
            if (re15_skel_compute_pose(cand[i].sk, kf, poses) == 0)
                printf("[posediff] %-38s kf=%3d b9(%d,%d,%d) b13(%d,%d,%d)\n", cand[i].n, kf,
                       (int)poses[9].trans[0], (int)poses[9].trans[1], (int)poses[9].trans[2],
                       (int)poses[13].trans[0], (int)poses[13].trans[1], (int)poses[13].trans[2]);
        }
    }

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

    int weapon = (variant == 'B' || variant == 'E') ? 3 : 1;
    re15_player_set_equipped_weapon(weapon);
    printf("[equip] Waffe=%d (equipped=%d)\n", weapon, re15_player_equipped_weapon());
    if (variant == 'D') pl->hp = 40;

    /* etwas settlen (Idle) */
    for (int f = 0; f < 30; f++) tick_game(0, 0);
    dump_render_view("IDLE", 0);

    int walk = (variant == 'C') ? 0 : 40;
    uint16_t extra = (variant == 'E') ? RE15_PAD_BIT_CROSS : 0;
    g_scd.player_mode = 0;
    int slot = fire_door_to(0x1040u, walk, extra);
    if (slot < 0 || !g_room_change.pending) { printf("FAIL: Tuer 1030->1040 nicht gefeuert\n"); return 1; }
    printf("[queue] room=%04X spawn=(%ld,%ld,%ld) yaw=%d cut=%d\n",
           g_room_change.room_id, (long)g_room_change.x, (long)g_room_change.y,
           (long)g_room_change.z, (int)g_room_change.yaw_4096, g_room_change.target_cut);
    dump_render_view("PREAP", 0);

    /* ===== PHASE 1: Tuer anwenden — exakt die main.c-Reihenfolge ===== */
    if (!re15_room_apply_pending(&rc)) { printf("FAIL: apply 1040\n"); return 1; }
    re15_room_transition_present();
    select_def_bank(0x1040);
    s_frame = 0;
    printf("== ZIELRAUM %04X — Zustand direkt nach apply+present (vor Bild 0) ==\n", g_current_room_id);
    dump_render_view("ENTRY", -1);

    /* ===== PHASE 2: die ersten nlog Bilder im Zielraum ===== */
    for (int t = 0; t < nlog; t++) {
        tick_game(0, 0);
        dump_render_view("DEST", t);
    }
    printf("\nBEFUND (%c): siehe RENDER-Spalte Bild 0..%d (bank/clip/kf) — Sollwert Original: "
           "cmd-0 @0x800318f8 setzt +0x94=1 @0x80031c10, +0x95=0 @0x80031c18, +0x8f=0 @0x80031c20 "
           "und ruft f314(W-Bank 0x800acbc4/0x800acbc8, a2=0, a3=0x200) @0x80031c24.\n", variant, nlog - 1);
    return 0;
}
