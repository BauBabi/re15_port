/*
 * wasserlauf_r41.c — Runde 41 Spur N "nachtrag30": Wasser die Leiter von ROOM2000/2001 herunter und ueber den Weg zum Kanal, bis zur
 * Flut in 2030 (AUFTRAG.md Nachtrag 30e). Belege, Lage und Port-Wahlen: include/re15_wasserlauf_r41.h, Dossier N_nachtrag30.md §7.
 */
#include "re15_wasserlauf_r41.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "re15_esp.h"            /* re15_esp_fx_spawn_rows = FUN_80019700, re15_esp_room_bank */
#include "re15_room.h"           /* g_current_room_id                                         */
#include "re15_scd.h"            /* g_scd.props, re15_game_flag_get                           */
#include "re15_ventilkette_r41.h"

#ifdef RE15_PLATFORM_PC
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

/* Dauer-Strahlen: { x, y, z, Skala }. Die Teilchen des Effekts 0x0B fallen in Welt-y und folgen damit von selbst der Leiter. Gemessen (Differenz zum
 * Hintergrund Cut 9/10, Anker y -6300, Skala 0x0C00): Ende y -3000, also 3300 lang (am Gitter 4500 bei Skala 0x1900). Zwei Spalten zwischen den
 * Holmen (z 9034 / 9945), drei Hoehen: Anker y -6900 = Schacht-Unterkante, y -5100 = Fuss -1800 minus 3300, y -6000 dazwischen. Skala 0x0C00 = PORT-WAHL
 * (schmaler als das Gitter, die Leiter ist 900 breit). */
static const int32_t k_strahlen[][4] = {
    { -26760, -6900, 9300, 0x0C00 }, { -26760, -6900, 9700, 0x0C00 },
    { -26750, -6000, 9300, 0x0C00 }, { -26750, -6000, 9700, 0x0C00 },
    { -26730, -5100, 9300, 0x0C00 }, { -26730, -5100, 9700, 0x0C00 },
};
#define STRAHLEN_N ((int)(sizeof k_strahlen / sizeof k_strahlen[0]))

/* Spritzer: { x, y, z }, wechselnd von der Leiter ueber den Weg bis ins Wasser des Kanals (y -900 @0x016E6). */
static const int32_t k_ring[][3]   = { { -26300, RE15_R41N_LAUF_Y, 9700 }, { -25900, RE15_R41N_LAUF_Y, 9300 }, { -25500, RE15_R41N_LAUF_Y, 9700 },
                                       { -25050, RE15_R41N_KANAL_Y, 9300 }, { -25050, RE15_R41N_KANAL_Y, 9700 } };
static const int32_t k_spritz[][3] = { { -26650, RE15_R41N_LAUF_Y, 9400 }, { -26500, RE15_R41N_LAUF_Y, 9650 }, { -26100, RE15_R41N_LAUF_Y, 9500 },
                                       { -25700, RE15_R41N_LAUF_Y, 9400 }, { -25300, RE15_R41N_LAUF_Y, 9600 } };

static int      s_aktiv = 0, s_strahlen = 0, s_prop = -1, s_takt = 0;
static unsigned s_spritzer = 0;

static int prop_lauf_anlegen(void)
{
    int p = -1;
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].active && g_scd.props[k].obj_id == RE15_R41N_LAUF_OBJ) p = k;
    if (p < 0) {
        if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return -1;
        p = (int)g_scd.prop_count++;
        memset(&g_scd.props[p], 0, sizeof g_scd.props[p]);
        g_scd.props[p].active     = 1;
        g_scd.props[p].obj_id     = RE15_R41N_LAUF_OBJ;
        g_scd.props[p].obj_type   = RE15_R41C_WASSER_TYP;      /* Form der Wasserflaeche ROOM2080 @0x00C2E */
        g_scd.props[p].band       = RE15_R41N_LAUF_BAND;
        g_scd.props[p].parent_obj = -1;
        g_scd.props[p].flags      = RE15_R41C_WASSER_FLAGS;
        g_scd.props[p].attr       = RE15_R41C_WASSER_ATTR;
    }
    g_scd.props[p].x = 0; g_scd.props[p].y = RE15_R41N_LAUF_Y; g_scd.props[p].z = 0;   /* Netz in Weltkoordinaten (Generator) */
    return p;
}

void re15_wasserlauf_r41_reset(void)
{
    s_aktiv = 0; s_strahlen = 0; s_prop = -1; s_takt = 0; s_spritzer = 0;
}

void re15_wasserlauf_r41_install(uint16_t room_id)
{
    re15_wasserlauf_r41_reset();
    if ((room_id & 0xFFFEu) != RE15_R41C_RAUM_2000) return;
    /* Ende: dieselbe Bedingung wie die Sperre der Leiter (install_2000), plus Flut gelaufen. */
    if (re15_game_flag_get(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI) ||
        re15_game_flag_get(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN)) return;
    s_aktiv = 1;
    s_prop = prop_lauf_anlegen();
    for (int i = 0; i < STRAHLEN_N; i++)
        if (re15_esp_fx_spawn_rows(re15_esp_room_bank(), RE15_R41N_ESP_STRAHL, 0, (uint16_t)k_strahlen[i][3], k_strahlen[i][0],
                                   k_strahlen[i][1], k_strahlen[i][2], k_strahlen[i][1], RE15_R41N_ESP_PARAM) > 0)
            s_strahlen++;
    LOG("[wasserlauf41] ROOM%04X: %d Strahlen, nasse Spur Prop %d\n", (unsigned)room_id, s_strahlen, s_prop);
}

void re15_wasserlauf_r41_tick(void)
{
    if (!s_aktiv) return;
    s_takt++;
    if ((s_takt % RE15_R41N_TAKT_RING) == 0) {
        const int i = (s_takt / RE15_R41N_TAKT_RING) % 5;
        if (re15_esp_fx_spawn_rows(re15_esp_room_bank(), RE15_R41N_ESP_WASSER, RE15_R41N_ESP_RING_SUB, RE15_R41N_ESP_RING_SKALA, k_ring[i][0],
                                   k_ring[i][1], k_ring[i][2], INT32_MAX, 0) > 0)
            s_spritzer++;
    }
    if ((s_takt % RE15_R41N_TAKT_SPRITZ) == 0) {
        const int i = (s_takt / RE15_R41N_TAKT_SPRITZ) % 5;
        if (re15_esp_fx_spawn_rows(re15_esp_room_bank(), RE15_R41N_ESP_WASSER, RE15_R41N_ESP_SPRITZ_SUB, RE15_R41N_ESP_SPRITZ_SKALA, k_spritz[i][0],
                                   k_spritz[i][1], k_spritz[i][2], INT32_MAX, 0) > 0)
            s_spritzer++;
    }
}

int      re15_wasserlauf_r41_aktiv(void)    { return s_aktiv; }
int      re15_wasserlauf_r41_strahlen(void) { return s_strahlen; }
unsigned re15_wasserlauf_r41_spritzer(void) { return s_spritzer; }
int      re15_wasserlauf_r41_prop(void)     { return s_prop; }
const int32_t *re15_wasserlauf_r41_strahl(int i) { return (i >= 0 && i < STRAHLEN_N) ? k_strahlen[i] : NULL; }
