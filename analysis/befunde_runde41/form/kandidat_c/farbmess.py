from bau import *
import form_pruefung as PR, form_video as FV
k = PR.Kandidat('lage.json')
for n in (1, 2):
    s, alb, m, _ = k.rendern(n)
    gef = FV.video_filter(s, bild=n, entzerren=False, rauschen=False).astype(np.float32)
    o = BILD[n].astype(np.float32)
    zonen = [(q['id'], q['polygone']) for q in PR.stellen() if q['bild'] == n]
    if n == 2:
        zonen.append(('Haken innen', [FB2['H']['polygon']]))
    for i, pp in zonen:
        p = np.zeros((480, 640), bool)
        for q in pp:
            p |= PR.poly_maske(q)
        if i == 'Haken innen':
            p = cv2.erode(p.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool)
        p &= m
        if p.sum() < 6:
            continue
        print(n, '%-12s' % i, 'Video', np.round(o[p].mean(0)).astype(int), 'Filter', np.round(gef[p].mean(0)).astype(int), 'Albedo', np.round(alb[p].mean(0)).astype(int))
    w = cv2.erode(m.astype(np.uint8), np.ones((9, 9), np.uint8)).astype(bool) & (ZIEL[n] == 255)
    Y = np.array([.299, .587, .114], np.float32)
    print(n, 'Figur Mittel Video', np.round(o[w].mean(0)).astype(int), 'Filter', np.round(gef[w].mean(0)).astype(int), 'Median Y', round(float(np.median(o[w] @ Y)), 1), round(float(np.median(gef[w] @ Y)), 1))
