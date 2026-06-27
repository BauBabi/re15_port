package de.re15.extractors.audio;

/**
 * Represents the usable portion of a single sample inside a VB file.
 */
public final class SampleDataWindow {
    private final int chunkOffset;
    private final int chunkLength;
    private final int dataOffset;
    private final int dataLength;

    private SampleDataWindow(int chunkOffset, int chunkLength, int dataOffset, int dataLength) {
        this.chunkOffset = chunkOffset;
        this.chunkLength = chunkLength;
        this.dataOffset = dataOffset;
        this.dataLength = dataLength;
    }

    public static SampleDataWindow from(byte[] vbData, VabFile.Sample sample, SampleLayout layout) {
        if (vbData == null || sample == null || layout == null) {
            return null;
        }
        int safeOffset = clamp(sample.offset(), 0, vbData.length);
        int chunkLength = Math.min(sample.length(), Math.max(0, vbData.length - safeOffset));
        if (chunkLength <= layout.prefixBytes()) {
            return null;
        }
        int dataOffset = safeOffset + layout.prefixBytes();
        if (dataOffset >= vbData.length) {
            return null;
        }
        int payloadLength = chunkLength - layout.prefixBytes();
        int available = Math.max(0, vbData.length - dataOffset);
        int usableLength = Math.min(payloadLength, available);
        if (usableLength <= 0) {
            return null;
        }
        return new SampleDataWindow(safeOffset, chunkLength, dataOffset, usableLength);
    }

    static SampleDataWindow create(int chunkOffset, int chunkLength, int dataOffset, int dataLength) {
        if (chunkLength <= 0 || dataLength <= 0) {
            return null;
        }
        return new SampleDataWindow(chunkOffset, chunkLength, dataOffset, dataLength);
    }

    private static int clamp(int value, int min, int max) {
        if (value < min) {
            return min;
        }
        if (value > max) {
            return max;
        }
        return value;
    }

    public int chunkOffset() {
        return chunkOffset;
    }

    public int chunkLength() {
        return chunkLength;
    }

    public int dataOffset() {
        return dataOffset;
    }

    public int dataLength() {
        return dataLength;
    }

    /**
     * Returns the payload length that can be consumed in whole ADPCM frames.
     */
    public int alignedDataLength(int alignment) {
        if (alignment <= 0) {
            return dataLength;
        }
        return dataLength - (dataLength % alignment);
    }
}
