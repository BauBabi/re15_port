# -*- coding: utf-8 -*-
"""Gegenpruefung R3: Kopie einer Datei mit ANDEREM Inhalt, aber gleicher Groesse und gleicher CRC32
(ein Byte bei q gekippt, 4 Ausgleichsbytes ab p = q + 1). Eigene Umsetzung (nicht die des Gates):
CRC32 ist ueber GF(2) linear; die Wirkung einer Bitaenderung haengt nur vom Abstand zum Dateiende ab,
deshalb werden die Wirkungen auf dem Dateirest ab q berechnet (kein 363-MB-Nullpuffer je Bit).
Aufruf: r3_crc_gleich.py <quelle> <ziel> <q>      (q = Datei-Offset des gekippten Bytes)
"""
import hashlib
import sys
import zlib


def crc_datei(pfad):
    c, h, n = 0, hashlib.sha256(), 0
    with open(pfad, "rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            c = zlib.crc32(b, c)
            h.update(b)
            n += len(b)
    return c & 0xFFFFFFFF, h.hexdigest(), n


def main():
    quelle, ziel, q = sys.argv[1], sys.argv[2], int(sys.argv[3])
    crc0, sha0, n = crc_datei(quelle)
    rest = n - q                                  # Laenge des Dateirests ab q
    null = zlib.crc32(bytes(rest))

    def wirkung(pos, maske):                      # pos relativ zu q
        v = bytearray(rest)
        v[pos] = maske
        return zlib.crc32(bytes(v)) ^ null

    ziel_w = wirkung(0, 0x01)
    basis = []
    for k in range(32):
        w, a = wirkung(1 + k // 8, 1 << (k % 8)), 1 << k
        for bw, ba in basis:
            if w ^ bw < w:
                w, a = w ^ bw, a ^ ba
        if w:
            basis.append((w, a))
            basis.sort(reverse=True)
    w, a = ziel_w, 0
    for bw, ba in basis:
        if w ^ bw < w:
            w, a = w ^ bw, a ^ ba
    if w:
        raise SystemExit("kein Ausgleich gefunden")
    with open(quelle, "rb") as f:
        d = bytearray(f.read())
    vorher = bytes(d[q:q + 5])
    d[q] ^= 0x01
    for k in range(32):
        if a >> k & 1:
            d[q + 1 + k // 8] ^= 1 << (k % 8)
    with open(ziel, "wb") as f:
        f.write(d)
    crc1, sha1, n1 = crc_datei(ziel)
    print("Quelle: sha256 %s crc32 %08x %d B" % (sha0, crc0, n))
    print("Ziel:   sha256 %s crc32 %08x %d B" % (sha1, crc1, n1))
    print("Bytes ab Offset %d: vorher %s, nachher %s" % (q, vorher.hex(), bytes(d[q:q + 5]).hex()))
    if (crc1, n1) != (crc0, n) or sha1 == sha0:
        raise SystemExit("FEHLER: CRC/Groesse nicht gleich oder Inhalt gleich")
    print("OK: gleiche Groesse und CRC32, anderer Inhalt")
    return 0


if __name__ == "__main__":
    sys.exit(main())
