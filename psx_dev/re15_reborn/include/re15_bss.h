/*
 * RE1.5 Rebuilt — BSS background frame format (Phase 4.5.6, 2026-05-18).
 *
 * BSS files contain MDEC-compressed static room-background frames. The
 * file is split into 64 KB chunks; each chunk is a self-contained frame:
 *
 *   +0   u16  runLengthWords  — # of MDEC-coefficient words after VLC decode
 *   +2   u16  id              — must be 0x3800 (VLC_ID) for compressed data;
 *                               other values indicate non-video chunk
 *                               (e.g. zeroed padding tail)
 *   +4   u16  quant           — quantization scale (typ. 1..16)
 *   +6   u16  version         — VLC encoding version (typ. 3 in RE1.5/RE2)
 *   +8   ...  vlc_payload[]   — variable-length-coded MDEC bitstream
 *
 * After VLC decode → array of int16 MDEC coefficients (zigzag-ordered,
 * quantized DCT). The PSX MDEC chip ingests these via DecDCTinRaw().
 * Software IDCT (PC target) yields 320×240 YUV → RGB framebuffer.
 *
 * Reference: BioClone, lib_bio's depack_vlc.c, and the Java port at
 * src/main/java/de/re15/extractors/bss/.
 */
#ifndef RE15_BSS_H
#define RE15_BSS_H

#include <stdint.h>
#include <stddef.h>

#define RE15_BSS_VLC_ID         0x3800
#define RE15_BSS_FRAME_WIDTH    320
#define RE15_BSS_FRAME_HEIGHT   240
#define RE15_BSS_CHUNK_SIZE     0x10000     /* 64 KB per chunk */
#define RE15_BSS_HEADER_SIZE    8

/* Parsed chunk header. Pointers reference the source buffer — caller
 * must keep that buffer alive while using this struct. */
typedef struct {
    uint16_t        run_length_words;   /* coefficient count after VLC decode */
    uint16_t        id;                 /* RE15_BSS_VLC_ID or non-video tag  */
    uint16_t        quant;              /* quantization scale                */
    uint16_t        version;            /* VLC encoding version              */
    const uint8_t  *vlc_payload;        /* offset +8 from chunk start        */
    size_t          vlc_payload_size;   /* chunk_size - 8                    */
} re15_bss_chunk_t;

/* Parse a chunk header from a buffer of `size` bytes (typically a 64 KB
 * BSS chunk). Returns 1 on success and populates *out_chunk. Returns 0
 * if `size` < 8 (truncated). The id field is preserved verbatim so the
 * caller can distinguish video chunks (id == VLC_ID) from padding. */
int re15_bss_parse_chunk(const uint8_t *buf, size_t size,
                         re15_bss_chunk_t *out_chunk);

/* Convenience: true if the parsed chunk contains MDEC video data
 * (id == RE15_BSS_VLC_ID). Non-video chunks at the file tail typically
 * contain zeros or other game data and should be skipped. */
int re15_bss_chunk_has_video(const re15_bss_chunk_t *chunk);

/* ------------------------------------------------------------------ */
/* Phase 4.5.6.2: VLC bitstream decoder
 *
 * Decodes the variable-length-coded MDEC payload of a BSS chunk into a
 * stream of int16 coefficients suitable for feeding to either:
 *   - the PSX hardware MDEC chip via libpsxpress DecDCTinRaw()
 *   - a software IDCT (PC target)
 *
 * Output buffer sizing: the decoder may produce up to
 *   (run_length_words + 2) * 4   16-bit coefficients
 * in the worst case (includes two synthetic header words + 1 zero pad
 * per block boundary). Caller must size `dst` accordingly.
 *
 * The two header coefficients written at the start of `dst` are:
 *   dst[0] = run_length_words   (matches input chunk header field)
 *   dst[1] = RE15_BSS_VLC_ID
 * The MDEC chip expects these as a frame preamble.
 *
 * Returns the number of int16 coefficients actually written, or a
 * negative value on error (corrupt bitstream / output overflow). */
int re15_bss_vlc_decode(const uint8_t *src_payload, size_t src_size,
                        int run_length_words, int quant, int version,
                        int16_t *dst, size_t dst_capacity);

/* ------------------------------------------------------------------ */
/* Phase 4.5.6.4: software MDEC (IDCT + dequant + YUV→RGB) for PC.
 *
 * On PSX the hardware MDEC chip does this. PC has no such chip, so we
 * port the algorithm. Pipeline per 16×16 macroblock:
 *
 *   1. rl2blk: read 6 blocks from coeff stream (Cr, Cb, Y0..Y3).
 *      Each block = 64 DCT coefficients. Run-length-encoded entries:
 *        bits 10..15 = run (zero coefficients to skip)
 *        bits  0..9  = signed level
 *      Dequantize per-coefficient using IQ_TABLE × AAN_SCALES.
 *      Inverse-zigzag via ZSCAN[].
 *      EOB sentinel terminates the block.
 *   2. idct: 2D inverse DCT (8×8 → spatial pixels).
 *   3. yuv2rgb: each macroblock has 4 Y blocks (full-res luma) and
 *      1 Cr + 1 Cb (half-res chroma, upsampled 2×2 to match Y).
 *
 * Input: VLC-decoded coefficient stream (output of re15_bss_vlc_decode).
 * Output: 24bpp R8G8B8 framebuffer, top-down row order.
 *
 * Note: the algorithm has a quirky Cr/Cb interpretation (matches the
 * revengi reference C code that produced the existing BMPs the user
 * has been working from). Block 0 is treated as Cb, Block 1 as Cr —
 * different from the standard MDEC docs but byte-for-byte matches the
 * Java extractor's output. Keeps PC visuals aligned with the
 * already_extracted/ reference BMPs.
 *
 * Returns 0 on success, -1 on bad input. */
int re15_bss_mdec_decode(const int16_t *coeffs, size_t coeff_count,
                         int width, int height,
                         uint8_t *rgb_out);

#endif /* RE15_BSS_H */
