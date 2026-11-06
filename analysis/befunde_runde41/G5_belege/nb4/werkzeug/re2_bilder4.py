# Nachbesserung 4 G5A: RE2-Einzelbilder an Szenenbildern n (Zeitachse der Abnahme 3, re2_kanten3.py: Bild 0 = 63,47 s)
import subprocess, sys, os
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"; FPS = 29.9
def re2_t(n):
    if n <= 132: return 63.47 + n / FPS
    if n <= 437: return 68.261 + (n - 133) / FPS
    if n <= 807: return 79.649 + (n - 438) / FPS
    return 92.776 + (n - 807) / FPS
src, ziel = sys.argv[1], sys.argv[2]
for a in sys.argv[3:]:
    n = int(a); p = os.path.join(ziel, "re2_%+05d.png" % n)
    if not os.path.exists(p):
        subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % (re2_t(n) + 0.5 / FPS), "-i", src, "-frames:v", "1", p], check=True)
