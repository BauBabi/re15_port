package de.re15.extractors.emd;

import de.re15.extractors.util.LittleEndianDataView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

final class MeshParser {
    private MeshParser() {}

    static List<MeshData> parseMeshes(byte[] data, int meshOffset, TextureInfo texture) {
        LittleEndianDataView view = new LittleEndianDataView(data);
        int length = view.readInt(meshOffset);
        int rawCount = view.readInt(meshOffset + 8);
        int meshCount = rawCount / 2;
        int textureWidth = texture != null ? texture.getWidth() : 256;
        int textureHeight = texture != null ? texture.getHeight() : 256;
        int clutWidth = texture != null ? texture.getClutWidth() : textureWidth;

        List<MeshData> meshes = new ArrayList<>(meshCount);
        int base = meshOffset + 12;
        for (int meshIndex = 0; meshIndex < meshCount; meshIndex++) {
            int headerOffset = base + meshIndex * 56;
            int[] header = new int[14];
            ByteBuffer headerBuf = ByteBuffer.wrap(data, headerOffset, 56).order(ByteOrder.LITTLE_ENDIAN);
            for (int i = 0; i < 14; i++) {
                header[i] = headerBuf.getInt();
            }
            int triVertexOffset = header[0];
            int triVertexCount = header[1];
            int triOffset = header[4];
            int triCount = header[5];
            int triUvOffset = header[6];
            int quadVertexOffset = header[7];
            int quadVertexCount = header[8];
            int quadOffset = header[11];
            int quadCount = header[12];
            int quadUvOffset = header[13];

            List<float[]> vertexCoords = new ArrayList<>(triVertexCount + quadVertexCount);
            for (int vertIndex = 0; vertIndex < triVertexCount; vertIndex++) {
                int coordOffset = base + triVertexOffset + vertIndex * 8;
                short x = ByteBuffer.wrap(data, coordOffset, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                short y = ByteBuffer.wrap(data, coordOffset + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                short z = ByteBuffer.wrap(data, coordOffset + 4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
                float[] gltf = EmdMath.psxTripletToGltf(x, y, z);
                vertexCoords.add(gltf);
            }

            List<int[]> indices = new ArrayList<>();
            List<int[]> uvTriplets = new ArrayList<>();
            for (int triIndex = 0; triIndex < triCount; triIndex++) {
                int offsetIndex = base + triOffset + triIndex * 12;
                ByteBuffer buf = ByteBuffer.wrap(data, offsetIndex, 12).order(ByteOrder.LITTLE_ENDIAN);
                buf.getShort(); // n0
                int v0 = Short.toUnsignedInt(buf.getShort());
                buf.getShort();
                int v1 = Short.toUnsignedInt(buf.getShort());
                buf.getShort();
                int v2 = Short.toUnsignedInt(buf.getShort());
                indices.add(new int[]{v0, v1, v2});

                int uvOffset = base + triUvOffset + triIndex * 12;
                ByteBuffer uvBuf = ByteBuffer.wrap(data, uvOffset, 12).order(ByteOrder.LITTLE_ENDIAN);
                int u0 = Byte.toUnsignedInt(uvBuf.get());
                int t0 = Byte.toUnsignedInt(uvBuf.get());
                uvBuf.getShort();
                int u1 = Byte.toUnsignedInt(uvBuf.get());
                int t1 = Byte.toUnsignedInt(uvBuf.get());
                int clutId = Short.toUnsignedInt(uvBuf.getShort());
                int u2 = Byte.toUnsignedInt(uvBuf.get());
                int t2 = Byte.toUnsignedInt(uvBuf.get());
                uvBuf.getShort();
                uvTriplets.add(new int[]{u0, t0, u1, t1, u2, t2, clutId});
            }

            for (int quadIndex = 0; quadIndex < quadCount; quadIndex++) {
                int offsetIndex = base + quadOffset + quadIndex * 16;
                ByteBuffer buf = ByteBuffer.wrap(data, offsetIndex, 16).order(ByteOrder.LITTLE_ENDIAN);
                buf.getShort();
                int v0 = Short.toUnsignedInt(buf.getShort());
                buf.getShort();
                int v1 = Short.toUnsignedInt(buf.getShort());
                buf.getShort();
                int v2 = Short.toUnsignedInt(buf.getShort());
                buf.getShort();
                int v3 = Short.toUnsignedInt(buf.getShort());
                indices.add(new int[]{v0, v1, v3});
                indices.add(new int[]{v0, v2, v3});

                int uvOffset = base + quadUvOffset + quadIndex * 16;
                ByteBuffer uvBuf = ByteBuffer.wrap(data, uvOffset, 16).order(ByteOrder.LITTLE_ENDIAN);
                int u0 = Byte.toUnsignedInt(uvBuf.get());
                int t0 = Byte.toUnsignedInt(uvBuf.get());
                uvBuf.getShort();
                int u1 = Byte.toUnsignedInt(uvBuf.get());
                int t1 = Byte.toUnsignedInt(uvBuf.get());
                int clutId = Short.toUnsignedInt(uvBuf.getShort());
                int u2 = Byte.toUnsignedInt(uvBuf.get());
                int t2 = Byte.toUnsignedInt(uvBuf.get());
                uvBuf.getShort();
                int u3 = Byte.toUnsignedInt(uvBuf.get());
                int t3 = Byte.toUnsignedInt(uvBuf.get());
                uvBuf.getShort();
                // UV mapping must match vertex order: v0->uv0, v1->uv1, v3->uv3 for triangle 1
                uvTriplets.add(new int[]{u0, t0, u1, t1, u3, t3, clutId});
                uvTriplets.add(new int[]{u0, t0, u2, t2, u3, t3, clutId});
            }

            List<Float> positionList = new ArrayList<>(indices.size() * 9);
            List<Float> uvList = new ArrayList<>(indices.size() * 6);
            for (int primIndex = 0; primIndex < indices.size(); primIndex++) {
                int[] prim = indices.get(primIndex);
                int[] uv = uvTriplets.get(primIndex);
                int clutId = uv[6];
                int xOffset = clutWidth * (clutId & 0x3);
                float[][] uvCoords = new float[][]{
                        {(uv[0] + xOffset) / (float) textureWidth, uv[1] / (float) textureHeight},
                        {(uv[2] + xOffset) / (float) textureWidth, uv[3] / (float) textureHeight},
                        {(uv[4] + xOffset) / (float) textureWidth, uv[5] / (float) textureHeight},
                };
                for (int i = 0; i < prim.length; i++) {
                    float[] coord = vertexCoords.get(prim[i]);
                    positionList.add(coord[0]);
                    positionList.add(coord[1]);
                    positionList.add(coord[2]);
                    uvList.add(uvCoords[i][0]);
                    uvList.add(uvCoords[i][1]);
                }
            }

            float[] positions = new float[positionList.size()];
            for (int i = 0; i < positionList.size(); i++) {
                positions[i] = positionList.get(i);
            }
            float[] uvs = new float[uvList.size()];
            for (int i = 0; i < uvList.size(); i++) {
                uvs[i] = uvList.get(i);
            }

            meshes.add(new MeshData("mesh_" + meshIndex, positions, uvs));
        }

        return meshes;
    }
}
