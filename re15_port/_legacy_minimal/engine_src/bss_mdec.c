/*
 * RE1.5 Rebuilt — Software MDEC (IDCT + dequant + YUV→RGB)
 * (Phase 4.5.6.4, 2026-05-18).
 *
 * Used by the PC target only — PSX uses the hardware MDEC chip via
 * libpsxpress. This file ports src/main/java/de/re15/extractors/bss/
 * MdecDecoder.java which itself is a byte-equivalent translation of
 * revengi/depack_mdec.c (Patrice Mandin's reference reader, GPL —
 * studied for the algorithm only; this is an independent C
 * implementation).
 *
 * Two notable quirks preserved from the reference:
 *
 *   1. Block 0 is treated as Cb and Block 1 as Cr — the inverse of
 *      what some MDEC docs say. This is what revengi does and what our
 *      Java extractor uses to produce the BMPs in already_extracted/.
 *      Keeps PC and "extractor reference" output identical.
 *
 *   2. The descale() step uses pure shift, no rounding term. Again
 *      matches the reference; introduces slight bias vs IEEE/JPEG
 *      reference IDCT but again keeps our BMPs reproducible.
 *
 * The pipeline:
 *
 *   for each 16-pixel-wide column of macroblocks:
 *     for each macroblock down the column:
 *       rl2blk()         — read 6 blocks, dequantize, inverse-zigzag
 *       idct(block)      — 2D 8×8 IDCT (AAN scaled algorithm)
 *       yuv2rgb_column() — combine Y0..Y3 + upsampled Cr/Cb into RGB
 *     copy_column()      — paste 16-wide column into output framebuffer
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "re15_bss_mdec.h"

#define BLOCK_COUNT      6
#define BLOCK_SIZE      64
#define DCTSIZE          8
#define COLUMN_STRIDE   (16 * 3)   /* 16 px × 3 bytes (R, G, B) per pixel  */
#define EOB_RAW         0xFE00     /* End-of-block sentinel in coeff stream */

/* YUV → RGB scalars, Q12 fixed point (× 2^12). Standard BT.601:
 *   R = Y                 + 1.402   * (Cr - 128)
 *   G = Y - 0.3437 * (Cb - 128) - 0.7143 * (Cr - 128)
 *   B = Y + 1.772   * (Cb - 128)
 * Java rounds the floats at build time to the same constants below. */
#define YUV_SHIFT         12
#define MUL_R          5743      /* round( 1.402  * 4096) */
#define MUL_G_CB      -1408      /* round(-0.3437 * 4096) */
#define MUL_G_CR      -2925      /* round(-0.7143 * 4096) */
#define MUL_B          7258      /* round( 1.772  * 4096) */

#define CONST_BITS        8
#define PASS1_BITS        2

/* IDCT trig constants (Q8 fixed-point Loeffler/AAN). */
#define FIX_1_082392200   277
#define FIX_1_414213562   362
#define FIX_1_847759065   473
#define FIX_2_613125930   669

#define IQ_CONST_BITS    14
#define IFAST_SCALE_BITS  2

/* Zigzag scan order — JPEG/MDEC standard. The 8×8 DCT block is stored
 * in this serpentine order in the coefficient stream; inverse-zigzag
 * lays them back into a raster 8×8 for the IDCT. */
static const uint8_t ZSCAN[64] = {
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63,
};

/* Inverse quantization table — JPEG luma quant scaled for our IDCT. */
static const uint8_t IQ_TABLE[64] = {
     2, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83,
};

/* AAN (Arai-Agui-Nakajima) scales — pre-multiplied into IQ at init. */
static const int32_t AAN_SCALES[64] = {
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
    21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
    19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
    16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
    12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
     8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
     4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247,
};

/* Saturation table — pre-built clamp from [-256..511] → [0..255] using
 * 3 zones (low=0, middle=identity, high=255). The +256 input offset
 * lets us index with a possibly-negative computed pixel value. */
static uint8_t s_round_table[256 * 3];
static int     s_round_table_built = 0;

static void build_round_table(void)
{
    if (s_round_table_built) return;
    for (int i = 0; i < 256; i++) {
        s_round_table[i]         = 0;
        s_round_table[i + 256]   = (uint8_t)i;
        s_round_table[i + 512]   = 255;
    }
    s_round_table_built = 1;
}

static inline uint8_t saturate_byte(int value)
{
    int index = value + 256;
    if (index < 0)                      return 0;
    if (index >= (int)sizeof(s_round_table)) return 255;
    return s_round_table[index];
}

/* Run-length code helpers */
static inline int rl_run(int code)   { return (code >> 10) & 0x3F; }
static inline int rl_value(int code) {
    /* 10-bit signed level (sign-extend from bit 9). */
    int level = (int)(int16_t)((int16_t)(code << 6) >> 6);
    return level;
}

static inline int multiply(int value, int constant) {
    return (value * constant) >> CONST_BITS;
}

/* No rounding — bit-equivalent to Java port. */
static inline int descale(int value, int n) {
    return value >> n;
}

/* ===== rl2blk: decompress 6 blocks of DCT coeffs into a 6*64 buffer ===== */

typedef struct {
    const int16_t *stream;
    size_t         stream_size;
    size_t         cursor;
    int32_t        iq[BLOCK_SIZE];
} mdec_ctx_t;

static int ctx_read(mdec_ctx_t *ctx)
{
    if (ctx->cursor >= ctx->stream_size) return EOB_RAW;
    return (int)(uint16_t)ctx->stream[ctx->cursor++];
}

static void ctx_init_iq(mdec_ctx_t *ctx)
{
    for (int i = 0; i < BLOCK_SIZE; i++) {
        ctx->iq[i] = (int32_t)((IQ_TABLE[i] * AAN_SCALES[i])
                                >> (IQ_CONST_BITS - IFAST_SCALE_BITS));
    }
}

/* Forward decl — defined after rl2blk to keep file readable. */
static void idct(int32_t *block, int offset, int non_zero_count);

static void rl2blk(mdec_ctx_t *ctx, int32_t *block_buf)
{
    memset(block_buf, 0, BLOCK_COUNT * BLOCK_SIZE * sizeof(int32_t));

    int ptr = 0;
    for (int i = 0; i < BLOCK_COUNT; i++, ptr += BLOCK_SIZE) {
        int rl = ctx_read(ctx);
        if (rl == EOB_RAW) continue;

        int qscale = (rl >> 10) & 0x3F;
        block_buf[ptr] = ctx->iq[0] * rl_value(rl);

        int k = 0;
        while (1) {
            rl = ctx_read(ctx);
            if (rl == EOB_RAW) break;
            k += rl_run(rl) + 1;
            if (k >= 64) break;
            int idx = ZSCAN[k];
            block_buf[ptr + idx] = (ctx->iq[idx] * qscale * rl_value(rl)) >> 3;
        }
        idct(block_buf, ptr, k + 1);
    }
}

/* ===== 2D 8×8 IDCT (AAN Loeffler-style scaled-output variant) =========== */

static void idct(int32_t *block, int offset, int non_zero_count)
{
    if (non_zero_count == 1) {
        int dc = descale(block[offset], PASS1_BITS + 3);
        for (int i = 0; i < BLOCK_SIZE; i++) block[offset + i] = dc;
        return;
    }

    /* Pass 1: per-column IDCT (8 columns of 8 rows). */
    for (int i = 0; i < DCTSIZE; i++) {
        int p = offset + i;

        int b1 = block[p + 1*DCTSIZE], b2 = block[p + 2*DCTSIZE];
        int b3 = block[p + 3*DCTSIZE], b4 = block[p + 4*DCTSIZE];
        int b5 = block[p + 5*DCTSIZE], b6 = block[p + 6*DCTSIZE];
        int b7 = block[p + 7*DCTSIZE];

        if ((b1 | b2 | b3 | b4 | b5 | b6 | b7) == 0) {
            int dc = block[p];
            for (int j = 0; j < DCTSIZE; j++) block[p + j*DCTSIZE] = dc;
            continue;
        }

        int z10 = block[p] + b4;
        int z11 = block[p] - b4;
        int z13 = b2 + b6;
        int z12 = multiply(b2 - b6, FIX_1_414213562) - z13;

        int tmp0 = z10 + z13;
        int tmp3 = z10 - z13;
        int tmp1 = z11 + z12;
        int tmp2 = z11 - z12;

        z13 = b3 + b5;
        z10 = b3 - b5;
        z11 = b1 + b7;
        z12 = b1 - b7;

        int z5   = multiply(z12 - z10, FIX_1_847759065);
        int tmp7 = z11 + z13;
        int tmp6 = multiply(z10, FIX_2_613125930) + z5 - tmp7;
        int tmp5 = multiply(z11 - z13, FIX_1_414213562) - tmp6;
        int tmp4 = multiply(z12, FIX_1_082392200) - z5 + tmp5;

        block[p]            = tmp0 + tmp7;
        block[p + 7*DCTSIZE] = tmp0 - tmp7;
        block[p + 1*DCTSIZE] = tmp1 + tmp6;
        block[p + 6*DCTSIZE] = tmp1 - tmp6;
        block[p + 2*DCTSIZE] = tmp2 + tmp5;
        block[p + 5*DCTSIZE] = tmp2 - tmp5;
        block[p + 4*DCTSIZE] = tmp3 + tmp4;
        block[p + 3*DCTSIZE] = tmp3 - tmp4;
    }

    /* Pass 2: per-row IDCT. */
    for (int i = 0; i < DCTSIZE; i++) {
        int p = offset + i * DCTSIZE;

        int b1 = block[p + 1], b2 = block[p + 2];
        int b3 = block[p + 3], b4 = block[p + 4];
        int b5 = block[p + 5], b6 = block[p + 6];
        int b7 = block[p + 7];

        if ((b1 | b2 | b3 | b4 | b5 | b6 | b7) == 0) {
            int dc = descale(block[p], PASS1_BITS + 3);
            for (int j = 0; j < DCTSIZE; j++) block[p + j] = dc;
            continue;
        }

        int z10 = block[p] + b4;
        int z11 = block[p] - b4;
        int z13 = b2 + b6;
        int z12 = multiply(b2 - b6, FIX_1_414213562) - z13;

        int tmp0 = z10 + z13;
        int tmp3 = z10 - z13;
        int tmp1 = z11 + z12;
        int tmp2 = z11 - z12;

        z13 = b3 + b5;
        z10 = b3 - b5;
        z11 = b1 + b7;
        z12 = b1 - b7;

        int z5   = multiply(z12 - z10, FIX_1_847759065);
        int tmp7 = z11 + z13;
        int tmp6 = multiply(z10, FIX_2_613125930) + z5 - tmp7;
        int tmp5 = multiply(z11 - z13, FIX_1_414213562) - tmp6;
        int tmp4 = multiply(z12, FIX_1_082392200) - z5 + tmp5;

        block[p]     = descale(tmp0 + tmp7, PASS1_BITS + 3);
        block[p + 7] = descale(tmp0 - tmp7, PASS1_BITS + 3);
        block[p + 1] = descale(tmp1 + tmp6, PASS1_BITS + 3);
        block[p + 6] = descale(tmp1 - tmp6, PASS1_BITS + 3);
        block[p + 2] = descale(tmp2 + tmp5, PASS1_BITS + 3);
        block[p + 5] = descale(tmp2 - tmp5, PASS1_BITS + 3);
        block[p + 4] = descale(tmp3 + tmp4, PASS1_BITS + 3);
        block[p + 3] = descale(tmp3 - tmp4, PASS1_BITS + 3);
    }
}

/* ===== YUV → RGB for one 16×16 macroblock ============================== */

static inline int mul_r(int v)    { return (MUL_R    * v) >> YUV_SHIFT; }
static inline int mul_g_cb(int v) { return (MUL_G_CB * v) >> YUV_SHIFT; }
static inline int mul_g_cr(int v) { return (MUL_G_CR * v) >> YUV_SHIFT; }
static inline int mul_b(int v)    { return (MUL_B    * v) >> YUV_SHIFT; }

static void write_pixel(uint8_t *col_buf, int pixel_idx,
                        int y_value, int r_base, int g_base, int b_base)
{
    int yv = y_value + 128;
    int dst = pixel_idx * 3;
    /* Memory layout = [R, G, B] per pixel. Note: the revengi-derived
     * Cr/Cb interpretation is swapped vs std MDEC; see file header. */
    col_buf[dst    ] = saturate_byte(yv + r_base);
    col_buf[dst + 1] = saturate_byte(yv + g_base);
    col_buf[dst + 2] = saturate_byte(yv + b_base);
}

static void yuv2rgb_column(const int32_t *block, uint8_t *col_buf, int byte_offset)
{
    /* Block layout in `block[]` (6 blocks × 64 coeffs):
     *   [0]   Cr  (treated as Cb per revengi quirk)
     *   [64]  Cb  (treated as Cr per revengi quirk)
     *   [128] Y0  upper-left  8×8
     *   [192] Y1  upper-right 8×8
     *   [256] Y2  lower-left  8×8
     *   [320] Y3  lower-right 8×8
     *
     * Chroma is 8×8 covering the full 16×16 macroblock — each chroma
     * sample shared by a 2×2 luma cell. We iterate yy=0..14 step 2 to
     * walk chroma; the inner work writes 4 luma pixels per chroma sample.
     */
    int blk_ptr  = 0;                  /* index into chroma block 0 */
    int y_ptr    = BLOCK_SIZE * 2;     /* index into Y0  */
    int img_pix  = byte_offset / 3;    /* image pixel index */

    for (int yy = 0; yy < 16; yy += 2) {
        if (yy == 8) {
            /* Cross from upper Y0/Y1 to lower Y2/Y3 row. */
            y_ptr += BLOCK_SIZE;
        }
        int blk_local = blk_ptr;
        int y_local   = y_ptr;
        int img_local = img_pix;

        for (int x = 0; x < 4; x++) {
            /* One iteration emits a 4×2 luma cell (left half of MB row).
             *
             * Phase 4.5.10-C: dropped the revengi Cb/Cr swap. PSX hardware
             * MDEC interprets block 0 as Cr (V) and block 1 as Cb (U), i.e.
             * the standard MDEC layout. Matching that makes PC colours
             * agree with PSX. The reference BMPs in already_extracted/
             * have the swap baked in, so they no longer round-trip — keep
             * the engine output as the new reference. */
            int blk0 = block[blk_local + BLOCK_SIZE];     /* Cb (U)  */
            int blk1 = block[blk_local];                  /* Cr (V)  */

            int r0 = mul_r(blk1);
            int g0 = mul_g_cb(blk0) + mul_g_cr(blk1);
            int b0 = mul_b(blk0);

            write_pixel(col_buf, img_local + 0,    block[y_local + 0], r0, g0, b0);
            write_pixel(col_buf, img_local + 1,    block[y_local + 1], r0, g0, b0);
            write_pixel(col_buf, img_local + 16,   block[y_local + 8], r0, g0, b0);
            write_pixel(col_buf, img_local + 17,   block[y_local + 9], r0, g0, b0);

            /* Right half: same chroma, different luma block (Y1 / Y3) */
            int blk0_r = block[blk_local + 4 + BLOCK_SIZE];  /* Cb (U)  */
            int blk1_r = block[blk_local + 4];               /* Cr (V)  */
            int r1 = mul_r(blk1_r);
            int g1 = mul_g_cb(blk0_r) + mul_g_cr(blk1_r);
            int b1 = mul_b(blk0_r);

            int y_off = y_local + BLOCK_SIZE;             /* Y1 (or Y3 in lower row) */
            write_pixel(col_buf, img_local + 8,  block[y_off + 0], r1, g1, b1);
            write_pixel(col_buf, img_local + 9,  block[y_off + 1], r1, g1, b1);
            write_pixel(col_buf, img_local + 24, block[y_off + 8], r1, g1, b1);
            write_pixel(col_buf, img_local + 25, block[y_off + 9], r1, g1, b1);

            blk_local += 1;
            y_local   += 2;
            img_local += 2;
        }

        blk_ptr  = blk_local + 4;       /* skip the 4 right-half chroma samples already covered */
        y_ptr    = y_local   + 8;       /* next luma row in current Y block */
        img_pix  = img_local + 8 + 16;  /* next image row (16-pixel-wide column) */
    }
}

/* ===== Public entry point ============================================= */

int re15_bss_mdec_decode(const int16_t *coeffs, size_t coeff_count,
                         int width, int height,
                         uint8_t *rgb_out)
{
    if (coeffs == NULL || rgb_out == NULL)               return -1;
    if (coeff_count < 2)                                  return -1;
    if ((uint16_t)coeffs[1] != RE15_BSS_VLC_ID)           return -1;
    if (width <= 0 || height <= 0)                        return -1;

    build_round_table();

    mdec_ctx_t ctx;
    ctx.stream      = coeffs;
    ctx.stream_size = coeff_count;
    ctx.cursor      = 2;                /* skip the 2-word preamble */
    ctx_init_iq(&ctx);

    int aligned_h = (height + 15) & ~15;
    int macro_w   = (width  + 15) >> 4;
    int macro_rows = aligned_h >> 4;

    /* Working buffers: one 16-wide column of decoded RGB, one 6-block
     * coefficient scratch. ~30 KB + 1.5 KB stack-or-static — keep static
     * to avoid blowing the C stack on PSX/PC. */
    static uint8_t  s_col_buf[16 * 3 * 256];   /* 16 × aligned_h × 3 bytes */
    static int32_t  s_blocks[BLOCK_COUNT * BLOCK_SIZE];

    int row_stride = width * 3;

    for (int col = 0; col < macro_w; col++) {
        memset(s_col_buf, 0, sizeof(s_col_buf));

        int byte_offset = 0;
        for (int mb = 0; mb < macro_rows; mb++) {
            rl2blk(&ctx, s_blocks);
            yuv2rgb_column(s_blocks, s_col_buf, byte_offset);
            byte_offset += 16 * 16 * 3;
        }

        /* Copy the 16-wide column into the final RGB image. The last
         * column may be partial if width % 16 != 0; clamp the width.
         * Iterate rows to deal with the column stride. */
        int copy_w = width - col * 16;
        if (copy_w <= 0) continue;
        if (copy_w > 16) copy_w = 16;
        int copy_bytes = copy_w * 3;

        for (int y = 0; y < height; y++) {
            const uint8_t *src = s_col_buf + y * COLUMN_STRIDE;
            uint8_t       *dst = rgb_out + y * row_stride + col * 16 * 3;
            memcpy(dst, src, (size_t)copy_bytes);
        }
    }

    return 0;
}
