#!/usr/bin/env python3
"""r30_title_vram_rows.py - Runde 30 / Thema D.

Misst im ORIGINAL (DuckStation-Savestate-VRAM), wie der Pulswert auf das BILD wirkt:
mittlere Helligkeit der drei Menuezeilen (x=0x20..0x11f, y=Zeile..Zeile+16) in BEIDEN
Bildpuffern (0,0) und (0,240), dazu Pulswert/-zaehler aus dem RAM.
Optional: schreibt den Bildpuffer als PNG (zweites Argument = Ausgabeverzeichnis).
Zeilen-Y aus dem Zeichner-Aufruf: 0x85 @0x80102bb4, 0x99 @0x80102bcc, 0xad @0x80102be8.
"""
import sys, os
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss
ROOT = r"C:\workspace\git\reAi_v2\stage_saves"
ROWS = [("NEW GAME", 0x85), ("LOAD GAME", 0x99), ("OPTION", 0xad)]

def px(r, x, y):
    v = r.vpix(x, y)
    return ((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3)

def row_stats(r, oy, y0):
    s = 0; n = 0; mx = 0; bright = 0
    for y in range(y0, y0 + 17):
        for x in range(0x20, 0x120):
            R, G, B = px(r, x, oy + y)
            l = (R * 299 + G * 587 + B * 114) // 1000
            s += l; n += 1; mx = max(mx, l)
            if l >= 128: bright += 1
    return s / n, mx, bright

def main():
    outdir = None
    args = sys.argv[1:]
    if args and os.path.isdir(args[-1]): outdir = args.pop()
    names = args or ["boot_40.sav", "boot_44.sav", "boot_48.sav", "boot_52.sav",
                     "nav_down1.sav", "nav_loadgame.sav", "mzd_title.sav"]
    print("%-20s %-4s %-5s %-4s %-4s | %s" % ("savestate", "ctr", "val", "cur", "buf",
          "  ".join("%-9s mean/max/n>=128" % n for n, _ in ROWS)))
    for n in names:
        r = re15_ss.Ram(os.path.join(ROOT, n))
        ctr = r.u16(0x80102946); val = r.u16(0x80102944); cur = r.u8(0x801026ca)
        for oy in (0, 240):
            st = [row_stats(r, oy, y0) for _, y0 in ROWS]
            print("%-20s %-4d 0x%02x  %-4d y=%-3d| %s" % (n, ctr, val, cur, oy,
                  "  ".join("%6.2f/%3d/%5d        " % s for s in st)))
        if outdir:
            for oy in (0, 240):
                rgb = bytearray()
                for y in range(240):
                    for x in range(320):
                        rgb += bytes(px(r, x, oy + y))
                re15_ss.write_png(os.path.join(outdir, "orig_%s_buf%d.png" % (n[:-4], oy)), 320, 240, bytes(rgb))

if __name__ == "__main__":
    main()
