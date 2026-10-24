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

extern int  re15_player_equipped_weapon(void);   /* re15_damage.c (DAT_800aca5d) */
extern void re15_player_waffe_roh(int id);       /* re15_damage.c, Runde 41 Spur K: aca5d ohne Ausruest-Platz */

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

static int s_modell = -1;
static int s_waffe_gemerkt = -1;

int  re15_sherry_r41_modell(void) { return s_modell; }

void re15_sherry_r41_modell_init(int modell)
{
    s_modell = modell & 0x0F;
    s_waffe_gemerkt = -1;
}

void re15_sherry_r41_modell_setzen(int modell)
{
    int alt = s_modell;
    s_modell = modell & 0x0F;
    if (s_modell == RE15_SHERRY_R41_MODELL && alt != RE15_SHERRY_R41_MODELL) {
        /* @0x800316b8 `bne v1,v0(=0xf)` / @0x800316c4 `sb zero,DAT_800aca5d`: Modell 0x0F -> Waffe 0 */
        s_waffe_gemerkt = re15_player_equipped_weapon();
        re15_player_waffe_roh(0);
    } else if (s_modell != RE15_SHERRY_R41_MODELL && alt == RE15_SHERRY_R41_MODELL && s_waffe_gemerkt >= 0) {
        re15_player_waffe_roh(s_waffe_gemerkt);   /* PORT-WAHL (Header): vorherige Waffe zurueck */
        s_waffe_gemerkt = -1;
    }
    fprintf(stderr, "[sherry-r41] Spielermodell %d -> %d, Waffe (aca5d) %d\n", alt, s_modell,
            re15_player_equipped_weapon());
}

int re15_sherry_r41_waffe_gesperrt(void) { return s_modell == RE15_SHERRY_R41_MODELL; }

/* Zeile 15 der vier Tabellen [Tempo, Drehung], Byte-Index 15*2 = +30 (aus info/Re1.5/PSX.EXE gelesen):
 *   GEHEN  @0x80073ec2 = 32 30 -> [50,48]   Leser Tempo @0x800324d4, Drehung (+1) @0x800323a0
 *   RUECK  @0x80073ee2 = 23 30 -> [35,48]   Leser Tempo @0x80032d5c, Drehung (+1) @0x80032c4c
 *   DREHEN @0x80073f02 = 00 50 -> [0,80]    Leser Drehung (+1) @0x80032a08
 *   RENNEN @0x80073f42 = 78 40 -> [120,64]  Leser Tempo @0x800327e8, Drehung (+1) @0x80032688
 * (Zum Vergleich Zeile 0 Leon: [75,48] [70,48] [0,96] [200,72]; Zeile 4 Elza: [70,48] [65,48] [0,96] [210,72].) */
static const uint8_t s_zeile15[4][2] = { { 50, 48 }, { 35, 48 }, { 0, 80 }, { 120, 64 } };

int re15_sherry_r41_tabelle(int tab, int feld, int standard)
{
    if (s_modell != RE15_SHERRY_R41_MODELL) return standard;
    if (tab < 0 || tab > 3 || feld < 0 || feld > 1) return standard;
    return (int)s_zeile15[tab][feld];
}
