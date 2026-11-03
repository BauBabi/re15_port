/* test_birkin_formen.c — Formpruefung der Hangar-/Labor-Nachbildung (Bahnsteig-Form: test_birkin_bahnsteig.c).
 *
 * EM30_HANGAR.EMD (re15_port/tools/birkin_formen.py) ist KEIN Original-Asset, sondern aus
 * EM30 (CDEMD0.EMS, Typ 0x30) abgeleitete Gestaltung. Seit Runde 41 Spur SF (Nutzer: "Der Schwanz ist viel zu
 * starr") haengt der Schwanz als eigene Knochenkette 16-22 an Knochen 8; Mesh i == Knochen i, das knochenlose
 * Rohr-Mesh 16 des Capcom-Modells entfaellt. Der Test haelt fest, damit die Datei im Port mit den EM30-Clips,
 * der EM30-KI und der Hangar-Szene (Knochen 15) laeuft:
 *   - re15_emd_parse_container liest die Datei,
 *   - Knochen 0-15 haben dieselben Eltern wie EM30, 16-22 bilden eine Kette ab Knochen 8,
 *   - Clip-Zahl und EDD sind byte-gleich; Keyframes: Nachbesserung 2 duckt Knochen 1-10/13 und senkt die Wurzel
 *     (Becken 0, Unterarme/Haende 11/12/14/15, Wurzel x/z, Spur wie EM30, Fussgelenke bleiben bis auf die Spreizung),
 *     die Schwanzwinkel haengen dahinter und bewegen sich,
 *   - Meshes 0-15: Topologie gleich, UVs gleich ausser den umtexturierten Teilen (Beine 1-6, Kopf 9,
 *     Sichel 15 nur CLUT); Meshes 16-22 zeigen nur in ihre eigenen Arrays,
 *   - Runde 41 SF Nachbesserung 1: Oberschenkel 1/4 an y 360 geteilt (kurze Hose; jedes alte Dreieck bleibt, jedes
 *     geteilte Viereck wird zu zwei, neue Ecken liegen hinter den alten), Haut-CLUTs 480-482 umgefaerbt (Jeans-Eintraege
 *     mit b > r unveraendert), Zusatzzeilen 483 (Schwanz, Meshes 16-22), 484 (Sichel, Mesh 15), 485 (Kopf, Mesh 9),
 *   - die Geometrie ist tatsaechlich anders (sonst waere die Datei nutzlos).
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
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
#define CLUT_KOPF    (485u << 6)  /* birkin_formen.py umtexturieren Mesh 9 (N1.10) */

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
    /* Keyframes (Runde 41 SF Nachbesserung 2, birkin_formen.py ducken(): geduckte Haltung nach Video 4):
     *   - Wurzel x/z und Spur (Byte 6-11) byte-gleich mit EM30; Wurzel-Y nur tiefer (0..BECKEN, Y nach unten),
     *   - Becken (0), Unterarme/Haende (11, 12, 14, 15) mit EM30-Winkeln (Klaue, Sichel, Griffpunkt Knochen 15),
     *   - Fussgelenke (Ursprung Knochen 3/6, Modellraum) bleiben bis auf die Spreizung (<= SPREIZ + 40) liegen
     *     -> Bodenkontakt und Gangbild der EM30-Clips bleiben,
     *   - mehr als die Haelfte der Keyframes ist tatsaechlich geduckt (Wurzel >= 50 tiefer),
     *   - dahinter die Schwanzwinkel, die sich bewegen.
     * Nur die vom EDD referenzierten Keyframes (keyframe_count reicht bis zum Blobende). */
    {
        enum { BECKEN = 110, SPREIZ = 340 };   /* birkin_formen.py FORMEN['hangar'] ducken becken/spreiz */
        int kn = s_basis.anim.last_keyframe_index + 1;
        int ka = s_basis.skel.keyframe_size_bytes, kb = s_form.skel.keyframe_size_bytes;
        CHECK(kb >= 12 + (s_form.skel.bone_count * 36 + 7) / 8 && kb % 4 == 0,
              "%s: Keyframe-Groesse %d zu klein fuer %d Knochen", name, kb, s_form.skel.bone_count);
        int bewegt = 0, geduckt = 0, wurzel_abw = 0, winkel_abw = 0, fuss_weg = 0;
        double fuss_max = 0;
        const uint8_t *k0 = s_form.skel.keyframe_data;
        static const int fest[] = { 0, 12, 14, 15 };   /* Neuaufbau ab NB2: 11 = Klauen-Ellbogen 60 Grad nach vorn (NB3, Klaue) */
        for (int k = 0; k < kn; k++) {
            const uint8_t *a = s_basis.skel.keyframe_data + (size_t)k * ka;
            const uint8_t *b = s_form.skel.keyframe_data + (size_t)k * kb;
            int16_t ax, ay, az, bx, by, bz;
            re15_emd_get_keyframe_position(&s_basis.skel, k, &ax, &ay, &az);
            re15_emd_get_keyframe_position(&s_form.skel, k, &bx, &by, &bz);
            wurzel_abw += (ax != bx || az != bz || by < ay || by > ay + BECKEN || memcmp(a + 6, b + 6, 6) != 0);
            geduckt += by >= ay + 50;
            for (size_t i = 0; i < sizeof fest / sizeof fest[0]; i++) {
                int16_t w0[3], w1[3];
                re15_emd_get_keyframe_angles(&s_basis.skel, k, fest[i], &w0[0], &w0[1], &w0[2]);
                re15_emd_get_keyframe_angles(&s_form.skel, k, fest[i], &w1[0], &w1[1], &w1[2]);
                winkel_abw += memcmp(w0, w1, sizeof w0) != 0;
            }
            /* Fussgelenke im Modellraum: Wurzel + R0 (rel1 + R1 (rel2 + R2 rel3)) mit der Port-RotMatrix */
            for (int bein = 0; bein < 2; bein++) {
                double p[2][3];
                for (int s = 0; s < 2; s++) {
                    const re15_emd_skeleton_t *sk = s ? &s_form.skel : &s_basis.skel;
                    int16_t px, py, pz; re15_emd_get_keyframe_position(sk, k, &px, &py, &pz);
                    double R[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 }, t[3] = { px, py, pz };
                    const int kette[4] = { 0, 1 + 3 * bein, 2 + 3 * bein, 3 + 3 * bein };
                    for (int j = 0; j < 4; j++) {
                        int bb = kette[j];
                        if (j > 0) for (int r = 0; r < 3; r++)
                            t[r] += R[r * 3] * sk->bone_relative_pos[bb][0] + R[r * 3 + 1] * sk->bone_relative_pos[bb][1] +
                                    R[r * 3 + 2] * sk->bone_relative_pos[bb][2];
                        if (j == 3) break;
                        int16_t ex, ey, ez; int32_t m[9]; double M[9];
                        re15_emd_get_keyframe_angles(sk, k, bb, &ex, &ey, &ez);
                        re15_skel_euler_matrix(ex, ey, ez, m);
                        double Rn[9];
                        for (int q = 0; q < 9; q++) M[q] = m[q] / 4096.0;
                        for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++)
                            Rn[r * 3 + c] = R[r * 3] * M[c] + R[r * 3 + 1] * M[3 + c] + R[r * 3 + 2] * M[6 + c];
                        memcpy(R, Rn, sizeof R);
                    }
                    memcpy(p[s], t, sizeof t);
                }
                double dx = p[1][0] - p[0][0], dy = p[1][1] - p[0][1], dz = p[1][2] - p[0][2];
                double d = sqrt(dx * dx + dy * dy + dz * dz);
                if (d > fuss_max) fuss_max = d;
                fuss_weg += d > SPREIZ + 40;
            }
            bewegt += memcmp(b + ka, k0 + ka, (size_t)(kb - ka)) != 0;
        }
        CHECK(wurzel_abw == 0, "%s: %d Keyframes mit Wurzel x/z/Spur anders oder Becken ausserhalb 0..%d", name, wurzel_abw,
              BECKEN);
        CHECK(winkel_abw == 0, "%s: %d Winkel der Knochen 0/12/14/15 weichen von EM30 ab", name, winkel_abw);
        CHECK(fuss_weg == 0, "%s: %d Fussgelenke mehr als %d vom EM30-Ort (max %.0f)", name, fuss_weg, SPREIZ + 40, fuss_max);
        CHECK(geduckt > kn / 2, "%s: nur %d von %d Keyframes geduckt", name, geduckt, kn);
        CHECK(bewegt > kn / 2, "%s: Schwanz starr (%d von %d Keyframes anders als Keyframe 0)", name, bewegt, kn);
        fprintf(stderr, "%s: %d von %d Keyframes geduckt, Fussgelenk max %.0f vom EM30-Ort, %d mit bewegtem Schwanz\n",
                name, geduckt, kn, fuss_max, bewegt);
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
            if (m == 9) {                /* N1.10: Kopf auf eigener CLUT-Zeile */
                int kopf = 0;
                for (int q = 0; q < b->quad_count; q++) kopf += b->quad_uvs[q].clut == CLUT_KOPF;
                for (int t = 0; t < b->triangle_count; t++) kopf += b->triangle_uvs[t].clut == CLUT_KOPF;
                CHECK(kopf == b->quad_count + b->triangle_count, "%s: Kopf (Mesh 9) nur %d/%d Teile auf CLUT 485",
                      name, kopf, b->quad_count + b->triangle_count);
            }
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
    /* CLUT-Block (N1.4/N1.5/N1.7): 3 EM30-Zeilen + 3 Zusatzzeilen; Haut umgefaerbt, blaue Eintraege (Jeans) gleich. */
    CHECK(s_basis.tim.clut_entries == 3 * 256 && s_form.tim.clut_entries == 6 * 256 &&
          s_form.tim.clut_y == s_basis.tim.clut_y, "%s: CLUT %d Eintraege (Soll 6 x 256) ab y %d", name,
          s_form.tim.clut_entries, s_form.tim.clut_y);
    if (s_form.tim.clut_entries == 6 * 256) {
        int blau_anders = 0, haut_anders = 0;
        for (int i = 0; i < 3 * 256; i++) {
            const uint16_t c = s_basis.tim.clut[i], d = s_form.tim.clut[i];
            const int r = c & 31, bl = (c >> 10) & 31;
            if (bl > r) blau_anders += c != d; else haut_anders += c != d;
            CHECK((c == 0) == (d == 0), "%s: CLUT %d Transparenz geaendert (%04x -> %04x)", name, i, c, d);
        }
        CHECK(blau_anders == 0, "%s: %d blaue CLUT-Eintraege (Jeans) umgefaerbt", name, blau_anders);
        CHECK(haut_anders > 200, "%s: nur %d Haut-Eintraege umgefaerbt", name, haut_anders);
        fprintf(stderr, "%s: %d Haut-CLUT-Eintraege umgefaerbt, 3 Zusatzzeilen\n", name, haut_anders);
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
