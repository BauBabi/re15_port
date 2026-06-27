package de.re15.extractors;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MD1VertexPatternFinder {
    public static void main(String[] args) {
        System.out.println("[MD1VertexPatternFinder] Starte Binäranalyse...");
        if (args.length < 1) {
            System.out.println("Usage: java de.re15.extractors.MD1VertexPatternFinder <md1-file>");
            return;
        }
        String md1Path = args[0];
        byte[] data = new byte[65536]; // 64 KB
        int bytesRead = 0;
        try (FileInputStream fis = new FileInputStream(md1Path)) {
            bytesRead = fis.read(data);
        } catch (IOException e) {
            System.out.println("Fehler beim Lesen: " + e.getMessage());
            return;
        }
        System.out.println("[MD1VertexPatternFinder] Bytes gelesen: " + bytesRead);
        int blockSize = 256;
        System.out.println("Offset | plausible shorts | plausible floats");
        for (int off = 0; off + blockSize <= data.length; off += blockSize) {
            int shortCount = 0;
            int floatCount = 0;
            // Shorts
            for (int i = 0; i < blockSize / 2; i++) {
                int idx = off + i * 2;
                if (idx + 2 > data.length) break;
                short val = ByteBuffer.wrap(data, idx, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                if (val > -32768 && val < 32767) shortCount++;
            }
            // Floats
            for (int i = 0; i < blockSize / 4; i++) {
                int idx = off + i * 4;
                if (idx + 4 > data.length) break;
                float val = ByteBuffer.wrap(data, idx, 4).order(ByteOrder.LITTLE_ENDIAN).getFloat();
                if (!Float.isNaN(val) && !Float.isInfinite(val) && val > -10000 && val < 10000) floatCount++;
            }
            System.out.printf("0x%05X | %3d/%3d | %3d/%3d\n", off, shortCount, blockSize/2, floatCount, blockSize/4);
        }
    }
}
