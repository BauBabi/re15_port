import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import cv2, json
from m12_tafel import linien
ns=list(range(1431,1452,2))
box=(240,160,480,360); k=2
tiles=[]
for n in ns:
    a=np.clip(frame(n)/255.0,0,1)**0.6*255
    t=cv2.resize(a[box[1]:box[3],box[0]:box[2]].astype(np.uint8),None,fx=k,fy=k,interpolation=cv2.INTER_NEAREST)
    linien(t,k,box[0],box[1],1)
    cv2.putText(t,'n%d'%n,(5,18),cv2.FONT_HERSHEY_SIMPLEX,0.6,(255,255,0),2)
    tiles.append(t)
rows=[np.hstack(tiles[i:i+4]) for i in range(0,len(tiles),4)]
w=max(r.shape[1] for r in rows); rows=[np.hstack([r,np.zeros((r.shape[0],w-r.shape[1],3),np.uint8)]) for r in rows]
Image.fromarray(np.vstack(rows)).save(S8+'/tafeln/03_nachbarbilder_n1431-1451.png')
# Bewegung: mittlere Abweichung im Schwanzband gegen n1439
V=frame(1439)
for n in range(1427,1456):
    d=np.abs(frame(n)-V)
    print(n,'Schwanzband %.1f'%d[220:340,262:330].mean(),'Ruecken %.1f'%d[175:215,330:450].mean())
