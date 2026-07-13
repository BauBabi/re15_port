/*
 * re15_xa.h — CD-XA ADPCM audio decoder for STR movies (FE-3 audio half).
 *
 * The PSX plays STR movie audio in HARDWARE: the CD controller auto-decodes the
 * interleaved XA-ADPCM sectors straight into the SPU CD-audio input (the game
 * only sets the real-time CD mode + the CdMix volume). The PC port has no CD
 * hardware, so it must software-decode those sectors to PCM — this module.
 *
 * BYTE-TRUE format (CAPCOM.STR, verified against the bytes + psx-spx):
 *   Audio sector = CD-XA subheader submode 0x64 (Audio|Form2|Real-time),
 *   coding_info 0x01 = STEREO / 37800 Hz / 4-bit / no emphasis. The 2336-byte
 *   sector = 8 subheader + 0x900 (2304) ADPCM + 0x14 zerofill + 4 EDC
 *   (cdromfileformats.md:3055-3061). The 2304 ADPCM bytes = 18 sound-groups ×
 *   128 bytes; each group = a 16-byte header (params at [4..0x0B], stored
 *   redundantly at [0..3]/[0x0C..0x0F]) + 28 data words × 4 bytes.
 *   4-bit stereo: 4 blocks/group, each block's LOW nibble = Left, HIGH = Right.
 *     shift  = header[4 + blk*2 + ch] & 0x0F   (reserved 13..15 act as 9)
 *     filter = (header[4 + blk*2 + ch] & 0x30) >> 4   (XA uses only 0..3)
 *     nibble = (data[16 + blk + j*4] >> (ch*4)) & 0x0F, sign-extended
 *     sample = ((nibble<<12) >> shift) + ((old*K0 + older*K1 + 32) >> 6), sat16
 *   with a separate (old,older) history per channel, carried across the whole
 *   movie. The K0/K1 tables + the (·+32)>>6 rounding are byte-identical to the
 *   port's hardware-verified SPU-ADPCM decoder (vab_common.c:353-357); psx-spx
 *   states the CD-XA shift/filter decode is the same as SPU-ADPCM
 *   (soundprocessingunitspu.md:119, cdromformat.md:829-847).
 *   Yield: 18*4*28 = 2016 stereo frames/sector; 329 sectors = 663,264 frames =
 *   17.55 s @ 37800 Hz (== the video duration; audio is the sync master).
 */
#ifndef RE15_XA_H
#define RE15_XA_H

#include <stdint.h>
#include <stddef.h>

/* Decode every CD-XA audio sector of an in-memory STR movie into one malloc'd
 * interleaved S16 stereo PCM buffer. Returns 0 on success (caller frees
 * *out_pcm), negative on error. *out_frames = stereo frame count, *out_rate =
 * source sample rate (Hz), *out_channels = 2. Only the 4-bit STEREO format that
 * CAPCOM.STR uses is decoded; a sector with any other coding_info is a hard
 * error (never silently wrong). */
int re15_xa_decode_movie(const uint8_t *str_data, size_t size,
                         int16_t **out_pcm, int *out_frames,
                         int *out_rate, int *out_channels);

/* Decode one 4-bit-stereo audio sector (2304 ADPCM bytes) into 2016 interleaved
 * stereo frames (dst holds 2016*2 int16). hist[4] = {old_L, older_L, old_R,
 * older_R}, carried across calls (init to 0 at movie start). Exposed for tests. */
void re15_xa_decode_sector(const uint8_t *adpcm2304, int16_t *dst2016x2, int hist[4]);

#endif /* RE15_XA_H */
