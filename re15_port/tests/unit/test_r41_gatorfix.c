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
#include "../../engine/src/gen/gatorszene_r41_schluessel.inc"   /* RE2-Modell bytegleich (Gegenprobe Punkt 6) */

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

/* ---- Werkzeug projektion <raum hex> <cut> x,y,z ... : Bildpunkte (320x240) wie der Zeichner (gr2_im_bild) ---- */
static int projektion(int argc, char **argv)
{
    const uint16_t raum = (uint16_t)strtoul(argv[2], NULL, 16);
    const int cut = atoi(argv[3]);
    if (rdt_laden(raum) != 0) { printf("ROOM%04X nicht ladbar\n", raum); return 1; }
    g_room_rdt_ok = 1;
    for (int i = 4; i < argc; i++) {
        int32_t p[3] = { 0, 0, 0 };
        if (sscanf(argv[i], "%d,%d,%d", &p[0], &p[1], &p[2]) != 3) continue;
        int sx = -1, sy = -1;
        const int im = re15_gr2_im_bild_cut(cut, p, &sx, &sy);
        printf("ROOM%04X Cut %d (%d,%d,%d) -> (%d,%d) %s\n", raum, cut, p[0], p[1], p[2], sx, sy, im ? "im Bild" : "draussen");
    }
    return 0;
}

/* ---- Riegel Punkt 3: Montage 2080 - fuenf Zombies, die zur Leiter laufen --------------------------------------------------- */
#include "re15_kanister2080_r41.h"
#define G2_LEITER_X (-9950)   /* Leiter-Ankunft ROOM2080 = ROOM2090 Tuer 0 @0x00996 Nutzlast (-9950,-14400,-25700) */
#define G2_LEITER_Z (-25700)
static int32_t g2_leiter_abstand(const re15_actor_t *a)
{
    const int64_t dx = (int64_t)a->x - G2_LEITER_X, dz = (int64_t)a->z - G2_LEITER_Z;
    return (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
}
/* Gier-Abweichung der Blickrichtung zur Richtung Leiter (|d| in 4096tel) */
static int g2_blick_ab(const re15_actor_t *a)
{
    const int soll = ((int)re15_atan2_q12(G2_LEITER_Z - a->z, G2_LEITER_X - a->x) - 1024) & 0xfff;
    int d = ((((int)a->rot_y - soll) + 0x800) & 0xfff) - 0x800;
    return d < 0 ? -d : d;
}
/* Zombie-Bank 0x16 wie im Spiel (RE2-Flavor Standard, enemy_ai_re2_zombie.c s_flavor; main.c laedt sie aus shared_assets/RE2/CDEMD0.EMS) -
 * ohne Bank laufen die Zombies im Harness nicht (kein Wurzelversatz). Form gator_bank. */
static int g2_zombie_bank(void)
{
    static uint8_t *ems = NULL; static size_t ems_sz = 0;
    if (!ems) ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &ems_sz);
    if (!ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(0x16u);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(0x16u);
    if (!eb) return 0;
    if (re2_ems_load_bank(ems, ems_sz, 0x16, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}
static void teil_montage(int elza)
{
    printf("== montage%s (Punkt 3) ==\n", elza ? "_elza" : "");
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_game_state_init();
    re15_inv_load_briefing();
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return;
    for (int f = 0; f < 600 && !g_room_change.pending; f++) frame(0, 0);
    if (!g2_zombie_bank()) { PRUEF(0, "Zombie-Bank 0x16 (RE2 CDEMD0.EMS)"); return; }
    if (room_boot(r2080, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, g_room_change.target_cut) != 0) return;
    int n = 0; int32_t ab0[RE15_ACTOR_MAX] = { 0 }, ab1[RE15_ACTOR_MAX] = { 0 }; int blick_max = 0, bilder = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x16) { n++; ab0[i] = g2_leiter_abstand(&g_actors[i]); const int b = g2_blick_ab(&g_actors[i]); if (b > blick_max) blick_max = b; }
    for (int f = 0; f < 600 && !g_room_change.pending; f++) { frame(0, 0); bilder++; }
    int naeher = 0; int32_t weg_min = 0x7fffffff, weg_max = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        if (!g_actors[i].active || g_actors[i].type != 0x16 || !ab0[i]) continue;
        ab1[i] = g2_leiter_abstand(&g_actors[i]);
        const int32_t w = ab0[i] - ab1[i];
        if (w > 0) naeher++;
        if (w < weg_min) weg_min = w;
        if (w > weg_max) weg_max = w;
        printf("    Slot %d: Abstand zur Leiter %d -> %d (%+d) in %d Bildern, Lage (%d,%d) Gier %d\n", i, (int)ab0[i], (int)ab1[i], (int)-w, bilder,
               g_actors[i].x, g_actors[i].z, (int)g_actors[i].rot_y);
    }
    printf("    %d Zombies, Start-Blick hoechstens %d von der Leiter-Richtung, %d naeher an der Leiter, Weg zur Leiter %d..%d\n", n, blick_max, naeher, (int)weg_min, (int)weg_max);
    PRUEF(n == 5, "fuenf Zombies in der Montage (%d; vorher 8)", n);
    PRUEF(blick_max <= 2, "Start-Blick zur Leiter (Abweichung hoechstens %d/4096; vorher Blick auf den Treppenabsatz, 1819..2698)", blick_max);
    /* Der Gang selbst (Wurzelversatz aus den Clip-Bildern, die der PC-NPC-Pass vorschiebt) laeuft nur in der exe: Riegel integration_r41_gatorfix_gang. */
    (void)naeher; (void)weg_min;
}

/* ---- Riegel Punkte 4 und 5: Marvin-/John-Szene - drei Zombies laufen zur Leiter, schnell erschossen; Kampfmusik bleibt ---------- */
static void teil_marvin(int elza)
{
    printf("== marvin%s (Punkte 4 + 5) ==\n", elza ? "_elza" : "");
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_k2080_reset();
    re15_actor_t *g = szene_bis_kampf(elza);
    if (!g) { PRUEF(0, "Szene bis zum Kampf"); return; }
    const int gslot = (int)(g - g_actors);
    int bgm_kampf = -2;
    int ausgeloest = 0;
    for (int f = 0; f < 1500 && !g_room_change.pending; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        if (!ausgeloest && re15_gator_boss_verfolgt(g)) {   /* ein echter Treffer unter die RE2-Schwelle (HURT-Selektor @0x80101ebc-dc) */
            g->hp = 24990; re15_gr2_hurt_reaktion(gslot, g, &g_actors[RE15_ACTOR_SLOT_PLAYER], 0); ausgeloest = 1;
            bgm_kampf = re15_gatorszene_r41_bgm_eintrag(1, 9);   /* Musik im Kampf beim Ausloesen */
        }
        frame(0, 0);
    }
    if (!g_room_change.pending || (g_room_change.room_id & 0xFFFEu) != 0x2080u) { PRUEF(0, "Ausloeser -> Raumwechsel nach 2080 (Phase %d)", re15_k2080_phase()); return; }
    if (!g2_zombie_bank()) { PRUEF(0, "Zombie-Bank 0x16 (RE2 CDEMD0.EMS)"); return; }
    if (room_boot(r2080, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, g_room_change.target_cut) != 0) return;
    const int bgm_szene = re15_gatorszene_r41_bgm_eintrag(1, 8);
    frame(0, 0);                                            /* erster VM-Lauf: Sce_em_set der Szene (Abschnitt 1) */
    int n = 0; int32_t ab0[RE15_ACTOR_MAX] = { 0 }, ab_tod[RE15_ACTOR_MAX] = { 0 }; int blick_max = 0, tod_bild[RE15_ACTOR_MAX];
    for (int i = 0; i < RE15_ACTOR_MAX; i++) tod_bild[i] = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x16) { n++; ab0[i] = g2_leiter_abstand(&g_actors[i]); const int b = g2_blick_ab(&g_actors[i]); if (b > blick_max) blick_max = b; }
    int letzter = -1, bilder = 0, bgm_max_abw = 0;
    for (int f = 0; f < 2000 && !g_room_change.pending; f++) {
        frame(0, 0); bilder++;
        if (re15_gatorszene_r41_bgm_eintrag(1, 8) != bgm_szene) bgm_max_abw++;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *z = &g_actors[i];
            if (!ab0[i] || tod_bild[i] >= 0 || z->type != 0x16) continue;
            if (!z->active || z->hp < 0 || z->state == 3 || z->state == 7) { tod_bild[i] = f; ab_tod[i] = g2_leiter_abstand(z); if (f > letzter) letzter = f; }
        }
    }
    int naeher = 0, tot = 0; int32_t weg_min = 0x7fffffff, rest_min = 0x7fffffff;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        if (!ab0[i]) continue;
        if (tod_bild[i] >= 0) tot++;
        const int32_t w = ab0[i] - ab_tod[i];
        if (tod_bild[i] >= 0 && w > 0) naeher++;
        if (w < weg_min) weg_min = w;
        if (ab_tod[i] && ab_tod[i] < rest_min) rest_min = ab_tod[i];
        printf("    Slot %d: Abstand zur Leiter %d -> %d beim Tod in Bild %d\n", i, (int)ab0[i], (int)ab_tod[i], tod_bild[i]);
    }
    int bgm_rueck = -2;
    if (g_room_change.pending && (g_room_change.room_id & 0xFFFEu) == 0x2090u &&
        room_boot(r2090, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, g_room_change.target_cut) == 0) {
        g_actors[gslot].active = 1;   /* FG-E3: derselbe Alligator liegt schon (gator_zuruecklegen) */
        bgm_rueck = re15_gatorszene_r41_bgm_eintrag(1, 9);
    }
    re15_k2080_reset();
    const int bgm_frei = (room_boot(r2080, -9950, -14400, -25700, 2048, 2) == 0) ? re15_gatorszene_r41_bgm_eintrag(1, 8) : -2;
    printf("    %d Zombies, Start-Blick hoechstens %d von der Leiter, %d tot, letzter Tod Bild %d der Szene, %d naeher an der Leiter (kleinster Weg %d), kleinster Rest %d; "
           "Musik-Eintrag Kampf 0x%04X / 2080 in der Szene 0x%04X (%d Bilder anders) / zurueck 0x%04X / 2080 ohne Szene %d\n", n, blick_max, tot, letzter, naeher,
           (int)weg_min, (int)rest_min, bgm_kampf & 0xffff, bgm_szene & 0xffff, bgm_max_abw, bgm_rueck & 0xffff, bgm_frei);
    PRUEF(n == 3 && tot == 3, "drei Zombies in der Szene, alle erschossen (%d / %d; vorher 8)", n, tot);
    PRUEF(blick_max <= 2, "Blick zur Leiter statt zu Marvin/John (Abweichung hoechstens %d/4096)", blick_max);
    PRUEF(rest_min >= 2000, "beim Tod noch >= 2000 von der Leiter (%d) - den Gang misst integration_r41_gatorfix_gang an der exe", (int)rest_min);
    PRUEF(letzter >= 0 && letzter <= 90, "alle erschossen nach %d Bildern (<= 90 = Anlegen 10 + 3 x Feuern 16 + Nachdrehen; vorher 351 mit 8 Zombies)", letzter);
    PRUEF(bgm_kampf == 0xFF3C && bgm_szene == 0xFF3C && bgm_max_abw == 0 && bgm_rueck == 0xFF3C, "Kampfmusik (Eintrag 0xFF3C) durch die ganze Szene und zurueck");
    PRUEF(bgm_frei < 0, "Gegenprobe: 2080 ohne Szene spielt seine Raum-Musik (Eintrag %d = Tabelle)", bgm_frei);
}

/* ---- Auswerter ganglog <state.log> <montage|szene> (exe-Riegel integration_r41_gatorfix_gang): Zombies (Typ 0x16) in den Bildern, in denen der
 *      Spieler an der Leiter-Ankunft steht (PL(-9950,-25700): Montage-Parkplatz GSZ_2080_PARK bzw. Szenen-Ankunft RE15_K2080_LEITER) ----
 * montage: je Zombie Abstand zur Leiter im ersten und letzten solchen Bild, alle >= 1000 naeher (Nutzer Punkt 3 "Richtung Leiter laufen").
 * szene:   je Zombie Abstand im ersten Bild und im Bild seines Todes (st=3 oder hp < 0): naeher (> 0) und noch >= 2000 entfernt (Punkt 4 "weiter Richtung
 *          Leiter Laufen - mit noch ein bisschen Weg dahin, bevor sie erschossen werden"). */
static int ganglog(const char *pfad, const char *modus)
{
    FILE *f = fopen(pfad, "r");
    if (!f) { printf("ganglog: %s fehlt\n", pfad); return 1; }
    static char z[16384];
    int32_t ab0[64], ab1[64]; int da[64], tot[64], n_bilder = 0;
    for (int i = 0; i < 64; i++) { ab0[i] = ab1[i] = -1; da[i] = 0; tot[i] = 0; }
    const int szene = !strcmp(modus, "szene");
    while (fgets(z, sizeof z, f)) {
        if (!strstr(z, "PL(-9950,-25700,")) continue;
        n_bilder++;
        for (char *p = strstr(z, " ["); p; p = strstr(p + 2, " [")) {
            int slot = -1, st = 0; unsigned t = 0; int x = 0, zz = 0, hp = 0;
            if (sscanf(p, " [%d t=%x st=%d", &slot, &t, &st) != 3 || t != 0x16 || slot < 0 || slot >= 64) continue;
            const char *at = strstr(p, "@("); const char *h = strstr(p, "] hp=");
            if (!at || !h || sscanf(at, "@(%d,%d", &x, &zz) != 2 || sscanf(h, "] hp=%d", &hp) != 1) continue;
            const int64_t dx = (int64_t)x - G2_LEITER_X, dz = (int64_t)zz - G2_LEITER_Z;
            const int32_t ab = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
            if (ab0[slot] < 0) ab0[slot] = ab;
            da[slot] = 1;
            if (szene && !tot[slot] && (st == 3 || hp < 0)) { tot[slot] = 1; ab1[slot] = ab; }
            if (!szene) ab1[slot] = ab;
        }
    }
    fclose(f);
    int n = 0, gut = 0; int32_t weg_min = 0x7fffffff, rest_min = 0x7fffffff;
    for (int i = 0; i < 64; i++) {
        if (!da[i]) continue;
        n++;
        const int32_t w = (ab1[i] >= 0) ? ab0[i] - ab1[i] : -1;
        if (w < weg_min) weg_min = w;
        if (ab1[i] >= 0 && ab1[i] < rest_min) rest_min = ab1[i];
        const int ok = szene ? (tot[i] && w > 0 && ab1[i] >= 2000) : (w >= 1000);
        if (ok) gut++;
        printf("ganglog %s: Slot %d Abstand zur Leiter %d -> %d (%s), Weg %d\n", modus, i, (int)ab0[i], (int)ab1[i], szene ? (tot[i] ? "beim Tod" : "nicht tot") : "Ende", (int)w);
    }
    printf("ganglog %s: %d Bilder an der Leiter, %d Zombies, %d gut, kleinster Weg %d, kleinster Rest %d\n", modus, n_bilder, n, gut, (int)weg_min, (int)rest_min);
    const int soll = szene ? 3 : 5;
    return (n == soll && gut == soll) ? 0 : 1;
}

/* ---- Riegel Punkt 6: Schluessel groesser (Ankunft auf der Leiche + nach dem Tod) ------------------------------------------------- */
/* Weltmatrix eines Props mit einer Elternstufe (main.c pc_prop_world, Tiefe 1 genuegt: Schluessel -> Leiche) */
static void g2_prop_welt(int k, int32_t r[9], int32_t t[3])
{
    g2_prop_matrix(g_scd.props[k].rot_x, g_scd.props[k].rot_y, g_scd.props[k].rot_z, r);
    t[0] = g_scd.props[k].x; t[1] = g_scd.props[k].y; t[2] = g_scd.props[k].z;
    const int pid = g_scd.props[k].parent_obj;
    const int pk = (pid >= 0) ? prop_von(pid) : -1;
    if (pk < 0 || pk == k) return;
    int32_t pr[9], nr[9], nt[3];
    g2_prop_matrix(g_scd.props[pk].rot_x, g_scd.props[pk].rot_y, g_scd.props[pk].rot_z, pr);
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) nr[a*3+b] = (int32_t)(((int64_t)pr[a*3] * r[b] + (int64_t)pr[a*3+1] * r[3+b] + (int64_t)pr[a*3+2] * r[6+b]) >> 12);
        nt[a] = g_scd.props[pk].x * (a == 0) + g_scd.props[pk].y * (a == 1) + g_scd.props[pk].z * (a == 2)
              + (int32_t)(((int64_t)pr[a*3] * t[0] + (int64_t)pr[a*3+1] * t[1] + (int64_t)pr[a*3+2] * t[2]) >> 12);
    }
    memcpy(r, nr, sizeof nr); memcpy(t, nt, sizeof nt);
}
/* Bildkasten (Breite x Hoehe) des Schluessel-Modells md1 im Bild von Cut cut an der aktuellen Prop-Lage */
static int g2_key_kasten(const uint8_t *md1, int n, int cut, int *b, int *h)
{
    static re15_md1_t m; static int32_t v[64][3];
    const int k = prop_von(RE15_GSZ_KEY_OBJ);
    if (k < 0 || !g_scd.props[k].active || re15_md1_parse(md1, n, &m) != 0) return 0;
    const int nv = g2_scheitel(&m, -1, v, 64);
    int32_t r[9], t[3]; g2_prop_welt(k, r, t);
    int x0 = 99999, x1 = -99999, y0 = 99999, y1 = -99999;
    for (int i = 0; i < nv; i++) {
        const int32_t w[3] = { t[0] + (int32_t)(((int64_t)r[0] * v[i][0] + (int64_t)r[1] * v[i][1] + (int64_t)r[2] * v[i][2]) >> 12),
                               t[1] + (int32_t)(((int64_t)r[3] * v[i][0] + (int64_t)r[4] * v[i][1] + (int64_t)r[5] * v[i][2]) >> 12),
                               t[2] + (int32_t)(((int64_t)r[6] * v[i][0] + (int64_t)r[7] * v[i][1] + (int64_t)r[8] * v[i][2]) >> 12) };
        int sx = 0, sy = 0;
        re15_gr2_im_bild_cut(cut, w, &sx, &sy);
        if (sx < x0) x0 = sx; if (sx > x1) x1 = sx; if (sy < y0) y0 = sy; if (sy > y1) y1 = sy;
    }
    *b = x1 - x0; *h = y1 - y0;
    return 1;
}
static void teil_schluessel_g2(int elza)
{
    printf("== schluessel%s (Punkt 6) ==\n", elza ? "_elza" : "");
    int n = 0; const uint8_t *md = re15_gatorszene_r41_md1_bytes((uint16_t)(0x2090 + elza), &n);
    static re15_md1_t m; static int32_t v[64][3];
    int32_t lo[3], hi[3], lo0[3], hi0[3];
    const int nv = (md && re15_md1_parse(md, n, &m) == 0) ? g2_scheitel(&m, -1, v, 64) : 0;
    g2_bbox(v, nv, lo, hi);
    static re15_md1_t m0; static int32_t v0[64][3];
    const int nv0 = (re15_md1_parse(re15_gsz_key_md1, (int)sizeof re15_gsz_key_md1, &m0) == 0) ? g2_scheitel(&m0, -1, v0, 64) : 0;
    g2_bbox(v0, nv0, lo0, hi0);
    printf("    gezeichnetes Modell: %d Scheitel x %d..%d z %d..%d; RE2-Modell ROOM7040 @0x354C: x %d..%d z %d..%d\n", nv, lo[0], hi[0], lo[2], hi[2],
           lo0[0], hi0[0], lo0[2], hi0[2]);
    PRUEF(nv == 4 && hi[0] - lo[0] == 270 && hi[2] - lo[2] == 270 && hi0[0] - lo0[0] == 180, "Quadrat 270 statt 180 (RE2-Modell bytegleich, Skala 3/2)");
    /* Ankunft: Leiche + Schluessel in der Leichen-Einstellung Cut 0 (Programm 1, GSZ_CUT_LEICHE) */
    const uint16_t r2090 = (uint16_t)(0x2090 + elza);
    re15_game_state_init();
    re15_inv_load_briefing();
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return;
    for (int f = 0; f < 10; f++) frame(0, 0);
    int b = 0, h = 0, b0 = 0, h0 = 0;
    const int ok1 = g2_key_kasten(md, n, 0, &b, &h), ok0 = g2_key_kasten(re15_gsz_key_md1, (int)sizeof re15_gsz_key_md1, 0, &b0, &h0);
    printf("    Ankunft Cut 0: Kasten %d x %d Bildpunkte (mit dem RE2-Modell wie vorher %d x %d)\n", b, h, b0, h0);
    PRUEF(ok1 && ok0 && b * h * 10 >= b0 * h0 * 18, "Ankunft: Schluessel-Flaeche >= 1,8 x (Skala 3/2 = 2,25 x, Rundung ganzer Bildpunkte) (%d x %d gegen %d x %d)", b, h, b0, h0);
    /* nach dem Tod (Form teil_schluessel der Spur F: Leiche Zustand 7) */
    re15_game_state_init();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(r2090, -8200, 0, -25550, 0, 0) != 0) return;
    re15_enemy_ai_set_paused(1);
    re15_actor_t *g = gator_setzen();
    if (g) { g->x = -3000; g->z = -22000; g->y = -1200; g->state = 7; }
    for (int f = 0; f < 5; f++) frame(0, 0);
    const int cut = re15_gatorszene_r41_key_kamera();
    const int ok2 = cut >= 0 && g2_key_kasten(md, n, cut, &b, &h), ok3 = cut >= 0 && g2_key_kasten(re15_gsz_key_md1, (int)sizeof re15_gsz_key_md1, cut, &b0, &h0);
    printf("    nach dem Tod Cut %d: Kasten %d x %d Bildpunkte (RE2-Modell %d x %d)\n", cut, b, h, b0, h0);
    PRUEF(ok2 && ok3 && b * h * 10 >= b0 * h0 * 18, "nach dem Tod: Schluessel-Flaeche >= 1,8 x (%d x %d gegen %d x %d)", b, h, b0, h0);
    re15_enemy_ai_set_paused(0);
}

/* ---- Werkzeug beckenkarte <raum hex> <aus.txt>: je 100er-Zelle x -11000..9000, z -29000..-3000: 1 = Wasser des Bosskampfs (re15_gr2_becken_draussen
 *      == 0 mit Rand 0), 2 = Wasserzone re15_aot_water_at, 0 sonst ---- */
static int beckenkarte(const char *raum_s, const char *aus)
{
    const uint16_t raum = (uint16_t)strtoul(raum_s, NULL, 16);
    re15_game_state_init();
    re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1);
    if (room_boot(raum, -8200, 0, -25550, 0, 0) != 0) return 1;
    if (!gator_bank()) return 1;
    re15_actor_t *g = gator_setzen();
    for (int f = 0; f < 3; f++) frame(0, 0);
    (void)g;
    FILE *o = fopen(aus, "w");
    if (!o) return 1;
    for (int z = -29000; z <= -3000; z += 100) {
        for (int x = -11000; x <= 9000; x += 100) {
            const int b = re15_gr2_becken_draussen(x, z, 0) == 0;
            const int w = re15_aot_water_at(x, z) != 0;
            fputc(b ? (w ? '3' : '1') : (w ? '2' : '0'), o);
        }
        fputc('\n', o);
    }
    fclose(o);
    return 0;
}

/* ---- Werkzeug/Riegel Punkt 7: Haenger mit dem Kanister im Maul - echter Boss-Takt, die Spielfigur laeuft einen festen Weg durchs Becken
 *      (West-Kanal und Nord-/Suedbecken, 25 Einheiten je Bild), der Alligator verfolgt sie mit bzw. ohne Kanister.
 *      HAENGER = 30 Bilder Verfolgung (Phase CHASE), in denen die Wurzel netto < 300 vorankommt, obwohl die Figur > 4000 entfernt ist. ---- */
#define G2_WEG_N 9
static int32_t k_weg[G2_WEG_N][2] = { { -6500, -24000 }, { -6500, -9000 }, { 4000, -9000 }, { -6500, -9000 }, { -6500, -24000 }, { 4000, -24000 }, { -6500, -24000 },
                                      { -6500, -24000 }, { -6500, -24000 } };
/* zweiter Weg: die vier Ecken des Beckens (Aussenwaende x -8900..7200, z -27000..-5400, je 900 davor), dazwischen langsam */
static const int32_t k_weg_ecken[G2_WEG_N][2] = { { -8000, -26100 }, { -8000, -6300 }, { 6300, -6300 }, { -8000, -6300 }, { -8000, -26100 }, { 6300, -26100 },
                                                  { -8000, -26100 }, { -8000, -6300 }, { 6300, -6300 } };
typedef struct { int bilder, verfolgt, haenger, haenger_bilder; int32_t lage[64][2]; int n_lage; int32_t weg_min; } g2_haeng_t;
static void g2_weg_punkt(int t, int32_t *x, int32_t *z)
{
    int32_t rest = t * 25;
    for (int i = 0; i + 1 < G2_WEG_N; i++) {
        const int64_t dx = k_weg[i+1][0] - k_weg[i][0], dz = k_weg[i+1][1] - k_weg[i][1];
        const int32_t l = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        if (rest <= l) { *x = k_weg[i][0] + (int32_t)(dx * rest / l); *z = k_weg[i][1] + (int32_t)(dz * rest / l); return; }
        rest -= l;
    }
    *x = k_weg[G2_WEG_N-1][0]; *z = k_weg[G2_WEG_N-1][1];
}
static int g2_haenger_lauf(int elza, int mit_kanister, int bilder, g2_haeng_t *m, int zeige)
{
    memset(m, 0, sizeof *m);
    re15_actor_t *g = szene_bis_kampf(elza);
    if (!g) return 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (mit_kanister) {   /* Kanister vor das Maul (Form trag_lauf der Spur P: 8800 x Skala voraus), Aufnahme abwarten */
        const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
        const int gier = g->rot_y & 0xfff;
        int32_t vx = (int32_t)(((int64_t)re15_cos_q12(gier) * 8800 * sk) >> 24), vz = (int32_t)(((int64_t)-re15_sin_q12(gier) * 8800 * sk) >> 24);
        if (!re15_aot_water_at(g->x + vx, g->z + vz)) { vx = vz = 0; }
        re15_gr2_kanister_setzen(g->x + vx, -1200, g->z + vz);
        const int32_t x0 = g->x, z0 = g->z;
        for (int f = 0; f < 300 && !re15_gr2_traegt(); f++) { g->x = x0; g->z = z0; g->rot_y = (int16_t)gier; pl->x = -8200; pl->z = -25550; pl->hp = 100; frame(0, 0); }
        if (!re15_gr2_traegt()) return 0;
    }
    int32_t hist[30][2]; int hn = 0, stand = 0; int32_t sx0 = 0, sz0 = 0;
    for (int t = 0; t < bilder && (!mit_kanister || re15_gr2_traegt()); t++) {
        int32_t px, pz; g2_weg_punkt(t, &px, &pz);
        pl->x = px; pl->z = pz; pl->hp = 100; pl->hit_react = 0;
        frame(0, 0);
        m->bilder++;
        const int64_t dx = (int64_t)g->x - px, dz = (int64_t)g->z - pz;
        const int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        const int chase = re15_gator_boss_phase(g) == 2;
        if (chase) m->verfolgt++;
        hist[hn % 30][0] = g->x; hist[hn % 30][1] = g->z; hn++;
        if (chase && d > 4000 && hn >= 30) {
            const int32_t *a = hist[hn % 30];
            const int64_t wx = (int64_t)g->x - a[0], wz = (int64_t)g->z - a[1];
            const int32_t w = (int32_t)re15_squareroot0((uint32_t)(wx * wx + wz * wz));
            if (w < 300) {
                if (!stand) { stand = 1; m->haenger++; sx0 = g->x; sz0 = g->z;
                    if (m->n_lage < 64) { m->lage[m->n_lage][0] = g->x; m->lage[m->n_lage][1] = g->z; m->n_lage++; }
                    if (zeige) printf("      HAENGER t%d Gator (%d,%d) Gier %d Clip %d Phase %d, Figur (%d,%d) Abstand %d\n", t, g->x, g->z, (int)(g->rot_y & 0xfff),
                                      (int)g->motion, re15_gator_boss_phase(g), px, pz, (int)d); }
                m->haenger_bilder++;
            } else stand = 0;
        } else stand = 0;
        (void)sx0; (void)sz0;
    }
    return 1;
}
static void teil_haenger(int elza)
{
    printf("== haenger%s (Punkt 7) ==\n", elza ? "_elza" : "");
    g2_haeng_t mk, mo;
    const int zeige = getenv("G2_ZEIGE") != NULL;
    if (getenv("G2_ECKEN")) memcpy(k_weg, k_weg_ecken, sizeof k_weg);
    const int ok1 = g2_haenger_lauf(elza, 1, 1500, &mk, zeige);
    const int ok0 = g2_haenger_lauf(elza, 0, 1500, &mo, zeige);
    printf("    mit Kanister: %d Bilder, %d verfolgt, %d Haenger (%d Bilder); ohne Kanister: %d Bilder, %d verfolgt, %d Haenger (%d Bilder)\n",
           mk.bilder, mk.verfolgt, mk.haenger, mk.haenger_bilder, mo.bilder, mo.verfolgt, mo.haenger, mo.haenger_bilder);
    PRUEF(ok1 && ok0 && mk.bilder >= 1000, "Laeufe gelaufen (%d Bilder mit Kanister)", mk.bilder);
    PRUEF(mk.haenger_bilder <= mo.haenger_bilder + 30, "mit Kanister nicht mehr Haenger-Bilder als ohne (%d gegen %d)", mk.haenger_bilder, mo.haenger_bilder);
}

/* ---- Riegel Punkt 7: Ziel und Kopf-Klemme widersprechen sich nicht mehr ------------------------------------------------------
 * Lage der Nutzer-Marke 1 (2026-11-01 F735, gator_boss.log F636..F1208: Wurzel (3191,-22595) Gier 1, Clip 1 Bild 20, Ziel (4450,-22600), Ueberstand
 * (-207,0)) und dieselbe Lage gegen jede Aussenwand gedreht: der Alligator mit dem Kanister im Maul wird AUF sein Steuerziel gestellt (Gier =
 * Ankunftsrichtung) und die Kopf-Klemme gemessen. Neu: Ueberstand 0 am verschobenen Ziel (er kommt an); Gegenprobe Ziel wie vor G2: Ueberstand > 0. */
static int32_t g2_ueber_am_ziel(re15_actor_t *g, int slot, int32_t x0, int32_t z0, int32_t tx, int32_t tz, int alt, int32_t *zx, int32_t *zz)
{
    re15_gr2_kk_ziel_alt_setzen(alt);
    g->x = x0; g->z = z0;
    g->rot_y = (int16_t)(((int)re15_atan2_q12(tz - z0, tx - x0) - 1024) & 0xfff);
    (void)re15_gr2_kk_ziel(g, &tx, &tz);
    re15_gr2_kk_ziel_alt_setzen(0);
    *zx = tx; *zz = tz;
    g->rot_y = (int16_t)(((int)re15_atan2_q12(tz - z0, tx - x0) - 1024) & 0xfff);
    g->x = tx; g->z = tz;
    int32_t dx = 0, dz = 0;
    re15_gr2_kopf_klemme(slot, g, &dx, &dz);
    return (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
}
static void teil_klemm(int elza)
{
    printf("== klemm%s (Punkt 7) ==\n", elza ? "_elza" : "");
    re15_actor_t *g = szene_bis_kampf(elza);
    if (!g) { PRUEF(0, "Szene bis zum Kampf"); return; }
    const int slot = (int)(g - g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    const int gier0 = g->rot_y & 0xfff;
    const int32_t vx = (int32_t)(((int64_t)re15_cos_q12(gier0) * 8800 * sk) >> 24), vz = (int32_t)(((int64_t)-re15_sin_q12(gier0) * 8800 * sk) >> 24);
    re15_gr2_kanister_setzen(g->x + vx, -1200, g->z + vz);
    const int32_t x0 = g->x, z0 = g->z;
    for (int f = 0; f < 300 && !re15_gr2_traegt(); f++) { g->x = x0; g->z = z0; g->rot_y = (int16_t)gier0; pl->x = -8200; pl->z = -25550; pl->hp = 100; frame(0, 0); }
    if (!re15_gr2_traegt()) { PRUEF(0, "Kanister aufgenommen"); return; }
    g->motion = 1; g->anim_frame = 20; g->anim_frac = 0; g->anim_prev_valid = 0;   /* Clip 1 Bild 20 wie in der Marke */
    static const int32_t fall[4][4] = { { 3191, -22595, 4450, -22600 },     /* Ost (Nutzer-Marke) */
                                        { -4000, -22600, -7300, -22600 },  /* West */
                                        { 3000, -20500, 3000, -25400 },    /* Sued */
                                        { -4000, -10000, -4000, -7000 } }; /* Nord (Lauf k_tragen_leon: ziel z -7000) */
    static const char *name[4] = { "Ost (Marke 1)", "West", "Sued", "Nord" };
    int ok = 1, gegen = 1;
    for (int i = 0; i < 4; i++) {
        int32_t zx = 0, zz = 0, ax = 0, az = 0;
        const int32_t u_neu = g2_ueber_am_ziel(g, slot, fall[i][0], fall[i][1], fall[i][2], fall[i][3], 0, &zx, &zz);
        const int32_t u_alt = g2_ueber_am_ziel(g, slot, fall[i][0], fall[i][1], fall[i][2], fall[i][3], 1, &ax, &az);
        printf("    %-14s Wurzel (%d,%d) Ziel (%d,%d): neu Ziel (%d,%d) Ueberstand %d | wie vor G2 Ziel (%d,%d) Ueberstand %d\n", name[i], fall[i][0], fall[i][1],
               fall[i][2], fall[i][3], zx, zz, (int)u_neu, ax, az, (int)u_alt);
        if (u_neu != 0) ok = 0;
        if (u_alt <= 0) gegen = 0;
    }
    PRUEF(ok, "am Steuerziel greift die Kopf-Klemme nicht (alle vier Waende)");
    PRUEF(gegen, "Gegenprobe Ziel wie vor G2: Kopf-Klemme greift am Ziel (Festdruecken der Nutzer-Marke)");
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_door_seq_setze_laeufer(fang);
    if (!strcmp(teil, "projektion") && argc >= 5) return projektion(argc, argv);
    if (!strcmp(teil, "ganglog") && argc >= 4) return ganglog(argv[2], argv[3]);
    if (!strcmp(teil, "beckenkarte") && argc >= 4) return beckenkarte(argv[2], argv[3]);
    if (!strcmp(teil, "maulgeo")) { if (!gator_bank()) return 1; maulgeo(argc > 2); return 0; }
    if (!strcmp(teil, "wurfkamera")) { if (!gator_bank()) return 1; wurfkamera(argc > 2); return 0; }
    if (!gator_bank()) { printf("RE2-EM23-Bank fehlt\n"); return 1; }
    const int alle = !strcmp(teil, "alle");
    if (alle || !strcmp(teil, "maul"))      teil_maul_riegel(0);
    if (alle || !strcmp(teil, "maul_elza")) teil_maul_riegel(1);
    if (alle || !strcmp(teil, "wurf"))      teil_wurf(0);
    if (alle || !strcmp(teil, "wurf_elza")) teil_wurf(1);
    if (alle || !strcmp(teil, "montage"))      teil_montage(0);
    if (alle || !strcmp(teil, "montage_elza")) teil_montage(1);
    if (alle || !strcmp(teil, "marvin"))       teil_marvin(0);
    if (alle || !strcmp(teil, "marvin_elza"))  teil_marvin(1);
    if (alle || !strcmp(teil, "schluessel"))      teil_schluessel_g2(0);
    if (alle || !strcmp(teil, "schluessel_elza")) teil_schluessel_g2(1);
    if (alle || !strcmp(teil, "haenger"))      teil_haenger(0);
    if (alle || !strcmp(teil, "haenger_elza")) teil_haenger(1);
    if (alle || !strcmp(teil, "klemm"))      teil_klemm(0);
    if (alle || !strcmp(teil, "klemm_elza")) teil_klemm(1);
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
