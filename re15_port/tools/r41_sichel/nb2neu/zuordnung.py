"""NB2neu: die 21 KI-hochskalierten Einzelbilder des Nutzers (Version_80%/Screenshot 2026-11-03 *.png, 748x565) je Bild dem
Videobild h (Video 4, n = 1350 + h) zuordnen: Bild auf 640x480 skaliert, Graustufen, Maske ohne Schriftzug "Vorschaumodus"
(Mitte), UI-Ecken und Balken; normierte Korrelation gegen jedes Videobild h -20..h200 (Gauss 2 auf beiden), bestes h und
Abstand zum zweitbesten."""
import os, glob, json, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from c6 import *
ORDNER = r'C:/workspace/git/reAi_v2/Version_80%'
dateien = sorted(glob.glob(os.path.join(ORDNER, 'Screenshot 2026-11-03 *.png')))
M = np.zeros((480, 640), bool); M[60:420, 10:630] = True
M[200:260, 200:440] = False                      # Schriftzug Vorschaumodus (Mitte)
M[:, :25] = False; M[:, 615:] = False
def g(im): return cv2.GaussianBlur(cv2.cvtColor(im, cv2.COLOR_BGR2GRAY).astype(np.float32), (0, 0), 2)
V = {h: g(vid(h)) for h in range(-20, 188)}
R = {}
for f in dateien:
    s = cv2.resize(cv2.imread(f), (640, 480), interpolation=cv2.INTER_AREA); sg = g(s)
    a = sg[M]; a = (a - a.mean()) / a.std()
    sc = []
    for h, v in V.items():
        b = v[M]; b = (b - b.mean()) / b.std(); sc.append((float((a * b).mean()), h))
    sc.sort(reverse=True)
    R[os.path.basename(f)] = dict(h=sc[0][1], ncc=round(sc[0][0], 3), zweit=sc[1][1], ncc2=round(sc[1][0], 3))
    print('%-36s h%-4d NCC %.3f  (zweitbestes h%d %.3f)' % (os.path.basename(f), sc[0][1], sc[0][0], sc[1][1], sc[1][0]))
json.dump(R, open(os.path.join(RUNS, 'zuordnung_screenshots.json'), 'w'), indent=1)
