package de.re15.extractors;

import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.file.*;
import javax.imageio.ImageIO;

/**
 * Konverter für standalone TIM-Dateien (AAA, CONFIG, C_BACK2, SELECT, etc.)
 */
public class StandaloneTIMToBMPConverter {

    public static void main(String[] args) {
        System.out.println("=== Standalone TIM-zu-BMP Konverter ===");

        try {
            String sourceRoot = "extracted";
            StandaloneTIMToBMPConverter converter = new StandaloneTIMToBMPConverter();
            converter.convertStandaloneTIMFiles(sourceRoot);

            System.out.println("Standalone TIM-Konvertierung abgeschlossen.");
        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void convertStandaloneTIMFiles(String sourceRoot) throws IOException {
        Path dataPath = Paths.get(sourceRoot, "PSX", "DATA");

        if (!Files.exists(dataPath)) {
            System.out.println("DATA-Verzeichnis nicht gefunden: " + dataPath);
            return;
        }

        // Standalone TIM-Dateien direkt im DATA-Verzeichnis (außer MAP-Dateien)
        try (DirectoryStream<Path> stream = Files.newDirectoryStream(dataPath, "*.TIM")) {
            for (Path timFile : stream) {
                String fileName = timFile.getFileName().toString();
                // Überspringe MAP-Dateien - diese werden vom MapTIMToBMPConverter behandelt
                if (!fileName.startsWith("MAP")) {
                    convertStandaloneTIM(timFile);
                }
            }
        }
    }

    private void convertStandaloneTIM(Path timFile) throws IOException {
        System.out.println("Konvertiere standalone TIM: " + timFile.getFileName());

        byte[] timData = Files.readAllBytes(timFile);

        // TIM Header validieren
        if (timData.length < 20) {
            System.out.println("  WARNUNG: TIM-Datei zu klein: " + timFile.getFileName());
            return;
        }

        // TIM ID prüfen (0x10)
        if (timData[0] != 0x10 || timData[1] != 0x00 || timData[2] != 0x00 || timData[3] != 0x00) {
            System.out.println("  WARNUNG: Ungültige TIM ID: " + timFile.getFileName());
            return;
        }

        // Flags lesen
        int flags = readLittleEndianInt(timData, 4);
        boolean hasCLUT = (flags & 0x8) != 0;
        int bpp = getBitsPerPixel(flags & 0x3);

        System.out.println("  BPP: " + bpp + ", Hat CLUT: " + hasCLUT);

        int offset = 8;
        int[] palette = null;

        // CLUT lesen falls vorhanden
        if (hasCLUT) {
            int clutSize = readLittleEndianInt(timData, offset);
            offset += 4;

            int clutX = readLittleEndianShort(timData, offset);
            int clutY = readLittleEndianShort(timData, offset + 2);
            int clutWidth = readLittleEndianShort(timData, offset + 4);
            int clutHeight = readLittleEndianShort(timData, offset + 6);
            offset += 8;

            System.out.println("  CLUT: " + clutWidth + "x" + clutHeight + " @ (" + clutX + "," + clutY + ")");

            palette = new int[clutWidth * clutHeight];
            for (int i = 0; i < palette.length; i++) {
                int color = readLittleEndianShort(timData, offset + i * 2);
                palette[i] = convertPSXColorToRGB(color);
            }
            offset += clutWidth * clutHeight * 2;
        }

        // Bilddaten lesen
        int imageSize = readLittleEndianInt(timData, offset);
        offset += 4;

        int imageX = readLittleEndianShort(timData, offset);
        int imageY = readLittleEndianShort(timData, offset + 2);
        int imageWidth = readLittleEndianShort(timData, offset + 4);
        int imageHeight = readLittleEndianShort(timData, offset + 6);
        offset += 8;

        System.out.println("  Bild: " + imageWidth + "x" + imageHeight + " @ (" + imageX + "," + imageY + ")");

        // Für 4-bit und 8-bit Bilder: Breite in Pixeln berechnen
        int actualWidth = imageWidth;
        if (bpp == 4) {
            actualWidth = imageWidth * 4; // 4 Pixel pro 16-bit Wort
        } else if (bpp == 8) {
            actualWidth = imageWidth * 2; // 2 Pixel pro 16-bit Wort
        }

        System.out.println("  Tatsächliche Breite: " + actualWidth + " Pixel");

        BufferedImage image = new BufferedImage(actualWidth, imageHeight, BufferedImage.TYPE_INT_RGB);

        if (bpp == 4) {
            // 4-bit indexed
            for (int y = 0; y < imageHeight; y++) {
                for (int x = 0; x < imageWidth; x++) {
                    int dataOffset = offset + (y * imageWidth + x) * 2;
                    if (dataOffset + 1 >= timData.length) break;

                    int word = readLittleEndianShort(timData, dataOffset);

                    // 4 Pixel pro 16-bit Wort
                    for (int p = 0; p < 4; p++) {
                        int pixelX = x * 4 + p;
                        if (pixelX >= actualWidth) break;

                        int paletteIndex = (word >> (p * 4)) & 0xF;
                        int color = (palette != null && paletteIndex < palette.length) ?
                                   palette[paletteIndex] : 0xFF000000;
                        image.setRGB(pixelX, y, color);
                    }
                }
            }
        } else if (bpp == 8) {
            // 8-bit indexed
            for (int y = 0; y < imageHeight; y++) {
                for (int x = 0; x < imageWidth; x++) {
                    int dataOffset = offset + (y * imageWidth + x) * 2;
                    if (dataOffset + 1 >= timData.length) break;

                    int word = readLittleEndianShort(timData, dataOffset);

                    // 2 Pixel pro 16-bit Wort
                    for (int p = 0; p < 2; p++) {
                        int pixelX = x * 2 + p;
                        if (pixelX >= actualWidth) break;

                        int paletteIndex = (word >> (p * 8)) & 0xFF;
                        int color = (palette != null && paletteIndex < palette.length) ?
                                   palette[paletteIndex] : 0xFF000000;
                        image.setRGB(pixelX, y, color);
                    }
                }
            }
        } else if (bpp == 16) {
            // 16-bit direct color
            for (int y = 0; y < imageHeight; y++) {
                for (int x = 0; x < imageWidth; x++) {
                    int dataOffset = offset + (y * imageWidth + x) * 2;
                    if (dataOffset + 1 >= timData.length) break;

                    int color = readLittleEndianShort(timData, dataOffset);
                    int rgb = convertPSXColorToRGB(color);
                    image.setRGB(x, y, rgb);
                }
            }
        }

        // BMP speichern
        String bmpFileName = timFile.getFileName().toString().replace(".TIM", ".bmp");
        Path bmpFile = timFile.getParent().resolve(bmpFileName);
        ImageIO.write(image, "BMP", bmpFile.toFile());

        System.out.println("  -> " + bmpFileName + " (" + actualWidth + "x" + imageHeight + ")");
    }

    private int readLittleEndianInt(byte[] data, int offset) {
        return (data[offset] & 0xFF) |
               ((data[offset + 1] & 0xFF) << 8) |
               ((data[offset + 2] & 0xFF) << 16) |
               ((data[offset + 3] & 0xFF) << 24);
    }

    private int readLittleEndianShort(byte[] data, int offset) {
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    private int getBitsPerPixel(int mode) {
        switch (mode) {
            case 0: return 4;
            case 1: return 8;
            case 2: return 16;
            case 3: return 24;
            default: return 16;
        }
    }

    private int convertPSXColorToRGB(int psxColor) {
        if (psxColor == 0) return 0xFF000000; // Transparent -> Black

        int r = (psxColor & 0x1F) << 3;
        int g = ((psxColor >> 5) & 0x1F) << 3;
        int b = ((psxColor >> 10) & 0x1F) << 3;

        return 0xFF000000 | (r << 16) | (g << 8) | b;
    }
}
