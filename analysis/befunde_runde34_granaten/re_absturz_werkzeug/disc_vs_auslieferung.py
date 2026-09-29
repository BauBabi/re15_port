#!/usr/bin/env python3
"""disc_vs_auslieferung.py - Runde 34 / re_absturz_original.md §2.1

Liest das ISO9660-Dateisystem eines MODE2/2352-Disc-Abbilds (MZD-Disc, die DuckStation
faehrt) direkt aus der .bin, extrahiert jede Datei (Form-1-Nutzdaten, 2048 Byte je Sektor)
und vergleicht sie Byte fuer Byte mit dem Auslieferungsstand `info/Re1.5/PSX/` (+ PSX.EXE).

Zweck: belegen, ob der Emulator den Auslieferungsstand misst oder einen Mod.
Ausgabe: je Datei  gleich / VERSCHIEDEN (mit erster Abweichung, Anzahl Abweichungsbytes)
         / nur-auf-Disc / nur-im-Baum.

Aufruf:  python disc_vs_auslieferung.py [bin] [baum] [--out datei]
"""
import os, sys, struct, hashlib

SECT = 2352
DATA_OFF = 24          # 12 sync + 4 header + 8 subheader (MODE2 Form 1)
USER = 2048

DEF_BIN = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.bin"
DEF_TREE = r"C:\workspace\git\reAi_v2\info\Re1.5"


class Disc:
    def __init__(self, path):
        self.f = open(path, "rb")

    def sector(self, lba):
        self.f.seek(lba * SECT)
        raw = self.f.read(SECT)
        return raw

    def user(self, lba):
        raw = self.sector(lba)
        return raw[DATA_OFF:DATA_OFF + USER]

    def is_form2(self, lba):
        raw = self.sector(lba)
        sub = raw[16:24]
        return bool(sub[2] & 0x20)

    def read_file(self, lba, size):
        out = bytearray()
        n = (size + USER - 1) // USER
        for i in range(n):
            out += self.user(lba + i)
        return bytes(out[:size])


def parse_dir(disc, lba, size, prefix, out):
    data = disc.read_file(lba, size)
    i = 0
    while i < len(data):
        ln = data[i]
        if ln == 0:
            # rest of sector is padding
            i = ((i // USER) + 1) * USER
            continue
        rec = data[i:i + ln]
        ext = struct.unpack_from("<I", rec, 2)[0]
        dsz = struct.unpack_from("<I", rec, 10)[0]
        flags = rec[25]
        nlen = rec[32]
        name = rec[33:33 + nlen]
        i += ln
        if name in (b"\x00", b"\x01"):
            continue
        nm = name.decode("ascii", "replace").split(";")[0]
        full = prefix + "/" + nm if prefix else nm
        if flags & 2:
            parse_dir(disc, ext, dsz, full, out)
        else:
            out.append((full, ext, dsz))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    outp = None
    if "--out" in sys.argv:
        outp = sys.argv[sys.argv.index("--out") + 1]
        args = [a for a in args if a != outp]
    binp = args[0] if len(args) > 0 else DEF_BIN
    tree = args[1] if len(args) > 1 else DEF_TREE
    disc = Disc(binp)
    pvd = disc.user(16)
    assert pvd[1:6] == b"CD001", "keine ISO9660-PVD in Sektor 16"
    root = pvd[156:156 + 34]
    rlba = struct.unpack_from("<I", root, 2)[0]
    rsz = struct.unpack_from("<I", root, 10)[0]
    files = []
    parse_dir(disc, rlba, rsz, "", files)
    lines = []
    lines.append("Disc: %s" % binp)
    lines.append("Baum: %s" % tree)
    lines.append("Dateien auf der Disc: %d" % len(files))
    n_same = n_diff = n_only = n_form2 = 0
    disc_names = set()
    for (name, lba, size) in sorted(files):
        disc_names.add(name.upper())
        # Baum-Pfad: PSX.EXE liegt in info/Re1.5/, der Rest unter info/Re1.5/PSX/
        cand = [os.path.join(tree, name), os.path.join(tree, "PSX", name)]
        local = None
        for c in cand:
            if os.path.isfile(c):
                local = c
                break
        if disc.is_form2(lba):
            n_form2 += 1
            tag = "FORM2(XA/STR, nicht verglichen)"
            if local:
                tag += " baum=%d B" % os.path.getsize(local)
            lines.append("%-34s lba=%7d size=%9d  %s" % (name, lba, size, tag))
            continue
        d = disc.read_file(lba, size)
        if local is None:
            n_only += 1
            lines.append("%-34s lba=%7d size=%9d  NUR-AUF-DISC md5=%s" % (
                name, lba, size, hashlib.md5(d).hexdigest()))
            continue
        l = open(local, "rb").read()
        if d == l:
            n_same += 1
            lines.append("%-34s lba=%7d size=%9d  gleich md5=%s" % (
                name, lba, size, hashlib.md5(d).hexdigest()))
        else:
            n_diff += 1
            m = min(len(d), len(l))
            first = next((k for k in range(m) if d[k] != l[k]), m)
            ndb = sum(1 for k in range(m) if d[k] != l[k]) + abs(len(d) - len(l))
            lines.append("%-34s lba=%7d size=%9d  VERSCHIEDEN baum=%d B erste_abw=0x%x abw_bytes=%d" % (
                name, lba, size, len(l), first, ndb))
    # Dateien nur im Baum
    for base, dirs, fns in os.walk(os.path.join(tree, "PSX")):
        for fn in fns:
            rel = os.path.relpath(os.path.join(base, fn), os.path.join(tree, "PSX")).replace("\\", "/")
            if rel.upper() not in disc_names and ("PSX/" + rel).upper() not in disc_names:
                lines.append("%-34s NUR-IM-BAUM" % rel)
    lines.append("SUMME: gleich=%d verschieden=%d nur_disc=%d form2=%d" % (n_same, n_diff, n_only, n_form2))
    txt = "\n".join(lines)
    print(txt)
    if outp:
        open(outp, "w", encoding="utf-8").write(txt + "\n")


if __name__ == "__main__":
    main()
