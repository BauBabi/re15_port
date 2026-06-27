package de.re15.extractors.emd;

import de.re15.extractors.util.LittleEndianDataView;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Deque;
import java.util.List;
import java.util.Optional;

final class SkeletonParser {
    private SkeletonParser() {}

    static Optional<SkeletonData> parseSkeleton(byte[] data, int offset, AnimationMetadata animation) {
        if (offset <= 0 || offset >= data.length) {
            return Optional.empty();
        }

        LittleEndianDataView view = new LittleEndianDataView(data);
        int bonesOffset = view.readUnsignedShort(offset);
        int framesOffset = view.readUnsignedShort(offset + 2);
        int boneCount = view.readUnsignedShort(offset + 4);
        int keyframeSize = view.readUnsignedShort(offset + 6);
        if (boneCount <= 0) {
            return Optional.empty();
        }

        float[][] relativePositions = new float[boneCount][3];
        int cursor = offset + 8;
        for (int i = 0; i < boneCount; i++) {
            short x = ByteBuffer.wrap(data, cursor, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short y = ByteBuffer.wrap(data, cursor + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short z = ByteBuffer.wrap(data, cursor + 4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            cursor += 6;
            relativePositions[i] = EmdMath.psxTripletToGltf(x, y, z);
        }

        int bonesOffsetAbs = offset + bonesOffset;
        int framesOffsetAbs = offset + framesOffset;

        List<List<Integer>> childrenLists = new ArrayList<>(boneCount);
        for (int i = 0; i < boneCount; i++) {
            childrenLists.add(new ArrayList<>());
        }

        for (int index = 0; index < boneCount; index++) {
            int rawOffset = bonesOffsetAbs + index * 4;
            if (rawOffset + 4 > data.length) {
                continue;
            }
            int meshCount = view.readUnsignedShort(rawOffset);
            int childOffset = view.readUnsignedShort(rawOffset + 2);
            if (meshCount <= 0) {
                continue;
            }
            int base = bonesOffsetAbs + childOffset;
            if (base >= data.length) {
                continue;
            }
            int available = Math.min(meshCount, data.length - base);
            List<Integer> filtered = new ArrayList<>();
            for (int i = 0; i < available; i++) {
                int childIndex = Byte.toUnsignedInt(data[base + i]);
                if (childIndex >= boneCount || childIndex == index) {
                    continue;
                }
                if (!filtered.contains(childIndex)) {
                    filtered.add(childIndex);
                }
            }
            childrenLists.set(index, filtered);
        }

        int[] parents = new int[boneCount];
        Arrays.fill(parents, -1);
        boolean[] visited = new boolean[boneCount];
        List<Integer> roots = new ArrayList<>();

        for (int candidate = 0; candidate < boneCount; candidate++) {
            if (!visited[candidate]) {
                roots.add(candidate);
                breadthFirstAssign(candidate, childrenLists, parents, visited);
            }
        }

        for (int root : roots) {
            parents[root] = -1;
        }

        int[] rootIndices = roots.stream().mapToInt(Integer::intValue).toArray();

        int totalKeyframes = Math.max(0, animation.getLastFrameIndex());
        float[][] keyframePositions = new float[totalKeyframes][3];
        float[][][] keyframeAngles = new float[totalKeyframes][boneCount][3];

        int pos = framesOffsetAbs;
        for (int keyIndex = 0; keyIndex < totalKeyframes; keyIndex++) {
            if (pos + 12 > data.length) {
                break;
            }
            short px = ByteBuffer.wrap(data, pos, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short py = ByteBuffer.wrap(data, pos + 2, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            short pz = ByteBuffer.wrap(data, pos + 4, 2).order(ByteOrder.LITTLE_ENDIAN).getShort();
            pos += 12; // skip speed vector as well
            float[] gltfPos = EmdMath.psxTripletToGltf(px, py, pz);
            keyframePositions[keyIndex] = gltfPos;

            int anglesSize = Math.max(0, keyframeSize - 12);
            if (pos + anglesSize > data.length) {
                break;
            }
            byte[] angleBytes = new byte[anglesSize];
            System.arraycopy(data, pos, angleBytes, 0, anglesSize);
            pos += anglesSize;

            if (angleBytes.length == 0) {
                continue;
            }
            int angleTriplets = (angleBytes.length * 8) / 36;
            if (angleTriplets <= 0) {
                continue;
            }
            BitReader reader = new BitReader(angleBytes);
            float[][] frameAngles = keyframeAngles[keyIndex];
            int limit = Math.min(angleTriplets, boneCount);
            for (int bone = 0; bone < limit; bone++) {
                float ax = EmdMath.deg12ToRad(reader.read(12));
                float ay = EmdMath.deg12ToRad(reader.read(12));
                float az = EmdMath.deg12ToRad(reader.read(12));
                frameAngles[bone][0] = ax;
                frameAngles[bone][1] = ay;
                frameAngles[bone][2] = az;
            }
        }

        int[][] childArrays = new int[boneCount][];
        for (int i = 0; i < boneCount; i++) {
            List<Integer> list = childrenLists.get(i);
            childArrays[i] = list.stream().mapToInt(Integer::intValue).toArray();
        }

        SkeletonData skeleton = new SkeletonData(
                boneCount,
                relativePositions,
                childArrays,
                parents,
                rootIndices,
                keyframePositions,
                keyframeAngles,
                animation.getFrames(),
                animation.getClips());
        return Optional.of(skeleton);
    }

    private static void breadthFirstAssign(int root,
                                           List<List<Integer>> children,
                                           int[] parents,
                                           boolean[] visited) {
        Deque<Integer> queue = new ArrayDeque<>();
        queue.add(root);
        visited[root] = true;
        parents[root] = -1;
        while (!queue.isEmpty()) {
            int current = queue.removeFirst();
            List<Integer> rawChildren = children.get(current);
            List<Integer> filtered = new ArrayList<>();
            for (int child : rawChildren) {
                if (!visited[child]) {
                    visited[child] = true;
                    parents[child] = current;
                    filtered.add(child);
                    queue.addLast(child);
                }
            }
            children.set(current, filtered);
        }
    }
}

