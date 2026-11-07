# Spielbilder neben die Vorlagen: Spielbild durch denselben Filter, zum Ansehen. Keine Messung.
import sys
from kern import *
import form_video as FV
a = cv2.imread('spiel_pose2_r5090_s11.png')[:, :, ::-1]
f = FV.video_filter(a, bild=2, rauschen=False)
o = BILD[2]
z = np.hstack([o[130:430, 180:470], f[130:430, 180:470], a[100:400, 150:440]])
cv2.imwrite('spiel_vergleich_bild2.png', cv2.resize(z, None, fx=1.5, fy=1.5, interpolation=cv2.INTER_NEAREST)[:, :, ::-1])
