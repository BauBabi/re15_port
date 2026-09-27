#!/usr/bin/env python3
"""MESSUNG: die Hoehe des Waffen-Knochens (Bone 11) je AIM-CLIP und Bild.

Rechnet dieselbe Vorwaerts-Kinematik wie der Port, mit denselben Zahlen:
  * Hierarchie + Bind-Offsets aus PL00.EMR   (emd_common.c:143-183)
  * Keyframes aus der W-Bank (PL00W0x.EMR ist reiner Keyframe-Strom, bones_table=0)
  * 12-Bit-Euler, 36 Bit je Bone ab Keyframe-Byte +12  (emd_common.c:219-283)
  * mat3_from_euler = RE1.5-RotMatrix @0x80068130      (skeleton_common.c:134-160)
  * sin/cos aus re15_trig_lut.c (DAT_800794c4, 4096 Woerter)
  * Wurzel-Translation = Keyframe px/py/pz, Kinder = parent_rot*bind + parent_trans
    (skeleton_common.c:717-729)
  * Weltkoordinate: out[1] = actor_y + trans[1] (Gier dreht Y nicht,
    skeleton_common.c:756)
Die Hoehe UEBER DEN FUESSEN ist damit  -trans[1]  (PSX-Y zeigt nach unten).

usage: zielpose_fk.py <PLD-Verzeichnis> [W-Bank z.B. PL00W03] [clip,clip,...]
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
LUT_C = os.path.join(ROOT, 're15_port', 'engine', 'src', 're15_trig_lut.c')


def load_lut():
    txt = open(LUT_C, 'r', encoding='utf-8', errors='replace').read()
    vals = [int(m, 16) for m in re.findall(r'0x([0-9a-fA-F]{8})u', txt)]
    assert len(vals) == 4096, "LUT hat %d Eintraege" % len(vals)

    def s16(v):
        return v - 0x10000 if v & 0x8000 else v
    cos = [s16(v >> 16) for v in vals]
    sin = [s16(v & 0xffff) for v in vals]
    return cos, sin


COS, SIN = load_lut()


def c(a):
    return COS[a & 0xfff]


def s(a):
    return SIN[a & 0xfff]


def s16c(v):
    v &= 0xffff
    return v - 0x10000 if v & 0x8000 else v


def euler(ax, ay, az):
    """mat3_from_euler, Term fuer Term wie skeleton_common.c:134-160."""
    sx, cx = s(ax), c(ax)
    sy, cy = s(ay), c(ay)
    sz, cz = s(az), c(az)
    nsy = -sy
    m = [0] * 9
    m[2] = s16c(sy)
    m[5] = s16c((-(cy * sx)) >> 12)
    m[8] = s16c((cy * cx) >> 12)
    m[0] = s16c((cz * cy) >> 12)
    m[1] = s16c((-(sz * cy)) >> 12)
    t1 = (cz * nsy) >> 12
    m[3] = s16c(s16c((sz * cx) >> 12) - s16c((t1 * sx) >> 12))
    m[6] = s16c(s16c((sz * sx) >> 12) + s16c((t1 * cx) >> 12))
    t2 = (sz * nsy) >> 12
    m[4] = s16c(s16c((cz * cx) >> 12) + s16c((t2 * sx) >> 12))
    m[7] = s16c(s16c((cz * sx) >> 12) - s16c((t2 * cx) >> 12))
    return m


def mat_mul(a, b):
    o = [0] * 9
    for r in range(3):
        for col in range(3):
            acc = 0
            for k in range(3):
                acc += a[r * 3 + k] * b[k * 3 + col]
            o[r * 3 + col] = acc >> 12
    return o


def mat_apply(m, v):
    return [(m[r * 3] * v[0] + m[r * 3 + 1] * v[1] + m[r * 3 + 2] * v[2]) >> 12
            for r in range(3)]


def parse_emr(path):
    d = open(path, 'rb').read()
    bt, kf_off, nb, kf_sz = struct.unpack_from('<4H', d, 0)
    out = {'nb': nb, 'kf_sz': kf_sz, 'kf': d[kf_off:], 'raw': d, 'bt': bt}
    if bt:                                            # volle EMR mit Hierarchie
        out['bind'] = [struct.unpack_from('<3h', d, 8 + i * 6) for i in range(nb)]
        parent = [-1] * nb
        for i in range(nb):
            cnt, coff = struct.unpack_from('<2H', d, bt + i * 4)
            for k in range(cnt):
                ci = d[bt + coff + k]
                if ci < nb:
                    parent[ci] = i
        out['parent'] = parent
    out['kf_count'] = len(out['kf']) // kf_sz
    return out


def parse_edd(path):
    d = open(path, 'rb').read()
    cnt0, off0 = struct.unpack_from('<2H', d, 0)
    nclip = off0 // 4
    clips = [(0, cnt0)]
    nxt = cnt0
    cur = 4
    for _ in range(1, nclip):
        ci = struct.unpack_from('<H', d, cur)[0]
        cur += 4
        clips.append((nxt, ci))
        nxt += ci
    frames = [struct.unpack_from('<I', d, off0 + i * 4)[0] for i in range(nxt)]
    return clips, frames


def angles(kfdata, kf_sz, kfi, bone):
    base = kfi * kf_sz + 12
    blk = kfdata[base:base + kf_sz - 12]
    bit = bone * 36

    def rd(o):
        v = 0
        for i in range(12):
            bi = (bit + o + i)
            if (bi >> 3) >= len(blk):
                break
            v |= ((blk[bi >> 3] >> (bi & 7)) & 1) << i
        return v - 4096 if v & 0x800 else v
    return rd(0), rd(12), rd(24)


def bone11_y(skel, kfdata, kf_sz, kfi):
    nb = skel['nb']
    par, bind = skel['parent'], skel['bind']
    rot = [None] * nb
    tr = [None] * nb
    px, py, pz = struct.unpack_from('<3h', kfdata, kfi * kf_sz)
    for b in range(nb):
        ax, ay, az = angles(kfdata, kf_sz, kfi, b)
        lr = euler(ax, ay, az)
        p = par[b]
        if p < 0:
            rot[b] = lr
            tr[b] = [px, py, pz]
        else:
            rot[b] = mat_mul(rot[p], lr)
            ro = mat_apply(rot[p], list(bind[b]))
            tr[b] = [ro[i] + tr[p][i] for i in range(3)]
    return tr[11][1], py


def main():
    pld = sys.argv[1]
    bank = sys.argv[2] if len(sys.argv) > 2 else 'PL00W03'
    skel = parse_emr(os.path.join(pld, 'PL00.EMR'))
    wemr = parse_emr(os.path.join(pld, bank + '.EMR'))
    clips, frames = parse_edd(os.path.join(pld, bank + '.EDD'))
    want = ([int(x) for x in sys.argv[3].split(',')] if len(sys.argv) > 3
            else list(range(len(clips))))
    print("Bank %s: %d Clips, %d Keyframes (Groesse %d B)"
          % (bank, len(clips), wemr['kf_count'], wemr['kf_sz']))
    print("PL00-Bindpose Bone 11 ueber den Fuessen: %d"
          % -(skel['bind'][0][1] + skel['bind'][9][1] + skel['bind'][10][1]
              + skel['bind'][11][1]))
    print("%-5s %-6s %-9s %-9s %-9s" % ("Clip", "Bilder", "Muend.min", "Muend.max", "Wurzel-py"))
    for ci in want:
        if ci >= len(clips):
            continue
        first, n = clips[ci]
        if n <= 0:
            print("%-5d %-6d  (leer)" % (ci, n))
            continue
        lo, hi, rlo, rhi = 1 << 30, -(1 << 30), 1 << 30, -(1 << 30)
        for f in range(n):
            kfi = frames[first + f] & 0xfff
            if kfi >= wemr['kf_count']:
                continue
            y, py = bone11_y(skel, wemr['kf'], wemr['kf_sz'], kfi)
            h = -y                              # Hoehe ueber den Fuessen
            lo, hi = min(lo, h), max(hi, h)
            rlo, rhi = min(rlo, -py), max(rhi, -py)
        print("%-5d %-6d %-9d %-9d %d..%d" % (ci, n, lo, hi, rlo, rhi))


main()
