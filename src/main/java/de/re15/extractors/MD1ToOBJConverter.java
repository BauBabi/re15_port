package de.re15.extractors;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * MD1ToOBJConverter extrahiert Vertices und Faces aus einer MD1-Datei und schreibt sie als OBJ-Datei.
 * - Offset und Struktur werden per Hex-Analyse bestimmt.
 * - Protokolliert alle Schritte gemäß Projektvorgabe.
 */
public class MD1ToOBJConverter {
    public static void main(String[] args) throws IOException {
        System.out.println("TEST: Start");
        if (args.length < 2) {
            System.out.println("Usage: java de.re15.extractors.MD1ToOBJConverter <input.md1> <output.obj>");
            return;
        }
        String inputPath = args[0];
        String outputPath = args[1];
        try (FileInputStream fis = new FileInputStream(inputPath);
             BufferedWriter writer = new BufferedWriter(new FileWriter(outputPath))) {
            byte[] data = fis.readAllBytes();
            System.out.println("[DEBUG] Datei eingelesen, Länge: " + data.length);
            try (BufferedWriter logDbg = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                logDbg.write("[DEBUG] Datei eingelesen, Länge: " + data.length + "\n");
            } catch (Exception e) { System.err.println("[DEBUG] Fehler beim Schreiben ins OBJ_HEAD: " + e); }
            // Offset-Tests direkt nach Einlesen der Datei
            int[] testOffsets = {0, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400};
            try (BufferedWriter log = new BufferedWriter(new FileWriter("extracted/EXTRACTION_LOG.txt", true))) {
                for (int off : testOffsets) {
                    if (off + 12 <= data.length) {
                        try {
                            ByteBuffer bbt = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
                            bbt.position(off);
                            int pUVt = bbt.getInt();
                            int unkt = bbt.getInt();
                            int nObjt = bbt.getInt();
                            int meshCountt = nObjt / 2;
                            log.write(String.format("[OFFSET_TEST] off=0x%X pUV=%08X unknown=%08X nObj=%d meshCount=%d\n", off, pUVt, unkt, nObjt, meshCountt));
                            System.out.println(String.format("[OFFSET_TEST] off=0x%X pUV=%08X unknown=%08X nObj=%d meshCount=%d", off, pUVt, unkt, nObjt, meshCountt));
                        } catch (Exception e) {
                            System.err.println("[FATAL] Exception bei Offset-Test off=0x" + Integer.toHexString(off) + ": " + e);
                            e.printStackTrace();
                        }
                    }
                }
            } catch (Exception e) {
                System.err.println("[FATAL] Exception beim Öffnen/Schreiben der Logdatei: " + e);
                e.printStackTrace();
            }
            try (BufferedWriter log = new BufferedWriter(new FileWriter("extracted/EXTRACTION_LOG.txt", true))) {
                log.write("[TEST] Logdatei erfolgreich geöffnet.\n");
                // 1. Header lesen
                int headerOffset = 0x0; // TEST: Header direkt am Anfang der Datei
                ByteBuffer bb = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
                bb.position(headerOffset);
                int pUV = bb.getInt(); // 0x00
                int unknown = bb.getInt(); // 0x04
                int nObj = bb.getInt(); // 0x08
                int meshCount = nObj / 2; // laut Referenz
                log.write(String.format("[INFO] TEST: Header ab 0x0: pUV=%08X unknown=%08X nObj=%d meshCount=%d\n", pUV, unknown, nObj, meshCount));
                log.write(String.format("[TEST] meshCount=%d\n", meshCount));
                // Nach Header-Parsing
                System.out.println("[DEBUG] Header: pUV=" + pUV + ", unknown=" + unknown + ", nObj=" + nObj + ", meshCount=" + meshCount);
                try (BufferedWriter logDbg = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                    logDbg.write("[DEBUG] Header: pUV=" + pUV + ", unknown=" + unknown + ", nObj=" + nObj + ", meshCount=" + meshCount + "\n");
                } catch (Exception e) { System.err.println("[DEBUG] Fehler beim Schreiben ins OBJ_HEAD: " + e); }
                if (meshCount > 256) {
                    log.write(String.format("[FATAL] meshCount (%d) zu groß, Abbruch!\n", meshCount));
                    System.out.println("[DEBUG] meshCount > 256, Abbruch!");
                    try (BufferedWriter logDbg = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                        logDbg.write("[DEBUG] meshCount > 256, Abbruch!\n");
                    } catch (Exception e) { System.err.println("[DEBUG] Fehler beim Schreiben ins OBJ_HEAD: " + e); }
                    return;
                }
                // 2. Mesh-Index-Tabelle lesen
                int meshIndexSize = 0x38; // 56 Bytes pro Mesh laut md1.h
                int meshIndexTableOffset = headerOffset + 0x0C;
                int[] meshVertStart = new int[meshCount];
                int[] meshVertCount = new int[meshCount];
                int[] meshTriStart = new int[meshCount];
                int[] meshTriCount = new int[meshCount];
                int totalVerts = 0;
                for (int i = 0; i < meshCount; i++) {
                    int idxOffset = meshIndexTableOffset + i * meshIndexSize;
                    int pTriVert = ByteBuffer.wrap(data, idxOffset + 0x00, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
                    int nTriVert = ByteBuffer.wrap(data, idxOffset + 0x04, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
                    int pTriPrim = ByteBuffer.wrap(data, idxOffset + 0x10, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
                    int nTriPrim = ByteBuffer.wrap(data, idxOffset + 0x14, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
                    meshVertStart[i] = pTriVert;
                    meshVertCount[i] = nTriVert;
                    meshTriStart[i] = pTriPrim;
                    meshTriCount[i] = nTriPrim;
                    log.write(String.format("[INFO] Mesh %d: pTriVert=%08X nTriVert=%d pTriPrim=%08X nTriPrim=%d\n", i, pTriVert, nTriVert, pTriPrim, nTriPrim));
                    totalVerts += nTriVert;
                }
                log.write(String.format("[TEST] totalVerts=%d\n", totalVerts));
                // Nach Mesh-Index-Parsing
                System.out.println("[DEBUG] meshCount=" + meshCount);
                for (int i = 0; i < Math.min(meshCount, 3); i++) {
                    System.out.println("[DEBUG] meshVertCount[" + i + "]=" + meshVertCount[i] + ", meshTriCount[" + i + "]=" + meshTriCount[i]);
                    try (BufferedWriter logDbg = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                        logDbg.write("[DEBUG] meshVertCount[" + i + "]=" + meshVertCount[i] + ", meshTriCount[" + i + "]=" + meshTriCount[i] + "\n");
                    } catch (Exception e) { System.err.println("[DEBUG] Fehler beim Schreiben ins OBJ_HEAD: " + e); }
                }
                if (totalVerts > 65536) {
                    log.write(String.format("[FATAL] Gesamtanzahl Vertices (%d) zu groß, Abbruch!\n", totalVerts));
                    System.out.println("[DEBUG] totalVerts > 65536, Abbruch!");
                    try (BufferedWriter logDbg = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                        logDbg.write("[DEBUG] totalVerts > 65536, Abbruch!\n");
                    } catch (Exception e) { System.err.println("[DEBUG] Fehler beim Schreiben ins OBJ_HEAD: " + e); }
                    return;
                }
                // Meshdaten-Start berechnen
                int meshDataStart = headerOffset + 0x0C + meshCount * meshIndexSize;
                int globalVertexCount = 0;
                double[][] globalVertices = new double[65536][3]; // großzügig dimensioniert
                int[] meshVertexBase = new int[meshCount];
                for (int i = 0; i < meshCount; i++) {
                    meshVertexBase[i] = globalVertexCount;
                    for (int v = 0; v < meshVertCount[i]; v++) {
                        int vtxOffset = meshDataStart + meshVertStart[i] + v * 8;
                        if (vtxOffset + 6 >= data.length) {
                            String msg = String.format("[ERROR] Vertex-Offset außerhalb der Datei: mesh=%d v=%d vtxOffset=%d data.length=%d\n", i, v, vtxOffset, data.length);
                            System.err.print(msg);
                            log.write(msg);
                            continue;
                        }
                        short x = ByteBuffer.wrap(data, vtxOffset, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                        short y = ByteBuffer.wrap(data, vtxOffset+2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                        short z = ByteBuffer.wrap(data, vtxOffset+4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                        globalVertices[globalVertexCount][0] = x * 1.0;
                        globalVertices[globalVertexCount][1] = z * 1.0;
                        globalVertices[globalVertexCount][2] = y * 1.0;
                        if (globalVertexCount < 10) {
                            log.write(String.format("[VERTEX-RAW] #%d: x=%d y=%d z=%d\n", globalVertexCount, x, y, z));
                            log.write(String.format("[VERTEX-SCALED] #%d: X=%.6f Y=%.6f Z=%.6f\n", globalVertexCount, globalVertices[globalVertexCount][0], globalVertices[globalVertexCount][1], globalVertices[globalVertexCount][2]));
                        }
                        globalVertexCount++;
                    }
                }
                // Testausgabe direkt nach meshTriCount-Initialisierung
                try (BufferedWriter logTest = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                    logTest.write("[TEST] Kontrollfluss: Nach meshTriCount-Initialisierung erreicht.\n");
                } catch (Exception e) { /* Ignorieren für Test */ }
                // 4. Vertices ins OBJ schreiben
                for (int i = 0; i < globalVertexCount; i++) {
                    writer.write(String.format("v %.6f %.6f %.6f\n", globalVertices[i][0], globalVertices[i][1], globalVertices[i][2]));
                }
                // Testausgabe direkt vor dem Schreiben der Vertices
                try (BufferedWriter logTest2 = new BufferedWriter(new FileWriter("extracted/EXTRACTED_OBJ_HEAD.txt", true))) {
                    logTest2.write("[TEST] Kontrollfluss: Vor Schreiben der Vertices erreicht.\n");
                } catch (Exception e) { /* Ignorieren für Test */ }
                int faceCount = 0;
                // 5. Faces pro Mesh extrahieren und ins OBJ schreiben
                for (int i = 0; i < meshCount; i++) {
                    for (int t = 0; t < meshTriCount[i]; t++) {
                        int triOffset = meshDataStart + meshTriStart[i] + t * 12; // 12 Bytes pro TriPrim
                        if (triOffset + 10 >= data.length) {
                            String msg = String.format("[ERROR] Face-Offset außerhalb der Datei: mesh=%d t=%d triOffset=%d data.length=%d\n", i, t, triOffset, data.length);
                            System.err.print(msg);
                            try (BufferedWriter logFace = new BufferedWriter(new FileWriter("extracted/EXTRACTION_LOG.txt", true))) { logFace.write(msg); }
                            continue;
                        }
                        // Struktur: n0(2), v0(2), n1(2), v1(2), n2(2), v2(2)
                        int v0 = (ByteBuffer.wrap(data, triOffset+2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF);
                        int v1 = (ByteBuffer.wrap(data, triOffset+6, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF);
                        int v2 = (ByteBuffer.wrap(data, triOffset+10,2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF);
                        // Indizes globalisieren
                        int idx0 = meshVertexBase[i] + v0 + 1;
                        int idx1 = meshVertexBase[i] + v1 + 1;
                        int idx2 = meshVertexBase[i] + v2 + 1;
                        writer.write(String.format("f %d %d %d\n", idx0, idx1, idx2));
                        faceCount++;
                    }
                }
                System.out.println("[MD1ToOBJConverter] Vertices geschrieben: " + globalVertexCount);
                System.out.println("[MD1ToOBJConverter] Faces geschrieben: " + faceCount);
            } catch (Exception ex) {
                String msg = "[FATAL] Exception: " + ex + "\n";
                System.err.print(msg);
                try (BufferedWriter log2 = new BufferedWriter(new FileWriter("extracted/EXTRACTION_LOG.txt", true))) { log2.write(msg); }
            }
        }
    }
}












