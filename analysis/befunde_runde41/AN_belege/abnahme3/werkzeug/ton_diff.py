"""ton_diff.py - Abnahme 3 Spur AN: Pegel je Spielbild (1470 Stereo-Samples, RE15_AUDIO_CAP_SYNC) zweier Laeufe und ihre Differenz.
Aufruf: ton_diff.py <ton_neu.raw> <ton_alt.raw> <offset tick-zeile> <zeile_von_bild_ref> <bild_ref> <von> <bis> [schritt]
Ausrichtung: Tick = state.log-Zeile + offset (Offset aus dem Einsatz 'Zug an' Ton 3 F800). Beide Laeufe sind bis zur ersten
Verhaltensaenderung bitgleich; die Differenz zeigt nur das, was im neuen Lauf zusaetzlich klingt."""
import sys, numpy as np
def load(p):
    a = np.fromfile(p, dtype='<i2').reshape(-1, 2).astype(np.float64)
    n = len(a) // 1470
    return a[:n * 1470].reshape(n, 1470, 2)
A, B = load(sys.argv[1]), load(sys.argv[2])
off, zref, fref, von, bis = map(int, sys.argv[3:8])
st = int(sys.argv[8]) if len(sys.argv) > 8 else 1
r = lambda x: np.sqrt((x ** 2).mean(axis=(1, 2)))
n = min(len(A), len(B))
d = np.abs(A[:n] - B[:n]).max(axis=(1, 2))
print('erste abweichende Tick-Nummer', int(np.argmax(d > 0)), '= Bild', int(np.argmax(d > 0)) - off - zref + fref)
for F in range(von, bis + 1, st):
    t = zref + (F - fref) + off
    if t >= n: break
    print('F%d tick %d neu %5d alt %5d diff %5d' % (F, t, int(r(A[t:t + 1])[0]), int(r(B[t:t + 1])[0]), int(r(A[t:t + 1] - B[t:t + 1])[0])))
