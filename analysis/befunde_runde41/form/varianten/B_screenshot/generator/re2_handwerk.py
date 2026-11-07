"""RE2-Gegner: Handwerk messen (nur Handwerk, nie Form). Aufruf: python re2_handwerk.py AUSDIR [FORM.EMD FORM.TIM]"""
import os, re, sys, json
import numpy as np, cv2
W = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', '..'))
sys.path.insert(0, os.path.join(W, 're15_port', 'tools'))
import form_emd as FE
inc = open(os.path.join(W, 're15_port/engine/src/gen/re2_ems_toc.inc'), encoding='utf-8', errors='replace').read()
toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
ems = open(os.path.join(W, 're15_port/shared_assets/RE2/CDEMD0.EMS'), 'rb').read()
def rec(kind, r):
    i = ((kind - 0x10) * 4 + r) * 2
    return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]]
def tri_area(p):
    return 0.5 * np.linalg.norm(np.cross(p[1] - p[0], p[2] - p[0]))
def uv_area(t):
    return 0.5 * abs((t[1][0]-t[0][0])*(t[2][1]-t[0][1]) - (t[2][0]-t[0][0])*(t[1][1]-t[0][1]))
def messen(name, emd, tim, aus):
    f = FE.lese_emd(emd)
    t = FE.tim_lesen(tim)
    px = t['pixel']; H, Wd = px.shape
    cl = np.array(t['cluts'], np.int64)
    rgb_c = np.stack([(cl & 31), (cl >> 5) & 31, (cl >> 10) & 31], -1) * 255 // 31
    cy0 = t['clut_y']
    # Pose 0 fuer Hoehe
    try:
        R, T = FE.pose_aus_bild(f, f['clips'][0]['bilder'][0]); pose_ok = True
    except Exception:
        pose_ok = False
    alle = []
    dichte_a = []   # (dichte, flaeche3d)
    je_mesh = []
    bedeckt = np.zeros((len(cl), H, Wd), bool)
    nf = 0
    for mi, m in enumerate(f['meshes']):
        E = np.array(m['ecken'], float)
        if pose_ok and len(E) and mi < len(R):
            alle.append(E @ (R[mi].astype(float) / 4096).T + T[mi])
        a3 = auv = 0.0
        zeilen = set()
        for fl, tris in [(q, ((0, 1, 3), (0, 3, 2))) for q in m['vierecke']] + [(d, ((0, 1, 2),)) for d in m['dreiecke']]:
            nf += 1
            if 'uv' in fl:
                pg = fl['page'] & 0xF
                tx = [[u + pg * 128, v] for u, v in fl['uv']]
                z = (fl['clut'] >> 6) - cy0
            else:
                tx = fl['tex']; z = fl.get('zeile', 0)
            z = int(np.clip(z, 0, len(cl) - 1)); zeilen.add(z)
            for a, b, c in tris:
                p = E[[fl['e'][a], fl['e'][b], fl['e'][c]]]
                q = [tx[a], tx[b], tx[c]]
                A, U = tri_area(p), uv_area(q)
                a3 += A; auv += U
                if A > 1 and U > 0.5:
                    dichte_a.append((np.sqrt(U / A), A))
                poly = np.array(q, np.int32)
                mk = np.zeros((H, Wd), np.uint8)
                cv2.fillConvexPoly(mk, poly, 1)
                bedeckt[z] |= mk.astype(bool)
        je_mesh.append(dict(mesh=mi, flaechen=len(m['dreiecke']) + len(m['vierecke']), zeilen=sorted(zeilen), dichte=round(float(np.sqrt(auv / a3)) if a3 > 0 else 0, 4)))
    d = np.array(dichte_a)
    w = d[:, 1] / d[:, 1].sum()
    o = np.argsort(d[:, 0]); cw = np.cumsum(w[o])
    pct = lambda q: float(d[o, 0][np.searchsorted(cw, q)])
    if alle:
        P = np.concatenate(alle); hoehe = float(P[:, 1].max() - P[:, 1].min()); breite = float(np.ptp(P[:, 0])); tiefe = float(np.ptp(P[:, 2]))
    else:
        hoehe = breite = tiefe = 0
    # Farben je CLUT-Zeile: Indizes im bedeckten Bereich
    farben = []
    for z in range(len(cl)):
        idx = px[bedeckt[z]]
        if idx.size == 0:
            continue
        u, n = np.unique(idx, return_counts=True)
        # 15-Bit-Farben verschieden
        c15 = np.unique(cl[z][u])
        n90 = int(np.searchsorted(np.cumsum(np.sort(n)[::-1]) / n.sum(), 0.9) + 1)
        farben.append(dict(zeile=z, texel=int(idx.size), indizes=int(len(u)), farben15=int(len(c15)), fuer_90_prozent=n90))
    # Stil: Kantenanteil, Glattheit im bedeckten Bereich (Farbe aus der jeweils ersten Zeile, die den Texel deckt)
    bild = np.zeros((H, Wd, 3), np.uint8); jede = np.zeros((H, Wd), bool)
    for z in range(len(cl)):
        s = bedeckt[z] & ~jede
        bild[s] = rgb_c[z][px[s]]; jede |= s
    Y = bild.astype(np.float32) @ np.array([.299, .587, .114], np.float32)
    gx = np.abs(np.diff(Y, axis=1))[:, :-0 or None]; gy = np.abs(np.diff(Y, axis=0))
    mx = jede[:, 1:] & jede[:, :-1]; my = jede[1:] & jede[:-1]
    g = np.concatenate([gx[mx], gy[my]])
    stil = dict(nachbar_gleich=round(float((g == 0).mean()), 3), nachbar_klein_1_6=round(float(((g > 0) & (g <= 6)).mean()), 3), nachbar_hart_ab_20=round(float((g >= 20).mean()), 3), mittel_Y=round(float(Y[jede].mean()), 1))
    cv2.imwrite(os.path.join(aus, 'tex_%s.png' % name), cv2.resize(bild[:, :, ::-1], None, fx=3, fy=3, interpolation=cv2.INTER_NEAREST))
    r = dict(name=name, flaechen=nf, meshes=len(f['meshes']), tim=[Wd, H], clut_zeilen=len(cl), hoehe=round(hoehe), breite=round(breite), tiefe=round(tiefe),
             dichte_median=round(pct(.5), 4), dichte_p10=round(pct(.1), 4), dichte_p90=round(pct(.9), 4),
             texel_ueber_hoehe=round(pct(.5) * hoehe, 1), bedeckt=int(jede.sum()), farben=farben, stil=stil, je_mesh=je_mesh)
    return r
if __name__ == '__main__':
    aus = sys.argv[1]; os.makedirs(aus, exist_ok=True)
    nur = len(sys.argv) > 2
    R = []
    for k in (() if nur else (0x34, 0x33, 0x31, 0x23, 0x30, 0x36, 0x10, 0x22, 0x2e)):
        try:
            R.append(messen('re2_%02x' % k, rec(k, 3), rec(k, 2), aus))
        except Exception as e:
            print('kind', hex(k), 'Fehler', e)
    for a in sys.argv[2:]:
        nm, e, t = a.split(',')
        R.append(messen(nm, open(e, 'rb').read(), open(t, 'rb').read(), aus))
    json.dump(R, open(os.path.join(aus, 'handwerk.json'), 'w'), indent=1)
    for r in R:
        fz = [x['farben15'] for x in r['farben']]; f9 = [x['fuer_90_prozent'] for x in r['farben']]
        print('%-10s Fl %4d Mesh %2d TIM %s CLUT %d  H %5d  Dichte p10/50/90 %.4f %.4f %.4f  Texel/Hoehe %5.1f  bedeckt %6d  Farben je Zeile %s  90%% %s  Stil %s'
              % (r['name'], r['flaechen'], r['meshes'], r['tim'], r['clut_zeilen'], r['hoehe'], r['dichte_p10'], r['dichte_median'], r['dichte_p90'], r['texel_ueber_hoehe'], r['bedeckt'], fz, f9, r['stil']))
