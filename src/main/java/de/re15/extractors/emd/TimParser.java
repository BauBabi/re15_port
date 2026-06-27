package de.re15.extractors.emd;

import de.re15.extractors.util.LittleEndianDataView;

import java.util.Optional;

final class TimParser {
    private TimParser() {}

    static Optional<TextureInfo> parseTim(byte[] data, int offset, String stem) {
        if (offset <= 0 || offset >= data.length) {
            return Optional.empty();
        }

        LittleEndianDataView view = new LittleEndianDataView(data);
        int magic = view.readInt(offset);
        if (magic != 0x10) {
            return Optional.empty();
        }

        int timType = view.readInt(offset + 4);

        /* BO-round 2026-05-29: a CLUT block exists ONLY when bit 3 of the type
         * is set — 4bpp(0x08)/8bpp(0x09). 16bpp(0x02)/24bpp(0x03) have NO CLUT;
         * the image block follows the 8-byte header directly. The old code read
         * the CLUT header unconditionally → for 16bpp it read image-block bytes
         * as clutColors/clutCount and advanced `pos` into garbage. */
        int clutColors = 0;
        int clutCount = 0;
        byte[] clutBytes = new byte[0];
        int pos = offset + 8;
        if ((timType & 0x08) != 0) {
            clutColors = view.readUnsignedShort(offset + 16);   /* CLUT block: bnum(4) dx dy w(=#colors) h(=#cluts) */
            clutCount = view.readUnsignedShort(offset + 18);
            int clutSize = clutColors * clutCount;
            clutBytes = clutSize > 0 ? view.slice(offset + 20, clutSize * 2) : new byte[0];
            pos = offset + 20 + clutSize * 2;
        }

        /* Image block header */
        int imageBlockSize = view.readInt(pos);
        int imageOriginX = view.readUnsignedShort(pos + 4);
        int imageOriginY = view.readUnsignedShort(pos + 6);
        int width = view.readUnsignedShort(pos + 8);
        int height = view.readUnsignedShort(pos + 10);
        pos += 12;

        int widthPx;
        if (timType == 0x08) {
            widthPx = width * 4;
        } else if (timType == 0x09) {
            widthPx = width * 2;
        } else if (timType == 0x02) {
            widthPx = width;
        } else {
            throw new IllegalArgumentException("Unsupported TIM bpp type: " + timType);
        }

        int effectiveClutCount = clutCount == 0 ? 1 : clutCount;
        int colorsPerClut = clutColors == 0 ? 1 : clutColors;
        int clutWidth = effectiveClutCount > 0 ? Math.max(1, widthPx / effectiveClutCount) : widthPx;

        short[] clutValues = new short[colorsPerClut * effectiveClutCount];
        for (int i = 0; i < clutValues.length && i * 2 < clutBytes.length; i++) {
            int lo = Byte.toUnsignedInt(clutBytes[i * 2]);
            int hi = Byte.toUnsignedInt(clutBytes[i * 2 + 1]);
            clutValues[i] = (short) (lo | (hi << 8));
        }

        byte[] pixels = new byte[widthPx * height * 4];
        int pixelIndex = 0;
        int pixelPointer = pos;

        if (timType == 0x08) { // 4bpp
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < widthPx; x += 2) {
                    if (pixelPointer >= data.length) {
                        throw new IllegalStateException("TIM pixel data exceeds buffer length");
                    }
                    int packed = Byte.toUnsignedInt(data[pixelPointer++]);
                    int low = packed & 0x0F;
                    int high = (packed >> 4) & 0x0F;
                    pixelIndex = writePalettedPixel(pixels, pixelIndex, clutValues, colorsPerClut, effectiveClutCount, clutWidth, x, low);
                    pixelIndex = writePalettedPixel(pixels, pixelIndex, clutValues, colorsPerClut, effectiveClutCount, clutWidth, x + 1, high);
                }
            }
        } else if (timType == 0x09) { // 8bpp
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < widthPx; x++) {
                    if (pixelPointer >= data.length) {
                        throw new IllegalStateException("TIM pixel data exceeds buffer length");
                    }
                    int paletteIndex = Byte.toUnsignedInt(data[pixelPointer++]);
                    pixelIndex = writePalettedPixel(pixels, pixelIndex, clutValues, colorsPerClut, effectiveClutCount, clutWidth, x, paletteIndex);
                }
            }
        } else { // 16bpp direct color
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < widthPx; x++) {
                    if (pixelPointer + 1 >= data.length) {
                        throw new IllegalStateException("TIM pixel data exceeds buffer length");
                    }
                    int color = Byte.toUnsignedInt(data[pixelPointer]) | (Byte.toUnsignedInt(data[pixelPointer + 1]) << 8);
                    pixelPointer += 2;
                    pixelIndex = writeDirectColorPixel(pixels, pixelIndex, color);
                }
            }
        }

        String imageName = stem + ".png";
        TextureInfo info = new TextureInfo(widthPx, height, effectiveClutCount, clutWidth, pixels, imageName);
        return Optional.of(info);
    }

    private static int writePalettedPixel(byte[] pixels,
                                          int pixelIndex,
                                          short[] clutValues,
                                          int colorsPerClut,
                                          int clutCount,
                                          int clutWidth,
                                          int x,
                                          int paletteIndex) {
        if (clutValues.length == 0 || colorsPerClut <= 0 || clutCount <= 0) {
            return writeDirectColorPixel(pixels, pixelIndex, paletteIndex);
        }
        int clutIndex = clutWidth > 0 ? Math.min(clutCount - 1, x / clutWidth) : 0;
        int normalizedPalette = paletteIndex % colorsPerClut;
        if (normalizedPalette < 0) {
            normalizedPalette += colorsPerClut;
        }
        int colorIndex = clutIndex * colorsPerClut + normalizedPalette;
        if (colorIndex < 0 || colorIndex >= clutValues.length) {
            colorIndex = Math.max(0, Math.min(clutValues.length - 1, colorIndex));
        }
        int color = Short.toUnsignedInt(clutValues[colorIndex]);
        return writeDirectColorPixel(pixels, pixelIndex, color);
    }

    private static int writeDirectColorPixel(byte[] pixels, int pixelIndex, int color) {
        int r = (color & 0x1F) * 255 / 31;
        int g = ((color >> 5) & 0x1F) * 255 / 31;
        int b = ((color >> 10) & 0x1F) * 255 / 31;
        int a = ((color >> 15) & 0x1) != 0 ? 0 : ((r == 0 && g == 0 && b == 0) ? 0 : 255);
        pixels[pixelIndex++] = (byte) r;
        pixels[pixelIndex++] = (byte) g;
        pixels[pixelIndex++] = (byte) b;
        pixels[pixelIndex++] = (byte) a;
        return pixelIndex;
    }
}
