package de.re15.extractors.audio;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Locale;
import java.util.stream.Stream;

/**
 * Helper that turns a VH/VB pair into a folder of WAV files.
 */
public final class VabToWavConverter {
    private final PsxAdpcmDecoder decoder = new PsxAdpcmDecoder();
    private final int sampleRate;

    public VabToWavConverter() {
        this(22_050);
    }

    public VabToWavConverter(int sampleRate) {
        this.sampleRate = sampleRate;
    }

    public void convert(Path vhPath, Path vbPath, Path outputDir) throws IOException {
        convert(vhPath, vbPath, outputDir, SampleLayout.STANDARD);
    }

    public void convert(Path vhPath, Path vbPath, Path outputDir, SampleLayout layout) throws IOException {
        if (vhPath == null || vbPath == null || outputDir == null) {
            return;
        }
        if (!Files.isRegularFile(vhPath) || !Files.isRegularFile(vbPath)) {
            return;
        }
        byte[] vhData = Files.readAllBytes(vhPath);
        byte[] vbData = Files.readAllBytes(vbPath);
        VabFile vab = VabFile.parse(vhData);
        List<VabFile.Sample> samples = vab.buildSamples(vbData.length);
        List<SampleWindowExtractor.SampleSlice> slices = SampleWindowExtractor.slice(vbData, samples, layout);
        if (slices.isEmpty()) {
            return;
        }
        prepareDirectory(outputDir);
        String baseName = stripExtension(vhPath.getFileName().toString());
        int index = 0;
        for (SampleWindowExtractor.SampleSlice slice : slices) {
            SampleDataWindow window = slice.window();
            byte[] chunk = SampleChunkBuilder.build(vbData, window, layout, slice.sample());
            if (chunk.length == 0) {
                continue;
            }
            short[] pcm = decoder.decode(chunk, 0, chunk.length);
            if (pcm.length == 0) {
                continue;
            }
            applyFadeOut(pcm);
            String fileName = String.format(Locale.ROOT, "%s_%05d.wav", baseName, index);
            Path wavPath = outputDir.resolve(fileName);
            WavWriter.writeMono16(wavPath, pcm, sampleRate);
            index++;
        }
    }

    public int getSampleRate() {
        return sampleRate;
    }

    private static void applyFadeOut(short[] pcm) {
        if (pcm == null || pcm.length < 1000) {
            return;
        }
        // Fade over ~59% of the sample (starts at ~41%) with squared curve
        int fadeLength = (int) (pcm.length * 0.59);
        int fadeStart = pcm.length - fadeLength;
        for (int i = 0; i < fadeLength; i++) {
            double progress = (double) i / fadeLength;
            // Squared decay - gentler than cubic
            double remaining = 1.0 - progress;
            double factor = remaining * remaining;
            pcm[fadeStart + i] = (short) (pcm[fadeStart + i] * factor);
        }
    }

    private static String stripExtension(String name) {
        int dot = name.lastIndexOf('.');
        return dot >= 0 ? name.substring(0, dot) : name;
    }

    private static void prepareDirectory(Path dir) throws IOException {
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
}
