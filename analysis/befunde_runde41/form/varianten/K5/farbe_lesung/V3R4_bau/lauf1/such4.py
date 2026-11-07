"""Bauer V3 R4: Bewertung eines Parametersatzes ueber das Flaechen-ID-Raster (1 oder 3 Saaten)."""
import sys, os, json, math, time
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kern4 as K
import ids as I
import ids4
import schnell as SN

ZUW, ZEILEN, NEU, MO = K.plan()
CNT = K.texel_je_paar(ZUW, ZEILEN)
T3 = K.KG.FE.tim_lesen(open(K.V3 + '/FORM.TIM', 'rb').read())
CL3 = [list(z) for z in T3['cluts']]
G4 = CL3[:3]
STAT = {p: K.statistik(G4[p[1]], CNT[p]) for p in ZEILEN}
NZ = max(max(ZEILEN.values()), 8) + 1

def szenen_neu():
    out = {}
    code_zeile = {}
    for n in (1, 2):
        d = np.load(os.path.join(HIER, 'fid%d.npz' % n))
        S = {k: d[k] for k in d.files}
        for zk, ck in (('z', 'c'), ('za', 'ca')):
            z = S[zk].astype(int).copy()
            for c in np.unique(S[ck][z >= 0]):
                if c not in code_zeile:
                    code_zeile[c] = NEU[ids4.code_key(c)]
                z[(S[ck] == c) & (z >= 0)] = code_zeile[c]
            S[zk] = z
        out[n] = S
    return out
SZ = szenen_neu()

F1_R4 = None   # Zeile 6 des Neubaus (Baufehler A behoben), gesetzt von setze_f1()

def setze_f1(vdir):
    global F1_R4
    F1_R4 = list(K.KG.FE.tim_lesen(open(vdir + '/FORM.TIM', 'rb').read())['cluts'][6])

def cluts(par, f1_neu=None):
    f1_neu = f1_neu if f1_neu is not None else F1_R4
    cl = [list(z) for z in G4] + [None] * (NZ - 3)
    for z in (6, 7, 8):
        cl[z] = list(CL3[z])
    if f1_neu is not None:
        cl[6] = f1_neu
    for p, z in ZEILEN.items():
        cl[z] = K.umfaerben(G4[p[1]], par[p[0]], STAT[p])
    for i in range(NZ):
        if cl[i] is None:
            cl[i] = list(G4[0])
    return cl

def bewerten(par, saaten=(15,), f1_neu=None):
    lut = I.lut_aus(cluts(par, f1_neu))
    SN._S.clear(); SN._S.update(SZ)
    return SN.bewerten(lut, saaten=saaten)

def diag(par, n=1, saat=15, f1_neu=None):
    """Lab T je Pixel (eine Saat), Zeile je Pixel, Basismaske, Regionen, A0."""
    lut = I.lut_aus(cluts(par, f1_neu))
    sauber, alb, m = I.sauber_aus(SZ[n], lut)
    basis = np.unpackbits(SN.M['basis%d' % n])[:480 * 640].reshape(480, 640).astype(bool) & m & (alb.max(2) > 0)
    reg = SN.M['reg%d' % n].astype(int)
    L = SN.RR.lab(SN.PZ.rueck(np.asarray(SN.PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True, saat=saat), np.uint8), n)[0])
    return L, SZ[n]['z'], basis, reg, SN.E['bilder'][str(n)]['A0']['T'], sauber

def ader_haut(par, f1_neu=None, saat=15, drucken=True):
    """Je Gruppe: Ader (G4-Eintrag mit Rot-Gewicht w > 0) gegen Haut (w = 0, L* < L_HELL): Mittel L*, a* im Texelraum (texelgewichtet)
    und in den Ketten Bild 1/2 (T, Saat 15). Nutzer 2026-11-07: Adern nie heller und nie roter als die umgebende Haut."""
    cl = cluts(par, f1_neu)
    out = {}
    for (g, s), zz in ZEILEN.items():
        Lg, C, h, w = K.eintrag_lab(G4[s])
        Ln = K.KG.lab(K.KG.wort_rgb(cl[zz]))
        cnt = CNT[(g, s)]
        ad = (w > 0) & (cnt > 0); ha = (w == 0) & (cnt > 0) & (np.array(G4[s]) != 0)
        mw = lambda sel, j: float((Ln[sel, j] * cnt[sel]).sum() / max(cnt[sel].sum(), 1))
        out.setdefault(g, {})['texel s%d' % s] = dict(n_ader=int(cnt[ad].sum()), L=(mw(ad, 0), mw(ha, 0)), a=(mw(ad, 1), mw(ha, 1)))
    for n in (1, 2):
        L, z, basis, reg, A0, _ = diag(par, n, saat, f1_neu)
        i = SZ[n]['i']
        for (g, s), zz in ZEILEN.items():
            Lg, C, h, w = K.eintrag_lab(G4[s])
            m = (z == zz) & basis
            if m.sum() < 100: continue
            ii = i[m]
            ad = w[ii] > 0; ha = w[ii] == 0
            if ad.sum() < 20 or ha.sum() < 20: continue
            Lm_ = L[m]
            out.setdefault(g, {})['kette%d s%d' % (n, s)] = dict(n_ader=int(ad.sum()), L=(float(Lm_[ad, 0].mean()), float(Lm_[ha, 0].mean())), a=(float(Lm_[ad, 1].mean()), float(Lm_[ha, 1].mean())))
    if drucken:
        for g, d in out.items():
            for k, v in d.items():
                ok = v['L'][0] <= v['L'][1] and v['a'][0] <= v['a'][1]
                print('  %-16s %-11s Ader/Haut L %5.1f/%5.1f  a* %5.1f/%5.1f  n_ader %6d  %s' % (g, k, v['L'][0], v['L'][1], v['a'][0], v['a'][1], v['n_ader'], 'ok' if ok else 'VERLETZT'))
    return out

RING = 3   # Pixel/Texel: Umkreis "umgebende Haut" (8-Nachbarschaft, 3 Schritte); Optik-sigma 2 px (farbe.md 3) + 1

def ader_ring(par, f1_neu=None, saat=15, drucken=True):
    """Ader gegen umgebende Haut: Haut = Nicht-Ader-Pixel/Texel derselben Gruppe im Umkreis RING um eine Ader. Texelraum (Atlas, Zeile der
    Gruppe, genutzte Texel) und Ketten Bild 1/2 (T)."""
    import cv2
    cl = cluts(par, f1_neu)
    out = {}
    ker = np.ones((2 * RING + 1, 2 * RING + 1), np.uint8)
    px = T3['pixel']
    for (g, s), zz in ZEILEN.items():
        Lg, C, h, w = K.eintrag_lab(G4[s])
        Ln = K.KG.lab(K.KG.wort_rgb(cl[zz]))
        gen = GENUTZT[(g, s)]
        idx = px
        ad = gen & (w[idx] > 0); ha = gen & (w[idx] == 0) & (np.array(G4[s])[idx] != 0)
        ring = ha & cv2.dilate(ad.astype(np.uint8), ker).astype(bool)
        if ad.sum() < 20 or ring.sum() < 20: continue
        LL = Ln[idx]
        out.setdefault(g, {})['texel s%d' % s] = dict(n_ader=int(ad.sum()), L=(float(LL[ad, 0].mean()), float(LL[ring, 0].mean())), a=(float(LL[ad, 1].mean()), float(LL[ring, 1].mean())))
    for n in (1, 2):
        L, z, basis, reg, A0, _ = diag(par, n, saat, f1_neu)
        i = SZ[n]['i']
        for (g, s), zz in ZEILEN.items():
            Lg, C, h, w = K.eintrag_lab(G4[s])
            m = (z == zz) & basis
            if m.sum() < 100: continue
            ad = m & (w[i] > 0); ha = m & (w[i] == 0)
            ring = ha & cv2.dilate(ad.astype(np.uint8), ker).astype(bool)
            if ad.sum() < 20 or ring.sum() < 20: continue
            out.setdefault(g, {})['kette%d s%d' % (n, s)] = dict(n_ader=int(ad.sum()), L=(float(L[ad, 0].mean()), float(L[ring, 0].mean())), a=(float(L[ad, 1].mean()), float(L[ring, 1].mean())))
    if drucken:
        for g, d in out.items():
            for k, v in d.items():
                ok = v['L'][0] <= v['L'][1] and v['a'][0] <= v['a'][1]
                print('  %-16s %-11s Ader/Ring L %5.1f/%5.1f  a* %5.1f/%5.1f  n_ader %6d  %s' % (g, k, v['L'][0], v['L'][1], v['a'][0], v['a'][1], v['n_ader'], 'ok' if ok else 'VERLETZT'))
    return out

def _genutzt():
    form = K.KG.FE.lese_emd(open(K.V3 + '/FORM.EMD', 'rb').read())
    out = {p: np.zeros((256, 512), bool) for p in ZEILEN}
    for (mi, teil, fi), (g, s, z, nm) in ZUW.items():
        if not g: continue
        f = form['meshes'][mi]['dreiecke' if teil == 't' else 'vierecke'][fi]
        q = np.array([[u + 128 * s, v] for u, v in f['uv']], float)
        xy = K.KG.texel_set(q)
        if len(xy):
            out[(g, s)][xy[:, 1], xy[:, 0]] = True
    return out
GENUTZT = _genutzt()
