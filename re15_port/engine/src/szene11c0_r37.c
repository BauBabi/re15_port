/*
 * RE1.5 Rebuilt — Runde 37 Spur O "szene11c0": Rechenhilfe + Mess-Haken der Ada/Marvin-Szene ROOM11C0.
 * Dossier analysis/befunde_runde37/O_szene11c0.md; Choreografie = k_p_11c0 in irons_tod_1150.c.
 */
#include "re15_szene11c0_r37.h"

#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_scd.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);   /* actor_locomotion.c (FUN_8001a6d4 + 0x400) */

int re15_s11c0_gier(int32_t vx, int32_t vz, int32_t zx, int32_t zz)
{
    /* = re15_enemy_steer_point: `bearing = ((int)re15_atan2_q12(tz - e->z, tx - e->x) - 0x400) & 0x0fff` */
    return ((int)re15_atan2_q12(zz - vz, zx - vx) - 0x400) & 0x0fff;
}

int re15_s11c0_winkel_diff(int a, int b)
{
    int d = (b - a) & 0x0fff;
    return (d > 0x800) ? d - 0x1000 : d;
}

void re15_s11c0_mess_takt(void)
{
#ifdef RE15_PLATFORM_PC
    static FILE *f = NULL; static int init = 0;
    if (!init) { init = 1; const char *p = getenv("RE15_S11C0_LOG"); if (p && *p) f = fopen(p, "w"); }
    if (!f) return;
    const re15_actor_t *fig[2] = { &g_actors[1], &g_actors[4] };
    fprintf(f, "F%u msg=%d:%d cam=%d", (unsigned)g_engine.frame_count, (int)g_scd.message_active,
            (int)g_scd.message_id, (int)g_scd.cam_id);
    for (int i = 0; i < 2; i++) {
        const re15_actor_t *a = fig[i], *p = fig[1 - i];
        if (!a->active) { fprintf(f, " %c(-)", i ? 'M' : 'A'); continue; }
        int gp = p->active ? re15_s11c0_gier(a->x, a->z, p->x, p->z) : -1;
        fprintf(f, " %c(p=(%d,%d) r=%d mo=%d s=%d nf=%02x nt=(%d,%d) acc=%d gp=%d)", i ? 'M' : 'A',
                (int)a->x, (int)a->z, (int)(a->rot_y & 0x0fff), (int)a->motion, (int)a->sub_state_1,
                (unsigned)a->neck_flags, (int)a->neck_tx, (int)a->neck_tz, (int)a->neck_yaw, gp);
    }
    fputc('\n', f); fflush(f);
#endif
}
