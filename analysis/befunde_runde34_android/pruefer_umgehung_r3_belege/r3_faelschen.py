# -*- coding: utf-8 -*-
"""Gegenpruefung R3 (Linse UMGEHUNG) - Faelschungen an Kopien der Referenz-APK.

Eigener ROHER ZIP-Leser/-Schreiber (weder zipfile noch der Leser des Gates; Aufbau wie
pruefer_umgehung_r2_belege/r2_faelschen.py, hier neu geschrieben): liest jeden Eintrag des
Zentralverzeichnisses samt Local Header und Rohdaten und schreibt die APK NEU (ohne APK Signing
Block -> unsigniert; signieren danach mit zipalign + apksigner, siehe signieren.sh).

Eingriffe (mehrfach moeglich):
  --manifest-sub ALT=NEU   Rohtext im Manifest assets/re15_assets.txt ersetzen (genau EIN Treffer;
                           Python-Escapes erlaubt: \\t \\r \\x00 \\uFF15 ...). Kopfzeile bleibt.
  --manifest-anhang TEXT   Rohtext ans Manifest-Ende haengen (Escapes wie oben)
  --daten NAME=DATEI       Daten eines Eintrags ersetzen (Stored, CRC/Groessen neu)
  --ohne NAME              Eintrag weglassen
  --zusatz NAME=DATEI      neuen Stored-Eintrag anhaengen
Aufruf: r3_faelschen.py <quelle.apk> <ziel.apk> [Eingriffe ...]
        r3_faelschen.py --zeige <apk> NAME...
Nur mit echtem Python starten (/c/Python310/python), nie mit dem WindowsApps-Alias.
"""
import argparse
import codecs
import struct
import sys
import zlib

EOCD, CDS, LFHS = b"PK\x05\x06", b"PK\x01\x02", b"PK\x03\x04"
MAN = b"assets/re15_assets.txt"


def lesen(pfad):
    d = open(pfad, "rb").read()
    i = d.rfind(EOCD)
    (_s, _dk, _cdk, _nh, n, _cds, cdo, kl) = struct.unpack("<4sHHHHIIH", d[i:i + 22])
    ein, p = [], cdo
    for _ in range(n):
        if d[p:p + 4] != CDS:
            raise SystemExit("CD-Signatur fehlt bei %d" % p)
        nl, xl, kl2 = struct.unpack("<HHH", d[p + 28:p + 34])
        cd = bytearray(d[p:p + 46 + nl + xl + kl2])
        lho, = struct.unpack("<I", cd[42:46])
        if d[lho:lho + 4] != LFHS:
            raise SystemExit("LFH-Signatur fehlt bei %d" % lho)
        lnl, lxl = struct.unpack("<HH", d[lho + 26:lho + 30])
        lfh = bytearray(d[lho:lho + 30 + lnl + lxl])
        cs, = struct.unpack("<I", cd[20:24])
        daten = d[lho + 30 + lnl + lxl:lho + 30 + lnl + lxl + cs]
        ein.append(dict(name=bytes(cd[46:46 + nl]), cd=cd, lfh=lfh, daten=daten, lho=lho))
        p += 46 + nl + xl + kl2
    return ein, d[i:i + 22 + kl]


def daten_setzen(e, roh):
    e["daten"] = roh
    crc = zlib.crc32(roh) & 0xFFFFFFFF
    for buf, o, fo, mo in ((e["lfh"], 14, 6, 8), (e["cd"], 16, 8, 10)):
        fl, = struct.unpack("<H", buf[fo:fo + 2])
        struct.pack_into("<H", buf, fo, fl & ~0x0008)
        struct.pack_into("<H", buf, mo, 0)
        struct.pack_into("<III", buf, o, crc, len(roh), len(roh))


def schreiben(ziel, ein, eocd):
    aus = bytearray()
    for e in sorted(ein, key=lambda x: x["lho"]):
        struct.pack_into("<I", e["cd"], 42, len(aus))
        aus += e["lfh"] + e["daten"]
    cd_off = len(aus)
    cd = b"".join(bytes(e["cd"]) for e in sorted(ein, key=lambda x: x["lho"]))
    aus += cd
    en = bytearray(eocd)
    struct.pack_into("<HHHHII", en, 4, 0, 0, len(ein), len(ein), len(cd), cd_off)
    aus += en
    open(ziel, "wb").write(bytes(aus))


def neuer_eintrag(vorlage, name, roh, lho):
    lfh = bytearray(vorlage["lfh"][:30])
    struct.pack_into("<HH", lfh, 26, len(name), 0)
    lfh += name
    cd = bytearray(vorlage["cd"][:46])
    struct.pack_into("<HHH", cd, 28, len(name), 0, 0)
    cd += name
    e = dict(name=name, cd=cd, lfh=lfh, daten=b"", lho=lho)
    daten_setzen(e, roh)
    return e


def text_esc(s):
    """'\\t', '\\x00', '\\uFF15' -> echte Zeichen; Ergebnis als UTF-8-Bytes."""
    return codecs.decode(s, "unicode_escape").encode("utf-8")


def main():
    if len(sys.argv) > 2 and sys.argv[1] == "--zeige":
        ein, _ = lesen(sys.argv[2])
        for nm in sys.argv[3:]:
            for e in ein:
                if e["name"] == nm.encode():
                    f = struct.unpack("<HHHHIII", e["cd"][8:28])
                    print(nm, "CD flags/meth=%d/%d crc=%08x cs=%d us=%d lho=%d" % (f[0], f[1], f[4], f[5], f[6], e["lho"]))
        return 0
    ap = argparse.ArgumentParser()
    ap.add_argument("quelle")
    ap.add_argument("ziel")
    ap.add_argument("--manifest-sub", action="append", default=[])
    ap.add_argument("--manifest-anhang", action="append", default=[])
    ap.add_argument("--daten", action="append", default=[])
    ap.add_argument("--ohne", action="append", default=[])
    ap.add_argument("--zusatz", action="append", default=[])
    a = ap.parse_args()
    ein, eocd = lesen(a.quelle)
    ohne = set(x.encode() for x in a.ohne)
    ein = [e for e in ein if e["name"] not in ohne]
    idx = {e["name"]: e for e in ein}
    for s in a.daten:
        nm, datei = s.split("=", 1)
        daten_setzen(idx[nm.encode()], open(datei, "rb").read())
    if a.manifest_sub or a.manifest_anhang:
        em = idx[MAN]
        if struct.unpack("<H", em["cd"][10:12])[0] != 0:
            raise SystemExit("Manifest nicht Stored")
        m = bytes(em["daten"])
        for s in a.manifest_sub:
            alt, neu = s.split("=", 1)
            alt_b, neu_b = text_esc(alt), text_esc(neu)
            if m.count(alt_b) != 1:
                raise SystemExit("--manifest-sub: %r kommt %d-mal vor" % (alt_b, m.count(alt_b)))
            m = m.replace(alt_b, neu_b)
        for s in a.manifest_anhang:
            m += text_esc(s)
        daten_setzen(em, m)
        print("Manifest neu: %d B" % len(m))
    vorlage = idx[MAN]
    for s in a.zusatz:
        nm, datei = s.split("=", 1)
        ein.append(neuer_eintrag(vorlage, nm.encode(), open(datei, "rb").read(), 10 ** 12 + len(ein)))
    schreiben(a.ziel, ein, eocd)
    print("geschrieben: %s (%d Eintraege)" % (a.ziel, len(ein)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
