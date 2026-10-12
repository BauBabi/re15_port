/*
 * Runde 37 Spur A (pri1100) — Messhaken RE15_GEGNER_POS, s. include/re15_pri1100_r37.h und
 * analysis/befunde_runde37/A_pri1100.md. Keine Spiellogik.
 */
#include "re15_pri1100_r37.h"

#include "re15_actor.h"
#include "re15_engine.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

extern unsigned g_current_room_id;

void re15_r37a_gegner_pos_haken(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_init = 0, s_slot = -1, s_x, s_z, s_rot, s_bild;
    static unsigned s_raum;
    static int s_gemeldet = 0;
    if (!s_init) {
        const char *e = getenv("RE15_GEGNER_POS");
        s_init = 1;
        if (e && *e) {
            unsigned r = 0;
            if (sscanf(e, "%d,%d,%d,%d@%d#%x", &s_slot, &s_x, &s_z, &s_rot, &s_bild, &r) == 6 &&
                s_slot > 0 && s_slot < RE15_ACTOR_MAX)
                s_raum = r;
            else
                s_slot = -1;
        }
    }
    if (s_slot < 0 || g_current_room_id != s_raum || (int)g_engine.frame_count < s_bild) return;
    {
        re15_actor_t *a = &g_actors[s_slot];
        if (!a->active) return;
        a->x = s_x; a->z = s_z; a->rot_y = (int16_t)s_rot;
        if (!s_gemeldet) {
            s_gemeldet = 1;
            fprintf(stderr, "[r37a] RE15_GEGNER_POS: Akteur %d -> (%d,%d,%d) rot=%d ab Bild %d in Raum %04X\n",
                    s_slot, s_x, (int)a->y, s_z, s_rot, s_bild, s_raum);
        }
    }
#endif
}
