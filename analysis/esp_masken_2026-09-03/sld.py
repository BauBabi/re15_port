import struct, os, sys, glob

def sld_decompress(src, sp, size, limit=None):
    """Port of SldDecoder.decompress (FUN_800c47e8). limit: stop after this many out bytes."""
    cap = size if limit is None else min(size, limit)
    dst = bytearray(size)
    dp = 0; flagMask = 0; flagByte = 0
    n = len(src)
    while dp < cap:
        if flagMask == 0:
            if sp >= n: raise ValueError("eof")
            flagByte = src[sp]; sp += 1; flagMask = 0x80
        if sp >= n: raise ValueError("eof")
        a = src[sp]; sp += 1
        if a < 0x80:
            dst[dp] = a; dp += 1
        elif (flagByte & flagMask) == 0:
            dst[dp] = a; dp += 1; flagMask >>= 1
        else:
            if sp >= n: raise ValueError("eof")
            b2 = src[sp]; sp += 1
            raw = (a << 4) | (b2 >> 4)
            if b2 & 0x0F:
                ln = (b2 & 0x0F) + 2
                off = raw - 0x1000 if raw & 0x800 else raw
            else:
                if sp >= n: raise ValueError("eof")
                b3 = src[sp]; sp += 1
                ln = (b3 & 0x3F) + 3
                s = raw - 0x1000 if raw & 0x800 else raw
                off = (s << 2) | (b3 >> 6)
            flagMask >>= 1
            base = dp + off
            if base < 0: raise ValueError("neg")
            if dp + ln > size: raise ValueError("ovf")
            for k in range(ln):
                dst[dp + k] = dst[base + k]
            dp += ln
    return bytes(dst), sp

def find_sld(chunk, aligned_only=False):
    """Return list of (offset, decompressed_size) of valid SLD->TIM blocks."""
    hits = []
    n = len(chunk)
    step = 4 if aligned_only else 1
    i = 8
    while i + 4 < n:
        v = struct.unpack_from("<I", chunk, i)[0]
        if 0x2000 <= v <= 0x40000:
            try:
                head, _ = sld_decompress(chunk, i + 4, v, limit=16)
                if head[0] == 0x10 and head[1] == 0 and head[2] == 0 and head[3] == 0 and (head[4] & 7) <= 3:
                    # full decode to confirm
                    full, _ = sld_decompress(chunk, i + 4, v)
                    hits.append((i, v))
                    i += 4
                    continue
            except Exception:
                pass
        i += step
    return hits
