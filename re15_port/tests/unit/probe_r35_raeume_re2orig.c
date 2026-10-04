/* probe_r35_raeume_re2orig.c — MESSUNG (kein add_test): Runde 35 Spur H, Nachbesserung 3.
 * Das Volumenmass der Riegel (r35_raeume_volumen.h: Arm-Vertices Unterarm + Hand in Leons Kopf/Rumpf) auf
 * dem RE2-ORIGINAL-RAM eines Griffs ROOM2050 (DuckStation-GDB, analysis/befunde_runde35/H_raeume/re2_mess/
 * re2_gdb_grab.py -> frames.bin: je Halte-Bild PL + Leons Parts + Halter + dessen Parts).
 * Part-Welt-Matrix +0x48 (m[3][3] s16 Q12) + Translation +0x5C/+0x60/+0x64 s32 — dieselbe, die B4 P0 als Pin
 * liest (@0x80100C18-38) und die das Zeichnen FUN_80027434 schreibt. Welt -> Knochen: lokal = M^T (P - t).
 * Aufruf: probe_r35_raeume_re2orig <frames.bin> [...] */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_md1.h"
#include "re2_ems.h"
#include "r35_raeume_volumen.h"

#define PART_N 16
#define PART_SZ 0xAC
#define ENT_SZ 0x248
#define REC (12 + ENT_SZ + PART_N * PART_SZ + ENT_SZ + PART_N * PART_SZ)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = sz; return b;
}
static int16_t rd16(const uint8_t *p) { return (int16_t)(p[0] | (p[1] << 8)); }
static int32_t rd32(const uint8_t *p) { return (int32_t)((uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24)); }

/* zweite Leon-Huelle aus einem beliebigen MD1 (RE2-PL00 = das Modell im Original-RAM) */
static vol_t s_vol2[2];
static int vol_aus_md1(const char *pfad, vol_t out[2])
{
    long n = 0; uint8_t *b = slurp(pfad, &n); static re15_md1_t md;
    if (!b || re15_md1_parse(b, (int)n, &md) != 0) return 0;
    const int mesh[2] = { 0, 8 };
    for (int k = 0; k < 2; k++) {
        vol_t *v = &out[k]; memset(v, 0, sizeof *v); v->ymin = 1 << 30;
        for (int pass = 0; pass < 2; pass++) {
            vol_add(v, md.meshes[mesh[k]].tri_vertices, md.meshes[mesh[k]].tri_vertex_count, pass);
            vol_add(v, md.meshes[mesh[k]].quad_vertices, md.meshes[mesh[k]].quad_vertex_count, pass);
        }
    }
    return out[0].nb > 0 && out[1].nb > 0;
}

int main(int argc, char **argv)
{
    long sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &sz);
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x1A);
    if (!ems || !eb || re2_ems_load_bank(ems, (size_t)sz, 0x2D, eb, NULL) != 0) { printf("FAIL: EM2D\n"); return 1; }
    if (!vol_laden(RE15_ASSET_PSX_DIR "/PLD/PL00.MD1")) { printf("FAIL: PL00.MD1\n"); return 1; }
    int re2pl = vol_aus_md1(RE15_ASSET_PSX_DIR "/../../../info/re2leon/PL0/PLD/PL00/PL00.md1", s_vol2);
    printf("Leon-Huelle RE1.5-PL00 (Port) geladen; RE2-PL00 (Original-Modell) %s\n", re2pl ? "geladen" : "FEHLT");
    for (int a = 1; a < argc; a++) {
        long n = 0; uint8_t *d = slurp(argv[a], &n);
        if (!d) { printf("FAIL: %s\n", argv[a]); continue; }
        int nf = (int)(n / REC);
        int z15_all = 0, z15_cyc = 0, z2_cyc = 0, mx15 = 0, mx2 = 0, cyc = 0;
        const uint8_t *e0 = d + 12, *h0 = d + 12 + ENT_SZ + PART_N * PART_SZ;
        int var = rd16(h0 + 0x10E) & 1, hb = var ? 10 : 3;
        printf("== %s: %d Halte-Bilder, Halter %08x Arm %c (Hand-Bone %d) Ursprung (%d,%d,%d) Blick %d | Leon (%d,%d,%d) Blick %d\n",
               argv[a], nf, (unsigned)rd32(d + 8), var ? 'B' : 'A', hb, rd32(h0 + 0x38), rd32(h0 + 0x3C), rd32(h0 + 0x40),
               rd16(h0 + 0x76), rd32(e0 + 0x38), rd32(e0 + 0x3C), rd32(e0 + 0x40), rd16(e0 + 0x76));
        for (int f = 0; f < nf; f++) {
            const uint8_t *r = d + (size_t)f * REC + 12;
            const uint8_t *pl = r, *lp = r + ENT_SZ, *he = lp + PART_N * PART_SZ, *hp = he + ENT_SZ;
            int16_t LM[2][9]; int32_t LT[2][3];
            const int lk[2] = { 0, 8 };
            for (int k = 0; k < 2; k++) {
                for (int i = 0; i < 9; i++) LM[k][i] = rd16(lp + lk[k] * PART_SZ + 0x48 + 2 * i);
                for (int i = 0; i < 3; i++) LT[k][i] = rd32(lp + lk[k] * PART_SZ + 0x5C + 4 * i);
            }
            int in15 = 0, in2 = 0;
            int32_t hand[3] = { rd32(hp + hb * PART_SZ + 0x5C), rd32(hp + hb * PART_SZ + 0x60), rd32(hp + hb * PART_SZ + 0x64) };
            for (int bb = hb - 1; bb <= hb; bb++) {
                int16_t M[9]; int32_t T[3];
                for (int i = 0; i < 9; i++) M[i] = rd16(hp + bb * PART_SZ + 0x48 + 2 * i);
                for (int i = 0; i < 3; i++) T[i] = rd32(hp + bb * PART_SZ + 0x5C + 4 * i);
                const re15_md1_mesh_t *m = &eb->md1.meshes[bb];
                for (int q = 0; q < 2; q++) {
                    const re15_md1_vertex_t *vv = q ? m->quad_vertices : m->tri_vertices;
                    int nv = q ? m->quad_vertex_count : m->tri_vertex_count;
                    for (int i = 0; i < nv; i++) {
                        int32_t P[3];
                        for (int j = 0; j < 3; j++)
                            P[j] = T[j] + (int32_t)(((int64_t)M[3*j]*vv[i].x + (int64_t)M[3*j+1]*vv[i].y + (int64_t)M[3*j+2]*vv[i].z) >> 12);
                        if (vol_innen_welt(&s_vol[0], LM[0], LT[0], P) || vol_innen_welt(&s_vol[1], LM[1], LT[1], P)) in15++;
                        if (re2pl && (vol_innen_welt(&s_vol2[0], LM[0], LT[0], P) || vol_innen_welt(&s_vol2[1], LM[1], LT[1], P))) in2++;
                    }
                }
            }
            /* Hand relativ zu Leons Kopfknochen (Knochen-Rahmen, Q12) */
            int64_t dx = hand[0] - LT[1][0], dy = hand[1] - LT[1][1], dz = hand[2] - LT[1][2];
            int lx = (int)((LM[1][0]*dx + LM[1][3]*dy + LM[1][6]*dz) >> 12);
            int ly = (int)((LM[1][1]*dx + LM[1][4]*dy + LM[1][7]*dz) >> 12);
            int lz = (int)((LM[1][2]*dx + LM[1][5]*dy + LM[1][8]*dz) >> 12);
            uint32_t pcw = (uint32_t)rd32(pl + 0x14C), acw = (uint32_t)rd32(he + 0x14C);
            printf("  f%2d Leon Clip %u Bild %2u Blend %2u | Arm Clip %u Bild %2u Blend %2u | Hand im Kopf-Rahmen (%5d,%5d,%5d) | "
                   "Arm-Vertices in Leon: PL00-RE1.5 %2d, PL00-RE2 %2d\n", f, pcw & 0xff, (pcw >> 8) & 0xff, (pcw >> 16) & 0xff,
                   acw & 0xff, (acw >> 8) & 0xff, (acw >> 16) & 0xff, lx, ly, lz, in15, in2);
            if (in15) z15_all++;
            if (f >= 16 && cyc < 19) { cyc++; if (in15) z15_cyc++; if (in2) z2_cyc++;
                                       if (in15 > mx15) mx15 = in15; if (in2 > mx2) mx2 = in2; }
        }
        printf("  ERGEBNIS %s: Halte-Zyklus nach der Ueberblendung (Bilder 16..34, %d Bilder): Arm in Leon (Huelle PL00-RE1.5) "
               "%d/%d (max %d Vertices), (Huelle PL00-RE2) %d/%d (max %d); alle %d Bilder: %d\n",
               argv[a], cyc, z15_cyc, cyc, mx15, z2_cyc, cyc, mx2, nf, z15_all);
        free(d);
    }
    return 0;
}
