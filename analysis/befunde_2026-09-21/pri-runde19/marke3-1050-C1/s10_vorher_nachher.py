"""Messung 7: vorher/nachher am BILD. Die Figurpunkte aus dem Screenshot gegen den
Hintergrund; wo die Maske sie verdeckt, wird der Hintergrund wieder eingesetzt. Das ist
kein Nachbau der Engine, sondern genau ihre Regel (bg_pc.c/render_pc.c: Maskentexel
zeichnen, Original-Test depth < (1023*vz)>>16) auf denselben Daten.
"""
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

bg = np.asarray(Image.open("re15_port/build_r19_marke310501/bg_ppm/ROOM10501.ppm").convert("RGB"), np.int16)
shot = np.asarray(Image.open("C:/workspace/git/reAi_v2/re15_port/build/platform/pc/"
                             "befund_1050_F239_marke3.bmp").convert("RGB").resize((320, 240), Image.BOX), np.int16)
gemalt = np.abs(shot - bg).max(2) > 18

rdt, st = geom.load_rdt("re15_port/shared_assets/PSX", "ROOM1050")
cam = struct.unpack_from("<I", rdt, 0x24)[0]
R, t, H = geom.cut_view(rdt, cam, 1)
PX, PZ = 16732, 6350
vzs, ok = geometrie.profil_spalte(R, t, H, PX, PZ, np.arange(240))
vzs = np.where(ok, vzs, np.nan)


def lade(msk, tim):
    ms = maskenbild.masken(open(msk, "rb").read(), 1)
    idx = maskenbild.lies_tim(tim)[0]
    return abnahme.deckung_und_tiefe(ms, idx) + (len(ms),)


A = lade("C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/vorher/ROOM1050.MSK.vorher", "C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/d917ef01-c258-42fe-ac63-7a1e284c7456/scratchpad/vorher/ROOM1050_PRI01.TIM.vorher")
B = lade("re15_port/shared_assets/PSX/MASKS/ROOM1050.MSK",
         "re15_port/shared_assets/PSX/MASKS/ROOM1050_PRI01.TIM")
print("vorher: %d Rechtecke, Deckung %d Texel | nachher: %d Rechtecke, Deckung %d Texel"
      % (A[2], A[0].sum(), B[2], B[0].sum()))
print("Deckung identisch: %s (XOR %d)" % ((A[0] == B[0]).all(), int((A[0] ^ B[0]).sum())))


def verdeckung(deck, tief):
    v = np.zeros((240, 320), bool)
    for y in range(240):
        if np.isfinite(vzs[y]):
            v[y] = deck[y] & (tief[y] < geometrie.bucket(vzs[y]))
    return v


vA = verdeckung(A[0], A[1])
vB = verdeckung(B[0], B[1])
print()
print("Figurpunkte im Bild: %d" % gemalt.sum())
print("  davon von der Maske verdeckt  VORHER: %4d   NACHHER: %4d"
      % (int((gemalt & vA).sum()), int((gemalt & vB).sum())))
neu = gemalt & vB & ~vA
print("  NEU verdeckte Figurpunkte: %d" % neu.sum())
if neu.any():
    ys, xs = np.nonzero(neu)
    print("    Kasten x%d..%d y%d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
    for y in range(ys.min(), ys.max() + 1):
        xr = np.nonzero(neu[y])[0]
        if len(xr):
            print("      y%3d  x %s  (%d Punkte)" % (y, geom._bereiche(list(xr)), len(xr)))
weg = gemalt & vA & ~vB
print("  Punkte, die VORHER verdeckt waren und jetzt frei sind: %d" % weg.sum())


def render(verd):
    out = shot.astype(float).copy()
    out[verd] = bg[verd]
    return out


Zf = 5
x0, x1, y0, y1 = 170, 260, 60, 210


def kachel(rgb, titel, rot=None):
    im = Image.fromarray(np.clip(rgb, 0, 255).astype(np.uint8)).crop((x0, y0, x1, y1))
    im = im.resize(((x1 - x0) * Zf, (y1 - y0) * Zf), Image.NEAREST)
    d = ImageDraw.Draw(im)
    if rot is not None:
        for (yy, xx) in zip(*np.nonzero(rot)):
            if x0 <= xx < x1 and y0 <= yy < y1:
                d.rectangle([(xx - x0) * Zf, (yy - y0) * Zf, (xx - x0 + 1) * Zf - 1, (yy - y0 + 1) * Zf - 1],
                            outline=(255, 255, 0))
    for g in range(x0, x1, 10):
        d.line([(g - x0) * Zf, 0, (g - x0) * Zf, (y1 - y0) * Zf], fill=(130, 60, 0))
        d.text(((g - x0) * Zf + 1, 1), str(g), fill=(255, 220, 0))
    for g in range(y0, y1, 10):
        d.line([0, (g - y0) * Zf, (x1 - x0) * Zf, (g - y0) * Zf], fill=(130, 60, 0))
        d.text((1, (g - y0) * Zf + 1), str(g), fill=(255, 220, 0))
    d.text((3, (y1 - y0) * Zf - 14), titel, fill=(255, 255, 255))
    return im


a = kachel(render(vA), "VORHER (Spaltenregel)")
b = kachel(render(vB), "NACHHER (Tiefe aus der SCA-Zelle)")
mk = bg.astype(float) * 0.5
mk[neu] = np.array([255, 235, 0])
c = kachel(mk, "gelb = die 244 Figurpunkte, die jetzt zusaetzlich verdeckt sind")
sheet = Image.new("RGB", ((x1 - x0) * Zf * 3 + 20, (y1 - y0) * Zf), (25, 25, 25))
sheet.paste(a, (0, 0)); sheet.paste(b, ((x1 - x0) * Zf + 10, 0)); sheet.paste(c, ((x1 - x0) * Zf * 2 + 20, 0))
sheet.save(os.path.join(D, "10_vorher_nachher.png"))
print("-> 10_vorher_nachher.png (gelb = die Punkte, die jetzt zusaetzlich verdeckt sind)")

# --- Tiefenvergleich je Spalte ---------------------------------------------
print()
print("Tiefe der Maske je Spalte (naechste Tiefe im Spielerkasten y122..190):")
print(" x   vorher   nachher   Leon-bucket dort   Urteil vorher / nachher")
for x in range(180, 224, 2):
    col = slice(122, 191)
    dA = A[1][col, x][A[0][col, x]]
    dB = B[1][col, x][B[0][col, x]]
    if dA.size == 0 and dB.size == 0:
        continue
    bk = int(np.nanmax([geometrie.bucket(vzs[y]) for y in range(122, 191) if np.isfinite(vzs[y])]))
    print("%4d  %6s   %7s   %3d               %s / %s"
          % (x,
             "%d..%d" % (dA.min(), dA.max()) if dA.size else "-",
             "%d..%d" % (dB.min(), dB.max()) if dB.size else "-",
             bk,
             "verdeckt" if (dA.size and dA.min() < bk) else "frei",
             "verdeckt" if (dB.size and dB.min() < bk) else "frei"))
