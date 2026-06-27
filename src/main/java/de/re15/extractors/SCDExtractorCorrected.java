package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Korrigierte SCD-Extraktion basierend auf tatsächlicher Hex-Analyse
 */
public class SCDExtractorCorrected {

    public static void main(String[] args) throws IOException {
        extractFromSCDFile("info/Re1.5/PSX/DATA/ROOM115U.SCD");
    }

    public static void extractFromSCDFile(String filePath) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get(filePath));

        System.out.println("=== KORREKTE SCD WERTE-EXTRAKTION ===");
        System.out.println("Datei: " + filePath);

        // Finde den Sce_em_set Opcode (0x44)
        for (int i = 0; i < data.length; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                System.out.println("Sce_em_set gefunden bei Offset: " + i);

                // Extrahiere die Parameter basierend auf der korrekten Struktur
                SCDEmSetParams params = extractCorrectParameters(data, i);

                System.out.println("Extrahierte Parameter:");
                System.out.printf("Sce_em_set(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)%n",
                    params.param1, params.param2, params.param3, params.param4,
                    params.param5, params.param6, params.param7, params.param8,
                    params.param9, params.param10, params.param11, params.param12, params.param13);

                // Zeige die rohen Bytes zur Verifikation
                System.out.println("\nRohe Bytes zur Verifikation:");
                for (int j = 0; j < 32 && i + j < data.length; j++) {
                    System.out.printf("Offset %d: 0x%02X (%d)%n",
                        i + j, data[i + j] & 0xFF, data[i + j] & 0xFF);
                }

                break;
            }
        }
    }

    private static SCDEmSetParams extractCorrectParameters(byte[] data, int offset) {
        SCDEmSetParams params = new SCDEmSetParams();

        // Basierend auf der Analyse: Erste 7 Parameter sind Bytes
        params.param1 = data[offset + 1] & 0xFF;  // Byte 1
        params.param2 = data[offset + 2] & 0xFF;  // Byte 2
        params.param3 = data[offset + 3] & 0xFF;  // Byte 3
        params.param4 = data[offset + 4] & 0xFF;  // Byte 4
        params.param5 = data[offset + 5] & 0xFF;  // Byte 5
        params.param6 = data[offset + 6] & 0xFF;  // Byte 6
        params.param7 = data[offset + 7] & 0xFF;  // Byte 7

        // Die letzten 6 Parameter sind 16-bit Little-Endian Werte ab Offset 8
        params.param8 = readLittleEndianSigned(data, offset + 8);
        params.param9 = readLittleEndianSigned(data, offset + 10);
        params.param10 = readLittleEndianSigned(data, offset + 12);
        params.param11 = readLittleEndianSigned(data, offset + 14);
        params.param12 = readLittleEndianSigned(data, offset + 16);
        params.param13 = readLittleEndianSigned(data, offset + 18);

        return params;
    }

    private static int readLittleEndianSigned(byte[] data, int offset) {
        if (offset + 1 >= data.length) return 0;
        int value = (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
        return value > 32767 ? value - 65536 : value;
    }

    static class SCDEmSetParams {
        int param1, param2, param3, param4, param5, param6, param7;
        int param8, param9, param10, param11, param12, param13;
    }
}
