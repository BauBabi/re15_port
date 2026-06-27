package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Vollständige SCD-Hex-Analyse zur korrekten Werte-Extraktion
 */
public class SCDHexAnalysis {

    public static void main(String[] args) throws IOException {
        analyzeSCDFile("info/Re1.5/PSX/DATA/ROOM115U.SCD");
    }

    public static void analyzeSCDFile(String filePath) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get(filePath));

        System.out.println("=== VOLLSTÄNDIGE SCD HEX-ANALYSE ===");
        System.out.println("Datei: " + filePath);
        System.out.println("Größe: " + data.length + " bytes");
        System.out.println();

        // Zeige die ersten 100 Bytes komplett
        System.out.println("Erste 100 Bytes (Hex + ASCII):");
        for (int i = 0; i < Math.min(100, data.length); i += 16) {
            System.out.printf("%08X: ", i);

            // Hex-Darstellung
            StringBuilder hex = new StringBuilder();
            StringBuilder ascii = new StringBuilder();
            for (int j = 0; j < 16 && i + j < data.length && i + j < 100; j++) {
                int b = data[i + j] & 0xFF;
                hex.append(String.format("%02X ", b));
                ascii.append((b >= 32 && b <= 126) ? (char) b : '.');
            }

            System.out.printf("%-48s %s\n", hex.toString(), ascii.toString());
        }

        // Finde alle 0x44 Opcodes
        System.out.println("\n=== SUCHE NACH SCE_EM_SET OPCODES (0x44) ===");
        for (int i = 0; i < data.length; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                System.out.println("\nSce_em_set (0x44) gefunden bei Offset " + i + ":");
                analyzeOpcode44(data, i);
            }
        }

        // Zusätzlich: Suche nach den Referenz-Werten in den Bytes
        System.out.println("\n=== SUCHE NACH REFERENZ-WERTEN ===");
        searchForReferenceValues(data);
    }

    private static void analyzeOpcode44(byte[] data, int offset) {
        System.out.println("Rohe Bytes ab Offset " + offset + ":");

        // Zeige 30 Bytes ab dem Opcode
        for (int i = 0; i < 30 && offset + i < data.length; i++) {
            int b = data[offset + i] & 0xFF;
            System.out.printf("  [%2d] = 0x%02X (%3d) %s\n",
                i, b, b, (i == 0) ? "← Opcode 0x44" : "");
        }

        System.out.println("\nByte-Parameter (1-6):");
        for (int i = 1; i <= 6 && offset + i < data.length; i++) {
            System.out.printf("  Parameter %d = %d (0x%02X)\n",
                i, data[offset + i] & 0xFF, data[offset + i] & 0xFF);
        }

        System.out.println("\nTeste verschiedene Short-Interpretationen:");

        // Teste Little-Endian signed shorts an verschiedenen Positionen
        for (int startPos = 7; startPos <= 10; startPos++) {
            System.out.printf("\nAb Position %d (Little-Endian signed):\n", startPos);
            for (int i = 0; i < 6; i++) {
                int pos = offset + startPos + i * 2;
                if (pos + 1 < data.length) {
                    int value = readLittleEndianSigned(data, pos);
                    System.out.printf("  Short %d = %6d (Bytes: 0x%02X 0x%02X)\n",
                        i, value, data[pos] & 0xFF, data[pos + 1] & 0xFF);
                }
            }
        }

        // Teste Big-Endian signed shorts
        System.out.printf("\nAb Position 8 (Big-Endian signed):\n");
        for (int i = 0; i < 6; i++) {
            int pos = offset + 8 + i * 2;
            if (pos + 1 < data.length) {
                int value = readBigEndianSigned(data, pos);
                System.out.printf("  Short %d = %6d (Bytes: 0x%02X 0x%02X)\n",
                    i, value, data[pos] & 0xFF, data[pos + 1] & 0xFF);
            }
        }
    }

    private static void searchForReferenceValues(byte[] data) {
        int[] referenceValues = {1537, 19456, 768, -24576, -25856, 20480};
        String[] names = {"1537", "19456", "768", "-24576", "-25856", "20480"};

        for (int r = 0; r < referenceValues.length; r++) {
            int target = referenceValues[r];
            System.out.printf("\nSuche nach Wert %s (%d):\n", names[r], target);

            boolean found = false;
            for (int i = 0; i < data.length - 1; i++) {
                int littleEndian = readLittleEndianSigned(data, i);
                int bigEndian = readBigEndianSigned(data, i);

                if (littleEndian == target) {
                    System.out.printf("  ✓ GEFUNDEN als Little-Endian bei Offset %d (Bytes: 0x%02X 0x%02X)\n",
                        i, data[i] & 0xFF, data[i + 1] & 0xFF);
                    found = true;
                }
                if (bigEndian == target) {
                    System.out.printf("  ✓ GEFUNDEN als Big-Endian bei Offset %d (Bytes: 0x%02X 0x%02X)\n",
                        i, data[i] & 0xFF, data[i + 1] & 0xFF);
                    found = true;
                }
            }
            if (!found) {
                System.out.println("  ✗ NICHT GEFUNDEN");
            }
        }
    }

    private static int readLittleEndianSigned(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;
        int value = (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
        return value > 32767 ? value - 65536 : value;
    }

    private static int readBigEndianSigned(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;
        int value = ((data[offset] & 0xFF) << 8) | (data[offset + 1] & 0xFF);
        return value > 32767 ? value - 65536 : value;
    }
}
