package de.re15.extractors.bss;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

final class BssChunk {

    private static final int HEADER_SIZE = 8;
    final byte[] quantY;
    final byte[] quantUv;
    final short[] runLength;

    private BssChunk(byte[] quantY, byte[] quantUv, short[] runLength) {
        this.quantY = quantY;
        this.quantUv = quantUv;
        this.runLength = runLength;
    }

    static BssChunk parse(byte[] chunk) {
        ByteBuffer buffer = ByteBuffer.wrap(chunk).order(ByteOrder.LITTLE_ENDIAN);
        if (buffer.remaining() < HEADER_SIZE) {
            throw new IllegalArgumentException("Truncated BS chunk");
        }
        int runLengthWords = Short.toUnsignedInt(buffer.getShort());
        int id = Short.toUnsignedInt(buffer.getShort());
        int quant = Short.toUnsignedInt(buffer.getShort());
        int version = Short.toUnsignedInt(buffer.getShort());
        if (id != VlcDecoder.VLC_ID) {
            throw new IllegalArgumentException("Chunk does not contain VLC data");
        }

        ByteBuffer runData = buffer.slice().order(ByteOrder.LITTLE_ENDIAN);
        short[] runLength = VlcDecoder.decode(runData, runLengthWords, quant, version);
        return new BssChunk(null, null, runLength);
    }
}
