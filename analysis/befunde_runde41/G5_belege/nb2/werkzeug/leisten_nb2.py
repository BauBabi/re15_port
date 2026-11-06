"""leisten_nb2.py OUTDIR : Nachbesserung 2 - Vergleichsleisten RE2 echt | Port Leon | Port Elza an denselben Szenenbildern
(Schnitt 16 Licht/Masse, Schnitt 17 Speer-Arme, Uebergang Intro -> Kampf).
RE2-Zeit (re2_g5_run1.mp4, Abnahme 1): Bild 0 = 63,48 s, Schnitt 10 79,64 s, Schnitt 5 92,76 s, 30 Spielbilder/s.
Port-Zeit: zeit.json aus zeitachse2.py (t = a + b*F, F = F_arm + Bild), Fensterlaeufe run_port.sh (gdigrab, echtes Fenster)."""
import sys, os, json, subprocess
from PIL import Image, ImageDraw
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
SP = r"C:/Users/mjoedicke/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad"
RE2 = SP + "/re2/re2_g5_run1.mp4"
out = sys.argv[1]
os.makedirs(out, exist_ok=True)
za = {n: json.load(open(SP + "/g5nb2/lauf/%s/zeit.json" % n)) for n in ("leon", "elza")}
GRUPPEN = [
    ("schnitt16_licht", [(100, "Schnitt 16"), (180, "Schnitt 16 Masse"), (240, "Schnitt 16 Masse"), (271, "Schnitt 16")]),
    ("schnitt17_speer", [(450, "Schnitt 17"), (472, "Schnitt 17 (RE2 s13)"), (520, "Schnitt 17"), (600, "Schnitt 17")]),
    ("uebergang_kampf", [(1100, "Cut 12 / Schnitt 5"), (1118, "[T14]"), (1123, "Kampfbeginn"), (1130, "Kampf"), (1150, "Kampf")]),
]
def re2_t(b):
    if b < 437: return 63.48 + b / 30.0
    if b < 807: return 79.64 + (b - 437) / 30.0
    return 92.76 + (b - 807) / 30.0
def bild(video, t, ziel):
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % t, "-i", video, "-frames:v", "1", ziel], check=True)
    return Image.open(ziel).convert("RGB")
W = 427; H = 320
for name, bilder in GRUPPEN:
    zeilen = []
    for b, txt in bilder:
        tiles = [("RE2 echt Bild %d" % b, bild(RE2, re2_t(b), out + "/_re2.png").resize((W, H)))]
        for n in ("leon", "elza"):
            z = za[n]
            t = z["a"] + z["b"] * (z["F_arm"] + b)
            tiles.append(("Port %s Bild %d" % ("5090 Leon" if n == "leon" else "5091 Elza", b),
                          bild(SP + "/g5nb2/lauf/%s/fenster.mp4" % n, t, out + "/_p.png").resize((W, H))))
        zeile = Image.new("RGB", (3 * W, H + 18), (0, 0, 0))
        d = ImageDraw.Draw(zeile)
        for i, (lab, im) in enumerate(tiles):
            zeile.paste(im, (i * W, 18))
            d.text((i * W + 4, 2), lab + "  " + txt, fill=(255, 255, 0))
        zeilen.append(zeile)
    blatt = Image.new("RGB", (3 * W, len(zeilen) * (H + 18)), (0, 0, 0))
    for i, z in enumerate(zeilen): blatt.paste(z, (0, i * (H + 18)))
    blatt.save(out + "/vergleich_nb2_%s.jpg" % name, quality=86)
os.remove(out + "/_re2.png"); os.remove(out + "/_p.png")
print("ok")
