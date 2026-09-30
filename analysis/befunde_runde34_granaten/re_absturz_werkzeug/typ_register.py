#!/usr/bin/env python3
"""typ_register.py - Runde 34 / re_absturz_original.md §1.5

Statischer Zensus: welche Gegner-Wurzel traegt jedes Stage-Overlay in die Typ-Tabelle
0x80072bac ein (Aufrufer FUN_8001a50c: `8001a570 lbu v0,8(v1)` / `8001a57c addu v0,v0,s3` /
`8001a580 lw v0,0(v0)` / `8001a588 jalr v0`, s3 = 0x80072bac @0x8001a544/48)?

Muster im Overlay (roh @0x80100000): `lui rX,hi` / `addiu rX,rX,lo` (Handler) ...
`lui at,0x8007` / `sw rX,imm(at)` mit 0x80070000 + (s16)imm in [0x80072bac, +0x60*4).
Danach fuer jede Wurzel: die +0x4-Zustandstabelle (Muster `lbu v0,4(..)` ... `lui at,HI` /
`addiu at,at,LO` / `addu at,at,v0` / `lw v0,0(at)` / `jalr v0`), deren Eintraege [2] (HURT) und
[3] (DEATH), und darin jeder Tabellen-Dispatch auf +0x5 / +0x6:
   2D:  `lbu v1,5(aX)` `lbu v0,6(aX)` `sll v1,v1,5` ... -> Basis + 9*32 + 1*4
   1D5: `lbu v0,5(..)` `sll v0,v0,2` `lui at` `addiu at` -> Basis + 9*4
   1D6: `lbu v0,6(..)` ... -> Basis + 1*4
Ausgabe: Zelle, die der Granatentreffer (+0x5 = 9, +0x6 = 1) waehlt, und ihr Inhalt.
Aufruf: python typ_register.py [STAGE1.BIN ...]
"""
import struct, sys, os

BIN = r"C:\workspace\git\reAi_v2\info\Re1.5\PSX\BIN"
BASE = 0x80100000
TAB = 0x80072bac


def s16(x):
    return x - 0x10000 if x & 0x8000 else x


class Ov:
    def __init__(self, path):
        self.d = open(path, "rb").read()
        self.name = os.path.basename(path)

    def w(self, a):
        o = a - BASE
        if o < 0 or o + 4 > len(self.d):
            return None
        return struct.unpack_from("<I", self.d, o)[0]


def dec(w):
    return dict(op=w >> 26, rs=(w >> 21) & 31, rt=(w >> 16) & 31, rd=(w >> 11) & 31, imm=w & 0xffff,
                fn=w & 63, sa=(w >> 6) & 31)


def register(ov):
    """Typ -> Wurzel aus den `sw rX,imm(at)`-Eintraegen."""
    out = {}
    n = len(ov.d) // 4
    for i in range(n):
        a = BASE + 4 * i
        w = ov.w(a)
        x = dec(w)
        if x["op"] != 0x2b or x["rs"] != 1:  # sw rt,imm(at)
            continue
        # lui at,0x8007 davor (bis 3 Instruktionen)
        at = None
        for k in range(1, 4):
            p = dec(ov.w(a - 4 * k))
            if p["op"] == 0x0f and p["rt"] == 1:
                at = p["imm"] << 16
                break
        if at is None:
            continue
        adr = (at + s16(x["imm"])) & 0xffffffff
        if not (TAB <= adr < TAB + 0x60 * 4):
            continue
        typ = (adr - TAB) // 4
        # rX: lui/addiu davor (bis 8 Instruktionen)
        rt = x["rt"]
        hi = lo = None
        for k in range(1, 80):   # bis 80 Instruktionen zurueck: ein Register kann mehrfach gespeichert werden
            p = dec(ov.w(a - 4 * k))
            if p["op"] == 0x09 and p["rt"] == rt and p["rs"] == rt and lo is None:
                lo = s16(p["imm"])
                continue
            if p["op"] == 0x0f and p["rt"] == rt:
                hi = p["imm"] << 16
                break
            # anderer Schreiber auf rt (ausser sw/sb/sh) -> abbrechen
            if p["op"] in (0x08, 0x09, 0x0c, 0x0d, 0x23, 0x24, 0x25, 0x21, 0x20) and p["rt"] == rt:
                break
            if p["op"] == 0 and p["rd"] == rt and p["fn"] not in (8, 9):
                break
        if hi is None:
            continue
        out[typ] = ((hi + (lo or 0)) & 0xffffffff, a)
    return out


def find_state_table(ov, root, span=200):
    """+0x4-Dispatch in der Wurzel: lbu v?,4(..) ... lui at,HI / addiu at,at,LO ... jalr"""
    seen4 = False
    hi = None
    for i in range(span):
        a = root + 4 * i
        w = ov.w(a)
        if w is None:
            return None
        x = dec(w)
        if x["op"] == 0x24 and x["imm"] == 4:  # lbu rt,4(rs)
            seen4 = True
        if seen4 and x["op"] == 0x0f and x["rt"] == 1:
            hi = x["imm"] << 16
        if seen4 and hi is not None and x["op"] == 0x09 and x["rt"] == 1 and x["rs"] == 1:
            return (hi + s16(x["imm"])) & 0xffffffff, a
    return None


def dispatch_in(ov, h, span=120):
    """Tabellen-Dispatch auf +0x5/+0x6 im Handler h (erste Fundstelle)."""
    ins = []
    for i in range(span):
        w = ov.w(h + 4 * i)
        if w is None:
            break
        ins.append((h + 4 * i, dec(w)))
    res = []
    for j, (a, x) in enumerate(ins):
        if x["op"] == 0 and x["fn"] == 9:  # jalr
            win = ins[max(0, j - 14):j]
            f5 = any(y["op"] == 0x24 and y["imm"] == 5 for _, y in win)
            f6 = any(y["op"] == 0x24 and y["imm"] == 6 for _, y in win)
            sll5 = any(y["op"] == 0 and y["fn"] == 0 and y["sa"] == 5 for _, y in win)
            base = None
            his = [y for _, y in win if y["op"] == 0x0f and y["rt"] in (1, 4)]
            los = [y for _, y in win if y["op"] == 0x09 and y["rt"] in (1, 4) and y["rs"] in (1, 4)]
            if his and los:
                base = ((his[-1]["imm"] << 16) + s16(los[-1]["imm"])) & 0xffffffff
            if base is None:
                continue
            if f5 and f6 and sll5:
                res.append(("2D[+5][+6]", base, base + 9 * 32 + 1 * 4, a))
            elif f5:
                res.append(("1D[+5]", base, base + 9 * 4, a))
            elif f6:
                res.append(("1D[+6]", base, base + 1 * 4, a))
        if x["op"] == 0 and x["fn"] == 8 and x["rs"] == 31:  # jr ra -> Ende
            break
    return res


def main():
    files = sys.argv[1:] or ["STAGE%d.BIN" % i for i in range(1, 7)]
    for f in files:
        ov = Ov(os.path.join(BIN, f))
        reg = register(ov)
        print("===== %s: %d Typ-Eintraege" % (f, len(reg)))
        roots = {}
        for typ, (root, at) in sorted(reg.items()):
            roots.setdefault(root, []).append((typ, at))
        for root, typs in sorted(roots.items()):
            tl = ",".join("%02x" % t for t, _ in typs)
            st = find_state_table(ov, root) if BASE <= root < BASE + len(ov.d) else None
            if st is None:
                print("  Typ %s -> %08x  (keine +0x4-Tabelle gefunden / ausserhalb)" % (tl, root))
                continue
            tb, ta = st
            ent = [ov.w(tb + 4 * k) for k in range(5)]
            print("  Typ %s -> Wurzel %08x, +0x4-Tabelle %08x (@%08x): [2]=%08x [3]=%08x" % (
                tl, root, tb, ta, ent[2] or 0, ent[3] or 0))
            for k, nm in ((2, "HURT"), (3, "DEATH")):
                h = ent[k]
                if h is None or not (BASE <= h < BASE + len(ov.d)):
                    print("     %s %08x: EXE/ausserhalb" % (nm, h or 0))
                    continue
                ds = dispatch_in(ov, h)
                if not ds:
                    print("     %s %08x: kein +5/+6-Tabellensprung (erste 120 Instr.)" % (nm, h))
                for kind, base, cell, ja in ds:
                    v = ov.w(cell)
                    vs = "%08x" % v if v is not None else "ausserhalb"
                    flag = "  <== NULL" if v == 0 else ""
                    print("     %s %08x: %s Basis %08x, jalr @%08x -> Zelle %08x = %s%s" % (
                        nm, h, kind, base, ja, cell, vs, flag))


if __name__ == "__main__":
    main()
