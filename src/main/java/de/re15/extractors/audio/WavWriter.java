package de.re15.extractors.audio;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

/**
 * Writes mono 16-bit PCM samples into a simple WAV container.
 */
final class WavWriter {
    private WavWriter() {}

    static void writeMono16(Path path, short[] samples, int sampleRate) throws IOException {
        if (path == null || samples == null) {
            return;
        }
        int dataSize = samples.length * 2;
        int chunkSize = 36 + dataSize;
        ByteBuffer buffer = ByteBuffer.allocate(44 + dataSize).order(ByteOrder.LITTLE_ENDIAN);
        buffer.put("RIFF".getBytes(StandardCharsets.US_ASCII));
        buffer.putInt(chunkSize);
        buffer.put("WAVE".getBytes(StandardCharsets.US_ASCII));
        buffer.put("fmt ".getBytes(StandardCharsets.US_ASCII));
        buffer.putInt(16); // PCM header size
        buffer.putShort((short) 1); // PCM format
        buffer.putShort((short) 1); // mono
        buffer.putInt(sampleRate);
        int byteRate = sampleRate * 2;
        buffer.putInt(byteRate);
        buffer.putShort((short) 2); // block align
        buffer.putShort((short) 16); // bits per sample
        buffer.put("data".getBytes(StandardCharsets.US_ASCII));
        buffer.putInt(dataSize);
        for (short sample : samples) {
            buffer.putShort(sample);
        }
        Files.write(path, buffer.array());
    }
}
