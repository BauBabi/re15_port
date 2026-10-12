/*
 * lift2030_se_pc.c — Runde 37 Spur C Nachbesserung 1 (Nachtrag 5b): PC-Seite des RE2-Lift-Tons fuer die zwei
 * STAGE2-Hubplattformen ROOM2030/2031 (engine/src/lift2030_r37.c).
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (Beta -> Retail). Belege: include/re15_lift2030_r37.h,
 * tools/fahrstuhl_r37/re2_lift6030_cut.py, analysis/befunde_runde37/C_fahrstuhl.md "Nachbesserung 1".
 *
 * Mini-Bank shared_assets/RE2/LIFT6030.VBS, Satzformat wie ELZASE.VBS / ELEVSE.VBS (FUN_8005a09c):
 *   [SE-Map @0 .. vh_off)[VH "pBAV" @vh_off][Trailer, u32 vh_off @edt_size-8][VBD]
 *   Satz 0 KLICK      = RE2 ROOM6030 Raumbank-Satz 0x11 (sub10 @0x02F90 / sub12 @0x030BA)
 *   Satz 1 FAHRT      = RE2 ROOM6030 Raumbank-Satz 0x0B (sub10 @0x02FBA / sub12 @0x030E4)
 *   Satz 2 FAHRT_KURZ = Satz 0x0B um 90 Fahrt-Bilder gekuerzt (Lift A, PORT-WAHL mit Herleitung im Werkzeug)
 * Geladen wie elza_se_pc.c (re15_vab_parse / re15_vag_adpcm_decode), gespielt ueber dieselbe Stimmen-Maschine
 * (re15_audio_se_play_bank -> se_play_layers). Nicht positional wie die RE2-Vorlage (Se_on Bezug 0, Lage 0,0,0).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_lift2030_r37.h"
#include "asset_root_pc.h"             /* re15_pc_read_re2 */
#include "../../../engine/src/gen/re2_lift6030_bank.inc"   /* RE2_LIFT_EDT_* / _VBD_* / _SE_* */

#if RE2_LIFT_SE_KLICK != RE15_LIFT_TON_KLICK || RE2_LIFT_SE_FAHRT != RE15_LIFT_TON_FAHRT || \
    RE2_LIFT_SE_FAHRT_KURZ != RE15_LIFT_TON_FAHRT_KURZ || RE2_LIFT_SE_COUNT != RE15_LIFT_TON_N
#error "gen/re2_lift6030_bank.inc und include/re15_lift2030_r37.h nummerieren die Saetze verschieden"
#endif

extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_loaded = 0;
static int        s_failed = 0;
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt[RE2_LIFT_SE_COUNT];

static int laden(void)
{
    if (s_loaded) return 1;
    if (s_failed) return 0;
    s_failed = 1;                                   /* nur EIN Versuch, danach still stumm */
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("LIFT6030.VBS", &sz);
    if (!vbs) {
        fprintf(stderr, "[liftse] shared_assets/RE2/LIFT6030.VBS fehlt -> STAGE2-Lift stumm\n");
        return 0;
    }
    if ((unsigned)sz != RE2_LIFT_DATEI_GROESSE || RE2_LIFT_EDT_SIZE < 12) {
        fprintf(stderr, "[liftse] LIFT6030.VBS %d B statt %u -> stumm\n", sz, (unsigned)RE2_LIFT_DATEI_GROESSE);
        free(vbs); return 0;
    }
    uint8_t *edt = (uint8_t *)malloc(RE2_LIFT_EDT_SIZE);
    if (!edt) { free(vbs); return 0; }
    memcpy(edt, vbs + RE2_LIFT_EDT_OFF, RE2_LIFT_EDT_SIZE);
    /* VH-Offset = Trailer-u32 @[edt_size-8] (FUN_8005a09c, wie ELZASE.VBS). */
    uint32_t vh_off = (uint32_t)edt[RE2_LIFT_EDT_SIZE-8]         | ((uint32_t)edt[RE2_LIFT_EDT_SIZE-7] << 8)
                    | ((uint32_t)edt[RE2_LIFT_EDT_SIZE-6] << 16) | ((uint32_t)edt[RE2_LIFT_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > RE2_LIFT_EDT_SIZE ||
        re15_vab_parse(edt + vh_off, (size_t)RE2_LIFT_EDT_SIZE - vh_off, &s_vab) != 0) {
        free(edt); free(vbs); return 0;
    }
    const uint8_t *vb = vbs + RE2_LIFT_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > RE2_LIFT_VBD_SIZE) continue;
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
    return 1;
}

/* Der Ton. satz = RE15_LIFT_TON_* (= Satz der Mini-Bank). */
void re15_audio_re2_lift_se(int satz)
{
    if (satz < 0 || satz >= RE2_LIFT_SE_COUNT) return;
    if (!g_audio.initialized) return;
    if (!laden()) return;
    s_gespielt[satz]++;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

unsigned re15_pc_lift2030_se_gespielt(int satz) { return (satz >= 0 && satz < RE2_LIFT_SE_COUNT) ? s_gespielt[satz] : 0u; }

void re15_pc_lift2030_se_init(void)
{
    re15_lift2030_ton_hook = re15_audio_re2_lift_se;
}
