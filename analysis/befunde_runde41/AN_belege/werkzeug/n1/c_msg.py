import sys, struct
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/tools/r34n_b")
from msg_suche import dec2
R = r"C:/workspace/git/reAi_v2/info/re2clair/PL1/RDT/"
room = sys.argv[1]
d = open(R + room + ".RDT", "rb").read()
offs = struct.unpack_from("<23I", d, 8)
print(" ".join("%d:%X" % (i, o) for i, o in enumerate(offs)))
for k in (13, 14):
    b = offs[k]
    if not b: continue
    n = struct.unpack_from("<H", d, b)[0] // 2
    ptr = struct.unpack_from("<%dH" % n, d, b)
    print("-- offs[%d] @0x%X, %d Nachrichten" % (k, b, n))
    for i, p in enumerate(ptr):
        e = d.find(b"\xfe", b + p)
        print("  msg %2d @0x%05X: %s" % (i, b + p, dec2(d[b + p:e + 1])))
