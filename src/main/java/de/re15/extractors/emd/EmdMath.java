package de.re15.extractors.emd;

import java.util.ArrayList;
import java.util.List;

public final class EmdMath {
    private EmdMath() {}

    public static float deg12ToRad(int value) {
        int v = value;
        if (v >= 2048) {
            v -= 4096;
        }
        return (float) (v / 4096.0 * (Math.PI * 2.0));
    }

    public static float normalizeS16(int value) {
        return value * EmdConstants.SCALE;
    }

    public static float[] quaternionFromEuler(float x, float y, float z) {
        /* BO-round 2026-05-29: YXZ Euler order (q = qy ⊗ qx ⊗ qz), matching the
         * engine's mat3_from_euler M = Ry·Rx·Rz (skeleton_common.c:114, ported
         * from RE2 RotMatrix.c) and the PSX RotMatrixYXZ BIOS. The old XYZ order
         * scrambled every bone with non-zero pitch+roll in the glTF clip exports
         * (which we use for clip identification). vs XYZ, only two sign terms
         * differ: qw's sx·sy·sz (−→+) and qz's sx·sy·cz (+→−); qx/qy identical. */
        float sx = (float) Math.sin(x / 2.0f);
        float cx = (float) Math.cos(x / 2.0f);
        float sy = (float) Math.sin(y / 2.0f);
        float cy = (float) Math.cos(y / 2.0f);
        float sz = (float) Math.sin(z / 2.0f);
        float cz = (float) Math.cos(z / 2.0f);
        float qw = cx * cy * cz + sx * sy * sz;
        float qx = sx * cy * cz + cx * sy * sz;
        float qy = cx * sy * cz - sx * cy * sz;
        float qz = cx * cy * sz - sx * sy * cz;
        return new float[]{qx, qy, qz, qw};
    }

    public static float[] psxAnglesToQuaternion(float ax, float ay, float az) {
        return quaternionFromEuler(ax, ay, az);
    }

    public static float[] psxTripletToGltf(int x, int y, int z) {
        return new float[]{normalizeS16(x), normalizeS16(y), normalizeS16(z)};
    }

    public static float[] psxTripletToGltfScaled(int x, int y, int z) {
        return psxTripletToGltf(x, y, z);
    }

    public static List<float[]> accumulateGlobalPositions(int[] parents, float[][] localPositions) {
        int count = localPositions.length;
        List<float[]> global = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            global.add(null);
        }
        for (int i = 0; i < count; i++) {
            resolve(i, parents, localPositions, global);
        }
        return global;
    }

    private static float[] resolve(int index, int[] parents, float[][] localPositions, List<float[]> global) {
        float[] cached = global.get(index);
        if (cached != null) {
            return cached;
        }
        float[] local = localPositions[index];
        int parent = parents[index];
        float[] result;
        if (parent < 0) {
            result = local.clone();
        } else {
            float[] parentPos = resolve(parent, parents, localPositions, global);
            result = new float[]{
                parentPos[0] + local[0],
                parentPos[1] + local[1],
                parentPos[2] + local[2]
            };
        }
        global.set(index, result);
        return result;
    }
}
