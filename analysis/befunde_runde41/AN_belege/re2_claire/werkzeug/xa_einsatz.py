"""xa_einsatz.py VIDEO XADIR NR... - Einsatzzeit jedes Xa-Eintrags in der Tonspur der Aufnahme per Kreuzkorrelation.
XADIR/xa_NN.wav = aus VOICE16.XAS dekodierte Eintraege (Kanal/Block/Laenge aus der Claire-STAGE6-Tabelle 0x8011A4D4,
werkzeug xadec.py). Beide Signale auf 8 kHz mono, Huellkurve nicht noetig: normierte Kreuzkorrelation der Wellenform.
Ausgabe: Videozeit des Einsatzes, Korrelationsspitze (normiert), Laenge des Eintrags."""
import sys, subprocess, numpy as np
from scipy.io import wavfile
from scipy.signal import resample_poly, fftconvolve
FF = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
v, xd = sys.argv[1], sys.argv[2]; nrs = sys.argv[3:]
R = 8000
p = subprocess.run([FF, "-v", "error", "-i", v, "-vn", "-ac", "1", "-ar", str(R), "-f", "s16le", "-"], capture_output=True)
a = np.frombuffer(p.stdout, np.int16).astype(np.float64)
a2 = np.r_[0.0, np.cumsum(a * a)]
for nr in nrs:
    r, x = wavfile.read(xd + "/xa_%s.wav" % nr)
    x = x.astype(np.float64)
    if x.ndim > 1: x = x.mean(axis=1)
    y = resample_poly(x, R, r)
    L = len(y)
    c = fftconvolve(a, y[::-1], mode="valid")
    ey = (y * y).sum(); ew = a2[L:] - a2[:-L]
    # Fenster-Energie nach unten begrenzen (Stille darf keine Scheinspitze erzeugen): Boden = 5 % der groessten Fensterenergie
    en = np.sqrt(np.maximum(ew, 0.05 * ew.max())) * np.sqrt(ey) + 1e-9
    nc = c / en[:len(c)]
    k = int(np.argmax(nc))
    print("Xa %s: Einsatz %8.3f s  Ende %8.3f s  Laenge %5.2f s  r=%.3f" % (nr, k / R, (k + L) / R, L / R, nc[k]))
