/* probe_lanee_reach_1060.c — LANE E MESSSONDE (kein ctest)
 *
 * FRAGE: Ist der Kamera-Ruecksprung, der die LETZTE Treppe blockiert, im
 * ORIGINAL ueberhaupt ERREICHBAR — d.h. kann ein Spieler auf dem Band, auf dem
 * er nach der vorletzten Treppe landet, in den RVD-Streifen laufen, der den Cut
 * ZURUECK schaltet?
 *
 * Der Kamera-Zonen-Scan FUN_80014230 (@0x8001ccec jeden Frame) ist reine
 * RDT-DATEN + Punkt-in-Quad (FUN_80014368 @0x80014298). Port und Original
 * waehlen identisch (probe_rvd_diff_10a0: 0 Abweichungen). Also entscheidet
 * ALLEIN die begehbare Flaeche, ob eine Zone im Original erreichbar ist.
 *
 * Diese Sonde macht eine FLUTFUELLUNG der begehbaren Flaeche pro Band
 * (Freiheitstest = re15_collision_constrain liefert den Punkt unveraendert
 * zurueck) ab dem gemessenen Treppen-Landepunkt, und meldet:
 *   - welche RVD-Zonen die erreichbare Flaeche schneidet,
 *   - ob der Startpunkt der NAECHSTEN Treppe erreichbar ist,
 *   - ob JEDER Weg dorthin durch eine Rueckschalt-Zone fuehrt (dann waere der
 *     Bruch ZWANGSLAEUFIG = Original-Quirk) oder ob es einen Weg gibt, der sie
 *     meidet (dann ist der Bruch pfad-abhaengig = "teilweise").
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_stair.h"
#include "re15_aot.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static re15_rdt_t g_rdt;

#define GX0 17000
#define GZ0 15500
#define GW  130      /* 130 * 100 = 13000 Einheiten */
#define GH  140
#define CELL 100

static uint8_t g_free[GH][GW];
static uint8_t g_seen[GH][GW];
static int16_t g_qx[GH*GW], g_qz[GH*GW];

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

static int is_free(int32_t x, int32_t z)
{
    int32_t cx = x, cz = z;
    re15_collision_constrain(&g_rdt, x, z, &cx, &cz);
    return (cx == x && cz == z);
}

static void build_free(int band)
{
    re15_collision_set_band(band);
    for (int r = 0; r < GH; r++)
        for (int c = 0; c < GW; c++)
            g_free[r][c] = (uint8_t)is_free(GX0 + c*CELL, GZ0 + r*CELL);
}

/* Flutfuellung; `avoid` = Zonenindex, der als BLOCKIERT gilt (-1 = keiner). */
static int flood(int32_t sx, int32_t sz, int avoid, int *out_cells)
{
    memset(g_seen, 0, sizeof g_seen);
    int c0 = (int)((sx - GX0) / CELL), r0 = (int)((sz - GZ0) / CELL);
    if (c0 < 0 || c0 >= GW || r0 < 0 || r0 >= GH) return 0;
    int head = 0, tail = 0, cells = 0;
    g_seen[r0][c0] = 1; g_qx[tail] = (int16_t)c0; g_qz[tail] = (int16_t)r0; tail++;
    static const int dc[4] = {1,-1,0,0}, dr[4] = {0,0,1,-1};
    while (head < tail) {
        int c = g_qx[head], r = g_qz[head]; head++; cells++;
        for (int k = 0; k < 4; k++) {
            int nc = c + dc[k], nr = r + dr[k];
            if (nc < 0 || nc >= GW || nr < 0 || nr >= GH) continue;
            if (g_seen[nr][nc] || !g_free[nr][nc]) continue;
            if (avoid >= 0) {
                const re15_rdt_zone_t *z = &g_rdt.zones[avoid];
                if (re15_aot_point_in_quad(GX0 + nc*CELL, GZ0 + nr*CELL, z->xs, z->zs))
                    continue;
            }
            g_seen[nr][nc] = 1; g_qx[tail] = (int16_t)nc; g_qz[tail] = (int16_t)nr; tail++;
        }
    }
    if (out_cells) *out_cells = cells;
    return 1;
}

static int reached(int32_t x, int32_t z)
{
    int c = (int)((x - GX0) / CELL), r = (int)((z - GZ0) / CELL);
    if (c < 0 || c >= GW || r < 0 || r >= GH) return 0;
    return g_seen[r][c];
}

static void report_band(const char *label, int band, int32_t sx, int32_t sz,
                        int32_t tx, int32_t tz, const char *tlabel)
{
    printf("\n=== %s  (Band %d)  Start=(%ld,%ld)  Ziel=%s (%ld,%ld)\n",
           label, band, (long)sx, (long)sz, tlabel, (long)tx, (long)tz);
    build_free(band);
    if (!is_free(sx, sz)) {
        /* Startpunkt liegt in einer Wandzelle (typischer Treppen-Landepunkt) —
         * naechste freie Zelle im 500er-Umkreis suchen. */
        int32_t bx = sx, bz = sz; int found = 0;
        for (int r = 1; r <= 8 && !found; r++)
            for (int dz = -r; dz <= r && !found; dz++)
                for (int dx = -r; dx <= r && !found; dx++) {
                    if (dx*dx + dz*dz < (r-1)*(r-1)) continue;
                    int32_t px = sx + dx*CELL, pz = sz + dz*CELL;
                    if (is_free(px, pz)) { bx = px; bz = pz; found = 1; }
                }
        printf("    (Startpunkt nicht frei -> naechste freie Zelle (%ld,%ld))\n",
               (long)bx, (long)bz);
        sx = bx; sz = bz;
    }
    int cells = 0;
    flood(sx, sz, -1, &cells);
    printf("    erreichbare Zellen (100u-Raster): %d\n", cells);
    printf("    Ziel erreichbar: %s   (Ziel frei? %s)\n",
           reached(tx, tz) ? "JA" : "NEIN", is_free(tx, tz) ? "ja" : "nein");
    {
        int32_t mnx=0,mxx=0,mnz=0,mxz=0,first=1;
        for (int r = 0; r < GH; r++) for (int c = 0; c < GW; c++) if (g_seen[r][c]) {
            int32_t px = GX0 + c*CELL, pz = GZ0 + r*CELL;
            if (first) { mnx=mxx=px; mnz=mxz=pz; first=0; }
            if (px<mnx)mnx=px; if (px>mxx)mxx=px; if (pz<mnz)mnz=pz; if (pz>mxz)mxz=pz;
        }
        printf("    erreichbare Huelle: x[%ld..%ld] z[%ld..%ld]\n",
               (long)mnx,(long)mxx,(long)mnz,(long)mxz);
        /* ASCII-Karte: '#'=Wand '.'=frei-unerreichbar 'o'=erreichbar 'S'=Start 'T'=Ziel */
        printf("    Karte (Spalte = x %d..%d in 200er, Zeile = z %d..%d in 200er):\n",
               GX0, GX0+(GW-1)*CELL, GZ0, GZ0+(GH-1)*CELL);
        for (int r = 0; r < GH; r += 2) {
            printf("    z=%5d ", GZ0 + r*CELL);
            for (int c = 0; c < GW; c += 2) {
                int32_t px = GX0 + c*CELL, pz = GZ0 + r*CELL;
                char ch = g_free[r][c] ? (g_seen[r][c] ? 'o' : '.') : '#';
                if (px/200 == sx/200 && pz/200 == sz/200) ch = 'S';
                if (px/200 == tx/200 && pz/200 == tz/200) ch = 'T';
                putchar(ch);
            }
            putchar('\n');
        }
    }

    printf("    RVD-Zonen, die die erreichbare Flaeche schneiden:\n");
    for (int i = 0; i < g_rdt.zone_count; i++) {
        const re15_rdt_zone_t *z = &g_rdt.zones[i];
        int32_t mnx=z->xs[0],mxx=z->xs[0],mnz=z->zs[0],mxz=z->zs[0];
        for (int k=1;k<4;k++){ if(z->xs[k]<mnx)mnx=z->xs[k]; if(z->xs[k]>mxx)mxx=z->xs[k];
                               if(z->zs[k]<mnz)mnz=z->zs[k]; if(z->zs[k]>mxz)mxz=z->zs[k]; }
        if (mxx - mnx > 8000) continue;   /* Anker-Zone (ganzer Raum) uebergehen */
        int hit = 0;
        for (int r = 0; r < GH && !hit; r++)
            for (int c = 0; c < GW && !hit; c++)
                if (g_seen[r][c] &&
                    re15_aot_point_in_quad(GX0 + c*CELL, GZ0 + r*CELL, z->xs, z->zs))
                    hit = 1;
        if (hit)
            printf("      RVD[%2d] %d->%-2d  x[%ld..%ld] z[%ld..%ld]\n",
                   i, (int)z->cam_from, (int)z->cam_to,
                   (long)mnx, (long)mxx, (long)mnz, (long)mxz);
    }
}

static void report_avoid(const char *label, int band, int32_t sx, int32_t sz,
                         int32_t tx, int32_t tz, int avoid_zone)
{
    build_free(band);
    if (!is_free(sx, sz)) {
        int32_t bx = sx, bz = sz; int found = 0;
        for (int r = 1; r <= 8 && !found; r++)
            for (int dz = -r; dz <= r && !found; dz++)
                for (int dx = -r; dx <= r && !found; dx++) {
                    if (dx*dx + dz*dz < (r-1)*(r-1)) continue;
                    int32_t px = sx + dx*CELL, pz = sz + dz*CELL;
                    if (is_free(px, pz)) { bx = px; bz = pz; found = 1; }
                }
        sx = bx; sz = bz;
    }
    int cells = 0;
    flood(sx, sz, avoid_zone, &cells);
    printf("    %s: Ziel ohne RVD[%d] erreichbar? %s  (%d Zellen)\n",
           label, avoid_zone, reached(tx, tz) ? "JA — der Bruch ist PFADABHAENGIG"
                                              : "NEIN — der Bruch ist ZWANGSLAEUFIG", cells);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t size = 0;
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1060.RDT", base);
    uint8_t *data = read_file(path, &size);
    if (!data) { fprintf(stderr, "FAIL: %s\n", path); return 1; }
    if (re15_rdt_parse(data, size, &g_rdt) != 0) { fprintf(stderr, "FAIL parse\n"); return 1; }

    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1060;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 26000; pl->y = -8 * 0x708; pl->z = 25300;
    scd_register_room_events(&g_rdt);
    scd_room_reenter(&g_rdt, 0, 0, 0);
    for (int f = 0; f < 10; f++) scd_vm_tick();

    /* PLAUSIBILITAETS-CHECK des Kollisions-Modells: die Tuer-Spawnpunkte MUESSEN
     * begehbar sein. ROOM1120 Door slot0 @0x0c76 -> Spawn (26000,-14400,25300)
     * = Band 8; ROOM1060 Door slot2 @0x0d52 Band 0 (Rechteck 26600,24300,1000,2200). */
    {
        re15_collision_set_band(8);
        printf("\nKollisions-Plausibilitaet: Tuer-Spawn (26000,25300) Band 8 begehbar? %s\n",
               is_free(26000, 25300) ? "JA" : "NEIN");
        re15_collision_set_band(0);
        printf("Kollisions-Plausibilitaet: Ausgangstuer-Mitte (27100,25400) Band 0 begehbar? %s\n",
               is_free(27100, 25400) ? "JA" : "NEIN");
    }

    /* Band 6: Landepunkt Treppe 1 (25162,19532) -> Start Treppe 2 (21400,21500). */
    report_band("Band 6 nach Treppe 1", 6, 25162, 19532, 21400, 21500, "Treppe-2-Start");
    report_avoid("Band 6", 6, 25162, 19532, 21400, 21500, 3);   /* RVD[3] 1->0 */

    /* Band 4: Landepunkt Treppe 2 (21388,25368) -> Start Treppe 3 (25150,23500). */
    report_band("Band 4 nach Treppe 2", 4, 21388, 25368, 25150, 23500, "Treppe-3-Start");
    report_avoid("Band 4", 4, 21388, 25368, 25150, 23500, 6);   /* RVD[6] 2->1 */

    /* Band 2: Landepunkt Treppe 3 (25162,19532) -> Start Treppe 4 (21400,21500).
     * RVD[12] 4->3 x[23000..27500] z[21000..22000] ist die Zone, die den Cut
     * ZURUECK auf 3 schaltet und damit RVD[13] (4->5) fuer die LETZTE Treppe
     * tot legt. */
    report_band("Band 2 nach Treppe 3 (KRITISCH)", 2, 25162, 19532, 21400, 21500, "Treppe-4-Start");
    report_avoid("Band 2", 2, 25162, 19532, 21400, 21500, 12);  /* RVD[12] 4->3 */

    /* Band 0: Landepunkt Treppe 4 (21388,25368) -> Ausgangstuer (26600..27600, 24300..26500). */
    report_band("Band 0 nach Treppe 4", 0, 21388, 25368, 27100, 25400, "Ausgangstuer");
    report_avoid("Band 0", 0, 21388, 25368, 27100, 25400, 15);  /* RVD[15] 5->4 */
    return 0;
}
