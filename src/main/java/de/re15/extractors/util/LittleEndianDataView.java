package de.re15.extractors.util;

import java.util.Arrays;

/**
 * Minimal helper to read little-endian integers from a backing byte array.
 */
public final class LittleEndianDataView {
    private final byte[] data;

    public LittleEndianDataView(byte[] data) {
        this.data = data;
    }

    public int readInt(int offset) {
        if (outOfBounds(offset, 4)) {
            return 0;
        }
        return (Byte.toUnsignedInt(data[offset])
                | (Byte.toUnsignedInt(data[offset + 1]) << 8)
                | (Byte.toUnsignedInt(data[offset + 2]) << 16)
                | (Byte.toUnsignedInt(data[offset + 3]) << 24));
    }

    public int readUnsignedShort(int offset) {
        if (outOfBounds(offset, 2)) {
            return 0;
        }
        return (Byte.toUnsignedInt(data[offset])
                | (Byte.toUnsignedInt(data[offset + 1]) << 8));
    }

    public byte[] slice(int offset, int length) {
        if (length <= 0 || outOfBounds(offset, length)) {
            return new byte[0];
        }
        return Arrays.copyOfRange(data, offset, offset + length);
    }

    private boolean outOfBounds(int offset, int size) {
        return offset < 0 || offset + size > data.length;
    }
}
