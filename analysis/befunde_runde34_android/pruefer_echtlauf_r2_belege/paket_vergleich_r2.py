# -*- coding: utf-8 -*-
"""Pruefer echtlauf r2: zwei (zusammengefuehrte) Paket-ZIPs Eintrag fuer Eintrag vergleichen.
Name, Groesse, CRC, Unix-Modus (external_attr >> 16); Zeitstempel ausgenommen (cp ohne -p).
Aufruf: <python> paket_vergleich_r2.py <alt.zip> <neu.zip> [--sha256-eintrag <name>]
Rueckgabe 0 = gleich, 1 = Unterschied, 2 = Fehler."""
import hashlib
import sys
import zipfile


def main():
    a_p, n_p = sys.argv[1], sys.argv[2]
    sha_name = sys.argv[4] if len(sys.argv) > 4 and sys.argv[3] == "--sha256-eintrag" else None
    with zipfile.ZipFile(a_p) as a, zipfile.ZipFile(n_p) as n:
        ai = {i.filename: i for i in a.infolist()}
        ni = {i.filename: i for i in n.infolist()}
        nur_a, nur_n = sorted(set(ai) - set(ni)), sorted(set(ni) - set(ai))
        anders = []
        for k in sorted(set(ai) & set(ni)):
            x, y = ai[k], ni[k]
            g = []
            if x.file_size != y.file_size:
                g.append("Groesse %d/%d" % (x.file_size, y.file_size))
            if x.CRC != y.CRC:
                g.append("CRC %08x/%08x" % (x.CRC, y.CRC))
            if (x.external_attr >> 16) != (y.external_attr >> 16):
                g.append("Modus %o/%o" % (x.external_attr >> 16, y.external_attr >> 16))
            if g:
                anders.append("%s: %s" % (k, "; ".join(g)))
        print("alt %s: %d Eintraege | neu %s: %d Eintraege" % (a_p, len(ai), n_p, len(ni)))
        print("nur alt %d, nur neu %d, anders %d" % (len(nur_a), len(nur_n), len(anders)))
        for z in (["NUR ALT " + s for s in nur_a] + ["NUR NEU " + s for s in nur_n] + ["ANDERS " + s for s in anders])[:30]:
            print("   " + z)
        if sha_name:
            h = hashlib.sha256()
            with n.open(sha_name) as f:
                while True:
                    b = f.read(1 << 20)
                    if not b:
                        break
                    h.update(b)
            print("sha256 neu:%s = %s" % (sha_name, h.hexdigest()))
        return 0 if not (nur_a or nur_n or anders) else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except SystemExit:
        raise
    except BaseException as e:  # noqa
        print("FEHLER: %r" % (e,))
        sys.exit(2)
