#!/usr/bin/env python3
"""K5g2 Runde 2, O1: Mass fuer die Mulde (Mulden-Bauer). Nur Messung, aendert nichts.

STREIFEN-PIXEL (Orchestrator-Entscheid O1): Bildpunkt (640 x 480, kf_k5g2_geo.keil_raster = Maler nach mittlerer Dreieckstiefe gegen
Tiefe je Bildpunkt), an dem der Maler ein Teil der Kopf-Gruppe {22, 23} zeigt, nach der Tiefe aber die Rumpf-Gruppe vorn liegt (oder
umgekehrt), UND der weiter als TOL = 2 Bildpunkte von der naechsten Stelle liegt, an der die gemalte Gruppe wirklich vorn ist.

Lesarten je Ansicht:
  ab4   nur die Teile 10, 21, 22, 23 gerastert; {22,23} gegen {10,21}                       (Riegel O1 (i))
  ab5   nur die Teile 10, 21, 16, 22, 23 gerastert; {22,23} gegen {10,21,16}                (Riegel O1 (ii))
  alle  ganzes Modell gerastert; je Teilpaar "P>Q" (Maler zeigt P, Q liegt vorn) - ALLE Paare gespeichert (O1 (iii)),
        dazu die Zahl der gezeichneten Dreiecke (O1 (vi)).

Aufrufe:
  python kf_k5g2_mulde_mass.py stich  <EMD> [ansicht:clip:bild ...]        einzelne Ansichten, Tabelle auf stdout
  python kf_k5g2_mulde_mass.py lauf   <aus.json> <prozesse> <schritt_rest> <EMD>
        schritt_rest = 1: ALLE Bilder ALLER Clips; 4: Stichprobe des Orchestrators (Clips 10/12 ganz, sonst jedes 4. Bild)
  python kf_k5g2_mulde_mass.py summe  <lauf.json> [<bezug.json>]            Zahlen zu O1 (i), (ii), (iii), (vi)
"""
import collections
import hashlib
import json
import os
import sys
import time

import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g2_geo as G  # noqa: E402

FB, FE = G.FB, G.FE
K5 = os.path.dirname(HIER)
TIM_K5E = os.path.join(K5, 'ruhe', 'K5e_f1', 'BAHNSTEIG_FORM.TIM')
TOL = 2.0
KOPF = (22, 23)
KOPFNAMEN = ('22', '23')
RUMPF4, RUMPF5 = (10, 21), (10, 21, 16)
ANSICHTEN = ('bild1', 'bild2', 'spiel5080', 'spiel5080_vorn', 'spiel5090')
_W = {}


def mit_kopf(paar):
    a, b = paar.split('>')
    return a in KOPFNAMEN or b in KOPFNAMEN


def _dist(maske_vorn):
    if not maske_vorn.any():
        return np.full(maske_vorn.shape, 1e9, np.float32)
    return cv2.distanceTransform((~maske_vorn).astype(np.uint8), cv2.DIST_L2, cv2.DIST_MASK_PRECISE)


def gruppen(mt, pt, rumpf, maske=False):
    """Streifen-Pixel Kopf-Gruppe gegen `rumpf`. Rueckgabe dict (keil, streif je Richtung, Stuecke, groesstes, tief)."""
    A = np.isin(pt, KOPF); B = np.isin(pt, rumpf)
    a = np.isin(mt, KOPF) & B
    b = np.isin(mt, rumpf) & A
    dA, dB = _dist(A), _dist(B)
    sa = a & (dA > TOL); sb = b & (dB > TOL)
    s = sa | sb
    n, _lab, st, _c = cv2.connectedComponentsWithStats(s.astype(np.uint8), connectivity=8)
    gr = int(st[1:, cv2.CC_STAT_AREA].max()) if n > 1 else 0
    tief = float(max(dA[a].max() if a.any() else 0, dB[b].max() if b.any() else 0))
    d = dict(keil=[int(a.sum()), int(b.sum())], streif=[int(sa.sum()), int(sb.sum())], stuecke=n - 1, groesstes=gr, tief=round(tief, 1))
    return (d, sa, sb) if maske else d


def alle_paare(mt, pt, maske=False):
    keil = (mt != pt) & (mt >= 0) & (pt >= 0)
    s = np.zeros(mt.shape, bool)
    for P in np.unique(mt[keil]):
        s |= keil & (mt == P) & (_dist(pt == P) > TOL)
    paare = collections.Counter(zip(mt[s].tolist(), pt[s].tolist()))
    d = dict(keil=int(keil.sum()), streif=int(s.sum()), paare=dict(('%d>%d' % k, v) for k, v in sorted(paare.items())), figur=int((mt >= 0).sum()))
    return (d, s) if maske else d


def modell(emd_pfad):
    if _W.get('key') != emd_pfad:
        _W['mo'] = FE.modell(open(emd_pfad, 'rb').read(), open(TIM_K5E, 'rb').read())
        _W['cb'], _W['R'], _W['T'] = G.posen(_W['mo'])
        _W['an'] = G.ansichten()
        _W['key'] = emd_pfad
        _W['alle'] = tuple(range(len(_W['mo'].meshes)))
    return _W


def raster(emd_pfad, name, i, teile):
    w = modell(emd_pfad)
    kam, lage, gier, H, W, ab = w['an'][name]
    tris = G.dreiecke_teile(w['mo'], w['R'][i], w['T'][i], lage, gier, kam, teile=teile)
    return tris, G.keil_raster(tris, H, W, ab)


def ansicht(emd_pfad, name, i, nur=None):
    """Eine Ansicht (Bildindex i ueber alle Clips): dict mit ab4, ab5, alle, dreiecke."""
    w = modell(emd_pfad)
    mo = w['mo']
    kam, lage, gier, H, W, ab = w['an'][name]
    c, bi, _kf = w['cb'][i]
    aus = dict(ansicht=name, clip=int(c), bild=int(bi))
    t5 = G.dreiecke_teile(mo, w['R'][i], w['T'][i], lage, gier, kam, teile=(10, 21, 16, 22, 23))
    t4 = [t for t in t5 if t[3] != 16]
    mt, pt, _a, _b = G.keil_raster(t4, H, W, ab)
    aus['ab4'] = gruppen(mt, pt, RUMPF4)
    mt, pt, _a, _b = G.keil_raster(t5, H, W, ab)
    aus['ab5'] = gruppen(mt, pt, RUMPF5)
    if nur != 'ab':
        ta = G.dreiecke_teile(mo, w['R'][i], w['T'][i], lage, gier, kam, teile=w['alle'])
        mt, pt, _a, _b = G.keil_raster(ta, H, W, ab)
        aus['alle'] = alle_paare(mt, pt)
        aus['dreiecke'] = len(ta)
    return aus


def _job(arg):
    return ansicht(*arg)


def bild_index(cb, clip, bild):
    return [k for k, (cc, bb, _kf) in enumerate(cb) if cc == clip and bb == bild][0]


def lauf(aus, proz, schritt, emd_pfad, nur=None):
    from multiprocessing import Pool
    t0 = time.time()
    w = modell(emd_pfad)
    cb = w['cb']
    if schritt <= 1:
        idx = list(range(len(cb)))
    else:
        idx = [i for i, (c, b, kf) in enumerate(cb) if c in (10, 12)]
        idx += [i for i, (c, b, kf) in enumerate(cb) if c not in (10, 12)][::schritt]
    jobs = [(emd_pfad, nm, i, nur) for nm in ANSICHTEN for i in idx]
    print('==', emd_pfad, 'Bilder', len(idx), 'Ansichten', len(jobs), flush=True)
    erg = []
    with Pool(proz) as pool:
        for k, r in enumerate(pool.imap(_job, jobs, chunksize=4)):
            erg.append(r)
            if (k + 1) % 400 == 0:
                print('   %d / %d  %.0f s' % (k + 1, len(jobs), time.time() - t0), flush=True)
    sha = hashlib.sha256(open(emd_pfad, 'rb').read()).hexdigest()
    json.dump(dict(emd=emd_pfad.replace(chr(92), '/'), sha256=sha, schritt_rest=schritt, bilder=len(idx), ansichten=len(jobs), tol=TOL, erg=erg),
              open(aus, 'w'))
    print('   fertig %.0f s -> %s (sha256 %s)' % (time.time() - t0, aus, sha), flush=True)


def summe(pfad, bezug=None, still=False):
    d = json.load(open(pfad))
    erg = d['erg']
    p = (lambda *a: None) if still else print
    p('==', pfad.replace(chr(92), '/').split('/')[-1], 'EMD sha256', d['sha256'], 'Ansichten', len(erg), 'schritt_rest', d['schritt_rest'])
    aus = dict(sha256=d['sha256'], ansichten=len(erg))
    for les in ('ab4', 'ab5'):
        v = [(sum(r[les]['streif']), r['ansicht'], r['clip'], r['bild']) for r in erg]
        s = sum(x[0] for x in v); mx = max(v)
        je = collections.Counter()
        for x in v:
            je[x[1]] += x[0]
        aus[les] = dict(summe=s, ohne=sum(1 for x in v if x[0] == 0), groesste=mx, ueber20=sum(1 for x in v if x[0] > 20), je_ansicht=dict(je),
                        keil=sum(sum(r[les]['keil']) for r in erg))
        p('  %s: Streifen-Pixel Summe %d (rohe Keil-Pixel %d), Ansichten mit 0: %d, ueber 20: %d, groesste %d (%s Clip %d Bild %d)' % (
            les, s, aus[les]['keil'], aus[les]['ohne'], aus[les]['ueber20'], mx[0], mx[1], mx[2], mx[3]))
        p('      je Ansicht:', dict(je))
    if 'alle' not in erg[0]:
        return aus
    paare = collections.Counter(); gr = {}
    for r in erg:
        for k, n in r['alle']['paare'].items():
            paare[k] += n
            if n > gr.get(k, (0,))[0]:
                gr[k] = (n, r['ansicht'], r['clip'], r['bild'])
    kopf = sum(n for k, n in paare.items() if mit_kopf(k))
    dr = max((r['dreiecke'], r['ansicht'], r['clip'], r['bild']) for r in erg)
    aus.update(paare=dict(paare), kopf_summe=kopf, groesste_paar=gr, dreiecke_max=dr)
    p('  alle Paare: Summe %d, mit Kopf 22 oder Unterkiefer 23: %d; meiste gezeichnete Dreiecke %d (%s Clip %d Bild %d)' % (
        sum(paare.values()), kopf, dr[0], dr[1], dr[2], dr[3]))
    bz = None
    if bezug:
        bz = collections.Counter()
        for r in json.load(open(bezug))['erg']:
            for k, n in r['alle']['paare'].items():
                bz[k] += n
        p('  Bezug mit Kopf/Unterkiefer:', sum(n for k, n in bz.items() if mit_kopf(k)))
    for k in sorted(set(paare) | set(bz or []), key=lambda k: -(paare.get(k, 0) + (bz or dict()).get(k, 0))):
        if not mit_kopf(k) and paare.get(k, 0) + (bz or dict()).get(k, 0) < 2000:
            continue
        g = gr.get(k, (0, '-', 0, 0))
        p('    %-7s %7d%s   groesste Ansicht %4d (%s Clip %d Bild %d)' % (k, paare.get(k, 0), ('  Bezug %7d' % bz.get(k, 0)) if bz is not None else '', g[0], g[1], g[2], g[3]))
    return aus


if __name__ == '__main__':
    if sys.argv[1] == 'stich':
        emd = sys.argv[2]
        w = modell(emd)
        shots = [s.split(':') for s in sys.argv[3:]] or [['bild2', '12', '0'], ['spiel5080_vorn', '12', '2']]
        print('==', emd.replace(chr(92), '/').split('/')[-1], hashlib.sha256(open(emd, 'rb').read()).hexdigest()[:16])
        for nm, c, b in shots:
            r = ansicht(emd, nm, bild_index(w['cb'], int(c), int(b)))
            kp = dict((k, v) for k, v in r['alle']['paare'].items() if mit_kopf(k))
            print('  %-15s C%-2d B%-3d ab4 Streifen %4d (Keil %4d)  ab5 Streifen %4d (Keil %4d)  alle: Streifen %5d, Dreiecke %4d, Kopf-Paare %s' % (
                nm, r['clip'], r['bild'], sum(r['ab4']['streif']), sum(r['ab4']['keil']), sum(r['ab5']['streif']), sum(r['ab5']['keil']),
                r['alle']['streif'], r['dreiecke'], kp), flush=True)
    elif sys.argv[1] == 'lauf':
        lauf(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]), sys.argv[5], sys.argv[6] if len(sys.argv) > 6 else None)
    elif sys.argv[1] == 'summe':
        summe(sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else None)
