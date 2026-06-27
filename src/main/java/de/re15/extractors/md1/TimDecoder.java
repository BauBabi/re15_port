package de.re15.extractors.md1;

import java.awt.image.BufferedImage;
import java.io.IOException;

/**
 * Minimal TIM image decoder used for converting MD1 companion textures.
 */
public final class TimDecoder {

    private TimDecoder() {
    }

    public static DecodedTim decode(byte[] data) throws IOException {
        if (data.length < 20) {
            throw new IOException("TIM file too small");
        }
        int magic = readInt(data, 0);
        if (magic != 0x10) {
            throw new IOException(String.format("Invalid TIM magic 0x%08X", magic));
        }
        int flags = readInt(data, 4);
        boolean hasClut = (flags & 0x08) != 0;
        int mode = flags & 0x07;
        int bitsPerPixel;
        switch (mode) {
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
                throw new IOException("Unsupported TIM mode: " + mode);
        }
        int offset = 8;
        int[] palette = null;
        if (hasClut) {
            if (offset + 4 > data.length) {
                throw new IOException("TIM CLUT block truncated");
            }
            int clutSize = readInt(data, offset);
            offset += 4;
            if (clutSize < 8) {
                throw new IOException("Invalid TIM CLUT block size: " + clutSize);
            }
            if (offset + clutSize > data.length) {
                clutSize = data.length - offset;
            }
            int clutX = readShort(data, offset);
            int clutY = readShort(data, offset + 2);
            int clutWidth = readShort(data, offset + 4);
            int clutHeight = readShort(data, offset + 6);
            offset += 8;
            int paletteEntries = clutWidth * clutHeight;
            int paletteBytes = Math.min(Math.max(clutSize - 8, 0), paletteEntries * 2);
            palette = new int[paletteEntries];
            for (int i = 0; i < paletteEntries; i++) {
                int entryOffset = offset + i * 2;
                if (entryOffset + 1 >= offset + paletteBytes) {
                    break;
                }
                palette[i] = convertColor(readShort(data, entryOffset));
            }
            offset += paletteBytes;
        }
        if (offset + 4 > data.length) {
            throw new IOException("TIM image block truncated");
        }
        int imageBlockSize = readInt(data, offset);
        offset += 4;
        if (imageBlockSize < 8) {
            throw new IOException("Invalid TIM image block size: " + imageBlockSize);
        }
        int remaining = data.length - offset;
        if (remaining < 8) {
            throw new IOException("TIM image header truncated");
        }
        int imageX = readShort(data, offset);
        int imageY = readShort(data, offset + 2);
        int imageWidthWords = readShort(data, offset + 4);
        int imageHeight = readShort(data, offset + 6);
        offset += 8;
        remaining = data.length - offset;
        int actualWidth;
        switch (bitsPerPixel) {
            case 4:
                actualWidth = imageWidthWords * 4;
                break;
            case 8:
                actualWidth = imageWidthWords * 2;
                break;
            case 16:
                actualWidth = imageWidthWords;
                break;
            default:
                throw new IOException("Unsupported bits per pixel: " + bitsPerPixel);
        }
        BufferedImage image = new BufferedImage(actualWidth, imageHeight, BufferedImage.TYPE_INT_RGB);
        int pixelDataStart = offset;
        int pixelBytes = Math.max(0, Math.min(imageBlockSize - 8, remaining));
        int pixelDataEnd = pixelDataStart + pixelBytes;
        if (bitsPerPixel == 4) {
            if (palette == null) {
                throw new IOException("4-bit TIM without CLUT");
            }
            for (int y = 0; y < imageHeight; y++) {
                for (int x = 0; x < imageWidthWords; x++) {
                    int wordOffset = pixelDataStart + (y * imageWidthWords + x) * 2;
                    int word = readShortBounded(data, wordOffset, pixelDataEnd);
                    for (int p = 0; p < 4; p++) {
                        int pixelX = x * 4 + p;
                        if (pixelX >= actualWidth) {
                            break;
                        }
                        int paletteIndex = (word >> (p * 4)) & 0x0F;
                        int color = paletteIndex < palette.length ? palette[paletteIndex] : 0;
                        image.setRGB(pixelX, y, color);
                    }
                }
            }
        } else if (bitsPerPixel == 8) {
            if (palette == null) {
                throw new IOException("8-bit TIM without CLUT");
            }
            for (int y = 0; y < imageHeight; y++) {
                for (int x = 0; x < imageWidthWords; x++) {
                    int wordOffset = pixelDataStart + (y * imageWidthWords + x) * 2;
                    int word = readShortBounded(data, wordOffset, pixelDataEnd);
                    for (int p = 0; p < 2; p++) {
                        int pixelX = x * 2 + p;
                        if (pixelX >= actualWidth) {
                            break;
                        }
                        int paletteIndex = (word >> (p * 8)) & 0xFF;
                        int color = paletteIndex < palette.length ? palette[paletteIndex] : 0;
                        image.setRGB(pixelX, y, color);
                    }
                }
            }
        } else { // 16-bit direct color
            for (int y = 0; y < imageHeight; y++) {
                for (int x = 0; x < actualWidth; x++) {
                    int wordOffset = pixelDataStart + (y * actualWidth + x) * 2;
                    int word = readShortBounded(data, wordOffset, pixelDataEnd);
                    image.setRGB(x, y, convertColor(word));
                }
            }
        }
        return new DecodedTim(image, actualWidth, imageHeight, bitsPerPixel, hasClut);
    }

    private static int readInt(byte[] data, int offset) {
        return (data[offset] & 0xFF)
                | ((data[offset + 1] & 0xFF) << 8)
                | ((data[offset + 2] & 0xFF) << 16)
                | ((data[offset + 3] & 0xFF) << 24);
    }

    private static int readShort(byte[] data, int offset) {
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    private static int readShortBounded(byte[] data, int offset, int endExclusive) {
        if (offset < 0 || offset + 1 >= endExclusive) {
            return 0;
        }
        return readShort(data, offset);
    }

    private static int convertColor(int psxColor) {
        int r = psxColor & 0x1F;
        int g = (psxColor >> 5) & 0x1F;
        int b = (psxColor >> 10) & 0x1F;
        int stp = (psxColor >> 15) & 0x1;
        int red = (r * 255) / 31;
        int green = (g * 255) / 31;
        int blue = (b * 255) / 31;
        int alpha = (stp == 0) ? 0xFF : 0x00;
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }

    public static final class DecodedTim {
        private final BufferedImage image;
        private final int width;
        private final int height;
        private final int bitsPerPixel;
        private final boolean hasClut;

        private DecodedTim(BufferedImage image, int width, int height, int bitsPerPixel, boolean hasClut) {
            this.image = image;
            this.width = width;
            this.height = height;
            this.bitsPerPixel = bitsPerPixel;
            this.hasClut = hasClut;
        }

        public BufferedImage image() {
            return image;
        }

        public int width() {
            return width;
        }

        public int height() {
            return height;
        }

        public int bitsPerPixel() {
            return bitsPerPixel;
        }

        public boolean hasClut() {
            return hasClut;
        }
    }
}
