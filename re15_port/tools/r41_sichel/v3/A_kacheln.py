# Leser A V3: Pruefkacheln der Augen- und Maulmasken (8-fach, Konturen: gruen Auge, cyan heller Kern,
# rot Pupille L*<=Lmin+25 %, gelb Zahnflecken, magenta Maulhuelle). Aufruf: A_kacheln.py out.png [keys...]
import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
from A_augen import KOPF, SCHULTER, auge, maul
from PIL import ImageDraw
out = sys.argv[1]; keys = sys.argv[2:]
tiles = []
F = 8
for k in keys:
    nn = int(k[1:]); c = (KOPF if k[0] == 'k' else SCHULTER)[nn]
    rgb, al = load(nn)
    r, mk = auge(nn, c, k)
    rm, mm = (maul(nn, c) if k[0] == 'k' and nn not in (11, 12, 13, 14) else (None, None))
    cx, cy = c
    X0, Y0 = max(cx - 32, 0), max(cy - 14, 0)
    X1, Y1 = min(cx + 36, rgb.shape[1]), min(cy + 46 if k[0] == 'k' else cy + 14, rgb.shape[0])
    if k[0] == 's': X0, X1 = max(cx - 16, 0), min(cx + 17, rgb.shape[1])
    crop = rgb[Y0:Y1, X0:X1]
    im = Image.fromarray(crop).resize((crop.shape[1] * F, crop.shape[0] * F), Image.NEAREST)
    d = ImageDraw.Draw(im)
    def kont(mask, ox, oy, col):
        m = mask.astype(np.uint8)
        cs, _ = cv2.findContours(m, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        for cc in cs:
            for (px, py) in cc[:, 0]:
                X = (px + ox - X0) * F; Y = (py + oy - Y0) * F
                d.rectangle([X + 2, Y + 2, X + F - 3, Y + F - 3], outline=col)
    wx0, wy0 = r['bbox'][0] - int(np.nonzero(mk['eye'])[1].min()), r['bbox'][1] - int(np.nonzero(mk['eye'])[0].min())
    kont(mk['eye'], wx0, wy0, (0, 255, 0)); kont(mk['kern'], wx0, wy0, (0, 255, 255)); kont(mk['pup'], wx0, wy0, (255, 0, 0))
    if mm:
        kont(mm['hull'], mm['win'][0], mm['win'][1], (255, 0, 255)); kont(mm['teeth'], mm['win'][0], mm['win'][1], (255, 255, 0))
    d.rectangle([0, 0, 260, 14], fill=(0, 0, 0)); d.text((2, 1), '%s birkin%02d h%d x%d y%d' % (k, nn, MAP[nn][1], X0, Y0), fill=(255, 255, 255))
    tiles.append(im)
Wt = sum(t.width for t in tiles) + 6 * len(tiles); Ht = max(t.height for t in tiles)
sheet = Image.new('RGB', (Wt, Ht), (255, 255, 255)); x = 0
for t in tiles:
    sheet.paste(t, (x, 0)); x += t.width + 6
sheet.save(out); print(sheet.size)
