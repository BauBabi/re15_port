# Pruefbild (nur Arbeitsbild): Hand-Ausschnitt Vorlage | ALT | K1 | K1b | K2 in F368/F371/F375, roh, naechster Nachbar
import sys; sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import numpy as np, cv2, riegel_handfarbe as RF, hand_mass as HM
P = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv8'
staende = [('ALT', P + '/r7d/szene')] + [(k, P + '/r7e/laeufe/' + k) for k in sys.argv[1].split(',')]
zeilen = []
H = 300
for f, b in ((368, 20), (371, 23), (375, 24)):
    a = RF.ppm(P + '/r7d/szene', f); h = RF.ppm(P + '/r7e/mk/lauf_HF_ALT', f)
    k = RF.klassen(h, a)
    m = k['unterarm'] | k['hand'] | k['finger'] | k['daumenkralle'] | k['kastenfinger']
    ys, xs = np.nonzero(m)
    y0, y1, x0, x1 = ys.min() - 8, ys.max() + 8, xs.min() - 8, xs.max() + 8
    teile = []
    v = HM.vorlage(b); im = v['_bild']; al = im[..., 3:4] / 255.0
    bild = (im[..., :3] * al + 64 * (1 - al)).astype(np.uint8)
    mk = v['_maske']; yy, xx = np.nonzero(mk)
    vb = bild[yy.min() - 10:yy.max() + 10, xx.min() - 10:xx.max() + 10]
    vb = cv2.resize(vb, (int(vb.shape[1] * H / vb.shape[0]), H), interpolation=cv2.INTER_NEAREST)
    teile.append(vb)
    for n, d in staende:
        img = RF.ppm(d, f)[..., ::-1].astype(np.uint8)[y0:y1, x0:x1]
        img = cv2.resize(img, (int(img.shape[1] * H / img.shape[0]), H), interpolation=cv2.INTER_NEAREST)
        cv2.putText(img, '%s F%d' % (n, f), (3, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 1)
        teile.append(img)
    zeilen.append(np.hstack(teile))
B = max(z.shape[1] for z in zeilen)
cv2.imwrite(sys.argv[2], np.vstack([np.hstack([z, np.zeros((z.shape[0], B - z.shape[1], 3), np.uint8)]) for z in zeilen]))
print('geschrieben', sys.argv[2])
