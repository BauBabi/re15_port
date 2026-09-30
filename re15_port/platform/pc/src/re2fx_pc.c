/*
 * re2fx_pc.c — Runde 34 Spur D (BAUPLAN §3.3 C6, bau_d.md §4): PC-Zeichner der RE2-FX-Maschine.
 *
 * Quelle der Quads: re2fx_quads() (engine/src/re2_fx.c) = FUN_80077924/FUN_80077ed0. Hier nur die
 * PC-Umsetzung: RE2 TEX.TIM als ein Textur-Slot und je Quad zwei Dreiecke ueber die Zeichen-API, die
 * main.c fuer die RE1.5-ESP-Effekte benutzt (re15_render_pc_* aus render_pc.c, pc_draw_effects
 * main.c:236-466) — main.c/render_pc.c bleiben unberuehrt.
 *
 * VRAM-Lage (alle RE2 PSX.EXE):
 *   0x800CFBF0 := 28 (`addiu v0,zero,28 / sh v0,-1040(at)` @0x8002b8cc-d4); Lader FUN_80076a40:
 *   x = 28*64 - 1024 = 768 (`lbu v1,-1040 / sll v0,v1,6 / addiu v0,v0,-1024` @0x80076a64-80),
 *   y = 256 (`sltiu / xori v0,v0,0x1 / sll v0,v0,8` @0x80076a9c-a4), CLUT y = 480 (`addiu v0,v0,480`
 *   @0x80076b08). TEX.TIM-Kopf: 4 bpp mit CLUT, CLUT (256,480) 32 x 19, Bild 256 hw x 256.
 *   TPage 0x1E = VRAM-x 896 = Bild-hw 128..191, 0x1F = x 960 = hw 192..255 (TPage-Wort Bits 0-3 = x/64,
 *   Bit 4 = y/256). Alle Aufschlag-Baenke haben CLUT-x 272 (Bank 3/4 0x7811, Bank 5 0x7911: x-Feld 0x11).
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "re2fx_pc.h"
#include "re2_fx.h"
#include "re15_tim.h"

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

#define RE2FX_CLUT_ZEILEN 19     /* CLUT-Block 32 x 19 (TEX.TIM-Kopf `20 00 13 00`) */
/* Nachbesserung N5 (Gegenpruefung M8): hochgeladen werden nur die CLUT-Zeilen 480..484. Der PC-Slot stapelt
 * je CLUT-Zeile eine Kopie der Seite (render_pc.c upload: Hoehe = 256 * Zeilen); 19 Zeilen = 512 x 4864 Texel
 * lagen ueber der 4096er Texturgrenze aelterer GLES-Geraete (Android baut diese Datei per GLOB mit). Belegt:
 * CLUT eines Platzes = Bankkopf + (Sub >> 3) * 0x40 (`srl v0,t5,3 / sll v0,v0,6 / addu v1,v1,v0 /
 * sh v1,50(t0)` @0x8001ccf4-d10); Bankkoepfe aus CORE00.ESP: Bank 3/4 0x7811 (Zeile 480), Bank 5 0x7911
 * (Zeile 484); die Aufschlag-Kinder haben Sub 0x0C..0x1F (Bank 3/4: Zeile 481..483) bzw. 4/5 (Bank 5: 484).
 * Quads mit einer anderen Zeile laesst re2fx_pc_draw aus (render_pc.c faellt sonst still auf Zeile 0 zurueck,
 * `if (clut_idx < 0 || clut_idx >= s_tim_n_cluts) clut_idx = 0`). Sonde: probe_r34_re2fx_pc. */
#define RE2FX_CLUT_ERSTE  480
#define RE2FX_CLUT_ANZAHL 5

static uint16_t s_clut[RE2FX_CLUT_ANZAHL * 16];
static uint8_t  s_pix[256 * 256];          /* 512 Texel x 256 Zeilen, 4 bpp = 256 Byte je Zeile */
static int      s_tex_ok;

static struct {
    int                gueltig;
    re15_camera_view_t cam;
    int                cx, cy, camf, has_region;
    int16_t            rxs[4], rzs[4];
} s_ansicht;

static int s_letzte_quads;
int re2fx_pc_letzte_quads(void) { return s_letzte_quads; }

static uint32_t le32(const uint8_t *b) { return (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24); }
static uint16_t le16(const uint8_t *b) { return (uint16_t)(b[0] | (b[1] << 8)); }

int re2fx_pc_lade_tex(const uint8_t *tim, size_t size)
{
    s_tex_ok = 0;
    if (!tim || size < 20) return -1;
    if (le32(tim) != 0x10u || (le32(tim + 4) & 0xFu) != 0x8u) return -2;          /* TIM, 4 bpp + CLUT */
    uint32_t csz = le32(tim + 8);
    uint16_t cx = le16(tim + 12), cy = le16(tim + 14), cw = le16(tim + 16), ch = le16(tim + 18);
    (void)cy;                                                                       /* y := 480 durch den Lader */
    if (cx != 256 || cw != 32 || ch != RE2FX_CLUT_ZEILEN) return -3;               /* x 256, 32 x 19 */
    if ((size_t)8 + csz + 12 > size) return -4;
    const uint8_t *img = tim + 8 + csz;
    /* Die Bildlage der DATEI ist (0,0); der Lader FUN_80076a40 ersetzt sie: prect.x = 28*64 - 1024 = 768
     * (`lbu v1,-1040(v1)` 0x800CFBF0 = 28 @0x80076a64, `sll v0,v1,6` / `addiu v0,v0,-1024` @0x80076a6c-80,
     * `sh v0,0(a1)` @0x80076a90), prect.y = 256 (`sltiu / xori / sll 8 / sh v0,2(v1)` @0x80076a9c-a8);
     * CLUT: crect.y = 0x800CFBF1 (0, Halbwort-Store 28 @0x8002b8d4) + 480 (@0x80076b00-0c), crect.x aus der
     * Datei (256). Geprueft werden deshalb nur die Masse. */
    uint16_t iw = le16(img + 8), ih = le16(img + 10);
    if (iw != 256 || ih != 256) return -5;                                          /* 256 hw x 256 */
    if ((size_t)(img - tim) + 12u + 256u * 256u * 2u > size) return -6;
    /* CLUT-Spalte x 272 = Eintraege 16..31 jeder 32er-Zeile; Zeilen 480..484 = Datei-Zeilen 0..4. */
    for (int r = 0; r < RE2FX_CLUT_ANZAHL; r++)
        for (int k = 0; k < 16; k++)
            s_clut[r * 16 + k] = le16(tim + 20 + (size_t)((RE2FX_CLUT_ERSTE - 480 + r) * 32 + 16 + k) * 2);
    /* Seiten 0x1E + 0x1F = Bild-hw 128..255 = Bytes 256..511 jeder 512-Byte-Zeile. */
    for (int y = 0; y < 256; y++)
        memcpy(s_pix + (size_t)y * 256, img + 12 + (size_t)y * 512 + 256, 256);
    re15_tim_t t;
    memset(&t, 0, sizeof t);
    t.bpp = 4; t.has_clut = 1;
    t.clut_x = 272; t.clut_y = RE2FX_CLUT_ERSTE; t.clut_entries = RE2FX_CLUT_ANZAHL * 16; t.clut = s_clut;
    t.data_x = 896; t.data_y = 256; t.width = 512; t.height = 256;
    t.pixels = (const uint16_t *)(const void *)s_pix;
    re15_render_pc_upload_tim_slot(&t, RE2FX_TIM_SLOT);
    s_tex_ok = re15_render_pc_dbg_slot_loaded(RE2FX_TIM_SLOT);
    if (!s_tex_ok)
        fprintf(stderr, "[re2fx] TEX.TIM: Slot %d nicht verfuegbar (RE15_TIM_SLOT_MAX in render_pc.c) — "
                        "RE2-FX werden nicht gezeichnet\n", RE2FX_TIM_SLOT);
    return s_tex_ok ? 0 : -7;
}

void re2fx_pc_set_ansicht(const re15_camera_view_t *cam, int cx, int cy, int camf,
                          int has_region, const int16_t rxs[4], const int16_t rzs[4])
{
    if (!cam) { s_ansicht.gueltig = 0; return; }
    s_ansicht.cam = *cam;
    s_ansicht.cx = cx; s_ansicht.cy = cy; s_ansicht.camf = camf; s_ansicht.has_region = has_region;
    if (has_region && rxs && rzs) { memcpy(s_ansicht.rxs, rxs, sizeof s_ansicht.rxs); memcpy(s_ansicht.rzs, rzs, sizeof s_ansicht.rzs); }
    else s_ansicht.has_region = 0;
    s_ansicht.gueltig = 1;
}

/* Paketpuffer je Bild: 0x3C00 Byte = 384 POLY_FT4 zu 40 Byte (Anfang 0x800C4418 + Puffer*0x3C00
 * FUN_800778f8, Ende 0x800C8018 + Puffer*0x3C00 `addiu a2,a2,-15360` @0x8007796c / @0x800779b0). Passt ein
 * Sprite nicht mehr hinein, LOESCHT das Original den Platz (`sh zero,24(s0)` @0x80077e40-54) — im Port
 * nicht nachgebaut: die Aufschlaege erzeugen hoechstens ~130 Quads je Bild (bau_d.md §4). */
#define RE2FX_PAKETE_JE_BILD 384

void re2fx_pc_draw(void)
{
    static re2fx_quad_t q[RE2FX_PAKETE_JE_BILD];
    s_letzte_quads = 0;
    if (!s_ansicht.gueltig || !s_tex_ok || !re15_render_pc_dbg_slot_loaded(RE2FX_TIM_SLOT)) return;
    int n = re2fx_quads(&s_ansicht.cam, s_ansicht.cx, s_ansicht.cy, s_ansicht.camf,
                        s_ansicht.has_region, s_ansicht.rxs, s_ansicht.rzs, q, (int)(sizeof q / sizeof q[0]));
    if (n <= 0) return;
    re15_render_pc_bind_tim_slot(RE2FX_TIM_SLOT);
    /* Reihenfolge: FUN_80077ed0 haengt jedes Paket VORN an seinen OT-Bucket (`lw v0,0(t3)` / `sw v0,0(t0)` /
     * `sw a0,0(t3)` @0x80077f94-fac) — innerhalb eines Buckets zeichnet die GPU die zuletzt eingereihten
     * zuerst. Der Port-Sortierer ist stabil (render_pc.c:963-971), also die Quads RUECKWAERTS einreihen:
     * gleich tiefe Sprites (z.B. die drei Saeure-Kinder an Q) liegen dann wie im Original uebereinander. */
    for (int i = n - 1; i >= 0; i--) {
        const re2fx_quad_t *e = &q[i];
        unsigned seite = e->tpage & 0x1Fu;
        if (seite != 0x1E && seite != 0x1F) continue;          /* nur die TEX.TIM-Seiten (s. Kopf) */
        if ((e->clut & 0x3Fu) != 0x11u) continue;              /* nur CLUT-Spalte x 272 geladen */
        const int zeile = (e->clut >> 6) & 0x1FF;
        if (zeile < RE2FX_CLUT_ERSTE || zeile >= RE2FX_CLUT_ERSTE + RE2FX_CLUT_ANZAHL) continue;   /* nur Zeilen 480..484 (N5) */
        int uo = (seite == 0x1F) ? 256 : 0;
        /* Code 0x2E = halbtransparent (Status 0x1000 @0x80077a44-50); Mischmodus = TPage-Bits 5-6
         * (FUN_80077ed0 `or v1,v1,s0` mit s0 = +0x2A << 16 @0x80078130-34 -> POLY_FT4 Wort 5). Die
         * ABR-Abbildung ist dieselbe wie in pc_draw_effects (main.c:436-448). */
        int abe = (e->code & 2) != 0, abr = (e->tpage >> 5) & 3;
        if (abe) {
            switch (abr) {
            case 1:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(255); break;   /* B + F */
            case 2:  re15_render_pc_set_tri_blend(2); re15_render_pc_set_tri_alpha(255); break;   /* B - F */
            case 3:  re15_render_pc_set_tri_blend(1); re15_render_pc_set_tri_alpha(64);  break;   /* B + F/4 */
            default: re15_render_pc_set_tri_alpha(128); break;                                    /* 0.5B + 0.5F */
            }
        }
        /* Paketfarbe 0x808080 (FUN_800783b4 @0x800783cc-d0) = Textur x 1.0; die unbeleuchtete
         * Queue-Funktion reicht die Farbe unveraendert an SDL (Textur x Farbe/255), also 255 —
         * dieselbe Umrechnung 0x80 -> 0xFF wie psx_prim_to_sdl_vert (render_pc.c:2536-2539). */
        int u0 = e->u0 + uo, u1 = e->u1 + uo;
        int z = e->vz;
        re15_render_textured_tri(e->x0, e->y0, u0, e->v0,  e->x1, e->y0, u1, e->v0,
                                 e->x0, e->y1, u0, e->v1,  e->tpage, e->clut, z, 255, 255, 255);
        re15_render_textured_tri(e->x1, e->y0, u1, e->v0,  e->x1, e->y1, u1, e->v1,
                                 e->x0, e->y1, u0, e->v1,  e->tpage, e->clut, z, 255, 255, 255);
        if (abe) { re15_render_pc_set_tri_blend(0); re15_render_pc_set_tri_alpha(255); }
        s_letzte_quads++;
    }
}
