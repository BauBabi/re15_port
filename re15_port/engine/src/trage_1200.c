/*
 * RE1.5 Rebuilt — ROOM1200: der Zombie von der Trage (Runde 35 Spur H, Punkt 3).
 * Herleitung und alle Adressen: include/re15_trage1200.h und
 * analysis/befunde_runde35/H_raeume.md (Punkt 3).
 */
#include "re15_trage1200.h"

#include "re15_actor.h"
#include "re15_engine.h"     /* g_engine.frame_count */
#include "re15_room.h"       /* g_room_rdt, g_room_rdt_ok, g_current_room_id */
#include "re15_collision.h"  /* re15_collision_floor_typeword = FUN_8003b7f0 mit Band/Radius */
#include "re15_climb.h"      /* re15_climb_dbg_standing = DAT_800aca3c & 0x4000 (Standobjekt) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

int re15_schwerkraft_8001bd60(re15_actor_t *e, int32_t a0, int32_t a1, int32_t radius)
{
    if (!e) return 0;
    /* 8001bd64-80: DAT_800aca3c & 0x4000 (Spieler steht auf einem Objekt, climb_common.c
     * @0x80031d24) -> Erkennung uebersprungen, `bne v0,zero,0x8001be58` = nur der Fall-Teil. */
    if (re15_climb_dbg_standing() < 0 && g_room_rdt_ok) {
        /* 8001bd94-a8: r = FUN_8003b7f0(&+0x34, -(*(+0x78)+6), +0x82) */
        const uint16_t r = re15_collision_floor_typeword(&g_room_rdt, e->x, e->z,
                                                         (int)e->floor, -radius);
        /* 8001bdb0-b4: andi v0,a1,0x2 / beq -> kein Absturz */
        if (r & 2u) {
            /* 8001bdc8-ec: y == -(floor * 1800) */
            if (e->y == -((int32_t)e->floor * RE15_SCHWERKRAFT_BAND)) {
                unsigned n = (unsigned)(r & 0xcu) >> 2;                 /* @0x8001bdf8-be04 */
                e->fall_1c0 = (uint16_t)(RE15_SCHWERKRAFT_FAELLT | (n << 13)); /* @0x8001bdf4-be14 */
                for (;;) {                                              /* do-while @0x8001be18-54 */
                    e->dog_floor_y = (int16_t)(e->dog_floor_y + RE15_SCHWERKRAFT_BAND); /* +0x1ba += 1800 */
                    const unsigned v1 = n & 0xffu;                      /* @0x8001be34 / be48 */
                    n = n - 1u;                                         /* @0x8001be40 */
                    e->floor = (uint8_t)(e->floor - 1u);                /* +0x82 -= 1 @0x8001be4c-54 */
                    if (v1 == 0) break;                                 /* bne v1,zero @0x8001be50 */
                }
            }
        }
    }
    /* 8001be58-70: faellt? */
    const uint16_t w = e->fall_1c0;
    if (!(w & RE15_SCHWERKRAFT_FAELLT)) return 0;
    const int32_t t = (int32_t)(w & 0x1fffu);                           /* andi 0x1fff @0x8001be7c */
    e->fall_1c0 = (uint16_t)(w + 1u);                                   /* addiu +1 @0x8001be84-88 */
    e->y = e->y + (int32_t)(int16_t)a0 + (int32_t)(int16_t)a1 * t;      /* @0x8001be78-a4 */
    if ((int32_t)e->dog_floor_y < e->y) {                               /* slt @0x8001bec0 */
        e->y = (int32_t)e->dog_floor_y;                                 /* sw a0,56 @0x8001becc */
        e->fall_1c0 = (uint16_t)(e->fall_1c0 & 0x7fffu);                /* andi 0x7fff @0x8001bee4 */
    }
    return 1;
}

void re15_schwerkraft_seed(re15_actor_t *e)
{
    if (!e) return;
    if (e->type == 0x10 || e->type == 0x11 || e->type == 0x12 || e->type == 0x16 || e->type == 0x18) {
        e->dog_floor_y = (int16_t)(-(int32_t)e->floor * RE15_SCHWERKRAFT_BAND);   /* @0x80042210 */
        e->fall_1c0 = 0;
    }
}

void re15_trage1200_mess(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_init = 0; static FILE *s_lg = NULL;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_GEGNER_Y_LOG");
        if (p && *p) s_lg = fopen(p, "w");
    }
    if (!s_lg) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(s_lg, "F%u R%04X PL(%d,%d,%d b%d)", (unsigned)g_engine.frame_count,
            (unsigned)g_current_room_id, pl->x, pl->y, pl->z, (int)pl->floor);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || e->type == 0) continue;
        fprintf(s_lg, " [%d t=%02x st=%d/%d/%d/%d g=%02x mo=%d af=%d @(%d,%d,%d) b%d f1ba=%d f1c0=%04x"
                      " rot=%d los=%u t158=%d t15a=%d cd=%u]",
                s, (unsigned)e->type, (int)e->state, (int)e->sub_state_1, (int)e->sub_state_2,
                (int)e->sub_state_3, (unsigned)e->grid_id, (int)e->motion, (int)e->anim_frame,
                e->x, e->y, e->z, (int)e->floor, (int)e->dog_floor_y, (unsigned)e->fall_1c0,
                (int)e->rot_y, (unsigned)e->re2z_los154, (int)e->re2z_t158, (int)e->re2z_t15a,
                (unsigned)e->re2z_cd23e);
    }
    fputc('\n', s_lg);
    fflush(s_lg);
#endif
}
