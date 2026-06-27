package de.re15.extractors.bss;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.DirectoryStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Locale;
import javax.imageio.ImageIO;

/**
 * Liest ROOM*.BSS Dateien, zerlegt sie in 64 KB Blöcke und erzeugt pro
 * Block zusätzlich eine dekodierte TIM- sowie BMP-Version (320x240).
 */
public class BssExtractor {

    private static final int FRAME_WIDTH = 320;
    private static final int FRAME_HEIGHT = 240;
    private static final int CHUNK_SIZE = 0x10000;

    public void extract(Path sourceRoot, Path outputRoot) throws IOException {
        Path psxRoot = sourceRoot.resolve("PSX");
        if (!Files.isDirectory(psxRoot)) {
            System.err.println("[BSS] Quelle nicht gefunden: " + psxRoot);
            return;
        }
        try (DirectoryStream<Path> stages = Files.newDirectoryStream(psxRoot, "STAGE*")) {
            for (Path stage : stages) {
                if (!Files.isDirectory(stage)) {
                    continue;
                }
                Path targetStage = outputRoot.resolve(sourceRoot.relativize(stage));
                Files.createDirectories(targetStage);
                processStage(stage, targetStage);
            }
        }
    }

    private void processStage(Path stageDir, Path outputStage) throws IOException {
        try (DirectoryStream<Path> rooms = Files.newDirectoryStream(stageDir, "ROOM*.BSS")) {
            for (Path bss : rooms) {
                processRoom(bss, outputStage);
            }
        }
    }

    private void processRoom(Path bssFile, Path stageOutput) throws IOException {
        byte[] data = Files.readAllBytes(bssFile);
        if (data.length == 0) {
            return;
        }
        String roomName = stripExtension(bssFile.getFileName().toString());
        Path roomDir = stageOutput.resolve(roomName);
        Files.createDirectories(roomDir);

        int chunkCount = (int) Math.ceil(data.length / (double) CHUNK_SIZE);
        System.out.println("[BSS] " + roomName + " (" + chunkCount + " Blöcke)");

        for (int i = 0; i < chunkCount; i++) {
            int start = i * CHUNK_SIZE;
            int end = Math.min(data.length, start + CHUNK_SIZE);
            byte[] chunk = new byte[end - start];
            System.arraycopy(data, start, chunk, 0, chunk.length);

            String baseName = roomName + String.format(Locale.ROOT, "%02d", i);
            Path chunkPath = roomDir.resolve(baseName + ".bs");
            Files.write(chunkPath, chunk);

            if (!hasVideoData(chunk)) {
                continue;
            }

            try {
                byte[] rgb = decodeChunk(chunk);
                if (rgb.length == 0) {
                    continue;
                }
                byte[] tim = TimWriter.to16BitTim(rgb, FRAME_WIDTH, FRAME_HEIGHT);
                Files.write(roomDir.resolve(baseName + ".tim"), tim);
                writeBmp(rgb, roomDir.resolve(baseName + ".bmp"));

                /* sprite.pri FOREGROUND ATLAS (2026-06-09): a cut that has foreground
                 * occluders carries, AFTER the main BG, an SLD-compressed 8-bit TIM
                 * (the atlas the sprite.pri masks sample). Decode it byte-true via the
                 * cracked codec (SldDecoder = FUN_800c47e8) and emit <base>_pri.tim. */
                int sldOff = SldDecoder.findBlock(chunk);
                if (sldOff >= 0) {
                    byte[] fgTim = SldDecoder.decompressBlock(chunk, sldOff);
                    Files.write(roomDir.resolve(baseName + "_pri.tim"), fgTim);
                }
            } catch (Exception ex) {
                System.err.println("[BSS] Fehler beim Dekodieren von " + baseName + ": " + ex.getMessage());
            }
        }

        writeIni(roomDir, roomName, chunkCount);
        Files.copy(bssFile, roomDir.resolve(bssFile.getFileName()), java.nio.file.StandardCopyOption.REPLACE_EXISTING);
    }

    private String stripExtension(String name) {
        int dot = name.lastIndexOf('.');
        return dot >= 0 ? name.substring(0, dot) : name;
    }

    private boolean hasVideoData(byte[] chunk) {
        if (chunk.length < 4) {
            return false;
        }
        int id = ((chunk[3] & 0xFF) << 8) | (chunk[2] & 0xFF);
        return id == VlcDecoder.VLC_ID;
    }

    private byte[] decodeChunk(byte[] chunk) {
        BssChunk parsed = BssChunk.parse(chunk);
        return MdecDecoder.decode(parsed.runLength, FRAME_WIDTH, FRAME_HEIGHT);
    }

    private void writeBmp(byte[] rgb, Path target) throws IOException {
        BufferedImage image = new BufferedImage(FRAME_WIDTH, FRAME_HEIGHT, BufferedImage.TYPE_INT_RGB);
        int idx = 0;
        for (int y = 0; y < FRAME_HEIGHT; y++) {
            for (int x = 0; x < FRAME_WIDTH; x++) {
                /* BO-round 2026-05-29: REVERTED an R/B-read change. User confirmed
                 * the BG color was WRONG after reading [r,g,b]; the original
                 * (b=byte0) produces the BG the user considers correct — i.e. it
                 * matches the engine's ACTUALLY-DISPLAYED colors (the SDL RGBA8888
                 * display path effectively swaps vs the raw buffer, so my "engine
                 * reads byte0=R" code-analysis was misleading). Visual = authority. */
                int b = rgb[idx++] & 0xFF;
                int g = rgb[idx++] & 0xFF;
                int r = rgb[idx++] & 0xFF;
                int color = (r << 16) | (g << 8) | b;
                image.setRGB(x, y, color);
            }
        }
        ImageIO.write(image, "BMP", target.toFile());
    }

    private void writeIni(Path roomDir, String roomName, int chunkCount) throws IOException {
        StringBuilder builder = new StringBuilder();
        builder.append("[BSS]\n");
        builder.append("Version\t2\n");
        builder.append("Amount\t").append(chunkCount).append("\n");
        builder.append("[DATA]\n");
        for (int i = 0; i < chunkCount; i++) {
            builder.append(".\\./")
                .append(roomName)
                .append("\\./")
                .append(roomName)
                .append(String.format(Locale.ROOT, "%02d.bs%n", i));
        }
        Files.writeString(roomDir.resolve(roomName + ".ini"), builder.toString());
    }
}
