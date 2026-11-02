"""kamm.py bild x0 x1 y0 y1 kandidat1 kandidat2 ...  (Spielfont der 40%-Disc, DATA/TEX.TIM)
Kamm-Korrelation je Kandidat ueber Teilung 26-31 und Zellhoehe 24-30, dazu Nullmodell aus 300 Zufallszeichen
gleicher Laenge (Leerzeichen und feste Satzzeichen bleiben an ihrem Platz). '_' = Leerzelle."""
import sys, os, random
sys.dont_write_bytecode = True
P=r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/va"
sys.path.insert(0,P)
args=sys.argv[1:]
img,x0,x1,y0,y1=args[0],*map(int,args[1:5])
cands=args[5:]
sys.argv=['x']
import ocr
import numpy as np, cv2
G=ocr.G
gray=cv2.cvtColor(cv2.imread(img),cv2.COLOR_BGR2GRAY).astype(np.float32)
roi=gray[y0:y1,x0:x1]
if os.environ.get('TOPHAT'):
    k=int(os.environ['TOPHAT']); roi=cv2.morphologyEx(roi,cv2.MORPH_TOPHAT,cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(k,k)))
    cv2.imwrite(img+'.roi.png',np.clip(roi*3,0,255).astype(np.uint8))
cache={}
def gl(ch,S):
    k=(ch,S)
    if k not in cache:
        cache[k]=cv2.resize((G[ch]>0).astype(np.float32),(S,S),interpolation=cv2.INTER_LINEAR)
    return cache[k]
def comb(t,S,pitch):
    w=int(round(pitch*(len(t)-1)+S))+2
    a=np.zeros((S,w),np.float32)
    for i,ch in enumerate(t):
        if ch in ' _　': continue
        g=gl(ch,S); xx=int(round(i*pitch)); a[:,xx:xx+S]=np.maximum(a[:,xx:xx+S],g)
    return cv2.GaussianBlur(a,(0,0),1.6)
def sc(t,grid):
    best=(-1,None)
    for S in grid[0]:
        for p in grid[1]:
            c=comb(t,S,p)
            if c.shape[1]>=roi.shape[1] or c.shape[0]>=roi.shape[0]: continue
            r=cv2.matchTemplate(roi,c,cv2.TM_CCOEFF_NORMED)
            v=float(r.max())
            if v>best[0]:
                loc=np.unravel_index(np.argmax(r),r.shape); best=(v,(S,p,x0+loc[1],y0+loc[0]))
    return best
grid=(range(20,33,2),[22.0,24.0,25.0,26.0,27.0,28.0,29.0,30.0])
res=[(sc(c,grid),c) for c in cands]
# best geometry from first candidate
(v0,(S,p,xx,yy)),c0=max(res,key=lambda r:r[0][0])
print(f'geometrie aus bestem Kandidaten: S={S} pitch={p} x={xx} y={yy}')
for (v,g),c in sorted(res,key=lambda r:-r[0][0]):
    print(f'  {v:.3f}  {c}  {g}')
pool=[ch for ch in G if len(ch)==1 and ch not in '?']
random.seed(1)
fixed=set(' _　、。！？…ー')
for c in cands[:1]:
    null=[]
    for k in range(300):
        t=''.join(ch if ch in fixed else random.choice(pool) for ch in c)
        null.append(sc(t,([S],[p]))[0])
    null.sort()
    print(f'Nullmodell zu Laenge/Muster von "{c}": max {null[-1]:.3f}, 99% {null[int(0.99*len(null))-1]:.3f}, median {np.median(null):.3f}')
