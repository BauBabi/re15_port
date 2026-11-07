import numpy as np, glob, os, warnings
warnings.filterwarnings("ignore")
from scipy.io import wavfile
from scipy.signal import resample_poly
V = "C:/workspace/git/reAi_v2/info/re2leon/PL0/VOICE/"
for ch in (9, 12):
    r, full = wavfile.read(V + "done/VOICE16.XAS[%d].wav" % ch); full = full.astype(np.float64)
    print("== Kanal %d: %d Samples = %.3f s = %d Sektoren je 4032" % (ch, len(full), len(full)/r, len(full)//4032))
    out = []
    for s in sorted(glob.glob(V + "*/VOICE16.XAS[[]%d[]]_*.wav" % ch)):
        rr, x = wavfile.read(s); x = x.astype(np.float64)
        if x.ndim > 1: x = x.mean(1)
        x = resample_poly(x, 63, 80)
        n = len(full) + len(x)
        c = np.fft.irfft(np.fft.rfft(full, n) * np.conj(np.fft.rfft(x, n)), n)
        k = int(np.argmax(c[:len(full)]))
        seg = full[k:k+len(x)]
        rho = np.corrcoef(seg, x[:len(seg)])[0,1]
        out.append((k, os.path.relpath(s, V).replace("\\","/"), len(x), rho))
    for k, nm, ln, rho in sorted(out):
        print("  %7d-%7d  %6.3f-%6.3f s  Sektor %5.1f-%5.1f  r=%.3f  %s" % (k, k+ln, k/r, (k+ln)/r, k/4032, (k+ln)/4032, rho, nm))
