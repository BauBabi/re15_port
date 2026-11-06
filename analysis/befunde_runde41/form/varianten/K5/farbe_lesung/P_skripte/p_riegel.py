"""Riegel MESSBLATT_FARBE_V1: misst eine Variante (Ordner mit FORM.EMD, FORM.TIM, form.json) gegen alle Riegel A1-A12 (Atlas) und K1-K10 (Kette).
Aufruf: python p_riegel.py <Variantenordner> [--familie]   (--familie: Atlas-Riegel zusaetzlich an EM30/G4/G5/Hund/Zombie als Plausibilitaet)
Ausgabe: Tabelle + riegel_<name>.json neben dem Skript."""
import sys, os, json, math
import numpy as np, cv2
from p_lade import var, lab, sammeln, em30, re2, re15typ
from p_messen import atlas, rect, km, hf_fft, hf_rest, dichte
from p_chroma_korn import korn

# Quellen der Grenzwerte: MESSBLATT_FARBE_V1.md Abschnitt 6 (Familie = EM30_HANGAR Haut, RE2-G4 0x34, RE2-G5 0x36, RE1.5 Hund 0x20, Zombie 0x10).
ORIG = None  # wird aus p_zonen gerechnet (Original Bild 1/2), nicht eingetragen


def hue(L):
    return math.degrees(math.atan2(L[2], L[1]))


def atlas_zonen(mo):
    per, tri = sammeln(mo); rgb, mid = atlas(mo, per); h, w = mid.shape; u = mid >= 0; xs = np.arange(w)[None, :]
    z = dict(koerper=u & (xs < 384))
    if w > 384:
        z.update(kopf=u & rect(h, w, (384, 0, 128, 104)), kiefer=u & rect(h, w, (384, 106, 128, 72)), auge=u & rect(h, w, (384, 196, 24, 10)),
                 kralle=u & rect(h, w, (410, 180, 100, 40)))
    return per, tri, rgb, mid, z


def atlas_riegel(mo, name):
    per, tri, rgb, mid, Z = atlas_zonen(mo)
    r = {}
    zk = Z['koerper']; L = lab(rgb[zk]); C = np.hypot(L[:, 1], L[:, 2]); hu = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    r['A1 Chroma Koerper'] = (round(float(C.mean()), 1), 9.5 <= C.mean() <= 21)
    sab = []
    for b in np.unique(mid[zk]):
        s = zk & (mid == b)
        if s.sum() >= 200:
            Lb = lab(rgb[s]); sab.append(math.sqrt(Lb[:, 1].var() + Lb[:, 2].var()))
    r['A2 Farbstreuung je Teil sigma_ab'] = (round(float(np.mean(sab)), 1), np.mean(sab) >= 7.0)
    rot = (L[:, 1] > 15) & (C > 20) & (hu > -40) & (hu < 70)
    r['A3 Fleischrot-Anteil %'] = (round(100 * float(rot.mean()), 1), 8 <= 100 * rot.mean() <= 40)
    hr = hue(L[rot].mean(0)) if rot.sum() > 20 else float('nan')
    r['A4 Farbton Fleischrot Grad'] = (round(hr, 0), 15 <= hr <= 50)
    nw = (L[:, 0] > 85) & (C < 10)
    r['A5 Neutralweiss Koerper %'] = (round(100 * float(nw.mean()), 2), 100 * nw.mean() <= 0.20)
    Lmap = np.zeros(mid.shape)
    for k, z in Z.items():
        Lmap[z] = lab(rgb[z])[:, 0]
    hf = {k: hf_fft(Lmap, mid, z) for k, z in Z.items() if k != 'auge'}
    r['A6 HF_fft je Feld (max)'] = ({k: round(v, 3) for k, v in hf.items() if v is not None}, all(0.04 <= v <= 0.26 for v in hf.values() if v is not None))
    hr2 = {k: hf_rest(Lmap, z) for k, z in Z.items() if k != 'auge' and z.sum() > 200}
    r['A7 HF_rest je Feld (max)'] = ({k: round(v, 3) for k, v in hr2.items()}, all(v <= 0.19 for v in hr2.values()))
    if 'kiefer' in Z:
        Lk = lab(rgb[Z['kiefer']]); Ck = np.hypot(Lk[:, 1], Lk[:, 2])
        zahn = 100 * float(((Lk[:, 0] > 60) & (Ck < 35)).mean()); rr = 100 * float((Lk[:, 1] > 20).mean()); dd = 100 * float((Lk[:, 0] < 20).mean())
        r['A8 Kiefer Zahn/Rot/Dunkel %'] = ((round(zahn, 1), round(rr, 1), round(dd, 1)), 2 <= zahn <= 20 and rr >= 20 and dd >= 20)
        La = lab(rgb[Z['auge']]).mean(0)
        r['A9 Auge L*/C*'] = ((round(float(La[0]), 1), round(float(np.hypot(La[1], La[2])), 1)), La[0] <= 30 and np.hypot(La[1], La[2]) <= 30)
    dm, df = dichte(tri, [b for b in per])
    dk = dichte(tri, [b for b in per if b not in (22, 23)])[0]
    teile = {}
    for b in per:
        try:
            teile[b] = dichte(tri, [b])[0] / dk
        except Exception:
            pass
    r['A10 Texeldichte Teil/Koerper (min,max)'] = ((round(min(teile.values()), 2), round(max(teile.values()), 2)), 0.5 <= min(teile.values()) and max(teile.values()) <= 2.7)
    if len(per) > 6:
        xs = np.arange(mid.shape[1])[None, :] < 384
        g = [float(lab(rgb[(mid == b) & xs])[:, 0].mean()) for b in range(1, 7)]
        spr = [abs(g[i + 1] - g[i]) for i in range(4)]
        r['A11 Schwanz L*-Sprung Glied 1-5 (max)'] = (round(max(spr), 1), max(spr) <= 4.0)
    fk = korn(rgb, Z['koerper'])
    r['A12 Farbkorn Koerper (a*b*-Rest)'] = (round(fk, 2), fk <= 5.7)
    return r


def kette_riegel(vdir):
    import p_zonen as PZ
    out = {}
    O = {}
    for n in (1, 2):
        v = cv2.cvtColor(cv2.imread(PZ.FORM + '/bild%d.png' % n), cv2.COLOR_BGR2RGB)
        T, clip = PZ.rueck(v, n); O[n] = PZ.messe(T, clip, PZ.regionen(n)); O[n]['_T'] = T
    k = PZ.P.Kandidat(vdir + '/form.json')
    V = {}
    for n in (1, 2):
        sauber, alb, m, kk = k.rendern(n)
        fig = m & (alb.max(2) > 0)
        vid = PZ.FV.video_filter(sauber, bild=n, entzerren=False, rauschen=True)
        T, clip = PZ.rueck(vid, n); V[n] = PZ.messe(T, clip, PZ.regionen(n), fig); V[n]['_T'] = T

    def dE(a, b):
        return round(float(np.linalg.norm(np.array(a) - np.array(b))), 1)
    o1, v1, o2, v2 = O[1]['Figur'], V[1]['Figur'], O[2]['Figur'], V[2]['Figur']
    out['K1 Bild1 Chroma Figur / Original'] = ((v1['chroma'], o1['chroma']), 0.85 * o1['chroma'] <= v1['chroma'] <= 1.2 * o1['chroma'])
    out['K2 Bild1 ab-Streuung Figur / Original'] = ((v1['ab_streu'], o1['ab_streu']), v1['ab_streu'] >= 0.85 * o1['ab_streu'])
    z1 = {z: dE(V[1][z]['lab'], O[1][z]['lab']) for z in O[1] if not z.startswith('_') and z != 'Figur'}
    out['K3 Bild1 dE je Zone (max)'] = (z1, max(z1.values()) <= 6)
    out['K4 Bild1 Rot-Klasse Figur % / Original'] = ((v1['kl_p']['R'], o1['kl_p']['R']), 0.6 * o1['kl_p']['R'] <= v1['kl_p']['R'] <= 1.5 * o1['kl_p']['R'])
    rh, oh = V[1]['Ruecken/Flanke']['kl_p']['H'], O[1]['Ruecken/Flanke']['kl_p']['H']
    out['K5 Bild1 Hell-Klasse Ruecken % / W Figur %'] = ((rh, oh, v1['kl_b']['W'], o1['kl_b']['W']), rh <= 1.5 * oh and v1['kl_b']['W'] <= 1.5 * o1['kl_b']['W'])
    out['K6 Bild2 Figur dE / Chroma'] = ((dE(v2['lab'], o2['lab']), v2['chroma'], o2['chroma']), dE(v2['lab'], o2['lab']) <= 6 and v2['chroma'] >= 0.85 * o2['chroma'])
    uk, ouk = V[2]['Unterkiefer'], O[2]['Unterkiefer']
    out['K7 Bild2 Unterkiefer dE / a*'] = ((dE(uk['lab'], ouk['lab']), uk['lab'][1], ouk['lab'][1]), dE(uk['lab'], ouk['lab']) <= 6 and uk['lab'][1] >= 0.85 * ouk['lab'][1])
    ok_, ook = V[2]['Oberkiefer'], O[2]['Oberkiefer']
    out['K8 Bild2 Oberkiefer dE'] = (dE(ok_['lab'], ook['lab']), dE(ok_['lab'], ook['lab']) <= 6)
    kr, okr = V[2]['Klaue r (W)'], O[2]['Klaue r (W)']; kl, okl = V[2]['Klaue l (W)'], O[2]['Klaue l (W)']
    out['K9 Bild2 Klauen a* (r, l) / Original'] = ((kr['lab'][1], okr['lab'][1], kl['lab'][1], okl['lab'][1]),
                                                   kr['lab'][1] >= 0.75 * okr['lab'][1] and kl['lab'][1] >= 0.75 * okl['lab'][1])
    yy, xx = np.mgrid[:480, :640]; aug = []
    for (x, y) in ((223, 271), (288, 269)):   # E24 Augenpunkte
        mm = (xx - x) ** 2 + (yy - y) ** 2 <= 16
        aug.append(round(float(lab(np.clip(V[2]['_T'][mm], 0, 255))[:, 0].max()), 1))
    out['K10 Bild2 Augenpunkte L*max (r, l)'] = (tuple(aug), max(aug) <= 30)
    return out


if __name__ == '__main__':
    vdir = sys.argv[1]
    name = os.path.basename(os.path.normpath(vdir))
    res = {}
    print('== Atlas', name)
    for k, (v, ok) in atlas_riegel(var(vdir), name).items():
        print('  %-42s %-60s %s' % (k, v, 'GRUEN' if ok else 'ROT')); res[k] = dict(wert=str(v), ok=bool(ok))
    print('== Kette', name)
    for k, (v, ok) in kette_riegel(vdir).items():
        print('  %-42s %-60s %s' % (k, v, 'GRUEN' if ok else 'ROT')); res[k] = dict(wert=str(v), ok=bool(ok))
    if '--familie' in sys.argv:
        for fn, f in (('EM30', em30), ('G4', lambda: re2(0x34)), ('G5', lambda: re2(0x36)), ('Hund', lambda: re15typ(0x20)), ('Zombie', lambda: re15typ(0x10))):
            print('== Familie', fn, '(nur A1-A7; Zonen hier: Koerper = alle genutzten Texel)')
            mo = f(); per, tri = sammeln(mo); rgb, mid = atlas(mo, per); u = mid >= 0
            L = lab(rgb[u]); C = np.hypot(L[:, 1], L[:, 2]); hu = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
            rot = (L[:, 1] > 15) & (C > 20) & (hu > -40) & (hu < 70)
            print('   A1 %.1f  A3 %.1f %%  A4 %.0f Grad  A5 %.2f %%' % (C.mean(), 100 * rot.mean(), hue(L[rot].mean(0)), 100 * ((L[:, 0] > 85) & (C < 10)).mean()))
    json.dump(res, open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'riegel_%s.json' % name), 'w'), indent=1, ensure_ascii=False)
