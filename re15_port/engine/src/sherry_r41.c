/*
 * RE1.5 Rebuilt — Runde 41 Spur K "stage3szenen_b": Sherry als Spielfigur (Modell 0x0F). Belege: include/re15_sherry_r41.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_sherry_r41.h"
#include "re15_scd.h"
#include "re15_engine.h"
#include "re15_room.h"

void re15_sherry_r41_messhaken(void)
{
    static int s_init = 0, s_done = 0;
    static long s_bild = -1;
    static unsigned s_raum = 0;
    static const char *s_spec = NULL;
    if (!s_init) {
        s_init = 1;
        const char *e = getenv("RE15_WORKVAR_AT");
        if (e && *e) {
            const char *at = strrchr(e, '@');
            if (at) {
                s_bild = atol(at + 1);
                const char *hs = strchr(at, '#');
                if (hs) s_raum = (unsigned)strtoul(hs + 1, NULL, 16);
                s_spec = e;
            }
        }
    }
    if (!s_spec || s_done) return;
    if ((long)g_engine.frame_count < s_bild) return;
    if (s_raum != 0 && g_current_room_id != s_raum) return;
    s_done = 1;
    const char *p = s_spec;
    while (*p && *p != '@') {
        char *q = NULL;
        long idx = strtol(p, &q, 0);
        if (q == p) break;
        p = q;
        while (*p == ':' || *p == ' ') p++;
        long val = strtol(p, &q, 0);
        if (q == p) break;
        p = q;
        if (idx >= 0 && idx < 256) {
            g_scd.work_vars[idx] = (int16_t)val;
            fprintf(stderr, "[workvar-at] Bild %u Raum %04X: work_vars[0x%02lX] = %ld\n",
                    (unsigned)g_engine.frame_count, g_current_room_id, idx, val);
        }
        while (*p == ',' || *p == ' ') p++;
    }
}
