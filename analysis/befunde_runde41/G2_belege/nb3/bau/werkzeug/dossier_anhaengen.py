# -*- coding: utf-8 -*-
"""NB3 Bauer: Dossier-Abschnitt (dossier_nb3.md mit ersetzten Platzhaltern aus dossier_werte.json) an G2_gatorfix.md anhaengen (CRLF wie die Datei)."""
import io, json, sys
B = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/g2nb3/bau"
D = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/analysis/befunde_runde41/G2_gatorfix.md"
t = io.open(B + "/dossier_nb3.md", encoding="utf-8").read()
w = json.load(io.open(B + "/dossier_werte.json", encoding="utf-8"))
for k, v in w.items():
    if "__%s__" % k not in t:
        print("Platzhalter fehlt:", k); sys.exit(1)
    t = t.replace("__%s__" % k, v)
if "__" in t.replace("__init__", ""):
    import re
    rest = re.findall(r"__[A-Z0-9]+__", t)
    if rest: print("offene Platzhalter:", rest); sys.exit(1)
s = io.open(D, encoding="utf-8", newline="").read()
crlf = "\r\n" in s
if "## G2-RE2 NB3 (Rampe oben, Plattform-Suedkante)" in s:
    print("Abschnitt schon vorhanden"); sys.exit(1)
if not s.endswith("\r\n") and not s.endswith("\n"):
    s += "\r\n" if crlf else "\n"
t = t.replace("\r\n", "\n")
if crlf: t = t.replace("\n", "\r\n")
io.open(D, "w", encoding="utf-8", newline="").write(s + t)
print("angehaengt, crlf=%d, Zeilen=%d" % (crlf, (s + t).count("\r\n" if crlf else "\n")))
