package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parser for floor.flr chunks (footstep sound regions). Each file begins with a
 * 2-byte header holding the declared entry count followed by 12-byte entries.
 * Every entry defines a rectangular floor area and which SAP footstep table to use.
 */
public class FloorParser {

    private static final int HEADER_SIZE = 2;
    private static final int ENTRY_SIZE = 12;

    public String parseToJson(byte[] raw) {
        byte[] data = raw == null ? new byte[0] : raw;
        return parse(data).toJson();
    }

    private FloorData parse(byte[] data) {
        if (data.length < HEADER_SIZE) {
            return new FloorData(0, List.of(), data);
        }
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        int declaredCount = Short.toUnsignedInt(buffer.getShort());

        int maxEntries = Math.max(0, (data.length - HEADER_SIZE) / ENTRY_SIZE);
        int entryCount = Math.min(declaredCount, maxEntries);
        if (entryCount <= 0 && maxEntries > 0) {
            entryCount = maxEntries;
        }

        List<FloorEntry> entries = new ArrayList<>(entryCount);
        for (int i = 0; i < entryCount; i++) {
            entries.add(FloorEntry.from(buffer, i));
        }

        int parsedBytes = Math.min(HEADER_SIZE + (entryCount * ENTRY_SIZE), data.length);
        byte[] trailing = Arrays.copyOfRange(data, parsedBytes, data.length);
        return new FloorData(declaredCount, entries, trailing);
    }

    private static final class FloorData {
        final int declaredCount;
        final List<FloorEntry> entries;
        final byte[] trailingBytes;

        FloorData(int declaredCount, List<FloorEntry> entries, byte[] trailingBytes) {
            this.declaredCount = declaredCount;
            this.entries = entries;
            this.trailingBytes = trailingBytes;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_FLOOR_FLR\",\n");
            sb.append("  \"description\": \"Footstep sound regions mapped to FSxx.SAP tables\",\n");
            sb.append("  \"declaredCount\": ").append(declaredCount).append(",\n");
            sb.append("  \"count\": ").append(entries.size()).append(",\n");
            sb.append("  \"entries\": [\n");
            for (int i = 0; i < entries.size(); i++) {
                sb.append(entries.get(i).toJson());
                if (i < entries.size() - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }
            sb.append("  ]");
            if (trailingBytes.length > 0) {
                sb.append(",\n");
                sb.append("  \"trailingBytes\": \"").append(toHex(trailingBytes)).append("\"");
            }
            sb.append("\n");
            sb.append("}\n");
            return sb.toString();
        }
    }

    private static final class FloorEntry {
        final int index;
        final int x;
        final int z;
        final int width;
        final int depth;
        final int soundId;
        final int dummy;

        FloorEntry(int index, int x, int z, int width, int depth, int soundId, int dummy) {
            this.index = index;
            this.x = x;
            this.z = z;
            this.width = width;
            this.depth = depth;
            this.soundId = soundId;
            this.dummy = dummy;
        }

        static FloorEntry from(ByteBuffer buffer, int index) {
            int x = buffer.getShort();
            int z = buffer.getShort();
            int width = Short.toUnsignedInt(buffer.getShort());
            int depth = Short.toUnsignedInt(buffer.getShort());
            int soundId = Short.toUnsignedInt(buffer.getShort());
            int dummy = Short.toUnsignedInt(buffer.getShort());
            return new FloorEntry(index, x, z, width, depth, soundId, dummy);
        }

        String soundFileName() {
            return String.format(Locale.ROOT, "FS%02d.SAP", soundId);
        }

        String toJson() {
            int minX = x;
            int minZ = z;
            int maxX = x + width;
            int maxZ = z + depth;
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"x\": ").append(x).append(",\n");
            sb.append("      \"z\": ").append(z).append(",\n");
            sb.append("      \"width\": ").append(width).append(",\n");
            sb.append("      \"depth\": ").append(depth).append(",\n");
            sb.append("      \"minX\": ").append(minX).append(",\n");
            sb.append("      \"maxX\": ").append(maxX).append(",\n");
            sb.append("      \"minZ\": ").append(minZ).append(",\n");
            sb.append("      \"maxZ\": ").append(maxZ).append(",\n");
            sb.append("      \"soundId\": ").append(soundId).append(",\n");
            sb.append("      \"soundFile\": \"").append(soundFileName()).append("\",\n");
            sb.append("      \"dummy\": ").append(dummy).append("\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    private static String toHex(byte[] bytes) {
        if (bytes.length == 0) {
            return "";
        }
        StringBuilder sb = new StringBuilder(bytes.length * 2);
        for (byte b : bytes) {
            sb.append(String.format(Locale.ROOT, "%02x", b));
        }
        return sb.toString();
    }
}
