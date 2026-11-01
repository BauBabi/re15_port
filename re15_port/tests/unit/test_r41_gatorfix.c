/*
 * test_r41_gatorfix.c - Runde 41 Spur G2 "gatorfix". Dossier analysis/befunde_runde41/G2_gatorfix.md.
 *
 * Nutzt den Harness von test_r41_gatorszene.c (echte Engine-Wege: Raumaufbau, SCD-VM, Boss-Takt, RE2-EM23-Bank); die Datei wird
 * eingebunden, ihr main heisst hier gsz_haupt.
 *
 * Werkzeug (kein Riegel):
 *   maulgeo [elza]        Geometrie der Leiche im Maul je Bild (Leichen-Scheitel im Rahmen von Part 6 / Part 7)
 */
#define main gsz_haupt
#include "test_r41_gatorszene.c"
#undef main

#include "re15_md1.h"
#include "re15_anim_select.h"

/* ---- Geometrie-Helfer ------------------------------------------------------------------------------------------------------- */
static inline int32_t g2_q12(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 12); }
/* Pose, wie der NPC-Zeichner sie in diesem Bild zeichnet (Form gator_re2_r41.c gr2_pose_gezeichnet; Schnappschuss zurueck). */
static int g2_pose(re15_actor_t *e, re15_skel_pose_t poses[RE15_EMD_MAX_BONES])
{
    re15_enemy_bank_t *bk = re15_enemy_find(e->type);
    if (!bk || !bk->ok || bk->skel.bone_count <= 7) return 0;
    int ov = (e->anim_flags & 0x04u) ? (int)e->motion : -1;
    int kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, ov, (uint32_t)e->anim_frame);
    int32_t root[3]; int16_t ang[32][3]; uint8_t gueltig = e->anim_prev_valid;
    memcpy(root, e->prev_root, sizeof root); memcpy(ang, e->prev_angles, sizeof ang);
    void *save = g_anim_pose_actor; g_anim_pose_actor = e;
    int rv = re15_skel_compute_pose(&bk->skel, kf, poses);
    g_anim_pose_actor = save;
    memcpy(e->prev_root, root, sizeof root); memcpy(e->prev_angles, ang, sizeof ang); e->anim_prev_valid = gueltig;
    return rv == 0;
}
/* Weltmatrix eines Parts (Form gr2_part_welt): rot = RotY(gier) x Part, pos = Wurzel + RotY x Skala x Part. */
static void g2_part_welt(const re15_actor_t *e, const re15_skel_pose_t *p, int32_t rot[9], int32_t pos[3])
{
    const int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    int32_t tr[3] = { g2_q12(p->trans[0], s), g2_q12(p->trans[1], s), g2_q12(p->trans[2], s) };
    re15_skel_bone_to_world(tr, e->rot_y, e->x, e->y, e->z, pos);
    int32_t ry[9]; re15_skel_euler_matrix(0, (int)e->rot_y, 0, ry);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            rot[r*3 + c] = (int32_t)(((int64_t)ry[r*3+0] * p->rot[0*3+c] + (int64_t)ry[r*3+1] * p->rot[1*3+c]
                                    + (int64_t)ry[r*3+2] * p->rot[2*3+c]) >> 12);
}
/* Prop-Drehmatrix wie main.c pc_prop_rot_q12 (Ry*Rx*Rz) */
static void g2_prop_matrix(int16_t rx, int16_t ry, int16_t rz, int32_t m[9])
{
    int32_t sx = re15_sin_q12(rx), cx = re15_cos_q12(rx), sy = re15_sin_q12(ry), cy = re15_cos_q12(ry), sz = re15_sin_q12(rz), cz = re15_cos_q12(rz);
    m[0] = g2_q12(cz, cy); m[1] = -g2_q12(sz, cy); m[2] = sy;
    m[3] = (int32_t)(((((int64_t)sz * cx) << 12) + (int64_t)cz * sy * sx) >> 24);
    m[4] = (int32_t)(((((int64_t)cz * cx) << 12) - (int64_t)sz * sy * sx) >> 24);
    m[5] = -g2_q12(cy, sx);
    m[6] = (int32_t)(((((int64_t)sz * sx) << 12) - (int64_t)cz * sy * cx) >> 24);
    m[7] = (int32_t)(((((int64_t)cz * sx) << 12) + (int64_t)sz * sy * cx) >> 24);
    m[8] = g2_q12(cy, cx);
}
/* Welt -> Lokal (R^T (w - p)) */
static void g2_lokal(const int32_t r[9], const int32_t p[3], const int32_t w[3], int32_t l[3])
{
    const int32_t d[3] = { w[0] - p[0], w[1] - p[1], w[2] - p[2] };
    for (int i = 0; i < 3; i++) l[i] = (int32_t)(((int64_t)r[0*3+i] * d[0] + (int64_t)r[1*3+i] * d[1] + (int64_t)r[2*3+i] * d[2]) >> 12);
}
/* Alle Scheitel eines MD1 (Tri + Quad) */
static int g2_scheitel(const re15_md1_t *m, int mesh, int32_t (*out)[3], int cap)
{
    int n = 0;
    for (int k = 0; k < m->mesh_count; k++) {
        if (mesh >= 0 && k != mesh) continue;
        const re15_md1_mesh_t *ms = &m->meshes[k];
        for (int i = 0; i < ms->tri_vertex_count && n < cap; i++) { out[n][0] = ms->tri_vertices[i].x; out[n][1] = ms->tri_vertices[i].y; out[n][2] = ms->tri_vertices[i].z; n++; }
        for (int i = 0; i < ms->quad_vertex_count && n < cap; i++) { out[n][0] = ms->quad_vertices[i].x; out[n][1] = ms->quad_vertices[i].y; out[n][2] = ms->quad_vertices[i].z; n++; }
    }
    return n;
}
static void g2_bbox(int32_t (*v)[3], int n, int32_t lo[3], int32_t hi[3])
{
    for (int i = 0; i < 3; i++) { lo[i] = 0x7fffffff; hi[i] = -0x7fffffff; }
    for (int k = 0; k < n; k++) for (int i = 0; i < 3; i++) { if (v[k][i] < lo[i]) lo[i] = v[k][i]; if (v[k][i] > hi[i]) hi[i] = v[k][i]; }
}
static int32_t s_lv[4096][3];   /* Leichen-Scheitel (Modellraum) */
static int     s_ln = 0;
static int leiche_md1(void)
{
    static re15_md1_t m;
    const int obj = RE15_GSZ_LEICHE_OBJ;
    if (obj >= s_rdt.prop_count || !s_rdt.prop_md1[obj]) return 0;
    if (re15_md1_parse(s_rdt.prop_md1[obj], s_rdt.prop_md1_size[obj], &m) != 0) return 0;
    s_ln = g2_scheitel(&m, -1, s_lv, 4096);
    return s_ln;
}

/* ---- Werkzeug maulgeo ------------------------------------------------------------------------------------------------------- */
static void maulgeo(int elza)
{
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_game_state_init();
    re15_inv_load_briefing();
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    if (room_boot(r2080, -9950, -14400, -25700, 2048, 2) != 0) return;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    const uint8_t cut = g_room_change.target_cut;
    if (room_boot(r2090, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, cut) != 0) return;
    if (!gator_bank()) return;
    if (!leiche_md1()) { printf("Leichen-MD1 fehlt\n"); return; }
    int32_t lo[3], hi[3];
    g2_bbox(s_lv, s_ln, lo, hi);
    { FILE *vf = fopen("leiche_scheitel.txt", "w"); if (vf) { for (int i = 0; i < s_ln; i++) fprintf(vf, "%d %d %d\n", s_lv[i][0], s_lv[i][1], s_lv[i][2]); fclose(vf); } }
    printf("Leiche obj %d: %d Scheitel, Kasten x %d..%d y %d..%d z %d..%d\n", RE15_GSZ_LEICHE_OBJ, s_ln, lo[0], hi[0], lo[1], hi[1], lo[2], hi[2]);
    re15_enemy_bank_t *bk = re15_enemy_find(0x23u);
    static int32_t gv[4096][3];
    for (int b = 4; b <= 7 && b < bk->md1.mesh_count; b++) {
        int n = g2_scheitel(&bk->md1, b, gv, 4096);
        { char fn[64]; snprintf(fn, sizeof fn, "gator_part%d.txt", b); FILE *vf = fopen(fn, "w"); if (vf) { for (int i = 0; i < n; i++) fprintf(vf, "%d %d %d\n", gv[i][0], gv[i][1], gv[i][2]); fclose(vf); } }
        g2_bbox(gv, n, lo, hi);
        printf("Gator Part %d: %d Scheitel, Kasten x %d..%d y %d..%d z %d..%d\n", b, n, lo[0], hi[0], lo[1], hi[1], lo[2], hi[2]);
    }
    re15_actor_t *g = gator_setzen();
    for (int f = 0; f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
        if (k < 0 || !g || !(g->motion == 4 || g->motion == 5 || g->motion == 11 || g->motion == 0 || g->motion == 10)) continue;
        re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
        if (!g2_pose(g, ps)) continue;
        int32_t r6[9], p6[3], r7[9], p7[3], ro[9];
        g2_part_welt(g, &ps[6], r6, p6); g2_part_welt(g, &ps[7], r7, p7);
        const __typeof__(g_scd.props[0]) *pr = &g_scd.props[k];
        g2_prop_matrix(pr->rot_x, pr->rot_y, pr->rot_z, ro);
        int32_t l6lo[3] = { 0x7fffffff, 0x7fffffff, 0x7fffffff }, l6hi[3] = { -0x7fffffff, -0x7fffffff, -0x7fffffff };
        for (int i = 0; i < s_ln; i++) {
            const int32_t w[3] = { pr->x + (int32_t)(((int64_t)ro[0] * s_lv[i][0] + (int64_t)ro[1] * s_lv[i][1] + (int64_t)ro[2] * s_lv[i][2]) >> 12),
                                   pr->y + (int32_t)(((int64_t)ro[3] * s_lv[i][0] + (int64_t)ro[4] * s_lv[i][1] + (int64_t)ro[5] * s_lv[i][2]) >> 12),
                                   pr->z + (int32_t)(((int64_t)ro[6] * s_lv[i][0] + (int64_t)ro[7] * s_lv[i][1] + (int64_t)ro[8] * s_lv[i][2]) >> 12) };
            int32_t l[3]; g2_lokal(r6, p6, w, l);
            for (int a = 0; a < 3; a++) { if (l[a] < l6lo[a]) l6lo[a] = l[a]; if (l[a] > l6hi[a]) l6hi[a] = l[a]; }
        }
        int32_t p7l[3]; g2_lokal(r6, p6, p7, p7l);
        {   /* Werkzeug: Szenenbild je gewaehltes Bild (Umgebung G2_DUMP=<bild>): Leiche + Part 6 + Part 7 + Weltlage im Part-6-Rahmen */
            static int dump_f = -2; if (dump_f == -2) { const char *d = getenv("G2_DUMP"); dump_f = d ? atoi(d) : -1; }
            if (dump_f == f || (dump_f == 9999 && (g->motion == 5 || g->motion == 11 || (g->motion == 4 && g->anim_frame >= 13)))) {
                char dn[64]; snprintf(dn, sizeof dn, "dump_rahmen6_%d.txt", f); FILE *vf = fopen(dump_f == 9999 ? dn : "dump_rahmen6.txt", "w");
                re15_enemy_bank_t *gb = re15_enemy_find(0x23u); static int32_t pv[4096][3];
                const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
                for (int b = 5; b <= 7 && vf; b++) {
                    int32_t rb[9], pb[3]; g2_part_welt(g, &ps[b], rb, pb);
                    int n = g2_scheitel(&gb->md1, b, pv, 4096);
                    for (int i = 0; i < n; i++) {
                        const int32_t v[3] = { g2_q12(pv[i][0], sk), g2_q12(pv[i][1], sk), g2_q12(pv[i][2], sk) };
                        const int32_t w[3] = { pb[0] + (int32_t)(((int64_t)rb[0] * v[0] + (int64_t)rb[1] * v[1] + (int64_t)rb[2] * v[2]) >> 12),
                                               pb[1] + (int32_t)(((int64_t)rb[3] * v[0] + (int64_t)rb[4] * v[1] + (int64_t)rb[5] * v[2]) >> 12),
                                               pb[2] + (int32_t)(((int64_t)rb[6] * v[0] + (int64_t)rb[7] * v[1] + (int64_t)rb[8] * v[2]) >> 12) };
                        int32_t l[3]; g2_lokal(r6, p6, w, l);
                        fprintf(vf, "%d %d %d %d %d %d %d\n", b, l[0], l[1], l[2], w[0], w[1], w[2]);
                    }
                }
                for (int i = 0; i < s_ln && vf; i++) {
                    const int32_t w[3] = { pr->x + (int32_t)(((int64_t)ro[0] * s_lv[i][0] + (int64_t)ro[1] * s_lv[i][1] + (int64_t)ro[2] * s_lv[i][2]) >> 12),
                                           pr->y + (int32_t)(((int64_t)ro[3] * s_lv[i][0] + (int64_t)ro[4] * s_lv[i][1] + (int64_t)ro[5] * s_lv[i][2]) >> 12),
                                           pr->z + (int32_t)(((int64_t)ro[6] * s_lv[i][0] + (int64_t)ro[7] * s_lv[i][1] + (int64_t)ro[8] * s_lv[i][2]) >> 12) };
                    int32_t l[3]; g2_lokal(r6, p6, w, l);
                    fprintf(vf, "%d %d %d %d %d %d %d\n", 99, l[0], l[1], l[2], w[0], w[1], w[2]);
                }
                if (vf) fclose(vf);
            }
        }
        printf("F%d clip %d bild %d aktiv %d: Leiche im Part-6-Rahmen x %d..%d y %d..%d z %d..%d | Part7-Ursprung (%d,%d,%d) | r7 vs r6 y-Achse %d\n",
               f, (int)g->motion, (int)g->anim_frame, pr->active, l6lo[0], l6hi[0], l6lo[1], l6hi[1], l6lo[2], l6hi[2], p7l[0], p7l[1], p7l[2],
               (int)(((int64_t)r6[1] * r7[1] + (int64_t)r6[4] * r7[4] + (int64_t)r6[7] * r7[7]) >> 12));
        {   /* Kopfhaltung: Maul-Mitte (Part 6 (600,-350)) Welt-y, Schnauzen-Neigung (Weltrichtung der Part-6-x-Achse, y-Anteil Q12: < 0 = Schnauze hoch), Maul-oben (Part-6-(-y)-Achse) */
            const int32_t mm[3] = { p6[0] + g2_q12(r6[0], 600) + g2_q12(r6[1], -350), p6[1] + g2_q12(r6[3], 600) + g2_q12(r6[4], -350),
                                    p6[2] + g2_q12(r6[6], 600) + g2_q12(r6[7], -350) };
            printf("    KOPF F%d clip %d bild %d: Maul (%d,%d,%d) Schnauze-y %d Maul-oben-y %d Wurzel (%d,%d)\n", f, (int)g->motion, (int)g->anim_frame,
                   mm[0], mm[1], mm[2], r6[3], -r6[4], g->x, g->z);
        }
    }
}

/* ---- Kopf-Band: die Kopf-Scheitel (Unterkiefer Part 6 + Schaedel Part 7) im Part-6-Rahmen; fuer einen Leichen-Scheitel gilt der oberste und unterste
 *      Kopf-Scheitel im Laengsfenster +-150 um ihn (vor der Schnauze / hinter dem Hals: das Fenster am Kopf-Ende) - dasselbe Mass wie die Lagewahl
 *      (Dossier G2 §1, suche5.py). ---- */
#define G2_FENSTER 150
typedef struct { int n; int32_t x[1024], y[1024]; int32_t x0, x1; } g2_band_t;
static void g2_band(re15_actor_t *g, const re15_skel_pose_t *ps, const int32_t r6[9], const int32_t p6[3], g2_band_t *b)
{
    static int32_t pv[4096][3];
    re15_enemy_bank_t *gb = re15_enemy_find(0x23u);
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    b->n = 0; b->x0 = 0x7fffffff; b->x1 = -0x7fffffff;
    for (int part = 6; part <= 7; part++) {
        int32_t rb[9], pb[3]; g2_part_welt(g, &ps[part], rb, pb);
        const int n = g2_scheitel(&gb->md1, part, pv, 4096);
        for (int i = 0; i < n && b->n < 1024; i++) {
            const int32_t v[3] = { g2_q12(pv[i][0], sk), g2_q12(pv[i][1], sk), g2_q12(pv[i][2], sk) };
            const int32_t w[3] = { pb[0] + (int32_t)(((int64_t)rb[0] * v[0] + (int64_t)rb[1] * v[1] + (int64_t)rb[2] * v[2]) >> 12),
                                   pb[1] + (int32_t)(((int64_t)rb[3] * v[0] + (int64_t)rb[4] * v[1] + (int64_t)rb[5] * v[2]) >> 12),
                                   pb[2] + (int32_t)(((int64_t)rb[6] * v[0] + (int64_t)rb[7] * v[1] + (int64_t)rb[8] * v[2]) >> 12) };
            int32_t l[3]; g2_lokal(r6, p6, w, l);
            b->x[b->n] = l[0]; b->y[b->n] = l[1]; b->n++;
            if (l[0] < b->x0) b->x0 = l[0];
            if (l[0] > b->x1) b->x1 = l[0];
        }
    }
}
static void g2_band_bei(const g2_band_t *b, int32_t x, int32_t *oben, int32_t *unten)
{
    if (x < b->x0 + G2_FENSTER) x = b->x0 + G2_FENSTER;
    if (x > b->x1 - G2_FENSTER) x = b->x1 - G2_FENSTER;
    *oben = 0x7fffffff; *unten = -0x7fffffff;
    for (int i = 0; i < b->n; i++) {
        if (b->x[i] < x - G2_FENSTER || b->x[i] > x + G2_FENSTER) continue;
        if (b->y[i] < *oben) *oben = b->y[i];
        if (b->y[i] > *unten) *unten = b->y[i];
    }
}
/* Leichen-Scheitel ausserhalb des Kopf-Bandes (ueber dem Schaedel / unter dem Unterkiefer) und kleinster Abstand zum Rand */
static int g2_draussen(const int32_t r6[9], const int32_t p6[3], const g2_band_t *b, int *unten, int32_t *marge)
{
    const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
    if (k < 0) return -1;
    const __typeof__(g_scd.props[0]) *pr = &g_scd.props[k];
    int32_t ro[9]; g2_prop_matrix(pr->rot_x, pr->rot_y, pr->rot_z, ro);
    int n = 0; if (unten) *unten = 0; if (marge) *marge = 0x7fffffff;
    for (int i = 0; i < s_ln; i++) {
        const int32_t w[3] = { pr->x + (int32_t)(((int64_t)ro[0] * s_lv[i][0] + (int64_t)ro[1] * s_lv[i][1] + (int64_t)ro[2] * s_lv[i][2]) >> 12),
                               pr->y + (int32_t)(((int64_t)ro[3] * s_lv[i][0] + (int64_t)ro[4] * s_lv[i][1] + (int64_t)ro[5] * s_lv[i][2]) >> 12),
                               pr->z + (int32_t)(((int64_t)ro[6] * s_lv[i][0] + (int64_t)ro[7] * s_lv[i][1] + (int64_t)ro[8] * s_lv[i][2]) >> 12) };
        int32_t l[3]; g2_lokal(r6, p6, w, l);
        int32_t ob, un; g2_band_bei(b, l[0], &ob, &un);
        const int32_t m = (l[1] - ob) < (un - l[1]) ? (l[1] - ob) : (un - l[1]);
        if (marge && m < *marge) *marge = m;
        if (l[1] < ob || l[1] > un) { n++; if (unten && l[1] > un) (*unten)++; if (getenv("G2_ZEIGE")) printf("        Scheitel %d Modell (%d,%d,%d) -> Part6 (%d,%d,%d) Band [%d,%d]\n", i, s_lv[i][0], s_lv[i][1], s_lv[i][2], l[0], l[1], l[2], ob, un); }
    }
    return n;
}
/* Szene 2090 -> 2080 -> 2090 bis zum Gator-Auftritt (Form teil_leiche_p der Spur P) */
static re15_actor_t *g2_szene_start(int elza)
{
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_game_state_init();
    re15_inv_load_briefing();
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return NULL;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    if (room_boot(r2080, -9950, -14400, -25700, 2048, 2) != 0) return NULL;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    const uint8_t cut = g_room_change.target_cut;
    if (room_boot(r2090, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, cut) != 0) return NULL;
    if (!gator_bank() || !leiche_md1()) return NULL;
    return gator_setzen();
}

/* ---- Riegel Punkt 1: Leiche im Maul, weder ueber dem Schaedel noch unter dem Unterkiefer ------------------------------------- */
static void teil_maul(int elza, int alt, int *aus_max, int *aus_bilder)
{
    re15_gatorszene_r41_g2_alt_setzen(alt);
    re15_actor_t *g = g2_szene_start(elza);
    int gemessen = 0, max_n = -1, max_u = 0, max_f = -1, bilder_draussen = 0, f_kopp = -1, ein_max = 0, ein_bilder = 0; int32_t marge_min = 0x7fffffff;
    for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        int32_t a = -1;
        if (re15_gatorszene_r41_leiche_sitz(&a) && f_kopp < 0) f_kopp = f;            /* Koppelbild: ab hier dreht sie GSZ_SITZ_BILDER = 26 Bilder ins Maul */
        if (!re15_gatorszene_r41_leiche_sitz(&a) || a > 40) continue;      /* angehaengt, nicht im Flug, angekommen (Spur-P-Mass 40) */
        re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
        if (!g2_pose(g, ps)) continue;
        int32_t r6[9], p6[3]; g2_part_welt(g, &ps[6], r6, p6);
        g2_band_t b; g2_band(g, ps, r6, p6, &b);
        int unten = 0; int32_t marge = 0;
        const int n = g2_draussen(r6, p6, &b, &unten, &marge);
        if (n < 0) continue;
        if (f_kopp >= 0 && f < f_kopp + 26) { ein_bilder++; if (n > ein_max) ein_max = n; continue; }   /* Einschwenken (26 Bilder ab dem Koppelbild): nur berichtet */
        gemessen++;
        if (n > 0) { bilder_draussen++; if (getenv("G2_ZEIGE")) printf("      F%d Clip %d Bild %d: %d draussen (unten %d), Rand %d\n", f, (int)g->motion, (int)g->anim_frame, n, unten, (int)marge); }
        if (n > max_n) { max_n = n; max_u = unten; max_f = f; }
        if (marge < marge_min) marge_min = marge;
    }
    re15_gatorszene_r41_g2_alt_setzen(0);
    printf("    %s%s: %d Bilder im Maul gemessen, Leichen-Scheitel ausserhalb des Kopf-Bandes hoechstens %d (davon unter dem Unterkiefer %d, Bild %d), "
           "in %d Bildern; kleinster Randabstand %d (Haltebilder ab Koppelbild F%d + 26; Einschwenken: %d Bilder, hoechstens %d Scheitel ausserhalb)\n",
           elza ? "Elza" : "Leon", alt ? " [Gegenprobe Leiche wie vor G2]" : "", gemessen, max_n, max_u, max_f, bilder_draussen, (int)marge_min, f_kopp, ein_bilder, ein_max);
    if (aus_max) *aus_max = gemessen > 0 ? max_n : -1;
    if (aus_bilder) *aus_bilder = gemessen;
}
static void teil_maul_riegel(int elza)
{
    printf("== maul%s (Punkt 1) ==\n", elza ? "_elza" : "");
    int n = -1, b = 0, n_alt = -1, b_alt = 0;
    teil_maul(elza, 0, &n, &b);
    teil_maul(elza, 1, &n_alt, &b_alt);
    PRUEF(b >= 34 && n == 0, "Leiche im Maul: %d Haltebilder (Clip 5 Bild 25..56 + 75..76 = 34), kein Scheitel ueber dem Schaedel oder unter dem Unterkiefer (%d)", b, n);
    PRUEF(b_alt >= 40 && n_alt >= 50, "Gegenprobe Leiche wie vor G2: %d Scheitel ausserhalb (rot ohne den Bau)", n_alt);
}

/* ---- Riegel Punkt 2: hochwerfen, im Maul fangen, dann weg -------------------------------------------------------------------- */
static void teil_wurf(int elza)
{
    printf("== wurf%s (Punkt 2) ==\n", elza ? "_elza" : "");
    for (int alt = 0; alt <= 1; alt++) {
        re15_gatorszene_r41_g2_alt_setzen(alt);
        re15_actor_t *g = g2_szene_start(elza);
        int32_t y_vor = 0; int steigt = 0, faellt = 0, sicht_nach_weg = 0, key_nach_weg = 0, weg_f = -1;
        for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
            frame(0, 0);
            int bw = -1, bf = -1, bweg = -1; int32_t h = 0, fa = -1;
            const int ph = re15_gatorszene_r41_leiche_flug(&bw, &bf, &bweg, &h, &fa);
            const int k = prop_von(RE15_GSZ_LEICHE_OBJ), kk = prop_von(RE15_GSZ_KEY_OBJ);
            if (ph == 1 && k >= 0) {
                if (g_scd.props[k].y < y_vor) steigt++; else if (g_scd.props[k].y > y_vor) faellt++;
                y_vor = g_scd.props[k].y;
            } else if (k >= 0) y_vor = g_scd.props[k].y;
            if (ph == 3 && weg_f < 0) weg_f = f;
            if (weg_f >= 0) { if (k >= 0 && g_scd.props[k].active) sicht_nach_weg++; if (kk >= 0 && g_scd.props[kk].active) key_nach_weg++; }
        }
        int bw = -1, bf = -1, bweg = -1; int32_t h = 0, fa = -1;
        const int ph = re15_gatorszene_r41_leiche_flug(&bw, &bf, &bweg, &h, &fa);
        re15_gatorszene_r41_g2_alt_setzen(0);
        printf("    %s%s: Zustand %d, Wurf Clip-5-Bild %d, Fang %d, weg %d; hoechster Punkt der Rumpfmitte %d ueber der Maullage; im Fangbild %d vom Maul; "
               "Flugbilder steigend %d / fallend %d; nach dem Verschlingen sichtbar: Leiche %d, Schluessel %d Bilder\n", elza ? "Elza" : "Leon",
               alt ? " [Gegenprobe Leiche wie vor G2]" : "", ph, bw, bf, bweg, (int)h, (int)fa, steigt, faellt, sicht_nach_weg, key_nach_weg);
        if (!alt) {
            PRUEF(ph == 3 && bw == 57 && bf == 75 && bweg == 77, "Wurf beim Hochreissen (Clip 5 Bild %d), Fang beim Zuschnappen (%d), weg mit dem geschlossenen Maul (%d)", bw, bf, bweg);
            PRUEF(h >= 405 && steigt >= 3 && faellt >= 3, "hochgeworfen: Rumpfmitte %d ueber der Maullage (>= H/2 = a T^2 / 16 = 405), %d Bilder steigend, %d fallend", (int)h, steigt, faellt);
            PRUEF(fa >= 0 && fa <= 40, "im Maul gefangen: Rumpfmitte %d von der Maullage", (int)fa);
            PRUEF(weg_f >= 0 && sicht_nach_weg == 0 && key_nach_weg == 0, "danach verschwunden (Leiche %d, Schluessel %d Bilder sichtbar)", sicht_nach_weg, key_nach_weg);
        } else {
            PRUEF(ph == 0, "Gegenprobe Leiche wie vor G2: kein Wurf (Zustand %d) - rot ohne den Bau", ph);
        }
    }
}

/* ---- Werkzeug wurfkamera: je Zonenkamera (Cut 0..7) Anteil der Leichen-Scheitel im Bild ueber die Flugbilder (Wurf .. Fang) und Kopf (Bone 6) ---- */
static int g2_leiche_im_bild(int cut, int *im, int *alle)
{
    const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
    if (k < 0 || !g_scd.props[k].active) return 0;
    const __typeof__(g_scd.props[0]) *pr = &g_scd.props[k];
    int32_t ro[9]; g2_prop_matrix(pr->rot_x, pr->rot_y, pr->rot_z, ro);
    for (int i = 0; i < s_ln; i++) {
        const int32_t w[3] = { pr->x + (int32_t)(((int64_t)ro[0] * s_lv[i][0] + (int64_t)ro[1] * s_lv[i][1] + (int64_t)ro[2] * s_lv[i][2]) >> 12),
                               pr->y + (int32_t)(((int64_t)ro[3] * s_lv[i][0] + (int64_t)ro[4] * s_lv[i][1] + (int64_t)ro[5] * s_lv[i][2]) >> 12),
                               pr->z + (int32_t)(((int64_t)ro[6] * s_lv[i][0] + (int64_t)ro[7] * s_lv[i][1] + (int64_t)ro[8] * s_lv[i][2]) >> 12) };
        (*alle)++;
        { int sx = 0, sy = 0; if (re15_gr2_im_bild_cut(cut, w, &sx, &sy) && sy >= 24 && sy < 216) (*im)++; }
    }
    return 1;
}
static int s_kopf_im[16], s_kopf_alle[16], s_bx0[16] = { 999,999,999,999,999,999,999,999 }, s_bx1[16], s_by0[16] = { 999,999,999,999,999,999,999,999 }, s_by1[16];
static void wurfkamera(int elza)
{
    re15_actor_t *g = g2_szene_start(elza);
    int im[16] = { 0 }, alle[16] = { 0 }, kopf[16] = { 0 }, bilder = 0;
    for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        const int ph = re15_gatorszene_r41_leiche_flug(NULL, NULL, NULL, NULL, NULL);
        if (!(g->motion == 5 && g->anim_frame >= 55 && g->anim_frame <= 77)) continue;
        (void)ph;
        bilder++;
        int32_t k6[3]; const int hk = re15_gr2_bone_welt_pub(g, 6, k6);
        re15_skel_pose_t ps[RE15_EMD_MAX_BONES]; const int hp = g2_pose(g, ps);
        if (hp && getenv("G2_ZEIGE") && (g->anim_frame == 57 || g->anim_frame == 66 || g->anim_frame == 75)) {   /* Maul-Achse im Bild des laufenden Cuts */
            int32_t r6[9], p6[3]; g2_part_welt(g, &ps[6], r6, p6);
            printf("      Bild %d Cut %d Maul-Achse (Part-6-x, y -330):", (int)g->anim_frame, (int)g_scd.cam_id);
            for (int x = 0; x <= 2500; x += 250) {
                const int32_t q[3] = { p6[0] + g2_q12(r6[0], x) + g2_q12(r6[1], -330), p6[1] + g2_q12(r6[3], x) + g2_q12(r6[4], -330), p6[2] + g2_q12(r6[6], x) + g2_q12(r6[7], -330) };
                int sx = -1, sy = -1; re15_gr2_im_bild_cut((int)g_scd.cam_id, q, &sx, &sy);
                printf(" x%d:(%d,%d)", x, sx, sy);
            }
            printf("\n");
        }
        static int32_t pv[4096][3];
        re15_enemy_bank_t *gb = re15_enemy_find(0x23u);
        const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
        for (int c = 0; c < 8 && c < g_room_rdt.cut_count; c++) {
            g2_leiche_im_bild(c, &im[c], &alle[c]);
            if (hk && re15_gr2_im_bild_cut(c, k6, NULL, NULL)) kopf[c]++;
            for (int part = 6; hp && part <= 7; part++) {   /* Kopf-Scheitel (Schnauze bis Hals) im Bild */
                int32_t rb[9], pb[3]; g2_part_welt(g, &ps[part], rb, pb);
                const int n = g2_scheitel(&gb->md1, part, pv, 4096);
                for (int i = 0; i < n; i++) {
                    const int32_t v[3] = { g2_q12(pv[i][0], sk), g2_q12(pv[i][1], sk), g2_q12(pv[i][2], sk) };
                    const int32_t w[3] = { pb[0] + (int32_t)(((int64_t)rb[0] * v[0] + (int64_t)rb[1] * v[1] + (int64_t)rb[2] * v[2]) >> 12),
                                           pb[1] + (int32_t)(((int64_t)rb[3] * v[0] + (int64_t)rb[4] * v[1] + (int64_t)rb[5] * v[2]) >> 12),
                                           pb[2] + (int32_t)(((int64_t)rb[6] * v[0] + (int64_t)rb[7] * v[1] + (int64_t)rb[8] * v[2]) >> 12) };
                    int sx = 0, sy = 0;
                    s_kopf_alle[c]++;
                    if (re15_gr2_im_bild_cut(c, w, &sx, &sy) && sy >= 24 && sy < 216) { s_kopf_im[c]++; if (sx < s_bx0[c]) s_bx0[c] = sx; if (sx > s_bx1[c]) s_bx1[c] = sx; if (sy < s_by0[c]) s_by0[c] = sy; if (sy > s_by1[c]) s_by1[c] = sy; }   /* sichtbar = zwischen den Kino-Balken (render_pc.c: 0..24 und 216..240) */
                }
            }
        }
        if (getenv("G2_ZEIGE")) {
            const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
            int sx = -1, sy = -1, kx = -1, ky = -1; int32_t p[3] = { 0, 0, 0 };
            if (k >= 0) { p[0] = g_scd.props[k].x; p[1] = g_scd.props[k].y; p[2] = g_scd.props[k].z; }
            re15_gr2_im_bild_cut((int)g_scd.cam_id, p, &sx, &sy); re15_gr2_im_bild_cut((int)g_scd.cam_id, k6, &kx, &ky);
            printf("      F%d Bild %d Cut %d: Leiche-Ursprung Bild (%d,%d) aktiv %d, Kopf Bild (%d,%d)\n", f, (int)g->anim_frame, (int)g_scd.cam_id, sx, sy,
                   k >= 0 ? g_scd.props[k].active : -1, kx, ky);
        }
    }
    printf("wurfkamera %s: %d Bilder (Clip 5 Bild 55..77), laufender Cut %d\n", elza ? "Elza" : "Leon", bilder, (int)g_scd.cam_id);
    for (int c = 0; c < 8; c++)
        printf("  Cut %d: Leichen-Scheitel im Bild %d / %d (%d %%), Kopf im Bild %d / %d, Kopf-Scheitel (Part 6/7) im Bild %d / %d (%d %%), Kopf-Kasten (%d,%d)-(%d,%d)\n", c, im[c], alle[c],
               alle[c] ? 100 * im[c] / alle[c] : 0, kopf[c], bilder, s_kopf_im[c], s_kopf_alle[c], s_kopf_alle[c] ? 100 * s_kopf_im[c] / s_kopf_alle[c] : 0,
               s_bx0[c], s_by0[c], s_bx1[c], s_by1[c]);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_door_seq_setze_laeufer(fang);
    if (!strcmp(teil, "maulgeo")) { if (!gator_bank()) return 1; maulgeo(argc > 2); return 0; }
    if (!strcmp(teil, "wurfkamera")) { if (!gator_bank()) return 1; wurfkamera(argc > 2); return 0; }
    if (!gator_bank()) { printf("RE2-EM23-Bank fehlt\n"); return 1; }
    const int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "maul"))      teil_maul_riegel(0);
    if (alle || !strcmp(teil, "maul_elza")) teil_maul_riegel(1);
    if (alle || !strcmp(teil, "wurf"))      teil_wurf(0);
    if (alle || !strcmp(teil, "wurf_elza")) teil_wurf(1);
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
