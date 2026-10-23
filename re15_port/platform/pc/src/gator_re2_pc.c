/*
 * gator_re2_pc.c — Runde 41 Spur B "re2gator", PC-Seite:
 *   (1) der GASKANISTER als Prop: ROOM40A0 Modell 1 (MD1 @RDT 0x4024, TIM @0x2fcfc, CLUT-Zeile 481 =
 *       Warnstreifen/Umbrella), byteidentisch als shared_assets/RE2/KANISTER40A0.{MD1,TIM}; Lage/Drehung
 *       liefert die Engine (gator_re2_r41.c: frei im Wasser bzw. an der Part-6-Weltmatrix des Gators,
 *       T2 @0x801018d0-60). Gezeichnet wie ein RDT-Prop (main.c Prop-Pass: view x Welt, RTPT/NCLIP ueber
 *       objpoly_r38, Licht des aktiven Cuts), eigener TIM-Platz 53 (frei: render_pc.c RE15_TIM_SLOT_MAX 56,
 *       52 = RE2FX_TIM_SLOT, 50/51 = FX-Seiten).
 *   (2) die RE2-RAUMBANK-Saetze von ROOM40A0 (Mini-Bank GATOR40A0.VBS, tools/r41_re2gator/gator40a0_se_cut.py):
 *       Satz 1 = Explosionsbrueller `Se_on(0x020f0001)` @0x8010237c-88 (R1), Satz 0 = Todesbrueller
 *       `Se_on(0x020e0001)` @0x8010280c (R2, im Port nicht erreichbar). Laderpaar wie TUERSE (audio_pc.c 5c),
 *       Wiedergabe ueber dieselbe Stimmen-/Prioritaetsmaschine (re15_audio_se_play_bank).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_gator_re2_r41.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_objpoly_r38.h"
#include "re15_engine.h"        /* re15_render_textured_tri_lit */
#include "re15_audio.h"
#include "re15_vab.h"
#include "asset_root_pc.h"      /* re15_pc_read_re2 */
#include "../../../engine/src/gen/r41_gator40a0_bank.inc"

#define R41_KANISTER_TIM_SLOT 53

extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);

/* ---- (1) Kanister-Modell --------------------------------------------------------------------- */
static int        s_modell_state;       /* 0 ungeprueft, 1 geladen, -1 fehlt */
static uint8_t   *s_md1_raw;
static re15_md1_t s_md1;

static int modell_laden(void)
{
    if (s_modell_state) return s_modell_state > 0;
    s_modell_state = -1;
    int n = 0;
    s_md1_raw = re15_pc_read_re2("KANISTER40A0.MD1", &n);
    if (!s_md1_raw || re15_md1_parse(s_md1_raw, n, &s_md1) != 0 || s_md1.mesh_count < 1) {
        fprintf(stderr, "[gator-re2] shared_assets/RE2/KANISTER40A0.MD1 fehlt/unlesbar -> kein Kanister\n");
        return 0;
    }
    int tn = 0;
    uint8_t *tim_raw = re15_pc_read_re2("KANISTER40A0.TIM", &tn);
    re15_tim_t tim;
    if (!tim_raw || re15_tim_parse(tim_raw, tn, &tim) != 0) {
        fprintf(stderr, "[gator-re2] shared_assets/RE2/KANISTER40A0.TIM fehlt/unlesbar -> kein Kanister\n");
        free(tim_raw); return 0;
    }
    re15_render_pc_upload_tim_slot(&tim, R41_KANISTER_TIM_SLOT);
    free(tim_raw);                          /* der Platz haelt eine Kopie (Slot-Textur) */
    s_modell_state = 1;
    fprintf(stderr, "[gator-re2] Kanister-Modell geladen: %d Meshes, TIM-Platz %d\n",
            s_md1.mesh_count, R41_KANISTER_TIM_SLOT);
    return 1;
}

void re15_gr2_pc_zeichnen(const void *cam_view_v, int cx, int cy)
{
    int32_t pos[3], rot[9];
    if (!re15_gr2_kanister_welt(pos, rot)) return;
    if (!modell_laden()) return;
    const re15_camera_view_t *cam_view = (const re15_camera_view_t *)cam_view_v;
    re15_render_pc_bind_tim_slot(R41_KANISTER_TIM_SLOT);

    re15_actor_lightctx_t lw, lctx;
    if (g_re15_room_lights_ok && g_re15_active_cut >= 0 && g_re15_active_cut < g_re15_room_lights.cut_count) {
        re15_light_setup_actor(&g_re15_room_lights.cuts[g_re15_active_cut], pos, NULL, &lw);
        re15_light_ctx_rotate_for_bone(&lw, rot, &lctx);
    } else {
        memset(&lctx, 0, sizeof lctx);
    }
    int32_t bm[9], bt[3];
    re15_camera_compose_view_bone(cam_view, rot, pos, bm, bt);

    /* MESSSCHIENE (stderr, kein Verhalten): die ersten drei Bilder und jeder Stufenwechsel - Lage, Drehung,
     * Bildschirm-Kasten, Tiefe, Licht. Grund: Sichtlauf e 2026-10-23 - RE15_TRILOG zeigte 110 Dreiecke je Bild
     * auf Platz 53 im Kasten (98..115, 91..107) bei z 10875..13512, im Framedump war dort nur Dunkel. Ob das
     * Beleuchtung oder Verdeckung ist, entscheidet diese Zeile, nicht das Auge. */
    static int s_diag_n, s_diag_stufe = -2;
    const int stufe = re15_gr2_kanister_stufe();
    const int diag = (s_diag_n < 3) || (stufe != s_diag_stufe);
    int d_tris = 0, d_weg = 0, d_x0 = 9999, d_x1 = -9999, d_y0 = 9999, d_y1 = -9999, d_z0 = 99999, d_z1 = -1;
    int d_c[3] = { -1, -1, -1 };

    for (int mi = 0; mi < s_md1.mesh_count; mi++) {
        const re15_md1_mesh_t *hm = &s_md1.meshes[mi];
        for (int ti = 0; ti < hm->triangle_count; ti++) {
            const re15_md1_triangle_t *tr = &hm->triangles[ti];
            if (tr->v0 >= (uint32_t)hm->tri_vertex_count || tr->v1 >= (uint32_t)hm->tri_vertex_count
                || tr->v2 >= (uint32_t)hm->tri_vertex_count) continue;
            const re15_md1_vertex_t *vp[3] = { &hm->tri_vertices[tr->v0], &hm->tri_vertices[tr->v1], &hm->tri_vertices[tr->v2] };
            int ax[3], ay[3], wz = 0;
            if (!re15_objpoly_r38_tri_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz)) { d_weg++; continue; }
            d_tris++;
            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
            int po = (int)((uv->page & 0x000F) * 128);
            uint8_t c[3][3];
            for (int k = 0; k < 3; k++) {
                uint32_t ni = (k == 0) ? tr->n0 : (k == 1) ? tr->n1 : tr->n2;
                if (hm->tri_normals && ni < (uint32_t)hm->tri_normal_count)
                    re15_light_shade_vertex(&lctx, hm->tri_normals[ni].x, hm->tri_normals[ni].y, hm->tri_normals[ni].z, &c[k][0], &c[k][1], &c[k][2]);
                else { c[k][0] = g_re15_light_tint[0]; c[k][1] = g_re15_light_tint[1]; c[k][2] = g_re15_light_tint[2]; }
            }
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                         ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                         ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2,
                                         (int)uv->page, (int)uv->clut, wz,
                                         c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2]);
        }
        for (int qi = 0; qi < hm->quad_count; qi++) {
            const re15_md1_quad_t *qd = &hm->quads[qi];
            if (qd->v0 >= (uint32_t)hm->quad_vertex_count || qd->v1 >= (uint32_t)hm->quad_vertex_count
                || qd->v2 >= (uint32_t)hm->quad_vertex_count || qd->v3 >= (uint32_t)hm->quad_vertex_count) continue;
            const re15_md1_vertex_t *vp[4] = { &hm->quad_vertices[qd->v0], &hm->quad_vertices[qd->v1],
                                               &hm->quad_vertices[qd->v2], &hm->quad_vertices[qd->v3] };
            int ax[4], ay[4], wz = 0; uint8_t ok[2];
            if (!re15_objpoly_r38_quad_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz, ok)) { d_weg++; continue; }
            d_tris++;
            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];
            int po = (int)((uv->page & 0x000F) * 128);
            uint8_t c[4][3];
            for (int k = 0; k < 4; k++) {
                uint32_t ni = (k == 0) ? qd->n0 : (k == 1) ? qd->n1 : (k == 2) ? qd->n2 : qd->n3;
                if (hm->quad_normals && ni < (uint32_t)hm->quad_normal_count)
                    re15_light_shade_vertex(&lctx, hm->quad_normals[ni].x, hm->quad_normals[ni].y, hm->quad_normals[ni].z, &c[k][0], &c[k][1], &c[k][2]);
                else { c[k][0] = g_re15_light_tint[0]; c[k][1] = g_re15_light_tint[1]; c[k][2] = g_re15_light_tint[2]; }
            }
            if (diag) {
                for (int k = 0; k < 4; k++) {
                    if (ax[k] < d_x0) d_x0 = ax[k]; if (ax[k] > d_x1) d_x1 = ax[k];
                    if (ay[k] < d_y0) d_y0 = ay[k]; if (ay[k] > d_y1) d_y1 = ay[k];
                }
                if (wz < d_z0) d_z0 = wz; if (wz > d_z1) d_z1 = wz;
                if (d_c[0] < 0 || (int)c[0][0] + c[0][1] + c[0][2] > d_c[0] + d_c[1] + d_c[2]) {
                    d_c[0] = c[0][0]; d_c[1] = c[0][1]; d_c[2] = c[0][2];   /* hellster Scheitel 0 */
                }
            }
            if (ok[0])
                re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                             ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                             ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                             (int)uv->page, (int)uv->clut, wz,
                                             c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[3][0], c[3][1], c[3][2]);
            if (ok[1])
                re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                             ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                             ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2,
                                             (int)uv->page, (int)uv->clut, wz,
                                             c[0][0], c[0][1], c[0][2], c[3][0], c[3][1], c[3][2], c[2][0], c[2][1], c[2][2]);
        }
    }
    if (diag) {
        s_diag_n++; s_diag_stufe = stufe;
        fprintf(stderr, "[gator-re2] Kanister Stufe %d pos=(%d,%d,%d) rot=[%d %d %d|%d %d %d|%d %d %d] "
                        "Kasten=(%d..%d,%d..%d) z=%d..%d Dreiecke %d weg %d hellster=(%d,%d,%d) "
                        "ambient=(%d,%d,%d) tint=(%d,%d,%d) cut=%d\n",
                stufe, pos[0], pos[1], pos[2], rot[0], rot[1], rot[2], rot[3], rot[4], rot[5], rot[6], rot[7], rot[8],
                d_x0, d_x1, d_y0, d_y1, d_z0, d_z1, d_tris, d_weg, d_c[0], d_c[1], d_c[2],
                (int)lctx.ambient[0], (int)lctx.ambient[1], (int)lctx.ambient[2],
                (int)g_re15_light_tint[0], (int)g_re15_light_tint[1], (int)g_re15_light_tint[2], g_re15_active_cut);
    }
}

/* ---- (2) Raumbank ROOM40A0 (Mini-Bank) --------------------------------------------------------- */
static int        s_bank_state;         /* 0 ungeprueft, 1 geladen, -1 fehlt */
static re15_vab_t s_vab;
static uint8_t   *s_edt;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];

static int bank_laden(void)
{
    if (s_bank_state) return s_bank_state > 0;
    s_bank_state = -1;
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("GATOR40A0.VBS", &sz);
    if (!vbs) { fprintf(stderr, "[gator-re2] shared_assets/RE2/GATOR40A0.VBS fehlt -> Raumbrueller stumm\n"); return 0; }
    if ((unsigned)sz < R41_GATOR_VBD_OFF + R41_GATOR_VBD_SIZE || R41_GATOR_EDT_SIZE < 12) { free(vbs); return 0; }
    s_edt = (uint8_t *)malloc(R41_GATOR_EDT_SIZE);
    if (!s_edt) { free(vbs); return 0; }
    memcpy(s_edt, vbs, R41_GATOR_EDT_SIZE);
    uint32_t vh_off = (uint32_t)s_edt[R41_GATOR_EDT_SIZE-8] | ((uint32_t)s_edt[R41_GATOR_EDT_SIZE-7] << 8)
                    | ((uint32_t)s_edt[R41_GATOR_EDT_SIZE-6] << 16) | ((uint32_t)s_edt[R41_GATOR_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > R41_GATOR_EDT_SIZE
        || re15_vab_parse(s_edt + vh_off, (size_t)R41_GATOR_EDT_SIZE - vh_off, &s_vab) != 0) { free(vbs); return 0; }
    const uint8_t *vb = vbs + R41_GATOR_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > R41_GATOR_VBD_SIZE) continue;
        size_t cap = (vsz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_decoded[i] = pcm;
        s_decoded_len[i] = re15_vag_adpcm_decode(vb + off, vsz, pcm, cap);
    }
    free(vbs);
    s_bank_state = 1;
    return 1;
}

static void raum_se(int satz_re2)
{
    int satz = (satz_re2 == 15) ? R41_GATOR_SE_EXPLOSION : (satz_re2 == 14) ? R41_GATOR_SE_TOD : -1;
    int ok = bank_laden();
    /* Messzeile (Abnahme 0, M6): der Raum-Brueller war in der exe nicht nachweisbar - debug.log traegt
     * jetzt jeden Ruf des Hakens (15 = Explosion @0x8010237c-88, 14 = Tod). */
    fprintf(stderr, "[gator-re2] Raum-SE %d -> GATOR40A0.VBS Satz %d%s\n", satz_re2, satz,
            ok ? "" : " (Bank fehlt, stumm)");
    if (!ok || satz < 0) return;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

void re15_pc_gator_re2_init(void)
{
    re15_gr2_raum_se_hook = raum_se;
}
