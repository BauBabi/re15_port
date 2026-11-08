"""Zahlen-Pruefer: Lichtgegenprobe b2-Render gegen RE15_FRAMEDUMP (eigene Entzerrung Framedump 960x720 -> Video 640x480)."""
import sys, os, re, numpy as np, cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
from pz_mess import vraum, lab, de
S = r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv4/'
r_, kam, M, li = b2.raum()
T = np.array([[1 / 3.0, 0, BB.PHASE], [0, 1 / 3.0, BB.PHASE], [0, 0, 1]])
Hf = np.asarray(M, float) @ T
mo = b2.modell(G.EMD_V4)
txt = open(S + 'r/V4/debug.log', encoding='latin-1').read()
for h in (118, 125, 135):
    m = re.search(r'\[hangar-takt\] h%d F(\d+) B clip (\d+) bild (\d+) gier (\d+) \((-?\d+),(-?\d+)\)' % h, txt)
    F, c, b, g, x, z = map(int, m.groups())
    pose = dict(clip=c, bild=b, gier=g, x=x, z=z)
    img, ids = G.zeichne(mo, pose)
    fd = cv2.cvtColor(cv2.imread(S + 'r/V4/fd/f%06d.ppm' % F), cv2.COLOR_BGR2RGB)
    fdv = cv2.warpPerspective(fd, Hf, (640, 480), flags=cv2.INTER_AREA)
    rm = cv2.erode(np.isin(ids, (7, 8)).astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    km = cv2.erode((ids == 9).astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    sil_r = ids >= 0
    # Silhouetten-Deckung: Figur im Framedump (Abweichung vom Hintergrund) gegen ids
    bg = G.hintergrund().astype(float)
    fig_fd = np.abs(fdv.astype(float) - bg).sum(2) > 60
    iou = (fig_fd & sil_r).sum() / max(1, (fig_fd | sil_r).sum())
    a, n = lab(vraum(img)[rm].mean(0)), rm.sum(); bb = lab(vraum(fdv)[rm].mean(0))
    ka, kb = lab(vraum(img)[km].mean(0)), lab(vraum(fdv)[km].mean(0))
    print('h%d F%d clip %d bild %d: Rumpf b2 %s / Spiel %s dE00 %.2f (%d px); Kopf b2 %s / Spiel %s dE00 %.2f (%d px); IoU Figur %.2f' % (
        h, F, c, b, np.round(a, 1), np.round(bb, 1), de(a, bb), n, np.round(ka, 1), np.round(kb, 1), de(ka, kb), km.sum(), iou))
    cv2.imwrite(S + 'pz/licht_h%d.png' % h, cv2.cvtColor(np.hstack([img, fdv]), cv2.COLOR_RGB2BGR))
