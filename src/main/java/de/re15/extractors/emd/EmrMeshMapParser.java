package de.re15.extractors.emd;

import de.re15.extractors.util.LittleEndianDataView;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Optional;

final class EmrMeshMapParser {
    private EmrMeshMapParser() {}

    static Optional<int[]> parseMeshMap(Path path, int expectedCount) {
        if (path == null || !Files.isRegularFile(path)) {
            return Optional.empty();
        }
        try {
            byte[] data = Files.readAllBytes(path);
            if (data.length < 8) {
                return Optional.empty();
            }
            LittleEndianDataView view = new LittleEndianDataView(data);
            int oArmatures = view.readUnsignedShort(0);
            int oFrames = view.readUnsignedShort(2);
            int count = view.readUnsignedShort(4);
            if (count <= 0) {
                return Optional.empty();
            }
            int table = oArmatures;
            if (table + count * 4 > data.length) {
                return Optional.empty();
            }
            int meshOffset = view.readUnsignedShort(table + 2);
            int base = table + meshOffset;
            int[] rawMap = new int[count];
            for (int i = 0; i < count; i++) {
                int value = Byte.toUnsignedInt(data[base + i]);
                rawMap[i] = value;
            }
            if (expectedCount > 0) {
                int[] map = new int[expectedCount];
                for (int i = 0; i < expectedCount; i++) {
                    map[i] = i < rawMap.length ? rawMap[i] : -1;
                }
                return Optional.of(map);
            }
            return Optional.of(rawMap);
        } catch (IOException e) {
            return Optional.empty();
        }
    }
}
