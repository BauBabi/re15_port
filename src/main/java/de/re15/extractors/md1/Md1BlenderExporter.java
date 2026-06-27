package de.re15.extractors.md1;

import de.re15.extractors.emd.GltfExportResult;
import de.re15.extractors.emd.GltfExporter;
import de.re15.extractors.emd.MeshData;
import de.re15.extractors.emd.TextureInfo;
import de.re15.extractors.emd.TextureWriter;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Optional;

/**
 * Converts MD1 + TIM pairs into glTF documents that can be imported by Blender.
 *
 * <p>Each mesh is exported as an independent glTF mesh, textures are decoded into PNG files
 * next to the generated glTF/binary pair.</p>
 */
public final class Md1BlenderExporter {

    private final GltfExporter gltfExporter = new GltfExporter();

    public void export(Path md1Path, Path outputRoot) throws IOException {
        if (md1Path == null) {
            throw new IllegalArgumentException("md1Path must not be null");
        }
        MD1File md1 = MD1File.read(md1Path);
        Md1TextureAtlas.TextureAtlasDimensions atlas = Md1TextureAtlas.resolve(md1Path, md1.meshes);
        Path timPath = findCompanion(md1Path, ".tim", ".TIM").orElse(null);
        TextureInfo textureInfo = timPath != null ? decodeTexture(timPath, stripExtension(md1Path.getFileName().toString())) : null;

        List<MeshData> meshes = buildMeshes(md1.meshes, atlas);
        if (meshes.isEmpty()) {
            System.out.println("[Md1BlenderExporter] Keine Mesh-Daten in " + md1Path);
            return;
        }

        String stem = stripExtension(md1Path.getFileName().toString());
        // Write directly into outputRoot (no additional subfolder)
        Files.createDirectories(outputRoot);

        GltfExportResult result = gltfExporter.export(stem, meshes, Map.of(), textureInfo, false, null);
        Files.writeString(outputRoot.resolve(stem + ".gltf"), result.gltfJson());
        Files.write(outputRoot.resolve(stem + ".bin"), result.binData());
        if (textureInfo != null) {
            TextureWriter.writeTexture(textureInfo, outputRoot);
        }
    }

    private static List<MeshData> buildMeshes(List<MD1File.Mesh> meshes, Md1TextureAtlas.TextureAtlasDimensions atlas) {
        List<MeshData> result = new ArrayList<>(meshes.size());
        for (int i = 0; i < meshes.size(); i++) {
            MD1File.Mesh mesh = meshes.get(i);
            MeshData data = buildMeshData(mesh, i, atlas);
            if (data != null && data.getPositions().length > 0) {
                result.add(data);
            }
        }
        return result;
    }

    private static MeshData buildMeshData(MD1File.Mesh mesh, int index, Md1TextureAtlas.TextureAtlasDimensions atlas) {
        List<Float> positions = new ArrayList<>();
        List<Float> uvs = new ArrayList<>();

        for (int i = 0; i < mesh.triangles.size(); i++) {
            MD1File.Triangle tri = mesh.triangles.get(i);
            MD1File.TriangleUV uv = i < mesh.triangleUVs.size() ? mesh.triangleUVs.get(i) : null;
            double[] uv2 = uv != null ? Md1Geometry.mapUv(uv.u2, uv.v2, uv.page, atlas) : null;
            double[] uv1 = uv != null ? Md1Geometry.mapUv(uv.u1, uv.v1, uv.page, atlas) : null;
            double[] uv0 = uv != null ? Md1Geometry.mapUv(uv.u0, uv.v0, uv.page, atlas) : null;
            appendVertex(positions, uvs, mesh.vertices.get(tri.v2), uv2);
            appendVertex(positions, uvs, mesh.vertices.get(tri.v1), uv1);
            appendVertex(positions, uvs, mesh.vertices.get(tri.v0), uv0);
        }

        for (int i = 0; i < mesh.quads.size(); i++) {
            MD1File.Quad quad = mesh.quads.get(i);
            MD1File.QuadUV uv = i < mesh.quadUVs.size() ? mesh.quadUVs.get(i) : null;

            MD1File.Vertex v0 = mesh.vertices.get(mesh.quadVertexBase + quad.v0);
            MD1File.Vertex v1 = mesh.vertices.get(mesh.quadVertexBase + quad.v1);
            MD1File.Vertex v2 = mesh.vertices.get(mesh.quadVertexBase + quad.v2);
            MD1File.Vertex v3 = mesh.vertices.get(mesh.quadVertexBase + quad.v3);

            double[] uv0 = uv != null ? Md1Geometry.mapUv(uv.u0, uv.v0, uv.page, atlas) : null;
            double[] uv1 = uv != null ? Md1Geometry.mapUv(uv.u1, uv.v1, uv.page, atlas) : null;
            double[] uv2 = uv != null ? Md1Geometry.mapUv(uv.u2, uv.v2, uv.page, atlas) : null;
            double[] uv3 = uv != null ? Md1Geometry.mapUv(uv.u3, uv.v3, uv.page, atlas) : null;

            // PSX quad triangulation (matching lib_bio reference):
            // First triangle (v0, v1, v3)
            appendVertex(positions, uvs, v0, uv0);
            appendVertex(positions, uvs, v1, uv1);
            appendVertex(positions, uvs, v3, uv3);

            // Second triangle (v0, v2, v3)
            appendVertex(positions, uvs, v0, uv0);
            appendVertex(positions, uvs, v2, uv2);
            appendVertex(positions, uvs, v3, uv3);
        }

        if (positions.isEmpty()) {
            return null;
        }
        float[] posArray = toArray(positions);
        float[] uvArray = toArray(uvs);
        String name = String.format(Locale.ROOT, "mesh_%02d", index);
        return new MeshData(name, posArray, uvArray);
    }

    private static void appendVertex(List<Float> positions, List<Float> uvs, MD1File.Vertex vertex, double[] uv) {
        double[] mapped = Md1Geometry.mapVertex(vertex);
        positions.add((float) mapped[0]);
        positions.add((float) mapped[1]);
        positions.add((float) mapped[2]);
        if (uv != null && uv.length >= 2) {
            uvs.add((float) uv[0]);
            uvs.add((float) uv[1]);
        } else {
            uvs.add(0f);
            uvs.add(0f);
        }
    }

    private static float[] toArray(List<Float> values) {
        float[] array = new float[values.size()];
        for (int i = 0; i < values.size(); i++) {
            array[i] = values.get(i);
        }
        return array;
    }

    private static TextureInfo decodeTexture(Path timPath, String stem) throws IOException {
        byte[] data = Files.readAllBytes(timPath);
        TimDecoder.DecodedTim decoded = TimDecoder.decode(data);
        BufferedImage image = decoded.image();
        int width = image.getWidth();
        int height = image.getHeight();
        byte[] rgba = new byte[width * height * 4];
        int cursor = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int argb = image.getRGB(x, y);
                rgba[cursor++] = (byte) ((argb >> 16) & 0xFF);
                rgba[cursor++] = (byte) ((argb >> 8) & 0xFF);
                rgba[cursor++] = (byte) (argb & 0xFF);
                rgba[cursor++] = (byte) ((argb >>> 24) & 0xFF);
            }
        }
        return new TextureInfo(width, height, 1, Math.max(1, width), rgba, stem + ".png");
    }

    private static Optional<Path> findCompanion(Path md1Path, String... extensions) {
        Path parent = md1Path.getParent();
        if (parent == null) {
            parent = Path.of(".").toAbsolutePath();
        }
        String stem = stripExtension(md1Path.getFileName().toString());
        for (String ext : extensions) {
            Path candidate = parent.resolve(stem + ext);
            if (Files.isRegularFile(candidate)) {
                return Optional.of(candidate);
            }
        }
        return Optional.empty();
    }

    private static String stripExtension(String name) {
        int dot = name.lastIndexOf('.');
        return dot >= 0 ? name.substring(0, dot) : name;
    }
}
