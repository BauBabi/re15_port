package de.re15.extractors;

import de.re15.extractors.audio.VabToWavConverter;
import de.re15.extractors.md1.Md1BlenderExporter;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.stream.Collectors;

/**
 * Extracts Resident Evil 1.5 DO2 door archives into MD1/TIM/SCD/VH/VB assets.
 *
 * <p>The implementation mirrors the layout that Biofat v0009 produces: an MD1 model, a TIM texture,
 * eight SCD door scripts (plus matching C disassembly) and a VAB pair (VH/VB). The existing
 * {@link SCDScriptDisassembler} is reused to generate the C sources for each script.</p>
 */
public class DO2Extractor {

    private static final String DEFAULT_SOURCE = "info/Re1.5";
    private static final String DEFAULT_TARGET = "extracted";

    private final SCDScriptDisassembler scdDisassembler = new SCDScriptDisassembler();
    private final Md1BlenderExporter md1BlenderExporter = new Md1BlenderExporter();
    private final VabToWavConverter vabConverter = new VabToWavConverter();

    // Instance fields for current extraction paths
    private Path inputRoot;
    private Path outputRoot;

    public static void main(String[] args) throws IOException {
        new DO2Extractor().extractDoors(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    /**
     * Extrahiert DO2-Dateien mit Standard-Pfaden (Fallback).
     */
    public void extractDoors() throws IOException {
        extractDoors(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    /**
     * Extrahiert DO2-Dateien mit benutzerdefinierten Pfaden.
     *
     * @param sourceRoot Quellverzeichnis (z.B. "info/Re1.5")
     * @param outputRoot Zielverzeichnis (z.B. "extracted")
     */
    public void extractDoors(String sourceRoot, String outputRoot) throws IOException {
        this.inputRoot = Paths.get(sourceRoot, "PSX", "DOOR");
        this.outputRoot = Paths.get(outputRoot, "PSX", "DOOR");

        if (!Files.isDirectory(inputRoot)) {
            System.out.println("Keine DOOR Dateien gefunden: " + inputRoot);
            return;
        }
        Files.createDirectories(this.outputRoot);
        try (var files = Files.list(inputRoot)) {
            List<Path> do2Files = files
                .filter(p -> p.getFileName().toString().toUpperCase(Locale.ROOT).endsWith(".DO2"))
                .sorted()
                .collect(Collectors.toList());
            for (Path do2 : do2Files) {
                try {
                    extractSingle(do2);
                } catch (Exception ex) {
                    throw new IOException("Fehler beim Extrahieren von " + do2 + ": " + ex.getMessage(), ex);
                }
            }
        }
    }

    private void extractSingle(Path do2File) throws IOException {
        String baseName = stripExtension(do2File.getFileName().toString());
        System.out.println("=== DO2 Extraktion: " + baseName + " ===");
        byte[] data = Files.readAllBytes(do2File);
        Layout layout = Layout.parse(data);

        Path targetDir = this.outputRoot.resolve(baseName);
        Files.createDirectories(targetDir);

        writeMd1(targetDir, baseName, layout);
        writeTim(targetDir, baseName, layout);
        writeScripts(targetDir, baseName, layout);
        writeSound(targetDir, baseName, layout);
        writeIni(targetDir, baseName, layout);
        exportForBlender(targetDir, baseName);
    }

    private void writeMd1(Path dir, String baseName, Layout layout) throws IOException {
        int md1LengthField = layout.reconstructMd1Length();
        ByteArrayOutputStream buffer = new ByteArrayOutputStream(layout.md1PayloadLength + 4);
        buffer.write(intToBytes(md1LengthField));
        buffer.write(layout.slice(layout.md1PayloadStart, layout.md1PayloadLength));
        Files.write(dir.resolve(baseName + ".md1"), buffer.toByteArray());
    }

    private void writeTim(Path dir, String baseName, Layout layout) throws IOException {
        byte[] tim = layout.slice(layout.timStart, layout.timLength());
        Files.write(dir.resolve(baseName + ".tim"), tim);
    }

    private void writeScripts(Path dir, String baseName, Layout layout) throws IOException {
        byte[] scdBlock = layout.slice(layout.scdStart, layout.scdLength());
        List<Integer> offsets = layout.readScdOffsets(scdBlock);
        String scriptPrefix = determineScriptPrefix(baseName);
        for (int i = 0; i < offsets.size(); i++) {
            int start = offsets.get(i);
            int end = (i + 1 < offsets.size()) ? offsets.get(i + 1) : scdBlock.length;
            if (end <= start) {
                continue;
            }
            byte[] script = Arrays.copyOfRange(scdBlock, start, end);
            String scriptName = String.format(Locale.ROOT, "%s%04d", scriptPrefix, i);
            Path scdPath = dir.resolve(scriptName + ".scd");
            Files.write(scdPath, script);
            SCDScriptDisassembler.BlockMetadata metadata = SCDScriptDisassembler.BlockMetadata.forDoorScript();
            scdDisassembler.disassembleBlock(script, dir, scriptName, metadata);
        }
    }

    private void writeSound(Path dir, String baseName, Layout layout) throws IOException {
        Path vhPath = dir.resolve(baseName + ".vh");
        Path vbPath = dir.resolve(baseName + ".vb");
        Files.write(vhPath, layout.slice(layout.vhStart(), layout.vhLength()));
        Files.write(vbPath, layout.slice(layout.vbStart(), layout.vbLength()));
        vabConverter.convert(vhPath, vbPath, dir.resolve("wav"));
    }

    private void exportForBlender(Path dir, String baseName) throws IOException {
        Path md1Path = dir.resolve(baseName + ".md1");
        if (!Files.isRegularFile(md1Path)) {
            return;
        }
        // Export directly into the model's folder (no gltf_export subfolder)
        md1BlenderExporter.export(md1Path, dir);
    }

    private void writeIni(Path dir, String baseName, Layout layout) throws IOException {
        List<String> lines = new ArrayList<>();
        lines.add("[do2]");
        lines.add(baseName + ".md1");
        lines.add(Integer.toString(layout.scriptCount));
        lines.add(baseName + ".tim");
        lines.add(toHex(layout.soundValue1));
        lines.add(toHex(layout.soundValue2));
        lines.add(baseName + ".vh");
        lines.add(baseName + ".vb");
        Files.write(dir.resolve(baseName + ".ini"), lines);
    }

    private static String stripExtension(String name) {
        int dot = name.lastIndexOf('.');
        return dot >= 0 ? name.substring(0, dot) : name;
    }

    private static String determineScriptPrefix(String baseName) {
        if (baseName == null || baseName.isEmpty()) {
            return "DOOR";
        }
        return baseName.length() >= 4 ? baseName.substring(0, 4) : baseName;
    }

    private static byte[] intToBytes(int value) {
        return new byte[]{
            (byte) (value & 0xFF),
            (byte) ((value >>> 8) & 0xFF),
            (byte) ((value >>> 16) & 0xFF),
            (byte) ((value >>> 24) & 0xFF)
        };
    }

    private static String toHex(int value) {
        return String.format(Locale.ROOT, "0x%08X", value);
    }

    private static final class Layout {
        final byte[] data;
        final int modelOffset;
        final int soundOffset;
        final int vbOffset;
        final int md1PayloadStart;
        final int md1PayloadLength;
        final int scdStart;
        final int timStart;
        final int soundValue1;
        final int soundValue2;
        final int scriptCount;
        final int vbStart;

        static Layout parse(byte[] data) {
            int modelOffset = readInt(data, 0);
            int soundOffset = readInt(data, 4);
            int vbOffset = readInt(data, 8);
            int scdRelative = readInt(data, modelOffset + 4);
            int timRelative = readInt(data, modelOffset + 8);
            int md1PayloadStart = modelOffset + 16;
            int scdStart = modelOffset + scdRelative;
            int timStart = modelOffset + timRelative;
            int md1PayloadLength = scdStart - md1PayloadStart;

            int soundValue1 = readInt(data, soundOffset);
            int soundValue2 = readInt(data, soundOffset + 4);
            int vhStart = soundOffset + 8;
            int scriptCount = readInt(data, vbOffset - 8);

            return new Layout(data, modelOffset, soundOffset, vbOffset, md1PayloadStart,
                md1PayloadLength, scdStart, timStart, soundValue1, soundValue2, scriptCount, vbOffset);
        }

        private Layout(byte[] data,
                       int modelOffset,
                       int soundOffset,
                       int vbOffset,
                       int md1PayloadStart,
                       int md1PayloadLength,
                       int scdStart,
                       int timStart,
                       int soundValue1,
                       int soundValue2,
                       int scriptCount,
                       int vbStart) {
            this.data = data;
            this.modelOffset = modelOffset;
            this.soundOffset = soundOffset;
            this.vbOffset = vbOffset;
            this.md1PayloadStart = md1PayloadStart;
            this.md1PayloadLength = md1PayloadLength;
            this.scdStart = scdStart;
            this.timStart = timStart;
            this.soundValue1 = soundValue1;
            this.soundValue2 = soundValue2;
            this.scriptCount = scriptCount;
            this.vbStart = vbStart;
        }

        int reconstructMd1Length() {
            int objectCount = readInt(data, md1PayloadStart + 4);
            int meshCount = Math.max(0, objectCount / 2);
            int headerStart = md1PayloadStart + 8;
            int limit = md1PayloadStart + md1PayloadLength;
            int maxQFaceOffset = 0;
            for (int i = 0; i < meshCount; i++) {
                int entry = headerStart + i * 56;
                if (entry + 56 > limit) {
                    break;
                }
                int qFaceOffset = readInt(data, entry + 11 * 4);
                if (qFaceOffset > maxQFaceOffset) {
                    maxQFaceOffset = qFaceOffset;
                }
            }
            if (maxQFaceOffset <= 0) {
                maxQFaceOffset = Math.max(0, md1PayloadLength - 16);
            }
            return maxQFaceOffset + 12;
        }

        byte[] slice(int start, int length) {
            int end = Math.min(data.length, start + Math.max(length, 0));
            if (start < 0 || start >= end) {
                return new byte[0];
            }
            return Arrays.copyOfRange(data, start, end);
        }

        int timLength() {
            return Math.max(0, soundOffset - timStart);
        }

        int scdLength() {
            return Math.max(0, timStart - scdStart);
        }

        List<Integer> readScdOffsets(byte[] scdBlock) {
            List<Integer> offsets = new ArrayList<>();
            int prev = -1;
            for (int pos = 0; pos + 1 < scdBlock.length; pos += 2) {
                int value = readUnsignedShort(scdBlock, pos);
                if (value <= 0 || value <= prev || value >= scdBlock.length) {
                    break;
                }
                offsets.add(value);
                prev = value;
                if (offsets.size() == scriptCount) {
                    break;
                }
            }
            return offsets;
        }

        int vhStart() {
            return soundOffset + 8;
        }

        int vhLength() {
            return Math.max(0, vbStart - vhStart() - 8);
        }

        int vbStart() {
            return vbStart;
        }

        int vbLength() {
            return Math.max(0, data.length - vbStart());
        }
    }

    private static int readInt(byte[] data, int offset) {
        if (offset < 0 || offset + 4 > data.length) {
            return 0;
        }
        return (data[offset] & 0xFF)
            | ((data[offset + 1] & 0xFF) << 8)
            | ((data[offset + 2] & 0xFF) << 16)
            | ((data[offset + 3] & 0xFF) << 24);
    }

    private static int readUnsignedShort(byte[] data, int offset) {
        if (offset < 0 || offset + 2 > data.length) {
            return 0;
        }
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }
}
