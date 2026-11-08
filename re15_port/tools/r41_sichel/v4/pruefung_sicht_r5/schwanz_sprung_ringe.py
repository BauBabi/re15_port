"""Pruefung V4 r5 Sicht: Ringe entlang des Schwanzes im Sprungbild h76 (Schwanz quer im Bild, x 400-500, y 215-255).
Schwanzpixel = Abstand zum Median-Hintergrund (spiel_median_bg.png) > 25; je Spalte Mittel L*/a* der Schwanzpixel im Schirm;
helle Abschnitte = Spaltenlaeufe mit L* > 30 (>= 2 Spalten). V4r5 gegen V4r4."""
import sys, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G
R = sys.argv[1]; out = {}
def rd(p): return cv2.cvtColor(cv2.imread(p), cv2.COLOR_BGR2RGB).astype(float)[:480]
bg = rd(R + '/spiel_median_bg.png')
for run in ('V4r5', 'V4r4'):
    s = rd(R + '/spiel_%s_h76.png' % run); lab = G.lab(s); m = np.abs(s - bg).sum(2) > 25; Ls, As, xs = [], [], []
    for x in range(400, 500):
        ys = np.nonzero(m[215:255, x])[0] + 215
        if len(ys) < 2: continue
        Ls.append(float(lab[ys, x, 0].mean())); As.append(float(lab[ys, x, 1].mean())); xs.append(x)
    L = np.array(Ls); hell = L > 30; laeufe = []; k = 0
    for i, h in enumerate(hell):
        if h: k += 1
        else:
            if k >= 2: laeufe.append(k)
            k = 0
    if k >= 2: laeufe.append(k)
    out[run] = dict(spalten=len(xs), helle_abschnitte=len(laeufe), laengen=laeufe, L_max=round(float(L.max()), 1), L_p50=round(float(np.median(L)), 1),
                    spruenge_dL_gt15=int((np.abs(L[2:] - L[:-2]) > 15).sum()), L_verlauf=[round(v, 0) for v in L[::4]], a_verlauf=[round(v, 0) for v in np.array(As)[::4]])
json.dump(out, open(R + '/PS5_schwanz_sprung_ringe.json', 'w'), indent=1); print(json.dumps(out, indent=1))
