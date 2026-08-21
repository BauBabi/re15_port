/* probe_texel_key_census.c — MESSSONDE (kein add_test): wie viele der TATSAECHLICH
 * gesampelten Texel eines Modells/Sprites loesen zu 0x0000 auf?
 *
 * Zweck: die Frage aus dem Auftrag "warum gibt es RE15_TIM_KEY_NONE / bekommen die
 * MD1-Modelltexturen unter dem GPU-Farbschluessel Loecher?" mit Zahlen beantworten,
 * statt sie zu glauben.
 *
 * Gemessen wird ueber DIESELBE Funktion, die der Upload benutzt (re15_tim_texel_argb),
 * auf den echten Asset-Bytes, mit der ECHTEN CLUT-Wahl pro Face:
 *     clut_idx = ((uv->clut >> 6) & 0x1FF) - tim.clut_y     (render_pc.c:2056-2058)
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_emd.h"
#include "re15_esp.h"
#include "re15_ems.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
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

static int tim_index_at(const re15_tim_t *t, int x, int y)
{
    const uint8_t *src = (const uint8_t *)t->pixels;
    long i = (long)y * t->width + x;
    if (t->bpp == 8) return src[i];
    return (i & 1) ? (src[i >> 1] >> 4) : (src[i >> 1] & 0xF);
}

typedef struct { long sampled, key0, idx0, idx0_nonzero; } cov_t;

/* Die CLUT-Zeile fuer ein Face waehlen — exakt wie render_pc.c:2056-2058. */
static const uint16_t *clut_row_for(const re15_tim_t *t, unsigned clut_word, int *out_idx)
{
    int row_width = (t->bpp == 4) ? 16 : 256;
    int n_cluts   = (t->has_clut && row_width > 0) ? (t->clut_entries / row_width) : 1;
    if (n_cluts < 1) n_cluts = 1;
    int clut_y = (int)((clut_word >> 6) & 0x1FF);
    int idx    = clut_y - t->clut_y;
    if (idx < 0 || idx >= n_cluts) idx = 0;
    if (out_idx) *out_idx = idx;
    return &t->clut[(long)idx * row_width];
}

static void cover_face(const re15_tim_t *tim, const uint8_t *us, const uint8_t *vs, int n,
                       unsigned page, unsigned clut_word, cov_t *acc)
{
    const uint16_t *cr = clut_row_for(tim, clut_word, NULL);
    int u0 = us[0], u1 = us[0], v0 = vs[0], v1 = vs[0];
    for (int k = 1; k < n; k++) {
        if (us[k] < u0) u0 = us[k];
        if (us[k] > u1) u1 = us[k];
        if (vs[k] < v0) v0 = vs[k];
        if (vs[k] > v1) v1 = vs[k];
    }
    int po = (int)(page & 0x000F) * 128;
    for (int y = v0; y <= v1; y++) {
        for (int x = u0; x <= u1; x++) {
            int X = x + po;
            if (X >= tim->width || y >= tim->height) continue;
            int idx = tim_index_at(tim, X, y);
            uint16_t raw = cr[idx];
            acc->sampled++;
            if (idx == 0) { acc->idx0++; if (raw != 0x0000) acc->idx0_nonzero++; }
            if (re15_tim_texel_argb(raw) == 0u) acc->key0++;
        }
    }
}

static cov_t cover_md1(const re15_md1_t *md1, const re15_tim_t *tim)
{
    cov_t c = {0,0,0,0};
    for (int m = 0; m < md1->mesh_count; m++) {
        const re15_md1_mesh_t *me = &md1->meshes[m];
        for (int i = 0; i < me->triangle_count; i++) {
            const re15_md1_tri_uv_t *uv = &me->triangle_uvs[i];
            uint8_t us[3] = { uv->u0, uv->u1, uv->u2 };
            uint8_t vs[3] = { uv->v0, uv->v1, uv->v2 };
            cover_face(tim, us, vs, 3, uv->page, uv->clut, &c);
        }
        for (int i = 0; i < me->quad_count; i++) {
            const re15_md1_quad_uv_t *uv = &me->quad_uvs[i];
            uint8_t us[4] = { uv->u0, uv->u1, uv->u2, uv->u3 };
            uint8_t vs[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
            cover_face(tim, us, vs, 4, uv->page, uv->clut, &c);
        }
    }
    return c;
}

static void report(const char *tag, cov_t c, const re15_tim_t *t)
{
    int row_width = (t->bpp == 4) ? 16 : 256;
    int n_cluts   = (t->has_clut && row_width > 0) ? (t->clut_entries / row_width) : 1;
    printf("%-34s %2dbpp %4dx%-4d cluts=%-3d  gesampelt=%8ld  key0=%8ld (%5.1f%%)  "
           "idx0=%8ld  idx0-mit-Farbe=%ld\n",
           tag, t->bpp, t->width, t->height, n_cluts,
           c.sampled, c.key0, c.sampled ? 100.0*(double)c.key0/(double)c.sampled : 0.0,
           c.idx0, c.idx0_nonzero);
}

/* ---- Ganze TIM (ohne UV): wie viele Texel loesen zu 0 auf, CLUT-Zeile 0 --------------- */
static void whole_tim(const char *tag, const re15_tim_t *t)
{
    long n = (long)t->width * t->height, k0 = 0, i0 = 0;
    int row_width = (t->bpp == 4) ? 16 : 256;
    int n_cluts   = (t->has_clut && row_width > 0) ? (t->clut_entries / row_width) : 1;
    if (n_cluts < 1) n_cluts = 1;
    for (long i = 0; i < n; i++) {
        int x = (int)(i % t->width), y = (int)(i / t->width);
        int idx = (t->bpp == 16) ? 0 : tim_index_at(t, x, y);
        uint16_t raw = (t->bpp == 16) ? t->pixels[i] : t->clut[idx];
        if (idx == 0) i0++;
        if (raw == 0x0000) k0++;
    }
    printf("%-34s %2dbpp %4dx%-4d cluts=%-3d  texel=%8ld  wert0=%8ld (%5.1f%%)  idx0=%ld\n",
           tag, t->bpp, t->width, t->height, n_cluts, n, k0,
           n ? 100.0*(double)k0/(double)n : 0.0, i0);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    size_t sz = 0;

    /* ===== A: Spieler-PLD (der GENANNTE Gegen-Fall: "dreieckige Loecher") ============== */
    printf("===== A: Spieler-/NPC-Modelle (PLD-Split) =====\n");
    {
        static const char *mods[][3] = {
            { "PLD/PL00.MD1", "PLD/PL00.TIM", "Leon PL00" },
            { "PLD/PL04.MD1", "PLD/PL04.TIM", "Elza PL04" },
            { "PLD/ELLIOT.MD1", "PLD/ELLIOT.TIM", "Elliot" },
        };
        for (size_t i = 0; i < sizeof(mods)/sizeof(mods[0]); i++) {
            size_t msz = 0, tsz = 0;
            snprintf(path, sizeof path, "%s/%s", base, mods[i][0]);
            uint8_t *mb = read_file(path, &msz);
            snprintf(path, sizeof path, "%s/%s", base, mods[i][1]);
            uint8_t *tb = read_file(path, &tsz);
            if (!mb || !tb) { printf("  %-20s FEHLT\n", mods[i][2]); free(mb); free(tb); continue; }
            static re15_md1_t md1; re15_tim_t tim;
            if (re15_md1_parse(mb, msz, &md1) == 0 && re15_tim_parse(tb, tsz, &tim) == 0)
                report(mods[i][2], cover_md1(&md1, &tim), &tim);
            else printf("  %-20s PARSE-FEHLER\n", mods[i][2]);
            free(mb); free(tb);
        }
    }

    /* ===== B: Gegner-EMDs ============================================================= */
    printf("\n===== B: Gegner-EMDs (EMD/EM*.EMD) =====\n");
    size_t ems_sz = 0;
    snprintf(path, sizeof path, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(path, &ems_sz);
    for (int t = 0; t < 0x60; t++) {
        size_t bsz = 0; uint8_t *b = NULL;
        snprintf(path, sizeof path, "%s/EMD/EM%02X.EMD", base, t);
        b = read_file(path, &sz);
        if (!b && ems) {
            int idx = re15_ems_index_for_type((uint8_t)t);
            size_t off = 0, len = 0;
            if (idx >= 0 && re15_ems_get_entry(ems, ems_sz, idx, &off, &len) == 0) {
                b = (uint8_t *)malloc(len); if (b) { memcpy(b, ems + off, len); sz = len; }
            }
        }
        (void)bsz;
        if (!b) continue;
        static re15_md1_t md1; static re15_emd_skeleton_t sk; static re15_emd_animation_t an;
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        if (re15_emd_parse_container(b, sz, &md1, &sk, &an, &tim) == 0 && tim.width > 0) {
            char tag[64]; snprintf(tag, sizeof tag, "EM%02X", t);
            report(tag, cover_md1(&md1, &tim), &tim);
        }
        free(b);
    }

    /* ===== C: Raum-Props ueber ALLE RDTs ============================================== */
    printf("\n===== C: Raum-Props (alle RDTs, nur die mit key0 > 0) =====\n");
    {
        long rooms = 0, props = 0, props_key = 0;
        for (int st = 1; st <= 6; st++) {
            for (int r = 0; r < 0x100; r++) {
                snprintf(path, sizeof path, "%s/STAGE%d/ROOM%d%02X0.RDT", base, st, st, r);
                uint8_t *b = read_file(path, &sz);
                if (!b) continue;
                rooms++;
                static re15_rdt_t rdt;
                if (re15_rdt_parse(b, sz, &rdt) == 0) {
                    for (int op = 0; op < rdt.prop_count && op < RE15_RDT_MAX_PROPS; op++) {
                        if (!rdt.prop_tim[op] || !rdt.prop_md1[op]) continue;
                        re15_tim_t tim; static re15_md1_t md1;
                        if (re15_tim_parse(rdt.prop_tim[op], rdt.prop_tim_size[op], &tim) != 0) continue;
                        if (re15_md1_parse(rdt.prop_md1[op], rdt.prop_md1_size[op], &md1) != 0) continue;
                        props++;
                        cov_t c = cover_md1(&md1, &tim);
                        if (c.key0 > 0) {
                            props_key++;
                            char tag[64]; snprintf(tag, sizeof tag, "ROOM%d%02X0 prop[%d]", st, r, op);
                            report(tag, c, &tim);
                        }
                    }
                }
                free(b);
            }
        }
        printf("  -- %ld RDTs, %ld Props, davon %ld mit key0>0\n", rooms, props, props_key);
    }

    /* ===== D: Raum-ESP-Sprite-Sheets (die FLAMMEN) ==================================== */
    printf("\n===== D: Raum-ESP-TIMs (Effekt-Sheets) =====\n");
    {
        static const struct { int st; const char *room; } esp_rooms[] = {
            { 1, "ROOM1090" }, { 1, "ROOM11E0" }, { 1, "ROOM1140" },
        };
        for (size_t i = 0; i < sizeof(esp_rooms)/sizeof(esp_rooms[0]); i++) {
            snprintf(path, sizeof path, "%s/STAGE%d/%s.RDT", base, esp_rooms[i].st, esp_rooms[i].room);
            uint8_t *b = read_file(path, &sz);
            if (!b) continue;
            #define U32LE(o) ((uint32_t)b[o] | ((uint32_t)b[(o)+1]<<8) | \
                              ((uint32_t)b[(o)+2]<<16) | ((uint32_t)b[(o)+3]<<24))
            uint32_t idh = U32LE(0x4C), pe = U32LE(0x50), tb = U32LE(0x54), te = U32LE(0x58);
            #undef U32LE
            static re15_esp_t esp;
            if (re15_esp_parse(b, sz, idh, pe, tb, te, &esp) == 0) {
                for (int ei = 0; ei < esp.id_count; ei++) {
                    if (!esp.eff[ei].tim_off || (uint32_t)sz <= esp.eff[ei].tim_off) continue;
                    re15_tim_t tim;
                    if (re15_tim_parse(b + esp.eff[ei].tim_off, sz - esp.eff[ei].tim_off, &tim) != 0)
                        continue;
                    char tag[64];
                    snprintf(tag, sizeof tag, "%s eff[%d] id=0x%02X slot=%d",
                             esp_rooms[i].room, ei, esp.eff[ei].effect_id, 36 + ei);
                    whole_tim(tag, &tim);
                }
            }
            free(b);
        }
    }

    /* ===== E: die globalen Effekt-Sheets ============================================== */
    printf("\n===== E: globale Effekt-Sheets (extracted_fx) =====\n");
    {
        static const char *fx[] = { "extracted_fx/effect0_blood.tim",
                                    "extracted_fx/effect2_muzzle.tim",
                                    "extracted_fx/effect3_smoke.tim",
                                    "extracted_fx/effect4_shell.tim",
                                    "extracted_fx/effect8_fire.tim" };
        static const int fxslot[] = { 20, 21, 22, 23, 44 };
        for (size_t i = 0; i < sizeof(fx)/sizeof(fx[0]); i++) {
            snprintf(path, sizeof path, "%s/../%s", base, fx[i]);
            uint8_t *b = read_file(path, &sz);
            if (!b) { printf("  %s FEHLT\n", fx[i]); continue; }
            re15_tim_t tim;
            if (re15_tim_parse(b, sz, &tim) == 0) {
                char tag[80]; snprintf(tag, sizeof tag, "%s slot=%d", fx[i] + 14, fxslot[i]);
                whole_tim(tag, &tim);
            }
            free(b);
        }
    }
    return 0;
}
