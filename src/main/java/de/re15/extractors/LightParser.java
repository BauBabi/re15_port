package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parser for light.lit chunks. Each camera stores 40 bytes describing up to
 * three lights, their colors, positions and brightness plus the ambient color.
 */
public class LightParser {

    private static final int ENTRY_SIZE = 40;

    public String parseToJson(byte[] raw, int cameraCount) {
        byte[] data = raw == null ? new byte[0] : raw;
        return parse(data, cameraCount).toJson();
    }

    private LightData parse(byte[] data, int cameraCount) {
        if (data.length < ENTRY_SIZE) {
            return new LightData(List.of(), data);
        }
        int availableEntries = data.length / ENTRY_SIZE;
        int count = cameraCount > 0 ? Math.min(cameraCount, availableEntries) : availableEntries;
        List<CameraLights> entries = new ArrayList<>(count);
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < count; i++) {
            entries.add(CameraLights.from(buffer, i));
        }
        int parsedBytes = Math.min(count * ENTRY_SIZE, data.length);
        byte[] trailing = Arrays.copyOfRange(data, parsedBytes, data.length);
        return new LightData(entries, trailing);
    }

    private static final class LightData {
        final List<CameraLights> cameras;
        final byte[] trailingBytes;

        LightData(List<CameraLights> cameras, byte[] trailingBytes) {
            this.cameras = cameras;
            this.trailingBytes = trailingBytes;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_LIGHT_LIT\",\n");
            sb.append("  \"description\": \"Per-camera light definitions including colors, positions, brightness and ambient RGB\",\n");
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
                sb.append("  \"trailingBytes\": \"").append(toHex(trailingBytes)).append("\"");
            }
            sb.append("\n");
            sb.append("}\n");
            return sb.toString();
        }
    }

    /* BF-round 2026-05-28: the first 4 bytes of the 40-byte entry are NOT two
     * int16 "lightTypes" as previously believed — they are:
     *   +0x00  uint8 globalScale  (light direction magnitude multiplier)
     *   +0x01  uint8 typeFlags[3] (per-light: 0 = positional, !=0 = directional)
     * Verified via RE_15_Quellcode_V2/render_calc_dynamic_lights.c
     * (FUN_80053fc0:46-128) — the dispatch loop reads `*(char*)(iVar7+iVar14+1)`
     * for each light index iVar14 ∈ {0,1,2}, treating it as a per-light type.
     * Old `int16 lightTypes[2]` field is kept as deprecated mirror so existing
     * JSON consumers don't break, but new code should read `globalScale` +
     * `typeFlags[3]` instead. */
    private static final class CameraLights {
        final int index;
        final int globalScale;
        final int[] typeFlags;          // BF: per-light positional/directional
        final int[] lightTypes;          // deprecated alias, same 4 bytes packed
        final Color[] colors;
        final Color ambient;
        final Position[] positions;
        final int[] brightness;

        CameraLights(int index, int globalScale, int[] typeFlags, int[] lightTypes,
                     Color[] colors, Color ambient,
                     Position[] positions, int[] brightness) {
            this.index = index;
            this.globalScale = globalScale;
            this.typeFlags = typeFlags;
            this.lightTypes = lightTypes;
            this.colors = colors;
            this.ambient = ambient;
            this.positions = positions;
            this.brightness = brightness;
        }

        static CameraLights from(ByteBuffer buffer, int index) {
            /* BF-round: read the first 4 bytes as discrete fields. */
            int globalScale = Byte.toUnsignedInt(buffer.get());      // +0x00
            int[] typeFlags = new int[3];
            for (int i = 0; i < typeFlags.length; i++) {
                typeFlags[i] = Byte.toUnsignedInt(buffer.get());     // +0x01..+0x03
            }
            /* Deprecated alias — mirror the same 4 bytes packed back into two
             * little-endian shorts so old JSON consumers still see lightTypes. */
            int legacy0 = globalScale | (typeFlags[0] << 8);
            int legacy1 = typeFlags[1] | (typeFlags[2] << 8);
            int[] lightTypes = { legacy0, legacy1 };
            Color[] colors = new Color[3];
            for (int i = 0; i < colors.length; i++) {
                int r = Byte.toUnsignedInt(buffer.get());
                int g = Byte.toUnsignedInt(buffer.get());
                int b = Byte.toUnsignedInt(buffer.get());
                colors[i] = new Color(r, g, b);
            }
            Color ambient = new Color(Byte.toUnsignedInt(buffer.get()), Byte.toUnsignedInt(buffer.get()), Byte.toUnsignedInt(buffer.get()));
            Position[] positions = new Position[3];
            for (int i = 0; i < positions.length; i++) {
                short x = buffer.getShort();
                short y = buffer.getShort();
                short z = buffer.getShort();
                positions[i] = new Position(x, y, z);
            }
            int[] brightness = new int[3];
            for (int i = 0; i < brightness.length; i++) {
                brightness[i] = Short.toUnsignedInt(buffer.getShort());
            }
            return new CameraLights(index, globalScale, typeFlags, lightTypes,
                                    colors, ambient, positions, brightness);
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            /* BF-round: emit corrected field interpretation. */
            sb.append("      \"globalScale\": ").append(globalScale).append(",\n");
            sb.append("      \"typeFlags\": ").append(Arrays.toString(typeFlags))
              .append(",\n");
            /* DEPRECATED: lightTypes was a misread of the 4-byte header as two
             * little-endian shorts. Retained for backwards-compat. */
            sb.append("      \"lightTypes\": ").append(Arrays.toString(lightTypes))
              .append(",\n");
            sb.append("      \"colors\": [\n");
            for (int i = 0; i < colors.length; i++) {
                sb.append(colors[i].toJson(i));
                if (i < colors.length - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }
            sb.append("      ],\n");
            sb.append("      \"ambient\": ").append(ambient.toJsonObject()).append(",\n");
            sb.append("      \"positions\": [\n");
            for (int i = 0; i < positions.length; i++) {
                sb.append(positions[i].toJson(i));
                if (i < positions.length - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }
            sb.append("      ],\n");
            sb.append("      \"brightness\": ").append(Arrays.toString(brightness)).append("\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    private static final class Color {
        final int r;
        final int g;
        final int b;

        Color(int r, int g, int b) {
            this.r = r;
            this.g = g;
            this.b = b;
        }

        String toJson(int index) {
            return "        { \"id\": " + index + ", \"r\": " + r + ", \"g\": " + g + ", \"b\": " + b
                + ", \"hex\": \"#" + String.format(Locale.ROOT, "%02X%02X%02X", r, g, b) + "\" }";
        }

        String toJsonObject() {
            return "{ \"r\": " + r + ", \"g\": " + g + ", \"b\": " + b
                + ", \"hex\": \"#" + String.format(Locale.ROOT, "%02X%02X%02X", r, g, b) + "\" }";
        }
    }

    private static final class Position {
        final int x;
        final int y;
        final int z;

        Position(int x, int y, int z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        String toJson(int index) {
            return "        { \"id\": " + index + ", \"x\": " + x + ", \"y\": " + y + ", \"z\": " + z + " }";
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
