package de.re15.extractors;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

/**
 * Parser for ITP files (Item Texture Pack) - Resident Evil 1.5 format.
 *
 * <p>ITP files contain multiple TIM images packed together, used for inventory
 * item icons. Each TIM is stored in a fixed-size slot of 0x3000 (12288) bytes,
 * with padding at the end of each slot.</p>
 *
 * <h3>Format Overview:</h3>
 * <ul>
 *   <li><b>Slot Size</b>: 0x3000 (12288) bytes per TIM</li>
 *   <li><b>TIM Format</b>: 8-bit with CLUT (256 colors)</li>
 *   <li><b>Image Size</b>: Typically 112x72 pixels</li>
 *   <li><b>Padding</b>: Remaining bytes in each slot filled with 0x00</li>
 * </ul>
 *
 * <p>Example: ITPS.ITP contains 71 item icons.</p>
 */
public class ItpParser {

    /** Fixed slot size for each TIM in the ITP file */
    private static final int TIM_SLOT_SIZE = 0x3000; // 12288 bytes

    /** TIM magic number */
    private static final int TIM_MAGIC = 0x10;

    /** Valid TIM flags for 8-bit with CLUT */
    private static final int TIM_FLAGS_8BIT_CLUT = 0x09;

    /**
     * Extract all TIM images from an ITP file and convert them to BMP.
     *
     * @param itpFile  Path to the ITP file
     * @param outputDir Output directory for BMP files
     * @return Number of extracted images
     */
    public int extractToBmp(Path itpFile, Path outputDir) throws IOException {
        byte[] data = Files.readAllBytes(itpFile);
        String baseName = getBaseName(itpFile.getFileName().toString());

        Files.createDirectories(outputDir);

        List<TimImage> images = parseItp(data);
        int count = 0;

        for (TimImage image : images) {
            String fileName = String.format("%s_%03d.bmp", baseName, image.index + 1);
            Path outputPath = outputDir.resolve(fileName);

            if (writeBmp(image.timData, outputPath)) {
                count++;
            }
        }

        return count;
    }

    /**
     * Extract all TIM images from an ITP file and save as separate TIM files.
     *
     * @param itpFile  Path to the ITP file
     * @param outputDir Output directory for TIM files
     * @return Number of extracted images
     */
    public int extractToTim(Path itpFile, Path outputDir) throws IOException {
        byte[] data = Files.readAllBytes(itpFile);
        String baseName = getBaseName(itpFile.getFileName().toString());

        Files.createDirectories(outputDir);

        List<TimImage> images = parseItp(data);
        int count = 0;

        for (TimImage image : images) {
            String fileName = String.format("%s_%03d.tim", baseName, image.index + 1);
            Path outputPath = outputDir.resolve(fileName);

            Files.write(outputPath, image.timData);
            count++;
        }

        return count;
    }

    /**
     * Parse an ITP file and extract all TIM images.
     *
     * @param data Raw ITP file data
     * @return List of extracted TIM images
     */
    public List<TimImage> parseItp(byte[] data) {
        List<TimImage> images = new ArrayList<>();

        int offset = 0;
        int index = 0;

        while (offset + TIM_SLOT_SIZE <= data.length) {
            // Check for TIM magic at slot start
            if (data[offset] == TIM_MAGIC && data[offset + 1] == 0x00 &&
                data[offset + 2] == 0x00 && data[offset + 3] == 0x00) {

                // Calculate actual TIM size (without padding)
                int timSize = calculateTimSize(data, offset);

                if (timSize > 0 && timSize <= TIM_SLOT_SIZE) {
                    byte[] timData = new byte[timSize];
                    System.arraycopy(data, offset, timData, 0, timSize);
                    images.add(new TimImage(index, offset, timSize, timData));
                }
            }

            offset += TIM_SLOT_SIZE;
            index++;
        }

        return images;
    }

    /**
     * Calculate the actual size of a TIM image (excluding padding).
     */
    private int calculateTimSize(byte[] data, int offset) {
        if (offset + 8 > data.length) {
            return -1;
        }

        ByteBuffer buffer = ByteBuffer.wrap(data, offset, Math.min(data.length - offset, TIM_SLOT_SIZE))
                .order(ByteOrder.LITTLE_ENDIAN);

        int magic = buffer.getInt();
        int flags = buffer.getInt();

        if (magic != TIM_MAGIC) {
            return -1;
        }

        boolean hasClut = (flags & 0x8) != 0;
        int headerSize = 8; // Magic + Flags

        int clutBlockSize = 0;
        if (hasClut) {
            if (buffer.remaining() < 4) {
                return -1;
            }
            clutBlockSize = buffer.getInt();
            buffer.position(buffer.position() + clutBlockSize - 4); // Skip CLUT data
        }

        if (buffer.remaining() < 4) {
            return -1;
        }
        int imageBlockSize = buffer.getInt();

        return headerSize + (hasClut ? clutBlockSize : 0) + imageBlockSize;
    }

    /**
     * Convert TIM data to BMP and write to file.
     */
    private boolean writeBmp(byte[] timData, Path outputPath) throws IOException {
        BufferedImage image = decodeTim(timData);
        if (image == null) {
            return false;
        }
        ImageIO.write(image, "BMP", outputPath.toFile());
        return true;
    }

    /**
     * Decode TIM data to BufferedImage.
     */
    private BufferedImage decodeTim(byte[] timData) {
        if (timData == null || timData.length < 20) {
            return null;
        }

        ByteBuffer buffer = ByteBuffer.wrap(timData).order(ByteOrder.LITTLE_ENDIAN);

        int magic = buffer.getInt();
        if (magic != TIM_MAGIC) {
            return null;
        }

        int flags = buffer.getInt();
        boolean hasClut = (flags & 0x8) != 0;
        int mode = flags & 0x3;
        int bpp = switch (mode) {
            case 0 -> 4;
            case 1 -> 8;
            case 2 -> 16;
            case 3 -> 24;
            default -> 16;
        };

        int[] palette = null;
        if (hasClut) {
            if (buffer.remaining() < 12) {
                return null;
            }
            int clutSize = buffer.getInt();
            buffer.getShort(); // clutX
            buffer.getShort(); // clutY
            int clutWidth = Short.toUnsignedInt(buffer.getShort());
            int clutHeight = Short.toUnsignedInt(buffer.getShort());
            int colorCount = clutWidth * clutHeight;
            palette = new int[colorCount];
            for (int i = 0; i < colorCount; i++) {
                int color = Short.toUnsignedInt(buffer.getShort());
                palette[i] = convertPsxColorToRgb(color);
            }
        }

        if (buffer.remaining() < 12) {
            return null;
        }

        int imageSize = buffer.getInt();
        buffer.getShort(); // imageX
        buffer.getShort(); // imageY
        int imageWidthWords = Short.toUnsignedInt(buffer.getShort());
        int imageHeight = Short.toUnsignedInt(buffer.getShort());

        int pixelWidth = switch (bpp) {
            case 4 -> imageWidthWords * 4;
            case 8 -> imageWidthWords * 2;
            default -> imageWidthWords;
        };

        BufferedImage image = new BufferedImage(pixelWidth, imageHeight, BufferedImage.TYPE_INT_RGB);

        switch (bpp) {
            case 4 -> decode4Bit(buffer, palette, image, imageWidthWords);
            case 8 -> decode8Bit(buffer, palette, image, imageWidthWords);
            case 16 -> decode16Bit(buffer, image, imageWidthWords);
        }

        return image;
    }

    private void decode4Bit(ByteBuffer img, int[] palette, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        int actualWidth = target.getWidth();
        for (int y = 0; y < height; y++) {
            for (int xWord = 0; xWord < wordWidth; xWord++) {
                if (img.remaining() < 2) return;
                int word = Short.toUnsignedInt(img.getShort());
                for (int p = 0; p < 4; p++) {
                    int pixelX = xWord * 4 + p;
                    if (pixelX >= actualWidth) break;
                    int index = (word >> (p * 4)) & 0xF;
                    int color = palette != null && index < palette.length ? palette[index] : 0xFF000000;
                    target.setRGB(pixelX, y, color);
                }
            }
        }
    }

    private void decode8Bit(ByteBuffer img, int[] palette, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        int actualWidth = target.getWidth();
        for (int y = 0; y < height; y++) {
            for (int xWord = 0; xWord < wordWidth; xWord++) {
                if (img.remaining() < 2) return;
                int word = Short.toUnsignedInt(img.getShort());
                for (int p = 0; p < 2; p++) {
                    int pixelX = xWord * 2 + p;
                    if (pixelX >= actualWidth) break;
                    int index = (word >> (p * 8)) & 0xFF;
                    int color = palette != null && index < palette.length ? palette[index] : 0xFF000000;
                    target.setRGB(pixelX, y, color);
                }
            }
        }
    }

    private void decode16Bit(ByteBuffer img, BufferedImage target, int wordWidth) {
        int height = target.getHeight();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < wordWidth; x++) {
                if (img.remaining() < 2) return;
                int color = Short.toUnsignedInt(img.getShort());
                target.setRGB(x, y, convertPsxColorToRgb(color));
            }
        }
    }

    private int convertPsxColorToRgb(int color) {
        int r = (color & 0x1F) << 3;
        int g = ((color >> 5) & 0x1F) << 3;
        int b = ((color >> 10) & 0x1F) << 3;
        return (r << 16) | (g << 8) | b;
    }

    private String getBaseName(String fileName) {
        int dotIndex = fileName.lastIndexOf('.');
        return dotIndex > 0 ? fileName.substring(0, dotIndex) : fileName;
    }

    /**
     * Container for extracted TIM image data.
     */
    public static class TimImage {
        public final int index;
        public final int offset;
        public final int size;
        public final byte[] timData;

        public TimImage(int index, int offset, int size, byte[] timData) {
            this.index = index;
            this.offset = offset;
            this.size = size;
            this.timData = timData;
        }
    }

    /**
     * Command-line entry point for standalone usage.
     */
    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: ItpParser <input.itp> [output_dir]");
            System.out.println();
            System.out.println("Extracts TIM images from ITP files and converts them to BMP.");
            System.out.println();
            System.out.println("Examples:");
            System.out.println("  ItpParser ITPS.ITP");
            System.out.println("  ItpParser ITPS.ITP ./output");
            return;
        }

        try {
            Path inputFile = Paths.get(args[0]);
            Path outputDir = args.length > 1 ? Paths.get(args[1]) : inputFile.getParent();

            if (outputDir == null) {
                outputDir = Paths.get(".");
            }

            System.out.println("=== ITP Parser - Resident Evil 1.5 ===");
            System.out.println("Input:  " + inputFile);
            System.out.println("Output: " + outputDir);
            System.out.println();

            ItpParser parser = new ItpParser();
            int count = parser.extractToBmp(inputFile, outputDir);

            System.out.println();
            System.out.println("Extracted " + count + " item icons to BMP.");

        } catch (Exception e) {
            System.err.println("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
