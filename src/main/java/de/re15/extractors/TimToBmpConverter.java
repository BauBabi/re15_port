package de.re15.extractors;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Path;

/**
 * Utility for decoding TIM images into BMP outputs.
 *
 * <p>This is a lightweight decoder that supports the TIM variants used by the
 * project (4-bit, 8-bit and 16-bit). It mirrors the logic from the standalone
 * converters but exposes a reusable API for in-code conversions.</p>
 */
public final class TimToBmpConverter {

    private TimToBmpConverter() {
    }

    public static boolean writeBmp(byte[] timData, Path target) throws IOException {
        BufferedImage image = decode(timData);
        if (image == null) {
            return false;
        }
        ImageIO.write(image, "BMP", target.toFile());
        return true;
    }

    private static BufferedImage decode(byte[] timData) {
        if (timData == null || timData.length < 20) {
            return null;
        }
        ByteBuffer buffer = ByteBuffer.wrap(timData).order(ByteOrder.LITTLE_ENDIAN);
        int magic = buffer.getInt();
        if (magic != 0x10) {
            return null;
        }
        int flags = buffer.getInt();
        boolean hasClut = (flags & 0x8) != 0;
        int mode = flags & 0x3;
        int bpp = switch (mode) {
            case 0 -> 4;
            case 1 -> 8;
            case 2 -> 16;
            case 3 -> 24;
            default -> 16;
        };

        int[] palette = null;
        if (hasClut) {
            if (buffer.remaining() < 12) {
                return null;
            }
            int clutSize = buffer.getInt();
            if (buffer.remaining() < clutSize) {
                return null;
            }
            int clutX = Short.toUnsignedInt(buffer.getShort());
            int clutY = Short.toUnsignedInt(buffer.getShort());
            int clutWidth = Short.toUnsignedInt(buffer.getShort());
            int clutHeight = Short.toUnsignedInt(buffer.getShort());
            int colorCount = clutWidth * clutHeight;
            palette = new int[colorCount];
            for (int i = 0; i < colorCount; i++) {
                int color = Short.toUnsignedInt(buffer.getShort());
                palette[i] = convertPSXColorToRGB(color);
            }
        }

        if (buffer.remaining() < 12) {
            return null;
        }
        int imageSize = buffer.getInt();
        int imageX = Short.toUnsignedInt(buffer.getShort());
        int imageY = Short.toUnsignedInt(buffer.getShort());
        int imageWidthWords = Short.toUnsignedInt(buffer.getShort());
        int imageHeight = Short.toUnsignedInt(buffer.getShort());
        if (buffer.remaining() < imageSize - 12) {
            return null;
        }
        int pixelWidth = imageWidthWords;
        if (bpp == 4) {
            pixelWidth = imageWidthWords * 4;
        } else if (bpp == 8) {
            pixelWidth = imageWidthWords * 2;
        }

        BufferedImage image = new BufferedImage(pixelWidth, imageHeight, BufferedImage.TYPE_INT_RGB);
        int imageDataStart = buffer.position();
        byte[] imageData = new byte[Math.min(buffer.remaining(), imageSize - 12)];
        buffer.get(imageData);
        ByteBuffer img = ByteBuffer.wrap(imageData).order(ByteOrder.LITTLE_ENDIAN);

        switch (bpp) {
            case 4 -> decode4Bit(img, palette, image, imageWidthWords);
            case 8 -> decode8Bit(img, palette, image, imageWidthWords);
            case 16 -> decode16Bit(img, image, imageWidthWords);
            case 24 -> decode24Bit(img, image, imageWidthWords);
            default -> {
                return null;
            }
        }
        return image;
    }

    private static void decode4Bit(ByteBuffer img, int[] palette, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        int actualWidth = target.getWidth();
        for (int y = 0; y < height; y++) {
            for (int xWord = 0; xWord < wordWidth; xWord++) {
                if (img.remaining() < 2) {
                    return;
                }
                int word = Short.toUnsignedInt(img.getShort());
                for (int p = 0; p < 4; p++) {
                    int pixelX = xWord * 4 + p;
                    if (pixelX >= actualWidth) {
                        break;
                    }
                    int index = (word >> (p * 4)) & 0xF;
                    int color = palette != null && index < palette.length ? palette[index] : 0xFF000000;
                    target.setRGB(pixelX, y, color);
                }
            }
        }
    }

    private static void decode8Bit(ByteBuffer img, int[] palette, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        int actualWidth = target.getWidth();
        for (int y = 0; y < height; y++) {
            for (int xWord = 0; xWord < wordWidth; xWord++) {
                if (img.remaining() < 2) {
                    return;
                }
                int word = Short.toUnsignedInt(img.getShort());
                for (int p = 0; p < 2; p++) {
                    int pixelX = xWord * 2 + p;
                    if (pixelX >= actualWidth) {
                        break;
                    }
                    int index = (word >> (p * 8)) & 0xFF;
                    int color = palette != null && index < palette.length ? palette[index] : 0xFF000000;
                    target.setRGB(pixelX, y, color);
                }
            }
        }
    }

    private static void decode16Bit(ByteBuffer img, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < wordWidth; x++) {
                if (img.remaining() < 2) {
                    return;
                }
                int color = Short.toUnsignedInt(img.getShort());
                target.setRGB(x, y, convertPSXColorToRGB(color));
            }
        }
    }

    private static void decode24Bit(ByteBuffer img, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < wordWidth; x++) {
                if (img.remaining() < 3) {
                    return;
                }
                int b = Byte.toUnsignedInt(img.get());
                int g = Byte.toUnsignedInt(img.get());
                int r = Byte.toUnsignedInt(img.get());
                int rgb = (r << 16) | (g << 8) | b;
                target.setRGB(x, y, rgb);
            }
        }
    }

    private static int convertPSXColorToRGB(int color) {
        int r = (color & 0x1F) << 3;
        int g = ((color >> 5) & 0x1F) << 3;
        int b = ((color >> 10) & 0x1F) << 3;
        return (r << 16) | (g << 8) | b;
    }
}
