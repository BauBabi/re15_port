/*
 * elza_se_pc.c — Runde 36 Spur L: PC-Seite der RE2-Toene fuer die Elza-Szenen (engine/src/elza_r36.c).
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (Beta -> Retail). Belege: include/re15_elza_r36.h,
 * tools/r36_l/re2_elza_se_cut.py, analysis/befunde_runde36/L_elza.md.
 *
 * Mini-Bank shared_assets/RE2/ELZASE.VBS, Satzformat wie TUERSE.VBS / ELEVSE.VBS (FUN_8005a09c):
 *   [SE-Map @0 .. vh_off)[VH "pBAV" @vh_off][Trailer, u32 vh_off @edt_size-8][VBD]
 *   Satz 0 AUF    = RE2 ROOM1140 Raumbank-Satz 0x25 (Tuer-Handler @0x80051654-5c nach msg 5 "You unlocked ...")
 *   Satz 1 SCHUSS = RE2 ROOM4040 Raumbank-Satz 0x0A, der RE2-Skript-Pistolenschuss (@0x014E4 `36 02 0a 00 03 01 ..` +
 *                   @0x014F0 `3a 00 18 ..` Muendungsfeuer; dieselbe Welle ROOM4090 @0x02A4E / ROOM6020 @0x00B94).
 *                   Runde 38 Spur H: vorher ROOM40A0 Satz 0x08 = der Schuss der PARTNERIN (Ada) auf den Alligator
 *                   (sub04 @0x02896 Se_on Art 2 = Partner laut RE2 @0x80011460/@0x8005649c, + ESP 0x18 @0x028A2).
 *                   Gewechselt, weil John im RE1.5-Skript im Gegnerfeld steht (Work_set Art 2 @0x80040d60 -> 0x800ACC2C)
 *                   = RE2 Art 3 (@0x80011464/@0x800564b8), und RE2s Gegnerfeld-Pistolenschuss im Skript ist ROOM4040
 *                   0x0A (Annette, @0x014E4 Art 3 Index 1) = die Aufnahme des RE1.5-Super-Redhawk ARMS07 SE 0
 *                   (Korrelation 0,98; H_elzatoene.md §8).
 * Geladen wie der Tuer-Slot 5c in audio_pc.c (dieselben Decoder re15_vab_parse / re15_vag_adpcm_decode), gespielt
 * ueber dieselbe Stimmen-/Prioritaets-Maschine (re15_audio_se_play_bank -> se_play_layers). Nicht positional wie
 * RE2 @0x8005165c `addu a1,zero,zero` (Aufschliessen) bzw. ROOM4040 @0x014E4 (Se_on Flags 0 = ohne Lage).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_elza_r36.h"
#include "asset_root_pc.h"             /* re15_pc_read_re2 */
#include "../../../engine/src/gen/re2_elza_bank.inc"   /* RE2_ELZA_EDT_* / _VBD_* / _SE_AUF / _SE_SCHUSS */

#if RE2_ELZA_SE_AUF != RE15_ELZA_TON_AUF || RE2_ELZA_SE_SCHUSS != RE15_ELZA_TON_SCHUSS
#error "gen/re2_elza_bank.inc und include/re15_elza_r36.h nummerieren die Saetze verschieden"
#endif

extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_loaded = 0;
static int        s_failed = 0;
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt[RE2_ELZA_SE_COUNT];

static int laden(void)
{
    if (s_loaded) return 1;
    if (s_failed) return 0;
    s_failed = 1;                                   /* nur EIN Versuch, danach still stumm */
    int sz = 0;
    uint8_t *vbs = re15_pc_read_re2("ELZASE.VBS", &sz);
    if (!vbs) {
        fprintf(stderr, "[elzase] shared_assets/RE2/ELZASE.VBS fehlt -> Elza-Szenen-Toene stumm\n");
        return 0;
    }
    if ((unsigned)sz < RE2_ELZA_VBD_OFF + RE2_ELZA_VBD_SIZE || RE2_ELZA_EDT_SIZE < 12) { free(vbs); return 0; }
    uint8_t *edt = (uint8_t *)malloc(RE2_ELZA_EDT_SIZE);
    if (!edt) { free(vbs); return 0; }
    memcpy(edt, vbs + RE2_ELZA_EDT_OFF, RE2_ELZA_EDT_SIZE);
    /* VH-Offset = Trailer-u32 @[edt_size-8] (FUN_8005a09c, wie TUERSE.VBS). */
    uint32_t vh_off = (uint32_t)edt[RE2_ELZA_EDT_SIZE-8]         | ((uint32_t)edt[RE2_ELZA_EDT_SIZE-7] << 8)
                    | ((uint32_t)edt[RE2_ELZA_EDT_SIZE-6] << 16) | ((uint32_t)edt[RE2_ELZA_EDT_SIZE-5] << 24);
    if (vh_off + 0x20u > RE2_ELZA_EDT_SIZE ||
        re15_vab_parse(edt + vh_off, (size_t)RE2_ELZA_EDT_SIZE - vh_off, &s_vab) != 0) {
        free(edt); free(vbs); return 0;
    }
    const uint8_t *vb = vbs + RE2_ELZA_VBD_OFF;
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, vsz = s_vab.samples[i].size;
        if (off + vsz > RE2_ELZA_VBD_SIZE) continue;
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

/* Der Ton. satz = RE15_ELZA_TON_AUF / RE15_ELZA_TON_SCHUSS (= Satz der Mini-Bank). */
void re15_audio_re2_elza_se(int satz)
{
    if (satz < 0 || satz >= RE2_ELZA_SE_COUNT) return;
    if (!g_audio.initialized) return;
    if (!laden()) return;
    s_gespielt[satz]++;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

unsigned re15_pc_elza_se_gespielt(int satz) { return (satz >= 0 && satz < RE2_ELZA_SE_COUNT) ? s_gespielt[satz] : 0u; }

void re15_pc_elza_se_init(void)
{
    re15_elza_se_hook = re15_audio_re2_elza_se;
}
