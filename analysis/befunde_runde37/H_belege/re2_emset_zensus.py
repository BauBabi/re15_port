import sys, os, struct, collections, glob
REPO = r"C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv"
os.environ.setdefault("RE15_INFO_DIR", r"C:/workspace/git/reAi_v2/info/Resident_Evil_und_Playstation_Information")
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde30", "tools"))
import r30_re2_scd as R
rdts = sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT")))
cnt = collections.Counter(); rows = []
for p in rdts:
    d = open(p, "rb").read()
    room = os.path.basename(p)[4:8]
    for which, nm in ((16, "main"), (17, "sub")):
        try: bl = R.blocks(d, which)
        except Exception as ex: continue
        for (i, s, e) in bl:
            ops, st = R.walk(d, s, e)
            for (off, op, r) in ops:
                if op != 0x44 or len(r) < 22: continue
                typ = r[3]; beh = struct.unpack_from('<H', r, 4)[0]
                x, y, z, rot = struct.unpack_from('<4h', r, 10)
                if 0x10 <= typ <= 0x1f:
                    cnt[(typ, beh & 0x3f)] += 1
                    rows.append((room, nm, i, off, typ, beh, x, y, z, rot, r.hex(' ')))
mode = sys.argv[1] if len(sys.argv) > 1 else "sum"
if mode == "sum":
    by = collections.Counter()
    for (t, b), n in cnt.items(): by[b] += n
    print("descriptor (+0x10E & 0x3F) -> count over all zombie Sce_em_set (type 0x10..0x1F):")
    for b in sorted(by): print("  %2d (0x%02x): %d" % (b, b, by[b]))
    print("total", sum(by.values()), "rooms", len(set(r[0] for r in rows)))
else:
    for r in rows:
        if mode == "all" or r[0].startswith(mode):
            print("%s %s%02d @0x%05X typ 0x%02x beh 0x%04x pos(%d,%d,%d) rot %d | %s" % r)
