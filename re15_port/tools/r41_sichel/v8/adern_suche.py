"""SF V8: Auswahl der Mesh-8-Flaechen fuer die Ader-Zeilen und UV-Schub je UV-Insel -> adern_v8.json (Messblatt V8 6c).

    python adern_suche.py [--basis EMD] [--kette JSON] [--ruecken JSON] [--aus JSON]

Flaechen (PORT-WAHL, Nutzer "die Rechte Seite ... mehr adern ... am Arm und Koerper"; Seite gemessen: (W)-rechts = Knochenraum-8 z < 0,
v8_gemein Kopf): Mesh 8, Schwerpunkt z < 0, CLUT-Zeile 480 oder 481 (Haut), in PV (V8-Form, Stufe s + r) mit >= 10 Bildpunkten sichtbar;
dazu die in PV sichtbaren Flaechen am Huegel H (Bild x 425-445, y 184-202; Messblatt 6b "rot (Ader-Zeile)").
UV-Inseln: Flaechen derselben Zeile und Textur-Seite, die eine UV-Ecke teilen (gleiche u, v), bilden eine Insel; je Insel EIN Schub
(du, dv) - die Insel behaelt ihre Form (keine neuen Naehte innerhalb der Insel).
Schub: unter allen ganzzahligen (du, dv), fuer die die verschobene Texelflaeche (KB.raster) ganz in der Seite liegt (u, v 0-255) und
nur Texel trifft, die (a) im Original-EM30 von Flaechen derselben Zeile benutzt werden (m17_adern.npz 'zeile'), (b) kein Flaechen-Rot
sind (Wunde/Augapfel: m17 'blob'), (c) nicht im Schulter-Augapfel C liegen (Seite 0, u 78-127, v 0-100; Nutzer V5: kein roter Fleck
auf der Schulter), (d) in V7 Original-Texel sind - der Schub mit den meisten Ader-Texeln (m17 'ader'); bei Gleichstand der kleinste
|du| + |dv|. Inseln, deren bester Schub keinen Ader-Texel mehr bringt als der Ort heute, bleiben, wo sie sind (nur Zeilenwechsel).
Kein RE-Wert; das Ergebnis ist PORT-WAHL.
"""
import os, sys, json, argparse
import numpy as np
from scipy.signal import fftconvolve
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402
import f_rast as FR                   # noqa: E402

H_FENSTER = (425, 184, 445, 202)      # Huegel H im Bild (Messblatt 3.2: H x 420-445, y 190-206; Kontur V8 dort 184-200)
C_FENSTER = (78, 0, 127, 100)         # Messblatt 6c Bereich C (Seite 0)


def schluessel(m):
    return [('d%d' % i, u) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u) for i, u in enumerate(m['quv'])]


def ecken_uv(uv):
    n = 3 if len(uv) == 9 else 4
    po = (uv[5] & 0xF) * 128
    return [(uv[3 * k] + po, uv[3 * k + 1]) for k in range(n)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=V.ASSET)
    ap.add_argument('--kette', default=os.path.join(HIER, 'kette_v8.json'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--aus', default=os.path.join(HIER, 'adern_v8.json'))
    ap.add_argument('--je-flaeche', action='store_true', help='jede Flaeche eine eigene Insel (ohne gemeinsame UV-Ecken)')
    ap.add_argument('--ruecken-hinten', action='store_true', help='auch sichtbare Flaechen der Rueckenhaelfte x < 0 (Messblatt-Region S7c)')
    ap.add_argument('--max-texel', type=int, default=0, help='Inseln groesser als so viele Texel in zusammenhaengende Teilinseln teilen')
    a = ap.parse_args()
    import v8_bau as VB
    basis = open(a.basis, 'rb').read()
    blob, _p = VB.bauen(a.basis, 'sr', kette=json.load(open(a.kette)), ruecken=json.load(open(a.ruecken)))
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    _oo, _om, to = AV.original()
    Z = np.load(os.path.join(V.MESS, 'm17_adern.npz'))
    ader, blobm, zeile = Z['ader'], Z['blob'], Z['zeile']
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV)
    bone, fl = A['bone'][:480], A['flaeche'][:480]
    m8 = ms[8]
    P = np.array([v[:3] for v in m8['v']], float)
    px = {}
    for k in np.unique(fl[bone == 8]):
        px[k] = int(((bone == 8) & (fl == k)).sum())
    x0, y0, x1, y1 = H_FENSTER
    hfl = set(np.unique(fl[y0:y1 + 1, x0:x1 + 1][bone[y0:y1 + 1, x0:x1 + 1] == 8]))
    wahl = []
    for key, uv in schluessel(m8):
        z = (uv[2] >> 6) & 0x1FF
        if z not in (480, 481):
            continue
        f = m8['qf'][int(key[1:])] if key[0] == 'v' else m8['tf'][int(key[1:])]
        zs = float(np.mean([P[i][2] for i in f[1::2]]))
        xs_ = float(np.mean([P[i][0] for i in f[1::2]]))
        if ((zs < 0 or (a.ruecken_hinten and xs_ < 0)) and px.get(key, 0) >= 10) or key in hfl:
            wahl.append(dict(f=key, zeile=z, z_schwer=round(zs, 1), x_schwer=round(xs_, 1), pv_px=px.get(key, 0), huegel=key in hfl))
    # Inseln
    uvs = {w['f']: AV.flaeche(m8, w['f']) for w in wahl}
    eltern = {w['f']: w['f'] for w in wahl}

    def wurzel(k):
        while eltern[k] != k:
            k = eltern[k]
        return k
    for i, wa in enumerate(wahl):
        for wb in wahl[i + 1:]:
            ua, ub = uvs[wa['f']], uvs[wb['f']]
            if not a.je_flaeche and wa['zeile'] == wb['zeile'] and (ua[5] & 0xF) == (ub[5] & 0xF) and set(ecken_uv(ua)) & set(ecken_uv(ub)):
                eltern[wurzel(wa['f'])] = wurzel(wb['f'])
    inseln = {}
    for w in wahl:
        inseln.setdefault(wurzel(w['f']), []).append(w)
    if a.max_texel:
        # Inseln ueber MAX_TEXEL in Teilinseln zerlegen: in fester Reihenfolge (Flaechenschluessel) Nachbarn (gemeinsame UV-Ecke) anlagern,
        # bis die Texelzahl die Grenze erreicht; jede Teilinsel bleibt zusammenhaengend
        def texel(fs):
            mk = np.zeros(ader.shape, bool)
            for f_ in fs:
                uv = uvs[f_['f']]
                mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            return int(mk.sum())
        neu_i = {}
        for wz, fs in sorted(inseln.items()):
            rest = sorted(fs, key=lambda f_: (f_['f'][0], int(f_['f'][1:])))
            while rest:
                teil = [rest.pop(0)]
                gewachsen = True
                while gewachsen:
                    gewachsen = False
                    for f_ in list(rest):
                        if any(set(ecken_uv(uvs[f_['f']])) & set(ecken_uv(uvs[g_['f']])) for g_ in teil) and texel(teil + [f_]) <= a.max_texel:
                            teil.append(f_); rest.remove(f_); gewachsen = True
                neu_i[teil[0]['f']] = teil
        inseln = neu_i
    ungueltig_basis = (blobm | (t['pix'] != to['pix']))
    cx0, cy0, cx1, cy1 = C_FENSTER
    ungueltig_basis[cy0:cy1 + 1, cx0:cx1 + 1] = True
    out = []
    for ins in sorted(inseln.values(), key=lambda l: l[0]['f']):
        zz = ins[0]['zeile']
        page = (uvs[ins[0]['f']][5] & 0xF) * 128
        mk = np.zeros(ader.shape, bool)
        for w in ins:
            uv = uvs[w['f']]
            mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, page)
        ys, xs = np.nonzero(mk)
        ung = ungueltig_basis | (zeile != zz)
        ung = ung.copy(); ung[:, :page] = True; ung[:, page + 256:] = True
        heute = int(ader[mk].sum())
        # Korrelation: Schub (dv, du) gueltig, wenn die verschobene Maske keinen ungueltigen Texel trifft
        k = mk[ys.min():ys.max() + 1, xs.min():xs.max() + 1].astype(float)
        kk = k[::-1, ::-1]
        S_ung = fftconvolve(ung.astype(float), kk, mode='valid')
        S_ad = fftconvolve(ader.astype(float), kk, mode='valid')
        ok = S_ung < 0.5
        best = None
        for (vy, vx) in zip(*np.nonzero(ok)):
            dv, du = int(vy - ys.min()), int(vx - xs.min())
            ua = [AV.flaeche(m8, w['f']) for w in ins]
            if any(not (0 <= u[3 * j] + du <= 255 and 0 <= u[3 * j + 1] + dv <= 255) for u in ua for j in range(3 if len(u) == 9 else 4)):
                continue
            s = int(round(S_ad[vy, vx]))
            key = (s, -(abs(du) + abs(dv)))
            if best is None or key > best[0]:
                best = (key, du, dv)
        du, dv, neu = 0, 0, heute
        if best is not None and best[0][0] > heute:
            du, dv, neu = best[1], best[2], best[0][0]
        for w in ins:
            out.append(dict(w, du=du, dv=dv, insel=ins[0]['f'], insel_texel=int(mk.sum()), ader_heute=heute, ader_neu=neu))
        print('Insel %s (%d Flaechen, Zeile %d, %d Texel): Ader %d -> %d, Schub (%d, %d)' % (ins[0]['f'], len(ins), zz, mk.sum(), heute, neu, du, dv))
    res = dict(quelle='adern_suche.py (Basis %s sha256 %s, Kette %s, Ruecken %s)' % (os.path.basename(a.basis), V.sha(basis),
                                                                                  os.path.basename(a.kette), os.path.basename(a.ruecken)),
               mesh8=out)
    json.dump(res, open(a.aus, 'w'), indent=1)
    print('%d Flaechen in %d Inseln' % (len(out), len(inseln)))


if __name__ == '__main__':
    main()
