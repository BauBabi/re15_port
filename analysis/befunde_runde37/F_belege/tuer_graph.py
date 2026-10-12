# Statischer Tuer-Zensus STAGE1 (Leon-Raeume ROOM1xx0): jede Door_aot_set (0x3b) aus main00 + allen subs,
# Ziel = Stage-Byte [22] + Raum-Byte [23] (re15_adaruf.h: @0x00B70/@0x00B71), sce [2], Band [4].
import sys, os, re, glob
dumpdir = sys.argv[1]
edges = []
for f in sorted(glob.glob(os.path.join(dumpdir, "ROOM1??0.txt"))):
    room = os.path.basename(f)[4:8]
    region = None
    for line in open(f, encoding="utf-8", errors="replace"):
        m = re.match(r"=== (\w+)", line)
        if m: region = m.group(1); continue
        m = re.search(r"(0x[0-9A-F]+) \(\+[0-9A-F]+\) Door_aot_set\s+((?:[0-9a-f]{2} )+)", line)
        if not m: continue
        b = [int(x, 16) for x in m.group(2).split()]
        sce = b[2]; band = b[4]; stage = b[22]; rb = b[23]
        tgt = "%X%02X0" % (stage + 1, rb)
        rect = tuple(int.from_bytes(bytes(b[6+2*i:8+2*i]), "little", signed=True) for i in range(4))
        edges.append((room, tgt, region, m.group(1), b[1], sce, band, rect))
for e in edges:
    print("%s -> %s  %-6s %s slot=%d sce=%d band=%d rect=%s" % e)
