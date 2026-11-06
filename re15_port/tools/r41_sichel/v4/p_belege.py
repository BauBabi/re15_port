"""Pruefer V4: Belegbilder. P_linien_bild{1,2}.png (Nutzerlinien-Maske gruen, Endpunkte, Zuordnung + Belegung),
P_kopf.png (Vorlage 1 entblendet x6 / Vorlage 2 x6 mit Auge/Haare/Nase-Messung), P_schwanz.png (Vorlage 2 x4 mit
Beginn, Biegung, Knick, Ende und Breitenmarken). Aufruf: python p_belege.py OUTDIR"""
import sys, json, numpy as np
from PIL import Image, ImageDraw
OUT = sys.argv[1]; V = r"C:/workspace/git/reAi_v2/Version_80%/"
K1 = np.array([0.766, 0.764, 0.768])
L = json.load(open(OUT + "/P_linien.json"))
TXT = {"Beschreibung1": ["Auge", "Haare", "Nase"],
       "Beschreibung2": ["Auge seitlich", "Haare seitlich", "Schwanzspitze Ende", "Schwanz Beginn", "Schwanz Knick"]}
for f, d in L.items():
    a = np.asarray(Image.open(V + f + ".png").convert("RGB")).copy()
    m = (a[:, :, 0] == 237) & (a[:, :, 1] == 28) & (a[:, :, 2] == 36)
    a[m] = (0, 220, 0)
    im = Image.fromarray(a).resize((a.shape[1] * 2, a.shape[0] * 2), Image.NEAREST); dr = ImageDraw.Draw(im)
    best = d["permutationen"][0]["zuordnung"]
    for i, z in enumerate(best):
        s, e = z.split("->"); sx, sy = map(int, s.split(",")); ex, ey = map(int, e.split(","))
        bel = d["matrix"][s][e]["gerade"]
        alt = max(v["gerade"] for k, v in d["matrix"][s].items() if k != e)
        dr.ellipse([ex * 2 - 7, ey * 2 - 7, ex * 2 + 7, ey * 2 + 7], outline=(255, 255, 0), width=2)
        dr.text((ex * 2 + 8, ey * 2 - 16 + (i % 2) * 18), f"{TXT[f][i]} ({ex},{ey}) bel {bel:.2f} alt {alt:.2f}", fill=(255, 255, 0))
    im.save(f"{OUT}/P_linien_{f}.png")
# Kopf
fl = {r["text"]: r for r in json.load(open(OUT + "/P_kopf_flecken.json"))}
tiles = []
for b, box, items in ((1, (355, 130, 425, 200), [("Auge vorn", (255, 255, 0)), ("Haare vorn", (0, 255, 255))]),
                      (2, (405, 180, 475, 260), [("Auge seitlich", (255, 255, 0))])):
    a = np.asarray(Image.open(V + f"Beschreibung{b}_Vorlage.png").convert("RGB")).astype(float)
    if b == 1: a = 17 + (a - 17) / K1
    a = np.clip(a, 0, 255).astype(np.uint8)
    x0, y0, x1, y1 = box; s = 6
    im = Image.fromarray(a[y0:y1, x0:x1]).resize(((x1 - x0) * s, (y1 - y0) * s), Image.NEAREST); dr = ImageDraw.Draw(im)
    for name, col in items:
        bx = fl[name]["bbox"]; sp = fl[name]["sp"]
        dr.rectangle([(bx[0] - x0) * s, (bx[1] - y0) * s, (bx[2] + 1 - x0) * s, (bx[3] + 1 - y0) * s], outline=col, width=2)
        dr.text(((bx[0] - x0) * s, (bx[3] + 1 - y0) * s + 2), f"{name} {fl[name]['w_h'][0]}x{fl[name]['w_h'][1]} SP {sp[0]},{sp[1]}", fill=col)
    if b == 1:
        dr.ellipse([(395 - x0) * s - 6, (177 - y0) * s - 6, (395 - x0) * s + 6, (177 - y0) * s + 6], outline=(255, 0, 255), width=2)
        dr.text(((395 - x0) * s + 8, (177 - y0) * s), "Nase (395,177)", fill=(255, 0, 255))
        dr.rectangle([(388 - x0) * s, (140 - y0) * s, (407 - x0) * s, (157 - y0) * s], outline=(255, 80, 80), width=1)
        dr.text(((388 - x0) * s, (140 - y0) * s - 12), "rote Kuppe", fill=(255, 80, 80))
    else:
        dr.rectangle([(458 - x0) * s, (228 - y0) * s, (466 - x0) * s, (249 - y0) * s], outline=(0, 255, 255), width=2)
        dr.text(((440 - x0) * s, (250 - y0) * s), "Haare seitlich x458-465 y228-248", fill=(0, 255, 255))
        dr.rectangle([(418 - x0) * s, (193 - y0) * s, (442 - x0) * s, (212 - y0) * s], outline=(255, 80, 80), width=1)
        dr.text(((418 - x0) * s, (193 - y0) * s - 12), "rote Zone oben", fill=(255, 80, 80))
    dr.text((4, 4), f"Vorlage {b} ({'n1485 entblendet' if b == 1 else 'n1435'})", fill=(255, 255, 255))
    tiles.append(np.asarray(im))
hmax = max(t.shape[0] for t in tiles)
tiles = [np.pad(t, ((0, hmax - t.shape[0]), (0, 8), (0, 0))) for t in tiles]
Image.fromarray(np.concatenate(tiles, 1)).save(OUT + "/P_kopf.png")
# Schwanz
a = np.asarray(Image.open(V + "Beschreibung2_Vorlage.png").convert("RGB"))
x0, y0, x1, y1, s = 255, 185, 375, 360, 4
im = Image.fromarray(a[y0:y1, x0:x1]).resize(((x1 - x0) * s, (y1 - y0) * s), Image.NEAREST); dr = ImageDraw.Draw(im)
P = lambda x, y: ((x - x0) * s, (y - y0) * s)
pts = [("Beginn (357,243)", (357, 243)), ("", (330, 236)), ("", (305, 241)), ("Biegung ~(295,252)", (295, 252)), ("", (291, 280)), ("Knick (290,308)", (290, 308)), ("Ende (295,343)", (295, 343))]
for i in range(len(pts) - 1):
    dr.line([P(*pts[i][1]), P(*pts[i + 1][1])], fill=(0, 255, 255), width=2)
for name, (x, y) in pts:
    if not name: continue
    dr.ellipse([P(x, y)[0] - 6, P(x, y)[1] - 6, P(x, y)[0] + 6, P(x, y)[1] + 6], outline=(255, 255, 0), width=2)
    dr.text((P(x, y)[0] + 8, P(x, y)[1] - 6), name, fill=(255, 255, 0))
for y, xa, xb in ((271, 279, 297), (287, 277, 296), (328, 284, 292), (336, 283, 293)):
    dr.line([P(xa, y), P(xb, y)], fill=(255, 0, 255), width=2)
    dr.text((P(xb, y)[0] + 4, P(xb, y)[1] - 6), f"{xb - xa + 1} px", fill=(255, 0, 255))
dr.text((4, 4), "Vorlage 2 n1435 x4: Achse (Maskenlesung A/B, Bogen) tuerkis, Nutzerpunkte gelb, Breiten magenta", fill=(255, 255, 255))
im.save(OUT + "/P_schwanz.png")
print("ok")
