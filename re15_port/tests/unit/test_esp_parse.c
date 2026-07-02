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
#include "re15_tim.h"

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

/* ESP-B pool dispatch recorder. */
static int s_disp[RE15_ESP_MAX_SLOTS];
static int s_disp_n;
static void rec_handler(re15_esp_slot_t *slot, int index)
{
    (void)slot;
    if (s_disp_n < RE15_ESP_MAX_SLOTS) s_disp[s_disp_n++] = index;
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
        printf("  (A) PASS: 2 effect ids {05,07}; EFF @0x11E8(196B)/0x13B8(132B); TIM @0x1A628/0x1CA68 (magic 0x10)\n");

    /* === Phase ESP-C: EFF clip record accessors (anim records + sprite-coord cells) ===
     * All expected values dumped from the real ROOM1140.RDT EFF bodies. */
    {
        re15_esp_anim_t  an;
        re15_esp_coord_t co;
        int cfail = 0;

        /* effect 05 (eff[0]): 9 anim + 29 coord records. */
        if (re15_esp_anim(&esp, 0, 0, &an) != 0 || an.desc != 0x0100 || an.param != 0x1801 || an.rsv != 0) {
            fprintf(stderr, "FAIL: (C) eff05 anim[0]={0x%04x,0x%04x,0x%x}\n", an.desc, an.param, an.rsv); cfail = 1; }
        if (re15_esp_anim(&esp, 0, 1, &an) != 0 || an.desc != 0x0401) {
            fprintf(stderr, "FAIL: (C) eff05 anim[1].desc=0x%04x (exp 0x0401)\n", an.desc); cfail = 1; }
        if (re15_esp_anim(&esp, 0, 8, &an) != 0 || an.desc != 0x0000) {
            fprintf(stderr, "FAIL: (C) eff05 anim[8] terminator desc=0x%04x\n", an.desc); cfail = 1; }
        if (re15_esp_coord(&esp, 0, 1, &co) != 0 || co.u != 24 || co.v != 0 || co.w != 232 || co.h != 232) {
            fprintf(stderr, "FAIL: (C) eff05 coord[1]={%u,%u,%u,%u}\n", co.u, co.v, co.w, co.h); cfail = 1; }

        /* effect 07 (eff[1]): 11 anim + 9 coord; anim[0].param=0x2003, coord[1]={32,0,240,240}. */
        if (re15_esp_anim(&esp, 1, 0, &an) != 0 || an.desc != 0x0100 || an.param != 0x2003) {
            fprintf(stderr, "FAIL: (C) eff07 anim[0]={0x%04x,0x%04x}\n", an.desc, an.param); cfail = 1; }
        if (re15_esp_coord(&esp, 1, 1, &co) != 0 || co.u != 32 || co.v != 0 || co.w != 240 || co.h != 240) {
            fprintf(stderr, "FAIL: (C) eff07 coord[1]={%u,%u,%u,%u}\n", co.u, co.v, co.w, co.h); cfail = 1; }

        /* bounds: out-of-range index returns -1, not 0. */
        if (re15_esp_anim (&esp, 0,  9, &an) != -1) { fprintf(stderr, "FAIL: (C) anim OOB !=-1\n");  cfail = 1; }
        if (re15_esp_coord(&esp, 0, 29, &co) != -1) { fprintf(stderr, "FAIL: (C) coord OOB !=-1\n"); cfail = 1; }

        if (cfail) fail = 1;
        else printf("  (C) PASS: EFF clip records byte-true (eff05 9 anim/29 cells; eff07 11/9; desc/param + coord cells)\n");
    }

    /* === Phase ESP-C: op-0x3a particle pool — spawn + byte-true anim cycling === */
    {
        int pfail = 0;
        re15_esp_fx_reset();
        if (re15_esp_fx_count() != 0) { fprintf(stderr, "FAIL: (C2) fx reset -> 0\n"); pfail = 1; }

        /* spawn effect 0x05 at a world position; resolves to eff[0]. */
        re15_esp_fx_t *fx = re15_esp_fx_spawn(&esp, 0x05, 2, 1000, 2000, 3000, 0x55);
        if (!fx || re15_esp_fx_count() != 1 || fx->effect_id != 0x05 || fx->sub_index != 2 ||
            fx->eff_idx != 0 || fx->x != 1000 || fx->y != 2000 || fx->z != 3000 || fx->param != 0x55) {
            fprintf(stderr, "FAIL: (C2) fx spawn fields\n"); pfail = 1; }

        /* effect 05: dur=1/frame, terminator anim record at frame 8 -> ends at the 8th tick. */
        for (int k = 0; k < 7; k++) re15_esp_fx_tick(&esp);
        if (re15_esp_fx_count() != 1) { fprintf(stderr, "FAIL: (C2) eff05 ended too early (k=7)\n"); pfail = 1; }
        for (int k = 0; k < 4; k++) re15_esp_fx_tick(&esp);
        if (re15_esp_fx_count() != 0) { fprintf(stderr, "FAIL: (C2) eff05 should end at terminator\n"); pfail = 1; }

        /* effect 07 LOOPS (anim[5].param low byte = 0xFF -> loop to desc-low 0) -> never ends. */
        re15_esp_fx_reset();
        re15_esp_fx_spawn(&esp, 0x07, 0, 0, 0, 0, 0);
        for (int k = 0; k < 60; k++) re15_esp_fx_tick(&esp);
        if (re15_esp_fx_count() != 1) { fprintf(stderr, "FAIL: (C2) eff07 should loop forever\n"); pfail = 1; }

        /* unresolved bank (NULL) -> eff_idx -1 -> first tick despawns. */
        re15_esp_fx_reset();
        re15_esp_fx_spawn(NULL, 0x05, 0, 0, 0, 0, 0);
        re15_esp_fx_tick(NULL);
        if (re15_esp_fx_count() != 0) { fprintf(stderr, "FAIL: (C2) unresolved bank should despawn\n"); pfail = 1; }

        re15_esp_fx_reset();
        if (pfail) fail = 1;
        else printf("  (C2) PASS: op-0x3a fx pool spawn + byte-true anim (eff05 terminates@8, eff07 loops@5->0)\n");
    }

    /* === Phase ESP-B: pool spawn + byte-true AABB-cull dispatch (FUN_8004d5f0) + lifetime === */
    {
        re15_esp_pool_reset();
        if (re15_esp_pool_count() != 0) { fprintf(stderr, "FAIL: (B) reset -> count 0\n"); fail = 1; }

        /* A: AABB [100..140]x[50..80]; B: tiny box far away. */
        re15_esp_slot_t *a = re15_esp_spawn(3, 100, 50, 40, 30, 0);
        re15_esp_slot_t *b = re15_esp_spawn(1, 200, 80, 10, 10, 0);
        if (!a || !b || re15_esp_pool_count() != 2) {
            fprintf(stderr, "FAIL: (B) spawn 2 -> count %d\n", re15_esp_pool_count()); fail = 1; }

        /* cull point (110,60) is inside A only -> exactly 1 dispatch, slot 0. */
        s_disp_n = 0;
        int n = re15_esp_run(110, 60, rec_handler);
        if (n != 1 || s_disp_n != 1 || s_disp[0] != 0) {
            fprintf(stderr, "FAIL: (B) run(110,60) dispatched=%d (expected 1, slot 0)\n", n); fail = 1; }

        /* byte-true unsigned AABB edges: px-x must be in [0,w]. (140-100=40<=40 hit; 141 miss;
         * 99 -> (u32)(-1) huge -> miss). */
        s_disp_n = 0; if (re15_esp_run(140, 50, rec_handler) != 1) {
            fprintf(stderr, "FAIL: (B) AABB right edge (px-x==w) should hit\n"); fail = 1; }
        s_disp_n = 0; if (re15_esp_run(141, 50, rec_handler) != 0) {
            fprintf(stderr, "FAIL: (B) AABB px-x==w+1 should miss\n"); fail = 1; }
        s_disp_n = 0; if (re15_esp_run(99, 50, rec_handler) != 0) {
            fprintf(stderr, "FAIL: (B) AABB px<x (unsigned wrap) should miss\n"); fail = 1; }

        /* lifetime: a duration=1 slot despawns after one run that ticks it. */
        re15_esp_pool_reset();
        re15_esp_spawn(3, 0, 0, 100, 100, 1);
        if (re15_esp_run(10, 10, NULL) != 1) { fprintf(stderr, "FAIL: (B) dur slot first run\n"); fail = 1; }
        if (re15_esp_pool_count() != 0) { fprintf(stderr, "FAIL: (B) duration=1 -> despawn after 1 run\n"); fail = 1; }
        if (re15_esp_run(10, 10, NULL) != 0) { fprintf(stderr, "FAIL: (B) despawned slot still dispatched\n"); fail = 1; }

        re15_esp_pool_reset();
        if (!fail)
            printf("  (B) PASS: pool spawn/count, byte-true AABB-cull dispatch, duration despawn, reset\n");
    }

    /* === Phase ESP-D: the GLOBAL effect bank CORE00.ESP (universal hit effects, effect-id 0) === */
    {
        const char *gpath = RE15_ASSET_PSX_DIR "/DATA/CORE00.ESP";
        long gsz = 0;
        uint8_t *gbuf = slurp(gpath, &gsz);
        if (!gbuf) { fprintf(stderr, "FAIL: (D) cannot open %s\n", gpath); fail = 1; }
        else {
            re15_esp_t gesp;
            int dfail = 0;
            if (re15_esp_parse_global(gbuf, (size_t)gsz, &gesp) != 0) {
                fprintf(stderr, "FAIL: (D) re15_esp_parse_global failed\n"); dfail = 1; }
            if (gesp.id_count != 5) {                                  /* ids {3,8,0,2,4} */
                fprintf(stderr, "FAIL: (D) id_count=%d (exp 5)\n", gesp.id_count); dfail = 1; }
            int i0 = re15_esp_find_id(&gesp, 0x00);                     /* the universal hit effects */
            if (i0 < 0 || gesp.eff[i0].eff_start != 0x824 ||
                gesp.eff[i0].count_a != 22 || gesp.eff[i0].count_b != 20) {
                fprintf(stderr, "FAIL: (D) effect-0 @0x824/22/20 (got i=%d start=0x%x a=%u b=%u)\n",
                        i0, i0 >= 0 ? gesp.eff[i0].eff_start : 0,
                        i0 >= 0 ? gesp.eff[i0].count_a : 0, i0 >= 0 ? gesp.eff[i0].count_b : 0); dfail = 1; }

            /* fx spawn resolves effect-0 from the GLOBAL bank when the room bank lacks it. */
            re15_esp_set_room_bank(NULL);
            re15_esp_set_global_bank(&gesp);
            re15_esp_fx_reset();
            re15_esp_fx_t *fx0 = re15_esp_fx_spawn(NULL, 0x00, 0, 10, 20, 30, 0);
            if (!fx0 || fx0->eff_idx < 0 || fx0->bank != &gesp) {
                fprintf(stderr, "FAIL: (D) effect-0 should resolve from the global bank\n"); dfail = 1; }
            /* effect-05 isn't in CORE00 (or the NULL room) -> stays unresolved. */
            re15_esp_fx_reset();
            re15_esp_fx_t *fx5 = re15_esp_fx_spawn(NULL, 0x05, 0, 0, 0, 0, 0);
            if (!fx5 || fx5->eff_idx >= 0) {
                fprintf(stderr, "FAIL: (D) effect-05 should be unresolved (not in CORE00/NULL room)\n"); dfail = 1; }

            re15_esp_fx_reset();
            re15_esp_set_global_bank(NULL);
            free(gbuf);
            if (dfail) fail = 1;
            else printf("  (D) PASS: CORE00.ESP global bank {3,8,0,2,4}; effect-0 @0x824 (22a/20c); fx resolves effect-0 from global\n");
        }
    }

    /* === Phase ESP-E: the effect-0 blood TEXTURE (extracted_fx/effect0_blood.tim, byte-true VRAM) ===
     * The GLOBAL effect sheet is VRAM-only; extracted byte-true (tpage 0x001f -> VRAM(960,256) 4bpp,
     * clut 0x7951 -> VRAM(272,485)). Verify it parses + is a blood sprite: CLUT[0]=0x0000 (index-0
     * TRANSPARENT — the port's effect slot treats it transparent, not opaque black), red-dominant
     * palette, and the sheet is a mostly-transparent sprite with substantial blood texels. */
    {
        const char *tpath = RE15_ASSET_PSX_DIR "/../extracted_fx/effect0_blood.tim";
        long tsz = 0;
        uint8_t *tb = slurp(tpath, &tsz);
        if (!tb) { fprintf(stderr, "FAIL: (E) cannot open %s\n", tpath); fail = 1; }
        else {
            re15_tim_t tim;
            int efail = 0;
            if (re15_tim_parse(tb, (int)tsz, &tim) != 0) {
                fprintf(stderr, "FAIL: (E) re15_tim_parse failed\n"); efail = 1; }
            else if (tim.width != 256 || tim.height != 256 || !tim.has_clut) {
                fprintf(stderr, "FAIL: (E) exp 256x256 with CLUT, got %dx%d clut=%d\n",
                        tim.width, tim.height, tim.has_clut); efail = 1; }
            else if (tim.clut[0] != 0x0000) {
                fprintf(stderr, "FAIL: (E) CLUT[0]=0x%04x (exp 0x0000 = transparent index)\n", tim.clut[0]); efail = 1; }
            else {
                /* red-dominant palette (blood) + mostly index-0 (transparent bg) for a 4bpp sheet. */
                int redcol = 0;
                for (int k = 1; k < 16 && k < tim.clut_entries; k++) {
                    uint16_t c = tim.clut[k]; int r = c & 0x1f, g = (c >> 5) & 0x1f, b = (c >> 10) & 0x1f;
                    if (c && r >= g && r >= b) redcol++;
                }
                if (redcol < 8) { fprintf(stderr, "FAIL: (E) only %d red-dominant CLUT entries (exp blood palette)\n", redcol); efail = 1; }
                if (tim.bpp == 4) {
                    const uint8_t *idx = (const uint8_t *)tim.pixels;
                    long clear = 0, blood = 0;
                    int npx = tim.width * tim.height;
                    for (int i = 0; i < npx; i++) {
                        int nib = (i & 1) ? (idx[i / 2] >> 4) : (idx[i / 2] & 0xF);
                        if (nib == 0) clear++; else blood++;
                    }
                    if (clear < npx / 2 || blood < 1000) {
                        fprintf(stderr, "FAIL: (E) index dist %ld transparent / %ld blood (exp majority transparent, >=1000 blood)\n",
                                clear, blood); efail = 1; }
                    if (!efail)
                        printf("  (E) PASS: effect0_blood.tim 256x256 %dbpp; CLUT[0]=transparent + %d red-dominant; %ld transparent / %ld blood texels\n",
                               tim.bpp, redcol, clear, blood);
                } else if (!efail) {
                    printf("  (E) PASS: effect0_blood.tim 256x256 %dbpp; CLUT[0]=transparent + %d red-dominant blood entries\n",
                           tim.bpp, redcol);
                }
            }
            free(tb);
            if (efail) fail = 1;
        }
    }

    free(buf);
    if (fail) { fprintf(stderr, "\nESP-PARSE TEST FAILED\n"); return 1; }
    printf("\nPASS: ESP-section parse byte-true vs ROOM1140.RDT\n");
    return 0;
}
