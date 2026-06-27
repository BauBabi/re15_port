package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * SCD Hex Debugger - Analysiert die exakte Byte-Struktur für Sce_em_set
 */
public class SCDHexDebugger {

    public static void main(String[] args) {
        try {
            System.out.println("=== SCD HEX DEBUGGER ===");
            analyzeSCDStructure("info/Re1.5/PSX/DATA/ROOM115U.SCD");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void analyzeSCDStructure(String filePath) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get(filePath));
        System.out.println("Analysiere: " + filePath);
        System.out.println("Dateigröße: " + data.length + " bytes");

        // Suche nach Sce_em_set Opcode (0x44)
        for (int i = 0; i < data.length - 20; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                System.out.println("\nSce_em_set Opcode 0x44 gefunden bei Offset " + i + ":");

                // Zeige Raw Bytes
                System.out.print("Raw Bytes: ");
                for (int j = 0; j < 20; j++) {
                    System.out.printf("%02X ", data[i + j] & 0xFF);
                    if (j == 7) System.out.print("| "); // Trenner nach Byte-Parametern
                }
                System.out.println();

                // Analysiere die Parameter-Struktur
                System.out.println("\nParameter-Analyse:");
                System.out.println("Byte-Parameter (1-7):");
                for (int j = 1; j <= 7; j++) {
                    System.out.printf("  param%d: %d (0x%02X)\n", j, data[i + j] & 0xFF, data[i + j] & 0xFF);
                }

                System.out.println("\nShort-Parameter (8-13) - Verschiedene Interpretationen:");

                // Parameter 8 (Bytes 8-9)
                int param8_LE = (data[i + 8] & 0xFF) | ((data[i + 9] & 0xFF) << 8);
                int param8_BE = ((data[i + 8] & 0xFF) << 8) | (data[i + 9] & 0xFF);
                System.out.printf("  param8: LE=%d, BE=%d (Referenz: 1537)\n", param8_LE, param8_BE);

                // Parameter 9 (Bytes 10-11)
                int param9_LE = (data[i + 10] & 0xFF) | ((data[i + 11] & 0xFF) << 8);
                int param9_BE = ((data[i + 10] & 0xFF) << 8) | (data[i + 11] & 0xFF);
                System.out.printf("  param9: LE=%d, BE=%d (Referenz: 19456)\n", param9_LE, param9_BE);

                // Parameter 10 (Bytes 12-13) - Das Problem!
                int param10_LE = (data[i + 12] & 0xFF) | ((data[i + 13] & 0xFF) << 8);
                int param10_BE = ((data[i + 12] & 0xFF) << 8) | (data[i + 13] & 0xFF);
                int param10_Byte1 = data[i + 12] & 0xFF;
                int param10_Byte2 = data[i + 13] & 0xFF;
                System.out.printf("  param10: LE=%d, BE=%d, Byte1=%d, Byte2=%d (Referenz: 768)\n",
                    param10_LE, param10_BE, param10_Byte1, param10_Byte2);

                // Parameter 11 (Bytes 14-15)
                int param11_LE = (data[i + 14] & 0xFF) | ((data[i + 15] & 0xFF) << 8);
                int param11_BE = ((data[i + 14] & 0xFF) << 8) | (data[i + 15] & 0xFF);
                int param11_Signed = param11_LE > 32767 ? param11_LE - 65536 : param11_LE;
                System.out.printf("  param11: LE=%d, BE=%d, Signed=%d (Referenz: -24576)\n",
                    param11_LE, param11_BE, param11_Signed);

                // Parameter 12 (Bytes 16-17)
                int param12_LE = (data[i + 16] & 0xFF) | ((data[i + 17] & 0xFF) << 8);
                int param12_BE = ((data[i + 16] & 0xFF) << 8) | (data[i + 17] & 0xFF);
                int param12_Signed = param12_LE > 32767 ? param12_LE - 65536 : param12_LE;
                System.out.printf("  param12: LE=%d, BE=%d, Signed=%d (Referenz: -25856)\n",
                    param12_LE, param12_BE, param12_Signed);

                // Parameter 13 (Bytes 18-19)
                int param13_LE = (data[i + 18] & 0xFF) | ((data[i + 19] & 0xFF) << 8);
                int param13_BE = ((data[i + 18] & 0xFF) << 8) | (data[i + 19] & 0xFF);
                System.out.printf("  param13: LE=%d, BE=%d (Referenz: 20480)\n", param13_LE, param13_BE);

                System.out.println("\nReferenz-Vergleich:");
                System.out.println("Soll: Sce_em_set(0, 0x46, 0, 84, 0, 168, 0, 1537, 19456, 768, -24576, -25856, 20480);");

                // Bestimme korrekte Interpretation
                boolean param8_correct = (param8_BE == 1537);
                boolean param9_correct = (param9_BE == 19456);
                boolean param10_correct = (param10_LE == 768);
                boolean param11_correct = (param11_Signed == -24576);
                boolean param12_correct = (param12_Signed == -25856);
                boolean param13_correct = (param13_LE == 20480);

                System.out.println("\nKorrektheits-Check:");
                System.out.println("  param8 (BE): " + (param8_correct ? "✅" : "❌"));
                System.out.println("  param9 (BE): " + (param9_correct ? "✅" : "❌"));
                System.out.println("  param10 (LE): " + (param10_correct ? "✅" : "❌"));
                System.out.println("  param11 (LE Signed): " + (param11_correct ? "✅" : "❌"));
                System.out.println("  param12 (LE Signed): " + (param12_correct ? "✅" : "❌"));
                System.out.println("  param13 (LE): " + (param13_correct ? "✅" : "❌"));

                break; // Nur den ersten Sce_em_set analysieren
            }
        }
    }
}
