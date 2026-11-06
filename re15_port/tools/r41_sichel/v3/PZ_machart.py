# Zahlen-Pruefer V3 r1: R29-R32 mit eigenem Parser (PZ_emd): Dreiecke, Texeldichte, Palette/C*.
import sys, json
import numpy as np, cv2
sys.path.insert(0, __import__('os').path.dirname(__file__))
from PZ_emd import sektionen, md1, tim, uv_polys
from skimage.color import rgb2lab

def flaeche3(P):
    P = np.asarray(P, float)
    if len(P) == 3:
        return 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[2] - P[0]))
    return flaeche3(P[[0, 1, 2]]) + flaeche3(P[[1, 3, 2]])

def flaeche2(Q):
    Q = np.asarray(Q, float)
    if len(Q) == 3:
        return 0.5 * abs((Q[1, 0] - Q[0, 0]) * (Q[2, 1] - Q[0, 1]) - (Q[2, 0] - Q[0, 0]) * (Q[1, 1] - Q[0, 1]))
    return flaeche2(Q[[0, 1, 2]]) + flaeche2(Q[[1, 3, 2]])

def palette(T):
    c = T['clut'].astype(int)
    rgb = np.stack([(c & 31) * 255 // 31, (c >> 5 & 31) * 255 // 31, (c >> 10 & 31) * 255 // 31], -1)
    return rgb

def main(p):
    B = open(p, 'rb').read()
    s, _ = sektionen(B)
    M = md1(B[s[7][0]:s[7][1]]); T = tim(B[s[8][0]:])
    pal = palette(T); lab = rgb2lab(pal.astype(float) / 255.0)
    C = np.hypot(lab[..., 1], lab[..., 2])
    out = {}
    out['dreiecke'] = {i: len(m['tf']) + 2 * len(m['qf']) for i, m in enumerate(M)}
    out['dreiecke_schwanz'] = sum(out['dreiecke'][i] for i in range(16, len(M)))
    dichte = {}
    for i, m in enumerate(M):
        V = [v[:3] for v in m['tv']]
        At = Am = 0.0
        Ak = Amk = 0.0
        P = uv_polys(m)
        faces = [f[:3] for f in m['tf']] + [f[:4] for f in m['qf']]
        # Faces: tf = (n0,v0,n1,v1,n2,v2)? Pruefen ueber Layout: EA nutzt tf[1],tf[3],tf[5] als Vertex-Indizes
        vids = [(f[1], f[3], f[5]) for f in m['tf']] + [(f[1], f[3], f[5], f[7]) for f in m['qf']]
        for (uvs, clut, tp), vi in zip(P, vids):
            a3 = flaeche3([V[j] for j in vi]) if len(vi) == 3 else flaeche3([V[vi[0]], V[vi[1]], V[vi[2]], V[vi[3]]])
            a2 = flaeche2(uvs)
            At += a2; Am += a3
            if (tp & 0xF) == 0 and min(v for _, v in uvs) >= 156:
                Ak += a2; Amk += a3
        dichte[i] = round(float(np.sqrt(At / Am)), 3) if Am else None
        if Amk:
            out['kopfbild_dichte_mesh%d' % i] = round(float(np.sqrt(Ak / Amk)), 3)
    out['dichte'] = dichte
    # Texel der Schwanz-UVs: Zeilen und C*
    H, W = T['px'].shape
    def texel_werte(meshes):
        vals = []
        for i in meshes:
            for uvs, clut, tp in uv_polys(M[i]):
                row = (clut >> 6) & 0x1FF; ox = ((tp & 0xF) * 64 - T['ix']) * 2
                msk = np.zeros((H, W), np.uint8)
                cv2.fillPoly(msk, [np.array([[u + ox, v] for u, v in uvs], np.int32)], 1)
                idx = T['px'][msk.astype(bool)]
                vals.append((row - T['cy'], idx))
        return vals
    ts = texel_werte(range(16, len(M)))
    out['schwanz_zeilen'] = sorted({int(r + T['cy']) for r, _ in ts})
    out['C_max_schwanz'] = round(float(max(C[r][idx].max() for r, idx in ts if len(idx))), 1)
    out['C_p95_schwanz'] = round(float(np.percentile(np.concatenate([C[r][idx] for r, idx in ts if len(idx)]), 95)), 1)
    hs = texel_werte([i for i in range(16) if i != 9])
    out['C_max_haut_koerper'] = round(float(max(C[r][idx].max() for r, idx in hs if len(idx))), 1)
    out['C_p95_haut_koerper'] = round(float(np.percentile(np.concatenate([C[r][idx] for r, idx in hs if len(idx)]), 95)), 1)
    out['L_schwanz_texel_mittel'] = round(float(np.concatenate([lab[r][idx][:, 0] for r, idx in ts if len(idx)]).mean()), 1)
    out['bpp'] = sorted({(tp >> 7) & 3 for m in M for _, _, tp in uv_polys(m)})
    print(json.dumps(out))

main(sys.argv[1])
