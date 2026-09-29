# Pruefer 1 (Runde 31, Tueren): Messwerkzeug, kein Spielcode. Dossier analysis/befunde_runde31/tueren_04_pruefer1.md
# Schliesston im Echtlauf: Referenz (2 Sequenzen hintereinander) gegen den Echtlauf ausrichten und je Tick
# pruefen, ob der Schliesston im Echtlauf weiterklingt (Korrelation + Restenergie nach Abzug).
import numpy as np, sys
ref=np.fromfile(sys.argv[1],dtype=np.int16).reshape(-1,2).astype(np.float64)
real=np.fromfile(sys.argv[2],dtype=np.int16).reshape(-1,2).astype(np.float64)
T=1470
oa,ob=int(sys.argv[3]),int(sys.argv[4])      # Oeffnungston-Ticks in ref
ca,cb=int(sys.argv[5]),int(sys.argv[6])      # Schliesston-Ticks in ref
x=ref[oa*T:ob*T,0]; y=real[:,0]
# Kreuzkorrelation per FFT
n=1
while n<len(x)+len(y): n*=2
X=np.fft.rfft(x[::-1],n); Y=np.fft.rfft(y,n)
c=np.fft.irfft(X*Y,n)[len(x)-1:len(x)-1+len(y)-len(x)+1]
off=int(np.argmax(c))  # Echt-Sample-Offset von ref tick oa
d=off-oa*T
print('Versatz ref->echt Samples',d,'(Ticks %.2f)'%(d/T))
for t in range(ca-2,cb+4):
    r=ref[t*T:(t+1)*T]; e=real[t*T+d:(t+1)*T+d]
    if len(e)<T: break
    rr=np.sqrt((r**2).mean()); ee=np.sqrt((e**2).mean())
    res=np.sqrt(((e-r)**2).mean())
    cc=np.corrcoef(r[:,0],e[:,0])[0,1] if rr>1 else 0
    print('ref-tick %d: ref %5d echt %5d rest(echt-ref) %5d korr %.2f'%(t,rr,ee,res,cc))
