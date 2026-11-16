"""Koerperumriss (Id-Maske ohne Schwanz) des Laufs je h: linker/rechter Rand in Bildzeilen (Videoraum)."""
import sys, numpy as np
sys.path.insert(0, '.')
import messen as M
import b2
lauf = sys.argv[1]
mo = b2.modell(M.EMD); P = M.pose_log(lauf + '/pose.log')
for h, zeilen in ((18, (255, 262, 270)), (22, (255, 262, 270)), (24, (265, 273, 280)), (26, (330, 345)), (30, (340, 360, 370)),
                  (60, (315, 320, 330)), (64, (315, 320, 330)), (68, (315, 322, 330)), (76, (195, 205, 215)), (84, (195, 205, 215)),
                  (90, (190, 205, 215, 230)), (96, (205, 215, 225))):
    p = P[h + 243]
    pose = dict(clip=p['clip'], bild=p['bild'], gier=p['gier'], x=p['x'], y=p['y'], z=p['z'])
    _b, ids = b2.zeichne(mo, pose, R_T=(p['R'], p['T']), licht=False)
    k = (ids >= 0) & ((ids < 16) | (ids > 22))
    aus = []
    for y in zeilen:
        xs = np.nonzero(k[y])[0]
        aus.append('y%d: %s' % (y, ('%d..%d' % (xs.min(), xs.max())) if len(xs) else '-'))
    ys, xs = np.nonzero(k)
    print(h, 'Koerper x %d..%d y %d..%d |' % (xs.min(), xs.max(), ys.min(), ys.max()), ' '.join(aus))
