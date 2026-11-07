/* test_r41_form_emd.c — Runde 41 Form-Werkzeug: das Probemodell aus tools/form_emd.py durch die echten Parser des Ports.
 *
 * tests/unit/form_probe/FORM_PROBE.EMD/.TIM schreibt `python re15_port/tools/form_emd.py probe --aus re15_port/tests/unit/form_probe`.
 * Drei Knochen aus Quadern, Spitze aus Dreiecken, zwei Clips (2 + 32 Bilder); die Erwartungswerte unten stehen von Hand in probe_form().
 * Keine Original-Konstanten: dieser Test haelt fest, dass der Schreiber das Format trifft, das
 * re2_emd_parse_bank / re15_md1_parse / re15_tim_parse lesen, inklusive freier Knochenzahl (Bank-Bit bb_form).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_tim.h"
#include "re15_birkin_formen.h"
#include "re2_ems.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}

static void pruefe_skelett(const re15_enemy_bank_t *eb)
{
    const re15_emd_skeleton_t *sk = &eb->skel;
    CHECK(sk->bone_count == 3, "Knochen %d", sk->bone_count);
    CHECK(sk->keyframe_size_bytes == 28, "Keyframe-Groesse %d (12 + 3*36 Bit, auf 4 Byte)", sk->keyframe_size_bytes);
    CHECK(sk->bone_parent[0] == -1 && sk->bone_parent[1] == 0 && sk->bone_parent[2] == 0, "Eltern");
    CHECK(sk->bone_relative_pos[1][0] == 1100 && sk->bone_relative_pos[1][1] == -300 && sk->bone_relative_pos[1][2] == 0, "Versatz Kopf");
    CHECK(sk->bone_relative_pos[2][0] == 600 && sk->bone_relative_pos[2][1] == 300 && sk->bone_relative_pos[2][2] == 700, "Versatz Arm");
}

static void pruefe_clips(const re15_enemy_bank_t *eb)
{
    const re15_emd_animation_t *an = &eb->anim;
    const re15_emd_skeleton_t *sk = &eb->skel;
    CHECK(an->clip_count == 2, "Clips %d", an->clip_count);
    CHECK(an->clips[0].frame_count == 2 && an->clips[1].frame_count == 32, "Bilder %d %d",
          an->clips[0].frame_count, an->clips[1].frame_count);
    CHECK(an->clips[1].first_frame == 2, "Clip 1 beginnt bei %d", an->clips[1].first_frame);
    /* Clip 1 Bild 0 ist inhaltsgleich mit Clip 0 Bild 0: der Schreiber legt beide auf Keyframe 0 */
    CHECK((re15_emd_get_frame_entry(an, 1, 0) & 0xFFFu) == 0u, "Pool: Bild 0 teilt Keyframe 0");
    CHECK((re15_emd_get_frame_entry(an, 1, 8) & 0xFFFu) == 9u, "Keyframe von Bild 8");
    CHECK((re15_emd_get_frame_entry(an, 1, 10) & 0xFFFFF000u) == 0x00010000u, "Trefferbit Bild 10");
    CHECK((re15_emd_get_frame_entry(an, 1, 20) & 0xFFFFF000u) == 0x38000000u, "Ton 3 Bild 20");
    CHECK((re15_emd_get_frame_entry(an, 1, 21) & 0xFFFFF000u) == 0u, "Bild 21 ohne Flags");
    {
        int kf = (int)(re15_emd_get_frame_entry(an, 1, 8) & 0xFFFu);
        int16_t x, y, z;
        CHECK(re15_emd_get_keyframe_position(sk, kf, &x, &y, &z) && x == 0 && y == -940 && z == 0, "Wurzel Bild 8: %d %d %d", x, y, z);
        CHECK(re15_emd_get_keyframe_speed(sk, kf, &x, &y, &z) && x == 56 && y == 0 && z == 0, "Spur Bild 8: %d %d %d", x, y, z);
        CHECK(re15_emd_get_keyframe_angles(sk, kf, 1, &x, &y, &z) && x == 0 && y == 300 && z == 0, "Winkel Kopf Bild 8");
        CHECK(re15_emd_get_keyframe_angles(sk, kf, 2, &x, &y, &z) && x == 0 && y == 0 && z == -700, "Winkel Arm Bild 8: %d (Vorzeichen)", z);
        kf = (int)(re15_emd_get_frame_entry(an, 1, 24) & 0xFFFu);
        CHECK(re15_emd_get_keyframe_angles(sk, kf, 1, &x, &y, &z) && y == -300, "Winkel Kopf Bild 24: %d", y);
        CHECK(re15_emd_get_keyframe_angles(sk, kf, 2, &x, &y, &z) && z == 700, "Winkel Arm Bild 24: %d", z);
    }
}

static void pruefe_pose(const re15_enemy_bank_t *eb)
{
    re15_skel_pose_t p[RE15_EMD_MAX_BONES];
    CHECK(re15_skel_compute_pose(&eb->skel, 1, p) == 0, "Pose");
    CHECK(p[0].trans[0] == 0 && p[0].trans[1] == -1000 && p[0].trans[2] == 0, "Wurzel %d %d %d", p[0].trans[0], p[0].trans[1], p[0].trans[2]);
    CHECK(p[1].trans[0] == 1100 && p[1].trans[1] == -1300 && p[1].trans[2] == 0, "Kopf %d %d %d", p[1].trans[0], p[1].trans[1], p[1].trans[2]);
    CHECK(p[2].trans[0] == 600 && p[2].trans[1] == -700 && p[2].trans[2] == 700, "Arm %d %d %d", p[2].trans[0], p[2].trans[1], p[2].trans[2]);
}

/* Regel des Zeichners fuer Typ 0x30 (re15_nclip_r41_weg): sichtbar, wenn cross(b-a, c-a) gegen die Aussennormale zeigt. */
static int gewunden_wie_regel(const re15_md1_mesh_t *m, const re15_md1_quad_t *q)
{
    const re15_md1_vertex_t *a = &m->quad_vertices[q->v0], *b = &m->quad_vertices[q->v1], *c = &m->quad_vertices[q->v2];
    const re15_md1_vertex_t *n = &m->quad_normals[q->n0];
    int64_t ux = b->x - a->x, uy = b->y - a->y, uz = b->z - a->z, vx = c->x - a->x, vy = c->y - a->y, vz = c->z - a->z;
    int64_t kx = uy * vz - uz * vy, ky = uz * vx - ux * vz, kz = ux * vy - uy * vx;
    return kx * n->x + ky * n->y + kz * n->z < 0;
}

static int gewunden_wie_regel_d(const re15_md1_mesh_t *m, const re15_md1_triangle_t *t)
{
    const re15_md1_vertex_t *a = &m->tri_vertices[t->v0], *b = &m->tri_vertices[t->v1], *c = &m->tri_vertices[t->v2];
    const re15_md1_vertex_t *n = &m->tri_normals[t->n2];
    int64_t ux = b->x - a->x, uy = b->y - a->y, uz = b->z - a->z, vx = c->x - a->x, vy = c->y - a->y, vz = c->z - a->z;
    int64_t kx = uy * vz - uz * vy, ky = uz * vx - ux * vz, kz = ux * vy - uy * vx;
    return kx * n->x + ky * n->y + kz * n->z < 0;
}

static void pruefe_mesh(const re15_enemy_bank_t *eb)
{
    const re15_md1_t *md = &eb->md1;
    CHECK(md->mesh_count == 3, "Meshes %d", md->mesh_count);
    for (int i = 0; i < md->mesh_count && i < 3; i++) {
        const re15_md1_mesh_t *m = &md->meshes[i];
        const int kopf = (i == 1);                     /* Kopf: Quader plus Spitze aus vier Dreiecken */
        CHECK(m->triangle_count == (kopf ? 4 : 0) && m->quad_count == 6, "Mesh %d: %d Dreiecke, %d Vierecke", i, m->triangle_count, m->quad_count);
        CHECK(m->quad_vertex_count == (kopf ? 13 : 8) && m->quad_normal_count == (kopf ? 11 : 6), "Mesh %d: %d Ecken, %d Normalen", i, m->quad_vertex_count, m->quad_normal_count);
        for (int q = 0; q < m->quad_count; q++)
            CHECK(gewunden_wie_regel(m, &m->quads[q]), "Mesh %d Viereck %d falsch gewunden", i, q);
        for (int t = 0; t < m->triangle_count; t++) {
            /* Tri- und Quad-Teil teilen den Eckblock (wie das Muster); die Spitze zeigt mit ihren Normalen nach aussen (+X) */
            CHECK(m->tri_vertices == m->quad_vertices && m->tri_normals == m->quad_normals, "Mesh %d: getrennte Bloecke", i);
            CHECK(m->tri_vertices[m->triangles[t].v2].x == 600, "Dreieck %d: dritte Ecke ist nicht die Spitze", t);
            CHECK(m->tri_normals[m->triangles[t].n2].x > 0, "Dreieck %d: Normale zeigt nach innen", t);
            CHECK(gewunden_wie_regel_d(m, &m->triangles[t]), "Dreieck %d falsch gewunden", t);
        }
    }
    if (md->mesh_count < 3) return;
    {   /* Kopf: Texel-x 130 liegt in TPage 1 (main.c page_off), CLUT-Wort Zeile 0 = 480 << 6 */
        const re15_md1_quad_uv_t *uv = &md->meshes[1].quad_uvs[0];
        CHECK((uv->page & 0x0F) == 1 && (uv->page & 0x80), "Seite 0x%x", uv->page);
        CHECK((int)uv->u0 + (uv->page & 0x0F) * 128 == 130, "Texel-x %d", (int)uv->u0 + (uv->page & 0x0F) * 128);
        CHECK(uv->clut == (480 << 6), "CLUT 0x%x", uv->clut);
    }
    {   /* Arm: zweite CLUT-Zeile */
        const re15_md1_quad_uv_t *uv = &md->meshes[2].quad_uvs[0];
        CHECK(uv->clut == (481 << 6), "CLUT 0x%x", uv->clut);
    }
    CHECK(md->meshes[0].quad_vertices[0].x == -900 && md->meshes[0].quad_vertices[0].y == -400 && md->meshes[0].quad_vertices[0].z == -500, "Ecke 0");
}

static void pruefe_tim(const uint8_t *t, size_t n)
{
    re15_tim_t tim;
    memset(&tim, 0, sizeof tim);
    CHECK(re15_tim_parse(t, (int)n, &tim) == 0, "TIM nicht lesbar");
    CHECK(tim.bpp == 8 && tim.has_clut, "bpp %d", tim.bpp);
    CHECK(tim.width == 256 && tim.height == 64, "TIM %dx%d", tim.width, tim.height);
    CHECK(tim.clut_entries == 512 && tim.clut_y == 480, "CLUT %d, y %d", tim.clut_entries, tim.clut_y);
    {
        const uint8_t *px = (const uint8_t *)tim.pixels;
        CHECK(px[0] == 5 && px[32 * 256 + 30] == 0, "Pixel %d %d", px[0], px[32 * 256 + 30]);   /* Gitter, Loch */
        CHECK(re15_tim_texel_argb(tim.clut[0]) == 0u, "Index 0 muss transparent sein");
        CHECK((re15_tim_texel_argb(tim.clut[256 + 5]) & 0xFF000000u) == 0xFF000000u, "Zeile 1 Index 5 opak");
        CHECK(tim.clut[5] != tim.clut[256 + 5], "CLUT-Zeilen gleich");
    }
}

int main(void)
{
    static re15_enemy_bank_t eb;
    size_t en = 0, tn = 0;
    uint8_t *emd = slurp(RE15_FORM_PROBE_DIR "/FORM_PROBE.EMD", &en);
    uint8_t *tim = slurp(RE15_FORM_PROBE_DIR "/FORM_PROBE.TIM", &tn);
    CHECK(emd && tim, "Probemodell fehlt (python re15_port/tools/form_emd.py probe --aus re15_port/tests/unit/form_probe)");
    if (!emd || !tim) return 1;
    {
        const int rc = re2_emd_parse_bank(emd, en, &eb);
        CHECK(rc == 0, "re2_emd_parse_bank %d", rc);
        if (rc != 0) return 1;
    }
    CHECK(eb.loco_ok == 1, "Paar 1");
    CHECK(eb.own_ok == 0 && eb.victim_ok == 0, "leere Paare: own %d victim %d", eb.own_ok, eb.victim_ok);
    CHECK(eb.morph_ok == 0 && eb.morph_mask == 0u, "Morphblock Maske 0");
    pruefe_skelett(&eb);
    pruefe_clips(&eb);
    pruefe_pose(&eb);
    pruefe_mesh(&eb);
    pruefe_tim(tim, tn);

    {   /* Freie Knochenzahl: der Lader setzt bb_form, der Tick erkennt die Bank daran (bei 3 Knochen sonst nicht). */
        re15_enemy_bank_t *b = re15_enemy_alloc(0x30u);
        re15_actor_t a;
        CHECK(b != NULL, "Bank");
        if (b) {
            uint8_t typ = b->type;
            memset(b, 0, sizeof *b); b->type = typ;
            CHECK(re2_emd_parse_bank(emd, en, b) == 0, "Parse in die Bank");
            b->re2_rig = 1; b->ok = 1;
            memset(&a, 0, sizeof a); a.type = 0x30u;
            CHECK(!re15_bb_bank_da(&a), "3 Knochen ohne bb_form gelten nicht als Form");
            b->bb_form = 1;
            CHECK(re15_bb_bank_da(&a), "bb_form: Bank nicht erkannt");
        }
    }
    free(emd); free(tim);
    if (fails) { fprintf(stderr, "test_r41_form_emd: %d Fehler\n", fails); return 1; }
    printf("test_r41_form_emd: OK\n");
    return 0;
}
