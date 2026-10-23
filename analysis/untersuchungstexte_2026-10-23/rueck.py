"""Rueckprojektion: Pixel (320x240) eines Cuts auf eine Wandebene (x=const oder z=const) -> Weltpunkt."""
import os, sys, struct
import numpy as np
REPO = sys.argv[1]
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools', 'maske'))
from geom import build_view

def view(room, cut):
    rdt = open(os.path.join(REPO, 're15_port', 'shared_assets', 'PSX', 'STAGE1', room + '.RDT'), 'rb').read()
    cam_off = struct.unpack_from('<I', rdt, 0x24)[0]
    rec = cam_off + cut * 32
    fov = struct.unpack_from('<H', rdt, rec + 2)[0]
    P = struct.unpack_from('<iii', rdt, rec + 4); T = struct.unpack_from('<iii', rdt, rec + 0x10)
    return build_view(fov, *P, *T)

def rueck(room, cut, u, v, achse, wert):
    R, t, H = view(room, cut)
    M = np.array(R, dtype=float).reshape(3, 3) / 4096.0
    tt = np.array(t, dtype=float)
    # Bedingungen: (u-160)*vz - H*vx = 0 ; (v-120)*vz - H*vy = 0 ; Weltkoordinate achse = wert
    A = np.zeros((3, 3)); b = np.zeros(3)
    A[0] = (u - 160) * M[2] - H * M[0]; b[0] = -((u - 160) * tt[2] - H * tt[0])
    A[1] = (v - 120) * M[2] - H * M[1]; b[1] = -((v - 120) * tt[2] - H * tt[1])
    A[2, {'x': 0, 'y': 1, 'z': 2}[achse]] = 1; b[2] = wert
    return np.linalg.solve(A, b)

if __name__ == '__main__':
    room = sys.argv[2]
    for spec in sys.argv[3:]:
        name, cut, u, v, achse, wert = spec.split(':')
        p = rueck(room, int(cut), float(u), float(v), achse, float(wert))
        print('%-18s cut%s px(%s,%s) %s=%s -> x=%7.0f y=%7.0f z=%7.0f' % (name, cut, u, v, achse, wert, p[0], p[1], p[2]))
