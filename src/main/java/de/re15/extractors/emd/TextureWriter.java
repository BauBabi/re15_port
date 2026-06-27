package de.re15.extractors.emd;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public final class TextureWriter {
    private TextureWriter() {}

    public static void writeTexture(TextureInfo texture, Path outputDir) throws IOException {
        if (texture == null) {
            return;
        }
        Files.createDirectories(outputDir);
        BufferedImage image = new BufferedImage(texture.getWidth(), texture.getHeight(), BufferedImage.TYPE_4BYTE_ABGR);
        byte[] rgba = texture.getRgba();
        int idx = 0;
        for (int y = 0; y < texture.getHeight(); y++) {
            for (int x = 0; x < texture.getWidth(); x++) {
                int r = Byte.toUnsignedInt(rgba[idx++]);
                int g = Byte.toUnsignedInt(rgba[idx++]);
                int b = Byte.toUnsignedInt(rgba[idx++]);
                int a = Byte.toUnsignedInt(rgba[idx++]);
                int argb = (a << 24) | (r << 16) | (g << 8) | b;
                image.setRGB(x, y, argb);
            }
        }
        Path file = outputDir.resolve(texture.getImageName());
        ImageIO.write(image, "png", file.toFile());
    }
}
