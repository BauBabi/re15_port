/* probe_r17_birkin_1zu1.c — PIN (ctest) fuer die Koerper-Kollision des G5-Endbosses
 * (Nutzer-Forderung Runde 17: "Birkin ist nicht solid").
 *
 * Gefahren wird derselbe ECHTE Ablauf wie in probe_p2_birkin_g5.c / probe_p3_birkin_rest.c
 * (ROOM5090.RDT -> SCD-VM -> re15_game_step -> re15_enemy_ai_run_all mit den RE2-Baenken
 * EM036/EM037).
 *
 *  A) SOLIDITAET. RE2 gibt G5 ZWEI Kollisionssegmente statt eines +0x78-Kastens:
 *     +0x1E8 = 2 (@0x8010052c-30), Seg0 lokal X -2000 / r 6000 (@0x80100534-40),
 *     Seg1 lokal X 2200..3700 / r 1300 (@0x80100550-5c), beide lokal Y -1500 /
 *     Halbhoehe 1500 (@0x80100470/@0x80100448 + @0x80100520-24/@0x80100580-88).
 *     Der Spieler traegt EIN Segment r 450 / Halbhoehe 1530 (@0x8003bdc0-ec) und wird aus
 *     seinem eigenen Tick heraus herausgeschoben (FUN_800355C4 @0x80026628 ->
 *     FUN_80034D0C @0x80035630). Scharf ab der Armierung (Wort0-Bit 0x2 geloescht,
 *     `addiu v1,zero,-3 / and / sw` @0x801011e4-f0).
 *     Gemessen wird der ABSTAND, auf den der Spieler herausgeschoben wird (Sweep von
 *     8000 nach 0), gegen den, den das alte Ein-Zylinder-Modell (r 1000 aus
 *     re15_enemy_apply_hitbox) liefert.
 *  B) PHASENABHAENGIGKEIT. Nur Segment 1 wandert (+0xB4, 2200 -> 3700 mit +500/Bild
 *     @0x801040b8-e0, zurueck mit -15/Bild @0x801042b0-d4). Segment 0 steht fest.
 *  C) ARMIERUNGS-GATE. Vor der Armierung schiebt G5 nicht (Ctor `ori v1,v1,0x2`
 *     @0x80100594, Ausstieg in FUN_80034D0C bei `(*p1|*p2)&2`).
 */
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
#include "re15_fade.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_g5_boss_zustand(int *sub, int *ph, int *timer);
extern int  re15_g5_body_segment(int idx, int32_t *wx, int32_t *wy, int32_t *wz, int32_t *r);
extern int  re15_g5_body_push_player(re15_actor_t *pl);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;
static int                s_fail = 0;

#define PIN(cond, ...) do { if (!(cond)) { s_fail++; printf("  PIN FEHLT: " __VA_ARGS__); printf("\n"); } \
                            else { printf("  PIN ok: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_re2_banks(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    for (int k = 0; k < 2; k++) {
        uint8_t type = k ? 0x37u : 0x36u;
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (!eb) return 0;
        if (re2_ems_load_bank(ems, n, type, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    return 1;
}

static int s_cine_was_active = 0;
static void frame(void)
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
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

static int find_boss(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x36) return s;
    return -1;
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    (void)argc; (void)argv;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== R17 birkin-1zu1 PIN: Koerper-Kollision des G5 (\"nicht solid\") ===\n");

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 14;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x5090; g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;
    re15_collision_set_band(0);

    if (!load_re2_banks()) { printf("FEHLT: RE2-Baenke (CDEMD0.EMS)\n"); return 77; }

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    /* ---------------- C) vor der Armierung: keine Segmente ----------------------------- */
    printf("-- C) Armierungs-Gate --\n");
    for (int f = 0; f < 30; f++) frame();
    {
        int32_t sx = 0;
        int vor = re15_g5_body_segment(0, &sx, NULL, NULL, NULL);
        PIN(vor == 0, "vor der Armierung liefert G5 keine Kollisionssegmente (Ctor"
                      " `ori v1,v1,0x2` @0x80100594; FUN_80034D0C steigt bei (*p1|*p2)&2 aus)");
    }

    int trigger_f = -1;
    for (int f = 30; f < 500; f++) {
        int32_t nx = pl->x - 75, nz = pl->z;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
        pl->x = nx; pl->z = nz;
        frame();
        if (trigger_f < 0 && s_shown == 12) trigger_f = f;
        if (trigger_f >= 0 && f > trigger_f + 3) break;
    }
    int bslot = find_boss();
    if (bslot < 0) { printf("FEHLT: kein Boss\n"); return 77; }
    re15_actor_t *e = &g_actors[bslot];

    /* Intro auslaufen lassen (Clip 5 = sub1). */
    for (int f = 0; f < 1500 && e->motion != 5; f++) frame();

    /* ---------------- A) Soliditaet: Standabstand ------------------------------------- */
    printf("-- A) Soliditaet (Sweep 8000 -> 100 auf der X-Achse, z = Boss-z) --\n");
    {
        int32_t seg0x = 0, seg0y = 0, seg0z = 0, seg0r = 0;
        int32_t seg1x = 0, seg1r = 0;
        re15_g5_body_segment(0, &seg0x, &seg0y, &seg0z, &seg0r);
        re15_g5_body_segment(1, &seg1x, NULL, NULL, &seg1r);
        printf("  Boss (%d,%d,%d)  Seg0 Welt-x %d r %d  Seg1 Welt-x %d r %d  (Seg-y %d)\n",
               (int)e->x, (int)e->y, (int)e->z, (int)seg0x, (int)seg0r,
               (int)seg1x, (int)seg1r, (int)seg0y);

        int32_t min_neu = 0x7fffffff, min_alt = 0x7fffffff;
        for (int32_t d = 8000; d >= 100; d -= 100) {
            /* NEU: das RE2-Segmentmodell. */
            pl->x = e->x + d; pl->z = e->z; pl->y = e->y;
            re15_g5_body_push_player(pl);
            if (pl->x - e->x < min_neu) min_neu = pl->x - e->x;
            /* ALT: der Ein-Zylinder-Push aus re15_enemy_apply_hitbox (r 1000 / h 1440),
             * derselbe Codepfad, den re15_body_push_player vor diesem Fix genommen hat. */
            pl->x = e->x + d; pl->z = e->z; pl->y = e->y;
            {   uint16_t r0 = e->hit_radius_min, r1 = e->hit_radius_max, h0 = e->hit_height;
                int16_t  oy = e->hit_offset_y;
                e->hit_radius_min = 1000; e->hit_radius_max = 1000;
                e->hit_height = 1440; e->hit_offset_y = -1440;
                re15_body_push(e, 1000, pl, 450);
                e->hit_radius_min = r0; e->hit_radius_max = r1;
                e->hit_height = h0; e->hit_offset_y = oy;
            }
            if (pl->x - e->x < min_alt) min_alt = pl->x - e->x;
        }
        printf("  kleinster Abstand Spieler-Ursprung zum Boss-Ursprung:"
               " ALT (1 Zylinder r1000) = %d,  NEU (RE2 2 Segmente) = %d\n",
               (int)min_alt, (int)min_neu);
        /* Seg0 dominiert: Ursprung + vor*(-2000) + (6000+450) = 4450 vor dem Ursprung,
         * Seg1 (2200+1300+450 = 3950) liegt darunter. Die Massenfront steht bei
         * Ursprung + 4494 (Dossier §1.2) — der Spieler steht also VOR der Masse. */
        PIN(min_neu >= 4300 && min_neu <= 4600,
            "RE2-Segmente halten den Spieler ~4450 vor dem Boss-Ursprung (Seg0 -2000 + 6000"
            " + 450 @0x80100534-40/@0x8003bdc0): gemessen %d", (int)min_neu);
        PIN(min_alt >= 1400 && min_alt <= 1460,
            "das alte Ein-Zylinder-Modell hielt nur ~1450 (1000 + 450) — 3 m INNERHALB der"
            " sichtbaren Masse (Front = Ursprung + 4494): gemessen %d", (int)min_alt);
        PIN(min_neu > min_alt + 2500,
            "der Spieler steht jetzt vor statt in der Masse (Differenz %d)",
            (int)(min_neu - min_alt));
    }

    /* Der Spieler darf die Masse auch seitlich nicht durchqueren. */
    {
        int drin = 0;
        for (int32_t dz = -7000; dz <= 7000; dz += 250) {
            int32_t sx = 0, sz = 0, sr = 0, dx, ddz;
            pl->x = e->x; pl->z = e->z + dz; pl->y = e->y;
            re15_g5_body_push_player(pl);
            re15_g5_body_segment(0, &sx, NULL, &sz, &sr);
            dx = pl->x - sx; ddz = pl->z - sz;
            if ((int64_t)dx * dx + (int64_t)ddz * ddz < (int64_t)(sr + 450) * (sr + 450) - 4096)
                drin++;
        }
        printf("  Seitlicher Durchlauf: %d von 57 Punkten bleiben im Segment 0\n", drin);
        PIN(drin <= 1, "auch quer bleibt kein Punkt in Segment 0 stecken (der Punkt exakt"
                       " auf der Achse hat dist 0 und ist im Original genauso schubfrei):"
                       " %d", drin);
    }

    /* ---------------- B) Phasenabhaengigkeit von Segment 1 ---------------------------- */
    printf("-- B) Phasenabhaengigkeit --\n");
    {
        int32_t s1min = 0x7fffffff, s1max = -0x7fffffff, s0min = 0x7fffffff, s0max = -0x7fffffff;
        for (int f = 0; f < 6000; f++) {
            int32_t x0 = 0, x1 = 0, r0 = 0, r1 = 0;
            pl->x = e->x + 5000; pl->z = e->z; pl->y = 0; pl->hp = 100;
            frame();
            if (!re15_g5_body_segment(0, &x0, NULL, NULL, &r0)) continue;
            re15_g5_body_segment(1, &x1, NULL, NULL, &r1);
            {   int32_t l0 = x0 - e->x, l1 = x1 - e->x;
                if (l0 < s0min) s0min = l0; if (l0 > s0max) s0max = l0;
                if (l1 < s1min) s1min = l1; if (l1 > s1max) s1max = l1;
                if (s1max >= 3700 && f > 1500) break;
            }
        }
        printf("  Segment 0 lokal X: %d..%d   Segment 1 lokal X: %d..%d\n",
               (int)s0min, (int)s0max, (int)s1min, (int)s1max);
        PIN(s0min == -2000 && s0max == -2000,
            "Segment 0 steht fest bei -2000 (@0x80100534-38): %d..%d", (int)s0min, (int)s0max);
        PIN(s1min == 2200 && s1max == 3700,
            "Segment 1 faehrt im Massen-Biss von 2200 auf 3700 aus (+500 @0x801040c0, Kappe"
            " 3700 @0x801040dc-e0; zurueck -15 @0x801042b8, Boden 2200 @0x801042d0-d4):"
            " %d..%d", (int)s1min, (int)s1max);
    }

    printf("=== %s (%d Pins fehlen) ===\n", s_fail ? "FEHLER" : "ALLE PINS OK", s_fail);
    return s_fail ? 1 : 0;
}
