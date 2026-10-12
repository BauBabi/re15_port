#!/usr/bin/env python3
"""zensus_objekte_ausschiebung.py - Runde 36 G2: welche Pool-Objekte KANN der Ruhe-Zweig des Objekt-Passes ueberhaupt aus
SCA-Zellen schieben, und welche davon bewegt das SCD (Cursor-/Panel-/Hebe-Muster)?

Regeln (alle aus info/Re1.5/PSX.EXE, selbst disassembliert):
  obj+0       Installer LAB_80040914: `lhu v0,6(a2) / ori v0,v0,0x1 / sll/sra / sw v0,0(a1)` @0x80040990-9a4
              (nur wenn der Platz frei war, @0x80040978-98c; sonst `sw zero,0(a1)` = Platz geloescht)
  obj+0 & 8   FUN_8003b558 @0x8003b668-674 `lw v0,0(s1) / andi v0,v0,0x8 / bne` -> JEDE Zelle uebersprungen
              (also keine Ausschiebung, egal wo das Objekt steht)
  Band        @0x8003b634-644 (s16)(Zelle+10) >> 12 == obj+0x82 (= pc[4], `sb v0,130(a1)` @0x80040974)
  Klasse      @0x8003b64c-660 (s8)u0 & 2 != 0 (Ruhe-Zweig a1 = 2 @0x8002c0b8)
  Rechteck    @0x8003b738-794 halboffen, r = hx (bzw. hz im Nicht-Quadrat-Zweig @0x8003b684-734)
  Box         obj+0x78 = pc + 22 (`addiu v0,a2,22 / sw v0,120(a1)` @0x800409f4-9f8): cx,cy,cz,hx,hy,hz (s16/u16)
Bewegte Objekte: Work_set(3,k) (`2e 03 kk`, Handler @0x80040d2c: kind 3 -> Pool 0x800b3f98 + k*0x94 @0x80040dd4-dec)
gefolgt von Add_speed (0x30, @0x80040f40: obj+0x34/+0x38/+0x3C += Tempo) oder Pos_set (0x32) im selben Skript.
Aufruf: python zensus_objekte_ausschiebung.py <PSX-Wurzel (shared_assets/PSX)>"""
import os, re, struct, subprocess, sys

HIER = os.path.dirname(os.path.abspath(__file__))
DUMP = os.path.join(HIER, "..", "..", "..", "re15_port", "tools", "scd_dump_room.py")


def sca_zellen(d):
    sca = struct.unpack_from("<I", d, 0x20)[0]
    if sca == 0 or sca + 24 > len(d):
        return None, []
    cx, cz = struct.unpack_from("<hh", d, sca)
    cnt = struct.unpack_from("<5I", d, sca + 4)
    if sum(cnt) > 4000:
        return None, []
    off = sca + 24
    reg = []
    for q in range(5):
        cells = []
        for i in range(cnt[q]):
            w, dd, x, z, t, u0, u1, fl = struct.unpack_from("<HHhhBBBB", d, off)
            cells.append((x, z, w, dd, u0, u1, fl, off))
            off += 12
        reg.append(cells)
    return (cx, cz), reg


def quadrant(px, pz, ceil):
    v0 = ((pz - ceil[1]) & 0xFFFFFFFF) & 0x80000000
    v1 = (((px - ceil[0]) & 0xFFFFFFFF) & 0x80000000) >> 1
    return (v0 | v1) >> 30


def treffer(ceil, reg, x, z, band, box):
    cx, cy, cz, hx, hy, hz = box
    q = quadrant(x + cx, z + cz, ceil)
    if q >= len(reg):
        return []
    out = []
    for i, (x0, z0, w, dd, u0, u1, fl, off) in enumerate(reg[q]):
        cband = struct.unpack("<h", bytes([u1, fl]))[0] >> 12
        if cband != band:
            continue
        if (struct.unpack("<b", bytes([u0]))[0] & 2) == 0:
            continue
        r = hx
        if hx != hz:
            cxw, czw = cx + x, cz + z
            xc = (((cxw - (x0 + w)) & 0x80000000) | ((((cxw - x0) & 0xFFFFFFFF) & 0x80000000) >> 1)) >> 30
            zc = (((czw - (z0 + dd)) & 0x80000000) | ((((czw - z0) & 0xFFFFFFFF) & 0x80000000) >> 1)) >> 30
            if xc == 2 and zc in (0, 3):
                r = hz
        if ((cx + x) - (x0 - r)) & 0xFFFFFFFF < (w + 2 * r) and ((cz + z) - (z0 - r)) & 0xFFFFFFFF < (dd + 2 * r):
            out.append("r%d[%d] @0x%05X x %d..%d z %d..%d" % (q, i, off, x0, x0 + w, z0, z0 + dd))
    return out


def main():
    wurzel = sys.argv[1]
    zeilen_obj = re.compile(r"^\s+0x([0-9A-F]+) \(\+[0-9A-F]+\) Obj_model_set\s+((?:[0-9a-f]{2} ){34})")
    zeilen_op = re.compile(r"^\s+0x([0-9A-F]+) \(\+[0-9A-F]+\) (\w+)\s+((?:[0-9a-f]{2} ?)+)")
    n_raeume = n_obj = n_frei = 0
    for st in sorted(os.listdir(wurzel)):
        if not st.startswith("STAGE"):
            continue
        for fn in sorted(os.listdir(os.path.join(wurzel, st))):
            if not fn.upper().endswith(".RDT"):
                continue
            p = os.path.join(wurzel, st, fn)
            d = open(p, "rb").read()
            if len(d) < 0x60:
                print("%s/%s: %d Bytes - kein RDT, uebersprungen" % (st, fn, len(d)))
                continue
            txt = subprocess.run([sys.executable, DUMP, p], capture_output=True, text=True).stdout
            n_raeume += 1
            ceil, reg = sca_zellen(d)
            objs = []
            beweger = {}
            work = None
            for l in txt.splitlines():
                if l.startswith("==="):
                    work = None
                m = zeilen_obj.match(l)
                if m:
                    b = bytes.fromhex(m.group(2).replace(" ", ""))
                    flags = (b[6] | (b[7] << 8)) | 1
                    pos = struct.unpack_from("<hhh", b, 10)
                    box = struct.unpack_from("<hhhHHH", b, 22)
                    objs.append((m.group(1), b[1], b[2], b[4], flags, pos, box))
                    continue
                m = zeilen_op.match(l)
                if not m:
                    continue
                name, bb = m.group(2), bytes.fromhex(m.group(3).replace(" ", ""))
                if name == "Work_set":
                    work = bb[2] if len(bb) >= 3 and bb[1] == 3 else None
                elif name in ("Add_speed", "Pos_set") and work is not None:
                    beweger.setdefault(work, set()).add("%s@0x%s" % (name, m.group(1)))
            for (off, oid, typ, band, flags, pos, box) in objs:
                n_obj += 1
                if flags & 8:
                    continue
                n_frei += 1
                t = treffer(ceil, reg, pos[0], pos[2], band, box) if ceil else []
                bew = sorted(beweger.get(oid, []))
                print("%s/%s @0x%s obj %d typ %d band %d +0=%04x lage (%d,%d,%d) box c(%d,%d,%d) h(%d,%d,%d) | Start-Treffer: %s | SCD bewegt: %s" % (
                    st, fn, off, oid, typ, band, flags, pos[0], pos[1], pos[2], box[0], box[1], box[2], box[3], box[4], box[5],
                    ", ".join(t) if t else "-", ", ".join(bew[:4]) + (" ..." if len(bew) > 4 else "") if bew else "-"))
    print("Summe: %d Raeume, %d Obj_model_set, davon %d OHNE Bit 3 (@0x8003b668-674 -> ausschiebbar)" % (n_raeume, n_obj, n_frei))


if __name__ == "__main__":
    main()
