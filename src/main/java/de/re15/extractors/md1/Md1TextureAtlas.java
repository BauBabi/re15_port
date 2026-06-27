package de.re15.extractors.md1;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.Optional;

/**
 * Resolves the texture atlas dimensions that are required to map MD1 UV coordinates.
 *
 * <p>The MD1 format only stores CLUT/page references, therefore the atlas size needs to be either
 * inferred from the mesh data or read from the companion TIM/BMP files.</p>
 */
public final class Md1TextureAtlas {

    private Md1TextureAtlas() {
    }

    public static TextureAtlasDimensions resolve(Path md1Path, List<MD1File.Mesh> meshes) {
        Objects.requireNonNull(md1Path, "md1Path");
        Objects.requireNonNull(meshes, "meshes");

        TextureAtlasDimensions result = inferTextureDimensions(meshes);
        Optional<TextureAtlasDimensions> fromTim = tryReadTimDimensions(md1Path);
        if (fromTim.isPresent()) {
            result = merge(result, fromTim.get());
        }
        Optional<TextureAtlasDimensions> fromBmp = tryReadBmpDimensions(md1Path);
        if (fromBmp.isPresent()) {
            result = merge(result, fromBmp.get());
        }
        return result;
    }

    private static TextureAtlasDimensions merge(TextureAtlasDimensions a, TextureAtlasDimensions b) {
        if (a == null) {
            return b;
        }
        if (b == null) {
            return a;
        }
        int width = Math.max(a.width(), b.width());
        int height = Math.max(a.height(), b.height());
        return new TextureAtlasDimensions(width, height);
    }

    private static Optional<TextureAtlasDimensions> tryReadTimDimensions(Path md1Path) {
        Path tim = findCompanion(md1Path, ".tim", ".TIM");
        if (tim == null || !Files.isRegularFile(tim)) {
            return Optional.empty();
        }
        try {
            TimFile file = TimFile.read(tim);
            return Optional.of(new TextureAtlasDimensions(file.getWidth(), file.getHeight()));
        } catch (IOException ignored) {
            return Optional.empty();
        }
    }

    private static Optional<TextureAtlasDimensions> tryReadBmpDimensions(Path md1Path) {
        Path bmp = findCompanion(md1Path, ".bmp", ".BMP");
        if (bmp == null || !Files.isRegularFile(bmp)) {
            return Optional.empty();
        }
        try {
            BufferedImage image = ImageIO.read(bmp.toFile());
            if (image == null) {
                return Optional.empty();
            }
            return Optional.of(new TextureAtlasDimensions(image.getWidth(), image.getHeight()));
        } catch (IOException ignored) {
            return Optional.empty();
        }
    }

    private static Path findCompanion(Path md1Path, String... suffixes) {
        Path parent = md1Path.getParent();
        if (parent == null) {
            parent = Path.of(".").toAbsolutePath();
        }
        String baseName = stripExtension(md1Path.getFileName().toString());
        for (String suffix : suffixes) {
            Path candidate = parent.resolve(baseName + suffix);
            if (Files.isRegularFile(candidate)) {
                return candidate;
            }
        }
        return null;
    }

    private static TextureAtlasDimensions inferTextureDimensions(List<MD1File.Mesh> meshes) {
        int maxX = 0;
        int maxY = 0;
        for (MD1File.Mesh mesh : meshes) {
            for (MD1File.TriangleUV uv : mesh.triangleUVs) {
                int baseX = Md1Geometry.computePageBaseX(uv.page & 0xFFFF);
                int baseY = Md1Geometry.computePageBaseY(uv.page & 0xFFFF);
                maxX = Math.max(maxX, baseX + (uv.u0 & 0xFF));
                maxX = Math.max(maxX, baseX + (uv.u1 & 0xFF));
                maxX = Math.max(maxX, baseX + (uv.u2 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v0 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v1 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v2 & 0xFF));
            }
            for (MD1File.QuadUV uv : mesh.quadUVs) {
                int baseX = Md1Geometry.computePageBaseX(uv.page & 0xFFFF);
                int baseY = Md1Geometry.computePageBaseY(uv.page & 0xFFFF);
                maxX = Math.max(maxX, baseX + (uv.u0 & 0xFF));
                maxX = Math.max(maxX, baseX + (uv.u1 & 0xFF));
                maxX = Math.max(maxX, baseX + (uv.u2 & 0xFF));
                maxX = Math.max(maxX, baseX + (uv.u3 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v0 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v1 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v2 & 0xFF));
                maxY = Math.max(maxY, baseY + (uv.v3 & 0xFF));
            }
        }
        int width = roundUp(maxX + 1, 64);
        if (width <= 0) {
            width = 256;
        }
        int height = roundUp(maxY + 1, 256);
        if (height <= 0) {
            height = 256;
        }
        return new TextureAtlasDimensions(width, height);
    }

    private static int roundUp(int value, int step) {
        if (step <= 0) {
            return value;
        }
        int remainder = value % step;
        return remainder == 0 ? value : value + (step - remainder);
    }

    private static String stripExtension(String name) {
        int dot = name.lastIndexOf('.');
        return dot >= 0 ? name.substring(0, dot) : name;
    }

    public record TextureAtlasDimensions(int width, int height) {
        @Override
        public String toString() {
            return String.format(Locale.ROOT, "TextureAtlasDimensions[%dx%d]", width, height);
        }
    }
}
