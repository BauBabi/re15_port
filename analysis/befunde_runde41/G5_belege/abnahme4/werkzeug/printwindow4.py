# Abnahme 4: echtes Fenster per PrintWindow(hwnd, PW_RENDERFULLCONTENT) abgreifen (nur DIESES Fenster, kein Desktop), alle ~0,1 s ein Bild
# (480x360) nach <ziel>/w_<ms>.png. gdigrab auf das hwnd scheiterte (Fehler 8, Fenster verdeckt).
import ctypes, ctypes.wintypes as wt, sys, time, os
from PIL import Image
hwnd, ziel, dauer = int(sys.argv[1], 0), sys.argv[2], float(sys.argv[3])
os.makedirs(ziel, exist_ok=True)
u, g = ctypes.windll.user32, ctypes.windll.gdi32
u.SetProcessDPIAware()
class BMI(ctypes.Structure):
    _fields_ = [("biSize", wt.DWORD), ("biWidth", wt.LONG), ("biHeight", wt.LONG), ("biPlanes", wt.WORD), ("biBitCount", wt.WORD),
                ("biCompression", wt.DWORD), ("biSizeImage", wt.DWORD), ("biXPelsPerMeter", wt.LONG), ("biYPelsPerMeter", wt.LONG),
                ("biClrUsed", wt.DWORD), ("biClrImportant", wt.DWORD)]
t0 = time.time(); n = 0; fehl = 0
while time.time() - t0 < dauer:
    if not u.IsWindow(hwnd): break
    r = wt.RECT(); u.GetClientRect(hwnd, ctypes.byref(r)); w, h = r.right, r.bottom
    if w <= 0 or h <= 0: time.sleep(0.1); continue
    hdc = u.GetDC(hwnd); mdc = g.CreateCompatibleDC(hdc); bmp = g.CreateCompatibleBitmap(hdc, w, h); g.SelectObject(mdc, bmp)
    ok = u.PrintWindow(hwnd, mdc, 3)   # PW_CLIENTONLY | PW_RENDERFULLCONTENT
    bi = BMI(); bi.biSize = ctypes.sizeof(BMI); bi.biWidth = w; bi.biHeight = -h; bi.biPlanes = 1; bi.biBitCount = 32
    buf = ctypes.create_string_buffer(w * h * 4); g.GetDIBits(mdc, bmp, 0, h, buf, ctypes.byref(bi), 0)
    g.DeleteObject(bmp); g.DeleteDC(mdc); u.ReleaseDC(hwnd, hdc)
    if ok:
        Image.frombuffer("RGBA", (w, h), buf, "raw", "BGRA", 0, 1).convert("RGB").resize((480, 360)).save(os.path.join(ziel, "w_%07d.png" % int((time.time() - t0) * 1000)))
        n += 1
    else: fehl += 1
    time.sleep(0.08)
print("PrintWindow: %d Bilder, %d Fehlschlaege, Client %dx%d" % (n, fehl, w, h))
