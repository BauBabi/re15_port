package de.re15.extractors.bss;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

final class TimWriter {

    private TimWriter() {
    }

    static byte[] to16BitTim(byte[] bgr, int width, int height) {
        int pixelCount = width * height;
        int imageDataBytes = pixelCount * 2;
        ByteBuffer buffer = ByteBuffer.allocate(8 + 4 + 8 + imageDataBytes)
            .order(ByteOrder.LITTLE_ENDIAN);

        buffer.putInt(0x10); // TIM magic
        buffer.putInt(0x00000002); // 16-bit, keine CLUT

        buffer.putInt(12 + imageDataBytes); // Bildblock (inkl. Header)
        buffer.putShort((short) 0); // VRAM X
        buffer.putShort((short) 0); // VRAM Y
        buffer.putShort((short) width);
        buffer.putShort((short) height);

        /* BO-round 2026-05-29: REVERTED — kept the original byte order (b=byte0)
         * to match the engine's actually-displayed BG colors (user-confirmed the
         * [r,g,b] read produced wrong colors). See BssExtractor.writeBmp. */
        for (int i = 0; i < bgr.length; i += 3) {
            int b = bgr[i] & 0xFF;
            int g = bgr[i + 1] & 0xFF;
            int r = bgr[i + 2] & 0xFF;
            int color = (r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10);
            buffer.putShort((short) color);
        }

        return buffer.array();
    }
}
