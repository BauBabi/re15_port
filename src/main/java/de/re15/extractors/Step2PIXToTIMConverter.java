package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Schritt 2: PIX-zu-TIM Konverter für MAP*.PIX Dateien
 * Konvertiert die Rohdaten in gültige TIM-Dateien
 */
public class Step2PIXToTIMConverter {

    public static void main(String[] args) {
        System.out.println("=== Schritt 2: PIX-zu-TIM Konvertierung ===");
        System.out.println("Fokus auf MAP*.PIX Dateien (32KB Rohdaten)");
        System.out.println();

        try {
            String sourceRoot = "info/Re1.5";
            String outputRoot = "extracted";

            Step2PIXToTIMConverter converter = new Step2PIXToTIMConverter();
            converter.convertMapPIXFiles(sourceRoot, outputRoot);

            System.out.println("\n=== SCHRITT 2 ABGESCHLOSSEN ===");
            System.out.println("Alle MAP*.PIX Dateien wurden zu TIM konvertiert.");

        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void convertMapPIXFiles(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("Quelle: " + sourceRoot);
        System.out.println("Ziel: " + outputRoot);
        System.out.println();

        // Finde alle MAP*.PIX Dateien
        Files.walk(Paths.get(sourceRoot))
             .filter(Files::isRegularFile)
             .filter(path -> {
                 String name = path.getFileName().toString().toUpperCase();
                 return name.startsWith("MAP") && name.endsWith(".PIX");
             })
             .sorted()
             .forEach(pixFile -> {
                 try {
                     convertMapPIXToTIM(pixFile, sourceRoot, outputRoot);
                 } catch (Exception e) {
                     System.err.println("Warnung: Fehler bei " + pixFile.getFileName() + ": " + e.getMessage());
                 }
             });
    }

    private void convertMapPIXToTIM(Path pixFile, String sourceRoot, String outputRoot) throws IOException {
        Path sourcePath = Paths.get(sourceRoot);
        Path relativePath = sourcePath.relativize(pixFile);
        String fileName = pixFile.getFileName().toString();
        String baseName = fileName.substring(0, fileName.lastIndexOf('.'));

        // Erstelle entsprechende Ausgabestruktur
        Path outputPath = Paths.get(outputRoot).resolve(relativePath.getParent() != null ? relativePath.getParent() : Paths.get(""));
        Files.createDirectories(outputPath);
        String outputDir = outputPath.toString();

        System.out.println("PIX-Datei: " + relativePath);

        byte[] pixData = Files.readAllBytes(pixFile);

        if (pixData.length == 32768) {
            // MAP-PIX: 32KB = 256x256 4-bit Pixel ohne Palette
            System.out.println("  MAP-PIX erkannt: 256x256 4-bit Rohdaten (ohne Palette)");

            // Erstelle TIM-Datei mit Standard-Graustufen-Palette
            byte[] timData = createTIMFromMapPIX(pixData);

            // Speichere TIM-Datei
            String timPath = outputDir + "/" + baseName + ".TIM";
            Files.write(Paths.get(timPath), timData);

            System.out.println("  ✓ " + baseName + ".TIM erstellt (256x256 4-bit mit Standard-Palette)");

        } else {
            System.out.println("  Unbekannte PIX-Größe: " + pixData.length + " bytes - übersprungen");
        }
    }

    /**
     * Erstellt eine TIM-Datei aus MAP-PIX Rohdaten
     * MAP-PIX = 256x256 4-bit Pixel ohne Header und ohne Palette
     */
    private byte[] createTIMFromMapPIX(byte[] pixData) throws IOException {
        ByteArrayOutputStream timStream = new ByteArrayOutputStream();

        // TIM Header
        timStream.write(0x10); // TIM Tag
        timStream.write(0x00); // Version
        timStream.write(0x00); // Padding
        timStream.write(0x00); // Padding

        // TIM Flags: 4-bit (0x00) + CLUT present (0x08) = 0x08
        writeInt32LE(timStream, 0x08);

        // CLUT Block erstellen (Echte MAP-Farbpalette aus extrahierter TIM)
        ByteArrayOutputStream clutStream = new ByteArrayOutputStream();

        // CLUT Header
        writeInt32LE(clutStream, 12 + 32); // CLUT Size: Header(12) + Palette(16*2)
        writeInt16LE(clutStream, 0);       // Org X
        writeInt16LE(clutStream, 0);       // Org Y
        writeInt16LE(clutStream, 16);      // Color Count (16 Farben)
        writeInt16LE(clutStream, 1);       // Palette Count (1 Palette)

        // Echte MAP-Farbpalette (BGR555 Format) - extrahiert aus MAP01.TIM
        int[] realMapPalette = {
            0x0000, 0x8120, 0x81A4, 0x12DC, 0x5AD6, 0x677B, 0x4EB5, 0x294A,
            0x9C85, 0x9084, 0x0000, 0x12DC, 0xB002, 0xB005, 0x0853, 0x0850
        };

        for (int color : realMapPalette) {
            writeInt16LE(clutStream, color);
        }

        // CLUT zu TIM hinzufügen
        timStream.write(clutStream.toByteArray());

        // Image Block erstellen
        ByteArrayOutputStream imageStream = new ByteArrayOutputStream();

        // Image Header
        writeInt32LE(imageStream, 12 + pixData.length); // Image Size: Header(12) + Data
        writeInt16LE(imageStream, 0);      // Org X
        writeInt16LE(imageStream, 0);      // Org Y
        writeInt16LE(imageStream, 64);     // Width in 16-bit words (256 pixel / 4 pixel per word = 64)
        writeInt16LE(imageStream, 256);    // Height in pixels

        // Pixeldaten direkt kopieren (sind bereits im richtigen 4-bit Format)
        imageStream.write(pixData);

        // Image Block zu TIM hinzufügen
        timStream.write(imageStream.toByteArray());

        return timStream.toByteArray();
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
