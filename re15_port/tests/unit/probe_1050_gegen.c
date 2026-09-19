/*
 * GEGENPRUEFUNG (Verifikationsauftrag 2026-09-14) zum Befund
 *   "ROOM1050.MSK Sektion @0xD38 - 101 Rechtecke, 25 verschiedene Tiefen".
 *
 * AENDERT NICHTS. Laeuft durch den ECHTEN Port-Parser (re15_pri_parse_section /
 * re15_pri_msk_section_offset) und gibt jede der 101 Kacheln maschinenlesbar aus,
 * damit sie gegen eine unabhaengige Python-Zerlegung derselben Bytes diffbar ist.
 */
#include "re15_pri.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *rel, long *n)
{
    char path[512];
    snprintf(path, sizeof path, "%s/%s", RE15_ASSET_PSX_DIR, rel);
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", path); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (b && fread(b, 1, (size_t)s, f) != (size_t)s) { free(b); b = NULL; }
    fclose(f); if (n) *n = s; return b;
}

int main(void)
{
    /* (1) Original-RDT: fuehrt Cut 6 wirklich eine NULL-Sektion? */
    long rn = 0; uint8_t *rdt = slurp("STAGE1/ROOM1050.RDT", &rn);
    if (!rdt) return 1;
    unsigned nCut = rdt[1];
    unsigned cam  = (unsigned)(rdt[0x24] | (rdt[0x25]<<8) | (rdt[0x26]<<16) | ((unsigned)rdt[0x27]<<24));
    printf("RDT nCut=%u hdr7=%u camStart=0x%X\n", nCut, rdt[7], cam);
    int null_cuts = 0;
    for (unsigned c = 0; c < nCut; c++) {
        unsigned o = cam + c*32 + 0x1C;
        unsigned pri = (unsigned)(rdt[o] | (rdt[o+1]<<8) | (rdt[o+2]<<16) | ((unsigned)rdt[o+3]<<24));
        re15_pri_cut_t t = {0};
        int k = re15_pri_parse_section(rdt, (size_t)rn, pri, &t);
        unsigned char *q = rdt + pri;
        printf("RDT cut %u pri=0x%08X bytes=%02X %02X %02X %02X -> %d\n",
               c, pri, q[0], q[1], q[2], q[3], k);
        if (k == 0) null_cuts++;
    }
    printf("RDT NULL-Cuts: %d von %u\n", null_cuts, nCut);

    /* (2) Der R15M-Nachschlag fuer Cut 6 durch den echten Port-Parser. */
    long mn = 0; uint8_t *msk = slurp("MASKS/ROOM1050.MSK", &mn);
    if (!msk) { free(rdt); return 1; }
    printf("MSK Groesse=%ld magic=%c%c%c%c\n", mn, msk[0], msk[1], msk[2], msk[3]);
    for (int c = 0; c < 10; c++)
        printf("MSK cut %d -> 0x%X\n", c, re15_pri_msk_section_offset(msk, (size_t)mn, c));

    uint32_t sect = re15_pri_msk_section_offset(msk, (size_t)mn, 6);
    re15_pri_cut_t p = {0};
    int n = re15_pri_parse_section(msk, (size_t)mn, sect, &p);
    printf("SEKTION @0x%X kopf=%02X %02X %02X %02X gebaut=%d gezeichnet=%d cap=%d\n",
           sect, msk[sect], msk[sect+1], msk[sect+2], msk[sect+3],
           n, p.draw_count, RE15_PRI_MAX_MASKS_PER_CUT);

    /* (3) Jede Kachel einzeln. */
    int dset[4096]; int ndep = 0;
    for (int i = 0; i < n; i++) {
        const re15_pri_mask_t *m = &p.masks[i];
        printf("T %3d src=%3u,%-3u dst=%4d,%-4d %2ux%-2u depth=%u\n",
               i, m->srcX, m->srcY, (int)(int16_t)m->dstX, (int)(int16_t)m->dstY,
               m->width, m->height, m->depth);
        int seen = 0;
        for (int k = 0; k < ndep; k++) if (dset[k] == m->depth) { seen = 1; break; }
        if (!seen && ndep < 4096) dset[ndep++] = m->depth;
    }
    printf("DISTINCT-DEPTHS %d\n", ndep);
    free(msk); free(rdt);
    return 0;
}
