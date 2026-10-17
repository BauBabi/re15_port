#!/usr/bin/env python3
"""Mitschnitt eines integration_r38_elzatoene_szene1021-Laufs: an den Schuss-Bildern aus elza.log (Aktion 2 der
[elza]-Anker @0x020CE/@0x020D6) gegen Vorlagen korrelieren: RE1.5 Super-Redhawk ARMS07 SE 0, Elzas Browning ARMS03 SE 0,
alter Satz RE2 40A0/08, neuer Satz RE2 4040/0A. Aufruf: kap_kreuz2.py <laufordner> <exit-raumbild>"""
import sys, os, re
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from wellen import *
from klangbild import resample_to_44k
import numpy as np
def zoh(w): return np.array(resample_to_44k(w[0].astype(int).tolist(), w[1]), float)
lauf, exitb = sys.argv[1], int(sys.argv[2])
bilder = []
for z in open(os.path.join(lauf, 'elza.log'), encoding='utf-8', errors='replace'):
    m = re.match(r'F(\d+) \[elza\] ROOM1021 Anker @0x020(CE|D6) -> Aktion 2', z)
    if m: bilder.append(int(m.group(1)))
V = {'Redhawk ARMS07 SE0': zoh(welle_arms(SND+'ARMS07.EDH', SND+'ARMS07.VB', 0)),
     'Browning ARMS03 SE0': zoh(welle_arms(SND+'ARMS03.EDH', SND+'ARMS03.VB', 0)),
     'alt RE2 40A0/08': zoh(welle_re2(R2+'ROOM40A0.RDT', 0x08)),
     'neu RE2 4040/0A': zoh(welle_re2(R2+'ROOM4040.RDT', 0x0A))}
d = np.fromfile(os.path.join(lauf, 'cap.raw'), dtype='<i2').astype(float).reshape(-1, 2).mean(axis=1)
T = 1470; n = len(d)//T; off = n - 1 - exitb
# elza.log zaehlt Spielbilder ab Programmstart (F...), der Mitschnitt Raum-Bilder bis exit: die Integration gibt die
# Raum-Bilder; hier dieselbe Zuordnung wie probe_r38_elzatoene_kreuz (Raum-Bild f bei Takt off + f).
for bild in bilder:
    s0 = (off + bild - 2) * T
    x = d[s0: s0 + 8*T + 13230]
    out = []
    for k, t in V.items():
        t = t[:13230]; st = (t*t).sum(); best = -2
        for lag in range(0, 8*T, 3):
            seg = x[lag:lag+len(t)]
            if len(seg) < len(t): break
            c = (seg*t).sum()/np.sqrt((seg*seg).sum()*st)
            if c > best: best = c
        out.append('%s %.3f' % (k, best))
    print('Bild %d: %s' % (bild, ' | '.join(out)))
