"""Pruefung V4 r5 Sicht (O9 fuer den Schwanz, Ids 16-22 - der Riegel AF prueft nur Ids 0-15): Anteil der Schwanzpixel (Videoraum),
die mehr als 10 dE76 von JEDER Farbe des Original-Schwanzes (Vorlage 2 roh, Leser-Maske sfv4/schwanz_maske.npy) entfernt sind.
Buehne Pose 85 mit Licht in 6 Gierwinkeln (wie Leiste 08, aber beleuchtet) und Spielbild h85 (Schwanzmaske aus der Laufpose)."""
import sys, os, json, numpy as np, cv2
from scipy.spatial import cKDTree
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G, b2
from PIL import Image
SF = G.SFV4; R = SF + '/sicht_r5'; out = {}
M = np.load(SF + '/schwanz_maske.npy'); o = G.lab(np.asarray(Image.open(R + '/vorlage2_entbl.png').convert('RGB')).astype(float))[M]; T = cKDTree(o)
for nm, emd in (('V4', G.EMD_V4), ('V4r4', os.path.join(SF, 'emd', 'EM30_V4r4.EMD'))):
    mo = b2.modell(emd); p0 = dict(G.posen()['85']); d = {}
    for dg in range(0, 360, 60):
        p = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
        img, ids = G.zeichne(mo, p); ids = ids[:480]; sw = np.isin(ids, range(16, 23))
        if sw.sum() < 30: continue
        V = G.lab(G.videoraum(np.asarray(img, float)[:480]))[sw]; dist, _ = T.query(V)
        d[dg] = dict(n=int(sw.sum()), gt10=round(float((dist > 10).mean()), 3), L_gt46=round(float((V[:, 0] > 46).mean()), 3), p99_dist=round(float(np.percentile(dist, 99)), 1))
    out[nm + '_gier'] = d
    ids = np.load(R + '/lp_%s_h85_ids.npy' % ('V4r5' if nm == 'V4' else 'V4r4'))[:480]
    s = cv2.cvtColor(cv2.imread(R + '/spiel_%s_h85.png' % ('V4r5' if nm == 'V4' else 'V4r4')), cv2.COLOR_BGR2RGB).astype(float)[:480]
    sw = cv2.erode(np.isin(ids, range(16, 23)).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    V = G.lab(G.videoraum(s))[sw]; dist, _ = T.query(V)
    out[nm + '_spiel_h85'] = dict(n=int(sw.sum()), gt10=round(float((dist > 10).mean()), 3), L_gt46=round(float((V[:, 0] > 46).mean()), 3))
out['orig_L_max'] = round(float(o[:, 0].max()), 1)
json.dump(out, open(R + '/PS5_schwanz_ausreisser.json', 'w'), indent=1); print(json.dumps(out))
