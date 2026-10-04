/* r35_raeume_volumen.h — Runde 35 Spur H, Nachbesserung 2 (M2): VOLUMENMASS fuer den Gitterarm-Griff.
 * Gemeinsam fuer unit_r35_raeume_arme (Riegel) und probe_r35_raeume_arme (Sonde, exe-Pins).
 *
 * Das waagerechte Mass "Hand < 120 an der Brustachse" kennt weder Hoehe noch Unterarm. Hier zaehlt man
 * ARM-VERTICES (EM2D-Mesh Unterarm + Hand = Bone Hand-1 / Hand, mesh == bone in der reinen RE2-Bank) in
 * Leons Kopf- und Rumpf-Volumen. Das Volumen kommt aus den Daten: PL00.MD1 Mesh 8 (Kopf) und Mesh 0
 * (Rumpf), je Hoehenband 32 Einheiten die Bounding-Box der Mesh-Vertices im Knochen-Rahmen — eine
 * Querschnitt-Huelle, eher zu gross als zu klein (strenges Mass: auch Auflegen auf Schulter/Hals zaehlt).
 * Welt -> Knochen: lokal = R_b^T * (R_y(yaw)^T * (P - Wurzel) - t_b) (Vertex-Transform main.c:
 * R_y * (R_b * v + t_b) + Wurzel). Kopf/Rumpf-Knochen von RE2-Leon und RE1.5-PL00 sind gleich
 * (PL00.EMR-Versaetze 1..9/12 byte-gleich, Dossier Nachbesserung 2). */
#ifndef R35_RAEUME_VOLUMEN_H
#define R35_RAEUME_VOLUMEN_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_enemy_ai_re2_zellenarm.h"

#define VOL_BAND 32
typedef struct { int ymin, nb; int16_t x0[64], x1[64], z0[64], z1[64]; uint8_t ok[64]; } vol_t;
static vol_t s_vol[2];                         /* 0 = Rumpf (Mesh 0), 1 = Kopf (Mesh 8) */
static re15_md1_t s_pl_md1; static uint8_t *s_pl_md1_buf = NULL;
static void vol_add(vol_t *v, const re15_md1_vertex_t *p, int n, int pass)
{
    for (int i = 0; i < n; i++) {
        if (pass == 0) { if (p[i].y < v->ymin) v->ymin = p[i].y; continue; }
        int b = (p[i].y - v->ymin) / VOL_BAND;
        if (b < 0 || b >= 64) continue;
        if (!v->ok[b]) { v->x0[b] = v->x1[b] = p[i].x; v->z0[b] = v->z1[b] = p[i].z; v->ok[b] = 1; }
        if (p[i].x < v->x0[b]) v->x0[b] = p[i].x;
        if (p[i].x > v->x1[b]) v->x1[b] = p[i].x;
        if (p[i].z < v->z0[b]) v->z0[b] = p[i].z;
        if (p[i].z > v->z1[b]) v->z1[b] = p[i].z;
        if (b + 1 > v->nb) v->nb = b + 1;
    }
}
static int vol_laden(const char *md1_pfad)
{
    if (!s_pl_md1_buf) {
        FILE *f = fopen(md1_pfad, "rb"); if (!f) return 0;
        fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
        s_pl_md1_buf = (uint8_t *)malloc((size_t)n);
        if (!s_pl_md1_buf || fread(s_pl_md1_buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); return 0; }
        fclose(f);
        if (re15_md1_parse(s_pl_md1_buf, (int)n, &s_pl_md1) != 0) return 0;
    }
    const int mesh[2] = { 0, 8 };
    for (int k = 0; k < 2; k++) {
        vol_t *v = &s_vol[k]; memset(v, 0, sizeof *v); v->ymin = 1 << 30;
        const re15_md1_mesh_t *m = &s_pl_md1.meshes[mesh[k]];
        for (int pass = 0; pass < 2; pass++) {
            vol_add(v, m->tri_vertices, m->tri_vertex_count, pass);
            vol_add(v, m->quad_vertices, m->quad_vertex_count, pass);
        }
    }
    return s_vol[0].nb > 0 && s_vol[1].nb > 0;
}
static int vol_innen(const vol_t *v, const re15_skel_pose_t *p, int yaw, int32_t lx, int32_t lz, const int32_t P[3])
{
    int64_t cs = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int64_t dx = P[0] - lx, dy = P[1], dz = P[2] - lz;
    int64_t mx = (cs * dx - sn * dz) >> 12, my = dy, mz = (sn * dx + cs * dz) >> 12;   /* R_y^T */
    mx -= p->trans[0]; my -= p->trans[1]; mz -= p->trans[2];
    int64_t x = (p->rot[0] * mx + p->rot[3] * my + p->rot[6] * mz) >> 12;              /* R_b^T */
    int64_t y = (p->rot[1] * mx + p->rot[4] * my + p->rot[7] * mz) >> 12;
    int64_t z = (p->rot[2] * mx + p->rot[5] * my + p->rot[8] * mz) >> 12;
    if (y < v->ymin) return 0;
    int b = (int)((y - v->ymin) / VOL_BAND);
    if (b < 0 || b >= v->nb || !v->ok[b]) return 0;
    return x >= v->x0[b] && x <= v->x1[b] && z >= v->z0[b] && z <= v->z1[b];
}
/* Nachbesserung 3: dieselbe Huellen-Pruefung mit einer WELT-Matrix des Knochens (RE2-RAM: Part +0x48 m[3][3]
 * Q12, Translation +0x5C/+0x60/+0x64 — Pin-Quelle @0x80100C18-38, geschrieben vom Zeichnen FUN_80027434).
 * lokal = M^T * (P - t). */
static int vol_innen_welt(const vol_t *v, const int16_t M[9], const int32_t t[3], const int32_t P[3])
{
    int64_t dx = P[0] - t[0], dy = P[1] - t[1], dz = P[2] - t[2];
    int64_t x = ((int64_t)M[0] * dx + (int64_t)M[3] * dy + (int64_t)M[6] * dz) >> 12;
    int64_t y = ((int64_t)M[1] * dx + (int64_t)M[4] * dy + (int64_t)M[7] * dz) >> 12;
    int64_t z = ((int64_t)M[2] * dx + (int64_t)M[5] * dy + (int64_t)M[8] * dz) >> 12;
    if (y < v->ymin) return 0;
    int b = (int)((y - v->ymin) / VOL_BAND);
    if (b < 0 || b >= v->nb || !v->ok[b]) return 0;
    return x >= v->x0[b] && x <= v->x1[b] && z >= v->z0[b] && z <= v->z1[b];
}
/* Arm-Vertices (Unterarm + Hand, Pose = Clip/Bild des Arms, rein) in Leons Kopf/Rumpf. leon[0] = Rumpf-,
 * leon[1] = Kopf-Pose (Knochen 0/8 der Opfer-Pose), Leon-Wurzel (lx, 0, lz), Blick lyaw. spiegel = 1: die
 * Arm-Punkte vorher waagerecht an Leons Wurzel um 180 Grad gedreht — die RE2-Konstruktion des Ruecken-Griffs
 * von der Gesicht-Lage aus (Leon um seine Wurzel gedreht @0x8010130C-18 == Welt gegengedreht). */
static int vol_arm_in_leon(re15_actor_t *arm, const re15_skel_pose_t leon[2], int lyaw, int32_t lx, int32_t lz,
                           int spiegel)
{
    re15_enemy_bank_t *b = re15_enemy_find(0x1A);
    if (!b) return -1;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, arm, -1, arm->anim_frame);
    re15_skel_pose_t ap[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = (kf >= 0) ? re15_skel_compute_pose(&b->skel, kf, ap) : -1;
    g_anim_pose_actor = save;
    if (rv != 0) return -1;
    const int hb = re15_re2arm_hand_bone(arm);
    const int bones[2] = { hb - 1, hb };
    int64_t cs = re15_cos_q12(arm->rot_y), sn = re15_sin_q12(arm->rot_y);
    int innen = 0;
    for (int k = 0; k < 2; k++) {
        const re15_skel_pose_t *p = &ap[bones[k]];
        const re15_md1_mesh_t *m = &b->md1.meshes[bones[k]];
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
            int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
            for (int i = 0; i < nv; i++) {
                int64_t mx = ((int64_t)p->rot[0]*vv[i].x + (int64_t)p->rot[1]*vv[i].y + (int64_t)p->rot[2]*vv[i].z) >> 12;
                int64_t my = ((int64_t)p->rot[3]*vv[i].x + (int64_t)p->rot[4]*vv[i].y + (int64_t)p->rot[5]*vv[i].z) >> 12;
                int64_t mz = ((int64_t)p->rot[6]*vv[i].x + (int64_t)p->rot[7]*vv[i].y + (int64_t)p->rot[8]*vv[i].z) >> 12;
                mx += p->trans[0]; my += p->trans[1]; mz += p->trans[2];
                int32_t P[3] = { arm->x + (int32_t)((cs * mx + sn * mz) >> 12), arm->y + (int32_t)my,
                                 arm->z + (int32_t)((-sn * mx + cs * mz) >> 12) };
                if (spiegel) { P[0] = 2 * lx - P[0]; P[2] = 2 * lz - P[2]; }
                if (vol_innen(&s_vol[0], &leon[0], lyaw, lx, lz, P) ||
                    vol_innen(&s_vol[1], &leon[1], lyaw, lx, lz, P)) innen++;
            }
        }
    }
    return innen;
}
#endif
