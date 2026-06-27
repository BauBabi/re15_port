package de.re15.extractors.pld;

import de.re15.extractors.MD1Extractor;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Converts extracted PLD/PLW MD1 assets into Blender-ready OBJ packages.
 * Keeps original files untouched and writes OBJ/BMP/companion data
 * into a dedicated {@code blender_export} subdirectory.
 */
public final class PldBlenderExporter {
    private final MD1Extractor md1Extractor = new MD1Extractor();

    public int export(Path pldRoot, Path outputRoot, StringBuilder log) {
        if (!Files.exists(pldRoot)) {
            return 0;
        }
        AtomicInteger counter = new AtomicInteger();
        try {
            Files.walk(pldRoot)
                    .filter(Files::isRegularFile)
                    .filter(path -> path.getFileName().toString().toUpperCase(Locale.ROOT).endsWith(".MD1"))
                    .sorted()
                    .forEach(md1 -> {
                        Path parent = md1.getParent();
                        if (parent == null) {
                            parent = pldRoot;
                        }
                        // Export directly into the model folder (no blender_export subfolder)
                        Path objPath = parent.resolve(replaceExtension(md1.getFileName().toString(), ".obj"));
                        try {
                            md1Extractor.convert(md1, objPath);
                            counter.incrementAndGet();
                            if (log != null) {
                                Path rel = safeRelativize(outputRoot, md1);
                                Path relExport = safeRelativize(outputRoot, objPath);
                                log.append("PLD Blender Export: ")
                                   .append(rel == null ? md1 : rel)
                                   .append(" -> ")
                                   .append(relExport == null ? objPath : relExport)
                                   .append(System.lineSeparator());
                            }
                        } catch (IOException e) {
                            System.err.println("Fehler beim MD1->OBJ Export " + md1 + ": " + e.getMessage());
                            if (log != null) {
                                Path rel = safeRelativize(outputRoot, md1);
                                log.append("FEHLER PLD Blender Export: ")
                                   .append(rel == null ? md1 : rel)
                                   .append(" - ")
                                   .append(e.getMessage())
                                   .append(System.lineSeparator());
                            }
                        }
                    });
        } catch (IOException e) {
            System.err.println("Fehler beim Durchsuchen der PLD-Ausgabe: " + e.getMessage());
            if (log != null) {
                log.append("FEHLER: PLD Blender Export Suche - ").append(e.getMessage()).append(System.lineSeparator());
            }
        }
        return counter.get();
    }

    private static String replaceExtension(String name, String newExt) {
        int dot = name.lastIndexOf('.');
        String stem = dot >= 0 ? name.substring(0, dot) : name;
        return stem + newExt;
    }

    private static Path safeRelativize(Path base, Path child) {
        if (base == null || child == null) {
            return null;
        }
        try {
            return base.relativize(child);
        } catch (IllegalArgumentException e) {
            return null;
        }
    }
}
