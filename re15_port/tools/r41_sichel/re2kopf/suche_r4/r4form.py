import numpy as np
def maullinie(V, m3, MAUL_T):
    return min(V[[m3.tf[i][2*k+1] for k in range(3)]].mean(0)[1] for i in MAUL_T)

def flaechen_n(m3, V):
    out = []
    for f in m3.tf:
        a, b, c = (V[f[2*k+1]] for k in range(3)); out.append(np.cross(b - a, c - a))
    for f in m3.qf:
        a, b, c, d = (V[f[2*k+1]] for k in range(4)); out.append(np.cross(b - a, c - a)); out.append(np.cross(d - b, c - b))
    return np.array(out)

def form_r4(V, N, f, m3, MAUL_T, MAUL_E, AUGE_E):
    W = V.copy()
    xp = V[:, 0].min(); s = f['sx']
    W[:, 0] = xp + s * (V[:, 0] - xp)
    L = np.linalg.norm(N, axis=1, keepdims=True)
    Na = N / np.array([s, 1, 1]); Na = Na / np.linalg.norm(Na, axis=1, keepdims=True) * L
    ym = maullinie(W, m3, MAUL_T); yt = W[:, 1].min()
    R = ym - yt
    xf = W[MAUL_E, 0].max()
    ax = f.get('ratio', 1.0) * R
    c = np.array([xf - ax, ym, 0.0])
    rz = np.abs(W[:, 2]).max()
    axes = np.array([ax, R, rz])
    U = (W - c) / axes; r = np.linalg.norm(U, axis=1)
    oben = W[:, 1] < ym
    vorn = W[:, 0] > c[0]
    raus = oben & vorn & (r > 1)
    W2 = W.copy()
    W2[raus] = c + (W[raus] - c) / r[raus][:, None]
    if f.get('unten'):
        yb = W[:, 1].max(); axes_u = np.array([ax, (yb - ym) * f.get('unten_f', 1.0), rz * f.get('unten_z', 1.0)])
        Uu = (W - c) / axes_u; ru = np.linalg.norm(Uu, axis=1)
        xu = W[:, 0] > c[0] + f.get('unten_x0', 0.0)
        if f.get('unten_seite') == 'rechts':
            xu &= W[:, 2] < 0
        ru_m = (~oben) & xu & (ru > 1)
        if f.get('unten_waag'):
            # waagerecht: nur x/z auf die Ellipse der Hoehe y (Kinn bleibt auf seiner Hoehe)
            for i in np.nonzero(ru_m)[0]:
                q = 1 - Uu[i, 1] ** 2
                h = np.hypot(Uu[i, 0], Uu[i, 2])
                if q <= 0 or h <= np.sqrt(q):
                    ru_m[i] = False; continue
                k = np.sqrt(q) / h
                W2[i, 0] = c[0] + (W[i, 0] - c[0]) * k; W2[i, 2] = c[2] + (W[i, 2] - c[2]) * k
        else:
            W2[ru_m] = c + (W[ru_m] - c) / ru[ru_m][:, None]
        raus = raus | ru_m
    # Augenmitte gleitet nach hinten/unten (S2/K4), nur die Nabe des Augenfaechers
    for i in f.get('auge_e', ()):
        W2[i] += np.array([-f.get('auge_dx', 0.0), f.get('auge_dy', 0.0), 0.0])
    return W, W2, Na, dict(ym=float(ym), yt=float(yt), xf=float(xf), c=c.tolist(), axes=axes.tolist(), raus=[int(i) for i in np.nonzero(raus)[0]],
                           r={int(i): float(r[i]) for i in np.nonzero(raus)[0]})
