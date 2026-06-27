package de.re15.extractors.emd;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Arrays;

final class GltfBufferBuilder {
    private final ByteArrayOutputStream buffer = new ByteArrayOutputStream();

    int add(byte[] data, int alignment) {
        align(alignment);
        int offset = buffer.size();
        try {
            buffer.write(data);
        } catch (IOException e) {
            throw new IllegalStateException("Failed to write buffer", e);
        }
        return offset;
    }

    int add(float[] values, int alignment) {
        byte[] bytes = new byte[values.length * 4];
        for (int i = 0; i < values.length; i++) {
            int intBits = Float.floatToIntBits(values[i]);
            bytes[i * 4] = (byte) (intBits & 0xFF);
            bytes[i * 4 + 1] = (byte) ((intBits >> 8) & 0xFF);
            bytes[i * 4 + 2] = (byte) ((intBits >> 16) & 0xFF);
            bytes[i * 4 + 3] = (byte) ((intBits >> 24) & 0xFF);
        }
        return add(bytes, alignment);
    }

    private void align(int alignment) {
        int padding = paddingNeeded(alignment);
        if (padding > 0) {
            byte[] zeros = new byte[padding];
            Arrays.fill(zeros, (byte) 0);
            try {
                buffer.write(zeros);
            } catch (IOException e) {
                throw new IllegalStateException(e);
            }
        }
    }

    private int paddingNeeded(int alignment) {
        int size = buffer.size();
        int remainder = size % alignment;
        return remainder == 0 ? 0 : alignment - remainder;
    }

    byte[] toByteArray() {
        return buffer.toByteArray();
    }

    int size() {
        return buffer.size();
    }
}
