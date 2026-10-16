/* test_r38_zopf.c — RIEGEL Spur D (Runde 38) "zopf": Elzas Zopf wie im Original (RE1.5 FUN_80024c30/FUN_80024e40,
 * dieselbe Routine wie Claires Zopf in RE2 FUN_8001844c/FUN_80018728). Dossier analysis/befunde_runde38/D_zopf.md.
 *
 * Teile (je ein ctest-Eintrag):
 *   libgte   Nachbauten: RotMatrix gegen den Port-Zwilling (mat3_from_euler) fuer alle nichtnegativen Winkel, der
 *            Negativ-Zweig (Tabelle DAT_800794c4 nicht antisymmetrisch: Eintrag 4095 = sin 0), RotMatrixYXZ = Ry*Rx*Rz,
 *            RotMatrixY/Z = Vormultiplikation, Winkelhelfer FUN_80025360/FUN_800253fc (Grundwerte, Quadranten)
 *   init     Gate: Modell-Bit 2 (@0x80031d8c) + Init nur fuer 4/5/6 (@0x80031a88-aa0) + 21 Teile; Versaetze des
 *            Kommando-0-Inits (@0x80031b44-80031b80), Leon (0/1/2) nie aktiv
 *   ruhe     echte PL04-Pose (EMR Keyframe 0): der Zopf haengt, die Glieder sind verbunden (Lage = Eltern * Versatz),
 *            die Bewegung kommt zur Ruhe
 *   drehung  Koerper dreht (Tank-Drehung 0x40/Bild): der Zopf bleibt zurueck, schwingt (Vorzeichenwechsel der
 *            Winkelgeschwindigkeit) und kehrt in die Ruhelage zurueck — der Winkelverlauf
 *   laufen   Koerper laeuft vorwaerts: die Zopfspitze bleibt HINTER der Ruhelage zurueck
 *   freeze   ohne Takt (Spieler-Freeze `bltz` @0x80031c78) kein Schritt, Zustand steht
 *   spur <zopf.log>                       Integration: Winkelverlauf aus RE15_ZOPF_LOG (Drehung -> Ausschlag -> Ruhe)
 *   pixel <mit.ppm> <ohne.ppm>            Integration: Pixel am Hinterkopf (Bild mit Zopf gegen Mess-Sonde ohne)
 */
#include "re15_zopf_r38.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_math.h"
#include "re15_md1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846   /* -std=c11 blendet M_PI aus */
#endif

void re15_skel_euler_matrix(int ax, int ay, int az, int32_t m[9]);

static int g_fail = 0;
#define PRUEF(c, ...) do { if (!(c)) { printf("  FEHLER: "); printf(__VA_ARGS__); printf("\n"); g_fail++; } \
                           else { printf("  ok: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

extern const uint32_t re15_trig_lut[4096];

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

/* ---------------------------------------------------------------------------------------------------- libgte */
static void teil_libgte(void)
{
    printf("[libgte]\n");
    /* (1) RotMatrix-Nachbau == Port-Zwilling mat3_from_euler fuer nichtnegative Winkel (beide aus RotMatrix
     *     @0x80068098 abgeleitet, unabhaengig geschrieben). */
    int abw = 0, n = 0;
    for (int ax = 0; ax < 4096; ax += 61)
        for (int ay = 0; ay < 4096; ay += 67)
            for (int az = 0; az < 4096; az += 509) {
                int16_t r[3] = { (int16_t)ax, (int16_t)ay, (int16_t)az }, m[9];
                int32_t p[9];
                re15_zopf_r38_rotmatrix(r, m);
                re15_skel_euler_matrix(ax, ay, az, p);
                for (int i = 0; i < 9; i++) if (m[i] != (int16_t)p[i]) abw++;
                n++;
            }
    PRUEF(abw == 0, "RotMatrix-Nachbau == mat3_from_euler auf %d Winkeltripeln (Abweichungen %d)", n, abw);

    /* (2) Negativ-Zweig: die Tabelle ist nicht antisymmetrisch, darum rechnet RotMatrixY(-1) mit -sin[1] = -6
     *     (`subu t7,zero,t7` @0x800659e0), nicht mit sin[4095]. */
    PRUEF((int16_t)(re15_trig_lut[4095] & 0xffff) == 0 && (int16_t)(re15_trig_lut[1] & 0xffff) == 6,
          "DAT_800794c4: sin[4095] = %d, sin[1] = %d (nicht antisymmetrisch)",
          (int)(int16_t)(re15_trig_lut[4095] & 0xffff), (int)(int16_t)(re15_trig_lut[1] & 0xffff));
    {
        int16_t m[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000 };
        re15_zopf_r38_rotmatrix_y(-1, m);
        PRUEF(m[2] == -6 && m[6] == 6, "RotMatrixY(-1): m02 = %d (sin(-1) = -6), m20 = %d", m[2], m[6]);
        int16_t z[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000 };
        re15_zopf_r38_rotmatrix_z(-1, z);
        PRUEF(z[3] == -6 && z[1] == 6, "RotMatrixZ(-1): m10 = %d, m01 = %d", z[3], z[1]);
    }
    /* (3) RotMatrixYXZ = Ry*Rx*Rz: Gegenrechnung in double mit DENSELBEN Tabellenwerten (Vorzeichenregel des
     *     Negativ-Zweigs unabhaengig hier nachgeschrieben), +-3 LSB fuer die drei Q12-Zwischenrundungen. Die Tabelle
     *     selbst ist nicht exakt periodisch (2. Haelfte um einen Eintrag versetzt) — darum keine sin()/cos(). */
    {
        int maxd = 0;
        for (int ax = -2048; ax < 2048; ax += 97)
            for (int ay = -2048; ay < 4096; ay += 211)
                for (int az = -1000; az < 1000; az += 333) {
                    int16_t r[3] = { (int16_t)ax, (int16_t)ay, (int16_t)az }, m[9];
                    re15_zopf_r38_rotmatrix_yxz(r, m);
                    double sv[3], cv[3];
                    int ang[3] = { ax, ay, az };
                    for (int q = 0; q < 3; q++) {
                        uint32_t w = re15_trig_lut[(uint32_t)abs(ang[q]) & 0xfffu];
                        sv[q] = (ang[q] < 0 ? -1.0 : 1.0) * (int16_t)(w & 0xffff) / 4096.0;
                        cv[q] = (int16_t)(w >> 16) / 4096.0;
                    }
                    double Ry[9] = { cv[1], 0, sv[1], 0, 1, 0, -sv[1], 0, cv[1] };
                    double Rx[9] = { 1, 0, 0, 0, cv[0], -sv[0], 0, sv[0], cv[0] };
                    double Rz[9] = { cv[2], -sv[2], 0, sv[2], cv[2], 0, 0, 0, 1 };
                    double t[9], e[9];
                    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
                        t[i*3+j] = 0; for (int k = 0; k < 3; k++) t[i*3+j] += Rx[i*3+k] * Rz[k*3+j]; }
                    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
                        e[i*3+j] = 0; for (int k = 0; k < 3; k++) e[i*3+j] += Ry[i*3+k] * t[k*3+j]; }
                    for (int i = 0; i < 9; i++) { int d = abs(m[i] - (int)lround(e[i] * 4096)); if (d > maxd) maxd = d; }
                }
        PRUEF(maxd <= 3, "RotMatrixYXZ = Ry*Rx*Rz (max. Abweichung %d LSB)", maxd);
    }
    /* (4) RotMatrixY/Z multiplizieren VON LINKS: RotMatrixY(a, M) == Ry(a)*M. */
    {
        int16_t m[9], ry[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000 };
        int16_t r[3] = { 300, 0, 700 };
        re15_zopf_r38_rotmatrix_yxz(r, m);
        re15_zopf_r38_rotmatrix_y(512, ry);
        int16_t e[9];
        for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
            int32_t s = 0; for (int k = 0; k < 3; k++) s += ry[i*3+k] * m[k*3+j]; e[i*3+j] = (int16_t)(s >> 12); }
        re15_zopf_r38_rotmatrix_y(512, m);
        int maxd = 0; for (int i = 0; i < 9; i++) { int d = abs(m[i] - e[i]); if (d > maxd) maxd = d; }
        PRUEF(maxd <= 1, "RotMatrixY(512, M) == Ry(512)*M (max. %d LSB)", maxd);
    }
    /* (5) Winkelhelfer: Grundwerte bei y == 0 und Quadranten (@0x80025360 / @0x800253fc). */
    PRUEF(re15_zopf_r38_winkel_z(5, 0) == 0x1400 && re15_zopf_r38_winkel_z(-5, 0) == 0xc00,
          "FUN_80025360(x,0): x>=0 -> 0x1400, x<0 -> 0xc00 (%d / %d)",
          re15_zopf_r38_winkel_z(5, 0), re15_zopf_r38_winkel_z(-5, 0));
    PRUEF(re15_zopf_r38_winkel_x(5, 0) == 0xc00 && re15_zopf_r38_winkel_x(-5, 0) == 0x400,
          "FUN_800253fc(x,0): x>=0 -> 0xc00, x<0 -> 0x400 (%d / %d)",
          re15_zopf_r38_winkel_x(5, 0), re15_zopf_r38_winkel_x(-5, 0));
    PRUEF(re15_zopf_r38_winkel_z(0, -100) == re15_catan(0) &&
          re15_zopf_r38_winkel_z(0, 100) == (int16_t)(re15_catan(0) + 0x800),
          "FUN_80025360(0,-100) = catan(0) = %d, (0,+100) = catan(0)+0x800 = %d",
          re15_zopf_r38_winkel_z(0, -100), re15_zopf_r38_winkel_z(0, 100));
    PRUEF(re15_zopf_r38_winkel_x(100, -100) == re15_catan(4096) &&
          re15_zopf_r38_winkel_x(100, 100) == -re15_catan(-4096),
          "FUN_800253fc(100,-100) = catan(4096) = %d, (100,100) = -catan(-4096) = %d",
          re15_zopf_r38_winkel_x(100, -100), re15_zopf_r38_winkel_x(100, 100));
}

/* ---------------------------------------------------------------------------------------------------- init */
static void teil_init(void)
{
    printf("[init]\n");
    static const int erwartet[4][2] = { { -0x73, -0x15a }, { -0x2d, -0x20 }, { -0x65, 0x61 }, { -0x1, 0x6a } };
    for (int m = 0; m < 8; m++) {
        int a21 = re15_zopf_r38_aktiv(m, 21);
        int soll = (m == 4 || m == 5 || m == 6);
        PRUEF(a21 == soll, "Modell %d mit 21 Teilen: Zopf %s (soll %s)", m, a21 ? "an" : "aus", soll ? "an" : "aus");
    }
    PRUEF(re15_zopf_r38_aktiv(0, 17) == 0, "Leon PL00 (17 Teile): kein Zopf");
    PRUEF(re15_zopf_r38_aktiv(4, 15) == 0, "Elza-Modell mit nur 15 Teilen: kein Zopf (Teile 17..20 fehlen)");
    PRUEF(re15_zopf_r38_aktiv(4, 21) == 1, "Elza PL04 (21 Teile): Zopf an");
    int ok = 1;
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
        if (!g || g->off[0] != erwartet[k][0] || g->off[1] != erwartet[k][1] || g->off[2] != 0 ||
            g->v94 != 0 || g->v96 != 0 || g->rot[1] != 0) ok = 0;
        printf("    Glied %d (Teil %d): Versatz (%d,%d,%d)\n", k, RE15_ZOPF_ERSTES_TEIL + k,
               g ? g->off[0] : -1, g ? g->off[1] : -1, g ? g->off[2] : -1);
    }
    PRUEF(ok, "Kommando-0-Init: Versaetze (-0x73,-0x15a) (-0x2d,-0x20) (-0x65,0x61) (-1,0x6a), v94/v96/rot.vy = 0");
    PRUEF(re15_zopf_r38_zeichnen(4, 21) == 0, "vor dem ersten Treiber-Schritt wird nichts gezeichnet");
}

/* ------------------------------------------------------------------------------------- echte PL04-Pose */
static re15_emd_skeleton_t s_sk;
static uint8_t *s_emr = NULL;
static re15_skel_pose_t s_po[RE15_EMD_MAX_BONES];

static int pose_laden(void)
{
    char p[512];
    snprintf(p, sizeof p, "%s/PLD/PL04.EMR", RE15_ASSET_PSX_DIR);
    size_t n = 0;
    s_emr = slurp(p, &n);
    if (!s_emr || re15_emd_parse_skeleton(s_emr, n, &s_sk) != 0) { printf("  FEHLER: %s nicht ladbar\n", p); return 0; }
    g_anim_pose_actor = NULL;
    g_skel_lokal_rot = re15_zopf_r38_lokal();
    int ok = re15_skel_compute_pose(&s_sk, 0, s_po) == 0;
    g_skel_lokal_rot = NULL;
    return ok;
}

static void eingang(re15_zopf_eingang_t *in, int32_t x, int32_t z, int16_t yaw)
{
    memset(in, 0, sizeof *in);
    in->ent_rot[1] = yaw;
    in->ent_pos[0] = x; in->ent_pos[1] = 0; in->ent_pos[2] = z;
    memcpy(in->l0_rot, s_po[0].rot, sizeof in->l0_rot);
    memcpy(in->l0_t, s_po[0].trans, sizeof in->l0_t);
    memcpy(in->l8_rot, re15_zopf_r38_lokal()[8], sizeof in->l8_rot);
    for (int i = 0; i < 3; i++) in->l8_t[i] = s_sk.bone_relative_pos[8][i];
    int16_t ax, ay, az;
    re15_emd_get_keyframe_angles(&s_sk, 0, 0, &ax, &ay, &az); in->teil0_vy = ay; in->teil0_vz = az;
    re15_emd_get_keyframe_angles(&s_sk, 0, 8, &ax, &ay, &az); in->teil8_vy = ay;
    in->nacken_flags = 0x12;   /* Bit 0 geloescht -> Nacken-Gier (0) wird addiert, wie im Spiel */
}

/* Lage des Glieds k im KOERPER-Rahmen (Gier ausgerechnet): R_y(-yaw) * (t - pos). */
static void koerperlage(int k, int32_t x, int32_t z, int16_t yaw, double out[3])
{
    const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
    double a = yaw * 2 * M_PI / 4096, dx = g->t[0] - x, dz = g->t[2] - z;
    /* Ry = [c 0 s; 0 1 0; -s 0 c] (RotMatrix-Konvention); Inverse = Transponierte */
    out[0] = cos(a) * dx - sin(a) * dz;
    out[1] = g->t[1];
    out[2] = sin(a) * dx + cos(a) * dz;
}

/* Verbunden? Lage(k) == Eltern * Versatz nach @0x8002502c-08c (unabhaengig nachgerechnet, ohne Packungs-Trick:
 * die Abweichung ist hoechstens 1 Einheit, weil Bits 12..15 von t.x nur 15/16 in vy einbringen). */
static int verbunden(int k, const int16_t pm[9], const int32_t pt[3])
{
    const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
    for (int i = 0; i < 3; i++) {
        double s = (pm[i*3+0] * (double)g->off[0] + pm[i*3+1] * (double)g->off[1] + pm[i*3+2] * (double)g->off[2]) / 4096.0;
        if (fabs(g->t[i] - (pt[i] + s)) > 2.0) return 0;
    }
    return 1;
}

static double dist3(const double a[3], const double b[3])
{
    return sqrt((a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2]));
}

static void ruhe_einschwingen(int32_t x, int32_t z, int16_t yaw, int n)
{
    re15_zopf_eingang_t in;
    eingang(&in, x, z, yaw);
    for (int i = 0; i < n; i++) { re15_zopf_r38_takt(1); re15_zopf_r38_bild(&in); }
}

static void teil_ruhe(void)
{
    printf("[ruhe]\n");
    if (!pose_laden()) { g_fail++; return; }
    (void)re15_zopf_r38_aktiv(4, 21);                 /* PLD-Laden + Kommando 0 (Modellwechsel) */
    ruhe_einschwingen(1000, 2000, 0, 400);
    re15_zopf_eingang_t in; eingang(&in, 1000, 2000, 0);
    /* ein weiterer Schritt: Bewegung in Ruhe? */
    const re15_zopf_glied_t *g3 = re15_zopf_r38_glied(3);
    int32_t vor[3] = { g3->t[0], g3->t[1], g3->t[2] };
    int maxv = 0;
    for (int i = 0; i < 30; i++) {
        re15_zopf_r38_takt(1); re15_zopf_r38_bild(&in);
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
            if (abs(g->v94) > maxv) maxv = abs(g->v94);
            if (abs(g->v96) > maxv) maxv = abs(g->v96);
        }
    }
    int dr = abs(g3->t[0] - vor[0]) + abs(g3->t[1] - vor[1]) + abs(g3->t[2] - vor[2]);
    PRUEF(maxv <= 8 && dr <= 8, "Ruhe: max |v94|,|v96| = %d, Spitze bewegt sich in 30 Bildern um %d Einheiten", maxv, dr);
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
        printf("    Glied %d: rot=(%d,%d,%d) v=(%d,%d) Lage=(%d,%d,%d)\n", k, g->rot[0], g->rot[1], g->rot[2],
               g->v94, g->v96, g->t[0], g->t[1], g->t[2]);
    }
    /* Verbindung: Glied k+1 haengt an Glied k (Elternmatrix = Weltmatrix des Vorgaengers @0x80024d5c/da4/ddc). */
    int verb = 1;
    for (int k = 1; k < RE15_ZOPF_GLIEDER; k++) {
        const re15_zopf_glied_t *p = re15_zopf_r38_glied(k - 1);
        if (!verbunden(k, p->m, p->t)) verb = 0;
    }
    PRUEF(verb, "Glieder 18..20 haengen an ihrem Vorgaenger (Lage = Eltern-Weltmatrix * Versatz)");
    /* Haengt: die NETZE der Teile 17..20 (PL04.MD1) mit den Glied-Weltmatrizen gesetzt — der tiefste Vertex liegt
     * deutlich unter dem Ansatz (PSX +y = unten), der hinterste deutlich hinter ihm (Modell blickt bei Gier 0
     * nach +x, s. Spurdossier §4). */
    const re15_zopf_glied_t *g0 = re15_zopf_r38_glied(0);
    double l0[3], l3[3];
    koerperlage(0, 1000, 2000, 0, l0); koerperlage(3, 1000, 2000, 0, l3);
    printf("    Koerperrahmen: Ansatz x=%.0f z=%.0f, Glied 20 x=%.0f z=%.0f\n", l0[0], l0[2], l3[0], l3[2]);
    {
        char p[512]; size_t n = 0;
        snprintf(p, sizeof p, "%s/PLD/PL04.MD1", RE15_ASSET_PSX_DIR);
        uint8_t *b = slurp(p, &n);
        static re15_md1_t md;
        if (!b || re15_md1_parse(b, (int)n, &md) != 0 || md.mesh_count < RE15_ZOPF_MIN_TEILE) {
            printf("  FEHLER: %s nicht ladbar / zu wenige Teile\n", p); g_fail++; free(b); return;
        }
        int32_t ymax = -(1 << 30), xmin = 1 << 30; int nv = 0;
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
            const re15_md1_mesh_t *ms = &md.meshes[RE15_ZOPF_ERSTES_TEIL + k];
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *vs = pass ? ms->quad_vertices : ms->tri_vertices;
                int cnt = pass ? ms->quad_vertex_count : ms->tri_vertex_count;
                for (int i = 0; vs && i < cnt; i++) {
                    int32_t w[3];
                    for (int r = 0; r < 3; r++)
                        w[r] = ((g->m[r*3+0] * vs[i].x + g->m[r*3+1] * vs[i].y + g->m[r*3+2] * vs[i].z) >> 12) + g->t[r];
                    if (w[1] > ymax) ymax = w[1];
                    if (w[0] - 1000 < xmin) xmin = w[0] - 1000;   /* Gier 0: Koerper-x = Welt-x - X */
                    nv++;
                }
            }
        }
        printf("    %d Zopf-Vertices: tiefster y=%d (Ansatz y=%d), hinterster x=%d (Ansatz x=%.0f)\n",
               nv, (int)ymax, (int)g0->t[1], (int)xmin, l0[0]);
        PRUEF(ymax > g0->t[1] + 250, "Zopf haengt: Netz reicht %d Einheiten unter den Ansatz (> 250)", (int)(ymax - g0->t[1]));
        PRUEF(xmin < l0[0] - 150, "Zopf liegt hinter dem Kopf: Netz reicht %.0f Einheiten hinter den Ansatz (> 150)",
              l0[0] - xmin);
        free(b);
    }
}

static void teil_drehung(void)
{
    printf("[drehung]\n");
    if (!pose_laden()) { g_fail++; return; }
    (void)re15_zopf_r38_aktiv(0, 17); (void)re15_zopf_r38_aktiv(4, 21);   /* Modellwechsel = frischer Zopf */
    const int32_t X = 1000, Z = 2000;
    ruhe_einschwingen(X, Z, 0, 400);
    double ruhe[3]; koerperlage(3, X, Z, 0, ruhe);
    /* Tank-Drehung: 16 Bilder je 0x40 = 0x400 (90 Grad), danach stehen. Gemessen wird der SEITLICHE Ausschlag der
     * Zopfspitze (Glied 20) im Koerperrahmen (z, quer zur Blickrichtung +x) gegen die Ruhelage — der Winkelverlauf
     * als Winkel atan2(seitlich, Abstand Spitze-Ansatz). */
    int16_t yaw = 0;
    double maxaus = 0, maxseit = 0, minseit = 0; int max_bild = -1, ruhig_ab = -1, wechsel = 0, last = 0;
    re15_zopf_eingang_t in;
    printf("    Bild  yaw  seitlich  Winkel(Grad)  v94/v96 Glieder 17..20\n");
    for (int f = 0; f < 240; f++) {
        if (f < 16) yaw = (int16_t)(yaw + 0x40);
        eingang(&in, X, Z, yaw);
        re15_zopf_r38_takt(1); re15_zopf_r38_bild(&in);
        double l[3], a0[3]; koerperlage(3, X, Z, yaw, l); koerperlage(0, X, Z, yaw, a0);
        double aus = dist3(l, ruhe), seit = l[2] - ruhe[2];
        double len = sqrt((l[0]-a0[0])*(l[0]-a0[0]) + (l[1]-a0[1])*(l[1]-a0[1]));
        double winkel = atan2(seit, len > 1 ? len : 1) * 180.0 / M_PI;
        if (aus > maxaus) { maxaus = aus; max_bild = f; }
        if (seit > maxseit) maxseit = seit;
        if (seit < minseit) minseit = seit;
        int sg = seit > 3 ? 1 : (seit < -3 ? -1 : 0);
        if (sg && last && sg != last) wechsel++;
        if (sg) last = sg;
        if (aus > 20) ruhig_ab = -1; else if (ruhig_ab < 0) ruhig_ab = f;
        if (f < 40 || f % 10 == 0) {
            printf("    %4d %5d %8.1f %10.1f   ", f, yaw, seit, winkel);
            for (int k = 0; k < RE15_ZOPF_GLIEDER; k++)
                printf(" %d/%d", re15_zopf_r38_glied(k)->v94, re15_zopf_r38_glied(k)->v96);
            printf("\n");
        }
    }
    PRUEF(maxaus > 40, "Drehung reisst den Zopf aus der Ruhelage: max. Ausschlag der Spitze %.0f Einheiten (Bild %d)",
          maxaus, max_bild);
    PRUEF(wechsel >= 2 && maxseit > 5 && minseit < -5,
          "Pendel schwingt durch die Ruhelage: %d Seitenwechsel, seitlich %.0f..%.0f Einheiten", wechsel, minseit, maxseit);
    PRUEF(ruhig_ab >= 0 && ruhig_ab < 230, "Pendel kommt zur Ruhe: Ausschlag <= 20 ab Bild %d", ruhig_ab);
}

static void teil_laufen(void)
{
    printf("[laufen]\n");
    if (!pose_laden()) { g_fail++; return; }
    (void)re15_zopf_r38_aktiv(0, 17); (void)re15_zopf_r38_aktiv(4, 21);
    int32_t X = 1000, Z = 2000;
    ruhe_einschwingen(X, Z, 0, 400);
    double ruhe[3]; koerperlage(3, X, Z, 0, ruhe);
    /* vorwaerts = Blickrichtung des Modells bei Gier 0: Lage-Differenz Kopf -> Gesicht unbekannt, darum beide
     * Richtungen messen: die Spitze muss GEGEN die Laufrichtung ausweichen. */
    re15_zopf_eingang_t in;
    double sum_dx = 0;
    for (int f = 0; f < 20; f++) {
        X += 40;                                   /* 40 Einheiten/Bild in +x */
        eingang(&in, X, Z, 0);
        re15_zopf_r38_takt(1); re15_zopf_r38_bild(&in);
        double l[3]; koerperlage(3, X, Z, 0, l);
        sum_dx += l[0] - ruhe[0];
    }
    PRUEF(sum_dx / 20 < -15, "Laufen in +x: Zopfspitze bleibt zurueck (mittlere Verschiebung x %.1f < -15)", sum_dx / 20);
}

static void teil_freeze(void)
{
    printf("[freeze]\n");
    if (!pose_laden()) { g_fail++; return; }
    (void)re15_zopf_r38_aktiv(0, 17); (void)re15_zopf_r38_aktiv(4, 21);
    ruhe_einschwingen(0, 0, 0, 50);
    re15_zopf_eingang_t in; eingang(&in, 0, 0, 0x300);
    re15_zopf_glied_t vor = *re15_zopf_r38_glied(0);
    re15_zopf_r38_takt(0);
    int r = re15_zopf_r38_bild(&in);
    const re15_zopf_glied_t *g = re15_zopf_r38_glied(0);
    PRUEF(r == 0 && memcmp(&vor, g, sizeof vor) == 0, "Spieler-Freeze (kein Takt): kein Schritt, Zustand unveraendert");
    re15_zopf_r38_takt(1);
    r = re15_zopf_r38_bild(&in);
    PRUEF(r == 1 && re15_zopf_r38_glied(0)->rot[1] != vor.rot[1], "mit Takt: ein Schritt (rot.vy %d -> %d)",
          vor.rot[1], re15_zopf_r38_glied(0)->rot[1]);
    r = re15_zopf_r38_bild(&in);
    PRUEF(r == 0, "ein Takt = hoechstens ein Schritt je Bild");
}

/* ---------------------------------------------------------------------------------------- Integration */
/* spur: RE15_ZOPF_LOG ab der letzten Raum-Marke "R kommando0" (Zopf-Init beim Raumeintritt). Je Schritt "Z<n> s0=<a>
 * g0=(rx,ry,rz v94=.. v96=.. t=x,y,z) .. g3=(...)": Koerperwinkel s0, die Winkelgeschwindigkeiten aller Glieder und der
 * SEITLICHE Winkel der Zopfspitze (Glied 20 gegen Glied 17) im Gierrahmen Ry(s0). Erwartet: s0 dreht sich (Summe der
 * Aenderungen >= 0x200), der Zopf schlaegt seitlich aus (>= 5 Grad gegen die Ruhelage), schwingt durch die Ruhelage
 * (>= 2 Seitenwechsel, Hysterese 1 Grad) und kommt danach zur Ruhe (>= 15 Schritte am Stueck mit |v| <= 6 und
 * Winkelaenderung <= 0.6 Grad je Schritt). Ruhelage = Mittel dieses Ruhefensters. Der Verlauf geht nach <log>.verlauf. */
#define SPUR_MAX 4096
static int teil_spur(const char *pfad)
{
    printf("[spur] %s\n", pfad);
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("  FEHLER: %s fehlt\n", pfad); return 1; }
    static char zeilen[SPUR_MAX][1024];
    static int    S0[SPUR_MAX], V[SPUR_MAX];
    static double W[SPUR_MAX];
    int nz = 0, start = 0;
    while (nz < SPUR_MAX && fgets(zeilen[nz], sizeof zeilen[0], f)) {
        if (!strncmp(zeilen[nz], "R kommando0", 11)) start = nz;
        nz++;
    }
    fclose(f);
    int n = 0;
    for (int i = start; i < nz; i++) {
        int idx, s0, rx, ry, rz, v94, v96, tx[4], ty[4], tz[4], v = 0, ok = 1;
        if (sscanf(zeilen[i], "Z%d s0=%d", &idx, &s0) != 2) continue;
        const char *p = zeilen[i];
        for (int k = 0; k < 4 && ok; k++) {
            char key[8]; snprintf(key, sizeof key, " g%d=(", k);
            p = strstr(p, key);
            if (!p || sscanf(p + 5, "%d,%d,%d v94=%d v96=%d t=%d,%d,%d", &rx, &ry, &rz, &v94, &v96,
                             &tx[k], &ty[k], &tz[k]) != 8) { ok = 0; break; }
            if (abs(v94) > v) v = abs(v94);
            if (abs(v96) > v) v = abs(v96);
            p += 5;
        }
        if (!ok) continue;
        double a = s0 * 2 * M_PI / 4096, dx = tx[3] - tx[0], dy = ty[3] - ty[0], dz = tz[3] - tz[0];
        double seit = dx * sin(a) + dz * cos(a), rueck = dx * cos(a) - dz * sin(a);
        S0[n] = s0; V[n] = v; W[n] = atan2(seit, sqrt(rueck * rueck + dy * dy)) * 180.0 / M_PI;
        n++;
    }
    char vp[600]; snprintf(vp, sizeof vp, "%s.verlauf", pfad);
    FILE *vo = fopen(vp, "w");
    for (int i = 0; vo && i < n; i++) fprintf(vo, "%d s0=%d seitwinkel=%.1f vmax=%d\n", i, S0[i], W[i], V[i]);
    if (vo) fclose(vo);
    /* Drehung (modular), groesster Ausschlag, Ruhefenster danach */
    int drehung = 0, vmax = 0, imax = 5;
    for (int i = 1; i < n; i++) {
        int d = ((S0[i] - S0[i-1]) % 4096 + 4096 + 2048) % 4096 - 2048;
        drehung += abs(d);
        if (i > 5 && V[i] > vmax) vmax = V[i];
    }
    for (int i = 5; i < n; i++) if (fabs(W[i]) > fabs(W[imax])) imax = i;
    int best = 0, best_s = -1, run = 0, run_s = -1;
    for (int i = imax + 1; i < n; i++) {
        if (V[i] <= 6 && fabs(W[i] - W[i-1]) <= 0.6) { if (!run) run_s = i; run++; if (run > best) { best = run; best_s = run_s; } }
        else run = 0;
    }
    double ruhe = 0;
    for (int i = 0; i < best; i++) ruhe += W[best_s + i];
    if (best) ruhe /= best;
    double maxaus = 0; int wechsel = 0, last = 0;
    int ende = best_s > 0 ? best_s : n;
    for (int i = 5; i < ende; i++) {
        double d = W[i] - ruhe;
        if (fabs(d) > maxaus) maxaus = fabs(d);
        int sg = d > 1.0 ? 1 : (d < -1.0 ? -1 : 0);
        if (sg && last && sg != last) wechsel++;
        if (sg) last = sg;
    }
    printf("    %d Schritte im Zielraum, Drehung %d, groesster Ausschlag Schritt %d (%.1f Grad), Ruhe ab Schritt %d"
           " (%d Schritte, %.1f Grad), %d Seitenwechsel\n", n, drehung, imax, W[imax], best_s, best, ruhe, wechsel);
    PRUEF(n >= 60, "%d Zopf-Schritte im Zielraum protokolliert", n);
    PRUEF(drehung >= 0x200, "Koerperwinkel s0 dreht sich um %d (>= 0x200)", drehung);
    PRUEF(vmax >= 8, "Zopfglieder schwingen mit: max |v94|/|v96| = %d (>= 8)", vmax);
    PRUEF(maxaus >= 5.0, "Zopfspitze schlaegt seitlich aus: %.1f Grad gegen die Ruhelage %.1f", maxaus, ruhe);
    PRUEF(wechsel >= 2, "Pendel schwingt durch die Ruhelage: %d Seitenwechsel (>= 2)", wechsel);
    PRUEF(best >= 15, "danach Ruhe: %d Schritte am Stueck ruhig (>= 15)", best);
    return g_fail ? 1 : 0;
}

static uint8_t *ppm(const char *p, int *w, int *h)
{
    size_t n = 0;
    uint8_t *b = slurp(p, &n);
    if (!b) return NULL;
    int off = 0, feld = 0, v[3] = {0, 0, 0};
    if (n < 2 || b[0] != 'P' || b[1] != '6') { free(b); return NULL; }
    off = 2;
    while (feld < 3 && (size_t)off < n) {
        while ((size_t)off < n && (b[off] == ' ' || b[off] == '\n' || b[off] == '\r' || b[off] == '\t')) off++;
        if (b[off] == '#') { while ((size_t)off < n && b[off] != '\n') off++; continue; }
        int x = 0; while ((size_t)off < n && b[off] >= '0' && b[off] <= '9') x = x * 10 + (b[off++] - '0');
        v[feld++] = x;
    }
    off++;
    *w = v[0]; *h = v[1];
    if ((size_t)(off + v[0] * v[1] * 3) > n) { free(b); return NULL; }
    uint8_t *px = (uint8_t *)malloc((size_t)v[0] * v[1] * 3);
    memcpy(px, b + off, (size_t)v[0] * v[1] * 3);
    free(b);
    return px;
}

/* pixel: dasselbe Bild mit Zopf und mit der Mess-Sonde RE15_ZOPF_SONDE_AUS (Zopf gerechnet, nicht gezeichnet).
 * Erwartet: >= 25 Pixel unterscheiden sich, alle im Fenster x0..x1/y0..y1 um Elzas Hinterkopf. */
static int teil_pixel(const char *mit, const char *ohne, int x0, int y0, int x1, int y1)
{
    printf("[pixel] %s gegen %s, Fenster (%d,%d)-(%d,%d)\n", mit, ohne, x0, y0, x1, y1);
    int w1, h1, w2, h2;
    uint8_t *a = ppm(mit, &w1, &h1), *b = ppm(ohne, &w2, &h2);
    if (!a || !b || w1 != w2 || h1 != h2) { printf("  FEHLER: Bilder fehlen/passen nicht\n"); return 1; }
    int diff = 0, innen = 0, bx0 = w1, by0 = h1, bx1 = -1, by1 = -1;
    for (int y = 0; y < h1; y++)
        for (int x = 0; x < w1; x++) {
            const uint8_t *p = a + (y * w1 + x) * 3, *q = b + (y * w1 + x) * 3;
            if (abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2]) > 24) {
                diff++;
                if (x >= x0 && x <= x1 && y >= y0 && y <= y1) innen++;
                if (x < bx0) bx0 = x; if (y < by0) by0 = y; if (x > bx1) bx1 = x; if (y > by1) by1 = y;
            }
        }
    printf("    %d Pixel verschieden, %d davon im Fenster, Huelle (%d,%d)-(%d,%d)\n", diff, innen, bx0, by0, bx1, by1);
    PRUEF(diff >= 25, "Zopfglieder zeichnen sichtbar: %d Pixel verschieden (>= 25)", diff);
    PRUEF(innen == diff, "alle Unterschiede am Hinterkopf-Fenster (%d von %d)", innen, diff);
    free(a); free(b);
    return g_fail ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Teil fehlt\n"); return 2; }
    const char *t = argv[1];
    if (!strcmp(t, "spur") && argc >= 3) return teil_spur(argv[2]);
    if (!strcmp(t, "pixel") && argc >= 8)
        return teil_pixel(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
    if (!strcmp(t, "libgte")) teil_libgte();
    else if (!strcmp(t, "init")) teil_init();
    else if (!strcmp(t, "ruhe")) teil_ruhe();
    else if (!strcmp(t, "drehung")) teil_drehung();
    else if (!strcmp(t, "laufen")) teil_laufen();
    else if (!strcmp(t, "freeze")) teil_freeze();
    else { printf("unbekannter Teil %s\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
