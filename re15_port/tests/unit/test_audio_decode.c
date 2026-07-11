/* test_audio_decode.c — byte-true ADPCM decode + EDT SE resolution (audio audit wf_1db9c802).
 *
 * (1) ADPCM END flag (AUD-ADPCM-END-FLAG): flag bit 0x01 = Loop End — the END block IS voiced,
 *     then the decode STOPS (psx-spx soundprocessingunitspu.md:103-141; Code 1 = End+Mute,
 *     Env=0000h). One-shot VAGs carry a dummy looper block (flags 0x07, data 0x77 x14 = 28
 *     samples of +28672) INSIDE their extent — decoding past the END appended that DC pulse to
 *     virtually every shipped SE.
 * (2) ADPCM shift clamp (AUD-ADPCM-SHIFT-CLAMP): reserved shifts 13..15 act as shift 9.
 * (3) EDT layering (AUD-EDT-LAYER-B3HI): record byte3 bits 5-7 = extra consecutive tones keyed
 *     with the base tone (FUN_80045024 @0x8004516c srl 5) — the handgun gunshot is 2 layers.
 * (4) Water-floor mask (AUD-FOOT-WATER-IDX): sound_type bit 0x80 is masked, not rejected
 *     (FUN_80045630 @0x80045718 andi 0x7f) — water-floor footsteps must resolve, not go silent.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_vab.h"

int main(void)
{
    int fail = 0;
    printf("=== ADPCM END/shift + EDT layering/water mask (audit wf_1db9c802) ===\n");

    /* (1) END flag: 3 blocks — data, END (flags 0x01), looper (flags 0x07, 0x77 data).
     *    Decode must return 2*28 = 56 samples and contain no +28672 looper output. */
    {
        uint8_t vag[48]; memset(vag, 0, sizeof vag);
        vag[0] = 0x00; vag[1] = 0x00;                   /* block 0: shift 0, filter 0, no flags */
        for (int i = 2; i < 16; i++) vag[i] = 0x11;     /* small nonzero data */
        vag[16] = 0x00; vag[17] = 0x01;                 /* block 1: END (Code 1, End+Mute) */
        vag[32] = 0x00; vag[33] = 0x07;                 /* block 2: the dummy looper */
        for (int i = 34; i < 48; i++) vag[i] = 0x77;    /* decodes to +28672 if voiced */

        int16_t pcm[128];
        int n = re15_vag_adpcm_decode(vag, sizeof vag, pcm, 128);
        if (n != 56) { fprintf(stderr, "FAIL(1a): decode must stop after the END block (56 samples), got %d\n", n); fail = 1; }
        int has_pulse = 0;
        for (int i = 0; i < n; i++) if (pcm[i] == 28672) has_pulse = 1;
        if (has_pulse) { fprintf(stderr, "FAIL(1b): the looper block's +28672 must never be voiced\n"); fail = 1; }
        if (!fail) printf("  (1) END flag: 56 samples, looper block never voiced\n");
    }

    /* (2) shift clamp: header 0x0D (filter 0, shift 13) + first nibble 0x7 -> hardware shift 9:
     *    (7<<12)>>9 = 56. The literal shift 13 gave 3. */
    {
        uint8_t vag[16]; memset(vag, 0, sizeof vag);
        vag[0] = 0x0D; vag[1] = 0x00; vag[2] = 0x07;    /* low nibble 7 = the first sample */
        int16_t pcm[28];
        int n = re15_vag_adpcm_decode(vag, sizeof vag, pcm, 28);
        if (n != 28 || pcm[0] != 56) {
            fprintf(stderr, "FAIL(2): shift 13 must act as shift 9 (first sample 56), got n=%d s0=%d\n",
                    n, n > 0 ? pcm[0] : -1);
            fail = 1;
        } else printf("  (2) reserved shift 13 acts as shift 9 (sample 56, not 3)\n");
    }

    /* (3) layering: the ARMS gunshot record 00 00 13 30 (prog 0, base tone 1, byte3 0x30 =
     *    1 extra) with tone1->vag2, tone2->vag3 must resolve TWO layers {1, 2} (0-based). */
    {
        re15_vab_t vab; memset(&vab, 0, sizeof vab);
        vab.vag_count = 3;
        vab.tones[1].vag_index = 2;                     /* prog 0, tone 1 -> VAG 2 (1-based) */
        vab.tones[2].vag_index = 3;                     /* prog 0, tone 2 -> VAG 3 */
        uint8_t edt[8] = { 0x00, 0x00, 0x13, 0x30,      /* record 0: the gunshot shape */
                           0x00, 0x00, 0x00, 0x00 };
        int vags[8];
        int n = re15_edt_resolve_layers(edt, &vab, 0, vags, 8);
        if (n != 2 || vags[0] != 1 || vags[1] != 2) {
            fprintf(stderr, "FAIL(3a): record 00 00 13 30 must layer 2 VAGs {1,2}, got n=%d {%d,%d}\n",
                    n, n > 0 ? vags[0] : -1, n > 1 ? vags[1] : -1);
            fail = 1;
        }
        /* a plain record (byte3 high bits 0) stays single-layer */
        uint8_t edt1[4] = { 0x00, 0x00, 0x13, 0x00 };
        n = re15_edt_resolve_layers(edt1, &vab, 0, vags, 8);
        if (n != 1 || vags[0] != 1) { fprintf(stderr, "FAIL(3b): byte3=0 must be 1 layer {1}, got n=%d\n", n); fail = 1; }
        if (!fail) printf("  (3) layering: gunshot record keys 2 tones (VAG2+VAG3); plain record 1\n");
    }

    /* (4) water mask: sound_type 0x97 must resolve record 0x17 (not return -1). */
    {
        re15_vab_t vab; memset(&vab, 0, sizeof vab);
        vab.vag_count = 9;
        vab.tones[7].vag_index = 8;                     /* prog 0, tone 7 -> VAG 8 */
        static uint8_t edt[128]; memset(edt, 0, sizeof edt);
        edt[0x17 * 4 + 2] = 0x73;                       /* record 0x17: tone 7 (byte2>>4) */
        edt[0x17 * 4 + 3] = 0x12;
        int vag = re15_footstep_vag(edt, &vab, 0x97);   /* water bit 0x80 set */
        if (vag != 7) { fprintf(stderr, "FAIL(4): water floor 0x97 must resolve record 0x17 -> sample 7, got %d\n", vag); fail = 1; }
        else printf("  (4) water floor 0x97 resolves (masked to 0x17), no longer silent\n");
    }

    if (fail) { printf("AUDIO-DECODE: FAIL\n"); return 1; }
    printf("AUDIO-DECODE: all checks passed\n");
    return 0;
}
