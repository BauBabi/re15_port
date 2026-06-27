package de.re15.extractors.emd;

import java.util.Objects;

public final class AnimationClip {
    private final String name;
    private final int start;
    private final int frameCount;

    public AnimationClip(String name, int start, int frameCount) {
        this.name = Objects.requireNonNull(name, "name");
        this.start = start;
        this.frameCount = frameCount;
    }

    public String getName() {
        return name;
    }

    public int getStart() {
        return start;
    }

    public int getFrameCount() {
        return frameCount;
    }
}
