"""Kopf Pose 135/85: lit | raw | Zonenkarte (grau gruen, dunkel blau, Maul rot, Faecher gelb) | Lichtfaktor; dazu Pixelanteile je Zone."""
import os, sys, json
import numpy as np, cv2
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB, kopf_re2_mess as KM
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mess_r2 as MR
OUT = os.path.dirname(os.path.abspath(__file__))
over = json.loads(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1].startswith('{') else {}
name = sys.argv[2] if len(sys.argv) > 2 else 'zonen'
p = MR.param_mit(over)
neu, prot, _ = KB.bauen(p)
mo = b2.modell(neu)
P = G.posen()
zn = prot['zonen']
for h in ('135', '85'):
    il, ids, iv = G.zeichne(mo, P[h], voll=True); ir, _ = G.zeichne(mo, P[h], licht=False)
    k = ids == 9; kb = G.kopfbox(ids); x0, y0, x1, y1 = kb
    Ll = G.lab(il.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
    zm = np.zeros((480, 640, 3), np.uint8)
    teile = {'grau': (zn['grau_T'], zn['grau_Q'], (0, 200, 0)), 'dunkel': (zn['dunkel_T'], zn['dunkel_Q'], (0, 80, 255)), 'maul': (zn['maul_T'], zn['maul_Q'], (255, 0, 0))}
    ant = {}
    for nm, (tt, qq, col) in teile.items():
        _i, idz = G.zeichne(KM.teil(mo, tt, qq), P[h], licht=False)
        m = (idz == 9) & k
        zm[m] = col; ant[nm] = dict(px=int(m.sum()), L_lit=float(np.median(Ll[m])) if m.any() else None, L_raw=float(np.median(Lr[m])) if m.any() else None,
                                   fak=float(np.median(Ll[m] / np.maximum(Lr[m], 1))) if m.any() else None)
    _i, idf = G.zeichne(KM.teil(mo, [19, 20, 41]), P[h], licht=False); fan = (idf == 9) & k; zm[fan] = (255, 255, 0)
    print(h, 'Kopf px', int(k.sum()), 'Kopf L lit p50 %.1f' % np.median(Ll[k]), json.dumps(ant))
    kbr = (max(x0 - 10, 0), max(y0 - 10, 0), min(x1 + 10, 639), min(y1 + 10, 479))
    crop = lambda a: cv2.resize(a[kbr[1]:kbr[3] + 1, kbr[0]:kbr[2] + 1], None, fx=4, fy=4, interpolation=cv2.INTER_NEAREST)
    fak = np.clip(np.nan_to_num(np.where(Lr > 1, Ll / np.maximum(Lr, 1), 0)) * 200, 0, 255).astype(np.uint8); fm = np.stack([fak] * 3, -1); fm[~k] = 0
    cv2.imwrite(os.path.join(OUT, '%s_%s.png' % (name, h)), cv2.cvtColor(np.hstack([crop(il), crop(ir), crop(zm), crop(fm)]), cv2.COLOR_RGB2BGR))
