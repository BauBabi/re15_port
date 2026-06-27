package de.re15.extractors;

import java.io.*;
import java.nio.file.*;
import java.util.*;

/**
 * STEP 5: ESP (Effect Sprite) Extraktor
 *
 * Extrahiert ESP-Dateien zu EFF-Dateien (Effect Files).
 * ESP-Dateien enthalten Animationseffekte für das Spiel.
 *
 * Verwendet den finalen ESP-Extraktor mit korrektem Header-Offset.
 */
public class Step5ESPExtractor {

    public void extractESPFiles(String sourceRoot, String outputRoot) throws IOException {
        System.out.println("=== ESP-DATEIEN EXTRAKTION GESTARTET ===");

        Path sourcePath = Paths.get(sourceRoot);
        Path outputPath = Paths.get(outputRoot);

        // Finde alle ESP-Dateien
        List<Path> espFiles = new ArrayList<>();
        Files.walk(sourcePath)
             .filter(Files::isRegularFile)
             .filter(path -> path.getFileName().toString().toUpperCase().endsWith(".ESP"))
             .forEach(espFiles::add);

        if (espFiles.isEmpty()) {
            System.out.println("Keine ESP-Dateien gefunden.");
            return;
        }

        System.out.println("Gefundene ESP-Dateien: " + espFiles.size());

        for (Path espFile : espFiles) {
            try {
                extractSingleESPFile(espFile, sourcePath, outputPath);
            } catch (Exception e) {
                System.err.println("Fehler beim Extrahieren von " + espFile.getFileName() + ": " + e.getMessage());
            }
        }

        System.out.println("=== ESP-EXTRAKTION ABGESCHLOSSEN ===");
    }

    private void extractSingleESPFile(Path espFile, Path sourceRoot, Path outputRoot) throws IOException {
        // Erstelle relative Pfadstruktur
        Path relativePath = sourceRoot.relativize(espFile);
        Path targetDirectory = outputRoot.resolve(relativePath.getParent());

        System.out.println("Extrahiere ESP: " + relativePath);

        // Verwende den finalen ESP-Extraktor
        ESPExtractorFinal.extractESP(espFile.toString(), targetDirectory.toString());
    }
}
