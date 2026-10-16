"""re2_baby_run.py — startet PCSX-Redux mit re2leon.cue und re2_baby.lua (ueber den sichtbaren Lader
analysis/befunde_runde35/H_raeume/re2_mess/re2_loader.lua), wartet auf '# fertig' im Log und beendet danach NUR
den eigenen Emulator-Prozess (PID-Differenz). Danach werden die Bildschirm-Rohdaten in PNG gewandelt.
Aufruf: python re2_baby_run.py <ausgabeordner> [R2_X R2_Z R2_YAW [R2_CUT [R2_MAXF [R2_TOD]]]]
Runde 38 Spur R (Muster re2_run.py, Runde 35 Spur H)."""
import os, subprocess, sys, time, glob

EXE = os.getenv("PCSX_EXE", r"C:\Users\mjoedicke\AppData\Local\Microsoft\WinGet\Packages"
                r"\GrumpyCoders.PCSX-Redux_Microsoft.Winget.Source_8wekyb3d8bbwe\pcsx-redux.exe")
BIOS = os.getenv("PCSX_BIOS", r"C:\tmp\scph1001.bin")
ISO = os.getenv("PCSX_ISO", r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue")
HERE = os.path.dirname(os.path.abspath(__file__))
LOADER = os.path.join(HERE, "..", "..", "befunde_runde35", "H_raeume", "re2_mess", "re2_loader.lua")
LUA = os.path.join(HERE, "re2_baby.lua")
MAXS = int(os.getenv("PCSX_MAX_SECS", "900"))


def pids():
    out = subprocess.run(["tasklist", "/NH", "/FI", "IMAGENAME eq pcsx-redux.main"],
                         capture_output=True, text=True, encoding="cp850", errors="replace").stdout
    s = set()
    for line in out.splitlines():
        p = line.split()
        if len(p) > 1 and p[0].lower().startswith("pcsx-redux"):
            s.add(p[1])
    return s


def raw_to_png(path):
    try:
        from PIL import Image
    except ImportError:
        return
    with open(path, "rb") as f:
        head = f.readline().decode().split()
        data = f.read()
    w, h, bpp = int(head[0]), int(head[1]), int(head[2])
    if bpp == 24 or len(data) >= w * h * 3:
        im = Image.frombytes("RGB", (w, h), data[:w * h * 3])
    else:
        px = bytearray()
        for i in range(0, w * h * 2, 2):
            v = data[i] | (data[i + 1] << 8)
            r, g, b = (v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3
            px += bytes((r, g, b))
        im = Image.frombytes("RGB", (w, h), bytes(px))
    im.save(path[:-4] + ".png")


out = os.path.abspath(sys.argv[1])
os.makedirs(out, exist_ok=True)
env = dict(os.environ)
env["R2_OUT"] = out.replace("\\", "/")
env["R2_LUA"] = os.path.abspath(LUA).replace("\\", "/")
for n, v in zip(["R2_X", "R2_Z", "R2_YAW", "R2_CUT", "R2_MAXF", "R2_TOD"], sys.argv[2:]):
    env[n] = v
logp = os.path.join(out, "log.txt")
if os.path.exists(logp):
    os.remove(logp)
vorher = pids()
proc = subprocess.Popen([EXE, "-bios", BIOS, "-iso", ISO, *(os.getenv("PCSX_CPU", "-interpreter").split()), "-run",
                         "-dofile", os.path.abspath(LOADER).replace("\\", "/")],
                        env=env, cwd=out, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
t0 = time.time()
time.sleep(8)
eigene = pids() - vorher
print("pcsx-redux gestartet, eigene PIDs", eigene, flush=True)
fertig = False
while time.time() - t0 < MAXS:
    time.sleep(5)
    try:
        with open(logp, encoding="utf-8", errors="replace") as f:
            txt = f.read()
        if "# fertig" in txt:
            fertig = True
            break
    except FileNotFoundError:
        pass
print("fertig" if fertig else "ZEITLIMIT", "nach %.0f s" % (time.time() - t0), flush=True)
time.sleep(6)
for p in eigene | (pids() - vorher):
    subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
try:
    proc.terminate()
except Exception:
    pass
for r in sorted(glob.glob(os.path.join(out, "shot_*.raw"))):
    raw_to_png(r)
