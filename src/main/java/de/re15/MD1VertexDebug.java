package de.re15;

import java.io.*;

public class MD1VertexDebug {
    public static void main(String[] args) throws Exception {
        String path = "info/example/PL00/PL00.md1";
        int[] offsets = {44, 32140}; // Beispiel: pTriVert und pQuadVert, ggf. anpassen
        for (int offset : offsets) {
            System.out.println("--- Offset: " + offset + " ---");
            try (RandomAccessFile raf = new RandomAccessFile(path, "r")) {
                raf.seek(offset);
                for (int i = 0; i < 2; i++) {
                    int a = raf.readUnsignedByte();
                    int b = raf.readUnsignedByte();
                    int c = raf.readUnsignedByte();
                    int d = raf.readUnsignedByte();
                    int e = raf.readUnsignedByte();
                    int f = raf.readUnsignedByte();
                    int g = raf.readUnsignedByte();
                    int h = raf.readUnsignedByte();
                    short s1_le = (short)((b << 8) | a);
                    short s2_le = (short)((d << 8) | c);
                    short s3_le = (short)((f << 8) | e);
                    short s1_be = (short)((a << 8) | b);
                    short s2_be = (short)((c << 8) | d);
                    short s3_be = (short)((e << 8) | f);
                    System.out.println("Vertex " + i + ":");
                    System.out.println("  Bytes: " + String.format("%02x %02x %02x %02x %02x %02x %02x %02x", a,b,c,d,e,f,g,h));
                    // Alle Reihenfolgen, little-endian
                    System.out.println("  LE x,y,z: " + s1_le + ", " + s2_le + ", " + s3_le);
                    System.out.println("  LE x,z,y: " + s1_le + ", " + s3_le + ", " + s2_le);
                    System.out.println("  LE y,x,z: " + s2_le + ", " + s1_le + ", " + s3_le);
                    System.out.println("  LE y,z,x: " + s2_le + ", " + s3_le + ", " + s1_le);
                    System.out.println("  LE z,x,y: " + s3_le + ", " + s1_le + ", " + s2_le);
                    System.out.println("  LE z,y,x: " + s3_le + ", " + s2_le + ", " + s1_le);
                    // Alle Reihenfolgen, big-endian
                    System.out.println("  BE x,y,z: " + s1_be + ", " + s2_be + ", " + s3_be);
                    System.out.println("  BE x,z,y: " + s1_be + ", " + s3_be + ", " + s2_be);
                    System.out.println("  BE y,x,z: " + s2_be + ", " + s1_be + ", " + s3_be);
                    System.out.println("  BE y,z,x: " + s2_be + ", " + s3_be + ", " + s1_be);
                    System.out.println("  BE z,x,y: " + s3_be + ", " + s1_be + ", " + s2_be);
                    System.out.println("  BE z,y,x: " + s3_be + ", " + s2_be + ", " + s1_be);
                    // Skalierungen
                    float[] scales = {32.0f, 16.0f, 1024.0f, 4096.0f};
                    for (float scale : scales) {
                        System.out.println("    LE x/"+scale+": " + (s1_le/scale) + ", y/"+scale+": " + (s2_le/scale) + ", z/"+scale+": " + (s3_le/scale));
                    }
                }
            }
        }
    }
}
