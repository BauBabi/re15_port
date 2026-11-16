"""Messung PV (Vorlage-Pose, Clip 17 B47 Gier 587, Spielkamera 50A0/12): Schwanz (Knochen 16-22) mit f_rast gezeichnet (Rueckseiten-Culling,
Maler-Reihenfolge, keine anderen Knochen), Breite quer zur projizierten Kette an s .1/.3/.5/.7/.9 (u = 7 s, wie roll_render.stationen_breite der
Analyse), Bildlaenge Kette + Nadel, Spitze. Videoraum 640x480."""
import os, sys, json
import numpy as np
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import f_rast as FR


def norm(v):
    n = np.linalg.norm(v); return v / n if n > 1e-9 else v


def stationen_breite(mask, Q, us):
    res = []
    for u in us:
        i = min(int(u), len(Q) - 2); f = u - i
        c = Q[i] * (1 - f) + Q[i + 1] * f
        t = norm(Q[i + 1] - Q[i]); n = np.array([-t[1], t[0]])
        def lauf(sg):
            k = 0.0
            while k < 150:
                q = c + n * sg * (k + 0.5)
                xi, yi = int(round(q[0])), int(round(q[1]))
                if not (0 <= xi < 640 and 0 <= yi < 480) or not mask[yi, xi]:
                    return k
                k += 0.5
            return k
        res.append(round(lauf(1) + lauf(-1), 1))
    return res


def messen(blob, pose=V.PV):
    mo = V.modell(blob)
    R, T = V.pose_r_t(mo, pose)
    A = FR.zeichne(mo, pose, knochen=list(range(16, 23)), licht=False)
    mask = A['bone'][:480] >= 0
    pts = [T[b].astype(float) for b in range(16, 23)]
    R22 = R[22].astype(float) / 4096.0
    pts.append(T[22].astype(float) + R22[:, 1] * 541.0)
    Q = V.proj(pose, np.array(pts))
    ymax = max(v[1] for v in mo.meshes[22].tv)
    tip = V.proj(pose, (T[22].astype(float) + R22[:, 1] * ymax)[None])[0]
    lang = float(np.linalg.norm(np.diff(Q, axis=0), axis=1).sum() + np.linalg.norm(tip - Q[-1]))
    # tiefster Schwanz-Bildpunkt (Spitze im Bild)
    ys, xs = np.nonzero(mask)
    i = int(np.argmax(ys))
    return dict(breite=stationen_breite(mask, Q, [7 * s for s in (0.1, 0.3, 0.5, 0.7, 0.9)]), laenge=round(lang, 1),
                spitze_nadel=[round(float(x), 1) for x in tip], tiefster_px=[int(xs[i]), int(ys[i])], kette=np.round(Q, 1).tolist(),
                px=int(mask.sum()))


if __name__ == '__main__':
    for p in sys.argv[1:]:
        r = messen(open(p, 'rb').read())
        print(os.path.basename(p), json.dumps({k: v for k, v in r.items() if k != 'kette'}))
