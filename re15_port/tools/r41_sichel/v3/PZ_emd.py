# Zahlen-Pruefer V3 r1: eigener EMD-Vergleich (eigener Parser, nicht birkin_formen/EA).
# python PZ_emd.py REF.EMD VAR.EMD
import sys, struct, json, hashlib
import numpy as np

def u32(b, o): return struct.unpack_from('<I', b, o)[0]
def u16(b, o): return struct.unpack_from('<H', b, o)[0]

def sektionen(b):
    do, n = u32(b, 0), u32(b, 4)
    offs = [u32(b, do + 4 * i) for i in range(n)]
    ends = offs[1:] + [do]
    return [(o, e) for o, e in zip(offs, ends)], do

def md1(d):
    obj = u32(d, 8); out = []
    for i in range(obj // 2):
        t = struct.unpack_from('<14I', d, 12 + i * 56)
        tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo = t
        rd = lambda off, cnt, fmt, sz: [struct.unpack_from(fmt, d, 12 + off + k * sz) for k in range(cnt)]
        out.append(dict(tv=rd(tvo, tvc, '<4h', 8), tn=rd(tno, tnc, '<4h', 8), tf=rd(tfo, tfc, '<6H', 12),
                        tuv=rd(tuo, tfc, '<12B', 12), qv=rd(qvo, qvc, '<4h', 8), qn=rd(qno, qnc, '<4h', 8),
                        qf=rd(qfo, qfc, '<8H', 16), quv=rd(quo, qfc, '<16B', 16)))
    return out

def tim(t):
    o = 8; cs = u32(t, o); cx, cy, cw, chh = struct.unpack_from('<4H', t, o + 4)
    clut = np.frombuffer(t[o + 12:o + cs], '<u2').reshape(chh, cw).copy()
    o += cs; ix, iy, iw, ih = struct.unpack_from('<4H', t, o + 4)
    px = np.frombuffer(t[o + 12:o + 12 + iw * 2 * ih], np.uint8).reshape(ih, iw * 2).copy()
    return dict(flags=u32(t, 4), cx=cx, cy=cy, clut=clut, ix=ix, iy=iy, px=px)

def uv_polys(m):
    """je Flaeche: Liste (u,v), clut-y, tpage."""
    P = []
    for uv in m['tuv']:
        u0, v0, c0, c1, u1, v1, p0, p1, u2, v2, _, _ = uv
        P.append(([(u0, v0), (u1, v1), (u2, v2)], (c0 | c1 << 8), (p0 | p1 << 8)))
    for uv in m['quv']:
        u0, v0, c0, c1, u1, v1, p0, p1, u2, v2, _, _, u3, v3, _, _ = uv
        P.append(([(u0, v0), (u1, v1), (u3, v3), (u2, v2)], (c0 | c1 << 8), (p0 | p1 << 8)))
    return P

def texel_maske(P, T):
    import cv2
    H, W = T['px'].shape
    M = np.zeros((H, W), np.uint8)
    for pts, clut, tp in P:
        xb = (tp & 0xF) * 64; ox = (xb - T['ix']) * 2
        oy = ((tp >> 4) & 1) * 256 - T['iy']
        q = np.array([[u + ox, v + oy] for u, v in pts], np.int32)
        cv2.fillPoly(M, [q], 1); cv2.polylines(M, [q], True, 1)
    return M.astype(bool)

def main(ra, rb):
    A, B = open(ra, 'rb').read(), open(rb, 'rb').read()
    out = dict(sha_ref=hashlib.sha256(A).hexdigest(), sha_var=hashlib.sha256(B).hexdigest(), len_ref=len(A), len_var=len(B))
    sa, _ = sektionen(A); sb, _ = sektionen(B)
    out['sektionen'] = len(sa), len(sb)
    sek = {}
    for i, ((a0, a1), (b0, b1)) in enumerate(zip(sa, sb)):
        sek[i] = dict(len_ref=a1 - a0, len_var=b1 - b0, gleich=A[a0:a1] == B[b0:b1])
    out['sektion_gleich'] = sek
    ma, mb = md1(A[sa[7][0]:sa[7][1]]), md1(B[sb[7][0]:sb[7][1]])
    out['meshes'] = len(ma), len(mb)
    md = {}
    for i in range(len(mb)):
        if i >= len(ma):
            md[i] = dict(neu=True, tri=len(mb[i]['tf']), quad=len(mb[i]['qf'])); continue
        a, b = ma[i], mb[i]
        d = {}
        for k in ('tv', 'tn', 'tf', 'tuv', 'qv', 'qn', 'qf', 'quv'):
            na, nb = len(a[k]), len(b[k]); n = min(na, nb)
            diff = sum(1 for j in range(n) if a[k][j] != b[k][j])
            d[k] = (na, nb, diff)
        # Vertex-Abweichung (max |dv|) ueber die gemeinsamen Vertices
        n = min(len(a['tv']), len(b['tv']))
        dv = max([max(abs(a['tv'][j][c] - b['tv'][j][c]) for c in range(3)) for j in range(n)], default=0)
        md[i] = dict(d, max_dv=dv, tri=(len(a['tf']), len(b['tf'])), quad=(len(a['qf']), len(b['qf'])))
    out['mesh_diff'] = md
    TA, TB = tim(A[sa[8][0]:]), tim(B[sb[8][0]:])
    out['tim'] = dict(flags=(TA['flags'], TB['flags']), clut_shape=(TA['clut'].shape, TB['clut'].shape), px_shape=(TA['px'].shape, TB['px'].shape),
                      cy=(TA['cy'], TB['cy']), img=(TA['ix'], TA['iy'], TB['ix'], TB['iy']))
    nc = min(TA['clut'].shape[0], TB['clut'].shape[0])
    out['clut_zeilen_geaendert'] = [int(TA['cy'] + r) for r in range(nc) if not np.array_equal(TA['clut'][r], TB['clut'][r])]
    out['clut_zeilen_neu'] = [int(TB['cy'] + r) for r in range(nc, TB['clut'].shape[0])]
    dpx = TA['px'] != TB['px'] if TA['px'].shape == TB['px'].shape else None
    out['texel_geaendert'] = int(dpx.sum()) if dpx is not None else 'Form verschieden'
    # Wer benutzt was: Koerper-Meshes 0-15 ohne 9 (Ref), Kopf 9 (Ref), Var 8 neu, Schwanz 16-22 (Var)
    nutz = {}
    for i in range(len(mb)):
        P = uv_polys(mb[i])
        nutz[i] = dict(clut_zeilen=sorted({(c >> 6) & 0x1FF for _, c, _ in P}), tpages=sorted({t for _, _, t in P}),
                       bpp=sorted({(t >> 7) & 3 for _, _, t in P}))
    out['nutzung_var'] = nutz
    if dpx is not None:
        geaendert_von = {}
        for i in range(min(16, len(ma))):
            M = texel_maske(uv_polys(ma[i]), TA)
            geaendert_von[i] = int((M & dpx).sum())
        out['geaenderte_texel_in_ref_mesh_uv'] = geaendert_von
        ys, xs = np.nonzero(dpx)
        out['texel_bbox'] = [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None
    # Koerper-Meshes, die geaenderte CLUT-Zeilen benutzen
    gc = set(out['clut_zeilen_geaendert'])
    out['koerper_mit_geaenderter_clut'] = {i: sorted(set(nutz[i]['clut_zeilen']) & gc) for i in range(16) if set(nutz[i]['clut_zeilen']) & gc}
    rf = {i: sorted({(c >> 6) & 0x1FF for _, c, _ in uv_polys(ma[i])}) for i in range(len(ma))}
    out['nutzung_ref_clut'] = rf
    print(json.dumps(out, indent=1, default=str))

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
