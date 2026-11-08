"""fdleiste.py <aus.jpg> <titel> <kopf1>=<laufordner1> [<kopf2>=<laufordner2> ...] -- F1 F2 ... : Bildleiste aus RE15_FRAMEDUMP-Serien (fd_NNNNNN.ppm je Spielbild,
Rueckleser des komponierten Bildes vor dem Present, render_pc.c re15_render_pc_request_readback), Spalten = Laeufe, Zeilen = Spielbild F (exakt, kein Videoversatz).
Fehlt ein Bild, wird das naechste vorhandene <= F+Schritt genommen und vermerkt. 320 x 240 je Bild (Fenster 640 x 480 halbiert)."""
import sys, os, glob, re
from PIL import Image, ImageDraw
aus, titel = sys.argv[1], sys.argv[2]
i = sys.argv.index("--")
spalten = [a.split("=", 1) for a in sys.argv[3:i]]
Fs = [int(f) for f in sys.argv[i + 1:]]
W, H, KO, RA = 320, 240, 22, 66
HELL = float(os.environ.get("HELL", "1.0"))   # Belegkopie heller (Faktor je Kanal), Vorgabe 1.0 = Normalhelligkeit
N = len(spalten)
img = Image.new("RGB", (RA + N * W + 6 * (N - 1) + 6, KO + len(Fs) * (H + 4)), (20, 20, 20))
d = ImageDraw.Draw(img)
farbe = [(255, 255, 0), (0, 200, 255), (0, 255, 0), (255, 140, 0)]
for k, (kopf, _) in enumerate(spalten): d.text((RA + k * (W + 6), 4), kopf, fill=farbe[k % 4])
d.text((2, 4), titel[:10], fill=(200, 200, 200))
if HELL != 1.0: d.text((2, 12), "hell x%.1f" % HELL, fill=(255, 120, 120))
vorrat = {}
for k, (_, lauf) in enumerate(spalten):
    fs = {}
    for p in glob.glob(os.path.join(lauf, "fd_*.ppm")):
        m = re.search(r"fd_(\d+)\.ppm$", p)
        if m: fs[int(m.group(1))] = p
    vorrat[k] = fs
for r, F in enumerate(Fs):
    y = KO + r * (H + 4)
    for k, (_, lauf) in enumerate(spalten):
        fs = vorrat[k]; p = fs.get(F); fx = F
        if p is None and fs:
            kand = [f for f in fs if f >= F]
            if kand: fx = min(kand); p = fs[fx]
        if p:
            try:
                im = Image.open(p).convert("RGB").resize((W, H), Image.BILINEAR)
                if HELL != 1.0: im = im.point(lambda v: min(255, int(v * HELL)))

            except Exception: im = Image.new("RGB", (W, H), (60, 0, 0))
        else: im = Image.new("RGB", (W, H), (40, 0, 0))
        img.paste(im, (RA + k * (W + 6), y))
        if fx != F: d.text((RA + k * (W + 6) + 4, y + 4), "F%d" % fx, fill=(255, 120, 120))
    d.text((4, y + H // 2), "F%d" % F, fill=(255, 255, 255))
img.save(aus, quality=88)
print("geschrieben", aus, img.size)
