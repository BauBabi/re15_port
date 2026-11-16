import numpy as np, cv2, json, os
from collections import defaultdict
from zonen_spec import *
PAL = {'rot': (237, 28, 36), 'gruen': (34, 177, 76), 'grau': (127, 127, 127), 'blau': (63, 72, 204), 'lila': (163, 73, 164), 'orange': (255, 127, 39),
       'dunkelrot': (136, 0, 21), 'hellblau': (0, 162, 232), 'braun': (185, 122, 87)}
os.makedirs('zdbg', exist_ok=True)
masken = {}; infos = {}
for z in SPEC:
    m, info = baue(z)
    masken[z['name']] = m; infos[z['name']] = info
    print('%-42s %-32s px %6d %s' % (z['name'], key(z['datei'])[:32], m.sum(), info if info else ''))
np.savez_compressed('zonen_beschriftet.npz', **masken)
json.dump(infos, open('zonen_info.json', 'w'), indent=1)
jd = defaultdict(list)
for z in SPEC:
    jd[z['datei']].append(z)
for lf, zs in jd.items():
    L = lies(D + lf); a = L[..., 3:4] / 255.; im = (L[..., :3] * a + 255 * (1 - a)).astype(np.uint8)
    for z in zs:
        m = masken[z['name']]; col = np.array(PAL[z['farbe']][::-1])
        cnt, _ = cv2.findContours(m.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        im[m] = (im[m] * 0.6 + col * 0.4).astype(np.uint8)
        cv2.drawContours(im, cnt, -1, (0, 0, 0), 1)
    cv2.imwrite('zdbg/%s.png' % key(lf), im)
