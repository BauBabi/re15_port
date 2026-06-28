/*
 * RE1.5 Rebuilt — BSS VLC (variable-length-code) decoder (Phase 4.5.6.2,
 * 2026-05-18).
 *
 * Pure C, target-agnostic. Decodes the MDEC bitstream from a BSS chunk
 * payload into the int16-coefficient stream expected by either the PSX
 * hardware MDEC chip (libpsxpress DecDCTinRaw) or our software IDCT.
 *
 * Ported from src/main/java/de/re15/extractors/bss/VlcDecoder.java (the
 * Java extractor in this same repo). The Java decoder is in turn an
 * implementation of the algorithm documented in revengi/depack_vlc.c
 * (Patrice Mandin, GPL — used as reference for the algorithm, not the
 * code; this file is an independent C implementation matching our own
 * Java port byte-for-byte).
 *
 * Pipeline per macroblock (6 blocks: Cr, Cb, Y0, Y1, Y2, Y3):
 *   1. decode_dc — DC coefficient, version-dependent (delta-encoded or
 *      raw 10-bit depending on the BSS version field)
 *   2. write the coefficient packed with q_code (quant scale in high bits)
 *   3. decode_ac — variable-length AC coefficients, run-length compressed
 *      until an EOB (end-of-block) marker is seen
 *   4. write the EOB sentinel
 *   5. advance the component-rotation counter (0..5)
 *
 * Bit-buffer state: 32-bit register, refilled from the source bitstream
 * in 16-bit chunks. `show_bits(N)` peeks the top N bits without
 * consuming; `flush_bits(N)` advances the bit cursor by N. Bits flow
 * from MSB → LSB.
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include "re15_bss_mdec.h"

/* ====================================================================
 * Lookup-table generation macros
 *
 * The MDEC bitstream uses several VLC tables. Each entry packs:
 *   bits 16..31  — bit-length of the VLC code that produced this entry
 *   bits 10..15  — run-length (zero coefficients to skip before level)
 *   bits  0..9   — signed 10-bit level (after sign-extend from bit 9)
 *
 * The macros below mirror the TableBuilder helpers in the Java port:
 *   VLC_CODE   — adds two entries, +level then -level (bits+1 each)
 *   VLC_CODE0  — adds two entries with the same (run, level, bits)
 *   VLC_CODE2  — adds two entries with the same (run, level, bits+1)
 *
 * These map 1:1 to TableBuilder.code() / .code0() / .code2() in
 * VlcDecoder.java; if you ever need to re-verify the C tables against
 * the Java, the call sequences match line-for-line. */

#define VLC_PACK(run, level, bits) \
    (((int)((bits) & 0xFF) << 16) | (((run) & 0x3F) << 10) | ((level) & 0x3FF))

#define VLC_CODE(r, l, b)  VLC_PACK((r), (l), (b)+1), VLC_PACK((r), -(l), (b)+1)
#define VLC_CODE0(r, l, b) VLC_PACK((r), (l), (b)),    VLC_PACK((r), (l),  (b))
#define VLC_CODE2(r, l, b) VLC_PACK((r), (l), (b)+1), VLC_PACK((r), (l),  (b)+1)

/* Standard MDEC sentinels (also used as table entries) */
#define EOB_VLC_CODE   VLC_PACK(63, 512, 2)   /* End-of-block — see decode_ac */
#define ESCAPE_CODE    VLC_PACK(63, 0, 6)     /* Escape code — read 16-bit raw level */

/* Macros to extract packed fields */
#define VLC_BITS(entry)   (((entry) >> 16) & 0xFF)
#define VLC_RUN(entry)    (((entry) >> 10) & 0x3F)
#define VLC_LEVEL(entry)  sign_extend_10(entry)

static int sign_extend_10(int packed)
{
    int level = packed & 0x3FF;
    if (level & 0x200) level -= 0x400;
    return level;
}

/* ====================================================================
 * Hand-translated tables (mirrors buildVlcTab* / buildDc*Tab0 in Java) */

static const int VLC_TAB_NEXT[] = {
    VLC_CODE(0, 2, 4),
    VLC_CODE(2, 1, 4),
    VLC_CODE2(1, 1, 3),
    VLC_CODE2(1, -1, 3),
    VLC_CODE0(63, 512, 2),
    VLC_CODE0(63, 512, 2),
    VLC_CODE0(63, 512, 2),
    VLC_CODE0(63, 512, 2),
    VLC_CODE2(0, 1, 2),
    VLC_CODE2(0, 1, 2),
    VLC_CODE2(0, -1, 2),
    VLC_CODE2(0, -1, 2),
};

static const int VLC_TAB0[] = {
    VLC_CODE0(63, 0, 6),
    VLC_CODE0(63, 0, 6),
    VLC_CODE0(63, 0, 6),
    VLC_CODE0(63, 0, 6),
    VLC_CODE2(2, 2, 7),
    VLC_CODE2(2, -2, 7),
    VLC_CODE2(9, 1, 7),
    VLC_CODE2(9, -1, 7),
    VLC_CODE2(0, 4, 7),
    VLC_CODE2(0, -4, 7),
    VLC_CODE2(8, 1, 7),
    VLC_CODE2(8, -1, 7),
    VLC_CODE2(7, 1, 6),
    VLC_CODE2(7, 1, 6),
    VLC_CODE2(7, -1, 6),
    VLC_CODE2(7, -1, 6),
    VLC_CODE2(6, 1, 6),
    VLC_CODE2(6, 1, 6),
    VLC_CODE2(6, -1, 6),
    VLC_CODE2(6, -1, 6),
    VLC_CODE2(1, 2, 6),
    VLC_CODE2(1, 2, 6),
    VLC_CODE2(1, -2, 6),
    VLC_CODE2(1, -2, 6),
    VLC_CODE2(5, 1, 6),
    VLC_CODE2(5, 1, 6),
    VLC_CODE2(5, -1, 6),
    VLC_CODE2(5, -1, 6),
    VLC_CODE(13, 1, 8),
    VLC_CODE(0, 6, 8),
    VLC_CODE(12, 1, 8),
    VLC_CODE(11, 1, 8),
    VLC_CODE(3, 2, 8),
    VLC_CODE(1, 3, 8),
    VLC_CODE(0, 5, 8),
    VLC_CODE(10, 1, 8),
    VLC_CODE2(0, 3, 5),
    VLC_CODE2(0, 3, 5),
    VLC_CODE2(0, 3, 5),
    VLC_CODE2(0, 3, 5),
    VLC_CODE2(0, -3, 5),
    VLC_CODE2(0, -3, 5),
    VLC_CODE2(0, -3, 5),
    VLC_CODE2(0, -3, 5),
    VLC_CODE2(4, 1, 5),
    VLC_CODE2(4, 1, 5),
    VLC_CODE2(4, 1, 5),
    VLC_CODE2(4, 1, 5),
    VLC_CODE2(4, -1, 5),
    VLC_CODE2(4, -1, 5),
    VLC_CODE2(4, -1, 5),
    VLC_CODE2(4, -1, 5),
    VLC_CODE2(3, 1, 5),
    VLC_CODE2(3, 1, 5),
    VLC_CODE2(3, 1, 5),
    VLC_CODE2(3, 1, 5),
    VLC_CODE2(3, -1, 5),
    VLC_CODE2(3, -1, 5),
    VLC_CODE2(3, -1, 5),
    VLC_CODE2(3, -1, 5),
};

static const int VLC_TAB1[] = {
    VLC_CODE(16, 1, 10), VLC_CODE(5, 2, 10), VLC_CODE(0, 7, 10),
    VLC_CODE(2, 3, 10),  VLC_CODE(1, 4, 10), VLC_CODE(15, 1, 10),
    VLC_CODE(14, 1, 10), VLC_CODE(4, 2, 10),
};

static const int VLC_TAB2[] = {
    VLC_CODE(0, 11, 12), VLC_CODE(8, 2, 12), VLC_CODE(4, 3, 12),
    VLC_CODE(0, 10, 12), VLC_CODE(2, 4, 12), VLC_CODE(7, 2, 12),
    VLC_CODE(21, 1, 12), VLC_CODE(20, 1, 12), VLC_CODE(0, 9, 12),
    VLC_CODE(19, 1, 12), VLC_CODE(18, 1, 12), VLC_CODE(1, 5, 12),
    VLC_CODE(3, 3, 12),  VLC_CODE(0, 8, 12),  VLC_CODE(6, 2, 12),
    VLC_CODE(17, 1, 12),
};

static const int VLC_TAB3[] = {
    VLC_CODE(10, 2, 13), VLC_CODE(9, 2, 13), VLC_CODE(5, 3, 13),
    VLC_CODE(3, 4, 13),  VLC_CODE(2, 5, 13), VLC_CODE(1, 7, 13),
    VLC_CODE(1, 6, 13),  VLC_CODE(0, 15, 13),
    VLC_CODE(0, 14, 13), VLC_CODE(0, 13, 13), VLC_CODE(0, 12, 13),
    VLC_CODE(26, 1, 13), VLC_CODE(25, 1, 13), VLC_CODE(24, 1, 13),
    VLC_CODE(23, 1, 13), VLC_CODE(22, 1, 13),
};

/* VLC_TAB4: levels 31..16 (descending) for run=0, all at 14 bits */
static const int VLC_TAB4[] = {
    VLC_CODE(0, 31, 14), VLC_CODE(0, 30, 14), VLC_CODE(0, 29, 14),
    VLC_CODE(0, 28, 14), VLC_CODE(0, 27, 14), VLC_CODE(0, 26, 14),
    VLC_CODE(0, 25, 14), VLC_CODE(0, 24, 14), VLC_CODE(0, 23, 14),
    VLC_CODE(0, 22, 14), VLC_CODE(0, 21, 14), VLC_CODE(0, 20, 14),
    VLC_CODE(0, 19, 14), VLC_CODE(0, 18, 14), VLC_CODE(0, 17, 14),
    VLC_CODE(0, 16, 14),
};

/* VLC_TAB5: levels 40..32 for run=0, then 14..8 for run=1, all at 15 bits */
static const int VLC_TAB5[] = {
    VLC_CODE(0, 40, 15), VLC_CODE(0, 39, 15), VLC_CODE(0, 38, 15),
    VLC_CODE(0, 37, 15), VLC_CODE(0, 36, 15), VLC_CODE(0, 35, 15),
    VLC_CODE(0, 34, 15), VLC_CODE(0, 33, 15), VLC_CODE(0, 32, 15),
    VLC_CODE(1, 14, 15), VLC_CODE(1, 13, 15), VLC_CODE(1, 12, 15),
    VLC_CODE(1, 11, 15), VLC_CODE(1, 10, 15), VLC_CODE(1, 9, 15),
    VLC_CODE(1, 8, 15),
};

/* VLC_TAB6: at 16 bits — descending levels for run=1, then mixed runs */
static const int VLC_TAB6[] = {
    VLC_CODE(1, 18, 16), VLC_CODE(1, 17, 16), VLC_CODE(1, 16, 16),
    VLC_CODE(1, 15, 16),
    VLC_CODE(6, 3, 16),
    VLC_CODE(16, 2, 16), VLC_CODE(15, 2, 16), VLC_CODE(14, 2, 16),
    VLC_CODE(13, 2, 16), VLC_CODE(12, 2, 16), VLC_CODE(11, 2, 16),
    VLC_CODE(31, 1, 16), VLC_CODE(30, 1, 16), VLC_CODE(29, 1, 16),
    VLC_CODE(28, 1, 16), VLC_CODE(27, 1, 16),
};

/* DC predictor tables (luma and chroma). Each entry packs (bits, level)
 * but with run=0 (DC has no run). Values are deltas from the previous
 * DC predictor for that component. */
static const int DC_Y_TAB0[48] = {
    VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3),
    VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3),
    VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3),
    VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3),
    VLC_PACK(0, -3, 4), VLC_PACK(0, -3, 4), VLC_PACK(0, -3, 4), VLC_PACK(0, -3, 4),
    VLC_PACK(0, -2, 4), VLC_PACK(0, -2, 4), VLC_PACK(0, -2, 4), VLC_PACK(0, -2, 4),
    VLC_PACK(0,  2, 4), VLC_PACK(0,  2, 4), VLC_PACK(0,  2, 4), VLC_PACK(0,  2, 4),
    VLC_PACK(0,  3, 4), VLC_PACK(0,  3, 4), VLC_PACK(0,  3, 4), VLC_PACK(0,  3, 4),
    VLC_PACK(0,  0, 3), VLC_PACK(0,  0, 3), VLC_PACK(0,  0, 3), VLC_PACK(0,  0, 3),
    VLC_PACK(0,  0, 3), VLC_PACK(0,  0, 3), VLC_PACK(0,  0, 3), VLC_PACK(0,  0, 3),
    VLC_PACK(0, -7, 6), VLC_PACK(0, -6, 6), VLC_PACK(0, -5, 6), VLC_PACK(0, -4, 6),
    VLC_PACK(0,  4, 6), VLC_PACK(0,  5, 6), VLC_PACK(0,  6, 6), VLC_PACK(0,  7, 6),
};

static const int DC_UV_TAB0[56] = {
    VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2),
    VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2),
    VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2),
    VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2), VLC_PACK(0,  0, 2),
    VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3),
    VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3), VLC_PACK(0, -1, 3),
    VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3),
    VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3), VLC_PACK(0,  1, 3),
    VLC_PACK(0, -3, 4), VLC_PACK(0, -3, 4), VLC_PACK(0, -3, 4), VLC_PACK(0, -3, 4),
    VLC_PACK(0, -2, 4), VLC_PACK(0, -2, 4), VLC_PACK(0, -2, 4), VLC_PACK(0, -2, 4),
    VLC_PACK(0,  2, 4), VLC_PACK(0,  2, 4), VLC_PACK(0,  2, 4), VLC_PACK(0,  2, 4),
    VLC_PACK(0,  3, 4), VLC_PACK(0,  3, 4), VLC_PACK(0,  3, 4), VLC_PACK(0,  3, 4),
    VLC_PACK(0, -7, 6), VLC_PACK(0, -6, 6), VLC_PACK(0, -5, 6), VLC_PACK(0, -4, 6),
    VLC_PACK(0,  4, 6), VLC_PACK(0,  5, 6), VLC_PACK(0,  6, 6), VLC_PACK(0,  7, 6),
};

#define DC_Y_TAB0_SIZE   ((int)(sizeof(DC_Y_TAB0)  / sizeof(DC_Y_TAB0[0])))
#define DC_UV_TAB0_SIZE  ((int)(sizeof(DC_UV_TAB0) / sizeof(DC_UV_TAB0[0])))

/* ====================================================================
 * Decoder state machine */

typedef struct {
    const uint8_t *src;          /* VLC bitstream */
    size_t         src_size;
    size_t         src_offset;   /* read cursor in src (byte) */

    int16_t       *dst;          /* output coefficient array */
    size_t         dst_capacity;
    size_t         dst_offset;

    int            q_code;       /* (quant << 10) — OR'd into first coeff of each block */
    int            version;
    int16_t        last_dc[3];   /* Cr, Cb, Y predictors */

    uint32_t       bit_buffer;
    int            bit_count;    /* -16..0 — count of bits remaining in buffer */

    int            error;        /* 0 = ok, non-zero = corrupt input */
} vlc_state_t;

static uint16_t read_word_le(vlc_state_t *s)
{
    if (s->src_offset + 2 <= s->src_size) {
        uint16_t w = (uint16_t)s->src[s->src_offset]
                   | ((uint16_t)s->src[s->src_offset + 1] << 8);
        s->src_offset += 2;
        return w;
    }
    return 0;
}

static void init_bit_buffer(vlc_state_t *s)
{
    uint32_t low  = read_word_le(s);
    uint32_t high = read_word_le(s);
    s->bit_buffer = (low << 16) | high;
    s->bit_count  = -16;
}

static uint32_t show_bits(vlc_state_t *s, int count)
{
    /* Peek the top `count` bits of the buffer (MSB first). */
    return s->bit_buffer >> (32 - count);
}

static void flush_bits(vlc_state_t *s, int count)
{
    s->bit_buffer <<= count;
    s->bit_count   += count;
    while (s->bit_count >= 0) {
        s->bit_buffer |= (uint32_t)read_word_le(s) << s->bit_count;
        s->bit_count  -= 16;
    }
}

static int write_coeff(vlc_state_t *s, int value)
{
    if (s->dst_offset >= s->dst_capacity) return 0;
    s->dst[s->dst_offset++] = (int16_t)value;
    return 1;
}

/* ====================================================================
 * DC coefficient decode
 *
 * Two formats per the version field:
 *   version == 2: raw 10-bit absolute DC (no predictor)
 *   else:         DC delta lookup against per-component last_dc[]
 */

static int decode_extended_dc(vlc_state_t *s, int comp_index, int is_y)
{
    /* Read variable-length DC delta. Loop expanding bit count until the
     * MSB of the next `bit` slice is 0. Then read the level value. */
    int bit = is_y ? 3 : 4;
    while ((show_bits(s, bit) & 1) != 0 && bit < 16) bit++;
    if (is_y) bit++;

    int nbits = is_y ? bit * 2 - 1 : bit * 2;
    int mask  = (1 << bit) - 1;
    int val   = (int)show_bits(s, nbits) & mask;
    if ((val & (1 << (bit - 1))) == 0) val -= (1 << bit) - 1;

    /* Update predictor — DO NOT mask in extended path (matches Java
     * / depack_vlc reference). The mask is only applied to the short-form
     * table entries below. */
    s->last_dc[comp_index] = (int16_t)(s->last_dc[comp_index] + val * 4);
    return (nbits << 16) | (s->last_dc[comp_index] & 0x3FF);
}

static int decode_dc(vlc_state_t *s, int n_component)
{
    if (s->version == 2) {
        /* Raw 10-bit DC. The 10 bits get used directly as the coefficient. */
        return (int)((show_bits(s, 10) & 0xFFFF) | (10 << 16));
    }

    int bits = (int)show_bits(s, 6);
    if (n_component >= 2) {
        /* Y component (blocks 2..5 of the 6-block macroblock pattern). */
        if (bits < DC_Y_TAB0_SIZE) {
            int entry = DC_Y_TAB0[bits];
            s->last_dc[2] = (int16_t)((s->last_dc[2] + sign_extend_10(entry) * 4) & 0x3FF);
            return (VLC_BITS(entry) << 16) | s->last_dc[2];
        }
        return decode_extended_dc(s, 2, 1);
    } else {
        /* Cr (n=0) or Cb (n=1). */
        if (bits < DC_UV_TAB0_SIZE) {
            int entry = DC_UV_TAB0[bits];
            s->last_dc[n_component] = (int16_t)((s->last_dc[n_component]
                                                 + sign_extend_10(entry) * 4) & 0x3FF);
            return (VLC_BITS(entry) << 16) | s->last_dc[n_component];
        }
        return decode_extended_dc(s, n_component, 0);
    }
}

/* ====================================================================
 * AC coefficient decode (run-length VLC)
 *
 * Returns the packed (bits, run, level) entry. Returns EOB_VLC_CODE when
 * an end-of-block marker is seen. Returns INT32_MIN (-0x80000000) on
 * decoder-failure ("can't find a valid VLC code") — caller should pad
 * the rest of the output and abort. INT_MIN (via <limits.h>) is the
 * portable spelling of "0x80000000 as signed int" — using `-0x80000000`
 * directly is technically unsigned-then-negated and emits MSVC C4146. */
#define VLC_DECODE_FAIL  INT_MIN

static int decode_ac(vlc_state_t *s)
{
    /* sbit = 17 bits is the longest VLC code we'd peek. */
    const int sbit = 17;
    int code = (int)show_bits(s, sbit);

    if (code >= (1 << (sbit - 2))) {
        int entry = VLC_TAB_NEXT[(code >> 12) - 8];
        if (entry == EOB_VLC_CODE) return EOB_VLC_CODE;
        return entry;
    } else if (code >= (1 << (sbit - 6))) {
        int entry = VLC_TAB0[(code >> 8) - 8];
        if (entry == ESCAPE_CODE) {
            /* Escape — read 16 raw bits as the level. Run encoded in the
             * top 6 bits, level in the bottom 10. */
            flush_bits(s, 6);
            return (int)((show_bits(s, 16) & 0xFFFF) | (16 << 16));
        }
        return entry;
    } else if (code >= (1 << (sbit - 7))) {
        return VLC_TAB1[(code >> 6) - 16];
    } else if (code >= (1 << (sbit - 8))) {
        return VLC_TAB2[(code >> 4) - 32];
    } else if (code >= (1 << (sbit - 9))) {
        return VLC_TAB3[(code >> 3) - 32];
    } else if (code >= (1 << (sbit - 10))) {
        return VLC_TAB4[(code >> 2) - 32];
    } else if (code >= (1 << (sbit - 11))) {
        return VLC_TAB5[(code >> 1) - 32];
    } else if (code >= (1 << (sbit - 12))) {
        return VLC_TAB6[(code >> 0) - 32];
    }

    /* Couldn't match any prefix → bitstream exhausted. Pad with EOB
     * sentinels so the consumer can still walk the buffer cleanly. The
     * raw EOB value 0xFE00 sign-extends to -512 in int16 — write it as
     * a negative literal to avoid MSVC C4310 (constant truncation). */
    while (s->dst_offset < s->dst_capacity) {
        s->dst[s->dst_offset++] = (int16_t)-512;
    }
    return VLC_DECODE_FAIL;
}

/* ====================================================================
 * Public entry point */

int re15_bss_vlc_decode(const uint8_t *src_payload, size_t src_size,
                        int run_length_words, int quant, int version,
                        int16_t *dst, size_t dst_capacity)
{
    if (src_payload == NULL || dst == NULL) return -1;
    if (dst_capacity < 2)                   return -1;

    vlc_state_t s;
    s.src          = src_payload;
    s.src_size     = src_size;
    s.src_offset   = 0;
    s.dst          = dst;
    s.dst_capacity = dst_capacity;
    s.dst_offset   = 0;
    s.q_code       = quant << 10;
    s.version      = version;
    s.last_dc[0]   = 0;
    s.last_dc[1]   = 0;
    s.last_dc[2]   = 0;
    s.bit_buffer   = 0;
    s.bit_count    = 0;
    s.error        = 0;

    /* Write the two MDEC frame preamble words. */
    write_coeff(&s, run_length_words);
    write_coeff(&s, RE15_BSS_VLC_ID);

    init_bit_buffer(&s);

    int n = 0;
    int predictive_dc = (version != 2);

    while (s.dst_offset < s.dst_capacity) {
        int code = decode_dc(&s, n);
        if (predictive_dc) {
            n++;
            if (n == 6) n = 0;
        }
        code |= s.q_code;

        while (1) {
            if (!write_coeff(&s, code)) return (int)s.dst_offset;
            flush_bits(&s, VLC_BITS(code));

            int ac = decode_ac(&s);
            if (ac == VLC_DECODE_FAIL) return (int)s.dst_offset;
            if (ac == EOB_VLC_CODE)    break;
            code = ac;
        }

        if (!write_coeff(&s, EOB_VLC_CODE)) return (int)s.dst_offset;
        flush_bits(&s, 2);
    }

    return (int)s.dst_offset;
}
