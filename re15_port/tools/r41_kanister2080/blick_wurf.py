#!/usr/bin/env python3
"""Runde 41 Spur G: Blickbeziehung des Wurf-Schnitts (Nachtrag 5) - RE2 ROOM7020 Cut 7 gegen alle ROOM2080-Kameras.

Kameras aus den RID-Saetzen (RDT+0x24, 32 B: +4 pos, +16 Ziel). Werfer RE2 = Ada (Pos_set (-28000,-7500,-17900) @SCD 0x0740,
Dir_set 0x0e74 @0x0748); Werfer Port = Marvin/John am Wurf-Stand (-7100,-14400,-17625) Gier 0 (re15_kanister2080_r41.h).
frontal = cos(Blickrichtung des Werfers (cos g, -sin g), Werfer-Kopf -> Kamera) in xz; Kopf = Fuss - 2500.
Aufruf: python re15_port/tools/r41_kanister2080/blick_wurf.py
"""
import math, os, struct
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))


def kameras(pfad):
    d = open(pfad, 'rb').read()
    cam = struct.unpack_from('<I', d, 8 + 7 * 4)[0]
    out = []
    for c in range(d[1]):
        _, _, px, py, pz, tx, ty, tz, _ = struct.unpack_from('<HHiiiiiiI', d, cam + c * 32)
        out.append(((px, py, pz), (tx, ty, tz)))
    return out


def bez(cam, wer, gier):
    kopf = (wer[0], wer[1] - 2500, wer[2])
    d = [cam[i] - kopf[i] for i in range(3)]
    hor = math.hypot(d[0], d[2])
    f = (math.cos(gier * 2 * math.pi / 4096), -math.sin(gier * 2 * math.pi / 4096))
    return (f[0] * d[0] + f[1] * d[2]) / hor, math.degrees(math.atan2(-d[1], hor)), math.sqrt(sum(x * x for x in d))


def zeile(name, cam, wer, gier):
    fr, pi, di = bez(cam, wer, gier)
    print('%-28s frontal %5.2f (%3.0f Grad)  Kamera %5.1f Grad %s dem Kopf  Abstand %6.0f' % (
        name, fr, math.degrees(math.acos(max(-1.0, min(1.0, fr)))), abs(pi), 'ueber' if pi > 0 else 'unter', di))


re2 = kameras(os.path.join(REPO, 'info', 're2leon', 'PL0', 'RDT', 'ROOM7020.RDT'))
zeile('RE2 ROOM7020 Cut 7 (Ada)', re2[7][0], (-28000, -7500, -17900), 0x0e74)
k80 = kameras(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE2', 'ROOM2080.RDT'))
for c, (pos, _) in enumerate(k80):
    zeile('ROOM2080 Cut %d' % c, pos, (-7100, -14400, -17625), 0)
