package de.re15.extractors.emd;

import java.util.List;
import java.util.Map;

public final class EmdParseResult {
    private final List<MeshData> meshes;
    private final Map<Integer, SkeletonData> skeletons;
    private final TextureInfo texture;

    public EmdParseResult(List<MeshData> meshes, Map<Integer, SkeletonData> skeletons, TextureInfo texture) {
        this.meshes = meshes;
        this.skeletons = skeletons;
        this.texture = texture;
    }

    public List<MeshData> getMeshes() {
        return meshes;
    }

    public Map<Integer, SkeletonData> getSkeletons() {
        return skeletons;
    }

    public TextureInfo getTexture() {
        return texture;
    }
}
