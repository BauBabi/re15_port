/**
 * @file test_g5_morph.c
 * @brief PIN — dir[0]-VERTEX-MORPH des G5-Blobs (Runde 7, g5-morph.md).
 *
 * Das Original verformt die Fleischmasse (EM036 Mesh 2) kettenweise gegen vier
 * Ziel-Posen: FUN_8004BF90 @0x8004bf90 rechnet je Vertex
 *     v = ((4095 - w) * v + w * ziel) >> 12      (GTE GPF/GPL, s16-gesaettigt)
 * und stellt vorher die Basis-Pose (Segment 0) wieder her.
 *
 *  PIN 1  FORMAT: EM036 traegt Maske 0x4 (nur Mesh 2), seg0 = emd+0x3C,
 *         seg_size 0xEE4, seg_count 4, nverts 635 - und Segment 0 ist
 *         VERTEX-FUER-VERTEX das MD1-Vertexarray (Basis-Pose, g5-morph.md 2.2).
 *  PIN 2  NEUTRAL: Gewichte {0,0,0,0} liefern exakt die Basis.
 *  PIN 3  REGRESSIONSANKER (integergenau nachgerechnet, g5-morph.md 2.3):
 *         w={8000,0,0,0} -> max |Delta| == 821; w={0,0,4096,0} -> max |Delta| == 5212;
 *         w={0,2895,0,0} -> max |z| == 2358 (SAR; das Dossier nennt 2357, mit
 *         Division gerechnet - s. Kommentar an der Stelle).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_md1.h"
#include "re15_enemy.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail = 1; \
    fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static re15_md1_vertex_t s_out[2048];

int main(void)
{
    printf("=== G5-BLOB: dir[0]-Vertex-Morph (FUN_8004BF90) ===\n");
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("SKIP: shared_assets/RE2/CDEMD0.EMS fehlt\n"); return 0; }

    static re15_enemy_bank_t eb;
    memset(&eb, 0, sizeof eb);
    if (re2_ems_load_bank(ems, n, 0x36, &eb, NULL) != 0) {
        printf("FAIL: EM036 laedt nicht\n"); return 1;
    }

    /* ---- PIN 1: Format ---- */
    CHECK(eb.morph_ok, "EM036 traegt keinen Morph-Block");
    CHECK(eb.morph_mask == 0x4u, "Mesh-Maske 0x%X != 0x4", (unsigned)eb.morph_mask);
    const re15_morph_ctrl_t *c = &eb.morph[2];
    printf("Mesh 2: seg_count=%u seg_size=0x%X nverts=%u\n",
           c->seg_count, (unsigned)c->seg_size, c->nverts);
    CHECK(c->seg_count == 4, "seg_count %u != 4", c->seg_count);
    CHECK(c->seg_size == 0xEE4u, "seg_size 0x%X != 0xEE4", (unsigned)c->seg_size);
    CHECK(c->nverts == 635, "nverts %u != 635", c->nverts);
    /* Segment 0 == MD1-Vertexarray (Basis-Pose). */
    {
        const int16_t *s0 = (const int16_t *)c->seg0;
        const re15_md1_vertex_t *mv = eb.md1.meshes[2].tri_vertices;
        int gleich = 1;
        for (int v = 0; v < c->nverts && gleich; v++)
            if (s0[v*3+0] != mv[v].x || s0[v*3+1] != mv[v].y || s0[v*3+2] != mv[v].z)
                gleich = 0;
        CHECK(gleich, "Segment 0 ist NICHT die MD1-Basis-Pose");
    }

    if (g_fail) { printf("=== FAIL (Format) ===\n"); return 1; }

    /* ---- PIN 2: neutrale Gewichte ---- */
    {
        int16_t w[4] = { 0, 0, 0, 0 };
        int gleich = 1;
        re15_emd_morph_apply(c, w, s_out);
        const re15_md1_vertex_t *mv = eb.md1.meshes[2].tri_vertices;
        for (int v = 0; v < c->nverts && gleich; v++)
            if (s_out[v].x != mv[v].x || s_out[v].y != mv[v].y || s_out[v].z != mv[v].z)
                gleich = 0;
        CHECK(gleich, "w={0,0,0,0} muss exakt die Basis liefern");
    }

    /* ---- PIN 3: Regressionsanker ---- */
    {
        const re15_md1_vertex_t *mv = eb.md1.meshes[2].tri_vertices;
        struct { int16_t w[4]; const char *name; } F[2] = {
            { { 8000, 0, 0, 0 }, "w0=8000 (Puls-Maximum)" },
            { { 0, 0, 4096, 0 }, "w2=4096 (Zerfliessen)" },
        };
        int soll[2] = { 821, 5212 };
        for (int k = 0; k < 2; k++) {
            int32_t maxd = 0;
            re15_emd_morph_apply(c, F[k].w, s_out);
            for (int v = 0; v < c->nverts; v++) {
                int32_t d;
                d = (int32_t)s_out[v].x - mv[v].x; if (d < 0) d = -d; if (d > maxd) maxd = d;
                d = (int32_t)s_out[v].y - mv[v].y; if (d < 0) d = -d; if (d > maxd) maxd = d;
                d = (int32_t)s_out[v].z - mv[v].z; if (d < 0) d = -d; if (d > maxd) maxd = d;
            }
            printf("%-26s max|Delta| = %d (Soll %d)\n", F[k].name, (int)maxd, soll[k]);
            CHECK(maxd == soll[k], "%s: max|Delta| %d != %d", F[k].name, (int)maxd, soll[k]);
        }
        {   int16_t w[4] = { 0, 2895, 0, 0 };
            int32_t maxz = 0;
            re15_emd_morph_apply(c, w, s_out);
            for (int v = 0; v < c->nverts; v++) {
                int32_t z = s_out[v].z; if (z < 0) z = -z; if (z > maxz) maxz = z;
            }
            /* 2358, NICHT die 2357 des Dossiers: der Unterschied ist genau die
             * Rundung. GPF/GPL mit sf=1 schieben ARITHMETISCH (SAR, Abrundung
             * Richtung -unendlich) - das Dossier hat diesen einen Anker mit
             * Division (Trunkierung Richtung 0) gerechnet. Selbst nachgerechnet
             * auf der EMS-Rohtabelle: SAR -> 2358, Division -> 2357; die beiden
             * anderen Anker (821/5212) sind rundungsunabhaengig und treffen exakt. */
            printf("%-26s max|z| = %d (Soll 2358, SAR)\n", "w[1]=2895 (Segment 2)", (int)maxz);
            CHECK(maxz == 2358, "w[1]=2895: max|z| %d != 2358", (int)maxz);
        }
    }

    free(ems);
    if (g_fail) { printf("=== FAIL ===\n"); return 1; }
    printf("=== PASS: Format, Basis-Restore, drei Regressionsanker ===\n");
    return 0;
}
