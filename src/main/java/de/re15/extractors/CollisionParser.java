package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parser for collision.sca chunks (SCA collision boundaries).
 *
 * <p>The format begins with a 24-byte header that stores ceiling information
 * and five counters. The counters denote how many collision planes follow,
 * grouped by internal categories. Each entry is 12 bytes long and stores two
 * corner coordinates alongside the raw type and floor height definition. The
 * section usually ends with a 4-byte size field.</p>
 */
public class CollisionParser {

    private static final int HEADER_SIZE = 24;
    private static final int ENTRY_SIZE = 12;
    private static final int FOOTER_SIZE = 4;

    public String parseToJson(byte[] raw) {
        byte[] data = raw == null ? new byte[0] : raw;
        return parse(data).toJson();
    }

    private CollisionData parse(byte[] data) {
        if (data.length < HEADER_SIZE) {
            return new CollisionData(Header.empty(), List.of(), 0, data);
        }

        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        Header header = Header.from(buffer);

        int availableAfterHeader = Math.max(0, data.length - HEADER_SIZE);
        int possibleEntries;
        if (availableAfterHeader >= FOOTER_SIZE) {
            possibleEntries = (availableAfterHeader - FOOTER_SIZE) / ENTRY_SIZE;
        } else {
            possibleEntries = availableAfterHeader / ENTRY_SIZE;
        }

        int declaredEntries = header.totalCount();
        int entryCount = Math.min(declaredEntries, possibleEntries);
        if (entryCount <= 0 && possibleEntries > 0) {
            entryCount = possibleEntries;
        }

        List<CollisionEntry> entries = new ArrayList<>(entryCount);
        for (int i = 0; i < entryCount; i++) {
            entries.add(CollisionEntry.from(buffer, i));
        }

        int footerOffset = HEADER_SIZE + (entryCount * ENTRY_SIZE);
        int declaredSize = 0;
        if (footerOffset + FOOTER_SIZE <= data.length) {
            declaredSize = ByteBuffer.wrap(data, footerOffset, FOOTER_SIZE)
                .order(ByteOrder.LITTLE_ENDIAN)
                .getInt();
        } else {
            footerOffset = data.length;
        }

        int trailingStart = Math.min(footerOffset + FOOTER_SIZE, data.length);
        byte[] trailing = Arrays.copyOfRange(data, trailingStart, data.length);

        return new CollisionData(header, entries, declaredSize, trailing);
    }

    private static final class CollisionData {
        final Header header;
        final List<CollisionEntry> entries;
        final int declaredSize;
        final byte[] trailingBytes;

        CollisionData(Header header, List<CollisionEntry> entries, int declaredSize, byte[] trailingBytes) {
            this.header = header;
            this.entries = entries;
            this.declaredSize = declaredSize;
            this.trailingBytes = trailingBytes;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_COLLISION_SCA\",\n");
            sb.append("  \"description\": \"Collision boundaries controlling the playable space\",\n");
            sb.append("  \"header\": ").append(header.toJson()).append(",\n");
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
            if (declaredSize > 0) {
                sb.append(",\n");
                sb.append("  \"declaredSize\": ").append(declaredSize);
            }
            if (trailingBytes.length > 0) {
                sb.append(",\n");
                sb.append("  \"trailingBytes\": \"").append(toHex(trailingBytes)).append("\"");
            }
            sb.append("\n");
            sb.append("}\n");
            return sb.toString();
        }
    }

    private static final class Header {
        final int ceilingX;
        final int ceilingZ;
        final long[] counts;

        Header(int ceilingX, int ceilingZ, long[] counts) {
            this.ceilingX = ceilingX;
            this.ceilingZ = ceilingZ;
            this.counts = counts;
        }

        static Header from(ByteBuffer buffer) {
            int cx = Short.toUnsignedInt(buffer.getShort());
            int cz = Short.toUnsignedInt(buffer.getShort());
            long[] counts = new long[5];
            for (int i = 0; i < counts.length; i++) {
                counts[i] = Integer.toUnsignedLong(buffer.getInt());
            }
            return new Header(cx, cz, counts);
        }

        static Header empty() {
            return new Header(0, 0, new long[]{0, 0, 0, 0, 0});
        }

        int totalCount() {
            long sum = 0;
            for (long value : counts) {
                sum += value;
            }
            return (int) Math.min(Integer.MAX_VALUE, sum);
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("    \"ceilingX\": ").append(ceilingX).append(",\n");
            sb.append("    \"ceilingZ\": ").append(ceilingZ).append(",\n");
            sb.append("    \"counts\": [");
            for (int i = 0; i < counts.length; i++) {
                sb.append(counts[i]);
                if (i < counts.length - 1) {
                    sb.append(", ");
                }
            }
            sb.append("],\n");
            sb.append("    \"totalDeclared\": ").append(totalCount()).append("\n");
            sb.append("  }");
            return sb.toString();
        }
    }

    private static final class CollisionEntry {
        final int index;
        final int x1;
        final int z1;
        final int x2;
        final int z2;
        final int rawType;
        final int type;
        final int floorRaw;
        final double floorHeight;
        final String typeLabel;

        CollisionEntry(int index, int x1, int z1, int x2, int z2, int rawType, int type,
                       int floorRaw, double floorHeight, String typeLabel) {
            this.index = index;
            this.x1 = x1;
            this.z1 = z1;
            this.x2 = x2;
            this.z2 = z2;
            this.rawType = rawType;
            this.type = type;
            this.floorRaw = floorRaw;
            this.floorHeight = floorHeight;
            this.typeLabel = typeLabel;
        }

        static CollisionEntry from(ByteBuffer buffer, int index) {
            int x1 = buffer.getShort();
            int z1 = buffer.getShort();
            int x2 = buffer.getShort();
            int z2 = buffer.getShort();
            int rawType = Short.toUnsignedInt(buffer.getShort());
            int floorRaw = Short.toUnsignedInt(buffer.getShort());
            int type = rawType & 0xFF;
            double floorHeight = floorRaw / 256.0;
            return new CollisionEntry(index, x1, z1, x2, z2, rawType, type, floorRaw, floorHeight, describeType(type));
        }

        String toJson() {
            int minX = Math.min(x1, x2);
            int maxX = Math.max(x1, x2);
            int minZ = Math.min(z1, z2);
            int maxZ = Math.max(z1, z2);
            int width = Math.abs(x2 - x1);
            int depth = Math.abs(z2 - z1);

            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"x1\": ").append(x1).append(",\n");
            sb.append("      \"z1\": ").append(z1).append(",\n");
            sb.append("      \"x2\": ").append(x2).append(",\n");
            sb.append("      \"z2\": ").append(z2).append(",\n");
            sb.append("      \"minX\": ").append(minX).append(",\n");
            sb.append("      \"maxX\": ").append(maxX).append(",\n");
            sb.append("      \"minZ\": ").append(minZ).append(",\n");
            sb.append("      \"maxZ\": ").append(maxZ).append(",\n");
            sb.append("      \"width\": ").append(width).append(",\n");
            sb.append("      \"depth\": ").append(depth).append(",\n");
            sb.append("      \"type\": ").append(type).append(",\n");
            sb.append("      \"typeHex\": \"0x").append(String.format(Locale.ROOT, "%04X", rawType)).append("\"");
            if (typeLabel != null) {
                sb.append(",\n");
                sb.append("      \"typeLabel\": \"").append(typeLabel).append("\"");
            }
            sb.append(",\n");
            sb.append("      \"floorRaw\": ").append(floorRaw).append(",\n");
            sb.append("      \"floorHeight\": ").append(floorHeight).append("\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    private static String describeType(int type) {
        return switch (type) {
            case 0, 1, 128 -> "STANDARD";
            case 5 -> "DIAGONAL_DIAMOND";
            case 6 -> "CIRCLE";
            case 7 -> "HORIZONTAL";
            case 8 -> "VERTICAL";
            case 9 -> "CLIMBABLE";
            case 10 -> "CLIMB_EDGE";
            case 11 -> "SLOPE";
            case 12, 140 -> "STAIRS";
            case 129 -> "DIAGONAL_SLASH";
            case 130, 131 -> "DIAGONAL_BACKSLASH";
            case 132 -> "DIAGONAL_SLASH";
            case 133 -> "DIAGONAL_DIAMOND";
            case 134 -> "CIRCLE";
            case 135 -> "HORIZONTAL";
            case 136 -> "VERTICAL";
            default -> null;
        };
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
