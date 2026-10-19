import sys, os, glob
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041/re15_port/tools')
import scd_walk_lib as L
base = r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041/re15_port/shared_assets/PSX'
# args: bank bit [bit...]  -> lists Set/Ck of (bank,bit) in all RDTs of all stages
bank = int(sys.argv[1], 0); bits = [int(x, 0) for x in sys.argv[2:]]
for p in sorted(glob.glob(base + '/STAGE*/ROOM*.RDT')):
    d = open(p, 'rb').read()
    try:
        regs = L.regionen(d)
    except Exception as e:
        continue
    for (tag, idx), ops in sorted(regs.items()):
        for pc, op, sz in ops:
            if op in (0x21, 0x22) and d[pc+1] == bank and d[pc+2] in bits:
                print("%s %s%02d @0x%05X %s(%d,0x%02X)=%d" % (os.path.basename(p)[:-4], tag, idx, pc, 'Ck' if op == 0x21 else 'Set', bank, d[pc+2], d[pc+3]))
