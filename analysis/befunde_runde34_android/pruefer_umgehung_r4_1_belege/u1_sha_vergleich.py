# -*- coding: utf-8 -*-
"""Pruefer UMGEHUNG R4-1, H6f: Ausgaben von u1_sha_zusatz.exe gegen Python hashlib.
Aufruf: u1_sha_vergleich.py <exe> <seed> <n> [--strom <bytes> <muster>]... [--datei <pfad>]..."""
import hashlib
import subprocess
import sys


def fuellen(n, seed):
    x = seed if seed else 0x9e3779b97f4a7c15
    m = (1 << 64) - 1
    out = bytearray(n)
    for i in range(n):
        x ^= (x << 13) & m
        x ^= x >> 7
        x ^= (x << 17) & m
        out[i] = (x >> 24) & 0xff
    return bytes(out)


def main():
    exe, seed, n = sys.argv[1], sys.argv[2], sys.argv[3]
    rest = sys.argv[4:]
    falsch = 0
    aus = subprocess.run([exe, "zufall", seed, n], stdout=subprocess.PIPE, check=True).stdout.decode().split("\n")
    zeilen = [z for z in aus if z.strip()]
    laengen = []
    for z in zeilen:
        l, s, h = z.split()
        l, s = int(l), int(s)
        soll = hashlib.sha256(fuellen(l, s)).hexdigest()
        laengen.append(l)
        if soll != h:
            falsch += 1
            print("FALSCH zufall laenge %d seed %d: C %s, Python %s" % (l, s, h, soll))
    print("zufall: %d Nachrichten (Laengen %d..%d, %d davon < 300 B, %d >= 64 KiB), %d falsch"
          % (len(zeilen), min(laengen), max(laengen), sum(1 for l in laengen if l < 300),
             sum(1 for l in laengen if l >= 65536), falsch))
    i = 0
    while i < len(rest):
        if rest[i] == "--strom":
            gesamt, muster = int(rest[i + 1]), int(rest[i + 2])
            i += 3
            stueck = 1 << 20
            buf = bytes(((j % muster) * 131 + 7) & 255 for j in range(stueck))
            h = hashlib.sha256()
            o = 0
            while o < gesamt:
                k = min(stueck, gesamt - o)
                h.update(buf[:k])
                o += k
            c = subprocess.run([exe, "strom", str(gesamt), str(muster)], stdout=subprocess.PIPE,
                               check=True).stdout.decode().split()
            ok = c[1] == h.hexdigest()
            falsch += 0 if ok else 1
            print("strom %d B (%.2f GiB): C %s.. Python %s.. %s" % (gesamt, gesamt / 2 ** 30, c[1][:16], h.hexdigest()[:16],
                                                                    "gleich" if ok else "FALSCH"))
        elif rest[i] == "--datei":
            pfad = rest[i + 1]
            i += 2
            h = hashlib.sha256()
            g = 0
            with open(pfad, "rb") as f:
                while True:
                    b = f.read(1 << 20)
                    if not b:
                        break
                    h.update(b)
                    g += len(b)
            c = subprocess.run([exe, "datei", pfad], stdout=subprocess.PIPE, check=True).stdout.decode().split()
            ok = c[0] == "0" and int(c[1]) == g and c[2] == h.hexdigest()
            falsch += 0 if ok else 1
            print("datei %s: C rc %s, %s B, %s.. / Python %d B, %s.. %s" % (pfad, c[0], c[1], c[2][:16], g,
                                                                          h.hexdigest()[:16], "gleich" if ok else "FALSCH"))
        else:
            sys.exit("unbekannt: " + rest[i])
    print("ERGEBNIS: %d falsch" % falsch)
    return 1 if falsch else 0


if __name__ == "__main__":
    sys.exit(main())
