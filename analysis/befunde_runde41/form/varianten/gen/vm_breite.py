#!/usr/bin/env python3
"""Varianten, Breitenmass je Teil (E18): Breite quer zur Hauptachse der Nutzermaske, je Station laengs (alle 2 px), Modell (sichtbare Pixel des Teils,
form_pruefung.teilbild, Verfeinerungen mit eigener Zuordnung) gegen Maske; Verhaeltnis je Station, Median je Teil (Ziel 0,95-1,05).
Nur Stationen, an denen die Maske mindestens 4 px breit ist; Logo ausgenommen. python vm_breite.py AUS.json "Name=form.json" ... Kein RE-Wert."""
import json
import sys

import numpy as np

from rf_kern import TOOLS
sys.path.insert(0, TOOLS)
import form_pruefung as P      # noqa: E402


def breiten(maske, achse):
    ys, xs = np.nonzero(maske)
    if len(xs) == 0:
        return {}
    q = np.stack([xs, ys], 1).astype(float) - achse['mitte']
    l = q @ achse['e']
    n = q @ achse['n']
    aus = {}
    for st in range(int(np.floor(l.min() / 2)), int(np.ceil(l.max() / 2)) + 1):
        s = (l >= st * 2 - 1) & (l < st * 2 + 1)
        if s.sum() >= 2:
            aus[st] = float(n[s].max() - n[s].min() + 1)
    return aus


def teil_breite(name, karte, idx):
    M = P.teil_maske(name)
    a = P.achse(M)
    if a is None:
        return None
    V = (karte == idx) & ~P.logo_maske()
    bm, bv = breiten(M, a), breiten(V, a)
    r = [bv.get(k, 0.0) / w for k, w in bm.items() if w >= 4]
    if not r:
        return None
    return dict(verhaeltnis_median=round(float(np.median(r)), 3), verhaeltnis_mittel=round(float(np.mean(r)), 3), stationen=len(r),
                schmal_anteil=round(float(np.mean(np.array(r) < 0.95)), 3))


def messen(k):
    aus = {}
    for n in (1, 2):
        zu, _w = P.teil_zuordnung(k, n)
        tb = P.teilbild(k, n, zu)
        for t in P.teile_von(n):
            if t in tb['namen']:
                aus[t] = teil_breite(t, tb['karte'], tb['namen'].index(t))
        T = k.j.get('teile') or {}
        vf = [t for t in P.teile_von(n, verfeinert=True) if T.get(t)]
        if vf:
            zu2 = {}
            for t in vf:
                for b in T[t]:
                    zu2.setdefault(b, t)
            tb2 = P.teilbild(k, n, zu2, verfeinert=True)
            for t in vf:
                aus[t] = teil_breite(t, tb2['karte'], tb2['namen'].index(t))
    return aus


def main():
    R = {}
    for z in sys.argv[2:]:
        nm, pf = z.split('=', 1)
        R[nm] = messen(P.Kandidat(pf))
    json.dump(R, open(sys.argv[1], 'w', encoding='utf-8'), indent=1)
    namen = sorted({t for v in R.values() for t in v})
    print('%-22s' % 'Teil' + ''.join('%-24s' % nm for nm in R))
    for t in namen:
        print('%-22s' % t + ''.join('%-24s' % ('-' if not R[nm].get(t) else '%.2f (schmal %.0f%%)' % (R[nm][t]['verhaeltnis_median'], 100 * R[nm][t]['schmal_anteil'])) for nm in R))


if __name__ == '__main__':
    main()
