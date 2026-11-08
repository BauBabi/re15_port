"""Augenfaecher T19/T20/T41 (+T0): Pixelzahl, Lichtfaktor-Verteilung (lit/raw), Ausdehnung; Varianten mit Rollen (Rx) und Zoom-Abschaetzung."""
import os, sys, json
import numpy as np
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB, kopf_re2_mess as KM
P = G.posen()

def rx(grad):
    a = np.radians(grad); c, s = np.cos(a), np.sin(a)
    return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])
_rot0 = KB.rot
KB.rot = lambda p: _rot0(p) @ rx(p.get('roll', 0.0))

for roll in (0.0, -20.0, -35.0):
    p = json.loads(json.dumps(KB.PARAM)); p['roll'] = roll
    neu, prot, _ = KB.bauen(p)
    mo = b2.modell(neu)
    for h in ('135',):
        il, ids, iv = G.zeichne(mo, P[h], voll=True); ir, _ = G.zeichne(mo, P[h], licht=False)
        Ll = G.lab(il.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
        H = G.h_port(iv)[2]; kb = G.kopfbox(ids)
        for nm, fl in (('fan', [19, 20, 41]), ('fan+T0', [0, 19, 20, 41])):
            _i, idf = G.zeichne(KM.teil(mo, fl), P[h], licht=False)
            m = (idf == 9) & (ids == 9)
            if not m.any():
                print(roll, h, nm, 'nichts sichtbar'); continue
            f = Ll[m] / np.maximum(Lr[m], 1)
            ys, xs = np.nonzero(m)
            print('roll %.0f %s %s: px %d, Lichtfaktor p10/p50/p90 %s, bbox %d x %d px = %.3f x %.3f H, Mitte dx %.3f H links, dy %.3f H' % (
                roll, h, nm, m.sum(), np.percentile(f, [10, 50, 90]).round(2), xs.ptp() + 1, ys.ptp() + 1, (xs.ptp() + 1) / H, (ys.ptp() + 1) / H,
                (0.5 * (kb[0] + kb[2]) - xs.mean()) / H, (ys.mean() - kb[1]) / H))
        # Zaehne und Kopf
        mz = KM.marker_render(mo, P[h], KM.zahn_texel(mo), KB.ZEILE_G3)
        k = ids == 9
        print('   Zaehne L_lit p50/max %.1f / %.1f; Kopf L_lit p50 %.1f; K10 %.3f x %.3f' % (np.median(Ll[mz]), Ll[mz].max(), np.median(Ll[k]), (kb[2] - kb[0] + 1) / H, (kb[3] - kb[1] + 1) / H))
