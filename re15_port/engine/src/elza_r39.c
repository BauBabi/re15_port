/*
 * RE1.5 Rebuilt — Runde 39 Spur H "elza". Belege und Herleitung: include/re15_elza_r39.h,
 * Dossier analysis/befunde_runde39/H_elza.md.
 */
#include "re15_elza_r39.h"
#include "re15_collision.h"   /* re15_collision_band_from_y = -(y / 0x708), re15_collision.c:186 */
#include "re15_actor.h"       /* re15_actor_set_motion, g_actors */
#include "re15_to_re2.h"      /* re15_to_re2_plc_dest_clip: +0x94 = 5 der PLW-Bank -> Sentinel 105 (RBJ-Overlay) */

int re15_elza_r39_plc_walk_init(re15_actor_t *a)
{
    if (a != &g_actors[RE15_ACTOR_SLOT_PLAYER]) return 0;        /* nur die Spieler-Subs @0x80073e30 [4]/[5]/[9] */
    if (a->walk_mode != 0x04 && a->walk_mode != 0x05 && a->walk_mode != 0x09) return 0;
    {
        /* +0x94 := 5 (@0x80030b58 WALK / @0x80030d90 RUN / @0x800313a4 TURN) = PLW-Paar Clip 5 = Sentinel 105 */
        const int clip = re15_to_re2_plc_dest_clip(0x04, 1);
        if (clip < 0) return 0;
        re15_actor_set_motion(a, (int16_t)clip);
        a->anim_frame  = 0;      /* +0x95 := 0 (@0x80030b64 / @0x80030d9c / @0x800313b0) — auch bei gleichem Clip */
        a->anim_frac   = 7;      /* +0x8f := 7 (@0x80030b6c / @0x80030da4 / @0x800313b8) — Blende aus der Vor-Pose */
        a->anim_freeze = 0;      /* Port-Halt einer vorigen Ankunft; das Original kennt keinen Halt, der Clip laeuft */
    }
    return 1;
}

int re15_elza_r39_latch_hinfaellig(int state)
{
    /* Plc_motion @0x80041bb0 `sb v1(=4),4(v0)`: Kommandowort 4 -> Dispatcher-Tabelle @0x80073f90[4] = 0x80030660;
     * cmd 0 (@0x80031c10-c24) und Fall 0 (@0x80032074-b4) kommen nicht mehr dran. */
    return state == RE15_R39H_KOMMANDOWORT_PLC;
}

int re15_elza_r39_prop_band_ueber_boden(int32_t y)
{
    /* Original-Form fuer Null-Box-Requisiten: ROOM1091 obj 2 @0x02202 pc[4] = 2 bei y -2653 (Spielerband 1, Boden -1800),
     * ROOM1010 @0x00930 pc[4] = 1 bei y -1600 (Spielerband 0). Das Band-Gate der Klettersonde FUN_8002d2c0
     * @0x8002d364-70 (Objekt +0x82 == 0x800acad6) laesst ein solches Objekt damit nie als Kletterobjekt zu. */
    return re15_collision_band_from_y(y) + 1;
}
