"""Referenzzaehlung je CLUT-Zeile und Eintrag am Asset (16 Zeilen): je Flaeche das UV-Polygon im Texelraster (cv2.fillPoly, Viereck-Ecken 0,1,3,2),
Eintraege = TIM-Indizes der bedeckten Texel. Ergebnis: je (Zeile, Eintrag) Texelzahl (Summe ueber Flaechen), Flaechenzahl, Meshes; je Zeile die
unbenutzten Eintraege (von keiner Flaeche dieser Zeile beruehrt) und die Farbe (RGB555<<3, Lab)."""
import json, sys
import numpy as np, cv2
import v4f


def flaechen_texel(ordner):
    k = v4f.kand(ordner)
    px = k.tex.px
    out = []
    for b, viereck, fi, z, uv in v4f.emd_flaechen(ordner):
        po = (uv[5] & 0xF) * 128
        if viereck:
            pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[9] + po, uv[10]), (uv[6] + po, uv[7])]
        else:
            pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
        m = np.zeros(px.shape, np.uint8)
        cv2.fillPoly(m, [np.array(pts, np.int32)], 1)
        cv2.polylines(m, [np.array(pts, np.int32)], True, 1)
        ys, xs = np.nonzero(m)
        out.append(dict(mesh=b, viereck=viereck, fi=fi, zeile=int(z), xs=xs, ys=ys, idx=px[ys, xs]))
    return out


def tabelle(ordner):
    k = v4f.kand(ordner)
    F = flaechen_texel(ordner)
    nz = k.tex.lut.shape[0]
    tex = np.zeros((nz, 256), np.int64)
    fl = [[set() for _ in range(256)] for _ in range(nz)]
    me = [[set() for _ in range(256)] for _ in range(nz)]
    for i, f in enumerate(F):
        z = f['zeile'] if 0 <= f['zeile'] < nz else 0
        u, c = np.unique(f['idx'], return_counts=True)
        tex[z, u] += c
        for e in u:
            fl[z][e].add(i)
            me[z][e].add(f['mesh'])
    return F, tex, fl, me


if __name__ == '__main__':
    ordner = v4f.ST[sys.argv[1]] if sys.argv[1] in v4f.ST else sys.argv[1]
    F, tex, fl, me = tabelle(ordner)
    k = v4f.kand(ordner)
    lut = k.tex.lut
    res = {}
    for z in range(tex.shape[0]):
        benutzt = np.nonzero(tex[z])[0]
        frei = [int(e) for e in range(256) if tex[z, e] == 0]
        nflaechen = len({f_ for e in benutzt for f_ in fl[z][e]})
        meshes = sorted({m for e in benutzt for m in me[z][e]})
        L = v4f.lab(lut[z][benutzt][:, :3]) if len(benutzt) else np.zeros((0, 3))
        res[z] = dict(flaechen=nflaechen, meshes=meshes, eintraege_benutzt=int(len(benutzt)), eintraege_frei=len(frei), frei=frei,
                      texel=int(tex[z].sum()), L_mittel=round(float((L[:, 0] * tex[z, benutzt]).sum() / max(tex[z, benutzt].sum(), 1)), 1) if len(benutzt) else None)
        print('Zeile %2d: %4d Flaechen, Meshes %s, benutzt %3d, frei %3d, Texel %6d, L* (texelgewichtet) %s' % (
            z, nflaechen, meshes, len(benutzt), len(frei), tex[z].sum(), res[z]['L_mittel']))
    json.dump(res, open(sys.argv[2], 'w'), indent=1)
    np.save(sys.argv[2].replace('.json', '_texel.npy'), tex)
