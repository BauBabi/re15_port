import sys, numpy as np
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0]=[T+'/v8',T+'/v6kopf',T+'/v4']
import kopf_orig_bau as KB
import v4_gemein as G
blob=open(sys.argv[1],'rb').read()
d,o,ms,tim=KB.teile(blob); t=KB.tim_teile(tim)
c=t['clut'][493-480].astype(int)
rgb=np.stack([(c&31)*255/31.,((c>>5)&31)*255/31.,((c>>10)&31)*255/31.],-1)
lab=G.lab(rgb[None])[0]
L=lab[t['pix'][51:56,54:66]]
np.set_printoptions(linewidth=200,precision=0,suppress=True)
print('L* v51-55 x u54-65'); print(L[...,0])
print('C*'); print(np.hypot(L[...,1],L[...,2]))
med=np.median(L[...,0]); print('median L*',med)
for u0 in (54,55,56):
    sub=L[:,u0-54:,0]; print(u0,'min L*',sub.min(),'mean',sub.mean().round(1),'std',sub.std().round(1))
