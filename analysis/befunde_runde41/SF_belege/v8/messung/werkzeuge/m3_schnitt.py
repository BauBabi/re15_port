import numpy as np, os
from PIL import Image
A=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/analysis/befunde_runde41/SF_belege/roh/alle'
ref=np.asarray(Image.open(A+'/f1440.png').convert('RGB'),np.float32)
for n in range(1340,1539,4):
    I=np.asarray(Image.open(A+'/f%04d.png'%n).convert('RGB'),np.float32)
    d=np.abs(I-ref)
    print(n,'wand %.1f'%d[60:140,0:150].mean(),'tuer %.1f'%d[150:340,280:480].mean(),'boden %.1f'%d[380:450,0:250].mean())
