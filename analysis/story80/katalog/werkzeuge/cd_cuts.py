import numpy as np, sys, json
W=sys.argv[1]
fu=30000/1001
out={}
for i in range(1,5):
    u=np.fromfile(f'{W}/cd/u{i}.raw',np.uint8).reshape(-1,48,64).astype(np.float32)
    d=np.abs(np.diff(u,axis=0)).mean((1,2))
    # cut if d > max(12, 4*median local)
    cuts=[0]
    for k in range(len(d)):
        loc=np.median(d[max(0,k-8):k+9])
        if d[k]>max(10,3.5*loc) and (k+1-cuts[-1])>4:
            cuts.append(k+1)
    cuts.append(len(u))
    segs=[(cuts[j]/fu,(cuts[j+1])/fu) for j in range(len(cuts)-1)]
    out[i]=segs
    print(f'film{i}: {len(segs)} shots')
    print('  '+' '.join(f'{a:.1f}' for a,b in segs))
json.dump(out,open(f'{W}/cd/cuts.json','w'))
