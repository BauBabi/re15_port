/*
 * kanister2080_pc.c — Runde 41 Spur G "kanister2080", PC-Seite (include/re15_kanister2080_r41.h, Dossier
 * analysis/befunde_runde41/G_kanister2080.md):
 *   (1) der RE2-GASKANISTER in ROOM2080/2081 (stehend an der Reling, in Marvins/Johns Haenden, im Fall): dasselbe Modell wie
 *       Spur B (shared_assets/RE2/KANISTER40A0.MD1/.TIM = ROOM40A0 Modell 1), dieselbe Zeichenart wie ein RDT-Prop (main.c
 *       Prop-Pass: view x Welt, RTPT/NCLIP ueber objpoly_r38, Licht des aktiven Cuts) und derselbe TIM-Platz 53 wie
 *       gator_re2_pc.c (gleiche Textur, R41_KANISTER_TIM_SLOT).
 *   (2) der TUER-TON: Tonteil des RE2-Tuerarchivs DOOR06 (Stahltuer der 2030-Tuer, gen/tuer_zuordnung.inc S161 T075;
 *       Archiv-Tabelle @0x8009A568) in die RE2-Tuerbank, Satz 1 = Door_exit (@0x800141f0 `jal 0x8005ba28`, `lui a0,0x1`).
 *   (3) der AUFSCHLAG im Wasser: Mini-Bank KANISTER2080.VBS Satz 0 (RE2 ROOM40A0 Satz 11, sub10 Se_on @SCD 0x0686),
 *       Laderpaar wie gator_re2_pc.c (2).
 *   (4) der NPC-SCHUSS: der Partner-Schuss der Runde 38 Spur H (re15_elza_se_hook(RE15_ELZA_TON_SCHUSS) = RE2 ROOM4040
 *       Satz 0x0A), wie Johns Schuesse in ROOM1021/1141.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_kanister2080_r41.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_objpoly_r38.h"
#include "re15_engine.h"
#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_door_seq.h"      /* re15_door_seq_re2_archiv: Tabelle @0x8009a520 */
#include "re15_elza_r36.h"      /* re15_elza_se_hook, RE15_ELZA_TON_SCHUSS */
#include "asset_root_pc.h"      /* re15_pc_read_re2 */
#include "../../../engine/src/gen/r41_kanister2080_bank.inc"

#define K2080_TIM_SLOT   53    /* = R41_KANISTER_TIM_SLOT (gator_re2_pc.c): dieselbe Textur KANISTER40A0.TIM */
#define K2080_TUER_NR    0x06  /* DOOR06 (gen/tuer_zuordnung.inc S161 ROOM2080 Slot 0 -> 2030: Archiv 0x06) */
#define K2080_TUER_SATZ  1     /* Door_exit @0x800141f0/@0x800141f4 */

extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);

/* ---- (1) Modell ------------------------------------------------------------------------------------------------------- */
static int        s_modell;       /* 0 ungeprueft, 1 geladen, -1 fehlt */
static uint8_t   *s_md1_raw;
static re15_md1_t s_md1;

static int modell_laden(void)
{
    if (s_modell) return s_modell > 0;
    s_modell = -1;
    int n = 0;
    s_md1_raw = re15_pc_read_re2("KANISTER40A0.MD1", &n);
    if (!s_md1_raw || re15_md1_parse(s_md1_raw, n, &s_md1) != 0 || s_md1.mesh_count < 1) {
        fprintf(stderr, "[k2080] shared_assets/RE2/KANISTER40A0.MD1 fehlt/unlesbar -> kein Kanister\n");
        return 0;
    }
    int tn = 0;
    uint8_t *tim_raw = re15_pc_read_re2("KANISTER40A0.TIM", &tn);
    re15_tim_t tim;
    if (!tim_raw || re15_tim_parse(tim_raw, tn, &tim) != 0) {
        fprintf(stderr, "[k2080] shared_assets/RE2/KANISTER40A0.TIM fehlt/unlesbar -> kein Kanister\n");
        free(tim_raw); return 0;
    }
    re15_render_pc_upload_tim_slot(&tim, K2080_TIM_SLOT);
    free(tim_raw);
    s_modell = 1;
    fprintf(stderr, "[k2080] Kanister-Modell geladen: %d Meshes, TIM-Platz %d\n", s_md1.mesh_count, K2080_TIM_SLOT);
    return 1;
}

void re15_k2080_pc_zeichnen(const void *cam_view_v, int cx, int cy)
{
    int32_t pos[3], rot[9];
    if (!re15_k2080_kanister_lage(pos, rot)) return;
    if (!modell_laden()) return;
    const re15_camera_view_t *cam_view = (const re15_camera_view_t *)cam_view_v;
    re15_render_pc_bind_tim_slot(K2080_TIM_SLOT);
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
    /* MESSZEILE (kein Verhalten): Lage + Bildkasten bei jedem Lagewechsel des Zustands (steht/Haende/Fall) und alle 30 Bilder. */
    static unsigned s_letzt = 0xffffffffu; static int32_t s_alt[3] = { 0, 0, 0 }; static int s_cut_alt = -99;
    const int neu = (pos[0] != s_alt[0] || pos[1] != s_alt[1] || pos[2] != s_alt[2]);
    if (s_letzt == 0xffffffffu || g_re15_active_cut != s_cut_alt || (neu && (g_engine.frame_count - s_letzt) >= 30u)) {
        s_letzt = g_engine.frame_count; memcpy(s_alt, pos, sizeof s_alt); s_cut_alt = g_re15_active_cut;
        fprintf(stderr, "[k2080] F%u Kanister pos=(%d,%d,%d) Cut %d Kasten=(%d..%d,%d..%d) Dreiecke %d\n",
                (unsigned)g_engine.frame_count, (int)pos[0], (int)pos[1], (int)pos[2], g_re15_active_cut, x0, x1, y0, y1, tris);
    }
}

/* ---- (2) Tuer-Ton: DOOR06-Tonteil in die RE2-Tuerbank, Satz 1 -------------------------------------------------------- */
static uint8_t *s_tuer;
static int      s_tuer_n, s_tuer_ton;
static void tuerton(void)
{
    if (!s_tuer) {
        int ton = 0, modell = 0, sektor = 0, datei = 0, n = 0;
        if (re15_door_seq_re2_archiv(K2080_TUER_NR, &ton, &modell, &sektor, &datei) != 0) {
            fprintf(stderr, "[k2080] DOOR%02X nicht in der Tabelle @0x8009a520 -> stumm\n", K2080_TUER_NR); return;
        }
        uint8_t *d = re15_pc_read_re2("DOOR/DOOR06.DO2", &n);
        if (!d || n != datei || ton <= 0 || ton > n) {
            fprintf(stderr, "[k2080] shared_assets/RE2/DOOR/DOOR06.DO2 fehlt/passt nicht (%d B, Tabelle %d B) -> stumm\n", n, datei);
            free(d); return;
        }
        s_tuer = d; s_tuer_n = n; s_tuer_ton = ton;
    }
    const int ok = re15_audio_re2_tuer_laden(s_tuer, s_tuer_ton);
    fprintf(stderr, "[k2080] F%u Tuer-Ton: DOOR06-Tonteil %d B %s, Satz %d (Door_exit @0x800141f0)\n", (unsigned)g_engine.frame_count,
            s_tuer_ton, ok ? "in der Tuerbank" : "nicht geladen (kein Audiogeraet)", K2080_TUER_SATZ);
    if (ok) re15_audio_re2_tuer_se(K2080_TUER_SATZ);
}

/* ---- (3) Aufschlag: Mini-Bank KANISTER2080.VBS ------------------------------------------------------------------------ */
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
    uint8_t *vbs = re15_pc_read_re2("KANISTER2080.VBS", &sz);
    if (!vbs) { fprintf(stderr, "[k2080] shared_assets/RE2/KANISTER2080.VBS fehlt -> Aufschlag stumm\n"); return 0; }
    if ((unsigned)sz < R41_K2080_VBD_OFF + R41_K2080_VBD_SIZE || R41_K2080_EDT_SIZE < 12) { free(vbs); return 0; }
    s_edt = (uint8_t *)malloc(R41_K2080_EDT_SIZE);
    if (!s_edt) { free(vbs); return 0; }
    memcpy(s_edt, vbs, R41_K2080_EDT_SIZE);
    uint32_t vh_off = (uint32_t)s_edt[R41_K2080_EDT_SIZE-8] | ((uint32_t)s_edt[R41_K2080_EDT_SIZE-7] << 8)
                    | ((uint32_t)s_edt[R41_K2080_EDT_SIZE-6] << 16) | ((uint32_t)s_edt[R41_K2080_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > R41_K2080_EDT_SIZE
        || re15_vab_parse(s_edt + vh_off, (size_t)R41_K2080_EDT_SIZE - vh_off, &s_vab) != 0) { free(vbs); return 0; }
    const uint8_t *vb = vbs + R41_K2080_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > R41_K2080_VBD_SIZE) continue;
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
static void platsch(void)
{
    const int ok = bank_laden();
    fprintf(stderr, "[k2080] F%u Aufschlag-Ton KANISTER2080.VBS Satz %d%s\n", (unsigned)g_engine.frame_count,
            R41_K2080_SE_PLATSCH, ok ? "" : " (Bank fehlt, stumm)");
    if (ok) re15_audio_se_play_bank(s_edt, &s_vab, s_dec, s_dec_len, R41_K2080_SE_PLATSCH);
}

/* ---- (4) NPC-Schuss ------------------------------------------------------------------------------------------------- */
static void schuss(void)
{
    if (re15_elza_se_hook) re15_elza_se_hook(RE15_ELZA_TON_SCHUSS);
}

void re15_pc_kanister2080_init(void)
{
    re15_k2080_tuerton_hook = tuerton;
    re15_k2080_platsch_hook = platsch;
    re15_k2080_schuss_hook  = schuss;
}
