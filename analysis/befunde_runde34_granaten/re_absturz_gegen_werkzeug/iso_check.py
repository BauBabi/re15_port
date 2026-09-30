#!/usr/bin/env python3
"""iso_check.py - eigener ISO9660-Leser (MODE2/2352, Form 1: Nutzdaten ab Byte 24, 2048 B/Sektor)
fuer die Gegenpruefung. Listet ALLE Dateien der Disc rekursiv und vergleicht jede mit
info/Re1.5/<pfad> (PSX.EXE / SYSTEM.CNF liegen dort direkt, der Rest unter info/Re1.5/PSX/...).
Ausgabe: gleich / verschieden / fehlt-im-Baum.  Aufruf: iso_check.py <disc.bin>
"""
import sys, os, struct, hashlib

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
TREE = os.path.join(REPO, "info", "Re1.5")


class Iso:
    def __init__(self, path):
        self.f = open(path, "rb")

    def sec(self, lba):
        self.f.seek(lba * 2352)
        raw = self.f.read(2352)
        return raw[24:24 + 2048]

    def read(self, lba, size):
        out = bytearray()
        n = (size + 2047) // 2048
        for k in range(n):
            out += self.sec(lba + k)
        return bytes(out[:size])

    def walk(self, lba, size, prefix=""):
        data = self.read(lba, size)
        i = 0
        while i < len(data):
            ln = data[i]
            if ln == 0:
                i = ((i // 2048) + 1) * 2048
                continue
            rec = data[i:i + ln]
            elba = struct.unpack_from("<I", rec, 2)[0]
            esize = struct.unpack_from("<I", rec, 10)[0]
            flags = rec[25]
            nl = rec[32]
            name = rec[33:33 + nl]
            i += ln
            if name in (b"\x00", b"\x01"):
                continue
            nm = name.decode("latin1").split(";")[0]
            if flags & 2:
                yield from self.walk(elba, esize, prefix + nm + "/")
            else:
                yield prefix + nm, elba, esize


def tree_path(p):
    c1 = os.path.join(TREE, p.replace("/", os.sep))
    if os.path.exists(c1):
        return c1
    return None


def main():
    iso = Iso(sys.argv[1])
    pvd = iso.sec(16)
    assert pvd[1:6] == b"CD001", "kein PVD"
    root = pvd[156:156 + 34]
    rl = struct.unpack_from("<I", root, 2)[0]
    rs = struct.unpack_from("<I", root, 10)[0]
    same = diff = miss = 0
    lines = []
    for p, lba, size in iso.walk(rl, rs):
        d = iso.read(lba, size)
        tp = tree_path(p)
        if tp is None:
            miss += 1
            lines.append("FEHLT  %-40s %9d md5=%s" % (p, size, hashlib.md5(d).hexdigest()))
            continue
        t = open(tp, "rb").read()
        if t == d:
            same += 1
            if p.split("/")[-1] in ("PSX.EXE", "SYSTEM.CNF", "STAGE1.BIN", "STAGE2.BIN", "STAGE3.BIN", "STAGE4.BIN",
                                    "STAGE5.BIN", "DEBUG.BIN", "CORE00.ESP", "PL00W09.PLW"):
                lines.append("GLEICH %-40s %9d md5=%s" % (p, size, hashlib.md5(d).hexdigest()))
        else:
            diff += 1
            lines.append("ANDERS %-40s disc %9d baum %9d" % (p, size, len(t)))
    print("\n".join(lines))
    print("Summe: gleich %d, anders %d, fehlt im Baum %d" % (same, diff, miss))


if __name__ == "__main__":
    main()
