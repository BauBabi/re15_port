package de.re15.converters;

import java.io.*;
import java.nio.file.*;
import java.util.*;

/**
 * EFFEKT-ANALYSATOR und KONVERTER für Resident Evil 1.5
 *
 * Analysiert .eff Dateien und konvertiert sie in moderne JSON-Formate.
 * Basiert auf der Analyse der BioRdt-Referenz-Implementierung und PlayStation-Effekt-Strukturen.
 */
public class EffectAnalyzer {

    public static void main(String[] args) {
        try {
            System.out.println("======================================================");
            System.out.println("RESIDENT EVIL 1.5 EFFEKT-ANALYSATOR & KONVERTER");
            System.out.println("======================================================");

            String inputDir = "extracted/PSX/DATA/CORE00";
            String outputDir = "analyzed_effects";

            analyzeAndConvertEffectFiles(inputDir, outputDir);

            System.out.println("======================================================");
            System.out.println("EFFEKT-ANALYSE UND KONVERTIERUNG ABGESCHLOSSEN!");
            System.out.println("======================================================");

        } catch (Exception e) {
            System.err.println("FEHLER: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public static void analyzeAndConvertEffectFiles(String inputDir, String outputDir) throws IOException {
        Path inputPath = Paths.get(inputDir);
        Path outputPath = Paths.get(outputDir);
        Files.createDirectories(outputPath);

        System.out.println("Analysiere und konvertiere Effekt-Dateien in: " + inputDir);
        System.out.println("Ausgabe: " + outputDir);

        // Finde alle .eff Dateien
        try (var stream = Files.walk(inputPath)) {
            stream.filter(path -> path.toString().toLowerCase().endsWith(".eff"))
                  .forEach(effFile -> {
                      try {
                          analyzeAndConvertEffectFile(effFile, outputPath);
                      } catch (IOException e) {
                          System.err.println("Fehler beim Verarbeiten von " + effFile + ": " + e.getMessage());
                      }
                  });
        }
    }

    private static void analyzeAndConvertEffectFile(Path effFile, Path outputPath) throws IOException {
        System.out.println("Verarbeite: " + effFile.getFileName());

        byte[] data = Files.readAllBytes(effFile);
        String baseName = effFile.getFileName().toString().replaceAll("\\.[^.]*$", "");

        // Analysiere PlayStation-Effekt-Struktur
        PlayStationEffect effect = parsePlayStationEffect(data, baseName);

        // Exportiere als modernes JSON
        exportAsModernJSON(effect, outputPath, baseName);

        // Erstelle detaillierte Analyse
        createDetailedAnalysis(effect, data, outputPath, baseName);

        System.out.println("  -> Konvertiert zu: " + baseName + "_effect.json");
    }

    /**
     * Analysiert EFF-Dateien in einem Verzeichnis und speichert JSON-Analysen im gleichen Ordner
     */
    public static void analyzeEffectFilesInPlace(String directory) throws IOException {
        Path dirPath = Paths.get(directory);
        if (!Files.exists(dirPath)) {
            System.out.println("Verzeichnis nicht gefunden: " + directory);
            return;
        }

        System.out.println("Analysiere EFF-Dateien in: " + directory);

        // Finde alle .eff Dateien
        try (var stream = Files.walk(dirPath)) {
            stream.filter(path -> path.toString().toLowerCase().endsWith(".eff"))
                  .forEach(effFile -> {
                      try {
                          analyzeEffectFileInPlace(effFile);
                      } catch (IOException e) {
                          System.err.println("Fehler beim Verarbeiten von " + effFile + ": " + e.getMessage());
                      }
                  });
        }
    }

    private static void analyzeEffectFileInPlace(Path effFile) throws IOException {
        System.out.println("Analysiere: " + effFile.getFileName());

        byte[] data = Files.readAllBytes(effFile);
        String baseName = effFile.getFileName().toString().replaceAll("\\.[^.]*$", "");

        // Analysiere PlayStation-Effekt-Struktur
        PlayStationEffect effect = parsePlayStationEffect(data, baseName);

        // Exportiere JSON im gleichen Verzeichnis wie die EFF-Datei
        Path outputDir = effFile.getParent();
        exportAsModernJSON(effect, outputDir, baseName);

        // Erstelle detaillierte Analyse im gleichen Verzeichnis
        createDetailedAnalysis(effect, data, outputDir, baseName);

        System.out.println("  -> JSON erstellt: " + baseName + "_effect.json");
    }

    private static PlayStationEffect parsePlayStationEffect(byte[] data, String name) {
        PlayStationEffect effect = new PlayStationEffect();
        effect.name = name;
        effect.size = data.length;

        // Analysiere Header (erste ~160 Bytes)
        int offset = 0;
        List<EffectFrame> frames = new ArrayList<>();

        // Parse Frame-Definitionen
        while (offset < Math.min(160, data.length - 8)) {
            if (data[offset] == 0 && data[offset + 1] == 0 &&
                data[offset + 2] == 0 && data[offset + 3] == 0) {
                break; // Ende der Frame-Definitionen
            }

            EffectFrame frame = new EffectFrame();
            frame.frameId = readUInt16(data, offset);
            frame.type = readUInt16(data, offset + 2);
            frame.x = readUInt16(data, offset + 4);
            frame.y = readUInt16(data, offset + 6);

            frames.add(frame);
            offset += 8;
        }

        effect.frames = frames;

        // Analysiere Koordinaten-Daten (ab Offset ~0xD0)
        List<EffectCoordinate> coordinates = new ArrayList<>();
        offset = 0xD0;

        while (offset < data.length - 4) {
            int x = readSInt16(data, offset);
            int y = readSInt16(data, offset + 2);

            // Plausibilitätsprüfung für PlayStation-Koordinaten
            if (Math.abs(x) <= 1024 && Math.abs(y) <= 1024) {
                EffectCoordinate coord = new EffectCoordinate();
                coord.x = x;
                coord.y = y;
                coordinates.add(coord);
            }

            offset += 4;
        }

        effect.coordinates = coordinates;

        // Berechne Effekt-Eigenschaften
        calculateEffectProperties(effect);

        return effect;
    }

    private static void calculateEffectProperties(PlayStationEffect effect) {
        if (!effect.coordinates.isEmpty()) {
            int minX = effect.coordinates.stream().mapToInt(c -> c.x).min().orElse(0);
            int maxX = effect.coordinates.stream().mapToInt(c -> c.x).max().orElse(0);
            int minY = effect.coordinates.stream().mapToInt(c -> c.y).min().orElse(0);
            int maxY = effect.coordinates.stream().mapToInt(c -> c.y).max().orElse(0);

            effect.boundingBox = new BoundingBox();
            effect.boundingBox.minX = minX;
            effect.boundingBox.maxX = maxX;
            effect.boundingBox.minY = minY;
            effect.boundingBox.maxY = maxY;
            effect.boundingBox.width = maxX - minX;
            effect.boundingBox.height = maxY - minY;
        }

        effect.frameCount = effect.frames.size();
        effect.coordinateCount = effect.coordinates.size();

        // Bestimme Effekt-Typ basierend auf Daten
        if (effect.frameCount > 10) {
            effect.effectType = "COMPLEX_ANIMATION";
        } else if (effect.coordinateCount > 50) {
            effect.effectType = "PARTICLE_SYSTEM";
        } else if (effect.frameCount > 0) {
            effect.effectType = "SPRITE_ANIMATION";
        } else {
            effect.effectType = "STATIC_EFFECT";
        }
    }

    private static void exportAsModernJSON(PlayStationEffect effect, Path outputPath, String baseName) throws IOException {
        Path jsonFile = outputPath.resolve(baseName + "_effect.json");

        try (PrintWriter writer = new PrintWriter(Files.newBufferedWriter(jsonFile))) {
            writer.println("{");
            writer.println("  \"metadata\": {");
            writer.println("    \"name\": \"" + effect.name + "\",");
            writer.println("    \"originalFormat\": \"PlayStation Effect (.eff)\",");
            writer.println("    \"game\": \"Resident Evil 1.5\",");
            writer.println("    \"extractedBy\": \"RE15 Effect Analyzer\",");
            writer.println("    \"extractionDate\": \"" + java.time.LocalDateTime.now() + "\",");
            writer.println("    \"originalSize\": " + effect.size);
            writer.println("  },");

            writer.println("  \"effect\": {");
            writer.println("    \"type\": \"" + effect.effectType + "\",");
            writer.println("    \"frameCount\": " + effect.frameCount + ",");
            writer.println("    \"coordinateCount\": " + effect.coordinateCount + ",");

            if (effect.boundingBox != null) {
                writer.println("    \"boundingBox\": {");
                writer.println("      \"x\": " + effect.boundingBox.minX + ",");
                writer.println("      \"y\": " + effect.boundingBox.minY + ",");
                writer.println("      \"width\": " + effect.boundingBox.width + ",");
                writer.println("      \"height\": " + effect.boundingBox.height);
                writer.println("    },");
            }

            writer.println("    \"frames\": [");
            for (int i = 0; i < effect.frames.size(); i++) {
                EffectFrame frame = effect.frames.get(i);
                writer.println("      {");
                writer.println("        \"frameId\": " + frame.frameId + ",");
                writer.println("        \"type\": " + frame.type + ",");
                writer.println("        \"x\": " + frame.x + ",");
                writer.println("        \"y\": " + frame.y);
                writer.print("      }");
                if (i < effect.frames.size() - 1) writer.print(",");
                writer.println();
            }
            writer.println("    ],");

            writer.println("    \"coordinates\": [");
            for (int i = 0; i < Math.min(effect.coordinates.size(), 100); i++) { // Limitiere auf 100 für JSON-Lesbarkeit
                EffectCoordinate coord = effect.coordinates.get(i);
                writer.print("      {\"x\": " + coord.x + ", \"y\": " + coord.y + "}");
                if (i < Math.min(effect.coordinates.size(), 100) - 1) writer.print(",");
                writer.println();
            }
            if (effect.coordinates.size() > 100) {
                writer.println("      // ... weitere " + (effect.coordinates.size() - 100) + " Koordinaten (gekürzt)");
            }
            writer.println("    ]");

            writer.println("  },");

            writer.println("  \"modernFormat\": {");
            writer.println("    \"recommendedEngine\": \"Unity/Unreal\",");
            writer.println("    \"animationType\": \"" + getModernAnimationType(effect.effectType) + "\",");
            writer.println("    \"usage\": \"" + getUsageRecommendation(effect.effectType) + "\"");
            writer.println("  }");

            writer.println("}");
        }
    }

    private static void createDetailedAnalysis(PlayStationEffect effect, byte[] data, Path outputPath, String baseName) throws IOException {
        Path analysisFile = outputPath.resolve(baseName + "_analysis.txt");

        try (PrintWriter writer = new PrintWriter(Files.newBufferedWriter(analysisFile))) {
            writer.println("RESIDENT EVIL 1.5 EFFEKT-ANALYSE");
            writer.println("======================================================");
            writer.println("Datei: " + baseName + ".eff");
            writer.println("Größe: " + data.length + " bytes");
            writer.println("Typ: " + effect.effectType);
            writer.println();

            writer.println("FRAME-STRUKTUR:");
            writer.println("Anzahl Frames: " + effect.frameCount);
            for (EffectFrame frame : effect.frames) {
                writer.printf("  Frame %d: Typ=%d, Pos=(%d,%d)\n",
                    frame.frameId, frame.type, frame.x, frame.y);
            }
            writer.println();

            writer.println("KOORDINATEN-ANALYSE:");
            writer.println("Anzahl Koordinaten: " + effect.coordinateCount);
            if (effect.boundingBox != null) {
                writer.printf("Bounding Box: (%d,%d) bis (%d,%d) [%dx%d]\n",
                    effect.boundingBox.minX, effect.boundingBox.minY,
                    effect.boundingBox.maxX, effect.boundingBox.maxY,
                    effect.boundingBox.width, effect.boundingBox.height);
            }
            writer.println();

            writer.println("MODERNE VERWENDUNG:");
            writer.println("Engine: " + getModernAnimationType(effect.effectType));
            writer.println("Typ: " + getUsageRecommendation(effect.effectType));
            writer.println();

            writer.println("HEX-DUMP (erste 128 bytes):");
            for (int i = 0; i < Math.min(128, data.length); i += 16) {
                writer.printf("%04X: ", i);
                for (int j = 0; j < 16 && (i + j) < data.length; j++) {
                    writer.printf("%02X ", data[i + j] & 0xFF);
                }
                writer.println();
            }
        }
    }

    private static String getModernAnimationType(String effectType) {
        switch (effectType) {
            case "PARTICLE_SYSTEM": return "Particle System (Unity Shuriken/Unreal Niagara)";
            case "SPRITE_ANIMATION": return "Sprite Animation (2D Flipbook)";
            case "COMPLEX_ANIMATION": return "Keyframe Animation";
            default: return "Static Sprite";
        }
    }

    private static String getUsageRecommendation(String effectType) {
        switch (effectType) {
            case "PARTICLE_SYSTEM": return "Umgebungseffekte, Explosionen, Rauch";
            case "SPRITE_ANIMATION": return "UI-Effekte, kleine Animationen";
            case "COMPLEX_ANIMATION": return "Komplexe Szenen-Effekte";
            default: return "Statische Dekorationen";
        }
    }

    // Hilfsmethoden für Byte-Reading
    private static int readUInt16(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;
        return ((data[offset + 1] & 0xFF) << 8) | (data[offset] & 0xFF);
    }

    private static int readSInt16(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;
        int value = ((data[offset + 1] & 0xFF) << 8) | (data[offset] & 0xFF);
        return (value > 32767) ? value - 65536 : value; // Konvertiere zu signed
    }

    // Datenstrukturen
    static class PlayStationEffect {
        String name;
        int size;
        String effectType;
        int frameCount;
        int coordinateCount;
        List<EffectFrame> frames = new ArrayList<>();
        List<EffectCoordinate> coordinates = new ArrayList<>();
        BoundingBox boundingBox;
    }

    static class EffectFrame {
        int frameId;
        int type;
        int x;
        int y;
    }

    static class EffectCoordinate {
        int x;
        int y;
    }

    static class BoundingBox {
        int minX, maxX, minY, maxY;
        int width, height;
    }
}
