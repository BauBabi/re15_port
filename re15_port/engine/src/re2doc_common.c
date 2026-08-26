/* re2doc_common.c — Lader und Pixel-Zugriff der RE2-Dokumentseiten.
 * Beleg-Block und Geometrie: include/re15_re2doc.h. Diese Datei enthaelt KEINE
 * Konstante, die nicht dort belegt waere; die Masse kommen aus den TIM-Koepfen.
 *
 * Aufbau wie itps_common.c (die Item-Modal-Bildquelle): die Engine liefert Pixel,
 * die Plattform zeichnet. Kein VRAM, keine Texturseite — der FILE-Schirm des Ports
 * blittet die zwei Rechtecke direkt.
 */
#include "re15_re2doc.h"
extern unsigned char *re15_asset_read_file(const char *path, int *out_size);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RE2DOC_CACHE 4                 /* Titelbild + aktuelle Seite + etwas Luft */

typedef struct {
    int      doc, page, kind;          /* Schluessel; doc < 0 = Platz frei */
    uint8_t *buf;
    size_t   size;
    int      w, h, bpp;
    size_t   clut_off;                 /* Byte-Offset der CLUT-Eintraege im Puffer */
    size_t   img_off;                  /* Byte-Offset der Bilddaten                */
    int      stride;                   /* Bytes je Bildzeile                       */
    int      clut_entries;
} re2doc_tile_t;

static re2doc_tile_t s_cache[RE2DOC_CACHE];
static int  s_next;
static char s_root[512];
static int  s_selected = -1;

void re15_re2doc_set_root(const char *dir)
{
    if (!dir || !*dir) { s_root[0] = 0; return; }
    snprintf(s_root, sizeof s_root, "%s", dir);
}

void re15_re2doc_select(int doc) { s_selected = doc; }
int  re15_re2doc_selected(void)  { return s_selected; }

static void tile_name(char *out, size_t cap, int doc, int page, int kind)
{
    if (kind == RE15_RE2DOC_PAPER)
        snprintf(out, cap, "FILE%02d_title_paper.TIM", doc);
    else if (page < 0)
        snprintf(out, cap, "FILE%02d_title_page.TIM", doc);
    else
        snprintf(out, cap, "FILE%02d_p%02d_page.TIM", doc, page);
}

/* PSX-TIM-Kopf zerlegen. Nur 4bpp und 8bpp mit CLUT — genau das, was die Dokumente
 * fuehren (Textseite 4bpp, Illustration 8bpp; s. Beleg-Block im Header). */
static int tile_parse(re2doc_tile_t *t)
{
    if (!t->buf || t->size < 20) return -1;
    uint32_t magic = (uint32_t)(t->buf[0] | (t->buf[1] << 8) | (t->buf[2] << 16) | ((uint32_t)t->buf[3] << 24));
    uint32_t flags = (uint32_t)(t->buf[4] | (t->buf[5] << 8) | (t->buf[6] << 16) | ((uint32_t)t->buf[7] << 24));
    if (magic != 0x10) return -1;
    int pmode = (int)(flags & 7);
    int has_clut = (int)((flags >> 3) & 1);
    if (!has_clut || (pmode != 0 && pmode != 1)) return -1;

    size_t o = 8;
    uint32_t clen = (uint32_t)(t->buf[o] | (t->buf[o+1] << 8) | (t->buf[o+2] << 16) | ((uint32_t)t->buf[o+3] << 24));
    int cw = (int)(t->buf[o+8] | (t->buf[o+9] << 8));
    int ch = (int)(t->buf[o+10] | (t->buf[o+11] << 8));
    if (clen < 12 || o + clen > t->size) return -1;
    t->clut_off = o + 12;
    t->clut_entries = cw * ch;
    o += clen;

    if (o + 12 > t->size) return -1;
    uint32_t ilen = (uint32_t)(t->buf[o] | (t->buf[o+1] << 8) | (t->buf[o+2] << 16) | ((uint32_t)t->buf[o+3] << 24));
    int iw = (int)(t->buf[o+8] | (t->buf[o+9] << 8));
    int ih = (int)(t->buf[o+10] | (t->buf[o+11] << 8));
    if (ilen < 12 || o + ilen > t->size) return -1;
    t->img_off = o + 12;
    t->bpp = (pmode == 0) ? 4 : 8;
    /* Ein VRAM-Wort haelt vier 4bpp- bzw. zwei 8bpp-Pixel. */
    t->w = (pmode == 0) ? iw * 4 : iw * 2;
    t->h = ih;
    t->stride = (pmode == 0) ? (t->w / 2) : t->w;
    if (t->w <= 0 || t->h <= 0) return -1;
    if (t->img_off + (size_t)t->stride * (size_t)t->h > t->size) return -1;
    return 0;
}

static re2doc_tile_t *tile_get(int doc, int page, int kind)
{
    if (doc < 0) return NULL;
    for (int i = 0; i < RE2DOC_CACHE; i++)
        if (s_cache[i].doc == doc && s_cache[i].page == page && s_cache[i].kind == kind
            && s_cache[i].buf)
            return &s_cache[i];

    char name[64], rel[700];
    tile_name(name, sizeof name, doc, page, kind);

    /* Suchreihenfolge wie in itps_common.c: erst der gesetzte Wurzelpfad, dann die
     * ueblichen Lagen relativ zum Arbeitsverzeichnis bzw. zum exe-Verzeichnis. */
    static const char *dirs[] = { "shared_assets/RE2/FILES/", "RE2/FILES/",
                                  "../shared_assets/RE2/FILES/", NULL };
    int sz = 0;
    uint8_t *data = NULL;
    if (s_root[0]) {
        snprintf(rel, sizeof rel, "%s/%s", s_root, name);
        data = re15_asset_read_file(rel, &sz);
    }
    for (int i = 0; dirs[i] && !data; i++) {
        snprintf(rel, sizeof rel, "%s%s", dirs[i], name);
        data = re15_asset_read_file(rel, &sz);
    }
    if (!data || sz <= 0) return NULL;

    re2doc_tile_t *t = &s_cache[s_next];
    s_next = (s_next + 1) % RE2DOC_CACHE;
    if (t->buf) free(t->buf);
    memset(t, 0, sizeof *t);
    t->buf = data; t->size = (size_t)sz;
    t->doc = doc; t->page = page; t->kind = kind;
    if (tile_parse(t) != 0) { free(t->buf); memset(t, 0, sizeof *t); t->doc = -1; return NULL; }
    return t;
}

int re15_re2doc_size(int doc, int page, re15_re2doc_kind_t kind, int *w, int *h)
{
    re2doc_tile_t *t = tile_get(doc, page, (int)kind);
    if (!t) return 0;
    if (w) *w = t->w;
    if (h) *h = t->h;
    return 1;
}

int re15_re2doc_page_height(int doc, int page)
{
    int w = 0, h = 0;
    if (!re15_re2doc_size(doc, page, RE15_RE2DOC_PAGE, &w, &h)) return 0;
    return h;
}

int re15_re2doc_page_count(int doc)
{
    int n = 0;
    /* Die Titelseite zaehlt mit; danach so lange, wie Folgeseiten da sind. */
    if (!re15_re2doc_size(doc, -1, RE15_RE2DOC_PAGE, NULL, NULL)) return 0;
    n = 1;
    for (int p = 0; p < 64; p++) {
        if (!re15_re2doc_size(doc, p, RE15_RE2DOC_PAGE, NULL, NULL)) break;
        n++;
    }
    return n;
}

int re15_re2doc_pixel(int doc, int page, re15_re2doc_kind_t kind,
                      int u, int v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    re2doc_tile_t *t = tile_get(doc, page, (int)kind);
    if (!t) return 0;
    if (u < 0 || v < 0 || u >= t->w || v >= t->h) return 0;

    unsigned idx;
    if (t->bpp == 4) {
        uint8_t byte = t->buf[t->img_off + (size_t)v * t->stride + (size_t)(u >> 1)];
        idx = ((u & 1) == 0) ? (unsigned)(byte & 0x0F) : (unsigned)(byte >> 4);
    } else {
        idx = t->buf[t->img_off + (size_t)v * t->stride + (size_t)u];
    }
    if (idx == 0) return 0;                       /* Index 0 = durchsichtig */
    if ((int)idx >= t->clut_entries) return 0;
    size_t c = t->clut_off + (size_t)idx * 2;
    unsigned col = (unsigned)(t->buf[c] | (t->buf[c + 1] << 8));   /* BGR555 */
    if (r) *r = (uint8_t)((col & 0x1f) << 3);
    if (g) *g = (uint8_t)(((col >> 5) & 0x1f) << 3);
    if (b) *b = (uint8_t)(((col >> 10) & 0x1f) << 3);
    return 1;
}
