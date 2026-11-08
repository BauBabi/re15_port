/*
 * Runde 41 Integration, Absturz Software-Renderer K5c (analysis/befunde_runde41/form/STUFE3.md Abschnitt
 * "Integration: Absturz Software-Renderer K5c"): Riegel fuer die ausgelieferten Modell-/Textur-Paare.
 *
 * Fuer jedes Paar prueft er, was der PC-Zeichner aus den MD1-UVs macht, mit genau dessen Formel:
 *   Texel-x = u + (page & 0xF) * 128            platform/pc/main.c page_off (NPC-Zeichner, Dreieck und Viereck)
 *   Texel-y = v + CLUT-Index * TIM-Hoehe        platform/pc/src/render_pc.c re15_render_textured_tri*: v_offset
 *   Port-Textur = TIM-Breite x (TIM-Hoehe * CLUT-Zeilen), CLUT-Zeilen = clut_entries / (4 bpp ? 16 : 256)
 *                                               render_pc.c re15_render_pc_upload_tim_slot (row_width, n_cluts)
 * und verlangt
 *   (1) jede UV-Ecke liegt in der Textur: Texel-x in 0..Breite-1, v in 0..TIM-Hoehe-1 (vor dem Zeilenversatz);
 *   (2) die Port-Texturhoehe ist <= 4096 (GLES2-Geraete melden max_texture_height 4096; SDL lehnt groessere Texturen ab,
 *       SDL 2.28.5 src/render/SDL_render.c:1266-1268).
 * CLUT-Indizes ausserhalb der TIM nimmt der Zeichner als Zeile 0 (render_pc.c `clut_idx = 0`); sie stehen nur im Protokoll.
 *
 * Paare: RE2/BAHNSTEIG_FORM.EMD + .TIM (Riesenform K5c), PSX/EMD/EM30_HANGAR.EMD, PSX/EMD/EM29_ROT.EMD, RE2/ANNETTE_4D.EMD,
 * RE2/ANNETTE_4D_BLUT.EMD (RE1.5-Container mit TIM), jede RE2-Bank aus RE2/CDEMD0.EMS mit EMD- und TIM-Record (re2_ems_load_bank).
 *
 * Gegenprobe (eingebaut): dieselbe Pruefung auf einer Kopie der BAHNSTEIG_FORM mit einer verschobenen UV (Dreieck 0 in Mesh 0,
 * u0 auf 255 und Seite 0xF -> Texel-x 2175 bei Breite 512) muss anschlagen. "test_r41_uv_textur gegenprobe" laesst dieselbe
 * Verschiebung durch die echte Pruefung laufen und endet rot (Beleg, dass der Riegel greift).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_enemy.h"
#include "re2_ems.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

#define RE2_DIR RE15_ASSET_PSX_DIR "/../RE2"
#define PORT_TEX_MAX_H 4096   /* SDL_RendererInfo.max_texture_height der GLES2-Geraete (Befund Integration Runde 41) */

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

typedef struct {
    int flaechen, ecken, aus_u, aus_v, clut_aus;
    int umin, umax, vmin, vmax;
    int tex_w, tex_h, tim_h, n_cluts;
    int erst_mesh, erst_flaeche, erst_u, erst_v, erst_page;   /* erste Ecke ausserhalb, -1 = keine */
} pruef_t;

/* wie render_pc.c re15_render_pc_upload_tim_slot */
static int n_cluts_wie_upload(const re15_tim_t *t)
{
    int row_width = (t->bpp == 4) ? 16 : 256;
    int n = (t->has_clut && row_width > 0) ? (t->clut_entries / row_width) : 1;
    return n < 1 ? 1 : n;
}

static void ecke(pruef_t *p, const re15_tim_t *t, int mesh, int flaeche, int u, int v, int page, int clut)
{
    int x = u + (page & 0x000F) * 128;
    int ci = ((clut >> 6) & 0x1FF) - t->clut_y;
    p->ecken++;
    if (x < p->umin) p->umin = x;
    if (x > p->umax) p->umax = x;
    if (v < p->vmin) p->vmin = v;
    if (v > p->vmax) p->vmax = v;
    int aus = 0;
    if (x < 0 || x > t->width - 1) { p->aus_u++; aus = 1; }
    if (v < 0 || v > t->height - 1) { p->aus_v++; aus = 1; }
    if (ci < 0 || ci >= p->n_cluts) p->clut_aus++;
    if (aus && p->erst_mesh < 0) {
        p->erst_mesh = mesh; p->erst_flaeche = flaeche; p->erst_u = u; p->erst_v = v; p->erst_page = page;
    }
}

static pruef_t pruefe(const re15_md1_t *md, const re15_tim_t *t)
{
    pruef_t p;
    memset(&p, 0, sizeof p);
    p.umin = p.vmin = 1 << 30; p.umax = p.vmax = -1;
    p.erst_mesh = -1;
    p.n_cluts = n_cluts_wie_upload(t);
    p.tex_w = t->width; p.tim_h = t->height; p.tex_h = t->height * p.n_cluts;
    for (int m = 0; m < md->mesh_count; m++) {
        const re15_md1_mesh_t *ms = &md->meshes[m];
        for (int i = 0; i < ms->triangle_count; i++) {
            const re15_md1_tri_uv_t *uv = &ms->triangle_uvs[i];
            p.flaechen++;
            ecke(&p, t, m, i, uv->u0, uv->v0, uv->page, uv->clut);
            ecke(&p, t, m, i, uv->u1, uv->v1, uv->page, uv->clut);
            ecke(&p, t, m, i, uv->u2, uv->v2, uv->page, uv->clut);
        }
        for (int i = 0; i < ms->quad_count; i++) {
            const re15_md1_quad_uv_t *uv = &ms->quad_uvs[i];
            p.flaechen++;
            ecke(&p, t, m, 10000 + i, uv->u0, uv->v0, uv->page, uv->clut);
            ecke(&p, t, m, 10000 + i, uv->u1, uv->v1, uv->page, uv->clut);
            ecke(&p, t, m, 10000 + i, uv->u2, uv->v2, uv->page, uv->clut);
            ecke(&p, t, m, 10000 + i, uv->u3, uv->v3, uv->page, uv->clut);
        }
    }
    return p;
}

static int pruef_rot(const pruef_t *p) { return p->aus_u || p->aus_v || p->tex_h > PORT_TEX_MAX_H; }

static void melden(const char *name, const re15_md1_t *md, const pruef_t *p)
{
    printf("%-28s %2d Meshes %5d Flaechen | Textur %dx%d (TIM-Hoehe %d x %d CLUT-Zeilen) | Texel-x %d..%d v %d..%d |"
           " ausserhalb u %d v %d | CLUT ausserhalb (Zeile 0) %d\n",
           name, md->mesh_count, p->flaechen, p->tex_w, p->tex_h, p->tim_h, p->n_cluts, p->umin, p->umax, p->vmin, p->vmax,
           p->aus_u, p->aus_v, p->clut_aus);
    CHECK(p->flaechen > 0, "%s: keine Flaechen gelesen", name);
    CHECK(!p->aus_u && !p->aus_v, "%s: %d/%d UV-Ecken ausserhalb der Textur %dx%d (erste: Mesh %d Flaeche %d uv(%d,%d) page 0x%x)",
          name, p->aus_u, p->aus_v, p->tex_w, p->tim_h, p->erst_mesh, p->erst_flaeche, p->erst_u, p->erst_v, p->erst_page);
    CHECK(p->tex_h <= PORT_TEX_MAX_H, "%s: Port-Texturhoehe %d > %d", name, p->tex_h, PORT_TEX_MAX_H);
}

static re15_enemy_bank_t s_eb;

/* RE2-Format (8 Verzeichnis-Eintraege, TIM getrennt) wie platform/pc/main.c pc_enemy_load_bahnsteig */
static int bahnsteig(uint8_t *emd, size_t emd_n, const uint8_t *tb, size_t tim_n, re15_tim_t *tim)
{
    memset(&s_eb, 0, sizeof s_eb);
    memset(tim, 0, sizeof *tim);
    if (re2_emd_parse_bank(emd, emd_n, &s_eb) != 0) return -1;
    if (re15_tim_parse(tb, (int)tim_n, tim) != 0) return -2;
    return 0;
}

/* Gegenprobe-Verschiebung: Dreieck 0 in Mesh 0 -> u0 255, Seite 0xF (Texel-x 255 + 15 * 128 = 2175) */
static void verschieben(re15_md1_t *md)
{
    re15_md1_tri_uv_t *uv = (re15_md1_tri_uv_t *)md->meshes[0].triangle_uvs;
    uv->u0 = 255;
    uv->page = (uint16_t)(uv->page | 0x000F);
}

int main(int argc, char **argv)
{
    const int gegenprobe = argc > 1 && !strcmp(argv[1], "gegenprobe");

    /* ---- Riesenform K5c ------------------------------------------------------------------------------------------- */
    {
        size_t en = 0, tn = 0;
        uint8_t *emd = slurp(RE2_DIR "/BAHNSTEIG_FORM.EMD", &en);
        uint8_t *tb  = slurp(RE2_DIR "/BAHNSTEIG_FORM.TIM", &tn);
        CHECK(emd && tb, "RE2/BAHNSTEIG_FORM.EMD/.TIM fehlt");
        re15_tim_t tim;
        if (emd && tb && bahnsteig(emd, en, tb, tn, &tim) == 0) {
            if (gegenprobe) {
                printf("GEGENPROBE: Mesh 0 Dreieck 0 verschoben (u0 255, Seite 0xF)\n");
                verschieben(&s_eb.md1);
            }
            pruef_t p = pruefe(&s_eb.md1, &tim);
            melden("RE2/BAHNSTEIG_FORM", &s_eb.md1, &p);
            CHECK(s_eb.md1.mesh_count == 32, "BAHNSTEIG_FORM: %d Meshes statt 32 (K5c)", s_eb.md1.mesh_count);
            CHECK(p.tex_w == 512 && p.tex_h == 4096 && p.n_cluts == 16,
                  "BAHNSTEIG_FORM: Textur %dx%d %d Zeilen statt K5c 512x4096 16 Zeilen", p.tex_w, p.tex_h, p.n_cluts);
            /* eingebaute Gegenprobe: eine Kopie mit verschobener UV muss anschlagen (nur im Normallauf) */
            if (!gegenprobe) {
                uint8_t *kopie = (uint8_t *)malloc(en);
                re15_tim_t t2;
                if (kopie) {
                    memcpy(kopie, emd, en);
                    if (bahnsteig(kopie, en, tb, tn, &t2) == 0) {
                        verschieben(&s_eb.md1);
                        pruef_t q = pruefe(&s_eb.md1, &t2);
                        CHECK(pruef_rot(&q) && q.aus_u == 3 && q.erst_mesh == 0 && q.erst_flaeche == 0,
                              "Gegenprobe: verschobene UV nicht erkannt (aus_u %d, erste Mesh %d Flaeche %d)",
                              q.aus_u, q.erst_mesh, q.erst_flaeche);
                        printf("Gegenprobe (Kopie, Mesh 0 Dreieck 0 u0 255 Seite 0xF): ausserhalb u %d -> %s\n",
                               q.aus_u, pruef_rot(&q) ? "rot wie verlangt" : "NICHT ROT");
                    }
                    free(kopie);
                }
            }
        } else if (emd && tb) {
            CHECK(0, "BAHNSTEIG_FORM: parst nicht");
        }
        free(emd); free(tb);
    }
    if (gegenprobe) {
        printf("%s\n", fails ? "GEGENPROBE ROT (erwartet)" : "GEGENPROBE GRUEN (Riegel greift nicht!)");
        return fails ? 1 : 0;
    }

    /* ---- RE1.5-Container mit TIM (re15_emd_parse_container, platform/pc/main.c pc_enemy_load_ex) -------------------- */
    {
        static const char *const dateien[][2] = {
            { RE15_ASSET_PSX_DIR "/EMD/EM30_HANGAR.EMD", "PSX/EMD/EM30_HANGAR" },
            { RE15_ASSET_PSX_DIR "/EMD/EM29_ROT.EMD",    "PSX/EMD/EM29_ROT" },
            { RE2_DIR "/ANNETTE_4D.EMD",                 "RE2/ANNETTE_4D" },
            { RE2_DIR "/ANNETTE_4D_BLUT.EMD",            "RE2/ANNETTE_4D_BLUT" },
        };
        for (size_t i = 0; i < sizeof dateien / sizeof dateien[0]; i++) {
            size_t n = 0;
            uint8_t *b = slurp(dateien[i][0], &n);
            CHECK(b != NULL, "%s fehlt", dateien[i][1]);
            if (!b) continue;
            static re15_md1_t md; static re15_emd_skeleton_t sk; static re15_emd_animation_t an;
            re15_tim_t tim;
            memset(&md, 0, sizeof md); memset(&tim, 0, sizeof tim);
            int rc = re15_emd_parse_container(b, n, &md, &sk, &an, &tim);
            CHECK(rc == 0 && tim.width > 0, "%s: parse_container rc %d, TIM %dx%d", dateien[i][1], rc, tim.width, tim.height);
            if (rc == 0 && tim.width > 0) {
                pruef_t p = pruefe(&md, &tim);
                melden(dateien[i][1], &md, &p);
            }
            free(b);
        }
    }

    /* ---- RE2-Banken aus RE2/CDEMD0.EMS (re2_ems_load_bank, platform/pc/main.c pc_enemy_load_re2_kind) ----------------- */
    {
        size_t n = 0;
        uint8_t *ems = slurp(RE2_DIR "/CDEMD0.EMS", &n);
        CHECK(ems != NULL, "RE2/CDEMD0.EMS fehlt");
        int banken = 0;
        for (int kind = RE2_EMS_KIND_MIN; ems && kind < RE2_EMS_KIND_MIN + RE2_EMS_KIND_COUNT; kind++) {
            if (re2_ems_toc_entry(kind, RE2_EMS_REC_EMD, NULL, NULL) != 0) continue;
            if (re2_ems_toc_entry(kind, RE2_EMS_REC_TIM, NULL, NULL) != 0) continue;
            re15_tim_t tim;
            memset(&s_eb, 0, sizeof s_eb); memset(&tim, 0, sizeof tim);
            if (re2_ems_load_bank(ems, n, kind, &s_eb, &tim) != 0 || tim.width <= 0 || s_eb.md1.mesh_count <= 0) continue;
            char name[40];
            snprintf(name, sizeof name, "RE2/CDEMD0 kind 0x%02X", kind);
            pruef_t p = pruefe(&s_eb.md1, &tim);
            melden(name, &s_eb.md1, &p);
            banken++;
        }
        printf("RE2-Banken geprueft: %d\n", banken);
        CHECK(banken > 0, "keine RE2-Bank mit EMD + TIM gelesen");
        free(ems);
    }

    if (fails) { fprintf(stderr, "test_r41_uv_textur: %d Fehler\n", fails); return 1; }
    printf("test_r41_uv_textur: alle Pruefungen gruen\n");
    return 0;
}
