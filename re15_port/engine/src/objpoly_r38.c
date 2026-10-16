/*
 * RE1.5 Rebuilt — Runde 38 Spur W "rolltor20a0": Polygon-Schranken des Original-Objektzeichners
 * (FUN_8002c18c -> FUN_80022f0c / FUN_80023098 / FUN_800254a0 / FUN_800256b0) fuer den PC-Zeichenweg.
 * Herleitung, Adressen und Messung: include/re15_objpoly_r38.h und analysis/befunde_runde38/W_rolltor20a0.md.
 */
#include <stdio.h>
#include <stdlib.h>

#include "re15_objpoly_r38.h"
#include "re15_engine.h"     /* g_engine.frame_count (Messschiene) */
#include "re15_math.h"       /* re15_gte_divide = GTE-UNR-Division, gesaettigt auf 0x1FFFF */

static int32_t sat(int64_t v, int32_t lo, int32_t hi)
{
    return v < lo ? lo : (v > hi ? hi : (int32_t)v);
}

void re15_objpoly_r38_rtps(int32_t mac1, int32_t mac2, int32_t mac3, int h, int ofx, int ofy,
                           int16_t *sx, int16_t *sy, uint16_t *sz)
{
    /* psx-spx RTPS: IR1/IR2 = MAC1/MAC2 gesaettigt -8000h..7FFFh (lm = 0), SZ3 = MAC3 gesaettigt 0..FFFFh,
     * MAC0 = n*IR + OF (64 Bit gerechnet), SX2/SY2 = MAC0 / 10000h gesaettigt -400h..+3FFh. Die Bildmitte steht
     * hier als ganze Pixel (SetGeomOffset(0xa0,0x78) @0x8001643c/@0x80016440 laedt sie << 16); (OF<<16 + n*IR) >> 16
     * == OF + (n*IR >> 16), weil OF<<16 ein Vielfaches von 10000h ist. */
    const int32_t ir1 = sat(mac1, -0x8000, 0x7FFF);
    const int32_t ir2 = sat(mac2, -0x8000, 0x7FFF);
    const uint32_t sz3 = (uint32_t)sat(mac3, 0, 0xFFFF);
    const uint32_t n = re15_gte_divide((uint32_t)h, sz3);
    *sx = (int16_t)sat((int64_t)ofx + (((int64_t)ir1 * (int64_t)n) >> 16), RE15_OBJPOLY_SXY_MIN, RE15_OBJPOLY_SXY_MAX);
    *sy = (int16_t)sat((int64_t)ofy + (((int64_t)ir2 * (int64_t)n) >> 16), RE15_OBJPOLY_SXY_MIN, RE15_OBJPOLY_SXY_MAX);
    *sz = (uint16_t)sz3;
}

int re15_objpoly_r38_gpu_ok(int x0, int y0, int x1, int y1, int x2, int y2)
{
    const int mnx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    const int mxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    const int mny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    const int mxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    return (mxx - mnx) <= RE15_OBJPOLY_GPU_DX_MAX && (mxy - mny) <= RE15_OBJPOLY_GPU_DY_MAX;
}

/* NCLIP (psx-spx): MAC0 = SX0*SY1 + SX1*SY2 + SX2*SY0 - SX0*SY2 - SX1*SY0 - SX2*SY1 auf den GESAETTIGTEN SXY. */
static int32_t nclip_opz(const re15_objpoly_t *p)
{
    const int32_t x0 = p->sx[0], y0 = p->sy[0], x1 = p->sx[1], y1 = p->sy[1], x2 = p->sx[2], y2 = p->sy[2];
    return x0 * y1 + x1 * y2 + x2 * y0 - x0 * y2 - x1 * y0 - x2 * y1;
}

/* Messschiene (RE15_OBJPOLY_LOG): Zaehler je Objekt eines Bilds. */
static int s_n_nclip, s_n_otz, s_n_gpu, s_n_ok;
static void zaehle(int nclip_weg, int otz_weg, int gpu_weg, int ok)
{
    s_n_nclip += nclip_weg; s_n_otz += otz_weg; s_n_gpu += gpu_weg; s_n_ok += ok;
}

int re15_objpoly_r38_tri(const int32_t mac[3][3], int h, int ofx, int ofy, int nclip, re15_objpoly_t *out)
{
    for (int v = 0; v < 3; v++)
        re15_objpoly_r38_rtps(mac[v][0], mac[v][1], mac[v][2], h, ofx, ofy, &out->sx[v], &out->sy[v], &out->sz[v]);
    out->sx[3] = out->sy[3] = 0; out->sz[3] = 0;
    out->opz = nclip_opz(out);
    /* AVSZ3: OTZ = ZSF3*(SZ1+SZ2+SZ3) >> 12 (gesaettigt 0..FFFFh) */
    out->otz = sat(((int64_t)RE15_OBJPOLY_ZSF3 * ((int64_t)out->sz[0] + out->sz[1] + out->sz[2])) >> 12, 0, 0xFFFF);
    out->tri_ok[0] = (uint8_t)re15_objpoly_r38_gpu_ok(out->sx[0], out->sy[0], out->sx[1], out->sy[1],
                                                      out->sx[2], out->sy[2]);
    out->tri_ok[1] = 0;
    if (nclip && out->opz < 0)              { zaehle(1, 0, 0, 0); return 0; }  /* bltz @0x80023010 / bgez @0x800255a8 */
    if (out->otz < RE15_OBJPOLY_OTZ_MIN)    { zaehle(0, 1, 0, 0); return 0; }  /* @0x80023044/48, @0x80025654/58 */
    if (!out->tri_ok[0])                    { zaehle(0, 0, 1, 0); return 0; }  /* GPU Size Restriction */
    zaehle(0, 0, 0, 1);
    return 1;
}

int re15_objpoly_r38_quad(const int32_t mac[4][3], int h, int ofx, int ofy, int nclip, re15_objpoly_t *out)
{
    for (int v = 0; v < 4; v++)
        re15_objpoly_r38_rtps(mac[v][0], mac[v][1], mac[v][2], h, ofx, ofy, &out->sx[v], &out->sy[v], &out->sz[v]);
    out->opz = nclip_opz(out);                  /* NCLIP nach dem RTPT der Scheitel 0..2 (@0x80023168 / @0x80025798) */
    /* AVSZ4: OTZ = ZSF4*(SZ0+SZ1+SZ2+SZ3) >> 12 (gesaettigt 0..FFFFh) */
    out->otz = sat(((int64_t)RE15_OBJPOLY_ZSF4 *
                    ((int64_t)out->sz[0] + out->sz[1] + out->sz[2] + out->sz[3])) >> 12, 0, 0xFFFF);
    out->tri_ok[0] = (uint8_t)re15_objpoly_r38_gpu_ok(out->sx[0], out->sy[0], out->sx[1], out->sy[1],
                                                      out->sx[2], out->sy[2]);
    out->tri_ok[1] = (uint8_t)re15_objpoly_r38_gpu_ok(out->sx[1], out->sy[1], out->sx[2], out->sy[2],
                                                      out->sx[3], out->sy[3]);
    if (nclip && out->opz < 0)              { zaehle(1, 0, 0, 0); return 0; }  /* bgez @0x80023190 / @0x800257c0 */
    if (out->otz < RE15_OBJPOLY_OTZ_MIN)    { zaehle(0, 1, 0, 0); return 0; }  /* @0x80023224/28, @0x800258b0/b4 */
    if (!out->tri_ok[0] && !out->tri_ok[1]) { zaehle(0, 0, 1, 0); return 0; }  /* GPU Size Restriction, beide */
    zaehle(0, 0, 0, 1);
    return 1;
}

/* Sichtraum eines MD1-Scheitels (x,y,z = v[0..2]) — dieselbe Rechnung wie die bisherige Prop-Schleife in main.c
 * (Kamera x Objekt in Q12, Translation in Welteinheiten), = GTE MAC1..3 = (TR*1000h + R*V) >> 12. */
static void sicht(const re15_md1_vertex_t *v, const int32_t bm[9], const int32_t bt[3], int32_t out[3])
{
    const int64_t x = v->x, y = v->y, z = v->z;
    out[0] = (int32_t)((x * bm[0] + y * bm[1] + z * bm[2]) >> 12) + bt[0];
    out[1] = (int32_t)((x * bm[3] + y * bm[4] + z * bm[5]) >> 12) + bt[1];
    out[2] = (int32_t)((x * bm[6] + y * bm[7] + z * bm[8]) >> 12) + bt[2];
}

int re15_objpoly_r38_tri_mesh(const re15_md1_vertex_t *const v[3], const int32_t bm[9], const int32_t bt[3],
                              int h, int ofx, int ofy, int nclip, int ax[3], int ay[3], int *wz)
{
    int32_t mac[3][3];
    for (int k = 0; k < 3; k++) sicht(v[k], bm, bt, mac[k]);
    re15_objpoly_t p;
    const int ok = re15_objpoly_r38_tri(mac, h, ofx, ofy, nclip, &p);
    for (int k = 0; k < 3; k++) { ax[k] = p.sx[k]; ay[k] = p.sy[k]; }
    *wz = ((int)p.sz[0] + p.sz[1] + p.sz[2]) / 3;
    return ok;
}

int re15_objpoly_r38_quad_mesh(const re15_md1_vertex_t *const v[4], const int32_t bm[9], const int32_t bt[3],
                               int h, int ofx, int ofy, int nclip, int ax[4], int ay[4], int *wz, uint8_t tri_ok[2])
{
    int32_t mac[4][3];
    for (int k = 0; k < 4; k++) sicht(v[k], bm, bt, mac[k]);
    re15_objpoly_t p;
    const int ok = re15_objpoly_r38_quad(mac, h, ofx, ofy, nclip, &p);
    for (int k = 0; k < 4; k++) { ax[k] = p.sx[k]; ay[k] = p.sy[k]; }
    *wz = ((int)p.sz[0] + p.sz[1] + p.sz[2] + p.sz[3]) / 4;
    tri_ok[0] = p.tri_ok[0]; tri_ok[1] = p.tri_ok[1];
    return ok;
}

/* ---- Messschiene RE15_OBJPOLY_LOG (Diagnose, kein Verhalten) ------------------------------------------------- */
static FILE    *s_log;
static int      s_log_init;
static int      s_obj = -1, s_cut = -1;
static unsigned s_room, s_frame;

static void flush_obj(void)
{
    if (s_log && s_obj >= 0 && (s_n_nclip + s_n_otz + s_n_gpu + s_n_ok) > 0) {
        fprintf(s_log, "F%u room=%04X cut=%d obj=%d gezeichnet=%d nclip=%d otz=%d gpu=%d\n",
                s_frame, s_room, s_cut, s_obj, s_n_ok, s_n_nclip, s_n_otz, s_n_gpu);
        fflush(s_log);
    }
    s_n_nclip = s_n_otz = s_n_gpu = s_n_ok = 0;
}

void re15_objpoly_r38_objekt(int obj_id, int cut, unsigned room)
{
    if (!s_log_init) {
        s_log_init = 1;
        const char *p = getenv("RE15_OBJPOLY_LOG");
        if (p && *p) s_log = fopen(p, "w");
    }
    flush_obj();
    s_obj = obj_id; s_cut = cut; s_room = room; s_frame = (unsigned)g_engine.frame_count;
}
