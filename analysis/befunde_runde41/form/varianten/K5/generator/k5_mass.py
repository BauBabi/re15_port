#!/usr/bin/env python3
"""Variante K5, Pruefwerkzeug (AE11): Regeln R1-R10 aus FORMBLATT_V2 Abschnitt 6 am GEBAUTEN Modell (FORM.EMD mit Gelenkkoerpern), Projektion
mit Tiefenpuffer (k_kern.raster), Kamera Bild 2 = ROOM5090 Cut 11 (kamera_bild2.json), Pose Clip 1; Bild 1 Clip 0. Augen = Flaechen von Knochen 22
mit Texeln im Atlasfeld 'auge' (K5: Augaepfel; K4: Augenwuelste, wie Leser A). R8: Umriss Bild 1/2 und Pruefpunkte aus form_pruefung (bericht.json
im Ordner pruefung/), dazu IoU Ober-/Unterkiefer aus dem eigenen Raster.

    python k5_mass.py messen <Variante-Ordner> [--k4] [--aus mass.json]
    python k5_mass.py tabelle AUS.md DIR1 DIR2 ...

Gegenprobe: K4 (varianten/K_alligator/K4_kroko) muss an R1/R2 rot sein (Augen verdeckt, 3/4-Profil, FORMBLATT_V2 AE1)."""
import argparse
import json
import math
import os
import sys

import cv2
import numpy as np

import k5_kern as C
import k5_kopf as K5

KK, FE, FK = C.KK, C.FE, C.FK
sys.path.insert(0, KK.TOOLS)
import k_form as KF  # noqa: E402

GRENZEN = {'R1': 'beide Augen sichtbar (>= 3 px), je <= 6 px von L1/L2', 'R2': 'Augenlinie <= 3 Grad, Abstand 57-69 px', 'R3': 'OK-Spitze <= 6 px von (245,371)',
           'R4': 'Mittellinie <= 5 Grad von L5 (1,9 Grad)', 'R5': 'UK deckt Lappen >= 60 %, (279,399) im UK', 'R6': 'Maulwinkel l (W) <= 8 px von (329,326)',
           'R7': 'UK-Schwerpunkt rechts (B), Achse 30-47 Grad', 'R8': 'Umriss B1 >= 0,954, B2 >= 0,948 (form_pruefung), IoU UK >= 0,80',
           'R9': 'Bodenpixel Bild 1 frei (0 von 189 + 160)', 'R10': 'Schwanz-Scheitel hoechster Punkt Bild 1, y <= 190'}


def _tex(f):
    pg = f['page'] & 0xF
    return [(u + pg * 128, v) for u, v in f['uv']]


def tris_kenn(form, Rw, Tw):
    """Alle Dreiecke in Welt mit Kennung: Knochen, Augen-Flaechen von 22 als AUGE_L (+Z) / AUGE_R (-Z)."""
    ax, ay, aw, ah = K5.ATLAS['auge']
    P, K = [], []
    for b, m in enumerate(form['meshes']):
        if not m['ecken']:
            continue
        V = np.array(m['ecken'], float)
        for art in ('dreiecke', 'vierecke'):
            for f in m[art]:
                e = f['e']
                tris = [e] if len(e) == 3 else [[e[0], e[1], e[3]], [e[0], e[3], e[2]]]
                k = b
                if b == 22 and 'uv' in f:
                    t = _tex(f)
                    if all(ax <= u <= ax + aw and ay <= v <= ay + ah for u, v in t):
                        k = C.AUGE_L if V[e].mean(0)[2] > 0 else C.AUGE_R
                for tr in tris:
                    P.append(V[tr] @ Rw[b].T + Tw[b])
                    K.append(k)
    return np.array(P), np.array(K)


def lade_form(d):
    emd = open(os.path.join(d, 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(d, 'FORM.TIM'), 'rb').read()
    fj = json.load(open(os.path.join(d, 'form.json'), encoding='utf-8'))
    return FE.lese_emd(emd), FE.modell(emd, tim), fj


def welt(d, n):
    form, mo, fj = lade_form(d)
    s = fj['bild%d' % n]
    R, T = mo.pose(KK.kf_von(mo, s['clip'], s['bild']))
    Rw, Tw = KK.welt_pose(R, T, s['lage'], s['gier'])
    return form, Rw, Tw


def kennung(d, n):
    form, Rw, Tw = welt(d, n)
    return C.raster(KK.kamera(n), *tris_kenn(form, Rw, Tw))[1]


def landmarken_k4(d):
    """K4 (Kopfart 'kroko'): OK-Spitze = Spitze des Kopfrohrs, UK-Spitze = Spitze des Kieferrohrs, Maulwinkel = Kieferrand an Station 2 aussen +Z
    (dieselbe Definition wie k5_kopf.landmarken); Augen = Mitte der Augenwulst-Flaechen (Atlas 'auge')."""
    kj = json.load(open(os.path.join(d, 'kopf.json'), encoding='utf-8'))
    p = dict(KF.VORGABE_KROKO)
    p.update(kj['form'])
    if p.get('schaedel') is not None:
        p['yt_k'] = p['yt_s'] - p['schaedel']
    xs, ws, yts, ybs = KF.kroko_stationen(p)
    kx, kw, _kyb, _tf, yr, Lj = KF.kiefer_kroko_stationen(p)
    return {22: dict(ok_spitze=[p['L'] + 40.0, (yts[-1] + ybs[-1]) / 2 + 30, 0.0]),
            23: dict(uk_spitze=[Lj + 40.0, yr + 30, 0.0], maulwinkel_l=[kx[2], yr, 0.96 * kw[2]])}


def augen_lokal(form):
    ax, ay, aw, ah = K5.ATLAS['auge']
    m = form['meshes'][22]
    V = np.array(m['ecken'], float)
    out = {}
    for nm, sg in (('auge_l', 1), ('auge_r', -1)):
        idx = set()
        for art in ('dreiecke', 'vierecke'):
            for f in m[art]:
                if 'uv' in f and all(ax <= u <= ax + aw and ay <= v <= ay + ah for u, v in _tex(f)) and np.sign(V[f['e']].mean(0)[2]) == sg:
                    idx |= set(f['e'])
        out[nm] = V[sorted(idx)].mean(0).tolist() if idx else None
    return out


def messen(d, k4=False):
    form, Rw, Tw = welt(d, 2)
    kam = KK.kamera(2)
    tris, kenn = tris_kenn(form, Rw, Tw)
    _t, idb = C.raster(kam, tris, kenn)
    if k4:
        lm = landmarken_k4(d)
    else:
        kj = json.load(open(os.path.join(d, 'kopf.json'), encoding='utf-8'))
        lm = {int(b): v for b, v in kj['landmarken'].items()}
    au = augen_lokal(form)
    lm[22] = dict(lm[22], **{k: v for k, v in au.items() if v is not None})
    lmb = {}
    for b, dd in lm.items():
        for nm, v in dd.items():
            lmb[nm] = FK.projiziere(kam, Rw[b] @ np.asarray(v, float) + Tw[b])
    r = C.messen_bild2(idb, lmb)
    form1, R1w, T1w = welt(d, 1)
    _t, idb1 = C.raster(KK.kamera(1), *tris_kenn(form1, R1w, T1w))
    r.update(C.messen_bild1(idb1))
    # R8 aus form_pruefung
    bj = os.path.join(d, 'pruefung', 'bericht.json')
    r8 = dict(ok=False, wert='form_pruefung fehlt')
    if os.path.exists(bj):
        b = json.load(open(bj, encoding='utf-8'))
        u1, u2 = b['bild1']['umriss']['iou'], b['bild2']['umriss']['iou']
        p1 = sum(1 for p in b['bild1']['punkte'] if p.get('zaehlt', True) and p.get('ok'))
        n1 = sum(1 for p in b['bild1']['punkte'] if p.get('zaehlt', True))
        p2 = sum(1 for p in b['bild2']['punkte'] if p.get('zaehlt', True) and p.get('ok'))
        n2 = sum(1 for p in b['bild2']['punkte'] if p.get('zaehlt', True))
        iu = r['iou']['uk']
        r8 = dict(ok=round(u1, 3) >= 0.954 and round(u2, 3) >= 0.948 and iu >= 0.80, wert='B1 %.3f (%d/%d), B2 %.3f (%d/%d), UK %.2f, OK %.2f' % (u1, p1, n1, u2, p2, n2, iu, r['iou']['ok']),
                  umriss1=u1, umriss2=u2, punkte1=[p1, n1], punkte2=[p2, n2], iou_uk=iu, iou_ok=r['iou']['ok'])
        # Umriss Bild 1 gegen das Ziel mit Boden (E26): Bodenbaender zaehlen dort als Hintergrund
        z = cv2.imread(os.path.join(C.FORM, 'ziel2_bild1.png'), 0)
        bl, br = C.boden_masken()
        weiss, grau = (z == 255) & ~(bl | br), z == 128
        fig = idb1 >= 0
        r8['umriss1_raster_e26'] = round(float((fig & weiss).sum()) / max(float(((fig | weiss) & ~grau).sum()), 1.0), 4)
        r8['umriss1_raster'] = C.umriss(idb1, 1)
        r8['umriss2_raster'] = C.umriss(idb, 2)
    r['R8'] = r8
    r['_lm_bild'] = {k: [round(float(v[0]), 1), round(float(v[1]), 1)] for k, v in lmb.items()}
    return r, idb, idb1


def zeile(name, r):
    z = ['| %s |' % name]
    for k in ('R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7', 'R8', 'R9', 'R10'):
        v = r[k]
        z.append(' %s %s |' % ('gruen' if v['ok'] else '**rot**', v['wert']))
    return ''.join(z)


def befehl_messen(d, k4, aus):
    r, idb, idb1 = messen(d, k4)
    aus = aus or os.path.join(d, 'mass.json')
    json.dump(r, open(aus, 'w', encoding='utf-8'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else bool(o) if isinstance(o, np.bool_) else str(o))
    np.save(os.path.splitext(aus)[0] + '_id2.npy', idb)
    np.save(os.path.splitext(aus)[0] + '_id1.npy', idb1)
    for k in ('R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7', 'R8', 'R9', 'R10'):
        print('%-4s %-5s %s' % (k, 'ok' if r[k]['ok'] else 'ROT', r[k]['wert']))


if __name__ == '__main__':
    a = argparse.ArgumentParser()
    a.add_argument('art', choices=('messen',))
    a.add_argument('dir')
    a.add_argument('--k4', action='store_true')
    a.add_argument('--aus', default=None)
    x = a.parse_args()
    befehl_messen(x.dir, x.k4, x.aus)
