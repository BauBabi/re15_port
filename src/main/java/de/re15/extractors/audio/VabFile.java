package de.re15.extractors.audio;

import de.re15.extractors.util.LittleEndianDataView;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;

/**
 * Minimal parser for PS1 VAB banks (.vh header files).
 */
public final class VabFile {
    private static final int SIGNATURE = 0x56414270; // "VABp" little endian
    private static final byte[] SIGNATURE_BYTES = {'p', 'B', 'A', 'V'};
    private static final int HEADER_SIZE = 32;
    private static final int PROGRAM_COUNT = 128;
    private static final int PROGRAM_ENTRY_SIZE = 16;
    private static final int PROGRAM_TABLE_SIZE = PROGRAM_COUNT * PROGRAM_ENTRY_SIZE;
    private static final int TONES_PER_PROGRAM = 16;
    private static final int TONE_ENTRY_SIZE = 32;
    private static final int VAG_TABLE_ENTRIES = 256;
    private static final int SIZE_TABLE_SIZE = VAG_TABLE_ENTRIES * 2;

    private final Header header;
    private final Program[] programs;
    private final Map<Integer, List<Tone>> tonesByVag;
    private final int[] vagSizes;
    private final int programSegmentCount;

    private VabFile(Header header,
                    Program[] programs,
                    Map<Integer, List<Tone>> tonesByVag,
                    int[] vagSizes,
                    int programSegmentCount) {
        this.header = header;
        this.programs = programs;
        this.tonesByVag = tonesByVag;
        this.vagSizes = vagSizes;
        this.programSegmentCount = programSegmentCount;
    }

    public static int findHeaderOffset(byte[] data) {
        return locateHeader(data);
    }

    public static VabFile parse(byte[] data) {
        if (data == null) {
            throw new IllegalArgumentException("Keine VH Daten.");
        }
        byte[] slice = extractVabPayload(data);
        LittleEndianDataView view = new LittleEndianDataView(slice);
        Header header = Header.read(view, slice);
        Program[] programs = readPrograms(slice, view);
        int toneSegmentBytes = TONES_PER_PROGRAM * TONE_ENTRY_SIZE;
        int toneBase = HEADER_SIZE + PROGRAM_TABLE_SIZE;
        int availableToneBytes = Math.max(0, slice.length - toneBase);
        int toneSegmentsInFile = Math.min(header.programCount(), availableToneBytes / toneSegmentBytes);
        Map<Integer, List<Tone>> toneMap = readTones(slice, view, toneSegmentsInFile, programs);
        int sizeTableOffset = toneBase + toneSegmentsInFile * toneSegmentBytes;
        int[] vagSizes = readVagSizes(view, sizeTableOffset, slice.length);
        toneMap.replaceAll((key, list) -> list == null ? List.of() : List.copyOf(list));
        return new VabFile(header, programs, toneMap, vagSizes, toneSegmentsInFile);
    }

    Header header() {
        return header;
    }

    public int headerSize() {
        return HEADER_SIZE
            + PROGRAM_TABLE_SIZE
            + programSegmentCount * TONES_PER_PROGRAM * TONE_ENTRY_SIZE
            + SIZE_TABLE_SIZE;
    }

    public int sampleDataLength() {
        int total = 0;
        int used = 0;
        int expected = header.vagCount();
        for (int length : vagSizes) {
            if (length <= 0) {
                continue;
            }
            total += length;
            used++;
            if (expected > 0 && used >= expected) {
                break;
            }
        }
        return total;
    }

    public List<Sample> buildSamples(int vbLength) {
        List<Sample> samples = new ArrayList<>();
        int offset = 0;
        int produced = 0;
        for (int vagIndex = 0; vagIndex < vagSizes.length; vagIndex++) {
            int length = vagSizes[vagIndex];
            if (length <= 0) {
                continue;
            }
            if (offset >= vbLength) {
                break;
            }
            int safeLength = Math.min(length, vbLength - offset);
            List<Tone> refs = tonesByVag.get(vagIndex);
            if (refs == null) {
                refs = Collections.emptyList();
            }
            samples.add(new Sample(vagIndex, offset, safeLength, refs));
            offset += length;
            produced++;
            if (header.vagCount() > 0 && produced >= header.vagCount()) {
                break;
            }
        }
        return samples;
    }

    private static Program[] readPrograms(byte[] data, LittleEndianDataView view) {
        Program[] result = new Program[PROGRAM_COUNT];
        int base = HEADER_SIZE;
        for (int index = 0; index < PROGRAM_COUNT; index++) {
            int offset = base + index * PROGRAM_ENTRY_SIZE;
            if (offset + PROGRAM_ENTRY_SIZE > data.length) {
                result[index] = Program.empty(index);
                continue;
            }
            int tones = Byte.toUnsignedInt(data[offset]);
            int volume = Byte.toUnsignedInt(data[offset + 1]);
            int priority = Byte.toUnsignedInt(data[offset + 2]);
            int mode = Byte.toUnsignedInt(data[offset + 3]);
            int pan = Byte.toUnsignedInt(data[offset + 4]);
            int attr = view.readUnsignedShort(offset + 6);
            result[index] = new Program(index, tones, volume, priority, mode, pan, attr);
        }
        return result;
    }

    private static Map<Integer, List<Tone>> readTones(byte[] data,
                                                      LittleEndianDataView view,
                                                      int programSegments,
                                                      Program[] programs) {
        Map<Integer, List<Tone>> toneMap = new LinkedHashMap<>();
        int toneBase = HEADER_SIZE + PROGRAM_TABLE_SIZE;
        int segmentSize = TONES_PER_PROGRAM * TONE_ENTRY_SIZE;
        for (int programIndex = 0; programIndex < programSegments; programIndex++) {
            Program program = programs[programIndex];
            int toneLimit = Math.min(program.toneCount(), TONES_PER_PROGRAM);
            int baseOffset = toneBase + programIndex * segmentSize;
            for (int toneIndex = 0; toneIndex < toneLimit; toneIndex++) {
                int entryOffset = baseOffset + toneIndex * TONE_ENTRY_SIZE;
                if (entryOffset + TONE_ENTRY_SIZE > data.length) {
                    break;
                }
                int vagIndex = view.readUnsignedShort(entryOffset + 22);
                if (vagIndex <= 0 || vagIndex >= VAG_TABLE_ENTRIES) {
                    continue;
                }
                int center = Byte.toUnsignedInt(data[entryOffset + 4]);
                int shift = Byte.toUnsignedInt(data[entryOffset + 5]);
                int adsr1 = view.readUnsignedShort(entryOffset + 16);
                int adsr2 = view.readUnsignedShort(entryOffset + 18);
                Tone tone = new Tone(programIndex, toneIndex, vagIndex, center, shift, adsr1, adsr2);
                toneMap.computeIfAbsent(vagIndex, key -> new ArrayList<>()).add(tone);
            }
        }
        return toneMap;
    }

    private static int[] readVagSizes(LittleEndianDataView view, int offset, int length) {
        int[] sizes = new int[VAG_TABLE_ENTRIES];
        int available = Math.max(0, length - offset);
        int entries = Math.min(VAG_TABLE_ENTRIES, available / 2);
        for (int i = 0; i < entries; i++) {
            int words = view.readUnsignedShort(offset + i * 2);
            sizes[i] = words << 3;
        }
        return sizes;
    }

    static final class Header {
        private final int form;
        private final int version;
        private final int id;
        private final int fileSize;
        private final int reserved0;
        private final int programCount;
        private final int toneCount;
        private final int vagCount;
        private final int masterVolume;
        private final int masterPan;
        private final int attr1;
        private final int attr2;
        private final int reserved1;

        private Header(int form,
                       int version,
                       int id,
                       int fileSize,
                       int reserved0,
                       int programCount,
                       int toneCount,
                       int vagCount,
                       int masterVolume,
                       int masterPan,
                       int attr1,
                       int attr2,
                       int reserved1) {
            this.form = form;
            this.version = version;
            this.id = id;
            this.fileSize = fileSize;
            this.reserved0 = reserved0;
            this.programCount = programCount;
            this.toneCount = toneCount;
            this.vagCount = vagCount;
            this.masterVolume = masterVolume;
            this.masterPan = masterPan;
            this.attr1 = attr1;
            this.attr2 = attr2;
            this.reserved1 = reserved1;
        }

        static Header read(LittleEndianDataView view, byte[] data) {
            int form = view.readInt(0);
            if (form != SIGNATURE) {
                throw new IllegalArgumentException(String.format(Locale.ROOT,
                    "Unbekannte VH Signatur: 0x%08X", form));
            }
            int version = view.readInt(4);
            int id = view.readInt(8);
            int fileSize = view.readInt(12);
            int reserved0 = view.readUnsignedShort(16);
            int programCount = view.readUnsignedShort(18);
            int toneCount = view.readUnsignedShort(20);
            int vagCount = view.readUnsignedShort(22);
            int masterVolume = Byte.toUnsignedInt(data[24]);
            int masterPan = Byte.toUnsignedInt(data[25]);
            int attr1 = Byte.toUnsignedInt(data[26]);
            int attr2 = Byte.toUnsignedInt(data[27]);
            int reserved1 = view.readInt(28);
            return new Header(form, version, id, fileSize, reserved0, programCount, toneCount,
                vagCount, masterVolume, masterPan, attr1, attr2, reserved1);
        }

        int programCount() {
            return programCount;
        }

        int vagCount() {
            return vagCount;
        }
    }

    static final class Program {
        private final int index;
        private final int toneCount;
        private final int volume;
        private final int priority;
        private final int mode;
        private final int pan;
        private final int attribute;

        Program(int index,
                int toneCount,
                int volume,
                int priority,
                int mode,
                int pan,
                int attribute) {
            this.index = index;
            this.toneCount = toneCount;
            this.volume = volume;
            this.priority = priority;
            this.mode = mode;
            this.pan = pan;
            this.attribute = attribute;
        }

        static Program empty(int index) {
            return new Program(index, 0, 0, 0, 0, 0, 0);
        }

        int toneCount() {
            return toneCount;
        }
    }

    static final class Tone {
        private final int programIndex;
        private final int toneIndex;
        private final int vagIndex;
        private final int centerNote;
        private final int pitchShift;
        private final int adsr1;
        private final int adsr2;

        Tone(int programIndex,
             int toneIndex,
             int vagIndex,
             int centerNote,
             int pitchShift,
             int adsr1,
             int adsr2) {
            this.programIndex = programIndex;
            this.toneIndex = toneIndex;
            this.vagIndex = vagIndex;
            this.centerNote = centerNote;
            this.pitchShift = pitchShift;
            this.adsr1 = adsr1;
            this.adsr2 = adsr2;
        }

        int programIndex() {
            return programIndex;
        }

        int toneIndex() {
            return toneIndex;
        }

        int vagIndex() {
            return vagIndex;
        }

        int centerNote() {
            return centerNote;
        }

        int pitchShift() {
            return pitchShift;
        }

        int adsr1() {
            return adsr1;
        }

        int adsr2() {
            return adsr2;
        }
    }

    public static final class Sample {
        private final int vagIndex;
        private final int offset;
        private final int length;
        private final List<Tone> tones;

        Sample(int vagIndex, int offset, int length, List<Tone> tones) {
            this.vagIndex = vagIndex;
            this.offset = offset;
            this.length = length;
            this.tones = tones;
        }

        public int vagIndex() {
            return vagIndex;
        }

        public int offset() {
            return offset;
        }

        public int length() {
            return length;
        }

        public List<Tone> tones() {
            return tones;
        }
    }

    private static byte[] extractVabPayload(byte[] data) {
        if (data.length < HEADER_SIZE) {
            throw new IllegalArgumentException("VH Datei ist zu klein.");
        }
        int offset = locateHeader(data);
        if (offset == 0) {
            return data;
        }
        return Arrays.copyOfRange(data, offset, data.length);
    }

    private static int locateHeader(byte[] data) {
        for (int i = 0; i <= data.length - SIGNATURE_BYTES.length; i++) {
            boolean match = true;
            for (int j = 0; j < SIGNATURE_BYTES.length; j++) {
                if (data[i + j] != SIGNATURE_BYTES[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                return i;
            }
        }
        throw new IllegalArgumentException("Keine VAB Signatur gefunden.");
    }
}
