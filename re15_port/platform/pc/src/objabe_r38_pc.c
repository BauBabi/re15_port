/*
 * RE1.5 Rebuilt — Runde 38 Spur W "rolltor20a0", Nachbesserung 1 (PC): halbtransparente Raum-Objekte.
 *
 * Original (Adressen in include/re15_objabe_r38.h): ein Objekt mit Attribut-Bit 0x2 zeichnet jedes Polygon mit dem
 * Code 0x36/0x3E; die GPU mischt dann JE TEXEL — STP 0 deckend, STP 1 nach ABR (Texpage-Bits 5-6 = MD1-Seite ODER
 * (Attribut & 0xC) << 3), 0000h gar nicht. SDL schaltet die Mischung nur je Dreieck um. Darum zwei Durchgaenge mit zwei
 * Teil-Texturen derselben TIM (re15_objabe_r38_teilen): erst die STP-0-Texel deckend, dann die STP-1-Texel gemischt.
 * Beide Teile sind texelweise disjunkt; zusammen ergeben sie genau die Regel der GPU.
 *   ABR 0  B/2 + F/2   SDL BLEND, Eckpunkt-Alpha 128 (= 0,502)
 *   ABR 1  B + F       SDL ADD (dst + src x Alpha), Alpha FF
 *   ABR 2  B - F       render_pc.c s_tri_sub_blend (ONE, ONE, REV_SUBTRACT); Nicht-STP-Texel der Teil-Textur sind 0
 *   ABR 3  B + F/4     SDL ADD, Eckpunkt-Alpha 64 (= 0,251)
 * Die Zeichen-Queue (render_pc.c) traegt den Modus je Dreieck (RE15_OBJABE_BLEND_*); ihr Flush fragt hier nach der
 * Teil-Textur (re15_objabe_r38_pc_flush_textur).
 */
#include <stdlib.h>
#include <SDL.h>

#include "re15_objabe_r38.h"
#include "re15_engine.h"     /* re15_render_textured_tri_lit */

extern SDL_Renderer *re15_render_pc_renderer(void);
extern void re15_render_pc_set_tri_alpha(int a);
extern void re15_render_pc_set_tri_blend(int m);

#define SLOTS 64             /* > RE15_TIM_SLOT_MAX (render_pc.c, 56) */

static SDL_Texture *s_deckend[SLOTS], *s_halb[SLOTS];
static uint8_t      s_geteilt[SLOTS];   /* 1 = Teil-Texturen gebaut (TIM mit STP-1-Texeln) */

static void slot_leeren(int slot)
{
    if (s_deckend[slot]) SDL_DestroyTexture(s_deckend[slot]);
    if (s_halb[slot])    SDL_DestroyTexture(s_halb[slot]);
    s_deckend[slot] = s_halb[slot] = NULL;
    s_geteilt[slot] = 0;
}

void re15_objabe_r38_pc_raum_neu(void)
{
    for (int s = 0; s < SLOTS; s++) slot_leeren(s);
}

static SDL_Texture *textur(SDL_Renderer *r, const uint32_t *px, int w, int h)
{
    SDL_Texture *t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
    if (t) {
        SDL_UpdateTexture(t, NULL, px, w * 4);
        SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
    }
    return t;
}

void re15_objabe_r38_pc_textur(const re15_tim_t *tim, int slot)
{
    if (slot < 0 || slot >= SLOTS) return;
    slot_leeren(slot);
    SDL_Renderer *r = re15_render_pc_renderer();
    int w, h;
    if (!r || !tim || re15_objabe_r38_textur_mass(tim, &w, &h) < 1 || w <= 0 || h <= 0) return;
    uint32_t *d = (uint32_t *)malloc((size_t)w * (size_t)h * 4u);
    uint32_t *b = (uint32_t *)malloc((size_t)w * (size_t)h * 4u);
    if (d && b && re15_objabe_r38_teilen(tim, d, b) > 0) {
        s_deckend[slot] = textur(r, d, w, h);
        s_halb[slot]    = textur(r, b, w, h);
        s_geteilt[slot] = (s_deckend[slot] && s_halb[slot]) ? 1 : 0;
    }
    free(d);
    free(b);
}

void re15_objabe_r38_pc_dreieck(uint16_t attr, uint16_t page,
                                int x0, int y0, int u0, int v0,
                                int x1, int y1, int u1, int v1,
                                int x2, int y2, int u2, int v2,
                                int tpage, int clut, int z,
                                uint8_t r0, uint8_t g0, uint8_t b0,
                                uint8_t r1, uint8_t g1, uint8_t b1,
                                uint8_t r2, uint8_t g2, uint8_t b2)
{
    if (!re15_objabe_r38_abe(attr)) {                    /* Code 0x34/0x3C: deckend, wie bisher */
        re15_render_textured_tri_lit(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, tpage, clut, z,
                                     r0, g0, b0, r1, g1, b1, r2, g2, b2);
        return;
    }
    const int abr = re15_objabe_r38_abr(attr, page);
    /* 1. Durchgang: STP-0-Texel deckend. */
    re15_render_pc_set_tri_blend(RE15_OBJABE_BLEND_DECKEND);
    re15_render_textured_tri_lit(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, tpage, clut, z,
                                 r0, g0, b0, r1, g1, b1, r2, g2, b2);
    /* 2. Durchgang: STP-1-Texel gemischt (psx-spx: 0 = B/2+F/2, 1 = B+F, 2 = B-F, 3 = B+F/4). */
    re15_render_pc_set_tri_blend(RE15_OBJABE_BLEND_HALB(abr));
    re15_render_pc_set_tri_alpha(abr == 0 ? 128 : (abr == 3 ? 64 : 255));
    re15_render_textured_tri_lit(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, tpage, clut, z,
                                 r0, g0, b0, r1, g1, b1, r2, g2, b2);
    re15_render_pc_set_tri_alpha(255);
    re15_render_pc_set_tri_blend(0);
}

SDL_Texture *re15_objabe_r38_pc_flush_textur(int slot, int blend, SDL_Texture *normal, int *sdl_modus)
{
    const int geteilt = (slot >= 0 && slot < SLOTS) ? s_geteilt[slot] : 0;
    if (blend == RE15_OBJABE_BLEND_DECKEND) {
        *sdl_modus = 0;
        return geteilt ? s_deckend[slot] : normal;       /* keine STP-1-Texel: alles deckend = die normale Textur */
    }
    const int abr = blend - RE15_OBJABE_BLEND_HALB(0);
    *sdl_modus = (abr == 1 || abr == 3) ? 1 : (abr == 2 ? 2 : 0);
    return geteilt ? s_halb[slot] : NULL;                /* keine STP-1-Texel: nichts zu mischen */
}
