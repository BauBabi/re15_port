/* test_maggot_bone_square.c — PIN (Nutzer-Report 2026-08-29 "Die Affen treffen Leon so
 * gut wie nie"): der Affen-Trefferpruefer ist das ORIGINAL-Bone-QUADRAT (FUN_8001bff8:
 * |plX-BoneX| <= r UND |plZ-BoneZ| <= r, @0x8001c080-c0c0), OHNE Winkeltest — der alte
 * Port-Proxy (Zentrums-Distanz 2051 + Arc ±384/±0xc0) verwarf seitliche Kontakt-Treffer
 * und erzeugte das Whiff-Band 2051..3000 (Biss-Commit @0x80117a60-74 bei dist<3000).
 * Biss: BONE 9 (Pool+0x64c) r=0x3e8 @0x801183c0-cc; Heavy: dual BONE 6/10 r=0x320
 * @0x801186f0-714. (analysis/nutzer_batch_2026-08-29/affen-treffer-clip.md)
 *
 * Dieser Pin fasst den MECHANISMUS bankfrei (Bone-Fallback = Aktor-Wurzel):
 * ein SEITLICH stehender Leon in Kontaktnaehe (900) wird vom Biss getroffen —
 * der alte Arc (±33,75 Grad) verwarf genau das. */
#include "re15_actor.h"
#include "re15_damage.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

#define MS 1

static re15_actor_t *setup(int32_t px, int32_t pz)
{
    re15_actor_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = px; pl->y = 0; pl->z = pz;

    re15_actor_t *e = &g_actors[MS];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x27; e->hp = 200;
    e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;   /* schaut +X-artig; Spieler steht SEITLICH */
    e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x27);
    /* direkt ins Biss-Fenster: B[5] Clip 0x12, Frames {0x0c..0x0f} @0x8012146c */
    e->state = 1; e->sub_state_1 = 5; e->sub_state_2 = 1; e->sub_state_3 = 0;
    e->motion = 0x12; e->anim_frame = 0x0b; e->anim_frac = 0;
    return e;
}

int main(void)
{
    printf("=== Affen-Biss = Bone-QUADRAT ohne Arc (FUN_8001bff8 @0x8001c080-c0c0) ===\n");

    /* A: Spieler SEITLICH (90 Grad zur Blickrichtung) bei 900 — im Quadrat (r=1000),
     *    vom alten Arc ±384 verworfen. */
    {
        re15_actor_t *e = setup(0, 900);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int16_t hp0 = pl->hp;
        for (int t = 0; t < 6 && pl->hp == hp0; t++) re15_enemy_ai_run_all(1);
        printf("  [A] frame=%d hp %d -> %d ctr=%d\n",
               (int)e->anim_frame, hp0, pl->hp, (int)e->dog_blocked_ctr);
        CHECK("seitlicher Kontakt-Biss trifft (-6 @0x80118460-6c)", pl->hp == hp0 - 6);
        CHECK("Biss-Lockout gesetzt (+0x1dc=0x2d @0x80118470-78; Tail dekrementiert im selben "
              "Tick -> 0x2c)", e->dog_blocked_ctr == 0x2c);
    }

    /* B: Spieler AUSSERHALB des Quadrats (1400 > r=1000) — kein Treffer. */
    {
        re15_actor_t *e = setup(0, 1400);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int16_t hp0 = pl->hp;
        for (int t = 0; t < 6; t++) re15_enemy_ai_run_all(1);
        (void)e;
        CHECK("ausserhalb des 1000er-Quadrats: kein Treffer", pl->hp == hp0);
    }

    if (g_fail) { printf("FAIL\n"); return 1; }
    printf("OK\n");
    return 0;
}
