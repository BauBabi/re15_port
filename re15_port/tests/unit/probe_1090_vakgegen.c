/* probe_1090_vakgegen.c - GEGENSONDE (adversariale Pruefung "schwarze-dreiecke", Linse vakuanz).
 *
 * Der Befund hat mit RE15_GOTO_ROOM=1090 + RE15_FORCE_CUT=13 + RE15_PLAYER_POS="2000,800,2048"
 * gemessen: Raumsprung, gepinnte Kamera, Teleport. Diese Sonde misst dieselbe Sache OHNE
 * Zwang, ueber den NATUERLICHEN Weg:
 *   (A) Spawnt der natuerliche Raum-Hochlauf (scd_room_reenter am Tuer-Spawn aus
 *       re15_room_spawns.h: (-10100,-1800,4200) yaw 3072, cut 3) die sieben 0x26-Emitter?
 *   (B) Fuer JEDEN Cut: enthaelt sein Region-Quad (re15_rdt_get_region_quad = das Tor der
 *       NPC-Render-Schleife main.c:6605) die Emitter? -> in welchen Cuts wuerde der Port
 *       das Dreieck ueberhaupt zeichnen?
 *   (C) ERREICHBARKEIT per re15_collision_constrain (NICHT on_floor): Flutfuellung ab dem
 *       Tuer-Spawn ueber das Bewegungsmodell aus game_step_common.c:1284-98.
 *   (D) Kamera-Zustandsmaschine: BFS ueber (Zelle, aktiver Cut) mit der RVD-Regel aus
 *       rdt_common.c:431 (Anker-Skip) + aot_common.c:921 (cam_from-Filter). Welche Cuts
 *       nimmt das Spiel auf einem begehbaren Weg wirklich ein? Ist Cut 13 dabei?
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_collision.h"

extern scd_vm_t g_scd;
extern re15_aot_state_t g_aot;

static re15_rdt_t s_rdt;

static uint8_t *read_file(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t*)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* --- Bewegungsmodell (game_step_common.c:1284-98): Band -> Wandklemme -> Objekte --- */
static int step_ok(int32_t ox, int32_t oz, int32_t tx, int32_t tz, int32_t y)
{
    int32_t nx = tx, nz = tz;
    re15_collision_ensure_band(y);
    re15_collision_constrain(&s_rdt, ox, oz, &nx, &nz);
    re15_collision_objects(&nx, &nz);
    int32_t dx = nx - tx, dz = nz - tz;
    if (dx < 0) dx = -dx;
    if (dz < 0) dz = -dz;
    return (dx <= 12 && dz <= 12);
}

#define GS   100
static int32_t GX0, GZ0;
static int GW, GH;
static uint8_t *g_rea;
static uint8_t *g_rc;
static int32_t *g_q;
#define CX(gx) (GX0 + (gx)*GS)
#define CZ(gz) (GZ0 + (gz)*GS)

static int cut_of_point(int cur_cut, int32_t x, int32_t z)
{
    /* rdt_common.c:431 Anker-Skip + aot_common.c:921 cam_from-Filter, FIRST-in-table */
    for (int i = 0; i < s_rdt.zone_count; i++) {
        const re15_rdt_zone_t *zz = &s_rdt.zones[i];
        if (i == 0 || s_rdt.zones[i-1].cam_from != zz->cam_from) continue;   /* Anker */
        if (zz->cam_from != (uint8_t)cur_cut) continue;
        if (re15_aot_point_in_quad(x, z, zz->xs, zz->zs)) return zz->cam_to;
    }
    return cur_cut;
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1090.RDT";
    size_t sz = 0;
    uint8_t *raw = read_file(path, &sz);
    if (!raw) { printf("FEHLT: %s\n", path); return 77; }
    if (re15_rdt_parse(raw, sz, &s_rdt) < 0) { printf("PARSE FEHLER\n"); return 1; }
    printf("ROOM1090.RDT %u B  nCut=%d zones=%d sca=%d props=%d\n",
           (unsigned)sz, s_rdt.cut_count, s_rdt.zone_count, s_rdt.sca_count, s_rdt.prop_count);

    /* ---------- (A) natuerlicher Raum-Hochlauf ---------- */
    re15_actor_init();
    scd_vm_init();
    g_current_room_id = 0x1090;
    g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -10100; pl->y = -1800; pl->z = 4200; pl->rot_y = 3072;  /* re15_room_spawns.h:28 */
    g_scd.cam_id = 3;                                               /* Spawn-Cut, dieselbe Zeile */
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    re15_collision_ensure_band(pl->y);

    int em_idx[16], nem = 0;
    for (int i = 1; i < RE15_ACTOR_MAX && nem < 16; i++)
        if (g_actors[i].active && g_actors[i].type == 0x26) em_idx[nem++] = i;
    printf("\n=== (A) NATUERLICHER Hochlauf (kein GOTO/FORCE/TELEPORT): 0x26-Aktoren = %d ===\n", nem);
    for (int i = 0; i < nem; i++) {
        re15_actor_t *e = &g_actors[em_idx[i]];
        printf("   slot%2d typ=0x%02X aktiv=%d pos=(%6ld,%6ld,%6ld) yaw=%d\n",
               em_idx[i], e->type, e->active, (long)e->x, (long)e->y, (long)e->z, (int)e->rot_y);
    }
    printf("   g_scd.cam_id nach Hochlauf = %u  work_vars[0x0A]=%d\n",
           g_scd.cam_id, (int)g_scd.work_vars[10]);

    /* ---------- (B) Region-Quad pro Cut ---------- */
    printf("\n=== (B) Region-Quad je Cut (main.c:6605-Tor) - wie viele Emitter drin? ===\n");
    int cut_has_em[64];
    memset(cut_has_em, 0, sizeof cut_has_em);
    for (int c = 0; c < s_rdt.cut_count && c < 64; c++) {
        int16_t xs[4], zs[4];
        if (!re15_rdt_get_region_quad(&s_rdt, c, xs, zs)) {
            printf("   Cut %2d: KEIN Region-Quad -> nichts gecullt\n", c);
            cut_has_em[c] = nem;
            continue;
        }
        int in = 0;
        for (int i = 0; i < nem; i++)
            if (re15_aot_point_in_quad(g_actors[em_idx[i]].x, g_actors[em_idx[i]].z, xs, zs)) in++;
        cut_has_em[c] = in;
        printf("   Cut %2d: quad (%d,%d)(%d,%d)(%d,%d)(%d,%d)  Emitter drin=%d  Spawn drin=%d\n",
               c, xs[0],zs[0],xs[1],zs[1],xs[2],zs[2],xs[3],zs[3], in,
               re15_aot_point_in_quad(-10100, 4200, xs, zs));
    }

    printf("\n=== (B2) RVD-Zonentabelle (Anker = erste Zone je cam_from) ===\n");
    for (int i = 0; i < s_rdt.zone_count; i++) {
        const re15_rdt_zone_t *zz = &s_rdt.zones[i];
        int anchor = (i == 0 || s_rdt.zones[i-1].cam_from != zz->cam_from);
        printf("   z%02d %s from=%2u to=%2u quad(%d,%d)(%d,%d)(%d,%d)(%d,%d)\n", i,
               anchor ? "ANKER" : "     ", zz->cam_from, zz->cam_to,
               zz->xs[0],zz->zs[0],zz->xs[1],zz->zs[1],zz->xs[2],zz->zs[2],zz->xs[3],zz->zs[3]);
    }

    /* ---------- Gitter ---------- */
    int32_t minx = 1<<30, maxx = -(1<<30), minz = 1<<30, maxz = -(1<<30);
    for (int i = 0; i < s_rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &s_rdt.sca[i];
        if (e->x < minx) minx = e->x;
        if (e->z < minz) minz = e->z;
        if ((int32_t)e->x + e->width   > maxx) maxx = (int32_t)e->x + e->width;
        if ((int32_t)e->z + e->density > maxz) maxz = (int32_t)e->z + e->density;
    }
    GX0 = minx - 1000; GZ0 = minz - 1000;
    GW = (int)((maxx + 1000 - GX0)/GS) + 1;
    GH = (int)((maxz + 1000 - GZ0)/GS) + 1;
    printf("\n   SCA-Bereich x[%ld..%ld] z[%ld..%ld] -> Gitter %dx%d (Schritt %d)\n",
           (long)minx,(long)maxx,(long)minz,(long)maxz, GW, GH, GS);
    g_rea = (uint8_t*)calloc((size_t)GW*GH, 1);
    g_q   = (int32_t*)malloc(sizeof(int32_t)*(size_t)GW*GH);

    /* ---------- (C) Erreichbarkeit ab Tuer-Spawn ---------- */
    int sgx = (int)((pl->x - GX0 + GS/2)/GS), sgz = (int)((pl->z - GZ0 + GS/2)/GS);
    int qh = 0, qt = 0;
    if (sgx < 0 || sgx >= GW || sgz < 0 || sgz >= GH) { printf("   Saat ausserhalb\n"); return 1; }
    g_rea[sgz*GW+sgx] = 1; g_q[qt++] = sgz*GW+sgx;
    while (qh < qt) {
        int id = g_q[qh++];
        int gx = id % GW, gz = id / GW;
        static const int dx[4] = {1,-1,0,0}, dz[4] = {0,0,1,-1};
        for (int k = 0; k < 4; k++) {
            int nx = gx+dx[k], nz = gz+dz[k];
            if (nx < 0 || nx >= GW || nz < 0 || nz >= GH) continue;
            if (g_rea[nz*GW+nx]) continue;
            if (!step_ok(CX(gx), CZ(gz), CX(nx), CZ(nz), pl->y)) continue;
            g_rea[nz*GW+nx] = 1; g_q[qt++] = nz*GW+nx;
        }
    }
    int nrea = 0;
    for (int i = 0; i < GW*GH; i++) nrea += g_rea[i];
    printf("\n=== (C) ERREICHBAR ab Tuer-Spawn (re15_collision_constrain): %d Zellen ===\n", nrea);
    {
        int tx = (int)((2000 - GX0 + GS/2)/GS), tz = (int)((800 - GZ0 + GS/2)/GS);
        int ok = (tx>=0&&tx<GW&&tz>=0&&tz<GH) ? g_rea[tz*GW+tx] : -1;
        printf("   Teleport-Ziel des Befundes (2000,800): erreichbar=%d\n", ok);
    }
    for (int i = 0; i < nem; i++) {
        re15_actor_t *e = &g_actors[em_idx[i]];
        int tx = (int)((e->x - GX0 + GS/2)/GS), tz = (int)((e->z - GZ0 + GS/2)/GS);
        int ok = (tx>=0&&tx<GW&&tz>=0&&tz<GH) ? g_rea[tz*GW+tx] : -1;
        printf("   Emitter %d (%6ld,%6ld): Standpunkt erreichbar=%d\n", i, (long)e->x, (long)e->z, ok);
    }

    /* ---------- (D) Kamera-Zustandsmaschine ---------- */
    int NC = s_rdt.cut_count; if (NC > 64) NC = 64;
    g_rc = (uint8_t*)calloc((size_t)GW*GH*NC, 1);
    int32_t *q2 = (int32_t*)malloc(sizeof(int32_t)*(size_t)GW*GH*NC);
    qh = qt = 0;
    int c0 = cut_of_point((int)g_scd.cam_id, pl->x, pl->z);
    printf("   (Start-Cut = g_scd.cam_id nach Hochlauf = %u)\n", g_scd.cam_id);
    g_rc[(size_t)(sgz*GW+sgx)*NC + c0] = 1;
    q2[qt++] = (int32_t)((size_t)(sgz*GW+sgx)*NC + c0);
    while (qh < qt) {
        int id = q2[qh++];
        int cut = id % NC, cell = id / NC;
        int gx = cell % GW, gz = cell / GW;
        static const int dx[4] = {1,-1,0,0}, dz[4] = {0,0,1,-1};
        for (int k = 0; k < 4; k++) {
            int nx = gx+dx[k], nz = gz+dz[k];
            if (nx < 0 || nx >= GW || nz < 0 || nz >= GH) continue;
            if (!step_ok(CX(gx), CZ(gz), CX(nx), CZ(nz), pl->y)) continue;
            int nc = cut_of_point(cut, CX(nx), CZ(nz));
            if (nc < 0 || nc >= NC) nc = cut;
            size_t nid = (size_t)(nz*GW+nx)*NC + nc;
            if (g_rc[nid]) continue;
            g_rc[nid] = 1;
            q2[qt++] = (int32_t)nid;
        }
    }
    printf("\n=== (D) Cuts, die auf einem BEGEHBAREN Weg ab dem Tuer-Spawn aktiv werden ===\n");
    printf("   Start-Cut nach Spawn-Zonenpruefung = %d\n", c0);
    for (int c = 0; c < NC; c++) {
        long cnt = 0;
        for (int cell = 0; cell < GW*GH; cell++) if (g_rc[(size_t)cell*NC + c]) cnt++;
        if (cnt) printf("   Cut %2d: in %5ld Zellen aktiv   (Emitter im Region-Quad: %d) %s\n",
                        c, cnt, cut_has_em[c],
                        cut_has_em[c] > 0 ? "<== DREIECKE WUERDEN GEZEICHNET" : "");
    }
    /* (E) Der Teleport-Punkt des Befundes im Detail */
    {
        int32_t nx = 2000, nz = 800;
        re15_collision_ensure_band(-1800);
        re15_collision_constrain(&s_rdt, 2000, 800, &nx, &nz);
        printf("\n=== (E) Teleport-Punkt (2000,800) des Befundes ===\n");
        printf("   on_floor=%d  constrain(selbst)->(%ld,%ld)\n",
               re15_collision_on_floor(&s_rdt, 2000, 800), (long)nx, (long)nz);
        int32_t ox = nx, oz = nz; re15_collision_objects(&ox, &oz);
        printf("   nach Objekt-Ausschiebung ->(%ld,%ld)\n", (long)ox, (long)oz);
        long best = 1L<<30; int bx=-1,bz=-1;
        for (int gz2 = 0; gz2 < GH; gz2++) for (int gx2 = 0; gx2 < GW; gx2++) {
            if (!g_rea[gz2*GW+gx2]) continue;
            long ddx = CX(gx2)-2000, ddz = CZ(gz2)-800; long dd = ddx*ddx+ddz*ddz;
            if (dd < best) { best = dd; bx = CX(gx2); bz = CZ(gz2); }
        }
        printf("   naechste ERREICHBARE Zelle: (%d,%d)\n", bx, bz);
    }

    int reach13 = 0;
    for (int cell = 0; cell < GW*GH; cell++) if (g_rc[(size_t)cell*NC + 13]) reach13++;
    printf("\n   Cut 13 (der vom Befund erzwungene): auf natuerlichem Weg in %d Zellen aktiv\n", reach13);
    return 0;
}
