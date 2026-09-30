# Pruefer echtlauf R4-1: unabhaengige Pruefung der Liste assets/re15_assets.txt einer APK (eigener Code, nicht das Gate).
# Aufruf: python pe1_apk_liste.py <apk> <repo> [--liste-aus <datei>]
#  - Kopfzeile "# re15 assets v2 <n> <bytes>", jede Zeile "<bytes>\t<sha256>\t<pfad>", sortiert, keine Dubletten
#  - je Zeile: sha256 + Groesse der APK-Daten (zipfile) UND der Quelldatei im Repo == Liste
#  - Menge der APK-Eintraege unter assets/ == Liste + re15_assets.txt
import hashlib, sys, zipfile, os
apk, repo = sys.argv[1], sys.argv[2]
z = zipfile.ZipFile(apk)
raw = z.read("assets/re15_assets.txt")
print("Liste: %d B, sha256 %s" % (len(raw), hashlib.sha256(raw).hexdigest()))
zeilen = raw.decode("utf-8").split("\n")
assert zeilen[-1] == "", "kein Schluss-LF"
kopf, daten = zeilen[0], zeilen[1:-1]
print("Kopf:", kopf)
teile = kopf.split(" ")
assert teile[:4] == ["#", "re15", "assets", "v2"], "kein v2-Kopf"
n_soll, b_soll = int(teile[4]), int(teile[5])
eintraege = []
for i, zl in enumerate(daten, 2):
    g, s, p = zl.split("\t")
    assert g.isascii() and g.isdigit() and len(s) == 64 and all(c in "0123456789abcdef" for c in s), "Zeile %d" % i
    eintraege.append((p, int(g), s))
pfade = [e[0] for e in eintraege]
assert pfade == sorted(pfade), "nicht sortiert"
assert len(set(p.lower() for p in pfade)) == len(pfade), "Dubletten (auch Gross/klein)"
assert len(eintraege) == n_soll and sum(e[1] for e in eintraege) == b_soll, "Kopf passt nicht"
namen = {i.filename for i in z.infolist() if i.filename.startswith("assets/") and not i.filename.endswith("/")}
assert namen == {"assets/" + p for p in pfade} | {"assets/re15_assets.txt"}, "APK-Eintraege != Liste"
fehl = 0
for p, g, s in eintraege:
    d = z.read("assets/" + p)
    q = os.path.join(repo, "re15_port", p) if p.startswith("shared_assets/") else os.path.join(repo, p)
    qd = open(q, "rb").read()
    if not (len(d) == g == len(qd) and hashlib.sha256(d).hexdigest() == s == hashlib.sha256(qd).hexdigest()):
        fehl += 1
        print("ABWEICHUNG", p)
print("Eintraege %d, Bytes %d, Abweichungen APK/Quelle/Liste: %d" % (len(eintraege), b_soll, fehl))
print("LISTE-OK" if fehl == 0 else "LISTE-FEHLER")
