/* align_probe.c — MISST die tatsaechliche Ausrichtung jedes Zeigers, den die
 * Asset-Parser aus rohen Byte-Puffern herausreichen (die -Wcast-align=strict-
 * Fundstellen im PC-Pfad). Faehrt den ECHTEN Code-Pfad (libre15_engine), kein
 * Nachbau, ueber den ECHTEN Asset-Baum re15_port/shared_assets/PSX.
 *
 * Aufruf:  align_probe [<asset-wurzel>]      (Vorgabe: shared_assets/PSX)
 * Exit 0 = jede gemessene Adresse erfuellt die Ausrichtung ihres Zieltyps.  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>

#include "re15_rdt.h"
#include "re15_tim.h"
#include "re15_md1.h"
#include "re15_camera.h"
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re2_ems.h"

enum {
    S_CUTS = 0, S_SCA, S_CLUT, S_PIX,
    S_TV, S_TN, S_TF, S_TU, S_QV, S_QN, S_QF, S_QU,
    S_MORPH0, S_MORPHSZ, S_NSITE
};
typedef struct { const char *name; int need; int worst; long n; char where[260]; } site_t;
static site_t S[S_NSITE] = {
    {"rdt_common.c:243  cuts          (re15_camera_cut_t*, int32 -> 4)", 4, 99, 0, ""},
    {"rdt_common.c:271  sca           (re15_sca_entry_t*,  u16   -> 2)", 2, 99, 0, ""},
    {"tim_common.c:60   clut          (uint16_t*)",                      2, 99, 0, ""},
    {"tim_common.c:84   pixels        (uint16_t*)",                      2, 99, 0, ""},
    {"md1_common.c:93   tri_vertices  (int16 -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:95   tri_normals   (int16 -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:97   triangles     (u16   -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:99   triangle_uvs  (u16   -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:111  quad_vertices (int16 -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:113  quad_normals  (int16 -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:115  quads         (u16   -> 2)",                     2, 99, 0, ""},
    {"md1_common.c:117  quad_uvs      (u16   -> 2)",                     2, 99, 0, ""},
    {"emd_morph.c:45/59 morph seg0    (int16 -> 2)",                     2, 99, 0, ""},
    {"emd_morph.c:59    morph seg_size PARITAET (gerade?)",              2, 99, 0, ""}
};

static int align_of(const void *p)
{
    uintptr_t v = (uintptr_t)p;
    if (v == 0) return 99;
    if (v & 1) return 1;
    if (v & 2) return 2;
    if (v & 4) return 4;
    return 8;
}
static void note(int i, const void *p, const char *file)
{
    int a;
    if (!p) return;
    a = align_of(p);
    S[i].n++;
    if (a < S[i].worst) { S[i].worst = a; snprintf(S[i].where, sizeof S[i].where, "%s", file); }
}
static void note_val(int i, unsigned long v, const char *file)
{
    int a = (v & 1u) ? 1 : ((v & 2u) ? 2 : ((v & 4u) ? 4 : 8));
    S[i].n++;
    if (a < S[i].worst) { S[i].worst = a; snprintf(S[i].where, sizeof S[i].where, "%s", file); }
}

static uint8_t *slurp(const char *path, size_t *out_n)
{
    FILE *f = fopen(path, "rb");
    long sz; uint8_t *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    b = (uint8_t *)malloc((size_t)sz);
    if (!b) { fclose(f); return NULL; }
    if (fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); *out_n = (size_t)sz; return b;
}

static long n_rdt = 0, n_tim = 0, n_md1 = 0, n_pld = 0, n_bank = 0, n_prop = 0, n_bad_malloc = 0;

static int icmp(const char *a, const char *b)
{
    while (*a && *b) {
        char ca = *a, cb = *b;
        if (ca >= 'a' && ca <= 'z') ca = (char)(ca - 32);
        if (cb >= 'a' && cb <= 'z') cb = (char)(cb - 32);
        if (ca != cb) return ca - cb;
        a++; b++;
    }
    return (int)((unsigned char)*a - (unsigned char)*b);
}
static int ends(const char *s, const char *suf)
{
    size_t a = strlen(s), b = strlen(suf);
    return a >= b && icmp(s + a - b, suf) == 0;
}

static void note_md1(const re15_md1_t *m, const char *path)
{
    int i;
    for (i = 0; i < m->mesh_count; i++) {
        note(S_TV, m->meshes[i].tri_vertices,  path);
        note(S_TN, m->meshes[i].tri_normals,   path);
        note(S_TF, m->meshes[i].triangles,     path);
        note(S_TU, m->meshes[i].triangle_uvs,  path);
        note(S_QV, m->meshes[i].quad_vertices, path);
        note(S_QN, m->meshes[i].quad_normals,  path);
        note(S_QF, m->meshes[i].quads,         path);
        note(S_QU, m->meshes[i].quad_uvs,      path);
    }
}

static void do_file(const char *path)
{
    size_t n = 0;
    uint8_t *d = slurp(path, &n);
    if (!d) return;
    if ((uintptr_t)d & 7) n_bad_malloc++;

    if (ends(path, ".RDT")) {
        re15_rdt_t r; memset(&r, 0, sizeof r);
        if (re15_rdt_parse(d, n, &r) == 0) {
            int p;
            n_rdt++;
            note(S_CUTS, r.cuts, path);
            note(S_SCA,  r.sca,  path);
            /* eingebettete Prop-MD1 (main.c:3117/3131 parst genau diese Zeiger) */
            for (p = 0; p < RE15_RDT_MAX_PROPS; p++) {
                re15_md1_t m; memset(&m, 0, sizeof m);
                if (!r.prop_md1[p] || r.prop_md1_size[p] <= 0) continue;
                if (re15_md1_parse(r.prop_md1[p], r.prop_md1_size[p], &m) == 0) {
                    n_prop++; note_md1(&m, path);
                }
            }
        }
    } else if (ends(path, ".TIM")) {
        re15_tim_t t; memset(&t, 0, sizeof t);
        if (re15_tim_parse(d, (int)n, &t) == 0) {
            n_tim++;
            note(S_CLUT, t.clut,   path);
            note(S_PIX,  t.pixels, path);
        }
    } else if (ends(path, ".MD1")) {
        re15_md1_t m; memset(&m, 0, sizeof m);
        if (re15_md1_parse(d, (int)n, &m) == 0) { n_md1++; note_md1(&m, path); }
    } else if (ends(path, ".PLD") || ends(path, ".PLW")) {
        unsigned long mo = 0, ml = 0, to = 0, tl = 0;
        if (re15_pld_part(d, (long)n, RE15_PLD_MD1, &mo, &ml)) {
            re15_md1_t m; memset(&m, 0, sizeof m);
            if (re15_md1_parse(d + mo, (int)ml, &m) == 0) { n_pld++; note_md1(&m, path); }
        }
        if (re15_pld_part(d, (long)n, RE15_PLD_TIM, &to, &tl)) {
            re15_tim_t t; memset(&t, 0, sizeof t);
            if (re15_tim_parse(d + to, (int)tl, &t) == 0) {
                note(S_CLUT, t.clut, path); note(S_PIX, t.pixels, path);
            }
        }
    } else if (ends(path, ".EMS")) {
        int kind;
        for (kind = RE2_EMS_KIND_MIN; kind < RE2_EMS_KIND_MIN + RE2_EMS_KIND_COUNT; kind++) {
            static re15_enemy_bank_t eb;     /* gross -> static */
            re15_tim_t t;
            int m;
            memset(&eb, 0, sizeof eb);
            memset(&t,  0, sizeof t);
            if (re2_ems_load_bank(d, n, kind, &eb, &t) != 0) continue;
            n_bank++;
            note_md1(&eb.md1, path);
            if (eb.md1_gore_ok) note_md1(&eb.md1_gore, path);
            note(S_CLUT, t.clut, path);
            note(S_PIX,  t.pixels, path);
            for (m = 0; m < MD1_MAX_MESHES; m++) {
                if (eb.morph[m].seg_count) {
                    note(S_MORPH0, eb.morph[m].seg0, path);
                    note_val(S_MORPHSZ, (unsigned long)eb.morph[m].seg_size, path);
                }
            }
        }
    }
    free(d);
}

static void walk(const char *dir)
{
    DIR *dp = opendir(dir);
    struct dirent *e;
    char path[1024];
    if (!dp) return;
    while ((e = readdir(dp)) != NULL) {
        struct stat st;
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
        snprintf(path, sizeof path, "%s/%s", dir, e->d_name);
        if (stat(path, &st) != 0) continue;
        if (st.st_mode & S_IFDIR) walk(path);
        else do_file(path);
    }
    closedir(dp);
}

int main(int argc, char **argv)
{
    const char *root = (argc > 1) ? argv[1] : "shared_assets/PSX";
    int i, fail = 0;
    printf("align_probe: Wurzel = %s\n", root);
    walk(root);
    printf("geparst: %ld RDT, %ld Prop-MD1 (in RDT), %ld TIM, %ld MD1, %ld PLD/PLW, %ld EMS-Baenke"
           "   (malloc-Bloecke nicht 8-aligned: %ld)\n",
           n_rdt, n_prop, n_tim, n_md1, n_pld, n_bank, n_bad_malloc);
    printf("%-64s %7s %9s %9s  %s\n", "Fundstelle", "noetig", "gemessen", "Treffer", "Urteil");
    for (i = 0; i < S_NSITE; i++) {
        const char *v;
        if (S[i].n == 0)                  v = "(nie erreicht)";
        else if (S[i].worst >= S[i].need) v = "OK";
        else                              { v = "UNAUSGERICHTET"; fail = 1; }
        printf("%-64s %7d %9d %9ld  %s %s\n", S[i].name, S[i].need,
               S[i].worst == 99 ? -1 : S[i].worst, S[i].n, v,
               (S[i].n && S[i].worst < S[i].need) ? S[i].where : "");
    }
    printf(fail ? "ERGEBNIS: MINDESTENS EINE ADRESSE IST UNAUSGERICHTET\n"
                : "ERGEBNIS: jede gemessene Adresse erfuellt die Ausrichtung ihres Zieltyps\n");
    return fail;
}
