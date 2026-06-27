package de.re15.extractors.audio;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;

/**
 * Converts simple PS1 SEQ sequences into Standard MIDI files.
 */
public final class SeqToMidiConverter {
    private static final byte[] MIDI_HEADER = "MThd".getBytes(StandardCharsets.US_ASCII);
    private static final byte[] TRACK_HEADER = "MTrk".getBytes(StandardCharsets.US_ASCII);
    private static final byte[] GM_RESET = {0x00, (byte) 0xF0, 0x05, 0x7E, 0x7F, 0x09, 0x01, (byte) 0xF7};

    public byte[] convert(byte[] seqData) {
        if (seqData == null || seqData.length < 16) {
            throw new IllegalArgumentException("SEQ Datei ist zu klein.");
        }
        if (seqData[0] != 'p' || seqData[1] != 'Q' || seqData[2] != 'E' || seqData[3] != 'S') {
            throw new IllegalArgumentException("Unbekannter SEQ Header.");
        }
        int division = readUnsignedShort(seqData, 8);
        int tempo = read24Bit(seqData, 10);
        int numerator = Byte.toUnsignedInt(seqData[13]);
        int denominator = Byte.toUnsignedInt(seqData[14]);
        int dataOffset = 0x10;
        if (dataOffset > seqData.length) {
            dataOffset = seqData.length;
        }
        byte[] score = trimScoreData(seqData, dataOffset);
        boolean hasEnd = endsWithEndOfTrack(score);
        ByteArrayOutputStream track = new ByteArrayOutputStream();
        try {
            writeTempo(track, tempo);
            writeTimeSignature(track, numerator, denominator);
            track.write(GM_RESET);
            track.write(score);
            if (!hasEnd) {
                track.write(0x00);
                track.write(0xFF);
                track.write(0x2F);
                track.write(0x00);
            }
        } catch (IOException e) {
            throw new IllegalStateException("Fehler beim Schreiben der MIDI Daten.", e);
        }
        byte[] trackBytes = track.toByteArray();
        ByteArrayOutputStream midi = new ByteArrayOutputStream(MIDI_HEADER.length + TRACK_HEADER.length + trackBytes.length + 12);
        try {
            midi.write(MIDI_HEADER);
            midi.write(intToBytes(6));
            midi.write(shortToBytes(0)); // format 0
            midi.write(shortToBytes(1)); // one track
            midi.write(shortToBytes(division));
            midi.write(TRACK_HEADER);
            midi.write(intToBytes(trackBytes.length));
            midi.write(trackBytes);
        } catch (IOException e) {
            throw new IllegalStateException("Fehler beim Schreiben der MIDI Datei.", e);
        }
        return midi.toByteArray();
    }

    private static void writeTempo(ByteArrayOutputStream out, int tempo) throws IOException {
        out.write(0x00);
        out.write(0xFF);
        out.write(0x51);
        out.write(0x03);
        out.write((tempo >> 16) & 0xFF);
        out.write((tempo >> 8) & 0xFF);
        out.write(tempo & 0xFF);
    }

    private static void writeTimeSignature(ByteArrayOutputStream out, int numerator, int denominator) throws IOException {
        int denomPower = 0;
        int value = Math.max(1, denominator);
        while ((1 << denomPower) < value && denomPower < 7) {
            denomPower++;
        }
        out.write(0x00);
        out.write(0xFF);
        out.write(0x58);
        out.write(0x04);
        out.write(Math.max(1, numerator));
        out.write(denomPower);
        out.write(0x18); // default metronome ticks
        out.write(0x08); // 32nd notes per beat
    }

    private static byte[] trimScoreData(byte[] source, int offset) {
        if (offset >= source.length) {
            return new byte[0];
        }
        int end = findEndOfTrack(source, offset);
        if (end < offset) {
            return Arrays.copyOfRange(source, offset, source.length);
        }
        return Arrays.copyOfRange(source, offset, end + 3);
    }

    private static int findEndOfTrack(byte[] data, int offset) {
        for (int i = data.length - 3; i >= offset; i--) {
            if ((data[i] & 0xFF) == 0xFF && (data[i + 1] & 0xFF) == 0x2F && data[i + 2] == 0x00) {
                return i;
            }
        }
        return -1;
    }

    private static boolean endsWithEndOfTrack(byte[] data) {
        if (data.length < 3) {
            return false;
        }
        int len = data.length;
        return (data[len - 3] & 0xFF) == 0xFF && (data[len - 2] & 0xFF) == 0x2F && data[len - 1] == 0x00;
    }

    private static int readUnsignedShort(byte[] data, int offset) {
        if (offset + 1 >= data.length) {
            return 0;
        }
        return ((data[offset] & 0xFF) << 8) | (data[offset + 1] & 0xFF);
    }

    private static int read24Bit(byte[] data, int offset) {
        if (offset + 2 >= data.length) {
            return 0;
        }
        return ((data[offset] & 0xFF) << 16)
            | ((data[offset + 1] & 0xFF) << 8)
            | (data[offset + 2] & 0xFF);
    }

    private static byte[] shortToBytes(int value) {
        return new byte[] {(byte) ((value >> 8) & 0xFF), (byte) (value & 0xFF)};
    }

    private static byte[] intToBytes(int value) {
        return new byte[] {
            (byte) ((value >> 24) & 0xFF),
            (byte) ((value >> 16) & 0xFF),
            (byte) ((value >> 8) & 0xFF),
            (byte) (value & 0xFF)
        };
    }
}
