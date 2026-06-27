package de.baubabi.parser;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public class HexParser {
    public HexParser() {}

    public static String readFileAsHex(String reFile) {
        Path filePath = Path.of(reFile);

        try {
            byte[] fileBytes = Files.readAllBytes(filePath);

            StringBuilder hexString = new StringBuilder();
            for (byte b : fileBytes) {
                hexString.append(String.format("%02X", b));
            }

            return hexString.toString();

        } catch (IOException e) {
            System.err.println("Fehler beim Lesen der Datei: " + e.getMessage());
        }

        return "";
    }

    public static String readULong4Byte(String hexString, int startIndex) {
        String byteWordString = hexString.substring(startIndex, startIndex + 8);
            if (byteWordString.length() != 8) {
                throw new IllegalArgumentException("Eingabe muss 8 Zeichen (4 Bytes) lang sein.");
            }

            // Direktes Umdrehen der 2-Byte-Blöcke
            return byteWordString.substring(6, 8) + // 4. Byte
                    byteWordString.substring(4, 6) + // 3. Byte
                    byteWordString.substring(2, 4) + // 2. Byte
                    byteWordString.substring(0, 2); // 1. Byte
    }

    public static String readUShort2Byte(String hexString, int startIndex) {
        String byteWordString = hexString.substring(startIndex, startIndex + 4);
        if (byteWordString.length() != 4) {
            throw new IllegalArgumentException("Eingabe muss 4 Zeichen (2 Bytes) lang sein.");
        }

        // Direktes Umdrehen der 2-Byte-Blöcke
        return  byteWordString.substring(2, 4) + // 2. Byte
                byteWordString.substring(0, 2); // 1. Byte
    }

    public static String readByte(String hexString, int startIndex) {
        String byteWordString = hexString.substring(startIndex, startIndex + 2);
        if (byteWordString.length() != 2) {
            throw new IllegalArgumentException("Eingabe muss 2 Zeichen (1 Bytes) lang sein.");
        }

        return  byteWordString;
    }
}
