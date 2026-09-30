# -*- coding: utf-8 -*-
"""Pruefer UMGEHUNG R4-1, H5/H6: APK-Faelschung "FK" fuer den Geraete-Versuch zur Unicode-Faltung.
Basis: eine APK mit Liste v2 (N2). Dazu zwei Stored-Eintraege gleicher Groesse, verschiedenen Inhalts, deren Namen sich
nur in Unicode-Gross/klein unterscheiden:
    assets/shared_assets/PSX/K.bin        "AAAAA"
    assets/shared_assets/PSX/<U+212A>.bin "BBBBB"   (KELVIN SIGN; Unicode-Faltung -> 'k')
und zwei passende Listenzeilen (Groesse, sha256), Kopfzeile nachgerechnet. Beide Leser (asset_abgleich.c, Gate
manifest_lesen) nehmen diese Liste an (Korpus pfad_kelvin_gegen_K). Schreiber/Leser: r3_faelschen.py (unveraendert,
als Modul). Danach signieren mit r3_signieren.sh.
Aufruf: u1_faelschen_fk.py <basis.apk> <ziel.apk>"""
import hashlib
import importlib.util
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
R3 = os.path.join(HIER, "..", "pruefer_umgehung_r3_belege", "r3_faelschen.py")


def main():
    basis, ziel = sys.argv[1], sys.argv[2]
    spec = importlib.util.spec_from_file_location("r3f", R3)
    r3f = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(r3f)
    ein, eocd = r3f.lesen(basis)
    idx = {e["name"]: e for e in ein}
    em = idx[r3f.MAN]
    m = bytes(em["daten"])
    kopf = m.split(b"\n")[0]
    teile = kopf.split(b" ")
    assert teile[:4] == [b"#", b"re15", b"assets", b"v2"], kopf
    n, b = int(teile[4]), int(teile[5])
    daten = [(b"shared_assets/PSX/K.bin", b"AAAAA"), ("shared_assets/PSX/K.bin".encode("utf-8"), b"BBBBB")]
    zeilen = b"".join(b"%d\t%s\t%s\n" % (len(d), hashlib.sha256(d).hexdigest().encode(), p) for p, d in daten)
    assert m.endswith(b"\n")
    neu_kopf = b"# re15 assets v2 %d %d" % (n + len(daten), b + sum(len(d) for _p, d in daten))
    m = neu_kopf + m[len(kopf):] + zeilen
    r3f.daten_setzen(em, m)
    for p, d in daten:
        ein.append(r3f.neuer_eintrag(em, b"assets/" + p, d, 10 ** 12 + len(ein)))
    r3f.schreiben(ziel, ein, eocd)
    print("FK: Kopf %r -> %r, angehaengt:" % (kopf, neu_kopf))
    for z in zeilen.split(b"\n")[:-1]:
        print("   %r" % z)
    print("geschrieben: %s (%d Eintraege)" % (ziel, len(ein)))


if __name__ == "__main__":
    main()
