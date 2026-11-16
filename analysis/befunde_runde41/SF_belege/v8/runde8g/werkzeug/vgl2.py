import sys, json, os, numpy as np
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0]=[T+'/v8',T+'/v6kopf',T+'/v4']
import kopf_orig_bau as KB, schwanz_r8 as Q, riegel_schwanz_r8 as RS
A=sys.argv[1]; neu=sys.argv[2]
L=lambda n: KB.teile(open(os.path.join(A,n),'rb').read())
_,_,c8,_=L('8c.EMD'); _,_,e8,_=L('8e.EMD'); _,_,g8,_=L(neu); _,_,ein,_=L('7e.EMD')
phi={int(k):float(v) for k,v in json.load(open(T+'/v8/unterseite_v8.json'))['innen_phi'].items()}
alt16={tuple(f[1::2]) for f in ein[16]['tf']+ein[16]['qf'] if set(f[1::2])<=set(range(22))|{34}}
tot=dict(bauch=0,ober=0,bauch_ne_8e=0,ober_ne_8c=0,bauch_eq_8c=0)
for b in range(16,23):
    m=g8[b]; P=np.array([v[:3] for v in m['v']],float); eh,eq=Q.achsen(phi[b]); th,gr=RS.ring_winkel(b,P,eh,eq)
    for key,F,U in (('tf','tf','tuv'),('qf','qf','quv')):
        for i,(f,uv) in enumerate(zip(m[F],m[U])):
            e=list(f[1::2]); ring=[k for k in e if k in th]
            wurzel = b==16 and tuple(e) in alt16
            nadel = b==22 and all(P[k][1]>=Q.ENDE_Y-1.0 for k in e)
            bauch = (not wurzel) and (not nadel) and all(th[k]<=gr[k]+RS.TOL_GRAD for k in ring)
            if bauch:
                tot['bauch']+=1; tot['bauch_ne_8e']+= uv!=e8[b][U][i]; tot['bauch_eq_8c']+= uv==c8[b][U][i]
            else:
                tot['ober']+=1; tot['ober_ne_8c']+= uv!=c8[b][U][i]
print(tot)
