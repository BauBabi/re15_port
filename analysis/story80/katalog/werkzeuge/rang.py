"""rang.py bild x y S pitch hypothese [positionen]  -> je Position alle Glyphen einsetzen, Rang und Top 8.
Geometrie fest (aus kamm.py). TOPHAT=k Vorfilter. '_' = Leerzelle."""
import sys, os
sys.dont_write_bytecode=True
P=r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/va"
sys.path.insert(0,P)
a=sys.argv[1:]
img,x,y,S,pitch,hyp=a[0],int(a[1]),int(a[2]),int(a[3]),float(a[4]),a[5]
pos=[int(v) for v in a[6].split(',')] if len(a)>6 else [i for i,c in enumerate(hyp) if c!='_']
sys.argv=['x']
import ocr, numpy as np, cv2
G=ocr.G
gray=cv2.cvtColor(cv2.imread(img),cv2.COLOR_BGR2GRAY).astype(np.float32)
M=6
roi=gray[y-M:y+S+M, x-M:x+int(pitch*(len(hyp)-1))+S+M]
if os.environ.get('TOPHAT'):
    k=int(os.environ['TOPHAT']); roi=cv2.morphologyEx(roi,cv2.MORPH_TOPHAT,cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(k,k)))
cache={}
def gl(ch):
    if ch not in cache: cache[ch]=cv2.resize((G[ch]>0).astype(np.float32),(S,S),interpolation=cv2.INTER_LINEAR)
    return cache[ch]
def comb(t):
    w=int(round(pitch*(len(t)-1)+S))+2
    A=np.zeros((S,w),np.float32)
    for i,ch in enumerate(t):
        if ch in '_ ': continue
        g=gl(ch); xx=int(round(i*pitch)); A[:,xx:xx+S]=np.maximum(A[:,xx:xx+S],g)
    return cv2.GaussianBlur(A,(0,0),1.6)
def sc(t): return float(cv2.matchTemplate(roi,comb(t),cv2.TM_CCOEFF_NORMED).max())
pool=[c for c in G if len(c)==1 and c!='?']
print('hyp',hyp,round(sc(hyp),3))
for p in pos:
    res=sorted(((sc(hyp[:p]+c+hyp[p+1:]),c) for c in pool+['_']),reverse=True)
    rk=[c for v,c in res]
    print(f'pos {p} soll {hyp[p]}: Rang {rk.index(hyp[p])+1}/{len(rk)}  top: '+' '.join(f'{c}={v:.3f}' for v,c in res[:8]))
