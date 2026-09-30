# -*- coding: utf-8 -*-
"""Pruefer echtlauf r2: zwei APKs Eintrag fuer Eintrag vergleichen - UNABHAENGIG von
release/apk_asset_gate.py (kein Import, eigener Code; Inhalt ueber zipfile, Datenoffsets ueber einen
eigenen Mini-Parser des Zentralverzeichnisses + Local Headers).

Aufruf: <echtes python> apk_vergleich_r2.py <referenz.apk> <neu.apk> [--repo <arbeitsbaum>]

Prueft:
  1. Eintragsnamen gesamt + unter assets/ (Menge UND Reihenfolge), Doppelte.
  2. JEDEN Eintrag: Groesse, CRC, Methode, sha256 des entpackten Inhalts (auch ausserhalb assets/).
  3. Manifest assets/re15_assets.txt: sha256 beidseitig; jede Zeile "<bytes>\t<pfad>" gegen die NEUE
     APK (Eintrag assets/<pfad> vorhanden, Groesse gleich), Kopfzeile = Anzahl/Summe.
  4. Datenoffset jedes Stored-Eintrags unter assets/ (aus dem LOCAL Header, eigener Parser):
     4-Byte-Ausrichtung (zipalign; AAssetManager mappt unkomprimierte Assets direkt).
  5. optional --repo: jede Datei der fuenf Baeume (re15_port/shared_assets/{PSX,extracted_fx,RE2,
     RE15DOOR}, synchro/STAGE*/**) liegt mit gleichem sha256 unter assets/ der NEUEN APK, und
     unter assets/ liegt sonst nichts ausser re15_assets.txt.
Rueckgabe 0 = assets/ identisch (+ Repo-Abgleich ok, falls verlangt), 1 = Unterschied, 2 = Fehler.
"""
import hashlib
import os
import struct
import sys
import zipfile

ASSET_BAEUME = [  # (Quellordner relativ zum Repo, Ziel unter assets/, nur Unterordner STAGE*?)
    ("re15_port/shared_assets/PSX", "shared_assets/PSX", False),
    ("re15_port/shared_assets/extracted_fx", "shared_assets/extracted_fx", False),
    ("re15_port/shared_assets/RE2", "shared_assets/RE2", False),
    ("re15_port/shared_assets/RE15DOOR", "shared_assets/RE15DOOR", False),
    ("synchro", "synchro", True),
]


def sha_zip(zf, info):
    h = hashlib.sha256()
    with zf.open(info) as f:
        while True:
            b = f.read(1 << 20)
            if not b:
                return h.hexdigest()
            h.update(b)


def sha_datei(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        while True:
            b = f.read(1 << 20)
            if not b:
                return h.hexdigest()
            h.update(b)


def datenoffsets(pfad):
    """Eigener Mini-Parser: {name_bytes: (methode, datenoffset)} aus EOCD -> CD -> LFH."""
    with open(pfad, "rb") as f:
        d = f.read()
    i = d.rfind(b"PK\x05\x06")
    if i < 0:
        raise ValueError("kein EOCD")
    total, cd_size, cd_off = struct.unpack("<HII", d[i + 10:i + 20])
    p, res = cd_off, {}
    for _ in range(total):
        if d[p:p + 4] != b"PK\x01\x02":
            raise ValueError("CD kaputt bei %d" % p)
        methode = struct.unpack("<H", d[p + 10:p + 12])[0]
        nlen, xlen, klen = struct.unpack("<HHH", d[p + 28:p + 34])
        lfh = struct.unpack("<I", d[p + 42:p + 46])[0]
        name = d[p + 46:p + 46 + nlen]
        if d[lfh:lfh + 4] != b"PK\x03\x04":
            raise ValueError("LFH-Signatur fehlt: %r" % name)
        ln, lx = struct.unpack("<HH", d[lfh + 26:lfh + 30])
        res[name] = (methode, lfh + 30 + ln + lx)
        p += 46 + nlen + xlen + klen
    return res


def repo_dateien(repo):
    """{ 'assets/<ziel>/<rel>': quellpfad } fuer alle Dateien der fuenf Baeume."""
    res = {}
    for quelle, ziel, nur_stage in ASSET_BAEUME:
        basis = os.path.join(repo, quelle)
        if not os.path.isdir(basis):
            raise ValueError("Quellbaum fehlt: %s" % basis)
        for wurzel, ordner, dateien in os.walk(basis):
            rel_w = os.path.relpath(wurzel, basis).replace(os.sep, "/")
            if nur_stage:
                erster = rel_w.split("/")[0]
                if rel_w == ".":
                    ordner[:] = [o for o in ordner if o.startswith("STAGE")]
                    continue  # Dateien direkt in synchro/ gehoeren nicht zu STAGE*/**
                if not erster.startswith("STAGE"):
                    continue
            for dn in dateien:
                rel = dn if rel_w == "." else rel_w + "/" + dn
                res["assets/%s/%s" % (ziel, rel)] = os.path.join(wurzel, dn)
    return res


def main():
    args = sys.argv[1:]
    repo = None
    if "--repo" in args:
        k = args.index("--repo")
        repo = args[k + 1]
        del args[k:k + 2]
    ref_p, neu_p = args[0], args[1]
    abw = 0
    with zipfile.ZipFile(ref_p) as ref, zipfile.ZipFile(neu_p) as neu:
        rl, nl = ref.infolist(), neu.infolist()
        ri = {i.filename: i for i in rl}
        ni = {i.filename: i for i in nl}
        print("Referenz: %s  Eintraege %d (Namen eindeutig %d)" % (ref_p, len(rl), len(ri)))
        print("Neu:      %s  Eintraege %d (Namen eindeutig %d)" % (neu_p, len(nl), len(ni)))
        if len(rl) != len(ri) or len(nl) != len(ni):
            print("   DOPPELTE NAMEN")
            abw += 1
        ra = [i.filename for i in rl if i.filename.startswith("assets/")]
        na = [i.filename for i in nl if i.filename.startswith("assets/")]
        print("assets/: Referenz %d, neu %d, gemeinsam %d, nur Referenz %d, nur neu %d, Reihenfolge gleich: %s"
              % (len(ra), len(na), len(set(ra) & set(na)), len(set(ra) - set(na)), len(set(na) - set(ra)),
                 ra == na))
        for n in sorted(set(ra) - set(na))[:20]:
            print("   NUR REFERENZ: %s" % n)
        for n in sorted(set(na) - set(ra))[:20]:
            print("   NUR NEU:      %s" % n)
        abw += len(set(ra) ^ set(na))
        # 2. jeder Eintrag
        n_sha, bytes_sum, anders_aussen = 0, 0, []
        for n in [i.filename for i in rl]:
            if n not in ni:
                if not n.startswith("assets/"):
                    anders_aussen.append("nur Referenz: %s" % n)
                continue
            a, b = ri[n], ni[n]
            grund = []
            if a.file_size != b.file_size:
                grund.append("Groesse %d/%d" % (a.file_size, b.file_size))
            if a.CRC != b.CRC:
                grund.append("CRC %08x/%08x" % (a.CRC, b.CRC))
            if a.compress_type != b.compress_type:
                grund.append("Methode %d/%d" % (a.compress_type, b.compress_type))
            sa, sb = sha_zip(ref, a), sha_zip(neu, b)
            n_sha += 1
            bytes_sum += b.file_size
            if sa != sb:
                grund.append("sha256 %s../%s.." % (sa[:12], sb[:12]))
            if grund:
                if n.startswith("assets/"):
                    abw += 1
                    print("   ABWEICHUNG %s: %s" % (n, "; ".join(grund)))
                else:
                    anders_aussen.append("anders: %s (%s)" % (n, "; ".join(grund)))
        for n in [i.filename for i in nl]:
            if n not in ri and not n.startswith("assets/"):
                anders_aussen.append("nur neu: %s" % n)
        print("Inhalt: %d gemeinsame Eintraege per sha256 verglichen (%d Bytes entpackt)" % (n_sha, bytes_sum))
        print("assets/ Stored: Referenz %d/%d, neu %d/%d" % (
            sum(1 for n in ra if ri[n].compress_type == 0), len(ra),
            sum(1 for n in na if ni[n].compress_type == 0), len(na)))
        print("--- ausserhalb assets/ (erwartet: nur Code/Signatur/Version) ---")
        for z in anders_aussen:
            print("   " + z)
        print("   gleich: %d von %d/%d" % (
            sum(1 for i in rl if not i.filename.startswith("assets/")) - sum(1 for z in anders_aussen if not z.startswith("nur neu")),
            sum(1 for i in rl if not i.filename.startswith("assets/")),
            sum(1 for i in nl if not i.filename.startswith("assets/"))))
        # 3. Manifest
        man = "assets/re15_assets.txt"
        if man not in ni:
            print("MANIFEST FEHLT in der neuen APK")
            return 1
        mr, mn = ref.read(ri[man]), neu.read(ni[man])
        print("Manifest sha256: Referenz %s, neu %s, gleich: %s" % (
            hashlib.sha256(mr).hexdigest()[:16], hashlib.sha256(mn).hexdigest()[:16], mr == mn))
        if mr != mn:
            abw += 1
        zeilen = mn.decode("utf-8").split("\n")
        kopf = zeilen[0].split()
        n_z, summe, fehl = 0, 0, 0
        genannt = set()
        for z in zeilen[1:]:
            z = z.rstrip("\r")
            if not z or z.startswith("#"):
                continue
            groesse, pfad = z.split("\t", 1)
            n_z += 1
            summe += int(groesse)
            e = ni.get("assets/" + pfad)
            genannt.add("assets/" + pfad)
            if e is None or e.file_size != int(groesse):
                fehl += 1
                print("   MANIFEST-ZEILE OHNE PASSENDEN EINTRAG: %s" % z)
        nicht_genannt = [n for n in na if n != man and n not in genannt]
        print("Manifest: Kopf %s, Zeilen %d, Summe %d, Zeilen ohne passenden Eintrag %d, Asset-Eintraege ohne Zeile %d"
              % (" ".join(kopf), n_z, summe, fehl, len(nicht_genannt)))
        if kopf[:3] != ["#", "re15", "assets"] or int(kopf[3]) != n_z or int(kopf[4]) != summe:
            print("   KOPFZEILE PASST NICHT")
            abw += 1
        abw += fehl + len(nicht_genannt)
    # 4. Ausrichtung (eigener Parser)
    for titel, p in (("Referenz", ref_p), ("neu", neu_p)):
        offs = datenoffsets(p)
        st = [(k, v) for k, v in offs.items() if k.startswith(b"assets/") and v[0] == 0]
        schief = [k for k, v in st if v[1] % 4]
        print("Ausrichtung %s: %d Stored-Assets, davon nicht 4-Byte-ausgerichtet: %d" % (titel, len(st), len(schief)))
        if titel == "neu" and schief:
            abw += 1
    # 5. Repo
    if repo:
        rd = repo_dateien(repo)
        with zipfile.ZipFile(neu_p) as neu:
            ni = {i.filename: i for i in neu.infolist()}
            fehlt, anders, gleich = [], [], 0
            je_baum = {}
            for name, quelle in sorted(rd.items()):
                baum = [z for _q, z, _s in ASSET_BAEUME if name.startswith("assets/" + z + "/")][0]
                je_baum.setdefault(baum, [0, 0])
                je_baum[baum][0] += 1
                e = ni.get(name)
                if e is None:
                    fehlt.append(name)
                    continue
                if os.path.getsize(quelle) != e.file_size or sha_datei(quelle) != sha_zip(neu, e):
                    anders.append(name)
                    continue
                gleich += 1
                je_baum[baum][1] += 1
            extra = [n for n in ni if n.startswith("assets/") and n != "assets/re15_assets.txt" and n not in rd]
            print("Repo-Abgleich (%s): Quelle %d Dateien, sha256 gleich %d, fehlt in APK %d, anders %d, nur APK %d"
                  % (repo, len(rd), gleich, len(fehlt), len(anders), len(extra)))
            for b, (q, g) in je_baum.items():
                print("   %-28s Quelle %5d  gleich %5d" % (b, q, g))
            for n in (fehlt + anders + extra)[:20]:
                print("   ABWEICHUNG Repo: %s" % n)
            abw += len(fehlt) + len(anders) + len(extra)
    print("ERGEBNIS: %s (Abweichungen unter assets/ bzw. Repo: %d)" % ("GLEICH" if abw == 0 else "UNGLEICH", abw))
    return 0 if abw == 0 else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except SystemExit:
        raise
    except BaseException as e:  # noqa
        print("FEHLER: %r" % (e,))
        sys.exit(2)
