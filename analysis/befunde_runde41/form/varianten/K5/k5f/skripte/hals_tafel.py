import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, cv2, rl
S, AUS = sys.argv[1], sys.argv[2]
AUSSEN = {(22, 1, 1), (22, 1, 8), (22, 1, 9), (22, 1, 194), (22, 1, 201), (22, 1, 209)}
def t(img, z, y=16):
    for i, s in enumerate(z): cv2.putText(img, s, (4, y + 15 * i), cv2.FONT_HERSHEY_SIMPLEX, 0.42, (240, 240, 240), 1, cv2.LINE_AA)
reihen = []
for nm, d in (('K5e_f1 (Spiel-Asset)', rl.K5 + '/ruhe/K5e_f1'), ('K5f-Vorschau v7', S + '/vorschau/v7')):
    mo = rl.fl.lade_var(d); z = []
    for az, el, wn in ((0, -85, 'von unten'), (90, -45, 'unten seitlich'), (45, -40, 'unten 3/4')):
        img, K = rl.weiss_id(mo, 2, az, el, 400)
        a = np.zeros(img.shape[:2], bool)
        for (m, q, fi) in AUSSEN: a |= (K['mesh'] == m) & (K['flaeche'] == q * 100000 + fi)
        a &= K['maske']
        o = cv2.cvtColor(img, cv2.COLOR_RGB2BGR); o2 = o.copy(); o2[a] = (0, 255, 0)
        lab = rl.fl.lab(img.reshape(-1, 3)).reshape(img.shape)
        rot = K['maske'] & np.isin(K['mesh'], (21, 22, 23)) & (lab[..., 1] > 12) & (np.hypot(lab[..., 1], lab[..., 2]) > 15) & (np.degrees(np.arctan2(lab[..., 2], lab[..., 1])) > -10)
        c = np.hstack([o, o2]); t(c, ['%s, weiss, %s, Ruhe-Clip 2' % (nm, wn), 'gruen = 6 Flaechen Mesh 22 qf 1/8/9/194/201/209 (%d px)' % a.sum(), 'warm-rot Hals/Kopf/Kiefer: %d px' % rot.sum()])
        z.append(c)
    reihen.append(np.hstack(z))
SP = S + '/spiel'; b = 'dump_000440.ppm'
n = cv2.imread(SP + '/hp1_form_k5e_f1_normal/' + b); k = cv2.imread(SP + '/hp1_form_k5e_f1_key0/' + b); v = cv2.imread(SP + '/hp1_v7/' + b)
m = (n != k).any(2); o = n.copy(); o[m] = (0, 255, 0)
sp = np.hstack([n, o, v]); t(sp, ['Spiel 5090 Schnitt 11, Pose Clip 27 Bild 20 (Griff, Kopf hoch), Framedump: K5e_f1 | Zeile-0-Pixel gruen (Schluessel nur Zeile 0) | K5f-Vorschau v7',
                                 'K5e_f1: 846 von 963 Zeile-0-Pixeln warm-rot (a*>6, Ton>-10 Grad); v7: 0'])
w = max(r.shape[1] for r in reihen + [sp])
pad = lambda r: np.hstack([r, np.zeros((r.shape[0], w - r.shape[1], 3), np.uint8)]) if r.shape[1] < w else r
cv2.imwrite(AUS, np.vstack([pad(r) for r in reihen] + [pad(sp)])); print('ok')
