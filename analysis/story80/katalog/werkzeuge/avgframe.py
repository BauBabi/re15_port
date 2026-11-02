"""avgframe.py video t0 t1 out.png  -> mittelt alle Bilder im Intervall (Rauschminderung)"""
import cv2, numpy as np, sys
V,t0,t1,out=sys.argv[1],float(sys.argv[2]),float(sys.argv[3]),sys.argv[4]
cap=cv2.VideoCapture(V); fps=cap.get(cv2.CAP_PROP_FPS)
cap.set(cv2.CAP_PROP_POS_FRAMES,int(round(t0*fps))); acc=None; n=0
for k in range(max(1,int(round((t1-t0)*fps)))):
    ok,f=cap.read()
    if not ok: break
    f=f.astype(np.float32); acc=f if acc is None else acc+f; n+=1
cv2.imwrite(out,(acc/n).astype(np.uint8)); print(out,n,'frames')
