/*
 * toene_r39_pc.c — Runde 39 Spur B "toene": PC-Seite der Toene fuer Schublade (ROOM11F0/11F1), Lueftungsgitter und
 * Schrank (ROOM1190/1191), engine/src/toene_r39.c.
 *
 * ⛔ ERGAENZUNG, KEIN RE1.5-ORIGINAL AN DIESEN STELLEN (RE1.5 hat dort kein Se_on). Belege: include/re15_toene_r39.h,
 * tools/r39_b/toene39_se_cut.py, analysis/befunde_runde39/B_toene.md.
 *
 * Mini-Bank shared_assets/RE2/TOENE39.VBS, Satzformat wie ELZASE.VBS / TUERSE.VBS (FUN_8005a09c):
 *   [SE-Map @0 .. vh_off)[VH "pBAV" @vh_off][Trailer, u32 vh_off @edt_size-8][VBD]
 *   Satz 0 SCHUBLADE  = RE2 ROOM1130 Raumbank-Satz 0x28 (Schubladen-Handler @0x80052044, Se_on @0x8005218c)
 *   Satz 1 GITTER_AUF = RE1.5 ROOM2060 snd0-Satz 0x0E (sub02 @0x0134A)
 *   Satz 2 GITTER_ZU  = RE1.5 ROOM2060 snd0-Satz 0x0D (sub02 @0x01376)
 *   Satz 3 REGAL      = RE2 ROOM1120 Raumbank-Satz 0x0C (sub04 @0x02F2E)
 * Geladen wie elza_se_pc.c (dieselben Decoder re15_vab_parse / re15_vag_adpcm_decode), gespielt ueber dieselbe
 * Stimmen-/Prioritaets-Maschine (re15_audio_se_play_bank -> se_play_layers). Nicht positional (Port spielt Se_on ohne
 * Lage, OFFEN wie Runde 38 H): RE2 @0x80052188 `addu a1,zero,zero` (Schublade) ist auch im Original ohne Lage.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_toene_r39.h"
#include "asset_root_pc.h"             /* re15_pc_read_re2 */
#include "../../../engine/src/gen/re2_toene39_bank.inc"   /* RE2_TOENE39_EDT_* / _VBD_* / _SE_* */

#if RE2_TOENE39_SE_SCHUBLADE != RE15_TOENE39_TON_SCHUBLADE || RE2_TOENE39_SE_GITTER_AUF != RE15_TOENE39_TON_GITTER_AUF \
 || RE2_TOENE39_SE_GITTER_ZU != RE15_TOENE39_TON_GITTER_ZU || RE2_TOENE39_SE_REGAL != RE15_TOENE39_TON_REGAL \
 || RE2_TOENE39_SE_COUNT != RE15_TOENE39_TON_N
#error "gen/re2_toene39_bank.inc und include/re15_toene_r39.h nummerieren die Saetze verschieden"
#endif

extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_loaded = 0;
static int        s_failed = 0;
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt[RE2_TOENE39_SE_COUNT];

static int laden(void)
{
    if (s_loaded) return 1;
    if (s_failed) return 0;
    s_failed = 1;                                   /* nur EIN Versuch, danach still stumm */
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("TOENE39.VBS", &sz);
    if (!vbs) {
        fprintf(stderr, "[toene39] shared_assets/RE2/TOENE39.VBS fehlt -> Schublade/Gitter/Schrank stumm\n");
        return 0;
    }
    if ((unsigned)sz < RE2_TOENE39_VBD_OFF + RE2_TOENE39_VBD_SIZE || RE2_TOENE39_EDT_SIZE < 12) { free(vbs); return 0; }
    uint8_t *edt = (uint8_t *)malloc(RE2_TOENE39_EDT_SIZE);
    if (!edt) { free(vbs); return 0; }
    memcpy(edt, vbs + RE2_TOENE39_EDT_OFF, RE2_TOENE39_EDT_SIZE);
    /* VH-Offset = Trailer-u32 @[edt_size-8] (FUN_8005a09c, wie TUERSE.VBS / ELZASE.VBS). */
    uint32_t vh_off = (uint32_t)edt[RE2_TOENE39_EDT_SIZE-8]         | ((uint32_t)edt[RE2_TOENE39_EDT_SIZE-7] << 8)
                    | ((uint32_t)edt[RE2_TOENE39_EDT_SIZE-6] << 16) | ((uint32_t)edt[RE2_TOENE39_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > RE2_TOENE39_EDT_SIZE ||
        re15_vab_parse(edt + vh_off, (size_t)RE2_TOENE39_EDT_SIZE - vh_off, &s_vab) != 0) {
        free(edt); free(vbs); return 0;
    }
    const uint8_t *vb = vbs + RE2_TOENE39_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > RE2_TOENE39_VBD_SIZE) continue;
        size_t cap = (vsz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_decoded[i]     = pcm;
        s_decoded_len[i] = re15_vag_adpcm_decode(vb + off, vsz, pcm, cap);
    }
    free(vbs);
    s_edt = edt;
    s_loaded = 1;
    s_failed = 0;
    fprintf(stderr, "[toene39] TOENE39.VBS geladen: %d Saetze, %d VAGs\n", RE2_TOENE39_SE_COUNT, s_vab.vag_count);
    return 1;
}

/* Der Ton. ton = RE15_TOENE39_TON_* (= Satz der Mini-Bank). */
static void re15_audio_toene39_se(int ton)
{
    if (ton < 0 || ton >= RE2_TOENE39_SE_COUNT) return;
    if (!g_audio.initialized) return;
    if (!laden()) return;
    s_gespielt[ton]++;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, ton);
}

unsigned re15_pc_toene39_gespielt(int ton) { return (ton >= 0 && ton < RE2_TOENE39_SE_COUNT) ? s_gespielt[ton] : 0u; }

void re15_pc_toene_r39_init(void)
{
    re15_toene39_se_hook = re15_audio_toene39_se;
}
