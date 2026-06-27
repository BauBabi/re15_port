package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Parser for camera.rid chunks. Converts the binary camera definitions found
 * in RDT files into a concise JSON representation that mirrors the style of
 * block.json.
 */
public class RIDParser {

    private static final int BIO15_ENTRY_SIZE = 32;
    private static final int BIO1_ENTRY_SIZE = 44;

    private enum Format {
        BIO15,
        BIO1
    }

    private static final class CameraEntry {
        final int index;
        final Format format;
        final int flag;
        final int fov;
        final int x;
        final int y;
        final int z;
        final int targetX;
        final int targetY;
        final int targetZ;
        final long priOffset;
        final long timOffset;
        final Integer roll;
        final Integer zero;

        CameraEntry(int index, Format format, int flag, int fov,
                    int x, int y, int z,
                    int targetX, int targetY, int targetZ,
                    long priOffset, long timOffset,
                    Integer roll, Integer zero) {
            this.index = index;
            this.format = format;
            this.flag = flag;
            this.fov = fov;
            this.x = x;
            this.y = y;
            this.z = z;
            this.targetX = targetX;
            this.targetY = targetY;
            this.targetZ = targetZ;
            this.priOffset = priOffset;
            this.timOffset = timOffset;
            this.roll = roll;
            this.zero = zero;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"format\": \"").append(format.name()).append("\",\n");
            if (format == Format.BIO15) {
                sb.append("      \"flag\": ").append(flag).append(",\n");
            }
            sb.append("      \"fov\": ").append(fov).append(",\n");
            sb.append("      \"position\": {\n");
            sb.append("        \"x\": ").append(x).append(", \"y\": ").append(y).append(", \"z\": ").append(z).append("\n");
            sb.append("      },\n");
            sb.append("      \"target\": {\n");
            sb.append("        \"x\": ").append(targetX).append(", \"y\": ").append(targetY).append(", \"z\": ").append(targetZ).append("\n");
            sb.append("      }");

            if (format == Format.BIO15) {
                sb.append(",\n");
                sb.append("      \"priOffset\": ").append(priOffset).append(",\n");
                sb.append("      \"priOffsetHex\": \"0x").append(String.format("%08X", priOffset)).append("\"\n");
            } else {
                sb.append(",\n");
                sb.append("      \"maskOffsets\": {\n");
                sb.append("        \"pri\": ").append(priOffset).append(",\n");
                sb.append("        \"priHex\": \"0x").append(String.format("%08X", priOffset)).append("\",\n");
                sb.append("        \"tim\": ").append(timOffset).append(",\n");
                sb.append("        \"timHex\": \"0x").append(String.format("%08X", timOffset)).append("\"\n");
                sb.append("      },\n");
                sb.append("      \"roll\": ").append(roll).append(",\n");
                sb.append("      \"zero\": ").append(zero).append("\n");
            }
            sb.append("    }");
            return sb.toString();
        }
    }

    private static final class CameraData {
        final Format format;
        final List<CameraEntry> cameras;
        final byte[] trailingBytes;

        CameraData(Format format, List<CameraEntry> cameras, byte[] trailingBytes) {
            this.format = format;
            this.cameras = cameras;
            this.trailingBytes = trailingBytes;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE_CAMERA_RID\",\n");
            sb.append("  \"description\": \"Static camera definitions (position, look-at, field of view, mask offsets)\",\n");
            sb.append("  \"entryFormat\": \"").append(format.name()).append("\",\n");
            sb.append("  \"count\": ").append(cameras.size()).append(",\n");
            sb.append("  \"cameras\": [\n");
            for (int i = 0; i < cameras.size(); i++) {
                sb.append(cameras.get(i).toJson());
                if (i < cameras.size() - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }
            sb.append("  ]");
            if (trailingBytes.length > 0) {
                sb.append(",\n");
                sb.append("  \"trailingBytes\": \"").append(toHex(trailingBytes)).append("\"\n");
            } else {
                sb.append("\n");
            }
            sb.append("}\n");
            return sb.toString();
        }
    }

    public String parseToJson(byte[] data, int cameraCount) {
        return parse(data, cameraCount).toJson();
    }

    private CameraData parse(byte[] data, int cameraCount) {
        if (data == null) {
            data = new byte[0];
        }

        int maxBio15 = data.length / BIO15_ENTRY_SIZE;
        int maxBio1 = data.length / BIO1_ENTRY_SIZE;

        Format format;
        int entrySize;

        if (cameraCount <= 0) {
            if (maxBio15 > 0) {
                format = Format.BIO15;
                entrySize = BIO15_ENTRY_SIZE;
                cameraCount = maxBio15;
            } else if (maxBio1 > 0) {
                format = Format.BIO1;
                entrySize = BIO1_ENTRY_SIZE;
                cameraCount = maxBio1;
            } else {
                return new CameraData(Format.BIO15, List.of(), data);
            }
        } else if (cameraCount <= maxBio15 || (maxBio15 > 0 && maxBio1 == 0)) {
            format = Format.BIO15;
            entrySize = BIO15_ENTRY_SIZE;
            cameraCount = Math.min(cameraCount, maxBio15);
        } else if (cameraCount <= maxBio1 || maxBio1 > 0) {
            format = Format.BIO1;
            entrySize = BIO1_ENTRY_SIZE;
            cameraCount = Math.min(cameraCount, maxBio1);
        } else {
            return new CameraData(Format.BIO15, List.of(), data);
        }

        if (cameraCount <= 0) {
            return new CameraData(format, List.of(), data);
        }

        List<CameraEntry> entries = new ArrayList<>(cameraCount);
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);

        for (int i = 0; i < cameraCount; i++) {
            int offset = i * entrySize;
            if (offset + entrySize > data.length) {
                break;
            }
            buffer.position(offset);
            if (format == Format.BIO15) {
                int flag = Short.toUnsignedInt(buffer.getShort());
                int fov = Short.toUnsignedInt(buffer.getShort());
                int x = buffer.getInt();
                int y = buffer.getInt();
                int z = buffer.getInt();
                int targetX = buffer.getInt();
                int targetY = buffer.getInt();
                int targetZ = buffer.getInt();
                long priOffset = Integer.toUnsignedLong(buffer.getInt());
                entries.add(new CameraEntry(i, format, flag, fov, x, y, z, targetX, targetY, targetZ, priOffset, 0L, null, null));
            } else {
                long pri = Integer.toUnsignedLong(buffer.getInt());
                long tim = Integer.toUnsignedLong(buffer.getInt());
                int x = buffer.getInt();
                int y = buffer.getInt();
                int z = buffer.getInt();
                int targetX = buffer.getInt();
                int targetY = buffer.getInt();
                int targetZ = buffer.getInt();
                int roll = buffer.getInt();
                int zero = buffer.getInt();
                int fov = buffer.getInt();
                entries.add(new CameraEntry(i, format, 0, fov, x, y, z, targetX, targetY, targetZ, pri, tim, roll, zero));
            }
        }

        int parsedBytes = Math.min(cameraCount * entrySize, data.length);
        byte[] trailing = Arrays.copyOfRange(data, parsedBytes, data.length);
        return new CameraData(format, entries, trailing);
    }

    private static String toHex(byte[] bytes) {
        StringBuilder sb = new StringBuilder(bytes.length * 2);
        for (byte b : bytes) {
            sb.append(String.format("%02X", b));
        }
        return sb.toString().toLowerCase();
    }
}
