/* probe_r32_unterteilung.c — Riegel der Runde 32 "Tueren-Unterteilung"
 * (analysis/befunde_runde32/tueren_unterteilung.md).
 *
 * Teile (Aufruf mit dem Teilnamen, je ein ctest):
 *   referenz  re15_door_divide_gt3 gegen die ORIGINAL-Befehle: ein kleiner MIPS-R3000-Interpreter
 *             (nur die Befehle, die DivideGT3 0x8008ebf4, RotAverageNclip3 0x8008edec, ReadSZfifo3
 *             0x8008edcc, RCpolyGT3/RCpolyGT3A 0x8008ee7c/0x8008ee84 und die Paketausgabe 0x8008f288
 *             benutzen, dazu GTE RTPT/NCLIP/AVSZ3 nach psx-spx) fuehrt den Code aus
 *             info/re2leon/PSX.EXE aus, aufgerufen wie FUN_8001468c @0x80014a38..94 (rgb0 dreimal,
 *             ot = ein OT-Eintrag, divp mit ndiv 3 / pih 320 / piv 240 wie Door_init @0x80013dcc..dc).
 *             Verglichen wird jedes ausgegebene POLY_GT3 (Reihenfolge ueber die OT-Kette, Ecken,
 *             UV, Farbe, Code 0x34, clut/tpage) und der zurueckgegebene Paketzeiger. Faelle: alle
 *             12 Dreiecke des DOOR13-Blatts (Mesh 0) in den echten Matrizen der Sequenz DOOR13 V0/V1
 *             (jedes 4. Bild), dazu 3000 Zufallsdreiecke (Nah-/Bild-Verwurf, Saettigung).
 *   tor       Flag-Tor am echten Zeichenpfad re15_door_mesh_zeichnen: DOOR13 Blatt mit 0x0aa0 teilt
 *             (je sichtbares Dreieck die Teildreiecke von DivideGT3, alle mit dem OT-Platz und der
 *             Eckfarbe 0 des Ursprungsdreiecks), mit 0x0a80 (Tor ROOM1170 / DOOR2E) keines.
 */
#include "re15_door_seq.h"
#include "re15_md1.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; if (g_fehler < 40) { printf("FEHLER: " __VA_ARGS__); printf("\n"); } } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* =============================================================================================
 * Mini-R3000 + GTE (nur was die fuenf Funktionen brauchen). RAM 2 MB ab 0x80000000.
 * Ladeverzoegerung wird nicht nachgebildet (der PsyQ-Code haelt sie mit nop/Umstellung ein).
 * =========================================================================================== */
#define RAM_N 0x200000u
static uint8_t  s_ram[RAM_N];
static uint32_t R[32];
static int32_t  G_d[32];   /* GTE-Datenregister (roh) */
static int32_t  G_c[32];   /* GTE-Steuerregister */
static int64_t  sxy_fifo[3][2];

static uint32_t ma(uint32_t a) { return a & (RAM_N - 1); }
static uint32_t ld32(uint32_t a) { a = ma(a); return (uint32_t)s_ram[a] | ((uint32_t)s_ram[a+1] << 8) | ((uint32_t)s_ram[a+2] << 16) | ((uint32_t)s_ram[a+3] << 24); }
static uint16_t ld16(uint32_t a) { a = ma(a); return (uint16_t)(s_ram[a] | (s_ram[a+1] << 8)); }
static void st32(uint32_t a, uint32_t v) { a = ma(a); s_ram[a] = (uint8_t)v; s_ram[a+1] = (uint8_t)(v >> 8); s_ram[a+2] = (uint8_t)(v >> 16); s_ram[a+3] = (uint8_t)(v >> 24); }
static void st16(uint32_t a, uint16_t v) { a = ma(a); s_ram[a] = (uint8_t)v; s_ram[a+1] = (uint8_t)(v >> 8); }

static int32_t sat(int64_t v, int32_t lo, int32_t hi) { return v < lo ? lo : (v > hi ? hi : (int32_t)v); }

/* GTE-Datenregister lesen/schreiben (psx-spx "GTE Registers") */
enum { VXY0 = 0, VZ0, VXY1, VZ1, VXY2, VZ2, RGBC, OTZ, IR0, IR1, IR2, IR3, SXY0, SXY1, SXY2, SXYP,
       SZ0, SZ1, SZ2, SZ3, RGB0, RGB1, RGB2, RES1, MAC0, MAC1, MAC2, MAC3 };
static uint32_t gte_lies(int r)
{
    switch (r) {
    case SXY0: case SXY1: case SXY2: case SXYP: {
        int k = r == SXYP ? 2 : r - SXY0;
        return (uint32_t)(uint16_t)sxy_fifo[k][0] | ((uint32_t)(uint16_t)sxy_fifo[k][1] << 16);
    }
    case OTZ: case SZ0: case SZ1: case SZ2: case SZ3: return (uint32_t)(uint16_t)G_d[r];
    case IR0: case IR1: case IR2: case IR3: return (uint32_t)(int32_t)(int16_t)G_d[r];
    default: return (uint32_t)G_d[r];
    }
}
static void gte_schreib(int r, uint32_t v) { G_d[r] = (int32_t)v; }
static int16_t vk(int n, int k) /* Vektor n, Komponente k */
{
    uint32_t xy = (uint32_t)G_d[n * 2], z = (uint32_t)G_d[n * 2 + 1];
    return k == 0 ? (int16_t)(xy & 0xffff) : (k == 1 ? (int16_t)(xy >> 16) : (int16_t)(z & 0xffff));
}
static int16_t rt(int i, int k)   /* RT11..RT33 aus den gepackten Steuerregistern 0..4 */
{
    int idx = i * 3 + k; uint32_t w = (uint32_t)G_c[idx / 2];
    return (int16_t)((idx & 1) ? (w >> 16) : (w & 0xffff));
}
static void gte_rtps(int n)   /* sf=1, lm=0 */
{
    int64_t mac[3];
    for (int i = 0; i < 3; i++)
        mac[i] = (((int64_t)G_c[5 + i] << 12) + (int64_t)rt(i, 0) * vk(n, 0) + (int64_t)rt(i, 1) * vk(n, 1)
                  + (int64_t)rt(i, 2) * vk(n, 2)) >> 12;
    for (int i = 0; i < 3; i++) { G_d[MAC1 + i] = (int32_t)mac[i]; G_d[IR1 + i] = sat(mac[i], -0x8000, 0x7fff); }
    G_d[SZ0] = G_d[SZ1]; G_d[SZ1] = G_d[SZ2]; G_d[SZ2] = G_d[SZ3];
    G_d[SZ3] = sat(mac[2], 0, 0xffff);
    uint32_t h = (uint32_t)(uint16_t)G_c[26];
    uint32_t q = re15_gte_divide(h, (uint32_t)G_d[SZ3]);
    int64_t sx = (int64_t)q * (int16_t)G_d[IR1] + G_c[24];
    int64_t sy = (int64_t)q * (int16_t)G_d[IR2] + G_c[25];
    memmove(sxy_fifo[0], sxy_fifo[1], sizeof sxy_fifo[0] * 2);
    sxy_fifo[2][0] = sat(sx >> 16, -0x400, 0x3ff);
    sxy_fifo[2][1] = sat(sy >> 16, -0x400, 0x3ff);
    int64_t dq = (int64_t)q * (int16_t)G_c[27] + G_c[28];
    G_d[MAC0] = (int32_t)dq;
    G_d[IR0] = sat(dq >> 12, 0, 0x1000);
}
static void gte_befehl(uint32_t w)
{
    switch (w & 0x3f) {
    case 0x30: gte_rtps(0); gte_rtps(1); gte_rtps(2); break;           /* RTPT */
    case 0x06: {                                                       /* NCLIP */
        int64_t m = sxy_fifo[0][0] * sxy_fifo[1][1] + sxy_fifo[1][0] * sxy_fifo[2][1] + sxy_fifo[2][0] * sxy_fifo[0][1]
                  - sxy_fifo[0][0] * sxy_fifo[2][1] - sxy_fifo[1][0] * sxy_fifo[0][1] - sxy_fifo[2][0] * sxy_fifo[1][1];
        G_d[MAC0] = (int32_t)m; break;
    }
    case 0x2d: {                                                       /* AVSZ3 */
        int64_t m = (int64_t)(int16_t)G_c[29] * ((uint16_t)G_d[SZ1] + (uint16_t)G_d[SZ2] + (uint16_t)G_d[SZ3]);
        G_d[MAC0] = (int32_t)m; G_d[OTZ] = sat(m >> 12, 0, 0xffff); break;
    }
    default: printf("FEHLER: GTE-Befehl %08x nicht nachgebildet\n", w); g_fehler++; break;
    }
}

/* Fuehrt ab pc aus, bis pc == stopp. Liefert 0 bei Erfolg. */
static int mips_lauf(uint32_t pc, uint32_t stopp)
{
    uint32_t npc = pc + 4;
    long schritte = 0;
    while (pc != stopp) {
        if (++schritte > 20000000L) { printf("FEHLER: Endlosschleife @%08x\n", pc); return -1; }
        uint32_t w = ld32(pc);
        uint32_t op = w >> 26, rs = (w >> 21) & 31, rtn = (w >> 16) & 31, rd = (w >> 11) & 31, sh = (w >> 6) & 31;
        uint32_t fn = w & 63; int32_t simm = (int16_t)(w & 0xffff); uint32_t imm = w & 0xffff;
        uint32_t ziel = npc + 4;   /* naechster npc */
        uint32_t a;
        switch (op) {
        case 0x00:
            switch (fn) {
            case 0x00: R[rd] = R[rtn] << sh; break;
            case 0x02: R[rd] = R[rtn] >> sh; break;
            case 0x03: R[rd] = (uint32_t)((int32_t)R[rtn] >> sh); break;
            case 0x08: ziel = R[rs]; break;
            case 0x20: case 0x21: R[rd] = R[rs] + R[rtn]; break;
            case 0x22: case 0x23: R[rd] = R[rs] - R[rtn]; break;
            case 0x24: R[rd] = R[rs] & R[rtn]; break;
            case 0x25: R[rd] = R[rs] | R[rtn]; break;
            case 0x2a: R[rd] = (int32_t)R[rs] < (int32_t)R[rtn]; break;
            case 0x2b: R[rd] = R[rs] < R[rtn]; break;
            default: printf("FEHLER: SPECIAL %08x @%08x\n", w, pc); return -1;
            }
            break;
        case 0x01: printf("FEHLER: REGIMM %08x @%08x\n", w, pc); return -1;
        case 0x02: ziel = (npc & 0xf0000000u) | ((w & 0x3ffffff) << 2); break;
        case 0x03: R[31] = npc + 4; ziel = (npc & 0xf0000000u) | ((w & 0x3ffffff) << 2); break;
        case 0x04: if (R[rs] == R[rtn]) ziel = npc + ((uint32_t)simm << 2); break;
        case 0x05: if (R[rs] != R[rtn]) ziel = npc + ((uint32_t)simm << 2); break;
        case 0x06: if ((int32_t)R[rs] <= 0) ziel = npc + ((uint32_t)simm << 2); break;
        case 0x07: if ((int32_t)R[rs] > 0) ziel = npc + ((uint32_t)simm << 2); break;
        case 0x08: case 0x09: R[rtn] = R[rs] + (uint32_t)simm; break;
        case 0x0a: R[rtn] = (int32_t)R[rs] < simm; break;
        case 0x0b: R[rtn] = R[rs] < (uint32_t)simm; break;
        case 0x0c: R[rtn] = R[rs] & imm; break;
        case 0x0d: R[rtn] = R[rs] | imm; break;
        case 0x0f: R[rtn] = imm << 16; break;
        case 0x12: /* COP2 */
            if (w & (1u << 25)) gte_befehl(w);
            else if (rs == 0x00) R[rtn] = gte_lies((int)rd);                       /* mfc2 */
            else if (rs == 0x02) R[rtn] = (rd == 26) ? (uint32_t)(int32_t)(int16_t)G_c[26] : (uint32_t)G_c[rd]; /* cfc2 */
            else if (rs == 0x04) gte_schreib((int)rd, R[rtn]);                     /* mtc2 */
            else if (rs == 0x06) G_c[rd] = (int32_t)R[rtn];                        /* ctc2 */
            else { printf("FEHLER: COP2 %08x @%08x\n", w, pc); return -1; }
            break;
        case 0x20: R[rtn] = (uint32_t)(int32_t)(int8_t)s_ram[ma(R[rs] + (uint32_t)simm)]; break;
        case 0x21: R[rtn] = (uint32_t)(int32_t)(int16_t)ld16(R[rs] + (uint32_t)simm); break;
        case 0x23: R[rtn] = ld32(R[rs] + (uint32_t)simm); break;
        case 0x24: R[rtn] = s_ram[ma(R[rs] + (uint32_t)simm)]; break;
        case 0x25: R[rtn] = ld16(R[rs] + (uint32_t)simm); break;
        case 0x22: { /* lwl */
            a = R[rs] + (uint32_t)simm; uint32_t m = ld32(a & ~3u); int k = (int)(a & 3);
            R[rtn] = (R[rtn] & (0x00ffffffu >> (k * 8))) | (m << (24 - k * 8)); break; }
        case 0x26: { /* lwr */
            a = R[rs] + (uint32_t)simm; uint32_t m = ld32(a & ~3u); int k = (int)(a & 3);
            R[rtn] = (R[rtn] & (0xffffff00u << (24 - k * 8))) | (m >> (k * 8)); break; }
        case 0x28: s_ram[ma(R[rs] + (uint32_t)simm)] = (uint8_t)R[rtn]; break;
        case 0x29: st16(R[rs] + (uint32_t)simm, (uint16_t)R[rtn]); break;
        case 0x2b: st32(R[rs] + (uint32_t)simm, R[rtn]); break;
        case 0x2a: { /* swl */
            a = R[rs] + (uint32_t)simm; uint32_t m = ld32(a & ~3u); int k = (int)(a & 3);
            m = (m & (0xffffff00u << (k * 8))) | (R[rtn] >> (24 - k * 8)); st32(a & ~3u, m); break; }
        case 0x2e: { /* swr */
            a = R[rs] + (uint32_t)simm; uint32_t m = ld32(a & ~3u); int k = (int)(a & 3);
            m = (m & (0x00ffffffu >> (24 - k * 8))) | (R[rtn] << (k * 8)); st32(a & ~3u, m); break; }
        case 0x32: gte_schreib((int)rtn, ld32(R[rs] + (uint32_t)simm)); break;              /* lwc2 */
        case 0x3a: st32(R[rs] + (uint32_t)simm, gte_lies((int)rtn)); break;                /* swc2 */
        default: printf("FEHLER: Befehl %08x @%08x nicht nachgebildet\n", w, pc); return -1;
        }
        R[0] = 0;
        pc = npc; npc = ziel;
    }
    return 0;
}

/* Arbeitsbereich im Nachbau-RAM */
#define A_DIVP 0x80100000u
#define A_VERT 0x80101000u
#define A_PAK  0x80102000u   /* das POLY_GT3 des Aufrufers (rgb0 @+4, uv0 @+12, uv1 @+24, uv2 @+36) */
#define A_OT   0x80103000u
#define A_AUS  0x80110000u
#define A_SP   0x801ff000u
#define A_HALT 0x80000100u
#define RE2_DIVIDEGT3 0x8008ebf4u
#define CODE_GT3 0x34        /* NCCT-Farbe traegt den Code aus RGBC (FUN_8001468c @0x80014744 ori v0,v0,0x34) */

static int s_exe_ok = 0;

static int exe_laden(void)
{
    size_t n = 0;
    uint8_t *d = slurp(RE15_RE2_EXE_PATH, &n);
    if (!d || n < 0x800) { printf("FEHLER: %s nicht lesbar\n", RE15_RE2_EXE_PATH); return -1; }
    uint32_t ta = (uint32_t)d[0x18] | ((uint32_t)d[0x19] << 8) | ((uint32_t)d[0x1a] << 16) | ((uint32_t)d[0x1b] << 24);
    uint32_t ts = (uint32_t)d[0x1c] | ((uint32_t)d[0x1d] << 8) | ((uint32_t)d[0x1e] << 16) | ((uint32_t)d[0x1f] << 24);
    if (0x800 + ts > n || ma(ta) + ts > RAM_N) { free(d); return -1; }
    memcpy(&s_ram[ma(ta)], d + 0x800, ts);
    free(d);
    /* Stichprobe: das erste Wort von DivideGT3 ist addiu sp,sp,-96 (27bdffa0) */
    if (ld32(RE2_DIVIDEGT3) != 0x27bdffa0u) { printf("FEHLER: 0x8008ebf4 ist nicht DivideGT3 (%08x)\n", ld32(RE2_DIVIDEGT3)); return -1; }
    s_exe_ok = 1;
    return 0;
}

typedef struct { int16_t x[3], y[3]; uint8_t u[3], v[3], rgb[3][3]; uint16_t clut, tpage; uint8_t code; } tri_t;
#define MAXT 128

/* Original: DivideGT3 wie FUN_8001468c @0x80014a38..94 aufrufen, Pakete ueber die OT-Kette lesen
 * (Kopf = zuletzt eingehaengt) und in Ausgabefolge umdrehen. */
static int orig_teilen(const re15_door_mat_t *w, const re15_md1_vertex_t *v[3], const uint8_t uv[3][2],
                       const uint8_t rgb0[3], uint16_t clut, uint16_t tpage, tri_t *out, uint32_t *rueck)
{
    memset(R, 0, sizeof R); memset(G_d, 0, sizeof G_d); memset(G_c, 0, sizeof G_c); memset(sxy_fifo, 0, sizeof sxy_fifo);
    for (int i = 0; i < 5; i++) {
        int a0 = i * 2, a1 = i * 2 + 1;
        uint16_t lo = (uint16_t)w->m[a0], hi = a1 < 9 ? (uint16_t)w->m[a1] : 0;
        G_c[i] = (int32_t)(lo | ((uint32_t)hi << 16));
    }
    G_c[5] = w->t[0]; G_c[6] = w->t[1]; G_c[7] = w->t[2];
    G_c[24] = RE15_DOOR_OFX << 16; G_c[25] = RE15_DOOR_OFY << 16;   /* SetGeomOffset 0x8008de04: sll 16 */
    G_c[26] = RE15_DOOR_H; G_c[29] = 341;                           /* ZSF3 @0x8008d29c */
    /* DIVPOLYGON3 */
    memset(&s_ram[ma(A_DIVP)], 0x5a, 600);
    st32(A_DIVP + 0, RE15_DOOR_NDIV); st32(A_DIVP + 4, RE15_DOOR_PIH); st32(A_DIVP + 8, RE15_DOOR_PIV);
    for (int i = 0; i < 3; i++) {
        st16(A_VERT + i * 8 + 0, (uint16_t)v[i]->x); st16(A_VERT + i * 8 + 2, (uint16_t)v[i]->y);
        st16(A_VERT + i * 8 + 4, (uint16_t)v[i]->z); st16(A_VERT + i * 8 + 6, 0);
    }
    st32(A_PAK + 4, rgb0[0] | (rgb0[1] << 8) | (rgb0[2] << 16) | ((uint32_t)CODE_GT3 << 24));
    st32(A_PAK + 12, uv[0][0] | (uv[0][1] << 8) | ((uint32_t)clut << 16));
    st32(A_PAK + 24, uv[1][0] | (uv[1][1] << 8) | ((uint32_t)tpage << 16));
    st32(A_PAK + 36, uv[2][0] | (uv[2][1] << 8));
    st32(A_OT, 0x00ffffffu);
    memset(&s_ram[ma(A_AUS)], 0, 64 * 40 + 64);
    /* Argumente (@0x80014a38..94): a0..a2 Vertexkopien, a3 uv0, Stapel 16 uv1, 20 uv2, 24/28/32 rgb0,
     * 36 Paket, 40 ot, 44 divp */
    R[29] = A_SP; R[31] = A_HALT;
    R[4] = A_VERT; R[5] = A_VERT + 8; R[6] = A_VERT + 16; R[7] = A_PAK + 12;
    st32(A_SP + 16, A_PAK + 24); st32(A_SP + 20, A_PAK + 36);
    st32(A_SP + 24, A_PAK + 4); st32(A_SP + 28, A_PAK + 4); st32(A_SP + 32, A_PAK + 4);
    st32(A_SP + 36, A_AUS); st32(A_SP + 40, A_OT); st32(A_SP + 44, A_DIVP);
    if (mips_lauf(RE2_DIVIDEGT3, A_HALT) != 0) return -1;
    *rueck = R[2];
    /* OT-Kette */
    uint32_t kette[MAXT]; int n = 0;
    uint32_t p = ld32(A_OT) & 0xffffff;
    while (p != 0xffffff && n < MAXT) {
        uint32_t pa = 0x80000000u | p;
        kette[n++] = pa;
        uint32_t tag = ld32(pa);
        if ((tag >> 24) != 9) { printf("FEHLER: Paket %08x Laenge %u statt 9\n", pa, tag >> 24); g_fehler++; }
        p = tag & 0xffffff;
    }
    for (int k = 0; k < n; k++) {
        uint32_t pa = kette[n - 1 - k];
        tri_t *t = &out[k];
        for (int e = 0; e < 3; e++) {
            uint32_t c = ld32(pa + 4 + e * 12), xy = ld32(pa + 8 + e * 12), uvw = ld32(pa + 12 + e * 12);
            t->rgb[e][0] = (uint8_t)c; t->rgb[e][1] = (uint8_t)(c >> 8); t->rgb[e][2] = (uint8_t)(c >> 16);
            if (e == 0) t->code = (uint8_t)(c >> 24);
            t->x[e] = (int16_t)(xy & 0xffff); t->y[e] = (int16_t)(xy >> 16);
            t->u[e] = (uint8_t)uvw; t->v[e] = (uint8_t)(uvw >> 8);
            if (e == 0) t->clut = (uint16_t)(uvw >> 16);
            if (e == 1) t->tpage = (uint16_t)(uvw >> 16);
        }
    }
    return n;
}

/* Port: re15_door_divide_gt3 mit Mitschrift */
static tri_t s_port[MAXT]; static int s_np;
static void port_mit(void *ctx, const re15_div_rvec_t *a, const re15_div_rvec_t *b, const re15_div_rvec_t *c)
{
    (void)ctx;
    if (s_np >= MAXT) { s_np++; return; }
    const re15_div_rvec_t *e[3] = { a, b, c };
    tri_t *t = &s_port[s_np++];
    for (int i = 0; i < 3; i++) {
        t->x[i] = e[i]->sx; t->y[i] = e[i]->sy; t->u[i] = e[i]->u; t->v[i] = e[i]->v;
        t->rgb[i][0] = e[i]->r; t->rgb[i][1] = e[i]->g; t->rgb[i][2] = e[i]->b;
    }
}

static long s_faelle, s_dreiecke, s_leer, s_voll;

static void vergleiche(const char *wo, const re15_door_mat_t *w, const re15_md1_vertex_t *v[3],
                       const uint8_t uv[3][2], const uint8_t rgb0[3])
{
    static tri_t orig[MAXT];
    uint16_t clut = 0x7800 | (uint16_t)(s_faelle & 0x3f), tpage = (uint16_t)(0x0080 | (s_faelle & 0x1f));
    uint32_t rueck = 0;
    int no = orig_teilen(w, v, uv, rgb0, clut, tpage, orig, &rueck);
    s_np = 0;
    int np = re15_door_divide_gt3(w, v, uv, rgb0, RE15_DOOR_NDIV, RE15_DOOR_PIH, RE15_DOOR_PIV, port_mit, NULL);
    s_faelle++;
    PRUEF(no >= 0, "%s: Interpreter-Abbruch", wo);
    if (no < 0) return;
    PRUEF(np == no && s_np == no, "%s: %d Teildreiecke im Port, %d im Original", wo, np, no);
    PRUEF(rueck == A_AUS + 40u * (uint32_t)no, "%s: Rueckgabe %08x statt %08x", wo, rueck, A_AUS + 40u * (uint32_t)no);
    if (no == 0) s_leer++;
    if (no == 64) s_voll++;
    s_dreiecke += no;
    int n = no < np ? no : np;
    for (int k = 0; k < n && k < MAXT; k++) {
        const tri_t *a = &orig[k], *b = &s_port[k];
        int gleich = 1;
        for (int e = 0; e < 3; e++)
            if (a->x[e] != b->x[e] || a->y[e] != b->y[e] || a->u[e] != b->u[e] || a->v[e] != b->v[e]
                || memcmp(a->rgb[e], b->rgb[e], 3) != 0) gleich = 0;
        PRUEF(gleich, "%s: Teildreieck %d: Original (%d,%d uv %d,%d)(%d,%d uv %d,%d)(%d,%d uv %d,%d) Port (%d,%d uv %d,%d)(%d,%d uv %d,%d)(%d,%d uv %d,%d)",
              wo, k, a->x[0], a->y[0], a->u[0], a->v[0], a->x[1], a->y[1], a->u[1], a->v[1], a->x[2], a->y[2], a->u[2], a->v[2],
              b->x[0], b->y[0], b->u[0], b->v[0], b->x[1], b->y[1], b->u[1], b->v[1], b->x[2], b->y[2], b->u[2], b->v[2]);
        PRUEF(a->code == CODE_GT3 && a->clut == clut && a->tpage == tpage,
              "%s: Teildreieck %d Code %02x clut %04x tpage %04x", wo, k, a->code, a->clut, a->tpage);
        for (int e = 0; e < 3; e++)
            PRUEF(memcmp(a->rgb[e], rgb0, 3) == 0, "%s: Teildreieck %d Ecke %d nicht in Farbe rgb0", wo, k, e);
    }
}

/* DOOR13 aus shared_assets/RE2/DOOR: Modellteil nach der Tabelle @0x8009a520 */
static uint8_t *s_arch; static re15_md1_t s_md1; static const uint8_t *s_teil; static int s_nteil;
static int door13_laden(void)
{
    int ton = 0, modell = 0, sektor = 0, datei = 0; size_t n = 0;
    if (re15_door_seq_re2_archiv(0x13, &ton, &modell, &sektor, &datei) != 0) return -1;
    s_arch = slurp(RE15_ASSET_RE2_DIR "/DOOR/DOOR13.DO2", &n);
    if (!s_arch || (int)n != datei) { printf("FEHLER: DOOR13.DO2 fehlt/Groesse\n"); return -1; }
    s_teil = s_arch + sektor * 0x800; s_nteil = modell;
    uint32_t md1 = (uint32_t)s_teil[0] | ((uint32_t)s_teil[1] << 8) | ((uint32_t)s_teil[2] << 16) | ((uint32_t)s_teil[3] << 24);
    uint32_t tim = (uint32_t)s_teil[4] | ((uint32_t)s_teil[5] << 8) | ((uint32_t)s_teil[6] << 16) | ((uint32_t)s_teil[7] << 24);
    if (re15_md1_parse(s_teil + md1, (int)(tim - md1), &s_md1) != 0 || s_md1.mesh_count < 1) return -1;
    return 0;
}

static uint32_t s_lcg = 12345u;
static int32_t zuf(int32_t lo, int32_t hi) { s_lcg = s_lcg * 1103515245u + 12345u; return lo + (int32_t)((s_lcg >> 8) % (uint32_t)(hi - lo + 1)); }

static int teil_referenz(void)
{
    if (exe_laden() != 0 || door13_laden() != 0) return 1;
    const re15_md1_mesh_t *m = &s_md1.meshes[0];
    PRUEF(m->triangle_count == 12, "DOOR13 Mesh 0 hat %d Dreiecke (erwartet 12)", m->triangle_count);
    /* echte Matrizen: Sequenz DOOR13 V0 und V1, Objekt 0 (Blatt, flags 0x0aa0), jedes 4. Bild */
    static re15_door_seq_t s;
    int n_mat = 0;
    for (int var = 0; var < 2; var++) {
        PRUEF(re15_door_seq_start(&s, s_teil, s_nteil, var, 0, 0x13) == 0, "DOOR13 V%d startet nicht", var);
        int bild = 0;
        while (bild < 600 && re15_door_seq_bild(&s, 1)) {
            if ((bild & 3) == 0 && s.obj[0].on) {
                PRUEF((s.obj[0].flags & RE15_DOOR_FLAG_TEILEN) != 0, "DOOR13 Objekt 0 ohne Flag 0x20 (0x%04x)", s.obj[0].flags);
                for (int t = 0; t < m->triangle_count; t++) {
                    const re15_md1_triangle_t *tr = &m->triangles[t];
                    const re15_md1_tri_uv_t *uv = &m->triangle_uvs[t];
                    const re15_md1_vertex_t *v[3] = { &m->tri_vertices[tr->v0], &m->tri_vertices[tr->v1], &m->tri_vertices[tr->v2] };
                    const uint8_t tuv[3][2] = { { uv->u0, uv->v0 }, { uv->u1, uv->v1 }, { uv->u2, uv->v2 } };
                    const uint8_t rgb0[3] = { (uint8_t)(40 + t), (uint8_t)(90 + bild % 50), 128 };
                    char wo[64]; snprintf(wo, sizeof wo, "DOOR13 V%d Bild %d Dreieck %d", var, bild, t);
                    vergleiche(wo, &s.obj[0].welt, v, tuv, rgb0);
                }
                n_mat++;
            }
            bild++;
        }
        re15_door_seq_ende(&s);
    }
    long echt_faelle = s_faelle, echt_tris = s_dreiecke;
    /* Zufall: Lage, Drehung, Tiefe (auch unter H/2 = 145), Ecken bis +-9000, UV/Farbe */
    for (int i = 0; i < 3000; i++) {
        re15_door_mat_t w;
        uint16_t rot[3] = { (uint16_t)zuf(0, 4095), (uint16_t)zuf(0, 4095), (uint16_t)zuf(0, 4095) };
        re15_door_rotmatrix(rot, w.m);
        w.t[0] = zuf(-3000, 3000); w.t[1] = zuf(-2500, 2500);
        w.t[2] = (i % 5 == 0) ? zuf(0, 400) : zuf(1500, 20000);
        re15_md1_vertex_t vv[3];
        int gross = (i % 3 == 0) ? 9000 : 2500;
        for (int k = 0; k < 3; k++) { vv[k].x = (int16_t)zuf(-gross, gross); vv[k].y = (int16_t)zuf(-gross, gross); vv[k].z = (int16_t)zuf(-gross, gross); vv[k].pad = 0; }
        const re15_md1_vertex_t *v[3] = { &vv[0], &vv[1], &vv[2] };
        const uint8_t tuv[3][2] = { { (uint8_t)zuf(0, 255), (uint8_t)zuf(0, 255) }, { (uint8_t)zuf(0, 255), (uint8_t)zuf(0, 255) },
                                    { (uint8_t)zuf(0, 255), (uint8_t)zuf(0, 255) } };
        const uint8_t rgb0[3] = { (uint8_t)zuf(0, 255), (uint8_t)zuf(0, 255), (uint8_t)zuf(0, 255) };
        char wo[48]; snprintf(wo, sizeof wo, "Zufall %d", i);
        vergleiche(wo, &w, v, tuv, rgb0);
    }
    /* Grenzfaelle der Verwurf-Tests (sltu/slt sind STRENG): alle drei sz == H/2 = 145 bleibt
     * (@0x8008eea4 sltu), 144 faellt; winzige Dreiecke an den Bildgrenzen OFX+-pih/2 = 0/320 und
     * OFY+-piv/2 = 0/240 (Teildreiecke mit allen Ecken genau auf der Grenze bleiben, @0x8008eef4 slt). */
    for (int zz = 144; zz <= 146; zz++) {
        re15_door_mat_t w; memset(&w, 0, sizeof w);
        w.m[0] = w.m[4] = w.m[8] = 4096; w.t[2] = zz;
        re15_md1_vertex_t vv[3] = { { 0, 0, 0, 0 }, { 40, 0, 0, 0 }, { 0, 40, 0, 0 } };
        const re15_md1_vertex_t *v[3] = { &vv[0], &vv[1], &vv[2] };
        const uint8_t tuv[3][2] = { { 0, 0 }, { 64, 0 }, { 0, 64 } };
        const uint8_t rgb0[3] = { 100, 110, 120 };
        char wo[48]; snprintf(wo, sizeof wo, "Nahgrenze sz %d", zz);
        long vor = s_dreiecke;
        vergleiche(wo, &w, v, tuv, rgb0);
        if (zz == 144) PRUEF(s_dreiecke == vor, "sz 144: Dreieck nicht verworfen");
        else PRUEF(s_dreiecke > vor, "sz %d: Dreieck verworfen", zz);
    }
    for (int i = 0; i < 4000; i++) {
        re15_door_mat_t w; memset(&w, 0, sizeof w);
        w.m[0] = w.m[4] = w.m[8] = 4096;
        w.t[2] = 290;                          /* n ~ 0x10000: SX ~ 160 + x, SY ~ 120 + y */
        int rand_x = (i & 1) ? 160 : -160, rand_y = (i & 2) ? 120 : -120;
        int wo_x = i & 4;                      /* Grenze in x oder in y pruefen */
        re15_md1_vertex_t vv[3];
        for (int k = 0; k < 3; k++) {
            vv[k].x = (int16_t)((wo_x ? rand_x : zuf(-100, 100)) + zuf(-4, 4));
            vv[k].y = (int16_t)((wo_x ? zuf(-100, 100) : rand_y) + zuf(-4, 4));
            vv[k].z = (int16_t)zuf(-3, 3); vv[k].pad = 0;
        }
        const re15_md1_vertex_t *v[3] = { &vv[0], &vv[1], &vv[2] };
        const uint8_t tuv[3][2] = { { (uint8_t)zuf(0, 255), 0 }, { 0, (uint8_t)zuf(0, 255) }, { 9, 9 } };
        const uint8_t rgb0[3] = { 1, 2, 3 };
        char wo[48]; snprintf(wo, sizeof wo, "Bildgrenze %d", i);
        vergleiche(wo, &w, v, tuv, rgb0);
    }
    printf("referenz: %d Blatt-Matrizen, %ld Blatt-Faelle (%ld Teildreiecke), %ld Faelle gesamt, "
           "%ld Teildreiecke, %ld ohne Paket (NCLIP/Verwurf), %ld mit allen 64\n",
           n_mat, echt_faelle, echt_tris, s_faelle, s_dreiecke, s_leer, s_voll);
    PRUEF(echt_tris > 0 && s_voll > 0 && s_leer > 0, "Faelle decken Teilen/Verwerfen nicht ab");
    PRUEF(s_voll < s_faelle - s_leer, "kein Fall mit teilweisem Bild-/Nah-Verwurf");
    free(s_arch);
    return 0;
}

/* ---------------------------------------------------------------------------------------------
 * Flag-Tor am echten Zeichenpfad
 * ------------------------------------------------------------------------------------------ */
#define MAXD 4096
static re15_door_dreieck_t s_d[MAXD]; static int s_nd;
static void mit(void *ctx, const re15_door_dreieck_t *d) { (void)ctx; if (s_nd < MAXD) s_d[s_nd] = *d; s_nd++; }

static int teil_tor(void)
{
    if (door13_laden() != 0) return 1;
    const re15_md1_mesh_t *m = &s_md1.meshes[0];
    static re15_door_seq_t s;
    PRUEF(re15_door_seq_start(&s, s_teil, s_nteil, 0, 0, 0x13) == 0, "DOOR13 V0 startet nicht");
    int bild = 0, geprueft = 0, geteilt_bilder = 0, randfaelle = 0;
    while (bild < 600 && re15_door_seq_bild(&s, 1)) {
        if (bild % 10 == 5 && s.obj[0].on) {
            const re15_door_obj_t *o = &s.obj[0];
            /* ohne 0x20 (Tor ROOM1170 / DOOR2E-Blatt 0x0a80) */
            static re15_door_dreieck_t ohne[64]; int n_ohne, lfd = 0;
            s_nd = 0;
            re15_door_mesh_zeichnen(m, &o->welt, &o->welt_vor, 0x0a80, &lfd, mit, NULL);
            n_ohne = s_nd;
            PRUEF(n_ohne <= 12, "Bild %d: %d Dreiecke ohne Flag 0x20", bild, n_ohne);
            for (int i = 0; i < n_ohne && i < 64; i++) {
                ohne[i] = s_d[i];
                PRUEF(!s_d[i].geteilt, "Bild %d: Dreieck %d ohne Flag 0x20 geteilt", bild, i);
            }
            /* mit 0x20 (DOOR13-Blatt 0x0aa0): je Ursprungsdreieck die Teildreiecke von DivideGT3 */
            lfd = 0; s_nd = 0;
            re15_door_mesh_zeichnen(m, &o->welt, &o->welt_vor, 0x0aa0, &lfd, mit, NULL);
            int soll = 0, k = 0, u = 0;
            for (int t = 0; t < m->triangle_count; t++) {
                const re15_md1_triangle_t *tr = &m->triangles[t];
                const re15_md1_tri_uv_t *uv = &m->triangle_uvs[t];
                const re15_md1_vertex_t *v[3] = { &m->tri_vertices[tr->v0], &m->tri_vertices[tr->v1], &m->tri_vertices[tr->v2] };
                re15_door_ecke_t e[3];
                for (int j = 0; j < 3; j++) e[j] = re15_door_rtpt(&o->welt, v[j]->x, v[j]->y, v[j]->z);
                int64_t mac0 = (int64_t)e[0].sx * e[1].sy + (int64_t)e[1].sx * e[2].sy + (int64_t)e[2].sx * e[0].sy
                             - (int64_t)e[0].sx * e[2].sy - (int64_t)e[1].sx * e[0].sy - (int64_t)e[2].sx * e[1].sy;
                int32_t otz = (341 * ((int32_t)e[0].sz + e[1].sz + e[2].sz)) >> 12;
                if (mac0 < 0 || (otz >> 6) == 0) continue;          /* auch ohne 0x20 nicht gezeichnet */
                const re15_door_dreieck_t *ur = &ohne[u++];
                const uint8_t tuv[3][2] = { { uv->u0, uv->v0 }, { uv->u1, uv->v1 }, { uv->u2, uv->v2 } };
                s_np = 0;
                int n = re15_door_divide_gt3(&o->welt, v, tuv, ur->rgb[0], RE15_DOOR_NDIV, RE15_DOOR_PIH, RE15_DOOR_PIV, port_mit, NULL);
                soll += n;
                for (int q = 0; q < n && k < s_nd && k < MAXD; q++, k++) {
                    const re15_door_dreieck_t *d = &s_d[k];
                    PRUEF(d->geteilt, "Bild %d: Teildreieck %d nicht als geteilt markiert", bild, k);
                    PRUEF((d->z >> 12) == (ur->z >> 12), "Bild %d: Teildreieck %d OT-Platz %d statt %d", bild, k, d->z >> 12, ur->z >> 12);
                    PRUEF(d->page == ur->page && d->clut == ur->clut, "Bild %d: Teildreieck %d page/clut", bild, k);
                    for (int j = 0; j < 3; j++) {
                        PRUEF(memcmp(d->rgb[j], ur->rgb[0], 3) == 0, "Bild %d: Teildreieck %d Ecke %d nicht Eckfarbe 0", bild, k, j);
                        PRUEF(d->x[j] == s_port[q].x[j] && d->y[j] == s_port[q].y[j] && d->u[j] == s_port[q].u[j]
                              && d->v[j] == s_port[q].v[j], "Bild %d: Teildreieck %d Ecke %d weicht von DivideGT3 ab", bild, k, j);
                    }
                }
            }
            PRUEF(u == n_ohne, "Bild %d: %d sichtbare Dreiecke gezaehlt, %d ohne Flag gezeichnet", bild, u, n_ohne);
            PRUEF(s_nd == soll, "Bild %d: %d Teildreiecke gezeichnet, %d erwartet (%d ungeteilt)", bild, s_nd, soll, n_ohne);
            if (soll > n_ohne) geteilt_bilder++;
            else if (n_ohne > 0) {
                /* sichtbar, aber DivideGT3 gibt nichts aus: nur erlaubt, wenn jedes Dreieck MAC0 == 0 hat
                 * (Blatt von der Kante, @0x8008ee34 bgtz) oder ganz ausserhalb pih/piv liegt */
                printf("tor: Bild %d: %d ungeteilt sichtbare Dreiecke, DivideGT3 gibt %d aus\n", bild, n_ohne, soll);
                for (int i = 0; i < n_ohne && i < 64; i++)
                    printf("     (%d,%d) (%d,%d) (%d,%d)\n", ohne[i].x[0], ohne[i].y[0], ohne[i].x[1], ohne[i].y[1], ohne[i].x[2], ohne[i].y[2]);
                randfaelle++;
            }
            geprueft++;
        }
        bild++;
    }
    re15_door_seq_ende(&s);
    printf("tor: %d Bilder geprueft, %d davon geteilt, %d ohne Teildreieck (0x0a80 ungeteilt, 0x0aa0 nach DivideGT3)\n",
           geprueft, geteilt_bilder, randfaelle);
    PRUEF(geprueft >= 10 && geteilt_bilder >= 10, "zu wenige Bilder (%d, geteilt %d)", geprueft, geteilt_bilder);
    free(s_arch);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) { printf("Aufruf: probe_r32_unterteilung referenz|tor\n"); return 2; }
    int r = 1;
    if (!strcmp(argv[1], "referenz")) r = teil_referenz();
    else if (!strcmp(argv[1], "tor")) r = teil_tor();
    else { printf("unbekannter Teil %s\n", argv[1]); return 2; }
    if (r != 0) g_fehler++;
    printf("%s: %s (%d Fehler)\n", argv[1], g_fehler ? "ROT" : "GRUEN", g_fehler);
    return g_fehler ? 1 : 0;
}
