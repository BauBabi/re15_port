"""Sucht die vier ANNETTE_SE-Wellen im Port-Mitschnitt (RE15_AUDIO_CAP, s16 stereo 44,1 kHz). Nur lesen.
   python ton_korr.py <vbs> <ton.raw> <von_s> <bis_s>"""
import sys
import numpy as np

vbs = open(sys.argv[1], 'rb').read()
vb = vbs[3128:]
F = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]


def dekodiere(blob):
    out, s1, s2, wellen, akt = [], 0.0, 0.0, [], []
    for i in range(0, len(blob) - 15, 16):
        b = blob[i:i + 16]
        sh, fi, fl = b[0] & 0x0F, (b[0] >> 4) & 0x0F, b[1]
        if fi > 4:
            fi = 0
        for k in range(14):
            for nib in (b[2 + k] & 0x0F, b[2 + k] >> 4):
                v = nib - 16 if nib >= 8 else nib
                v = (v << 12) >> sh
                v = v + (s1 * F[fi][0] + s2 * F[fi][1]) / 64.0
                v = max(-32768, min(32767, v))
                s2, s1 = s1, v
                akt.append(v)
        if fl & 1:
            if len(akt) > 100:
                wellen.append(np.array(akt, dtype=np.float32))
            akt, s1, s2 = [], 0.0, 0.0
    if len(akt) > 100:
        wellen.append(np.array(akt, dtype=np.float32))
    return wellen


wellen = dekodiere(vb)
print('Wellen:', [len(w) for w in wellen])
raten = {22904: 16397.5, 16716: 11025.0, 15848: 4866.5, 22260: 6137.0}
a = np.fromfile(sys.argv[2], dtype=np.int16).reshape(-1, 2).astype(np.float32).mean(axis=1)
von, bis = float(sys.argv[3]), float(sys.argv[4])
seg = a[int(von * 44100):int(bis * 44100)]
for w in wellen:
    r = None
    for n, hz in raten.items():
        if abs(len(w) - n) < 300:
            r = hz
    if r is None:
        print('Welle', len(w), 'ohne Rate'); continue
    t = np.arange(0, len(w) / r, 1 / 44100.0)
    x = np.interp(t * r, np.arange(len(w)), w)
    x = (x - x.mean()) / (x.std() + 1e-9)
    # normierte Kreuzkorrelation per FFT
    n = len(seg) + len(x)
    N = 1 << (n - 1).bit_length()
    c = np.fft.irfft(np.fft.rfft(seg, N) * np.conj(np.fft.rfft(x, N)), N)[:len(seg) - len(x)]
    cs = np.concatenate([[0.0], np.cumsum(seg.astype(np.float64) ** 2)])
    e = np.sqrt(np.maximum(cs[len(x):] - cs[:-len(x)], 0))[:len(c)] + 1e-9
    rr = c / e / np.sqrt(len(x))
    k = np.argsort(rr)[::-1]
    best = k[0]
    zweit = [i for i in k if abs(i - best) > 44100][0]
    print('Welle %5d (%.1f Hz, %.2f s): Einsatz %.3f s, r = %.3f (zweitbeste %.3f @ %.2f s)' % (
        len(w), r, len(w) / r, von + best / 44100.0, rr[best], rr[zweit], von + zweit / 44100.0))
