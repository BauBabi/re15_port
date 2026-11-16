import sys, numpy as np
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0]=[T+'/v8',T+'/v6kopf']
import kopf_orig_bau as KB
blob=open(sys.argv[1],'rb').read()
d,o,ms,tim=KB.teile(blob); t=KB.tim_teile(tim)
print('cy',t['cy'],'clut rows',t['clut'].shape,'pix',t['pix'].shape)
def rgb(z,u,v,seite):
    i=t['pix'][v,u+seite*128]; c=int(t['clut'][z-t['cy']][i])
    return ((c&31)*8,((c>>5)&31)*8,((c>>10)&31)*8, c>>15)
print('C1 row493 seite0, v 51..55, u 52..67:')
for v in range(49,58):
    print(v,' '.join('%3d,%3d,%3d'%rgb(493,u,v,0)[:3] for u in range(52,68)))
print('grau row487 seite2, u 62..66 and 85..89 at v 76,100,131:')
for v in (76,90,100,120,131):
    print(v,' '.join('%3d,%3d,%3d'%rgb(487,u,v,2)[:3] for u in list(range(62,67))+list(range(85,90))))
# faces of tail meshes: per class, UV ranges
import collections
for b in range(16,23):
    m=ms[b]; cnt=collections.Counter()
    for uv in m['tuv']+m['quv']:
        z=(uv[2]>>6)&0x1FF; s=uv[5]&0xF; us=uv[0::3][:len(uv)//3]; vs=uv[1::3][:len(uv)//3]
        cnt[(z,s,min(us),max(us),min(vs),max(vs))]+=1
    zs=collections.Counter((k[0],k[1]) for k in cnt.elements())
    print(b, dict(zs))
