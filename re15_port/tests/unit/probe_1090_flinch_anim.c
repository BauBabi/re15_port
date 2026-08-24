/* probe_1090_flinch_anim.c — FINDING 5 (Wiederholungs-Report 2026-08-24):
 * "fehlt noch die Verletzt Animation wenn man in das Feuer reinlaeuft" (ROOM1090).
 *
 * ⛔ WARUM ES EINE ZWEITE SONDE BRAUCHT: probe_1090_flame_touch.c misst den ZUSTAND
 * (pl->motion == 8/9, HP-Abfall, CORE-SE) — und der war seit v0.3.18 richtig. Die Sonde
 * ruft re15_actor_anim_select / re15_compute_actor_kf NIE und kann die GERENDERTE Pose
 * per Konstruktion nicht sehen. Genau die Klasse "Sonde luegt" aus lesson.txt.
 *
 * DIESE Sonde geht durch den ECHTEN Durchlauf (scd_vm_tick -> msg -> cam -> re15_game_step,
 * echter Feuer-Kontakt in ROOM1090, echte Kollision) UND loest in JEDEM Bild die Pose ueber
 * denselben Render-Pfad auf, den platform/pc/main.c benutzt — inklusive des RDT-Animations-
 * blocks (@0x5C), den re15_apply_room_cinematic auf Leons Default-Bank legt.
 *
 * SOLLSEITE (Original, selbst disassembliert):
 *   cmd-2-Handler [2] @0x80035de0 Phase 0: `ori v0,zero,0x8` @0x80035e38 / `sb v0,-13592(at)`
 *     @0x80035e40 = +0x94 = Clip 8 — und posiert @0x80035ea8 `lw a0,DAT_800acad8` /
 *     @0x80035eb0 `lw a1,DAT_800acbc0` = die COMMON-Bank PL00.EMR/PL00.EDD.
 *   cmd-2-Handler [3] @0x80035f64: Clip 9 @0x80035fbc-c4, dieselbe Bank @0x8003602c/@0x80036034.
 *   cmd-2 [0]/[1] (Clip 0xa): @0x80035c28/@0x80035c30 bzw. @0x80035d60/@0x80035d68.
 *   cmd-2 [4]/[5] Knockdown (Clips 0x0b-0x10): @0x800361b8 / @0x80036284 / @0x800363d0 /
 *     @0x80036518 / @0x80036640.
 *   cmd-3 TOD (Clip 7): @0x800367d0.
 * Und die beiden Zeiger haben im GESAMTEN PSX.EXE genau zwei Schreiber, beide im
 * Spieler-Loader FUN_800314b0: @0x80031578 (DAT_800ACAD8 = PL00.EMR) und @0x8003154c
 * (DAT_800ACBC0 = PL00.EDD). Der RBJ-Binder FUN_8001b3f8 repointet nur Entity-Felder
 * (+0x16c/+0x170/...), NIE diese Globals => die Reaktions-Clips sind in JEDEM Raum PL00-Clips.
 *
 * IST-MESSUNG VOR DEM FIX (dieser Harnisch, echte Assets):
 *   ROOM1090 RBJ rec0: 25 Clips, Clip 8 fc=1, Clip 9 fc=1, Clip 0xa fc=1, Clip 7 fc=1
 *   PL00.EDD:          24 Clips, Clip 8 fc=22, Clip 9 fc=22, Clip 0xa fc=20, Clip 7 fc=113
 *   => der 22-Bilder-Flinch rendert als EIN eingefrorener Cutscene-Keyframe.
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
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_damage.h"
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

/* Leons Baenke wie in platform/pc/main.c:
 *   pl00_*  = die unveraenderte PL00-Basis (COMMON, == DAT_800acbc0/acad8)
 *   def_*   = dieselbe Basis NACH dem Raum-RBJ-Overlay (main.c `anim`/`skel`) */
static re15_emd_animation_t s_pl00_anim, s_def_anim, s_scratch_anim;
static re15_emd_skeleton_t  s_pl00_skel, s_def_skel, s_scratch_skel;
static re15_anim_banks_t    s_banks;

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

/* Der Render-Pfad in EINEM Aufruf — exakt die Kette aus platform/pc/main.c:5664/6816. */
typedef struct { const char *bank; int clip; int fc_render; int fc_soll; int kf; } pose_t;
static pose_t resolve_pose(const re15_actor_t *pl)
{
    pose_t r; re15_anim_view_t av;
    re15_actor_anim_select(pl, 1, &s_banks, &av);
    r.clip = (av.clip_override >= 0) ? av.clip_override : (int)pl->motion;
    r.fc_render = (av.anim && r.clip >= 0 && r.clip < av.anim->clip_count)
                ? av.anim->clips[r.clip].frame_count : -1;
    r.fc_soll   = (r.clip >= 0 && r.clip < s_pl00_anim.clip_count)
                ? s_pl00_anim.clips[r.clip].frame_count : -1;
    r.bank = (av.anim == &s_pl00_anim) ? "PL00" : (av.anim == &s_def_anim) ? "RBJ" : "?";
    r.kf = re15_compute_actor_kf(av.anim, av.skel, pl, av.clip_override, pl->anim_frame);
    return r;
}

int main(void)
{
    size_t esz = 0, rsz = 0, sz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    if (!edd || !emr) { printf("SKIP: PL00.EDD/EMR fehlt\n"); return 77; }
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 ||
        re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) != 0) {
        printf("FAIL: PL00-Parse\n"); return 1;
    }
    s_def_anim = s_pl00_anim; s_def_skel = s_pl00_skel;

    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT", &sz);
    if (!raw) { printf("FAIL: ROOM1090.RDT fehlt\n"); return 1; }
    if (re15_rdt_parse(raw, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* Der Raum-RBJ-Overlay, exakt wie main.c ihn beim Raumwechsel legt. */
    CHECK(s_rdt.animation != NULL, "ROOM1090 hat keinen RDT-Animationsblock — Messung sinnlos");
    if (s_rdt.animation)
        re15_apply_room_cinematic(s_rdt.animation, (size_t)s_rdt.animation_size, 0x1090,
                                  &s_pl00_skel, &s_def_skel, &s_def_anim,
                                  NULL, 0, NULL, NULL,
                                  &s_scratch_skel, &s_scratch_anim, NULL);
    printf("[M0] PL00-Basis: %d Clips  c7=%d c8=%d c9=%d c10=%d c11=%d c16=%d\n",
           s_pl00_anim.clip_count,
           s_pl00_anim.clips[7].frame_count,  s_pl00_anim.clips[8].frame_count,
           s_pl00_anim.clips[9].frame_count,  s_pl00_anim.clips[10].frame_count,
           s_pl00_anim.clips[11].frame_count, s_pl00_anim.clips[16].frame_count);
    printf("[M0] def(RBJ) : %d Clips  c7=%d c8=%d c9=%d c10=%d c11=%d c16=%d\n",
           s_def_anim.clip_count,
           s_def_anim.clip_count > 7  ? s_def_anim.clips[7].frame_count  : -1,
           s_def_anim.clip_count > 8  ? s_def_anim.clips[8].frame_count  : -1,
           s_def_anim.clip_count > 9  ? s_def_anim.clips[9].frame_count  : -1,
           s_def_anim.clip_count > 10 ? s_def_anim.clips[10].frame_count : -1,
           s_def_anim.clip_count > 11 ? s_def_anim.clips[11].frame_count : -1,
           s_def_anim.clip_count > 16 ? s_def_anim.clips[16].frame_count : -1);

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    s_ctx.pl00_skel = &s_pl00_skel; s_ctx.pl00_anim = &s_pl00_anim;
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
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);

    memset(&s_banks, 0, sizeof s_banks);
    s_banks.def_skel = &s_def_skel;  s_banks.def_anim = &s_def_anim;
    s_banks.pl00_skel = &s_pl00_skel; s_banks.pl00_anim = &s_pl00_anim; s_banks.pl00_ok = 1;
    s_banks.w01_ok = 0;              /* die 105/100/200er-Sentinels aus dem Weg */

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
    if (fire < 0) { printf("probe_1090_flinch_anim: %d FAILURES\n", s_fail); return 1; }
    re15_actor_t *fe = &g_actors[fire];

    /* 900 Einheiten vor die Flamme, Blick auf die Flamme, dann hineinlaufen. */
    {
        double ang = atan2((double)-(fe->z - pl->z), (double)(fe->x - pl->x))
                   * 4096.0 / 6.283185307179586;
        int yaw = ((int)(ang + 0.5)) & 0xfff;
        pl->x = fe->x - (int32_t)((cos(ang * 6.283185307179586 / 4096.0)) * 900.0);
        pl->z = fe->z + (int32_t)((sin(ang * 6.283185307179586 / 4096.0)) * 900.0);
        pl->rot_y = (int16_t)yaw;
    }

    /* --- M1: FEUER-FLINCH ueber den echten Durchlauf --- */
    int flinch_frames = 0, flinch_rbj = 0, flinch_bad_fc = 0, first = -1;
    int seen_mo = -1, kf_first = -1, kf_last = -1, kf_distinct = 0, kf_prev = -0x7fffffff;
    printf("[M1] Bild motion clip fc_render fc_soll frame kf bank\n");
    for (int f = 0; f < 300; f++) {
        frame(RE15_PAD_BIT_UP, 0);
        int mo = (int)pl->motion;
        if (!(mo == 8 || mo == 9 || mo == 10)) continue;      /* nur die cmd-2-Flinch-Familie */
        pose_t p = resolve_pose(pl);
        if (first < 0) { first = f; seen_mo = mo; kf_first = p.kf; }
        if (p.kf != kf_prev) { kf_distinct++; kf_prev = p.kf; }
        kf_last = p.kf;
        flinch_frames++;
        if (strcmp(p.bank, "PL00") != 0) flinch_rbj++;
        if (p.fc_render != p.fc_soll) flinch_bad_fc++;
        if (flinch_frames <= 24)
            printf("     %4d   %3d  %3d      %3d     %3d   %3d %4d %s\n",
                   f, mo, p.clip, p.fc_render, p.fc_soll, (int)pl->anim_frame, p.kf, p.bank);
        if (flinch_frames >= 40) break;
    }
    printf("[M1] Flinch-Bilder=%d (mo=%d ab f%d), aus der RBJ-Bank=%d, fc-Abweichungen=%d, "
           "verschiedene Keyframes=%d (%d..%d)\n",
           flinch_frames, seen_mo, first, flinch_rbj, flinch_bad_fc, kf_distinct,
           kf_first, kf_last);
    CHECK(flinch_frames > 0, "kein Flinch im echten Feuer-Durchlauf erreicht");
    CHECK(flinch_rbj == 0,
          "%d Flinch-Bilder posierten aus der RAUM-RBJ-Bank; das Original nimmt "
          "@0x80035ea8/@0x80035eb0 (Clip 8) bzw. @0x8003602c/@0x80036034 (Clip 9) immer "
          "DAT_800acad8/DAT_800acbc0 = PL00 (einzige Schreiber @0x80031578/@0x8003154c)",
          flinch_rbj);
    CHECK(flinch_bad_fc == 0,
          "%d Flinch-Bilder trugen die falsche Cliplaenge (RBJ-Clip statt PL00-Clip)",
          flinch_bad_fc);
    CHECK(kf_distinct > 1,
          "die Flinch-Pose war ueber %d Bilder ein EINZIGER eingefrorener Keyframe (%d) — "
          "genau der Nutzer-Report", flinch_frames, kf_first);

    /* --- M2: KNOCKDOWN (cmd-2 [4]/[5], Clips 0x0b-0x10) im selben Raum --- */
    re15_player_cmd_reset();
    pl->hp = 100; pl->motion = 0; pl->anim_frame = 0;
    re15_player_knockdown_begin(0);
    int kd_frames = 0, kd_rbj = 0, kd_bad_fc = 0;
    for (int f = 0; f < 120 && re15_player_knockdown_active(); f++) {
        frame(0, 0);
        int mo = (int)pl->motion;
        if (mo < 0x0b || mo > 0x10) continue;
        pose_t p = resolve_pose(pl);
        kd_frames++;
        if (strcmp(p.bank, "PL00") != 0) kd_rbj++;
        if (p.fc_render != p.fc_soll) kd_bad_fc++;
    }
    printf("[M2] Knockdown-Bilder=%d, aus der RBJ-Bank=%d, fc-Abweichungen=%d\n",
           kd_frames, kd_rbj, kd_bad_fc);
    CHECK(kd_frames > 0, "kein Knockdown-Clip erreicht");
    CHECK(kd_rbj == 0, "%d Knockdown-Bilder aus der RBJ-Bank (Original @0x800361b8/"
                       "@0x80036284/@0x800363d0/@0x80036518/@0x80036640 = PL00)", kd_rbj);
    CHECK(kd_bad_fc == 0, "%d Knockdown-Bilder mit falscher Cliplaenge", kd_bad_fc);

    /* --- M3: TOD (cmd 3, Clip 7 @0x80036778-80, posiert @0x800367d0) --- */
    re15_player_cmd_reset();
    pl->hp = -1; pl->motion = 0; pl->anim_frame = 0;
    re15_player_death_cmd3();
    int d_frames = 0, d_rbj = 0, d_bad_fc = 0;
    for (int f = 0; f < 130; f++) {
        frame(0, 0);
        if ((int)pl->motion != 7) continue;
        pose_t p = resolve_pose(pl);
        d_frames++;
        if (strcmp(p.bank, "PL00") != 0) d_rbj++;
        if (p.fc_render != p.fc_soll) d_bad_fc++;
    }
    printf("[M3] Todes-Bilder=%d, aus der RBJ-Bank=%d, fc-Abweichungen=%d\n",
           d_frames, d_rbj, d_bad_fc);
    CHECK(d_frames > 0, "kein Todes-Clip (cmd 3, +0x94 = 7 @0x80036778-80) erreicht");
    CHECK(d_rbj == 0, "%d Todes-Bilder aus der RBJ-Bank (Original @0x800367d0 = PL00)", d_rbj);
    CHECK(d_bad_fc == 0, "%d Todes-Bilder mit falscher Cliplaenge", d_bad_fc);

    free(raw); free(edd); free(emr);
    if (s_fail == 0) printf("probe_1090_flinch_anim: OK\n");
    else             printf("probe_1090_flinch_anim: %d FAILURES\n", s_fail);
    return s_fail ? 1 : 0;
}
