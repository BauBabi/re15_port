/*
 * wasserlauf_r41.c — Runde 41 Spur N "nachtrag30": Wasser die Leiter von ROOM2000/2001 herunter und ueber den Weg zum Kanal, bis zur
 * Flut in 2030 (AUFTRAG.md Nachtrag 30e). Belege, Lage und Port-Wahlen: include/re15_wasserlauf_r41.h, Dossier N_nachtrag30.md §7.
 */
#include "re15_wasserlauf_r41.h"

#include <stdint.h>
#include <stdio.h>
#include "re15_esp.h"            /* re15_esp_fx_spawn_rows = FUN_80019700, re15_esp_room_bank */
#include "re15_room.h"           /* g_current_room_id                                         */
#include "re15_scd.h"            /* re15_game_flag_get, RE15_PAUSE_ACTION                     */
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

/* Lauf: { x, z, Sub }. Flach von der Leiter bis vor die Kante (13 Bilder x 100 = 1300), kurz vor der Kante die fallenden. */
static const struct { int32_t x, z; uint8_t sub; } k_lauf[] = {
    { -26650, 9200, RE15_R41N_LAUF_SUB_FLACH }, { -26650, 9500, RE15_R41N_LAUF_SUB_FLACH }, { -26650, 9800, RE15_R41N_LAUF_SUB_FLACH },
    { -25500, 9350, RE15_R41N_LAUF_SUB_FALL },  { -25500, 9650, RE15_R41N_LAUF_SUB_FALL },
};
#define LAUF_N ((int)(sizeof k_lauf / sizeof k_lauf[0]))
static const int16_t k_lauf_z[5] = { 0, 130, -90, 60, -130 };    /* PORT-WAHL: Versatz quer zum Lauf je Spawn */
static const uint8_t k_lauf_abstand[8] = { 3, 6, 4, 8, 3, 5, 7, 4 };   /* PORT-WAHL: Bilder bis zum naechsten Spawn eines Punkts, Mittel 5. Ein fester Takt ergaebe ein stehendes Muster: gemessen wiederholte sich das Bild alle 4 Bilder (Korrelation 0,89) */

/* Einschlaege im Wasser des Kanals (y -900 @0x016E6). */
static const int32_t k_ring[][3] = { { -24800, RE15_R41N_KANAL_Y, 9350 }, { -24800, RE15_R41N_KANAL_Y, 9650 } };
#define RING_N ((int)(sizeof k_ring / sizeof k_ring[0]))

static int      s_aktiv = 0, s_strahlen = 0, s_takt = 0;
static unsigned s_spritzer = 0, s_laeufer = 0;
static uint8_t  s_warte[LAUF_N], s_schritt[LAUF_N];

void re15_wasserlauf_r41_reset(void)
{
    s_aktiv = 0; s_strahlen = 0; s_takt = 0; s_spritzer = 0; s_laeufer = 0;
    for (int i = 0; i < LAUF_N; i++) { s_warte[i] = (uint8_t)(1 + i); s_schritt[i] = (uint8_t)(3 * i); }
}

int re15_wasserlauf_r41_soll(uint16_t room_id)
{
    /* Ende: dieselbe Bedingung wie die Sperre der Leiter (install_2000), plus Flut gelaufen. */
    return (room_id & 0xFFFEu) == RE15_R41C_RAUM_2000 && !re15_game_flag_get(RE15_R41C_BANK, RE15_R41C_BIT_LEITER_FREI) &&
           !re15_game_flag_get(RE15_R41C_BANK, RE15_R41C_BIT_FLUT_GELAUFEN);
}

void re15_wasserlauf_r41_install(uint16_t room_id)
{
    re15_wasserlauf_r41_reset();
    if (!re15_wasserlauf_r41_soll(room_id)) return;
    s_aktiv = 1;
    for (int i = 0; i < STRAHLEN_N; i++)
        if (re15_esp_fx_spawn_rows(re15_esp_room_bank(), RE15_R41N_ESP_STRAHL, 0, (uint16_t)k_strahlen[i][3], k_strahlen[i][0],
                                   k_strahlen[i][1], k_strahlen[i][2], k_strahlen[i][1], RE15_R41N_ESP_PARAM) > 0)
            s_strahlen++;
    LOG("[wasserlauf41] ROOM%04X: %d Strahlen, Lauf ueber den Weg Effekt 0x%02X\n", (unsigned)room_id, s_strahlen, RE15_R41N_ESP_LAUF);
}

void re15_wasserlauf_r41_tick(void)
{
    if (!s_aktiv || (g_re15_pauseflags & RE15_PAUSE_ACTION)) return;     /* steht wie der Effekt-Takt selbst (@0x80019e28), sonst liefe der Pool unter einem Text voll */
    s_takt++;
    for (int i = 0; i < LAUF_N; i++) {
        if (--s_warte[i]) continue;
        s_warte[i] = k_lauf_abstand[s_schritt[i]++ & 7];
        if (re15_esp_fx_spawn_rows(re15_esp_room_bank(), RE15_R41N_ESP_LAUF, k_lauf[i].sub, RE15_R41N_LAUF_SKALA, k_lauf[i].x, RE15_R41N_LAUF_HOEHE, k_lauf[i].z + k_lauf_z[s_laeufer % 5],
                                   k_lauf[i].sub == RE15_R41N_LAUF_SUB_FALL ? RE15_R41N_KANAL_Y : INT32_MAX, 0) > 0)
            s_laeufer++;
    }
    if ((s_takt % RE15_R41N_TAKT_RING) == 0) {
        const int i = (s_takt / RE15_R41N_TAKT_RING) % RING_N;
        if (re15_esp_fx_spawn_rows(re15_esp_room_bank(), RE15_R41N_ESP_WASSER, RE15_R41N_ESP_RING_SUB, RE15_R41N_ESP_RING_SKALA, k_ring[i][0],
                                   k_ring[i][1], k_ring[i][2], INT32_MAX, 0) > 0)
            s_spritzer++;
    }
}

int      re15_wasserlauf_r41_aktiv(void)    { return s_aktiv; }
int      re15_wasserlauf_r41_strahlen(void) { return s_strahlen; }
unsigned re15_wasserlauf_r41_spritzer(void) { return s_spritzer; }
unsigned re15_wasserlauf_r41_laeufer(void)  { return s_laeufer; }
const int32_t *re15_wasserlauf_r41_strahl(int i) { return (i >= 0 && i < STRAHLEN_N) ? k_strahlen[i] : NULL; }
