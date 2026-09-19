/* Anker-Messung ROOM1210: Projektion Welt<->Bild fuer die Cuts, mit der byte-true
 * Kamera-Mathematik der Engine (re15_camera_build_view, FUN_80053ca4). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_rdt.h"
#include "re15_camera.h"

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc((size_t)sz);
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static void view_apply(const re15_camera_view_t *v, int32_t x, int32_t y, int32_t z, int32_t o[3])
{
    o[0] = (int32_t)(((int64_t)v->rot[0]*x + (int64_t)v->rot[1]*y + (int64_t)v->rot[2]*z) >> 12) + v->trans[0];
    o[1] = (int32_t)(((int64_t)v->rot[3]*x + (int64_t)v->rot[4]*y + (int64_t)v->rot[5]*z) >> 12) + v->trans[1];
    o[2] = (int32_t)(((int64_t)v->rot[6]*x + (int64_t)v->rot[7]*y + (int64_t)v->rot[8]*z) >> 12) + v->trans[2];
}
static int proj(const re15_camera_view_t *v, int32_t x, int32_t y, int32_t z, int *su, int *sv)
{
    int32_t c[3]; view_apply(v, x, y, z, c);
    if (c[2] <= 1) return 0;
    *su = 160 + (int)((int64_t)c[0] * v->fov_screen_dist / c[2]);
    *sv = 120 + (int)((int64_t)c[1] * v->fov_screen_dist / c[2]);
    return 1;
}
/* Rueckwaerts: Bildpixel (u,v) -> Weltstrahl; Schnitt mit Ebene x = px oder z = pz. */
static void ray_world(const re15_camera_view_t *vw, const re15_camera_cut_t *cut,
                      int u, int v, double d[3])
{
    double cx = (double)(u - 160) / (double)vw->fov_screen_dist;
    double cy = (double)(v - 120) / (double)vw->fov_screen_dist;
    double cz = 1.0;
    /* Welt-Richtung = R^T * Kamera-Richtung (R orthonormal, Q12) */
    d[0] = (vw->rot[0]*cx + vw->rot[3]*cy + vw->rot[6]*cz) / 4096.0;
    d[1] = (vw->rot[1]*cx + vw->rot[4]*cy + vw->rot[7]*cz) / 4096.0;
    d[2] = (vw->rot[2]*cx + vw->rot[5]*cy + vw->rot[8]*cz) / 4096.0;
    (void)cut;
}
int main(int argc, char **argv)
{
    const char *rdtp = (argc > 1) ? argv[1] : "shared_assets/PSX/STAGE1/ROOM1210.RDT";
    size_t n = 0; uint8_t *buf = slurp(rdtp, &n);
    if (!buf) { printf("RDT fehlt: %s\n", rdtp); return 1; }
    re15_rdt_t rdt; if (re15_rdt_parse(buf, n, &rdt) != 0) { printf("Parse-Fehler\n"); return 1; }
    printf("cuts=%d\n", rdt.cut_count);
    for (int c = 0; c < rdt.cut_count; c++) {
        const re15_camera_cut_t *cut = &rdt.cuts[c];
        re15_camera_view_t vw;
        if (re15_camera_build_view(cut, &vw) != 0) { printf("cut %d degeneriert\n", c); continue; }
        printf("cut %d pos=(%d,%d,%d) tgt=(%d,%d,%d) H=%d\n", c, cut->pos_x, cut->pos_y, cut->pos_z,
               cut->target_x, cut->target_y, cut->target_z, vw.fov_screen_dist);
    }
    /* Der Messschnitt wird als Argument gewaehlt. */
    int cutid = (argc > 2) ? atoi(argv[2]) : 4;
    re15_camera_view_t vw; re15_camera_build_view(&rdt.cuts[cutid], &vw);
    const re15_camera_cut_t *cut = &rdt.cuts[cutid];
    printf("\n== Cut %d: Westwandflaeche x=-21090, Ostwandflaeche x=-16676 ==\n", cutid);
    int32_t zs[] = { -15747, -17130, -12797, -14197, -21158, -22365, -8847, -10247 };
    int32_t ys[] = { 0, -1000, -1500, -2000, -2513, -3000, -3500 };
    for (size_t i = 0; i < sizeof zs / sizeof zs[0]; i++) {
        printf("  z=%6d :", zs[i]);
        for (size_t j = 0; j < sizeof ys / sizeof ys[0]; j++) {
            int u, v;
            if (proj(&vw, -21090, ys[j], zs[i], &u, &v)) printf("  y%5d->(%3d,%3d)", ys[j], u, v);
            else printf("  y%5d-> hinter", ys[j]);
        }
        printf("\n");
    }
    printf("\n== Rueckwaerts: Bildpixel -> Welt auf der Westwandebene x=-21090 ==\n");
    for (int a = 3; a < argc - 1; a += 2) {
        if (argv[a][0] != 'p') continue;
        int u = atoi(argv[a] + 1), v = atoi(argv[a + 1]);
        double d[3]; ray_world(&vw, cut, u, v, d);
        double t = ((-21090.0) - cut->pos_x) / d[0];
        printf("  Pixel (%3d,%3d) -> West x=-21090 bei y=%.0f z=%.0f\n", u, v,
               cut->pos_y + d[1]*t, cut->pos_z + d[2]*t);
        double t2 = ((-16676.0) - cut->pos_x) / d[0];
        if (t2 > 0) printf("                 -> Ost  x=-16676 bei y=%.0f z=%.0f\n",
                           cut->pos_y + d[1]*t2, cut->pos_z + d[2]*t2);
    }
    return 0;
}
