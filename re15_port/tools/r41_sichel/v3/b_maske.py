# Leser B v3: Figurmaske fuer KI-Bilder aus Video 4 _slp (Seiten- und Vorderansicht).
# dE = CIE-Lab-Abstand (D65, OpenCV float Lab) zum Hintergrund-Modus bg_modus_ki.png (b_hintergrund.py),
# 3x3-Gauss, Schwelle SCHWELLE, Oeffnen 3x3, Schliessen 7x7, groesste Komponente, Loecher gefuellt.
# Aufruf: python b_maske.py <sfv3-dir> <schwelle> n [n ...] -> sfv3/masken/m<n>.png, sfv3/tmp/mo<n>.png
import sys, os, cv2, numpy as np
S=sys.argv[1]; SCHWELLE=float(sys.argv[2])
def lab(i): return cv2.cvtColor(i.astype(np.float32)/255,cv2.COLOR_BGR2Lab)
bg=lab(cv2.imread(f"{S}/tmp/bg_modus_ki.png"))
os.makedirs(f"{S}/masken",exist_ok=True)
for n in map(int,sys.argv[3:]):
    raw=cv2.imread(f"{S}/ki/n{n}.png"); f=lab(raw)
    d=cv2.GaussianBlur(np.sqrt(((f-bg)**2).sum(2)),(5,5),0)
    m=(d>SCHWELLE).astype(np.uint8)*255
    m[:120]=0; m[880:]=0
    m=cv2.morphologyEx(m,cv2.MORPH_OPEN,np.ones((3,3),np.uint8))
    m=cv2.morphologyEx(m,cv2.MORPH_CLOSE,np.ones((7,7),np.uint8))
    k,l,st,_=cv2.connectedComponentsWithStats(m); i=1+np.argmax(st[1:,4])
    m=np.where(l==i,255,0).astype(np.uint8)
    cs,_=cv2.findContours(m,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)
    m=np.zeros_like(m); cv2.drawContours(m,cs,-1,255,-1)
    cv2.imwrite(f"{S}/masken/m{n}.png",m)
    o=raw.copy(); cv2.drawContours(o,cs,-1,(0,255,0),1)
    cv2.imwrite(f"{S}/tmp/mo{n}.png",o[320:900,440:1040])
