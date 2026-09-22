#!/usr/bin/env python3
"""codepanel_schranke.py - druckt die Erfolgs-Schranke der sechs Code-Panels als ROHE Datei-Bytes.

Zweck: die Bedingung, die der Nutzer verlangt ("erst wenn der Zugriffscode einmal richtig
eingegeben wurde"), ist EIN Flag-Bit. Dieses Werkzeug zeigt, wo das Bit gesetzt wird -
Datei-Byte-Offset und Rohbytes, ohne Zwischenschicht.

Aufruf: python codepanel_schranke.py [ASSET_ROOT]
"""
import glob, os, sys

MUS = bytes([0x21,0x05,0x0d,0x01, 0x21,0x05,0x0e,0x01,
             0x21,0x05,0x0f,0x01, 0x21,0x05,0x10,0x01])
RAEUME = ("ROOM10D0","ROOM10D1","ROOM11E0","ROOM11E1","ROOM1230","ROOM1231")


def hexs(d, a, n):
    return " ".join("%02x" % b for b in d[a:a+n])


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "..", "..", "..", "re15_port", "shared_assets", "PSX")
    print("=== Erfolgs-Schranke der sechs Code-Panels, rohe Datei-Bytes ===")
    for rn in RAEUME:
        tr = glob.glob(os.path.join(root, "STAGE*", rn + ".RDT"))
        if not tr:
            print("== %s: keine RDT gefunden" % rn)
            continue
        p = tr[0]
        d = open(p, "rb").read()
        off = d.find(MUS)
        if off < 0:
            print("== %s: Muster NICHT gefunden" % rn)
            continue
        print("== %s  %d B   Muster @0x%05X   (%s)"
              % (rn, len(d), off, os.path.relpath(p, root)))
        blk = d[off-6] | (d[off-5] << 8)
        print("   0x%05X  %s   Ifel_ck  (Block %d B)" % (off-8, hexs(d, off-8, 4), blk))
        print("   0x%05X  %s   Ck(%d,%d,%d)   = Schloss noch NICHT offen"
              % (off-4, hexs(d, off-4, 4), d[off-3], d[off-2], d[off-1]))
        for k in range(4):
            o = off + 4*k
            print("   0x%05X  %s   Ck(%d,%d,%d)   = Ziffer %d richtig"
                  % (o, hexs(d, o, 4), d[o+1], d[o+2], d[o+3], k+1))
        print("   0x%05X  %s   Evt_exec sub=%d   = Erfolgs-Ereignis (\"You've opened the lock.\")"
              % (off+16, hexs(d, off+16, 4), d[off+19]))
        print("   0x%05X  %s   Set(%d,%d,%d)   = ERFOLGS-BIT  <<< die gesuchte Bedingung"
              % (off+20, hexs(d, off+20, 4), d[off+21], d[off+22], d[off+23]))


if __name__ == "__main__":
    main()
