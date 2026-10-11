# Projektion von Moebel-Hoehen in ROOM1010 Cut 0/1 (Engine-Matrizen aus probe_r34n_e_dokumente kamera 1010)
import sys, os
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r36_zombieki/re15_port/tools/r34n_e")
import geom
from PIL import Image, ImageDraw
S = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/nb1"
cams = geom.lade_kameras("1010")
for cut in (0, 1):
    cam = cams[cut]
    im = Image.open(os.path.join(S, "nb1_r1010_c%d.png" % cut)).convert("RGB")
    sx, sy = im.size[0] / 320.0, im.size[1] / 240.0
    d = ImageDraw.Draw(im)
    def pt(p, col, r=2):
        q = geom.projiziere(cam, p)
        if q is None: return None
        x, y = q[0] * sx, q[1] * sy
        d.ellipse([x - r, y - r, x + r, y + r], outline=col)
        return (round(q[0], 1), round(q[1], 1))
    print("Cut", cut)
    # Tisch #5 Oberkante y=-1600: Ecken
    for p in [(-50, -1600, 1550), (2450, -1600, 1550), (2450, -1600, 5900), (-50, -1600, 5900)]:
        print("  Tisch-Ecke", p, "->", pt(p, (255, 255, 0), 3))
    # Stuhlkreise #12 (2450,4700), #13 (2450,2800), #14 (-100,3400): Saeule y 0..-2400
    for name, cx, cz in [("#12", 2450, 4700), ("#13", 2450, 2800), ("#14", -100, 3400)]:
        for y, col in [(0, (0, 255, 0)), (-800, (0, 200, 255)), (-1600, (255, 255, 0)), (-2000, (255, 128, 0)), (-2400, (255, 0, 0))]:
            print("  Stuhl", name, "y", y, "->", pt((cx, y, cz), col))
    im.save(os.path.join(S, "nb1_r1010_c%d_hoehen.png" % cut))
