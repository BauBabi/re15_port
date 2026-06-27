package de.re15.extractors.md1;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Files;
import java.nio.file.Path;

/**
 * Minimal TIM reader that exposes texture dimensions and bit depth.
 */
public final class TimFile {

    private final int width;
    private final int height;
    private final int bitsPerPixel;

    private TimFile(int width, int height, int bitsPerPixel) {
        this.width = width;
        this.height = height;
        this.bitsPerPixel = bitsPerPixel;
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getBitsPerPixel() {
        return bitsPerPixel;
    }

    public static TimFile read(Path path) throws IOException {
        byte[] data = Files.readAllBytes(path);
        if (data.length < 32) {
            throw new IOException("TIM file too small: " + path);
        }
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        int magic = buffer.getInt();
        if (magic != 0x00000010) {
            throw new IOException("Invalid TIM magic: 0x" + Integer.toHexString(magic));
        }
        int flags = buffer.getInt();
        boolean hasClut = (flags & 0x08) != 0;
        int bppCode = flags & 0x07;
        int bitsPerPixel;
        switch (bppCode & 0x03) {
            case 0:
                bitsPerPixel = 4;
                break;
            case 1:
                bitsPerPixel = 8;
                break;
            case 2:
                bitsPerPixel = 16;
                break;
            default:
                throw new IOException("Unsupported TIM bpp code: " + bppCode);
        }
        if (hasClut) {
            int clutBlockSize = buffer.getInt();
            if (clutBlockSize < 12 || buffer.remaining() < clutBlockSize - 4) {
                throw new IOException("Invalid CLUT block size in TIM: " + clutBlockSize);
            }
            int blockStart = buffer.position();
            // Skip CLUT header (x, y, width, height) + palette data.
            buffer.position(blockStart + clutBlockSize);
        }
        if (buffer.remaining() < 12) {
            throw new IOException("Incomplete TIM image header");
        }
        int imageBlockSize = buffer.getInt();
        if (imageBlockSize < 12 || buffer.remaining() < imageBlockSize - 4) {
            throw new IOException("Invalid TIM image block size: " + imageBlockSize);
        }
        int imageX = buffer.getShort() & 0xFFFF; // unused but parsed for completeness
        int imageY = buffer.getShort() & 0xFFFF;
        int imageWidthWords = buffer.getShort() & 0xFFFF;
        int imageHeight = buffer.getShort() & 0xFFFF;
        int imageWidth;
        switch (bitsPerPixel) {
            case 4:
                imageWidth = imageWidthWords * 4;
                break;
            case 8:
                imageWidth = imageWidthWords * 2;
                break;
            case 16:
                imageWidth = imageWidthWords;
                break;
            default:
                throw new IOException("Unsupported bits per pixel: " + bitsPerPixel);
        }
        return new TimFile(imageWidth, imageHeight, bitsPerPixel);
    }
}
