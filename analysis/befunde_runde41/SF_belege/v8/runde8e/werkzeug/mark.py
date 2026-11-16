# Messkopie: Texel eines Bereichs auf den freien Index 0 setzen, Index 0 in allen CLUT-Zeilen = reines Gruen (nur Messung, nie Spiel-Asset)
import sys, numpy as np
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0]=[T+'/v8',T+'/v6kopf']
import kopf_orig_bau as KB
src,dst=sys.argv[1],sys.argv[2]
blob=bytearray(open(src,'rb').read())
d,o,ms,tim=KB.teile(bytes(blob)); t=KB.tim_teile(tim)
farbe={'g':0x03E0,'m':0x7C1F}
for spec in sys.argv[3:]:
    seite,us,vs=spec.split(':'); u0,u1=map(int,us.split('-')); v0,v1=map(int,vs.split('-'))
    t['pix'][v0:v1+1,int(seite)*128+u0:int(seite)*128+u1+1]=0
t['clut'][:,0]=0x03E0
neu=KB.tim_packen(t)
assert len(neu)==len(tim), (len(neu),len(tim))
blob[d[8]:d[8]+len(tim)]=neu
open(dst,'wb').write(blob)
print('ok',dst)
