package de.re15.extractors.emd;

final class AnimationMetadata {
    private final int[] frames;
    private final AnimationClip[] clips;
    private final int lastFrameIndex;

    AnimationMetadata(int[] frames, AnimationClip[] clips, int lastFrameIndex) {
        this.frames = frames;
        this.clips = clips;
        this.lastFrameIndex = lastFrameIndex;
    }

    public int[] getFrames() {
        return frames;
    }

    public AnimationClip[] getClips() {
        return clips;
    }

    public int getLastFrameIndex() {
        return lastFrameIndex;
    }
}
