package de.re15.extractors.emd;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.Map;

public final class EmdGltfExtractor {
    private final GltfExporter exporter = new GltfExporter();

    public void extract(Path emdPath, Path outputDir) throws IOException {
        Files.createDirectories(outputDir);
        EmdParseResult parseResult = EmdFileParser.parse(emdPath);
        if (parseResult.getSkeletons().isEmpty()) {
            System.out.println("[EmdGltfExtractor] Keine Skeleton-Daten in " + emdPath);
            return;
        }
        int primarySid = parseResult.getSkeletons().keySet().stream().sorted().findFirst().orElse(0);
        SkeletonData primarySkeleton = parseResult.getSkeletons().get(primarySid);
        Map<Integer, SkeletonData> primaryMap = new LinkedHashMap<>();
        primaryMap.put(primarySid, primarySkeleton);

        String stem = stripExtension(emdPath.getFileName().toString());
        // Write directly into outputDir (no additional subfolder)
        Files.createDirectories(outputDir);

        TextureWriter.writeTexture(parseResult.getTexture(), outputDir);

        GltfExportResult mainDoc = exporter.export(stem, parseResult.getMeshes(), parseResult.getSkeletons(), parseResult.getTexture(), true, null);
        Files.writeString(outputDir.resolve(stem + ".gltf"), mainDoc.gltfJson());
        Files.write(outputDir.resolve(stem + ".bin"), mainDoc.binData());
        AnimationJsonWriter.writePrimarySummary(stem, outputDir, primarySkeleton);

        for (AnimationClip clip : primarySkeleton.getClips()) {
            if (clip.getFrameCount() <= 0) {
                continue;
            }
            String clipStem = stem + "_" + clip.getName();
            GltfExportResult clipDoc = exporter.export(clipStem, parseResult.getMeshes(), primaryMap, parseResult.getTexture(), true, clip);
            Files.writeString(outputDir.resolve(clipStem + ".gltf"), clipDoc.gltfJson());
            Files.write(outputDir.resolve(clipStem + ".bin"), clipDoc.binData());
        }
    }

    private static String stripExtension(String name) {
        int idx = name.lastIndexOf('.');
        return idx >= 0 ? name.substring(0, idx) : name;
    }
}
