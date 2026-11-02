#!/usr/bin/env python3
"""Abnahme 2: Ton aus den Screenshot-Kameras, nur aus FORM.EMD, Tiefenpuffer, Eckennormalen geglaettet je Knochen. Daneben Original aufgehellt, darueber Nutzerumrisse. Kein RE-Wert."""
import math, os, sys, json, glob
import cv2
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
FORM = os.path.dirname(HIER)
TOOLS = os.path.normpath(os.path.join(FORM, '..', '..', '..', 're15_port', 'tools'))
sys.path.insert(0, TOOLS)
import form_pruefung as P
import form_buehne as FB
K = os.path.join(FORM, 'rohbau_final', 'form.json')
SK = 3
TEIL = os.path.join(FORM, 'nutzer_teile')
GR = {(0, 7, 10): (.80, .60, .55), (21, 22, 23): (.85, .78, .40), (1, 2, 3, 4, 5, 6): (.50, .78, .55), (16, 17, 18, 19, 20, 29, 31): (.80, .42, .80), (11, 12, 13, 14, 15, 28, 30): (.40, .75, .40), (8, 24, 25): (.90, .60, .28), (9, 26, 27): (.35, .72, .86)}
FB_K = {}
for ks, f in GR.items():
    for x in ks:
        FB_K[x] = f

def netz(k):
    aus = []
    for b in range(min(k.mo.skel.bone_count, len(k.mo.meshes))):
        m = k.mo.meshes[b]
        vl, tl = [], []
        off = 0
        if m.tf:
            v = np.array([x[:3] for x in m.tv], float); vl.append(v)
            for g in m.tf:
                ie = g[1::2]; tl.append((ie[0], ie[1], ie[2]))
            off = len(v)
        if m.qf:
            v = np.array([x[:3] for x in m.qv], float); vl.append(v)
            for g in m.qf:
                ie = [i + off for i in g[1::2]]
                tl.append((ie[0], ie[1], ie[3])); tl.append((ie[0], ie[3], ie[2]))
        aus.append((np.vstack(vl) if vl else np.zeros((0, 3)), np.array(tl, int).reshape(-1, 3)))
    return aus

def welt(q, lage, gier):
    a = 2 * math.pi * gier / 4096.0
    s, c = math.sin(a), math.cos(a)
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    return q @ Y.T + np.array(lage, float)

def proj(W, kam):
    v = (kam.V.astype(float) / 4096.0) @ W.T + kam.t.astype(float)[:, None]
    u = 160 + kam.h * v[0] / v[2]
    w = 120 + kam.h * v[1] / v[2]
    x, y = FB.abbilden(u + FB.PHASE, w + FB.PHASE, kam.abbildung)
    return np.stack([x, y], 1), v[2]

def render(k, n, nr, teile=None, farbig=False):
    lage, gier, clip, bild = k.stand(nr)
    kam = P.kamera(nr)
    R, T = k.mo.pose(k.kf(clip, bild))
    B, H = 480 * SK, 640 * SK
    zb = np.full((B, H), 1e18); hell = np.zeros((B, H)); kn = np.full((B, H), -1, int)
    L = np.array([.35, -.8, .45]); L /= np.linalg.norm(L)
    for b, (v, f) in enumerate(n):
        if not len(v) or (teile is not None and b not in teile):
            continue
        q = v @ (R[b].astype(float).T / 4096.0) + T[b].astype(float)
        W = welt(q, lage, gier)
        px, z = proj(W, kam)
        px = px * SK
        # Eckennormalen, gleiche Lage = gleiche Normale
        schl = {tuple(np.round(p, 3)): i for i, p in enumerate(W)}
        idx = np.array([schl[tuple(np.round(p, 3))] for p in W])
        n3 = np.cross(W[f[:, 1]] - W[f[:, 0]], W[f[:, 2]] - W[f[:, 0]])
        vn = np.zeros_like(W)
        for j in range(3):
            np.add.at(vn, idx[f[:, j]], n3)
        vn = vn[idx]; vn /= np.linalg.norm(vn, axis=1, keepdims=True) + 1e-9
        s = np.abs(vn @ L)
        for t in f:
            q2 = px[t]
            d = (q2[1, 0] - q2[0, 0]) * (q2[2, 1] - q2[0, 1]) - (q2[2, 0] - q2[0, 0]) * (q2[1, 1] - q2[0, 1])
            if d == 0:
                continue
            x0 = max(int(math.floor(q2[:, 0].min())), 0); x1 = min(int(math.ceil(q2[:, 0].max())) + 1, H)
            y0 = max(int(math.floor(q2[:, 1].min())), 0); y1 = min(int(math.ceil(q2[:, 1].max())) + 1, B)
            if x0 >= x1 or y0 >= y1:
                continue
            X, Y = np.meshgrid(np.arange(x0, x1) + .5, np.arange(y0, y1) + .5)
            w1 = ((X - q2[0, 0]) * (q2[2, 1] - q2[0, 1]) - (q2[2, 0] - q2[0, 0]) * (Y - q2[0, 1])) / d
            w2 = ((q2[1, 0] - q2[0, 0]) * (Y - q2[0, 1]) - (X - q2[0, 0]) * (q2[1, 1] - q2[0, 1])) / d
            w0 = 1 - w1 - w2
            i = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
            if not i.any():
                continue
            zz = w0 * z[t[0]] + w1 * z[t[1]] + w2 * z[t[2]]
            neu = i & (zz < zb[y0:y1, x0:x1])
            zb[y0:y1, x0:x1][neu] = zz[neu]
            hell[y0:y1, x0:x1][neu] = (w0 * s[t[0]] + w1 * s[t[1]] + w2 * s[t[2]])[neu]
            kn[y0:y1, x0:x1][neu] = b
    return hell, kn

k = P.Kandidat(K)
n = netz(k)
TEILE = {1: ['tail', 'left_leg', 'right_leg', 'right_arm', 'part_of_Body'], 2: ['face_and_upper_body', 'left_arm_front', 'right_arm_front']}
FA = {'tail': (120, 255, 120), 'left_leg': (255, 150, 40), 'right_leg': (60, 230, 255), 'right_arm': (255, 60, 255), 'part_of_Body': (255, 170, 170), 'face_and_upper_body': (255, 255, 60), 'left_arm_front': (60, 255, 60), 'right_arm_front': (70, 140, 255)}
for nr, box in ((1, (175, 170, 450, 480)), (2, (130, 135, 485, 480))):
    h, kn = render(k, n, nr)
    g = (.20 + .78 * h)
    tn = np.zeros(h.shape + (3,), np.float32) + (.27, .27, .30)
    far = np.zeros(h.shape + (3,), np.float32) + (.20, .20, .23)
    for b in range(32):
        m = kn == b
        tn[m] = np.stack([g[m]] * 3, 1)
        far[m] = g[m][:, None] * np.array(FB_K.get(b, (.7, .7, .7)))
    r = np.zeros(kn.shape, bool)
    r[1:] |= kn[1:] != kn[:-1]; r[:, 1:] |= kn[:, 1:] != kn[:, :-1]
    far[r] = (.0, .0, .0)
    tn8 = (tn * 255).astype(np.uint8); far8 = (far * 255).astype(np.uint8)
    far8 = far8.copy()
    for t in TEILE[nr]:
        mk = cv2.imread(os.path.join(TEIL, 'maske_%s.png' % t), 0) > 127
        mg = cv2.resize(mk.astype(np.uint8), None, fx=SK, fy=SK, interpolation=cv2.INTER_NEAREST)
        ko, _ = cv2.findContours(mg, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(far8, ko, -1, FA[t], 1)
    o = P.original(nr).astype(np.float32)
    he = np.clip(255 * np.power(np.clip(o / 255 * 2.4, 0, 1), .7), 0, 255).astype(np.uint8)
    he3 = cv2.resize(he, None, fx=SK, fy=SK, interpolation=cv2.INTER_CUBIC)
    x0, y0, x1, y1 = [v * SK for v in box]
    t = np.hstack([he3[y0:y1, x0:x1], np.full((y1 - y0, 4, 3), 255, np.uint8), tn8[y0:y1, x0:x1], np.full((y1 - y0, 4, 3), 255, np.uint8), far8[y0:y1, x0:x1]])
    cv2.imwrite(os.path.join(HIER, 'bild', 'kamton_bild%d.png' % nr), t[:, :, ::-1])
    print('Bild', nr, t.shape)
