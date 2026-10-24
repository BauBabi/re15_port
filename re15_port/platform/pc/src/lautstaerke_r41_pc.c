/* lautstaerke_r41_pc.c — Runde 41 Spur D: die VOLUME-Zeile im SOUND-Schirm der OPTIONS (platform/pc/main.c
 * pc_run_config / pc_config_draw_overlay) und ihre Eingabe. Belege: include/re15_lautstaerke_r41.h,
 * analysis/befunde_runde41/D_kleinkram_a.md §1.
 *
 * Lage (PORT-WAHL): dieselbe CONFIG.TIM-Kachel wie die zwei Original-Kacheln (y 0x33/0x46, FUN_8002f0d4
 * @0x8002f128 / @0x8002f17c; uv(0,232) 88x18 bei x 0x74 (116), @0x8002f11c-30), gezeichnet im freien Streifen
 * unter dem byte-true Kasten (Kastenende y 220 -> Kachel y 221..238, Vorbild CFG_AI_Y 225 in main.c). Die
 * Raster-Lage y 89 wurde am Shot gemessen und verworfen: die Original-Beschriftungskaesten DAT_80073d2c[2] =
 * (46,91) und [8] = (203,91) wuerden von der Kachel 116..204 angeschnitten ("Forwar").
 * Text "VOLUME" ab x 119 wie "STEREO" (Prim-Scan der Options-Spur, main.c), Zeile y 223 (Kachel + 2 wie STEREO
 * 51 -> 53); Zahl rechtsbuendig bis x 201
 * (Kachelende 204 - 3 px Rand wie der linke Rand 116 -> 119). Breiten aus der Vorschubtabelle der Spielschrift
 * (BIN/DEBUG.BIN@0x4416, re15_render_pc_game_text_width): "VOLUME" 52 px, "100" 23 px, "0" 8 px. */
#include "re15_lautstaerke_r41.h"

#include <stdio.h>
#include <stdint.h>

#include "re15_tim.h"
#include "re15_player.h"   /* RE15_PAD_BIT_LEFT / RIGHT */

extern void re15_render_pc_config_tile(const re15_tim_t *t, int su, int sv, int w, int h, int dx, int dy);
extern int  re15_render_pc_config_text(int x, int y, const unsigned char *codes, int len, int attr);
extern int  re15_render_pc_game_text_width(const char *str);
extern void re15_audio_set_mono(int mono);

#define LAUT_PC_KACHEL_X   116   /* @0x8002f124 `ori t2,zero,0x74` */
#define LAUT_PC_KACHEL_Y   221   /* freier Streifen unter dem byte-true Kasten (Kastenende y 220, Vorbild CFG_AI_Y 225 in main.c); y 89 im Kachel-Raster verdeckte den Forward-Kasten (DAT_80073d2c), gemessen am Shot */
#define LAUT_PC_KACHEL_W   88    /* @0x8002f0e0 `ori t4,zero,0x58` */
#define LAUT_PC_KACHEL_H   18    /* @0x8002f0e4 `ori t3,zero,0x12` */
#define LAUT_PC_KACHEL_U   0     /* @0x8002f12c `sb zero,0xc(s1)` */
#define LAUT_PC_KACHEL_V   232   /* @0x8002f0ec `ori t1,zero,0xe8` */
#define LAUT_PC_TEXT_X     119   /* wie STEREO */
#define LAUT_PC_TEXT_Y     223   /* Kachel + 2 wie STEREO (51 -> 53) */
#define LAUT_PC_ZAHL_RECHTS 201  /* Kachelende 204 - 3 px */

/* ASCII -> Glyphcode der Spielschrift (TEX.TIM): A-Z ab 0x1D, 0-9 ab 0x0C (main.c pc_font_code, shots/texfont_grid.png). */
static int glyph(char c)
{
    if (c >= 'A' && c <= 'Z') return 0x1D + (c - 'A');
    if (c >= '0' && c <= '9') return 0x0C + (c - '0');
    return 0x00;
}

static int codes_von(const char *s, unsigned char *out, int max)
{
    int n = 0;
    for (const char *p = s; *p && n < max; p++) out[n++] = (unsigned char)glyph(*p);
    return n;
}

/* Programmstart: Datei lesen und den gespeicherten Sound-Modus anwenden (main.c, vor dem ersten Ton). */
void re15_lautstaerke_r41_pc_start(void)
{
    re15_lautstaerke_r41_laden();
    re15_audio_set_mono(!re15_lautstaerke_r41_stereo());
}

/* Eintritt in den SOUND-Schirm: Arbeitskopie := gespeichert (@0x8002f244-4c); liefert den Stereo-Stand fuer
 * s_config_sound. */
int re15_lautstaerke_r41_pc_eintritt(void)
{
    return re15_lautstaerke_r41_eintritt();
}

void re15_lautstaerke_r41_pc_zeichnen(const re15_tim_t *tim)
{
    unsigned char b[8];
    re15_render_pc_config_tile(tim, LAUT_PC_KACHEL_U, LAUT_PC_KACHEL_V, LAUT_PC_KACHEL_W, LAUT_PC_KACHEL_H,
                               LAUT_PC_KACHEL_X, LAUT_PC_KACHEL_Y);
    int n = codes_von("VOLUME", b, (int)sizeof b);
    re15_render_pc_config_text(LAUT_PC_TEXT_X, LAUT_PC_TEXT_Y, b, n, 0);
    char z[8];
    snprintf(z, sizeof z, "%d", re15_lautstaerke_r41_prozent());
    int w = re15_render_pc_game_text_width(z);
    n = codes_von(z, b, (int)sizeof b);
    re15_render_pc_config_text(LAUT_PC_ZAHL_RECHTS - w, LAUT_PC_TEXT_Y, b, n, 0);
}

/* LEFT = leiser, RIGHT = lauter (im Original-Treiber ungenutzte Bits 0x8000 / 0x2000). Rueckgabe 1 = geaendert. */
int re15_lautstaerke_r41_pc_eingabe(uint16_t pad_pressed)
{
    int vor = re15_lautstaerke_r41_prozent();
    if (pad_pressed & RE15_PAD_BIT_LEFT)  re15_lautstaerke_r41_schritt(-1);
    if (pad_pressed & RE15_PAD_BIT_RIGHT) re15_lautstaerke_r41_schritt(+1);
    return re15_lautstaerke_r41_prozent() != vor;
}
