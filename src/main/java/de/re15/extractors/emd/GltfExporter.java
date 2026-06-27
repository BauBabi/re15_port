package de.re15.extractors.emd;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public final class GltfExporter {
    private static final float[] ROOT_ROTATION = {1.0f, 0.0f, 0.0f, 0.0f};

    public GltfExportResult export(String stem,
                                   List<MeshData> meshes,
                                   Map<Integer, SkeletonData> skeletonEntries,
                                   TextureInfo texture,
                                   boolean includeAnimations,
                                   AnimationClip clipFilter) {
        GltfBufferBuilder buffer = new GltfBufferBuilder();
        List<Map<String, Object>> bufferViews = new ArrayList<>();
        List<Map<String, Object>> accessors = new ArrayList<>();
        List<Map<String, Object>> gltfMeshes = new ArrayList<>();
        Integer materialIndex = texture != null ? 0 : null;

        for (MeshData mesh : meshes) {
            if (mesh.getPositions().length == 0) {
                continue;
            }
            int posOffset = buffer.add(mesh.getPositions(), 4);
            int posViewIndex = bufferViews.size();
            Map<String, Object> posView = new LinkedHashMap<>();
            posView.put("buffer", 0);
            posView.put("byteOffset", posOffset);
            posView.put("byteLength", mesh.getPositions().length * 4);
            bufferViews.add(posView);

            float[] positions = mesh.getPositions();
            float minX = Float.POSITIVE_INFINITY, minY = Float.POSITIVE_INFINITY, minZ = Float.POSITIVE_INFINITY;
            float maxX = Float.NEGATIVE_INFINITY, maxY = Float.NEGATIVE_INFINITY, maxZ = Float.NEGATIVE_INFINITY;
            for (int i = 0; i < positions.length; i += 3) {
                float x = positions[i];
                float y = positions[i + 1];
                float z = positions[i + 2];
                minX = Math.min(minX, x);
                minY = Math.min(minY, y);
                minZ = Math.min(minZ, z);
                maxX = Math.max(maxX, x);
                maxY = Math.max(maxY, y);
                maxZ = Math.max(maxZ, z);
            }
            int posAccessorIndex = accessors.size();
            Map<String, Object> posAccessor = new LinkedHashMap<>();
            posAccessor.put("bufferView", posViewIndex);
            posAccessor.put("componentType", 5126);
            posAccessor.put("count", mesh.positionCount());
            posAccessor.put("type", "VEC3");
            posAccessor.put("min", List.of(minX, minY, minZ));
            posAccessor.put("max", List.of(maxX, maxY, maxZ));
            accessors.add(posAccessor);

            int uvOffset = buffer.add(mesh.getUvs(), 4);
            int uvViewIndex = bufferViews.size();
            Map<String, Object> uvView = new LinkedHashMap<>();
            uvView.put("buffer", 0);
            uvView.put("byteOffset", uvOffset);
            uvView.put("byteLength", mesh.getUvs().length * 4);
            bufferViews.add(uvView);

            int uvAccessorIndex = accessors.size();
            Map<String, Object> uvAccessor = new LinkedHashMap<>();
            uvAccessor.put("bufferView", uvViewIndex);
            uvAccessor.put("componentType", 5126);
            uvAccessor.put("count", mesh.uvCount());
            uvAccessor.put("type", "VEC2");
            accessors.add(uvAccessor);

            Map<String, Object> primitive = new LinkedHashMap<>();
            Map<String, Object> attributes = new LinkedHashMap<>();
            attributes.put("POSITION", posAccessorIndex);
            attributes.put("TEXCOORD_0", uvAccessorIndex);
            primitive.put("attributes", attributes);
            if (materialIndex != null) {
                primitive.put("material", materialIndex);
            }
            Map<String, Object> meshJson = new LinkedHashMap<>();
            meshJson.put("name", mesh.getName());
            meshJson.put("primitives", List.of(primitive));
            gltfMeshes.add(meshJson);
        }

        List<Map<String, Object>> nodes = new ArrayList<>();
        int rootIndex = nodes.size();
        Map<String, Object> rootNode = new LinkedHashMap<>();
        rootNode.put("name", stem + "_root");
        rootNode.put("rotation", List.of(ROOT_ROTATION[0], ROOT_ROTATION[1], ROOT_ROTATION[2], ROOT_ROTATION[3]));
        nodes.add(rootNode);

        Map<Integer, Integer> skeletonRootNodes = new HashMap<>();
        Map<String, Integer> nodeMap = new HashMap<>();
        List<CombinedBone> combinedBones = new ArrayList<>();
        boolean singleSkeleton = skeletonEntries.size() <= 1;
        int meshAssignmentSid = skeletonEntries.keySet().stream().sorted().findFirst().orElse(0);

        if (skeletonEntries.isEmpty()) {
            List<Integer> children = new ArrayList<>();
            for (int meshIndex = 0; meshIndex < gltfMeshes.size(); meshIndex++) {
                int nodeIdx = nodes.size();
                Map<String, Object> meshNode = new LinkedHashMap<>();
                meshNode.put("name", gltfMeshes.get(meshIndex).getOrDefault("name", "mesh_" + meshIndex));
                meshNode.put("mesh", meshIndex);
                nodes.add(meshNode);
                children.add(nodeIdx);
            }
            if (!children.isEmpty()) {
                rootNode.put("children", children);
            }
        } else {
            int meshIndex = 0;
            for (Map.Entry<Integer, SkeletonData> entry : skeletonEntries.entrySet()) {
                int sid = entry.getKey();
                SkeletonData skeleton = entry.getValue();
                int skeletonRootIdx;
                if (singleSkeleton) {
                    skeletonRootIdx = rootIndex;
                } else {
                    skeletonRootIdx = nodes.size();
                    Map<String, Object> skeletonRoot = new LinkedHashMap<>();
                    skeletonRoot.put("name", "sk" + sid + "_root");
                    skeletonRoot.put("rotation", List.of(ROOT_ROTATION[0], ROOT_ROTATION[1], ROOT_ROTATION[2], ROOT_ROTATION[3]));
                    nodes.add(skeletonRoot);
                    rootNode.computeIfAbsent("children", key -> new ArrayList<Integer>());
                    ((List<Integer>) rootNode.get("children")).add(skeletonRootIdx);
                }
                skeletonRootNodes.put(sid, skeletonRootIdx);

                for (int boneIdx = 0; boneIdx < skeleton.getBoneCount(); boneIdx++) {
                    int nodeIdx = nodes.size();
                    Map<String, Object> boneNode = new LinkedHashMap<>();
                    boneNode.put("name", singleSkeleton ? "bone_" + boneIdx : "sk" + sid + "_bone_" + boneIdx);
                    float[] t = skeleton.getRelativePositions()[boneIdx];
                    boneNode.put("translation", List.of(t[0], t[1], t[2]));
                    nodes.add(boneNode);
                    nodeMap.put(key(sid, boneIdx), nodeIdx);
                    combinedBones.add(new CombinedBone(sid, boneIdx, nodeIdx));
                }
                for (int boneIdx = 0; boneIdx < skeleton.getBoneCount(); boneIdx++) {
                    int nodeIdx = nodeMap.get(key(sid, boneIdx));
                    int parent = skeleton.getParents()[boneIdx];
                    if (parent < 0) {
                        nodes.get(skeletonRootIdx).computeIfAbsent("children", k -> new ArrayList<Integer>());
                        ((List<Integer>) nodes.get(skeletonRootIdx).get("children")).add(nodeIdx);
                    } else {
                        int parentNode = nodeMap.get(key(sid, parent));
                        nodes.get(parentNode).computeIfAbsent("children", k -> new ArrayList<Integer>());
                        ((List<Integer>) nodes.get(parentNode).get("children")).add(nodeIdx);
                    }
                }

                if (sid == meshAssignmentSid) {
                    for (CombinedBone bone : combinedBones) {
                        if (bone.skeletonId() != sid) {
                            continue;
                        }
                        if (meshIndex < gltfMeshes.size()) {
                            nodes.get(bone.nodeIndex()).put("mesh", meshIndex);
                            meshIndex++;
                        }
                    }
                }
            }
        }

        List<Map<String, Object>> animations = new ArrayList<>();
        if (includeAnimations && !skeletonEntries.isEmpty()) {
            int primarySid = skeletonEntries.keySet().stream().sorted().findFirst().orElse(0);
            SkeletonData primarySkeleton = skeletonEntries.get(primarySid);
            int[] frames = primarySkeleton.getFrames();
            AnimationClip[] clips = primarySkeleton.getClips();

            for (AnimationClip clip : clips) {
                if (clipFilter != null && !clip.getName().equals(clipFilter.getName())) {
                    continue;
                }
                if (clip.getFrameCount() <= 0) {
                    continue;
                }
                int frameCount = clip.getFrameCount();
                float[] timeSamples = new float[frameCount];
                for (int i = 0; i < frameCount; i++) {
                    timeSamples[i] = i / EmdConstants.FPS;
                }
                int timeOffset = buffer.add(timeSamples, 4);
                int timeViewIndex = bufferViews.size();
                Map<String, Object> timeView = new LinkedHashMap<>();
                timeView.put("buffer", 0);
                timeView.put("byteOffset", timeOffset);
                timeView.put("byteLength", frameCount * 4);
                bufferViews.add(timeView);

                int timeAccessorIndex = accessors.size();
                Map<String, Object> timeAccessor = new LinkedHashMap<>();
                timeAccessor.put("bufferView", timeViewIndex);
                timeAccessor.put("componentType", 5126);
                timeAccessor.put("count", frameCount);
                timeAccessor.put("type", "SCALAR");
                timeAccessor.put("min", List.of(0.0f));
                timeAccessor.put("max", List.of(timeSamples[frameCount - 1]));
                accessors.add(timeAccessor);

                List<Map<String, Object>> samplers = new ArrayList<>();
                List<Map<String, Object>> channels = new ArrayList<>();

                Map<String, float[]> rotationState = new HashMap<>();
                Map<Integer, float[]> translationState = new HashMap<>();
                for (Map.Entry<Integer, SkeletonData> stateEntry : skeletonEntries.entrySet()) {
                    int sid = stateEntry.getKey();
                    SkeletonData skeleton = stateEntry.getValue();
                    for (int boneIdx = 0; boneIdx < skeleton.getBoneCount(); boneIdx++) {
                        rotationState.put(key(sid, boneIdx), new float[]{0f, 0f, 0f, 1f});
                    }
                    float[] initialPos = skeleton.getKeyframePositions().length > 0
                            ? skeleton.getKeyframePositions()[0]
                            : new float[]{0f, 0f, 0f};
                    translationState.put(sid, initialPos.clone());
                }

                Map<Integer, float[]> translationBuffers = new LinkedHashMap<>();
                Map<Integer, Integer> translationCursor = new HashMap<>();
                for (int sid : skeletonEntries.keySet()) {
                    translationBuffers.put(sid, new float[frameCount * 3]);
                    translationCursor.put(sid, 0);
                }

                Map<Integer, float[]> rotationBuffersMap = new LinkedHashMap<>();
                Map<Integer, Integer> rotationCursor = new HashMap<>();
                for (CombinedBone bone : combinedBones) {
                    rotationBuffersMap.put(bone.nodeIndex(), new float[frameCount * 4]);
                    rotationCursor.put(bone.nodeIndex(), 0);
                }

                for (int frameOffset = 0; frameOffset < frameCount; frameOffset++) {
                    int packedFrame = frames[clip.getStart() + frameOffset];
                    int frameSid = (packedFrame >> 12) & 0xF;
                    int keyIndex = packedFrame & 0xFFF;
                    SkeletonData active = skeletonEntries.get(frameSid);
                    if (active != null) {
                        if (keyIndex < active.getKeyframeAngles().length) {
                            float[][] angleList = active.getKeyframeAngles()[keyIndex];
                            int limit = Math.min(angleList.length, active.getBoneCount());
                            for (int boneIdx = 0; boneIdx < limit; boneIdx++) {
                                float[] angles = angleList[boneIdx];
                                float[] quat = EmdMath.psxAnglesToQuaternion(angles[0], angles[1], angles[2]);
                                rotationState.put(key(frameSid, boneIdx), quat);
                            }
                        }
                        if (keyIndex < active.getKeyframePositions().length) {
                            float[] pos = active.getKeyframePositions()[keyIndex];
                            translationState.put(frameSid, new float[]{pos[0], pos[1], pos[2]});
                        }
                    }

                    for (Map.Entry<Integer, float[]> entryBuf : translationBuffers.entrySet()) {
                        int sid = entryBuf.getKey();
                        float[] values = entryBuf.getValue();
                        int cursor = translationCursor.get(sid);
                        float[] state = translationState.getOrDefault(sid, new float[]{0f, 0f, 0f});
                        values[cursor++] = state[0];
                        values[cursor++] = state[1];
                        values[cursor++] = state[2];
                        translationCursor.put(sid, cursor);
                    }

                    for (CombinedBone bone : combinedBones) {
                        float[] quat = rotationState.getOrDefault(key(bone.skeletonId(), bone.boneIndex()), new float[]{0f, 0f, 0f, 1f});
                        if (bone.skeletonId() == 0 && frameOffset < 3) {
                            System.out.println("debug frame " + frameOffset + " bone " + bone.boneIndex() + " node " + bone.nodeIndex() + " quat " + java.util.Arrays.toString(quat));
                        }
                        float[] values = rotationBuffersMap.get(bone.nodeIndex());
                        int cursor = rotationCursor.get(bone.nodeIndex());
                        values[cursor++] = quat[0];
                        values[cursor++] = quat[1];
                        values[cursor++] = quat[2];
                        values[cursor++] = quat[3];
                        rotationCursor.put(bone.nodeIndex(), cursor);
                    }
                }

                for (Map.Entry<Integer, float[]> entryBuf : translationBuffers.entrySet()) {
                    int sid = entryBuf.getKey();
                    float[] translations = entryBuf.getValue();
                    int transOffset = buffer.add(translations, 4);
                    int transViewIndex = bufferViews.size();
                    Map<String, Object> transView = new LinkedHashMap<>();
                    transView.put("buffer", 0);
                    transView.put("byteOffset", transOffset);
                    transView.put("byteLength", translations.length * 4);
                    bufferViews.add(transView);

                    int transAccessorIndex = accessors.size();
                    Map<String, Object> transAccessor = new LinkedHashMap<>();
                    transAccessor.put("bufferView", transViewIndex);
                    transAccessor.put("componentType", 5126);
                    transAccessor.put("count", frameCount);
                    transAccessor.put("type", "VEC3");
                    accessors.add(transAccessor);

                    int samplerIndex = samplers.size();
                    Map<String, Object> sampler = new LinkedHashMap<>();
                    sampler.put("input", timeAccessorIndex);
                    sampler.put("output", transAccessorIndex);
                    sampler.put("interpolation", "STEP");
                    samplers.add(sampler);

                    Map<String, Object> channel = new LinkedHashMap<>();
                    channel.put("sampler", samplerIndex);
                    Map<String, Object> target = new LinkedHashMap<>();
                    target.put("node", skeletonRootNodes.get(sid));
                    target.put("path", "translation");
                    channel.put("target", target);
                    channels.add(channel);
                }

                for (CombinedBone bone : combinedBones) {
                    float[] rotations = rotationBuffersMap.get(bone.nodeIndex());
                    int rotOffset = buffer.add(rotations, 4);
                    int rotViewIndex = bufferViews.size();
                    Map<String, Object> rotView = new LinkedHashMap<>();
                    rotView.put("buffer", 0);
                    rotView.put("byteOffset", rotOffset);
                    rotView.put("byteLength", rotations.length * 4);
                    bufferViews.add(rotView);

                    int rotAccessorIndex = accessors.size();
                    Map<String, Object> rotAccessor = new LinkedHashMap<>();
                    rotAccessor.put("bufferView", rotViewIndex);
                    rotAccessor.put("componentType", 5126);
                    rotAccessor.put("count", frameCount);
                    rotAccessor.put("type", "VEC4");
                    accessors.add(rotAccessor);

                    int samplerIndex = samplers.size();
                    Map<String, Object> sampler = new LinkedHashMap<>();
                    sampler.put("input", timeAccessorIndex);
                    sampler.put("output", rotAccessorIndex);
                    sampler.put("interpolation", "STEP");
                    samplers.add(sampler);

                    Map<String, Object> channel = new LinkedHashMap<>();
                    channel.put("sampler", samplerIndex);
                    Map<String, Object> target = new LinkedHashMap<>();
                    target.put("node", bone.nodeIndex());
                    target.put("path", "rotation");
                    channel.put("target", target);
                    channels.add(channel);
                }

                Map<String, Object> animation = new LinkedHashMap<>();
                animation.put("name", clip.getName());
                animation.put("samplers", samplers);
                animation.put("channels", channels);
                animations.add(animation);
            }
        }

        List<Map<String, Object>> materials = new ArrayList<>();
        List<Map<String, Object>> images = new ArrayList<>();
        List<Map<String, Object>> textures = new ArrayList<>();
        List<Map<String, Object>> samplersDef = new ArrayList<>();
        if (texture != null) {
            Map<String, Object> sampler = new LinkedHashMap<>();
            sampler.put("magFilter", 9729);
            sampler.put("minFilter", 9987);
            sampler.put("wrapS", 10497);
            sampler.put("wrapT", 10497);
            samplersDef.add(sampler);

            Map<String, Object> image = new LinkedHashMap<>();
            image.put("uri", texture.getImageName());
            images.add(image);

            Map<String, Object> tex = new LinkedHashMap<>();
            tex.put("sampler", 0);
            tex.put("source", 0);
            textures.add(tex);

            Map<String, Object> material = new LinkedHashMap<>();
            material.put("name", "Material");
            Map<String, Object> pbr = new LinkedHashMap<>();
            pbr.put("baseColorTexture", Map.of("index", 0));
            pbr.put("metallicFactor", 0.0f);
            pbr.put("roughnessFactor", 1.0f);
            material.put("pbrMetallicRoughness", pbr);
            material.put("doubleSided", true);
            materials.add(material);
        }

        Map<String, Object> gltf = new LinkedHashMap<>();
        gltf.put("asset", Map.of("version", "2.0"));
        gltf.put("scene", 0);
        gltf.put("scenes", List.of(Map.of("nodes", List.of(rootIndex))));
        gltf.put("nodes", nodes);
        gltf.put("meshes", gltfMeshes);
        gltf.put("buffers", List.of(Map.of("uri", stem + ".bin", "byteLength", buffer.size())));
        gltf.put("bufferViews", bufferViews);
        gltf.put("accessors", accessors);
        if (!materials.isEmpty()) {
            gltf.put("materials", materials);
            gltf.put("samplers", samplersDef);
            gltf.put("images", images);
            gltf.put("textures", textures);
        }
        if (!animations.isEmpty()) {
            gltf.put("animations", animations);
        }

        return new GltfExportResult(JsonUtil.toJson(gltf), buffer.toByteArray());
    }

    private static String key(int sid, int boneIdx) {
        return sid + ":" + boneIdx;
    }

    private record CombinedBone(int skeletonId, int boneIndex, int nodeIndex) {}
}

