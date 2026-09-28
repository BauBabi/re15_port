#!/usr/bin/env python3
"""r30_hundtod_flagscan.py - sucht in RE2 PSX.EXE (und optional Overlays) alle Zugriffe auf
ein absolutes Wort (Default 0x800CFB74) und zeigt, welche Maske im Umfeld benutzt wird.
Aufruf: python r30_hundtod_flagscan.py <adresse-hex> <maske-hex> [datei] [ladeadresse-hex]
Ausgabe: je Treffer Adresse der lw/lhu/lbu/sw-Instruktion + die Masken-Instruktion.
Nur Lesen; schreibt nichts."""
import struct, sys, os
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
def main():
    target = int(sys.argv[1], 16)
    mask = int(sys.argv[2], 16)
    path = sys.argv[3] if len(sys.argv) > 3 else os.path.join(REPO, "info", "re2leon", "PSX.EXE")
    data = open(path, "rb").read()
    if len(sys.argv) > 4:
        base = int(sys.argv[4], 16); off0 = 0
    else:
        base = struct.unpack_from("<I", data, 0x18)[0]; off0 = 0x800
    n = (len(data) - off0) // 4
    words = struct.unpack_from("<%dI" % n, data, off0)
    hi = (target + 0x8000) >> 16
    lo = target & 0xffff
    hits = 0
    for i, w in enumerate(words):
        op = w >> 26
        imm = w & 0xffff
        # direkte Zugriffe: lw/lhu/lbu/lh/lb/sw/sh/sb/addiu mit imm == lo
        if op in (0x23, 0x25, 0x24, 0x21, 0x20, 0x2b, 0x29, 0x28, 0x09) and imm == lo:
            rs = (w >> 21) & 31
            # rueckwaerts nach lui rs,hi suchen (bis 12 Instruktionen)
            ok = False
            for j in range(i - 1, max(i - 14, -1), -1):
                wj = words[j]
                if (wj >> 26) == 0x0f and ((wj >> 16) & 31) == rs and (wj & 0xffff) == hi:
                    ok = True; break
            if not ok: continue
            # Umfeld nach der Maske absuchen
            found = []
            for j in range(max(i - 6, 0), min(i + 14, n)):
                wj = words[j]; oj = wj >> 26; ij = wj & 0xffff
                if mask <= 0xffff:
                    if oj in (0x0c, 0x0d) and ij == mask:
                        found.append((j, "andi/ori 0x%x" % ij))
                    if oj == 0x0c and ij == ((~mask) & 0xffff):
                        found.append((j, "andi ~0x%x" % mask))
                else:
                    if oj == 0x0f and ij == (mask >> 16):
                        found.append((j, "lui 0x%x" % ij))
                    if oj == 0x0f and ij == ((~mask >> 16) & 0xffff):
                        found.append((j, "lui ~0x%x(=0x%x)" % (mask >> 16, ij)))
            if found:
                hits += 1
                kind = {0x23:"lw",0x25:"lhu",0x24:"lbu",0x21:"lh",0x20:"lb",0x2b:"sw",0x29:"sh",0x28:"sb",0x09:"addiu"}[op]
                print("0x%08x %-5s  Maske: %s" % (base + i * 4, kind,
                      ", ".join("0x%08x %s" % (base + j * 4, t) for j, t in found)))
    print("# Treffer: %d  (Datei %s, Basis 0x%08x)" % (hits, os.path.basename(path), base))
main()
