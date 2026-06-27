package de.re15.extractors.emd;

import java.util.Arrays;
import java.util.Objects;

public final class SkeletonData {
    private final int boneCount;
    private final float[][] relativePositions; // [bone][xyz]
    private final int[][] children; // [bone][childIndices]
    private final int[] parents; // -1 for roots
    private final int[] rootIndices;
    private final float[][] keyframePositions; // [key][xyz]
    private final float[][][] keyframeAngles; // [key][bone][xyz]
    private final int[] frames; // packed frame metadata
    private final AnimationClip[] clips;

    public SkeletonData(
            int boneCount,
            float[][] relativePositions,
            int[][] children,
            int[] parents,
            int[] rootIndices,
            float[][] keyframePositions,
            float[][][] keyframeAngles,
            int[] frames,
            AnimationClip[] clips) {
        this.boneCount = boneCount;
        this.relativePositions = Objects.requireNonNull(relativePositions, "relativePositions");
        this.children = Objects.requireNonNull(children, "children");
        this.parents = Objects.requireNonNull(parents, "parents");
        this.rootIndices = Objects.requireNonNull(rootIndices, "rootIndices");
        this.keyframePositions = Objects.requireNonNull(keyframePositions, "keyframePositions");
        this.keyframeAngles = Objects.requireNonNull(keyframeAngles, "keyframeAngles");
        this.frames = Objects.requireNonNull(frames, "frames");
        this.clips = Objects.requireNonNull(clips, "clips");
    }

    public int getBoneCount() {
        return boneCount;
    }

    public float[][] getRelativePositions() {
        return relativePositions;
    }

    public int[][] getChildren() {
        return children;
    }

    public int[] getParents() {
        return parents;
    }

    public int[] getRootIndices() {
        return rootIndices;
    }

    public float[][] getKeyframePositions() {
        return keyframePositions;
    }

    public float[][][] getKeyframeAngles() {
        return keyframeAngles;
    }

    public int[] getFrames() {
        return frames;
    }

    public AnimationClip[] getClips() {
        return clips;
    }

    @Override
    public String toString() {
        return "SkeletonData{" +
                "boneCount=" + boneCount +
                ", clips=" + Arrays.toString(clips) +
                '}';
    }
}
