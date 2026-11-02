"""vglbg.py out.jpg 'Q:i:k:lab|B1|B2|B3' ... Zeile: Filmbild (cd/f<i>/<k>.jpg) oder Pfad, dann Hintergruende D.../M..."""
import cv2,numpy as np,sys,os
W=r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/st80"
S=r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/s40"
def bg(n):
    d=S+'/bg40' if n[0]=='D' else S+'/pruefung/va/bgmod'
    im=cv2.imread(f'{d}/ROOM{n[1:]}.ppm')
    if im is None: im=np.zeros((240,320,3),np.uint8)
    im=cv2.resize(im,(320,240)); cv2.putText(im,n,(4,16),0,0.5,(0,255,255),1); return im
def q(s):
    if s.startswith('Q:'):
        _,i,k,lab=s.split(':',3); im=cv2.imread(f'{W}/cd/f{i}/{int(k):04d}.jpg')
    else:
        p,_,lab=s.partition('@'); im=cv2.imread(p)
    im=cv2.resize(im,(320,240)); cv2.putText(im,lab,(4,16),0,0.5,(0,255,0),1); return im
rows=[]
for r in sys.argv[2:]:
    parts=r.split('|'); row=[q(parts[0])]+[bg(n) for n in parts[1:]]
    while len(row)<4: row.append(np.zeros((240,320,3),np.uint8))
    rows.append(np.hstack(row[:4]))
cv2.imwrite(sys.argv[1],np.vstack(rows))
