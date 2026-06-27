package de.re15;

import java.io.RandomAccessFile;
import java.io.PrintWriter;
import java.io.File;

public class MD1VertexBruteForceSearch {
    public static void main(String[] args) throws Exception {
        System.out.println("DEBUG: Start BruteForceMD1");
        PrintWriter log = new PrintWriter(new File("extracted/MD1VertexBruteForceSearch_tail.txt"));
        log.println("TESTAUSGABE: main() gestartet");
        log.flush();
        try {
            String path = "c:/workspace/git/reAi/already_extracted/PSX/EMD/CDEMD0/em1A/em1A.md1";
            log.println("DEBUG: Pfad gesetzt: " + path);
            log.flush();
            float tol = 0.1f;
            float[][] refs = {{688.75f, 0.1875f, 0.0f}};
            long maxBlocks = 1;
            try (RandomAccessFile raf = new RandomAccessFile(path, "r")) {
                log.println("DEBUG: Datei erfolgreich geöffnet");
                log.println("DEBUG: Dateilänge: " + raf.length());
                log.flush();
                long fileLen = raf.length();
                for (long pos = 0; pos < fileLen - 8; pos++) {
                    raf.seek(pos);
                    int[] b = new int[8];
                    for (int i = 0; i < 8; i++) b[i] = raf.readUnsignedByte();
                    short[] le = new short[3];
                    le[0] = (short) ((b[1] << 8) | b[0]);
                    le[1] = (short) ((b[3] << 8) | b[2]);
                    le[2] = (short) ((b[5] << 8) | b[4]);
                    short[] be = new short[3];
                    be[0] = (short) ((b[0] << 8) | b[1]);
                    be[1] = (short) ((b[2] << 8) | b[3]);
                    be[2] = (short) ((b[4] << 8) | b[5]);
                    float[] scales = {32.0f, 16.0f, 1024.0f, 4096.0f, 100.0f, 2048.0f, 64.0f, 128.0f, 256.0f, 512.0f};
                    for (float scale : scales) {
                        for (int perm = 0; perm < 6; perm++) {
                            int[] p = switch (perm) {
                                case 0 -> new int[]{0, 1, 2};
                                case 1 -> new int[]{0, 2, 1};
                                case 2 -> new int[]{1, 0, 2};
                                case 3 -> new int[]{1, 2, 0};
                                case 4 -> new int[]{2, 0, 1};
                                default -> new int[]{2, 1, 0};
                            };
                            for (int neg = 0; neg < 8; neg++) {
                                float signX = ((neg & 1) == 0) ? 1 : -1;
                                float signY = ((neg & 2) == 0) ? 1 : -1;
                                float signZ = ((neg & 4) == 0) ? 1 : -1;
                                for (float[] ref : refs) {
                                    float x = signX * le[p[0]] / scale, y = signY * le[p[1]] / scale, z = signZ * le[p[2]] / scale;
                                    if (Math.abs(x - ref[0]) < tol && Math.abs(y - ref[1]) < tol && Math.abs(z - ref[2]) < tol) {
                                        System.out.printf("MATCH RAW POS %d (LE, perm %d, scale %.1f, neg %d): %.6f %.6f %.6f\n", pos, perm, scale, neg, x, y, z);
                                    }
                                    x = signX * be[p[0]] / scale;
                                    y = signY * be[p[1]] / scale;
                                    z = signZ * be[p[2]] / scale;
                                    if (Math.abs(x - ref[0]) < tol && Math.abs(y - ref[1]) < tol && Math.abs(z - ref[2]) < tol) {
                                        System.out.printf("MATCH RAW POS %d (BE, perm %d, scale %.1f, neg %d): %.6f %.6f %.6f\n", pos, perm, scale, neg, x, y, z);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            log.println("DEBUG: Nach Brute-Force-Schleife");
            log.flush();
            // Statistische Analyse: Minimum, Maximum, Mittelwert der Abweichungen
            double minDist = Double.MAX_VALUE, maxDist = 0, sumDist = 0;
            long count = 0;
            try (RandomAccessFile raf = new RandomAccessFile(path, "r")) {
                long fileLen = raf.length();
                for (long pos = 0; pos < fileLen - 8; pos++) {
                    raf.seek(pos);
                    int[] b = new int[8];
                    for (int i = 0; i < 8; i++) b[i] = raf.readUnsignedByte();
                    short[] le = new short[3];
                    le[0] = (short) ((b[1] << 8) | b[0]);
                    le[1] = (short) ((b[3] << 8) | b[2]);
                    le[2] = (short) ((b[5] << 8) | b[4]);
                    short[] be = new short[3];
                    be[0] = (short) ((b[0] << 8) | b[1]);
                    be[1] = (short) ((b[2] << 8) | b[3]);
                    be[2] = (short) ((b[4] << 8) | b[5]);
                    float[] scales = {32.0f, 16.0f, 1024.0f, 4096.0f, 100.0f, 2048.0f, 64.0f, 128.0f, 256.0f, 512.0f};
                    for (float scale : scales) {
                        for (int perm = 0; perm < 6; perm++) {
                            int[] p = switch (perm) {
                                case 0 -> new int[]{0, 1, 2};
                                case 1 -> new int[]{0, 2, 1};
                                case 2 -> new int[]{1, 0, 2};
                                case 3 -> new int[]{1, 2, 0};
                                case 4 -> new int[]{2, 0, 1};
                                default -> new int[]{2, 1, 0};
                            };
                            for (int neg = 0; neg < 8; neg++) {
                                float signX = ((neg & 1) == 0) ? 1 : -1;
                                float signY = ((neg & 2) == 0) ? 1 : -1;
                                float signZ = ((neg & 4) == 0) ? 1 : -1;
                                for (float[] ref : refs) {
                                    float x = signX * le[p[0]] / scale, y = signY * le[p[1]] / scale, z = signZ * le[p[2]] / scale;
                                    double dist = Math.sqrt((x - ref[0]) * (x - ref[0]) + (y - ref[1]) * (y - ref[1]) + (z - ref[2]) * (z - ref[2]));
                                    minDist = Math.min(minDist, dist);
                                    maxDist = Math.max(maxDist, dist);
                                    sumDist += dist;
                                    count++;
                                    x = signX * be[p[0]] / scale;
                                    y = signY * be[p[1]] / scale;
                                    z = signZ * be[p[2]] / scale;
                                    dist = Math.sqrt((x - ref[0]) * (x - ref[0]) + (y - ref[1]) * (y - ref[1]) + (z - ref[2]) * (z - ref[2]));
                                    minDist = Math.min(minDist, dist);
                                    maxDist = Math.max(maxDist, dist);
                                    sumDist += dist;
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
            log.println("DEBUG: Nach Statistik-Berechnung");
            log.flush();
            System.out.println("DEBUG: Nach Datei-Ausgabe, Programmende");
        } catch (Exception e) {
            log.println("FEHLER: " + e);
            log.flush();
            throw e;
        } finally {
            log.println("DEBUG: main() wird beendet");
            log.close();
        }
    }
}