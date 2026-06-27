package de.re15.extractors.emd;

import java.util.Objects;

public final class TextureInfo {
    private final int width;
    private final int height;
    private final int clutCount;
    private final int clutWidth;
    private final byte[] rgba;
    private final String imageName;

    public TextureInfo(int width, int height, int clutCount, int clutWidth, byte[] rgba, String imageName) {
        this.width = width;
        this.height = height;
        this.clutCount = clutCount;
        this.clutWidth = clutWidth;
        this.rgba = Objects.requireNonNull(rgba, "rgba");
        this.imageName = Objects.requireNonNull(imageName, "imageName");
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public int getClutCount() {
        return clutCount;
    }

    public int getClutWidth() {
        return clutWidth;
    }

    public byte[] getRgba() {
        return rgba;
    }

    public String getImageName() {
        return imageName;
    }
}
