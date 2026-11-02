"""Kamera (RDT) + Projektion, unabhaengig vom Generator geschrieben: sx = 160 + H*x/z, sy = 120 + H*y/z, H = fov >> 7 (camera_common.c, FUN_80053ca4)."""
import struct, numpy as np
RDT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/shared_assets/PSX/STAGE2/ROOM%s.RDT'

def kamera(raum, cut):
    d = open(RDT % raum, 'rb').read()
    rid = struct.unpack_from('<I', d, 0x24)[0]
    o = rid + cut * 0x20
    fl, fov = struct.unpack_from('<HH', d, o)
    p = np.array(struct.unpack_from('<3i', d, o + 4), float)
    t = np.array(struct.unpack_from('<3i', d, o + 16), float)
    f = t - p; f /= np.linalg.norm(f)
    r = np.array([f[2], 0.0, -f[0]]); r /= np.linalg.norm(r)
    u = np.cross(f, r)
    return dict(p=p, r=r, u=u, f=f, H=fov >> 7, off=o)

def proj(k, w):
    d = np.asarray(w, float) - k['p']
    z = d @ k['f']
    return np.array([160 + k['H'] * (d @ k['r']) / z, 120 + k['H'] * (d @ k['u']) / z]), z
