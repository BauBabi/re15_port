#!/usr/bin/env python3
"""Variante K (E20), Kern: J laden, Posen in Welt, Tiefenpuffer-Raster (Gleitkomma, nur fuer Masken und Messung), Masken der Nutzer-Teile.
Die Abnahme-Messung selbst laeuft ueber form_pruefung.py (re15_port/tools, unveraendert). Kein RE-Wert."""
import json
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K_DIR = os.path.normpath(os.path.join(HIER, '..'))
VAR = os.path.normpath(os.path.join(K_DIR, '..'))
FORM = os.path.normpath(os.path.join(VAR, '..'))
NT = os.path.join(FORM, 'nutzer_teile')
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import emd_ansichtsblatt as EA  # noqa: E402
import form_emd as FE  # noqa: E402
import form_kamera as FK  # noqa: E402

J_DIR = os.path.join(VAR, 'J_dicker')
H, W = 480, 640


def lade(d=J_DIR):
    emd = open(os.path.join(d, 'FORM.EMD'), 'rb').read()
    tim = open(os.path.join(d, 'FORM.TIM'), 'rb').read()
    fj = json.load(open(os.path.join(d, 'form.json'), encoding='utf-8'))
    return FE.lese_emd(emd), FE.modell(emd, tim), fj


def maske(name):
    return cv2.imread(os.path.join(NT, 'maske_%s.png' % name), 0) > 0


def kf_von(mo, clip, bild=0):
    f0, k = mo.clips[clip]
    return mo.frames[f0 + min(bild, k - 1)] & 0xFFF


def welt_pose(R, T, lage, gier):
    """R, T aus mo.pose (Q12 int) -> Welt (float): welt = Y(gier) (R v + T) + lage."""
    s, c = EA.sin_q12(gier) / 4096.0, EA.cos_q12(gier) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    Rw = [Y @ (np.asarray(r, float) / 4096.0) for r in R]
    Tw = [Y @ np.asarray(t, float) + np.asarray(lage, float) for t in T]
    return Rw, Tw


def flaechen(mesh):
    """Dreiecke als Eckindizes; Vierecke Z-foermig: a,b,d und a,d,c (form_emd)."""
    out = [f['e'] for f in mesh['dreiecke']]
    for f in mesh['vierecke']:
        a, b, c, d = f['e']
        out += [[a, b, d], [a, d, c]]
    return np.array(out, int).reshape(-1, 3)


def tris_welt(form, Rw, Tw, knochen=None):
    """Alle Dreiecke in Welt: (N,3,3) und Knochen je Dreieck."""
    P, K = [], []
    for b, m in enumerate(form['meshes']):
        if knochen is not None and b not in knochen:
            continue
        if not m['ecken']:
            continue
        v = np.array(m['ecken'], float) @ Rw[b].T + Tw[b]
        f = flaechen(m)
        if len(f):
            P.append(v[f])
            K.append(np.full(len(f), b))
    if not P:
        return np.zeros((0, 3, 3)), np.zeros(0, int)
    return np.concatenate(P), np.concatenate(K)


def raster(kam, tris, ids, tief=None, idb=None):
    """Tiefenpuffer-Raster: je Pixel naechstes Dreieck. tris (N,3,3) Welt. Rueckgabe Tiefe (inf = leer), Kennung (-1 = leer)."""
    if tief is None:
        tief = np.full((H, W), np.inf)
        idb = np.full((H, W), -1, int)
    if not len(tris):
        return tief, idb
    p = FK.projiziere(kam, tris.reshape(-1, 3)).reshape(-1, 3, 2)
    z = FK.tiefe(kam, tris.reshape(-1, 3)).reshape(-1, 3)
    for i in range(len(tris)):
        q = p[i]
        if not np.isfinite(q).all() or (z[i] < 64).any():
            continue
        x0, y0 = np.floor(q.min(0)).astype(int)
        x1, y1 = np.ceil(q.max(0)).astype(int) + 1
        x0, y0, x1, y1 = max(x0, 0), max(y0, 0), min(x1, W), min(y1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        fl = (q[1, 0] - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (q[1, 1] - q[0, 1])
        if abs(fl) < 1e-9:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.0, np.arange(y0, y1) + 0.0)
        w1 = ((PX - q[0, 0]) * (q[2, 1] - q[0, 1]) - (q[2, 0] - q[0, 0]) * (PY - q[0, 1])) / fl
        w2 = ((q[1, 0] - q[0, 0]) * (PY - q[0, 1]) - (PX - q[0, 0]) * (q[1, 1] - q[0, 1])) / fl
        w0 = 1 - w1 - w2
        inn = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
        if not inn.any():
            continue
        zz = w0 * z[i, 0] + w1 * z[i, 1] + w2 * z[i, 2]
        tb = tief[y0:y1, x0:x1]
        ib = idb[y0:y1, x0:x1]
        neu = inn & (zz < tb)
        tb[neu] = zz[neu]
        ib[neu] = ids[i]
    return tief, idb


def iou(a, b):
    return float((a & b).sum()) / max(float((a | b).sum()), 1.0)


def kamera(n):
    return FK.lade_kamera(n)
