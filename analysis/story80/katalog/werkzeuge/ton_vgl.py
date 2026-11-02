import numpy as np, sys
from scipy.io import wavfile
from scipy.signal import fftconvolve
W=sys.argv[1]
def rd(n):
    sr,x=wavfile.read(f'{W}/ton/{n}.wav'); return x.astype(np.float32)/32768
B=rd('B'); sr=16000
def env(x,hop=160):
    n=len(x)//hop; e=np.sqrt((x[:n*hop].reshape(n,hop)**2).mean(1)+1e-9); return np.log(e)
def xcorr_best(a,b,lo,hi):
    # find lag (in env frames) of b inside a, in range lo..hi; return lag, r
    best=(-2,0)
    for lag in range(lo,hi):
        if lag<0 or lag+len(b)>len(a): continue
        s=a[lag:lag+len(b)]
        r=np.corrcoef(s,b)[0,1]
        if r>best[0]: best=(r,lag)
    return best
EB=env(B)
for i,off in ((1,17.1),(2,303.6),(3,197.9),(4,738.1)):
    u=rd(f'u{i}'); s=rd(f's{i}')
    Eu=env(u); Es=env(s)
    # trim 2 s at both ends
    seg=Eu[200:-200]
    r,lag=xcorr_best(EB,seg,int((off+2)*100)-300,int((off+2)*100)+300)
    r2,lag2=xcorr_best(Es,seg,int((6.6+2)*100)-200,int((6.6+2)*100)+200)
    # per-5s windows correlation in B
    loc=[]
    for k in range(0,len(seg)-500,500):
        w=seg[k:k+500]; a=EB[lag+k:lag+k+500]
        loc.append(round(float(np.corrcoef(a,w)[0,1]),2))
    print(f'CD{i}: in B lag {lag/100-2:.2f}s (B={off}) r_env={r:.3f} | in 360p lag {lag2/100-2:.2f}s r_env={r2:.3f} | 5s-Fenster gegen B: {loc}')
