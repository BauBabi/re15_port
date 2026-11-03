# Abnahme 3: RE2-Einzelbilder an Szenenbildern n (Anker: eigene Standbild-Messung re2_kanten3.py, Bild 0 = 63,47 s wie Abnahme 2)
import subprocess, sys
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
FPS = 29.9
def re2_t(n):
    if n <= 132: return 63.47 + n / FPS
    if n <= 437: return 68.261 + (n - 133) / FPS
    if n <= 807: return 79.649 + (n - 438) / FPS
    return 92.776 + (n - 807) / FPS
src = sys.argv[1]
for a in sys.argv[2:]:
    n = int(a); t = re2_t(n) + 0.5 / FPS
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % t, "-i", src, "-frames:v", "1", "re2bilder/re2_%+05d.png" % n], check=True)
    print(n, "%.3f" % t)
