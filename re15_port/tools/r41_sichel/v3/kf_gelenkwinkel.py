import os
import sys, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools')
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v2')
import b2, birkin_buehne as BB
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from gelenke import winkel
mo = b2.modell(sys.argv[1])
for clip, bild in [(1,0),(1,26),(1,10),(0,0),(11,85)]:
    kf = BB.kf_von_clip(mo, clip, bild)
    P = b2.kette_punkte(mo, kf)
    print(clip, bild, np.round(winkel(P),1))
