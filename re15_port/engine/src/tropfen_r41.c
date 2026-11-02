/*
 * tropfen_r41.c — Runde 41 Spur W, OFFEN 4 entschieden (Variante B, wie die 40%-Disc): Die Deckentropfen-Schleife in ROOM2000/2001
 * startet nicht. Belege, Bytes und Eingrenzung stehen in include/re15_tropfen_r41.h, das Dossier ist analysis/befunde_runde41/W_wasserblink.md (OFFEN 4).
 */
#include "re15_tropfen_r41.h"

#include <stddef.h>
#include <stdio.h>

#ifdef RE15_PLATFORM_PC
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif

static unsigned s_zaehler = 0;

/* Erste Zeile der Tropfen-Schleife: Sce_espr_on Effekt 6 Sub 0 Skala 0x1000 (sub07 @0x01B06 / sub08 @0x01D32). */
static const uint8_t k_schleife_kopf[8] = { 0x3a, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x10 };

int re15_tropfen_r41_evt_exec_gestrichen(uint16_t room_id, const uint8_t *pc, const re15_rdt_t *rdt)
{
    uint8_t  sub;
    uint32_t ziel_off;
    if (room_id == RE15_R41W_RAUM_2000)      { sub = RE15_R41W_SUB_2000; ziel_off = RE15_R41W_SUB_OFF_2000; }
    else if (room_id == RE15_R41W_RAUM_2001) { sub = RE15_R41W_SUB_2001; ziel_off = RE15_R41W_SUB_OFF_2001; }
    else return 0;
    if (!pc || !rdt || !rdt->raw || !rdt->main_scd || rdt->raw_size <= 0) return 0;
    const uint8_t *raw = rdt->raw;
    const size_t   n   = (size_t)rdt->raw_size;
    /* Skript: main00 beginnt bei 0x0161E, und pc liegt in main00. */
    if ((size_t)(rdt->main_scd - raw) != RE15_R41W_MAIN00_OFF) return 0;
    if (pc < rdt->main_scd || (size_t)(pc - rdt->main_scd) >= rdt->main_scd_size) return 0;
    /* Stelle und Bytes: @0x0177C `04 ff 18 07` (2000) bzw. `04 ff 18 08` (2001). */
    if ((size_t)(pc - raw) != RE15_R41W_EVT_OFF || RE15_R41W_EVT_OFF + 4u > n) return 0;
    if (pc[0] != 0x04 || pc[1] != 0xFF || pc[2] != 0x18 || pc[3] != sub) return 0;
    /* Ziel: sub07 @0x01B06 bzw. sub08 @0x01D32 beginnt mit der Tropfen-Zeile. */
    if (sub >= RE15_RDT_MAX_SUB_SCD || !rdt->sub_scd[sub]) return 0;
    if ((size_t)(rdt->sub_scd[sub] - raw) != ziel_off || ziel_off + sizeof k_schleife_kopf > n) return 0;
    for (size_t i = 0; i < sizeof k_schleife_kopf; i++)
        if (rdt->sub_scd[sub][i] != k_schleife_kopf[i]) return 0;
    s_zaehler++;
    LOG("[tropfen41] ROOM%04X main00 @0x%05X Evt_exec sub%02u (Deckentropfen Effekt 6 Sub 0 @0x%05X) ausgelassen, wie Version_40%% "
        "(main00 endet @0x01598 01 00)\n", (unsigned)room_id, (unsigned)RE15_R41W_EVT_OFF, (unsigned)sub, (unsigned)ziel_off);
    return 1;
}

unsigned re15_tropfen_r41_zaehler(void) { return s_zaehler; }
void re15_tropfen_r41_reset(void) { s_zaehler = 0; }
