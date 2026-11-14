"""Tafel 20 (Runde 8, Schwanz): Nutzerbild Areas*.png | V8 vorher (07b6703f) | neu (Stufe q) in denselben Ansichten.

    python tafel20.py <aus.png> <fd_vorher> <fd_neu> <emd_vorher> <emd_neu> [Name neu]

h22 / h24 / h64: echte Framedumps der Szenenlaeufe (lauf_v4.sh, F = h + 243, 960x720 -> Videoraum 640x480 wie paar.port640 der Analyse:
vx = 2 px + 1,97, vy = 2,079 py + 1,3); h90: Vorlage-Pose PV (Clip 17 B47 Gier 587) offline mit f_rast und RDT-Licht (Szene h90 steht in diesem
Zweig ~93 Grad anders, ANALYSE.md 5.1). Nutzerbilder Version_80%/Areas*.png (nur gelesen; Video im Bild bei y 40-520, Areas.png 320 px versetzt).
"""
import os, sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont
from scipy.ndimage import map_coordinates
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
sys.path.insert(0, W); sys.dont_write_bytecode = True
import v8_gemein as V
import f_rast as FR

V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
NUTZER = {22: ('Areas4.png', 0), 24: ('Areas2.png', 0), 64: ('Areas3.png', 0), 90: ('Areas.png', 320), 91: ('Areas_Tip.png', 0)}
BOX = {22: (170, 200, 450, 450), 24: (140, 200, 420, 450), 64: (340, 230, 620, 480), 90: (200, 150, 480, 400), 91: (230, 270, 342, 370)}
S = 1.6
SK = {91: 4.0}   # Zeile 5: Spitze (Areas_Tip.png, h90) 4x


def port640(fd, h):
    p = np.asarray(Image.open(os.path.join(fd, 'f%06d.ppm' % (h + 243))).convert('RGB'), dtype=float)
    yy, xx = np.mgrid[0:480, 0:640].astype(float)
    px = (xx - 1.97) / 2.0 * 3 + 1.0; py = (yy - 1.3) / 2.079 * 3 + 1.0
    out = np.zeros((480, 640, 3))
    for c in range(3):
        out[..., c] = map_coordinates(p[..., c], [py, px], order=1, mode='nearest')
    return Image.fromarray(out.clip(0, 255).astype(np.uint8))


def pv(emd):
    mo = V.modell(open(emd, 'rb').read())
    A = FR.mit_bg(mo, V.PV)
    return Image.fromarray(A['img'][:480])


def nutzer(h):
    f, dx = NUTZER[h]
    im = Image.open(os.path.join(V80, f)).convert('RGB')
    return im.crop((dx, 40, dx + 640, 520))


def main():
    aus, fd_alt, fd_neu, emd_alt, emd_neu = sys.argv[1:6]
    name_neu = sys.argv[6] if len(sys.argv) > 6 else 'neu'
    try:
        font = ImageFont.truetype('arial.ttf', 15); fk = ImageFont.truetype('arial.ttf', 20)
    except OSError:
        font = fk = ImageFont.load_default()
    zeilen = []
    for h in (22, 24, 64, 90, 91):
        b = BOX[h]
        w, hh = int((b[2] - b[0]) * SK.get(h, S)), int((b[3] - b[1]) * SK.get(h, S))
        if h == 91:
            bilder = [('Nutzer Areas_Tip.png (Spitze h90, 4x)', nutzer(h)), ('V8 vorher, Spitze PV offline (4x)', pv(emd_alt)),
                      ('%s, Spitze PV offline (4x)' % name_neu, pv(emd_neu))]
        elif h == 90:
            bilder = [('Nutzer Areas.png (Video h90)', nutzer(h)), ('V8 vorher 07b6703f, Pose PV offline (wie h90)', pv(emd_alt)),
                      ('%s, Pose PV offline (wie h90)' % name_neu, pv(emd_neu))]
        else:
            bilder = [('Nutzer %s (Video h%d)' % (NUTZER[h][0], h), nutzer(h)), ('V8 vorher 07b6703f, Spiel F%d (h%d)' % (h + 243, h), port640(fd_alt, h)),
                      ('%s, Spiel F%d (h%d)' % (name_neu, h + 243, h), port640(fd_neu, h))]
        T = Image.new('RGB', (3 * (w + 6), hh + 24), (0, 0, 0))
        d = ImageDraw.Draw(T)
        for k, (nm, im) in enumerate(bilder):
            T.paste(im.crop(b).resize((w, hh), Image.LANCZOS), (k * (w + 6), 24))
            d.text((k * (w + 6) + 4, 3), nm, fill=(255, 255, 0), font=font)
        zeilen.append(T)
    Wd = max(t.width for t in zeilen)
    kopf = 64
    S_ = Image.new('RGB', (Wd, kopf + sum(t.height + 8 for t in zeilen)), (0, 0, 0))
    d = ImageDraw.Draw(S_)
    d.text((6, 6), 'TAFEL 20 - Schwanz Runde 8: Nutzerbild | V8 vorher | neu (gleiche Ansicht je Zeile)', fill=(255, 255, 255), font=fk)
    d.text((6, 34), 'h22/h24/h64 echte Framedumps der Szene (Videoraum 640x480); h90 = Vorlage-Pose PV offline (f_rast, RDT-Licht). '
           'Gruene Linien/rote Umrisse nur im Nutzerbild.', fill=(200, 200, 200), font=font)
    y = kopf
    for t in zeilen:
        S_.paste(t, (0, y)); y += t.height + 8
    S_.save(aus)
    print(aus, S_.size)


if __name__ == '__main__':
    main()
