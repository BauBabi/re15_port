# Leser B v3: H je Bild = Oberkante Buckel bis Sohle, KI-Video 4 _slp (1280x960).
# Oberkante: je Spalte x in [xa,xb] die erste Zeile ab y 300, ab der dE (CIE-Lab-Abstand zum Hintergrund-Modus
#   bg_modus_ki.png) in 3 Folgezeilen > 12 ist; Spaltenfolge mit Median 9 geglaettet; OK = Minimum.
# Sohle: unterste Zeile mit >= 3 Pixeln Schuhrot (Lab a* > 18, L* > 12) in y 780-900, x in [xa2,xb2].
# Aufruf: python b_hoehe.py <sfv3-dir> n:xa:xb:xa2:xb2 ...
import sys, cv2, numpy as np
from scipy.ndimage import median_filter
S=sys.argv[1]
def lab(i): return cv2.cvtColor(i.astype(np.float32)/255,cv2.COLOR_BGR2Lab)
bg=lab(cv2.imread(f"{S}/tmp/bg_modus_ki.png"))
for arg in sys.argv[2:]:
    n,xa,xb,xa2,xb2=map(int,arg.split(":"))
    f=lab(cv2.imread(f"{S}/ki/n{n}.png"))
    d=np.sqrt(((f-bg)**2).sum(2))
    b=(d[300:700,xa:xb]>12)
    b3=b[:-2]&b[1:-1]&b[2:]
    top=np.where(b3.any(0),b3.argmax(0),999)+300
    tops=median_filter(top,9)
    i=int(np.argmin(tops)); ok=int(tops[i]); okx=xa+i
    sh=((f[780:900,xa2:xb2,1]>18)&(f[780:900,xa2:xb2,0]>12)).sum(1)
    r=np.where(sh>=3)[0]; so=780+int(r[-1]) if len(r) else -1
    print(f"n{n} h{n-1350}: OK y {ok} bei x {okx}; Sohle y {so}; H = {so-ok if so>0 else 'n/a'} px")
