"""re2_run.py — startet PCSX-Redux mit re2leon.cue und re2_arm_grab.lua, wartet auf '# fertig' im Log,
beendet danach NUR den eigenen Emulator-Prozess (pcsx-redux.main, per PID-Differenz).
Aufruf: python re2_run.py <ausgabeordner> [R2_X R2_Z R2_YAW [R2_CUT [R2_MAXF]]]
Runde 35 Spur H, Nachbesserung 3."""
import os, subprocess, sys, time

EXE = os.getenv("PCSX_EXE", r"C:\Users\mjoedicke\AppData\Local\Microsoft\WinGet\Packages"
                r"\GrumpyCoders.PCSX-Redux_Microsoft.Winget.Source_8wekyb3d8bbwe\pcsx-redux.exe")
BIOS = os.getenv("PCSX_BIOS", r"C:\tmp\scph1001.bin")
ISO = os.getenv("PCSX_ISO", r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue")
HERE = os.path.dirname(os.path.abspath(__file__))
LUA = os.path.join(HERE, "re2_loader.lua")
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


out = os.path.abspath(sys.argv[1])
os.makedirs(out, exist_ok=True)
env = dict(os.environ)
env["R2_OUT"] = out.replace("\\", "/")
env["R2_LUA"] = os.path.join(HERE, "re2_arm_grab.lua").replace("\\", "/")
names = ["R2_X", "R2_Z", "R2_YAW", "R2_CUT", "R2_MAXF"]
for n, v in zip(names, sys.argv[2:]):
    env[n] = v
logp = os.path.join(out, "log.txt")
if os.path.exists(logp):
    os.remove(logp)
vorher = pids()
proc = subprocess.Popen([EXE, "-bios", BIOS, "-iso", ISO, *(os.getenv("PCSX_CPU", "-interpreter").split()), "-run", "-dofile", LUA.replace("\\", "/")],
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
