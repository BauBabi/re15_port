"""Stufe 3: groesste Drehung je Knochen zwischen Folgebildern eines Clips (Grad), zur Fehlersuche."""
import sys, math, numpy as np
import s3_rig as S, form_emd as FE
r = S.Rig()
f = FE.lese_emd(open(sys.argv[1], 'rb').read())
grenze = float(sys.argv[3]) if len(sys.argv) > 3 else 40
for ci in [int(x) for x in sys.argv[2].split(',')]:
    prev = None
    for bi, b in enumerate(f['clips'][ci]['bilder']):
        R, T = r.fk(r.aus_keyframe(f['pool'][b['kf']]))
        if prev is not None:
            for j in range(32):
                m = prev[j].T @ R[j]
                d = math.degrees(math.acos(max(-1, min(1, (np.trace(m) - 1) / 2))))
                if d > grenze:
                    print('Clip %d Bild %d Knochen %d %-16s %.0f Grad' % (ci, bi, j, S.NAMEN[j], d))
        prev = R
