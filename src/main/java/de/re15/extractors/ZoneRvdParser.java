package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parser for zone.rvd chunks (camera switch volumes).
 *
 * <p>Each entry defines a quadrilateral in camera space where walking through
 * triggers a camera change. The file ends with 0xFFFFFFFF.</p>
 *
 * <p><b>Important:</b> The first zone listed for a camera switch with cameraTo=0
 * has a different purpose and must be ignored for actual camera switching.
 * For example, if there are two zones (1,0,...) listed for a cam switch from 1 to 0,
 * only the second one is the real camera switch trigger.</p>
 *
 * <p>Format per entry (20 bytes):</p>
 * <ul>
 *   <li>const0: unsigned short (2 bytes) - Usually 0xFF01</li>
 *   <li>cam0: unsigned byte (1 byte) - Source camera index</li>
 *   <li>cam1: unsigned byte (1 byte) - Target camera index</li>
 *   <li>x1,y1,x2,y2,x3,y3,x4,y4: signed shorts (16 bytes) - Quadrilateral vertices</li>
 * </ul>
 */
public class ZoneRvdParser {

    private static final int ENTRY_SIZE = 20;

    public String parseToJson(byte[] data) {
        return parseEntries(data == null ? new byte[0] : data).toJson();
    }

    private ZoneData parseEntries(byte[] data) {
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        List<ZoneEntry> entries = new ArrayList<>();

        // Track how many times we've seen each (camFrom, camTo) pair
        // to identify first occurrences that should be skipped
        java.util.Map<String, Integer> pairCounts = new java.util.HashMap<>();

        while (buffer.remaining() >= ENTRY_SIZE) {
            int start = buffer.position();
            int constValue = Short.toUnsignedInt(buffer.getShort());
            int camFrom = Byte.toUnsignedInt(buffer.get());
            int camTo = Byte.toUnsignedInt(buffer.get());

            // Terminator check: 0xFFFFFFFF
            if (constValue == 0xFFFF && camFrom == 0xFF && camTo == 0xFF) {
                break;
            }
            if (buffer.remaining() < 16) {
                buffer.position(start);
                break;
            }

            int[] coords = new int[8];
            for (int i = 0; i < coords.length; i++) {
                coords[i] = buffer.getShort();
            }

            // Track pair occurrences
            String pairKey = camFrom + "->" + camTo;
            int occurrence = pairCounts.getOrDefault(pairKey, 0) + 1;
            pairCounts.put(pairKey, occurrence);

            // Determine if this is the first zone for a pair where camTo == 0
            // These zones have a special purpose and should not be used for camera switching
            boolean isFirstOccurrence = (occurrence == 1);
            boolean skipForCameraSwitch = isFirstOccurrence && (camTo == 0);

            entries.add(new ZoneEntry(entries.size(), constValue, camFrom, camTo, coords,
                    occurrence, skipForCameraSwitch));
        }

        int consumed = buffer.position();
        byte[] trailing = Arrays.copyOfRange(data, consumed, data.length);
        return new ZoneData(entries, trailing);
    }

    private static final class ZoneData {
        final List<ZoneEntry> entries;
        final byte[] trailingBytes;

        ZoneData(List<ZoneEntry> entries, byte[] trailingBytes) {
            this.entries = entries;
            this.trailingBytes = trailingBytes;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_ZONE_RVD\",\n");
            sb.append("  \"description\": \"Camera switch trigger volumes (RVD)\",\n");
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

    private static final class ZoneEntry {
        final int index;
        final int constValue;
        final int cameraFrom;
        final int cameraTo;
        final int[] coords;
        final int pairOccurrence;
        final boolean skipForCameraSwitch;

        ZoneEntry(int index, int constValue, int cameraFrom, int cameraTo, int[] coords,
                  int pairOccurrence, boolean skipForCameraSwitch) {
            this.index = index;
            this.constValue = constValue;
            this.cameraFrom = cameraFrom;
            this.cameraTo = cameraTo;
            this.coords = coords;
            this.pairOccurrence = pairOccurrence;
            this.skipForCameraSwitch = skipForCameraSwitch;
        }

        String toJson() {
            int[] xs = {coords[0], coords[2], coords[4], coords[6]};
            int[] ys = {coords[1], coords[3], coords[5], coords[7]};
            int minX = Arrays.stream(xs).min().orElse(0);
            int maxX = Arrays.stream(xs).max().orElse(0);
            int minY = Arrays.stream(ys).min().orElse(0);
            int maxY = Arrays.stream(ys).max().orElse(0);
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"const\": \"0x").append(String.format(Locale.ROOT, "%04X", constValue)).append("\",\n");
            sb.append("      \"cameraFrom\": ").append(cameraFrom).append(",\n");
            sb.append("      \"cameraTo\": ").append(cameraTo).append(",\n");
            sb.append("      \"pairOccurrence\": ").append(pairOccurrence).append(",\n");
            if (skipForCameraSwitch) {
                sb.append("      \"skipForCameraSwitch\": true,\n");
                sb.append("      \"_note\": \"First zone with cameraTo=0 has special purpose, not used for actual camera switching\",\n");
            }
            sb.append("      \"points\": [");
            for (int i = 0; i < 4; i++) {
                if (i > 0) {
                    sb.append(", ");
                }
                sb.append("{\"x\": ").append(xs[i]).append(", \"y\": ").append(ys[i]).append("}");
            }
            sb.append("],\n");
            sb.append("      \"bounds\": { \"minX\": ").append(minX).append(", \"maxX\": ").append(maxX)
                .append(", \"minY\": ").append(minY).append(", \"maxY\": ").append(maxY).append(" }\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    private static String toHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder(bytes.length * 2);
        for (byte b : bytes) {
            sb.append(String.format(Locale.ROOT, "%02x", b));
        }
        return sb.toString();
    }
}
