/* =============================================================================
 * test_re2_enemse — RE2-Gegner-SE-Infrastruktur WELLE A gegen die ECHTEN Bytes:
 * die vendorte ENEMSE-Bank-TOC (EXE @0x800a7b1c, Datei 0x9831C; Loader
 * FUN_8005a09c) gegen shared_assets/RE2/ENEMSE.VBS, das EDT-Record-Layout
 * (SE-Map @0, VH "pBAV" @u32[size-8]) und die byte-true Map-Eintrag-Dekodierung
 * (FUN_8005bd6c) an real gedumpten Eintraegen der Baenke 0 und 11.
 * ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_ems.h"
#include "re15_vab.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int g_fail = 0;
#define CHECK(cond, msg, ...) do { if (!(cond)) { \
    fprintf(stderr, "FAIL: " msg "\n", ##__VA_ARGS__); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *path, size_t *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (out_sz) *out_sz = (size_t)sz;
    return b;
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

int main(void)
{
    printf("=== RE2 ENEMSE.VBS Bank-TOC + SE-Map (WELLE A) ===\n");

    /* (1) TOC-Anker (Rohbyte-verifiziert @EXE-Datei 0x9831C):
     *     Bank 0  = EDT{0xCA8 @s0}  VBD{0x11FE0 @s2}
     *     Bank 11 = EDT{0xCA8 @s0x193} VBD @s0x195 (EDT belegt 2 Sektoren, kontig)
     *     Bank 72 (letzte) = EDT{0xEA8 @s3220} VBD{0x17060 @s3222} -> endet @Sektor 3269. */
    re2_enemse_rec_t r;
    CHECK(re2_enemse_toc_entry(0, &r) == 0, "Bank 0 nicht lesbar");
    CHECK(r.edt_size == 0xCA8 && r.edt_off == 0 && r.vbd_size == 0x11FE0 && r.vbd_off == 2*0x800u,
          "Bank 0 = EDT(0x%X@0x%X) VBD(0x%X@0x%X)", r.edt_size, r.edt_off, r.vbd_size, r.vbd_off);
    re2_enemse_rec_t r11;
    CHECK(re2_enemse_toc_entry(11, &r11) == 0 && r11.edt_size == 0xCA8 &&
          r11.edt_off == 0x193*0x800u && r11.vbd_off == 0x195*0x800u,
          "Bank 11 = EDT(0x%X@0x%X) VBD(@0x%X)", r11.edt_size, r11.edt_off, r11.vbd_off);
    re2_enemse_rec_t rl;
    CHECK(re2_enemse_toc_entry(RE2_ENEMSE_BANK_COUNT-1, &rl) == 0 &&
          rl.edt_size == 0xEA8 && rl.edt_off == 3220*0x800u &&
          rl.vbd_size == 0x17060 && rl.vbd_off == 3222*0x800u,
          "Bank 72 = EDT(0x%X@0x%X) VBD(0x%X@0x%X)", rl.edt_size, rl.edt_off, rl.vbd_size, rl.vbd_off);
    CHECK(re2_enemse_toc_entry(RE2_ENEMSE_BANK_COUNT, &r) == -1 &&
          re2_enemse_toc_entry(-1, &r) == -1, "Bank-Grenzen 0..72");
    if (!g_fail) printf("  (1) TOC: Bank 0/11/72 + Grenzen OK\n");

    /* (2) gegen die echte Datei. */
    size_t vbs_sz = 0;
    uint8_t *vbs = slurp(RE15_ASSET_RE2_DIR "/ENEMSE.VBS", &vbs_sz);
    if (!vbs) {
        fprintf(stderr, "FAIL: %s/ENEMSE.VBS nicht lesbar (Nutzer-Entscheidung: Assets liegen dort)\n",
                RE15_ASSET_RE2_DIR);
        return 1;
    }
    CHECK(vbs_sz == 0x662800, "VBS-Groesse 0x%zX != 0x662800", vbs_sz);
    /* Letzte Bank endet exakt am Dateiende (3222 + ceil(0x17060/0x800) = 3269 Sektoren). */
    CHECK(rl.vbd_off + ((rl.vbd_size + 0x7FFu) & ~0x7FFu) == vbs_sz,
          "letzte Bank endet nicht am VBS-Ende");

    /* (3) EDT-Record Bank 11: VH-Offset-Trailer + "pBAV" + SE-Map-Rohbytes.
     *     FUN_8005a09c: VH = base + u32@[size-8]; Bank 11 real: 0x80. */
    const uint8_t *edt11 = vbs + r11.edt_off;
    uint32_t vh_off = rd32(edt11 + r11.edt_size - 8);
    CHECK(vh_off == 0x80, "Bank-11 VH-Offset 0x%X != 0x80", vh_off);
    CHECK(memcmp(edt11 + vh_off, "pBAV", 4) == 0, "Bank-11 VH-Magic != pBAV");
    re15_vab_t vab;
    CHECK(re15_vab_parse(edt11 + vh_off, r11.edt_size - vh_off, &vab) == 0,
          "Bank-11 VH parst nicht");
    /* Map-Eintrag 0 (Rohbytes 00 00 11 02 = 0x02110000, selbst gedumpt) -> Tone prog0/1;
     * dessen VH-Tone muss einen gueltigen 1-basierten vag_index tragen. */
    CHECK(rd32(edt11 + 0) == 0x02110000u, "Bank-11 Map[0] = 0x%08X != 0x02110000", rd32(edt11));
    CHECK(rd32(edt11 + 11*4) == 0xFFFFFFFFu, "Bank-11 Map[11] nicht -1 (stumm)");
    CHECK(vab.tones_loaded && vab.tones[0*16 + 1].vag_index >= 1,
          "Bank-11 prog0/tone1 ohne VAG");
    if (!g_fail) printf("  (3) Bank 11: vh_off 0x80, pBAV, VH %d VAGs, Map[0]=0x02110000\n",
                        vab.vag_count);

    /* (4) Map-Dekodierung byte-true FUN_8005bd6c an echten Eintraegen. */
    re2_enemse_se_t se;
    re2_enemse_decode_entry(0x02110000u, &se);          /* Bank 11 Eintrag 0 */
    CHECK(!se.silent && se.vab_override == -1 && se.prog == 0 && se.tone == 1 &&
          se.prio == 1 && se.chan == 2 && se.extra == 0,
          "decode(02110000): prog=%d tone=%d prio=%d chan=%d extra=%d ovr=%d",
          se.prog, se.tone, se.prio, se.chan, se.extra, se.vab_override);
    re2_enemse_decode_entry(0x04190000u, &se);          /* Bank 0 Eintrag 0 (real) */
    CHECK(!se.silent && se.prog == 0 && se.tone == 1 && se.prio == 9 && se.chan == 4 && se.extra == 0,
          "decode(04190000): tone=%d prio=%d chan=%d", se.tone, se.prio, se.chan);
    re2_enemse_decode_entry(0xFFFFFFFFu, &se);          /* -1 = stumm (Original: return) */
    CHECK(se.silent, "decode(-1) nicht stumm");
    /* Feld-Grenzen synthetisch: b0 bit7 Override, b1&0x7F, b2>>4, b3&0x1F, b3>>5. */
    re2_enemse_decode_entry(0x7FFFFF85u, &se);
    CHECK(se.vab_override == 0x05 && se.prog == 0x7F && se.tone == 0xF && se.prio == 0xF &&
          se.chan == 0x1F && se.extra == 3,
          "decode(7FFFFF85): ovr=%d prog=%d tone=%d chan=%d extra=%d",
          se.vab_override, se.prog, se.tone, se.chan, se.extra);
    if (!g_fail) printf("  (4) Map-Dekodierung (FUN_8005bd6c-Semantik) OK\n");

    /* (5) Bank-0-EDT ebenfalls: gleicher Trailer-Mechanismus, Map-Rohbytes. */
    const uint8_t *edt0 = vbs + 0;
    CHECK(rd32(edt0 + 0xCA8 - 8) == 0x80, "Bank-0 VH-Offset != 0x80");
    CHECK(memcmp(edt0 + 0x80, "pBAV", 4) == 0, "Bank-0 VH-Magic != pBAV");
    CHECK(rd32(edt0 + 0) == 0x04190000u && rd32(edt0 + 14*4) == 0xFFFFFFFFu,
          "Bank-0 Map[0]/[14] weichen ab");
    if (!g_fail) printf("  (5) Bank 0: Trailer + Map-Rohbytes OK\n");

    free(vbs);
    if (g_fail) { fprintf(stderr, "test_re2_enemse: FAILED\n"); return 1; }
    printf("test_re2_enemse: OK\n");
    return 0;
}
