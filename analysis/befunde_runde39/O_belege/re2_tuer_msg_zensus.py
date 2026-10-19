import os, sys, struct, glob
REPO = sys.argv[1]
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
from re2_scd_walk import scd_blocks, block_end, walk
ENC = {" ": 0x00, ":": 0x16, ",": 0x18, "\"": 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x01}
for i in range(10): ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i
DEC = {v: k for k, v in ENC.items()}
def msgtext(d, idx):
    offs = struct.unpack_from("<23I", d, 8); ms = offs[14]
    if not ms: return "?"
    n = struct.unpack_from("<H", d, ms)[0] // 2
    if idx >= n: return "?(idx %d >= %d)" % (idx, n)
    st = ms + struct.unpack_from("<H", d, ms + 2 * idx)[0]
    out = []; i = st
    while i < len(d) and i < st + 200:
        x = d[i]
        if x == 0xFE: break
        if x == 0xFC: out.append(" / "); i += 1; continue
        if x in (0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xEF, 0xEB, 0xF0): i += 2; continue
        out.append(DEC.get(x, "<%02x>" % x)); i += 1
    return "".join(out)
for p in sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT"))):
    d = open(p, "rb").read(); offs = struct.unpack_from("<23I", d, 8)
    rn = os.path.basename(p)[:-4]
    doors = {}   # slot -> rect
    allops = []
    for name, base, subs in scd_blocks(d):
        for i in range(len(subs)):
            s = base + subs[i]; e = block_end(d, base, subs, i, offs)
            ops, st = walk(d, s, e, last=(i + 1 == len(subs)))
            for (a, op, ln) in ops:
                allops.append((name, i, a, op, ln))
                if op == 0x3B:
                    doors[d[a + 1]] = d[a + 6:a + 14]
    rects = {v: k for k, v in doors.items()}
    for idx, (name, i, a, op, ln) in enumerate(allops):
        hit = None
        if op == 0x2C and d[a + 2] == 4 and d[a + 6:a + 14] in rects:
            hit = "aot_set slot %d MESSAGE msg %d ueber Tuer-Rechteck (Tuer-Slot %d)" % (d[a + 1], d[a + 14], rects[d[a + 6:a + 14]])
            msg = d[a + 14]
        elif op == 0x46 and d[a + 1] in doors and d[a + 2] == 4:
            hit = "aot_reset Tuer-Slot %d -> MESSAGE msg %d" % (d[a + 1], d[a + 4]); msg = d[a + 4]
        if hit:
            ctx = []
            for (n2, i2, a2, op2, ln2) in allops[max(0, idx - 12):idx]:
                if n2 == name and i2 == i and op2 in (0x21, 0x23, 0x06, 0x07, 0x5E, 0x3E):
                    ctx.append(d[a2:a2 + ln2].hex(" "))
            print("%s %s sub%02d @0x%05X %s \"%s\"   bed: %s" % (rn, name, i, a, hit, msgtext(d, msg), " | ".join(ctx[-5:])))
