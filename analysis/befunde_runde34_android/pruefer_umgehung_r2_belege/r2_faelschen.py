# -*- coding: utf-8 -*-
"""Gegenpruefung R2 (Linse UMGEHUNG) - Faelschungen an Kopien der Referenz-APK.

Eigener ROHER ZIP-Parser/Schreiber (nicht zipfile, nicht der Leser des Gates): liest jeden
Zentralverzeichnis-Eintrag samt Local Header und Rohdaten und schreibt die APK NEU (ohne APK
Signing Block -> unsigniert; signieren danach mit apksigner). Eingriffe:

  --ohne NAME               Eintrag weglassen
  --daten NAME=DATEI        Daten ersetzen (Stored, CRC/Groessen neu)
  --manifest-weg PFAD       Manifestzeile PFAD entfernen, Kopfzeile (Anzahl/Bytes) nachziehen
  --manifest-groesse PFAD=N Groesse der Manifestzeile PFAD auf N, Kopfzeile (Bytes) nachziehen
  --manifest-kopf-arabisch  Ziffern der Kopfzeile als arabisch-indische Ziffern (U+0660..)
  --usize-plus NAME=N       unkomprimierte Groesse in Local Header UND Zentralverzeichnis +N
                            (Daten und CRC bleiben)
  --kommentar-plus N        EOCD-Kommentarlaenge +N (ohne Bytes anzuhaengen)
  --praefix N               N Bytes vor den ersten Local Header, alle Offsets verschoben

Aufruf: r2_faelschen.py <quelle.apk> <ziel.apk> [Eingriffe ...]
        r2_faelschen.py --zeige <apk> NAME...   (Felder aus CD + LFH)
Nur mit echtem Python starten (/c/Python310/python), nie mit dem WindowsApps-Alias.
"""
import argparse
import struct
import sys
import zlib

EOCD, CDS, LFHS = b"PK\x05\x06", b"PK\x01\x02", b"PK\x03\x04"
MAN = b"assets/re15_assets.txt"


def lesen(pfad):
    d = open(pfad, "rb").read()
    i = d.rfind(EOCD)
    (_s, _dk, _cdk, _nh, n, cds, cdo, kl) = struct.unpack("<4sHHHHIIH", d[i:i + 22])
    ein, p = [], cdo
    for _ in range(n):
        assert d[p:p + 4] == CDS, "CD-Signatur"
        nl, xl, kl2 = struct.unpack("<HHH", d[p + 28:p + 34])
        cd = bytearray(d[p:p + 46 + nl + xl + kl2])
        lho, = struct.unpack("<I", cd[42:46])
        assert d[lho:lho + 4] == LFHS, "LFH-Signatur"
        lnl, lxl = struct.unpack("<HH", d[lho + 26:lho + 30])
        lfh = bytearray(d[lho:lho + 30 + lnl + lxl])
        cs, = struct.unpack("<I", cd[20:24])
        daten = d[lho + 30 + lnl + lxl:lho + 30 + lnl + lxl + cs]
        ein.append(dict(name=bytes(cd[46:46 + nl]), cd=cd, lfh=lfh, daten=daten, lho=lho))
        p += 46 + nl + xl + kl2
    return ein, d[i:i + 22 + kl]


def setze(e, crc=None, cs=None, us=None, meth=None):
    for buf, o in ((e["lfh"], 14), (e["cd"], 16)):
        if crc is not None:
            struct.pack_into("<I", buf, o, crc)
        if cs is not None:
            struct.pack_into("<I", buf, o + 4, cs)
        if us is not None:
            struct.pack_into("<I", buf, o + 8, us)
    if meth is not None:
        struct.pack_into("<H", e["lfh"], 8, meth)
        struct.pack_into("<H", e["cd"], 10, meth)


def daten_setzen(e, roh):
    e["daten"] = roh
    fl, = struct.unpack("<H", e["lfh"][6:8])
    struct.pack_into("<H", e["lfh"], 6, fl & ~0x0008)
    fl, = struct.unpack("<H", e["cd"][8:10])
    struct.pack_into("<H", e["cd"], 8, fl & ~0x0008)
    setze(e, crc=zlib.crc32(roh) & 0xFFFFFFFF, cs=len(roh), us=len(roh), meth=0)


def manifest(e):
    m = e["cd"][10:12]
    assert struct.unpack("<H", m)[0] == 0, "Manifest nicht Stored"
    return e["daten"].decode("utf-8")


def kopf_neu(zeilen):
    n, b = 0, 0
    for z in zeilen[1:]:
        if z and not z.startswith("#") and "\t" in z:
            n += 1
            b += int(z.split("\t", 1)[0])
    return "# re15 assets %d %d" % (n, b)


def schreiben(ziel, ein, eocd, praefix=0, kommentar_plus=0):
    aus = bytearray(b"\x00" * praefix)
    for e in sorted(ein, key=lambda x: x["lho"]):
        off = len(aus)
        struct.pack_into("<I", e["cd"], 42, off)
        aus += e["lfh"] + e["daten"]
    cd_off = len(aus)
    cd = b"".join(bytes(e["cd"]) for e in sorted(ein, key=lambda x: x["lho"]))
    aus += cd
    en = bytearray(eocd)
    struct.pack_into("<HHHHII", en, 4, 0, 0, len(ein), len(ein), len(cd), cd_off)
    kl, = struct.unpack("<H", en[20:22])
    struct.pack_into("<H", en, 20, kl + kommentar_plus)
    aus += en
    open(ziel, "wb").write(bytes(aus))


def main():
    if len(sys.argv) > 2 and sys.argv[1] == "--zeige":
        ein, _ = lesen(sys.argv[2])
        for nm in sys.argv[3:]:
            for e in ein:
                if e["name"] == nm.encode():
                    f = struct.unpack("<HHHHIII", e["cd"][8:28])      # flags meth zeit datum crc cs us
                    lf = struct.unpack("<HHHHIII", e["lfh"][6:26])
                    print(nm, "CD flags/meth=%d/%d crc=%08x cs=%d us=%d" % (f[0], f[1], f[4], f[5], f[6]),
                          "| LFH flags/meth=%d/%d crc=%08x cs=%d us=%d" % (lf[0], lf[1], lf[4], lf[5], lf[6]),
                          "| lho=%d" % e["lho"])
        return 0
    ap = argparse.ArgumentParser()
    ap.add_argument("quelle")
    ap.add_argument("ziel")
    ap.add_argument("--ohne", action="append", default=[])
    ap.add_argument("--daten", action="append", default=[])
    ap.add_argument("--manifest-weg", action="append", default=[])
    ap.add_argument("--manifest-groesse", action="append", default=[])
    ap.add_argument("--manifest-kopf-arabisch", action="store_true")
    ap.add_argument("--usize-plus", action="append", default=[])
    ap.add_argument("--cd-dd", action="append", default=[])
    ap.add_argument("--lfh-crc-kippen", action="append", default=[])
    ap.add_argument("--daten-kippen", action="append", default=[])
    ap.add_argument("--kommentar-plus", type=int, default=0)
    ap.add_argument("--praefix", type=int, default=0)
    a = ap.parse_args()
    ein, eocd = lesen(a.quelle)
    ohne = set(x.encode() for x in a.ohne)
    ein = [e for e in ein if e["name"] not in ohne]
    idx = {e["name"]: e for e in ein}
    for s in a.daten:
        nm, datei = s.split("=", 1)
        daten_setzen(idx[nm.encode()], open(datei, "rb").read())
    if a.manifest_weg or a.manifest_groesse or a.manifest_kopf_arabisch:
        em = idx[MAN]
        zeilen = manifest(em).split("\n")
        for p in a.manifest_weg:
            vorher = len(zeilen)
            zeilen = [z for z in zeilen if not z.endswith("\t" + p)]
            assert len(zeilen) == vorher - 1, "Zeile %s nicht genau einmal" % p
        for s in a.manifest_groesse:
            p, n = s.rsplit("=", 1)
            t = [k for k, z in enumerate(zeilen) if z.endswith("\t" + p)]
            assert len(t) == 1
            zeilen[t[0]] = "%s\t%s" % (n, p)
        zeilen[0] = kopf_neu(zeilen)
        if a.manifest_kopf_arabisch:          # nur Anzahl und Bytes, "re15" bleibt ASCII
            kopf, zahlen = zeilen[0].split(" assets ", 1)
            zeilen[0] = kopf + " assets " + "".join(chr(0x0660 + int(c)) if c.isdigit() else c for c in zahlen)
        daten_setzen(em, "\n".join(zeilen).encode("utf-8"))
        print("Kopfzeile neu: %s" % ascii(zeilen[0]))
    for s in a.usize_plus:
        nm, n = s.split("=", 1)
        e = idx[nm.encode()]
        us, = struct.unpack("<I", e["cd"][24:28])
        setze(e, us=us + int(n))
    for nm in a.cd_dd:                     # Data-Descriptor-Bit NUR im Zentralverzeichnis setzen
        e = idx[nm.encode()]
        fl, = struct.unpack("<H", e["cd"][8:10])
        struct.pack_into("<H", e["cd"], 8, fl | 0x0008)
    for nm in a.lfh_crc_kippen:            # CRC NUR im Local Header ^1
        e = idx[nm.encode()]
        c, = struct.unpack("<I", e["lfh"][14:18])
        struct.pack_into("<I", e["lfh"], 14, c ^ 1)
    for s in a.daten_kippen:               # ein Byte der ROHdaten ^0x01 (CRC im Header bleibt alt)
        nm, off = s.split("=", 1)
        e = idx[nm.encode()]
        d = bytearray(e["daten"])
        d[int(off)] ^= 0x01
        e["daten"] = bytes(d)
    schreiben(a.ziel, ein, eocd, a.praefix, a.kommentar_plus)
    print("geschrieben: %s (%d Eintraege)" % (a.ziel, len(ein)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
