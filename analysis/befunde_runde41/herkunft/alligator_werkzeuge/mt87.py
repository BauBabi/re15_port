# V1 86.57..87.17 s gemittelt; Vorlage = Wandbereich mit Leiter (ohne Wasserzeichen). cv2.matchTemplate TM_CCOEFF_NORMED
# gegen alle Disc-Hintergruende (s40/bg40) + Mod/Port-Hintergruende (bgall), Skalen 0.8..1.4.
import glob,os,cv2,numpy as np
SP=r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad'
fs=sorted(glob.glob(SP+'/gator40/z2/f_*.png'))[11:21]
A=np.mean([cv2.resize(cv2.imread(f,0),(320,240)).astype(np.float32) for f in fs],0)
for name,box in (('oben',(4,100,20,300)),('links',(0,200,0,170))):
    y0,y1,x0,x1=box
    T=np.log1p(A[y0:y1,x0:x1]).astype(np.float32)
    res=[]
    for f in sorted(glob.glob(SP+'/s40/bg40/ROOM*.ppm'))+sorted(glob.glob(SP+'/bgall/ROOM*.ppm')):
        tag=('D' if 'bg40' in f else 'M')+os.path.basename(f)[4:-4]
        im=cv2.imread(f,0)
        if im is None: continue
        best=(-1,None)
        for sc in (0.8,0.9,1.0,1.1,1.2,1.3,1.4):
            B=np.log1p(cv2.resize(im,(int(320*sc),int(240*sc))).astype(np.float32))
            if B.shape[0]<T.shape[0] or B.shape[1]<T.shape[1]: continue
            r=cv2.matchTemplate(B,T,cv2.TM_CCOEFF_NORMED)
            _,mx,_,loc=cv2.minMaxLoc(r)
            if mx>best[0]: best=(mx,(sc,loc))
        res.append((round(best[0],3),tag,best[1]))
    res.sort(reverse=True)
    print(name,'Top15',res[:15])
    print(name,'Rang D20900..D20910:',[(i,r[1],r[0]) for i,r in enumerate(res) if r[1].startswith('D209')][:5],'von',len(res))
