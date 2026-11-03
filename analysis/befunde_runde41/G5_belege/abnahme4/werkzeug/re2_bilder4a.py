# Abnahme 4: RE2-Einzelbilder an Szenenbildern n -> re2bilder/re2_<n>.png
import subprocess, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2zeit4 import re2_t, FPS
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
src = sys.argv[1]; os.makedirs("re2bilder", exist_ok=True)
for a in sys.argv[2:]:
    for n in ([int(x) for x in a.split(",")]):
        p = "re2bilder/re2_%+05d.png" % n
        if os.path.exists(p): continue
        subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % (re2_t(n) + 0.5 / FPS), "-i", src, "-frames:v", "1", p], check=True)
