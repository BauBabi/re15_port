"""Leser fuer den Dump von probe_r16_pri_masken_audit + Kamera-Helfer."""
import numpy as np

def parse(path):
    rooms = {}
    cur = None; curcut = None; curband = None
    lines = open(path).read().split("\n")
    i = 0
    while i < len(lines):
        ln = lines[i]
        if ln.startswith("ROOM "):
            p = ln.split(); rid = int(p[1], 16)
            cur = rooms[rid] = {"cuts": {}, "floor": {}, "msk": int(p[5][4:]), "ncuts": int(p[2][5:])}
        elif ln.startswith("B "):
            curband = int(ln.split()[1]); cur["floor"][curband] = []
        elif ln.startswith("F "):
            p = ln.split(); cur["floor"][curband].append((int(p[1]), int(p[2])))
        elif ln.startswith("C "):
            p = ln.split()
            if p[2] == "KEINE_SICHT":
                i += 1; continue
            c = int(p[1]); kv = {}; j = 2
            while j < len(p):
                if p[j].startswith("rot="):
                    kv["rot"] = [int(p[j][4:])] + [int(v) for v in p[j+1:j+9]]; j += 9; continue
                if p[j].startswith("trans="):
                    kv["trans"] = [int(p[j][6:])] + [int(v) for v in p[j+1:j+3]]; j += 3; continue
                k, v = p[j].split("="); kv[k] = int(v); j += 1
            kv["rects"] = []; cur["cuts"][c] = kv; curcut = kv
        elif ln.startswith("R "):
            p = [int(v) for v in ln.split()[1:]]; curcut["rects"].append(p[1:])
        elif ln.startswith("T "):
            p = ln.split(); curcut["tim"] = {k: int(v) for k, v in (q.split("=") for q in p[2:])}
        elif ln.startswith("D "):
            rows = lines[i+1:i+241]; a = np.zeros((240, 320), bool)
            for y, r in enumerate(rows):
                bits = np.array([int(ch, 16) for ch in r], np.uint8)
                a[y] = np.unpackbits(bits[:, None] << 4, axis=1)[:, :4].reshape(-1)[:320]
            curcut["deck"] = a; i += 240
        elif ln.startswith("P "):
            rows = lines[i+1:i+241]; a = np.zeros((240, 320), np.int32)
            for y, r in enumerate(rows):
                a[y] = [int(r[k:k+3], 16) for k in range(0, 960, 3)]
            curcut["tief"] = a; i += 240
        i += 1
    return rooms

def proj(rot, trans, H, x, y, z):
    vx = (x*rot[0] + y*rot[1] + z*rot[2]) / 4096.0 + trans[0]
    vy = (x*rot[3] + y*rot[4] + z*rot[5]) / 4096.0 + trans[1]
    vz = (x*rot[6] + y*rot[7] + z*rot[8]) / 4096.0 + trans[2]
    if vz <= 64: return None
    return 160 + vx*H/vz, 120 + vy*H/vz, vz

def vz_senkrechte(rot, trans, H, wx, wz, sy):
    a = rot[4] / 4096.0; b = (rot[3]*wx + rot[5]*wz) / 4096.0 + trans[1]
    c = rot[7] / 4096.0; d = (rot[6]*wx + rot[8]*wz) / 4096.0 + trans[2]
    n = (sy - 120.0)*c - H*a
    if abs(n) < 1e-9: return None
    y = (H*b - (sy - 120.0)*d) / n
    return c*y + d

def vz_boden(rot, trans, H, sx, sy, y0):
    a = (sx - 160.0) / H; b = (sy - 120.0) / H
    k = rot[1]*a + rot[4]*b + rot[7]
    c = rot[1]*trans[0] + rot[4]*trans[1] + rot[7]*trans[2]
    if abs(k) < 1e-9: return None
    vz = (y0*4096.0 + c) / k
    return vz if vz > 1 else None
