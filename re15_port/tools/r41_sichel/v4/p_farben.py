"""Pruefer V4: Farbstichprobe je Region in beiden Vorlagebildern.
Bild 1 (n1485) entblendet: I' = 17 + (I-17)/k, k = 0,766/0,764/0,768 (Schwarzwert 17).
Ausgabe je Region: Masken-Pixel (2 px erodiert, ausser Flecken), RGB, Lab (D65, OpenCV), Lab nach
Schwarzwert-Abzug 17, Port-Schirm-Ziel P = (V - a)/b (Eichung p_eichung.py, sigma 1,5:
a = 9,42/11,96/10,94, b = 0,9979/0,9901/0,9964) und dessen Lab.
Aufruf: python p_farben.py OUTDIR"""
import sys, json, numpy as np, cv2
from PIL import Image
OUT = sys.argv[1]; V = r"C:/workspace/git/reAi_v2/Version_80%/"
K1 = np.array([0.766, 0.764, 0.768]); EA = np.array([9.42, 11.96, 10.94]); EB = np.array([0.9979, 0.9901, 0.9964])
def lade(b):
    a = np.asarray(Image.open(V + f"Beschreibung{b}_Vorlage.png").convert("RGB")).astype(float)
    return 17 + (a - 17) / K1 if b == 1 else a
def lab(c):
    x = np.clip(np.array(c, float).reshape(1, 1, 3) / 255.0, 0, 1).astype(np.float32)
    return [round(float(v), 1) for v in cv2.cvtColor(x, cv2.COLOR_RGB2Lab)[0, 0]]
def box(b, x0, y0, x1, y1, bed=None, ero=2):
    a = IM[b]; m = np.zeros(a.shape[:2], np.uint8); sub = np.ones((y1 - y0 + 1, x1 - x0 + 1), bool)
    if bed is not None: sub = bed(a[y0:y1 + 1, x0:x1 + 1])
    m[y0:y1 + 1, x0:x1 + 1] = sub
    if ero: m = cv2.erode(m, np.ones((2 * ero + 1, 2 * ero + 1), np.uint8))
    return m.astype(bool)
def kreis(b, cx, cy, r, bed, ero=2):
    a = IM[b]; yy, xx = np.mgrid[:480, :640]; m = (((xx - cx) ** 2 + (yy - cy) ** 2) <= r * r) & bed(a)
    m = m.astype(np.uint8)
    if ero: m = cv2.erode(m, np.ones((2 * ero + 1, 2 * ero + 1), np.uint8))
    return m.astype(bool)
IM = {1: lade(1), 2: lade(2)}
L_ = lambda a: a @ np.array([0.299, 0.587, 0.114])
RG = lambda t: (lambda a: (a[..., 0] - a[..., 1]) >= t)
CH = lambda t: (lambda a: (a[..., 0] - (a[..., 1] + a[..., 2]) / 2) >= t)
REG = [
    (1, "Kopf rote Kuppe", lambda: box(1, 388, 140, 406, 156, RG(15)), "box x388-406 y140-156, R-G>=15"),
    (1, "Auge vorn (heller Fleck)", lambda: box(1, 374, 162, 381, 171, lambda a: L_(a) > 96.4, 0), "Fleck p_blob: L>96,4 in bbox x374-381 y162-171, nicht erodiert"),
    (1, "Haare vorn (dunkles Feld)", lambda: box(1, 393, 152, 408, 178, lambda a: L_(a) < 35, 1), "box x393-408 y152-178, L<35, 1 px erodiert"),
    (1, "Nase", lambda: box(1, 389, 174, 397, 181, None, 0), "box x389-397 y174-181"),
    (1, "Kinn/Maulzone dunkel", lambda: box(1, 378, 185, 398, 194, None, 0), "box x378-398 y185-194"),
    (1, "Brust rot", lambda: box(1, 372, 197, 424, 232, RG(15)), "box x372-424 y197-232, R-G>=15"),
    (1, "Oberarm (B) links = (W) rechts", lambda: box(1, 307, 167, 347, 212), "box x307-347 y167-212"),
    (1, "Oberarm (B) rechts = (W) links", lambda: box(1, 437, 182, 477, 227), "box x437-477 y182-227"),
    (1, "Jeans", lambda: box(1, 374, 264, 437, 297), "box x374-437 y264-297"),
    (1, "Klaue (B) rechts rot", lambda: box(1, 482, 330, 507, 356, RG(15)), "box x482-507 y330-356, R-G>=15"),
    (1, "Bezug Wand links", lambda: box(1, 40, 150, 119, 299), "box x40-119 y150-299"),
    (2, "Kopf rote Zone oben", lambda: box(2, 418, 193, 441, 211, RG(25)), "box x418-441 y193-211, R-G>=25"),
    (2, "Auge seitlich (heller Fleck)", lambda: box(2, 448, 230, 457, 243, lambda a: L_(a) > 97.7, 0), "Fleck p_blob: L>97,7 in bbox x448-457 y230-243"),
    (2, "Haare seitlich (dunkle Vorderkante)", lambda: box(2, 457, 228, 465, 248, lambda a: L_(a) < 40, 0), "box x457-465 y228-248, L<40"),
    (2, "Buckel hell", lambda: box(2, 372, 195, 414, 227, lambda a: L_(a) >= 80), "box x372-414 y195-227, L>=80"),
    (2, "Schwanz Wurzel (Unterseite)", lambda: kreis(2, 345, 238, 12, CH(6)), "Kreis r12 um (345,238), Chroma R-(G+B)/2>=6"),
    (2, "Schwanz Bogen breit (Unterseite)", lambda: kreis(2, 305, 250, 10, CH(6)), "Kreis r10 um (305,250), Chroma>=6"),
    (2, "Schwanz Bogen Aussenseite hell", lambda: box(2, 278, 212, 300, 250, lambda a: L_(a) >= 55, 1), "box x278-300 y212-250, L>=55 (beleuchtete Aussenkante)"),
    (2, "Schwanz unter dem Knick", lambda: box(2, 283, 314, 296, 342, CH(4), 0), "box x283-296 y314-342, Chroma>=4"),
    (2, "Jeans seitlich", lambda: box(2, 397, 298, 432, 327), "box x397-432 y298-327"),
    (2, "Bezug Wand links", lambda: box(2, 40, 150, 119, 299), "box x40-119 y150-299"),
]
res = []
for b, name, mk, beschr in REG:
    m = mk(); px = int(m.sum())
    if px == 0:
        print(b, name, "LEER"); continue
    rgb = IM[b][m].mean(0); sd = IM[b][m].std(0)
    ziel = np.clip((rgb - EA) / EB, 0, 255)
    r = {"bild": b, "region": name, "fenster": beschr, "px": px, "rgb": rgb.round(1).tolist(), "std": sd.round(1).tolist(),
         "lab": lab(rgb), "lab_sw17": lab(np.clip(rgb - 17, 0, 255)), "port_ziel_rgb": ziel.round(1).tolist(), "port_ziel_lab": lab(ziel)}
    res.append(r)
    print(f"{b} {name:38s} px{px:5d} RGB {r['rgb']} Lab {r['lab']} SW {r['lab_sw17']} Ziel {r['port_ziel_rgb']} {r['port_ziel_lab']}")
json.dump(res, open(OUT + "/P_farben.json", "w"), indent=1, ensure_ascii=False)
