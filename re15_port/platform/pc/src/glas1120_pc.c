/*
 * glas1120_pc.c — Runde 35 Spur M: PC-Seite des Fenster-Ereignisses ROOM1120 (engine/src/fenster_1120.c).
 *
 *   1. RE2-RAUM-ESP registrieren: shared_assets/RE2/GLAS1090.ESP (bytegleich RE2 room1090 effect.esp,
 *      Ids 29 10 11 12 13 14 0C 19) -> re2fx_register_raum (FUN_8001bca0 mit Registry-Basis 8).
 *   2. Die Texturen der fuenf Banken 0x10..0x14 (GLAS1090_10..14.TIM = room1090 esp10..14.tim, 4 bpp,
 *      je 256 Texel breit, eine CLUT-Zeile) uebereinander in EINEN Texturplatz (RE15_GLAS_TIM_SLOT):
 *      Bank 0x10 v 0, 0x11 v 32, 0x12 v 64, 0x13 v 128, 0x14 v 168; CLUT-Zeile k = Bank - 0x10.
 *      Der RE2-Lader legt TPage/CLUT der Raum-ESP auf seinen VRAM-Platz um (Bankkopf 0x7840 / TPage 0,
 *      Datei-TIM (0,0)/(0,480)); der Port bildet stattdessen Bank -> Streifen ab (PORT-ZUORDNUNG).
 *   3. Zeichnen: dieselben Quads wie re2fx_pc_draw (re2fx_quads = FUN_80077924/FUN_80077ed0), nur die
 *      Plaetze der Raum-Banken. Mischmodus wie dort: Code 0x2E (Status 0x1000 von 0xB003) + TPage-OR
 *      0x20 -> ABR 1 = B+F.
 *   4. Knall: re15_fenster1120_se_hook = re15_audio_re2_glas_se (audio_pc.c, Raumbank-Satz 0x21).
 *   5. Das beschaedigte Fenster: Pixel-Operationen aus gen/fenster1120_schaden.inc in den
 *      Software-Framebuffer direkt nach dem Hintergrund (Ebene 1, wie panel_lampen_pc.c) — unter Leon.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re2_fx.h"
#include "re15_tim.h"
#include "re15_fenster1120.h"
#include "asset_root_pc.h"        /* re15_pc_read_re2 */
#include "glas1120_pc.h"
#include "re15_engine.h"          /* SCREEN_XRES / SCREEN_YRES */

extern void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
extern int  re15_render_pc_dbg_slot_loaded(int slot);
extern void re15_render_pc_bind_tim_slot(int slot);
extern void re15_render_pc_set_tri_alpha(int a);
extern void re15_render_pc_set_tri_blend(int m);
extern void re15_render_textured_tri(int x0, int y0, int u0, int v0,
                                     int x1, int y1, int u1, int v1,
                                     int x2, int y2, int u2, int v2,
                                     int tpage, int clut, int z,
                                     uint8_t r, uint8_t g, uint8_t b);
extern uint32_t *re15_pc_framebuffer(void);   /* render_pc.c: RGBA8888, R in Bit 24..31 */

#define GLAS_BANKEN   5
#define GLAS_BREITE   256
#define GLAS_HOEHE    200
#define GLAS_CLUT_Y   481                     /* Bankkopf-CLUT 0x7840 -> Zeile 0x7840 >> 6 = 481 */
static const int k_vbasis[GLAS_BANKEN] = { 0, 32, 64, 128, 168 };
static const int k_zeilen[GLAS_BANKEN] = { 32, 32, 64, 40, 32 };   /* TIM-Kopf: Hoehe je Datei */

static uint8_t  *s_esp = NULL;                /* lebt bis Prozessende (re2fx haelt den Zeiger) */
static int       s_esp_sz = 0;
static int       s_esp_rc = -9;
static uint8_t   s_pix[GLAS_BREITE / 2 * GLAS_HOEHE];
static uint16_t  s_clut[GLAS_BANKEN * 16];
static int       s_tex_ok = 0;
static int       s_letzte = 0;

int re15_pc_glas1120_esp_rc(void)  { return s_esp_rc; }
int re15_pc_glas1120_tex_ok(void)  { return s_tex_ok; }
int re15_pc_glas1120_letzte_quads(void) { return s_letzte; }

static uint32_t le32(const uint8_t *b) { return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24); }
static uint16_t le16(const uint8_t *b) { return (uint16_t)(b[0] | (b[1] << 8)); }

static int tim_streifen(int k, const uint8_t *tim, int sz)
{
    if (!tim || sz < 20 || le32(tim) != 0x10u || (le32(tim + 4) & 0xFu) != 0x8u) return -1;
    uint32_t csz = le32(tim + 8);
    if (le16(tim + 16) != 16 || le16(tim + 18) != 1) return -2;          /* CLUT 16 x 1 */
    for (int i = 0; i < 16; i++) s_clut[k * 16 + i] = le16(tim + 20 + i * 2);
    const uint8_t *img = tim + 8 + csz;
    if ((int)(img - tim) + 12 > sz) return -3;
    int hw = le16(img + 8), h = le16(img + 10);
    if (hw != GLAS_BREITE / 4 || h != k_zeilen[k]) return -4;
    if ((int)(img - tim) + 12 + hw * 2 * h > sz) return -5;
    for (int y = 0; y < h; y++)
        memcpy(s_pix + (size_t)(k_vbasis[k] + y) * (GLAS_BREITE / 2), img + 12 + (size_t)y * hw * 2, (size_t)hw * 2);
    return 0;
}

extern void re15_audio_re2_glas_se(int satz);

void re15_pc_glas1120_init(void)
{
    if (!s_esp) s_esp = re15_pc_read_re2("GLAS1090.ESP", &s_esp_sz);
    s_esp_rc = s_esp ? re2fx_register_raum(s_esp, (size_t)s_esp_sz) : -9;
    memset(s_pix, 0, sizeof s_pix);
    int fehler = 0;
    for (int k = 0; k < GLAS_BANKEN; k++) {
        char name[32];
        snprintf(name, sizeof name, "GLAS1090_%02X.TIM", 0x10 + k);
        int sz = 0;
        uint8_t *t = re15_pc_read_re2(name, &sz);
        if (tim_streifen(k, t, sz) != 0) fehler++;
        free(t);
    }
    if (!fehler) {
        re15_tim_t t;
        memset(&t, 0, sizeof t);
        t.bpp = 4; t.has_clut = 1;
        t.clut_x = 0; t.clut_y = GLAS_CLUT_Y; t.clut_entries = GLAS_BANKEN * 16; t.clut = s_clut;
        t.data_x = 0; t.data_y = 0; t.width = GLAS_BREITE; t.height = GLAS_HOEHE;
        t.pixels = (const uint16_t *)(const void *)s_pix;
        re15_render_pc_upload_tim_slot(&t, RE15_GLAS_TIM_SLOT);
        s_tex_ok = re15_render_pc_dbg_slot_loaded(RE15_GLAS_TIM_SLOT);
    }
    re15_fenster1120_se_hook = re15_audio_re2_glas_se;
    fprintf(stderr, "[glas1120] GLAS1090.ESP %d B -> re2fx_register_raum rc=%d, Texturen %s (Slot %d)\n",
            s_esp_sz, s_esp_rc, s_tex_ok ? "ok" : "FEHLEN", RE15_GLAS_TIM_SLOT);
}

void re15_pc_glas1120_draw(const re15_camera_view_t *cam, int cx, int cy, int camf,
                           int has_region, const int16_t rxs[4], const int16_t rzs[4])
{
    static re2fx_quad_t q[128];
    s_letzte = 0;
    if (!cam || !s_tex_ok || !re15_render_pc_dbg_slot_loaded(RE15_GLAS_TIM_SLOT)) return;
    int n = re2fx_quads(cam, cx, cy, camf, has_region, rxs, rzs, q, (int)(sizeof q / sizeof q[0]));
    if (n <= 0) return;
    int gebunden = 0;
    for (int i = n - 1; i >= 0; i--) {                 /* rueckwaerts wie re2fx_pc_draw (OT vorn) */
        const re2fx_quad_t *e = &q[i];
        const uint8_t *pl = re2fx_platz(e->platz);
        if (!pl) continue;
        int k = (int)pl[0x1C] - 0x10;                  /* +0x1C Bank (`sw v0,28(t0)` @0x8001cc98) */
        if (k < 0 || k >= GLAS_BANKEN) continue;
        if (!gebunden) { re15_render_pc_bind_tim_slot(RE15_GLAS_TIM_SLOT); gebunden = 1; }
        int abe = (e->code & 2) != 0, abr = (e->tpage >> 5) & 3;
        if (abe) {
            switch (abr) {
            case 1:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(255); break;   /* B + F */
            case 2:  re15_render_pc_set_tri_blend(2); re15_render_pc_set_tri_alpha(255); break;   /* B - F */
            case 3:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(64);  break;   /* B + F/4 */
            default: re15_render_pc_set_tri_alpha(128); break;                                    /* 0.5B + 0.5F */
            }
        }
        int v0 = k_vbasis[k] + e->v0, v1 = k_vbasis[k] + e->v1;
        int clut = (GLAS_CLUT_Y + k) << 6;
        re15_render_textured_tri(e->x0, e->y0, e->u0, v0,  e->x1, e->y0, e->u1, v0,
                                 e->x0, e->y1, e->u0, v1,  e->tpage, clut, e->vz, 255, 255, 255);
        re15_render_textured_tri(e->x1, e->y0, e->u1, v0,  e->x1, e->y1, e->u1, v1,
                                 e->x0, e->y1, e->u0, v1,  e->tpage, clut, e->vz, 255, 255, 255);
        if (abe) { re15_render_pc_set_tri_blend(0); re15_render_pc_set_tri_alpha(255); }
        s_letzte++;
    }
}

/* ---- das beschaedigte Fenster ------------------------------------------------------------------ */
typedef struct { int16_t x, y; uint8_t art, a, r, g, b; } schaden_op_t;
static const schaden_op_t k_schaden[] = {
#include "../../../engine/src/gen/fenster1120_schaden.inc"
};
int re15_pc_glas1120_schaden_ops(void) { return (int)(sizeof k_schaden / sizeof k_schaden[0]); }

void re15_fenster1120_pc_zeichnen(int cut)
{
    if (!re15_fenster1120_schaden_sichtbar(cut)) return;
    uint32_t *fb = re15_pc_framebuffer();
    if (!fb) return;
    for (size_t i = 0; i < sizeof k_schaden / sizeof k_schaden[0]; i++) {
        const schaden_op_t *o = &k_schaden[i];
        if (o->x < 0 || o->x >= SCREEN_XRES || o->y < 0 || o->y >= SCREEN_YRES) continue;
        uint32_t *px = &fb[o->y * SCREEN_XRES + o->x];
        int r = (int)((*px >> 24) & 0xffu), g = (int)((*px >> 16) & 0xffu), b = (int)((*px >> 8) & 0xffu);
        if (o->art == 0) { r = r * o->a / 256; g = g * o->a / 256; b = b * o->a / 256; }
        else {
            r += ((int)o->r - r) * o->a / 256;
            g += ((int)o->g - g) * o->a / 256;
            b += ((int)o->b - b) * o->a / 256;
        }
        *px = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xffu;
    }
}
