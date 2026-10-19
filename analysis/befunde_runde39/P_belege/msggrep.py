import sys, os, glob, re
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041/re15_port/tools')
import scd_walk_lib as L
base = r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041/re15_port/shared_assets/PSX'
pat = re.compile(sys.argv[1], re.I)
for p in sorted(glob.glob(base + '/STAGE*/ROOM*.RDT')):
    d = open(p, 'rb').read()
    try:
        m = L.messages(d)
    except Exception:
        continue
    for i, (t, c) in sorted(m.items()):
        if pat.search(t):
            print(os.path.basename(p)[:-4], i, repr(t)[:150], c[:3])
