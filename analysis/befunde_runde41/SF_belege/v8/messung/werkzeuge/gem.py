import os, sys
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
for p in ('foto','v4','v7hand','v3','v2',''):
    sys.path.insert(0, os.path.join(T,p))
sys.path.insert(0, os.path.dirname(T))
import numpy as np
from PIL import Image
S8=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
ALLE=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/roh/alle'
V80=r'C:/workspace/git/reAi_v2/Version_80%/'
EMD_V7=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD'
def vorlage():
    return np.asarray(Image.open(V80+'Darstellung_Vorlage.png').convert('RGB')).astype(float)
def frame(n):  # Videobild nN = f(N+1)
    return np.asarray(Image.open(ALLE+'/f%04d.png'%(n+1)).convert('RGB')).astype(float)
