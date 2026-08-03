/* probe_crow_flinch.c — DIAGNOSE (kein ctest): Nutzer-Report 2026-08-03 nach c9a19140
 * ("Kraehen greifen jetzt an"): Leons REAKTIONS-Animation beim Kraehen-Treffer FEHLT.
 *
 * Original (analysis/crow_victim_anim.md): der Dive-Hit schreibt cmd 2 + aca59=facing+2
 * (@0x80113b00/28) -> EXE-Standard-Flinch [2]/[3] = Clips 8/9 aus Paar A (PLD).
 * Port-Erwartung: HP-Drop-Detector in re15_game_step (game_step_common.c:351ff) setzt
 * pl->motion=8/9 fuer 22 Ticks + Knockback 200.
 *
 * Diese Sonde faehrt den VOLLEN re15_game_step (nicht nur run_all) unter Live-Bedingungen
 * (ROOM1170 Re-Entry, Band 4, echte RDT) und loggt pl->motion/hp/hit_react um jeden
 * Dive-/Strike-/Grab-Kontakt herum. Variante B wiederholt das mit gehaltener AIM-Taste. */
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
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_camera.h"
#include "re15_game_step.h"

extern int re15_player_victim_state(void);   /* enemy_ai_common.c: 0 none / 1 struggle / 3 release */

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

/* EM021-Victim-Bank aus CDEMD0.EMS laden (blob 8 = Typ 0x21), damit die Kraehen-Opfer-FSM
 * im Engine-only-Harness greift (live laedt das die Plattform in pc_load_enemy). Verifiziert
 * nebenbei den F9-Geometrie-Fix: kf_size muss aus dem dir[6]-Pool-Header kommen (80), nicht
 * aus dir[2] (72), und die Clips 0/1/2 muessen 14/36/20 Frames haben (EM021-EDD @blob+0x49d4). */
static uint8_t *s_em21_blob = NULL;
static int load_crow_victim_bank(const char *base)
{
    re15_enemy_bank_t *eb = re15_enemy_find(0x21);
    if (!eb) eb = re15_enemy_alloc(0x21);
    if (!eb) return 0;
    if (!s_em21_blob) {
        char path[600]; size_t ems_size = 0;
        snprintf(path, sizeof path, "%s/EMD/CDEMD0.EMS", base);
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
    eb->ok = 1; eb->buf = NULL;   /* blob bleibt statisch resident (kein free durch enemy_reset) */
    printf("EM021 victim bank: ok=%d clips=%d kf_size=%d (soll 80; dir[2]=72 waere der F9-Bug)"
           " fc[0..2]=%d/%d/%d (soll 14/36/20)\n",
           eb->victim_ok, eb->anim_victim.clip_count, eb->skel_victim.keyframe_size_bytes,
           eb->anim_victim.clip_count > 0 ? eb->anim_victim.clips[0].frame_count : -1,
           eb->anim_victim.clip_count > 1 ? eb->anim_victim.clips[1].frame_count : -1,
           eb->anim_victim.clip_count > 2 ? eb->anim_victim.clips[2].frame_count : -1);
    return eb->victim_ok;
}

static const char *s_asset_base = NULL;

static void run_flinch_variant(const re15_rdt_t *rdt, uint16_t held_pad, const char *label)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_game_flag_set(3, 125, 1);                 /* Re-Entry-Zweig: 7 Pad-Kraehen */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150; pl->hp = 100;
    pl->rot_y = 0; pl->hit_react = 0; pl->motion = 0; pl->anim_frame = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(4);                    /* wie live: band_from_y(-7200)=4 */
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);
    g_scd.combat_active = 1;
    if (s_asset_base) load_crow_victim_bank(s_asset_base);

    re15_camera_view_t cam; memset(&cam, 0, sizeof cam);
    re15_game_ctx_t ctx; memset(&ctx, 0, sizeof ctx);
    ctx.rdt = rdt; ctx.rdt_ok = 1; ctx.cam_view = &cam; ctx.active_cut = 0;
    ctx.pad_current = held_pad; ctx.pad_pressed = 0;

    int16_t prev_hp = pl->hp;
    int drops = 0, flinch_starts = 0, flinch_ticks = 0, grab_ticks = 0, victim_ticks = 0;
    int prev_vs = 0;
    uint8_t motions_seen[256]; memset(motions_seen, 0, sizeof motions_seen);
    uint8_t victim_motions[8]; memset(victim_motions, 0, sizeof victim_motions);
    printf("== Variante %s (pad_held=0x%04x) ==\n", label, held_pad);
    for (int f = 0; f < 3600 && pl->hp > 0; f++) {
        uint8_t motion_before = pl->motion;
        re15_game_step(&ctx);
        motions_seen[pl->motion] = 1;
        if (re15_player_is_grabbed()) grab_ticks++;
        {   /* Kraehen-Opfer-FSM: Leon muss waehrend des Grabs die EM021-victim Clips 0/1/2
             * spielen (Hook A [0x21]; motion = der Bank-Clip, Render-Override main.c) */
            int vs = re15_player_victim_state();
            if (vs) { victim_ticks++; if (pl->motion < 8) victim_motions[pl->motion & 7] = 1; }
            if (vs != prev_vs)
                printf("  t=%4d VICTIM %d->%d motion=0x%02x frame=%d frac=%d\n",
                       f, prev_vs, vs, pl->motion, pl->anim_frame, pl->anim_frac);
            prev_vs = vs;
        }
        if (pl->motion == 0x08 || pl->motion == 0x09 || pl->motion == 0x0a) {
            flinch_ticks++;
            if (motion_before != pl->motion) {
                flinch_starts++;
                printf("  t=%4d FLINCH-Start: motion=0x%02x hp=%d hit_react=%d\n",
                       f, pl->motion, (int)pl->hp, pl->hit_react);
            }
        }
        if (pl->hp < prev_hp) {
            drops++;
            if (drops <= 8)
                printf("  t=%4d HP-Drop %d->%d  motion=0x%02x hit_react=%d grabbed=%d\n",
                       f, (int)prev_hp, (int)pl->hp, pl->motion, pl->hit_react,
                       re15_player_is_grabbed());
            prev_hp = pl->hp;
        }
    }
    printf("  Ende: hp=%d  HP-Drops=%d  Flinch-Starts=%d  Flinch-Ticks=%d  Grab-Ticks=%d  Victim-Ticks=%d\n",
           (int)pl->hp, drops, flinch_starts, flinch_ticks, grab_ticks, victim_ticks);
    printf("  gesehene motion-Werte:");
    for (int m = 0; m < 256; m++) if (motions_seen[m]) printf(" 0x%02x", m);
    printf("\n  Victim-Clips gesehen:");
    for (int m = 0; m < 8; m++) if (victim_motions[m]) printf(" %d", m);
    printf("  (soll: 0 Intro, 1 Halte-Loop, 2 Release)\n");
    g_room_rdt_ok = 0; re15_collision_reset_band();
}

/* Variante C: DIVE ERZWINGEN — die armed 1170-Kraehen (grid 0x00) committen per steer[9]
 * IMMER in sub 12 (Grapple, @0x8011335c-68 grid<0x80 -> 0xc); der Dive (sub 10/11) laeuft
 * nur fuer grid>=0x80-Kraehen (in 1170 nur nach Re-Arm-Broadcast erreichbar). Hier wird die
 * armed Kraehe direkt in sub 11 gesetzt (ueber dem Spieler, vert_err=2000 in [1,3599]) und
 * gemessen, ob der HP-Drop-Detector den Flinch (motion 8/9) ausloest. */
static void run_force_dive(const re15_rdt_t *rdt)
{
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_game_flag_set(3, 125, 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150; pl->hp = 100;
    pl->rot_y = 0; pl->hit_react = 0; pl->motion = 0; pl->anim_frame = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(4);
    scd_register_room_events(rdt);
    scd_room_reenter((re15_rdt_t *)rdt, 0, 0, 0);
    g_scd.combat_active = 1;

    re15_camera_view_t cam; memset(&cam, 0, sizeof cam);
    re15_game_ctx_t ctx; memset(&ctx, 0, sizeof ctx);
    ctx.rdt = rdt; ctx.rdt_ok = 1; ctx.cam_view = &cam; ctx.active_cut = 0;

    /* settle: Kraehen spawnen + State 1 erreichen */
    for (int f = 0; f < 30; f++) re15_game_step(&ctx);

    re15_actor_t *crow = NULL;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (e->active && e->type == 0x21 && !(e->grid_id & 0x10)) { crow = e; break; }
    }
    if (!crow) { printf("== Variante C: keine armed Kraehe gefunden\n"); return; }
    /* Kraehe direkt ueber den Spieler, Dive-Lane sub 11 */
    crow->x = pl->x + 300; crow->y = pl->y - 2000; crow->z = pl->z + 300;
    crow->state = 1; crow->sub_state_1 = 11; crow->sub_state_2 = 0;
    crow->hit_react = 0;
    printf("== Variante C: Dive erzwungen (crow slot=%d grid=0x%02x vert_err soll=2000) ==\n",
           (int)(crow - g_actors), crow->grid_id);
    int16_t prev_hp = pl->hp;
    for (int f = 0; f < 240; f++) {
        /* Dive-Phase 2 hart pinnen, bis der Connect feuert (dist<600, vert in [1,3599]) */
        if (pl->hp == 100 && crow->state == 1) {
            crow->x = pl->x + 100; crow->y = pl->y - 1500; crow->z = pl->z + 100;
            crow->sub_state_1 = 11; crow->sub_state_2 = 2; crow->crow_timer = 20;
        }
        re15_game_step(&ctx);
        {
            extern int re15_player_aim_active(void);
            if ((f % 10) == 0 || pl->hp != prev_hp || pl->motion == 8 || pl->motion == 9 || pl->motion == 0x0a)
                printf("  t=%3d hp=%d motion=0x%02x frame=%d hit_react=%d grabbed=%d aim=%d crow-ss1=%d/%d vert=%d dist=%u\n",
                       f, (int)pl->hp, pl->motion, pl->anim_frame, pl->hit_react,
                       re15_player_is_grabbed(), re15_player_aim_active(),
                       crow->sub_state_1, crow->sub_state_2,
                       (int)crow->crow_vert_err, (unsigned)crow->crow_dist);
        }
        prev_hp = pl->hp;
    }
    g_room_rdt_ok = 0; re15_collision_reset_band();
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    s_asset_base = base;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    size_t size = 0;
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    re15_actor_init();
    run_force_dive(&rdt);          /* C zuerst: sauberer Prozesszustand (kein Aim-Latch aus B) */
    re15_actor_init();
    run_flinch_variant(&rdt, 0x0000, "A: neutral (kein Input)");
    re15_actor_init();
    run_flinch_variant(&rdt, RE15_PAD_BIT_R1, "B: AIM gehalten (R1)");
    re15_actor_init();
    run_force_dive(&rdt);
    free(data);
    return 0;
}
