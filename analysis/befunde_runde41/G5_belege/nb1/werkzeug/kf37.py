import sys
sys.path.insert(0, '.')
from re2ss import SS
from tent_kf import angles
s = SS(r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s04.sav")
EMR = 0x801512f0
for kf in list(range(0,7)) + [747, 760, 776, 777, 806, 807, 836]:
    h, a = angles(s, EMR, kf, 4)
    print(kf, h, [tuple((x if x < 2048 else x-4096) for x in t) for t in a])
