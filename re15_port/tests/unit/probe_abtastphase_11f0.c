/* probe_abtastphase_11f0.c — RIEGEL fuer die Abtastphase der texturierten Dreiecke.
 *
 * WAS DIESE SONDE PRUEFT — und wogegen.
 * Der Nutzer meldet zweimal: "der Cursor hat einen schraeg versetzten Schatten"
 * (ROOM11F0, Cut 10). Die GRUNDWAHRHEIT ist ein echter PSX-Lauf (DuckStation,
 * analysis/grundwahrheit/psx_room11f0_cut10.png + .sav). Dort gilt, aus dem
 * Framebuffer abgelesen (Prim-Modulation x1.111):
 *     senkrechter Kreuzarm  HELL  auf Bildschirmspalte x=160, SCHATTEN auf x=161
 *     waagerechter Kreuzarm HELL  auf Bildschirmzeile  y=118, SCHATTEN auf y=119
 * Der Schatten selbst ist ORIGINAL: er ist in die Cursor-Textur gebacken
 * (ROOM11F0.RDT @Datei 0x018DAC, +5u/+3v). Falsch war bei uns, dass das Kreuz
 * UEBERHAUPT NICHT HELL gezeichnet wurde.
 *
 * Die Sonde rechnet genau diese vier Bildschirmzellen nach:
 *   - die Quad-Geometrie (Scheitel x159->x162 mit u55->u73, y117->y120 mit v34->v46)
 *     stammt aus RE15_TRILOG eines echten Laufs und steht in include/re15_abtastphase.h,
 *   - der Abtastpunkt folgt der SDL-Regel "Pixelmitte": p + 0.5 - RE15_UV_ABTASTPHASE,
 *   - welche FARBE dort liegt, wird NICHT behauptet, sondern aus dem echten TIM in
 *     ROOM11F0.RDT gelesen.
 *
 * ALT-STAND (nachgefahren, Zahlen im Paketbericht):
 *   RE15_UV_ABTASTPHASE = -0.375f (v0.8.13)  -> ROT
 *   RE15_UV_ABTASTPHASE =  0.0f   (davor)     -> ROT
 * Beide treffen an x=160 die SCHATTENBAHN statt der hellen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_abtastphase.h"

static int s_fail = 0;
#define CHECK(name, cond) do { \
    if (cond) { printf("  OK   %s\n", (name)); } \
    else      { printf("  FAIL %s\n", (name)); s_fail++; } } while (0)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = sz;
    return b;
}

/* Das Cursor-TIM @Datei 0x018DAC: magic 0x10, flags 0x09 (8bpp + CLUT),
 * danach CLUT-Block (len,x,y,w,h) und Pixel-Block (len,x,y,words,h). */
static const uint8_t *s_pix;   /* 8bpp, 128 breit */
static int s_w, s_h;

static int tim_open(const uint8_t *rdt, long n)
{
    if (n < (long)(RE15_CUR_TIM_OFF + 32)) return 0;
    const uint8_t *t = rdt + RE15_CUR_TIM_OFF;
    uint32_t magic, flags, clen, plen;
    memcpy(&magic, t + 0, 4); memcpy(&flags, t + 4, 4);
    if (magic != 0x10u || flags != 0x09u) return 0;
    memcpy(&clen, t + 8, 4);
    const uint8_t *p = t + 8 + clen;
    memcpy(&plen, p + 0, 4);
    uint16_t words, h;
    memcpy(&words, p + 8, 2); memcpy(&h, p + 10, 2);
    s_w = (int)words * 2; s_h = (int)h;
    s_pix = p + 12;
    return (s_w == 128 && s_h == 256 && plen > 0);
}

static int texel(int u, int v)
{
    if (u < 0 || u >= s_w || v < 0 || v >= s_h) return -1;
    return s_pix[(long)v * s_w + u];
}

/* Abtastpunkt der SDL-Geometrie fuer Bildschirmpixel p bei Scheitelverschiebung a. */
static double abtast(int p) { return (double)p + 0.5 - (double)RE15_UV_ABTASTPHASE; }

static int lerp_uv(int p, int p0, int p1, int c0, int c1)
{
    double t = (abtast(p) - (double)p0) / (double)(p1 - p0);
    double c = (double)c0 + t * (double)(c1 - c0);
    return (int)c;   /* NEAREST = abschneiden auf den Texel */
}

int main(void)
{
    char rp[600];
    long n = 0;
    snprintf(rp, sizeof rp, "%s/STAGE1/ROOM11F0.RDT", RE15_ASSET_PSX_DIR);
    uint8_t *rdt = slurp(rp, &n);
    if (!rdt) { printf("SKIP: %s fehlt\n", rp); return 77; }
    if (!tim_open(rdt, n)) { printf("FAIL: Cursor-TIM @0x%06X ist nicht 8bpp 128x256\n",
                                    (unsigned)RE15_CUR_TIM_OFF); free(rdt); return 1; }
    printf("[anchor] Cursor-TIM @Datei 0x%06X = 8bpp %dx%d\n",
           (unsigned)RE15_CUR_TIM_OFF, s_w, s_h);

    /* --- A) die Textur selbst: der Schlagschatten ist gebacken (+5u / +3v). --- */
    /* Zeile v=21 traegt nur den senkrechten Kreuzarm: hell, danach der Schatten. */
    int hell_u0 = -1, hell_u1 = -1, sch_u0 = -1, sch_u1 = -1;
    for (int u = 0; u < s_w; u++) {
        int c = texel(u, 25);
        if (c == RE15_CUR_IDX_HELL || c == RE15_CUR_IDX_HELLKERN) {
            if (hell_u0 < 0) hell_u0 = u;
            hell_u1 = u;
        } else if (c == RE15_CUR_IDX_SCHATTEN) {
            if (sch_u0 < 0) sch_u0 = u;
            sch_u1 = u;
        }
    }
    printf("[textur] v=25: hell u %d..%d, Schatten u %d..%d\n", hell_u0, hell_u1, sch_u0, sch_u1);
    CHECK("senkrechter Kreuzarm ist 5 Texel breit", hell_u1 - hell_u0 == 4);
    CHECK("sein Schatten liegt unmittelbar rechts (+5u)", sch_u0 == hell_u1 + 1);

    /* Spalte u=61 traegt den waagerechten Arm: hell, danach der Schatten darunter. */
    int hell_v0 = -1, hell_v1 = -1, sch_v0 = -1;
    for (int v = 36; v < 48; v++) {
        int c = texel(38, v);
        if (c == RE15_CUR_IDX_HELL || c == RE15_CUR_IDX_HELLKERN) {
            if (hell_v0 < 0) hell_v0 = v;
            hell_v1 = v;
        } else if (c == RE15_CUR_IDX_SCHATTEN && sch_v0 < 0 && hell_v1 >= 0) {
            sch_v0 = v;
        }
    }
    printf("[textur] u=38: hell v %d..%d, Schatten ab v %d\n", hell_v0, hell_v1, sch_v0);
    CHECK("waagerechter Kreuzarm ist 3 Texel hoch", hell_v1 - hell_v0 == 2);
    CHECK("sein Schatten liegt unmittelbar darunter (+3v)", sch_v0 == hell_v1 + 1);

    /* --- B) die Abtastung MUSS die PSX-Zellen treffen. --- */
    int u_hell = lerp_uv(RE15_GW_HELL_X, RE15_GW_QUAD_X0, RE15_GW_QUAD_X1,
                         RE15_GW_QUAD_U0, RE15_GW_QUAD_U1);
    int u_dunk = lerp_uv(RE15_GW_SCHATTEN_X, RE15_GW_QUAD_X0, RE15_GW_QUAD_X1,
                         RE15_GW_QUAD_U0, RE15_GW_QUAD_U1);
    int v_hell = lerp_uv(RE15_GW_HELL_Y, RE15_GW_QUAD_Y0, RE15_GW_QUAD_Y1,
                         RE15_GW_QUAD_V0, RE15_GW_QUAD_V1);
    int v_dunk = lerp_uv(RE15_GW_SCHATTEN_Y, RE15_GW_QUAD_Y0, RE15_GW_QUAD_Y1,
                         RE15_GW_QUAD_V0, RE15_GW_QUAD_V1);
    printf("[phase %.4f] x=%d -> u=%d | x=%d -> u=%d | y=%d -> v=%d | y=%d -> v=%d\n",
           (double)RE15_UV_ABTASTPHASE, RE15_GW_HELL_X, u_hell, RE15_GW_SCHATTEN_X, u_dunk,
           RE15_GW_HELL_Y, v_hell, RE15_GW_SCHATTEN_Y, v_dunk);

    int c_hell = texel(u_hell, 25);
    int c_dunk = texel(u_dunk, 25);
    printf("[abtast] Spalte x=%d -> Texel-Index %d | Spalte x=%d -> Texel-Index %d\n",
           RE15_GW_HELL_X, c_hell, RE15_GW_SCHATTEN_X, c_dunk);
    CHECK("PSX x=160: senkrechter Kreuzarm HELL",
          c_hell == RE15_CUR_IDX_HELL || c_hell == RE15_CUR_IDX_HELLKERN);
    CHECK("PSX x=161: dort liegt sein SCHATTEN", c_dunk == RE15_CUR_IDX_SCHATTEN);

    int r_hell = texel(38, v_hell);
    int r_dunk = texel(38, v_dunk);
    printf("[abtast] Zeile y=%d -> Texel-Index %d | Zeile y=%d -> Texel-Index %d\n",
           RE15_GW_HELL_Y, r_hell, RE15_GW_SCHATTEN_Y, r_dunk);
    CHECK("PSX y=118: waagerechter Kreuzarm HELL",
          r_hell == RE15_CUR_IDX_HELL || r_hell == RE15_CUR_IDX_HELLKERN);
    CHECK("PSX y=119: dort liegt sein SCHATTEN", r_dunk == RE15_CUR_IDX_SCHATTEN);

    free(rdt);
    printf(s_fail ? "\nROT: %d Pruefung(en) fehlgeschlagen\n" : "\nGRUEN\n", s_fail);
    return s_fail ? 1 : 0;
}
