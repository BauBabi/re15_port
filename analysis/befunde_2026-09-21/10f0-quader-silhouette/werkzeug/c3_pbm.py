import os, sys, struct
sys.path.insert(0, os.path.abspath("re15_port/tools/maske"))
import geom

CD = "re15_port/shared_assets/PSX"
M = CD + "/MASKS"
faelle = []
for f in sorted(os.listdir(M)):
    if not f.endswith(".TIM"):
        continue
    room, rest = f[:8], f[13:15]
    cut = int(f[12:14]) if False else int(f.split("_PRI")[1][:2])
    rdt, st = geom.load_rdt(CD, room)
    if rdt is None:
        print("kein RDT", room); continue
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    orig = geom.original_has_masks(rdt, cam, cut)
    hat_pbm = os.path.exists(os.path.join(M, "%s_PRI%02d.PBM" % (room, cut)))
    faelle.append((room, cut, orig, hat_pbm))

print("Cut-TIMs gesamt:", len(faelle))
print("mit Original-Sektion im RDT:", sum(1 for a in faelle if a[2]))
print("ohne Original, MIT PBM     :", sum(1 for a in faelle if not a[2] and a[3]))
print("ohne Original, OHNE PBM    :", sum(1 for a in faelle if not a[2] and not a[3]))
for a in faelle:
    if not a[2] and not a[3]:
        print("   ⛔ nachgezeichnet, aber KEIN Silhouetten-Riegel:", a[0], "Cut", a[1])
