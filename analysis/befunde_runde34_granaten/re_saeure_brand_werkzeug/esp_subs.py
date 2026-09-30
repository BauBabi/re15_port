#!/usr/bin/env python3
"""Runde 34 / re_saeure_brand: ALLE Row-Subs (sub&7 = 0..7) eines ESP-Effekts auflisten,
je Stream die Zeilen mit Routine A/B (Tabelle @0x80071d40), damit sichtbar wird, ob es eine
Saeure-/Feuer-Explosion als eigene Row-Menge gibt.

Layout (selbst disassembliert, FUN_80019700 @0x80019700-0x800199b0, RE1.5 PSX.EXE; RE2
gegengeprueft im Dossier):
  a0 = (fx_id<<24)|(sub<<16)|scale      (@0x80019728 srl t8,a0,24 / @0x8001970c srl v0,a0,16)
  rowblk  = DAT_800b22d4[fx_id]          (@0x80019744-50)
  sub_off = u16 @ rowblk + (sub&7)*2     (@0x80019734-5c)
  base    = rowblk + sub_off*4           (@0x80019770-74)
  streams = u16 @ base                   (@0x80019778 lhu t4,0(t6))
  stream k: u16 nrows (+2), nrows*40 Byte Zeilen (Sprung nrows*40+4 @0x800198c0-e0)
  CLUT    = EFF-Kopf+4 + (sub>>3)*0x40   (@0x8001973c srl v1,t7,3 / @0x80019754 sll s0,v1,6 /
                                          @0x8001987c-88 lhu v0,4(t5); addu v0,v0,s0; sh 50(t0))
Datei-Lage der Row-Bloecke (Installer FUN_8001945c): rowblk = EFF-Ende =
  eff_start + (count_a*2 + count_b + 2)*4  (Port re15_esp.c esp_rowblk, gegen Runde-30-Messung
  Effekt 4 sub 0x0D -> Zeilen @0x1AB8 bestaetigt).

Aufruf: esp_subs.py <ESP-Datei> [effekt-id ...]   (Default: CORE00.ESP, alle Effekte)
"""
import os, sys, struct
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))


def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def parse_global(raw):
    ids = []
    for i in range(64):
        if raw[i] == 0xFF:
            break
        ids.append(raw[i])
    ptr_end = ((len(raw) + 3) & ~3) - 4
    effs = []
    for i, eid in enumerate(ids):
        ent = struct.unpack_from("<i", raw, ptr_end - 4 * i)[0]
        st = ent
        w0 = u32(raw, st)
        ca, cb = w0 & 0xFFFF, w0 >> 16
        end = st + (ca * 2 + cb + 2) * 4
        effs.append(dict(id=eid, start=st, end=end, ca=ca, cb=cb,
                         w1=u32(raw, st + 4)))
    return effs


def dump(raw, eff, next_start):
    print("Effekt %d: EFF @0x%04X..0x%04X count_a=%d count_b=%d  Kopf+4 (clut/tpage-Wort)=0x%08X"
          % (eff['id'], eff['start'], eff['end'], eff['ca'], eff['cb'], eff['w1']))
    rb = eff['end']
    subtab = [u16(raw, rb + 2 * k) for k in range(8)]
    print("  Sub-Tabelle @0x%04X: %s" % (rb, " ".join("%d" % s for s in subtab)))
    seen = {}
    for k in range(8):
        so = subtab[k]
        base = rb + so * 4
        if so == 0:
            print("  sub&7=%d: Offset 0 -> zeigt auf die Sub-Tabelle selbst = KEIN Row-Block" % k)
            continue
        if base + 4 > len(raw) or (next_start and base >= next_start):
            print("  sub&7=%d: Offset %d -> 0x%04X ausserhalb" % (k, so, base))
            continue
        if base in seen:
            print("  sub&7=%d: = sub&7=%d (gleicher Block @0x%04X)" % (k, seen[base], base))
            continue
        seen[base] = k
        ns = u16(raw, base)
        print("  sub&7=%d: Block @0x%04X, %d Stream(s)" % (k, base, ns))
        p = base + 4
        for s in range(ns):
            nr = u16(raw, p)
            print("    Stream %d @0x%04X: %d Zeile(n)" % (s, p, nr))
            for r in range(nr):
                z = p + 4 + r * 40
                h = [s16(raw, z + 2 * j) for j in range(20)]
                print("      Z%02d @0x%04X A=%-2d B=%-2d wh=%d,%d acc=(%d,%d,%d) p0e=%d vel=(%d,%d,%d) "
                      "p16=%d ang=(%d,%d,%d) p1e=%d eul=(%d,%d,%d) gate=%d"
                      % (r, z, h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7], h[8], h[9], h[10],
                         h[11], h[12], h[13], h[14], h[15], h[16], h[17], h[18], h[19]))
            p += 4 + nr * 40


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
        REPO, "re15_port", "shared_assets", "PSX", "DATA", "CORE00.ESP")
    want = [int(x, 0) for x in sys.argv[2:]]
    raw = open(path, "rb").read()
    effs = parse_global(raw)
    print("; %s (%d B), Ids %s" % (path, len(raw), [e['id'] for e in effs]))
    starts = sorted(e['start'] for e in effs)
    for e in effs:
        if want and e['id'] not in want:
            continue
        nxt = [s for s in starts if s > e['start']]
        dump(raw, e, nxt[0] if nxt else None)


if __name__ == "__main__":
    main()
