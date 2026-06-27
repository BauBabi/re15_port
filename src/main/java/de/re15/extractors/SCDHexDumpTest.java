package de.re15.extractors;
import java.nio.file.Files;
import java.nio.file.Paths;
public class SCDHexDumpTest {
    public static void main(String[] args) throws Exception {
        byte[] data = Files.readAllBytes(Paths.get("info/Re1.5/PSX/DATA/ROOM115U.SCD"));
        int len = Math.min(64, data.length);
        for (int i = 0; i < len; i++) {
            System.out.printf("%02X ", data[i]);
            if ((i+1) % 16 == 0) System.out.println();
        }
        System.out.println();
    }
}

