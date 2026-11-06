/*
 * test_r41_gatorfix.c - Runde 41 Spur G2 "gatorfix". Dossier analysis/befunde_runde41/G2_gatorfix.md.
 *
 * Nutzt den Harness von test_r41_gatorszene.c (echte Engine-Wege: Raumaufbau, SCD-VM, Boss-Takt, RE2-EM23-Bank); die Datei wird
 * eingebunden, ihr main heisst hier gsz_haupt.
 *
 * Werkzeug (kein Riegel):
 *   maulgeo [elza]        Geometrie der Leiche im Maul je Bild (Leichen-Scheitel im Rahmen von Part 6 / Part 7)
 *   leichenfit [elza]     NB4 (M4): Einpassung je Bild im Rollfenster gegen Stand vor NB4 und das erreichbare Minimum
 *   kamerazone <raum> [band] x,z ...   NB4: RVD-Zonen und Fixpunkt-Kamera je Start-Cut
 *   begehbar <raum> <band> x,z ...     NB4: schiebt die Spieler-Kollision die Figur auf diesem Band weg?
 *   sichtkarte <raum> <cut>           NB4: Kreispunkte (von 16, R 2000) im Bild je Mittelpunkt
 *   (Umgebung G2_SPUR=1 im Fuzz/kamera: Lage von Alligator und Figur alle 60 Bilder)
 */
#define main gsz_haupt
#include <math.h>
#include <time.h>
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
    int32_t ro[9], to[3]; re15_gatorszene_r41_leiche_matrix(ro, to);   /* NB4: wie gezeichnet (Drehung x Laengsskala) */
    const struct { int32_t x, y, z; } prv = { to[0], to[1], to[2] }, *pr = &prv;
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
/* ---- NB4 (Abnahme 2 M4): Kopf-Band in ZWEI Rahmen - A Kieferrahmen (Part 6, = g2_band) und B waagerechter Rahmen (x = Kieferachse, y = Welt-
 *      Senkrechte ohne x-Anteil), je oben und unten; Mass der Einpassung gatorszene_r41.c (Abschnitt EINPASSEN), hier unabhaengig in Gleitkomma
 *      nachgerechnet. g2_ab_minimum: kleinste Zahl Scheitel draussen ueber den erlaubten Raum der Einpassung (Rollanteil 0..1 in 1/16, Laengsskala
 *      1..0,6 in 0,05, Versatz dy +-480 in 40, dz +-800 in 50) - das, was je Bild ueberhaupt erreichbar ist. ---- */
typedef struct { int nh; double x0, x1, Y[3], YA[3], beta, R6[9], p6[3]; } g2_ab_t;
static double s_ab_hx[2048], s_ab_ya[2048], s_ab_yb[2048];
static int g2_ab_kopf(re15_actor_t *g, const re15_skel_pose_t *ps, g2_ab_t *k)
{
    static int32_t pv[4096][3];
    re15_enemy_bank_t *gb = re15_enemy_find(0x23u);
    const double sk = g->render_scale_q12 ? g->render_scale_q12 / 4096.0 : 1.0;
    int32_t r6i[9], p6i[3]; g2_part_welt(g, &ps[6], r6i, p6i);
    for (int i = 0; i < 9; i++) k->R6[i] = r6i[i] / 4096.0;
    for (int i = 0; i < 3; i++) k->p6[i] = p6i[i];
    const double X[3] = { k->R6[0], k->R6[3], k->R6[6] };
    double Y[3] = { -X[1] * X[0], 1.0 - X[1] * X[1], -X[1] * X[2] };
    const double ln = sqrt(Y[0] * Y[0] + Y[1] * Y[1] + Y[2] * Y[2]);
    if (ln <= 0) return 0;
    for (int i = 0; i < 3; i++) k->Y[i] = Y[i] / ln;
    for (int i = 0; i < 3; i++) k->YA[i] = k->R6[0*3+i] * k->Y[0] + k->R6[1*3+i] * k->Y[1] + k->R6[2*3+i] * k->Y[2];
    k->beta = atan2(k->YA[2], k->YA[1]);
    k->nh = 0; k->x0 = 1e9; k->x1 = -1e9;
    for (int part = 6; part <= 7; part++) {
        int32_t rb[9], pb[3]; g2_part_welt(g, &ps[part], rb, pb);
        const int n = g2_scheitel(&gb->md1, part, pv, 4096);
        for (int i = 0; i < n && k->nh < 2048; i++) {
            const double v[3] = { pv[i][0] * sk, pv[i][1] * sk, pv[i][2] * sk };
            double w[3]; for (int r = 0; r < 3; r++) w[r] = pb[r] + (rb[r*3] * v[0] + rb[r*3+1] * v[1] + rb[r*3+2] * v[2]) / 4096.0 - k->p6[r];
            const double lx = k->R6[0] * w[0] + k->R6[3] * w[1] + k->R6[6] * w[2];
            s_ab_hx[k->nh] = lx;
            s_ab_ya[k->nh] = k->R6[1] * w[0] + k->R6[4] * w[1] + k->R6[7] * w[2];
            s_ab_yb[k->nh] = w[0] * k->Y[0] + w[1] * k->Y[1] + w[2] * k->Y[2];
            if (lx < k->x0) k->x0 = lx;
            if (lx > k->x1) k->x1 = lx;
            k->nh++;
        }
    }
    return 1;
}
/* Band an x: [0] A oben, [1] A unten, [2] B oben, [3] B unten (Fenster +-G2_FENSTER, x am Kopf-Ende geklemmt wie g2_band_bei) */
static void g2_ab_band(const g2_ab_t *k, double x, double b[4])
{
    if (x < k->x0 + G2_FENSTER) x = k->x0 + G2_FENSTER;
    if (x > k->x1 - G2_FENSTER) x = k->x1 - G2_FENSTER;
    b[0] = 1e9; b[1] = -1e9; b[2] = 1e9; b[3] = -1e9;
    for (int h = 0; h < k->nh; h++) {
        if (s_ab_hx[h] < x - G2_FENSTER || s_ab_hx[h] > x + G2_FENSTER) continue;
        if (s_ab_ya[h] < b[0]) b[0] = s_ab_ya[h];
        if (s_ab_ya[h] > b[1]) b[1] = s_ab_ya[h];
        if (s_ab_yb[h] < b[2]) b[2] = s_ab_yb[h];
        if (s_ab_yb[h] > b[3]) b[3] = s_ab_yb[h];
    }
}
/* gezeichnete Leiche (re15_gatorszene_r41_leiche_matrix = Drehung x Laengsskala): Scheitel draussen in A (na) und B (nb) */
static int s_kl_bild = 0, s_kl_bild_max = 0; static double s_kl_weg_max = 0;   /* NB5: geklemmte Scheitel im Bild / hoechstens, groesster Weg */
static int g2_ab_jetzt(const g2_ab_t *k, int *na, int *nb)
{
    s_kl_bild = 0;
    int32_t m[9], t[3];
    if (!re15_gatorszene_r41_leiche_matrix(m, t)) return 0;
    *na = 0; *nb = 0;
    for (int i = 0; i < s_ln; i++) {
        double w[3];
        /* NB5 (M1): der Scheitel, wie der Zeichner ihn zeichnet (Klemmen je Scheitel, re15_gatorszene_r41_leiche_klemm; ausserhalb des Fensters unveraendert) */
        const int16_t v0[3] = { (int16_t)s_lv[i][0], (int16_t)s_lv[i][1], (int16_t)s_lv[i][2] }; int16_t v[3];
        if (re15_gatorszene_r41_leiche_klemm(v0, v)) {   /* NB5: Zahl und Weg der geklemmten Scheitel (Welt-Einheiten) */
            s_kl_bild++;
            double d2 = 0; for (int r = 0; r < 3; r++) { const double dd = (m[r*3] * (double)(v[0] - v0[0]) + m[r*3+1] * (double)(v[1] - v0[1]) + m[r*3+2] * (double)(v[2] - v0[2])) / 4096.0; d2 += dd * dd; }
            if (sqrt(d2) > s_kl_weg_max) s_kl_weg_max = sqrt(d2);
        }
        for (int r = 0; r < 3; r++) w[r] = t[r] + (m[r*3] * (double)v[0] + m[r*3+1] * (double)v[1] + m[r*3+2] * (double)v[2]) / 4096.0 - k->p6[r];
        double l[3]; for (int c = 0; c < 3; c++) l[c] = k->R6[0*3+c] * w[0] + k->R6[1*3+c] * w[1] + k->R6[2*3+c] * w[2];
        const double yb = w[0] * k->Y[0] + w[1] * k->Y[1] + w[2] * k->Y[2];
        double b[4]; g2_ab_band(k, l[0], b);
        *na += (l[1] < b[0] || l[1] > b[1]);
        *nb += (yb < b[2] || yb > b[3]);
        if (getenv("G2_KL") && (l[1] < b[0] || l[1] > b[1] || yb < b[2] || yb > b[3]))
            printf("        KL Scheitel %d Modell (%d,%d,%d) -> (%d,%d,%d) Kiefer (%.0f,%.0f,%.0f) A [%.0f,%.0f] yB %.0f B [%.0f,%.0f] YA (%.3f,%.3f,%.3f)\n", i, s_lv[i][0], s_lv[i][1], s_lv[i][2],
                   v[0], v[1], v[2], l[0], l[1], l[2], b[0], b[1], yb, b[2], b[3], k->YA[0], k->YA[1], k->YA[2]);
    }
    return 1;
}
/* Minimum ueber den erlaubten Raum (Halte-Relation wie gatorszene_r41.c leiche_rel_soll: Kipp 171, Rumpfmitte (1090,-170,-100) auf der Maullage
 * (675,-495) x Zeichen-Skala im Part-6-Rahmen) */
static int g2_ab_minimum(re15_actor_t *g, const g2_ab_t *k, double *b_a, double *b_s, double *b_dy, double *b_dz)
{
    static double bd[512][4], lx[512], y0[512], z0[512], ya[512], za[512];
    const double sk = g->render_scale_q12 ? g->render_scale_q12 / 4096.0 : 1.0;
    const double kc = cos(171 * 2 * 3.14159265358979 / 4096), ks = sin(171 * 2 * 3.14159265358979 / 4096);
    const double Rs[9] = { 0, 0, -1, -ks, kc, 0, kc, ks, 0 };
    const double m[3] = { 1090, -170, -100 }, mt[3] = { 675 * sk, -495 * sk, 0 };
    for (int i = 0; i < s_ln; i++) {
        const double d1 = s_lv[i][1] - m[1], d2 = s_lv[i][2] - m[2];
        lx[i] = Rs[1] * d1 + Rs[2] * d2 + mt[0];
        g2_ab_band(k, lx[i], bd[i]);
    }
    int best = 1 << 30, best_n = 0;
    for (int ai = 0; ai <= 16; ai++)
    for (int si = 0; si <= 8; si++) {
        const double sc = 1.0 - 0.05 * si, ang = k->beta * ai / 16.0, c = cos(ang), s = sin(ang);
        for (int i = 0; i < s_ln; i++) {
            const double d0 = (s_lv[i][0] - m[0]) * sc, d1 = s_lv[i][1] - m[1], d2 = s_lv[i][2] - m[2];
            y0[i] = Rs[3] * d0 + Rs[4] * d1 + Rs[5] * d2; z0[i] = Rs[6] * d0 + Rs[7] * d1 + Rs[8] * d2;
            ya[i] = c * y0[i] - s * z0[i] + mt[1]; za[i] = s * y0[i] + c * z0[i] + mt[2];
        }
        for (int dyi = -12; dyi <= 12; dyi++)
        for (int dzi = -16; dzi <= 16; dzi++) {
            const double dy = dyi * 40.0, dz = dzi * 50.0;
            int n = 0;
            for (int i = 0; i < s_ln && n < best_n + 1 + (best == (1 << 30) ? 1000 : 0); i++) {
                const double y = ya[i] + dy, z = za[i] + dz, yb = k->YA[0] * lx[i] + k->YA[1] * y + k->YA[2] * z;
                n += (y < bd[i][0] || y > bd[i][1]) + (yb < bd[i][2] || yb > bd[i][3]);
            }
            const int pen = n * 64 + ai + si + abs(dyi) + abs(dzi);
            if (pen < best) { best = pen; best_n = n; *b_a = ai / 16.0; *b_s = sc; *b_dy = dy; *b_dz = dz; }
        }
    }
    return best_n;
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
        if (getenv("G2_MITTE") && g->motion == 4) {   /* NB1 H1: Rumpfmitte im Part-6-Rahmen und Kopf-Band an ihrem x je Biss-Bild */
            const int kk = prop_von(RE15_GSZ_LEICHE_OBJ);
            re15_skel_pose_t q[RE15_EMD_MAX_BONES];
            if (kk >= 0 && g2_pose(g, q)) {
                int32_t r6[9], p6[3]; g2_part_welt(g, &q[6], r6, p6);
                g2_band_t b; g2_band(g, q, r6, p6, &b);
                int32_t ro[9]; g2_prop_matrix(g_scd.props[kk].rot_x, g_scd.props[kk].rot_y, g_scd.props[kk].rot_z, ro);
                const int32_t m[3] = { 1090, -170, -100 };
                const int32_t w[3] = { g_scd.props[kk].x + g2_q12(ro[0], m[0]) + g2_q12(ro[1], m[1]) + g2_q12(ro[2], m[2]),
                                       g_scd.props[kk].y + g2_q12(ro[3], m[0]) + g2_q12(ro[4], m[1]) + g2_q12(ro[5], m[2]),
                                       g_scd.props[kk].z + g2_q12(ro[6], m[0]) + g2_q12(ro[7], m[1]) + g2_q12(ro[8], m[2]) };
                int32_t l[3]; g2_lokal(r6, p6, w, l);
                int32_t ob = 0, un = 0; g2_band_bei(&b, l[0], &ob, &un);
                int unten = 0; int32_t mg = 0; const int nd = g2_draussen(r6, p6, &b, &unten, &mg);
                printf("      MITTE F%d Clip 4 Bild %d: Rumpfmitte Part6 (%d,%d,%d) Band [%d,%d] Leichen-Scheitel draussen %d (unten %d) %s\n", f, (int)g->anim_frame,
                       l[0], l[1], l[2], ob, un, nd, unten, f_kopp >= 0 ? "angehaengt" : "frei");
            }
        }
        /* NB1 (H1 "auch beim Biss ganz im Maul"): auch die Biss-Bilder vor dem Koppelbild ab Clip 4 Bild 7 (dort geht der Kiefer ueber die Leiche) */
        const int biss = f_kopp < 0 && g->motion == 4 && g->anim_frame >= 7 && !re15_gatorszene_r41_leiche_sitz(&a);
        if (!biss && (!re15_gatorszene_r41_leiche_sitz(&a) || a > 40)) continue;      /* angehaengt, nicht im Flug, angekommen (Spur-P-Mass 40) */
        re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
        if (!g2_pose(g, ps)) continue;
        int32_t r6[9], p6[3]; g2_part_welt(g, &ps[6], r6, p6);
        g2_band_t b; g2_band(g, ps, r6, p6, &b);
        int unten = 0; int32_t marge = 0;
        const int n = g2_draussen(r6, p6, &b, &unten, &marge);
        if (n < 0) continue;
        if (biss || (f_kopp >= 0 && f < f_kopp + 26)) { ein_bilder++; if (n > ein_max) ein_max = n; if (getenv("G2_ZEIGE")) printf("      EIN F%d Clip %d Bild %d: %d draussen (unten %d)\n", f, (int)g->motion, (int)g->anim_frame, n, unten);
            if (alt) continue; }   /* NB1: Biss + frueheres Einschwenken zaehlen mit (vor NB1 nur berichtet; Gegenprobe wie bisher) */   /* Einschwenken (26 Bilder ab dem Koppelbild): nur berichtet */
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
static int s_m4_zeige = 0;
/* ---- Riegel NB4 (Abnahme 2 M4): Rollfenster ohne Ausnahme. Gemessen wird JEDES Bild mit der Leiche im Maul (Clip 4 ab Bild 7 bis zum Wurf Clip 5
 *      Bild 57, Fang bis weg), ohne das Spur-P-Mass "angekommen" (a <= 40): Scheitel draussen in A + B fuer die gezeichnete Leiche. Rollfenster = die
 *      Bilder, in denen der Stand vor NB4 (Pruefhaken re15_gatorszene_r41_m4_alt_setzen) Scheitel draussen hat; dort je Bild gegen das erreichbare
 *      Minimum (g2_ab_minimum), sonst 0. ---- */
#define G2_M4_SPIEL 4   /* Nachlauf der Schrittgrenze je Bild (Einpassung hoechstens 2/16 Rollanteil, 0,1 Skala, 80/100 Versatz je Bild) */
typedef struct { int bilder, fenster, max_n, max_min, max_ueber, f_ueber, summe, summe_min, aussen_max, aussen_f, sprung, ruhe_vor_wurf, fang_bilder; int n[2][128]; } g2_m4_t;
static void teil_maul_m4(int elza, int alt, const g2_m4_t *ref, g2_m4_t *o)
{
    memset(o, 0, sizeof *o);
    o->ruhe_vor_wurf = -1;
    re15_gatorszene_r41_m4_alt_setzen(alt);
    if (getenv("G2_KL")) printf("      LAUF m4 alt=%d ref=%d\n", alt, ref != NULL);
    re15_actor_t *g = g2_szene_start(elza);
    int pa = -1; int32_t ps = 0, pd[3] = { 0, 0, 0 };
    for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        const int clip = (int)g->motion, bild = (int)g->anim_frame;
        int32_t a = 0;
        const int sitzt = re15_gatorszene_r41_leiche_sitz(&a);
        const int halt = (clip == 4 && bild >= 7) || (clip == 5 && bild < 57)    /* Halte-Bilder bis zum Wurf */
                         || (clip == 5 && bild >= 74);                             /* NB5 (M1): letztes Flugbild vor dem Fang (75) bis zum Verschlingen (77) */
        (void)sitzt;
        const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
        if (!halt || k < 0 || !g_scd.props[k].active || (clip != 4 && clip != 5) || bild >= 128) continue;
        re15_skel_pose_t ps_[RE15_EMD_MAX_BONES];
        if (!g2_pose(g, ps_)) continue;
        g2_ab_t kk; int na = 0, nb = 0;
        if (!g2_ab_kopf(g, ps_, &kk) || !g2_ab_jetzt(&kk, &na, &nb)) continue;
        const int n = na + nb;
        if (s_kl_bild > s_kl_bild_max) s_kl_bild_max = s_kl_bild;
        o->bilder++;
        if (clip == 5 && bild >= 74) o->fang_bilder++;   /* NB5 (M1) */
        o->n[clip - 4][bild] = n;
        if (n > o->max_n) o->max_n = n;
        o->summe += n;
        if (alt) continue;
        int fa = 0; int32_t fs = 4096, fd[3] = { 0, 0, 0 };
        re15_gatorszene_r41_leiche_fit(&fa, &fs, fd, NULL, NULL, NULL);
        if (pa >= 0 && (abs(fa - pa) > 2 || abs(fs - ps) > 410 || abs(fd[1] - pd[1]) > 80 || abs(fd[2] - pd[2]) > 100)) o->sprung++;
        pa = fa; ps = fs; pd[1] = fd[1]; pd[2] = fd[2];
        if (clip == 5 && bild <= 56) o->ruhe_vor_wurf = (fa == 0 && fs == 4096 && fd[1] == 0 && fd[2] == 0);
        if (ref && ref->n[clip - 4][bild] > 0) {
            o->fenster++;
            double ba = 0, bs = 1, bdy = 0, bdz = 0;
            const int mn = n > 0 ? g2_ab_minimum(g, &kk, &ba, &bs, &bdy, &bdz) : 0;
            o->summe_min += mn;
            if (n - mn > o->max_ueber) { o->max_ueber = n - mn; o->f_ueber = f; }
            if (mn > o->max_min) o->max_min = mn;
            if (s_m4_zeige || getenv("G2_ZEIGE"))
                printf("      M4 F%d Clip %d Bild %d Roll %.1f Grad: draussen %d (A %d, B %d), vorher %d, erreichbar %d (Roll %.3f Skala %.2f dy %.0f dz %.0f); "
                       "Einpassung Roll %d/16 Skala %d dy %d dz %d\n", f, clip, bild, kk.beta * 180 / 3.14159265358979, n, na, nb, ref->n[clip - 4][bild], mn,
                       ba, bs, bdy, bdz, fa, (int)fs, (int)fd[1], (int)fd[2]);
        } else if (n > o->aussen_max) { o->aussen_max = n; o->aussen_f = f; }
    }
    re15_gatorszene_r41_m4_alt_setzen(0);
}
static void teil_maul_riegel(int elza)
{
    printf("== maul%s (Punkt 1) ==\n", elza ? "_elza" : "");
    int n = -1, b = 0, n_alt = -1, b_alt = 0;
    teil_maul(elza, 0, &n, &b);
    teil_maul(elza, 1, &n_alt, &b_alt);
    int n_nb1 = -1, b_nb1 = 0;
    re15_gatorszene_r41_nb1_alt_setzen(1);   /* NB1 (H1): Gegenprobe ohne den Vorgriff - der Kiefer geht im Biss ueber die Leiche */
    teil_maul(elza, 0, &n_nb1, &b_nb1);
    re15_gatorszene_r41_nb1_alt_setzen(0);
    /* NB4 (M4): das Rollfenster misst teil_maul_m4 ohne Ausnahme (Kiefer- UND waagerechter Rahmen); ausserhalb gilt weiter 0 */
    g2_m4_t m4alt, m4, m4nb4;
    teil_maul_m4(elza, 1, NULL, &m4alt);
    s_kl_bild_max = 0; s_kl_weg_max = 0;
    teil_maul_m4(elza, 0, &m4alt, &m4);
    const int kl_max = s_kl_bild_max; const double kl_weg = s_kl_weg_max;
    re15_gatorszene_r41_klemm_alt_setzen(1);   /* NB5 (M1): Gegenprobe Stand NB4 (Einpassung ohne Klemmen je Scheitel) */
    teil_maul_m4(elza, 0, &m4alt, &m4nb4);
    re15_gatorszene_r41_klemm_alt_setzen(0);
    int fang_nb4 = 0, fang = 0, roll_nb4 = 0;
    for (int i = 74; i < 128; i++) { fang_nb4 += m4nb4.n[1][i]; fang += m4.n[1][i]; }
    for (int i = 0; i < 12; i++) roll_nb4 += m4nb4.n[1][i];
    printf("    NB5 (M1) %s: %d Bilder gemessen (Biss Clip 4 Bild 7 .. Wurf Clip 5 Bild 56, Fang Clip 5 Bild 74 .. weg), draussen hoechstens %d (Summe %d, Fangbilder %d); "
           "Stand NB4: hoechstens %d (Summe %d, Clip 5 Bild 0-11 %d, Fangbilder %d); geklemmt je Bild hoechstens %d von %d Scheiteln, groesster Weg %.0f\n", elza ? "Elza" : "Leon", m4.bilder, m4.max_n, m4.summe, fang, m4nb4.max_n, m4nb4.summe, roll_nb4, fang_nb4,
           kl_max, s_ln, kl_weg);
    int fenster_alt = 0;
    for (int c = 0; c < 2; c++) for (int i = 0; i < 128; i++) fenster_alt += m4alt.n[c][i] > 0;
    printf("    NB4 (M4) %s: %d Bilder im Maul, Rollfenster %d Bilder (vorher %d): draussen hoechstens %d (vorher %d), Summe %d (vorher %d, erreichbar %d), "
           "hoechstens %d ueber dem erreichbaren Minimum (F%d; Minimum bis %d); ausserhalb hoechstens %d (F%d); Spruenge %d; Ruhelage vor dem Wurf %d\n",
           elza ? "Elza" : "Leon", m4.bilder, m4.fenster, fenster_alt, m4.max_n, m4alt.max_n, m4.summe, m4alt.summe, m4.summe_min, m4.max_ueber, m4.f_ueber,
           m4.max_min, m4.aussen_max, m4.aussen_f, m4.sprung, m4.ruhe_vor_wurf);
    PRUEF(b >= 34 + 26 + 6 && m4.bilder + 2 >= b, "Leiche im Maul: %d Bilder (Biss Clip 4 Bild 7..12, Koppelbild, Clip 5 Bild 0..56 + 75..76), NB4 ohne Spur-P-Mass gemessen bis zum Wurf: %d (+ Fang 75/76 im Riegel wurf)", b, m4.bilder);
    PRUEF(m4alt.max_n >= 100, "Gegenprobe Stand vor NB4: im Rollfenster bis %d von %d Scheiteln ueber dem Schaedel / unter dem Kiefer (Kiefer- + waagerechter Rahmen) (rot ohne den Bau)", m4alt.max_n, s_ln);
    PRUEF(m4.fenster == fenster_alt && m4.max_ueber <= G2_M4_SPIEL, "Rollfenster ohne Ausnahme: %d von %d Bildern gemessen, je Bild hoechstens %d ueber dem erreichbaren Minimum (%d, Rollanteil x Skala 1..0,6 x Versatz)", m4.fenster, fenster_alt, m4.max_ueber, G2_M4_SPIEL);
    PRUEF(m4.max_n == 0 && m4.bilder - m4.fang_bilder >= 60 && m4.fang_bilder >= 2, "NB5 (M1): in KEINEM Bild von Biss bis Wurf und Fang bis Verschlingen ein Leichen-Scheitel ueber dem Schaedel oder unter dem Unterkiefer (Kiefer- + waagerechter Rahmen, gezeichnete Scheitel): hoechstens %d in %d Bildern (davon %d Fangbilder)", m4.max_n, m4.bilder, m4.fang_bilder);
    PRUEF(m4nb4.max_n > 0 && fang_nb4 > 0, "Gegenprobe Stand NB4 (ohne Klemmen je Scheitel): bis %d Scheitel draussen, Fangbilder %d (rot ohne den Bau)", m4nb4.max_n, fang_nb4);
    PRUEF(m4.aussen_max == 0, "ausserhalb des Rollfensters kein Scheitel ueber dem Schaedel oder unter dem Unterkiefer (Kiefer- + waagerechter Rahmen: %d, F%d)", m4.aussen_max, m4.aussen_f);
    PRUEF(m4.sprung == 0 && m4.ruhe_vor_wurf == 1, "Einpassung glatt (%d Spruenge ueber 2/16 Roll, 0,1 Skala, 80/100 Versatz je Bild) und vor dem Wurf in Ruhelage (%d)", m4.sprung, m4.ruhe_vor_wurf);
    PRUEF(b_alt >= 40 && n_alt >= 50, "Gegenprobe Leiche wie vor G2: %d Scheitel ausserhalb (rot ohne den Bau)", n_alt);
    PRUEF(n_nb1 > 0, "Gegenprobe Stand vor NB1 (ohne Vorgriff): %d Scheitel ausserhalb im Biss (rot ohne den Bau)", n_nb1);
}

/* ---- Riegel NB5 (Abnahme 3, Haken): der Stauch-Haken der Leiche (re15_gatorszene_r41_leiche_skala, pc_prop_world, obj_id 2) gilt nach einem Raumwechsel
 *      nicht weiter. Szene bis zum ersten gestauchten Haltebild, dann der Raumaufbau des naechsten Raums (re15_gatorszene_r41_install wie scd_room_setup.c)
 *      - danach meldet der Haken fuer den Leichen-Slot und jeden Slot nichts mehr. Gegenprobe Pruefhaken _fit_reset_alt_setzen(1): Haken bleibt an. ---- */
static int g2_haken_lauf(int elza, int alt, int *bild, int32_t *sq)
{
    re15_gatorszene_r41_fit_reset_alt_setzen(alt);
    re15_actor_t *g = g2_szene_start(elza);
    int k = -1, an = 0;
    *bild = -1; *sq = 4096;
    for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        k = prop_von(RE15_GSZ_LEICHE_OBJ);
        int32_t s = 4096;
        if (k >= 0 && re15_gatorszene_r41_leiche_skala(k, &s)) { *bild = f; *sq = s; break; }
    }
    if (*bild >= 0) {
        re15_gatorszene_r41_install((uint16_t)(0x2080 + elza));   /* Raumaufbau 2080 (Leiter hinauf), wie scd_room_setup.c */
        for (int i = 0; i < RE15_SCD_MAX_PROPS; i++) { int32_t s = 4096; if (re15_gatorszene_r41_leiche_skala(i, &s)) an = 1; }
    }
    re15_gatorszene_r41_fit_reset_alt_setzen(0);
    return an;
}
static void teil_haken(int elza)
{
    printf("== haken%s (NB5) ==\n", elza ? "_elza" : "");
    int b = -1, b_alt = -1; int32_t s = 0, s_alt = 0;
    const int an = g2_haken_lauf(elza, 0, &b, &s);
    const int an_alt = g2_haken_lauf(elza, 1, &b_alt, &s_alt);
    printf("    gestaucht ab Szenenbild %d (Skala %d/4096); nach dem Raumaufbau 2080 Haken an: %d (Gegenprobe ohne Leeren: Bild %d, Haken an %d)\n", b, (int)s, an, b_alt, an_alt);
    PRUEF(b >= 0 && s < 4096, "Szene erreicht ein gestauchtes Haltebild (Bild %d, Skala %d)", b, (int)s);
    PRUEF(an == 0, "nach dem Raumwechsel staucht der Haken keinen Prop mehr (%d)", an);
    PRUEF(b_alt >= 0 && an_alt == 1, "Gegenprobe ohne Leeren beim Raumaufbau: Haken bleibt an (%d) (rot ohne den Bau)", an_alt);
}

/* NB1 (M2): im Bild der laufenden Einstellung (Kino-Balken 0..24 / 216..240, render_pc.c): Schnauzenspitze (Part-6-x 2500) und Bildkasten der Leiche */
static int g2_wurf_bild(re15_actor_t *g, int *schnauze_y, int *oben, int *unten)
{
    re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
    if (!g2_pose(g, ps)) return 0;
    int32_t r6[9], p6[3]; g2_part_welt(g, &ps[6], r6, p6);
    const int32_t q[3] = { p6[0] + g2_q12(r6[0], 2500), p6[1] + g2_q12(r6[3], 2500), p6[2] + g2_q12(r6[6], 2500) };
    int sx = -1, sy = -1; re15_gr2_im_bild_cut((int)g_scd.cam_id, q, &sx, &sy);
    *schnauze_y = sy; *oben = 999; *unten = -999;
    const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
    if (k < 0 || !g_scd.props[k].active) return 1;
    const __typeof__(g_scd.props[0]) *pr = &g_scd.props[k];
    int32_t ro[9]; g2_prop_matrix(pr->rot_x, pr->rot_y, pr->rot_z, ro);
    for (int i = 0; i < s_ln; i++) {
        const int32_t w[3] = { pr->x + g2_q12(ro[0], s_lv[i][0]) + g2_q12(ro[1], s_lv[i][1]) + g2_q12(ro[2], s_lv[i][2]),
                               pr->y + g2_q12(ro[3], s_lv[i][0]) + g2_q12(ro[4], s_lv[i][1]) + g2_q12(ro[5], s_lv[i][2]),
                               pr->z + g2_q12(ro[6], s_lv[i][0]) + g2_q12(ro[7], s_lv[i][1]) + g2_q12(ro[8], s_lv[i][2]) };
        int x = 0, y = 0;
        if (re15_gr2_im_bild_cut((int)g_scd.cam_id, w, &x, &y)) { if (y < *oben) *oben = y; if (y > *unten) *unten = y; }
    }
    return 1;
}
/* ---- Riegel Punkt 2: hochwerfen, im Maul fangen, dann weg -------------------------------------------------------------------- */
static void teil_wurf(int elza)
{
    printf("== wurf%s (Punkt 2) ==\n", elza ? "_elza" : "");
    for (int alt = 0; alt <= 2; alt++) {
        re15_gatorszene_r41_g2_alt_setzen(alt == 1);
        re15_gatorszene_r41_nb1_alt_setzen(alt == 2);
        re15_actor_t *g = g2_szene_start(elza);
        int32_t y_vor = 0; int steigt = 0, faellt = 0, sicht_nach_weg = 0, key_nach_weg = 0, weg_f = -1;
        int schnauze_weg = 0, leiche_weg_oben = 0, flug_bilder = 0, abstand_max = -999;   /* NB1 (M2): Bildmasse vom Wurf bis zum Verschlingen */
        for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
            frame(0, 0);
            int bw = -1, bf = -1, bweg = -1; int32_t h = 0, fa = -1;
            const int ph = re15_gatorszene_r41_leiche_flug(&bw, &bf, &bweg, &h, &fa);
            const int k = prop_von(RE15_GSZ_LEICHE_OBJ), kk = prop_von(RE15_GSZ_KEY_OBJ);
            if (ph == 1 || ph == 2) {   /* Flug und Fang bis zum Verschlingen */
                int zy = 0, lo = 0, lu = 0;
                if (g2_wurf_bild(g, &zy, &lo, &lu)) {
                    flug_bilder++;
                    if (zy < 24) schnauze_weg++;
                    if (lo < 24) leiche_weg_oben++;
                    if (lu > -999 && zy - lu > abstand_max) abstand_max = zy - lu;
                }
            }
            if (ph == 1 && k >= 0) {
                if (g_scd.props[k].y < y_vor) steigt++; else if (g_scd.props[k].y > y_vor) faellt++;
                y_vor = g_scd.props[k].y;
            } else if (k >= 0) y_vor = g_scd.props[k].y;
            if (ph == 3 && weg_f < 0) weg_f = f;
            if (weg_f >= 0) { if (k >= 0 && g_scd.props[k].active) sicht_nach_weg++; if (kk >= 0 && g_scd.props[kk].active) key_nach_weg++; }
        }
        int bw = -1, bf = -1, bweg = -1; int32_t h = 0, fa = -1;
        const int ph = re15_gatorszene_r41_leiche_flug(&bw, &bf, &bweg, &h, &fa);
        re15_gatorszene_r41_g2_alt_setzen(0); re15_gatorszene_r41_nb1_alt_setzen(0);
        printf("    Bild (Cut %d): %d Bilder Flug/Fang, Schnauze hinter dem oberen Balken in %d, Leiche oben angeschnitten in %d, groesster Abstand Leichen-Unterkante ueber der Schnauze %d\n",
               (int)g_scd.cam_id, flug_bilder, schnauze_weg, leiche_weg_oben, abstand_max);
        if (alt == 2) {
            PRUEF(schnauze_weg > 0, "Gegenprobe Stand vor NB1: Schnauze in %d Flugbildern hinter dem Balken (rot ohne den Bau)", schnauze_weg);
            continue;
        }
        printf("    %s%s: Zustand %d, Wurf Clip-5-Bild %d, Fang %d, weg %d; hoechster Punkt der Rumpfmitte %d ueber der Maullage; im Fangbild %d vom Maul; "
               "Flugbilder steigend %d / fallend %d; nach dem Verschlingen sichtbar: Leiche %d, Schluessel %d Bilder\n", elza ? "Elza" : "Leon",
               alt ? " [Gegenprobe Leiche wie vor G2]" : "", ph, bw, bf, bweg, (int)h, (int)fa, steigt, faellt, sicht_nach_weg, key_nach_weg);
        if (!alt) {
            PRUEF(ph == 3 && bw == 57 && bf == 75 && bweg == 77, "Wurf beim Hochreissen (Clip 5 Bild %d), Fang beim Zuschnappen (%d), weg mit dem geschlossenen Maul (%d)", bw, bf, bweg);
            PRUEF(h >= 405 && steigt >= 3 && faellt >= 3, "hochgeworfen: Rumpfmitte %d ueber der Maullage (>= H/2 = a T^2 / 16 = 405), %d Bilder steigend, %d fallend", (int)h, steigt, faellt);
            PRUEF(fa >= 0 && fa <= 40, "im Maul gefangen: Rumpfmitte %d von der Maullage", (int)fa);
            PRUEF(weg_f >= 0 && sicht_nach_weg == 0 && key_nach_weg == 0, "danach verschwunden (Leiche %d, Schluessel %d Bilder sichtbar)", sicht_nach_weg, key_nach_weg);
            PRUEF(flug_bilder >= 18 && schnauze_weg == 0 && leiche_weg_oben == 0, "NB1 (M2) lesbar im Bild: Schnauze (%d) und Leiche (%d) in keinem der %d Flug-/Fangbilder hinter dem oberen Balken", schnauze_weg, leiche_weg_oben, flug_bilder);
            PRUEF(abstand_max > 0, "NB1 (M2): die Leiche fliegt ganz ueber die Schnauze (Unterkante %d Bildpunkte darueber)", abstand_max);
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
            int ly0 = 999, ly1 = -999, lx0 = 999, lx1 = -999, zy = -1, zx = -1;
            if (k >= 0 && g_scd.props[k].active) {   /* NB1 (M2): Bildkasten der Leiche */
                const __typeof__(g_scd.props[0]) *pr = &g_scd.props[k];
                int32_t ro[9]; g2_prop_matrix(pr->rot_x, pr->rot_y, pr->rot_z, ro);
                for (int i = 0; i < s_ln; i++) {
                    const int32_t w[3] = { pr->x + g2_q12(ro[0], s_lv[i][0]) + g2_q12(ro[1], s_lv[i][1]) + g2_q12(ro[2], s_lv[i][2]),
                                           pr->y + g2_q12(ro[3], s_lv[i][0]) + g2_q12(ro[4], s_lv[i][1]) + g2_q12(ro[5], s_lv[i][2]),
                                           pr->z + g2_q12(ro[6], s_lv[i][0]) + g2_q12(ro[7], s_lv[i][1]) + g2_q12(ro[8], s_lv[i][2]) };
                    int qx = 0, qy = 0;
                    if (re15_gr2_im_bild_cut((int)g_scd.cam_id, w, &qx, &qy)) { if (qy < ly0) ly0 = qy; if (qy > ly1) ly1 = qy; if (qx < lx0) lx0 = qx; if (qx > lx1) lx1 = qx; }
                }
            }
            if (hp) {   /* Schnauzenspitze Part-6-x 2500 */
                int32_t r6[9], p6[3]; g2_part_welt(g, &ps[6], r6, p6);
                const int32_t q[3] = { p6[0] + g2_q12(r6[0], 2500), p6[1] + g2_q12(r6[3], 2500), p6[2] + g2_q12(r6[6], 2500) };
                re15_gr2_im_bild_cut((int)g_scd.cam_id, q, &zx, &zy);
            }
            printf("      F%d Bild %d Cut %d: Leiche-Ursprung Bild (%d,%d) aktiv %d, Kopf Bild (%d,%d), Leiche Kasten x %d..%d y %d..%d, Schnauze (%d,%d)\n", f, (int)g->anim_frame,
                   (int)g_scd.cam_id, sx, sy, k >= 0 ? g_scd.props[k].active : -1, kx, ky, lx0, lx1, ly0, ly1, zx, zy);
        }
    }
    printf("wurfkamera %s: %d Bilder (Clip 5 Bild 55..77), laufender Cut %d\n", elza ? "Elza" : "Leon", bilder, (int)g_scd.cam_id);
    for (int c = 0; c < 8; c++)
        printf("  Cut %d: Leichen-Scheitel im Bild %d / %d (%d %%), Kopf im Bild %d / %d, Kopf-Scheitel (Part 6/7) im Bild %d / %d (%d %%), Kopf-Kasten (%d,%d)-(%d,%d)\n", c, im[c], alle[c],
               alle[c] ? 100 * im[c] / alle[c] : 0, kopf[c], bilder, s_kopf_im[c], s_kopf_alle[c], s_kopf_alle[c] ? 100 * s_kopf_im[c] / s_kopf_alle[c] : 0,
               s_bx0[c], s_by0[c], s_bx1[c], s_by1[c]);
}


/* ---- Werkzeug kopfwinkel: Keyframe-Winkel (ay/az) der Bones 0..7 in Clip 5 und Neigung der Part-6-Achse (Welt) je Bild ---- */
static int16_t s_kw_ay[16], s_kw_az[16];
static void kw_haken(const void *a, int b, int16_t *ay, int16_t *az) { (void)a; if (b >= 0 && b < 16) { s_kw_ay[b] = *ay; s_kw_az[b] = *az; } }
static void kopfwinkel(int elza)
{
    re15_actor_t *g = g2_szene_start(elza);
    for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        if (!(g->motion == 5 && g->anim_frame >= 50 && g->anim_frame <= 130)) continue;
        void (*alt)(const void *, int, int16_t *, int16_t *) = g_anim_bone_angle_hook;
        g_anim_bone_angle_hook = kw_haken;
        re15_skel_pose_t ps[RE15_EMD_MAX_BONES]; const int hp = g2_pose(g, ps);
        g_anim_bone_angle_hook = alt;
        if (!hp) continue;
        int32_t r6[9], p6[3]; g2_part_welt(g, &ps[6], r6, p6);
        int sx = -1, sy = -1; const int32_t q[3] = { p6[0] + g2_q12(r6[0], 2500), p6[1] + g2_q12(r6[3], 2500), p6[2] + g2_q12(r6[6], 2500) };
        re15_gr2_im_bild_cut((int)g_scd.cam_id, q, &sx, &sy);
        printf("    F%d Bild %d Cut %d: az", f, (int)g->anim_frame, (int)g_scd.cam_id);
        for (int b = 0; b < 8; b++) printf(" %d", (int)s_kw_az[b]);
        printf(" | ay"); for (int b = 0; b < 8; b++) printf(" %d", (int)s_kw_ay[b]);
        printf(" | Part6-x welt (%d,%d,%d) Schnauze Bild (%d,%d)\n", r6[0], r6[3], r6[6], sx, sy);
    }
}

/* ---- Werkzeug kletterkarte <raum hex>: Zellen mit Kletter-Attribut (Bodentypwort Bit 0, climb_common.c @0x8002d530-50) je Band ---- */
#include "re15_collision.h"
static int kletterkarte(const char *raum_s)
{
    const uint16_t raum = (uint16_t)strtoul(raum_s, NULL, 16);
    if (rdt_laden(raum) != 0) { printf("ROOM%04X nicht ladbar\n", raum); return 1; }
    g_room_rdt_ok = 1;
    for (int band = 0; band < 4; band++) {
        int n = 0;
        for (int32_t z = -27000; z <= -5400; z += 200)
            for (int32_t x = -9000; x <= 7200; x += 200) {
                const uint16_t tw = re15_collision_floor_typeword(&g_room_rdt, x, z, band, 0);
                if (tw & 1u) { if (n < 60) printf("  Band %d Zelle (%d,%d) Typwort 0x%04X\n", band, x, z, tw); n++; }
            }
        printf("ROOM%04X Band %d: %d Zellen mit Bit 0\n", raum, band, n);
    }
    return 0;
}

/* ---- Auswerter traglog <laufordner> (exe-Riegel integration_r41_gatorfix_frei*, Punkt 7 / NB1 M1+M3, E1): freies Spiel mit dem Kanister im Maul.
 * Masse wie die Abnahme 1 (still_state.py): je Bild der letzten 2090-Folge in state.log Wurzel/Gier/Clip des Alligators (Slot 35) und die Figur;
 * OHNE FORTSCHRITT = Wurzel kommt in 30 Bildern netto < 300 voran; NB2 (Abnahme 2 M5): KEINE Ausnahme (auch nicht Dash-Clips oder Kopf nahe der Figur) ausser
 * einem laufenden Biss (Phase 3/8) am Spieler (Wurzel < 3200 oder Kopf < 2400). WANDREIBEN = ohne Fortschritt, waehrend die Kopf-Klemme
 * greift (gator_boss.log "KOPF-KLEMME an/aus"), fern und ausserhalb der Biss-Reichweite; Spanne der Klemm-Bilder je Stillstands-Folge. E1: Bilder vom Aufschlag (debug.log "Kanister im Becken") bis "KANISTER angehaengt". */
typedef struct { int f; int32_t px, pz, gx, gz; int rot, mo, cam; } g2_tl_t;
static g2_tl_t s_tl[20000];
static uint8_t s_tl_kk[40000];
static int traglog(const char *dir)
{
    char pf[1024]; static char z[16384];
    int auf = -1, land = -1, n = 0;
    snprintf(pf, sizeof pf, "%s/gator_boss.log", dir);
    FILE *f = fopen(pf, "r");
    if (!f) { printf("traglog: %s fehlt\n", pf); return 1; }
    memset(s_tl_kk, 0, sizeof s_tl_kk);
    while (fgets(z, sizeof z, f)) {
        const char *k = strstr(z, "KANISTER angehaengt");
        if (k && auf < 0) { const char *q = strrchr(z, 'F'); if (q) auf = atoi(q + 1); }
        unsigned a = 0, b0 = 0; int nb = 0;
        const char *kk = strstr(z, "KOPF-KLEMME aus F");
        if (kk && sscanf(kk, "KOPF-KLEMME aus F%u: %d Bilder ab F%u", &a, &nb, &b0) == 3)
            for (unsigned x = b0; x < a && x < sizeof s_tl_kk; x++) s_tl_kk[x] = 1;
    }
    fclose(f);
    snprintf(pf, sizeof pf, "%s/debug.log", dir);
    if ((f = fopen(pf, "r")) != NULL) {
        while (fgets(z, sizeof z, f)) {
            const char *k = strstr(z, "Kanister im Becken");
            const char *q = strstr(z, "[k2080] F");
            if (k && q) land = atoi(q + 9);                              /* letzte Zeile = Rueckkehr-Besuch */
        }
        fclose(f);
    }
    snprintf(pf, sizeof pf, "%s/state.log", dir);
    if (!(f = fopen(pf, "r"))) { printf("traglog: %s fehlt\n", pf); return 1; }
    while (fgets(z, sizeof z, f)) {
        int fr = -1;
        if (sscanf(z, "F%d ", &fr) != 1) continue;
        if (n > 0 && fr < s_tl[n-1].f) n = 0;                            /* neue Folge (Raumwechsel): nur die letzte zaehlt */
        const char *pl = strstr(z, " PL("), *g = strstr(z, "[35 t=23 ");
        if (!pl || !g || n >= (int)(sizeof s_tl / sizeof s_tl[0])) continue;
        g2_tl_t r; r.f = fr;
        if (sscanf(pl, " PL(%d,%d,", &r.px, &r.pz) != 2) continue;
        const char *m = strstr(g, " mo="), *at = strstr(g, "@(");
        const char *ende = strchr(g, ']');
        if (!m || !at || !ende || m > ende || at > ende || sscanf(m, " mo=%d", &r.mo) != 1 || sscanf(at, "@(%d,%d,r%d", &r.gx, &r.gz, &r.rot) != 3) continue;
        { const char *c = strstr(z, " cam="); r.cam = c ? atoi(c + 5) : -1; }
        s_tl[n++] = r;
    }
    fclose(f);
    /* NB2 (M5 der Abnahme 2: "Fortschritt messen, keine Ausnahmen ... ausser bei gerade laufendem Biss am Spieler"): Phase je Bild aus gator_trag.log */
    static int8_t ph_von[40000];
    memset(ph_von, -1, sizeof ph_von);
    snprintf(pf, sizeof pf, "%s/gator_trag.log", dir);
    if ((f = fopen(pf, "r")) != NULL) {
        while (fgets(z, sizeof z, f)) { int fr = -1, ph = -1; if (sscanf(z, "TRAG F%d ph=%d", &fr, &ph) == 2 && fr >= 0 && fr < 40000) ph_von[fr] = (int8_t)ph; }
        fclose(f);
    }
    int trag = 0, still = 0, biss = 0, ep = 0, ep_max = 0, ep_f = -1, rub = -1, rub_letzt = -1, rub_max = 0, rub_f = -1;
    static uint8_t st[20000];
    memset(st, 0, sizeof st);
    for (int i = 0; i + 30 < n; i++) {
        if (s_tl[i+30].f - s_tl[i].f != 30) continue;
        const int64_t dx = (int64_t)s_tl[i+30].gx - s_tl[i].gx, dz = (int64_t)s_tl[i+30].gz - s_tl[i].gz;
        if (dx * dx + dz * dz < 300LL * 300) for (int k = i; k <= i + 30; k++) st[k] = 1;
    }
    /* Orchestrator NB2: "kein Zeitfenster > 2 s ohne Fortschritt" - Fenster 60 Bilder, Weg der Wurzel < 300, ausser Fenster mit Biss am Spieler */
    static uint8_t st2[20000];
    memset(st2, 0, sizeof st2);
    for (int i = 0; i + 60 < n; i++) {                     /* Fortschritt = die Wurzel entfernt sich im Fenster >= 300 vom Fensteranfang */
        if (s_tl[i+60].f - s_tl[i].f != 60 || auf < 0 || s_tl[i].f < auf) continue;
        double w = 0.0;                                    /* weitester Abstand vom Fensteranfang: Zappeln/Wandreiben/Wende auf der Stelle bleibt darunter, Kreisen nicht */
        for (int k = i + 1; k <= i + 60; k++) { const double ax = (double)s_tl[k].gx - s_tl[i].gx, az = (double)s_tl[k].gz - s_tl[i].gz; const double d = sqrt(ax * ax + az * az); if (d > w) w = d; }
        if (w < 300.0) st2[i] = 1;
    }
    int fenster2 = 0, fenster2_f = -1;
    static uint8_t bs[20000];
    memset(bs, 0, sizeof bs);
    for (int i = 0; i < n; i++) {
        const g2_tl_t *r = &s_tl[i];
        if (auf < 0 || r->f < auf) continue;
        trag++;
        const double th = r->rot * 2.0 * 3.14159265358979 / 4096.0;
        const double hx = r->gx + 4200.0 * cos(th) - r->px, hz = r->gz - 4200.0 * sin(th) - r->pz;
        const double wx = (double)r->gx - r->px, wz = (double)r->gz - r->pz;
        const int oben = (r->px > -1700 && r->px < 1850 && r->pz > -20000 && r->pz < -12450) || (r->px > 1850 && r->px < 7200 && r->pz > -18100 && r->pz < -14500);
        const int ph = (r->f >= 0 && r->f < 40000) ? ph_von[r->f] : -1;
        /* einzige Ausnahme: laufender Biss (Phase 3 LUNGE / 8 FRESSEN) am Spieler im Wasser (Wurzel < 3200 = Spur-B-Bisstor oder Kopf < 2400 = Maul-Tor) */
        /* auch der Hochbiss der Belagerung (Phase 5, Biss-Clip 4) an der Figur oben: Kopf unter dem 4200er-Aufricht-Gate der Spur B (enemy_ai_boss_gator.c GUARD) */
        const int am_spieler = ((ph == 3 || ph == 8) && (sqrt(wx * wx + wz * wz) < 3200.0 || sqrt(hx * hx + hz * hz) < 2400.0))
                             || (ph == 5 && r->mo == 4 && oben && sqrt(hx * hx + hz * hz) < 4200.0);
        const int ohne = st[i] && !am_spieler;
        if (st[i] && am_spieler) biss++;
        bs[i] = (uint8_t)am_spieler;
        if (ohne) { still++; if (++ep > ep_max) { ep_max = ep; ep_f = r->f; } } else ep = 0;
        /* Wandreiben: in einer Folge ohne Fortschritt die Spanne der Bilder mit greifender Kopf-Klemme (Luecken <= 26 = ein Paddel-Zyklus) */
        const int kk = r->f >= 0 && r->f < (int)sizeof s_tl_kk && s_tl_kk[r->f];
        int ohne_weg = 0;                                      /* NB2: Wandreiben nur ohne Weg (2-s-Fenster ab diesem oder einem der 60 Bilder davor) */
        for (int k = i; k >= 0 && k > i - 60; k--) if (st2[k]) ohne_weg = 1;
        if (!ohne || !ohne_weg) rub = -1;
        else if (kk) {
            if (rub < 0 || r->f - rub_letzt > 26) rub = r->f;
            rub_letzt = r->f;
            if (rub_letzt - rub + 1 > rub_max) { rub_max = rub_letzt - rub + 1; rub_f = r->f; }
        }
    }
    for (int i = 0; i + 60 < n; i++) {                     /* 2-s-Fenster ohne Fortschritt; ausgenommen nur Fenster mit einem Biss am Spieler darin */
        if (!st2[i]) continue;
        int b = 0;
        for (int k = i; k <= i + 60 && k < n; k++) if (bs[k]) b = 1;
        if (!b) { fenster2++; if (fenster2_f < 0) fenster2_f = s_tl[i].f; }
    }
    /* NB3 (M3'): Figur oben (Plattform/Rampe) - mittlerer Kopf-Abstand und Anteil der Bilder, in denen die Wurzel des Alligators im Bild der laufenden Kamera
     * (state.log cam=, Kino-Balken 24..216) liegt */
    int oben_n = 0, oben_bild = 0; int64_t oben_kopf = 0;
    if (rdt_laden((uint16_t)(strstr(dir, "elza") ? 0x2091 : 0x2090)) == 0) {
        g_room_rdt_ok = 1;
        for (int i = 0; i < n; i++) {
            const g2_tl_t *r = &s_tl[i];
            if (auf < 0 || r->f < auf) continue;
            const int oben = (r->px > -1700 && r->px < 1850 && r->pz > -20000 && r->pz < -12450) || (r->px > 1850 && r->px < 7200 && r->pz > -18100 && r->pz < -14500);
            if (!oben) continue;
            oben_n++;
            const double th = r->rot * 2.0 * 3.14159265358979 / 4096.0;
            const double hx = r->gx + 4200.0 * cos(th) - r->px, hz = r->gz - 4200.0 * sin(th) - r->pz;
            oben_kopf += (int64_t)sqrt(hx * hx + hz * hz);
            const int32_t w[3] = { r->gx, -1200, r->gz }; int sx = 0, sy = 0;
            if (r->cam >= 0 && re15_gr2_im_bild_cut(r->cam, w, &sx, &sy) && sy >= 24 && sy < 216) oben_bild++;
        }
    }
    const int auf_bilder = (land >= 0 && auf >= 0) ? auf - land : -1;
    printf("traglog: %d Bilder 2090, Aufschlag F%d, angehaengt F%d (%d Bilder = %.1f s), %d Trag-Bilder: ohne Fortschritt %d (%d %%), davon keine Ausnahme ausser Biss am Spieler "
           "(%d Bilder), laengste Folge ohne Fortschritt %d Bilder (bis F%d), laengstes Wandreiben %d Bilder (bis F%d), 2-s-Fenster ohne Fortschritt %d (erstes F%d); "
           "Figur oben %d Bilder, Kopf-Abstand mittel %d, Gator im Bild der Spielerkamera %d %%\n", n, land, auf, auf_bilder,
           auf_bilder / 30.0, trag, still, trag ? 100 * still / trag : 0, biss, ep_max, ep_f, rub_max, rub_f, fenster2, fenster2_f,
           oben_n, oben_n ? (int)(oben_kopf / oben_n) : -1, oben_n ? 100 * oben_bild / oben_n : 0);
    return trag > 0 ? 0 : 1;
}

/* ---- Werkzeug leichenachse (NB3 M4): je Bild Clip 4/5 die Welt-Richtung der Part-6-Achsen (x = Kiefer, z = quer = Leichen-Laengsachse) und die Bild-y von
 *      Leichen-Enden, Schaedel-Oberkante (Part 7) und Kiefer im Bild der laufenden Kamera ---- */
static void leichenachse(int elza)
{
    re15_actor_t *g = g2_szene_start(elza);
    re15_enemy_bank_t *gb = re15_enemy_find(0x23u);
    const int32_t sk = g && g->render_scale_q12 ? g->render_scale_q12 : 4096;
    static int32_t pv[4096][3];
    for (int f = 0; g && f < 2000 && re15_gatorszene_r41_zustand() != RE15_GSZ_KAMPF; f++) {
        frame(0, 0);
        if (!((g->motion == 4 && g->anim_frame >= 10) || (g->motion == 5 && g->anim_frame <= 30))) continue;
        re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
        if (!g2_pose(g, ps)) continue;
        int32_t r6[9], p6[3], r7[9], p7[3]; g2_part_welt(g, &ps[6], r6, p6); g2_part_welt(g, &ps[7], r7, p7);
        int sy7 = 999, sy6 = 999, ly0 = 999, ly1 = -999;
        for (int part = 6; part <= 7; part++) {   /* hoechster Kopf-Punkt im Bild */
            int32_t rb[9], pb[3]; g2_part_welt(g, &ps[part], rb, pb);
            const int nv = g2_scheitel(&gb->md1, part, pv, 4096);
            for (int i = 0; i < nv; i++) {
                const int32_t v[3] = { g2_q12(pv[i][0], sk), g2_q12(pv[i][1], sk), g2_q12(pv[i][2], sk) };
                const int32_t w[3] = { pb[0] + g2_q12(rb[0], v[0]) + g2_q12(rb[1], v[1]) + g2_q12(rb[2], v[2]),
                                       pb[1] + g2_q12(rb[3], v[0]) + g2_q12(rb[4], v[1]) + g2_q12(rb[5], v[2]),
                                       pb[2] + g2_q12(rb[6], v[0]) + g2_q12(rb[7], v[1]) + g2_q12(rb[8], v[2]) };
                int sx = 0, sy = 0;
                if (re15_gr2_im_bild_cut((int)g_scd.cam_id, w, &sx, &sy)) { if (part == 7 && sy < sy7) sy7 = sy; if (part == 6 && sy < sy6) sy6 = sy; }
            }
        }
        const int k = prop_von(RE15_GSZ_LEICHE_OBJ);
        if (k >= 0 && g_scd.props[k].active) {
            const __typeof__(g_scd.props[0]) *pr = &g_scd.props[k];
            int32_t ro[9]; g2_prop_matrix(pr->rot_x, pr->rot_y, pr->rot_z, ro);
            for (int i = 0; i < s_ln; i++) {
                const int32_t w[3] = { pr->x + g2_q12(ro[0], s_lv[i][0]) + g2_q12(ro[1], s_lv[i][1]) + g2_q12(ro[2], s_lv[i][2]),
                                       pr->y + g2_q12(ro[3], s_lv[i][0]) + g2_q12(ro[4], s_lv[i][1]) + g2_q12(ro[5], s_lv[i][2]),
                                       pr->z + g2_q12(ro[6], s_lv[i][0]) + g2_q12(ro[7], s_lv[i][1]) + g2_q12(ro[8], s_lv[i][2]) };
                int sx = 0, sy = 0;
                if (re15_gr2_im_bild_cut((int)g_scd.cam_id, w, &sx, &sy)) { if (sy < ly0) ly0 = sy; if (sy > ly1) ly1 = sy; }
            }
        }
        printf("    F%d Clip %d Bild %d Cut %d: Part6-x Welt (%d,%d,%d) Part6-z Welt (%d,%d,%d); Bild-y Schaedel-Oberkante %d, Unterkiefer-Oberkante %d, Leiche %d..%d\n",
               f, (int)g->motion, (int)g->anim_frame, (int)g_scd.cam_id, r6[0], r6[3], r6[6], r6[2], r6[5], r6[8], sy7, sy6, ly0, ly1);
    }
}
/* ---- Werkzeug leichenfit [elza] (NB4 M4): je Bild im Rollfenster Scheitel draussen (Kiefer- + waagerechter Rahmen) der Einpassung, des Stands vor NB4 und
 *      das erreichbare Minimum mit seinen Werten (Zeilen "M4"), wie der Riegel maul ---- */
static void leichenfit(int elza)
{
    g2_m4_t a, n;
    s_m4_zeige = 1;
    teil_maul_m4(elza, 1, NULL, &a);
    teil_maul_m4(elza, 0, &a, &n);
    s_m4_zeige = 0;
    printf("leichenfit %s: vorher hoechstens %d (Summe %d), Einpassung hoechstens %d (Summe %d, erreichbar %d), hoechstens %d ueber dem Minimum, ausserhalb %d\n",
           elza ? "Elza" : "Leon", a.max_n, a.summe, n.max_n, n.summe, n.summe_min, n.max_ueber, n.aussen_max);
}
/* ---- Werkzeug kamerazone <raum hex> [band] x,z ... (NB4 Punkt 2): RVD-Zonen des Raums (Cut von -> nach, Viereck, Stockwerk) und je Lage die Kamera,
 *      die der Zonen-Scan (aot_common.c, FUN_80014230: Zone mit cam_from == laufender Cut und Lage im Viereck -> cam_to, erste in Tabellenfolge) von
 *      jedem Start-Cut aus erreicht (Fixpunkt) ---- */
static int g2_zonen_fix(int32_t x, int32_t z, int band, int start)
{
    int c = start;
    for (int it = 0; it < 16; it++) {
        int nach = -1;
        for (int i = 0; i < g_room_rdt.zone_count && nach < 0; i++) {
            const re15_rdt_zone_t *zo = &g_room_rdt.zones[i];
            if (i == 0 || g_room_rdt.zones[i - 1].cam_from != zo->cam_from) continue;   /* Anker der Gruppe (rdt_common.c re15_rdt_apply_zones_as_aots, FUN_80014230) */
            if (zo->cam_from != c) continue;
            if (zo->floor != 0xFF && band >= 0 && zo->floor != band) continue;
            if (re15_aot_point_in_quad(x, z, zo->xs, zo->zs)) nach = zo->cam_to;
        }
        if (nach < 0 || nach == c) return c;
        c = nach;
    }
    return -1;   /* Pendel */
}
static int kamerazone(int argc, char **argv)
{
    const uint16_t raum = (uint16_t)strtoul(argv[2], NULL, 16);
    if (rdt_laden(raum) != 0) { printf("ROOM%04X nicht ladbar\n", raum); return 1; }
    g_room_rdt_ok = 1;
    printf("ROOM%04X: %d Cuts, %d RVD-Zonen\n", raum, g_room_rdt.cut_count, g_room_rdt.zone_count);
    for (int i = 0; i < g_room_rdt.zone_count; i++) {
        const re15_rdt_zone_t *zo = &g_room_rdt.zones[i];
        printf("  Zone %2d: Cut %d -> %d Stockwerk %3d (%d,%d) (%d,%d) (%d,%d) (%d,%d)\n", i, zo->cam_from, zo->cam_to, zo->floor,
               zo->xs[0], zo->zs[0], zo->xs[1], zo->zs[1], zo->xs[2], zo->zs[2], zo->xs[3], zo->zs[3]);
    }
    int band = -1, a0 = 3;
    if (argc > 3 && strchr(argv[3], ',') == NULL) { band = atoi(argv[3]); a0 = 4; }
    for (int i = a0; i < argc; i++) {
        int32_t x = 0, z = 0;
        if (sscanf(argv[i], "%d,%d", &x, &z) != 2) continue;
        printf("  Lage (%d,%d) Band %d: Fixpunkt je Start-Cut", x, z, band);
        for (int c = 0; c < g_room_rdt.cut_count; c++) printf(" %d->%d", c, g2_zonen_fix(x, z, band, c));
        printf("\n");
    }
    return 0;
}
/* ---- Werkzeug begehbar <raum hex> <band> x,z ... (NB4 Punkt 2): steht die Figur dort auf diesem Kollisionsband, oder schiebt die Spieler-Kollision
 *      (re15_collision_constrain = FUN_8003b0a4) sie weg? ---- */
static int begehbar(int argc, char **argv)
{
    const uint16_t raum = (uint16_t)strtoul(argv[2], NULL, 16);
    if (rdt_laden(raum) != 0) { printf("ROOM%04X nicht ladbar\n", raum); return 1; }
    g_room_rdt_ok = 1;
    re15_collision_set_band(atoi(argv[3]));
    for (int i = 4; i < argc; i++) {
        int32_t x = 0, z = 0;
        if (sscanf(argv[i], "%d,%d", &x, &z) != 2) continue;
        int32_t nx = x, nz = z;
        for (int k = 0; k < 8; k++) re15_collision_constrain(&g_room_rdt, nx, nz, &nx, &nz);
        printf("ROOM%04X Band %s (%d,%d) -> (%d,%d) %s\n", raum, argv[3], x, z, nx, nz, (nx == x && nz == z) ? "begehbar" : "weggeschoben");
    }
    return 0;
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
static int teil_marvin_lauf(int elza, int alt)   /* Rueckgabe: Bild des ersten Todes (NB1 H2) */
{
    re15_k2080_lauf_alt_setzen(alt);
    const uint16_t r2090 = (uint16_t)(0x2090 + elza), r2080 = (uint16_t)(0x2080 + elza);
    re15_k2080_reset();
    re15_actor_t *g = szene_bis_kampf(elza);
    if (!g) { PRUEF(0, "Szene bis zum Kampf"); re15_k2080_lauf_alt_setzen(0); return -1; }
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
    if (!g_room_change.pending || (g_room_change.room_id & 0xFFFEu) != 0x2080u) { PRUEF(0, "Ausloeser -> Raumwechsel nach 2080 (Phase %d)", re15_k2080_phase()); re15_k2080_lauf_alt_setzen(0); return -1; }
    if (!g2_zombie_bank()) { PRUEF(0, "Zombie-Bank 0x16 (RE2 CDEMD0.EMS)"); re15_k2080_lauf_alt_setzen(0); return -1; }
    if (room_boot(r2080, g_room_change.x, g_room_change.y, g_room_change.z, g_room_change.yaw_4096, g_room_change.target_cut) != 0) { re15_k2080_lauf_alt_setzen(0); return -1; }
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
    int erster = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (ab0[i] && tod_bild[i] >= 0 && (erster < 0 || tod_bild[i] < erster)) erster = tod_bild[i];
    re15_k2080_lauf_alt_setzen(0);
    printf("    %s: erster Tod Bild %d der Szene\n", alt ? "Gegenprobe ohne Laufzeit (Stand vor NB1)" : "NB1", erster);
    if (alt) return erster;
    PRUEF(erster >= 30, "jeder Zombie laeuft >= 30 Bilder (rund 1 s), bevor er erschossen wird (erster Tod Bild %d; Abnahme 1 H2: 19)", erster);
    PRUEF(n == 3 && tot == 3, "drei Zombies in der Szene, alle erschossen (%d / %d; vorher 8)", n, tot);
    PRUEF(blick_max <= 2, "Blick zur Leiter statt zu Marvin/John (Abweichung hoechstens %d/4096)", blick_max);
    PRUEF(rest_min >= 2000, "beim Tod noch >= 2000 von der Leiter (%d) - den Gang misst integration_r41_gatorfix_gang an der exe", (int)rest_min);
    PRUEF(letzter >= 0 && letzter <= 90, "alle erschossen nach %d Bildern (<= 90 = Laufen 20 + Anlegen 10 + 3 x Feuern 16 + Nachdrehen; vorher 351 mit 8 Zombies)", letzter);
    PRUEF(bgm_kampf == 0xFF3C && bgm_szene == 0xFF3C && bgm_max_abw == 0 && bgm_rueck == 0xFF3C, "Kampfmusik (Eintrag 0xFF3C) durch die ganze Szene und zurueck");
    PRUEF(bgm_frei < 0, "Gegenprobe: 2080 ohne Szene spielt seine Raum-Musik (Eintrag %d = Tabelle)", bgm_frei);
    return erster;
}
static void teil_marvin(int elza)
{
    printf("== marvin%s (Punkte 4 + 5, NB1 H2) ==\n", elza ? "_elza" : "");
    (void)teil_marvin_lauf(elza, 0);
    const int e_alt = teil_marvin_lauf(elza, 1);
    PRUEF(e_alt >= 0 && e_alt < 30, "Gegenprobe ohne die Laufzeit: erster Tod Bild %d < 30 (rot ohne den Bau)", e_alt);
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
    int32_t ab0[64], ab1[64], lx[64], lz[64]; int da[64], tot[64], b0[64], b1[64], n_bilder = 0; int64_t weg[64];
    for (int i = 0; i < 64; i++) { ab0[i] = ab1[i] = -1; da[i] = 0; tot[i] = 0; b0[i] = b1[i] = -1; weg[i] = 0; lx[i] = lz[i] = 0; }
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
            if (ab0[slot] < 0) { ab0[slot] = ab; b0[slot] = n_bilder; lx[slot] = x; lz[slot] = zz; }
            da[slot] = 1;
            if (!tot[slot]) { const int64_t wx = (int64_t)x - lx[slot], wz = (int64_t)zz - lz[slot]; weg[slot] += re15_squareroot0((uint32_t)(wx * wx + wz * wz)); lx[slot] = x; lz[slot] = zz; }
            if (szene && !tot[slot] && (st == 3 || hp < 0)) { tot[slot] = 1; ab1[slot] = ab; b1[slot] = n_bilder; }
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
        /* NB1 (H2): in der Szene laeuft jeder >= 30 Bilder (rund 1 s), bevor er stirbt */
        const int ok = szene ? (tot[i] && w > 0 && ab1[i] >= 2000 && b1[i] - b0[i] >= 30) : (w >= 1000);
        if (ok) gut++;
        printf("ganglog %s: Slot %d Abstand zur Leiter %d -> %d (%s), Weg %d, %d Bilder gelaufen (Bahn %d)\n", modus, i, (int)ab0[i], (int)ab1[i], szene ? (tot[i] ? "beim Tod" : "nicht tot") : "Ende", (int)w,
               szene ? b1[i] - b0[i] : n_bilder - b0[i], (int)weg[i]);
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
static void teil_klemm_innen(int elza);
static int g2_kanister_ins_maul(re15_actor_t *g, re15_actor_t *pl);
static void teil_klemm(int elza)
{
    /* G2-RE2 NB2 "Vorschlag 4": der Mechanismus dieses Riegels (der Kanister als Koerper in Lenkung/Klemme) gilt nur noch mit dem Pruefhaken v4_alt (Stand f942f56c0);
     * der Riegel prueft ihn dort weiter. Die Bewegung mit Vorschlag 4 pruefen unit_r41_gatorfix_v4 / _re2 (Dossier G2_gatorfix.md "G2-RE2 NB2 (Vorschlag 4)"). */
    re15_gr2_v4_alt_setzen(1);
    teil_klemm_innen(elza);
    re15_gr2_v4_alt_setzen(0);
    {   /* Vorschlag 4: mit dem Kanister im Maul greift die Kopf-Klemme nicht (wie ohne Kanister) */
        re15_actor_t *g = szene_bis_kampf(elza);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int32_t dx = 0, dz = 0, zx = 0, zz = 0; int an = -1, ziel = -1;
        if (g && g2_kanister_ins_maul(g, pl)) {
            g->x = 3191; g->z = -22595; g->rot_y = 1;   /* Lage der Nutzer-Marke 1 F735 (Dossier Punkt 7) */
            an = re15_gr2_kopf_klemme((int)(g - g_actors), g, &dx, &dz);
            ziel = re15_gr2_kk_ziel(g, &zx, &zz);
        }
        PRUEF(an == 0 && ziel == 0, "Vorschlag 4: an der Lage der Nutzer-Marke keine Kopf-Klemme (%d, Ueberstand %d,%d) und kein verschobenes Steuerziel (%d) mit dem Kanister im Maul",
              an, (int)dx, (int)dz, ziel);
    }
}
static void teil_klemm_innen(int elza)
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


/* ---- Riegel Punkt 7 (Nachbesserung 1, Abnahme 1 M1/M3): Fuzz ueber Spielerlagen mit dem Kanister im Maul ------------------------
 * Je Lage steht die Figur still (Wand, Ecke, Plattform oben, Rampe, Leiter); der Alligator traegt den Kanister und startet an der von ihr
 * fernsten von fuenf Beckenlagen, echter Boss-Takt G2_FZ_BILDER Bilder. Masse wie die Abnahme (haenger.py/still_state.py): STILLSTAND = Wurzel
 * kommt in 30 Bildern netto < 300 voran, Kopf (Wurzel + 4200 voraus) > 2500 von der Figur, Phase Verfolgung/Belagerung; WANDREIBEN = Stillstand
 * und die Kopf-Klemme griff in den letzten 30 Bildern (re15_gr2_kk_aktiv). Gegenproben ueber re15_gr2_kk_ziel_alt_setzen: 1 = vor G2, 2 = G2 vor NB1. */
#define G2_FZ_BILDER 900   /* NB2: 30 s je Lage (Schleifen der Abnahme 2: Periode 266-440 Bilder) */
/* NB5: Lauflaenge je Lage (Riegel kamera 1800: die Anfahrt zur Bahn der Belagerung ueber den Westkanal dauert bis ~640 Bilder, Riegel bahn) */
static int s_fz_bilder = G2_FZ_BILDER;
typedef struct { const char *name; int32_t x, y, z; } g2_lage_t;
static const g2_lage_t k_fuzz[] = {
    { "W1", -8400, 0, -25000 }, { "W2", -8400, 0, -20000 }, { "W3", -8400, 0, -15000 }, { "W4", -8400, 0, -10000 },
    { "N1", -6000, 0, -5900 }, { "N2", -3000, 0, -5900 }, { "N3", 0, 0, -5900 }, { "N4", 3000, 0, -5900 },
    { "O1", 6700, 0, -8000 }, { "O2", 6700, 0, -12000 }, { "O3", 6700, 0, -21000 }, { "O4", 6700, 0, -25000 },
    { "S1", -6000, 0, -26500 }, { "S2", -3000, 0, -26500 }, { "S3", 0, 0, -26500 }, { "S4", 3000, 0, -26500 },
    { "NW", -8400, 0, -5900 }, { "NO", 6700, 0, -5900 }, { "SO", 6700, 0, -26500 }, { "SW", -8400, 0, -26500 },
    { "P-mitte", 0, -1800, -16000 }, { "P-west", -1400, -1800, -16000 }, { "P-nord", 0, -1800, -12800 }, { "P-sued", 0, -1800, -19700 },
    { "Rampe", 4500, -1800, -16300 }, { "Leiter", -8200, 0, -25550 },
    /* NB2: die Lagen der Abnahme 2 (G2_abnahme_2.md Abschnitt 2/4) */
    { "A2-Leiterecke", -8437, 0, -26507 }, { "A2-NW-Ecke", -8432, 0, -5868 }, { "A2-Ostnische", 7668, 0, -16257 }, { "A2-NO-Ecke", 6732, 0, -5868 },
    { "A2-RampeWest", 2318, -1800, -16685 },
};
static const int32_t k_fz_start[5][2] = { { -5000, -23000 }, { -5000, -9000 }, { 4000, -9000 }, { 4000, -23000 }, { -5500, -16000 } };
typedef struct { int bilder, still, rub_max, still_max, vorbei, fenster2, cam, im_bild, n2, kreis2, biss2, phase5, kreis_vis_min; int64_t kopf_summe; } g2_fz_t;   /* NB4: kreis2/biss2/phase5 = Bilder der zweiten Haelfte mit Belagerungs-Kreis (Trag 3) / Biss am Spieler / GUARD */
/* NB3 (M3'): Kamera, in der die Figur steht = der Cut 0..7, in dem sie am naechsten zur Bildmitte liegt (Kino-Balken 24..216) */
static int g2_spielerkamera(const g2_lage_t *L)
{
    int best = -1; int64_t bd = -1;
    for (int c = 0; c < 8 && c < g_room_rdt.cut_count; c++) {
        const int32_t w[3] = { L->x, L->y - 900, L->z }; int sx = 0, sy = 0;
        if (!re15_gr2_im_bild_cut(c, w, &sx, &sy) || sy < 24 || sy >= 216) continue;
        const int64_t d = (int64_t)(sx - 160) * (sx - 160) + (int64_t)(sy - 120) * (sy - 120);
        if (bd < 0 || d < bd) { bd = d; best = c; }
    }
    return best;
}

static int g2_kanister_ins_maul(re15_actor_t *g, re15_actor_t *pl)
{
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    const int gier = g->rot_y & 0xfff;
    int32_t vx = (int32_t)(((int64_t)re15_cos_q12(gier) * 8800 * sk) >> 24), vz = (int32_t)(((int64_t)-re15_sin_q12(gier) * 8800 * sk) >> 24);
    if (!re15_aot_water_at(g->x + vx, g->z + vz)) { vx = vz = 0; }
    re15_gr2_kanister_setzen(g->x + vx, -1200, g->z + vz);
    const int32_t x0 = g->x, z0 = g->z;
    for (int f = 0; f < 300 && !re15_gr2_traegt(); f++) { g->x = x0; g->z = z0; g->rot_y = (int16_t)gier; pl->x = -8200; pl->z = -25550; pl->hp = 100; frame(0, 0); }
    return re15_gr2_traegt();
}
static int s_fz_cam = -1;   /* NB4 (Punkt 2): Kamera der Figur, wie das Spiel sie ueber die RVD-Zonen waehlt (g2_weg_kamera); -1 = Projektion (NB3) */
static void g2_fuzz_lage(re15_actor_t *g, re15_actor_t *pl, const g2_lage_t *L, g2_fz_t *m, int zeige)
{
    memset(m, 0, sizeof *m);
    m->kreis_vis_min = -1;
    int bi = 0; int64_t bd = -1;
    for (int i = 0; i < 5; i++) {
        const int64_t dx = k_fz_start[i][0] - L->x, dz = k_fz_start[i][1] - L->z;
        if (dx * dx + dz * dz > bd) { bd = dx * dx + dz * dz; bi = i; }
    }
    g->x = k_fz_start[bi][0]; g->z = k_fz_start[bi][1];
    m->cam = s_fz_cam >= 0 ? s_fz_cam : g2_spielerkamera(L);
    int32_t hist[30][2], h60[60][2]; uint8_t kk[30], bs60[60]; int hn = 0, still_lauf = 0, rub_lauf = 0;
    memset(bs60, 0, sizeof bs60);
    int32_t schritt[60]; for (int k = 0; k < 60; k++) schritt[k] = 1000;
    for (int t = 0; t < s_fz_bilder && re15_gr2_traegt(); t++) {
        pl->x = L->x; pl->y = L->y; pl->z = L->z; pl->hp = 100; pl->hit_react = 0;
        if (s_fz_cam >= 0) { g_scd.cam_id = (uint8_t)s_fz_cam;   /* NB4: die Figur steht in dieser Kamera */
            re15_collision_set_band(re15_collision_band_from_y(L->y)); }   /* NB4: Kollisionsband der Lage wie nach dem Klettern (climb_common.c @0x800384bc) - sonst
                                                                              * schob die Kollision die Figur von der Rampe (z -16300 -> -14032) und sie war nie "oben" */
        frame(0, 0);
        m->bilder++;
        int64_t weit = 0;                                  /* weitester Abstand der letzten 60 Lagen von der aeltesten (Fensteranfang) */
        {   const int32_t *a0 = h60[hn % 60];
            for (int k = 0; k < 60; k++) { const int64_t ax = (int64_t)h60[k][0] - a0[0], az = (int64_t)h60[k][1] - a0[1]; if (ax * ax + az * az > weit) weit = ax * ax + az * az; }
            const int64_t ax = (int64_t)g->x - a0[0], az = (int64_t)g->z - a0[1]; if (ax * ax + az * az > weit) weit = ax * ax + az * az; }
        const int fenster2 = hn >= 60 && weit < 300LL * 300;
        h60[hn % 60][0] = g->x; h60[hn % 60][1] = g->z;
        hist[hn % 30][0] = g->x; hist[hn % 30][1] = g->z; kk[hn % 30] = (uint8_t)re15_gr2_kk_aktiv(); hn++;
        const int ph = re15_gator_boss_phase(g);
        const int th = g->rot_y & 0xfff;
        if (getenv("G2_BAHN")) {   /* NB5: Bahn je Bild (Werkzeug, Auswertung scratchpad) */
            static FILE *bf = NULL; if (!bf) bf = fopen(getenv("G2_BAHN"), "a");
            int32_t kx = 0, kz = 0; const int kr = re15_gr2_trag_kreis((int)(g - g_actors), &kx, &kz);
            int32_t bb[7] = { 0, 0, 0, 0, 0, 0, 0 }, ab = -1, sb = -1; const int bo = re15_gr2_bahn((int)(g - g_actors), bb);
            if (bo) sb = re15_gr2_bahn_naechst_mess(bb, g->x, g->z, &ab);
            if (bf) fprintf(bf, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", L->name, m->cam, t, g->x, g->z, th, ph, (int)g->motion, re15_gr2_trag_modus((int)(g - g_actors)), re15_gr2_kk_aktiv(), kr, kx, kz,
                            bo, bb[0], bb[1], bb[2], bb[3], bb[4], bb[5], bb[6], sb, ab);
        }
        if (getenv("G2_SPUR") && t % 60 == 0) printf("        %s t%d Gator (%d,%d) Gier %d Phase %d Trag %d Clip %d Cut %d Figur (%d,%d,%d) Band %d\n", L->name, t, g->x, g->z, th, ph, re15_gr2_trag_modus((int)(g - g_actors)), (int)g->motion, (int)g_scd.cam_id, pl->x, pl->y, pl->z, re15_collision_debug_band());
        const int64_t hx = (int64_t)g->x + ((4200LL * re15_cos_q12(th)) >> 12) - L->x, hz = (int64_t)g->z - ((4200LL * re15_sin_q12(th)) >> 12) - L->z;
        const int32_t kopf = (int32_t)re15_squareroot0((uint32_t)(hx * hx + hz * hz > 0xffffffffLL ? 0xffffffffLL : hx * hx + hz * hz));
        int still = 0, rub = 0;
        /* NB2 (M5): ohne Ausnahme ausser laufendem Biss (Phase 3/8) am Spieler im Wasser (Wurzel < 3200 oder Kopf < 2400) */
        const int64_t wx0 = (int64_t)g->x - L->x, wz0 = (int64_t)g->z - L->z;
        const int am_spieler = ((ph == 3 || ph == 8) && (wx0 * wx0 + wz0 * wz0 < 3200LL * 3200 || kopf < 2400))
                             || (ph == 5 && g->motion == 4 && L->y != 0 && kopf < 4200)   /* Hochbiss der Belagerung (Aufricht-Gate 4200) */
                             || re15_gator_boss_lauert(g) || re15_gr2_rw_taucht((int)(g - g_actors));   /* G2-RE2: Lauern am Startpunkt der Ueberquerung / Tauchgang der Notloesung */
        if (hn >= 30 && !am_spieler) {
            const int32_t *a = hist[hn % 30];
            const int64_t wx = (int64_t)g->x - a[0], wz = (int64_t)g->z - a[1];
            if (wx * wx + wz * wz < 300LL * 300) still = 1;
        }
        if (still) { m->still++; still_lauf++; if (still_lauf > m->still_max) m->still_max = still_lauf; } else still_lauf = 0;
        bs60[(hn - 1) % 60] = (uint8_t)am_spieler;
        int biss_im_fenster = 0;
        for (int k = 0; k < 60; k++) if (bs60[k]) biss_im_fenster = 1;
        if (t >= s_fz_bilder / 2) { m->kopf_summe += kopf; m->n2++; m->kreis2 += re15_gr2_trag_modus((int)(g - g_actors)) == 3; m->biss2 += am_spieler; m->phase5 += ph == 5;
            int32_t kx = 0, kz = 0;   /* NB4: Kreispunkte (von 16) des laufenden Kreises im Bild der Spielerkamera, kleinster Wert */
            if (m->cam >= 0 && re15_gr2_trag_kreis((int)(g - g_actors), &kx, &kz)) { const int v = re15_gr2_kreis_sicht(m->cam, kx, kz); if (m->kreis_vis_min < 0 || v < m->kreis_vis_min) m->kreis_vis_min = v; } }   /* NB3: Mittel ueber die zweite Haelfte (angekommen, Patrouille) */
        if (m->cam >= 0 && t >= s_fz_bilder / 2) { const int32_t w[3] = { g->x, -1200, g->z }; int sx = 0, sy = 0; if (re15_gr2_im_bild_cut(m->cam, w, &sx, &sy) && sy >= 24 && sy < 216) m->im_bild++; }
        if (fenster2 && !biss_im_fenster) { m->fenster2++; if (zeige) printf("        %s t%d FENSTER Gator (%d,%d) Gier %d Clip %d Phase %d Trag %d\n", L->name, t, g->x, g->z, th, (int)g->motion, ph, re15_gr2_trag_modus((int)(g - g_actors))); }
        if (fenster2 && !biss_im_fenster && re15_gr2_kk_aktiv()) rub = 1;   /* NB2: Wandreiben = Kopf-Klemme ohne Weg (2-s-Fenster) */
        if (rub) { rub_lauf++; if (rub_lauf > m->rub_max) m->rub_max = rub_lauf; } else rub_lauf = 0;
        if (zeige && still && (still_lauf == 1 || still_lauf % 60 == 0))
            printf("        %s t%d STILL Gator (%d,%d) Gier %d Clip %d Phase %d Trag %d Klemme %d Kopf-Abstand %d\n", L->name, t, g->x, g->z, th, (int)g->motion, ph, re15_gr2_trag_modus((int)(g - g_actors)),
                   re15_gr2_kk_aktiv(), (int)kopf);
    }
}

/* ---- Riegel NB4 Punkt 2 (Orchestrator nach NB3): Kreispunkt im sichtbaren Wasser der Spielerkamera. Die Kamera waehlt das Spiel ueber die RVD-Zonen
 *      (aot_common.c re15_aot_scan: Zone mit cam_from == laufender Cut, Lage im Viereck -> cam_to, die erste in Tabellenfolge, Anker der Gruppe nicht;
 *      FUN_80014230) - in ROOM2090 haengt sie am Weg (Werkzeug kamerazone). Nachgebaut: der Weg der Figur von der Leiter (Cut 0) durchs Wasser zur Rampe
 *      (Kletterzellen nur dort, Dossier NB1 M3), hinauf und oben zur Lage - von Sueden (Rampen-Suedkante) und von Norden (um die Plattform herum zur
 *      Rampen-Nordkante), je 50 Einheiten ein Scan-Schritt. Je Lage und Kamera 900 Bilder Belagerung mit dem Kanister (g2_fuzz_lage), Mass wie traglog:
 *      Wurzel (y -1200) im Bild der Kamera zwischen den Kino-Balken, zweite Haelfte. ---- */
static int g2_zonen_schritt(int32_t x, int32_t z, int c)
{
    for (int i = 0; i < g_room_rdt.zone_count; i++) {
        const re15_rdt_zone_t *zo = &g_room_rdt.zones[i];
        if (i == 0 || g_room_rdt.zones[i - 1].cam_from != zo->cam_from) continue;   /* Anker */
        if (zo->cam_from == c && re15_aot_point_in_quad(x, z, zo->xs, zo->zs)) return zo->cam_to;
    }
    return c;
}
static int g2_weg_kamera(const int32_t (*w)[2], int n, int c)
{
    for (int i = 0; i + 1 < n; i++) {
        const int64_t dx = w[i + 1][0] - w[i][0], dz = w[i + 1][1] - w[i][1];
        const int32_t d = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz));
        for (int32_t t = 0; t <= d; t += 50)
            c = g2_zonen_schritt(w[i][0] + (int32_t)(dx * t / (d ? d : 1)), w[i][1] + (int32_t)(dz * t / (d ? d : 1)), c);
    }
    for (int k = 0; k < 4; k++) { const int c2 = g2_zonen_schritt(w[n - 1][0], w[n - 1][1], c); if (c2 == c) return c; c = c2; }
    return -1;   /* Pendel an der Lage */
}
static const g2_lage_t k_kam[] = {
    { "R-2318s", 2318, -1800, -17700 }, { "R-2318m", 2318, -1800, -16300 }, { "R-2602n", 2602, -1800, -15000 }, { "R-3500s", 3500, -1800, -17700 },
    { "R-3500m", 3500, -1800, -16300 }, { "R-3500n", 3500, -1800, -14900 }, { "R-4500s", 4500, -1800, -17700 }, { "R-4500m", 4500, -1800, -16300 },
    { "R-4500n", 4500, -1800, -14900 }, { "R-5500s", 5500, -1800, -17700 }, { "R-5500m", 5500, -1800, -16300 }, { "R-5500n", 5500, -1800, -14900 },
    { "R-6500s", 6500, -1800, -17700 }, { "R-6500m", 6500, -1800, -16300 }, { "R-6500n", 6500, -1800, -14900 },
    /* Plattform: fuer die Figur nicht betretbar - auf Band 1 schiebt die Spieler-Kollision sie bei x < 2318 von der Plattform (Werkzeug begehbar:
     * (0,-17500) -> (-2168,-17500), (1600,-16000) -> (2318,-16000)), Kletterzellen hat nur die Rampe (kletterkarte); Abnahme 2: "blieb bei x 2318 an der
     * Plattformkante stehen". Keine Plattformlagen. */
    { "Leiter", -8200, 0, -25550 }, { "Leiterecke", -8437, 0, -26507 },
};
#define G2_KAM_MIN 70   /* Orchestrator: Anteil im Bild mindestens 70 % */
/* Weg zur Lage: 0 = von Sueden (Leiter -> Suedbecken -> Rampen-Suedkante), 1 = von Norden (Westkanal -> Nordbecken -> Rampen-Nordkante); Leiter: nur 0 */
static int g2_lage_kamera(const g2_lage_t *L, int von)
{
    int32_t w[10][2]; int n = 0;
    w[n][0] = -8200; w[n][1] = -25550; n++;
    if (L->y == 0) { w[n][0] = L->x; w[n][1] = L->z; n++; return g2_weg_kamera(w, n, 0); }
    const int32_t kx = L->x < 2300 ? 2300 : L->x > 6500 ? 6500 : L->x;   /* Kletterzellen der Rampe x 2000..6800 (kletterkarte) */
    if (von == 0) {
        w[n][0] = 0; w[n][1] = -24000; n++; w[n][0] = 4000; w[n][1] = -22000; n++;
        w[n][0] = kx; w[n][1] = -18800; n++; w[n][0] = kx; w[n][1] = -17900; n++;
    } else {
        w[n][0] = -6000; w[n][1] = -20000; n++; w[n][0] = -6000; w[n][1] = -9000; n++; w[n][0] = 4000; w[n][1] = -11000; n++;
        w[n][0] = kx; w[n][1] = -13800; n++; w[n][0] = kx; w[n][1] = -14700; n++;
    }
    if (L->x < 1850) { w[n][0] = 1850; w[n][1] = L->z < -18000 ? -18000 : L->z > -14600 ? -14600 : L->z; n++; }   /* ueber die Rampe auf die Plattform */
    w[n][0] = L->x; w[n][1] = L->z; n++;
    return g2_weg_kamera(w, n, 0);
}
static void teil_kamera(int elza)
{
    printf("== kamera%s (NB4 Punkt 2) ==\n", elza ? "_elza" : "");
    s_fz_bilder = 2 * G2_FZ_BILDER;   /* NB5: zweite Haelfte = nach der Anfahrt zur Bahn (s. s_fz_bilder) */
    const int zeige = getenv("G2_ZEIGE") != NULL;
    const char *nur = getenv("G2_FZ_NUR");
    const int nl = (int)(sizeof k_kam / sizeof k_kam[0]);
    int laeufe = 0, ok = 0, ohne = 0, schlecht = 0, fenster = 0, alt_unter = 0, alt_laeufe = 0, min_neu = 101; const char *min_wo = "-";
    for (int alt = 0; alt <= 1; alt++) {
        re15_gr2_kreis_kamera_alt_setzen(alt);
        re15_actor_t *g = szene_bis_kampf(elza);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (!g || !g2_kanister_ins_maul(g, pl)) { printf("    alt %d: kein Tragen\n", alt); continue; }
        for (int i = 0; i < nl; i++) {
            if (nur && strcmp(nur, k_kam[i].name)) continue;
            int cams[2] = { g2_lage_kamera(&k_kam[i], 0), k_kam[i].y == 0 ? -2 : g2_lage_kamera(&k_kam[i], 1) };
            if (cams[1] == cams[0]) cams[1] = -2;
            for (int v = 0; v < 2; v++) {
                if (cams[v] == -2) continue;
                if (cams[v] < 0) { if (!alt) printf("    %-14s %s: Kamera pendelt an der Lage - nicht gemessen\n", k_kam[i].name, v ? "Nord" : "Sued"); continue; }
                if (!re15_gr2_traegt() && !g2_kanister_ins_maul(g, pl)) break;
                int32_t cx = 0, cz = 0; int vis = 0;
                const int wasser = re15_gr2_kreis_kamera(cams[v], g, k_kam[i].x, k_kam[i].z, &cx, &cz, &vis);
                s_fz_cam = cams[v];
                g2_fz_t m; g2_fuzz_lage(g, pl, &k_kam[i], &m, zeige);
                s_fz_cam = -1;
                const int pct = m.n2 ? 100 * m.im_bild / m.n2 : 0;
                const int kopf = m.n2 ? (int)(m.kopf_summe / m.n2) : -1;
                const int kreis = m.n2 ? 100 * m.kreis2 / m.n2 : 0, biss = m.n2 ? 100 * m.biss2 / m.n2 : 0, guard = m.n2 ? 100 * m.phase5 / m.n2 : 0;
                printf("    %s %-14s %s Cut %d: im Bild %3d %% (Kopf-Abstand %5d, 2-s-Fenster %d; Belagerung %d %%, Kreisen %d %%, Biss am Spieler %d %%)%s", alt ? "NB3" : "neu", k_kam[i].name,
                       v ? "Nord" : "Sued", cams[v], pct, kopf, m.fenster2, guard, kreis, biss,
                       wasser ? "" : " - kein sichtbares Wasser in dieser Kamera");
                if (wasser) printf(", Mittelpunkt (%d,%d) %d/16 im Bild", (int)cx, (int)cz, vis);
                printf("\n");
                printf("        Kreis der Haelfte 2: kleinster Anteil im Bild %d/16 (-1 = kein Kreis)\n", m.kreis_vis_min);
                if (alt) { if (wasser) { alt_laeufe++; if (pct < G2_KAM_MIN || (m.kreis_vis_min >= 0 && m.kreis_vis_min < 12)) alt_unter++; } continue; }
                laeufe++; fenster += m.fenster2;
                if (!wasser) { ohne++; continue; }
                if (pct >= G2_KAM_MIN) ok++; else schlecht++;
                if (pct < min_neu) { min_neu = pct; min_wo = k_kam[i].name; }
            }
        }
    }
    re15_gr2_kreis_kamera_alt_setzen(0);
    s_fz_bilder = G2_FZ_BILDER;
    printf("    neu: %d Laeufe, %d mit sichtbarem Wasser, davon %d >= %d %% (kleinster %d %% bei %s), %d ohne sichtbares Wasser; Gegenprobe NB3: %d von %d unter %d %%\n",
           laeufe, ok + schlecht, ok, G2_KAM_MIN, min_neu, min_wo, ohne, alt_unter, alt_laeufe, G2_KAM_MIN);
    PRUEF(laeufe >= 30 && schlecht == 0, "Belagerung mit Kanister: in jeder Lage und Kamera mit sichtbarem Wasser kreist er zu >= %d %% im Bild (%d von %d, kleinster %d %% bei %s)",
          G2_KAM_MIN, ok, ok + schlecht, min_neu, min_wo);
    PRUEF(fenster == 0, "kein 2-s-Fenster ohne Fortschritt (ausser Biss am Spieler): %d", fenster);
    PRUEF(alt_unter > 0, "Gegenprobe Mittelpunkt wie NB3: %d von %d Lagen/Kameras unter %d %% oder Kreis unter 12/16 im Bild (rot ohne den Bau)", alt_unter, alt_laeufe, G2_KAM_MIN);
}
/* ---- Riegel NB5 (Abnahme 3 M2a/M2b): die GEFAHRENE Bahn der Belagerung gegen den Plan, und die Erkennbarkeit im Bild der Spielerkamera ----
 * Je Rampenlage (k_kam) und Kamera des Wegs (Sued Cut 2 / Nord Cut 3) G2_BAHN_BILDER Bilder Belagerung mit dem Kanister (g2_fuzz_lage). Ab dem ersten Bild
 * auf der Bahn (Abstand zur geplanten Bahn < 200): Abstand je Bild (p95, max), Fortschritt der Bogenlaenge (Runden, Umlaufzeit gegen Plan = Umfang /
 * Median-Schritt), Schwerpunkt gegen Plan-Mitte, Kopf-Klemme. ERKENNBAR: Silhouette des Alligators im Bild der Spielerkamera = Vereinigung der projizierten
 * konvexen Huellen je Part (EMD-Scheitel, Pose dieses Bildes, Kamera wie der Zeichner, Kino-Balken 24..216), in Bildpunkten (320 x 192); Bezug = derselbe
 * Wert fuer die Verfolgung an der Leiter in Cut 0 (Abnahme 3: dort klar erkennbar, bewegte Flaeche 12,8 %). Gegenprobe Stand NB4 (Pruefhaken
 * re15_gr2_bahn_alt_setzen + re15_gr2_karotte_alt_setzen): Kreis (3450,-9150) R 2000 wird nicht gefahren, in Cut 2 klein. */
#define G2_BAHN_BILDER 1800
static int gr2_test_pkt_cmp(const void *x, const void *y)
{
    const int32_t *a = (const int32_t *)x, *b = (const int32_t *)y;
    return a[0] != b[0] ? (a[0] < b[0] ? -1 : 1) : (a[1] < b[1] ? -1 : a[1] > b[1]);
}
static int g2_cmp_dbl(const double *a, const double *b) { return *a < *b ? -1 : *a > *b; }
static uint8_t s_sil[240][320];
static int g2_sil_flaeche(re15_actor_t *g, int cam)
{
    re15_enemy_bank_t *gb = re15_enemy_find(0x23u);
    re15_camera_view_t v;
    if (!gb || cam < 0 || cam >= g_room_rdt.cut_count || re15_camera_build_view(&g_room_rdt.cuts[cam], &v) != 0) return -1;
    re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
    if (!g2_pose(g, ps)) return -1;
    static int32_t pv[4096][3];
    memset(s_sil, 0, sizeof s_sil);
    const int32_t sk = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    const int np = gb->skel.bone_count < gb->md1.mesh_count ? gb->skel.bone_count : gb->md1.mesh_count;
    for (int part = 0; part < np; part++) {
        int32_t rb[9], pb[3]; g2_part_welt(g, &ps[part], rb, pb);
        const int n = g2_scheitel(&gb->md1, part, pv, 4096);
        static int32_t q[4096][2]; int m = 0;
        for (int i = 0; i < n; i++) {
            const int32_t vv[3] = { g2_q12(pv[i][0], sk), g2_q12(pv[i][1], sk), g2_q12(pv[i][2], sk) };
            const int64_t wx = pb[0] + (((int64_t)rb[0] * vv[0] + (int64_t)rb[1] * vv[1] + (int64_t)rb[2] * vv[2]) >> 12);
            const int64_t wy = pb[1] + (((int64_t)rb[3] * vv[0] + (int64_t)rb[4] * vv[1] + (int64_t)rb[5] * vv[2]) >> 12);
            const int64_t wz = pb[2] + (((int64_t)rb[6] * vv[0] + (int64_t)rb[7] * vv[1] + (int64_t)rb[8] * vv[2]) >> 12);
            const int32_t vx = (int32_t)((v.rot[0]*wx + v.rot[1]*wy + v.rot[2]*wz) >> 12) + v.trans[0];
            const int32_t vy = (int32_t)((v.rot[3]*wx + v.rot[4]*wy + v.rot[5]*wz) >> 12) + v.trans[1];
            const int32_t vz = (int32_t)((v.rot[6]*wx + v.rot[7]*wy + v.rot[8]*wz) >> 12) + v.trans[2];
            if (vz <= 16) continue;
            q[m][0] = 160 + (int32_t)(((int64_t)vx * v.fov_screen_dist) / vz); q[m][1] = 120 + (int32_t)(((int64_t)vy * v.fov_screen_dist) / vz); m++;
        }
        if (m < 3) continue;
        /* konvexe Huelle (monotone Kette) und Zeilenfuellung */
        qsort(q, (size_t)m, sizeof q[0], gr2_test_pkt_cmp);
        static int32_t h[8192][2]; int k = 0;
        for (int i = 0; i < m; i++) { while (k >= 2 && (int64_t)(h[k-1][0]-h[k-2][0])*(q[i][1]-h[k-2][1]) - (int64_t)(h[k-1][1]-h[k-2][1])*(q[i][0]-h[k-2][0]) <= 0) k--; h[k][0] = q[i][0]; h[k][1] = q[i][1]; k++; }
        for (int i = m - 2, u = k + 1; i >= 0; i--) { while (k >= u && (int64_t)(h[k-1][0]-h[k-2][0])*(q[i][1]-h[k-2][1]) - (int64_t)(h[k-1][1]-h[k-2][1])*(q[i][0]-h[k-2][0]) <= 0) k--; h[k][0] = q[i][0]; h[k][1] = q[i][1]; k++; }
        if (k > 1) k--;
        int32_t y0 = 0x7fffffff, y1 = -0x7fffffff;
        for (int i = 0; i < k; i++) { if (h[i][1] < y0) y0 = h[i][1]; if (h[i][1] > y1) y1 = h[i][1]; }
        if (y0 < 24) y0 = 24;
        if (y1 > 215) y1 = 215;
        for (int32_t y = y0; y <= y1; y++) {
            double xa = 1e9, xb = -1e9;
            for (int i = 0; i < k; i++) {
                const int32_t *a = h[i], *b = h[(i + 1) % k];
                if ((a[1] <= y && b[1] >= y) || (b[1] <= y && a[1] >= y)) {
                    double x = a[1] == b[1] ? a[0] : a[0] + (double)(b[0] - a[0]) * (y - a[1]) / (double)(b[1] - a[1]);
                    if (a[1] == b[1]) { if (b[0] < xa) xa = b[0]; if (b[0] > xb) xb = b[0]; }
                    if (x < xa) xa = x;
                    if (x > xb) xb = x;
                }
            }
            for (int x = xa < 0 ? 0 : (int)ceil(xa); x <= (xb > 319 ? 319 : (int)floor(xb)); x++) s_sil[y][x] = 1;
        }
    }
    int a = 0;
    for (int y = 24; y < 216; y++) for (int x = 0; x < 320; x++) a += s_sil[y][x];
    return a;
}
typedef struct { int auf, bilder, f_an; double ab_p95, ab_max, runden, umlauf, umlauf_plan, sx, sz, mx, mz; int kk; int sil_med, sil_n, sil_sicht; int32_t b[7]; } g2_bahn_m_t;
static int g2_cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }
static void g2_bahn_lauf(re15_actor_t *g, re15_actor_t *pl, const g2_lage_t *L, int cam, g2_bahn_m_t *o, int alt)
{
    memset(o, 0, sizeof *o); o->f_an = -1;
    const int slot = (int)(g - g_actors);
    static int ab[G2_BAHN_BILDER], sil[G2_BAHN_BILDER]; static int32_t xs[G2_BAHN_BILDER], zs[G2_BAHN_BILDER], ss[G2_BAHN_BILDER];
    int n = 0, ns = 0;
    int bi = 0; int64_t bd = -1;
    for (int i = 0; i < 5; i++) { const int64_t dx = k_fz_start[i][0] - L->x, dz = k_fz_start[i][1] - L->z; if (dx * dx + dz * dz > bd) { bd = dx * dx + dz * dz; bi = i; } }
    g->x = k_fz_start[bi][0]; g->z = k_fz_start[bi][1];
    for (int t = 0; t < G2_BAHN_BILDER && re15_gr2_traegt(); t++) {
        pl->x = L->x; pl->y = L->y; pl->z = L->z; pl->hp = 100; pl->hit_react = 0;
        g_scd.cam_id = (uint8_t)cam; re15_collision_set_band(re15_collision_band_from_y(L->y));
        frame(0, 0);
        int32_t b[7], d = 0, s = 0, kx = 0, kz = 0;
        if (!alt) {
            if (!re15_gr2_bahn(slot, b)) continue;
            s = re15_gr2_bahn_naechst_mess(b, g->x, g->z, &d);
            if (o->f_an < 0 && d < 200) { o->f_an = t; memcpy(o->b, b, sizeof b); }   /* angekommen: Abstand zur Bahn < 200 */
        } else {                                           /* Gegenprobe NB4: Abstand zum geplanten Kreis R 2000 */
            if (re15_gr2_trag_kreis(slot, &kx, &kz) != 2) continue;
            const double dd = hypot((double)g->x - kx, (double)g->z - kz);
            d = (int32_t)fabs(dd - 2000); s = (int32_t)(atan2((double)g->z - kz, (double)g->x - kx) * 2000);
            if (o->f_an < 0 && d < 2000) { o->f_an = t; o->b[0] = o->b[2] = kx; o->b[1] = o->b[3] = kz; o->b[4] = 2000; o->b[6] = (int32_t)(2 * 3.14159265 * 2000); }
        }
        if (o->f_an < 0) continue;
        ab[n] = d; xs[n] = g->x; zs[n] = g->z; ss[n] = s; o->kk += re15_gr2_kk_aktiv(); n++;
        if ((t & 3) == 0) { const int a = g2_sil_flaeche(g, cam); if (a >= 0) { sil[ns++] = a; o->sil_sicht += a > 0; } }
    }
    o->bilder = n; o->sil_n = ns;
    if (n < 30) return;
    int *tmp = (int *)malloc(sizeof(int) * (size_t)n); memcpy(tmp, ab, sizeof(int) * (size_t)n); qsort(tmp, (size_t)n, sizeof(int), g2_cmp_int);
    o->ab_p95 = tmp[(n * 95) / 100]; o->ab_max = tmp[n - 1]; free(tmp);
    double prog = 0, step_sum = 0; static double steps[G2_BAHN_BILDER];
    const double U = o->b[6];
    for (int i = 1; i < n; i++) {
        double ds = ss[i] - ss[i-1];
        if (alt) { ds = ds / 2000.0; ds = fmod(ds + 3.14159265 * 3, 2 * 3.14159265) - 3.14159265; ds *= 2000; }
        else ds = fmod(ds + U * 1.5, U) - U / 2;
        prog += ds;
        steps[i-1] = hypot((double)xs[i] - xs[i-1], (double)zs[i] - zs[i-1]); step_sum += steps[i-1];
    }
    qsort(steps, (size_t)(n - 1), sizeof(double), (int (*)(const void *, const void *))g2_cmp_dbl);
    const double v = steps[(n - 1) / 2];
    o->runden = fabs(prog) / U; o->umlauf = o->runden > 0 ? (n - 1) / o->runden : 1e9; o->umlauf_plan = v > 0 ? U / v : 1e9;
    for (int i = 0; i < n; i++) { o->sx += xs[i]; o->sz += zs[i]; }
    o->sx /= n; o->sz /= n; o->mx = (o->b[0] + o->b[2]) / 2.0; o->mz = (o->b[1] + o->b[3]) / 2.0;
    if (ns > 0) { qsort(sil, (size_t)ns, sizeof(int), g2_cmp_int); o->sil_med = sil[ns / 2]; }
}
static int s_bahn_ref_sil = -1;   /* Bezug: Silhouette der Verfolgung an der Leiter (Cut 0), Median */
static void teil_bahn(int elza)
{
    printf("== bahn%s (NB5 M2a/M2b) ==\n", elza ? "_elza" : "");
    const char *nur = getenv("G2_FZ_NUR");
    const int nl = (int)(sizeof k_kam / sizeof k_kam[0]);
    int laeufe = 0, ok_bahn = 0, ok_sicht = 0, alt_rot = 0, alt_n = 0, ref = -1;
    double schlecht_ab = 0, min_sicht_q = 1e9; const char *wo_ab = "-", *wo_sicht = "-";
    for (int alt = 0; alt <= 1; alt++) {
        re15_gr2_bahn_alt_setzen(alt); re15_gr2_karotte_alt_setzen(alt);
        re15_actor_t *g = szene_bis_kampf(elza);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (!g || !g2_kanister_ins_maul(g, pl)) { printf("    alt %d: kein Tragen\n", alt); continue; }
        if (!alt) {                                        /* Bezug: Verfolgung an der Leiter, Cut 0, zweite Haelfte von G2_FZ_BILDER */
            const g2_lage_t *Lt = &k_kam[nl - 2];
            int sil[G2_FZ_BILDER]; int ns = 0;
            for (int t = 0; t < G2_FZ_BILDER && re15_gr2_traegt(); t++) {
                pl->x = Lt->x; pl->y = Lt->y; pl->z = Lt->z; pl->hp = 100; pl->hit_react = 0; g_scd.cam_id = 0;
                frame(0, 0);
                if (t >= G2_FZ_BILDER / 2 && (t & 3) == 0) { const int a = g2_sil_flaeche(g, 0); if (a >= 0) sil[ns++] = a; }
            }
            if (ns) { qsort(sil, (size_t)ns, sizeof(int), g2_cmp_int); ref = sil[ns / 2]; }
            s_bahn_ref_sil = ref;
            printf("    Bezug Verfolgung an der Leiter (Cut 0): Silhouette Median %d Bildpunkte (%d Bilder gemessen)\n", ref, ns);
            if (!re15_gr2_traegt() && !g2_kanister_ins_maul(g, pl)) break;
        }
        for (int i = 0; i < nl - 2; i++) {
            if (nur && strcmp(nur, k_kam[i].name)) continue;
            for (int von = 0; von < 2; von++) {
                const int cam = g2_lage_kamera(&k_kam[i], von);
                if (cam < 0) continue;
                if (!re15_gr2_traegt() && !g2_kanister_ins_maul(g, pl)) break;
                g2_bahn_m_t m; g2_bahn_lauf(g, pl, &k_kam[i], cam, &m, alt);
                const double q = ref > 0 ? (double)m.sil_med / ref : 0;
                printf("    %s %-8s %s Cut %d: Plan A (%d,%d) B (%d,%d) R %d Umfang %d | auf der Bahn ab Bild %d, %d Bilder: Abstand p95 %.0f max %.0f, %.2f Runden, Umlauf %.0f Bilder (Plan %.0f), "
                       "Schwerpunkt (%.0f,%.0f) Plan-Mitte (%.0f,%.0f), Klemme %d Bilder | Silhouette Median %d (%.0f %% des Bezugs), im Bild %d/%d\n",
                       alt ? "NB4" : "neu", k_kam[i].name, von ? "Nord" : "Sued", cam, m.b[0], m.b[1], m.b[2], m.b[3], m.b[4], m.b[6], m.f_an, m.bilder, m.ab_p95, m.ab_max, m.runden,
                       m.umlauf, m.umlauf_plan, m.sx, m.sz, m.mx, m.mz, m.kk, m.sil_med, 100 * q, m.sil_sicht, m.sil_n);
                if (alt) { alt_n++; if (m.bilder < 300 || m.ab_p95 > 300 || m.umlauf > 1.25 * m.umlauf_plan || m.umlauf < 0.75 * m.umlauf_plan || (cam == 2 && q < 0.5)) alt_rot++; continue; }
                laeufe++;
                const int bahn_ok = m.bilder >= 300 && m.ab_p95 <= 300 && m.ab_max <= 450 && m.umlauf <= 1.25 * m.umlauf_plan && m.umlauf >= 0.75 * m.umlauf_plan
                                    && hypot(m.sx - m.mx, m.sz - m.mz) <= 300 && m.kk * 20 <= m.bilder;
                if (bahn_ok) ok_bahn++; else if (m.ab_p95 >= schlecht_ab) { schlecht_ab = m.ab_p95; wo_ab = k_kam[i].name; }
                const int sicht_ok = q >= 0.5 && m.sil_sicht * 3 >= m.sil_n * 2;
                if (sicht_ok) ok_sicht++;
                if (q < min_sicht_q) { min_sicht_q = q; wo_sicht = k_kam[i].name; }
            }
        }
    }
    re15_gr2_bahn_alt_setzen(0); re15_gr2_karotte_alt_setzen(0);
    PRUEF(laeufe >= 20 && ok_bahn == laeufe, "M2a: gefahrene Bahn = Plan in %d von %d Lagen/Kameras (auf der Bahn >= 300 Bilder, Abstand p95 <= 300 / max <= 450, Umlauf 0,75..1,25 x Plan, Schwerpunkt <= 300 von der Plan-Mitte, Klemme <= 5 %%; schlechteste %s p95 %.0f)",
          ok_bahn, laeufe, wo_ab, schlecht_ab);
    PRUEF(ref > 0 && ok_sicht == laeufe, "M2b: erkennbar in %d von %d Lagen/Kameras (Silhouette Median >= 50 %% des Bezugs Leiter/Cut 0 = %d Bildpunkte, in >= 2/3 der Bilder im Bild; kleinster %.0f %% bei %s)",
          ok_sicht, laeufe, ref, 100 * min_sicht_q, wo_sicht);
    PRUEF(alt_n > 0 && alt_rot == alt_n, "Gegenprobe Stand NB4 (Kreis R 2000, gespiegelte Karotte): %d von %d Laeufen faehrt er den Plan nicht oder ist in Cut 2 nicht erkennbar (rot ohne den Bau)", alt_rot, alt_n);
}
static void teil_fuzz_innen(int elza);
static void teil_fuzz(int elza)
{
    /* G2-RE2 NB2 "Vorschlag 4": der Mechanismus dieses Riegels (Trag-Lenkung mit Kopf + Kanister: Kuerzung, Vorbei-Etappe, Kreis) gilt nur noch mit dem Pruefhaken v4_alt (Stand f942f56c0);
     * der Riegel prueft ihn dort weiter. Die Bewegung mit Vorschlag 4 pruefen unit_r41_gatorfix_v4 / _re2 (Dossier G2_gatorfix.md "G2-RE2 NB2 (Vorschlag 4)"). */
    re15_gr2_v4_alt_setzen(1);
    teil_fuzz_innen(elza);
    re15_gr2_v4_alt_setzen(0);
}
static void teil_fuzz_innen(int elza)
{
    printf("== fuzz%s (Punkt 7, Nachbesserung 1) ==\n", elza ? "_elza" : "");
    const int zeige = getenv("G2_ZEIGE") != NULL;
    const char *nur = getenv("G2_FZ_NUR");
    int still[3] = { 0, 0, 0 }, bilder[3] = { 0, 0, 0 }, rub[3] = { 0, 0, 0 }, smax[3] = { 0, 0, 0 };
    int f2[3] = { 0, 0, 0 };
    int kopf_max = 0, bild_min = 100;
    const char *rub_wo[3] = { "-", "-", "-" };
    const int nl = (int)(sizeof k_fuzz / sizeof k_fuzz[0]);
    for (int alt = 0; alt <= 2; alt++) {
        if (getenv("G2_FZ_NEU") && alt) break;
        re15_gr2_kk_ziel_alt_setzen(alt);
        re15_actor_t *g = szene_bis_kampf(elza);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (!g || !g2_kanister_ins_maul(g, pl)) { printf("    alt %d: kein Tragen\n", alt); re15_gr2_kk_ziel_alt_setzen(0); continue; }
        for (int i = 0; i < nl; i++) {
            if (nur && strcmp(nur, k_fuzz[i].name)) continue;
            /* NB4: Lagen oben / an der Leiter in der Kamera, die der Weg von Sueden ergibt (RVD-Zonen, g2_lage_kamera), mit dem Kollisionsband der Lage -
             * vorher stand die Figur nie oben (Kollision schob sie von der Rampe) und die Kamera war die zufaellig laufende */
            s_fz_cam = (k_fuzz[i].y != 0 || !strcmp(k_fuzz[i].name, "Leiter")) ? g2_lage_kamera(&k_fuzz[i], 0) : -1;
            g2_fz_t m; g2_fuzz_lage(g, pl, &k_fuzz[i], &m, zeige);
            s_fz_cam = -1;
            still[alt] += m.still; bilder[alt] += m.bilder;
            if (m.rub_max > rub[alt]) { rub[alt] = m.rub_max; rub_wo[alt] = k_fuzz[i].name; }
            if (m.still_max > smax[alt]) smax[alt] = m.still_max;
            printf("    %s %-7s: %3d Bilder, Stillstand %3d (laengster %3d), Wandreiben laengstes %3d, 2-s-Fenster ohne Fortschritt %3d%s\n", alt == 0 ? "neu  " : alt == 1 ? "vorG2" : "G2v1 ",
                   k_fuzz[i].name, m.bilder, m.still, m.still_max, m.rub_max, m.fenster2, re15_gr2_traegt() ? "" : " [Kanister weg]");
            f2[alt] += m.fenster2;
            if (k_fuzz[i].y != 0 || !strcmp(k_fuzz[i].name, "Leiter")) {   /* NB3 (M3'): Figur oben bzw. an der Leiter */
                printf("        %s: Kopf-Abstand im Mittel %d, Gator im Bild der Spielerkamera (Cut %d) %d %%\n", k_fuzz[i].name, m.n2 ? (int)(m.kopf_summe / m.n2) : -1,
                       m.cam, m.n2 ? 100 * m.im_bild / m.n2 : 0);
                if (alt == 0) { if (m.n2 && m.kopf_summe / m.n2 > kopf_max) kopf_max = (int)(m.kopf_summe / m.n2); if (m.n2 && 100 * m.im_bild / m.n2 < bild_min) bild_min = 100 * m.im_bild / m.n2; }
            }
        }
        re15_gr2_kk_ziel_alt_setzen(0);
    }
    for (int alt = 0; alt <= 2; alt++)
        printf("    %s: %d Bilder, Stillstand %d (%d %%), laengster Stillstand %d, laengstes Wandreiben %d (%s)\n", alt == 0 ? "neu  " : alt == 1 ? "vor G2" : "G2 v1",
               bilder[alt], still[alt], bilder[alt] ? 100 * still[alt] / bilder[alt] : 0, smax[alt], rub[alt], rub_wo[alt]);
    PRUEF(bilder[0] >= nl * G2_FZ_BILDER * 9 / 10, "Fuzz gelaufen (%d Bilder mit Kanister)", bilder[0]);
    PRUEF(rub[0] <= 30, "kein Wandreiben > 1 s mit dem Kanister (laengstes %d Bilder bei %s)", rub[0], rub_wo[0]);
    printf("    Etappen ohne Annaeherung (abgebrochen): %d\n", re15_gr2_trag_ohne_annaeherung());
    /* NB4 (Punkt 2): der Mittelpunkt liegt im sichtbaren Wasser der Spielerkamera, nicht mehr am naechsten freien Punkt - der naechste sichtbare liegt bis
     * 8930 von der Figur (Rampe (6500,-15000) in Cut 3 -> (3450,-23400), Riegel kamera); Schranke NB3 8500 (Rampenmitte) -> 9000 */
    PRUEF(kopf_max <= 9000, "NB3/NB4: Figur oben/an der Leiter - mittlerer Kopf-Abstand der Patrouille hoechstens %d (<= 9000: naechster Mittelpunkt im sichtbaren Wasser bis 8930 von der Figur; im Bild misst der Riegel kamera; Bildanteil hier nur berichtet: %d %%)", kopf_max, bild_min);
    /* G2-RE2: Rueckwaerts setzen an Wand/Ecke (RE2-Clip 10, -700 in 30 Bildern, danach wieder vor) ergibt einzelne 2-s-Fenster; Schwelle 10 in 31 Lagen x 900 Bildern
     * (PORT-WAHL, Dossier G2-RE2; vor G2 1032 Fenster) */
    PRUEF(f2[0] <= 10, "NB2 (M5)/G2-RE2: hoechstens 10 Zeitfenster von 2 s (60 Bilder), in denen die Wurzel nicht 300 vom Anfang wegkommt, ausser mit Biss am Spieler (%d Fenster; laengste 30-Bilder-Folge %d)", f2[0], smax[0]);
    if (!getenv("G2_FZ_NEU")) {
        PRUEF((int64_t)still[0] * bilder[1] <= (int64_t)still[1] * bilder[0], "Stillstand mit Kanister hoechstens wie vor G2 (%d / %d gegen %d / %d)", still[0], bilder[0], still[1], bilder[1]);
        PRUEF(rub[2] > 30 || f2[2] > 0, "Gegenprobe G2 vor NB1: Wandreiben %d / 2-s-Fenster ohne Fortschritt %d (rot ohne den Bau)", rub[2], f2[2]);
    }
}

/* ---- Werkzeug NB5 (M2a/M2b): bahnpruef [elza] [cam ax az bx bz r kdir ...] - Plan je Kamera 0..7 und Bewertung gegebener Bahnen ---- */
static int bahnpruef_werkzeug(int argc, char **argv)
{
    const int elza = argc > 2 && !strcmp(argv[2], "elza");
    re15_actor_t *g = szene_bis_kampf(elza);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!g || !g2_kanister_ins_maul(g, pl)) { printf("kein Tragen\n"); return 1; }
    int i0 = elza ? 3 : 2;
    if (argc <= i0) {
        for (int c = 0; c < 8 && c < g_room_rdt.cut_count; c++) {
            int32_t b[7]; int vis = 0;
            const clock_t t0 = clock();
            const int ok = re15_gr2_bahn_plan_mess(c, g, 4500, -16300, b, &vis);
            const double ms = 1000.0 * (double)(clock() - t0) / CLOCKS_PER_SEC;
            int v2 = 0, kopf = 0;
            if (ok) re15_gr2_bahn_bewerten_mess(c, g, b, &v2, &kopf);
            printf("    Cut %d: %s A (%d,%d) B (%d,%d) R %d Drehsinn %d Umfang %d, %d/48 im Bild, Kopflaenge %d Bildpunkte (%.0f ms)\n", c, ok ? "Bahn" : "keine",
                   b[0], b[1], b[2], b[3], b[4], b[5], b[6], v2, kopf, ms);
        }
        return 0;
    }
    for (int i = i0; i + 6 < argc; i += 7) {
        int32_t b[7] = { atoi(argv[i+1]), atoi(argv[i+2]), atoi(argv[i+3]), atoi(argv[i+4]), atoi(argv[i+5]), atoi(argv[i+6]), 0 };
        int vis = 0, kopf = 0;
        const int ok = re15_gr2_bahn_bewerten_mess(atoi(argv[i]), g, b, &vis, &kopf);
        printf("    Cut %s A (%d,%d) B (%d,%d) R %d Drehsinn %d: %s, %s %d, Kopflaenge %d\n", argv[i], b[0], b[1], b[2], b[3], b[4], b[5], ok > 0 ? "zulaessig" : "UNZULAESSIG",
               ok > 0 ? "im Bild" : "erster Stuetzpunkt", vis, kopf);
    }
    return 0;
}
/* ---- Werkzeug NB5 (M2a): Wurzel-Kasten der Trag-Lenkung je Schwimmrichtung (Kopf + Kanister in den Aussenwaenden) ---- */
static int kasten_werkzeug(int elza)
{
    re15_actor_t *g = szene_bis_kampf(elza);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (!g || !g2_kanister_ins_maul(g, pl)) { printf("kein Tragen\n"); return 1; }
    for (int c = 0; c < g_room_rdt.cut_count; c++) {
        const re15_camera_cut_t *k = &g_room_rdt.cuts[c];
        printf("    Cut %d: Kamera (%d,%d,%d) Blick (%d,%d,%d) fov %u\n", c, k->pos_x, k->pos_y, k->pos_z, k->target_x, k->target_y, k->target_z, (unsigned)k->fov);
    }
    for (int c = 0; c < 12; c++) {
        int32_t a8[8];
        if (!re15_gr2_huelle_ausdehnung(g, c, a8)) continue;
        printf("    Clip %2d: Kopf vorn %5d hinten %5d rechts %5d links %5d | Kanister vorn %5d hinten %5d rechts %5d links %5d\n", c, a8[1], a8[0], a8[2], a8[3], a8[5], a8[4], a8[6], a8[7]);
    }
    for (int a = 0; a < 4096; a += 256) {   /* a = Schwimmrichtung atan2(dz,dx) */
        const int gier = (a - 1024) & 0xfff;
        int32_t k[4];
        if (!re15_gr2_tr_kasten_mess(g, gier, k)) { printf("keine Huelle\n"); return 1; }
        printf("    Richtung %4d (dx %5d dz %5d) Gier %4d: Wurzel x [%6d,%6d] z [%6d,%6d]\n", a, re15_cos_q12(a), re15_sin_q12(a), gier, k[0], k[1], k[2], k[3]);
    }
    return 0;
}

/* ==== Runde 41 Spur G2-RE2 (Neuaufbau ab NB5 bc5fb7c9c): MASS UND RIEGEL ===========================================================================
 * Dossier G2_gatorfix.md Abschnitt "G2-RE2 (Neuaufbau ab NB5)". Echter Boss-Takt mit dem Kanister im Maul, je Lage der Figur (Abnahme 4: SO-Ecke = frei4_leon,
 * Nordbecken-Ost = west4_elza, NO-Ecke = nordost; Rampe x4428/x2318 = west4_leon/west5_elza; Leiter/Leiterecke = leiter2; dazu Westkanal, Mitte-Nord,
 * Plattform-Sued, zwei Wege mit bewegter Figur) G2_RE2_BILDER Bilder; Stand "nb5" = Pruefhaken re15_gr2_rw_alt_setzen(1), Stand "neu" = Bau.
 * Masse (PORT-WAHL des Messwegs, je Bild):
 *  - ohne Fortschritt: Folge von Bildern, in denen der Kopf (Bone 6) der Figur nicht 300 naeher kommt als sein bester Wert seit Folgenbeginn, solange er
 *    weiter als 4500 von ihr ist (naeher = "am Spieler"); die Folge endet mit Annaeherung, Naehe oder wenn die Figur > 1500 weitergeht;
 *  - 2-s-Fenster (Mass der Abnahmen): 60 Bilder, in denen die Wurzel nicht 300 vom Fensteranfang wegkommt, ausser mit Biss;
 *  - Drehen auf der Stelle: Bild liegt in einem 120-Bilder-Fenster, in dem die Gier sich um >= 360 Grad dreht und die Wurzel in einem Kasten < 3000 bleibt;
 *  - rueckwaerts / Tauchgaenge / untergetaucht: re15_gr2_rw_modus;
 *  - Kopf/Kanister in Rampe/Plattform: ein Punkt von re15_gr2_rw_kopf_punkte (Mesh 5/6/7 + Kanister-Zylinder der gezeichneten Pose) unter dem Deck (y > -1800)
 *    im Grundriss von Plattform/Rampe (ohne Rand); vor der Kamera: kleinste Bildtiefe dieser Punkte in der Kamera der Figur < 2500 bzw. <= 0 (dahinter);
 *  - Rammen / Ueberquerungen: Zaehler des Boss-Moduls; Takt-Rechenzeit: Faden-Zyklen des Boss-Takts (re15_gator_boss_zeit_letzt) in Mikrosekunden;
 *  - Gang: in Bildern der normalen Verfolgung (Phase CHASE, Modus 0, Clip 0/1) Taktschritt je (Clip, Bild) gegen die Tafel des NB5-Laufs, |Gier-Delta|. */
typedef struct { const char *name; int32_t x, y, z; int weg, von; } g2r_lage_t;
static const g2r_lage_t k_re2_lagen[] = {
    { "SO-Ecke", 6734, 0, -26532, 0, 11 }, { "Nordbecken-Ost", 6732, 0, -13687, 0, 13 }, { "NO-Ecke", 4944, 0, -5868, 0, 14 },
    { "Rampe-x4428", 4428, -1800, -16300, 0, 0 }, { "Rampe-x2318", 2318, -1800, -16685, 0, 0 }, { "Rampe-Nord", 4500, -1800, -15000, 0, 1 },
    { "Leiter", -8200, 0, -25550, 0, 0 }, { "Leiterecke", -8437, 0, -26507, 0, 0 }, { "Westkanal", -6000, 0, -16000, 0, 0 },
    { "Mitte-Nord", 0, 0, -8000, 0, 0 }, { "Plattform-Sued", 1000, 0, -22500, 0, 0 },
    { "Weg-Becken", -6000, 0, -24000, 1, 0 }, { "Weg-Rampe", 2600, -1800, -16300, 2, 0 },
};
static const int32_t k_weg_becken[][2] = { { -6000, -24000 }, { -6000, -9000 }, { 5000, -9000 }, { -6000, -9000 }, { -6000, -24000 }, { 5000, -23000 } };
static const int32_t k_weg_rampe[][2] = { { 2600, -16300 }, { 6400, -16300 }, { 6400, -15200 }, { 2600, -17300 } };
#define G2R_WEG_TEMPO 50   /* PORT-WAHL des Messwegs: Figur geht 50 je Bild */
typedef struct {
    int bilder, ohne_max, fenster2, lauer, drehen, rueck, rueckzug, tauch_n, taucht, block, kam2500, kam0, rammen, kreuz, gang_n, gang_fehl, gier_max, clip1, rw_haeng;
    int block_k, kamk2500, kamk0, drehen_f, treffer;   /* drehen_f = Drehen, das ganze 120-Bilder-Fenster fern der Figur (Kopf > 4500); treffer = Bilder, in denen die Figur Schaden nahm. G2-RE2 NB2 Vorschlag 4: block = Kopf + Kanister (nur dokumentiert), block_k = nur Kopf; kam2500/kam0 = nur Kopf (Riegel), kamk* = Kopf + Kanister */
    int64_t kopf_summe; int zeit_n; double zeit[4096];
} g2r_m_t;
static const char *g2r_stand(int st) { return st == 0 ? "nb5" : st == 1 ? "f942" : st == 2 ? "neu" : "ohne"; }   /* 0 = NB5 (Pruefhaken rw_alt + v4_alt), 1 = f942f56c0 (v4_alt), 2 = Bau, 3 = Bau ohne Kanister (nach dem Anhaengen entfernt) */
static int16_t s_g2r_tafel[2][256]; static int s_g2r_blk; static int32_t s_g2r_vmin;   /* Taktschritt je (Clip 0/1, Bild) aus dem NB5-Lauf; -1 = unbekannt */
static int g2r_cmp_d(const void *a, const void *b) { const double x = *(const double *)a, y = *(const double *)b; return x < y ? -1 : x > y; }
static void g2r_weg(const g2r_lage_t *L, int t, int32_t *x, int32_t *z)
{
    const int32_t (*w)[2] = L->weg == 1 ? k_weg_becken : k_weg_rampe;
    const int n = L->weg == 1 ? (int)(sizeof k_weg_becken / sizeof k_weg_becken[0]) : (int)(sizeof k_weg_rampe / sizeof k_weg_rampe[0]);
    int64_t s = (int64_t)t * G2R_WEG_TEMPO, um = 0;
    int32_t ln[16];
    for (int i = 0; i < n; i++) { const int64_t dx = w[(i + 1) % n][0] - w[i][0], dz = w[(i + 1) % n][1] - w[i][1]; ln[i] = (int32_t)re15_squareroot0((uint32_t)(dx * dx + dz * dz)); um += ln[i]; }
    s %= um;
    for (int i = 0; i < n; i++) {
        if (s <= ln[i]) { *x = w[i][0] + (int32_t)((int64_t)(w[(i + 1) % n][0] - w[i][0]) * s / (ln[i] ? ln[i] : 1)); *z = w[i][1] + (int32_t)((int64_t)(w[(i + 1) % n][1] - w[i][1]) * s / (ln[i] ? ln[i] : 1)); return; }
        s -= ln[i];
    }
    *x = w[0][0]; *z = w[0][1];
}
static void g2r_lauf(re15_actor_t *g, re15_actor_t *pl, const g2r_lage_t *L, int bilder, int stand, g2r_m_t *m, FILE *spur)
{
    memset(m, 0, sizeof *m);
    int bi = 0; int64_t bd = -1;
    for (int i = 0; i < 5; i++) { const int64_t dx = k_fz_start[i][0] - L->x, dz = k_fz_start[i][1] - L->z; if (dx * dx + dz * dz > bd) { bd = dx * dx + dz * dz; bi = i; } }
    g->x = k_fz_start[bi][0]; g->z = k_fz_start[bi][1]; g->anchor_x = g->x; g->anchor_z = g->z;
    g2_lage_t gl = { L->name, L->x, L->y, L->z };
    int cam = L->von >= 10 ? L->von - 10 : g2_lage_kamera(&gl, L->von);   /* von >= 10: Kamera der Abnahme-4-Laeufe (state.log cam=), 10 + Cut */
    if (cam < 0) cam = 0;
    re15_gr2_rw_stat_t st0; re15_gr2_rw_stat((int)(g - g_actors), &st0);
    const int ramm0 = re15_gator_boss_rammen(), kreuz0 = re15_gator_boss_kreuz_kanister();
    int32_t h60[60][2]; int hn = 0; uint8_t biss60[60]; memset(biss60, 0, sizeof biss60);
    int32_t h120[120][2], k120[120]; int16_t d120[120]; int64_t dsum = 0; int dreh_bis = -1, dreh_bis_f = -1;
    int lauf = 0; int64_t best = -1; int32_t px0 = L->x, pz0 = L->z; int prev_mod = 0; int prev_gier = g->rot_y & 0xfff;
    static int32_t pts[640][3], ptk[640][3];
    for (int t = 0; t < bilder && (stand == 3 || re15_gr2_traegt()); t++) {
        int32_t px = L->x, pz = L->z;
        if (L->weg) g2r_weg(L, t, &px, &pz);
        pl->x = px; pl->y = L->y; pl->z = pz; pl->hp = 100; pl->hit_react = 0;
        if (L->weg == 1) cam = g2_zonen_schritt(px, pz, cam);
        g_scd.cam_id = (uint8_t)cam;
        re15_collision_set_band(re15_collision_band_from_y(L->y));
        frame(0, 0);
        m->bilder++;
        if (pl->hp < 100) m->treffer++;   /* G2-RE2 NB2: Biss oder Rammen traf die Figur in diesem Bild (hp vor dem Bild 100) */
        const int slot = (int)(g - g_actors), ph = re15_gator_boss_phase(g), mod = re15_gr2_rw_modus(slot);
        const int gier = g->rot_y & 0xfff;
        int dg = ((gier - prev_gier + 0x800) & 0xfff) - 0x800; prev_gier = gier;
        int32_t kopf[3] = { g->x, g->y, g->z }; (void)re15_gr2_bone_welt_pub(g, 6, kopf);
        const int64_t kx = (int64_t)kopf[0] - px, kz = (int64_t)kopf[2] - pz, k2 = kx * kx + kz * kz;
        const int32_t kd = (int32_t)re15_squareroot0((uint32_t)(k2 > 0xffffffffLL ? 0xffffffffLL : k2));
        m->kopf_summe += kd;
        /* ohne Fortschritt */
        { const int64_t mx = (int64_t)px - px0, mz = (int64_t)pz - pz0;
          if (kd <= 4500 || mx * mx + mz * mz > 1500LL * 1500 || best < 0 || kd < best - 300) { lauf = 0; best = kd; px0 = px; pz0 = pz; }
          else { lauf++; if (lauf > m->ohne_max) m->ohne_max = lauf; } }
        /* 2-s-Fenster */
        { const int biss = ((ph == 3) && kd < 2400) || re15_gator_boss_lauert(g);
          biss60[hn % 60] = (uint8_t)biss;
          int64_t weit = 0; const int32_t *a0 = h60[hn % 60];
          if (hn >= 60) { for (int k = 0; k < 60; k++) { const int64_t ax = (int64_t)h60[k][0] - a0[0], az = (int64_t)h60[k][1] - a0[1]; if (ax * ax + az * az > weit) weit = ax * ax + az * az; }
                          const int64_t ax = (int64_t)g->x - a0[0], az = (int64_t)g->z - a0[1]; if (ax * ax + az * az > weit) weit = ax * ax + az * az; }
          int b = 0; for (int k = 0; k < 60; k++) b |= biss60[k];
          if (hn >= 60 && weit < 300LL * 300 && !b && mod < 2) m->fenster2++;
          if (re15_gator_boss_lauert(g)) m->lauer++;
          h60[hn % 60][0] = g->x; h60[hn % 60][1] = g->z; }
        /* Drehen auf der Stelle */
        { const int i = hn % 120;
          if (hn >= 120) dsum -= d120[i] < 0 ? -d120[i] : d120[i];
          d120[i] = (int16_t)dg; dsum += dg < 0 ? -dg : dg; h120[i][0] = g->x; h120[i][1] = g->z; k120[i] = kd;
          if (hn >= 119 && dsum >= 4096) {
              int32_t x0 = h120[0][0], x1 = x0, z0 = h120[0][1], z1 = z0;
              for (int k = 1; k < 120; k++) { if (h120[k][0] < x0) x0 = h120[k][0]; if (h120[k][0] > x1) x1 = h120[k][0]; if (h120[k][1] < z0) z0 = h120[k][1]; if (h120[k][1] > z1) z1 = h120[k][1]; }
              if (x1 - x0 < 3000 && z1 - z0 < 3000 && kd > 4500) { const int von = dreh_bis >= hn - 119 ? dreh_bis + 1 : hn - 119; m->drehen += hn - von + 1; dreh_bis = hn; }
              int32_t kmin = k120[0]; for (int k = 1; k < 120; k++) if (k120[k] < kmin) kmin = k120[k];
              if (x1 - x0 < 3000 && z1 - z0 < 3000 && kmin > 4500) { const int von = dreh_bis_f >= hn - 119 ? dreh_bis_f + 1 : hn - 119; m->drehen_f += hn - von + 1; dreh_bis_f = hn; }
          } }
        hn++;
        if (mod == 1) m->rueck++;
        if (ph == 7 && g->motion == 10) m->rueckzug++;
        if (mod >= 2) m->taucht++;
        if (mod == 2 && prev_mod != 2) m->tauch_n++;
        prev_mod = mod;
        /* Kopf/Kanister in Rampe/Plattform, vor der Kamera */
        { const int n = mod == 3 ? 0 : re15_gr2_rw_kopf_punkte(g, pts, 640);        /* Mesh 5/6/7 + Kanister-Zylinder */
          const int nk = mod == 3 ? 0 : re15_gr2_rw_nur_kopf_punkte(g, ptk, 640);   /* G2-RE2 NB2 Vorschlag 4: nur Mesh 5/6/7 (Kopf-Riegel) */
          int im_block = 0, im_block_k = 0; int32_t vmin = 1 << 30, vmin_k = 1 << 30;
          for (int i = 0; i < n; i++) {
              if (((pts[i][0] > -1700 && pts[i][0] < 1850 && pts[i][2] > -20000 && pts[i][2] < -12450)
                                        || (pts[i][0] > 1850 && pts[i][0] < 7200 && pts[i][2] > -18100 && pts[i][2] < -14500))) im_block = 1;
              const int32_t vz = re15_gr2_kamera_tiefe(cam, pts[i]); if (vz < vmin) vmin = vz;
          }
          for (int i = 0; i < nk; i++) {
              if (((ptk[i][0] > -1700 && ptk[i][0] < 1850 && ptk[i][2] > -20000 && ptk[i][2] < -12450)
                                        || (ptk[i][0] > 1850 && ptk[i][0] < 7200 && ptk[i][2] > -18100 && ptk[i][2] < -14500))) im_block_k = 1;
              const int32_t vz = re15_gr2_kamera_tiefe(cam, ptk[i]); if (vz < vmin_k) vmin_k = vz;
          }
          if (ph == 7) im_block = im_block_k = 0;   /* Ueberquerung: der Hub traegt ihn ueber das Deck (gewollt) */
          m->block += im_block; m->block_k += im_block_k; m->kam2500 += vmin_k < 2500; m->kam0 += vmin_k <= 0; m->kamk2500 += vmin < 2500; m->kamk0 += vmin <= 0;
          s_g2r_blk = im_block; s_g2r_vmin = vmin_k; }
        /* Gang */
        { int32_t sdx = 0, sdz = 0; re15_gator_boss_takt_schritt(&sdx, &sdz);
          const int32_t sl = (int32_t)re15_squareroot0((uint32_t)((int64_t)sdx * sdx + (int64_t)sdz * sdz));
          if (ph == 2 && mod == 0 && g->motion <= 1 && g->anim_frame < 256 && !re15_gator_boss_lauert(g)) {
              m->clip1 += g->motion == 1;
              int32_t soll = re15_gr2_vortrieb_soll(g, g->motion, g->anim_frame); if (soll < 0) soll = -soll;
              const int32_t soll44 = soll > 44 ? soll - 44 : 44 - soll;   /* NB5-Wende nahe der Figur: zusaetzlich 44 zurueck (CHASE "RUECKWAERTSGANG") */
              m->gang_n++; if ((sl - soll > 3 || soll - sl > 3) && (sl - soll44 > 3 || soll44 - sl > 3)) m->gang_fehl++;
              if ((dg < 0 ? -dg : dg) > m->gier_max) m->gier_max = dg < 0 ? -dg : dg;
          } }
        { int64_t ns = 0, zy = 0; re15_gator_boss_zeit_letzt(&ns, &zy); if (m->zeit_n < 4096) m->zeit[m->zeit_n++] = (double)zy;
          if (getenv("G2_RE2_ZEIT_ZEIGE") && ns > 1500000) printf("        ZEIT %s t%d %.2f ms (Faden %lld Zyklen) ph %d mod %d clip %d\n", L->name, t, ns / 1e6, (long long)zy, ph, mod, (int)g->motion); }
        if (spur) { int32_t zx = 0, zz = 0; re15_gator_boss_ziel(g, &zx, &zz);
                    fprintf(spur, "%s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", L->name, g2r_stand(stand), t, g->x, g->z, gier, ph, mod, (int)g->motion,
                            (int)g->anim_frame, px, pz, cam, kd, zx, zz, re15_gr2_kk_aktiv(), re15_gr2_trag_modus((int)(g - g_actors)), s_g2r_blk, (int)s_g2r_vmin, g->y); }
    }
    re15_gr2_rw_stat_t st1; re15_gr2_rw_stat((int)(g - g_actors), &st1);
    m->rw_haeng = (int)(st1.haenger - st0.haenger);
    m->rammen = re15_gator_boss_rammen() - ramm0; m->kreuz = re15_gator_boss_kreuz_kanister() - kreuz0;
    const double zj = re15_g2_zeit_zyk_je_us();
    for (int i = 0; i < m->zeit_n; i++) m->zeit[i] = zj > 0 ? m->zeit[i] / zj : 0;
}
typedef struct { int bilder, ohne_max, fenster2, lauer, drehen, rueck, rueckzug, tauch_n, taucht, block, kam2500, kam0, rammen, kreuz, gang_n, gang_fehl, gier_max, block_k, kamk2500, kamk0, drehen_f, treffer; double z_mittel, z_p95, z_max; } g2r_sum_t;
static void g2r_zeit(g2r_m_t *m, double *mi, double *p95, double *mx)
{
    if (!m->zeit_n) { *mi = *p95 = *mx = 0; return; }
    double s = 0; for (int i = 0; i < m->zeit_n; i++) s += m->zeit[i];
    *mi = s / m->zeit_n;
    qsort(m->zeit, (size_t)m->zeit_n, sizeof m->zeit[0], g2r_cmp_d);
    *p95 = m->zeit[(m->zeit_n * 95) / 100]; *mx = m->zeit[m->zeit_n - 1];
}
/* je Stand (0 = nb5, 1 = f942f56c0, 2 = neu) alle Lagen; Rueckgabe: Summen */
static int g2r_alle(int elza, int stand, int bilder, g2r_sum_t *S, g2r_m_t *M, int nm, FILE *spur, const char *nur)
{
    memset(S, 0, sizeof *S);
    re15_gr2_rw_alt_setzen(stand == 0); re15_gr2_v4_alt_setzen(stand <= 1);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    const int nl = (int)(sizeof k_re2_lagen / sizeof k_re2_lagen[0]);
    static double alle_z[64 * 4096]; int nz = 0;
    for (int i = 0; i < nl && i < nm; i++) {
        if (nur && strcmp(nur, k_re2_lagen[i].name)) { M[i].bilder = 0; continue; }
        /* G2-RE2 NB2: je Lage ein frischer Kampf (gleicher Anfangszustand in allen Staenden; vorher liefen die Lagen nacheinander im selben Kampf) */
        re15_actor_t *g = szene_bis_kampf(elza);
        if (!g || !g2_kanister_ins_maul(g, pl)) { printf("    %s %s: kein Tragen\n", g2r_stand(stand), k_re2_lagen[i].name); re15_gr2_rw_alt_setzen(0); re15_gr2_v4_alt_setzen(0); return 0; }
        if (stand == 3) re15_gr2_kanister_entfernen();   /* derselbe Kampf ohne Kanister (Bezug der Kopf-Riegel) */
        g2r_lauf(g, pl, &k_re2_lagen[i], bilder, stand, &M[i], spur);
        g2r_m_t *m = &M[i];
        for (int k = 0; k < m->zeit_n && nz < 64 * 4096; k++) alle_z[nz++] = m->zeit[k];
        double mi, p95, mx; g2r_zeit(m, &mi, &p95, &mx);
        const double min_s = m->bilder / 1800.0;
        printf("    %s %-15s %4d B.: ohne Fortschritt max %5.1f s, 2-s-Fenster %3d, Drehen %3d %%, rueckw. %3d %% (+Rueckzug %2d %%), Tauch %d (%.1f/min) unter %2d %%, Block Kopf %3d %% (+Kanister %3d %%), "
               "Kopf Kam<2500 %3d %%, hinter %3d %% (+Kanister %3d %% / %3d %%), Kopf %5d, Drehen ganz fern %3d %%, Treffer %d, Rammen %d/Querung %d, Haenger %d, Gang %d/%d abw., Gier max %d, Takt %.0f/%.0f/%.0f us\n",
               g2r_stand(stand), k_re2_lagen[i].name, m->bilder, m->ohne_max / 30.0, m->fenster2, m->bilder ? 100 * m->drehen / m->bilder : 0, m->bilder ? 100 * m->rueck / m->bilder : 0, m->bilder ? 100 * m->rueckzug / m->bilder : 0,
               m->tauch_n, min_s > 0 ? m->tauch_n / min_s : 0, m->bilder ? 100 * m->taucht / m->bilder : 0, m->bilder ? 100 * m->block_k / m->bilder : 0, m->bilder ? 100 * m->block / m->bilder : 0,
               m->bilder ? 100 * m->kam2500 / m->bilder : 0, m->bilder ? 100 * m->kam0 / m->bilder : 0, m->bilder ? 100 * m->kamk2500 / m->bilder : 0, m->bilder ? 100 * m->kamk0 / m->bilder : 0,
               m->bilder ? (int)(m->kopf_summe / m->bilder) : 0, m->bilder ? 100 * m->drehen_f / m->bilder : 0, m->treffer, m->rammen, m->kreuz, m->rw_haeng,
               m->gang_fehl, m->gang_n, m->gier_max, mi, p95, mx);
        S->bilder += m->bilder; if (m->ohne_max > S->ohne_max) S->ohne_max = m->ohne_max; S->fenster2 += m->fenster2; S->drehen += m->drehen; S->rueck += m->rueck; S->rueckzug += m->rueckzug;
        S->tauch_n += m->tauch_n; S->taucht += m->taucht; S->block += m->block; S->kam2500 += m->kam2500; S->kam0 += m->kam0; S->rammen += m->rammen; S->kreuz += m->kreuz;
        S->gang_n += m->gang_n; S->gang_fehl += m->gang_fehl; if (m->gier_max > S->gier_max) S->gier_max = m->gier_max;
        S->block_k += m->block_k; S->kamk2500 += m->kamk2500; S->kamk0 += m->kamk0; S->drehen_f += m->drehen_f; S->treffer += m->treffer;
    }
    if (nz) { double s = 0; for (int k = 0; k < nz; k++) s += alle_z[k]; S->z_mittel = s / nz; qsort(alle_z, (size_t)nz, sizeof alle_z[0], g2r_cmp_d); S->z_p95 = alle_z[(nz * 95) / 100]; S->z_max = alle_z[nz - 1]; }
    re15_gr2_rw_alt_setzen(0); re15_gr2_v4_alt_setzen(0);
    return 1;
}
static void g2r_summe_druck(const char *w, const g2r_sum_t *S)
{
    const double min = S->bilder / 1800.0;
    printf("    SUMME %s: %d Bilder (%.1f min): ohne Fortschritt max %.1f s, 2-s-Fenster %d, Drehen %.1f %%, rueckwaerts %.1f %% (Rueckzug nach Vorstoss %.1f %%), Tauchgaenge %d = %.2f/min, untergetaucht %.1f %%, "
           "Kopf in Rampe/Plattform %.1f %% (mit Kanister %.1f %%), Kopf < 2500 vor der Kamera %.1f %%, hinter der Kamera %.1f %% (mit Kanister %.1f %% / %.1f %%), Rammen %d (Querungen %d), Gang %d von %d abweichend, "
           "Gier max %d, Takt-Rechenzeit Mittel %.0f us / 95 %% %.0f us / max %.0f us\n", w, S->bilder, min, S->ohne_max / 30.0, S->fenster2,
           S->bilder ? 100.0 * S->drehen / S->bilder : 0, S->bilder ? 100.0 * S->rueck / S->bilder : 0, S->bilder ? 100.0 * S->rueckzug / S->bilder : 0, S->tauch_n, min > 0 ? S->tauch_n / min : 0,
           S->bilder ? 100.0 * S->taucht / S->bilder : 0, S->bilder ? 100.0 * S->block_k / S->bilder : 0, S->bilder ? 100.0 * S->block / S->bilder : 0, S->bilder ? 100.0 * S->kam2500 / S->bilder : 0,
           S->bilder ? 100.0 * S->kam0 / S->bilder : 0, S->bilder ? 100.0 * S->kamk2500 / S->bilder : 0, S->bilder ? 100.0 * S->kamk0 / S->bilder : 0,
           S->rammen, S->kreuz, S->gang_fehl, S->gang_n, S->gier_max, S->z_mittel, S->z_p95, S->z_max);
}
/* Riegel re2(_elza): Hängen, Drehen, Kopf in Rampe/Kamera, Rammen, Tauch-Obergrenze, Gang (mit Kanister) - je mit Gegenprobe */
#define G2R_BILDER 1800
static void teil_re2(int elza)
{
    /* G2-RE2 NB2 "Vorschlag 4" (Dossier G2_gatorfix.md Abschnitt "G2-RE2 NB2 (Vorschlag 4)"): Stand "f942" = Pruefhaken re15_gr2_v4_alt_setzen(1) (der Kanister zaehlt,
     * Stand f942f56c0), Stand "neu" = Bau (der Kanister zaehlt nicht). Die Kopf-Riegel der NB1 (Auftrag Punkt 4c) messen nur noch Mesh 5/6/7; Kopf + Kanister in
     * Rampe/Plattform und vor der Kamera stehen nur noch zur Dokumentation in der Ausgabe (Nutzerentscheid: der Kanister darf in die Geometrie ragen). */
    printf("== re2%s (G2-RE2 NB2 Vorschlag 4: Kopf-Riegel, f942f56c0 gegen neu) ==\n", elza ? "_elza" : "");
    const char *bs = getenv("G2_RE2_BILDER"); const int bilder = bs ? atoi(bs) : G2R_BILDER;
    const char *nur = getenv("G2_RE2_NUR");
    FILE *spur = getenv("G2_RE2_SPUR") ? fopen(getenv("G2_RE2_SPUR"), "w") : NULL;
    static g2r_m_t M[3][16];
    g2r_sum_t S[3];
    memset(s_g2r_tafel, 0xff, sizeof s_g2r_tafel);
    re15_gr2_rw_stat_null();
    const int ok0 = g2r_alle(elza, 1, bilder, &S[0], M[0], 16, spur, nur);
    const int ok1 = g2r_alle(elza, 2, bilder, &S[1], M[1], 16, spur, nur);
    const int ok2 = g2r_alle(elza, 3, bilder, &S[2], M[2], 16, spur, nur);
    if (spur) fclose(spur);
    g2r_summe_druck("f942", &S[0]); g2r_summe_druck("neu", &S[1]); g2r_summe_druck("ohne", &S[2]);
    const double min = S[1].bilder / 1800.0;
    PRUEF(ok0 && ok1 && ok2 && S[1].bilder >= (nur ? 1 : 12) * bilder * 9 / 10, "gelaufen (neu %d Bilder mit Kanister, ohne %d)", S[1].bilder, S[2].bilder);
    printf("    (Dokumentation, kein Riegel: 2-s-Fenster ohne Fortschritt neu %d, f942 %d - der Angriff an der Figur ohne Kanister (Biss Kopf 2400..3600) zaehlt hier mit;"
           " Kopf + Kanister unter dem Deck in Rampe/Plattform neu %d Bilder = %.1f %%, f942 %d)\n",
           S[1].fenster2, S[0].fenster2, S[1].block, S[1].bilder ? 100.0 * S[1].block / S[1].bilder : 0, S[0].block);
    {   int ohne_haeng = 0;   /* Lagen der Abnahme-4-Haenger mit erreichbarer Figur: SO-Ecke (frei4_leon) und die Rampe */
        for (int i = 0; i < 16; i++) if (M[1][i].bilder && (!strcmp(k_re2_lagen[i].name, "SO-Ecke") || k_re2_lagen[i].y != 0) && M[1][i].ohne_max > ohne_haeng) ohne_haeng = M[1][i].ohne_max;
        PRUEF(ohne_haeng <= 20 * 30, "Kopf-Riegel Haengen: SO-Ecke und Rampe - laengste Zeit ohne Fortschritt zur Figur <= 20 s (neu %.1f s; alle Lagen neu %.1f s, f942 %.1f s)", ohne_haeng / 30.0,
              S[1].ohne_max / 30.0, S[0].ohne_max / 30.0); }
    {   /* Drehen: das alte Mass zaehlt ein 120-Bilder-Fenster, wenn der Kopf nur im LETZTEN Bild > 4500 von der Figur ist - mit Vorschlag 4 erreicht er die Figur und
         * kreist beim Beissen (Treffer je Lage in der Ausgabe); "fern der Figur" heisst jetzt: im ganzen Fenster Kopf > 4500 (drehen_f). Je Lage hoechstens 15 %. */
        int fmax = 0, fmax0 = 0; const char *fn = "-", *fn0 = "-";
        for (int i = 0; i < 16; i++) {
            if (M[1][i].bilder && 100 * M[1][i].drehen_f > fmax * M[1][i].bilder) { fmax = 100 * M[1][i].drehen_f / M[1][i].bilder; fn = k_re2_lagen[i].name; }
            if (M[0][i].bilder && 100 * M[0][i].drehen_f > fmax0 * M[0][i].bilder) { fmax0 = 100 * M[0][i].drehen_f / M[0][i].bilder; fn0 = k_re2_lagen[i].name; }
        }
        printf("    (Dokumentation: Drehen altes Mass neu %.1f %%, ohne Kanister %.1f %%, f942 %.1f %%)\n", 100.0 * S[1].drehen / S[1].bilder, S[2].bilder ? 100.0 * S[2].drehen / S[2].bilder : 0,
               S[0].bilder ? 100.0 * S[0].drehen / S[0].bilder : 0);
        PRUEF(fmax <= 15, "Kopf-Riegel Drehen auf der Stelle fern der Figur (ganzes 120-Bilder-Fenster Kopf > 4500) je Lage <= 15 %% (neu hoechstens %d %% %s)", fmax, fn);
        if (!nur) PRUEF(fmax0 > 15, "Gegenprobe f942f56c0: Drehen fern der Figur in einer Lage > 15 %% (%d %% %s) - das Mass sieht das Haengen", fmax0, fn0);
    }
    /* Kopf vor der Kamera: die NB1-Schranke (<= 8 %% / <= 0,1 %%) war eine G2-RE2-Regel fuer Kopf + Kanister (Klemme vor der Kamera); ohne Kanister gab und gibt es sie
     * nicht (NB5). Mit Vorschlag 4 ist das Mass deshalb das des Gangs ohne Kanister - Riegel: mit Kanister nicht haeufiger als ohne Kanister im selben Kampf; die
     * absoluten Werte gegen die alte Schranke stehen in der Ausgabe (Dossier "Offen"). */
    printf("    (Dokumentation Kopf vor der Kamera gegen die NB1-Schranke 8 %% / 0,1 %%: neu %.1f %% / %.1f %%, ohne Kanister %.1f %% / %.1f %%, f942 %.1f %% / %.1f %%)\n",
           100.0 * S[1].kam2500 / (S[1].bilder ? S[1].bilder : 1), 100.0 * S[1].kam0 / (S[1].bilder ? S[1].bilder : 1), 100.0 * S[2].kam2500 / (S[2].bilder ? S[2].bilder : 1),
           100.0 * S[2].kam0 / (S[2].bilder ? S[2].bilder : 1), 100.0 * S[0].kam2500 / (S[0].bilder ? S[0].bilder : 1), 100.0 * S[0].kam0 / (S[0].bilder ? S[0].bilder : 1));
    PRUEF((int64_t)S[1].kam2500 * S[2].bilder <= (int64_t)S[2].kam2500 * S[1].bilder && (int64_t)S[1].kam0 * S[2].bilder <= (int64_t)S[2].kam0 * S[1].bilder,
          "Kopf-Riegel Kopf (Mesh 5/6/7) < 2500 vor / hinter der Kamera mit Kanister nicht haeufiger als ohne Kanister (neu %d / %d, ohne %d / %d Bilder)", S[1].kam2500, S[1].kam0, S[2].kam2500, S[2].kam0);
    PRUEF(S[1].rueck * 100 <= S[1].bilder * 12, "Rueckfall rueckwaerts setzen hoechstens 12 %% (neu %.1f %%, f942 %.1f %%)", S[1].bilder ? 100.0 * S[1].rueck / S[1].bilder : 0,
          S[0].bilder ? 100.0 * S[0].rueck / S[0].bilder : 0);
    PRUEF(min > 0 && S[1].tauch_n <= min + 0.0001 && S[1].taucht * 20 < S[1].bilder, "Rueckfall Tauchen: <= 1 je Minute und < 5 %% untergetaucht (neu %d in %.1f min, %.1f %%)",
          S[1].tauch_n, min, S[1].bilder ? 100.0 * S[1].taucht / S[1].bilder : 0);
    /* Gang: mit Vorschlag 4 gilt "mit Kanister = ohne Kanister" (Riegel v4, Bild fuer Bild) und "ohne Kanister = NB5" (Riegel gang) - hier nur dokumentiert */
    printf("    (Dokumentation: Taktschritt gegen den authored RE2-Vortrieb abweichend neu %d von %d, f942 %d von %d; |Gier-Delta| max neu %d)\n",
           S[1].gang_fehl, S[1].gang_n, S[0].gang_fehl, S[0].gang_n, S[1].gier_max);
    PRUEF(S[1].gier_max <= 0x60, "Gier je Bild in der Verfolgung <= 0x60 (Port-Wenderate, GIER-WACHE; neu %d)", S[1].gier_max);
    if (!nur) {
        /* Rampe "rammen wie gehabt, keine Deckung" (Vorschlag 4: Belagerung wie ohne Kanister + Querung ueber die Figur, wenn sie von der Kamera weg fuehrt):
         * in jeder Rampenlage wird die Figur getroffen (Hochbiss oder Rammen), in jeder Lage mit einer Querung rammt er, insgesamt mindestens ein Rammen; an der
         * Lage, die ohne Kanister fast sicher ist (Rampe-x2318 = Abnahme 4 west5_elza), trifft er mit dem Kanister oefter als ohne. */
        int rampe = 0, treff_ok = 1, ramm_ok = 1, ramm = 0, t2318 = -1, t2318_o = -1;
        for (int i = 0; i < 16; i++) if (M[1][i].bilder && k_re2_lagen[i].y != 0) {
            rampe++; ramm += M[1][i].rammen;
            if (M[1][i].treffer < 1) treff_ok = 0;
            if (M[1][i].kreuz > 0 && M[1][i].rammen < 1) ramm_ok = 0;
            if (!strcmp(k_re2_lagen[i].name, "Rampe-x2318")) { t2318 = M[1][i].treffer; t2318_o = M[2][i].treffer; }
        }
        PRUEF(rampe >= 3 && treff_ok, "Kopf-Riegel Rampe (keine Deckung): in jeder Lage mit der Figur oben mindestens ein Treffer (%d Lagen)", rampe);
        PRUEF(ramm >= 1 && ramm_ok, "Kopf-Riegel Rampe: Rammen in jeder Lage mit einer Querung, zusammen %d", ramm);
        PRUEF(t2318 > t2318_o, "Rampe-x2318 (ohne Kanister fast sicher): Treffer mit Kanister %d > ohne Kanister %d", t2318, t2318_o);
        int r0 = 0; for (int i = 0; i < 16; i++) if (M[2][i].bilder && k_re2_lagen[i].y != 0) r0 += M[2][i].rammen;
        PRUEF(r0 == 0, "Gegenprobe ohne Kanister: kein Rammen auf der Rampe (%d) - die Querung mit Rammen gehoert zum Kanister (Auftrag)", r0);
    }
}
/* Riegel re2_gegen(_elza): Gegenproben der Bausteine - ohne Rammen (Pruefhaken), ohne Rueckwaerts, mit RE1.5-Tempo: muss rot sein */
static void teil_re2_gegen(int elza)
{
    printf("== re2_gegen%s ==\n", elza ? "_elza" : "");
    const char *bs = getenv("G2_RE2_BILDER"); const int bilder = bs ? atoi(bs) : 900;
    static g2r_m_t M[16]; g2r_sum_t S;
    re15_gr2_rw_stat_null();
    memset(s_g2r_tafel, 0xff, sizeof s_g2r_tafel);
    re15_gator_boss_tempo_alt_setzen(1);
    (void)g2r_alle(elza, 2, bilder, &S, M, 16, NULL, "Westkanal");
    re15_gator_boss_tempo_alt_setzen(0);
    PRUEF(S.gang_fehl * 10 > S.gang_n, "Gegenprobe RE1.5-Tempo: Gang weicht vom authored RE2-Vortrieb ab (%d von %d)", S.gang_fehl, S.gang_n);
    re15_gator_boss_ramm_alt_setzen(1);
    (void)g2r_alle(elza, 2, bilder, &S, M, 16, NULL, "Rampe-x4428");
    re15_gator_boss_ramm_alt_setzen(0);
    PRUEF(S.rammen == 0, "Gegenprobe ohne Rammen: 0 Rammen auf der Rampe (%d)", S.rammen);
    re15_gr2_rw_rueck_aus_setzen(1); re15_gr2_rw_tauchen_aus_setzen(1);
    (void)g2r_alle(elza, 2, bilder, &S, M, 16, NULL, "SO-Ecke");
    re15_gr2_rw_rueck_aus_setzen(0); re15_gr2_rw_tauchen_aus_setzen(0);
    printf("    ohne Rueckwaerts/Tauchen SO-Ecke: 2-s-Fenster %d, ohne Fortschritt %.1f s\n", S.fenster2, S.ohne_max / 30.0);
}
/* Riegel gang(_elza): ohne Kanister Bild fuer Bild gleich NB5 (Lage, Gier, Clip, Bild, Hoehe); Gegenprobe Pruefhaken Tempo 2 (RE1.5-Tempo auch ohne Kanister) */
static int g2r_ohne_lauf(int elza, int alt, int tempo, int32_t (*z)[6], int n)
{
    re15_gr2_rw_alt_setzen(alt); re15_gator_boss_tempo_alt_setzen(tempo);
    re15_actor_t *g = szene_bis_kampf(elza);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int k = 0;
    if (g) {
        static const int32_t lage[3][3] = { { -6000, 0, -16000 }, { 6732, 0, -13687 }, { 4428, -1800, -16300 } };
        for (int li = 0; li < 3; li++)
            for (int t = 0; t < n / 3 && k < n; t++) {
                pl->x = lage[li][0]; pl->y = lage[li][1]; pl->z = lage[li][2]; pl->hp = 100; pl->hit_react = 0;
                re15_collision_set_band(re15_collision_band_from_y(lage[li][1]));
                frame(0, 0);
                if (re15_gr2_traegt()) break;
                z[k][0] = g->x; z[k][1] = g->z; z[k][2] = g->rot_y & 0xfff; z[k][3] = g->motion; z[k][4] = g->anim_frame; z[k][5] = g->y; k++;
            }
    }
    re15_gr2_rw_alt_setzen(0); re15_gator_boss_tempo_alt_setzen(0);
    return k;
}
static void teil_gang(int elza)
{
    printf("== gang%s (G2-RE2: Gang ohne Kanister gleich NB5) ==\n", elza ? "_elza" : "");
    enum { N = 2700 };
    static int32_t a[N][6], b[N][6], c[N][6];
    const int na = g2r_ohne_lauf(elza, 1, 0, a, N), nb = g2r_ohne_lauf(elza, 0, 0, b, N), nc = g2r_ohne_lauf(elza, 0, 2, c, N);
    int diff = 0, diff_c = 0, clip1 = 0;
    for (int i = 0; i < na && i < nb; i++) { if (memcmp(a[i], b[i], sizeof a[i])) diff++; clip1 += b[i][3] == 1; }
    for (int i = 0; i < na && i < nc; i++) if (memcmp(a[i], c[i], sizeof a[i])) diff_c++;
    printf("    nb5 %d Bilder, neu %d Bilder (Clip 1 in %d), abweichend %d; Gegenprobe RE1.5-Tempo %d Bilder, abweichend %d\n", na, nb, clip1, diff, nc, diff_c);
    PRUEF(na >= N * 9 / 10 && na == nb && diff == 0, "ohne Kanister Bild fuer Bild gleich NB5 (Lage, Gier, Clip, Bild, Hoehe): %d von %d abweichend", diff, na);
    PRUEF(diff_c > 0, "Gegenprobe RE1.5-Tempo ohne Kanister: abweichend %d (rot ohne den Bau)", diff_c);
}

/* ==== Runde 41 Spur G2-RE2 NB2 "Vorschlag 4": Riegel v4(_elza) =====================================================================================
 * Dossier G2_gatorfix.md Abschnitt "G2-RE2 NB2 (Vorschlag 4)". Nutzer: "Ignoriere den Kanister einfach in der Berechnung. Tu so als ob er nicht da waere" /
 * "wichtig ist nur das der Kanister synchron mit dem Aligator mitlaeuft".
 *  (a) BEWEGUNG MIT = OHNE KANISTER: je Stand ein frischer Kampf (szene_bis_kampf), der Kanister wird ins Maul gegeben (g2_kanister_ins_maul, bis Stufe 7); dann
 *      "mit" = so weiter, "ohne" = re15_gr2_kanister_entfernen() im selben Bild (Lage des Alligators bis dahin bitgleich), "huelle" = mit + Pruefhaken
 *      re15_gr2_v4_alt_setzen(1) (Stand f942f56c0: der Kanister zaehlt). Alle 13 Lagen des Riegels re2 nacheinander (Start, Figur, Kamera wie g2r_lauf), je
 *      G2V4_BILDER Bilder; je Bild Lage x/y/z, Gier, Clip, Bild, Phase. Rammen aus (Pruefhaken re15_gator_boss_ramm_alt_setzen(1)): das Rammen mit dem Kanister ist
 *      ein Angriff (Riegel re2 "Rammen auf der Rampe"), kein Teil der Bewegung, und gibt es ohne Kanister nicht. Soll: mit = ohne in jedem Bild; huelle != ohne (rot).
 *  (b) HALTERUNG: in jedem Bild "mit" ist der Kanister genau Part 6 (Pose wie der Zeichner, g2_pose / g2_part_welt - eigene Rechnung im Riegel) x Lokallage/Relation
 *      des Moduls: |Lage - (p6 + R6 x s x lokal)| und |Drehung - R6 x rel| je Komponente = 0. Gegenprobe Pruefhaken re15_gr2_kan_vorbild_setzen(1) (Part 6 des
 *      Vorbilds = "ein Bild alte Pose"): > 0 (rot). Dazu: in wie vielen Bildern die Lokallage selbst wandert (RE2 Stufe 7 fuehrt sie heran @0x80101b58-8c). */
#define G2V4_BILDER 900
typedef struct { int32_t x, y, z; int16_t gier, clip, bild, ph; } g2v4_z_t;
static g2v4_z_t s_v4z[3][16][G2V4_BILDER];
static int s_v4n[3][16];
typedef struct { int bilder, abw_bilder, rw_bilder, lokal_wechsel, lokal_letzt; int32_t abw_pos, abw_rot; } g2v4_halt_t;
static int g2v4_halt(re15_actor_t *g, int32_t *ap, int32_t *ar, int32_t lok[3])
{
    re15_skel_pose_t ps[RE15_EMD_MAX_BONES]; int32_t r6[9], p6[3], l[3], rel[9], kp[3], kr[9];
    if (!g2_pose(g, ps) || !re15_gr2_kanister_lokal(l) || !re15_gr2_kanister_rel(rel) || !re15_gr2_kanister_welt(kp, kr)) return 0;
    g2_part_welt(g, &ps[6], r6, p6);
    const int32_t s = g->render_scale_q12 ? g->render_scale_q12 : 4096;
    const int32_t ls[3] = { g2_q12(l[0], s), g2_q12(l[1], s), g2_q12(l[2], s) };
    *ap = *ar = 0;
    for (int r = 0; r < 3; r++) {
        const int32_t soll = p6[r] + (int32_t)(((int64_t)r6[r*3] * ls[0] + (int64_t)r6[r*3+1] * ls[1] + (int64_t)r6[r*3+2] * ls[2]) >> 12);
        int32_t d = soll - kp[r]; if (d < 0) d = -d; if (d > *ap) *ap = d;
        for (int c = 0; c < 3; c++) {
            const int32_t sr = (int32_t)(((int64_t)r6[r*3] * rel[c] + (int64_t)r6[r*3+1] * rel[3+c] + (int64_t)r6[r*3+2] * rel[6+c]) >> 12);
            int32_t dr = sr - kr[r*3+c]; if (dr < 0) dr = -dr; if (dr > *ar) *ar = dr;
        }
    }
    memcpy(lok, l, 12);
    return 1;
}
/* eine Lage wie g2r_lauf (Start, Figur, Kamera); z = Zustand je Bild, h = Halterung (nur "mit") */
static int g2v4_lage(re15_actor_t *g, re15_actor_t *pl, const g2r_lage_t *L, int bilder, g2v4_z_t *z, g2v4_halt_t *h, int mit)
{
    int bi = 0; int64_t bd = -1;
    for (int i = 0; i < 5; i++) { const int64_t dx = k_fz_start[i][0] - L->x, dz = k_fz_start[i][1] - L->z; if (dx * dx + dz * dz > bd) { bd = dx * dx + dz * dz; bi = i; } }
    g->x = k_fz_start[bi][0]; g->z = k_fz_start[bi][1]; g->anchor_x = g->x; g->anchor_z = g->z;
    g2_lage_t gl = { L->name, L->x, L->y, L->z };
    int cam = L->von >= 10 ? L->von - 10 : g2_lage_kamera(&gl, L->von);
    if (cam < 0) cam = 0;
    int n = 0; int32_t lok_alt[3] = { 0, 0, 0 }; int lok_ok = 0;
    for (int t = 0; t < bilder; t++) {
        if (mit && !re15_gr2_traegt()) break;
        int32_t px = L->x, pz = L->z;
        if (L->weg) g2r_weg(L, t, &px, &pz);
        pl->x = px; pl->y = L->y; pl->z = pz; pl->hp = 100; pl->hit_react = 0;
        if (L->weg == 1) cam = g2_zonen_schritt(px, pz, cam);
        g_scd.cam_id = (uint8_t)cam;
        re15_collision_set_band(re15_collision_band_from_y(L->y));
        frame(0, 0);
        z[n].x = g->x; z[n].y = g->y; z[n].z = g->z; z[n].gier = (int16_t)(g->rot_y & 0xfff); z[n].clip = (int16_t)g->motion; z[n].bild = (int16_t)g->anim_frame;
        z[n].ph = (int16_t)re15_gator_boss_phase(g); n++;
        if (h && mit) {
            int32_t ap = 0, ar = 0, lok[3];
            if (g2v4_halt(g, &ap, &ar, lok)) {
                h->bilder++;
                if (ap > h->abw_pos) h->abw_pos = ap;
                if (ar > h->abw_rot) h->abw_rot = ar;
                if (ap || ar) h->abw_bilder++;
                if (lok_ok && memcmp(lok, lok_alt, sizeof lok_alt)) { h->lokal_wechsel++; h->lokal_letzt = h->bilder; }
                memcpy(lok_alt, lok, sizeof lok_alt); lok_ok = 1;
            }
            if (re15_gr2_rw_modus((int)(g - g_actors)) != 0) h->rw_bilder++;
        }
    }
    return n;
}
/* Stand: 0 = mit, 1 = ohne (entfernt), 2 = mit + Huelle (Pruefhaken v4_alt). Je Lage ein frischer Kampf (gleicher Anfangszustand in allen Staenden). */
static int g2v4_stand(int elza, int st, int bilder, g2v4_halt_t *h)
{
    int ok = 1;
    const int nl = (int)(sizeof k_re2_lagen / sizeof k_re2_lagen[0]);
    for (int i = 0; i < nl && i < 16; i++) {
        re15_gr2_v4_alt_setzen(0); re15_gr2_kan_vorbild_setzen(0); re15_gator_boss_ramm_alt_setzen(1);
        re15_actor_t *g = szene_bis_kampf(elza);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        s_v4n[st][i] = 0;
        if (!g || !g2_kanister_ins_maul(g, pl)) { ok = 0; continue; }
        if (st == 1) re15_gr2_kanister_entfernen();
        if (st == 2) re15_gr2_v4_alt_setzen(1);
        s_v4n[st][i] = g2v4_lage(g, pl, &k_re2_lagen[i], bilder, s_v4z[st][i], h, st != 1);
    }
    re15_gr2_v4_alt_setzen(0); re15_gator_boss_ramm_alt_setzen(0);
    return ok;
}
static int g2v4_vergleich(int a, int b, int i, int *erstes)
{
    int d = 0; *erstes = -1;
    const int n = s_v4n[a][i] < s_v4n[b][i] ? s_v4n[a][i] : s_v4n[b][i];
    for (int t = 0; t < n; t++) if (memcmp(&s_v4z[a][i][t], &s_v4z[b][i][t], sizeof(g2v4_z_t))) { d++; if (*erstes < 0) *erstes = t; }
    return d + (s_v4n[a][i] > s_v4n[b][i] ? s_v4n[a][i] - s_v4n[b][i] : s_v4n[b][i] - s_v4n[a][i]);
}
static void teil_v4(int elza)
{
    printf("== v4%s (G2-RE2 NB2 Vorschlag 4: Bewegung mit = ohne Kanister, Halterung am Kiefer) ==\n", elza ? "_elza" : "");
    const char *bs = getenv("G2_V4_BILDER"); int bilder = bs ? atoi(bs) : G2V4_BILDER; if (bilder > G2V4_BILDER) bilder = G2V4_BILDER;
    g2v4_halt_t h; memset(&h, 0, sizeof h);
    re15_gr2_rw_stat_null();
    const int ok0 = g2v4_stand(elza, 0, bilder, &h), ok1 = g2v4_stand(elza, 1, bilder, NULL), ok2 = g2v4_stand(elza, 2, bilder, NULL);
    const int nl = (int)(sizeof k_re2_lagen / sizeof k_re2_lagen[0]);
    int summe = 0, alle = 0, diff = 0, diff_h = 0, diff_r = 0, bild_r = 0, rampe_vor_querung = 0, gegen_rot = 0, n_wasser = 0;
    for (int i = 0; i < nl && i < 16; i++) {
        int e01 = -1, e21 = -1;
        const int d01 = g2v4_vergleich(0, 1, i, &e01), d21 = g2v4_vergleich(2, 1, i, &e21);
        const int rampe = k_re2_lagen[i].y != 0;   /* Figur oben auf der Rampe: Querung mit Rammen (Auftrag "Rampe: rammen wie gehabt"), s. enemy_ai_boss_gator.c GBP_GUARD */
        summe += s_v4n[0][i]; alle += s_v4n[1][i];
        if (rampe) { diff_r += d01; bild_r += s_v4n[0][i]; } else { diff += d01; n_wasser++; if (d21 > 0) gegen_rot++; }
        diff_h += d21;
        int q = -1;                                /* erste Querung im Lauf mit */
        for (int t = 0; t < s_v4n[0][i]; t++) if (s_v4z[0][i][t].ph == 7) { q = t; break; }
        const int ph_e = e01 >= 0 ? s_v4z[0][i][e01].ph : -1, ph_o = e01 >= 0 ? s_v4z[1][i][e01].ph : -1;   /* Phase mit / ohne im ersten abweichenden Bild */
        if (rampe && d01 > 0 && !((ph_e == 5 || ph_e == 7) && (ph_o == 5 || ph_o == 7))) rampe_vor_querung++;   /* Abweichung beginnt nur in Belagerung/Querung (GBP_GUARD 5 / GBP_CROSS 7) */
        printf("    %-15s %s mit %4d / ohne %4d Bilder: abweichend %4d (erstes Bild %d%s) | Gegenprobe Huelle aktiv (f942f56c0): abweichend %4d (erstes Bild %d)\n",
               k_re2_lagen[i].name, rampe ? "Rampe " : "Wasser", s_v4n[0][i], s_v4n[1][i], d01, e01, rampe ? ", Rampe" : "", d21, e21);
        if (rampe) printf("                      erste Querung im Lauf mit: Bild %d; Phase im ersten abweichenden Bild mit %d / ohne %d\n", q, ph_e, ph_o);
    }
    printf("    SUMME Wasser-Lagen: abweichend %d; Rampen-Lagen: abweichend %d von %d Bildern (ab der ersten Querung mit Rammen); Gegenprobe Huelle %d; Rueckfall (Rueckwaerts/Tauchen) im Lauf mit: %d Bilder\n",
           diff, diff_r, bild_r, diff_h, h.rw_bilder);
    printf("    Halterung: %d Bilder getragen, Lage-Abweichung max %d, Drehungs-Abweichung max %d (Q12), Bilder mit Abweichung %d; Lokallage wandert in %d Bildern (zuletzt im Bild %d)\n",
           h.bilder, (int)h.abw_pos, (int)h.abw_rot, h.abw_bilder, h.lokal_wechsel, h.lokal_letzt);
    PRUEF(ok0 && ok1 && ok2 && summe >= nl * bilder * 9 / 10, "gelaufen (mit %d, ohne %d Bilder)", summe, alle);
    PRUEF(diff == 0, "(a) Wasser-Lagen: Bewegung mit Kanister Bild fuer Bild = ohne Kanister (Lage, Hoehe, Gier, Clip, Bild, Phase): %d abweichende Bilder", diff);
    PRUEF(rampe_vor_querung == 0, "(a) Rampen-Lagen: mit = ohne Kanister, bis die Seitenwahl der Belagerung (Querung ueber die Figur mit Rammen) greift - erste Abweichung in GUARD/CROSS (%d Lagen weichen anders ab)", rampe_vor_querung);
    PRUEF(h.rw_bilder == 0, "(a) kein Rueckfall Rueckwaerts/Tauchen in den 13 Lagen (%d Bilder)", h.rw_bilder);
    PRUEF(gegen_rot == n_wasser, "(a) Gegenprobe Kanister-Huelle aktiv (Stand f942f56c0): jede Wasser-Lage weicht ab (%d von %d) - rot ohne den Bau", gegen_rot, n_wasser);
    PRUEF(h.bilder >= summe * 9 / 10 && h.abw_pos == 0 && h.abw_rot == 0, "(b) Halterung: Kanister = Part 6 der gezeichneten Pose x Lokallage in jedem Bild (%d Bilder, Lage max %d, Drehung max %d)",
          h.bilder, (int)h.abw_pos, (int)h.abw_rot);
    {   /* Gegenprobe (b): ein Bild alte Pose */
        g2v4_halt_t hg; memset(&hg, 0, sizeof hg);
        re15_gr2_v4_alt_setzen(0); re15_gator_boss_ramm_alt_setzen(1);
        re15_actor_t *g = szene_bis_kampf(elza);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        static g2v4_z_t zg[G2V4_BILDER];
        if (g && g2_kanister_ins_maul(g, pl)) {
            re15_gr2_kan_vorbild_setzen(1);
            (void)g2v4_lage(g, pl, &k_re2_lagen[11], 300, zg, &hg, 1);   /* Weg-Becken: Figur geht, er verfolgt */
            re15_gr2_kan_vorbild_setzen(0);
        }
        re15_gator_boss_ramm_alt_setzen(0);
        printf("    Gegenprobe ein Bild alte Pose: %d Bilder, Lage-Abweichung max %d, Drehung max %d, Bilder mit Abweichung %d\n", hg.bilder, (int)hg.abw_pos, (int)hg.abw_rot, hg.abw_bilder);
        PRUEF(hg.bilder > 100 && hg.abw_pos > 0 && hg.abw_bilder * 2 > hg.bilder, "(b) Gegenprobe ein Bild alte Pose: Abweichung > 0 in mehr als der Haelfte der Bilder (%d von %d, max %d) - rot ohne den Bau",
              hg.abw_bilder, hg.bilder, (int)hg.abw_pos);
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    re15_door_seq_setze_laeufer(fang);
    if (getenv("G2_RE2_ALT")) re15_gr2_rw_alt_setzen(1);   /* G2-RE2: Werkzeug-Laeufe (fuzz/bahn) im Stand NB5 - Vergleich mit dem NB5-Bau */
    if (!strcmp(teil, "projektion") && argc >= 5) return projektion(argc, argv);
    if (!strcmp(teil, "kamerazone") && argc >= 3) return kamerazone(argc, argv);
    if (!strcmp(teil, "begehbar") && argc >= 5) return begehbar(argc, argv);
    if (!strcmp(teil, "sichtkarte") && argc >= 4) {   /* NB4: Kreispunkte (von 16, R 2000) im Bild von Cut argv[3] je Mittelpunkt, Raster 1000 */
        if (rdt_laden((uint16_t)strtoul(argv[2], NULL, 16)) != 0) return 1;
        g_room_rdt_ok = 1;
        for (int z = -25000; z <= -7000; z += 1000) { printf("z %6d:", z); for (int x = -7000; x <= 5000; x += 1000) printf(" %2d", re15_gr2_kreis_sicht(atoi(argv[3]), x, z)); printf("\n"); }
        return 0;
    }
    if (!strcmp(teil, "traglog") && argc >= 3) return traglog(argv[2]);
    if (!strcmp(teil, "kletterkarte") && argc >= 3) return kletterkarte(argv[2]);
    if (!strcmp(teil, "ganglog") && argc >= 4) return ganglog(argv[2], argv[3]);
    if (!strcmp(teil, "beckenkarte") && argc >= 4) return beckenkarte(argv[2], argv[3]);
    if (!strcmp(teil, "maulgeo")) { if (!gator_bank()) return 1; maulgeo(argc > 2); return 0; }
    if (!strcmp(teil, "wurfkamera")) { if (!gator_bank()) return 1; wurfkamera(argc > 2); return 0; }
    if (!strcmp(teil, "leichenachse")) { if (!gator_bank()) return 1; leichenachse(argc > 2); return 0; }
    if (!strcmp(teil, "kopfwinkel")) { if (!gator_bank()) return 1; kopfwinkel(argc > 2); return 0; }
    if (!strcmp(teil, "bahnpruef")) { if (!gator_bank()) return 1; return bahnpruef_werkzeug(argc, argv); }
    if (!strcmp(teil, "kasten")) { if (!gator_bank()) return 1; return kasten_werkzeug(argc > 2); }
    if (!strcmp(teil, "leichenfit")) { if (!gator_bank()) return 1; leichenfit(argc > 2); return 0; }
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
    if (alle || !strcmp(teil, "fuzz"))      teil_fuzz(0);
    if (alle || !strcmp(teil, "fuzz_elza")) teil_fuzz(1);
    if (alle || !strcmp(teil, "haken"))      teil_haken(0);
    if (alle || !strcmp(teil, "haken_elza")) teil_haken(1);
    if (alle || !strcmp(teil, "bahn"))      teil_bahn(0);
    if (alle || !strcmp(teil, "bahn_elza")) teil_bahn(1);
    if (alle || !strcmp(teil, "kamera"))      teil_kamera(0);
    if (alle || !strcmp(teil, "kamera_elza")) teil_kamera(1);
    if (alle || !strcmp(teil, "re2"))       teil_re2(0);
    if (alle || !strcmp(teil, "re2_elza"))  teil_re2(1);
    if (alle || !strcmp(teil, "re2_gegen"))      teil_re2_gegen(0);
    if (alle || !strcmp(teil, "re2_gegen_elza")) teil_re2_gegen(1);
    if (alle || !strcmp(teil, "gang"))      teil_gang(0);
    if (alle || !strcmp(teil, "gang_elza")) teil_gang(1);
    if (alle || !strcmp(teil, "v4"))        teil_v4(0);
    if (alle || !strcmp(teil, "v4_elza"))   teil_v4(1);
    re15_door_seq_setze_laeufer(NULL);
    printf("%s: %d Fehler\n", teil, g_fail);
    return g_fail ? 1 : 0;
}
