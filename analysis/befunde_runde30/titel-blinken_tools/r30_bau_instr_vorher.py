#!/usr/bin/env python3
"""r30_bau_instr_vorher.py - Runde 30 / Thema D, Bau-Agent.
Baut die INSTRUMENTIERTE KOPIE des AUSGANGSZUSTANDS (render_pc.c von master 437905cb, der
Pulsschritt sitzt dort noch im Zeichner) gegen die uebrigen Objekte eines Bauverzeichnisses.
Wie r30_build_instr.sh, aber ohne feste Pfade: Uebersetzungs- und Link-Befehl kommen aus
ninja -t commands.
⚠ Das Bauverzeichnis muss SELBST auf dem Ausgangsstand gebaut sein (main.c.obj von 437905cb):
  gegen ein Bauverzeichnis nach dem Umbau gelinkt, misst die Kopie einen Mischstand.
Aufruf: r30_bau_instr_vorher.py <render_pc.c-Quelle> <ausgabe-verzeichnis> <bauverzeichnis>
        (die Quelle z.B. per  git show 437905cb:re15_port/platform/pc/src/render_pc.c)
Lauf:   R30_PULSE_LOG=<datei> <ausgabe>/re15_pc_r30instr.exe ; Auswertung r30_pulse_log_stats.py
"""
import os, subprocess, sys, shlex

src_path = sys.argv[1]
out = os.path.abspath(sys.argv[2])
BLD = os.path.abspath(sys.argv[3])
os.makedirs(out, exist_ok=True)
env = dict(os.environ)
env["PATH"] = r"C:\msys64\mingw64\bin;C:\msys64\usr\bin;" + env["PATH"]

src = open(src_path, encoding="utf-8", errors="surrogateescape").read()
anchor = "    if (++s_tmoji_pulse_ctr >= 0x3c) { s_tmoji_pulse_ctr = 0; s_tmoji_pulse_val = 0x80; }\n"
assert src.count(anchor) == 1, "Anker nicht eindeutig: %d" % src.count(anchor)
probe = anchor + (
    "    /* R30-MESSKOPIE: nie im Quellbaum. */\n"
    "    { static FILE *r30_lf; static int r30_init;\n"
    "      if (!r30_init) { r30_init = 1; const char *p = getenv(\"R30_PULSE_LOG\"); if (p && *p) r30_lf = fopen(p, \"w\"); }\n"
    "      if (r30_lf) { unsigned long long c = (unsigned long long) SDL_GetPerformanceCounter();\n"
    "                    unsigned long long f = (unsigned long long) SDL_GetPerformanceFrequency();\n"
    "                    fprintf(r30_lf, \"%llu %d %d\\n\", (c * 1000000ull) / f, s_tmoji_pulse_ctr, s_tmoji_pulse_val);\n"
    "                    fflush(r30_lf); } }\n")
cpy = out + "/render_pc_r30instr.c"
open(cpy, "w", encoding="utf-8", errors="surrogateescape", newline="").write(src.replace(anchor, probe))

cmds = subprocess.run(["ninja", "-C", BLD, "-t", "commands", "platform/pc/re15_pc.exe"],
                      capture_output=True, text=True, env=env).stdout.splitlines()
cc = [c for c in cmds if ("/render_pc.c.obj" in c or "\\render_pc.c.obj" in c) and " -c " in c]
ln = [c for c in cmds if "re15_pc.exe" in c and " -c " not in c and "-o platform" in c]
assert len(cc) == 1 and len(ln) >= 1, (len(cc), len(ln))
cc = cc[0]; ln = ln[-1]
obj = out + "/render_pc_r30instr.obj"
# Uebersetzen: Quelle und Objekt austauschen, Abhaengigkeitsdatei weglassen
parts = cc.split(" ")
res = []
skip = 0
for i, p in enumerate(parts):
    if skip: skip -= 1; continue
    if p in ("-MD", "-MMD"): continue
    if p in ("-MT", "-MF"): skip = 1; continue
    res.append(p)
cc2 = " ".join(res)
i_o = cc2.rfind(" -o ")
cc2 = cc2[:i_o] + ' -o "%s" -c "%s"' % (obj, cpy)
print("CC:", cc2[:200], "...")
r = subprocess.run(cc2, shell=True, cwd=BLD, env=env)
assert r.returncode == 0, "Uebersetzen fehlgeschlagen"
ln2 = ln.replace("platform/pc/CMakeFiles/re15_pc.dir/src/render_pc.c.obj", '"%s"' % obj)
assert ln2 != ln
exe = out + "/re15_pc_r30instr.exe"
ln2 = ln2.replace("-o platform\\pc\\re15_pc.exe", '-o "%s"' % exe).replace("-o platform/pc/re15_pc.exe", '-o "%s"' % exe)
# cmake haengt an die Link-Zeile ggf. "&& ..." an; nur den ersten Befehl fahren
if ln2.startswith("cmd.exe /C \""):
    ln2 = ln2[len("cmd.exe /C \""):]
    ln2 = ln2.split(" && ")
    ln2 = [x for x in ln2 if " -o " in x][0]
print("LD:", ln2[:200], "...")
r = subprocess.run(ln2, shell=True, cwd=BLD, env=env)
assert r.returncode == 0, "Linken fehlgeschlagen"
print("fertig:", exe, os.path.getsize(exe))
