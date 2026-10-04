/**
 * @file probe_r35_raeume_trage_los.c
 * @brief Runde 35 Spur H, Nachbesserung 1 (M1) — MESS-SONDE (kein add_test): welche Zelle bricht die
 *        RE2-Sichtlinie des Bahren-Zombies an der Kante von ROOM1200?
 *
 * Gemessen (Lauf m1_D_vorher, RE15_GEGNER_Y_LOG): Slot 2 steht nach Sub 14 bei (-24049,-1800,-17373)
 * Band 1 in Sub 0 P1 und hat los=0 in JEDEM Bild — der Weck-Ausgang (+0x154&0x800) feuert nie.
 * Die Sonde zerlegt re15_re2_los_clear in seine Teile (Zell-Strahl auf dem Gegnerband, RE1.5-Ray
 * FUN_8003dcc4 je Region) und nennt die blockierende Zelle.
 */
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_ai_flavor.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

int main(int argc, char **argv)
{
    long sz = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1200.RDT", &sz);
    if (!buf) { printf("ROOM1200.RDT fehlt\n"); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) { printf("parse\n"); return 1; }
    g_room_rdt = rdt; g_room_rdt_ok = 1;

    printf("Zellen (Index x z w d floor u0 u1 type):\n");
    for (int i = 0; i < rdt.sca_count; i++) {
        const re15_sca_entry_t *c = &rdt.sca[i];
        printf("  %2d x %6d..%6d z %6d..%6d fl %02x u0 %02x u1 %02x ty %02x\n", i, c->x, c->x + c->width,
               c->z, c->z + c->density, c->floor, c->u0, c->u1, c->type);
    }
    re15_actor_t e = {0}, pl = {0};
    e.active = 1; e.type = 0x10; e.floor = 1; e.y = -1800; e.x = -24049; e.z = -17373; e.sca_mask = 4;
    if (argc >= 3) { e.x = atoi(argv[1]); e.z = atoi(argv[2]); }
    pl.active = 1; pl.floor = 0; pl.y = 0;
    static const int32_t P[][2] = {
        {-25788, -16450}, {-25880, -16450}, {-22611, -16581}, {-24049, -16000}, {-24049, -15000},
        {-23000, -16800}, {-25000, -16800}, {-24300, -12500}, {-22685, -17314},
    };
    for (unsigned k = 0; k < sizeof P / sizeof P[0]; k++) {
        pl.x = P[k][0]; pl.z = P[k][1];
        int band = re15_collision_band_from_y(e.y);
        int zell = re15_re2_los_cells_blocked(&rdt, e.x, e.z, pl.x, pl.z, band, 4u);
        int welche = -1;
        if (zell) {
            for (int i = 0; i < rdt.sca_count; i++) {
                re15_rdt_t one = rdt; one.sca = &rdt.sca[i]; one.sca_count = 1;
                if (re15_re2_los_cells_blocked(&one, e.x, e.z, pl.x, pl.z, band, 4u)) { welche = i; break; }
            }
        }
        int frei = re15_re2_los_clear(&e, &pl);
        /* Region-Ray FUN_8003dcc4 (static im Port): je Zelle ausknipsen (Nibble != 3) und sehen, ob
         * die Sicht frei wird -> die Zelle war der (einzige) Blocker. */
        char wer[256] = ""; int nw = 0;
        if (!frei && !zell) {
            static re15_sca_entry_t z2[256];
            for (int i = 0; i < rdt.sca_count && i < 256; i++) {
                for (int j = 0; j < rdt.sca_count && j < 256; j++) z2[j] = rdt.sca[j];
                z2[i].floor = (uint8_t)(z2[i].floor & 0xf0);          /* (w5&0xf00) != 0x300 */
                g_room_rdt.sca = z2;
                if (re15_re2_los_clear(&e, &pl))
                    nw += snprintf(wer + nw, sizeof wer - (size_t)nw, " %d(u0 %02x u1 %02x)", i,
                                   rdt.sca[i].u0, rdt.sca[i].u1);
                g_room_rdt.sca = rdt.sca;
            }
        }
        printf("Region-Ray-Blocker:%s | ", wer);
        printf("Zombie (%d,%d) b%d -> Spieler (%d,%d) b0: Zell-Strahl Band %d %s (Zelle %d), los_clear=%d\n",
               e.x, e.z, e.floor, pl.x, pl.z, band, zell ? "BLOCKT" : "frei", welche, frei);
    }
    free(buf);
    return 0;
}
