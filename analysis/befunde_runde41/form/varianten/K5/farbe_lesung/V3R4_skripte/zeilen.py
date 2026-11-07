"""Vorlauf fuer mess.py: CLUT-Zeile je Pixel der Variante V3 r3 in Bild 1/2 (zeile1.npy, zeile2.npy) und Regionskarten (reg1.npy, reg2.npy)."""
from basis import *
k = P.Kandidat(V3DIR + '/form.json')
for n in (1, 2):
    np.save('zeile%d.npy' % n, zeilen_karte(k, n))
    np.save('reg%d.npy' % n, regionen_szene(n, k.mo)[0])
