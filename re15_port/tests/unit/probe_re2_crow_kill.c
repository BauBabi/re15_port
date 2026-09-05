/* probe_re2_crow_kill.c — MESSUNG (Diagnose 2026-09-05, Symptom 3): Nutzer-Report RE2-Modus
 * "Wenn ich durch Kraehen sterbe - bekomme ich keine Sterbeanimation, sondern bleibe
 * einfach stehen."
 *
 * ORIGINAL-Kette (EMOVL21_S0.BIN + RE2-PSX.EXE, Diagnose-Dossier diag_crow_kill_noanim.md,
 * dort selbst disassembliert):
 *   GRAB-Peck P1: jal 0x800401d4(5, +0x218) @0x8010265C-64; ret 2 (Tod) -> Broadcast 128
 *     @0x80102680-84 -> grab_release @0x80102848.
 *   grab_release: geclaimt -> PL+0x6 := 3 UNBEDINGT (sb 3,-1026(at) = 0x800cfbfe
 *     @0x80102890-98).
 *   Victim-Hook Ph3 @0x80104804: PL-HP < 0 -> PL(3,0,0,0) (sb 3,0x800cfbfc @0x80104818-38,
 *     KEIN Release-Clip); cmd 3 = RE2-Spieler-Todeshandler FUN_8003fee4 (Tabelle
 *     @0x800a4030[3]) = Todes-Kollaps ueber die normale Spielerbank + Se_on 0x04030001.
 *   Port-Aequivalent des cmd-3-Handlers: re15_player_death_cmd3() (PL00 Clip 7, 113 F).
 *
 * GEMESSEN WIRD: was der PORT unter RE15_AI_FLAVOR_RE2 tut, wenn der Kraehen-GRAB-Peck den
 * Spieler toetet — pl->motion/anim_frame/victim_state/grabbed + Game-Over-Praesentation
 * ueber 200 Ticks nach hp<0. Byte-Treue-Erwartung: motion springt auf 7 (Todes-Clip) und
 * laeuft 113 Frames. IST vor dem Fix (Diagnose): motion==2 (Kraehen-Release-Clip, 20 F),
 * danach motion==200 (Idle-Sentinel) dauerhaft — Leon bleibt stehen. */
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
#include "re15_ai_flavor.h"
#include "re2_ems.h"

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
    if (s_pl00_ok)
        printf("PL00.EDD: clips=%d  Clip7=%d Frames (Todes-Clip; Soll 113)\n",
               s_pl00_anim.clip_count, s_pl00_anim.clips[7].frame_count);
    else
        printf("!! PL00.EDD/EMR nicht ladbar\n");
}

/* RE2-EM021-Bank (Kraehe) inkl. Victim-Paar-3 laden — der Weg des B-Laufs von
 * test_re2_room10c0_ab.c (re2_ems_load_bank; EMS bleibt resident, Bank aliast hinein). */
static uint8_t *s_re2_ems = NULL;
static int load_re2_crow_bank(void)
{
    if (!s_re2_ems) {
        char p[600]; size_t sz = 0;
        snprintf(p, sizeof p, "%s/../RE2/CDEMD0.EMS", s_base);
        s_re2_ems = read_file(p, &sz);
        if (!s_re2_ems) { printf("!! %s nicht lesbar\n", p); return 0; }
        re15_enemy_bank_t *eb = re15_enemy_find(0x21);
        if (!eb) eb = re15_enemy_alloc(0x21);
        if (!eb) return 0;
        int rc = re2_ems_load_bank(s_re2_ems, sz, 0x21, eb, NULL);
        printf("RE2 EM021: rc=%d clips=%d victim_ok=%d\n",
               rc, eb->ok ? eb->anim.clip_count : -1, eb->victim_ok);
        return rc == 0 && eb->victim_ok;
    }
    return 1;
}

static void log_row(int f, const re15_actor_t *pl, const re15_actor_t *crow, const char *tag)
{
    printf("  t=%4d %-6s hp=%4d motion=0x%02x(%3d) frame=%3d victim=%d grabbed=%d | "
           "crow st=%d sub=%d fl=%04x | pool=%d cam=%d flyin=%d go=%d\n",
           f, tag, (int)pl->hp, pl->motion, pl->motion, pl->anim_frame,
           re15_player_victim_state(), re15_player_is_grabbed(),
           crow->state, crow->sub_state_1, crow->re2c_flags22a,
           g_death_pool, g_death_cam, g_death_flyin, g_gameover_active);
}

/* Kraehe deterministisch in den geclaimten GRAB setzen (Muster test_re2_crow_ai.c:173-175):
 * Mutex an den Claimer, Zustand (1,14,0), Flags LOS+Claim, Position ueber dem Spieler. */
static void force_grab(re15_actor_t *crow, const re15_actor_t *pl)
{
    g_re2_room_gflags |= 0x1u;
    crow->state = 1; crow->sub_state_1 = 14; crow->sub_state_2 = 0; crow->sub_state_3 = 0;
    crow->re2c_flags22a = 0x2u | 0x4u;
    crow->x = pl->x + 100; crow->y = pl->y - 1500; crow->z = pl->z + 100;
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
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    if (!load_re2_crow_bank()) { fprintf(stderr, "FAIL: RE2-Bank\n"); return 1; }

    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150;
    pl->hp = 4;                       /* 1. Peck (5, mode 1) -> One-Save hp=0+Latch;
                                       * 2. Peck (5, mode 0) -> ret 2 = TOD */
    pl->rot_y = 0; pl->hit_react = 0; pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 0;
    memcpy((void *)&g_room_rdt, &rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(4);

    re15_actor_t *crow = &g_actors[1];
    memset(crow, 0, sizeof *crow);
    crow->active = 1; crow->type = 0x21; crow->hp = 10;
    crow->hit_radius_min = 96; crow->hit_height = 512;
    force_grab(crow, pl);

    re15_camera_view_t cam; memset(&cam, 0, sizeof cam);
    re15_game_ctx_t ctx; memset(&ctx, 0, sizeof ctx);
    ctx.rdt = &rdt; ctx.rdt_ok = 1; ctx.cam_view = &cam; ctx.active_cut = 0;
    if (s_pl00_ok) { ctx.pl00_skel = &s_pl00_skel; ctx.pl00_anim = &s_pl00_anim; }

    printf("\n===== RE2-Kraehen-GRAB-Kill (hp=4 -> One-Save -> Tod am 2. Peck) =====\n");
    int f;
    for (f = 0; f < 1200 && pl->hp >= 0; f++) {
        /* Zwischen den Peck-Zyklen (Release nach One-Save wirft die Kraehe in Sub 4)
         * den Grab erneut erzwingen, solange der Spieler lebt. */
        if (!(crow->state == 1 && crow->sub_state_1 == 14))
            force_grab(crow, pl);
        re15_game_step(&ctx);
        if ((f % 25) == 0) log_row(f, pl, crow, "alive");
    }
    if (pl->hp >= 0) {
        printf("  !! kein Tod in 1200 Ticks (hp=%d) — Messaufbau defekt\n", (int)pl->hp);
        return 1;
    }
    log_row(f, pl, crow, "KILL");

    int motion_changes = 0, saw7 = 0, maxframe = -1;
    uint8_t prev = pl->motion;
    for (int t = 0; t < 200; t++) {
        re15_game_step(&ctx);
        if (pl->motion != prev) { motion_changes++; prev = pl->motion; }
        if (pl->motion == 7) { saw7 = 1; if (pl->anim_frame > maxframe) maxframe = pl->anim_frame; }
        if (t < 30 || (t % 20) == 0 || t > 190) log_row(t, pl, crow, "dead");
    }
    printf("  == RE2-CROW-KILL: motion-Wechsel=%d, Todes-Clip motion==7 gesehen=%s "
           "(max frame %d), Endmotion=0x%02x frame=%d victim=%d\n",
           motion_changes, saw7 ? "JA" : "NEIN", maxframe,
           pl->motion, pl->anim_frame, re15_player_victim_state());
    printf("  SOLL (byte-true): motion==7 laeuft 113 Frames (cmd-3-Analog; "
           "PL(3,0,0,0) @0x80104818-38 + FUN_8003fee4)\n");
    free(data);
    return 0;
}
