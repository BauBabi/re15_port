"""spek.py name t0 t1 out.png [marks t:label,...] -> Spektrogramm 0-5 kHz, log, mit Zeitachse"""
import numpy as np, sys, cv2
from scipy.io import wavfile
from scipy.signal import stft
W=sys.argv[1]; name=sys.argv[2]; t0=float(sys.argv[3]); t1=float(sys.argv[4]); out=sys.argv[5]
marks=[m.split(':') for m in sys.argv[6].split(',')] if len(sys.argv)>6 and sys.argv[6] else []
sr,x=wavfile.read(f'{W}/ton/{name}.wav'); x=x.astype(np.float32)/32768
x=x[int(t0*sr):int(t1*sr)]
import os
NP=int(os.environ.get('NP','1024')); FM=float(os.environ.get('FM','5000'))
f,t,Z=stft(x,sr,nperseg=NP,noverlap=NP-80)
A=20*np.log10(np.abs(Z)+1e-6)
A=A[f<=FM]
lo,hi=np.percentile(A,5),np.percentile(A,99.7)
img=np.clip((A-lo)/(hi-lo),0,1)[::-1]
img=(img*255).astype(np.uint8)
pxs=int(sys.argv[7]) if len(sys.argv)>7 else 40  # pixel per second
w=int((t1-t0)*pxs); img=cv2.resize(img,(w,320),interpolation=cv2.INTER_AREA)
img=cv2.applyColorMap(img,cv2.COLORMAP_INFERNO)
img=cv2.copyMakeBorder(img,0,40,40,0,cv2.BORDER_CONSTANT,value=(255,255,255))
for s in range(int(np.ceil(t0)),int(t1)+1):
    x0=40+int((s-t0)*pxs)
    cv2.line(img,(x0,320),(x0,326 if s%5 else 334),(0,0,0),1)
    if s%5==0: cv2.putText(img,f'{s//60}:{s%60:02d}',(x0-14,350),0,0.4,(0,0,0),1)
for kHz in [k*0.5 for k in range(1,int(FM/500)+1)]:
    y=int(320-kHz*1000/FM*320); cv2.putText(img,f'{kHz:g}k',(2,y+12),0,0.35,(0,0,0),1); cv2.line(img,(36,y),(40,y),(0,0,0),1)
for tm,lab in marks:
    a,b=[float(v) for v in tm.split('-')]
    xa=40+int((a-t0)*pxs); xb=40+int((b-t0)*pxs)
    cv2.rectangle(img,(xa,2),(xb,14),(0,255,0),-1); cv2.putText(img,lab,(xa+2,12),0,0.35,(0,0,0),1)
cv2.imwrite(out,img)
