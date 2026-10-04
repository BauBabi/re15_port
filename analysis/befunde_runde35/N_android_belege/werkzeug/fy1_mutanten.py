# Runde 35 Spur N: die fuenf Ueberlebenden der Gegenpruefung R4-2 (F-Y1, pruefer_umgehung_r4_2.md Abschnitt 1) und je ein
# R1/R2-Mutant gegen das NEUE Gate. Je Mutant: Kopie von release/apk_asset_gate.py mit genau einer Aenderung, dann
# --selbsttest (Schnellmodus). Erkannt = Rueckgabe != 0. Aufruf aus der Repo-Wurzel: python <dies> [gate]
import os
import shutil
import subprocess
import sys
import tempfile

gate = sys.argv[1] if len(sys.argv) > 1 else "release/apk_asset_gate.py"
src = open(gate, encoding="utf-8", newline="").read()
M = [
    ("E_manifest_pruefen_gt", "elif a_sha is not None and a_sha != m_sha:", "elif a_sha is not None and a_sha > m_sha:"),
    ("D06_ASCII_KLEIN_ohne_Z", 'bytes.maketrans(b"ABCDEFGHIJKLMNOPQRSTUVWXYZ", b"abcdefghijklmnopqrstuvwxyz")',
     'bytes.maketrans(b"ABCDEFGHIJKLMNOPQRSTUVWXY", b"abcdefghijklmnopqrstuvwxy")'),
    ("D08_KOPF_RE_plus_anzahl", 'KOPF_RE = re.compile(rb"# re15 assets v2 ([0-9]{1,18}) ([0-9]{1,18})")',
     'KOPF_RE = re.compile(rb"# re15 assets v2 ([0-9]+) ([0-9]{1,18})")'),
    ("D13_kopf_strip_cr", 'kopf = zeilen[0].rstrip(b"\\r")', 'kopf = zeilen[0].strip(b"\\r")'),
    ("D15_zeile_strip_cr", '        z = z.rstrip(b"\\r")\n        if not z:', '        z = z.strip(b"\\r")\n        if not z:'),
    ("R1_nur_ganzer_pfad", "    if any(t[-len(NEU_ENDUNG):].translate(_ASCII_KLEIN) == NEU_ENDUNG for t in teile):",
     "    if False:"),
    ("R2_weg", '            if vor:\n                fehler.append("Manifest: Datei und Ordner',
     '            if False:\n                fehler.append("Manifest: Datei und Ordner'),
]
tmp = tempfile.mkdtemp(prefix="r35n_mut_")
env = dict(os.environ, RE15_GATE_SELBSTTEST_SCHNELL="1")
erkannt = 0
for name, a, b in M:
    n = src.count(a)
    if n != 1:
        print("%-26s NICHT ANWENDBAR (%d Treffer)" % (name, n))
        continue
    p = os.path.join(tmp, name + ".py")
    open(p, "w", encoding="utf-8", newline="").write(src.replace(a, b))
    r = subprocess.run([sys.executable, p, "--selbsttest"], capture_output=True, text=True, env=env)
    letzte = [z for z in r.stdout.splitlines() if z.strip()][-1:] or ["(keine Ausgabe)"]
    fehl = [z.strip()[:110] for z in r.stdout.splitlines() if "[FEHLER]" in z][:2]
    erkannt += r.returncode != 0
    print("%-26s rc=%d  %s  %s" % (name, r.returncode, "ERKANNT" if r.returncode != 0 else "UEBERLEBT", letzte[0][:90]))
    for z in fehl:
        print("      " + z)
shutil.rmtree(tmp, ignore_errors=True)
print("%d von %d Mutanten erkannt" % (erkannt, len(M)))
sys.exit(0 if erkannt == len(M) else 1)
