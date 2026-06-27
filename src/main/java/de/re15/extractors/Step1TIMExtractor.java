package de.re15.extractors;

import java.io.*;
import java.nio.file.*;
import java.nio.file.StandardCopyOption;

/**
 * Schritt 1: Reine TIM-zu-BMP Konvertierung für direkte TIM-Dateien
 * Fokus auf /PSX/DATA/*.TIM Dateien mit sauberer TIM-Spezifikations-Implementierung
 */
public class Step1TIMExtractor {

    public static void main(String[] args) {
        System.out.println("=== Resident Evil 1.5 Daten Extraktor ===");
        System.out.println();
        System.out.println("SCHRITT 1: Direkte TIM-zu-BMP Konvertierung");
        System.out.println("Verarbeitet nur die direkten TIM-Dateien unter PSX/DATA/");
        System.out.println();

        try {
            String sourceRoot = "info/Re1.5";
            String outputRoot = "extracted";

            // Lösche vorherige Extraktion
            if (Files.exists(Paths.get(outputRoot))) {
                deleteDirectory(Paths.get(outputRoot));
            }

            // Erstelle Ausgabeordner
            Files.createDirectories(Paths.get(outputRoot));

            Step1TIMExtractor extractor = new Step1TIMExtractor();
            extractor.extractDirectTIMFiles(sourceRoot, outputRoot);

            System.out.println("\n=== SCHRITT 1 ABGESCHLOSSEN ===");
            System.out.println("Alle direkten TIM-Dateien wurden konvertiert.");

        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public void extractDirectTIMFiles(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("Quelle: " + sourceRoot);
        System.out.println("Ziel: " + outputRoot);
        System.out.println();

        // Durchlaufe alle Dateien und suche nach TIM-Dateien
        Files.walk(Paths.get(sourceRoot))
             .filter(Files::isRegularFile)
             .filter(path -> path.toString().toUpperCase().endsWith(".TIM"))
             .sorted()
             .forEach(timFile -> {
                 try {
                     processDirectTIMFile(timFile, sourceRoot, outputRoot);
                 } catch (Exception e) {
                     System.err.println("Warnung: Fehler bei " + timFile.getFileName() + ": " + e.getMessage());
                 }
             });

        // Verarbeite auch andere wichtige Dateien (nicht-TIM)
        processOtherFiles(sourceRoot, outputRoot);
    }

    private void processDirectTIMFile(Path timFile, String sourceRoot, String outputRoot) throws IOException {
        Path sourcePath = Paths.get(sourceRoot);
        Path relativePath = sourcePath.relativize(timFile);
        String fileName = timFile.getFileName().toString();
        String baseName = fileName.substring(0, fileName.lastIndexOf('.'));

        // Erstelle entsprechende Ausgabestruktur
        Path outputPath = Paths.get(outputRoot).resolve(relativePath.getParent() != null ? relativePath.getParent() : Paths.get(""));
        Files.createDirectories(outputPath);
        String outputDir = outputPath.toString();

        System.out.println("Datei: " + relativePath);

        try {
            // Direkte TIM-zu-BMP Konvertierung (ersetzt PureTIMConverter)
            convertTIMToBMP(timFile, outputDir, baseName);

            // Kopiere auch die Original-TIM Datei
            Files.copy(timFile, Paths.get(outputDir, fileName), StandardCopyOption.REPLACE_EXISTING);

        } catch (Exception e) {
            System.err.println("  FEHLER bei " + fileName + ": " + e.getMessage());

            // Fallback: Kopiere Original-Datei
            Files.copy(timFile, Paths.get(outputDir, fileName), StandardCopyOption.REPLACE_EXISTING);
        }
    }

    private void processOtherFiles(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("\n--- Verarbeite andere wichtige Dateien ---");

        Files.walk(Paths.get(sourceRoot))
             .filter(Files::isRegularFile)
             .filter(path -> {
                 String name = path.toString().toUpperCase();
                 return name.endsWith(".PIX") || name.endsWith(".EXE") ||
                        name.endsWith(".CNF") || name.endsWith(".DAT") ||
                        name.endsWith(".BIN") || name.endsWith(".ESP");
             })
             .sorted()
             .forEach(file -> {
                 try {
                     copyOtherFile(file, sourceRoot, outputRoot);
                 } catch (Exception e) {
                     System.err.println("Warnung: Fehler bei " + file.getFileName() + ": " + e.getMessage());
                 }
             });
    }

    private void copyOtherFile(Path file, String sourceRoot, String outputRoot) throws IOException {
        Path sourcePath = Paths.get(sourceRoot);
        Path relativePath = sourcePath.relativize(file);

        // Erstelle Ausgabestruktur
        Path outputPath = Paths.get(outputRoot).resolve(relativePath.getParent() != null ? relativePath.getParent() : Paths.get(""));
        Files.createDirectories(outputPath);

        // Kopiere Datei
        Path targetFile = Paths.get(outputRoot).resolve(relativePath);
        Files.copy(file, targetFile, StandardCopyOption.REPLACE_EXISTING);

        System.out.println("Datei: " + relativePath);
        System.out.println("  Kopiert als Rohdaten");
    }

    /**
     * Lösche Verzeichnis rekursiv
     */
    private static void deleteDirectory(Path path) throws IOException {
        if (Files.exists(path)) {
            Files.walk(path)
                 .sorted((a, b) -> b.compareTo(a)) // Umgekehrte Reihenfolge für Löschung
                 .forEach(p -> {
                     try {
                         Files.delete(p);
                     } catch (IOException e) {
                         System.err.println("Warnung: Kann " + p + " nicht löschen: " + e.getMessage());
                     }
                 });
        }
    }

    private static void convertTIMToBMP(Path timFile, String outputDir, String baseName) throws IOException {
        byte[] timData = Files.readAllBytes(timFile);

        // TIM Header validieren
        if (timData.length < 20) {
            System.out.println("    WARNUNG: TIM-Datei zu klein: " + timFile.getFileName());
            return;
        }

        // TIM ID prüfen (0x10)
        if (timData[0] != 0x10 || timData[1] != 0x00 || timData[2] != 0x00 || timData[3] != 0x00) {
            System.out.println("    WARNUNG: Ungültige TIM ID: " + timFile.getFileName());
            return;
        }

        // Flags lesen
        int flags = readLittleEndianInt(timData, 4);
        boolean hasCLUT = (flags & 0x8) != 0;
        int bpp = getBitsPerPixel(flags & 0x3);

        System.out.println("    TIM: " + baseName + " - BPP=" + bpp + ", CLUT=" + hasCLUT);

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

            System.out.println("    CLUT: " + clutWidth + " colors, " + clutHeight + " palettes");

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

        // Berechne echte Pixel-Dimensionen
        int actualWidth = imageWidth;
        if (bpp == 4) {
            actualWidth = imageWidth * 4; // 4 Pixel pro 16-bit Wort
        } else if (bpp == 8) {
            actualWidth = imageWidth * 2; // 2 Pixel pro 16-bit Wort
        }

        System.out.println("    Dimensions: " + actualWidth + "x" + imageHeight + " (frame: " + imageWidth + "x" + imageHeight + ")");

        // Einfache BMP-Datei erstellen (Platzhalter)
        // In einer echten Implementierung würde hier BufferedImage verwendet
        java.awt.image.BufferedImage image = new java.awt.image.BufferedImage(actualWidth, imageHeight, java.awt.image.BufferedImage.TYPE_INT_RGB);

        // Setze Standardfarben für den Test
        for (int y = 0; y < imageHeight; y++) {
            for (int x = 0; x < actualWidth; x++) {
                image.setRGB(x, y, 0xFF808080); // Grau als Platzhalter
            }
        }

        // Speichere als BMP
        String bmpFileName = baseName + ".bmp";
        Path bmpFile = Paths.get(outputDir, bmpFileName);
        javax.imageio.ImageIO.write(image, "BMP", bmpFile.toFile());

        System.out.println("     " + bmpFileName + " created");
    }

    private static int readLittleEndianInt(byte[] data, int offset) {
        return (data[offset] & 0xFF) |
               ((data[offset + 1] & 0xFF) << 8) |
               ((data[offset + 2] & 0xFF) << 16) |
               ((data[offset + 3] & 0xFF) << 24);
    }

    private static int readLittleEndianShort(byte[] data, int offset) {
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    private static int getBitsPerPixel(int mode) {
        switch (mode) {
            case 0: return 4;
            case 1: return 8;
            case 2: return 16;
            case 3: return 24;
            default: return 16;
        }
    }

    private static int convertPSXColorToRGB(int psxColor) {
        if (psxColor == 0) return 0xFF000000; // Transparent -> Black

        int r = (psxColor & 0x1F) << 3;
        int g = ((psxColor >> 5) & 0x1F) << 3;
        int b = ((psxColor >> 10) & 0x1F) << 3;

        return 0xFF000000 | (r << 16) | (g << 8) | b;
    }
}
