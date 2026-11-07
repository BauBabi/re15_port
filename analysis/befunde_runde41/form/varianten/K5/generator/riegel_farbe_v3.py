#!/usr/bin/env python3
"""Riegel MESSBLATT_FARBE_V3 Abschnitt 5, jede Zeile, an einer Variante (Ordner mit FORM.EMD, FORM.TIM, form.json), mit Gegenproben.

    python riegel_farbe_v3.py <Variante> [--gegenprobe-v2 <farbe/K5c_farbe_v2>] [--json aus.json] [--schnell]

Zeilen: G0, H1, H1b, H2, P1, P2, D1, S1, B10-B17, A1-A5/A7/A8/A9/A11/A12 (+A6 nur Sonderfelder, A10), Glanz (A5 + N2), K1v2/K2v2/K3-K10/K4b,
B2/B9/B9b, K11-K15, F1t/F2t (+ Zaehne). Exit 1, wenn eine Zeile der Variante ROT ist oder eine Gegenprobe nicht ROT (bzw. G4 nicht GRUEN) ist.

Atlas mit mehreren CLUT-Zeilen: Ein Texel, das zwei Teile mit verschiedenen Zeilen nutzen, ist zwei Farben. Gemessen wird darum am
"Zeilen-Atlas" (wie der Port: je CLUT-Zeile eine Kopie der Textur untereinander, render_pc.c re15_render_pc_upload_tim_slot): jede Flaeche
liegt in der Kopie ihrer Zeile. Felder der V3-Anordnung (Messblatt V3 1.3 "Felder im Riegel-Skript nachtragen"): Koerper = alle Flaechen
ausser Kopf 22, Kiefer 23 und Sonderfeldern; Kopf = Mesh 22 ohne Sonderfelder; Kiefer = Mesh 23 ohne Sonderfelder plus Maul-Band (A8);
Auge = Augenflaechen (K5c-Augenfeld); Kralle = Klauen 14/15/19/20/28-31; Sonderfelder = Zeilen 6-8 (F1-Stelle, Band, Zaehne) und Augen.
"""
import argparse
import copy
import json
import math
import os
import sys

import cv2
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
K5 = os.path.normpath(os.path.join(HIER, '..'))
FL = os.path.join(K5, 'farbe_lesung')
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(FL, 'P_skripte'))
sys.path.insert(0, os.path.join(FL, 'B_skripte'))
sys.path.insert(0, os.path.join(FL, 'N2_skripte'))
import kf_g4 as KG  # noqa: E402
import p_lade as PL  # noqa: E402
import p_riegel as PR  # noqa: E402
import riegel_farbe as RF  # noqa: E402

FE = KG.FE
FB = KG.FB if hasattr(KG, 'FB') else None
import form_buehne as FB  # noqa: E402,F811

# ---------------------------------------------------------------- Grenzen (Messblatt V3 5; Quelle je Zeile dort)
H1_MIN = 95.0              # H1 >= 95 % je Teil
H1B_MAX = 10.0             # H1b <= 10 % (Bildanteile 2,0 + 6,6 = 8,6 %, gerundet 10)
P1_DL0_MAX, P1_DL5_MIN, P1_FARBEN_MIN = 30.0, 15.0, 200     # Familie dL=0 17,1-28,9 %, 5-10 21,3-25,6 %, 206-420 Farben
P2_PLUS = 8.0              # PORT-WAHL Pruefer: dL=0 K5c-Teil <= G4-Quellteil + 8 Punkte
D1_MAX = 2.0               # lineare Vergroesserung <= 2,0
S1_MAX = 1.0               # fremde Texel <= 1 % je Teil
K11_FIGUR, K11_RUECKEN = 4.0, 0.7          # Rundlauf -6,0 / -9,3 Grad + 10 Grad
K12_FAKTOR, K12_PLUS = 1.5, 3.0            # <= max(1,5 x Original, Original + 3 Punkte)
K13_MIN = 0.85 * 54.6                      # 46,4 %
K14_ZIEL, K14_ROT = (45.0, 16.6, -2.0), 0.85 * 69.2   # Rundlauf F1 (ziel_kette.json, 3 Saaten), Rotklasse >= 58,8 %
K15_ZIEL, K15_ROT = (49.9, 28.2, 16.1), 0.85 * 96.7   # Rundlauf Band, Rotklasse >= 82,2 %
K_DE = 6.0                                  # dE <= 6 wie V1 K3/K7
F_DE = 6.0                                  # F1t/F2t dE <= 6
ZAHN_L, ZAHN_C = 70.0, 12.0                 # Zaehne L* >= 70, C* <= 12
GLANZ_ATLAS, GLANZ_RENDER = 0.20, 0.10      # V1 A5 / N2
SAAT = 15

TEILE = KG.TEILE_H1
# P2: K5c-Teil <- G4-Quellteil (Messblatt V3 2.3), gleicher Weltmassstab, 3 Ansichten (stich.py)
P2_TEILE = [('Rumpf', [0, 7], [0, 9]), ('Brust', [10], [16]), ('Schwanz', [1, 2, 3, 4, 5, 6], [11, 12]), ('Hals', [21], [16]),
            ('Hinterbein l (W)', [8, 24, 25], [5, 6, 7]), ('Hinterbein r (W)', [9, 26, 27], [1, 2, 3]),
            ('Vorderbein l (W)', [11, 12, 13], [11, 12, 13]), ('Vorderbein r (W)', [16, 17, 18], [15, 16, 17, 18]),
            ('Klauen l (W)', [14, 15, 28, 30], [14]), ('Klauen r (W)', [19, 20, 29, 31], [19]), ('Kopf + Kiefer', [22, 23], [16])]
# Runde 2: jedes Teil gegen seine tatsaechliche Quelle laut kf_g4.ZUORDNUNG (Zahlen-Pruefer P2: Runde 1 mass den Kopf gegen G4 10 statt 16).
# Kopf, Brust und Hals kommen aus G4 16 (F18), der Rumpf hinten aus G4 0/9.
KLAUEN = [14, 15, 28, 30, 19, 20, 29, 31]


def lab(rgb):
    return KG.lab(np.asarray(rgb, float).reshape(-1, 3))


def de(a, b):
    return float(np.linalg.norm(np.asarray(a, float) - np.asarray(b, float)))


# ================================================================ Laden, Flaechen, Zeilen-Atlas
class Var:
    def __init__(self, vdir):
        self.vdir = vdir
        self.emd = open(os.path.join(vdir, 'FORM.EMD'), 'rb').read()
        self.tim = open(os.path.join(vdir, 'FORM.TIM'), 'rb').read()
        self.mo = FE.modell(self.emd, self.tim)
        t = FE.tim_lesen(self.tim)
        self.px, self.cl, self.clut_y = t['pixel'], t['cluts'], t['clut_y']
        self.rgb_zeilen = np.stack([KG.wort_rgb(z) for z in self.cl])           # Zeilen x 256 x 3
        # Herkunftskarte des Baus (Runde 2): je Texel G4-Quelle (hx, hy) und Art (1 G4-Texel, 2 F1-Fleisch, 3 Band). Ohne Datei (V2r2, Runde 1):
        # Seiten 0-2 an Ort, Seite 3 ohne Quelle.
        hp = os.path.join(vdir, 'herkunft.npz')
        if os.path.exists(hp):
            z_ = np.load(hp)
            self.hx, self.hy, self.hart = z_['hx'].astype(int), z_['hy'].astype(int), z_['art']
        else:
            H_, W_ = self.px.shape
            self.hx = np.full((H_, W_), -1, int)
            self.hy = np.full((H_, W_), -1, int)
            self.hart = np.zeros((H_, W_), np.uint8)
            self.hx[:, :384] = np.arange(384)[None, :]
            self.hy[:, :384] = np.arange(H_)[:, None]
            self.hart[:, :384] = 1
        self.flaechen = []                                                      # (key, mesh, zeile, Atlas-Ecken, 3D-Flaeche)
        for mi, m in enumerate(self.mo.meshes):
            for teil, verts, faces, uvs in (('t', m.tv, m.tf, m.tuv), ('q', m.qv, m.qf, m.quv)):
                V = np.array([x[:3] for x in verts], float) if verts else np.zeros((0, 3))
                for fi, (g, uv) in enumerate(zip(faces, uvs)):
                    po = (uv[5] & 0xF) * 128
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if teil == 'q' else [])
                    P = V[list(g[1::2])]
                    if teil == 'q':
                        a3 = 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[3] - P[0])) + 0.5 * np.linalg.norm(np.cross(P[3] - P[0], P[2] - P[0]))
                    else:
                        a3 = 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[2] - P[0]))
                    z = ((uv[2] >> 6) & 0x1FF) - self.clut_y
                    self.flaechen.append(((mi, teil, fi), mi, int(z), np.array(tx, float), a3))

    def texel(self, ecken):
        return KG.texel_set(np.asarray(ecken, float))


def sonder_keys(v, k5c):
    """Sonderfelder: Zeilen 6-8 (F1, Band, Zaehne) und Augen (K5c-Augenfeld, an der K5c-UV erkannt, kf_g4.K5C_AUGENRECHT)."""
    aug = set()
    for key in KG.alle_flaechen(k5c, [22, 23]):
        if KG.in_recht(KG.flaeche_info(k5c, key)[2], KG.K5C_AUGENRECHT):
            aug.add(key)
    s = {f[0] for f in v.flaechen if f[2] in (KG.Z_F1, KG.Z_BAND, KG.Z_ZAHN)}
    return s | aug, aug


def flach(mo, nzeilen):
    """Zeilen-Atlas-Modell: Textur nzeilen-mal untereinander, jede Flaeche um 256 x Zeile nach unten (Port-Weg)."""
    m2 = copy.copy(mo)
    m2.tex = np.vstack([mo.tex] * nzeilen)
    m2.texh = m2.tex.shape[0]
    m2.meshes = []
    for m in mo.meshes:
        mm = copy.copy(m)

        def schieb(uv, vier):
            z = ((uv[2] >> 6) & 0x1FF) - mo.clut_y
            d = 256 * z
            u = list(uv)
            for i in ((1, 4, 7, 10) if vier else (1, 4, 7)):
                u[i] = uv[i] + d
            return tuple(u)
        mm.tuv = [schieb(uv, False) for uv in m.tuv]
        mm.quv = [schieb(uv, True) for uv in m.quv]
        m2.meshes.append(mm)
    return m2


# ================================================================ G0
def g0(v, k5c_emd, k5c):
    d0, d1 = FE.verzeichnis(k5c_emd)[1], FE.verzeichnis(v.emd)[1]
    edd_emr = k5c_emd[d0[1]:d0[3]] == v.emd[d1[1]:d1[3]]
    geo = all(a.tv == b.tv and a.tn == b.tn and a.tf == b.tf and a.qv == b.qv and a.qn == b.qn and a.qf == b.qf
              for a, b in zip(k5c.meshes, v.mo.meshes)) and len(k5c.meshes) == len(v.mo.meshes)
    kn = v.mo.skel.bone_count
    cl = len(v.mo.clips) == len(k5c.clips)
    ok = edd_emr and geo and kn == 32 and cl
    return (dict(geometrie_gleich=geo, edd_emr_bytegleich=edd_emr, knochen=kn, clips=len(v.mo.clips)), ok)


# ================================================================ H1, H1b, S1, D1 (am echten Atlas)
def herkunft(v, G, sonder, px_g4):
    """je Teil (Runde 2, Abtastung wie die PSX: kf_g4.texel_psx, auch UV-Punkte und -Linien; Herkunft ueber die Karte des Baus):
    H1   = Anteil der gelesenen Texel (flaechengewichtet), deren Index gleich dem G4-Index an der Quelle ist und deren Quelle in der G4-Maske liegt;
    S1   = Anteil ohne G4-Quelle in der G4-Maske (fremde Texel);
    D1   = sqrt(Summe 3D-Flaeche / Summe (UV-Flaeche x G4-3D-Flaeche je Texel an der Quelle)): die Messblatt-Formel mit den G4-Flaechen, die
           die Flaechen tatsaechlich abbilden (Wiederholungen mitgezaehlt); dazu ortsgenau je Flaeche Median und Anteil > 2 (Zahlen-Pruefer, vergr.py);
    koll = 3D-Anteil der Flaechen, deren UV ein Punkt/eine Linie ist, obwohl sie bei G4-Dichte >= 1 Texel^2 decken (Befund A)."""
    gm = G.global_maske
    tf = G.texel_flaeche
    res = {}
    for nm, meshes in TEILE.items():
        w_ok = w_all = w_fremd = 0.0
        a3s = auvg = 0.0
        lok, lok_w = [], []
        a_koll = a_ges = 0.0
        used = set()
        for key, mi, z, ecken, a3 in v.flaechen:
            if mi not in meshes or key in sonder:
                continue
            xy = v.texel(ecken)
            a_ges += a3
            if not len(xy):
                continue
            xs, ys = xy[:, 0], xy[:, 1]
            inn = (xs >= 0) & (xs < v.px.shape[1]) & (ys >= 0) & (ys < v.px.shape[0])
            sx = np.full(len(xy), -1)
            sy = np.full(len(xy), -1)
            sx[inn], sy[inn] = v.hx[ys[inn], xs[inn]], v.hy[ys[inn], xs[inn]]
            q = inn.copy()
            q[inn] &= (v.hart[ys[inn], xs[inn]] == 1) & (sx[inn] >= 0)
            src_ok = np.zeros(len(xy), bool)
            src_ok[q] = gm[sy[q], sx[q]]
            gl = np.zeros(len(xy), bool)
            gl[q] = src_ok[q] & (v.px[ys[q], xs[q]] == px_g4[sy[q], sx[q]])
            w = a3 / len(xy)
            w_ok += w * gl.sum()
            w_all += w * len(xy)
            w_fremd += w * (~src_ok).sum()
            auv = KG._flaeche2(ecken)
            ag = float(tf[sy[src_ok], sx[src_ok]].mean()) if src_ok.any() else float('nan')
            if ag == ag and ag > 0:
                a3s += a3
                auvg += auv * ag
                lok.append(math.sqrt(a3 / (auv * ag)) if auv > 0 else float('inf'))
                lok_w.append(a3)
                if auv == 0 and a3 / ag >= 1.0:
                    a_koll += a3
            for x_, y_ in zip(sx[src_ok], sy[src_ok]):
                used.add((int(x_), int(y_)))
        lok, lok_w = np.array(lok), np.array(lok_w)
        o = np.argsort(lok)
        cw = np.cumsum(lok_w[o]) / max(lok_w.sum(), 1e-9)
        med = float(lok[o][np.searchsorted(cw, 0.5)]) if len(lok) else float('nan')
        res[nm] = dict(H1=round(100 * w_ok / max(w_all, 1e-9), 2), S1=round(100 * w_fremd / max(w_all, 1e-9), 2),
                       D1=round(math.sqrt(a3s / max(auvg, 1e-9)), 2), D1_median=round(med, 2),
                       D1_ueber2=round(100 * float(lok_w[lok > 2.0].sum() / max(lok_w.sum(), 1e-9)), 1),
                       kollabiert=round(100 * a_koll / max(a_ges, 1e-9), 2), texel=len(used))
    tot = sum(f[4] for f in v.flaechen)
    sa = sum(f[4] for f in v.flaechen if f[0] in sonder)
    return res, round(100 * sa / tot, 2)


def d1_messblatt(v, G, sonder):
    """D1 wie Messblatt V3 5: sqrt(3D-Flaeche K5c-Teil / 3D-Flaeche der G4-Quellflaechen); Quellflaechen = Flaechen der erlaubten Inseln
    der Zuordnung (kf_g4.ZUORDNUNG, 'einzeln' je Ziel-Mesh einmal), Kopf zusaetzlich F2 (Maulinneres)."""
    a_k = {}
    for key, mi, z, ecken, a3 in v.flaechen:
        if key not in sonder:
            a_k[mi] = a_k.get(mi, 0.0) + a3

    def g4_flaeche(qm, op):
        ohne, nur, oi = set(op.get('ohne', [])), op.get('nur'), op.get('ohne_inseln', {})
        A = 0.0
        for mi in qm:
            for k, (V, T, cw, q) in enumerate(G.fl[mi]):
                isl = int(G.ins[mi][k])
                fn = G.feld(mi, isl)
                if isl in oi.get(mi, []) or (nur is not None and fn not in nur) or (nur is None and fn in ohne):
                    continue
                A += 0.5 * np.linalg.norm(np.cross(V[1] - V[0], V[2] - V[0]))
        return A
    q = {}
    for nm, zm, qm, art, op in KG.ZUORDNUNG:
        ag = g4_flaeche(qm, op) * (len(zm) if art == 'einzeln' else 1)
        for t, ms in TEILE.items():
            if set(zm) <= set(ms):
                q.setdefault(t, [0.0, 0.0])
                q[t][0] += sum(a_k.get(m, 0.0) for m in zm)
                q[t][1] += ag
    q['Kopf/Kiefer aussen'][1] += g4_flaeche([26], dict(nur=['F2']))
    return {t: round(math.sqrt(a / b), 2) for t, (a, b) in q.items()}


# ================================================================ H2
def h2(v, cl_g4):
    """Zeilen 0-2 bytegleich G4; je umgefaerbter Zeile (3-8) die genutzten Eintraege: L*-Rangfolge gegen den G4-Eintrag (Vertauschungen bei dL >= 2)."""
    gleich = [v.cl[p] == list(cl_g4[p]) for p in range(3)]
    genutzt = {}
    for key, mi, z, ecken, a3 in v.flaechen:
        xy = v.texel(ecken)
        if len(xy):
            genutzt.setdefault(z, set()).update(int(i) for i in v.px[xy[:, 1], xy[:, 0]])
    basis = {3: 0, 4: 1, 5: 2, KG.Z_F1: 2, KG.Z_BAND: 0, KG.Z_ZAHN: 0}
    zeilen = {}
    tausch_ges = 0
    for z in range(3, len(v.cl)):
        b = basis.get(z, 0)
        e = sorted(i for i in genutzt.get(z, set()) if v.cl[z][i] != 0)
        Lg = lab(KG.wort_rgb([cl_g4[b][i] for i in e]))[:, 0] if e else np.zeros(0)
        Ln = lab(KG.wort_rgb([v.cl[z][i] for i in e]))[:, 0] if e else np.zeros(0)
        t = 0
        for i in range(len(e)):
            for j in range(len(e)):
                if Lg[i] - Lg[j] >= 2 and Ln[i] < Ln[j]:
                    t += 1
        um = [i for i in e if v.cl[z][i] != cl_g4[b][i]]
        dEs = [round(de(lab(KG.wort_rgb([cl_g4[b][i]]))[0], lab(KG.wort_rgb([v.cl[z][i]]))[0]), 1) for i in um]
        zeilen[z] = dict(basis=b, genutzt=len(e), umgefaerbt=len(um), vertauschungen=t, dE_median=float(np.median(dEs)) if dEs else 0.0,
                         dE_max=float(max(dEs)) if dEs else 0.0)
        tausch_ges += t
    return dict(zeilen_0_2_bytegleich=gleich, zeilen=zeilen), all(gleich) and tausch_ges == 0


# ================================================================ P1 (Zeilen-Atlas), F1t/F2t, Zaehne
def p1_mass(rgb, u):
    from scipy import ndimage as ndi
    L = lab(rgb.reshape(-1, 3)).reshape(rgb.shape)
    fl = ndi.binary_erosion((L[..., 1] > 15) & u, iterations=1)
    res = []
    for dy, dx in ((0, 1), (1, 0)):
        a = fl[:L.shape[0] - dy, :L.shape[1] - dx] & fl[dy:, dx:]
        res.append(np.abs(L[:L.shape[0] - dy, :L.shape[1] - dx, 0] - L[dy:, dx:, 0])[a])
    d = np.concatenate(res)
    farben = len(np.unique(rgb[fl].reshape(-1, 3), axis=0))
    return dict(fleischtexel=int(fl.sum()), dL0=round(100 * float((d < 0.5).mean()), 1), dL5_10=round(100 * float(((d >= 5) & (d < 10)).mean()), 1),
                farben=farben)


def zeilen_atlas(v):
    """RGB des Zeilen-Atlas (nzeilen x 256 hoch) und Maske der genutzten Texel; Flaechen-Mesh je Texel (erste Flaeche)."""
    n = len(v.cl)
    rgb = np.zeros((256 * n, 512, 3), np.uint8)
    u = np.zeros((256 * n, 512), bool)
    for key, mi, z, ecken, a3 in v.flaechen:
        xy = v.texel(ecken)
        if not len(xy):
            continue
        xs, ys = np.clip(xy[:, 0], 0, 511), np.clip(xy[:, 1], 0, 255)
        rgb[ys + 256 * z, xs] = np.round(v.rgb_zeilen[z][v.px[ys, xs]]).astype(np.uint8)
        u[ys + 256 * z, xs] = True
    return rgb, u


def p1(v):
    rgb, u = zeilen_atlas(v)
    m = p1_mass(rgb, u)
    return m, (m['dL0'] <= P1_DL0_MAX and m['dL5_10'] >= P1_DL5_MIN and m['farben'] >= P1_FARBEN_MIN)


def p1_g4(G, g4tim):
    px, cl = KG.g4_cluts(g4tim)
    rgb = np.zeros((256, 384, 3), np.uint8)
    for p in range(3):
        rgb[:, p * 128:(p + 1) * 128] = np.round(KG.wort_rgb(np.array(cl[p])[px[:, p * 128:(p + 1) * 128]])).astype(np.uint8)
    m = p1_mass(rgb, G.global_maske)
    return m, (m['dL0'] <= P1_DL0_MAX and m['dL5_10'] >= P1_DL5_MIN and m['farben'] >= P1_FARBEN_MIN)


def feldmittel(v, cl_g4):
    """F1t: Texel der Zeile 6, deren Eintrag umgefaerbt ist (Fleisch des F1-Flecks); F2t: Texel der Zeile 7; Zaehne: Zeile 8."""
    out = {}
    for nm, z, b in (('F1t', KG.Z_F1, 2), ('F2t', KG.Z_BAND, 0), ('Zaehne', KG.Z_ZAHN, 0)):
        xy_all = set()
        for key, mi, zz, ecken, a3 in v.flaechen:
            if zz == z:
                for x, y in v.texel(ecken):
                    xy_all.add((int(x), int(y)))
        if nm == 'F1t':
            # Runde 2: das Feld = alle gelesenen Texel der Art "F1-Fleisch" (Herkunftskarte); die G4-Hautkopie der F1-Flaechen ist G4-Texel (H1)
            fl_ = [(x, y) for x, y in xy_all if v.hart[y, x] == 2]
            if not fl_:          # ohne Herkunftskarte (Runde 1, V2r2): alle gelesenen Texel der Zeile
                fl_ = list(xy_all)
            out['F1_feld'] = dict(gelesen=len(xy_all), fleisch=len(fl_), g4_haut=sum(1 for x, y in xy_all if v.hart[y, x] == 1))
            xy_all = fl_
        idx = [int(v.px[y, x]) for x, y in xy_all]
        L = lab(v.rgb_zeilen[z][idx]) if idx else np.zeros((1, 3))
        out[nm] = dict(texel=len(idx), lab=[round(float(x), 2) for x in L.mean(0)], C=round(float(np.hypot(*L.mean(0)[1:])), 2))
    return out


# ================================================================ P2 (Render je Teil gegen G4-Quellteil), nach Pruefer stich.py (bild, masse)
def _p2_bild(mo, kf, bones, tex, az, el, s, mitte, G_=300):
    R, T = mo.pose(kf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    Lr = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    aus = []
    for b in bones:
        m = mo.meshes[b]
        W = R[b].astype(float) / 4096.0
        t = T[b].astype(float)
        for (verts, norms, faces, uvs, q) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            vv = np.array([x[:3] for x in verts], float) @ W.T + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ W.T
            nn /= np.maximum(np.linalg.norm(nn, axis=1, keepdims=True), 1e-9)
            qq = (vv - mitte) @ A.T
            sx, sy = G_ / 2 + qq[:, 0] * s, G_ / 2 + qq[:, 1] * s
            hell = np.clip(0.35 + 0.65 * np.maximum(nn @ Lr, 0), 0, 1) * 255.0
            for g, uv in zip(faces, uvs):
                ie, inn = g[1::2], g[0::2]
                p = [(float(sx[i]), float(sy[i])) for i in ie]
                m0 = FB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if q else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128
                z = (uv[2] >> 6) & 0x1FF
                c = [np.full(3, hell[i]) for i in inn]
                if q:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                        aus.append((float(qq[ie[x], 2] + qq[ie[y], 2] + qq[ie[zz], 2]) / 3, (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz]), z, (c[x], c[y], c[zz])))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    aus.append((float(qq[ie[0], 2] + qq[ie[1], 2] + qq[ie[2], 2]) / 3, tuple(p), tuple(tx), z, tuple(c)))
    img = np.full((G_, G_, 3), 40, np.uint8)
    m = FB.rastern(aus, tex, img, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    return img, np.asarray(m, bool)


def _dl0(img, m):
    L = lab(img.reshape(-1, 3).astype(np.float32)).reshape(img.shape)
    Li = np.round(L[..., 0])
    pa = []
    for dy, dx in ((0, 1), (1, 0)):
        a = m[:m.shape[0] - dy, :m.shape[1] - dx] & m[dy:, dx:]
        pa.append(np.abs(Li[:Li.shape[0] - dy, :Li.shape[1] - dx] - Li[dy:, dx:])[a])
    d = np.concatenate(pa)
    return 100 * float((d == 0).mean()) if len(d) else 0.0


def p2(v, g4, g4tim, mo=None, tv=None, teile=None, clip=2):
    """mo/tv/teile: fuer die Gegenprobe (b2)-Probe A (RGB-Textur, Kopf aus G4-F1 gegen G4 26)."""
    mo_v = v.mo if mo is None else mo
    tv = FB.Textur(v.tim) if tv is None else tv
    tg = FB.Textur(g4tim)
    kfv = mo_v.frames[mo_v.clips[clip if clip < len(mo_v.clips) else 0][0]] & 0xFFF
    kfg = g4.frames[g4.clips[0][0]] & 0xFFF
    res, ok = {}, True
    for nm, kb, gb in (P2_TEILE if teile is None else teile):
        def pkt(mo, kf, bones):
            R, T = mo.pose(kf)
            return np.vstack([np.array([x[:3] for x in mo.meshes[b].tv + mo.meshes[b].qv], float) @ (R[b].astype(float) / 4096.0).T + T[b].astype(float)
                              for b in bones])
        Pk, Pg = pkt(mo_v, kfv, kb), pkt(g4, kfg, gb)
        mk, mg = (Pk.max(0) + Pk.min(0)) / 2, (Pg.max(0) + Pg.min(0)) / 2
        rad = max(np.linalg.norm(Pk - mk, axis=1).max(), np.linalg.norm(Pg - mg, axis=1).max())
        s = 300 * 0.46 / rad
        dk, dg = [], []
        for az, el in ((0, 10), (-90, 10), (45, 25)):
            dk.append(_dl0(*_p2_bild(mo_v, kfv, kb, tv, az, el, s, mk)))
            dg.append(_dl0(*_p2_bild(g4, kfg, gb, tg, az, el, s, mg)))
        a, b = float(np.mean(dk)), float(np.mean(dg))
        res[nm] = (round(a, 1), round(b, 1), round(a - b, 1))
        ok &= a <= b + P2_PLUS
    return res, ok


# ================================================================ Atlas-Riegel V1 auf dem Zeilen-Atlas mit V3-Feldern
def atlas_v3(v, sonder, aug):
    fm = flach(v.mo, len(v.cl))
    sk = {(f[0]) for f in v.flaechen}
    band_keys = {f[0] for f in v.flaechen if f[2] == KG.Z_BAND}
    per, tri = PL.sammeln(fm)
    rgb, mid = PR.atlas(fm, per)
    h, w = mid.shape
    u = mid >= 0
    # Feldmasken aus den Flaechen (Zeilen-Atlas-Koordinaten)
    masken = {k: np.zeros((h, w), bool) for k in ('koerper', 'kopf', 'kiefer', 'auge', 'kralle', 'sonder')}
    for key, mi, z, ecken, a3 in v.flaechen:
        xy = v.texel(ecken)
        if not len(xy):
            continue
        xs, ys = np.clip(xy[:, 0], 0, w - 1), np.clip(xy[:, 1] + 256 * z, 0, h - 1)
        if key in aug:
            masken['auge'][ys, xs] = True
        elif key in band_keys or (z == KG.Z_ZAHN and mi == 23):
            # Runde 2: Kieferfeld = Unterkiefer aussen + Band + Zaehne des Unterkiefers (Messblatt V3 1.3 A8: "Bild 2 zeigt dort 66 % Rot und 12 % Zahn")
            masken['kiefer'][ys, xs] = True
        elif key in sonder:
            masken['sonder'][ys, xs] = True
        elif mi == 22:
            masken['kopf'][ys, xs] = True
        elif mi == 23:
            masken['kiefer'][ys, xs] = True
        elif mi in KLAUEN:
            masken['kralle'][ys, xs] = True
            masken['koerper'][ys, xs] = True
        else:
            masken['koerper'][ys, xs] = True
    Z = {k: m & u for k, m in masken.items() if k != 'sonder'}
    alt = PR.atlas_zonen
    PR.atlas_zonen = lambda mo: (per, tri, rgb, mid, Z)
    xs_alt = None
    try:
        r = PR.atlas_riegel(fm, 'v3')
    finally:
        PR.atlas_zonen = alt
    # A6 nur fuer Sonderfelder (Messblatt V3 1.3): HF_fft je Sonderfeld
    from p_messen import hf_fft
    Lmap = np.zeros(mid.shape)
    sm = masken['sonder'] & u
    Lmap[sm] = PL.lab(rgb[sm])[:, 0]
    hf = hf_fft(Lmap, mid, sm)
    r['A6 HF_fft je Feld (max)'] = ({'sonder': None if hf is None else round(hf, 3)}, hf is None or 0.04 <= hf <= 0.26)
    return r, fm, rgb, mid, Z


# ================================================================ Ketten (Bild 1/2) K11-K15
def _rot(T, m):
    L = lab(T[m])
    C = np.hypot(L[:, 1], L[:, 2])
    r = (L[:, 1] >= 12) & (C >= 15) & (L[:, 0] >= 20)
    P = L[r]
    h = math.degrees(math.atan2(P[:, 2].mean(), P[:, 1].mean())) if len(P) else None
    return dict(anteil=round(100 * float(r.mean()), 1) if len(L) else 0.0, h=None if h is None else round(h, 1),
                dunkel=round(100 * float((L[:, 0] < 20).mean()), 1) if len(L) else 0.0, lab=[round(float(x), 1) for x in lab(T[m].mean(0))[0]] if m.any() else None)


def ketten_v3(vdir, masken, mo=None, tex=None):
    import p_zonen as PZ
    k = PZ.P.Kandidat(os.path.join(vdir, 'form.json'))
    if mo is not None:                       # Gegenprobe (b2)-Probe A: Modell und RGB-Textur der Probe in der K5c-Lage (wie Pruefer kette_b2.py)
        k.mo, k.tex = mo, tex
    out = {}
    R = {}
    for n in (1, 2):
        sauber, alb, m, kk = k.rendern(n)
        f = m & (alb.max(2) > 0)
        vid = PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=SAAT)
        T, c = PZ.rueck(vid, n)
        vo = PZ.P.original(n)
        To, co = PZ.rueck(vo, n)
        regs = dict(PZ.regionen(n))
        fig = np.zeros((480, 640), bool)
        for g in regs.values():
            fig |= g
        R[n] = dict(T=T, To=To, regs=regs, fig=fig, f=f, m=m)
    # K11
    r1 = R[1]
    fi = _rot(r1['T'], r1['fig'] & r1['f'])
    ru = _rot(r1['T'], r1['regs']['Ruecken/Flanke'] & r1['f'])
    h_f = fi['h'] if fi['h'] is not None else -99
    h_r = ru['h'] if ru['h'] is not None else -99
    out['K11 Bild1 Farbton Rotklasse Figur / Ruecken Grad'] = ((fi['h'], ru['h']), h_f <= K11_FIGUR and h_r <= K11_RUECKEN)
    # K12
    zz, ok12 = {}, True
    for nm, g in r1['regs'].items():
        o = _rot(r1['To'], g)['anteil']
        vv = _rot(r1['T'], g & r1['f'])['anteil']
        gr = max(K12_FAKTOR * o, o + K12_PLUS)
        zz[nm] = (vv, o, round(gr, 1))
        ok12 &= vv <= gr
    out['K12 Bild1 Rotklasse je Zone % (Variante, Original, Grenze)'] = (zz, ok12)
    # K13
    r2 = R[2]
    d2 = _rot(r2['T'], r2['fig'] & r2['m'])['dunkel']
    out['K13 Bild2 dunkle Klasse Figur %'] = (d2, d2 >= K13_MIN)
    # K14 / K15
    for nm, n, mk, ziel, rmin in (() if masken.get('nur_k11_13') else (('K14 Bild1 F1-Stelle', 1, masken['F1'], K14_ZIEL, K14_ROT),
                                                                      ('K15 Bild2 Fleischband', 2, masken['band'], K15_ZIEL, K15_ROT))):
        T = R[n]['T']
        mu = lab(T[mk].mean(0))[0]
        rr = _rot(T, mk)['anteil']
        d = de(mu, ziel)
        out[nm + ' Lab / dE / Rotklasse %'] = (([round(float(x), 1) for x in mu], round(d, 1), rr), d <= K_DE and rr >= rmin)
    return out


def masken_daten():
    z = np.load(os.path.join(HIER, 'v3_daten', 'masken.npz'))
    sh = tuple(z['form'])
    return {k: np.unpackbits(z[k])[:sh[0] * sh[1]].reshape(sh).astype(bool) for k in ('F1', 'band', 'zahn')}


# ================================================================ Gesamt
def messen(vdir, schnell=False):
    g4, g4tim, _q = KG.g4_modell()
    G = KG.G4Quelle(g4)
    px_g4, cl_g4 = KG.g4_cluts(g4tim)
    k5c_emd, k5c_tim = KG.k5c_bytes()
    k5c = FE.modell(k5c_emd, k5c_tim)
    v = Var(vdir)
    sonder, aug = sonder_keys(v, k5c)
    res = {}
    res['G0 Geometrie-Diff'] = g0(v, k5c_emd, k5c)
    hk, h1b = herkunft(v, G, sonder, px_g4)
    res['H1 Herkunft G4-Texel je Teil % (>= 95)'] = ({k: x['H1'] for k, x in hk.items()}, all(x['H1'] >= H1_MIN for x in hk.values()))
    res['H1b Anteil Sonderfelder % 3D-Flaeche (<= 10)'] = (h1b, h1b <= H1B_MAX)
    res['H2 CLUT (0-2 bytegleich, Rangfolge)'] = h2(v, cl_g4)
    res['P1 Posterisierung Atlas dL0/5-10/Farben'] = p1(v)
    d1 = d1_messblatt(v, G, sonder)
    res['D1 Vergroesserung je Teil, abgebildete G4-Flaeche / Median ortsgenau (<= 2,0)'] = (
        {k: (x['D1'], x['D1_median']) for k, x in hk.items()}, all(x['D1'] <= D1_MAX and x['D1_median'] <= D1_MAX for x in hk.values()))
    res['D1 ortsgenau Anteil Flaechen > 2,0 % [info]'] = ({k: x['D1_ueber2'] for k, x in hk.items()}, None)
    res['D1 Messblatt-Formel ohne Wiederholung (Quellmeshes einmal) [info]'] = (d1, None)
    res['S1b kollabierte UV (Punkt/Linie bei >= 1 Texel^2) % 3D [info]'] = ({k: x['kollabiert'] for k, x in hk.items()}, None)
    res['S1 fremde Texel je Teil % (<= 1)'] = ({k: x['S1'] for k, x in hk.items()}, all(x['S1'] <= S1_MAX for x in hk.values()))
    fm_ = feldmittel(v, cl_g4)
    zf1, zb = lab(np.array([KG.ZIEL_F1]))[0], lab(np.array([KG.ZIEL_BAND]))[0]
    d1, d2 = de(fm_['F1t']['lab'], zf1), de(fm_['F2t']['lab'], zb)
    res['F1t Feldmittel F1-Stelle Lab / dE'] = ((fm_['F1t']['lab'], round(d1, 2), fm_['F1t']['texel']), d1 <= F_DE)
    res['F2t Feldmittel Band Lab / dE'] = ((fm_['F2t']['lab'], round(d2, 2), fm_['F2t']['texel']), d2 <= F_DE)
    zl = fm_['Zaehne']
    res['F3t Zaehne L* / C*'] = ((zl['lab'][0], zl['C']), zl['lab'][0] >= ZAHN_L and zl['C'] <= ZAHN_C)
    ar, fm, rgbf, midf, Zf = atlas_v3(v, sonder, aug)
    for k, (val, ok) in ar.items():
        res[k + ' [V3-Felder]'] = (val, bool(ok))
    if not schnell:
        res['P2 Render dL=0 je Teil (K5c, G4, Diff)'] = p2(v, g4, g4tim)
    # B10-B17 (n2_riegel): Atlas am Zeilen-Atlas, Koerper/Kopf = V3-Felder; Render am echten Modell
    import n2_riegel as NR
    import fl as FLB
    mo_r = FLB.lade_var(vdir)
    a_ = NR.N.atlas_mass(fm, lambda u, mid: Zf['koerper'], 'koerper')
    k_ = NR.N.atlas_mass(fm, lambda u, mid: Zf['kopf'], 'kopf')
    r_ = NR.N.render_mass(mo_r, 2, 'render')
    g_, ko_ = NR.render_koh(mo_r, 2)
    for k, (val, ok) in NR.riegel(dict(atlas=a_, kopf=k_, render=r_, koh_gross=g_, koh=ko_)).items():
        res[k] = (val, bool(ok))
    # Glanz: A5 (oben) + N2 / N1
    for k, (val, ok) in RF.n_riegel(vdir).items():
        res[k] = (val, bool(ok))
    # Ketten V1/V2
    for k, (val, ok) in PR.kette_riegel(vdir).items():
        if k.startswith(('K1 ', 'K2 ')):
            res[k + ' [V1, ersetzt]'] = (val, None)
        else:
            res[k] = (val, bool(ok))
    for k, (val, ok) in RF.kette_v2(vdir).items():
        res[k] = (val, bool(ok))
    st = RF.struktur_render(mo_r, 2)
    g2 = [100.0 * q['gross'] / q['px'] for q in st[:2]]
    res['B2 groesste Rotflaeche % Figur (3/4 vorn, Seite)'] = (tuple(round(x, 2) for x in g2), min(g2) >= 3.2)
    sm = RF._rot_saum(mo_r, 2)
    res['B9 Rot im Umrissband % (3 Winkel)'] = (round(sm, 1), sm <= RF.B9_MAX)
    s0 = RF._rot_saum(mo_r, 0)
    res['B9b Rot im Umrissband % Clip 0'] = (round(s0, 1), s0 <= RF.B9_MAX)
    M = masken_daten()
    for k, (val, ok) in ketten_v3(vdir, M).items():
        res[k] = (val, bool(ok))
    zus = dict(teile=hk, h1b=h1b, feldmittel=fm_)
    return res, zus


def gegenprobe_v2(vdir_v2):
    """V2r2: H1 (G4-Indexbloecke), P1, K11, K14, K15 muessen ROT sein (Messblatt V3 5 Gegenproben)."""
    g4, g4tim, _q = KG.g4_modell()
    G = KG.G4Quelle(g4)
    px_g4, cl_g4 = KG.g4_cluts(g4tim)
    k5c_emd, k5c_tim = KG.k5c_bytes()
    k5c = FE.modell(k5c_emd, k5c_tim)
    v = Var(vdir_v2)
    sonder, aug = sonder_keys(v, k5c)
    hk, _ = herkunft(v, G, sonder, px_g4)
    out = {}
    out['H1'] = ({k: x['H1'] for k, x in hk.items()}, all(x['H1'] >= H1_MIN for x in hk.values()))
    out['P1'] = p1(v)
    kv = ketten_v3(vdir_v2, masken_daten())
    for k, val in kv.items():
        if k.startswith(('K11', 'K14', 'K15')):
            out[k] = val
    # G4 selbst: P1 GRUEN
    out['P1 G4 (muss GRUEN)'] = p1_g4(G, g4tim)
    return out


A_PROBE = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe3/A'
ORANGE, GELB = (255, 140, 40), (255, 200, 60)     # Messblatt V3 3.3 Tabelle "Orange (255,140,40) / Gelb (255,200,60)"


def gegenproben_weitere(vdir):
    """Runde 2 (Zahlen-Pruefer (4): "Nicht im Riegel, obwohl in Messblatt 5 als Gegenprobe gefuehrt"), als Lauf mit denselben Funktionen:
    - G4 selbst bei B10-B17 (n2_riegel.messen wie --familie): muss 8/8 GRUEN sein.
    - (b2)-Probe A (Leser A voll2_uv.pkl, Kopf aus G4-F1, ohne Feuerstellen; Modell wie Pruefer kette_b2.py): K11, K12, K13 und P2 Kopf
      (Kopf + Kiefer gegen G4 26) muessen ROT sein.
    - Orange und Gelb als Feldfarbe (CLUT-Zeilen 6/7 der Variante einfarbig): F1t und F2t muessen ROT sein."""
    out = {}
    import n2_riegel as NR
    import n2_mass as N2M
    import kf_tafeln as KT
    Z = N2M.zonen()
    r = NR.riegel(NR.messen(KT.re2_mo(0x34), 0, Z['g4'], Z['g4kopf']))
    out['G4 B10-B17 (muss 8/8 GRUEN)'] = ('%d/%d' % (sum(1 for _v, ok in r.values() if ok), len(r)), all(ok for _v, ok in r.values()))
    # (b2)-Probe A
    alt = os.getcwd()
    # Leser As Module (render.py, kern3.py, ...) heissen wie Module der B-Skripte: fuer den Import der Probe kurz aus sys.modules nehmen
    gesichert = {n: sys.modules.pop(n) for n in ('render', 'kern3', 'uebertrag', 'uvprojekt', 'probe', 'lade') if n in sys.modules}
    try:
        os.chdir(A_PROBE)
        sys.path.insert(0, A_PROBE)
        import pickle
        from uvprojekt import modell_mit, RGBTextur, kombi_atlas  # noqa: F401
        from probe import mo as g4mo, tg, k5, k5_rgba
        alle = pickle.load(open('voll2_uv.pkl', 'rb'))
        m2 = modell_mit(k5, alle)
        tex = RGBTextur(kombi_atlas(k5_rgba(), g4mo, tg))
    finally:
        os.chdir(alt)
        sys.path.remove(A_PROBE)
        for n in ('render', 'kern3', 'uebertrag', 'uvprojekt', 'probe', 'lade'):
            sys.modules.pop(n, None)
        sys.modules.update(gesichert)
    kv = ketten_v3(vdir, dict(nur_k11_13=True), mo=m2, tex=tex)
    for k, val in kv.items():
        if k.startswith(('K11', 'K12', 'K13')):
            out['(b2)-Probe A ' + k] = val
    g4, g4tim, _q = KG.g4_modell()
    v = Var(vdir)
    pr, ok = p2(v, g4, g4tim, mo=m2, tv=tex, teile=[('Kopf + Kiefer (G4-F1)', [22, 23], [26])])
    out['(b2)-Probe A P2 Kopf aus G4-F1 (K5c, G4 26, Diff)'] = (pr, ok)
    # Orange / Gelb
    px_g4, cl_g4 = KG.g4_cluts(g4tim)
    for nm, rgb in (('Orange', ORANGE), ('Gelb', GELB)):
        w = copy.copy(v)
        w.cl = [list(z) for z in v.cl]
        for zz in (KG.Z_F1, KG.Z_BAND):
            w.cl[zz] = [KG.wort(rgb) if c != 0 else 0 for c in v.cl[zz]]
        w.rgb_zeilen = np.stack([KG.wort_rgb(z) for z in w.cl])
        fm_ = feldmittel(w, cl_g4)
        zf1, zb = lab(np.array([KG.ZIEL_F1]))[0], lab(np.array([KG.ZIEL_BAND]))[0]
        d1_, d2_ = de(fm_['F1t']['lab'], zf1), de(fm_['F2t']['lab'], zb)
        out['%s F1t / F2t dE' % nm] = ((round(d1_, 1), round(d2_, 1)), d1_ <= F_DE or d2_ <= F_DE)
    return out


def drucken(name, res):
    print('== %s' % name)
    for k, (v, ok) in res.items():
        print('  %-58s %-90s %s' % (k, v, 'info' if ok is None else ('GRUEN' if ok else 'ROT')))
    g = sum(1 for _v, ok in res.values() if ok)
    n = sum(1 for _v, ok in res.values() if ok is not None)
    print('  -> %d/%d GRUEN' % (g, n))
    return g, n


def main():
    a = argparse.ArgumentParser()
    a.add_argument('variante')
    a.add_argument('--gegenprobe-v2', default=None)
    a.add_argument('--json', default=None)
    a.add_argument('--schnell', action='store_true', help='ohne P2 (Render je Teil)')
    x = a.parse_args()
    r, zus = messen(x.variante, x.schnell)
    g, n = drucken(x.variante, r)
    exitcode = 0 if g == n else 1
    out = dict(variante={k: dict(wert=str(v), ok=ok) for k, (v, ok) in r.items()}, zusatz=zus)
    if x.gegenprobe_v2:
        gp = gegenprobe_v2(x.gegenprobe_v2)
        gp.update(gegenproben_weitere(x.variante))
        print('== Gegenproben Messblatt V3 5 (V2r2: H1, P1, K11, K14, K15 ROT; G4: P1 und B10-B17 GRUEN; (b2)-Probe A: K11, K12, K13, P2 Kopf ROT;'
              ' Orange/Gelb: F1t/F2t ROT)')
        falsch = []
        for k, (v, ok) in gp.items():
            soll_gruen = k.startswith(('P1 G4', 'G4 '))
            print('  %-58s %-90s %s' % (k, v, 'GRUEN' if ok else 'ROT'))
            if ok != soll_gruen:
                falsch.append(k)
        print('  Gegenprobe: falsch ausgefallen: %s' % (falsch or 'keine'))
        out['gegenprobe_v2'] = {k: dict(wert=str(v), ok=ok) for k, (v, ok) in gp.items()}
        out['gegenprobe_falsch'] = falsch
        if falsch:
            exitcode = 1
    print('EXIT %d' % exitcode)
    if x.json:
        json.dump(out, open(x.json, 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=str)
    sys.exit(exitcode)


if __name__ == '__main__':
    main()
