"""Leser B Formfarbe: Lader fuer EM30_HANGAR (RE1.5-EMD mit TIM) und Varianten (FORM.EMD/TIM), Texel je Flaeche."""
import os, sys, struct, math
import numpy as np, cv2
WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
TOOLS = WT + '/re15_port/tools'
sys.path.insert(0, TOOLS)
import emd_ansichtsblatt as EA
import form_emd as FE
import form_buehne as FB
FORM = WT + '/analysis/befunde_runde41/form'
K5 = FORM + '/varianten/K5'
OUT = K5 + '/farbe_lesung'
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe'

class M: pass

def lade_em30(pfad=WT + '/re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD'):
    b = open(pfad, 'rb').read()
    d = EA.emd_dir(b)
    mo = EA.Modell.__new__(EA.Modell)
    mo.meshes = EA.md1_parse(b[d[7]:d[8]])
    mo.skel = EA.emr_parse(b[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(b[d[1]:d[2]])
    tim = b[d[8]:EA.u32(b, 0)]
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(tim)
    mo.texh, mo.texw = mo.tex.shape
    mo.tim = tim
    mo.T = FB.Textur(tim)
    return mo

def lade_var(d):
    emd = open(d + '/FORM.EMD', 'rb').read(); tim = open(d + '/FORM.TIM', 'rb').read()
    mo = FE.modell(emd, tim); mo.tim = tim; mo.T = FB.Textur(tim)
    return mo

def flaechen(mo):
    """Je Flaeche: mesh, Weltecken (Knochenraum), UV (u inkl. Seite), CLUT-Zeile. Vierecke wie EA.geometrie: (a,b,c),(b,d,c)."""
    out = []
    for b, m in enumerate(mo.meshes):
        for verts, faces, uvs, q in ((m.tv, m.tf, m.tuv, 0), (m.qv, m.qf, m.quv, 1)):
            for g, uv in zip(faces, uvs):
                ie = g[1::2]
                po = (uv[5] & 0xF) * 128
                zeile = max(0, min(mo.pal.shape[0] - 1, (uv[2] >> 6) - mo.clut_y))
                P = [np.array(verts[i][:3], float) for i in ie]
                if q:
                    T = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for a, bb, c in ((0, 1, 2), (1, 3, 2)):
                        out.append((b, (P[a], P[bb], P[c]), (T[a], T[bb], T[c]), zeile))
                else:
                    T = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    out.append((b, tuple(P), tuple(T), zeile))
    return out

def texel_der_flaeche(T, w, h):
    """Texelzentren (u+0.5,v+0.5) im UV-Dreieck (inkl. Rand), wie der Rasterer mit int-Abschnitt ungefaehr."""
    u = np.array([t[0] for t in T], float); v = np.array([t[1] for t in T], float)
    x0, x1 = int(np.floor(u.min())), int(np.ceil(u.max())); y0, y1 = int(np.floor(v.min())), int(np.ceil(v.max()))
    fl = (u[1]-u[0])*(v[2]-v[0]) - (u[2]-u[0])*(v[1]-v[0])
    if abs(fl) < 1e-9:
        return np.zeros((0,), int), np.zeros((0,), int), 0.0
    X, Y = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
    w1 = ((X-u[0])*(v[2]-v[0]) - (u[2]-u[0])*(Y-v[0])) / fl
    w2 = ((u[1]-u[0])*(Y-v[0]) - (X-u[0])*(v[1]-v[0])) / fl
    w0 = 1 - w1 - w2
    e = 0.5 / max(abs(fl), 1)  # kleiner Rand
    inn = (w0 >= -e) & (w1 >= -e) & (w2 >= -e)
    xs = np.clip(np.floor(X[inn]).astype(int), 0, w - 1); ys = np.clip(np.floor(Y[inn]).astype(int), 0, h - 1)
    return xs, ys, abs(fl) / 2

def texel_rgb(mo, zeile, xs, ys):
    idx = mo.tex[ys, xs]
    return mo.pal[zeile][idx], idx

def lab(rgb):
    a = np.asarray(rgb, np.uint8).reshape(-1, 1, 3)
    L = cv2.cvtColor(a, cv2.COLOR_RGB2LAB).reshape(-1, 3).astype(float)
    L[:, 0] *= 100.0 / 255.0; L[:, 1] -= 128; L[:, 2] -= 128
    return L

def lab1(rgb):
    return [round(float(x), 1) for x in lab(np.array([rgb]))[0]]
