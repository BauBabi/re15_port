/* Runde 41 Spur G "kanister2080": Lagen-Probe ROOM2080 Band 8 (Laufgang). Je Kandidat (Fuss y -14400, Kopf y -16200
 * = Fuss - 1800, Etage 8 nach Sce_em_set pc[4] = 8 -> y = -8*1800 @0x800421d4) die Bildlage in den Cuts 2..7, der
 * Abstand zu den SCA-Waenden des Bandes 8 (floor-Byte 0x88, Zellen @0x534..0x5C4) und der Mindestabstand der Zombies
 * untereinander (>= 800 = zwei Radien 400, +0x78-Satz +6 FUN_8003a31c). Projektion wie probe_kanister_lage.c.
 * Aufruf: probe_lagen <ROOM2080.RDT> */
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
static re15_camera_cut_t g_cut[16]; static int g_ncut;
typedef struct { int16_t w, d, x, z; uint8_t typ, u0, u1, fl; } sca_t;
static sca_t g_sca[64]; static int g_nsca;

static int proj(int c, double x, double y, double z, double *sx, double *sy)
{
    re15_camera_view_t v; if (re15_camera_build_view(&g_cut[c], &v) != 0) return 0;
    double vx = (v.rot[0]*x + v.rot[1]*y + v.rot[2]*z) / 4096.0 + v.trans[0];
    double vy = (v.rot[3]*x + v.rot[4]*y + v.rot[5]*z) / 4096.0 + v.trans[1];
    double vz = (v.rot[6]*x + v.rot[7]*y + v.rot[8]*z) / 4096.0 + v.trans[2];
    if (vz <= 0) return 0;
    *sx = 160.0 + vx * v.fov_screen_dist / vz; *sy = 120.0 + vy * v.fov_screen_dist / vz;
    return *sx >= 0 && *sx < 320 && *sy >= 0 && *sy < 240;
}
/* Abstand zur naechsten Band-8-Wandzelle (0 = drin). */
static double wand(double x, double z)
{
    double best = 1e9;
    for (int i = 0; i < g_nsca; i++) {
        if ((g_sca[i].fl >> 4) != 8) continue;
        double x0 = g_sca[i].x, x1 = x0 + (uint16_t)g_sca[i].w, z0 = g_sca[i].z, z1 = z0 + (uint16_t)g_sca[i].d;
        double dx = x < x0 ? x0 - x : (x > x1 ? x - x1 : 0), dz = z < z0 ? z0 - z : (z > z1 ? z - z1 : 0);
        double d = sqrt(dx*dx + dz*dz);
        if (d < best) best = d;
    }
    return best;
}
static void zeile(const char *nm, double x, double z)
{
    printf("  %-10s (%6.0f,%6.0f) wand %5.0f |", nm, x, z, wand(x, z));
    for (int c = 2; c <= 7; c++) {
        double fx, fy, kx, ky;
        int a = proj(c, x, -14400, z, &fx, &fy), b = proj(c, x, -16200, z, &kx, &ky);
        if (a && b) printf(" c%d F(%3.0f,%3.0f)K(%3.0f,%3.0f)", c, fx, fy, kx, ky);
        else if (b) printf(" c%d K(%3.0f,%3.0f)     ", c, kx, ky);
        else printf(" c%d -                  ", c);
    }
    printf("\n");
}
int main(int argc, char **argv)
{
    long n = 0; uint8_t *d = argc > 1 ? slurp(argv[1], &n) : NULL;
    if (!d) { fprintf(stderr, "RDT fehlt\n"); return 1; }
    g_ncut = d[1]; uint32_t cam = rd32(d + 8 + 7 * 4);
    for (int c = 0; c < g_ncut && c < 16; c++) memcpy(&g_cut[c], d + cam + c * 32, 32);
    uint32_t sca = rd32(d + 8 + 6 * 4); uint32_t cnt0 = rd32(d + sca + 4);
    g_nsca = (int)cnt0; if (g_nsca > 64) g_nsca = 64;
    for (int i = 0; i < g_nsca; i++) memcpy(&g_sca[i], d + sca + 24 + i * 12, 12);
    printf("== Kandidaten (Band 8)\n");
    for (int i = 2; i + 1 < argc; i += 2) {
        char nm[16]; snprintf(nm, sizeof nm, "K%d", i / 2);
        zeile(nm, atof(argv[i]), atof(argv[i + 1]));
    }
    return 0;
}
