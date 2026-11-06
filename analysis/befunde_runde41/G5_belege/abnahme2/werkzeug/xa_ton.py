# Abnahme 2: Xa-Einsatz im Port-Mitschnitt (RE15_AUDIO_CAP_SYNC, s16le stereo 44100, 1470 Stereo-Frames je Takt) per Kreuzkorrelation
# mit den RE2-Strecken R704_XA0B/0C.XAS (CD-XA ADPCM nach psx-spx, 4 Bit mono 37800 Hz; Dekoder wie nb1/werkzeug/xa_mono.py)
import numpy as np, sys
K0 = [0, 60, 115, 98]; K1 = [0, 0, -52, -55]
def dec(path):
    b = open(path, "rb").read(); n = len(b) // 2336; hist = [0, 0]; out = []
    for i in range(n):
        sec = b[i*2336:(i+1)*2336]
        if not (sec[2] & 0x04): continue
        data = sec[8:8+0x900]
        for g in range(18):
            grp = data[g*128:(g+1)*128]
            for u in range(8):
                p = grp[4+u]; sh = p & 15; f = (p >> 4) & 3
                if sh > 12: sh = 9
                for j in range(28):
                    byte = grp[16+j*4+(u >> 1)]; nib = (byte >> 4) if (u & 1) else (byte & 15)
                    t = nib - 16 if nib >= 8 else nib
                    s = ((t << 12) >> sh) + ((hist[0]*K0[f] + hist[1]*K1[f] + 32) >> 6)
                    s = max(-32768, min(32767, s)); hist[1] = hist[0]; hist[0] = s; out.append(s)
    return np.array(out, float)
A = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/re15_port/shared_assets/RE2/"
ton = np.fromfile(sys.argv[1], dtype=np.int16).reshape(-1, 2).astype(float).mean(axis=1)
print("Ton-Takte", len(ton) / 1470.0)
res = {}
for nm in ("R704_XA0B.XAS", "R704_XA0C.XAS"):
    x = dec(A + nm)
    t = np.arange(int(len(x) * 44100 / 37800)) * 37800 / 44100.0
    xr = np.interp(t, np.arange(len(x)), x)[:44100 * 2]
    # grobe Suche ueber den ganzen Mitschnitt per FFT-Korrelation
    nfft = 1 << int(np.ceil(np.log2(len(ton) + len(xr))))
    c = np.fft.irfft(np.fft.rfft(ton, nfft) * np.conj(np.fft.rfft(xr, nfft)), nfft)[:len(ton) - len(xr)]
    k = int(np.argmax(np.abs(c)))
    norm = np.linalg.norm(xr) * np.linalg.norm(ton[k:k+len(xr)]) + 1e-9
    res[nm] = k / 1470.0
    print(nm, "Dauer %.2f s" % (len(x) / 37800.0), "Einsatz bei Ton-Takt %.2f, Korrelation %.3f" % (k / 1470.0, abs(c[k]) / norm))
print("Abstand 0C - 0B: %.2f Takte (Soll 438 - 132 = 306)" % (res["R704_XA0C.XAS"] - res["R704_XA0B.XAS"]))
