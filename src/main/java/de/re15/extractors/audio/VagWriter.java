package de.re15.extractors.audio;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

/**
 * Writes ADPCM payloads into PS1 VAG files so they can be verified in PSound.
 */
public final class VagWriter {
    private static final int HEADER_SIZE = 64;
    private static final int NAME_LENGTH = 16;
    private static final int MAGIC = 0x56414770; // "VAGp"
    private static final int VERSION = 0x20;

    private VagWriter() {}

    public static void write(Path path, byte[] payload, String name, int sampleRate) throws IOException {
        if (path == null || payload == null) {
            return;
        }
        byte[] header = buildHeader(payload.length, name, sampleRate);
        ByteBuffer buffer = ByteBuffer.allocate(header.length + payload.length);
        buffer.put(header);
        buffer.put(payload);
        Files.write(path, buffer.array());
    }

    private static byte[] buildHeader(int dataSize, String name, int sampleRate) {
        ByteBuffer buffer = ByteBuffer.allocate(HEADER_SIZE).order(ByteOrder.BIG_ENDIAN);
        buffer.putInt(MAGIC);
        buffer.putInt(VERSION);
        buffer.putInt(0); // id/reserved
        buffer.putInt(Math.max(0, dataSize));
        buffer.putInt(sampleRate);
        buffer.putInt(0);
        buffer.putInt(0);
        buffer.putInt(0);
        byte[] nameBytes = buildName(name);
        buffer.put(nameBytes);
        buffer.position(HEADER_SIZE);
        return buffer.array();
    }

    private static byte[] buildName(String name) {
        byte[] raw;
        if (name == null || name.isEmpty()) {
            raw = new byte[0];
        } else {
            raw = name.toUpperCase().getBytes(StandardCharsets.US_ASCII);
        }
        byte[] result = new byte[NAME_LENGTH];
        int copy = Math.min(raw.length, NAME_LENGTH);
        System.arraycopy(raw, 0, result, 0, copy);
        return result;
    }
}
