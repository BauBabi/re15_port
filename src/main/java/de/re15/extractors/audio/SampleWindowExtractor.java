package de.re15.extractors.audio;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Builds {@link SampleDataWindow} instances for a VB payload while taking
 * layout-specific quirks into account.
 */
public final class SampleWindowExtractor {
    private SampleWindowExtractor() {}

    public static List<SampleSlice> slice(byte[] vbData,
                                          List<VabFile.Sample> samples,
                                          SampleLayout layout) {
        if (vbData == null || vbData.length == 0 || samples == null || samples.isEmpty() || layout == null) {
            return Collections.emptyList();
        }
        return switch (layout) {
            case BIOHARB_VB -> sliceBioharb(vbData, samples, layout);
            default -> sliceStandard(vbData, samples, layout);
        };
    }

    private static List<SampleSlice> sliceStandard(byte[] vbData,
                                                   List<VabFile.Sample> samples,
                                                   SampleLayout layout) {
        List<SampleSlice> result = new ArrayList<>(samples.size());
        for (VabFile.Sample sample : samples) {
            SampleDataWindow window = SampleDataWindow.from(vbData, sample, layout);
            if (window != null) {
                result.add(new SampleSlice(sample, window));
            }
        }
        return result;
    }

    private static List<SampleSlice> sliceBioharb(byte[] vbData,
                                                  List<VabFile.Sample> samples,
                                                  SampleLayout layout) {
        List<SampleSlice> result = new ArrayList<>(samples.size());
        int cursor = 0;
        for (int i = 0; i < samples.size(); i++) {
            VabFile.Sample sample = samples.get(i);
            int nextDeclared = i + 1 < samples.size() ? samples.get(i + 1).offset() : Integer.MAX_VALUE;
            SampleDataWindow window = buildBioharbWindow(vbData, sample, nextDeclared, layout, cursor);
            if (window == null) {
                break;
            }
            result.add(new SampleSlice(sample, window));
            cursor = window.chunkOffset() + window.chunkLength();
        }
        return result;
    }

    private static SampleDataWindow buildBioharbWindow(byte[] data,
                                                       VabFile.Sample sample,
                                                       int nextDeclaredOffset,
                                                       SampleLayout layout,
                                                       int cursor) {
        int chunkOffset = clamp(Math.max(cursor, sample.offset()), 0, data.length);
        if (chunkOffset >= data.length) {
            return null;
        }
        int prefix = Math.min(layout.prefixBytes(), Math.max(0, data.length - chunkOffset));
        int dataOffset = chunkOffset + prefix;
        int declaredEnd = clamp(sample.offset() + sample.length(), chunkOffset, data.length);
        int nextStart = clamp(Math.max(nextDeclaredOffset, chunkOffset), chunkOffset, data.length);
        int maxChunkLength = clamp(nextStart + layout.prefixBytes(), chunkOffset, data.length) - chunkOffset;

        int nominalData = Math.max(0, declaredEnd - dataOffset);
        int maxData = Math.max(0, maxChunkLength - prefix);
        int dataLength = Math.min(alignUp(nominalData, 16), maxData);
        if (dataLength <= 0) {
            return null;
        }
        int chunkLength = prefix + dataLength;
        return SampleDataWindow.create(chunkOffset, chunkLength, dataOffset, dataLength);
    }

    private static int alignUp(int value, int multiple) {
        if (multiple <= 0) {
            return value;
        }
        int remainder = value % multiple;
        if (remainder == 0) {
            return value;
        }
        return value + (multiple - remainder);
    }

    private static int clamp(int value, int min, int max) {
        if (value < min) {
            return min;
        }
        if (value > max) {
            return max;
        }
        return value;
    }

    public record SampleSlice(VabFile.Sample sample, SampleDataWindow window) {}
}
