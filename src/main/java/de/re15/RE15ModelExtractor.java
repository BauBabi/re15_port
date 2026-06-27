package de.re15;

import java.io.*;
import java.util.*;

public class RE15ModelExtractor {
    public static void main(String[] args) {
        // Protokollierung erfolgt jetzt extern per echo/Umleitung
        System.out.println("[DEBUG] MAIN gestartet!");
        // Automatische Zielpfad-Logik: aus info/Re1.5/.../DATEI.md1 -> extracted/.../DATEI.obj
        // Extrahiere nur das erste Objekt (Objekt 0) aus der MD1-Datei
        String inputPath = "info/example/PL00/PL00.md1";
        String outputPath = "extracted/example/PL00/PL00_obj0.obj";
        // Zielverzeichnis anlegen
        File outFile = new File(outputPath);
        File outDir = outFile.getParentFile();
        if (!outDir.exists()) {
            System.out.println("Erstelle Verzeichnis: " + outDir.getAbsolutePath());
            outDir.mkdirs();
        }
        System.out.println("Starte Modell-Extraktion...");
        System.out.println("Arbeitsverzeichnis: " + new File("").getAbsolutePath());
        File inputFile = new File(inputPath);
        if (!inputFile.exists()) {
            System.err.println("[FEHLER] Eingabedatei nicht gefunden: " + inputFile.getAbsolutePath());
        }
        try {
            // Debug-Ausgabe vor Aufruf von extractMD1ToOBJ
            try (PrintWriter debugMain = new PrintWriter(new FileWriter("extracted/DEBUG_MAIN.txt", true))) {
                debugMain.println("Main ruft extractMD1ToOBJ auf für: " + inputPath + " -> " + outputPath);
            }
            extractMD1ObjectToOBJ(inputPath, outputPath, 0); // Nur Objekt 0 extrahieren
            System.out.println("Extraktion abgeschlossen: " + outputPath);
        } catch (Exception e) {
            System.err.println("Fehler bei der Extraktion: " + e.getMessage());
            e.printStackTrace();
        }
        System.out.println("[DEBUG] Fortsetzung der Analyse...");
    // MINIMALTEST: Schreibe eine Datei und beende sofort
        try (PrintWriter minimalTest = new PrintWriter(new FileWriter("C:/Temp/copilot_minimaltest.txt", false))) {
            minimalTest.println("COPILOT MINIMALTEST: " + new Date());
            minimalTest.println("Arbeitsverzeichnis: " + System.getProperty("user.dir"));
        } catch (Exception e) {
            System.err.println("[FEHLER] Konnte C:/Temp/copilot_minimaltest.txt nicht schreiben: " + e.getMessage());
        }
        System.exit(0);
    }

    // Hilfsmethode: Referenz-OBJ-Vertices einlesen
    private static List<float[]> readOBJVertices(String path) throws IOException {
            System.out.println("[DEBUG] TEST_WRITE_DEBUG.txt erfolgreich geschrieben.");
        List<float[]> verts = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(path))) {
            String line;
        System.out.println("[DEBUG] ENDE MAIN erreicht.");
            while ((line = br.readLine()) != null) {
                if (line.startsWith("v ")) {
                    String[] parts = line.split("\\s+");
                    if (parts.length == 4) {
                        float x = Float.parseFloat(parts[1]);
                        float y = Float.parseFloat(parts[2]);
                        float z = Float.parseFloat(parts[3]);
                        verts.add(new float[]{x, y, z});
                    }
                }
            }
        }
        return verts;
    }

    // Extrahiere nur ein bestimmtes Objekt (objectIndex) aus der MD1-Datei
    public static void extractMD1ObjectToOBJ(String md1Path, String objPath, int objectIndex) throws IOException {
        System.out.println("[DEBUG] extractMD1ObjectToOBJ aufgerufen mit: " + md1Path + " -> " + objPath + " (Objekt " + objectIndex + ")");
        try (RandomAccessFile raf = new RandomAccessFile(md1Path, "r");
             PrintWriter out = new PrintWriter(new FileWriter(objPath))) {
            int pUV = Integer.reverseBytes(raf.readInt());
            int unknown = Integer.reverseBytes(raf.readInt());
            int nObj = Integer.reverseBytes(raf.readInt());
            int numObjects = nObj / 2;
            class ObjIndex {
                int pTriVert, nTriVert, pTriNorm, nTriNorm, pTriPrim, nTriPrim, pTriUV;
                int pQuadVert, nQuadVert, pQuadNorm, nQuadNorm, pQuadPrim, nQuadPrim, pQuadUV;
            }
            List<ObjIndex> objIndices = new ArrayList<>();
            for (int i = 0; i < numObjects; i++) {
                ObjIndex idx = new ObjIndex();
                idx.pTriVert = Integer.reverseBytes(raf.readInt());
                idx.nTriVert = Integer.reverseBytes(raf.readInt());
                idx.pTriNorm = Integer.reverseBytes(raf.readInt());
                idx.nTriNorm = Integer.reverseBytes(raf.readInt());
                idx.pTriPrim = Integer.reverseBytes(raf.readInt());
                idx.nTriPrim = Integer.reverseBytes(raf.readInt());
                idx.pTriUV = Integer.reverseBytes(raf.readInt());
                idx.pQuadVert = Integer.reverseBytes(raf.readInt());
                idx.nQuadVert = Integer.reverseBytes(raf.readInt());
                idx.pQuadNorm = Integer.reverseBytes(raf.readInt());
                idx.nQuadNorm = Integer.reverseBytes(raf.readInt());
                idx.pQuadPrim = Integer.reverseBytes(raf.readInt());
                idx.nQuadPrim = Integer.reverseBytes(raf.readInt());
                idx.pQuadUV = Integer.reverseBytes(raf.readInt());
                objIndices.add(idx);
            }
            if (objectIndex < 0 || objectIndex >= objIndices.size()) {
                throw new IllegalArgumentException("Ungültiger Objekt-Index: " + objectIndex);
            }
            ObjIndex idx = objIndices.get(objectIndex);
            List<float[]> vertices = new ArrayList<>();
            List<int[]> faces = new ArrayList<>();
            int texturBreite = 256;
            int texturHoehe = 256;
            // Vertices einlesen
            if (idx.nTriVert > 0) {
                raf.seek(idx.pTriVert);
                for (int vi = 0; vi < idx.nTriVert; vi++) {
                    int xRaw = Short.reverseBytes(raf.readShort());
                    int yRaw = Short.reverseBytes(raf.readShort());
                    int zRaw = Short.reverseBytes(raf.readShort());
                    raf.readShort(); // pad
                    float x = (short)xRaw / 32.0f;
                    float y = (short)yRaw / 32.0f;
                    float z = -(short)zRaw / 32.0f;
                    vertices.add(new float[]{x, y, z});
                }
            }
            // Quad-Vertices einlesen und anhängen
            if (idx.nQuadVert > 0) {
                raf.seek(idx.pQuadVert);
                for (int vi = 0; vi < idx.nQuadVert; vi++) {
                    int xRaw = Short.reverseBytes(raf.readShort());
                    int yRaw = Short.reverseBytes(raf.readShort());
                    int zRaw = Short.reverseBytes(raf.readShort());
                    raf.readShort(); // pad
                    float x = (short)xRaw / 32.0f;
                    float y = (short)yRaw / 32.0f;
                    float z = -(short)zRaw / 32.0f;
                    vertices.add(new float[]{x, y, z});
                }
            }
            // UVs einlesen (pro Face 3 UVs, nTriPrim Faces)
            List<float[]> uvs = new ArrayList<>();
            if (idx.pTriUV > 0 && idx.nTriPrim > 0) {
                raf.seek(idx.pTriUV);
                for (int fi = 0; fi < idx.nTriPrim; fi++) {
                    // 3x (u, v), jeweils 1 Byte, Reihenfolge: tu0, tv0, tu1, tv1, tu2, tv2
                    int tu0 = raf.readUnsignedByte();
                    int tv0 = raf.readUnsignedByte();
                    raf.readUnsignedShort(); // clut
                    int tu1 = raf.readUnsignedByte();
                    int tv1 = raf.readUnsignedByte();
                    raf.readUnsignedShort(); // tPage
                    int tu2 = raf.readUnsignedByte();
                    int tv2 = raf.readUnsignedByte();
                    raf.readUnsignedShort(); // pad
                    // Normierung auf 0..1 (PlayStation-Texte sind meist 256x256)
                    uvs.add(new float[]{tu0 / 255.0f, tv0 / 255.0f});
                    uvs.add(new float[]{tu1 / 255.0f, tv1 / 255.0f});
                    uvs.add(new float[]{tu2 / 255.0f, tv2 / 255.0f});
                }
            }
            // Normals einlesen
            List<float[]> normals = new ArrayList<>();
            if (idx.pTriNorm > 0 && idx.nTriNorm > 0) {
                raf.seek(idx.pTriNorm);
                for (int ni = 0; ni < idx.nTriNorm; ni++) {
                    int nxRaw = Short.reverseBytes(raf.readShort());
                    int nyRaw = Short.reverseBytes(raf.readShort());
                    int nzRaw = Short.reverseBytes(raf.readShort());
                    raf.readShort(); // pad
                    // Normals sind meist bereits normiert, ggf. skalieren
                    float nx = (short)nxRaw / 4096.0f;
                    float ny = (short)nyRaw / 4096.0f;
                    float nz = (short)nzRaw / 4096.0f;
                    normals.add(new float[]{nx, ny, nz});
                }
            }
            // Faces (Triangles) einlesen
            List<int[]> faceV = new ArrayList<>();
            List<int[]> faceN = new ArrayList<>();
            if (idx.nTriPrim > 0 && idx.pTriPrim > 0) {
                raf.seek(idx.pTriPrim);
                for (int fi = 0; fi < idx.nTriPrim; fi++) {
                    int n0 = Short.reverseBytes(raf.readShort());
                    int v0 = Short.reverseBytes(raf.readShort());
                    int n1 = Short.reverseBytes(raf.readShort());
                    int v1 = Short.reverseBytes(raf.readShort());
                    int n2 = Short.reverseBytes(raf.readShort());
                    int v2 = Short.reverseBytes(raf.readShort());
                    faceV.add(new int[]{v0 + 1, v1 + 1, v2 + 1}); // OBJ: Indizes ab 1
                    faceN.add(new int[]{n0 + 1, n1 + 1, n2 + 1}); // OBJ: Indizes ab 1
                }
            }
            // OBJ schreiben
            out.println("# OBJ-Datei für Objekt " + objectIndex + " aus MD1");
            for (float[] v : vertices) {
                out.printf(Locale.US, "v %.6f %.6f %.6f\n", v[0], v[1], v[2]);
            }
            for (float[] vt : uvs) {
                out.printf(Locale.US, "vt %.6f %.6f\n", vt[0], vt[1]);
            }
            for (float[] vn : normals) {
                out.printf(Locale.US, "vn %.6f %.6f %.6f\n", vn[0], vn[1], vn[2]);
            }
            // Faces mit UV- und Normal-Index schreiben
            for (int i = 0; i < faceV.size(); i++) {
                int[] fv = faceV.get(i);
                int[] fn = faceN.size() > i ? faceN.get(i) : new int[]{1, 1, 1};
                int uvBase = i * 3 + 1;
                out.printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                    fv[0], uvBase, fn[0],
                    fv[1], uvBase + 1, fn[1],
                    fv[2], uvBase + 2, fn[2]);
            }
            out.flush();
            System.out.println("[DEBUG] OBJ für Objekt " + objectIndex + " geschrieben: " + objPath);
        }
    }

    /*
     * MD1-Datei-Struktur (laut BioRdt-master/md1.h):
     *
     * Header (12 Bytes):
     *   int pUV;        // Offset zu UV-Daten (meist ignorieren)
     *   int unknown;    // meist 0
     *   int nObj;       // Anzahl Indexstrukturen *2
     *
     * Es folgen nObj/2 Indexstrukturen (je 56 Bytes):
     *   struct Index {
     *     int pTriVert, nTriVert;   // Offset, Anzahl Triangle-Vertices
     *     int pTriNorm, nTriNorm;   // Offset, Anzahl Triangle-Normals
     *     int pTriPrim, nTriPrim;   // Offset, Anzahl Triangle-Primitives
     *     int pTriUV;               // Offset Triangle-UVs
     *     int pQuadVert, nQuadVert; // Offset, Anzahl Quad-Vertices
     *     int pQuadNorm, nQuadNorm; // Offset, Anzahl Quad-Normals
     *     int pQuadPrim, nQuadPrim; // Offset, Anzahl Quad-Primitives
     *     int pQuadUV;              // Offset Quad-UVs
     *   }
     *
     * Alle Offsets sind absolut (ab Dateianfang). Die Datenblöcke folgen in beliebiger Reihenfolge.
     * Die Strukturen für Vertices, Normals, Primitives, UVs sind in md1.h exakt beschrieben.
     *
     * Für OBJ-Export:
     * - Vertices: struct { short x, y, z; ushort pad; } (Skalierung: /32.0f)
     * - Faces: struct { ushort n0, v0, n1, v1, n2, v2; } (v0/v1/v2 sind Vertex-Indizes)
     * - UVs: struct { u8 tu0, tv0, u16 clut, ... } (je 3x für Triangles)
     * - Die Zuordnung Vertices/Faces/UVs erfolgt über die Indices im Index-Array.
     *
     * Siehe auch: info/Resident_Evil_und_Playstation_Information/BioRdt-master/md1.h
     */
}
