#!/usr/bin/env python3
"""r30_tuer_pincode_zensus.py - ZUSATZ zur Spur tuer-verschlossen: hat RE1.5 PINCODE-TUEREN,
und steht deren VERSCHLOSSEN-Meldung (nicht die Falscheingabe am Feld) in der Tabelle
engine/src/gen/lock_se_sites.inc als Art K?

Verfahren: je ausgelieferter RDT (shared_assets/PSX/STAGE*/ROOM*.RDT) alle Nachrichten, deren
Wortlaut auf ein Ziffern-/Code-Feld zeigt (digit, code, keypanel, key pad, password, number).
Fuer jeden solchen Raum werden ALLE Nachrichten des Raums ausgegeben, die das Wort lock/latched
tragen, zusammen mit dem Tabellen-Eintrag (Art/Wege) oder "NICHT IN TABELLE" + Datei-Offset.
Ausgabe: stdout.
"""
import glob, os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L

INC = os.path.join(REPO, "re15_port", "engine", "src", "gen", "lock_se_sites.inc")
tab = {}
for m in re.finditer(r"\{ 0x([0-9A-F]{4}),\s*(\d+), RE15_LOCK_ART_([KM]), (\d) \}", open(INC).read()):
    tab[(int(m.group(1), 16), int(m.group(2)))] = (m.group(3), int(m.group(4)))

CODE = re.compile(r"digit|\bcode\b|keypanel|key ?pad|password|numerical|number", re.I)
LOCK = re.compile(r"lock|latched", re.I)

def norm(t):
    return re.sub(r"\s+", " ", t.replace(" / ", " ")).strip()

files = sorted(glob.glob(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT")))
n_code_rooms = 0
for f in files:
    room = int(os.path.basename(f)[4:8], 16)
    d = open(f, "rb").read()
    if len(d) < 0x100:
        continue
    ms = L.messages(d)
    code = [(mi, t) for mi, (t, c) in sorted(ms.items()) if CODE.search(t)]
    if not code:
        continue
    n_code_rooms += 1
    mstart = L.u32(d, 0x3C)
    print("ROOM%04X  Code-/Ziffern-Wortlaut:" % room)
    for mi, t in code:
        print("    code  msg %2d @0x%05X  \"%s\"" % (mi, mstart + L.u16(d, mstart + 2 * mi), norm(t)[:110]))
    for mi, (t, c) in sorted(ms.items()):
        if not LOCK.search(t):
            continue
        e = tab.get((room, mi))
        st = ("TABELLE Art %s wege %d" % e) if e else "NICHT IN TABELLE"
        print("    lock  msg %2d @0x%05X  %-24s \"%s\"" % (mi, mstart + L.u16(d, mstart + 2 * mi), st, norm(t)[:110]))
print("Raeume mit Code-/Ziffern-Wortlaut: %d" % n_code_rooms)
