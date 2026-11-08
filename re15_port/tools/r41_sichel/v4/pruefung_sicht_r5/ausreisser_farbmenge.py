"""Pruefung V4 r5 Sicht (O9, unabhaengig vom Riegel AF): je Koerper-Mesh Anteil der Port-Pixel (Videoraum), deren Farbe mehr als
10 dE76 von JEDER Farbe der Original-Figur entfernt ist. Farbmenge Original = alle Pixel der Leser-Regionen (sfv4/reg_bild1_1..19 fuer
Vorlage 1 = Pose 135; reg_bild2_20..34 fuer Vorlage 2 = Pose 85), roh. Dazu dieselbe Zahl je Spielbild h118/h135 gegen Vorlage 1."""
import sys, json, numpy as np, cv2
from scipy.spatial import cKDTree
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
SF = sys.argv[1]; R = SF + '/sicht_r5'; out = {}
def rd(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
menge = {}
for b, rng, pose in ((1, range(1, 20), '135'), (2, range(20, 35), '85')):
    o = G.lab(rd(R + '/vorlage%d_entbl.png' % b)); m = np.zeros((480, 640), bool)
    for i in rng: m |= np.load(SF + '/reg_bild%d_%d.npy' % (b, i))
    menge[pose] = cKDTree(o[m]); out['orig_%s_n' % pose] = int(m.sum())
for pose in ('135', '85'):
    for nm in ('V4', 'V4r4', 'REF2'):
        V = G.lab(G.videoraum(rd(R + '/%s_%s.png' % (nm, pose)))); ids = np.load(R + '/%s_%s_ids.npy' % (nm, pose))[:480]; d = {}
        for i in range(0, 23):
            mm = ids == i
            if mm.sum() < 30: continue
            dist, _ = menge[pose].query(V[mm]); d[i] = dict(n=int(mm.sum()), gt10=round(float((dist > 10).mean()), 3), max=round(float(dist.max()), 1))
        aus = [k for k, v in d.items() if v['gt10'] > 0.01]
        out['%s_%s' % (nm, pose)] = dict(je_mesh=d, meshes_ueber_1proz=aus)
for h in (118, 135):
    for run in ('V4r5', 'V4r4'):
        V = G.lab(G.videoraum(rd(R + '/spiel_%s_h%d.png' % (run, h)))); ids = np.load(R + '/lp_V4r5_h%d_ids.npy' % h)[:480]
        mm = cv2.erode((ids >= 0).astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)
        dist, _ = menge['135'].query(V[mm]); out['spiel_%s_h%d' % (run, h)] = dict(n=int(mm.sum()), gt10=round(float((dist > 10).mean()), 4), gt15=round(float((dist > 15).mean()), 4))
json.dump(out, open(R + '/PS5_ausreisser_farbmenge.json', 'w'), indent=1)
for k, v in out.items():
    if isinstance(v, dict) and 'je_mesh' in v: print(k, 'ueber 1 %:', v['meshes_ueber_1proz'], {i: x['gt10'] for i, x in v['je_mesh'].items()})
    else: print(k, v)
