import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools')
import numpy as np, form_buehne as FB
raum = sys.argv[1]; x, z = int(sys.argv[2]), int(sys.argv[3])
R = FB.Raum(raum)
for i in range(R.n):
    c = R.schnitt(i)
    k = FB.Kamera.aus_rid(c['pos'], c['ziel'], c['fov'])
    for y in (0, -900, -1800):
        p = np.array([x, y, z], float)
        q = (np.array(k.V, float) / 4096.0) @ p + np.array(k.t, float)
        if q[2] < 64: continue
    pts = []
    for y in (0, -1800):
        q = (np.array(k.V, float) / 4096.0) @ np.array([x, y, z], float) + np.array(k.t, float)
        sx, sy = FB._gte(np.array([int(q[0])]), np.array([int(q[1])]), np.array([int(q[2])]), k.h)
        pts.append((int(sx[0]), int(sy[0]), int(q[2])))
    ok = all(0 <= p[0] < 320 and -40 <= p[1] < 280 and p[2] > 64 for p in pts)
    print(i, 'Fuss/Kopf', pts, 'IM BILD' if ok else '')
