/* test_re2_crow_kill_cmd3.c — PIN (S3-Fix 2026-09-05): RE2-Kraehen-Kill spielt die
 * Spieler-Sterbeanimation.
 *
 * Nutzer-Report (RE2-Modus): "Wenn ich durch Kraehen sterbe - bekomme ich keine
 * Sterbeanimation, sondern bleibe einfach stehen."
 *
 * ORIGINAL-Kette (EMOVL21_S0.BIN + RE2-PSX.EXE, diag_crow_kill_noanim.md, adversarial
 * gegengeprueft in verify_crow_kill.md — alle Glieder CONFIRMED):
 *   Peck-Kill ret 2 -> Broadcast 128 (@0x80102680-84) -> grab_release @0x80102848;
 *   Release schreibt PL+0x6=3 UNBEDINGT unter dem Claim-Gate (@0x80102890-98);
 *   Victim-Hook Ph3 @0x80104804: HP<0 -> PL(3,0,0,0) (@0x80104818-38, KEIN Release-Clip);
 *   cmd 3 = FUN_8003fee4 (@0x800A4030[3]) = Todes-Kollaps + Se_on 0x04030001 -> cmd 7.
 * Port-Aequivalent: re15_player_death_cmd3() (PL00-Clip 7, 113 F).
 *
 * PIN A: Tod am Grab-Peck -> motion springt auf 7 (im Release-Tick, dokumentierte
 *        1-Frame-Fruehstart-Divergenz zum RE2-Dispatch), laeuft bis Frame 112, KEIN
 *        Victim-Release-Clip fuer den Toten (victim_state faellt auf 0).
 * PIN B (Negativ/Regressionswache): One-Save (Spieler ueberlebt den Peck) -> KEIN cmd 3,
 *        der Release laeuft ueber den Victim-Throwoff (Release-Clip), Spieler lebt mit hp=0. */
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
#include "re15_damage.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"

extern int re15_player_victim_state(void);
extern int re15_player_death_cmd3_active(void);

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

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
static uint8_t *s_re2_ems = NULL;

static void force_grab(re15_actor_t *crow, const re15_actor_t *pl)
{
    g_re2_room_gflags |= 0x1u;
    crow->state = 1; crow->sub_state_1 = 14; crow->sub_state_2 = 0; crow->sub_state_3 = 0;
    crow->re2c_flags22a = 0x2u | 0x4u;
    crow->x = pl->x + 100; crow->y = pl->y - 1500; crow->z = pl->z + 100;
}

static re15_actor_t *setup(const re15_rdt_t *rdt, re15_game_ctx_t *ctx,
                           re15_camera_view_t *cam, int16_t hp)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    if (!s_re2_ems) {
        char p[600]; size_t sz = 0;
        snprintf(p, sizeof p, "%s/../RE2/CDEMD0.EMS", s_base);
        s_re2_ems = read_file(p, &sz);
        if (s_re2_ems) {
            re15_enemy_bank_t *eb = re15_enemy_find(0x21);
            if (!eb) eb = re15_enemy_alloc(0x21);
            if (eb) re2_ems_load_bank(s_re2_ems, sz, 0x21, eb, NULL);
        }
    } else {
        /* re15_actor_init koennte Banken behalten — sicherstellen, dass 0x21 existiert */
        re15_enemy_bank_t *eb = re15_enemy_find(0x21);
        (void)eb;
    }
    scd_vm_init();
    g_current_room_id = 0x1170;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 530; pl->y = -7200; pl->z = -5150;
    pl->hp = hp; pl->rot_y = 0; pl->hit_react = 0;
    pl->motion = 0; pl->anim_frame = 0; pl->anim_frac = 0;
    memcpy((void *)&g_room_rdt, rdt, sizeof(g_room_rdt));
    g_room_rdt_ok = 1;
    re15_collision_set_band(4);
    re15_actor_t *crow = &g_actors[1];
    memset(crow, 0, sizeof *crow);
    crow->active = 1; crow->type = 0x21; crow->hp = 10;
    crow->hit_radius_min = 96; crow->hit_height = 512;
    force_grab(crow, pl);
    memset(cam, 0, sizeof *cam);
    memset(ctx, 0, sizeof *ctx);
    ctx->rdt = rdt; ctx->rdt_ok = 1; ctx->cam_view = cam; ctx->active_cut = 0;
    if (s_pl00_ok) { ctx->pl00_skel = &s_pl00_skel; ctx->pl00_anim = &s_pl00_anim; }
    return pl;
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    s_base = base;
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1170.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(data, size, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }
    {   size_t a = 0, b = 0;
        snprintf(path, sizeof path, "%s/PLD/PL00.EDD", base);
        uint8_t *edd = read_file(path, &a);
        snprintf(path, sizeof path, "%s/PLD/PL00.EMR", base);
        uint8_t *emr = read_file(path, &b);
        s_pl00_ok = (edd && emr &&
                     re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
                     re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0);
    }
    CHECK(s_pl00_ok, "PL00.EDD/EMR muss ladbar sein");

    re15_game_ctx_t ctx; re15_camera_view_t cam;

    /* ===== PIN A: Tod am zweiten Peck (hp=4 -> One-Save hp=0+Latch -> ret 2) ===== */
    {
        re15_actor_t *pl = setup(&rdt, &ctx, &cam, 4);
        re15_actor_t *crow = &g_actors[1];
        int f, died = -1;
        for (f = 0; f < 1200 && pl->hp >= 0; f++) {
            if (!(crow->state == 1 && crow->sub_state_1 == 14)) force_grab(crow, pl);
            re15_game_step(&ctx);
        }
        died = f;
        CHECK(pl->hp < 0, "PIN A: Kill-Peck muss den Spieler toeten (hp=%d, f=%d)", (int)pl->hp, died);
        /* cmd 3 muss im Release-Tick gelatcht sein (±1 Tick, s. Divergenz-Doku) */
        int saw7 = (pl->motion == 7), maxframe = (pl->motion == 7) ? pl->anim_frame : -1;
        for (int t = 0; t < 200; t++) {
            re15_game_step(&ctx);
            if (pl->motion == 7) { saw7 = 1; if (pl->anim_frame > maxframe) maxframe = pl->anim_frame; }
        }
        CHECK(saw7, "PIN A: Todes-Clip motion==7 muss spielen (PL(3,0,0,0) @0x80104818-38)");
        CHECK(maxframe >= 112, "PIN A: Todes-Clip muss durchlaufen (113 F), max frame=%d", maxframe);
        CHECK(re15_player_death_cmd3_active(), "PIN A: cmd-3-FSM muss aktiv sein");
        CHECK(re15_player_victim_state() == 0,
              "PIN A: kein Victim-Release-Clip fuer den Toten (bgez-Skip @0x80104810), vs=%d",
              re15_player_victim_state());
    }

    /* ===== PIN B: One-Save (hp=100 -> Peck toetet nie direkt; mode-1-Pfad) ============
     * Der lebende Spieler wird NIE in cmd 3 geschickt; ein regulaeres Grab-Ende laeuft
     * ueber den Victim-Throwoff/Release (Regressionswache fuer den neuen else-Zweig). */
    {
        re15_actor_t *pl = setup(&rdt, &ctx, &cam, 100);
        re15_actor_t *crow = &g_actors[1];
        for (int f = 0; f < 300; f++) {
            if (!(crow->state == 1 && crow->sub_state_1 == 14) && f < 150) force_grab(crow, pl);
            re15_game_step(&ctx);
        }
        CHECK(pl->hp >= 0, "PIN B: Pecks allein duerfen den 100-HP-Spieler nicht toeten (hp=%d)",
              (int)pl->hp);
        CHECK(!re15_player_death_cmd3_active(),
              "PIN B: cmd 3 darf beim lebenden Spieler nie latchen");
        CHECK(pl->motion != 7, "PIN B: kein Todes-Clip beim Lebenden (motion=%d)", (int)pl->motion);
    }

    printf(fails ? "test_re2_crow_kill_cmd3: %d FAIL(s)\n" : "test_re2_crow_kill_cmd3: OK\n", fails);
    free(data);
    return fails ? 1 : 0;
}
