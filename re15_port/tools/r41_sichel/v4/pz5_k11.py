"""Zahlen-Pruefer V4 r4: K11 Kopfstruktur vorn (Entscheid O4) mit eigenem Code.
(a) Nachrechnung der Bauer-Definition (Sobel-Betrag der Luminanz 0,299/0,587/0,114 im Videoraum; Original = Kopfbox 368-416 x 139-191
    2 px erodiert, 9 Verschiebungen +-3 px; Port = Id 9 2 px erodiert; Rumpf Original Box 352-448 x 196-292, Port Id 8).
(b) Gleiche Fenster in beiden Bildern: Anteils-Fenster der Kopfbox (nur Kopfinneres, im Port zusaetzlich Id 9), Port vorher auf die
    Originalgroesse skaliert (H1 302 / H_port), damit der Gradient je Pixel vergleichbar ist; Ziel/Toleranz aus den Original-Fenstern.
(c) L*-Streuung (Std von L* nach Abzug Gauss sigma 3) als zweites, gradientenfreies Strukturmass."""
import sys, os, json, numpy as np, cv2
from scipy import ndimage as nd
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pz5_mess import vraum, lab, orig, SP
lum = lambda a: np.asarray(a, float) @ np.array([0.299, 0.587, 0.114])
def grad(L): L = L.astype(np.float32); return np.hypot(cv2.Sobel(L, cv2.CV_32F, 1, 0), cv2.Sobel(L, cv2.CV_32F, 0, 1))
def boxm(b, sh=(480, 640)): m = np.zeros(sh, bool); m[b[1]:b[3] + 1, b[0]:b[2] + 1] = True; return m
er = lambda m, k=2: nd.binary_erosion(m, iterations=k)
O = orig(1); gO = grad(lum(O)); LO = lab(O)[..., 0]
KB = (368, 139, 416, 191); RB = (352, 196, 448, 292)
res = {}
w9 = [float(gO[er(boxm((KB[0] + dx, KB[1] + dy, KB[2] + dx, KB[3] + dy)))].mean()) for dx in (-3, 0, 3) for dy in (-3, 0, 3)]
res['a_orig_box9'] = dict(mittel=round(np.mean(w9), 1), min=round(min(w9), 1), max=round(max(w9), 1), rumpf=round(float(gO[er(boxm(RB))].mean()), 1))
FEN = dict(innen=(0.2, 0.15, 0.8, 0.85), oben=(0.25, 0.05, 0.8, 0.4), mitte=(0.15, 0.35, 0.85, 0.7), unten=(0.2, 0.6, 0.8, 0.9),
           links=(0.1, 0.2, 0.5, 0.85), rechts=(0.5, 0.2, 0.9, 0.85))
def fenster(kb, f): x0, y0, x1, y1 = kb; w, h = x1 - x0, y1 - y0; return (int(round(x0 + f[0] * w)), int(round(y0 + f[1] * h)), int(round(x0 + f[2] * w)), int(round(y0 + f[3] * h)))
def mittel(g, m): return float(g[m].mean()) if m.any() else float('nan')
def streu(L, m): d = L - cv2.GaussianBlur(L.astype(np.float32), (0, 0), 3); return float(d[m].std()) if m.any() else float('nan')
ob = {k: dict(grad=round(mittel(gO, boxm(fenster(KB, f))), 1), streu=round(streu(LO, boxm(fenster(KB, f))), 2)) for k, f in FEN.items()}
res['b_orig_fenster'] = ob
M = json.load(open(SP + 'mess_V4_V4R4_V3.json'))
for nm in ('V4', 'V4R4', 'V3'):
    d = np.load(SP + 'r_%s_135.npz' % nm); V = vraum(d['img']); ids = d['ids']; H = M[nm]['vorn']['H']
    g = grad(lum(V))
    a = dict(kopf=round(mittel(g, er(ids == 9)), 1), rumpf=round(mittel(g, er(ids == 8)), 1))
    # (b) skaliert: Port-Bild und Ids auf Originalmassstab
    f = 302.0 / H
    Vs = cv2.resize(V.astype(np.float32), None, fx=f, fy=f, interpolation=cv2.INTER_AREA)
    ids_s = cv2.resize(ids.astype(np.uint8), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
    gs = grad(lum(Vs)); Ls = lab(Vs)[..., 0]
    ys, xs = np.nonzero(ids_s == 9); kb = (xs.min(), ys.min(), xs.max(), ys.max())
    b = {}
    for k, fr in FEN.items():
        m = boxm(fenster(kb, fr), gs.shape) & er(ids_s == 9, 1)
        b[k] = dict(grad=round(mittel(gs, m), 1), streu=round(streu(Ls, m), 2), n=int(m.sum()))
    a['kopf_skaliert'] = round(mittel(gs, er(ids_s == 9)), 1); a['rumpf_skaliert'] = round(mittel(gs, er(ids_s == 8)), 1)
    res[nm] = dict(a=a, b=b, faktor=round(f, 3))
json.dump(res, open(SP + 'k11.json', 'w'), indent=1)
for k, v in res.items(): print(k, json.dumps(v))
