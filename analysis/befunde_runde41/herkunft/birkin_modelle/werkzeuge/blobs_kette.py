import struct
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
def kette(a):
    """Sektorweise: jeder Sektor mit plausiblem EMD-Kopf. Siehe blobs.py."""
    out = []
    s = 0
    n = len(a)
    while s*2048 + 8 <= n:
        o = s*2048
        d, c = u32(a, o), u32(a, o+4)
        if d >= 36 and not (d & 3) and o + d + 36 <= n and c in (8, 9) and o+d+c*4 <= n:
            e = [u32(a, o+d+i*4) for i in range(c)]
            if e == sorted(e) and e[0] == 8:
                ln = d + c*4
                out.append((s, ln, c))
                s += (ln + 2047) // 2048
                continue
        s += 1
    return out
