"""match.py out.json q1.png[@label] ... : SIFT+RANSAC gegen Disc- (D) und Mod-Hintergruende (M), Top 5 je Bild.
Hintergruende: s40/bg40 (Disc, 531) und s40/pruefung/va/bgmod (Mod = Port-Baum, 1119)."""
import cv2, glob, json, os, sys, numpy as np
S=r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40"
out=sys.argv[1]; qs=sys.argv[2:]
clahe=cv2.createCLAHE(clipLimit=3.0,tileGridSize=(4,4))
sift=cv2.SIFT_create(nfeatures=1500,contrastThreshold=0.02,edgeThreshold=15)
names=[];kps=[];des=[];own=[]
for tag,d in (('D',S+'/bg40'),('M',S+'/pruefung/va/bgmod')):
    for f in sorted(glob.glob(d+'/ROOM*.ppm')):
        g=cv2.cvtColor(cv2.imread(f),cv2.COLOR_BGR2GRAY); g=cv2.resize(g,(640,480),interpolation=cv2.INTER_CUBIC)
        k,dd=sift.detectAndCompute(clahe.apply(g),None)
        if dd is None: dd=np.zeros((0,128),np.float32)
        i=len(names); names.append(tag+os.path.basename(f)[4:-4]); kps.append(np.array([p.pt for p in k],np.float32).reshape(-1,2)); des.append(dd); own.append(np.full(len(dd),i,np.int32))
D=np.vstack(des).astype(np.float32); O=np.concatenate(own); offs=np.cumsum([0]+[len(d) for d in des])
fl=cv2.FlannBasedMatcher(dict(algorithm=1,trees=6),dict(checks=64)); fl.add([D]); fl.train()
res={}
for q in qs:
    p,_,lab=q.partition('@'); lab=lab or os.path.basename(p)
    im=cv2.imread(p); g=cv2.cvtColor(im,cv2.COLOR_BGR2GRAY); g=cv2.resize(g,(640,480))
    kp,d=sift.detectAndCompute(clahe.apply(g),None)
    votes=np.zeros(len(names)); pairs=[[] for _ in names]
    if d is not None and len(d)>8:
        for x in fl.knnMatch(d.astype(np.float32),k=2):
            if len(x)<2: continue
            a,b=x
            if a.distance<0.82*b.distance:
                o=O[a.trainIdx]; votes[o]+=1; pairs[o].append((kp[a.queryIdx].pt,tuple(kps[o][a.trainIdx-offs[o]])))
    cand=[]
    for o in np.argsort(-votes)[:25]:
        pp=pairs[o]; inl=0
        if len(pp)>=6:
            A=np.array([z[0] for z in pp],np.float32); B=np.array([z[1] for z in pp],np.float32)
            H,ma=cv2.findHomography(A,B,cv2.RANSAC,12.0,maxIters=1500)
            if H is not None: inl=int(ma.sum())
        cand.append((names[o],int(votes[o]),inl))
    cand.sort(key=lambda c:(-c[2],-c[1]))
    res[lab]=cand[:6]
    print(lab,' '.join(f'{n}:{i}' for n,v,i in cand[:6]),flush=True)
json.dump(res,open(out,'w'),indent=1)
