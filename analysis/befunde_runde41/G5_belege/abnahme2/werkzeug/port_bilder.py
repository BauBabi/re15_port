# Abnahme 2: Port-Einzelbilder an Szenenbildern n (Spielbild F_arm + n) aus dem Fensterlauf, Zeit t = a + b*F (port_zeit.py)
import subprocess, json, sys, os
FF = r"C:/ProgramData/chocolatey/bin/ffmpeg"
d = sys.argv[1]; ns = [int(x) for x in sys.argv[2:]]
z = json.load(open(os.path.join(d, "zeit.json")))
os.makedirs(os.path.join(d, "bilder"), exist_ok=True)
for n in ns:
    F = z["F_arm"] + n
    t = z["a"] + z["b"] * F + 0.5 * z["b"]       # Mitte des Spielbilds
    subprocess.run([FF, "-hide_banner", "-loglevel", "error", "-y", "-ss", "%.3f" % t, "-i", os.path.join(d, "fenster.mp4"),
                    "-frames:v", "1", os.path.join(d, "bilder", "port_%04d.png" % (n + 100))], check=True)
print("ok", len(ns))
