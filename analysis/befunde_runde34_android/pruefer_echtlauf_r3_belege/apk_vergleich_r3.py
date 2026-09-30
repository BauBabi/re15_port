# Pruefer echtlauf r3 - EIGENER Vergleich zweier APKs + Abgleich mit dem Quellbaum, UNABHAENGIG vom Gate
# (release/apk_asset_gate.py wird weder importiert noch aufgerufen; eigener Rohleser fuer Zentralverzeichnis und
# Local Header, zlib fuer Deflate, eigener Baum-Lauf). Aufruf mit einem ECHTEN Python (nie dem WindowsApps-Alias):
#   /c/Python310/python apk_vergleich_r3.py <referenz.apk> <neu.apk> <repo>
# Rueckgabe 0 = unter assets/ alles gleich (Liste, Reihenfolge, Methode, Groessen, CRC, sha256) UND jede
# Asset-Datei = Quellbaum UND jede Quelldatei der fuenf Baeume in der APK; 1 = Abweichung; 2 = Lesefehler.
import hashlib
import os
import struct
import sys
import zlib


def cd_lesen(pfad):
    with open(pfad, "rb") as f:
        f.seek(0, 2)
        groesse = f.tell()
        f.seek(max(0, groesse - 65557))
        ende = f.read()
    i = ende.rfind(b"PK\x05\x06")
    if i < 0:
        raise SystemExit("kein EOCD: %s" % pfad)
    (_sig, disk, cd_disk, n_hier, n_ges, cd_size, cd_off, klen) = struct.unpack("<IHHHHIIH", ende[i:i + 22])
    if disk or cd_disk or n_hier != n_ges:
        raise SystemExit("mehrteilig? %s" % pfad)
    with open(pfad, "rb") as f:
        f.seek(cd_off)
        cd = f.read(cd_size)
    eintraege, p = [], 0
    for _ in range(n_ges):
        if cd[p:p + 4] != b"PK\x01\x02":
            raise SystemExit("CD kaputt bei %d in %s" % (p, pfad))
        (flags, meth, _t, _d, crc, csize, usize, nlen, xlen, clen) = struct.unpack("<HHHHIIIHHH", cd[p + 8:p + 34])
        lfh_off = struct.unpack("<I", cd[p + 42:p + 46])[0]
        name = cd[p + 46:p + 46 + nlen]
        eintraege.append(dict(name=name, flags=flags, meth=meth, crc=crc, csize=csize, usize=usize, lfh=lfh_off))
        p += 46 + nlen + xlen + clen
    if p != cd_size:
        raise SystemExit("CD-Rest %d B in %s" % (cd_size - p, pfad))
    return groesse, eintraege


def inhalt(f, e):
    f.seek(e["lfh"])
    kopf = f.read(30)
    if kopf[:4] != b"PK\x03\x04":
        raise SystemExit("LFH-Signatur fehlt: %r" % e["name"])
    (_v, _fl, _m, _t, _d, crc, csize, usize, nlen, xlen) = struct.unpack("<HHHHHIIIHH", kopf[4:30])
    lname = f.read(nlen)
    if lname != e["name"] or (crc, csize, usize) != (e["crc"], e["csize"], e["usize"]):
        raise SystemExit("LFH != CD: %r" % e["name"])
    daten_off = e["lfh"] + 30 + nlen + xlen
    f.seek(daten_off)
    roh = f.read(e["csize"])
    if e["meth"] == 0:
        daten = roh
    elif e["meth"] == 8:
        daten = zlib.decompress(roh, -15)
    else:
        raise SystemExit("Methode %d: %r" % (e["meth"], e["name"]))
    if len(daten) != e["usize"] or (zlib.crc32(daten) & 0xFFFFFFFF) != e["crc"]:
        raise SystemExit("CRC/Laenge falsch: %r" % e["name"])
    return daten_off, hashlib.sha256(daten).hexdigest(), daten


def apk_lesen(pfad):
    groesse, ee = cd_lesen(pfad)
    aus = []
    with open(pfad, "rb") as f:
        for e in ee:
            off, sha, daten = inhalt(f, e)
            e["daten_off"], e["sha"] = off, sha
            if e["name"] == b"assets/re15_assets.txt":
                e["manifest"] = daten
            aus.append(e)
    return groesse, aus


def quellbaum(repo):
    """Die fuenf Baeume, eigener Lauf: shared_assets/{PSX,extracted_fx,RE2,RE15DOOR} und synchro/STAGE*/**."""
    dateien = {}
    wurzeln = [(os.path.join(repo, "re15_port", "shared_assets", b), "shared_assets/" + b, b)
               for b in ("PSX", "extracted_fx", "RE2", "RE15DOOR")]
    sy = os.path.join(repo, "synchro")
    for st in sorted(os.listdir(sy)):
        if st.startswith("STAGE") and os.path.isdir(os.path.join(sy, st)):
            wurzeln.append((os.path.join(sy, st), "synchro/" + st, "synchro"))
    for basis, ziel, baum in wurzeln:
        for d, _ds, fs in os.walk(basis):
            for fn in fs:
                voll = os.path.join(d, fn)
                rel = os.path.relpath(voll, basis).replace(os.sep, "/")
                h = hashlib.sha256()
                with open(voll, "rb") as g:
                    for blk in iter(lambda: g.read(1 << 20), b""):
                        h.update(blk)
                dateien["assets/%s/%s" % (ziel, rel)] = (os.path.getsize(voll), h.hexdigest(), baum)
    return dateien


def main():
    ref_p, neu_p, repo = sys.argv[1:4]
    rc = 0
    rg, ref = apk_lesen(ref_p)
    ng, neu = apk_lesen(neu_p)
    print("Referenz: %s (%d B, %d Eintraege)" % (ref_p, rg, len(ref)))
    print("Neu:      %s (%d B, %d Eintraege)" % (neu_p, ng, len(neu)))
    rn = [e["name"] for e in ref]
    nn = [e["name"] for e in neu]
    print("Namen eindeutig: ref %s, neu %s" % (len(set(rn)) == len(rn), len(set(nn)) == len(nn)))
    ra = [e for e in ref if e["name"].startswith(b"assets/")]
    na = [e for e in neu if e["name"].startswith(b"assets/")]
    print("assets/: ref %d, neu %d; Reihenfolge gleich: %s" % (len(ra), len(na), [e["name"] for e in ra] == [e["name"] for e in na]))
    rd = {e["name"]: e for e in ref}
    nd = {e["name"]: e for e in neu}
    nur_ref = sorted(set(rd) - set(nd))
    nur_neu = sorted(set(nd) - set(rd))
    print("nur Referenz: %d %s" % (len(nur_ref), nur_ref[:5]))
    print("nur neu:      %d %s" % (len(nur_neu), nur_neu[:5]))
    if nur_ref or nur_neu:
        rc = 1
    asset_abw, sonst_gleich, sonst_anders = [], [], []
    for name in rn:
        if name not in nd:
            continue
        a, b = rd[name], nd[name]
        felder = [k for k in ("meth", "crc", "csize", "usize", "sha", "flags") if a[k] != b[k]]
        if name.startswith(b"assets/"):
            if felder:
                asset_abw.append((name, felder))
        else:
            (sonst_anders if felder else sonst_gleich).append((name, felder, a["usize"], b["usize"]))
    print("assets/ mit Abweichung (Methode/CRC/Groessen/sha256/Flags): %d %s" % (len(asset_abw), asset_abw[:5]))
    if asset_abw:
        rc = 1
    print("ausserhalb assets/ gleich: %d  anders: %d" % (len(sonst_gleich), len(sonst_anders)))
    for name, felder, ua, ub in sonst_anders:
        print("   anders: %s %s (%d B -> %d B)" % (name.decode("utf-8", "replace"), felder, ua, ub))
    stored_r = sum(1 for e in ra if e["meth"] == 0)
    stored_n = sum(1 for e in na if e["meth"] == 0)
    print("assets/ Stored: ref %d/%d, neu %d/%d" % (stored_r, len(ra), stored_n, len(na)))
    ausr = [e["name"] for e in na if e["meth"] == 0 and e["daten_off"] % 4]
    so = [e["name"] for e in neu if e["name"].endswith(b".so") and e["daten_off"] % 16384]
    print("neu: Stored-Assets nicht 4-B-ausgerichtet: %d; .so nicht 16-KiB-ausgerichtet: %d" % (len(ausr), len(so)))
    if ausr or so:
        rc = 1
    mr, mn = rd[b"assets/re15_assets.txt"]["manifest"], nd[b"assets/re15_assets.txt"]["manifest"]
    print("Manifest bytegleich: %s (sha256 %s / %s), Kopf neu: %r" % (mr == mn, hashlib.sha256(mr).hexdigest()[:16],
          hashlib.sha256(mn).hexdigest()[:16], mn.split(b"\n", 1)[0]))
    if mr != mn:
        rc = 1
    # Manifest-Zeilen gegen die neue APK (eigene Lesart wie android_glue: \n, \r ab, leer/# ueberspringen, <bytes>\t<pfad>)
    zeilen, fehler = 0, []
    for z in mn.split(b"\n"):
        z = z.rstrip(b"\r")
        if not z or z.startswith(b"#"):
            continue
        n_s, _tab, pfad = z.partition(b"\t")
        zeilen += 1
        e = nd.get(b"assets/" + pfad)
        if e is None or e["usize"] != int(n_s):
            fehler.append(pfad)
    print("Manifest: %d Zeilen, ohne passenden Eintrag: %d %s" % (zeilen, len(fehler), fehler[:3]))
    if fehler or zeilen != len(na) - 1:
        rc = 1
    # Quellbaum
    q = quellbaum(repo)
    je_baum = {}
    q_fehlt, q_anders = [], []
    for pfad, (n, sha, baum) in q.items():
        e = nd.get(pfad.encode("utf-8"))
        t = je_baum.setdefault(baum, [0, 0])
        t[0] += 1
        if e is None:
            q_fehlt.append(pfad)
        elif e["usize"] != n or e["sha"] != sha:
            q_anders.append(pfad)
        else:
            t[1] += 1
    apk_ohne_quelle = [e["name"] for e in na if e["name"] != b"assets/re15_assets.txt"
                       and e["name"].decode("utf-8") not in q]
    print("Quellbaum: %d Dateien; je Baum (Quelle/gleich in neu): %s" % (len(q), {k: tuple(v) for k, v in sorted(je_baum.items())}))
    print("Quellbaum: fehlt in neu %d, anders %d, APK-Asset ohne Quelle %d" % (len(q_fehlt), len(q_anders), len(apk_ohne_quelle)))
    for lst in (q_fehlt, q_anders, apk_ohne_quelle):
        for x in lst[:5]:
            print("   %r" % (x,))
    if q_fehlt or q_anders or apk_ohne_quelle:
        rc = 1
    tuer_r2 = sorted(p for p in q if p.startswith("assets/shared_assets/RE2/DOOR/"))
    tuer_eig = sorted(p for p in q if p.startswith("assets/shared_assets/RE15DOOR/"))
    torse = "assets/shared_assets/RE2/TORSE.VBS"
    print("RE2/DOOR: %d Quelle, %d gleich in neu; RE15DOOR: %d Quelle, %d gleich in neu; TORSE.VBS gleich: %s" % (
        len(tuer_r2), sum(1 for p in tuer_r2 if p not in q_fehlt and p not in q_anders),
        len(tuer_eig), sum(1 for p in tuer_eig if p not in q_fehlt and p not in q_anders),
        torse in q and torse not in q_fehlt and torse not in q_anders))
    print("ERGEBNIS rc=%d" % rc)
    return rc


if __name__ == "__main__":
    sys.exit(main())
