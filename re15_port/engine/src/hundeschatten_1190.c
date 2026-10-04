/*
 * RE1.5 Rebuilt — Hunde-Schatten auf die Boden-Referenz (Runde 35 Spur H, Punkt 1).
 * Herleitung und alle Adressen: include/re15_hundeschatten.h und
 * analysis/befunde_runde35/H_raeume.md (Punkt 1).
 */
#include "re15_hundeschatten.h"

#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#include "re15_engine.h"   /* g_engine.frame_count — nur fuer die Messzeile */
#endif

int32_t re15_hundeschatten_y(const re15_actor_t *e)
{
    if (!e) return 0;
    if (e->type != RE15_HUNDESCHATTEN_TYP) return e->y;
    /* FUN_8010d7f8: `lh a1,442(v0)` @0x8010d91c -> `jal 0x8001b064` @0x8010d920 (t[1] = a1) */
    const int32_t y = (int32_t)e->dog_floor_y;
#ifdef RE15_PLATFORM_PC
    /* Messschiene RE15_HUNDESCHATTEN_LOG=<datei> (env-gegatet, kein Verhalten): je gezeichnetem
     * Hunde-Schatten eine Zeile — Koerper-Y, Boden-Referenz, gewaehlte Quad-Hoehe. */
    {
        static int s_init = 0; static FILE *s_lg = NULL;
        if (!s_init) {
            s_init = 1;
            const char *p = getenv("RE15_HUNDESCHATTEN_LOG");
            if (p && *p) s_lg = fopen(p, "w");
        }
        if (s_lg) {
            fprintf(s_lg, "F%u t=%02x st=%d ss1=%d ss2=%d koerper_y=%d boden_y=%d schatten_y=%d luft=%d\n",
                    (unsigned)g_engine.frame_count, (unsigned)e->type, (int)e->state,
                    (int)e->sub_state_1, (int)e->sub_state_2, (int)e->y, (int)y, (int)y,
                    (int)(e->y < y));
            fflush(s_lg);
        }
    }
#endif
    return y;
}
