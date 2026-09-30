# Pruefer echtlauf r3 - zusammengefuehrte Pakete (zip -s 0) gegen die Archiv-Pakete v0.8.19: Name, Groesse, CRC32,
# Unix-Modus je Eintrag (aus dem Zentralverzeichnis, zipfile). Eigenes Werkzeug, nicht aus make_package.sh.
# Aufruf: /c/Python310/python paket_vergleich_r3.py <alt_ganz.zip> <neu_ganz.zip> [<gepruefte_apk_sha256>]
import hashlib
import sys
import zipfile


def katalog(p):
    with zipfile.ZipFile(p) as z:
        return [(i.filename, i.file_size, i.CRC, (i.external_attr >> 16) & 0o777777) for i in z.infolist()]


alt, neu = katalog(sys.argv[1]), katalog(sys.argv[2])
da, dn = {e[0]: e for e in alt}, {e[0]: e for e in neu}
nur_alt = sorted(set(da) - set(dn))
nur_neu = sorted(set(dn) - set(da))
anders = [(n, da[n][1:], dn[n][1:]) for n in sorted(set(da) & set(dn)) if da[n] != dn[n]]
print("%s: %d Eintraege | %s: %d Eintraege" % (sys.argv[1], len(alt), sys.argv[2], len(neu)))
print("nur alt: %d %s" % (len(nur_alt), nur_alt[:5]))
print("nur neu: %d %s" % (len(nur_neu), nur_neu[:5]))
print("anders (Groesse, CRC32, Modus): %d" % len(anders))
for n, a, b in anders[:10]:
    print("   %s: alt %s / neu %s" % (n, (a[0], "%08x" % a[1], oct(a[2])), (b[0], "%08x" % b[1], oct(b[2]))))
modi = sorted({(e[0].rsplit("/", 1)[-1], oct(e[3])) for e in neu if e[0].endswith(("re15_pc", "run.sh"))})
print("Modi neu (re15_pc/run.sh): %s" % modi)
if len(sys.argv) > 3:
    with zipfile.ZipFile(sys.argv[2]) as z:
        apks = [i for i in z.infolist() if i.filename.endswith(".apk")]
        for i in apks:
            h = hashlib.sha256()
            with z.open(i) as f:
                for b in iter(lambda: f.read(1 << 20), b""):
                    h.update(b)
            print("APK im neuen Satz: %s sha256 %s = gepruefte APK: %s" % (i.filename, h.hexdigest()[:16],
                  h.hexdigest() == sys.argv[3]))
