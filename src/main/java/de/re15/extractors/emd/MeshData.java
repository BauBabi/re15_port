package de.re15.extractors.emd;

import java.util.Arrays;
import java.util.Objects;

public final class MeshData {
    private final String name;
    private final float[] positions;
    private final float[] uvs;

    public MeshData(String name, float[] positions, float[] uvs) {
        this.name = Objects.requireNonNull(name, "name");
        this.positions = Objects.requireNonNull(positions, "positions");
        this.uvs = Objects.requireNonNull(uvs, "uvs");
    }

    public String getName() {
        return name;
    }

    public float[] getPositions() {
        return positions;
    }

    public float[] getUvs() {
        return uvs;
    }

    public int positionCount() {
        return positions.length / 3;
    }

    public int uvCount() {
        return uvs.length / 2;
    }

    @Override
    public String toString() {
        return "MeshData{" +
                "name='" + name + '\'' +
                ", positions=" + positions.length +
                ", uvs=" + uvs.length +
                '}';
    }
}
