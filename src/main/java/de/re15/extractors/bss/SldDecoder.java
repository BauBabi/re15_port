package de.re15.extractors.bss;

/**
 * RE1.5 "BSS SLD" foreground-atlas decompressor.
 *
 * EXACT port of the game routine FUN_800c47e8 (recovered by disassembling a
 * DuckStation save-state RAM image at virtual 0x800c47e8 — it is not in the
 * static ghidra export). Verified BYTE-EXACT against the game's own VRAM output
 * for ROOM1170 cuts 1/2/4 (65536/65536 image bytes each). See
 * scripts/sld_decompress.py for the reference + provenance.
 *
 * Each BSS camera-cut chunk carries, AFTER the main MDEC background, an optional
 * SLD-compressed Sony TIM = the per-cut foreground-occluder atlas (8-bit, 256x256,
 * CLUT at VRAM (0,480)). The block is {LE32 decompressed_size}{flag-byte stream}.
 *
 * Decode (a = data byte; the flag byte is consumed MSB-first but ONLY for a>=0x80):
 *   if a < 0x80                      -> raw literal (no flag bit consumed)
 *   else if (flagByte &amp; flagMask)==0 -> literal (keeps bit7); consume flag bit
 *   else                             -> back-reference; consume flag bit
 *        b2; raw=(a&lt;&lt;4)|(b2&gt;&gt;4)
 *        if (b2 &amp; 0x0f): len=(b2&amp;0xf)+2,  off=signext12(raw)
 *        else:           b3; len=(b3&amp;0x3f)+3, off=(signext12(raw)&lt;&lt;2)|(b3&gt;&gt;6)
 *        copy len bytes from dst[dp+off]   (off is negative; overlapping)
 */
public final class SldDecoder {

    private SldDecoder() {}

    /** Decompress an SLD block that begins with its LE32 decompressed-size word. */
    public static byte[] decompressBlock(byte[] data, int off) {
        int size = (data[off] & 0xFF) | ((data[off + 1] & 0xFF) << 8)
                 | ((data[off + 2] & 0xFF) << 16) | ((data[off + 3] & 0xFF) << 24);
        return decompress(data, off + 4, size);
    }

    /** Decompress {@code size} bytes from the flag-byte stream at {@code src[sp]}. */
    public static byte[] decompress(byte[] src, int sp, int size) {
        byte[] dst = new byte[size];
        int dp = 0, flagMask = 0, flagByte = 0;
        while (dp < size) {
            if (flagMask == 0) {
                flagByte = src[sp++] & 0xFF;
                flagMask = 0x80;
            }
            int a = src[sp++] & 0xFF;
            if ((a & 0x80) == 0) {
                dst[dp++] = (byte) a;                       // raw literal (<0x80)
            } else if ((flagByte & flagMask) == 0) {
                dst[dp++] = (byte) a;                       // flagged literal (>=0x80)
                flagMask >>= 1;
            } else {                                        // back-reference
                int b2 = src[sp++] & 0xFF;
                int raw = (a << 4) | (b2 >> 4);
                int len, backOff;
                if ((b2 & 0x0F) != 0) {
                    len = (b2 & 0x0F) + 2;
                    backOff = signExt12(raw);
                } else {
                    int b3 = src[sp++] & 0xFF;
                    len = (b3 & 0x3F) + 3;
                    backOff = (signExt12(raw) << 2) | (b3 >> 6);
                }
                flagMask >>= 1;
                int base = dp + backOff;                    // backOff < 0
                for (int k = 0; k < len; k++) {
                    dst[dp + k] = dst[base + k];
                }
                dp += len;
            }
        }
        return dst;
    }

    private static int signExt12(int v) {
        return (v & 0x800) != 0 ? v - 0x1000 : v;
    }

    /**
     * Locate the foreground SLD block inside a 64KB BSS cut chunk (it follows the
     * main MDEC background). Heuristic: scan for an LE32 size word that, when
     * decompressed, yields a valid Sony TIM (magic 0x10). Returns the block offset
     * or -1 (cut has no foreground). Validating by decompression makes it robust
     * across rooms/atlas sizes rather than hard-coding 0x10220.
     */
    public static int findBlock(byte[] chunk) {
        for (int i = 8; i + 4 < chunk.length; i++) {
            int v = (chunk[i] & 0xFF) | ((chunk[i + 1] & 0xFF) << 8)
                  | ((chunk[i + 2] & 0xFF) << 16) | ((chunk[i + 3] & 0xFF) << 24);
            if (v < 0x2000 || v > 0x40000) continue;        // plausible TIM size only
            try {
                byte[] out = decompress(chunk, i + 4, v);    // full decode → validate
                if (out.length >= 8 && out[0] == 0x10 && out[1] == 0
                        && out[2] == 0 && out[3] == 0 && (out[4] & 0x07) <= 3) {
                    return i;                                // valid Sony TIM magic + bpp
                }
            } catch (RuntimeException ignore) {
                // bad candidate offset (out-of-bounds copy) — keep scanning
            }
        }
        return -1;
    }
}
