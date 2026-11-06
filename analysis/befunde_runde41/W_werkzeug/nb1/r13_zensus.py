"""Zensus: welche RDT-ESP-Zeilen tragen Routine A=13 (Layout wie wa_esp.py / re15_esp.c esp_rowblk)."""
import struct, sys, glob, os
def scan(fn):
    d = open(fn, "rb").read()
    try:
        idh, pe = struct.unpack_from("<II", d, 0x4C)
    except struct.error:
        return []
    if not (0 < idh < len(d) and 0 < pe <= len(d)): return []
    ids = []
    for i in range(32):
        if idh + i >= len(d) or d[idh + i] == 0xFF: break
        ids.append(d[idh + i])
    hits = []
    for k, eid in enumerate(ids):
        try:
            entry = struct.unpack_from("<i", d, pe - 4 * k)[0]
            es = idh + entry
            ca, cb = struct.unpack_from("<HH", d, es)
            ee = es + (ca * 2 + cb + 2) * 4
            subt = struct.unpack_from("<8H", d, ee)
        except struct.error:
            continue
        seen = set()
        for sub in range(8):
            so = subt[sub] * 4
            if so == 0 or so in seen: continue
            seen.add(so)
            base = ee + so
            if base + 4 > len(d): continue
            streams = struct.unpack_from("<H", d, base)[0]
            if streams == 0 or streams > 16: continue
            p = base + 4
            for s in range(streams):
                if p + 4 > len(d): break
                nr = struct.unpack_from("<H", d, p)[0]
                if nr == 0 or nr > 64: break
                for r in range(nr):
                    ro = p + 4 + r * 40
                    if ro + 40 > len(d): break
                    A = struct.unpack_from("<H", d, ro)[0]
                    if A == 13:
                        hits.append((eid, sub, s, r, ro, struct.unpack_from("<3h", d, ro + 8), struct.unpack_from("<H", d, ro + 0x16)[0]))
                p += 4 + nr * 40
    return hits
for root in sys.argv[1:]:
    fs = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
    rooms = {}
    for fn in fs:
        h = scan(fn)
        if h: rooms[os.path.basename(fn)] = h
    print("==", root, "RDTs:", len(fs), "mit Routine 13:", len(rooms))
    for k, v in rooms.items():
        print("  %s %s" % (k, "; ".join("eff 0x%02X sub %d str %d zeile %d @0x%X accel=%s +16=0x%X" % x for x in v)))
