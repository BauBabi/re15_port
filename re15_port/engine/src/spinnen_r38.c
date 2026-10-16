/*
 * RE1.5 Rebuilt — Runde 38 Spur R "spinnen" (Stage-2-Spinnen). Kopf: include/re15_spinnen_r38.h,
 * Dossier: analysis/befunde_runde38/R_spinnen.md.
 */
#include "re15_spinnen_r38.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_esp.h"

#include <stdio.h>
#include <stdlib.h>

/* ---- MESSSCHIENE RE15_SPINNEN_LOG ------------------------------------------------------------
 * Reiner Messhaken (env-gegated, kein Spielverhalten): je Bild und Spinne eine Zeile. Die GUI-exe
 * hat kein nutzbares stderr, deshalb eine Datei (Muster RE15_STATE_LOG, platform/pc/main.c). */
void re15_spinnen_r38_log_frame(unsigned frame)
{
    static FILE *s_fp = NULL;
    static int   s_init = 0;
    if (!s_init) {
        s_init = 1;
        const char *p = getenv("RE15_SPINNEN_LOG");
        if (p && *p) s_fp = fopen(p, "w");
    }
    if (!s_fp) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(s_fp, "F%u R%04X PL(%d,%d,%d hp=%d st=%d ps=%04x fl=%d) fx=%d", frame,
            (unsigned)g_current_room_id, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->hp,
            (int)pl->state, (unsigned)pl->status_flags, (int)pl->floor, re15_esp_fx_count());
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (!e->active || (e->type != 0x25 && e->type != 0x26)) continue;
        fprintf(s_fp, " [%d t=%02x st=%d s=%d.%d.%d g=%02x fl=%d m=%d y22c=%d mo=%d af=%d"
                      " @(%d,%d,%d) r=(%d,%d,%d) hp=%d]",
                i, e->type, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                e->grid_id, e->floor, e->re2s_mode222, (int)e->re2s_y22c, e->motion,
                (int)e->anim_frame, (int)e->x, (int)e->y, (int)e->z,
                (int)e->rot_x, (int)e->rot_y, (int)e->rot_z, (int)e->hp);
    }
    fputc('\n', s_fp);
    fflush(s_fp);
}
