package de.re15.extractors.bss;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * Port der depack_vlc.c Routinen zur Bitstream-Entpackung.
 */
final class VlcDecoder {

    static final int VLC_ID = 0x3800;
    private static final int EOB = 0xFE00;
    private static final int EOB_CODE = packCode(63, 512, 2);
    private static final int ESCAPE_CODE = packCode(63, 0, 6);

    private VlcDecoder() {
    }

    static short[] decode(ByteBuffer bitstream, int length, int quant, int version) {
        Decoder decoder = new Decoder(bitstream.slice().order(ByteOrder.LITTLE_ENDIAN), length, quant, version);
        decoder.decode();
        return decoder.output();
    }

    private static final class Decoder {
        private final ByteBuffer src;
        private final short[] dst;
        private final int dstLimit;
        private final int qCode;
        private final int version;
        private final int[] lastDc = new int[3];
        private int dstOffset;
        private int bitBuffer;
        private int bitCount;

        Decoder(ByteBuffer src, int length, int quant, int version) {
            this.src = src;
            this.dst = new short[(length + 2) * 4];
            this.dstLimit = dst.length;
            this.qCode = quant << 10;
            this.version = version;
            write(length);
            write(VLC_ID);
            initBitBuffer();
        }

        short[] output() {
            return dst;
        }

        void decode() {
            int n = 0;
            lastDc[0] = lastDc[1] = lastDc[2] = 0;
            boolean predictiveDc = version != 2;
            while (dstOffset < dstLimit) {
                int code = decodeDc(n);
                if (predictiveDc) {
                    n++;
                    if (n == 6) {
                        n = 0;
                    }
                }
                code |= qCode;
                while (true) {
                    if (!write(code)) {
                        return;
                    }
                    flushBits(bitsOf(code));
                    int ac = decodeAc();
                    if (ac == Integer.MIN_VALUE) {
                        return;
                    }
                    if (ac == EOB_CODE) {
                        break;
                    }
                    code = ac;
                }
                if (!write(EOB_CODE)) {
                    return;
                }
                flushBits(2);
            }
        }

        private int decodeDc(int componentIndex) {
            if (version == 2) {
                return (showBits(10) & 0xFFFF) | (10 << 16);
            }
            int bits = showBits(6);
            if (componentIndex >= 2) {
                if (bits < DC_Y_TAB0.length) {
                    int entry = DC_Y_TAB0[bits];
                    lastDc[2] = (lastDc[2] + valueOf(entry) * 4) & 0x3FF;
                    return (bitsOf(entry) << 16) | lastDc[2];
                }
                return decodeExtendedDc(2, true);
            } else {
                if (bits < DC_UV_TAB0.length) {
                    int entry = DC_UV_TAB0[bits];
                    lastDc[componentIndex] = (lastDc[componentIndex] + valueOf(entry) * 4) & 0x3FF;
                    return (bitsOf(entry) << 16) | lastDc[componentIndex];
                }
                return decodeExtendedDc(componentIndex, false);
            }
        }

        private int decodeExtendedDc(int index, boolean isY) {
            int bit = isY ? 3 : 4;
            while ((showBits(bit) & 1) != 0 && bit < 16) {
                bit++;
            }
            if (isY) {
                bit++;
            }
            int nbits = isY ? bit * 2 - 1 : bit * 2;
            int mask = (1 << bit) - 1;
            int val = showBits(nbits) & mask;
            if ((val & (1 << (bit - 1))) == 0) {
                val -= (1 << bit) - 1;
            }
            // Don't mask last_dc in extended case (matches C reference)
            lastDc[index] = lastDc[index] + val * 4;
            return (nbits << 16) | (lastDc[index] & 0x3FF);
        }

        private int decodeAc() {
            final int sbit = 17;
            int code = showBits(sbit);
            if (code >= 1 << (sbit - 2)) {
                int entry = VLC_TAB_NEXT[(code >>> 12) - 8];
                if (entry == EOB_CODE) {
                    return EOB_CODE;
                }
                return entry;
            } else if (code >= 1 << (sbit - 6)) {
                int entry = VLC_TAB0[(code >>> 8) - 8];
                if (entry == ESCAPE_CODE) {
                    flushBits(6);
                    return (showBits(16) & 0xFFFF) | (16 << 16);
                }
                return entry;
            } else if (code >= 1 << (sbit - 7)) {
                return VLC_TAB1[(code >>> 6) - 16];
            } else if (code >= 1 << (sbit - 8)) {
                return VLC_TAB2[(code >>> 4) - 32];
            } else if (code >= 1 << (sbit - 9)) {
                return VLC_TAB3[(code >>> 3) - 32];
            } else if (code >= 1 << (sbit - 10)) {
                return VLC_TAB4[(code >>> 2) - 32];
            } else if (code >= 1 << (sbit - 11)) {
                return VLC_TAB5[(code >>> 1) - 32];
            } else if (code >= 1 << (sbit - 12)) {
                return VLC_TAB6[(code >>> 0) - 32];
            }
            while (dstOffset < dstLimit) {
                dst[dstOffset++] = (short) EOB;
            }
            return Integer.MIN_VALUE;
        }

        private void initBitBuffer() {
            int low = readWord();
            int high = readWord();
            bitBuffer = (low << 16) | high;
            bitCount = -16;
        }

        private int readWord() {
            if (src.remaining() >= 2) {
                return Short.toUnsignedInt(src.getShort());
            }
            return 0;
        }

        private int showBits(int count) {
            return bitBuffer >>> (32 - count);
        }

        private void flushBits(int count) {
            bitBuffer <<= count;
            bitCount += count;
            while (bitCount >= 0) {
                bitBuffer |= readWord() << bitCount;
                bitCount -= 16;
            }
        }

        private boolean write(int value) {
            if (dstOffset >= dstLimit) {
                return false;
            }
            dst[dstOffset++] = (short) value;
            return true;
        }
    }

    private static int bitsOf(int code) {
        return code >>> 16;
    }

    private static int valueOf(int code) {
        int level = code & 0x3FF;
        if ((level & 0x200) != 0) {
            level -= 0x400;
        }
        return level;
    }

    private static int packCode(int run, int level, int bits) {
        return (run << 10) | (level & 0x3FF) | (bits << 16);
    }

    private static final int[] VLC_TAB_NEXT = buildVlcTabNext();
    private static final int[] VLC_TAB0 = buildVlcTab0();
    private static final int[] VLC_TAB1 = buildVlcTab1();
    private static final int[] VLC_TAB2 = buildVlcTab2();
    private static final int[] VLC_TAB3 = buildVlcTab3();
    private static final int[] VLC_TAB4 = buildVlcTab4();
    private static final int[] VLC_TAB5 = buildVlcTab5();
    private static final int[] VLC_TAB6 = buildVlcTab6();
    private static final int[] DC_Y_TAB0 = buildDcYTab0();
    private static final int[] DC_UV_TAB0 = buildDcUvTab0();

    private static int[] buildVlcTabNext() {
        TableBuilder builder = new TableBuilder();
        builder.code(0, 2, 4);
        builder.code(2, 1, 4);
        builder.code2(1, 1, 3);
        builder.code2(1, -1, 3);
        builder.code0(63, 512, 2);
        builder.code0(63, 512, 2);
        builder.code0(63, 512, 2);
        builder.code0(63, 512, 2);
        builder.code2(0, 1, 2);
        builder.code2(0, 1, 2);
        builder.code2(0, -1, 2);
        builder.code2(0, -1, 2);
        return builder.toArray();
    }

    private static int[] buildVlcTab0() {
        TableBuilder builder = new TableBuilder();
        builder.code0(63, 0, 6);
        builder.code0(63, 0, 6);
        builder.code0(63, 0, 6);
        builder.code0(63, 0, 6);
        builder.code2(2, 2, 7);
        builder.code2(2, -2, 7);
        builder.code2(9, 1, 7);
        builder.code2(9, -1, 7);
        builder.code2(0, 4, 7);
        builder.code2(0, -4, 7);
        builder.code2(8, 1, 7);
        builder.code2(8, -1, 7);
        builder.code2(7, 1, 6);
        builder.code2(7, 1, 6);
        builder.code2(7, -1, 6);
        builder.code2(7, -1, 6);
        builder.code2(6, 1, 6);
        builder.code2(6, 1, 6);
        builder.code2(6, -1, 6);
        builder.code2(6, -1, 6);
        builder.code2(1, 2, 6);
        builder.code2(1, 2, 6);
        builder.code2(1, -2, 6);
        builder.code2(1, -2, 6);
        builder.code2(5, 1, 6);
        builder.code2(5, 1, 6);
        builder.code2(5, -1, 6);
        builder.code2(5, -1, 6);
        builder.code(13, 1, 8);
        builder.code(0, 6, 8);
        builder.code(12, 1, 8);
        builder.code(11, 1, 8);
        builder.code(3, 2, 8);
        builder.code(1, 3, 8);
        builder.code(0, 5, 8);
        builder.code(10, 1, 8);
        builder.code2(0, 3, 5);
        builder.code2(0, 3, 5);
        builder.code2(0, 3, 5);
        builder.code2(0, 3, 5);
        builder.code2(0, -3, 5);
        builder.code2(0, -3, 5);
        builder.code2(0, -3, 5);
        builder.code2(0, -3, 5);
        builder.code2(4, 1, 5);
        builder.code2(4, 1, 5);
        builder.code2(4, 1, 5);
        builder.code2(4, 1, 5);
        builder.code2(4, -1, 5);
        builder.code2(4, -1, 5);
        builder.code2(4, -1, 5);
        builder.code2(4, -1, 5);
        builder.code2(3, 1, 5);
        builder.code2(3, 1, 5);
        builder.code2(3, 1, 5);
        builder.code2(3, 1, 5);
        builder.code2(3, -1, 5);
        builder.code2(3, -1, 5);
        builder.code2(3, -1, 5);
        builder.code2(3, -1, 5);
        return builder.toArray();
    }

    private static int[] buildVlcTab1() {
        TableBuilder builder = new TableBuilder();
        builder.code(16, 1, 10);
        builder.code(5, 2, 10);
        builder.code(0, 7, 10);
        builder.code(2, 3, 10);
        builder.code(1, 4, 10);
        builder.code(15, 1, 10);
        builder.code(14, 1, 10);
        builder.code(4, 2, 10);
        return builder.toArray();
    }

    private static int[] buildVlcTab2() {
        TableBuilder builder = new TableBuilder();
        builder.code(0, 11, 12);
        builder.code(8, 2, 12);
        builder.code(4, 3, 12);
        builder.code(0, 10, 12);
        builder.code(2, 4, 12);
        builder.code(7, 2, 12);
        builder.code(21, 1, 12);
        builder.code(20, 1, 12);
        builder.code(0, 9, 12);
        builder.code(19, 1, 12);
        builder.code(18, 1, 12);
        builder.code(1, 5, 12);
        builder.code(3, 3, 12);
        builder.code(0, 8, 12);
        builder.code(6, 2, 12);
        builder.code(17, 1, 12);
        return builder.toArray();
    }

    private static int[] buildVlcTab3() {
        TableBuilder builder = new TableBuilder();
        builder.code(10, 2, 13);
        builder.code(9, 2, 13);
        builder.code(5, 3, 13);
        builder.code(3, 4, 13);
        builder.code(2, 5, 13);
        builder.code(1, 7, 13);
        builder.code(1, 6, 13);
        builder.code(0, 15, 13);
        builder.code(0, 14, 13);
        builder.code(0, 13, 13);
        builder.code(0, 12, 13);
        builder.code(26, 1, 13);
        builder.code(25, 1, 13);
        builder.code(24, 1, 13);
        builder.code(23, 1, 13);
        builder.code(22, 1, 13);
        return builder.toArray();
    }

    private static int[] buildVlcTab4() {
        TableBuilder builder = new TableBuilder();
        for (int v = 31; v >= 16; v--) {
            builder.code(0, v, 14);
        }
        return builder.toArray();
    }

    private static int[] buildVlcTab5() {
        TableBuilder builder = new TableBuilder();
        for (int v = 40; v >= 32; v--) {
            builder.code(0, v, 15);
        }
        for (int v = 14; v >= 8; v--) {
            builder.code(1, v, 15);
        }
        return builder.toArray();
    }

    private static int[] buildVlcTab6() {
        TableBuilder builder = new TableBuilder();
        for (int v = 18; v >= 15; v--) {
            builder.code(1, v, 16);
        }
        builder.code(6, 3, 16);
        builder.code(16, 2, 16);
        builder.code(15, 2, 16);
        builder.code(14, 2, 16);
        builder.code(13, 2, 16);
        builder.code(12, 2, 16);
        builder.code(11, 2, 16);
        for (int v = 31; v >= 27; v--) {
            builder.code(v, 1, 16);
        }
        return builder.toArray();
    }

    private static int[] buildDcYTab0() {
        return new int[] {
            packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3),
            packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3),
            packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3),
            packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3),
            packCode(0, -3, 4), packCode(0, -3, 4), packCode(0, -3, 4), packCode(0, -3, 4),
            packCode(0, -2, 4), packCode(0, -2, 4), packCode(0, -2, 4), packCode(0, -2, 4),
            packCode(0, 2, 4), packCode(0, 2, 4), packCode(0, 2, 4), packCode(0, 2, 4),
            packCode(0, 3, 4), packCode(0, 3, 4), packCode(0, 3, 4), packCode(0, 3, 4),
            packCode(0, 0, 3), packCode(0, 0, 3), packCode(0, 0, 3), packCode(0, 0, 3),
            packCode(0, 0, 3), packCode(0, 0, 3), packCode(0, 0, 3), packCode(0, 0, 3),
            packCode(0, -7, 6), packCode(0, -6, 6), packCode(0, -5, 6), packCode(0, -4, 6),
            packCode(0, 4, 6), packCode(0, 5, 6), packCode(0, 6, 6), packCode(0, 7, 6)
        };
    }

    private static int[] buildDcUvTab0() {
        return new int[] {
            packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2),
            packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2),
            packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2),
            packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2), packCode(0, 0, 2),
            packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3),
            packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3), packCode(0, -1, 3),
            packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3),
            packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3), packCode(0, 1, 3),
            packCode(0, -3, 4), packCode(0, -3, 4), packCode(0, -3, 4), packCode(0, -3, 4),
            packCode(0, -2, 4), packCode(0, -2, 4), packCode(0, -2, 4), packCode(0, -2, 4),
            packCode(0, 2, 4), packCode(0, 2, 4), packCode(0, 2, 4), packCode(0, 2, 4),
            packCode(0, 3, 4), packCode(0, 3, 4), packCode(0, 3, 4), packCode(0, 3, 4),
            packCode(0, -7, 6), packCode(0, -6, 6), packCode(0, -5, 6), packCode(0, -4, 6),
            packCode(0, 4, 6), packCode(0, 5, 6), packCode(0, 6, 6), packCode(0, 7, 6)
        };
    }

    private static final class TableBuilder {
        private final List<Integer> values = new ArrayList<>();

        void code(int run, int level, int bits) {
            values.add(packCode(run, level, bits + 1));
            values.add(packCode(run, -level, bits + 1));
        }

        void code0(int run, int level, int bits) {
            values.add(packCode(run, level, bits));
            values.add(packCode(run, level, bits));
        }

        void code2(int run, int level, int bits) {
            values.add(packCode(run, level, bits + 1));
            values.add(packCode(run, level, bits + 1));
        }

        int[] toArray() {
            int[] arr = new int[values.size()];
            for (int i = 0; i < values.size(); i++) {
                arr[i] = values.get(i);
            }
            return arr;
        }
    }
}
