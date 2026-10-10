import struct, sys, os
root = sys.argv[1]
for r in sys.argv[2].split(","):
    st = "STAGE" + r[0]
    path = os.path.join(root, "re15_port/shared_assets/PSX", st, "ROOM%s.RDT" % r)
    d = open(path, 'rb').read()
    off = struct.unpack_from('<I', d, 0x20)[0]
    cnt = struct.unpack_from('<5I', d, off + 4)
    p = off + 24; n = 0; up = []; down = []
    for g in range(5):
        for i in range(cnt[g]):
            w, dd, x, z, t, u0, u1, fl = struct.unpack_from('<HHhhBBBB', d, p); p += 12
            w10 = u1 | (fl << 8)
            b = ((w10 << 16) & 0xFFFFFFFF)
            b = (b - (1 << 32) if b & 0x80000000 else b) >> 28
            n += 1
            if w10 & 1: up.append((g, b, x, z, w, dd, hex(w10)))
            if w10 & 2: down.append((g, b, x, z, w, dd, hex(w10)))
    bands = sorted(set((((( (u1 | (fl << 8)) << 16) & 0xFFFFFFFF) ^ 0x80000000) - 0x80000000) >> 28 for (w, dd, x, z, t, u0, u1, fl) in [struct.unpack_from('<HHhhBBBB', d, off + 24 + 12 * k) for k in range(n)]))
    print("ROOM%s: %d Zellen, Baender %s, Aufstiegszellen (Wort&1): %d, Absprungzellen (Wort&2): %d" % (r, n, bands, len(up), len(down)))
    for u in up[:6]: print("   auf:", u)
