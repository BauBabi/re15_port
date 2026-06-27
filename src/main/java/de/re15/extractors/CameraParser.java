package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * Parser for Resident Evil 1.5/2 camera.rid files.
 *
 * Camera files define fixed camera positions and their look-at targets.
 * Each room can have up to 16 cameras that switch based on player position.
 *
 * Format:
 * - Entry size: 32 bytes per camera
 * - Structure:
 *   - End flag: unsigned short (2 bytes) - 0=normal, 1=last camera
 *   - FOV: unsigned short (2 bytes) - Field of View value
 *   - Camera X: signed int (4 bytes) - Camera position X
 *   - Camera Y: signed int (4 bytes) - Camera position Y (height)
 *   - Camera Z: signed int (4 bytes) - Camera position Z
 *   - Target X: signed int (4 bytes) - Look-at target X
 *   - Target Y: signed int (4 bytes) - Look-at target Y (height)
 *   - Target Z: signed int (4 bytes) - Look-at target Z
 *   - PRI Offset: unsigned int (4 bytes) - Offset to sprite mask data in RDT
 */
public class CameraParser {

    private static final int ENTRY_SIZE = 32;

    public static class Camera {
        public final int index;
        public final int endFlag;
        public final int fov;
        public final int posX;
        public final int posY;
        public final int posZ;
        public final int targetX;
        public final int targetY;
        public final int targetZ;
        public final long priOffset;

        public Camera(int index, int endFlag, int fov,
                      int posX, int posY, int posZ,
                      int targetX, int targetY, int targetZ,
                      long priOffset) {
            this.index = index;
            this.endFlag = endFlag;
            this.fov = fov;
            this.posX = posX;
            this.posY = posY;
            this.posZ = posZ;
            this.targetX = targetX;
            this.targetY = targetY;
            this.targetZ = targetZ;
            this.priOffset = priOffset;
        }

        /** Distance from camera to target */
        public double getDistance() {
            double dx = targetX - posX;
            double dy = targetY - posY;
            double dz = targetZ - posZ;
            return Math.sqrt(dx * dx + dy * dy + dz * dz);
        }

        /** Yaw angle in degrees (horizontal rotation) */
        public double getYawDegrees() {
            double dx = targetX - posX;
            double dz = targetZ - posZ;
            return Math.toDegrees(Math.atan2(dx, dz));
        }

        /** Pitch angle in degrees (vertical rotation) */
        public double getPitchDegrees() {
            double dx = targetX - posX;
            double dy = targetY - posY;
            double dz = targetZ - posZ;
            double horizontalDist = Math.sqrt(dx * dx + dz * dz);
            return Math.toDegrees(Math.atan2(dy, horizontalDist));
        }

        /** Approximate FOV in degrees (RE2 uses internal units) */
        public double getFovDegrees() {
            // RE2 FOV value 26684 corresponds to roughly 60 degrees
            // Formula: degrees = fov * 60.0 / 26684.0
            return fov * 60.0 / 26684.0;
        }

        public boolean isLastCamera() {
            return endFlag == 1;
        }

        public String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"isLast\": ").append(isLastCamera()).append(",\n");
            sb.append("      \"fov\": ").append(fov).append(",\n");
            sb.append("      \"fovDegrees\": ").append(String.format("%.2f", getFovDegrees())).append(",\n");
            sb.append("      \"position\": {\n");
            sb.append("        \"x\": ").append(posX).append(",\n");
            sb.append("        \"y\": ").append(posY).append(",\n");
            sb.append("        \"z\": ").append(posZ).append("\n");
            sb.append("      },\n");
            sb.append("      \"target\": {\n");
            sb.append("        \"x\": ").append(targetX).append(",\n");
            sb.append("        \"y\": ").append(targetY).append(",\n");
            sb.append("        \"z\": ").append(targetZ).append("\n");
            sb.append("      },\n");
            sb.append("      \"computed\": {\n");
            sb.append("        \"distance\": ").append(String.format("%.2f", getDistance())).append(",\n");
            sb.append("        \"yawDegrees\": ").append(String.format("%.2f", getYawDegrees())).append(",\n");
            sb.append("        \"pitchDegrees\": ").append(String.format("%.2f", getPitchDegrees())).append("\n");
            sb.append("      },\n");
            sb.append("      \"priOffset\": ").append(priOffset).append(",\n");
            sb.append("      \"priOffsetHex\": \"0x").append(String.format("%08X", priOffset)).append("\"\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    public static class CameraData {
        public final List<Camera> cameras;

        public CameraData(List<Camera> cameras) {
            this.cameras = cameras;
        }

        public String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_CAMERA\",\n");
            sb.append("  \"description\": \"Fixed camera positions and look-at targets for room rendering\",\n");
            sb.append("  \"count\": ").append(cameras.size()).append(",\n");
            sb.append("  \"cameras\": [\n");

            for (int i = 0; i < cameras.size(); i++) {
                sb.append(cameras.get(i).toJson());
                if (i < cameras.size() - 1) {
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
     * Parse camera.rid binary data into structured CameraData.
     *
     * @param data Raw binary data from camera.rid file
     * @return Parsed CameraData with all cameras
     */
    public CameraData parse(byte[] data) {
        if (data == null || data.length < ENTRY_SIZE) {
            return new CameraData(new ArrayList<>());
        }

        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        List<Camera> cameras = new ArrayList<>();

        int numEntries = data.length / ENTRY_SIZE;

        for (int i = 0; i < numEntries; i++) {
            int offset = i * ENTRY_SIZE;
            if (offset + ENTRY_SIZE > data.length) {
                break;
            }

            buffer.position(offset);
            int endFlag = buffer.getShort() & 0xFFFF;
            int fov = buffer.getShort() & 0xFFFF;
            int posX = buffer.getInt();
            int posY = buffer.getInt();
            int posZ = buffer.getInt();
            int targetX = buffer.getInt();
            int targetY = buffer.getInt();
            int targetZ = buffer.getInt();
            long priOffset = buffer.getInt() & 0xFFFFFFFFL;

            cameras.add(new Camera(i, endFlag, fov, posX, posY, posZ,
                                   targetX, targetY, targetZ, priOffset));

            // Stop after last camera
            if (endFlag == 1) {
                break;
            }
        }

        return new CameraData(cameras);
    }

    /**
     * Parse and return JSON string representation.
     *
     * @param data Raw binary data from camera.rid file
     * @return JSON string
     */
    public String parseToJson(byte[] data) {
        return parse(data).toJson();
    }
}
