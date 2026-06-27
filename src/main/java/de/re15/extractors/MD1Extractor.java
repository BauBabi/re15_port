package de.re15.extractors;

import de.re15.extractors.md1.MD1File;
import de.re15.extractors.md1.MD1File.Mesh;
import de.re15.extractors.md1.MD1File.Normal;
import de.re15.extractors.md1.MD1File.Quad;
import de.re15.extractors.md1.MD1File.QuadUV;
import de.re15.extractors.md1.MD1File.Triangle;
import de.re15.extractors.md1.MD1File.TriangleUV;
import de.re15.extractors.md1.MD1File.Vertex;
import de.re15.extractors.md1.Md1Geometry;
import de.re15.extractors.md1.Md1TextureAtlas;
import de.re15.extractors.md1.Md1TextureAtlas.TextureAtlasDimensions;
import de.re15.extractors.md1.TimDecoder;

import javax.imageio.ImageIO;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Optional;

/**
 * Clean MD1 ? OBJ converter that mirrors the reference implementation from lib_bio/lib_md1.py.
 */
public final class MD1Extractor {

    public static void main(String[] args) throws Exception {
        if (args.length < 2) {
            System.err.println("Usage: java de.re15.extractors.MD1Extractor <input.md1> <output.obj>");
            System.exit(1);
        }
        Path md1Path = Path.of(args[0]);
        Path objPath = Path.of(args[1]);
        new MD1Extractor().convert(md1Path, objPath);
    }

    public void extractMD1ToOBJ(File md1File, File objFile, File logFile) throws IOException {
        convert(md1File.toPath(), objFile.toPath());
    }

    public void convert(Path md1Path, Path objPath) throws IOException {
        MD1File md1 = MD1File.read(md1Path);
        List<Mesh> meshes = md1.meshes;
        TextureAtlasDimensions atlas = Md1TextureAtlas.resolve(md1Path, meshes);
        Path outputDir = objPath.toAbsolutePath().getParent();
        if (outputDir != null) {
            Files.createDirectories(outputDir);
        }
        int vertexOffset = 0;
        int normalOffset = 0;
        int uvOffset = 0;

        try (BufferedWriter writer = Files.newBufferedWriter(objPath)) {
            writer.write("# OBJ generated from MD1\n");
            for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++) {
                Mesh mesh = meshes.get(meshIndex);
                writer.write(String.format(Locale.US, "g mesh_%02d%n", meshIndex));

                for (Vertex vertex : mesh.vertices) {
                    double[] mapped = Md1Geometry.mapVertex(vertex);
                    writer.write("v " + format(mapped[0]) + ' ' + format(mapped[1]) + ' ' + format(mapped[2]) + "\n");
                }

                List<double[]> meshUvs = new ArrayList<>();
                for (TriangleUV uv : mesh.triangleUVs) {
                    meshUvs.add(Md1Geometry.mapUv(uv.u2, uv.v2, uv.page, atlas));
                    meshUvs.add(Md1Geometry.mapUv(uv.u1, uv.v1, uv.page, atlas));
                    meshUvs.add(Md1Geometry.mapUv(uv.u0, uv.v0, uv.page, atlas));
                }
                for (QuadUV uv : mesh.quadUVs) {
                    meshUvs.add(Md1Geometry.mapUv(uv.u3, uv.v3, uv.page, atlas));
                    meshUvs.add(Md1Geometry.mapUv(uv.u2, uv.v2, uv.page, atlas));
                    meshUvs.add(Md1Geometry.mapUv(uv.u1, uv.v1, uv.page, atlas));
                    meshUvs.add(Md1Geometry.mapUv(uv.u0, uv.v0, uv.page, atlas));
                }
                for (double[] uv : meshUvs) {
                    writer.write("vt " + format(uv[0]) + ' ' + format(uv[1]) + "\n");
                }

                for (Normal normal : mesh.normals) {
                    double[] mapped = Md1Geometry.mapNormal(normal);
                    writer.write("vn " + format(mapped[0]) + ' ' + format(mapped[1]) + ' ' + format(mapped[2]) + "\n");
                }

                int triangleUvCursor = 0;
                int triangleNormalBase = normalOffset;
                for (Triangle triangle : mesh.triangles) {
                    int v0 = vertexOffset + triangle.v0 + 1;
                    int v1 = vertexOffset + triangle.v1 + 1;
                    int v2 = vertexOffset + triangle.v2 + 1;
                    int tForV2 = uvOffset + triangleUvCursor + 1;
                    int tForV1 = uvOffset + triangleUvCursor + 2;
                    int tForV0 = uvOffset + triangleUvCursor + 3;
                    triangleUvCursor += 3;
                    int n0 = triangleNormalBase + triangle.n0 + 1;
                    int n1 = triangleNormalBase + triangle.n1 + 1;
                    int n2 = triangleNormalBase + triangle.n2 + 1;
                    writer.write(String.format(Locale.US,
                            "f %d/%d/%d %d/%d/%d %d/%d/%d%n",
                            v2, tForV2, n2,
                            v1, tForV1, n1,
                            v0, tForV0, n0));
                }

                int quadUvCursor = uvOffset + triangleUvCursor;
                int quadVertexBase = vertexOffset + mesh.quadVertexBase;
                int quadNormalBase = normalOffset + mesh.quadNormalBase;
                for (Quad quad : mesh.quads) {
                    int v0 = quadVertexBase + quad.v0 + 1;
                    int v1 = quadVertexBase + quad.v1 + 1;
                    int v2 = quadVertexBase + quad.v2 + 1;
                    int v3 = quadVertexBase + quad.v3 + 1;
                    int tForV2 = quadUvCursor + 2;
                    int tForV3 = quadUvCursor + 1;
                    int tForV1 = quadUvCursor + 3;
                    int tForV0 = quadUvCursor + 4;
                    quadUvCursor += 4;
                    int n0 = quadNormalBase + quad.n0 + 1;
                    int n1 = quadNormalBase + quad.n1 + 1;
                    int n2 = quadNormalBase + quad.n2 + 1;
                    int n3 = quadNormalBase + quad.n3 + 1;
                    writer.write(String.format(Locale.US,
                            "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d%n",
                            v2, tForV2, n2,
                            v3, tForV3, n3,
                            v1, tForV1, n1,
                            v0, tForV0, n0));
                }

                vertexOffset += mesh.vertices.size();
                uvOffset += meshUvs.size();
                normalOffset += mesh.normals.size();
            }
        }
        exportCompanionResources(md1Path, objPath);
    }


    private void exportCompanionResources(Path md1Path, Path objPath) throws IOException {
        Path sourceDir = md1Path.toAbsolutePath().getParent();
        if (sourceDir == null) {
            sourceDir = Path.of(".").toAbsolutePath();
        }
        Path outputDir = objPath.toAbsolutePath().getParent();
        if (outputDir == null) {
            outputDir = Path.of(".").toAbsolutePath();
        } else {
            Files.createDirectories(outputDir);
        }
        String baseName = stripExtension(md1Path.getFileName().toString());

        Optional<Path> timSource = findCompanion(sourceDir, baseName, ".tim", ".TIM");
        if (timSource.isPresent()) {
            Path pathTim = timSource.get();
            Path target = outputDir.resolve(pathTim.getFileName());
            Files.copy(pathTim, target, StandardCopyOption.REPLACE_EXISTING);
            try {
                convertTimToBmp(target, outputDir);
            } catch (Exception e) {
                System.err.println("WARN: Failed to decode TIM to BMP: " + pathTim.getFileName() + " - " + e.getMessage());
            }
        }

        copyCompanionIfPresent(sourceDir, outputDir, baseName, ".edd", ".EDD");
        copyCompanionIfPresent(sourceDir, outputDir, baseName, ".emr", ".EMR");
    }

    private static void convertTimToBmp(Path timPath, Path outputDir) throws IOException {
        byte[] data = Files.readAllBytes(timPath);
        TimDecoder.DecodedTim decoded = TimDecoder.decode(data);
        String timName = timPath.getFileName().toString();
        String bmpName = stripExtension(timName) + ".bmp";
        Path bmpTarget = outputDir.resolve(bmpName);
        ImageIO.write(decoded.image(), "BMP", bmpTarget.toFile());
    }

    private static void copyCompanionIfPresent(Path sourceDir, Path outputDir, String baseName, String... extensions) throws IOException {
        Optional<Path> source = findCompanion(sourceDir, baseName, extensions);
        if (source.isPresent()) {
            Path path = source.get();
            Path target = outputDir.resolve(path.getFileName());
            Files.copy(path, target, StandardCopyOption.REPLACE_EXISTING);
        }
    }

    private static Optional<Path> findCompanion(Path directory, String baseName, String... extensions) {
        if (directory == null) {
            return Optional.empty();
        }
        for (String ext : extensions) {
            Path candidate = directory.resolve(baseName + ext);
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

    private static final int OUTPUT_SCALE = 6;

    private static String format(double value) {
        BigDecimal bd = BigDecimal.valueOf(value);
        bd = bd.setScale(OUTPUT_SCALE, RoundingMode.DOWN);
        return bd.toPlainString();
    }

    private static double clamp(double value) {
        if (value < 0.0) {
            return 0.0;
        }
        if (value > 1.0) {
            return 1.0;
        }
        return value;
    }

}


