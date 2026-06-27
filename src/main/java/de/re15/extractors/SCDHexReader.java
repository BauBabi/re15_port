package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * SCD-Datei Hex-Reader mit direkter Ausgabe in Datei
 */
public class SCDHexReader {

    public static void main(String[] args) {
        try {
            analyzeAndWriteResults();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void analyzeAndWriteResults() throws IOException {
        StringBuilder results = new StringBuilder();
        results.append("=== SCD HEX-ANALYSE ERGEBNISSE ===\n\n");

        // Analysiere ROOM115U.SCD
        byte[] data = Files.readAllBytes(Paths.get("info/Re1.5/PSX/DATA/ROOM115U.SCD"));
        results.append("ROOM115U.SCD Analyse:\n");
        results.append("Dateigröße: ").append(data.length).append(" bytes\n\n");

        // Suche nach Sce_em_set Opcode (0x44)
        for (int i = 0; i < data.length - 20; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                results.append("Sce_em_set gefunden bei Offset ").append(i).append(":\n");

                // Zeige Raw Bytes
                results.append("Raw Bytes: ");
                for (int j = 0; j < 20; j++) {
                    results.append(String.format("%02X ", data[i + j] & 0xFF));
                }
                results.append("\n\n");

                // Parameter 1-7 (Bytes)
                results.append("Parameter 1-7 (einzelne Bytes):\n");
                for (int j = 1; j <= 7; j++) {
                    results.append(String.format("  param%d: %d (0x%02X)\n", j, data[i + j] & 0xFF, data[i + j] & 0xFF));
                }
                results.append("\n");

                // Parameter 8-13 (Shorts) - verschiedene Interpretationen
                results.append("Parameter 8-13 (Short-Interpretationen):\n");
                for (int paramNum = 8, offset = 8; paramNum <= 13; paramNum++, offset += 2) {
                    int b1 = data[i + offset] & 0xFF;
                    int b2 = data[i + offset + 1] & 0xFF;

                    int le = b1 | (b2 << 8);
                    int be = (b1 << 8) | b2;
                    int signedLE = le > 32767 ? le - 65536 : le;
                    int signedBE = be > 32767 ? be - 65536 : be;

                    results.append(String.format("  param%d [%02X %02X]: LE=%d, BE=%d, SignedLE=%d, SignedBE=%d\n",
                        paramNum, b1, b2, le, be, signedLE, signedBE));
                }
                results.append("\n");

                // Vergleich mit Referenz
                results.append("Referenz-Vergleich:\n");
                results.append("Soll: Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480)\n");

                int[] targetValues = {1537, 19456, 768, -24576, -25856, 20480};
                for (int paramNum = 8, offset = 8, refIndex = 0; paramNum <= 13; paramNum++, offset += 2, refIndex++) {
                    int b1 = data[i + offset] & 0xFF;
                    int b2 = data[i + offset + 1] & 0xFF;

                    int le = b1 | (b2 << 8);
                    int be = (b1 << 8) | b2;
                    int signedLE = le > 32767 ? le - 65536 : le;
                    int signedBE = be > 32767 ? be - 65536 : be;

                    int expectedValue = targetValues[refIndex];

                    results.append(String.format("  param%d (Soll: %d):\n", paramNum, expectedValue));
                    if (le == expectedValue) results.append("    ✅ Little-Endian korrekt!\n");
                    if (be == expectedValue) results.append("    ✅ Big-Endian korrekt!\n");
                    if (signedLE == expectedValue) results.append("    ✅ Signed Little-Endian korrekt!\n");
                    if (signedBE == expectedValue) results.append("    ✅ Signed Big-Endian korrekt!\n");

                    if (le != expectedValue && be != expectedValue && signedLE != expectedValue && signedBE != expectedValue) {
                        results.append("    ❌ Keine Standard-Interpretation passt!\n");

                        // Teste andere Möglichkeiten
                        int reversedLE = b2 | (b1 << 8);
                        int reversedSignedLE = reversedLE > 32767 ? reversedLE - 65536 : reversedLE;
                        results.append(String.format("    Umgekehrt LE: %d, Umgekehrt SignedLE: %d\n", reversedLE, reversedSignedLE));
                        if (reversedLE == expectedValue) results.append("    ✅ Umgekehrte Little-Endian korrekt!\n");
                        if (reversedSignedLE == expectedValue) results.append("    ✅ Umgekehrte Signed Little-Endian korrekt!\n");
                    }
                }

                break; // Nur ersten Sce_em_set analysieren
            }
        }

        // Schreibe Ergebnisse in Datei
        Files.write(Paths.get("scd_analysis_results.txt"), results.toString().getBytes());
        System.out.println("Analyse-Ergebnisse geschrieben in: scd_analysis_results.txt");
    }
}
