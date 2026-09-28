#!/usr/bin/env python3
"""r30_karte3010_welle_suche.py - liegt die RE2-Hinweis-Welle irgendwo in RE1.5?

Sucht die ersten 64 Nutz-Bytes (ab Block 1, der Block 0 ist der Null-Kopf) der Welle
ROOM3010.RDT VAG 18 @0x39788 (4480 B, sha1 eb386970...) in JEDER Datei unter
re15_port/shared_assets/PSX (RDT-Raumbaenke, SOUND/*.VB, ...). Treffer werden auf
volle Laenge (4480 B) nachgeprueft.
"""
import os, sys, hashlib
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
src = open(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM3010.RDT"), "rb").read()
W = src[0x39788:0x39788 + 4480]
assert hashlib.sha1(W).hexdigest() == "eb386970f9a996369889d101b68314681138ff99"
key = W[16:80]
root = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
nfiles = 0; nbytes = 0; hits = []
for dp, dn, fn in os.walk(root):
    for f in fn:
        p = os.path.join(dp, f)
        try: d = open(p, "rb").read()
        except OSError: continue
        nfiles += 1; nbytes += len(d)
        o = d.find(key)
        while o >= 0:
            voll = d[o - 16:o - 16 + 4480] == W
            hits.append((os.path.relpath(p, root), o - 16, voll))
            o = d.find(key, o + 1)
print("durchsucht: %d Dateien, %d Byte" % (nfiles, nbytes))
for h in hits: print("  TREFFER %s @0x%X  volle Welle: %s" % h)
print("Treffer: %d" % len(hits))
