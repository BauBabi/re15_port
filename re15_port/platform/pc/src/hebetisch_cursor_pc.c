/* hebetisch_cursor_pc.c — zeichnet den HEBETISCH-CURSOR in Irons' Buero (ROOM1150/1151) auf dem PC.
 *
 * Runde 34 Nacht, Spur B. Herleitung und Belege: include/re15_hebetisch_cursor.h; Zustand, Takt und
 * Treffertest liegen plattformfrei in engine/src/hebetisch_cursor_1150.c. Hier nur das Bild, und
 * zwar GENAU so, wie die Raum-Prop-Schleife in main.c den Cursor in ROOM11F0 zeichnet:
 *   * Modell/Textur = ROOM11F0.RDT MD1 @0x001928 / TIM @0x018DAC (eingebacken); Textur in TIM-Platz
 *     RE15_HC_TIM_SLOT = RE15_TIM_SLOT_PROP(8) = 28, neu hochgeladen je Cursor-Sitzung (11F0 belegt
 *     denselben Platz mit seinem Prop 8);
 *   * Abbildung ueber die Cut-10-Kamera von ROOM11F0 (re15_hebetisch_cursor_matrix), gezeichnet ueber
 *     das Cut-4-Bild — nur solange Cut 4 angezeigt wird (sub04 @0x0FB2 `29 04`);
 *   * Licht = Lichtsatz ROOM11F0 Cut 10 @0x0718 wie in 11F0 (re15_light_setup_actor mit der
 *     Weltlage (x,-900,z) + re15_light_ctx_rotate_for_bone mit der Einheitsdrehung, dann je
 *     Punktnormale re15_light_shade_vertex — dieselben drei Aufrufe wie main.c fuer Raum-Props);
 *   * Zeichenfolge je Flaeche wie dort (Dreieck v0,v1,v2; Viereck (v0,v1,v3) + (v0,v3,v2); UV-Seite
 *     page&0xF * 128; CLUT-Wort je Flaeche), damit das Kreuz dieselben Pixel trifft (Abtastphase
 *     re15_abtastphase.h, Runde 26/27);
 *   * opak (11F0 obj+0x0C = 0 -> ABE-Bit 0, main.c zu LAB_80040914 @0x800409a8/@0x800409b0);
 *   * Tiefe = eigene Kameratiefe - RE15_HC_TIEFE_VERSATZ: vor jedem Raum-Dreieck und jeder PRI-Maske,
 *     untereinander nach Tiefe wie in 11F0 (PORT-WAHL, Kopf).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_hebetisch_cursor.h"
#include "re15_engine.h"     /* re15_render_textured_tri_lit */
#include "re15_light.h"      /* g_re15_active_cut, Lichtsatz + NCCT-Schattierung */
#include "re15_md1.h"
#include "re15_tim.h"

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_set_tri_blend(int m);

static re15_md1_t       s_md1;
static int              s_md1_ok = -1;      /* -1 = noch nicht geparst */
static re15_light_set_t s_licht;
static int              s_licht_ok = -1;
static unsigned         s_hochgeladen = 0;  /* Sitzung, deren Textur im Platz liegt (0 = keine) */

static void schattiere(const re15_actor_lightctx_t *ctx, const re15_md1_vertex_t *nrm, int n_ok,
                       uint8_t *r, uint8_t *g, uint8_t *b)
{
    if (ctx && n_ok) { re15_light_shade_vertex(ctx, nrm->x, nrm->y, nrm->z, r, g, b); return; }
    *r = *g = *b = RE15_HC_TINT_OHNE_NORMALE;
}

void re15_hebetisch_cursor_zeichnen_pc(void)
{
    int32_t x = 0, z = 0;
    if (!re15_hebetisch_cursor_sicht(&x, &z)) return;
    if (g_re15_active_cut != RE15_HC_CUT) return;             /* nur ueber dem Cut-4-Bild */

    if (s_md1_ok < 0) {
        int n = 0;
        const uint8_t *b = re15_hebetisch_cursor_md1_bytes(&n);
        s_md1_ok = (b && re15_md1_parse(b, n, &s_md1) == 0) ? 1 : 0;
        if (!s_md1_ok) fprintf(stderr, "[hebetisch-cursor] MD1 nicht lesbar\n");
    }
    if (!s_md1_ok) return;
    if (s_licht_ok < 0) {
        int n = 0;
        const uint8_t *b = re15_hebetisch_cursor_licht_bytes(&n);
        s_licht_ok = (b && re15_light_parse(b, (size_t)n, &s_licht) == 0 && s_licht.cut_count >= 1) ? 1 : 0;
    }

    const unsigned sitzung = re15_hebetisch_cursor_sitzung();
    if (s_hochgeladen != sitzung) {
        int n = 0;
        const uint8_t *b = re15_hebetisch_cursor_tim_bytes(&n);
        re15_tim_t tim;
        if (!b || re15_tim_parse(b, n, &tim) != 0) {
            fprintf(stderr, "[hebetisch-cursor] TIM nicht lesbar\n");
            return;
        }
        re15_render_pc_upload_tim_slot(&tim, RE15_HC_TIM_SLOT);
        s_hochgeladen = sitzung;
        fprintf(stderr, "[hebetisch-cursor] Sitzung %u: Cursor-Textur in TIM-Platz %d, Cut %d, Licht %s\n",
                sitzung, RE15_HC_TIM_SLOT, g_re15_active_cut, s_licht_ok ? "11F0 Cut 10" : "neutral");
    }
    re15_render_pc_bind_tim_slot(RE15_HC_TIM_SLOT);
    re15_render_pc_set_tri_blend(0);

    int32_t rot[9], trans[3];
    int h = 0;
    re15_hebetisch_cursor_matrix(x, z, rot, trans, &h);

    /* Licht wie main.c fuer Raum-Props: Weltlage = gezeichnete Lage (Typ-4-Anhebung), Drehung 0. */
    re15_actor_lightctx_t lwelt, lctx;
    const re15_actor_lightctx_t *lp = NULL;
    if (s_licht_ok) {
        static const int32_t k_ident[9] = { 4096, 0, 0, 0, 4096, 0, 0, 0, 4096 };
        const int32_t lage[3] = { x, RE15_HC_OBJ_Y + RE15_HC_TYP4_ANHEBUNG, z };
        re15_light_setup_actor(&s_licht.cuts[0], lage, NULL, &lwelt);
        re15_light_ctx_rotate_for_bone(&lwelt, k_ident, &lctx);
        lp = &lctx;
    }

    for (int mi = 0; mi < s_md1.mesh_count; mi++) {
        const re15_md1_mesh_t *m = &s_md1.meshes[mi];
        for (int ti = 0; ti < m->triangle_count; ti++) {
            const re15_md1_triangle_t *tr = &m->triangles[ti];
            if (tr->v0 >= (uint32_t)m->tri_vertex_count || tr->v1 >= (uint32_t)m->tri_vertex_count ||
                tr->v2 >= (uint32_t)m->tri_vertex_count) continue;
            const re15_md1_vertex_t *vp[3] = { &m->tri_vertices[tr->v0], &m->tri_vertices[tr->v1],
                                               &m->tri_vertices[tr->v2] };
            int ax[3], ay[3], ok = 1;
            long wz = 0;
            for (int v = 0; v < 3 && ok; v++) {
                int32_t vz = 0;
                ok = re15_hebetisch_cursor_projiziere(rot, trans, h, vp[v]->x, vp[v]->y, vp[v]->z,
                                                      &ax[v], &ay[v], &vz);
                wz += vz;
            }
            if (!ok) continue;
            const int n_ok = m->tri_normals && tr->n0 < (uint32_t)m->tri_normal_count &&
                             tr->n1 < (uint32_t)m->tri_normal_count && tr->n2 < (uint32_t)m->tri_normal_count;
            uint8_t c[3][3];
            schattiere(lp, n_ok ? &m->tri_normals[tr->n0] : NULL, n_ok, &c[0][0], &c[0][1], &c[0][2]);
            schattiere(lp, n_ok ? &m->tri_normals[tr->n1] : NULL, n_ok, &c[1][0], &c[1][1], &c[1][2]);
            schattiere(lp, n_ok ? &m->tri_normals[tr->n2] : NULL, n_ok, &c[2][0], &c[2][1], &c[2][2]);
            const re15_md1_tri_uv_t *uv = &m->triangle_uvs[ti];
            const int po = (int)((uv->page & 0x000F) * 128);
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                         ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                         ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2,
                                         0, (int)uv->clut, (int)(wz / 3) - RE15_HC_TIEFE_VERSATZ,
                                         c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2],
                                         c[2][0], c[2][1], c[2][2]);
        }
        for (int qi = 0; qi < m->quad_count; qi++) {
            const re15_md1_quad_t *q = &m->quads[qi];
            if (q->v0 >= (uint32_t)m->quad_vertex_count || q->v1 >= (uint32_t)m->quad_vertex_count ||
                q->v2 >= (uint32_t)m->quad_vertex_count || q->v3 >= (uint32_t)m->quad_vertex_count)
                continue;
            const re15_md1_vertex_t *vp[4] = { &m->quad_vertices[q->v0], &m->quad_vertices[q->v1],
                                               &m->quad_vertices[q->v2], &m->quad_vertices[q->v3] };
            int ax[4], ay[4], ok = 1;
            long wz = 0;
            for (int v = 0; v < 4 && ok; v++) {
                int32_t vz = 0;
                ok = re15_hebetisch_cursor_projiziere(rot, trans, h, vp[v]->x, vp[v]->y, vp[v]->z,
                                                      &ax[v], &ay[v], &vz);
                wz += vz;
            }
            if (!ok) continue;
            const int n_ok = m->quad_normals && q->n0 < (uint32_t)m->quad_normal_count &&
                             q->n1 < (uint32_t)m->quad_normal_count && q->n2 < (uint32_t)m->quad_normal_count &&
                             q->n3 < (uint32_t)m->quad_normal_count;
            uint8_t c[4][3];
            schattiere(lp, n_ok ? &m->quad_normals[q->n0] : NULL, n_ok, &c[0][0], &c[0][1], &c[0][2]);
            schattiere(lp, n_ok ? &m->quad_normals[q->n1] : NULL, n_ok, &c[1][0], &c[1][1], &c[1][2]);
            schattiere(lp, n_ok ? &m->quad_normals[q->n2] : NULL, n_ok, &c[2][0], &c[2][1], &c[2][2]);
            schattiere(lp, n_ok ? &m->quad_normals[q->n3] : NULL, n_ok, &c[3][0], &c[3][1], &c[3][2]);
            const re15_md1_quad_uv_t *uv = &m->quad_uvs[qi];
            const int po = (int)((uv->page & 0x000F) * 128);
            const int tiefe = (int)(wz / 4) - RE15_HC_TIEFE_VERSATZ;
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                         ax[1], ay[1], (int)uv->u1 + po, (int)uv->v1,
                                         ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                         0, (int)uv->clut, tiefe,
                                         c[0][0], c[0][1], c[0][2], c[1][0], c[1][1], c[1][2],
                                         c[3][0], c[3][1], c[3][2]);
            re15_render_textured_tri_lit(ax[0], ay[0], (int)uv->u0 + po, (int)uv->v0,
                                         ax[3], ay[3], (int)uv->u3 + po, (int)uv->v3,
                                         ax[2], ay[2], (int)uv->u2 + po, (int)uv->v2,
                                         0, (int)uv->clut, tiefe,
                                         c[0][0], c[0][1], c[0][2], c[3][0], c[3][1], c[3][2],
                                         c[2][0], c[2][1], c[2][2]);
        }
    }
}
