"""ton_versatz.py : Versatz Spielbild F -> Tonbild (RE15_AUDIO_CAP_SYNC, 1470 Stereo-Frames je Takt) per Kreuzkorrelation der zwei
RE2-XA-Strecken (Start laut Szene F_arm+132 / F_arm+438) gegen den Mitschnitt."""
import numpy as np, wave, sys
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
F_arm = int(sys.argv[1]) if len(sys.argv) > 1 else 1613
def lade(p):
    w = wave.open(p); n = w.getnframes(); r = w.getframerate(); c = w.getnchannels()
    a = np.frombuffer(w.readframes(n), dtype=np.int16).astype(np.float64)
    if c == 2: a = a.reshape(-1, 2).mean(axis=1)
    return a, r
ton = np.fromfile(SP + "/g5nb1/lauf/ton_leon_sync.raw", dtype=np.int16).reshape(-1, 2).astype(np.float64).mean(axis=1)
for nm, f_erw in (("xa0B_ch4.wav", F_arm + 132), ("xa0C_ch1.wav", F_arm + 438)):
    x, r = lade(SP + "/g5nb1_d/" + nm)
    t = np.arange(int(len(x) * 44100 / r)) * r / 44100.0
    xr = np.interp(t, np.arange(len(x)), x)[:44100]
    lo = (f_erw + 300) * 1470; hi = (f_erw + 600) * 1470 + len(xr)
    seg = ton[lo:hi]
    c = np.correlate(seg, xr, mode="valid")
    k = int(np.argmax(np.abs(c)))
    norm = np.linalg.norm(xr) * np.linalg.norm(seg[k:k + len(xr)]) + 1e-9
    start = (lo + k) / 1470.0
    print(nm, "XA-Beginn im Ton bei Tonbild %.2f, erwartet Spielbild %d -> Versatz %.2f, Korrelation %.3f" % (start, f_erw, start - f_erw, abs(c[k]) / norm))
