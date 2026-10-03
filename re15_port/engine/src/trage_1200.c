/*
 * RE1.5 Rebuilt — ROOM1200: der Zombie von der Trage (Runde 35 Spur H, Punkt 3).
 * Herleitung: include/re15_trage1200.h und analysis/befunde_runde35/H_raeume.md (Punkt 3).
 */
#include "re15_trage1200.h"

#include "re15_actor.h"
#include "re15_engine.h"   /* g_engine.frame_count */
#include "re15_room.h"     /* g_current_room_id */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

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
        fprintf(s_lg, " [%d t=%02x st=%d/%d/%d/%d g=%02x mo=%d af=%d @(%d,%d,%d) b%d]",
                s, (unsigned)e->type, (int)e->state, (int)e->sub_state_1, (int)e->sub_state_2,
                (int)e->sub_state_3, (unsigned)e->grid_id, (int)e->motion, (int)e->anim_frame,
                e->x, e->y, e->z, (int)e->floor);
    }
    fputc('\n', s_lg);
    fflush(s_lg);
#endif
}
