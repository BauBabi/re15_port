"""Zahlen-Pruefer V4 r5: eigene Messung der neuen Riegel TN / TV / TR / AF / FR (Entscheide O8-O10) - eigener Code, nicht r5_mass/riegel_v4.
Benutzt nur Bild-/Datenzugriff (b2-Renders aus pz5_render, Vorlagen, Leser-Masken, BSS-Hintergrund v4_gemein.hintergrund, FRAMEDUMP ueber
nb2neu/c6.port); Mittellinie, Stationen, Farbmengen, Figurmaske, Zaehlungen hier neu geschrieben.
  TN  (a) Wortlaut: je Spalte x 280-376 Maskenmitte (y < 262), Lab 3x3, max dE76 ueber 3 Spalten; (b) Bandmittel je Spalte (erodierte Maske);
      (c) entlang der eigenen geodaetischen Mittellinie (pz5_mess.schwanz) ab dem Beginn, Lab 3x3, Sprung ueber 3 Punkte.
  TV  Stationen auf dem Polygonzug Beginn-Biegung-Knick-Ende (Wurzel 0,15 / Mitte 0,5 des 1. Abschnitts, Biegung, 0,5 / 0,5 der beiden anderen),
      auf den naechsten Maskenpunkt gezogen, Kreis r 0,02 H in der erodierten Maske.
  TR  Anteil a* > 12 in x 284-356, y < 262 (Maske 3x3 erodiert): (a) Wortlaut Port Videoraum / Original roh; (b) beide roh (Port a + b P ohne
      Gauss); (c) beide weich (Original ebenfalls Gauss 1,5).
  AF  Figurmaske im Original = |Vorlage - Hintergrund (BSS in der Eichung)| > 12 dE76 (geoeffnet); Farbmenge je Mesh = Figurpunkte im Umkreis
      15 px um die Mesh-Bildpunkte (A1, ohne Hintergrund) bzw. alle Figurpunkte (A2); Port-Bildpunkte V = a + b P (ohne Gauss); dE76 > 10.
  AFT Texel-Ebene: Texel der UV-Polygone je Koerper-Mesh ueber die CLUT-Zeile, gelb = b* > 20 und a* < 12 und L* > 25 (unbeleuchtet).
  FR  a* > 22 im Spielbild (eigener Lauf pz5/lauf) h85/h100 Fenster, h118/h135 Polygone (r4_mass.PS_/PV_SPIEL), Video gleich; Spielbild
      Videoraum (Wortlaut) und roh."""
import os, sys, json, re
import numpy as np, cv2
from scipy import ndimage as nd
from scipy.spatial import cKDTree
HIER = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, HIER)
import pz5_mess as PM
from pz5_mess import vraum, lab, orig, SP
import v4_gemein as G
import t2_mass_v4 as T2M
from pz5_g1 import secs, md1, tim
S = SP.rsplit('pz5', 1)[0]
EA, EB = PM.EA_, PM.EB_
out = {}


def maske(f):
    d = np.load(f)
    return np.unpackbits(d['m'])[:int(np.prod(d['shape']))].reshape(tuple(d['shape'])).astype(bool)


MO = [maske(f) for f in T2M.MASKEN]
O2 = orig(2)
O1 = orig(1)
yy, xx = np.mgrid[:480, :640]
BOX = (xx >= 284) & (xx <= 356) & (yy < 262)


def tn(V, m):
    L = lab(V)
    a, b = [], []
    me = nd.binary_erosion(m)
    for x in range(280, 377):
        ys = np.nonzero(m[:262, x])[0]
        if len(ys) < 5:
            continue
        y = int((ys.min() + ys.max()) / 2)
        a.append((x, L[y - 1:y + 2, x - 1:x + 2].reshape(-1, 3).mean(0)))
        ye = np.nonzero(me[:262, x])[0]
        if len(ye):
            b.append((x, L[ye, x].mean(0)))
    r = {}
    for nm, v in (('mitte', a), ('band', b)):
        xs = [q[0] for q in v]
        c = np.array([q[1] for q in v])
        d = np.linalg.norm(c[3:] - c[:-3], axis=1)
        r[nm] = (round(float(d.max()), 1), xs[int(np.argmax(d)) + 1])
    return r


def tr(V, m):
    me = cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & BOX
    return round(float((lab(V[me])[:, 1] > 12).mean()), 3), int(me.sum())


out['TN_orig'] = [tn(O2, m) for m in MO]
O2g = cv2.GaussianBlur(O2.astype(np.float32), (0, 0), 1.5).astype(float)
out['TN_orig_weich'] = [tn(O2g, m) for m in MO]
out['TR_orig'] = dict(roh=[tr(O2, m) for m in MO], weich=[tr(O2g, m) for m in MO])
M = json.load(open(SP + 'mess_V4_V4R4_V3.json'))
for nm in ('V4', 'V4R4', 'V3'):
    d = np.load(SP + 'r_%s_85.npz' % nm)
    img, ids = d['img'], d['ids']
    sw = (ids >= 16) & (ids <= 22)
    V = vraum(img)
    Vr = np.clip(EA + EB * img.astype(float), 0, 255)
    o = dict(TN=tn(V, sw), TN_roh=tn(Vr, sw), TR=dict(wortlaut=tr(V, sw), roh=tr(Vr, sw)))
    T = M[nm]['seite']['T']
    P = np.array(T['_P'])
    L = lab(V)
    beg = np.array(T['beginn'], float)
    j0 = int(np.argmin(np.linalg.norm(P - beg, axis=1)))
    me = nd.binary_erosion(sw)
    c = []
    for x, y in np.round(P).astype(int):
        mm = me[y - 1:y + 2, x - 1:x + 2]
        c.append(L[y - 1:y + 2, x - 1:x + 2][mm].mean(0) if mm.any() else L[y, x])
    c = np.array(c)
    dd = np.linalg.norm(c[3:] - c[:-3], axis=1)
    o['TN_achse'] = (round(float(dd[j0:].max()), 1), [round(float(v)) for v in P[j0 + int(np.argmax(dd[j0:])) + 1]])
    out[nm] = o


def stationen(V, m, pk, H):
    L = lab(V)
    me = nd.binary_erosion(m)
    ys, xs = np.nonzero(me)
    pts = np.c_[xs, ys].astype(float)
    kd = cKDTree(pts)
    pk = [np.array(p, float) for p in pk]
    st = dict(Wurzel=pk[0] + 0.15 * (pk[1] - pk[0]), Mitte=pk[0] + 0.5 * (pk[1] - pk[0]), Biegung=pk[1],
              senkrecht=pk[1] + 0.5 * (pk[2] - pk[1]), Endstueck=pk[2] + 0.5 * (pk[3] - pk[2]))
    r = {}
    for k, p in st.items():
        _, i = kd.query(p)
        c = pts[i]
        mm = me & (np.hypot(xx - c[0], yy - c[1]) <= 0.02 * H)
        r[k] = [round(float(v), 1) for v in L[mm].mean(0)] + [int(mm.sum()), [int(c[0]), int(c[1])]]
    return r


out['TV_orig'] = [stationen(O2, m, [(357, 243), (295.0, 252.1), (290, 308), (295, 343)], 253.0) for m in MO]
for nm in ('V4', 'V4R4'):
    d = np.load(SP + 'r_%s_85.npz' % nm)
    img, ids = d['img'], d['ids']
    sch = json.load(open(SP + 'schwanz_%s.json' % nm))
    T = M[nm]['seite']['T']
    pk = [sch['Id16']['p'], sch['Id16']['T2']['biegung'], T['T3']['p'], T['T4']['p']]
    out[nm]['TV'] = stationen(vraum(img), (ids >= 16) & (ids <= 22), pk, M[nm]['seite']['H'])

# --- AF: Figurmaske im Original aus dem BSS-Hintergrund
bg = G.hintergrund().astype(np.float32)
bgv = np.clip(EA + EB * cv2.GaussianBlur(bg, (0, 0), 1.5), 0, 255)
fig = {}
for b, O in ((1, O1), (2, O2)):
    dE = np.linalg.norm(lab(cv2.GaussianBlur(O.astype(np.float32), (0, 0), 1.0)) - lab(bgv), axis=2)
    f = nd.binary_opening(dE > 12, iterations=2)
    n, lb, st_, _ = cv2.connectedComponentsWithStats(f.astype(np.uint8), connectivity=8)
    k = 1 + int(np.argmax(st_[1:, 4]))
    fig[b] = lb == k
    out['figur_%d' % b] = dict(px=int(fig[b].sum()), bbox=[int(st_[k, 0]), int(st_[k, 1]), int(st_[k, 0] + st_[k, 2]), int(st_[k, 1] + st_[k, 3])],
                               wand_dE_median=round(float(np.median(dE[150:300, 40:120])), 1))
    cv2.imwrite(SP + 'figur_orig_%d.png' % b, (fig[b] * 255).astype(np.uint8))


def gelb(Lp):
    return (Lp[..., 2] > 20) & (Lp[..., 1] < 12) & (Lp[..., 0] > 25)


def rgb15(v):
    v = v.astype(int)
    return np.stack([(v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3], -1)


P_ = G.posen()
import b2
for nm, pf in (('V4', SP + 'emd/EM30_V4.EMD'), ('V4R4', S + 'emd/EM30_V4r4.EMD'), ('V3', G.EMD_V3)):
    mo = b2.modell(pf)
    res = {}
    for ans, b in (('135', 1), ('85', 2)):
        img, ids = G.zeichne(mo, P_[ans])
        Pl = lab(np.clip(EA + EB * img.astype(float), 0, 255))
        Lo = lab(O1 if b == 1 else O2)
        fl = fig[b]
        alle = cKDTree(Lo[fl])
        r = {}
        for i in range(16):
            m = ids == i
            if m.sum() < 30:
                continue
            um = nd.binary_dilation(m, iterations=15) & fl
            d2, _ = alle.query(Pl[m])
            e = dict(n=int(m.sum()), A2=round(float((d2 > 10).mean()), 3), gelb=int((gelb(Pl) & m).sum()))
            if um.sum() >= 30:
                d1, _ = cKDTree(Lo[um]).query(Pl[m])
                e['A1'] = round(float((d1 > 10).mean()), 3)
                e['A1_p99'] = round(float(np.percentile(d1, 99)), 1)
            r[i] = e
        res[ans] = r
    out[nm]['AF'] = res
    B = open(pf, 'rb').read()
    sc = secs(B)
    ms = md1(sc[7])
    T_ = tim(sc[8])
    pix, clut, cy = T_['pix'], T_['clut'], T_['cy']
    aft = {}
    for i in range(23):
        m_ = ms[i]
        polys = []
        for q in m_['quv_raw']:
            polys.append((q[2] >> 6, q[5] & 0xf, [[q[0], q[1]], [q[3], q[4]], [q[9], q[10]], [q[6], q[7]]]))
        for t in m_['tuv_raw']:
            polys.append((t[2] >> 6, t[5] & 0xf, [[t[0], t[1]], [t[3], t[4]], [t[6], t[7]]]))
        z = {}
        for row, pg, pts in polys:
            zz = z.setdefault((row, pg), np.zeros((256, 256), np.uint8))
            cv2.fillPoly(zz, [np.array(pts, np.int32)], 1)
            cv2.polylines(zz, [np.array(pts, np.int32)], True, 1)
        cnt, tot, gl = {}, 0, []
        for (row, pg), zz in z.items():
            if row < cy or row - cy >= clut.shape[0]:
                continue
            idx = pix[:, pg * 128:pg * 128 + 256]
            sel = zz[:, :idx.shape[1]].astype(bool)
            iv = idx[sel]
            col = rgb15(clut[row - cy][iv]).astype(float)
            Lt = lab(col.reshape(-1, 1, 3)).reshape(-1, 3)
            g = gelb(Lt)
            tot += len(Lt)
            if g.any():
                cnt[row] = cnt.get(row, 0) + int(g.sum())
                gl.append(Lt[g])
        aft[i] = dict(texel=tot, gelb=cnt, gelb_lab=[round(float(v), 1) for v in np.vstack(gl).mean(0)] if gl else None)
    out[nm]['AFT'] = aft

# --- FR im Spielbild (eigener Lauf)
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
os.environ['NB6_RUNS'] = S + 'pz5'
import c6
import r4_mass as R4
txt = open(S + 'pz5/lauf/debug.log', encoding='latin-1').read()
fr = dict(off=c6.off('lauf'))
for h, fen in ((85, (300, 150, 480, 380)), (100, (300, 150, 480, 330)), (118, None), (135, None)):
    F = int(re.search(r'\[hangar-takt\] h%d F(\d+) ' % h, txt).group(1))
    vid = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB).astype(float) if h != 135 else O1
    if fen:
        mp = np.zeros((480, 640), bool)
        mp[fen[1]:fen[3], fen[0]:fen[2]] = True
        mv = mp
    else:
        mp = R4.poly_maske(R4.PS_SPIEL[h])
        mv = R4.poly_maske(R4.PV_SPIEL[h])
    e = dict(F_takt=F)
    for art, Fx in (('takt', F), ('off', None)):
        im = c6.port('lauf', h, F=Fx)
        im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB).astype(float)
        a_w = lab(vraum(im)[mp])[:, 1]
        a_r = lab(np.clip(EA + EB * im, 0, 255)[mp])[:, 1]
        e[art] = dict(videoraum=int((a_w > 22).sum()), roh=int((a_r > 22).sum()), a_p999=round(float(np.percentile(a_w, 99.9)), 1))
        if art == 'takt':
            cv2.imwrite(SP + 'fr_spiel_h%d.png' % h, cv2.cvtColor(np.clip(im, 0, 255).astype(np.uint8), cv2.COLOR_RGB2BGR))
    av = lab(vid[mv])[:, 1]
    e['video'] = int((av > 22).sum())
    e['video_weich'] = int((lab(cv2.GaussianBlur(vid.astype(np.float32), (0, 0), 1.5)[mv])[:, 1] > 22).sum())
    e['grenze'] = max(10 * e['video'], int(0.001 * mp.sum()))
    e['n'] = int(mp.sum())
    fr[h] = e
out['FR'] = fr
json.dump(out, open(SP + 'neu.json', 'w'), indent=1, default=str)
print('fertig')
