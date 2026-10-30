#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""belege_n17_n20.py - Runde 41 Spur G, Belege zu Nachtrag 17 (Knall) und Nachtrag 20 (Ebene der Zombies in 2080).

  knall   RE2 ROOM1060 (info/re2leon/PL0/RDT/ROOM1060.RDT): Se_on sub05 @0x0F48, Raumbank-Kopf (+0x08/+0x0C/+0x10),
          EDT[0x0a], VAG 5 @0x066B8 (8768 B) - und derselbe VAG in re15_port/engine/src/gen/knall_bank.inc (Satz 0).
  sca     ROOM2080 SCA @RDT+0x20: alle Zellen (einmal je Inhalt) mit Band = floor >> 4, Treppen-Aot @0x0BE2.
  zlage   <state.log>: Spieler + Zombies (Typ 16) je N Bilder, Extremwerte, Zahl der Saetze westlich der Band-8-Absatzwand
          @0x05C4 (x < -16650 = auf der Treppe).

Aufruf: python re15_port/tools/r41_kanister2080/belege_n17_n20.py knall|sca|zlage <state.log> [schritt]
"""
import hashlib, os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
R1060 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM1060.RDT")
KNALL_INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "knall_bank.inc")
R2080 = os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE2", "ROOM2080.RDT")
ABSATZ_WAND_X = -16650   # SCA @0x05C4 x -17650..-16650 (Ostkante)


def knall():
    d = open(R1060, "rb").read()
    print("ROOM1060.RDT %d B" % len(d))
    print("@0x0F48: %s" % d[0xF48:0xF48 + 12].hex(" "))
    edt, vh, vb = struct.unpack_from("<III", d, 8)
    print("Kopf +0x08/+0x0C/+0x10 = EDT 0x%X VH 0x%X VB 0x%X" % (edt, vh, vb))
    print("EDT[0x0a] @0x%X: %s" % (edt + 0x0a * 4, d[edt + 0x0a * 4:edt + 0x0a * 4 + 4].hex(" ")))
    vag = d[0x066B8:0x066B8 + 8768]
    print("VAG @0x066B8 8768 B sha1 %s" % hashlib.sha1(vag).hexdigest())
    inc = open(KNALL_INC, encoding="latin-1").read()
    body = inc[inc.index("{") + 1:inc.rindex("}")]
    b = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", body))
    print("knall_bank.inc %d B; SE-Map Satz 0 %s, Satz 1 %s" % (len(b), b[0:4].hex(" "), b[4:8].hex(" ")))
    i = b.find(vag[:64])
    print("VAG 5 in der Bank bei %s, sha1 %s" % (hex(i) if i >= 0 else "-",
                                                  hashlib.sha1(b[i:i + 8768]).hexdigest() if i >= 0 else "-"))


def sca():
    d = open(R2080, "rb").read()
    s = struct.unpack_from("<I", d, 0x20)[0]
    cx, cz = struct.unpack_from("<hh", d, s)
    cnt = struct.unpack_from("<5I", d, s + 4)
    print("SCA @0x%X Decke (%d,%d) Gruppen %s" % (s, cx, cz, cnt))
    seen, off = set(), s + 24
    for g in range(5):
        for _ in range(cnt[g]):
            w, dd, x, z, typ, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, off)
            k = (w, dd, x, z, typ, u0, u1, fl)
            if k not in seen:
                seen.add(k)
                print("@0x%04X g%d typ %d u0 %02x u1 %02x floor %02x Band %d  x %6d..%6d  z %6d..%6d"
                      % (off, g, typ, u0, u1, fl, fl >> 4, x, x + w, z, z + dd))
            off += 12
    a = d[0x0BE2:0x0BE2 + 20]
    x, z, w, dd = struct.unpack_from("<hhHH", a, 6)
    print("Treppe @0x0BE2 %s: Slot %d sce %d Band 0x%02x Rechteck x %d..%d z %d..%d side %d Bandsprung %d"
          % (a.hex(" "), a[1], a[2], a[4], x, x + w, z, z + dd, struct.unpack_from("<H", a, 14)[0],
             struct.unpack_from("<H", a, 16)[0]))


def zlage(path, step):
    pl_re = re.compile(r"^F(\d+) .*?PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? cam=(\d+)")
    z_re = re.compile(r"\[(\d+) t=16 st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(\d+) "
                      r"d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\]")
    seg, last_f = 0, -1
    xmin, xmax, zmin, zmax, treppe, n = 10 ** 9, -10 ** 9, 10 ** 9, -10 ** 9, 0, 0
    for l in open(path, encoding="latin-1").read().splitlines():
        m = pl_re.match(l)
        if not m:
            continue
        f = int(m.group(1))
        if f < last_f:
            seg += 1
        last_f = f
        zs = z_re.findall(l)
        for q in zs:
            x, zz = int(q[10]), int(q[11])
            xmin, xmax, zmin, zmax = min(xmin, x), max(xmax, x), min(zmin, zz), max(zmax, zz)
            n += 1
            treppe += x < ABSATZ_WAND_X
        if zs and f % step == 0:
            print("seg%d F%d PL(%s,%s) rot %s hp %s cam %s | " % (seg, f, m.group(2), m.group(3), m.group(4), m.group(5),
                                                                 m.group(6))
                  + " ".join("%s:(%s,%s)" % (q[0], q[10], q[11]) for q in zs))
    print("Zombie-Saetze %d: x %d..%d z %d..%d, westlich der Absatzwand x < %d: %d"
          % (n, xmin, xmax, zmin, zmax, ABSATZ_WAND_X, treppe))


if __name__ == "__main__":
    was = sys.argv[1] if len(sys.argv) > 1 else "knall"
    if was == "knall":
        knall()
    elif was == "sca":
        sca()
    elif was == "zlage":
        zlage(sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 60)
    else:
        sys.exit("unbekannt: %s" % was)
