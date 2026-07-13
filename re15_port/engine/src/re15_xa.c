/*
 * re15_xa.c — CD-XA ADPCM audio decoder for STR movies. See re15_xa.h for the
 * byte-true format. This complements re15_str.c (which demuxes/decodes the
 * video sectors and skips audio); together they play CAPCOM.STR with sound.
 */
#include "re15_xa.h"
#include <stdlib.h>
#include <string.h>

#define XA_SECTOR       2336
#define XA_SUBHDR       8
#define XA_ADPCM_OFF    8       /* ADPCM data begins right after the subheader   */
#define XA_GROUPS       18      /* sound-groups per sector                       */
#define XA_GROUP_SZ     128
#define XA_BLOCKS       4       /* 4-bit stereo: 4 blocks/group                  */
#define XA_SAMPLES      28      /* samples per block                             */
#define XA_FRAMES_SEC   (XA_GROUPS * XA_BLOCKS * XA_SAMPLES)   /* 2016 stereo frames */

/* XA-ADPCM predictor coefficients (psx-spx cdromformat.md:843-844). XA uses
 * only filters 0..3; identical to the SPU table's first four entries
 * (vab_common.c:304-305), which the port already verified against real audio. */
static const int s_xa_k0[4] = { 0, 60, 115, 98 };
static const int s_xa_k1[4] = { 0,  0, -52, -55 };

static int16_t sat16(int v)
{
    if (v < -32768) return (int16_t)-32768;
    if (v >  32767) return (int16_t) 32767;
    return (int16_t)v;
}

/* Decode one 4-bit-stereo audio sector's 2304 ADPCM bytes into 2016 interleaved
 * stereo frames at dst. hist = {old_L, older_L, old_R, older_R}, carried in/out. */
void re15_xa_decode_sector(const uint8_t *adpcm, int16_t *dst, int hist[4])
{
    for (int g = 0; g < XA_GROUPS; g++) {
        const uint8_t *grp = adpcm + g * XA_GROUP_SZ;
        for (int blk = 0; blk < XA_BLOCKS; blk++) {
            for (int ch = 0; ch < 2; ch++) {                 /* nibble 0 = L, 1 = R */
                uint8_t param = grp[4 + blk * 2 + ch];
                int shift  = param & 0x0F;
                if (shift > 12) shift = 9;                   /* reserved 13..15 -> 9 */
                int filter = (param >> 4) & 0x03;            /* XA: filters 0..3     */
                int k0 = s_xa_k0[filter], k1 = s_xa_k1[filter];
                int *old   = &hist[ch * 2];
                int *older = &hist[ch * 2 + 1];
                int frame0 = g * (XA_BLOCKS * XA_SAMPLES) + blk * XA_SAMPLES;
                for (int j = 0; j < XA_SAMPLES; j++) {
                    int nib = (grp[16 + blk + j * 4] >> (ch * 4)) & 0x0F;
                    int t = nib;
                    if (t & 0x08) t -= 16;                    /* sign-extend 4-bit    */
                    int v = (t << 12) >> shift;
                    v += (*old * k0 + *older * k1 + 32) >> 6; /* == verified SPU step */
                    int16_t s = sat16(v);
                    *older = *old;
                    *old   = s;
                    dst[(frame0 + j) * 2 + ch] = s;
                }
            }
        }
    }
}

int re15_xa_decode_movie(const uint8_t *str_data, size_t size,
                         int16_t **out_pcm, int *out_frames,
                         int *out_rate, int *out_channels)
{
    if (!str_data || !out_pcm) return -1;
    *out_pcm = NULL;

    /* Pass 1: count audio sectors + validate the format is 4-bit stereo. */
    int audio_sectors = 0, rate = 0;
    for (size_t off = 0; off + XA_SECTOR <= size; off += XA_SECTOR) {
        const uint8_t *sec = str_data + off;
        if ((sec[2] & 0x04) == 0) continue;                  /* submode bit2 = Audio */
        uint8_t coding = sec[3];
        int stereo = (coding & 0x03) == 1;
        int r      = ((coding >> 2) & 0x03) == 0 ? 37800 : 18900;
        int bits4  = ((coding >> 4) & 0x03) == 0;
        if (!stereo || !bits4) return -2;                    /* only 4-bit stereo    */
        rate = r;
        audio_sectors++;
    }
    if (audio_sectors == 0) return -3;

    int    total_frames = audio_sectors * XA_FRAMES_SEC;
    int16_t *pcm = (int16_t *) malloc((size_t)total_frames * 2 * sizeof(int16_t));
    if (!pcm) return -4;

    /* Pass 2: decode. The (old,older) predictor history is continuous across the
     * whole movie (psx-spx: the incoming old/older are those of the previous
     * part), so it is initialised once here — not per sector. */
    int hist[4] = { 0, 0, 0, 0 };
    int fpos = 0;
    for (size_t off = 0; off + XA_SECTOR <= size; off += XA_SECTOR) {
        const uint8_t *sec = str_data + off;
        if ((sec[2] & 0x04) == 0) continue;
        re15_xa_decode_sector(sec + XA_ADPCM_OFF, pcm + (size_t)fpos * 2, hist);
        fpos += XA_FRAMES_SEC;
    }

    *out_pcm = pcm;
    if (out_frames)   *out_frames   = total_frames;
    if (out_rate)     *out_rate     = rate;
    if (out_channels) *out_channels = 2;
    return 0;
}
