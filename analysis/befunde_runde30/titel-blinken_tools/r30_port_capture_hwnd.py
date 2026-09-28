#!/usr/bin/env python3
"""r30_port_capture_hwnd.py - Runde 30 / Thema D, ABNAHME am echten Fenster.

Startet die UNVERAENDERTE re15_pc.exe (beschleunigter Renderer, kein AUTOSHOT, kein
SOFTWARE_RENDER), sucht das Fenster ueber die EIGENE PID (EnumWindows +
GetWindowThreadProcessId - nicht ueber den Fenstertitel: parallel laufende Agenten haben
Fenster mit demselben Titel), schiebt es in die linke obere Bildschirmecke, holt es nach
vorn und nimmt es per ffmpeg gdigrab ueber sein FENSTER-HANDLE auf. Beendet wird nur die
eigene PID.

Aufruf:
  r30_port_capture_hwnd.py <exe> <ausgabe-verzeichnis> <sekunden> [--fps N] [--warte S]
                           [--scale K] [VAR=wert ...]
Ergebnis im Ausgabe-Verzeichnis:
  capture.mkv          verlustfrei (ffv1)
  frames/f_00001.png   jedes aufgenommene Bild
  capture.txt          pid, hwnd, Fenstergroesse, ffmpeg-Rueckgabe
"""
import ctypes, ctypes.wintypes as wt, os, subprocess, sys, time

FFMPEG = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
user32 = ctypes.windll.user32

def find_hwnd(pid):
    found = []
    PROC = ctypes.WINFUNCTYPE(wt.BOOL, wt.HWND, wt.LPARAM)
    def cb(h, _):
        p = wt.DWORD()
        user32.GetWindowThreadProcessId(h, ctypes.byref(p))
        if p.value == pid and user32.IsWindowVisible(h):
            r = wt.RECT(); user32.GetClientRect(h, ctypes.byref(r))
            if r.right - r.left >= 320:
                found.append((h, r.right - r.left, r.bottom - r.top))
        return True
    user32.EnumWindows(PROC(cb), 0)
    return found

def main():
    a = sys.argv[1:]
    exe, out, secs = a[0], a[1], float(a[2])
    fps, warte, scale, envs = 144, 3.0, 1, []
    i = 3
    while i < len(a):
        if a[i] == "--fps": fps = int(a[i + 1]); i += 2
        elif a[i] == "--warte": warte = float(a[i + 1]); i += 2
        elif a[i] == "--scale": scale = int(a[i + 1]); i += 2
        else: envs.append(a[i]); i += 1
    os.makedirs(os.path.join(out, "frames"), exist_ok=True)
    for f in os.listdir(os.path.join(out, "frames")):
        os.remove(os.path.join(out, "frames", f))
    env = dict(os.environ)
    env["RE15_NO_INTRO"] = "1"
    env["RE15_WINDOW_SCALE"] = str(scale)
    for kv in envs:
        k, v = kv.split("=", 1); env[k] = v
    p = subprocess.Popen([exe], cwd=out, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    log = open(os.path.join(out, "capture.txt"), "w")
    rc = 1
    try:
        hw = []
        t0 = time.time()
        while time.time() - t0 < 15 and not hw:
            time.sleep(0.25)
            hw = find_hwnd(p.pid)
        if not hw:
            print("kein Fenster der PID %d gefunden" % p.pid); return 2
        h, w, hh = hw[0]
        SWP_NOSIZE, SWP_NOZORDER, SWP_SHOWWINDOW = 0x0001, 0x0004, 0x0040
        user32.SetWindowPos(wt.HWND(h), wt.HWND(-1), 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW)   # HWND_TOPMOST
        user32.SetForegroundWindow(wt.HWND(h))
        msg = "pid=%d hwnd=%d client=%dx%d scale=%d" % (p.pid, h, w, hh, scale)
        print(msg); log.write(msg + "\n")
        time.sleep(warte)
        mkv = os.path.join(out, "capture.mkv")
        r = subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", "-f", "gdigrab", "-draw_mouse", "0",
                            "-framerate", str(fps), "-i", "hwnd=%d" % h, "-t", str(secs),
                            "-c:v", "ffv1", "-fps_mode", "passthrough", mkv], capture_output=True, text=True)
        log.write("ffmpeg aufnahme rc=%d %s\n" % (r.returncode, r.stderr.strip()))
        if r.returncode != 0:
            print("ffmpeg:", r.stderr.strip()); return 2
        r2 = subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", "-i", mkv, "-fps_mode", "passthrough",
                             "-frame_pts", "0", os.path.join(out, "frames", "f_%05d.png")], capture_output=True, text=True)
        log.write("ffmpeg bilder rc=%d %s\n" % (r2.returncode, r2.stderr.strip()))
        r3 = subprocess.run([FFMPEG, "-hide_banner", "-i", mkv, "-map", "0:v", "-c", "copy", "-f", "null", "-"],
                            capture_output=True, text=True)
        n = len(os.listdir(os.path.join(out, "frames")))
        print("Bilder: %d in %.1f s" % (n, secs)); log.write("bilder=%d sekunden=%.1f\n" % (n, secs))
        rc = 0 if (r2.returncode == 0 and n > 0) else 2
    finally:
        p.terminate(); p.wait()
        log.close()
    return rc

if __name__ == "__main__":
    sys.exit(main())
