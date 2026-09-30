# Pruefer echtlauf R4-2: Geraetezustand gegen die Liste einer APK (eigener Code).
# Aufruf: /c/Python310/python.exe pe2_geraet.py <apk> <stat_aus.txt|-> [--roh]
#   sha256 JEDER Datei unter shared_assets/ und synchro/ des Speicherordners (toybox sha256sum auf dem Geraet) gegen
#   assets/re15_assets.txt der APK; zusaetzliche Dateien; *.neu UEBERALL im Speicherordner; leere Ordner in den Baeumen;
#   oberste Ebene; Liste "zuletzt entpackt" (sha256 == Liste der APK?); alter Marker re15_assets_ok.txt.
#   stat_aus: je Datei "inode|groesse|mtime|pfad" (fuer "genau diese Datei neu geschrieben").
#   --roh: nur Bestand zeigen (Zustand nach einem Abbruch), Urteil trotzdem gegen die Liste.
# Letzte Zeile: GERAET-KONSISTENT oder GERAET-ABWEICHUNG.
import hashlib, subprocess, sys, zipfile
ADB = [r"C:/Users/mjoedicke/AppData/Local/Android/Sdk/platform-tools/adb.exe", "-s", "emulator-5586"]
R = "/sdcard/Android/data/de.re15.port/files"


def sh(cmd):
    r = subprocess.run(ADB + ["shell", cmd], capture_output=True)
    return r.stdout.decode("utf-8", "replace").replace("\r", "")


apk, stat_aus = sys.argv[1], sys.argv[2]
raw = zipfile.ZipFile(apk).read("assets/re15_assets.txt")
kopf, *zeilen = raw.decode("ascii").split("\n")[:-1]
soll = {}
for zl in zeilen:
    g, s, p = zl.split("\t")
    soll[p] = (int(g), s)
liste_sha = hashlib.sha256(raw).hexdigest()
print("APK-Liste: %s | %d Eintraege | sha256 %s" % (kopf, len(soll), liste_sha))
oben = [x for x in sh("ls -a %s 2>&1" % R).split() if x not in (".", "..")]
print("oberste Ebene:", " ".join(oben))
alle = [x[2:] if x.startswith("./") else x for x in sh("cd %s && find . -type f" % R).split("\n") if x]
neu_reste = sorted(x for x in alle if x.lower().endswith(".neu"))
ist = {}
for zl in sh("cd %s && find shared_assets synchro -type f -exec sha256sum {} + 2>/dev/null" % R).split("\n"):
    if zl.strip():
        h, p = zl.split(None, 1)
        ist[p.strip()] = h
leer = [x for x in sh("cd %s && find shared_assets synchro -type d -empty 2>/dev/null" % R).split("\n") if x]
if stat_aus != "-":
    st = sh("cd %s && find shared_assets synchro -type f -exec stat -c '%%i|%%s|%%y|%%n' {} + 2>/dev/null" % R)
    open(stat_aus, "w", newline="\n").write("".join(sorted(l + "\n" for l in st.split("\n") if l)))
fehlt = sorted(p for p in soll if p not in ist)
falsch = sorted(p for p in soll if p in ist and ist[p] != soll[p][1])
zusatz = sorted(p for p in ist if p not in soll)
if "re15_assets_entpackt.txt" in oben:
    liste_geraet = sh("sha256sum %s/re15_assets_entpackt.txt" % R).split()[0]
else:
    liste_geraet = "-"
marker = "re15_assets_ok.txt" in oben
print("Dateien in den Asset-Baeumen: %d | fehlen %d | sha256 falsch %d | zusaetzlich %d | .neu (ueberall) %d | leere Ordner %d"
      % (len(ist), len(fehlt), len(falsch), len(zusatz), len(neu_reste), len(leer)))
for n, l in (("fehlt", fehlt), ("falsch", falsch), ("zusaetzlich", zusatz), (".neu", neu_reste), ("leer", leer)):
    for p in l[:25]:
        extra = ""
        if n in (".neu", "zusaetzlich"):
            extra = " (" + sh("stat -c %%s '%s/%s'" % (R, p)).strip() + " B)"
        print("  %s: %s%s" % (n, p, extra))
    if len(l) > 25:
        print("  %s: ... (%d weitere)" % (n, len(l) - 25))
print("zuletzt entpackt: %s (%s) | alter Marker re15_assets_ok.txt: %s"
      % (liste_geraet, "= Liste der APK" if liste_geraet == liste_sha else "NICHT die Liste der APK", "da" if marker else "weg"))
ok = not (fehlt or falsch or zusatz or neu_reste or leer) and liste_geraet == liste_sha and not marker
print("GERAET-KONSISTENT" if ok else "GERAET-ABWEICHUNG")
