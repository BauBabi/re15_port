package de.re15.extractors.bss;

import java.util.Arrays;

/**
 * Bytegenaue Portierung der depack_mdec.c Routinen.
 */
final class MdecDecoder {

    private static final int BLOCK_COUNT = 6;
    private static final int BLOCK_SIZE = 64;
    private static final int DCTSIZE = 8;
    private static final int COLUMN_STRIDE = 16 * 3;
    private static final int EOB = 0xFE00;

    private static final int SHIFT = 12;
    private static final int MUL_R = (int) (1.402 * (1 << SHIFT));
    private static final int MUL_G_CB = (int) (-0.3437 * (1 << SHIFT));
    private static final int MUL_G_CR = (int) (-0.7143 * (1 << SHIFT));
    private static final int MUL_B = (int) (1.772 * (1 << SHIFT));

    private static final int CONST_BITS = 8;
    private static final int PASS1_BITS = 2;

    private static final int FIX_1_082392200 = 277;
    private static final int FIX_1_414213562 = 362;
    private static final int FIX_1_847759065 = 473;
    private static final int FIX_2_613125930 = 669;

    private static final int IQ_CONST_BITS = 14;
    private static final int IFAST_SCALE_BITS = 2;

    private static final int[] ROUND_TABLE = new int[256 * 3];

    private static final int[] ZSCAN = {
        0, 1, 8, 16, 9, 2, 3, 10,
        17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13, 6, 7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63
    };

    private static final int[] IQ_TABLE = {
        2, 16, 19, 22, 26, 27, 29, 34,
        16, 16, 22, 24, 27, 29, 34, 37,
        19, 22, 26, 27, 29, 34, 34, 38,
        22, 22, 26, 27, 29, 34, 37, 40,
        22, 26, 27, 29, 32, 35, 40, 48,
        26, 27, 29, 32, 35, 40, 48, 58,
        26, 27, 29, 34, 38, 46, 56, 69,
        27, 29, 35, 38, 46, 56, 69, 83
    };

    private static final int[] AAN_SCALES = {
        16384, 22725, 21407, 19266, 16384, 12873, 8867, 4520,
        22725, 31521, 29692, 26722, 22725, 17855, 12299, 6270,
        21407, 29692, 27969, 25172, 21407, 16819, 11585, 5906,
        19266, 26722, 25172, 22654, 19266, 15137, 10426, 5315,
        16384, 22725, 21407, 19266, 16384, 12873, 8867, 4520,
        12873, 17855, 16819, 15137, 12873, 10114, 6967, 3552,
        8867, 12299, 11585, 10426, 8867, 6967, 4799, 2446,
        4520, 6270, 5906, 5315, 4520, 3552, 2446, 1247
    };

    static {
        for (int i = 0; i < 256; i++) {
            ROUND_TABLE[i] = 0;
            ROUND_TABLE[i + 256] = i;
            ROUND_TABLE[i + 512] = 255;
        }
    }

    private MdecDecoder() {
    }

    static byte[] decode(short[] stream, int width, int height) {
        return decode(stream, width, height, null);
    }

    static byte[] decode(short[] stream, int width, int height, ColumnTracer tracer) {
        if (stream.length < 2 || (stream[1] & 0xFFFF) != VlcDecoder.VLC_ID) {
            throw new IllegalArgumentException("Invalid MDEC stream");
        }
        Context context = new Context(stream, 2);
        context.initIq();

        int alignedHeight = (height + 15) & ~15;
        int macroWidth = (width + 15) >> 4;
        int macroRows = alignedHeight >> 4;
        byte[] columnBytes = new byte[alignedHeight * COLUMN_STRIDE];
        byte[] rgb = new byte[width * height * 3];
        int[] blocks = new int[BLOCK_COUNT * BLOCK_SIZE];

        for (int column = 0; column < macroWidth; column++) {
            Arrays.fill(columnBytes, (byte) 0);
            processColumn(context, blocks, columnBytes, macroRows);
            if (tracer != null) {
                byte[] snapshot = Arrays.copyOf(columnBytes, columnBytes.length);
                tracer.onColumnDecoded(column, snapshot, height, alignedHeight, COLUMN_STRIDE);
            }
            copyColumn(columnBytes, rgb, width, height, column * 16);
        }
        return rgb;
    }

    private static void rl2blk(Context ctx, int[] block) {
        Arrays.fill(block, 0);
        int ptr = 0;
        for (int i = 0; i < BLOCK_COUNT; i++, ptr += BLOCK_SIZE) {
            int rl = ctx.read();
            if (rl == EOB) {
                continue;
            }
            int qscale = rl >>> 10;
            block[ptr] = ctx.iq[0] * valueOf(rl);
            int k = 0;
            while (true) {
                rl = ctx.read();
                if (rl == EOB) {
                    break;
                }
                k += (rl >>> 10) + 1;
                if (k >= ZSCAN.length) {
                    break;
                }
                int idx = ZSCAN[k];
                block[ptr + idx] = (ctx.iq[idx] * qscale * valueOf(rl)) >> 3;
            }
            idct(block, ptr, k + 1);
        }
    }

    private static void processColumn(Context ctx, int[] block, byte[] columnBuffer, int macroRows) {
        int offset = 0;
        for (int i = 0; i < macroRows; i++) {
            rl2blk(ctx, block);
            yuv2rgbColumn(block, columnBuffer, offset);
            offset += 16 * 16 * 3;
        }
    }

    private static void copyColumn(byte[] columnBuffer, byte[] rgb, int width, int height, int columnX) {
        int copyWidth = width - columnX;
        if (copyWidth <= 0) {
            return;
        }
        if (copyWidth > 16) {
            copyWidth = 16;
        }
        int rowStride = width * 3;
        for (int y = 0; y < height; y++) {
            int src = y * COLUMN_STRIDE;
            int dst = y * rowStride + columnX * 3;
            System.arraycopy(columnBuffer, src, rgb, dst, copyWidth * 3);
        }
    }

    private static void yuv2rgbColumn(int[] block, byte[] columnBuffer, int offset) {
        int blkPtr = 0;
        int yPtr = BLOCK_SIZE * 2;
        int imagePtr = offset / 3;
        for (int yy = 0; yy < 16; yy += 2) {
            if (yy == 8) {
                yPtr += BLOCK_SIZE;
            }
            int blkLocal = blkPtr;
            int yLocal = yPtr;
            int imgLocal = imagePtr;
            for (int x = 0; x < 4; x++) {
                writeChromaPair(block, columnBuffer, blkLocal, yLocal, imgLocal);
                blkLocal += 1;
                yLocal += 2;
                imgLocal += 2;
            }
            blkPtr = blkLocal + 4;
            yPtr = yLocal + 8;
            imagePtr = imgLocal + 8 + 16;
        }
    }

    private static void writeChromaPair(int[] block, byte[] columnBuffer, int blkPtr, int yPtr, int imagePtr) {
        // Matching revengi's block interpretation (not standard MDEC):
        // Block 0 is used as "cb" (for B and G), Block 1 is used as "cr" (for R and G)
        // Even though the stream order is actually Cr first, then Cb
        int blk0 = block[blkPtr];               // Block 0 - revengi treats as cb
        int blk1 = block[blkPtr + BLOCK_SIZE];  // Block 1 - revengi treats as cr
        int r0 = mulR(blk1);   // R from blk[64] like revengi's MULR(blk[DCTSIZE2])
        int g0 = mulG(blk0) + mulG2(blk1);  // G from both blocks
        int b0 = mulB(blk0);   // B from blk[0] like revengi's MULB(blk[0])
        writePixel(columnBuffer, imagePtr + 0, block[yPtr + 0], r0, g0, b0);
        writePixel(columnBuffer, imagePtr + 1, block[yPtr + 1], r0, g0, b0);
        writePixel(columnBuffer, imagePtr + 16, block[yPtr + 8], r0, g0, b0);
        writePixel(columnBuffer, imagePtr + 17, block[yPtr + 9], r0, g0, b0);

        int blk0_2 = block[blkPtr + 4];
        int blk1_2 = block[blkPtr + 4 + BLOCK_SIZE];
        int r1 = mulR(blk1_2);
        int g1 = mulG(blk0_2) + mulG2(blk1_2);
        int b1 = mulB(blk0_2);
        int yOffset = yPtr + BLOCK_SIZE;
        writePixel(columnBuffer, imagePtr + 8, block[yOffset + 0], r1, g1, b1);
        writePixel(columnBuffer, imagePtr + 9, block[yOffset + 1], r1, g1, b1);
        writePixel(columnBuffer, imagePtr + 24, block[yOffset + 8], r1, g1, b1);
        writePixel(columnBuffer, imagePtr + 25, block[yOffset + 9], r1, g1, b1);
    }

    private static void writePixel(byte[] columnBuffer, int pixelIndex, int yValue, int rBase, int gBase, int bBase) {
        int y = yValue + 128;
        int dst = pixelIndex * 3;
        // Output BGR order but swap R/B values to match reference files
        // (reference files have R and B swapped in the color values)
        columnBuffer[dst] = (byte) roundToByte(y + rBase);     // B slot gets R value
        columnBuffer[dst + 1] = (byte) roundToByte(y + gBase); // G stays same
        columnBuffer[dst + 2] = (byte) roundToByte(y + bBase); // R slot gets B value
    }

    private static void idct(int[] block, int offset, int nonZeroCount) {
        if (nonZeroCount == 1) {
            int dc = descale(block[offset], PASS1_BITS + 3);
            Arrays.fill(block, offset, offset + BLOCK_SIZE, dc);
            return;
        }

        for (int i = 0; i < DCTSIZE; i++) {
            int ptr = offset + i;
            if ((block[ptr + DCTSIZE] | block[ptr + 2 * DCTSIZE] | block[ptr + 3 * DCTSIZE]
                | block[ptr + 4 * DCTSIZE] | block[ptr + 5 * DCTSIZE]
                | block[ptr + 6 * DCTSIZE] | block[ptr + 7 * DCTSIZE]) == 0) {
                int dc = block[ptr];
                for (int j = 0; j < DCTSIZE; j++) {
                    block[ptr + j * DCTSIZE] = dc;
                }
                continue;
            }

            int z10 = block[ptr] + block[ptr + 4 * DCTSIZE];
            int z11 = block[ptr] - block[ptr + 4 * DCTSIZE];
            int z13 = block[ptr + 2 * DCTSIZE] + block[ptr + 6 * DCTSIZE];
            int z12 = multiply(block[ptr + 2 * DCTSIZE] - block[ptr + 6 * DCTSIZE], FIX_1_414213562) - z13;

            int tmp0 = z10 + z13;
            int tmp3 = z10 - z13;
            int tmp1 = z11 + z12;
            int tmp2 = z11 - z12;

            z13 = block[ptr + 3 * DCTSIZE] + block[ptr + 5 * DCTSIZE];
            z10 = block[ptr + 3 * DCTSIZE] - block[ptr + 5 * DCTSIZE];
            z11 = block[ptr + DCTSIZE] + block[ptr + 7 * DCTSIZE];
            z12 = block[ptr + DCTSIZE] - block[ptr + 7 * DCTSIZE];

            int z5 = multiply(z12 - z10, FIX_1_847759065);
            int tmp7 = z11 + z13;
            int tmp6 = multiply(z10, FIX_2_613125930) + z5 - tmp7;
            int tmp5 = multiply(z11 - z13, FIX_1_414213562) - tmp6;
            int tmp4 = multiply(z12, FIX_1_082392200) - z5 + tmp5;

            block[ptr] = tmp0 + tmp7;
            block[ptr + 7 * DCTSIZE] = tmp0 - tmp7;
            block[ptr + DCTSIZE] = tmp1 + tmp6;
            block[ptr + 6 * DCTSIZE] = tmp1 - tmp6;
            block[ptr + 2 * DCTSIZE] = tmp2 + tmp5;
            block[ptr + 5 * DCTSIZE] = tmp2 - tmp5;
            block[ptr + 4 * DCTSIZE] = tmp3 + tmp4;
            block[ptr + 3 * DCTSIZE] = tmp3 - tmp4;
        }

        for (int i = 0; i < DCTSIZE; i++) {
            int ptr = offset + i * DCTSIZE;
            if ((block[ptr + 1] | block[ptr + 2] | block[ptr + 3]
                | block[ptr + 4] | block[ptr + 5]
                | block[ptr + 6] | block[ptr + 7]) == 0) {
                int dc = descale(block[ptr], PASS1_BITS + 3);
                Arrays.fill(block, ptr, ptr + DCTSIZE, dc);
                continue;
            }

            int z10 = block[ptr] + block[ptr + 4];
            int z11 = block[ptr] - block[ptr + 4];
            int z13 = block[ptr + 2] + block[ptr + 6];
            int z12 = multiply(block[ptr + 2] - block[ptr + 6], FIX_1_414213562) - z13;

            int tmp0 = z10 + z13;
            int tmp3 = z10 - z13;
            int tmp1 = z11 + z12;
            int tmp2 = z11 - z12;

            z13 = block[ptr + 3] + block[ptr + 5];
            z10 = block[ptr + 3] - block[ptr + 5];
            z11 = block[ptr + 1] + block[ptr + 7];
            z12 = block[ptr + 1] - block[ptr + 7];

            int z5 = multiply(z12 - z10, FIX_1_847759065);
            int tmp7 = z11 + z13;
            int tmp6 = multiply(z10, FIX_2_613125930) + z5 - tmp7;
            int tmp5 = multiply(z11 - z13, FIX_1_414213562) - tmp6;
            int tmp4 = multiply(z12, FIX_1_082392200) - z5 + tmp5;

            block[ptr] = descale(tmp0 + tmp7, PASS1_BITS + 3);
            block[ptr + 7] = descale(tmp0 - tmp7, PASS1_BITS + 3);
            block[ptr + 1] = descale(tmp1 + tmp6, PASS1_BITS + 3);
            block[ptr + 6] = descale(tmp1 - tmp6, PASS1_BITS + 3);
            block[ptr + 2] = descale(tmp2 + tmp5, PASS1_BITS + 3);
            block[ptr + 5] = descale(tmp2 - tmp5, PASS1_BITS + 3);
            block[ptr + 4] = descale(tmp3 + tmp4, PASS1_BITS + 3);
            block[ptr + 3] = descale(tmp3 - tmp4, PASS1_BITS + 3);
        }
    }

    private static int valueOf(int code) {
        return (short) (code << 6) >> 6;
    }

    private static int multiply(int value, int constant) {
        return (value * constant) >> CONST_BITS;
    }

    private static int descale(int value, int n) {
        // No rounding - matches reference files better
        return value >> n;
    }

    private static int mulR(int value) {
        return (MUL_R * value) >> SHIFT;
    }

    private static int mulG(int value) {
        return (MUL_G_CB * value) >> SHIFT;
    }

    private static int mulG2(int value) {
        return (MUL_G_CR * value) >> SHIFT;
    }

    private static int mulB(int value) {
        return (MUL_B * value) >> SHIFT;
    }

    private static int roundToByte(int value) {
        int index = value + 256;
        if (index < 0) {
            return 0;
        }
        if (index >= ROUND_TABLE.length) {
            return 255;
        }
        return ROUND_TABLE[index];
    }

    @FunctionalInterface
    interface ColumnTracer {
        void onColumnDecoded(int columnIndex, byte[] columnData, int visibleHeight, int paddedHeight, int columnStride);
    }

    private static final class Context {
        private final short[] stream;
        private final int[] iq = new int[BLOCK_SIZE];
        private int index;

        Context(short[] stream, int index) {
            this.stream = stream;
            this.index = index;
        }

        void initIq() {
            for (int i = 0; i < BLOCK_SIZE; i++) {
                iq[i] = (IQ_TABLE[i] * AAN_SCALES[i]) >> (IQ_CONST_BITS - IFAST_SCALE_BITS);
            }
        }

        int read() {
            if (index >= stream.length) {
                return EOB;
            }
            return stream[index++] & 0xFFFF;
        }
    }
}
