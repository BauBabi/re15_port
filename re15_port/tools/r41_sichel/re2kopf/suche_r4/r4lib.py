import sys, json, time
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np
import v4_gemein as G, b2, kopf_re2_mess as KM, kopf_re2_bau as KB, riegel_re2kopf as RG, re2_leser as RL
P = G.posen()
m3 = RL.modell(KB.G2).meshes[3]
V0 = np.array([v[:3] for v in m3.tv], float); N0 = np.array([n[:3] for n in m3.tn], float)
MAUL_E = sorted({m3.tf[i][2*k+1] for i in KB.MAUL_T for k in range(3)})

def maullinie(V):
    return min(V[[m3.tf[i][2*k+1] for k in range(3)]].mean(0)[1] for i in KB.MAUL_T)

def form_r4(V, N, f):
    """(a) x-Skalierung um den Hinterkopf, (b) Kuppe: Ecken oberhalb der Maullinie vorn entlang -x auf die runde Kappe."""
    W = V.copy(); Nn = N.copy()
    xp = V[:, 0].min()
    s = f['sx']
    W[:, 0] = xp + s * (V[:, 0] - xp)
    L = np.linalg.norm(N, axis=1, keepdims=True)
    Nn = N / np.array([s, 1, 1]); Nn = Nn / np.linalg.norm(Nn, axis=1, keepdims=True) * L
    ym = maullinie(W); yt = W[:, 1].min()
    ay = ym - yt
    xf = W[MAUL_E, 0].max() - f.get('dxf', 0.0)
    ax = f.get('ratio', 1.0) * ay
    cx = xf - ax
    cz = 0.0
    rz = np.abs(W[:, 2]).max() * f.get('rzf', 1.0)
    W2 = W.copy(); bew = []
    for i in range(len(W)):
        x, y, z = W[i]
        if y >= ym or x <= cx:
            continue
        q = 1 - ((y - ym) / ay) ** 2 - ((z - cz) / rz) ** 2
        xd = cx + ax * np.sqrt(max(q, 0.0))
        if x > xd:
            W2[i, 0] = xd; bew.append(i)
    if f.get('auge_dx') or f.get('auge_dy'):
        for i in f.get('auge_e', (39,)):
            W2[i, 0] -= f.get('auge_dx', 0); W2[i, 1] += f.get('auge_dy', 0)
    if f.get('dome', True) is False:
        W2 = W.copy()
        if f.get('auge_dx'):
            for i in f.get('auge_e', (39,)):
                W2[i, 0] -= f['auge_dx']
    return W, W2, Nn, dict(ym=ym, yt=yt, xf=xf, cx=cx, ax=ax, ay=ay, rz=rz, bewegt=bew)

import r4form as F4
def geom_patch(f):
    def g(p):
        Wa, W2, Na, info = F4.form_r4(V0, N0, f, m3, KB.MAUL_T, MAUL_E, ())
        bew = [i for i in range(len(V0)) if np.linalg.norm(W2[i] - Wa[i]) > 0.5]
        N2, nidx = KB.normalen_neu(m3, W2, bew)
        # Normalen: skaliert fuer alle, neu fuer bewegte
        Nf = Na.copy(); Nf[nidx] = N2[nidx]
        info['normalen_neu'] = nidx; info['bewegt'] = bew
        return V0, W2, Nf, info
    return g

def bauen(f, versatz=None):
    p = json.loads(json.dumps(KB.PARAM_R3))
    if versatz is not None: p['versatz'] = list(versatz)
    KB.g2_geometrie = geom_patch(f)
    neu, prot, _ = KB.bauen(p)
    return neu, prot

def messen(blob, prot):
    mo = b2.modell(blob)
    M = KM.messen(mo, P, bilder=True); RG.hell_unten(mo, M)
    out = {}
    b = M['85']; kb = b['kopfbox']
    out['vis85'] = (kb[2]-kb[0]+1)/(kb[3]-kb[1]+1)
    a = M['135']; ka = a['kopfbox']
    s_ = RG.silhouette(mo, P)
    out['ueber'] = s_['ueber_max']; out['iou'] = s_['iou']; out['front85'] = s_['kopf_vorderkante']
    out['kb85'] = kb; out['kb135'] = ka
    out['K10'] = a.get('K10'); out['K4'] = a.get('K4'); out['S2'] = b.get('S2')
    out['K7'] = a['zaehne'].get('anteil'); out['z135'] = a['zaehne']['px']
    out['KS2'] = (b['KS2']['kopf'][0], b['kopf_p50_v'] - 5)
    Z = RG.tr2_tr3(mo, M) + RG.kopf_r2(mo, M, P, prot['zonen'])
    out['rot'] = [z['mass'] for z in Z if not z['ok'] and not z.get('info')]
    return out, mo, M

def kurz(o):
    return 'vis85 %.3f ueber %d iou %.3f front85 %d kb85 %s K10 %s K4 %s S2 %s K7 %s z %d KS2 %.1f<=%.1f ROT %s' % (
        o['vis85'], o['ueber'], o['iou'], o['front85'], o['kb85'], np.round(o['K10'], 3).tolist(), np.round(o['K4'], 3).tolist() if o['K4'] else None,
        np.round(o['S2'], 3).tolist() if o['S2'] else None, np.round(o['K7'], 2).tolist() if o['K7'] else None, o['z135'], o['KS2'][0], o['KS2'][1], o['rot'])

import cv2
import emd_ansichtsblatt as EA
def g2_rahmen(mo, p=None):
    """Mesh 9 zurueck in den G2-Rahmen: (V, N, F) fuer EA.render."""
    p = p or KB.PARAM_R3
    m9 = mo.meshes[9]
    Q = np.array([v[:3] for v in m9.tv], float); Nq = np.array([n[:3] for n in m9.tn], float)
    sk = np.array(p['skal'], float); R = KB.rot(p); t = np.array(p['versatz'], float)
    V = ((Q - t) @ R) / sk
    Nn = (Nq @ R) * sk; Nn /= np.maximum(np.linalg.norm(Nn, axis=1, keepdims=True), 1e-9)
    F = []
    for i, (n0, v0, n1, v1, n2, v2) in enumerate(m9.tf):
        uv = m9.tuv[i]
        F.append(dict(v=(v0, v1, v2), n=(n0, n1, n2), uv=((uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])), clut=uv[2], page=uv[5], bone=9))
    for i, (n0, v0, n1, v1, n2, v2, n3, v3) in enumerate(m9.qf):
        uv = m9.quv[i]; a, bb, c, dd = (uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])
        F.append(dict(v=(v0, v1, v2), n=(n0, n1, n2), uv=(a, bb, c), clut=uv[2], page=uv[5], bone=9))
        F.append(dict(v=(v1, v3, v2), n=(n1, n3, n2), uv=(bb, dd, c), clut=uv[2], page=uv[5], bone=9))
    return V, Nn, F

def profil(mo, ansicht='seite', scale=0.5, W=300, H=300, cx=150, cy=170, p=None):
    V, N, F = g2_rahmen(mo, p)
    if ansicht == 'seite':
        r, d, dp = EA.V_RIGHT
    else:
        r, d, dp = EA.V_FRONT
    # Mitte: G2-Ursprung bei (cx, cy)
    col, alp = EA.render(mo, V, N, F, r, d, dp, W, H, scale, cx, cy)
    img = (np.clip(col, 0, 1) * 255).astype(np.uint8)
    img[alp == 0] = (40, 40, 46)
    return img, alp
