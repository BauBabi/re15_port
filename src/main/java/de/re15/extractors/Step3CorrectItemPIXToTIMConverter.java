package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Schritt 3 - Korrekte Version: PIX-zu-TIM Konverter mit echter Item-Palette
 * Extrahiert 40x30 8-bit TIM-Dateien mit korrekter Farbpalette und VRAM-Position
 */
public class Step3CorrectItemPIXToTIMConverter {

    // Korrekte Item-Palette (256 Farben á 2 Bytes = 512 Bytes)
    // Diese Palette wurde aus der funktionierenden Referenz extrahiert
    private static final byte[] CORRECT_ITEM_PALETTE = {
        (byte)0x00, (byte)0x80, (byte)0x9C, (byte)0x73, (byte)0x7B, (byte)0x6F, (byte)0x18, (byte)0x63, (byte)0xF7, (byte)0x5E, (byte)0xD6, (byte)0x5A, (byte)0xB5, (byte)0x56, (byte)0x73, (byte)0x4E,
        (byte)0x52, (byte)0x4A, (byte)0x10, (byte)0x42, (byte)0xEF, (byte)0x3D, (byte)0xCE, (byte)0x39, (byte)0xAD, (byte)0x35, (byte)0x8C, (byte)0x31, (byte)0x29, (byte)0x25, (byte)0xE7, (byte)0x1C,
        (byte)0xC6, (byte)0x18, (byte)0xA5, (byte)0x14, (byte)0x84, (byte)0x10, (byte)0x63, (byte)0x0C, (byte)0x74, (byte)0x4E, (byte)0x32, (byte)0x46, (byte)0xCF, (byte)0x39, (byte)0x4C, (byte)0x29,
        (byte)0xD1, (byte)0x39, (byte)0xB7, (byte)0x35, (byte)0x74, (byte)0x2D, (byte)0x55, (byte)0x29, (byte)0x67, (byte)0x0C, (byte)0xCF, (byte)0x18, (byte)0x8E, (byte)0x10, (byte)0x91, (byte)0x10,
        (byte)0x11, (byte)0x00, (byte)0x0E, (byte)0x00, (byte)0x0A, (byte)0x00, (byte)0x08, (byte)0x00, (byte)0x02, (byte)0x00, (byte)0x4E, (byte)0x04, (byte)0x96, (byte)0x2D, (byte)0xD5, (byte)0x10,
        (byte)0x4B, (byte)0x04, (byte)0x38, (byte)0x42, (byte)0x70, (byte)0x29, (byte)0xD8, (byte)0x31, (byte)0xB7, (byte)0x2D, (byte)0x37, (byte)0x19, (byte)0x93, (byte)0x04, (byte)0x90, (byte)0x04,
        (byte)0x16, (byte)0x11, (byte)0x79, (byte)0x46, (byte)0xD3, (byte)0x08, (byte)0xB5, (byte)0x00, (byte)0xAD, (byte)0x08, (byte)0xF7, (byte)0x04, (byte)0x2E, (byte)0x1D, (byte)0xB2, (byte)0x29,
        (byte)0xFE, (byte)0x4A, (byte)0x0E, (byte)0x11, (byte)0xEE, (byte)0x08, (byte)0x70, (byte)0x1D, (byte)0xF7, (byte)0x25, (byte)0xD3, (byte)0x29, (byte)0xB4, (byte)0x1D, (byte)0x31, (byte)0x09,
        (byte)0x6C, (byte)0x29, (byte)0x33, (byte)0x01, (byte)0x5B, (byte)0x67, (byte)0xD7, (byte)0x56, (byte)0xCF, (byte)0x35, (byte)0x96, (byte)0x4A, (byte)0x56, (byte)0x3A, (byte)0xB0, (byte)0x29,
        (byte)0x58, (byte)0x32, (byte)0x0B, (byte)0x15, (byte)0x86, (byte)0x08, (byte)0x95, (byte)0x4E, (byte)0xF5, (byte)0x21, (byte)0xD0, (byte)0x2D, (byte)0xFA, (byte)0x4A, (byte)0x0A, (byte)0x15,
        (byte)0x57, (byte)0x26, (byte)0x64, (byte)0x04, (byte)0xCF, (byte)0x31, (byte)0x4B, (byte)0x21, (byte)0xB0, (byte)0x1D, (byte)0x6E, (byte)0x15, (byte)0xB6, (byte)0x4E, (byte)0xAE, (byte)0x2D,
        (byte)0x34, (byte)0x2A, (byte)0xD1, (byte)0x1D, (byte)0x85, (byte)0x08, (byte)0x6E, (byte)0x11, (byte)0x75, (byte)0x3A, (byte)0x76, (byte)0x2E, (byte)0x13, (byte)0x26, (byte)0xD9, (byte)0x36,
        (byte)0x4C, (byte)0x11, (byte)0xB0, (byte)0x11, (byte)0x32, (byte)0x3A, (byte)0xFA, (byte)0x36, (byte)0x7F, (byte)0x3F, (byte)0xAF, (byte)0x1D, (byte)0xC7, (byte)0x0C, (byte)0x55, (byte)0x22,
        (byte)0x3C, (byte)0x3B, (byte)0x8E, (byte)0x15, (byte)0x9F, (byte)0x3F, (byte)0x34, (byte)0x0A, (byte)0xB6, (byte)0x3E, (byte)0xF0, (byte)0x25, (byte)0x74, (byte)0x2E, (byte)0x09, (byte)0x05,
        (byte)0xD0, (byte)0x01, (byte)0x33, (byte)0x06, (byte)0x75, (byte)0x06, (byte)0xAE, (byte)0x09, (byte)0x8D, (byte)0x05, (byte)0x4A, (byte)0x25, (byte)0x10, (byte)0x36, (byte)0x08, (byte)0x19,
        (byte)0xEF, (byte)0x21, (byte)0x10, (byte)0x1E, (byte)0x73, (byte)0x22, (byte)0x52, (byte)0x1E, (byte)0x52, (byte)0x0E, (byte)0xEE, (byte)0x11, (byte)0x49, (byte)0x05, (byte)0x50, (byte)0x22,
        (byte)0x69, (byte)0x09, (byte)0x49, (byte)0x19, (byte)0xC5, (byte)0x08, (byte)0xAA, (byte)0x09, (byte)0x69, (byte)0x1D, (byte)0x07, (byte)0x0A, (byte)0x88, (byte)0x0E, (byte)0xEA, (byte)0x21,
        (byte)0x85, (byte)0x0D, (byte)0xC9, (byte)0x21, (byte)0xA7, (byte)0x16, (byte)0x06, (byte)0x16, (byte)0x38, (byte)0x63, (byte)0x6A, (byte)0x29, (byte)0xED, (byte)0x35, (byte)0xA4, (byte)0x10,
        (byte)0xE4, (byte)0x10, (byte)0x41, (byte)0x04, (byte)0x44, (byte)0x11, (byte)0x63, (byte)0x12, (byte)0xE0, (byte)0x05, (byte)0x01, (byte)0x0D, (byte)0x00, (byte)0x16, (byte)0x00, (byte)0x27,
        (byte)0xA0, (byte)0x15, (byte)0x40, (byte)0x11, (byte)0x80, (byte)0x26, (byte)0x01, (byte)0x22, (byte)0x25, (byte)0x1D, (byte)0x20, (byte)0x37, (byte)0x83, (byte)0x21, (byte)0xE8, (byte)0x31,
        (byte)0x80, (byte)0x36, (byte)0x80, (byte)0x21, (byte)0xEB, (byte)0x39, (byte)0x42, (byte)0x3A, (byte)0x00, (byte)0x32, (byte)0xA1, (byte)0x2D, (byte)0x20, (byte)0x21, (byte)0x22, (byte)0x42,
        (byte)0x59, (byte)0x6B, (byte)0x17, (byte)0x63, (byte)0x49, (byte)0x29, (byte)0xC2, (byte)0x39, (byte)0x00, (byte)0x52, (byte)0x20, (byte)0x5A, (byte)0x80, (byte)0x41, (byte)0xC0, (byte)0x55,
        (byte)0x01, (byte)0x31, (byte)0xE0, (byte)0x61, (byte)0x80, (byte)0x51, (byte)0x00, (byte)0x3D, (byte)0xEA, (byte)0x55, (byte)0x01, (byte)0x45, (byte)0x20, (byte)0x51, (byte)0xCA, (byte)0x55,
        (byte)0x71, (byte)0x5E, (byte)0x89, (byte)0x49, (byte)0xEB, (byte)0x5D, (byte)0x6A, (byte)0x35, (byte)0x81, (byte)0x30, (byte)0x27, (byte)0x45, (byte)0xE5, (byte)0x38, (byte)0x04, (byte)0x5D,
        (byte)0x51, (byte)0x5A, (byte)0x83, (byte)0x24, (byte)0x26, (byte)0x59, (byte)0xCD, (byte)0x4D, (byte)0x68, (byte)0x65, (byte)0x82, (byte)0x3C, (byte)0xA2, (byte)0x4C, (byte)0x50, (byte)0x76,
        (byte)0x07, (byte)0x3D, (byte)0xED, (byte)0x71, (byte)0xA4, (byte)0x30, (byte)0xC5, (byte)0x38, (byte)0x62, (byte)0x30, (byte)0x62, (byte)0x34, (byte)0xA4, (byte)0x58, (byte)0x73, (byte)0x52,
        (byte)0x31, (byte)0x4A, (byte)0xCE, (byte)0x3D, (byte)0xAD, (byte)0x39, (byte)0x8C, (byte)0x35, (byte)0xF7, (byte)0x66, (byte)0x8C, (byte)0x39, (byte)0xD6, (byte)0x6A, (byte)0x08, (byte)0x29,
        (byte)0x10, (byte)0x52, (byte)0x8C, (byte)0x41, (byte)0x29, (byte)0x31, (byte)0xAD, (byte)0x49, (byte)0x4A, (byte)0x41, (byte)0xE7, (byte)0x38, (byte)0xC6, (byte)0x30, (byte)0x63, (byte)0x38,
        (byte)0x42, (byte)0x2C, (byte)0xE8, (byte)0x6C, (byte)0x22, (byte)0x54, (byte)0x01, (byte)0x2C, (byte)0x01, (byte)0x28, (byte)0x22, (byte)0x24, (byte)0x01, (byte)0x24, (byte)0x43, (byte)0x24,
        (byte)0x85, (byte)0x24, (byte)0xC7, (byte)0x28, (byte)0x02, (byte)0x1C, (byte)0x01, (byte)0x0C, (byte)0x6C, (byte)0x35, (byte)0x0C, (byte)0x35, (byte)0x89, (byte)0x28, (byte)0xBE, (byte)0x7B,
        (byte)0xFF, (byte)0x7F, (byte)0x07, (byte)0x1C, (byte)0x2C, (byte)0x28, (byte)0x0F, (byte)0x31, (byte)0xCD, (byte)0x28, (byte)0x50, (byte)0x28, (byte)0x32, (byte)0x28, (byte)0xCE, (byte)0x28,
        (byte)0x0E, (byte)0x1C, (byte)0x06, (byte)0x0C, (byte)0x0D, (byte)0x0C, (byte)0x0C, (byte)0x08, (byte)0x8C, (byte)0x14, (byte)0x0A, (byte)0x04, (byte)0xB3, (byte)0x18, (byte)0x01, (byte)0x00
    };

    public static void main(String[] args) {
        System.out.println("=== Schritt 3 KORRIGIERT: ITEMALL/MIXITEM PIX-zu-TIM Konvertierung ===");
        System.out.println("Mit echter Item-Farbpalette und korrekter VRAM-Position");
        System.out.println();

        try {
            String sourceRoot = "info/Re1.5";
            String outputRoot = "extracted";

            Step3CorrectItemPIXToTIMConverter converter = new Step3CorrectItemPIXToTIMConverter();
            converter.convertItemPIXContainers(sourceRoot, outputRoot);

            System.out.println("\n=== SCHRITT 3 KORRIGIERT ABGESCHLOSSEN ===");
            System.out.println("Alle ITEMALL/MIXITEM.PIX Container wurden mit echter Palette zu TIM konvertiert.");

        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void convertItemPIXContainers(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("Quelle: " + sourceRoot);
        System.out.println("Ziel: " + outputRoot);
        System.out.println();

        // Lade echte Item-Palette aus Referenz-TIM
        byte[] realPalette = loadRealItemPalette();
        if (realPalette == null) {
            System.err.println("FEHLER: Konnte echte Item-Palette nicht laden!");
            return;
        }
        System.out.println("✓ Echte Item-Palette geladen (" + realPalette.length + " bytes)");

        // Finde ITEMALL.PIX und MIXITEM.PIX Container
        Files.walk(Paths.get(sourceRoot))
             .filter(Files::isRegularFile)
             .filter(path -> {
                 String name = path.getFileName().toString().toUpperCase();
                 return name.equals("ITEMALL.PIX") || name.equals("MIXITEM.PIX");
             })
             .sorted()
             .forEach(pixFile -> {
                 try {
                     extractItemContainer(pixFile, sourceRoot, outputRoot, realPalette);
                 } catch (Exception e) {
                     System.err.println("Warnung: Fehler bei " + pixFile.getFileName() + ": " + e.getMessage());
                 }
             });
    }

    private byte[] loadRealItemPalette() throws IOException {
        // Verwende die eingebettete korrekte Item-Palette
        // Keine Abhängigkeit zu already_extracted mehr erforderlich!
        System.out.println("Verwende eingebettete Item-Palette (keine Abhängigkeit zu already_extracted)");
        return CORRECT_ITEM_PALETTE.clone();
    }

    private void extractItemContainer(Path pixFile, String sourceRoot, String outputRoot, byte[] realPalette) throws IOException {
        Path sourcePath = Paths.get(sourceRoot);
        Path relativePath = sourcePath.relativize(pixFile);
        String fileName = pixFile.getFileName().toString();
        String baseName = fileName.substring(0, fileName.lastIndexOf('.'));

        System.out.println("PIX-Container: " + relativePath);

        byte[] containerData = Files.readAllBytes(pixFile);
        System.out.println("  Container-Größe: " + containerData.length + " bytes");

        // Erstelle Ausgabeverzeichnis
        Path outputPath = Paths.get(outputRoot).resolve(relativePath.getParent() != null ? relativePath.getParent() : Paths.get(""));
        Path itemOutputPath = outputPath.resolve(baseName);
        Files.createDirectories(itemOutputPath);
        String outputDir = itemOutputPath.toString();

        // Korrekte Container-Struktur
        int itemWidth = 40;
        int itemHeight = 30;
        int itemSize = itemWidth * itemHeight; // 1200 bytes pro Item

        System.out.println("  Item-Größe: " + itemSize + " bytes (40x30 8-bit)");

        int itemCount = containerData.length / itemSize;
        System.out.println("  Item-Anzahl: " + itemCount);

        // Extrahiere Items mit echter Palette
        for (int i = 0; i < itemCount; i++) {
            int offset = i * itemSize;

            if (offset + itemSize <= containerData.length) {
                try {
                    extractSingleItemWithRealPalette(containerData, offset, itemWidth, itemHeight,
                                                   realPalette, outputDir, baseName, i + 1);
                } catch (Exception e) {
                    System.err.println("    Warnung: Item " + (i + 1) + " konnte nicht extrahiert werden: " + e.getMessage());
                }
            }
        }

        System.out.println("  ✓ " + itemCount + " Items mit echter Palette aus " + baseName + " extrahiert");
    }

    private void extractSingleItemWithRealPalette(byte[] containerData, int offset, int width, int height,
                                                byte[] realPalette, String outputDir, String baseName, int itemNumber) throws IOException {

        System.out.println("    Extrahiere Item " + String.format("%03d", itemNumber) + " mit echter Palette");

        // Extrahiere Pixeldaten
        byte[] pixelData = new byte[width * height];
        System.arraycopy(containerData, offset, pixelData, 0, pixelData.length);

        // Erstelle TIM mit echter Palette
        byte[] timData = createCorrectTIM(pixelData, realPalette, width, height);

        String timPath = outputDir + "/" + baseName + "_" + String.format("%03d", itemNumber) + ".TIM";
        Files.write(Paths.get(timPath), timData);

        System.out.println("      ✓ " + baseName + "_" + String.format("%03d", itemNumber) + ".TIM mit echter Palette erstellt");
    }

    private byte[] createCorrectTIM(byte[] pixelData, byte[] realPalette, int width, int height) throws IOException {
        ByteArrayOutputStream timStream = new ByteArrayOutputStream();

        // TIM Header
        timStream.write(0x10); // TIM Tag
        timStream.write(0x00); // Version
        timStream.write(0x00); // Padding
        timStream.write(0x00); // Padding

        // TIM Flags: 8-bit (0x01) + CLUT present (0x08) = 0x09
        writeInt32LE(timStream, 0x09);

        // CLUT Block mit echter Palette
        ByteArrayOutputStream clutStream = new ByteArrayOutputStream();

        // CLUT Header mit korrekter VRAM-Position
        writeInt32LE(clutStream, 12 + realPalette.length); // CLUT Size
        writeInt16LE(clutStream, 0);       // Org X
        writeInt16LE(clutStream, 496);     // Org Y (korrekte VRAM-Position)
        writeInt16LE(clutStream, 256);     // Color Count
        writeInt16LE(clutStream, 1);       // Palette Count

        // Echte Palette hinzufügen
        clutStream.write(realPalette);
        timStream.write(clutStream.toByteArray());

        // Image Block
        ByteArrayOutputStream imageStream = new ByteArrayOutputStream();

        writeInt32LE(imageStream, 12 + pixelData.length); // Image Size
        writeInt16LE(imageStream, 0);      // Org X
        writeInt16LE(imageStream, 0);      // Org Y
        writeInt16LE(imageStream, width / 2); // Width in 16-bit words
        writeInt16LE(imageStream, height); // Height

        // Pixeldaten direkt (kein Byte-Swapping bei 8-bit)
        imageStream.write(pixelData);
        timStream.write(imageStream.toByteArray());

        return timStream.toByteArray();
    }

    private int readInt32LE(byte[] data, int offset) {
        if (offset + 3 >= data.length) return 0;
        return (data[offset] & 0xFF) |
               ((data[offset + 1] & 0xFF) << 8) |
               ((data[offset + 2] & 0xFF) << 16) |
               ((data[offset + 3] & 0xFF) << 24);
    }

    private void writeInt32LE(ByteArrayOutputStream stream, int value) {
        stream.write(value & 0xFF);
        stream.write((value >> 8) & 0xFF);
        stream.write((value >> 16) & 0xFF);
        stream.write((value >> 24) & 0xFF);
    }

    private void writeInt16LE(ByteArrayOutputStream stream, int value) {
        stream.write(value & 0xFF);
        stream.write((value >> 8) & 0xFF);
    }
}
