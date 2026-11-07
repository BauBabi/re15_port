"""Pruefung V4 r5 Sicht: helle Stellen (L* > 45 / > 50) auf dem Schwanz in den Spielbildern h60..h100 (Lauf sfv4/r/V4r5 und V4r4,
c6.port registriert); Schwanzmaske aus dem b2-Render der Laufpose (riegel_v3.lauf_posen) des jeweiligen Laufs, 1 px erodiert."""
import sys, os, json, numpy as np, cv2
H = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel'
sys.path.insert(0, H + '/v4'); sys.path.insert(0, H + '/nb2neu')
import v4_gemein as G, b2, riegel_v3 as R3, c6
out = {}
for run, emd in (('V4r5', G.EMD_V4), ('V4r4', os.path.join(G.SFV4, 'emd', 'EM30_V4r4.EMD'))):
    PO = R3.lauf_posen(os.path.join(G.SFV4, 'r', run, 'debug.log')); mo = b2.modell(emd); d = {}
    for h in range(60, 101, 4):
        img, ids = G.zeichne(mo, PO[h]); m = cv2.erode(np.isin(ids[:480], range(16, 23)).astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
        p = c6.port(G.SFV4 + '/r/' + run, h)
        if p is None or m.sum() < 20: d[h] = None; continue
        L = G.lab(cv2.cvtColor(p, cv2.COLOR_BGR2RGB).astype(float)[:480])[m]
        d[h] = dict(n=int(m.sum()), L45=int((L[:, 0] > 45).sum()), L50=int((L[:, 0] > 50).sum()), Lmax=round(float(L[:, 0].max()), 1), mittel=G.r1(L.mean(0)))
    out[run] = d
json.dump(out, open(sys.argv[1], 'w'), indent=1); print(json.dumps(out, indent=0))
