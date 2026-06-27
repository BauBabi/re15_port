package de.re15.extractors;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Verbesserte Version: Dynamische Erkennung von nObj, Vertex-Offset und Vertexanzahl aus MD1-Header/Index.
 * Protokolliert alle Schritte in CHAT_PROTOKOLL.txt.
 */
public class MD1VertexToObjConverter {

    private static final String PROTOKOLL = "CHAT_PROTOKOLL.txt";

    public static class Face {
        public int[] indices;
        public Face(int... idx) { this.indices = idx; }
    }

    public static class IndexEntry {
        public int pTriPrim, nTriPrim, pQuadPrim, nQuadPrim;
        public IndexEntry(int pTriPrim, int nTriPrim, int pQuadPrim, int nQuadPrim) {
            this.pTriPrim = pTriPrim;
            this.nTriPrim = nTriPrim;
            this.pQuadPrim = pQuadPrim;
            this.nQuadPrim = nQuadPrim;
        }
    }

    static {
        System.out.println("[DEBUG] Static Initializer MD1VertexToObjConverter");
        protokoll("[DEBUG] Static Initializer MD1VertexToObjConverter");
    }

    public static void main(String[] args) {
        System.out.println("[DEBUG] Start main");
        protokoll("[Order] Starte MD1VertexToObjConverter mit Parametern: " + String.join(" ", args));
        if (args.length < 2) {
            System.out.println("Usage: java MD1VertexToObjConverter <input_md1_file> <output_obj_file> [scale]");
            protokoll("[Error] Zu wenige Parameter übergeben.");
            return;
        }
        String md1Path = args[0];
        String objPath = args[1];
        float scale = 1.0f;
        int manualVertexOffset = -1;
        if (args.length >= 3) {
            try { scale = Float.parseFloat(args[2]); } catch (Exception ignored) {}
        }
        if (args.length >= 4) {
            try { manualVertexOffset = Integer.decode(args[3]); } catch (Exception ignored) {}
        }
        try {
            byte[] data = Files.readAllBytes(Paths.get(md1Path));
            if (data.length < 0x40) throw new IOException("Datei zu klein für MD1-Header");
            int nObj = ByteBuffer.wrap(data, 0x08, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
            int nIndex = nObj / 2;
            int headerSize = 0x0C;
            int indexSize = 0x38 * nIndex;
            int vertexOffset = manualVertexOffset >= 0 ? manualVertexOffset : headerSize + indexSize;
            int nVertices = 0;
            if (vertexOffset + 4 <= data.length) {
                nVertices = ByteBuffer.wrap(data, vertexOffset, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
            }
            if (nVertices <= 0 || vertexOffset + 4 + nVertices * 8 > data.length) {
                nVertices = (data.length - (vertexOffset + 4)) / 8;
                protokoll("[Warn] nVertices unplausibel, berechne aus Dateigröße: " + nVertices);
            }
            protokoll("[Info] nObj=" + nObj + ", nIndex=" + nIndex + ", headerSize=" + headerSize + ", indexSize=" + indexSize + ", vertexOffset=" + vertexOffset + ", nVertices=" + nVertices);
            List<float[]> vertices = new ArrayList<>();
            int offset = vertexOffset + 4;
            for (int i = 0; i < nVertices; i++) {
                if (offset + 8 > data.length) break;
                short x = ByteBuffer.wrap(data, offset, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                short y = ByteBuffer.wrap(data, offset + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                short z = ByteBuffer.wrap(data, offset + 4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                vertices.add(new float[]{x * scale, y * scale, z * scale});
                offset += 8;
            }
            protokoll("[Debug] Offset: " + offset + ", nVertices: " + nVertices + ", data.length: " + data.length);
            protokoll("[Debug] Vertices extrahiert: " + vertices.size());
            List<IndexEntry> indexEntries = extractIndexEntries(data, nIndex, headerSize);
            List<Face> faces = new ArrayList<>();
            for (IndexEntry entry : indexEntries) {
                faces.addAll(extractFaces(data, entry));
            }
            writeObj(objPath, vertices, faces);
            protokoll("[Result] Extraktion und Export abgeschlossen: " + objPath + ", Vertices: " + vertices.size() + ", Faces: " + faces.size());
            System.out.println("Extraktion und Export abgeschlossen: " + objPath);
        } catch (Exception e) {
            protokoll("[Error] " + e.getMessage());
            e.printStackTrace();
        }
    }

    /**
     * Extrahiert Vertexdaten aus einer MD1-Datei, dynamisch anhand Header/Index.
     */
    public static List<float[]> extractVertices(String md1Path, float scale) throws IOException {
        byte[] data = Files.readAllBytes(Paths.get(md1Path));
        if (data.length < 0x40) throw new IOException("Datei zu klein für MD1-Header");
        // nObj an Offset 0x08 (ULONG, Little Endian)
        int nObj = ByteBuffer.wrap(data, 0x08, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
        int nIndex = nObj / 2;
        int headerSize = 0x0C;
        int indexSize = 0x38 * nIndex;
        int vertexOffset = headerSize + indexSize;
        // nVertices steht meist nach Indexstrukturen (z.B. Offset 0x3C oder vertexOffset)
        int nVertices = 0;
        if (vertexOffset + 4 <= data.length) {
            nVertices = ByteBuffer.wrap(data, vertexOffset, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
        }
        // Fallback: Wenn nVertices unplausibel, so viele wie möglich extrahieren
        if (nVertices <= 0 || vertexOffset + 4 + nVertices * 8 > data.length) {
            nVertices = (data.length - (vertexOffset + 4)) / 8;
            protokoll("[Warn] nVertices unplausibel, berechne aus Dateigröße: " + nVertices);
        }
        protokoll("[Info] nObj=" + nObj + ", nIndex=" + nIndex + ", headerSize=" + headerSize + ", indexSize=" + indexSize + ", vertexOffset=" + vertexOffset + ", nVertices=" + nVertices);
        List<float[]> vertices = new ArrayList<>();
        int offset = vertexOffset + 4; // +4, da nVertices meist an vertexOffset steht
        for (int i = 0; i < nVertices; i++) {
            if (offset + 8 > data.length) break;
            short x = ByteBuffer.wrap(data, offset, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short y = ByteBuffer.wrap(data, offset + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short z = ByteBuffer.wrap(data, offset + 4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            // short pad = ByteBuffer.wrap(data, offset + 6, 2).order(ByteOrder.LITTLE_ENDIAN).getShort(); // Padding ignorieren
            vertices.add(new float[]{x * scale, y * scale, z * scale});
            offset += 8;
        }
        protokoll("[Debug] Offset: " + offset + ", nVertices: " + nVertices + ", data.length: " + data.length);
        protokoll("[Debug] Vertices extrahiert: " + vertices.size());
        protokoll("[Result] " + vertices.size() + " Vertices extrahiert.");
        return vertices;
    }

    public static List<IndexEntry> extractIndexEntries(byte[] data, int nIndex, int headerSize) {
        List<IndexEntry> entries = new ArrayList<>();
        for (int i = 0; i < nIndex; i++) {
            int base = headerSize + i * 0x38;
            int pTriPrim = ByteBuffer.wrap(data, base + 0x10, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
            int nTriPrim = ByteBuffer.wrap(data, base + 0x14, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            int pQuadPrim = ByteBuffer.wrap(data, base + 0x2C, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
            int nQuadPrim = ByteBuffer.wrap(data, base + 0x30, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            entries.add(new IndexEntry(pTriPrim, nTriPrim, pQuadPrim, nQuadPrim));
        }
        return entries;
    }

    public static List<Face> extractFaces(byte[] data, IndexEntry entry) {
        List<Face> faces = new ArrayList<>();
        // Triangles
        for (int i = 0; i < entry.nTriPrim; i++) {
            int off = entry.pTriPrim + i * 12; // 3x(USHORT n, USHORT v) = 12 Bytes
            int v0 = ByteBuffer.wrap(data, off + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            int v1 = ByteBuffer.wrap(data, off + 6, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            int v2 = ByteBuffer.wrap(data, off + 10, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            faces.add(new Face(v0, v1, v2));
        }
        // Quads
        for (int i = 0; i < entry.nQuadPrim; i++) {
            int off = entry.pQuadPrim + i * 16; // 4x(USHORT n, USHORT v) = 16 Bytes
            int v0 = ByteBuffer.wrap(data, off + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            int v1 = ByteBuffer.wrap(data, off + 6, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            int v2 = ByteBuffer.wrap(data, off + 10, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            int v3 = ByteBuffer.wrap(data, off + 14, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
            faces.add(new Face(v0, v1, v2, v3));
        }
        return faces;
    }

    /**
     * Schreibt die Vertexliste ins OBJ-Format
     */
    public static void writeObj(String objPath, List<float[]> vertices, List<Face> faces) throws IOException {
        try (PrintWriter out = new PrintWriter(new FileWriter(objPath))) {
            for (float[] v : vertices) {
                out.printf("v %.6f %.6f %.6f\n", v[0], v[1], v[2]);
            }
            for (Face f : faces) {
                out.print("f");
                for (int idx : f.indices) out.print(" " + (idx + 1)); // OBJ: 1-basiert
                out.println();
            }
        }
        protokoll("[Action] OBJ-Datei mit Vertices und Faces geschrieben: " + objPath);
    }

    /**
     * Protokolliert eine Zeile mit Zeitstempel in CHAT_PROTOKOLL.txt
     */
    private static void protokoll(String msg) {
        try (PrintWriter out = new PrintWriter(new FileWriter(PROTOKOLL, true))) {
            String ts = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date());
            out.println(ts + " " + msg);
        } catch (IOException ignored) {}
    }
}

