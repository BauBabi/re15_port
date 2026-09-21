/* Sonde Runde 22 — ROOM10F0 Cut 4/5: WIE VIEL FIGUR BLEIBT UEBRIG?
 *
 * ⛔ WOZU. Nutzer-Marke 2026-09-21 (befund.log Zeile 15602, Bild
 * befund_10F0_F335_marke1.bmp): "Leon ist da grossteils transparent". Gemessen am
 * Bild gegen den Hintergrund desselben Winkels: gezeichnet sind die Bildzeilen
 * 55..79, ab Zeile 80 nichts mehr — und der Maskenblock dieses Winkels beginnt
 * bei dstY=80.
 *
 * Diese Sonde ersetzt den RECHTECK-Kasten der bisherigen PRI-Messungen durch die
 * WIRKLICHE Figur: PL00.MD1 + PL00.EMR posiert, mit demselben Transform wie der
 * Renderer (main.c PROJECT_VERT: view x RotY(yaw) x Bone, GTE-Divide), in einen
 * 320x240-Tiefenpuffer gerastert. Gegen sie steht die TEXELGENAUE Maskendeckung
 * (R15M-Sektion + PRI-Atlas, Index 0 = durchsichtig — bg_pc.c pri_publish_tim).
 *
 * Ausgabe (stdout, maschinenlesbar):
 *   MODELL  hoehe=<modelleinheiten>  ...   Kopf-ueber-Fuss der posierten Figur
 *   MARKE   ...                            der Standpunkt der Nutzer-Marke
 *   PLATZ x z sichtbar verdeckt anteil     je begehbarem Standplatz
 *   SUMME  ...
 *
 * Aufruf: probe_r22_10f0_figur [cut] [keyframe]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_math.h"

#define W 320
#define H 240

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t *p; long len;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (uint8_t *) malloc((size_t) len);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t) len, f) != (size_t) len) { free(p); fclose(f); return NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return p;
}

/* ---------- Maskenkarte: kleinste Tiefe je Bildpunkt, TEXELGENAU ---------- */
static int32_t g_mtief[H][W];          /* INT32_MAX = kein Masken-Texel */
/* ⛔ RUNDE 22 - UNBELEGTE Maskentexel. Ein Texel ist BELEGT, wenn er in einer
 * Freistellung des Nutzers (oder in deren Loechern) liegt ODER ausserhalb jeder
 * Quader-Silhouette einer Zelle, fuer die eine Freistellung vorliegt. UNBELEGT heisst:
 * er liegt in der Silhouette einer Zelle MIT Freistellung, aber ausserhalb dieser
 * Freistellung - dort ist BEKANNT, wie der Gegenstand aussieht, und der Texel gehoert
 * nicht dazu. Das Feld kommt aus dem Beiblatt ROOM####_PRI##.BELEG (P4-PBM, von
 * raum.alte_kette geschrieben) und haengt NUR an der Freistellung und der
 * Raumgeometrie - nicht an den Dunkel-Regeln, die es beschraenken soll. */
static int32_t g_mtief_unb[H][W];      /* nur die UNBELEGTEN Texel */
static uint8_t g_belegt[H][W];         /* 1 = an dieser Stelle darf eine Maske stehen */
static int     g_beleg_da;             /* 0 = Beiblatt fehlt -> Riegel schlaegt an */
static int     g_riegelmodus;          /* 1 = als ctest gestartet ("riegel") */

static int maskenkarte(unsigned raum, int cut)
{
    char pfad[600];
    size_t sz = 0, tsz = 0;
    uint8_t *blob, *timbuf;
    re15_pri_cut_t pc;
    re15_tim_t tim;
    uint32_t off;
    int i, y, x, n;

    for (y = 0; y < H; y++) for (x = 0; x < W; x++) g_mtief[y][x] = INT32_MAX;

    /* ⛔ GEGENMODELL-VERZEICHNIS (Runde 22, Umsetzung): R22_MASKEN_DIR zeigt auf einen
     * BAUM mit ROOM####.MSK + ROOM####_PRI##.TIM. Ohne die Variable liest die Sonde das
     * ausgelieferte Asset. Nur so laesst sich eine UMGEBAUTE Maske mit derselben Figur
     * und demselben Urteil messen, ohne das Asset anzufassen. */
    {   const char *md = getenv("R22_MASKEN_DIR");
        if (md) snprintf(pfad, sizeof pfad, "%s/ROOM%04X.MSK", md, raum);
        else    snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, raum);
    }
    blob = slurp(pfad, &sz);
    if (!blob) return -1;
    off = re15_pri_msk_section_offset(blob, sz, cut);
    if (!off) { free(blob); return -2; }
    n = re15_pri_parse_section(blob, sz, off, &pc);
    if (n <= 0) { free(blob); return -3; }

    {   const char *md = getenv("R22_MASKEN_DIR");
        if (md) snprintf(pfad, sizeof pfad, "%s/ROOM%04X_PRI%02d.TIM", md, raum, cut);
        else    snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X_PRI%02d.TIM", RE15_ASSET_PSX_DIR, raum, cut);
    }
    timbuf = slurp(pfad, &tsz);
    if (!timbuf) { free(blob); return -4; }
    if (re15_tim_parse(timbuf, tsz, &tim) != 0 || tim.bpp != 8) {
        free(timbuf); free(blob); return -5;
    }
    {
        const uint8_t *idx = (const uint8_t *) tim.pixels;
        for (i = 0; i < pc.draw_count; i++) {
            const re15_pri_mask_t *m = &pc.masks[i];
            int dy = (int16_t) m->dstY, dx = (int16_t) m->dstX;
            for (y = 0; y < (int) m->height; y++) {
                int sy = (int) m->srcY + y, ty = dy + y;
                if (ty < 0 || ty >= H || sy < 0 || sy >= tim.height) continue;
                for (x = 0; x < (int) m->width; x++) {
                    int sx = (int) m->srcX + x, tx = dx + x;
                    if (tx < 0 || tx >= W || sx < 0 || sx >= tim.width) continue;
                    if (idx[sy * tim.width + sx] == 0) continue;     /* durchsichtig */
                    if ((int32_t) m->depth < g_mtief[ty][tx])
                        g_mtief[ty][tx] = (int32_t) m->depth;
                }
            }
        }
    }
    free(timbuf);
    free(blob);
    /* GEGENMODELL: nur die Texel behalten, die eine 320x240-Bytekarte freigibt
     * (1 = behalten). Damit laesst sich dieselbe Figur gegen "Auslieferung" und
     * gegen "nur Nutzer-Lasso" rechnen, ohne das Asset anzufassen. */
    {   const char *fp = getenv("R22_MASKENFILTER");
        if (fp) {
            FILE *f = fopen(fp, "rb");
            if (f) {
                static uint8_t k[H][W];
                if (fread(k, 1, sizeof k, f) == sizeof k) {
                    int yy, xx, weg = 0;
                    for (yy = 0; yy < H; yy++) for (xx = 0; xx < W; xx++)
                        if (!k[yy][xx] && g_mtief[yy][xx] != INT32_MAX) {
                            g_mtief[yy][xx] = INT32_MAX; weg++;
                        }
                    printf("MASKENFILTER %s entfernt=%d\n", fp, weg);
                }
                fclose(f);
            }
        }
    }
    /* --- Beiblatt .BELEG lesen und die UNBELEGTE Maskenkarte bilden --- */
    {   char bp[600];
        const char *md = getenv("R22_MASKEN_DIR");
        size_t bn = 0; uint8_t *bb;
        if (md) snprintf(bp, sizeof bp, "%s/ROOM%04X_PRI%02d.BELEG", md, raum, cut);
        else    snprintf(bp, sizeof bp, "%s/MASKS/ROOM%04X_PRI%02d.BELEG",
                         RE15_ASSET_PSX_DIR, raum, cut);
        for (y = 0; y < H; y++) for (x = 0; x < W; x++) { g_belegt[y][x] = 1; g_mtief_unb[y][x] = INT32_MAX; }
        g_beleg_da = 0;
        bb = slurp(bp, &bn);
        if (bb) {
            /* P4, 320x240: Kopf "P4
320 240
", danach 40 Bytes je Zeile, MSB links */
            size_t kopf = 0;
            int zeilen = 0;
            while (kopf < bn && zeilen < 2) { if (bb[kopf++] == 0x0A) zeilen++; }
            if (bn - kopf >= (size_t)(40 * H)) {
                const uint8_t *bits = bb + kopf;
                for (y = 0; y < H; y++) for (x = 0; x < W; x++)
                    g_belegt[y][x] = (uint8_t)((bits[y * 40 + (x >> 3)] >> (7 - (x & 7))) & 1);
                g_beleg_da = 1;
            }
            free(bb);
        }
        {   int unb = 0;
            for (y = 0; y < H; y++) for (x = 0; x < W; x++)
                if (!g_belegt[y][x]) {
                    g_mtief_unb[y][x] = g_mtief[y][x];
                    if (g_mtief[y][x] != INT32_MAX) unb++;
                }
            printf("BELEG cut=%d datei=%s gelesen=%d unbelegte_texel=%d\n",
                   cut, bp, g_beleg_da, unb);
        }
    }
    return pc.draw_count;
}

/* ---------- Figur: posierte PL00 in den Bildraum rastern ---------- */
static re15_md1_t  g_md1;
static re15_emd_skeleton_t g_skel;
static uint8_t    *g_md1buf, *g_emrbuf;

static int32_t g_fz[H][W];             /* Kamera-z der SICHTBAREN Figurflaeche */

static void fig_leer(void)
{
    int y, x;
    for (y = 0; y < H; y++) for (x = 0; x < W; x++) g_fz[y][x] = 0;
}

typedef struct { int sx, sy; int32_t vz; int ok; } pnt_t;

static pnt_t proj(const int32_t m[9], const int32_t t[3], int H_scr,
                  const re15_md1_vertex_t *v)
{
    pnt_t p;
    int32_t _x = v->x, _y = v->y, _z = v->z;
    int32_t vx = (int32_t)(((int64_t)_x*m[0] + (int64_t)_y*m[1] + (int64_t)_z*m[2]) >> 12) + t[0];
    int32_t vy = (int32_t)(((int64_t)_x*m[3] + (int64_t)_y*m[4] + (int64_t)_z*m[5]) >> 12) + t[1];
    int32_t vz = (int32_t)(((int64_t)_x*m[6] + (int64_t)_y*m[7] + (int64_t)_z*m[8]) >> 12) + t[2];
    p.ok = 0; p.sx = p.sy = 0; p.vz = vz;
    if (vz < 64) return p;
    {
        int32_t ir1 = vx > 0x7FFF ? 0x7FFF : (vx < -0x8000 ? -0x8000 : vx);
        int32_t ir2 = vy > 0x7FFF ? 0x7FFF : (vy < -0x8000 ? -0x8000 : vy);
        uint32_t sz3 = vz > 0xFFFF ? 0xFFFFu : (uint32_t) vz;
        uint32_t n = re15_gte_divide((uint32_t) H_scr, sz3);
        p.sx = 160 + (int)(((int64_t) ir1 * (int64_t) n) >> 16);
        p.sy = 120 + (int)(((int64_t) ir2 * (int64_t) n) >> 16);
        p.ok = 1;
    }
    return p;
}

/* Dreieck fuellen, NAECHSTE Tiefe je Punkt behalten (Maler ueber die ganze Figur:
 * sichtbar ist, was vorne liegt). */
static void tri(pnt_t a, pnt_t b, pnt_t c)
{
    int minx, maxx, miny, maxy, x, y;
    long d;
    if (!a.ok || !b.ok || !c.ok) return;
    minx = a.sx < b.sx ? (a.sx < c.sx ? a.sx : c.sx) : (b.sx < c.sx ? b.sx : c.sx);
    maxx = a.sx > b.sx ? (a.sx > c.sx ? a.sx : c.sx) : (b.sx > c.sx ? b.sx : c.sx);
    miny = a.sy < b.sy ? (a.sy < c.sy ? a.sy : c.sy) : (b.sy < c.sy ? b.sy : c.sy);
    maxy = a.sy > b.sy ? (a.sy > c.sy ? a.sy : c.sy) : (b.sy > c.sy ? b.sy : c.sy);
    if (minx < 0) minx = 0; if (maxx >= W) maxx = W - 1;
    if (miny < 0) miny = 0; if (maxy >= H) maxy = H - 1;
    d = (long)(b.sx - a.sx) * (c.sy - a.sy) - (long)(b.sy - a.sy) * (c.sx - a.sx);
    if (d == 0) return;
    for (y = miny; y <= maxy; y++)
        for (x = minx; x <= maxx; x++) {
            long w0 = (long)(b.sx - a.sx) * (y - a.sy) - (long)(b.sy - a.sy) * (x - a.sx);
            long w1 = (long)(c.sx - b.sx) * (y - b.sy) - (long)(c.sy - b.sy) * (x - b.sx);
            long w2 = (long)(a.sx - c.sx) * (y - c.sy) - (long)(a.sy - c.sy) * (x - c.sx);
            int innen = (d > 0) ? (w0 >= 0 && w1 >= 0 && w2 >= 0)
                                : (w0 <= 0 && w1 <= 0 && w2 <= 0);
            if (!innen) continue;
            {
                int32_t z = (a.vz + b.vz + c.vz) / 3;
                if (g_fz[y][x] == 0 || z < g_fz[y][x]) g_fz[y][x] = z;
            }
        }
}

/* Figur an (wx, wy, wz) mit Blickrichtung yaw in g_fz rastern.
 * Rueckgabe: Zahl der gezeichneten Bildpunkte. Setzt *y_oben/*y_unten. */
static int figur(const re15_camera_view_t *view, int32_t wx, int32_t wy, int32_t wz,
                 int16_t yaw, int keyframe, int *y_oben, int *y_unten,
                 int *x_links, int *x_rechts)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t yawrot[9];
    int32_t welt[3]; int b, n = 0, y, x;
    fig_leer();
    if (re15_skel_compute_pose(&g_skel, keyframe, poses) != 0) return -1;
    re15_camera_yaw_matrix_angle(yaw, yawrot);
    welt[0] = wx; welt[1] = wy; welt[2] = wz;
    for (b = 0; b < g_skel.bone_count && b < g_md1.mesh_count; b++) {
        const re15_skel_pose_t *p = &poses[b];
        const re15_md1_mesh_t *m = &g_md1.meshes[b];
        int32_t bm[9], bt[3], cm[9], ct[3];
        int k;
        /* Bone-Welt = RotY(yaw) * (pose.rot, pose.trans), Ursprung = Figurposition.
         * Genau main.c: yaw_rot_q12 x pose, dann compose_view_bone. */
        for (k = 0; k < 3; k++) {
            int r;
            for (r = 0; r < 3; r++)
                bm[k*3+r] = (int32_t)(((int64_t)yawrot[k*3+0]*p->rot[0*3+r]
                                     + (int64_t)yawrot[k*3+1]*p->rot[1*3+r]
                                     + (int64_t)yawrot[k*3+2]*p->rot[2*3+r]) >> 12);
        }
        for (k = 0; k < 3; k++)
            bt[k] = welt[k] + (int32_t)(((int64_t)yawrot[k*3+0]*p->trans[0]
                                       + (int64_t)yawrot[k*3+1]*p->trans[1]
                                       + (int64_t)yawrot[k*3+2]*p->trans[2]) >> 12);
        re15_camera_compose_view_bone(view, bm, bt, cm, ct);
        {
            int ti;
            for (ti = 0; ti < m->triangle_count; ti++) {
                const re15_md1_triangle_t *t = &m->triangles[ti];
                tri(proj(cm, ct, view->fov_screen_dist, &m->tri_vertices[t->v0]),
                    proj(cm, ct, view->fov_screen_dist, &m->tri_vertices[t->v1]),
                    proj(cm, ct, view->fov_screen_dist, &m->tri_vertices[t->v2]));
            }
            for (ti = 0; ti < m->quad_count; ti++) {
                const re15_md1_quad_t *q = &m->quads[ti];
                pnt_t p0 = proj(cm, ct, view->fov_screen_dist, &m->quad_vertices[q->v0]);
                pnt_t p1 = proj(cm, ct, view->fov_screen_dist, &m->quad_vertices[q->v1]);
                pnt_t p2 = proj(cm, ct, view->fov_screen_dist, &m->quad_vertices[q->v2]);
                pnt_t p3 = proj(cm, ct, view->fov_screen_dist, &m->quad_vertices[q->v3]);
                tri(p0, p1, p2); tri(p1, p3, p2);
            }
        }
    }
    *y_oben = H; *y_unten = -1; *x_links = W; *x_rechts = -1;
    for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x]) {
        n++;
        if (y < *y_oben) *y_oben = y;
        if (y > *y_unten) *y_unten = y;
        if (x < *x_links) *x_links = x;
        if (x > *x_rechts) *x_rechts = x;
    }
    return n;
}

/* Liegt (x,z) in einer RVD-Zone, die auf diesen Cut schaltet? Punkt-in-Viereck
 * wie FUN_80014368 (4 Kreuzprodukte, gleiches Vorzeichen). */
static int in_quad(const re15_rdt_zone_t *z, int x, int zz)
{
    int i, pos = 0, neg = 0;
    for (i = 0; i < 4; i++) {
        int j = (i + 1) & 3;
        long c = (long)(z->xs[j] - z->xs[i]) * (zz - z->zs[i])
               - (long)(z->zs[j] - z->zs[i]) * (x - z->xs[i]);
        if (c > 0) pos = 1;
        if (c < 0) neg = 1;
    }
    return !(pos && neg);
}

/* ANKER-Zone eines Cuts = der Eintrag `von=<cut> nach=0`: das Gebiet, in dem der
 * Winkel STEHEN BLEIBT (die uebrigen Eintraege `von=N nach=M` sind die schmalen
 * Schaltbaender). Beleg ROOM10F0: ZONEDEF 11 `von=4 nach=0` = x-2400..3600 /
 * z3300..16800 — deckungsgleich mit der Anker-Angabe des Zensus Runde 19. */
static int zone_des_cuts(const re15_rdt_t *rdt, int cut, int x, int z)
{
    int i;
    for (i = 0; i < rdt->zone_count; i++)
        if ((int) rdt->zones[i].cam_from == cut && rdt->zones[i].cam_to == 0
            && in_quad(&rdt->zones[i], x, z)) return 1;
    return 0;
}

/* ================= RIEGEL (Runde 22) =====================================
 *
 * ERKENNUNGSMASS "UNBELEGTE VERDECKUNG":
 *   Ein Figurpunkt gilt als UNBELEGT VERDECKT, wenn ihn ein Maskentexel nach dem
 *   Original-Urteil (depth < (1023*vz)>>16, re15_pri.h:104/120) verdeckt UND dieser
 *   Texel ausserhalb des BELEGT-Feldes des Winkels liegt, also in der Silhouette
 *   einer Moebelzelle, fuer die eine Freistellung des Nutzers vorliegt, aber
 *   ausserhalb dieser Freistellung.
 *   SCHRANKE = 0. Kein freier Parameter: die Schranke heisst nicht "hoechstens x %",
 *   sondern "gar nicht" - wo BEKANNT ist, wie der Gegenstand aussieht, darf nichts
 *   ausserhalb davon die Figur verdecken. (Der Nutzer-Fall waren 154 solcher Punkte
 *   an einem einzigen Standplatz, 50,9 % statt 32,5 % verdeckte Figur.)
 *
 * ABDECKUNG - damit die Zahl nicht fuer mehr genommen wird, als sie traegt:
 *   - die Winkel ROOM10F0 C4 und C5. Das sind GENAU die Winkel, in denen die
 *     Dunkel-Regeln ueberhaupt wirken: sie wirken nur ueber "nur_kunst" und
 *     tiefe="szene", und die stehen in der ganzen auswahl.json nur dort
 *     (2 von 439 Winkeln mit Masken, 0,46 %).
 *   - je Winkel ALLE begehbaren Standplaetze des Raums im 200er-Raster
 *     (Klemmpfad re15_collision_constrain), die der Anker-Zone getrennt gezaehlt.
 *   - die Nutzer-Marke F335 und, mit Pfaddatei, jedes protokollierte Bild.
 *   - Fehlt das Beiblatt .BELEG, schlaegt der Riegel an - kein stilles Gruen.
 */
static int riegel(const re15_rdt_t *rdt, const char *pfaddatei, int keyframe)
{
    static const int CUTS[2] = { 4, 5 };
    int ci, fehler = 0;
    for (ci = 0; ci < 2; ci++) {
        int cut = CUTS[ci];
        re15_camera_view_t view;
        int lo = 0, hi = 0, s, b, y, x;
        int plaetze = 0, schlecht = 0, zplaetze = 0;
        long summe = 0;
        int X0 = 1 << 30, X1 = -(1 << 30), Z0 = 1 << 30, Z1 = -(1 << 30);
        if (cut >= rdt->cut_count) { printf("RIEGEL cut=%d FEHLT\n", cut); return 1; }
        if (re15_camera_build_view(&rdt->cuts[cut], &view) != 0) return 1;
        if (maskenkarte(0x10F0, cut) <= 0) {
            printf("RIEGEL cut=%d keine Maske\n", cut); return 1;
        }
        if (!g_beleg_da) {
            printf("RIEGEL cut=%d Beiblatt .BELEG fehlt oder ist unlesbar\n", cut);
            fehler = 1;
            continue;
        }
        if (cut == 4) {
            int yo, yu, xl, xr, n, unb = 0;
            n = figur(&view, -448, 0, 14087, (int16_t) 13040, keyframe, &yo, &yu, &xl, &xr);
            for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x])
                if (g_mtief_unb[y][x] < re15_pri_bucket_of_vz(g_fz[y][x])) unb++;
            printf("RIEGEL MARKE cut=4 F335 punkte=%d unbelegt_verdeckt=%d\n", n, unb);
            if (unb) fehler = 1;
        }
        if (pfaddatei) {
            FILE *f = fopen(pfaddatei, "r");
            char zeile[256];
            int n_cut = 0, n_bad = 0, s_unb = 0;
            if (f) {
                while (fgets(zeile, sizeof zeile, f)) {
                    int fr, c, px, py, pz, prot, yo, yu, xl, xr, n, unb = 0;
                    if (sscanf(zeile, "%d %d %d %d %d %d", &fr, &c, &px, &py, &pz, &prot) != 6)
                        continue;
                    if (c != cut) continue;
                    n_cut++;
                    n = figur(&view, px, py, pz, (int16_t) prot, keyframe, &yo, &yu, &xl, &xr);
                    if (n <= 0) continue;
                    for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x])
                        if (g_mtief_unb[y][x] < re15_pri_bucket_of_vz(g_fz[y][x])) unb++;
                    if (unb) { n_bad++; s_unb += unb; }
                }
                fclose(f);
            }
            printf("RIEGEL PFAD cut=%d bilder=%d mit_unbelegter_verdeckung=%d punkte=%d\n",
                   cut, n_cut, n_bad, s_unb);
            if (n_bad) fehler = 1;
        }
        re15_collision_reset_band();
        re15_collision_band_range(rdt, &lo, &hi);
        for (s = 0; s < rdt->sca_count; s++) {
            const re15_sca_entry_t *e = &rdt->sca[s];
            if ((int) e->x < X0) X0 = e->x;
            if ((int) e->z < Z0) Z0 = e->z;
            if ((int) e->x + (int) e->width   > X1) X1 = e->x + e->width;
            if ((int) e->z + (int) e->density > Z1) Z1 = e->z + e->density;
        }
        for (b = lo; b <= hi; b++) {
            int gx, gz, hat = 0;
            for (s = 0; s < rdt->sca_count; s++)
                if ((rdt->sca[s].floor >> 4) == b) { hat = 1; break; }
            if (!hat) continue;
            re15_collision_set_band(b);
            for (gx = X0; gx <= X1; gx += 200)
                for (gz = Z0; gz <= Z1; gz += 200) {
                    int32_t x2 = gx, z2 = gz;
                    int yo, yu, xl, xr, n, unb = 0;
                    re15_collision_constrain(rdt, gx, gz, &x2, &z2);
                    if (x2 != gx || z2 != gz) continue;
                    n = figur(&view, gx, (int32_t)(-b * 0x708), gz, 0, keyframe,
                              &yo, &yu, &xl, &xr);
                    if (n <= 0) continue;
                    if (xr < 0 || xl >= W || yu < 0 || yo >= H) continue;
                    plaetze++;
                    if (zone_des_cuts(rdt, cut, gx, gz)) zplaetze++;
                    for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x])
                        if (g_mtief_unb[y][x] < re15_pri_bucket_of_vz(g_fz[y][x])) unb++;
                    if (unb) {
                        if (schlecht < 8)
                            printf("RIEGEL PLATZ cut=%d band=%d x=%d z=%d punkte=%d "
                                   "unbelegt_verdeckt=%d\n", cut, b, gx, gz, n, unb);
                        schlecht++; summe += unb;
                    }
                }
        }
        re15_collision_reset_band();
        printf("RIEGEL STAND cut=%d plaetze=%d davon_ankerzone=%d "
               "mit_unbelegter_verdeckung=%d punkte=%ld\n",
               cut, plaetze, zplaetze, schlecht, summe);
        if (schlecht) fehler = 1;
    }
    printf("RIEGEL r22_10f0: %s\n", fehler ? "GEFALLEN" : "GEHALTEN");
    return fehler;
}

int main(int argc, char **argv)
{
    unsigned raum = 0x10F0;
    int cut = 4, keyframe = 0;
    size_t sz = 0;
    uint8_t *rdtbuf;
    re15_rdt_t rdt;
    re15_camera_view_t view;
    int mn, i;
    const char *pfaddatei = NULL;

    {   int i2, ist_riegel = 0;
        for (i2 = 1; i2 < argc; i2++) if (!strcmp(argv[i2], "riegel")) ist_riegel = 1;
        g_riegelmodus = ist_riegel;
    }
    if (!g_riegelmodus && argc > 1) cut = atoi(argv[1]);
    if (!g_riegelmodus && argc > 2) keyframe = atoi(argv[2]);
    if (g_riegelmodus) { if (argc > 2) pfaddatei = argv[2]; }
    else if (argc > 3) pfaddatei = argv[3];

    {   char p[600];
        size_t n = 0;
        snprintf(p, sizeof p, "%s/PLD/PL00.MD1", RE15_ASSET_PSX_DIR);
        g_md1buf = slurp(p, &n);
        if (!g_md1buf || re15_md1_parse(g_md1buf, n, &g_md1) != 0) {
            printf("FEHLER PL00.MD1\n"); return 1; }
        snprintf(p, sizeof p, "%s/PLD/PL00.EMR", RE15_ASSET_PSX_DIR);
        g_emrbuf = slurp(p, &n);
        if (!g_emrbuf || re15_emd_parse_skeleton(g_emrbuf, n, &g_skel) != 0) {
            printf("FEHLER PL00.EMR\n"); return 1; }
    }
    printf("MODELL meshes=%d bones=%d\n", g_md1.mesh_count, g_skel.bone_count);

    {   char p[600];
        snprintf(p, sizeof p, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, raum);
        rdtbuf = slurp(p, &sz);
        if (!rdtbuf || re15_rdt_parse(rdtbuf, sz, &rdt) < 0) { printf("FEHLER RDT\n"); return 1; }
    }
    if (g_riegelmodus) return riegel(&rdt, pfaddatei, keyframe);
    if (cut >= rdt.cut_count) { printf("FEHLER cut\n"); return 1; }
    if (re15_camera_build_view(&rdt.cuts[cut], &view) != 0) { printf("FEHLER view\n"); return 1; }
    printf("KAMERA cut=%d H=%d\n", cut, view.fov_screen_dist);
    for (i = 0; i < rdt.zone_count; i++)
        printf("ZONEDEF %d von=%d nach=%d q=(%d,%d)(%d,%d)(%d,%d)(%d,%d)\n", i,
               rdt.zones[i].cam_from, rdt.zones[i].cam_to,
               rdt.zones[i].xs[0], rdt.zones[i].zs[0], rdt.zones[i].xs[1], rdt.zones[i].zs[1],
               rdt.zones[i].xs[2], rdt.zones[i].zs[2], rdt.zones[i].xs[3], rdt.zones[i].zs[3]);

    mn = maskenkarte(raum, cut);
    printf("MASKEN cut=%d rechtecke=%d\n", cut, mn);
    {   int y, x, belegt = 0;
        for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_mtief[y][x] != INT32_MAX) belegt++;
        printf("MASKEN texel=%d\n", belegt);
    }

    /* ---- (1) die MARKE des Nutzers ---- */
    {   int yo, yu, xl, xr, n;
        n = figur(&view, -448, 0, 14087, (int16_t)13040, keyframe, &yo, &yu, &xl, &xr);
        printf("MARKE punkte=%d kasten=x%d..%d,y%d..%d\n", n, xl, xr, yo, yu);
        {   int y, x, verd = 0, frei = 0;
            int zeile_ges[H], zeile_verd[H];
            for (y = 0; y < H; y++) { zeile_ges[y] = 0; zeile_verd[y] = 0; }
            for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x]) {
                int bk = re15_pri_bucket_of_vz(g_fz[y][x]);
                zeile_ges[y]++;
                if (g_mtief[y][x] < bk) { verd++; zeile_verd[y]++; }
                else frei++;
            }
            printf("MARKE verdeckt=%d frei=%d anteil=%.1f%%\n", verd, frei,
                   n ? 100.0 * verd / n : 0.0);
            for (y = 0; y < H; y++) if (zeile_ges[y])
                printf("MARKEZEILE y=%d ges=%d verdeckt=%d\n", y, zeile_ges[y], zeile_verd[y]);
        }
    }

    /* ---- (1b) Hoehe der WIRKLICHEN Figur gegen die 1700-Naeherung der Sonden ---- */
    {   int yo, yu, xl, xr, n;
        long vz;
        n = figur(&view, -448, 0, 14087, (int16_t)13040, keyframe, &yo, &yu, &xl, &xr);
        (void)n;
        vz = 11081;                       /* Fuss-Kamera-z der Marke (Log F335) */
        printf("HOEHE figur_px=%d kasten1700_px=%d modelleinheiten=%ld\n",
               yu - yo + 1, (int)(1700L * view.fov_screen_dist / vz),
               (long)((yu - yo + 1) * vz / view.fov_screen_dist));
        /* Die Tiefenkarte der Figur an der Marke roh ausgeben (320*240 int32,
         * 0 = nicht gezeichnet) — damit die Masken-Varianten in Python gegen
         * DIESELBE Figur gerechnet werden koennen. */
        {   const char *dp = getenv("R22_FIGDUMP");
            if (dp) { FILE *f = fopen(dp, "wb");
                      if (f) { fwrite(g_fz, sizeof g_fz, 1, f); fclose(f);
                               printf("FIGDUMP %s\n", dp); } }
        }
    }

    /* ---- (1b2) Modellhoehe DIREKT: Kopf-ueber-Fuss der posierten Figur in
     * Welteinheiten (Vertex-Extrem, nicht aus dem Bild zurueckgerechnet). ---- */
    {   static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        int b, minY = 1 << 30, maxY = -(1 << 30);
        if (re15_skel_compute_pose(&g_skel, keyframe, poses) == 0) {
            for (b = 0; b < g_skel.bone_count && b < g_md1.mesh_count; b++) {
                const re15_skel_pose_t *p = &poses[b];
                const re15_md1_mesh_t *m = &g_md1.meshes[b];
                int pass, v;
                for (pass = 0; pass < 2; pass++) {
                    const re15_md1_vertex_t *vs = pass ? m->quad_vertices : m->tri_vertices;
                    int nv = pass ? m->quad_vertex_count : m->tri_vertex_count;
                    if (!vs) continue;
                    for (v = 0; v < nv; v++) {
                        int32_t Y = (int32_t)(((int64_t)p->rot[3]*vs[v].x
                                             + (int64_t)p->rot[4]*vs[v].y
                                             + (int64_t)p->rot[5]*vs[v].z) >> 12) + p->trans[1];
                        if (Y < minY) minY = Y;
                        if (Y > maxY) maxY = Y;
                    }
                }
            }
            printf("MODELLHOEHE kf=%d yMin=%d yMax=%d hoehe=%d "
                   "(Sonden-Naeherung 1700, Faktor %.2f)\n",
                   keyframe, minY, maxY, maxY - minY, (maxY - minY) / 1700.0);
        }
    }

    /* ---- (1c) DER ECHTE PFAD DES NUTZERS (befund.log, eine Zeile je 15 Bilder) ----
     * Das ist die einzige Messung, die keinen Standplatz-Ersatz braucht: die Orte,
     * an denen der Nutzer wirklich stand. Format je Zeile: F cut x y z rot */
    if (pfaddatei) {
        FILE *f = fopen(pfaddatei, "r");
        if (!f) { printf("FEHLER pfad\n"); }
        else {
            char zeile[256];
            int n_all = 0, n_cut = 0, n_halb = 0, n_kontakt = 0;
            long s_ges = 0, s_verd = 0;
            re15_camera_view_t pv;
            int pcut = -1;
            while (fgets(zeile, sizeof zeile, f)) {
                int fr, c, px, py, pz, prot;
                int yo, yu, xl, xr, n, y, x, verd = 0;
                if (sscanf(zeile, "%d %d %d %d %d %d", &fr, &c, &px, &py, &pz, &prot) != 6)
                    continue;
                n_all++;
                if (c != cut) continue;
                if (c != pcut) {
                    if (c >= rdt.cut_count) continue;
                    if (re15_camera_build_view(&rdt.cuts[c], &pv) != 0) continue;
                    pcut = c;
                }
                n_cut++;
                n = figur(&pv, px, py, pz, (int16_t) prot, keyframe, &yo, &yu, &xl, &xr);
                if (n <= 0) continue;
                for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x]) {
                    int bk = re15_pri_bucket_of_vz(g_fz[y][x]);
                    if (g_mtief[y][x] < bk) verd++;
                }
                s_ges += n; s_verd += verd;
                if (verd > 0) n_kontakt++;
                if (verd * 2 > n) n_halb++;
                printf("PFAD F%d cut=%d pos=(%d,%d,%d) rot=%d punkte=%d verdeckt=%d %.1f%%\n",
                       fr, c, px, py, pz, prot, n, verd, 100.0 * verd / n);
            }
            fclose(f);
            printf("PFADSUMME cut=%d zeilen=%d davon_cut=%d mitkontakt=%d ueberhalb=%d "
                   "figurpunkte=%ld verdeckt=%ld anteil=%.2f%%\n",
                   cut, n_all, n_cut, n_kontakt, n_halb, s_ges, s_verd,
                   s_ges ? 100.0 * s_verd / s_ges : 0.0);
        }
    }

    /* ---- (2) Sweep ueber die begehbaren Standplaetze ---- */
    {   int lo = 0, hi = 0, s, b;
        int X0 = 1 << 30, X1 = -(1 << 30), Z0 = 1 << 30, Z1 = -(1 << 30);
        int plaetze = 0, halb = 0, ganz = 0, mitkontakt = 0;
        int zplaetze = 0, zhalb = 0, zkontakt = 0;
        long zsum_ges = 0, zsum_verd = 0;
        long sum_ges = 0, sum_verd = 0;
        re15_collision_reset_band();
        re15_collision_band_range(&rdt, &lo, &hi);
        for (s = 0; s < rdt.sca_count; s++) {
            const re15_sca_entry_t *e = &rdt.sca[s];
            if ((int) e->x < X0) X0 = e->x;
            if ((int) e->z < Z0) Z0 = e->z;
            if ((int) e->x + (int) e->width   > X1) X1 = e->x + e->width;
            if ((int) e->z + (int) e->density > Z1) Z1 = e->z + e->density;
        }
        for (b = lo; b <= hi; b++) {
            int gx, gz, hat = 0;
            for (s = 0; s < rdt.sca_count; s++) if ((rdt.sca[s].floor >> 4) == b) { hat = 1; break; }
            if (!hat) continue;
            re15_collision_set_band(b);
            for (gx = X0; gx <= X1; gx += 200)
                for (gz = Z0; gz <= Z1; gz += 200) {
                    int32_t x2 = gx, z2 = gz;
                    int yo, yu, xl, xr, n, y, x, verd = 0;
                    re15_collision_constrain(&rdt, gx, gz, &x2, &z2);
                    if (x2 != gx || z2 != gz) continue;
                    n = figur(&view, gx, (int32_t)(-b * 0x708), gz, 0, keyframe,
                              &yo, &yu, &xl, &xr);
                    if (n <= 0) continue;
                    if (xr < 0 || xl >= W || yu < 0 || yo >= H) continue;
                    plaetze++;
                    for (y = 0; y < H; y++) for (x = 0; x < W; x++) if (g_fz[y][x]) {
                        int bk = re15_pri_bucket_of_vz(g_fz[y][x]);
                        if (g_mtief[y][x] < bk) verd++;
                    }
                    sum_ges += n; sum_verd += verd;
                    if (verd > 0) mitkontakt++;
                    if (verd * 2 > n) halb++;
                    if (verd * 10 > n * 9) ganz++;
                    {   int inzone = zone_des_cuts(&rdt, cut, gx, gz);
                        if (inzone) {
                            zplaetze++; zsum_ges += n; zsum_verd += verd;
                            if (verd > 0) zkontakt++;
                            if (verd * 2 > n) zhalb++;
                        }
                        printf("PLATZ %d %d %d %d %d %d %.1f\n",
                               b, gx, gz, inzone, n, verd, 100.0 * verd / n);
                    }
                }
        }
        re15_collision_reset_band();
        printf("SUMME cut=%d plaetze=%d mitkontakt=%d ueberhalb=%d ueber90=%d "
               "figurpunkte=%ld verdeckt=%ld anteil=%.2f%%\n",
               cut, plaetze, mitkontakt, halb, ganz, sum_ges, sum_verd,
               sum_ges ? 100.0 * sum_verd / sum_ges : 0.0);
        printf("ZONE cut=%d plaetze=%d mitkontakt=%d ueberhalb=%d "
               "figurpunkte=%ld verdeckt=%ld anteil=%.2f%%\n",
               cut, zplaetze, zkontakt, zhalb, zsum_ges, zsum_verd,
               zsum_ges ? 100.0 * zsum_verd / zsum_ges : 0.0);
    }
    return 0;
}
