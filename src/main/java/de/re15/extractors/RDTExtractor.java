package de.re15.extractors;

import de.re15.extractors.audio.VabToWavConverter;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Locale;
import java.util.NavigableSet;
import java.util.Objects;
import java.util.TreeSet;

/**
 * Resident Evil 1.5 RDT extractor.
 *
 * <p>The extractor mirrors the layout that was produced manually in
 * {@code already_extracted/PSX/STAGEx/roomXXXX}. It walks every room file found
 * in {@code info/Re1.5/PSX/STAGE1..6}, recreates the stage/room directory
 * structure below {@code extracted/PSX}, and splits the RDT resources into the
 * expected assets (camera/zone/light/sprite blocks, SCD scripts, messages,
 * sound banks, effect sprites, model TIM/MD1 pairs, etc.).</p>
 *
 * <p>The implementation follows the public documentation in
 * {@code information93.txt} and cross checks the offsets with the material that
 * shipped inside the repository. The goal is not to be a complete RE2-format
 * parser but to provide the deterministic assets that the tooling around this
 * project requires.</p>
 */
public class RDTExtractor {

    private static final String DEFAULT_SOURCE = "info/Re1.5";
    private static final String DEFAULT_TARGET = "extracted";

    private final SCDScriptDisassembler scdDisassembler = new SCDScriptDisassembler();
    private final VabToWavConverter vabConverter = new VabToWavConverter();
    private final MSGParser msgParser = new MSGParser();
    private final BLKParser blkParser = new BLKParser();
    private final RIDParser ridParser = new RIDParser();
    private final CollisionParser collisionParser = new CollisionParser();
    private final FloorParser floorParser = new FloorParser();
    private final LightParser lightParser = new LightParser();
    private final SpritePriParser spritePriParser = new SpritePriParser();
    private final ZoneRvdParser zoneParser = new ZoneRvdParser();
    private final EffectEspParser effectParser = new EffectEspParser();

    public static void main(String[] args) throws IOException {
        new RDTExtractor().extractStages(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    /**
     * Extrahiert RDT-Dateien mit Standard-Pfaden (Fallback).
     */
    public void extractStages() throws IOException {
        extractStages(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    /**
     * Extrahiert RDT-Dateien mit benutzerdefinierten Pfaden.
     *
     * @param sourceRoot Quellverzeichnis (z.B. "info/Re1.5")
     * @param outputRoot Zielverzeichnis (z.B. "extracted")
     */
    public void extractStages(String sourceRoot, String outputRoot) throws IOException {
        Path inputRoot = Paths.get(sourceRoot, "PSX");
        Path outputRootPath = Paths.get(outputRoot, "PSX");

        for (int stage = 1; stage <= 6; stage++) {
            Path stageInput = inputRoot.resolve("STAGE" + stage);
            if (!Files.isDirectory(stageInput)) {
                continue;
            }
            Path stageOutput = outputRootPath.resolve("STAGE" + stage);
            Files.createDirectories(stageOutput);
            final int stageNumber = stage;
            final Path stageDir = stageOutput;
            try (var files = Files.list(stageInput)) {
                files.filter(p -> p.getFileName().toString().toUpperCase(Locale.ROOT).endsWith(".RDT"))
                    .sorted()
                    .forEach(path -> {
                        try {
                            extractRdt(stageNumber, stageDir, path);
                        } catch (IOException e) {
                            throw new RuntimeException("Failed to extract " + path, e);
                        }
                    });
            }
        }
    }

    private void extractRdt(int stage, Path stageOutput, Path rdtFile) throws IOException {
        byte[] data = Files.readAllBytes(rdtFile);
        RdtFile rdt = RdtFile.parse(data);
        String fileName = rdtFile.getFileName().toString().toUpperCase(Locale.ROOT);
        String roomCode = fileName.substring(4, 8);
        String roomId = roomCode.substring(1, 3);
        char playerId = roomCode.charAt(3);
        String roomDirName = "room" + roomCode;
        Path roomDir = stageOutput.resolve(roomDirName);
        Files.createDirectories(roomDir);
        Files.createDirectories(roomDir.resolve("scd"));
        Files.createDirectories(roomDir.resolve("msg"));
        Files.createDirectories(roomDir.resolve("obj"));

        writeBasicChunks(roomDir, rdt);
        writeSound(roomDir, rdt);
        String stageName = stageOutput.getFileName().toString();
        writeMessages(roomDir, rdt);
        writeScd(roomDir, rdt, stageName, roomDirName, rdtFile);
        writeEffectAndSprites(roomDir, rdt);
        writeModels(roomDir, rdt);
        writeAnimation(roomDir, rdt);
        writeIni(stage, roomId, playerId, roomCode, roomDir, rdt);
    }

    private void writeIni(int stage, String roomId, char playerId, String roomCode, Path dir, RdtFile rdt) throws IOException {
        List<String> lines = new ArrayList<>();
        lines.add("// meta");
        lines.add("Stage\t\t" + stage);
        lines.add("Room\t\t" + roomId);
        lines.add("Player\t\t" + playerId);
        lines.add("");
        lines.add("// header");
        lines.add("nSprite\t\t" + rdt.header.spriteCount);
        lines.add("nCut\t\t" + rdt.header.cameraCount);
        lines.add("nOmodel\t\t" + rdt.header.modelCount);
        lines.add("nItem\t\t" + rdt.header.itemCount);
        lines.add("nDoor\t\t" + rdt.header.doorCount);
        lines.add("nRoom_at\t" + rdt.header.roomAtCount);
        lines.add("Reverb_lv\t" + rdt.header.reverbLevel);
        lines.add("");
        lines.add("// message");
        if (rdt.messageCount > 0) {
            lines.add("nMsg\t\t" + rdt.messageCount);
        }
        if (rdt.mainScdCount > 0 || rdt.subScdCount > 0 || rdt.extraScdCount > 0) {
            lines.add("// script");
            lines.add("nScdMain\t" + rdt.mainScdCount);
            lines.add("nScdSub0\t" + rdt.subScdCount);
            lines.add("nScdSub1\t" + rdt.extraScdCount);
        }
        lines.add("");
        lines.add("// object model");
        lines.add("ObjMdlZero\t0 0");
        lines.add("ObjTimZero\t0 0");
        lines.add("");
        lines.add("// effect sprite");
        lines.add("Esp\t\t" + formatEffectIds(rdt.effectIds));
        lines.add("");
        Files.write(dir.resolve("room" + roomCode + ".ini"), lines);
    }

    private void writeBasicChunks(Path dir, RdtFile rdt) throws IOException {
        byte[] cameraData = rdt.slice(rdt.addresses.cameraStart, rdt.nextBoundary(rdt.addresses.cameraStart));
        writeChunk(dir.resolve("camera.rid"), cameraData);
        try {
            String cameraJson = ridParser.parseToJson(cameraData, rdt.header.cameraCount);
            Files.writeString(dir.resolve("camera.json"), cameraJson, java.nio.charset.StandardCharsets.UTF_8);
        } catch (Exception e) {
            // Ignore parsing failures and keep raw data only
        }
        byte[] zoneData = rdt.slice(rdt.addresses.zoneStart, rdt.nextBoundary(rdt.addresses.zoneStart));
        writeChunk(dir.resolve("zone.rvd"), zoneData);
        try {
            String zoneJson = zoneParser.parseToJson(zoneData);
            Files.writeString(dir.resolve("zone.json"), zoneJson, java.nio.charset.StandardCharsets.UTF_8);
        } catch (Exception e) {
            // Ignore parse issues
        }
        byte[] collisionData = rdt.slice(rdt.addresses.collisionStart, rdt.nextBoundary(rdt.addresses.collisionStart));
        writeChunk(dir.resolve("collision.sca"), collisionData);
        try {
            String collisionJson = collisionParser.parseToJson(collisionData);
            Files.writeString(dir.resolve("collision.json"), collisionJson, java.nio.charset.StandardCharsets.UTF_8);
        } catch (Exception e) {
            // Ignore parser failures to keep extraction resilient
        }
        byte[] floorData = rdt.slice(rdt.addresses.floorStart, rdt.nextBoundary(rdt.addresses.floorStart));
        writeChunk(dir.resolve("floor.flr"), floorData);
        try {
            String floorJson = floorParser.parseToJson(floorData);
            Files.writeString(dir.resolve("floor.json"), floorJson, java.nio.charset.StandardCharsets.UTF_8);
        } catch (Exception e) {
            // Ignore parser failures and leave only the binary
        }
        byte[] blockData = rdt.slice(rdt.addresses.blockStart, rdt.nextBoundary(rdt.addresses.blockStart));
        writeChunk(dir.resolve("block.blk"), blockData);
        // Generate JSON representation of block data
        try {
            String blockJson = blkParser.parseToJson(blockData);
            Files.writeString(dir.resolve("block.json"), blockJson, java.nio.charset.StandardCharsets.UTF_8);
        } catch (Exception e) {
            // Silently ignore parsing errors
        }

        if (rdt.addresses.lightStart > 0) {
            int lightEnd = rdt.nextBoundary(rdt.addresses.lightStart);
            byte[] chunk = rdt.slice(rdt.addresses.lightStart, lightEnd);
            int lightLength = Math.min(chunk.length, Math.max(0, rdt.header.cameraCount * 40));
            byte[] lightBytes = Arrays.copyOfRange(chunk, 0, lightLength);
            writeChunk(dir.resolve("light.lit"), lightBytes);
            try {
                String lightJson = lightParser.parseToJson(lightBytes, rdt.header.cameraCount);
                Files.writeString(dir.resolve("light.json"), lightJson, java.nio.charset.StandardCharsets.UTF_8);
            } catch (Exception e) {
                // Keep raw chunk when parsing fails
            }
            if (chunk.length > lightLength) {
                byte[] spriteBytes = Arrays.copyOfRange(chunk, lightLength, chunk.length);
                writeChunk(dir.resolve("sprite.pri"), spriteBytes);
                try {
                    String spriteJson = spritePriParser.parseToJson(spriteBytes);
                    Files.writeString(dir.resolve("sprite.json"), spriteJson, java.nio.charset.StandardCharsets.UTF_8);
                } catch (Exception e) {
                    // Keep binary fallback on parse errors
                }
            }
        }
    }

    private void writeSound(Path dir, RdtFile rdt) throws IOException {
        Path snd0VhPath = dir.resolve("snd0.vh");
        Path snd0VbPath = dir.resolve("snd0.vb");
        Path snd1VhPath = dir.resolve("snd1.vh");
        Path snd1VbPath = dir.resolve("snd1.vb");

        if (rdt.addresses.snd0Edt > 0) {
            writeChunk(dir.resolve("snd0.edt"), rdt.slice(rdt.addresses.snd0Edt, rdt.nextBoundary(rdt.addresses.snd0Edt)));
        }
        if (rdt.addresses.snd0Vh > 0) {
            writeChunk(snd0VhPath, rdt.slice(rdt.addresses.snd0Vh, rdt.nextBoundary(rdt.addresses.snd0Vh)));
        }
        if (rdt.addresses.snd0Vb > 0) {
            writeChunk(snd0VbPath, rdt.slice(rdt.addresses.snd0Vb, rdt.nextBoundary(rdt.addresses.snd0Vb)));
            convertVab(snd0VhPath, snd0VbPath, dir.resolve("wav").resolve("snd0"));
        }
        if (rdt.addresses.snd1Edt > 0) {
            writeChunk(dir.resolve("snd1.edt"), rdt.slice(rdt.addresses.snd1Edt, rdt.nextBoundary(rdt.addresses.snd1Edt)));
        }
        if (rdt.addresses.snd1Vh > 0) {
            writeChunk(snd1VhPath, rdt.slice(rdt.addresses.snd1Vh, rdt.nextBoundary(rdt.addresses.snd1Vh)));
        }

        if (rdt.addresses.snd1Vb > 0) {
            int snd1End = rdt.nextBoundary(rdt.addresses.snd1Vb);
            byte[] data = rdt.slice(rdt.addresses.snd1Vb, snd1End);
            if (data.length >= 24) {
                int sndTail = data.length - 24;
                writeChunk(snd1VbPath, Arrays.copyOf(data, sndTail));
                writeChunk(dir.resolve("snd0.snd"), Arrays.copyOfRange(data, sndTail, sndTail + 12));
                writeChunk(dir.resolve("snd1.snd"), Arrays.copyOfRange(data, sndTail + 12, data.length));
            } else {
                writeChunk(snd1VbPath, data);
            }
            convertVab(snd1VhPath, snd1VbPath, dir.resolve("wav").resolve("snd1"));
        }
    }

    private void convertVab(Path vhPath, Path vbPath, Path outputDir) throws IOException {
        if (Files.isRegularFile(vhPath) && Files.isRegularFile(vbPath)) {
            vabConverter.convert(vhPath, vbPath, outputDir);
        }
    }

    private void writeScd(Path dir, RdtFile rdt, String stageName, String roomName, Path rdtSourcePath) throws IOException {
        // Use subScdStart as end boundary for mainScd if it comes after mainScdStart
        // Search backward from subScdStart to find the actual end (last Evt_end before sub section)
        int mainScdEnd;
        if (rdt.addresses.subScdStart > rdt.addresses.mainScdStart) {
            // Find the last 01 00 (Evt_end) before subScdStart
            int searchStart = Math.max(rdt.addresses.mainScdStart, rdt.addresses.subScdStart - 16);
            mainScdEnd = rdt.addresses.subScdStart;
            for (int pos = rdt.addresses.subScdStart - 2; pos >= searchStart; pos--) {
                if ((rdt.data[pos] & 0xFF) == 0x01 && (rdt.data[pos + 1] & 0xFF) == 0x00) {
                    mainScdEnd = pos + 2;
                    break;
                }
            }
        } else {
            mainScdEnd = rdt.nextBoundary(rdt.addresses.mainScdStart);
        }
        extractPointerBlock(rdt, rdt.addresses.mainScdStart, mainScdEnd,
            dir.resolve("scd"), "main",
            (index, relOffset, pointerTable, sectionStart) -> SCDScriptDisassembler.BlockMetadata.forBlock(
                stageName, roomName, dir, rdtSourcePath, "main", index, sectionStart, relOffset, pointerTable));
        extractPointerBlock(rdt, rdt.addresses.subScdStart, rdt.nextBoundary(rdt.addresses.subScdStart),
            dir.resolve("scd"), "sub",
            (index, relOffset, pointerTable, sectionStart) -> SCDScriptDisassembler.BlockMetadata.forBlock(
                stageName, roomName, dir, rdtSourcePath, "sub", index, sectionStart, relOffset, pointerTable));
        extractPointerBlock(rdt, rdt.addresses.extraScdStart, rdt.nextBoundary(rdt.addresses.extraScdStart),
            dir.resolve("scd"), "extra",
            (index, relOffset, pointerTable, sectionStart) -> SCDScriptDisassembler.BlockMetadata.forBlock(
                stageName, roomName, dir, rdtSourcePath, "extra", index, sectionStart, relOffset, pointerTable));
    }

    private void writeMessages(Path dir, RdtFile rdt) throws IOException {
        if (rdt.addresses.messageStart <= 0) {
            return;
        }
        extractPointerBlock(rdt, rdt.addresses.messageStart, rdt.nextBoundary(rdt.addresses.messageStart),
            dir.resolve("msg"), "main", null);
    }

    private void writeEffectAndSprites(Path dir, RdtFile rdt) throws IOException {
        if (rdt.addresses.effectStart > 0) {
            byte[] effectData = rdt.slice(rdt.addresses.effectStart, rdt.nextBoundary(rdt.addresses.effectStart));
            writeChunk(dir.resolve("effect.esp"), effectData);
            try {
                String effectJson = effectParser.parseToJson(effectData);
                Files.writeString(dir.resolve("effect.json"), effectJson, java.nio.charset.StandardCharsets.UTF_8);
            } catch (Exception e) {
                // Keep binary only when parsing fails
            }
        }
        if (rdt.addresses.espTimStart > 0) {
            int counter = 0;
            List<byte[]> tims = rdt.extractTimImages(rdt.addresses.espTimStart, rdt.nextBoundary(rdt.addresses.espTimStart));
            for (int i = 0; i < tims.size(); i++) {
                int effectId = counter < rdt.effectIds.length ? rdt.effectIds[counter] : 0xFF;
                while (effectId == 0xFF && counter < rdt.effectIds.length - 1) {
                    counter++;
                    effectId = rdt.effectIds[counter];
                }
                String suffix;
                if (effectId == 0xFF) {
                    suffix = String.format(Locale.ROOT, "%02d", i);
                } else {
                    suffix = String.format(Locale.ROOT, "%02X", effectId);
                }
                byte[] tim = tims.get(i);
                Path timPath = dir.resolve("esp" + suffix + ".tim");
                writeChunk(timPath, tim);
                writeTimPreview(tim, timPath);
                counter++;
            }
        }
    }

    private void writeModels(Path dir, RdtFile rdt) throws IOException {
        List<ModelEntry> models = rdt.models;
        for (int i = 0; i < models.size(); i++) {
            ModelEntry entry = models.get(i);
            String suffix = String.format(Locale.ROOT, "%02d", i);
            if (entry.timStart > 0) {
                writeChunk(dir.resolve("obj").resolve("model" + suffix + ".tim"),
                    rdt.slice(entry.timStart, rdt.nextBoundary(entry.timStart)));
            }
            if (entry.md1Start > 0) {
                writeChunk(dir.resolve("obj").resolve("model" + suffix + ".md1"),
                    rdt.slice(entry.md1Start, rdt.nextBoundary(entry.md1Start)));
            }
        }
    }

    private void writeAnimation(Path dir, RdtFile rdt) throws IOException {
        if (rdt.addresses.animationStart <= 0) {
            return;
        }
        byte[] rbj = rdt.slice(rdt.addresses.animationStart, rdt.nextBoundary(rdt.addresses.animationStart));
        // Raw container (consumed verbatim by the C engine's re15_emd_parse_rbj).
        writeChunk(dir.resolve("animation.rbj"), rbj);
        // Also split the RBJ into its per-record EDD (clip table) + keyframe pool,
        // flagging the idle-bank record. RBJ container layout (RE-verified 2026-06-06
        // via switchD_8003206c / FUN_8001b3f8 / re15_emd_parse_rbj):
        //   @0: u32 dir_off   @4: u32 record_count (low byte used)
        //   directory @dir_off: record_count * { u32 record_off, u32 edd_off }
        //   record @record_off: u32 flags (bit0 = IDLE bank), then EMR header
        //     @record_off+4: { u16 o_arm, u16 o_frm, u16 bone_count, u16 kf_size }
        //     keyframe pool @ (record_off+4 + o_frm) .. edd_off
        //   EDD clip table @edd_off: u16 count0, u16 tbl_bytes; clip i = u16 frame_count.
        splitRbjRecords(dir.resolve("rbj"), rbj);
    }

    /** Splits an RBJ animation container into per-record EDD (clip table) + keyframe-
     *  pool files plus a records.json describing each record (incl. the idle-bank
     *  flag). The skeleton hierarchy/rest offsets are NOT in the record (the runtime
     *  reuses the base PLD's EMR); only the keyframe pool is per-record here. */
    private void splitRbjRecords(Path outDir, byte[] rbj) throws IOException {
        if (rbj.length < 0x10) {
            return;
        }
        int dirOff = readInt(rbj, 0);
        int recordCount = readInt(rbj, 4) & 0xFF; // low byte (cf. FUN_8001b3f8 cVar8)
        if (recordCount <= 0 || recordCount > 8 || dirOff <= 0 || dirOff + recordCount * 8 > rbj.length) {
            return; // not an RBJ container (some rooms have an empty/odd animation block)
        }
        StringBuilder json = new StringBuilder();
        json.append("{\n  \"record_count\": ").append(recordCount).append(",\n  \"records\": [\n");
        boolean wroteAny = false;
        for (int i = 0; i < recordCount; i++) {
            int recOff = readInt(rbj, dirOff + i * 8);
            int eddOff = readInt(rbj, dirOff + i * 8 + 4);
            if (recOff < 0 || recOff + 8 > rbj.length || eddOff <= 0 || eddOff >= rbj.length) {
                continue;
            }
            int flags = readInt(rbj, recOff);
            boolean isIdle = (flags & 1) != 0;
            int emrHdr = recOff + 4;
            int oFrm = readUnsignedShort(rbj, emrHdr + 2);
            int boneCount = readUnsignedShort(rbj, emrHdr + 4);
            int kfSize = readUnsignedShort(rbj, emrHdr + 6);
            int kfOff = emrHdr + oFrm;
            int kfBytes = eddOff - kfOff;
            int kfCount = (kfSize > 0) ? kfBytes / kfSize : 0;
            int eddEnd = (i + 1 < recordCount) ? readInt(rbj, dirOff + (i + 1) * 8) : dirOff;
            int eddBytes = eddEnd - eddOff;
            if (kfOff < 0 || kfBytes <= 0 || kfOff + kfBytes > rbj.length
                    || eddBytes <= 0 || eddOff + eddBytes > rbj.length) {
                continue;
            }
            writeChunk(outDir.resolve("record" + i + ".edd"), Arrays.copyOfRange(rbj, eddOff, eddOff + eddBytes));
            writeChunk(outDir.resolve("record" + i + ".kf"), Arrays.copyOfRange(rbj, kfOff, kfOff + kfBytes));
            int[] clipFrames = parseEddClipFrames(rbj, eddOff);
            if (wroteAny) {
                json.append(",\n");
            }
            wroteAny = true;
            json.append("    { \"index\": ").append(i)
                .append(", \"flags\": ").append(flags)
                .append(", \"idle_bank\": ").append(isIdle)
                .append(", \"bone_count\": ").append(boneCount)
                .append(", \"keyframe_size\": ").append(kfSize)
                .append(", \"keyframe_count\": ").append(kfCount)
                .append(", \"clip_count\": ").append(clipFrames.length)
                .append(", \"clip_frame_counts\": ").append(Arrays.toString(clipFrames))
                .append(" }");
        }
        json.append("\n  ]\n}\n");
        if (wroteAny) {
            Files.writeString(outDir.resolve("records.json"), json.toString(),
                java.nio.charset.StandardCharsets.UTF_8);
        }
    }

    /** EDD clip-table frame counts: {u16 count0, u16 tbl_bytes}; clipCount=tbl_bytes/4;
     *  clip i frame_count = (i==0 ? count0 : u16 @ off + i*4). Mirrors the C
     *  re15_emd_parse_animation / Java AnimationParser. */
    private static int[] parseEddClipFrames(byte[] d, int off) {
        int count0 = readUnsignedShort(d, off);
        int tblBytes = readUnsignedShort(d, off + 2);
        if (tblBytes <= 0 || off + tblBytes > d.length) {
            return new int[0];
        }
        int clipCount = tblBytes / 4;
        if (clipCount <= 0 || clipCount > 256) {
            return new int[0];
        }
        int[] frames = new int[clipCount];
        frames[0] = count0;
        for (int i = 1; i < clipCount; i++) {
            frames[i] = readUnsignedShort(d, off + i * 4);
        }
        return frames;
    }

    private void extractPointerBlock(RdtFile rdt, int start, int end, Path targetDir, String prefix,
                                     BlockMetadataFactory metadataFactory) throws IOException {
        if (start <= 0 || end <= start) {
            return;
        }
        List<Integer> relativeOffsets = rdt.readPointerOffsets(start, end);
        final boolean isMessage = targetDir.getFileName().toString().equals("msg");
        final String extension = isMessage ? "msg" : "scd";

        if (!relativeOffsets.isEmpty()) {
            Collections.sort(relativeOffsets);
        }

        if (!isMessage && prefix.equals("sub")) {
            end = trimTrailingEvtEndEntry(rdt, start, end, relativeOffsets);
        }

        if (!isMessage && prefix.equals("main") && !relativeOffsets.isEmpty()) {
            int relStart = relativeOffsets.get(0);
            int dataStart = Math.min(Math.max(start + relStart, start), end);
            int dataEnd = !isMessage ? ensureEvtEndBoundary(rdt, dataStart, end) : end;
            byte[] data = Arrays.copyOfRange(rdt.data, dataStart, dataEnd);
            if (data.length >= 2) {
                int trailingValue = relativeOffsets.get(0);
                int tailLE = ((data[data.length - 2] & 0xFF) | ((data[data.length - 1] & 0xFF) << 8));
                int tailBE = ((data[data.length - 1] & 0xFF) | ((data[data.length - 2] & 0xFF) << 8));
                if (tailLE == trailingValue || tailBE == trailingValue) {
                    data = Arrays.copyOf(data, data.length - 2);
                }
            }
            data = (!isMessage) ? stripScdTerminator(data) : data;
            Path out = targetDir.resolve("main00." + extension);
            writeChunk(out, data);
            rdt.mainScdCount = 1;
            SCDScriptDisassembler.BlockMetadata metadata = metadataFactory != null
                ? metadataFactory.create(0, relStart, relativeOffsets, start)
                : null;
            if (data.length > 0) {
                scdDisassembler.disassembleBlock(data, targetDir, "main00", metadata);
            }
            return;
        }

        if (relativeOffsets.isEmpty()) {
            int blockStart = start;
            int blockEnd = (!isMessage) ? ensureEvtEndBoundary(rdt, start, end) : end;
            byte[] data = Arrays.copyOfRange(rdt.data, blockStart, blockEnd);
            data = (!isMessage) ? stripScdTerminator(data) : data;
            Path out = targetDir.resolve(prefix + "00." + extension);
            writeChunk(out, data);
            if (isMessage) {
                rdt.messageCount = 1;
                parseMessageToText(data, targetDir, prefix + "00");
            } else if (prefix.equals("main")) {
                rdt.mainScdCount = 1;
            } else if (prefix.equals("sub")) {
                rdt.subScdCount = 1;
            } else if (prefix.equals("extra")) {
                rdt.extraScdCount = 1;
            }
            if (!isMessage && data.length > 0) {
                SCDScriptDisassembler.BlockMetadata metadata = metadataFactory != null
                    ? metadataFactory.create(0, 0, List.of(), start)
                    : null;
                scdDisassembler.disassembleBlock(data, targetDir, String.format(Locale.ROOT, "%s%02d", prefix, 0), metadata);
            }
            return;
        }
        for (int i = 0; i < relativeOffsets.size(); i++) {
            int relStart = relativeOffsets.get(i);
            int relEnd = (i + 1 < relativeOffsets.size()) ? relativeOffsets.get(i + 1) : (end - start);
            if (relEnd <= relStart) {
                continue;
            }
            int chunkStart = start + relStart;
            int chunkEnd = start + relEnd;
            if (!isMessage && i == relativeOffsets.size() - 1) {
                chunkEnd = ensureEvtEndBoundary(rdt, chunkStart, chunkEnd);
            }
            byte[] data = Arrays.copyOfRange(rdt.data, chunkStart, chunkEnd);
            data = (!isMessage) ? stripScdTerminator(data) : data;
            Path out = targetDir.resolve(String.format(Locale.ROOT, "%s%02d.%s", prefix, i, extension));
            writeChunk(out, data);
            if (isMessage && data.length > 0) {
                parseMessageToText(data, targetDir, String.format(Locale.ROOT, "%s%02d", prefix, i));
            }
            if (!isMessage && data.length > 0) {
                SCDScriptDisassembler.BlockMetadata metadata = metadataFactory != null
                    ? metadataFactory.create(i, relStart, relativeOffsets, start)
                    : null;
                scdDisassembler.disassembleBlock(data, targetDir, String.format(Locale.ROOT, "%s%02d", prefix, i), metadata);
            }
        }
        if (isMessage) {
            rdt.messageCount = relativeOffsets.size();
        } else if (prefix.equals("main")) {
            rdt.mainScdCount = relativeOffsets.size();
        } else if (prefix.equals("sub")) {
            rdt.subScdCount = relativeOffsets.size();
        } else if (prefix.equals("extra")) {
            rdt.extraScdCount = relativeOffsets.size();
        }
    }

    private static String formatEffectIds(int[] ids) {
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < ids.length; i++) {
            if (i > 0) {
                builder.append(' ');
            }
            builder.append(String.format(Locale.ROOT, "%02X", ids[i]));
        }
        return builder.toString();
    }

    private void parseMessageToText(byte[] data, Path targetDir, String baseName) {
        try {
            String text = msgParser.parse(data);
            Path txtPath = targetDir.resolve(baseName + ".txt");
            Files.writeString(txtPath, text, java.nio.charset.StandardCharsets.UTF_8);
        } catch (IOException e) {
            // Silently ignore parsing errors
        }
    }

    private int ensureEvtEndBoundary(RdtFile rdt, int chunkStart, int chunkEnd) {
        if (rdt == null || rdt.data == null || chunkStart < 0 || chunkStart >= rdt.data.length) {
            return chunkEnd;
        }
        int cappedEnd = Math.min(Math.max(chunkEnd, chunkStart), rdt.data.length);
        if (cappedEnd - chunkStart >= 2
            && rdt.data[cappedEnd - 2] == (byte) 0x01
            && rdt.data[cappedEnd - 1] == 0x00) {
            return cappedEnd;
        }
        int boundaryLimit = rdt.nextBoundary(cappedEnd);
        int minSearchLimit = Math.min(rdt.data.length, cappedEnd + 0x40);
        int searchLimit = Math.min(rdt.data.length, Math.max(boundaryLimit, minSearchLimit));
        for (int pos = cappedEnd; pos + 1 < searchLimit; pos++) {
            if ((rdt.data[pos] & 0xFF) == 0x01 && (rdt.data[pos + 1] & 0xFF) == 0x00) {
                return pos + 2;
            }
        }
        return cappedEnd;
    }

    private int trimTrailingEvtEndEntry(RdtFile rdt, int start, int end, List<Integer> offsets) {
        if (rdt == null || offsets == null || offsets.isEmpty()) {
            return end;
        }
        int lastIndex = offsets.size() - 1;
        int relStart = offsets.get(lastIndex);
        int relEnd = end - start;
        if (relEnd <= relStart) {
            return end;
        }
        int chunkStart = Math.min(start + relStart, rdt.data.length);
        int chunkEnd = Math.min(start + relEnd, rdt.data.length);
        byte[] chunk = Arrays.copyOfRange(rdt.data, chunkStart, chunkEnd);
        byte[] stripped = stripScdTerminator(chunk);
        if (stripped.length == 2
            && stripped[0] == (byte) 0x01
            && stripped[1] == 0x00) {
            offsets.remove(lastIndex);
            return chunkStart;
        }
        return end;
    }

    private byte[] stripScdTerminator(byte[] data) {
        if (data == null || data.length < 2) {
            return data;
        }
        // Strip D5 20 terminator if present
        int len = data.length;
        if (data[len - 2] == (byte) 0xD5 && data[len - 1] == 0x20) {
            data = Arrays.copyOf(data, len - 2);
            len = data.length;
        }
        // Find the actual end of the SCD script by parsing opcodes
        int scriptEnd = findScdScriptEnd(data);
        if (scriptEnd > 0 && scriptEnd < len) {
            return Arrays.copyOf(data, scriptEnd);
        }
        return data;
    }

    /**
     * Parse SCD opcodes to find where the script actually ends (after Evt_end).
     * Returns the position after the terminating Evt_end, or -1 if not found.
     */
    private int findScdScriptEnd(byte[] data) {
        int pos = 0;
        int len = data.length;
        int lastEnd = -1;
        while (pos < len) {
            int opcode = data[pos] & 0xFF;
            int size = getOpcodeSize(opcode, data, pos);
            if (size <= 0) {
                // Unknown opcode, can't determine end
                return lastEnd;
            }
            if (opcode == 0x01) {
                // Remember every Evt_end we encounter and continue scanning.
                lastEnd = pos + size;
            }
            pos += size;
        }
        return lastEnd;
    }

    /**
     * Get the size of an SCD opcode. Returns -1 for unknown opcodes.
     */
    private int getOpcodeSize(int opcode, byte[] data, int pos) {
        // Common opcodes with fixed sizes
        switch (opcode) {
            case 0x00: return 1;  // nop
            case 0x01: return 2;  // Evt_end
            case 0x02: return 1;  // Evt_next
            case 0x03: return 4;  // Evt_chain
            case 0x04: return 4;  // Evt_exec
            case 0x05: return 2;  // Evt_kill
            case 0x06: return 4;  // Ifel_ck
            case 0x07: return 4;  // Else_ck
            case 0x08: return 2;  // Endif
            case 0x09: return 4;  // Sleep
            case 0x0A: return 3;  // Sleeping
            case 0x0B: return 1;  // Wsleep
            case 0x0C: return 1;  // Wsleeping
            case 0x0D: return 6;  // For
            case 0x0E: return 2;  // Next
            case 0x0F: return 4;  // While
            case 0x10: return 2;  // Ewhile
            case 0x11: return 4;  // Do
            case 0x12: return 2;  // Edwhile
            case 0x13: return 4;  // Switch
            case 0x14: return 6;  // Case
            case 0x15: return 4;  // Default
            case 0x16: return 2;  // Eswitch
            case 0x17: return 6;  // Goto
            case 0x18: return 2;  // Gosub
            case 0x19: return 2;  // Return
            case 0x1A: return 2;  // Break
            case 0x1B: return 6;  // For2
            case 0x1C: return 1;  // Break_point
            case 0x1D: return 1;  // Work_copy
            case 0x1E: return 1;  // nop
            case 0x20: return 1;  // nop
            case 0x21: return 4;  // Ck
            case 0x22: return 4;  // Set
            case 0x23: return 6;  // Cmp
            case 0x24: return 4;  // Save
            case 0x25: return 3;  // Copy
            case 0x26: return 6;  // Calc
            case 0x27: return 4;  // Calc2
            case 0x28: return 1;  // Sce_rnd
            case 0x29: return 2;  // Cut_chg
            case 0x2A: return 1;  // Cut_old
            case 0x2B: return 4;  // Message_on
            case 0x2C: return 20; // Aot_set
            case 0x2D: return determineObjModelSize(data, pos); // Obj_model_set (variable size)
            case 0x2E: return 3;  // Work_set
            case 0x2F: return 4;  // Speed_set
            case 0x30: return 1;  // Add_speed
            case 0x31: return 1;  // Add_aspeed
            case 0x32: return 8;  // Pos_set
            case 0x33: return 8;  // Dir_set
            case 0x34: return 4;  // Member_set
            case 0x35: return 3;  // Member_set2
            case 0x36: return 12; // Se_on
            case 0x37: return 4;  // Sca_id_set
            case 0x38: return 3;  // Flr_set
            case 0x39: return 8;  // Dir_ck
            case 0x3A: return 16; // Sce_espr_on
            case 0x3B: return 32; // Door_aot_set
            case 0x3C: return 2;  // Cut_auto
            case 0x3D: return 3;  // Member_copy
            case 0x3E: return 6;  // Member_cmp
            case 0x3F: return 4;  // Plc_motion
            case 0x40: return 8;  // Plc_dest
            case 0x41: return 10; // Plc_neck
            case 0x42: return 1;  // Plc_ret
            case 0x43: return 4;  // Plc_flg
            case 0x44: return 20; // Sce_em_set
            case 0x45: return 6;  // Col_chg_set
            case 0x46: return 10; // Aot_reset
            case 0x47: return 2;  // Aot_on
            case 0x48: return 16; // Super_set
            case 0x4A: return 2;  // Plc_gun
            case 0x4B: return 3;  // Cut_replace
            case 0x4C: return 5;  // Sce_espr_kill
            case 0x4D: return 22; // Op4D
            case 0x4E: return 22; // Item_aot_set (legacy)
            case 0x4F: return 4;  // Sce_key_ck
            case 0x50: return 22; // Item_aot_set
            case 0x51: return 4;  // Sce_key_ck
            case 0x53: return 6;  // Sce_fade_set
            case 0x54: return 6;  // Sce_bgm_control (SsSeq slot play/stop; was mislabeled Sce_espr_control2)
            case 0x55: return 6;  // Member_calc
            case 0x56: return 4;  // Member_calc2
            case 0x57: return 8;  // Sce_bgmtbl_set
            case 0x58: return 4;  // Plc_rot
            case 0x59: return 4;  // Xa_on
            case 0x5A: return 2;  // Weapon_chg
            case 0x5C: return 3;  // Sce_shake_on
            case 0x5D: return 2;  // Mizu_div_set
            case 0x5E: return 2;  // Keep_Item_ck
            case 0x5F: return 2;  // Xa_vol
            case 0x60: return 14; // Kage_set
            case 0x61: return 4;  // Cut_be_set
            case 0x62: return 2;  // Sce_item_lost
            case 0x63: return 1;  // Plc_gun_eff
            case 0x64: return 16; // Sce_espr_on2
            case 0x65: return 2;  // Sce_espr_kill2
            case 0x66: return 1;  // Plc_stop
            case 0x67: return 28; // Aot_set_4p
            case 0x68: return 40; // Door_aot_set_4p
            case 0x69: return 30; // Item_aot_set_4p
            case 0x6A: return 6;  // Light_pos_set
            case 0x6B: return 4;  // Light_kido_set
            case 0x6C: return 1;  // Rbj_reset
            case 0x6D: return 4;  // Sce_scr_move
            case 0x6E: return 6;  // Parts_set
            case 0x6F: return 2;  // Movie_on
            case 0x70: return 1;  // Splc_ret
            case 0x71: return 1;  // Splc_sce
            case 0x72: return 16; // Super_on
            case 0x73: return 8;  // Mirror_set
            case 0x74: return 4;  // Sce_fade_adjust
            case 0x75: return 22; // Sce_espr3d_on2
            case 0x76: return 3;  // Sce_item_get
            case 0x77: return 4;  // Sce_line_start
            case 0x78: return 6;  // Sce_line_main
            case 0x79: return 1;  // Sce_line_end
            case 0x7A: return 16; // Sce_parts_bomb
            case 0x7B: return 16; // Sce_parts_down
            case 0x7C: return 6;  // Light_color_set
            case 0x7D: return 6;  // Light_pos_set2
            case 0x7E: return 6;  // Light_kido_set2
            case 0x7F: return 6;  // Light_color_set2
            case 0x80: return 2;  // Se_vol
            case 0x81: return 3;  // Op81
            case 0x82: return 3;  // Op82
            case 0x83: return 1;  // Op83
            case 0x84: return 2;  // Op84
            case 0x85: return 6;  // Op85
            case 0x86: return 1;  // Poison_ck
            case 0x87: return 1;  // Poison_clr
            case 0x88: return 3;  // Sce_item_ck_lost
            case 0x89: return 1;  // Op89
            case 0x8A: return 6;  // Nop8A
            case 0x8B: return 6;  // Nop8B
            case 0x8C: return 1;  // Nop8C
            case 0x8D: return 24; // Op8D
            case 0x8E: return 2;  // Nop8E
            default: return -1;   // Unknown opcode
        }
    }

    /**
     * Determine the size of Obj_model_set opcode (0x2D) which has variable length.
     * The opcode ends after a sequence of zero bytes followed by a valid opcode.
     */
    private int determineObjModelSize(byte[] data, int offset) {
        final int BASE_SIZE = 8;  // opcode + obj_id + 6 header bytes
        final int MAX_PAYLOAD_WORDS = 15;
        final int TERMINATOR_ZERO_BYTES = 8;

        int searchStart = offset + BASE_SIZE;
        if (searchStart >= data.length) {
            return data.length - offset;
        }

        int pos = searchStart;
        int zeroBytes = 0;
        int maxEnd = Math.min(data.length, offset + BASE_SIZE + (MAX_PAYLOAD_WORDS * 2) + TERMINATOR_ZERO_BYTES);

        while (pos + 1 < maxEnd) {
            if (data[pos] == 0 && data[pos + 1] == 0) {
                zeroBytes += 2;
                pos += 2;
                if (zeroBytes >= TERMINATOR_ZERO_BYTES) {
                    // Skip any remaining zero bytes
                    while (pos < data.length && data[pos] == 0) {
                        pos++;
                    }
                    // Check if next byte is a valid opcode
                    if (pos < data.length) {
                        int nextOpcode = data[pos] & 0xFF;
                        if (getOpcodeSize(nextOpcode, data, pos) > 0) {
                            return pos - offset;
                        }
                    }
                    return pos - offset;
                }
            } else {
                zeroBytes = 0;
                pos += 2;
            }
        }

        // Fallback: use fixed size
        return Math.min(38, data.length - offset);
    }

    @FunctionalInterface
    private interface BlockMetadataFactory {
        SCDScriptDisassembler.BlockMetadata create(int blockIndex, int relativeOffset, List<Integer> pointerTable, int sectionStart);
    }

    private static void writeChunk(Path path, byte[] data) throws IOException {
        Files.createDirectories(path.getParent());
        Files.write(path, data);
    }

    private void writeTimPreview(byte[] timData, Path timPath) {
        try {
            String name = timPath.getFileName().toString();
            String base = name.contains(".") ? name.substring(0, name.lastIndexOf('.')) : name;
            Path bmpPath = timPath.resolveSibling(base + ".bmp");
            TimToBmpConverter.writeBmp(timData, bmpPath);
        } catch (Exception ignored) {
        }
    }

    private static final class RdtFile {
        final byte[] data;
        final Header header;
        final Addresses addresses;
        final NavigableSet<Integer> boundaries = new TreeSet<>();
        final int[] effectIds = new int[8];
        final List<ModelEntry> models = new ArrayList<>();

        int mainScdCount;
        int subScdCount;
        int extraScdCount;
        int messageCount;

        static RdtFile parse(byte[] data) {
            RdtFile file = new RdtFile(data, Header.read(data), Addresses.read(data));
            file.initialize();
            return file;
        }

        private RdtFile(byte[] data, Header header, Addresses addresses) {
            this.data = data;
            this.header = header;
            this.addresses = addresses;
        }

        private void initialize() {
            addBoundary(addresses.cameraStart);
            addBoundary(addresses.zoneStart);
            addBoundary(addresses.lightStart);
            addBoundary(addresses.collisionStart);
            addBoundary(addresses.floorStart);
            addBoundary(addresses.blockStart);
            addBoundary(addresses.mainScdStart);
            addBoundary(addresses.subScdStart);
            addBoundary(addresses.extraScdStart);
            addBoundary(addresses.messageStart);
            addBoundary(addresses.effectStart);
            addBoundary(addresses.espTimStart);
            addBoundary(addresses.modelTimStart);
            addBoundary(addresses.animationStart);
            addBoundary(addresses.snd0Edt);
            addBoundary(addresses.snd0Vh);
            addBoundary(addresses.snd0Vb);
            addBoundary(addresses.snd1Edt);
            addBoundary(addresses.snd1Vh);
            addBoundary(addresses.snd1Vb);
            boundaries.add(data.length);

            readEffectIds();
            parseModelPointers();
        }

        private void readEffectIds() {
            if (addresses.effectStart <= 0 || addresses.effectStart + 8 > data.length) {
                Arrays.fill(effectIds, 0xFF);
                return;
            }
            for (int i = 0; i < effectIds.length; i++) {
                effectIds[i] = Byte.toUnsignedInt(data[addresses.effectStart + i]);
            }
        }

        private void parseModelPointers() {
            if (header.modelCount <= 0 || addresses.md1PointerStart <= 0) {
                return;
            }
            // Some rooms reuse the pointer slot for other structures (e.g. when no models exist).
            // Abort when the pointer region overlaps with other well-known sections.
            if (addresses.md1PointerStart == addresses.zoneStart
                || addresses.md1PointerStart == addresses.lightStart
                || addresses.md1PointerStart == addresses.collisionStart) {
                return;
            }
            int end = higherBoundary(addresses.md1PointerStart);
            boolean expectTim = true;
            int currentTim = -1;
            for (int pos = addresses.md1PointerStart; pos + 4 <= end; pos += 4) {
                int pointer = readInt(data, pos);
                if (pointer <= 0 || pointer >= data.length) {
                    continue;
                }
                if (expectTim) {
                    currentTim = pointer;
                    addBoundary(pointer);
                } else {
                    addBoundary(pointer);
                    models.add(new ModelEntry(currentTim, pointer));
                }
                expectTim = !expectTim;
            }
            models.sort(Comparator.comparingInt(m -> m.md1Start));
        }

        int nextBoundary(int start) {
            return higherBoundary(start);
        }

        byte[] slice(int start, int end) {
            if (start <= 0 || end <= start || start >= data.length) {
                return new byte[0];
            }
            int safeEnd = Math.min(end, data.length);
            return Arrays.copyOfRange(data, start, safeEnd);
        }

        List<Integer> readPointerOffsets(int blockStart, int blockEnd) {
            if (blockStart <= 0 || blockEnd <= blockStart) {
                return List.of();
            }
            List<Integer> offsets = new ArrayList<>();
            int limit = blockEnd - blockStart;
            int prev = -1;
            for (int pos = 0; pos + 1 < limit; pos += 2) {
                int value = readUnsignedShort(data, blockStart + pos);
                if (value == 0 || value >= limit || value <= prev) {
                    break;
                }
                offsets.add(value);
                prev = value;
            }
            return offsets;
        }

        List<byte[]> extractTimImages(int start, int end) {
            if (start <= 0 || end <= start) {
                return List.of();
            }
        List<Integer> starts = new ArrayList<>();
        for (int pos = start; pos + 12 < end; pos++) {
            int magic = readInt(data, pos);
            if (magic != 0x10) {
                continue;
            }
            int flags = readInt(data, pos + 4);
            int clutLength = readInt(data, pos + 8);
            if (clutLength <= 0 || clutLength > (end - pos)) {
                continue;
            }
            // Flags rarely exceed 0xFF for RE TIMs, filter obvious false positives.
            if ((flags & 0xFFFFFF00) != 0 && (flags & 0xFFFF0000) != 0) {
                continue;
            }
            starts.add(pos);
        }

        if (starts.isEmpty()) {
            return List.of();
        }
        Collections.sort(starts);
        List<byte[]> images = new ArrayList<>(starts.size());
        for (int i = 0; i < starts.size(); i++) {
            int imgStart = starts.get(i);
            int imgEnd = (i + 1 < starts.size()) ? starts.get(i + 1) : end;
            if (imgEnd <= imgStart) {
                continue;
            }
            images.add(Arrays.copyOfRange(data, imgStart, imgEnd));
            addBoundary(imgStart);
        }
        return images;
        }

        private int higherBoundary(int start) {
            Integer next = boundaries.higher(start);
            return next == null ? data.length : next;
        }

        private void addBoundary(int value) {
            if (value > 0 && value < data.length) {
                boundaries.add(value);
            }
        }
    }

    private static final class Header {
        final int spriteCount;
        final int cameraCount;
        final int modelCount;
        final int itemCount;
        final int doorCount;
        final int roomAtCount;
        final int reverbLevel;

        private Header(int spriteCount,
                       int cameraCount,
                       int modelCount,
                       int itemCount,
                       int doorCount,
                       int roomAtCount,
                       int reverbLevel) {
            this.spriteCount = spriteCount;
            this.cameraCount = cameraCount;
            this.modelCount = modelCount;
            this.itemCount = itemCount;
            this.doorCount = doorCount;
            this.roomAtCount = roomAtCount;
            this.reverbLevel = reverbLevel;
        }

        static Header read(byte[] data) {
            if (data.length < 8) {
                return new Header(0, 0, 0, 0, 0, 0, 0);
            }
            return new Header(
                Byte.toUnsignedInt(data[0]),
                Byte.toUnsignedInt(data[1]),
                Byte.toUnsignedInt(data[2]),
                Byte.toUnsignedInt(data[3]),
                Byte.toUnsignedInt(data[4]),
                Byte.toUnsignedInt(data[5]),
                Byte.toUnsignedInt(data[6])
            );
        }
    }

    private static final class Addresses {
        final int snd0Edt;
        final int snd0Vh;
        final int snd0Vb;
        final int snd1Edt;
        final int snd1Vh;
        final int snd1Vb;
        final int collisionStart;
        final int cameraStart;
        final int zoneStart;
        final int lightStart;
        final int md1PointerStart;
        final int floorStart;
        final int blockStart;
        final int messageStart;
        final int mainScdStart;
        final int subScdStart;
        final int extraScdStart;
        final int effectStart;
        final int espTimStart;
        final int modelTimStart;
        final int animationStart;

        private Addresses(int snd0Edt,
                          int snd0Vh,
                          int snd0Vb,
                          int snd1Edt,
                          int snd1Vh,
                          int snd1Vb,
                          int collisionStart,
                          int cameraStart,
                          int zoneStart,
                          int lightStart,
                          int md1PointerStart,
                          int floorStart,
                          int blockStart,
                          int messageStart,
                          int mainScdStart,
                          int subScdStart,
                          int extraScdStart,
                          int effectStart,
                          int espTimStart,
                          int modelTimStart,
                          int animationStart) {
            this.snd0Edt = snd0Edt;
            this.snd0Vh = snd0Vh;
            this.snd0Vb = snd0Vb;
            this.snd1Edt = snd1Edt;
            this.snd1Vh = snd1Vh;
            this.snd1Vb = snd1Vb;
            this.collisionStart = collisionStart;
            this.cameraStart = cameraStart;
            this.zoneStart = zoneStart;
            this.lightStart = lightStart;
            this.md1PointerStart = md1PointerStart;
            this.floorStart = floorStart;
            this.blockStart = blockStart;
            this.messageStart = messageStart;
            this.mainScdStart = mainScdStart;
            this.subScdStart = subScdStart;
            this.extraScdStart = extraScdStart;
            this.effectStart = effectStart;
            this.espTimStart = espTimStart;
            this.modelTimStart = modelTimStart;
            this.animationStart = animationStart;
        }

        static Addresses read(byte[] data) {
            return new Addresses(
                readInt(data, 8),
                readInt(data, 12),
                readInt(data, 16),
                readInt(data, 20),
                readInt(data, 24),
                readInt(data, 28),
                readInt(data, 32),
                readInt(data, 36),
                readInt(data, 40),
                readInt(data, 44),
                readInt(data, 48),
                readInt(data, 52),
                readInt(data, 56),
                readInt(data, 60),
                readInt(data, 64),
                readInt(data, 68),
                readInt(data, 72),
                readInt(data, 76),
                readInt(data, 84),
                readInt(data, 88),
                readInt(data, 92)
            );
        }
    }

    private static final class ModelEntry {
        final int timStart;
        final int md1Start;

        ModelEntry(int timStart, int md1Start) {
            this.timStart = timStart;
            this.md1Start = md1Start;
        }
    }

    private static int readInt(byte[] data, int offset) {
        if (offset < 0 || offset + 4 > data.length) {
            return 0;
        }
        return ByteBuffer.wrap(data, offset, 4).order(ByteOrder.LITTLE_ENDIAN).getInt();
    }

    private static int readUnsignedShort(byte[] data, int offset) {
        if (offset < 0 || offset + 2 > data.length) {
            return 0;
        }
        return ByteBuffer.wrap(data, offset, 2).order(ByteOrder.LITTLE_ENDIAN).getShort() & 0xFFFF;
    }
}
