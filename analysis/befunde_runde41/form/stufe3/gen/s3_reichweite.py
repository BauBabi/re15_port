"""Stufe 3: Reichweite der Hiebe gegen die Trefferprobe des Ticks (bb_nah: Teil 18/13, Radius 1700, oder Versatz (0,600,0)
Radius 1500, @0x80101d38 / @0x80101d50), Spieler 2600 vor der Form (wie test_birkin_bahnsteig). Ausgabe je Bild."""
import sys, math, numpy as np
import s3_rig as S, form_emd as FE
r = S.Rig()
f = FE.lese_emd(open(sys.argv[1], 'rb').read())
abst = float(sys.argv[2]) if len(sys.argv) > 2 else 2600
for c, teil in ((3, 18), (4, 13)):
    zeile = []
    for i, b in enumerate(f['clips'][c]['bilder']):
        R, T = r.fk(r.aus_keyframe(f['pool'][b['kf']]))
        p0 = T[teil]
        p1 = T[teil] + R[teil] @ np.array([0, 600.0, 0])
        d0 = math.hypot(p0[0] - abst, p0[2]); d1 = math.hypot(p1[0] - abst, p1[2])
        tr = 'T' if b['wort'] & FE.BILD_TREFFER else ' '
        trifft = d0 < 1700 or d1 < 1500
        zeile.append('%d%s%s' % (i, tr, '*' if trifft else ''))
    print('Clip %d (Teil %d):' % (c, teil), ' '.join(zeile))
