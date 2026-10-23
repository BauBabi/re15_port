"""Vorschlagszonen (magenta, msg-Nummer) + vorhandene Examine-AOTs (gruen) in die Kamerabilder zeichnen."""
import os, sys, struct
REPO, BG, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools', 'maske'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
from geom import build_view
from PIL import Image, ImageDraw
import scd_walk_lib as L

VORSCHLAG = {
    'ROOM1070': [
        ('19 Leiche', 344, 13624, 1000, 1000),
        ('6/5 Dienstplan', 13820, 14401, 1430, 1542),
        ('15 Reihe Ost', 15360, 6600, 1000, 7030), ('15', 18654, 6600, 1000, 7030),
        ('15', 15360, 3054, 1000, 2546), ('15', 18654, 3054, 1000, 2546),
        ('16 Reihe Mitte', 8520, 8582, 1000, 5094), ('16', 12000, 2986, 500, 7233),
        ('17 Reihe West', 1320, 6915, 1000, 6713), ('17', 4988, 6100, 1000, 3500),
        ('13 Regal', 4988, 9600, 1000, 2000), ('14', 4988, 11600, 1000, 2028),
        ('10', 2638, 2604, 1579, 1000),
    ],
    'ROOM1120': [
        ('1 Music Lesson', -197, 1150, 1140, 1000),
        ('2 5 Frauen', -5896, 450, 2271, 1700),
        ('3 Fenster', 6150, -1037, 1000, 4876),
        ('4 Akten', 4950, -5800, 1000, 1850),
        ('8 Pinnwand?', 1800, -6300, 3600, 1000),
    ],
}
SCHNITTE = {'ROOM1070': [1, 2, 3, 4, 5, 7], 'ROOM1120': [2, 3, 4]}

def proj(R, t, H, wx, wy, wz):
    vx = (R[0] * wx + R[1] * wy + R[2] * wz >> 12) + t[0]
    vy = (R[3] * wx + R[4] * wy + R[5] * wz >> 12) + t[1]
    vz = (R[6] * wx + R[7] * wy + R[8] * wz >> 12) + t[2]
    if vz <= 16: return None
    return (160 + H * vx / vz, 120 + H * vy / vz)

os.makedirs(OUT, exist_ok=True)
for room, zonen in VORSCHLAG.items():
    rdt = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE1', room + '.RDT'), 'rb').read()
    cam_off = struct.unpack_from('<I', rdt, 0x24)[0]
    alt = []
    for key, ops in L.regionen(rdt).items():
        if key[0] != 'main': continue
        for pc, op, sz in ops:
            if op == 0x2C and rdt[pc + 2] == 1:
                x, z = struct.unpack_from('<hh', rdt, pc + 6); w, d = struct.unpack_from('<HH', rdt, pc + 10)
                alt.append(('m%d' % struct.unpack_from('<H', rdt, pc + 14)[0], x, z, w, d))
    for cut in SCHNITTE[room]:
        rec = cam_off + cut * 32
        fov = struct.unpack_from('<H', rdt, rec + 2)[0]
        P = struct.unpack_from('<iii', rdt, rec + 4); T = struct.unpack_from('<iii', rdt, rec + 0x10)
        R, t, H = build_view(fov, *P, *T)
        im = Image.open(os.path.join(BG, 'ROOM%s%02d.ppm' % (room[4:7], cut))).convert('RGB').resize((640, 480), Image.NEAREST)
        dr = ImageDraw.Draw(im)
        def poly(x, z, w, d, farbe, txt):
            pts = [proj(R, t, H, ex, 0, ez) for ex, ez in ((x, z), (x + w, z), (x + w, z + d), (x, z + d))]
            if any(p is None for p in pts): return
            pts = [(p[0] * 2, p[1] * 2) for p in pts]
            dr.polygon(pts, outline=farbe); dr.polygon([(a + 1, b) for a, b in pts], outline=farbe)
            cx = sum(p[0] for p in pts) / 4; cy = sum(p[1] for p in pts) / 4
            if -50 < cx < 690 and -50 < cy < 530: dr.text((cx - 10, cy - 5), txt, fill=farbe)
        for (txt, x, z, w, d) in alt: poly(x, z, w, d, (0, 255, 0), txt)
        for (txt, x, z, w, d) in zonen: poly(x, z, w, d, (255, 0, 255), txt)
        im.save(os.path.join(OUT, '%s_C%d_vorschlag.png' % (room, cut)))
print('ok')
