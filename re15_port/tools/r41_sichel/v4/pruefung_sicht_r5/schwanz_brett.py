"""Pruefung V4 r5 Sicht, Beobachtung (ii): ist der waagerechte Schwanzteil ein flaches Brett? (a) Querprofile L* (Videoraum) quer
durch den waagerechten Teil bei x = 310/320/330/340 (Original: Vorlage 2 roh in der Leser-Maske; Port: Ids 16-22 Pose 85);
(b) Streuung/Gradient der Helligkeit im waagerechten Teil (x 300-350, Maske); (c) Querschnitt der Schwanz-Meshes im Modell
(Hauptachsen der Ecken je Mesh: Laenge/Breite/Dicke) V4 r4 und V4 r4."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G, b2
from PIL import Image
SF = sys.argv[1]; R = SF + '/sicht_r5'; out = {}
M = np.load(SF + '/schwanz_maske.npy')
o2 = np.asarray(Image.open(R + '/vorlage2_entbl.png').convert('RGB')).astype(float); Lo = G.lab(o2)[..., 0]
def grad(L): L = L.astype(np.float32); return np.hypot(cv2.Sobel(L, cv2.CV_32F, 1, 0), cv2.Sobel(L, cv2.CV_32F, 0, 1))
def prof(L, m, x):
    ys = np.nonzero(m[:, x])[0]
    if not len(ys): return None
    ys = ys[(ys < 262)]
    return dict(y0=int(ys.min()), y1=int(ys.max()), L=[round(float(L[y, x]), 1) for y in range(ys.min(), ys.max() + 1)])
def flach(L, m):
    xx = np.arange(640)[None, :]; yy = np.arange(480)[:, None]; mm = cv2.erode(m.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & (xx >= 300) & (xx <= 350) & (yy < 262)
    g = grad(L); return dict(n=int(mm.sum()), L_std=round(float(L[mm].std()), 1), grad=round(float(g[mm].mean()), 1), glatt_grad_lt3=round(float((g[mm] < 3).mean()), 3),
                             L_p10_50_90=G.r1(np.percentile(L[mm], [10, 50, 90])))
out['orig'] = {'flaeche': flach(Lo, M), 'profile': {x: prof(Lo, M, x) for x in (310, 320, 330, 340)}}
for nm in ('V4', 'V4r4'):
    img = np.asarray(Image.open(R + '/%s_85.png' % nm).convert('RGB')).astype(float); ids = np.load(R + '/%s_85_ids.npy' % nm)[:480]
    sw = np.isin(ids, range(16, 23)); L = G.lab(G.videoraum(img))[..., 0]; Ls = G.lab(img)[..., 0]
    out[nm] = {'flaeche_videoraum': flach(L, sw), 'flaeche_schirm': flach(Ls, sw), 'profile_videoraum': {x: prof(L, sw, x) for x in (310, 320, 330, 340)},
               'ids_bei_x320': sorted(set(ids[:262, 320][sw[:262, 320]].tolist()))}
# (c) Querschnitt der Meshes
for nm, emd in (('V4', G.EMD_V4), ('V4r4', SF + '/emd/EM30_V4r4.EMD'), ('REF2', G.REF2)):
    mo = b2.modell(emd); d = {}
    for i in range(16, 23):
        me = mo.meshes[i]; v = np.array([p[:3] for p in list(me.tv) + list(me.qv)], float)
        if len(v) < 3: continue
        c = v - v.mean(0); s = np.linalg.svd(c, compute_uv=False); ax = np.linalg.svd(c, full_matrices=False)[2]
        ext = [float(np.ptp(c @ a)) for a in ax]
        d[i] = dict(n_ecken=len(v), laenge=round(ext[0]), breite=round(ext[1]), dicke=round(ext[2]), dicke_durch_breite=round(ext[2] / max(ext[1], 1), 2),
                    flaechen=len(me.tf) + len(me.qf))
    out['mesh_' + nm] = d
json.dump(out, open(R + '/PS5_schwanz_brett.json', 'w'), indent=1, default=str)
for k in ('orig', 'V4', 'V4r4'):
    v = out[k]; f = v.get('flaeche') or v['flaeche_videoraum']; print(k, 'Flaeche', f, ('Schirm ' + str(v['flaeche_schirm'])) if 'flaeche_schirm' in v else '')
    pr = v.get('profile') or v['profile_videoraum']
    for x, p in pr.items(): print('   x', x, p)
for k in ('mesh_V4', 'mesh_V4r4', 'mesh_REF2'): print(k, out[k])
# Kennzahlen: Querprofil (Plateau-Anteil |dL/dy| < 0,3; groesste Stufe ueber 3 px) und Geradheit der Oberkontur (x 284-332, RMS zur Geraden; ab x 336 ist die Leser-Maske oben bei y 196 abgeschnitten)
def kenn(p):
    L = np.array(p['L']); d = np.diff(L); st3 = np.abs(L[3:] - L[:-3]).max() if len(L) > 3 else 0
    return dict(plateau=round(float((np.abs(d) < 0.3).mean()), 2), stufe3px=round(float(st3), 1))
def gerade(m, x0=284, x1=332):
    xs = []; ys = []
    for x in range(x0, x1 + 1):
        yy = np.nonzero(m[:262, x])[0]
        if len(yy): xs.append(x); ys.append(yy.min())
    xs = np.array(xs, float); ys = np.array(ys, float); k = np.polyfit(xs, ys, 1); r = ys - np.polyval(k, xs)
    return dict(rms=round(float(np.sqrt((r ** 2).mean())), 2), max_abw=round(float(np.abs(r).max()), 1), n=len(xs))
K = {'orig': {x: kenn(p) for x, p in out['orig']['profile'].items() if p}, 'orig_oberkontur': gerade(M)}
for nm in ('V4', 'V4r4'):
    ids = np.load(R + '/%s_85_ids.npy' % nm)[:480]; sw = np.isin(ids, range(16, 23))
    K[nm] = {x: kenn(p) for x, p in out[nm]['profile_videoraum'].items() if p}; K[nm + '_oberkontur'] = gerade(sw)
json.dump(K, open(R + '/PS5_schwanz_brett_kennzahlen.json', 'w'), indent=1, default=str); print(json.dumps(K))
