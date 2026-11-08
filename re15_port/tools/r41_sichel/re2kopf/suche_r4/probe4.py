import sys
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
sys.path[:0] = [T + '/r41_sichel/re2kopf', T, T + '/r41_sichel/v4']
import numpy as np, cv2
import v4_gemein as G, b2
v = G.vorlage(2); bg = G.videoraum(G.hintergrund())
Lv, Lb = G.lab(v), G.lab(bg)
dE = np.linalg.norm(Lv - Lb, axis=2)
x0, y0, x1, y1 = 380, 160, 500, 280
d = dE[y0:y1, x0:x1]
im = np.clip(d * 6, 0, 255).astype(np.uint8)
im = cv2.applyColorMap(im, cv2.COLORMAP_JET)
im = cv2.resize(im, None, fx=6, fy=6, interpolation=cv2.INTER_NEAREST)
for gx in range(0, x1 - x0, 10):
    cv2.putText(im, str(x0 + gx), (gx * 6 + 1, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 255, 255), 1)
for gy in range(0, y1 - y0, 10):
    cv2.putText(im, str(y0 + gy), (2, gy * 6 + 4), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 255, 255), 1)
cv2.imwrite('p4_dE.png', im)
# also bg crop
b = cv2.resize(bg[y0:y1, x0:x1].astype(np.uint8), None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)
cv2.imwrite('p4_bg.png', cv2.cvtColor(b, cv2.COLOR_RGB2BGR))
