package de.re15.extractors;

import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.file.*;
import javax.imageio.ImageIO;

/**
 * Konverter für ITEMALL/MIXITEM TIM-Dateien (40x30, 8-bit color)
 */
public class ItemTIMToBMPConverter {

    public static void main(String[] args) {
        System.out.println("=== ITEMALL/MIXITEM TIM-zu-BMP Konverter ===");

        try {
            String sourceRoot = "extracted";
            ItemTIMToBMPConverter converter = new ItemTIMToBMPConverter();
            converter.convertItemTIMFiles(sourceRoot);

            System.out.println("ITEMALL/MIXITEM TIM-Konvertierung abgeschlossen.");
        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void convertItemTIMFiles(String sourceRoot) throws IOException {
        Path dataPath = Paths.get(sourceRoot, "PSX", "DATA");

        if (!Files.exists(dataPath)) {
            System.out.println("DATA-Verzeichnis nicht gefunden: " + dataPath);
            return;
        }

        // ITEMALL TIM-Dateien
        Path itemallPath = dataPath.resolve("ITEMALL");
        if (Files.exists(itemallPath)) {
            convertItemDirectory(itemallPath, "ITEMALL");
        }

        // MIXITEM TIM-Dateien
        Path mixitemPath = dataPath.resolve("MIXITEM");
        if (Files.exists(mixitemPath)) {
            convertItemDirectory(mixitemPath, "MIXITEM");
        }
    }

    private void convertItemDirectory(Path itemPath, String type) throws IOException {
        System.out.println("Konvertiere " + type + " TIM-Dateien in: " + itemPath);

        try (DirectoryStream<Path> stream = Files.newDirectoryStream(itemPath, "*.TIM")) {
            for (Path timFile : stream) {
                convertItemTIM(timFile, type);
            }
        }
    }

    private void convertItemTIM(Path timFile, String type) throws IOException {
        System.out.println("Konvertiere " + type + " TIM: " + timFile.getFileName());

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

        // ITEMALL/MIXITEM spezifische Behandlung: 40x30 Pixel, 8-bit
        // Diese sollten bereits die korrekte Größe haben
        int actualWidth = 40;  // Fest für ITEMALL/MIXITEM
        int actualHeight = 30; // Fest für ITEMALL/MIXITEM

        // Falls die TIM-Datei andere Dimensionen hat, verwende diese aber mit Vorsicht
        if (bpp == 8) {
            // Für 8-bit: 2 Pixel pro 16-bit Wort in der TIM-Breite
            if (imageWidth * 2 == 40) {
                actualWidth = 40;
            } else {
                actualWidth = imageWidth * 2;
            }
            actualHeight = imageHeight;
        } else if (bpp == 4) {
            // Für 4-bit: 4 Pixel pro 16-bit Wort
            if (imageWidth * 4 == 40) {
                actualWidth = 40;
            } else {
                actualWidth = imageWidth * 4;
            }
            actualHeight = imageHeight;
        }

        System.out.println("  Item-Größe: " + actualWidth + "x" + actualHeight + " Pixel");

        BufferedImage image = new BufferedImage(actualWidth, actualHeight, BufferedImage.TYPE_INT_RGB);

        if (bpp == 8) {
            // 8-bit indexed für ITEMALL/MIXITEM
            for (int y = 0; y < actualHeight; y++) {
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
        } else if (bpp == 4) {
            // 4-bit indexed für ITEMALL/MIXITEM
            for (int y = 0; y < actualHeight; y++) {
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
        } else if (bpp == 16) {
            // 16-bit direct color für ITEMALL/MIXITEM
            for (int y = 0; y < actualHeight; y++) {
                for (int x = 0; x < actualWidth; x++) {
                    int dataOffset = offset + (y * actualWidth + x) * 2;
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

        System.out.println("  -> " + bmpFileName + " (" + actualWidth + "x" + actualHeight + ")");
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
