package de.re15.extractors;

import java.io.*;
import java.nio.file.*;
import java.util.*;

/**
 * SCD Script Extractor - Extrahiert Script-Befehle aus SCD-Dateien
 * Basiert auf korrekter Hex-Analyse der rohen Bytes
 */
public class SCDExtractor {

    public static void main(String[] args) throws IOException {
        String sourceDir = "info/Re1.5/PSX/DATA";
        String targetDir = "extracted/PSX/DATA";

        extractAllSCDFiles(sourceDir, targetDir);
    }

    public static void extractAllSCDFiles(String sourceDir, String targetDir) throws IOException {
        File sourceDirFile = new File(sourceDir);
        File targetDirFile = new File(targetDir);

        if (!targetDirFile.exists()) {
            targetDirFile.mkdirs();
        }

        File[] scdFiles = sourceDirFile.listFiles((dir, name) -> name.endsWith(".SCD"));
        if (scdFiles != null) {
            for (File scdFile : scdFiles) {
                System.out.println("Extrahiere SCD-Datei: " + scdFile.getName());
                extractSCDFile(scdFile.getAbsolutePath(),
                              new File(targetDir, scdFile.getName().replace(".SCD", ".txt")).getAbsolutePath());
            }
        }
    }

    public static void extractSCDFile(String inputPath, String outputPath) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get(inputPath));
        List<String> extractedCommands = new ArrayList<>();

        System.out.println("Analysiere SCD-Datei: " + inputPath);
        System.out.println("Dateigröße: " + data.length + " bytes");

        // Suche nach allen Script-Befehlen
        for (int i = 0; i < data.length; i++) {
            int opcode = data[i] & 0xFF;

            switch (opcode) {
                case 0x44: // Sce_em_set
                    String emSetCommand = extractSceEmSet(data, i);
                    if (emSetCommand != null) {
                        extractedCommands.add(String.format("Offset %d: %s", i, emSetCommand));
                        System.out.println("Gefunden: " + emSetCommand);
                    }
                    break;

                case 0x01: // Sce_em_control
                    String emControlCommand = extractSceEmControl(data, i);
                    if (emControlCommand != null) {
                        extractedCommands.add(String.format("Offset %d: %s", i, emControlCommand));
                    }
                    break;

                // Weitere Opcodes können hier hinzugefügt werden
            }
        }

        // Schreibe extrahierte Befehle in Ausgabedatei
        try (PrintWriter writer = new PrintWriter(new FileWriter(outputPath))) {
            writer.println("=== SCD Script Extraktion ===");
            writer.println("Quelldatei: " + inputPath);
            writer.println("Extrahiert am: " + new Date());
            writer.println("Anzahl gefundener Befehle: " + extractedCommands.size());
            writer.println();

            for (String command : extractedCommands) {
                writer.println(command);
            }
        }

        System.out.println("Extraktion abgeschlossen. " + extractedCommands.size() + " Befehle gefunden.");
        System.out.println("Ausgabe gespeichert in: " + outputPath);
    }

    /**
     * Extrahiert Sce_em_set Parameter aus den rohen Bytes
     * Korrekte Struktur: 7 Byte-Parameter + 6 Short-Parameter (Little-Endian)
     */
    private static String extractSceEmSet(byte[] data, int offset) {
        if (offset + 19 >= data.length) { // Mindestens 20 Bytes benötigt
            return null;
        }

        try {
            // Erste 7 Parameter sind einzelne Bytes
            int param1 = data[offset + 1] & 0xFF;
            int param2 = data[offset + 2] & 0xFF;
            int param3 = data[offset + 3] & 0xFF;
            int param4 = data[offset + 4] & 0xFF;
            int param5 = data[offset + 5] & 0xFF;
            int param6 = data[offset + 6] & 0xFF;
            int param7 = data[offset + 7] & 0xFF;

            // Letzten 6 Parameter sind 16-bit Little-Endian signed Werte
            int param8 = readLittleEndianSigned(data, offset + 8);
            int param9 = readLittleEndianSigned(data, offset + 10);
            int param10 = readLittleEndianSigned(data, offset + 12);
            int param11 = readLittleEndianSigned(data, offset + 14);
            int param12 = readLittleEndianSigned(data, offset + 16);
            int param13 = readLittleEndianSigned(data, offset + 18);

            return String.format("Sce_em_set(%d, 0x%02X, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                param1, param2, param3, param4, param5, param6, param7,
                param8, param9, param10, param11, param12, param13);

        } catch (Exception e) {
            System.err.println("Fehler beim Extrahieren von Sce_em_set bei Offset " + offset + ": " + e.getMessage());
            return null;
        }
    }

    /**
     * Extrahiert Sce_em_control Parameter (Beispiel für weiteren Opcode)
     */
    private static String extractSceEmControl(byte[] data, int offset) {
        if (offset + 3 >= data.length) {
            return null;
        }

        try {
            int param1 = data[offset + 1] & 0xFF;
            int param2 = data[offset + 2] & 0xFF;
            int param3 = data[offset + 3] & 0xFF;

            return String.format("Sce_em_control(%d, %d, %d)", param1, param2, param3);
        } catch (Exception e) {
            return null;
        }
    }

    /**
     * Liest einen 16-bit Little-Endian signed Wert aus den Bytes
     */
    private static int readLittleEndianSigned(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;

        int value = (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
        return value > 32767 ? value - 65536 : value;
    }

    /**
     * Liest einen 16-bit Little-Endian unsigned Wert aus den Bytes
     */
    private static int readLittleEndianUnsigned(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;

        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    /**
     * Debug-Methode: Zeigt rohe Hex-Daten um einen Offset herum
     */
    private static void debugHexData(byte[] data, int offset, int length) {
        System.out.println("Hex-Daten bei Offset " + offset + ":");
        for (int i = 0; i < length && offset + i < data.length; i++) {
            System.out.printf("  [%2d] = 0x%02X (%3d)\n",
                i, data[offset + i] & 0xFF, data[offset + i] & 0xFF);
        }
    }
}
