# Nachbesserung R4-1: Geraetezustand gegen die Liste einer APK (nach dem Vorbild pe1_geraet.py, eigene Instanz 5584).
# Aufruf: python nb_geraet.py <apk>
# sha256 JEDER Datei unter shared_assets/ und synchro/ im Speicherordner (toybox sha256sum) gegen assets/re15_assets.txt
# der APK; zusaetzliche Dateien (= Waisen), *.neu ueberall, oberste Ebene, Liste "zuletzt entpackt", alter Marker.
# Ausgabe endet mit GERAET-KONSISTENT oder GERAET-ABWEICHUNG.
import hashlib, subprocess, sys, zipfile
ADB = [r"C:/Users/mjoedicke/AppData/Local/Android/Sdk/platform-tools/adb.exe", "-s", "emulator-5584"]
R = "/sdcard/Android/data/de.re15.port/files"


def sh(cmd):
    r = subprocess.run(ADB + ["shell", cmd], capture_output=True)
    return r.stdout.decode("utf-8", "replace").replace("\r", "")


apk = sys.argv[1]
raw = zipfile.ZipFile(apk).read("assets/re15_assets.txt")
kopf, *zeilen = raw.decode("utf-8").split("\n")[:-1]
soll = {}
for zl in zeilen:
    g, s, p = zl.split("\t")
    soll[p] = (int(g), s)
print("APK-Liste: %s | %d Eintraege | sha256 %s" % (kopf, len(soll), hashlib.sha256(raw).hexdigest()))
oben = sh("ls -a %s" % R).split()
print("oberste Ebene:", " ".join(x for x in oben if x not in (".", "..")))
alle = [x for x in sh("cd %s && find . -type f" % R).split("\n") if x]
alle = [x[2:] if x.startswith("./") else x for x in alle]
neu_reste = [x for x in alle if x.lower().endswith(".neu")]
ist = {}
for zl in sh("cd %s && find shared_assets synchro -type f -exec sha256sum {} + 2>/dev/null" % R).split("\n"):
    if not zl.strip():
        continue
    h, p = zl.split(None, 1)
    ist[p.strip()] = h
leere = [x for x in sh("cd %s && find shared_assets synchro -type d -empty 2>/dev/null" % R).split("\n") if x.strip()]
fehlt = sorted(p for p in soll if p not in ist)
falsch = sorted(p for p in soll if p in ist and ist[p] != soll[p][1])
zusatz = sorted(p for p in ist if p not in soll)
liste = sh("cat %s/re15_assets_entpackt.txt 2>/dev/null | sha256sum" % R).split()[0] if "re15_assets_entpackt.txt" in oben else "-"
marker = "re15_assets_ok.txt" in oben
print("Dateien in den Asset-Baeumen: %d | fehlen %d | sha256 falsch %d | zusaetzlich (Waisen) %d | .neu-Reste %d | leere Ordner %d"
      % (len(ist), len(fehlt), len(falsch), len(zusatz), len(neu_reste), len(leere)))
for n, l in (("fehlt", fehlt), ("falsch", falsch), ("zusaetzlich", zusatz), (".neu", neu_reste), ("leerer Ordner", leere)):
    for p in l[:20]:
        print("  %s: %s" % (n, p))
print("re15_assets_entpackt.txt sha256: %s (%s)" % (liste, "= APK-Liste" if liste == hashlib.sha256(raw).hexdigest() else "NICHT die APK-Liste"))
print("alter Marker re15_assets_ok.txt vorhanden:", marker)
ok = not fehlt and not falsch and not zusatz and not neu_reste and liste == hashlib.sha256(raw).hexdigest() and not marker
print("GERAET-KONSISTENT" if ok else "GERAET-ABWEICHUNG")
