import sys, cv2, numpy as np
sys.path.insert(0, '.')
s = open('b_teile.py', encoding='utf-8').read()
s = s.replace("KUPPEL_MITTE = np.array([-100.0, -2010.0, -100.0])", "KUPPEL_MITTE = np.array([-420.0, -2020.0, -100.0])")
s = s.replace("n('d0', c, 980)", "n('d0', c, 1050)")
s = s.replace("n('d%d' % (i + 1), c + d * (960 - rr), rr)", "n('d%d' % (i + 1), c + d * (1040 - rr), rr)")
s = s.replace("n('dt', c + np.array([0, -560, 0]), 430)", "n('dt', c + np.array([0, -600, 0]), 430)")
s = s.replace("n('du', c + np.array([0, 520, 0]), 480)", "n('du', c + np.array([0, 560, 0]), 480)")
s = s.replace("FEST = {'rumpf_f': (-620.0, -2000.0, -100.0)", "FEST = {'rumpf_f': (-940.0, -2000.0, -100.0)")
open('b_teile.py', 'w', encoding='utf-8').write(s)
s = open('b_fit.py', encoding='utf-8').read()
s = s.replace("starr = [T.IDX[x] for x in ('nl_ober', 'nl_unter', 'nl_faust', 'nl_innen', 'nr_ober', 'nr_unter', 'nr_faust', 'nr_innen', 'fr_unter', 'fr_fuss', 'fr_zeh')]",
              "starr = [T.IDX[x] for x in ('nl_ober', 'nl_unter', 'nl_faust', 'nl_innen', 'nr_ober', 'nr_unter', 'nr_faust', 'nr_innen', 'fr_ober', 'fr_unter', 'fr_fuss', 'fr_zeh')]")
open('b_fit.py', 'w', encoding='utf-8').write(s)
s = open('b_mesh.py', encoding='utf-8').read()
s = s.replace("k_kuppel=70.0", "k_kuppel=30.0")
open('b_mesh.py', 'w', encoding='utf-8').write(s)
from b_geom import *
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_pruefung as P
m = P.poly_maske(P.teil_b2('H')['polygon']).astype(np.uint8) * 255
e = P.teil_b2('Y')['ellipse']
cv2.ellipse(m, tuple(e['mitte']), (e['halbachsen'][0] - 8, e['halbachsen'][1] - 8), 0, 0, 360, 255, -1)
m[436:, :253] = 0
cv2.imwrite('zw/haken.png', m)
print(e, (m > 0).sum())
