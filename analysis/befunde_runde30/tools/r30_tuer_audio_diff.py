#!/usr/bin/env python3
# r30_tuer_audio_diff.py <mit_druck.raw> <ohne_druck.raw> <erwartet.wav>
# Zwei RE15_AUDIO_CAP_SYNC-Aufnahmen desselben Echtlaufs (r30_tuer_echtlauf.sh) mit und ohne
# QUADRAT-Druck: wo unterscheiden sie sich, und ist die Differenz die erwartete Tuer-Welle?
"""Differenz zweier RE15_AUDIO_CAP_SYNC-Aufnahmen (mit / ohne QUADRAT) gegen die erwartete Welle."""
import sys, wave
import numpy as np
a = np.fromfile(sys.argv[1], dtype='<i2').reshape(-1, 2).astype(np.int64)
b = np.fromfile(sys.argv[2], dtype='<i2').reshape(-1, 2).astype(np.int64)
n = min(len(a), len(b)); d = a[:n] - b[:n]
T = 1470
nz = np.nonzero(d.any(axis=1))[0]
if len(nz) == 0:
    print("KEIN Unterschied"); sys.exit(0)
s, e = int(nz[0]), int(nz[-1])
print("Unterschied: Stereo-Frame %d (Tick %d) .. %d (Tick %d) = %.3f s; L==R: %s"
      % (s, s // T, e, e // T, (e - s + 1) / 44100.0, bool((d[s:e+1, 0] == d[s:e+1, 1]).all())))
w = wave.open(sys.argv[3]); rate = w.getframerate()
src = np.frombuffer(w.readframes(w.getnframes()), dtype='<i2').astype(np.float64)
m = int(len(src) * 44100 / rate)
idx = np.minimum((np.arange(m) * rate / 44100).astype(np.int64), len(src) - 1)
exp = src[idx]
L = d[:, 0].astype(np.float64)
best = (-2.0, 0)
for st in range(max(0, s - 3000), s + 200):
    seg = L[st:st + m]
    if len(seg) < m: break
    den = np.sqrt((seg * seg).sum() * (exp * exp).sum())
    r = float((seg * exp).sum() / den) if den else 0.0
    if r > best[0]: best = (r, st)
st = best[1]
seg = L[st:st + m]
g = float(np.sqrt((seg * seg).sum() / (exp * exp).sum()))
print("erwartet %d Stereo-Frames = %.3f s (Quelle %d Hz, %d Samples)" % (m, m / 44100.0, rate, len(src)))
print("beste Korrelation r = %.4f, Welle beginnt bei Stereo-Frame %d (Tick %d, %+d gegen den ersten Unterschied), Pegel %.4f"
      % (best[0], st, st // T, st - s, g))
print("Rest ausserhalb [Beginn, Beginn+Laenge): %d Stereo-Frames ungleich 0"
      % int(np.count_nonzero(np.concatenate([d[:st].any(axis=1), d[st + m:].any(axis=1)]))))
