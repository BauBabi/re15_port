package de.re15.extractors;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class SCDSleepByteAnalyzer {
    public static void main(String[] args) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get("info/Re1.5/PSX/DATA/ROOM115U.SCD"));
        for (int i = 0; i < data.length - 4; i++) {
            if ((data[i] & 0xFF) == 0x09) {
                System.out.printf("Sleep-Opcode @ 0x%08X: %02X %02X %02X %02X\n", i, data[i] & 0xFF, data[i+1] & 0xFF, data[i+2] & 0xFF, data[i+3] & 0xFF);
            }
        }
    }
}

