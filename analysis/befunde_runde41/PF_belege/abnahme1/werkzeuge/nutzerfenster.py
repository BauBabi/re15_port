"""Nur lesend: Fenster der Nutzer-Instanz (PID) finden, Groesse ausgeben, ein gdigrab-Bild. Keine Eingabe, kein Fokus."""
import ctypes, subprocess, sys
from ctypes import wintypes
u = ctypes.WinDLL('user32')
P = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
pid = int(sys.argv[1]); out = sys.argv[2]; gef = []
def cb(h, l):
    p = wintypes.DWORD(); u.GetWindowThreadProcessId(h, ctypes.byref(p))
    if p.value == pid and u.IsWindowVisible(h):
        n = u.GetWindowTextLengthW(h); b = ctypes.create_unicode_buffer(n + 1); u.GetWindowTextW(h, b, n + 1)
        if b.value.startswith('RE1.5'): gef.append(h)
    return True
u.EnumWindows(P(cb), 0)
h = gef[0]
r = wintypes.RECT(); u.GetClientRect(h, ctypes.byref(r)); w = wintypes.RECT(); u.GetWindowRect(h, ctypes.byref(w))
print('hwnd', h, 'Client %dx%d' % (r.right - r.left, r.bottom - r.top), 'Fenster', (w.left, w.top, w.right, w.bottom), 'maximiert', u.IsZoomed(h), 'minimiert', u.IsIconic(h))
subprocess.run([r'C:/ProgramData/chocolatey/bin/ffmpeg.exe', '-hide_banner', '-loglevel', 'error', '-f', 'gdigrab', '-draw_mouse', '0', '-framerate', '1', '-i', 'hwnd=%d' % h, '-frames:v', '1', '-y', out])
