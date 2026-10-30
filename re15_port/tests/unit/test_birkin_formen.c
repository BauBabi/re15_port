/* test_birkin_formen.c — Formpruefung der beiden Beta-Birkin-Nachbildungen.
 *
 * EM30_HANGAR.EMD / EM30_BAHNSTEIG.EMD (re15_port/tools/birkin_formen.py) sind KEINE
 * Original-Assets, sondern aus EM30 (CDEMD0.EMS, Typ 0x30) abgeleitete Gestaltung. Der
 * Generator verspricht, dass nur MD1-Vertices/-Normalen, EMR-Bindoffsets und TIM-CLUTs
 * anders sind. Dieser Test haelt das fest, damit die Dateien im Port (RE15_BIRKIN_FORM,
 * platform/pc/main.c) mit den unveraenderten EM30-Clips und der EM30-KI laufen:
 *   - re15_emd_parse_container liest beide Dateien,
 *   - Mesh-/Knochen-/Clip-/Keyframe-Zahlen und Hierarchie sind gleich wie bei EM30,
 *   - EDD ist byte-gleich; Keyframes byte-gleich (Hangar) bzw. nur an den umgehaltenen
 *     Knochen + Wurzelhoehe geaendert (Bahnsteig, Vierbein-Haltung),
 *   - die EM30-Topologie (Dreiecke/Quads, UVs) ist ein byte-gleiches Praefix; die Hangar-
 *     Form haengt ihren Schwanz nur an (Mesh 8) und jede Zusatz-Face zeigt in die Arrays,
 *   - die Geometrie ist tatsaechlich anders (sonst waere die Datei nutzlos).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_md1.h"
#include "re15_tim.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

typedef struct {
    re15_md1_t           md1;
    re15_emd_skeleton_t  skel;
    re15_emd_animation_t anim;
    re15_tim_t           tim;
} modell_t;

static modell_t s_basis, s_form;

static void vergleiche(const char *name, const uint8_t *em30, size_t em30_len, int umgehalten)
{
    char pfad[512];
    snprintf(pfad, sizeof pfad, RE15_ASSET_PSX_DIR "/EMD/%s", name);
    size_t len = 0;
    uint8_t *blob = slurp(pfad, &len);
    CHECK(blob != NULL, "%s fehlt (python re15_port/tools/birkin_formen.py)", pfad);
    if (!blob) return;
    CHECK(len >= em30_len, "%s: Laenge %zu < EM30 %zu", name, len, em30_len);

    memset(&s_form, 0, sizeof s_form);
    int rc = re15_emd_parse_container(blob, len, &s_form.md1, &s_form.skel, &s_form.anim, &s_form.tim);
    CHECK(rc == 0, "%s: re15_emd_parse_container rc=%d", name, rc);
    if (rc != 0) { free(blob); return; }

    CHECK(s_form.md1.mesh_count == s_basis.md1.mesh_count, "%s: Meshes %d != %d",
          name, s_form.md1.mesh_count, s_basis.md1.mesh_count);
    CHECK(s_form.skel.bone_count == s_basis.skel.bone_count, "%s: Knochen %d != %d",
          name, s_form.skel.bone_count, s_basis.skel.bone_count);
    CHECK(s_form.anim.clip_count == s_basis.anim.clip_count, "%s: Clips %d != %d",
          name, s_form.anim.clip_count, s_basis.anim.clip_count);
    CHECK(s_form.tim.width == s_basis.tim.width && s_form.tim.height == s_basis.tim.height,
          "%s: TIM %dx%d != %dx%d", name, s_form.tim.width, s_form.tim.height,
          s_basis.tim.width, s_basis.tim.height);
    for (int b = 0; b < s_basis.skel.bone_count; b++)
        CHECK(s_form.skel.bone_parent[b] == s_basis.skel.bone_parent[b],
              "%s: Parent von Knochen %d %d != %d", name, b,
              s_form.skel.bone_parent[b], s_basis.skel.bone_parent[b]);
    CHECK(memcmp(s_form.anim.frames, s_basis.anim.frames,
                 sizeof(uint32_t) * (size_t)s_basis.anim.frame_count) == 0,
          "%s: EDD-Frames weichen von EM30 ab", name);
    /* nur die Keyframes, die das EDD referenziert: keyframe_count/-data_size reichen beim
     * Container-Parse bis zum Blobende und umfassen damit auch die (geaenderte) MD1/TIM.
     * Hangar: byte-gleich. Bahnsteig: umgehalten (birkin_formen.py umhalten, Vierbein-
     * Haltung) -> Wurzel-X/Z und die NICHT umgehaltenen Knochen 0/3/6/8/11/14 byte-gleich. */
    {
        int kn = s_basis.anim.last_keyframe_index + 1, ks = s_basis.skel.keyframe_size_bytes;
        if (!umgehalten) {
            CHECK(memcmp(s_form.skel.keyframe_data, s_basis.skel.keyframe_data,
                         (size_t)kn * (size_t)ks) == 0, "%s: Keyframes weichen von EM30 ab", name);
        } else {
            static const int fest[] = {0, 3, 6, 8, 11, 14};
            int abw = 0, y_neu = 0;
            for (int k = 0; k < kn; k++) {
                const uint8_t *a = s_basis.skel.keyframe_data + (size_t)k * ks;
                const uint8_t *b = s_form.skel.keyframe_data + (size_t)k * ks;
                abw += memcmp(a, b, 2) != 0 || memcmp(a + 4, b + 4, 2) != 0;
                y_neu += memcmp(a + 2, b + 2, 2) != 0;
                for (int i = 0; i < (int)(sizeof fest / sizeof fest[0]); i++)
                    for (int bit = fest[i] * 36; bit < fest[i] * 36 + 36; bit++)
                        abw += ((a[12 + bit / 8] ^ b[12 + bit / 8]) >> (bit & 7)) & 1;
            }
            CHECK(abw == 0, "%s: %d Abweichungen in Wurzel-X/Z bzw. festen Knochen", name, abw);
            CHECK(y_neu > 0, "%s: Haltung ohne Wirkung (keine Wurzelhoehe geaendert)", name);
        }
    }

    long geaendert = 0, zusatz = 0;
    for (int m = 0; m < s_basis.md1.mesh_count; m++) {
        const re15_md1_mesh_t *a = &s_basis.md1.meshes[m], *b = &s_form.md1.meshes[m];
        /* Zusatzgeometrie (Hangar-Schwanz an Mesh 8) wird nur ANGEHAENGT: die EM30-Faces,
         * -UVs und Vertex-Indizes bleiben ein unveraendertes Praefix. */
        CHECK(b->triangle_count >= a->triangle_count && b->quad_count >= a->quad_count &&
              b->tri_vertex_count >= a->tri_vertex_count &&
              b->quad_vertex_count >= a->quad_vertex_count,
              "%s: Mesh %d hat weniger Elemente als EM30", name, m);
        if (b->triangle_count >= a->triangle_count && a->triangle_count > 0) {
            CHECK(!memcmp(a->triangles, b->triangles, sizeof *a->triangles * (size_t)a->triangle_count) &&
                  !memcmp(a->triangle_uvs, b->triangle_uvs, sizeof *a->triangle_uvs * (size_t)a->triangle_count),
                  "%s: Mesh %d Dreieckstopologie/UVs weichen ab", name, m);
        }
        if (b->quad_count >= a->quad_count && a->quad_count > 0) {
            CHECK(!memcmp(a->quads, b->quads, sizeof *a->quads * (size_t)a->quad_count) &&
                  !memcmp(a->quad_uvs, b->quad_uvs, sizeof *a->quad_uvs * (size_t)a->quad_count),
                  "%s: Mesh %d Quadtopologie/UVs weichen ab", name, m);
        }
        zusatz += (b->quad_count - a->quad_count) + (b->triangle_count - a->triangle_count);
        /* jede neue Face muss auf existierende Vertices/Normalen des Meshes zeigen */
        for (int q = a->quad_count; q < b->quad_count; q++) {
            const re15_md1_quad_t *f = &b->quads[q];
            CHECK(f->v0 < b->quad_vertex_count && f->v1 < b->quad_vertex_count &&
                  f->v2 < b->quad_vertex_count && f->v3 < b->quad_vertex_count &&
                  f->n0 < b->quad_normal_count && f->n1 < b->quad_normal_count &&
                  f->n2 < b->quad_normal_count && f->n3 < b->quad_normal_count,
                  "%s: Mesh %d Zusatz-Quad %d zeigt ausserhalb", name, m, q);
        }
        for (int t = a->triangle_count; t < b->triangle_count; t++) {
            const re15_md1_triangle_t *f = &b->triangles[t];
            CHECK(f->v0 < b->tri_vertex_count && f->v1 < b->tri_vertex_count &&
                  f->v2 < b->tri_vertex_count && f->n0 < b->tri_normal_count &&
                  f->n1 < b->tri_normal_count && f->n2 < b->tri_normal_count,
                  "%s: Mesh %d Zusatz-Dreieck %d zeigt ausserhalb", name, m, t);
        }
        for (int v = 0; v < a->tri_vertex_count && v < b->tri_vertex_count; v++)
            geaendert += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
    }
    CHECK(geaendert > 0, "%s: keine einzige Vertex-Aenderung gegenueber EM30", name);
    if (!strcmp(name, "EM30_HANGAR.EMD"))
        CHECK(zusatz > 0, "%s: Schwanz-Geometrie fehlt", name);
    fprintf(stderr, "%s: %d Meshes, %d Knochen, %d Clips, %ld Tri-Vertices umgeformt, %ld Zusatz-Faces\n",
            name, s_form.md1.mesh_count, s_form.skel.bone_count, s_form.anim.clip_count, geaendert, zusatz);
    free(blob);
}

int main(void)
{
    size_t esz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &esz);
    if (!ems) { fprintf(stderr, "SKIP: CDEMD0.EMS fehlt\n"); return 0; }
    size_t off = 0, len = 0;
    int idx = re15_ems_index_for_type(0x30);
    if (idx < 0 || re15_ems_get_entry(ems, esz, idx, &off, &len) != 0) {
        fprintf(stderr, "FAIL: EM30 nicht in CDEMD0.EMS\n");
        return 1;
    }
    int rc = re15_emd_parse_container(ems + off, len, &s_basis.md1, &s_basis.skel,
                                      &s_basis.anim, &s_basis.tim);
    CHECK(rc == 0, "EM30: re15_emd_parse_container rc=%d", rc);
    if (rc == 0) {
        vergleiche("EM30_HANGAR.EMD", ems + off, len, 0);
        vergleiche("EM30_BAHNSTEIG.EMD", ems + off, len, 1);
    }
    free(ems);
    if (fails) { fprintf(stderr, "test_birkin_formen: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_formen: OK\n");
    return 0;
}
