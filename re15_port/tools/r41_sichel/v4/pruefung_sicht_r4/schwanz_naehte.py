"""Pruefung V4 r4 Sicht: Naehte entlang des waagerechten Schwanzteils. Je Spalte x 280..376 Mittelpunkt der Maske (y < 262),
dort Lab (Mittel 3x3) im Videoraum (Port) bzw. roh (Vorlage 2, Leser-Maske). Kennzahlen: groesster Sprung dL/da ueber 3 Spalten,
Zahl der Spruenge |dE76| > 8 ueber 3 Spalten (= sichtbare Quernaht), Glattanteil Gradient < 3 im waagerechten Teil."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
from PIL import Image
SF = sys.argv[1]; R = SF + '/sicht_r4'; out = {}
def L(p): return np.asarray(Image.open(p).convert('RGB')).astype(float)
def achse(lab, m):
    xs, vals = [], []
    for x in range(280, 377):
        ys = np.nonzero(m[:262, x])[0]
        if len(ys) < 5: continue
        y = int((ys.min() + ys.max()) / 2); vals.append(lab[y-1:y+2, x-1:x+2].reshape(-1, 3).mean(0)); xs.append(x)
    v = np.array(vals); d = np.linalg.norm(v[3:] - v[:-3], axis=1); dl = np.abs(v[3:, 0] - v[:-3, 0]); da = np.abs(v[3:, 1] - v[:-3, 1])
    # Spruenge zaehlen: lokale Maxima von d ueber 8 mit Mindestabstand 6 Spalten
    j = []; 
    for i in np.argsort(-d):
        if d[i] <= 8: break
        if all(abs(i - k) >= 6 for k in j): j.append(int(i))
    return dict(n=len(xs), x=[xs[0], xs[-1]], max_dE76_3sp=round(float(d.max()), 1), max_dL_3sp=round(float(dl.max()), 1), max_da_3sp=round(float(da.max()), 1),
                naehte_dE76_gt8=len(j), naht_x=sorted(xs[i] + 1 for i in j), L_verlauf=[round(float(a), 1) for a in v[::4, 0]], a_verlauf=[round(float(a), 1) for a in v[::4, 1]])
M = np.load(SF + '/schwanz_maske.npy'); o2 = L(R + '/vorlage2_entbl.png')
out['orig'] = achse(G.lab(o2), M)
for nm in ('V4', 'V4r3'):
    img = L(R + '/%s_85.png' % nm); ids = np.load(R + '/%s_85_ids.npy' % nm)[:480]; sw = np.isin(ids, range(16, 23))
    out[nm] = achse(G.lab(G.videoraum(img)), sw); out[nm + '_schirm'] = achse(G.lab(img), sw)
    # welche Mesh-Ids liegen an den Naehten?
    out[nm]['ids_an_naht'] = {x: sorted(set(ids[:262, x][sw[:262, x]].tolist())) for x in out[nm]['naht_x']}
json.dump(out, open(sys.argv[2], 'w'), indent=1); print(json.dumps(out, indent=1))
