# -*- coding: utf-8 -*-
"""Nachbesserung R1: einzelne Selbsttest-Faelle des Gates aufbauen und die VOLLE Gate-Ausgabe zeigen
(der Selbsttest selbst zeigt nur ok/FEHLER). Aufruf: fall_zeigen.py <gate.py> <arbeitsordner> <nr> [<nr> ...]
Zusaetzlich: roher Eintragsname des gefaelschten Eintrags (eigener Mini-Parser des Gates, _fx_cd)."""
import importlib.util, os, shutil, subprocess, sys
gate = os.path.abspath(sys.argv[1]); wurzel = os.path.abspath(sys.argv[2])
spec = importlib.util.spec_from_file_location("gate", gate); g = importlib.util.module_from_spec(spec); spec.loader.exec_module(g)
faelle = g._faelle()
if os.path.isdir(wurzel): shutil.rmtree(wurzel)
os.makedirs(wurzel)
for nr in [int(x) for x in sys.argv[3:]]:
    f = g._fall_vorbereiten(wurzel, nr, faelle[nr - 1])
    rc, aus = g._fall_laufen(f)
    print("=" * 100); print("Fall %02d: %s (soll %d) -> rc %d" % (nr, faelle[nr - 1][0], faelle[nr - 1][2], rc))
    if os.path.isfile(f.apk):
        cd, cd_off, eocd = g._fx_cd(g._fx_lesen(f.apk))
        for (p, lho, nlen, xlen, klen, csize, name) in cd:
            if b"P07" in name or b"TEX" in name or b"classes" in name or name.endswith(b"/"):
                d = g._fx_lesen(f.apk); l_nlen, l_xlen = __import__("struct").unpack("<HH", d[lho + 26:lho + 30])
                print("   roh: %-52r lho=%-6d cd_xlen=%d lfh_xlen=%d lfh_name=%r" % (name, lho, xlen, l_xlen, d[lho + 30:lho + 30 + l_nlen]))
    print(aus.rstrip())
