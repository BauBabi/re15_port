package de.re15.extractors.md1;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Minimal reader for Capcom's MD1 format based on lib_bio's reference implementation.
 *
 * <p>The binary layout mirrors the structures described in lib_bio/lib_md1.py. The offsets stored in the
 * object headers are absolute within the MD1 payload (i.e. measured from the file start + 12 bytes header size).</p>
 */
public final class MD1File {

    public static final class Header {
        public final int length;
        public final int unknown;
        public final int objectCount;

        private Header(int length, int unknown, int objectCount) {
            this.length = length;
            this.unknown = unknown;
            this.objectCount = objectCount;
        }
    }

    public static final class Vertex {
        public final short x;
        public final short y;
        public final short z;

        private Vertex(short x, short y, short z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    public static final class Normal {
        public final short x;
        public final short y;
        public final short z;

        private Normal(short x, short y, short z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    public static final class Triangle {
        public final int n0, v0, n1, v1, n2, v2;

        private Triangle(int n0, int v0, int n1, int v1, int n2, int v2) {
            this.n0 = n0;
            this.v0 = v0;
            this.n1 = n1;
            this.v1 = v1;
            this.n2 = n2;
            this.v2 = v2;
        }
    }

    public static final class Quad {
        public final int n0, v0, n1, v1, n2, v2, n3, v3;

        private Quad(int n0, int v0, int n1, int v1, int n2, int v2, int n3, int v3) {
            this.n0 = n0;
            this.v0 = v0;
            this.n1 = n1;
            this.v1 = v1;
            this.n2 = n2;
            this.v2 = v2;
            this.n3 = n3;
            this.v3 = v3;
        }
    }

    public static final class TriangleUV {
        public final int u0, v0, u1, v1, u2, v2;
        public final int clut;
        public final int page;

        private TriangleUV(int u0, int v0, int clut, int u1, int v1, int page, int u2, int v2) {
            this.u0 = u0;
            this.v0 = v0;
            this.clut = clut;
            this.u1 = u1;
            this.v1 = v1;
            this.page = page;
            this.u2 = u2;
            this.v2 = v2;
        }
    }

    public static final class QuadUV {
        public final int u0, v0, u1, v1, u2, v2, u3, v3;
        public final int clut;
        public final int page;

        private QuadUV(int u0, int v0, int clut, int u1, int v1, int page, int u2, int v2, int u3, int v3) {
            this.u0 = u0;
            this.v0 = v0;
            this.clut = clut;
            this.u1 = u1;
            this.v1 = v1;
            this.page = page;
            this.u2 = u2;
            this.v2 = v2;
            this.u3 = u3;
            this.v3 = v3;
        }
    }

    public static final class Mesh {
        public final List<Vertex> vertices;
        public final List<Normal> normals;
        public final List<Triangle> triangles;
        public final List<Quad> quads;
        public final List<TriangleUV> triangleUVs;
        public final List<QuadUV> quadUVs;
        public final int triVertexCount;
        public final int quadVertexBase;
        public final int triNormalCount;
        public final int quadNormalBase;

        private Mesh(List<Vertex> vertices,
                     List<Normal> normals,
                     List<Triangle> triangles,
                     List<Quad> quads,
                     List<TriangleUV> triangleUVs,
                     List<QuadUV> quadUVs,
                     int triVertexCount,
                     int quadVertexBase,
                     int triNormalCount,
                     int quadNormalBase) {
            this.vertices = vertices;
            this.normals = normals;
            this.triangles = triangles;
            this.quads = quads;
            this.triangleUVs = triangleUVs;
            this.quadUVs = quadUVs;
            this.triVertexCount = triVertexCount;
            this.quadVertexBase = quadVertexBase;
            this.triNormalCount = triNormalCount;
            this.quadNormalBase = quadNormalBase;
        }
    }

    public final Header header;
    public final List<Mesh> meshes;

    private MD1File(Header header, List<Mesh> meshes) {
        this.header = header;
        this.meshes = Collections.unmodifiableList(meshes);
    }

    public static MD1File read(Path path) throws IOException {
        byte[] data = Files.readAllBytes(path);
        if (data.length < 12) {
            throw new IOException("MD1 file too small: " + data.length);
        }
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        Header header = new Header(buffer.getInt(), buffer.getInt(), buffer.getInt());
        int meshCount = header.objectCount / 2;
        if (meshCount <= 0) {
            return new MD1File(header, List.of());
        }
        List<Mesh> meshes = new ArrayList<>(meshCount);
        for (int i = 0; i < meshCount; i++) {
            int headerOffset = 12 + i * 56; // 14 uint32 entries per mesh
            meshes.add(readMesh(data, headerOffset));
        }
        return new MD1File(header, meshes);
    }

    private static Mesh readMesh(byte[] data, int headerOffset) throws IOException {
        if (headerOffset + 56 > data.length) {
            throw new IOException("MD1 mesh header outside file bounds at " + headerOffset);
        }
        ByteBuffer bb = slice(data, headerOffset, 56);
        int tVertexOffset = bb.getInt();
        int tVertexCount = bb.getInt();
        int tNormalOffset = bb.getInt();
        int tNormalCount = bb.getInt();
        int tFaceOffset = bb.getInt();
        int tFaceCount = bb.getInt();
        int tUvOffset = bb.getInt();
        int qVertexOffset = bb.getInt();
        int qVertexCount = bb.getInt();
        int qNormalOffset = bb.getInt();
        int qNormalCount = bb.getInt();
        int qFaceOffset = bb.getInt();
        int qFaceCount = bb.getInt();
        int qUvOffset = bb.getInt();

        List<Vertex> vertices = new ArrayList<>();
        List<Normal> normals = new ArrayList<>();
        List<Triangle> triangles = new ArrayList<>(tFaceCount);
        List<Quad> quads = new ArrayList<>(qFaceCount);
        List<TriangleUV> triUVs = new ArrayList<>(tFaceCount);
        List<QuadUV> quadUVs = new ArrayList<>(qFaceCount);

        final int base = 12; // header size in bytes

        int triVertexCount = 0;
        int triNormalCount = 0;

        if (tFaceCount > 0) {
            List<Vertex> triVertices = readVertices(data, base + tVertexOffset, tVertexCount);
            triVertexCount = triVertices.size();
            vertices.addAll(triVertices);

            List<Normal> triNormals = readNormals(data, base + tNormalOffset, tNormalCount);
            triNormalCount = triNormals.size();
            normals.addAll(triNormals);

            triangles.addAll(readTriangles(data, base + tFaceOffset, tFaceCount));
            triUVs.addAll(readTriangleUVs(data, base + tUvOffset, tFaceCount));
        }

        int quadVertexBase = triVertexCount;
        int quadNormalBase = triNormalCount;

        if (qFaceCount > 0) {
            List<Vertex> quadVertices = readVertices(data, base + qVertexOffset, qVertexCount);
            if (triVertexCount > 0 && qVertexOffset == tVertexOffset && qVertexCount == tVertexCount) {
                quadVertexBase = 0;
            } else {
                quadVertexBase = vertices.size();
                vertices.addAll(quadVertices);
            }

            List<Normal> quadNormals = readNormals(data, base + qNormalOffset, qNormalCount);
            if (triNormalCount > 0 && qNormalOffset == tNormalOffset && qNormalCount == tNormalCount) {
                quadNormalBase = 0;
            } else {
                quadNormalBase = normals.size();
                normals.addAll(quadNormals);
            }

            quads.addAll(readQuads(data, base + qFaceOffset, qFaceCount));
            quadUVs.addAll(readQuadUVs(data, base + qUvOffset, qFaceCount));
        }


        return new Mesh(vertices, normals, triangles, quads, triUVs, quadUVs, triVertexCount, quadVertexBase, triNormalCount, quadNormalBase);
    }

    private static List<Vertex> readVertices(byte[] data, int offset, int count) throws IOException {
        int stride = 8;
        int end = offset + count * stride;
        if (end > data.length) {
            throw new IOException("Vertex block outside file bounds");
        }
        ByteBuffer bb = slice(data, offset, count * stride);
        List<Vertex> verts = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            short x = bb.getShort();
            short y = bb.getShort();
            short z = bb.getShort();
            bb.getShort(); // padding
            verts.add(new Vertex(x, y, z));
        }
        return verts;
    }

    private static List<Normal> readNormals(byte[] data, int offset, int count) throws IOException {
        int stride = 8;
        int end = offset + count * stride;
        if (end > data.length) {
            throw new IOException("Normal block outside file bounds");
        }
        ByteBuffer bb = slice(data, offset, count * stride);
        List<Normal> normals = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            short x = bb.getShort();
            short y = bb.getShort();
            short z = bb.getShort();
            bb.getShort();
            normals.add(new Normal(x, y, z));
        }
        return normals;
    }

    private static List<Triangle> readTriangles(byte[] data, int offset, int count) throws IOException {
        int stride = 12;
        int end = offset + count * stride;
        if (end > data.length) {
            throw new IOException("Triangle block outside file bounds");
        }
        ByteBuffer bb = slice(data, offset, count * stride);
        List<Triangle> tris = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            int n0 = bb.getShort() & 0xFFFF;
            int v0 = bb.getShort() & 0xFFFF;
            int n1 = bb.getShort() & 0xFFFF;
            int v1 = bb.getShort() & 0xFFFF;
            int n2 = bb.getShort() & 0xFFFF;
            int v2 = bb.getShort() & 0xFFFF;
            tris.add(new Triangle(n0, v0, n1, v1, n2, v2));
        }
        return tris;
    }

    private static List<Quad> readQuads(byte[] data, int offset, int count) throws IOException {
        int stride = 16;
        int end = offset + count * stride;
        if (end > data.length) {
            throw new IOException("Quad block outside file bounds");
        }
        ByteBuffer bb = slice(data, offset, count * stride);
        List<Quad> quads = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            int n0 = bb.getShort() & 0xFFFF;
            int v0 = bb.getShort() & 0xFFFF;
            int n1 = bb.getShort() & 0xFFFF;
            int v1 = bb.getShort() & 0xFFFF;
            int n2 = bb.getShort() & 0xFFFF;
            int v2 = bb.getShort() & 0xFFFF;
            int n3 = bb.getShort() & 0xFFFF;
            int v3 = bb.getShort() & 0xFFFF;
            quads.add(new Quad(n0, v0, n1, v1, n2, v2, n3, v3));
        }
        return quads;
    }

    private static List<TriangleUV> readTriangleUVs(byte[] data, int offset, int count) throws IOException {
        int stride = 12;
        int end = offset + count * stride;
        if (end > data.length) {
            throw new IOException("Triangle UV block outside file bounds");
        }
        ByteBuffer bb = slice(data, offset, count * stride);
        List<TriangleUV> uvs = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            int u0 = bb.get() & 0xFF;
            int v0 = bb.get() & 0xFF;
            int clut = bb.getShort() & 0xFFFF;
            int u1 = bb.get() & 0xFF;
            int v1 = bb.get() & 0xFF;
            int page = bb.getShort() & 0xFFFF;
            int u2 = bb.get() & 0xFF;
            int v2 = bb.get() & 0xFF;
            bb.getShort(); // padding
            uvs.add(new TriangleUV(u0, v0, clut, u1, v1, page, u2, v2));
        }
        return uvs;
    }

    private static List<QuadUV> readQuadUVs(byte[] data, int offset, int count) throws IOException {
        int stride = 16;
        int end = offset + count * stride;
        if (end > data.length) {
            throw new IOException("Quad UV block outside file bounds");
        }
        ByteBuffer bb = slice(data, offset, count * stride);
        List<QuadUV> uvs = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            int u0 = bb.get() & 0xFF;
            int v0 = bb.get() & 0xFF;
            int clut = bb.getShort() & 0xFFFF;
            int u1 = bb.get() & 0xFF;
            int v1 = bb.get() & 0xFF;
            int page = bb.getShort() & 0xFFFF;
            int u2 = bb.get() & 0xFF;
            int v2 = bb.get() & 0xFF;
            bb.getShort();
            int u3 = bb.get() & 0xFF;
            int v3 = bb.get() & 0xFF;
            bb.getShort();
            uvs.add(new QuadUV(u0, v0, clut, u1, v1, page, u2, v2, u3, v3));
        }
        return uvs;
    }

    private static ByteBuffer slice(byte[] data, int offset, int length) {
        ByteBuffer bb = ByteBuffer.wrap(data, offset, length);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb;
    }
}
