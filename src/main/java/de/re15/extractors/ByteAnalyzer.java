package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Byte-Analyse für Sce_em_set Parameter-Korrektur
 */
public class ByteAnalyzer {

    public static void main(String[] args) {
        try {
            analyzeConversion();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void analyzeConversion() {
        System.out.println("=== BYTE-ZU-SHORT KONVERTIERUNGSANALYSE ===");

        // Aktuelle falsche Werte: 3, 160, 155, 80
        // Gewünschte Werte: 768, -24576, -25856, 20480

        System.out.println("\nAnalyse der Parameter 10-13:");

        // Parameter 10: 3, 160 -> 768
        analyzeShortConversion(3, 160, 768, "param10");

        // Parameter 11: 155, 80 -> -24576
        analyzeShortConversion(155, 80, -24576, "param11");

        // Teste verschiedene Short-Konvertierungen
        System.out.println("\nTeste für 768:");
        testShortValue(768);

        System.out.println("\nTeste für -24576:");
        testShortValue(-24576);

        System.out.println("\nTeste für -25856:");
        testShortValue(-25856);

        System.out.println("\nTeste für 20480:");
        testShortValue(20480);
    }

    private static void analyzeShortConversion(int byte1, int byte2, int target, String paramName) {
        System.out.printf("\n%s: Bytes %d, %d -> Ziel: %d\n", paramName, byte1, byte2, target);

        // Little-Endian
        int le = byte1 | (byte2 << 8);
        System.out.printf("  Little-Endian: %d | (%d << 8) = %d\n", byte1, byte2, le);

        // Big-Endian
        int be = (byte1 << 8) | byte2;
        System.out.printf("  Big-Endian: (%d << 8) | %d = %d\n", byte1, byte2, be);

        // Signed Little-Endian
        int signedLE = le;
        if (signedLE > 32767) signedLE -= 65536;
        System.out.printf("  Signed LE: %d\n", signedLE);

        // Signed Big-Endian
        int signedBE = be;
        if (signedBE > 32767) signedBE -= 65536;
        System.out.printf("  Signed BE: %d\n", signedBE);

        // Prüfe, welche Methode das Ziel erreicht
        if (le == target) System.out.println("  ✅ Little-Endian korrekt!");
        if (be == target) System.out.println("  ✅ Big-Endian korrekt!");
        if (signedLE == target) System.out.println("  ✅ Signed Little-Endian korrekt!");
        if (signedBE == target) System.out.println("  ✅ Signed Big-Endian korrekt!");
    }

    private static void testShortValue(int value) {
        // Konvertiere Zielwert zu Bytes
        int byte1_LE = value & 0xFF;
        int byte2_LE = (value >> 8) & 0xFF;

        int byte1_BE = (value >> 8) & 0xFF;
        int byte2_BE = value & 0xFF;

        System.out.printf("  Wert %d als LE-Bytes: %d, %d\n", value, byte1_LE, byte2_LE);
        System.out.printf("  Wert %d als BE-Bytes: %d, %d\n", value, byte1_BE, byte2_BE);

        // Teste negative Werte
        if (value < 0) {
            int unsignedValue = value + 65536;
            int ubyte1_LE = unsignedValue & 0xFF;
            int ubyte2_LE = (unsignedValue >> 8) & 0xFF;
            System.out.printf("  Wert %d als unsigned LE-Bytes: %d, %d\n", value, ubyte1_LE, ubyte2_LE);
        }
    }
}
