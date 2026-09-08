/* probe_sichtregion.c - Nutzer-Frage 2026-09-08:
 *   "Wenn ich Raeume mit Gegnern habe und die Gegner sind quasi um die Wandecke - muss ich
 *    dann immer ein PRI fuer die Wand mitliefern, damit sie abgedeckt sind?"
 *
 * Die Engine hat ZWEI Stufen:
 *   1. Das REGIONS-VIERECK je Kamerawinkel (RVD-Ankerzone, byte-true FUN_80039CA0 ->
 *      FUN_80014368): ein Aktor, dessen Welt-XZ ausserhalb liegt, wird GAR NICHT gezeichnet.
 *   2. Die PRI-Masken: innerhalb der sichtbaren Region verdecken sie, was ferner ist.
 *
 * Diese Sonde misst Stufe 1: wie viele Winkel haben ueberhaupt ein Viereck, und welchen
 * Anteil der begehbaren Flaeche des Raums deckt es ab? Was ausserhalb liegt, braucht KEINE
 * Maske - dort ist der Gegner schon weggeschnitten.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_collision.h"
#include "re15_aot.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

int main(void)
{
    static const char *raeume[] = {
        "ROOM1000","ROOM1010","ROOM1020","ROOM1030","ROOM1040","ROOM1050","ROOM1060",
        "ROOM1070","ROOM1080","ROOM1090","ROOM10A0","ROOM10B0","ROOM10C0","ROOM10D0",
        "ROOM10E0","ROOM10F0","ROOM1100","ROOM1110","ROOM1120","ROOM1130","ROOM1140",
        "ROOM1150","ROOM1160","ROOM1170","ROOM1180","ROOM1190","ROOM11A0","ROOM11B0",
        "ROOM11C0","ROOM1200","ROOM1210","ROOM1220","ROOM1230","ROOM1240" };
    long cuts = 0, mit_quad = 0;
    long summe_anteil = 0, gezaehlt = 0;
    printf("Regions-Viereck je Kamerawinkel (Stufe 1 der Verdeckung)\n");
    printf("%-9s %5s %5s   %s\n", "Raum", "Cuts", "Quad", "mittlerer Anteil der begehbaren Flaeche im Viereck");
    for (unsigned r = 0; r < sizeof raeume / sizeof raeume[0]; r++) {
        char path[600];
        snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", RE15_XSTR(RE15_ASSETS_PATH), raeume[r]);
        size_t size = 0;
        uint8_t *data = read_file(path, &size);
        if (!data) continue;
        re15_rdt_t rdt;
        if (re15_rdt_parse(data, size, &rdt) != 0) { free(data); continue; }
        int32_t X0 = 32000, X1 = -32000, Z0 = 32000, Z1 = -32000;
        for (int i = 0; i < rdt.sca_count; i++) {
            const re15_sca_entry_t *c = &rdt.sca[i];
            if ((int32_t)c->x < X0) X0 = c->x;
            if ((int32_t)c->x + c->width  > X1) X1 = (int32_t)c->x + c->width;
            if ((int32_t)c->z < Z0) Z0 = c->z;
            if ((int32_t)c->z + c->density > Z1) Z1 = (int32_t)c->z + c->density;
        }
        if (X1 <= X0 || Z1 <= Z0) { free(data); continue; }
        int rc = 0, rq = 0; long rsum = 0, rn = 0;
        for (int c = 0; c < rdt.cut_count; c++) {
            int16_t xs[4], zs[4];
            rc++; cuts++;
            if (!re15_rdt_get_region_quad(&rdt, c, xs, zs)) continue;
            rq++; mit_quad++;
            long drin = 0, alle = 0;
            for (int32_t x = X0; x <= X1; x += 500)
            for (int32_t z = Z0; z <= Z1; z += 500) {
                alle++;
                if (re15_aot_point_in_quad(x, z, xs, zs)) drin++;
            }
            if (alle) { rsum += 100 * drin / alle; rn++; summe_anteil += 100 * drin / alle; gezaehlt++; }
        }
        printf("%-9s %5d %5d   %ld %%\n", raeume[r], rc, rq, rn ? rsum / rn : 0);
        free(data);
    }
    printf("\nGESAMT: %ld Winkel, davon %ld mit Regions-Viereck (%.0f %%); "
           "im Mittel %ld %% der Raumflaeche darin.\n",
           cuts, mit_quad, cuts ? 100.0 * mit_quad / cuts : 0.0,
           gezaehlt ? summe_anteil / gezaehlt : 0);
    printf("Was AUSSERHALB liegt, wird gar nicht gezeichnet - dort braucht es KEINE Maske.\n");
    return 0;
}
