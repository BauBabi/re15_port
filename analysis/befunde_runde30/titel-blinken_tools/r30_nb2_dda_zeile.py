#!/usr/bin/env python3
"""r30_nb2_dda_zeile.py - Runde 30 / Thema D, zweite Nachbesserung.

PRUEFT DIE MESSSCHIENE GEGEN DEN BILDSCHIRM: startet die echte re15_pc.exe (beschleunigter
Renderer, RE15_WINDOW_SCALE=1, RE15_TITLE_PULSE_LOG), findet das Fenster ueber die EIGENE PID
(nie ueber den Titel), nimmt den Client-Bereich per Desktop Duplication (ffmpeg ddagrab) auf
und rechnet je aufgenommenem Bild denselben FNV-1a-32 ueber (r>>3, g>>3, b>>3) der
Zeilenregion NEW GAME (x 0x20..0x11f, y 0x85..0x95) wie render_pc.c.

Ausgabe:
  - wie viele Bildschirmbilder (nach der Einblende) ein Zeilenbild zeigen, das die Messschiene
    fuer einen Pulswert protokolliert hat, und welchen;
  - die Pulsperiode AM BILDSCHIRM (Abstand der Abfaelle auf das Zeilenbild des Pulswerts 0x80);
  - Zeitversatz Bildschirm gegen Protokoll je Abfall (beide Uhren = QueryPerformanceCounter).

Aufruf: python r30_nb2_dda_zeile.py <exe> <ausgabe-verzeichnis> [sekunden=12] [fps=144] [warte=2.0]
Endet die exe ueber terminate() der EIGENEN PID (kein taskkill /IM).
"""
import ctypes, ctypes.wintypes as wt, os, re, subprocess, sys, time
import numpy as np

FFMPEG = r"C:\ProgramData\chocolatey\bin\ffmpeg.exe"
FFPROBE = r"C:\ProgramData\chocolatey\bin\ffprobe.exe"
user32 = ctypes.windll.user32
kernel32 = ctypes.windll.kernel32

def qpc_us():
    c = ctypes.c_longlong(); f = ctypes.c_longlong()
    kernel32.QueryPerformanceCounter(ctypes.byref(c)); kernel32.QueryPerformanceFrequency(ctypes.byref(f))
    return (c.value // f.value) * 1000000 + ((c.value % f.value) * 1000000) // f.value

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

def fnv(block):
    h = 2166136261
    for c in block.reshape(-1):
        h ^= int(c); h = (h * 16777619) & 0xffffffff
    return h

def main():
    exe, out = sys.argv[1], sys.argv[2]
    secs = float(sys.argv[3]) if len(sys.argv) > 3 else 12.0
    fps = int(sys.argv[4]) if len(sys.argv) > 4 else 144
    warte = float(sys.argv[5]) if len(sys.argv) > 5 else 2.0
    os.makedirs(out, exist_ok=True)
    env = dict(os.environ)
    env.update(RE15_NO_INTRO="1", RE15_NOAUDIO="1", RE15_WINDOW_SCALE="1", RE15_TITLE_PULSE_LOG="puls.txt")
    for k in ("RE15_SOFTWARE_RENDER", "RE15_TITLE_CONFIRM_MS"): env.pop(k, None)
    p = subprocess.Popen([exe], cwd=out, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    t_start = time.time()
    try:
        hw = []
        while time.time() - t_start < 30 and not hw:
            time.sleep(0.1); hw = find_hwnd(p.pid)
        if not hw: print("kein Fenster"); sys.exit(2)
        h, w, hh = hw[0]
        user32.SetWindowPos(wt.HWND(h), wt.HWND(-1), 0, 0, 0, 0, 0x0001 | 0x0040)   # TOPMOST, zeigen
        user32.SetForegroundWindow(wt.HWND(h))
        time.sleep(warte)
        pt = wt.POINT(0, 0); user32.ClientToScreen(wt.HWND(h), ctypes.byref(pt))
        print("pid=%d hwnd=%d client=%dx%d @%d,%d" % (p.pid, h, w, hh, pt.x, pt.y))
        mkv = os.path.join(out, "cap.mkv")
        src = ("ddagrab=output_idx=0:framerate=%d:dup_frames=0:offset_x=%d:offset_y=%d:video_size=%dx%d,"
               "hwdownload,format=bgra" % (fps, pt.x, pt.y, w, hh))
        t0 = qpc_us()
        r = subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-y", "-f", "lavfi", "-i", src,
                            "-t", str(secs), "-c:v", "ffv1", "-fps_mode", "passthrough", mkv],
                           capture_output=True, text=True)
        print("ffmpeg rc", r.returncode, r.stderr.strip()[:300])
    finally:
        p.terminate(); p.wait()
    if (w, hh) != (320, 240):
        print("Client %dx%d statt 320x240 - Massstab 1 nicht erreicht, Auswertung abgebrochen" % (w, hh)); sys.exit(3)

    si = subprocess.run([FFMPEG, "-hide_banner", "-i", mkv, "-vf", "showinfo", "-f", "null", "-"],
                        capture_output=True, text=True)
    pts = [float(m) for m in re.findall(r"pts_time:([0-9.]+)", si.stderr)]
    raw = subprocess.run([FFMPEG, "-hide_banner", "-loglevel", "error", "-i", mkv, "-fps_mode", "passthrough",
                          "-f", "rawvideo", "-pix_fmt", "rgb24", "-"], capture_output=True).stdout
    fr = np.frombuffer(raw, np.uint8).reshape(-1, 240, 320, 3)
    n = min(len(pts), len(fr))
    # Protokoll: Zeilenbild -> Pulswert (nur gezeigte Bilder)
    bild2val = {}; log = []
    for ln in open(os.path.join(out, "puls.txt")):
        f = ln.split()
        if len(f) != 12 or f[7] != "1": continue
        t, val, b, hsh = int(f[0]), int(f[2]), int(f[6]), int(f[10], 16)
        bild2val.setdefault(hsh, set()).add(val)
        log.append((t, val, b, hsh))
    t_b0 = next((t for t, val, b, hsh in log if b == 0), None)
    # Bildschirm
    reihe = []
    for i in range(n):
        blk = (fr[i, 0x85:0x85 + 17, 0x20:0x120, :] >> 3).astype(np.uint8)
        reihe.append((t0 + int(pts[i] * 1e6), fnv(blk)))
    nach = [(t, hsh) for t, hsh in reihe if t_b0 is not None and t > t_b0 + 20000]
    bekannt = sum(1 for t, hsh in nach if hsh in bild2val)
    print("Protokoll: %d gezeigte Bilder, %d Zeilenbilder; Einblende fertig bei t=%s" % (len(log), len(bild2val), t_b0))
    print("Bildschirm: %d Bilder aufgenommen, davon %d nach der Einblende; Zeilenbild im Protokoll: %d von %d"
          % (n, len(nach), bekannt, len(nach)))
    eindeutig = sum(1 for t, hsh in nach if len(bild2val.get(hsh, ())) == 1)
    werte = sorted({min(bild2val[hsh]) for t, hsh in nach if hsh in bild2val})
    print("  davon einem Pulswert zugeordnet: %d; gesehene Pulsstufen: %d" % (eindeutig, len(werte)))
    h80 = [hsh for hsh, vs in bild2val.items() if 0x80 in vs]
    if not h80: print("kein Zeilenbild fuer 0x80 im Protokoll"); return
    h80 = h80[0]
    ev_s = [c[0] for a, c in zip(nach, nach[1:]) if c[1] == h80 and a[1] != h80]
    ev_l = [c[0] for a, c in zip(log, log[1:]) if c[3] == h80 and a[3] != h80]
    print("Abfaelle auf das Zeilenbild 0x80: Bildschirm %d, Protokoll %d" % (len(ev_s), len(ev_l)))
    for a, b in zip(ev_s, ev_s[1:]):
        print("  Periode am Bildschirm: %.1f ms" % ((b - a) / 1000.0))
    for s in ev_s:
        l = max((x for x in ev_l if x <= s), default=None)
        if l is not None: print("  Abfall Bildschirm t=%d, Protokoll t=%d, Versatz %.1f ms" % (s, l, (s - l) / 1000.0))

if __name__ == "__main__":
    main()
