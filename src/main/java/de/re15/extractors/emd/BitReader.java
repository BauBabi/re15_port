package de.re15.extractors.emd;

public final class BitReader {
    private final byte[] data;
    private int bitOffset;

    public BitReader(byte[] data) {
        this.data = data;
        this.bitOffset = 0;
    }

    public int read(int count) {
        if (count > 32) {
            throw new IllegalArgumentException("Cannot read more than 32 bits at once");
        }
        int value = 0;
        for (int bitIndex = 0; bitIndex < count; bitIndex++) {
            int byteIndex = (bitOffset + bitIndex) >>> 3;
            int innerIndex = (bitOffset + bitIndex) & 7;
            if (byteIndex >= data.length) {
                break;
            }
            int bit = (data[byteIndex] >>> innerIndex) & 1;
            value |= (bit << bitIndex);
        }
        bitOffset += count;
        return value;
    }

    public int getBitOffset() {
        return bitOffset;
    }
}
