"""Zahlen-Pruefer V4 r1: G1/M1-M4 mit eigenem Byte-Parser (kein riegel_v4, kein EA)."""
import struct, sys, hashlib, json
import numpy as np
R = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD'
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/emd/'
u16 = lambda b, o: struct.unpack_from('<H', b, o)[0]
u32 = lambda b, o: struct.unpack_from('<I', b, o)[0]
def secs(b):
    do, n = u32(b, 0), u32(b, 4)
    d = [u32(b, do + 4*i) for i in range(n)] + [do]
    return [b[d[i]:d[i+1]] for i in range(n)]
def md1(m):
    obj = u32(m, 8); out = []
    for i in range(obj // 2):
        h = struct.unpack_from('<14I', m, 12 + 56*i)
        tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo = h
        b = 12
        blk = dict(tv=m[b+tvo:b+tvo+8*tvc], tn=m[b+tno:b+tno+8*tnc], tf=m[b+tfo:b+tfo+12*tfc], tuv=m[b+tuo:b+tuo+12*tfc],
                   qv=m[b+qvo:b+qvo+8*qvc], qn=m[b+qno:b+qno+8*qnc], qf=m[b+qfo:b+qfo+16*qfc], quv=m[b+quo:b+quo+16*qfc])
        out.append(dict(blk=blk, ntri=tfc, nquad=qfc, quv_raw=[struct.unpack_from('<BBHBBHBBHBBH', m, b+quo+16*k) for k in range(qfc)],
                        tuv_raw=[struct.unpack_from('<BBHBBHBBH', m, b+tuo+12*k) for k in range(tfc)],
                        tv=np.frombuffer(blk['tv'], '<i2').reshape(-1, 4), qv=np.frombuffer(blk['qv'], '<i2').reshape(-1, 4),
                        tf=np.frombuffer(blk['tf'], '<u2').reshape(-1, 6), qf=np.frombuffer(blk['qf'], '<u2').reshape(-1, 8)))
    return out
def emr(e):
    bt, ko, nb, ks = u16(e, 0), u16(e, 2), u16(e, 4), u16(e, 6)
    return dict(bt=bt, ko=ko, nb=nb, ks=ks, rel=e[8:8+6*nb], tab=e[bt:ko], kf=e[ko:])
def tim(t):
    cs = u32(t, 8); cx, cy, cw, ch = struct.unpack_from('<4H', t, 12)
    clut = np.frombuffer(t[20:8+cs], '<u2').reshape(ch, cw)
    o = 8 + cs; px, py, iw, ih = struct.unpack_from('<4H', t, o+4)
    pix = np.frombuffer(t[o+12:o+12+iw*2*ih], np.uint8).reshape(ih, iw*2)
    return dict(flag=u32(t, 4), cy=cy, cw=cw, ch=ch, clut=clut, pix=pix, px=px, py=py)
def main(var):
    a, b = open(R, 'rb').read(), open(S + 'EM30_%s.EMD' % var, 'rb').read()
    out = dict(var=var, sha_ref=hashlib.sha256(a).hexdigest(), sha_var=hashlib.sha256(b).hexdigest())
    A, B = secs(a), secs(b)
    out['n_sek'] = (len(A), len(B))
    out['sek_gleich'] = {i: A[i] == B[i] for i in range(9)}
    ea, eb = emr(A[2]), emr(B[2])
    out['emr'] = dict(nb=(ea['nb'], eb['nb']), ks=(ea['ks'], eb['ks']), rel0_15=ea['rel'][:96] == eb['rel'][:96],
                      rel16_22_gleich=ea['rel'][96:] == eb['rel'][96:], tab=ea['tab'] == eb['tab'])
    ks = ea['ks']; nfa, nfb = len(ea['kf']) // ks, len(eb['kf']) // ks
    # Keyframe: 12 Byte Kopf + 12-bit-Winkel je Knochen (3 x 12 Bit = 4,5 Byte); Knochen 0-15 = 12 + 72 Byte
    kfa = np.frombuffer(ea['kf'][:nfa*ks], np.uint8).reshape(nfa, ks); kfb = np.frombuffer(eb['kf'][:nfb*ks], np.uint8).reshape(nfb, ks)
    out['kf'] = dict(n=(nfa, nfb), kf0_15_bytes_gleich=bool(nfa == nfb and np.array_equal(kfa[:, :84], kfb[:, :84])),
                     kf_alle_gleich=bool(nfa == nfb and np.array_equal(kfa, kfb)),
                     abw_spalten=[int(c) for c in np.nonzero((kfa != kfb).any(0))[0]][:20] if nfa == nfb else None)
    ma, mb = md1(A[7]), md1(B[7])
    out['n_mesh'] = (len(ma), len(mb))
    abw = {}
    for i in list(range(0, 9)) + list(range(10, 16)):
        d = [k for k in ma[i]['blk'] if ma[i]['blk'][k] != mb[i]['blk'][k]]
        if d: abw[i] = d
    out['mesh_abw_0_8_10_15'] = abw
    # Dreiecke
    tri = lambda m: m['ntri'] + 2*m['nquad']
    out['dreiecke_kopf'] = (tri(ma[9]), tri(mb[9]))
    out['dreiecke_schwanz'] = ([tri(m) for m in ma[16:]], [tri(m) for m in mb[16:]])
    # Textur/CLUT
    ta, tb = tim(A[8]), tim(B[8])
    out['tim'] = dict(flag=(hex(ta['flag']), hex(tb['flag'])), cy=(ta['cy'], tb['cy']), ch=(ta['ch'], tb['ch']), pix_shape=(ta['pix'].shape, tb['pix'].shape))
    clutrow = lambda m: sorted({(q[2] >> 6) for q in m['quv_raw']} | {(t[2] >> 6) for t in m['tuv_raw']})
    page = lambda m: sorted({(q[5] & 0x1f) for q in m['quv_raw']} | {(t[5] & 0x1f) for t in m['tuv_raw']})
    out['clut_kopf'] = clutrow(mb[9]); out['page_kopf'] = page(mb[9])
    out['clut_schwanz'] = sorted(set(sum([clutrow(m) for m in mb[16:]], []))); out['page_schwanz'] = sorted(set(sum([page(m) for m in mb[16:]], [])))
    out['clut_koerper_var'] = sorted(set(sum([clutrow(mb[i]) for i in range(16) if i != 9], [])))
    # CLUT-Zeilen-Vergleich
    rows = {}
    for z in range(max(ta['ch'], tb['ch'])):
        ra = ta['clut'][z] if z < ta['ch'] else None; rb = tb['clut'][z] if z < tb['ch'] else None
        rows[ta['cy'] + z] = 'gleich' if (ra is not None and rb is not None and np.array_equal(ra, rb)) else ('neu' if ra is None else 'geaendert')
    out['clut_zeilen'] = rows
    # Texel-Diff: Seiten, in denen sich pix unterscheidet (Breite 256 Byte je Page bei 8bpp? pix in Bytes)
    if ta['pix'].shape == tb['pix'].shape:
        dm = ta['pix'] != tb['pix']
        ys, xs = np.nonzero(dm)
        out['texel_diff'] = dict(n=int(dm.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                                 je_page={int(p): int(dm[:, p*128:(p+1)*128].sum()) for p in range(dm.shape[1] // 128)})
        # Texel von Meshes 0-15 (ohne 9): UV-Bereiche sammeln, pruefen ob Diff dort liegt
        hit = 0; tot = 0
        for i in [k for k in range(16) if k != 9]:
            for q in mb[i]['quv_raw']:
                pg = q[5] & 0xf; us = [q[0], q[3], q[6], q[9]]; vs = [q[1], q[4], q[7], q[10]]
                sub = dm[min(vs):max(vs)+1, pg*128 + min(us) - 0: pg*128 + max(us) + 1] if False else None
            for t in mb[i]['tuv_raw']:
                pass
        out['texel_diff']['hinweis'] = 'page = 128 Bytes je TPage-Spalte bei 8bpp (64 Halbworte)'
    # 8 bpp: TIM flag 9 = 8bpp mit CLUT
    print(json.dumps(out, indent=1, default=str))
    # Pixel per mesh: Kopf/Schwanz Texel-Indizes in Zeile 486/487 alle gesetzt?
    cl = tb['clut']
    for z in (486, 487):
        if z - tb['cy'] < tb['ch']:
            r = cl[z - tb['cy']]
            print('CLUT', z, 'Eintraege !=0:', int((r != 0).sum()), 'eindeutig:', len(set(r.tolist())))
if __name__ == '__main__':
    main(sys.argv[1])
