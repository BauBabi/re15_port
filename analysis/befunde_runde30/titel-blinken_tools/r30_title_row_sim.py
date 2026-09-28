#!/usr/bin/env python3
"""r30_title_row_sim.py - Runde 30 / Thema D.

PRUEFT die aus dem Zeichner FUN_801027a0 gelesene Wirkung des Pulswerts GEGEN DAS BILD
des Originals: die aktive Menuezeile wird aus den Savestate-Daten selbst nachgerechnet
(Textur + CLUT aus dem VRAM des Savestates, Hintergrund = DATA/TITLEU.TIM) und
pixelgenau mit dem Bildpuffer desselben Savestates verglichen.

Modell (jede Zahl aus dem Zeichner, TITLE.BIN):
  zwei Durchgaenge je Zeile (@0x801027f4 s4=0, Schleife @0x80102880-84):
    s4=0: Texpage 0xe10000b5 (@0x80102800-04 | 0x20 @0x80102824) = ABR 1 (B+F), Lage y
    s4=1: Texpage 0xe10000d5 (| 0x40 @0x8010281c)               = ABR 2 (B-F), Lage y+1
          (@0x80102810-14: +0x10000 auf das xy-Wort)
  Befehl 0x66808080 (@0x80102830-38) = texturiertes Rechteck, halbtransparent, MODULIERT
  aktive Zeile: die drei Farbbytes des Befehls = Pulswert (@0x80102844-58),
                CLUT 0x7fc0 (Deskriptor +0, @0x8010284c)
  inaktive:     Farbe 0x80 0x80 0x80, CLUT 0x7fcc (Deskriptor +4, @0x80102840)
  Reihenfolge im Bild: AddPrim haengt VORN ein -> der zuletzt eingehaengte Durchgang
  (s4=1, subtraktiv) wird ZUERST gezeichnet.
  Modulation (psx-spx, GPU "Modulation"): kanal = texel * farbe / 128, Saettigung 31.
"""
import sys, os, struct
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
import re15_ss
ROOT = r"C:\workspace\git\reAi_v2\stage_saves"
TITLEU = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\DATA\TITLEU.TIM"
ROWY = [0x85, 0x99, 0xad]

def load_bg():
    d = open(TITLEU, "rb").read()
    blen, x, y, w, h = struct.unpack_from("<IHHHH", d, 8)
    px = struct.unpack_from("<%dH" % (w * h), d, 20)
    return [px[i * w:(i + 1) * w] for i in range(h)]

def split(c): return (c & 31, (c >> 5) & 31, (c >> 10) & 31)

def sim_row(r, bg, row, active, val, mode):
    """liefert dict[(x,y)] = (r5,g5,b5) fuer die Zeilenregion (y0..y0+16)"""
    y0 = ROWY[row]; v0 = 16 * (row + 1)          # Deskriptor 0x801028ac + 16*idx: v = idx*16, idx = row+1
    clut_x = 0 if active else 192                # 0x7fc0 -> x=0 / 0x7fcc -> x=12*16=192, y=511
    out = {}
    for y in range(y0, y0 + 17):
        for x in range(0x20, 0x120):
            out[(x, y)] = split(bg[y][x])
    def texel(u, v):
        hw = r.vpix(320 + (u >> 1), 256 + v)
        idx = (hw >> 8) if (u & 1) else (hw & 0xff)
        return r.vpix(clut_x + idx, 511)
    for s4 in (1, 0):                            # subtraktiv (y+1) zuerst, dann additiv (y)
        for ty in range(16):
            y = y0 + ty + (1 if s4 else 0)
            for tx in range(256):
                c = texel(tx, v0 + ty)
                if c == 0: continue              # Farbwert 0 = durchsichtig
                m = val if active else 0x80
                f = [min(31, (ch * m) >> 7) for ch in split(c)]
                x = 0x20 + tx
                b = out[(x, y)]
                stp = c & 0x8000
                if mode == "stp" and not stp:
                    out[(x, y)] = tuple(f)
                elif s4:
                    out[(x, y)] = tuple(max(0, b[i] - f[i]) for i in range(3))
                else:
                    out[(x, y)] = tuple(min(31, b[i] + f[i]) for i in range(3))
    return out

def compare(r, sim, oy):
    bad = 0; n = 0; maxd = 0
    for (x, y), c in sim.items():
        got = split(r.vpix(x, oy + y)); n += 1
        d = max(abs(got[i] - c[i]) for i in range(3))
        if d: bad += 1; maxd = max(maxd, d)
    return n, bad, maxd

def main():
    bg = load_bg()
    names = sys.argv[1:] or ["boot_40.sav", "boot_44.sav", "boot_48.sav", "boot_52.sav", "mzd_title.sav"]
    for n in names:
        r = re15_ss.Ram(os.path.join(ROOT, n))
        val = r.u16(0x80102944); ctr = r.u16(0x80102946); cur = r.u8(0x801026ca)
        # STP-Bit der benutzten CLUT-Eintraege
        print("%s: ctr=%d val=0x%02x cursor=%d" % (n, ctr, val, cur))
        for row in range(3):
            active = (row == cur)
            for oy in (0, 240):
                best = None
                cands = [val] if not active else sorted(set([val, val - 2, val + 2, val - 4, val + 4, 0x80]))
                for v in cands:
                    for mode in ("blend", "stp"):
                        s = sim_row(r, bg, row, active, v, mode)
                        nn, bad, maxd = compare(r, s, oy)
                        if best is None or bad < best[0]: best = (bad, maxd, v, mode, nn)
                print("   Zeile %d (%s) Puffer y=%-3d : beste Deckung val=0x%02x Modus=%s -> %d von %d Pixeln weichen ab (max %d Stufen)" % (
                    row, "AKTIV" if active else "inaktiv", oy, best[2], best[3], best[0], best[4], best[1]))

if __name__ == "__main__":
    main()
