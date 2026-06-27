package de.re15.extractors;

import de.re15.extractors.audio.SampleChunkBuilder;
import de.re15.extractors.audio.SampleLayout;
import de.re15.extractors.audio.SampleWindowExtractor;
import de.re15.extractors.audio.SeqToMidiConverter;
import de.re15.extractors.audio.VabFile;
import de.re15.extractors.audio.VabToWavConverter;
import de.re15.extractors.audio.VagWriter;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.io.UncheckedIOException;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.Locale;
import java.util.stream.Stream;

/**
 * Extracts the contents of info/Re1.5/PSX/SOUND.
 *
 * <p>The directory contains simple ARMS/CORE banks (*.EDH + *.VB) and the
 * sequenced BGM archives (*.BGM). This extractor copies the raw sources,
 * decodes the contained VAB banks to WAVs and saves the SEQ data so the user
 * can verify the material with modern tools.</p>
 */
public final class SoundExtractor {
    private static final String DEFAULT_SOURCE = "info/Re1.5";
    private static final String DEFAULT_TARGET = "extracted";

    private final VabToWavConverter converter = new VabToWavConverter();
    private final SeqToMidiConverter midiConverter = new SeqToMidiConverter();

    // Instance fields for current extraction paths
    private Path inputRoot;
    private Path outputRoot;

    public static void main(String[] args) throws IOException {
        new SoundExtractor().extract(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    /**
     * Extrahiert Sound-Dateien mit Standard-Pfaden (Fallback).
     */
    public void extract() throws IOException {
        extract(DEFAULT_SOURCE, DEFAULT_TARGET);
    }

    /**
     * Extrahiert Sound-Dateien mit benutzerdefinierten Pfaden.
     *
     * @param sourceRoot Quellverzeichnis (z.B. "info/Re1.5")
     * @param outputRoot Zielverzeichnis (z.B. "extracted")
     */
    public void extract(String sourceRoot, String outputRoot) throws IOException {
        this.inputRoot = Paths.get(sourceRoot, "PSX", "SOUND");
        this.outputRoot = Paths.get(outputRoot, "PSX", "SOUND");

        if (!Files.isDirectory(inputRoot)) {
            System.out.println("Kein SOUND Ordner gefunden: " + inputRoot);
            return;
        }
        Files.createDirectories(this.outputRoot);
        extractEdhBanks();
        extractBgms();
    }

    private void extractEdhBanks() throws IOException {
        try (Stream<Path> files = Files.list(inputRoot)) {
            files.filter(SoundExtractor::isEdhFile)
                .sorted(Comparator.comparing(path -> path.getFileName().toString().toUpperCase(Locale.ROOT)))
                .forEach(path -> {
                    try {
                        extractEdhBank(path);
                    } catch (IOException e) {
                        throw new RuntimeException("Fehler beim Extrahieren von " + path, e);
                    }
                });
        }
    }

    private void extractEdhBank(Path edhPath) throws IOException {
        String baseName = stripExtension(edhPath.getFileName().toString());
        Path vbPath = inputRoot.resolve(baseName + ".VB");
        if (!Files.isRegularFile(vbPath)) {
            System.out.println("Keine VB Datei f├╝r " + baseName + " gefunden, ├╝berspringe.");
            return;
        }
        Path targetDir = outputRoot.resolve(baseName);
        Files.createDirectories(targetDir);
        Path headerOut = targetDir.resolve(baseName + ".edh");
        Path vbOut = targetDir.resolve(baseName + ".vb");
        Files.copy(edhPath, headerOut, StandardCopyOption.REPLACE_EXISTING);
        Files.copy(vbPath, vbOut, StandardCopyOption.REPLACE_EXISTING);
        converter.convert(headerOut, vbOut, targetDir.resolve("wav"), SampleLayout.STANDARD);
    }

    private void extractBgms() throws IOException {
        Path bgmRoot = outputRoot.resolve("BGM");
        Files.createDirectories(bgmRoot);
        try (Stream<Path> files = Files.list(inputRoot)) {
            files.filter(SoundExtractor::isBgmFile)
                .sorted(Comparator.comparing(path -> path.getFileName().toString().toUpperCase(Locale.ROOT)))
                .forEach(path -> {
                    try {
                        extractSingleBgm(path, bgmRoot);
                    } catch (IOException e) {
                        System.err.println("Warnung: Fehler beim Extrahieren von " + path + ": " + e.getMessage());
                    } catch (IllegalArgumentException e) {
                        System.err.println("Warnung: BGM-Datei uebersprungen (kein VAB-Header): " + path.getFileName());
                    }
                });
        }
    }

    private void extractSingleBgm(Path bgmPath, Path bgmRoot) throws IOException {
        String baseName = stripExtension(bgmPath.getFileName().toString());
        byte[] data = Files.readAllBytes(bgmPath);

        // Versuche VAB-Header zu finden, ueberspringe Datei wenn nicht vorhanden
        int vhOffset;
        try {
            vhOffset = VabFile.findHeaderOffset(data);
        } catch (IllegalArgumentException e) {
            // Kein VAB-Header gefunden - kopiere nur die Originaldatei
            Path targetDir = bgmRoot.resolve(baseName);
            Files.createDirectories(targetDir);
            Path bgmCopy = targetDir.resolve(baseName + ".bgm");
            Files.copy(bgmPath, bgmCopy, StandardCopyOption.REPLACE_EXISTING);
            System.out.println("    BGM kopiert (kein VAB): " + baseName);
            return;
        }
        byte[] seqData = Arrays.copyOfRange(data, 0, vhOffset);
        VabFile vab = VabFile.parse(data);
        int vhSize = vab.headerSize();
        int vbLength = vab.sampleDataLength();
        int vbOffset = vhOffset + vhSize;
        if (vbOffset + vbLength > data.length) {
            vbLength = Math.max(0, data.length - vbOffset);
        }

        Path targetDir = bgmRoot.resolve(baseName);
        Files.createDirectories(targetDir);
        Path bgmCopy = targetDir.resolve(baseName + ".bgm");
        Files.copy(bgmPath, bgmCopy, StandardCopyOption.REPLACE_EXISTING);
        Path seqPath = targetDir.resolve(baseName + ".seq");
        Files.write(seqPath, seqData);
        writeMidiCopy(targetDir.resolve(baseName + ".mid"), seqData, baseName);

        byte[] vhData = Arrays.copyOfRange(data, vhOffset, vhOffset + vhSize);
        byte[] rawVbData = Arrays.copyOfRange(data, vbOffset, vbOffset + vbLength);
        int vbSkip = findVbDataStart(rawVbData);
        byte[] vbData = vbSkip > 0 ? Arrays.copyOfRange(rawVbData, vbSkip, rawVbData.length) : rawVbData;

        Path vhPath = targetDir.resolve(baseName + ".vh");
        Path vbPath = targetDir.resolve(baseName + ".vb");
        Files.write(vhPath, vhData);
        Files.write(vbPath, vbData);

        List<VabFile.Sample> samples = vab.buildSamples(vbData.length);
        writeSampleVags(targetDir.resolve("vag"), vbData, samples, baseName, SampleLayout.STANDARD, converter.getSampleRate());
        converter.convert(vhPath, vbPath, targetDir.resolve("wav"), SampleLayout.STANDARD);
    }

    private static int findVbDataStart(byte[] data) {
        if (data == null || data.length < 32) {
            return 0;
        }
        int zeroCount = 0;
        for (int i = 0; i < Math.min(32, data.length); i++) {
            if (data[i] == 0) {
                zeroCount++;
            } else {
                break;
            }
        }
        if (zeroCount >= 20 && zeroCount % 4 == 0) {
            return zeroCount - 16;
        }
        return 0;
    }

    private void writeMidiCopy(Path midiPath, byte[] seqData, String baseName) {
        try {
            byte[] midi = midiConverter.convert(seqData);
            Files.write(midiPath, midi);
        } catch (IllegalArgumentException | IOException ex) {
            System.out.println("Konnte MIDI f\u00fcr " + baseName + " nicht erzeugen: " + ex.getMessage());
        }
    }

    private void writeSampleVags(Path dir,
                                 byte[] vbData,
                                 List<VabFile.Sample> samples,
                                 String baseName,
                                 SampleLayout layout,
                                 int sampleRate) throws IOException {
        List<SampleWindowExtractor.SampleSlice> slices = SampleWindowExtractor.slice(vbData, samples, layout);
        if (slices.isEmpty()) {
            return;
        }
        cleanDirectory(dir);
        int index = 0;
        for (SampleWindowExtractor.SampleSlice slice : slices) {
            byte[] chunk = SampleChunkBuilder.build(vbData, slice.window(), layout, slice.sample());
            if (chunk.length == 0) {
                continue;
            }
            String fileStem = String.format(Locale.ROOT, "%s%02d", baseName, index);
            Path output = dir.resolve(fileStem + ".vag");
            VagWriter.write(output, chunk, fileStem, sampleRate);
            index++;
        }
    }

    private static void cleanDirectory(Path dir) throws IOException {
        if (Files.isDirectory(dir)) {
            try (Stream<Path> entries = Files.list(dir)) {
                try {
                    entries.forEach(path -> {
                        try {
                            Files.delete(path);
                        } catch (IOException e) {
                            throw new UncheckedIOException(e);
                        }
                    });
                } catch (UncheckedIOException e) {
                    throw e.getCause();
                }
            }
        }
        Files.createDirectories(dir);
    }

    private static boolean isEdhFile(Path path) {
        String name = path.getFileName().toString().toUpperCase(Locale.ROOT);
        return name.endsWith(".EDH");
    }

    private static boolean isBgmFile(Path path) {
        String name = path.getFileName().toString().toUpperCase(Locale.ROOT);
        return name.endsWith(".BGM");
    }

    private static String stripExtension(String value) {
        int idx = value.lastIndexOf('.');
        return idx >= 0 ? value.substring(0, idx) : value;
    }
}
