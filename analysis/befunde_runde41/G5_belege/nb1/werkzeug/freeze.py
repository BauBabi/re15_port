import numpy as np
pts=np.array([float(l.strip().rstrip(',')) for l in open('pts.txt') if l.strip().rstrip(',')])
fr=np.fromfile('frames_gray.raw',dtype=np.uint8).reshape(-1,120,160).astype(np.int16)
n=min(len(pts),len(fr)); pts=pts[:n]; fr=fr[:n]
d=np.abs(fr[1:]-fr[:-1]).mean(axis=(1,2))
# runs of d<=0.05
run=0; st=None
for i in range(len(d)):
    t=pts[i+1]
    if t<55 or t>106: continue
    if d[i]<=0.05:
        if run==0: st=pts[i]
        run+=1
    else:
        if run>=8: print('Standbild %.3f .. %.3f s (%d Videobilder = %.3f s), naechste Aenderung diff %.1f'%(st,pts[i],run,pts[i]-st,d[i]))
        run=0
# big cuts
for i in range(len(d)):
    if 55<pts[i+1]<106 and d[i]>20: print('Schnittkante bei %.3f s (diff %.1f)'%(pts[i+1],d[i]))
