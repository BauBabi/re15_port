package de.re15.extractors.emd;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

final class AnimationJsonWriter {
    private AnimationJsonWriter() {}

    static void writePrimarySummary(String stem, Path outputDir, SkeletonData skeleton) throws IOException {
        Map<String, Object> root = new LinkedHashMap<>();
        root.put("fps", EmdConstants.FPS);
        root.put("boneCount", skeleton.getBoneCount());
        List<Map<String, Object>> bones = new ArrayList<>();
        for (int idx = 0; idx < skeleton.getBoneCount(); idx++) {
            Map<String, Object> bone = new LinkedHashMap<>();
            bone.put("index", idx);
            bone.put("children", toList(skeleton.getChildren()[idx]));
            float[] rel = skeleton.getRelativePositions()[idx];
            bone.put("relativePosition", List.of(rel[0], rel[1], rel[2]));
            bones.add(bone);
        }
        root.put("bones", bones);

        List<Map<String, Object>> clips = new ArrayList<>();
        AnimationClip[] clipDefs = skeleton.getClips();
        int[] frames = skeleton.getFrames();
        for (AnimationClip clip : clipDefs) {
            Map<String, Object> clipJson = new LinkedHashMap<>();
            clipJson.put("name", clip.getName());
            clipJson.put("startFrame", clip.getStart());
            clipJson.put("frameCount", clip.getFrameCount());
            List<Map<String, Object>> framesJson = new ArrayList<>();
            for (int i = 0; i < clip.getFrameCount(); i++) {
                int frameIndex = clip.getStart() + i;
                int packed = frames[frameIndex];
                int keyIndex = packed & 0xFFF;
                Map<String, Object> frameJson = new LinkedHashMap<>();
                frameJson.put("frame", i);
                frameJson.put("keyIndex", keyIndex);
                float[] rootPos = skeleton.getKeyframePositions()[Math.min(keyIndex, skeleton.getKeyframePositions().length - 1)];
                frameJson.put("rootPosition", List.of(rootPos[0], rootPos[1], rootPos[2]));
                framesJson.add(frameJson);
            }
            clipJson.put("frames", framesJson);
            clips.add(clipJson);
        }
        root.put("clips", clips);
        String json = JsonUtil.toJson(root);
        Files.createDirectories(outputDir);
        Files.writeString(outputDir.resolve(stem + "_animations.json"), json);
    }

    private static List<Integer> toList(int[] values) {
        List<Integer> list = new ArrayList<>(values.length);
        for (int value : values) {
            list.add(value);
        }
        return list;
    }
}
