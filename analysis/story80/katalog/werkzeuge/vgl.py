"""vgl.py bild x y S pitch out.png hyp1 hyp2 ... -> Ausschnitt (4x) uebereinander mit gerenderten Hypothesen im Disc-Font"""
import sys, os
sys.dont_write_bytecode=True
P=r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40/pruefung/va"
sys.path.insert(0,P)
a=sys.argv[1:]
img,x,y,S,pitch,out=a[0],int(a[1]),int(a[2]),int(a[3]),float(a[4]),a[5]; hyps=a[6:]
sys.argv=['x']
import ocr, numpy as np, cv2
G=ocr.G
im=cv2.imread(img)
n=max(len(h) for h in hyps)
w=int(pitch*(n-1)+S)+16
crop=im[y-8:y+S+8, x-8:x-8+w]
Z=4
rows=[cv2.resize(crop,None,fx=Z,fy=Z,interpolation=cv2.INTER_CUBIC)]
for h in hyps:
    A=np.zeros((S+16,w),np.float32)
    for i,ch in enumerate(h):
        if ch in '_ ' or ch not in G: continue
        g=cv2.resize((G[ch]>0).astype(np.float32),(S,S),interpolation=cv2.INTER_NEAREST)
        xx=8+int(round(i*pitch)); A[8:8+S,xx:xx+S]=np.maximum(A[8:8+S,xx:xx+S],g)
    A=cv2.GaussianBlur(A,(0,0),1.0)
    r=cv2.cvtColor((A*255).astype(np.uint8),cv2.COLOR_GRAY2BGR)
    rows.append(cv2.resize(r,None,fx=Z,fy=Z,interpolation=cv2.INTER_CUBIC))
cv2.imwrite(out,np.vstack(rows))
