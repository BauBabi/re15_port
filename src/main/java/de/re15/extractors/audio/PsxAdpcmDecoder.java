package de.re15.extractors.audio;

import java.util.Arrays;

/**
 * Decodes standard PlayStation ADPCM (a.k.a. VAG) data into 16-bit PCM.
 */
final class PsxAdpcmDecoder {
    private static final int[][] FILTERS = {
        {0, 0},
        {60, 0},
        {115, -52},
        {98, -55},
        {122, -60}
    };

    short[] decode(byte[] data, int offset, int length) {
        if (data == null || offset < 0 || length <= 0 || offset >= data.length) {
            return new short[0];
        }
        int available = Math.min(length, data.length - offset);
        int frames = Math.max(0, available / 16);
        if (frames == 0) {
            return new short[0];
        }
        short[] pcm = new short[frames * 28];
        int pcmPos = 0;
        state1 = 0;
        state2 = 0;
        for (int frame = 0; frame < frames; frame++) {
            int frameOffset = offset + frame * 16;
            int header = Byte.toUnsignedInt(data[frameOffset]);
            int filter = (header >> 4) & 0xF;
            int shift = header & 0xF;
            int[] coeffs = FILTERS[Math.min(filter, FILTERS.length - 1)];
            for (int i = 0; i < 14; i++) {
                int sampleByte = Byte.toUnsignedInt(data[frameOffset + 2 + i]);
                pcmPos = decodeNibble(sampleByte & 0xF, shift, coeffs, pcm, pcmPos);
                pcmPos = decodeNibble(sampleByte >>> 4, shift, coeffs, pcm, pcmPos);
            }
            int flags = Byte.toUnsignedInt(data[frameOffset + 1]);
            if ((flags & 0x4) != 0) {
                state1 = 0;
                state2 = 0;
            }
        }
        if (pcmPos == pcm.length) {
            return pcm;
        }
        return Arrays.copyOf(pcm, pcmPos);
    }

    private int decodeNibble(int nibble, int shift, int[] coeffs, short[] pcm, int pos) {
        int sample = nibble;
        if ((sample & 0x8) != 0) {
            sample -= 16;
        }
        int value = sample << 12;
        value >>= shift;
        value += ((state1 * coeffs[0]) + (state2 * coeffs[1]) + 32) >> 6;
        if (value < Short.MIN_VALUE) {
            value = Short.MIN_VALUE;
        } else if (value > Short.MAX_VALUE) {
            value = Short.MAX_VALUE;
        }
        state2 = state1;
        state1 = value;
        pcm[pos] = (short) value;
        return pos + 1;
    }

    private int state1;
    private int state2;
}
