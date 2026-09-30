#!/usr/bin/env python3
"""Runde 34 / Granate: CORE00.ESP (globale Effektbank) - Kopf, Anim-Saetze, Koordinatensaetze
und Zeilenstroeme eines Effekts ausgeben. Reines Lesewerkzeug.

Layout (selbst gegen PSX.EXE belegt, siehe Dossier re_wurf_flug_explosion.md §3/§4):
  Id-Kopf @0 (u8-Liste, 0xFF-Ende), Zeigertabelle ABWAERTS ab round_up4(size)-4
  (FUN_8001923c; Port re15_esp_parse_global), Effektkoerper = Eintrag (relativ zu 0).
  Koerper: u32 w0 = count_a | count_b<<16, u16 CLUT @+4, u16 TPAGE @+6
           (FUN_80019700 @0x8001987c `lhu v0,4(t5)` -> +0x32, @0x8001988c `lhu v1,6(t5)` -> +0x30),
           count_a Anim-Saetze a 8 B ab +8 (Basis +0x78 @0x80019890-94),
           count_b Koordinatensaetze a 4 B danach (Basis +0x7c @0x800198a8-b0),
           Zeilenblock danach (rowblk = Koerper + (ca*2+cb+2)*4; Loader @0x800194c0-e0).
  Anim-Satz (Tick @0x8001a3cc-0x8001a47c, Draw FUN_800534c4/FUN_80053240):
     [0] Koord-Startindex bzw. Schleifenziel bei [2]==0xFF   [1] Anzahl Teilsprites
     [2] Dauer (Bilder); 0xFF = Schleife; [0]==0&&[2]==0 = Ende   [3] Zellen-Kantenlaenge (Texel)
  Koordinatensatz (FUN_800534c4): [0] u, [1] v, [2] s8 dx, [3] s8 dy (Bildschirmversatz)
  Zeilenblock: 8 x u16 Sub-Offsets (Einheit 4 B); base: u16 Stroeme (+pad); je Strom u16 nrows (+pad),
  nrows x 40-B-Zeilen.

Aufruf: python esp_effekt.py <CORE00.ESP> <effekt-id> [sub ...] [--anim a-b]
"""
import struct, sys

def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]

def parse(b):
    size = len(b)
    ptr_end = ((size + 3) & ~3) - 4
    effs = []
    for i in range(16):
        eid = b[i]
        if eid == 0xFF:
            break
        ent = struct.unpack_from("<i", b, ptr_end - 4 * i)[0]
        start = ent
        w0 = u32(b, start)
        ca, cb = w0 & 0xffff, w0 >> 16
        end = start + (ca * 2 + cb + 2) * 4
        effs.append(dict(id=eid, start=start, ca=ca, cb=cb, rowblk=end,
                         clut=u16(b, start + 4), tpage=u16(b, start + 6)))
    return effs

ROWF = [("A", 0), ("B", 2), ("w", 4), ("h", 6), ("ax", 8), ("ay", 10), ("az", 12), ("p0e", 14),
        ("vx", 16), ("vy", 18), ("vz", 20), ("p16", 22), ("wx", 24), ("wy", 26), ("wz", 28),
        ("p1e", 30), ("ex", 32), ("ey", 34), ("ez", 36), ("p26", 38)]

def rows(b, e, sub):
    rb = e["rowblk"]
    off = u16(b, rb + (sub & 7) * 2)
    base = rb + off * 4
    ns = u16(b, base)
    out = []
    p = base + 4
    for s in range(ns):
        nr = u16(b, p)
        rr = []
        for r in range(nr):
            z = p + 4 + 40 * r
            rr.append((z, {k: s16(b, z + o) for k, o in ROWF}))
        out.append((p, nr, rr))
        p += 4 + nr * 40
    return base, ns, out

def main():
    path = sys.argv[1]
    eid = int(sys.argv[2], 0)
    args = sys.argv[3:]
    arng = None
    if "--anim" in args:
        k = args.index("--anim"); a, z = args[k + 1].split("-"); arng = (int(a, 0), int(z, 0))
        args = args[:k] + args[k + 2:]
    subs = [int(x, 0) for x in args]
    b = open(path, "rb").read()
    effs = parse(b)
    print("%s: %d B, Ids %s" % (path, len(b), [hex(e["id"]) for e in effs]))
    e = [x for x in effs if x["id"] == eid][0]
    print("Effekt 0x%02X: Koerper @0x%X ca=%d cb=%d CLUT=0x%04X TPAGE=0x%04X rowblk @0x%X"
          % (eid, e["start"], e["ca"], e["cb"], e["clut"], e["tpage"], e["rowblk"]))
    rng = range(e["ca"]) if arng is None else range(arng[0], min(arng[1] + 1, e["ca"]))
    for i in rng:
        o = e["start"] + 8 + 8 * i
        r = b[o:o + 8]
        cells = []
        if r[2] != 0xFF and not (r[0] == 0 and r[2] == 0):
            for c in range(r[1]):
                co = e["start"] + 8 + 8 * e["ca"] + 4 * (r[0] + c)
                cc = b[co:co + 4]
                cells.append("(u%d v%d dx%d dy%d)" % (cc[0], cc[1], struct.unpack("b", cc[2:3])[0],
                                                       struct.unpack("b", cc[3:4])[0]))
        kind = "SCHLEIFE->%d" % r[0] if r[2] == 0xFF else ("ENDE" if (r[0] == 0 and r[2] == 0) else "")
        print("  Anim %2d @0x%05X: %s  koord=%d n=%d dauer=%d kante=%d %s %s"
              % (i, o, r.hex(" "), r[0], r[1], r[2], r[3], kind, " ".join(cells)))
    for sub in subs:
        base, ns, out = rows(b, e, sub)
        print("sub 0x%02X (Tabelle sub&7=%d, CLUT-Zusatz (sub>>3)*0x40=0x%X): base @0x%X, Stroeme %d"
              % (sub, sub & 7, (sub >> 3) * 0x40, base, ns))
        for si, (p, nr, rr) in enumerate(out):
            print("  Strom %d @0x%X: %d Zeilen" % (si, p, nr))
            for z, f in rr:
                print("    Zeile @0x%05X: " % z + " ".join("%s=%d" % (k, f[k]) for k, _ in ROWF))

if __name__ == "__main__":
    main()
