package de.re15.extractors.pld;

import de.re15.extractors.util.LittleEndianDataView;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Extracts Biohazard 1.5 PLD/PLW containers into their component files.
 * Each container holds animation data (EDD), skeleton data (EMR),
 * mesh data (MD1) and texture data (TIM).
 */
public final class PldExtractor {

    public int extract(Path sourceRoot, Path outputRoot, StringBuilder log) {
        AtomicInteger processed = new AtomicInteger();
        try {
            Files.walk(sourceRoot)
                .filter(Files::isRegularFile)
                .filter(PldExtractor::isPldOrPlw)
                .sorted()
                .forEach(path -> {
                    try {
                        processSingle(path, sourceRoot, outputRoot, log);
                        processed.incrementAndGet();
                    } catch (Exception e) {
                        System.err.println("Fehler beim Zerlegen von " + path + ": " + e.getMessage());
                        if (log != null) {
                            Path rel = safeRelativize(sourceRoot, path);
                            log.append("FEHLER PLD/PLW: ")
                               .append(rel == null ? path : rel)
                               .append(" - ")
                               .append(e.getMessage())
                               .append(System.lineSeparator());
                        }
                    }
                });
        } catch (IOException e) {
            System.err.println("Fehler beim Durchsuchen nach PLD/PLW-Dateien: " + e.getMessage());
            if (log != null) {
                log.append("FEHLER: PLD/PLW-Suche - ").append(e.getMessage()).append(System.lineSeparator());
            }
        }
        return processed.get();
    }

    private static boolean isPldOrPlw(Path path) {
        String name = path.getFileName().toString().toUpperCase(Locale.ROOT);
        return name.endsWith(".PLD") || name.endsWith(".PLW");
    }

    private void processSingle(Path file, Path sourceRoot, Path outputRoot, StringBuilder log) throws IOException {
        byte[] data = Files.readAllBytes(file);
        Segments segments = Segments.parse(data);

        String fileName = file.getFileName().toString();
        String stem = stripExtension(fileName);
        boolean isWeapon = fileName.toUpperCase(Locale.ROOT).endsWith(".PLW");
        String groupName = determineGroupName(stem, isWeapon);

        Path relative = safeRelativize(sourceRoot, file);
        Path parent = relative != null ? relative.getParent() : null;
        Path targetDir = outputRoot;
        if (parent != null) {
            targetDir = targetDir.resolve(parent);
        }
        targetDir = targetDir.resolve(groupName);
        if (isWeapon) {
            targetDir = targetDir.resolve(stem);
        }
        Files.createDirectories(targetDir);

        writeBytes(targetDir.resolve(stem + ".edd"), segments.edd());
        writeBytes(targetDir.resolve(stem + ".emr"), segments.emr());
        writeBytes(targetDir.resolve(stem + ".md1"), segments.md1());
        writeBytes(targetDir.resolve(stem + ".tim"), segments.tim());
        writeIni(targetDir.resolve(stem + ".ini"), isWeapon ? "plw" : "pld", stem);

        if (log != null) {
            Path outRel = safeRelativize(outputRoot, targetDir);
            log.append(isWeapon ? "PLW zerlegt: " : "PLD zerlegt: ")
               .append(relative == null ? file : relative)
               .append(" -> ")
               .append(outRel == null ? targetDir : outRel)
               .append(System.lineSeparator());
        }
    }

    private static void writeBytes(Path path, byte[] data) throws IOException {
        Files.write(path, data);
    }

    private static void writeIni(Path path, String section, String stem) throws IOException {
        String newline = System.lineSeparator();
        String content = "[" + section + "]" + newline
                + stem + ".edd" + newline
                + stem + ".emr" + newline
                + stem + ".md1" + newline
                + stem + ".tim" + newline;
        Files.writeString(path, content, StandardCharsets.UTF_8);
    }

    private static String stripExtension(String name) {
        int dot = name.lastIndexOf('.');
        return dot >= 0 ? name.substring(0, dot) : name;
    }

    private static String determineGroupName(String stem, boolean isWeapon) {
        if (!isWeapon) {
            return stem;
        }
        int idx = stem.indexOf('W');
        if (idx > 0 && idx < stem.length() - 1) {
            String suffix = stem.substring(idx + 1);
            boolean hexSuffix = !suffix.isEmpty() && suffix.chars().allMatch(ch -> Character.digit(ch, 16) != -1);
            if (hexSuffix) {
                return stem.substring(0, idx);
            }
        }
        return stem;
    }

    private static Path safeRelativize(Path root, Path child) {
        try {
            return root.relativize(child);
        } catch (IllegalArgumentException e) {
            return null;
        }
    }

    private record Segments(byte[] edd, byte[] emr, byte[] md1, byte[] tim) {
        static Segments parse(byte[] data) {
            if (data.length < 32) {
                throw new IllegalArgumentException("Datei zu klein für PLD/PLW");
            }
            LittleEndianDataView view = new LittleEndianDataView(data);
            int tableStart = view.readInt(0);
            int count = view.readInt(4);
            if (tableStart <= 0 || tableStart > data.length) {
                throw new IllegalArgumentException("Ungültiger Offset-Tabelle-Start: " + tableStart);
            }
            if (count < 4) {
                throw new IllegalArgumentException("Unerwartete Anzahl Offsets: " + count);
            }
            int tableBytes = count * 4;
            if (tableStart + tableBytes > data.length) {
                throw new IllegalArgumentException("Offset-Tabelle außerhalb der Datei");
            }

            int[] offsets = new int[count];
            for (int i = 0; i < count; i++) {
                int offset = view.readInt(tableStart + i * 4);
                offsets[i] = offset;
            }

            for (int i = 0; i < 4; i++) {
                int start = offsets[i];
                if (start < 0 || start >= tableStart) {
                    throw new IllegalArgumentException("Offset außerhalb der Datenregion: " + start);
                }
                if (i > 0 && start < offsets[i - 1]) {
                    throw new IllegalArgumentException("Offsets nicht sortiert");
                }
            }

            byte[] edd = slice(data, offsets[0], offsets[1]);
            byte[] emr = slice(data, offsets[1], offsets[2]);
            byte[] md1 = slice(data, offsets[2], offsets[3]);
            byte[] tim = slice(data, offsets[3], tableStart);
            return new Segments(edd, emr, md1, tim);
        }

        private static byte[] slice(byte[] data, int start, int end) {
            if (start < 0 || end < start || end > data.length) {
                throw new IllegalArgumentException("Ungültiger Bereich: " + start + " - " + end);
            }
            return Arrays.copyOfRange(data, start, end);
        }
    }
}
