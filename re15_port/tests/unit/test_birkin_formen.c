/* test_birkin_formen.c — Formpruefung der Hangar-/Labor-Nachbildung (Bahnsteig-Form: test_birkin_bahnsteig.c).
 *
 * EM30_HANGAR.EMD (re15_port/tools/birkin_formen.py) ist KEIN Original-Asset, sondern aus
 * EM30 (CDEMD0.EMS, Typ 0x30) abgeleitete Gestaltung. Seit Runde 41 Spur SF (Nutzer: "Der Schwanz ist viel zu
 * starr") haengt der Schwanz als eigene Knochenkette 16-22 an Knochen 8; Mesh i == Knochen i, das knochenlose
 * Rohr-Mesh 16 des Capcom-Modells entfaellt. Der Test haelt fest, damit die Datei im Port mit den EM30-Clips,
 * der EM30-KI und der Hangar-Szene (Knochen 15) laeuft:
 *   - re15_emd_parse_container liest die Datei,
 *   - Knochen 0-15 haben dieselben Eltern wie EM30, 16-22 bilden eine Kette ab Knochen 8,
 *   - Clip-Zahl und EDD sind byte-gleich; jeder Keyframe beginnt mit dem EM30-Keyframe (Wurzel, Spur,
 *     16 x 36 Bit), die Schwanzwinkel haengen dahinter und bewegen sich,
 *   - Meshes 0-15: Topologie gleich, UVs gleich ausser den umtexturierten Teilen (Beine 1-6, Kopf 9,
 *     Sichel 15 nur CLUT); Meshes 16-22 zeigen nur in ihre eigenen Arrays,
 *   - Runde 41 SF Nachbesserung 1: Oberschenkel 1/4 an y 360 geteilt (kurze Hose; jedes alte Dreieck bleibt, jedes
 *     geteilte Viereck wird zu zwei, neue Ecken liegen hinter den alten), Haut-CLUTs 480-482 umgefaerbt (Jeans-Eintraege
 *     mit b > r unveraendert), Zusatzzeilen 483 (Schwanz, Meshes 16-22) und 484 (Sichel, Mesh 15),
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

#define KETTE 7                  /* Schwanzglieder, birkin_formen.py FORMEN['hangar'] schwanz_kette glieder */
static int umtexturiert(int m)   /* birkin_formen.py FORMEN['hangar'] umtexturieren */
{
    return (m >= 1 && m <= 6) || m == 9 || m == 15;
}
static int geteilt(int m)        /* birkin_formen.py FORMEN['hangar'] teilen (kurze Hose, N1.6) */
{
    return m == 1 || m == 4;
}
#define CLUT_SCHWANZ (483u << 6)  /* birkin_formen.py schwanz_kette uv clut */
#define CLUT_SICHEL  (484u << 6)  /* birkin_formen.py umtexturieren Mesh 15 */

static void vergleiche(const char *name, const uint8_t *em30, size_t em30_len)
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

    const int nb0 = s_basis.skel.bone_count;
    CHECK(nb0 == 16, "EM30: %d Knochen statt 16", nb0);
    CHECK(s_form.skel.bone_count == nb0 + KETTE, "%s: Knochen %d != %d", name, s_form.skel.bone_count, nb0 + KETTE);
    CHECK(s_form.md1.mesh_count == nb0 + KETTE, "%s: Meshes %d != %d (Mesh i == Knochen i)",
          name, s_form.md1.mesh_count, nb0 + KETTE);
    CHECK(s_form.anim.clip_count == s_basis.anim.clip_count, "%s: Clips %d != %d",
          name, s_form.anim.clip_count, s_basis.anim.clip_count);
    CHECK(s_form.tim.width == s_basis.tim.width && s_form.tim.height == s_basis.tim.height,
          "%s: TIM %dx%d != %dx%d", name, s_form.tim.width, s_form.tim.height,
          s_basis.tim.width, s_basis.tim.height);
    for (int b = 0; b < nb0; b++)
        CHECK(s_form.skel.bone_parent[b] == s_basis.skel.bone_parent[b],
              "%s: Parent von Knochen %d %d != %d", name, b,
              s_form.skel.bone_parent[b], s_basis.skel.bone_parent[b]);
    for (int b = nb0; b < s_form.skel.bone_count; b++)
        CHECK(s_form.skel.bone_parent[b] == (b == nb0 ? 8 : b - 1),
              "%s: Schwanzknochen %d haengt an %d", name, b, s_form.skel.bone_parent[b]);
    CHECK(s_form.anim.frame_count == s_basis.anim.frame_count &&
          memcmp(s_form.anim.frames, s_basis.anim.frames,
                 sizeof(uint32_t) * (size_t)s_basis.anim.frame_count) == 0,
          "%s: EDD-Frames weichen von EM30 ab", name);
    /* Keyframes: die ersten 84 Byte (Wurzel + Spur + 16 x 36 Bit) byte-gleich mit EM30, dahinter die
     * Schwanzwinkel. Nur die vom EDD referenzierten Keyframes (keyframe_count reicht bis zum Blobende). */
    {
        int kn = s_basis.anim.last_keyframe_index + 1;
        int ka = s_basis.skel.keyframe_size_bytes, kb = s_form.skel.keyframe_size_bytes;
        CHECK(kb >= 12 + (s_form.skel.bone_count * 36 + 7) / 8 && kb % 4 == 0,
              "%s: Keyframe-Groesse %d zu klein fuer %d Knochen", name, kb, s_form.skel.bone_count);
        int abw = 0, bewegt = 0;
        const uint8_t *k0 = s_form.skel.keyframe_data;
        for (int k = 0; k < kn; k++) {
            const uint8_t *a = s_basis.skel.keyframe_data + (size_t)k * ka;
            const uint8_t *b = s_form.skel.keyframe_data + (size_t)k * kb;
            abw += memcmp(a, b, (size_t)ka) != 0;
            bewegt += memcmp(b + ka, k0 + ka, (size_t)(kb - ka)) != 0;
        }
        CHECK(abw == 0, "%s: %d Keyframes weichen in Knochen 0-15 von EM30 ab", name, abw);
        CHECK(bewegt > kn / 2, "%s: Schwanz starr (%d von %d Keyframes anders als Keyframe 0)", name, bewegt, kn);
        fprintf(stderr, "%s: %d von %d Keyframes mit bewegtem Schwanz\n", name, bewegt, kn);
    }

    long geaendert = 0, uv_neu = 0;
    for (int m = 0; m < s_form.md1.mesh_count; m++) {
        const re15_md1_mesh_t *b = &s_form.md1.meshes[m];
        if (m < nb0) {
            const re15_md1_mesh_t *a = &s_basis.md1.meshes[m];
            if (geteilt(m)) {
                /* Dreiecke unveraendert (Topologie), Vierecke: mehr als vorher, alle Indizes im Block, alte Ecken
                 * an ihrem Platz (neue Ecken haengen hinten an). */
                CHECK(b->triangle_count == a->triangle_count && b->quad_count > a->quad_count &&
                      b->quad_vertex_count > a->quad_vertex_count, "%s: Mesh %d nicht geteilt (Quads %d/%d)", name, m,
                      b->quad_count, a->quad_count);
                CHECK(b->quad_count - a->quad_count == (b->quad_vertex_count - a->quad_vertex_count) / 2,
                      "%s: Mesh %d: %d neue Quads zu %d neuen Ecken", name, m, b->quad_count - a->quad_count,
                      b->quad_vertex_count - a->quad_vertex_count);
                if (a->triangle_count > 0)
                    CHECK(!memcmp(a->triangles, b->triangles, sizeof *a->triangles * (size_t)a->triangle_count),
                          "%s: Mesh %d Dreieckstopologie weicht ab", name, m);
                for (int q = 0; q < b->quad_count; q++) {
                    const re15_md1_quad_t *f = &b->quads[q];
                    CHECK(f->v0 < b->quad_vertex_count && f->v1 < b->quad_vertex_count && f->v2 < b->quad_vertex_count &&
                          f->v3 < b->quad_vertex_count && f->n0 < b->quad_normal_count && f->n1 < b->quad_normal_count &&
                          f->n2 < b->quad_normal_count && f->n3 < b->quad_normal_count,
                          "%s: Mesh %d Quad %d zeigt ausserhalb", name, m, q);
                }
                uv_neu++;
                for (int v = 0; v < a->tri_vertex_count; v++)
                    geaendert += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
                continue;
            }
            CHECK(b->triangle_count == a->triangle_count && b->quad_count == a->quad_count &&
                  b->tri_vertex_count == a->tri_vertex_count && b->quad_vertex_count == a->quad_vertex_count,
                  "%s: Mesh %d Elementzahlen weichen von EM30 ab", name, m);
            if (b->triangle_count != a->triangle_count || b->quad_count != a->quad_count) continue;
            if (a->triangle_count > 0)
                CHECK(!memcmp(a->triangles, b->triangles, sizeof *a->triangles * (size_t)a->triangle_count),
                      "%s: Mesh %d Dreieckstopologie weicht ab", name, m);
            if (a->quad_count > 0)
                CHECK(!memcmp(a->quads, b->quads, sizeof *a->quads * (size_t)a->quad_count),
                      "%s: Mesh %d Quadtopologie weicht ab", name, m);
            int uv_gleich =
                (a->triangle_count == 0 ||
                 !memcmp(a->triangle_uvs, b->triangle_uvs, sizeof *a->triangle_uvs * (size_t)a->triangle_count)) &&
                (a->quad_count == 0 ||
                 !memcmp(a->quad_uvs, b->quad_uvs, sizeof *a->quad_uvs * (size_t)a->quad_count));
            if (umtexturiert(m)) { CHECK(!uv_gleich, "%s: Mesh %d nicht umtexturiert", name, m); uv_neu++; }
            else CHECK(uv_gleich, "%s: Mesh %d UVs weichen ab", name, m);
            if (m == 15) {
                int sichel = 0;
                for (int q = 0; q < b->quad_count; q++) sichel += b->quad_uvs[q].clut == CLUT_SICHEL;
                for (int t = 0; t < b->triangle_count; t++) sichel += b->triangle_uvs[t].clut == CLUT_SICHEL;
                CHECK(sichel == b->quad_count + b->triangle_count, "%s: Sichel (Mesh 15) nur %d/%d Teile auf CLUT 484",
                      name, sichel, b->quad_count + b->triangle_count);
            }
            for (int v = 0; v < a->tri_vertex_count; v++)
                geaendert += memcmp(&a->tri_vertices[v], &b->tri_vertices[v], sizeof *a->tri_vertices) != 0;
        } else {
            CHECK(b->quad_count > 0, "%s: Schwanzglied-Mesh %d leer", name, m);
            for (int q = 0; q < b->quad_count; q++)
                CHECK(b->quad_uvs[q].clut == CLUT_SCHWANZ, "%s: Schwanzglied %d Quad %d CLUT %04x statt 483", name, m, q,
                      b->quad_uvs[q].clut);
            for (int q = 0; q < b->quad_count; q++) {
                const re15_md1_quad_t *f = &b->quads[q];
                CHECK(f->v0 < b->quad_vertex_count && f->v1 < b->quad_vertex_count &&
                      f->v2 < b->quad_vertex_count && f->v3 < b->quad_vertex_count &&
                      f->n0 < b->quad_normal_count && f->n1 < b->quad_normal_count &&
                      f->n2 < b->quad_normal_count && f->n3 < b->quad_normal_count,
                      "%s: Mesh %d Quad %d zeigt ausserhalb", name, m, q);
            }
            for (int t = 0; t < b->triangle_count; t++) {
                const re15_md1_triangle_t *f = &b->triangles[t];
                CHECK(f->v0 < b->tri_vertex_count && f->v1 < b->tri_vertex_count &&
                      f->v2 < b->tri_vertex_count && f->n0 < b->tri_normal_count &&
                      f->n1 < b->tri_normal_count && f->n2 < b->tri_normal_count,
                      "%s: Mesh %d Dreieck %d zeigt ausserhalb", name, m, t);
            }
        }
    }
    CHECK(geaendert > 0, "%s: keine einzige Vertex-Aenderung gegenueber EM30", name);
    /* CLUT-Block (N1.4/N1.5/N1.7): 3 EM30-Zeilen + 2 Zusatzzeilen; Haut umgefaerbt, blaue Eintraege (Jeans) gleich. */
    CHECK(s_basis.tim.clut_entries == 3 * 256 && s_form.tim.clut_entries == 5 * 256 &&
          s_form.tim.clut_y == s_basis.tim.clut_y, "%s: CLUT %d Eintraege (Soll 5 x 256) ab y %d", name,
          s_form.tim.clut_entries, s_form.tim.clut_y);
    if (s_form.tim.clut_entries == 5 * 256) {
        int blau_anders = 0, haut_anders = 0;
        for (int i = 0; i < 3 * 256; i++) {
            const uint16_t c = s_basis.tim.clut[i], d = s_form.tim.clut[i];
            const int r = c & 31, bl = (c >> 10) & 31;
            if (bl > r) blau_anders += c != d; else haut_anders += c != d;
            CHECK((c == 0) == (d == 0), "%s: CLUT %d Transparenz geaendert (%04x -> %04x)", name, i, c, d);
        }
        CHECK(blau_anders == 0, "%s: %d blaue CLUT-Eintraege (Jeans) umgefaerbt", name, blau_anders);
        CHECK(haut_anders > 200, "%s: nur %d Haut-Eintraege umgefaerbt", name, haut_anders);
        fprintf(stderr, "%s: %d Haut-CLUT-Eintraege umgefaerbt, 2 Zusatzzeilen\n", name, haut_anders);
    }
    fprintf(stderr, "%s: %d Meshes, %d Knochen, %d Clips, %ld Tri-Vertices umgeformt, %ld Teile umtexturiert\n",
            name, s_form.md1.mesh_count, s_form.skel.bone_count, s_form.anim.clip_count, geaendert, uv_neu);
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
        vergleiche("EM30_HANGAR.EMD", ems + off, len);
    }
    free(ems);
    if (fails) { fprintf(stderr, "test_birkin_formen: %d Fehler\n", fails); return 1; }
    fprintf(stderr, "test_birkin_formen: OK\n");
    return 0;
}
