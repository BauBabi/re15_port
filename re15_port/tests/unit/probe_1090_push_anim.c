/* probe_1090_push_anim.c — MESSSONDE zum Nutzer-Befund (A) "die Animation von Leon beim
 * Kistenschieben ist noch falsch" (ROOM1090).
 *
 * Gemessen wird NICHT der FSM-Zustand (der war schon richtig), sondern die GERENDERTE Pose:
 * welche Bank + welcher Clip-Index + welche Cliplaenge kommen aus dem Render-Pfad
 * (re15_actor_anim_select -> re15_compute_actor_kf), wenn ROOM1090 wie im Spiel geladen ist
 * (RDT-Animationsblock @0x5C ueber re15_apply_room_cinematic auf Leons Default-Bank gelegt,
 * genau wie platform/pc/main.c es tut).
 *
 * SOLLSEITE (Original): der Schiebe-Handler LAB_80035810 posiert IMMER aus der COMMON-Bank
 *   @0x800358bc `lw a0,DAT_800acad8` / @0x800358c4 `lw a1,DAT_800acbc0`   (Fall 1)
 *   @0x80035960 / @0x80035968                                            (Fall 3)
 *   @0x80035a2c / @0x80035a34                                            (Fall 5)
 *   @0x80035a80 / @0x80035a88                                            (Fall 6)
 * und beide Zeiger werden im GESAMTEN PSX.EXE genau EINMAL geschrieben:
 *   @0x8003154c `sw v0,-0x3440(at)=>DAT_800acbc0`   v0 = PLD-Directory[0] = PL00.EDD
 *   @0x80031578 `sw v0,-0x3528(at)=>DAT_800acad8`   v0 = PLD-Directory[1] = PL00.EMR
 * => Ein Raum-RBJ kann Leons COMMON-Bank NICHT umbiegen; Clip 0x11/0x12 sind IMMER
 *    PL00.EDD-Clips (10 bzw. 25 Frames).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_player.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_collision.h"
#include "re15_anim_select.h"
#include "re15_enemy.h"      /* re15_apply_room_cinematic — der Raum-RBJ-Overlay wie in main.c */

extern scd_vm_t g_scd;

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { printf("FAIL: "); printf(__VA_ARGS__); \
                              printf("\n"); g_fail = 1; } } while (0)

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

/* Leons Baenke, so wie main.c sie haelt:
 *   pl00_*  = die unveraenderte PL00-Basis (COMMON, == DAT_800acbc0/acad8)
 *   def_*   = dieselbe Basis, aber NACH dem Raum-RBJ-Overlay (main.c `anim`/`skel`) */
static re15_emd_animation_t s_pl00_anim, s_def_anim, s_scratch_anim;
static re15_emd_skeleton_t  s_pl00_skel, s_def_skel, s_scratch_skel;

int main(void)
{
    const char *base = RE15_ASSET_PSX_DIR;
    char p[600];
    size_t esz = 0, rsz = 0;
    snprintf(p, sizeof p, "%s/PLD/PL00.EDD", base);
    uint8_t *edd = read_file(p, &esz);
    snprintf(p, sizeof p, "%s/PLD/PL00.EMR", base);
    uint8_t *emr = read_file(p, &rsz);
    if (!edd || !emr) { printf("SKIP: PL00.EDD/EMR fehlt\n"); return 77; }
    if (re15_emd_parse_animation(edd, esz, &s_pl00_anim) != 0 ||
        re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) != 0) {
        printf("FAIL: PL00-Parse\n"); return 1;
    }
    s_def_anim = s_pl00_anim; s_def_skel = s_pl00_skel;

    char rp[600];
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM1090.RDT", base);
    size_t sz = 0;
    uint8_t *raw = read_file(rp, &sz);
    if (!raw) { printf("SKIP: %s fehlt\n", rp); return 77; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(raw, sz, &rdt) < 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    /* --- M0: hat ROOM1090 einen Animationsblock, und wie sieht seine Clip-Tabelle aus? --- */
    printf("[M0] ROOM1090 RDT-Animationsblock (@0x5C): %s, %d Bytes\n",
           rdt.animation ? "VORHANDEN" : "keiner", rdt.animation_size);
    CHECK(rdt.animation != NULL, "ROOM1090 hat keinen RDT-Animationsblock — Messung sinnlos");
    if (!rdt.animation) return 1;

    /* Exakt der Aufruf aus main.c (Raumwechsel-Pfad): Leons def_* wird VOLL ERSETZT. */
    re15_apply_room_cinematic(rdt.animation, (size_t)rdt.animation_size, 0x1090,
                              &s_pl00_skel, &s_def_skel, &s_def_anim,
                              NULL, 0, NULL, NULL,
                              &s_scratch_skel, &s_scratch_anim, NULL);
    printf("[M0] PL00-Basis : %d Clips, Clip 0x11 fc=%d, Clip 0x12 fc=%d\n",
           s_pl00_anim.clip_count,
           s_pl00_anim.clip_count > 0x12 ? s_pl00_anim.clips[0x11].frame_count : -1,
           s_pl00_anim.clip_count > 0x12 ? s_pl00_anim.clips[0x12].frame_count : -1);
    printf("[M0] def (RBJ)  : %d Clips, Clip 0x11 fc=%d, Clip 0x12 fc=%d\n",
           s_def_anim.clip_count,
           s_def_anim.clip_count > 0x12 ? s_def_anim.clips[0x11].frame_count : -1,
           s_def_anim.clip_count > 0x12 ? s_def_anim.clips[0x12].frame_count : -1);

    /* --- Raum hochfahren wie test_1090_push_pin --- */
    re15_actor_init();
    scd_vm_init();
    re15_aot_init();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_player_set_pl00_banks(&s_pl00_skel, &s_pl00_anim);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    int box = -1;
    for (int i = 0; i < (int)g_scd.prop_count; i++)
        if (g_scd.props[i].active && g_scd.props[i].obj_type == 4 && g_scd.props[i].band == 1) box = i;
    if (box < 0) { printf("FAIL: keine Typ-4-Kiste\n"); return 1; }

    {   /* buendig vor der Kiste, Richtung +X (der einzige freie Schiebe-Standort) */
        int32_t c = re15_cos_q12(0), s = re15_sin_q12(0);
        int32_t dd = 1350;
        pl->x = g_scd.props[box].x + g_scd.props[box].box_cx - (int32_t)((c * dd) >> 12);
        pl->z = g_scd.props[box].z + g_scd.props[box].box_cz - (int32_t)((-s * dd) >> 12);
        pl->y = g_scd.props[box].y;
        pl->rot_y = 0;
        pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 0; pl->anim_flags = 0;
        re15_collision_set_band((int)g_scd.props[box].band);
        re15_player_push_reset();
        re15_prop_push_reset();
    }

    /* --- M1: 60 Bilder schieben, in JEDEM Bild die GERENDERTE Pose aufloesen --- */
    re15_anim_banks_t banks = {0};
    banks.def_mesh = NULL; banks.def_skel = &s_def_skel; banks.def_anim = &s_def_anim;
    banks.w01_skel = NULL; banks.w01_anim = NULL; banks.w01_ok = 0;   /* Sentinels aus dem Weg */
    banks.pl00_skel = &s_pl00_skel; banks.pl00_anim = &s_pl00_anim; banks.pl00_ok = 1;

    int seen_rbj_bank = 0, seen_pl00_bank = 0;
    int fc_mismatch = 0;
    printf("[M1] Bild phase motion clip fc_render fc_soll frame slot bank\n");
    for (int f = 0; f < 60; f++) {
        int32_t ox = pl->x, oz = pl->z;
        re15_player_tick(NULL, RE15_PAD_BIT_UP);
        int32_t nx = pl->x, nz = pl->z;
        re15_collision_constrain(&rdt, ox, oz, &nx, &nz);
        pl->x = nx; pl->z = nz;
        re15_prop_push_tick(&rdt, RE15_PAD_BIT_UP);
        re15_collision_objects(&nx, &nz);
        pl->x = nx; pl->z = nz;

        int ph = re15_player_push_phase();
        if (ph < 0) continue;

        re15_anim_view_t av;
        re15_actor_anim_select(pl, 1, &banks, &av);
        int clip = (av.clip_override >= 0) ? av.clip_override : (int)pl->motion;
        int fc_render = (clip >= 0 && clip < av.anim->clip_count)
                      ? av.anim->clips[clip].frame_count : -1;
        int fc_soll   = (clip >= 0 && clip < s_pl00_anim.clip_count)
                      ? s_pl00_anim.clips[clip].frame_count : -1;
        int slot = re15_actor_playback_slot(pl, pl->anim_frame,
                                            fc_render > 0 ? fc_render : 1);
        const char *bank = (av.anim == &s_pl00_anim) ? "PL00" :
                           (av.anim == &s_def_anim)  ? "RBJ"  : "?";
        if (av.anim == &s_def_anim)  seen_rbj_bank = 1;
        if (av.anim == &s_pl00_anim) seen_pl00_bank = 1;
        if (fc_render != fc_soll) fc_mismatch++;
        if (f < 45)
            printf("      %3d  %d    %3d   %3d   %3d      %3d   %3d  %3d  %s\n",
                   f, ph, (int)pl->motion, clip, fc_render, fc_soll,
                   (int)pl->anim_frame, slot, bank);
    }
    printf("[M1] Bank RBJ gesehen=%d, Bank PL00 gesehen=%d, fc-Abweichungen=%d\n",
           seen_rbj_bank, seen_pl00_bank, fc_mismatch);

    /* --- SOLL: die Schiebe-Pose kommt IMMER aus der COMMON-Bank (PL00) --- */
    CHECK(seen_rbj_bank == 0,
          "der Schiebe-Substate posiert aus der RAUM-RBJ-Bank; das Original nimmt "
          "@0x800358c4/@0x80035968/@0x80035a34/@0x80035a88 immer DAT_800acbc0 = PL00.EDD "
          "(einziger Schreiber @0x8003154c)");
    CHECK(fc_mismatch == 0,
          "%d Bilder posierten einen Clip mit falscher Laenge (RBJ-Clip statt PL00-Clip)",
          fc_mismatch);
    CHECK(seen_pl00_bank == 1, "die Schiebe-Pose kam in keinem Bild aus PL00");

    free(raw); free(edd); free(emr);
    printf(g_fail ? "SONDE 1090-push-anim: ABWEICHUNG\n" : "SONDE 1090-push-anim: OK\n");
    return g_fail;
}
