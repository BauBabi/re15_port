"""Der Befund eng: x180..208, y134..180, 14-fach. Links Hintergrund (so muss die
Tischplatte aussehen), Mitte was der Nutzer sieht, rechts nach dem Fix."""
import os
import struct
import sys
import numpy as np
from PIL import Image, ImageDraw

D = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(D, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "re15_port/tools/maske"))
os.chdir(ROOT)
import geom
import geometrie
import maskenbild
import abnahme

SP = ("C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/"
      "d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/vorher/")
bg = np.asarray(Image.open("re15_port/build_r19_marke310501/bg_ppm/ROOM10501.ppm").convert("RGB"), np.int16)
shot = np.asarray(Image.open("C:/workspace/git/reAi_v2/re15_port/build/platform/pc/"
                             "befund_1050_F239_marke3.bmp").convert("RGB").resize((320, 240), Image.BOX), np.int16)
rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)
vzs, ok = geometrie.profil_spalte(R, t, H, 16732, 6350, np.arange(240))
vzs = np.where(ok, vzs, np.nan)


def verd(msk, tim):
    ms = maskenbild.masken(open(msk, "rb").read(), 1)
    idx = maskenbild.lies_tim(tim)[0]
    d, tf = abnahme.deckung_und_tiefe(ms, idx)
    v = np.zeros((240, 320), bool)
    for y in range(240):
        if np.isfinite(vzs[y]):
            v[y] = d[y] & (tf[y] < geometrie.bucket(vzs[y]))
    return v


vA = verd(SP + "ROOM1050.MSK.vorher", SP + "ROOM1050_PRI01.TIM.vorher")
vB = verd("re15_port/shared_assets/PSX/MASKS/ROOM1050.MSK",
          "re15_port/shared_assets/PSX/MASKS/ROOM1050_PRI01.TIM")
A = shot.astype(float).copy(); A[vA] = bg[vA]
B = shot.astype(float).copy(); B[vB] = bg[vB]

x0, x1, y0, y1 = 180, 208, 134, 180
Zf = 14


def kachel(rgb, titel):
    im = Image.fromarray(np.clip(rgb, 0, 255).astype(np.uint8)).crop((x0, y0, x1, y1))
    im = im.resize(((x1 - x0) * Zf, (y1 - y0) * Zf), Image.NEAREST)
    d = ImageDraw.Draw(im)
    for g in range(x0, x1, 5):
        d.line([(g - x0) * Zf, 0, (g - x0) * Zf, (y1 - y0) * Zf], fill=(120, 60, 0))
        d.text(((g - x0) * Zf + 1, 1), str(g), fill=(255, 220, 0))
    for g in range(y0, y1, 5):
        d.line([0, (g - y0) * Zf, (x1 - x0) * Zf, (g - y0) * Zf], fill=(120, 60, 0))
        d.text((1, (g - y0) * Zf + 1), str(g), fill=(255, 220, 0))
    d.text((3, (y1 - y0) * Zf - 16), titel, fill=(255, 255, 255))
    return im


t1 = kachel(bg.copy(), "Hintergrund (Soll der Tischplatte)")
t2 = kachel(A, "VORHER: Bein ueber der Platte")
t3 = kachel(B, "NACHHER")
sheet = Image.new("RGB", ((x1 - x0) * Zf * 3 + 20, (y1 - y0) * Zf), (25, 25, 25))
sheet.paste(t1, (0, 0)); sheet.paste(t2, ((x1 - x0) * Zf + 10, 0))
sheet.paste(t3, ((x1 - x0) * Zf * 2 + 20, 0))
sheet.save(os.path.join(D, "14_eng_x180-208_y134-180.png"))
print("-> 14_eng_x180-208_y134-180.png")
