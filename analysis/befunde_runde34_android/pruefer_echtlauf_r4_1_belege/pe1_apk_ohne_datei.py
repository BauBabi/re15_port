# Pruefer echtlauf R4-1: MESS-APK (kein Auslieferungsstueck) = Eingabe-APK ohne EINE Asset-Datei - Eintrag und Listenzeile
# entfernt, Kopfzeile (Anzahl/Bytes) nachgerechnet, alle uebrigen Eintraege mit Name/Methode/Datum/Attributen unveraendert
# uebernommen. Danach zipalign + Signieren mit dem Debug-Schluessel (pe1_signieren.sh) -> installierbar als Update.
# Aufruf: python pe1_apk_ohne_datei.py <ein.apk> <aus.zip> <asset-pfad relativ zu assets/>
import sys, zipfile, hashlib
src, dst, weg = sys.argv[1:4]
zi = zipfile.ZipFile(src)
zo = zipfile.ZipFile(dst, "w")
n_weg = 0
for info in zi.infolist():
    if info.filename == "assets/" + weg:
        n_weg += 1
        continue
    if info.filename.startswith("META-INF/"):
        print("Hinweis: uebernehme", info.filename)
    data = zi.read(info.filename)
    if info.filename == "assets/re15_assets.txt":
        zl = data.decode("utf-8").split("\n")
        kopf, rows = zl[0].split(" "), zl[1:-1]
        treffer = [r for r in rows if r.split("\t")[2] == weg]
        assert len(treffer) == 1, treffer
        rows = [r for r in rows if r.split("\t")[2] != weg]
        kopf[4] = str(int(kopf[4]) - 1)
        kopf[5] = str(int(kopf[5]) - int(treffer[0].split("\t")[0]))
        data = (" ".join(kopf) + "\n" + "\n".join(rows) + "\n").encode("utf-8")
        print("Liste neu: %s | %d Zeilen | sha256 %s" % (" ".join(kopf), len(rows), hashlib.sha256(data).hexdigest()))
    ni = zipfile.ZipInfo(info.filename, date_time=info.date_time)
    ni.compress_type = info.compress_type
    ni.external_attr = info.external_attr
    ni.create_system = info.create_system
    zo.writestr(ni, data)
zo.close()
assert n_weg == 1, n_weg
print("entfernt: assets/%s" % weg)
