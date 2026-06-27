package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * Parser for Resident Evil 1.5/2 block.blk files.
 *
 * Block files define rectangular blocking zones in a room that affect
 * player movement and interactions. Each zone has coordinates and flags.
 *
 * Format:
 * - Header: 4 bytes (little-endian uint32) = entry count
 * - Entries: 12 bytes each
 *   - X1: signed short (2 bytes) - First X coordinate
 *   - Z1: signed short (2 bytes) - First Z coordinate
 *   - X2: signed short (2 bytes) - Second X coordinate
 *   - Z2: signed short (2 bytes) - Second Z coordinate
 *   - Reserved: 2 bytes (usually 0)
 *   - Flags: unsigned short (2 bytes) - Zone type/flags
 */
public class BLKParser {

    public static class BlockZone {
        public final int index;
        public final short x1;
        public final short z1;
        public final short x2;
        public final short z2;
        public final short reserved;
        public final int flags;

        public BlockZone(int index, short x1, short z1, short x2, short z2, short reserved, int flags) {
            this.index = index;
            this.x1 = x1;
            this.z1 = z1;
            this.x2 = x2;
            this.z2 = z2;
            this.reserved = reserved;
            this.flags = flags;
        }

        public int getWidth() {
            return Math.abs(x2 - x1);
        }

        public int getDepth() {
            return Math.abs(z2 - z1);
        }

        public int getCenterX() {
            return (x1 + x2) / 2;
        }

        public int getCenterZ() {
            return (z1 + z2) / 2;
        }

        public String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"x1\": ").append(x1).append(",\n");
            sb.append("      \"z1\": ").append(z1).append(",\n");
            sb.append("      \"x2\": ").append(x2).append(",\n");
            sb.append("      \"z2\": ").append(z2).append(",\n");
            sb.append("      \"width\": ").append(getWidth()).append(",\n");
            sb.append("      \"depth\": ").append(getDepth()).append(",\n");
            sb.append("      \"centerX\": ").append(getCenterX()).append(",\n");
            sb.append("      \"centerZ\": ").append(getCenterZ()).append(",\n");
            sb.append("      \"flags\": ").append(flags).append(",\n");
            sb.append("      \"flagsHex\": \"0x").append(String.format("%04X", flags)).append("\"\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    public static class BlockData {
        public final List<BlockZone> zones;

        public BlockData(List<BlockZone> zones) {
            this.zones = zones;
        }

        public String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_BLOCK\",\n");
            sb.append("  \"description\": \"Blocking zones defining areas that affect player movement\",\n");
            sb.append("  \"count\": ").append(zones.size()).append(",\n");
            sb.append("  \"zones\": [\n");

            for (int i = 0; i < zones.size(); i++) {
                sb.append(zones.get(i).toJson());
                if (i < zones.size() - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }

            sb.append("  ]\n");
            sb.append("}\n");
            return sb.toString();
        }
    }

    /**
     * Parse block.blk binary data into structured BlockData.
     *
     * @param data Raw binary data from block.blk file
     * @return Parsed BlockData with all zones
     */
    public BlockData parse(byte[] data) {
        if (data == null || data.length < 4) {
            return new BlockData(new ArrayList<>());
        }

        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);

        int count = buffer.getInt();

        // Validate count against file size
        int expectedSize = 4 + (count * 12);
        if (data.length < expectedSize) {
            // Adjust count if file is smaller than expected
            count = (data.length - 4) / 12;
        }

        List<BlockZone> zones = new ArrayList<>();

        for (int i = 0; i < count; i++) {
            int offset = 4 + (i * 12);
            if (offset + 12 > data.length) {
                break;
            }

            buffer.position(offset);
            short x1 = buffer.getShort();
            short z1 = buffer.getShort();
            short x2 = buffer.getShort();
            short z2 = buffer.getShort();
            short reserved = buffer.getShort();
            int flags = buffer.getShort() & 0xFFFF; // Unsigned

            zones.add(new BlockZone(i, x1, z1, x2, z2, reserved, flags));
        }

        return new BlockData(zones);
    }

    /**
     * Parse and return JSON string representation.
     *
     * @param data Raw binary data from block.blk file
     * @return JSON string
     */
    public String parseToJson(byte[] data) {
        return parse(data).toJson();
    }
}
