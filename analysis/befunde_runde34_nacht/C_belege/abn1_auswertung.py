#!/usr/bin/env python3
"""Spur C, ABNAHME 1: Framedumps eines abn1_lauf.sh-Laufs auswerten.
Je Bild (320x240, naechster Nachbar aus RE15_WINDOW_SCALE=3):
  gruen_o / gruen_u = Zahl der "leuchtend gruenen" Pixel (G >= 150, G-R >= 60, G-B >= 60) in den
                      Lampenquadraten (212..233 / 65..86 bzw. 125..146) -- unabhaengig vom Cut, damit
                      auch ein Leck auf einen FREMDEN Hintergrund (Cut 8, 0x0D, 0x0E) auffaellt;
  dG_o / dG_u       = mittleres G der Glasflaeche minus Original-Hintergrund ROOM11F10.bmp (nur Cut 10
                      aussagekraeftig);
dazu die Zeile des RE15_PANEL_LOG des VORBILDS (Bild F zeigt den Stand F-1) und aus RE15_STATE_LOG
Spielerlage, Kamera, Meldung, Pausenwort.
Aufruf: abn1_auswertung.py <laufverzeichnis>"""
import os, sys, glob, re
from PIL import Image
BG10 = "C:/workspace/git/reAi_v2/extracted/PSX/STAGE1/ROOM11F/ROOM11F10.bmp"
GLAS = {"o": (218, 228, 72, 80), "u": (218, 229, 131, 140)}
QUAD = {"o": (212, 65), "u": (212, 125)}
d = sys.argv[1]
bgp = Image.open(BG10).convert("RGB").load()

def lese(pfad, schluessel_re):
    out = {}
    if not os.path.exists(pfad):
        return out
    for z in open(pfad, errors="replace"):
        m = re.match(r"F(\d+) ", z)
        if m:
            out[int(m.group(1))] = z.rstrip("\n")
    return out
plog = lese(os.path.join(d, "panel.log"), None)
slog = lese(os.path.join(d, "state.log"), None)
def kv(z):
    return dict(t.split("=", 1) for t in z.split()[1:] if "=" in t) if z else {}
def mittel_g(p, box):
    x0, x1, y0, y1 = box; s = 0; n = 0
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            s += p[x, y][1]; n += 1
    return s / float(n)
def gruen(p, ecke):
    x0, y0 = ecke; n = 0
    for y in range(y0, y0 + 22):
        for x in range(x0, x0 + 22):
            r, g, b = p[x, y]
            if g >= 150 and g - r >= 60 and g - b >= 60:
                n += 1
    return n
print("%5s | %4s %4s %6s %6s | %4s %4s %5s %4s %4s %3s %3s | %s" % (
    "F", "gr_o", "gr_u", "dG_o", "dG_u", "cut", "sper", "maske", "wert", "gel", "lo", "lu", "state(F-1)"))
for f in sorted(glob.glob(os.path.join(d, "fd_*.ppm"))):
    n = int(re.search(r"fd_(\d+)", f).group(1))
    im = Image.open(f).convert("RGB")
    if im.width != 320:
        im = im.resize((320, 240), Image.NEAREST)
    p = im.load()
    k = kv(plog.get(n - 1))
    s = slog.get(n - 1, "")
    m = re.search(r"PL\(([-\d]+),([-\d]+),rot=([-\d]+)", s)
    cam = re.search(r"cam=(\d+)", s)
    msg = re.search(r"msg\(a=(\d+)", s)
    pf = re.search(r"pf=([0-9A-F]+)", s)
    st = "PL(%s,%s) cam=%s msg=%s pf=%s" % (m.group(1) if m else "?", m.group(2) if m else "?",
                                            cam.group(1) if cam else "?", msg.group(1) if msg else "?",
                                            pf.group(1) if pf else "?")
    print("%5d | %4d %4d %6.1f %6.1f | %4s %4s %5s %4s %4s %3s %3s | %s" % (
        n, gruen(p, QUAD["o"]), gruen(p, QUAD["u"]),
        mittel_g(p, GLAS["o"]) - mittel_g(bgp, GLAS["o"]), mittel_g(p, GLAS["u"]) - mittel_g(bgp, GLAS["u"]),
        k.get("cut", "?"), k.get("panelsperre", "?"), k.get("maske", "?"), k.get("wert", "?"),
        k.get("geloest", "?"), k.get("lampe_o", "?"), k.get("lampe_u", "?"), st))
