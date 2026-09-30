# Pruefer echtlauf r1: Inhalt zweier (zusammengefuehrter) Paket-ZIPs vergleichen: Namen, Groesse, CRC,
# Unix-Modus (external_attr >> 16). Zeitstempel werden NICHT verglichen (Kopierzeit). Rueckgabe 0 = gleich.
import sys, zipfile
a, b = zipfile.ZipFile(sys.argv[1]), zipfile.ZipFile(sys.argv[2])
ia = {i.filename: i for i in a.infolist()}; ib = {i.filename: i for i in b.infolist()}
nur_a = sorted(set(ia) - set(ib)); nur_b = sorted(set(ib) - set(ia))
anders = []
for n in sorted(set(ia) & set(ib)):
    x, y = ia[n], ib[n]
    if (x.file_size, x.CRC, x.external_attr >> 16) != (y.file_size, y.CRC, y.external_attr >> 16):
        anders.append("%s: alt %d B crc %08x mode %o | neu %d B crc %08x mode %o" % (
            n, x.file_size, x.CRC, x.external_attr >> 16, y.file_size, y.CRC, y.external_attr >> 16))
print("alt %s: %d Eintraege | neu %s: %d Eintraege" % (sys.argv[1], len(ia), sys.argv[2], len(ib)))
print("nur alt: %d %s" % (len(nur_a), nur_a[:5])); print("nur neu: %d %s" % (len(nur_b), nur_b[:5]))
print("anders (Groesse/CRC/Modus): %d" % len(anders))
for z in anders[:10]: print("   " + z)
sys.exit(0 if not (nur_a or nur_b or anders) else 1)
