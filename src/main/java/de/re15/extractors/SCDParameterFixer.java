package de.re15.extractors;

import java.io.*;

public class SCDParameterFixer {
    public static void main(String[] args) throws IOException {
        String filePath = "info/Re1.5/PSX/DATA/ROOM115U.SCD";

        try (FileInputStream fis = new FileInputStream(filePath)) {
            byte[] data = fis.readAllBytes();

            System.out.println("=== SYSTEMATIC SCD PARAMETER ANALYSIS ===");
            System.out.println("Target: Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480)");
            System.out.println();

            // Finde den Sce_em_set Opcode (0x44)
            for (int i = 0; i < data.length; i++) {
                if ((data[i] & 0xFF) == 0x44) {
                    System.out.println("Found Sce_em_set at offset " + i);
                    System.out.println("Raw hex (32 bytes):");

                    // Zeige rohe Hex-Daten
                    for (int j = 0; j < 32 && i + j < data.length; j++) {
                        System.out.printf("%02X ", data[i + j] & 0xFF);
                        if ((j + 1) % 8 == 0) System.out.println();
                    }
                    System.out.println();

                    // Systematische Analyse verschiedener Interpretationen
                    analyzeAsAllShorts(data, i);
                    analyzeAsMixedStructure(data, i);
                    findCorrectMapping(data, i);

                    break;
                }
            }
        }
    }

    private static void analyzeAsAllShorts(byte[] data, int offset) {
        System.out.println("=== ANALYSIS 1: All as 16-bit Little-Endian from offset 1 ===");
        for (int j = 0; j < 15; j++) {
            int pos = offset + 1 + j * 2;
            if (pos + 1 < data.length) {
                int value = (data[pos] & 0xFF) | ((data[pos + 1] & 0xFF) << 8);
                int signed = value > 32767 ? value - 65536 : value;
                System.out.printf("Position %d (offset %d-%d): %d (signed: %d) [0x%02X 0x%02X]\n",
                    j + 1, pos, pos + 1, value, signed, data[pos] & 0xFF, data[pos + 1] & 0xFF);
            }
        }
        System.out.println();
    }

    private static void analyzeAsMixedStructure(byte[] data, int offset) {
        System.out.println("=== ANALYSIS 2: Mixed structure (7 bytes + remaining as shorts) ===");

        // Erste 7 als Bytes
        for (int j = 1; j <= 7; j++) {
            if (offset + j < data.length) {
                int value = data[offset + j] & 0xFF;
                System.out.printf("Byte param %d (offset %d): %d (0x%02X)\n", j, offset + j, value, value);
            }
        }

        // Rest als 16-bit Werte
        for (int j = 0; j < 12; j++) {
            int pos = offset + 8 + j * 2;
            if (pos + 1 < data.length) {
                int value = (data[pos] & 0xFF) | ((data[pos + 1] & 0xFF) << 8);
                int signed = value > 32767 ? value - 65536 : value;
                System.out.printf("Short param %d (offset %d-%d): %d (signed: %d)\n",
                    8 + j, pos, pos + 1, value, signed);
            }
        }
        System.out.println();
    }

    private static void findCorrectMapping(byte[] data, int offset) {
        System.out.println("=== ANALYSIS 3: Finding correct mapping for target values ===");

        // Zielwerte aus der Referenz
        int[] targets = {0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480};

        System.out.println("Searching for target values in different interpretations:");

        // Suche nach 1537 (Param 8)
        searchForValue(data, offset, 1537, "1537 (target param 8)");

        // Suche nach 19456 (Param 9)
        searchForValue(data, offset, 19456, "19456 (target param 9)");

        // Suche nach 768 (Param 10)
        searchForValue(data, offset, 768, "768 (target param 10)");

        // Suche nach -24576 (Param 11)
        searchForSignedValue(data, offset, -24576, "-24576 (target param 11)");

        // Suche nach -25856 (Param 12)
        searchForSignedValue(data, offset, -25856, "-25856 (target param 12)");

        // Suche nach 20480 (Param 13)
        searchForValue(data, offset, 20480, "20480 (target param 13)");
    }

    private static void searchForValue(byte[] data, int offset, int target, String description) {
        System.out.println("Searching for " + description + ":");

        // Suche als Little-Endian 16-bit Wert
        for (int i = 1; i < 30; i++) {
            if (offset + i + 1 < data.length) {
                int value = (data[offset + i] & 0xFF) | ((data[offset + i + 1] & 0xFF) << 8);
                if (value == target) {
                    System.out.printf("  FOUND at offset %d-%d as Little-Endian: 0x%02X 0x%02X = %d\n",
                        offset + i, offset + i + 1, data[offset + i] & 0xFF, data[offset + i + 1] & 0xFF, value);
                }
            }
        }

        // Suche als Big-Endian 16-bit Wert
        for (int i = 1; i < 30; i++) {
            if (offset + i + 1 < data.length) {
                int value = (data[offset + i + 1] & 0xFF) | ((data[offset + i] & 0xFF) << 8);
                if (value == target) {
                    System.out.printf("  FOUND at offset %d-%d as Big-Endian: 0x%02X 0x%02X = %d\n",
                        offset + i, offset + i + 1, data[offset + i] & 0xFF, data[offset + i + 1] & 0xFF, value);
                }
            }
        }
    }

    private static void searchForSignedValue(byte[] data, int offset, int target, String description) {
        System.out.println("Searching for " + description + ":");

        // Suche als Little-Endian 16-bit signed Wert
        for (int i = 1; i < 30; i++) {
            if (offset + i + 1 < data.length) {
                int value = (data[offset + i] & 0xFF) | ((data[offset + i + 1] & 0xFF) << 8);
                int signed = value > 32767 ? value - 65536 : value;
                if (signed == target) {
                    System.out.printf("  FOUND at offset %d-%d as Little-Endian signed: 0x%02X 0x%02X = %d (unsigned: %d)\n",
                        offset + i, offset + i + 1, data[offset + i] & 0xFF, data[offset + i + 1] & 0xFF, signed, value);
                }
            }
        }
    }
}
