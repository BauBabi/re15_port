"""Pruefung V4 r5 Sicht: Schwanz (Ids 16-22) in 6 Gierwinkeln (Pose 85, neutrales Licht = Texelfarbe, wie Leiste 08) - Texelfarben je
Winkel: Anteil a* > 15, a* > 20, L* > 45, gelb; dazu Farbstreifen-Kachel x2 der Schwanz-Ausschnitte V4 r5 | V4 r4 | REF2."""
import sys, os, json, numpy as np, cv2
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v4')
import v4_gemein as G, b2
OUT = sys.argv[1]; out = {}; kach = []
for nm, emd in (('V4', G.EMD_V4), ('V4r4', os.path.join(G.SFV4, 'emd', 'EM30_V4r4.EMD')), ('REF2', G.REF2)):
    m = b2.modell(emd); p0 = dict(G.posen()['85']); d = {}; row = []
    for dg in range(0, 360, 60):
        p = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
        img, ids = b2.zeichne(m, p, None, licht=False); img = np.ascontiguousarray(img).astype(np.uint8)[:480]; ids = ids[:480]
        sw = np.isin(ids, range(16, 23)); lab = G.lab(img.astype(float))[sw]
        d[dg] = dict(n=int(sw.sum()), a15=round(float((lab[:, 1] > 15).mean()), 3), a20=round(float((lab[:, 1] > 20).mean()), 3),
                     L45=round(float((lab[:, 0] > 45).mean()), 3), gelb=int(((lab[:, 2] > 20) & (lab[:, 1] < 12) & (lab[:, 0] > 25)).sum()), mittel=G.r1(lab.mean(0)))
        ys, xs = np.nonzero(sw)
        if len(xs):
            x0, x1, y0, y1 = max(xs.min() - 5, 0), min(xs.max() + 5, 640), max(ys.min() - 5, 0), min(ys.max() + 5, 480)
            z = img[y0:y1, x0:x1].copy(); z[~sw[y0:y1, x0:x1]] = (40, 40, 40)
            z = cv2.resize(z, (160, 200), interpolation=cv2.INTER_NEAREST); cv2.putText(z, '%s %d' % (nm, dg), (3, 12), 0, 0.4, (0, 255, 255), 1); row.append(z)
    out[nm] = d; kach.append(np.concatenate(row, 1))
cv2.imwrite(OUT + '/PS5_schwanz_rundum.png', cv2.cvtColor(np.concatenate(kach, 0), cv2.COLOR_RGB2BGR))
json.dump(out, open(OUT + '/PS5_schwanz_rundum.json', 'w'), indent=1)
for k, v in out.items(): print(k, json.dumps(v))
