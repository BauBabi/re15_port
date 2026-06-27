package de.re15.extractors.audio;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

/**
 * MP3 → WAV transcoder. Java mirror of {@code scripts/patch_save_final.py:_ensure_wav_input}.
 *
 * <p>Decodes MP3 via JLayer (registered into {@code javax.sound.sampled} by the
 * {@code mp3spi} ServiceProvider) and resamples to the target format expected by
 * the VAG encoder: 22050 Hz, mono, 16-bit signed little-endian PCM.
 *
 * <p>Output is cached: if the .wav already exists and is newer than the source
 * .mp3, the existing file is returned unchanged.
 */
public final class Mp3ToWavConverter {

    private static final float TARGET_SAMPLE_RATE = 22050f;
    private static final int TARGET_CHANNELS = 1;
    private static final int TARGET_SAMPLE_SIZE_BITS = 16;

    private Mp3ToWavConverter() {}

    /**
     * If {@code source} is .wav, return it unchanged. If .mp3, transcode to a
     * sibling .wav (cached by mtime). Other extensions throw IllegalArgumentException.
     */
    public static Path ensureWav(Path source) throws IOException, UnsupportedAudioFileException {
        String name = source.getFileName().toString().toLowerCase();
        if (name.endsWith(".wav")) {
            return source;
        }
        if (!name.endsWith(".mp3")) {
            throw new IllegalArgumentException(
                "unsupported audio format: " + source.getFileName() + " (expected .wav or .mp3)");
        }
        String stem = name.substring(0, name.length() - 4);
        Path wav = source.resolveSibling(stem + ".wav");
        if (Files.exists(wav)
                && Files.getLastModifiedTime(wav).toMillis()
                   >= Files.getLastModifiedTime(source).toMillis()) {
            return wav;
        }
        convert(source, wav);
        return wav;
    }

    /** Transcode {@code mp3} → {@code wav} unconditionally (overwrites existing target). */
    public static void convert(Path mp3, Path wav) throws IOException, UnsupportedAudioFileException {
        File mp3File = mp3.toFile();
        try (AudioInputStream src = AudioSystem.getAudioInputStream(mp3File)) {
            AudioFormat srcFmt = src.getFormat();

            // Decode mp3 to PCM at source rate first (mp3spi can only produce PCM at source rate)
            AudioFormat pcmIntermediate = new AudioFormat(
                AudioFormat.Encoding.PCM_SIGNED,
                srcFmt.getSampleRate(),
                TARGET_SAMPLE_SIZE_BITS,
                srcFmt.getChannels(),
                srcFmt.getChannels() * 2,
                srcFmt.getSampleRate(),
                false  // little-endian
            );
            try (AudioInputStream pcm = AudioSystem.getAudioInputStream(pcmIntermediate, src)) {

                // Then resample to target rate / mono
                AudioFormat target = new AudioFormat(
                    AudioFormat.Encoding.PCM_SIGNED,
                    TARGET_SAMPLE_RATE,
                    TARGET_SAMPLE_SIZE_BITS,
                    TARGET_CHANNELS,
                    TARGET_CHANNELS * 2,
                    TARGET_SAMPLE_RATE,
                    false
                );
                try (AudioInputStream resampled = AudioSystem.getAudioInputStream(target, pcm)) {
                    Files.deleteIfExists(wav);
                    AudioSystem.write(resampled, AudioFileFormat.Type.WAVE, wav.toFile());
                }
            }
        }
    }

    /** Standalone CLI: {@code Mp3ToWavConverter <input.mp3> <output.wav>}. */
    public static void main(String[] args) throws Exception {
        if (args.length != 2) {
            System.err.println("Usage: Mp3ToWavConverter <input.mp3> <output.wav>");
            System.exit(1);
        }
        convert(Path.of(args[0]), Path.of(args[1]));
        System.out.println("OK: " + args[0] + " -> " + args[1]);
    }
}
