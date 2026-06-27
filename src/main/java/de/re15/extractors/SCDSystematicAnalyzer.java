package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Systematische SCD-Datei Analyse zur korrekten Parameter-Ermittlung
 */
public class SCDSystematicAnalyzer {

    public static void main(String[] args) {
        try {
            // Analysiere beide SCD-Dateien
            System.out.println("=== SYSTEMATISCHE SCD-ANALYSE ===");
            analyzeSCDFile("info/Re1.5/PSX/DATA/ROOM115U.SCD");
            System.out.println("\n" + "=".repeat(60) + "\n");
            analyzeSCDFile("info/Re1.5/PSX/DATA/ROOM506U.SCD");

            // Vergleiche mit der Referenz
            System.out.println("\n=== REFERENZ-VERGLEICH ===");
            compareWithReference();

        } catch (Exception e) {
            System.err.println("Fehler: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void analyzeSCDFile(String filePath) throws IOException {
        System.out.println("Analysiere: " + filePath);

        byte[] data = Files.readAllBytes(Paths.get(filePath));
        System.out.println("Dateigröße: " + data.length + " bytes");

        // Erstelle vollständigen Hex-Dump der ersten 100 Bytes
        System.out.println("\nHex-Dump (erste 100 Bytes):");
        for (int i = 0; i < Math.min(100, data.length); i += 16) {
            System.out.printf("%04X: ", i);

            // Hex-Ausgabe
            for (int j = 0; j < 16; j++) {
                if (i + j < data.length) {
                    System.out.printf("%02X ", data[i + j] & 0xFF);
                } else {
                    System.out.print("   ");
                }
            }

            System.out.print(" | ");

            // ASCII-Ausgabe
            for (int j = 0; j < 16; j++) {
                if (i + j < data.length) {
                    int b = data[i + j] & 0xFF;
                    if (b >= 32 && b <= 126) {
                        System.out.print((char) b);
                    } else {
                        System.out.print(".");
                    }
                } else {
                    System.out.print(" ");
                }
            }
            System.out.println();
        }

        // Suche nach Sce_em_set Opcodes
        System.out.println("\nSuche nach Sce_em_set Opcodes (0x44):");
        for (int i = 0; i < data.length - 20; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                System.out.printf("\nSce_em_set gefunden bei Offset 0x%04X (%d):\n", i, i);

                // Zeige die komplette 20-Byte Struktur
                System.out.print("  Raw Bytes: ");
                for (int j = 0; j < 20 && (i + j) < data.length; j++) {
                    System.out.printf("%02X ", data[i + j] & 0xFF);
                }
                System.out.println();

                // Analysiere jeden Parameter
                System.out.println("  Parameter-Analyse:");
                System.out.printf("    param1-7: %d, %d, %d, %d, %d, %d, %d\n",
                    data[i + 1] & 0xFF, data[i + 2] & 0xFF, data[i + 3] & 0xFF,
                    data[i + 4] & 0xFF, data[i + 5] & 0xFF, data[i + 6] & 0xFF,
                    data[i + 7] & 0xFF);

                // Teste verschiedene Short-Interpretationen für Parameter 8-13
                System.out.println("    Short-Parameter (8-13):");
                for (int paramNum = 8, offset = 8; paramNum <= 13; paramNum++, offset += 2) {
                    if (i + offset + 1 < data.length) {
                        int b1 = data[i + offset] & 0xFF;
                        int b2 = data[i + offset + 1] & 0xFF;

                        int le = b1 | (b2 << 8);
                        int be = (b1 << 8) | b2;
                        int signedLE = le > 32767 ? le - 65536 : le;
                        int signedBE = be > 32767 ? be - 65536 : be;

                        System.out.printf("      param%d [%02X %02X]: LE=%d, BE=%d, SLE=%d, SBE=%d\n",
                            paramNum, b1, b2, le, be, signedLE, signedBE);
                    }
                }
            }
        }
    }

    private static void compareWithReference() {
        System.out.println("Referenz aus already_extracted/PSX/DATA/ROOM115U.c:");
        System.out.println("Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480);");
        System.out.println();

        System.out.println("Soll-Werte für Parameter 8-13:");
        int[] targetValues = {1537, 19456, 768, -24576, -25856, 20480};
        for (int i = 0; i < targetValues.length; i++) {
            int value = targetValues[i];
            System.out.printf("  param%d = %d (0x%04X)\n", i + 8, value, value & 0xFFFF);

            // Zeige, wie der Wert als Bytes aussehen würde
            if (value >= 0) {
                int leByte1 = value & 0xFF;
                int leByte2 = (value >> 8) & 0xFF;
                int beByte1 = (value >> 8) & 0xFF;
                int beByte2 = value & 0xFF;
                System.out.printf("    Als LE-Bytes: %02X %02X (%d, %d)\n", leByte1, leByte2, leByte1, leByte2);
                System.out.printf("    Als BE-Bytes: %02X %02X (%d, %d)\n", beByte1, beByte2, beByte1, beByte2);
            } else {
                int unsignedValue = value + 65536;
                int leByte1 = unsignedValue & 0xFF;
                int leByte2 = (unsignedValue >> 8) & 0xFF;
                System.out.printf("    Als LE-Bytes (unsigned): %02X %02X (%d, %d)\n", leByte1, leByte2, leByte1, leByte2);
            }
        }
    }
}
