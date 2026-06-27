package de.re15.extractors.audio;

/**
 * Describes how the sample data is arranged inside a VB file.
 */
public enum SampleLayout {
    /**
     * Standard PlayStation VB layout: samples are stored back to back without
     * any additional metadata.
     */
    STANDARD(0, false),

    /**
     * Layout used by the BioHarb BGM extractions where each sample is prefixed
     * with a 20 byte metadata stub before the actual ADPCM frames.
     */
    BIOHARB_VB(20, true);

    private final int prefixBytes;
    private final boolean zeroPadded;

    SampleLayout(int prefixBytes, boolean zeroPadded) {
        this.prefixBytes = prefixBytes;
        this.zeroPadded = zeroPadded;
    }

    public int prefixBytes() {
        return prefixBytes;
    }

    public boolean requiresZeroPadding() {
        return zeroPadded;
    }
}
