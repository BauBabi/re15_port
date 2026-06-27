package de.re15.extractors;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;

/**
 * Parser for effect.esp chunks (Effect Sprite data) - Resident Evil 1.5 format.
 *
 * <p>ESP files contain effect sprite definitions used by SCE_ESPR script instructions.
 * These define visual effects like fire, explosions, particle effects, etc.</p>
 *
 * <h3>RE1.5 Format Overview:</h3>
 * <p>The ESP format consists of:</p>
 * <ul>
 *   <li><b>Effect IDs</b>: 8 universal effect ID slots (bytes 0-7).
 *       Value 0xFF indicates an unused slot.</li>
 *   <li><b>EFF Pointers</b>: Pointers to the actual effect data (EFF).
 *       Value 0xFFFFFFFF indicates an unused pointer.
 *       To get the real pointer: subtract 0x20 for the pointer index,
 *       then add 0x08 for the actual data offset.</li>
 *   <li><b>Effect Data</b>: The actual effect animation/sprite data.</li>
 * </ul>
 */
public class EffectEspParser {

    /** Maximum effect IDs for RE1.5 format (8 bytes header) */
    private static final int MAX_EFFECT_IDS = 8;

    /** Marker for unused effect slot */
    private static final int UNUSED_ID = 0xFF;

    /** Marker for unused pointer */
    private static final long UNUSED_POINTER = 0xFFFFFFFFL;

    public String parseToJson(byte[] raw) {
        byte[] data = raw == null ? new byte[0] : raw;
        return parse(data).toJson();
    }

    private EffectData parse(byte[] data) {
        if (data.length < MAX_EFFECT_IDS) {
            return EffectData.empty(data);
        }

        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);

        // Detect if this looks like the ID-header format or legacy format
        boolean hasIdHeader = detectIdHeaderFormat(data);

        List<EffectSlot> effectSlots = new ArrayList<>();
        List<EffectPointer> pointers = new ArrayList<>();
        List<EffectEntry> entries = new ArrayList<>();

        if (hasIdHeader) {
            // Parse effect ID slots (8 bytes)
            buffer.position(0);
            for (int i = 0; i < MAX_EFFECT_IDS; i++) {
                int id = Byte.toUnsignedInt(buffer.get());
                boolean used = (id != UNUSED_ID);
                effectSlots.add(new EffectSlot(i, id, used));
            }

            // Parse pointers - they come after the ID header
            int pointerIndex = 0;
            while (buffer.remaining() >= 4) {
                int currentPos = buffer.position();
                long rawPointer = Integer.toUnsignedLong(buffer.getInt());

                // Check if this looks like a valid pointer or terminator
                if (rawPointer == UNUSED_POINTER) {
                    pointers.add(new EffectPointer(pointerIndex, currentPos, rawPointer, true,
                            -1, -1, "Unused pointer slot"));
                    pointerIndex++;
                    continue;
                }

                // Check if we've hit actual effect data (non-pointer values)
                if (rawPointer > data.length && rawPointer != UNUSED_POINTER) {
                    buffer.position(currentPos);
                    break;
                }

                // Calculate real offsets
                int pointerOffset = (int) (rawPointer & 0xFFFF);
                int flags = (int) ((rawPointer >>> 16) & 0xFFFF);
                int dataOffset = pointerOffset + 8; // Add 0x08 for real data offset

                String note = null;
                if (dataOffset > 0 && dataOffset < data.length) {
                    note = String.format("Points to data at offset 0x%04X", dataOffset);
                }

                pointers.add(new EffectPointer(pointerIndex, currentPos, rawPointer, false,
                        pointerOffset, flags, note));
                pointerIndex++;

                // Limit pointer parsing
                if (pointerIndex >= MAX_EFFECT_IDS * 2) break;
            }

            // Parse effect entries (remaining data)
            entries = parseEffectEntries(buffer);

        } else {
            // Fallback: legacy format parsing
            return parseOriginalFormat(data);
        }

        int consumed = buffer.position();
        byte[] trailing = (consumed < data.length)
                ? Arrays.copyOfRange(data, consumed, data.length)
                : new byte[0];

        return new EffectData(hasIdHeader, effectSlots, pointers, entries, trailing);
    }

    /**
     * Detect if the data uses the ID-header format (first 8 bytes are effect IDs).
     */
    private boolean detectIdHeaderFormat(byte[] data) {
        if (data.length < MAX_EFFECT_IDS) {
            return false;
        }

        // Check first 8 bytes for valid effect IDs pattern
        int validIdCount = 0;
        for (int i = 0; i < MAX_EFFECT_IDS; i++) {
            int val = Byte.toUnsignedInt(data[i]);
            if (val == UNUSED_ID || val < 32) {
                // Valid effect ID (typically 0-31) or UNUSED marker
                validIdCount++;
            }
        }

        // If most bytes look like valid IDs, use ID-header format
        return validIdCount >= 6;
    }

    /**
     * Parse effect data entries from the buffer.
     */
    private List<EffectEntry> parseEffectEntries(ByteBuffer buffer) {
        List<EffectEntry> entries = new ArrayList<>();

        int entryIndex = 0;
        while (buffer.remaining() >= 8) {
            int offset = buffer.position();

            // Read potential effect entry data (8 bytes = 4 shorts)
            short val0 = buffer.getShort();
            short val1 = buffer.getShort();
            short val2 = buffer.getShort();
            short val3 = buffer.getShort();

            // Classify entry type
            String type = "DATA";
            String note = null;

            if (val0 == 0 && val1 == 0 && val2 == 0 && val3 == 0) {
                type = "PADDING";
                note = "All zeros - likely padding";
            } else if (Math.abs(val0) < 512 && Math.abs(val1) < 512) {
                type = "SPRITE_COORD";
                note = String.format("Position (%d, %d), Size/Flags (%d, %d)", val0, val1, val2, val3);
            }

            entries.add(new EffectEntry(entryIndex, offset, val0, val1, val2, val3, type, note));
            entryIndex++;

            // Limit entries to avoid runaway parsing
            if (entryIndex > 256) break;
        }

        return entries;
    }

    /**
     * Fallback parser for legacy format.
     */
    private EffectData parseOriginalFormat(byte[] data) {
        ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);

        int word0 = Short.toUnsignedInt(buffer.getShort());
        int word1 = Short.toUnsignedInt(buffer.getShort());
        int word2 = Short.toUnsignedInt(buffer.getShort());
        int word3 = Short.toUnsignedInt(buffer.getShort());
        int countsWord = buffer.getInt();
        int payloadOffset = buffer.getInt();

        int pointerCountDeclared = countsWord & 0xFFFF;

        List<EffectPointer> pointers = new ArrayList<>();
        int pointerCount = Math.min(pointerCountDeclared, Math.max(0, buffer.remaining() / 8));
        for (int i = 0; i < pointerCount; i++) {
            int pos = buffer.position();
            int rawPointer = buffer.getInt();
            int reserved = buffer.getInt();
            int flags = (rawPointer >>> 16) & 0xFFFF;
            int offset = rawPointer & 0xFFFF;
            pointers.add(new EffectPointer(i, pos, Integer.toUnsignedLong(rawPointer), false,
                    offset, flags, "Reserved: " + reserved));
        }

        List<EffectEntry> entries = new ArrayList<>();
        int idx = 0;
        while (buffer.remaining() >= 4) {
            int pos = buffer.position();
            short x = buffer.getShort();
            short y = buffer.getShort();
            entries.add(new EffectEntry(idx, pos, x, y, (short) 0, (short) 0, "POINT",
                    String.format("Coordinate (%d, %d)", x, y)));
            idx++;
            if (idx > 256) break;
        }

        byte[] trailing = Arrays.copyOfRange(data, buffer.position(), data.length);
        return new EffectData(false, List.of(), pointers, entries, trailing);
    }

    // ========== Data Classes ==========

    private static final class EffectSlot {
        final int slotIndex;
        final int effectId;
        final boolean isUsed;

        EffectSlot(int slotIndex, int effectId, boolean isUsed) {
            this.slotIndex = slotIndex;
            this.effectId = effectId;
            this.isUsed = isUsed;
        }

        String toJson() {
            if (isUsed) {
                return String.format(Locale.ROOT,
                        "    { \"slot\": %d, \"effectId\": %d, \"effectIdHex\": \"0x%02X\", \"used\": true }",
                        slotIndex, effectId, effectId);
            } else {
                return String.format(Locale.ROOT,
                        "    { \"slot\": %d, \"effectId\": 255, \"effectIdHex\": \"0xFF\", \"used\": false, \"_note\": \"UNUSED\" }",
                        slotIndex);
            }
        }
    }

    private static final class EffectPointer {
        final int index;
        final int fileOffset;
        final long rawValue;
        final boolean isUnused;
        final int pointerOffset;
        final int flags;
        final String note;

        EffectPointer(int index, int fileOffset, long rawValue, boolean isUnused,
                      int pointerOffset, int flags, String note) {
            this.index = index;
            this.fileOffset = fileOffset;
            this.rawValue = rawValue;
            this.isUnused = isUnused;
            this.pointerOffset = pointerOffset;
            this.flags = flags;
            this.note = note;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append(String.format(Locale.ROOT,
                    "    { \"index\": %d, \"fileOffset\": %d, \"raw\": \"0x%08X\"",
                    index, fileOffset, rawValue));

            if (isUnused) {
                sb.append(", \"unused\": true");
            } else {
                sb.append(String.format(Locale.ROOT,
                        ", \"pointerOffset\": %d, \"flags\": \"0x%04X\", \"dataOffset\": %d",
                        pointerOffset, flags, pointerOffset + 8));
            }

            if (note != null && !note.isEmpty()) {
                sb.append(", \"_note\": \"").append(escapeJson(note)).append("\"");
            }

            sb.append(" }");
            return sb.toString();
        }
    }

    private static final class EffectEntry {
        final int index;
        final int fileOffset;
        final short val0;
        final short val1;
        final short val2;
        final short val3;
        final String type;
        final String note;

        EffectEntry(int index, int fileOffset, short val0, short val1, short val2, short val3,
                    String type, String note) {
            this.index = index;
            this.fileOffset = fileOffset;
            this.val0 = val0;
            this.val1 = val1;
            this.val2 = val2;
            this.val3 = val3;
            this.type = type;
            this.note = note;
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append(String.format(Locale.ROOT,
                    "    { \"index\": %d, \"offset\": %d, \"type\": \"%s\", \"values\": [%d, %d, %d, %d]",
                    index, fileOffset, type, val0, val1, val2, val3));

            sb.append(String.format(Locale.ROOT,
                    ", \"hex\": [\"0x%04X\", \"0x%04X\", \"0x%04X\", \"0x%04X\"]",
                    val0 & 0xFFFF, val1 & 0xFFFF, val2 & 0xFFFF, val3 & 0xFFFF));

            if (note != null && !note.isEmpty()) {
                sb.append(", \"_note\": \"").append(escapeJson(note)).append("\"");
            }

            sb.append(" }");
            return sb.toString();
        }
    }

    private static final class EffectData {
        final boolean hasIdHeader;
        final List<EffectSlot> effectSlots;
        final List<EffectPointer> pointers;
        final List<EffectEntry> entries;
        final byte[] trailingBytes;

        EffectData(boolean hasIdHeader, List<EffectSlot> effectSlots, List<EffectPointer> pointers,
                   List<EffectEntry> entries, byte[] trailingBytes) {
            this.hasIdHeader = hasIdHeader;
            this.effectSlots = effectSlots;
            this.pointers = pointers;
            this.entries = entries;
            this.trailingBytes = trailingBytes;
        }

        static EffectData empty(byte[] trailing) {
            return new EffectData(false, List.of(), List.of(), List.of(), trailing);
        }

        String toJson() {
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            sb.append("  \"format\": \"RE15_EFFECT_ESP\",\n");
            sb.append("  \"description\": \"Effect sprite definitions for visual effects (fire, explosions, particles)\",\n");
            sb.append("  \"maxEffectSlots\": ").append(MAX_EFFECT_IDS).append(",\n");

            // Effect slots (if present)
            if (!effectSlots.isEmpty()) {
                int usedCount = (int) effectSlots.stream().filter(s -> s.isUsed).count();
                sb.append("  \"effectSlots\": {\n");
                sb.append("    \"total\": ").append(effectSlots.size()).append(",\n");
                sb.append("    \"used\": ").append(usedCount).append(",\n");
                sb.append("    \"slots\": [\n");
                for (int i = 0; i < effectSlots.size(); i++) {
                    sb.append(effectSlots.get(i).toJson());
                    if (i < effectSlots.size() - 1) sb.append(",");
                    sb.append("\n");
                }
                sb.append("    ]\n");
                sb.append("  },\n");
            }

            // Pointers
            sb.append("  \"pointers\": {\n");
            sb.append("    \"count\": ").append(pointers.size()).append(",\n");
            sb.append("    \"items\": [\n");
            for (int i = 0; i < pointers.size(); i++) {
                sb.append(pointers.get(i).toJson());
                if (i < pointers.size() - 1) sb.append(",");
                sb.append("\n");
            }
            sb.append("    ]\n");
            sb.append("  },\n");

            // Effect entries
            sb.append("  \"effectData\": {\n");
            sb.append("    \"count\": ").append(entries.size()).append(",\n");
            sb.append("    \"entries\": [\n");
            for (int i = 0; i < entries.size(); i++) {
                sb.append(entries.get(i).toJson());
                if (i < entries.size() - 1) sb.append(",");
                sb.append("\n");
            }
            sb.append("    ]\n");
            sb.append("  }");

            // Trailing bytes
            if (trailingBytes.length > 0) {
                sb.append(",\n");
                sb.append("  \"trailingBytes\": {\n");
                sb.append("    \"length\": ").append(trailingBytes.length).append(",\n");
                sb.append("    \"hex\": \"").append(toHex(trailingBytes)).append("\"\n");
                sb.append("  }");
            }

            sb.append("\n}\n");
            return sb.toString();
        }
    }

    // ========== Utility Methods ==========

    private static String toHex(byte[] bytes) {
        if (bytes.length == 0) return "";
        StringBuilder sb = new StringBuilder(bytes.length * 2);
        for (byte b : bytes) {
            sb.append(String.format(Locale.ROOT, "%02x", b));
        }
        return sb.toString();
    }

    private static String escapeJson(String s) {
        if (s == null) return "";
        return s.replace("\\", "\\\\")
                .replace("\"", "\\\"")
                .replace("\n", "\\n")
                .replace("\r", "\\r")
                .replace("\t", "\\t");
    }
}
