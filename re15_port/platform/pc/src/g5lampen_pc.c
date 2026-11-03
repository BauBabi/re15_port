/*
 * g5lampen_pc.c — Runde 41 G5-Auftritt, Nachbesserung 5 (M4), PC-Seite (Engine: engine/src/g5auftritt_r41.c, Kopf
 * include/re15_g5auftritt_r41.h, Dossier analysis/befunde_runde41/G5_auftritt.md Abschnitt 11):
 *   (1) die TUNNELLAMPEN = RE2 ROOM7040 Raumobjekte 3/4 (Modell R704_LAMPE.MD1/.TIM, tools/r41_g5auftritt/re2_m4_7040.py).
 *       RE2-Zeichner FUN_800363f0 je Objekt (Wort +0 & 1, nicht & 0x80000 @0x8003644c-60):
 *         RotMatrix(+0x74, +0x24) `jal 0x8008e1f4` @0x80036470, CompMatrix(Eltern +0x80 = 0x8009db44 = Einheit, t 0) @0x80036484,
 *         Licht je Objektlage `jal 0x80076f88` @0x800364ac (Lichtsatz des Schnitts, wie re15_light_setup_actor),
 *         Sichtpruefung FUN_8002c820(+0x68, *(0x800ce338)) @0x80036534 (re15_g5a_lampe_zone),
 *         Wort +0x10 = Obj_model_set pc[0x0C..0x0D] = 0 (`2d 03 .. 00 00 ..` @0x01240/@0x01266): weder Flach (& 0x10 @0x800365e8)
 *         noch halbtransparent ((+0x10 >> 1) & 1 @0x8003650c) -> FUN_80027bec (Dreiecke, Code 0x34 = GT3) / FUN_80027dbc (Vierecke)
 *         mit Grundfarbe +0x7C = 0x808080 (RAM s03) - derselbe beleuchtete Weg wie die RDT-Props (main.c Prop-Pass) und der
 *         Kanister (kanister2080_pc.c). RAM s03: Weltmatrix +0x54 = (0,0,-4096 / 0,-4096,0 / -4096,0,0) = RotMatrix(0C00,0C00,0400).
 *       Die RE2-Masken von Schnitt 9 (R704_CUT09.PRI) verdecken die Lampe ueber die Tiefe der Dreiecke wie jedes Prop.
 *   (2) der BEBEN-TON: Mini-Bank R704_BEBEN.VBS Satz 0 = ROOM7040 EDT-Satz 12 (sub04 Se_on `36 02 0c 00 01 ..` @SCD 0x016AC).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_g5auftritt_r41.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_objpoly_r38.h"
#include "re15_engine.h"
#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_skeleton.h"     /* re15_sin_q12 / re15_cos_q12 */
#include "asset_root_pc.h"     /* re15_pc_read_re2 */
#include "../../../engine/src/gen/r41_g5a_beben_bank.inc"

#define G5L_TIM_SLOT 57        /* render_pc.c RE15_TIM_SLOT_MAX 58: 57 = Tunnellampen ROOM7040 (Nachbesserung 5) */

extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern int  re15_render_pc_dbg_slot_loaded(int slot);
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);

/* ---- (1) Modell --------------------------------------------------------------------------------------------------------- */
static int        s_modell;       /* 0 ungeprueft, 1 geladen, -1 fehlt */
static uint8_t   *s_md1_raw;
static re15_md1_t s_md1;

static int modell_laden(void)
{
    if (s_modell > 0 && !re15_render_pc_dbg_slot_loaded(G5L_TIM_SLOT)) s_modell = 0;   /* Slot von einem Teardown geleert */
    if (s_modell) return s_modell > 0;
    s_modell = -1;
    int n = 0;
    if (!s_md1_raw) {
        s_md1_raw = re15_pc_read_re2("R704_LAMPE.MD1", &n);
        if (!s_md1_raw || re15_md1_parse(s_md1_raw, n, &s_md1) != 0 || s_md1.mesh_count < 1) {
            fprintf(stderr, "[g5lampe] shared_assets/RE2/R704_LAMPE.MD1 fehlt/unlesbar -> keine Tunnellampen\n");
            free(s_md1_raw); s_md1_raw = NULL; return 0;
        }
    }
    int tn = 0;
    uint8_t *tim_raw = re15_pc_read_re2("R704_LAMPE.TIM", &tn);
    re15_tim_t tim;
    if (!tim_raw || re15_tim_parse(tim_raw, tn, &tim) != 0) {
        fprintf(stderr, "[g5lampe] shared_assets/RE2/R704_LAMPE.TIM fehlt/unlesbar -> keine Tunnellampen\n");
        free(tim_raw); return 0;
    }
    re15_render_pc_upload_tim_slot(&tim, G5L_TIM_SLOT);
    free(tim_raw);
    if (!re15_render_pc_dbg_slot_loaded(G5L_TIM_SLOT)) return 0;
    s_modell = 1;
    fprintf(stderr, "[g5lampe] Lampen-Modell geladen: %d Mesh, TIM-Platz %d\n", s_md1.mesh_count, G5L_TIM_SLOT);
    return 1;
}

/* RotMatrix (RE2 0x8008e1f4 = RE1.5 RotMatrix, dieselbe Entwicklung wie main.c pc_prop_rot_q12). */
static void rotmatrix(const int16_t r[3], int32_t m[9])
{
    const int32_t sx = re15_sin_q12((int)r[0]), cx = re15_cos_q12((int)r[0]);
    const int32_t sy = re15_sin_q12((int)r[1]), cy = re15_cos_q12((int)r[1]);
    const int32_t sz = re15_sin_q12((int)r[2]), cz = re15_cos_q12((int)r[2]);
    #define Q12(a, b) ((int32_t)(((int64_t)(a) * (int64_t)(b)) >> 12))
    m[0] = Q12(cz, cy);  m[1] = -Q12(sz, cy);  m[2] = sy;
    m[3] = (int32_t)((((int64_t)sz * cx) * 4096 + (int64_t)cz * sy * sx) >> 24);
    m[4] = (int32_t)((((int64_t)cz * cx) * 4096 - (int64_t)sz * sy * sx) >> 24);
    m[5] = -Q12(cy, sx);
    m[6] = (int32_t)((((int64_t)sz * sx) * 4096 - (int64_t)cz * sy * cx) >> 24);
    m[7] = (int32_t)((((int64_t)cz * sx) * 4096 + (int64_t)sz * sy * cx) >> 24);
    m[8] = Q12(cy, cx);
    #undef Q12
}

/* MESSUNG (kein Verhalten): je gezeichneter Lampe Bildkasten + Dreieckszahl; RE15_G5A_LAMPE_LOG=1 jedes Bild, sonst nur der Erste. */
static int s_log = -1;

static void lampe_zeichnen(int i, const re15_camera_view_t *cam_view, int cx, int cy)
{
    int32_t pos[3]; int16_t dir[3]; int32_t rot[9];
    if (!re15_g5a_lampe(i, pos, dir)) return;
    if (!re15_g5a_lampe_zone(g_re15_active_cut, pos[0], pos[2])) return;        /* FUN_8002c820 @0x80036534 */
    if (!modell_laden()) return;
    rotmatrix(dir, rot);
    re15_render_pc_bind_tim_slot(G5L_TIM_SLOT);
    re15_actor_lightctx_t lw, lctx;
    if (g_re15_room_lights_ok && g_re15_active_cut >= 0 && g_re15_active_cut < g_re15_room_lights.cut_count) {
        re15_light_setup_actor(&g_re15_room_lights.cuts[g_re15_active_cut], pos, NULL, &lw);
        re15_light_ctx_rotate_for_bone(&lw, rot, &lctx);
    } else {
        memset(&lctx, 0, sizeof lctx);
    }
    int32_t bm[9], bt[3];
    re15_camera_compose_view_bone(cam_view, rot, pos, bm, bt);
    int tris = 0, x0 = 9999, x1 = -9999, y0 = 9999, y1 = -9999;
    for (int mi = 0; mi < s_md1.mesh_count; mi++) {
        const re15_md1_mesh_t *hm = &s_md1.meshes[mi];
        for (int ti = 0; ti < hm->triangle_count; ti++) {
            const re15_md1_triangle_t *tr = &hm->triangles[ti];
            if (tr->v0 >= (uint32_t)hm->tri_vertex_count || tr->v1 >= (uint32_t)hm->tri_vertex_count
                || tr->v2 >= (uint32_t)hm->tri_vertex_count) continue;
            const re15_md1_vertex_t *vp[3] = { &hm->tri_vertices[tr->v0], &hm->tri_vertices[tr->v1], &hm->tri_vertices[tr->v2] };
            int ax[3], ay[3], wz = 0;
            if (!re15_objpoly_r38_tri_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz)) continue;
            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
            int po = (int)((uv->page & 0x000F) * 128);
            uint8_t c[3][3];
            for (int k = 0; k < 3; k++) {
                uint32_t ni = (k == 0) ? tr->n0 : (k == 1) ? tr->n1 : tr->n2;
                if (hm->tri_normals && ni < (uint32_t)hm->tri_normal_count)
                    re15_light_shade_vertex(&lctx, hm->tri_normals[ni].x, hm->tri_normals[ni].y, hm->tri_normals[ni].z,
                                            &c[k][0], &c[k][1], &c[k][2]);
                else { c[k][0] = g_re15_light_tint[0]; c[k][1] = g_re15_light_tint[1]; c[k][2] = g_re15_light_tint[2]; }
                if (ax[k] < x0) x0 = ax[k]; if (ax[k] > x1) x1 = ax[k];
                if (ay[k] < y0) y0 = ay[k]; if (ay[k] > y1) y1 = ay[k];
            }
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0, ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                         ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2, (int)uv->page, (int)uv->clut, wz,
                                         c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2]);
            tris++;
        }
        for (int qi = 0; qi < hm->quad_count; qi++) {
            const re15_md1_quad_t *qd = &hm->quads[qi];
            if (qd->v0 >= (uint32_t)hm->quad_vertex_count || qd->v1 >= (uint32_t)hm->quad_vertex_count
                || qd->v2 >= (uint32_t)hm->quad_vertex_count || qd->v3 >= (uint32_t)hm->quad_vertex_count) continue;
            const re15_md1_vertex_t *vp[4] = { &hm->quad_vertices[qd->v0], &hm->quad_vertices[qd->v1],
                                               &hm->quad_vertices[qd->v2], &hm->quad_vertices[qd->v3] };
            int ax[4], ay[4], wz = 0; uint8_t ok[2];
            if (!re15_objpoly_r38_quad_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz, ok)) continue;
            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];
            int po = (int)((uv->page & 0x000F) * 128);
            uint8_t c[4][3];
            for (int k = 0; k < 4; k++) {
                uint32_t ni = (k == 0) ? qd->n0 : (k == 1) ? qd->n1 : (k == 2) ? qd->n2 : qd->n3;
                if (hm->quad_normals && ni < (uint32_t)hm->quad_normal_count)
                    re15_light_shade_vertex(&lctx, hm->quad_normals[ni].x, hm->quad_normals[ni].y, hm->quad_normals[ni].z,
                                            &c[k][0], &c[k][1], &c[k][2]);
                else { c[k][0] = g_re15_light_tint[0]; c[k][1] = g_re15_light_tint[1]; c[k][2] = g_re15_light_tint[2]; }
                if (ax[k] < x0) x0 = ax[k]; if (ax[k] > x1) x1 = ax[k];
                if (ay[k] < y0) y0 = ay[k]; if (ay[k] > y1) y1 = ay[k];
            }
            if (ok[0])
                re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0, ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                             ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3, (int)uv->page, (int)uv->clut, wz,
                                             c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[3][0], c[3][1], c[3][2]);
            if (ok[1])
                re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0, ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                             ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2, (int)uv->page, (int)uv->clut, wz,
                                             c[0][0], c[0][1], c[0][2], c[3][0], c[3][1], c[3][2], c[2][0], c[2][1], c[2][2]);
            tris++;
        }
    }
    if (s_log < 0) { const char *e = getenv("RE15_G5A_LAMPE_LOG"); s_log = (e && *e && *e != '0') ? 1 : 0; }
    static int s_erst = 1;
    if (s_log || s_erst) {
        s_erst = 0;
        fprintf(stderr, "[g5lampe] F%u Obj %d x %d Cut %d Kasten=(%d..%d,%d..%d) Dreiecke %d\n", (unsigned)g_engine.frame_count,
                3 + i, (int)pos[0], g_re15_active_cut, x0, x1, y0, y1, tris);
    }
}

void re15_g5lampen_pc_zeichnen(const void *cam_view_v, int cx, int cy)
{
    if (!re15_g5a_laeuft()) return;
    if (s_log < 0) { const char *e = getenv("RE15_G5A_LAMPE_LOG"); s_log = (e && *e && *e != '0') ? 1 : 0; }
    if (s_log) {   /* MESSZEILE (kein Verhalten): Skriptzustand beider Objekte je Bild, auch ungezeichnet */
        int32_t p3[3], p4[3];
        const int a3 = re15_g5a_lampe(0, p3, NULL), a4 = re15_g5a_lampe(1, p4, NULL);
        fprintf(stderr, "[g5lampe] F%u Zustand Vorlauf %d Bild %d: Obj 3 %s x %d, Obj 4 %s x %d, Cut %d, Beben %d\n", (unsigned)g_engine.frame_count,
                re15_g5a_vorlauf(), re15_g5a_bild(), a3 ? "an" : "park", (int)p3[0], a4 ? "an" : "park", (int)p4[0], g_re15_active_cut,
                re15_g5a_beben());
    }
    const re15_camera_view_t *cam_view = (const re15_camera_view_t *)cam_view_v;
    lampe_zeichnen(0, cam_view, cx, cy);
    lampe_zeichnen(1, cam_view, cx, cy);
}

/* ---- (2) Beben-Ton: Mini-Bank R704_BEBEN.VBS (Laderpaar wie kanister2080_pc.c bank_laden) ------------------------------- */
static int        s_bank;
static re15_vab_t s_vab;
static uint8_t   *s_edt;
static int16_t   *s_dec[RE15_VAB_MAX_SAMPLES];
static int        s_dec_len[RE15_VAB_MAX_SAMPLES];
static int bank_laden(void)
{
    if (s_bank) return s_bank > 0;
    s_bank = -1;
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("R704_BEBEN.VBS", &sz);
    if (!vbs) { fprintf(stderr, "[g5lampe] shared_assets/RE2/R704_BEBEN.VBS fehlt -> Beben stumm\n"); return 0; }
    if ((unsigned)sz < R41_G5A_BEBEN_VBD_OFF + R41_G5A_BEBEN_VBD_SIZE || R41_G5A_BEBEN_EDT_SIZE < 12) { free(vbs); return 0; }
    s_edt = (uint8_t *)malloc(R41_G5A_BEBEN_EDT_SIZE);
    if (!s_edt) { free(vbs); return 0; }
    memcpy(s_edt, vbs, R41_G5A_BEBEN_EDT_SIZE);
    uint32_t vh_off = (uint32_t)s_edt[R41_G5A_BEBEN_EDT_SIZE-8] | ((uint32_t)s_edt[R41_G5A_BEBEN_EDT_SIZE-7] << 8)
                    | ((uint32_t)s_edt[R41_G5A_BEBEN_EDT_SIZE-6] << 16) | ((uint32_t)s_edt[R41_G5A_BEBEN_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > R41_G5A_BEBEN_EDT_SIZE
        || re15_vab_parse(s_edt + vh_off, (size_t)R41_G5A_BEBEN_EDT_SIZE - vh_off, &s_vab) != 0) { free(vbs); return 0; }
    const uint8_t *vb = vbs + R41_G5A_BEBEN_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > R41_G5A_BEBEN_VBD_SIZE) continue;
        size_t cap = (vsz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_dec[i] = pcm;
        s_dec_len[i] = re15_vag_adpcm_decode(vb + off, vsz, pcm, cap);
    }
    free(vbs);
    s_bank = 1;
    return 1;
}

static void beben_ton(void)
{
    const int ok = bank_laden();
    fprintf(stderr, "[g5lampe] F%u Beben-Ton R704_BEBEN.VBS Satz %d (ROOM7040 sub04 Se_on @0x016AC)%s\n",
            (unsigned)g_engine.frame_count, R41_G5A_SE_BEBEN, ok ? "" : " (Bank fehlt, stumm)");
    if (ok) re15_audio_se_play_bank(s_edt, &s_vab, s_dec, s_dec_len, R41_G5A_SE_BEBEN);
}

void re15_pc_g5lampen_init(void)
{
    re15_g5a_beben_ton_hook = beben_ton;
}
