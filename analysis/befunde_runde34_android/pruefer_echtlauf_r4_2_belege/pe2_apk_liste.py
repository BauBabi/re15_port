# Pruefer echtlauf R4-2: Liste v2 einer APK UNABHAENGIG vom Gate pruefen (Python zipfile, eigener Code).
# Aufruf: /c/Python310/python.exe pe2_apk_liste.py <apk> <repo> [<liste_aus.txt>]
# - Kopf "# re15 assets v2 <n> <bytes>", n Zeilen "<bytes>\t<64 hex klein>\t<pfad>", Summe, sortiert, keine
#   (auch Gross/klein-)Dubletten, Pfad nur druckbares ASCII ohne '\', Segmente 1..251 B
# - jede Zeile: Eintrag assets/<pfad> in der APK, STORED, Groesse + sha256 der Daten = Zeile
# - assets/-Eintraege der APK = Liste + re15_assets.txt (nichts sonst)
# - Quellbaum: re15_port/shared_assets/{PSX,extracted_fx,RE2,RE15DOOR} + synchro/STAGE* = genau die Liste,
#   Groesse + sha256 je Datei gleich; Tuerarchive RE15DOOR/*.DO2 und RE2/DOOR/*.DO2 gezaehlt
import hashlib, os, re, sys, zipfile

apk, repo = sys.argv[1], sys.argv[2]
aus = sys.argv[3] if len(sys.argv) > 3 else None
fehler = []
z = zipfile.ZipFile(apk)
raw = z.read("assets/re15_assets.txt")
if aus:
    open(aus, "wb").write(raw)
text = raw.decode("ascii")
if not text.endswith("\n"):
    fehler.append("Liste endet nicht mit LF")
zeilen = text.split("\n")[:-1]
kopf = zeilen[0]
m = re.fullmatch(r"# re15 assets v2 (\d+) (\d+)", kopf)
if not m:
    print("KOPF UNERWARTET: %r" % kopf)
    sys.exit(1)
n_soll, b_soll = int(m.group(1)), int(m.group(2))
eintraege = []
for i, zl in enumerate(zeilen[1:], 2):
    t = zl.split("\t")
    if len(t) != 3 or not re.fullmatch(r"[0-9]+", t[0]) or not re.fullmatch(r"[0-9a-f]{64}", t[1]):
        fehler.append("Zeile %d unlesbar: %r" % (i, zl[:80]))
        continue
    p = t[2]
    if any(ord(c) < 0x20 or ord(c) > 0x7e or c == "\\" for c in p) or \
       any(len(s) == 0 or len(s) > 251 or s in (".", "..") for s in p.split("/")):
        fehler.append("Zeile %d: Pfad verletzt die Regel: %r" % (i, p))
    eintraege.append((p, int(t[0]), t[1]))
pfade = [e[0] for e in eintraege]
if len(eintraege) != n_soll:
    fehler.append("Kopf nennt %d Zeilen, gelesen %d" % (n_soll, len(eintraege)))
if sum(e[1] for e in eintraege) != b_soll:
    fehler.append("Kopf nennt %d Bytes, Summe %d" % (b_soll, sum(e[1] for e in eintraege)))
if pfade != sorted(pfade):
    fehler.append("Liste nicht sortiert")
if len(set(p.lower() for p in pfade)) != len(pfade):
    fehler.append("Gross/klein-Dubletten in der Liste")

# APK-Seite
infos = {i.filename: i for i in z.infolist()}
asset_eintraege = sorted(n[len("assets/"):] for n in infos if n.startswith("assets/") and not n.endswith("/"))
soll_assets = sorted(pfade + ["re15_assets.txt"])
if asset_eintraege != soll_assets:
    fehler.append("assets/-Eintraege != Liste: nur APK %s, nur Liste %s"
                  % (sorted(set(asset_eintraege) - set(soll_assets))[:5], sorted(set(soll_assets) - set(asset_eintraege))[:5]))
n_apk_ok = 0
for p, g, s in eintraege:
    info = infos.get("assets/" + p)
    if info is None:
        continue
    if info.compress_type != zipfile.ZIP_STORED:
        fehler.append("APK: %s nicht STORED (compress_type %d)" % (p, info.compress_type))
    h = hashlib.sha256()
    n = 0
    with z.open(info) as f:
        while True:
            b = f.read(1 << 20)
            if not b:
                break
            h.update(b)
            n += len(b)
    if n != g or h.hexdigest() != s:
        fehler.append("APK: %s %d B sha256 %s.. != Liste %d B %s.." % (p, n, h.hexdigest()[:16], g, s[:16]))
    else:
        n_apk_ok += 1

# Quellbaum-Seite
def quelle(p):
    if p.startswith("shared_assets/"):
        return os.path.join(repo, "re15_port", p)
    if p.startswith("synchro/"):
        return os.path.join(repo, p)
    return None
quell_ist = set()
for basis, rel0 in ((os.path.join(repo, "re15_port", "shared_assets"), "shared_assets"),):
    for baum in ("PSX", "extracted_fx", "RE2", "RE15DOOR"):
        for wurzel, _d, dateien in os.walk(os.path.join(basis, baum)):
            for d in dateien:
                quell_ist.add(os.path.relpath(os.path.join(wurzel, d), os.path.join(repo, "re15_port")).replace("\\", "/"))
for st in sorted(os.listdir(os.path.join(repo, "synchro"))):
    if st.startswith("STAGE") and os.path.isdir(os.path.join(repo, "synchro", st)):
        for wurzel, _d, dateien in os.walk(os.path.join(repo, "synchro", st)):
            for d in dateien:
                quell_ist.add(os.path.relpath(os.path.join(wurzel, d), repo).replace("\\", "/"))
if quell_ist != set(pfade):
    fehler.append("Quellbaum != Liste: nur Quelle %s, nur Liste %s"
                  % (sorted(quell_ist - set(pfade))[:5], sorted(set(pfade) - quell_ist)[:5]))
n_quelle_ok = 0
for p, g, s in eintraege:
    q = quelle(p)
    if not q or not os.path.isfile(q):
        continue
    h = hashlib.sha256(open(q, "rb").read()).hexdigest()
    if os.path.getsize(q) != g or h != s:
        fehler.append("Quelle: %s %d B %s.. != Liste %d B %s.." % (p, os.path.getsize(q), h[:16], g, s[:16]))
    else:
        n_quelle_ok += 1
t15 = sum(1 for p in pfade if re.fullmatch(r"shared_assets/RE15DOOR/[^/]+\.DO2", p))
t2 = sum(1 for p in pfade if re.fullmatch(r"shared_assets/RE2/DOOR/[^/]+\.DO2", p))
print("Liste: %s | %d Zeilen | Liste %d B sha256 %s" % (kopf, len(eintraege), len(raw), hashlib.sha256(raw).hexdigest()))
print("APK: %d/%d Eintraege STORED mit Groesse+sha256 = Liste; assets/-Eintraege %d (= Liste + re15_assets.txt: %s)"
      % (n_apk_ok, len(eintraege), len(asset_eintraege), asset_eintraege == soll_assets))
print("Quellbaum: %d Dateien in den 5 Baeumen, %d/%d mit Groesse+sha256 = Liste" % (len(quell_ist), n_quelle_ok, len(eintraege)))
print("Tuerarchive in der Liste: RE15DOOR %d x *.DO2, RE2/DOOR %d x *.DO2" % (t15, t2))
for f in fehler[:40]:
    print("  ABWEICHUNG:", f)
print("LISTE-OK" if not fehler else "LISTE-ABWEICHUNG: %d" % len(fehler))
sys.exit(0 if not fehler else 1)
