"""Bauer V3 R4: Gruppen, Zeilenplan, Umfaerbung (Prototyp fuer kf_g4.py Runde 4)."""
import sys, os, json, math
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import ids as I
K5 = I.K5
for p in (K5 + '/generator', K5 + '/farbe_lesung/V3R4_skripte'):
    if p not in sys.path:
        sys.path.insert(0, p)
import kf_g4 as KG
import basis as BA

V3 = K5 + '/farbe/K5c_farbe_v3r3'   # Runde-3-Stand (gleiche UV wie der Neubau)
GRUPPEN = ['Schwanz', 'Hinterbeine', 'Rumpf Ruecken', 'Rumpf Flanke', 'Vorderbein r B1', 'Brust', 'Hals Bauch', 'Kopf', 'Vorderbeine']

def lade():
    bau = json.load(open(V3 + '/bau.json', encoding='utf-8'))
    emd = open(V3 + '/FORM.EMD', 'rb').read(); tim = open(V3 + '/FORM.TIM', 'rb').read()
    mo = KG.FE.modell(emd, tim)
    return bau, mo

def gruppe(key, nm, zeile, mo, fn):
    mi, teil, fi = key
    if zeile >= 6 or nm.startswith(('Augen', 'Maulinneres')):
        return None
    b1 = zeile in (3, 4, 5)
    nr = fi if teil == 't' else len(mo.meshes[mi].tf) + fi
    nx, ny = fn[mi][nr][0], fn[mi][nr][1]
    r = BA.REGIONEN[BA.region_k5c(mi, nx, ny)]
    if r == 'Schwanz': return 'Schwanz'
    if r == 'Hinterbeine': return 'Hinterbeine'
    if r == 'Vorderbeine': return 'Vorderbein r B1' if b1 else 'Vorderbeine'
    if r == 'Hals': return 'Hals Bauch'
    if r == 'Kopf': return 'Kopf'
    if mi in (0, 7):
        return 'Rumpf Ruecken' if r == 'Ruecken' else 'Rumpf Flanke'
    if mi == 10:
        return 'Hals Bauch' if r == 'Brust/Bauch' else 'Brust'
    raise SystemExit('ohne Gruppe %s %s' % (key, r))

def plan():
    bau, mo = lade()
    fn = BA.flaechen_normalen(mo)
    zuw = {}
    for ks, (nm, zeile, q) in bau['flaechen_herkunft'].items():
        a, t, f = ks.split('|'); key = (int(a), t, int(f))
        g = gruppe(key, nm, int(zeile), mo, fn)
        seite = int(zeile) % 3 if int(zeile) < 6 else None
        zuw[key] = (g, seite, int(zeile), nm)
    paare = sorted({(g, s) for g, s, z, nm in zuw.values() if g}, key=lambda p: (GRUPPEN.index(p[0]), p[1]))
    frei = [3, 4, 5] + list(range(9, 9 + len(paare)))
    zeilen = {p: frei[i] for i, p in enumerate(paare)}
    neu = {}
    for key, (g, s, z, nm) in zuw.items():
        if g:
            neu[key] = zeilen[(g, s)]
        elif nm.startswith('Augen'):
            neu[key] = 0
        else:
            neu[key] = z
    return zuw, zeilen, neu, mo

def texel_je_paar(zuw, zeilen, vdir=V3):
    """Texelzaehlung (Index je G4-Zeile) je (Gruppe, Seite) aus den UV der Flaechen."""
    t = KG.FE.tim_lesen(open(vdir + '/FORM.TIM', 'rb').read())
    px = t['pixel']
    form = KG.FE.lese_emd(open(vdir + '/FORM.EMD', 'rb').read())
    cnt = {p: np.zeros(256) for p in zeilen}
    for (mi, teil, fi), (g, s, z, nm) in zuw.items():
        if not g: continue
        f = form['meshes'][mi]['dreiecke' if teil == 't' else 'vierecke'][fi]
        q = np.array([[u + 128 * s, v] for u, v in f['uv']], float)
        if teil == 'q':
            q = q[[0, 1, 3, 2]] if False else q
        xy = KG.texel_set(q)
        if len(xy):
            np.add.at(cnt[(g, s)], px[xy[:, 1], xy[:, 0]], 1)
    return cnt

def eintrag_lab(zeile):
    rgb = KG.wort_rgb(zeile)
    L = KG.lab(rgb)
    C = np.hypot(L[:, 1], L[:, 2]); h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    w = np.clip((C - KG.ROT_C0) / (KG.ROT_C1 - KG.ROT_C0), 0, 1) * ((h > KG.ROT_H0) & (h < KG.ROT_H1))
    return L, C, h, w

def statistik(g4zeile, cnt):
    L, C, h, w = eintrag_lab(g4zeile)
    haut = (w == 0) & (cnt > 0) & (np.array(g4zeile) != 0)
    ader = (w > 0) & (cnt > 0)
    def wq(x, sel, q):
        if cnt[sel].sum() == 0: return float('nan')
        o = np.argsort(x[sel]); cs = np.cumsum(cnt[sel][o]); return float(x[sel][o][np.searchsorted(cs, q * cs[-1])])
    haut_d = haut & (L[:, 0] < L_HELL)
    return dict(cnt=np.asarray(cnt, float), Lm=wq(L[:, 0], haut, 0.5), Lv90=float(L[ader, 0].max()) if ader.any() else float('nan'), C75=wq(C, haut_d, 0.75))

L_HELL = 52.4

def umfaerben(g4zeile, par, st):
    """par: h (Grad), C (Haut-Buntheit am Hautmedian), beta (Buntheit dunkler Haut), g (Anteil G4-Hautbuntheit), smax,
    q (Aderhelligkeit <= q x Hautmedian), kappa (Ader-Buntheit <= kappa x mittlere Haut-Buntheit, gemessen an den 15-Bit-Farben)."""
    L, C, h, w = eintrag_lab(g4zeile)
    Lm, Lv90 = st['Lm'], st['Lv90']
    q, kap, gam = par.get('q', 1.0), par.get('kappa', 1.0), par.get('g', 1.0)
    cnt = st['cnt']
    z0 = np.array(g4zeile) != 0
    def s(Lx):
        return np.clip(1 + par['beta'] * (Lm - Lx) / Lm, 0, par.get('smax', 3.0))
    hr = math.radians(par['h'])
    behalten = (w == 0) & (L[:, 0] >= L_HELL)
    # Haut: L* bleibt, Farbton h, Buntheit C x s(L) x G4-Hautbuntheit / C75
    Ls = L[:, 0]
    Cs = par['C'] * s(Ls) * ((1 - gam) + gam * C / st['C75'])
    Xs = np.stack([Ls, Cs * math.cos(hr), Cs * math.sin(hr)], 1)
    rgb_s = KG.rgb_aus_lab(Xs)
    Lab_s = KG.lab(KG.wort_rgb([KG.wort(x) for x in rgb_s]))          # tatsaechliche 15-Bit-Farbe
    haut = (w == 0) & (cnt > 0) & ~behalten & z0
    C_haut = float((np.hypot(Lab_s[haut, 1], Lab_s[haut, 2]) * cnt[haut]).sum() / max(cnt[haut].sum(), 1))
    # Ader: L* <= q x Hautmedian (Rangfolge unter den Adern bleibt), Buntheit <= kappa x mittlere Haut-Buntheit
    Lv = np.minimum(L[:, 0], q * Lm)                 # Ader: G4-L*, aber nie heller als q x Hautmedian
    Cv = np.minimum(par['C'] * s(Lv), kap * C_haut)
    Ln = (1 - w) * Ls + w * Lv
    Cn = (1 - w) * Cs + w * Cv
    ad = w > 0                                   # Deckel nach der Mischung: auch Eintraege im Rampenbereich sind Ader
    Ln[ad] = np.minimum(Ln[ad], q * Lm)
    Cn[ad] = np.minimum(Cn[ad], kap * C_haut)
    X = np.stack([Ln, Cn * math.cos(hr), Cn * math.sin(hr)], 1)
    rgb = KG.rgb_aus_lab(X)
    neu = list(g4zeile)
    for i in range(256):
        if g4zeile[i] == 0 or behalten[i]:
            continue
        wn = KG.wort(rgb[i]) | (g4zeile[i] & 0x8000)
        if par.get('boden') and w[i] > 0:
            wn = 0x0421 | (g4zeile[i] & 0x8000)      # Boden-Probe: Ader = dunkelstes unbuntes 15-Bit-Grau (8,8,8)
        neu[i] = wn if wn != 0 else 0x8000      # 0x0000 = transparent (re15_tim_texel_argb); Schwarz wie G4 als 0x8000
    return neu
