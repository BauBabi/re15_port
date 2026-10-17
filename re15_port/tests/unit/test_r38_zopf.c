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
 *   original Datensatz aus der ORIGINAL-RAM (r38_zopf_orig.inc, DuckStation-GDB @0x80024c30/@0x80024e10, Elza ROOM1121):
 *            der Port rechnet 235 Bilder (Ruhe, Drehung R, Nachlauf, Gehen U, Stehen) fortlaufend BITGLEICH nach
 *   orig <frames.txt>                     Werkzeug: dieselbe Nachrechnung gegen die volle Messung (Glied fuer Glied)
 *   skelett                               Werkzeug: PL04 Keyframe 0 + eingeschwungener Zopf, Huellen je Teil
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

/* skelett (Werkzeug, kein Riegel): PL04 Keyframe 0 im Koerperrahmen (Gier 0, Lage 0) — je Knochen Elter, EMR-Versatz,
 * Lage und die Vertex-Huelle des Netzes; danach der eingeschwungene Zopf (Teile 17..20) mit derselben Huelle. */
static void huelle_netz(const re15_md1_mesh_t *ms, const int32_t *m, int m_stride16, const int32_t t[3], int32_t lo[3],
                        int32_t hi[3])
{
    for (int i = 0; i < 3; i++) { lo[i] = 1 << 30; hi[i] = -(1 << 30); }
    for (int pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *vs = pass ? ms->quad_vertices : ms->tri_vertices;
        int cnt = pass ? ms->quad_vertex_count : ms->tri_vertex_count;
        for (int i = 0; vs && i < cnt; i++)
            for (int r = 0; r < 3; r++) {
                int32_t w = ((m[r*3+0] * vs[i].x + m[r*3+1] * vs[i].y + m[r*3+2] * vs[i].z) >> 12) + t[r];
                if (w < lo[r]) lo[r] = w;
                if (w > hi[r]) hi[r] = w;
            }
    }
    (void)m_stride16;
}

static void teil_skelett(void)
{
    printf("[skelett] PL04 Keyframe 0, Koerperrahmen (Gier 0, Lage 0; PSX: -y = oben)\n");
    if (!pose_laden()) { g_fail++; return; }
    char p[512]; size_t n = 0;
    snprintf(p, sizeof p, "%s/PLD/PL04.MD1", RE15_ASSET_PSX_DIR);
    uint8_t *b = slurp(p, &n);
    static re15_md1_t md;
    if (!b || re15_md1_parse(b, (int)n, &md) != 0) { printf("  FEHLER: %s\n", p); g_fail++; free(b); return; }
    int32_t ymin_koerper = 1 << 30;
    for (int k = 0; k < s_sk.bone_count && k < md.mesh_count; k++) {
        int32_t lo[3], hi[3];
        huelle_netz(&md.meshes[k], s_po[k].rot, 0, s_po[k].trans, lo, hi);
        if (lo[1] < ymin_koerper) ymin_koerper = lo[1];
        printf("    Teil %2d Elter %2d EMR (%5d,%5d,%5d) Lage (%5d,%5d,%5d)  Netz x %5d..%5d y %5d..%5d z %5d..%5d\n", k,
               (int)s_sk.bone_parent[k], (int)s_sk.bone_relative_pos[k][0], (int)s_sk.bone_relative_pos[k][1],
               (int)s_sk.bone_relative_pos[k][2], (int)s_po[k].trans[0], (int)s_po[k].trans[1], (int)s_po[k].trans[2],
               (int)lo[0], (int)hi[0], (int)lo[1], (int)hi[1], (int)lo[2], (int)hi[2]);
    }
    (void)re15_zopf_r38_aktiv(0, 17); (void)re15_zopf_r38_aktiv(4, 21);
    ruhe_einschwingen(0, 0, 0, 400);
    int32_t ymin_zopf = 1 << 30, rmax = 0;
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
        int32_t m[9], lo[3], hi[3];
        for (int i = 0; i < 9; i++) m[i] = g->m[i];
        huelle_netz(&md.meshes[RE15_ZOPF_ERSTES_TEIL + k], m, 0, g->t, lo, hi);
        if (lo[1] < ymin_zopf) ymin_zopf = lo[1];
        for (int sx = 0; sx < 2; sx++) for (int sz = 0; sz < 2; sz++) {
            int32_t x = sx ? hi[0] : lo[0], z = sz ? hi[2] : lo[2];
            int32_t r = (int32_t)sqrt((double)x * x + (double)z * z);
            if (r > rmax) rmax = r;
        }
        printf("    Zopf Teil %d Lage (%5d,%5d,%5d) rot (%d,%d,%d)  Netz x %5d..%5d y %5d..%5d z %5d..%5d\n",
               RE15_ZOPF_ERSTES_TEIL + k, (int)g->t[0], (int)g->t[1], (int)g->t[2], g->rot[0], g->rot[1], g->rot[2],
               (int)lo[0], (int)hi[0], (int)lo[1], (int)hi[1], (int)lo[2], (int)hi[2]);
    }
    printf("    hoechster Koerperpunkt y %d, hoechster Zopfpunkt y %d, Zopf waagerecht hoechstens %d von der Achse\n",
           (int)ymin_koerper, (int)ymin_zopf, (int)rmax);
    free(b);
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

/* orig <frames.txt>: Nachrechnung gegen die ORIGINAL-RAM (D_belege/orig_zopf_gdb.py, DuckStation-GDB, Haltepunkte
 * @0x80024c30 / @0x80024e10). Je Bild: Eingang (Spieler +0x68/+0x34.., Teil 0 +0x18, Teil 8 +0x18, Winkel, Nacken) und
 * die Glieder 17..20 VOR dem Treiber; erwartet die Glieder NACH dem Treiber und Teil 8 +0x40.
 *   einzeln     je Bild Zustand aus "vor" setzen, ein re15_zopf_r38_schritt, Vergleich BITGENAU mit "nach"/"w8"
 *   fortlaufend Zustand nur aus Bild 0, danach traegt der Port seinen eigenen Zustand weiter (Vergleich wie oben)
 *   Kette       Original selbst: "vor" von Bild n+1 == "nach" von Bild n (sonst schreibt noch jemand in die Teile) */
#define ORIG_MAX 1024
typedef struct {
    re15_zopf_eingang_t in;
    re15_zopf_glied_t vor[RE15_ZOPF_GLIEDER];
    re15_zopf_glied_t nach[RE15_ZOPF_GLIEDER];
    int16_t w8m[9];
    int32_t w8t[3];
    char phase[16];
} orig_bild_t;

static int orig_lesen(const char *pfad, orig_bild_t *b, int max)
{
    FILE *f = fopen(pfad, "r");
    if (!f) return -1;
    static char z[16384];
    int n = 0;
    while (n < max && fgets(z, sizeof z, f)) {
        if (z[0] != 'F') continue;
        orig_bild_t *o = &b[n];
        memset(o, 0, sizeof *o);
        long v[400]; int nv = 0;
        int feld = 0;   /* Felder 0..2 = "F", Bildnummer, Phase; danach Marken und Zahlen */
        char *s = z;
        for (;;) {
            while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
            if (!*s) break;
            char *tok = s;
            while (*s && *s != ' ' && *s != '\t' && *s != '\n' && *s != '\r') s++;
            char ende = *s; *s = '\0';
            if (feld < 3) { if (feld == 2) snprintf(o->phase, sizeof o->phase, "%s", tok); feld++; }
            else if ((tok[0] >= '0' && tok[0] <= '9') || tok[0] == '-') { if (nv < 400) v[nv++] = strtol(tok, NULL, 10); }
            /* sonst Marke: pad=.., ent, l0, l8, a, vor, nach, w8 */
            if (!ende) break;
            s++;
        }
        if (nv != 6 + 12 + 12 + 5 + 4 * 20 + 4 * 17 + 12) { printf("  Zeile %d: %d Zahlen\n", n, nv); continue; }
        int p = 0;
        for (int i = 0; i < 3; i++) o->in.ent_rot[i] = (int16_t)v[p++];
        for (int i = 0; i < 3; i++) o->in.ent_pos[i] = (int32_t)v[p++];
        for (int i = 0; i < 9; i++) o->in.l0_rot[i] = (int32_t)v[p++];
        for (int i = 0; i < 3; i++) o->in.l0_t[i] = (int32_t)v[p++];
        for (int i = 0; i < 9; i++) o->in.l8_rot[i] = (int32_t)v[p++];
        for (int i = 0; i < 3; i++) o->in.l8_t[i] = (int32_t)v[p++];
        o->in.teil0_vy = (int16_t)v[p++]; o->in.teil0_vz = (int16_t)v[p++]; o->in.teil8_vy = (int16_t)v[p++];
        o->in.nacken_gier = (int16_t)v[p++]; o->in.nacken_flags = (uint8_t)v[p++];
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            re15_zopf_glied_t *g = &o->vor[k];
            for (int i = 0; i < 3; i++) g->off[i] = (int32_t)v[p++];
            for (int i = 0; i < 9; i++) g->m[i] = (int16_t)v[p++];
            for (int i = 0; i < 3; i++) g->t[i] = (int32_t)v[p++];
            for (int i = 0; i < 3; i++) g->rot[i] = (int16_t)v[p++];
            g->v94 = (int16_t)v[p++]; g->v96 = (int16_t)v[p++];
        }
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            re15_zopf_glied_t *g = &o->nach[k];
            g->off[0] = o->vor[k].off[0]; g->off[1] = o->vor[k].off[1]; g->off[2] = o->vor[k].off[2];
            for (int i = 0; i < 9; i++) g->m[i] = (int16_t)v[p++];
            for (int i = 0; i < 3; i++) g->t[i] = (int32_t)v[p++];
            for (int i = 0; i < 3; i++) g->rot[i] = (int16_t)v[p++];
            g->v94 = (int16_t)v[p++]; g->v96 = (int16_t)v[p++];
        }
        for (int i = 0; i < 9; i++) o->w8m[i] = (int16_t)v[p++];
        for (int i = 0; i < 3; i++) o->w8t[i] = (int32_t)v[p++];
        n++;
    }
    fclose(f);
    return n;
}

static int glied_gleich(const re15_zopf_glied_t *a, const re15_zopf_glied_t *b)
{
    for (int i = 0; i < 9; i++) if (a->m[i] != b->m[i]) return 0;
    for (int i = 0; i < 3; i++) if (a->t[i] != b->t[i] || a->rot[i] != b->rot[i]) return 0;
    return a->v94 == b->v94 && a->v96 == b->v96;
}

static void glied_zeigen(const char *tag, const re15_zopf_glied_t *g)
{
    printf("      %s m=[%d %d %d|%d %d %d|%d %d %d] t=(%d,%d,%d) rot=(%d,%d,%d) v=%d/%d\n", tag, g->m[0], g->m[1], g->m[2],
           g->m[3], g->m[4], g->m[5], g->m[6], g->m[7], g->m[8], (int)g->t[0], (int)g->t[1], (int)g->t[2], g->rot[0],
           g->rot[1], g->rot[2], g->v94, g->v96);
}

/* Ein Bild vergleichen; liefert Zahl der abweichenden Glieder (+1 fuer Teil 8). */
static int orig_bild_pruefen(const orig_bild_t *o, int bild, int zeigen)
{
    int fehl = 0;
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
        if (!glied_gleich(g, &o->nach[k])) {
            fehl++;
            if (zeigen) {
                printf("    Bild %d (%s) Glied %d (Teil %d) weicht ab:\n", bild, o->phase, k, RE15_ZOPF_ERSTES_TEIL + k);
                glied_zeigen("Port    ", g);
                glied_zeigen("Original", &o->nach[k]);
            }
        }
    }
    int16_t m[9]; int32_t t[3];
    re15_zopf_r38_kopf_welt(m, t);
    if (memcmp(m, o->w8m, sizeof m) || t[0] != o->w8t[0] || t[1] != o->w8t[1] || t[2] != o->w8t[2]) {
        fehl++;
        if (zeigen)
            printf("    Bild %d Teil 8 +0x40 weicht ab: Port t=(%d,%d,%d) m0=%d, Original t=(%d,%d,%d) m0=%d\n", bild,
                   (int)t[0], (int)t[1], (int)t[2], m[0], (int)o->w8t[0], (int)o->w8t[1], (int)o->w8t[2], o->w8m[0]);
    }
    return fehl;
}

static int teil_orig(const char *pfad)
{
    printf("[orig] %s\n", pfad);
    static orig_bild_t b[ORIG_MAX];
    int n = orig_lesen(pfad, b, ORIG_MAX);
    if (n <= 0) { printf("  FEHLER: keine Bilder in %s\n", pfad); return 1; }
    /* Kette im Original */
    int bruch = 0;
    for (int i = 1; i < n; i++)
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++)
            if (!glied_gleich(&b[i].vor[k], &b[i - 1].nach[k])) { bruch++; break; }
    PRUEF(bruch == 0, "Original: %d Bilder, 'vor' von Bild n+1 == 'nach' von Bild n in %d von %d Uebergaengen", n,
          n - 1 - bruch, n - 1);
    /* einzeln */
    int fehl_b = 0, gezeigt = 0;
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) re15_zopf_r38_glied_setzen(k, &b[i].vor[k]);
        re15_zopf_r38_schritt(&b[i].in);
        int f = orig_bild_pruefen(&b[i], i, gezeigt < 3);
        if (f) { fehl_b++; gezeigt++; }
    }
    PRUEF(fehl_b == 0, "einzeln: Port-Schritt == Original in %d von %d Bildern (Glieder 17..20 + Teil 8, bitgenau)",
          n - fehl_b, n);
    /* fortlaufend */
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) re15_zopf_r38_glied_setzen(k, &b[0].vor[k]);
    int fehl_f = 0, erst = -1;
    for (int i = 0; i < n; i++) {
        re15_zopf_r38_schritt(&b[i].in);
        int f = orig_bild_pruefen(&b[i], i, erst < 0 && fehl_f == 0 && 0);
        if (f) { fehl_f++; if (erst < 0) erst = i; }
    }
    PRUEF(fehl_f == 0, "fortlaufend ab Bild 0: Port-Zustand == Original in %d von %d Bildern (erste Abweichung %d)",
          n - fehl_f, n, erst);
    /* Umfang der Messung: Drehung und Ausschlag */
    int vmax = 0, dreh = 0;
    for (int i = 0; i < n; i++)
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            if (abs(b[i].nach[k].v94) > vmax) vmax = abs(b[i].nach[k].v94);
            if (abs(b[i].nach[k].v96) > vmax) vmax = abs(b[i].nach[k].v96);
        }
    for (int i = 1; i < n; i++) dreh += abs(((b[i].in.ent_rot[1] - b[i - 1].in.ent_rot[1]) % 4096 + 6144) % 4096 - 2048);
    printf("    Messumfang: Koerper dreht sich um %d, max |v94|/|v96| = %d\n", dreh, vmax);
    PRUEF(dreh >= 0x200 && vmax >= 8, "Messung enthaelt Drehung (%d >= 0x200) und Pendelausschlag (%d >= 8)", dreh, vmax);
    return g_fail ? 1 : 0;
}

/* konst <frames.txt> <schritt> (Werkzeug, Dossier §4.4): dieselbe fortlaufende Nachrechnung wie "orig", aber in der
 * Phase "gehen" (bis zur Wand: Originalschritt >= 50) bewegt sich der Koerper mit KONSTANTER Schrittweite <schritt> in
 * der mittleren Gehrichtung des Originals, statt mit der Original-Lage (die mit dem Gang 55..90 je Bild schwankt).
 * Alle anderen Eingaenge bleiben Original. Ausgabe: Abweichung xang/zang je Glied gegen die Original-RAM — misst, wie viel
 * der Abweichung exe gegen Original (vergleich_orig.py) allein die gleichmaessige Port-Schrittweite erklaert. */
static int teil_konst(const char *pfad, int schritt)
{
    printf("[konst] %s, Schritt %d je Bild\n", pfad, schritt);
    static orig_bild_t b[ORIG_MAX];
    int n = orig_lesen(pfad, b, ORIG_MAX);
    if (n <= 0) { printf("  FEHLER: keine Bilder in %s\n", pfad); return 1; }
    int g0 = -1, g1 = -1;   /* erstes Gehbild, letztes Gehbild vor der Wand */
    for (int i = 1; i < n; i++) {
        double dx = b[i].in.ent_pos[0] - b[i - 1].in.ent_pos[0], dz = b[i].in.ent_pos[2] - b[i - 1].in.ent_pos[2];
        double s = sqrt(dx * dx + dz * dz);
        if (!strcmp(b[i].phase, "gehen") && s >= 50) { if (g0 < 0) g0 = i; g1 = i; }
        else if (g0 >= 0 && s < 50) break;
    }
    if (g0 < 1) { printf("  FEHLER: keine Gehbilder\n"); return 1; }
    double ux = b[g1].in.ent_pos[0] - b[g0 - 1].in.ent_pos[0], uz = b[g1].in.ent_pos[2] - b[g0 - 1].in.ent_pos[2];
    double ul = sqrt(ux * ux + uz * uz); ux /= ul; uz /= ul;
    printf("    Gehbilder %d..%d, mittlere Original-Schrittweite %.1f, Richtung (%.3f,%.3f)\n", g0, g1, ul / (g1 - g0 + 1), ux, uz);
    for (int lauf = 0; lauf < 2; lauf++) {        /* 0 = Original-Lage (Gegenprobe), 1 = konstante Schrittweite */
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) re15_zopf_r38_glied_setzen(k, &b[0].vor[k]);
        double sum[4][2] = {{0}}; int mx[4][2] = {{0}}, nb = 0;
        for (int i = 0; i < n && i <= g1; i++) {
            re15_zopf_eingang_t in = b[i].in;
            if (lauf && i >= g0) {
                in.ent_pos[0] = b[g0 - 1].in.ent_pos[0] + (int32_t)lround(ux * schritt * (i - g0 + 1));
                in.ent_pos[2] = b[g0 - 1].in.ent_pos[2] + (int32_t)lround(uz * schritt * (i - g0 + 1));
            }
            re15_zopf_r38_schritt(&in);
            if (i < g0) continue;
            nb++;
            for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
                const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
                for (int a = 0; a < 2; a++) {
                    int ax = a ? 2 : 0;
                    int d = abs((((int)g->rot[ax] - (int)b[i].nach[k].rot[ax]) % 4096 + 6144) % 4096 - 2048);
                    sum[k][a] += d; if (d > mx[k][a]) mx[k][a] = d;
                }
            }
        }
        printf("    %s:\n", lauf ? "KONSTANTE Schrittweite" : "Original-Lage (Gegenprobe, muss 0 sein)");
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++)
            printf("      Glied %d xang Mittel %6.1f max %4d | zang Mittel %6.1f max %4d\n", RE15_ZOPF_ERSTES_TEIL + k,
                   sum[k][0] / nb, mx[k][0], sum[k][1] / nb, mx[k][1]);
        if (!lauf) {
            int null = 1;
            for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) if (mx[k][0] || mx[k][1]) null = 0;
            PRUEF(null, "Gegenprobe: mit der Original-Lage rechnet der Port die %d Gehbilder bitgleich", nb);
        }
    }
    return g_fail ? 1 : 0;
}

/* original (RIEGEL): der Datensatz r38_zopf_orig.inc (aus orig_zopf_frames.txt erzeugt, orig_zopf_inc.py) — Zustand der
 * Glieder 17..20 aus der Original-RAM vor Bild 0, danach rechnet der Port 235 Bilder FORTLAUFEND mit seinem eigenen
 * Zustand und den Original-Eingaengen; je Bild muss der FNV-1a ueber die 80 Werte (Glieder m/t/rot/v94/v96, Teil 8 +0x40)
 * dem Original entsprechen. Phasen: ruhe 15, drehung (R) 30, nachlauf 90, gehen (U) 40, stehen 60. */
#include "r38_zopf_orig.inc"

static uint32_t fnv_i32(uint32_t h, int32_t w)
{
    uint32_t u = (uint32_t)w;
    for (int i = 0; i < 4; i++) { h ^= (u >> (8 * i)) & 0xffu; h *= 0x01000193u; }
    return h;
}

static void teil_original(void)
{
    printf("[original] %d Bilder aus der Original-RAM (ROOM1121, Elza PL04)\n", R38_ORIG_BILDER);
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        re15_zopf_glied_t g;
        const int32_t *s = k_r38_orig_start[k];
        memset(&g, 0, sizeof g);
        for (int i = 0; i < 3; i++) g.off[i] = s[i];
        for (int i = 0; i < 9; i++) g.m[i] = (int16_t)s[3 + i];
        for (int i = 0; i < 3; i++) g.t[i] = s[12 + i];
        for (int i = 0; i < 3; i++) g.rot[i] = (int16_t)s[15 + i];
        g.v94 = (int16_t)s[18]; g.v96 = (int16_t)s[19];
        re15_zopf_r38_glied_setzen(k, &g);
    }
    int gleich[5] = {0}, gesamt[5] = {0}, erst = -1;
    const char *ph = "rdngs";
    for (int b = 0; b < R38_ORIG_BILDER; b++) {
        const int32_t *e = k_r38_orig_ein[b];
        re15_zopf_eingang_t in;
        memset(&in, 0, sizeof in);
        int p = 0;
        for (int i = 0; i < 3; i++) in.ent_rot[i] = (int16_t)e[p++];
        for (int i = 0; i < 3; i++) in.ent_pos[i] = e[p++];
        for (int i = 0; i < 9; i++) in.l0_rot[i] = e[p++];
        for (int i = 0; i < 3; i++) in.l0_t[i] = e[p++];
        for (int i = 0; i < 9; i++) in.l8_rot[i] = e[p++];
        for (int i = 0; i < 3; i++) in.l8_t[i] = e[p++];
        in.teil0_vy = (int16_t)e[p++]; in.teil0_vz = (int16_t)e[p++]; in.teil8_vy = (int16_t)e[p++];
        in.nacken_gier = (int16_t)e[p++]; in.nacken_flags = (uint8_t)e[p++];
        re15_zopf_r38_schritt(&in);
        uint32_t h = 0x811c9dc5u;
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
            for (int i = 0; i < 9; i++) h = fnv_i32(h, g->m[i]);
            for (int i = 0; i < 3; i++) h = fnv_i32(h, g->t[i]);
            for (int i = 0; i < 3; i++) h = fnv_i32(h, g->rot[i]);
            h = fnv_i32(h, g->v94); h = fnv_i32(h, g->v96);
        }
        int16_t m8[9]; int32_t t8[3];
        re15_zopf_r38_kopf_welt(m8, t8);
        for (int i = 0; i < 9; i++) h = fnv_i32(h, m8[i]);
        for (int i = 0; i < 3; i++) h = fnv_i32(h, t8[i]);
        const char *q = strchr(ph, k_r38_orig_phase[b]);
        int pi = q ? (int)(q - ph) : 0;
        gesamt[pi]++;
        if (h == k_r38_orig_hash[b]) gleich[pi]++;
        else if (erst < 0) erst = b;
    }
    int alle = 0;
    for (int i = 0; i < 5; i++) alle += gleich[i];
    printf("    ruhe %d/%d, drehung %d/%d, nachlauf %d/%d, gehen %d/%d, stehen %d/%d bitgleich\n", gleich[0], gesamt[0],
           gleich[1], gesamt[1], gleich[2], gesamt[2], gleich[3], gesamt[3], gleich[4], gesamt[4]);
    PRUEF(alle == R38_ORIG_BILDER, "Port-Zopf == ORIGINAL-RAM in %d von %d Bildern, fortlaufend (erste Abweichung Bild %d)",
          alle, R38_ORIG_BILDER, erst);
    PRUEF(gesamt[1] >= 20 && gesamt[3] >= 20, "Datensatz enthaelt Drehung (%d Bilder) und Gehen (%d Bilder)",
          gesamt[1], gesamt[3]);
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

/* ------------------------------------------------------------------------- Zopf-Log lesen (lauf / huelle) */
/* Eine Zeile "Z<n> s0=.. g0=(rx,ry,rz v94=.. v96=.. t=x,y,z) .. g3=(..) kopf=.. w8=[..] ein=(..) pos=x,y,z"
 * (zopf_r38.c, RE15_ZOPF_LOG), gelesen ab der letzten Raum-Marke "R kommando0". rot = (xang, s0, zang) wie
 * FUN_80024e40 Schritt 7 sie ablegt (@0x80025304/0c, rot.vy = s0 @0x80025328). */
typedef struct { int s0; int rot[4][3]; int v94[4], v96[4]; int t[4][3]; int pos[3]; int yaw, ng; } zl_schritt_t;
#define ZL_MAX 4096
static zl_schritt_t s_zl[ZL_MAX];

static int zl_lesen(const char *pfad)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("  FEHLER: %s fehlt\n", pfad); return -1; }
    static char z[1024];
    int n = 0;
    while (fgets(z, sizeof z, f)) {
        if (!strncmp(z, "R kommando0", 11)) { n = 0; continue; }
        zl_schritt_t s; int idx;
        if (sscanf(z, "Z%d s0=%d", &idx, &s.s0) != 2) continue;
        const char *p = z; int ok = 1;
        for (int k = 0; k < 4 && ok; k++) {
            char key[8]; snprintf(key, sizeof key, " g%d=(", k);
            p = strstr(p, key);
            if (!p || sscanf(p + 5, "%d,%d,%d v94=%d v96=%d t=%d,%d,%d", &s.rot[k][0], &s.rot[k][1], &s.rot[k][2],
                             &s.v94[k], &s.v96[k], &s.t[k][0], &s.t[k][1], &s.t[k][2]) != 8) ok = 0;
            else p += 5;
        }
        const char *q = strstr(z, " pos=");
        if (!ok || !q || sscanf(q + 5, "%d,%d,%d", &s.pos[0], &s.pos[1], &s.pos[2]) != 3) continue;
        int ex, ez, t0vy, t0vz, t8vy;
        const char *e = strstr(z, " ein=(ent ");
        if (!e || sscanf(e + 10, "%d,%d,%d t0vy %d t0vz %d t8vy %d ng %d", &ex, &s.yaw, &ez, &t0vy, &t0vz, &t8vy,
                         &s.ng) != 7) continue;
        if (n < ZL_MAX) s_zl[n++] = s;
    }
    fclose(f);
    return n;
}

/* huelle <zopf.log>: das Zopf-NETZ (PL04.MD1 Teile 17..20) je Schritt in Weltlage — Matrix wie FUN_80024e40 Schritt 7
 * (RotMatrixYXZ(xang,0,zang) @0x80025308, RotMatrixY(s0) @0x80025324), Lage t — gegen die Spielerlage pos: hoechster
 * Zopfpunkt ueber dem Boden (PSX -y = oben) und groesster waagerechter Abstand von der Figurachse. Vergleich mit dem
 * Figurkasten der Runde-37-Spur P (r37/masken36 tests/unit/test_r37_masken36.c FIGUR_HOCH 3000 / FIGUR_HALB 350, dort
 * an der exe an Leon gemessen: Hoehe 2999, quer 706). Werkzeug (Dossier §4.5): schreibt den Verlauf nach <log>.huelle,
 * Rueckgabe 0 = Datei lesbar; die Kastenpruefung steht als Zeile "KASTEN" in der Ausgabe. */
#define P_FIGUR_HOCH 3000
#define P_FIGUR_HALB 350
static int teil_huelle(const char *pfad)
{
    printf("[huelle] %s\n", pfad);
    int n = zl_lesen(pfad);
    if (n <= 0) { printf("  FEHLER: keine Schritte mit pos= in %s\n", pfad); return 1; }
    char p[512]; size_t nb = 0;
    snprintf(p, sizeof p, "%s/PLD/PL04.MD1", RE15_ASSET_PSX_DIR);
    uint8_t *b = slurp(p, &nb);
    static re15_md1_t md;
    if (!b || re15_md1_parse(b, (int)nb, &md) != 0 || md.mesh_count < RE15_ZOPF_ERSTES_TEIL + RE15_ZOPF_GLIEDER) {
        printf("  FEHLER: %s\n", p); free(b); return 1;
    }
    char vp[600]; snprintf(vp, sizeof vp, "%s.huelle", pfad);
    FILE *vo = fopen(vp, "w");
    int hmax = -(1 << 30), rmax = 0, hmax_i = -1, rmax_i = -1, ueber_r = 0, ueber_h = 0;
    for (int i = 0; i < n; i++) {
        const zl_schritt_t *s = &s_zl[i];
        int h_i = -(1 << 30), r_i = 0;
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            int16_t r[3] = { (int16_t)s->rot[k][0], 0, (int16_t)s->rot[k][2] }, m[9];
            re15_zopf_r38_rotmatrix_yxz(r, m);
            re15_zopf_r38_rotmatrix_y(s->rot[k][1], m);
            const re15_md1_mesh_t *ms = &md.meshes[RE15_ZOPF_ERSTES_TEIL + k];
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *vs = pass ? ms->quad_vertices : ms->tri_vertices;
                int cnt = pass ? ms->quad_vertex_count : ms->tri_vertex_count;
                for (int j = 0; vs && j < cnt; j++) {
                    int32_t w[3];
                    for (int a = 0; a < 3; a++)
                        w[a] = ((m[a*3+0] * vs[j].x + m[a*3+1] * vs[j].y + m[a*3+2] * vs[j].z) >> 12) + s->t[k][a];
                    int h = s->pos[1] - w[1];
                    double dx = w[0] - s->pos[0], dz = w[2] - s->pos[2];
                    int rr = (int)sqrt(dx * dx + dz * dz);
                    if (h > h_i) h_i = h;
                    if (rr > r_i) r_i = rr;
                }
            }
        }
        if (vo) fprintf(vo, "%d pos=%d,%d,%d hoch=%d quer=%d\n", i, s->pos[0], s->pos[1], s->pos[2], h_i, r_i);
        if (h_i > hmax) { hmax = h_i; hmax_i = i; }
        if (r_i > rmax) { rmax = r_i; rmax_i = i; }
        if (r_i > P_FIGUR_HALB) ueber_r++;
        if (h_i > P_FIGUR_HOCH) ueber_h++;
    }
    if (vo) fclose(vo);
    free(b);
    printf("    %d Schritte: hoechster Zopfpunkt %d ueber dem Boden (Schritt %d), weitester %d von der Figurachse (Schritt %d)\n",
           n, hmax, hmax_i, rmax, rmax_i);
    printf("    KASTEN Spur P (Hoehe %d, quer %d): %d Schritte hoeher, %d Schritte weiter als der Kasten -> %s\n",
           P_FIGUR_HOCH, P_FIGUR_HALB, ueber_h, ueber_r, (ueber_h || ueber_r) ? "RAGT HINAUS" : "innerhalb");
    return 0;
}

/* ------------------------------------------------------------------- lauf: Gehen/Rennen an der exe gegen das Original */
static void orig_start_setzen(void)
{
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        re15_zopf_glied_t g;
        const int32_t *s = k_r38_orig_start[k];
        memset(&g, 0, sizeof g);
        for (int i = 0; i < 3; i++) g.off[i] = s[i];
        for (int i = 0; i < 9; i++) g.m[i] = (int16_t)s[3 + i];
        for (int i = 0; i < 3; i++) g.t[i] = s[12 + i];
        for (int i = 0; i < 3; i++) g.rot[i] = (int16_t)s[15 + i];
        g.v94 = (int16_t)s[18]; g.v96 = (int16_t)s[19];
        re15_zopf_r38_glied_setzen(k, &g);
    }
}

static void orig_eingang(int b, re15_zopf_eingang_t *in)
{
    const int32_t *e = k_r38_orig_ein[b];
    memset(in, 0, sizeof *in);
    int p = 0;
    for (int i = 0; i < 3; i++) in->ent_rot[i] = (int16_t)e[p++];
    for (int i = 0; i < 3; i++) in->ent_pos[i] = e[p++];
    for (int i = 0; i < 9; i++) in->l0_rot[i] = e[p++];
    for (int i = 0; i < 3; i++) in->l0_t[i] = e[p++];
    for (int i = 0; i < 9; i++) in->l8_rot[i] = e[p++];
    for (int i = 0; i < 3; i++) in->l8_t[i] = e[p++];
    in->teil0_vy = (int16_t)e[p++]; in->teil0_vz = (int16_t)e[p++]; in->teil8_vy = (int16_t)e[p++];
    in->nacken_gier = (int16_t)e[p++]; in->nacken_flags = (uint8_t)e[p++];
}

static uint32_t zustand_hash(void)
{
    uint32_t h = 0x811c9dc5u;
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        const re15_zopf_glied_t *g = re15_zopf_r38_glied(k);
        for (int i = 0; i < 9; i++) h = fnv_i32(h, g->m[i]);
        for (int i = 0; i < 3; i++) h = fnv_i32(h, g->t[i]);
        for (int i = 0; i < 3; i++) h = fnv_i32(h, g->rot[i]);
        h = fnv_i32(h, g->v94); h = fnv_i32(h, g->v96);
    }
    int16_t m8[9]; int32_t t8[3];
    re15_zopf_r38_kopf_welt(m8, t8);
    for (int i = 0; i < 9; i++) h = fnv_i32(h, m8[i]);
    for (int i = 0; i < 3; i++) h = fnv_i32(h, t8[i]);
    return h;
}

static double orig_schritt(int b)   /* Lage-Schritt des Originals von Bild b-1 nach b (Spieler +0x34/+0x3c) */
{
    double dx = k_r38_orig_ein[b][3] - k_r38_orig_ein[b - 1][3], dz = k_r38_orig_ein[b][5] - k_r38_orig_ein[b - 1][5];
    return sqrt(dx * dx + dz * dz);
}

static double zl_schritt(int i)
{
    double dx = s_zl[i].pos[0] - s_zl[i - 1].pos[0], dz = s_zl[i].pos[2] - s_zl[i - 1].pos[2];
    return sqrt(dx * dx + dz * dz);
}

/* lauf <zopf.log> (INTEGRATION, Dossier §4.4): Elza an der exe mit der Eingabe der ORIGINAL-MESSUNG (mess_laufen.sh
 * lauf: ROOM1121 (150,9550) Blick 2048, stehen, R 29 Bilder, stehen, U 40 Bilder — wie orig_zopf_gdb.py), danach Kehrt,
 * Rennen X+U 14 Bilder, stehen.
 *  Gehen   ausgerichtet am Gehbeginn (Original Bild 135 = erste neue Lage), Fenster solange die Eingaenge gleich sind
 *          (Nacken-Gier Teil 8 +0x94 exe == Original, Schritt >= 50 in beiden: vor der Wand / vor der Port-Nackendrehung).
 *          Je Glied der groesste zang (Ausschlag nach hinten, FUN_80024e40 Schritt 4 @0x80025140) exe gegen Original.
 *          Schranke je Glied = die Abweichung, die die GLEICHMAESSIGE Port-Schrittweite allein erzeugt: das Original
 *          (r38_zopf_orig.inc, bitgleich nachgerechnet) noch einmal mit konstanter Schrittweite = der an der exe gemessenen
 *          (player_common.c WALK_SPEED_PER_FRAME 75 statt der Original-Lage, die mit dem Gang 55..90 je Bild schwankt).
 *  Rennen  groesster zang von Glied 17 beim Rennen (Schritt >= 150) > beim Gehen; nach dem Rennen schwingt Glied 17 durch
 *          die Ruhelage nach vorn (zang < Ruhe - 11 = 1 Grad) und schwingt aus (Abstand zur Ruhe in den Schritten 11..30
 *          nach dem Halt < halber Abstand in 1..10; Glied 17 hat den Daempfer p6 = 2 @0x80024d04-50, v94 halbiert sich
 *          je Bild). */
static int teil_lauf(const char *pfad)
{
    printf("[lauf] %s\n", pfad);
    int n = zl_lesen(pfad);
    if (n < 100) { printf("  FEHLER: nur %d Schritte mit pos= in %s\n", n, pfad); return 1; }
    /* Original: Gehbeginn og, erstes Bild an der Wand oe */
    int og = -1, oe = R38_ORIG_BILDER;
    for (int b = 1; b < R38_ORIG_BILDER; b++) {
        double s = orig_schritt(b);
        if (og < 0 && s > 0) og = b;
        else if (og >= 0 && s < 50) { oe = b; break; }
    }
    /* 1. Original-Referenz (fortlaufend, Hash je Bild) */
    static int16_t oz[R38_ORIG_BILDER][RE15_ZOPF_GLIEDER];
    re15_zopf_eingang_t in;
    orig_start_setzen();
    int bit = 0;
    for (int b = 0; b < R38_ORIG_BILDER; b++) {
        orig_eingang(b, &in);
        re15_zopf_r38_schritt(&in);
        if (zustand_hash() == k_r38_orig_hash[b]) bit++;
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) oz[b][k] = re15_zopf_r38_glied(k)->rot[2];
    }
    PRUEF(bit == R38_ORIG_BILDER, "Referenz: Original-RAM bitgleich nachgerechnet (%d/%d Bilder), Gehbeginn Bild %d, Wand Bild %d",
          bit, R38_ORIG_BILDER, og, oe);
    /* 2. exe: Gehbeginn pg, Fenster w gleicher Eingaenge */
    int pg = -1;
    for (int i = 1; i < n; i++) if (zl_schritt(i) > 0) { pg = i; break; }
    if (pg < 16 || og < 1) { printf("  FEHLER: kein Gehbeginn (exe %d, Original %d)\n", pg, og); return 1; }
    int w = 0; double ssum = 0;
    while (pg + w < n && og + w < oe) {
        double sp = zl_schritt(pg + w);
        if (sp < 50 || s_zl[pg + w].ng != k_r38_orig_ein[og + w][33]) break;
        ssum += sp; w++;
    }
    int S = w ? (int)lround(ssum / w) : 0;
    printf("    exe: %d Schritte, Gehbeginn Schritt %d, gleiche Eingaenge %d Bilder (Schrittweite exe %d, Original %.0f..%.0f)\n",
           n, pg, w, S, orig_schritt(og), orig_schritt(og + 3));
    PRUEF(w >= 30, "Gehfenster mit gleichen Eingaengen: %d Bilder (>= 30)", w);
    if (w < 1) return 1;
    /* 3. Schranke: Original mit konstanter Schrittweite S */
    double ux = k_r38_orig_ein[oe - 1][3] - k_r38_orig_ein[og - 1][3], uz = k_r38_orig_ein[oe - 1][5] - k_r38_orig_ein[og - 1][5];
    double ul = sqrt(ux * ux + uz * uz); ux /= ul; uz /= ul;
    int schranke[RE15_ZOPF_GLIEDER] = {0};
    orig_start_setzen();
    for (int b = 0; b < og + w; b++) {
        orig_eingang(b, &in);
        if (b >= og) {
            in.ent_pos[0] = k_r38_orig_ein[og - 1][3] + (int32_t)lround(ux * S * (b - og + 1));
            in.ent_pos[2] = k_r38_orig_ein[og - 1][5] + (int32_t)lround(uz * S * (b - og + 1));
        }
        re15_zopf_r38_schritt(&in);
        if (b < og) continue;
        for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
            int d = abs(re15_zopf_r38_glied(k)->rot[2] - oz[b][k]);
            if (d > schranke[k]) schranke[k] = d;
        }
    }
    /* 4. Ausschlag nach hinten beim Gehen: exe gegen Original */
    int zmax_geh0 = -100000;
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        int pe = -100000, po = -100000;
        for (int j = 0; j < w; j++) {
            if (s_zl[pg + j].rot[k][2] > pe) pe = s_zl[pg + j].rot[k][2];
            if (oz[og + j][k] > po) po = oz[og + j][k];
        }
        if (k == 0) zmax_geh0 = pe;
        PRUEF(abs(pe - po) <= schranke[k], "Gehen, Glied %d: groesster zang exe %d / Original %d (%.1f / %.1f Grad), "
              "Abweichung %d <= %d (konstante Schrittweite %d)", RE15_ZOPF_ERSTES_TEIL + k, pe, po, pe * 360.0 / 4096,
              po * 360.0 / 4096, abs(pe - po), schranke[k], S);
    }
    /* 5. Rennen: Block mit Schritt >= 150 nach dem Gehfenster */
    int rs = -1, re = -1;
    for (int i = pg + w; i < n; i++) {
        double sp = zl_schritt(i);
        if (sp >= 150) { if (rs < 0) rs = i; re = i; }
        else if (rs >= 0) break;
    }
    PRUEF(rs >= 0 && re - rs + 1 >= 5, "Rennen: %d Schritte mit Schritt >= 150 (ab Schritt %d)", rs >= 0 ? re - rs + 1 : 0, rs);
    if (rs < 0) return 1;
    int zmax_renn = -100000;
    for (int i = rs; i <= re; i++) if (s_zl[i].rot[0][2] > zmax_renn) zmax_renn = s_zl[i].rot[0][2];
    PRUEF(zmax_renn > zmax_geh0, "Rennen reisst den Zopf weiter nach hinten als Gehen: Glied 17 zang %d > %d (%.1f > %.1f Grad)",
          zmax_renn, zmax_geh0, zmax_renn * 360.0 / 4096, zmax_geh0 * 360.0 / 4096);
    double ruhe = 0;
    for (int i = pg - 15; i < pg; i++) ruhe += s_zl[i].rot[0][2];
    ruhe /= 15;
    int zmin = 100000, zmin_i = -1;
    for (int i = re + 1; i < n && i <= re + 40; i++) if (s_zl[i].rot[0][2] < zmin) { zmin = s_zl[i].rot[0][2]; zmin_i = i; }
    PRUEF(zmin < ruhe - 11, "nach dem Rennen schwingt Glied 17 durch die Ruhelage nach vorn: zang %d (Schritt %d) < Ruhe %.0f - 11",
          zmin, zmin_i, ruhe);
    /* Ausschwingen: groesster Abstand von der Ruhelage in den ersten 10 Schritten nach dem Halt gegen die 20 danach
     * (Daempfer v94 -= v94/p6 @0x800251ec-260). Spaeter (Nacken-Blick, Leerlauf-Animation der Port-Figur) wird der
     * Zopf neu angestossen — darum kein "Ruhe"-Fenster hier; das Einschwingen prueft der Riegel "spur" (Drehung). */
    double a1 = 0, a2 = 0;
    for (int i = re + 1; i < n && i <= re + 10; i++) if (fabs(s_zl[i].rot[0][2] - ruhe) > a1) a1 = fabs(s_zl[i].rot[0][2] - ruhe);
    for (int i = re + 11; i < n && i <= re + 30; i++) if (fabs(s_zl[i].rot[0][2] - ruhe) > a2) a2 = fabs(s_zl[i].rot[0][2] - ruhe);
    PRUEF(a1 > 0 && a2 < a1 * 0.5, "Glied 17 schwingt aus: Abstand zur Ruhelage %.0f (Schritte 1..10 nach dem Halt) -> %.0f "
          "(Schritte 11..30), weniger als die Haelfte", a1, a2);
    return g_fail ? 1 : 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Teil fehlt\n"); return 2; }
    if (!strcmp(argv[1], "lauf") && argc >= 3) return teil_lauf(argv[2]);
    const char *t = argv[1];
    if (!strcmp(t, "spur") && argc >= 3) return teil_spur(argv[2]);
    if (!strcmp(t, "huelle") && argc >= 3) return teil_huelle(argv[2]);
    if (!strcmp(t, "konst") && argc >= 4) return teil_konst(argv[2], atoi(argv[3]));
    if (!strcmp(t, "orig") && argc >= 3) return teil_orig(argv[2]);
    if (!strcmp(t, "pixel") && argc >= 8)
        return teil_pixel(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));
    if (!strcmp(t, "libgte")) teil_libgte();
    else if (!strcmp(t, "init")) teil_init();
    else if (!strcmp(t, "ruhe")) teil_ruhe();
    else if (!strcmp(t, "drehung")) teil_drehung();
    else if (!strcmp(t, "laufen")) teil_laufen();
    else if (!strcmp(t, "freeze")) teil_freeze();
    else if (!strcmp(t, "skelett")) teil_skelett();
    else if (!strcmp(t, "original")) teil_original();
    else { printf("unbekannter Teil %s\n", t); return 2; }
    printf("%s: %d Fehler\n", t, g_fail);
    return g_fail ? 1 : 0;
}
