"""Claire-RDT SCD dump/scan (RE2 PL1). Nutzt re2_scd_walk/lens aus dem Worktree."""
import os, sys, struct, glob
T = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/analysis/nutzer_batch_2026-08-27/tools"
sys.path.insert(0, T)
from re2_scd_walk import scd_blocks, block_end, walk
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/tools/r34n_c")
RDT = r"C:/workspace/git/reAi_v2/info/re2clair/PL1/RDT"
def load_names():
    src = open(r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/tools/r34n_c/re2_scd_dump.py").read()
    i = src.index('N = ('); j = src.index('.split()', i)
    ns = {}
    exec(src[i:j+8], ns)
    return ns['N']
N = load_names()
def subs_of(path):
    d = open(path, "rb").read(); offs = struct.unpack_from("<23I", d, 8)
    for name, base, subs in scd_blocks(d):
        for i, so in enumerate(subs):
            s = base + so; e = block_end(d, base, subs, i, offs)
            if e <= s or e > len(d): continue
            ops, st = walk(d, s, e, last=(i + 1 == len(subs)))
            yield d, name, i, s, e, ops, st
if __name__ == "__main__":
    mode = sys.argv[1]
    if mode == "scan":
        for p in sorted(glob.glob(RDT + "/ROOM*.RDT")):
            ty = set()
            for d, name, i, s, e, ops, st in subs_of(p):
                for a, op, ln in ops:
                    if op == 0x44: ty.add(d[a+3])
            print(os.path.basename(p), " ".join("%02X" % t for t in sorted(ty)))
    elif mode == "dump":
        room = sys.argv[2]; blk = sys.argv[3] if len(sys.argv) > 3 else None
        want = int(sys.argv[4]) if len(sys.argv) > 4 else None
        for d, name, i, s, e, ops, st in subs_of(RDT + "/" + room + ".RDT"):
            if blk and name != blk: continue
            if want is not None and i != want: continue
            print("=== %s sub%02d @0x%05X..0x%05X status=%s" % (name, i, s, e, st))
            for (a, op, ln) in ops:
                nm = N[op] if op < len(N) else "op%02X" % op
                print("  @0x%05X  %-40s %s" % (a, d[a:a+ln].hex(" "), nm))
