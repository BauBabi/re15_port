#!/usr/bin/env python3
"""Riegel MESSBLATT_FARBE_V1 Abschnitt 6, alle 24 Zeilen (A1-A12 Atlas, K1-K10 Kette, N1-N2 neutraler Render gegen EM30) an einer Variante.
Atlas und Kette: Pruefer-Skript farbe_lesung/P_skripte/p_riegel.py (atlas_riegel, kette_riegel; dieselbe Definition, unveraendert).
N1/N2: Leser-B-Werkzeug farbe_lesung/B_skripte/render.py (form_buehne.rastern, Licht 0,55 + 0,45 max(0, N.L), weiss, Umkugel 0,46, 3 Winkel, roh,
EM30 Clip 0 Bild 0, Variante Clip 2 Ruhe Bild 0, Kachel 380 px wie lauf_render.py). Grenzen N1 >= 0,75 x EM30 (live gerechnet), N2 <= 0,10 %.

    python riegel_farbe.py <Variantenordner> [--gegenprobe <Ordner>] [--json <aus.json>]

Exit 1, wenn ein Riegel der Variante ROT ist, oder wenn die Gegenprobe (z. B. K5c heute) NICHT rot ist (dann misst der Riegel nichts)."""
import argparse
import json
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
FL = os.path.normpath(os.path.join(HIER, '..', 'farbe_lesung'))
sys.path.insert(0, os.path.join(FL, 'P_skripte'))
sys.path.insert(0, os.path.join(FL, 'B_skripte'))
import p_riegel as PR  # noqa: E402
from p_lade import var  # noqa: E402


def n_riegel(vdir):
    import fl
    import render as R
    em = fl.lade_em30()
    mo = fl.lade_var(vdir)
    out = {}
    c_em, c_v, h_v = [], [], []
    for wt, az, el in R.WINKEL:
        ie, me = R.blick(em, R.kf(em, 0), az, el, 380, None)
        iv, mv = R.blick(mo, R.kf(mo, 2), az, el, 380, None)
        a, b = R.masse(ie, me), R.masse(iv, mv)
        c_em.append(a['C'])
        c_v.append(b['C'])
        h_v.append(b['hell85'])
    out['N1 neutral C* je Winkel (Variante / EM30)'] = ((tuple(c_v), tuple(c_em)), all(v >= 0.75 * e for v, e in zip(c_v, c_em)))
    out['N2 neutral L*>85 % je Winkel'] = (tuple(h_v), all(h <= 0.10 for h in h_v))
    return out


def messen(vdir):
    res = {}
    for k, (v, ok) in PR.atlas_riegel(var(vdir), os.path.basename(os.path.normpath(vdir))).items():
        res[k] = (v, bool(ok))
    for k, (v, ok) in PR.kette_riegel(vdir).items():
        res[k] = (v, bool(ok))
    for k, (v, ok) in n_riegel(vdir).items():
        res[k] = (v, bool(ok))
    return res


def drucken(name, res):
    print('== %s' % name)
    for k, (v, ok) in res.items():
        print('  %-44s %-70s %s' % (k, v, 'GRUEN' if ok else 'ROT'))
    g = sum(1 for _v, ok in res.values() if ok)
    print('  -> %d/%d GRUEN' % (g, len(res)))
    return g


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('variante')
    a.add_argument('--gegenprobe', default=None)
    a.add_argument('--json', default=None)
    x = a.parse_args()
    r = messen(x.variante)
    g = drucken(x.variante, r)
    exitcode = 0 if g == len(r) else 1
    out = dict(variante={k: dict(wert=str(v), ok=ok) for k, (v, ok) in r.items()})
    if x.gegenprobe:
        rg = messen(x.gegenprobe)
        gg = drucken('Gegenprobe ' + x.gegenprobe, rg)
        out['gegenprobe'] = {k: dict(wert=str(v), ok=ok) for k, (v, ok) in rg.items()}
        # Gegenprobe: jeder Riegel, den die Variante bestehen muss und der eine Farbaussage ist, soll an der Gegenprobe (K5c heute) ROT sein.
        # Ausnahmen laut Messblatt 6 (K5c heute GRUEN): A10 Texeldichte, A11 Schwanz, A12 Farbkorn = Halteriegel.
        halte = ('A10', 'A11', 'A12')
        falsch = [k for k, (_v, ok) in rg.items() if ok and not k.startswith(halte)]
        print('  Gegenprobe: %d/%d GRUEN; Farbriegel, die an der Gegenprobe NICHT rot sind: %s' % (gg, len(rg), falsch or 'keine'))
        out['gegenprobe_nicht_rot'] = falsch
        if falsch:
            exitcode = 1
    if x.json:
        json.dump(out, open(x.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    sys.exit(exitcode)
