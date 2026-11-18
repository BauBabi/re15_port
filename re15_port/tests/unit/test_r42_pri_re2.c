/*
 * test_r42_pri_re2.c — Runde 42 Strang rest, Stufe 6: die vier Vordergrund-Masken aus RE2 (STAGE4-6, gleiches Hintergrundbild).
 * NUTZERWUNSCH 15.11. "STAGE4 Labor: fehlende Masken/PRIs aus RE2 uebernehmen", 18.11. "ob du die grossteils von Resident Evil 2
 * uebernehmen kannst". Daten: tools/maske/re2_uebernahme_r42.py, Lader bg_pc.c re15_pri_re2_zusatz, Anhaengen
 * pri_common.c re15_pri_re2_anhaengen, Aufruf platform/pc/main.c.
 *
 * daten      Je Kamera und je Fassung (Leon xxx0, Elza xxx1): MASKS/ROOM%04X_RE2.MSK ist ein R15M-Container mit genau der
 *            Sektion dieses Cuts. Jede Maske = die RE2-Maske an derselben Stelle der RE2-Sektion (RE2 RDT, Kamerasatz RID
 *            @RDT+0x24 + Cut*0x20, +0x1C; RE2-Form gelesen mit re15_pri_parse_section_re2 = RE2 FUN_80049b1c @0x80049c10-48):
 *            srcX/srcY/Breite/Hoehe gleich, dst = RE2-dst + Bildversatz, Tiefe = ceil(d/2) - 1 (RE2 otz>>3 @0x80027d60,
 *            RE1.5 otz>>4 @0x8002565c, Masken-OT = depth @0x80049dac / @0x80039658, gleicher Bucket: RE2 Maske oben
 *            @0x80026770, RE1.5 Figur oben @0x8001ce54). Der Atlas MASKS/ROOM%04X_RE2_%02d.TIM hat die Abmessung und die
 *            durchsichtigen Punkte (Index 0) des RE2-Atlas (Gegenprobe: info/re2leon/COMMON/BSS/ROOMrrr/ROOMrrrrccc.tim, bytegleich
 *            mit dem RE2-eigenen Entpacker FUN_80010084, tools/r41_g5auftritt/re2_schnitt9.py). Leon- und Elza-Datei bytegleich;
 *            RE1.5-Kamerasatz Leon == Elza (+0x00..+0x1B, Projektion @0x80021e68/FUN_80053ca4).
 * farbe      Jeder gezeichnete Atlaspunkt hat (bis auf die 5-Bit-CLUT) die Farbe des RE1.5-Hintergrunds an seiner Bildstelle
 *            (Engine-Dekoder bss_common/vlc/mdec): mittlerer Abstand <= 12/765 und deutlich kleiner als ohne den Bildversatz
 *            (ROOM5100 K7: RE1.5-Bild um +1 Spalte gegen RE2 verschoben).
 * anhaengen  Die Kette des Zeichners (main.c): Basis = Original-Sektion (ROOM4090 K2, 24 Capcom-Masken @RDT+0x444) bzw. NULL
 *            (50D0 K10 @0x7F0, 5100 K7 @0x5A0, 6000 K1 @0x6DC) -> R15M-Seitendaten + Ausschluss (masken36) -> Zusatz. Die Basis
 *            bleibt unveraendert vorn, der Zusatz folgt mit srcY + Atlas-Versatz, Summe <= RE15_PRI_MAX_MASKS_PER_CUT.
 */
#include "re15_pri.h"
#include "re15_masken36_r37.h"
#include "re15_bss.h"
#include "re15_tim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_RE2_DIR
#define RE15_RE2_DIR "../info/re2leon"
#endif

static int g_fail = 0;
#define PRUEF(cond, ...) do { if (!(cond)) { printf("  FAIL: " __VA_ARGS__); printf("\n"); g_fail = 1; } \
                              else { printf("  ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static const char *g_psx = RE15_ASSET_PSX_DIR;   /* argv[2] ueberschreibt (Gegenprobe gegen einen alten Asset-Baum) */

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)(sz > 0 ? sz : 1));
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}

static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

typedef struct {
    unsigned raum; int cut;            /* RE1.5 (Leon) */
    const char *re2; int re2_cut;      /* RE2 Leon PL0/RDT/ROOM<re2>.RDT */
    uint32_t re2_pri;                  /* Abschnitt-Offset in der RE2-RDT (Kamerasatz +0x1C, selbst gelesen) */
    const char *re2_tim;               /* COMMON/BSS/<...>.tim (fremd entpackt, bytegleich mit FUN_80010084) */
    int vx, vy;                        /* Bildversatz RE1.5 gegen RE2 (re2_uebernahme_r42.py "Bildlage") */
    uint32_t basis_off;                /* RE1.5 sprite.pri-Offset des Cuts (Kamerasatz +0x1C) */
    int basis_n;                       /* Original-Masken des Cuts (0 = NULL-Sektion FF FF FF FF) */
} kamera_t;

static const kamera_t K[4] = {
    { 0x4090,  2, "60D0",  2, 0x4E0,  "COMMON/BSS/ROOM60D/ROOM60D0002.tim", 0, 0, 0x444, 24 },
    { 0x50D0, 10, "6030", 10, 0x1BE8, "COMMON/BSS/ROOM603/ROOM6030010.tim", 0, 0, 0x7F0, 0 },
    { 0x5100,  7, "6020",  3, 0x5F4,  "COMMON/BSS/ROOM602/ROOM6020003.tim", 1, 0, 0x5A0, 0 },
    { 0x6000,  1, "7000",  1, 0x844,  "COMMON/BSS/ROOM700/ROOM7000001.tim", 0, 0, 0x6DC, 0 },
};

static int zusatz_lesen(unsigned raum, int cut, re15_pri_cut_t *z, uint8_t **msk_out, size_t *msk_n)
{
    char p[600];
    size_t n = 0;
    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X_RE2.MSK", g_psx, raum);
    uint8_t *b = slurp(p, &n);
    memset(z, 0, sizeof *z);
    if (!b) return -1;
    uint32_t off = re15_pri_msk_section_offset(b, n, cut);
    int k = off ? re15_pri_parse_section(b, n, off, z) : 0;
    if (msk_out) { *msk_out = b; *msk_n = n; } else free(b);
    return k;
}

static int tim_lesen(const char *p, uint8_t **buf, re15_tim_t *t)
{
    size_t n = 0;
    *buf = slurp(p, &n);
    if (!*buf) return -1;
    return re15_tim_parse(*buf, (int)n, t);
}

static int teil_daten(void)
{
    printf("== daten: Zusatzmasken == RE2-Masken (umgerechnet), Atlas im RE2-Layout ==\n");
    for (int k = 0; k < 4; k++) {
        const kamera_t *c = &K[k];
        char p[600];
        size_t n2 = 0;
        snprintf(p, sizeof p, "%s/PL0/RDT/ROOM%s.RDT", RE15_RE2_DIR, c->re2);
        uint8_t *r2 = slurp(p, &n2);
        PRUEF(r2 != NULL, "RE2 %s lesbar", p);
        if (!r2) continue;
        uint32_t rid = u32(r2 + 0x24);
        uint32_t po = u32(r2 + rid + (uint32_t)c->re2_cut * 32u + 0x1Cu);
        PRUEF(po == c->re2_pri, "RE2 ROOM%s K%d: sprite.pri @0x%X (Kamerasatz RDT+0x%X+0x1C), erwartet 0x%X", c->re2, c->re2_cut,
              (unsigned)po, (unsigned)(rid + (uint32_t)c->re2_cut * 32u), (unsigned)c->re2_pri);
        static re15_pri_cut_t a, z, ze;
        int na = re15_pri_parse_section_re2(r2, n2, po, &a);
        uint8_t *mskL = NULL, *mskE = NULL; size_t nL = 0, nE = 0;
        int nz = zusatz_lesen(c->raum, c->cut, &z, &mskL, &nL);
        int ne = zusatz_lesen(c->raum + 1, c->cut, &ze, &mskE, &nE);
        PRUEF(nz > 0, "ROOM%04X_RE2.MSK Cut %d: %d Masken", c->raum, c->cut, nz);
        PRUEF(ne > 0 && mskL && mskE && nL == nE && memcmp(mskL, mskE, nL) == 0, "ROOM%04X_RE2.MSK == ROOM%04X_RE2.MSK (Elza bytegleich)",
              c->raum, c->raum + 1);
        PRUEF(nz == na && z.draw_count == a.draw_count, "ROOM%04X K%d: %d/%d Masken (gebaut/gezeichnet) == RE2 %d/%d", c->raum, c->cut,
              nz, z.draw_count, na, a.draw_count);
        int abw = 0, ungerade = 0;
        for (int i = 0; i < nz && i < na; i++) {
            const re15_pri_mask_t *m = &z.masks[i], *q = &a.masks[i];
            int soll = ((int)q->depth + 1) / 2 - 1;
            if (soll < 0) soll = 0;
            if (q->depth & 1) ungerade++;
            if (m->srcX != q->srcX || m->srcY != q->srcY || m->width != q->width || m->height != q->height ||
                (int16_t)m->dstX != (int16_t)q->dstX + c->vx || (int16_t)m->dstY != (int16_t)q->dstY + c->vy || (int)m->depth != soll) {
                if (abw < 3) printf("    Maske %d: src (%d,%d) dst (%d,%d) %dx%d t %d | RE2 src (%d,%d) dst (%d,%d) %dx%d t %d -> %d\n", i,
                                    m->srcX, m->srcY, (int16_t)m->dstX, (int16_t)m->dstY, m->width, m->height, m->depth,
                                    q->srcX, q->srcY, (int16_t)q->dstX, (int16_t)q->dstY, q->width, q->height, q->depth, soll);
                abw++;
            }
        }
        PRUEF(abw == 0, "ROOM%04X K%d: jede Maske = RE2 (src, Groesse, dst + (%d,%d), Tiefe ceil(d/2)-1; %d ungerade d)", c->raum, c->cut,
              c->vx, c->vy, ungerade);
        /* nur dieser Cut belegt */
        if (mskL) {
            uint32_t cuts = u32(mskL + 8), belegt = 0;
            for (uint32_t i = 0; i < cuts; i++) if (u32(mskL + 12 + 4 * i)) belegt++;
            PRUEF(belegt == 1 && u32(mskL + 12 + 4u * (uint32_t)c->cut) != 0, "ROOM%04X_RE2.MSK: genau eine Sektion (Cut %d) von %u", c->raum,
                  c->cut, (unsigned)cuts);
        }
        /* Atlas: Abmessung + durchsichtige Punkte wie RE2 */
        {
            uint8_t *b1 = NULL, *b2 = NULL, *b3 = NULL; re15_tim_t t1, t2, t3;
            char q[600];
            snprintf(p, sizeof p, "%s/MASKS/ROOM%04X_RE2_%02d.TIM", g_psx, c->raum, c->cut);
            snprintf(q, sizeof q, "%s/%s", RE15_RE2_DIR, c->re2_tim);
            int ok1 = tim_lesen(p, &b1, &t1) == 0, ok2 = tim_lesen(q, &b2, &t2) == 0;
            PRUEF(ok1 && t1.bpp == 8 && t1.has_clut, "%s: 8-bpp-TIM mit CLUT", p);
            PRUEF(ok2, "RE2-Atlas %s lesbar", q);
            if (ok1 && ok2) {
                int gleich = (t1.width == t2.width && t1.height == t2.height), d0 = 0, opak = 0;
                if (gleich) {
                    const uint8_t *i1 = (const uint8_t *)t1.pixels, *i2 = (const uint8_t *)t2.pixels;
                    for (int i = 0; i < t1.width * t1.height; i++) { if ((i1[i] == 0) != (i2[i] == 0)) d0++; if (i1[i]) opak++; }
                }
                PRUEF(gleich && d0 == 0, "Atlas %dx%d == RE2 %dx%d, durchsichtige Punkte gleich (%d Abweichungen, %d undurchsichtig)",
                      t1.width, t1.height, t2.width, t2.height, d0, opak);
            }
            snprintf(p, sizeof p, "%s/MASKS/ROOM%04X_RE2_%02d.TIM", g_psx, c->raum + 1, c->cut);
            size_t n3 = 0, n1 = 0;
            b3 = slurp(p, &n3);
            free(b1);
            snprintf(q, sizeof q, "%s/MASKS/ROOM%04X_RE2_%02d.TIM", g_psx, c->raum, c->cut);
            b1 = slurp(q, &n1);
            PRUEF(b1 && b3 && n1 == n3 && memcmp(b1, b3, n1) == 0, "ROOM%04X_RE2_%02d.TIM == Elza-Datei", c->raum, c->cut);
            (void)t3;
            free(b1); free(b2); free(b3);
        }
        /* RE1.5-Kamerasatz Leon == Elza, Basis-Sektion wie erwartet */
        {
            size_t nl = 0, ne2 = 0;
            snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", g_psx, c->raum >> 12, c->raum);
            uint8_t *l = slurp(p, &nl);
            snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", g_psx, c->raum >> 12, c->raum + 1);
            uint8_t *e = slurp(p, &ne2);
            if (l && e) {
                uint32_t sl = u32(l + 0x24) + (uint32_t)c->cut * 32u, se = u32(e + 0x24) + (uint32_t)c->cut * 32u;
                PRUEF(memcmp(l + sl, e + se, 0x1C) == 0, "ROOM%04X/%04X Kamerasatz K%d +0x00..+0x1B gleich", c->raum, c->raum + 1, c->cut);
                uint32_t bo = u32(l + sl + 0x1C);
                static re15_pri_cut_t b;
                int nb = re15_pri_parse_section(l, nl, bo, &b);
                PRUEF(bo == c->basis_off && nb == c->basis_n, "ROOM%04X K%d: sprite.pri @0x%X mit %d Original-Masken", c->raum, c->cut,
                      (unsigned)bo, nb);
            } else PRUEF(0, "RDT ROOM%04X/%04X lesbar", c->raum, c->raum + 1);
            free(l); free(e);
        }
        free(mskL); free(mskE); free(r2);
    }
    return g_fail;
}

/* RE1.5-Hintergrund des Cuts (Engine-Dekoder wie probe_bg_dump). */
static int bg_rgb(unsigned raum, int cut, uint8_t *rgb)
{
    char p[600]; size_t n = 0;
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%03X.BSS", g_psx, raum >> 12, raum >> 4);
    uint8_t *bss = slurp(p, &n);
    if (!bss || n < (size_t)(cut + 1) * 0x10000u) { free(bss); return -1; }
    re15_bss_chunk_t ch;
    int rv = -2;
    if (re15_bss_parse_chunk(bss + (size_t)cut * 0x10000u, 0x10000, &ch) && re15_bss_chunk_has_video(&ch)) {
        size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
        int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
        int k = co ? re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap) : -1;
        if (k >= 0 && re15_bss_mdec_decode(co, (size_t)k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) == 0) rv = 0;
        free(co);
    }
    free(bss);
    return rv;
}

static int teil_farbe(void)
{
    printf("== farbe: Atlaspunkt == RE1.5-Bild an der Bildstelle ==\n");
    static uint8_t rgb[320 * 240 * 3];
    for (int k = 0; k < 4; k++) {
        const kamera_t *c = &K[k];
        static re15_pri_cut_t z;
        char p[600];
        uint8_t *tb = NULL; re15_tim_t t;
        if (zusatz_lesen(c->raum, c->cut, &z, NULL, NULL) <= 0) { PRUEF(0, "ROOM%04X_RE2.MSK fehlt", c->raum); continue; }
        snprintf(p, sizeof p, "%s/MASKS/ROOM%04X_RE2_%02d.TIM", g_psx, c->raum, c->cut);
        if (tim_lesen(p, &tb, &t) != 0 || bg_rgb(c->raum, c->cut, rgb) != 0) { PRUEF(0, "Atlas/Hintergrund ROOM%04X K%d", c->raum, c->cut); free(tb); continue; }
        const uint8_t *idx = (const uint8_t *)t.pixels;
        double sum[2] = {0, 0}; long cnt[2] = {0, 0};
        for (int v = 0; v < 2; v++) {        /* v=0: mit Versatz (wie gebaut), v=1: Versatz zurueckgenommen */
            for (int i = 0; i < z.draw_count; i++) {
                const re15_pri_mask_t *m = &z.masks[i];
                for (int y = 0; y < m->height; y++)
                    for (int x = 0; x < m->width; x++) {
                        int ax = m->srcX + x, ay = m->srcY + y;
                        int X = (int16_t)m->dstX + x - (v ? c->vx : 0), Y = (int16_t)m->dstY + y - (v ? c->vy : 0);
                        if (ax >= t.width || ay >= t.height || X < 0 || Y < 0 || X >= 320 || Y >= 240) continue;
                        uint8_t ix = idx[ay * t.width + ax];
                        if (!ix) continue;
                        uint16_t cc = t.clut[ix];
                        int r = (cc & 31) << 3, g = ((cc >> 5) & 31) << 3, b = ((cc >> 10) & 31) << 3;
                        const uint8_t *q = rgb + (Y * 320 + X) * 3;
                        sum[v] += abs(r - q[0]) + abs(g - q[1]) + abs(b - q[2]);
                        cnt[v]++;
                    }
            }
        }
        double m0 = cnt[0] ? sum[0] / cnt[0] : 999, m1 = cnt[1] ? sum[1] / cnt[1] : 999;
        PRUEF(cnt[0] > 1000 && m0 <= 12.0, "ROOM%04X K%d: %ld Punkte, mittlerer Abstand %.2f/765 (<= 12)", c->raum, c->cut, cnt[0], m0);
        if (c->vx || c->vy)
            PRUEF(m1 > m0 * 2.0, "ROOM%04X K%d: ohne den Versatz (%d,%d) waere er %.2f (> 2x)", c->raum, c->cut, c->vx, c->vy, m1);
        free(tb);
    }
    return g_fail;
}

static int teil_anhaengen(void)
{
    printf("== anhaengen: Kette des Zeichners (Basis -> Zusatz) ==\n");
    for (int k = 0; k < 4; k++) {
        const kamera_t *c = &K[k];
        static re15_pri_cut_t pri, vorher, z;
        char p[600]; size_t n = 0;
        memset(&pri, 0, sizeof pri);
        snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", g_psx, c->raum >> 12, c->raum);
        uint8_t *rdt = slurp(p, &n);
        if (!rdt) { PRUEF(0, "%s", p); continue; }
        int pn = re15_pri_parse_section(rdt, n, c->basis_off, &pri);
        if (pn == 0) {                                   /* R15M-Seitendaten wie main.c */
            size_t mn = 0;
            snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", g_psx, c->raum);
            uint8_t *msk = slurp(p, &mn);
            if (msk) {
                uint32_t mo = re15_pri_msk_section_offset(msk, mn, c->cut);
                if (mo) pn = re15_pri_parse_section(msk, mn, mo, &pri);
                if (pn > 0) pn = re15_masken36_filter(c->raum, c->cut, &pri);
                free(msk);
            }
        }
        vorher = pri;
        int nz = zusatz_lesen(c->raum, c->cut, &z, NULL, NULL);
        const int yv = (pn > 0) ? 256 : 0;               /* Atlas des Cuts 256 Zeilen (Original-/eigener Atlas 256x256) */
        if (pn == 0) { pri.mask_count = 0; pri.draw_count = 0; }
        int a = re15_pri_re2_anhaengen(&pri, &z, yv);
        PRUEF(nz > 0 && a == z.draw_count, "ROOM%04X K%d: Basis %d gezeichnet, Zusatz %d angehaengt (von %d)", c->raum, c->cut,
              vorher.draw_count, a, nz);
        PRUEF(pri.draw_count == vorher.draw_count + z.draw_count && pri.draw_count <= RE15_PRI_MAX_MASKS_PER_CUT,
              "ROOM%04X K%d: zusammen %d <= %d", c->raum, c->cut, pri.draw_count, RE15_PRI_MAX_MASKS_PER_CUT);
        PRUEF(memcmp(pri.masks, vorher.masks, sizeof(re15_pri_mask_t) * (size_t)vorher.draw_count) == 0,
              "ROOM%04X K%d: die %d Basis-Masken unveraendert vorn", c->raum, c->cut, vorher.draw_count);
        int abw = 0;
        for (int i = 0; i < a; i++) {
            re15_pri_mask_t s = z.masks[i];
            s.srcY = (uint16_t)(s.srcY + yv);
            if (memcmp(&s, &pri.masks[vorher.draw_count + i], sizeof s) != 0) abw++;
        }
        PRUEF(abw == 0, "ROOM%04X K%d: Zusatz in Reihenfolge, srcY + %d", c->raum, c->cut, yv);
        free(rdt);
    }
    return g_fail;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "daten";
    if (argc > 2) g_psx = argv[2];
    if (!strcmp(teil, "daten")) teil_daten();
    else if (!strcmp(teil, "farbe")) teil_farbe();
    else if (!strcmp(teil, "anhaengen")) teil_anhaengen();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    printf(g_fail ? "== ROT ==\n" : "== GRUEN ==\n");
    return g_fail;
}
