package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Direkte SCD Hex-Analyse zur Ermittlung der korrekten Sce_em_set Parameter
 */
public class SCDDirectAnalyzer {

    public static void main(String[] args) {
        try {
            analyzeRoomSCD("info/Re1.5/PSX/DATA/ROOM115U.SCD");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void analyzeRoomSCD(String filePath) throws IOException {
        System.out.println("=== DIREKTE SCD HEX-ANALYSE ===");
        System.out.println("Datei: " + filePath);

        byte[] data = Files.readAllBytes(Paths.get(filePath));
        System.out.println("Dateigröße: " + data.length + " bytes");

        // Suche nach Sce_em_set Opcode (0x44)
        for (int i = 0; i < data.length - 32; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                System.out.println("\nSce_em_set Opcode 0x44 gefunden bei Offset " + i + ":");

                // Zeige die nächsten 32 Bytes im Hex-Format
                System.out.print("Hex-Bytes: ");
                for (int j = 0; j < 32 && (i + j) < data.length; j++) {
                    System.out.printf("%02X ", data[i + j] & 0xFF);
                    if (j == 7 || j == 15 || j == 23) System.out.print("| ");
                }
                System.out.println();

                // Analysiere Parameter 1-7 (einzelne Bytes)
                System.out.println("\nParameter 1-7 (einzelne Bytes):");
                for (int j = 1; j <= 7; j++) {
                    if (i + j < data.length) {
                        System.out.printf("  param%d: %d (0x%02X)\n", j, data[i + j] & 0xFF, data[i + j] & 0xFF);
                    }
                }

                // Analysiere Parameter 8-13 (verschiedene Short-Interpretationen)
                System.out.println("\nParameter 8-13 (Short-Analyse):");

                for (int paramNum = 8, offset = 8; paramNum <= 13; paramNum++, offset += 2) {
                    if (i + offset + 1 < data.length) {
                        int byte1 = data[i + offset] & 0xFF;
                        int byte2 = data[i + offset + 1] & 0xFF;

                        int littleEndian = byte1 | (byte2 << 8);
                        int bigEndian = (byte1 << 8) | byte2;
                        int signedLE = littleEndian > 32767 ? littleEndian - 65536 : littleEndian;
                        int signedBE = bigEndian > 32767 ? bigEndian - 65536 : bigEndian;

                        System.out.printf("  param%d: Bytes(%d,%d) -> LE=%d, BE=%d, SignedLE=%d, SignedBE=%d\n",
                            paramNum, byte1, byte2, littleEndian, bigEndian, signedLE, signedBE);
                    }
                }

                // Vergleiche mit Referenz-Werten
                System.out.println("\nReferenz-Vergleich:");
                System.out.println("Referenz: Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480);");

                // Teste, welche Interpretation zu den Referenz-Werten passt
                int[] referenceValues = {1537, 19456, 768, -24576, -25856, 20480};

                for (int paramNum = 8, offset = 8, refIndex = 0; paramNum <= 13; paramNum++, offset += 2, refIndex++) {
                    if (i + offset + 1 < data.length && refIndex < referenceValues.length) {
                        int byte1 = data[i + offset] & 0xFF;
                        int byte2 = data[i + offset + 1] & 0xFF;

                        int littleEndian = byte1 | (byte2 << 8);
                        int bigEndian = (byte1 << 8) | byte2;
                        int signedLE = littleEndian > 32767 ? littleEndian - 65536 : littleEndian;
                        int signedBE = bigEndian > 32767 ? bigEndian - 65536 : bigEndian;

                        int expectedValue = referenceValues[refIndex];

                        System.out.printf("  param%d (Referenz: %d):\n", paramNum, expectedValue);
                        if (littleEndian == expectedValue) System.out.println("    ✅ Little-Endian passt!");
                        if (bigEndian == expectedValue) System.out.println("    ✅ Big-Endian passt!");
                        if (signedLE == expectedValue) System.out.println("    ✅ Signed Little-Endian passt!");
                        if (signedBE == expectedValue) System.out.println("    ✅ Signed Big-Endian passt!");

                        if (littleEndian != expectedValue && bigEndian != expectedValue &&
                            signedLE != expectedValue && signedBE != expectedValue) {
                            System.out.println("    ❌ Keine Standard-Interpretation passt!");

                            // Versuche umgekehrte Byte-Reihenfolge
                            int reversedLE = byte2 | (byte1 << 8);
                            int reversedSignedLE = reversedLE > 32767 ? reversedLE - 65536 : reversedLE;
                            System.out.printf("    Reversed LE: %d, Reversed Signed LE: %d\n", reversedLE, reversedSignedLE);
                            if (reversedLE == expectedValue) System.out.println("    ✅ Umgekehrte Little-Endian passt!");
                            if (reversedSignedLE == expectedValue) System.out.println("    ✅ Umgekehrte Signed Little-Endian passt!");
                        }
                    }
                }

                break; // Nur den ersten Sce_em_set analysieren
            }
        }
    }
}
