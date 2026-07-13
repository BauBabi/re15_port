/*
 * re15_str.h — PSX STR/MDEC streaming-movie demux + decode (FE-3 of
 * PORTING_ROADMAP.md). Plays CAPCOM.STR (the opening Capcom-logo movie) and
 * any other .STR the game streams via SCD opcode 0x6F (Movie_on).
 *
 * BYTE-TRUE demux (derived directly from CAPCOM.STR, not guessed):
 *   The file is a raw CD-XA Mode2/Form1 sector stream, 2336 bytes/sector:
 *     [8]    CD-XA subheader (file/chan/submode/coding ×2)
 *     [2048] user data
 *     [280]  EDC(4) + ECC(276)
 *   Video sectors  = subheader submode 0x48, STR magic 0x0160 at user +0.
 *   Audio sectors  = submode 0x64, magic 0x0c0c (interleaved) — skipped here
 *                    (the opening logo has no dialogue; XA audio is deferred).
 *   The 32-byte STR frame header (user +0):
 *     +0x00 u16 magic     0x0160
 *     +0x02 u16 type      0x8001
 *     +0x04 u16 cur       sector index within this frame (0..cnt-1)
 *     +0x06 u16 cnt       sector count of this frame
 *     +0x08 u32 frame     1-based frame number
 *     +0x0C u32 demuxSize MDEC bitstream length in bytes
 *     +0x10 u16 width     320
 *     +0x12 u16 height    240
 *     +0x14 ...           a copy of the MDEC header words
 *   The decodable MDEC bitstream begins at sector offset +0x28 (8 subheader +
 *   32 STR header) and is 2016 bytes/sector (2048 user − 32 header). It opens
 *   with the exact BSS-chunk header [run_length_words, id=0x3800, quant,
 *   version, VLC…], so the reassembled bitstream feeds the port's existing
 *   byte-true BSS MDEC pipeline (re15_bss_parse_chunk → _vlc_decode →
 *   _mdec_decode) verbatim. A frame's bitstream = the concatenation of its
 *   cur=0..cnt-1 sectors' 2016-byte payloads; only `demuxSize` bytes are real,
 *   the rest is zero padding the VLC decoder stops before (run_length/EOB).
 *   Arithmetic check (frame 1): 2016 (sec0) + 696 (sec1) = 2712 = demuxSize. ✓
 */
#ifndef RE15_STR_H
#define RE15_STR_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    const uint8_t *data;       /* the whole .STR file (caller-owned)          */
    size_t         size;
    int            width;      /* frame width  (320)                          */
    int            height;     /* frame height (240)                          */
    int            num_frames; /* number of video frames (cur==0 sectors)     */
} re15_str_t;

/* Scan the STR container: count frames + read width/height. Returns 0 on
 * success (>=1 video frame found), negative otherwise. `data` must stay alive
 * for the lifetime of `str` (no copy is made). */
int re15_str_open(re15_str_t *str, const uint8_t *data, size_t size);

/* Decode the frame_index-th video frame (0-based) into rgba_out, which must
 * hold width*height uint32_t in RGBA8888 (0xRRGGBBAA, alpha 0xFF) — the PC
 * framebuffer layout. Returns 0 on success, negative on error. */
int re15_str_decode(const re15_str_t *str, int frame_index, uint32_t *rgba_out);

#endif /* RE15_STR_H */
