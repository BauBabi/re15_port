"""tonbeleg.py name t0 t1 out.png 'UT:a-b:text;...' 'SP:a-b:text;...' -> Spektrogramm 0-4 kHz mit Untertitel- und Sprachbalken"""
import numpy as np, sys, cv2
from scipy.io import wavfile
from scipy.signal import stft
from PIL import Image, ImageDraw, ImageFont
W=sys.argv[1]; name=sys.argv[2]; t0=float(sys.argv[3]); t1=float(sys.argv[4]); out=sys.argv[5]
def parse(s): return [(float(p.split(':')[0].split('-')[0]),float(p.split(':')[0].split('-')[1]),p.split(':',1)[1]) for p in s.split(';') if p]
UT=parse(sys.argv[6]); SP=parse(sys.argv[7])
sr,x=wavfile.read(f'{W}/ton/{name}.wav'); x=x.astype(np.float32)/32768; x=x[int(t0*sr):int(t1*sr)]
f,t,Z=stft(x,sr,nperseg=1024,noverlap=1024-80); A=20*np.log10(np.abs(Z)+1e-6); A=A[f<=4000]
lo,hi=np.percentile(A,5),np.percentile(A,99.7); img=(np.clip((A-lo)/(hi-lo),0,1)[::-1]*255).astype(np.uint8)
pxs=110; w=int((t1-t0)*pxs); H=300
img=cv2.applyColorMap(cv2.resize(img,(w,H),interpolation=cv2.INTER_AREA),cv2.COLORMAP_INFERNO)
L=60; top=110
canvas=np.full((top+H+40,w+L+10,3),255,np.uint8); canvas[top:top+H,L:L+w]=img
im=Image.fromarray(cv2.cvtColor(canvas,cv2.COLOR_BGR2RGB)); d=ImageDraw.Draw(im)
fj=ImageFont.truetype(r'C:\Windows\Fonts\msgothic.ttc',14); fs=ImageFont.truetype(r'C:\Windows\Fonts\arial.ttf',12)
X=lambda s: L+int((s-t0)*pxs)
d.text((2,8),'Untertitel',font=fs,fill=(0,0,0)); d.text((2,58),'Sprache',font=fs,fill=(0,0,0))
for a,b,tx in UT:
    d.rectangle([X(a),5,X(b),45],fill=(150,230,150),outline=(0,0,0)); d.text((X(a)+3,8),tx,font=fj,fill=(0,0,0))
for a,b,tx in SP:
    d.rectangle([X(a),55,X(b),100],fill=(150,200,255),outline=(0,0,0)); d.text((X(a)+3,58),tx,font=fs,fill=(0,0,0))
for s in range(int(np.ceil(t0)),int(t1)+1):
    d.line([X(s),top+H,X(s),top+H+6],fill=(0,0,0)); d.text((X(s)-8,top+H+8),f'{s}s',font=fs,fill=(0,0,0))
for k in (1,2,3,4):
    y=top+H-int(k/4*H); d.text((20,y),f'{k} kHz',font=fs,fill=(0,0,0))
im.save(out)
