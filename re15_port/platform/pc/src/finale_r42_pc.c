/*
 * finale_r42_pc.c — Runde 42 Strang finale, Stufe 1 Runde 3 (Plan S1, Pruefbericht s1 r2 R2): PC-Seite der RE2-Raumtonbank des Finales.
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (NUTZERWUNSCH 07.11.2026: Finale wie das RE2-Ende). Die Bank ist ROOM7020 snd0, bytegleich als
 * shared_assets/RE2/R702.EDT/.VH/.VB (RDT-Kopfworte +0x08/+0x0C/+0x10 = @0x05C90/@0x05D50/@0x06B70, tools/r42_finale/re2_r702_esp.py,
 * Riegel unit_r42_finale_assets). Gespielt wird Satz 0x0F im Einschlagbild (Se_on 0x020F0001 @0x80103b54-5c, CDEMD0 EM2B-Overlay,
 * engine/src/finale_r42.c Phase 7); Satz 0x0E (Landung des Werfers) folgt mit der Wurfszene (Stufe 3).
 * Lader = Muster dining_r38_pc.c lade_klopfen (re15_vab_parse / re15_vag_adpcm_decode), Wiedergabe ueber dieselbe Stimmen-Maschine
 * wie jede SE (re15_audio_se_play_bank -> se_play_layers). PORT-WAHL: nicht positional (RE2 a1 = Lage der Form), wie die uebrigen
 * RE2-Raumbaenke im Port. Fehlt die Bank: EINE Protokollzeile, danach still stumm.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_engine.h"
#include "re15_finale_r42.h"
#include "asset_root_pc.h"             /* re15_pc_read_re2 */
#include "re15_finale_wurf_r42.h"      /* Stufe 3: Werfer-Lage */
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_camera.h"
#include "re15_light.h"
#include "re15_objpoly_r38.h"

extern long re15_audio_cap_tick(void);   /* audio_pc.c: Mitschnitt-Takt */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_state = 0;           /* 0 ungeprueft, 1 geladen, -1 fehlt */
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int        s_edt_count = 0;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt = 0;

static int laden(void)
{
    int es = 0, hs = 0, vs = 0;
    uint8_t *edt = re15_pc_read_re2(RE15_FI_TONBANK ".EDT", &es);
    uint8_t *vh  = re15_pc_read_re2(RE15_FI_TONBANK ".VH", &hs);
    uint8_t *vb  = re15_pc_read_re2(RE15_FI_TONBANK ".VB", &vs);
    if (!edt || !vh || !vb || es < 4 || re15_vab_parse(vh, (size_t)hs, &s_vab) != 0) {
        free(edt); free(vh); free(vb); return -1;
    }
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, sz = s_vab.samples[i].size;
        if (sz == 0 || off + sz > (uint32_t)vs) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_decoded[i]     = pcm;
        s_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    free(vh); free(vb);
    s_edt = edt;
    s_edt_count = es / 4;
    return 0;
}

static void spielen(int satz)
{
    if (!s_state) {
        s_state = laden() == 0 ? 1 : -1;
        fprintf(stderr, "[finale-se] Raumbank %s (RE2 ROOM7020 snd0) %s (%d VAGs, %d Saetze)\n", RE15_FI_TONBANK,
                s_state == 1 ? "geladen" : "FEHLT -> stumm", s_vab.vag_count, s_edt_count);
    }
    if (s_state != 1 || satz < 0 || satz >= s_edt_count) return;
    s_gespielt++;
    fprintf(stderr, "[finale-se] Se_on Raumbank %s Satz 0x%02X F%u capTick %ld\n", RE15_FI_TONBANK, satz,
            (unsigned)g_engine.frame_count, re15_audio_cap_tick());
    if (!g_audio.initialized) return;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

unsigned re15_pc_finale_r42_gespielt(void) { return s_gespielt; }

extern int re15_pc_tuer_ton_vorspielen(const re15_door_seq_anfrage_t *a);   /* door_scene_pc.c */
void re15_pc_finale_r42_init(void)
{
    re15_finale_r42_se_hook(spielen);
    re15_finale_wurf_tuer_ton_hook(re15_pc_tuer_ton_vorspielen);   /* Stufe 3 Runde 3: Tuerton beim Abgang */
}

/* ---- Stufe 3 (Plan S5/D13): der geworfene Raketenwerfer. Seit Runde 3 = das Waffenband von PLD/PL00W12.PLW (Kopf
 * re15_finale_wurf_r42.h RE15_FW_TEIL: Leons Raketenwerfer, derselbe, den der Spieler nach dem Aufheben haelt): dir[2] MD1, nur die
 * Dreiecke mit v >= 224 (Waffenband, 38 Stueck), Textur dir[3] (56 x 32, 8 bpp), UV - (72, 224) = Lage der Textur im Komposit
 * (main.c "X = 200, Y = 480"). Runde 1/2 zeigten RE2s Raumobjekt R702 (ROOM7020 Objekt 0); der Plan-D13-Vorbehalt U12 ("Aussehen gegen
 * PL00W12.PLW") ist damit erledigt. Gezeichnet wie ein RDT-Prop und wie der Kanister (kanister2080_pc.c: view x Welt, RTPT/NCLIP ueber
 * objpoly_r38, Licht des aktiven Cuts). TIM-Platz 57 (render_pc.c RE15_TIM_SLOT_MAX: "57 = bis v0.8.30 Tunnellampen des G5-Auftritts,
 * seit Runde 42 frei"). */
#define FW_TIM_SLOT 57
extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
static int        s_w_modell;          /* 0 ungeprueft, 1 geladen, -1 fehlt */
static uint8_t   *s_w_plw;
static re15_md1_t s_w_md1;
static int        s_w_band;            /* Zahl der Waffenband-Dreiecke */

static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static int werfer_laden(void)
{
    if (s_w_modell) return s_w_modell > 0;
    s_w_modell = -1;
    int n = 0;
    s_w_plw = re15_pc_read_any(RE15_FW_W12_DATEI, &n);
    if (!s_w_plw || n < 16) { fprintf(stderr, "[finale] %s fehlt -> kein Werfer-Modell\n", RE15_FW_W12_DATEI); return 0; }
    const uint32_t dir = le32(s_w_plw);
    if (dir + 16 > (uint32_t)n) { fprintf(stderr, "[finale] %s: Verzeichnis kaputt\n", RE15_FW_W12_DATEI); return 0; }
    const uint32_t d2 = le32(s_w_plw + dir + 8), d3 = le32(s_w_plw + dir + 12);   /* dir[2] MD1, dir[3] TIM (bis zum Verzeichnis) */
    if (!(d2 < d3 && d3 < dir) || re15_md1_parse(s_w_plw + d2, (int)(d3 - d2), &s_w_md1) != 0 || s_w_md1.mesh_count < 1) {
        fprintf(stderr, "[finale] %s dir[2] unlesbar -> kein Werfer-Modell\n", RE15_FW_W12_DATEI); return 0;
    }
    re15_tim_t tim;
    if (re15_tim_parse(s_w_plw + d3, (int)(dir - d3), &tim) != 0) {
        fprintf(stderr, "[finale] %s dir[3] unlesbar -> kein Werfer-Modell\n", RE15_FW_W12_DATEI); return 0;
    }
    re15_render_pc_upload_tim_slot(&tim, FW_TIM_SLOT);
    s_w_band = 0;
    const re15_md1_mesh_t *hm = &s_w_md1.meshes[0];
    for (int ti = 0; ti < hm->triangle_count; ti++) {
        const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
        if (uv->v0 >= RE15_FW_W12_V_MIN && uv->v1 >= RE15_FW_W12_V_MIN && uv->v2 >= RE15_FW_W12_V_MIN) s_w_band++;
    }
    s_w_modell = 1;
    fprintf(stderr, "[finale] Werfer-Modell %s geladen: Waffenband %d von %d Dreiecken, TIM %dx%d %d bpp, TIM-Platz %d\n",
            RE15_FW_W12_DATEI, s_w_band, hm->triangle_count, tim.width, tim.height, tim.bpp, FW_TIM_SLOT);
    return 1;
}

static unsigned s_w_dreiecke;
unsigned re15_pc_finale_wurf_dreiecke(void) { return s_w_dreiecke; }

void re15_pc_finale_wurf_zeichnen(const void *cam_view_v, int cx, int cy)
{
    int32_t pos[3], rot[9];
    if (!re15_finale_wurf_werfer_lage(pos, rot)) return;
    if (!werfer_laden()) return;
    const re15_camera_view_t *cam_view = (const re15_camera_view_t *)cam_view_v;
    re15_render_pc_bind_tim_slot(FW_TIM_SLOT);
    re15_actor_lightctx_t lw, lctx;
    if (g_re15_room_lights_ok && g_re15_active_cut >= 0 && g_re15_active_cut < g_re15_room_lights.cut_count) {
        re15_light_setup_actor(&g_re15_room_lights.cuts[g_re15_active_cut], pos, NULL, &lw);
        re15_light_ctx_rotate_for_bone(&lw, rot, &lctx);
    } else {
        memset(&lctx, 0, sizeof lctx);
    }
    int32_t bm[9], bt[3];
    re15_camera_compose_view_bone(cam_view, rot, pos, bm, bt);
    unsigned tris = 0;
    int x0 = 9999, x1 = -9999, y0 = 9999, y1 = -9999;
    {
        const re15_md1_mesh_t *hm = &s_w_md1.meshes[0];
        for (int ti = 0; ti < hm->triangle_count; ti++) {
            const re15_md1_triangle_t *tr = &hm->triangles[ti];
            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];
            if (uv->v0 < RE15_FW_W12_V_MIN || uv->v1 < RE15_FW_W12_V_MIN || uv->v2 < RE15_FW_W12_V_MIN) continue;   /* Handband */
            if (tr->v0 >= (uint32_t)hm->tri_vertex_count || tr->v1 >= (uint32_t)hm->tri_vertex_count
                || tr->v2 >= (uint32_t)hm->tri_vertex_count) continue;
            const re15_md1_vertex_t *vp[3] = { &hm->tri_vertices[tr->v0], &hm->tri_vertices[tr->v1], &hm->tri_vertices[tr->v2] };
            int ax[3], ay[3], wz = 0;
            if (!re15_objpoly_r38_tri_mesh(vp, bm, bt, cam_view->fov_screen_dist, cx, cy, ax, ay, &wz)) continue;
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
            /* UV in die eigene TIM (Komposit-Ursprung (72, 224)), Seite ohne Spaltenbasis (eigene Textur, nicht die Hautseite) */
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 - RE15_FW_W12_U0, (int)uv->v0 - RE15_FW_W12_V0,
                                         ax[1], ay[1], (int)uv->u1 - RE15_FW_W12_U0, (int)uv->v1 - RE15_FW_W12_V0,
                                         ax[2], ay[2], (int)uv->u2 - RE15_FW_W12_U0, (int)uv->v2 - RE15_FW_W12_V0,
                                         (int)(uv->page & 0xFFF0u), (int)uv->clut, wz,
                                         c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2], c[2][0], c[2][1], c[2][2]);
            tris++;
        }
    }
    s_w_dreiecke += tris;
    /* Messzeile bei jedem Zustands- und Schnittwechsel (Hand/Flug/liegt) und wenn er im selben Zustand/Schnitt ins Bild kommt (Runde 3:
     * der Schnitt im Flug kommt, bevor er im Bild des Spielers ist) */
    static int s_alt_z = -1, s_alt_cut = -99, s_alt_sicht = 0;
    const int z = re15_finale_wurf_werfer_zustand();
    if (z != s_alt_z || g_re15_active_cut != s_alt_cut || (tris > 0 && !s_alt_sicht)) {
        s_alt_z = z; s_alt_cut = g_re15_active_cut; s_alt_sicht = tris > 0;
        fprintf(stderr, "[finale] F%u Werfer gezeichnet: Zustand %d Lage (%d,%d,%d) Cut %d Flaechen %u Kasten (%d..%d,%d..%d)\n",
                (unsigned)g_engine.frame_count, z, (int)pos[0], (int)pos[1], (int)pos[2], g_re15_active_cut, tris, x0, x1, y0, y1);
    }
}
