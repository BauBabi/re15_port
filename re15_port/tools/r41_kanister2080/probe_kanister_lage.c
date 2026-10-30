/* Runde 41 Spur G "kanister2080": Rueckprojektion der roten Marke aus Gas_Can.bmp (= ROOM2080 Cut 5, Dossier 1.1) auf
 * die Bodenebene des Balkons und Vorwaertsprojektion des stehenden Kanisters in alle Kameras von ROOM2080/2090.
 * Projektion wie der Zeichner (re15_camera_build_view: v = R*p + T, Bild = (160 + H*vx/vz, 120 + H*vy/vz), H =
 * fov_screen_dist) - dieselbe Rechnung wie tools/r41_re2gator/probe_kamera_re2.c.
 * Bau: gcc -std=c11 -DRE15_PLATFORM_PC=1 -I../../include probe_kanister_lage.c ../../build/engine/libre15_engine.a -lm
 * Aufruf: probe_kanister_lage <ROOM2080.RDT> <ROOM2090.RDT> */
#include "re15_camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

static uint8_t *slurp(const char *path, long *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = sz; return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

typedef struct { re15_camera_cut_t cut[16]; int n; } raum_t;
static int raum_laden(const char *pfad, raum_t *r)
{
    long n = 0; uint8_t *d = slurp(pfad, &n); if (!d) return -1;
    r->n = d[1]; uint32_t cam = rd32(d + 8 + 7 * 4);
    for (int c = 0; c < r->n && c < 16; c++) memcpy(&r->cut[c], d + cam + c * 32, 32);
    free(d); return 0;
}
static int proj(const re15_camera_cut_t *cut, const double p[3], double *sx, double *sy, double *tiefe)
{
    re15_camera_view_t v; if (re15_camera_build_view(cut, &v) != 0) return 0;
    double vx = (v.rot[0]*p[0] + v.rot[1]*p[1] + v.rot[2]*p[2]) / 4096.0 + v.trans[0];
    double vy = (v.rot[3]*p[0] + v.rot[4]*p[1] + v.rot[5]*p[2]) / 4096.0 + v.trans[1];
    double vz = (v.rot[6]*p[0] + v.rot[7]*p[1] + v.rot[8]*p[2]) / 4096.0 + v.trans[2];
    *tiefe = vz; if (vz <= 0) return 0;
    *sx = 160.0 + vx * v.fov_screen_dist / vz; *sy = 120.0 + vy * v.fov_screen_dist / vz;
    return 1;
}
/* Strahl durch Bildpunkt (u,v) mit der Ebene y = Y schneiden. */
static int rueck(const re15_camera_cut_t *cut, double u, double vv, double Y, double out[3])
{
    re15_camera_view_t v; if (re15_camera_build_view(cut, &v) != 0) return 0;
    double R[9]; for (int i = 0; i < 9; i++) R[i] = v.rot[i] / 4096.0;
    double T[3] = { v.trans[0], v.trans[1], v.trans[2] };
    double c[3]; for (int i = 0; i < 3; i++) c[i] = -(R[0*3+i]*T[0] + R[1*3+i]*T[1] + R[2*3+i]*T[2]);
    double dv[3] = { (u - 160.0) / v.fov_screen_dist, (vv - 120.0) / v.fov_screen_dist, 1.0 };
    double dw[3]; for (int i = 0; i < 3; i++) dw[i] = R[0*3+i]*dv[0] + R[1*3+i]*dv[1] + R[2*3+i]*dv[2];
    if (fabs(dw[1]) < 1e-9) return 0;
    double t = (Y - c[1]) / dw[1]; if (t <= 0) return 0;
    for (int i = 0; i < 3; i++) out[i] = c[i] + t * dw[i];
    return 1;
}
int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "Aufruf: %s ROOM2080.RDT ROOM2090.RDT [x z]\n", argv[0]); return 2; }
    raum_t r80, r90;
    if (raum_laden(argv[1], &r80) || raum_laden(argv[2], &r90)) { fprintf(stderr, "RDT nicht lesbar\n"); return 1; }
    const double Y8 = -14400.0;   /* Band 8: Etage 8 * -1800 (Tuer Slot 1 nfloor 8, SCA floor-Byte 0x88) */
    printf("== Rueckprojektion Gas_Can.bmp-Marke (Cut 5) auf y = %.0f\n", Y8);
    const double mk[][2] = { {167.0,156.0}, {159.0,149.0}, {175.0,149.0}, {159.0,163.0}, {175.0,163.0}, {167.0,163.0} };
    const char *nm[] = { "Mitte", "li-oben", "re-oben", "li-unten", "re-unten", "Mitte-unten" };
    double ziel[3] = {0,0,0};
    for (int i = 0; i < 6; i++) {
        double w[3];
        if (rueck(&r80.cut[5], mk[i][0], mk[i][1], Y8, w)) {
            printf("  %-11s (%5.1f,%5.1f) -> Welt (%.0f, %.0f, %.0f)\n", nm[i], mk[i][0], mk[i][1], w[0], w[1], w[2]);
            if (i == 0) memcpy(ziel, w, sizeof ziel);
        }
    }
    if (argc >= 5) { ziel[0] = atof(argv[3]); ziel[2] = atof(argv[4]); ziel[1] = Y8; }
    printf("== Kanister stehend: Fuss (%.0f,%.0f,%.0f), Kopf y %.0f (MD1 y -2784..0, r 267)\n", ziel[0], ziel[1], ziel[2], ziel[1] - 2784);
    for (int rr = 0; rr < 2; rr++) {
        raum_t *r = rr ? &r90 : &r80;
        for (int c = 0; c < r->n; c++) {
            double fx, fy, fz, kx, ky, kz;
            double fuss[3] = { ziel[0], ziel[1], ziel[2] }, kopf[3] = { ziel[0], ziel[1] - 2784, ziel[2] };
            int a = proj(&r->cut[c], fuss, &fx, &fy, &fz), b = proj(&r->cut[c], kopf, &kx, &ky, &kz);
            int sicht = a && b && fx >= 0 && fx < 320 && fy >= 0 && fy < 240 && kx >= 0 && kx < 320 && ky >= 0 && ky < 240;
            printf("  ROOM%s Cut %2d: Fuss (%.1f,%.1f) z %.0f  Kopf (%.1f,%.1f)  %s\n", rr ? "2090" : "2080", c,
                   a ? fx : -1.0, a ? fy : -1.0, fz, b ? kx : -1.0, b ? ky : -1.0, sicht ? "IM BILD" : "-");
        }
    }
    return 0;
}
