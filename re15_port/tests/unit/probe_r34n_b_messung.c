/* probe_r34n_b_messung.c — MESS-SONDE Spur B (Runde 34 Nacht): Hebetisch-Cursor ROOM1150/1151.
 *
 * KEIN Riegel (kein add_test), nur Messung fuer das Dossier
 * analysis/befunde_runde34_nacht/B_hebetisch.md. Misst mit ENGINE-Code (nicht mit Python):
 *
 *  A  HALTE-STELLE: sub04 aus der Raumkamera starten (scd_event_fire(4), wie der ACTION-Scan);
 *     in welchem Bild steht der sub04-Thread mit seinem PC auf dem For 15 der Deckelfahrt
 *     (ROOM1150 @0x0FC0, ROOM1151 @0x0F9E; Signatur Cut_chg 4 / Pos_set / Sleep 5 / For 15)?
 *     Plattform-y, angeforderter Cut, gemerkter Cut (cam_id_prev), Deckel-z in diesem Bild.
 *     Danach sub04 bis zum Ende: wohin stellt Cut_old @0x10B2 zurueck?
 *  B  GEGENPROBE "Vorschalt-Variante": zeigt der Port Cut 4 schon VOR sub04 (work_vars[0x0A] = 4,
 *     = angezeigter Cut, den Cut_chg LAB_800402a0 @0x800402c0 merkt), wohin stellt Cut_old dann?
 *  C  KUPPEL unter Cut 4: Deckel (Prop 1/2) + Podest (Prop 0, Punkte in y[-1036,-886],
 *     x[-490,-70], z[870,1650]) mit re15_camera_build_view + re15_camera_compose_view_bone +
 *     re15_gte_divide (dieselbe Projektion wie main.c PROJECT_VERT) -> Huelle, Abstand zur
 *     Python-Huelle aus re15_port/tools/r34n_b/kuppel_flaeche.py.
 *  D  11F0-CURSOR unter der Cut-10-Kamera von ROOM11F0: Heisspunkt (Objektlage, Welt-y -1800) am
 *     Start (-19554,22684) und nach einem Schritt 200 in x bzw. z.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_camera.h"
#include "re15_math.h"
#include "re15_inventory.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *datei(const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

static void raum_frisch(re15_rdt_t *rdt, uint16_t rid)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -21000; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt->messages, rdt->messages_size); }
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, -21000, -18500, 0);
}

static int slot_von_obj(uint8_t o)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == o) return k;
    return -1;
}

static const uint8_t k_sig[20] = { 0x29,0x04, 0x32,0x00,0x24,0xaf,0xcf,0xfe,0xcc,0xbb,
                                   0x09,0x0a,0x05,0x00, 0x0d,0x00,0x18,0x00,0x0f,0x00 };

static long halt_suchen(const re15_rdt_t *rdt)
{
    for (int i = 0; i + 20 <= rdt->raw_size; i++)
        if (rdt->raw[i] == 0x29 && memcmp(rdt->raw + i, k_sig, 20) == 0) return i + 14;
    return -1;
}

static int thread_auf(const uint8_t *pc)
{
    for (int t = 0; t < SCD_THREAD_COUNT; t++)
        if (g_scd.threads[t].active && !g_scd.threads[t].kill_pending && g_scd.threads[t].pc == pc) return t;
    return -1;
}

/* A + B */
static void halt_messen(re15_rdt_t *rdt, uint16_t rid, int vorschalt)
{
    raum_frisch(rdt, rid);
    long halt = halt_suchen(rdt);
    int w0 = g_scd.work_vars[0x0A];
    if (vorschalt) g_scd.work_vars[0x0A] = 4;   /* "der Port zeigt Cut 4 schon an" */
    printf("\n== %04X %s: Halt-PC @0x%04lX, angezeigter Cut vor sub04 = %d%s ==\n", rid,
           vorschalt ? "GEGENPROBE Vorschalt" : "sub04 aus der Raumkamera", halt, w0,
           vorschalt ? " -> auf 4 gesetzt" : "");
    scd_event_fire(4);
    const uint8_t *hp = rdt->raw + halt;
    int f_halt = -1, ende = -1;
    for (int f = 0; f < 900; f++) {
        scd_vm_tick();
        int p0 = slot_von_obj(0), p1 = slot_von_obj(1), p2 = slot_von_obj(2);
        if (f_halt < 0 && thread_auf(hp) >= 0) {
            f_halt = f;
            printf("   Bild %d (nach %d VM-Takten): sub04-PC @0x%04lX = For 15 NOCH NICHT ausgefuehrt; "
                   "Plattform y=%d, cam_id(angefordert)=%u, cam_id_prev(gemerkt)=%u, work_vars[0x0A]=%d, "
                   "Deckel z %d / %d\n", f, f + 1, halt,
                   p0 >= 0 ? (int)g_scd.props[p0].y : 0, (unsigned)g_scd.cam_id, (unsigned)g_scd.cam_id_prev,
                   (int)g_scd.work_vars[0x0A],
                   p1 >= 0 ? (int)g_scd.props[p1].z : 0, p2 >= 0 ? (int)g_scd.props[p2].z : 0);
        }
        if (f_halt >= 0 && f == f_halt + 1)
            printf("   Bild %d: Deckel z %d / %d (For laeuft)\n", f,
                   p1 >= 0 ? (int)g_scd.props[p1].z : 0, p2 >= 0 ? (int)g_scd.props[p2].z : 0);
        /* Ende: Plattform wieder geparkt und cam_id_prev ausgewertet (Cut_old @0x10B2) */
        if (f_halt >= 0 && p0 >= 0 && g_scd.props[p0].y < -20000 && f > f_halt + 100 && ende < 0) {
            /* ein paar Takte weiter, bis Cut_old gelaufen ist */
            for (int k = 0; k < 4; k++) scd_vm_tick();
            ende = f;
            printf("   Ende (Plattform geparkt y=%d, Bild %d): cam_id nach Cut_old = %u, work_vars[0x0A] = %d -> %s\n",
                   (int)g_scd.props[p0].y, f, (unsigned)g_scd.cam_id, (int)g_scd.work_vars[0x0A],
                   g_scd.cam_id == 4 ? "BLEIBT AUF CUT 4 (Loch-Bild)" : "zurueck in die Raumkamera");
            break;
        }
    }
    if (f_halt < 0) printf("   Halt-PC nie erreicht!\n");
}

/* C */
typedef struct { int x, y; } pkt_t;
static int kreuz(pkt_t o, pkt_t a, pkt_t b) { return (a.x-o.x)*(b.y-o.y) - (a.y-o.y)*(b.x-o.x); }
static int huelle(pkt_t *p, int n, pkt_t *h)
{
    /* sortieren (x, dann y) */
    for (int i = 1; i < n; i++) { pkt_t k = p[i]; int j = i - 1;
        while (j >= 0 && (p[j].x > k.x || (p[j].x == k.x && p[j].y > k.y))) { p[j+1] = p[j]; j--; }
        p[j+1] = k; }
    int m = 0;
    for (int i = 0; i < n; i++) { while (m >= 2 && kreuz(h[m-2], h[m-1], p[i]) <= 0) m--; h[m++] = p[i]; }
    for (int i = n - 2, t = m + 1; i >= 0; i--) { while (m >= t && kreuz(h[m-2], h[m-1], p[i]) <= 0) m--; h[m++] = p[i]; }
    return m - 1;
}

static void projiziere(const int32_t r[9], const int32_t t[3], int H, int vx, int vy, int vz, int *sx, int *sy, int *z)
{
    int32_t x = (int32_t)(((int64_t)vx*r[0] + (int64_t)vy*r[1] + (int64_t)vz*r[2]) >> 12) + t[0];
    int32_t y = (int32_t)(((int64_t)vx*r[3] + (int64_t)vy*r[4] + (int64_t)vz*r[5]) >> 12) + t[1];
    int32_t zz = (int32_t)(((int64_t)vx*r[6] + (int64_t)vy*r[7] + (int64_t)vz*r[8]) >> 12) + t[2];
    int32_t i1 = x > 0x7FFF ? 0x7FFF : (x < -0x8000 ? -0x8000 : x);
    int32_t i2 = y > 0x7FFF ? 0x7FFF : (y < -0x8000 ? -0x8000 : y);
    uint32_t sz = zz > 0xFFFF ? 0xFFFFu : (uint32_t)(zz < 0 ? 0 : zz);
    uint32_t n = re15_gte_divide((uint32_t)H, sz);
    *sx = 160 + (int)(((int64_t)i1 * (int64_t)n) >> 16);
    *sy = 120 + (int)(((int64_t)i2 * (int64_t)n) >> 16);
    *z = zz;
}

static int im_podest(int x, int y, int z) { return y >= -1036 && y <= -886 && x >= -490 && x <= -70 && z >= 870 && z <= 1650; }

static void kuppel_messen(const re15_rdt_t *rdt, uint16_t rid)
{
    re15_camera_view_t v;
    re15_camera_build_view(&rdt->cuts[4], &v);
    static const int32_t R2048[9] = { -4096,0,0, 0,4096,0, 0,0,-4096 };   /* rot_y 0x800 @0x0E00 */
    static const int32_t P[3] = { -20700, -305, -17460 };                  /* Pos_set @0x0FB4 */
    int32_t cr[9], ct[3];
    re15_camera_compose_view_bone(&v, R2048, P, cr, ct);
    static pkt_t pts[4096]; int n = 0;
    for (int op = 0; op <= 2; op++) {
        re15_md1_t m;
        if (re15_md1_parse(rdt->prop_md1[op], rdt->prop_md1_size[op], &m) != 0) continue;
        for (int mi = 0; mi < m.mesh_count; mi++) {
            const re15_md1_mesh_t *me = &m.meshes[mi];
            for (int k = 0; k < me->triangle_count; k++) {
                const re15_md1_vertex_t *a[3] = { &me->tri_vertices[me->triangles[k].v0],
                    &me->tri_vertices[me->triangles[k].v1], &me->tri_vertices[me->triangles[k].v2] };
                int ok = 1; for (int j = 0; j < 3; j++) if (op == 0 && !im_podest(a[j]->x, a[j]->y, a[j]->z)) ok = 0;
                if (!ok) continue;
                for (int j = 0; j < 3 && n < 4096; j++) { int z; projiziere(cr, ct, v.fov_screen_dist, a[j]->x, a[j]->y, a[j]->z, &pts[n].x, &pts[n].y, &z); n++; }
            }
            for (int k = 0; k < me->quad_count; k++) {
                const re15_md1_vertex_t *a[4] = { &me->quad_vertices[me->quads[k].v0], &me->quad_vertices[me->quads[k].v1],
                    &me->quad_vertices[me->quads[k].v2], &me->quad_vertices[me->quads[k].v3] };
                int ok = 1; for (int j = 0; j < 4; j++) if (op == 0 && !im_podest(a[j]->x, a[j]->y, a[j]->z)) ok = 0;
                if (!ok) continue;
                for (int j = 0; j < 4 && n < 4096; j++) { int z; projiziere(cr, ct, v.fov_screen_dist, a[j]->x, a[j]->y, a[j]->z, &pts[n].x, &pts[n].y, &z); n++; }
            }
        }
    }
    static pkt_t h[4200];
    int hn = huelle(pts, n, h);
    printf("\n== %04X Kuppel unter Cut 4 (Engine-Projektion, H=%d): %d Punkte, Huelle %d Ecken:", rid, (int)v.fov_screen_dist, n, hn);
    for (int i = 0; i < hn; i++) printf(" (%d,%d)", h[i].x, h[i].y);
    printf("\n");
    /* Python-Huelle (kuppel_flaeche.py, gerundet) */
    static const pkt_t py[12] = { {151,171},{163,161},{180,151},{206,149},{234,151},{248,157},{267,171},{264,191},{261,205},{214,211},{164,205},{151,191} };
    int maxd = 0;
    for (int i = 0; i < 12; i++) {
        int best = 1 << 30;
        for (int j = 0; j < hn; j++) { int dx = py[i].x - h[j].x, dy = py[i].y - h[j].y; int d = dx*dx + dy*dy; if (d < best) best = d; }
        if (best > maxd) maxd = best;
    }
    printf("   groesster Abstand Python-Ecke -> naechste Engine-Ecke: %d px^2\n", maxd);
}

/* D */
static void cursor_messen(void)
{
    size_t n = 0;
    uint8_t *buf = datei("STAGE1/ROOM11F0.RDT", &n);
    if (!buf) return;
    static re15_rdt_t r;
    memset(&r, 0, sizeof r);
    if (re15_rdt_parse(buf, n, &r) != 0 || r.cut_count <= 10) { printf("11F0 nicht lesbar\n"); free(buf); return; }
    re15_camera_view_t v;
    re15_camera_build_view(&r.cuts[10], &v);
    static const int32_t I[9] = { 4096,0,0, 0,4096,0, 0,0,4096 };
    static const int32_t O[3] = { 0,0,0 };
    int32_t cr[9], ct[3];
    re15_camera_compose_view_bone(&v, I, O, cr, ct);
    printf("\n== 11F0 Cut 10: fov %u -> H %d, pos (%d,%d,%d) tgt (%d,%d,%d)\n", r.cuts[10].fov, (int)v.fov_screen_dist,
           r.cuts[10].pos_x, r.cuts[10].pos_y, r.cuts[10].pos_z, r.cuts[10].target_x, r.cuts[10].target_y, r.cuts[10].target_z);
    static const int L[5][2] = { {-19554,22684}, {-19354,22684}, {-19554,22884}, {-19554-900,22684}, {-19554+900,22684} };
    static const char *name[5] = { "Start", "x+200 (RIGHT)", "z+200 (UP)", "Start-Kante x-900", "Start-Kante x+900" };
    for (int i = 0; i < 5; i++) {
        int sx, sy, z, sx9, sy9, z9;
        projiziere(cr, ct, v.fov_screen_dist, L[i][0], -1800, L[i][1], &sx, &sy, &z);   /* Oberseite */
        projiziere(cr, ct, v.fov_screen_dist, L[i][0], -900, L[i][1], &sx9, &sy9, &z9);  /* Objektlage */
        printf("   %-18s (%d,%d): Oberseite y-1800 -> (%d,%d) z %d | Objektlage y-900 -> (%d,%d) z %d\n",
               name[i], L[i][0], L[i][1], sx, sy, z, sx9, sy9, z9);
    }
    free(buf);
}

static int ein_raum(uint16_t rid, const char *rel)
{
    size_t n = 0;
    uint8_t *buf = datei(rel, &n);
    if (!buf) return 77;
    static re15_rdt_t rdt;
    memset(&rdt, 0, sizeof rdt);
    if (re15_rdt_parse(buf, n, &rdt) != 0) { free(buf); return 1; }
    halt_messen(&rdt, rid, 0);
    halt_messen(&rdt, rid, 1);
    kuppel_messen(&rdt, rid);
    free(buf);
    return 0;
}

int main(void)
{
    if (ein_raum(0x1150, "STAGE1/ROOM1150.RDT") == 77) return 77;
    if (ein_raum(0x1151, "STAGE1/ROOM1151.RDT") == 77) return 77;
    cursor_messen();
    return 0;
}
