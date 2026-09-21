#!/usr/bin/env python3
"""anchor_scan.py - Gegenprobe zum Walk: JEDE Byteposition pruefen.

Der opcode-exakte Walk (re2_scd_walk.py) laesst 45 von 2553 SCD-Bloecken
desynchron stehen; was hinter einem Desync liegt, sieht er nicht. Diese
Gegenprobe ignoriert die Opcode-Struktur voellig und prueft an JEDER Byteposition
aller RDT-Rohdateien, ob dort ein plausibler Item-AOT mit der gesuchten Id steht.
Sie kann also nichts UEBERSEHEN; sie kann nur Fehltreffer liefern, die dann
einzeln von Hand entschieden werden.

Plausibilitaet (aus den Handlern FUN_80054CD4 / FUN_80055008):
  0x4E: +14 i_item == gesuchte Id, +16 n_item in 1..250, +20 md1 < 0x20 oder 0xFF
  0x69: +22 i_item == gesuchte Id, +24 n_item in 1..250, +28 md1 < 0x20 oder 0xFF
Zusaetzlich muss +3 sat in {0, 49} liegen (die beiden im Walk beobachteten Werte)
bzw. wird der Wert mit ausgegeben, damit nichts stillschweigend wegfaellt.
"""
import struct, sys, os, glob

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

LAY = {0x4E: (22, 14, 16, 18, 20, 21), 0x69: (30, 22, 24, 26, 28, 29)}


def hits(buf, want):
    out = []
    for p in range(len(buf)):
        op = buf[p]
        if op not in LAY:
            continue
        n, oi, on, of, om, oa = LAY[op]
        if p + n > len(buf):
            continue
        iid = struct.unpack_from("<H", buf, p + oi)[0]
        if iid not in want:
            continue
        cnt = struct.unpack_from("<H", buf, p + on)[0]
        flg = struct.unpack_from("<H", buf, p + of)[0]
        md1, act = buf[p + om], buf[p + oa]
        ok = (1 <= cnt <= 250) and (md1 < 0x20 or md1 == 0xFF)
        out.append(dict(off=p, op=op, sat=buf[p + 3], aot=buf[p + 1],
                        sce=buf[p + 2], id=iid, n=cnt, flag=flg,
                        md1=md1, act=act, plausibel=ok,
                        raw=bytes(buf[p:p + n])))
    return out


if __name__ == "__main__":
    want = set(int(a, 0) for a in sys.argv[1:]) or {76, 77}
    sys.path.insert(0, HERE)
    import re2_items
    pats = [os.path.join(RDT, "*.RDT"), os.path.join(RDT, "room*", "scd", "*.scd")]
    files = []
    for p in pats:
        files += sorted(glob.glob(p))
    print("durchsuchte Dateien: %d (%d RDT-Container + %d SCD-Bloecke)" % (
        len(files), len(glob.glob(pats[0])), len(glob.glob(pats[1]))))
    tot = 0
    for f in files:
        buf = open(f, "rb").read()
        for h in hits(buf, want):
            tot += 1
            nm = re2_items.name(h["id"])[1]
            print("%-58s +0x%06X op=0x%02X aot=%-3d sce=%-3d sat=%-3d id=%3d %-12s "
                  "n=%-3d flag=%-4d md1=%-3d act=%-3d %s  %s" % (
                      os.path.relpath(f, REPO), h["off"], h["op"], h["aot"], h["sce"],
                      h["sat"], h["id"], nm, h["n"], h["flag"], h["md1"], h["act"],
                      "plausibel" if h["plausibel"] else "UNPLAUSIBEL",
                      " ".join("%02x" % b for b in h["raw"])))
    print("--- %d Treffer fuer Ids %s ---" % (tot, sorted(want)))
