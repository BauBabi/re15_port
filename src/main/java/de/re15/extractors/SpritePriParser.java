package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parser for sprite.pri files. These chunks store the rectangular sprite masks
 * that hide parts of the static background (e.g. pillars that should occlude
 * the player). Each section represents one mask table referenced by the PRI
 * offsets stored inside camera.rid entries.
 */
public class SpritePriParser {

    private static final int SECTION_HEADER_SIZE = 4;
    private static final int GROUP_ENTRY_SIZE = 8;
    private static final int MAX_SECTIONS = 512;
    private static final int MAX_GROUPS = 2048;
    private static final int MAX_MASKS = 16384;

    public String parseToJson(byte[] raw) {
        byte[] data = raw == null ? new byte[0] : raw;
        return parse(data).toJson();
    }

    private SpriteData parse(byte[] data) {
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);
        List<Section> sections = new ArrayList<>();
        int safety = 0;

        while (buffer.remaining() >= SECTION_HEADER_SIZE && safety++ < MAX_SECTIONS) {
            int sectionOffset = buffer.position();
            int groupCount = Short.toUnsignedInt(buffer.getShort());
            int maskCountDeclared = Short.toUnsignedInt(buffer.getShort());

            // BB-round 2026-05-28: per-cut NULL section is `FF FF FF FF`
            // (groupCount=0xFFFF, maskCount=0xFFFF) — emit empty section and
            // continue. Old code globally aborted on Cut 0 NULL so cut 1/2/4/5
            // sections in ROOM1170 were never parsed.
            // Also treat (0,0) as NULL terminator to match runtime parser.
            boolean isNullTerminator =
                (groupCount == 0xFFFF && maskCountDeclared == 0xFFFF) ||
                (groupCount == 0 && maskCountDeclared == 0);

            if (isNullTerminator) {
                sections.add(new Section(sections.size(), sectionOffset, 0, 0,
                                         new ArrayList<>(), new ArrayList<>()));
                continue;  // 4-byte NULL header already consumed by getShort×2
            }

            if (groupCount > MAX_GROUPS || maskCountDeclared > MAX_MASKS) {
                // Absurd values — likely random padding after last section.
                // Rewind so the trailingBytes dump catches them and bail out.
                buffer.position(sectionOffset);
                break;
            }

            if (buffer.remaining() < groupCount * GROUP_ENTRY_SIZE) {
                buffer.position(sectionOffset);
                break;
            }

            List<Group> groups = new ArrayList<>(groupCount);
            for (int i = 0; i < groupCount; i++) {
                int maskCount = Short.toUnsignedInt(buffer.getShort());
                int baseRaw = Short.toUnsignedInt(buffer.getShort());
                int destX = Short.toUnsignedInt(buffer.getShort());
                int destY = Short.toUnsignedInt(buffer.getShort());
                groups.add(new Group(i, maskCount, baseRaw, destX, destY));
            }

            List<Mask> parsedMasks = new ArrayList<>(maskCountDeclared);
            boolean truncated = false;
            for (int i = 0; i < maskCountDeclared; i++) {
                if (buffer.remaining() < 8) {
                    truncated = true;
                    break;
                }
                int maskOffset = buffer.position();
                int srcX = Byte.toUnsignedInt(buffer.get());
                int srcY = Byte.toUnsignedInt(buffer.get());
                int dstX = Byte.toUnsignedInt(buffer.get());
                int dstY = Byte.toUnsignedInt(buffer.get());
                int depth = Short.toUnsignedInt(buffer.getShort());
                int unknown = Byte.toUnsignedInt(buffer.get());
                int size = Byte.toUnsignedInt(buffer.get());
                boolean rectangular = size == 0;
                int width;
                int height;

                if (rectangular) {
                    if (buffer.remaining() < 4) {
                        truncated = true;
                        break;
                    }
                    width = Short.toUnsignedInt(buffer.getShort());
                    height = Short.toUnsignedInt(buffer.getShort());
                } else {
                    width = height = size;
                }

                parsedMasks.add(new Mask(parsedMasks.size(), maskOffset, srcX, srcY, dstX, dstY, depth, unknown, size, width, height, rectangular));
            }

            if (truncated) {
                buffer.position(sectionOffset);
                break;
            }

            int cursor = 0;
            for (Group group : groups) {
                int expected = group.expectedCount;
                int remaining = parsedMasks.size() - cursor;
                int take = Math.min(expected, Math.max(remaining, 0));
                if (take <= 0) {
                    break;
                }
                List<Mask> share = new ArrayList<>(take);
                for (int i = 0; i < take; i++) {
                    share.add(parsedMasks.get(cursor++));
                }
                group.masks = share;
            }

            sections.add(new Section(sections.size(), sectionOffset, groupCount, maskCountDeclared, parsedMasks, groups));
        }

        byte[] trailing = Arrays.copyOfRange(data, buffer.position(), data.length);
        return new SpriteData(data.length, sections, trailing);
    }

    private static final class SpriteData {
        final int length;
        final List<Section> sections;
        final byte[] trailingBytes;

        SpriteData(int length, List<Section> sections, byte[] trailingBytes) {
            this.length = length;
            this.sections = sections;
            this.trailingBytes = trailingBytes;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_SPRITE_PRI\",\n");
            sb.append("  \"description\": \"Sprite occlusion masks (PRI) grouped per camera\",\n");
            sb.append("  \"length\": ").append(length).append(",\n");
            sb.append("  \"sections\": [\n");
            for (int i = 0; i < sections.size(); i++) {
                sb.append(sections.get(i).toJson());
                if (i < sections.size() - 1) {
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

    private static final class Section {
        final int index;
        final int offset;
        final int groupCount;
        final int maskCountDeclared;
        final List<Mask> masks;
        final List<Group> groups;

        Section(int index, int offset, int groupCount, int maskCountDeclared, List<Mask> masks, List<Group> groups) {
            this.index = index;
            this.offset = offset;
            this.groupCount = groupCount;
            this.maskCountDeclared = maskCountDeclared;
            this.masks = masks;
            this.groups = groups;
        }

        String toJson() {
            int assignedMasks = groups.stream().mapToInt(g -> g.masks.size()).sum();
            StringBuilder sb = new StringBuilder();
            sb.append("    {\n");
            sb.append("      \"index\": ").append(index).append(",\n");
            sb.append("      \"offset\": ").append(offset).append(",\n");
            sb.append("      \"groups\": ").append(groupCount).append(",\n");
            sb.append("      \"maskCountDeclared\": ").append(maskCountDeclared).append(",\n");
            sb.append("      \"maskCountParsed\": ").append(masks.size()).append(",\n");
            sb.append("      \"maskCountAssigned\": ").append(assignedMasks).append(",\n");
            sb.append("      \"groupsDetail\": [\n");
            for (int i = 0; i < groups.size(); i++) {
                sb.append(groups.get(i).toJson());
                if (i < groups.size() - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }
            sb.append("      ]\n");
            sb.append("    }");
            return sb.toString();
        }
    }

    private static final class Group {
        final int index;
        final int expectedCount;
        final int baseRaw;
        final int destXRaw;
        final int destYRaw;
        List<Mask> masks = new ArrayList<>();

        Group(int index, int expectedCount, int baseRaw, int destXRaw, int destYRaw) {
            this.index = index;
            this.expectedCount = expectedCount;
            this.baseRaw = baseRaw;
            this.destXRaw = destXRaw;
            this.destYRaw = destYRaw;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("        {\n");
            sb.append("          \"index\": ").append(index).append(",\n");
            sb.append("          \"count\": ").append(expectedCount).append(",\n");
            sb.append("          \"baseRaw\": ").append(baseRaw).append(",\n");
            sb.append("          \"baseHex\": \"0x").append(String.format(Locale.ROOT, "%04X", baseRaw)).append("\",\n");
            sb.append("          \"destination\": {\n");
            sb.append("            \"x\": ").append((short) destXRaw).append(",\n");
            sb.append("            \"y\": ").append((short) destYRaw).append("\n");
            sb.append("          },\n");
            sb.append("          \"masks\": [\n");
            for (int i = 0; i < masks.size(); i++) {
                sb.append(masks.get(i).toJson());
                if (i < masks.size() - 1) {
                    sb.append(",");
                }
                sb.append("\n");
            }
            sb.append("          ]\n");
            sb.append("        }");
            return sb.toString();
        }
    }

    private static final class Mask {
        final int index;
        final int offset;
        final int srcX;
        final int srcY;
        final int dstX;
        final int dstY;
        final int depth;
        final int unknown;
        final int sizeByte;
        final int width;
        final int height;
        final boolean rectangular;

        Mask(int index, int offset, int srcX, int srcY, int dstX, int dstY,
             int depth, int unknown, int sizeByte, int width, int height, boolean rectangular) {
            this.index = index;
            this.offset = offset;
            this.srcX = srcX;
            this.srcY = srcY;
            this.dstX = dstX;
            this.dstY = dstY;
            this.depth = depth;
            this.unknown = unknown;
            this.sizeByte = sizeByte;
            this.width = width;
            this.height = height;
            this.rectangular = rectangular;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("            {\n");
            sb.append("              \"index\": ").append(index).append(",\n");
            sb.append("              \"offset\": ").append(offset).append(",\n");
            sb.append("              \"type\": \"").append(rectangular ? "RECT" : "SQUARE").append("\",\n");
            sb.append("              \"source\": { \"x\": ").append(srcX).append(", \"y\": ").append(srcY).append(" },\n");
            sb.append("              \"destination\": { \"x\": ").append(dstX).append(", \"y\": ").append(dstY).append(" },\n");
            sb.append("              \"depth\": ").append(depth).append(",\n");
            sb.append("              \"depthHex\": \"0x").append(String.format(Locale.ROOT, "%04X", depth)).append("\",\n");
            sb.append("              \"unknown\": ").append(unknown).append(",\n");
            sb.append("              \"sizeByte\": ").append(sizeByte).append(",\n");
            sb.append("              \"width\": ").append(width).append(",\n");
            sb.append("              \"height\": ").append(height).append("\n");
            sb.append("            }");
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
