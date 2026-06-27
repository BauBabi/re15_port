package de.re15.extractors.emd;

import de.re15.extractors.util.LittleEndianDataView;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

public final class EmdFileParser {
    private EmdFileParser() {}

    public static EmdParseResult parse(Path emdPath) throws IOException {
        byte[] data = Files.readAllBytes(emdPath);
        LittleEndianDataView view = new LittleEndianDataView(data);
        int begin = view.readInt(0);
        int count = view.readInt(4);
        int[] offsets = new int[count];
        for (int i = 0; i < count; i++) {
            offsets[i] = view.readInt(begin + i * 4);
        }
        int[] animationOffsets = new int[3];
        int[] skeletonOffsets = new int[3];
        int meshOffset = 0;
        int textureOffset = 0;

        if (count == 4) {
            // PLD layout: [EDD, EMR, MD1, TIM]
            animationOffsets[0] = offsets[0];
            skeletonOffsets[0] = offsets[1];
            meshOffset = offsets[2];
            textureOffset = offsets[3];
        } else {
            for (int i = 0; i < 3; i++) {
                int animIndex = 1 + i * 2;
                int skeletonIndex = 2 + i * 2;
                if (animIndex < count) {
                    animationOffsets[i] = offsets[animIndex];
                }
                if (skeletonIndex < count) {
                    skeletonOffsets[i] = offsets[skeletonIndex];
                }
            }
            if (count > 7) {
                meshOffset = offsets[7];
            }
            if (count > 8) {
                textureOffset = offsets[8];
            }
        }
        String stem = stripExtension(emdPath.getFileName().toString());

        Optional<TextureInfo> textureOpt = TimParser.parseTim(data, textureOffset, stem);
        TextureInfo texture = textureOpt.orElse(null);
        List<MeshData> meshes = MeshParser.parseMeshes(data, meshOffset, texture);

        Map<Integer, SkeletonData> skeletons = new LinkedHashMap<>();
        for (int index = 0; index < 3; index++) {
            Optional<AnimationMetadata> animation = AnimationParser.parseAnimation(data, animationOffsets[index]);
            if (animation.isEmpty()) {
                continue;
            }
            Optional<SkeletonData> skeleton = SkeletonParser.parseSkeleton(data, skeletonOffsets[index], animation.get());
            final int skeletonIndex = index;
            skeleton.ifPresent(value -> skeletons.put(skeletonIndex, value));
        }
        return new EmdParseResult(meshes, skeletons, texture);
    }

    private static String stripExtension(String name) {
        int idx = name.lastIndexOf('.');
        return idx >= 0 ? name.substring(0, idx) : name;
    }
}
