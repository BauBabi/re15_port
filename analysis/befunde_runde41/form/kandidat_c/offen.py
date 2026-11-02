# Messung der benannten Abweichungen: je Zone Video gegen Filter. Keine Anpassung.
from bau import *
import form_pruefung as PR, form_video as FV
Y = np.array([.299, .587, .114], np.float32)
k = PR.Kandidat('lage.json')
for n, zonen in ((1, (('Q-Streifen ueber Z1', 333, 192, 346, 222), ('Scheitel links', 285, 195, 320, 215), ('Rumpf Mitte rechts', 350, 240, 378, 300), ('Kugel NL', 205, 350, 240, 420), ('Hand C4', 325, 405, 365, 435))),
                 (2, (('Bein FL unten', 350, 345, 366, 378), ('Bein FR unten', 385, 345, 410, 378), ('Rumpf Mitte', 290, 225, 340, 280), ('Unterrand G', 300, 318, 345, 352)))):
    s, alb, m, _ = k.rendern(n)
    gef = FV.video_filter(s, bild=n, entzerren=False, rauschen=False).astype(np.float32)
    o = BILD[n].astype(np.float32)
    for nm, x0, y0, x1, y1 in zonen:
        p = PR.rechteck(x0, y0, x1, y1)
        print(n, '%-22s' % nm, 'Figur %.2f' % (m & p).sum().__truediv__(p.sum()), 'Video', np.round(o[p].mean(0)).astype(int), 'Filter', np.round(gef[p].mean(0)).astype(int), 'Y', round(float((o[p] @ Y).mean())), round(float((gef[p] @ Y).mean())))
a = cv2.imread('spiel_pose1_r5080_s1.png')[:, :, ::-1].astype(np.float32)
c = a[110:265, 295:425]
print('Spiel Pose 1 Figur Median', np.median(c.reshape(-1, 3), 0), 'p90', np.percentile(c.reshape(-1, 3), 90, axis=0), 'Mittel', np.round(c.reshape(-1, 3).mean(0)))
b = cv2.imread('spiel_pose2_r5090_s11.png')[:, :, ::-1].astype(np.float32)
c = b[190:300, 260:400]
print('Spiel Pose 2 Rumpf Median', np.median(c.reshape(-1, 3), 0), 'p90', np.percentile(c.reshape(-1, 3), 90, axis=0), 'Mittel', np.round(c.reshape(-1, 3).mean(0)))
