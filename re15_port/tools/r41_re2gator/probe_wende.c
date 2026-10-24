/* probe_wende.c - Runde 41 Spur B, Nachbesserung 2 (Abnahme 1 M2, Orchestrator-Entscheid B7).
 * Rechnet die RE2-Bahnen des Alligators im ROOM2090-Becken GENAU wie gator_re2_r41.c:
 *   Intro = Clip 6 -> Clip 3 -> yaw += 1024 (@0x80100938-48), Dash = Clip 2 -> Clip 3 -> yaw += 1024 (@0x80100eb0-c0);
 *   je Takt Wurzelschritt off[f]-off[f-1] (FUN_80015e7c @0x80015fcc/fd4, Keyframe-Bytes 6..11) fuer f = 1..n-2,
 *   das letzte Bild ohne Schritt (FUN_8001a330 laesst FUN_800152c8 aus, `bne v0,zero,0x8001a3cc` @0x8001a3b0),
 *   Bild 0 = Neu-Anker; Pose des Bildes (re15_skel_compute_pose), Skala 2731 (GB_SCALE_Q12), Drehung wie
 *   re15_skel_bone_to_world / gr2_rig_welt.
 * Prueft die Bones gegen das Becken der Boss-KI (gb_*: Becken x[-8900,7200] z[-27000,-5400], Plattform x[-1700,1850]
 * z[-20000,-12450], Rampe x[1850,7200] z[-18100,-14500]) und sucht Startlagen/-gieren, bei denen KEIN Bone die Wand
 * durchdringt und die Wurzel ausserhalb der Klemmzone (1466) bleibt.
 * Aufruf: probe_wende <CDEMD0.EMS> [intro|dash] [x z yaw]   (ohne Lage: Suche + Leon-Tabelle)
 * Bau: gcc -std=c11 -O2 -DRE15_PLATFORM_PC=1 -I re15_port/include probe_wende.c -L build/engine -lre15_engine -lm */
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SKALA 2731
#define NMAX  400
static re15_enemy_bank_t *bk;
static int n_seq;
static int seq_clip[NMAX], seq_frame[NMAX], seq_knick[NMAX];
static int32_t seq_wurzel[NMAX][2];                     /* kumulierte Wurzel (Rig, skaliert): [0] laengs, [1] quer */
static int32_t seq_bone[NMAX][RE15_EMD_MAX_BONES][3];   /* Bone-Lage (Rig, skaliert) */
static int n_bones;

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static int kf_of(int clip, int frame)
{
    const re15_emd_animation_t *an = &bk->anim;
    const re15_emd_clip_t *c = &an->clips[clip];
    int fi = c->first_frame + frame, fend = c->first_frame + c->frame_count;
    while ((an->frames[fi] & 0x8000u) && fi + 1 < fend) fi++;
    return (int)(an->frames[fi] & 0xfffu);
}
static void off(int clip, int f, int32_t o[3])
{
    int16_t a[3];
    re15_emd_get_keyframe_speed(&bk->skel, kf_of(clip, f), &a[0], &a[1], &a[2]);
    o[0] = a[0]; o[1] = a[1]; o[2] = a[2];
}
static int32_t sk(int32_t v) { return (int32_t)(((int64_t)v * SKALA) >> 12); }

/* Folge wie gr2_clip_tick: Bild 0 ohne Schritt, f = 1..n-2 mit Schritt, n-1 ohne; Knick im letzten Bild der 2. Clip. */
static void folge_bauen(int clip_a, int clip_b)
{
    n_seq = 0;
    int32_t w[2] = { 0, 0 };
    int clips[2] = { clip_a, clip_b };
    for (int k = 0; k < 2; k++) {
        int c = clips[k], n = bk->anim.clips[c].frame_count;
        for (int f = 0; f < n && n_seq < NMAX; f++) {
            if (f >= 1 && f <= n - 2) {
                int32_t a[3], b[3]; off(c, f, b); off(c, f - 1, a);
                w[0] += sk(b[0] - a[0]); w[1] += sk(b[2] - a[2]);
            }
            seq_clip[n_seq] = c; seq_frame[n_seq] = f; seq_knick[n_seq] = (k == 1 && f == n - 1);
            seq_wurzel[n_seq][0] = w[0]; seq_wurzel[n_seq][1] = w[1];
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            re15_skel_compute_pose(&bk->skel, kf_of(c, f), poses);
            for (int b = 0; b < n_bones; b++)
                for (int i = 0; i < 3; i++) seq_bone[n_seq][b][i] = sk(poses[b].trans[i]);
            n_seq++;
        }
    }
}
/* Aussenabstand eines Punkts zum Becken (0 = im Wasser), Rand m nach innen. */
static int32_t draussen(int32_t x, int32_t z, int32_t m)
{
    int32_t d = 0;
    if (x < -8900 + m) d = (-8900 + m) - x;
    if (x > 7200 - m && x - (7200 - m) > d) d = x - (7200 - m);
    if (z < -27000 + m && (-27000 + m) - z > d) d = (-27000 + m) - z;
    if (z > -5400 - m && z - (-5400 - m) > d) d = z - (-5400 - m);
    /* Bloecke: Tiefe des Punkts im Block (kleinster Weg hinaus) */
    static const int32_t blk[2][4] = { { -1700, 1850, -20000, -12450 }, { 1850, 7200, -18100, -14500 } };
    for (int k = 0; k < 2; k++) {
        int32_t x0 = blk[k][0] - m, x1 = blk[k][1] + m, z0 = blk[k][2] - m, z1 = blk[k][3] + m;
        if (x > x0 && x < x1 && z > z0 && z < z1) {
            int32_t t = x - x0;
            if (x1 - x < t) t = x1 - x;
            if (z - z0 < t) t = z - z0;
            if (z1 - z < t) t = z1 - z;
            if (t > d) d = t;
        }
    }
    return d;
}
static void welt(int32_t sx, int32_t sz, int yaw, int32_t lx, int32_t lz, int32_t *wx, int32_t *wz)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    *wx = sx + (int32_t)(((int64_t)c * lx + (int64_t)s * lz) >> 12);
    *wz = sz + (int32_t)((-(int64_t)s * lx + (int64_t)c * lz) >> 12);
}
typedef struct { int32_t kopf, leib, schwanz, wurzel, end_x, end_z; int end_yaw; int32_t kopf_min_x, kopf_max_x; } ergebnis_t;
/* Kopf/Leib = Rumpf 0/1 + Hals 5 + Kopf 6 + Kiefer 7 (Kette 1->5->6, Memory gator-bosskampf); Schwanz 11..15 getrennt */
static const int k_leib[] = { 0, 1, 5, 6, 7, 11, 12, 13, 14, 15 };
/* Leon fuer die Bahn-Pruefung der Auswahlregel: kein Rumpf-/Kopf-Bone naeher als 1350 (SEG[0].r 900 + 0x1c2) */
static int g_leon_an; static int32_t g_leon_x, g_leon_z;
static void bahn(int32_t sx, int32_t sz, int yaw0, int schritt, ergebnis_t *r, FILE *zeilen)
{
    memset(r, 0, sizeof *r); r->kopf_min_x = 0x7fffffff; r->kopf_max_x = -0x7fffffff;
    for (int t = 0; t < n_seq; t += schritt) {
        int yaw = seq_knick[t] ? ((yaw0 + 1024) & 0xfff) : yaw0;
        int32_t rx, rz; welt(sx, sz, yaw0, seq_wurzel[t][0], seq_wurzel[t][1], &rx, &rz);
        int32_t d = draussen(rx, rz, 1466); if (d > r->wurzel) r->wurzel = d;
        if (g_leon_an) {   /* Koerperlaengen-Push-Kreise des Boss-Moduls (k_gb_seg: ofs/r + Spielerradius 450) */
            static const int32_t seg[5][2] = { { 2600, 900 }, { 4600, 700 }, { -2600, 900 }, { -4800, 750 }, { 0, 1050 } };
            for (int si = 0; si < 5; si++) {
                int32_t cx, cz; welt(rx, rz, yaw, seg[si][0], 0, &cx, &cz);
                if (hypot((double)(cx - g_leon_x), (double)(cz - g_leon_z)) < seg[si][1] + 450 && r->leib == 0) r->leib = 1;
            }
        }
        for (unsigned i = 0; i < sizeof k_leib / sizeof *k_leib; i++) {
            int b = k_leib[i]; if (b >= n_bones) continue;
            int32_t bx, bz; welt(rx, rz, yaw, seq_bone[t][b][0], seq_bone[t][b][2], &bx, &bz);
            int32_t db = draussen(bx, bz, 0);
            if (b == 6) {
                if (db > r->kopf) r->kopf = db;
                if (bx < r->kopf_min_x) r->kopf_min_x = bx;
                if (bx > r->kopf_max_x) r->kopf_max_x = bx;
                if (zeilen)
                    fprintf(zeilen, "  t=%3d clip %d bild %3d wurzel=(%6d,%6d) kopf=(%6d,%6d) draussen=%d\n", t, seq_clip[t],
                            seq_frame[t], rx, rz, bx, bz, db);
            }
            if (b >= 11) { if (db > r->schwanz) r->schwanz = db; }
            else if (db > r->leib) r->leib = db;
            if (g_leon_an && b < 11 && hypot((double)(bx - g_leon_x), (double)(bz - g_leon_z)) < 1350 && r->leib == 0) r->leib = 1;
        }
        if (t + schritt >= n_seq && t != n_seq - 1) t = n_seq - 1 - schritt;   /* letztes Bild immer */
    }
    welt(sx, sz, yaw0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &r->end_x, &r->end_z);
    r->end_yaw = (yaw0 + 1024) & 0xfff;
}
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
static int peilung(int32_t fx, int32_t fz, int32_t tx, int32_t tz)
{
    /* Engine-Peilung wie die Boss-KI (`re15_atan2_q12(tz - e->z, tx - e->x) - 0x400`, re15_actor.h): Ost 0, Sued
     * (-z) 1024, West 2048, Nord (+z) 3072 - dieselbe Konvention wie die Gier (welt(): yaw 1024 = -z). */
    return ((int)re15_atan2_q12(tz - fz, tx - fx) - 0x400) & 0xfff;
}
static int wdiff(int a, int b) { int d = ((a - b + 0x800) & 0xfff) - 0x800; return d < 0 ? -d : d; }

typedef struct { int32_t x, z; int yaw; ergebnis_t r; } kand_t;

/* Auswahlregel (= gator_re2_r41.c gr2_intro_lage, Nachbesserung 2): Bahnende auf dem Strahl von Leon mit Toleranz
 * delta der End-Gier gegen die Peilung zu Leon (Stufen 128/256/512), Kopf am Ende >= 1350 von Leon (Schnauzenkreis
 * SEG[0] 900 + Spielerradius 0x1c2 450 des Boss-Moduls), Rumpf/Kopf im Wasser, Wurzel ausserhalb der Klemmzone;
 * Kosten = Startabstand zur aktuellen Lage + 4 x Gier-Drehung + 16 x |delta|. Rueckgabe Stufe (0..2) oder -1. */
static int wahl_rechnen(int32_t lx, int32_t lz, int32_t gx, int32_t gz, int gyaw, int32_t kopfmin,
                        kand_t *aus, int *delta_aus, int32_t *kopf_aus)
{
    static const int tols[3] = { 128, 256, 512 };
    g_leon_an = 1; g_leon_x = lx; g_leon_z = lz;
    for (int st = 0; st < 3; st++) {
        long best = -1;
        for (int y0 = 0; y0 < 4096; y0 += 32) {
            int ye = (y0 + 1024) & 0xfff;
            int32_t ex, ez; welt(0, 0, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &ex, &ez);
            for (int dl = -tols[st]; dl <= tols[st]; dl += 32) {
                int32_t ux, uz; welt(0, 0, (ye + dl) & 0xfff, 4096, 0, &ux, &uz);
                for (int32_t d = 1000; d <= 24000; d += 250) {
                    int32_t sx = lx - (int32_t)(((int64_t)ux * d) >> 12) - ex;
                    int32_t sz = lz - (int32_t)(((int64_t)uz * d) >> 12) - ez;
                    if (draussen(sx, sz, 1466)) continue;
                    int32_t rx, rz, hx, hz;
                    welt(sx, sz, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &rx, &rz);
                    welt(rx, rz, ye, seq_bone[n_seq-1][6][0], seq_bone[n_seq-1][6][2], &hx, &hz);
                    double dk = hypot((double)(hx - lx), (double)(hz - lz));
                    if (dk < kopfmin) continue;
                    /* KEINE Biss-Sperre am Ende: RE2 liest +0x238 (Intro-Sperre 110) nur in der Paddel-Hysterese
                     * (@0x80100b5c, @0x80100bf4, @0x80100cb4) - die Biss-Maschine T1 (FUN_801012fc) prueft es nicht. */
                    long k = (long)hypot((double)(sx - gx), (double)(sz - gz)) + 4L * wdiff(y0, gyaw) + 16L * (dl < 0 ? -dl : dl);
                    if (best >= 0 && k >= best) continue;
                    ergebnis_t r; bahn(sx, sz, y0, 3, &r, NULL);
                    if (r.leib || r.wurzel) continue;
                    best = k; aus->x = sx; aus->z = sz; aus->yaw = y0; aus->r = r; *delta_aus = dl; *kopf_aus = (int32_t)dk;
                }
            }
        }
        if (best >= 0) { g_leon_an = 0; return st; }
    }
    g_leon_an = 0;
    return -1;
}
static kand_t s_ok[300000];

int main(int argc, char **argv)
{
    size_t n = 0;
    if (argc < 2) { printf("probe_wende <CDEMD0.EMS> [intro|dash] [x z yaw]\n"); return 1; }
    uint8_t *ems = slurp(argv[1], &n);
    if (!ems) { printf("EMS fehlt\n"); return 1; }
    re15_enemy_reset();
    bk = re15_enemy_alloc(0x23u);
    if (!bk || re2_ems_load_bank(ems, n, 0x23, bk, NULL) != 0) { printf("Bank\n"); return 1; }
    bk->ok = 1; n_bones = bk->skel.bone_count;
    const char *art = argc > 2 ? argv[2] : "intro";
    int dash = !strcmp(art, "dash");
    folge_bauen(dash ? 2 : 6, 3);
    printf("%s: %d Takte (Clip %d %d + Clip 3 %d), Bones %d, Wurzel-Ende rig (laengs %d, quer %d)\n", art, n_seq,
           dash ? 2 : 6, bk->anim.clips[dash ? 2 : 6].frame_count, bk->anim.clips[3].frame_count, n_bones,
           seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1]);
    if (argc >= 9 && !strcmp(argv[3], "gier")) {
        /* gier <x> <z> <yaw_jetzt> <leon_x> <leon_z>: alle Startgieren an fester Lage (Dash beginnt, wo der Zonenwechsel
         * passiert) - zulaessige Gieren, die naechste zur aktuellen Gier und die zu Leon am besten ausgerichtete */
        int32_t x = atoi(argv[4]), z = atoi(argv[5]); int jetzt = atoi(argv[6]) & 0xfff;
        int32_t lx = atoi(argv[7]), lz = atoi(argv[8]);
        int nah = -1, nah_d = 9999, aus = -1, aus_d = 9999, nzul = 0;
        ergebnis_t r0; bahn(x, z, jetzt, 1, &r0, NULL);
        printf("jetzt yaw %d: Kopf draussen %d, Rumpf/Kopf %d, Wurzel %d, Ende (%d,%d) yaw %d\n", jetzt, r0.kopf, r0.leib, r0.wurzel,
               r0.end_x, r0.end_z, r0.end_yaw);
        for (int y = 0; y < 4096; y += 16) {
            ergebnis_t r; bahn(x, z, y, 1, &r, NULL);
            if (r.leib || r.wurzel) continue;
            nzul++;
            int dj = wdiff(y, jetzt), da = wdiff(r.end_yaw, peilung(r.end_x, r.end_z, lx, lz));
            if (dj < nah_d) { nah_d = dj; nah = y; }
            if (da < aus_d) { aus_d = da; aus = y; }
        }
        printf("zulaessige Startgieren: %d von 256\n", nzul);
        if (nah >= 0) { ergebnis_t r; bahn(x, z, nah, 1, &r, NULL);
            printf("  naechste zur Gier %d: yaw %d (Drehung %d) -> Ende (%d,%d) yaw %d, Peilung zu Leon %d\n", jetzt, nah, nah_d,
                   r.end_x, r.end_z, r.end_yaw, peilung(r.end_x, r.end_z, lx, lz)); }
        if (aus >= 0) { ergebnis_t r; bahn(x, z, aus, 1, &r, NULL);
            printf("  beste Ausrichtung zu Leon: yaw %d (Drehung %d) -> Ende (%d,%d) yaw %d, Peilung %d (Abw. %d)\n", aus,
                   wdiff(aus, jetzt), r.end_x, r.end_z, r.end_yaw, peilung(r.end_x, r.end_z, lx, lz), aus_d); }
        return 0;
    }
    if (argc >= 6 && !strcmp(argv[3], "regel")) {
        /* regel <lx> <lz>: die Auswahlregel (wahl_rechnen) fuer EINE Leon-Lage, Gator am Lauerplatz */
        int32_t lx = atoi(argv[4]), lz = atoi(argv[5]);
        kand_t k; int dl = 0; int32_t dk = 0;
        int st = wahl_rechnen(lx, lz, -7200, -9200, 1089, 1350, &k, &dl, &dk);
        if (st < 0) { printf("Leon (%6d,%6d): keine Lage\n", lx, lz); return 0; }
        printf("Leon (%6d,%6d): Stufe %d Start (%6d,%6d) yaw %4d (Abstand %5.0f, Drehung %4d) -> Ende (%6d,%6d) yaw %4d, Abw. %4d, Kopf-Leon %5d, Wurzel-Leon %5.0f, Schwanz %d\n",
               lx, lz, st, k.x, k.z, k.yaw, hypot((double)(k.x + 7200), (double)(k.z + 9200)), wdiff(k.yaw, 1089),
               k.r.end_x, k.r.end_z, k.r.end_yaw, dl, dk, hypot((double)(k.r.end_x - lx), (double)(k.r.end_z - lz)), k.r.schwanz);
        return 0;
    }
    if (argc >= 7 && !strcmp(argv[3], "nah")) {
        /* nah <lx> <lz> <R>: beste Lage (Ende ausserhalb des Biss-Commits, Rumpf/Kopf im Wasser) mit Start hoechstens R vom
         * Lauerplatz, je Ausrichtungs-Toleranz - zeigt den Preis der Naehe zum Lauerplatz. */
        int32_t lx = atoi(argv[4]), lz = atoi(argv[5]), R = atoi(argv[6]);
        static const int tols[5] = { 128, 256, 512, 1024, 2048 };
        for (int ti = 0; ti < 5; ti++) {
            long best = -1; kand_t bk; int bdl = 0; int32_t bkopf = 0; memset(&bk, 0, sizeof bk);
            for (int y0 = 0; y0 < 4096; y0 += 32) {
                int ye = (y0 + 1024) & 0xfff;
                int32_t ex, ez; welt(0, 0, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &ex, &ez);
                for (int dl = -tols[ti]; dl <= tols[ti]; dl += 32) {
                    int32_t ux, uz; welt(0, 0, (ye + dl) & 0xfff, 4096, 0, &ux, &uz);
                    for (int32_t d = 1000; d <= 24000; d += 250) {
                        int32_t sx = lx - (int32_t)(((int64_t)ux * d) >> 12) - ex;
                        int32_t sz = lz - (int32_t)(((int64_t)uz * d) >> 12) - ez;
                        if (draussen(sx, sz, 1466)) continue;
                        if (hypot((double)(sx + 7200), (double)(sz + 9200)) > R) continue;
                        int32_t rx, rz, hx, hz, mx, mz;
                        welt(sx, sz, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &rx, &rz);
                        welt(rx, rz, ye, seq_bone[n_seq-1][6][0], seq_bone[n_seq-1][6][2], &hx, &hz);
                        welt(rx, rz, ye, 2600, 0, &mx, &mz);
                        double dk = hypot((double)(hx - lx), (double)(hz - lz));
                        if (dk < 1350 || hypot((double)(rx - lx), (double)(rz - lz)) < 3200
                            || hypot((double)(mx - lx), (double)(mz - lz)) < 2400) continue;
                        long k = (long)hypot((double)(sx + 7200), (double)(sz + 9200)) + 4L * wdiff(y0, 1089) + 16L * (dl < 0 ? -dl : dl);
                        if (best >= 0 && k >= best) continue;
                        ergebnis_t r; bahn(sx, sz, y0, 3, &r, NULL);
                        if (r.leib || r.wurzel) continue;
                        best = k; bk.x = sx; bk.z = sz; bk.yaw = y0; bk.r = r; bdl = dl; bkopf = (int32_t)dk;
                    }
                }
            }
            if (best < 0) { printf("  tol %4d: keine Lage mit Start <= %d vom Lauerplatz\n", tols[ti], R); continue; }
            printf("  tol %4d: Start (%6d,%6d) yaw %4d (Abstand %5.0f) -> Ende (%6d,%6d) yaw %4d, Abw. %5d, Kopf-Leon %5d, Wurzel-Leon %5.0f\n",
                   tols[ti], bk.x, bk.z, bk.yaw, hypot((double)(bk.x + 7200), (double)(bk.z + 9200)), bk.r.end_x, bk.r.end_z,
                   bk.r.end_yaw, bdl, bkopf, hypot((double)(bk.r.end_x - lx), (double)(bk.r.end_z - lz)));
        }
        return 0;
    }
    if (argc >= 4 && !strcmp(argv[3], "kreis")) {
        /* Leon auf dem Aggro-Kreis (GB_AGGRO 9000 um den Lauerplatz (-7200,-9200), Boss-Modul LURK `dist < GB_AGGRO`)
         * je 15 Grad; begehbar = Becken (Wasser, Leon watet y=0) oder Plattform/Rampe (Leon oben). */
        for (int w = 0; w < 360; w += 15) {
            double a = w * M_PI / 180.0;
            int32_t lx = -7200 + (int32_t)lround(9000 * cos(a)), lz = -9200 + (int32_t)lround(9000 * sin(a));
            if (lx < -8900 || lx > 7200 || lz < -27000 || lz > -5400) continue;
            kand_t k; int dl = 0; int32_t dk = 0;
            int st = wahl_rechnen(lx, lz, -7200, -9200, 1089, 1350, &k, &dl, &dk);
            if (st < 0) { printf("Leon (%6d,%6d): keine Lage\n", lx, lz); continue; }
            printf("Leon (%6d,%6d): Stufe %d Start (%6d,%6d) yaw %4d (Abstand %5.0f, Drehung %4d) -> Ende (%6d,%6d) yaw %4d, Abw. %4d, Kopf-Leon %5d, Schwanz %d\n",
                   lx, lz, st, k.x, k.z, k.yaw, hypot((double)(k.x + 7200), (double)(k.z + 9200)), wdiff(k.yaw, 1089),
                   k.r.end_x, k.r.end_z, k.r.end_yaw, dl, dk, k.r.schwanz);
        }
        return 0;
    }
    if (argc >= 6 && !strcmp(argv[3], "wahl")) {
        /* wahl <lx> <lz> [gx gz gyaw]: Auswahlregel wie gr2_intro_lage (Nachbesserung 2): Bahnende auf dem Strahl von Leon
         * mit Ausrichtungs-Toleranz delta (End-Gier gegen Peilung zu Leon), Kopf am Ende >= 2400 von Leon (Biss-Commit
         * maul < 2400 des Boss-Moduls), Rumpf/Kopf im Wasser, Wurzel ausserhalb der Klemmzone; Kosten = Startabstand zur
         * aktuellen Lage + 8 x Gier-Drehung (Q12) + 16 x |delta|. */
        int32_t lx = atoi(argv[4]), lz = atoi(argv[5]);
        int32_t gx = -7200, gz = -9200; int gyaw = 1089;
        int32_t kopfmin = argc >= 7 ? atoi(argv[6]) : 2400; int tol = argc >= 8 ? atoi(argv[7]) : 96;
        long best = -1; int by = 0, bdl = 0; int32_t bx = 0, bz = 0, bk = 0; ergebnis_t br; memset(&br, 0, sizeof br);
        int nz = 0;
        for (int y0 = 0; y0 < 4096; y0 += 32) {
            int ye = (y0 + 1024) & 0xfff;
            int32_t ex, ez; welt(0, 0, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &ex, &ez);
            for (int dl = -tol; dl <= tol; dl += 32) {
                int32_t ux, uz; welt(0, 0, (ye + dl) & 0xfff, 4096, 0, &ux, &uz);
                for (int32_t d = 1000; d <= 24000; d += 250) {
                    int32_t sx = lx - (int32_t)(((int64_t)ux * d) >> 12) - ex;
                    int32_t sz = lz - (int32_t)(((int64_t)uz * d) >> 12) - ez;
                    if (draussen(sx, sz, 1466)) continue;
                    int32_t rx, rz, hx, hz;
                    welt(sx, sz, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &rx, &rz);
                    welt(rx, rz, ye, seq_bone[n_seq-1][6][0], seq_bone[n_seq-1][6][2], &hx, &hz);
                    double dk = hypot((double)(hx - lx), (double)(hz - lz));
                    if (dk < kopfmin) continue;
                    ergebnis_t r; bahn(sx, sz, y0, 3, &r, NULL);
                    if (r.leib || r.wurzel) continue;
                    nz++;
                    long k = (long)hypot((double)(sx - gx), (double)(sz - gz)) + 8L * wdiff(y0, gyaw) + 16L * (dl < 0 ? -dl : dl);
                    if (best < 0 || k < best) { best = k; by = y0; bx = sx; bz = sz; bk = (int32_t)dk; br = r; bdl = dl; }
                }
            }
        }
        if (best < 0) { printf("  keine zulaessige Lage\n"); return 0; }
        printf("  %d zulaessig; Wahl Start (%6d,%6d) yaw %4d (Abstand %5.0f, Drehung %d) -> Ende (%6d,%6d) yaw %4d, Abw. %d, Kopf-Leon %d, Schwanz draussen %d\n",
               nz, bx, bz, by, hypot((double)(bx - gx), (double)(bz - gz)), wdiff(by, gyaw), br.end_x, br.end_z, br.end_yaw,
               bdl, bk, br.schwanz);
        return 0;
    }
    if (argc >= 6 && !strcmp(argv[3], "leon")) {
        /* leon <lx> <lz> [gx gz]: Intro-Startlagen, deren Knick GENAU auf Leon zeigt (Bau wie gator_re2_r41.c
         * gr2_intro_lage: je Startgier y0 und Abstand d liegt das Bahnende auf dem Strahl von Leon gegen die End-Gier
         * y0+1024, S = Leon - d*dir(y0+1024) - R(y0)*w_ende); je Mindestabstand Kopf<->Leon die zulaessige Lage mit dem
         * kleinsten Abstand zur aktuellen Lage des Gators (Lauerplatz). */
        int32_t lx = atoi(argv[4]), lz = atoi(argv[5]);
        int32_t gx = argc >= 8 ? atoi(argv[6]) : -7200, gz = argc >= 8 ? atoi(argv[7]) : -9200;
        static const int32_t kmin[] = { 0, 2500, 4000, 6000, 9000 };
        for (unsigned k = 0; k < sizeof kmin / sizeof *kmin; k++) {
            int by = -1; int32_t bx = 0, bz = 0, bd = 0; double bl = 1e18; ergebnis_t br; memset(&br, 0, sizeof br);
            int nz = 0;
            for (int y0 = 0; y0 < 4096; y0 += 32) {
                int ye = (y0 + 1024) & 0xfff;
                int32_t ex, ez; welt(0, 0, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &ex, &ez);  /* R(y0)*w_ende */
                int32_t ux, uz; welt(0, 0, ye, 4096, 0, &ux, &uz);                                     /* dir(ye) Q12 */
                for (int32_t d = 1000; d <= 24000; d += 250) {
                    int32_t sx = lx - (int32_t)(((int64_t)ux * d) >> 12) - ex;
                    int32_t sz = lz - (int32_t)(((int64_t)uz * d) >> 12) - ez;
                    if (draussen(sx, sz, 1466)) continue;
                    ergebnis_t r; bahn(sx, sz, y0, 3, &r, NULL);
                    if (r.leib || r.wurzel) continue;
                    /* Kopf am Ende: letzter Takt, Gier ye */
                    int32_t rx, rz, hx, hz;
                    welt(sx, sz, y0, seq_wurzel[n_seq-1][0], seq_wurzel[n_seq-1][1], &rx, &rz);
                    welt(rx, rz, ye, seq_bone[n_seq-1][6][0], seq_bone[n_seq-1][6][2], &hx, &hz);
                    double dk = hypot((double)(hx - lx), (double)(hz - lz));
                    if (dk < kmin[k]) continue;
                    nz++;
                    double dl = hypot((double)(sx - gx), (double)(sz - gz));
                    if (dl < bl) { bl = dl; by = y0; bx = sx; bz = sz; bd = (int32_t)dk; br = r; }
                }
            }
            if (by < 0) { printf("  Kopf>=%5d: keine zulaessige Lage\n", kmin[k]); continue; }
            printf("  Kopf>=%5d: %5d zulaessig, naechste Start (%6d,%6d) yaw %4d (Abstand %5.0f) -> Ende (%6d,%6d) yaw %4d, Kopf-Leon %d, Schwanz draussen %d\n",
                   kmin[k], nz, bx, bz, by, bl, br.end_x, br.end_z, br.end_yaw, bd, br.schwanz);
        }
        return 0;
    }
    if (argc >= 6) {
        ergebnis_t r;
        bahn(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]) & 0xfff, 1, &r, stdout);
        printf("Lage (%s,%s) yaw %s: Kopf draussen max %d, Rumpf/Kopf max %d, Schwanz max %d, Wurzel-Klemme %d, Kopf x %d..%d, Ende (%d,%d) yaw %d\n",
               argv[3], argv[4], argv[5], r.kopf, r.leib, r.schwanz, r.wurzel, r.kopf_min_x, r.kopf_max_x, r.end_x, r.end_z, r.end_yaw);
        return 0;
    }
    /* Suche: alle Startlagen (Raster 200) und Gieren (Schritt 32), Bahn jedes 3. Bild + letztes */
    int n_ok = 0, n_ges = 0;
    for (int32_t z = -27000; z <= -5400; z += 200)
        for (int32_t x = -8900; x <= 7200; x += 200) {
            if (draussen(x, z, 1466)) continue;
            for (int yaw = 0; yaw < 4096; yaw += 32) {
                ergebnis_t r; n_ges++;
                bahn(x, z, yaw, 3, &r, NULL);
                if (r.leib == 0 && r.wurzel == 0 && n_ok < (int)(sizeof s_ok / sizeof *s_ok)) {
                    s_ok[n_ok].x = x; s_ok[n_ok].z = z; s_ok[n_ok].yaw = yaw; s_ok[n_ok].r = r; n_ok++;
                }
            }
        }
    printf("Suche: %d Kandidaten, %d ohne Wanddurchdringung (Leib-Bones 0/1/5/6/7/11-15 im Wasser, Wurzel ausserhalb der Klemmzone)\n",
           n_ges, n_ok);
    int hist[8] = { 0 };
    for (int i = 0; i < n_ok; i++) hist[((s_ok[i].r.end_yaw + 256) & 0xfff) / 512]++;
    printf("End-Gier der zulaessigen Bahnen (Oktanten: Ost 0, Sued 1024, West 2048, Nord 3072): O %d SO %d S %d SW %d W %d NW %d N %d NO %d\n",
           hist[0], hist[1], hist[2], hist[3], hist[4], hist[5], hist[6], hist[7]);
    static const struct { const char *name; int32_t x, z; } leon[] = {
        { "Tuernische (GB_TEST=1, Lauf A)", -8200, -25550 }, { "Westkanal Sued (Lauf N, Aggro)", -8440, -18171 },
        { "Nordbecken Mitte", -2000, -9000 }, { "Suedbecken Ost (Lauf C2)", 2500, -25000 }, { "Rampe", 4500, -16300 } };
    for (unsigned li = 0; li < sizeof leon / sizeof *leon; li++) {
        int best = -1; long bestk = 0x7fffffff;
        for (int i = 0; i < n_ok; i++) {
            int p = peilung(s_ok[i].r.end_x, s_ok[i].r.end_z, leon[li].x, leon[li].z);
            int dw = wdiff(s_ok[i].r.end_yaw, p);
            long dl = (long)hypot((double)(s_ok[i].x + 7200), (double)(s_ok[i].z + 9200));
            long k = (long)dw * 20 + dl / 100;
            if (k < bestk) { bestk = k; best = i; }
        }
        if (best < 0) { printf("Leon %-34s: keine zulaessige Bahn\n", leon[li].name); continue; }
        int p = peilung(s_ok[best].r.end_x, s_ok[best].r.end_z, leon[li].x, leon[li].z);
        printf("Leon %-34s (%6d,%6d): Start (%6d,%6d) yaw %4d -> Ende (%6d,%6d) yaw %4d, Peilung %4d (Abw. %4d), Lauerplatz-Abstand %.0f\n",
               leon[li].name, leon[li].x, leon[li].z, s_ok[best].x, s_ok[best].z, s_ok[best].yaw, s_ok[best].r.end_x,
               s_ok[best].r.end_z, s_ok[best].r.end_yaw, p, wdiff(s_ok[best].r.end_yaw, p),
               hypot((double)(s_ok[best].x + 7200), (double)(s_ok[best].z + 9200)));
    }
    return 0;
}
