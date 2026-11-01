"""wa2_lauf.py - Abnahme W2 (unveraenderte Kopie von abnahme1/wa_lauf.py): echtes Fenster aufnehmen (Kopie des Musters W_werkzeug/w_fenster.py).
Startet eine exe-Messkopie (beschleunigter Renderer, kein AUTOSHOT, kein SOFTWARE_RENDER), sucht das Fenster ueber die
EIGENE PID, legt es oben links nach vorn, nimmt per ffmpeg gdigrab hwnd= mit 30 Bildern/s auf (ffv1), beendet nur die eigene PID.
Aufruf: wa_lauf.py <exe> <out> <aufnahme_s> --warte S [VAR=wert ...]"""
import ctypes, ctypes.wintypes as wt, os, subprocess, sys, time
FFMPEG = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
user32 = ctypes.windll.user32
def find_hwnd(pid):
    found = []
    PROC = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)
    def cb(h, _):
        p = wt.DWORD(); user32.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and user32.IsWindowVisible(h):
            r = wt.RECT(); user32.GetClientRect(h, ctypes.byref(r))
            if r.right - r.left >= 320: found.append((h, r.right - r.left, r.bottom - r.top))
        return True
    user32.EnumWindows(PROC(cb), 0)
    return found
a = sys.argv[1:]; exe, out, secs = a[0], a[1], float(a[2]); warte = 3.0; envs = []; i = 3
while i < len(a):
    if a[i] == "--warte": warte = float(a[i + 1]); i += 2
    else: envs.append(a[i]); i += 1
os.makedirs(out, exist_ok=True)
env = dict(os.environ); env.update({"RE15_NO_INTRO": "1", "SDL_ASSERT": "always_ignore", "RE15_WINDOW_SCALE": "1", "RE15_NOAUDIO": "1"})
for kv in envs:
    k, v = kv.split("=", 1); env[k] = v
for k in ("RE15_AUTOSHOT", "RE15_SOFTWARE_RENDER"): env.pop(k, None)
t_start = time.time()
p = subprocess.Popen([exe], cwd=out, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
log = open(os.path.join(out, "capture.txt"), "w"); rc = 1
try:
    hw = []
    while time.time() - t_start < 20 and not hw:
        time.sleep(0.2); hw = find_hwnd(p.pid)
    if not hw: print("kein Fenster der PID %d" % p.pid); sys.exit(2)
    h, w, hh = hw[0]
    user32.SetWindowPos(wt.HWND(h), wt.HWND(-1), 0, 0, 0, 0, 0x0001 | 0x0040)
    user32.SetForegroundWindow(wt.HWND(h))
    t_win = time.time() - t_start
    time.sleep(warte)
    t_cap = time.time() - t_start
    mkv = os.path.join(out, "capture.mkv")
    r = subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", "-f", "gdigrab", "-draw_mouse", "0", "-framerate", "30",
                        "-i", "hwnd=%d" % h, "-t", str(secs), "-c:v", "ffv1", mkv], capture_output=True, text=True)
    msg = "exe=%s pid=%d hwnd=%d client=%dx%d fenster_nach=%.1fs aufnahme_ab=%.1fs dauer=%.1fs ffmpeg_rc=%d %s env=%s" % (
        exe, p.pid, h, w, hh, t_win, t_cap, secs, r.returncode, r.stderr.strip()[:200], " ".join(envs))
    print(msg); log.write(msg + "\n"); rc = r.returncode
finally:
    if p.poll() is None:
        p.terminate()
        try: p.wait(10)
        except Exception: p.kill()
    log.close()
sys.exit(rc)
