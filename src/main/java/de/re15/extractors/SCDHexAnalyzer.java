package de.re15.extractors;

import java.io.*;
import java.nio.file.*;

/**
 * Detaillierte Hex-Analyse der SCD-Dateien
 */
public class SCDHexAnalyzer {

    public static void main(String[] args) throws IOException {
        analyzeSCDFile("info/Re1.5/PSX/DATA/ROOM115U.SCD");
    }

    public static void analyzeSCDFile(String filePath) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get(filePath));

        System.out.println("=== SCD HEX ANALYSE: " + filePath + " ===");
        System.out.println("Dateigröße: " + data.length + " bytes");
        System.out.println();

        // Zeige die ersten 100 Bytes in Hex und analysiere Opcodes
        for (int i = 0; i < Math.min(100, data.length); i += 16) {
            System.out.printf("%08X: ", i);

            // Hex-Darstellung
            for (int j = 0; j < 16 && i + j < data.length; j++) {
                System.out.printf("%02X ", data[i + j] & 0xFF);
            }

            System.out.println();
        }

        System.out.println("\n=== OPCODE ANALYSE ===");

        // Analysiere die ersten Opcodes
        analyzeOpcodeAt(data, 0);
        analyzeOpcodeAt(data, 1);
        analyzeOpcodeAt(data, 2);

        // Suche nach dem Sce_em_set Opcode (0x44)
        for (int i = 0; i < data.length; i++) {
            if ((data[i] & 0xFF) == 0x44) {
                System.out.println("\nSce_em_set (0x44) gefunden at offset " + i + ":");
                analyzeSceEmSet(data, i);
                break;
            }
        }
    }

    private static void analyzeOpcodeAt(byte[] data, int offset) {
        if (offset >= data.length) return;

        int opcode = data[offset] & 0xFF;
        System.out.printf("Offset %d (0x%02X): Opcode 0x%02X", offset, offset, opcode);

        String name = getOpcodeName(opcode);
        if (name != null) {
            System.out.println(" = " + name);
        } else {
            System.out.println(" = UNKNOWN");
        }
    }

    private static void analyzeSceEmSet(byte[] data, int offset) {
        if (offset + 21 >= data.length) {
            System.out.println("  Nicht genug Daten für vollständige Analyse");
            return;
        }

        System.out.println("  Byte-Parameter:");
        for (int i = 1; i <= 6; i++) {
            System.out.printf("    [%d] = 0x%02X (%d)\n", i, data[offset + i] & 0xFF, data[offset + i] & 0xFF);
        }

        System.out.println("  Position-Parameter (Little-Endian signed shorts):");
        int x = readSignedShort(data, offset + 7);
        int y = readSignedShort(data, offset + 9);
        int z = readSignedShort(data, offset + 11);
        int dir = readSignedShort(data, offset + 13);

        System.out.printf("    X = %d (0x%04X)\n", x, x & 0xFFFF);
        System.out.printf("    Y = %d (0x%04X)\n", y, y & 0xFFFF);
        System.out.printf("    Z = %d (0x%04X)\n", z, z & 0xFFFF);
        System.out.printf("    DIR = %d (0x%04X)\n", dir, dir & 0xFFFF);

        System.out.println("  Unbekannte Parameter (Little-Endian unsigned shorts):");
        int unk1 = readUnsignedShort(data, offset + 15);
        int unk2 = readUnsignedShort(data, offset + 17);

        System.out.printf("    UNK1 = %d (0x%04X)\n", unk1, unk1);
        System.out.printf("    UNK2 = %d (0x%04X)\n", unk2, unk2);
    }

    private static int readSignedShort(byte[] data, int offset) {
        int value = (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
        return value > 32767 ? value - 65536 : value;
    }

    private static int readUnsignedShort(byte[] data, int offset) {
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    private static String getOpcodeName(int opcode) {
        switch (opcode) {
            case 0x00: return "nop";
            case 0x01: return "return";
            case 0x02: return "Evt_next";
            case 0x0B: return "Wsleep";
            case 0x0C: return "Wsleeping";
            case 0x44: return "Sce_em_set";
            default: return null;
        }
    }
}
