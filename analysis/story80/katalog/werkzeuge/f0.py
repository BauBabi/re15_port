"""f0.py name t0:t1:label ... -> Grundfrequenz (Autokorrelation, 70-600 Hz) je Abschnitt: Median der stimmhaften Rahmen"""
import numpy as np, sys
from scipy.io import wavfile
from scipy.signal import butter, sosfiltfilt
W=sys.argv[1]; name=sys.argv[2]
sr,x=wavfile.read(f'{W}/ton/{name}.wav'); x=x.astype(np.float32)/32768
x=sosfiltfilt(butter(4,[60,1200],btype='band',fs=sr,output='sos'),x)
def f0s(a,b):
    seg=x[int(a*sr):int(b*sr)]; N=int(0.04*sr); H=int(0.01*sr); out=[]
    for k in range(0,len(seg)-N,H):
        fr=seg[k:k+N]*np.hanning(N); fr=fr-fr.mean()
        ac=np.correlate(fr,fr,'full')[N-1:]
        if ac[0]<=1e-8: continue
        ac=ac/ac[0]
        lo,hi=int(sr/600),int(sr/70)
        j=lo+int(np.argmax(ac[lo:hi])); 
        if ac[j]>0.6: out.append(sr/j)
    return np.array(out)
for spec in sys.argv[3:]:
    a,b,lab=spec.split(':'); f=f0s(float(a),float(b))
    if len(f): print(f'{lab:40s} {a}-{b}s  stimmhaft {len(f):3d} Rahmen  F0 Median {np.median(f):5.0f} Hz  (25-75%: {np.percentile(f,25):.0f}-{np.percentile(f,75):.0f})')
    else: print(lab,'keine stimmhaften Rahmen')
