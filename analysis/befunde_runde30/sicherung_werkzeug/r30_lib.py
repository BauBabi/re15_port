#!/usr/bin/env python3
"""Gemeinsame Lesefunktionen der Runde-30-Sicherungswerkzeuge (nur LESEND).

Formate, jeweils mit der Quelle im Port:
  RDT-Kopf / Kameratabelle @[0x24]  32 B je Cut        re15_port/engine/src/rdt_common.c:236-243
  RDT-Modelltabelle @[0x30]        (TIM,MD1) je Prop   re15_port/engine/src/rdt_common.c:51-79
  MD1  Kopf 12 B, Mesh-Kopf 56 B                       re15_port/engine/src/md1_common.c:41-126
  Kamera -> Sichtmatrix (ganzzahlig)                   re15_port/engine/src/camera_common.c:59-112
  Prop-Drehung Ry*Rx*Rz + Elternkette                  re15_port/platform/pc/main.c:549-620
"""
import math
import os
import re
import struct

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def rdt_laden(name):
    stage = "STAGE%s" % name[4]
    with open(os.path.join(PSX, stage, name), "rb") as f:
        return f.read()


def cuts(rdt):
    n = rdt[1]
    start = u32(rdt, 0x24)
    aus = []
    for i in range(n):
        o = start + i * 32
        flag, fov = struct.unpack_from("<HH", rdt, o)
        px, py, pz, tx, ty, tz = struct.unpack_from("<6i", rdt, o + 4)
        pri = u32(rdt, o + 28)
        aus.append(dict(idx=i, off=o, flag=flag, fov=fov, pos=(px, py, pz),
                        tgt=(tx, ty, tz), pri=pri))
    return aus


def props(rdt):
    n = rdt[2]
    tbl = u32(rdt, 0x30)
    ents = [u32(rdt, tbl + 4 * k) for k in range(2 * n)]
    grenzen = sorted(set([e for e in ents if e] + [len(rdt)] +
                         [u32(rdt, o) for o in range(0x08, 0x60, 4)
                          if 0 < u32(rdt, o) <= len(rdt)]))

    def ende(a):
        for g in grenzen:
            if g > a:
                return g
        return len(rdt)
    aus = []
    for k in range(n):
        tim, md1 = ents[2 * k], ents[2 * k + 1]
        aus.append(dict(idx=k, tim_off=tim, tim=rdt[tim:ende(tim)] if tim else b"",
                        md1_off=md1, md1=rdt[md1:ende(md1)] if md1 else b""))
    return aus


def md1_lesen(d):
    """-> dict(length, unknown, nobj, meshes). Je Mesh: tv/qv Punktlisten, tris/quads
    Indexlisten (Datei-Reihenfolge v0..v3), tuv/quv die UV-Records."""
    ln, unk, nobj = struct.unpack_from("<III", d, 0)
    aus = []
    for i in range(nobj // 2):
        h = 12 + i * 56
        (tv, tvc, tn, tnc, tf, tfc, tu,
         qv, qvc, qn, qnc, qf, qfc, qu) = struct.unpack_from("<14I", d, h)
        m = dict(tv=[], qv=[], tris=[], quads=[], tuv=[], quv=[],
                 kopf=(tv, tvc, tn, tnc, tf, tfc, tu, qv, qvc, qn, qnc, qf, qfc, qu))
        for k in range(tvc):
            m["tv"].append(struct.unpack_from("<3h", d, 12 + tv + k * 8))
        for k in range(qvc):
            m["qv"].append(struct.unpack_from("<3h", d, 12 + qv + k * 8))
        for k in range(tfc):
            n0, v0, n1, v1, n2, v2 = struct.unpack_from("<6H", d, 12 + tf + k * 12)
            m["tris"].append((v0, v1, v2))
            m["tuv"].append(struct.unpack_from("<BBHBBHBBH", d, 12 + tu + k * 12))
        for k in range(qfc):
            n0, v0, n1, v1, n2, v2, n3, v3 = struct.unpack_from("<8H", d, 12 + qf + k * 16)
            m["quads"].append((v0, v1, v2, v3))
            m["quv"].append(struct.unpack_from("<BBHBBHBBHBBH", d, 12 + qu + k * 16))
        aus.append(m)
    return dict(length=ln, unknown=unk, nobj=nobj, meshes=aus)


def inc_bytes(name):
    """Liest das Bytefeld <name> aus gen/sicherung_prop.inc."""
    p = os.path.join(REPO, "re15_port", "engine", "src", "gen", "sicherung_prop.inc")
    t = open(p, "r", encoding="utf-8").read()
    m = re.search(r"%s\[(\d+)\]\s*=\s*\{(.*?)\};" % re.escape(name), t, re.S)
    b = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", m.group(2)))
    assert len(b) == int(m.group(1)), (len(b), m.group(1))
    return b


def tim_lesen(d):
    """TIM 4/8bpp mit CLUT -> (breite, hoehe, idx[h][w], clut[n][256] als 15-Bit-Woerter,
    kopf-dict). Breite in PIXELN (Datei fuehrt 16-Bit-Worte)."""
    magic, flag = struct.unpack_from("<II", d, 0)
    assert magic == 0x10, hex(magic)
    bpp = flag & 7
    o = 8
    cluts = []
    ck = None
    if flag & 8:
        ln, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, o)
        ck = dict(len=ln, x=cx, y=cy, w=cw, h=ch)
        for r in range(ch):
            cluts.append(list(struct.unpack_from("<%dH" % cw, d, o + 12 + r * cw * 2)))
        o += ln
    ln, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, o)
    if bpp == 1:
        w = iw * 2
        roh = d[o + 12:o + 12 + w * ih]
        idx = [list(roh[y * w:(y + 1) * w]) for y in range(ih)]
    elif bpp == 0:
        w = iw * 4
        roh = d[o + 12:o + 12 + iw * 2 * ih]
        idx = []
        for y in range(ih):
            z = []
            for x in range(iw * 2):
                b = roh[y * iw * 2 + x]
                z.append(b & 15)
                z.append(b >> 4)
            idx.append(z)
    else:
        raise SystemExit("TIM bpp=%d nicht vorgesehen" % bpp)
    return w, ih, idx, cluts, dict(flag=flag, clut=ck, img=dict(len=ln, x=ix, y=iy, w=iw, h=ih))


def rgb15(c):
    return ((c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3)


# ---- ganzzahlige Mathematik des Ports ------------------------------------
def sin_q12(a):
    """Nur fuer Vielfache von 1024 EXAKT (0, +-4096); genau diese kommen hier vor
    (Plattform rot_y=2048, Sicherung 0). Andere Winkel waeren eine Naeherung."""
    return int(round(math.sin((a & 4095) * 2 * math.pi / 4096) * 4096))


def cos_q12(a):
    return sin_q12(a + 1024)


def squareroot0(x):
    """Exakte Ganzzahlwurzel. Die Engine nutzt die BIOS-Tabellennaeherung SquareRoot0
    (camera_common.c:77); die Abweichung liegt unter einem Pixel und aendert die
    Sichtbarkeitsaussage nicht. Die MESSUNG im Spiel (Framedump) ist massgeblich."""
    return int(math.isqrt(int(x)))


def _tdiv(a, b):
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


def view_bauen(cut):
    dx = cut["tgt"][0] - cut["pos"][0]
    dy = cut["tgt"][1] - cut["pos"][1]
    dz = cut["tgt"][2] - cut["pos"][2]
    dist = squareroot0(dx * dx + dy * dy + dz * dz)
    horiz = squareroot0(dx * dx + dz * dz)
    sp = _tdiv(-dy * 4096, dist)
    cp = _tdiv(horiz * 4096, dist)
    sy = _tdiv(dx * 4096, horiz)
    cy = _tdiv(dz * 4096, horiz)
    r = [cy, 0, -sy,
         (sp * sy) >> 12, cp, (sp * cy) >> 12,
         (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    n = [-c for c in cut["pos"]]
    t = [(r[0] * n[0] + r[1] * n[1] + r[2] * n[2]) >> 12,
         (r[3] * n[0] + r[4] * n[1] + r[5] * n[2]) >> 12,
         (r[6] * n[0] + r[7] * n[1] + r[8] * n[2]) >> 12]
    return dict(rot=r, trans=t, H=min(cut["fov"] >> 7, 4095))


def prop_rot(rx, ry, rz):
    rsx, rcx = sin_q12(rx), cos_q12(rx)
    rsy, rcy = sin_q12(ry), cos_q12(ry)
    rsz, rcz = sin_q12(rz), cos_q12(rz)
    m = [0] * 9
    m[0] = (rcz * rcy) >> 12
    m[1] = -((rsz * rcy) >> 12)
    m[2] = rsy
    m[3] = (((rsz * rcx) << 12) + rcz * rsy * rsx) >> 24
    m[4] = (((rcz * rcx) << 12) - rsz * rsy * rsx) >> 24
    m[5] = -((rcy * rsx) >> 12)
    m[6] = (((rsz * rsx) << 12) - rcz * rsy * rcx) >> 24
    m[7] = (((rcz * rsx) << 12) + rsz * rsy * rcx) >> 24
    m[8] = (rcy * rcx) >> 12
    return m


def verketten(pr, pt, r, t):
    """Weltmatrix = ELTERN o LOKAL (main.c:603-617 / FUN_80022da0)."""
    nr = [0] * 9
    for row in range(3):
        for col in range(3):
            nr[row * 3 + col] = sum(pr[row * 3 + k] * r[k * 3 + col] for k in range(3)) >> 12
    nt = [((pr[row * 3] * t[0] + pr[row * 3 + 1] * t[1] + pr[row * 3 + 2] * t[2]) >> 12)
          + pt[row] for row in range(3)]
    return nr, nt


def gte_divide(h, sz):
    if sz == 0:
        return 0x1FFFF
    n = ((h * 0x20000) // sz + 1) // 2
    return min(n, 0x1FFFF)


def view_x_welt(view, wrot, wpos):
    r, t = view["rot"], view["trans"]
    cr = [sum(r[row * 3 + k] * wrot[k * 3 + col] for k in range(3)) >> 12
          for row in range(3) for col in range(3)]
    ct = [((r[row * 3] * wpos[0] + r[row * 3 + 1] * wpos[1] + r[row * 3 + 2] * wpos[2]) >> 12)
          + t[row] for row in range(3)]
    return cr, ct


def projiziere(view, cr, ct, v):
    """Modellpunkt v -> (sx, sy, vz) wie main.c:9640-9655. None = Near-Clip (vz<64)."""
    vx = ((v[0] * cr[0] + v[1] * cr[1] + v[2] * cr[2]) >> 12) + ct[0]
    vy = ((v[0] * cr[3] + v[1] * cr[4] + v[2] * cr[5]) >> 12) + ct[1]
    vz = ((v[0] * cr[6] + v[1] * cr[7] + v[2] * cr[8]) >> 12) + ct[2]
    if vz < 64:
        return None
    ir1 = max(-0x8000, min(0x7FFF, vx))
    ir2 = max(-0x8000, min(0x7FFF, vy))
    n = gte_divide(view["H"], min(vz, 0xFFFF))
    return 160 + ((ir1 * n) >> 16), 120 + ((ir2 * n) >> 16), vz
