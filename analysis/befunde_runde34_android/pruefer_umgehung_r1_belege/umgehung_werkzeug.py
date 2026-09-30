# -*- coding: utf-8 -*-
"""Gegenpruefung Runde 34a (Linse UMGEHUNG) - Faelschungen, die der Bauer nicht getestet hat.

Arbeitet auf ROHEN ZIP-Bytes (eigener Parser), NICHT ueber zipfile - sonst wuerde das Werkzeug
dieselben Normalisierungen erben (NUL-Abschnitt, '\\' -> '/'), die es aufdecken soll.

Aufruf (nur echtes Python, z.B. /c/Python310/python.exe):
  umgehung_werkzeug.py inspect <apk>
  umgehung_werkzeug.py forge <quelle.apk> <ziel.apk> <art> [<eintrag>] [<arg>]
     Arten (Eintrag = voller Name, z.B. assets/shared_assets/RE15DOOR/P07G.DO2):
       backslash <e>      Name in LFH+CD: '/' -> '\\' (gleiche Laenge, in place)
       case <e> <neu>     Name in LFH+CD umbenennen (gleiche Laenge, in place)
       lfh_crc <e>        CRC-Feld NUR im Local File Header kippen (CD bleibt richtig)
       lfh_usize <e>      unkomprimierte Groesse NUR im LFH +1 (CD bleibt richtig)
       crc_kollision <e>  Inhalt aendern (1 Nutzbyte + 4 Ausgleichsbytes am Ende), CRC32 bleibt
                          GLEICH, Groesse bleibt gleich; LFH/CD unveraendert
       nul <e>            Name in LFH+CD := <e> + b"\\0abc" (neu geschrieben, Offsets verschoben)
       abschneiden <n>    nur die ersten (Groesse - n) Bytes kopieren
       cd_sig <e>         Signatur des CD-Eintrags zerstoeren (in place)
  umgehung_werkzeug.py crc <apk> <e>      CRC32 aus CD, LFH und Daten ausgeben
"""
import os
import shutil
import struct
import sys
import zlib

EOCD_SIG = b"PK\x05\x06"
CD_SIG = b"PK\x01\x02"
LFH_SIG = b"PK\x03\x04"


def eocd_lesen(f, groesse):
    lesen = min(groesse, 22 + 65535)
    f.seek(groesse - lesen)
    d = f.read(lesen)
    i = d.rfind(EOCD_SIG)
    if i < 0:
        raise SystemExit("kein EOCD")
    (sig, disk, cdisk, n_here, n_total, cd_size, cd_off, clen) = struct.unpack("<4sHHHHIIH", d[i:i + 22])
    return groesse - lesen + i, n_total, cd_size, cd_off


def cd_lesen(pfad):
    groesse = os.path.getsize(pfad)
    with open(pfad, "rb") as f:
        eocd_pos, n, cd_size, cd_off = eocd_lesen(f, groesse)
        f.seek(cd_off)
        cd = f.read(cd_size)
        eintraege = []
        p = 0
        for _ in range(n):
            if cd[p:p + 4] != CD_SIG:
                raise SystemExit("CD-Signatur fehlt bei %d" % p)
            (vmade, vneed, flags, meth, mt, md, crc, csize, usize, nlen, xlen, clen, disk, iattr, eattr,
             lho) = struct.unpack("<HHHHHHIIIHHHHHII", cd[p + 4:p + 46])
            name = cd[p + 46:p + 46 + nlen]
            e = dict(cd_pos=cd_off + p, flags=flags, meth=meth, crc=crc, csize=csize, usize=usize,
                     nlen=nlen, xlen=xlen, clen=clen, lho=lho, name=name,
                     cd_raw=cd[p:p + 46 + nlen + xlen + clen])
            f.seek(lho)
            lfh = f.read(30)
            (lsig, lver, lflags, lmeth, lmt, lmd, lcrc, lcsize, lusize, lnlen, lxlen) = struct.unpack(
                "<4sHHHHHIIIHH", lfh)
            e.update(lfh_flags=lflags, lfh_crc=lcrc, lfh_csize=lcsize, lfh_usize=lusize, lfh_nlen=lnlen,
                     lfh_xlen=lxlen, daten=lho + 30 + lnlen + lxlen)
            e["lfh_name"] = f.read(lnlen)
            eintraege.append(e)
            p += 46 + nlen + xlen + clen
    return dict(groesse=groesse, eocd_pos=eocd_pos, cd_off=cd_off, cd_size=cd_size, eintraege=eintraege)


def finde(z, name):
    nb = name.encode("utf-8")
    t = [e for e in z["eintraege"] if e["name"] == nb]
    if len(t) != 1:
        raise SystemExit("Eintrag %s %d-mal gefunden" % (name, len(t)))
    return t[0]


def inspect(pfad):
    z = cd_lesen(pfad)
    es = z["eintraege"]
    print("Datei %d B, %d Eintraege, CD @%d (%d B), EOCD @%d" % (z["groesse"], len(es), z["cd_off"], z["cd_size"],
                                                                z["eocd_pos"]))
    ende = max(e["daten"] + e["csize"] for e in es)
    print("Ende letzter Eintragsdaten @%d, Luecke bis CD = %d B (APK Signing Block?)" % (ende, z["cd_off"] - ende))
    if z["cd_off"] - ende >= 24:
        with open(pfad, "rb") as f:
            f.seek(z["cd_off"] - 16)
            print("   16 B vor CD: %r" % f.read(16))
    print("Flags-Bits gesetzt: bit3(DD)=%d  bit11(UTF8)=%d  verschl=%d" % (
        sum(1 for e in es if e["flags"] & 8), sum(1 for e in es if e["flags"] & 0x800),
        sum(1 for e in es if e["flags"] & 1)))
    print("Methoden: %s" % sorted(set(e["meth"] for e in es)))
    inkons = [e for e in es if (e["crc"], e["csize"], e["usize"]) != (e["lfh_crc"], e["lfh_csize"], e["lfh_usize"])
              or e["name"] != e["lfh_name"]]
    print("LFH/CD inkonsistent: %d" % len(inkons))
    gross = sorted((e for e in es if e["name"].startswith(b"assets/")), key=lambda e: -e["usize"])[:5]
    for e in gross:
        print("   gross: %10d  %s" % (e["usize"], e["name"].decode()))
    bs = [e for e in es if b"\\" in e["name"] or b"\x00" in e["name"]]
    print("Namen mit '\\\\' oder NUL: %d" % len(bs))


def kopie(src, dst):
    shutil.copyfile(src, dst)


def patch(pfad, pos, neu):
    with open(pfad, "r+b") as f:
        f.seek(pos)
        f.write(neu)


def crc_kollision(daten):
    """1 Nutzbyte bei L-40 kippen, 4 Ausgleichsbytes bei L-32..L-29 so waehlen, dass CRC32 gleich bleibt."""
    L = len(daten)
    if L < 64:
        raise SystemExit("Eintrag zu klein")
    q, p, s = L - 40, L - 32, L - 40            # Nutzbyte, Ausgleichsfenster, Suffix-Start
    n = L - s
    null = zlib.crc32(b"\0" * n)

    def f_lin(delta):                            # linearer Anteil (GF(2)) fuer ein Delta im Suffix
        return zlib.crc32(bytes(delta)) ^ null

    nutz = bytearray(n)
    nutz[q - s] = 0x01
    ziel = f_lin(nutz)                           # das muessen die Ausgleichsbits aufheben
    spalten = []
    for bit in range(32):
        d = bytearray(n)
        d[p - s + bit // 8] = 1 << (bit % 8)
        spalten.append(f_lin(d))
    # Gauss ueber GF(2): finde x (32 Bit) mit XOR_{bit in x} spalten[bit] == ziel
    zeilen = [(spalten[b], 1 << b) for b in range(32)]
    basis = []                                   # (wert, kombination) mit eindeutigem hoechsten Bit
    for w, k in zeilen:
        for bw, bk in basis:
            if w ^ bw < w:
                w, k = w ^ bw, k ^ bk
        if w:
            basis.append((w, k))
            basis.sort(key=lambda t: -t[0])
    w, k = ziel, 0
    for bw, bk in basis:
        if w ^ bw < w:
            w, k = w ^ bw, k ^ bk
    if w:
        raise SystemExit("kein Ausgleich gefunden")
    neu = bytearray(daten)
    neu[q] ^= 0x01
    for bit in range(32):
        if k >> bit & 1:
            neu[p + bit // 8] ^= 1 << (bit % 8)
    neu = bytes(neu)
    assert neu != daten and len(neu) == L
    assert zlib.crc32(neu) == zlib.crc32(daten), "CRC nicht erhalten"
    return neu, [q] + [p + i for i in range(4)]


def neu_schreiben(src, dst, umbenennen):
    """ZIP neu schreiben (ohne Signing Block): Eintraege in LFH-Reihenfolge, Namen per umbenennen(name)->name."""
    z = cd_lesen(src)
    es = sorted(z["eintraege"], key=lambda e: e["lho"])
    neue_cd = []
    with open(src, "rb") as fi, open(dst, "wb") as fo:
        for e in es:
            fi.seek(e["lho"])
            lfh = bytearray(fi.read(30))
            fi.read(e["lfh_nlen"])
            extra = fi.read(e["lfh_xlen"])
            laenge = e["csize"]
            if e["lfh_flags"] & 8:
                raise SystemExit("Data Descriptor nicht unterstuetzt")
            name = umbenennen(e["name"])
            neu_lho = fo.tell()
            struct.pack_into("<H", lfh, 26, len(name))
            fo.write(lfh)
            fo.write(name)
            fo.write(extra)
            rest = laenge
            while rest:
                b = fi.read(min(rest, 1 << 20))
                if not b:
                    raise SystemExit("Daten zu kurz")
                fo.write(b)
                rest -= len(b)
            cd = bytearray(e["cd_raw"])
            nlen, xlen, clen = e["nlen"], e["xlen"], e["clen"]
            kopf = cd[:46]
            struct.pack_into("<H", kopf, 28, len(name))
            struct.pack_into("<I", kopf, 42, neu_lho)
            neue_cd.append(bytes(kopf) + name + bytes(cd[46 + nlen:46 + nlen + xlen + clen]))
        cd_off = fo.tell()
        for c in neue_cd:
            fo.write(c)
        cd_size = fo.tell() - cd_off
        fo.write(struct.pack("<4sHHHHIIH", EOCD_SIG, 0, 0, len(neue_cd), len(neue_cd), cd_size, cd_off, 0))


def forge(src, dst, art, arg1=None, arg2=None):
    z = cd_lesen(src)
    if art == "backslash":
        e = finde(z, arg1)
        neu = e["name"].replace(b"/", b"\\")
        kopie(src, dst)
        patch(dst, e["lho"] + 30, neu)
        patch(dst, e["cd_pos"] + 46, neu)
        print("backslash: %s -> %r (LFH @%d, CD @%d)" % (arg1, neu, e["lho"] + 30, e["cd_pos"] + 46))
    elif art == "case":
        e = finde(z, arg1)
        neu = arg2.encode()
        if len(neu) != len(e["name"]):
            raise SystemExit("gleiche Laenge noetig")
        kopie(src, dst)
        patch(dst, e["lho"] + 30, neu)
        patch(dst, e["cd_pos"] + 46, neu)
        print("case: %s -> %s" % (arg1, arg2))
    elif art == "lfh_crc":
        e = finde(z, arg1)
        kopie(src, dst)
        patch(dst, e["lho"] + 14, struct.pack("<I", e["lfh_crc"] ^ 1))
        print("lfh_crc: %s LFH-CRC %08x -> %08x, CD-CRC bleibt %08x" % (arg1, e["lfh_crc"], e["lfh_crc"] ^ 1, e["crc"]))
    elif art == "lfh_usize":
        e = finde(z, arg1)
        kopie(src, dst)
        patch(dst, e["lho"] + 22, struct.pack("<I", e["lfh_usize"] + 1))
        print("lfh_usize: %s LFH-usize %d -> %d, CD bleibt %d" % (arg1, e["lfh_usize"], e["lfh_usize"] + 1, e["usize"]))
    elif art == "crc_kollision":
        e = finde(z, arg1)
        if e["meth"] != 0:
            raise SystemExit("nur Stored")
        with open(src, "rb") as f:
            f.seek(e["daten"])
            daten = f.read(e["csize"])
        neu, pos = crc_kollision(daten)
        kopie(src, dst)
        for i in pos:
            patch(dst, e["daten"] + i, neu[i:i + 1])
        import hashlib
        print("crc_kollision: %s (%d B) Bytes %s geaendert; CRC32 alt %08x neu %08x; sha256 alt %s.. neu %s.."
              % (arg1, len(daten), pos, zlib.crc32(daten), zlib.crc32(neu), hashlib.sha256(daten).hexdigest()[:16],
                 hashlib.sha256(neu).hexdigest()[:16]))
    elif art == "nul":
        ziel = arg1.encode()

        def umb(n):
            return n + b"\x00abc" if n == ziel else n
        finde(z, arg1)
        neu_schreiben(src, dst, umb)
        print("nul: %s -> %r (neu geschrieben)" % (arg1, ziel + b"\x00abc"))
    elif art == "neu_identisch":                 # Kontrolle: Neuschreiben selbst loest nichts aus
        neu_schreiben(src, dst, lambda n: n)
        print("neu_identisch: neu geschrieben ohne Aenderung (Signing Block entfaellt)")
    elif art == "abschneiden":
        n = int(arg1)
        with open(src, "rb") as fi, open(dst, "wb") as fo:
            rest = z["groesse"] - n
            while rest:
                b = fi.read(min(rest, 1 << 20))
                fo.write(b)
                rest -= len(b)
        print("abschneiden: letzte %d B weg (%d -> %d)" % (n, z["groesse"], z["groesse"] - n))
    elif art == "cd_sig":
        e = finde(z, arg1)
        kopie(src, dst)
        patch(dst, e["cd_pos"], b"PK\x09\x09")
        print("cd_sig: CD-Eintrag %s @%d zerstoert" % (arg1, e["cd_pos"]))
    else:
        raise SystemExit("unbekannte Art %s" % art)


def crc_zeigen(pfad, name):
    z = cd_lesen(pfad)
    e = finde(z, name)
    with open(pfad, "rb") as f:
        f.seek(e["daten"])
        d = f.read(e["csize"])
    print("%s: CD-CRC %08x  LFH-CRC %08x  Daten-CRC %08x  CD-usize %d LFH-usize %d  Name CD %r LFH %r" % (
        name, e["crc"], e["lfh_crc"], zlib.crc32(d), e["usize"], e["lfh_usize"], e["name"], e["lfh_name"]))


if __name__ == "__main__":
    a = sys.argv[1:]
    if a[0] == "inspect":
        inspect(a[1])
    elif a[0] == "forge":
        forge(*a[1:])
    elif a[0] == "crc":
        crc_zeigen(a[1], a[2])
    else:
        raise SystemExit(__doc__)
