/*
 * finale_r42_pc.c — Runde 42 Strang finale, Stufe 1 Runde 3 (Plan S1, Pruefbericht s1 r2 R2): PC-Seite der RE2-Raumtonbank des Finales.
 *
 * ⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (NUTZERWUNSCH 07.11.2026: Finale wie das RE2-Ende). Die Bank ist ROOM7020 snd0, bytegleich als
 * shared_assets/RE2/R702.EDT/.VH/.VB (RDT-Kopfworte +0x08/+0x0C/+0x10 = @0x05C90/@0x05D50/@0x06B70, tools/r42_finale/re2_r702_esp.py,
 * Riegel unit_r42_finale_assets). Gespielt wird Satz 0x0F im Einschlagbild (Se_on 0x020F0001 @0x80103b54-5c, CDEMD0 EM2B-Overlay,
 * engine/src/finale_r42.c Phase 7); Satz 0x0E (Landung des Werfers) folgt mit der Wurfszene (Stufe 3).
 * Lader = Muster dining_r38_pc.c lade_klopfen (re15_vab_parse / re15_vag_adpcm_decode), Wiedergabe ueber dieselbe Stimmen-Maschine
 * wie jede SE (re15_audio_se_play_bank -> se_play_layers). PORT-WAHL: nicht positional (RE2 a1 = Lage der Form), wie die uebrigen
 * RE2-Raumbaenke im Port. Fehlt die Bank: EINE Protokollzeile, danach still stumm.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_audio.h"
#include "re15_vab.h"
#include "re15_engine.h"
#include "re15_finale_r42.h"
#include "asset_root_pc.h"             /* re15_pc_read_re2 */

extern long re15_audio_cap_tick(void);   /* audio_pc.c: Mitschnitt-Takt */
extern void re15_audio_se_play_bank(const uint8_t *edt, const re15_vab_t *vab, int16_t *const *decoded,
                                    const int *decoded_len, int satz);   /* audio_pc.c */

static int        s_state = 0;           /* 0 ungeprueft, 1 geladen, -1 fehlt */
static re15_vab_t s_vab;
static uint8_t   *s_edt = NULL;
static int        s_edt_count = 0;
static int16_t   *s_decoded[RE15_VAB_MAX_SAMPLES];
static int        s_decoded_len[RE15_VAB_MAX_SAMPLES];
static unsigned   s_gespielt = 0;

static int laden(void)
{
    int es = 0, hs = 0, vs = 0;
    uint8_t *edt = re15_pc_read_re2(RE15_FI_TONBANK ".EDT", &es);
    uint8_t *vh  = re15_pc_read_re2(RE15_FI_TONBANK ".VH", &hs);
    uint8_t *vb  = re15_pc_read_re2(RE15_FI_TONBANK ".VB", &vs);
    if (!edt || !vh || !vb || es < 4 || re15_vab_parse(vh, (size_t)hs, &s_vab) != 0) {
        free(edt); free(vh); free(vb); return -1;
    }
    for (int i = 0; i < s_vab.vag_count && i < RE15_VAB_MAX_SAMPLES; i++) {
        uint32_t off = s_vab.samples[i].offset, sz = s_vab.samples[i].size;
        if (sz == 0 || off + sz > (uint32_t)vs) continue;
        size_t cap = (sz / 16) * 28;
        int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
        if (!pcm) continue;
        s_decoded[i]     = pcm;
        s_decoded_len[i] = re15_vag_adpcm_decode(vb + off, sz, pcm, cap);
    }
    free(vh); free(vb);
    s_edt = edt;
    s_edt_count = es / 4;
    return 0;
}

static void spielen(int satz)
{
    if (!s_state) {
        s_state = laden() == 0 ? 1 : -1;
        fprintf(stderr, "[finale-se] Raumbank %s (RE2 ROOM7020 snd0) %s (%d VAGs, %d Saetze)\n", RE15_FI_TONBANK,
                s_state == 1 ? "geladen" : "FEHLT -> stumm", s_vab.vag_count, s_edt_count);
    }
    if (s_state != 1 || satz < 0 || satz >= s_edt_count) return;
    s_gespielt++;
    fprintf(stderr, "[finale-se] Se_on Raumbank %s Satz 0x%02X F%u capTick %ld\n", RE15_FI_TONBANK, satz,
            (unsigned)g_engine.frame_count, re15_audio_cap_tick());
    if (!g_audio.initialized) return;
    re15_audio_se_play_bank(s_edt, &s_vab, s_decoded, s_decoded_len, satz);
}

unsigned re15_pc_finale_r42_gespielt(void) { return s_gespielt; }

void re15_pc_finale_r42_init(void) { re15_finale_r42_se_hook(spielen); }
