/*
 * RE1.5 Rebuilt — Runde 37 Spur O "szene11c0": Rechenhilfe, Port-Takt "Ada folgt Marvin" (Nachtrag 12) und
 * Mess-Haken der Ada/Marvin-Szene ROOM11C0.
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

/* NACHTRAG 12: Ada folgt dem laufenden Marvin (Belege im Kopf von re15_szene11c0_r37.h). */
static void folgen_takt(void)
{
    if (!re15_game_flag_get(5, RE15_S11C0_SIG_FOLGEN)) return;
    re15_actor_t *ada = &g_actors[RE15_S11C0_ADA_AKTOR];
    const re15_actor_t *mar = &g_actors[RE15_S11C0_MARVIN_AKTOR];
    if (!ada->active || !mar->active) return;
    /* Kopf: Ziel = Kopf-Ort der Figur (RE2 FUN_800177C0 @0x80017924-30 `lw v0,92(t0)` ... = Teil-Weltlage); der Port
     * fuehrt ihn in head_world (Vorbild-Bild), sonst die Wurzel. Nur im Weltpunkt-Modus (0x04, Plc_neck m1). */
    if (ada->neck_flags & 0x04) {
        ada->neck_tx = (int16_t)(mar->head_world_ok ? mar->head_world[0] : mar->x);
        ada->neck_tz = (int16_t)(mar->head_world_ok ? mar->head_world[2] : mar->z);
    }
    /* Koerper: nur waehrend des Drehens (Sub 9, Phase 0/1 — re15_npc_sub_turn liest +0x1bc/+0x1be je Bild). */
    if (ada->state == 4 && ada->sub_state_1 == 9 && ada->sub_state_2 <= 1) {
        ada->steer_x = (int16_t)mar->x;
        ada->steer_z = (int16_t)mar->z;
    }
}

void re15_s11c0_takt(void)
{
    folgen_takt();
    re15_s11c0_mess_takt();
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
        fprintf(f, " %c(p=(%d,%d) r=%d mo=%d s=%d nf=%02x nt=(%d,%d) acc=%d gp=%d h=(%d,%d))", i ? 'M' : 'A',
                (int)a->x, (int)a->z, (int)(a->rot_y & 0x0fff), (int)a->motion, (int)a->sub_state_1,
                (unsigned)a->neck_flags, (int)a->neck_tx, (int)a->neck_tz, (int)a->neck_yaw, gp,
                (int)a->head_world[0], (int)a->head_world[2]);
    }
    fputc('\n', f); fflush(f);
#endif
}
