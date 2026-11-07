"""Pruefer Formfarbe: eigene Lader + Texelsammlung (Texelzentren im UV-Dreieck, je Mesh eindeutig, CLUT-Zeile der Flaeche)."""
import sys, re, math, os
import numpy as np, cv2
WT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), *(['..'] * 7)))  # Arbeitsbaum-Wurzel
sys.path.insert(0, WT + '/re15_port/tools')
sys.path.insert(0, WT + '/analysis/befunde_runde41/form/varianten/gen')
import emd_ansichtsblatt as EA
import form_emd as FE
K5 = WT + '/analysis/befunde_runde41/form/varianten/K5'
VAR = WT + '/analysis/befunde_runde41/form/varianten'

def _blob(b):
    mo = EA.Modell.__new__(EA.Modell)
    d = EA.emd_dir(b)
    mo.meshes = EA.md1_parse(b[d[7]:d[8]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(b[d[8]:])
    return mo

def em30():
    return _blob(open(WT + '/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD', 'rb').read())

def re15typ(t):
    return EA.Modell(t)

def re2(kind):
    import birkin_bahnsteig as BB
    inc = open(WT + '/re15_port/engine/src/gen/re2_ems_toc.inc', encoding='utf-8', errors='replace').read()
    toc = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]+)u', re.search(r's_re2_cdemd0_toc\[600\]\s*=\s*\{(.*?)\};', inc, re.S).group(1))]
    ems = open(WT + '/re15_port/shared_assets/RE2/CDEMD0.EMS', 'rb').read()
    def rec(r):
        i = ((kind - 0x10) * 4 + r) * 2
        return ems[toc[i] * 0x800:toc[i] * 0x800 + toc[i + 1]]
    BB.KIND = kind
    return BB.modell(rec(3), rec(2))

def var(d):
    return FE.modell(open(d + '/FORM.EMD', 'rb').read(), open(d + '/FORM.TIM', 'rb').read())

def lab(rgb):
    a = np.asarray(rgb, np.float32).reshape(1, -1, 3) / 255.0
    return cv2.cvtColor(a, cv2.COLOR_RGB2LAB).reshape(-1, 3).astype(float)

def flaechen(mo):
    out = []
    for b, m in enumerate(mo.meshes):
        for verts, faces, uvs, q in ((m.tv, m.tf, m.tuv, 0), (m.qv, m.qf, m.quv, 1)):
            for g, uv in zip(faces, uvs):
                ie = g[1::2]
                po = (uv[5] & 0xF) * 128
                z = int(np.clip((uv[2] >> 6) - mo.clut_y, 0, mo.pal.shape[0] - 1))
                P = [np.array(verts[i][:3], float) for i in ie]
                if q:
                    T = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, c in ((0, 1, 2), (1, 3, 2)):
                        out.append((b, (P[a], P[bb], P[c]), (T[a], T[bb], T[c]), z))
                else:
                    T = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    out.append((b, tuple(P), tuple(T), z))
    return out

def sammeln(mo):
    """je Mesh: dict (x,y)->zeile ; Dreiecke (mesh, uvFlaeche, 3D-Flaeche). Texelzentrum im geschlossenen Dreieck (Toleranz 1e-6)."""
    h, w = mo.tex.shape
    per, tri = {}, []
    for b, P, T, z in flaechen(mo):
        u = np.array([t[0] for t in T], float); v = np.array([t[1] for t in T], float)
        A2 = (u[1]-u[0])*(v[2]-v[0]) - (u[2]-u[0])*(v[1]-v[0])
        A3 = 0.5 * np.linalg.norm(np.cross(P[1]-P[0], P[2]-P[0]))
        tri.append((b, abs(A2)/2, A3))
        if abs(A2) < 1e-9:
            continue
        x0, x1 = int(np.floor(u.min())), int(np.ceil(u.max())); y0, y1 = int(np.floor(v.min())), int(np.ceil(v.max()))
        X, Y = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        w1 = ((X-u[0])*(v[2]-v[0]) - (u[2]-u[0])*(Y-v[0])) / A2
        w2 = ((u[1]-u[0])*(Y-v[0]) - (X-u[0])*(v[1]-v[0])) / A2
        w0 = 1 - w1 - w2
        inn = (w0 >= -1e-6) & (w1 >= -1e-6) & (w2 >= -1e-6)
        xs = np.clip(np.floor(X[inn]).astype(int), 0, w-1); ys = np.clip(np.floor(Y[inn]).astype(int), 0, h-1)
        s = per.setdefault(b, {})
        for x, y in zip(xs.tolist(), ys.tolist()):
            s.setdefault((x, y), z)
    out = {}
    for b, s in per.items():
        k = np.array(list(s.keys()), int); z = np.array(list(s.values()), int)
        out[b] = dict(xy=k, rgb=mo.pal[z, mo.tex[k[:, 1], k[:, 0]]].astype(np.uint8))
    return out, tri
