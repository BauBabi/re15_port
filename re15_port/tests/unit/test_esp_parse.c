/**
 * @file test_esp_parse.c
 * @brief Phase ESP-A — byte-true test of re15_esp_parse against the REAL ROOM1140.RDT.
 *
 * Verifies the ESP-section parser (port of FUN_80019354/8001945c/800194f8) reproduces the
 * exact effect-ID header, the EFF pointer-table walk (id array forward, ptr table DOWNWARD,
 * EFF start = entry+idh, EFF end = (count_a*2+count_b+2) u32 words), and the embedded-TIM
 * enumeration (downward, tim = base + *(--ptr)). All expected values were dumped from the
 * real asset bytes (id header `05 07 FF..`; EFF @0x11E8 size 196, @0x13B8 size 132; TIM
 * @0x1A628 / @0x1CA68, both PSX-TIM magic 0x10).
 */
#include "re15_esp.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static uint32_t u32le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }

    int fail = 0;
    printf("=== ESP-section parse (Phase ESP-A) vs real ROOM1140.RDT ===\n");

    /* The four RDT directory slots feeding FUN_80019354. */
    uint32_t idh      = u32le(&buf[0x4C]);
    uint32_t ptr_end  = u32le(&buf[0x50]);
    uint32_t tim_base = u32le(&buf[0x54]);
    uint32_t tim_end  = u32le(&buf[0x58]);

    re15_esp_t esp;
    int rc = re15_esp_parse(buf, (size_t)sz, idh, ptr_end, tim_base, tim_end, &esp);
    if (rc != 0) { fprintf(stderr, "FAIL: re15_esp_parse rc=%d\n", rc); free(buf); return 1; }

    /* (1) effect-ID header: 05 07 FF.. -> 2 used ids. */
    if (esp.id_count != 2) {
        fprintf(stderr, "FAIL: id_count expected 2, got %d\n", esp.id_count); fail = 1; }

    /* (2) per-EFF byte-true resolution (dumped from the asset). */
    struct { uint8_t id; uint32_t start, end; uint16_t a, b; uint32_t tim; } exp[2] = {
        { 0x05, 0x11E8, 0x12AC,  9, 29, 0x1A628 },
        { 0x07, 0x13B8, 0x143C, 11,  9, 0x1CA68 },
    };
    for (int i = 0; i < 2 && i < esp.id_count; i++) {
        re15_esp_eff_t *e = &esp.eff[i];
        if (e->effect_id != exp[i].id || e->eff_start != exp[i].start || e->eff_end != exp[i].end ||
            e->count_a != exp[i].a || e->count_b != exp[i].b) {
            fprintf(stderr, "FAIL: eff[%d] id=0x%02x start=0x%x end=0x%x a=%u b=%u "
                            "(expected id=0x%02x start=0x%x end=0x%x a=%u b=%u)\n",
                    i, e->effect_id, e->eff_start, e->eff_end, e->count_a, e->count_b,
                    exp[i].id, exp[i].start, exp[i].end, exp[i].a, exp[i].b);
            fail = 1;
        }
        /* (3) embedded TIM offset + the PSX-TIM magic (0x00000010) at that offset. */
        if (e->tim_off != exp[i].tim) {
            fprintf(stderr, "FAIL: eff[%d] tim_off=0x%x expected 0x%x\n", i, e->tim_off, exp[i].tim);
            fail = 1;
        } else if (u32le(&buf[e->tim_off]) != 0x00000010u) {
            fprintf(stderr, "FAIL: eff[%d] tim@0x%x not a TIM (magic=0x%08x)\n",
                    i, e->tim_off, u32le(&buf[e->tim_off])); fail = 1;
        }
        /* size sanity: (a*2+b+2)*4 == end-start. */
        if (e->eff_end - e->eff_start != ((uint32_t)e->count_a * 2u + e->count_b + 2u) * 4u) {
            fprintf(stderr, "FAIL: eff[%d] size formula mismatch\n", i); fail = 1; }
    }

    /* (4) negative guard: a buffer whose id-header first word is the 0xFFFFFFFF "no effects"
     * marker must return -2 (FUN_80019354 guard *piVar4 != -1). */
    {
        uint8_t nofx[16];
        for (int i = 0; i < 16; i++) nofx[i] = 0xFF;
        re15_esp_t e2;
        if (re15_esp_parse(nofx, sizeof(nofx), 0, 8, 0, 0, &e2) != -2) {
            fprintf(stderr, "FAIL: empty-ESP guard should return -2\n"); fail = 1; }
    }

    if (!fail)
        printf("  PASS: 2 effect ids {05,07}; EFF @0x11E8(196B)/0x13B8(132B); TIM @0x1A628/0x1CA68 (magic 0x10)\n");

    free(buf);
    if (fail) { fprintf(stderr, "\nESP-PARSE TEST FAILED\n"); return 1; }
    printf("\nPASS: ESP-section parse byte-true vs ROOM1140.RDT\n");
    return 0;
}
