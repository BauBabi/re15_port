"""Bild des ECHTEN Spielfensters einer bestimmten PID (PrintWindow PW_RENDERFULLCONTENT) - kein SDL-Readback,
kein Software-Renderer, keine Verwechslung mit Fenstern paralleler Agenten (gleicher Titel)."""
import ctypes, sys
from ctypes import wintypes
from PIL import Image

user32 = ctypes.windll.user32; gdi32 = ctypes.windll.gdi32
pid_ziel = int(sys.argv[1]); out = sys.argv[2]
gefunden = []
@ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)
def cb(hwnd, lp):
    pid = wintypes.DWORD(); user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
    if pid.value == pid_ziel and user32.IsWindowVisible(hwnd):
        n = user32.GetWindowTextLengthW(hwnd); b = ctypes.create_unicode_buffer(n + 1); user32.GetWindowTextW(hwnd, b, n + 1)
        if b.value.startswith("RE1.5"): gefunden.append(hwnd)
    return True
user32.EnumWindows(cb, 0)
if not gefunden: print("kein Fenster fuer PID", pid_ziel); sys.exit(1)
hwnd = gefunden[0]
r = wintypes.RECT(); user32.GetClientRect(hwnd, ctypes.byref(r)); w, h = r.right, r.bottom
wr = wintypes.RECT(); user32.GetWindowRect(hwnd, ctypes.byref(wr)); W, H = wr.right - wr.left, wr.bottom - wr.top
hdc = user32.GetWindowDC(hwnd); mdc = gdi32.CreateCompatibleDC(hdc); bmp = gdi32.CreateCompatibleBitmap(hdc, W, H)
gdi32.SelectObject(mdc, bmp)
ok = user32.PrintWindow(hwnd, mdc, 2)
class BIH(ctypes.Structure):
    _fields_ = [("biSize", wintypes.DWORD), ("biWidth", wintypes.LONG), ("biHeight", wintypes.LONG), ("biPlanes", wintypes.WORD),
                ("biBitCount", wintypes.WORD), ("biCompression", wintypes.DWORD), ("biSizeImage", wintypes.DWORD),
                ("biXPelsPerMeter", wintypes.LONG), ("biYPelsPerMeter", wintypes.LONG), ("biClrUsed", wintypes.DWORD), ("biClrImportant", wintypes.DWORD)]
bih = BIH(); bih.biSize = ctypes.sizeof(BIH); bih.biWidth = W; bih.biHeight = -H; bih.biPlanes = 1; bih.biBitCount = 32
buf = ctypes.create_string_buffer(W * H * 4)
gdi32.GetDIBits(mdc, bmp, 0, H, buf, ctypes.byref(bih), 0)
im = Image.frombuffer("RGBA", (W, H), buf, "raw", "BGRA", 0, 1).convert("RGB")
# Client-Bereich ausschneiden (Rahmen/Titelleiste weg)
pt = wintypes.POINT(0, 0); user32.ClientToScreen(hwnd, ctypes.byref(pt))
ox, oy = pt.x - wr.left, pt.y - wr.top
im = im.crop((ox, oy, ox + w, oy + h))
im.save(out)
gdi32.DeleteObject(bmp); gdi32.DeleteDC(mdc); user32.ReleaseDC(hwnd, hdc)
print("ok", ok, out, im.size, im.convert("L").getextrema())
