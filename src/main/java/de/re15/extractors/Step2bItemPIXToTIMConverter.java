package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Schritt 2b: PIX-zu-TIM Konverter für ITEMALL und MIXITEM PIX-Dateien
 * Konvertiert die Item-Sprites zu einzelnen TIM-Dateien (8-bit Format)
 */
public class Step2bItemPIXToTIMConverter {

    public static void main(String[] args) {
        System.out.println("=== Schritt 2b: ITEMALL/MIXITEM PIX-zu-TIM Konvertierung ===");
        System.out.println("Konvertiert Item-Sprites zu einzelnen TIM-Dateien (8-bit Format)");
        System.out.println();

        try {
            String sourceRoot = "info/Re1.5";
            String outputRoot = "extracted";

            Step2bItemPIXToTIMConverter converter = new Step2bItemPIXToTIMConverter();

            // Lösche alte falsche TIM-Dateien
            converter.cleanupOldTIMFiles(outputRoot);

            converter.convertItemPIXFiles(sourceRoot, outputRoot);

            System.out.println("\n=== SCHRITT 2b ABGESCHLOSSEN ===");
            System.out.println("Alle ITEMALL/MIXITEM PIX-Dateien wurden zu TIM konvertiert (8-bit).");

        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private void cleanupOldTIMFiles(String outputRoot) throws IOException {
        System.out.println("Lösche alte falsche TIM-Dateien...");

        Path itemallDir = Paths.get(outputRoot, "PSX", "DATA", "ITEMALL");
        Path mixitemDir = Paths.get(outputRoot, "PSX", "DATA", "MIXITEM");

        if (Files.exists(itemallDir)) {
            Files.walk(itemallDir).filter(Files::isRegularFile).forEach(file -> {
                try {
                    Files.delete(file);
                } catch (IOException e) {
                    System.err.println("Warnung: Kann " + file + " nicht löschen");
                }
            });
        }

        if (Files.exists(mixitemDir)) {
            Files.walk(mixitemDir).filter(Files::isRegularFile).forEach(file -> {
                try {
                    Files.delete(file);
                } catch (IOException e) {
                    System.err.println("Warnung: Kann " + file + " nicht löschen");
                }
            });
        }

        System.out.println("Alte TIM-Dateien gelöscht.");
    }

    public void convertItemPIXFiles(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("Quelle: " + sourceRoot);
        System.out.println("Ziel: " + outputRoot);
        System.out.println();

        // Finde ITEMALL und MIXITEM PIX-Dateien
        Files.walk(Paths.get(sourceRoot))
             .filter(Files::isRegularFile)
             .filter(path -> {
                 String name = path.getFileName().toString().toUpperCase();
                 return (name.equals("ITEMALL.PIX") || name.equals("MIXITEM.PIX"));
             })
             .sorted()
             .forEach(pixFile -> {
                 try {
                     convertItemPIXToTIMs(pixFile, sourceRoot, outputRoot);
                 } catch (Exception e) {
                     System.err.println("Warnung: Fehler bei " + pixFile.getFileName() + ": " + e.getMessage());
                 }
             });
    }

    private void convertItemPIXToTIMs(Path pixFile, String sourceRoot, String outputRoot) throws IOException {
        Path sourcePath = Paths.get(sourceRoot);
        Path relativePath = sourcePath.relativize(pixFile);
        String fileName = pixFile.getFileName().toString();
        String baseName = fileName.substring(0, fileName.lastIndexOf('.'));

        // Erstelle Ausgabeverzeichnis für Item-Sprites
        Path outputPath = Paths.get(outputRoot).resolve(relativePath.getParent() != null ? relativePath.getParent() : Paths.get(""));
        Path itemOutputDir = outputPath.resolve(baseName);
        Files.createDirectories(itemOutputDir);

        System.out.println("PIX-Datei: " + relativePath);

        byte[] pixData = Files.readAllBytes(pixFile);

        // Berechne Anzahl der Sprites (8-bit: 40x30 = 1200 bytes)
        int spriteDataSize = 1200; // 40x30 8-bit = 1200 bytes
        int spriteCount = pixData.length / spriteDataSize;

        System.out.println("  " + baseName + "-PIX erkannt: " + spriteCount + " Sprites (40x30 8-bit je 1200 bytes)");

        // Extrahiere jeden Sprite als separate TIM-Datei
        for (int i = 0; i < spriteCount; i++) {
            int offset = i * spriteDataSize;
            byte[] spriteData = new byte[spriteDataSize];
            System.arraycopy(pixData, offset, spriteData, 0, spriteDataSize);

            // Erstelle TIM-Datei für diesen Sprite
            byte[] timData = createTIMFromItemSprite8Bit(spriteData);

            // Speichere TIM-Datei
            String spriteName = String.format("%s_%03d.TIM", baseName, i + 1);
            Path timPath = itemOutputDir.resolve(spriteName);
            Files.write(timPath, timData);

            if ((i + 1) % 10 == 0 || i == spriteCount - 1) {
                System.out.println("  ✓ " + (i + 1) + "/" + spriteCount + " Sprites konvertiert");
            }
        }

        System.out.println("  ✓ " + baseName + " vollständig: " + spriteCount + " TIM-Dateien erstellt");
    }

    /**
     * Erstellt eine TIM-Datei aus einem Item-Sprite mit exakter Referenz-Struktur
     */
    private byte[] createTIMFromItemSprite8Bit(byte[] spriteData) throws IOException {
        ByteArrayOutputStream timStream = new ByteArrayOutputStream();

        // TIM Header
        timStream.write(0x10); // TIM Tag
        timStream.write(0x00); // Version
        timStream.write(0x00); // Padding
        timStream.write(0x00); // Padding

        // TIM Flags: 8-bit (0x01) + CLUT present (0x08) = 0x09
        writeInt32LE(timStream, 0x09);

        // CLUT Block erstellen (256 Farben für 8-bit)
        ByteArrayOutputStream clutStream = new ByteArrayOutputStream();

        // CLUT Header
        writeInt32LE(clutStream, 12 + 512); // CLUT Size: Header(12) + Palette(256*2)
        writeInt16LE(clutStream, 0);        // Org X
        writeInt16LE(clutStream, 0);        // Org Y
        writeInt16LE(clutStream, 256);      // Color Count (256 Farben)
        writeInt16LE(clutStream, 1);        // Palette Count (1 Palette)

        // Extrahiere 256-Farben Palette aus Referenz-TIM
        int[] palette256 = extract256ColorPalette();
        for (int color : palette256) {
            writeInt16LE(clutStream, color);
        }

        // CLUT zu TIM hinzufügen
        timStream.write(clutStream.toByteArray());

        // Image Block erstellen - mit korrekter Bildgröße
        ByteArrayOutputStream imageStream = new ByteArrayOutputStream();

        // Image Header - korrigierte Werte basierend auf Referenz-Analyse
        writeInt32LE(imageStream, 12 + 1200); // Image Size: 1212 bytes wie Referenz (12 + 1200)
        writeInt16LE(imageStream, 20);         // Org X: 20 (wie Referenz)
        writeInt16LE(imageStream, 30);         // Org Y: 30 (wie Referenz)
        writeInt16LE(imageStream, 20);         // Width: 20 pixels (wie Referenz)
        writeInt16LE(imageStream, 30);         // Height: 30 pixels (wie Referenz)

        // Alle 1200 bytes der Sprite-Daten verwenden (40x30 8-bit)
        imageStream.write(spriteData);

        // Image Block zu TIM hinzufügen
        timStream.write(imageStream.toByteArray());

        return timStream.toByteArray();
    }

    /**
     * Extrahiert die 256-Farben Palette - verwendet korrekte extrahierte Palette
     */
    private int[] extract256ColorPalette() throws IOException {
        // Verwende die gleiche korrekte Palette wie Step3CorrectItemPIXToTIMConverter
        // Diese wurde aus der funktionierenden Referenz extrahiert

        // Korrekte Item-Palette (wie in Step3CorrectItemPIXToTIMConverter)
        byte[] correctPalette = {
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

        // Konvertiere byte[] zu int[] (BGR555 zu RGB)
        int[] palette = new int[256];
        for (int i = 0; i < 256; i++) {
            if (i * 2 + 1 < correctPalette.length) {
                int bgr555 = ((correctPalette[i * 2 + 1] & 0xFF) << 8) | (correctPalette[i * 2] & 0xFF);

                // BGR555 zu RGB konvertieren
                int r = (bgr555 & 0x1F) << 3;
                int g = ((bgr555 >> 5) & 0x1F) << 3;
                int b = ((bgr555 >> 10) & 0x1F) << 3;

                palette[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
            } else {
                palette[i] = 0xFF000000; // Fallback: Schwarz
            }
        }

        return palette;
    }

    private int readInt32LE(byte[] data, int offset) {
        return (data[offset] & 0xFF) |
               ((data[offset + 1] & 0xFF) << 8) |
               ((data[offset + 2] & 0xFF) << 16) |
               ((data[offset + 3] & 0xFF) << 24);
    }

    private int readInt16LE(byte[] data, int offset) {
        return (data[offset] & 0xFF) |
               ((data[offset + 1] & 0xFF) << 8);
    }

    /**
     * Schreibt einen 32-bit Wert im Little-Endian Format
     */
    private void writeInt32LE(ByteArrayOutputStream stream, int value) {
        stream.write(value & 0xFF);
        stream.write((value >> 8) & 0xFF);
        stream.write((value >> 16) & 0xFF);
        stream.write((value >> 24) & 0xFF);
    }

    /**
     * Schreibt einen 16-bit Wert im Little-Endian Format
     */
    private void writeInt16LE(ByteArrayOutputStream stream, int value) {
        stream.write(value & 0xFF);
        stream.write((value >> 8) & 0xFF);
    }
}
