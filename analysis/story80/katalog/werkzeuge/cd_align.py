import numpy as np, sys
W=sys.argv[1]
def load(p):
    a=np.fromfile(p,np.uint8).reshape(-1,48,64).astype(np.float32)
    return a
def nz(a):
    a=a.reshape(len(a),-1); a=a-a.mean(1,keepdims=True); n=np.linalg.norm(a,axis=1,keepdims=True)+1e-6; return a/n
for i in range(1,5):
    u=load(f'{W}/cd/u{i}.raw'); s=load(f'{W}/cd/s{i}.raw')
    fu=30000/1001; fs=30.0
    U=nz(u); Sx=nz(s)
    # for sample of u frames, find best s frame
    res=[]
    for k in range(0,len(u),15):
        c=Sx@U[k]
        j=int(np.argmax(c)); res.append((k/fu, j/fs, float(c[j])))
    off=[b-a for a,b,c in res if c>0.9]
    print(f'film{i}: u {len(u)/fu:.2f}s s {len(s)/fs:.2f}s  n={len(res)} good={len(off)} offset median {np.median(off) if off else None:.3f} min {min(off) if off else 0:.3f} max {max(off) if off else 0:.3f}  corr min {min(r[2] for r in res):.3f} median {np.median([r[2] for r in res]):.3f}')
    # which s frames are not covered by u
    best_u=[]
    for j in range(0,len(s),15):
        c=U@Sx[j]; best_u.append((j/fs,float(c.max())))
    unc=[t for t,c in best_u if c<0.85]
    print('   s-times not in u (<0.85):', [round(t,1) for t in unc][:60])
    low=[(round(a,1),round(c,2)) for a,b,c in res if c<0.9]
    print('   u-times weak:', low[:40])
