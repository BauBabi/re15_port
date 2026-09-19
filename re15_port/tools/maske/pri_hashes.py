"""Hash-Riegel fuer die Freistellungen des Nutzers: erzeugt tests/unit/pri_stage1_hashes.inc.

Phase 2 (2026-09-19), Punkt 1 + Gegenpruefung Punkt 1: NICHT "erste Git-Fassung" (der Nutzer
liefert mehrfach, z.B. 1130/03.png dreimal), sondern der JETZT festgelegte Stand je Datei —
Git-Blob-Hash (sha1 ueber "blob <n>\\0" + Inhalt) und der aelteste Commit, der genau diesen Blob
enthaelt (git log --find-object). test_pri_hashes.c rechnet den Hash der Datei im Baum nach
und verlangt Gleichheit fuer JEDE Datei der Liste — und dass keine Datei unter pri/STAGE1
fehlt oder hinzukommt, ohne dass die Liste (bewusst) neu erzeugt wird.

Aufruf (Repo-Wurzel): python re15_port/tools/maske/pri_hashes.py
"""
import glob
import hashlib
import os
import subprocess
import sys

WURZEL = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
ZIEL = os.path.join(WURZEL, "re15_port", "tests", "unit", "pri_stage1_hashes.inc")


def blob_sha1(pfad):
    b = open(pfad, "rb").read()
    h = hashlib.sha1()
    h.update(b"blob %d\0" % len(b))
    h.update(b)
    return h.hexdigest()


# Zurueckgeholte Nutzer-Originale (Phase 2): der Blob stammt aus einem ANDEREN Pfad.
HERKUNFT_FEST = {
    "pri/STAGE1/10D0/06.png": "61607499:pri/10D0/06.png",
    "pri/STAGE1/10D0/01.png": "61607499:pri/10D0/01.png",
    "pri/STAGE1/10E0/07_01.png": "61607499:pri/10E0/07_01.png",
}


def herkunft(rel, sha):
    """Aeltester Commit im Verlauf DIESES Pfads (Umbenennungen verfolgt), dessen Blob gleich
    dem heutigen ist — also der Commit, der die heutige Fassung eingefuehrt hat."""
    if rel in HERKUNFT_FEST:
        return HERKUNFT_FEST[rel]
    try:
        r = subprocess.run(["git", "log", "--follow", "--format=@%h", "--name-status", "--", rel],
                           cwd=WURZEL, capture_output=True, text=True, timeout=120)
    except Exception:
        return "?"
    schritte = []          # (commit, pfad zu diesem commit), neu -> alt
    cur = None
    for ln in r.stdout.splitlines():
        if ln.startswith("@"):
            cur = ln[1:]
        elif ln.strip() and cur:
            teile = ln.split("\t")
            schritte.append((cur, teile[-1]))
    aeltester = "neu"
    for c, pfad in schritte:
        try:
            b = subprocess.run(["git", "rev-parse", "%s:%s" % (c, pfad)], cwd=WURZEL,
                               capture_output=True, text=True, timeout=60).stdout.strip()
        except Exception:
            break
        if b != sha:
            break
        aeltester = c
    return aeltester


def main():
    dateien = sorted(p.replace("\\", "/") for p in glob.glob(os.path.join(WURZEL, "pri", "STAGE1", "*", "*.png")))
    zeilen = []
    for p in dateien:
        rel = os.path.relpath(p, WURZEL).replace("\\", "/")
        sha = blob_sha1(p)
        zeilen.append((rel, sha, herkunft(rel, sha)))
        print("%-32s %s %s" % (rel, sha, zeilen[-1][2]))
    with open(ZIEL, "w", encoding="utf-8") as f:
        f.write("/* ERZEUGT von re15_port/tools/maske/pri_hashes.py (%d Dateien) — nicht von Hand aendern.\n"
                " * Jede Zeile: Pfad (Repo-Wurzel), Git-Blob-SHA1, aeltester Commit mit genau diesem Blob. */\n" % len(zeilen))
        for rel, sha, h in zeilen:
            f.write('    { "%s", "%s", "%s" },\n' % (rel, sha, h))
    print("-> %s" % ZIEL)
    return 0


if __name__ == "__main__":
    sys.exit(main())
