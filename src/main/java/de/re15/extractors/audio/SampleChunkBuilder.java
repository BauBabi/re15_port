package de.re15.extractors.audio;

import de.re15.extractors.audio.VabFile.Sample;

import java.io.ByteArrayOutputStream;
import java.util.Arrays;

/**
 * Builds raw ADPCM chunks that are ready for decoding or wrapping in a VAG file.
 */
public final class SampleChunkBuilder {
    private static final byte[] ZERO_BLOCK = new byte[16];
    private static final int FRAME_SIZE = 16;
    private static final int SAMPLES_PER_FRAME = 28;
    /** Target sample rate for loop extension calculation (matches PSOUND algorithm). */
    private static final int SAMPLE_RATE = 22050;
    /** Maximum output samples for large loop sections (~7.5 seconds). */
    private static final int MAX_OUTPUT_SAMPLES = 165375;
    /** Threshold for considering a loop section as "large" (~4 seconds). */
    private static final int LARGE_LOOP_THRESHOLD = 88200;

    private SampleChunkBuilder() {}

    public static byte[] build(byte[] source,
                               SampleDataWindow window,
                               SampleLayout layout,
                               Sample sample) {
        if (source == null || window == null) {
            return new byte[0];
        }
        int decodeBytes = window.alignedDataLength(FRAME_SIZE);
        if (decodeBytes <= 0) {
            return new byte[0];
        }
        int offset = window.dataOffset();
        if (offset < 0 || offset + decodeBytes > source.length) {
            decodeBytes = Math.max(0, source.length - Math.max(0, offset));
        }
        if (decodeBytes <= 0) {
            return new byte[0];
        }
        byte[] prefix = layout.prefixBytes() > 0
            ? Arrays.copyOfRange(source, window.chunkOffset(), Math.min(source.length, window.chunkOffset() + layout.prefixBytes()))
            : new byte[0];
        int zeroPad = layout.requiresZeroPadding() ? ZERO_BLOCK.length : 0;

        LoopInfo loop = detectLoop(source, offset, decodeBytes);
        byte[] data = buildDataWithLoop(source, offset, decodeBytes, loop);

        byte[] chunk = new byte[zeroPad + data.length + prefix.length];
        int cursor = 0;
        if (zeroPad > 0) {
            System.arraycopy(ZERO_BLOCK, 0, chunk, 0, ZERO_BLOCK.length);
            cursor += ZERO_BLOCK.length;
        }
        System.arraycopy(data, 0, chunk, cursor, data.length);
        cursor += data.length;
        if (prefix.length > 0) {
            System.arraycopy(prefix, 0, chunk, cursor, prefix.length);
        }
        return chunk;
    }

    private static byte[] buildDataWithLoop(byte[] source, int dataOffset, int dataLength, LoopInfo loop) {
        // No loop point - return data as-is
        if (loop.startOffset <= 0) {
            return Arrays.copyOfRange(source, dataOffset, dataOffset + loop.endOffset);
        }

        // For end_flag=1 (END without REPEAT), don't add extra loops
        if (loop.hasEnd && !loop.hasRepeat) {
            return Arrays.copyOfRange(source, dataOffset, dataOffset + loop.endOffset);
        }

        ByteArrayOutputStream output = new ByteArrayOutputStream();
        output.write(source, dataOffset, loop.endOffset);

        int loopStart = loop.startOffset;
        int loopEnd = loop.endOffset;
        int loopSize = loopEnd - loopStart;

        if (loopSize > 0) {
            int loopSamples = (loopSize / FRAME_SIZE) * SAMPLES_PER_FRAME;
            int baseSamples = (loopEnd / FRAME_SIZE) * SAMPLES_PER_FRAME;

            // For large loop sections, cap output at MAX_OUTPUT_SAMPLES
            if (loopSamples >= LARGE_LOOP_THRESHOLD) {
                int targetSamples = MAX_OUTPUT_SAMPLES - baseSamples;
                if (targetSamples > 0 && targetSamples < loopSamples) {
                    // Partial loop - calculate how many bytes to add
                    int targetFrames = targetSamples / SAMPLES_PER_FRAME;
                    int targetBytes = targetFrames * FRAME_SIZE;
                    if (targetBytes > 0 && targetBytes <= loopSize) {
                        output.write(source, dataOffset + loopStart, targetBytes);
                    }
                }
            } else {
                int repeatCount = calculateRepeatCount(loopSize);
                for (int i = 0; i < repeatCount; i++) {
                    output.write(source, dataOffset + loopStart, loopSize);
                }
            }
        }

        return output.toByteArray();
    }

    /**
     * Calculate loop repeat count using PSOUND algorithm.
     * Formula: ceil(SAMPLE_RATE / loopSamples) - adds ~1 second of loop audio.
     */
    private static int calculateRepeatCount(int loopBytes) {
        int loopSamples = (loopBytes / FRAME_SIZE) * SAMPLES_PER_FRAME;
        if (loopSamples <= 0) {
            return 0;
        }
        // PSOUND algorithm: repeat enough times to add ~1 second of audio
        return (int) Math.ceil((double) SAMPLE_RATE / loopSamples);
    }

    private static LoopInfo detectLoop(byte[] source, int dataOffset, int maxLength) {
        int frames = maxLength / FRAME_SIZE;
        if (frames == 0) {
            return new LoopInfo(maxLength, 0, false, false);
        }

        int loopStartFrame = -1;
        int loopEndFrame = frames;
        boolean hasEnd = false;
        boolean hasRepeat = false;

        for (int frame = 0; frame < frames; frame++) {
            int base = dataOffset + frame * FRAME_SIZE;
            if (base + 1 >= source.length) {
                loopEndFrame = frame + 1;
                break;
            }
            int flag = Byte.toUnsignedInt(source[base + 1]);

            if ((flag & 0x4) != 0) {
                loopStartFrame = frame;
            }

            // Check for end flag (0x1)
            if (flag == 7 || (flag & 0x1) != 0) {
                hasEnd = true;
                hasRepeat = (flag & 0x2) != 0;
                loopEndFrame = frame + 1;
                break;
            }
        }

        int startOffset = loopStartFrame >= 0 ? loopStartFrame * FRAME_SIZE : 0;
        int endOffset = Math.min(loopEndFrame * FRAME_SIZE, maxLength);

        return new LoopInfo(endOffset, startOffset, hasEnd, hasRepeat);
    }

    private record LoopInfo(int endOffset, int startOffset, boolean hasEnd, boolean hasRepeat) {}
}
