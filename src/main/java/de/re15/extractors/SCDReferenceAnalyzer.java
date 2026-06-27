package de.re15.extractors;

import java.io.*;

public class SCDReferenceAnalyzer {
    public static void main(String[] args) throws IOException {
        String filePath = "info/Re1.5/PSX/DATA/ROOM115U.SCD";

        try (FileInputStream fis = new FileInputStream(filePath)) {
            byte[] data = fis.readAllBytes();

            System.out.println("=== REVERSE ENGINEERING FROM REFERENCE ===");
            System.out.println("Target: Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480)");
            System.out.println();

            // Finde den Sce_em_set Opcode
            for (int i = 0; i < data.length; i++) {
                if ((data[i] & 0xFF) == 0x44) {
                    System.out.println("Raw hex at Sce_em_set:");
                    for (int j = 0; j < 32 && i + j < data.length; j++) {
                        System.out.printf("%02X ", data[i + j] & 0xFF);
                        if ((j + 1) % 8 == 0) System.out.println();
                    }
                    System.out.println();

                    // Reverse Engineering: Wo müssen die Referenzwerte stehen?
                    System.out.println("=== REVERSE MAPPING ===");
                    findExactMapping(data, i);
                    break;
                }
            }
        }
    }

    private static void findExactMapping(byte[] data, int offset) {
        // Konvertiere Referenzwerte zu Bytes
        int[] targetValues = {0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480};

        System.out.println("Mapping reference values to byte positions:");

        // Parameter 1-7 sind definitiv einzelne Bytes
        for (int i = 1; i <= 7; i++) {
            int actual = data[offset + i] & 0xFF;
            int expected = targetValues[i];
            String status = (actual == expected) ? "✓" : "✗";
            System.out.printf("Param %d: offset %d = 0x%02X (%d) expected %d %s\n",
                i, offset + i, actual, actual, expected, status);
        }

        System.out.println("\nSearching for 16-bit values:");

        // Suche nach 1537 (Parameter 8)
        searchForSpecificValue(data, offset, 1537, "Parameter 8");

        // Suche nach 19456 (Parameter 9)
        searchForSpecificValue(data, offset, 19456, "Parameter 9");

        // Suche nach 768 (Parameter 10)
        searchForSpecificValue(data, offset, 768, "Parameter 10");

        // Suche nach -24576 (Parameter 11)
        searchForSpecificSignedValue(data, offset, -24576, "Parameter 11");

        // Suche nach -25856 (Parameter 12)
        searchForSpecificSignedValue(data, offset, -25856, "Parameter 12");

        // Suche nach 20480 (Parameter 13)
        searchForSpecificValue(data, offset, 20480, "Parameter 13");

        System.out.println("\n=== PROPOSED STRUCTURE ===");
        proposeBestStructure(data, offset);
    }

    private static void searchForSpecificValue(byte[] data, int offset, int target, String name) {
        for (int i = 8; i < 30; i++) {
            if (offset + i + 1 < data.length) {
                // Little-Endian
                int le = (data[offset + i] & 0xFF) | ((data[offset + i + 1] & 0xFF) << 8);
                if (le == target) {
                    System.out.printf("%s (%d): FOUND at offset %d-%d as Little-Endian: 0x%02X 0x%02X\n",
                        name, target, offset + i, offset + i + 1, data[offset + i] & 0xFF, data[offset + i + 1] & 0xFF);
                }

                // Big-Endian
                int be = ((data[offset + i] & 0xFF) << 8) | (data[offset + i + 1] & 0xFF);
                if (be == target) {
                    System.out.printf("%s (%d): FOUND at offset %d-%d as Big-Endian: 0x%02X 0x%02X\n",
                        name, target, offset + i, offset + i + 1, data[offset + i] & 0xFF, data[offset + i + 1] & 0xFF);
                }
            }
        }
    }

    private static void searchForSpecificSignedValue(byte[] data, int offset, int target, String name) {
        for (int i = 8; i < 30; i++) {
            if (offset + i + 1 < data.length) {
                // Little-Endian signed
                int le = (data[offset + i] & 0xFF) | ((data[offset + i + 1] & 0xFF) << 8);
                int leSigned = le > 32767 ? le - 65536 : le;
                if (leSigned == target) {
                    System.out.printf("%s (%d): FOUND at offset %d-%d as Little-Endian signed: 0x%02X 0x%02X\n",
                        name, target, offset + i, offset + i + 1, data[offset + i] & 0xFF, data[offset + i + 1] & 0xFF);
                }
            }
        }
    }

    private static void proposeBestStructure(byte[] data, int offset) {
        System.out.println("Based on analysis, the correct structure should be:");
        System.out.println("// First 7 parameters as bytes:");
        for (int i = 1; i <= 7; i++) {
            System.out.printf("param%d = data[offset + %d] = %d\n", i, i, data[offset + i] & 0xFF);
        }

        System.out.println("\n// Remaining parameters as 16-bit values:");
        System.out.println("// Need to find the exact pattern where all target values align correctly");

        // Teste verschiedene Kombinationen
        testStructure(data, offset, new int[]{8, 10, 12, 14, 16, 18}, "Structure A");
        testStructure(data, offset, new int[]{9, 11, 13, 15, 17, 19}, "Structure B");
        testStructure(data, offset, new int[]{8, 11, 13, 15, 17, 19}, "Structure C (mixed)");
    }

    private static void testStructure(byte[] data, int offset, int[] offsets, String name) {
        System.out.println("\n" + name + ":");
        int[] targets = {1537, 19456, 768, -24576, -25856, 20480};

        for (int i = 0; i < offsets.length && i < targets.length; i++) {
            if (offset + offsets[i] + 1 < data.length) {
                int le = (data[offset + offsets[i]] & 0xFF) | ((data[offset + offsets[i] + 1] & 0xFF) << 8);
                int leSigned = le > 32767 ? le - 65536 : le;
                int be = ((data[offset + offsets[i]] & 0xFF) << 8) | (data[offset + offsets[i] + 1] & 0xFF);

                String match = "";
                if (le == targets[i]) match = " ← LE MATCH!";
                else if (leSigned == targets[i]) match = " ← LE SIGNED MATCH!";
                else if (be == targets[i]) match = " ← BE MATCH!";

                System.out.printf("  Offset %d-%d: LE=%d, LE_SIGNED=%d, BE=%d (target=%d)%s\n",
                    offsets[i], offsets[i] + 1, le, leSigned, be, targets[i], match);
            }
        }
    }
}
