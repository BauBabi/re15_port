import sys; sys.path.insert(0,r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8/py')
from gem import *
import cv2
def gitter(img, box, k, pfad, gamma=1.0, schritt=10, extra=None):
    x0,y0,x1,y1=box
    a=np.clip(img[y0:y1,x0:x1],0,255)/255.0
    a=(a**gamma*255).astype(np.uint8)
    a=cv2.resize(a,((x1-x0)*k,(y1-y0)*k),interpolation=cv2.INTER_NEAREST)
    for x in range((x0//schritt+1)*schritt,x1,schritt):
        c=(255,255,0) if x%50==0 else (110,110,0)
        cv2.line(a,((x-x0)*k,0),((x-x0)*k,a.shape[0]),c,1)
        if x%50==0: cv2.putText(a,str(x),((x-x0)*k+2,12),cv2.FONT_HERSHEY_SIMPLEX,0.4,(255,255,0),1)
    for y in range((y0//schritt+1)*schritt,y1,schritt):
        c=(255,255,0) if y%50==0 else (110,110,0)
        cv2.line(a,(0,(y-y0)*k),(a.shape[1],(y-y0)*k),c,1)
        if y%50==0: cv2.putText(a,str(y),(2,(y-y0)*k-2),cv2.FONT_HERSHEY_SIMPLEX,0.4,(255,255,0),1)
    if extra:
        for pts,col in extra:
            p=np.array([[(x-x0+0.5)*k,(y-y0+0.5)*k] for x,y in pts],np.int32)
            cv2.polylines(a,[p],False,col,1)
    Image.fromarray(a).save(pfad)
if __name__=='__main__':
    V=vorlage()
    gitter(V,(250,150,480,345),4,S8+'/g_vorlage_x4.png',gamma=0.6)
