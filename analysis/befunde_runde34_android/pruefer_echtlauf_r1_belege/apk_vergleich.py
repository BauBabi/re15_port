# -*- coding: utf-8 -*-
"""Pruefer echtlauf r1: zwei APKs Eintrag fuer Eintrag vergleichen (UNABHAENGIG von apk_asset_gate.py).

Aufruf: <echtes python> apk_vergleich.py <referenz.apk> <neu.apk>
Ausgabe: Mengenvergleich der Eintragsnamen (gesamt und unter assets/), je gemeinsamem Eintrag
Groesse/CRC/Methode, fuer ALLE assets/-Eintraege zusaetzlich sha256 des entpackten Inhalts, Liste der
Unterschiede ausserhalb assets/. Rueckgabe 0 = assets/ identisch (Namen, Groesse, CRC, sha256, Methode),
1 = Unterschied unter assets/, 2 = Fehler.
"""
import hashlib
import sys
import zipfile


def sha(zf, info):
    h = hashlib.sha256()
    with zf.open(info) as f:
        while True:
            b = f.read(1 << 20)
            if not b:
                return h.hexdigest()
            h.update(b)


def main():
    ref_p, neu_p = sys.argv[1], sys.argv[2]
    with zipfile.ZipFile(ref_p) as ref, zipfile.ZipFile(neu_p) as neu:
        ri = {i.filename: i for i in ref.infolist()}
        ni = {i.filename: i for i in neu.infolist()}
        print("Referenz: %s  Eintraege %d (Namen eindeutig: %d)" % (ref_p, len(ref.infolist()), len(ri)))
        print("Neu:      %s  Eintraege %d (Namen eindeutig: %d)" % (neu_p, len(neu.infolist()), len(ni)))
        ra = {n for n in ri if n.startswith("assets/")}
        na = {n for n in ni if n.startswith("assets/")}
        print("assets/-Eintraege: Referenz %d, neu %d, gemeinsam %d, nur Referenz %d, nur neu %d"
              % (len(ra), len(na), len(ra & na), len(ra - na), len(na - ra)))
        for n in sorted(ra - na)[:20]:
            print("   NUR REFERENZ: %s" % n)
        for n in sorted(na - ra)[:20]:
            print("   NUR NEU:      %s" % n)
        abw_assets = len(ra ^ na)
        n_sha = 0
        bytes_sum = 0
        for n in sorted(ra & na):
            a, b = ri[n], ni[n]
            grund = []
            if a.file_size != b.file_size:
                grund.append("Groesse %d/%d" % (a.file_size, b.file_size))
            if a.CRC != b.CRC:
                grund.append("CRC %08x/%08x" % (a.CRC, b.CRC))
            if a.compress_type != b.compress_type:
                grund.append("Methode %d/%d" % (a.compress_type, b.compress_type))
            sa, sb = sha(ref, a), sha(neu, b)
            n_sha += 1
            bytes_sum += b.file_size
            if sa != sb:
                grund.append("sha256 %s../%s.." % (sa[:12], sb[:12]))
            if grund:
                abw_assets += 1
                print("   ABWEICHUNG %s: %s" % (n, "; ".join(grund)))
        print("assets/: %d gemeinsame Eintraege per sha256 verglichen (%d Bytes), Abweichungen unter assets/: %d"
              % (n_sha, bytes_sum, abw_assets))
        stored_ref = sum(1 for n in ra if ri[n].compress_type == zipfile.ZIP_STORED)
        stored_neu = sum(1 for n in na if ni[n].compress_type == zipfile.ZIP_STORED)
        print("assets/ Stored: Referenz %d/%d, neu %d/%d" % (stored_ref, len(ra), stored_neu, len(na)))
        man = "assets/re15_assets.txt"
        if man in ri and man in ni:
            print("Manifest sha256: Referenz %s, neu %s" % (sha(ref, ri[man]), sha(neu, ni[man])))
        print("--- ausserhalb assets/ ---")
        rs = {n for n in ri if not n.startswith("assets/")}
        ns = {n for n in ni if not n.startswith("assets/")}
        gleich = 0
        for n in sorted(rs | ns):
            if n not in ri:
                print("   nur neu:      %s (%d B)" % (n, ni[n].file_size))
            elif n not in ni:
                print("   nur Referenz: %s (%d B)" % (n, ri[n].file_size))
            elif ri[n].CRC != ni[n].CRC or ri[n].file_size != ni[n].file_size:
                print("   anders:       %s (Referenz %d B crc %08x, neu %d B crc %08x)"
                      % (n, ri[n].file_size, ri[n].CRC, ni[n].file_size, ni[n].CRC))
            else:
                gleich += 1
        print("   gleich (Groesse+CRC): %d von %d/%d" % (gleich, len(rs), len(ns)))
        return 0 if abw_assets == 0 else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except SystemExit:
        raise
    except BaseException as e:
        print("FEHLER: %r" % (e,))
        sys.exit(2)
