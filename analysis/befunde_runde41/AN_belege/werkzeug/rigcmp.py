import sys, struct
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_annette/re15_port/tools")
import emd_ansichtsblatt as E
from re2em import R2
import io, contextlib
with contextlib.redirect_stdout(io.StringIO()):
    a = E.Modell(0x4D)
    b = R2(0x47)
for nm, m in (("RE1.5 EM4D", a), ("RE2 EM47", b)):
    s = m.skel
    print(nm, "bones", s.bone_count, "clips", len(m.clips), "frames", len(m.frames) if hasattr(m.frames,'__len__') else m.frames)
    for i in range(s.bone_count):
        print("   %2d parent %2d rel %s  mesh tv %d" % (i, s.parent[i], tuple(s.rel[i]), len(m.meshes[i].tv) if i < len(m.meshes) else -1))
