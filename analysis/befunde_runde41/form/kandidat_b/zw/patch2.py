s = open('b_fit.py', encoding='utf-8').read()
a = s.index("if __name__ == '__main__':")
s = s[:a] + '''if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--aus', default='zw/fit3')
    ap.add_argument('--iter', type=int, default=1200)
    ap.add_argument('--gier', type=float, default=2336)
    ap.add_argument('--x', type=float, default=2626)
    ap.add_argument('--z', type=float, default=-23499)
    a = ap.parse_args()
    os.makedirs(os.path.dirname(a.aus) or '.', exist_ok=True)
    # Sicht Ende: Pose 2 = Pose 1, nur Bein FL schwingt angehoben nach vorn (Boden vor der Figur ist frei, FORMBLATT 6B)
    lg = np.array([a.x, 0.0, a.z])

    def lokal2welt(p):
        return Y(a.gier) @ np.array(p, float) + lg
    ziele = {'fl_k': lokal2welt((-420, -1040, -780)), 'fl_a': lokal2welt((-120, -640, -820)), 'fl_f': lokal2welt((-20, -470, -820))}
    lauf(a.aus, a.gier, lage0=(a.x, a.z), ziele=ziele, iters=a.iter)
'''
s = s.replace("l_w = (mod.w2[rumpf] ** 2).sum() * 4.0 + (mod.w2 ** 2).sum() * 0.01", "l_w = (mod.w2[rumpf] ** 2).sum() * 4.0 + (mod.w2[starr] ** 2).sum() * 1.0 + (mod.w2 ** 2).sum() * 0.01")
s = s.replace("    i_d = [i for k, i in mod.ni.items() if k.startswith('d')]\n", "    i_d = [i for k, i in mod.ni.items() if k.startswith('d')]\n    starr = [T.IDX[x] for x in ('nl_ober', 'nl_unter', 'nl_faust', 'nl_innen', 'nr_ober', 'nr_unter', 'nr_faust', 'nr_innen', 'fr_unter', 'fr_fuss', 'fr_zeh')]\n")
# Haken in Bild 2 staerker gewichten
s = s.replace("    t, w, z = lade_masken(n)\n        xi, yi", "XX")
s = s.replace("""        w[g & f] = 0.5
    return t, w, z""", """        w[g & f] = 0.5
        h = cv2.imread(HIER + '/zw/haken.png', 0)
        if h is not None:
            w[h > 0] = np.maximum(w[h > 0], 1.5)
            t[h > 0] = 1.0
    return t, w, z""")
open('b_fit.py', 'w', encoding='utf-8').write(s)
import sys, cv2, numpy as np
sys.path.insert(0, '.')
from b_geom import *
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
m = P.poly_maske(P.teil_b2('H')['polygon']).astype(np.uint8) * 255
cv2.imwrite('zw/haken.png', m)
print(m.sum() / 255)
