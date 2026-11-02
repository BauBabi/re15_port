"""leisten.py OUTDIR : Vergleichsleisten RE2 echt | Port Leon | Port Elza an denselben Szenenbildern.
RE2-Zeit (re2_g5_run1.mp4): Armierung 63,80 s, Schnitt 10 79,64 s, Schnitt 5 92,76 s (Abnahme 1, 1.2), 30 Spielbilder/s.
Port-Zeit: zeitachse.py (t = a + b*F, F = F_arm + Bild)."""
import sys, os, json, subprocess
from PIL import Image, ImageDraw
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
RE2 = SP + "/re2/re2_g5_run1.mp4"
out = sys.argv[1]
os.makedirs(out, exist_ok=True)
za = {n: json.load(open(SP + "/g5nb1/lauf/%s/zeit.json" % n)) for n in ("leon", "elza")}
BILDER = [(-40, "Vorlauf"), (0, "Armierung"), (12, ""), (30, "Leon/Elza aus dem Bild"), (45, ""), (100, ""), (180, ""), (240, ""), (265, ""), (271, ""), (360, ""),
          (470, "Schnitt 10 / 17"), (560, "Schnitt 10 / 17"), (840, "Schnitt 5 / Cut 12"), (1000, "Schnitt 5 / Cut 12")]
def re2_t(b):
    if b < 437: return 63.48 + b / 30.0   # Bild 0: RAM-Laufbeginn Bild 12 (s03 x 5736 = 1400 + 20*216,8) + Austritt aus dem Bild 64,37 s bei x ~4600
    if b < 807: return 79.64 + (b - 437) / 30.0
    return 92.76 + (b - 807) / 30.0
def bild(video, t, ziel):
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % t, "-i", video, "-frames:v", "1", ziel], check=True)
    return Image.open(ziel).convert("RGB")
W = 427; H = 320
zeilen = []
for b, txt in BILDER:
    tiles = []
    im = bild(RE2, re2_t(b), out + "/_re2.png").resize((W, H))
    tiles.append(("RE2 echt Bild %d" % b, im))
    for n in ("leon", "elza"):
        z = za[n]
        t = z["a"] + z["b"] * (z["F_arm"] + b)
        im = bild(SP + "/g5nb1/lauf/%s/fenster.mp4" % n, t, out + "/_p.png")
        w, h = im.size
        # Fenster: 4:3-Bild mittig (schwarze Raender abschneiden)
        tiles.append(("Port %s Bild %d" % ("5090 Leon" if n == "leon" else "5091 Elza", b), im.resize((W, H))))
    zeile = Image.new("RGB", (3 * W, H + 18), (0, 0, 0))
    d = ImageDraw.Draw(zeile)
    for i, (lab, im) in enumerate(tiles):
        zeile.paste(im, (i * W, 18))
        d.text((i * W + 4, 2), lab + ("  " + txt if txt else ""), fill=(255, 255, 0))
    zeilen.append((b, zeile))
for teil, (a, e) in enumerate(((0, 8), (8, 15))):
    sel = zeilen[a:e]
    blatt = Image.new("RGB", (3 * W, len(sel) * (H + 18)), (0, 0, 0))
    for i, (b, z) in enumerate(sel): blatt.paste(z, (0, i * (H + 18)))
    blatt.save(out + "/vergleich_re2_leon_elza_nb1_%d.jpg" % (teil + 1), quality=85)
for b, z in zeilen:
    if b in (30, 45, 100, 240, 265, 271, 470, 840):
        z.save(out + "/leiste_bild%04d.jpg" % b, quality=88)
os.remove(out + "/_re2.png"); os.remove(out + "/_p.png")
print("ok", len(zeilen))
