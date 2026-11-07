import sys
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/analysis/story_begegnungen/werkzeuge")
import jp
d = open(r"C:/workspace/git/reAi_v2/Version_40%/PSX/STAGE3/ROOM30E1.RDT", "rb").read()
off = 0x0067D
print("Bytes @0x%05X:" % off, d[off:off+80].hex(" "))
r = jp.decode(d, off, off + 200)
print(r if isinstance(r, str) else "".join(x if isinstance(x, str) else str(x) for x in r))
